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

قسمتعریف ساخت ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    طویل	uc_def;		// The default value for a range/count
    صحیح		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    صحیح		uc_flags;	// some UC_ flags
    صحیح		uc_compl_opt;	// completion options (UCC_ flags)
#اگر_تعریف_شدہ FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#اگر_ختم
} ucmd_T;

// List of all user commands.
ساکن garray_T ucmds = {0, 0, حجم(ucmd_T), 4, تہی};

// When non-zero it is not allowed to add or remove user commands
ساکن صحیح ucmd_locked = 0;

#تعریف_کرو USER_CMD(i) (&((ucmd_T *)(ucmds.ga_ڈیٹا))[i])
#تعریف_کرو USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_ڈیٹا))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
ساکن keyvalue_T command_complete_tab[] =
{
    KEYVALUE_اندراج(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_اندراج(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_اندراج(EXPAND_BEHAVE, "behave"),
#اگر defined(FEAT_EVAL)
    KEYVALUE_اندراج(EXPAND_BREAKPOINT, "breakpoint"),
#اگر_ختم
    KEYVALUE_اندراج(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_اندراج(EXPAND_COLORS, "color"),
    KEYVALUE_اندراج(EXPAND_COMMANDS, "command"),
    KEYVALUE_اندراج(EXPAND_COMPILER, "compiler"),
#اگر defined(FEAT_CSCOPE)
    KEYVALUE_اندراج(EXPAND_CSCOPE, "cscope"),
#اگر_ختم
#اگر defined(FEAT_EVAL)
    KEYVALUE_اندراج(EXPAND_USER_DEFINED, "custom"),
    KEYVALUE_اندراج(EXPAND_USER_LIST, "customlist"),
#اگر_ختم
    KEYVALUE_اندراج(EXPAND_DIFF_BUFFERS, "diff_buffer"),
    KEYVALUE_اندراج(EXPAND_DIRECTORIES, "dir"),
    KEYVALUE_اندراج(EXPAND_DIRS_اندر_CDPATH, "dir_in_path"),
    KEYVALUE_اندراج(EXPAND_ENV_VARS, "environment"),
    KEYVALUE_اندراج(EXPAND_EVENTS, "event"),
    KEYVALUE_اندراج(EXPAND_عبارت, "expression"),
    KEYVALUE_اندراج(EXPAND_FILES, "file"),
    KEYVALUE_اندراج(EXPAND_FILES_اندر_راستہ, "file_in_path"),
    KEYVALUE_اندراج(EXPAND_FILETYPE, "filetype"),
    KEYVALUE_اندراج(EXPAND_FILETYPECMD, "filetypecmd"),
    KEYVALUE_اندراج(EXPAND_FUNCTIONS, "function"),
    KEYVALUE_اندراج(EXPAND_HELP, "help"),
    KEYVALUE_اندراج(EXPAND_HIGHLIGHT, "highlight"),
    KEYVALUE_اندراج(EXPAND_HISTORY, "history"),
#اگر defined(FEAT_KEYMAP)
    KEYVALUE_اندراج(EXPAND_KEYMAP, "keymap"),
#اگر_ختم
#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    KEYVALUE_اندراج(EXPAND_LOCALES, "locale"),
#اگر_ختم
    KEYVALUE_اندراج(EXPAND_MAPCLEAR, "mapclear"),
    KEYVALUE_اندراج(EXPAND_MAPPINGS, "mapping"),
    KEYVALUE_اندراج(EXPAND_MENUS, "menu"),
    KEYVALUE_اندراج(EXPAND_MESSAGES, "messages"),
    KEYVALUE_اندراج(EXPAND_SETTINGS, "option"),
    KEYVALUE_اندراج(EXPAND_PACKADD, "packadd"),
    KEYVALUE_اندراج(EXPAND_RETAB, "retab"),
    KEYVALUE_اندراج(EXPAND_RUNTIME, "runtime"),
#اگر defined(FEAT_EVAL)
    KEYVALUE_اندراج(EXPAND_SCRIPTNAMES, "scriptnames"),
#اگر_ختم
    KEYVALUE_اندراج(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_اندراج(EXPAND_SHELLCMDLINE, "shellcmdline"),
#اگر defined(FEAT_SIGNS)
    KEYVALUE_اندراج(EXPAND_SIGN, "sign"),
#اگر_ختم
    KEYVALUE_اندراج(EXPAND_OWNSYNTAX, "syntax"),
#اگر defined(FEAT_PROFILE)
    KEYVALUE_اندراج(EXPAND_SYNTIME, "syntime"),
#اگر_ختم
    KEYVALUE_اندراج(EXPAND_لیبل_فہرست, "tag"),
    KEYVALUE_اندراج(EXPAND_لیبل_فہرست_LISTFILES, "tag_listfiles"),
    KEYVALUE_اندراج(EXPAND_USER, "user"),
    KEYVALUE_اندراج(EXPAND_USER_VARS, "var")
};

قسمتعریف ساخت
{
    cmd_addr_T کلید;
    حرف *fullname;
    size_t fullnamelen;
    حرف *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#تعریف_کرو ADDRTYPE_اندراج(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
ساکن addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_اندراج(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_اندراج(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_اندراج(ADDR_سطریں, "lines", "line"),
    ADDRTYPE_اندراج(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_اندراج(ADDR_OTHER, "other", "?"),
    ADDRTYPE_اندراج(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_اندراج(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_اندراج(ADDR_WINDOWS, "windows", "win")
};

ساکن صحیح cmp_addr_type(ثابت خالی *a, ثابت خالی *b);

/*
 * Search for a user command that matches "eap->cmd".
 * Return cmdidx in "eap->cmdidx", flags in "eap->argt", idx in "eap->useridx".
 * Return a pointer to just after the command.
 * Return NULL if there is no matching command.
 */
    char_u *
ڈھونڈو_ucmd(
    exarg_T	*eap,
    char_u	*p,	 // end of the command (possibly including count)
    صحیح		*بھرا,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    صحیح		*complp) // completion flags or NULL
{
    صحیح		len = (صحیح)(p - eap->cmd);
    صحیح		j, k, matchlen = 0;
    ucmd_T	*uc;
    صحیح		ملا = FALSE;
    صحیح		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    صحیح		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_بفر->b_ucmds;
    برائے (;;)
    {
	برائے (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    جب (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    اگر (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		اگر (k == len && ملا && *np != NUL)
		{
		    اگر (gap == &ucmds)
			واپس تہی;
		    amb_local = TRUE;
		}

		اگر (!ملا || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    اگر (k == len)
			ملا = TRUE;
		    ورنہ
			possible = TRUE;

		    اگر (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    ورنہ
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (طویل)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    اگر (complp != تہی)
			*complp = uc->uc_compl;
#اگر_تعریف_شدہ FEAT_EVAL
		    اگر (xp != تہی)
		    {
			xp->xp_arg = uc->uc_compl_arg;
			xp->xp_complete_opt = uc->uc_compl_opt;
			xp->xp_script_ctx = uc->uc_script_ctx;
			xp->xp_script_ctx.sc_lnum += SOURCING_LNUM;
		    }
#اگر_ختم
		    // Do not search for further abbreviations
		    // if this is an exact match.
		    matchlen = k;
		    اگر (k == len && *np == NUL)
		    {
			اگر (بھرا != تہی)
			    *بھرا = TRUE;
			amb_local = FALSE;
			توڑو;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	اگر (j < gap->ga_len || gap == &ucmds)
	    توڑو;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    اگر (amb_local)
    {
	اگر (xp != تہی)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	واپس تہی;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    اگر (ملا || possible)
	واپس p + (matchlen - len);
    واپس p;
}

/*
 * Set completion context for :command
 */
    char_u *
سیٹ_context_اندر_user_cmd(expand_T *xp, char_u *arg_اندر)
{
    char_u	*arg = arg_اندر;
    char_u	*p;

    // Check for attributes
    جب (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	اگر (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    اگر (p == تہی)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_نمونہ = arg;
		واپس تہی;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    اگر (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_نمونہ = p + 1;
	    }
	    ورنہ اگر (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_نمونہ = p + 1;
		برائے (char_u *c = p + 1; *c != NUL; ++c)
		    اگر (*c == ',')
			xp->xp_نمونہ = c + 1;
	    }
	    ورنہ اگر (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_نمونہ = p + 1;
	    }
	    ورنہ اگر (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_نمونہ = p + 1;
	    }
	    واپس تہی;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    اگر (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_نمونہ = arg;
	واپس تہی;
    }

    // And finally comes a normal command
    واپس skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
سیٹ_context_اندر_user_cmdarg(
	char_u		*cmd غیر_مستعمل,
	char_u		*arg,
	طویل		argt,
	صحیح		context,
	expand_T	*xp,
	صحیح		forceit)
{
    char_u	*p;

    اگر (context == EXPAND_NOTHING)
	واپس تہی;

    اگر (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	واپس تہی;
    }

#اگر_تعریف_شدہ FEAT_MENU
    اگر (context == EXPAND_MENUS)
	واپس سیٹ_context_اندر_menu_cmd(xp, cmd, arg, forceit);
#اگر_ختم
    اگر (context == EXPAND_COMMANDS)
	واپس arg;
    اگر (context == EXPAND_MAPPINGS)
	واپس سیٹ_context_اندر_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    اگر (!(argt & EX_ARGSPACE))
    {
	p = arg;
	جب (*p)
	{
	    اگر (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    ورنہ اگر (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_نمونہ = arg;
    xp->xp_context = context;

    واپس تہی;
}

    char_u *
expand_user_command_name(صحیح idx)
{
    واپس get_user_commands(تہی, idx - (صحیح)CMD_سائز);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp غیر_مستعمل, صحیح idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_بفر;

    اگر (idx < buf->b_ucmds.ga_len)
	واپس USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    اگر (idx < ucmds.ga_len)
    {
	صحیح	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	برائے (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    اگر (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		واپس (char_u *)"";
	واپس name;
    }
    واپس تہی;
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(صحیح idx, صحیح cmdidx)
{
    اگر (cmdidx == CMD_USER && idx < ucmds.ga_len)
	واپس USER_CMD(idx)->uc_name;
    اگر (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_بفر;

	اگر (idx < buf->b_ucmds.ga_len)
	    واپس USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    واپس تہی;
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
				? &prevwin_curwin()->w_بفر->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    اگر (idx >= gap->ga_len)
	واپس FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_جوڑو_string(d, "name", uc->uc_name);
    dict_جوڑو_string(d, "kind", (char_u *)"user");
    dict_جوڑو_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_جوڑو_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_مقصد_str(uc->uc_compl, uc->uc_compl_arg);
    dict_جوڑو_string(d, "complete", compl == تہی ? (char_u *)"" : compl);
    vim_free(compl);
    dict_جوڑو_string(d, "definition", uc->uc_rep);
    dict_جوڑو_نمبر(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_جوڑو_نمبر(d, "lnum", uc->uc_script_ctx.sc_lnum);
    واپس OK;
}
#اگر_ختم

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx < 0 || idx >= (صحیح)ARRAY_لمبائی(addr_type_complete_tab))
	واپس تہی;
    واپس (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp غیر_مستعمل, صحیح idx)
{
    ساکن حرف *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    اگر (idx < 0 || idx >= (صحیح)ARRAY_لمبائی(user_cmd_flags))
	واپس تہی;
    واپس (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp غیر_مستعمل, صحیح idx)
{
    ساکن حرف *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    اگر (idx < 0 || idx >= (صحیح)ARRAY_لمبائی(user_cmd_nargs))
	واپس تہی;
    واپس (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx < 0 || idx >= (صحیح)ARRAY_لمبائی(command_complete_tab))
	واپس تہی;
    واپس command_complete_tab[idx].قدر.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
ساکن حرف *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx < 0 || idx >= (صحیح)ARRAY_لمبائی(user_cmd_completeopt_tab))
	واپس تہی;
    واپس (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    ساکن keyvalue_T *
get_commandtype(صحیح expand)
{
    صحیح i;

    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(command_complete_tab); ++i)
	اگر (command_complete_tab[i].کلید == expand)
	    واپس &command_complete_tab[i];

    واپس تہی;
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_مقصد_str(صحیح expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    اگر (kv == تہی || kv->قدر.string == تہی)
	واپس تہی;

    cmd_compl = kv->قدر.string;
    اگر (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*بفر;

	بفر = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	اگر (بفر == تہی)
	    واپس تہی;
	sprintf((حرف *)بفر, "%s,%s", cmd_compl, compl_arg);
	واپس بفر;
    }

    واپس vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    صحیح
cmdcomplete_str_مقصد_type(char_u *complete_str)
{
    keyvalue_T ہدف;
    keyvalue_T *اندراج_2;
    ساکن keyvalue_T *آخری_اندراج = تہی;	// cached result

    اگر (STRNCMP(complete_str, "custom,", 7) == 0)
	واپس EXPAND_USER_DEFINED;
    اگر (STRNCMP(complete_str, "customlist,", 11) == 0)
	واپس EXPAND_USER_LIST;

    ہدف.کلید = 0;
    ہدف.قدر.string = complete_str;
    ہدف.قدر.لمبائی = 0;			// not used, see cmp_keyvalue_value()

    اگر (آخری_اندراج != تہی && cmp_keyvalue_قدر(&ہدف, آخری_اندراج) == 0)
	اندراج_2 = آخری_اندراج;
    ورنہ
    {
	اندراج_2 = (keyvalue_T *)bsearch(&ہدف,
	    &command_complete_tab,
	    ARRAY_لمبائی(command_complete_tab),
	    حجم(command_complete_tab[0]),
	    cmp_keyvalue_قدر);
	اگر (اندراج_2 == تہی)
	    واپس EXPAND_NOTHING;

	آخری_اندراج = اندراج_2;
    }

    واپس اندراج_2->کلید;
}
#اگر_ختم

/*
 * List user commands starting with "name[name_len]".
 */
    ساکن خالی
uc_list(char_u *name, size_t name_len)
{
    صحیح		i, j;
    صحیح		ملا = FALSE;
    ucmd_T	*cmd;
    صحیح		len;
    صحیح		over;
    طویل	a;
    garray_T	*gap;
    keyvalue_T	*اندراج_2;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_بفر->b_ucmds;
    برائے (;;)
    {
	برائے (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (طویل)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    اگر (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || پیغام_filtered(cmd->uc_name))
		جاری;

	    // Put out the title first time
	    اگر (!ملا)
		پیغام_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    ملا = TRUE;
	    پیغام_putchar('\n');
	    اگر (got_int)
		توڑو;

	    // Special cases
	    len = 4;
	    اگر (a & EX_BANG)
	    {
		پیغام_putchar('!');
		--len;
	    }
	    اگر (a & EX_REGSTR)
	    {
		پیغام_putchar('"');
		--len;
	    }
	    اگر (gap != &ucmds)
	    {
		پیغام_putchar('b');
		--len;
	    }
	    اگر (a & EX_TRLBAR)
	    {
		پیغام_putchar('|');
		--len;
	    }
	    اگر (len != 0)
		پیغام_puts(&"    "[4 - len]);

	    پیغام_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (صحیح)cmd->uc_namelen + 4;

	    اگر (len < 21)
	    {
		// Field padding spaces   12345678901234567
		ساکن حرف spaces[18] = "                 ";
		پیغام_puts(&spaces[len - 4]);
		len = 21;
	    }
	    پیغام_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    انتخاب ((صحیح)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		حالت 0:				IObuff[len++] = '0'; توڑو;
		حالت (EX_EXTRA):		IObuff[len++] = '*'; توڑو;
		حالت (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; توڑو;
		حالت (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; توڑو;
		حالت (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; توڑو;
		حالت (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; توڑو;
	    }

	    کرو
	    {
		IObuff[len++] = ' ';
	    } جب (len < 5 - over);

	    // Address / Range
	    اگر (a & (EX_RANGE|EX_گنتی))
	    {
		اگر (a & EX_گنتی)
		{
		    // -count=N
		    len += vim_snprintf((حرف *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		ورنہ اگر (a & EX_DFLALL)
		    IObuff[len++] = '%';
		ورنہ اگر (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((حرف *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		ورنہ
		    IObuff[len++] = '.';
	    }

	    کرو
	    {
		IObuff[len++] = ' ';
	    } جب (len < 8 - over);

	    // Address Type
	    برائے (j = 0; j < (صحیح)ARRAY_لمبائی(addr_type_complete_tab); ++j)
		اگر (addr_type_complete_tab[j].کلید != ADDR_سطریں
			&& addr_type_complete_tab[j].کلید == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (صحیح)addr_type_complete_tab[j].shortnamelen;
		    توڑو;
		}

	    کرو
	    {
		IObuff[len++] = ' ';
	    } جب (len < 13 - over);

	    // Completion
	    اندراج_2 = get_commandtype(cmd->uc_compl);
	    اگر (اندراج_2 != تہی)
	    {
		STRCPY(IObuff + len, اندراج_2->قدر.string);
		len += (صحیح)اندراج_2->قدر.لمبائی;
#اگر_تعریف_شدہ FEAT_EVAL
		اگر (p_verbose > 0 && cmd->uc_compl_arg != تہی)
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
		    len += vim_snprintf((حرف *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#اگر_ختم
	    }

	    کرو
	    {
		IObuff[len++] = ' ';
	    } جب (len < 25 - over);

	    IObuff[len] = NUL;
	    پیغام_outtrans(IObuff);

	    پیغام_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (p_verbose > 0)
		آخری_سیٹ_پیغام(cmd->uc_script_ctx);
#اگر_ختم
	    برآمد_flush();
	    ui_breakcheck();
	    اگر (got_int)
		توڑو;
	}
	اگر (gap == &ucmds || i < gap->ga_len)
	    توڑو;
	gap = &ucmds;
    }

    اگر (!ملا)
	پیغام(_("No user-defined commands found"));

    --ucmd_locked;
}

    حرف *
uc_fun_cmd(خالی)
{
    ساکن char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    صحیح		i;

    برائے (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    واپس (حرف *)IObuff;
}

/*
 * Parse address type argument
 */
    ساکن صحیح
parse_addr_type_arg(
    char_u	*قدر,
    صحیح		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T ہدف;
    addrtype_T *اندراج_2;
    ساکن addrtype_T *آخری_اندراج;	// cached result

    ہدف.کلید = 0;
    ہدف.fullname = (حرف *)قدر;
    ہدف.fullnamelen = vallen;

    اگر (آخری_اندراج != تہی && cmp_addr_type(&ہدف, آخری_اندراج) == 0)
	اندراج_2 = آخری_اندراج;
    ورنہ
    {
	اندراج_2 = (addrtype_T *)bsearch(&ہدف,
	    &addr_type_complete_tab,
	    ARRAY_لمبائی(addr_type_complete_tab),
	    حجم(addr_type_complete_tab[0]),
	    cmp_addr_type);
	اگر (اندراج_2 == تہی)
	{
	    صحیح i;
	    char_u	*err = قدر;

	    برائے (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_پتہ_type_قدر_str), err);
	    واپس FAIL;
	}

	آخری_اندراج = اندراج_2;
    }

    *addr_type_arg = اندراج_2->کلید;

    واپس OK;
}

    ساکن صحیح
cmp_addr_type(ثابت خالی *a, ثابت خالی *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    واپس STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
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
    char_u	*قدر,
    صحیح		vallen,
    صحیح		*complp,
    طویل	*argt,
    char_u	**compl_arg غیر_مستعمل)
{
    char_u	*arg = تہی;
#اگر defined(FEAT_EVAL)
    size_t	arglen = 0;
#اگر_ختم
    صحیح		i;
    صحیح		valend = vallen;
    keyvalue_T	ہدف;
    keyvalue_T	*اندراج_2;
    ساکن keyvalue_T	*آخری_اندراج = تہی;	    // cached result

    // Look for any argument part - which is the part after any ','
    برائے (i = 0; i < vallen; ++i)
    {
	اگر (قدر[i] == ',')
	{
	    arg = &قدر[i + 1];
#اگر defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#اگر_ختم
	    valend = i;
	    توڑو;
	}
    }

    ہدف.کلید = 0;
    ہدف.قدر.string = قدر;
    ہدف.قدر.لمبائی = valend;

    اگر (آخری_اندراج != تہی && cmp_keyvalue_قدر_n(&ہدف, آخری_اندراج) == 0)
	اندراج_2 = آخری_اندراج;
    ورنہ
    {
	اندراج_2 = (keyvalue_T *)bsearch(&ہدف,
	    &command_complete_tab,
	    ARRAY_لمبائی(command_complete_tab),
	    حجم(command_complete_tab[0]),
	    cmp_keyvalue_قدر_n);
	اگر (اندراج_2 == تہی)
	{
	    semsg(_(e_invalid_complete_قدر_str), قدر);
	    واپس FAIL;
	}

	آخری_اندراج = اندراج_2;
    }

    *complp = اندراج_2->کلید;
    اگر (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    ورنہ اگر (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    اگر (
#اگر defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#اگر_ختم
								arg != تہی)
    {
	emsg(_(e_completion_دلیل_only_allowed_for_custom_completion));
	واپس FAIL;
    }

#اگر defined(FEAT_EVAL)
    اگر ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == تہی)
    {
	emsg(_(e_custom_completion_requires_function_دلیل));
	واپس FAIL;
    }

    اگر (arg != تہی)
	*compl_arg = vim_strnsave(arg, arglen);
#اگر_ختم

    واپس OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    ساکن صحیح
parse_completeopt_arg(char_u *قدر, صحیح vallen, صحیح *compl_opt)
{
    char_u  *p = قدر;
    char_u  *انتہا = قدر + vallen;
    صحیح	    flags = 0;

    اگر (vallen == 0)
    {
	semsg(_(e_دلیل_required_for_str), "-completeopt");
	واپس FAIL;
    }

    جب (p < انتہا)
    {
	char_u	*comma;
	size_t	itemlen;
	صحیح	matched = FALSE;
	صحیح	i;

	comma = vim_strchr(p, ',');
	اگر (comma == تہی || comma > انتہا)
	    itemlen = (size_t)(انتہا - p);
	ورنہ
	    itemlen = (size_t)(comma - p);

	اگر (itemlen == 0)
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str), "completeopt");
	    واپس FAIL;
	}

	برائے (i = 0; i < (صحیح)ARRAY_لمبائی(user_cmd_completeopt_tab); ++i)
	{
	    حرف *name = user_cmd_completeopt_tab[i];

	    اگر (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		اگر (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		توڑو;
	    }
	}
	اگر (!matched)
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str), "completeopt");
	    واپس FAIL;
	}

	p += itemlen;
	اگر (p < انتہا && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    واپس OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    ساکن صحیح
uc_جائزہ_attr(
    char_u	*attr,
    size_t	len,
    طویل	*argt,
    طویل	*def,
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
	واپس FAIL;
    }

    // First, try the simple attributes (no arguments)
    اگر (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    ورنہ اگر (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_بفر;
    ورنہ اگر (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    ورنہ اگر (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    ورنہ اگر (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    ورنہ
    {
	صحیح	i;
	char_u	*val = تہی;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	برائے (i = 0; i < (صحیح)len; ++i)
	{
	    اگر (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		توڑو;
	    }
	}

	اگر (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    اگر (vallen == 1)
	    {
		اگر (*val == '0')
		    // Do nothing - this is the default
		    ;
		ورنہ اگر (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		ورنہ اگر (*val == '*')
		    *argt |= EX_EXTRA;
		ورنہ اگر (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		ورنہ اگر (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		ورنہ اگر (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		ورنہ
		    جاؤ wrong_nargs;
	    }
	    ورنہ
	    {
wrong_nargs:
		emsg(_(e_invalid_نمبر_کا_arguments));
		واپس FAIL;
	    }
	}
	ورنہ اگر (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    اگر (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    ورنہ اگر (val != تہی)
	    {
		p = val;
		اگر (*def >= 0)
		{
two_گنتی:
		    emsg(_(e_گنتی_cannot_be_specified_twice));
		    واپس FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		اگر (p != val + vallen || vallen == 0)
		{
invalid_گنتی:
		    emsg(_(e_invalid_default_قدر_for_گنتی));
		    واپس FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    اگر (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_سطریں;
	}
	ورنہ اگر (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_گنتی | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    اگر (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    اگر (val != تہی)
	    {
		p = val;
		اگر (*def >= 0)
		    جاؤ two_گنتی;

		*def = getdigits(&p);

		اگر (p != val + vallen)
		    جاؤ invalid_گنتی;
	    }

	    اگر (*def < 0)
		*def = 0;
	}
	ورنہ اگر (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    اگر (val == تہی)
	    {
		semsg(_(e_دلیل_required_for_str), "-complete");
		واپس FAIL;
	    }

	    اگر (parse_compl_arg(val, (صحیح)vallen, complp, argt, compl_arg)
								      == FAIL)
		واپس FAIL;
	}
	ورنہ اگر (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    اگر (val == تہی)
	    {
		semsg(_(e_دلیل_required_for_str), "-completeopt");
		واپس FAIL;
	    }

	    اگر (parse_completeopt_arg(val, (صحیح)vallen, compl_opt) == FAIL)
		واپس FAIL;
	}
	ورنہ اگر (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    اگر (val == تہی)
	    {
		semsg(_(e_دلیل_required_for_str), "-addr");
		واپس FAIL;
	    }
	    اگر (parse_addr_type_arg(val, (صحیح)vallen, addr_type_arg) == FAIL)
		واپس FAIL;
	    اگر (*addr_type_arg != ADDR_سطریں)
		*argt |= EX_ZEROR;
	}
	ورنہ
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    واپس FAIL;
	}
    }

    واپس OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    ساکن صحیح
uc_جوڑو_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    طویل	argt,
    طویل	def,
    صحیح		flags,
    صحیح		compl,
    char_u	*compl_arg غیر_مستعمل,
    صحیح		compl_opt,
    cmd_addr_T	addr_type,
    صحیح		force)
{
    ucmd_T	*cmd = تہی;
    char_u	*p;
    صحیح		i;
    صحیح		cmp = 1;
    char_u	*rep_buf = تہی;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, تہی);
    اگر (rep_buf == تہی)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	اگر (rep_buf == تہی)
	    واپس FAIL;
    }

    // get address of growarray: global or in curbuf
    اگر (flags & UC_بفر)
    {
	gap = &curbuf->b_ucmds;
	اگر (gap->ga_itemsize == 0)
	    ga_init2(gap, حجم(ucmd_T), 4);
    }
    ورنہ
	gap = &ucmds;

    // Search for the command in the already defined commands.
    برائے (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	اگر (cmp == 0)
	{
	    اگر (name_len < cmd->uc_namelen)
		cmp = -1;
	    ورنہ اگر (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	اگر (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    اگر (!force
#اگر_تعریف_شدہ FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#اگر_ختم
		    )
	    {
		semsg(_(e_command_already_exists_جوڑو_bang_مقصد_replace_it_str),
									 name);
		جاؤ fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#اگر defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#اگر_ختم
	    توڑو;
	}

	// Stop as soon as we pass the name to add
	اگر (cmp < 0)
	    توڑو;
    }

    // Extend the array unless we're replacing an existing command
    اگر (cmp != 0)
    {
	اگر (ga_grow(gap, 1) == FAIL)
	    جاؤ fail;
	اگر ((p = vim_strnsave(name, name_len)) == تہی)
	    جاؤ fail;

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
    اگر (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_نسخہ = SCRIPT_نسخہ_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#اگر_تعریف_شدہ FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#اگر_ختم
    cmd->uc_addr_type = addr_type;

    واپس OK;

fail:
    vim_free(rep_buf);
#اگر defined(FEAT_EVAL)
    vim_free(compl_arg);
#اگر_ختم
    واپس FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_بلاک(exarg_T *eap, char_u *p, char_u **tofree, صحیح *flags)
{
    char_u *retp = p;
    char_u *بلاک = ڈھونڈو_cmd_بلاک_آغاز(p);

    اگر (بلاک != تہی && eap->ea_getline != تہی)
    {
	garray_T    ga;
	char_u	    *سطر = تہی;

	ga_init2(&ga, حجم(char_u *), 10);
	اگر (ga_copy_string(&ga, p) == FAIL)
	    واپس retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	اگر (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    برائے (;;)
	    {
		vim_free(سطر);
		اگر ((سطر = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == تہی)
		{
		    emsg(_(e_missing_rcurly));
		    توڑو;
		}
		اگر (ga_copy_string(&ga, سطر) == FAIL)
		    توڑو;
		اگر (*skipwhite(سطر) == '}')
		    توڑو;
	    }
	vim_free(سطر);
	retp = *tofree = ga_concat_لڑیاں(&ga, "\n");
	اگر (retp == تہی)
	    retp = p;
	ga_clear_لڑیاں(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	اگر (بلاک == p)
	    *flags |= UC_VIM9;
    }
    واپس retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    خالی
skip_cmd_بلاک(exarg_T *eap)
{
    char_u	*tofree = تہی;
    صحیح		flags = 0;

    (خالی)may_get_cmd_بلاک(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    خالی
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*انتہا;
    char_u	*p;
    طویل	argt = 0;
    طویل	def = -1;
    صحیح		flags = 0;
    صحیح		compl = EXPAND_NOTHING;
    char_u	*compl_arg = تہی;
    صحیح		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    صحیح		has_attr = (eap->arg[0] == '-');
    صحیح		name_len;

    اگر (eap->skip)
    {
	skip_cmd_بلاک(eap);
	واپس;
    }

    p = eap->arg;

    // Check for attributes
    جب (*p == '-')
    {
	++p;
	انتہا = skiptowhite(p);
	اگر (uc_جائزہ_attr(p, انتہا - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    جاؤ theend;
	p = skipwhite(انتہا);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    اگر (ASCII_ISALPHA(*p))
	جب (ASCII_ISALNUM(*p))
	    ++p;
    اگر (!سرے_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	جاؤ theend;
    }
    انتہا = p;
    name_len = (صحیح)(انتہا - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(انتہا);
    اگر (!has_attr && سرے_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    ورنہ اگر (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_آغاز_with_an_uppercase_letter));
    }
    ورنہ اگر ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_مستعمل_for_user_defined_command));
    }
    ورنہ اگر (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	اگر (اندر_vim9script())
	    emsg(_(e_complete_مستعمل_without_allowing_arguments));
	ورنہ
	    give_warning_with_ماخذ(
		       (char_u *)_(e_complete_مستعمل_without_allowing_arguments),
								   TRUE, TRUE);
    }
    ورنہ اگر ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_مستعمل_with_nargs_underscore));
    }
    ورنہ
    {
	char_u *tofree = تہی;

	p = may_get_cmd_بلاک(eap, p, &tofree, &flags);

	uc_جوڑو_command(name, انتہا - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	واپس;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    خالی
ex_comclear(exarg_T *eap غیر_مستعمل)
{
    uc_clear(&ucmds);
    اگر (curbuf != تہی)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ساکن صحیح
is_ucmd_locked(خالی)
{
    اگر (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    خالی
uc_clear(garray_T *gap)
{
    صحیح		i;
    ucmd_T	*cmd;

    اگر (is_ucmd_locked())
	واپس;

    برائے (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#اگر defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#اگر_ختم
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    خالی
ex_delcommand(exarg_T *eap)
{
    صحیح		i = 0;
    ucmd_T	*cmd = تہی;
    صحیح		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    صحیح		بفر_only = FALSE;

    اگر (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	بفر_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    برائے (;;)
    {
	برائے (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    اگر (res <= 0)
		توڑو;
	}
	اگر (gap == &ucmds || res == 0 || بفر_only)
	    توڑو;
	gap = &ucmds;
    }

    اگر (res != 0)
    {
	semsg(_(بفر_only
		    ? e_no_such_user_defined_command_اندر_current_بفر_str
		    : e_no_such_user_defined_command_str), arg);
	واپس;
    }

    اگر (is_ucmd_locked())
	واپس;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#اگر defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#اگر_ختم

    --gap->ga_len;

    اگر (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * حجم(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    ساکن char_u *
uc_بانٹو_دلائل(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    صحیح len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    جب (*p)
    {
	اگر (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	ورنہ اگر (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	ورنہ اگر (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	ورنہ اگر (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    اگر (*p == NUL)
		توڑو;
	    len += 4; // ", "
	}
	ورنہ
	{
	    صحیح charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    اگر (buf == تہی)
    {
	*lenp = 0;
	واپس buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    جب (*p)
    {
	اگر (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	ورنہ اگر (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	ورنہ اگر (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	ورنہ اگر (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    اگر (*p == NUL)
		توڑو;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	ورنہ
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    واپس buf;
}

    ساکن size_t
جوڑو_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    حرف	*mod_str,
    size_t	mod_strlen,
    صحیح		*multi_mods)
{
    اگر (buf != تہی)
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
    ورنہ
	*multi_mods = 1;

    واپس mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
جوڑو_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, صحیح *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    اگر (cmod->cmod_بانٹو & WSP_ABOVE)
	buflen += جوڑو_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    اگر (cmod->cmod_بانٹو & WSP_نیچے)
	buflen += جوڑو_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    اگر (cmod->cmod_بانٹو & WSP_BOT)
	buflen += جوڑو_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    اگر (cmod->cmod_tab > 0)
    {
	صحیح tabnr = cmod->cmod_tab - 1;

	اگر (tabnr == tabpage_اشاریہ(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += جوڑو_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	ورنہ
	{
	    حرف tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, حجم(tab_buf), "%dtab", tabnr);
	    buflen += جوڑو_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    اگر (cmod->cmod_بانٹو & WSP_چوٹی)
	buflen += جوڑو_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    اگر (cmod->cmod_بانٹو & WSP_VERT)
	buflen += جوڑو_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    اگر (cmod->cmod_بانٹو & WSP_HOR)
	buflen += جوڑو_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    واپس buflen;
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
    ساکن keyvalue_T mod_اندراج_tab[] =
    {
#اگر_تعریف_شدہ FEAT_BROWSE_CMD
	KEYVALUE_اندراج(CMOD_BROWSE, "browse"),
#اگر_ختم
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	KEYVALUE_اندراج(CMOD_CONFIRM, "confirm"),
#اگر_ختم
	KEYVALUE_اندراج(CMOD_HIDE, "hide"),
	KEYVALUE_اندراج(CMOD_KEEPALT, "keepalt"),
	KEYVALUE_اندراج(CMOD_KEEPJUMPS, "keepjumps"),
	KEYVALUE_اندراج(CMOD_KEEPMARKS, "keepmarks"),
	KEYVALUE_اندراج(CMOD_KEEPPATTERNS, "keeppatterns"),
	KEYVALUE_اندراج(CMOD_LOCKMARKS, "lockmarks"),
	KEYVALUE_اندراج(CMOD_NOSWAPFILE, "noswapfile"),
	KEYVALUE_اندراج(CMOD_UNSILENT, "unsilent"),
	KEYVALUE_اندراج(CMOD_NOAUTOCMD, "noautocmd"),
#اگر_تعریف_شدہ HAVE_SANDBOX
	KEYVALUE_اندراج(CMOD_SANDBOX, "sandbox"),
#اگر_ختم
	KEYVALUE_اندراج(CMOD_LEGACY, "legacy")
    };

    اگر (quote)
    {
	++buflen;
	اگر (buf != تہی)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    ورنہ
    اگر (buf != تہی)
	*buf = NUL;

    // the modifiers that are simple flags
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(mod_اندراج_tab); ++i)
	اگر (cmod->cmod_flags & mod_اندراج_tab[i].کلید)
	    buflen += جوڑو_cmd_modifier(buf, buflen,
		    (حرف *)mod_اندراج_tab[i].قدر.string,
		    mod_اندراج_tab[i].قدر.لمبائی, &multi_mods);

    // :silent
    اگر (cmod->cmod_flags & CMOD_SILENT)
    {
	اگر (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += جوڑو_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	ورنہ
	    buflen += جوڑو_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    اگر (cmod->cmod_verbose > 0)
    {
	صحیح verbose_قدر = cmod->cmod_verbose - 1;

	اگر (verbose_قدر == 1)
	    buflen += جوڑو_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	ورنہ
	{
	    حرف verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, حجم(verbose_buf), "%dverbose", verbose_قدر);
	    buflen += جوڑو_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += جوڑو_win_cmd_modifiers((buf == تہی) ? تہی : buf + buflen, cmod, &multi_mods);

    اگر (quote)
    {
	اگر (buf == تہی)
	    ++buflen;
	ورنہ
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    واپس buflen;
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
    ساکن size_t
uc_check_رمز(
    char_u	*رمز,
    size_t	len,
    char_u	*buf,
    ucmd_T	*cmd,		// the user command we're expanding
    exarg_T	*eap,		// ex arguments
    char_u	**بانٹو_buf,
    size_t	*بانٹو_len)
{
    size_t	نتیجہ = 0;
    char_u	*p = رمز + 1;
    size_t	l = len - 2;
    صحیح		quote = 0;
    شمار {
	ct_دلائل,
	ct_BANG,
	ct_گنتی,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_رجسٹر,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    اگر ((vim_strchr((char_u *)"qQfF", *p) != تہی) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    اگر (l <= 1)
	type = ct_NONE;
    ورنہ اگر (STRNICMP(p, "args>", l) == 0)
	type = ct_دلائل;
    ورنہ اگر (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    ورنہ اگر (STRNICMP(p, "count>", l) == 0)
	type = ct_گنتی;
    ورنہ اگر (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    ورنہ اگر (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    ورنہ اگر (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    ورنہ اگر (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    ورنہ اگر (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_رجسٹر;
    ورنہ اگر (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    انتخاب (type)
    {
    حالت ct_دلائل:
	// Simple case first
	اگر (*eap->arg == NUL)
	{
	    اگر (quote == 1)
	    {
		نتیجہ = 2;
		اگر (buf != تہی)
		    STRCPY(buf, "''");
	    }
	    ورنہ
		نتیجہ = 0;
	    توڑو;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	اگر ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	انتخاب (quote)
	{
	حالت 0: // No quoting, no splitting
	    نتیجہ = STRLEN(eap->arg);
	    اگر (buf != تہی)
		STRCPY(buf, eap->arg);
	    توڑو;
	حالت 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    صحیح	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    نتیجہ = STRLEN(eap->arg) + 2;
	    برائے (p = eap->arg; *p; ++p)
	    {
		اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		ورنہ اگر (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    اگر (VIM_ISWHITE(p[1]))
			--نتیجہ;
		    ++p;
		}
		ورنہ
		     اگر (*p == '\\' || *p == '"')
		    ++نتیجہ;
	    }

	    اگر (buf != تہی)
	    {
		*buf++ = '"';
		برائے (p = eap->arg; *p; ++p)
		{
		    اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    ورنہ اگر (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			اگر (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    ورنہ
			 اگر (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    توڑو;
	}
	حالت 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    اگر (*بانٹو_buf == تہی)
		*بانٹو_buf = uc_بانٹو_دلائل(eap->arg, بانٹو_len);

	    نتیجہ = *بانٹو_len;
	    اگر (buf != تہی && نتیجہ != 0)
		STRCPY(buf, *بانٹو_buf);

	    توڑو;
	}
	توڑو;

    حالت ct_BANG:
	نتیجہ = eap->forceit ? 1 : 0;
	اگر (quote)
	    نتیجہ += 2;
	اگر (buf != تہی)
	{
	    اگر (quote)
		*buf++ = '"';
	    اگر (eap->forceit)
		*buf++ = '!';
	    اگر (quote)
		*buf = '"';
	}
	توڑو;

    حالت ct_LINE1:
    حالت ct_LINE2:
    حالت ct_RANGE:
    حالت ct_گنتی:
    {
	حرف عدد_buf[NUMBUFLEN];
	طویل عدد = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_گنتی :
		   (eap->addr_گنتی > 0) ? eap->line2 : cmd->uc_def;
	size_t عدد_len;

	عدد_len = vim_snprintf(عدد_buf, حجم(عدد_buf), "%ld", عدد);
	نتیجہ = عدد_len;

	اگر (quote)
	    نتیجہ += 2;

	اگر (buf != تہی)
	{
	    اگر (quote)
		*buf++ = '"';
	    STRCPY(buf, عدد_buf);
	    buf += عدد_len;
	    اگر (quote)
		*buf = '"';
	}

	توڑو;
    }

    حالت ct_MODS:
    {
	نتیجہ = produce_cmdmods(buf, &cmdmod, quote);
	توڑو;
    }

    حالت ct_رجسٹر:
	نتیجہ = eap->regname ? 1 : 0;
	اگر (quote)
	    نتیجہ += 2;
	اگر (buf != تہی)
	{
	    اگر (quote)
		*buf++ = '\'';
	    اگر (eap->regname)
		*buf++ = eap->regname;
	    اگر (quote)
		*buf = '\'';
	}
	توڑو;

    حالت ct_LT:
	نتیجہ = 1;
	اگر (buf != تہی)
	    *buf = '<';
	توڑو;

    طےشدہ:
	// Not recognized: just copy the '<' and return -1.
	نتیجہ = (size_t)-1;
	اگر (buf != تہی)
	    *buf = '<';
	توڑو;
    }

    واپس نتیجہ;
}

/*
 * Execute a user defined command.
 */
    خالی
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*آغاز;
    char_u	*انتہا = تہی;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	بانٹو_len = 0;
    char_u	*بانٹو_buf = تہی;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    صحیح		restore_current_sctx = FALSE;
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح		restore_script_نسخہ = 0;
#اگر_ختم

    اگر (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    ورنہ
	cmd = USER_CMD_GA(&prevwin_curwin()->w_بفر->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = تہی;
    برائے (;;)
    {
	p = cmd->uc_rep;    //ماخذ
	q = buf;	    // destination
	totlen = 0;

	برائے (;;)
	{
	    آغاز = vim_strchr(p, '<');
	    اگر (آغاز != تہی)
		انتہا = vim_strchr(آغاز + 1, '>');
	    اگر (buf != تہی)
	    {
		برائے (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		اگر (*ksp == K_SPECIAL
			&& (آغاز == تہی || ksp < آغاز || انتہا == تہی)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#اگر_تعریف_شدہ FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (صحیح)KE_CSI)
#اگر_ختم
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
		    جاری;
		}
	    }

	    // break if no <item> is found
	    اگر (آغاز == تہی || انتہا == تہی)
		توڑو;

	    // Include the '>'
	    ++انتہا;

	    // Take everything up to the '<'
	    len = آغاز - p;
	    اگر (buf == تہی)
		totlen += len;
	    ورنہ
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_رمز(آغاز, انتہا - آغاز, q, cmd, eap,
			     &بانٹو_buf, &بانٹو_len);
	    اگر (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = آغاز + 1;
		len = 1;
	    }
	    ورنہ
		p = انتہا;
	    اگر (buf == تہی)
		totlen += len;
	    ورنہ
		q += len;
	}
	اگر (buf != تہی)	    // second time here, finished
	{
	    STRCPY(q, p);
	    توڑو;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	اگر (buf == تہی)
	{
	    vim_free(بانٹو_buf);
	    واپس;
	}
    }

    اگر ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_نسخہ = cmd->uc_script_ctx.sc_نسخہ;
#اگر_تعریف_شدہ FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	اگر (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_نسخہ =
				  SCRIPT_شے(current_sctx.sc_sid)->sn_نسخہ;
	    SCRIPT_شے(current_sctx.sc_sid)->sn_نسخہ = SCRIPT_نسخہ_VIM9;
	}
#اگر_ختم
    }

    (خالی)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    اگر (restore_current_sctx)
    {
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (restore_script_نسخہ != 0)
	    SCRIPT_شے(current_sctx.sc_sid)->sn_نسخہ =
							restore_script_نسخہ;
#اگر_ختم
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(بانٹو_buf);
}
