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

#அந்தர்பாவ "vim.h"

வகைவரை கட்டமைப்பு ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    long	uc_def;		// The default value for a range/count
    முழு		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    முழு		uc_flags;	// some UC_ flags
    முழு		uc_compl_opt;	// completion options (UCC_ flags)
#ifdef FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#endif
} ucmd_T;

// List of all user commands.
நிலையான garray_T ucmds = {0, 0, அளவு(ucmd_T), 4, வெற்று};

// When non-zero it is not allowed to add or remove user commands
நிலையான முழு ucmd_locked = 0;

#define USER_CMD(i) (&((ucmd_T *)(ucmds.ga_data))[i])
#define USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_data))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
நிலையான keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ENTRY(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ENTRY(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ENTRY(EXPAND_BEHAVE, "behave"),
#எனில் defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_BREAKPOINT, "breakpoint"),
#endif
    KEYVALUE_ENTRY(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ENTRY(EXPAND_COLORS, "color"),
    KEYVALUE_ENTRY(EXPAND_COMMANDS, "command"),
    KEYVALUE_ENTRY(EXPAND_COMPILER, "compiler"),
#எனில் defined(FEAT_CSCOPE)
    KEYVALUE_ENTRY(EXPAND_CSCOPE, "cscope"),
#endif
#எனில் defined(FEAT_EVAL)
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
#எனில் defined(FEAT_KEYMAP)
    KEYVALUE_ENTRY(EXPAND_KEYMAP, "keymap"),
#endif
#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
#எனில் defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_SCRIPTNAMES, "scriptnames"),
#endif
    KEYVALUE_ENTRY(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ENTRY(EXPAND_SHELLCMDLINE, "shellcmdline"),
#எனில் defined(FEAT_SIGNS)
    KEYVALUE_ENTRY(EXPAND_SIGN, "sign"),
#endif
    KEYVALUE_ENTRY(EXPAND_OWNSYNTAX, "syntax"),
#எனில் defined(FEAT_PROFILE)
    KEYVALUE_ENTRY(EXPAND_SYNTIME, "syntime"),
#endif
    KEYVALUE_ENTRY(EXPAND_TAGS, "tag"),
    KEYVALUE_ENTRY(EXPAND_TAGS_LISTFILES, "tag_listfiles"),
    KEYVALUE_ENTRY(EXPAND_USER, "user"),
    KEYVALUE_ENTRY(EXPAND_USER_VARS, "var")
};

வகைவரை கட்டமைப்பு
{
    cmd_addr_T சாவி;
    எழுத்து *fullname;
    size_t fullnamelen;
    எழுத்து *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#define ADDRTYPE_ENTRY(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
நிலையான addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ENTRY(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ENTRY(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ENTRY(ADDR_வரிகள், "lines", "line"),
    ADDRTYPE_ENTRY(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ENTRY(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ENTRY(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ENTRY(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ENTRY(ADDR_WINDOWS, "windows", "win")
};

நிலையான முழு cmp_addr_type(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b);

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
    முழு		*full,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    முழு		*complp) // completion flags or NULL
{
    முழு		len = (முழு)(p - eap->cmd);
    முழு		j, k, matchlen = 0;
    ucmd_T	*uc;
    முழு		found = FALSE;
    முழு		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    முழு		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    க்கு (;;)
    {
	க்கு (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    போது (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    எனில் (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		எனில் (k == len && found && *np != NUL)
		{
		    எனில் (gap == &ucmds)
			திரும்பு வெற்று;
		    amb_local = TRUE;
		}

		எனில் (!found || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    எனில் (k == len)
			found = TRUE;
		    இல்லையெனில்
			possible = TRUE;

		    எனில் (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    இல்லையெனில்
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (long)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    எனில் (complp != வெற்று)
			*complp = uc->uc_compl;
#ifdef FEAT_EVAL
		    எனில் (xp != வெற்று)
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
		    எனில் (k == len && *np == NUL)
		    {
			எனில் (full != வெற்று)
			    *full = TRUE;
			amb_local = FALSE;
			நிறுத்து;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	எனில் (j < gap->ga_len || gap == &ucmds)
	    நிறுத்து;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    எனில் (amb_local)
    {
	எனில் (xp != வெற்று)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	திரும்பு வெற்று;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    எனில் (found || possible)
	திரும்பு p + (matchlen - len);
    திரும்பு p;
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
    போது (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	எனில் (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    எனில் (p == வெற்று)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_pattern = arg;
		திரும்பு வெற்று;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    எனில் (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_pattern = p + 1;
	    }
	    இல்லையெனில் எனில் (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_pattern = p + 1;
		க்கு (char_u *c = p + 1; *c != NUL; ++c)
		    எனில் (*c == ',')
			xp->xp_pattern = c + 1;
	    }
	    இல்லையெனில் எனில் (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_pattern = p + 1;
	    }
	    இல்லையெனில் எனில் (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_pattern = p + 1;
	    }
	    திரும்பு வெற்று;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    எனில் (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_pattern = arg;
	திரும்பு வெற்று;
    }

    // And finally comes a normal command
    திரும்பு skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
set_context_in_user_cmdarg(
	char_u		*cmd UNUSED,
	char_u		*arg,
	long		argt,
	முழு		context,
	expand_T	*xp,
	முழு		forceit)
{
    char_u	*p;

    எனில் (context == EXPAND_NOTHING)
	திரும்பு வெற்று;

    எனில் (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	திரும்பு வெற்று;
    }

#ifdef FEAT_MENU
    எனில் (context == EXPAND_MENUS)
	திரும்பு set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif
    எனில் (context == EXPAND_COMMANDS)
	திரும்பு arg;
    எனில் (context == EXPAND_MAPPINGS)
	திரும்பு set_context_in_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    எனில் (!(argt & EX_ARGSPACE))
    {
	p = arg;
	போது (*p)
	{
	    எனில் (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    இல்லையெனில் எனில் (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_pattern = arg;
    xp->xp_context = context;

    திரும்பு வெற்று;
}

    char_u *
expand_user_command_name(முழு idx)
{
    திரும்பு get_user_commands(வெற்று, idx - (முழு)CMD_SIZE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp UNUSED, முழு idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_buffer;

    எனில் (idx < buf->b_ucmds.ga_len)
	திரும்பு USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    எனில் (idx < ucmds.ga_len)
    {
	முழு	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	க்கு (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    எனில் (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		திரும்பு (char_u *)"";
	திரும்பு name;
    }
    திரும்பு வெற்று;
}

#ifdef FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(முழு idx, முழு cmdidx)
{
    எனில் (cmdidx == CMD_USER && idx < ucmds.ga_len)
	திரும்பு USER_CMD(idx)->uc_name;
    எனில் (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_buffer;

	எனில் (idx < buf->b_ucmds.ga_len)
	    திரும்பு USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    திரும்பு வெற்று;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    முழு
user_command_info(முழு idx, முழு cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_buffer->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    எனில் (idx >= gap->ga_len)
	திரும்பு FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_add_string(d, "name", uc->uc_name);
    dict_add_string(d, "kind", (char_u *)"user");
    dict_add_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_add_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_to_str(uc->uc_compl, uc->uc_compl_arg);
    dict_add_string(d, "complete", compl == வெற்று ? (char_u *)"" : compl);
    vim_free(compl);
    dict_add_string(d, "definition", uc->uc_rep);
    dict_add_எண்(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_add_எண்(d, "lnum", uc->uc_script_ctx.sc_lnum);
    திரும்பு OK;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx < 0 || idx >= (முழு)ARRAY_LENGTH(addr_type_complete_tab))
	திரும்பு வெற்று;
    திரும்பு (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp UNUSED, முழு idx)
{
    நிலையான எழுத்து *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    எனில் (idx < 0 || idx >= (முழு)ARRAY_LENGTH(user_cmd_flags))
	திரும்பு வெற்று;
    திரும்பு (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp UNUSED, முழு idx)
{
    நிலையான எழுத்து *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    எனில் (idx < 0 || idx >= (முழு)ARRAY_LENGTH(user_cmd_nargs))
	திரும்பு வெற்று;
    திரும்பு (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx < 0 || idx >= (முழு)ARRAY_LENGTH(command_complete_tab))
	திரும்பு வெற்று;
    திரும்பு command_complete_tab[idx].மதிப்பு.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
நிலையான எழுத்து *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx < 0 || idx >= (முழு)ARRAY_LENGTH(user_cmd_completeopt_tab))
	திரும்பு வெற்று;
    திரும்பு (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    நிலையான keyvalue_T *
get_commandtype(முழு expand)
{
    முழு i;

    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(command_complete_tab); ++i)
	எனில் (command_complete_tab[i].சாவி == expand)
	    திரும்பு &command_complete_tab[i];

    திரும்பு வெற்று;
}

#ifdef FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_to_str(முழு expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    எனில் (kv == வெற்று || kv->மதிப்பு.string == வெற்று)
	திரும்பு வெற்று;

    cmd_compl = kv->மதிப்பு.string;
    எனில் (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*buffer;

	buffer = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	எனில் (buffer == வெற்று)
	    திரும்பு வெற்று;
	sprintf((எழுத்து *)buffer, "%s,%s", cmd_compl, compl_arg);
	திரும்பு buffer;
    }

    திரும்பு vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    முழு
cmdcomplete_str_to_type(char_u *complete_str)
{
    keyvalue_T target;
    keyvalue_T *entry;
    நிலையான keyvalue_T *last_entry = வெற்று;	// cached result

    எனில் (STRNCMP(complete_str, "custom,", 7) == 0)
	திரும்பு EXPAND_USER_DEFINED;
    எனில் (STRNCMP(complete_str, "customlist,", 11) == 0)
	திரும்பு EXPAND_USER_LIST;

    target.சாவி = 0;
    target.மதிப்பு.string = complete_str;
    target.மதிப்பு.length = 0;			// not used, see cmp_keyvalue_value()

    எனில் (last_entry != வெற்று && cmp_keyvalue_மதிப்பு(&target, last_entry) == 0)
	entry = last_entry;
    இல்லையெனில்
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    அளவு(command_complete_tab[0]),
	    cmp_keyvalue_மதிப்பு);
	எனில் (entry == வெற்று)
	    திரும்பு EXPAND_NOTHING;

	last_entry = entry;
    }

    திரும்பு entry->சாவி;
}
#endif

/*
 * List user commands starting with "name[name_len]".
 */
    நிலையான வெறுமை
uc_list(char_u *name, size_t name_len)
{
    முழு		i, j;
    முழு		found = FALSE;
    ucmd_T	*cmd;
    முழு		len;
    முழு		over;
    long	a;
    garray_T	*gap;
    keyvalue_T	*entry;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    க்கு (;;)
    {
	க்கு (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (long)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    எனில் (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || message_filtered(cmd->uc_name))
		தொடர்;

	    // Put out the title first time
	    எனில் (!found)
		செய்தி_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    found = TRUE;
	    செய்தி_putchar('\n');
	    எனில் (got_int)
		நிறுத்து;

	    // Special cases
	    len = 4;
	    எனில் (a & EX_BANG)
	    {
		செய்தி_putchar('!');
		--len;
	    }
	    எனில் (a & EX_REGSTR)
	    {
		செய்தி_putchar('"');
		--len;
	    }
	    எனில் (gap != &ucmds)
	    {
		செய்தி_putchar('b');
		--len;
	    }
	    எனில் (a & EX_TRLBAR)
	    {
		செய்தி_putchar('|');
		--len;
	    }
	    எனில் (len != 0)
		செய்தி_puts(&"    "[4 - len]);

	    செய்தி_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (முழு)cmd->uc_namelen + 4;

	    எனில் (len < 21)
	    {
		// Field padding spaces   12345678901234567
		நிலையான எழுத்து spaces[18] = "                 ";
		செய்தி_puts(&spaces[len - 4]);
		len = 21;
	    }
	    செய்தி_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    தேர்வு ((முழு)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		நிலை 0:				IObuff[len++] = '0'; நிறுத்து;
		நிலை (EX_EXTRA):		IObuff[len++] = '*'; நிறுத்து;
		நிலை (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; நிறுத்து;
		நிலை (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; நிறுத்து;
		நிலை (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; நிறுத்து;
		நிலை (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; நிறுத்து;
	    }

	    செய்
	    {
		IObuff[len++] = ' ';
	    } போது (len < 5 - over);

	    // Address / Range
	    எனில் (a & (EX_RANGE|EX_மொத்தம்))
	    {
		எனில் (a & EX_மொத்தம்)
		{
		    // -count=N
		    len += vim_snprintf((எழுத்து *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		இல்லையெனில் எனில் (a & EX_DFLALL)
		    IObuff[len++] = '%';
		இல்லையெனில் எனில் (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((எழுத்து *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		இல்லையெனில்
		    IObuff[len++] = '.';
	    }

	    செய்
	    {
		IObuff[len++] = ' ';
	    } போது (len < 8 - over);

	    // Address Type
	    க்கு (j = 0; j < (முழு)ARRAY_LENGTH(addr_type_complete_tab); ++j)
		எனில் (addr_type_complete_tab[j].சாவி != ADDR_வரிகள்
			&& addr_type_complete_tab[j].சாவி == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (முழு)addr_type_complete_tab[j].shortnamelen;
		    நிறுத்து;
		}

	    செய்
	    {
		IObuff[len++] = ' ';
	    } போது (len < 13 - over);

	    // Completion
	    entry = get_commandtype(cmd->uc_compl);
	    எனில் (entry != வெற்று)
	    {
		STRCPY(IObuff + len, entry->மதிப்பு.string);
		len += (முழு)entry->மதிப்பு.length;
#ifdef FEAT_EVAL
		எனில் (p_verbose > 0 && cmd->uc_compl_arg != வெற்று)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    எனில் (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (முழு)uc_compl_arglen;
		    }
		}
		எனில் (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((எழுத்து *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#endif
	    }

	    செய்
	    {
		IObuff[len++] = ' ';
	    } போது (len < 25 - over);

	    IObuff[len] = NUL;
	    செய்தி_outtrans(IObuff);

	    செய்தி_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ifdef FEAT_EVAL
	    எனில் (p_verbose > 0)
		last_set_செய்தி(cmd->uc_script_ctx);
#endif
	    out_flush();
	    ui_breakcheck();
	    எனில் (got_int)
		நிறுத்து;
	}
	எனில் (gap == &ucmds || i < gap->ga_len)
	    நிறுத்து;
	gap = &ucmds;
    }

    எனில் (!found)
	செய்தி(_("No user-defined commands found"));

    --ucmd_locked;
}

    எழுத்து *
uc_fun_cmd(வெறுமை)
{
    நிலையான char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    முழு		i;

    க்கு (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    திரும்பு (எழுத்து *)IObuff;
}

/*
 * Parse address type argument
 */
    நிலையான முழு
parse_addr_type_arg(
    char_u	*மதிப்பு,
    முழு		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T target;
    addrtype_T *entry;
    நிலையான addrtype_T *last_entry;	// cached result

    target.சாவி = 0;
    target.fullname = (எழுத்து *)மதிப்பு;
    target.fullnamelen = vallen;

    எனில் (last_entry != வெற்று && cmp_addr_type(&target, last_entry) == 0)
	entry = last_entry;
    இல்லையெனில்
    {
	entry = (addrtype_T *)bsearch(&target,
	    &addr_type_complete_tab,
	    ARRAY_LENGTH(addr_type_complete_tab),
	    அளவு(addr_type_complete_tab[0]),
	    cmp_addr_type);
	எனில் (entry == வெற்று)
	{
	    முழு i;
	    char_u	*err = மதிப்பு;

	    க்கு (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_address_type_மதிப்பு_str), err);
	    திரும்பு FAIL;
	}

	last_entry = entry;
    }

    *addr_type_arg = entry->சாவி;

    திரும்பு OK;
}

    நிலையான முழு
cmp_addr_type(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    திரும்பு STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    முழு
parse_compl_arg(
    char_u	*மதிப்பு,
    முழு		vallen,
    முழு		*complp,
    long	*argt,
    char_u	**compl_arg UNUSED)
{
    char_u	*arg = வெற்று;
#எனில் defined(FEAT_EVAL)
    size_t	arglen = 0;
#endif
    முழு		i;
    முழு		valend = vallen;
    keyvalue_T	target;
    keyvalue_T	*entry;
    நிலையான keyvalue_T	*last_entry = வெற்று;	    // cached result

    // Look for any argument part - which is the part after any ','
    க்கு (i = 0; i < vallen; ++i)
    {
	எனில் (மதிப்பு[i] == ',')
	{
	    arg = &மதிப்பு[i + 1];
#எனில் defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#endif
	    valend = i;
	    நிறுத்து;
	}
    }

    target.சாவி = 0;
    target.மதிப்பு.string = மதிப்பு;
    target.மதிப்பு.length = valend;

    எனில் (last_entry != வெற்று && cmp_keyvalue_மதிப்பு_n(&target, last_entry) == 0)
	entry = last_entry;
    இல்லையெனில்
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    அளவு(command_complete_tab[0]),
	    cmp_keyvalue_மதிப்பு_n);
	எனில் (entry == வெற்று)
	{
	    semsg(_(e_invalid_complete_மதிப்பு_str), மதிப்பு);
	    திரும்பு FAIL;
	}

	last_entry = entry;
    }

    *complp = entry->சாவி;
    எனில் (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    இல்லையெனில் எனில் (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    எனில் (
#எனில் defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#endif
								arg != வெற்று)
    {
	emsg(_(e_completion_argument_only_allowed_for_custom_completion));
	திரும்பு FAIL;
    }

#எனில் defined(FEAT_EVAL)
    எனில் ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == வெற்று)
    {
	emsg(_(e_custom_completion_requires_function_argument));
	திரும்பு FAIL;
    }

    எனில் (arg != வெற்று)
	*compl_arg = vim_strnsave(arg, arglen);
#endif

    திரும்பு OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    நிலையான முழு
parse_completeopt_arg(char_u *மதிப்பு, முழு vallen, முழு *compl_opt)
{
    char_u  *p = மதிப்பு;
    char_u  *end = மதிப்பு + vallen;
    முழு	    flags = 0;

    எனில் (vallen == 0)
    {
	semsg(_(e_argument_required_for_str), "-completeopt");
	திரும்பு FAIL;
    }

    போது (p < end)
    {
	char_u	*comma;
	size_t	itemlen;
	முழு	matched = FALSE;
	முழு	i;

	comma = vim_strchr(p, ',');
	எனில் (comma == வெற்று || comma > end)
	    itemlen = (size_t)(end - p);
	இல்லையெனில்
	    itemlen = (size_t)(comma - p);

	எனில் (itemlen == 0)
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str), "completeopt");
	    திரும்பு FAIL;
	}

	க்கு (i = 0; i < (முழு)ARRAY_LENGTH(user_cmd_completeopt_tab); ++i)
	{
	    எழுத்து *name = user_cmd_completeopt_tab[i];

	    எனில் (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		எனில் (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		நிறுத்து;
	    }
	}
	எனில் (!matched)
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str), "completeopt");
	    திரும்பு FAIL;
	}

	p += itemlen;
	எனில் (p < end && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    திரும்பு OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    நிலையான முழு
uc_scan_attr(
    char_u	*attr,
    size_t	len,
    long	*argt,
    long	*def,
    முழு		*flags,
    முழு		*complp,
    char_u	**compl_arg,
    முழு		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    எனில் (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	திரும்பு FAIL;
    }

    // First, try the simple attributes (no arguments)
    எனில் (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    இல்லையெனில் எனில் (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_BUFFER;
    இல்லையெனில் எனில் (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    இல்லையெனில் எனில் (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    இல்லையெனில் எனில் (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    இல்லையெனில்
    {
	முழு	i;
	char_u	*val = வெற்று;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	க்கு (i = 0; i < (முழு)len; ++i)
	{
	    எனில் (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		நிறுத்து;
	    }
	}

	எனில் (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    எனில் (vallen == 1)
	    {
		எனில் (*val == '0')
		    // Do nothing - this is the default
		    ;
		இல்லையெனில் எனில் (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		இல்லையெனில் எனில் (*val == '*')
		    *argt |= EX_EXTRA;
		இல்லையெனில் எனில் (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		இல்லையெனில் எனில் (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		இல்லையெனில் எனில் (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		இல்லையெனில்
		    செல் wrong_nargs;
	    }
	    இல்லையெனில்
	    {
wrong_nargs:
		emsg(_(e_invalid_எண்_of_arguments));
		திரும்பு FAIL;
	    }
	}
	இல்லையெனில் எனில் (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    எனில் (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    இல்லையெனில் எனில் (val != வெற்று)
	    {
		p = val;
		எனில் (*def >= 0)
		{
two_மொத்தம்:
		    emsg(_(e_மொத்தம்_cannot_be_specified_twice));
		    திரும்பு FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		எனில் (p != val + vallen || vallen == 0)
		{
invalid_மொத்தம்:
		    emsg(_(e_invalid_default_மதிப்பு_for_மொத்தம்));
		    திரும்பு FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    எனில் (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_வரிகள்;
	}
	இல்லையெனில் எனில் (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_மொத்தம் | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    எனில் (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    எனில் (val != வெற்று)
	    {
		p = val;
		எனில் (*def >= 0)
		    செல் two_மொத்தம்;

		*def = getdigits(&p);

		எனில் (p != val + vallen)
		    செல் invalid_மொத்தம்;
	    }

	    எனில் (*def < 0)
		*def = 0;
	}
	இல்லையெனில் எனில் (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    எனில் (val == வெற்று)
	    {
		semsg(_(e_argument_required_for_str), "-complete");
		திரும்பு FAIL;
	    }

	    எனில் (parse_compl_arg(val, (முழு)vallen, complp, argt, compl_arg)
								      == FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    எனில் (val == வெற்று)
	    {
		semsg(_(e_argument_required_for_str), "-completeopt");
		திரும்பு FAIL;
	    }

	    எனில் (parse_completeopt_arg(val, (முழு)vallen, compl_opt) == FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    எனில் (val == வெற்று)
	    {
		semsg(_(e_argument_required_for_str), "-addr");
		திரும்பு FAIL;
	    }
	    எனில் (parse_addr_type_arg(val, (முழு)vallen, addr_type_arg) == FAIL)
		திரும்பு FAIL;
	    எனில் (*addr_type_arg != ADDR_வரிகள்)
		*argt |= EX_ZEROR;
	}
	இல்லையெனில்
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    திரும்பு FAIL;
	}
    }

    திரும்பு OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    நிலையான முழு
uc_add_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    long	argt,
    long	def,
    முழு		flags,
    முழு		compl,
    char_u	*compl_arg UNUSED,
    முழு		compl_opt,
    cmd_addr_T	addr_type,
    முழு		force)
{
    ucmd_T	*cmd = வெற்று;
    char_u	*p;
    முழு		i;
    முழு		cmp = 1;
    char_u	*rep_buf = வெற்று;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, வெற்று);
    எனில் (rep_buf == வெற்று)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	எனில் (rep_buf == வெற்று)
	    திரும்பு FAIL;
    }

    // get address of growarray: global or in curbuf
    எனில் (flags & UC_BUFFER)
    {
	gap = &curbuf->b_ucmds;
	எனில் (gap->ga_itemsize == 0)
	    ga_init2(gap, அளவு(ucmd_T), 4);
    }
    இல்லையெனில்
	gap = &ucmds;

    // Search for the command in the already defined commands.
    க்கு (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	எனில் (cmp == 0)
	{
	    எனில் (name_len < cmd->uc_namelen)
		cmp = -1;
	    இல்லையெனில் எனில் (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	எனில் (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    எனில் (!force
#ifdef FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#endif
		    )
	    {
		semsg(_(e_command_already_exists_add_bang_to_replace_it_str),
									 name);
		செல் fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#எனில் defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#endif
	    நிறுத்து;
	}

	// Stop as soon as we pass the name to add
	எனில் (cmp < 0)
	    நிறுத்து;
    }

    // Extend the array unless we're replacing an existing command
    எனில் (cmp != 0)
    {
	எனில் (ga_grow(gap, 1) == FAIL)
	    செல் fail;
	எனில் ((p = vim_strnsave(name, name_len)) == வெற்று)
	    செல் fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * அளவு(ucmd_T));

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
    எனில் (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ifdef FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#endif
    cmd->uc_addr_type = addr_type;

    திரும்பு OK;

fail:
    vim_free(rep_buf);
#எனில் defined(FEAT_EVAL)
    vim_free(compl_arg);
#endif
    திரும்பு FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_block(exarg_T *eap, char_u *p, char_u **tofree, முழு *flags)
{
    char_u *retp = p;
    char_u *block = find_cmd_block_start(p);

    எனில் (block != வெற்று && eap->ea_getline != வெற்று)
    {
	garray_T    ga;
	char_u	    *வரி = வெற்று;

	ga_init2(&ga, அளவு(char_u *), 10);
	எனில் (ga_copy_string(&ga, p) == FAIL)
	    திரும்பு retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	எனில் (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    க்கு (;;)
	    {
		vim_free(வரி);
		எனில் ((வரி = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == வெற்று)
		{
		    emsg(_(e_missing_rcurly));
		    நிறுத்து;
		}
		எனில் (ga_copy_string(&ga, வரி) == FAIL)
		    நிறுத்து;
		எனில் (*skipwhite(வரி) == '}')
		    நிறுத்து;
	    }
	vim_free(வரி);
	retp = *tofree = ga_concat_strings(&ga, "\n");
	எனில் (retp == வெற்று)
	    retp = p;
	ga_clear_strings(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	எனில் (block == p)
	    *flags |= UC_VIM9;
    }
    திரும்பு retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    வெறுமை
skip_cmd_block(exarg_T *eap)
{
    char_u	*tofree = வெற்று;
    முழு		flags = 0;

    (வெறுமை)may_get_cmd_block(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    வெறுமை
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*end;
    char_u	*p;
    long	argt = 0;
    long	def = -1;
    முழு		flags = 0;
    முழு		compl = EXPAND_NOTHING;
    char_u	*compl_arg = வெற்று;
    முழு		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    முழு		has_attr = (eap->arg[0] == '-');
    முழு		name_len;

    எனில் (eap->skip)
    {
	skip_cmd_block(eap);
	திரும்பு;
    }

    p = eap->arg;

    // Check for attributes
    போது (*p == '-')
    {
	++p;
	end = skiptowhite(p);
	எனில் (uc_scan_attr(p, end - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    செல் theend;
	p = skipwhite(end);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    எனில் (ASCII_ISALPHA(*p))
	போது (ASCII_ISALNUM(*p))
	    ++p;
    எனில் (!ends_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	செல் theend;
    }
    end = p;
    name_len = (முழு)(end - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(end);
    எனில் (!has_attr && ends_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    இல்லையெனில் எனில் (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_start_with_an_uppercase_letter));
    }
    இல்லையெனில் எனில் ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_used_for_user_defined_command));
    }
    இல்லையெனில் எனில் (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	எனில் (in_vim9script())
	    emsg(_(e_complete_used_without_allowing_arguments));
	இல்லையெனில்
	    give_warning_with_source(
		       (char_u *)_(e_complete_used_without_allowing_arguments),
								   TRUE, TRUE);
    }
    இல்லையெனில் எனில் ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_used_with_nargs_underscore));
    }
    இல்லையெனில்
    {
	char_u *tofree = வெற்று;

	p = may_get_cmd_block(eap, p, &tofree, &flags);

	uc_add_command(name, end - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	திரும்பு;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    வெறுமை
ex_comclear(exarg_T *eap UNUSED)
{
    uc_clear(&ucmds);
    எனில் (curbuf != வெற்று)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    நிலையான முழு
is_ucmd_locked(வெறுமை)
{
    எனில் (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    வெறுமை
uc_clear(garray_T *gap)
{
    முழு		i;
    ucmd_T	*cmd;

    எனில் (is_ucmd_locked())
	திரும்பு;

    க்கு (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#எனில் defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#endif
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    வெறுமை
ex_delcommand(exarg_T *eap)
{
    முழு		i = 0;
    ucmd_T	*cmd = வெற்று;
    முழு		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    முழு		buffer_only = FALSE;

    எனில் (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	buffer_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    க்கு (;;)
    {
	க்கு (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    எனில் (res <= 0)
		நிறுத்து;
	}
	எனில் (gap == &ucmds || res == 0 || buffer_only)
	    நிறுத்து;
	gap = &ucmds;
    }

    எனில் (res != 0)
    {
	semsg(_(buffer_only
		    ? e_no_such_user_defined_command_in_current_buffer_str
		    : e_no_such_user_defined_command_str), arg);
	திரும்பு;
    }

    எனில் (is_ucmd_locked())
	திரும்பு;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#எனில் defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#endif

    --gap->ga_len;

    எனில் (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * அளவு(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    நிலையான char_u *
uc_split_args(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    முழு len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    போது (*p)
    {
	எனில் (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	இல்லையெனில் எனில் (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	இல்லையெனில் எனில் (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	இல்லையெனில் எனில் (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    எனில் (*p == NUL)
		நிறுத்து;
	    len += 4; // ", "
	}
	இல்லையெனில்
	{
	    முழு charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    எனில் (buf == வெற்று)
    {
	*lenp = 0;
	திரும்பு buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    போது (*p)
    {
	எனில் (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	இல்லையெனில் எனில் (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	இல்லையெனில் எனில் (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	இல்லையெனில் எனில் (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    எனில் (*p == NUL)
		நிறுத்து;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	இல்லையெனில்
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    திரும்பு buf;
}

    நிலையான size_t
add_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    எழுத்து	*mod_str,
    size_t	mod_strlen,
    முழு		*multi_mods)
{
    எனில் (buf != வெற்று)
    {
	எனில் (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    எனில் (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    இல்லையெனில்
	*multi_mods = 1;

    திரும்பு mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
add_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, முழு *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    எனில் (cmod->cmod_split & WSP_ABOVE)
	buflen += add_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    எனில் (cmod->cmod_split & WSP_BELOW)
	buflen += add_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    எனில் (cmod->cmod_split & WSP_BOT)
	buflen += add_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    எனில் (cmod->cmod_tab > 0)
    {
	முழு tabnr = cmod->cmod_tab - 1;

	எனில் (tabnr == tabpage_index(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += add_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	இல்லையெனில்
	{
	    எழுத்து tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, அளவு(tab_buf), "%dtab", tabnr);
	    buflen += add_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    எனில் (cmod->cmod_split & WSP_உச்சி)
	buflen += add_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    எனில் (cmod->cmod_split & WSP_VERT)
	buflen += add_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    எனில் (cmod->cmod_split & WSP_HOR)
	buflen += add_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    திரும்பு buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, முழு quote)
{
    size_t  buflen = 0;
    முழு	    multi_mods = 0;
    முழு	    i;
    நிலையான keyvalue_T mod_entry_tab[] =
    {
#ifdef FEAT_BROWSE_CMD
	KEYVALUE_ENTRY(CMOD_BROWSE, "browse"),
#endif
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
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

    எனில் (quote)
    {
	++buflen;
	எனில் (buf != வெற்று)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    இல்லையெனில்
    எனில் (buf != வெற்று)
	*buf = NUL;

    // the modifiers that are simple flags
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(mod_entry_tab); ++i)
	எனில் (cmod->cmod_flags & mod_entry_tab[i].சாவி)
	    buflen += add_cmd_modifier(buf, buflen,
		    (எழுத்து *)mod_entry_tab[i].மதிப்பு.string,
		    mod_entry_tab[i].மதிப்பு.length, &multi_mods);

    // :silent
    எனில் (cmod->cmod_flags & CMOD_SILENT)
    {
	எனில் (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += add_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	இல்லையெனில்
	    buflen += add_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    எனில் (cmod->cmod_verbose > 0)
    {
	முழு verbose_மதிப்பு = cmod->cmod_verbose - 1;

	எனில் (verbose_மதிப்பு == 1)
	    buflen += add_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	இல்லையெனில்
	{
	    எழுத்து verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, அளவு(verbose_buf), "%dverbose", verbose_மதிப்பு);
	    buflen += add_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += add_win_cmd_modifiers((buf == வெற்று) ? வெற்று : buf + buflen, cmod, &multi_mods);

    எனில் (quote)
    {
	எனில் (buf == வெற்று)
	    ++buflen;
	இல்லையெனில்
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    திரும்பு buflen;
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
    நிலையான size_t
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
    முழு		quote = 0;
    எண்ணிக்கை {
	ct_ARGS,
	ct_BANG,
	ct_மொத்தம்,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_REGISTER,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    எனில் ((vim_strchr((char_u *)"qQfF", *p) != வெற்று) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    எனில் (l <= 1)
	type = ct_NONE;
    இல்லையெனில் எனில் (STRNICMP(p, "args>", l) == 0)
	type = ct_ARGS;
    இல்லையெனில் எனில் (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    இல்லையெனில் எனில் (STRNICMP(p, "count>", l) == 0)
	type = ct_மொத்தம்;
    இல்லையெனில் எனில் (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    இல்லையெனில் எனில் (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    இல்லையெனில் எனில் (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    இல்லையெனில் எனில் (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    இல்லையெனில் எனில் (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_REGISTER;
    இல்லையெனில் எனில் (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    தேர்வு (type)
    {
    நிலை ct_ARGS:
	// Simple case first
	எனில் (*eap->arg == NUL)
	{
	    எனில் (quote == 1)
	    {
		result = 2;
		எனில் (buf != வெற்று)
		    STRCPY(buf, "''");
	    }
	    இல்லையெனில்
		result = 0;
	    நிறுத்து;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	எனில் ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	தேர்வு (quote)
	{
	நிலை 0: // No quoting, no splitting
	    result = STRLEN(eap->arg);
	    எனில் (buf != வெற்று)
		STRCPY(buf, eap->arg);
	    நிறுத்து;
	நிலை 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    முழு	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    result = STRLEN(eap->arg) + 2;
	    க்கு (p = eap->arg; *p; ++p)
	    {
		எனில் (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		இல்லையெனில் எனில் (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    எனில் (VIM_ISWHITE(p[1]))
			--result;
		    ++p;
		}
		இல்லையெனில்
		     எனில் (*p == '\\' || *p == '"')
		    ++result;
	    }

	    எனில் (buf != வெற்று)
	    {
		*buf++ = '"';
		க்கு (p = eap->arg; *p; ++p)
		{
		    எனில் (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    இல்லையெனில் எனில் (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			எனில் (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    இல்லையெனில்
			 எனில் (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    நிறுத்து;
	}
	நிலை 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    எனில் (*split_buf == வெற்று)
		*split_buf = uc_split_args(eap->arg, split_len);

	    result = *split_len;
	    எனில் (buf != வெற்று && result != 0)
		STRCPY(buf, *split_buf);

	    நிறுத்து;
	}
	நிறுத்து;

    நிலை ct_BANG:
	result = eap->forceit ? 1 : 0;
	எனில் (quote)
	    result += 2;
	எனில் (buf != வெற்று)
	{
	    எனில் (quote)
		*buf++ = '"';
	    எனில் (eap->forceit)
		*buf++ = '!';
	    எனில் (quote)
		*buf = '"';
	}
	நிறுத்து;

    நிலை ct_LINE1:
    நிலை ct_LINE2:
    நிலை ct_RANGE:
    நிலை ct_மொத்தம்:
    {
	எழுத்து இலக்கம்_buf[NUMBUFLEN];
	long இலக்கம் = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_மொத்தம் :
		   (eap->addr_மொத்தம் > 0) ? eap->line2 : cmd->uc_def;
	size_t இலக்கம்_len;

	இலக்கம்_len = vim_snprintf(இலக்கம்_buf, அளவு(இலக்கம்_buf), "%ld", இலக்கம்);
	result = இலக்கம்_len;

	எனில் (quote)
	    result += 2;

	எனில் (buf != வெற்று)
	{
	    எனில் (quote)
		*buf++ = '"';
	    STRCPY(buf, இலக்கம்_buf);
	    buf += இலக்கம்_len;
	    எனில் (quote)
		*buf = '"';
	}

	நிறுத்து;
    }

    நிலை ct_MODS:
    {
	result = produce_cmdmods(buf, &cmdmod, quote);
	நிறுத்து;
    }

    நிலை ct_REGISTER:
	result = eap->regname ? 1 : 0;
	எனில் (quote)
	    result += 2;
	எனில் (buf != வெற்று)
	{
	    எனில் (quote)
		*buf++ = '\'';
	    எனில் (eap->regname)
		*buf++ = eap->regname;
	    எனில் (quote)
		*buf = '\'';
	}
	நிறுத்து;

    நிலை ct_LT:
	result = 1;
	எனில் (buf != வெற்று)
	    *buf = '<';
	நிறுத்து;

    இயல்பு:
	// Not recognized: just copy the '<' and return -1.
	result = (size_t)-1;
	எனில் (buf != வெற்று)
	    *buf = '<';
	நிறுத்து;
    }

    திரும்பு result;
}

/*
 * Execute a user defined command.
 */
    வெறுமை
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*start;
    char_u	*end = வெற்று;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	split_len = 0;
    char_u	*split_buf = வெற்று;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    முழு		restore_current_sctx = FALSE;
#ifdef FEAT_EVAL
    முழு		restore_script_version = 0;
#endif

    எனில் (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    இல்லையெனில்
	cmd = USER_CMD_GA(&prevwin_curwin()->w_buffer->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = வெற்று;
    க்கு (;;)
    {
	p = cmd->uc_rep;    // source
	q = buf;	    // destination
	totlen = 0;

	க்கு (;;)
	{
	    start = vim_strchr(p, '<');
	    எனில் (start != வெற்று)
		end = vim_strchr(start + 1, '>');
	    எனில் (buf != வெற்று)
	    {
		க்கு (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		எனில் (*ksp == K_SPECIAL
			&& (start == வெற்று || ksp < start || end == வெற்று)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ifdef FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (முழு)KE_CSI)
#endif
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    எனில் (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    தொடர்;
		}
	    }

	    // break if no <item> is found
	    எனில் (start == வெற்று || end == வெற்று)
		நிறுத்து;

	    // Include the '>'
	    ++end;

	    // Take everything up to the '<'
	    len = start - p;
	    எனில் (buf == வெற்று)
		totlen += len;
	    இல்லையெனில்
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_code(start, end - start, q, cmd, eap,
			     &split_buf, &split_len);
	    எனில் (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = start + 1;
		len = 1;
	    }
	    இல்லையெனில்
		p = end;
	    எனில் (buf == வெற்று)
		totlen += len;
	    இல்லையெனில்
		q += len;
	}
	எனில் (buf != வெற்று)	    // second time here, finished
	{
	    STRCPY(q, p);
	    நிறுத்து;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	எனில் (buf == வெற்று)
	{
	    vim_free(split_buf);
	    திரும்பு;
	}
    }

    எனில் ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_version = cmd->uc_script_ctx.sc_version;
#ifdef FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	எனில் (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_version =
				  SCRIPT_ITEM(current_sctx.sc_sid)->sn_version;
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = SCRIPT_VERSION_VIM9;
	}
#endif
    }

    (வெறுமை)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    எனில் (restore_current_sctx)
    {
#ifdef FEAT_EVAL
	எனில் (restore_script_version != 0)
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version =
							restore_script_version;
#endif
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(split_buf);
}
