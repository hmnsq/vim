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

#ضمن "vim.h"

تعريفنوع بنية ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    ڊگهو	uc_def;		// The default value for a range/count
    صحيح		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    صحيح		uc_flags;	// some UC_ flags
    صحيح		uc_compl_opt;	// completion options (UCC_ flags)
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#جيڪڏهن_پورو
} ucmd_T;

// List of all user commands.
ساكن garray_T ucmds = {0, 0, حجم(ucmd_T), 4, عدم};

// When non-zero it is not allowed to add or remove user commands
ساكن صحيح ucmd_locked = 0;

#وصف_ڪريو USER_CMD(i) (&((ucmd_T *)(ucmds.ga_معلومات))[i])
#وصف_ڪريو USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_معلومات))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
ساكن keyvalue_T command_complete_tab[] =
{
    KEYVALUE_داخلا(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_داخلا(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_داخلا(EXPAND_BEHAVE, "behave"),
#إذا defined(FEAT_EVAL)
    KEYVALUE_داخلا(EXPAND_BREAKPOINT, "breakpoint"),
#جيڪڏهن_پورو
    KEYVALUE_داخلا(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_داخلا(EXPAND_COLORS, "color"),
    KEYVALUE_داخلا(EXPAND_COMMANDS, "command"),
    KEYVALUE_داخلا(EXPAND_COMPILER, "compiler"),
#إذا defined(FEAT_CSCOPE)
    KEYVALUE_داخلا(EXPAND_CSCOPE, "cscope"),
#جيڪڏهن_پورو
#إذا defined(FEAT_EVAL)
    KEYVALUE_داخلا(EXPAND_USER_DEFINED, "custom"),
    KEYVALUE_داخلا(EXPAND_USER_LIST, "customlist"),
#جيڪڏهن_پورو
    KEYVALUE_داخلا(EXPAND_DIFF_BUFFERS, "diff_buffer"),
    KEYVALUE_داخلا(EXPAND_DIRECTORIES, "dir"),
    KEYVALUE_داخلا(EXPAND_DIRS_اندريون_CDPATH, "dir_in_path"),
    KEYVALUE_داخلا(EXPAND_ENV_VARS, "environment"),
    KEYVALUE_داخلا(EXPAND_EVENTS, "event"),
    KEYVALUE_داخلا(EXPAND_اظهار, "expression"),
    KEYVALUE_داخلا(EXPAND_FILES, "file"),
    KEYVALUE_داخلا(EXPAND_FILES_اندريون_رستو, "file_in_path"),
    KEYVALUE_داخلا(EXPAND_FILETYPE, "filetype"),
    KEYVALUE_داخلا(EXPAND_FILETYPECMD, "filetypecmd"),
    KEYVALUE_داخلا(EXPAND_FUNCTIONS, "function"),
    KEYVALUE_داخلا(EXPAND_HELP, "help"),
    KEYVALUE_داخلا(EXPAND_HIGHLIGHT, "highlight"),
    KEYVALUE_داخلا(EXPAND_HISTORY, "history"),
#إذا defined(FEAT_KEYMAP)
    KEYVALUE_داخلا(EXPAND_KEYMAP, "keymap"),
#جيڪڏهن_پورو
#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    KEYVALUE_داخلا(EXPAND_LOCALES, "locale"),
#جيڪڏهن_پورو
    KEYVALUE_داخلا(EXPAND_MAPCLEAR, "mapclear"),
    KEYVALUE_داخلا(EXPAND_MAPPINGS, "mapping"),
    KEYVALUE_داخلا(EXPAND_MENUS, "menu"),
    KEYVALUE_داخلا(EXPAND_MESSAGES, "messages"),
    KEYVALUE_داخلا(EXPAND_SETTINGS, "option"),
    KEYVALUE_داخلا(EXPAND_PACKADD, "packadd"),
    KEYVALUE_داخلا(EXPAND_RETAB, "retab"),
    KEYVALUE_داخلا(EXPAND_RUNTIME, "runtime"),
#إذا defined(FEAT_EVAL)
    KEYVALUE_داخلا(EXPAND_SCRIPTNAMES, "scriptnames"),
#جيڪڏهن_پورو
    KEYVALUE_داخلا(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_داخلا(EXPAND_SHELLCMDLINE, "shellcmdline"),
#إذا defined(FEAT_SIGNS)
    KEYVALUE_داخلا(EXPAND_SIGN, "sign"),
#جيڪڏهن_پورو
    KEYVALUE_داخلا(EXPAND_OWNSYNTAX, "syntax"),
#إذا defined(FEAT_PROFILE)
    KEYVALUE_داخلا(EXPAND_SYNTIME, "syntime"),
#جيڪڏهن_پورو
    KEYVALUE_داخلا(EXPAND_ٽيگ_فهرست, "tag"),
    KEYVALUE_داخلا(EXPAND_ٽيگ_فهرست_LISTFILES, "tag_listfiles"),
    KEYVALUE_داخلا(EXPAND_USER, "user"),
    KEYVALUE_داخلا(EXPAND_USER_VARS, "var")
};

تعريفنوع بنية
{
    cmd_addr_T ڪنجي;
    حرف *fullname;
    size_t fullnamelen;
    حرف *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#وصف_ڪريو ADDRTYPE_داخلا(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
ساكن addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_داخلا(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_داخلا(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_داخلا(ADDR_سٽون, "lines", "line"),
    ADDRTYPE_داخلا(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_داخلا(ADDR_OTHER, "other", "?"),
    ADDRTYPE_داخلا(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_داخلا(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_داخلا(ADDR_WINDOWS, "windows", "win")
};

ساكن صحيح cmp_addr_type(ثابت فراغ *a, ثابت فراغ *b);

/*
 * Search for a user command that matches "eap->cmd".
 * Return cmdidx in "eap->cmdidx", flags in "eap->argt", idx in "eap->useridx".
 * Return a pointer to just after the command.
 * Return NULL if there is no matching command.
 */
    char_u *
ڳوليو_ucmd(
    exarg_T	*eap,
    char_u	*p,	 // end of the command (possibly including count)
    صحيح		*ڀريل,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    صحيح		*complp) // completion flags or NULL
{
    صحيح		len = (صحيح)(p - eap->cmd);
    صحيح		j, k, matchlen = 0;
    ucmd_T	*uc;
    صحيح		مليو = FALSE;
    صحيح		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    صحيح		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_بفر->b_ucmds;
    لكل (;;)
    {
	لكل (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    طالما (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    إذا (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		إذا (k == len && مليو && *np != NUL)
		{
		    إذا (gap == &ucmds)
			أرجع عدم;
		    amb_local = TRUE;
		}

		إذا (!مليو || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    إذا (k == len)
			مليو = TRUE;
		    وإلا
			possible = TRUE;

		    إذا (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    وإلا
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (ڊگهو)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    إذا (complp != عدم)
			*complp = uc->uc_compl;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		    إذا (xp != عدم)
		    {
			xp->xp_arg = uc->uc_compl_arg;
			xp->xp_complete_opt = uc->uc_compl_opt;
			xp->xp_script_ctx = uc->uc_script_ctx;
			xp->xp_script_ctx.sc_lnum += SOURCING_LNUM;
		    }
#جيڪڏهن_پورو
		    // Do not search for further abbreviations
		    // if this is an exact match.
		    matchlen = k;
		    إذا (k == len && *np == NUL)
		    {
			إذا (ڀريل != عدم)
			    *ڀريل = TRUE;
			amb_local = FALSE;
			اكسر;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	إذا (j < gap->ga_len || gap == &ucmds)
	    اكسر;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    إذا (amb_local)
    {
	إذا (xp != عدم)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	أرجع عدم;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    إذا (مليو || possible)
	أرجع p + (matchlen - len);
    أرجع p;
}

/*
 * Set completion context for :command
 */
    char_u *
سيٽ_context_اندريون_user_cmd(expand_T *xp, char_u *arg_اندريون)
{
    char_u	*arg = arg_اندريون;
    char_u	*p;

    // Check for attributes
    طالما (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	إذا (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    إذا (p == عدم)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_نمونو = arg;
		أرجع عدم;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    إذا (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_نمونو = p + 1;
	    }
	    وإلا إذا (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_نمونو = p + 1;
		لكل (char_u *c = p + 1; *c != NUL; ++c)
		    إذا (*c == ',')
			xp->xp_نمونو = c + 1;
	    }
	    وإلا إذا (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_نمونو = p + 1;
	    }
	    وإلا إذا (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_نمونو = p + 1;
	    }
	    أرجع عدم;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    إذا (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_نمونو = arg;
	أرجع عدم;
    }

    // And finally comes a normal command
    أرجع skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
سيٽ_context_اندريون_user_cmdarg(
	char_u		*cmd اڻ_استعمال_ٿيل,
	char_u		*arg,
	ڊگهو		argt,
	صحيح		context,
	expand_T	*xp,
	صحيح		forceit)
{
    char_u	*p;

    إذا (context == EXPAND_NOTHING)
	أرجع عدم;

    إذا (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	أرجع عدم;
    }

#جيڪڏهن_وصف_ٿيل FEAT_MENU
    إذا (context == EXPAND_MENUS)
	أرجع سيٽ_context_اندريون_menu_cmd(xp, cmd, arg, forceit);
#جيڪڏهن_پورو
    إذا (context == EXPAND_COMMANDS)
	أرجع arg;
    إذا (context == EXPAND_MAPPINGS)
	أرجع سيٽ_context_اندريون_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    إذا (!(argt & EX_ARGSPACE))
    {
	p = arg;
	طالما (*p)
	{
	    إذا (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    وإلا إذا (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_نمونو = arg;
    xp->xp_context = context;

    أرجع عدم;
}

    char_u *
expand_user_command_name(صحيح idx)
{
    أرجع get_user_commands(عدم, idx - (صحيح)CMD_سائيز);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_بفر;

    إذا (idx < buf->b_ucmds.ga_len)
	أرجع USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    إذا (idx < ucmds.ga_len)
    {
	صحيح	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	لكل (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    إذا (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		أرجع (char_u *)"";
	أرجع name;
    }
    أرجع عدم;
}

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(صحيح idx, صحيح cmdidx)
{
    إذا (cmdidx == CMD_USER && idx < ucmds.ga_len)
	أرجع USER_CMD(idx)->uc_name;
    إذا (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_بفر;

	إذا (idx < buf->b_ucmds.ga_len)
	    أرجع USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    أرجع عدم;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    صحيح
user_command_info(صحيح idx, صحيح cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_بفر->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    إذا (idx >= gap->ga_len)
	أرجع FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_جوڙيو_string(d, "name", uc->uc_name);
    dict_جوڙيو_string(d, "kind", (char_u *)"user");
    dict_جوڙيو_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_جوڙيو_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_منزل_str(uc->uc_compl, uc->uc_compl_arg);
    dict_جوڙيو_string(d, "complete", compl == عدم ? (char_u *)"" : compl);
    vim_free(compl);
    dict_جوڙيو_string(d, "definition", uc->uc_rep);
    dict_جوڙيو_عدد(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_جوڙيو_عدد(d, "lnum", uc->uc_script_ctx.sc_lnum);
    أرجع OK;
}
#جيڪڏهن_پورو

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    إذا (idx < 0 || idx >= (صحيح)ARRAY_ڊيگهائي(addr_type_complete_tab))
	أرجع عدم;
    أرجع (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    ساكن حرف *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    إذا (idx < 0 || idx >= (صحيح)ARRAY_ڊيگهائي(user_cmd_flags))
	أرجع عدم;
    أرجع (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    ساكن حرف *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    إذا (idx < 0 || idx >= (صحيح)ARRAY_ڊيگهائي(user_cmd_nargs))
	أرجع عدم;
    أرجع (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    إذا (idx < 0 || idx >= (صحيح)ARRAY_ڊيگهائي(command_complete_tab))
	أرجع عدم;
    أرجع command_complete_tab[idx].قدر.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
ساكن حرف *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    إذا (idx < 0 || idx >= (صحيح)ARRAY_ڊيگهائي(user_cmd_completeopt_tab))
	أرجع عدم;
    أرجع (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    ساكن keyvalue_T *
get_commandtype(صحيح expand)
{
    صحيح i;

    لكل (i = 0; i < (صحيح)ARRAY_ڊيگهائي(command_complete_tab); ++i)
	إذا (command_complete_tab[i].ڪنجي == expand)
	    أرجع &command_complete_tab[i];

    أرجع عدم;
}

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_منزل_str(صحيح expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    إذا (kv == عدم || kv->قدر.string == عدم)
	أرجع عدم;

    cmd_compl = kv->قدر.string;
    إذا (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*بفر;

	بفر = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	إذا (بفر == عدم)
	    أرجع عدم;
	sprintf((حرف *)بفر, "%s,%s", cmd_compl, compl_arg);
	أرجع بفر;
    }

    أرجع vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    صحيح
cmdcomplete_str_منزل_type(char_u *complete_str)
{
    keyvalue_T هدف;
    keyvalue_T *داخلا;
    ساكن keyvalue_T *آخري_داخلا = عدم;	// cached result

    إذا (STRNCMP(complete_str, "custom,", 7) == 0)
	أرجع EXPAND_USER_DEFINED;
    إذا (STRNCMP(complete_str, "customlist,", 11) == 0)
	أرجع EXPAND_USER_LIST;

    هدف.ڪنجي = 0;
    هدف.قدر.string = complete_str;
    هدف.قدر.ڊيگهائي = 0;			// not used, see cmp_keyvalue_value()

    إذا (آخري_داخلا != عدم && cmp_keyvalue_قدر(&هدف, آخري_داخلا) == 0)
	داخلا = آخري_داخلا;
    وإلا
    {
	داخلا = (keyvalue_T *)bsearch(&هدف,
	    &command_complete_tab,
	    ARRAY_ڊيگهائي(command_complete_tab),
	    حجم(command_complete_tab[0]),
	    cmp_keyvalue_قدر);
	إذا (داخلا == عدم)
	    أرجع EXPAND_NOTHING;

	آخري_داخلا = داخلا;
    }

    أرجع داخلا->ڪنجي;
}
#جيڪڏهن_پورو

/*
 * List user commands starting with "name[name_len]".
 */
    ساكن فراغ
uc_list(char_u *name, size_t name_len)
{
    صحيح		i, j;
    صحيح		مليو = FALSE;
    ucmd_T	*cmd;
    صحيح		len;
    صحيح		over;
    ڊگهو	a;
    garray_T	*gap;
    keyvalue_T	*داخلا;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_بفر->b_ucmds;
    لكل (;;)
    {
	لكل (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (ڊگهو)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    إذا (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || پيغام_filtered(cmd->uc_name))
		استمر;

	    // Put out the title first time
	    إذا (!مليو)
		نياپو_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    مليو = TRUE;
	    نياپو_putchar('\n');
	    إذا (got_int)
		اكسر;

	    // Special cases
	    len = 4;
	    إذا (a & EX_BANG)
	    {
		نياپو_putchar('!');
		--len;
	    }
	    إذا (a & EX_REGSTR)
	    {
		نياپو_putchar('"');
		--len;
	    }
	    إذا (gap != &ucmds)
	    {
		نياپو_putchar('b');
		--len;
	    }
	    إذا (a & EX_TRLBAR)
	    {
		نياپو_putchar('|');
		--len;
	    }
	    إذا (len != 0)
		نياپو_puts(&"    "[4 - len]);

	    نياپو_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (صحيح)cmd->uc_namelen + 4;

	    إذا (len < 21)
	    {
		// Field padding spaces   12345678901234567
		ساكن حرف spaces[18] = "                 ";
		نياپو_puts(&spaces[len - 4]);
		len = 21;
	    }
	    نياپو_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    بدل ((صحيح)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		حالة 0:				IObuff[len++] = '0'; اكسر;
		حالة (EX_EXTRA):		IObuff[len++] = '*'; اكسر;
		حالة (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; اكسر;
		حالة (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; اكسر;
		حالة (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; اكسر;
		حالة (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; اكسر;
	    }

	    افعل
	    {
		IObuff[len++] = ' ';
	    } طالما (len < 5 - over);

	    // Address / Range
	    إذا (a & (EX_RANGE|EX_ڳڻپ))
	    {
		إذا (a & EX_ڳڻپ)
		{
		    // -count=N
		    len += vim_snprintf((حرف *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		وإلا إذا (a & EX_DFLALL)
		    IObuff[len++] = '%';
		وإلا إذا (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((حرف *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		وإلا
		    IObuff[len++] = '.';
	    }

	    افعل
	    {
		IObuff[len++] = ' ';
	    } طالما (len < 8 - over);

	    // Address Type
	    لكل (j = 0; j < (صحيح)ARRAY_ڊيگهائي(addr_type_complete_tab); ++j)
		إذا (addr_type_complete_tab[j].ڪنجي != ADDR_سٽون
			&& addr_type_complete_tab[j].ڪنجي == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (صحيح)addr_type_complete_tab[j].shortnamelen;
		    اكسر;
		}

	    افعل
	    {
		IObuff[len++] = ' ';
	    } طالما (len < 13 - over);

	    // Completion
	    داخلا = get_commandtype(cmd->uc_compl);
	    إذا (داخلا != عدم)
	    {
		STRCPY(IObuff + len, داخلا->قدر.string);
		len += (صحيح)داخلا->قدر.ڊيگهائي;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		إذا (p_verbose > 0 && cmd->uc_compl_arg != عدم)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    إذا (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (صحيح)uc_compl_arglen;
		    }
		}
		إذا (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((حرف *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#جيڪڏهن_پورو
	    }

	    افعل
	    {
		IObuff[len++] = ' ';
	    } طالما (len < 25 - over);

	    IObuff[len] = NUL;
	    نياپو_outtrans(IObuff);

	    نياپو_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    إذا (p_verbose > 0)
		آخري_سيٽ_نياپو(cmd->uc_script_ctx);
#جيڪڏهن_پورو
	    ٻاهريون_flush();
	    ui_breakcheck();
	    إذا (got_int)
		اكسر;
	}
	إذا (gap == &ucmds || i < gap->ga_len)
	    اكسر;
	gap = &ucmds;
    }

    إذا (!مليو)
	نياپو(_("No user-defined commands found"));

    --ucmd_locked;
}

    حرف *
uc_fun_cmd(فراغ)
{
    ساكن char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    صحيح		i;

    لكل (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    أرجع (حرف *)IObuff;
}

/*
 * Parse address type argument
 */
    ساكن صحيح
parse_addr_type_arg(
    char_u	*قدر,
    صحيح		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T هدف;
    addrtype_T *داخلا;
    ساكن addrtype_T *آخري_داخلا;	// cached result

    هدف.ڪنجي = 0;
    هدف.fullname = (حرف *)قدر;
    هدف.fullnamelen = vallen;

    إذا (آخري_داخلا != عدم && cmp_addr_type(&هدف, آخري_داخلا) == 0)
	داخلا = آخري_داخلا;
    وإلا
    {
	داخلا = (addrtype_T *)bsearch(&هدف,
	    &addr_type_complete_tab,
	    ARRAY_ڊيگهائي(addr_type_complete_tab),
	    حجم(addr_type_complete_tab[0]),
	    cmp_addr_type);
	إذا (داخلا == عدم)
	{
	    صحيح i;
	    char_u	*err = قدر;

	    لكل (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_پتو_type_قدر_str), err);
	    أرجع FAIL;
	}

	آخري_داخلا = داخلا;
    }

    *addr_type_arg = داخلا->ڪنجي;

    أرجع OK;
}

    ساكن صحيح
cmp_addr_type(ثابت فراغ *a, ثابت فراغ *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    أرجع STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    صحيح
parse_compl_arg(
    char_u	*قدر,
    صحيح		vallen,
    صحيح		*complp,
    ڊگهو	*argt,
    char_u	**compl_arg اڻ_استعمال_ٿيل)
{
    char_u	*arg = عدم;
#إذا defined(FEAT_EVAL)
    size_t	arglen = 0;
#جيڪڏهن_پورو
    صحيح		i;
    صحيح		valend = vallen;
    keyvalue_T	هدف;
    keyvalue_T	*داخلا;
    ساكن keyvalue_T	*آخري_داخلا = عدم;	    // cached result

    // Look for any argument part - which is the part after any ','
    لكل (i = 0; i < vallen; ++i)
    {
	إذا (قدر[i] == ',')
	{
	    arg = &قدر[i + 1];
#إذا defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#جيڪڏهن_پورو
	    valend = i;
	    اكسر;
	}
    }

    هدف.ڪنجي = 0;
    هدف.قدر.string = قدر;
    هدف.قدر.ڊيگهائي = valend;

    إذا (آخري_داخلا != عدم && cmp_keyvalue_قدر_n(&هدف, آخري_داخلا) == 0)
	داخلا = آخري_داخلا;
    وإلا
    {
	داخلا = (keyvalue_T *)bsearch(&هدف,
	    &command_complete_tab,
	    ARRAY_ڊيگهائي(command_complete_tab),
	    حجم(command_complete_tab[0]),
	    cmp_keyvalue_قدر_n);
	إذا (داخلا == عدم)
	{
	    semsg(_(e_invalid_complete_قدر_str), قدر);
	    أرجع FAIL;
	}

	آخري_داخلا = داخلا;
    }

    *complp = داخلا->ڪنجي;
    إذا (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    وإلا إذا (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    إذا (
#إذا defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#جيڪڏهن_پورو
								arg != عدم)
    {
	emsg(_(e_completion_دليل_only_allowed_for_custom_completion));
	أرجع FAIL;
    }

#إذا defined(FEAT_EVAL)
    إذا ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == عدم)
    {
	emsg(_(e_custom_completion_requires_function_دليل));
	أرجع FAIL;
    }

    إذا (arg != عدم)
	*compl_arg = vim_strnsave(arg, arglen);
#جيڪڏهن_پورو

    أرجع OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    ساكن صحيح
parse_completeopt_arg(char_u *قدر, صحيح vallen, صحيح *compl_opt)
{
    char_u  *p = قدر;
    char_u  *پڇاڙي = قدر + vallen;
    صحيح	    flags = 0;

    إذا (vallen == 0)
    {
	semsg(_(e_دليل_required_for_str), "-completeopt");
	أرجع FAIL;
    }

    طالما (p < پڇاڙي)
    {
	char_u	*comma;
	size_t	itemlen;
	صحيح	matched = FALSE;
	صحيح	i;

	comma = vim_strchr(p, ',');
	إذا (comma == عدم || comma > پڇاڙي)
	    itemlen = (size_t)(پڇاڙي - p);
	وإلا
	    itemlen = (size_t)(comma - p);

	إذا (itemlen == 0)
	{
	    semsg(_(e_invalid_قدر_for_دليل_str), "completeopt");
	    أرجع FAIL;
	}

	لكل (i = 0; i < (صحيح)ARRAY_ڊيگهائي(user_cmd_completeopt_tab); ++i)
	{
	    حرف *name = user_cmd_completeopt_tab[i];

	    إذا (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		إذا (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		اكسر;
	    }
	}
	إذا (!matched)
	{
	    semsg(_(e_invalid_قدر_for_دليل_str), "completeopt");
	    أرجع FAIL;
	}

	p += itemlen;
	إذا (p < پڇاڙي && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    أرجع OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    ساكن صحيح
uc_جاچ_attr(
    char_u	*attr,
    size_t	len,
    ڊگهو	*argt,
    ڊگهو	*def,
    صحيح		*flags,
    صحيح		*complp,
    char_u	**compl_arg,
    صحيح		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    إذا (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	أرجع FAIL;
    }

    // First, try the simple attributes (no arguments)
    إذا (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    وإلا إذا (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_بفر;
    وإلا إذا (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    وإلا إذا (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    وإلا إذا (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    وإلا
    {
	صحيح	i;
	char_u	*val = عدم;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	لكل (i = 0; i < (صحيح)len; ++i)
	{
	    إذا (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		اكسر;
	    }
	}

	إذا (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    إذا (vallen == 1)
	    {
		إذا (*val == '0')
		    // Do nothing - this is the default
		    ;
		وإلا إذا (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		وإلا إذا (*val == '*')
		    *argt |= EX_EXTRA;
		وإلا إذا (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		وإلا إذا (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		وإلا إذا (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		وإلا
		    اذهب wrong_nargs;
	    }
	    وإلا
	    {
wrong_nargs:
		emsg(_(e_invalid_عدد_جو_arguments));
		أرجع FAIL;
	    }
	}
	وإلا إذا (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    إذا (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    وإلا إذا (val != عدم)
	    {
		p = val;
		إذا (*def >= 0)
		{
two_ڳڻپ:
		    emsg(_(e_ڳڻپ_cannot_be_specified_twice));
		    أرجع FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		إذا (p != val + vallen || vallen == 0)
		{
invalid_ڳڻپ:
		    emsg(_(e_invalid_default_قدر_for_ڳڻپ));
		    أرجع FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    إذا (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_سٽون;
	}
	وإلا إذا (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_ڳڻپ | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    إذا (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    إذا (val != عدم)
	    {
		p = val;
		إذا (*def >= 0)
		    اذهب two_ڳڻپ;

		*def = getdigits(&p);

		إذا (p != val + vallen)
		    اذهب invalid_ڳڻپ;
	    }

	    إذا (*def < 0)
		*def = 0;
	}
	وإلا إذا (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    إذا (val == عدم)
	    {
		semsg(_(e_دليل_required_for_str), "-complete");
		أرجع FAIL;
	    }

	    إذا (parse_compl_arg(val, (صحيح)vallen, complp, argt, compl_arg)
								      == FAIL)
		أرجع FAIL;
	}
	وإلا إذا (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    إذا (val == عدم)
	    {
		semsg(_(e_دليل_required_for_str), "-completeopt");
		أرجع FAIL;
	    }

	    إذا (parse_completeopt_arg(val, (صحيح)vallen, compl_opt) == FAIL)
		أرجع FAIL;
	}
	وإلا إذا (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    إذا (val == عدم)
	    {
		semsg(_(e_دليل_required_for_str), "-addr");
		أرجع FAIL;
	    }
	    إذا (parse_addr_type_arg(val, (صحيح)vallen, addr_type_arg) == FAIL)
		أرجع FAIL;
	    إذا (*addr_type_arg != ADDR_سٽون)
		*argt |= EX_ZEROR;
	}
	وإلا
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    ساكن صحيح
uc_جوڙيو_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    ڊگهو	argt,
    ڊگهو	def,
    صحيح		flags,
    صحيح		compl,
    char_u	*compl_arg اڻ_استعمال_ٿيل,
    صحيح		compl_opt,
    cmd_addr_T	addr_type,
    صحيح		force)
{
    ucmd_T	*cmd = عدم;
    char_u	*p;
    صحيح		i;
    صحيح		cmp = 1;
    char_u	*rep_buf = عدم;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, عدم);
    إذا (rep_buf == عدم)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	إذا (rep_buf == عدم)
	    أرجع FAIL;
    }

    // get address of growarray: global or in curbuf
    إذا (flags & UC_بفر)
    {
	gap = &curbuf->b_ucmds;
	إذا (gap->ga_itemsize == 0)
	    ga_init2(gap, حجم(ucmd_T), 4);
    }
    وإلا
	gap = &ucmds;

    // Search for the command in the already defined commands.
    لكل (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	إذا (cmp == 0)
	{
	    إذا (name_len < cmd->uc_namelen)
		cmp = -1;
	    وإلا إذا (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	إذا (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    إذا (!force
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#جيڪڏهن_پورو
		    )
	    {
		semsg(_(e_command_already_exists_جوڙيو_bang_منزل_replace_it_str),
									 name);
		اذهب fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#إذا defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#جيڪڏهن_پورو
	    اكسر;
	}

	// Stop as soon as we pass the name to add
	إذا (cmp < 0)
	    اكسر;
    }

    // Extend the array unless we're replacing an existing command
    إذا (cmp != 0)
    {
	إذا (ga_grow(gap, 1) == FAIL)
	    اذهب fail;
	إذا ((p = vim_strnsave(name, name_len)) == عدم)
	    اذهب fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * حجم(ucmd_T));

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
    إذا (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_نسخو = SCRIPT_نسخو_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#جيڪڏهن_پورو
    cmd->uc_addr_type = addr_type;

    أرجع OK;

fail:
    vim_free(rep_buf);
#إذا defined(FEAT_EVAL)
    vim_free(compl_arg);
#جيڪڏهن_پورو
    أرجع FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_بلاڪ(exarg_T *eap, char_u *p, char_u **tofree, صحيح *flags)
{
    char_u *retp = p;
    char_u *بلاڪ = ڳوليو_cmd_بلاڪ_شروعات(p);

    إذا (بلاڪ != عدم && eap->ea_getline != عدم)
    {
	garray_T    ga;
	char_u	    *سٽ = عدم;

	ga_init2(&ga, حجم(char_u *), 10);
	إذا (ga_copy_string(&ga, p) == FAIL)
	    أرجع retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	إذا (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    لكل (;;)
	    {
		vim_free(سٽ);
		إذا ((سٽ = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == عدم)
		{
		    emsg(_(e_missing_rcurly));
		    اكسر;
		}
		إذا (ga_copy_string(&ga, سٽ) == FAIL)
		    اكسر;
		إذا (*skipwhite(سٽ) == '}')
		    اكسر;
	    }
	vim_free(سٽ);
	retp = *tofree = ga_concat_لڙيون(&ga, "\n");
	إذا (retp == عدم)
	    retp = p;
	ga_clear_لڙيون(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	إذا (بلاڪ == p)
	    *flags |= UC_VIM9;
    }
    أرجع retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    فراغ
skip_cmd_بلاڪ(exarg_T *eap)
{
    char_u	*tofree = عدم;
    صحيح		flags = 0;

    (فراغ)may_get_cmd_بلاڪ(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    فراغ
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*پڇاڙي;
    char_u	*p;
    ڊگهو	argt = 0;
    ڊگهو	def = -1;
    صحيح		flags = 0;
    صحيح		compl = EXPAND_NOTHING;
    char_u	*compl_arg = عدم;
    صحيح		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    صحيح		has_attr = (eap->arg[0] == '-');
    صحيح		name_len;

    إذا (eap->skip)
    {
	skip_cmd_بلاڪ(eap);
	أرجع;
    }

    p = eap->arg;

    // Check for attributes
    طالما (*p == '-')
    {
	++p;
	پڇاڙي = skiptowhite(p);
	إذا (uc_جاچ_attr(p, پڇاڙي - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    اذهب theend;
	p = skipwhite(پڇاڙي);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    إذا (ASCII_ISALPHA(*p))
	طالما (ASCII_ISALNUM(*p))
	    ++p;
    إذا (!پڇاڙيون_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	اذهب theend;
    }
    پڇاڙي = p;
    name_len = (صحيح)(پڇاڙي - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(پڇاڙي);
    إذا (!has_attr && پڇاڙيون_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    وإلا إذا (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_شروعات_with_an_uppercase_letter));
    }
    وإلا إذا ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_استعمال_ٿيل_for_user_defined_command));
    }
    وإلا إذا (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	إذا (اندريون_vim9script())
	    emsg(_(e_complete_استعمال_ٿيل_without_allowing_arguments));
	وإلا
	    give_warning_with_ذريعو(
		       (char_u *)_(e_complete_استعمال_ٿيل_without_allowing_arguments),
								   TRUE, TRUE);
    }
    وإلا إذا ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_استعمال_ٿيل_with_nargs_underscore));
    }
    وإلا
    {
	char_u *tofree = عدم;

	p = may_get_cmd_بلاڪ(eap, p, &tofree, &flags);

	uc_جوڙيو_command(name, پڇاڙي - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	أرجع;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    فراغ
ex_comclear(exarg_T *eap اڻ_استعمال_ٿيل)
{
    uc_clear(&ucmds);
    إذا (curbuf != عدم)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ساكن صحيح
is_ucmd_locked(فراغ)
{
    إذا (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    فراغ
uc_clear(garray_T *gap)
{
    صحيح		i;
    ucmd_T	*cmd;

    إذا (is_ucmd_locked())
	أرجع;

    لكل (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#إذا defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#جيڪڏهن_پورو
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    فراغ
ex_delcommand(exarg_T *eap)
{
    صحيح		i = 0;
    ucmd_T	*cmd = عدم;
    صحيح		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    صحيح		بفر_only = FALSE;

    إذا (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	بفر_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    لكل (;;)
    {
	لكل (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    إذا (res <= 0)
		اكسر;
	}
	إذا (gap == &ucmds || res == 0 || بفر_only)
	    اكسر;
	gap = &ucmds;
    }

    إذا (res != 0)
    {
	semsg(_(بفر_only
		    ? e_no_such_user_defined_command_اندريون_current_بفر_str
		    : e_no_such_user_defined_command_str), arg);
	أرجع;
    }

    إذا (is_ucmd_locked())
	أرجع;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#إذا defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#جيڪڏهن_پورو

    --gap->ga_len;

    إذا (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * حجم(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    ساكن char_u *
uc_ورهايو_دليلون(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    صحيح len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    طالما (*p)
    {
	إذا (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	وإلا إذا (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	وإلا إذا (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	وإلا إذا (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    إذا (*p == NUL)
		اكسر;
	    len += 4; // ", "
	}
	وإلا
	{
	    صحيح charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    إذا (buf == عدم)
    {
	*lenp = 0;
	أرجع buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    طالما (*p)
    {
	إذا (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	وإلا إذا (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	وإلا إذا (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	وإلا إذا (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    إذا (*p == NUL)
		اكسر;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	وإلا
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    أرجع buf;
}

    ساكن size_t
جوڙيو_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    حرف	*mod_str,
    size_t	mod_strlen,
    صحيح		*multi_mods)
{
    إذا (buf != عدم)
    {
	إذا (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    إذا (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    وإلا
	*multi_mods = 1;

    أرجع mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
جوڙيو_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, صحيح *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    إذا (cmod->cmod_ورهايو & WSP_ABOVE)
	buflen += جوڙيو_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    إذا (cmod->cmod_ورهايو & WSP_هيٺ)
	buflen += جوڙيو_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    إذا (cmod->cmod_ورهايو & WSP_BOT)
	buflen += جوڙيو_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    إذا (cmod->cmod_tab > 0)
    {
	صحيح tabnr = cmod->cmod_tab - 1;

	إذا (tabnr == tabpage_اشاريو(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += جوڙيو_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	وإلا
	{
	    حرف tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, حجم(tab_buf), "%dtab", tabnr);
	    buflen += جوڙيو_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    إذا (cmod->cmod_ورهايو & WSP_چوٽي)
	buflen += جوڙيو_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    إذا (cmod->cmod_ورهايو & WSP_VERT)
	buflen += جوڙيو_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    إذا (cmod->cmod_ورهايو & WSP_HOR)
	buflen += جوڙيو_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    أرجع buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, صحيح quote)
{
    size_t  buflen = 0;
    صحيح	    multi_mods = 0;
    صحيح	    i;
    ساكن keyvalue_T mod_داخلا_tab[] =
    {
#جيڪڏهن_وصف_ٿيل FEAT_BROWSE_CMD
	KEYVALUE_داخلا(CMOD_BROWSE, "browse"),
#جيڪڏهن_پورو
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	KEYVALUE_داخلا(CMOD_CONFIRM, "confirm"),
#جيڪڏهن_پورو
	KEYVALUE_داخلا(CMOD_HIDE, "hide"),
	KEYVALUE_داخلا(CMOD_KEEPALT, "keepalt"),
	KEYVALUE_داخلا(CMOD_KEEPJUMPS, "keepjumps"),
	KEYVALUE_داخلا(CMOD_KEEPMARKS, "keepmarks"),
	KEYVALUE_داخلا(CMOD_KEEPPATTERNS, "keeppatterns"),
	KEYVALUE_داخلا(CMOD_LOCKMARKS, "lockmarks"),
	KEYVALUE_داخلا(CMOD_NOSWAPFILE, "noswapfile"),
	KEYVALUE_داخلا(CMOD_UNSILENT, "unsilent"),
	KEYVALUE_داخلا(CMOD_NOAUTOCMD, "noautocmd"),
#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
	KEYVALUE_داخلا(CMOD_SANDBOX, "sandbox"),
#جيڪڏهن_پورو
	KEYVALUE_داخلا(CMOD_LEGACY, "legacy")
    };

    إذا (quote)
    {
	++buflen;
	إذا (buf != عدم)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    وإلا
    إذا (buf != عدم)
	*buf = NUL;

    // the modifiers that are simple flags
    لكل (i = 0; i < (صحيح)ARRAY_ڊيگهائي(mod_داخلا_tab); ++i)
	إذا (cmod->cmod_flags & mod_داخلا_tab[i].ڪنجي)
	    buflen += جوڙيو_cmd_modifier(buf, buflen,
		    (حرف *)mod_داخلا_tab[i].قدر.string,
		    mod_داخلا_tab[i].قدر.ڊيگهائي, &multi_mods);

    // :silent
    إذا (cmod->cmod_flags & CMOD_SILENT)
    {
	إذا (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += جوڙيو_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	وإلا
	    buflen += جوڙيو_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    إذا (cmod->cmod_verbose > 0)
    {
	صحيح verbose_قدر = cmod->cmod_verbose - 1;

	إذا (verbose_قدر == 1)
	    buflen += جوڙيو_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	وإلا
	{
	    حرف verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, حجم(verbose_buf), "%dverbose", verbose_قدر);
	    buflen += جوڙيو_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += جوڙيو_win_cmd_modifiers((buf == عدم) ? عدم : buf + buflen, cmod, &multi_mods);

    إذا (quote)
    {
	إذا (buf == عدم)
	    ++buflen;
	وإلا
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    أرجع buflen;
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
    ساكن size_t
uc_check_ڪوڊ(
    char_u	*ڪوڊ,
    size_t	len,
    char_u	*buf,
    ucmd_T	*cmd,		// the user command we're expanding
    exarg_T	*eap,		// ex arguments
    char_u	**ورهايو_buf,
    size_t	*ورهايو_len)
{
    size_t	نتيجو = 0;
    char_u	*p = ڪوڊ + 1;
    size_t	l = len - 2;
    صحيح		quote = 0;
    تعداد {
	ct_دليلون,
	ct_BANG,
	ct_ڳڻپ,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_رجسٽر,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    إذا ((vim_strchr((char_u *)"qQfF", *p) != عدم) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    إذا (l <= 1)
	type = ct_NONE;
    وإلا إذا (STRNICMP(p, "args>", l) == 0)
	type = ct_دليلون;
    وإلا إذا (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    وإلا إذا (STRNICMP(p, "count>", l) == 0)
	type = ct_ڳڻپ;
    وإلا إذا (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    وإلا إذا (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    وإلا إذا (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    وإلا إذا (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    وإلا إذا (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_رجسٽر;
    وإلا إذا (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    بدل (type)
    {
    حالة ct_دليلون:
	// Simple case first
	إذا (*eap->arg == NUL)
	{
	    إذا (quote == 1)
	    {
		نتيجو = 2;
		إذا (buf != عدم)
		    STRCPY(buf, "''");
	    }
	    وإلا
		نتيجو = 0;
	    اكسر;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	إذا ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	بدل (quote)
	{
	حالة 0: // No quoting, no splitting
	    نتيجو = STRLEN(eap->arg);
	    إذا (buf != عدم)
		STRCPY(buf, eap->arg);
	    اكسر;
	حالة 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    صحيح	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    نتيجو = STRLEN(eap->arg) + 2;
	    لكل (p = eap->arg; *p; ++p)
	    {
		إذا (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		وإلا إذا (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    إذا (VIM_ISWHITE(p[1]))
			--نتيجو;
		    ++p;
		}
		وإلا
		     إذا (*p == '\\' || *p == '"')
		    ++نتيجو;
	    }

	    إذا (buf != عدم)
	    {
		*buf++ = '"';
		لكل (p = eap->arg; *p; ++p)
		{
		    إذا (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    وإلا إذا (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			إذا (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    وإلا
			 إذا (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    اكسر;
	}
	حالة 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    إذا (*ورهايو_buf == عدم)
		*ورهايو_buf = uc_ورهايو_دليلون(eap->arg, ورهايو_len);

	    نتيجو = *ورهايو_len;
	    إذا (buf != عدم && نتيجو != 0)
		STRCPY(buf, *ورهايو_buf);

	    اكسر;
	}
	اكسر;

    حالة ct_BANG:
	نتيجو = eap->forceit ? 1 : 0;
	إذا (quote)
	    نتيجو += 2;
	إذا (buf != عدم)
	{
	    إذا (quote)
		*buf++ = '"';
	    إذا (eap->forceit)
		*buf++ = '!';
	    إذا (quote)
		*buf = '"';
	}
	اكسر;

    حالة ct_LINE1:
    حالة ct_LINE2:
    حالة ct_RANGE:
    حالة ct_ڳڻپ:
    {
	حرف انگ_buf[NUMBUFLEN];
	ڊگهو انگ = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_ڳڻپ :
		   (eap->addr_ڳڻپ > 0) ? eap->line2 : cmd->uc_def;
	size_t انگ_len;

	انگ_len = vim_snprintf(انگ_buf, حجم(انگ_buf), "%ld", انگ);
	نتيجو = انگ_len;

	إذا (quote)
	    نتيجو += 2;

	إذا (buf != عدم)
	{
	    إذا (quote)
		*buf++ = '"';
	    STRCPY(buf, انگ_buf);
	    buf += انگ_len;
	    إذا (quote)
		*buf = '"';
	}

	اكسر;
    }

    حالة ct_MODS:
    {
	نتيجو = produce_cmdmods(buf, &cmdmod, quote);
	اكسر;
    }

    حالة ct_رجسٽر:
	نتيجو = eap->regname ? 1 : 0;
	إذا (quote)
	    نتيجو += 2;
	إذا (buf != عدم)
	{
	    إذا (quote)
		*buf++ = '\'';
	    إذا (eap->regname)
		*buf++ = eap->regname;
	    إذا (quote)
		*buf = '\'';
	}
	اكسر;

    حالة ct_LT:
	نتيجو = 1;
	إذا (buf != عدم)
	    *buf = '<';
	اكسر;

    افتراضي:
	// Not recognized: just copy the '<' and return -1.
	نتيجو = (size_t)-1;
	إذا (buf != عدم)
	    *buf = '<';
	اكسر;
    }

    أرجع نتيجو;
}

/*
 * Execute a user defined command.
 */
    فراغ
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*شروعات;
    char_u	*پڇاڙي = عدم;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	ورهايو_len = 0;
    char_u	*ورهايو_buf = عدم;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    صحيح		restore_current_sctx = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    صحيح		restore_script_نسخو = 0;
#جيڪڏهن_پورو

    إذا (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    وإلا
	cmd = USER_CMD_GA(&prevwin_curwin()->w_بفر->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = عدم;
    لكل (;;)
    {
	p = cmd->uc_rep;    //ذريعو
	q = buf;	    // destination
	totlen = 0;

	لكل (;;)
	{
	    شروعات = vim_strchr(p, '<');
	    إذا (شروعات != عدم)
		پڇاڙي = vim_strchr(شروعات + 1, '>');
	    إذا (buf != عدم)
	    {
		لكل (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		إذا (*ksp == K_SPECIAL
			&& (شروعات == عدم || ksp < شروعات || پڇاڙي == عدم)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#جيڪڏهن_وصف_ٿيل FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (صحيح)KE_CSI)
#جيڪڏهن_پورو
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    إذا (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    استمر;
		}
	    }

	    // break if no <item> is found
	    إذا (شروعات == عدم || پڇاڙي == عدم)
		اكسر;

	    // Include the '>'
	    ++پڇاڙي;

	    // Take everything up to the '<'
	    len = شروعات - p;
	    إذا (buf == عدم)
		totlen += len;
	    وإلا
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_ڪوڊ(شروعات, پڇاڙي - شروعات, q, cmd, eap,
			     &ورهايو_buf, &ورهايو_len);
	    إذا (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = شروعات + 1;
		len = 1;
	    }
	    وإلا
		p = پڇاڙي;
	    إذا (buf == عدم)
		totlen += len;
	    وإلا
		q += len;
	}
	إذا (buf != عدم)	    // second time here, finished
	{
	    STRCPY(q, p);
	    اكسر;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	إذا (buf == عدم)
	{
	    vim_free(ورهايو_buf);
	    أرجع;
	}
    }

    إذا ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_نسخو = cmd->uc_script_ctx.sc_نسخو;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	إذا (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_نسخو =
				  SCRIPT_شي(current_sctx.sc_sid)->sn_نسخو;
	    SCRIPT_شي(current_sctx.sc_sid)->sn_نسخو = SCRIPT_نسخو_VIM9;
	}
#جيڪڏهن_پورو
    }

    (فراغ)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    إذا (restore_current_sctx)
    {
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (restore_script_نسخو != 0)
	    SCRIPT_شي(current_sctx.sc_sid)->sn_نسخو =
							restore_script_نسخو;
#جيڪڏهن_پورو
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(ورهايو_buf);
}
