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

#అన్తర్భావ "vim.h"

రకనిర్వచనం నిర్మాణం ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    దీర్ఘ	uc_def;		// The default value for a range/count
    పూర్ణాంకం		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    పూర్ణాంకం		uc_flags;	// some UC_ flags
    పూర్ణాంకం		uc_compl_opt;	// completion options (UCC_ flags)
#నిర్వచితమైతే FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#అయితే_ముగింపు
} ucmd_T;

// List of all user commands.
స్థిర garray_T ucmds = {0, 0, పరిమాణం(ucmd_T), 4, ఖాళీ};

// When non-zero it is not allowed to add or remove user commands
స్థిర పూర్ణాంకం ucmd_locked = 0;

#నిర్వచించు USER_CMD(i) (&((ucmd_T *)(ucmds.ga_దత్తాంశం))[i])
#నిర్వచించు USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_దత్తాంశం))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
స్థిర keyvalue_T command_complete_tab[] =
{
    KEYVALUE_నమోదు(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_నమోదు(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_నమోదు(EXPAND_BEHAVE, "behave"),
#ఒకవేళ defined(FEAT_EVAL)
    KEYVALUE_నమోదు(EXPAND_BREAKPOINT, "breakpoint"),
#అయితే_ముగింపు
    KEYVALUE_నమోదు(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_నమోదు(EXPAND_COLORS, "color"),
    KEYVALUE_నమోదు(EXPAND_COMMANDS, "command"),
    KEYVALUE_నమోదు(EXPAND_COMPILER, "compiler"),
#ఒకవేళ defined(FEAT_CSCOPE)
    KEYVALUE_నమోదు(EXPAND_CSCOPE, "cscope"),
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL)
    KEYVALUE_నమోదు(EXPAND_USER_DEFINED, "custom"),
    KEYVALUE_నమోదు(EXPAND_USER_LIST, "customlist"),
#అయితే_ముగింపు
    KEYVALUE_నమోదు(EXPAND_DIFF_BUFFERS, "diff_buffer"),
    KEYVALUE_నమోదు(EXPAND_DIRECTORIES, "dir"),
    KEYVALUE_నమోదు(EXPAND_DIRS_లోపల_CDPATH, "dir_in_path"),
    KEYVALUE_నమోదు(EXPAND_ENV_VARS, "environment"),
    KEYVALUE_నమోదు(EXPAND_EVENTS, "event"),
    KEYVALUE_నమోదు(EXPAND_వ్యక్తీకరణ, "expression"),
    KEYVALUE_నమోదు(EXPAND_FILES, "file"),
    KEYVALUE_నమోదు(EXPAND_FILES_లోపల_మార్గం, "file_in_path"),
    KEYVALUE_నమోదు(EXPAND_FILETYPE, "filetype"),
    KEYVALUE_నమోదు(EXPAND_FILETYPECMD, "filetypecmd"),
    KEYVALUE_నమోదు(EXPAND_FUNCTIONS, "function"),
    KEYVALUE_నమోదు(EXPAND_HELP, "help"),
    KEYVALUE_నమోదు(EXPAND_HIGHLIGHT, "highlight"),
    KEYVALUE_నమోదు(EXPAND_HISTORY, "history"),
#ఒకవేళ defined(FEAT_KEYMAP)
    KEYVALUE_నమోదు(EXPAND_KEYMAP, "keymap"),
#అయితే_ముగింపు
#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    KEYVALUE_నమోదు(EXPAND_LOCALES, "locale"),
#అయితే_ముగింపు
    KEYVALUE_నమోదు(EXPAND_MAPCLEAR, "mapclear"),
    KEYVALUE_నమోదు(EXPAND_MAPPINGS, "mapping"),
    KEYVALUE_నమోదు(EXPAND_MENUS, "menu"),
    KEYVALUE_నమోదు(EXPAND_MESSAGES, "messages"),
    KEYVALUE_నమోదు(EXPAND_SETTINGS, "option"),
    KEYVALUE_నమోదు(EXPAND_PACKADD, "packadd"),
    KEYVALUE_నమోదు(EXPAND_RETAB, "retab"),
    KEYVALUE_నమోదు(EXPAND_RUNTIME, "runtime"),
#ఒకవేళ defined(FEAT_EVAL)
    KEYVALUE_నమోదు(EXPAND_SCRIPTNAMES, "scriptnames"),
#అయితే_ముగింపు
    KEYVALUE_నమోదు(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_నమోదు(EXPAND_SHELLCMDLINE, "shellcmdline"),
#ఒకవేళ defined(FEAT_SIGNS)
    KEYVALUE_నమోదు(EXPAND_SIGN, "sign"),
#అయితే_ముగింపు
    KEYVALUE_నమోదు(EXPAND_OWNSYNTAX, "syntax"),
#ఒకవేళ defined(FEAT_PROFILE)
    KEYVALUE_నమోదు(EXPAND_SYNTIME, "syntime"),
#అయితే_ముగింపు
    KEYVALUE_నమోదు(EXPAND_ట్యాగులు, "tag"),
    KEYVALUE_నమోదు(EXPAND_ట్యాగులు_LISTFILES, "tag_listfiles"),
    KEYVALUE_నమోదు(EXPAND_USER, "user"),
    KEYVALUE_నమోదు(EXPAND_USER_VARS, "var")
};

రకనిర్వచనం నిర్మాణం
{
    cmd_addr_T కీలకం;
    అక్షరం *fullname;
    size_t fullnamelen;
    అక్షరం *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#నిర్వచించు ADDRTYPE_నమోదు(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
స్థిర addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_నమోదు(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_నమోదు(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_నమోదు(ADDR_పంక్తులు, "lines", "line"),
    ADDRTYPE_నమోదు(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_నమోదు(ADDR_OTHER, "other", "?"),
    ADDRTYPE_నమోదు(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_నమోదు(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_నమోదు(ADDR_WINDOWS, "windows", "win")
};

స్థిర పూర్ణాంకం cmp_addr_type(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b);

/*
 * Search for a user command that matches "eap->cmd".
 * Return cmdidx in "eap->cmdidx", flags in "eap->argt", idx in "eap->useridx".
 * Return a pointer to just after the command.
 * Return NULL if there is no matching command.
 */
    char_u *
కనుగొను_ucmd(
    exarg_T	*eap,
    char_u	*p,	 // end of the command (possibly including count)
    పూర్ణాంకం		*నిండిన,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    పూర్ణాంకం		*complp) // completion flags or NULL
{
    పూర్ణాంకం		len = (పూర్ణాంకం)(p - eap->cmd);
    పూర్ణాంకం		j, k, matchlen = 0;
    ucmd_T	*uc;
    పూర్ణాంకం		దొరికింది = FALSE;
    పూర్ణాంకం		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    పూర్ణాంకం		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_బఫరు->b_ucmds;
    కొరకు (;;)
    {
	కొరకు (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    అయితే (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    ఒకవేళ (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		ఒకవేళ (k == len && దొరికింది && *np != NUL)
		{
		    ఒకవేళ (gap == &ucmds)
			తిరిగి ఖాళీ;
		    amb_local = TRUE;
		}

		ఒకవేళ (!దొరికింది || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    ఒకవేళ (k == len)
			దొరికింది = TRUE;
		    లేకపోతే
			possible = TRUE;

		    ఒకవేళ (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    లేకపోతే
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (దీర్ఘ)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    ఒకవేళ (complp != ఖాళీ)
			*complp = uc->uc_compl;
#నిర్వచితమైతే FEAT_EVAL
		    ఒకవేళ (xp != ఖాళీ)
		    {
			xp->xp_arg = uc->uc_compl_arg;
			xp->xp_complete_opt = uc->uc_compl_opt;
			xp->xp_script_ctx = uc->uc_script_ctx;
			xp->xp_script_ctx.sc_lnum += SOURCING_LNUM;
		    }
#అయితే_ముగింపు
		    // Do not search for further abbreviations
		    // if this is an exact match.
		    matchlen = k;
		    ఒకవేళ (k == len && *np == NUL)
		    {
			ఒకవేళ (నిండిన != ఖాళీ)
			    *నిండిన = TRUE;
			amb_local = FALSE;
			ఆపు;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	ఒకవేళ (j < gap->ga_len || gap == &ucmds)
	    ఆపు;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    ఒకవేళ (amb_local)
    {
	ఒకవేళ (xp != ఖాళీ)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	తిరిగి ఖాళీ;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    ఒకవేళ (దొరికింది || possible)
	తిరిగి p + (matchlen - len);
    తిరిగి p;
}

/*
 * Set completion context for :command
 */
    char_u *
సమితి_context_లోపల_user_cmd(expand_T *xp, char_u *arg_లోపల)
{
    char_u	*arg = arg_లోపల;
    char_u	*p;

    // Check for attributes
    అయితే (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	ఒకవేళ (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    ఒకవేళ (p == ఖాళీ)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_నమూనా = arg;
		తిరిగి ఖాళీ;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    ఒకవేళ (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_నమూనా = p + 1;
	    }
	    లేకపోతే ఒకవేళ (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_నమూనా = p + 1;
		కొరకు (char_u *c = p + 1; *c != NUL; ++c)
		    ఒకవేళ (*c == ',')
			xp->xp_నమూనా = c + 1;
	    }
	    లేకపోతే ఒకవేళ (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_నమూనా = p + 1;
	    }
	    లేకపోతే ఒకవేళ (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_నమూనా = p + 1;
	    }
	    తిరిగి ఖాళీ;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    ఒకవేళ (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_నమూనా = arg;
	తిరిగి ఖాళీ;
    }

    // And finally comes a normal command
    తిరిగి skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
సమితి_context_లోపల_user_cmdarg(
	char_u		*cmd వాడని,
	char_u		*arg,
	దీర్ఘ		argt,
	పూర్ణాంకం		context,
	expand_T	*xp,
	పూర్ణాంకం		forceit)
{
    char_u	*p;

    ఒకవేళ (context == EXPAND_NOTHING)
	తిరిగి ఖాళీ;

    ఒకవేళ (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	తిరిగి ఖాళీ;
    }

#నిర్వచితమైతే FEAT_MENU
    ఒకవేళ (context == EXPAND_MENUS)
	తిరిగి సమితి_context_లోపల_menu_cmd(xp, cmd, arg, forceit);
#అయితే_ముగింపు
    ఒకవేళ (context == EXPAND_COMMANDS)
	తిరిగి arg;
    ఒకవేళ (context == EXPAND_MAPPINGS)
	తిరిగి సమితి_context_లోపల_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    ఒకవేళ (!(argt & EX_ARGSPACE))
    {
	p = arg;
	అయితే (*p)
	{
	    ఒకవేళ (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    లేకపోతే ఒకవేళ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_నమూనా = arg;
    xp->xp_context = context;

    తిరిగి ఖాళీ;
}

    char_u *
expand_user_command_name(పూర్ణాంకం idx)
{
    తిరిగి get_user_commands(ఖాళీ, idx - (పూర్ణాంకం)CMD_కొలత);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_బఫరు;

    ఒకవేళ (idx < buf->b_ucmds.ga_len)
	తిరిగి USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    ఒకవేళ (idx < ucmds.ga_len)
    {
	పూర్ణాంకం	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	కొరకు (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    ఒకవేళ (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		తిరిగి (char_u *)"";
	తిరిగి name;
    }
    తిరిగి ఖాళీ;
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(పూర్ణాంకం idx, పూర్ణాంకం cmdidx)
{
    ఒకవేళ (cmdidx == CMD_USER && idx < ucmds.ga_len)
	తిరిగి USER_CMD(idx)->uc_name;
    ఒకవేళ (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_బఫరు;

	ఒకవేళ (idx < buf->b_ucmds.ga_len)
	    తిరిగి USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    తిరిగి ఖాళీ;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    పూర్ణాంకం
user_command_info(పూర్ణాంకం idx, పూర్ణాంకం cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_బఫరు->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    ఒకవేళ (idx >= gap->ga_len)
	తిరిగి FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_కూడు_string(d, "name", uc->uc_name);
    dict_కూడు_string(d, "kind", (char_u *)"user");
    dict_కూడు_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_కూడు_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_గమ్యం_str(uc->uc_compl, uc->uc_compl_arg);
    dict_కూడు_string(d, "complete", compl == ఖాళీ ? (char_u *)"" : compl);
    vim_free(compl);
    dict_కూడు_string(d, "definition", uc->uc_rep);
    dict_కూడు_సంఖ్య(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_కూడు_సంఖ్య(d, "lnum", uc->uc_script_ctx.sc_lnum);
    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx < 0 || idx >= (పూర్ణాంకం)ARRAY_నిడివి(addr_type_complete_tab))
	తిరిగి ఖాళీ;
    తిరిగి (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    స్థిర అక్షరం *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    ఒకవేళ (idx < 0 || idx >= (పూర్ణాంకం)ARRAY_నిడివి(user_cmd_flags))
	తిరిగి ఖాళీ;
    తిరిగి (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    స్థిర అక్షరం *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    ఒకవేళ (idx < 0 || idx >= (పూర్ణాంకం)ARRAY_నిడివి(user_cmd_nargs))
	తిరిగి ఖాళీ;
    తిరిగి (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx < 0 || idx >= (పూర్ణాంకం)ARRAY_నిడివి(command_complete_tab))
	తిరిగి ఖాళీ;
    తిరిగి command_complete_tab[idx].విలువ.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
స్థిర అక్షరం *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx < 0 || idx >= (పూర్ణాంకం)ARRAY_నిడివి(user_cmd_completeopt_tab))
	తిరిగి ఖాళీ;
    తిరిగి (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    స్థిర keyvalue_T *
get_commandtype(పూర్ణాంకం expand)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(command_complete_tab); ++i)
	ఒకవేళ (command_complete_tab[i].కీలకం == expand)
	    తిరిగి &command_complete_tab[i];

    తిరిగి ఖాళీ;
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_గమ్యం_str(పూర్ణాంకం expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    ఒకవేళ (kv == ఖాళీ || kv->విలువ.string == ఖాళీ)
	తిరిగి ఖాళీ;

    cmd_compl = kv->విలువ.string;
    ఒకవేళ (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*బఫరు;

	బఫరు = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	ఒకవేళ (బఫరు == ఖాళీ)
	    తిరిగి ఖాళీ;
	sprintf((అక్షరం *)బఫరు, "%s,%s", cmd_compl, compl_arg);
	తిరిగి బఫరు;
    }

    తిరిగి vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    పూర్ణాంకం
cmdcomplete_str_గమ్యం_type(char_u *complete_str)
{
    keyvalue_T లక్ష్యం;
    keyvalue_T *నమోదు_2;
    స్థిర keyvalue_T *చివరి_నమోదు = ఖాళీ;	// cached result

    ఒకవేళ (STRNCMP(complete_str, "custom,", 7) == 0)
	తిరిగి EXPAND_USER_DEFINED;
    ఒకవేళ (STRNCMP(complete_str, "customlist,", 11) == 0)
	తిరిగి EXPAND_USER_LIST;

    లక్ష్యం.కీలకం = 0;
    లక్ష్యం.విలువ.string = complete_str;
    లక్ష్యం.విలువ.నిడివి = 0;			// not used, see cmp_keyvalue_value()

    ఒకవేళ (చివరి_నమోదు != ఖాళీ && cmp_keyvalue_విలువ(&లక్ష్యం, చివరి_నమోదు) == 0)
	నమోదు_2 = చివరి_నమోదు;
    లేకపోతే
    {
	నమోదు_2 = (keyvalue_T *)bsearch(&లక్ష్యం,
	    &command_complete_tab,
	    ARRAY_నిడివి(command_complete_tab),
	    పరిమాణం(command_complete_tab[0]),
	    cmp_keyvalue_విలువ);
	ఒకవేళ (నమోదు_2 == ఖాళీ)
	    తిరిగి EXPAND_NOTHING;

	చివరి_నమోదు = నమోదు_2;
    }

    తిరిగి నమోదు_2->కీలకం;
}
#అయితే_ముగింపు

/*
 * List user commands starting with "name[name_len]".
 */
    స్థిర శూన్య
uc_list(char_u *name, size_t name_len)
{
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		దొరికింది = FALSE;
    ucmd_T	*cmd;
    పూర్ణాంకం		len;
    పూర్ణాంకం		over;
    దీర్ఘ	a;
    garray_T	*gap;
    keyvalue_T	*నమోదు_2;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_బఫరు->b_ucmds;
    కొరకు (;;)
    {
	కొరకు (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (దీర్ఘ)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    ఒకవేళ (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || సందేశం_filtered(cmd->uc_name))
		కొనసాగు;

	    // Put out the title first time
	    ఒకవేళ (!దొరికింది)
		సందేశం_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    దొరికింది = TRUE;
	    సందేశం_putchar('\n');
	    ఒకవేళ (got_int)
		ఆపు;

	    // Special cases
	    len = 4;
	    ఒకవేళ (a & EX_BANG)
	    {
		సందేశం_putchar('!');
		--len;
	    }
	    ఒకవేళ (a & EX_REGSTR)
	    {
		సందేశం_putchar('"');
		--len;
	    }
	    ఒకవేళ (gap != &ucmds)
	    {
		సందేశం_putchar('b');
		--len;
	    }
	    ఒకవేళ (a & EX_TRLBAR)
	    {
		సందేశం_putchar('|');
		--len;
	    }
	    ఒకవేళ (len != 0)
		సందేశం_puts(&"    "[4 - len]);

	    సందేశం_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (పూర్ణాంకం)cmd->uc_namelen + 4;

	    ఒకవేళ (len < 21)
	    {
		// Field padding spaces   12345678901234567
		స్థిర అక్షరం spaces[18] = "                 ";
		సందేశం_puts(&spaces[len - 4]);
		len = 21;
	    }
	    సందేశం_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    ఎంపిక ((పూర్ణాంకం)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		స్థితి 0:				IObuff[len++] = '0'; ఆపు;
		స్థితి (EX_EXTRA):		IObuff[len++] = '*'; ఆపు;
		స్థితి (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; ఆపు;
		స్థితి (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; ఆపు;
		స్థితి (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; ఆపు;
		స్థితి (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; ఆపు;
	    }

	    చేయి
	    {
		IObuff[len++] = ' ';
	    } అయితే (len < 5 - over);

	    // Address / Range
	    ఒకవేళ (a & (EX_RANGE|EX_లెక్క))
	    {
		ఒకవేళ (a & EX_లెక్క)
		{
		    // -count=N
		    len += vim_snprintf((అక్షరం *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		లేకపోతే ఒకవేళ (a & EX_DFLALL)
		    IObuff[len++] = '%';
		లేకపోతే ఒకవేళ (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((అక్షరం *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		లేకపోతే
		    IObuff[len++] = '.';
	    }

	    చేయి
	    {
		IObuff[len++] = ' ';
	    } అయితే (len < 8 - over);

	    // Address Type
	    కొరకు (j = 0; j < (పూర్ణాంకం)ARRAY_నిడివి(addr_type_complete_tab); ++j)
		ఒకవేళ (addr_type_complete_tab[j].కీలకం != ADDR_పంక్తులు
			&& addr_type_complete_tab[j].కీలకం == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (పూర్ణాంకం)addr_type_complete_tab[j].shortnamelen;
		    ఆపు;
		}

	    చేయి
	    {
		IObuff[len++] = ' ';
	    } అయితే (len < 13 - over);

	    // Completion
	    నమోదు_2 = get_commandtype(cmd->uc_compl);
	    ఒకవేళ (నమోదు_2 != ఖాళీ)
	    {
		STRCPY(IObuff + len, నమోదు_2->విలువ.string);
		len += (పూర్ణాంకం)నమోదు_2->విలువ.నిడివి;
#నిర్వచితమైతే FEAT_EVAL
		ఒకవేళ (p_verbose > 0 && cmd->uc_compl_arg != ఖాళీ)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    ఒకవేళ (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (పూర్ణాంకం)uc_compl_arglen;
		    }
		}
		ఒకవేళ (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((అక్షరం *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#అయితే_ముగింపు
	    }

	    చేయి
	    {
		IObuff[len++] = ' ';
	    } అయితే (len < 25 - over);

	    IObuff[len] = NUL;
	    సందేశం_outtrans(IObuff);

	    సందేశం_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (p_verbose > 0)
		చివరి_సమితి_సందేశం(cmd->uc_script_ctx);
#అయితే_ముగింపు
	    బహిర్గతం_flush();
	    ui_breakcheck();
	    ఒకవేళ (got_int)
		ఆపు;
	}
	ఒకవేళ (gap == &ucmds || i < gap->ga_len)
	    ఆపు;
	gap = &ucmds;
    }

    ఒకవేళ (!దొరికింది)
	సందేశం(_("No user-defined commands found"));

    --ucmd_locked;
}

    అక్షరం *
uc_fun_cmd(శూన్య)
{
    స్థిర char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    పూర్ణాంకం		i;

    కొరకు (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    తిరిగి (అక్షరం *)IObuff;
}

/*
 * Parse address type argument
 */
    స్థిర పూర్ణాంకం
parse_addr_type_arg(
    char_u	*విలువ,
    పూర్ణాంకం		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T లక్ష్యం;
    addrtype_T *నమోదు_2;
    స్థిర addrtype_T *చివరి_నమోదు;	// cached result

    లక్ష్యం.కీలకం = 0;
    లక్ష్యం.fullname = (అక్షరం *)విలువ;
    లక్ష్యం.fullnamelen = vallen;

    ఒకవేళ (చివరి_నమోదు != ఖాళీ && cmp_addr_type(&లక్ష్యం, చివరి_నమోదు) == 0)
	నమోదు_2 = చివరి_నమోదు;
    లేకపోతే
    {
	నమోదు_2 = (addrtype_T *)bsearch(&లక్ష్యం,
	    &addr_type_complete_tab,
	    ARRAY_నిడివి(addr_type_complete_tab),
	    పరిమాణం(addr_type_complete_tab[0]),
	    cmp_addr_type);
	ఒకవేళ (నమోదు_2 == ఖాళీ)
	{
	    పూర్ణాంకం i;
	    char_u	*err = విలువ;

	    కొరకు (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_చిరునామా_type_విలువ_str), err);
	    తిరిగి FAIL;
	}

	చివరి_నమోదు = నమోదు_2;
    }

    *addr_type_arg = నమోదు_2->కీలకం;

    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
cmp_addr_type(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    తిరిగి STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    పూర్ణాంకం
parse_compl_arg(
    char_u	*విలువ,
    పూర్ణాంకం		vallen,
    పూర్ణాంకం		*complp,
    దీర్ఘ	*argt,
    char_u	**compl_arg వాడని)
{
    char_u	*arg = ఖాళీ;
#ఒకవేళ defined(FEAT_EVAL)
    size_t	arglen = 0;
#అయితే_ముగింపు
    పూర్ణాంకం		i;
    పూర్ణాంకం		valend = vallen;
    keyvalue_T	లక్ష్యం;
    keyvalue_T	*నమోదు_2;
    స్థిర keyvalue_T	*చివరి_నమోదు = ఖాళీ;	    // cached result

    // Look for any argument part - which is the part after any ','
    కొరకు (i = 0; i < vallen; ++i)
    {
	ఒకవేళ (విలువ[i] == ',')
	{
	    arg = &విలువ[i + 1];
#ఒకవేళ defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#అయితే_ముగింపు
	    valend = i;
	    ఆపు;
	}
    }

    లక్ష్యం.కీలకం = 0;
    లక్ష్యం.విలువ.string = విలువ;
    లక్ష్యం.విలువ.నిడివి = valend;

    ఒకవేళ (చివరి_నమోదు != ఖాళీ && cmp_keyvalue_విలువ_n(&లక్ష్యం, చివరి_నమోదు) == 0)
	నమోదు_2 = చివరి_నమోదు;
    లేకపోతే
    {
	నమోదు_2 = (keyvalue_T *)bsearch(&లక్ష్యం,
	    &command_complete_tab,
	    ARRAY_నిడివి(command_complete_tab),
	    పరిమాణం(command_complete_tab[0]),
	    cmp_keyvalue_విలువ_n);
	ఒకవేళ (నమోదు_2 == ఖాళీ)
	{
	    semsg(_(e_invalid_complete_విలువ_str), విలువ);
	    తిరిగి FAIL;
	}

	చివరి_నమోదు = నమోదు_2;
    }

    *complp = నమోదు_2->కీలకం;
    ఒకవేళ (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    లేకపోతే ఒకవేళ (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    ఒకవేళ (
#ఒకవేళ defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#అయితే_ముగింపు
								arg != ఖాళీ)
    {
	emsg(_(e_completion_వాదం_only_allowed_for_custom_completion));
	తిరిగి FAIL;
    }

#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == ఖాళీ)
    {
	emsg(_(e_custom_completion_requires_function_వాదం));
	తిరిగి FAIL;
    }

    ఒకవేళ (arg != ఖాళీ)
	*compl_arg = vim_strnsave(arg, arglen);
#అయితే_ముగింపు

    తిరిగి OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    స్థిర పూర్ణాంకం
parse_completeopt_arg(char_u *విలువ, పూర్ణాంకం vallen, పూర్ణాంకం *compl_opt)
{
    char_u  *p = విలువ;
    char_u  *చివర = విలువ + vallen;
    పూర్ణాంకం	    flags = 0;

    ఒకవేళ (vallen == 0)
    {
	semsg(_(e_వాదం_required_for_str), "-completeopt");
	తిరిగి FAIL;
    }

    అయితే (p < చివర)
    {
	char_u	*comma;
	size_t	itemlen;
	పూర్ణాంకం	matched = FALSE;
	పూర్ణాంకం	i;

	comma = vim_strchr(p, ',');
	ఒకవేళ (comma == ఖాళీ || comma > చివర)
	    itemlen = (size_t)(చివర - p);
	లేకపోతే
	    itemlen = (size_t)(comma - p);

	ఒకవేళ (itemlen == 0)
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str), "completeopt");
	    తిరిగి FAIL;
	}

	కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(user_cmd_completeopt_tab); ++i)
	{
	    అక్షరం *name = user_cmd_completeopt_tab[i];

	    ఒకవేళ (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		ఒకవేళ (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		ఆపు;
	    }
	}
	ఒకవేళ (!matched)
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str), "completeopt");
	    తిరిగి FAIL;
	}

	p += itemlen;
	ఒకవేళ (p < చివర && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    తిరిగి OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    స్థిర పూర్ణాంకం
uc_పరిశీలన_attr(
    char_u	*attr,
    size_t	len,
    దీర్ఘ	*argt,
    దీర్ఘ	*def,
    పూర్ణాంకం		*flags,
    పూర్ణాంకం		*complp,
    char_u	**compl_arg,
    పూర్ణాంకం		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    ఒకవేళ (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	తిరిగి FAIL;
    }

    // First, try the simple attributes (no arguments)
    ఒకవేళ (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    లేకపోతే ఒకవేళ (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_బఫరు;
    లేకపోతే ఒకవేళ (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    లేకపోతే ఒకవేళ (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    లేకపోతే ఒకవేళ (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    లేకపోతే
    {
	పూర్ణాంకం	i;
	char_u	*val = ఖాళీ;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	కొరకు (i = 0; i < (పూర్ణాంకం)len; ++i)
	{
	    ఒకవేళ (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		ఆపు;
	    }
	}

	ఒకవేళ (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    ఒకవేళ (vallen == 1)
	    {
		ఒకవేళ (*val == '0')
		    // Do nothing - this is the default
		    ;
		లేకపోతే ఒకవేళ (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		లేకపోతే ఒకవేళ (*val == '*')
		    *argt |= EX_EXTRA;
		లేకపోతే ఒకవేళ (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		లేకపోతే ఒకవేళ (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		లేకపోతే ఒకవేళ (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		లేకపోతే
		    వెళ్ళు wrong_nargs;
	    }
	    లేకపోతే
	    {
wrong_nargs:
		emsg(_(e_invalid_సంఖ్య_యొక్క_arguments));
		తిరిగి FAIL;
	    }
	}
	లేకపోతే ఒకవేళ (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ఒకవేళ (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    లేకపోతే ఒకవేళ (val != ఖాళీ)
	    {
		p = val;
		ఒకవేళ (*def >= 0)
		{
two_లెక్క:
		    emsg(_(e_లెక్క_cannot_be_specified_twice));
		    తిరిగి FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		ఒకవేళ (p != val + vallen || vallen == 0)
		{
invalid_లెక్క:
		    emsg(_(e_invalid_default_విలువ_for_లెక్క));
		    తిరిగి FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    ఒకవేళ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_పంక్తులు;
	}
	లేకపోతే ఒకవేళ (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_లెక్క | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    ఒకవేళ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    ఒకవేళ (val != ఖాళీ)
	    {
		p = val;
		ఒకవేళ (*def >= 0)
		    వెళ్ళు two_లెక్క;

		*def = getdigits(&p);

		ఒకవేళ (p != val + vallen)
		    వెళ్ళు invalid_లెక్క;
	    }

	    ఒకవేళ (*def < 0)
		*def = 0;
	}
	లేకపోతే ఒకవేళ (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    ఒకవేళ (val == ఖాళీ)
	    {
		semsg(_(e_వాదం_required_for_str), "-complete");
		తిరిగి FAIL;
	    }

	    ఒకవేళ (parse_compl_arg(val, (పూర్ణాంకం)vallen, complp, argt, compl_arg)
								      == FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    ఒకవేళ (val == ఖాళీ)
	    {
		semsg(_(e_వాదం_required_for_str), "-completeopt");
		తిరిగి FAIL;
	    }

	    ఒకవేళ (parse_completeopt_arg(val, (పూర్ణాంకం)vallen, compl_opt) == FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ఒకవేళ (val == ఖాళీ)
	    {
		semsg(_(e_వాదం_required_for_str), "-addr");
		తిరిగి FAIL;
	    }
	    ఒకవేళ (parse_addr_type_arg(val, (పూర్ణాంకం)vallen, addr_type_arg) == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (*addr_type_arg != ADDR_పంక్తులు)
		*argt |= EX_ZEROR;
	}
	లేకపోతే
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    తిరిగి FAIL;
	}
    }

    తిరిగి OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    స్థిర పూర్ణాంకం
uc_కూడు_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    దీర్ఘ	argt,
    దీర్ఘ	def,
    పూర్ణాంకం		flags,
    పూర్ణాంకం		compl,
    char_u	*compl_arg వాడని,
    పూర్ణాంకం		compl_opt,
    cmd_addr_T	addr_type,
    పూర్ణాంకం		force)
{
    ucmd_T	*cmd = ఖాళీ;
    char_u	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		cmp = 1;
    char_u	*rep_buf = ఖాళీ;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, ఖాళీ);
    ఒకవేళ (rep_buf == ఖాళీ)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	ఒకవేళ (rep_buf == ఖాళీ)
	    తిరిగి FAIL;
    }

    // get address of growarray: global or in curbuf
    ఒకవేళ (flags & UC_బఫరు)
    {
	gap = &curbuf->b_ucmds;
	ఒకవేళ (gap->ga_itemsize == 0)
	    ga_init2(gap, పరిమాణం(ucmd_T), 4);
    }
    లేకపోతే
	gap = &ucmds;

    // Search for the command in the already defined commands.
    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	ఒకవేళ (cmp == 0)
	{
	    ఒకవేళ (name_len < cmd->uc_namelen)
		cmp = -1;
	    లేకపోతే ఒకవేళ (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	ఒకవేళ (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    ఒకవేళ (!force
#నిర్వచితమైతే FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#అయితే_ముగింపు
		    )
	    {
		semsg(_(e_command_already_exists_కూడు_bang_గమ్యం_replace_it_str),
									 name);
		వెళ్ళు fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#ఒకవేళ defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#అయితే_ముగింపు
	    ఆపు;
	}

	// Stop as soon as we pass the name to add
	ఒకవేళ (cmp < 0)
	    ఆపు;
    }

    // Extend the array unless we're replacing an existing command
    ఒకవేళ (cmp != 0)
    {
	ఒకవేళ (ga_grow(gap, 1) == FAIL)
	    వెళ్ళు fail;
	ఒకవేళ ((p = vim_strnsave(name, name_len)) == ఖాళీ)
	    వెళ్ళు fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * పరిమాణం(ucmd_T));

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
    ఒకవేళ (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_సంస్కరణ = SCRIPT_సంస్కరణ_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#నిర్వచితమైతే FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#అయితే_ముగింపు
    cmd->uc_addr_type = addr_type;

    తిరిగి OK;

fail:
    vim_free(rep_buf);
#ఒకవేళ defined(FEAT_EVAL)
    vim_free(compl_arg);
#అయితే_ముగింపు
    తిరిగి FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_ఖండం(exarg_T *eap, char_u *p, char_u **tofree, పూర్ణాంకం *flags)
{
    char_u *retp = p;
    char_u *ఖండం = కనుగొను_cmd_ఖండం_ప్రారంభం(p);

    ఒకవేళ (ఖండం != ఖాళీ && eap->ea_getline != ఖాళీ)
    {
	garray_T    ga;
	char_u	    *పంక్తి = ఖాళీ;

	ga_init2(&ga, పరిమాణం(char_u *), 10);
	ఒకవేళ (ga_copy_string(&ga, p) == FAIL)
	    తిరిగి retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	ఒకవేళ (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    కొరకు (;;)
	    {
		vim_free(పంక్తి);
		ఒకవేళ ((పంక్తి = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == ఖాళీ)
		{
		    emsg(_(e_missing_rcurly));
		    ఆపు;
		}
		ఒకవేళ (ga_copy_string(&ga, పంక్తి) == FAIL)
		    ఆపు;
		ఒకవేళ (*skipwhite(పంక్తి) == '}')
		    ఆపు;
	    }
	vim_free(పంక్తి);
	retp = *tofree = ga_concat_అక్షరశ్రేణులు(&ga, "\n");
	ఒకవేళ (retp == ఖాళీ)
	    retp = p;
	ga_clear_అక్షరశ్రేణులు(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	ఒకవేళ (ఖండం == p)
	    *flags |= UC_VIM9;
    }
    తిరిగి retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    శూన్య
skip_cmd_ఖండం(exarg_T *eap)
{
    char_u	*tofree = ఖాళీ;
    పూర్ణాంకం		flags = 0;

    (శూన్య)may_get_cmd_ఖండం(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    శూన్య
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*చివర;
    char_u	*p;
    దీర్ఘ	argt = 0;
    దీర్ఘ	def = -1;
    పూర్ణాంకం		flags = 0;
    పూర్ణాంకం		compl = EXPAND_NOTHING;
    char_u	*compl_arg = ఖాళీ;
    పూర్ణాంకం		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    పూర్ణాంకం		has_attr = (eap->arg[0] == '-');
    పూర్ణాంకం		name_len;

    ఒకవేళ (eap->skip)
    {
	skip_cmd_ఖండం(eap);
	తిరిగి;
    }

    p = eap->arg;

    // Check for attributes
    అయితే (*p == '-')
    {
	++p;
	చివర = skiptowhite(p);
	ఒకవేళ (uc_పరిశీలన_attr(p, చివర - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    వెళ్ళు theend;
	p = skipwhite(చివర);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    ఒకవేళ (ASCII_ISALPHA(*p))
	అయితే (ASCII_ISALNUM(*p))
	    ++p;
    ఒకవేళ (!చివర్లు_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	వెళ్ళు theend;
    }
    చివర = p;
    name_len = (పూర్ణాంకం)(చివర - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(చివర);
    ఒకవేళ (!has_attr && చివర్లు_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    లేకపోతే ఒకవేళ (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_ప్రారంభం_with_an_uppercase_letter));
    }
    లేకపోతే ఒకవేళ ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_వాడిన_for_user_defined_command));
    }
    లేకపోతే ఒకవేళ (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	ఒకవేళ (లోపల_vim9script())
	    emsg(_(e_complete_వాడిన_without_allowing_arguments));
	లేకపోతే
	    give_warning_with_ఆధారం(
		       (char_u *)_(e_complete_వాడిన_without_allowing_arguments),
								   TRUE, TRUE);
    }
    లేకపోతే ఒకవేళ ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_వాడిన_with_nargs_underscore));
    }
    లేకపోతే
    {
	char_u *tofree = ఖాళీ;

	p = may_get_cmd_ఖండం(eap, p, &tofree, &flags);

	uc_కూడు_command(name, చివర - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	తిరిగి;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    శూన్య
ex_comclear(exarg_T *eap వాడని)
{
    uc_clear(&ucmds);
    ఒకవేళ (curbuf != ఖాళీ)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    స్థిర పూర్ణాంకం
is_ucmd_locked(శూన్య)
{
    ఒకవేళ (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    శూన్య
uc_clear(garray_T *gap)
{
    పూర్ణాంకం		i;
    ucmd_T	*cmd;

    ఒకవేళ (is_ucmd_locked())
	తిరిగి;

    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#ఒకవేళ defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#అయితే_ముగింపు
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    శూన్య
ex_delcommand(exarg_T *eap)
{
    పూర్ణాంకం		i = 0;
    ucmd_T	*cmd = ఖాళీ;
    పూర్ణాంకం		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    పూర్ణాంకం		బఫరు_only = FALSE;

    ఒకవేళ (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	బఫరు_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    కొరకు (;;)
    {
	కొరకు (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    ఒకవేళ (res <= 0)
		ఆపు;
	}
	ఒకవేళ (gap == &ucmds || res == 0 || బఫరు_only)
	    ఆపు;
	gap = &ucmds;
    }

    ఒకవేళ (res != 0)
    {
	semsg(_(బఫరు_only
		    ? e_no_such_user_defined_command_లోపల_current_బఫరు_str
		    : e_no_such_user_defined_command_str), arg);
	తిరిగి;
    }

    ఒకవేళ (is_ucmd_locked())
	తిరిగి;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#ఒకవేళ defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#అయితే_ముగింపు

    --gap->ga_len;

    ఒకవేళ (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * పరిమాణం(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    స్థిర char_u *
uc_విభజించు_వాదనలు(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    పూర్ణాంకం len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    అయితే (*p)
    {
	ఒకవేళ (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	లేకపోతే ఒకవేళ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	లేకపోతే ఒకవేళ (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	లేకపోతే ఒకవేళ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ఒకవేళ (*p == NUL)
		ఆపు;
	    len += 4; // ", "
	}
	లేకపోతే
	{
	    పూర్ణాంకం charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    ఒకవేళ (buf == ఖాళీ)
    {
	*lenp = 0;
	తిరిగి buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    అయితే (*p)
    {
	ఒకవేళ (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	లేకపోతే ఒకవేళ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	లేకపోతే ఒకవేళ (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	లేకపోతే ఒకవేళ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ఒకవేళ (*p == NUL)
		ఆపు;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	లేకపోతే
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    తిరిగి buf;
}

    స్థిర size_t
కూడు_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    అక్షరం	*mod_str,
    size_t	mod_strlen,
    పూర్ణాంకం		*multi_mods)
{
    ఒకవేళ (buf != ఖాళీ)
    {
	ఒకవేళ (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    ఒకవేళ (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    లేకపోతే
	*multi_mods = 1;

    తిరిగి mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
కూడు_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, పూర్ణాంకం *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    ఒకవేళ (cmod->cmod_విభజించు & WSP_ABOVE)
	buflen += కూడు_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    ఒకవేళ (cmod->cmod_విభజించు & WSP_క్రింద)
	buflen += కూడు_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    ఒకవేళ (cmod->cmod_విభజించు & WSP_BOT)
	buflen += కూడు_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    ఒకవేళ (cmod->cmod_tab > 0)
    {
	పూర్ణాంకం tabnr = cmod->cmod_tab - 1;

	ఒకవేళ (tabnr == tabpage_సూచీ(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += కూడు_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	లేకపోతే
	{
	    అక్షరం tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, పరిమాణం(tab_buf), "%dtab", tabnr);
	    buflen += కూడు_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    ఒకవేళ (cmod->cmod_విభజించు & WSP_శిఖరం)
	buflen += కూడు_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    ఒకవేళ (cmod->cmod_విభజించు & WSP_VERT)
	buflen += కూడు_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    ఒకవేళ (cmod->cmod_విభజించు & WSP_HOR)
	buflen += కూడు_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    తిరిగి buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, పూర్ణాంకం quote)
{
    size_t  buflen = 0;
    పూర్ణాంకం	    multi_mods = 0;
    పూర్ణాంకం	    i;
    స్థిర keyvalue_T mod_నమోదు_tab[] =
    {
#నిర్వచితమైతే FEAT_BROWSE_CMD
	KEYVALUE_నమోదు(CMOD_BROWSE, "browse"),
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	KEYVALUE_నమోదు(CMOD_CONFIRM, "confirm"),
#అయితే_ముగింపు
	KEYVALUE_నమోదు(CMOD_HIDE, "hide"),
	KEYVALUE_నమోదు(CMOD_KEEPALT, "keepalt"),
	KEYVALUE_నమోదు(CMOD_KEEPJUMPS, "keepjumps"),
	KEYVALUE_నమోదు(CMOD_KEEPMARKS, "keepmarks"),
	KEYVALUE_నమోదు(CMOD_KEEPPATTERNS, "keeppatterns"),
	KEYVALUE_నమోదు(CMOD_LOCKMARKS, "lockmarks"),
	KEYVALUE_నమోదు(CMOD_NOSWAPFILE, "noswapfile"),
	KEYVALUE_నమోదు(CMOD_UNSILENT, "unsilent"),
	KEYVALUE_నమోదు(CMOD_NOAUTOCMD, "noautocmd"),
#నిర్వచితమైతే HAVE_SANDBOX
	KEYVALUE_నమోదు(CMOD_SANDBOX, "sandbox"),
#అయితే_ముగింపు
	KEYVALUE_నమోదు(CMOD_LEGACY, "legacy")
    };

    ఒకవేళ (quote)
    {
	++buflen;
	ఒకవేళ (buf != ఖాళీ)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    లేకపోతే
    ఒకవేళ (buf != ఖాళీ)
	*buf = NUL;

    // the modifiers that are simple flags
    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(mod_నమోదు_tab); ++i)
	ఒకవేళ (cmod->cmod_flags & mod_నమోదు_tab[i].కీలకం)
	    buflen += కూడు_cmd_modifier(buf, buflen,
		    (అక్షరం *)mod_నమోదు_tab[i].విలువ.string,
		    mod_నమోదు_tab[i].విలువ.నిడివి, &multi_mods);

    // :silent
    ఒకవేళ (cmod->cmod_flags & CMOD_SILENT)
    {
	ఒకవేళ (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += కూడు_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	లేకపోతే
	    buflen += కూడు_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    ఒకవేళ (cmod->cmod_verbose > 0)
    {
	పూర్ణాంకం verbose_విలువ = cmod->cmod_verbose - 1;

	ఒకవేళ (verbose_విలువ == 1)
	    buflen += కూడు_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	లేకపోతే
	{
	    అక్షరం verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, పరిమాణం(verbose_buf), "%dverbose", verbose_విలువ);
	    buflen += కూడు_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += కూడు_win_cmd_modifiers((buf == ఖాళీ) ? ఖాళీ : buf + buflen, cmod, &multi_mods);

    ఒకవేళ (quote)
    {
	ఒకవేళ (buf == ఖాళీ)
	    ++buflen;
	లేకపోతే
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    తిరిగి buflen;
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
    స్థిర size_t
uc_check_సంకేతం(
    char_u	*సంకేతం_2,
    size_t	len,
    char_u	*buf,
    ucmd_T	*cmd,		// the user command we're expanding
    exarg_T	*eap,		// ex arguments
    char_u	**విభజించు_buf,
    size_t	*విభజించు_len)
{
    size_t	ఫలితం = 0;
    char_u	*p = సంకేతం_2 + 1;
    size_t	l = len - 2;
    పూర్ణాంకం		quote = 0;
    గణన {
	ct_వాదనలు,
	ct_BANG,
	ct_లెక్క,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_రిజిస్టరు,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    ఒకవేళ ((vim_strchr((char_u *)"qQfF", *p) != ఖాళీ) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    ఒకవేళ (l <= 1)
	type = ct_NONE;
    లేకపోతే ఒకవేళ (STRNICMP(p, "args>", l) == 0)
	type = ct_వాదనలు;
    లేకపోతే ఒకవేళ (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    లేకపోతే ఒకవేళ (STRNICMP(p, "count>", l) == 0)
	type = ct_లెక్క;
    లేకపోతే ఒకవేళ (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    లేకపోతే ఒకవేళ (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    లేకపోతే ఒకవేళ (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    లేకపోతే ఒకవేళ (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    లేకపోతే ఒకవేళ (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_రిజిస్టరు;
    లేకపోతే ఒకవేళ (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    ఎంపిక (type)
    {
    స్థితి ct_వాదనలు:
	// Simple case first
	ఒకవేళ (*eap->arg == NUL)
	{
	    ఒకవేళ (quote == 1)
	    {
		ఫలితం = 2;
		ఒకవేళ (buf != ఖాళీ)
		    STRCPY(buf, "''");
	    }
	    లేకపోతే
		ఫలితం = 0;
	    ఆపు;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	ఒకవేళ ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	ఎంపిక (quote)
	{
	స్థితి 0: // No quoting, no splitting
	    ఫలితం = STRLEN(eap->arg);
	    ఒకవేళ (buf != ఖాళీ)
		STRCPY(buf, eap->arg);
	    ఆపు;
	స్థితి 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    పూర్ణాంకం	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    ఫలితం = STRLEN(eap->arg) + 2;
	    కొరకు (p = eap->arg; *p; ++p)
	    {
		ఒకవేళ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		లేకపోతే ఒకవేళ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    ఒకవేళ (VIM_ISWHITE(p[1]))
			--ఫలితం;
		    ++p;
		}
		లేకపోతే
		     ఒకవేళ (*p == '\\' || *p == '"')
		    ++ఫలితం;
	    }

	    ఒకవేళ (buf != ఖాళీ)
	    {
		*buf++ = '"';
		కొరకు (p = eap->arg; *p; ++p)
		{
		    ఒకవేళ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    లేకపోతే ఒకవేళ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			ఒకవేళ (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    లేకపోతే
			 ఒకవేళ (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    ఆపు;
	}
	స్థితి 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    ఒకవేళ (*విభజించు_buf == ఖాళీ)
		*విభజించు_buf = uc_విభజించు_వాదనలు(eap->arg, విభజించు_len);

	    ఫలితం = *విభజించు_len;
	    ఒకవేళ (buf != ఖాళీ && ఫలితం != 0)
		STRCPY(buf, *విభజించు_buf);

	    ఆపు;
	}
	ఆపు;

    స్థితి ct_BANG:
	ఫలితం = eap->forceit ? 1 : 0;
	ఒకవేళ (quote)
	    ఫలితం += 2;
	ఒకవేళ (buf != ఖాళీ)
	{
	    ఒకవేళ (quote)
		*buf++ = '"';
	    ఒకవేళ (eap->forceit)
		*buf++ = '!';
	    ఒకవేళ (quote)
		*buf = '"';
	}
	ఆపు;

    స్థితి ct_LINE1:
    స్థితి ct_LINE2:
    స్థితి ct_RANGE:
    స్థితి ct_లెక్క:
    {
	అక్షరం అంకె_buf[NUMBUFLEN];
	దీర్ఘ అంకె = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_లెక్క :
		   (eap->addr_లెక్క > 0) ? eap->line2 : cmd->uc_def;
	size_t అంకె_len;

	అంకె_len = vim_snprintf(అంకె_buf, పరిమాణం(అంకె_buf), "%ld", అంకె);
	ఫలితం = అంకె_len;

	ఒకవేళ (quote)
	    ఫలితం += 2;

	ఒకవేళ (buf != ఖాళీ)
	{
	    ఒకవేళ (quote)
		*buf++ = '"';
	    STRCPY(buf, అంకె_buf);
	    buf += అంకె_len;
	    ఒకవేళ (quote)
		*buf = '"';
	}

	ఆపు;
    }

    స్థితి ct_MODS:
    {
	ఫలితం = produce_cmdmods(buf, &cmdmod, quote);
	ఆపు;
    }

    స్థితి ct_రిజిస్టరు:
	ఫలితం = eap->regname ? 1 : 0;
	ఒకవేళ (quote)
	    ఫలితం += 2;
	ఒకవేళ (buf != ఖాళీ)
	{
	    ఒకవేళ (quote)
		*buf++ = '\'';
	    ఒకవేళ (eap->regname)
		*buf++ = eap->regname;
	    ఒకవేళ (quote)
		*buf = '\'';
	}
	ఆపు;

    స్థితి ct_LT:
	ఫలితం = 1;
	ఒకవేళ (buf != ఖాళీ)
	    *buf = '<';
	ఆపు;

    అప్రమేయ:
	// Not recognized: just copy the '<' and return -1.
	ఫలితం = (size_t)-1;
	ఒకవేళ (buf != ఖాళీ)
	    *buf = '<';
	ఆపు;
    }

    తిరిగి ఫలితం;
}

/*
 * Execute a user defined command.
 */
    శూన్య
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*ప్రారంభం;
    char_u	*చివర = ఖాళీ;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	విభజించు_len = 0;
    char_u	*విభజించు_buf = ఖాళీ;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    పూర్ణాంకం		restore_current_sctx = FALSE;
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం		restore_script_సంస్కరణ = 0;
#అయితే_ముగింపు

    ఒకవేళ (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    లేకపోతే
	cmd = USER_CMD_GA(&prevwin_curwin()->w_బఫరు->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = ఖాళీ;
    కొరకు (;;)
    {
	p = cmd->uc_rep;    //ఆధారం
	q = buf;	    // destination
	totlen = 0;

	కొరకు (;;)
	{
	    ప్రారంభం = vim_strchr(p, '<');
	    ఒకవేళ (ప్రారంభం != ఖాళీ)
		చివర = vim_strchr(ప్రారంభం + 1, '>');
	    ఒకవేళ (buf != ఖాళీ)
	    {
		కొరకు (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		ఒకవేళ (*ksp == K_SPECIAL
			&& (ప్రారంభం == ఖాళీ || ksp < ప్రారంభం || చివర == ఖాళీ)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#నిర్వచితమైతే FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (పూర్ణాంకం)KE_CSI)
#అయితే_ముగింపు
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    ఒకవేళ (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    కొనసాగు;
		}
	    }

	    // break if no <item> is found
	    ఒకవేళ (ప్రారంభం == ఖాళీ || చివర == ఖాళీ)
		ఆపు;

	    // Include the '>'
	    ++చివర;

	    // Take everything up to the '<'
	    len = ప్రారంభం - p;
	    ఒకవేళ (buf == ఖాళీ)
		totlen += len;
	    లేకపోతే
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_సంకేతం(ప్రారంభం, చివర - ప్రారంభం, q, cmd, eap,
			     &విభజించు_buf, &విభజించు_len);
	    ఒకవేళ (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = ప్రారంభం + 1;
		len = 1;
	    }
	    లేకపోతే
		p = చివర;
	    ఒకవేళ (buf == ఖాళీ)
		totlen += len;
	    లేకపోతే
		q += len;
	}
	ఒకవేళ (buf != ఖాళీ)	    // second time here, finished
	{
	    STRCPY(q, p);
	    ఆపు;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	ఒకవేళ (buf == ఖాళీ)
	{
	    vim_free(విభజించు_buf);
	    తిరిగి;
	}
    }

    ఒకవేళ ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_సంస్కరణ = cmd->uc_script_ctx.sc_సంస్కరణ;
#నిర్వచితమైతే FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	ఒకవేళ (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_సంస్కరణ =
				  SCRIPT_అంశం(current_sctx.sc_sid)->sn_సంస్కరణ;
	    SCRIPT_అంశం(current_sctx.sc_sid)->sn_సంస్కరణ = SCRIPT_సంస్కరణ_VIM9;
	}
#అయితే_ముగింపు
    }

    (శూన్య)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    ఒకవేళ (restore_current_sctx)
    {
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (restore_script_సంస్కరణ != 0)
	    SCRIPT_అంశం(current_sctx.sc_sid)->sn_సంస్కరణ =
							restore_script_సంస్కరణ;
#అయితే_ముగింపు
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(విభజించు_buf);
}
