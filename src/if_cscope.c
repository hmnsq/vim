/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * CSCOPE support for Vim added by Andy Kahn <kahn@zk3.dec.com>
 * Ported to Win32 by Sergey Khorev <sergey.khorev@gmail.com>
 *
 * The basic idea/structure of cscope for Vim was borrowed from Nvi.  There
 * might be a few lines of code that look similar to what Nvi has.
 *
 * See README.txt for an overview of the Vim source code.
 */

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# എങ്കിൽ defined(UNIX)
#  include <sys/wait.h>
# endif

# എങ്കിൽ defined (MSWIN)
#  ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
# endif

# define CSCOPE_SUCCESS		0
# define CSCOPE_FAILURE		-1

# define	CSCOPE_DBFILE		"cscope.out"
# define	CSCOPE_PROMPT		">> "

/*
 * See ":help cscope-find" for the possible queries.
 */

തരനിർവചനം ഘടന {
    അക്ഷരം *  name;
    പൂർണ്ണസംഖ്യ     (*func)(exarg_T *eap);
    അക്ഷരം *  help;
    അക്ഷരം *  usage;
    പൂർണ്ണസംഖ്യ	    cansplit;		// if supports splitting window
} cscmd_T;

തരനിർവചനം ഘടന csi {
    അക്ഷരം *	    fname;	// cscope db name
    അക്ഷരം *	    ppath;	// path to prepend (the -P option)
    അക്ഷരം *	    flags;	// additional cscope flags/options (e.g, -p2)
# എങ്കിൽ defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# അല്ലെങ്കിൽ
#  എങ്കിൽ defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_Index_മുകളിലെ;	// st_ino has no meaning in the Windows
    DWORD	    n_Index_താഴെയുള്ള;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    to_fp;	// to cscope: FILE.
} csinfo_T;

തരനിർവചനം എണ്ണൽ { Add, Find, Help, Kill, Reset, Show } csid_e;

തരനിർവചനം എണ്ണൽ {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_add(exarg_T *eap);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_add_common(അക്ഷരം *, അക്ഷരം *, അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_check_for_connections(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_check_for_tags(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_cnt_connections(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_create_connection(പൂർണ്ണസംഖ്യ i);
# ifdef FEAT_QUICKFIX
സ്ഥിതം ശൂന്യം	    cs_file_results(FILE *, പൂർണ്ണസംഖ്യ *);
# endif
സ്ഥിതം ശൂന്യം	    cs_fill_results(അക്ഷരം *, പൂർണ്ണസംഖ്യ , പൂർണ്ണസംഖ്യ *, അക്ഷരം ***,
			അക്ഷരം ***, പൂർണ്ണസംഖ്യ *);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_find(exarg_T *eap);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_find_common(അക്ഷരം *opt, അക്ഷരം *pat, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, char_u *cmdline);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_help(exarg_T *eap);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_തിരുകുക_filelist(അക്ഷരം *, അക്ഷരം *, അക്ഷരം *,
			stat_T *);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_kill(exarg_T *eap);
സ്ഥിതം ശൂന്യം	    cs_kill_execute(പൂർണ്ണസംഖ്യ, അക്ഷരം *);
സ്ഥിതം cscmd_T *    cs_lookup_cmd(exarg_T *eap);
സ്ഥിതം അക്ഷരം *	    cs_make_vim_style_matches(അക്ഷരം *, അക്ഷരം *,
			അക്ഷരം *, അക്ഷരം *);
സ്ഥിതം അക്ഷരം *	    cs_manage_matches(അക്ഷരം **, അക്ഷരം **, പൂർണ്ണസംഖ്യ, mcmd_e);
സ്ഥിതം ശൂന്യം	    cs_print_tags_priv(അക്ഷരം **, അക്ഷരം **, പൂർണ്ണസംഖ്യ);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_read_prompt(പൂർണ്ണസംഖ്യ);
സ്ഥിതം ശൂന്യം	    cs_release_csp(പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ freefnpp);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_reset(exarg_T *eap);
സ്ഥിതം അക്ഷരം *	    cs_resolve_file(പൂർണ്ണസംഖ്യ, അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ	    cs_show(exarg_T *eap);


സ്ഥിതം csinfo_T *   csinfo = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

സ്ഥിതം പൂർണ്ണസംഖ്യ	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
സ്ഥിതം cscmd_T	    cs_cmds[] =
{
    { "add",	cs_add,
		N_("Add a new database"),     "add file|dir [pre-path] [flags]", 0 },
    { "find",	cs_find,
		N_("Query for a pattern"),    "find a|c|d|e|f|g|i|s|t name", 1 },
    { "help",	cs_help,
		N_("Show this message"),      "help", 0 },
    { "kill",	cs_kill,
		N_("Kill a connection"),      "kill #", 0 },
    { "reset",	cs_reset,
		N_("Reinit all connections"), "reset", 0 },
    { "show",	cs_show,
		N_("Show connections"),       "show", 0 },
    { ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, 0 }
};

    സ്ഥിതം ശൂന്യം
cs_usage_സന്ദേശം(csid_e x)
{
    (ശൂന്യം)semsg(_(e_usage_cscope_str), cs_cmds[(പൂർണ്ണസംഖ്യ)x].usage);
}

സ്ഥിതം എണ്ണൽ
{
    EXP_CSCOPE_SUBCMD,	// expand ":cscope" sub-commands
    EXP_SCSCOPE_SUBCMD,	// expand ":scscope" sub-commands
    EXP_CSCOPE_FIND,	// expand ":cscope find" arguments
    EXP_CSCOPE_KILL	// expand ":cscope kill" arguments
} expand_what;

/*
 * Function given to ExpandGeneric() to obtain the cscope command
 * expansion.
 */
    char_u *
get_cscope_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    പൂർണ്ണസംഖ്യ		current_idx;
    പൂർണ്ണസംഖ്യ		i;

    തിരഞ്ഞെടുപ്പ് (expand_what)
    {
    അവസ്ഥ EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	തിരികെ (char_u *)cs_cmds[idx].name;
    അവസ്ഥ EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	വേണ്ടി (i = 0, current_idx = 0; cs_cmds[i].name != ശൂന്യ; i++)
	    എങ്കിൽ (cs_cmds[i].cansplit)
		എങ്കിൽ (current_idx++ == idx)
		    നിർത്തുക;
	തിരികെ (char_u *)cs_cmds[i].name;
    അവസ്ഥ EXP_CSCOPE_FIND:
	{
	    സ്ഥിരം അക്ഷരം *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", ശൂന്യ
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    തിരികെ (char_u *)query_type[idx];
	}
    അവസ്ഥ EXP_CSCOPE_KILL:
	{
	    സ്ഥിതം അക്ഷരം	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    വേണ്ടി (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		എങ്കിൽ (csinfo[i].fname == ശൂന്യ)
		    തുടരുക;
		എങ്കിൽ (current_idx++ == idx)
		{
		    vim_snprintf(connection, വലുപ്പം(connection), "%d", i);
		    തിരികെ (char_u *)connection;
		}
	    }
	    തിരികെ (current_idx == idx && idx > 0) ? (char_u *)"-1" : ശൂന്യ;
	}
    സ്ഥിരസ്ഥിതി:
	തിരികെ ശൂന്യ;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    ശൂന്യം
set_context_in_cscope_cmd(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_CSCOPE;
    xp->xp_pattern = arg;
    expand_what = (cmdidx == CMD_scscope)
			? EXP_SCSCOPE_SUBCMD : EXP_CSCOPE_SUBCMD;

    എങ്കിൽ (*arg == NUL)
	തിരികെ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    എങ്കിൽ (*p == NUL)
	തിരികെ;

    // past first word
    xp->xp_pattern = skipwhite(p);
    എങ്കിൽ (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    അല്ലെങ്കിൽ
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    സ്ഥിതം ശൂന്യം
do_cscope_general(
    exarg_T	*eap,
    പൂർണ്ണസംഖ്യ		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    എങ്കിൽ (check_restricted())
	തിരികെ;

    എങ്കിൽ ((cmdp = cs_lookup_cmd(eap)) == ശൂന്യ)
    {
	cs_help(eap);
	തിരികെ;
    }

    എങ്കിൽ (make_split)
    {
	എങ്കിൽ (!cmdp->cansplit)
	{
	    (ശൂന്യം)സന്ദേശം_puts(_("This cscope command does not support splitting the window.\n"));
	    തിരികെ;
	}
	postponed_split = -1;
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
    }

    cmdp->func(eap);

    postponed_split_flags = 0;
    postponed_split_tab = 0;
}

/*
 * Implementation of ":cscope" and ":lcscope"
 */
    ശൂന്യം
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    ശൂന്യം
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    ശൂന്യം
ex_cstag(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ ret = FALSE;

    എങ്കിൽ (*eap->arg == NUL)
    {
	(ശൂന്യം)emsg(_(e_usage_cstag_ident));
	തിരികെ;
    }

    തിരഞ്ഞെടുപ്പ് (p_csto)
    {
    അവസ്ഥ 0 :
	എങ്കിൽ (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (അക്ഷരം *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    എങ്കിൽ (ret == FALSE)
	    {
		cs_free_tags();
		എങ്കിൽ (സന്ദേശം_col)
		    സന്ദേശം_putchar('\n');

		എങ്കിൽ (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	നിർത്തുക;
    അവസ്ഥ 1 :
	എങ്കിൽ (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    എങ്കിൽ (ret == FALSE)
	    {
		എങ്കിൽ (സന്ദേശം_col)
		    സന്ദേശം_putchar('\n');

		എങ്കിൽ (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (അക്ഷരം *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    എങ്കിൽ (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (അക്ഷരം *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    എങ്കിൽ (ret == FALSE)
		cs_free_tags();
	}
	നിർത്തുക;
    സ്ഥിരസ്ഥിതി :
	നിർത്തുക;
    }

    എങ്കിൽ (!ret)
    {
	(ശൂന്യം)emsg(_(e_cstag_tag_not_founc));
# എങ്കിൽ defined(FEAT_QUICKFIX)
	g_do_tagpreview = 0;
# endif
    }

}


/*
 * This simulates a vim_fgets(), but for cscope, returns the next line
 * from the cscope output.  should only be called from find_tags()
 *
 * returns TRUE if eof, FALSE otherwise
 */
    പൂർണ്ണസംഖ്യ
cs_fgets(char_u *buf, പൂർണ്ണസംഖ്യ size)
{
    അക്ഷരം *p;

    എങ്കിൽ ((p = cs_manage_matches(ശൂന്യ, ശൂന്യ, -1, Get)) == ശൂന്യ)
	തിരികെ TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    തിരികെ FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    ശൂന്യം
cs_free_tags(ശൂന്യം)
{
    cs_manage_matches(ശൂന്യ, ശൂന്യ, -1, Free);
}


/*
 * Called from do_tag().
 */
    ശൂന്യം
cs_print_tags(ശൂന്യം)
{
    cs_manage_matches(ശൂന്യ, ശൂന്യ, -1, Print);
}


/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 *		Checks for the existence of a |cscope| connection.  If no
 *		parameters are specified, then the function returns:
 *
 *		0, if cscope was not available (not compiled in), or if there
 *		are no cscope connections; or
 *		1, if there is at least one cscope connection.
 *
 *		If parameters are specified, then the value of {num}
 *		determines how existence of a cscope connection is checked:
 *
 *		{num}	Description of existence check
 *		-----	------------------------------
 *		0	Same as no parameters (e.g., "cscope_connection()").
 *		1	Ignore {prepend}, and use partial string matches for
 *			{dbpath}.
 *		2	Ignore {prepend}, and use exact string matches for
 *			{dbpath}.
 *		3	Use {prepend}, use partial string matches for both
 *			{dbpath} and {prepend}.
 *		4	Use {prepend}, use exact string matches for both
 *			{dbpath} and {prepend}.
 *
 *		Note: All string comparisons are case sensitive!
 */
# എങ്കിൽ defined(FEAT_EVAL)
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_connection(പൂർണ്ണസംഖ്യ അക്കം, char_u *dbpath, char_u *ppath)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (അക്കം < 0 || അക്കം > 4 || (അക്കം > 0 && !dbpath))
	തിരികെ FALSE;

    വേണ്ടി (i = 0; i < csinfo_size; i++)
    {
	എങ്കിൽ (!csinfo[i].fname)
	    തുടരുക;

	എങ്കിൽ (അക്കം == 0)
	    തിരികെ TRUE;

	തിരഞ്ഞെടുപ്പ് (അക്കം)
	{
	അവസ്ഥ 1:
	    എങ്കിൽ (strstr(csinfo[i].fname, (അക്ഷരം *)dbpath))
		തിരികെ TRUE;
	    നിർത്തുക;
	അവസ്ഥ 2:
	    എങ്കിൽ (strcmp(csinfo[i].fname, (അക്ഷരം *)dbpath) == 0)
		തിരികെ TRUE;
	    നിർത്തുക;
	അവസ്ഥ 3:
	    എങ്കിൽ (strstr(csinfo[i].fname, (അക്ഷരം *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (അക്ഷരം *)ppath))))
		തിരികെ TRUE;
	    നിർത്തുക;
	അവസ്ഥ 4:
	    എങ്കിൽ ((strcmp(csinfo[i].fname, (അക്ഷരം *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (അക്ഷരം *)ppath) == 0))))
		തിരികെ TRUE;
	    നിർത്തുക;
	}
    }

    തിരികെ FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_add(exarg_T *eap UNUSED)
{
    അക്ഷരം *fname, *ppath, *flags = ശൂന്യ;

    എങ്കിൽ ((fname = strtok((അക്ഷരം *)ശൂന്യ, (സ്ഥിരം അക്ഷരം *)" ")) == ശൂന്യ)
    {
	cs_usage_സന്ദേശം(Add);
	തിരികെ CSCOPE_FAILURE;
    }
    എങ്കിൽ ((ppath = strtok((അക്ഷരം *)ശൂന്യ, (സ്ഥിരം അക്ഷരം *)" ")) != ശൂന്യ)
	flags = strtok((അക്ഷരം *)ശൂന്യ, (സ്ഥിരം അക്ഷരം *)" ");

    തിരികെ cs_add_common(fname, ppath, flags);
}

    സ്ഥിതം ശൂന്യം
cs_stat_emsg(അക്ഷരം *fname)
{
    പൂർണ്ണസംഖ്യ err = errno;
    (ശൂന്യം)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_add_common(
    അക്ഷരം *arg1,	    // filename - may contain environment variables
    അക്ഷരം *arg2,	    // prepend path - may contain environment variables
    അക്ഷരം *flags)
{
    stat_T	statbuf;
    പൂർണ്ണസംഖ്യ		ret;
    അക്ഷരം	*fname = ശൂന്യ;
    അക്ഷരം	*fname2 = ശൂന്യ;
    അക്ഷരം	*ppath = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = ശൂന്യ;

    // get the filename (arg1), expand it, and try to stat it
    എങ്കിൽ ((fname = alloc(MAXPATHL + 1)) == ശൂന്യ)
	പോകുക add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (ശൂന്യം)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    എങ്കിൽ (fname == ശൂന്യ)
	പോകുക add_err;
    fname = (അക്ഷരം *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    എങ്കിൽ (ret < 0)
    {
staterr:
	എങ്കിൽ (p_csverbose)
	    cs_stat_emsg(fname);
	പോകുക add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    എങ്കിൽ (arg2 != ശൂന്യ)
    {
	stat_T	    statbuf2;

	എങ്കിൽ ((ppath = alloc(MAXPATHL + 1)) == ശൂന്യ)
	    പോകുക add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	എങ്കിൽ (ret < 0)
	    പോകുക staterr;
    }

    // if filename is a directory, append the cscope database name to it
    എങ്കിൽ (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	എങ്കിൽ (fname2 == ശൂന്യ)
	    പോകുക add_err;

	ആകുമ്പോൾ (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    എങ്കിൽ (fname[0] == '\0')
		നിർത്തുക;
	}
	എങ്കിൽ (fname[0] == '\0')
	    (ശൂന്യം)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	അല്ലെങ്കിൽ
	    (ശൂന്യം)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	എങ്കിൽ (ret < 0)
	{
	    എങ്കിൽ (p_csverbose)
		cs_stat_emsg(fname2);
	    പോകുക add_err;
	}

	i = cs_തിരുകുക_filelist(fname2, ppath, flags, &statbuf);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_തിരുകുക_filelist(fname, ppath, flags, &statbuf);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (p_csverbose)
	    (ശൂന്യം)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	പോകുക add_err;
    }

    എങ്കിൽ (i != -1)
    {
	എങ്കിൽ (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    പോകുക add_err;
	}

	എങ്കിൽ (p_csverbose)
	{
	    സന്ദേശം_clr_eos();
	    (ശൂന്യം)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    തിരികെ CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    തിരികെ CSCOPE_FAILURE;
}


    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_check_for_connections(ശൂന്യം)
{
    തിരികെ (cs_cnt_connections() > 0);
}


    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_check_for_tags(ശൂന്യം)
{
    തിരികെ (p_tags[0] != NUL && curbuf->b_p_tags != ശൂന്യ);
}


/*
 * Count the number of cscope connections.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_cnt_connections(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;
    പൂർണ്ണസംഖ്യ cnt = 0;

    വേണ്ടി (i = 0; i < csinfo_size; i++)
	എങ്കിൽ (csinfo[i].fname != ശൂന്യ)
	    cnt++;
    തിരികെ cnt;
}

    സ്ഥിതം ശൂന്യം
cs_reading_emsg(
    പൂർണ്ണസംഖ്യ idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_cnt_matches(പൂർണ്ണസംഖ്യ idx)
{
    അക്ഷരം *stok;
    അക്ഷരം *buf;
    പൂർണ്ണസംഖ്യ nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ 0;
    വേണ്ടി (;;)
    {
	എങ്കിൽ (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    എങ്കിൽ (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    തിരികെ -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	എങ്കിൽ (strstr((സ്ഥിരം അക്ഷരം *)buf, "Unable to search database") != ശൂന്യ)
	    നിർത്തുക;
	എങ്കിൽ ((stok = strtok(buf, (സ്ഥിരം അക്ഷരം *)" ")) == ശൂന്യ)
	    തുടരുക;
	എങ്കിൽ (strstr((സ്ഥിരം അക്ഷരം *)stok, "cscope:") == ശൂന്യ)
	    തുടരുക;

	എങ്കിൽ ((stok = strtok(ശൂന്യ, (സ്ഥിരം അക്ഷരം *)" ")) == ശൂന്യ)
	    തുടരുക;
	nlines = atoi(stok);
	എങ്കിൽ (nlines < 0)
	{
	    nlines = 0;
	    നിർത്തുക;
	}

	എങ്കിൽ ((stok = strtok(ശൂന്യ, (സ്ഥിരം അക്ഷരം *)" ")) == ശൂന്യ)
	    തുടരുക;
	എങ്കിൽ (strncmp((സ്ഥിരം അക്ഷരം *)stok, "lines", 5))
	    തുടരുക;

	നിർത്തുക;
    }

    vim_free(buf);
    തിരികെ nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    സ്ഥിതം അക്ഷരം *
cs_create_cmd(അക്ഷരം *csoption, അക്ഷരം *pattern)
{
    അക്ഷരം *cmd;
    short തിരയൽ;
    അക്ഷരം *pat;

    തിരഞ്ഞെടുപ്പ് (csoption[0])
    {
    അവസ്ഥ '0' : അവസ്ഥ 's' :
	തിരയൽ = 0;
	നിർത്തുക;
    അവസ്ഥ '1' : അവസ്ഥ 'g' :
	തിരയൽ = 1;
	നിർത്തുക;
    അവസ്ഥ '2' : അവസ്ഥ 'd' :
	തിരയൽ = 2;
	നിർത്തുക;
    അവസ്ഥ '3' : അവസ്ഥ 'c' :
	തിരയൽ = 3;
	നിർത്തുക;
    അവസ്ഥ '4' : അവസ്ഥ 't' :
	തിരയൽ = 4;
	നിർത്തുക;
    അവസ്ഥ '6' : അവസ്ഥ 'e' :
	തിരയൽ = 6;
	നിർത്തുക;
    അവസ്ഥ '7' : അവസ്ഥ 'f' :
	തിരയൽ = 7;
	നിർത്തുക;
    അവസ്ഥ '8' : അവസ്ഥ 'i' :
	തിരയൽ = 8;
	നിർത്തുക;
    അവസ്ഥ '9' : അവസ്ഥ 'a' :
	തിരയൽ = 9;
	നിർത്തുക;
    സ്ഥിരസ്ഥിതി :
	(ശൂന്യം)emsg(_(e_unknown_cscope_തിരയൽ_type));
	cs_usage_സന്ദേശം(Find);
	തിരികെ ശൂന്യ;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    എങ്കിൽ (തിരയൽ != 4 && തിരയൽ != 6)
	ആകുമ്പോൾ VIM_ISWHITE(*pat)
	    ++pat;

    എങ്കിൽ ((cmd = alloc(strlen(pat) + 2)) == ശൂന്യ)
	തിരികെ ശൂന്യ;

    (ശൂന്യം)sprintf(cmd, "%d%s", തിരയൽ, pat);

    തിരികെ cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_create_connection(പൂർണ്ണസംഖ്യ i)
{
# ifdef UNIX
    പൂർണ്ണസംഖ്യ		to_cs[2], from_cs[2];
# endif
    പൂർണ്ണസംഖ്യ		cmdlen;
    അക്ഷരം	*prog, *cmd, *ppath = ശൂന്യ;
    size_t	proglen;
# ifdef MSWIN
    പൂർണ്ണസംഖ്യ		fd;
    പൂർണ്ണസംഖ്യ		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# എങ്കിൽ defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    എങ്കിൽ (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(ശൂന്യം)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	എങ്കിൽ (to_cs[0] != -1)
	    (ശൂന്യം)close(to_cs[0]);
	എങ്കിൽ (to_cs[1] != -1)
	    (ശൂന്യം)close(to_cs[1]);
	എങ്കിൽ (from_cs[0] != -1)
	    (ശൂന്യം)close(from_cs[0]);
	എങ്കിൽ (from_cs[1] != -1)
	    (ശൂന്യം)close(from_cs[1]);
	തിരികെ CSCOPE_FAILURE;
    }

    എങ്കിൽ ((csinfo[i].pid = fork()) == -1)
    {
	(ശൂന്യം)emsg(_(e_could_not_fork_for_cscope));
	പോകുക err_closing;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (csinfo[i].pid == 0)	// child: run cscope.
    {
	അക്ഷരം **argv = ശൂന്യ;

	എങ്കിൽ (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	എങ്കിൽ (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	എങ്കിൽ (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(ശൂന്യം)close(to_cs[1]);
	(ശൂന്യം)close(from_cs[0]);
# അല്ലെങ്കിൽ
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = വലുപ്പം(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ശൂന്യ;

	എങ്കിൽ (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (ശൂന്യം)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    എങ്കിൽ (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    എങ്കിൽ (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    തിരികെ CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	എങ്കിൽ ((prog = alloc(MAXPATHL + 1)) == ശൂന്യ)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# അല്ലെങ്കിൽ
	    // MSWIN
	    പോകുക err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (പൂർണ്ണസംഖ്യ)(proglen + strlen(csinfo[i].fname) + 32);
	എങ്കിൽ (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    എങ്കിൽ ((ppath = alloc(MAXPATHL + 1)) == ശൂന്യ)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# അല്ലെങ്കിൽ
		// MSWIN
		പോകുക err_closing;
# endif
	    }
	    cmdlen += (പൂർണ്ണസംഖ്യ)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	എങ്കിൽ (csinfo[i].flags)
	    cmdlen += (പൂർണ്ണസംഖ്യ)strlen(csinfo[i].flags);

	എങ്കിൽ ((cmd = alloc(cmdlen)) == ശൂന്യ)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# അല്ലെങ്കിൽ
	    // MSWIN
	    പോകുക err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    അക്ഷരം	**tok = ശൂന്യ;
	    പൂർണ്ണസംഖ്യ		tokc = 0;

	    ga_init2(&ga_argv, വലുപ്പം(അക്ഷരം *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    എങ്കിൽ (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < tokc; ++k)
	    {
		എങ്കിൽ (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((അക്ഷരം **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    എങ്കിൽ (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((അക്ഷരം **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (അക്ഷരം *)vim_strsave((char_u *)"-dl");
	    ((അക്ഷരം **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (അക്ഷരം *)vim_strsave((char_u *)"-f");
	    ((അക്ഷരം **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(അക്ഷരം *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    എങ്കിൽ (ppath != ശൂന്യ)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		അക്ഷരം	*parg = alloc(plen);

		എങ്കിൽ (parg == ശൂന്യ || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((അക്ഷരം **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    എങ്കിൽ (csinfo[i].flags != ശൂന്യ)
	    {
		എങ്കിൽ (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < tokc; ++k)
		{
		    എങ്കിൽ (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((അക്ഷരം **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    എങ്കിൽ (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((അക്ഷരം **)ga_argv.ga_data)[ga_argv.ga_len] = ശൂന്യ;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  എങ്കിൽ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   എങ്കിൽ defined(HAVE_SETSID)
	    (ശൂന്യം)setsid();
#   അല്ലെങ്കിൽ
	    എങ്കിൽ (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (അക്ഷരം **)ga_argv.ga_data;
	    എങ്കിൽ (argv[0] == ശൂന്യ || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    അല്ലെങ്കിൽ	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	എങ്കിൽ ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == ശൂന്യ)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	എങ്കിൽ ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == ശൂന്യ)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(ശൂന്യം)close(to_cs[0]);
	(ശൂന്യം)close(from_cs[1]);
    }
# അല്ലെങ്കിൽ
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    എങ്കിൽ (csinfo[i].ppath != ശൂന്യ)
    {
	len = (പൂർണ്ണസംഖ്യ)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    എങ്കിൽ (csinfo[i].flags != ശൂന്യ)
    {
	len = (പൂർണ്ണസംഖ്യ)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_നിവേശം  = stdin_rd;
    created = CreateProcess(ശൂന്യ, cmd, ശൂന്യ, ശൂന്യ, TRUE, CREATE_NEW_CONSOLE,
							ശൂന്യ, ശൂന്യ, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    എങ്കിൽ (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(ശൂന്യം)emsg(_(e_could_not_spawn_cscope_process));
	പോകുക err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    എങ്കിൽ (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == ശൂന്യ))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    എങ്കിൽ (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == ശൂന്യ))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    തിരികെ CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_find(exarg_T *eap)
{
    അക്ഷരം *opt, *pat;
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (cs_check_for_connections() == FALSE)
    {
	(ശൂന്യം)emsg(_(e_no_cscope_connections));
	തിരികെ FALSE;
    }

    എങ്കിൽ ((opt = strtok((അക്ഷരം *)ശൂന്യ, (സ്ഥിരം അക്ഷരം *)" ")) == ശൂന്യ)
    {
	cs_usage_സന്ദേശം(Find);
	തിരികെ FALSE;
    }

    pat = opt + strlen(opt) + 1;
    എങ്കിൽ (pat >= (അക്ഷരം *)eap->arg + eap_arg_len)
    {
	cs_usage_സന്ദേശം(Find);
	തിരികെ FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    വേണ്ടി (i = 0; i < eap_arg_len; ++i)
	എങ്കിൽ (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    തിരികെ cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_find_common(
    അക്ഷരം *opt,
    അക്ഷരം *pat,
    പൂർണ്ണസംഖ്യ forceit,
    പൂർണ്ണസംഖ്യ verbose,
    പൂർണ്ണസംഖ്യ	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    പൂർണ്ണസംഖ്യ i;
    അക്ഷരം *cmd;
    പൂർണ്ണസംഖ്യ *nummatches;
    പൂർണ്ണസംഖ്യ totmatches;
# ifdef FEAT_QUICKFIX
    അക്ഷരം cmdletter;
    അക്ഷരം *qfpos;

    // get cmd letter
    തിരഞ്ഞെടുപ്പ് (opt[0])
    {
    അവസ്ഥ '0' :
	cmdletter = 's';
	നിർത്തുക;
    അവസ്ഥ '1' :
	cmdletter = 'g';
	നിർത്തുക;
    അവസ്ഥ '2' :
	cmdletter = 'd';
	നിർത്തുക;
    അവസ്ഥ '3' :
	cmdletter = 'c';
	നിർത്തുക;
    അവസ്ഥ '4' :
	cmdletter = 't';
	നിർത്തുക;
    അവസ്ഥ '6' :
	cmdletter = 'e';
	നിർത്തുക;
    അവസ്ഥ '7' :
	cmdletter = 'f';
	നിർത്തുക;
    അവസ്ഥ '8' :
	cmdletter = 'i';
	നിർത്തുക;
    അവസ്ഥ '9' :
	cmdletter = 'a';
	നിർത്തുക;
    സ്ഥിരസ്ഥിതി :
	cmdletter = opt[0];
    }

    qfpos = (അക്ഷരം *)vim_strchr(p_csqf, cmdletter);
    എങ്കിൽ (qfpos != ശൂന്യ)
    {
	qfpos++;
	// next symbol must be + or -
	എങ്കിൽ (strchr(CSQF_FLAGS, *qfpos) == ശൂന്യ)
	{
	    (ശൂന്യം)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    തിരികെ FALSE;
	}

	എങ്കിൽ (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    എങ്കിൽ (aborting())
		തിരികെ FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    എങ്കിൽ (cmd == ശൂന്യ)
	തിരികെ FALSE;

    nummatches = ALLOC_MULT(പൂർണ്ണസംഖ്യ, csinfo_size);
    എങ്കിൽ (nummatches == ശൂന്യ)
    {
	vim_free(cmd);
	തിരികെ FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    വേണ്ടി (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    വേണ്ടി (i = 0; i < csinfo_size; i++)
    {
	എങ്കിൽ (csinfo[i].fname == ശൂന്യ || csinfo[i].to_fp == ശൂന്യ)
	    തുടരുക;

	// send cmd to cscope
	(ശൂന്യം)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(ശൂന്യം)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	എങ്കിൽ (nummatches[i] > -1)
	    totmatches += nummatches[i];

	എങ്കിൽ (nummatches[i] == 0)
	    (ശൂന്യം)cs_read_prompt(i);
    }
    vim_free(cmd);

    എങ്കിൽ (totmatches == 0)
    {
	എങ്കിൽ (verbose)
	    (ശൂന്യം)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	തിരികെ FALSE;
    }

# ifdef FEAT_QUICKFIX
    എങ്കിൽ (qfpos != ശൂന്യ && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = ശൂന്യ;
	win_T	    *wp = ശൂന്യ;

	എങ്കിൽ (tmp == ശൂന്യ)
	{
	    vim_free(nummatches);
	    തിരികെ FALSE;
	}

	f = mch_fopen((അക്ഷരം *)tmp, "w");
	എങ്കിൽ (f == ശൂന്യ)
	    semsg(_(e_cant_open_file_str), tmp);
	അല്ലെങ്കിൽ
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    എങ്കിൽ (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    എങ്കിൽ (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, ശൂന്യ) > 0)
	    {
		എങ്കിൽ (postponed_split != 0)
		{
		    (ശൂന്യം)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		എങ്കിൽ (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != ശൂന്യ)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	തിരികെ TRUE;
    }
    അല്ലെങ്കിൽ
# endif // FEAT_QUICKFIX
    {
	അക്ഷരം **matches = ശൂന്യ, **contexts = ശൂന്യ;
	പൂർണ്ണസംഖ്യ matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	എങ്കിൽ (matches == ശൂന്യ)
	    തിരികെ FALSE;

	(ശൂന്യം)cs_manage_matches(matches, contexts, matched, Store);

	തിരികെ do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (ശൂന്യം)സന്ദേശം_puts(_("cscope commands:\n"));
    ആകുമ്പോൾ (cmdp->name != ശൂന്യ)
    {
	അക്ഷരം *help = _(cmdp->help);
	പൂർണ്ണസംഖ്യ  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	എങ്കിൽ (space_cnt < 0)
	    space_cnt = 0;
	(ശൂന്യം)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	എങ്കിൽ (strcmp(cmdp->name, "find") == 0)
	    സന്ദേശം_puts(_("\n"
		       "       a: Find assignments to this symbol\n"
		       "       c: Find functions calling this function\n"
		       "       d: Find functions called by this function\n"
		       "       e: Find this egrep pattern\n"
		       "       f: Find this file\n"
		       "       g: Find this definition\n"
		       "       i: Find files #including this file\n"
		       "       s: Find this C symbol\n"
		       "       t: Find this text string\n"));

	cmdp++;
    }

    wait_return(TRUE);
    തിരികെ 0;
}


    സ്ഥിതം ശൂന്യം
clear_csinfo(പൂർണ്ണസംഖ്യ i)
{
    csinfo[i].fname  = ശൂന്യ;
    csinfo[i].ppath  = ശൂന്യ;
    csinfo[i].flags  = ശൂന്യ;
# എങ്കിൽ defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# അല്ലെങ്കിൽ
    csinfo[i].nVolume = 0;
    csinfo[i].n_Index_മുകളിലെ = 0;
    csinfo[i].n_Index_താഴെയുള്ള = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = ശൂന്യ;
    csinfo[i].to_fp  = ശൂന്യ;
# എങ്കിൽ defined(MSWIN)
    csinfo[i].hProc = ശൂന്യ;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_തിരുകുക_filelist(
    അക്ഷരം *fname,
    അക്ഷരം *ppath,
    അക്ഷരം *flags,
    stat_T *sb UNUSED)
{
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    തിരഞ്ഞെടുപ്പ് (win32_fileinfo((char_u *)fname, &bhfi))
    {
	അവസ്ഥ FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	അവസ്ഥ FILEINFO_READ_FAIL:	// CreateFile() failed
	    എങ്കിൽ (p_csverbose)
	    {
		അക്ഷരം *cant_സന്ദേശം = _(e_cannot_open_cscope_database_str);
		അക്ഷരം *winmsg = GetWin32Error();

		എങ്കിൽ (winmsg != ശൂന്യ)
		    (ശൂന്യം)semsg(cant_സന്ദേശം, winmsg);
		അല്ലെങ്കിൽ
		    // subst filename if can't get error text
		    (ശൂന്യം)semsg(cant_സന്ദേശം, fname);
	    }
	    തിരികെ -1;

	അവസ്ഥ FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    എങ്കിൽ (p_csverbose)
		(ശൂന്യം)emsg(_(e_cannot_get_cscope_database_information));
	    തിരികെ -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    വേണ്ടി (j = 0; j < csinfo_size; j++)
    {
	എങ്കിൽ (csinfo[j].fname != ശൂന്യ
# എങ്കിൽ defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# അല്ലെങ്കിൽ
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_സംഖ്യ
		    && csinfo[j].n_Index_മുകളിലെ == bhfi.n_File_Index_മുകളിലെ
		    && csinfo[j].n_Index_താഴെയുള്ള == bhfi.n_File_Index_താഴെയുള്ള))
# endif
	    )
	{
	    എങ്കിൽ (p_csverbose)
		(ശൂന്യം)emsg(_(e_duplicate_cscope_database_not_added));
	    തിരികെ -1;
	}

	എങ്കിൽ (csinfo[j].fname == ശൂന്യ && i == -1)
	    i = j; // remember first empty entry
    }

    എങ്കിൽ (i == -1)
    {
	i = csinfo_size;
	എങ്കിൽ (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	അല്ലെങ്കിൽ
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, വലുപ്പം(csinfo_T)*csinfo_size);
	    എങ്കിൽ (csinfo == ശൂന്യ)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		തിരികെ -1;
	    }
	}
	എങ്കിൽ (csinfo == ശൂന്യ)
	    തിരികെ -1;
	വേണ്ടി (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    എങ്കിൽ ((csinfo[i].fname = alloc(strlen(fname)+1)) == ശൂന്യ)
	തിരികെ -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((സ്ഥിരം അക്ഷരം *)fname));

    എങ്കിൽ (ppath != ശൂന്യ)
    {
	എങ്കിൽ ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == ശൂന്യ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    തിരികെ -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((സ്ഥിരം അക്ഷരം *)ppath));
    }
    അല്ലെങ്കിൽ
	csinfo[i].ppath = ശൂന്യ;

    എങ്കിൽ (flags != ശൂന്യ)
    {
	എങ്കിൽ ((csinfo[i].flags = alloc(strlen(flags) + 1)) == ശൂന്യ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    തിരികെ -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((സ്ഥിരം അക്ഷരം *)flags));
    }
    അല്ലെങ്കിൽ
	csinfo[i].flags = ശൂന്യ;

# എങ്കിൽ defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# അല്ലെങ്കിൽ
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_സംഖ്യ;
    csinfo[i].n_Index_താഴെയുള്ള = bhfi.n_File_Index_താഴെയുള്ള;
    csinfo[i].n_Index_മുകളിലെ = bhfi.n_File_Index_മുകളിലെ;
# endif
    തിരികെ i;
}


/*
 * Find cscope command in command table.
 */
    സ്ഥിതം cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    അക്ഷരം *stok;
    size_t len;

    എങ്കിൽ (eap->arg == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (പൂർണ്ണസംഖ്യ)STRLEN(eap->arg);

    എങ്കിൽ ((stok = strtok((അക്ഷരം *)(eap->arg), (സ്ഥിരം അക്ഷരം *)" ")) == ശൂന്യ)
	തിരികെ ശൂന്യ;

    len = strlen(stok);
    വേണ്ടി (cmdp = cs_cmds; cmdp->name != ശൂന്യ; ++cmdp)
    {
	എങ്കിൽ (strncmp((സ്ഥിരം അക്ഷരം *)(stok), cmdp->name, len) == 0)
	    തിരികെ (cmdp);
    }
    തിരികെ ശൂന്യ;
}


/*
 * Nuke em.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_kill(exarg_T *eap UNUSED)
{
    അക്ഷരം *stok;
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ ((stok = strtok((അക്ഷരം *)ശൂന്യ, (സ്ഥിരം അക്ഷരം *)" ")) == ശൂന്യ)
    {
	cs_usage_സന്ദേശം(Kill);
	തിരികെ CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    എങ്കിൽ ((strlen(stok) < 2 && VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(stok[1]))))
	i = atoi(stok);
    അല്ലെങ്കിൽ
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	വേണ്ടി (i = 0; i < csinfo_size; i++)
	{
	    എങ്കിൽ (csinfo[i].fname != ശൂന്യ && strstr(csinfo[i].fname, stok))
		നിർത്തുക;
	}
    }

    എങ്കിൽ ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == ശൂന്യ))
    {
	എങ്കിൽ (p_csverbose)
	    (ശൂന്യം)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (i == -1)
	{
	    വേണ്ടി (i = 0; i < csinfo_size; i++)
	    {
		എങ്കിൽ (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	അല്ലെങ്കിൽ
	    cs_kill_execute(i, stok);
    }

    തിരികെ 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    സ്ഥിതം ശൂന്യം
cs_kill_execute(
    പൂർണ്ണസംഖ്യ i,	    // cscope table index
    അക്ഷരം *cname)    // cscope database name
{
    എങ്കിൽ (p_csverbose)
    {
	സന്ദേശം_clr_eos();
	(ശൂന്യം)smsg_attr(HL_ATTR(HLF_R) | സന്ദേശം_HIST,
			_("cscope connection %s closed"), cname);
    }
    cs_release_csp(i, TRUE);
}


/*
 * Convert the cscope output into a ctags style entry (as might be found
 * in a ctags tags file).  there's one catch though: cscope doesn't tell you
 * the type of the tag you are looking for.  for example, in Darren Hiebert's
 * ctags (the one that comes with vim), #define's use a line number to find the
 * tag in a file while function definitions use a regexp search pattern.
 *
 * I'm going to always use the line number because cscope does something
 * quirky (and probably other things i don't know about):
 *
 *     if you have "#  define" in your source file, which is
 *     perfectly legal, cscope thinks you have "#define".  this
 *     will result in a failed regexp search. :(
 *
 * Besides, even if this particular case didn't happen, the search pattern
 * would still have to be modified to escape all the special regular expression
 * characters to comply with ctags formatting.
 */
    സ്ഥിതം അക്ഷരം *
cs_make_vim_style_matches(
    അക്ഷരം *fname,
    അക്ഷരം *slno,
    അക്ഷരം *തിരയൽ,
    അക്ഷരം *tagstr)
{
    // vim style is ctags:
    //
    //	    <tagstr>\t<filename>\t<linenum_or_search>"\t<extra>
    //
    // but as mentioned above, we'll always use the line number and
    // put the search pattern (if one exists) as "extra"
    //
    // buf is used as part of vim's method of handling tags, and
    // (i think) vim frees it when you pop your tags and get replaced
    // by new ones on the tag stack.
    അക്ഷരം *buf;
    പൂർണ്ണസംഖ്യ amt;

    എങ്കിൽ (തിരയൽ != ശൂന്യ)
    {
	amt = (പൂർണ്ണസംഖ്യ)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(തിരയൽ)+6);
	എങ്കിൽ ((buf = alloc(amt)) == ശൂന്യ)
	    തിരികെ ശൂന്യ;

	(ശൂന്യം)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, തിരയൽ);
    }
    അല്ലെങ്കിൽ
    {
	amt = (പൂർണ്ണസംഖ്യ)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	എങ്കിൽ ((buf = alloc(amt)) == ശൂന്യ)
	    തിരികെ ശൂന്യ;

	(ശൂന്യം)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    തിരികെ buf;
}


/*
 * This is kind of hokey, but i don't see an easy way round this.
 *
 * Store: keep a ptr to the (malloc'd) memory of matches originally
 * generated from cs_find().  the matches are originally lines directly
 * from cscope output, but transformed to look like something out of a
 * ctags.  see cs_make_vim_style_matches for more details.
 *
 * Get: used only from cs_fgets(), this simulates a vim_fgets() to return
 * the next line from the cscope output.  it basically keeps track of which
 * lines have been "used" and returns the next one.
 *
 * Free: frees up everything and resets
 *
 * Print: prints the tags
 */
    സ്ഥിതം അക്ഷരം *
cs_manage_matches(
    അക്ഷരം **matches,
    അക്ഷരം **contexts,
    പൂർണ്ണസംഖ്യ totmatches,
    mcmd_e cmd)
{
    സ്ഥിതം അക്ഷരം **mp = ശൂന്യ;
    സ്ഥിതം അക്ഷരം **cp = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ cnt = -1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ next = -1;
    അക്ഷരം *p = ശൂന്യ;

    തിരഞ്ഞെടുപ്പ് (cmd)
    {
    അവസ്ഥ Store:
	പ്രതിജ്ഞാ(matches != ശൂന്യ);
	പ്രതിജ്ഞാ(totmatches > 0);
	എങ്കിൽ (mp != ശൂന്യ || cp != ശൂന്യ)
	    (ശൂന്യം)cs_manage_matches(ശൂന്യ, ശൂന്യ, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	നിർത്തുക;
    അവസ്ഥ Get:
	എങ്കിൽ (next >= cnt)
	    തിരികെ ശൂന്യ;

	p = mp[next];
	next++;
	നിർത്തുക;
    അവസ്ഥ Free:
	എങ്കിൽ (mp != ശൂന്യ)
	{
	    എങ്കിൽ (cnt > 0)
		ആകുമ്പോൾ (cnt--)
		{
		    vim_free(mp[cnt]);
		    എങ്കിൽ (cp != ശൂന്യ)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = ശൂന്യ;
	cp = ശൂന്യ;
	cnt = 0;
	next = 0;
	നിർത്തുക;
    അവസ്ഥ Print:
	cs_print_tags_priv(mp, cp, cnt);
	നിർത്തുക;
    സ്ഥിരസ്ഥിതി:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	തിരികെ ശൂന്യ;
    }

    തിരികെ p;
}


/*
 * Parse cscope output.
 */
    സ്ഥിതം അക്ഷരം *
cs_parse_results(
    പൂർണ്ണസംഖ്യ cnumber,
    അക്ഷരം *buf,
    പൂർണ്ണസംഖ്യ bufsize,
    അക്ഷരം **context,
    അക്ഷരം **linenumber,
    അക്ഷരം **തിരയൽ)
{
    പൂർണ്ണസംഖ്യ ch;
    അക്ഷരം *p;
    അക്ഷരം *name;

    എങ്കിൽ (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == ശൂന്യ)
    {
	എങ്കിൽ (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	തിരികെ ശൂന്യ;
    }

    // If the line's too long for the buffer, discard it.
    എങ്കിൽ ((p = strchr(buf, '\n')) == ശൂന്യ)
    {
	ആകുമ്പോൾ ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	തിരികെ ശൂന്യ;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    എങ്കിൽ ((name = strtok(buf, (സ്ഥിരം അക്ഷരം *)" ")) == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ ((*context = strtok(ശൂന്യ, (സ്ഥിരം അക്ഷരം *)" ")) == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ ((*linenumber = strtok(ശൂന്യ, (സ്ഥിരം അക്ഷരം *)" ")) == ശൂന്യ)
	തിരികെ ശൂന്യ;
    *തിരയൽ = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    എങ്കിൽ (strcmp(*തിരയൽ, "<unknown>") == 0)
	*തിരയൽ = ശൂന്യ;

    name = cs_resolve_file(cnumber, name);
    തിരികെ name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    സ്ഥിതം ശൂന്യം
cs_file_results(FILE *f, പൂർണ്ണസംഖ്യ *nummatches_a)
{
    പൂർണ്ണസംഖ്യ i, j;
    അക്ഷരം *buf;
    അക്ഷരം *തിരയൽ, *slno;
    അക്ഷരം *fullname;
    അക്ഷരം *cntx;
    അക്ഷരം *context;

    buf = alloc(CSREAD_BUFSIZE);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (i = 0; i < csinfo_size; i++)
    {
	എങ്കിൽ (nummatches_a[i] < 1)
	    തുടരുക;

	വേണ്ടി (j = 0; j < nummatches_a[i]; j++)
	{
	   എങ്കിൽ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &തിരയൽ)) == ശൂന്യ)
	       തുടരുക;

	   context = alloc(strlen(cntx)+5);
	   എങ്കിൽ (context == ശൂന്യ)
	   {
	       vim_free(fullname);
	       തുടരുക;
	   }

	   എങ്കിൽ (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   അല്ലെങ്കിൽ
	       sprintf(context, "<<%s>>", cntx);

	   എങ്കിൽ (തിരയൽ == ശൂന്യ)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   അല്ലെങ്കിൽ
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, തിരയൽ);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(ശൂന്യം)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    സ്ഥിതം ശൂന്യം
cs_fill_results(
    അക്ഷരം *tagstr,
    പൂർണ്ണസംഖ്യ totmatches,
    പൂർണ്ണസംഖ്യ *nummatches_a,
    അക്ഷരം ***matches_p,
    അക്ഷരം ***cntxts_p,
    പൂർണ്ണസംഖ്യ *matched)
{
    പൂർണ്ണസംഖ്യ i, j;
    അക്ഷരം *buf;
    അക്ഷരം *തിരയൽ, *slno;
    പൂർണ്ണസംഖ്യ totsofar = 0;
    അക്ഷരം **matches = ശൂന്യ;
    അക്ഷരം **cntxts = ശൂന്യ;
    അക്ഷരം *fullname;
    അക്ഷരം *cntx;

    പ്രതിജ്ഞാ(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ ((matches = ALLOC_MULT(അക്ഷരം *, totmatches)) == ശൂന്യ)
	പോകുക parse_out;
    എങ്കിൽ ((cntxts = ALLOC_MULT(അക്ഷരം *, totmatches)) == ശൂന്യ)
	പോകുക parse_out;

    വേണ്ടി (i = 0; i < csinfo_size; i++)
    {
	എങ്കിൽ (nummatches_a[i] < 1)
	    തുടരുക;

	വേണ്ടി (j = 0; j < nummatches_a[i]; j++)
	{
	   എങ്കിൽ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &തിരയൽ)) == ശൂന്യ)
		തുടരുക;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  തിരയൽ, tagstr);

	    vim_free(fullname);

	    എങ്കിൽ (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = ശൂന്യ;
	    അല്ലെങ്കിൽ
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (അക്ഷരം *)vim_strsave((char_u *)cntx);

	    എങ്കിൽ (matches[totsofar] != ശൂന്യ)
		totsofar++;

	} // for all matches

	(ശൂന്യം)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    എങ്കിൽ (totsofar == 0)
    {
	// No matches, free the arrays and return NULL in "*matches_p".
	VIM_CLEAR(matches);
	VIM_CLEAR(cntxts);
    }
    *matched = totsofar;
    *matches_p = matches;
    *cntxts_p = cntxts;

    vim_free(buf);
}


/*
 * get the requested path components
 */
    സ്ഥിതം അക്ഷരം *
cs_pathcomponents(അക്ഷരം *path)
{
    പൂർണ്ണസംഖ്യ		i;
    അക്ഷരം	*s;

    എങ്കിൽ (p_cspc == 0)
	തിരികെ path;

    s = path + strlen(path) - 1;
    വേണ്ടി (i = 0; i < p_cspc; ++i)
    {
	ആകുമ്പോൾ (s > path)
	{
	   s--;
	   എങ്കിൽ (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      നിർത്തുക;
	}
    }
    എങ്കിൽ ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    തിരികെ s;
}

/*
 * Called from cs_manage_matches().
 */
    സ്ഥിതം ശൂന്യം
cs_print_tags_priv(അക്ഷരം **matches, അക്ഷരം **cntxts, പൂർണ്ണസംഖ്യ അക്കം_matches)
{
    അക്ഷരം	*buf = ശൂന്യ;
    അക്ഷരം	*t_buf;
    പൂർണ്ണസംഖ്യ		bufsize = 0; // Track available bufsize
    പൂർണ്ണസംഖ്യ		newsize = 0;
    അക്ഷരം	*ptag;
    അക്ഷരം	*fname, *lno, *extra, *tbuf;
    പൂർണ്ണസംഖ്യ		i, idx, അക്കം;
    അക്ഷരം	*globalcntx = "GLOBAL";
    അക്ഷരം	*cntxformat = " <<%s>>";
    അക്ഷരം	*context;
    അക്ഷരം	*cstag_സന്ദേശം = _("Cscope tag: %s");
    അക്ഷരം	*csfmt_str = "%4d %6s  ";

    പ്രതിജ്ഞാ(അക്കം_matches > 0);

    എങ്കിൽ ((tbuf = alloc(strlen(matches[0]) + 1)) == ശൂന്യ)
	തിരികെ;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    എങ്കിൽ (ptag == ശൂന്യ)
    {
	vim_free(tbuf);
	തിരികെ;
    }

    newsize = (പൂർണ്ണസംഖ്യ)(strlen(cstag_സന്ദേശം) + strlen(ptag));
    buf = alloc(newsize);
    എങ്കിൽ (buf != ശൂന്യ)
    {
	bufsize = newsize;
	(ശൂന്യം)sprintf(buf, cstag_സന്ദേശം, ptag);
	സന്ദേശം_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    സന്ദേശം_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    സന്ദേശം_advance(സന്ദേശം_col + 2);
    സന്ദേശം_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    അക്കം = 1;
    വേണ്ടി (i = 0; i < അക്കം_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	എങ്കിൽ ((tbuf = alloc(strlen(matches[idx]) + 1)) == ശൂന്യ)
	    തുടരുക;
	(ശൂന്യം)strcpy(tbuf, matches[idx]);

	എങ്കിൽ (strtok(tbuf, (സ്ഥിരം അക്ഷരം *)"\t") == ശൂന്യ
		|| (fname = strtok(ശൂന്യ, (സ്ഥിരം അക്ഷരം *)"\t")) == ശൂന്യ
		|| (lno = strtok(ശൂന്യ, (സ്ഥിരം അക്ഷരം *)"\t")) == ശൂന്യ)
	{
	    vim_free(tbuf);
	    തുടരുക;
	}
	extra = strtok(ശൂന്യ, (സ്ഥിരം അക്ഷരം *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (പൂർണ്ണസംഖ്യ)(strlen(csfmt_str) + 16 + strlen(lno));
	എങ്കിൽ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    അല്ലെങ്കിൽ
		bufsize = newsize;
	}
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    // csfmt_str = "%4d %6s  ";
	    (ശൂന്യം)sprintf(buf, csfmt_str, അക്കം, lno);
	    സന്ദേശം_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	സന്ദേശം_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	എങ്കിൽ (cntxts[idx] != ശൂന്യ)
	    context = cntxts[idx];
	അല്ലെങ്കിൽ
	    context = globalcntx;
	newsize = (പൂർണ്ണസംഖ്യ)(strlen(context) + strlen(cntxformat));

	എങ്കിൽ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    അല്ലെങ്കിൽ
		bufsize = newsize;
	}
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    (ശൂന്യം)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    എങ്കിൽ (സന്ദേശം_col + (പൂർണ്ണസംഖ്യ)strlen(buf) >= cmdline_width)
		സന്ദേശം_putchar('\n');
	    സന്ദേശം_advance(12);
	    സന്ദേശം_outtrans_long_attr((char_u *)buf, 0);
	    സന്ദേശം_putchar('\n');
	}
	എങ്കിൽ (extra != ശൂന്യ)
	{
	    സന്ദേശം_advance(13);
	    സന്ദേശം_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	എങ്കിൽ (സന്ദേശം_col)
	    സന്ദേശം_putchar('\n');

	ui_breakcheck();
	എങ്കിൽ (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    നിർത്തുക;
	}

	അക്കം++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_read_prompt(പൂർണ്ണസംഖ്യ i)
{
    പൂർണ്ണസംഖ്യ		ch;
    അക്ഷരം	*buf = ശൂന്യ; // buffer for possible error message from cscope
    പൂർണ്ണസംഖ്യ		bufpos = 0;
    പൂർണ്ണസംഖ്യ		maxlen;
    സ്ഥിതം അക്ഷരം *eprompt = "Press the RETURN key to continue:";
    പൂർണ്ണസംഖ്യ		epromptlen = (പൂർണ്ണസംഖ്യ)strlen(eprompt);
    പൂർണ്ണസംഖ്യ		n;

    // compute maximum allowed len for Cscope error message
    അക്ഷരം *cs_emsg = _(e_cscope_error_str);
    maxlen = (പൂർണ്ണസംഖ്യ)(IOSIZE - strlen(cs_emsg));

    വേണ്ടി (;;)
    {
	ആകുമ്പോൾ ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    എങ്കിൽ (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		എങ്കിൽ (buf == ശൂന്യ) // lazy buffer allocation
		    buf = alloc(maxlen);
		എങ്കിൽ (buf != ശൂന്യ)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    എങ്കിൽ (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(ശൂന്യം)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(ശൂന്യം)putc('\n', csinfo[i].to_fp);
			(ശൂന്യം)fflush(csinfo[i].to_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	വേണ്ടി (n = 0; n < (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    എങ്കിൽ (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    എങ്കിൽ (ch == EOF)
	    {
		എങ്കിൽ (buf != ശൂന്യ && buf[0] != NUL)
		    (ശൂന്യം)semsg(cs_emsg, buf);
		അല്ലെങ്കിൽ എങ്കിൽ (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		തിരികെ CSCOPE_FAILURE;
	    }

	    എങ്കിൽ (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		നിർത്തുക;
	    }
	}

	എങ്കിൽ (ch == EOF)
	    തുടരുക;	    // didn't find the prompt
	നിർത്തുക;		    // did find the prompt
    }

    vim_free(buf);
    തിരികെ CSCOPE_SUCCESS;
}

# എങ്കിൽ defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    സ്ഥിതം ശൂന്യം
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    സ്ഥിതം ശൂന്യം
cs_release_csp(പൂർണ്ണസംഖ്യ i, പൂർണ്ണസംഖ്യ freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    എങ്കിൽ (csinfo[i].to_fp != ശൂന്യ)
    {
	(ശൂന്യം)fputs("q\n", csinfo[i].to_fp);
	(ശൂന്യം)fflush(csinfo[i].to_fp);
    }
# എങ്കിൽ defined(UNIX)
    {
	പൂർണ്ണസംഖ്യ waitpid_errno;
	പൂർണ്ണസംഖ്യ pstat;
	pid_t pid;

#  എങ്കിൽ defined(HAVE_SIGACTION)
	ഘടന sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   അല്ലെങ്കിൽ
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, ശൂന്യ);
#  അല്ലെങ്കിൽ
	പൂർണ്ണസംഖ്യ waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	വേണ്ടി (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    എങ്കിൽ (pid != 0)
		നിർത്തുക;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	എങ്കിൽ (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    പൂർണ്ണസംഖ്യ alive = TRUE;

	    എങ്കിൽ (waitpid_errno == ECHILD)
	    {
		/*
		 * When using 'vim -g', vim is forked and cscope process is
		 * no longer a child process but a sibling.  So waitpid()
		 * fails with errno being ECHILD (No child processes).
		 * Don't send SIGKILL to cscope immediately but wait
		 * (polling) for it to exit normally as result of sending
		 * the "q" command, hence giving it a chance to clean up
		 * its temporary files.
		 */
		പൂർണ്ണസംഖ്യ waited;

		sleep(0);
		വേണ്ടി (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    എങ്കിൽ (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			നിർത്തുക;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    എങ്കിൽ (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(ശൂന്യം)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# അല്ലെങ്കിൽ  // !UNIX
    എങ്കിൽ (csinfo[i].hProc != ശൂന്യ)
    {
	// Give cscope a chance to exit normally
	എങ്കിൽ (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    എങ്കിൽ (csinfo[i].fr_fp != ശൂന്യ)
	(ശൂന്യം)fclose(csinfo[i].fr_fp);
    എങ്കിൽ (csinfo[i].to_fp != ശൂന്യ)
	(ശൂന്യം)fclose(csinfo[i].to_fp);

    എങ്കിൽ (freefnpp)
    {
	vim_free(csinfo[i].fname);
	vim_free(csinfo[i].ppath);
	vim_free(csinfo[i].flags);
    }

    clear_csinfo(i);
}


/*
 * Calls cs_kill on all cscope connections then reinits.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_reset(exarg_T *eap UNUSED)
{
    അക്ഷരം	**dblist = ശൂന്യ, **pplist = ശൂന്യ, **fllist = ശൂന്യ;
    പൂർണ്ണസംഖ്യ	i;
    അക്ഷരം buf[20]; // for sprintf " (#%d)"

    എങ്കിൽ (csinfo_size == 0)
	തിരികെ CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(അക്ഷരം *, csinfo_size);
    pplist = ALLOC_MULT(അക്ഷരം *, csinfo_size);
    fllist = ALLOC_MULT(അക്ഷരം *, csinfo_size);
    എങ്കിൽ (dblist == ശൂന്യ || pplist == ശൂന്യ || fllist == ശൂന്യ)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	തിരികെ CSCOPE_FAILURE;
    }

    വേണ്ടി (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	എങ്കിൽ (csinfo[i].fname != ശൂന്യ)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    വേണ്ടി (i = 0; i < csinfo_size; i++)
    {
	എങ്കിൽ (dblist[i] != ശൂന്യ)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    എങ്കിൽ (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		സന്ദേശം_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    എങ്കിൽ (p_csverbose)
	സന്ദേശം_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | സന്ദേശം_HIST);
    തിരികെ CSCOPE_SUCCESS;
}


/*
 * Construct the full pathname to a file found in the cscope database.
 * (Prepends ppath, if there is one and if it's not already prepended,
 * otherwise just uses the name found.)
 *
 * We need to prepend the prefix because on some cscope's (e.g., the one that
 * ships with Solaris 2.6), the output never has the prefix prepended.
 * Contrast this with my development system (Digital Unix), which does.
 */
    സ്ഥിതം അക്ഷരം *
cs_resolve_file(പൂർണ്ണസംഖ്യ i, അക്ഷരം *name)
{
    അക്ഷരം	*fullname;
    string_T	csdir = {ശൂന്യ, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    പൂർണ്ണസംഖ്യ		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (പൂർണ്ണസംഖ്യ)namelen + 2;
    എങ്കിൽ (csinfo[i].ppath != ശൂന്യ)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (പൂർണ്ണസംഖ്യ)ppathlen;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p_csre && csinfo[i].fname != ശൂന്യ)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	എങ്കിൽ (csdir.string != ശൂന്യ)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (പൂർണ്ണസംഖ്യ)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    എങ്കിൽ (csinfo[i].ppath != ശൂന്യ
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	എങ്കിൽ ((fullname = alloc(len)) != ശൂന്യ)
	    (ശൂന്യം)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (csdir.string != ശൂന്യ && csinfo[i].fname != ശൂന്യ && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (അക്ഷരം *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    അല്ലെങ്കിൽ
    {
	fullname = (അക്ഷരം *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    തിരികെ fullname;
}


/*
 * Show all cscope connections.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cs_show(exarg_T *eap UNUSED)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (cs_cnt_connections() == 0)
	സന്ദേശം_puts(_("no cscope connections\n"));
    അല്ലെങ്കിൽ
    {
	സന്ദേശം_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	വേണ്ടി (i = 0; i < csinfo_size; i++)
	{
	    എങ്കിൽ (csinfo[i].fname == ശൂന്യ)
		തുടരുക;

	    എങ്കിൽ (csinfo[i].ppath != ശൂന്യ)
		(ശൂന്യം)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    അല്ലെങ്കിൽ
		(ശൂന്യം)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    തിരികെ CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    ശൂന്യം
cs_end(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    ശൂന്യം
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    പൂർണ്ണസംഖ്യ		അക്കം = 0;
    char_u	*dbpath = ശൂന്യ;
    char_u	*prepend = ശൂന്യ;
    char_u	buf[NUMBUFLEN];

    എങ്കിൽ (in_vim9script()
	    && (check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	അക്കം = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_സംഖ്യ = cs_connection(അക്കം, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
