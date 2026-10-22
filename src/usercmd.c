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

#अन्तर्भाव "vim.h"

प्रकारनिर्देश संरचना ucmd
{
    char_u	*uc_name;	// The command name
    माप_प्रकार	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    दीर्घ	uc_def;		// The default value for a range/count
    पूर्णाङ्क		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    पूर्णाङ्क		uc_flags;	// some UC_ flags
    पूर्णाङ्क		uc_compl_opt;	// completion options (UCC_ flags)
#ifdef FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#endif
} ucmd_T;

// List of all user commands.
स्थिर garray_T ucmds = {0, 0, परिमाण(ucmd_T), 4, रिक्त};

// When non-zero it is not allowed to add or remove user commands
स्थिर पूर्णाङ्क ucmd_locked = 0;

#define USER_CMD(i) (&((ucmd_T *)(ucmds.ga_data))[i])
#define USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_data))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
स्थिर keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ENTRY(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ENTRY(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ENTRY(EXPAND_BEHAVE, "behave"),
#यदि defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_BREAKPOINT, "breakpoint"),
#endif
    KEYVALUE_ENTRY(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ENTRY(EXPAND_COLORS, "color"),
    KEYVALUE_ENTRY(EXPAND_COMMANDS, "command"),
    KEYVALUE_ENTRY(EXPAND_COMPILER, "compiler"),
#यदि defined(FEAT_CSCOPE)
    KEYVALUE_ENTRY(EXPAND_CSCOPE, "cscope"),
#endif
#यदि defined(FEAT_EVAL)
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
#यदि defined(FEAT_KEYMAP)
    KEYVALUE_ENTRY(EXPAND_KEYMAP, "keymap"),
#endif
#यदि defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
#यदि defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_SCRIPTNAMES, "scriptnames"),
#endif
    KEYVALUE_ENTRY(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ENTRY(EXPAND_SHELLCMDLINE, "shellcmdline"),
#यदि defined(FEAT_SIGNS)
    KEYVALUE_ENTRY(EXPAND_SIGN, "sign"),
#endif
    KEYVALUE_ENTRY(EXPAND_OWNSYNTAX, "syntax"),
#यदि defined(FEAT_PROFILE)
    KEYVALUE_ENTRY(EXPAND_SYNTIME, "syntime"),
#endif
    KEYVALUE_ENTRY(EXPAND_TAGS, "tag"),
    KEYVALUE_ENTRY(EXPAND_TAGS_LISTFILES, "tag_listfiles"),
    KEYVALUE_ENTRY(EXPAND_USER, "user"),
    KEYVALUE_ENTRY(EXPAND_USER_VARS, "var")
};

प्रकारनिर्देश संरचना
{
    cmd_addr_T कुंजी;
    वर्ण *fullname;
    माप_प्रकार fullnamelen;
    वर्ण *shortname;
    माप_प्रकार shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#define ADDRTYPE_ENTRY(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
स्थिर addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ENTRY(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ENTRY(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ENTRY(ADDR_पंक्तियाँ, "lines", "line"),
    ADDRTYPE_ENTRY(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ENTRY(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ENTRY(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ENTRY(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ENTRY(ADDR_WINDOWS, "windows", "win")
};

स्थिर पूर्णाङ्क cmp_addr_type(ध्रुव शून्य *a, ध्रुव शून्य *b);

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
    पूर्णाङ्क		*full,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    पूर्णाङ्क		*complp) // completion flags or NULL
{
    पूर्णाङ्क		len = (पूर्णाङ्क)(p - eap->cmd);
    पूर्णाङ्क		j, k, matchlen = 0;
    ucmd_T	*uc;
    पूर्णाङ्क		found = FALSE;
    पूर्णाङ्क		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    पूर्णाङ्क		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    प्रति (;;)
    {
	प्रति (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    यावत् (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    यदि (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		यदि (k == len && found && *np != NUL)
		{
		    यदि (gap == &ucmds)
			प्रत्यागम रिक्त;
		    amb_local = TRUE;
		}

		यदि (!found || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    यदि (k == len)
			found = TRUE;
		    अन्यथा
			possible = TRUE;

		    यदि (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    अन्यथा
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (दीर्घ)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    यदि (complp != रिक्त)
			*complp = uc->uc_compl;
#ifdef FEAT_EVAL
		    यदि (xp != रिक्त)
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
		    यदि (k == len && *np == NUL)
		    {
			यदि (full != रिक्त)
			    *full = TRUE;
			amb_local = FALSE;
			भङ्ग;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	यदि (j < gap->ga_len || gap == &ucmds)
	    भङ्ग;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    यदि (amb_local)
    {
	यदि (xp != रिक्त)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	प्रत्यागम रिक्त;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    यदि (found || possible)
	प्रत्यागम p + (matchlen - len);
    प्रत्यागम p;
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
    यावत् (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	यदि (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    यदि (p == रिक्त)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_pattern = arg;
		प्रत्यागम रिक्त;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    यदि (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_pattern = p + 1;
	    }
	    अन्यथा यदि (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_pattern = p + 1;
		प्रति (char_u *c = p + 1; *c != NUL; ++c)
		    यदि (*c == ',')
			xp->xp_pattern = c + 1;
	    }
	    अन्यथा यदि (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_pattern = p + 1;
	    }
	    अन्यथा यदि (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_pattern = p + 1;
	    }
	    प्रत्यागम रिक्त;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    यदि (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_pattern = arg;
	प्रत्यागम रिक्त;
    }

    // And finally comes a normal command
    प्रत्यागम skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
set_context_in_user_cmdarg(
	char_u		*cmd UNUSED,
	char_u		*arg,
	दीर्घ		argt,
	पूर्णाङ्क		context,
	expand_T	*xp,
	पूर्णाङ्क		forceit)
{
    char_u	*p;

    यदि (context == EXPAND_NOTHING)
	प्रत्यागम रिक्त;

    यदि (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	प्रत्यागम रिक्त;
    }

#ifdef FEAT_MENU
    यदि (context == EXPAND_MENUS)
	प्रत्यागम set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif
    यदि (context == EXPAND_COMMANDS)
	प्रत्यागम arg;
    यदि (context == EXPAND_MAPPINGS)
	प्रत्यागम set_context_in_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    यदि (!(argt & EX_ARGSPACE))
    {
	p = arg;
	यावत् (*p)
	{
	    यदि (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    अन्यथा यदि (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_pattern = arg;
    xp->xp_context = context;

    प्रत्यागम रिक्त;
}

    char_u *
expand_user_command_name(पूर्णाङ्क idx)
{
    प्रत्यागम get_user_commands(रिक्त, idx - (पूर्णाङ्क)CMD_SIZE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_buffer;

    यदि (idx < buf->b_ucmds.ga_len)
	प्रत्यागम USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    यदि (idx < ucmds.ga_len)
    {
	पूर्णाङ्क	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	प्रति (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    यदि (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		प्रत्यागम (char_u *)"";
	प्रत्यागम name;
    }
    प्रत्यागम रिक्त;
}

#ifdef FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(पूर्णाङ्क idx, पूर्णाङ्क cmdidx)
{
    यदि (cmdidx == CMD_USER && idx < ucmds.ga_len)
	प्रत्यागम USER_CMD(idx)->uc_name;
    यदि (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_buffer;

	यदि (idx < buf->b_ucmds.ga_len)
	    प्रत्यागम USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    प्रत्यागम रिक्त;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    पूर्णाङ्क
user_command_info(पूर्णाङ्क idx, पूर्णाङ्क cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_buffer->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    यदि (idx >= gap->ga_len)
	प्रत्यागम FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_add_string(d, "name", uc->uc_name);
    dict_add_string(d, "kind", (char_u *)"user");
    dict_add_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_add_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_to_str(uc->uc_compl, uc->uc_compl_arg);
    dict_add_string(d, "complete", compl == रिक्त ? (char_u *)"" : compl);
    vim_free(compl);
    dict_add_string(d, "definition", uc->uc_rep);
    dict_add_संख्या(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_add_संख्या(d, "lnum", uc->uc_script_ctx.sc_lnum);
    प्रत्यागम OK;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    यदि (idx < 0 || idx >= (पूर्णाङ्क)ARRAY_LENGTH(addr_type_complete_tab))
	प्रत्यागम रिक्त;
    प्रत्यागम (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    स्थिर वर्ण *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    यदि (idx < 0 || idx >= (पूर्णाङ्क)ARRAY_LENGTH(user_cmd_flags))
	प्रत्यागम रिक्त;
    प्रत्यागम (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    स्थिर वर्ण *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    यदि (idx < 0 || idx >= (पूर्णाङ्क)ARRAY_LENGTH(user_cmd_nargs))
	प्रत्यागम रिक्त;
    प्रत्यागम (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    यदि (idx < 0 || idx >= (पूर्णाङ्क)ARRAY_LENGTH(command_complete_tab))
	प्रत्यागम रिक्त;
    प्रत्यागम command_complete_tab[idx].मूल्य.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
स्थिर वर्ण *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    यदि (idx < 0 || idx >= (पूर्णाङ्क)ARRAY_LENGTH(user_cmd_completeopt_tab))
	प्रत्यागम रिक्त;
    प्रत्यागम (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    स्थिर keyvalue_T *
get_commandtype(पूर्णाङ्क expand)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(command_complete_tab); ++i)
	यदि (command_complete_tab[i].कुंजी == expand)
	    प्रत्यागम &command_complete_tab[i];

    प्रत्यागम रिक्त;
}

#ifdef FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_to_str(पूर्णाङ्क expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    यदि (kv == रिक्त || kv->मूल्य.string == रिक्त)
	प्रत्यागम रिक्त;

    cmd_compl = kv->मूल्य.string;
    यदि (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*buffer;

	buffer = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	यदि (buffer == रिक्त)
	    प्रत्यागम रिक्त;
	प्र_लिखो((वर्ण *)buffer, "%s,%s", cmd_compl, compl_arg);
	प्रत्यागम buffer;
    }

    प्रत्यागम vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    पूर्णाङ्क
cmdcomplete_str_to_type(char_u *complete_str)
{
    keyvalue_T target;
    keyvalue_T *entry;
    स्थिर keyvalue_T *last_entry = रिक्त;	// cached result

    यदि (STRNCMP(complete_str, "custom,", 7) == 0)
	प्रत्यागम EXPAND_USER_DEFINED;
    यदि (STRNCMP(complete_str, "customlist,", 11) == 0)
	प्रत्यागम EXPAND_USER_LIST;

    target.कुंजी = 0;
    target.मूल्य.string = complete_str;
    target.मूल्य.length = 0;			// not used, see cmp_keyvalue_value()

    यदि (last_entry != रिक्त && cmp_keyvalue_मूल्य(&target, last_entry) == 0)
	entry = last_entry;
    अन्यथा
    {
	entry = (keyvalue_T *)द्वा_खोज(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    परिमाण(command_complete_tab[0]),
	    cmp_keyvalue_मूल्य);
	यदि (entry == रिक्त)
	    प्रत्यागम EXPAND_NOTHING;

	last_entry = entry;
    }

    प्रत्यागम entry->कुंजी;
}
#endif

/*
 * List user commands starting with "name[name_len]".
 */
    स्थिर शून्य
uc_list(char_u *name, माप_प्रकार name_len)
{
    पूर्णाङ्क		i, j;
    पूर्णाङ्क		found = FALSE;
    ucmd_T	*cmd;
    पूर्णाङ्क		len;
    पूर्णाङ्क		over;
    दीर्घ	a;
    garray_T	*gap;
    keyvalue_T	*entry;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    प्रति (;;)
    {
	प्रति (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (दीर्घ)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    यदि (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || message_filtered(cmd->uc_name))
		अनुवर्त;

	    // Put out the title first time
	    यदि (!found)
		संदेश_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    found = TRUE;
	    संदेश_putchar('\n');
	    यदि (got_int)
		भङ्ग;

	    // Special cases
	    len = 4;
	    यदि (a & EX_BANG)
	    {
		संदेश_putchar('!');
		--len;
	    }
	    यदि (a & EX_REGSTR)
	    {
		संदेश_putchar('"');
		--len;
	    }
	    यदि (gap != &ucmds)
	    {
		संदेश_putchar('b');
		--len;
	    }
	    यदि (a & EX_TRLBAR)
	    {
		संदेश_putchar('|');
		--len;
	    }
	    यदि (len != 0)
		संदेश_puts(&"    "[4 - len]);

	    संदेश_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (पूर्णाङ्क)cmd->uc_namelen + 4;

	    यदि (len < 21)
	    {
		// Field padding spaces   12345678901234567
		स्थिर वर्ण spaces[18] = "                 ";
		संदेश_puts(&spaces[len - 4]);
		len = 21;
	    }
	    संदेश_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    विकल्प ((पूर्णाङ्क)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		पक्ष 0:				IObuff[len++] = '0'; भङ्ग;
		पक्ष (EX_EXTRA):		IObuff[len++] = '*'; भङ्ग;
		पक्ष (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; भङ्ग;
		पक्ष (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; भङ्ग;
		पक्ष (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; भङ्ग;
		पक्ष (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; भङ्ग;
	    }

	    कुरु
	    {
		IObuff[len++] = ' ';
	    } यावत् (len < 5 - over);

	    // Address / Range
	    यदि (a & (EX_RANGE|EX_गिनती))
	    {
		यदि (a & EX_गिनती)
		{
		    // -count=N
		    len += vim_snprintf((वर्ण *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		अन्यथा यदि (a & EX_DFLALL)
		    IObuff[len++] = '%';
		अन्यथा यदि (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((वर्ण *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		अन्यथा
		    IObuff[len++] = '.';
	    }

	    कुरु
	    {
		IObuff[len++] = ' ';
	    } यावत् (len < 8 - over);

	    // Address Type
	    प्रति (j = 0; j < (पूर्णाङ्क)ARRAY_LENGTH(addr_type_complete_tab); ++j)
		यदि (addr_type_complete_tab[j].कुंजी != ADDR_पंक्तियाँ
			&& addr_type_complete_tab[j].कुंजी == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (पूर्णाङ्क)addr_type_complete_tab[j].shortnamelen;
		    भङ्ग;
		}

	    कुरु
	    {
		IObuff[len++] = ' ';
	    } यावत् (len < 13 - over);

	    // Completion
	    entry = get_commandtype(cmd->uc_compl);
	    यदि (entry != रिक्त)
	    {
		STRCPY(IObuff + len, entry->मूल्य.string);
		len += (पूर्णाङ्क)entry->मूल्य.length;
#ifdef FEAT_EVAL
		यदि (p_verbose > 0 && cmd->uc_compl_arg != रिक्त)
		{
		    माप_प्रकार uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    यदि (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (पूर्णाङ्क)uc_compl_arglen;
		    }
		}
		यदि (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((वर्ण *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#endif
	    }

	    कुरु
	    {
		IObuff[len++] = ' ';
	    } यावत् (len < 25 - over);

	    IObuff[len] = NUL;
	    संदेश_outtrans(IObuff);

	    संदेश_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ifdef FEAT_EVAL
	    यदि (p_verbose > 0)
		last_set_संदेश(cmd->uc_script_ctx);
#endif
	    out_flush();
	    ui_breakcheck();
	    यदि (got_int)
		भङ्ग;
	}
	यदि (gap == &ucmds || i < gap->ga_len)
	    भङ्ग;
	gap = &ucmds;
    }

    यदि (!found)
	संदेश(_("No user-defined commands found"));

    --ucmd_locked;
}

    वर्ण *
uc_fun_cmd(शून्य)
{
    स्थिर char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    पूर्णाङ्क		i;

    प्रति (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    प्रत्यागम (वर्ण *)IObuff;
}

/*
 * Parse address type argument
 */
    स्थिर पूर्णाङ्क
parse_addr_type_arg(
    char_u	*मूल्य,
    पूर्णाङ्क		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T target;
    addrtype_T *entry;
    स्थिर addrtype_T *last_entry;	// cached result

    target.कुंजी = 0;
    target.fullname = (वर्ण *)मूल्य;
    target.fullnamelen = vallen;

    यदि (last_entry != रिक्त && cmp_addr_type(&target, last_entry) == 0)
	entry = last_entry;
    अन्यथा
    {
	entry = (addrtype_T *)द्वा_खोज(&target,
	    &addr_type_complete_tab,
	    ARRAY_LENGTH(addr_type_complete_tab),
	    परिमाण(addr_type_complete_tab[0]),
	    cmp_addr_type);
	यदि (entry == रिक्त)
	{
	    पूर्णाङ्क i;
	    char_u	*err = मूल्य;

	    प्रति (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_address_type_मूल्य_str), err);
	    प्रत्यागम FAIL;
	}

	last_entry = entry;
    }

    *addr_type_arg = entry->कुंजी;

    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
cmp_addr_type(ध्रुव शून्य *a, ध्रुव शून्य *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    प्रत्यागम STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    पूर्णाङ्क
parse_compl_arg(
    char_u	*मूल्य,
    पूर्णाङ्क		vallen,
    पूर्णाङ्क		*complp,
    दीर्घ	*argt,
    char_u	**compl_arg UNUSED)
{
    char_u	*arg = रिक्त;
#यदि defined(FEAT_EVAL)
    माप_प्रकार	arglen = 0;
#endif
    पूर्णाङ्क		i;
    पूर्णाङ्क		valend = vallen;
    keyvalue_T	target;
    keyvalue_T	*entry;
    स्थिर keyvalue_T	*last_entry = रिक्त;	    // cached result

    // Look for any argument part - which is the part after any ','
    प्रति (i = 0; i < vallen; ++i)
    {
	यदि (मूल्य[i] == ',')
	{
	    arg = &मूल्य[i + 1];
#यदि defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#endif
	    valend = i;
	    भङ्ग;
	}
    }

    target.कुंजी = 0;
    target.मूल्य.string = मूल्य;
    target.मूल्य.length = valend;

    यदि (last_entry != रिक्त && cmp_keyvalue_मूल्य_n(&target, last_entry) == 0)
	entry = last_entry;
    अन्यथा
    {
	entry = (keyvalue_T *)द्वा_खोज(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    परिमाण(command_complete_tab[0]),
	    cmp_keyvalue_मूल्य_n);
	यदि (entry == रिक्त)
	{
	    semsg(_(e_invalid_complete_मूल्य_str), मूल्य);
	    प्रत्यागम FAIL;
	}

	last_entry = entry;
    }

    *complp = entry->कुंजी;
    यदि (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    अन्यथा यदि (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    यदि (
#यदि defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#endif
								arg != रिक्त)
    {
	emsg(_(e_completion_argument_only_allowed_for_custom_completion));
	प्रत्यागम FAIL;
    }

#यदि defined(FEAT_EVAL)
    यदि ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == रिक्त)
    {
	emsg(_(e_custom_completion_requires_function_argument));
	प्रत्यागम FAIL;
    }

    यदि (arg != रिक्त)
	*compl_arg = vim_strnsave(arg, arglen);
#endif

    प्रत्यागम OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    स्थिर पूर्णाङ्क
parse_completeopt_arg(char_u *मूल्य, पूर्णाङ्क vallen, पूर्णाङ्क *compl_opt)
{
    char_u  *p = मूल्य;
    char_u  *end = मूल्य + vallen;
    पूर्णाङ्क	    flags = 0;

    यदि (vallen == 0)
    {
	semsg(_(e_argument_required_for_str), "-completeopt");
	प्रत्यागम FAIL;
    }

    यावत् (p < end)
    {
	char_u	*comma;
	माप_प्रकार	itemlen;
	पूर्णाङ्क	matched = FALSE;
	पूर्णाङ्क	i;

	comma = vim_strchr(p, ',');
	यदि (comma == रिक्त || comma > end)
	    itemlen = (माप_प्रकार)(end - p);
	अन्यथा
	    itemlen = (माप_प्रकार)(comma - p);

	यदि (itemlen == 0)
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), "completeopt");
	    प्रत्यागम FAIL;
	}

	प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(user_cmd_completeopt_tab); ++i)
	{
	    वर्ण *name = user_cmd_completeopt_tab[i];

	    यदि (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		यदि (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		भङ्ग;
	    }
	}
	यदि (!matched)
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), "completeopt");
	    प्रत्यागम FAIL;
	}

	p += itemlen;
	यदि (p < end && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    प्रत्यागम OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    स्थिर पूर्णाङ्क
uc_scan_attr(
    char_u	*attr,
    माप_प्रकार	len,
    दीर्घ	*argt,
    दीर्घ	*def,
    पूर्णाङ्क		*flags,
    पूर्णाङ्क		*complp,
    char_u	**compl_arg,
    पूर्णाङ्क		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    यदि (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	प्रत्यागम FAIL;
    }

    // First, try the simple attributes (no arguments)
    यदि (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    अन्यथा यदि (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_BUFFER;
    अन्यथा यदि (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    अन्यथा यदि (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    अन्यथा यदि (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    अन्यथा
    {
	पूर्णाङ्क	i;
	char_u	*val = रिक्त;
	माप_प्रकार	vallen = 0;
	माप_प्रकार	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	प्रति (i = 0; i < (पूर्णाङ्क)len; ++i)
	{
	    यदि (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		भङ्ग;
	    }
	}

	यदि (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    यदि (vallen == 1)
	    {
		यदि (*val == '0')
		    // Do nothing - this is the default
		    ;
		अन्यथा यदि (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		अन्यथा यदि (*val == '*')
		    *argt |= EX_EXTRA;
		अन्यथा यदि (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		अन्यथा यदि (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		अन्यथा यदि (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		अन्यथा
		    गच्छ wrong_nargs;
	    }
	    अन्यथा
	    {
wrong_nargs:
		emsg(_(e_invalid_संख्या_of_arguments));
		प्रत्यागम FAIL;
	    }
	}
	अन्यथा यदि (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    यदि (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    अन्यथा यदि (val != रिक्त)
	    {
		p = val;
		यदि (*def >= 0)
		{
two_गिनती:
		    emsg(_(e_गिनती_cannot_be_specified_twice));
		    प्रत्यागम FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		यदि (p != val + vallen || vallen == 0)
		{
invalid_गिनती:
		    emsg(_(e_invalid_default_मूल्य_for_गिनती));
		    प्रत्यागम FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    यदि (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_पंक्तियाँ;
	}
	अन्यथा यदि (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_गिनती | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    यदि (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    यदि (val != रिक्त)
	    {
		p = val;
		यदि (*def >= 0)
		    गच्छ two_गिनती;

		*def = getdigits(&p);

		यदि (p != val + vallen)
		    गच्छ invalid_गिनती;
	    }

	    यदि (*def < 0)
		*def = 0;
	}
	अन्यथा यदि (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    यदि (val == रिक्त)
	    {
		semsg(_(e_argument_required_for_str), "-complete");
		प्रत्यागम FAIL;
	    }

	    यदि (parse_compl_arg(val, (पूर्णाङ्क)vallen, complp, argt, compl_arg)
								      == FAIL)
		प्रत्यागम FAIL;
	}
	अन्यथा यदि (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    यदि (val == रिक्त)
	    {
		semsg(_(e_argument_required_for_str), "-completeopt");
		प्रत्यागम FAIL;
	    }

	    यदि (parse_completeopt_arg(val, (पूर्णाङ्क)vallen, compl_opt) == FAIL)
		प्रत्यागम FAIL;
	}
	अन्यथा यदि (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    यदि (val == रिक्त)
	    {
		semsg(_(e_argument_required_for_str), "-addr");
		प्रत्यागम FAIL;
	    }
	    यदि (parse_addr_type_arg(val, (पूर्णाङ्क)vallen, addr_type_arg) == FAIL)
		प्रत्यागम FAIL;
	    यदि (*addr_type_arg != ADDR_पंक्तियाँ)
		*argt |= EX_ZEROR;
	}
	अन्यथा
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    प्रत्यागम FAIL;
	}
    }

    प्रत्यागम OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    स्थिर पूर्णाङ्क
uc_add_command(
    char_u	*name,
    माप_प्रकार	name_len,
    char_u	*rep,
    दीर्घ	argt,
    दीर्घ	def,
    पूर्णाङ्क		flags,
    पूर्णाङ्क		compl,
    char_u	*compl_arg UNUSED,
    पूर्णाङ्क		compl_opt,
    cmd_addr_T	addr_type,
    पूर्णाङ्क		force)
{
    ucmd_T	*cmd = रिक्त;
    char_u	*p;
    पूर्णाङ्क		i;
    पूर्णाङ्क		cmp = 1;
    char_u	*rep_buf = रिक्त;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, रिक्त);
    यदि (rep_buf == रिक्त)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	यदि (rep_buf == रिक्त)
	    प्रत्यागम FAIL;
    }

    // get address of growarray: global or in curbuf
    यदि (flags & UC_BUFFER)
    {
	gap = &curbuf->b_ucmds;
	यदि (gap->ga_itemsize == 0)
	    ga_init2(gap, परिमाण(ucmd_T), 4);
    }
    अन्यथा
	gap = &ucmds;

    // Search for the command in the already defined commands.
    प्रति (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	यदि (cmp == 0)
	{
	    यदि (name_len < cmd->uc_namelen)
		cmp = -1;
	    अन्यथा यदि (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	यदि (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    यदि (!force
#ifdef FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#endif
		    )
	    {
		semsg(_(e_command_already_exists_add_bang_to_replace_it_str),
									 name);
		गच्छ fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#यदि defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#endif
	    भङ्ग;
	}

	// Stop as soon as we pass the name to add
	यदि (cmp < 0)
	    भङ्ग;
    }

    // Extend the array unless we're replacing an existing command
    यदि (cmp != 0)
    {
	यदि (ga_grow(gap, 1) == FAIL)
	    गच्छ fail;
	यदि ((p = vim_strnsave(name, name_len)) == रिक्त)
	    गच्छ fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * परिमाण(ucmd_T));

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
    यदि (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ifdef FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#endif
    cmd->uc_addr_type = addr_type;

    प्रत्यागम OK;

fail:
    vim_free(rep_buf);
#यदि defined(FEAT_EVAL)
    vim_free(compl_arg);
#endif
    प्रत्यागम FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_block(exarg_T *eap, char_u *p, char_u **tofree, पूर्णाङ्क *flags)
{
    char_u *retp = p;
    char_u *block = find_cmd_block_start(p);

    यदि (block != रिक्त && eap->ea_getline != रिक्त)
    {
	garray_T    ga;
	char_u	    *पंक्ति = रिक्त;

	ga_init2(&ga, परिमाण(char_u *), 10);
	यदि (ga_copy_string(&ga, p) == FAIL)
	    प्रत्यागम retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	यदि (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    प्रति (;;)
	    {
		vim_free(पंक्ति);
		यदि ((पंक्ति = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == रिक्त)
		{
		    emsg(_(e_missing_rcurly));
		    भङ्ग;
		}
		यदि (ga_copy_string(&ga, पंक्ति) == FAIL)
		    भङ्ग;
		यदि (*skipwhite(पंक्ति) == '}')
		    भङ्ग;
	    }
	vim_free(पंक्ति);
	retp = *tofree = ga_concat_strings(&ga, "\n");
	यदि (retp == रिक्त)
	    retp = p;
	ga_clear_strings(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	यदि (block == p)
	    *flags |= UC_VIM9;
    }
    प्रत्यागम retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    शून्य
skip_cmd_block(exarg_T *eap)
{
    char_u	*tofree = रिक्त;
    पूर्णाङ्क		flags = 0;

    (शून्य)may_get_cmd_block(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    शून्य
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*end;
    char_u	*p;
    दीर्घ	argt = 0;
    दीर्घ	def = -1;
    पूर्णाङ्क		flags = 0;
    पूर्णाङ्क		compl = EXPAND_NOTHING;
    char_u	*compl_arg = रिक्त;
    पूर्णाङ्क		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    पूर्णाङ्क		has_attr = (eap->arg[0] == '-');
    पूर्णाङ्क		name_len;

    यदि (eap->skip)
    {
	skip_cmd_block(eap);
	प्रत्यागम;
    }

    p = eap->arg;

    // Check for attributes
    यावत् (*p == '-')
    {
	++p;
	end = skiptowhite(p);
	यदि (uc_scan_attr(p, end - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    गच्छ theend;
	p = skipwhite(end);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    यदि (ASCII_ISALPHA(*p))
	यावत् (ASCII_ISALNUM(*p))
	    ++p;
    यदि (!ends_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	गच्छ theend;
    }
    end = p;
    name_len = (पूर्णाङ्क)(end - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(end);
    यदि (!has_attr && ends_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    अन्यथा यदि (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_start_with_an_uppercase_letter));
    }
    अन्यथा यदि ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_used_for_user_defined_command));
    }
    अन्यथा यदि (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	यदि (in_vim9script())
	    emsg(_(e_complete_used_without_allowing_arguments));
	अन्यथा
	    give_warning_with_source(
		       (char_u *)_(e_complete_used_without_allowing_arguments),
								   TRUE, TRUE);
    }
    अन्यथा यदि ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_used_with_nargs_underscore));
    }
    अन्यथा
    {
	char_u *tofree = रिक्त;

	p = may_get_cmd_block(eap, p, &tofree, &flags);

	uc_add_command(name, end - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	प्रत्यागम;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    शून्य
ex_comclear(exarg_T *eap UNUSED)
{
    uc_clear(&ucmds);
    यदि (curbuf != रिक्त)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    स्थिर पूर्णाङ्क
is_ucmd_locked(शून्य)
{
    यदि (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    शून्य
uc_clear(garray_T *gap)
{
    पूर्णाङ्क		i;
    ucmd_T	*cmd;

    यदि (is_ucmd_locked())
	प्रत्यागम;

    प्रति (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#यदि defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#endif
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    शून्य
ex_delcommand(exarg_T *eap)
{
    पूर्णाङ्क		i = 0;
    ucmd_T	*cmd = रिक्त;
    पूर्णाङ्क		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    पूर्णाङ्क		buffer_only = FALSE;

    यदि (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	buffer_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    प्रति (;;)
    {
	प्रति (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    यदि (res <= 0)
		भङ्ग;
	}
	यदि (gap == &ucmds || res == 0 || buffer_only)
	    भङ्ग;
	gap = &ucmds;
    }

    यदि (res != 0)
    {
	semsg(_(buffer_only
		    ? e_no_such_user_defined_command_in_current_buffer_str
		    : e_no_such_user_defined_command_str), arg);
	प्रत्यागम;
    }

    यदि (is_ucmd_locked())
	प्रत्यागम;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#यदि defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#endif

    --gap->ga_len;

    यदि (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * परिमाण(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    स्थिर char_u *
uc_split_args(char_u *arg, माप_प्रकार *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    पूर्णाङ्क len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    यावत् (*p)
    {
	यदि (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	अन्यथा यदि (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	अन्यथा यदि (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	अन्यथा यदि (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    यदि (*p == NUL)
		भङ्ग;
	    len += 4; // ", "
	}
	अन्यथा
	{
	    पूर्णाङ्क charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    यदि (buf == रिक्त)
    {
	*lenp = 0;
	प्रत्यागम buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    यावत् (*p)
    {
	यदि (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	अन्यथा यदि (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	अन्यथा यदि (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	अन्यथा यदि (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    यदि (*p == NUL)
		भङ्ग;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	अन्यथा
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    प्रत्यागम buf;
}

    स्थिर माप_प्रकार
add_cmd_modifier(
    char_u	*buf,
    माप_प्रकार	buflen,
    वर्ण	*mod_str,
    माप_प्रकार	mod_strlen,
    पूर्णाङ्क		*multi_mods)
{
    यदि (buf != रिक्त)
    {
	यदि (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    यदि (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    अन्यथा
	*multi_mods = 1;

    प्रत्यागम mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    माप_प्रकार
add_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, पूर्णाङ्क *multi_mods)
{
    माप_प्रकार buflen = 0;

    // :aboveleft and :leftabove
    यदि (cmod->cmod_split & WSP_ABOVE)
	buflen += add_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    यदि (cmod->cmod_split & WSP_BELOW)
	buflen += add_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    यदि (cmod->cmod_split & WSP_BOT)
	buflen += add_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    यदि (cmod->cmod_tab > 0)
    {
	पूर्णाङ्क tabnr = cmod->cmod_tab - 1;

	यदि (tabnr == tabpage_index(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += add_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	अन्यथा
	{
	    वर्ण tab_buf[NUMBUFLEN + 3];
	    माप_प्रकार tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, परिमाण(tab_buf), "%dtab", tabnr);
	    buflen += add_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    यदि (cmod->cmod_split & WSP_शीर्ष)
	buflen += add_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    यदि (cmod->cmod_split & WSP_VERT)
	buflen += add_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    यदि (cmod->cmod_split & WSP_HOR)
	buflen += add_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    प्रत्यागम buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    माप_प्रकार
produce_cmdmods(char_u *buf, cmdmod_T *cmod, पूर्णाङ्क quote)
{
    माप_प्रकार  buflen = 0;
    पूर्णाङ्क	    multi_mods = 0;
    पूर्णाङ्क	    i;
    स्थिर keyvalue_T mod_entry_tab[] =
    {
#ifdef FEAT_BROWSE_CMD
	KEYVALUE_ENTRY(CMOD_BROWSE, "browse"),
#endif
#यदि defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
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

    यदि (quote)
    {
	++buflen;
	यदि (buf != रिक्त)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    अन्यथा
    यदि (buf != रिक्त)
	*buf = NUL;

    // the modifiers that are simple flags
    प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(mod_entry_tab); ++i)
	यदि (cmod->cmod_flags & mod_entry_tab[i].कुंजी)
	    buflen += add_cmd_modifier(buf, buflen,
		    (वर्ण *)mod_entry_tab[i].मूल्य.string,
		    mod_entry_tab[i].मूल्य.length, &multi_mods);

    // :silent
    यदि (cmod->cmod_flags & CMOD_SILENT)
    {
	यदि (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += add_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	अन्यथा
	    buflen += add_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    यदि (cmod->cmod_verbose > 0)
    {
	पूर्णाङ्क verbose_मूल्य = cmod->cmod_verbose - 1;

	यदि (verbose_मूल्य == 1)
	    buflen += add_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	अन्यथा
	{
	    वर्ण verbose_buf[NUMBUFLEN];
	    माप_प्रकार verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, परिमाण(verbose_buf), "%dverbose", verbose_मूल्य);
	    buflen += add_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += add_win_cmd_modifiers((buf == रिक्त) ? रिक्त : buf + buflen, cmod, &multi_mods);

    यदि (quote)
    {
	यदि (buf == रिक्त)
	    ++buflen;
	अन्यथा
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    प्रत्यागम buflen;
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
    स्थिर माप_प्रकार
uc_check_code(
    char_u	*code,
    माप_प्रकार	len,
    char_u	*buf,
    ucmd_T	*cmd,		// the user command we're expanding
    exarg_T	*eap,		// ex arguments
    char_u	**split_buf,
    माप_प्रकार	*split_len)
{
    माप_प्रकार	result = 0;
    char_u	*p = code + 1;
    माप_प्रकार	l = len - 2;
    पूर्णाङ्क		quote = 0;
    गणना {
	ct_ARGS,
	ct_BANG,
	ct_गिनती,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_REGISTER,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    यदि ((vim_strchr((char_u *)"qQfF", *p) != रिक्त) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    यदि (l <= 1)
	type = ct_NONE;
    अन्यथा यदि (STRNICMP(p, "args>", l) == 0)
	type = ct_ARGS;
    अन्यथा यदि (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    अन्यथा यदि (STRNICMP(p, "count>", l) == 0)
	type = ct_गिनती;
    अन्यथा यदि (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    अन्यथा यदि (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    अन्यथा यदि (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    अन्यथा यदि (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    अन्यथा यदि (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_REGISTER;
    अन्यथा यदि (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    विकल्प (type)
    {
    पक्ष ct_ARGS:
	// Simple case first
	यदि (*eap->arg == NUL)
	{
	    यदि (quote == 1)
	    {
		result = 2;
		यदि (buf != रिक्त)
		    STRCPY(buf, "''");
	    }
	    अन्यथा
		result = 0;
	    भङ्ग;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	यदि ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	विकल्प (quote)
	{
	पक्ष 0: // No quoting, no splitting
	    result = STRLEN(eap->arg);
	    यदि (buf != रिक्त)
		STRCPY(buf, eap->arg);
	    भङ्ग;
	पक्ष 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    पूर्णाङ्क	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    result = STRLEN(eap->arg) + 2;
	    प्रति (p = eap->arg; *p; ++p)
	    {
		यदि (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		अन्यथा यदि (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    यदि (VIM_ISWHITE(p[1]))
			--result;
		    ++p;
		}
		अन्यथा
		     यदि (*p == '\\' || *p == '"')
		    ++result;
	    }

	    यदि (buf != रिक्त)
	    {
		*buf++ = '"';
		प्रति (p = eap->arg; *p; ++p)
		{
		    यदि (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    अन्यथा यदि (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			यदि (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    अन्यथा
			 यदि (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    भङ्ग;
	}
	पक्ष 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    यदि (*split_buf == रिक्त)
		*split_buf = uc_split_args(eap->arg, split_len);

	    result = *split_len;
	    यदि (buf != रिक्त && result != 0)
		STRCPY(buf, *split_buf);

	    भङ्ग;
	}
	भङ्ग;

    पक्ष ct_BANG:
	result = eap->forceit ? 1 : 0;
	यदि (quote)
	    result += 2;
	यदि (buf != रिक्त)
	{
	    यदि (quote)
		*buf++ = '"';
	    यदि (eap->forceit)
		*buf++ = '!';
	    यदि (quote)
		*buf = '"';
	}
	भङ्ग;

    पक्ष ct_LINE1:
    पक्ष ct_LINE2:
    पक्ष ct_RANGE:
    पक्ष ct_गिनती:
    {
	वर्ण अंक_buf[NUMBUFLEN];
	दीर्घ अंक = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_गिनती :
		   (eap->addr_गिनती > 0) ? eap->line2 : cmd->uc_def;
	माप_प्रकार अंक_len;

	अंक_len = vim_snprintf(अंक_buf, परिमाण(अंक_buf), "%ld", अंक);
	result = अंक_len;

	यदि (quote)
	    result += 2;

	यदि (buf != रिक्त)
	{
	    यदि (quote)
		*buf++ = '"';
	    STRCPY(buf, अंक_buf);
	    buf += अंक_len;
	    यदि (quote)
		*buf = '"';
	}

	भङ्ग;
    }

    पक्ष ct_MODS:
    {
	result = produce_cmdmods(buf, &cmdmod, quote);
	भङ्ग;
    }

    पक्ष ct_REGISTER:
	result = eap->regname ? 1 : 0;
	यदि (quote)
	    result += 2;
	यदि (buf != रिक्त)
	{
	    यदि (quote)
		*buf++ = '\'';
	    यदि (eap->regname)
		*buf++ = eap->regname;
	    यदि (quote)
		*buf = '\'';
	}
	भङ्ग;

    पक्ष ct_LT:
	result = 1;
	यदि (buf != रिक्त)
	    *buf = '<';
	भङ्ग;

    स्वभाव:
	// Not recognized: just copy the '<' and return -1.
	result = (माप_प्रकार)-1;
	यदि (buf != रिक्त)
	    *buf = '<';
	भङ्ग;
    }

    प्रत्यागम result;
}

/*
 * Execute a user defined command.
 */
    शून्य
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*start;
    char_u	*end = रिक्त;
    char_u	*ksp;
    माप_प्रकार	len, totlen;

    माप_प्रकार	split_len = 0;
    char_u	*split_buf = रिक्त;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    पूर्णाङ्क		restore_current_sctx = FALSE;
#ifdef FEAT_EVAL
    पूर्णाङ्क		restore_script_version = 0;
#endif

    यदि (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    अन्यथा
	cmd = USER_CMD_GA(&prevwin_curwin()->w_buffer->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = रिक्त;
    प्रति (;;)
    {
	p = cmd->uc_rep;    // source
	q = buf;	    // destination
	totlen = 0;

	प्रति (;;)
	{
	    start = vim_strchr(p, '<');
	    यदि (start != रिक्त)
		end = vim_strchr(start + 1, '>');
	    यदि (buf != रिक्त)
	    {
		प्रति (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		यदि (*ksp == K_SPECIAL
			&& (start == रिक्त || ksp < start || end == रिक्त)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ifdef FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (पूर्णाङ्क)KE_CSI)
#endif
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    यदि (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    अनुवर्त;
		}
	    }

	    // break if no <item> is found
	    यदि (start == रिक्त || end == रिक्त)
		भङ्ग;

	    // Include the '>'
	    ++end;

	    // Take everything up to the '<'
	    len = start - p;
	    यदि (buf == रिक्त)
		totlen += len;
	    अन्यथा
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_code(start, end - start, q, cmd, eap,
			     &split_buf, &split_len);
	    यदि (len == (माप_प्रकार)-1)
	    {
		// no match, continue after '<'
		p = start + 1;
		len = 1;
	    }
	    अन्यथा
		p = end;
	    यदि (buf == रिक्त)
		totlen += len;
	    अन्यथा
		q += len;
	}
	यदि (buf != रिक्त)	    // second time here, finished
	{
	    STRCPY(q, p);
	    भङ्ग;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	यदि (buf == रिक्त)
	{
	    vim_free(split_buf);
	    प्रत्यागम;
	}
    }

    यदि ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_version = cmd->uc_script_ctx.sc_version;
#ifdef FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	यदि (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_version =
				  SCRIPT_ITEM(current_sctx.sc_sid)->sn_version;
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = SCRIPT_VERSION_VIM9;
	}
#endif
    }

    (शून्य)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    यदि (restore_current_sctx)
    {
#ifdef FEAT_EVAL
	यदि (restore_script_version != 0)
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version =
							restore_script_version;
#endif
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(split_buf);
}
