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

#અન્તર્ભાવ "vim.h"

પ્રકારવ્યાખ્યા સંરચના ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    long	uc_def;		// The default value for a range/count
    પૂર્ણાંક		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    પૂર્ણાંક		uc_flags;	// some UC_ flags
    પૂર્ણાંક		uc_compl_opt;	// completion options (UCC_ flags)
#ifdef FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#endif
} ucmd_T;

// List of all user commands.
સ્થિર garray_T ucmds = {0, 0, કદ(ucmd_T), 4, ખાલી};

// When non-zero it is not allowed to add or remove user commands
સ્થિર પૂર્ણાંક ucmd_locked = 0;

#define USER_CMD(i) (&((ucmd_T *)(ucmds.ga_data))[i])
#define USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_data))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
સ્થિર keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ENTRY(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ENTRY(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ENTRY(EXPAND_BEHAVE, "behave"),
#જો defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_BREAKPOINT, "breakpoint"),
#endif
    KEYVALUE_ENTRY(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ENTRY(EXPAND_COLORS, "color"),
    KEYVALUE_ENTRY(EXPAND_COMMANDS, "command"),
    KEYVALUE_ENTRY(EXPAND_COMPILER, "compiler"),
#જો defined(FEAT_CSCOPE)
    KEYVALUE_ENTRY(EXPAND_CSCOPE, "cscope"),
#endif
#જો defined(FEAT_EVAL)
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
#જો defined(FEAT_KEYMAP)
    KEYVALUE_ENTRY(EXPAND_KEYMAP, "keymap"),
#endif
#જો defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
#જો defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_SCRIPTNAMES, "scriptnames"),
#endif
    KEYVALUE_ENTRY(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ENTRY(EXPAND_SHELLCMDLINE, "shellcmdline"),
#જો defined(FEAT_SIGNS)
    KEYVALUE_ENTRY(EXPAND_SIGN, "sign"),
#endif
    KEYVALUE_ENTRY(EXPAND_OWNSYNTAX, "syntax"),
#જો defined(FEAT_PROFILE)
    KEYVALUE_ENTRY(EXPAND_SYNTIME, "syntime"),
#endif
    KEYVALUE_ENTRY(EXPAND_TAGS, "tag"),
    KEYVALUE_ENTRY(EXPAND_TAGS_LISTFILES, "tag_listfiles"),
    KEYVALUE_ENTRY(EXPAND_USER, "user"),
    KEYVALUE_ENTRY(EXPAND_USER_VARS, "var")
};

પ્રકારવ્યાખ્યા સંરચના
{
    cmd_addr_T ચાવી;
    અક્ષર *fullname;
    size_t fullnamelen;
    અક્ષર *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#define ADDRTYPE_ENTRY(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
સ્થિર addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ENTRY(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ENTRY(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ENTRY(ADDR_લીટીઓ, "lines", "line"),
    ADDRTYPE_ENTRY(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ENTRY(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ENTRY(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ENTRY(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ENTRY(ADDR_WINDOWS, "windows", "win")
};

સ્થિર પૂર્ણાંક cmp_addr_type(અચલ શૂન્ય *a, અચલ શૂન્ય *b);

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
    પૂર્ણાંક		*full,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    પૂર્ણાંક		*complp) // completion flags or NULL
{
    પૂર્ણાંક		len = (પૂર્ણાંક)(p - eap->cmd);
    પૂર્ણાંક		j, k, matchlen = 0;
    ucmd_T	*uc;
    પૂર્ણાંક		found = FALSE;
    પૂર્ણાંક		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    પૂર્ણાંક		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    માટે (;;)
    {
	માટે (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    જ્યારે (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    જો (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		જો (k == len && found && *np != NUL)
		{
		    જો (gap == &ucmds)
			પાછા ખાલી;
		    amb_local = TRUE;
		}

		જો (!found || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    જો (k == len)
			found = TRUE;
		    નહીંતર
			possible = TRUE;

		    જો (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    નહીંતર
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (long)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    જો (complp != ખાલી)
			*complp = uc->uc_compl;
#ifdef FEAT_EVAL
		    જો (xp != ખાલી)
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
		    જો (k == len && *np == NUL)
		    {
			જો (full != ખાલી)
			    *full = TRUE;
			amb_local = FALSE;
			તોડો;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	જો (j < gap->ga_len || gap == &ucmds)
	    તોડો;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    જો (amb_local)
    {
	જો (xp != ખાલી)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	પાછા ખાલી;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    જો (found || possible)
	પાછા p + (matchlen - len);
    પાછા p;
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
    જ્યારે (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	જો (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    જો (p == ખાલી)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_pattern = arg;
		પાછા ખાલી;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    જો (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_pattern = p + 1;
	    }
	    નહીંતર જો (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_pattern = p + 1;
		માટે (char_u *c = p + 1; *c != NUL; ++c)
		    જો (*c == ',')
			xp->xp_pattern = c + 1;
	    }
	    નહીંતર જો (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_pattern = p + 1;
	    }
	    નહીંતર જો (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_pattern = p + 1;
	    }
	    પાછા ખાલી;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    જો (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_pattern = arg;
	પાછા ખાલી;
    }

    // And finally comes a normal command
    પાછા skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
set_context_in_user_cmdarg(
	char_u		*cmd UNUSED,
	char_u		*arg,
	long		argt,
	પૂર્ણાંક		context,
	expand_T	*xp,
	પૂર્ણાંક		forceit)
{
    char_u	*p;

    જો (context == EXPAND_NOTHING)
	પાછા ખાલી;

    જો (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	પાછા ખાલી;
    }

#ifdef FEAT_MENU
    જો (context == EXPAND_MENUS)
	પાછા set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif
    જો (context == EXPAND_COMMANDS)
	પાછા arg;
    જો (context == EXPAND_MAPPINGS)
	પાછા set_context_in_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    જો (!(argt & EX_ARGSPACE))
    {
	p = arg;
	જ્યારે (*p)
	{
	    જો (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    નહીંતર જો (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_pattern = arg;
    xp->xp_context = context;

    પાછા ખાલી;
}

    char_u *
expand_user_command_name(પૂર્ણાંક idx)
{
    પાછા get_user_commands(ખાલી, idx - (પૂર્ણાંક)CMD_SIZE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_buffer;

    જો (idx < buf->b_ucmds.ga_len)
	પાછા USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    જો (idx < ucmds.ga_len)
    {
	પૂર્ણાંક	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	માટે (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    જો (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		પાછા (char_u *)"";
	પાછા name;
    }
    પાછા ખાલી;
}

#ifdef FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(પૂર્ણાંક idx, પૂર્ણાંક cmdidx)
{
    જો (cmdidx == CMD_USER && idx < ucmds.ga_len)
	પાછા USER_CMD(idx)->uc_name;
    જો (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_buffer;

	જો (idx < buf->b_ucmds.ga_len)
	    પાછા USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    પાછા ખાલી;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    પૂર્ણાંક
user_command_info(પૂર્ણાંક idx, પૂર્ણાંક cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_buffer->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    જો (idx >= gap->ga_len)
	પાછા FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_add_string(d, "name", uc->uc_name);
    dict_add_string(d, "kind", (char_u *)"user");
    dict_add_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_add_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_to_str(uc->uc_compl, uc->uc_compl_arg);
    dict_add_string(d, "complete", compl == ખાલી ? (char_u *)"" : compl);
    vim_free(compl);
    dict_add_string(d, "definition", uc->uc_rep);
    dict_add_સંખ્યા(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_add_સંખ્યા(d, "lnum", uc->uc_script_ctx.sc_lnum);
    પાછા OK;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx < 0 || idx >= (પૂર્ણાંક)ARRAY_LENGTH(addr_type_complete_tab))
	પાછા ખાલી;
    પાછા (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    સ્થિર અક્ષર *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    જો (idx < 0 || idx >= (પૂર્ણાંક)ARRAY_LENGTH(user_cmd_flags))
	પાછા ખાલી;
    પાછા (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    સ્થિર અક્ષર *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    જો (idx < 0 || idx >= (પૂર્ણાંક)ARRAY_LENGTH(user_cmd_nargs))
	પાછા ખાલી;
    પાછા (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx < 0 || idx >= (પૂર્ણાંક)ARRAY_LENGTH(command_complete_tab))
	પાછા ખાલી;
    પાછા command_complete_tab[idx].કિંમત.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
સ્થિર અક્ષર *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx < 0 || idx >= (પૂર્ણાંક)ARRAY_LENGTH(user_cmd_completeopt_tab))
	પાછા ખાલી;
    પાછા (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    સ્થિર keyvalue_T *
get_commandtype(પૂર્ણાંક expand)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(command_complete_tab); ++i)
	જો (command_complete_tab[i].ચાવી == expand)
	    પાછા &command_complete_tab[i];

    પાછા ખાલી;
}

#ifdef FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_to_str(પૂર્ણાંક expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    જો (kv == ખાલી || kv->કિંમત.string == ખાલી)
	પાછા ખાલી;

    cmd_compl = kv->કિંમત.string;
    જો (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*buffer;

	buffer = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	જો (buffer == ખાલી)
	    પાછા ખાલી;
	sprintf((અક્ષર *)buffer, "%s,%s", cmd_compl, compl_arg);
	પાછા buffer;
    }

    પાછા vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    પૂર્ણાંક
cmdcomplete_str_to_type(char_u *complete_str)
{
    keyvalue_T target;
    keyvalue_T *entry;
    સ્થિર keyvalue_T *last_entry = ખાલી;	// cached result

    જો (STRNCMP(complete_str, "custom,", 7) == 0)
	પાછા EXPAND_USER_DEFINED;
    જો (STRNCMP(complete_str, "customlist,", 11) == 0)
	પાછા EXPAND_USER_LIST;

    target.ચાવી = 0;
    target.કિંમત.string = complete_str;
    target.કિંમત.length = 0;			// not used, see cmp_keyvalue_value()

    જો (last_entry != ખાલી && cmp_keyvalue_કિંમત(&target, last_entry) == 0)
	entry = last_entry;
    નહીંતર
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    કદ(command_complete_tab[0]),
	    cmp_keyvalue_કિંમત);
	જો (entry == ખાલી)
	    પાછા EXPAND_NOTHING;

	last_entry = entry;
    }

    પાછા entry->ચાવી;
}
#endif

/*
 * List user commands starting with "name[name_len]".
 */
    સ્થિર શૂન્ય
uc_list(char_u *name, size_t name_len)
{
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		found = FALSE;
    ucmd_T	*cmd;
    પૂર્ણાંક		len;
    પૂર્ણાંક		over;
    long	a;
    garray_T	*gap;
    keyvalue_T	*entry;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    માટે (;;)
    {
	માટે (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (long)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    જો (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || message_filtered(cmd->uc_name))
		ચાલુ;

	    // Put out the title first time
	    જો (!found)
		સંદેશ_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    found = TRUE;
	    સંદેશ_putchar('\n');
	    જો (got_int)
		તોડો;

	    // Special cases
	    len = 4;
	    જો (a & EX_BANG)
	    {
		સંદેશ_putchar('!');
		--len;
	    }
	    જો (a & EX_REGSTR)
	    {
		સંદેશ_putchar('"');
		--len;
	    }
	    જો (gap != &ucmds)
	    {
		સંદેશ_putchar('b');
		--len;
	    }
	    જો (a & EX_TRLBAR)
	    {
		સંદેશ_putchar('|');
		--len;
	    }
	    જો (len != 0)
		સંદેશ_puts(&"    "[4 - len]);

	    સંદેશ_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (પૂર્ણાંક)cmd->uc_namelen + 4;

	    જો (len < 21)
	    {
		// Field padding spaces   12345678901234567
		સ્થિર અક્ષર spaces[18] = "                 ";
		સંદેશ_puts(&spaces[len - 4]);
		len = 21;
	    }
	    સંદેશ_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    પસંદગી ((પૂર્ણાંક)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		સ્થિતિ 0:				IObuff[len++] = '0'; તોડો;
		સ્થિતિ (EX_EXTRA):		IObuff[len++] = '*'; તોડો;
		સ્થિતિ (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; તોડો;
		સ્થિતિ (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; તોડો;
		સ્થિતિ (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; તોડો;
		સ્થિતિ (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; તોડો;
	    }

	    કરો
	    {
		IObuff[len++] = ' ';
	    } જ્યારે (len < 5 - over);

	    // Address / Range
	    જો (a & (EX_RANGE|EX_ગણતરી))
	    {
		જો (a & EX_ગણતરી)
		{
		    // -count=N
		    len += vim_snprintf((અક્ષર *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		નહીંતર જો (a & EX_DFLALL)
		    IObuff[len++] = '%';
		નહીંતર જો (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((અક્ષર *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		નહીંતર
		    IObuff[len++] = '.';
	    }

	    કરો
	    {
		IObuff[len++] = ' ';
	    } જ્યારે (len < 8 - over);

	    // Address Type
	    માટે (j = 0; j < (પૂર્ણાંક)ARRAY_LENGTH(addr_type_complete_tab); ++j)
		જો (addr_type_complete_tab[j].ચાવી != ADDR_લીટીઓ
			&& addr_type_complete_tab[j].ચાવી == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (પૂર્ણાંક)addr_type_complete_tab[j].shortnamelen;
		    તોડો;
		}

	    કરો
	    {
		IObuff[len++] = ' ';
	    } જ્યારે (len < 13 - over);

	    // Completion
	    entry = get_commandtype(cmd->uc_compl);
	    જો (entry != ખાલી)
	    {
		STRCPY(IObuff + len, entry->કિંમત.string);
		len += (પૂર્ણાંક)entry->કિંમત.length;
#ifdef FEAT_EVAL
		જો (p_verbose > 0 && cmd->uc_compl_arg != ખાલી)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    જો (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (પૂર્ણાંક)uc_compl_arglen;
		    }
		}
		જો (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((અક્ષર *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#endif
	    }

	    કરો
	    {
		IObuff[len++] = ' ';
	    } જ્યારે (len < 25 - over);

	    IObuff[len] = NUL;
	    સંદેશ_outtrans(IObuff);

	    સંદેશ_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ifdef FEAT_EVAL
	    જો (p_verbose > 0)
		last_set_સંદેશ(cmd->uc_script_ctx);
#endif
	    out_flush();
	    ui_breakcheck();
	    જો (got_int)
		તોડો;
	}
	જો (gap == &ucmds || i < gap->ga_len)
	    તોડો;
	gap = &ucmds;
    }

    જો (!found)
	સંદેશ(_("No user-defined commands found"));

    --ucmd_locked;
}

    અક્ષર *
uc_fun_cmd(શૂન્ય)
{
    સ્થિર char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    પૂર્ણાંક		i;

    માટે (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    પાછા (અક્ષર *)IObuff;
}

/*
 * Parse address type argument
 */
    સ્થિર પૂર્ણાંક
parse_addr_type_arg(
    char_u	*કિંમત,
    પૂર્ણાંક		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T target;
    addrtype_T *entry;
    સ્થિર addrtype_T *last_entry;	// cached result

    target.ચાવી = 0;
    target.fullname = (અક્ષર *)કિંમત;
    target.fullnamelen = vallen;

    જો (last_entry != ખાલી && cmp_addr_type(&target, last_entry) == 0)
	entry = last_entry;
    નહીંતર
    {
	entry = (addrtype_T *)bsearch(&target,
	    &addr_type_complete_tab,
	    ARRAY_LENGTH(addr_type_complete_tab),
	    કદ(addr_type_complete_tab[0]),
	    cmp_addr_type);
	જો (entry == ખાલી)
	{
	    પૂર્ણાંક i;
	    char_u	*err = કિંમત;

	    માટે (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_address_type_કિંમત_str), err);
	    પાછા FAIL;
	}

	last_entry = entry;
    }

    *addr_type_arg = entry->ચાવી;

    પાછા OK;
}

    સ્થિર પૂર્ણાંક
cmp_addr_type(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    પાછા STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    પૂર્ણાંક
parse_compl_arg(
    char_u	*કિંમત,
    પૂર્ણાંક		vallen,
    પૂર્ણાંક		*complp,
    long	*argt,
    char_u	**compl_arg UNUSED)
{
    char_u	*arg = ખાલી;
#જો defined(FEAT_EVAL)
    size_t	arglen = 0;
#endif
    પૂર્ણાંક		i;
    પૂર્ણાંક		valend = vallen;
    keyvalue_T	target;
    keyvalue_T	*entry;
    સ્થિર keyvalue_T	*last_entry = ખાલી;	    // cached result

    // Look for any argument part - which is the part after any ','
    માટે (i = 0; i < vallen; ++i)
    {
	જો (કિંમત[i] == ',')
	{
	    arg = &કિંમત[i + 1];
#જો defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#endif
	    valend = i;
	    તોડો;
	}
    }

    target.ચાવી = 0;
    target.કિંમત.string = કિંમત;
    target.કિંમત.length = valend;

    જો (last_entry != ખાલી && cmp_keyvalue_કિંમત_n(&target, last_entry) == 0)
	entry = last_entry;
    નહીંતર
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    કદ(command_complete_tab[0]),
	    cmp_keyvalue_કિંમત_n);
	જો (entry == ખાલી)
	{
	    semsg(_(e_invalid_complete_કિંમત_str), કિંમત);
	    પાછા FAIL;
	}

	last_entry = entry;
    }

    *complp = entry->ચાવી;
    જો (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    નહીંતર જો (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    જો (
#જો defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#endif
								arg != ખાલી)
    {
	emsg(_(e_completion_argument_only_allowed_for_custom_completion));
	પાછા FAIL;
    }

#જો defined(FEAT_EVAL)
    જો ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == ખાલી)
    {
	emsg(_(e_custom_completion_requires_function_argument));
	પાછા FAIL;
    }

    જો (arg != ખાલી)
	*compl_arg = vim_strnsave(arg, arglen);
#endif

    પાછા OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    સ્થિર પૂર્ણાંક
parse_completeopt_arg(char_u *કિંમત, પૂર્ણાંક vallen, પૂર્ણાંક *compl_opt)
{
    char_u  *p = કિંમત;
    char_u  *end = કિંમત + vallen;
    પૂર્ણાંક	    flags = 0;

    જો (vallen == 0)
    {
	semsg(_(e_argument_required_for_str), "-completeopt");
	પાછા FAIL;
    }

    જ્યારે (p < end)
    {
	char_u	*comma;
	size_t	itemlen;
	પૂર્ણાંક	matched = FALSE;
	પૂર્ણાંક	i;

	comma = vim_strchr(p, ',');
	જો (comma == ખાલી || comma > end)
	    itemlen = (size_t)(end - p);
	નહીંતર
	    itemlen = (size_t)(comma - p);

	જો (itemlen == 0)
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str), "completeopt");
	    પાછા FAIL;
	}

	માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(user_cmd_completeopt_tab); ++i)
	{
	    અક્ષર *name = user_cmd_completeopt_tab[i];

	    જો (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		જો (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		તોડો;
	    }
	}
	જો (!matched)
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str), "completeopt");
	    પાછા FAIL;
	}

	p += itemlen;
	જો (p < end && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    પાછા OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    સ્થિર પૂર્ણાંક
uc_scan_attr(
    char_u	*attr,
    size_t	len,
    long	*argt,
    long	*def,
    પૂર્ણાંક		*flags,
    પૂર્ણાંક		*complp,
    char_u	**compl_arg,
    પૂર્ણાંક		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    જો (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	પાછા FAIL;
    }

    // First, try the simple attributes (no arguments)
    જો (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    નહીંતર જો (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_BUFFER;
    નહીંતર જો (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    નહીંતર જો (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    નહીંતર જો (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    નહીંતર
    {
	પૂર્ણાંક	i;
	char_u	*val = ખાલી;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	માટે (i = 0; i < (પૂર્ણાંક)len; ++i)
	{
	    જો (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		તોડો;
	    }
	}

	જો (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    જો (vallen == 1)
	    {
		જો (*val == '0')
		    // Do nothing - this is the default
		    ;
		નહીંતર જો (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		નહીંતર જો (*val == '*')
		    *argt |= EX_EXTRA;
		નહીંતર જો (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		નહીંતર જો (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		નહીંતર જો (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		નહીંતર
		    જાઓ wrong_nargs;
	    }
	    નહીંતર
	    {
wrong_nargs:
		emsg(_(e_invalid_સંખ્યા_of_arguments));
		પાછા FAIL;
	    }
	}
	નહીંતર જો (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    જો (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    નહીંતર જો (val != ખાલી)
	    {
		p = val;
		જો (*def >= 0)
		{
two_ગણતરી:
		    emsg(_(e_ગણતરી_cannot_be_specified_twice));
		    પાછા FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		જો (p != val + vallen || vallen == 0)
		{
invalid_ગણતરી:
		    emsg(_(e_invalid_default_કિંમત_for_ગણતરી));
		    પાછા FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    જો (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_લીટીઓ;
	}
	નહીંતર જો (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_ગણતરી | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    જો (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    જો (val != ખાલી)
	    {
		p = val;
		જો (*def >= 0)
		    જાઓ two_ગણતરી;

		*def = getdigits(&p);

		જો (p != val + vallen)
		    જાઓ invalid_ગણતરી;
	    }

	    જો (*def < 0)
		*def = 0;
	}
	નહીંતર જો (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    જો (val == ખાલી)
	    {
		semsg(_(e_argument_required_for_str), "-complete");
		પાછા FAIL;
	    }

	    જો (parse_compl_arg(val, (પૂર્ણાંક)vallen, complp, argt, compl_arg)
								      == FAIL)
		પાછા FAIL;
	}
	નહીંતર જો (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    જો (val == ખાલી)
	    {
		semsg(_(e_argument_required_for_str), "-completeopt");
		પાછા FAIL;
	    }

	    જો (parse_completeopt_arg(val, (પૂર્ણાંક)vallen, compl_opt) == FAIL)
		પાછા FAIL;
	}
	નહીંતર જો (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    જો (val == ખાલી)
	    {
		semsg(_(e_argument_required_for_str), "-addr");
		પાછા FAIL;
	    }
	    જો (parse_addr_type_arg(val, (પૂર્ણાંક)vallen, addr_type_arg) == FAIL)
		પાછા FAIL;
	    જો (*addr_type_arg != ADDR_લીટીઓ)
		*argt |= EX_ZEROR;
	}
	નહીંતર
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    પાછા FAIL;
	}
    }

    પાછા OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    સ્થિર પૂર્ણાંક
uc_add_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    long	argt,
    long	def,
    પૂર્ણાંક		flags,
    પૂર્ણાંક		compl,
    char_u	*compl_arg UNUSED,
    પૂર્ણાંક		compl_opt,
    cmd_addr_T	addr_type,
    પૂર્ણાંક		force)
{
    ucmd_T	*cmd = ખાલી;
    char_u	*p;
    પૂર્ણાંક		i;
    પૂર્ણાંક		cmp = 1;
    char_u	*rep_buf = ખાલી;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, ખાલી);
    જો (rep_buf == ખાલી)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	જો (rep_buf == ખાલી)
	    પાછા FAIL;
    }

    // get address of growarray: global or in curbuf
    જો (flags & UC_BUFFER)
    {
	gap = &curbuf->b_ucmds;
	જો (gap->ga_itemsize == 0)
	    ga_init2(gap, કદ(ucmd_T), 4);
    }
    નહીંતર
	gap = &ucmds;

    // Search for the command in the already defined commands.
    માટે (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	જો (cmp == 0)
	{
	    જો (name_len < cmd->uc_namelen)
		cmp = -1;
	    નહીંતર જો (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	જો (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    જો (!force
#ifdef FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#endif
		    )
	    {
		semsg(_(e_command_already_exists_add_bang_to_replace_it_str),
									 name);
		જાઓ fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#જો defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#endif
	    તોડો;
	}

	// Stop as soon as we pass the name to add
	જો (cmp < 0)
	    તોડો;
    }

    // Extend the array unless we're replacing an existing command
    જો (cmp != 0)
    {
	જો (ga_grow(gap, 1) == FAIL)
	    જાઓ fail;
	જો ((p = vim_strnsave(name, name_len)) == ખાલી)
	    જાઓ fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * કદ(ucmd_T));

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
    જો (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ifdef FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#endif
    cmd->uc_addr_type = addr_type;

    પાછા OK;

fail:
    vim_free(rep_buf);
#જો defined(FEAT_EVAL)
    vim_free(compl_arg);
#endif
    પાછા FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_block(exarg_T *eap, char_u *p, char_u **tofree, પૂર્ણાંક *flags)
{
    char_u *retp = p;
    char_u *block = find_cmd_block_start(p);

    જો (block != ખાલી && eap->ea_getline != ખાલી)
    {
	garray_T    ga;
	char_u	    *લીટી = ખાલી;

	ga_init2(&ga, કદ(char_u *), 10);
	જો (ga_copy_string(&ga, p) == FAIL)
	    પાછા retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	જો (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    માટે (;;)
	    {
		vim_free(લીટી);
		જો ((લીટી = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == ખાલી)
		{
		    emsg(_(e_missing_rcurly));
		    તોડો;
		}
		જો (ga_copy_string(&ga, લીટી) == FAIL)
		    તોડો;
		જો (*skipwhite(લીટી) == '}')
		    તોડો;
	    }
	vim_free(લીટી);
	retp = *tofree = ga_concat_strings(&ga, "\n");
	જો (retp == ખાલી)
	    retp = p;
	ga_clear_strings(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	જો (block == p)
	    *flags |= UC_VIM9;
    }
    પાછા retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    શૂન્ય
skip_cmd_block(exarg_T *eap)
{
    char_u	*tofree = ખાલી;
    પૂર્ણાંક		flags = 0;

    (શૂન્ય)may_get_cmd_block(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    શૂન્ય
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*end;
    char_u	*p;
    long	argt = 0;
    long	def = -1;
    પૂર્ણાંક		flags = 0;
    પૂર્ણાંક		compl = EXPAND_NOTHING;
    char_u	*compl_arg = ખાલી;
    પૂર્ણાંક		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    પૂર્ણાંક		has_attr = (eap->arg[0] == '-');
    પૂર્ણાંક		name_len;

    જો (eap->skip)
    {
	skip_cmd_block(eap);
	પાછા;
    }

    p = eap->arg;

    // Check for attributes
    જ્યારે (*p == '-')
    {
	++p;
	end = skiptowhite(p);
	જો (uc_scan_attr(p, end - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    જાઓ theend;
	p = skipwhite(end);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    જો (ASCII_ISALPHA(*p))
	જ્યારે (ASCII_ISALNUM(*p))
	    ++p;
    જો (!ends_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	જાઓ theend;
    }
    end = p;
    name_len = (પૂર્ણાંક)(end - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(end);
    જો (!has_attr && ends_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    નહીંતર જો (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_start_with_an_uppercase_letter));
    }
    નહીંતર જો ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_used_for_user_defined_command));
    }
    નહીંતર જો (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	જો (in_vim9script())
	    emsg(_(e_complete_used_without_allowing_arguments));
	નહીંતર
	    give_warning_with_source(
		       (char_u *)_(e_complete_used_without_allowing_arguments),
								   TRUE, TRUE);
    }
    નહીંતર જો ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_used_with_nargs_underscore));
    }
    નહીંતર
    {
	char_u *tofree = ખાલી;

	p = may_get_cmd_block(eap, p, &tofree, &flags);

	uc_add_command(name, end - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	પાછા;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    શૂન્ય
ex_comclear(exarg_T *eap UNUSED)
{
    uc_clear(&ucmds);
    જો (curbuf != ખાલી)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    સ્થિર પૂર્ણાંક
is_ucmd_locked(શૂન્ય)
{
    જો (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    શૂન્ય
uc_clear(garray_T *gap)
{
    પૂર્ણાંક		i;
    ucmd_T	*cmd;

    જો (is_ucmd_locked())
	પાછા;

    માટે (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#જો defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#endif
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    શૂન્ય
ex_delcommand(exarg_T *eap)
{
    પૂર્ણાંક		i = 0;
    ucmd_T	*cmd = ખાલી;
    પૂર્ણાંક		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    પૂર્ણાંક		buffer_only = FALSE;

    જો (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	buffer_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    માટે (;;)
    {
	માટે (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    જો (res <= 0)
		તોડો;
	}
	જો (gap == &ucmds || res == 0 || buffer_only)
	    તોડો;
	gap = &ucmds;
    }

    જો (res != 0)
    {
	semsg(_(buffer_only
		    ? e_no_such_user_defined_command_in_current_buffer_str
		    : e_no_such_user_defined_command_str), arg);
	પાછા;
    }

    જો (is_ucmd_locked())
	પાછા;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#જો defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#endif

    --gap->ga_len;

    જો (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * કદ(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    સ્થિર char_u *
uc_split_args(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    પૂર્ણાંક len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    જ્યારે (*p)
    {
	જો (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	નહીંતર જો (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	નહીંતર જો (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	નહીંતર જો (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    જો (*p == NUL)
		તોડો;
	    len += 4; // ", "
	}
	નહીંતર
	{
	    પૂર્ણાંક charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    જો (buf == ખાલી)
    {
	*lenp = 0;
	પાછા buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    જ્યારે (*p)
    {
	જો (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	નહીંતર જો (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	નહીંતર જો (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	નહીંતર જો (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    જો (*p == NUL)
		તોડો;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	નહીંતર
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    પાછા buf;
}

    સ્થિર size_t
add_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    અક્ષર	*mod_str,
    size_t	mod_strlen,
    પૂર્ણાંક		*multi_mods)
{
    જો (buf != ખાલી)
    {
	જો (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    જો (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    નહીંતર
	*multi_mods = 1;

    પાછા mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
add_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, પૂર્ણાંક *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    જો (cmod->cmod_split & WSP_ABOVE)
	buflen += add_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    જો (cmod->cmod_split & WSP_BELOW)
	buflen += add_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    જો (cmod->cmod_split & WSP_BOT)
	buflen += add_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    જો (cmod->cmod_tab > 0)
    {
	પૂર્ણાંક tabnr = cmod->cmod_tab - 1;

	જો (tabnr == tabpage_index(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += add_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	નહીંતર
	{
	    અક્ષર tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, કદ(tab_buf), "%dtab", tabnr);
	    buflen += add_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    જો (cmod->cmod_split & WSP_ટોચ)
	buflen += add_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    જો (cmod->cmod_split & WSP_VERT)
	buflen += add_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    જો (cmod->cmod_split & WSP_HOR)
	buflen += add_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    પાછા buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, પૂર્ણાંક quote)
{
    size_t  buflen = 0;
    પૂર્ણાંક	    multi_mods = 0;
    પૂર્ણાંક	    i;
    સ્થિર keyvalue_T mod_entry_tab[] =
    {
#ifdef FEAT_BROWSE_CMD
	KEYVALUE_ENTRY(CMOD_BROWSE, "browse"),
#endif
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
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

    જો (quote)
    {
	++buflen;
	જો (buf != ખાલી)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    નહીંતર
    જો (buf != ખાલી)
	*buf = NUL;

    // the modifiers that are simple flags
    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(mod_entry_tab); ++i)
	જો (cmod->cmod_flags & mod_entry_tab[i].ચાવી)
	    buflen += add_cmd_modifier(buf, buflen,
		    (અક્ષર *)mod_entry_tab[i].કિંમત.string,
		    mod_entry_tab[i].કિંમત.length, &multi_mods);

    // :silent
    જો (cmod->cmod_flags & CMOD_SILENT)
    {
	જો (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += add_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	નહીંતર
	    buflen += add_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    જો (cmod->cmod_verbose > 0)
    {
	પૂર્ણાંક verbose_કિંમત = cmod->cmod_verbose - 1;

	જો (verbose_કિંમત == 1)
	    buflen += add_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	નહીંતર
	{
	    અક્ષર verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, કદ(verbose_buf), "%dverbose", verbose_કિંમત);
	    buflen += add_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += add_win_cmd_modifiers((buf == ખાલી) ? ખાલી : buf + buflen, cmod, &multi_mods);

    જો (quote)
    {
	જો (buf == ખાલી)
	    ++buflen;
	નહીંતર
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    પાછા buflen;
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
    સ્થિર size_t
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
    પૂર્ણાંક		quote = 0;
    ગણના {
	ct_ARGS,
	ct_BANG,
	ct_ગણતરી,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_REGISTER,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    જો ((vim_strchr((char_u *)"qQfF", *p) != ખાલી) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    જો (l <= 1)
	type = ct_NONE;
    નહીંતર જો (STRNICMP(p, "args>", l) == 0)
	type = ct_ARGS;
    નહીંતર જો (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    નહીંતર જો (STRNICMP(p, "count>", l) == 0)
	type = ct_ગણતરી;
    નહીંતર જો (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    નહીંતર જો (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    નહીંતર જો (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    નહીંતર જો (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    નહીંતર જો (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_REGISTER;
    નહીંતર જો (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    પસંદગી (type)
    {
    સ્થિતિ ct_ARGS:
	// Simple case first
	જો (*eap->arg == NUL)
	{
	    જો (quote == 1)
	    {
		result = 2;
		જો (buf != ખાલી)
		    STRCPY(buf, "''");
	    }
	    નહીંતર
		result = 0;
	    તોડો;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	જો ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	પસંદગી (quote)
	{
	સ્થિતિ 0: // No quoting, no splitting
	    result = STRLEN(eap->arg);
	    જો (buf != ખાલી)
		STRCPY(buf, eap->arg);
	    તોડો;
	સ્થિતિ 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    પૂર્ણાંક	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    result = STRLEN(eap->arg) + 2;
	    માટે (p = eap->arg; *p; ++p)
	    {
		જો (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		નહીંતર જો (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    જો (VIM_ISWHITE(p[1]))
			--result;
		    ++p;
		}
		નહીંતર
		     જો (*p == '\\' || *p == '"')
		    ++result;
	    }

	    જો (buf != ખાલી)
	    {
		*buf++ = '"';
		માટે (p = eap->arg; *p; ++p)
		{
		    જો (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    નહીંતર જો (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			જો (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    નહીંતર
			 જો (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    તોડો;
	}
	સ્થિતિ 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    જો (*split_buf == ખાલી)
		*split_buf = uc_split_args(eap->arg, split_len);

	    result = *split_len;
	    જો (buf != ખાલી && result != 0)
		STRCPY(buf, *split_buf);

	    તોડો;
	}
	તોડો;

    સ્થિતિ ct_BANG:
	result = eap->forceit ? 1 : 0;
	જો (quote)
	    result += 2;
	જો (buf != ખાલી)
	{
	    જો (quote)
		*buf++ = '"';
	    જો (eap->forceit)
		*buf++ = '!';
	    જો (quote)
		*buf = '"';
	}
	તોડો;

    સ્થિતિ ct_LINE1:
    સ્થિતિ ct_LINE2:
    સ્થિતિ ct_RANGE:
    સ્થિતિ ct_ગણતરી:
    {
	અક્ષર અંક_buf[NUMBUFLEN];
	long અંક = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_ગણતરી :
		   (eap->addr_ગણતરી > 0) ? eap->line2 : cmd->uc_def;
	size_t અંક_len;

	અંક_len = vim_snprintf(અંક_buf, કદ(અંક_buf), "%ld", અંક);
	result = અંક_len;

	જો (quote)
	    result += 2;

	જો (buf != ખાલી)
	{
	    જો (quote)
		*buf++ = '"';
	    STRCPY(buf, અંક_buf);
	    buf += અંક_len;
	    જો (quote)
		*buf = '"';
	}

	તોડો;
    }

    સ્થિતિ ct_MODS:
    {
	result = produce_cmdmods(buf, &cmdmod, quote);
	તોડો;
    }

    સ્થિતિ ct_REGISTER:
	result = eap->regname ? 1 : 0;
	જો (quote)
	    result += 2;
	જો (buf != ખાલી)
	{
	    જો (quote)
		*buf++ = '\'';
	    જો (eap->regname)
		*buf++ = eap->regname;
	    જો (quote)
		*buf = '\'';
	}
	તોડો;

    સ્થિતિ ct_LT:
	result = 1;
	જો (buf != ખાલી)
	    *buf = '<';
	તોડો;

    મૂળભૂત:
	// Not recognized: just copy the '<' and return -1.
	result = (size_t)-1;
	જો (buf != ખાલી)
	    *buf = '<';
	તોડો;
    }

    પાછા result;
}

/*
 * Execute a user defined command.
 */
    શૂન્ય
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*start;
    char_u	*end = ખાલી;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	split_len = 0;
    char_u	*split_buf = ખાલી;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    પૂર્ણાંક		restore_current_sctx = FALSE;
#ifdef FEAT_EVAL
    પૂર્ણાંક		restore_script_version = 0;
#endif

    જો (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    નહીંતર
	cmd = USER_CMD_GA(&prevwin_curwin()->w_buffer->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = ખાલી;
    માટે (;;)
    {
	p = cmd->uc_rep;    // source
	q = buf;	    // destination
	totlen = 0;

	માટે (;;)
	{
	    start = vim_strchr(p, '<');
	    જો (start != ખાલી)
		end = vim_strchr(start + 1, '>');
	    જો (buf != ખાલી)
	    {
		માટે (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		જો (*ksp == K_SPECIAL
			&& (start == ખાલી || ksp < start || end == ખાલી)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ifdef FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (પૂર્ણાંક)KE_CSI)
#endif
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    જો (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    ચાલુ;
		}
	    }

	    // break if no <item> is found
	    જો (start == ખાલી || end == ખાલી)
		તોડો;

	    // Include the '>'
	    ++end;

	    // Take everything up to the '<'
	    len = start - p;
	    જો (buf == ખાલી)
		totlen += len;
	    નહીંતર
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_code(start, end - start, q, cmd, eap,
			     &split_buf, &split_len);
	    જો (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = start + 1;
		len = 1;
	    }
	    નહીંતર
		p = end;
	    જો (buf == ખાલી)
		totlen += len;
	    નહીંતર
		q += len;
	}
	જો (buf != ખાલી)	    // second time here, finished
	{
	    STRCPY(q, p);
	    તોડો;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	જો (buf == ખાલી)
	{
	    vim_free(split_buf);
	    પાછા;
	}
    }

    જો ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_version = cmd->uc_script_ctx.sc_version;
#ifdef FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	જો (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_version =
				  SCRIPT_ITEM(current_sctx.sc_sid)->sn_version;
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = SCRIPT_VERSION_VIM9;
	}
#endif
    }

    (શૂન્ય)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    જો (restore_current_sctx)
    {
#ifdef FEAT_EVAL
	જો (restore_script_version != 0)
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version =
							restore_script_version;
#endif
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(split_buf);
}
