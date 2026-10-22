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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    ದೀರ್ಘ	uc_def;		// The default value for a range/count
    ಪೂರ್ಣಾಂಕ		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    ಪೂರ್ಣಾಂಕ		uc_flags;	// some UC_ flags
    ಪೂರ್ಣಾಂಕ		uc_compl_opt;	// completion options (UCC_ flags)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#ಆದರೆ_ಅಂತ್ಯ
} ucmd_T;

// List of all user commands.
ಸ್ಥಿರ garray_T ucmds = {0, 0, ಗಾತ್ರ(ucmd_T), 4, ಖಾಲಿ};

// When non-zero it is not allowed to add or remove user commands
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ucmd_locked = 0;

#ವ್ಯಾಖ್ಯಾನಿಸು USER_CMD(i) (&((ucmd_T *)(ucmds.ga_ದತ್ತಾಂಶ))[i])
#ವ್ಯಾಖ್ಯಾನಿಸು USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_ದತ್ತಾಂಶ))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
ಸ್ಥಿರ keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ನಮೂದು(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ನಮೂದು(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ನಮೂದು(EXPAND_BEHAVE, "behave"),
#ಆದರೆ defined(FEAT_EVAL)
    KEYVALUE_ನಮೂದು(EXPAND_BREAKPOINT, "breakpoint"),
#ಆದರೆ_ಅಂತ್ಯ
    KEYVALUE_ನಮೂದು(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ನಮೂದು(EXPAND_COLORS, "color"),
    KEYVALUE_ನಮೂದು(EXPAND_COMMANDS, "command"),
    KEYVALUE_ನಮೂದು(EXPAND_COMPILER, "compiler"),
#ಆದರೆ defined(FEAT_CSCOPE)
    KEYVALUE_ನಮೂದು(EXPAND_CSCOPE, "cscope"),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL)
    KEYVALUE_ನಮೂದು(EXPAND_USER_DEFINED, "custom"),
    KEYVALUE_ನಮೂದು(EXPAND_USER_LIST, "customlist"),
#ಆದರೆ_ಅಂತ್ಯ
    KEYVALUE_ನಮೂದು(EXPAND_DIFF_BUFFERS, "diff_buffer"),
    KEYVALUE_ನಮೂದು(EXPAND_DIRECTORIES, "dir"),
    KEYVALUE_ನಮೂದು(EXPAND_DIRS_ಒಳ_CDPATH, "dir_in_path"),
    KEYVALUE_ನಮೂದು(EXPAND_ENV_VARS, "environment"),
    KEYVALUE_ನಮೂದು(EXPAND_EVENTS, "event"),
    KEYVALUE_ನಮೂದು(EXPAND_ಅಭಿವ್ಯಕ್ತಿ, "expression"),
    KEYVALUE_ನಮೂದು(EXPAND_FILES, "file"),
    KEYVALUE_ನಮೂದು(EXPAND_FILES_ಒಳ_ಮಾರ್ಗ, "file_in_path"),
    KEYVALUE_ನಮೂದು(EXPAND_FILETYPE, "filetype"),
    KEYVALUE_ನಮೂದು(EXPAND_FILETYPECMD, "filetypecmd"),
    KEYVALUE_ನಮೂದು(EXPAND_FUNCTIONS, "function"),
    KEYVALUE_ನಮೂದು(EXPAND_HELP, "help"),
    KEYVALUE_ನಮೂದು(EXPAND_HIGHLIGHT, "highlight"),
    KEYVALUE_ನಮೂದು(EXPAND_HISTORY, "history"),
#ಆದರೆ defined(FEAT_KEYMAP)
    KEYVALUE_ನಮೂದು(EXPAND_KEYMAP, "keymap"),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    KEYVALUE_ನಮೂದು(EXPAND_LOCALES, "locale"),
#ಆದರೆ_ಅಂತ್ಯ
    KEYVALUE_ನಮೂದು(EXPAND_MAPCLEAR, "mapclear"),
    KEYVALUE_ನಮೂದು(EXPAND_MAPPINGS, "mapping"),
    KEYVALUE_ನಮೂದು(EXPAND_MENUS, "menu"),
    KEYVALUE_ನಮೂದು(EXPAND_MESSAGES, "messages"),
    KEYVALUE_ನಮೂದು(EXPAND_SETTINGS, "option"),
    KEYVALUE_ನಮೂದು(EXPAND_PACKADD, "packadd"),
    KEYVALUE_ನಮೂದು(EXPAND_RETAB, "retab"),
    KEYVALUE_ನಮೂದು(EXPAND_RUNTIME, "runtime"),
#ಆದರೆ defined(FEAT_EVAL)
    KEYVALUE_ನಮೂದು(EXPAND_SCRIPTNAMES, "scriptnames"),
#ಆದರೆ_ಅಂತ್ಯ
    KEYVALUE_ನಮೂದು(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ನಮೂದು(EXPAND_SHELLCMDLINE, "shellcmdline"),
#ಆದರೆ defined(FEAT_SIGNS)
    KEYVALUE_ನಮೂದು(EXPAND_SIGN, "sign"),
#ಆದರೆ_ಅಂತ್ಯ
    KEYVALUE_ನಮೂದು(EXPAND_OWNSYNTAX, "syntax"),
#ಆದರೆ defined(FEAT_PROFILE)
    KEYVALUE_ನಮೂದು(EXPAND_SYNTIME, "syntime"),
#ಆದರೆ_ಅಂತ್ಯ
    KEYVALUE_ನಮೂದು(EXPAND_ಟ್ಯಾಗುಗಳು, "tag"),
    KEYVALUE_ನಮೂದು(EXPAND_ಟ್ಯಾಗುಗಳು_LISTFILES, "tag_listfiles"),
    KEYVALUE_ನಮೂದು(EXPAND_USER, "user"),
    KEYVALUE_ನಮೂದು(EXPAND_USER_VARS, "var")
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    cmd_addr_T ಕೀಲಿ;
    ಅಕ್ಷರ *fullname;
    size_t fullnamelen;
    ಅಕ್ಷರ *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ADDRTYPE_ನಮೂದು(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
ಸ್ಥಿರ addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ನಮೂದು(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ನಮೂದು(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ನಮೂದು(ADDR_ಸಾಲುಗಳು, "lines", "line"),
    ADDRTYPE_ನಮೂದು(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ನಮೂದು(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ನಮೂದು(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ನಮೂದು(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ನಮೂದು(ADDR_WINDOWS, "windows", "win")
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cmp_addr_type(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b);

/*
 * Search for a user command that matches "eap->cmd".
 * Return cmdidx in "eap->cmdidx", flags in "eap->argt", idx in "eap->useridx".
 * Return a pointer to just after the command.
 * Return NULL if there is no matching command.
 */
    char_u *
ಕಂಡುಹಿಡಿ_ucmd(
    exarg_T	*eap,
    char_u	*p,	 // end of the command (possibly including count)
    ಪೂರ್ಣಾಂಕ		*ತುಂಬಿದ,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    ಪೂರ್ಣಾಂಕ		*complp) // completion flags or NULL
{
    ಪೂರ್ಣಾಂಕ		len = (ಪೂರ್ಣಾಂಕ)(p - eap->cmd);
    ಪೂರ್ಣಾಂಕ		j, k, matchlen = 0;
    ucmd_T	*uc;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು = FALSE;
    ಪೂರ್ಣಾಂಕ		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    ಪೂರ್ಣಾಂಕ		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_ಬಫರ್->b_ucmds;
    ಗಾಗಿ (;;)
    {
	ಗಾಗಿ (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    ಆಗ (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    ಆದರೆ (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		ಆದರೆ (k == len && ಸಿಕ್ಕಿತು && *np != NUL)
		{
		    ಆದರೆ (gap == &ucmds)
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		    amb_local = TRUE;
		}

		ಆದರೆ (!ಸಿಕ್ಕಿತು || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    ಆದರೆ (k == len)
			ಸಿಕ್ಕಿತು = TRUE;
		    ಇಲ್ಲದಿದ್ದರೆ
			possible = TRUE;

		    ಆದರೆ (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    ಇಲ್ಲದಿದ್ದರೆ
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (ದೀರ್ಘ)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    ಆದರೆ (complp != ಖಾಲಿ)
			*complp = uc->uc_compl;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    ಆದರೆ (xp != ಖಾಲಿ)
		    {
			xp->xp_arg = uc->uc_compl_arg;
			xp->xp_complete_opt = uc->uc_compl_opt;
			xp->xp_script_ctx = uc->uc_script_ctx;
			xp->xp_script_ctx.sc_lnum += SOURCING_LNUM;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		    // Do not search for further abbreviations
		    // if this is an exact match.
		    matchlen = k;
		    ಆದರೆ (k == len && *np == NUL)
		    {
			ಆದರೆ (ತುಂಬಿದ != ಖಾಲಿ)
			    *ತುಂಬಿದ = TRUE;
			amb_local = FALSE;
			ನಿಲ್ಲಿಸು;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	ಆದರೆ (j < gap->ga_len || gap == &ucmds)
	    ನಿಲ್ಲಿಸು;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    ಆದರೆ (amb_local)
    {
	ಆದರೆ (xp != ಖಾಲಿ)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    ಆದರೆ (ಸಿಕ್ಕಿತು || possible)
	ಹಿಂತಿರುಗಿ p + (matchlen - len);
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Set completion context for :command
 */
    char_u *
ಗಣ_context_ಒಳ_user_cmd(expand_T *xp, char_u *arg_ಒಳ)
{
    char_u	*arg = arg_ಒಳ;
    char_u	*p;

    // Check for attributes
    ಆಗ (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	ಆದರೆ (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_ನಮೂನೆ = arg;
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    ಆದರೆ (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_ನಮೂನೆ = p + 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_ನಮೂನೆ = p + 1;
		ಗಾಗಿ (char_u *c = p + 1; *c != NUL; ++c)
		    ಆದರೆ (*c == ',')
			xp->xp_ನಮೂನೆ = c + 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_ನಮೂನೆ = p + 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_ನಮೂನೆ = p + 1;
	    }
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    ಆದರೆ (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_ನಮೂನೆ = arg;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // And finally comes a normal command
    ಹಿಂತಿರುಗಿ skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
ಗಣ_context_ಒಳ_user_cmdarg(
	char_u		*cmd ಬಳಸದ,
	char_u		*arg,
	ದೀರ್ಘ		argt,
	ಪೂರ್ಣಾಂಕ		context,
	expand_T	*xp,
	ಪೂರ್ಣಾಂಕ		forceit)
{
    char_u	*p;

    ಆದರೆ (context == EXPAND_NOTHING)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಆದರೆ (context == EXPAND_MENUS)
	ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_menu_cmd(xp, cmd, arg, forceit);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (context == EXPAND_COMMANDS)
	ಹಿಂತಿರುಗಿ arg;
    ಆದರೆ (context == EXPAND_MAPPINGS)
	ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    ಆದರೆ (!(argt & EX_ARGSPACE))
    {
	p = arg;
	ಆಗ (*p)
	{
	    ಆದರೆ (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_ನಮೂನೆ = arg;
    xp->xp_context = context;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    char_u *
expand_user_command_name(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ get_user_commands(ಖಾಲಿ, idx - (ಪೂರ್ಣಾಂಕ)CMD_ಗಾತ್ರ);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_ಬಫರ್;

    ಆದರೆ (idx < buf->b_ucmds.ga_len)
	ಹಿಂತಿರುಗಿ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    ಆದರೆ (idx < ucmds.ga_len)
    {
	ಪೂರ್ಣಾಂಕ	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	ಗಾಗಿ (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    ಆದರೆ (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		ಹಿಂತಿರುಗಿ (char_u *)"";
	ಹಿಂತಿರುಗಿ name;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ cmdidx)
{
    ಆದರೆ (cmdidx == CMD_USER && idx < ucmds.ga_len)
	ಹಿಂತಿರುಗಿ USER_CMD(idx)->uc_name;
    ಆದರೆ (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_ಬಫರ್;

	ಆದರೆ (idx < buf->b_ucmds.ga_len)
	    ಹಿಂತಿರುಗಿ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    ಪೂರ್ಣಾಂಕ
user_command_info(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_ಬಫರ್->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    ಆದರೆ (idx >= gap->ga_len)
	ಹಿಂತಿರುಗಿ FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_ಕೂಡಿಸು_string(d, "name", uc->uc_name);
    dict_ಕೂಡಿಸು_string(d, "kind", (char_u *)"user");
    dict_ಕೂಡಿಸು_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_ಕೂಡಿಸು_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_ಗಮ್ಯ_str(uc->uc_compl, uc->uc_compl_arg);
    dict_ಕೂಡಿಸು_string(d, "complete", compl == ಖಾಲಿ ? (char_u *)"" : compl);
    vim_free(compl);
    dict_ಕೂಡಿಸು_string(d, "definition", uc->uc_rep);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "lnum", uc->uc_script_ctx.sc_lnum);
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx < 0 || idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(addr_type_complete_tab))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    ಆದರೆ (idx < 0 || idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(user_cmd_flags))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    ಆದರೆ (idx < 0 || idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(user_cmd_nargs))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx < 0 || idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(command_complete_tab))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ command_complete_tab[idx].ಮೌಲ್ಯ.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
ಸ್ಥಿರ ಅಕ್ಷರ *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx < 0 || idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(user_cmd_completeopt_tab))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    ಸ್ಥಿರ keyvalue_T *
get_commandtype(ಪೂರ್ಣಾಂಕ expand)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(command_complete_tab); ++i)
	ಆದರೆ (command_complete_tab[i].ಕೀಲಿ == expand)
	    ಹಿಂತಿರುಗಿ &command_complete_tab[i];

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_ಗಮ್ಯ_str(ಪೂರ್ಣಾಂಕ expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    ಆದರೆ (kv == ಖಾಲಿ || kv->ಮೌಲ್ಯ.string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    cmd_compl = kv->ಮೌಲ್ಯ.string;
    ಆದರೆ (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*ಬಫರ್;

	ಬಫರ್ = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	sprintf((ಅಕ್ಷರ *)ಬಫರ್, "%s,%s", cmd_compl, compl_arg);
	ಹಿಂತಿರುಗಿ ಬಫರ್;
    }

    ಹಿಂತಿರುಗಿ vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    ಪೂರ್ಣಾಂಕ
cmdcomplete_str_ಗಮ್ಯ_type(char_u *complete_str)
{
    keyvalue_T ಗುರಿ;
    keyvalue_T *ನಮೂದು;
    ಸ್ಥಿರ keyvalue_T *ಕೊನೆಯ_ನಮೂದು = ಖಾಲಿ;	// cached result

    ಆದರೆ (STRNCMP(complete_str, "custom,", 7) == 0)
	ಹಿಂತಿರುಗಿ EXPAND_USER_DEFINED;
    ಆದರೆ (STRNCMP(complete_str, "customlist,", 11) == 0)
	ಹಿಂತಿರುಗಿ EXPAND_USER_LIST;

    ಗುರಿ.ಕೀಲಿ = 0;
    ಗುರಿ.ಮೌಲ್ಯ.string = complete_str;
    ಗುರಿ.ಮೌಲ್ಯ.ಉದ್ದಳತೆ = 0;			// not used, see cmp_keyvalue_value()

    ಆದರೆ (ಕೊನೆಯ_ನಮೂದು != ಖಾಲಿ && cmp_keyvalue_ಮೌಲ್ಯ(&ಗುರಿ, ಕೊನೆಯ_ನಮೂದು) == 0)
	ನಮೂದು = ಕೊನೆಯ_ನಮೂದು;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ನಮೂದು = (keyvalue_T *)bsearch(&ಗುರಿ,
	    &command_complete_tab,
	    ARRAY_ಉದ್ದಳತೆ(command_complete_tab),
	    ಗಾತ್ರ(command_complete_tab[0]),
	    cmp_keyvalue_ಮೌಲ್ಯ);
	ಆದರೆ (ನಮೂದು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ EXPAND_NOTHING;

	ಕೊನೆಯ_ನಮೂದು = ನಮೂದು;
    }

    ಹಿಂತಿರುಗಿ ನಮೂದು->ಕೀಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * List user commands starting with "name[name_len]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
uc_list(char_u *name, size_t name_len)
{
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು = FALSE;
    ucmd_T	*cmd;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		over;
    ದೀರ್ಘ	a;
    garray_T	*gap;
    keyvalue_T	*ನಮೂದು;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_ಬಫರ್->b_ucmds;
    ಗಾಗಿ (;;)
    {
	ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (ದೀರ್ಘ)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    ಆದರೆ (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || ಸಂದೇಶ_filtered(cmd->uc_name))
		ಮುಂದುವರಿ;

	    // Put out the title first time
	    ಆದರೆ (!ಸಿಕ್ಕಿತು)
		ಸಂದೇಶ_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    ಸಿಕ್ಕಿತು = TRUE;
	    ಸಂದೇಶ_putchar('\n');
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;

	    // Special cases
	    len = 4;
	    ಆದರೆ (a & EX_BANG)
	    {
		ಸಂದೇಶ_putchar('!');
		--len;
	    }
	    ಆದರೆ (a & EX_REGSTR)
	    {
		ಸಂದೇಶ_putchar('"');
		--len;
	    }
	    ಆದರೆ (gap != &ucmds)
	    {
		ಸಂದೇಶ_putchar('b');
		--len;
	    }
	    ಆದರೆ (a & EX_TRLBAR)
	    {
		ಸಂದೇಶ_putchar('|');
		--len;
	    }
	    ಆದರೆ (len != 0)
		ಸಂದೇಶ_puts(&"    "[4 - len]);

	    ಸಂದೇಶ_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (ಪೂರ್ಣಾಂಕ)cmd->uc_namelen + 4;

	    ಆದರೆ (len < 21)
	    {
		// Field padding spaces   12345678901234567
		ಸ್ಥಿರ ಅಕ್ಷರ spaces[18] = "                 ";
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
	    ಆಯ್ಕೆ ((ಪೂರ್ಣಾಂಕ)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		ಸ್ಥಿತಿ 0:				IObuff[len++] = '0'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ (EX_EXTRA):		IObuff[len++] = '*'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; ನಿಲ್ಲಿಸು;
	    }

	    ಮಾಡು
	    {
		IObuff[len++] = ' ';
	    } ಆಗ (len < 5 - over);

	    // Address / Range
	    ಆದರೆ (a & (EX_RANGE|EX_ಎಣಿಕೆ))
	    {
		ಆದರೆ (a & EX_ಎಣಿಕೆ)
		{
		    // -count=N
		    len += vim_snprintf((ಅಕ್ಷರ *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a & EX_DFLALL)
		    IObuff[len++] = '%';
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((ಅಕ್ಷರ *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    IObuff[len++] = '.';
	    }

	    ಮಾಡು
	    {
		IObuff[len++] = ' ';
	    } ಆಗ (len < 8 - over);

	    // Address Type
	    ಗಾಗಿ (j = 0; j < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(addr_type_complete_tab); ++j)
		ಆದರೆ (addr_type_complete_tab[j].ಕೀಲಿ != ADDR_ಸಾಲುಗಳು
			&& addr_type_complete_tab[j].ಕೀಲಿ == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (ಪೂರ್ಣಾಂಕ)addr_type_complete_tab[j].shortnamelen;
		    ನಿಲ್ಲಿಸು;
		}

	    ಮಾಡು
	    {
		IObuff[len++] = ' ';
	    } ಆಗ (len < 13 - over);

	    // Completion
	    ನಮೂದು = get_commandtype(cmd->uc_compl);
	    ಆದರೆ (ನಮೂದು != ಖಾಲಿ)
	    {
		STRCPY(IObuff + len, ನಮೂದು->ಮೌಲ್ಯ.string);
		len += (ಪೂರ್ಣಾಂಕ)ನಮೂದು->ಮೌಲ್ಯ.ಉದ್ದಳತೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಆದರೆ (p_verbose > 0 && cmd->uc_compl_arg != ಖಾಲಿ)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    ಆದರೆ (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (ಪೂರ್ಣಾಂಕ)uc_compl_arglen;
		    }
		}
		ಆದರೆ (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((ಅಕ್ಷರ *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    ಮಾಡು
	    {
		IObuff[len++] = ' ';
	    } ಆಗ (len < 25 - over);

	    IObuff[len] = NUL;
	    ಸಂದೇಶ_outtrans(IObuff);

	    ಸಂದೇಶ_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (p_verbose > 0)
		ಕೊನೆಯ_ಗಣ_ಸಂದೇಶ(cmd->uc_script_ctx);
#ಆದರೆ_ಅಂತ್ಯ
	    ಹೊರಹೋಗು_flush();
	    ui_breakcheck();
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (gap == &ucmds || i < gap->ga_len)
	    ನಿಲ್ಲಿಸು;
	gap = &ucmds;
    }

    ಆದರೆ (!ಸಿಕ್ಕಿತು)
	ಸಂದೇಶ(_("No user-defined commands found"));

    --ucmd_locked;
}

    ಅಕ್ಷರ *
uc_fun_cmd(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)IObuff;
}

/*
 * Parse address type argument
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_addr_type_arg(
    char_u	*ಮೌಲ್ಯ,
    ಪೂರ್ಣಾಂಕ		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T ಗುರಿ;
    addrtype_T *ನಮೂದು;
    ಸ್ಥಿರ addrtype_T *ಕೊನೆಯ_ನಮೂದು;	// cached result

    ಗುರಿ.ಕೀಲಿ = 0;
    ಗುರಿ.fullname = (ಅಕ್ಷರ *)ಮೌಲ್ಯ;
    ಗುರಿ.fullnamelen = vallen;

    ಆದರೆ (ಕೊನೆಯ_ನಮೂದು != ಖಾಲಿ && cmp_addr_type(&ಗುರಿ, ಕೊನೆಯ_ನಮೂದು) == 0)
	ನಮೂದು = ಕೊನೆಯ_ನಮೂದು;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ನಮೂದು = (addrtype_T *)bsearch(&ಗುರಿ,
	    &addr_type_complete_tab,
	    ARRAY_ಉದ್ದಳತೆ(addr_type_complete_tab),
	    ಗಾತ್ರ(addr_type_complete_tab[0]),
	    cmp_addr_type);
	ಆದರೆ (ನಮೂದು == ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ i;
	    char_u	*err = ಮೌಲ್ಯ;

	    ಗಾಗಿ (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_ವಿಳಾಸ_type_ಮೌಲ್ಯ_str), err);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಕೊನೆಯ_ನಮೂದು = ನಮೂದು;
    }

    *addr_type_arg = ನಮೂದು->ಕೀಲಿ;

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmp_addr_type(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    ಹಿಂತಿರುಗಿ STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    ಪೂರ್ಣಾಂಕ
parse_compl_arg(
    char_u	*ಮೌಲ್ಯ,
    ಪೂರ್ಣಾಂಕ		vallen,
    ಪೂರ್ಣಾಂಕ		*complp,
    ದೀರ್ಘ	*argt,
    char_u	**compl_arg ಬಳಸದ)
{
    char_u	*arg = ಖಾಲಿ;
#ಆದರೆ defined(FEAT_EVAL)
    size_t	arglen = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		valend = vallen;
    keyvalue_T	ಗುರಿ;
    keyvalue_T	*ನಮೂದು;
    ಸ್ಥಿರ keyvalue_T	*ಕೊನೆಯ_ನಮೂದು = ಖಾಲಿ;	    // cached result

    // Look for any argument part - which is the part after any ','
    ಗಾಗಿ (i = 0; i < vallen; ++i)
    {
	ಆದರೆ (ಮೌಲ್ಯ[i] == ',')
	{
	    arg = &ಮೌಲ್ಯ[i + 1];
#ಆದರೆ defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#ಆದರೆ_ಅಂತ್ಯ
	    valend = i;
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಗುರಿ.ಕೀಲಿ = 0;
    ಗುರಿ.ಮೌಲ್ಯ.string = ಮೌಲ್ಯ;
    ಗುರಿ.ಮೌಲ್ಯ.ಉದ್ದಳತೆ = valend;

    ಆದರೆ (ಕೊನೆಯ_ನಮೂದು != ಖಾಲಿ && cmp_keyvalue_ಮೌಲ್ಯ_n(&ಗುರಿ, ಕೊನೆಯ_ನಮೂದು) == 0)
	ನಮೂದು = ಕೊನೆಯ_ನಮೂದು;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ನಮೂದು = (keyvalue_T *)bsearch(&ಗುರಿ,
	    &command_complete_tab,
	    ARRAY_ಉದ್ದಳತೆ(command_complete_tab),
	    ಗಾತ್ರ(command_complete_tab[0]),
	    cmp_keyvalue_ಮೌಲ್ಯ_n);
	ಆದರೆ (ನಮೂದು == ಖಾಲಿ)
	{
	    semsg(_(e_invalid_complete_ಮೌಲ್ಯ_str), ಮೌಲ್ಯ);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಕೊನೆಯ_ನಮೂದು = ನಮೂದು;
    }

    *complp = ನಮೂದು->ಕೀಲಿ;
    ಆದರೆ (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    ಆದರೆ (
#ಆದರೆ defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#ಆದರೆ_ಅಂತ್ಯ
								arg != ಖಾಲಿ)
    {
	emsg(_(e_completion_ವಾದ_only_allowed_for_custom_completion));
	ಹಿಂತಿರುಗಿ FAIL;
    }

#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == ಖಾಲಿ)
    {
	emsg(_(e_custom_completion_requires_function_ವಾದ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (arg != ಖಾಲಿ)
	*compl_arg = vim_strnsave(arg, arglen);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_completeopt_arg(char_u *ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ vallen, ಪೂರ್ಣಾಂಕ *compl_opt)
{
    char_u  *p = ಮೌಲ್ಯ;
    char_u  *ಕೊನೆ = ಮೌಲ್ಯ + vallen;
    ಪೂರ್ಣಾಂಕ	    flags = 0;

    ಆದರೆ (vallen == 0)
    {
	semsg(_(e_ವಾದ_required_for_str), "-completeopt");
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆಗ (p < ಕೊನೆ)
    {
	char_u	*comma;
	size_t	itemlen;
	ಪೂರ್ಣಾಂಕ	matched = FALSE;
	ಪೂರ್ಣಾಂಕ	i;

	comma = vim_strchr(p, ',');
	ಆದರೆ (comma == ಖಾಲಿ || comma > ಕೊನೆ)
	    itemlen = (size_t)(ಕೊನೆ - p);
	ಇಲ್ಲದಿದ್ದರೆ
	    itemlen = (size_t)(comma - p);

	ಆದರೆ (itemlen == 0)
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "completeopt");
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(user_cmd_completeopt_tab); ++i)
	{
	    ಅಕ್ಷರ *name = user_cmd_completeopt_tab[i];

	    ಆದರೆ (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		ಆದರೆ (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (!matched)
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "completeopt");
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	p += itemlen;
	ಆದರೆ (p < ಕೊನೆ && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
uc_ಪರಿಶೀಲನೆ_attr(
    char_u	*attr,
    size_t	len,
    ದೀರ್ಘ	*argt,
    ದೀರ್ಘ	*def,
    ಪೂರ್ಣಾಂಕ		*flags,
    ಪೂರ್ಣಾಂಕ		*complp,
    char_u	**compl_arg,
    ಪೂರ್ಣಾಂಕ		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    ಆದರೆ (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // First, try the simple attributes (no arguments)
    ಆದರೆ (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_ಬಫರ್;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	i;
	char_u	*val = ಖಾಲಿ;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)len; ++i)
	{
	    ಆದರೆ (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		ನಿಲ್ಲಿಸು;
	    }
	}

	ಆದರೆ (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    ಆದರೆ (vallen == 1)
	    {
		ಆದರೆ (*val == '0')
		    // Do nothing - this is the default
		    ;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*val == '*')
		    *argt |= EX_EXTRA;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹೋಗು wrong_nargs;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
wrong_nargs:
		emsg(_(e_invalid_ಸಂಖ್ಯೆ_ದ_arguments));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ಆದರೆ (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val != ಖಾಲಿ)
	    {
		p = val;
		ಆದರೆ (*def >= 0)
		{
two_ಎಣಿಕೆ:
		    emsg(_(e_ಎಣಿಕೆ_cannot_be_specified_twice));
		    ಹಿಂತಿರುಗಿ FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		ಆದರೆ (p != val + vallen || vallen == 0)
		{
invalid_ಎಣಿಕೆ:
		    emsg(_(e_invalid_default_ಮೌಲ್ಯ_for_ಎಣಿಕೆ));
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    ಆದರೆ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_ಸಾಲುಗಳು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_ಎಣಿಕೆ | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    ಆದರೆ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    ಆದರೆ (val != ಖಾಲಿ)
	    {
		p = val;
		ಆದರೆ (*def >= 0)
		    ಹೋಗು two_ಎಣಿಕೆ;

		*def = getdigits(&p);

		ಆದರೆ (p != val + vallen)
		    ಹೋಗು invalid_ಎಣಿಕೆ;
	    }

	    ಆದರೆ (*def < 0)
		*def = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    ಆದರೆ (val == ಖಾಲಿ)
	    {
		semsg(_(e_ವಾದ_required_for_str), "-complete");
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    ಆದರೆ (parse_compl_arg(val, (ಪೂರ್ಣಾಂಕ)vallen, complp, argt, compl_arg)
								      == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    ಆದರೆ (val == ಖಾಲಿ)
	    {
		semsg(_(e_ವಾದ_required_for_str), "-completeopt");
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    ಆದರೆ (parse_completeopt_arg(val, (ಪೂರ್ಣಾಂಕ)vallen, compl_opt) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ಆದರೆ (val == ಖಾಲಿ)
	    {
		semsg(_(e_ವಾದ_required_for_str), "-addr");
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (parse_addr_type_arg(val, (ಪೂರ್ಣಾಂಕ)vallen, addr_type_arg) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (*addr_type_arg != ADDR_ಸಾಲುಗಳು)
		*argt |= EX_ZEROR;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
uc_ಕೂಡಿಸು_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    ದೀರ್ಘ	argt,
    ದೀರ್ಘ	def,
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		compl,
    char_u	*compl_arg ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		compl_opt,
    cmd_addr_T	addr_type,
    ಪೂರ್ಣಾಂಕ		force)
{
    ucmd_T	*cmd = ಖಾಲಿ;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		cmp = 1;
    char_u	*rep_buf = ಖಾಲಿ;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, ಖಾಲಿ);
    ಆದರೆ (rep_buf == ಖಾಲಿ)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	ಆದರೆ (rep_buf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // get address of growarray: global or in curbuf
    ಆದರೆ (flags & UC_ಬಫರ್)
    {
	gap = &curbuf->b_ucmds;
	ಆದರೆ (gap->ga_itemsize == 0)
	    ga_init2(gap, ಗಾತ್ರ(ucmd_T), 4);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	gap = &ucmds;

    // Search for the command in the already defined commands.
    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	ಆದರೆ (cmp == 0)
	{
	    ಆದರೆ (name_len < cmd->uc_namelen)
		cmp = -1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	ಆದರೆ (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    ಆದರೆ (!force
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#ಆದರೆ_ಅಂತ್ಯ
		    )
	    {
		semsg(_(e_command_already_exists_ಕೂಡಿಸು_bang_ಗಮ್ಯ_replace_it_str),
									 name);
		ಹೋಗು fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#ಆದರೆ defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	}

	// Stop as soon as we pass the name to add
	ಆದರೆ (cmp < 0)
	    ನಿಲ್ಲಿಸು;
    }

    // Extend the array unless we're replacing an existing command
    ಆದರೆ (cmp != 0)
    {
	ಆದರೆ (ga_grow(gap, 1) == FAIL)
	    ಹೋಗು fail;
	ಆದರೆ ((p = vim_strnsave(name, name_len)) == ಖಾಲಿ)
	    ಹೋಗು fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * ಗಾತ್ರ(ucmd_T));

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
    ಆದರೆ (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#ಆದರೆ_ಅಂತ್ಯ
    cmd->uc_addr_type = addr_type;

    ಹಿಂತಿರುಗಿ OK;

fail:
    vim_free(rep_buf);
#ಆದರೆ defined(FEAT_EVAL)
    vim_free(compl_arg);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_ಖಂಡ(exarg_T *eap, char_u *p, char_u **tofree, ಪೂರ್ಣಾಂಕ *flags)
{
    char_u *retp = p;
    char_u *ಖಂಡ = ಕಂಡುಹಿಡಿ_cmd_ಖಂಡ_ಆರಂಭ(p);

    ಆದರೆ (ಖಂಡ != ಖಾಲಿ && eap->ea_getline != ಖಾಲಿ)
    {
	garray_T    ga;
	char_u	    *ಸಾಲು = ಖಾಲಿ;

	ga_init2(&ga, ಗಾತ್ರ(char_u *), 10);
	ಆದರೆ (ga_copy_string(&ga, p) == FAIL)
	    ಹಿಂತಿರುಗಿ retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	ಆದರೆ (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    ಗಾಗಿ (;;)
	    {
		vim_free(ಸಾಲು);
		ಆದರೆ ((ಸಾಲು = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == ಖಾಲಿ)
		{
		    emsg(_(e_missing_rcurly));
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (ga_copy_string(&ga, ಸಾಲು) == FAIL)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (*skipwhite(ಸಾಲು) == '}')
		    ನಿಲ್ಲಿಸು;
	    }
	vim_free(ಸಾಲು);
	retp = *tofree = ga_concat_ಅಕ್ಷರಸರಣಿಗಳು(&ga, "\n");
	ಆದರೆ (retp == ಖಾಲಿ)
	    retp = p;
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	ಆದರೆ (ಖಂಡ == p)
	    *flags |= UC_VIM9;
    }
    ಹಿಂತಿರುಗಿ retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    ಶೂನ್ಯ
skip_cmd_ಖಂಡ(exarg_T *eap)
{
    char_u	*tofree = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		flags = 0;

    (ಶೂನ್ಯ)may_get_cmd_ಖಂಡ(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    ಶೂನ್ಯ
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*ಕೊನೆ;
    char_u	*p;
    ದೀರ್ಘ	argt = 0;
    ದೀರ್ಘ	def = -1;
    ಪೂರ್ಣಾಂಕ		flags = 0;
    ಪೂರ್ಣಾಂಕ		compl = EXPAND_NOTHING;
    char_u	*compl_arg = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    ಪೂರ್ಣಾಂಕ		has_attr = (eap->arg[0] == '-');
    ಪೂರ್ಣಾಂಕ		name_len;

    ಆದರೆ (eap->skip)
    {
	skip_cmd_ಖಂಡ(eap);
	ಹಿಂತಿರುಗಿ;
    }

    p = eap->arg;

    // Check for attributes
    ಆಗ (*p == '-')
    {
	++p;
	ಕೊನೆ = skiptowhite(p);
	ಆದರೆ (uc_ಪರಿಶೀಲನೆ_attr(p, ಕೊನೆ - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    ಹೋಗು theend;
	p = skipwhite(ಕೊನೆ);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    ಆದರೆ (ASCII_ISALPHA(*p))
	ಆಗ (ASCII_ISALNUM(*p))
	    ++p;
    ಆದರೆ (!ತುದಿಗಳು_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	ಹೋಗು theend;
    }
    ಕೊನೆ = p;
    name_len = (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(ಕೊನೆ);
    ಆದರೆ (!has_attr && ತುದಿಗಳು_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_ಆರಂಭ_with_an_uppercase_letter));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_ಬಳಸಿದ_for_user_defined_command));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	ಆದರೆ (ಒಳ_vim9script())
	    emsg(_(e_complete_ಬಳಸಿದ_without_allowing_arguments));
	ಇಲ್ಲದಿದ್ದರೆ
	    give_warning_with_ಮೂಲ(
		       (char_u *)_(e_complete_ಬಳಸಿದ_without_allowing_arguments),
								   TRUE, TRUE);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_ಬಳಸಿದ_with_nargs_underscore));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *tofree = ಖಾಲಿ;

	p = may_get_cmd_ಖಂಡ(eap, p, &tofree, &flags);

	uc_ಕೂಡಿಸು_command(name, ಕೊನೆ - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	ಹಿಂತಿರುಗಿ;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    ಶೂನ್ಯ
ex_comclear(exarg_T *eap ಬಳಸದ)
{
    uc_clear(&ucmds);
    ಆದರೆ (curbuf != ಖಾಲಿ)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_ucmd_locked(ಶೂನ್ಯ)
{
    ಆದರೆ (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    ಶೂನ್ಯ
uc_clear(garray_T *gap)
{
    ಪೂರ್ಣಾಂಕ		i;
    ucmd_T	*cmd;

    ಆದರೆ (is_ucmd_locked())
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#ಆದರೆ defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    ಶೂನ್ಯ
ex_delcommand(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		i = 0;
    ucmd_T	*cmd = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಂಕ		ಬಫರ್_only = FALSE;

    ಆದರೆ (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	ಬಫರ್_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    ಗಾಗಿ (;;)
    {
	ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    ಆದರೆ (res <= 0)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (gap == &ucmds || res == 0 || ಬಫರ್_only)
	    ನಿಲ್ಲಿಸು;
	gap = &ucmds;
    }

    ಆದರೆ (res != 0)
    {
	semsg(_(ಬಫರ್_only
		    ? e_no_such_user_defined_command_ಒಳ_current_ಬಫರ್_str
		    : e_no_such_user_defined_command_str), arg);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (is_ucmd_locked())
	ಹಿಂತಿರುಗಿ;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#ಆದರೆ defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#ಆದರೆ_ಅಂತ್ಯ

    --gap->ga_len;

    ಆದರೆ (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * ಗಾತ್ರ(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    ಸ್ಥಿರ char_u *
uc_ವಿಭಜಿಸು_ವಾದಗಳು(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    ಪೂರ್ಣಾಂಕ len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    ಆಗ (*p)
    {
	ಆದರೆ (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ಆದರೆ (*p == NUL)
		ನಿಲ್ಲಿಸು;
	    len += 4; // ", "
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	*lenp = 0;
	ಹಿಂತಿರುಗಿ buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    ಆಗ (*p)
    {
	ಆದರೆ (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ಆದರೆ (*p == NUL)
		ನಿಲ್ಲಿಸು;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    ಹಿಂತಿರುಗಿ buf;
}

    ಸ್ಥಿರ size_t
ಕೂಡಿಸು_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    ಅಕ್ಷರ	*mod_str,
    size_t	mod_strlen,
    ಪೂರ್ಣಾಂಕ		*multi_mods)
{
    ಆದರೆ (buf != ಖಾಲಿ)
    {
	ಆದರೆ (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    ಆದರೆ (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    ಇಲ್ಲದಿದ್ದರೆ
	*multi_mods = 1;

    ಹಿಂತಿರುಗಿ mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
ಕೂಡಿಸು_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, ಪೂರ್ಣಾಂಕ *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    ಆದರೆ (cmod->cmod_ವಿಭಜಿಸು & WSP_ABOVE)
	buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    ಆದರೆ (cmod->cmod_ವಿಭಜಿಸು & WSP_ಕೆಳಗೆ)
	buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    ಆದರೆ (cmod->cmod_ವಿಭಜಿಸು & WSP_BOT)
	buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    ಆದರೆ (cmod->cmod_tab > 0)
    {
	ಪೂರ್ಣಾಂಕ tabnr = cmod->cmod_tab - 1;

	ಆದರೆ (tabnr == tabpage_ಸೂಚ್ಯಂಕ(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಅಕ್ಷರ tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, ಗಾತ್ರ(tab_buf), "%dtab", tabnr);
	    buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    ಆದರೆ (cmod->cmod_ವಿಭಜಿಸು & WSP_ತುದಿ)
	buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    ಆದರೆ (cmod->cmod_ವಿಭಜಿಸು & WSP_VERT)
	buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    ಆದರೆ (cmod->cmod_ವಿಭಜಿಸು & WSP_HOR)
	buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    ಹಿಂತಿರುಗಿ buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, ಪೂರ್ಣಾಂಕ quote)
{
    size_t  buflen = 0;
    ಪೂರ್ಣಾಂಕ	    multi_mods = 0;
    ಪೂರ್ಣಾಂಕ	    i;
    ಸ್ಥಿರ keyvalue_T mod_ನಮೂದು_tab[] =
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE_CMD
	KEYVALUE_ನಮೂದು(CMOD_BROWSE, "browse"),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	KEYVALUE_ನಮೂದು(CMOD_CONFIRM, "confirm"),
#ಆದರೆ_ಅಂತ್ಯ
	KEYVALUE_ನಮೂದು(CMOD_HIDE, "hide"),
	KEYVALUE_ನಮೂದು(CMOD_KEEPALT, "keepalt"),
	KEYVALUE_ನಮೂದು(CMOD_KEEPJUMPS, "keepjumps"),
	KEYVALUE_ನಮೂದು(CMOD_KEEPMARKS, "keepmarks"),
	KEYVALUE_ನಮೂದು(CMOD_KEEPPATTERNS, "keeppatterns"),
	KEYVALUE_ನಮೂದು(CMOD_LOCKMARKS, "lockmarks"),
	KEYVALUE_ನಮೂದು(CMOD_NOSWAPFILE, "noswapfile"),
	KEYVALUE_ನಮೂದು(CMOD_UNSILENT, "unsilent"),
	KEYVALUE_ನಮೂದು(CMOD_NOAUTOCMD, "noautocmd"),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
	KEYVALUE_ನಮೂದು(CMOD_SANDBOX, "sandbox"),
#ಆದರೆ_ಅಂತ್ಯ
	KEYVALUE_ನಮೂದು(CMOD_LEGACY, "legacy")
    };

    ಆದರೆ (quote)
    {
	++buflen;
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (buf != ಖಾಲಿ)
	*buf = NUL;

    // the modifiers that are simple flags
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(mod_ನಮೂದು_tab); ++i)
	ಆದರೆ (cmod->cmod_flags & mod_ನಮೂದು_tab[i].ಕೀಲಿ)
	    buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen,
		    (ಅಕ್ಷರ *)mod_ನಮೂದು_tab[i].ಮೌಲ್ಯ.string,
		    mod_ನಮೂದು_tab[i].ಮೌಲ್ಯ.ಉದ್ದಳತೆ, &multi_mods);

    // :silent
    ಆದರೆ (cmod->cmod_flags & CMOD_SILENT)
    {
	ಆದರೆ (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	ಇಲ್ಲದಿದ್ದರೆ
	    buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    ಆದರೆ (cmod->cmod_verbose > 0)
    {
	ಪೂರ್ಣಾಂಕ verbose_ಮೌಲ್ಯ = cmod->cmod_verbose - 1;

	ಆದರೆ (verbose_ಮೌಲ್ಯ == 1)
	    buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಅಕ್ಷರ verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, ಗಾತ್ರ(verbose_buf), "%dverbose", verbose_ಮೌಲ್ಯ);
	    buflen += ಕೂಡಿಸು_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += ಕೂಡಿಸು_win_cmd_modifiers((buf == ಖಾಲಿ) ? ಖಾಲಿ : buf + buflen, cmod, &multi_mods);

    ಆದರೆ (quote)
    {
	ಆದರೆ (buf == ಖಾಲಿ)
	    ++buflen;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    ಹಿಂತಿರುಗಿ buflen;
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
uc_check_ಸಂಕೇತ(
    char_u	*ಸಂಕೇತ_2,
    size_t	len,
    char_u	*buf,
    ucmd_T	*cmd,		// the user command we're expanding
    exarg_T	*eap,		// ex arguments
    char_u	**ವಿಭಜಿಸು_buf,
    size_t	*ವಿಭಜಿಸು_len)
{
    size_t	ಫಲಿತಾಂಶ = 0;
    char_u	*p = ಸಂಕೇತ_2 + 1;
    size_t	l = len - 2;
    ಪೂರ್ಣಾಂಕ		quote = 0;
    ಎಣಿಕೆ {
	ct_ವಾದಗಳು,
	ct_BANG,
	ct_ಎಣಿಕೆ,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_ರಿಜಿಸ್ಟರ್,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    ಆದರೆ ((vim_strchr((char_u *)"qQfF", *p) != ಖಾಲಿ) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    ಆದರೆ (l <= 1)
	type = ct_NONE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "args>", l) == 0)
	type = ct_ವಾದಗಳು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "count>", l) == 0)
	type = ct_ಎಣಿಕೆ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_ರಿಜಿಸ್ಟರ್;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    ಆಯ್ಕೆ (type)
    {
    ಸ್ಥಿತಿ ct_ವಾದಗಳು:
	// Simple case first
	ಆದರೆ (*eap->arg == NUL)
	{
	    ಆದರೆ (quote == 1)
	    {
		ಫಲಿತಾಂಶ = 2;
		ಆದರೆ (buf != ಖಾಲಿ)
		    STRCPY(buf, "''");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಫಲಿತಾಂಶ = 0;
	    ನಿಲ್ಲಿಸು;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	ಆದರೆ ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	ಆಯ್ಕೆ (quote)
	{
	ಸ್ಥಿತಿ 0: // No quoting, no splitting
	    ಫಲಿತಾಂಶ = STRLEN(eap->arg);
	    ಆದರೆ (buf != ಖಾಲಿ)
		STRCPY(buf, eap->arg);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    ಪೂರ್ಣಾಂಕ	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    ಫಲಿತಾಂಶ = STRLEN(eap->arg) + 2;
	    ಗಾಗಿ (p = eap->arg; *p; ++p)
	    {
		ಆದರೆ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    ಆದರೆ (VIM_ISWHITE(p[1]))
			--ಫಲಿತಾಂಶ;
		    ++p;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		     ಆದರೆ (*p == '\\' || *p == '"')
		    ++ಫಲಿತಾಂಶ;
	    }

	    ಆದರೆ (buf != ಖಾಲಿ)
	    {
		*buf++ = '"';
		ಗಾಗಿ (p = eap->arg; *p; ++p)
		{
		    ಆದರೆ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			ಆದರೆ (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			 ಆದರೆ (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    ನಿಲ್ಲಿಸು;
	}
	ಸ್ಥಿತಿ 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    ಆದರೆ (*ವಿಭಜಿಸು_buf == ಖಾಲಿ)
		*ವಿಭಜಿಸು_buf = uc_ವಿಭಜಿಸು_ವಾದಗಳು(eap->arg, ವಿಭಜಿಸು_len);

	    ಫಲಿತಾಂಶ = *ವಿಭಜಿಸು_len;
	    ಆದರೆ (buf != ಖಾಲಿ && ಫಲಿತಾಂಶ != 0)
		STRCPY(buf, *ವಿಭಜಿಸು_buf);

	    ನಿಲ್ಲಿಸು;
	}
	ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ ct_BANG:
	ಫಲಿತಾಂಶ = eap->forceit ? 1 : 0;
	ಆದರೆ (quote)
	    ಫಲಿತಾಂಶ += 2;
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    ಆದರೆ (quote)
		*buf++ = '"';
	    ಆದರೆ (eap->forceit)
		*buf++ = '!';
	    ಆದರೆ (quote)
		*buf = '"';
	}
	ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ ct_LINE1:
    ಸ್ಥಿತಿ ct_LINE2:
    ಸ್ಥಿತಿ ct_RANGE:
    ಸ್ಥಿತಿ ct_ಎಣಿಕೆ:
    {
	ಅಕ್ಷರ ಅಂಕೆ_buf[NUMBUFLEN];
	ದೀರ್ಘ ಅಂಕೆ = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_ಎಣಿಕೆ :
		   (eap->addr_ಎಣಿಕೆ > 0) ? eap->line2 : cmd->uc_def;
	size_t ಅಂಕೆ_len;

	ಅಂಕೆ_len = vim_snprintf(ಅಂಕೆ_buf, ಗಾತ್ರ(ಅಂಕೆ_buf), "%ld", ಅಂಕೆ);
	ಫಲಿತಾಂಶ = ಅಂಕೆ_len;

	ಆದರೆ (quote)
	    ಫಲಿತಾಂಶ += 2;

	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    ಆದರೆ (quote)
		*buf++ = '"';
	    STRCPY(buf, ಅಂಕೆ_buf);
	    buf += ಅಂಕೆ_len;
	    ಆದರೆ (quote)
		*buf = '"';
	}

	ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ ct_MODS:
    {
	ಫಲಿತಾಂಶ = produce_cmdmods(buf, &cmdmod, quote);
	ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ ct_ರಿಜಿಸ್ಟರ್:
	ಫಲಿತಾಂಶ = eap->regname ? 1 : 0;
	ಆದರೆ (quote)
	    ಫಲಿತಾಂಶ += 2;
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    ಆದರೆ (quote)
		*buf++ = '\'';
	    ಆದರೆ (eap->regname)
		*buf++ = eap->regname;
	    ಆದರೆ (quote)
		*buf = '\'';
	}
	ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ ct_LT:
	ಫಲಿತಾಂಶ = 1;
	ಆದರೆ (buf != ಖಾಲಿ)
	    *buf = '<';
	ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
	// Not recognized: just copy the '<' and return -1.
	ಫಲಿತಾಂಶ = (size_t)-1;
	ಆದರೆ (buf != ಖಾಲಿ)
	    *buf = '<';
	ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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

    char_u	*ಆರಂಭ;
    char_u	*ಕೊನೆ = ಖಾಲಿ;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	ವಿಭಜಿಸು_len = 0;
    char_u	*ವಿಭಜಿಸು_buf = ಖಾಲಿ;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    ಪೂರ್ಣಾಂಕ		restore_current_sctx = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		restore_script_ಆವೃತ್ತಿ = 0;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    ಇಲ್ಲದಿದ್ದರೆ
	cmd = USER_CMD_GA(&prevwin_curwin()->w_ಬಫರ್->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = ಖಾಲಿ;
    ಗಾಗಿ (;;)
    {
	p = cmd->uc_rep;    //ಮೂಲ
	q = buf;	    // destination
	totlen = 0;

	ಗಾಗಿ (;;)
	{
	    ಆರಂಭ = vim_strchr(p, '<');
	    ಆದರೆ (ಆರಂಭ != ಖಾಲಿ)
		ಕೊನೆ = vim_strchr(ಆರಂಭ + 1, '>');
	    ಆದರೆ (buf != ಖಾಲಿ)
	    {
		ಗಾಗಿ (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		ಆದರೆ (*ksp == K_SPECIAL
			&& (ಆರಂಭ == ಖಾಲಿ || ksp < ಆರಂಭ || ಕೊನೆ == ಖಾಲಿ)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (ಪೂರ್ಣಾಂಕ)KE_CSI)
#ಆದರೆ_ಅಂತ್ಯ
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    ಆದರೆ (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    ಮುಂದುವರಿ;
		}
	    }

	    // break if no <item> is found
	    ಆದರೆ (ಆರಂಭ == ಖಾಲಿ || ಕೊನೆ == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    // Include the '>'
	    ++ಕೊನೆ;

	    // Take everything up to the '<'
	    len = ಆರಂಭ - p;
	    ಆದರೆ (buf == ಖಾಲಿ)
		totlen += len;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_ಸಂಕೇತ(ಆರಂಭ, ಕೊನೆ - ಆರಂಭ, q, cmd, eap,
			     &ವಿಭಜಿಸು_buf, &ವಿಭಜಿಸು_len);
	    ಆದರೆ (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = ಆರಂಭ + 1;
		len = 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		p = ಕೊನೆ;
	    ಆದರೆ (buf == ಖಾಲಿ)
		totlen += len;
	    ಇಲ್ಲದಿದ್ದರೆ
		q += len;
	}
	ಆದರೆ (buf != ಖಾಲಿ)	    // second time here, finished
	{
	    STRCPY(q, p);
	    ನಿಲ್ಲಿಸು;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    vim_free(ವಿಭಜಿಸು_buf);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಆದರೆ ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_ಆವೃತ್ತಿ = cmd->uc_script_ctx.sc_ಆವೃತ್ತಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	ಆದರೆ (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_ಆವೃತ್ತಿ =
				  SCRIPT_ಅಂಶ(current_sctx.sc_sid)->sn_ಆವೃತ್ತಿ;
	    SCRIPT_ಅಂಶ(current_sctx.sc_sid)->sn_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_VIM9;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    (ಶೂನ್ಯ)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    ಆದರೆ (restore_current_sctx)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (restore_script_ಆವೃತ್ತಿ != 0)
	    SCRIPT_ಅಂಶ(current_sctx.sc_sid)->sn_ಆವೃತ್ತಿ =
							restore_script_ಆವೃತ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(ವಿಭಜಿಸು_buf);
}
