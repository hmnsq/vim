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

#شامل "vim.h"

تعریفنوع ساختار ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    long	uc_def;		// The default value for a range/count
    صحیح		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    صحیح		uc_flags;	// some UC_ flags
    صحیح		uc_compl_opt;	// completion options (UCC_ flags)
#ifdef FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#endif
} ucmd_T;

// List of all user commands.
ایستا garray_T ucmds = {0, 0, اندازه(ucmd_T), 4, پوچ};

// When non-zero it is not allowed to add or remove user commands
ایستا صحیح ucmd_locked = 0;

#define USER_CMD(i) (&((ucmd_T *)(ucmds.ga_data))[i])
#define USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_data))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
ایستا keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ENTRY(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ENTRY(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ENTRY(EXPAND_BEHAVE, "behave"),
#اگر defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_BREAKPOINT, "breakpoint"),
#endif
    KEYVALUE_ENTRY(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ENTRY(EXPAND_COLORS, "color"),
    KEYVALUE_ENTRY(EXPAND_COMMANDS, "command"),
    KEYVALUE_ENTRY(EXPAND_COMPILER, "compiler"),
#اگر defined(FEAT_CSCOPE)
    KEYVALUE_ENTRY(EXPAND_CSCOPE, "cscope"),
#endif
#اگر defined(FEAT_EVAL)
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
#اگر defined(FEAT_KEYMAP)
    KEYVALUE_ENTRY(EXPAND_KEYMAP, "keymap"),
#endif
#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
#اگر defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_SCRIPTNAMES, "scriptnames"),
#endif
    KEYVALUE_ENTRY(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ENTRY(EXPAND_SHELLCMDLINE, "shellcmdline"),
#اگر defined(FEAT_SIGNS)
    KEYVALUE_ENTRY(EXPAND_SIGN, "sign"),
#endif
    KEYVALUE_ENTRY(EXPAND_OWNSYNTAX, "syntax"),
#اگر defined(FEAT_PROFILE)
    KEYVALUE_ENTRY(EXPAND_SYNTIME, "syntime"),
#endif
    KEYVALUE_ENTRY(EXPAND_TAGS, "tag"),
    KEYVALUE_ENTRY(EXPAND_TAGS_LISTFILES, "tag_listfiles"),
    KEYVALUE_ENTRY(EXPAND_USER, "user"),
    KEYVALUE_ENTRY(EXPAND_USER_VARS, "var")
};

تعریفنوع ساختار
{
    cmd_addr_T کلید;
    نویسه *fullname;
    size_t fullnamelen;
    نویسه *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#define ADDRTYPE_ENTRY(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
ایستا addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ENTRY(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ENTRY(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ENTRY(ADDR_سطرها, "lines", "line"),
    ADDRTYPE_ENTRY(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ENTRY(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ENTRY(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ENTRY(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ENTRY(ADDR_WINDOWS, "windows", "win")
};

ایستا صحیح cmp_addr_type(ثابت تهی *a, ثابت تهی *b);

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
    صحیح		*full,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    صحیح		*complp) // completion flags or NULL
{
    صحیح		len = (صحیح)(p - eap->cmd);
    صحیح		j, k, matchlen = 0;
    ucmd_T	*uc;
    صحیح		found = FALSE;
    صحیح		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    صحیح		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    برای (;;)
    {
	برای (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    وقتی (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    اگر (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		اگر (k == len && found && *np != NUL)
		{
		    اگر (gap == &ucmds)
			بازگشت پوچ;
		    amb_local = TRUE;
		}

		اگر (!found || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    اگر (k == len)
			found = TRUE;
		    وگرنه
			possible = TRUE;

		    اگر (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    وگرنه
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (long)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    اگر (complp != پوچ)
			*complp = uc->uc_compl;
#ifdef FEAT_EVAL
		    اگر (xp != پوچ)
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
		    اگر (k == len && *np == NUL)
		    {
			اگر (full != پوچ)
			    *full = TRUE;
			amb_local = FALSE;
			بشکن;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	اگر (j < gap->ga_len || gap == &ucmds)
	    بشکن;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    اگر (amb_local)
    {
	اگر (xp != پوچ)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	بازگشت پوچ;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    اگر (found || possible)
	بازگشت p + (matchlen - len);
    بازگشت p;
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
    وقتی (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	اگر (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    اگر (p == پوچ)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_pattern = arg;
		بازگشت پوچ;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    اگر (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_pattern = p + 1;
	    }
	    وگرنه اگر (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_pattern = p + 1;
		برای (char_u *c = p + 1; *c != NUL; ++c)
		    اگر (*c == ',')
			xp->xp_pattern = c + 1;
	    }
	    وگرنه اگر (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_pattern = p + 1;
	    }
	    وگرنه اگر (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_pattern = p + 1;
	    }
	    بازگشت پوچ;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    اگر (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_pattern = arg;
	بازگشت پوچ;
    }

    // And finally comes a normal command
    بازگشت skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
set_context_in_user_cmdarg(
	char_u		*cmd UNUSED,
	char_u		*arg,
	long		argt,
	صحیح		context,
	expand_T	*xp,
	صحیح		forceit)
{
    char_u	*p;

    اگر (context == EXPAND_NOTHING)
	بازگشت پوچ;

    اگر (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	بازگشت پوچ;
    }

#ifdef FEAT_MENU
    اگر (context == EXPAND_MENUS)
	بازگشت set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif
    اگر (context == EXPAND_COMMANDS)
	بازگشت arg;
    اگر (context == EXPAND_MAPPINGS)
	بازگشت set_context_in_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    اگر (!(argt & EX_ARGSPACE))
    {
	p = arg;
	وقتی (*p)
	{
	    اگر (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    وگرنه اگر (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_pattern = arg;
    xp->xp_context = context;

    بازگشت پوچ;
}

    char_u *
expand_user_command_name(صحیح idx)
{
    بازگشت get_user_commands(پوچ, idx - (صحیح)CMD_SIZE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp UNUSED, صحیح idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_buffer;

    اگر (idx < buf->b_ucmds.ga_len)
	بازگشت USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    اگر (idx < ucmds.ga_len)
    {
	صحیح	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	برای (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    اگر (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		بازگشت (char_u *)"";
	بازگشت name;
    }
    بازگشت پوچ;
}

#ifdef FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(صحیح idx, صحیح cmdidx)
{
    اگر (cmdidx == CMD_USER && idx < ucmds.ga_len)
	بازگشت USER_CMD(idx)->uc_name;
    اگر (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_buffer;

	اگر (idx < buf->b_ucmds.ga_len)
	    بازگشت USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    بازگشت پوچ;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    صحیح
user_command_info(صحیح idx, صحیح cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_buffer->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    اگر (idx >= gap->ga_len)
	بازگشت FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_add_string(d, "name", uc->uc_name);
    dict_add_string(d, "kind", (char_u *)"user");
    dict_add_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_add_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_to_str(uc->uc_compl, uc->uc_compl_arg);
    dict_add_string(d, "complete", compl == پوچ ? (char_u *)"" : compl);
    vim_free(compl);
    dict_add_string(d, "definition", uc->uc_rep);
    dict_add_عدد(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_add_عدد(d, "lnum", uc->uc_script_ctx.sc_lnum);
    بازگشت OK;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx < 0 || idx >= (صحیح)ARRAY_LENGTH(addr_type_complete_tab))
	بازگشت پوچ;
    بازگشت (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp UNUSED, صحیح idx)
{
    ایستا نویسه *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    اگر (idx < 0 || idx >= (صحیح)ARRAY_LENGTH(user_cmd_flags))
	بازگشت پوچ;
    بازگشت (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp UNUSED, صحیح idx)
{
    ایستا نویسه *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    اگر (idx < 0 || idx >= (صحیح)ARRAY_LENGTH(user_cmd_nargs))
	بازگشت پوچ;
    بازگشت (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx < 0 || idx >= (صحیح)ARRAY_LENGTH(command_complete_tab))
	بازگشت پوچ;
    بازگشت command_complete_tab[idx].مقدار.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
ایستا نویسه *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx < 0 || idx >= (صحیح)ARRAY_LENGTH(user_cmd_completeopt_tab))
	بازگشت پوچ;
    بازگشت (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    ایستا keyvalue_T *
get_commandtype(صحیح expand)
{
    صحیح i;

    برای (i = 0; i < (صحیح)ARRAY_LENGTH(command_complete_tab); ++i)
	اگر (command_complete_tab[i].کلید == expand)
	    بازگشت &command_complete_tab[i];

    بازگشت پوچ;
}

#ifdef FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_to_str(صحیح expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    اگر (kv == پوچ || kv->مقدار.string == پوچ)
	بازگشت پوچ;

    cmd_compl = kv->مقدار.string;
    اگر (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*buffer;

	buffer = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	اگر (buffer == پوچ)
	    بازگشت پوچ;
	sprintf((نویسه *)buffer, "%s,%s", cmd_compl, compl_arg);
	بازگشت buffer;
    }

    بازگشت vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    صحیح
cmdcomplete_str_to_type(char_u *complete_str)
{
    keyvalue_T target;
    keyvalue_T *entry;
    ایستا keyvalue_T *last_entry = پوچ;	// cached result

    اگر (STRNCMP(complete_str, "custom,", 7) == 0)
	بازگشت EXPAND_USER_DEFINED;
    اگر (STRNCMP(complete_str, "customlist,", 11) == 0)
	بازگشت EXPAND_USER_LIST;

    target.کلید = 0;
    target.مقدار.string = complete_str;
    target.مقدار.length = 0;			// not used, see cmp_keyvalue_value()

    اگر (last_entry != پوچ && cmp_keyvalue_مقدار(&target, last_entry) == 0)
	entry = last_entry;
    وگرنه
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    اندازه(command_complete_tab[0]),
	    cmp_keyvalue_مقدار);
	اگر (entry == پوچ)
	    بازگشت EXPAND_NOTHING;

	last_entry = entry;
    }

    بازگشت entry->کلید;
}
#endif

/*
 * List user commands starting with "name[name_len]".
 */
    ایستا تهی
uc_list(char_u *name, size_t name_len)
{
    صحیح		i, j;
    صحیح		found = FALSE;
    ucmd_T	*cmd;
    صحیح		len;
    صحیح		over;
    long	a;
    garray_T	*gap;
    keyvalue_T	*entry;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    برای (;;)
    {
	برای (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (long)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    اگر (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || message_filtered(cmd->uc_name))
		ادامه;

	    // Put out the title first time
	    اگر (!found)
		پیام_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    found = TRUE;
	    پیام_putchar('\n');
	    اگر (got_int)
		بشکن;

	    // Special cases
	    len = 4;
	    اگر (a & EX_BANG)
	    {
		پیام_putchar('!');
		--len;
	    }
	    اگر (a & EX_REGSTR)
	    {
		پیام_putchar('"');
		--len;
	    }
	    اگر (gap != &ucmds)
	    {
		پیام_putchar('b');
		--len;
	    }
	    اگر (a & EX_TRLBAR)
	    {
		پیام_putchar('|');
		--len;
	    }
	    اگر (len != 0)
		پیام_puts(&"    "[4 - len]);

	    پیام_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (صحیح)cmd->uc_namelen + 4;

	    اگر (len < 21)
	    {
		// Field padding spaces   12345678901234567
		ایستا نویسه spaces[18] = "                 ";
		پیام_puts(&spaces[len - 4]);
		len = 21;
	    }
	    پیام_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    انتخاب ((صحیح)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		حالت 0:				IObuff[len++] = '0'; بشکن;
		حالت (EX_EXTRA):		IObuff[len++] = '*'; بشکن;
		حالت (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; بشکن;
		حالت (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; بشکن;
		حالت (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; بشکن;
		حالت (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; بشکن;
	    }

	    انجامبده
	    {
		IObuff[len++] = ' ';
	    } وقتی (len < 5 - over);

	    // Address / Range
	    اگر (a & (EX_RANGE|EX_شمار))
	    {
		اگر (a & EX_شمار)
		{
		    // -count=N
		    len += vim_snprintf((نویسه *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		وگرنه اگر (a & EX_DFLALL)
		    IObuff[len++] = '%';
		وگرنه اگر (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((نویسه *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		وگرنه
		    IObuff[len++] = '.';
	    }

	    انجامبده
	    {
		IObuff[len++] = ' ';
	    } وقتی (len < 8 - over);

	    // Address Type
	    برای (j = 0; j < (صحیح)ARRAY_LENGTH(addr_type_complete_tab); ++j)
		اگر (addr_type_complete_tab[j].کلید != ADDR_سطرها
			&& addr_type_complete_tab[j].کلید == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (صحیح)addr_type_complete_tab[j].shortnamelen;
		    بشکن;
		}

	    انجامبده
	    {
		IObuff[len++] = ' ';
	    } وقتی (len < 13 - over);

	    // Completion
	    entry = get_commandtype(cmd->uc_compl);
	    اگر (entry != پوچ)
	    {
		STRCPY(IObuff + len, entry->مقدار.string);
		len += (صحیح)entry->مقدار.length;
#ifdef FEAT_EVAL
		اگر (p_verbose > 0 && cmd->uc_compl_arg != پوچ)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    اگر (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (صحیح)uc_compl_arglen;
		    }
		}
		اگر (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((نویسه *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#endif
	    }

	    انجامبده
	    {
		IObuff[len++] = ' ';
	    } وقتی (len < 25 - over);

	    IObuff[len] = NUL;
	    پیام_outtrans(IObuff);

	    پیام_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ifdef FEAT_EVAL
	    اگر (p_verbose > 0)
		last_set_پیام(cmd->uc_script_ctx);
#endif
	    out_flush();
	    ui_breakcheck();
	    اگر (got_int)
		بشکن;
	}
	اگر (gap == &ucmds || i < gap->ga_len)
	    بشکن;
	gap = &ucmds;
    }

    اگر (!found)
	پیام(_("No user-defined commands found"));

    --ucmd_locked;
}

    نویسه *
uc_fun_cmd(تهی)
{
    ایستا char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    صحیح		i;

    برای (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    بازگشت (نویسه *)IObuff;
}

/*
 * Parse address type argument
 */
    ایستا صحیح
parse_addr_type_arg(
    char_u	*مقدار,
    صحیح		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T target;
    addrtype_T *entry;
    ایستا addrtype_T *last_entry;	// cached result

    target.کلید = 0;
    target.fullname = (نویسه *)مقدار;
    target.fullnamelen = vallen;

    اگر (last_entry != پوچ && cmp_addr_type(&target, last_entry) == 0)
	entry = last_entry;
    وگرنه
    {
	entry = (addrtype_T *)bsearch(&target,
	    &addr_type_complete_tab,
	    ARRAY_LENGTH(addr_type_complete_tab),
	    اندازه(addr_type_complete_tab[0]),
	    cmp_addr_type);
	اگر (entry == پوچ)
	{
	    صحیح i;
	    char_u	*err = مقدار;

	    برای (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_address_type_مقدار_str), err);
	    بازگشت FAIL;
	}

	last_entry = entry;
    }

    *addr_type_arg = entry->کلید;

    بازگشت OK;
}

    ایستا صحیح
cmp_addr_type(ثابت تهی *a, ثابت تهی *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    بازگشت STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    صحیح
parse_compl_arg(
    char_u	*مقدار,
    صحیح		vallen,
    صحیح		*complp,
    long	*argt,
    char_u	**compl_arg UNUSED)
{
    char_u	*arg = پوچ;
#اگر defined(FEAT_EVAL)
    size_t	arglen = 0;
#endif
    صحیح		i;
    صحیح		valend = vallen;
    keyvalue_T	target;
    keyvalue_T	*entry;
    ایستا keyvalue_T	*last_entry = پوچ;	    // cached result

    // Look for any argument part - which is the part after any ','
    برای (i = 0; i < vallen; ++i)
    {
	اگر (مقدار[i] == ',')
	{
	    arg = &مقدار[i + 1];
#اگر defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#endif
	    valend = i;
	    بشکن;
	}
    }

    target.کلید = 0;
    target.مقدار.string = مقدار;
    target.مقدار.length = valend;

    اگر (last_entry != پوچ && cmp_keyvalue_مقدار_n(&target, last_entry) == 0)
	entry = last_entry;
    وگرنه
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    اندازه(command_complete_tab[0]),
	    cmp_keyvalue_مقدار_n);
	اگر (entry == پوچ)
	{
	    semsg(_(e_invalid_complete_مقدار_str), مقدار);
	    بازگشت FAIL;
	}

	last_entry = entry;
    }

    *complp = entry->کلید;
    اگر (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    وگرنه اگر (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    اگر (
#اگر defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#endif
								arg != پوچ)
    {
	emsg(_(e_completion_argument_only_allowed_for_custom_completion));
	بازگشت FAIL;
    }

#اگر defined(FEAT_EVAL)
    اگر ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == پوچ)
    {
	emsg(_(e_custom_completion_requires_function_argument));
	بازگشت FAIL;
    }

    اگر (arg != پوچ)
	*compl_arg = vim_strnsave(arg, arglen);
#endif

    بازگشت OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    ایستا صحیح
parse_completeopt_arg(char_u *مقدار, صحیح vallen, صحیح *compl_opt)
{
    char_u  *p = مقدار;
    char_u  *end = مقدار + vallen;
    صحیح	    flags = 0;

    اگر (vallen == 0)
    {
	semsg(_(e_argument_required_for_str), "-completeopt");
	بازگشت FAIL;
    }

    وقتی (p < end)
    {
	char_u	*comma;
	size_t	itemlen;
	صحیح	matched = FALSE;
	صحیح	i;

	comma = vim_strchr(p, ',');
	اگر (comma == پوچ || comma > end)
	    itemlen = (size_t)(end - p);
	وگرنه
	    itemlen = (size_t)(comma - p);

	اگر (itemlen == 0)
	{
	    semsg(_(e_invalid_مقدار_for_argument_str), "completeopt");
	    بازگشت FAIL;
	}

	برای (i = 0; i < (صحیح)ARRAY_LENGTH(user_cmd_completeopt_tab); ++i)
	{
	    نویسه *name = user_cmd_completeopt_tab[i];

	    اگر (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		اگر (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		بشکن;
	    }
	}
	اگر (!matched)
	{
	    semsg(_(e_invalid_مقدار_for_argument_str), "completeopt");
	    بازگشت FAIL;
	}

	p += itemlen;
	اگر (p < end && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    بازگشت OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    ایستا صحیح
uc_scan_attr(
    char_u	*attr,
    size_t	len,
    long	*argt,
    long	*def,
    صحیح		*flags,
    صحیح		*complp,
    char_u	**compl_arg,
    صحیح		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    اگر (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	بازگشت FAIL;
    }

    // First, try the simple attributes (no arguments)
    اگر (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    وگرنه اگر (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_BUFFER;
    وگرنه اگر (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    وگرنه اگر (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    وگرنه اگر (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    وگرنه
    {
	صحیح	i;
	char_u	*val = پوچ;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	برای (i = 0; i < (صحیح)len; ++i)
	{
	    اگر (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		بشکن;
	    }
	}

	اگر (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    اگر (vallen == 1)
	    {
		اگر (*val == '0')
		    // Do nothing - this is the default
		    ;
		وگرنه اگر (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		وگرنه اگر (*val == '*')
		    *argt |= EX_EXTRA;
		وگرنه اگر (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		وگرنه اگر (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		وگرنه اگر (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		وگرنه
		    برو wrong_nargs;
	    }
	    وگرنه
	    {
wrong_nargs:
		emsg(_(e_invalid_عدد_of_arguments));
		بازگشت FAIL;
	    }
	}
	وگرنه اگر (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    اگر (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    وگرنه اگر (val != پوچ)
	    {
		p = val;
		اگر (*def >= 0)
		{
two_شمار:
		    emsg(_(e_شمار_cannot_be_specified_twice));
		    بازگشت FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		اگر (p != val + vallen || vallen == 0)
		{
invalid_شمار:
		    emsg(_(e_invalid_default_مقدار_for_شمار));
		    بازگشت FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    اگر (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_سطرها;
	}
	وگرنه اگر (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_شمار | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    اگر (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    اگر (val != پوچ)
	    {
		p = val;
		اگر (*def >= 0)
		    برو two_شمار;

		*def = getdigits(&p);

		اگر (p != val + vallen)
		    برو invalid_شمار;
	    }

	    اگر (*def < 0)
		*def = 0;
	}
	وگرنه اگر (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    اگر (val == پوچ)
	    {
		semsg(_(e_argument_required_for_str), "-complete");
		بازگشت FAIL;
	    }

	    اگر (parse_compl_arg(val, (صحیح)vallen, complp, argt, compl_arg)
								      == FAIL)
		بازگشت FAIL;
	}
	وگرنه اگر (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    اگر (val == پوچ)
	    {
		semsg(_(e_argument_required_for_str), "-completeopt");
		بازگشت FAIL;
	    }

	    اگر (parse_completeopt_arg(val, (صحیح)vallen, compl_opt) == FAIL)
		بازگشت FAIL;
	}
	وگرنه اگر (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    اگر (val == پوچ)
	    {
		semsg(_(e_argument_required_for_str), "-addr");
		بازگشت FAIL;
	    }
	    اگر (parse_addr_type_arg(val, (صحیح)vallen, addr_type_arg) == FAIL)
		بازگشت FAIL;
	    اگر (*addr_type_arg != ADDR_سطرها)
		*argt |= EX_ZEROR;
	}
	وگرنه
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    بازگشت FAIL;
	}
    }

    بازگشت OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    ایستا صحیح
uc_add_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    long	argt,
    long	def,
    صحیح		flags,
    صحیح		compl,
    char_u	*compl_arg UNUSED,
    صحیح		compl_opt,
    cmd_addr_T	addr_type,
    صحیح		force)
{
    ucmd_T	*cmd = پوچ;
    char_u	*p;
    صحیح		i;
    صحیح		cmp = 1;
    char_u	*rep_buf = پوچ;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, پوچ);
    اگر (rep_buf == پوچ)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	اگر (rep_buf == پوچ)
	    بازگشت FAIL;
    }

    // get address of growarray: global or in curbuf
    اگر (flags & UC_BUFFER)
    {
	gap = &curbuf->b_ucmds;
	اگر (gap->ga_itemsize == 0)
	    ga_init2(gap, اندازه(ucmd_T), 4);
    }
    وگرنه
	gap = &ucmds;

    // Search for the command in the already defined commands.
    برای (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	اگر (cmp == 0)
	{
	    اگر (name_len < cmd->uc_namelen)
		cmp = -1;
	    وگرنه اگر (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	اگر (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    اگر (!force
#ifdef FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#endif
		    )
	    {
		semsg(_(e_command_already_exists_add_bang_to_replace_it_str),
									 name);
		برو fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#اگر defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#endif
	    بشکن;
	}

	// Stop as soon as we pass the name to add
	اگر (cmp < 0)
	    بشکن;
    }

    // Extend the array unless we're replacing an existing command
    اگر (cmp != 0)
    {
	اگر (ga_grow(gap, 1) == FAIL)
	    برو fail;
	اگر ((p = vim_strnsave(name, name_len)) == پوچ)
	    برو fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * اندازه(ucmd_T));

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
    اگر (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ifdef FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#endif
    cmd->uc_addr_type = addr_type;

    بازگشت OK;

fail:
    vim_free(rep_buf);
#اگر defined(FEAT_EVAL)
    vim_free(compl_arg);
#endif
    بازگشت FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_block(exarg_T *eap, char_u *p, char_u **tofree, صحیح *flags)
{
    char_u *retp = p;
    char_u *block = find_cmd_block_start(p);

    اگر (block != پوچ && eap->ea_getline != پوچ)
    {
	garray_T    ga;
	char_u	    *سطر = پوچ;

	ga_init2(&ga, اندازه(char_u *), 10);
	اگر (ga_copy_string(&ga, p) == FAIL)
	    بازگشت retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	اگر (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    برای (;;)
	    {
		vim_free(سطر);
		اگر ((سطر = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == پوچ)
		{
		    emsg(_(e_missing_rcurly));
		    بشکن;
		}
		اگر (ga_copy_string(&ga, سطر) == FAIL)
		    بشکن;
		اگر (*skipwhite(سطر) == '}')
		    بشکن;
	    }
	vim_free(سطر);
	retp = *tofree = ga_concat_strings(&ga, "\n");
	اگر (retp == پوچ)
	    retp = p;
	ga_clear_strings(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	اگر (block == p)
	    *flags |= UC_VIM9;
    }
    بازگشت retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    تهی
skip_cmd_block(exarg_T *eap)
{
    char_u	*tofree = پوچ;
    صحیح		flags = 0;

    (تهی)may_get_cmd_block(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    تهی
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*end;
    char_u	*p;
    long	argt = 0;
    long	def = -1;
    صحیح		flags = 0;
    صحیح		compl = EXPAND_NOTHING;
    char_u	*compl_arg = پوچ;
    صحیح		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    صحیح		has_attr = (eap->arg[0] == '-');
    صحیح		name_len;

    اگر (eap->skip)
    {
	skip_cmd_block(eap);
	بازگشت;
    }

    p = eap->arg;

    // Check for attributes
    وقتی (*p == '-')
    {
	++p;
	end = skiptowhite(p);
	اگر (uc_scan_attr(p, end - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    برو theend;
	p = skipwhite(end);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    اگر (ASCII_ISALPHA(*p))
	وقتی (ASCII_ISALNUM(*p))
	    ++p;
    اگر (!ends_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	برو theend;
    }
    end = p;
    name_len = (صحیح)(end - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(end);
    اگر (!has_attr && ends_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    وگرنه اگر (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_start_with_an_uppercase_letter));
    }
    وگرنه اگر ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_used_for_user_defined_command));
    }
    وگرنه اگر (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	اگر (in_vim9script())
	    emsg(_(e_complete_used_without_allowing_arguments));
	وگرنه
	    give_warning_with_source(
		       (char_u *)_(e_complete_used_without_allowing_arguments),
								   TRUE, TRUE);
    }
    وگرنه اگر ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_used_with_nargs_underscore));
    }
    وگرنه
    {
	char_u *tofree = پوچ;

	p = may_get_cmd_block(eap, p, &tofree, &flags);

	uc_add_command(name, end - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	بازگشت;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    تهی
ex_comclear(exarg_T *eap UNUSED)
{
    uc_clear(&ucmds);
    اگر (curbuf != پوچ)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ایستا صحیح
is_ucmd_locked(تهی)
{
    اگر (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    تهی
uc_clear(garray_T *gap)
{
    صحیح		i;
    ucmd_T	*cmd;

    اگر (is_ucmd_locked())
	بازگشت;

    برای (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#اگر defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#endif
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    تهی
ex_delcommand(exarg_T *eap)
{
    صحیح		i = 0;
    ucmd_T	*cmd = پوچ;
    صحیح		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    صحیح		buffer_only = FALSE;

    اگر (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	buffer_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    برای (;;)
    {
	برای (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    اگر (res <= 0)
		بشکن;
	}
	اگر (gap == &ucmds || res == 0 || buffer_only)
	    بشکن;
	gap = &ucmds;
    }

    اگر (res != 0)
    {
	semsg(_(buffer_only
		    ? e_no_such_user_defined_command_in_current_buffer_str
		    : e_no_such_user_defined_command_str), arg);
	بازگشت;
    }

    اگر (is_ucmd_locked())
	بازگشت;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#اگر defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#endif

    --gap->ga_len;

    اگر (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * اندازه(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    ایستا char_u *
uc_split_args(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    صحیح len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    وقتی (*p)
    {
	اگر (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	وگرنه اگر (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	وگرنه اگر (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	وگرنه اگر (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    اگر (*p == NUL)
		بشکن;
	    len += 4; // ", "
	}
	وگرنه
	{
	    صحیح charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    اگر (buf == پوچ)
    {
	*lenp = 0;
	بازگشت buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    وقتی (*p)
    {
	اگر (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	وگرنه اگر (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	وگرنه اگر (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	وگرنه اگر (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    اگر (*p == NUL)
		بشکن;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	وگرنه
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    بازگشت buf;
}

    ایستا size_t
add_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    نویسه	*mod_str,
    size_t	mod_strlen,
    صحیح		*multi_mods)
{
    اگر (buf != پوچ)
    {
	اگر (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    اگر (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    وگرنه
	*multi_mods = 1;

    بازگشت mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
add_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, صحیح *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    اگر (cmod->cmod_split & WSP_ABOVE)
	buflen += add_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    اگر (cmod->cmod_split & WSP_BELOW)
	buflen += add_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    اگر (cmod->cmod_split & WSP_BOT)
	buflen += add_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    اگر (cmod->cmod_tab > 0)
    {
	صحیح tabnr = cmod->cmod_tab - 1;

	اگر (tabnr == tabpage_index(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += add_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	وگرنه
	{
	    نویسه tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, اندازه(tab_buf), "%dtab", tabnr);
	    buflen += add_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    اگر (cmod->cmod_split & WSP_بالاترین)
	buflen += add_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    اگر (cmod->cmod_split & WSP_VERT)
	buflen += add_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    اگر (cmod->cmod_split & WSP_HOR)
	buflen += add_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    بازگشت buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, صحیح quote)
{
    size_t  buflen = 0;
    صحیح	    multi_mods = 0;
    صحیح	    i;
    ایستا keyvalue_T mod_entry_tab[] =
    {
#ifdef FEAT_BROWSE_CMD
	KEYVALUE_ENTRY(CMOD_BROWSE, "browse"),
#endif
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
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

    اگر (quote)
    {
	++buflen;
	اگر (buf != پوچ)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    وگرنه
    اگر (buf != پوچ)
	*buf = NUL;

    // the modifiers that are simple flags
    برای (i = 0; i < (صحیح)ARRAY_LENGTH(mod_entry_tab); ++i)
	اگر (cmod->cmod_flags & mod_entry_tab[i].کلید)
	    buflen += add_cmd_modifier(buf, buflen,
		    (نویسه *)mod_entry_tab[i].مقدار.string,
		    mod_entry_tab[i].مقدار.length, &multi_mods);

    // :silent
    اگر (cmod->cmod_flags & CMOD_SILENT)
    {
	اگر (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += add_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	وگرنه
	    buflen += add_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    اگر (cmod->cmod_verbose > 0)
    {
	صحیح verbose_مقدار = cmod->cmod_verbose - 1;

	اگر (verbose_مقدار == 1)
	    buflen += add_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	وگرنه
	{
	    نویسه verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, اندازه(verbose_buf), "%dverbose", verbose_مقدار);
	    buflen += add_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += add_win_cmd_modifiers((buf == پوچ) ? پوچ : buf + buflen, cmod, &multi_mods);

    اگر (quote)
    {
	اگر (buf == پوچ)
	    ++buflen;
	وگرنه
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    بازگشت buflen;
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
    ایستا size_t
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
    صحیح		quote = 0;
    شمارش {
	ct_ARGS,
	ct_BANG,
	ct_شمار,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_REGISTER,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    اگر ((vim_strchr((char_u *)"qQfF", *p) != پوچ) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    اگر (l <= 1)
	type = ct_NONE;
    وگرنه اگر (STRNICMP(p, "args>", l) == 0)
	type = ct_ARGS;
    وگرنه اگر (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    وگرنه اگر (STRNICMP(p, "count>", l) == 0)
	type = ct_شمار;
    وگرنه اگر (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    وگرنه اگر (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    وگرنه اگر (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    وگرنه اگر (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    وگرنه اگر (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_REGISTER;
    وگرنه اگر (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    انتخاب (type)
    {
    حالت ct_ARGS:
	// Simple case first
	اگر (*eap->arg == NUL)
	{
	    اگر (quote == 1)
	    {
		result = 2;
		اگر (buf != پوچ)
		    STRCPY(buf, "''");
	    }
	    وگرنه
		result = 0;
	    بشکن;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	اگر ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	انتخاب (quote)
	{
	حالت 0: // No quoting, no splitting
	    result = STRLEN(eap->arg);
	    اگر (buf != پوچ)
		STRCPY(buf, eap->arg);
	    بشکن;
	حالت 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    صحیح	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    result = STRLEN(eap->arg) + 2;
	    برای (p = eap->arg; *p; ++p)
	    {
		اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		وگرنه اگر (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    اگر (VIM_ISWHITE(p[1]))
			--result;
		    ++p;
		}
		وگرنه
		     اگر (*p == '\\' || *p == '"')
		    ++result;
	    }

	    اگر (buf != پوچ)
	    {
		*buf++ = '"';
		برای (p = eap->arg; *p; ++p)
		{
		    اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    وگرنه اگر (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			اگر (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    وگرنه
			 اگر (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    بشکن;
	}
	حالت 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    اگر (*split_buf == پوچ)
		*split_buf = uc_split_args(eap->arg, split_len);

	    result = *split_len;
	    اگر (buf != پوچ && result != 0)
		STRCPY(buf, *split_buf);

	    بشکن;
	}
	بشکن;

    حالت ct_BANG:
	result = eap->forceit ? 1 : 0;
	اگر (quote)
	    result += 2;
	اگر (buf != پوچ)
	{
	    اگر (quote)
		*buf++ = '"';
	    اگر (eap->forceit)
		*buf++ = '!';
	    اگر (quote)
		*buf = '"';
	}
	بشکن;

    حالت ct_LINE1:
    حالت ct_LINE2:
    حالت ct_RANGE:
    حالت ct_شمار:
    {
	نویسه رقم_buf[NUMBUFLEN];
	long رقم = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_شمار :
		   (eap->addr_شمار > 0) ? eap->line2 : cmd->uc_def;
	size_t رقم_len;

	رقم_len = vim_snprintf(رقم_buf, اندازه(رقم_buf), "%ld", رقم);
	result = رقم_len;

	اگر (quote)
	    result += 2;

	اگر (buf != پوچ)
	{
	    اگر (quote)
		*buf++ = '"';
	    STRCPY(buf, رقم_buf);
	    buf += رقم_len;
	    اگر (quote)
		*buf = '"';
	}

	بشکن;
    }

    حالت ct_MODS:
    {
	result = produce_cmdmods(buf, &cmdmod, quote);
	بشکن;
    }

    حالت ct_REGISTER:
	result = eap->regname ? 1 : 0;
	اگر (quote)
	    result += 2;
	اگر (buf != پوچ)
	{
	    اگر (quote)
		*buf++ = '\'';
	    اگر (eap->regname)
		*buf++ = eap->regname;
	    اگر (quote)
		*buf = '\'';
	}
	بشکن;

    حالت ct_LT:
	result = 1;
	اگر (buf != پوچ)
	    *buf = '<';
	بشکن;

    پیشفرض:
	// Not recognized: just copy the '<' and return -1.
	result = (size_t)-1;
	اگر (buf != پوچ)
	    *buf = '<';
	بشکن;
    }

    بازگشت result;
}

/*
 * Execute a user defined command.
 */
    تهی
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*start;
    char_u	*end = پوچ;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	split_len = 0;
    char_u	*split_buf = پوچ;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    صحیح		restore_current_sctx = FALSE;
#ifdef FEAT_EVAL
    صحیح		restore_script_version = 0;
#endif

    اگر (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    وگرنه
	cmd = USER_CMD_GA(&prevwin_curwin()->w_buffer->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = پوچ;
    برای (;;)
    {
	p = cmd->uc_rep;    // source
	q = buf;	    // destination
	totlen = 0;

	برای (;;)
	{
	    start = vim_strchr(p, '<');
	    اگر (start != پوچ)
		end = vim_strchr(start + 1, '>');
	    اگر (buf != پوچ)
	    {
		برای (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		اگر (*ksp == K_SPECIAL
			&& (start == پوچ || ksp < start || end == پوچ)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ifdef FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (صحیح)KE_CSI)
#endif
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    اگر (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    ادامه;
		}
	    }

	    // break if no <item> is found
	    اگر (start == پوچ || end == پوچ)
		بشکن;

	    // Include the '>'
	    ++end;

	    // Take everything up to the '<'
	    len = start - p;
	    اگر (buf == پوچ)
		totlen += len;
	    وگرنه
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_code(start, end - start, q, cmd, eap,
			     &split_buf, &split_len);
	    اگر (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = start + 1;
		len = 1;
	    }
	    وگرنه
		p = end;
	    اگر (buf == پوچ)
		totlen += len;
	    وگرنه
		q += len;
	}
	اگر (buf != پوچ)	    // second time here, finished
	{
	    STRCPY(q, p);
	    بشکن;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	اگر (buf == پوچ)
	{
	    vim_free(split_buf);
	    بازگشت;
	}
    }

    اگر ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_version = cmd->uc_script_ctx.sc_version;
#ifdef FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	اگر (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_version =
				  SCRIPT_ITEM(current_sctx.sc_sid)->sn_version;
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = SCRIPT_VERSION_VIM9;
	}
#endif
    }

    (تهی)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    اگر (restore_current_sctx)
    {
#ifdef FEAT_EVAL
	اگر (restore_script_version != 0)
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version =
							restore_script_version;
#endif
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(split_buf);
}
