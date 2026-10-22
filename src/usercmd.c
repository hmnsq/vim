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

#অন্তর্ভাব "vim.h"

প্রকারলেখ কাঠামো ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    long	uc_def;		// The default value for a range/count
    পূর্ণসংখ্যা		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    পূর্ণসংখ্যা		uc_flags;	// some UC_ flags
    পূর্ণসংখ্যা		uc_compl_opt;	// completion options (UCC_ flags)
#ifdef FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#endif
} ucmd_T;

// List of all user commands.
স্থির garray_T ucmds = {0, 0, আকার(ucmd_T), 4, ফাঁকা};

// When non-zero it is not allowed to add or remove user commands
স্থির পূর্ণসংখ্যা ucmd_locked = 0;

#define USER_CMD(i) (&((ucmd_T *)(ucmds.ga_তথ্য))[i])
#define USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_তথ্য))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
স্থির keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ENTRY(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ENTRY(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ENTRY(EXPAND_BEHAVE, "behave"),
#যদি defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_BREAKPOINT, "breakpoint"),
#endif
    KEYVALUE_ENTRY(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ENTRY(EXPAND_COLORS, "color"),
    KEYVALUE_ENTRY(EXPAND_COMMANDS, "command"),
    KEYVALUE_ENTRY(EXPAND_COMPILER, "compiler"),
#যদি defined(FEAT_CSCOPE)
    KEYVALUE_ENTRY(EXPAND_CSCOPE, "cscope"),
#endif
#যদি defined(FEAT_EVAL)
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
#যদি defined(FEAT_KEYMAP)
    KEYVALUE_ENTRY(EXPAND_KEYMAP, "keymap"),
#endif
#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
#যদি defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_SCRIPTNAMES, "scriptnames"),
#endif
    KEYVALUE_ENTRY(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ENTRY(EXPAND_SHELLCMDLINE, "shellcmdline"),
#যদি defined(FEAT_SIGNS)
    KEYVALUE_ENTRY(EXPAND_SIGN, "sign"),
#endif
    KEYVALUE_ENTRY(EXPAND_OWNSYNTAX, "syntax"),
#যদি defined(FEAT_PROFILE)
    KEYVALUE_ENTRY(EXPAND_SYNTIME, "syntime"),
#endif
    KEYVALUE_ENTRY(EXPAND_TAGS, "tag"),
    KEYVALUE_ENTRY(EXPAND_TAGS_LISTFILES, "tag_listfiles"),
    KEYVALUE_ENTRY(EXPAND_USER, "user"),
    KEYVALUE_ENTRY(EXPAND_USER_VARS, "var")
};

প্রকারলেখ কাঠামো
{
    cmd_addr_T চাবি;
    অক্ষর *fullname;
    size_t fullnamelen;
    অক্ষর *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#define ADDRTYPE_ENTRY(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
স্থির addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ENTRY(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ENTRY(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ENTRY(ADDR_সারিগুলি, "lines", "line"),
    ADDRTYPE_ENTRY(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ENTRY(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ENTRY(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ENTRY(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ENTRY(ADDR_WINDOWS, "windows", "win")
};

স্থির পূর্ণসংখ্যা cmp_addr_type(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b);

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
    পূর্ণসংখ্যা		*full,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    পূর্ণসংখ্যা		*complp) // completion flags or NULL
{
    পূর্ণসংখ্যা		len = (পূর্ণসংখ্যা)(p - eap->cmd);
    পূর্ণসংখ্যা		j, k, matchlen = 0;
    ucmd_T	*uc;
    পূর্ণসংখ্যা		found = FALSE;
    পূর্ণসংখ্যা		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    পূর্ণসংখ্যা		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_বাফার->b_ucmds;
    জন্য (;;)
    {
	জন্য (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    যখন (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    যদি (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		যদি (k == len && found && *np != NUL)
		{
		    যদি (gap == &ucmds)
			ফেরত ফাঁকা;
		    amb_local = TRUE;
		}

		যদি (!found || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    যদি (k == len)
			found = TRUE;
		    অন্যথা
			possible = TRUE;

		    যদি (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    অন্যথা
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (long)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    যদি (complp != ফাঁকা)
			*complp = uc->uc_compl;
#ifdef FEAT_EVAL
		    যদি (xp != ফাঁকা)
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
		    যদি (k == len && *np == NUL)
		    {
			যদি (full != ফাঁকা)
			    *full = TRUE;
			amb_local = FALSE;
			বিরতি;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	যদি (j < gap->ga_len || gap == &ucmds)
	    বিরতি;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    যদি (amb_local)
    {
	যদি (xp != ফাঁকা)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	ফেরত ফাঁকা;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    যদি (found || possible)
	ফেরত p + (matchlen - len);
    ফেরত p;
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
    যখন (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	যদি (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    যদি (p == ফাঁকা)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_pattern = arg;
		ফেরত ফাঁকা;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    যদি (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_pattern = p + 1;
	    }
	    অন্যথা যদি (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_pattern = p + 1;
		জন্য (char_u *c = p + 1; *c != NUL; ++c)
		    যদি (*c == ',')
			xp->xp_pattern = c + 1;
	    }
	    অন্যথা যদি (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_pattern = p + 1;
	    }
	    অন্যথা যদি (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_pattern = p + 1;
	    }
	    ফেরত ফাঁকা;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    যদি (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_pattern = arg;
	ফেরত ফাঁকা;
    }

    // And finally comes a normal command
    ফেরত skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
set_context_in_user_cmdarg(
	char_u		*cmd UNUSED,
	char_u		*arg,
	long		argt,
	পূর্ণসংখ্যা		context,
	expand_T	*xp,
	পূর্ণসংখ্যা		forceit)
{
    char_u	*p;

    যদি (context == EXPAND_NOTHING)
	ফেরত ফাঁকা;

    যদি (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	ফেরত ফাঁকা;
    }

#ifdef FEAT_MENU
    যদি (context == EXPAND_MENUS)
	ফেরত set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif
    যদি (context == EXPAND_COMMANDS)
	ফেরত arg;
    যদি (context == EXPAND_MAPPINGS)
	ফেরত set_context_in_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    যদি (!(argt & EX_ARGSPACE))
    {
	p = arg;
	যখন (*p)
	{
	    যদি (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    অন্যথা যদি (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_pattern = arg;
    xp->xp_context = context;

    ফেরত ফাঁকা;
}

    char_u *
expand_user_command_name(পূর্ণসংখ্যা idx)
{
    ফেরত get_user_commands(ফাঁকা, idx - (পূর্ণসংখ্যা)CMD_আকার);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_বাফার;

    যদি (idx < buf->b_ucmds.ga_len)
	ফেরত USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    যদি (idx < ucmds.ga_len)
    {
	পূর্ণসংখ্যা	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	জন্য (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    যদি (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		ফেরত (char_u *)"";
	ফেরত name;
    }
    ফেরত ফাঁকা;
}

#ifdef FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(পূর্ণসংখ্যা idx, পূর্ণসংখ্যা cmdidx)
{
    যদি (cmdidx == CMD_USER && idx < ucmds.ga_len)
	ফেরত USER_CMD(idx)->uc_name;
    যদি (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_বাফার;

	যদি (idx < buf->b_ucmds.ga_len)
	    ফেরত USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    ফেরত ফাঁকা;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    পূর্ণসংখ্যা
user_command_info(পূর্ণসংখ্যা idx, পূর্ণসংখ্যা cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_বাফার->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    যদি (idx >= gap->ga_len)
	ফেরত FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_add_string(d, "name", uc->uc_name);
    dict_add_string(d, "kind", (char_u *)"user");
    dict_add_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_add_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_to_str(uc->uc_compl, uc->uc_compl_arg);
    dict_add_string(d, "complete", compl == ফাঁকা ? (char_u *)"" : compl);
    vim_free(compl);
    dict_add_string(d, "definition", uc->uc_rep);
    dict_add_সংখ্যা(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_add_সংখ্যা(d, "lnum", uc->uc_script_ctx.sc_lnum);
    ফেরত OK;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    যদি (idx < 0 || idx >= (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(addr_type_complete_tab))
	ফেরত ফাঁকা;
    ফেরত (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    স্থির অক্ষর *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    যদি (idx < 0 || idx >= (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(user_cmd_flags))
	ফেরত ফাঁকা;
    ফেরত (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    স্থির অক্ষর *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    যদি (idx < 0 || idx >= (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(user_cmd_nargs))
	ফেরত ফাঁকা;
    ফেরত (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    যদি (idx < 0 || idx >= (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(command_complete_tab))
	ফেরত ফাঁকা;
    ফেরত command_complete_tab[idx].মান.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
স্থির অক্ষর *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    যদি (idx < 0 || idx >= (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(user_cmd_completeopt_tab))
	ফেরত ফাঁকা;
    ফেরত (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    স্থির keyvalue_T *
get_commandtype(পূর্ণসংখ্যা expand)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(command_complete_tab); ++i)
	যদি (command_complete_tab[i].চাবি == expand)
	    ফেরত &command_complete_tab[i];

    ফেরত ফাঁকা;
}

#ifdef FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_to_str(পূর্ণসংখ্যা expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    যদি (kv == ফাঁকা || kv->মান.string == ফাঁকা)
	ফেরত ফাঁকা;

    cmd_compl = kv->মান.string;
    যদি (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*বাফার;

	বাফার = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	যদি (বাফার == ফাঁকা)
	    ফেরত ফাঁকা;
	sprintf((অক্ষর *)বাফার, "%s,%s", cmd_compl, compl_arg);
	ফেরত বাফার;
    }

    ফেরত vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    পূর্ণসংখ্যা
cmdcomplete_str_to_type(char_u *complete_str)
{
    keyvalue_T target;
    keyvalue_T *entry;
    স্থির keyvalue_T *last_entry = ফাঁকা;	// cached result

    যদি (STRNCMP(complete_str, "custom,", 7) == 0)
	ফেরত EXPAND_USER_DEFINED;
    যদি (STRNCMP(complete_str, "customlist,", 11) == 0)
	ফেরত EXPAND_USER_LIST;

    target.চাবি = 0;
    target.মান.string = complete_str;
    target.মান.দৈর্ঘ্য = 0;			// not used, see cmp_keyvalue_value()

    যদি (last_entry != ফাঁকা && cmp_keyvalue_মান(&target, last_entry) == 0)
	entry = last_entry;
    অন্যথা
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_দৈর্ঘ্য(command_complete_tab),
	    আকার(command_complete_tab[0]),
	    cmp_keyvalue_মান);
	যদি (entry == ফাঁকা)
	    ফেরত EXPAND_NOTHING;

	last_entry = entry;
    }

    ফেরত entry->চাবি;
}
#endif

/*
 * List user commands starting with "name[name_len]".
 */
    স্থির শূন্য
uc_list(char_u *name, size_t name_len)
{
    পূর্ণসংখ্যা		i, j;
    পূর্ণসংখ্যা		found = FALSE;
    ucmd_T	*cmd;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		over;
    long	a;
    garray_T	*gap;
    keyvalue_T	*entry;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_বাফার->b_ucmds;
    জন্য (;;)
    {
	জন্য (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (long)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    যদি (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || message_filtered(cmd->uc_name))
		চালু;

	    // Put out the title first time
	    যদি (!found)
		বার্তা_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    found = TRUE;
	    বার্তা_putchar('\n');
	    যদি (got_int)
		বিরতি;

	    // Special cases
	    len = 4;
	    যদি (a & EX_BANG)
	    {
		বার্তা_putchar('!');
		--len;
	    }
	    যদি (a & EX_REGSTR)
	    {
		বার্তা_putchar('"');
		--len;
	    }
	    যদি (gap != &ucmds)
	    {
		বার্তা_putchar('b');
		--len;
	    }
	    যদি (a & EX_TRLBAR)
	    {
		বার্তা_putchar('|');
		--len;
	    }
	    যদি (len != 0)
		বার্তা_puts(&"    "[4 - len]);

	    বার্তা_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (পূর্ণসংখ্যা)cmd->uc_namelen + 4;

	    যদি (len < 21)
	    {
		// Field padding spaces   12345678901234567
		স্থির অক্ষর spaces[18] = "                 ";
		বার্তা_puts(&spaces[len - 4]);
		len = 21;
	    }
	    বার্তা_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    নির্বাচন ((পূর্ণসংখ্যা)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		ক্ষেত্র 0:				IObuff[len++] = '0'; বিরতি;
		ক্ষেত্র (EX_EXTRA):		IObuff[len++] = '*'; বিরতি;
		ক্ষেত্র (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; বিরতি;
		ক্ষেত্র (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; বিরতি;
		ক্ষেত্র (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; বিরতি;
		ক্ষেত্র (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; বিরতি;
	    }

	    করো
	    {
		IObuff[len++] = ' ';
	    } যখন (len < 5 - over);

	    // Address / Range
	    যদি (a & (EX_RANGE|EX_গুনতি))
	    {
		যদি (a & EX_গুনতি)
		{
		    // -count=N
		    len += vim_snprintf((অক্ষর *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		অন্যথা যদি (a & EX_DFLALL)
		    IObuff[len++] = '%';
		অন্যথা যদি (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((অক্ষর *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		অন্যথা
		    IObuff[len++] = '.';
	    }

	    করো
	    {
		IObuff[len++] = ' ';
	    } যখন (len < 8 - over);

	    // Address Type
	    জন্য (j = 0; j < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(addr_type_complete_tab); ++j)
		যদি (addr_type_complete_tab[j].চাবি != ADDR_সারিগুলি
			&& addr_type_complete_tab[j].চাবি == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (পূর্ণসংখ্যা)addr_type_complete_tab[j].shortnamelen;
		    বিরতি;
		}

	    করো
	    {
		IObuff[len++] = ' ';
	    } যখন (len < 13 - over);

	    // Completion
	    entry = get_commandtype(cmd->uc_compl);
	    যদি (entry != ফাঁকা)
	    {
		STRCPY(IObuff + len, entry->মান.string);
		len += (পূর্ণসংখ্যা)entry->মান.দৈর্ঘ্য;
#ifdef FEAT_EVAL
		যদি (p_verbose > 0 && cmd->uc_compl_arg != ফাঁকা)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    যদি (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (পূর্ণসংখ্যা)uc_compl_arglen;
		    }
		}
		যদি (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((অক্ষর *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#endif
	    }

	    করো
	    {
		IObuff[len++] = ' ';
	    } যখন (len < 25 - over);

	    IObuff[len] = NUL;
	    বার্তা_outtrans(IObuff);

	    বার্তা_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ifdef FEAT_EVAL
	    যদি (p_verbose > 0)
		last_set_বার্তা(cmd->uc_script_ctx);
#endif
	    out_flush();
	    ui_breakcheck();
	    যদি (got_int)
		বিরতি;
	}
	যদি (gap == &ucmds || i < gap->ga_len)
	    বিরতি;
	gap = &ucmds;
    }

    যদি (!found)
	বার্তা(_("No user-defined commands found"));

    --ucmd_locked;
}

    অক্ষর *
uc_fun_cmd(শূন্য)
{
    স্থির char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    ফেরত (অক্ষর *)IObuff;
}

/*
 * Parse address type argument
 */
    স্থির পূর্ণসংখ্যা
parse_addr_type_arg(
    char_u	*মান,
    পূর্ণসংখ্যা		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T target;
    addrtype_T *entry;
    স্থির addrtype_T *last_entry;	// cached result

    target.চাবি = 0;
    target.fullname = (অক্ষর *)মান;
    target.fullnamelen = vallen;

    যদি (last_entry != ফাঁকা && cmp_addr_type(&target, last_entry) == 0)
	entry = last_entry;
    অন্যথা
    {
	entry = (addrtype_T *)bsearch(&target,
	    &addr_type_complete_tab,
	    ARRAY_দৈর্ঘ্য(addr_type_complete_tab),
	    আকার(addr_type_complete_tab[0]),
	    cmp_addr_type);
	যদি (entry == ফাঁকা)
	{
	    পূর্ণসংখ্যা i;
	    char_u	*err = মান;

	    জন্য (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_address_type_মান_str), err);
	    ফেরত FAIL;
	}

	last_entry = entry;
    }

    *addr_type_arg = entry->চাবি;

    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
cmp_addr_type(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    ফেরত STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    পূর্ণসংখ্যা
parse_compl_arg(
    char_u	*মান,
    পূর্ণসংখ্যা		vallen,
    পূর্ণসংখ্যা		*complp,
    long	*argt,
    char_u	**compl_arg UNUSED)
{
    char_u	*arg = ফাঁকা;
#যদি defined(FEAT_EVAL)
    size_t	arglen = 0;
#endif
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		valend = vallen;
    keyvalue_T	target;
    keyvalue_T	*entry;
    স্থির keyvalue_T	*last_entry = ফাঁকা;	    // cached result

    // Look for any argument part - which is the part after any ','
    জন্য (i = 0; i < vallen; ++i)
    {
	যদি (মান[i] == ',')
	{
	    arg = &মান[i + 1];
#যদি defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#endif
	    valend = i;
	    বিরতি;
	}
    }

    target.চাবি = 0;
    target.মান.string = মান;
    target.মান.দৈর্ঘ্য = valend;

    যদি (last_entry != ফাঁকা && cmp_keyvalue_মান_n(&target, last_entry) == 0)
	entry = last_entry;
    অন্যথা
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_দৈর্ঘ্য(command_complete_tab),
	    আকার(command_complete_tab[0]),
	    cmp_keyvalue_মান_n);
	যদি (entry == ফাঁকা)
	{
	    semsg(_(e_invalid_complete_মান_str), মান);
	    ফেরত FAIL;
	}

	last_entry = entry;
    }

    *complp = entry->চাবি;
    যদি (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    অন্যথা যদি (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    যদি (
#যদি defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#endif
								arg != ফাঁকা)
    {
	emsg(_(e_completion_argument_only_allowed_for_custom_completion));
	ফেরত FAIL;
    }

#যদি defined(FEAT_EVAL)
    যদি ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == ফাঁকা)
    {
	emsg(_(e_custom_completion_requires_function_argument));
	ফেরত FAIL;
    }

    যদি (arg != ফাঁকা)
	*compl_arg = vim_strnsave(arg, arglen);
#endif

    ফেরত OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    স্থির পূর্ণসংখ্যা
parse_completeopt_arg(char_u *মান, পূর্ণসংখ্যা vallen, পূর্ণসংখ্যা *compl_opt)
{
    char_u  *p = মান;
    char_u  *end = মান + vallen;
    পূর্ণসংখ্যা	    flags = 0;

    যদি (vallen == 0)
    {
	semsg(_(e_argument_required_for_str), "-completeopt");
	ফেরত FAIL;
    }

    যখন (p < end)
    {
	char_u	*comma;
	size_t	itemlen;
	পূর্ণসংখ্যা	matched = FALSE;
	পূর্ণসংখ্যা	i;

	comma = vim_strchr(p, ',');
	যদি (comma == ফাঁকা || comma > end)
	    itemlen = (size_t)(end - p);
	অন্যথা
	    itemlen = (size_t)(comma - p);

	যদি (itemlen == 0)
	{
	    semsg(_(e_invalid_মান_for_argument_str), "completeopt");
	    ফেরত FAIL;
	}

	জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(user_cmd_completeopt_tab); ++i)
	{
	    অক্ষর *name = user_cmd_completeopt_tab[i];

	    যদি (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		যদি (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		বিরতি;
	    }
	}
	যদি (!matched)
	{
	    semsg(_(e_invalid_মান_for_argument_str), "completeopt");
	    ফেরত FAIL;
	}

	p += itemlen;
	যদি (p < end && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    ফেরত OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    স্থির পূর্ণসংখ্যা
uc_scan_attr(
    char_u	*attr,
    size_t	len,
    long	*argt,
    long	*def,
    পূর্ণসংখ্যা		*flags,
    পূর্ণসংখ্যা		*complp,
    char_u	**compl_arg,
    পূর্ণসংখ্যা		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    যদি (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	ফেরত FAIL;
    }

    // First, try the simple attributes (no arguments)
    যদি (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    অন্যথা যদি (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_বাফার;
    অন্যথা যদি (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    অন্যথা যদি (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    অন্যথা যদি (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    অন্যথা
    {
	পূর্ণসংখ্যা	i;
	char_u	*val = ফাঁকা;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	জন্য (i = 0; i < (পূর্ণসংখ্যা)len; ++i)
	{
	    যদি (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		বিরতি;
	    }
	}

	যদি (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    যদি (vallen == 1)
	    {
		যদি (*val == '0')
		    // Do nothing - this is the default
		    ;
		অন্যথা যদি (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		অন্যথা যদি (*val == '*')
		    *argt |= EX_EXTRA;
		অন্যথা যদি (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		অন্যথা যদি (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		অন্যথা যদি (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		অন্যথা
		    যাও wrong_nargs;
	    }
	    অন্যথা
	    {
wrong_nargs:
		emsg(_(e_invalid_সংখ্যা_of_arguments));
		ফেরত FAIL;
	    }
	}
	অন্যথা যদি (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    যদি (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    অন্যথা যদি (val != ফাঁকা)
	    {
		p = val;
		যদি (*def >= 0)
		{
two_গুনতি:
		    emsg(_(e_গুনতি_cannot_be_specified_twice));
		    ফেরত FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		যদি (p != val + vallen || vallen == 0)
		{
invalid_গুনতি:
		    emsg(_(e_invalid_default_মান_for_গুনতি));
		    ফেরত FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    যদি (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_সারিগুলি;
	}
	অন্যথা যদি (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_গুনতি | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    যদি (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    যদি (val != ফাঁকা)
	    {
		p = val;
		যদি (*def >= 0)
		    যাও two_গুনতি;

		*def = getdigits(&p);

		যদি (p != val + vallen)
		    যাও invalid_গুনতি;
	    }

	    যদি (*def < 0)
		*def = 0;
	}
	অন্যথা যদি (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    যদি (val == ফাঁকা)
	    {
		semsg(_(e_argument_required_for_str), "-complete");
		ফেরত FAIL;
	    }

	    যদি (parse_compl_arg(val, (পূর্ণসংখ্যা)vallen, complp, argt, compl_arg)
								      == FAIL)
		ফেরত FAIL;
	}
	অন্যথা যদি (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    যদি (val == ফাঁকা)
	    {
		semsg(_(e_argument_required_for_str), "-completeopt");
		ফেরত FAIL;
	    }

	    যদি (parse_completeopt_arg(val, (পূর্ণসংখ্যা)vallen, compl_opt) == FAIL)
		ফেরত FAIL;
	}
	অন্যথা যদি (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    যদি (val == ফাঁকা)
	    {
		semsg(_(e_argument_required_for_str), "-addr");
		ফেরত FAIL;
	    }
	    যদি (parse_addr_type_arg(val, (পূর্ণসংখ্যা)vallen, addr_type_arg) == FAIL)
		ফেরত FAIL;
	    যদি (*addr_type_arg != ADDR_সারিগুলি)
		*argt |= EX_ZEROR;
	}
	অন্যথা
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    ফেরত FAIL;
	}
    }

    ফেরত OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    স্থির পূর্ণসংখ্যা
uc_add_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    long	argt,
    long	def,
    পূর্ণসংখ্যা		flags,
    পূর্ণসংখ্যা		compl,
    char_u	*compl_arg UNUSED,
    পূর্ণসংখ্যা		compl_opt,
    cmd_addr_T	addr_type,
    পূর্ণসংখ্যা		force)
{
    ucmd_T	*cmd = ফাঁকা;
    char_u	*p;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		cmp = 1;
    char_u	*rep_buf = ফাঁকা;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, ফাঁকা);
    যদি (rep_buf == ফাঁকা)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	যদি (rep_buf == ফাঁকা)
	    ফেরত FAIL;
    }

    // get address of growarray: global or in curbuf
    যদি (flags & UC_বাফার)
    {
	gap = &curbuf->b_ucmds;
	যদি (gap->ga_itemsize == 0)
	    ga_init2(gap, আকার(ucmd_T), 4);
    }
    অন্যথা
	gap = &ucmds;

    // Search for the command in the already defined commands.
    জন্য (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	যদি (cmp == 0)
	{
	    যদি (name_len < cmd->uc_namelen)
		cmp = -1;
	    অন্যথা যদি (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	যদি (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    যদি (!force
#ifdef FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#endif
		    )
	    {
		semsg(_(e_command_already_exists_add_bang_to_replace_it_str),
									 name);
		যাও fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#যদি defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#endif
	    বিরতি;
	}

	// Stop as soon as we pass the name to add
	যদি (cmp < 0)
	    বিরতি;
    }

    // Extend the array unless we're replacing an existing command
    যদি (cmp != 0)
    {
	যদি (ga_grow(gap, 1) == FAIL)
	    যাও fail;
	যদি ((p = vim_strnsave(name, name_len)) == ফাঁকা)
	    যাও fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * আকার(ucmd_T));

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
    যদি (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ifdef FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#endif
    cmd->uc_addr_type = addr_type;

    ফেরত OK;

fail:
    vim_free(rep_buf);
#যদি defined(FEAT_EVAL)
    vim_free(compl_arg);
#endif
    ফেরত FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_block(exarg_T *eap, char_u *p, char_u **tofree, পূর্ণসংখ্যা *flags)
{
    char_u *retp = p;
    char_u *block = find_cmd_block_start(p);

    যদি (block != ফাঁকা && eap->ea_getline != ফাঁকা)
    {
	garray_T    ga;
	char_u	    *সারি = ফাঁকা;

	ga_init2(&ga, আকার(char_u *), 10);
	যদি (ga_copy_string(&ga, p) == FAIL)
	    ফেরত retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	যদি (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    জন্য (;;)
	    {
		vim_free(সারি);
		যদি ((সারি = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == ফাঁকা)
		{
		    emsg(_(e_missing_rcurly));
		    বিরতি;
		}
		যদি (ga_copy_string(&ga, সারি) == FAIL)
		    বিরতি;
		যদি (*skipwhite(সারি) == '}')
		    বিরতি;
	    }
	vim_free(সারি);
	retp = *tofree = ga_concat_strings(&ga, "\n");
	যদি (retp == ফাঁকা)
	    retp = p;
	ga_clear_strings(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	যদি (block == p)
	    *flags |= UC_VIM9;
    }
    ফেরত retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    শূন্য
skip_cmd_block(exarg_T *eap)
{
    char_u	*tofree = ফাঁকা;
    পূর্ণসংখ্যা		flags = 0;

    (শূন্য)may_get_cmd_block(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    শূন্য
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*end;
    char_u	*p;
    long	argt = 0;
    long	def = -1;
    পূর্ণসংখ্যা		flags = 0;
    পূর্ণসংখ্যা		compl = EXPAND_NOTHING;
    char_u	*compl_arg = ফাঁকা;
    পূর্ণসংখ্যা		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    পূর্ণসংখ্যা		has_attr = (eap->arg[0] == '-');
    পূর্ণসংখ্যা		name_len;

    যদি (eap->skip)
    {
	skip_cmd_block(eap);
	ফেরত;
    }

    p = eap->arg;

    // Check for attributes
    যখন (*p == '-')
    {
	++p;
	end = skiptowhite(p);
	যদি (uc_scan_attr(p, end - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    যাও theend;
	p = skipwhite(end);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    যদি (ASCII_ISALPHA(*p))
	যখন (ASCII_ISALNUM(*p))
	    ++p;
    যদি (!ends_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	যাও theend;
    }
    end = p;
    name_len = (পূর্ণসংখ্যা)(end - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(end);
    যদি (!has_attr && ends_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    অন্যথা যদি (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_start_with_an_uppercase_letter));
    }
    অন্যথা যদি ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_used_for_user_defined_command));
    }
    অন্যথা যদি (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	যদি (in_vim9script())
	    emsg(_(e_complete_used_without_allowing_arguments));
	অন্যথা
	    give_warning_with_source(
		       (char_u *)_(e_complete_used_without_allowing_arguments),
								   TRUE, TRUE);
    }
    অন্যথা যদি ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_used_with_nargs_underscore));
    }
    অন্যথা
    {
	char_u *tofree = ফাঁকা;

	p = may_get_cmd_block(eap, p, &tofree, &flags);

	uc_add_command(name, end - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	ফেরত;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    শূন্য
ex_comclear(exarg_T *eap UNUSED)
{
    uc_clear(&ucmds);
    যদি (curbuf != ফাঁকা)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    স্থির পূর্ণসংখ্যা
is_ucmd_locked(শূন্য)
{
    যদি (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    শূন্য
uc_clear(garray_T *gap)
{
    পূর্ণসংখ্যা		i;
    ucmd_T	*cmd;

    যদি (is_ucmd_locked())
	ফেরত;

    জন্য (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#যদি defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#endif
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    শূন্য
ex_delcommand(exarg_T *eap)
{
    পূর্ণসংখ্যা		i = 0;
    ucmd_T	*cmd = ফাঁকা;
    পূর্ণসংখ্যা		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    পূর্ণসংখ্যা		বাফার_only = FALSE;

    যদি (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	বাফার_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    জন্য (;;)
    {
	জন্য (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    যদি (res <= 0)
		বিরতি;
	}
	যদি (gap == &ucmds || res == 0 || বাফার_only)
	    বিরতি;
	gap = &ucmds;
    }

    যদি (res != 0)
    {
	semsg(_(বাফার_only
		    ? e_no_such_user_defined_command_in_current_বাফার_str
		    : e_no_such_user_defined_command_str), arg);
	ফেরত;
    }

    যদি (is_ucmd_locked())
	ফেরত;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#যদি defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#endif

    --gap->ga_len;

    যদি (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * আকার(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    স্থির char_u *
uc_split_args(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    পূর্ণসংখ্যা len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    যখন (*p)
    {
	যদি (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	অন্যথা যদি (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	অন্যথা যদি (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	অন্যথা যদি (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    যদি (*p == NUL)
		বিরতি;
	    len += 4; // ", "
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    যদি (buf == ফাঁকা)
    {
	*lenp = 0;
	ফেরত buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    যখন (*p)
    {
	যদি (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	অন্যথা যদি (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	অন্যথা যদি (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	অন্যথা যদি (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    যদি (*p == NUL)
		বিরতি;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	অন্যথা
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    ফেরত buf;
}

    স্থির size_t
add_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    অক্ষর	*mod_str,
    size_t	mod_strlen,
    পূর্ণসংখ্যা		*multi_mods)
{
    যদি (buf != ফাঁকা)
    {
	যদি (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    যদি (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    অন্যথা
	*multi_mods = 1;

    ফেরত mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
add_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, পূর্ণসংখ্যা *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    যদি (cmod->cmod_split & WSP_ABOVE)
	buflen += add_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    যদি (cmod->cmod_split & WSP_BELOW)
	buflen += add_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    যদি (cmod->cmod_split & WSP_BOT)
	buflen += add_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    যদি (cmod->cmod_tab > 0)
    {
	পূর্ণসংখ্যা tabnr = cmod->cmod_tab - 1;

	যদি (tabnr == tabpage_সূচক(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += add_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	অন্যথা
	{
	    অক্ষর tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, আকার(tab_buf), "%dtab", tabnr);
	    buflen += add_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    যদি (cmod->cmod_split & WSP_শীর্ষ)
	buflen += add_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    যদি (cmod->cmod_split & WSP_VERT)
	buflen += add_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    যদি (cmod->cmod_split & WSP_HOR)
	buflen += add_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    ফেরত buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, পূর্ণসংখ্যা quote)
{
    size_t  buflen = 0;
    পূর্ণসংখ্যা	    multi_mods = 0;
    পূর্ণসংখ্যা	    i;
    স্থির keyvalue_T mod_entry_tab[] =
    {
#ifdef FEAT_BROWSE_CMD
	KEYVALUE_ENTRY(CMOD_BROWSE, "browse"),
#endif
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
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

    যদি (quote)
    {
	++buflen;
	যদি (buf != ফাঁকা)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    অন্যথা
    যদি (buf != ফাঁকা)
	*buf = NUL;

    // the modifiers that are simple flags
    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(mod_entry_tab); ++i)
	যদি (cmod->cmod_flags & mod_entry_tab[i].চাবি)
	    buflen += add_cmd_modifier(buf, buflen,
		    (অক্ষর *)mod_entry_tab[i].মান.string,
		    mod_entry_tab[i].মান.দৈর্ঘ্য, &multi_mods);

    // :silent
    যদি (cmod->cmod_flags & CMOD_SILENT)
    {
	যদি (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += add_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	অন্যথা
	    buflen += add_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    যদি (cmod->cmod_verbose > 0)
    {
	পূর্ণসংখ্যা verbose_মান = cmod->cmod_verbose - 1;

	যদি (verbose_মান == 1)
	    buflen += add_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	অন্যথা
	{
	    অক্ষর verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, আকার(verbose_buf), "%dverbose", verbose_মান);
	    buflen += add_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += add_win_cmd_modifiers((buf == ফাঁকা) ? ফাঁকা : buf + buflen, cmod, &multi_mods);

    যদি (quote)
    {
	যদি (buf == ফাঁকা)
	    ++buflen;
	অন্যথা
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    ফেরত buflen;
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
    স্থির size_t
uc_check_code(
    char_u	*code,
    size_t	len,
    char_u	*buf,
    ucmd_T	*cmd,		// the user command we're expanding
    exarg_T	*eap,		// ex arguments
    char_u	**split_buf,
    size_t	*split_len)
{
    size_t	ফলাফল = 0;
    char_u	*p = code + 1;
    size_t	l = len - 2;
    পূর্ণসংখ্যা		quote = 0;
    গণনা {
	ct_ARGS,
	ct_BANG,
	ct_গুনতি,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_REGISTER,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    যদি ((vim_strchr((char_u *)"qQfF", *p) != ফাঁকা) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    যদি (l <= 1)
	type = ct_NONE;
    অন্যথা যদি (STRNICMP(p, "args>", l) == 0)
	type = ct_ARGS;
    অন্যথা যদি (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    অন্যথা যদি (STRNICMP(p, "count>", l) == 0)
	type = ct_গুনতি;
    অন্যথা যদি (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    অন্যথা যদি (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    অন্যথা যদি (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    অন্যথা যদি (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    অন্যথা যদি (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_REGISTER;
    অন্যথা যদি (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    নির্বাচন (type)
    {
    ক্ষেত্র ct_ARGS:
	// Simple case first
	যদি (*eap->arg == NUL)
	{
	    যদি (quote == 1)
	    {
		ফলাফল = 2;
		যদি (buf != ফাঁকা)
		    STRCPY(buf, "''");
	    }
	    অন্যথা
		ফলাফল = 0;
	    বিরতি;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	যদি ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	নির্বাচন (quote)
	{
	ক্ষেত্র 0: // No quoting, no splitting
	    ফলাফল = STRLEN(eap->arg);
	    যদি (buf != ফাঁকা)
		STRCPY(buf, eap->arg);
	    বিরতি;
	ক্ষেত্র 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    পূর্ণসংখ্যা	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    ফলাফল = STRLEN(eap->arg) + 2;
	    জন্য (p = eap->arg; *p; ++p)
	    {
		যদি (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		অন্যথা যদি (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    যদি (VIM_ISWHITE(p[1]))
			--ফলাফল;
		    ++p;
		}
		অন্যথা
		     যদি (*p == '\\' || *p == '"')
		    ++ফলাফল;
	    }

	    যদি (buf != ফাঁকা)
	    {
		*buf++ = '"';
		জন্য (p = eap->arg; *p; ++p)
		{
		    যদি (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    অন্যথা যদি (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			যদি (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    অন্যথা
			 যদি (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    বিরতি;
	}
	ক্ষেত্র 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    যদি (*split_buf == ফাঁকা)
		*split_buf = uc_split_args(eap->arg, split_len);

	    ফলাফল = *split_len;
	    যদি (buf != ফাঁকা && ফলাফল != 0)
		STRCPY(buf, *split_buf);

	    বিরতি;
	}
	বিরতি;

    ক্ষেত্র ct_BANG:
	ফলাফল = eap->forceit ? 1 : 0;
	যদি (quote)
	    ফলাফল += 2;
	যদি (buf != ফাঁকা)
	{
	    যদি (quote)
		*buf++ = '"';
	    যদি (eap->forceit)
		*buf++ = '!';
	    যদি (quote)
		*buf = '"';
	}
	বিরতি;

    ক্ষেত্র ct_LINE1:
    ক্ষেত্র ct_LINE2:
    ক্ষেত্র ct_RANGE:
    ক্ষেত্র ct_গুনতি:
    {
	অক্ষর অঙ্ক_buf[NUMBUFLEN];
	long অঙ্ক = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_গুনতি :
		   (eap->addr_গুনতি > 0) ? eap->line2 : cmd->uc_def;
	size_t অঙ্ক_len;

	অঙ্ক_len = vim_snprintf(অঙ্ক_buf, আকার(অঙ্ক_buf), "%ld", অঙ্ক);
	ফলাফল = অঙ্ক_len;

	যদি (quote)
	    ফলাফল += 2;

	যদি (buf != ফাঁকা)
	{
	    যদি (quote)
		*buf++ = '"';
	    STRCPY(buf, অঙ্ক_buf);
	    buf += অঙ্ক_len;
	    যদি (quote)
		*buf = '"';
	}

	বিরতি;
    }

    ক্ষেত্র ct_MODS:
    {
	ফলাফল = produce_cmdmods(buf, &cmdmod, quote);
	বিরতি;
    }

    ক্ষেত্র ct_REGISTER:
	ফলাফল = eap->regname ? 1 : 0;
	যদি (quote)
	    ফলাফল += 2;
	যদি (buf != ফাঁকা)
	{
	    যদি (quote)
		*buf++ = '\'';
	    যদি (eap->regname)
		*buf++ = eap->regname;
	    যদি (quote)
		*buf = '\'';
	}
	বিরতি;

    ক্ষেত্র ct_LT:
	ফলাফল = 1;
	যদি (buf != ফাঁকা)
	    *buf = '<';
	বিরতি;

    পূর্বনির্ধারিত:
	// Not recognized: just copy the '<' and return -1.
	ফলাফল = (size_t)-1;
	যদি (buf != ফাঁকা)
	    *buf = '<';
	বিরতি;
    }

    ফেরত ফলাফল;
}

/*
 * Execute a user defined command.
 */
    শূন্য
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*start;
    char_u	*end = ফাঁকা;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	split_len = 0;
    char_u	*split_buf = ফাঁকা;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    পূর্ণসংখ্যা		restore_current_sctx = FALSE;
#ifdef FEAT_EVAL
    পূর্ণসংখ্যা		restore_script_version = 0;
#endif

    যদি (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    অন্যথা
	cmd = USER_CMD_GA(&prevwin_curwin()->w_বাফার->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = ফাঁকা;
    জন্য (;;)
    {
	p = cmd->uc_rep;    // source
	q = buf;	    // destination
	totlen = 0;

	জন্য (;;)
	{
	    start = vim_strchr(p, '<');
	    যদি (start != ফাঁকা)
		end = vim_strchr(start + 1, '>');
	    যদি (buf != ফাঁকা)
	    {
		জন্য (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		যদি (*ksp == K_SPECIAL
			&& (start == ফাঁকা || ksp < start || end == ফাঁকা)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ifdef FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (পূর্ণসংখ্যা)KE_CSI)
#endif
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    যদি (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    চালু;
		}
	    }

	    // break if no <item> is found
	    যদি (start == ফাঁকা || end == ফাঁকা)
		বিরতি;

	    // Include the '>'
	    ++end;

	    // Take everything up to the '<'
	    len = start - p;
	    যদি (buf == ফাঁকা)
		totlen += len;
	    অন্যথা
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_code(start, end - start, q, cmd, eap,
			     &split_buf, &split_len);
	    যদি (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = start + 1;
		len = 1;
	    }
	    অন্যথা
		p = end;
	    যদি (buf == ফাঁকা)
		totlen += len;
	    অন্যথা
		q += len;
	}
	যদি (buf != ফাঁকা)	    // second time here, finished
	{
	    STRCPY(q, p);
	    বিরতি;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	যদি (buf == ফাঁকা)
	{
	    vim_free(split_buf);
	    ফেরত;
	}
    }

    যদি ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_version = cmd->uc_script_ctx.sc_version;
#ifdef FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	যদি (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_version =
				  SCRIPT_ITEM(current_sctx.sc_sid)->sn_version;
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = SCRIPT_VERSION_VIM9;
	}
#endif
    }

    (শূন্য)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    যদি (restore_current_sctx)
    {
#ifdef FEAT_EVAL
	যদি (restore_script_version != 0)
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version =
							restore_script_version;
#endif
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(split_buf);
}
