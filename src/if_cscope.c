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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# ਜੇ defined(UNIX)
#  include <sys/wait.h>
# endif

# ਜੇ defined (MSWIN)
#  ifndef WIN32_LEAN_AND_ਔਸਤ
#   define WIN32_LEAN_AND_ਔਸਤ
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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ *  name;
    ਪੂਰਨ     (*func)(exarg_T *eap);
    ਅੱਖਰ *  help;
    ਅੱਖਰ *  usage;
    ਪੂਰਨ	    cansplit;		// if supports splitting window
} cscmd_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ csi {
    ਅੱਖਰ *	    fname;	// cscope db name
    ਅੱਖਰ *	    ppath;	// path to prepend (the -P option)
    ਅੱਖਰ *	    flags;	// additional cscope flags/options (e.g, -p2)
# ਜੇ defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# ਨਹੀਂਤਾਂ
#  ਜੇ defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_ਸੂਚਕਾਂਕ_ਉੱਚਾ;	// st_ino has no meaning in the Windows
    DWORD	    n_ਸੂਚਕਾਂਕ_ਨੀਵਾਂ;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    ਮੰਜ਼ਿਲ_fp;	// to cscope: FILE.
} csinfo_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ { ਜੋੜੋ_2, ਲੱਭੋ_2, Help, Kill, Reset, Show } csid_e;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

ਸਥਿਰ ਪੂਰਨ	    cs_ਜੋੜੋ(exarg_T *eap);
ਸਥਿਰ ਪੂਰਨ	    cs_ਜੋੜੋ_common(ਅੱਖਰ *, ਅੱਖਰ *, ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ	    cs_check_for_connections(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ	    cs_check_for_ਟੈਗ_ਸੂਚੀ(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ	    cs_cnt_connections(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ	    cs_create_connection(ਪੂਰਨ i);
# ifdef FEAT_QUICKFIX
ਸਥਿਰ ਖਾਲੀ	    cs_file_results(FILE *, ਪੂਰਨ *);
# endif
ਸਥਿਰ ਖਾਲੀ	    cs_fill_results(ਅੱਖਰ *, ਪੂਰਨ , ਪੂਰਨ *, ਅੱਖਰ ***,
			ਅੱਖਰ ***, ਪੂਰਨ *);
ਸਥਿਰ ਪੂਰਨ	    cs_ਲੱਭੋ(exarg_T *eap);
ਸਥਿਰ ਪੂਰਨ	    cs_ਲੱਭੋ_common(ਅੱਖਰ *opt, ਅੱਖਰ *pat, ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ, char_u *cmdline);
ਸਥਿਰ ਪੂਰਨ	    cs_help(exarg_T *eap);
ਸਥਿਰ ਪੂਰਨ	    cs_ਪਾਓ_filelist(ਅੱਖਰ *, ਅੱਖਰ *, ਅੱਖਰ *,
			stat_T *);
ਸਥਿਰ ਪੂਰਨ	    cs_kill(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	    cs_kill_execute(ਪੂਰਨ, ਅੱਖਰ *);
ਸਥਿਰ cscmd_T *    cs_lookup_cmd(exarg_T *eap);
ਸਥਿਰ ਅੱਖਰ *	    cs_make_vim_style_matches(ਅੱਖਰ *, ਅੱਖਰ *,
			ਅੱਖਰ *, ਅੱਖਰ *);
ਸਥਿਰ ਅੱਖਰ *	    cs_manage_matches(ਅੱਖਰ **, ਅੱਖਰ **, ਪੂਰਨ, mcmd_e);
ਸਥਿਰ ਖਾਲੀ	    cs_print_ਟੈਗ_ਸੂਚੀ_priv(ਅੱਖਰ **, ਅੱਖਰ **, ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ	    cs_read_prompt(ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ	    cs_release_csp(ਪੂਰਨ, ਪੂਰਨ freefnpp);
ਸਥਿਰ ਪੂਰਨ	    cs_reset(exarg_T *eap);
ਸਥਿਰ ਅੱਖਰ *	    cs_resolve_file(ਪੂਰਨ, ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ	    cs_show(exarg_T *eap);


ਸਥਿਰ csinfo_T *   csinfo = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	    csinfo_ਮਾਪ = 0;	// number of items allocated in
					// csinfo[]

ਸਥਿਰ ਪੂਰਨ	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
ਸਥਿਰ cscmd_T	    cs_cmds[] =
{
    { "add",	cs_ਜੋੜੋ,
		N_("Add a new database"),     "add file|dir [pre-path] [flags]", 0 },
    { "find",	cs_ਲੱਭੋ,
		N_("Query for a pattern"),    "find a|c|d|e|f|g|i|s|t name", 1 },
    { "help",	cs_help,
		N_("Show this message"),      "help", 0 },
    { "kill",	cs_kill,
		N_("Kill a connection"),      "kill #", 0 },
    { "reset",	cs_reset,
		N_("Reinit all connections"), "reset", 0 },
    { "show",	cs_show,
		N_("Show connections"),       "show", 0 },
    { ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, 0 }
};

    ਸਥਿਰ ਖਾਲੀ
cs_usage_ਸੁਨੇਹਾ(csid_e x)
{
    (ਖਾਲੀ)semsg(_(e_usage_cscope_str), cs_cmds[(ਪੂਰਨ)x].usage);
}

ਸਥਿਰ ਗਿਣਤੀ
{
    EXP_CSCOPE_SUBCMD,	// expand ":cscope" sub-commands
    EXP_SCSCOPE_SUBCMD,	// expand ":scscope" sub-commands
    EXP_CSCOPE_ਲੱਭੋ,	// expand ":cscope find" arguments
    EXP_CSCOPE_KILL	// expand ":cscope kill" arguments
} expand_what;

/*
 * Function given to ExpandGeneric() to obtain the cscope command
 * expansion.
 */
    char_u *
get_cscope_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਪੂਰਨ		current_idx;
    ਪੂਰਨ		i;

    ਚੋਣ (expand_what)
    {
    ਹਾਲਤ EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	ਵਾਪਸ (char_u *)cs_cmds[idx].name;
    ਹਾਲਤ EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	ਲਈ (i = 0, current_idx = 0; cs_cmds[i].name != ਨਿਰਾ; i++)
	    ਜੇ (cs_cmds[i].cansplit)
		ਜੇ (current_idx++ == idx)
		    ਰੋਕੋ;
	ਵਾਪਸ (char_u *)cs_cmds[i].name;
    ਹਾਲਤ EXP_CSCOPE_ਲੱਭੋ:
	{
	    ਅਚਲ ਅੱਖਰ *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", ਨਿਰਾ
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    ਵਾਪਸ (char_u *)query_type[idx];
	}
    ਹਾਲਤ EXP_CSCOPE_KILL:
	{
	    ਸਥਿਰ ਅੱਖਰ	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    ਲਈ (i = 0, current_idx = 0; i < csinfo_ਮਾਪ; i++)
	    {
		ਜੇ (csinfo[i].fname == ਨਿਰਾ)
		    ਜਾਰੀ;
		ਜੇ (current_idx++ == idx)
		{
		    vim_snprintf(connection, ਆਕਾਰ(connection), "%d", i);
		    ਵਾਪਸ (char_u *)connection;
		}
	    }
	    ਵਾਪਸ (current_idx == idx && idx > 0) ? (char_u *)"-1" : ਨਿਰਾ;
	}
    ਮੂਲ:
	ਵਾਪਸ ਨਿਰਾ;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    ਖਾਲੀ
ਸੈੱਟ_context_ਵਿੱਚ_cscope_cmd(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_CSCOPE;
    xp->xp_ਨਮੂਨਾ = arg;
    expand_what = (cmdidx == CMD_scscope)
			? EXP_SCSCOPE_SUBCMD : EXP_CSCOPE_SUBCMD;

    ਜੇ (*arg == NUL)
	ਵਾਪਸ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ਜੇ (*p == NUL)
	ਵਾਪਸ;

    // past first word
    xp->xp_ਨਮੂਨਾ = skipwhite(p);
    ਜੇ (*skiptowhite(xp->xp_ਨਮੂਨਾ) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_ਲੱਭੋ;
    ਨਹੀਂਤਾਂ
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    ਸਥਿਰ ਖਾਲੀ
do_cscope_general(
    exarg_T	*eap,
    ਪੂਰਨ		make_ਵੰਡੋ ਅਣਵਰਤਿਆ) // whether to split window
{
    cscmd_T *cmdp;

    ਜੇ (check_restricted())
	ਵਾਪਸ;

    ਜੇ ((cmdp = cs_lookup_cmd(eap)) == ਨਿਰਾ)
    {
	cs_help(eap);
	ਵਾਪਸ;
    }

    ਜੇ (make_ਵੰਡੋ)
    {
	ਜੇ (!cmdp->cansplit)
	{
	    (ਖਾਲੀ)ਸੁਨੇਹਾ_puts(_("This cscope command does not support splitting the window.\n"));
	    ਵਾਪਸ;
	}
	postponed_ਵੰਡੋ = -1;
	postponed_ਵੰਡੋ_flags = cmdmod.cmod_ਵੰਡੋ;
	postponed_ਵੰਡੋ_tab = cmdmod.cmod_tab;
    }

    cmdp->func(eap);

    postponed_ਵੰਡੋ_flags = 0;
    postponed_ਵੰਡੋ_tab = 0;
}

/*
 * Implementation of ":cscope" and ":lcscope"
 */
    ਖਾਲੀ
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    ਖਾਲੀ
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    ਖਾਲੀ
ex_cstag(exarg_T *eap)
{
    ਪੂਰਨ ret = FALSE;

    ਜੇ (*eap->arg == NUL)
    {
	(ਖਾਲੀ)emsg(_(e_usage_cstag_ident));
	ਵਾਪਸ;
    }

    ਚੋਣ (p_csto)
    {
    ਹਾਲਤ 0 :
	ਜੇ (cs_check_for_connections())
	{
	    ret = cs_ਲੱਭੋ_common("g", (ਅੱਖਰ *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ਜੇ (ret == FALSE)
	    {
		cs_free_ਟੈਗ_ਸੂਚੀ();
		ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ)
		    ਸੁਨੇਹਾ_putchar('\n');

		ਜੇ (cs_check_for_ਟੈਗ_ਸੂਚੀ())
		    ret = do_ਟੈਗ(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (cs_check_for_ਟੈਗ_ਸੂਚੀ())
	{
	    ret = do_ਟੈਗ(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	ਰੋਕੋ;
    ਹਾਲਤ 1 :
	ਜੇ (cs_check_for_ਟੈਗ_ਸੂਚੀ())
	{
	    ret = do_ਟੈਗ(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    ਜੇ (ret == FALSE)
	    {
		ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ)
		    ਸੁਨੇਹਾ_putchar('\n');

		ਜੇ (cs_check_for_connections())
		{
		    ret = cs_ਲੱਭੋ_common("g", (ਅੱਖਰ *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    ਜੇ (ret == FALSE)
			cs_free_ਟੈਗ_ਸੂਚੀ();
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (cs_check_for_connections())
	{
	    ret = cs_ਲੱਭੋ_common("g", (ਅੱਖਰ *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ਜੇ (ret == FALSE)
		cs_free_ਟੈਗ_ਸੂਚੀ();
	}
	ਰੋਕੋ;
    ਮੂਲ :
	ਰੋਕੋ;
    }

    ਜੇ (!ret)
    {
	(ਖਾਲੀ)emsg(_(e_cstag_ਟੈਗ_not_founc));
# ਜੇ defined(FEAT_QUICKFIX)
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
    ਪੂਰਨ
cs_fgets(char_u *buf, ਪੂਰਨ ਮਾਪ)
{
    ਅੱਖਰ *p;

    ਜੇ ((p = cs_manage_matches(ਨਿਰਾ, ਨਿਰਾ, -1, Get)) == ਨਿਰਾ)
	ਵਾਪਸ TRUE;
    vim_strncpy(buf, (char_u *)p, ਮਾਪ - 1);

    ਵਾਪਸ FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    ਖਾਲੀ
cs_free_ਟੈਗ_ਸੂਚੀ(ਖਾਲੀ)
{
    cs_manage_matches(ਨਿਰਾ, ਨਿਰਾ, -1, Free);
}


/*
 * Called from do_tag().
 */
    ਖਾਲੀ
cs_print_ਟੈਗ_ਸੂਚੀ(ਖਾਲੀ)
{
    cs_manage_matches(ਨਿਰਾ, ਨਿਰਾ, -1, Print);
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
# ਜੇ defined(FEAT_EVAL)
    ਸਥਿਰ ਪੂਰਨ
cs_connection(ਪੂਰਨ ਅੰਕ, char_u *dbpath, char_u *ppath)
{
    ਪੂਰਨ i;

    ਜੇ (ਅੰਕ < 0 || ਅੰਕ > 4 || (ਅੰਕ > 0 && !dbpath))
	ਵਾਪਸ FALSE;

    ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
    {
	ਜੇ (!csinfo[i].fname)
	    ਜਾਰੀ;

	ਜੇ (ਅੰਕ == 0)
	    ਵਾਪਸ TRUE;

	ਚੋਣ (ਅੰਕ)
	{
	ਹਾਲਤ 1:
	    ਜੇ (ਲੜੀ_ਖੋਜ(csinfo[i].fname, (ਅੱਖਰ *)dbpath))
		ਵਾਪਸ TRUE;
	    ਰੋਕੋ;
	ਹਾਲਤ 2:
	    ਜੇ (strcmp(csinfo[i].fname, (ਅੱਖਰ *)dbpath) == 0)
		ਵਾਪਸ TRUE;
	    ਰੋਕੋ;
	ਹਾਲਤ 3:
	    ਜੇ (ਲੜੀ_ਖੋਜ(csinfo[i].fname, (ਅੱਖਰ *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && ਲੜੀ_ਖੋਜ(csinfo[i].ppath, (ਅੱਖਰ *)ppath))))
		ਵਾਪਸ TRUE;
	    ਰੋਕੋ;
	ਹਾਲਤ 4:
	    ਜੇ ((strcmp(csinfo[i].fname, (ਅੱਖਰ *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (ਅੱਖਰ *)ppath) == 0))))
		ਵਾਪਸ TRUE;
	    ਰੋਕੋ;
	}
    }

    ਵਾਪਸ FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    ਸਥਿਰ ਪੂਰਨ
cs_ਜੋੜੋ(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *fname, *ppath, *flags = ਨਿਰਾ;

    ਜੇ ((fname = strtok((ਅੱਖਰ *)ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)" ")) == ਨਿਰਾ)
    {
	cs_usage_ਸੁਨੇਹਾ(ਜੋੜੋ_2);
	ਵਾਪਸ CSCOPE_FAILURE;
    }
    ਜੇ ((ppath = strtok((ਅੱਖਰ *)ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)" ")) != ਨਿਰਾ)
	flags = strtok((ਅੱਖਰ *)ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)" ");

    ਵਾਪਸ cs_ਜੋੜੋ_common(fname, ppath, flags);
}

    ਸਥਿਰ ਖਾਲੀ
cs_stat_emsg(ਅੱਖਰ *fname)
{
    ਪੂਰਨ err = errno;
    (ਖਾਲੀ)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    ਸਥਿਰ ਪੂਰਨ
cs_ਜੋੜੋ_common(
    ਅੱਖਰ *arg1,	    // filename - may contain environment variables
    ਅੱਖਰ *arg2,	    // prepend path - may contain environment variables
    ਅੱਖਰ *flags)
{
    stat_T	statbuf;
    ਪੂਰਨ		ret;
    ਅੱਖਰ	*fname = ਨਿਰਾ;
    ਅੱਖਰ	*fname2 = ਨਿਰਾ;
    ਅੱਖਰ	*ppath = ਨਿਰਾ;
    ਪੂਰਨ		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = ਨਿਰਾ;

    // get the filename (arg1), expand it, and try to stat it
    ਜੇ ((fname = alloc(MAXPATHL + 1)) == ਨਿਰਾ)
	ਜਾਓ ਜੋੜੋ_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (ਖਾਲੀ)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    ਜੇ (fname == ਨਿਰਾ)
	ਜਾਓ ਜੋੜੋ_err;
    fname = (ਅੱਖਰ *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    ਜੇ (ret < 0)
    {
staterr:
	ਜੇ (p_csverbose)
	    cs_stat_emsg(fname);
	ਜਾਓ ਜੋੜੋ_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    ਜੇ (arg2 != ਨਿਰਾ)
    {
	stat_T	    statbuf2;

	ਜੇ ((ppath = alloc(MAXPATHL + 1)) == ਨਿਰਾ)
	    ਜਾਓ ਜੋੜੋ_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	ਜੇ (ret < 0)
	    ਜਾਓ staterr;
    }

    // if filename is a directory, append the cscope database name to it
    ਜੇ (S_ISDIR(statbuf.st_ਬਹੁਲਕ))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + ਲੜੀ_ਲੰਬਾਈ(fname) + 2);
	ਜੇ (fname2 == ਨਿਰਾ)
	    ਜਾਓ ਜੋੜੋ_err;

	ਜਦੋਂ (fname[ਲੜੀ_ਲੰਬਾਈ(fname)-1] == '/'
# ifdef MSWIN
		|| fname[ਲੜੀ_ਲੰਬਾਈ(fname)-1] == '\\'
# endif
		)
	{
	    fname[ਲੜੀ_ਲੰਬਾਈ(fname)-1] = '\0';
	    ਜੇ (fname[0] == '\0')
		ਰੋਕੋ;
	}
	ਜੇ (fname[0] == '\0')
	    (ਖਾਲੀ)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	ਨਹੀਂਤਾਂ
	    (ਖਾਲੀ)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	ਜੇ (ret < 0)
	{
	    ਜੇ (p_csverbose)
		cs_stat_emsg(fname2);
	    ਜਾਓ ਜੋੜੋ_err;
	}

	i = cs_ਪਾਓ_filelist(fname2, ppath, flags, &statbuf);
    }
    ਨਹੀਂਤਾਂ ਜੇ (S_ISREG(statbuf.st_ਬਹੁਲਕ) || S_ISLNK(statbuf.st_ਬਹੁਲਕ))
    {
	i = cs_ਪਾਓ_filelist(fname, ppath, flags, &statbuf);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (p_csverbose)
	    (ਖਾਲੀ)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	ਜਾਓ ਜੋੜੋ_err;
    }

    ਜੇ (i != -1)
    {
	ਜੇ (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    ਜਾਓ ਜੋੜੋ_err;
	}

	ਜੇ (p_csverbose)
	{
	    ਸੁਨੇਹਾ_clr_eos();
	    (ਖਾਲੀ)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    ਵਾਪਸ CSCOPE_SUCCESS;

ਜੋੜੋ_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    ਵਾਪਸ CSCOPE_FAILURE;
}


    ਸਥਿਰ ਪੂਰਨ
cs_check_for_connections(ਖਾਲੀ)
{
    ਵਾਪਸ (cs_cnt_connections() > 0);
}


    ਸਥਿਰ ਪੂਰਨ
cs_check_for_ਟੈਗ_ਸੂਚੀ(ਖਾਲੀ)
{
    ਵਾਪਸ (p_ਟੈਗ_ਸੂਚੀ[0] != NUL && curbuf->b_p_ਟੈਗ_ਸੂਚੀ != ਨਿਰਾ);
}


/*
 * Count the number of cscope connections.
 */
    ਸਥਿਰ ਪੂਰਨ
cs_cnt_connections(ਖਾਲੀ)
{
    ਪੂਰਨ i;
    ਪੂਰਨ cnt = 0;

    ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
	ਜੇ (csinfo[i].fname != ਨਿਰਾ)
	    cnt++;
    ਵਾਪਸ cnt;
}

    ਸਥਿਰ ਖਾਲੀ
cs_reading_emsg(
    ਪੂਰਨ idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    ਸਥਿਰ ਪੂਰਨ
cs_cnt_matches(ਪੂਰਨ idx)
{
    ਅੱਖਰ *stok;
    ਅੱਖਰ *buf;
    ਪੂਰਨ nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ 0;
    ਲਈ (;;)
    {
	ਜੇ (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    ਜੇ (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    ਵਾਪਸ -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	ਜੇ (ਲੜੀ_ਖੋਜ((ਅਚਲ ਅੱਖਰ *)buf, "Unable to search database") != ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜੇ ((stok = strtok(buf, (ਅਚਲ ਅੱਖਰ *)" ")) == ਨਿਰਾ)
	    ਜਾਰੀ;
	ਜੇ (ਲੜੀ_ਖੋਜ((ਅਚਲ ਅੱਖਰ *)stok, "cscope:") == ਨਿਰਾ)
	    ਜਾਰੀ;

	ਜੇ ((stok = strtok(ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)" ")) == ਨਿਰਾ)
	    ਜਾਰੀ;
	nlines = atoi(stok);
	ਜੇ (nlines < 0)
	{
	    nlines = 0;
	    ਰੋਕੋ;
	}

	ਜੇ ((stok = strtok(ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)" ")) == ਨਿਰਾ)
	    ਜਾਰੀ;
	ਜੇ (ਲੜੀ_ਤੁਲਨਾ((ਅਚਲ ਅੱਖਰ *)stok, "lines", 5))
	    ਜਾਰੀ;

	ਰੋਕੋ;
    }

    vim_free(buf);
    ਵਾਪਸ nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    ਸਥਿਰ ਅੱਖਰ *
cs_create_cmd(ਅੱਖਰ *csoption, ਅੱਖਰ *ਨਮੂਨਾ)
{
    ਅੱਖਰ *cmd;
    ਛੋਟਾ ਖੋਜ;
    ਅੱਖਰ *pat;

    ਚੋਣ (csoption[0])
    {
    ਹਾਲਤ '0' : ਹਾਲਤ 's' :
	ਖੋਜ = 0;
	ਰੋਕੋ;
    ਹਾਲਤ '1' : ਹਾਲਤ 'g' :
	ਖੋਜ = 1;
	ਰੋਕੋ;
    ਹਾਲਤ '2' : ਹਾਲਤ 'd' :
	ਖੋਜ = 2;
	ਰੋਕੋ;
    ਹਾਲਤ '3' : ਹਾਲਤ 'c' :
	ਖੋਜ = 3;
	ਰੋਕੋ;
    ਹਾਲਤ '4' : ਹਾਲਤ 't' :
	ਖੋਜ = 4;
	ਰੋਕੋ;
    ਹਾਲਤ '6' : ਹਾਲਤ 'e' :
	ਖੋਜ = 6;
	ਰੋਕੋ;
    ਹਾਲਤ '7' : ਹਾਲਤ 'f' :
	ਖੋਜ = 7;
	ਰੋਕੋ;
    ਹਾਲਤ '8' : ਹਾਲਤ 'i' :
	ਖੋਜ = 8;
	ਰੋਕੋ;
    ਹਾਲਤ '9' : ਹਾਲਤ 'a' :
	ਖੋਜ = 9;
	ਰੋਕੋ;
    ਮੂਲ :
	(ਖਾਲੀ)emsg(_(e_unknown_cscope_ਖੋਜ_type));
	cs_usage_ਸੁਨੇਹਾ(ਲੱਭੋ_2);
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = ਨਮੂਨਾ;
    ਜੇ (ਖੋਜ != 4 && ਖੋਜ != 6)
	ਜਦੋਂ VIM_ISWHITE(*pat)
	    ++pat;

    ਜੇ ((cmd = alloc(ਲੜੀ_ਲੰਬਾਈ(pat) + 2)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    (ਖਾਲੀ)sprintf(cmd, "%d%s", ਖੋਜ, pat);

    ਵਾਪਸ cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    ਸਥਿਰ ਪੂਰਨ
cs_create_connection(ਪੂਰਨ i)
{
# ifdef UNIX
    ਪੂਰਨ		ਮੰਜ਼ਿਲ_cs[2], from_cs[2];
# endif
    ਪੂਰਨ		cmdlen;
    ਅੱਖਰ	*prog, *cmd, *ppath = ਨਿਰਾ;
    size_t	proglen;
# ifdef MSWIN
    ਪੂਰਨ		fd;
    ਪੂਰਨ		len;
    SECURITY_ਗੁਣ sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# ਜੇ defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    ਮੰਜ਼ਿਲ_cs[0] = ਮੰਜ਼ਿਲ_cs[1] = from_cs[0] = from_cs[1] = -1;
    ਜੇ (pipe(ਮੰਜ਼ਿਲ_cs) < 0 || pipe(from_cs) < 0)
    {
	(ਖਾਲੀ)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	ਜੇ (ਮੰਜ਼ਿਲ_cs[0] != -1)
	    (ਖਾਲੀ)close(ਮੰਜ਼ਿਲ_cs[0]);
	ਜੇ (ਮੰਜ਼ਿਲ_cs[1] != -1)
	    (ਖਾਲੀ)close(ਮੰਜ਼ਿਲ_cs[1]);
	ਜੇ (from_cs[0] != -1)
	    (ਖਾਲੀ)close(from_cs[0]);
	ਜੇ (from_cs[1] != -1)
	    (ਖਾਲੀ)close(from_cs[1]);
	ਵਾਪਸ CSCOPE_FAILURE;
    }

    ਜੇ ((csinfo[i].pid = fork()) == -1)
    {
	(ਖਾਲੀ)emsg(_(e_could_not_fork_for_cscope));
	ਜਾਓ err_closing;
    }
    ਨਹੀਂਤਾਂ ਜੇ (csinfo[i].pid == 0)	// child: run cscope.
    {
	ਅੱਖਰ **argv = ਨਿਰਾ;

	ਜੇ (dup2(ਮੰਜ਼ਿਲ_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	ਜੇ (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	ਜੇ (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(ਖਾਲੀ)close(ਮੰਜ਼ਿਲ_cs[1]);
	(ਖਾਲੀ)close(from_cs[0]);
# ਨਹੀਂਤਾਂ
	// MSWIN
	// Create pipes to communicate with cscope
	sa.n_ਲੰਮਾਈ = ਆਕਾਰ(SECURITY_ਗੁਣ);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ਨਿਰਾ;

	ਜੇ (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (ਖਾਲੀ)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    ਜੇ (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    ਜੇ (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    ਵਾਪਸ CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	ਜੇ ((prog = alloc(MAXPATHL + 1)) == ਨਿਰਾ)
	{
# ifdef UNIX
	    ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
# ਨਹੀਂਤਾਂ
	    // MSWIN
	    ਜਾਓ err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (ਪੂਰਨ)(proglen + ਲੜੀ_ਲੰਬਾਈ(csinfo[i].fname) + 32);
	ਜੇ (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    ਜੇ ((ppath = alloc(MAXPATHL + 1)) == ਨਿਰਾ)
	    {
		vim_free(prog);
# ifdef UNIX
		ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
# ਨਹੀਂਤਾਂ
		// MSWIN
		ਜਾਓ err_closing;
# endif
	    }
	    cmdlen += (ਪੂਰਨ)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	ਜੇ (csinfo[i].flags)
	    cmdlen += (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(csinfo[i].flags);

	ਜੇ ((cmd = alloc(cmdlen)) == ਨਿਰਾ)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
# ਨਹੀਂਤਾਂ
	    // MSWIN
	    ਜਾਓ err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    ਅੱਖਰ	**tok = ਨਿਰਾ;
	    ਪੂਰਨ		tokc = 0;

	    ga_init2(&ga_argv, ਆਕਾਰ(ਅੱਖਰ *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    ਜੇ (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
	    ਲਈ (ਪੂਰਨ k = 0; k < tokc; ++k)
	    {
		ਜੇ (ga_grow(&ga_argv, 1) == FAIL)
		    ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
		((ਅੱਖਰ **)ga_argv.ga_ਅੰਕੜੇ)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    ਜੇ (ga_grow(&ga_argv, 3) == FAIL)
		ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
	    ((ਅੱਖਰ **)ga_argv.ga_ਅੰਕੜੇ)[ga_argv.ga_len++] =
				    (ਅੱਖਰ *)vim_strsave((char_u *)"-dl");
	    ((ਅੱਖਰ **)ga_argv.ga_ਅੰਕੜੇ)[ga_argv.ga_len++] =
				    (ਅੱਖਰ *)vim_strsave((char_u *)"-f");
	    ((ਅੱਖਰ **)ga_argv.ga_ਅੰਕੜੇ)[ga_argv.ga_len++] =
				(ਅੱਖਰ *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    ਜੇ (ppath != ਨਿਰਾ)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		ਅੱਖਰ	*parg = alloc(plen);

		ਜੇ (parg == ਨਿਰਾ || ga_grow(&ga_argv, 1) == FAIL)
		    ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((ਅੱਖਰ **)ga_argv.ga_ਅੰਕੜੇ)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    ਜੇ (csinfo[i].flags != ਨਿਰਾ)
	    {
		ਜੇ (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
		ਲਈ (ਪੂਰਨ k = 0; k < tokc; ++k)
		{
		    ਜੇ (ga_grow(&ga_argv, 1) == FAIL)
			ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
		    ((ਅੱਖਰ **)ga_argv.ga_ਅੰਕੜੇ)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    ਜੇ (ga_grow(&ga_argv, 1) == FAIL)
		ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
	    ((ਅੱਖਰ **)ga_argv.ga_ਅੰਕੜੇ)[ga_argv.ga_len] = ਨਿਰਾ;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  ਜੇ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   ਜੇ defined(HAVE_SETSID)
	    (ਖਾਲੀ)setsid();
#   ਨਹੀਂਤਾਂ
	    ਜੇ (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (ਅੱਖਰ **)ga_argv.ga_ਅੰਕੜੇ;
	    ਜੇ (argv[0] == ਨਿਰਾ || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    ਬਾਹਰ_ਨਿਕਲੋ(127);
	    // NOTREACHED
	}
    }
    ਨਹੀਂਤਾਂ	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	ਜੇ ((csinfo[i].ਮੰਜ਼ਿਲ_fp = fdopen(ਮੰਜ਼ਿਲ_cs[1], "w")) == ਨਿਰਾ)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	ਜੇ ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == ਨਿਰਾ)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(ਖਾਲੀ)close(ਮੰਜ਼ਿਲ_cs[0]);
	(ਖਾਲੀ)close(from_cs[1]);
    }
# ਨਹੀਂਤਾਂ
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    ਜੇ (csinfo[i].ppath != ਨਿਰਾ)
    {
	len = (ਪੂਰਨ)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    ਜੇ (csinfo[i].flags != ਨਿਰਾ)
    {
	len = (ਪੂਰਨ)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.w_Show_ਖਿੜਕੀ = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_ਨਿਵੇਸ਼  = stdin_rd;
    created = CreateProcess(ਨਿਰਾ, cmd, ਨਿਰਾ, ਨਿਰਾ, TRUE, CREATE_NEW_CONSOLE,
							ਨਿਰਾ, ਨਿਰਾ, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    ਜੇ (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(ਖਾਲੀ)emsg(_(e_could_not_spawn_cscope_process));
	ਜਾਓ err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    ਜੇ (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      O_ਪਾਠ|_O_APPEND)) < 0)
	    || ((csinfo[i].ਮੰਜ਼ਿਲ_fp = _fdopen(fd, "w")) == ਨਿਰਾ))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    ਜੇ (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      O_ਪਾਠ|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == ਨਿਰਾ))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    ਵਾਪਸ CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    ਸਥਿਰ ਪੂਰਨ
cs_ਲੱਭੋ(exarg_T *eap)
{
    ਅੱਖਰ *opt, *pat;
    ਪੂਰਨ i;

    ਜੇ (cs_check_for_connections() == FALSE)
    {
	(ਖਾਲੀ)emsg(_(e_no_cscope_connections));
	ਵਾਪਸ FALSE;
    }

    ਜੇ ((opt = strtok((ਅੱਖਰ *)ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)" ")) == ਨਿਰਾ)
    {
	cs_usage_ਸੁਨੇਹਾ(ਲੱਭੋ_2);
	ਵਾਪਸ FALSE;
    }

    pat = opt + ਲੜੀ_ਲੰਬਾਈ(opt) + 1;
    ਜੇ (pat >= (ਅੱਖਰ *)eap->arg + eap_arg_len)
    {
	cs_usage_ਸੁਨੇਹਾ(ਲੱਭੋ_2);
	ਵਾਪਸ FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    ਲਈ (i = 0; i < eap_arg_len; ++i)
	ਜੇ (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    ਵਾਪਸ cs_ਲੱਭੋ_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    ਸਥਿਰ ਪੂਰਨ
cs_ਲੱਭੋ_common(
    ਅੱਖਰ *opt,
    ਅੱਖਰ *pat,
    ਪੂਰਨ forceit,
    ਪੂਰਨ verbose,
    ਪੂਰਨ	use_ll ਅਣਵਰਤਿਆ,
    char_u *cmdline ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ i;
    ਅੱਖਰ *cmd;
    ਪੂਰਨ *nummatches;
    ਪੂਰਨ totmatches;
# ifdef FEAT_QUICKFIX
    ਅੱਖਰ cmdletter;
    ਅੱਖਰ *qfpos;

    // get cmd letter
    ਚੋਣ (opt[0])
    {
    ਹਾਲਤ '0' :
	cmdletter = 's';
	ਰੋਕੋ;
    ਹਾਲਤ '1' :
	cmdletter = 'g';
	ਰੋਕੋ;
    ਹਾਲਤ '2' :
	cmdletter = 'd';
	ਰੋਕੋ;
    ਹਾਲਤ '3' :
	cmdletter = 'c';
	ਰੋਕੋ;
    ਹਾਲਤ '4' :
	cmdletter = 't';
	ਰੋਕੋ;
    ਹਾਲਤ '6' :
	cmdletter = 'e';
	ਰੋਕੋ;
    ਹਾਲਤ '7' :
	cmdletter = 'f';
	ਰੋਕੋ;
    ਹਾਲਤ '8' :
	cmdletter = 'i';
	ਰੋਕੋ;
    ਹਾਲਤ '9' :
	cmdletter = 'a';
	ਰੋਕੋ;
    ਮੂਲ :
	cmdletter = opt[0];
    }

    qfpos = (ਅੱਖਰ *)vim_strchr(p_csqf, cmdletter);
    ਜੇ (qfpos != ਨਿਰਾ)
    {
	qfpos++;
	// next symbol must be + or -
	ਜੇ (strchr(CSQF_FLAGS, *qfpos) == ਨਿਰਾ)
	{
	    (ਖਾਲੀ)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    ਵਾਪਸ FALSE;
	}

	ਜੇ (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    ਜੇ (aborting())
		ਵਾਪਸ FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    ਜੇ (cmd == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    nummatches = ALLOC_MULT(ਪੂਰਨ, csinfo_ਮਾਪ);
    ਜੇ (nummatches == ਨਿਰਾ)
    {
	vim_free(cmd);
	ਵਾਪਸ FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
	nummatches[i] = 0;
    totmatches = 0;
    ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
    {
	ਜੇ (csinfo[i].fname == ਨਿਰਾ || csinfo[i].ਮੰਜ਼ਿਲ_fp == ਨਿਰਾ)
	    ਜਾਰੀ;

	// send cmd to cscope
	(ਖਾਲੀ)fprintf(csinfo[i].ਮੰਜ਼ਿਲ_fp, "%s\n", cmd);
	(ਖਾਲੀ)fflush(csinfo[i].ਮੰਜ਼ਿਲ_fp);

	nummatches[i] = cs_cnt_matches(i);

	ਜੇ (nummatches[i] > -1)
	    totmatches += nummatches[i];

	ਜੇ (nummatches[i] == 0)
	    (ਖਾਲੀ)cs_read_prompt(i);
    }
    vim_free(cmd);

    ਜੇ (totmatches == 0)
    {
	ਜੇ (verbose)
	    (ਖਾਲੀ)semsg(_(e_no_matches_ਮਿਲਿਆ_for_cscope_query_str_ਦਾ_str),
								     opt, pat);
	vim_free(nummatches);
	ਵਾਪਸ FALSE;
    }

# ifdef FEAT_QUICKFIX
    ਜੇ (qfpos != ਨਿਰਾ && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = ਨਿਰਾ;
	win_T	    *wp = ਨਿਰਾ;

	ਜੇ (tmp == ਨਿਰਾ)
	{
	    vim_free(nummatches);
	    ਵਾਪਸ FALSE;
	}

	f = mch_fopen((ਅੱਖਰ *)tmp, "w");
	ਜੇ (f == ਨਿਰਾ)
	    semsg(_(e_cant_open_file_str), tmp);
	ਨਹੀਂਤਾਂ
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    ਜੇ (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    ਜੇ (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, ਨਿਰਾ) > 0)
	    {
		ਜੇ (postponed_ਵੰਡੋ != 0)
		{
		    (ਖਾਲੀ)win_ਵੰਡੋ(postponed_ਵੰਡੋ > 0 ? postponed_ਵੰਡੋ : 0,
						       postponed_ਵੰਡੋ_flags);
		    RESET_BINDING(curwin);
		    postponed_ਵੰਡੋ = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		ਜੇ (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_ਬਫਰ) && wp->w_llist_ref != ਨਿਰਾ)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	ਵਾਪਸ TRUE;
    }
    ਨਹੀਂਤਾਂ
# endif // FEAT_QUICKFIX
    {
	ਅੱਖਰ **matches = ਨਿਰਾ, **contexts = ਨਿਰਾ;
	ਪੂਰਨ matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	ਜੇ (matches == ਨਿਰਾ)
	    ਵਾਪਸ FALSE;

	(ਖਾਲੀ)cs_manage_matches(matches, contexts, matched, Store);

	ਵਾਪਸ do_ਟੈਗ((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    ਸਥਿਰ ਪੂਰਨ
cs_help(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    cscmd_T *cmdp = cs_cmds;

    (ਖਾਲੀ)ਸੁਨੇਹਾ_puts(_("cscope commands:\n"));
    ਜਦੋਂ (cmdp->name != ਨਿਰਾ)
    {
	ਅੱਖਰ *help = _(cmdp->help);
	ਪੂਰਨ  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	ਜੇ (space_cnt < 0)
	    space_cnt = 0;
	(ਖਾਲੀ)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	ਜੇ (strcmp(cmdp->name, "find") == 0)
	    ਸੁਨੇਹਾ_puts(_("\n"
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
    ਵਾਪਸ 0;
}


    ਸਥਿਰ ਖਾਲੀ
clear_csinfo(ਪੂਰਨ i)
{
    csinfo[i].fname  = ਨਿਰਾ;
    csinfo[i].ppath  = ਨਿਰਾ;
    csinfo[i].flags  = ਨਿਰਾ;
# ਜੇ defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# ਨਹੀਂਤਾਂ
    csinfo[i].nVolume = 0;
    csinfo[i].n_ਸੂਚਕਾਂਕ_ਉੱਚਾ = 0;
    csinfo[i].n_ਸੂਚਕਾਂਕ_ਨੀਵਾਂ = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = ਨਿਰਾ;
    csinfo[i].ਮੰਜ਼ਿਲ_fp  = ਨਿਰਾ;
# ਜੇ defined(MSWIN)
    csinfo[i].hProc = ਨਿਰਾ;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    ਸਥਿਰ ਪੂਰਨ
cs_ਪਾਓ_filelist(
    ਅੱਖਰ *fname,
    ਅੱਖਰ *ppath,
    ਅੱਖਰ *flags,
    stat_T *sb ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    ਚੋਣ (win32_fileinfo((char_u *)fname, &bhfi))
    {
	ਹਾਲਤ FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	ਹਾਲਤ FILEINFO_READ_FAIL:	// CreateFile() failed
	    ਜੇ (p_csverbose)
	    {
		ਅੱਖਰ *cant_ਸੁਨੇਹਾ = _(e_cannot_open_cscope_database_str);
		ਅੱਖਰ *winmsg = GetWin32Error();

		ਜੇ (winmsg != ਨਿਰਾ)
		    (ਖਾਲੀ)semsg(cant_ਸੁਨੇਹਾ, winmsg);
		ਨਹੀਂਤਾਂ
		    // subst filename if can't get error text
		    (ਖਾਲੀ)semsg(cant_ਸੁਨੇਹਾ, fname);
	    }
	    ਵਾਪਸ -1;

	ਹਾਲਤ FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    ਜੇ (p_csverbose)
		(ਖਾਲੀ)emsg(_(e_cannot_get_cscope_database_information));
	    ਵਾਪਸ -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    ਲਈ (j = 0; j < csinfo_ਮਾਪ; j++)
    {
	ਜੇ (csinfo[j].fname != ਨਿਰਾ
# ਜੇ defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# ਨਹੀਂਤਾਂ
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_ਸੰਖਿਆ
		    && csinfo[j].n_ਸੂਚਕਾਂਕ_ਉੱਚਾ == bhfi.n_File_ਸੂਚਕਾਂਕ_ਉੱਚਾ
		    && csinfo[j].n_ਸੂਚਕਾਂਕ_ਨੀਵਾਂ == bhfi.n_File_ਸੂਚਕਾਂਕ_ਨੀਵਾਂ))
# endif
	    )
	{
	    ਜੇ (p_csverbose)
		(ਖਾਲੀ)emsg(_(e_duplicate_cscope_database_not_added));
	    ਵਾਪਸ -1;
	}

	ਜੇ (csinfo[j].fname == ਨਿਰਾ && i == -1)
	    i = j; // remember first empty entry
    }

    ਜੇ (i == -1)
    {
	i = csinfo_ਮਾਪ;
	ਜੇ (csinfo_ਮਾਪ == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_ਮਾਪ = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	ਨਹੀਂਤਾਂ
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_ਮਾਪ *= 2;
	    csinfo = vim_realloc(csinfo, ਆਕਾਰ(csinfo_T)*csinfo_ਮਾਪ);
	    ਜੇ (csinfo == ਨਿਰਾ)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_ਮਾਪ = i;
		ਵਾਪਸ -1;
	    }
	}
	ਜੇ (csinfo == ਨਿਰਾ)
	    ਵਾਪਸ -1;
	ਲਈ (j = csinfo_ਮਾਪ/2; j < csinfo_ਮਾਪ; j++)
	    clear_csinfo(j);
    }

    ਜੇ ((csinfo[i].fname = alloc(ਲੜੀ_ਲੰਬਾਈ(fname)+1)) == ਨਿਰਾ)
	ਵਾਪਸ -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, ਲੜੀ_ਲੰਬਾਈ((ਅਚਲ ਅੱਖਰ *)fname));

    ਜੇ (ppath != ਨਿਰਾ)
    {
	ਜੇ ((csinfo[i].ppath = alloc(ਲੜੀ_ਲੰਬਾਈ(ppath) + 1)) == ਨਿਰਾ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    ਵਾਪਸ -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, ਲੜੀ_ਲੰਬਾਈ((ਅਚਲ ਅੱਖਰ *)ppath));
    }
    ਨਹੀਂਤਾਂ
	csinfo[i].ppath = ਨਿਰਾ;

    ਜੇ (flags != ਨਿਰਾ)
    {
	ਜੇ ((csinfo[i].flags = alloc(ਲੜੀ_ਲੰਬਾਈ(flags) + 1)) == ਨਿਰਾ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    ਵਾਪਸ -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, ਲੜੀ_ਲੰਬਾਈ((ਅਚਲ ਅੱਖਰ *)flags));
    }
    ਨਹੀਂਤਾਂ
	csinfo[i].flags = ਨਿਰਾ;

# ਜੇ defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# ਨਹੀਂਤਾਂ
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_ਸੰਖਿਆ;
    csinfo[i].n_ਸੂਚਕਾਂਕ_ਨੀਵਾਂ = bhfi.n_File_ਸੂਚਕਾਂਕ_ਨੀਵਾਂ;
    csinfo[i].n_ਸੂਚਕਾਂਕ_ਉੱਚਾ = bhfi.n_File_ਸੂਚਕਾਂਕ_ਉੱਚਾ;
# endif
    ਵਾਪਸ i;
}


/*
 * Find cscope command in command table.
 */
    ਸਥਿਰ cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    ਅੱਖਰ *stok;
    size_t len;

    ਜੇ (eap->arg == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (ਪੂਰਨ)STRLEN(eap->arg);

    ਜੇ ((stok = strtok((ਅੱਖਰ *)(eap->arg), (ਅਚਲ ਅੱਖਰ *)" ")) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    len = ਲੜੀ_ਲੰਬਾਈ(stok);
    ਲਈ (cmdp = cs_cmds; cmdp->name != ਨਿਰਾ; ++cmdp)
    {
	ਜੇ (ਲੜੀ_ਤੁਲਨਾ((ਅਚਲ ਅੱਖਰ *)(stok), cmdp->name, len) == 0)
	    ਵਾਪਸ (cmdp);
    }
    ਵਾਪਸ ਨਿਰਾ;
}


/*
 * Nuke em.
 */
    ਸਥਿਰ ਪੂਰਨ
cs_kill(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *stok;
    ਪੂਰਨ i;

    ਜੇ ((stok = strtok((ਅੱਖਰ *)ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)" ")) == ਨਿਰਾ)
    {
	cs_usage_ਸੁਨੇਹਾ(Kill);
	ਵਾਪਸ CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    ਜੇ ((ਲੜੀ_ਲੰਬਾਈ(stok) < 2 && VIM_ISDIGIT((ਪੂਰਨ)(stok[0])))
	    || (ਲੜੀ_ਲੰਬਾਈ(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((ਪੂਰਨ)(stok[1]))))
	i = atoi(stok);
    ਨਹੀਂਤਾਂ
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
	{
	    ਜੇ (csinfo[i].fname != ਨਿਰਾ && ਲੜੀ_ਖੋਜ(csinfo[i].fname, stok))
		ਰੋਕੋ;
	}
    }

    ਜੇ ((i != -1) && (i >= csinfo_ਮਾਪ || i < -1 || csinfo[i].fname == ਨਿਰਾ))
    {
	ਜੇ (p_csverbose)
	    (ਖਾਲੀ)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (i == -1)
	{
	    ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
	    {
		ਜੇ (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	ਨਹੀਂਤਾਂ
	    cs_kill_execute(i, stok);
    }

    ਵਾਪਸ 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    ਸਥਿਰ ਖਾਲੀ
cs_kill_execute(
    ਪੂਰਨ i,	    // cscope table index
    ਅੱਖਰ *cname)    // cscope database name
{
    ਜੇ (p_csverbose)
    {
	ਸੁਨੇਹਾ_clr_eos();
	(ਖਾਲੀ)smsg_attr(HL_ATTR(HLF_R) | ਸੁਨੇਹਾ_HIST,
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
    ਸਥਿਰ ਅੱਖਰ *
cs_make_vim_style_matches(
    ਅੱਖਰ *fname,
    ਅੱਖਰ *slno,
    ਅੱਖਰ *ਖੋਜ,
    ਅੱਖਰ *tagstr)
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
    ਅੱਖਰ *buf;
    ਪੂਰਨ amt;

    ਜੇ (ਖੋਜ != ਨਿਰਾ)
    {
	amt = (ਪੂਰਨ)(ਲੜੀ_ਲੰਬਾਈ(fname) + ਲੜੀ_ਲੰਬਾਈ(slno) + ਲੜੀ_ਲੰਬਾਈ(tagstr) + ਲੜੀ_ਲੰਬਾਈ(ਖੋਜ)+6);
	ਜੇ ((buf = alloc(amt)) == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	(ਖਾਲੀ)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, ਖੋਜ);
    }
    ਨਹੀਂਤਾਂ
    {
	amt = (ਪੂਰਨ)(ਲੜੀ_ਲੰਬਾਈ(fname) + ਲੜੀ_ਲੰਬਾਈ(slno) + ਲੜੀ_ਲੰਬਾਈ(tagstr) + 5);
	ਜੇ ((buf = alloc(amt)) == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	(ਖਾਲੀ)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    ਵਾਪਸ buf;
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
    ਸਥਿਰ ਅੱਖਰ *
cs_manage_matches(
    ਅੱਖਰ **matches,
    ਅੱਖਰ **contexts,
    ਪੂਰਨ totmatches,
    mcmd_e cmd)
{
    ਸਥਿਰ ਅੱਖਰ **mp = ਨਿਰਾ;
    ਸਥਿਰ ਅੱਖਰ **cp = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ cnt = -1;
    ਸਥਿਰ ਪੂਰਨ ਅਗਲਾ = -1;
    ਅੱਖਰ *p = ਨਿਰਾ;

    ਚੋਣ (cmd)
    {
    ਹਾਲਤ Store:
	ਪ੍ਰਤਿਜ੍ਞਾ(matches != ਨਿਰਾ);
	ਪ੍ਰਤਿਜ੍ਞਾ(totmatches > 0);
	ਜੇ (mp != ਨਿਰਾ || cp != ਨਿਰਾ)
	    (ਖਾਲੀ)cs_manage_matches(ਨਿਰਾ, ਨਿਰਾ, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	ਅਗਲਾ = 0;
	ਰੋਕੋ;
    ਹਾਲਤ Get:
	ਜੇ (ਅਗਲਾ >= cnt)
	    ਵਾਪਸ ਨਿਰਾ;

	p = mp[ਅਗਲਾ];
	ਅਗਲਾ++;
	ਰੋਕੋ;
    ਹਾਲਤ Free:
	ਜੇ (mp != ਨਿਰਾ)
	{
	    ਜੇ (cnt > 0)
		ਜਦੋਂ (cnt--)
		{
		    vim_free(mp[cnt]);
		    ਜੇ (cp != ਨਿਰਾ)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = ਨਿਰਾ;
	cp = ਨਿਰਾ;
	cnt = 0;
	ਅਗਲਾ = 0;
	ਰੋਕੋ;
    ਹਾਲਤ Print:
	cs_print_ਟੈਗ_ਸੂਚੀ_priv(mp, cp, cnt);
	ਰੋਕੋ;
    ਮੂਲ:	// should not reach here
	iemsg(e_fatal_error_ਵਿੱਚ_cs_manage_matches);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ p;
}


/*
 * Parse cscope output.
 */
    ਸਥਿਰ ਅੱਖਰ *
cs_parse_results(
    ਪੂਰਨ cnumber,
    ਅੱਖਰ *buf,
    ਪੂਰਨ bufsize,
    ਅੱਖਰ **context,
    ਅੱਖਰ **linenumber,
    ਅੱਖਰ **ਖੋਜ)
{
    ਪੂਰਨ ch;
    ਅੱਖਰ *p;
    ਅੱਖਰ *name;

    ਜੇ (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == ਨਿਰਾ)
    {
	ਜੇ (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	ਵਾਪਸ ਨਿਰਾ;
    }

    // If the line's too long for the buffer, discard it.
    ਜੇ ((p = strchr(buf, '\n')) == ਨਿਰਾ)
    {
	ਜਦੋਂ ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	ਵਾਪਸ ਨਿਰਾ;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    ਜੇ ((name = strtok(buf, (ਅਚਲ ਅੱਖਰ *)" ")) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ ((*context = strtok(ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)" ")) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ ((*linenumber = strtok(ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)" ")) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    *ਖੋਜ = *linenumber + ਲੜੀ_ਲੰਬਾਈ(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    ਜੇ (strcmp(*ਖੋਜ, "<unknown>") == 0)
	*ਖੋਜ = ਨਿਰਾ;

    name = cs_resolve_file(cnumber, name);
    ਵਾਪਸ name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    ਸਥਿਰ ਖਾਲੀ
cs_file_results(FILE *f, ਪੂਰਨ *nummatches_a)
{
    ਪੂਰਨ i, j;
    ਅੱਖਰ *buf;
    ਅੱਖਰ *ਖੋਜ, *slno;
    ਅੱਖਰ *fullname;
    ਅੱਖਰ *cntx;
    ਅੱਖਰ *context;

    buf = alloc(CSREAD_BUFSIZE);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
    {
	ਜੇ (nummatches_a[i] < 1)
	    ਜਾਰੀ;

	ਲਈ (j = 0; j < nummatches_a[i]; j++)
	{
	   ਜੇ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &ਖੋਜ)) == ਨਿਰਾ)
	       ਜਾਰੀ;

	   context = alloc(ਲੜੀ_ਲੰਬਾਈ(cntx)+5);
	   ਜੇ (context == ਨਿਰਾ)
	   {
	       vim_free(fullname);
	       ਜਾਰੀ;
	   }

	   ਜੇ (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   ਨਹੀਂਤਾਂ
	       sprintf(context, "<<%s>>", cntx);

	   ਜੇ (ਖੋਜ == ਨਿਰਾ)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   ਨਹੀਂਤਾਂ
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, ਖੋਜ);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(ਖਾਲੀ)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    ਸਥਿਰ ਖਾਲੀ
cs_fill_results(
    ਅੱਖਰ *tagstr,
    ਪੂਰਨ totmatches,
    ਪੂਰਨ *nummatches_a,
    ਅੱਖਰ ***matches_p,
    ਅੱਖਰ ***cntxts_p,
    ਪੂਰਨ *matched)
{
    ਪੂਰਨ i, j;
    ਅੱਖਰ *buf;
    ਅੱਖਰ *ਖੋਜ, *slno;
    ਪੂਰਨ totsofar = 0;
    ਅੱਖਰ **matches = ਨਿਰਾ;
    ਅੱਖਰ **cntxts = ਨਿਰਾ;
    ਅੱਖਰ *fullname;
    ਅੱਖਰ *cntx;

    ਪ੍ਰਤਿਜ੍ਞਾ(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ ((matches = ALLOC_MULT(ਅੱਖਰ *, totmatches)) == ਨਿਰਾ)
	ਜਾਓ parse_ਬਾਹਰਲਾ;
    ਜੇ ((cntxts = ALLOC_MULT(ਅੱਖਰ *, totmatches)) == ਨਿਰਾ)
	ਜਾਓ parse_ਬਾਹਰਲਾ;

    ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
    {
	ਜੇ (nummatches_a[i] < 1)
	    ਜਾਰੀ;

	ਲਈ (j = 0; j < nummatches_a[i]; j++)
	{
	   ਜੇ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &ਖੋਜ)) == ਨਿਰਾ)
		ਜਾਰੀ;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  ਖੋਜ, tagstr);

	    vim_free(fullname);

	    ਜੇ (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (ਅੱਖਰ *)vim_strsave((char_u *)cntx);

	    ਜੇ (matches[totsofar] != ਨਿਰਾ)
		totsofar++;

	} // for all matches

	(ਖਾਲੀ)cs_read_prompt(i);

    } // for all cscope connections

parse_ਬਾਹਰਲਾ:
    ਜੇ (totsofar == 0)
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
    ਸਥਿਰ ਅੱਖਰ *
cs_pathcomponents(ਅੱਖਰ *ਰਸਤਾ)
{
    ਪੂਰਨ		i;
    ਅੱਖਰ	*s;

    ਜੇ (p_cspc == 0)
	ਵਾਪਸ ਰਸਤਾ;

    s = ਰਸਤਾ + ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ) - 1;
    ਲਈ (i = 0; i < p_cspc; ++i)
    {
	ਜਦੋਂ (s > ਰਸਤਾ)
	{
	   s--;
	   ਜੇ (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      ਰੋਕੋ;
	}
    }
    ਜੇ ((s > ਰਸਤਾ && *s == '/')
# ifdef MSWIN
	|| (s > ਰਸਤਾ && *s == '\\')
# endif
	    )
	++s;
    ਵਾਪਸ s;
}

/*
 * Called from cs_manage_matches().
 */
    ਸਥਿਰ ਖਾਲੀ
cs_print_ਟੈਗ_ਸੂਚੀ_priv(ਅੱਖਰ **matches, ਅੱਖਰ **cntxts, ਪੂਰਨ ਅੰਕ_matches)
{
    ਅੱਖਰ	*buf = ਨਿਰਾ;
    ਅੱਖਰ	*t_buf;
    ਪੂਰਨ		bufsize = 0; // Track available bufsize
    ਪੂਰਨ		newsize = 0;
    ਅੱਖਰ	*ptag;
    ਅੱਖਰ	*fname, *lno, *extra, *tbuf;
    ਪੂਰਨ		i, idx, ਅੰਕ;
    ਅੱਖਰ	*globalcntx = "GLOBAL";
    ਅੱਖਰ	*cntxformat = " <<%s>>";
    ਅੱਖਰ	*context;
    ਅੱਖਰ	*cstag_ਸੁਨੇਹਾ = _("Cscope tag: %s");
    ਅੱਖਰ	*csfmt_str = "%4d %6s  ";

    ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕ_matches > 0);

    ਜੇ ((tbuf = alloc(ਲੜੀ_ਲੰਬਾਈ(matches[0]) + 1)) == ਨਿਰਾ)
	ਵਾਪਸ;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    ਜੇ (ptag == ਨਿਰਾ)
    {
	vim_free(tbuf);
	ਵਾਪਸ;
    }

    newsize = (ਪੂਰਨ)(ਲੜੀ_ਲੰਬਾਈ(cstag_ਸੁਨੇਹਾ) + ਲੜੀ_ਲੰਬਾਈ(ptag));
    buf = alloc(newsize);
    ਜੇ (buf != ਨਿਰਾ)
    {
	bufsize = newsize;
	(ਖਾਲੀ)sprintf(buf, cstag_ਸੁਨੇਹਾ, ptag);
	ਸੁਨੇਹਾ_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    ਸੁਨੇਹਾ_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    ਸੁਨੇਹਾ_advance(ਸੁਨੇਹਾ_ਖਾਨਾ + 2);
    ਸੁਨੇਹਾ_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    ਅੰਕ = 1;
    ਲਈ (i = 0; i < ਅੰਕ_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	ਜੇ ((tbuf = alloc(ਲੜੀ_ਲੰਬਾਈ(matches[idx]) + 1)) == ਨਿਰਾ)
	    ਜਾਰੀ;
	(ਖਾਲੀ)strcpy(tbuf, matches[idx]);

	ਜੇ (strtok(tbuf, (ਅਚਲ ਅੱਖਰ *)"\t") == ਨਿਰਾ
		|| (fname = strtok(ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)"\t")) == ਨਿਰਾ
		|| (lno = strtok(ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)"\t")) == ਨਿਰਾ)
	{
	    vim_free(tbuf);
	    ਜਾਰੀ;
	}
	extra = strtok(ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)"\t");

	lno[ਲੜੀ_ਲੰਬਾਈ(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (ਪੂਰਨ)(ਲੜੀ_ਲੰਬਾਈ(csfmt_str) + 16 + ਲੜੀ_ਲੰਬਾਈ(lno));
	ਜੇ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ਨਹੀਂਤਾਂ
		bufsize = newsize;
	}
	ਜੇ (buf != ਨਿਰਾ)
	{
	    // csfmt_str = "%4d %6s  ";
	    (ਖਾਲੀ)sprintf(buf, csfmt_str, ਅੰਕ, lno);
	    ਸੁਨੇਹਾ_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	ਸੁਨੇਹਾ_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	ਜੇ (cntxts[idx] != ਨਿਰਾ)
	    context = cntxts[idx];
	ਨਹੀਂਤਾਂ
	    context = globalcntx;
	newsize = (ਪੂਰਨ)(ਲੜੀ_ਲੰਬਾਈ(context) + ਲੜੀ_ਲੰਬਾਈ(cntxformat));

	ਜੇ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ਨਹੀਂਤਾਂ
		bufsize = newsize;
	}
	ਜੇ (buf != ਨਿਰਾ)
	{
	    (ਖਾਲੀ)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ + (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(buf) >= cmdline_ਚੌੜਾਈ)
		ਸੁਨੇਹਾ_putchar('\n');
	    ਸੁਨੇਹਾ_advance(12);
	    ਸੁਨੇਹਾ_outtrans_long_attr((char_u *)buf, 0);
	    ਸੁਨੇਹਾ_putchar('\n');
	}
	ਜੇ (extra != ਨਿਰਾ)
	{
	    ਸੁਨੇਹਾ_advance(13);
	    ਸੁਨੇਹਾ_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ)
	    ਸੁਨੇਹਾ_putchar('\n');

	ui_breakcheck();
	ਜੇ (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    ਰੋਕੋ;
	}

	ਅੰਕ++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    ਸਥਿਰ ਪੂਰਨ
cs_read_prompt(ਪੂਰਨ i)
{
    ਪੂਰਨ		ch;
    ਅੱਖਰ	*buf = ਨਿਰਾ; // buffer for possible error message from cscope
    ਪੂਰਨ		bufpos = 0;
    ਪੂਰਨ		maxlen;
    ਸਥਿਰ ਅੱਖਰ *eprompt = "Press the RETURN key to continue:";
    ਪੂਰਨ		epromptlen = (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(eprompt);
    ਪੂਰਨ		n;

    // compute maximum allowed len for Cscope error message
    ਅੱਖਰ *cs_emsg = _(e_cscope_error_str);
    maxlen = (ਪੂਰਨ)(IOSIZE - ਲੜੀ_ਲੰਬਾਈ(cs_emsg));

    ਲਈ (;;)
    {
	ਜਦੋਂ ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    ਜੇ (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		ਜੇ (buf == ਨਿਰਾ) // lazy buffer allocation
		    buf = alloc(maxlen);
		ਜੇ (buf != ਨਿਰਾ)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    ਜੇ (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(ਖਾਲੀ)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(ਖਾਲੀ)putc('\n', csinfo[i].ਮੰਜ਼ਿਲ_fp);
			(ਖਾਲੀ)fflush(csinfo[i].ਮੰਜ਼ਿਲ_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	ਲਈ (n = 0; n < (ਪੂਰਨ)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    ਜੇ (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    ਜੇ (ch == EOF)
	    {
		ਜੇ (buf != ਨਿਰਾ && buf[0] != NUL)
		    (ਖਾਲੀ)semsg(cs_emsg, buf);
		ਨਹੀਂਤਾਂ ਜੇ (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		ਵਾਪਸ CSCOPE_FAILURE;
	    }

	    ਜੇ (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		ਰੋਕੋ;
	    }
	}

	ਜੇ (ch == EOF)
	    ਜਾਰੀ;	    // didn't find the prompt
	ਰੋਕੋ;		    // did find the prompt
    }

    vim_free(buf);
    ਵਾਪਸ CSCOPE_SUCCESS;
}

# ਜੇ defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    ਸਥਿਰ ਖਾਲੀ
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    ਸਥਿਰ ਖਾਲੀ
cs_release_csp(ਪੂਰਨ i, ਪੂਰਨ freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    ਜੇ (csinfo[i].ਮੰਜ਼ਿਲ_fp != ਨਿਰਾ)
    {
	(ਖਾਲੀ)fputs("q\n", csinfo[i].ਮੰਜ਼ਿਲ_fp);
	(ਖਾਲੀ)fflush(csinfo[i].ਮੰਜ਼ਿਲ_fp);
    }
# ਜੇ defined(UNIX)
    {
	ਪੂਰਨ waitpid_errno;
	ਪੂਰਨ pstat;
	pid_t pid;

#  ਜੇ defined(HAVE_SIGACTION)
	ਬਣਤਰ sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   ਨਹੀਂਤਾਂ
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, ਨਿਰਾ);
#  ਨਹੀਂਤਾਂ
	ਪੂਰਨ waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	ਲਈ (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    ਜੇ (pid != 0)
		ਰੋਕੋ;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	ਜੇ (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    ਪੂਰਨ alive = TRUE;

	    ਜੇ (waitpid_errno == ECHILD)
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
		ਪੂਰਨ waited;

		sleep(0);
		ਲਈ (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    ਜੇ (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			ਰੋਕੋ;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    ਜੇ (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(ਖਾਲੀ)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# ਨਹੀਂਤਾਂ  // !UNIX
    ਜੇ (csinfo[i].hProc != ਨਿਰਾ)
    {
	// Give cscope a chance to exit normally
	ਜੇ (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    ਜੇ (csinfo[i].fr_fp != ਨਿਰਾ)
	(ਖਾਲੀ)fclose(csinfo[i].fr_fp);
    ਜੇ (csinfo[i].ਮੰਜ਼ਿਲ_fp != ਨਿਰਾ)
	(ਖਾਲੀ)fclose(csinfo[i].ਮੰਜ਼ਿਲ_fp);

    ਜੇ (freefnpp)
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
    ਸਥਿਰ ਪੂਰਨ
cs_reset(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ	**dblist = ਨਿਰਾ, **pplist = ਨਿਰਾ, **fllist = ਨਿਰਾ;
    ਪੂਰਨ	i;
    ਅੱਖਰ buf[20]; // for sprintf " (#%d)"

    ਜੇ (csinfo_ਮਾਪ == 0)
	ਵਾਪਸ CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(ਅੱਖਰ *, csinfo_ਮਾਪ);
    pplist = ALLOC_MULT(ਅੱਖਰ *, csinfo_ਮਾਪ);
    fllist = ALLOC_MULT(ਅੱਖਰ *, csinfo_ਮਾਪ);
    ਜੇ (dblist == ਨਿਰਾ || pplist == ਨਿਰਾ || fllist == ਨਿਰਾ)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	ਵਾਪਸ CSCOPE_FAILURE;
    }

    ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	ਜੇ (csinfo[i].fname != ਨਿਰਾ)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
    {
	ਜੇ (dblist[i] != ਨਿਰਾ)
	{
	    cs_ਜੋੜੋ_common(dblist[i], pplist[i], fllist[i]);
	    ਜੇ (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		ਸੁਨੇਹਾ_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    ਜੇ (p_csverbose)
	ਸੁਨੇਹਾ_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | ਸੁਨੇਹਾ_HIST);
    ਵਾਪਸ CSCOPE_SUCCESS;
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
    ਸਥਿਰ ਅੱਖਰ *
cs_resolve_file(ਪੂਰਨ i, ਅੱਖਰ *name)
{
    ਅੱਖਰ	*fullname;
    string_T	csdir = {ਨਿਰਾ, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    ਪੂਰਨ		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (ਪੂਰਨ)namelen + 2;
    ਜੇ (csinfo[i].ppath != ਨਿਰਾ)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (ਪੂਰਨ)ppathlen;
    }
    ਨਹੀਂਤਾਂ ਜੇ (p_csre && csinfo[i].fname != ਨਿਰਾ)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	ਜੇ (csdir.string != ਨਿਰਾ)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.ਲੰਮਾਈ = STRLEN(csdir.string);
	    len += (ਪੂਰਨ)csdir.ਲੰਮਾਈ;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    ਜੇ (csinfo[i].ppath != ਨਿਰਾ
	    && (ਲੜੀ_ਤੁਲਨਾ(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	ਜੇ ((fullname = alloc(len)) != ਨਿਰਾ)
	    (ਖਾਲੀ)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    ਨਹੀਂਤਾਂ ਜੇ (csdir.string != ਨਿਰਾ && csinfo[i].fname != ਨਿਰਾ && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (ਅੱਖਰ *)concat_fnames(csdir.string, csdir.ਲੰਮਾਈ, (char_u *)name, namelen, TRUE, &ret);
    }
    ਨਹੀਂਤਾਂ
    {
	fullname = (ਅੱਖਰ *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    ਵਾਪਸ fullname;
}


/*
 * Show all cscope connections.
 */
    ਸਥਿਰ ਪੂਰਨ
cs_show(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ i;

    ਜੇ (cs_cnt_connections() == 0)
	ਸੁਨੇਹਾ_puts(_("no cscope connections\n"));
    ਨਹੀਂਤਾਂ
    {
	ਸੁਨੇਹਾ_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
	{
	    ਜੇ (csinfo[i].fname == ਨਿਰਾ)
		ਜਾਰੀ;

	    ਜੇ (csinfo[i].ppath != ਨਿਰਾ)
		(ਖਾਲੀ)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (ਲੰਮਾ)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    ਨਹੀਂਤਾਂ
		(ਖਾਲੀ)smsg("%2d %-5ld  %-34s  <none>",
			   i, (ਲੰਮਾ)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    ਵਾਪਸ CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    ਖਾਲੀ
cs_ਸਿਰਾ(ਖਾਲੀ)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < csinfo_ਮਾਪ; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_ਮਾਪ = 0;
}

#ਜੇ_ਅੰਤ	// FEAT_CSCOPE

#ਜੇ defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    ਖਾਲੀ
f_cscope_connection(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_CSCOPE
    ਪੂਰਨ		ਅੰਕ = 0;
    char_u	*dbpath = ਨਿਰਾ;
    char_u	*prepend = ਨਿਰਾ;
    char_u	buf[NUMBUFLEN];

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	ਅੰਕ = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_ਸੰਖਿਆ = cs_connection(ਅੰਕ, dbpath, prepend);
# endif
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
