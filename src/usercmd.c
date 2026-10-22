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

प्रकारव्याख्या संरचना ucmd
{
    char_u	*uc_name;	// The command name
    माप_प्रकार	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    दीर्घ	uc_def;		// The default value for a range/count
    पूर्णांक		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    पूर्णांक		uc_flags;	// some UC_ flags
    पूर्णांक		uc_compl_opt;	// completion options (UCC_ flags)
#ifdef FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#endif
} ucmd_T;

// List of all user commands.
स्थिर garray_T ucmds = {0, 0, आकार(ucmd_T), 4, रिक्त};

// When non-zero it is not allowed to add or remove user commands
स्थिर पूर्णांक ucmd_locked = 0;

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
#जर defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_BREAKPOINT, "breakpoint"),
#endif
    KEYVALUE_ENTRY(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ENTRY(EXPAND_COLORS, "color"),
    KEYVALUE_ENTRY(EXPAND_COMMANDS, "command"),
    KEYVALUE_ENTRY(EXPAND_COMPILER, "compiler"),
#जर defined(FEAT_CSCOPE)
    KEYVALUE_ENTRY(EXPAND_CSCOPE, "cscope"),
#endif
#जर defined(FEAT_EVAL)
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
#जर defined(FEAT_KEYMAP)
    KEYVALUE_ENTRY(EXPAND_KEYMAP, "keymap"),
#endif
#जर defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
#जर defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_SCRIPTNAMES, "scriptnames"),
#endif
    KEYVALUE_ENTRY(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ENTRY(EXPAND_SHELLCMDLINE, "shellcmdline"),
#जर defined(FEAT_SIGNS)
    KEYVALUE_ENTRY(EXPAND_SIGN, "sign"),
#endif
    KEYVALUE_ENTRY(EXPAND_OWNSYNTAX, "syntax"),
#जर defined(FEAT_PROFILE)
    KEYVALUE_ENTRY(EXPAND_SYNTIME, "syntime"),
#endif
    KEYVALUE_ENTRY(EXPAND_TAGS, "tag"),
    KEYVALUE_ENTRY(EXPAND_TAGS_LISTFILES, "tag_listfiles"),
    KEYVALUE_ENTRY(EXPAND_USER, "user"),
    KEYVALUE_ENTRY(EXPAND_USER_VARS, "var")
};

प्रकारव्याख्या संरचना
{
    cmd_addr_T किल्ली;
    अक्षर *fullname;
    माप_प्रकार fullnamelen;
    अक्षर *shortname;
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
    ADDRTYPE_ENTRY(ADDR_ओळी, "lines", "line"),
    ADDRTYPE_ENTRY(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ENTRY(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ENTRY(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ENTRY(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ENTRY(ADDR_WINDOWS, "windows", "win")
};

स्थिर पूर्णांक cmp_addr_type(स्थिरांक शून्य *a, स्थिरांक शून्य *b);

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
    पूर्णांक		*full,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    पूर्णांक		*complp) // completion flags or NULL
{
    पूर्णांक		len = (पूर्णांक)(p - eap->cmd);
    पूर्णांक		j, k, matchlen = 0;
    ucmd_T	*uc;
    पूर्णांक		found = FALSE;
    पूर्णांक		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    पूर्णांक		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    साठी (;;)
    {
	साठी (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    पर्यंत (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    जर (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		जर (k == len && found && *np != NUL)
		{
		    जर (gap == &ucmds)
			परत रिक्त;
		    amb_local = TRUE;
		}

		जर (!found || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    जर (k == len)
			found = TRUE;
		    अन्यथा
			possible = TRUE;

		    जर (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    अन्यथा
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (दीर्घ)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    जर (complp != रिक्त)
			*complp = uc->uc_compl;
#ifdef FEAT_EVAL
		    जर (xp != रिक्त)
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
		    जर (k == len && *np == NUL)
		    {
			जर (full != रिक्त)
			    *full = TRUE;
			amb_local = FALSE;
			थांबा;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	जर (j < gap->ga_len || gap == &ucmds)
	    थांबा;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    जर (amb_local)
    {
	जर (xp != रिक्त)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	परत रिक्त;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    जर (found || possible)
	परत p + (matchlen - len);
    परत p;
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
    पर्यंत (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	जर (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    जर (p == रिक्त)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_pattern = arg;
		परत रिक्त;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    जर (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_pattern = p + 1;
	    }
	    अन्यथा जर (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_pattern = p + 1;
		साठी (char_u *c = p + 1; *c != NUL; ++c)
		    जर (*c == ',')
			xp->xp_pattern = c + 1;
	    }
	    अन्यथा जर (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_pattern = p + 1;
	    }
	    अन्यथा जर (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_pattern = p + 1;
	    }
	    परत रिक्त;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    जर (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_pattern = arg;
	परत रिक्त;
    }

    // And finally comes a normal command
    परत skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
set_context_in_user_cmdarg(
	char_u		*cmd UNUSED,
	char_u		*arg,
	दीर्घ		argt,
	पूर्णांक		context,
	expand_T	*xp,
	पूर्णांक		forceit)
{
    char_u	*p;

    जर (context == EXPAND_NOTHING)
	परत रिक्त;

    जर (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	परत रिक्त;
    }

#ifdef FEAT_MENU
    जर (context == EXPAND_MENUS)
	परत set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif
    जर (context == EXPAND_COMMANDS)
	परत arg;
    जर (context == EXPAND_MAPPINGS)
	परत set_context_in_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    जर (!(argt & EX_ARGSPACE))
    {
	p = arg;
	पर्यंत (*p)
	{
	    जर (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    अन्यथा जर (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_pattern = arg;
    xp->xp_context = context;

    परत रिक्त;
}

    char_u *
expand_user_command_name(पूर्णांक idx)
{
    परत get_user_commands(रिक्त, idx - (पूर्णांक)CMD_SIZE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp UNUSED, पूर्णांक idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_buffer;

    जर (idx < buf->b_ucmds.ga_len)
	परत USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    जर (idx < ucmds.ga_len)
    {
	पूर्णांक	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	साठी (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    जर (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		परत (char_u *)"";
	परत name;
    }
    परत रिक्त;
}

#ifdef FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(पूर्णांक idx, पूर्णांक cmdidx)
{
    जर (cmdidx == CMD_USER && idx < ucmds.ga_len)
	परत USER_CMD(idx)->uc_name;
    जर (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_buffer;

	जर (idx < buf->b_ucmds.ga_len)
	    परत USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    परत रिक्त;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    पूर्णांक
user_command_info(पूर्णांक idx, पूर्णांक cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_buffer->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    जर (idx >= gap->ga_len)
	परत FAIL;
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
    परत OK;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx < 0 || idx >= (पूर्णांक)ARRAY_LENGTH(addr_type_complete_tab))
	परत रिक्त;
    परत (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp UNUSED, पूर्णांक idx)
{
    स्थिर अक्षर *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    जर (idx < 0 || idx >= (पूर्णांक)ARRAY_LENGTH(user_cmd_flags))
	परत रिक्त;
    परत (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp UNUSED, पूर्णांक idx)
{
    स्थिर अक्षर *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    जर (idx < 0 || idx >= (पूर्णांक)ARRAY_LENGTH(user_cmd_nargs))
	परत रिक्त;
    परत (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx < 0 || idx >= (पूर्णांक)ARRAY_LENGTH(command_complete_tab))
	परत रिक्त;
    परत command_complete_tab[idx].मूल्य.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
स्थिर अक्षर *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx < 0 || idx >= (पूर्णांक)ARRAY_LENGTH(user_cmd_completeopt_tab))
	परत रिक्त;
    परत (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    स्थिर keyvalue_T *
get_commandtype(पूर्णांक expand)
{
    पूर्णांक i;

    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(command_complete_tab); ++i)
	जर (command_complete_tab[i].किल्ली == expand)
	    परत &command_complete_tab[i];

    परत रिक्त;
}

#ifdef FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_to_str(पूर्णांक expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    जर (kv == रिक्त || kv->मूल्य.string == रिक्त)
	परत रिक्त;

    cmd_compl = kv->मूल्य.string;
    जर (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*buffer;

	buffer = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	जर (buffer == रिक्त)
	    परत रिक्त;
	प्र_लिखो((अक्षर *)buffer, "%s,%s", cmd_compl, compl_arg);
	परत buffer;
    }

    परत vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    पूर्णांक
cmdcomplete_str_to_type(char_u *complete_str)
{
    keyvalue_T target;
    keyvalue_T *entry;
    स्थिर keyvalue_T *last_entry = रिक्त;	// cached result

    जर (STRNCMP(complete_str, "custom,", 7) == 0)
	परत EXPAND_USER_DEFINED;
    जर (STRNCMP(complete_str, "customlist,", 11) == 0)
	परत EXPAND_USER_LIST;

    target.किल्ली = 0;
    target.मूल्य.string = complete_str;
    target.मूल्य.length = 0;			// not used, see cmp_keyvalue_value()

    जर (last_entry != रिक्त && cmp_keyvalue_मूल्य(&target, last_entry) == 0)
	entry = last_entry;
    अन्यथा
    {
	entry = (keyvalue_T *)द्वा_खोज(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    आकार(command_complete_tab[0]),
	    cmp_keyvalue_मूल्य);
	जर (entry == रिक्त)
	    परत EXPAND_NOTHING;

	last_entry = entry;
    }

    परत entry->किल्ली;
}
#endif

/*
 * List user commands starting with "name[name_len]".
 */
    स्थिर शून्य
uc_list(char_u *name, माप_प्रकार name_len)
{
    पूर्णांक		i, j;
    पूर्णांक		found = FALSE;
    ucmd_T	*cmd;
    पूर्णांक		len;
    पूर्णांक		over;
    दीर्घ	a;
    garray_T	*gap;
    keyvalue_T	*entry;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    साठी (;;)
    {
	साठी (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (दीर्घ)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    जर (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || message_filtered(cmd->uc_name))
		सुरूठेवा;

	    // Put out the title first time
	    जर (!found)
		संदेश_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    found = TRUE;
	    संदेश_putchar('\n');
	    जर (got_int)
		थांबा;

	    // Special cases
	    len = 4;
	    जर (a & EX_BANG)
	    {
		संदेश_putchar('!');
		--len;
	    }
	    जर (a & EX_REGSTR)
	    {
		संदेश_putchar('"');
		--len;
	    }
	    जर (gap != &ucmds)
	    {
		संदेश_putchar('b');
		--len;
	    }
	    जर (a & EX_TRLBAR)
	    {
		संदेश_putchar('|');
		--len;
	    }
	    जर (len != 0)
		संदेश_puts(&"    "[4 - len]);

	    संदेश_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (पूर्णांक)cmd->uc_namelen + 4;

	    जर (len < 21)
	    {
		// Field padding spaces   12345678901234567
		स्थिर अक्षर spaces[18] = "                 ";
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
	    निवड ((पूर्णांक)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		स्थिती 0:				IObuff[len++] = '0'; थांबा;
		स्थिती (EX_EXTRA):		IObuff[len++] = '*'; थांबा;
		स्थिती (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; थांबा;
		स्थिती (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; थांबा;
		स्थिती (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; थांबा;
		स्थिती (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; थांबा;
	    }

	    करा
	    {
		IObuff[len++] = ' ';
	    } पर्यंत (len < 5 - over);

	    // Address / Range
	    जर (a & (EX_RANGE|EX_मोजणी))
	    {
		जर (a & EX_मोजणी)
		{
		    // -count=N
		    len += vim_snprintf((अक्षर *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		अन्यथा जर (a & EX_DFLALL)
		    IObuff[len++] = '%';
		अन्यथा जर (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((अक्षर *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		अन्यथा
		    IObuff[len++] = '.';
	    }

	    करा
	    {
		IObuff[len++] = ' ';
	    } पर्यंत (len < 8 - over);

	    // Address Type
	    साठी (j = 0; j < (पूर्णांक)ARRAY_LENGTH(addr_type_complete_tab); ++j)
		जर (addr_type_complete_tab[j].किल्ली != ADDR_ओळी
			&& addr_type_complete_tab[j].किल्ली == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (पूर्णांक)addr_type_complete_tab[j].shortnamelen;
		    थांबा;
		}

	    करा
	    {
		IObuff[len++] = ' ';
	    } पर्यंत (len < 13 - over);

	    // Completion
	    entry = get_commandtype(cmd->uc_compl);
	    जर (entry != रिक्त)
	    {
		STRCPY(IObuff + len, entry->मूल्य.string);
		len += (पूर्णांक)entry->मूल्य.length;
#ifdef FEAT_EVAL
		जर (p_verbose > 0 && cmd->uc_compl_arg != रिक्त)
		{
		    माप_प्रकार uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    जर (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (पूर्णांक)uc_compl_arglen;
		    }
		}
		जर (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((अक्षर *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#endif
	    }

	    करा
	    {
		IObuff[len++] = ' ';
	    } पर्यंत (len < 25 - over);

	    IObuff[len] = NUL;
	    संदेश_outtrans(IObuff);

	    संदेश_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ifdef FEAT_EVAL
	    जर (p_verbose > 0)
		last_set_संदेश(cmd->uc_script_ctx);
#endif
	    out_flush();
	    ui_breakcheck();
	    जर (got_int)
		थांबा;
	}
	जर (gap == &ucmds || i < gap->ga_len)
	    थांबा;
	gap = &ucmds;
    }

    जर (!found)
	संदेश(_("No user-defined commands found"));

    --ucmd_locked;
}

    अक्षर *
uc_fun_cmd(शून्य)
{
    स्थिर char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    पूर्णांक		i;

    साठी (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    परत (अक्षर *)IObuff;
}

/*
 * Parse address type argument
 */
    स्थिर पूर्णांक
parse_addr_type_arg(
    char_u	*मूल्य,
    पूर्णांक		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T target;
    addrtype_T *entry;
    स्थिर addrtype_T *last_entry;	// cached result

    target.किल्ली = 0;
    target.fullname = (अक्षर *)मूल्य;
    target.fullnamelen = vallen;

    जर (last_entry != रिक्त && cmp_addr_type(&target, last_entry) == 0)
	entry = last_entry;
    अन्यथा
    {
	entry = (addrtype_T *)द्वा_खोज(&target,
	    &addr_type_complete_tab,
	    ARRAY_LENGTH(addr_type_complete_tab),
	    आकार(addr_type_complete_tab[0]),
	    cmp_addr_type);
	जर (entry == रिक्त)
	{
	    पूर्णांक i;
	    char_u	*err = मूल्य;

	    साठी (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_address_type_मूल्य_str), err);
	    परत FAIL;
	}

	last_entry = entry;
    }

    *addr_type_arg = entry->किल्ली;

    परत OK;
}

    स्थिर पूर्णांक
cmp_addr_type(स्थिरांक शून्य *a, स्थिरांक शून्य *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    परत STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    पूर्णांक
parse_compl_arg(
    char_u	*मूल्य,
    पूर्णांक		vallen,
    पूर्णांक		*complp,
    दीर्घ	*argt,
    char_u	**compl_arg UNUSED)
{
    char_u	*arg = रिक्त;
#जर defined(FEAT_EVAL)
    माप_प्रकार	arglen = 0;
#endif
    पूर्णांक		i;
    पूर्णांक		valend = vallen;
    keyvalue_T	target;
    keyvalue_T	*entry;
    स्थिर keyvalue_T	*last_entry = रिक्त;	    // cached result

    // Look for any argument part - which is the part after any ','
    साठी (i = 0; i < vallen; ++i)
    {
	जर (मूल्य[i] == ',')
	{
	    arg = &मूल्य[i + 1];
#जर defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#endif
	    valend = i;
	    थांबा;
	}
    }

    target.किल्ली = 0;
    target.मूल्य.string = मूल्य;
    target.मूल्य.length = valend;

    जर (last_entry != रिक्त && cmp_keyvalue_मूल्य_n(&target, last_entry) == 0)
	entry = last_entry;
    अन्यथा
    {
	entry = (keyvalue_T *)द्वा_खोज(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    आकार(command_complete_tab[0]),
	    cmp_keyvalue_मूल्य_n);
	जर (entry == रिक्त)
	{
	    semsg(_(e_invalid_complete_मूल्य_str), मूल्य);
	    परत FAIL;
	}

	last_entry = entry;
    }

    *complp = entry->किल्ली;
    जर (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    अन्यथा जर (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    जर (
#जर defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#endif
								arg != रिक्त)
    {
	emsg(_(e_completion_argument_only_allowed_for_custom_completion));
	परत FAIL;
    }

#जर defined(FEAT_EVAL)
    जर ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == रिक्त)
    {
	emsg(_(e_custom_completion_requires_function_argument));
	परत FAIL;
    }

    जर (arg != रिक्त)
	*compl_arg = vim_strnsave(arg, arglen);
#endif

    परत OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    स्थिर पूर्णांक
parse_completeopt_arg(char_u *मूल्य, पूर्णांक vallen, पूर्णांक *compl_opt)
{
    char_u  *p = मूल्य;
    char_u  *end = मूल्य + vallen;
    पूर्णांक	    flags = 0;

    जर (vallen == 0)
    {
	semsg(_(e_argument_required_for_str), "-completeopt");
	परत FAIL;
    }

    पर्यंत (p < end)
    {
	char_u	*comma;
	माप_प्रकार	itemlen;
	पूर्णांक	matched = FALSE;
	पूर्णांक	i;

	comma = vim_strchr(p, ',');
	जर (comma == रिक्त || comma > end)
	    itemlen = (माप_प्रकार)(end - p);
	अन्यथा
	    itemlen = (माप_प्रकार)(comma - p);

	जर (itemlen == 0)
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), "completeopt");
	    परत FAIL;
	}

	साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(user_cmd_completeopt_tab); ++i)
	{
	    अक्षर *name = user_cmd_completeopt_tab[i];

	    जर (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		जर (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		थांबा;
	    }
	}
	जर (!matched)
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), "completeopt");
	    परत FAIL;
	}

	p += itemlen;
	जर (p < end && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    परत OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    स्थिर पूर्णांक
uc_scan_attr(
    char_u	*attr,
    माप_प्रकार	len,
    दीर्घ	*argt,
    दीर्घ	*def,
    पूर्णांक		*flags,
    पूर्णांक		*complp,
    char_u	**compl_arg,
    पूर्णांक		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    जर (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	परत FAIL;
    }

    // First, try the simple attributes (no arguments)
    जर (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    अन्यथा जर (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_BUFFER;
    अन्यथा जर (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    अन्यथा जर (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    अन्यथा जर (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    अन्यथा
    {
	पूर्णांक	i;
	char_u	*val = रिक्त;
	माप_प्रकार	vallen = 0;
	माप_प्रकार	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	साठी (i = 0; i < (पूर्णांक)len; ++i)
	{
	    जर (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		थांबा;
	    }
	}

	जर (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    जर (vallen == 1)
	    {
		जर (*val == '0')
		    // Do nothing - this is the default
		    ;
		अन्यथा जर (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		अन्यथा जर (*val == '*')
		    *argt |= EX_EXTRA;
		अन्यथा जर (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		अन्यथा जर (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		अन्यथा जर (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		अन्यथा
		    जा wrong_nargs;
	    }
	    अन्यथा
	    {
wrong_nargs:
		emsg(_(e_invalid_संख्या_of_arguments));
		परत FAIL;
	    }
	}
	अन्यथा जर (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    जर (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    अन्यथा जर (val != रिक्त)
	    {
		p = val;
		जर (*def >= 0)
		{
two_मोजणी:
		    emsg(_(e_मोजणी_cannot_be_specified_twice));
		    परत FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		जर (p != val + vallen || vallen == 0)
		{
invalid_मोजणी:
		    emsg(_(e_invalid_default_मूल्य_for_मोजणी));
		    परत FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    जर (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_ओळी;
	}
	अन्यथा जर (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_मोजणी | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    जर (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    जर (val != रिक्त)
	    {
		p = val;
		जर (*def >= 0)
		    जा two_मोजणी;

		*def = getdigits(&p);

		जर (p != val + vallen)
		    जा invalid_मोजणी;
	    }

	    जर (*def < 0)
		*def = 0;
	}
	अन्यथा जर (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    जर (val == रिक्त)
	    {
		semsg(_(e_argument_required_for_str), "-complete");
		परत FAIL;
	    }

	    जर (parse_compl_arg(val, (पूर्णांक)vallen, complp, argt, compl_arg)
								      == FAIL)
		परत FAIL;
	}
	अन्यथा जर (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    जर (val == रिक्त)
	    {
		semsg(_(e_argument_required_for_str), "-completeopt");
		परत FAIL;
	    }

	    जर (parse_completeopt_arg(val, (पूर्णांक)vallen, compl_opt) == FAIL)
		परत FAIL;
	}
	अन्यथा जर (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    जर (val == रिक्त)
	    {
		semsg(_(e_argument_required_for_str), "-addr");
		परत FAIL;
	    }
	    जर (parse_addr_type_arg(val, (पूर्णांक)vallen, addr_type_arg) == FAIL)
		परत FAIL;
	    जर (*addr_type_arg != ADDR_ओळी)
		*argt |= EX_ZEROR;
	}
	अन्यथा
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    परत FAIL;
	}
    }

    परत OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    स्थिर पूर्णांक
uc_add_command(
    char_u	*name,
    माप_प्रकार	name_len,
    char_u	*rep,
    दीर्घ	argt,
    दीर्घ	def,
    पूर्णांक		flags,
    पूर्णांक		compl,
    char_u	*compl_arg UNUSED,
    पूर्णांक		compl_opt,
    cmd_addr_T	addr_type,
    पूर्णांक		force)
{
    ucmd_T	*cmd = रिक्त;
    char_u	*p;
    पूर्णांक		i;
    पूर्णांक		cmp = 1;
    char_u	*rep_buf = रिक्त;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, रिक्त);
    जर (rep_buf == रिक्त)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	जर (rep_buf == रिक्त)
	    परत FAIL;
    }

    // get address of growarray: global or in curbuf
    जर (flags & UC_BUFFER)
    {
	gap = &curbuf->b_ucmds;
	जर (gap->ga_itemsize == 0)
	    ga_init2(gap, आकार(ucmd_T), 4);
    }
    अन्यथा
	gap = &ucmds;

    // Search for the command in the already defined commands.
    साठी (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	जर (cmp == 0)
	{
	    जर (name_len < cmd->uc_namelen)
		cmp = -1;
	    अन्यथा जर (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	जर (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    जर (!force
#ifdef FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#endif
		    )
	    {
		semsg(_(e_command_already_exists_add_bang_to_replace_it_str),
									 name);
		जा fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#जर defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#endif
	    थांबा;
	}

	// Stop as soon as we pass the name to add
	जर (cmp < 0)
	    थांबा;
    }

    // Extend the array unless we're replacing an existing command
    जर (cmp != 0)
    {
	जर (ga_grow(gap, 1) == FAIL)
	    जा fail;
	जर ((p = vim_strnsave(name, name_len)) == रिक्त)
	    जा fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * आकार(ucmd_T));

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
    जर (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ifdef FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#endif
    cmd->uc_addr_type = addr_type;

    परत OK;

fail:
    vim_free(rep_buf);
#जर defined(FEAT_EVAL)
    vim_free(compl_arg);
#endif
    परत FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_block(exarg_T *eap, char_u *p, char_u **tofree, पूर्णांक *flags)
{
    char_u *retp = p;
    char_u *block = find_cmd_block_start(p);

    जर (block != रिक्त && eap->ea_getline != रिक्त)
    {
	garray_T    ga;
	char_u	    *ओळ = रिक्त;

	ga_init2(&ga, आकार(char_u *), 10);
	जर (ga_copy_string(&ga, p) == FAIL)
	    परत retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	जर (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    साठी (;;)
	    {
		vim_free(ओळ);
		जर ((ओळ = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == रिक्त)
		{
		    emsg(_(e_missing_rcurly));
		    थांबा;
		}
		जर (ga_copy_string(&ga, ओळ) == FAIL)
		    थांबा;
		जर (*skipwhite(ओळ) == '}')
		    थांबा;
	    }
	vim_free(ओळ);
	retp = *tofree = ga_concat_strings(&ga, "\n");
	जर (retp == रिक्त)
	    retp = p;
	ga_clear_strings(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	जर (block == p)
	    *flags |= UC_VIM9;
    }
    परत retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    शून्य
skip_cmd_block(exarg_T *eap)
{
    char_u	*tofree = रिक्त;
    पूर्णांक		flags = 0;

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
    पूर्णांक		flags = 0;
    पूर्णांक		compl = EXPAND_NOTHING;
    char_u	*compl_arg = रिक्त;
    पूर्णांक		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    पूर्णांक		has_attr = (eap->arg[0] == '-');
    पूर्णांक		name_len;

    जर (eap->skip)
    {
	skip_cmd_block(eap);
	परत;
    }

    p = eap->arg;

    // Check for attributes
    पर्यंत (*p == '-')
    {
	++p;
	end = skiptowhite(p);
	जर (uc_scan_attr(p, end - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    जा theend;
	p = skipwhite(end);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    जर (ASCII_ISALPHA(*p))
	पर्यंत (ASCII_ISALNUM(*p))
	    ++p;
    जर (!ends_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	जा theend;
    }
    end = p;
    name_len = (पूर्णांक)(end - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(end);
    जर (!has_attr && ends_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    अन्यथा जर (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_start_with_an_uppercase_letter));
    }
    अन्यथा जर ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_used_for_user_defined_command));
    }
    अन्यथा जर (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	जर (in_vim9script())
	    emsg(_(e_complete_used_without_allowing_arguments));
	अन्यथा
	    give_warning_with_source(
		       (char_u *)_(e_complete_used_without_allowing_arguments),
								   TRUE, TRUE);
    }
    अन्यथा जर ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
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

	परत;  // success
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
    जर (curbuf != रिक्त)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    स्थिर पूर्णांक
is_ucmd_locked(शून्य)
{
    जर (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	परत TRUE;
    }
    परत FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    शून्य
uc_clear(garray_T *gap)
{
    पूर्णांक		i;
    ucmd_T	*cmd;

    जर (is_ucmd_locked())
	परत;

    साठी (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#जर defined(FEAT_EVAL)
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
    पूर्णांक		i = 0;
    ucmd_T	*cmd = रिक्त;
    पूर्णांक		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    पूर्णांक		buffer_only = FALSE;

    जर (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	buffer_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    साठी (;;)
    {
	साठी (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    जर (res <= 0)
		थांबा;
	}
	जर (gap == &ucmds || res == 0 || buffer_only)
	    थांबा;
	gap = &ucmds;
    }

    जर (res != 0)
    {
	semsg(_(buffer_only
		    ? e_no_such_user_defined_command_in_current_buffer_str
		    : e_no_such_user_defined_command_str), arg);
	परत;
    }

    जर (is_ucmd_locked())
	परत;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#जर defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#endif

    --gap->ga_len;

    जर (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * आकार(ucmd_T));
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
    पूर्णांक len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    पर्यंत (*p)
    {
	जर (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	अन्यथा जर (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	अन्यथा जर (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	अन्यथा जर (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    जर (*p == NUL)
		थांबा;
	    len += 4; // ", "
	}
	अन्यथा
	{
	    पूर्णांक charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    जर (buf == रिक्त)
    {
	*lenp = 0;
	परत buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    पर्यंत (*p)
    {
	जर (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	अन्यथा जर (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	अन्यथा जर (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	अन्यथा जर (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    जर (*p == NUL)
		थांबा;
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
    परत buf;
}

    स्थिर माप_प्रकार
add_cmd_modifier(
    char_u	*buf,
    माप_प्रकार	buflen,
    अक्षर	*mod_str,
    माप_प्रकार	mod_strlen,
    पूर्णांक		*multi_mods)
{
    जर (buf != रिक्त)
    {
	जर (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    जर (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    अन्यथा
	*multi_mods = 1;

    परत mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    माप_प्रकार
add_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, पूर्णांक *multi_mods)
{
    माप_प्रकार buflen = 0;

    // :aboveleft and :leftabove
    जर (cmod->cmod_split & WSP_ABOVE)
	buflen += add_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    जर (cmod->cmod_split & WSP_BELOW)
	buflen += add_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    जर (cmod->cmod_split & WSP_BOT)
	buflen += add_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    जर (cmod->cmod_tab > 0)
    {
	पूर्णांक tabnr = cmod->cmod_tab - 1;

	जर (tabnr == tabpage_index(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += add_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	अन्यथा
	{
	    अक्षर tab_buf[NUMBUFLEN + 3];
	    माप_प्रकार tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, आकार(tab_buf), "%dtab", tabnr);
	    buflen += add_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    जर (cmod->cmod_split & WSP_शीर्ष)
	buflen += add_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    जर (cmod->cmod_split & WSP_VERT)
	buflen += add_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    जर (cmod->cmod_split & WSP_HOR)
	buflen += add_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    परत buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    माप_प्रकार
produce_cmdmods(char_u *buf, cmdmod_T *cmod, पूर्णांक quote)
{
    माप_प्रकार  buflen = 0;
    पूर्णांक	    multi_mods = 0;
    पूर्णांक	    i;
    स्थिर keyvalue_T mod_entry_tab[] =
    {
#ifdef FEAT_BROWSE_CMD
	KEYVALUE_ENTRY(CMOD_BROWSE, "browse"),
#endif
#जर defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
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

    जर (quote)
    {
	++buflen;
	जर (buf != रिक्त)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    अन्यथा
    जर (buf != रिक्त)
	*buf = NUL;

    // the modifiers that are simple flags
    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(mod_entry_tab); ++i)
	जर (cmod->cmod_flags & mod_entry_tab[i].किल्ली)
	    buflen += add_cmd_modifier(buf, buflen,
		    (अक्षर *)mod_entry_tab[i].मूल्य.string,
		    mod_entry_tab[i].मूल्य.length, &multi_mods);

    // :silent
    जर (cmod->cmod_flags & CMOD_SILENT)
    {
	जर (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += add_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	अन्यथा
	    buflen += add_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    जर (cmod->cmod_verbose > 0)
    {
	पूर्णांक verbose_मूल्य = cmod->cmod_verbose - 1;

	जर (verbose_मूल्य == 1)
	    buflen += add_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	अन्यथा
	{
	    अक्षर verbose_buf[NUMBUFLEN];
	    माप_प्रकार verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, आकार(verbose_buf), "%dverbose", verbose_मूल्य);
	    buflen += add_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += add_win_cmd_modifiers((buf == रिक्त) ? रिक्त : buf + buflen, cmod, &multi_mods);

    जर (quote)
    {
	जर (buf == रिक्त)
	    ++buflen;
	अन्यथा
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    परत buflen;
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
    पूर्णांक		quote = 0;
    गणना {
	ct_ARGS,
	ct_BANG,
	ct_मोजणी,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_REGISTER,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    जर ((vim_strchr((char_u *)"qQfF", *p) != रिक्त) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    जर (l <= 1)
	type = ct_NONE;
    अन्यथा जर (STRNICMP(p, "args>", l) == 0)
	type = ct_ARGS;
    अन्यथा जर (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    अन्यथा जर (STRNICMP(p, "count>", l) == 0)
	type = ct_मोजणी;
    अन्यथा जर (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    अन्यथा जर (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    अन्यथा जर (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    अन्यथा जर (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    अन्यथा जर (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_REGISTER;
    अन्यथा जर (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    निवड (type)
    {
    स्थिती ct_ARGS:
	// Simple case first
	जर (*eap->arg == NUL)
	{
	    जर (quote == 1)
	    {
		result = 2;
		जर (buf != रिक्त)
		    STRCPY(buf, "''");
	    }
	    अन्यथा
		result = 0;
	    थांबा;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	जर ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	निवड (quote)
	{
	स्थिती 0: // No quoting, no splitting
	    result = STRLEN(eap->arg);
	    जर (buf != रिक्त)
		STRCPY(buf, eap->arg);
	    थांबा;
	स्थिती 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    पूर्णांक	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    result = STRLEN(eap->arg) + 2;
	    साठी (p = eap->arg; *p; ++p)
	    {
		जर (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		अन्यथा जर (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    जर (VIM_ISWHITE(p[1]))
			--result;
		    ++p;
		}
		अन्यथा
		     जर (*p == '\\' || *p == '"')
		    ++result;
	    }

	    जर (buf != रिक्त)
	    {
		*buf++ = '"';
		साठी (p = eap->arg; *p; ++p)
		{
		    जर (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    अन्यथा जर (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			जर (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    अन्यथा
			 जर (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    थांबा;
	}
	स्थिती 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    जर (*split_buf == रिक्त)
		*split_buf = uc_split_args(eap->arg, split_len);

	    result = *split_len;
	    जर (buf != रिक्त && result != 0)
		STRCPY(buf, *split_buf);

	    थांबा;
	}
	थांबा;

    स्थिती ct_BANG:
	result = eap->forceit ? 1 : 0;
	जर (quote)
	    result += 2;
	जर (buf != रिक्त)
	{
	    जर (quote)
		*buf++ = '"';
	    जर (eap->forceit)
		*buf++ = '!';
	    जर (quote)
		*buf = '"';
	}
	थांबा;

    स्थिती ct_LINE1:
    स्थिती ct_LINE2:
    स्थिती ct_RANGE:
    स्थिती ct_मोजणी:
    {
	अक्षर अंक_buf[NUMBUFLEN];
	दीर्घ अंक = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_मोजणी :
		   (eap->addr_मोजणी > 0) ? eap->line2 : cmd->uc_def;
	माप_प्रकार अंक_len;

	अंक_len = vim_snprintf(अंक_buf, आकार(अंक_buf), "%ld", अंक);
	result = अंक_len;

	जर (quote)
	    result += 2;

	जर (buf != रिक्त)
	{
	    जर (quote)
		*buf++ = '"';
	    STRCPY(buf, अंक_buf);
	    buf += अंक_len;
	    जर (quote)
		*buf = '"';
	}

	थांबा;
    }

    स्थिती ct_MODS:
    {
	result = produce_cmdmods(buf, &cmdmod, quote);
	थांबा;
    }

    स्थिती ct_REGISTER:
	result = eap->regname ? 1 : 0;
	जर (quote)
	    result += 2;
	जर (buf != रिक्त)
	{
	    जर (quote)
		*buf++ = '\'';
	    जर (eap->regname)
		*buf++ = eap->regname;
	    जर (quote)
		*buf = '\'';
	}
	थांबा;

    स्थिती ct_LT:
	result = 1;
	जर (buf != रिक्त)
	    *buf = '<';
	थांबा;

    पूर्वनिर्धारित:
	// Not recognized: just copy the '<' and return -1.
	result = (माप_प्रकार)-1;
	जर (buf != रिक्त)
	    *buf = '<';
	थांबा;
    }

    परत result;
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
    पूर्णांक		restore_current_sctx = FALSE;
#ifdef FEAT_EVAL
    पूर्णांक		restore_script_version = 0;
#endif

    जर (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    अन्यथा
	cmd = USER_CMD_GA(&prevwin_curwin()->w_buffer->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = रिक्त;
    साठी (;;)
    {
	p = cmd->uc_rep;    // source
	q = buf;	    // destination
	totlen = 0;

	साठी (;;)
	{
	    start = vim_strchr(p, '<');
	    जर (start != रिक्त)
		end = vim_strchr(start + 1, '>');
	    जर (buf != रिक्त)
	    {
		साठी (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		जर (*ksp == K_SPECIAL
			&& (start == रिक्त || ksp < start || end == रिक्त)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ifdef FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (पूर्णांक)KE_CSI)
#endif
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    जर (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    सुरूठेवा;
		}
	    }

	    // break if no <item> is found
	    जर (start == रिक्त || end == रिक्त)
		थांबा;

	    // Include the '>'
	    ++end;

	    // Take everything up to the '<'
	    len = start - p;
	    जर (buf == रिक्त)
		totlen += len;
	    अन्यथा
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_code(start, end - start, q, cmd, eap,
			     &split_buf, &split_len);
	    जर (len == (माप_प्रकार)-1)
	    {
		// no match, continue after '<'
		p = start + 1;
		len = 1;
	    }
	    अन्यथा
		p = end;
	    जर (buf == रिक्त)
		totlen += len;
	    अन्यथा
		q += len;
	}
	जर (buf != रिक्त)	    // second time here, finished
	{
	    STRCPY(q, p);
	    थांबा;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	जर (buf == रिक्त)
	{
	    vim_free(split_buf);
	    परत;
	}
    }

    जर ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_version = cmd->uc_script_ctx.sc_version;
#ifdef FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	जर (cmd->uc_flags & UC_VIM9)
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
    जर (restore_current_sctx)
    {
#ifdef FEAT_EVAL
	जर (restore_script_version != 0)
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version =
							restore_script_version;
#endif
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(split_buf);
}
