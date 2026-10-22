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

#include "vim.h"

typedef sa_nracanaa ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    long	uc_def;		// The default value for a range/count
    puur_naa_nka		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    puur_naa_nka		uc_flags;	// some UC_ flags
    puur_naa_nka		uc_compl_opt;	// completion options (UCC_ flags)
#ifdef FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#endif
} ucmd_T;

// List of all user commands.
static garray_T ucmds = {0, 0, parimaa_na(ucmd_T), 4, NULL};

// When non-zero it is not allowed to add or remove user commands
static puur_naa_nka ucmd_locked = 0;

#define USER_CMD(i) (&((ucmd_T *)(ucmds.ga_data))[i])
#define USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_data))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
static keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ENTRY(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ENTRY(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ENTRY(EXPAND_BEHAVE, "behave"),
#ᤀᤡ᤺ defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_BREAKPOINT, "breakpoint"),
#endif
    KEYVALUE_ENTRY(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ENTRY(EXPAND_COLORS, "color"),
    KEYVALUE_ENTRY(EXPAND_COMMANDS, "command"),
    KEYVALUE_ENTRY(EXPAND_COMPILER, "compiler"),
#ᤀᤡ᤺ defined(FEAT_CSCOPE)
    KEYVALUE_ENTRY(EXPAND_CSCOPE, "cscope"),
#endif
#ᤀᤡ᤺ defined(FEAT_EVAL)
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
#ᤀᤡ᤺ defined(FEAT_KEYMAP)
    KEYVALUE_ENTRY(EXPAND_KEYMAP, "keymap"),
#endif
#ᤀᤡ᤺ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
#ᤀᤡ᤺ defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_SCRIPTNAMES, "scriptnames"),
#endif
    KEYVALUE_ENTRY(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ENTRY(EXPAND_SHELLCMDLINE, "shellcmdline"),
#ᤀᤡ᤺ defined(FEAT_SIGNS)
    KEYVALUE_ENTRY(EXPAND_SIGN, "sign"),
#endif
    KEYVALUE_ENTRY(EXPAND_OWNSYNTAX, "syntax"),
#ᤀᤡ᤺ defined(FEAT_PROFILE)
    KEYVALUE_ENTRY(EXPAND_SYNTIME, "syntime"),
#endif
    KEYVALUE_ENTRY(EXPAND_TAGS, "tag"),
    KEYVALUE_ENTRY(EXPAND_TAGS_LISTFILES, "tag_listfiles"),
    KEYVALUE_ENTRY(EXPAND_USER, "user"),
    KEYVALUE_ENTRY(EXPAND_USER_VARS, "var")
};

typedef sa_nracanaa
{
    cmd_addr_T key;
    var_na *fullname;
    size_t fullnamelen;
    var_na *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#define ADDRTYPE_ENTRY(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
static addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ENTRY(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ENTRY(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ENTRY(ADDR_LINES, "lines", "line"),
    ADDRTYPE_ENTRY(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ENTRY(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ENTRY(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ENTRY(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ENTRY(ADDR_WINDOWS, "windows", "win")
};

static puur_naa_nka cmp_addr_type(const void *a, const void *b);

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
    puur_naa_nka		*full,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    puur_naa_nka		*complp) // completion flags or NULL
{
    puur_naa_nka		len = (puur_naa_nka)(p - eap->cmd);
    puur_naa_nka		j, k, matchlen = 0;
    ucmd_T	*uc;
    puur_naa_nka		found = FALSE;
    puur_naa_nka		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    puur_naa_nka		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    for (;;)
    {
	for (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    ᤀᤠ᤺ (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    ᤀᤡ᤺ (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		ᤀᤡ᤺ (k == len && found && *np != NUL)
		{
		    ᤀᤡ᤺ (gap == &ucmds)
			ᤀᤡᤴ NULL;
		    amb_local = TRUE;
		}

		ᤀᤡ᤺ (!found || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    ᤀᤡ᤺ (k == len)
			found = TRUE;
		    ᤀᤡ᤺ᤆᤡ
			possible = TRUE;

		    ᤀᤡ᤺ (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    ᤀᤡ᤺ᤆᤡ
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (long)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    ᤀᤡ᤺ (complp != NULL)
			*complp = uc->uc_compl;
#ifdef FEAT_EVAL
		    ᤀᤡ᤺ (xp != NULL)
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
		    ᤀᤡ᤺ (k == len && *np == NUL)
		    {
			ᤀᤡ᤺ (full != NULL)
			    *full = TRUE;
			amb_local = FALSE;
			bha_nga;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	ᤀᤡ᤺ (j < gap->ga_len || gap == &ucmds)
	    bha_nga;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    ᤀᤡ᤺ (amb_local)
    {
	ᤀᤡ᤺ (xp != NULL)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	ᤀᤡᤴ NULL;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    ᤀᤡ᤺ (found || possible)
	ᤀᤡᤴ p + (matchlen - len);
    ᤀᤡᤴ p;
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
    ᤀᤠ᤺ (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	ᤀᤡ᤺ (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    ᤀᤡ᤺ (p == NULL)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_pattern = arg;
		ᤀᤡᤴ NULL;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    ᤀᤡ᤺ (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_pattern = p + 1;
	    }
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_pattern = p + 1;
		for (char_u *c = p + 1; *c != NUL; ++c)
		    ᤀᤡ᤺ (*c == ',')
			xp->xp_pattern = c + 1;
	    }
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_pattern = p + 1;
	    }
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_pattern = p + 1;
	    }
	    ᤀᤡᤴ NULL;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    ᤀᤡ᤺ (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_pattern = arg;
	ᤀᤡᤴ NULL;
    }

    // And finally comes a normal command
    ᤀᤡᤴ skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
set_context_in_user_cmdarg(
	char_u		*cmd UNUSED,
	char_u		*arg,
	long		argt,
	puur_naa_nka		context,
	expand_T	*xp,
	puur_naa_nka		forceit)
{
    char_u	*p;

    ᤀᤡ᤺ (context == EXPAND_NOTHING)
	ᤀᤡᤴ NULL;

    ᤀᤡ᤺ (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	ᤀᤡᤴ NULL;
    }

#ifdef FEAT_MENU
    ᤀᤡ᤺ (context == EXPAND_MENUS)
	ᤀᤡᤴ set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif
    ᤀᤡ᤺ (context == EXPAND_COMMANDS)
	ᤀᤡᤴ arg;
    ᤀᤡ᤺ (context == EXPAND_MAPPINGS)
	ᤀᤡᤴ set_context_in_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    ᤀᤡ᤺ (!(argt & EX_ARGSPACE))
    {
	p = arg;
	ᤀᤠ᤺ (*p)
	{
	    ᤀᤡ᤺ (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_pattern = arg;
    xp->xp_context = context;

    ᤀᤡᤴ NULL;
}

    char_u *
expand_user_command_name(puur_naa_nka idx)
{
    ᤀᤡᤴ get_user_commands(NULL, idx - (puur_naa_nka)CMD_SIZE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp UNUSED, puur_naa_nka idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_buffer;

    ᤀᤡ᤺ (idx < buf->b_ucmds.ga_len)
	ᤀᤡᤴ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    ᤀᤡ᤺ (idx < ucmds.ga_len)
    {
	puur_naa_nka	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	for (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    ᤀᤡ᤺ (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		ᤀᤡᤴ (char_u *)"";
	ᤀᤡᤴ name;
    }
    ᤀᤡᤴ NULL;
}

#ifdef FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(puur_naa_nka idx, puur_naa_nka cmdidx)
{
    ᤀᤡ᤺ (cmdidx == CMD_USER && idx < ucmds.ga_len)
	ᤀᤡᤴ USER_CMD(idx)->uc_name;
    ᤀᤡ᤺ (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_buffer;

	ᤀᤡ᤺ (idx < buf->b_ucmds.ga_len)
	    ᤀᤡᤴ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    ᤀᤡᤴ NULL;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    puur_naa_nka
user_command_info(puur_naa_nka idx, puur_naa_nka cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_buffer->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    ᤀᤡ᤺ (idx >= gap->ga_len)
	ᤀᤡᤴ FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_add_string(d, "name", uc->uc_name);
    dict_add_string(d, "kind", (char_u *)"user");
    dict_add_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_add_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_to_str(uc->uc_compl, uc->uc_compl_arg);
    dict_add_string(d, "complete", compl == NULL ? (char_u *)"" : compl);
    vim_free(compl);
    dict_add_string(d, "definition", uc->uc_rep);
    dict_add_number(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_add_number(d, "lnum", uc->uc_script_ctx.sc_lnum);
    ᤀᤡᤴ OK;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᤀᤡ᤺ (idx < 0 || idx >= (puur_naa_nka)ARRAY_LENGTH(addr_type_complete_tab))
	ᤀᤡᤴ NULL;
    ᤀᤡᤴ (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp UNUSED, puur_naa_nka idx)
{
    static var_na *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    ᤀᤡ᤺ (idx < 0 || idx >= (puur_naa_nka)ARRAY_LENGTH(user_cmd_flags))
	ᤀᤡᤴ NULL;
    ᤀᤡᤴ (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp UNUSED, puur_naa_nka idx)
{
    static var_na *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    ᤀᤡ᤺ (idx < 0 || idx >= (puur_naa_nka)ARRAY_LENGTH(user_cmd_nargs))
	ᤀᤡᤴ NULL;
    ᤀᤡᤴ (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᤀᤡ᤺ (idx < 0 || idx >= (puur_naa_nka)ARRAY_LENGTH(command_complete_tab))
	ᤀᤡᤴ NULL;
    ᤀᤡᤴ command_complete_tab[idx].value.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
static var_na *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᤀᤡ᤺ (idx < 0 || idx >= (puur_naa_nka)ARRAY_LENGTH(user_cmd_completeopt_tab))
	ᤀᤡᤴ NULL;
    ᤀᤡᤴ (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    static keyvalue_T *
get_commandtype(puur_naa_nka expand)
{
    puur_naa_nka i;

    for (i = 0; i < (puur_naa_nka)ARRAY_LENGTH(command_complete_tab); ++i)
	ᤀᤡ᤺ (command_complete_tab[i].key == expand)
	    ᤀᤡᤴ &command_complete_tab[i];

    ᤀᤡᤴ NULL;
}

#ifdef FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_to_str(puur_naa_nka expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    ᤀᤡ᤺ (kv == NULL || kv->value.string == NULL)
	ᤀᤡᤴ NULL;

    cmd_compl = kv->value.string;
    ᤀᤡ᤺ (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*buffer;

	buffer = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	ᤀᤡ᤺ (buffer == NULL)
	    ᤀᤡᤴ NULL;
	sprintf((var_na *)buffer, "%s,%s", cmd_compl, compl_arg);
	ᤀᤡᤴ buffer;
    }

    ᤀᤡᤴ vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    puur_naa_nka
cmdcomplete_str_to_type(char_u *complete_str)
{
    keyvalue_T target;
    keyvalue_T *entry;
    static keyvalue_T *last_entry = NULL;	// cached result

    ᤀᤡ᤺ (STRNCMP(complete_str, "custom,", 7) == 0)
	ᤀᤡᤴ EXPAND_USER_DEFINED;
    ᤀᤡ᤺ (STRNCMP(complete_str, "customlist,", 11) == 0)
	ᤀᤡᤴ EXPAND_USER_LIST;

    target.key = 0;
    target.value.string = complete_str;
    target.value.length = 0;			// not used, see cmp_keyvalue_value()

    ᤀᤡ᤺ (last_entry != NULL && cmp_keyvalue_value(&target, last_entry) == 0)
	entry = last_entry;
    ᤀᤡ᤺ᤆᤡ
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    parimaa_na(command_complete_tab[0]),
	    cmp_keyvalue_value);
	ᤀᤡ᤺ (entry == NULL)
	    ᤀᤡᤴ EXPAND_NOTHING;

	last_entry = entry;
    }

    ᤀᤡᤴ entry->key;
}
#endif

/*
 * List user commands starting with "name[name_len]".
 */
    static void
uc_list(char_u *name, size_t name_len)
{
    puur_naa_nka		i, j;
    puur_naa_nka		found = FALSE;
    ucmd_T	*cmd;
    puur_naa_nka		len;
    puur_naa_nka		over;
    long	a;
    garray_T	*gap;
    keyvalue_T	*entry;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    for (;;)
    {
	for (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (long)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    ᤀᤡ᤺ (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || message_filtered(cmd->uc_name))
		continue;

	    // Put out the title first time
	    ᤀᤡ᤺ (!found)
		msg_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    found = TRUE;
	    msg_putchar('\n');
	    ᤀᤡ᤺ (got_int)
		bha_nga;

	    // Special cases
	    len = 4;
	    ᤀᤡ᤺ (a & EX_BANG)
	    {
		msg_putchar('!');
		--len;
	    }
	    ᤀᤡ᤺ (a & EX_REGSTR)
	    {
		msg_putchar('"');
		--len;
	    }
	    ᤀᤡ᤺ (gap != &ucmds)
	    {
		msg_putchar('b');
		--len;
	    }
	    ᤀᤡ᤺ (a & EX_TRLBAR)
	    {
		msg_putchar('|');
		--len;
	    }
	    ᤀᤡ᤺ (len != 0)
		msg_puts(&"    "[4 - len]);

	    msg_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (puur_naa_nka)cmd->uc_namelen + 4;

	    ᤀᤡ᤺ (len < 21)
	    {
		// Field padding spaces   12345678901234567
		static var_na spaces[18] = "                 ";
		msg_puts(&spaces[len - 4]);
		len = 21;
	    }
	    msg_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    switch ((puur_naa_nka)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		pak_sa 0:				IObuff[len++] = '0'; bha_nga;
		pak_sa (EX_EXTRA):		IObuff[len++] = '*'; bha_nga;
		pak_sa (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; bha_nga;
		pak_sa (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; bha_nga;
		pak_sa (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; bha_nga;
		pak_sa (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; bha_nga;
	    }

	    do
	    {
		IObuff[len++] = ' ';
	    } ᤀᤠ᤺ (len < 5 - over);

	    // Address / Range
	    ᤀᤡ᤺ (a & (EX_RANGE|EX_COUNT))
	    {
		ᤀᤡ᤺ (a & EX_COUNT)
		{
		    // -count=N
		    len += vim_snprintf((var_na *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (a & EX_DFLALL)
		    IObuff[len++] = '%';
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((var_na *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		ᤀᤡ᤺ᤆᤡ
		    IObuff[len++] = '.';
	    }

	    do
	    {
		IObuff[len++] = ' ';
	    } ᤀᤠ᤺ (len < 8 - over);

	    // Address Type
	    for (j = 0; j < (puur_naa_nka)ARRAY_LENGTH(addr_type_complete_tab); ++j)
		ᤀᤡ᤺ (addr_type_complete_tab[j].key != ADDR_LINES
			&& addr_type_complete_tab[j].key == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (puur_naa_nka)addr_type_complete_tab[j].shortnamelen;
		    bha_nga;
		}

	    do
	    {
		IObuff[len++] = ' ';
	    } ᤀᤠ᤺ (len < 13 - over);

	    // Completion
	    entry = get_commandtype(cmd->uc_compl);
	    ᤀᤡ᤺ (entry != NULL)
	    {
		STRCPY(IObuff + len, entry->value.string);
		len += (puur_naa_nka)entry->value.length;
#ifdef FEAT_EVAL
		ᤀᤡ᤺ (p_verbose > 0 && cmd->uc_compl_arg != NULL)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    ᤀᤡ᤺ (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (puur_naa_nka)uc_compl_arglen;
		    }
		}
		ᤀᤡ᤺ (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((var_na *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#endif
	    }

	    do
	    {
		IObuff[len++] = ' ';
	    } ᤀᤠ᤺ (len < 25 - over);

	    IObuff[len] = NUL;
	    msg_outtrans(IObuff);

	    msg_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ifdef FEAT_EVAL
	    ᤀᤡ᤺ (p_verbose > 0)
		last_set_msg(cmd->uc_script_ctx);
#endif
	    out_flush();
	    ui_breakcheck();
	    ᤀᤡ᤺ (got_int)
		bha_nga;
	}
	ᤀᤡ᤺ (gap == &ucmds || i < gap->ga_len)
	    bha_nga;
	gap = &ucmds;
    }

    ᤀᤡ᤺ (!found)
	msg(_("No user-defined commands found"));

    --ucmd_locked;
}

    var_na *
uc_fun_cmd(void)
{
    static char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    puur_naa_nka		i;

    for (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    ᤀᤡᤴ (var_na *)IObuff;
}

/*
 * Parse address type argument
 */
    static puur_naa_nka
parse_addr_type_arg(
    char_u	*value,
    puur_naa_nka		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T target;
    addrtype_T *entry;
    static addrtype_T *last_entry;	// cached result

    target.key = 0;
    target.fullname = (var_na *)value;
    target.fullnamelen = vallen;

    ᤀᤡ᤺ (last_entry != NULL && cmp_addr_type(&target, last_entry) == 0)
	entry = last_entry;
    ᤀᤡ᤺ᤆᤡ
    {
	entry = (addrtype_T *)bsearch(&target,
	    &addr_type_complete_tab,
	    ARRAY_LENGTH(addr_type_complete_tab),
	    parimaa_na(addr_type_complete_tab[0]),
	    cmp_addr_type);
	ᤀᤡ᤺ (entry == NULL)
	{
	    puur_naa_nka i;
	    char_u	*err = value;

	    for (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_address_type_value_str), err);
	    ᤀᤡᤴ FAIL;
	}

	last_entry = entry;
    }

    *addr_type_arg = entry->key;

    ᤀᤡᤴ OK;
}

    static puur_naa_nka
cmp_addr_type(const void *a, const void *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    ᤀᤡᤴ STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    puur_naa_nka
parse_compl_arg(
    char_u	*value,
    puur_naa_nka		vallen,
    puur_naa_nka		*complp,
    long	*argt,
    char_u	**compl_arg UNUSED)
{
    char_u	*arg = NULL;
#ᤀᤡ᤺ defined(FEAT_EVAL)
    size_t	arglen = 0;
#endif
    puur_naa_nka		i;
    puur_naa_nka		valend = vallen;
    keyvalue_T	target;
    keyvalue_T	*entry;
    static keyvalue_T	*last_entry = NULL;	    // cached result

    // Look for any argument part - which is the part after any ','
    for (i = 0; i < vallen; ++i)
    {
	ᤀᤡ᤺ (value[i] == ',')
	{
	    arg = &value[i + 1];
#ᤀᤡ᤺ defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#endif
	    valend = i;
	    bha_nga;
	}
    }

    target.key = 0;
    target.value.string = value;
    target.value.length = valend;

    ᤀᤡ᤺ (last_entry != NULL && cmp_keyvalue_value_n(&target, last_entry) == 0)
	entry = last_entry;
    ᤀᤡ᤺ᤆᤡ
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    parimaa_na(command_complete_tab[0]),
	    cmp_keyvalue_value_n);
	ᤀᤡ᤺ (entry == NULL)
	{
	    semsg(_(e_invalid_complete_value_str), value);
	    ᤀᤡᤴ FAIL;
	}

	last_entry = entry;
    }

    *complp = entry->key;
    ᤀᤡ᤺ (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    ᤀᤡ᤺ (
#ᤀᤡ᤺ defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#endif
								arg != NULL)
    {
	emsg(_(e_completion_argument_only_allowed_for_custom_completion));
	ᤀᤡᤴ FAIL;
    }

#ᤀᤡ᤺ defined(FEAT_EVAL)
    ᤀᤡ᤺ ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == NULL)
    {
	emsg(_(e_custom_completion_requires_function_argument));
	ᤀᤡᤴ FAIL;
    }

    ᤀᤡ᤺ (arg != NULL)
	*compl_arg = vim_strnsave(arg, arglen);
#endif

    ᤀᤡᤴ OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    static puur_naa_nka
parse_completeopt_arg(char_u *value, puur_naa_nka vallen, puur_naa_nka *compl_opt)
{
    char_u  *p = value;
    char_u  *end = value + vallen;
    puur_naa_nka	    flags = 0;

    ᤀᤡ᤺ (vallen == 0)
    {
	semsg(_(e_argument_required_for_str), "-completeopt");
	ᤀᤡᤴ FAIL;
    }

    ᤀᤠ᤺ (p < end)
    {
	char_u	*comma;
	size_t	itemlen;
	puur_naa_nka	matched = FALSE;
	puur_naa_nka	i;

	comma = vim_strchr(p, ',');
	ᤀᤡ᤺ (comma == NULL || comma > end)
	    itemlen = (size_t)(end - p);
	ᤀᤡ᤺ᤆᤡ
	    itemlen = (size_t)(comma - p);

	ᤀᤡ᤺ (itemlen == 0)
	{
	    semsg(_(e_invalid_value_for_argument_str), "completeopt");
	    ᤀᤡᤴ FAIL;
	}

	for (i = 0; i < (puur_naa_nka)ARRAY_LENGTH(user_cmd_completeopt_tab); ++i)
	{
	    var_na *name = user_cmd_completeopt_tab[i];

	    ᤀᤡ᤺ (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		ᤀᤡ᤺ (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		bha_nga;
	    }
	}
	ᤀᤡ᤺ (!matched)
	{
	    semsg(_(e_invalid_value_for_argument_str), "completeopt");
	    ᤀᤡᤴ FAIL;
	}

	p += itemlen;
	ᤀᤡ᤺ (p < end && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    ᤀᤡᤴ OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    static puur_naa_nka
uc_scan_attr(
    char_u	*attr,
    size_t	len,
    long	*argt,
    long	*def,
    puur_naa_nka		*flags,
    puur_naa_nka		*complp,
    char_u	**compl_arg,
    puur_naa_nka		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    ᤀᤡ᤺ (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	ᤀᤡᤴ FAIL;
    }

    // First, try the simple attributes (no arguments)
    ᤀᤡ᤺ (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_BUFFER;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    ᤀᤡ᤺ᤆᤡ
    {
	puur_naa_nka	i;
	char_u	*val = NULL;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	for (i = 0; i < (puur_naa_nka)len; ++i)
	{
	    ᤀᤡ᤺ (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		bha_nga;
	    }
	}

	ᤀᤡ᤺ (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    ᤀᤡ᤺ (vallen == 1)
	    {
		ᤀᤡ᤺ (*val == '0')
		    // Do nothing - this is the default
		    ;
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*val == '*')
		    *argt |= EX_EXTRA;
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		ᤀᤡ᤺ᤆᤡ
		    goto wrong_nargs;
	    }
	    ᤀᤡ᤺ᤆᤡ
	    {
wrong_nargs:
		emsg(_(e_invalid_number_of_arguments));
		ᤀᤡᤴ FAIL;
	    }
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ᤀᤡ᤺ (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (val != NULL)
	    {
		p = val;
		ᤀᤡ᤺ (*def >= 0)
		{
two_count:
		    emsg(_(e_count_cannot_be_specified_twice));
		    ᤀᤡᤴ FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		ᤀᤡ᤺ (p != val + vallen || vallen == 0)
		{
invalid_count:
		    emsg(_(e_invalid_default_value_for_count));
		    ᤀᤡᤴ FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    ᤀᤡ᤺ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_LINES;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_COUNT | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    ᤀᤡ᤺ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    ᤀᤡ᤺ (val != NULL)
	    {
		p = val;
		ᤀᤡ᤺ (*def >= 0)
		    goto two_count;

		*def = getdigits(&p);

		ᤀᤡ᤺ (p != val + vallen)
		    goto invalid_count;
	    }

	    ᤀᤡ᤺ (*def < 0)
		*def = 0;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    ᤀᤡ᤺ (val == NULL)
	    {
		semsg(_(e_argument_required_for_str), "-complete");
		ᤀᤡᤴ FAIL;
	    }

	    ᤀᤡ᤺ (parse_compl_arg(val, (puur_naa_nka)vallen, complp, argt, compl_arg)
								      == FAIL)
		ᤀᤡᤴ FAIL;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    ᤀᤡ᤺ (val == NULL)
	    {
		semsg(_(e_argument_required_for_str), "-completeopt");
		ᤀᤡᤴ FAIL;
	    }

	    ᤀᤡ᤺ (parse_completeopt_arg(val, (puur_naa_nka)vallen, compl_opt) == FAIL)
		ᤀᤡᤴ FAIL;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ᤀᤡ᤺ (val == NULL)
	    {
		semsg(_(e_argument_required_for_str), "-addr");
		ᤀᤡᤴ FAIL;
	    }
	    ᤀᤡ᤺ (parse_addr_type_arg(val, (puur_naa_nka)vallen, addr_type_arg) == FAIL)
		ᤀᤡᤴ FAIL;
	    ᤀᤡ᤺ (*addr_type_arg != ADDR_LINES)
		*argt |= EX_ZEROR;
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    ᤀᤡᤴ FAIL;
	}
    }

    ᤀᤡᤴ OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    static puur_naa_nka
uc_add_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    long	argt,
    long	def,
    puur_naa_nka		flags,
    puur_naa_nka		compl,
    char_u	*compl_arg UNUSED,
    puur_naa_nka		compl_opt,
    cmd_addr_T	addr_type,
    puur_naa_nka		force)
{
    ucmd_T	*cmd = NULL;
    char_u	*p;
    puur_naa_nka		i;
    puur_naa_nka		cmp = 1;
    char_u	*rep_buf = NULL;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, NULL);
    ᤀᤡ᤺ (rep_buf == NULL)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	ᤀᤡ᤺ (rep_buf == NULL)
	    ᤀᤡᤴ FAIL;
    }

    // get address of growarray: global or in curbuf
    ᤀᤡ᤺ (flags & UC_BUFFER)
    {
	gap = &curbuf->b_ucmds;
	ᤀᤡ᤺ (gap->ga_itemsize == 0)
	    ga_init2(gap, parimaa_na(ucmd_T), 4);
    }
    ᤀᤡ᤺ᤆᤡ
	gap = &ucmds;

    // Search for the command in the already defined commands.
    for (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	ᤀᤡ᤺ (cmp == 0)
	{
	    ᤀᤡ᤺ (name_len < cmd->uc_namelen)
		cmp = -1;
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	ᤀᤡ᤺ (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    ᤀᤡ᤺ (!force
#ifdef FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#endif
		    )
	    {
		semsg(_(e_command_already_exists_add_bang_to_replace_it_str),
									 name);
		goto fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#ᤀᤡ᤺ defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#endif
	    bha_nga;
	}

	// Stop as soon as we pass the name to add
	ᤀᤡ᤺ (cmp < 0)
	    bha_nga;
    }

    // Extend the array unless we're replacing an existing command
    ᤀᤡ᤺ (cmp != 0)
    {
	ᤀᤡ᤺ (ga_grow(gap, 1) == FAIL)
	    goto fail;
	ᤀᤡ᤺ ((p = vim_strnsave(name, name_len)) == NULL)
	    goto fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * parimaa_na(ucmd_T));

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
    ᤀᤡ᤺ (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ifdef FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#endif
    cmd->uc_addr_type = addr_type;

    ᤀᤡᤴ OK;

fail:
    vim_free(rep_buf);
#ᤀᤡ᤺ defined(FEAT_EVAL)
    vim_free(compl_arg);
#endif
    ᤀᤡᤴ FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_block(exarg_T *eap, char_u *p, char_u **tofree, puur_naa_nka *flags)
{
    char_u *retp = p;
    char_u *block = find_cmd_block_start(p);

    ᤀᤡ᤺ (block != NULL && eap->ea_getline != NULL)
    {
	garray_T    ga;
	char_u	    *line = NULL;

	ga_init2(&ga, parimaa_na(char_u *), 10);
	ᤀᤡ᤺ (ga_copy_string(&ga, p) == FAIL)
	    ᤀᤡᤴ retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	ᤀᤡ᤺ (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    for (;;)
	    {
		vim_free(line);
		ᤀᤡ᤺ ((line = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == NULL)
		{
		    emsg(_(e_missing_rcurly));
		    bha_nga;
		}
		ᤀᤡ᤺ (ga_copy_string(&ga, line) == FAIL)
		    bha_nga;
		ᤀᤡ᤺ (*skipwhite(line) == '}')
		    bha_nga;
	    }
	vim_free(line);
	retp = *tofree = ga_concat_strings(&ga, "\n");
	ᤀᤡ᤺ (retp == NULL)
	    retp = p;
	ga_clear_strings(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	ᤀᤡ᤺ (block == p)
	    *flags |= UC_VIM9;
    }
    ᤀᤡᤴ retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    void
skip_cmd_block(exarg_T *eap)
{
    char_u	*tofree = NULL;
    puur_naa_nka		flags = 0;

    (void)may_get_cmd_block(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    void
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*end;
    char_u	*p;
    long	argt = 0;
    long	def = -1;
    puur_naa_nka		flags = 0;
    puur_naa_nka		compl = EXPAND_NOTHING;
    char_u	*compl_arg = NULL;
    puur_naa_nka		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    puur_naa_nka		has_attr = (eap->arg[0] == '-');
    puur_naa_nka		name_len;

    ᤀᤡ᤺ (eap->skip)
    {
	skip_cmd_block(eap);
	ᤀᤡᤴ;
    }

    p = eap->arg;

    // Check for attributes
    ᤀᤠ᤺ (*p == '-')
    {
	++p;
	end = skiptowhite(p);
	ᤀᤡ᤺ (uc_scan_attr(p, end - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    goto theend;
	p = skipwhite(end);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    ᤀᤡ᤺ (ASCII_ISALPHA(*p))
	ᤀᤠ᤺ (ASCII_ISALNUM(*p))
	    ++p;
    ᤀᤡ᤺ (!ends_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	goto theend;
    }
    end = p;
    name_len = (puur_naa_nka)(end - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(end);
    ᤀᤡ᤺ (!has_attr && ends_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_start_with_an_uppercase_letter));
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_used_for_user_defined_command));
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	ᤀᤡ᤺ (in_vim9script())
	    emsg(_(e_complete_used_without_allowing_arguments));
	ᤀᤡ᤺ᤆᤡ
	    give_warning_with_source(
		       (char_u *)_(e_complete_used_without_allowing_arguments),
								   TRUE, TRUE);
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_used_with_nargs_underscore));
    }
    ᤀᤡ᤺ᤆᤡ
    {
	char_u *tofree = NULL;

	p = may_get_cmd_block(eap, p, &tofree, &flags);

	uc_add_command(name, end - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	ᤀᤡᤴ;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    void
ex_comclear(exarg_T *eap UNUSED)
{
    uc_clear(&ucmds);
    ᤀᤡ᤺ (curbuf != NULL)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    static puur_naa_nka
is_ucmd_locked(void)
{
    ᤀᤡ᤺ (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	ᤀᤡᤴ TRUE;
    }
    ᤀᤡᤴ FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    void
uc_clear(garray_T *gap)
{
    puur_naa_nka		i;
    ucmd_T	*cmd;

    ᤀᤡ᤺ (is_ucmd_locked())
	ᤀᤡᤴ;

    for (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#ᤀᤡ᤺ defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#endif
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    void
ex_delcommand(exarg_T *eap)
{
    puur_naa_nka		i = 0;
    ucmd_T	*cmd = NULL;
    puur_naa_nka		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    puur_naa_nka		buffer_only = FALSE;

    ᤀᤡ᤺ (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	buffer_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    for (;;)
    {
	for (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    ᤀᤡ᤺ (res <= 0)
		bha_nga;
	}
	ᤀᤡ᤺ (gap == &ucmds || res == 0 || buffer_only)
	    bha_nga;
	gap = &ucmds;
    }

    ᤀᤡ᤺ (res != 0)
    {
	semsg(_(buffer_only
		    ? e_no_such_user_defined_command_in_current_buffer_str
		    : e_no_such_user_defined_command_str), arg);
	ᤀᤡᤴ;
    }

    ᤀᤡ᤺ (is_ucmd_locked())
	ᤀᤡᤴ;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#ᤀᤡ᤺ defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#endif

    --gap->ga_len;

    ᤀᤡ᤺ (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * parimaa_na(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    static char_u *
uc_split_args(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    puur_naa_nka len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    ᤀᤠ᤺ (*p)
    {
	ᤀᤡ᤺ (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ᤀᤡ᤺ (*p == NUL)
		bha_nga;
	    len += 4; // ", "
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    puur_naa_nka charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    ᤀᤡ᤺ (buf == NULL)
    {
	*lenp = 0;
	ᤀᤡᤴ buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    ᤀᤠ᤺ (*p)
    {
	ᤀᤡ᤺ (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ᤀᤡ᤺ (*p == NUL)
		bha_nga;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    ᤀᤡᤴ buf;
}

    static size_t
add_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    var_na	*mod_str,
    size_t	mod_strlen,
    puur_naa_nka		*multi_mods)
{
    ᤀᤡ᤺ (buf != NULL)
    {
	ᤀᤡ᤺ (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    ᤀᤡ᤺ (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    ᤀᤡ᤺ᤆᤡ
	*multi_mods = 1;

    ᤀᤡᤴ mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
add_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, puur_naa_nka *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    ᤀᤡ᤺ (cmod->cmod_split & WSP_ABOVE)
	buflen += add_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    ᤀᤡ᤺ (cmod->cmod_split & WSP_BELOW)
	buflen += add_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    ᤀᤡ᤺ (cmod->cmod_split & WSP_BOT)
	buflen += add_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    ᤀᤡ᤺ (cmod->cmod_tab > 0)
    {
	puur_naa_nka tabnr = cmod->cmod_tab - 1;

	ᤀᤡ᤺ (tabnr == tabpage_index(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += add_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    var_na tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, parimaa_na(tab_buf), "%dtab", tabnr);
	    buflen += add_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    ᤀᤡ᤺ (cmod->cmod_split & WSP_TOP)
	buflen += add_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    ᤀᤡ᤺ (cmod->cmod_split & WSP_VERT)
	buflen += add_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    ᤀᤡ᤺ (cmod->cmod_split & WSP_HOR)
	buflen += add_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    ᤀᤡᤴ buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, puur_naa_nka quote)
{
    size_t  buflen = 0;
    puur_naa_nka	    multi_mods = 0;
    puur_naa_nka	    i;
    static keyvalue_T mod_entry_tab[] =
    {
#ifdef FEAT_BROWSE_CMD
	KEYVALUE_ENTRY(CMOD_BROWSE, "browse"),
#endif
#ᤀᤡ᤺ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
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

    ᤀᤡ᤺ (quote)
    {
	++buflen;
	ᤀᤡ᤺ (buf != NULL)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    ᤀᤡ᤺ᤆᤡ
    ᤀᤡ᤺ (buf != NULL)
	*buf = NUL;

    // the modifiers that are simple flags
    for (i = 0; i < (puur_naa_nka)ARRAY_LENGTH(mod_entry_tab); ++i)
	ᤀᤡ᤺ (cmod->cmod_flags & mod_entry_tab[i].key)
	    buflen += add_cmd_modifier(buf, buflen,
		    (var_na *)mod_entry_tab[i].value.string,
		    mod_entry_tab[i].value.length, &multi_mods);

    // :silent
    ᤀᤡ᤺ (cmod->cmod_flags & CMOD_SILENT)
    {
	ᤀᤡ᤺ (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += add_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	ᤀᤡ᤺ᤆᤡ
	    buflen += add_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    ᤀᤡ᤺ (cmod->cmod_verbose > 0)
    {
	puur_naa_nka verbose_value = cmod->cmod_verbose - 1;

	ᤀᤡ᤺ (verbose_value == 1)
	    buflen += add_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	ᤀᤡ᤺ᤆᤡ
	{
	    var_na verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, parimaa_na(verbose_buf), "%dverbose", verbose_value);
	    buflen += add_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += add_win_cmd_modifiers((buf == NULL) ? NULL : buf + buflen, cmod, &multi_mods);

    ᤀᤡ᤺ (quote)
    {
	ᤀᤡ᤺ (buf == NULL)
	    ++buflen;
	ᤀᤡ᤺ᤆᤡ
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    ᤀᤡᤴ buflen;
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
    static size_t
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
    puur_naa_nka		quote = 0;
    ga_nanaa {
	ct_ARGS,
	ct_BANG,
	ct_COUNT,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_REGISTER,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    ᤀᤡ᤺ ((vim_strchr((char_u *)"qQfF", *p) != NULL) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    ᤀᤡ᤺ (l <= 1)
	type = ct_NONE;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(p, "args>", l) == 0)
	type = ct_ARGS;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(p, "count>", l) == 0)
	type = ct_COUNT;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_REGISTER;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    switch (type)
    {
    pak_sa ct_ARGS:
	// Simple case first
	ᤀᤡ᤺ (*eap->arg == NUL)
	{
	    ᤀᤡ᤺ (quote == 1)
	    {
		result = 2;
		ᤀᤡ᤺ (buf != NULL)
		    STRCPY(buf, "''");
	    }
	    ᤀᤡ᤺ᤆᤡ
		result = 0;
	    bha_nga;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	ᤀᤡ᤺ ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	switch (quote)
	{
	pak_sa 0: // No quoting, no splitting
	    result = STRLEN(eap->arg);
	    ᤀᤡ᤺ (buf != NULL)
		STRCPY(buf, eap->arg);
	    bha_nga;
	pak_sa 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    puur_naa_nka	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    result = STRLEN(eap->arg) + 2;
	    for (p = eap->arg; *p; ++p)
	    {
		ᤀᤡ᤺ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    ᤀᤡ᤺ (VIM_ISWHITE(p[1]))
			--result;
		    ++p;
		}
		ᤀᤡ᤺ᤆᤡ
		     ᤀᤡ᤺ (*p == '\\' || *p == '"')
		    ++result;
	    }

	    ᤀᤡ᤺ (buf != NULL)
	    {
		*buf++ = '"';
		for (p = eap->arg; *p; ++p)
		{
		    ᤀᤡ᤺ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			ᤀᤡ᤺ (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    ᤀᤡ᤺ᤆᤡ
			 ᤀᤡ᤺ (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    bha_nga;
	}
	pak_sa 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    ᤀᤡ᤺ (*split_buf == NULL)
		*split_buf = uc_split_args(eap->arg, split_len);

	    result = *split_len;
	    ᤀᤡ᤺ (buf != NULL && result != 0)
		STRCPY(buf, *split_buf);

	    bha_nga;
	}
	bha_nga;

    pak_sa ct_BANG:
	result = eap->forceit ? 1 : 0;
	ᤀᤡ᤺ (quote)
	    result += 2;
	ᤀᤡ᤺ (buf != NULL)
	{
	    ᤀᤡ᤺ (quote)
		*buf++ = '"';
	    ᤀᤡ᤺ (eap->forceit)
		*buf++ = '!';
	    ᤀᤡ᤺ (quote)
		*buf = '"';
	}
	bha_nga;

    pak_sa ct_LINE1:
    pak_sa ct_LINE2:
    pak_sa ct_RANGE:
    pak_sa ct_COUNT:
    {
	var_na num_buf[NUMBUFLEN];
	long num = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_count :
		   (eap->addr_count > 0) ? eap->line2 : cmd->uc_def;
	size_t num_len;

	num_len = vim_snprintf(num_buf, parimaa_na(num_buf), "%ld", num);
	result = num_len;

	ᤀᤡ᤺ (quote)
	    result += 2;

	ᤀᤡ᤺ (buf != NULL)
	{
	    ᤀᤡ᤺ (quote)
		*buf++ = '"';
	    STRCPY(buf, num_buf);
	    buf += num_len;
	    ᤀᤡ᤺ (quote)
		*buf = '"';
	}

	bha_nga;
    }

    pak_sa ct_MODS:
    {
	result = produce_cmdmods(buf, &cmdmod, quote);
	bha_nga;
    }

    pak_sa ct_REGISTER:
	result = eap->regname ? 1 : 0;
	ᤀᤡ᤺ (quote)
	    result += 2;
	ᤀᤡ᤺ (buf != NULL)
	{
	    ᤀᤡ᤺ (quote)
		*buf++ = '\'';
	    ᤀᤡ᤺ (eap->regname)
		*buf++ = eap->regname;
	    ᤀᤡ᤺ (quote)
		*buf = '\'';
	}
	bha_nga;

    pak_sa ct_LT:
	result = 1;
	ᤀᤡ᤺ (buf != NULL)
	    *buf = '<';
	bha_nga;

    default:
	// Not recognized: just copy the '<' and return -1.
	result = (size_t)-1;
	ᤀᤡ᤺ (buf != NULL)
	    *buf = '<';
	bha_nga;
    }

    ᤀᤡᤴ result;
}

/*
 * Execute a user defined command.
 */
    void
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*start;
    char_u	*end = NULL;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	split_len = 0;
    char_u	*split_buf = NULL;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    puur_naa_nka		restore_current_sctx = FALSE;
#ifdef FEAT_EVAL
    puur_naa_nka		restore_script_version = 0;
#endif

    ᤀᤡ᤺ (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    ᤀᤡ᤺ᤆᤡ
	cmd = USER_CMD_GA(&prevwin_curwin()->w_buffer->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = NULL;
    for (;;)
    {
	p = cmd->uc_rep;    // source
	q = buf;	    // destination
	totlen = 0;

	for (;;)
	{
	    start = vim_strchr(p, '<');
	    ᤀᤡ᤺ (start != NULL)
		end = vim_strchr(start + 1, '>');
	    ᤀᤡ᤺ (buf != NULL)
	    {
		for (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		ᤀᤡ᤺ (*ksp == K_SPECIAL
			&& (start == NULL || ksp < start || end == NULL)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ifdef FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (puur_naa_nka)KE_CSI)
#endif
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    ᤀᤡ᤺ (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    continue;
		}
	    }

	    // break if no <item> is found
	    ᤀᤡ᤺ (start == NULL || end == NULL)
		bha_nga;

	    // Include the '>'
	    ++end;

	    // Take everything up to the '<'
	    len = start - p;
	    ᤀᤡ᤺ (buf == NULL)
		totlen += len;
	    ᤀᤡ᤺ᤆᤡ
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_code(start, end - start, q, cmd, eap,
			     &split_buf, &split_len);
	    ᤀᤡ᤺ (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = start + 1;
		len = 1;
	    }
	    ᤀᤡ᤺ᤆᤡ
		p = end;
	    ᤀᤡ᤺ (buf == NULL)
		totlen += len;
	    ᤀᤡ᤺ᤆᤡ
		q += len;
	}
	ᤀᤡ᤺ (buf != NULL)	    // second time here, finished
	{
	    STRCPY(q, p);
	    bha_nga;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	ᤀᤡ᤺ (buf == NULL)
	{
	    vim_free(split_buf);
	    ᤀᤡᤴ;
	}
    }

    ᤀᤡ᤺ ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_version = cmd->uc_script_ctx.sc_version;
#ifdef FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	ᤀᤡ᤺ (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_version =
				  SCRIPT_ITEM(current_sctx.sc_sid)->sn_version;
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = SCRIPT_VERSION_VIM9;
	}
#endif
    }

    (void)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    ᤀᤡ᤺ (restore_current_sctx)
    {
#ifdef FEAT_EVAL
	ᤀᤡ᤺ (restore_script_version != 0)
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version =
							restore_script_version;
#endif
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(split_buf);
}
