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

#ಅನ್ತರ್ಭಾವ "vim.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    long	uc_def;		// The default value for a range/count
    ಪೂರ್ಣಾಙ್ಕ		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    ಪೂರ್ಣಾಙ್ಕ		uc_flags;	// some UC_ flags
    ಪೂರ್ಣಾಙ್ಕ		uc_compl_opt;	// completion options (UCC_ flags)
#ifdef FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#endif
} ucmd_T;

// List of all user commands.
ಸ್ಥಿರ garray_T ucmds = {0, 0, ಪರಿಮಾಣ(ucmd_T), 4, ರಿಕ್ತ};

// When non-zero it is not allowed to add or remove user commands
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ucmd_locked = 0;

#define USER_CMD(i) (&((ucmd_T *)(ucmds.ga_data))[i])
#define USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_data))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
ಸ್ಥಿರ keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ENTRY(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ENTRY(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ENTRY(EXPAND_BEHAVE, "behave"),
#ಯದಿ defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_BREAKPOINT, "breakpoint"),
#endif
    KEYVALUE_ENTRY(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ENTRY(EXPAND_COLORS, "color"),
    KEYVALUE_ENTRY(EXPAND_COMMANDS, "command"),
    KEYVALUE_ENTRY(EXPAND_COMPILER, "compiler"),
#ಯದಿ defined(FEAT_CSCOPE)
    KEYVALUE_ENTRY(EXPAND_CSCOPE, "cscope"),
#endif
#ಯದಿ defined(FEAT_EVAL)
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
#ಯದಿ defined(FEAT_KEYMAP)
    KEYVALUE_ENTRY(EXPAND_KEYMAP, "keymap"),
#endif
#ಯದಿ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
#ಯದಿ defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_SCRIPTNAMES, "scriptnames"),
#endif
    KEYVALUE_ENTRY(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ENTRY(EXPAND_SHELLCMDLINE, "shellcmdline"),
#ಯದಿ defined(FEAT_SIGNS)
    KEYVALUE_ENTRY(EXPAND_SIGN, "sign"),
#endif
    KEYVALUE_ENTRY(EXPAND_OWNSYNTAX, "syntax"),
#ಯದಿ defined(FEAT_PROFILE)
    KEYVALUE_ENTRY(EXPAND_SYNTIME, "syntime"),
#endif
    KEYVALUE_ENTRY(EXPAND_TAGS, "tag"),
    KEYVALUE_ENTRY(EXPAND_TAGS_LISTFILES, "tag_listfiles"),
    KEYVALUE_ENTRY(EXPAND_USER, "user"),
    KEYVALUE_ENTRY(EXPAND_USER_VARS, "var")
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    cmd_addr_T ಕೀಲಿ;
    ವರ್ಣ *fullname;
    size_t fullnamelen;
    ವರ್ಣ *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#define ADDRTYPE_ENTRY(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
ಸ್ಥಿರ addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ENTRY(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ENTRY(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ENTRY(ADDR_ಸಾಲುಗಳು, "lines", "line"),
    ADDRTYPE_ENTRY(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ENTRY(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ENTRY(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ENTRY(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ENTRY(ADDR_WINDOWS, "windows", "win")
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cmp_addr_type(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b);

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
    ಪೂರ್ಣಾಙ್ಕ		*full,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    ಪೂರ್ಣಾಙ್ಕ		*complp) // completion flags or NULL
{
    ಪೂರ್ಣಾಙ್ಕ		len = (ಪೂರ್ಣಾಙ್ಕ)(p - eap->cmd);
    ಪೂರ್ಣಾಙ್ಕ		j, k, matchlen = 0;
    ucmd_T	*uc;
    ಪೂರ್ಣಾಙ್ಕ		found = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    ಪೂರ್ಣಾಙ್ಕ		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    ಪ್ರತಿ (;;)
    {
	ಪ್ರತಿ (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    ಯಾವತ್ (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    ಯದಿ (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		ಯದಿ (k == len && found && *np != NUL)
		{
		    ಯದಿ (gap == &ucmds)
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		    amb_local = TRUE;
		}

		ಯದಿ (!found || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    ಯದಿ (k == len)
			found = TRUE;
		    ಅನ್ಯಥಾ
			possible = TRUE;

		    ಯದಿ (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    ಅನ್ಯಥಾ
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (long)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    ಯದಿ (complp != ರಿಕ್ತ)
			*complp = uc->uc_compl;
#ifdef FEAT_EVAL
		    ಯದಿ (xp != ರಿಕ್ತ)
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
		    ಯದಿ (k == len && *np == NUL)
		    {
			ಯದಿ (full != ರಿಕ್ತ)
			    *full = TRUE;
			amb_local = FALSE;
			ಭಙ್ಗ;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	ಯದಿ (j < gap->ga_len || gap == &ucmds)
	    ಭಙ್ಗ;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    ಯದಿ (amb_local)
    {
	ಯದಿ (xp != ರಿಕ್ತ)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    ಯದಿ (found || possible)
	ಪ್ರತ್ಯಾಗಮ p + (matchlen - len);
    ಪ್ರತ್ಯಾಗಮ p;
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
    ಯಾವತ್ (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	ಯದಿ (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_pattern = arg;
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    ಯದಿ (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_pattern = p + 1;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_pattern = p + 1;
		ಪ್ರತಿ (char_u *c = p + 1; *c != NUL; ++c)
		    ಯದಿ (*c == ',')
			xp->xp_pattern = c + 1;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_pattern = p + 1;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_pattern = p + 1;
	    }
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    ಯದಿ (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_pattern = arg;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // And finally comes a normal command
    ಪ್ರತ್ಯಾಗಮ skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
set_context_in_user_cmdarg(
	char_u		*cmd UNUSED,
	char_u		*arg,
	long		argt,
	ಪೂರ್ಣಾಙ್ಕ		context,
	expand_T	*xp,
	ಪೂರ್ಣಾಙ್ಕ		forceit)
{
    char_u	*p;

    ಯದಿ (context == EXPAND_NOTHING)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

#ifdef FEAT_MENU
    ಯದಿ (context == EXPAND_MENUS)
	ಪ್ರತ್ಯಾಗಮ set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif
    ಯದಿ (context == EXPAND_COMMANDS)
	ಪ್ರತ್ಯಾಗಮ arg;
    ಯದಿ (context == EXPAND_MAPPINGS)
	ಪ್ರತ್ಯಾಗಮ set_context_in_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    ಯದಿ (!(argt & EX_ARGSPACE))
    {
	p = arg;
	ಯಾವತ್ (*p)
	{
	    ಯದಿ (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    ಅನ್ಯಥಾ ಯದಿ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_pattern = arg;
    xp->xp_context = context;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    char_u *
expand_user_command_name(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ get_user_commands(ರಿಕ್ತ, idx - (ಪೂರ್ಣಾಙ್ಕ)CMD_SIZE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_buffer;

    ಯದಿ (idx < buf->b_ucmds.ga_len)
	ಪ್ರತ್ಯಾಗಮ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    ಯದಿ (idx < ucmds.ga_len)
    {
	ಪೂರ್ಣಾಙ್ಕ	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	ಪ್ರತಿ (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    ಯದಿ (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		ಪ್ರತ್ಯಾಗಮ (char_u *)"";
	ಪ್ರತ್ಯಾಗಮ name;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ifdef FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ cmdidx)
{
    ಯದಿ (cmdidx == CMD_USER && idx < ucmds.ga_len)
	ಪ್ರತ್ಯಾಗಮ USER_CMD(idx)->uc_name;
    ಯದಿ (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_buffer;

	ಯದಿ (idx < buf->b_ucmds.ga_len)
	    ಪ್ರತ್ಯಾಗಮ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
user_command_info(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_buffer->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    ಯದಿ (idx >= gap->ga_len)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_add_string(d, "name", uc->uc_name);
    dict_add_string(d, "kind", (char_u *)"user");
    dict_add_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_add_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_to_str(uc->uc_compl, uc->uc_compl_arg);
    dict_add_string(d, "complete", compl == ರಿಕ್ತ ? (char_u *)"" : compl);
    vim_free(compl);
    dict_add_string(d, "definition", uc->uc_rep);
    dict_add_ಸಂಖ್ಯೆ(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_add_ಸಂಖ್ಯೆ(d, "lnum", uc->uc_script_ctx.sc_lnum);
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx < 0 || idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(addr_type_complete_tab))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಸ್ಥಿರ ವರ್ಣ *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    ಯದಿ (idx < 0 || idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(user_cmd_flags))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಸ್ಥಿರ ವರ್ಣ *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    ಯದಿ (idx < 0 || idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(user_cmd_nargs))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx < 0 || idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(command_complete_tab))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ command_complete_tab[idx].ಮೌಲ್ಯ.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
ಸ್ಥಿರ ವರ್ಣ *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx < 0 || idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(user_cmd_completeopt_tab))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    ಸ್ಥಿರ keyvalue_T *
get_commandtype(ಪೂರ್ಣಾಙ್ಕ expand)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(command_complete_tab); ++i)
	ಯದಿ (command_complete_tab[i].ಕೀಲಿ == expand)
	    ಪ್ರತ್ಯಾಗಮ &command_complete_tab[i];

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ifdef FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_to_str(ಪೂರ್ಣಾಙ್ಕ expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    ಯದಿ (kv == ರಿಕ್ತ || kv->ಮೌಲ್ಯ.string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    cmd_compl = kv->ಮೌಲ್ಯ.string;
    ಯದಿ (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*buffer;

	buffer = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	ಯದಿ (buffer == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	sprintf((ವರ್ಣ *)buffer, "%s,%s", cmd_compl, compl_arg);
	ಪ್ರತ್ಯಾಗಮ buffer;
    }

    ಪ್ರತ್ಯಾಗಮ vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    ಪೂರ್ಣಾಙ್ಕ
cmdcomplete_str_to_type(char_u *complete_str)
{
    keyvalue_T target;
    keyvalue_T *entry;
    ಸ್ಥಿರ keyvalue_T *last_entry = ರಿಕ್ತ;	// cached result

    ಯದಿ (STRNCMP(complete_str, "custom,", 7) == 0)
	ಪ್ರತ್ಯಾಗಮ EXPAND_USER_DEFINED;
    ಯದಿ (STRNCMP(complete_str, "customlist,", 11) == 0)
	ಪ್ರತ್ಯಾಗಮ EXPAND_USER_LIST;

    target.ಕೀಲಿ = 0;
    target.ಮೌಲ್ಯ.string = complete_str;
    target.ಮೌಲ್ಯ.length = 0;			// not used, see cmp_keyvalue_value()

    ಯದಿ (last_entry != ರಿಕ್ತ && cmp_keyvalue_ಮೌಲ್ಯ(&target, last_entry) == 0)
	entry = last_entry;
    ಅನ್ಯಥಾ
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    ಪರಿಮಾಣ(command_complete_tab[0]),
	    cmp_keyvalue_ಮೌಲ್ಯ);
	ಯದಿ (entry == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ EXPAND_NOTHING;

	last_entry = entry;
    }

    ಪ್ರತ್ಯಾಗಮ entry->ಕೀಲಿ;
}
#endif

/*
 * List user commands starting with "name[name_len]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
uc_list(char_u *name, size_t name_len)
{
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		found = FALSE;
    ucmd_T	*cmd;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		over;
    long	a;
    garray_T	*gap;
    keyvalue_T	*entry;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    ಪ್ರತಿ (;;)
    {
	ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (long)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    ಯದಿ (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || message_filtered(cmd->uc_name))
		ಅನುವರ್ತ;

	    // Put out the title first time
	    ಯದಿ (!found)
		ಸಂದೇಶ_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    found = TRUE;
	    ಸಂದೇಶ_putchar('\n');
	    ಯದಿ (got_int)
		ಭಙ್ಗ;

	    // Special cases
	    len = 4;
	    ಯದಿ (a & EX_BANG)
	    {
		ಸಂದೇಶ_putchar('!');
		--len;
	    }
	    ಯದಿ (a & EX_REGSTR)
	    {
		ಸಂದೇಶ_putchar('"');
		--len;
	    }
	    ಯದಿ (gap != &ucmds)
	    {
		ಸಂದೇಶ_putchar('b');
		--len;
	    }
	    ಯದಿ (a & EX_TRLBAR)
	    {
		ಸಂದೇಶ_putchar('|');
		--len;
	    }
	    ಯದಿ (len != 0)
		ಸಂದೇಶ_puts(&"    "[4 - len]);

	    ಸಂದೇಶ_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (ಪೂರ್ಣಾಙ್ಕ)cmd->uc_namelen + 4;

	    ಯದಿ (len < 21)
	    {
		// Field padding spaces   12345678901234567
		ಸ್ಥಿರ ವರ್ಣ spaces[18] = "                 ";
		ಸಂದೇಶ_puts(&spaces[len - 4]);
		len = 21;
	    }
	    ಸಂದೇಶ_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    ವಿಕಲ್ಪ ((ಪೂರ್ಣಾಙ್ಕ)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		ಪಕ್ಷ 0:				IObuff[len++] = '0'; ಭಙ್ಗ;
		ಪಕ್ಷ (EX_EXTRA):		IObuff[len++] = '*'; ಭಙ್ಗ;
		ಪಕ್ಷ (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; ಭಙ್ಗ;
		ಪಕ್ಷ (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; ಭಙ್ಗ;
		ಪಕ್ಷ (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; ಭಙ್ಗ;
		ಪಕ್ಷ (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; ಭಙ್ಗ;
	    }

	    ಕುರು
	    {
		IObuff[len++] = ' ';
	    } ಯಾವತ್ (len < 5 - over);

	    // Address / Range
	    ಯದಿ (a & (EX_RANGE|EX_ಎಣಿಕೆ))
	    {
		ಯದಿ (a & EX_ಎಣಿಕೆ)
		{
		    // -count=N
		    len += vim_snprintf((ವರ್ಣ *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		ಅನ್ಯಥಾ ಯದಿ (a & EX_DFLALL)
		    IObuff[len++] = '%';
		ಅನ್ಯಥಾ ಯದಿ (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((ವರ್ಣ *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		ಅನ್ಯಥಾ
		    IObuff[len++] = '.';
	    }

	    ಕುರು
	    {
		IObuff[len++] = ' ';
	    } ಯಾವತ್ (len < 8 - over);

	    // Address Type
	    ಪ್ರತಿ (j = 0; j < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(addr_type_complete_tab); ++j)
		ಯದಿ (addr_type_complete_tab[j].ಕೀಲಿ != ADDR_ಸಾಲುಗಳು
			&& addr_type_complete_tab[j].ಕೀಲಿ == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (ಪೂರ್ಣಾಙ್ಕ)addr_type_complete_tab[j].shortnamelen;
		    ಭಙ್ಗ;
		}

	    ಕುರು
	    {
		IObuff[len++] = ' ';
	    } ಯಾವತ್ (len < 13 - over);

	    // Completion
	    entry = get_commandtype(cmd->uc_compl);
	    ಯದಿ (entry != ರಿಕ್ತ)
	    {
		STRCPY(IObuff + len, entry->ಮೌಲ್ಯ.string);
		len += (ಪೂರ್ಣಾಙ್ಕ)entry->ಮೌಲ್ಯ.length;
#ifdef FEAT_EVAL
		ಯದಿ (p_verbose > 0 && cmd->uc_compl_arg != ರಿಕ್ತ)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    ಯದಿ (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (ಪೂರ್ಣಾಙ್ಕ)uc_compl_arglen;
		    }
		}
		ಯದಿ (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((ವರ್ಣ *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#endif
	    }

	    ಕುರು
	    {
		IObuff[len++] = ' ';
	    } ಯಾವತ್ (len < 25 - over);

	    IObuff[len] = NUL;
	    ಸಂದೇಶ_outtrans(IObuff);

	    ಸಂದೇಶ_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ifdef FEAT_EVAL
	    ಯದಿ (p_verbose > 0)
		last_set_ಸಂದೇಶ(cmd->uc_script_ctx);
#endif
	    out_flush();
	    ui_breakcheck();
	    ಯದಿ (got_int)
		ಭಙ್ಗ;
	}
	ಯದಿ (gap == &ucmds || i < gap->ga_len)
	    ಭಙ್ಗ;
	gap = &ucmds;
    }

    ಯದಿ (!found)
	ಸಂದೇಶ(_("No user-defined commands found"));

    --ucmd_locked;
}

    ವರ್ಣ *
uc_fun_cmd(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)IObuff;
}

/*
 * Parse address type argument
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_addr_type_arg(
    char_u	*ಮೌಲ್ಯ,
    ಪೂರ್ಣಾಙ್ಕ		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T target;
    addrtype_T *entry;
    ಸ್ಥಿರ addrtype_T *last_entry;	// cached result

    target.ಕೀಲಿ = 0;
    target.fullname = (ವರ್ಣ *)ಮೌಲ್ಯ;
    target.fullnamelen = vallen;

    ಯದಿ (last_entry != ರಿಕ್ತ && cmp_addr_type(&target, last_entry) == 0)
	entry = last_entry;
    ಅನ್ಯಥಾ
    {
	entry = (addrtype_T *)bsearch(&target,
	    &addr_type_complete_tab,
	    ARRAY_LENGTH(addr_type_complete_tab),
	    ಪರಿಮಾಣ(addr_type_complete_tab[0]),
	    cmp_addr_type);
	ಯದಿ (entry == ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ i;
	    char_u	*err = ಮೌಲ್ಯ;

	    ಪ್ರತಿ (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_address_type_ಮೌಲ್ಯ_str), err);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	last_entry = entry;
    }

    *addr_type_arg = entry->ಕೀಲಿ;

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cmp_addr_type(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    ಪ್ರತ್ಯಾಗಮ STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
parse_compl_arg(
    char_u	*ಮೌಲ್ಯ,
    ಪೂರ್ಣಾಙ್ಕ		vallen,
    ಪೂರ್ಣಾಙ್ಕ		*complp,
    long	*argt,
    char_u	**compl_arg UNUSED)
{
    char_u	*arg = ರಿಕ್ತ;
#ಯದಿ defined(FEAT_EVAL)
    size_t	arglen = 0;
#endif
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		valend = vallen;
    keyvalue_T	target;
    keyvalue_T	*entry;
    ಸ್ಥಿರ keyvalue_T	*last_entry = ರಿಕ್ತ;	    // cached result

    // Look for any argument part - which is the part after any ','
    ಪ್ರತಿ (i = 0; i < vallen; ++i)
    {
	ಯದಿ (ಮೌಲ್ಯ[i] == ',')
	{
	    arg = &ಮೌಲ್ಯ[i + 1];
#ಯದಿ defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#endif
	    valend = i;
	    ಭಙ್ಗ;
	}
    }

    target.ಕೀಲಿ = 0;
    target.ಮೌಲ್ಯ.string = ಮೌಲ್ಯ;
    target.ಮೌಲ್ಯ.length = valend;

    ಯದಿ (last_entry != ರಿಕ್ತ && cmp_keyvalue_ಮೌಲ್ಯ_n(&target, last_entry) == 0)
	entry = last_entry;
    ಅನ್ಯಥಾ
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    ಪರಿಮಾಣ(command_complete_tab[0]),
	    cmp_keyvalue_ಮೌಲ್ಯ_n);
	ಯದಿ (entry == ರಿಕ್ತ)
	{
	    semsg(_(e_invalid_complete_ಮೌಲ್ಯ_str), ಮೌಲ್ಯ);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	last_entry = entry;
    }

    *complp = entry->ಕೀಲಿ;
    ಯದಿ (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    ಅನ್ಯಥಾ ಯದಿ (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    ಯದಿ (
#ಯದಿ defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#endif
								arg != ರಿಕ್ತ)
    {
	emsg(_(e_completion_argument_only_allowed_for_custom_completion));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

#ಯದಿ defined(FEAT_EVAL)
    ಯದಿ ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == ರಿಕ್ತ)
    {
	emsg(_(e_custom_completion_requires_function_argument));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (arg != ರಿಕ್ತ)
	*compl_arg = vim_strnsave(arg, arglen);
#endif

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_completeopt_arg(char_u *ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ vallen, ಪೂರ್ಣಾಙ್ಕ *compl_opt)
{
    char_u  *p = ಮೌಲ್ಯ;
    char_u  *end = ಮೌಲ್ಯ + vallen;
    ಪೂರ್ಣಾಙ್ಕ	    flags = 0;

    ಯದಿ (vallen == 0)
    {
	semsg(_(e_argument_required_for_str), "-completeopt");
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯಾವತ್ (p < end)
    {
	char_u	*comma;
	size_t	itemlen;
	ಪೂರ್ಣಾಙ್ಕ	matched = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	i;

	comma = vim_strchr(p, ',');
	ಯದಿ (comma == ರಿಕ್ತ || comma > end)
	    itemlen = (size_t)(end - p);
	ಅನ್ಯಥಾ
	    itemlen = (size_t)(comma - p);

	ಯದಿ (itemlen == 0)
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "completeopt");
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(user_cmd_completeopt_tab); ++i)
	{
	    ವರ್ಣ *name = user_cmd_completeopt_tab[i];

	    ಯದಿ (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		ಯದಿ (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (!matched)
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "completeopt");
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	p += itemlen;
	ಯದಿ (p < end && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
uc_scan_attr(
    char_u	*attr,
    size_t	len,
    long	*argt,
    long	*def,
    ಪೂರ್ಣಾಙ್ಕ		*flags,
    ಪೂರ್ಣಾಙ್ಕ		*complp,
    char_u	**compl_arg,
    ಪೂರ್ಣಾಙ್ಕ		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    ಯದಿ (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // First, try the simple attributes (no arguments)
    ಯದಿ (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_BUFFER;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	i;
	char_u	*val = ರಿಕ್ತ;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)len; ++i)
	{
	    ಯದಿ (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		ಭಙ್ಗ;
	    }
	}

	ಯದಿ (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    ಯದಿ (vallen == 1)
	    {
		ಯದಿ (*val == '0')
		    // Do nothing - this is the default
		    ;
		ಅನ್ಯಥಾ ಯದಿ (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		ಅನ್ಯಥಾ ಯದಿ (*val == '*')
		    *argt |= EX_EXTRA;
		ಅನ್ಯಥಾ ಯದಿ (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		ಅನ್ಯಥಾ ಯದಿ (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		ಅನ್ಯಥಾ ಯದಿ (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		ಅನ್ಯಥಾ
		    ಗಚ್ಛ wrong_nargs;
	    }
	    ಅನ್ಯಥಾ
	    {
wrong_nargs:
		emsg(_(e_invalid_ಸಂಖ್ಯೆ_of_arguments));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ಯದಿ (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    ಅನ್ಯಥಾ ಯದಿ (val != ರಿಕ್ತ)
	    {
		p = val;
		ಯದಿ (*def >= 0)
		{
two_ಎಣಿಕೆ:
		    emsg(_(e_ಎಣಿಕೆ_cannot_be_specified_twice));
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		ಯದಿ (p != val + vallen || vallen == 0)
		{
invalid_ಎಣಿಕೆ:
		    emsg(_(e_invalid_default_ಮೌಲ್ಯ_for_ಎಣಿಕೆ));
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    ಯದಿ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_ಸಾಲುಗಳು;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_ಎಣಿಕೆ | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    ಯದಿ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    ಯದಿ (val != ರಿಕ್ತ)
	    {
		p = val;
		ಯದಿ (*def >= 0)
		    ಗಚ್ಛ two_ಎಣಿಕೆ;

		*def = getdigits(&p);

		ಯದಿ (p != val + vallen)
		    ಗಚ್ಛ invalid_ಎಣಿಕೆ;
	    }

	    ಯದಿ (*def < 0)
		*def = 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    ಯದಿ (val == ರಿಕ್ತ)
	    {
		semsg(_(e_argument_required_for_str), "-complete");
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    ಯದಿ (parse_compl_arg(val, (ಪೂರ್ಣಾಙ್ಕ)vallen, complp, argt, compl_arg)
								      == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    ಯದಿ (val == ರಿಕ್ತ)
	    {
		semsg(_(e_argument_required_for_str), "-completeopt");
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    ಯದಿ (parse_completeopt_arg(val, (ಪೂರ್ಣಾಙ್ಕ)vallen, compl_opt) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ಯದಿ (val == ರಿಕ್ತ)
	    {
		semsg(_(e_argument_required_for_str), "-addr");
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (parse_addr_type_arg(val, (ಪೂರ್ಣಾಙ್ಕ)vallen, addr_type_arg) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (*addr_type_arg != ADDR_ಸಾಲುಗಳು)
		*argt |= EX_ZEROR;
	}
	ಅನ್ಯಥಾ
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
uc_add_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    long	argt,
    long	def,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		compl,
    char_u	*compl_arg UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		compl_opt,
    cmd_addr_T	addr_type,
    ಪೂರ್ಣಾಙ್ಕ		force)
{
    ucmd_T	*cmd = ರಿಕ್ತ;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		cmp = 1;
    char_u	*rep_buf = ರಿಕ್ತ;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, ರಿಕ್ತ);
    ಯದಿ (rep_buf == ರಿಕ್ತ)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	ಯದಿ (rep_buf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // get address of growarray: global or in curbuf
    ಯದಿ (flags & UC_BUFFER)
    {
	gap = &curbuf->b_ucmds;
	ಯದಿ (gap->ga_itemsize == 0)
	    ga_init2(gap, ಪರಿಮಾಣ(ucmd_T), 4);
    }
    ಅನ್ಯಥಾ
	gap = &ucmds;

    // Search for the command in the already defined commands.
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	ಯದಿ (cmp == 0)
	{
	    ಯದಿ (name_len < cmd->uc_namelen)
		cmp = -1;
	    ಅನ್ಯಥಾ ಯದಿ (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	ಯದಿ (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    ಯದಿ (!force
#ifdef FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#endif
		    )
	    {
		semsg(_(e_command_already_exists_add_bang_to_replace_it_str),
									 name);
		ಗಚ್ಛ fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#ಯದಿ defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#endif
	    ಭಙ್ಗ;
	}

	// Stop as soon as we pass the name to add
	ಯದಿ (cmp < 0)
	    ಭಙ್ಗ;
    }

    // Extend the array unless we're replacing an existing command
    ಯದಿ (cmp != 0)
    {
	ಯದಿ (ga_grow(gap, 1) == FAIL)
	    ಗಚ್ಛ fail;
	ಯದಿ ((p = vim_strnsave(name, name_len)) == ರಿಕ್ತ)
	    ಗಚ್ಛ fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * ಪರಿಮಾಣ(ucmd_T));

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
    ಯದಿ (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ifdef FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#endif
    cmd->uc_addr_type = addr_type;

    ಪ್ರತ್ಯಾಗಮ OK;

fail:
    vim_free(rep_buf);
#ಯದಿ defined(FEAT_EVAL)
    vim_free(compl_arg);
#endif
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_block(exarg_T *eap, char_u *p, char_u **tofree, ಪೂರ್ಣಾಙ್ಕ *flags)
{
    char_u *retp = p;
    char_u *block = find_cmd_block_start(p);

    ಯದಿ (block != ರಿಕ್ತ && eap->ea_getline != ರಿಕ್ತ)
    {
	garray_T    ga;
	char_u	    *ಸಾಲು = ರಿಕ್ತ;

	ga_init2(&ga, ಪರಿಮಾಣ(char_u *), 10);
	ಯದಿ (ga_copy_string(&ga, p) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	ಯದಿ (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    ಪ್ರತಿ (;;)
	    {
		vim_free(ಸಾಲು);
		ಯದಿ ((ಸಾಲು = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == ರಿಕ್ತ)
		{
		    emsg(_(e_missing_rcurly));
		    ಭಙ್ಗ;
		}
		ಯದಿ (ga_copy_string(&ga, ಸಾಲು) == FAIL)
		    ಭಙ್ಗ;
		ಯದಿ (*skipwhite(ಸಾಲು) == '}')
		    ಭಙ್ಗ;
	    }
	vim_free(ಸಾಲು);
	retp = *tofree = ga_concat_strings(&ga, "\n");
	ಯದಿ (retp == ರಿಕ್ತ)
	    retp = p;
	ga_clear_strings(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	ಯದಿ (block == p)
	    *flags |= UC_VIM9;
    }
    ಪ್ರತ್ಯಾಗಮ retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    ಶೂನ್ಯ
skip_cmd_block(exarg_T *eap)
{
    char_u	*tofree = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;

    (ಶೂನ್ಯ)may_get_cmd_block(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    ಶೂನ್ಯ
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*end;
    char_u	*p;
    long	argt = 0;
    long	def = -1;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;
    ಪೂರ್ಣಾಙ್ಕ		compl = EXPAND_NOTHING;
    char_u	*compl_arg = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    ಪೂರ್ಣಾಙ್ಕ		has_attr = (eap->arg[0] == '-');
    ಪೂರ್ಣಾಙ್ಕ		name_len;

    ಯದಿ (eap->skip)
    {
	skip_cmd_block(eap);
	ಪ್ರತ್ಯಾಗಮ;
    }

    p = eap->arg;

    // Check for attributes
    ಯಾವತ್ (*p == '-')
    {
	++p;
	end = skiptowhite(p);
	ಯದಿ (uc_scan_attr(p, end - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    ಗಚ್ಛ theend;
	p = skipwhite(end);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    ಯದಿ (ASCII_ISALPHA(*p))
	ಯಾವತ್ (ASCII_ISALNUM(*p))
	    ++p;
    ಯದಿ (!ends_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	ಗಚ್ಛ theend;
    }
    end = p;
    name_len = (ಪೂರ್ಣಾಙ್ಕ)(end - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(end);
    ಯದಿ (!has_attr && ends_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    ಅನ್ಯಥಾ ಯದಿ (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_start_with_an_uppercase_letter));
    }
    ಅನ್ಯಥಾ ಯದಿ ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_used_for_user_defined_command));
    }
    ಅನ್ಯಥಾ ಯದಿ (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	ಯದಿ (in_vim9script())
	    emsg(_(e_complete_used_without_allowing_arguments));
	ಅನ್ಯಥಾ
	    give_warning_with_source(
		       (char_u *)_(e_complete_used_without_allowing_arguments),
								   TRUE, TRUE);
    }
    ಅನ್ಯಥಾ ಯದಿ ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_used_with_nargs_underscore));
    }
    ಅನ್ಯಥಾ
    {
	char_u *tofree = ರಿಕ್ತ;

	p = may_get_cmd_block(eap, p, &tofree, &flags);

	uc_add_command(name, end - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	ಪ್ರತ್ಯಾಗಮ;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    ಶೂನ್ಯ
ex_comclear(exarg_T *eap UNUSED)
{
    uc_clear(&ucmds);
    ಯದಿ (curbuf != ರಿಕ್ತ)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_ucmd_locked(ಶೂನ್ಯ)
{
    ಯದಿ (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    ಶೂನ್ಯ
uc_clear(garray_T *gap)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ucmd_T	*cmd;

    ಯದಿ (is_ucmd_locked())
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#ಯದಿ defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#endif
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    ಶೂನ್ಯ
ex_delcommand(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		i = 0;
    ucmd_T	*cmd = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಙ್ಕ		buffer_only = FALSE;

    ಯದಿ (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	buffer_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    ಪ್ರತಿ (;;)
    {
	ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    ಯದಿ (res <= 0)
		ಭಙ್ಗ;
	}
	ಯದಿ (gap == &ucmds || res == 0 || buffer_only)
	    ಭಙ್ಗ;
	gap = &ucmds;
    }

    ಯದಿ (res != 0)
    {
	semsg(_(buffer_only
		    ? e_no_such_user_defined_command_in_current_buffer_str
		    : e_no_such_user_defined_command_str), arg);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (is_ucmd_locked())
	ಪ್ರತ್ಯಾಗಮ;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#ಯದಿ defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#endif

    --gap->ga_len;

    ಯದಿ (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * ಪರಿಮಾಣ(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    ಸ್ಥಿರ char_u *
uc_split_args(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    ಪೂರ್ಣಾಙ್ಕ len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    ಯಾವತ್ (*p)
    {
	ಯದಿ (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	ಅನ್ಯಥಾ ಯದಿ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ಯದಿ (*p == NUL)
		ಭಙ್ಗ;
	    len += 4; // ", "
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    ಯದಿ (buf == ರಿಕ್ತ)
    {
	*lenp = 0;
	ಪ್ರತ್ಯಾಗಮ buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    ಯಾವತ್ (*p)
    {
	ಯದಿ (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	ಅನ್ಯಥಾ ಯದಿ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	ಅನ್ಯಥಾ ಯದಿ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ಯದಿ (*p == NUL)
		ಭಙ್ಗ;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	ಅನ್ಯಥಾ
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    ಪ್ರತ್ಯಾಗಮ buf;
}

    ಸ್ಥಿರ size_t
add_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    ವರ್ಣ	*mod_str,
    size_t	mod_strlen,
    ಪೂರ್ಣಾಙ್ಕ		*multi_mods)
{
    ಯದಿ (buf != ರಿಕ್ತ)
    {
	ಯದಿ (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    ಯದಿ (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    ಅನ್ಯಥಾ
	*multi_mods = 1;

    ಪ್ರತ್ಯಾಗಮ mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
add_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, ಪೂರ್ಣಾಙ್ಕ *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    ಯದಿ (cmod->cmod_split & WSP_ABOVE)
	buflen += add_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    ಯದಿ (cmod->cmod_split & WSP_BELOW)
	buflen += add_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    ಯದಿ (cmod->cmod_split & WSP_BOT)
	buflen += add_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    ಯದಿ (cmod->cmod_tab > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ tabnr = cmod->cmod_tab - 1;

	ಯದಿ (tabnr == tabpage_index(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += add_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	ಅನ್ಯಥಾ
	{
	    ವರ್ಣ tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, ಪರಿಮಾಣ(tab_buf), "%dtab", tabnr);
	    buflen += add_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    ಯದಿ (cmod->cmod_split & WSP_ತುದಿ)
	buflen += add_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    ಯದಿ (cmod->cmod_split & WSP_VERT)
	buflen += add_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    ಯದಿ (cmod->cmod_split & WSP_HOR)
	buflen += add_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    ಪ್ರತ್ಯಾಗಮ buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, ಪೂರ್ಣಾಙ್ಕ quote)
{
    size_t  buflen = 0;
    ಪೂರ್ಣಾಙ್ಕ	    multi_mods = 0;
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಸ್ಥಿರ keyvalue_T mod_entry_tab[] =
    {
#ifdef FEAT_BROWSE_CMD
	KEYVALUE_ENTRY(CMOD_BROWSE, "browse"),
#endif
#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
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

    ಯದಿ (quote)
    {
	++buflen;
	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    ಅನ್ಯಥಾ
    ಯದಿ (buf != ರಿಕ್ತ)
	*buf = NUL;

    // the modifiers that are simple flags
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(mod_entry_tab); ++i)
	ಯದಿ (cmod->cmod_flags & mod_entry_tab[i].ಕೀಲಿ)
	    buflen += add_cmd_modifier(buf, buflen,
		    (ವರ್ಣ *)mod_entry_tab[i].ಮೌಲ್ಯ.string,
		    mod_entry_tab[i].ಮೌಲ್ಯ.length, &multi_mods);

    // :silent
    ಯದಿ (cmod->cmod_flags & CMOD_SILENT)
    {
	ಯದಿ (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += add_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	ಅನ್ಯಥಾ
	    buflen += add_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    ಯದಿ (cmod->cmod_verbose > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ verbose_ಮೌಲ್ಯ = cmod->cmod_verbose - 1;

	ಯದಿ (verbose_ಮೌಲ್ಯ == 1)
	    buflen += add_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	ಅನ್ಯಥಾ
	{
	    ವರ್ಣ verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, ಪರಿಮಾಣ(verbose_buf), "%dverbose", verbose_ಮೌಲ್ಯ);
	    buflen += add_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += add_win_cmd_modifiers((buf == ರಿಕ್ತ) ? ರಿಕ್ತ : buf + buflen, cmod, &multi_mods);

    ಯದಿ (quote)
    {
	ಯದಿ (buf == ರಿಕ್ತ)
	    ++buflen;
	ಅನ್ಯಥಾ
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    ಪ್ರತ್ಯಾಗಮ buflen;
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
    ಸ್ಥಿರ size_t
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
    ಪೂರ್ಣಾಙ್ಕ		quote = 0;
    ಗಣನಾ {
	ct_ARGS,
	ct_BANG,
	ct_ಎಣಿಕೆ,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_REGISTER,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    ಯದಿ ((vim_strchr((char_u *)"qQfF", *p) != ರಿಕ್ತ) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    ಯದಿ (l <= 1)
	type = ct_NONE;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "args>", l) == 0)
	type = ct_ARGS;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "count>", l) == 0)
	type = ct_ಎಣಿಕೆ;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_REGISTER;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    ವಿಕಲ್ಪ (type)
    {
    ಪಕ್ಷ ct_ARGS:
	// Simple case first
	ಯದಿ (*eap->arg == NUL)
	{
	    ಯದಿ (quote == 1)
	    {
		result = 2;
		ಯದಿ (buf != ರಿಕ್ತ)
		    STRCPY(buf, "''");
	    }
	    ಅನ್ಯಥಾ
		result = 0;
	    ಭಙ್ಗ;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	ಯದಿ ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	ವಿಕಲ್ಪ (quote)
	{
	ಪಕ್ಷ 0: // No quoting, no splitting
	    result = STRLEN(eap->arg);
	    ಯದಿ (buf != ರಿಕ್ತ)
		STRCPY(buf, eap->arg);
	    ಭಙ್ಗ;
	ಪಕ್ಷ 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    ಪೂರ್ಣಾಙ್ಕ	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    result = STRLEN(eap->arg) + 2;
	    ಪ್ರತಿ (p = eap->arg; *p; ++p)
	    {
		ಯದಿ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		ಅನ್ಯಥಾ ಯದಿ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    ಯದಿ (VIM_ISWHITE(p[1]))
			--result;
		    ++p;
		}
		ಅನ್ಯಥಾ
		     ಯದಿ (*p == '\\' || *p == '"')
		    ++result;
	    }

	    ಯದಿ (buf != ರಿಕ್ತ)
	    {
		*buf++ = '"';
		ಪ್ರತಿ (p = eap->arg; *p; ++p)
		{
		    ಯದಿ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    ಅನ್ಯಥಾ ಯದಿ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			ಯದಿ (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    ಅನ್ಯಥಾ
			 ಯದಿ (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    ಭಙ್ಗ;
	}
	ಪಕ್ಷ 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    ಯದಿ (*split_buf == ರಿಕ್ತ)
		*split_buf = uc_split_args(eap->arg, split_len);

	    result = *split_len;
	    ಯದಿ (buf != ರಿಕ್ತ && result != 0)
		STRCPY(buf, *split_buf);

	    ಭಙ್ಗ;
	}
	ಭಙ್ಗ;

    ಪಕ್ಷ ct_BANG:
	result = eap->forceit ? 1 : 0;
	ಯದಿ (quote)
	    result += 2;
	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    ಯದಿ (quote)
		*buf++ = '"';
	    ಯದಿ (eap->forceit)
		*buf++ = '!';
	    ಯದಿ (quote)
		*buf = '"';
	}
	ಭಙ್ಗ;

    ಪಕ್ಷ ct_LINE1:
    ಪಕ್ಷ ct_LINE2:
    ಪಕ್ಷ ct_RANGE:
    ಪಕ್ಷ ct_ಎಣಿಕೆ:
    {
	ವರ್ಣ ಅಂಕೆ_buf[NUMBUFLEN];
	long ಅಂಕೆ = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_ಎಣಿಕೆ :
		   (eap->addr_ಎಣಿಕೆ > 0) ? eap->line2 : cmd->uc_def;
	size_t ಅಂಕೆ_len;

	ಅಂಕೆ_len = vim_snprintf(ಅಂಕೆ_buf, ಪರಿಮಾಣ(ಅಂಕೆ_buf), "%ld", ಅಂಕೆ);
	result = ಅಂಕೆ_len;

	ಯದಿ (quote)
	    result += 2;

	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    ಯದಿ (quote)
		*buf++ = '"';
	    STRCPY(buf, ಅಂಕೆ_buf);
	    buf += ಅಂಕೆ_len;
	    ಯದಿ (quote)
		*buf = '"';
	}

	ಭಙ್ಗ;
    }

    ಪಕ್ಷ ct_MODS:
    {
	result = produce_cmdmods(buf, &cmdmod, quote);
	ಭಙ್ಗ;
    }

    ಪಕ್ಷ ct_REGISTER:
	result = eap->regname ? 1 : 0;
	ಯದಿ (quote)
	    result += 2;
	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    ಯದಿ (quote)
		*buf++ = '\'';
	    ಯದಿ (eap->regname)
		*buf++ = eap->regname;
	    ಯದಿ (quote)
		*buf = '\'';
	}
	ಭಙ್ಗ;

    ಪಕ್ಷ ct_LT:
	result = 1;
	ಯದಿ (buf != ರಿಕ್ತ)
	    *buf = '<';
	ಭಙ್ಗ;

    ಸ್ವಭಾವ:
	// Not recognized: just copy the '<' and return -1.
	result = (size_t)-1;
	ಯದಿ (buf != ರಿಕ್ತ)
	    *buf = '<';
	ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Execute a user defined command.
 */
    ಶೂನ್ಯ
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*start;
    char_u	*end = ರಿಕ್ತ;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	split_len = 0;
    char_u	*split_buf = ರಿಕ್ತ;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    ಪೂರ್ಣಾಙ್ಕ		restore_current_sctx = FALSE;
#ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ		restore_script_version = 0;
#endif

    ಯದಿ (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    ಅನ್ಯಥಾ
	cmd = USER_CMD_GA(&prevwin_curwin()->w_buffer->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = ರಿಕ್ತ;
    ಪ್ರತಿ (;;)
    {
	p = cmd->uc_rep;    // source
	q = buf;	    // destination
	totlen = 0;

	ಪ್ರತಿ (;;)
	{
	    start = vim_strchr(p, '<');
	    ಯದಿ (start != ರಿಕ್ತ)
		end = vim_strchr(start + 1, '>');
	    ಯದಿ (buf != ರಿಕ್ತ)
	    {
		ಪ್ರತಿ (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		ಯದಿ (*ksp == K_SPECIAL
			&& (start == ರಿಕ್ತ || ksp < start || end == ರಿಕ್ತ)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ifdef FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (ಪೂರ್ಣಾಙ್ಕ)KE_CSI)
#endif
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    ಯದಿ (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    ಅನುವರ್ತ;
		}
	    }

	    // break if no <item> is found
	    ಯದಿ (start == ರಿಕ್ತ || end == ರಿಕ್ತ)
		ಭಙ್ಗ;

	    // Include the '>'
	    ++end;

	    // Take everything up to the '<'
	    len = start - p;
	    ಯದಿ (buf == ರಿಕ್ತ)
		totlen += len;
	    ಅನ್ಯಥಾ
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_code(start, end - start, q, cmd, eap,
			     &split_buf, &split_len);
	    ಯದಿ (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = start + 1;
		len = 1;
	    }
	    ಅನ್ಯಥಾ
		p = end;
	    ಯದಿ (buf == ರಿಕ್ತ)
		totlen += len;
	    ಅನ್ಯಥಾ
		q += len;
	}
	ಯದಿ (buf != ರಿಕ್ತ)	    // second time here, finished
	{
	    STRCPY(q, p);
	    ಭಙ್ಗ;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	ಯದಿ (buf == ರಿಕ್ತ)
	{
	    vim_free(split_buf);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    ಯದಿ ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_version = cmd->uc_script_ctx.sc_version;
#ifdef FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	ಯದಿ (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_version =
				  SCRIPT_ITEM(current_sctx.sc_sid)->sn_version;
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = SCRIPT_VERSION_VIM9;
	}
#endif
    }

    (ಶೂನ್ಯ)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    ಯದಿ (restore_current_sctx)
    {
#ifdef FEAT_EVAL
	ಯದಿ (restore_script_version != 0)
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version =
							restore_script_version;
#endif
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(split_buf);
}
