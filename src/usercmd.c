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

#ଅନ୍ତର୍ଭାଵ "vim.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    long	uc_def;		// The default value for a range/count
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		uc_flags;	// some UC_ flags
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		uc_compl_opt;	// completion options (UCC_ flags)
#ifdef FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#endif
} ucmd_T;

// List of all user commands.
ସ୍ଥିର garray_T ucmds = {0, 0, ଆକାର(ucmd_T), 4, ଖାଲି};

// When non-zero it is not allowed to add or remove user commands
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ucmd_locked = 0;

#define USER_CMD(i) (&((ucmd_T *)(ucmds.ga_data))[i])
#define USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_data))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
ସ୍ଥିର keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ENTRY(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ENTRY(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ENTRY(EXPAND_BEHAVE, "behave"),
#ଯଦି defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_BREAKPOINT, "breakpoint"),
#endif
    KEYVALUE_ENTRY(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ENTRY(EXPAND_COLORS, "color"),
    KEYVALUE_ENTRY(EXPAND_COMMANDS, "command"),
    KEYVALUE_ENTRY(EXPAND_COMPILER, "compiler"),
#ଯଦି defined(FEAT_CSCOPE)
    KEYVALUE_ENTRY(EXPAND_CSCOPE, "cscope"),
#endif
#ଯଦି defined(FEAT_EVAL)
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
#ଯଦି defined(FEAT_KEYMAP)
    KEYVALUE_ENTRY(EXPAND_KEYMAP, "keymap"),
#endif
#ଯଦି defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
#ଯଦି defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_SCRIPTNAMES, "scriptnames"),
#endif
    KEYVALUE_ENTRY(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ENTRY(EXPAND_SHELLCMDLINE, "shellcmdline"),
#ଯଦି defined(FEAT_SIGNS)
    KEYVALUE_ENTRY(EXPAND_SIGN, "sign"),
#endif
    KEYVALUE_ENTRY(EXPAND_OWNSYNTAX, "syntax"),
#ଯଦି defined(FEAT_PROFILE)
    KEYVALUE_ENTRY(EXPAND_SYNTIME, "syntime"),
#endif
    KEYVALUE_ENTRY(EXPAND_TAGS, "tag"),
    KEYVALUE_ENTRY(EXPAND_TAGS_LISTFILES, "tag_listfiles"),
    KEYVALUE_ENTRY(EXPAND_USER, "user"),
    KEYVALUE_ENTRY(EXPAND_USER_VARS, "var")
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    cmd_addr_T ଚାବି;
    ଅକ୍ଷର *fullname;
    size_t fullnamelen;
    ଅକ୍ଷର *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#define ADDRTYPE_ENTRY(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
ସ୍ଥିର addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ENTRY(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ENTRY(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ENTRY(ADDR_ଧାଡ଼ିଗୁଡ଼ିକ, "lines", "line"),
    ADDRTYPE_ENTRY(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ENTRY(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ENTRY(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ENTRY(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ENTRY(ADDR_WINDOWS, "windows", "win")
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp_addr_type(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b);

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*full,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*complp) // completion flags or NULL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - eap->cmd);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j, k, matchlen = 0;
    ucmd_T	*uc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    ପାଇଁ (;;)
    {
	ପାଇଁ (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    ଯେତେବେଳେ (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    ଯଦି (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		ଯଦି (k == len && found && *np != NUL)
		{
		    ଯଦି (gap == &ucmds)
			ଫେରନ୍ତୁ ଖାଲି;
		    amb_local = TRUE;
		}

		ଯଦି (!found || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    ଯଦି (k == len)
			found = TRUE;
		    ଅନ୍ୟଥା
			possible = TRUE;

		    ଯଦି (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    ଅନ୍ୟଥା
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (long)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    ଯଦି (complp != ଖାଲି)
			*complp = uc->uc_compl;
#ifdef FEAT_EVAL
		    ଯଦି (xp != ଖାଲି)
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
		    ଯଦି (k == len && *np == NUL)
		    {
			ଯଦି (full != ଖାଲି)
			    *full = TRUE;
			amb_local = FALSE;
			ଭାଙ୍ଗ;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	ଯଦି (j < gap->ga_len || gap == &ucmds)
	    ଭାଙ୍ଗ;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    ଯଦି (amb_local)
    {
	ଯଦି (xp != ଖାଲି)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    ଯଦି (found || possible)
	ଫେରନ୍ତୁ p + (matchlen - len);
    ଫେରନ୍ତୁ p;
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
    ଯେତେବେଳେ (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	ଯଦି (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    ଯଦି (p == ଖାଲି)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_pattern = arg;
		ଫେରନ୍ତୁ ଖାଲି;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    ଯଦି (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_pattern = p + 1;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_pattern = p + 1;
		ପାଇଁ (char_u *c = p + 1; *c != NUL; ++c)
		    ଯଦି (*c == ',')
			xp->xp_pattern = c + 1;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_pattern = p + 1;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_pattern = p + 1;
	    }
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    ଯଦି (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_pattern = arg;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // And finally comes a normal command
    ଫେରନ୍ତୁ skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
set_context_in_user_cmdarg(
	char_u		*cmd UNUSED,
	char_u		*arg,
	long		argt,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		context,
	expand_T	*xp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit)
{
    char_u	*p;

    ଯଦି (context == EXPAND_NOTHING)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	ଫେରନ୍ତୁ ଖାଲି;
    }

#ifdef FEAT_MENU
    ଯଦି (context == EXPAND_MENUS)
	ଫେରନ୍ତୁ set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif
    ଯଦି (context == EXPAND_COMMANDS)
	ଫେରନ୍ତୁ arg;
    ଯଦି (context == EXPAND_MAPPINGS)
	ଫେରନ୍ତୁ set_context_in_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    ଯଦି (!(argt & EX_ARGSPACE))
    {
	p = arg;
	ଯେତେବେଳେ (*p)
	{
	    ଯଦି (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    ଅନ୍ୟଥା ଯଦି (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_pattern = arg;
    xp->xp_context = context;

    ଫେରନ୍ତୁ ଖାଲି;
}

    char_u *
expand_user_command_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ get_user_commands(ଖାଲି, idx - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CMD_SIZE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_buffer;

    ଯଦି (idx < buf->b_ucmds.ga_len)
	ଫେରନ୍ତୁ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    ଯଦି (idx < ucmds.ga_len)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	ପାଇଁ (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    ଯଦି (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		ଫେରନ୍ତୁ (char_u *)"";
	ଫେରନ୍ତୁ name;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

#ifdef FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdidx)
{
    ଯଦି (cmdidx == CMD_USER && idx < ucmds.ga_len)
	ଫେରନ୍ତୁ USER_CMD(idx)->uc_name;
    ଯଦି (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_buffer;

	ଯଦି (idx < buf->b_ucmds.ga_len)
	    ଫେରନ୍ତୁ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
user_command_info(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_buffer->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    ଯଦି (idx >= gap->ga_len)
	ଫେରନ୍ତୁ FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_add_string(d, "name", uc->uc_name);
    dict_add_string(d, "kind", (char_u *)"user");
    dict_add_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_add_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_to_str(uc->uc_compl, uc->uc_compl_arg);
    dict_add_string(d, "complete", compl == ଖାଲି ? (char_u *)"" : compl);
    vim_free(compl);
    dict_add_string(d, "definition", uc->uc_rep);
    dict_add_ସଂଖ୍ୟା(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_add_ସଂଖ୍ୟା(d, "lnum", uc->uc_script_ctx.sc_lnum);
    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx < 0 || idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(addr_type_complete_tab))
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ସ୍ଥିର ଅକ୍ଷର *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    ଯଦି (idx < 0 || idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(user_cmd_flags))
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ସ୍ଥିର ଅକ୍ଷର *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    ଯଦି (idx < 0 || idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(user_cmd_nargs))
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx < 0 || idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(command_complete_tab))
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ command_complete_tab[idx].ମୂଲ୍ୟ.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
ସ୍ଥିର ଅକ୍ଷର *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx < 0 || idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(user_cmd_completeopt_tab))
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    ସ୍ଥିର keyvalue_T *
get_commandtype(ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(command_complete_tab); ++i)
	ଯଦି (command_complete_tab[i].ଚାବି == expand)
	    ଫେରନ୍ତୁ &command_complete_tab[i];

    ଫେରନ୍ତୁ ଖାଲି;
}

#ifdef FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_to_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    ଯଦି (kv == ଖାଲି || kv->ମୂଲ୍ୟ.string == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    cmd_compl = kv->ମୂଲ୍ୟ.string;
    ଯଦି (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*buffer;

	buffer = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	ଯଦି (buffer == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	sprintf((ଅକ୍ଷର *)buffer, "%s,%s", cmd_compl, compl_arg);
	ଫେରନ୍ତୁ buffer;
    }

    ଫେରନ୍ତୁ vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdcomplete_str_to_type(char_u *complete_str)
{
    keyvalue_T target;
    keyvalue_T *entry;
    ସ୍ଥିର keyvalue_T *last_entry = ଖାଲି;	// cached result

    ଯଦି (STRNCMP(complete_str, "custom,", 7) == 0)
	ଫେରନ୍ତୁ EXPAND_USER_DEFINED;
    ଯଦି (STRNCMP(complete_str, "customlist,", 11) == 0)
	ଫେରନ୍ତୁ EXPAND_USER_LIST;

    target.ଚାବି = 0;
    target.ମୂଲ୍ୟ.string = complete_str;
    target.ମୂଲ୍ୟ.length = 0;			// not used, see cmp_keyvalue_value()

    ଯଦି (last_entry != ଖାଲି && cmp_keyvalue_ମୂଲ୍ୟ(&target, last_entry) == 0)
	entry = last_entry;
    ଅନ୍ୟଥା
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    ଆକାର(command_complete_tab[0]),
	    cmp_keyvalue_ମୂଲ୍ୟ);
	ଯଦି (entry == ଖାଲି)
	    ଫେରନ୍ତୁ EXPAND_NOTHING;

	last_entry = entry;
    }

    ଫେରନ୍ତୁ entry->ଚାବି;
}
#endif

/*
 * List user commands starting with "name[name_len]".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
uc_list(char_u *name, size_t name_len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found = FALSE;
    ucmd_T	*cmd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		over;
    long	a;
    garray_T	*gap;
    keyvalue_T	*entry;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    ପାଇଁ (;;)
    {
	ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (long)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    ଯଦି (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || message_filtered(cmd->uc_name))
		ଜାରି;

	    // Put out the title first time
	    ଯଦି (!found)
		ସନ୍ଦେଶ_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    found = TRUE;
	    ସନ୍ଦେଶ_putchar('\n');
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;

	    // Special cases
	    len = 4;
	    ଯଦି (a & EX_BANG)
	    {
		ସନ୍ଦେଶ_putchar('!');
		--len;
	    }
	    ଯଦି (a & EX_REGSTR)
	    {
		ସନ୍ଦେଶ_putchar('"');
		--len;
	    }
	    ଯଦି (gap != &ucmds)
	    {
		ସନ୍ଦେଶ_putchar('b');
		--len;
	    }
	    ଯଦି (a & EX_TRLBAR)
	    {
		ସନ୍ଦେଶ_putchar('|');
		--len;
	    }
	    ଯଦି (len != 0)
		ସନ୍ଦେଶ_puts(&"    "[4 - len]);

	    ସନ୍ଦେଶ_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cmd->uc_namelen + 4;

	    ଯଦି (len < 21)
	    {
		// Field padding spaces   12345678901234567
		ସ୍ଥିର ଅକ୍ଷର spaces[18] = "                 ";
		ସନ୍ଦେଶ_puts(&spaces[len - 4]);
		len = 21;
	    }
	    ସନ୍ଦେଶ_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    ବାଛ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		ସ୍ଥିତି 0:				IObuff[len++] = '0'; ଭାଙ୍ଗ;
		ସ୍ଥିତି (EX_EXTRA):		IObuff[len++] = '*'; ଭାଙ୍ଗ;
		ସ୍ଥିତି (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; ଭାଙ୍ଗ;
		ସ୍ଥିତି (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; ଭାଙ୍ଗ;
		ସ୍ଥିତି (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; ଭାଙ୍ଗ;
		ସ୍ଥିତି (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; ଭାଙ୍ଗ;
	    }

	    କର
	    {
		IObuff[len++] = ' ';
	    } ଯେତେବେଳେ (len < 5 - over);

	    // Address / Range
	    ଯଦି (a & (EX_RANGE|EX_ଗଣନା))
	    {
		ଯଦି (a & EX_ଗଣନା)
		{
		    // -count=N
		    len += vim_snprintf((ଅକ୍ଷର *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		ଅନ୍ୟଥା ଯଦି (a & EX_DFLALL)
		    IObuff[len++] = '%';
		ଅନ୍ୟଥା ଯଦି (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((ଅକ୍ଷର *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		ଅନ୍ୟଥା
		    IObuff[len++] = '.';
	    }

	    କର
	    {
		IObuff[len++] = ' ';
	    } ଯେତେବେଳେ (len < 8 - over);

	    // Address Type
	    ପାଇଁ (j = 0; j < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(addr_type_complete_tab); ++j)
		ଯଦି (addr_type_complete_tab[j].ଚାବି != ADDR_ଧାଡ଼ିଗୁଡ଼ିକ
			&& addr_type_complete_tab[j].ଚାବି == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)addr_type_complete_tab[j].shortnamelen;
		    ଭାଙ୍ଗ;
		}

	    କର
	    {
		IObuff[len++] = ' ';
	    } ଯେତେବେଳେ (len < 13 - over);

	    // Completion
	    entry = get_commandtype(cmd->uc_compl);
	    ଯଦି (entry != ଖାଲି)
	    {
		STRCPY(IObuff + len, entry->ମୂଲ୍ୟ.string);
		len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)entry->ମୂଲ୍ୟ.length;
#ifdef FEAT_EVAL
		ଯଦି (p_verbose > 0 && cmd->uc_compl_arg != ଖାଲି)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    ଯଦି (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)uc_compl_arglen;
		    }
		}
		ଯଦି (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((ଅକ୍ଷର *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#endif
	    }

	    କର
	    {
		IObuff[len++] = ' ';
	    } ଯେତେବେଳେ (len < 25 - over);

	    IObuff[len] = NUL;
	    ସନ୍ଦେଶ_outtrans(IObuff);

	    ସନ୍ଦେଶ_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ifdef FEAT_EVAL
	    ଯଦି (p_verbose > 0)
		last_set_ସନ୍ଦେଶ(cmd->uc_script_ctx);
#endif
	    out_flush();
	    ui_breakcheck();
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
	}
	ଯଦି (gap == &ucmds || i < gap->ga_len)
	    ଭାଙ୍ଗ;
	gap = &ucmds;
    }

    ଯଦି (!found)
	ସନ୍ଦେଶ(_("No user-defined commands found"));

    --ucmd_locked;
}

    ଅକ୍ଷର *
uc_fun_cmd(ଶୂନ୍ୟ)
{
    ସ୍ଥିର char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    ଫେରନ୍ତୁ (ଅକ୍ଷର *)IObuff;
}

/*
 * Parse address type argument
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_addr_type_arg(
    char_u	*ମୂଲ୍ୟ,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T target;
    addrtype_T *entry;
    ସ୍ଥିର addrtype_T *last_entry;	// cached result

    target.ଚାବି = 0;
    target.fullname = (ଅକ୍ଷର *)ମୂଲ୍ୟ;
    target.fullnamelen = vallen;

    ଯଦି (last_entry != ଖାଲି && cmp_addr_type(&target, last_entry) == 0)
	entry = last_entry;
    ଅନ୍ୟଥା
    {
	entry = (addrtype_T *)bsearch(&target,
	    &addr_type_complete_tab,
	    ARRAY_LENGTH(addr_type_complete_tab),
	    ଆକାର(addr_type_complete_tab[0]),
	    cmp_addr_type);
	ଯଦି (entry == ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	    char_u	*err = ମୂଲ୍ୟ;

	    ପାଇଁ (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_address_type_ମୂଲ୍ୟ_str), err);
	    ଫେରନ୍ତୁ FAIL;
	}

	last_entry = entry;
    }

    *addr_type_arg = entry->ଚାବି;

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmp_addr_type(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    ଫେରନ୍ତୁ STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_compl_arg(
    char_u	*ମୂଲ୍ୟ,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vallen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*complp,
    long	*argt,
    char_u	**compl_arg UNUSED)
{
    char_u	*arg = ଖାଲି;
#ଯଦି defined(FEAT_EVAL)
    size_t	arglen = 0;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		valend = vallen;
    keyvalue_T	target;
    keyvalue_T	*entry;
    ସ୍ଥିର keyvalue_T	*last_entry = ଖାଲି;	    // cached result

    // Look for any argument part - which is the part after any ','
    ପାଇଁ (i = 0; i < vallen; ++i)
    {
	ଯଦି (ମୂଲ୍ୟ[i] == ',')
	{
	    arg = &ମୂଲ୍ୟ[i + 1];
#ଯଦି defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#endif
	    valend = i;
	    ଭାଙ୍ଗ;
	}
    }

    target.ଚାବି = 0;
    target.ମୂଲ୍ୟ.string = ମୂଲ୍ୟ;
    target.ମୂଲ୍ୟ.length = valend;

    ଯଦି (last_entry != ଖାଲି && cmp_keyvalue_ମୂଲ୍ୟ_n(&target, last_entry) == 0)
	entry = last_entry;
    ଅନ୍ୟଥା
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    ଆକାର(command_complete_tab[0]),
	    cmp_keyvalue_ମୂଲ୍ୟ_n);
	ଯଦି (entry == ଖାଲି)
	{
	    semsg(_(e_invalid_complete_ମୂଲ୍ୟ_str), ମୂଲ୍ୟ);
	    ଫେରନ୍ତୁ FAIL;
	}

	last_entry = entry;
    }

    *complp = entry->ଚାବି;
    ଯଦି (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    ଅନ୍ୟଥା ଯଦି (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    ଯଦି (
#ଯଦି defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#endif
								arg != ଖାଲି)
    {
	emsg(_(e_completion_argument_only_allowed_for_custom_completion));
	ଫେରନ୍ତୁ FAIL;
    }

#ଯଦି defined(FEAT_EVAL)
    ଯଦି ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == ଖାଲି)
    {
	emsg(_(e_custom_completion_requires_function_argument));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (arg != ଖାଲି)
	*compl_arg = vim_strnsave(arg, arglen);
#endif

    ଫେରନ୍ତୁ OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_completeopt_arg(char_u *ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vallen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *compl_opt)
{
    char_u  *p = ମୂଲ୍ୟ;
    char_u  *end = ମୂଲ୍ୟ + vallen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags = 0;

    ଯଦି (vallen == 0)
    {
	semsg(_(e_argument_required_for_str), "-completeopt");
	ଫେରନ୍ତୁ FAIL;
    }

    ଯେତେବେଳେ (p < end)
    {
	char_u	*comma;
	size_t	itemlen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	matched = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	comma = vim_strchr(p, ',');
	ଯଦି (comma == ଖାଲି || comma > end)
	    itemlen = (size_t)(end - p);
	ଅନ୍ୟଥା
	    itemlen = (size_t)(comma - p);

	ଯଦି (itemlen == 0)
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "completeopt");
	    ଫେରନ୍ତୁ FAIL;
	}

	ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(user_cmd_completeopt_tab); ++i)
	{
	    ଅକ୍ଷର *name = user_cmd_completeopt_tab[i];

	    ଯଦି (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		ଯଦି (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (!matched)
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "completeopt");
	    ଫେରନ୍ତୁ FAIL;
	}

	p += itemlen;
	ଯଦି (p < end && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    ଫେରନ୍ତୁ OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
uc_scan_attr(
    char_u	*attr,
    size_t	len,
    long	*argt,
    long	*def,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*complp,
    char_u	**compl_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    ଯଦି (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	ଫେରନ୍ତୁ FAIL;
    }

    // First, try the simple attributes (no arguments)
    ଯଦି (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_BUFFER;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
	char_u	*val = ଖାଲି;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len; ++i)
	{
	    ଯଦି (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		ଭାଙ୍ଗ;
	    }
	}

	ଯଦି (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    ଯଦି (vallen == 1)
	    {
		ଯଦି (*val == '0')
		    // Do nothing - this is the default
		    ;
		ଅନ୍ୟଥା ଯଦି (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		ଅନ୍ୟଥା ଯଦି (*val == '*')
		    *argt |= EX_EXTRA;
		ଅନ୍ୟଥା ଯଦି (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		ଅନ୍ୟଥା ଯଦି (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		ଅନ୍ୟଥା ଯଦି (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		ଅନ୍ୟଥା
		    ଯାଅ wrong_nargs;
	    }
	    ଅନ୍ୟଥା
	    {
wrong_nargs:
		emsg(_(e_invalid_ସଂଖ୍ୟା_of_arguments));
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ଯଦି (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    ଅନ୍ୟଥା ଯଦି (val != ଖାଲି)
	    {
		p = val;
		ଯଦି (*def >= 0)
		{
two_ଗଣନା:
		    emsg(_(e_ଗଣନା_cannot_be_specified_twice));
		    ଫେରନ୍ତୁ FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		ଯଦି (p != val + vallen || vallen == 0)
		{
invalid_ଗଣନା:
		    emsg(_(e_invalid_default_ମୂଲ୍ୟ_for_ଗଣନା));
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    ଯଦି (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_ଧାଡ଼ିଗୁଡ଼ିକ;
	}
	ଅନ୍ୟଥା ଯଦି (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_ଗଣନା | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    ଯଦି (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    ଯଦି (val != ଖାଲି)
	    {
		p = val;
		ଯଦି (*def >= 0)
		    ଯାଅ two_ଗଣନା;

		*def = getdigits(&p);

		ଯଦି (p != val + vallen)
		    ଯାଅ invalid_ଗଣନା;
	    }

	    ଯଦି (*def < 0)
		*def = 0;
	}
	ଅନ୍ୟଥା ଯଦି (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    ଯଦି (val == ଖାଲି)
	    {
		semsg(_(e_argument_required_for_str), "-complete");
		ଫେରନ୍ତୁ FAIL;
	    }

	    ଯଦି (parse_compl_arg(val, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vallen, complp, argt, compl_arg)
								      == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    ଯଦି (val == ଖାଲି)
	    {
		semsg(_(e_argument_required_for_str), "-completeopt");
		ଫେରନ୍ତୁ FAIL;
	    }

	    ଯଦି (parse_completeopt_arg(val, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vallen, compl_opt) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ଯଦି (val == ଖାଲି)
	    {
		semsg(_(e_argument_required_for_str), "-addr");
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (parse_addr_type_arg(val, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vallen, addr_type_arg) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (*addr_type_arg != ADDR_ଧାଡ଼ିଗୁଡ଼ିକ)
		*argt |= EX_ZEROR;
	}
	ଅନ୍ୟଥା
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
uc_add_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    long	argt,
    long	def,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compl,
    char_u	*compl_arg UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compl_opt,
    cmd_addr_T	addr_type,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force)
{
    ucmd_T	*cmd = ଖାଲି;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmp = 1;
    char_u	*rep_buf = ଖାଲି;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, ଖାଲି);
    ଯଦି (rep_buf == ଖାଲି)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	ଯଦି (rep_buf == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }

    // get address of growarray: global or in curbuf
    ଯଦି (flags & UC_BUFFER)
    {
	gap = &curbuf->b_ucmds;
	ଯଦି (gap->ga_itemsize == 0)
	    ga_init2(gap, ଆକାର(ucmd_T), 4);
    }
    ଅନ୍ୟଥା
	gap = &ucmds;

    // Search for the command in the already defined commands.
    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	ଯଦି (cmp == 0)
	{
	    ଯଦି (name_len < cmd->uc_namelen)
		cmp = -1;
	    ଅନ୍ୟଥା ଯଦି (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	ଯଦି (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    ଯଦି (!force
#ifdef FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#endif
		    )
	    {
		semsg(_(e_command_already_exists_add_bang_to_replace_it_str),
									 name);
		ଯାଅ fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#ଯଦି defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#endif
	    ଭାଙ୍ଗ;
	}

	// Stop as soon as we pass the name to add
	ଯଦି (cmp < 0)
	    ଭାଙ୍ଗ;
    }

    // Extend the array unless we're replacing an existing command
    ଯଦି (cmp != 0)
    {
	ଯଦି (ga_grow(gap, 1) == FAIL)
	    ଯାଅ fail;
	ଯଦି ((p = vim_strnsave(name, name_len)) == ଖାଲି)
	    ଯାଅ fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * ଆକାର(ucmd_T));

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
    ଯଦି (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ifdef FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#endif
    cmd->uc_addr_type = addr_type;

    ଫେରନ୍ତୁ OK;

fail:
    vim_free(rep_buf);
#ଯଦି defined(FEAT_EVAL)
    vim_free(compl_arg);
#endif
    ଫେରନ୍ତୁ FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_block(exarg_T *eap, char_u *p, char_u **tofree, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flags)
{
    char_u *retp = p;
    char_u *block = find_cmd_block_start(p);

    ଯଦି (block != ଖାଲି && eap->ea_getline != ଖାଲି)
    {
	garray_T    ga;
	char_u	    *ଧାଡ଼ି = ଖାଲି;

	ga_init2(&ga, ଆକାର(char_u *), 10);
	ଯଦି (ga_copy_string(&ga, p) == FAIL)
	    ଫେରନ୍ତୁ retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	ଯଦି (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    ପାଇଁ (;;)
	    {
		vim_free(ଧାଡ଼ି);
		ଯଦି ((ଧାଡ଼ି = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == ଖାଲି)
		{
		    emsg(_(e_missing_rcurly));
		    ଭାଙ୍ଗ;
		}
		ଯଦି (ga_copy_string(&ga, ଧାଡ଼ି) == FAIL)
		    ଭାଙ୍ଗ;
		ଯଦି (*skipwhite(ଧାଡ଼ି) == '}')
		    ଭାଙ୍ଗ;
	    }
	vim_free(ଧାଡ଼ି);
	retp = *tofree = ga_concat_strings(&ga, "\n");
	ଯଦି (retp == ଖାଲି)
	    retp = p;
	ga_clear_strings(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	ଯଦି (block == p)
	    *flags |= UC_VIM9;
    }
    ଫେରନ୍ତୁ retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    ଶୂନ୍ୟ
skip_cmd_block(exarg_T *eap)
{
    char_u	*tofree = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;

    (ଶୂନ୍ୟ)may_get_cmd_block(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    ଶୂନ୍ୟ
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*end;
    char_u	*p;
    long	argt = 0;
    long	def = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compl = EXPAND_NOTHING;
    char_u	*compl_arg = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_attr = (eap->arg[0] == '-');
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name_len;

    ଯଦି (eap->skip)
    {
	skip_cmd_block(eap);
	ଫେରନ୍ତୁ;
    }

    p = eap->arg;

    // Check for attributes
    ଯେତେବେଳେ (*p == '-')
    {
	++p;
	end = skiptowhite(p);
	ଯଦି (uc_scan_attr(p, end - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    ଯାଅ theend;
	p = skipwhite(end);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    ଯଦି (ASCII_ISALPHA(*p))
	ଯେତେବେଳେ (ASCII_ISALNUM(*p))
	    ++p;
    ଯଦି (!ends_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	ଯାଅ theend;
    }
    end = p;
    name_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(end);
    ଯଦି (!has_attr && ends_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    ଅନ୍ୟଥା ଯଦି (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_start_with_an_uppercase_letter));
    }
    ଅନ୍ୟଥା ଯଦି ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_used_for_user_defined_command));
    }
    ଅନ୍ୟଥା ଯଦି (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	ଯଦି (in_vim9script())
	    emsg(_(e_complete_used_without_allowing_arguments));
	ଅନ୍ୟଥା
	    give_warning_with_source(
		       (char_u *)_(e_complete_used_without_allowing_arguments),
								   TRUE, TRUE);
    }
    ଅନ୍ୟଥା ଯଦି ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_used_with_nargs_underscore));
    }
    ଅନ୍ୟଥା
    {
	char_u *tofree = ଖାଲି;

	p = may_get_cmd_block(eap, p, &tofree, &flags);

	uc_add_command(name, end - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	ଫେରନ୍ତୁ;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    ଶୂନ୍ୟ
ex_comclear(exarg_T *eap UNUSED)
{
    uc_clear(&ucmds);
    ଯଦି (curbuf != ଖାଲି)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_ucmd_locked(ଶୂନ୍ୟ)
{
    ଯଦି (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    ଶୂନ୍ୟ
uc_clear(garray_T *gap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ucmd_T	*cmd;

    ଯଦି (is_ucmd_locked())
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#ଯଦି defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#endif
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    ଶୂନ୍ୟ
ex_delcommand(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;
    ucmd_T	*cmd = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buffer_only = FALSE;

    ଯଦି (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	buffer_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    ପାଇଁ (;;)
    {
	ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    ଯଦି (res <= 0)
		ଭାଙ୍ଗ;
	}
	ଯଦି (gap == &ucmds || res == 0 || buffer_only)
	    ଭାଙ୍ଗ;
	gap = &ucmds;
    }

    ଯଦି (res != 0)
    {
	semsg(_(buffer_only
		    ? e_no_such_user_defined_command_in_current_buffer_str
		    : e_no_such_user_defined_command_str), arg);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (is_ucmd_locked())
	ଫେରନ୍ତୁ;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#ଯଦି defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#endif

    --gap->ga_len;

    ଯଦି (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * ଆକାର(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    ସ୍ଥିର char_u *
uc_split_args(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    ଯେତେବେଳେ (*p)
    {
	ଯଦି (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	ଅନ୍ୟଥା ଯଦି (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	ଅନ୍ୟଥା ଯଦି (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ଯଦି (*p == NUL)
		ଭାଙ୍ଗ;
	    len += 4; // ", "
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    ଯଦି (buf == ଖାଲି)
    {
	*lenp = 0;
	ଫେରନ୍ତୁ buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    ଯେତେବେଳେ (*p)
    {
	ଯଦି (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	ଅନ୍ୟଥା ଯଦି (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	ଅନ୍ୟଥା ଯଦି (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ଯଦି (*p == NUL)
		ଭାଙ୍ଗ;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	ଅନ୍ୟଥା
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    ଫେରନ୍ତୁ buf;
}

    ସ୍ଥିର size_t
add_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    ଅକ୍ଷର	*mod_str,
    size_t	mod_strlen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*multi_mods)
{
    ଯଦି (buf != ଖାଲି)
    {
	ଯଦି (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    ଯଦି (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    ଅନ୍ୟଥା
	*multi_mods = 1;

    ଫେରନ୍ତୁ mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
add_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    ଯଦି (cmod->cmod_split & WSP_ABOVE)
	buflen += add_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    ଯଦି (cmod->cmod_split & WSP_BELOW)
	buflen += add_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    ଯଦି (cmod->cmod_split & WSP_BOT)
	buflen += add_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    ଯଦି (cmod->cmod_tab > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା tabnr = cmod->cmod_tab - 1;

	ଯଦି (tabnr == tabpage_index(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += add_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	ଅନ୍ୟଥା
	{
	    ଅକ୍ଷର tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, ଆକାର(tab_buf), "%dtab", tabnr);
	    buflen += add_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    ଯଦି (cmod->cmod_split & WSP_ଶୀର୍ଷ)
	buflen += add_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    ଯଦି (cmod->cmod_split & WSP_VERT)
	buflen += add_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    ଯଦି (cmod->cmod_split & WSP_HOR)
	buflen += add_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    ଫେରନ୍ତୁ buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, ପୂର୍ଣ୍ଣସଂଖ୍ୟା quote)
{
    size_t  buflen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    multi_mods = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ସ୍ଥିର keyvalue_T mod_entry_tab[] =
    {
#ifdef FEAT_BROWSE_CMD
	KEYVALUE_ENTRY(CMOD_BROWSE, "browse"),
#endif
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
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

    ଯଦି (quote)
    {
	++buflen;
	ଯଦି (buf != ଖାଲି)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    ଅନ୍ୟଥା
    ଯଦି (buf != ଖାଲି)
	*buf = NUL;

    // the modifiers that are simple flags
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(mod_entry_tab); ++i)
	ଯଦି (cmod->cmod_flags & mod_entry_tab[i].ଚାବି)
	    buflen += add_cmd_modifier(buf, buflen,
		    (ଅକ୍ଷର *)mod_entry_tab[i].ମୂଲ୍ୟ.string,
		    mod_entry_tab[i].ମୂଲ୍ୟ.length, &multi_mods);

    // :silent
    ଯଦି (cmod->cmod_flags & CMOD_SILENT)
    {
	ଯଦି (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += add_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	ଅନ୍ୟଥା
	    buflen += add_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    ଯଦି (cmod->cmod_verbose > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose_ମୂଲ୍ୟ = cmod->cmod_verbose - 1;

	ଯଦି (verbose_ମୂଲ୍ୟ == 1)
	    buflen += add_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	ଅନ୍ୟଥା
	{
	    ଅକ୍ଷର verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, ଆକାର(verbose_buf), "%dverbose", verbose_ମୂଲ୍ୟ);
	    buflen += add_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += add_win_cmd_modifiers((buf == ଖାଲି) ? ଖାଲି : buf + buflen, cmod, &multi_mods);

    ଯଦି (quote)
    {
	ଯଦି (buf == ଖାଲି)
	    ++buflen;
	ଅନ୍ୟଥା
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    ଫେରନ୍ତୁ buflen;
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
    ସ୍ଥିର size_t
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quote = 0;
    ଗଣନା {
	ct_ARGS,
	ct_BANG,
	ct_ଗଣନା,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_REGISTER,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    ଯଦି ((vim_strchr((char_u *)"qQfF", *p) != ଖାଲି) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    ଯଦି (l <= 1)
	type = ct_NONE;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(p, "args>", l) == 0)
	type = ct_ARGS;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(p, "count>", l) == 0)
	type = ct_ଗଣନା;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_REGISTER;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    ବାଛ (type)
    {
    ସ୍ଥିତି ct_ARGS:
	// Simple case first
	ଯଦି (*eap->arg == NUL)
	{
	    ଯଦି (quote == 1)
	    {
		result = 2;
		ଯଦି (buf != ଖାଲି)
		    STRCPY(buf, "''");
	    }
	    ଅନ୍ୟଥା
		result = 0;
	    ଭାଙ୍ଗ;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	ଯଦି ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	ବାଛ (quote)
	{
	ସ୍ଥିତି 0: // No quoting, no splitting
	    result = STRLEN(eap->arg);
	    ଯଦି (buf != ଖାଲି)
		STRCPY(buf, eap->arg);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    result = STRLEN(eap->arg) + 2;
	    ପାଇଁ (p = eap->arg; *p; ++p)
	    {
		ଯଦି (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		ଅନ୍ୟଥା ଯଦି (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    ଯଦି (VIM_ISWHITE(p[1]))
			--result;
		    ++p;
		}
		ଅନ୍ୟଥା
		     ଯଦି (*p == '\\' || *p == '"')
		    ++result;
	    }

	    ଯଦି (buf != ଖାଲି)
	    {
		*buf++ = '"';
		ପାଇଁ (p = eap->arg; *p; ++p)
		{
		    ଯଦି (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    ଅନ୍ୟଥା ଯଦି (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			ଯଦି (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    ଅନ୍ୟଥା
			 ଯଦି (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    ଭାଙ୍ଗ;
	}
	ସ୍ଥିତି 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    ଯଦି (*split_buf == ଖାଲି)
		*split_buf = uc_split_args(eap->arg, split_len);

	    result = *split_len;
	    ଯଦି (buf != ଖାଲି && result != 0)
		STRCPY(buf, *split_buf);

	    ଭାଙ୍ଗ;
	}
	ଭାଙ୍ଗ;

    ସ୍ଥିତି ct_BANG:
	result = eap->forceit ? 1 : 0;
	ଯଦି (quote)
	    result += 2;
	ଯଦି (buf != ଖାଲି)
	{
	    ଯଦି (quote)
		*buf++ = '"';
	    ଯଦି (eap->forceit)
		*buf++ = '!';
	    ଯଦି (quote)
		*buf = '"';
	}
	ଭାଙ୍ଗ;

    ସ୍ଥିତି ct_LINE1:
    ସ୍ଥିତି ct_LINE2:
    ସ୍ଥିତି ct_RANGE:
    ସ୍ଥିତି ct_ଗଣନା:
    {
	ଅକ୍ଷର ଅଙ୍କ_buf[NUMBUFLEN];
	long ଅଙ୍କ = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_ଗଣନା :
		   (eap->addr_ଗଣନା > 0) ? eap->line2 : cmd->uc_def;
	size_t ଅଙ୍କ_len;

	ଅଙ୍କ_len = vim_snprintf(ଅଙ୍କ_buf, ଆକାର(ଅଙ୍କ_buf), "%ld", ଅଙ୍କ);
	result = ଅଙ୍କ_len;

	ଯଦି (quote)
	    result += 2;

	ଯଦି (buf != ଖାଲି)
	{
	    ଯଦି (quote)
		*buf++ = '"';
	    STRCPY(buf, ଅଙ୍କ_buf);
	    buf += ଅଙ୍କ_len;
	    ଯଦି (quote)
		*buf = '"';
	}

	ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି ct_MODS:
    {
	result = produce_cmdmods(buf, &cmdmod, quote);
	ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି ct_REGISTER:
	result = eap->regname ? 1 : 0;
	ଯଦି (quote)
	    result += 2;
	ଯଦି (buf != ଖାଲି)
	{
	    ଯଦି (quote)
		*buf++ = '\'';
	    ଯଦି (eap->regname)
		*buf++ = eap->regname;
	    ଯଦି (quote)
		*buf = '\'';
	}
	ଭାଙ୍ଗ;

    ସ୍ଥିତି ct_LT:
	result = 1;
	ଯଦି (buf != ଖାଲି)
	    *buf = '<';
	ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	// Not recognized: just copy the '<' and return -1.
	result = (size_t)-1;
	ଯଦି (buf != ଖାଲି)
	    *buf = '<';
	ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ result;
}

/*
 * Execute a user defined command.
 */
    ଶୂନ୍ୟ
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*start;
    char_u	*end = ଖାଲି;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	split_len = 0;
    char_u	*split_buf = ଖାଲି;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_current_sctx = FALSE;
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_script_version = 0;
#endif

    ଯଦି (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    ଅନ୍ୟଥା
	cmd = USER_CMD_GA(&prevwin_curwin()->w_buffer->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = ଖାଲି;
    ପାଇଁ (;;)
    {
	p = cmd->uc_rep;    // source
	q = buf;	    // destination
	totlen = 0;

	ପାଇଁ (;;)
	{
	    start = vim_strchr(p, '<');
	    ଯଦି (start != ଖାଲି)
		end = vim_strchr(start + 1, '>');
	    ଯଦି (buf != ଖାଲି)
	    {
		ପାଇଁ (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		ଯଦି (*ksp == K_SPECIAL
			&& (start == ଖାଲି || ksp < start || end == ଖାଲି)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ifdef FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI)
#endif
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    ଯଦି (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    ଜାରି;
		}
	    }

	    // break if no <item> is found
	    ଯଦି (start == ଖାଲି || end == ଖାଲି)
		ଭାଙ୍ଗ;

	    // Include the '>'
	    ++end;

	    // Take everything up to the '<'
	    len = start - p;
	    ଯଦି (buf == ଖାଲି)
		totlen += len;
	    ଅନ୍ୟଥା
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_code(start, end - start, q, cmd, eap,
			     &split_buf, &split_len);
	    ଯଦି (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = start + 1;
		len = 1;
	    }
	    ଅନ୍ୟଥା
		p = end;
	    ଯଦି (buf == ଖାଲି)
		totlen += len;
	    ଅନ୍ୟଥା
		q += len;
	}
	ଯଦି (buf != ଖାଲି)	    // second time here, finished
	{
	    STRCPY(q, p);
	    ଭାଙ୍ଗ;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	ଯଦି (buf == ଖାଲି)
	{
	    vim_free(split_buf);
	    ଫେରନ୍ତୁ;
	}
    }

    ଯଦି ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_version = cmd->uc_script_ctx.sc_version;
#ifdef FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	ଯଦି (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_version =
				  SCRIPT_ITEM(current_sctx.sc_sid)->sn_version;
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = SCRIPT_VERSION_VIM9;
	}
#endif
    }

    (ଶୂନ୍ୟ)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    ଯଦି (restore_current_sctx)
    {
#ifdef FEAT_EVAL
	ଯଦି (restore_script_version != 0)
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version =
							restore_script_version;
#endif
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(split_buf);
}
