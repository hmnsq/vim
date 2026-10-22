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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# ఒకవేళ defined(UNIX)
#  include <sys/wait.h>
# endif

# ఒకవేళ defined (MSWIN)
#  ifndef WIN32_LEAN_AND_సగటు
#   define WIN32_LEAN_AND_సగటు
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

రకనిర్వచనం నిర్మాణం {
    అక్షరం *  name;
    పూర్ణాంకం     (*func)(exarg_T *eap);
    అక్షరం *  help;
    అక్షరం *  usage;
    పూర్ణాంకం	    cansplit;		// if supports splitting window
} cscmd_T;

రకనిర్వచనం నిర్మాణం csi {
    అక్షరం *	    fname;	// cscope db name
    అక్షరం *	    ppath;	// path to prepend (the -P option)
    అక్షరం *	    flags;	// additional cscope flags/options (e.g, -p2)
# ఒకవేళ defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# లేకపోతే
#  ఒకవేళ defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_సూచీ_ఎగువ;	// st_ino has no meaning in the Windows
    DWORD	    n_సూచీ_దిగువ;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    గమ్యం_fp;	// to cscope: FILE.
} csinfo_T;

రకనిర్వచనం గణన { కూడు_2, కనుగొను_2, Help, Kill, Reset, Show } csid_e;

రకనిర్వచనం గణన {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

స్థిర పూర్ణాంకం	    cs_కూడు(exarg_T *eap);
స్థిర పూర్ణాంకం	    cs_కూడు_common(అక్షరం *, అక్షరం *, అక్షరం *);
స్థిర పూర్ణాంకం	    cs_check_for_connections(శూన్య);
స్థిర పూర్ణాంకం	    cs_check_for_ట్యాగులు(శూన్య);
స్థిర పూర్ణాంకం	    cs_cnt_connections(శూన్య);
స్థిర పూర్ణాంకం	    cs_create_connection(పూర్ణాంకం i);
# ifdef FEAT_QUICKFIX
స్థిర శూన్య	    cs_file_results(FILE *, పూర్ణాంకం *);
# endif
స్థిర శూన్య	    cs_fill_results(అక్షరం *, పూర్ణాంకం , పూర్ణాంకం *, అక్షరం ***,
			అక్షరం ***, పూర్ణాంకం *);
స్థిర పూర్ణాంకం	    cs_కనుగొను(exarg_T *eap);
స్థిర పూర్ణాంకం	    cs_కనుగొను_common(అక్షరం *opt, అక్షరం *pat, పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం, char_u *cmdline);
స్థిర పూర్ణాంకం	    cs_help(exarg_T *eap);
స్థిర పూర్ణాంకం	    cs_చొప్పించు_filelist(అక్షరం *, అక్షరం *, అక్షరం *,
			stat_T *);
స్థిర పూర్ణాంకం	    cs_kill(exarg_T *eap);
స్థిర శూన్య	    cs_kill_execute(పూర్ణాంకం, అక్షరం *);
స్థిర cscmd_T *    cs_lookup_cmd(exarg_T *eap);
స్థిర అక్షరం *	    cs_make_vim_style_matches(అక్షరం *, అక్షరం *,
			అక్షరం *, అక్షరం *);
స్థిర అక్షరం *	    cs_manage_matches(అక్షరం **, అక్షరం **, పూర్ణాంకం, mcmd_e);
స్థిర శూన్య	    cs_print_ట్యాగులు_priv(అక్షరం **, అక్షరం **, పూర్ణాంకం);
స్థిర పూర్ణాంకం	    cs_read_prompt(పూర్ణాంకం);
స్థిర శూన్య	    cs_release_csp(పూర్ణాంకం, పూర్ణాంకం freefnpp);
స్థిర పూర్ణాంకం	    cs_reset(exarg_T *eap);
స్థిర అక్షరం *	    cs_resolve_file(పూర్ణాంకం, అక్షరం *);
స్థిర పూర్ణాంకం	    cs_show(exarg_T *eap);


స్థిర csinfo_T *   csinfo = ఖాళీ;
స్థిర పూర్ణాంకం	    csinfo_కొలత = 0;	// number of items allocated in
					// csinfo[]

స్థిర పూర్ణాంకం	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
స్థిర cscmd_T	    cs_cmds[] =
{
    { "add",	cs_కూడు,
		N_("Add a new database"),     "add file|dir [pre-path] [flags]", 0 },
    { "find",	cs_కనుగొను,
		N_("Query for a pattern"),    "find a|c|d|e|f|g|i|s|t name", 1 },
    { "help",	cs_help,
		N_("Show this message"),      "help", 0 },
    { "kill",	cs_kill,
		N_("Kill a connection"),      "kill #", 0 },
    { "reset",	cs_reset,
		N_("Reinit all connections"), "reset", 0 },
    { "show",	cs_show,
		N_("Show connections"),       "show", 0 },
    { ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, 0 }
};

    స్థిర శూన్య
cs_usage_సందేశం(csid_e x)
{
    (శూన్య)semsg(_(e_usage_cscope_str), cs_cmds[(పూర్ణాంకం)x].usage);
}

స్థిర గణన
{
    EXP_CSCOPE_SUBCMD,	// expand ":cscope" sub-commands
    EXP_SCSCOPE_SUBCMD,	// expand ":scscope" sub-commands
    EXP_CSCOPE_కనుగొను,	// expand ":cscope find" arguments
    EXP_CSCOPE_KILL	// expand ":cscope kill" arguments
} expand_what;

/*
 * Function given to ExpandGeneric() to obtain the cscope command
 * expansion.
 */
    char_u *
get_cscope_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    పూర్ణాంకం		current_idx;
    పూర్ణాంకం		i;

    ఎంపిక (expand_what)
    {
    స్థితి EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	తిరిగి (char_u *)cs_cmds[idx].name;
    స్థితి EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	కొరకు (i = 0, current_idx = 0; cs_cmds[i].name != ఖాళీ; i++)
	    ఒకవేళ (cs_cmds[i].cansplit)
		ఒకవేళ (current_idx++ == idx)
		    ఆపు;
	తిరిగి (char_u *)cs_cmds[i].name;
    స్థితి EXP_CSCOPE_కనుగొను:
	{
	    స్థిరరాశి అక్షరం *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", ఖాళీ
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    తిరిగి (char_u *)query_type[idx];
	}
    స్థితి EXP_CSCOPE_KILL:
	{
	    స్థిర అక్షరం	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    కొరకు (i = 0, current_idx = 0; i < csinfo_కొలత; i++)
	    {
		ఒకవేళ (csinfo[i].fname == ఖాళీ)
		    కొనసాగు;
		ఒకవేళ (current_idx++ == idx)
		{
		    vim_snprintf(connection, పరిమాణం(connection), "%d", i);
		    తిరిగి (char_u *)connection;
		}
	    }
	    తిరిగి (current_idx == idx && idx > 0) ? (char_u *)"-1" : ఖాళీ;
	}
    అప్రమేయ:
	తిరిగి ఖాళీ;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    శూన్య
సమితి_context_లోపల_cscope_cmd(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_CSCOPE;
    xp->xp_నమూనా = arg;
    expand_what = (cmdidx == CMD_scscope)
			? EXP_SCSCOPE_SUBCMD : EXP_CSCOPE_SUBCMD;

    ఒకవేళ (*arg == NUL)
	తిరిగి;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ఒకవేళ (*p == NUL)
	తిరిగి;

    // past first word
    xp->xp_నమూనా = skipwhite(p);
    ఒకవేళ (*skiptowhite(xp->xp_నమూనా) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_కనుగొను;
    లేకపోతే
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    స్థిర శూన్య
do_cscope_general(
    exarg_T	*eap,
    పూర్ణాంకం		make_విభజించు వాడని) // whether to split window
{
    cscmd_T *cmdp;

    ఒకవేళ (check_restricted())
	తిరిగి;

    ఒకవేళ ((cmdp = cs_lookup_cmd(eap)) == ఖాళీ)
    {
	cs_help(eap);
	తిరిగి;
    }

    ఒకవేళ (make_విభజించు)
    {
	ఒకవేళ (!cmdp->cansplit)
	{
	    (శూన్య)సందేశం_puts(_("This cscope command does not support splitting the window.\n"));
	    తిరిగి;
	}
	postponed_విభజించు = -1;
	postponed_విభజించు_flags = cmdmod.cmod_విభజించు;
	postponed_విభజించు_tab = cmdmod.cmod_tab;
    }

    cmdp->func(eap);

    postponed_విభజించు_flags = 0;
    postponed_విభజించు_tab = 0;
}

/*
 * Implementation of ":cscope" and ":lcscope"
 */
    శూన్య
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    శూన్య
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    శూన్య
ex_cstag(exarg_T *eap)
{
    పూర్ణాంకం ret = FALSE;

    ఒకవేళ (*eap->arg == NUL)
    {
	(శూన్య)emsg(_(e_usage_cstag_ident));
	తిరిగి;
    }

    ఎంపిక (p_csto)
    {
    స్థితి 0 :
	ఒకవేళ (cs_check_for_connections())
	{
	    ret = cs_కనుగొను_common("g", (అక్షరం *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ఒకవేళ (ret == FALSE)
	    {
		cs_free_ట్యాగులు();
		ఒకవేళ (సందేశం_నిలువు)
		    సందేశం_putchar('\n');

		ఒకవేళ (cs_check_for_ట్యాగులు())
		    ret = do_ట్యాగ్(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	లేకపోతే ఒకవేళ (cs_check_for_ట్యాగులు())
	{
	    ret = do_ట్యాగ్(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	ఆపు;
    స్థితి 1 :
	ఒకవేళ (cs_check_for_ట్యాగులు())
	{
	    ret = do_ట్యాగ్(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    ఒకవేళ (ret == FALSE)
	    {
		ఒకవేళ (సందేశం_నిలువు)
		    సందేశం_putchar('\n');

		ఒకవేళ (cs_check_for_connections())
		{
		    ret = cs_కనుగొను_common("g", (అక్షరం *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    ఒకవేళ (ret == FALSE)
			cs_free_ట్యాగులు();
		}
	    }
	}
	లేకపోతే ఒకవేళ (cs_check_for_connections())
	{
	    ret = cs_కనుగొను_common("g", (అక్షరం *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ఒకవేళ (ret == FALSE)
		cs_free_ట్యాగులు();
	}
	ఆపు;
    అప్రమేయ :
	ఆపు;
    }

    ఒకవేళ (!ret)
    {
	(శూన్య)emsg(_(e_cstag_ట్యాగ్_not_founc));
# ఒకవేళ defined(FEAT_QUICKFIX)
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
    పూర్ణాంకం
cs_fgets(char_u *buf, పూర్ణాంకం కొలత)
{
    అక్షరం *p;

    ఒకవేళ ((p = cs_manage_matches(ఖాళీ, ఖాళీ, -1, Get)) == ఖాళీ)
	తిరిగి TRUE;
    vim_strncpy(buf, (char_u *)p, కొలత - 1);

    తిరిగి FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    శూన్య
cs_free_ట్యాగులు(శూన్య)
{
    cs_manage_matches(ఖాళీ, ఖాళీ, -1, Free);
}


/*
 * Called from do_tag().
 */
    శూన్య
cs_print_ట్యాగులు(శూన్య)
{
    cs_manage_matches(ఖాళీ, ఖాళీ, -1, Print);
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
# ఒకవేళ defined(FEAT_EVAL)
    స్థిర పూర్ణాంకం
cs_connection(పూర్ణాంకం అంకె, char_u *dbpath, char_u *ppath)
{
    పూర్ణాంకం i;

    ఒకవేళ (అంకె < 0 || అంకె > 4 || (అంకె > 0 && !dbpath))
	తిరిగి FALSE;

    కొరకు (i = 0; i < csinfo_కొలత; i++)
    {
	ఒకవేళ (!csinfo[i].fname)
	    కొనసాగు;

	ఒకవేళ (అంకె == 0)
	    తిరిగి TRUE;

	ఎంపిక (అంకె)
	{
	స్థితి 1:
	    ఒకవేళ (అక్షరశ్రేణి_శోధన(csinfo[i].fname, (అక్షరం *)dbpath))
		తిరిగి TRUE;
	    ఆపు;
	స్థితి 2:
	    ఒకవేళ (strcmp(csinfo[i].fname, (అక్షరం *)dbpath) == 0)
		తిరిగి TRUE;
	    ఆపు;
	స్థితి 3:
	    ఒకవేళ (అక్షరశ్రేణి_శోధన(csinfo[i].fname, (అక్షరం *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && అక్షరశ్రేణి_శోధన(csinfo[i].ppath, (అక్షరం *)ppath))))
		తిరిగి TRUE;
	    ఆపు;
	స్థితి 4:
	    ఒకవేళ ((strcmp(csinfo[i].fname, (అక్షరం *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (అక్షరం *)ppath) == 0))))
		తిరిగి TRUE;
	    ఆపు;
	}
    }

    తిరిగి FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    స్థిర పూర్ణాంకం
cs_కూడు(exarg_T *eap వాడని)
{
    అక్షరం *fname, *ppath, *flags = ఖాళీ;

    ఒకవేళ ((fname = strtok((అక్షరం *)ఖాళీ, (స్థిరరాశి అక్షరం *)" ")) == ఖాళీ)
    {
	cs_usage_సందేశం(కూడు_2);
	తిరిగి CSCOPE_FAILURE;
    }
    ఒకవేళ ((ppath = strtok((అక్షరం *)ఖాళీ, (స్థిరరాశి అక్షరం *)" ")) != ఖాళీ)
	flags = strtok((అక్షరం *)ఖాళీ, (స్థిరరాశి అక్షరం *)" ");

    తిరిగి cs_కూడు_common(fname, ppath, flags);
}

    స్థిర శూన్య
cs_stat_emsg(అక్షరం *fname)
{
    పూర్ణాంకం err = errno;
    (శూన్య)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    స్థిర పూర్ణాంకం
cs_కూడు_common(
    అక్షరం *arg1,	    // filename - may contain environment variables
    అక్షరం *arg2,	    // prepend path - may contain environment variables
    అక్షరం *flags)
{
    stat_T	statbuf;
    పూర్ణాంకం		ret;
    అక్షరం	*fname = ఖాళీ;
    అక్షరం	*fname2 = ఖాళీ;
    అక్షరం	*ppath = ఖాళీ;
    పూర్ణాంకం		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = ఖాళీ;

    // get the filename (arg1), expand it, and try to stat it
    ఒకవేళ ((fname = alloc(MAXPATHL + 1)) == ఖాళీ)
	వెళ్ళు కూడు_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (శూన్య)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    ఒకవేళ (fname == ఖాళీ)
	వెళ్ళు కూడు_err;
    fname = (అక్షరం *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    ఒకవేళ (ret < 0)
    {
staterr:
	ఒకవేళ (p_csverbose)
	    cs_stat_emsg(fname);
	వెళ్ళు కూడు_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    ఒకవేళ (arg2 != ఖాళీ)
    {
	stat_T	    statbuf2;

	ఒకవేళ ((ppath = alloc(MAXPATHL + 1)) == ఖాళీ)
	    వెళ్ళు కూడు_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	ఒకవేళ (ret < 0)
	    వెళ్ళు staterr;
    }

    // if filename is a directory, append the cscope database name to it
    ఒకవేళ (S_ISDIR(statbuf.st_బహుళకం))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + అక్షరశ్రేణి_పొడవు(fname) + 2);
	ఒకవేళ (fname2 == ఖాళీ)
	    వెళ్ళు కూడు_err;

	అయితే (fname[అక్షరశ్రేణి_పొడవు(fname)-1] == '/'
# ifdef MSWIN
		|| fname[అక్షరశ్రేణి_పొడవు(fname)-1] == '\\'
# endif
		)
	{
	    fname[అక్షరశ్రేణి_పొడవు(fname)-1] = '\0';
	    ఒకవేళ (fname[0] == '\0')
		ఆపు;
	}
	ఒకవేళ (fname[0] == '\0')
	    (శూన్య)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	లేకపోతే
	    (శూన్య)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	ఒకవేళ (ret < 0)
	{
	    ఒకవేళ (p_csverbose)
		cs_stat_emsg(fname2);
	    వెళ్ళు కూడు_err;
	}

	i = cs_చొప్పించు_filelist(fname2, ppath, flags, &statbuf);
    }
    లేకపోతే ఒకవేళ (S_ISREG(statbuf.st_బహుళకం) || S_ISLNK(statbuf.st_బహుళకం))
    {
	i = cs_చొప్పించు_filelist(fname, ppath, flags, &statbuf);
    }
    లేకపోతే
    {
	ఒకవేళ (p_csverbose)
	    (శూన్య)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	వెళ్ళు కూడు_err;
    }

    ఒకవేళ (i != -1)
    {
	ఒకవేళ (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    వెళ్ళు కూడు_err;
	}

	ఒకవేళ (p_csverbose)
	{
	    సందేశం_clr_eos();
	    (శూన్య)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    తిరిగి CSCOPE_SUCCESS;

కూడు_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    తిరిగి CSCOPE_FAILURE;
}


    స్థిర పూర్ణాంకం
cs_check_for_connections(శూన్య)
{
    తిరిగి (cs_cnt_connections() > 0);
}


    స్థిర పూర్ణాంకం
cs_check_for_ట్యాగులు(శూన్య)
{
    తిరిగి (p_ట్యాగులు[0] != NUL && curbuf->b_p_ట్యాగులు != ఖాళీ);
}


/*
 * Count the number of cscope connections.
 */
    స్థిర పూర్ణాంకం
cs_cnt_connections(శూన్య)
{
    పూర్ణాంకం i;
    పూర్ణాంకం cnt = 0;

    కొరకు (i = 0; i < csinfo_కొలత; i++)
	ఒకవేళ (csinfo[i].fname != ఖాళీ)
	    cnt++;
    తిరిగి cnt;
}

    స్థిర శూన్య
cs_reading_emsg(
    పూర్ణాంకం idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    స్థిర పూర్ణాంకం
cs_cnt_matches(పూర్ణాంకం idx)
{
    అక్షరం *stok;
    అక్షరం *buf;
    పూర్ణాంకం nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి 0;
    కొరకు (;;)
    {
	ఒకవేళ (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    ఒకవేళ (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    తిరిగి -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	ఒకవేళ (అక్షరశ్రేణి_శోధన((స్థిరరాశి అక్షరం *)buf, "Unable to search database") != ఖాళీ)
	    ఆపు;
	ఒకవేళ ((stok = strtok(buf, (స్థిరరాశి అక్షరం *)" ")) == ఖాళీ)
	    కొనసాగు;
	ఒకవేళ (అక్షరశ్రేణి_శోధన((స్థిరరాశి అక్షరం *)stok, "cscope:") == ఖాళీ)
	    కొనసాగు;

	ఒకవేళ ((stok = strtok(ఖాళీ, (స్థిరరాశి అక్షరం *)" ")) == ఖాళీ)
	    కొనసాగు;
	nlines = atoi(stok);
	ఒకవేళ (nlines < 0)
	{
	    nlines = 0;
	    ఆపు;
	}

	ఒకవేళ ((stok = strtok(ఖాళీ, (స్థిరరాశి అక్షరం *)" ")) == ఖాళీ)
	    కొనసాగు;
	ఒకవేళ (అక్షరశ్రేణి_పోలిక((స్థిరరాశి అక్షరం *)stok, "lines", 5))
	    కొనసాగు;

	ఆపు;
    }

    vim_free(buf);
    తిరిగి nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    స్థిర అక్షరం *
cs_create_cmd(అక్షరం *csoption, అక్షరం *నమూనా)
{
    అక్షరం *cmd;
    పొట్టి శోధన;
    అక్షరం *pat;

    ఎంపిక (csoption[0])
    {
    స్థితి '0' : స్థితి 's' :
	శోధన = 0;
	ఆపు;
    స్థితి '1' : స్థితి 'g' :
	శోధన = 1;
	ఆపు;
    స్థితి '2' : స్థితి 'd' :
	శోధన = 2;
	ఆపు;
    స్థితి '3' : స్థితి 'c' :
	శోధన = 3;
	ఆపు;
    స్థితి '4' : స్థితి 't' :
	శోధన = 4;
	ఆపు;
    స్థితి '6' : స్థితి 'e' :
	శోధన = 6;
	ఆపు;
    స్థితి '7' : స్థితి 'f' :
	శోధన = 7;
	ఆపు;
    స్థితి '8' : స్థితి 'i' :
	శోధన = 8;
	ఆపు;
    స్థితి '9' : స్థితి 'a' :
	శోధన = 9;
	ఆపు;
    అప్రమేయ :
	(శూన్య)emsg(_(e_unknown_cscope_శోధన_type));
	cs_usage_సందేశం(కనుగొను_2);
	తిరిగి ఖాళీ;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = నమూనా;
    ఒకవేళ (శోధన != 4 && శోధన != 6)
	అయితే VIM_ISWHITE(*pat)
	    ++pat;

    ఒకవేళ ((cmd = alloc(అక్షరశ్రేణి_పొడవు(pat) + 2)) == ఖాళీ)
	తిరిగి ఖాళీ;

    (శూన్య)sprintf(cmd, "%d%s", శోధన, pat);

    తిరిగి cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    స్థిర పూర్ణాంకం
cs_create_connection(పూర్ణాంకం i)
{
# ifdef UNIX
    పూర్ణాంకం		గమ్యం_cs[2], from_cs[2];
# endif
    పూర్ణాంకం		cmdlen;
    అక్షరం	*prog, *cmd, *ppath = ఖాళీ;
    size_t	proglen;
# ifdef MSWIN
    పూర్ణాంకం		fd;
    పూర్ణాంకం		len;
    SECURITY_లక్షణాలు sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# ఒకవేళ defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    గమ్యం_cs[0] = గమ్యం_cs[1] = from_cs[0] = from_cs[1] = -1;
    ఒకవేళ (pipe(గమ్యం_cs) < 0 || pipe(from_cs) < 0)
    {
	(శూన్య)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	ఒకవేళ (గమ్యం_cs[0] != -1)
	    (శూన్య)close(గమ్యం_cs[0]);
	ఒకవేళ (గమ్యం_cs[1] != -1)
	    (శూన్య)close(గమ్యం_cs[1]);
	ఒకవేళ (from_cs[0] != -1)
	    (శూన్య)close(from_cs[0]);
	ఒకవేళ (from_cs[1] != -1)
	    (శూన్య)close(from_cs[1]);
	తిరిగి CSCOPE_FAILURE;
    }

    ఒకవేళ ((csinfo[i].pid = fork()) == -1)
    {
	(శూన్య)emsg(_(e_could_not_fork_for_cscope));
	వెళ్ళు err_closing;
    }
    లేకపోతే ఒకవేళ (csinfo[i].pid == 0)	// child: run cscope.
    {
	అక్షరం **argv = ఖాళీ;

	ఒకవేళ (dup2(గమ్యం_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	ఒకవేళ (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	ఒకవేళ (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(శూన్య)close(గమ్యం_cs[1]);
	(శూన్య)close(from_cs[0]);
# లేకపోతే
	// MSWIN
	// Create pipes to communicate with cscope
	sa.n_నిడివి = పరిమాణం(SECURITY_లక్షణాలు);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ఖాళీ;

	ఒకవేళ (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (శూన్య)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    ఒకవేళ (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    ఒకవేళ (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    తిరిగి CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	ఒకవేళ ((prog = alloc(MAXPATHL + 1)) == ఖాళీ)
	{
# ifdef UNIX
	    నిష్క్రమించు(EXIT_FAILURE);
# లేకపోతే
	    // MSWIN
	    వెళ్ళు err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (పూర్ణాంకం)(proglen + అక్షరశ్రేణి_పొడవు(csinfo[i].fname) + 32);
	ఒకవేళ (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    ఒకవేళ ((ppath = alloc(MAXPATHL + 1)) == ఖాళీ)
	    {
		vim_free(prog);
# ifdef UNIX
		నిష్క్రమించు(EXIT_FAILURE);
# లేకపోతే
		// MSWIN
		వెళ్ళు err_closing;
# endif
	    }
	    cmdlen += (పూర్ణాంకం)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	ఒకవేళ (csinfo[i].flags)
	    cmdlen += (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(csinfo[i].flags);

	ఒకవేళ ((cmd = alloc(cmdlen)) == ఖాళీ)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    నిష్క్రమించు(EXIT_FAILURE);
# లేకపోతే
	    // MSWIN
	    వెళ్ళు err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    అక్షరం	**tok = ఖాళీ;
	    పూర్ణాంకం		tokc = 0;

	    ga_init2(&ga_argv, పరిమాణం(అక్షరం *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    ఒకవేళ (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		నిష్క్రమించు(EXIT_FAILURE);
	    కొరకు (పూర్ణాంకం k = 0; k < tokc; ++k)
	    {
		ఒకవేళ (ga_grow(&ga_argv, 1) == FAIL)
		    నిష్క్రమించు(EXIT_FAILURE);
		((అక్షరం **)ga_argv.ga_దత్తాంశం)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    ఒకవేళ (ga_grow(&ga_argv, 3) == FAIL)
		నిష్క్రమించు(EXIT_FAILURE);
	    ((అక్షరం **)ga_argv.ga_దత్తాంశం)[ga_argv.ga_len++] =
				    (అక్షరం *)vim_strsave((char_u *)"-dl");
	    ((అక్షరం **)ga_argv.ga_దత్తాంశం)[ga_argv.ga_len++] =
				    (అక్షరం *)vim_strsave((char_u *)"-f");
	    ((అక్షరం **)ga_argv.ga_దత్తాంశం)[ga_argv.ga_len++] =
				(అక్షరం *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    ఒకవేళ (ppath != ఖాళీ)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		అక్షరం	*parg = alloc(plen);

		ఒకవేళ (parg == ఖాళీ || ga_grow(&ga_argv, 1) == FAIL)
		    నిష్క్రమించు(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((అక్షరం **)ga_argv.ga_దత్తాంశం)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    ఒకవేళ (csinfo[i].flags != ఖాళీ)
	    {
		ఒకవేళ (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    నిష్క్రమించు(EXIT_FAILURE);
		కొరకు (పూర్ణాంకం k = 0; k < tokc; ++k)
		{
		    ఒకవేళ (ga_grow(&ga_argv, 1) == FAIL)
			నిష్క్రమించు(EXIT_FAILURE);
		    ((అక్షరం **)ga_argv.ga_దత్తాంశం)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    ఒకవేళ (ga_grow(&ga_argv, 1) == FAIL)
		నిష్క్రమించు(EXIT_FAILURE);
	    ((అక్షరం **)ga_argv.ga_దత్తాంశం)[ga_argv.ga_len] = ఖాళీ;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  ఒకవేళ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   ఒకవేళ defined(HAVE_SETSID)
	    (శూన్య)setsid();
#   లేకపోతే
	    ఒకవేళ (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (అక్షరం **)ga_argv.ga_దత్తాంశం;
	    ఒకవేళ (argv[0] == ఖాళీ || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    నిష్క్రమించు(127);
	    // NOTREACHED
	}
    }
    లేకపోతే	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	ఒకవేళ ((csinfo[i].గమ్యం_fp = fdopen(గమ్యం_cs[1], "w")) == ఖాళీ)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	ఒకవేళ ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == ఖాళీ)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(శూన్య)close(గమ్యం_cs[0]);
	(శూన్య)close(from_cs[1]);
    }
# లేకపోతే
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    ఒకవేళ (csinfo[i].ppath != ఖాళీ)
    {
	len = (పూర్ణాంకం)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    ఒకవేళ (csinfo[i].flags != ఖాళీ)
    {
	len = (పూర్ణాంకం)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.w_Show_కిటికీ = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_ఆదానం  = stdin_rd;
    created = CreateProcess(ఖాళీ, cmd, ఖాళీ, ఖాళీ, TRUE, CREATE_NEW_CONSOLE,
							ఖాళీ, ఖాళీ, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    ఒకవేళ (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(శూన్య)emsg(_(e_could_not_spawn_cscope_process));
	వెళ్ళు err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    ఒకవేళ (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      O_పాఠ్యం|_O_APPEND)) < 0)
	    || ((csinfo[i].గమ్యం_fp = _fdopen(fd, "w")) == ఖాళీ))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    ఒకవేళ (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      O_పాఠ్యం|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == ఖాళీ))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    తిరిగి CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    స్థిర పూర్ణాంకం
cs_కనుగొను(exarg_T *eap)
{
    అక్షరం *opt, *pat;
    పూర్ణాంకం i;

    ఒకవేళ (cs_check_for_connections() == FALSE)
    {
	(శూన్య)emsg(_(e_no_cscope_connections));
	తిరిగి FALSE;
    }

    ఒకవేళ ((opt = strtok((అక్షరం *)ఖాళీ, (స్థిరరాశి అక్షరం *)" ")) == ఖాళీ)
    {
	cs_usage_సందేశం(కనుగొను_2);
	తిరిగి FALSE;
    }

    pat = opt + అక్షరశ్రేణి_పొడవు(opt) + 1;
    ఒకవేళ (pat >= (అక్షరం *)eap->arg + eap_arg_len)
    {
	cs_usage_సందేశం(కనుగొను_2);
	తిరిగి FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    కొరకు (i = 0; i < eap_arg_len; ++i)
	ఒకవేళ (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    తిరిగి cs_కనుగొను_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    స్థిర పూర్ణాంకం
cs_కనుగొను_common(
    అక్షరం *opt,
    అక్షరం *pat,
    పూర్ణాంకం forceit,
    పూర్ణాంకం verbose,
    పూర్ణాంకం	use_ll వాడని,
    char_u *cmdline వాడని)
{
    పూర్ణాంకం i;
    అక్షరం *cmd;
    పూర్ణాంకం *nummatches;
    పూర్ణాంకం totmatches;
# ifdef FEAT_QUICKFIX
    అక్షరం cmdletter;
    అక్షరం *qfpos;

    // get cmd letter
    ఎంపిక (opt[0])
    {
    స్థితి '0' :
	cmdletter = 's';
	ఆపు;
    స్థితి '1' :
	cmdletter = 'g';
	ఆపు;
    స్థితి '2' :
	cmdletter = 'd';
	ఆపు;
    స్థితి '3' :
	cmdletter = 'c';
	ఆపు;
    స్థితి '4' :
	cmdletter = 't';
	ఆపు;
    స్థితి '6' :
	cmdletter = 'e';
	ఆపు;
    స్థితి '7' :
	cmdletter = 'f';
	ఆపు;
    స్థితి '8' :
	cmdletter = 'i';
	ఆపు;
    స్థితి '9' :
	cmdletter = 'a';
	ఆపు;
    అప్రమేయ :
	cmdletter = opt[0];
    }

    qfpos = (అక్షరం *)vim_strchr(p_csqf, cmdletter);
    ఒకవేళ (qfpos != ఖాళీ)
    {
	qfpos++;
	// next symbol must be + or -
	ఒకవేళ (strchr(CSQF_FLAGS, *qfpos) == ఖాళీ)
	{
	    (శూన్య)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    తిరిగి FALSE;
	}

	ఒకవేళ (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    ఒకవేళ (aborting())
		తిరిగి FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    ఒకవేళ (cmd == ఖాళీ)
	తిరిగి FALSE;

    nummatches = ALLOC_MULT(పూర్ణాంకం, csinfo_కొలత);
    ఒకవేళ (nummatches == ఖాళీ)
    {
	vim_free(cmd);
	తిరిగి FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    కొరకు (i = 0; i < csinfo_కొలత; i++)
	nummatches[i] = 0;
    totmatches = 0;
    కొరకు (i = 0; i < csinfo_కొలత; i++)
    {
	ఒకవేళ (csinfo[i].fname == ఖాళీ || csinfo[i].గమ్యం_fp == ఖాళీ)
	    కొనసాగు;

	// send cmd to cscope
	(శూన్య)fprintf(csinfo[i].గమ్యం_fp, "%s\n", cmd);
	(శూన్య)fflush(csinfo[i].గమ్యం_fp);

	nummatches[i] = cs_cnt_matches(i);

	ఒకవేళ (nummatches[i] > -1)
	    totmatches += nummatches[i];

	ఒకవేళ (nummatches[i] == 0)
	    (శూన్య)cs_read_prompt(i);
    }
    vim_free(cmd);

    ఒకవేళ (totmatches == 0)
    {
	ఒకవేళ (verbose)
	    (శూన్య)semsg(_(e_no_matches_దొరికింది_for_cscope_query_str_యొక్క_str),
								     opt, pat);
	vim_free(nummatches);
	తిరిగి FALSE;
    }

# ifdef FEAT_QUICKFIX
    ఒకవేళ (qfpos != ఖాళీ && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = ఖాళీ;
	win_T	    *wp = ఖాళీ;

	ఒకవేళ (tmp == ఖాళీ)
	{
	    vim_free(nummatches);
	    తిరిగి FALSE;
	}

	f = mch_fopen((అక్షరం *)tmp, "w");
	ఒకవేళ (f == ఖాళీ)
	    semsg(_(e_cant_open_file_str), tmp);
	లేకపోతే
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    ఒకవేళ (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    ఒకవేళ (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, ఖాళీ) > 0)
	    {
		ఒకవేళ (postponed_విభజించు != 0)
		{
		    (శూన్య)win_విభజించు(postponed_విభజించు > 0 ? postponed_విభజించు : 0,
						       postponed_విభజించు_flags);
		    RESET_BINDING(curwin);
		    postponed_విభజించు = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		ఒకవేళ (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_బఫరు) && wp->w_llist_ref != ఖాళీ)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	తిరిగి TRUE;
    }
    లేకపోతే
# endif // FEAT_QUICKFIX
    {
	అక్షరం **matches = ఖాళీ, **contexts = ఖాళీ;
	పూర్ణాంకం matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	ఒకవేళ (matches == ఖాళీ)
	    తిరిగి FALSE;

	(శూన్య)cs_manage_matches(matches, contexts, matched, Store);

	తిరిగి do_ట్యాగ్((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    స్థిర పూర్ణాంకం
cs_help(exarg_T *eap వాడని)
{
    cscmd_T *cmdp = cs_cmds;

    (శూన్య)సందేశం_puts(_("cscope commands:\n"));
    అయితే (cmdp->name != ఖాళీ)
    {
	అక్షరం *help = _(cmdp->help);
	పూర్ణాంకం  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	ఒకవేళ (space_cnt < 0)
	    space_cnt = 0;
	(శూన్య)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	ఒకవేళ (strcmp(cmdp->name, "find") == 0)
	    సందేశం_puts(_("\n"
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
    తిరిగి 0;
}


    స్థిర శూన్య
clear_csinfo(పూర్ణాంకం i)
{
    csinfo[i].fname  = ఖాళీ;
    csinfo[i].ppath  = ఖాళీ;
    csinfo[i].flags  = ఖాళీ;
# ఒకవేళ defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# లేకపోతే
    csinfo[i].nVolume = 0;
    csinfo[i].n_సూచీ_ఎగువ = 0;
    csinfo[i].n_సూచీ_దిగువ = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = ఖాళీ;
    csinfo[i].గమ్యం_fp  = ఖాళీ;
# ఒకవేళ defined(MSWIN)
    csinfo[i].hProc = ఖాళీ;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    స్థిర పూర్ణాంకం
cs_చొప్పించు_filelist(
    అక్షరం *fname,
    అక్షరం *ppath,
    అక్షరం *flags,
    stat_T *sb వాడని)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    ఎంపిక (win32_fileinfo((char_u *)fname, &bhfi))
    {
	స్థితి FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	స్థితి FILEINFO_READ_FAIL:	// CreateFile() failed
	    ఒకవేళ (p_csverbose)
	    {
		అక్షరం *cant_సందేశం = _(e_cannot_open_cscope_database_str);
		అక్షరం *winmsg = GetWin32Error();

		ఒకవేళ (winmsg != ఖాళీ)
		    (శూన్య)semsg(cant_సందేశం, winmsg);
		లేకపోతే
		    // subst filename if can't get error text
		    (శూన్య)semsg(cant_సందేశం, fname);
	    }
	    తిరిగి -1;

	స్థితి FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    ఒకవేళ (p_csverbose)
		(శూన్య)emsg(_(e_cannot_get_cscope_database_information));
	    తిరిగి -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    కొరకు (j = 0; j < csinfo_కొలత; j++)
    {
	ఒకవేళ (csinfo[j].fname != ఖాళీ
# ఒకవేళ defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# లేకపోతే
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_సంఖ్య
		    && csinfo[j].n_సూచీ_ఎగువ == bhfi.n_File_సూచీ_ఎగువ
		    && csinfo[j].n_సూచీ_దిగువ == bhfi.n_File_సూచీ_దిగువ))
# endif
	    )
	{
	    ఒకవేళ (p_csverbose)
		(శూన్య)emsg(_(e_duplicate_cscope_database_not_added));
	    తిరిగి -1;
	}

	ఒకవేళ (csinfo[j].fname == ఖాళీ && i == -1)
	    i = j; // remember first empty entry
    }

    ఒకవేళ (i == -1)
    {
	i = csinfo_కొలత;
	ఒకవేళ (csinfo_కొలత == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_కొలత = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	లేకపోతే
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_కొలత *= 2;
	    csinfo = vim_realloc(csinfo, పరిమాణం(csinfo_T)*csinfo_కొలత);
	    ఒకవేళ (csinfo == ఖాళీ)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_కొలత = i;
		తిరిగి -1;
	    }
	}
	ఒకవేళ (csinfo == ఖాళీ)
	    తిరిగి -1;
	కొరకు (j = csinfo_కొలత/2; j < csinfo_కొలత; j++)
	    clear_csinfo(j);
    }

    ఒకవేళ ((csinfo[i].fname = alloc(అక్షరశ్రేణి_పొడవు(fname)+1)) == ఖాళీ)
	తిరిగి -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, అక్షరశ్రేణి_పొడవు((స్థిరరాశి అక్షరం *)fname));

    ఒకవేళ (ppath != ఖాళీ)
    {
	ఒకవేళ ((csinfo[i].ppath = alloc(అక్షరశ్రేణి_పొడవు(ppath) + 1)) == ఖాళీ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    తిరిగి -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, అక్షరశ్రేణి_పొడవు((స్థిరరాశి అక్షరం *)ppath));
    }
    లేకపోతే
	csinfo[i].ppath = ఖాళీ;

    ఒకవేళ (flags != ఖాళీ)
    {
	ఒకవేళ ((csinfo[i].flags = alloc(అక్షరశ్రేణి_పొడవు(flags) + 1)) == ఖాళీ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    తిరిగి -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, అక్షరశ్రేణి_పొడవు((స్థిరరాశి అక్షరం *)flags));
    }
    లేకపోతే
	csinfo[i].flags = ఖాళీ;

# ఒకవేళ defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# లేకపోతే
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_సంఖ్య;
    csinfo[i].n_సూచీ_దిగువ = bhfi.n_File_సూచీ_దిగువ;
    csinfo[i].n_సూచీ_ఎగువ = bhfi.n_File_సూచీ_ఎగువ;
# endif
    తిరిగి i;
}


/*
 * Find cscope command in command table.
 */
    స్థిర cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    అక్షరం *stok;
    size_t len;

    ఒకవేళ (eap->arg == ఖాళీ)
	తిరిగి ఖాళీ;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (పూర్ణాంకం)STRLEN(eap->arg);

    ఒకవేళ ((stok = strtok((అక్షరం *)(eap->arg), (స్థిరరాశి అక్షరం *)" ")) == ఖాళీ)
	తిరిగి ఖాళీ;

    len = అక్షరశ్రేణి_పొడవు(stok);
    కొరకు (cmdp = cs_cmds; cmdp->name != ఖాళీ; ++cmdp)
    {
	ఒకవేళ (అక్షరశ్రేణి_పోలిక((స్థిరరాశి అక్షరం *)(stok), cmdp->name, len) == 0)
	    తిరిగి (cmdp);
    }
    తిరిగి ఖాళీ;
}


/*
 * Nuke em.
 */
    స్థిర పూర్ణాంకం
cs_kill(exarg_T *eap వాడని)
{
    అక్షరం *stok;
    పూర్ణాంకం i;

    ఒకవేళ ((stok = strtok((అక్షరం *)ఖాళీ, (స్థిరరాశి అక్షరం *)" ")) == ఖాళీ)
    {
	cs_usage_సందేశం(Kill);
	తిరిగి CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    ఒకవేళ ((అక్షరశ్రేణి_పొడవు(stok) < 2 && VIM_ISDIGIT((పూర్ణాంకం)(stok[0])))
	    || (అక్షరశ్రేణి_పొడవు(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((పూర్ణాంకం)(stok[1]))))
	i = atoi(stok);
    లేకపోతే
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	కొరకు (i = 0; i < csinfo_కొలత; i++)
	{
	    ఒకవేళ (csinfo[i].fname != ఖాళీ && అక్షరశ్రేణి_శోధన(csinfo[i].fname, stok))
		ఆపు;
	}
    }

    ఒకవేళ ((i != -1) && (i >= csinfo_కొలత || i < -1 || csinfo[i].fname == ఖాళీ))
    {
	ఒకవేళ (p_csverbose)
	    (శూన్య)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    లేకపోతే
    {
	ఒకవేళ (i == -1)
	{
	    కొరకు (i = 0; i < csinfo_కొలత; i++)
	    {
		ఒకవేళ (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	లేకపోతే
	    cs_kill_execute(i, stok);
    }

    తిరిగి 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    స్థిర శూన్య
cs_kill_execute(
    పూర్ణాంకం i,	    // cscope table index
    అక్షరం *cname)    // cscope database name
{
    ఒకవేళ (p_csverbose)
    {
	సందేశం_clr_eos();
	(శూన్య)smsg_attr(HL_ATTR(HLF_R) | సందేశం_HIST,
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
    స్థిర అక్షరం *
cs_make_vim_style_matches(
    అక్షరం *fname,
    అక్షరం *slno,
    అక్షరం *శోధన,
    అక్షరం *tagstr)
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
    అక్షరం *buf;
    పూర్ణాంకం amt;

    ఒకవేళ (శోధన != ఖాళీ)
    {
	amt = (పూర్ణాంకం)(అక్షరశ్రేణి_పొడవు(fname) + అక్షరశ్రేణి_పొడవు(slno) + అక్షరశ్రేణి_పొడవు(tagstr) + అక్షరశ్రేణి_పొడవు(శోధన)+6);
	ఒకవేళ ((buf = alloc(amt)) == ఖాళీ)
	    తిరిగి ఖాళీ;

	(శూన్య)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, శోధన);
    }
    లేకపోతే
    {
	amt = (పూర్ణాంకం)(అక్షరశ్రేణి_పొడవు(fname) + అక్షరశ్రేణి_పొడవు(slno) + అక్షరశ్రేణి_పొడవు(tagstr) + 5);
	ఒకవేళ ((buf = alloc(amt)) == ఖాళీ)
	    తిరిగి ఖాళీ;

	(శూన్య)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    తిరిగి buf;
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
    స్థిర అక్షరం *
cs_manage_matches(
    అక్షరం **matches,
    అక్షరం **contexts,
    పూర్ణాంకం totmatches,
    mcmd_e cmd)
{
    స్థిర అక్షరం **mp = ఖాళీ;
    స్థిర అక్షరం **cp = ఖాళీ;
    స్థిర పూర్ణాంకం cnt = -1;
    స్థిర పూర్ణాంకం తదుపరి = -1;
    అక్షరం *p = ఖాళీ;

    ఎంపిక (cmd)
    {
    స్థితి Store:
	ప్రతిజ్ఞా(matches != ఖాళీ);
	ప్రతిజ్ఞా(totmatches > 0);
	ఒకవేళ (mp != ఖాళీ || cp != ఖాళీ)
	    (శూన్య)cs_manage_matches(ఖాళీ, ఖాళీ, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	తదుపరి = 0;
	ఆపు;
    స్థితి Get:
	ఒకవేళ (తదుపరి >= cnt)
	    తిరిగి ఖాళీ;

	p = mp[తదుపరి];
	తదుపరి++;
	ఆపు;
    స్థితి Free:
	ఒకవేళ (mp != ఖాళీ)
	{
	    ఒకవేళ (cnt > 0)
		అయితే (cnt--)
		{
		    vim_free(mp[cnt]);
		    ఒకవేళ (cp != ఖాళీ)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = ఖాళీ;
	cp = ఖాళీ;
	cnt = 0;
	తదుపరి = 0;
	ఆపు;
    స్థితి Print:
	cs_print_ట్యాగులు_priv(mp, cp, cnt);
	ఆపు;
    అప్రమేయ:	// should not reach here
	iemsg(e_fatal_error_లోపల_cs_manage_matches);
	తిరిగి ఖాళీ;
    }

    తిరిగి p;
}


/*
 * Parse cscope output.
 */
    స్థిర అక్షరం *
cs_parse_results(
    పూర్ణాంకం cnumber,
    అక్షరం *buf,
    పూర్ణాంకం bufsize,
    అక్షరం **context,
    అక్షరం **linenumber,
    అక్షరం **శోధన)
{
    పూర్ణాంకం ch;
    అక్షరం *p;
    అక్షరం *name;

    ఒకవేళ (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == ఖాళీ)
    {
	ఒకవేళ (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	తిరిగి ఖాళీ;
    }

    // If the line's too long for the buffer, discard it.
    ఒకవేళ ((p = strchr(buf, '\n')) == ఖాళీ)
    {
	అయితే ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	తిరిగి ఖాళీ;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    ఒకవేళ ((name = strtok(buf, (స్థిరరాశి అక్షరం *)" ")) == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ ((*context = strtok(ఖాళీ, (స్థిరరాశి అక్షరం *)" ")) == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ ((*linenumber = strtok(ఖాళీ, (స్థిరరాశి అక్షరం *)" ")) == ఖాళీ)
	తిరిగి ఖాళీ;
    *శోధన = *linenumber + అక్షరశ్రేణి_పొడవు(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    ఒకవేళ (strcmp(*శోధన, "<unknown>") == 0)
	*శోధన = ఖాళీ;

    name = cs_resolve_file(cnumber, name);
    తిరిగి name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    స్థిర శూన్య
cs_file_results(FILE *f, పూర్ణాంకం *nummatches_a)
{
    పూర్ణాంకం i, j;
    అక్షరం *buf;
    అక్షరం *శోధన, *slno;
    అక్షరం *fullname;
    అక్షరం *cntx;
    అక్షరం *context;

    buf = alloc(CSREAD_BUFSIZE);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    కొరకు (i = 0; i < csinfo_కొలత; i++)
    {
	ఒకవేళ (nummatches_a[i] < 1)
	    కొనసాగు;

	కొరకు (j = 0; j < nummatches_a[i]; j++)
	{
	   ఒకవేళ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &శోధన)) == ఖాళీ)
	       కొనసాగు;

	   context = alloc(అక్షరశ్రేణి_పొడవు(cntx)+5);
	   ఒకవేళ (context == ఖాళీ)
	   {
	       vim_free(fullname);
	       కొనసాగు;
	   }

	   ఒకవేళ (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   లేకపోతే
	       sprintf(context, "<<%s>>", cntx);

	   ఒకవేళ (శోధన == ఖాళీ)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   లేకపోతే
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, శోధన);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(శూన్య)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    స్థిర శూన్య
cs_fill_results(
    అక్షరం *tagstr,
    పూర్ణాంకం totmatches,
    పూర్ణాంకం *nummatches_a,
    అక్షరం ***matches_p,
    అక్షరం ***cntxts_p,
    పూర్ణాంకం *matched)
{
    పూర్ణాంకం i, j;
    అక్షరం *buf;
    అక్షరం *శోధన, *slno;
    పూర్ణాంకం totsofar = 0;
    అక్షరం **matches = ఖాళీ;
    అక్షరం **cntxts = ఖాళీ;
    అక్షరం *fullname;
    అక్షరం *cntx;

    ప్రతిజ్ఞా(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    ఒకవేళ ((matches = ALLOC_MULT(అక్షరం *, totmatches)) == ఖాళీ)
	వెళ్ళు parse_బహిర్గతం;
    ఒకవేళ ((cntxts = ALLOC_MULT(అక్షరం *, totmatches)) == ఖాళీ)
	వెళ్ళు parse_బహిర్గతం;

    కొరకు (i = 0; i < csinfo_కొలత; i++)
    {
	ఒకవేళ (nummatches_a[i] < 1)
	    కొనసాగు;

	కొరకు (j = 0; j < nummatches_a[i]; j++)
	{
	   ఒకవేళ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &శోధన)) == ఖాళీ)
		కొనసాగు;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  శోధన, tagstr);

	    vim_free(fullname);

	    ఒకవేళ (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = ఖాళీ;
	    లేకపోతే
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (అక్షరం *)vim_strsave((char_u *)cntx);

	    ఒకవేళ (matches[totsofar] != ఖాళీ)
		totsofar++;

	} // for all matches

	(శూన్య)cs_read_prompt(i);

    } // for all cscope connections

parse_బహిర్గతం:
    ఒకవేళ (totsofar == 0)
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
    స్థిర అక్షరం *
cs_pathcomponents(అక్షరం *మార్గం)
{
    పూర్ణాంకం		i;
    అక్షరం	*s;

    ఒకవేళ (p_cspc == 0)
	తిరిగి మార్గం;

    s = మార్గం + అక్షరశ్రేణి_పొడవు(మార్గం) - 1;
    కొరకు (i = 0; i < p_cspc; ++i)
    {
	అయితే (s > మార్గం)
	{
	   s--;
	   ఒకవేళ (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      ఆపు;
	}
    }
    ఒకవేళ ((s > మార్గం && *s == '/')
# ifdef MSWIN
	|| (s > మార్గం && *s == '\\')
# endif
	    )
	++s;
    తిరిగి s;
}

/*
 * Called from cs_manage_matches().
 */
    స్థిర శూన్య
cs_print_ట్యాగులు_priv(అక్షరం **matches, అక్షరం **cntxts, పూర్ణాంకం అంకె_matches)
{
    అక్షరం	*buf = ఖాళీ;
    అక్షరం	*t_buf;
    పూర్ణాంకం		bufsize = 0; // Track available bufsize
    పూర్ణాంకం		newsize = 0;
    అక్షరం	*ptag;
    అక్షరం	*fname, *lno, *extra, *tbuf;
    పూర్ణాంకం		i, idx, అంకె;
    అక్షరం	*globalcntx = "GLOBAL";
    అక్షరం	*cntxformat = " <<%s>>";
    అక్షరం	*context;
    అక్షరం	*cstag_సందేశం = _("Cscope tag: %s");
    అక్షరం	*csfmt_str = "%4d %6s  ";

    ప్రతిజ్ఞా(అంకె_matches > 0);

    ఒకవేళ ((tbuf = alloc(అక్షరశ్రేణి_పొడవు(matches[0]) + 1)) == ఖాళీ)
	తిరిగి;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    ఒకవేళ (ptag == ఖాళీ)
    {
	vim_free(tbuf);
	తిరిగి;
    }

    newsize = (పూర్ణాంకం)(అక్షరశ్రేణి_పొడవు(cstag_సందేశం) + అక్షరశ్రేణి_పొడవు(ptag));
    buf = alloc(newsize);
    ఒకవేళ (buf != ఖాళీ)
    {
	bufsize = newsize;
	(శూన్య)sprintf(buf, cstag_సందేశం, ptag);
	సందేశం_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    సందేశం_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    సందేశం_advance(సందేశం_నిలువు + 2);
    సందేశం_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    అంకె = 1;
    కొరకు (i = 0; i < అంకె_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	ఒకవేళ ((tbuf = alloc(అక్షరశ్రేణి_పొడవు(matches[idx]) + 1)) == ఖాళీ)
	    కొనసాగు;
	(శూన్య)strcpy(tbuf, matches[idx]);

	ఒకవేళ (strtok(tbuf, (స్థిరరాశి అక్షరం *)"\t") == ఖాళీ
		|| (fname = strtok(ఖాళీ, (స్థిరరాశి అక్షరం *)"\t")) == ఖాళీ
		|| (lno = strtok(ఖాళీ, (స్థిరరాశి అక్షరం *)"\t")) == ఖాళీ)
	{
	    vim_free(tbuf);
	    కొనసాగు;
	}
	extra = strtok(ఖాళీ, (స్థిరరాశి అక్షరం *)"\t");

	lno[అక్షరశ్రేణి_పొడవు(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (పూర్ణాంకం)(అక్షరశ్రేణి_పొడవు(csfmt_str) + 16 + అక్షరశ్రేణి_పొడవు(lno));
	ఒకవేళ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ఒకవేళ (buf == ఖాళీ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    లేకపోతే
		bufsize = newsize;
	}
	ఒకవేళ (buf != ఖాళీ)
	{
	    // csfmt_str = "%4d %6s  ";
	    (శూన్య)sprintf(buf, csfmt_str, అంకె, lno);
	    సందేశం_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	సందేశం_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	ఒకవేళ (cntxts[idx] != ఖాళీ)
	    context = cntxts[idx];
	లేకపోతే
	    context = globalcntx;
	newsize = (పూర్ణాంకం)(అక్షరశ్రేణి_పొడవు(context) + అక్షరశ్రేణి_పొడవు(cntxformat));

	ఒకవేళ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ఒకవేళ (buf == ఖాళీ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    లేకపోతే
		bufsize = newsize;
	}
	ఒకవేళ (buf != ఖాళీ)
	{
	    (శూన్య)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    ఒకవేళ (సందేశం_నిలువు + (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(buf) >= cmdline_వెడల్పు)
		సందేశం_putchar('\n');
	    సందేశం_advance(12);
	    సందేశం_outtrans_long_attr((char_u *)buf, 0);
	    సందేశం_putchar('\n');
	}
	ఒకవేళ (extra != ఖాళీ)
	{
	    సందేశం_advance(13);
	    సందేశం_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	ఒకవేళ (సందేశం_నిలువు)
	    సందేశం_putchar('\n');

	ui_breakcheck();
	ఒకవేళ (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    ఆపు;
	}

	అంకె++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    స్థిర పూర్ణాంకం
cs_read_prompt(పూర్ణాంకం i)
{
    పూర్ణాంకం		ch;
    అక్షరం	*buf = ఖాళీ; // buffer for possible error message from cscope
    పూర్ణాంకం		bufpos = 0;
    పూర్ణాంకం		maxlen;
    స్థిర అక్షరం *eprompt = "Press the RETURN key to continue:";
    పూర్ణాంకం		epromptlen = (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(eprompt);
    పూర్ణాంకం		n;

    // compute maximum allowed len for Cscope error message
    అక్షరం *cs_emsg = _(e_cscope_error_str);
    maxlen = (పూర్ణాంకం)(IOSIZE - అక్షరశ్రేణి_పొడవు(cs_emsg));

    కొరకు (;;)
    {
	అయితే ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    ఒకవేళ (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		ఒకవేళ (buf == ఖాళీ) // lazy buffer allocation
		    buf = alloc(maxlen);
		ఒకవేళ (buf != ఖాళీ)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    ఒకవేళ (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(శూన్య)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(శూన్య)putc('\n', csinfo[i].గమ్యం_fp);
			(శూన్య)fflush(csinfo[i].గమ్యం_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	కొరకు (n = 0; n < (పూర్ణాంకం)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    ఒకవేళ (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    ఒకవేళ (ch == EOF)
	    {
		ఒకవేళ (buf != ఖాళీ && buf[0] != NUL)
		    (శూన్య)semsg(cs_emsg, buf);
		లేకపోతే ఒకవేళ (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		తిరిగి CSCOPE_FAILURE;
	    }

	    ఒకవేళ (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		ఆపు;
	    }
	}

	ఒకవేళ (ch == EOF)
	    కొనసాగు;	    // didn't find the prompt
	ఆపు;		    // did find the prompt
    }

    vim_free(buf);
    తిరిగి CSCOPE_SUCCESS;
}

# ఒకవేళ defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    స్థిర శూన్య
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    స్థిర శూన్య
cs_release_csp(పూర్ణాంకం i, పూర్ణాంకం freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    ఒకవేళ (csinfo[i].గమ్యం_fp != ఖాళీ)
    {
	(శూన్య)fputs("q\n", csinfo[i].గమ్యం_fp);
	(శూన్య)fflush(csinfo[i].గమ్యం_fp);
    }
# ఒకవేళ defined(UNIX)
    {
	పూర్ణాంకం waitpid_errno;
	పూర్ణాంకం pstat;
	pid_t pid;

#  ఒకవేళ defined(HAVE_SIGACTION)
	నిర్మాణం sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   లేకపోతే
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, ఖాళీ);
#  లేకపోతే
	పూర్ణాంకం waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	కొరకు (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    ఒకవేళ (pid != 0)
		ఆపు;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	ఒకవేళ (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    పూర్ణాంకం alive = TRUE;

	    ఒకవేళ (waitpid_errno == ECHILD)
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
		పూర్ణాంకం waited;

		sleep(0);
		కొరకు (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    ఒకవేళ (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			ఆపు;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    ఒకవేళ (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(శూన్య)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# లేకపోతే  // !UNIX
    ఒకవేళ (csinfo[i].hProc != ఖాళీ)
    {
	// Give cscope a chance to exit normally
	ఒకవేళ (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    ఒకవేళ (csinfo[i].fr_fp != ఖాళీ)
	(శూన్య)fclose(csinfo[i].fr_fp);
    ఒకవేళ (csinfo[i].గమ్యం_fp != ఖాళీ)
	(శూన్య)fclose(csinfo[i].గమ్యం_fp);

    ఒకవేళ (freefnpp)
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
    స్థిర పూర్ణాంకం
cs_reset(exarg_T *eap వాడని)
{
    అక్షరం	**dblist = ఖాళీ, **pplist = ఖాళీ, **fllist = ఖాళీ;
    పూర్ణాంకం	i;
    అక్షరం buf[20]; // for sprintf " (#%d)"

    ఒకవేళ (csinfo_కొలత == 0)
	తిరిగి CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(అక్షరం *, csinfo_కొలత);
    pplist = ALLOC_MULT(అక్షరం *, csinfo_కొలత);
    fllist = ALLOC_MULT(అక్షరం *, csinfo_కొలత);
    ఒకవేళ (dblist == ఖాళీ || pplist == ఖాళీ || fllist == ఖాళీ)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	తిరిగి CSCOPE_FAILURE;
    }

    కొరకు (i = 0; i < csinfo_కొలత; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	ఒకవేళ (csinfo[i].fname != ఖాళీ)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    కొరకు (i = 0; i < csinfo_కొలత; i++)
    {
	ఒకవేళ (dblist[i] != ఖాళీ)
	{
	    cs_కూడు_common(dblist[i], pplist[i], fllist[i]);
	    ఒకవేళ (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		సందేశం_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    ఒకవేళ (p_csverbose)
	సందేశం_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | సందేశం_HIST);
    తిరిగి CSCOPE_SUCCESS;
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
    స్థిర అక్షరం *
cs_resolve_file(పూర్ణాంకం i, అక్షరం *name)
{
    అక్షరం	*fullname;
    string_T	csdir = {ఖాళీ, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    పూర్ణాంకం		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (పూర్ణాంకం)namelen + 2;
    ఒకవేళ (csinfo[i].ppath != ఖాళీ)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (పూర్ణాంకం)ppathlen;
    }
    లేకపోతే ఒకవేళ (p_csre && csinfo[i].fname != ఖాళీ)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	ఒకవేళ (csdir.string != ఖాళీ)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.నిడివి = STRLEN(csdir.string);
	    len += (పూర్ణాంకం)csdir.నిడివి;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    ఒకవేళ (csinfo[i].ppath != ఖాళీ
	    && (అక్షరశ్రేణి_పోలిక(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	ఒకవేళ ((fullname = alloc(len)) != ఖాళీ)
	    (శూన్య)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    లేకపోతే ఒకవేళ (csdir.string != ఖాళీ && csinfo[i].fname != ఖాళీ && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (అక్షరం *)concat_fnames(csdir.string, csdir.నిడివి, (char_u *)name, namelen, TRUE, &ret);
    }
    లేకపోతే
    {
	fullname = (అక్షరం *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    తిరిగి fullname;
}


/*
 * Show all cscope connections.
 */
    స్థిర పూర్ణాంకం
cs_show(exarg_T *eap వాడని)
{
    పూర్ణాంకం i;

    ఒకవేళ (cs_cnt_connections() == 0)
	సందేశం_puts(_("no cscope connections\n"));
    లేకపోతే
    {
	సందేశం_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	కొరకు (i = 0; i < csinfo_కొలత; i++)
	{
	    ఒకవేళ (csinfo[i].fname == ఖాళీ)
		కొనసాగు;

	    ఒకవేళ (csinfo[i].ppath != ఖాళీ)
		(శూన్య)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (దీర్ఘ)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    లేకపోతే
		(శూన్య)smsg("%2d %-5ld  %-34s  <none>",
			   i, (దీర్ఘ)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    తిరిగి CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    శూన్య
cs_చివర(శూన్య)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < csinfo_కొలత; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_కొలత = 0;
}

#అయితే_ముగింపు	// FEAT_CSCOPE

#ఒకవేళ defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    శూన్య
f_cscope_connection(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_CSCOPE
    పూర్ణాంకం		అంకె = 0;
    char_u	*dbpath = ఖాళీ;
    char_u	*prepend = ఖాళీ;
    char_u	buf[NUMBUFLEN];

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	అంకె = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_సంఖ్య = cs_connection(అంకె, dbpath, prepend);
# endif
}

#అయితే_ముగింపు // FEAT_EVAL
