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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# ಯದಿ defined(UNIX)
#  include <sys/wait.h>
# endif

# ಯದಿ defined (MSWIN)
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ವರ್ಣ *  name;
    ಪೂರ್ಣಾಙ್ಕ     (*func)(exarg_T *eap);
    ವರ್ಣ *  help;
    ವರ್ಣ *  usage;
    ಪೂರ್ಣಾಙ್ಕ	    cansplit;		// if supports splitting window
} cscmd_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ csi {
    ವರ್ಣ *	    fname;	// cscope db name
    ವರ್ಣ *	    ppath;	// path to prepend (the -P option)
    ವರ್ಣ *	    flags;	// additional cscope flags/options (e.g, -p2)
# ಯದಿ defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# ಅನ್ಯಥಾ
#  ಯದಿ defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_Index_ಮೇಲಿನ;	// st_ino has no meaning in the Windows
    DWORD	    n_Index_ಕೆಳಗಿನ;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    to_fp;	// to cscope: FILE.
} csinfo_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ { Add, Find, Help, Kill, Reset, Show } csid_e;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_add(exarg_T *eap);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_add_common(ವರ್ಣ *, ವರ್ಣ *, ವರ್ಣ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_check_for_connections(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_check_for_tags(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_cnt_connections(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_create_connection(ಪೂರ್ಣಾಙ್ಕ i);
# ifdef FEAT_QUICKFIX
ಸ್ಥಿರ ಶೂನ್ಯ	    cs_file_results(FILE *, ಪೂರ್ಣಾಙ್ಕ *);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ	    cs_fill_results(ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ , ಪೂರ್ಣಾಙ್ಕ *, ವರ್ಣ ***,
			ವರ್ಣ ***, ಪೂರ್ಣಾಙ್ಕ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_find(exarg_T *eap);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_find_common(ವರ್ಣ *opt, ವರ್ಣ *pat, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, char_u *cmdline);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_help(exarg_T *eap);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_ಸೇರಿಸು_filelist(ವರ್ಣ *, ವರ್ಣ *, ವರ್ಣ *,
			stat_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_kill(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	    cs_kill_execute(ಪೂರ್ಣಾಙ್ಕ, ವರ್ಣ *);
ಸ್ಥಿರ cscmd_T *    cs_lookup_cmd(exarg_T *eap);
ಸ್ಥಿರ ವರ್ಣ *	    cs_make_vim_style_matches(ವರ್ಣ *, ವರ್ಣ *,
			ವರ್ಣ *, ವರ್ಣ *);
ಸ್ಥಿರ ವರ್ಣ *	    cs_manage_matches(ವರ್ಣ **, ವರ್ಣ **, ಪೂರ್ಣಾಙ್ಕ, mcmd_e);
ಸ್ಥಿರ ಶೂನ್ಯ	    cs_print_tags_priv(ವರ್ಣ **, ವರ್ಣ **, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_read_prompt(ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಶೂನ್ಯ	    cs_release_csp(ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ freefnpp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_reset(exarg_T *eap);
ಸ್ಥಿರ ವರ್ಣ *	    cs_resolve_file(ಪೂರ್ಣಾಙ್ಕ, ವರ್ಣ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    cs_show(exarg_T *eap);


ಸ್ಥಿರ csinfo_T *   csinfo = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
ಸ್ಥಿರ cscmd_T	    cs_cmds[] =
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
    { ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, 0 }
};

    ಸ್ಥಿರ ಶೂನ್ಯ
cs_usage_ಸಂದೇಶ(csid_e x)
{
    (ಶೂನ್ಯ)semsg(_(e_usage_cscope_str), cs_cmds[(ಪೂರ್ಣಾಙ್ಕ)x].usage);
}

ಸ್ಥಿರ ಗಣನಾ
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
get_cscope_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ		current_idx;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ವಿಕಲ್ಪ (expand_what)
    {
    ಪಕ್ಷ EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	ಪ್ರತ್ಯಾಗಮ (char_u *)cs_cmds[idx].name;
    ಪಕ್ಷ EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	ಪ್ರತಿ (i = 0, current_idx = 0; cs_cmds[i].name != ರಿಕ್ತ; i++)
	    ಯದಿ (cs_cmds[i].cansplit)
		ಯದಿ (current_idx++ == idx)
		    ಭಙ್ಗ;
	ಪ್ರತ್ಯಾಗಮ (char_u *)cs_cmds[i].name;
    ಪಕ್ಷ EXP_CSCOPE_FIND:
	{
	    ಧ್ರುವ ವರ್ಣ *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", ರಿಕ್ತ
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    ಪ್ರತ್ಯಾಗಮ (char_u *)query_type[idx];
	}
    ಪಕ್ಷ EXP_CSCOPE_KILL:
	{
	    ಸ್ಥಿರ ವರ್ಣ	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    ಪ್ರತಿ (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		ಯದಿ (csinfo[i].fname == ರಿಕ್ತ)
		    ಅನುವರ್ತ;
		ಯದಿ (current_idx++ == idx)
		{
		    vim_snprintf(connection, ಪರಿಮಾಣ(connection), "%d", i);
		    ಪ್ರತ್ಯಾಗಮ (char_u *)connection;
		}
	    }
	    ಪ್ರತ್ಯಾಗಮ (current_idx == idx && idx > 0) ? (char_u *)"-1" : ರಿಕ್ತ;
	}
    ಸ್ವಭಾವ:
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    ಶೂನ್ಯ
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

    ಯದಿ (*arg == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    // past first word
    xp->xp_pattern = skipwhite(p);
    ಯದಿ (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    ಅನ್ಯಥಾ
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_cscope_general(
    exarg_T	*eap,
    ಪೂರ್ಣಾಙ್ಕ		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    ಯದಿ (check_restricted())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((cmdp = cs_lookup_cmd(eap)) == ರಿಕ್ತ)
    {
	cs_help(eap);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (make_split)
    {
	ಯದಿ (!cmdp->cansplit)
	{
	    (ಶೂನ್ಯ)ಸಂದೇಶ_puts(_("This cscope command does not support splitting the window.\n"));
	    ಪ್ರತ್ಯಾಗಮ;
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
    ಶೂನ್ಯ
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    ಶೂನ್ಯ
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    ಶೂನ್ಯ
ex_cstag(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ ret = FALSE;

    ಯದಿ (*eap->arg == NUL)
    {
	(ಶೂನ್ಯ)emsg(_(e_usage_cstag_ident));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ವಿಕಲ್ಪ (p_csto)
    {
    ಪಕ್ಷ 0 :
	ಯದಿ (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (ವರ್ಣ *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ಯದಿ (ret == FALSE)
	    {
		cs_free_tags();
		ಯದಿ (ಸಂದೇಶ_col)
		    ಸಂದೇಶ_putchar('\n');

		ಯದಿ (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	ಭಙ್ಗ;
    ಪಕ್ಷ 1 :
	ಯದಿ (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    ಯದಿ (ret == FALSE)
	    {
		ಯದಿ (ಸಂದೇಶ_col)
		    ಸಂದೇಶ_putchar('\n');

		ಯದಿ (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (ವರ್ಣ *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    ಯದಿ (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (ವರ್ಣ *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ಯದಿ (ret == FALSE)
		cs_free_tags();
	}
	ಭಙ್ಗ;
    ಸ್ವಭಾವ :
	ಭಙ್ಗ;
    }

    ಯದಿ (!ret)
    {
	(ಶೂನ್ಯ)emsg(_(e_cstag_tag_not_founc));
# ಯದಿ defined(FEAT_QUICKFIX)
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
    ಪೂರ್ಣಾಙ್ಕ
cs_fgets(char_u *buf, ಪೂರ್ಣಾಙ್ಕ size)
{
    ವರ್ಣ *p;

    ಯದಿ ((p = cs_manage_matches(ರಿಕ್ತ, ರಿಕ್ತ, -1, Get)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    ಪ್ರತ್ಯಾಗಮ FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    ಶೂನ್ಯ
cs_free_tags(ಶೂನ್ಯ)
{
    cs_manage_matches(ರಿಕ್ತ, ರಿಕ್ತ, -1, Free);
}


/*
 * Called from do_tag().
 */
    ಶೂನ್ಯ
cs_print_tags(ಶೂನ್ಯ)
{
    cs_manage_matches(ರಿಕ್ತ, ರಿಕ್ತ, -1, Print);
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
# ಯದಿ defined(FEAT_EVAL)
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_connection(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ, char_u *dbpath, char_u *ppath)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (ಅಂಕೆ < 0 || ಅಂಕೆ > 4 || (ಅಂಕೆ > 0 && !dbpath))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
    {
	ಯದಿ (!csinfo[i].fname)
	    ಅನುವರ್ತ;

	ಯದಿ (ಅಂಕೆ == 0)
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	ವಿಕಲ್ಪ (ಅಂಕೆ)
	{
	ಪಕ್ಷ 1:
	    ಯದಿ (strstr(csinfo[i].fname, (ವರ್ಣ *)dbpath))
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    ಭಙ್ಗ;
	ಪಕ್ಷ 2:
	    ಯದಿ (strcmp(csinfo[i].fname, (ವರ್ಣ *)dbpath) == 0)
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    ಭಙ್ಗ;
	ಪಕ್ಷ 3:
	    ಯದಿ (strstr(csinfo[i].fname, (ವರ್ಣ *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (ವರ್ಣ *)ppath))))
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    ಭಙ್ಗ;
	ಪಕ್ಷ 4:
	    ಯದಿ ((strcmp(csinfo[i].fname, (ವರ್ಣ *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (ವರ್ಣ *)ppath) == 0))))
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    ಭಙ್ಗ;
	}
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_add(exarg_T *eap UNUSED)
{
    ವರ್ಣ *fname, *ppath, *flags = ರಿಕ್ತ;

    ಯದಿ ((fname = strtok((ವರ್ಣ *)ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)" ")) == ರಿಕ್ತ)
    {
	cs_usage_ಸಂದೇಶ(Add);
	ಪ್ರತ್ಯಾಗಮ CSCOPE_FAILURE;
    }
    ಯದಿ ((ppath = strtok((ವರ್ಣ *)ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)" ")) != ರಿಕ್ತ)
	flags = strtok((ವರ್ಣ *)ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)" ");

    ಪ್ರತ್ಯಾಗಮ cs_add_common(fname, ppath, flags);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cs_stat_emsg(ವರ್ಣ *fname)
{
    ಪೂರ್ಣಾಙ್ಕ err = errno;
    (ಶೂನ್ಯ)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_add_common(
    ವರ್ಣ *arg1,	    // filename - may contain environment variables
    ವರ್ಣ *arg2,	    // prepend path - may contain environment variables
    ವರ್ಣ *flags)
{
    stat_T	statbuf;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    ವರ್ಣ	*fname = ರಿಕ್ತ;
    ವರ್ಣ	*fname2 = ರಿಕ್ತ;
    ವರ್ಣ	*ppath = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = ರಿಕ್ತ;

    // get the filename (arg1), expand it, and try to stat it
    ಯದಿ ((fname = alloc(MAXPATHL + 1)) == ರಿಕ್ತ)
	ಗಚ್ಛ add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (ಶೂನ್ಯ)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    ಯದಿ (fname == ರಿಕ್ತ)
	ಗಚ್ಛ add_err;
    fname = (ವರ್ಣ *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    ಯದಿ (ret < 0)
    {
staterr:
	ಯದಿ (p_csverbose)
	    cs_stat_emsg(fname);
	ಗಚ್ಛ add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    ಯದಿ (arg2 != ರಿಕ್ತ)
    {
	stat_T	    statbuf2;

	ಯದಿ ((ppath = alloc(MAXPATHL + 1)) == ರಿಕ್ತ)
	    ಗಚ್ಛ add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	ಯದಿ (ret < 0)
	    ಗಚ್ಛ staterr;
    }

    // if filename is a directory, append the cscope database name to it
    ಯದಿ (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	ಯದಿ (fname2 == ರಿಕ್ತ)
	    ಗಚ್ಛ add_err;

	ಯಾವತ್ (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    ಯದಿ (fname[0] == '\0')
		ಭಙ್ಗ;
	}
	ಯದಿ (fname[0] == '\0')
	    (ಶೂನ್ಯ)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	ಅನ್ಯಥಾ
	    (ಶೂನ್ಯ)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	ಯದಿ (ret < 0)
	{
	    ಯದಿ (p_csverbose)
		cs_stat_emsg(fname2);
	    ಗಚ್ಛ add_err;
	}

	i = cs_ಸೇರಿಸು_filelist(fname2, ppath, flags, &statbuf);
    }
    ಅನ್ಯಥಾ ಯದಿ (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_ಸೇರಿಸು_filelist(fname, ppath, flags, &statbuf);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (p_csverbose)
	    (ಶೂನ್ಯ)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	ಗಚ್ಛ add_err;
    }

    ಯದಿ (i != -1)
    {
	ಯದಿ (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    ಗಚ್ಛ add_err;
	}

	ಯದಿ (p_csverbose)
	{
	    ಸಂದೇಶ_clr_eos();
	    (ಶೂನ್ಯ)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    ಪ್ರತ್ಯಾಗಮ CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    ಪ್ರತ್ಯಾಗಮ CSCOPE_FAILURE;
}


    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_check_for_connections(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (cs_cnt_connections() > 0);
}


    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_check_for_tags(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (p_tags[0] != NUL && curbuf->b_p_tags != ರಿಕ್ತ);
}


/*
 * Count the number of cscope connections.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_cnt_connections(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪೂರ್ಣಾಙ್ಕ cnt = 0;

    ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
	ಯದಿ (csinfo[i].fname != ರಿಕ್ತ)
	    cnt++;
    ಪ್ರತ್ಯಾಗಮ cnt;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cs_reading_emsg(
    ಪೂರ್ಣಾಙ್ಕ idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_cnt_matches(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ವರ್ಣ *stok;
    ವರ್ಣ *buf;
    ಪೂರ್ಣಾಙ್ಕ nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    ಯದಿ (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	ಯದಿ (strstr((ಧ್ರುವ ವರ್ಣ *)buf, "Unable to search database") != ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಯದಿ ((stok = strtok(buf, (ಧ್ರುವ ವರ್ಣ *)" ")) == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	ಯದಿ (strstr((ಧ್ರುವ ವರ್ಣ *)stok, "cscope:") == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	ಯದಿ ((stok = strtok(ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)" ")) == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	nlines = atoi(stok);
	ಯದಿ (nlines < 0)
	{
	    nlines = 0;
	    ಭಙ್ಗ;
	}

	ಯದಿ ((stok = strtok(ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)" ")) == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	ಯದಿ (strncmp((ಧ್ರುವ ವರ್ಣ *)stok, "lines", 5))
	    ಅನುವರ್ತ;

	ಭಙ್ಗ;
    }

    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    ಸ್ಥಿರ ವರ್ಣ *
cs_create_cmd(ವರ್ಣ *csoption, ವರ್ಣ *pattern)
{
    ವರ್ಣ *cmd;
    short ಹುಡುಕು;
    ವರ್ಣ *pat;

    ವಿಕಲ್ಪ (csoption[0])
    {
    ಪಕ್ಷ '0' : ಪಕ್ಷ 's' :
	ಹುಡುಕು = 0;
	ಭಙ್ಗ;
    ಪಕ್ಷ '1' : ಪಕ್ಷ 'g' :
	ಹುಡುಕು = 1;
	ಭಙ್ಗ;
    ಪಕ್ಷ '2' : ಪಕ್ಷ 'd' :
	ಹುಡುಕು = 2;
	ಭಙ್ಗ;
    ಪಕ್ಷ '3' : ಪಕ್ಷ 'c' :
	ಹುಡುಕು = 3;
	ಭಙ್ಗ;
    ಪಕ್ಷ '4' : ಪಕ್ಷ 't' :
	ಹುಡುಕು = 4;
	ಭಙ್ಗ;
    ಪಕ್ಷ '6' : ಪಕ್ಷ 'e' :
	ಹುಡುಕು = 6;
	ಭಙ್ಗ;
    ಪಕ್ಷ '7' : ಪಕ್ಷ 'f' :
	ಹುಡುಕು = 7;
	ಭಙ್ಗ;
    ಪಕ್ಷ '8' : ಪಕ್ಷ 'i' :
	ಹುಡುಕು = 8;
	ಭಙ್ಗ;
    ಪಕ್ಷ '9' : ಪಕ್ಷ 'a' :
	ಹುಡುಕು = 9;
	ಭಙ್ಗ;
    ಸ್ವಭಾವ :
	(ಶೂನ್ಯ)emsg(_(e_unknown_cscope_ಹುಡುಕು_type));
	cs_usage_ಸಂದೇಶ(Find);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    ಯದಿ (ಹುಡುಕು != 4 && ಹುಡುಕು != 6)
	ಯಾವತ್ VIM_ISWHITE(*pat)
	    ++pat;

    ಯದಿ ((cmd = alloc(strlen(pat) + 2)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    (ಶೂನ್ಯ)sprintf(cmd, "%d%s", ಹುಡುಕು, pat);

    ಪ್ರತ್ಯಾಗಮ cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_create_connection(ಪೂರ್ಣಾಙ್ಕ i)
{
# ifdef UNIX
    ಪೂರ್ಣಾಙ್ಕ		to_cs[2], from_cs[2];
# endif
    ಪೂರ್ಣಾಙ್ಕ		cmdlen;
    ವರ್ಣ	*prog, *cmd, *ppath = ರಿಕ್ತ;
    size_t	proglen;
# ifdef MSWIN
    ಪೂರ್ಣಾಙ್ಕ		fd;
    ಪೂರ್ಣಾಙ್ಕ		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# ಯದಿ defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    ಯದಿ (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(ಶೂನ್ಯ)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	ಯದಿ (to_cs[0] != -1)
	    (ಶೂನ್ಯ)close(to_cs[0]);
	ಯದಿ (to_cs[1] != -1)
	    (ಶೂನ್ಯ)close(to_cs[1]);
	ಯದಿ (from_cs[0] != -1)
	    (ಶೂನ್ಯ)close(from_cs[0]);
	ಯದಿ (from_cs[1] != -1)
	    (ಶೂನ್ಯ)close(from_cs[1]);
	ಪ್ರತ್ಯಾಗಮ CSCOPE_FAILURE;
    }

    ಯದಿ ((csinfo[i].pid = fork()) == -1)
    {
	(ಶೂನ್ಯ)emsg(_(e_could_not_fork_for_cscope));
	ಗಚ್ಛ err_closing;
    }
    ಅನ್ಯಥಾ ಯದಿ (csinfo[i].pid == 0)	// child: run cscope.
    {
	ವರ್ಣ **argv = ರಿಕ್ತ;

	ಯದಿ (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	ಯದಿ (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	ಯದಿ (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(ಶೂನ್ಯ)close(to_cs[1]);
	(ಶೂನ್ಯ)close(from_cs[0]);
# ಅನ್ಯಥಾ
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = ಪರಿಮಾಣ(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ರಿಕ್ತ;

	ಯದಿ (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (ಶೂನ್ಯ)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    ಯದಿ (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    ಯದಿ (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    ಪ್ರತ್ಯಾಗಮ CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	ಯದಿ ((prog = alloc(MAXPATHL + 1)) == ರಿಕ್ತ)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# ಅನ್ಯಥಾ
	    // MSWIN
	    ಗಚ್ಛ err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (ಪೂರ್ಣಾಙ್ಕ)(proglen + strlen(csinfo[i].fname) + 32);
	ಯದಿ (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    ಯದಿ ((ppath = alloc(MAXPATHL + 1)) == ರಿಕ್ತ)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# ಅನ್ಯಥಾ
		// MSWIN
		ಗಚ್ಛ err_closing;
# endif
	    }
	    cmdlen += (ಪೂರ್ಣಾಙ್ಕ)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	ಯದಿ (csinfo[i].flags)
	    cmdlen += (ಪೂರ್ಣಾಙ್ಕ)strlen(csinfo[i].flags);

	ಯದಿ ((cmd = alloc(cmdlen)) == ರಿಕ್ತ)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# ಅನ್ಯಥಾ
	    // MSWIN
	    ಗಚ್ಛ err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    ವರ್ಣ	**tok = ರಿಕ್ತ;
	    ಪೂರ್ಣಾಙ್ಕ		tokc = 0;

	    ga_init2(&ga_argv, ಪರಿಮಾಣ(ವರ್ಣ *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    ಯದಿ (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < tokc; ++k)
	    {
		ಯದಿ (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((ವರ್ಣ **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    ಯದಿ (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((ವರ್ಣ **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (ವರ್ಣ *)vim_strsave((char_u *)"-dl");
	    ((ವರ್ಣ **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (ವರ್ಣ *)vim_strsave((char_u *)"-f");
	    ((ವರ್ಣ **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(ವರ್ಣ *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    ಯದಿ (ppath != ರಿಕ್ತ)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		ವರ್ಣ	*parg = alloc(plen);

		ಯದಿ (parg == ರಿಕ್ತ || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((ವರ್ಣ **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    ಯದಿ (csinfo[i].flags != ರಿಕ್ತ)
	    {
		ಯದಿ (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < tokc; ++k)
		{
		    ಯದಿ (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((ವರ್ಣ **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    ಯದಿ (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((ವರ್ಣ **)ga_argv.ga_data)[ga_argv.ga_len] = ರಿಕ್ತ;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  ಯದಿ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   ಯದಿ defined(HAVE_SETSID)
	    (ಶೂನ್ಯ)setsid();
#   ಅನ್ಯಥಾ
	    ಯದಿ (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (ವರ್ಣ **)ga_argv.ga_data;
	    ಯದಿ (argv[0] == ರಿಕ್ತ || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    ಅನ್ಯಥಾ	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	ಯದಿ ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == ರಿಕ್ತ)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	ಯದಿ ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == ರಿಕ್ತ)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(ಶೂನ್ಯ)close(to_cs[0]);
	(ಶೂನ್ಯ)close(from_cs[1]);
    }
# ಅನ್ಯಥಾ
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    ಯದಿ (csinfo[i].ppath != ರಿಕ್ತ)
    {
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    ಯದಿ (csinfo[i].flags != ರಿಕ್ತ)
    {
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_ಒಳಹರಿವು  = stdin_rd;
    created = CreateProcess(ರಿಕ್ತ, cmd, ರಿಕ್ತ, ರಿಕ್ತ, TRUE, CREATE_NEW_CONSOLE,
							ರಿಕ್ತ, ರಿಕ್ತ, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    ಯದಿ (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(ಶೂನ್ಯ)emsg(_(e_could_not_spawn_cscope_process));
	ಗಚ್ಛ err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    ಯದಿ (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == ರಿಕ್ತ))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    ಯದಿ (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == ರಿಕ್ತ))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    ಪ್ರತ್ಯಾಗಮ CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_find(exarg_T *eap)
{
    ವರ್ಣ *opt, *pat;
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (cs_check_for_connections() == FALSE)
    {
	(ಶೂನ್ಯ)emsg(_(e_no_cscope_connections));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಯದಿ ((opt = strtok((ವರ್ಣ *)ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)" ")) == ರಿಕ್ತ)
    {
	cs_usage_ಸಂದೇಶ(Find);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    pat = opt + strlen(opt) + 1;
    ಯದಿ (pat >= (ವರ್ಣ *)eap->arg + eap_arg_len)
    {
	cs_usage_ಸಂದೇಶ(Find);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    ಪ್ರತಿ (i = 0; i < eap_arg_len; ++i)
	ಯದಿ (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    ಪ್ರತ್ಯಾಗಮ cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_find_common(
    ವರ್ಣ *opt,
    ವರ್ಣ *pat,
    ಪೂರ್ಣಾಙ್ಕ forceit,
    ಪೂರ್ಣಾಙ್ಕ verbose,
    ಪೂರ್ಣಾಙ್ಕ	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ವರ್ಣ *cmd;
    ಪೂರ್ಣಾಙ್ಕ *nummatches;
    ಪೂರ್ಣಾಙ್ಕ totmatches;
# ifdef FEAT_QUICKFIX
    ವರ್ಣ cmdletter;
    ವರ್ಣ *qfpos;

    // get cmd letter
    ವಿಕಲ್ಪ (opt[0])
    {
    ಪಕ್ಷ '0' :
	cmdletter = 's';
	ಭಙ್ಗ;
    ಪಕ್ಷ '1' :
	cmdletter = 'g';
	ಭಙ್ಗ;
    ಪಕ್ಷ '2' :
	cmdletter = 'd';
	ಭಙ್ಗ;
    ಪಕ್ಷ '3' :
	cmdletter = 'c';
	ಭಙ್ಗ;
    ಪಕ್ಷ '4' :
	cmdletter = 't';
	ಭಙ್ಗ;
    ಪಕ್ಷ '6' :
	cmdletter = 'e';
	ಭಙ್ಗ;
    ಪಕ್ಷ '7' :
	cmdletter = 'f';
	ಭಙ್ಗ;
    ಪಕ್ಷ '8' :
	cmdletter = 'i';
	ಭಙ್ಗ;
    ಪಕ್ಷ '9' :
	cmdletter = 'a';
	ಭಙ್ಗ;
    ಸ್ವಭಾವ :
	cmdletter = opt[0];
    }

    qfpos = (ವರ್ಣ *)vim_strchr(p_csqf, cmdletter);
    ಯದಿ (qfpos != ರಿಕ್ತ)
    {
	qfpos++;
	// next symbol must be + or -
	ಯದಿ (strchr(CSQF_FLAGS, *qfpos) == ರಿಕ್ತ)
	{
	    (ಶೂನ್ಯ)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	ಯದಿ (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    ಯದಿ (aborting())
		ಪ್ರತ್ಯಾಗಮ FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    ಯದಿ (cmd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    nummatches = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, csinfo_size);
    ಯದಿ (nummatches == ರಿಕ್ತ)
    {
	vim_free(cmd);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
    {
	ಯದಿ (csinfo[i].fname == ರಿಕ್ತ || csinfo[i].to_fp == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	// send cmd to cscope
	(ಶೂನ್ಯ)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(ಶೂನ್ಯ)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	ಯದಿ (nummatches[i] > -1)
	    totmatches += nummatches[i];

	ಯದಿ (nummatches[i] == 0)
	    (ಶೂನ್ಯ)cs_read_prompt(i);
    }
    vim_free(cmd);

    ಯದಿ (totmatches == 0)
    {
	ಯದಿ (verbose)
	    (ಶೂನ್ಯ)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

# ifdef FEAT_QUICKFIX
    ಯದಿ (qfpos != ರಿಕ್ತ && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = ರಿಕ್ತ;
	win_T	    *wp = ರಿಕ್ತ;

	ಯದಿ (tmp == ರಿಕ್ತ)
	{
	    vim_free(nummatches);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	f = mch_fopen((ವರ್ಣ *)tmp, "w");
	ಯದಿ (f == ರಿಕ್ತ)
	    semsg(_(e_cant_open_file_str), tmp);
	ಅನ್ಯಥಾ
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    ಯದಿ (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    ಯದಿ (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, ರಿಕ್ತ) > 0)
	    {
		ಯದಿ (postponed_split != 0)
		{
		    (ಶೂನ್ಯ)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		ಯದಿ (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != ರಿಕ್ತ)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಅನ್ಯಥಾ
# endif // FEAT_QUICKFIX
    {
	ವರ್ಣ **matches = ರಿಕ್ತ, **contexts = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	ಯದಿ (matches == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	(ಶೂನ್ಯ)cs_manage_matches(matches, contexts, matched, Store);

	ಪ್ರತ್ಯಾಗಮ do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (ಶೂನ್ಯ)ಸಂದೇಶ_puts(_("cscope commands:\n"));
    ಯಾವತ್ (cmdp->name != ರಿಕ್ತ)
    {
	ವರ್ಣ *help = _(cmdp->help);
	ಪೂರ್ಣಾಙ್ಕ  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	ಯದಿ (space_cnt < 0)
	    space_cnt = 0;
	(ಶೂನ್ಯ)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	ಯದಿ (strcmp(cmdp->name, "find") == 0)
	    ಸಂದೇಶ_puts(_("\n"
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
    ಪ್ರತ್ಯಾಗಮ 0;
}


    ಸ್ಥಿರ ಶೂನ್ಯ
clear_csinfo(ಪೂರ್ಣಾಙ್ಕ i)
{
    csinfo[i].fname  = ರಿಕ್ತ;
    csinfo[i].ppath  = ರಿಕ್ತ;
    csinfo[i].flags  = ರಿಕ್ತ;
# ಯದಿ defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# ಅನ್ಯಥಾ
    csinfo[i].nVolume = 0;
    csinfo[i].n_Index_ಮೇಲಿನ = 0;
    csinfo[i].n_Index_ಕೆಳಗಿನ = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = ರಿಕ್ತ;
    csinfo[i].to_fp  = ರಿಕ್ತ;
# ಯದಿ defined(MSWIN)
    csinfo[i].hProc = ರಿಕ್ತ;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_ಸೇರಿಸು_filelist(
    ವರ್ಣ *fname,
    ವರ್ಣ *ppath,
    ವರ್ಣ *flags,
    stat_T *sb UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    ವಿಕಲ್ಪ (win32_fileinfo((char_u *)fname, &bhfi))
    {
	ಪಕ್ಷ FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	ಪಕ್ಷ FILEINFO_READ_FAIL:	// CreateFile() failed
	    ಯದಿ (p_csverbose)
	    {
		ವರ್ಣ *cant_ಸಂದೇಶ = _(e_cannot_open_cscope_database_str);
		ವರ್ಣ *winmsg = GetWin32Error();

		ಯದಿ (winmsg != ರಿಕ್ತ)
		    (ಶೂನ್ಯ)semsg(cant_ಸಂದೇಶ, winmsg);
		ಅನ್ಯಥಾ
		    // subst filename if can't get error text
		    (ಶೂನ್ಯ)semsg(cant_ಸಂದೇಶ, fname);
	    }
	    ಪ್ರತ್ಯಾಗಮ -1;

	ಪಕ್ಷ FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    ಯದಿ (p_csverbose)
		(ಶೂನ್ಯ)emsg(_(e_cannot_get_cscope_database_information));
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    ಪ್ರತಿ (j = 0; j < csinfo_size; j++)
    {
	ಯದಿ (csinfo[j].fname != ರಿಕ್ತ
# ಯದಿ defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# ಅನ್ಯಥಾ
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_ಸಂಖ್ಯೆ
		    && csinfo[j].n_Index_ಮೇಲಿನ == bhfi.n_File_Index_ಮೇಲಿನ
		    && csinfo[j].n_Index_ಕೆಳಗಿನ == bhfi.n_File_Index_ಕೆಳಗಿನ))
# endif
	    )
	{
	    ಯದಿ (p_csverbose)
		(ಶೂನ್ಯ)emsg(_(e_duplicate_cscope_database_not_added));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (csinfo[j].fname == ರಿಕ್ತ && i == -1)
	    i = j; // remember first empty entry
    }

    ಯದಿ (i == -1)
    {
	i = csinfo_size;
	ಯದಿ (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	ಅನ್ಯಥಾ
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, ಪರಿಮಾಣ(csinfo_T)*csinfo_size);
	    ಯದಿ (csinfo == ರಿಕ್ತ)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		ಪ್ರತ್ಯಾಗಮ -1;
	    }
	}
	ಯದಿ (csinfo == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಪ್ರತಿ (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    ಯದಿ ((csinfo[i].fname = alloc(strlen(fname)+1)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((ಧ್ರುವ ವರ್ಣ *)fname));

    ಯದಿ (ppath != ರಿಕ್ತ)
    {
	ಯದಿ ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == ರಿಕ್ತ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((ಧ್ರುವ ವರ್ಣ *)ppath));
    }
    ಅನ್ಯಥಾ
	csinfo[i].ppath = ರಿಕ್ತ;

    ಯದಿ (flags != ರಿಕ್ತ)
    {
	ಯದಿ ((csinfo[i].flags = alloc(strlen(flags) + 1)) == ರಿಕ್ತ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((ಧ್ರುವ ವರ್ಣ *)flags));
    }
    ಅನ್ಯಥಾ
	csinfo[i].flags = ರಿಕ್ತ;

# ಯದಿ defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# ಅನ್ಯಥಾ
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_ಸಂಖ್ಯೆ;
    csinfo[i].n_Index_ಕೆಳಗಿನ = bhfi.n_File_Index_ಕೆಳಗಿನ;
    csinfo[i].n_Index_ಮೇಲಿನ = bhfi.n_File_Index_ಮೇಲಿನ;
# endif
    ಪ್ರತ್ಯಾಗಮ i;
}


/*
 * Find cscope command in command table.
 */
    ಸ್ಥಿರ cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    ವರ್ಣ *stok;
    size_t len;

    ಯದಿ (eap->arg == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(eap->arg);

    ಯದಿ ((stok = strtok((ವರ್ಣ *)(eap->arg), (ಧ್ರುವ ವರ್ಣ *)" ")) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    len = strlen(stok);
    ಪ್ರತಿ (cmdp = cs_cmds; cmdp->name != ರಿಕ್ತ; ++cmdp)
    {
	ಯದಿ (strncmp((ಧ್ರುವ ವರ್ಣ *)(stok), cmdp->name, len) == 0)
	    ಪ್ರತ್ಯಾಗಮ (cmdp);
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}


/*
 * Nuke em.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_kill(exarg_T *eap UNUSED)
{
    ವರ್ಣ *stok;
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ ((stok = strtok((ವರ್ಣ *)ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)" ")) == ರಿಕ್ತ)
    {
	cs_usage_ಸಂದೇಶ(Kill);
	ಪ್ರತ್ಯಾಗಮ CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    ಯದಿ ((strlen(stok) < 2 && VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(stok[1]))))
	i = atoi(stok);
    ಅನ್ಯಥಾ
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
	{
	    ಯದಿ (csinfo[i].fname != ರಿಕ್ತ && strstr(csinfo[i].fname, stok))
		ಭಙ್ಗ;
	}
    }

    ಯದಿ ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == ರಿಕ್ತ))
    {
	ಯದಿ (p_csverbose)
	    (ಶೂನ್ಯ)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (i == -1)
	{
	    ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
	    {
		ಯದಿ (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	ಅನ್ಯಥಾ
	    cs_kill_execute(i, stok);
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cs_kill_execute(
    ಪೂರ್ಣಾಙ್ಕ i,	    // cscope table index
    ವರ್ಣ *cname)    // cscope database name
{
    ಯದಿ (p_csverbose)
    {
	ಸಂದೇಶ_clr_eos();
	(ಶೂನ್ಯ)smsg_attr(HL_ATTR(HLF_R) | ಸಂದೇಶ_HIST,
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
    ಸ್ಥಿರ ವರ್ಣ *
cs_make_vim_style_matches(
    ವರ್ಣ *fname,
    ವರ್ಣ *slno,
    ವರ್ಣ *ಹುಡುಕು,
    ವರ್ಣ *tagstr)
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
    ವರ್ಣ *buf;
    ಪೂರ್ಣಾಙ್ಕ amt;

    ಯದಿ (ಹುಡುಕು != ರಿಕ್ತ)
    {
	amt = (ಪೂರ್ಣಾಙ್ಕ)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(ಹುಡುಕು)+6);
	ಯದಿ ((buf = alloc(amt)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	(ಶೂನ್ಯ)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, ಹುಡುಕು);
    }
    ಅನ್ಯಥಾ
    {
	amt = (ಪೂರ್ಣಾಙ್ಕ)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	ಯದಿ ((buf = alloc(amt)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	(ಶೂನ್ಯ)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    ಪ್ರತ್ಯಾಗಮ buf;
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
    ಸ್ಥಿರ ವರ್ಣ *
cs_manage_matches(
    ವರ್ಣ **matches,
    ವರ್ಣ **contexts,
    ಪೂರ್ಣಾಙ್ಕ totmatches,
    mcmd_e cmd)
{
    ಸ್ಥಿರ ವರ್ಣ **mp = ರಿಕ್ತ;
    ಸ್ಥಿರ ವರ್ಣ **cp = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cnt = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ next = -1;
    ವರ್ಣ *p = ರಿಕ್ತ;

    ವಿಕಲ್ಪ (cmd)
    {
    ಪಕ್ಷ Store:
	ಪ್ರತಿಜ್ಞಾ(matches != ರಿಕ್ತ);
	ಪ್ರತಿಜ್ಞಾ(totmatches > 0);
	ಯದಿ (mp != ರಿಕ್ತ || cp != ರಿಕ್ತ)
	    (ಶೂನ್ಯ)cs_manage_matches(ರಿಕ್ತ, ರಿಕ್ತ, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	ಭಙ್ಗ;
    ಪಕ್ಷ Get:
	ಯದಿ (next >= cnt)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	p = mp[next];
	next++;
	ಭಙ್ಗ;
    ಪಕ್ಷ Free:
	ಯದಿ (mp != ರಿಕ್ತ)
	{
	    ಯದಿ (cnt > 0)
		ಯಾವತ್ (cnt--)
		{
		    vim_free(mp[cnt]);
		    ಯದಿ (cp != ರಿಕ್ತ)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = ರಿಕ್ತ;
	cp = ರಿಕ್ತ;
	cnt = 0;
	next = 0;
	ಭಙ್ಗ;
    ಪಕ್ಷ Print:
	cs_print_tags_priv(mp, cp, cnt);
	ಭಙ್ಗ;
    ಸ್ವಭಾವ:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ p;
}


/*
 * Parse cscope output.
 */
    ಸ್ಥಿರ ವರ್ಣ *
cs_parse_results(
    ಪೂರ್ಣಾಙ್ಕ cnumber,
    ವರ್ಣ *buf,
    ಪೂರ್ಣಾಙ್ಕ bufsize,
    ವರ್ಣ **context,
    ವರ್ಣ **linenumber,
    ವರ್ಣ **ಹುಡುಕು)
{
    ಪೂರ್ಣಾಙ್ಕ ch;
    ವರ್ಣ *p;
    ವರ್ಣ *name;

    ಯದಿ (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == ರಿಕ್ತ)
    {
	ಯದಿ (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // If the line's too long for the buffer, discard it.
    ಯದಿ ((p = strchr(buf, '\n')) == ರಿಕ್ತ)
    {
	ಯಾವತ್ ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    ಯದಿ ((name = strtok(buf, (ಧ್ರುವ ವರ್ಣ *)" ")) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ ((*context = strtok(ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)" ")) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ ((*linenumber = strtok(ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)" ")) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    *ಹುಡುಕು = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    ಯದಿ (strcmp(*ಹುಡುಕು, "<unknown>") == 0)
	*ಹುಡುಕು = ರಿಕ್ತ;

    name = cs_resolve_file(cnumber, name);
    ಪ್ರತ್ಯಾಗಮ name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cs_file_results(FILE *f, ಪೂರ್ಣಾಙ್ಕ *nummatches_a)
{
    ಪೂರ್ಣಾಙ್ಕ i, j;
    ವರ್ಣ *buf;
    ವರ್ಣ *ಹುಡುಕು, *slno;
    ವರ್ಣ *fullname;
    ವರ್ಣ *cntx;
    ವರ್ಣ *context;

    buf = alloc(CSREAD_BUFSIZE);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
    {
	ಯದಿ (nummatches_a[i] < 1)
	    ಅನುವರ್ತ;

	ಪ್ರತಿ (j = 0; j < nummatches_a[i]; j++)
	{
	   ಯದಿ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &ಹುಡುಕು)) == ರಿಕ್ತ)
	       ಅನುವರ್ತ;

	   context = alloc(strlen(cntx)+5);
	   ಯದಿ (context == ರಿಕ್ತ)
	   {
	       vim_free(fullname);
	       ಅನುವರ್ತ;
	   }

	   ಯದಿ (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   ಅನ್ಯಥಾ
	       sprintf(context, "<<%s>>", cntx);

	   ಯದಿ (ಹುಡುಕು == ರಿಕ್ತ)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   ಅನ್ಯಥಾ
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, ಹುಡುಕು);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(ಶೂನ್ಯ)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cs_fill_results(
    ವರ್ಣ *tagstr,
    ಪೂರ್ಣಾಙ್ಕ totmatches,
    ಪೂರ್ಣಾಙ್ಕ *nummatches_a,
    ವರ್ಣ ***matches_p,
    ವರ್ಣ ***cntxts_p,
    ಪೂರ್ಣಾಙ್ಕ *matched)
{
    ಪೂರ್ಣಾಙ್ಕ i, j;
    ವರ್ಣ *buf;
    ವರ್ಣ *ಹುಡುಕು, *slno;
    ಪೂರ್ಣಾಙ್ಕ totsofar = 0;
    ವರ್ಣ **matches = ರಿಕ್ತ;
    ವರ್ಣ **cntxts = ರಿಕ್ತ;
    ವರ್ಣ *fullname;
    ವರ್ಣ *cntx;

    ಪ್ರತಿಜ್ಞಾ(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((matches = ALLOC_MULT(ವರ್ಣ *, totmatches)) == ರಿಕ್ತ)
	ಗಚ್ಛ parse_out;
    ಯದಿ ((cntxts = ALLOC_MULT(ವರ್ಣ *, totmatches)) == ರಿಕ್ತ)
	ಗಚ್ಛ parse_out;

    ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
    {
	ಯದಿ (nummatches_a[i] < 1)
	    ಅನುವರ್ತ;

	ಪ್ರತಿ (j = 0; j < nummatches_a[i]; j++)
	{
	   ಯದಿ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &ಹುಡುಕು)) == ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  ಹುಡುಕು, tagstr);

	    vim_free(fullname);

	    ಯದಿ (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (ವರ್ಣ *)vim_strsave((char_u *)cntx);

	    ಯದಿ (matches[totsofar] != ರಿಕ್ತ)
		totsofar++;

	} // for all matches

	(ಶೂನ್ಯ)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    ಯದಿ (totsofar == 0)
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
    ಸ್ಥಿರ ವರ್ಣ *
cs_pathcomponents(ವರ್ಣ *path)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ವರ್ಣ	*s;

    ಯದಿ (p_cspc == 0)
	ಪ್ರತ್ಯಾಗಮ path;

    s = path + strlen(path) - 1;
    ಪ್ರತಿ (i = 0; i < p_cspc; ++i)
    {
	ಯಾವತ್ (s > path)
	{
	   s--;
	   ಯದಿ (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      ಭಙ್ಗ;
	}
    }
    ಯದಿ ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    ಪ್ರತ್ಯಾಗಮ s;
}

/*
 * Called from cs_manage_matches().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cs_print_tags_priv(ವರ್ಣ **matches, ವರ್ಣ **cntxts, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_matches)
{
    ವರ್ಣ	*buf = ರಿಕ್ತ;
    ವರ್ಣ	*t_buf;
    ಪೂರ್ಣಾಙ್ಕ		bufsize = 0; // Track available bufsize
    ಪೂರ್ಣಾಙ್ಕ		newsize = 0;
    ವರ್ಣ	*ptag;
    ವರ್ಣ	*fname, *lno, *extra, *tbuf;
    ಪೂರ್ಣಾಙ್ಕ		i, idx, ಅಂಕೆ;
    ವರ್ಣ	*globalcntx = "GLOBAL";
    ವರ್ಣ	*cntxformat = " <<%s>>";
    ವರ್ಣ	*context;
    ವರ್ಣ	*cstag_ಸಂದೇಶ = _("Cscope tag: %s");
    ವರ್ಣ	*csfmt_str = "%4d %6s  ";

    ಪ್ರತಿಜ್ಞಾ(ಅಂಕೆ_matches > 0);

    ಯದಿ ((tbuf = alloc(strlen(matches[0]) + 1)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    ಯದಿ (ptag == ರಿಕ್ತ)
    {
	vim_free(tbuf);
	ಪ್ರತ್ಯಾಗಮ;
    }

    newsize = (ಪೂರ್ಣಾಙ್ಕ)(strlen(cstag_ಸಂದೇಶ) + strlen(ptag));
    buf = alloc(newsize);
    ಯದಿ (buf != ರಿಕ್ತ)
    {
	bufsize = newsize;
	(ಶೂನ್ಯ)sprintf(buf, cstag_ಸಂದೇಶ, ptag);
	ಸಂದೇಶ_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    ಸಂದೇಶ_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    ಸಂದೇಶ_advance(ಸಂದೇಶ_col + 2);
    ಸಂದೇಶ_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    ಅಂಕೆ = 1;
    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	ಯದಿ ((tbuf = alloc(strlen(matches[idx]) + 1)) == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	(ಶೂನ್ಯ)strcpy(tbuf, matches[idx]);

	ಯದಿ (strtok(tbuf, (ಧ್ರುವ ವರ್ಣ *)"\t") == ರಿಕ್ತ
		|| (fname = strtok(ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)"\t")) == ರಿಕ್ತ
		|| (lno = strtok(ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)"\t")) == ರಿಕ್ತ)
	{
	    vim_free(tbuf);
	    ಅನುವರ್ತ;
	}
	extra = strtok(ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (ಪೂರ್ಣಾಙ್ಕ)(strlen(csfmt_str) + 16 + strlen(lno));
	ಯದಿ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ಯದಿ (buf == ರಿಕ್ತ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ಅನ್ಯಥಾ
		bufsize = newsize;
	}
	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    // csfmt_str = "%4d %6s  ";
	    (ಶೂನ್ಯ)sprintf(buf, csfmt_str, ಅಂಕೆ, lno);
	    ಸಂದೇಶ_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	ಸಂದೇಶ_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	ಯದಿ (cntxts[idx] != ರಿಕ್ತ)
	    context = cntxts[idx];
	ಅನ್ಯಥಾ
	    context = globalcntx;
	newsize = (ಪೂರ್ಣಾಙ್ಕ)(strlen(context) + strlen(cntxformat));

	ಯದಿ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ಯದಿ (buf == ರಿಕ್ತ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ಅನ್ಯಥಾ
		bufsize = newsize;
	}
	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    (ಶೂನ್ಯ)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    ಯದಿ (ಸಂದೇಶ_col + (ಪೂರ್ಣಾಙ್ಕ)strlen(buf) >= cmdline_width)
		ಸಂದೇಶ_putchar('\n');
	    ಸಂದೇಶ_advance(12);
	    ಸಂದೇಶ_outtrans_long_attr((char_u *)buf, 0);
	    ಸಂದೇಶ_putchar('\n');
	}
	ಯದಿ (extra != ರಿಕ್ತ)
	{
	    ಸಂದೇಶ_advance(13);
	    ಸಂದೇಶ_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	ಯದಿ (ಸಂದೇಶ_col)
	    ಸಂದೇಶ_putchar('\n');

	ui_breakcheck();
	ಯದಿ (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    ಭಙ್ಗ;
	}

	ಅಂಕೆ++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_read_prompt(ಪೂರ್ಣಾಙ್ಕ i)
{
    ಪೂರ್ಣಾಙ್ಕ		ch;
    ವರ್ಣ	*buf = ರಿಕ್ತ; // buffer for possible error message from cscope
    ಪೂರ್ಣಾಙ್ಕ		bufpos = 0;
    ಪೂರ್ಣಾಙ್ಕ		maxlen;
    ಸ್ಥಿರ ವರ್ಣ *eprompt = "Press the RETURN key to continue:";
    ಪೂರ್ಣಾಙ್ಕ		epromptlen = (ಪೂರ್ಣಾಙ್ಕ)strlen(eprompt);
    ಪೂರ್ಣಾಙ್ಕ		n;

    // compute maximum allowed len for Cscope error message
    ವರ್ಣ *cs_emsg = _(e_cscope_error_str);
    maxlen = (ಪೂರ್ಣಾಙ್ಕ)(IOSIZE - strlen(cs_emsg));

    ಪ್ರತಿ (;;)
    {
	ಯಾವತ್ ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    ಯದಿ (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		ಯದಿ (buf == ರಿಕ್ತ) // lazy buffer allocation
		    buf = alloc(maxlen);
		ಯದಿ (buf != ರಿಕ್ತ)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    ಯದಿ (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(ಶೂನ್ಯ)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(ಶೂನ್ಯ)putc('\n', csinfo[i].to_fp);
			(ಶೂನ್ಯ)fflush(csinfo[i].to_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	ಪ್ರತಿ (n = 0; n < (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    ಯದಿ (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    ಯದಿ (ch == EOF)
	    {
		ಯದಿ (buf != ರಿಕ್ತ && buf[0] != NUL)
		    (ಶೂನ್ಯ)semsg(cs_emsg, buf);
		ಅನ್ಯಥಾ ಯದಿ (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		ಪ್ರತ್ಯಾಗಮ CSCOPE_FAILURE;
	    }

	    ಯದಿ (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		ಭಙ್ಗ;
	    }
	}

	ಯದಿ (ch == EOF)
	    ಅನುವರ್ತ;	    // didn't find the prompt
	ಭಙ್ಗ;		    // did find the prompt
    }

    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ CSCOPE_SUCCESS;
}

# ಯದಿ defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cs_release_csp(ಪೂರ್ಣಾಙ್ಕ i, ಪೂರ್ಣಾಙ್ಕ freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    ಯದಿ (csinfo[i].to_fp != ರಿಕ್ತ)
    {
	(ಶೂನ್ಯ)fputs("q\n", csinfo[i].to_fp);
	(ಶೂನ್ಯ)fflush(csinfo[i].to_fp);
    }
# ಯದಿ defined(UNIX)
    {
	ಪೂರ್ಣಾಙ್ಕ waitpid_errno;
	ಪೂರ್ಣಾಙ್ಕ pstat;
	pid_t pid;

#  ಯದಿ defined(HAVE_SIGACTION)
	ಸಂರಚನಾ sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   ಅನ್ಯಥಾ
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, ರಿಕ್ತ);
#  ಅನ್ಯಥಾ
	ಪೂರ್ಣಾಙ್ಕ waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	ಪ್ರತಿ (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    ಯದಿ (pid != 0)
		ಭಙ್ಗ;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	ಯದಿ (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    ಪೂರ್ಣಾಙ್ಕ alive = TRUE;

	    ಯದಿ (waitpid_errno == ECHILD)
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
		ಪೂರ್ಣಾಙ್ಕ waited;

		sleep(0);
		ಪ್ರತಿ (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    ಯದಿ (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			ಭಙ್ಗ;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    ಯದಿ (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(ಶೂನ್ಯ)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# ಅನ್ಯಥಾ  // !UNIX
    ಯದಿ (csinfo[i].hProc != ರಿಕ್ತ)
    {
	// Give cscope a chance to exit normally
	ಯದಿ (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    ಯದಿ (csinfo[i].fr_fp != ರಿಕ್ತ)
	(ಶೂನ್ಯ)fclose(csinfo[i].fr_fp);
    ಯದಿ (csinfo[i].to_fp != ರಿಕ್ತ)
	(ಶೂನ್ಯ)fclose(csinfo[i].to_fp);

    ಯದಿ (freefnpp)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_reset(exarg_T *eap UNUSED)
{
    ವರ್ಣ	**dblist = ರಿಕ್ತ, **pplist = ರಿಕ್ತ, **fllist = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ	i;
    ವರ್ಣ buf[20]; // for sprintf " (#%d)"

    ಯದಿ (csinfo_size == 0)
	ಪ್ರತ್ಯಾಗಮ CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(ವರ್ಣ *, csinfo_size);
    pplist = ALLOC_MULT(ವರ್ಣ *, csinfo_size);
    fllist = ALLOC_MULT(ವರ್ಣ *, csinfo_size);
    ಯದಿ (dblist == ರಿಕ್ತ || pplist == ರಿಕ್ತ || fllist == ರಿಕ್ತ)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	ಪ್ರತ್ಯಾಗಮ CSCOPE_FAILURE;
    }

    ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	ಯದಿ (csinfo[i].fname != ರಿಕ್ತ)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
    {
	ಯದಿ (dblist[i] != ರಿಕ್ತ)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    ಯದಿ (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		ಸಂದೇಶ_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    ಯದಿ (p_csverbose)
	ಸಂದೇಶ_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | ಸಂದೇಶ_HIST);
    ಪ್ರತ್ಯಾಗಮ CSCOPE_SUCCESS;
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
    ಸ್ಥಿರ ವರ್ಣ *
cs_resolve_file(ಪೂರ್ಣಾಙ್ಕ i, ವರ್ಣ *name)
{
    ವರ್ಣ	*fullname;
    string_T	csdir = {ರಿಕ್ತ, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    ಪೂರ್ಣಾಙ್ಕ		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (ಪೂರ್ಣಾಙ್ಕ)namelen + 2;
    ಯದಿ (csinfo[i].ppath != ರಿಕ್ತ)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (ಪೂರ್ಣಾಙ್ಕ)ppathlen;
    }
    ಅನ್ಯಥಾ ಯದಿ (p_csre && csinfo[i].fname != ರಿಕ್ತ)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	ಯದಿ (csdir.string != ರಿಕ್ತ)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (ಪೂರ್ಣಾಙ್ಕ)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    ಯದಿ (csinfo[i].ppath != ರಿಕ್ತ
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	ಯದಿ ((fullname = alloc(len)) != ರಿಕ್ತ)
	    (ಶೂನ್ಯ)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    ಅನ್ಯಥಾ ಯದಿ (csdir.string != ರಿಕ್ತ && csinfo[i].fname != ರಿಕ್ತ && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (ವರ್ಣ *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    ಅನ್ಯಥಾ
    {
	fullname = (ವರ್ಣ *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    ಪ್ರತ್ಯಾಗಮ fullname;
}


/*
 * Show all cscope connections.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cs_show(exarg_T *eap UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (cs_cnt_connections() == 0)
	ಸಂದೇಶ_puts(_("no cscope connections\n"));
    ಅನ್ಯಥಾ
    {
	ಸಂದೇಶ_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
	{
	    ಯದಿ (csinfo[i].fname == ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    ಯದಿ (csinfo[i].ppath != ರಿಕ್ತ)
		(ಶೂನ್ಯ)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    ಅನ್ಯಥಾ
		(ಶೂನ್ಯ)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    ಪ್ರತ್ಯಾಗಮ CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    ಶೂನ್ಯ
cs_end(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#ಯದಿ defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    ಶೂನ್ಯ
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ = 0;
    char_u	*dbpath = ರಿಕ್ತ;
    char_u	*prepend = ರಿಕ್ತ;
    char_u	buf[NUMBUFLEN];

    ಯದಿ (in_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	ಅಂಕೆ = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = cs_connection(ಅಂಕೆ, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
