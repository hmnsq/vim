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

#ಆದರೆ defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# ಆದರೆ defined(UNIX)
#  include <sys/wait.h>
# endif

# ಆದರೆ defined (MSWIN)
#  ifndef WIN32_LEAN_AND_ಸರಾಸರಿ
#   define WIN32_LEAN_AND_ಸರಾಸರಿ
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ *  name;
    ಪೂರ್ಣಾಂಕ     (*func)(exarg_T *eap);
    ಅಕ್ಷರ *  help;
    ಅಕ್ಷರ *  usage;
    ಪೂರ್ಣಾಂಕ	    cansplit;		// if supports splitting window
} cscmd_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ csi {
    ಅಕ್ಷರ *	    fname;	// cscope db name
    ಅಕ್ಷರ *	    ppath;	// path to prepend (the -P option)
    ಅಕ್ಷರ *	    flags;	// additional cscope flags/options (e.g, -p2)
# ಆದರೆ defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_ಸೂಚ್ಯಂಕ_ಮೇಲಿನ;	// st_ino has no meaning in the Windows
    DWORD	    n_ಸೂಚ್ಯಂಕ_ಕೆಳಗಿನ;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    ಗಮ್ಯ_fp;	// to cscope: FILE.
} csinfo_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ { ಕೂಡಿಸು_2, ಕಂಡುಹಿಡಿ_2, Help, Kill, Reset, Show } csid_e;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_ಕೂಡಿಸು(exarg_T *eap);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_ಕೂಡಿಸು_common(ಅಕ್ಷರ *, ಅಕ್ಷರ *, ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_check_for_connections(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_check_for_ಟ್ಯಾಗುಗಳು(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_cnt_connections(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_create_connection(ಪೂರ್ಣಾಂಕ i);
# ifdef FEAT_QUICKFIX
ಸ್ಥಿರ ಶೂನ್ಯ	    cs_file_results(FILE *, ಪೂರ್ಣಾಂಕ *);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ	    cs_fill_results(ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ , ಪೂರ್ಣಾಂಕ *, ಅಕ್ಷರ ***,
			ಅಕ್ಷರ ***, ಪೂರ್ಣಾಂಕ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_ಕಂಡುಹಿಡಿ(exarg_T *eap);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_ಕಂಡುಹಿಡಿ_common(ಅಕ್ಷರ *opt, ಅಕ್ಷರ *pat, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, char_u *cmdline);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_help(exarg_T *eap);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_ಸೇರಿಸು_filelist(ಅಕ್ಷರ *, ಅಕ್ಷರ *, ಅಕ್ಷರ *,
			stat_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_kill(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	    cs_kill_execute(ಪೂರ್ಣಾಂಕ, ಅಕ್ಷರ *);
ಸ್ಥಿರ cscmd_T *    cs_lookup_cmd(exarg_T *eap);
ಸ್ಥಿರ ಅಕ್ಷರ *	    cs_make_vim_style_matches(ಅಕ್ಷರ *, ಅಕ್ಷರ *,
			ಅಕ್ಷರ *, ಅಕ್ಷರ *);
ಸ್ಥಿರ ಅಕ್ಷರ *	    cs_manage_matches(ಅಕ್ಷರ **, ಅಕ್ಷರ **, ಪೂರ್ಣಾಂಕ, mcmd_e);
ಸ್ಥಿರ ಶೂನ್ಯ	    cs_print_ಟ್ಯಾಗುಗಳು_priv(ಅಕ್ಷರ **, ಅಕ್ಷರ **, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_read_prompt(ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ	    cs_release_csp(ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ freefnpp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_reset(exarg_T *eap);
ಸ್ಥಿರ ಅಕ್ಷರ *	    cs_resolve_file(ಪೂರ್ಣಾಂಕ, ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    cs_show(exarg_T *eap);


ಸ್ಥಿರ csinfo_T *   csinfo = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    csinfo_ಗಾತ್ರ = 0;	// number of items allocated in
					// csinfo[]

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
ಸ್ಥಿರ cscmd_T	    cs_cmds[] =
{
    { "add",	cs_ಕೂಡಿಸು,
		N_("Add a new database"),     "add file|dir [pre-path] [flags]", 0 },
    { "find",	cs_ಕಂಡುಹಿಡಿ,
		N_("Query for a pattern"),    "find a|c|d|e|f|g|i|s|t name", 1 },
    { "help",	cs_help,
		N_("Show this message"),      "help", 0 },
    { "kill",	cs_kill,
		N_("Kill a connection"),      "kill #", 0 },
    { "reset",	cs_reset,
		N_("Reinit all connections"), "reset", 0 },
    { "show",	cs_show,
		N_("Show connections"),       "show", 0 },
    { ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, 0 }
};

    ಸ್ಥಿರ ಶೂನ್ಯ
cs_usage_ಸಂದೇಶ(csid_e x)
{
    (ಶೂನ್ಯ)semsg(_(e_usage_cscope_str), cs_cmds[(ಪೂರ್ಣಾಂಕ)x].usage);
}

ಸ್ಥಿರ ಎಣಿಕೆ
{
    EXP_CSCOPE_SUBCMD,	// expand ":cscope" sub-commands
    EXP_SCSCOPE_SUBCMD,	// expand ":scscope" sub-commands
    EXP_CSCOPE_ಕಂಡುಹಿಡಿ,	// expand ":cscope find" arguments
    EXP_CSCOPE_KILL	// expand ":cscope kill" arguments
} expand_what;

/*
 * Function given to ExpandGeneric() to obtain the cscope command
 * expansion.
 */
    char_u *
get_cscope_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ		current_idx;
    ಪೂರ್ಣಾಂಕ		i;

    ಆಯ್ಕೆ (expand_what)
    {
    ಸ್ಥಿತಿ EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	ಹಿಂತಿರುಗಿ (char_u *)cs_cmds[idx].name;
    ಸ್ಥಿತಿ EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	ಗಾಗಿ (i = 0, current_idx = 0; cs_cmds[i].name != ಖಾಲಿ; i++)
	    ಆದರೆ (cs_cmds[i].cansplit)
		ಆದರೆ (current_idx++ == idx)
		    ನಿಲ್ಲಿಸು;
	ಹಿಂತಿರುಗಿ (char_u *)cs_cmds[i].name;
    ಸ್ಥಿತಿ EXP_CSCOPE_ಕಂಡುಹಿಡಿ:
	{
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", ಖಾಲಿ
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    ಹಿಂತಿರುಗಿ (char_u *)query_type[idx];
	}
    ಸ್ಥಿತಿ EXP_CSCOPE_KILL:
	{
	    ಸ್ಥಿರ ಅಕ್ಷರ	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    ಗಾಗಿ (i = 0, current_idx = 0; i < csinfo_ಗಾತ್ರ; i++)
	    {
		ಆದರೆ (csinfo[i].fname == ಖಾಲಿ)
		    ಮುಂದುವರಿ;
		ಆದರೆ (current_idx++ == idx)
		{
		    vim_snprintf(connection, ಗಾತ್ರ(connection), "%d", i);
		    ಹಿಂತಿರುಗಿ (char_u *)connection;
		}
	    }
	    ಹಿಂತಿರುಗಿ (current_idx == idx && idx > 0) ? (char_u *)"-1" : ಖಾಲಿ;
	}
    ಪೂರ್ವನಿಯೋಜಿತ:
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    ಶೂನ್ಯ
ಗಣ_context_ಒಳ_cscope_cmd(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_CSCOPE;
    xp->xp_ನಮೂನೆ = arg;
    expand_what = (cmdidx == CMD_scscope)
			? EXP_SCSCOPE_SUBCMD : EXP_CSCOPE_SUBCMD;

    ಆದರೆ (*arg == NUL)
	ಹಿಂತಿರುಗಿ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ;

    // past first word
    xp->xp_ನಮೂನೆ = skipwhite(p);
    ಆದರೆ (*skiptowhite(xp->xp_ನಮೂನೆ) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_ಕಂಡುಹಿಡಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_cscope_general(
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		make_ವಿಭಜಿಸು ಬಳಸದ) // whether to split window
{
    cscmd_T *cmdp;

    ಆದರೆ (check_restricted())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((cmdp = cs_lookup_cmd(eap)) == ಖಾಲಿ)
    {
	cs_help(eap);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (make_ವಿಭಜಿಸು)
    {
	ಆದರೆ (!cmdp->cansplit)
	{
	    (ಶೂನ್ಯ)ಸಂದೇಶ_puts(_("This cscope command does not support splitting the window.\n"));
	    ಹಿಂತಿರುಗಿ;
	}
	postponed_ವಿಭಜಿಸು = -1;
	postponed_ವಿಭಜಿಸು_flags = cmdmod.cmod_ವಿಭಜಿಸು;
	postponed_ವಿಭಜಿಸು_tab = cmdmod.cmod_tab;
    }

    cmdp->func(eap);

    postponed_ವಿಭಜಿಸು_flags = 0;
    postponed_ವಿಭಜಿಸು_tab = 0;
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
    ಪೂರ್ಣಾಂಕ ret = FALSE;

    ಆದರೆ (*eap->arg == NUL)
    {
	(ಶೂನ್ಯ)emsg(_(e_usage_cstag_ident));
	ಹಿಂತಿರುಗಿ;
    }

    ಆಯ್ಕೆ (p_csto)
    {
    ಸ್ಥಿತಿ 0 :
	ಆದರೆ (cs_check_for_connections())
	{
	    ret = cs_ಕಂಡುಹಿಡಿ_common("g", (ಅಕ್ಷರ *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ಆದರೆ (ret == FALSE)
	    {
		cs_free_ಟ್ಯಾಗುಗಳು();
		ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು)
		    ಸಂದೇಶ_putchar('\n');

		ಆದರೆ (cs_check_for_ಟ್ಯಾಗುಗಳು())
		    ret = do_ಟ್ಯಾಗ್(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cs_check_for_ಟ್ಯಾಗುಗಳು())
	{
	    ret = do_ಟ್ಯಾಗ್(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1 :
	ಆದರೆ (cs_check_for_ಟ್ಯಾಗುಗಳು())
	{
	    ret = do_ಟ್ಯಾಗ್(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    ಆದರೆ (ret == FALSE)
	    {
		ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು)
		    ಸಂದೇಶ_putchar('\n');

		ಆದರೆ (cs_check_for_connections())
		{
		    ret = cs_ಕಂಡುಹಿಡಿ_common("g", (ಅಕ್ಷರ *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    ಆದರೆ (ret == FALSE)
			cs_free_ಟ್ಯಾಗುಗಳು();
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cs_check_for_connections())
	{
	    ret = cs_ಕಂಡುಹಿಡಿ_common("g", (ಅಕ್ಷರ *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ಆದರೆ (ret == FALSE)
		cs_free_ಟ್ಯಾಗುಗಳು();
	}
	ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ :
	ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (!ret)
    {
	(ಶೂನ್ಯ)emsg(_(e_cstag_ಟ್ಯಾಗ್_not_founc));
# ಆದರೆ defined(FEAT_QUICKFIX)
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
    ಪೂರ್ಣಾಂಕ
cs_fgets(char_u *buf, ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2)
{
    ಅಕ್ಷರ *p;

    ಆದರೆ ((p = cs_manage_matches(ಖಾಲಿ, ಖಾಲಿ, -1, Get)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;
    vim_strncpy(buf, (char_u *)p, ಗಾತ್ರ_2 - 1);

    ಹಿಂತಿರುಗಿ FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    ಶೂನ್ಯ
cs_free_ಟ್ಯಾಗುಗಳು(ಶೂನ್ಯ)
{
    cs_manage_matches(ಖಾಲಿ, ಖಾಲಿ, -1, Free);
}


/*
 * Called from do_tag().
 */
    ಶೂನ್ಯ
cs_print_ಟ್ಯಾಗುಗಳು(ಶೂನ್ಯ)
{
    cs_manage_matches(ಖಾಲಿ, ಖಾಲಿ, -1, Print);
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
# ಆದರೆ defined(FEAT_EVAL)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_connection(ಪೂರ್ಣಾಂಕ ಅಂಕೆ, char_u *dbpath, char_u *ppath)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (ಅಂಕೆ < 0 || ಅಂಕೆ > 4 || (ಅಂಕೆ > 0 && !dbpath))
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
    {
	ಆದರೆ (!csinfo[i].fname)
	    ಮುಂದುವರಿ;

	ಆದರೆ (ಅಂಕೆ == 0)
	    ಹಿಂತಿರುಗಿ TRUE;

	ಆಯ್ಕೆ (ಅಂಕೆ)
	{
	ಸ್ಥಿತಿ 1:
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(csinfo[i].fname, (ಅಕ್ಷರ *)dbpath))
		ಹಿಂತಿರುಗಿ TRUE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2:
	    ಆದರೆ (strcmp(csinfo[i].fname, (ಅಕ್ಷರ *)dbpath) == 0)
		ಹಿಂತಿರುಗಿ TRUE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 3:
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(csinfo[i].fname, (ಅಕ್ಷರ *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(csinfo[i].ppath, (ಅಕ್ಷರ *)ppath))))
		ಹಿಂತಿರುಗಿ TRUE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 4:
	    ಆದರೆ ((strcmp(csinfo[i].fname, (ಅಕ್ಷರ *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (ಅಕ್ಷರ *)ppath) == 0))))
		ಹಿಂತಿರುಗಿ TRUE;
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_ಕೂಡಿಸು(exarg_T *eap ಬಳಸದ)
{
    ಅಕ್ಷರ *fname, *ppath, *flags = ಖಾಲಿ;

    ಆದರೆ ((fname = strtok((ಅಕ್ಷರ *)ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ")) == ಖಾಲಿ)
    {
	cs_usage_ಸಂದೇಶ(ಕೂಡಿಸು_2);
	ಹಿಂತಿರುಗಿ CSCOPE_FAILURE;
    }
    ಆದರೆ ((ppath = strtok((ಅಕ್ಷರ *)ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ")) != ಖಾಲಿ)
	flags = strtok((ಅಕ್ಷರ *)ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ");

    ಹಿಂತಿರುಗಿ cs_ಕೂಡಿಸು_common(fname, ppath, flags);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cs_stat_emsg(ಅಕ್ಷರ *fname)
{
    ಪೂರ್ಣಾಂಕ err = errno;
    (ಶೂನ್ಯ)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_ಕೂಡಿಸು_common(
    ಅಕ್ಷರ *arg1,	    // filename - may contain environment variables
    ಅಕ್ಷರ *arg2,	    // prepend path - may contain environment variables
    ಅಕ್ಷರ *flags)
{
    stat_T	statbuf;
    ಪೂರ್ಣಾಂಕ		ret;
    ಅಕ್ಷರ	*fname = ಖಾಲಿ;
    ಅಕ್ಷರ	*fname2 = ಖಾಲಿ;
    ಅಕ್ಷರ	*ppath = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = ಖಾಲಿ;

    // get the filename (arg1), expand it, and try to stat it
    ಆದರೆ ((fname = alloc(MAXPATHL + 1)) == ಖಾಲಿ)
	ಹೋಗು ಕೂಡಿಸು_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (ಶೂನ್ಯ)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹೋಗು ಕೂಡಿಸು_err;
    fname = (ಅಕ್ಷರ *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    ಆದರೆ (ret < 0)
    {
staterr:
	ಆದರೆ (p_csverbose)
	    cs_stat_emsg(fname);
	ಹೋಗು ಕೂಡಿಸು_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    ಆದರೆ (arg2 != ಖಾಲಿ)
    {
	stat_T	    statbuf2;

	ಆದರೆ ((ppath = alloc(MAXPATHL + 1)) == ಖಾಲಿ)
	    ಹೋಗು ಕೂಡಿಸು_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	ಆದರೆ (ret < 0)
	    ಹೋಗು staterr;
    }

    // if filename is a directory, append the cscope database name to it
    ಆದರೆ (S_ISDIR(statbuf.st_ಬಹುಲಕ))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(fname) + 2);
	ಆದರೆ (fname2 == ಖಾಲಿ)
	    ಹೋಗು ಕೂಡಿಸು_err;

	ಆಗ (fname[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(fname)-1] == '/'
# ifdef MSWIN
		|| fname[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(fname)-1] == '\\'
# endif
		)
	{
	    fname[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(fname)-1] = '\0';
	    ಆದರೆ (fname[0] == '\0')
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (fname[0] == '\0')
	    (ಶೂನ್ಯ)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	ಇಲ್ಲದಿದ್ದರೆ
	    (ಶೂನ್ಯ)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	ಆದರೆ (ret < 0)
	{
	    ಆದರೆ (p_csverbose)
		cs_stat_emsg(fname2);
	    ಹೋಗು ಕೂಡಿಸು_err;
	}

	i = cs_ಸೇರಿಸು_filelist(fname2, ppath, flags, &statbuf);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (S_ISREG(statbuf.st_ಬಹುಲಕ) || S_ISLNK(statbuf.st_ಬಹುಲಕ))
    {
	i = cs_ಸೇರಿಸು_filelist(fname, ppath, flags, &statbuf);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (p_csverbose)
	    (ಶೂನ್ಯ)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	ಹೋಗು ಕೂಡಿಸು_err;
    }

    ಆದರೆ (i != -1)
    {
	ಆದರೆ (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    ಹೋಗು ಕೂಡಿಸು_err;
	}

	ಆದರೆ (p_csverbose)
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
    ಹಿಂತಿರುಗಿ CSCOPE_SUCCESS;

ಕೂಡಿಸು_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    ಹಿಂತಿರುಗಿ CSCOPE_FAILURE;
}


    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_check_for_connections(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (cs_cnt_connections() > 0);
}


    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_check_for_ಟ್ಯಾಗುಗಳು(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (p_ಟ್ಯಾಗುಗಳು[0] != NUL && curbuf->b_p_ಟ್ಯಾಗುಗಳು != ಖಾಲಿ);
}


/*
 * Count the number of cscope connections.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_cnt_connections(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ cnt = 0;

    ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
	ಆದರೆ (csinfo[i].fname != ಖಾಲಿ)
	    cnt++;
    ಹಿಂತಿರುಗಿ cnt;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cs_reading_emsg(
    ಪೂರ್ಣಾಂಕ idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_cnt_matches(ಪೂರ್ಣಾಂಕ idx)
{
    ಅಕ್ಷರ *stok;
    ಅಕ್ಷರ *buf;
    ಪೂರ್ಣಾಂಕ nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    ಆದರೆ (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    ಹಿಂತಿರುಗಿ -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf, "Unable to search database") != ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ ((stok = strtok(buf, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ")) == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)stok, "cscope:") == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	ಆದರೆ ((stok = strtok(ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ")) == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	nlines = atoi(stok);
	ಆದರೆ (nlines < 0)
	{
	    nlines = 0;
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ ((stok = strtok(ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ")) == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)stok, "lines", 5))
	    ಮುಂದುವರಿ;

	ನಿಲ್ಲಿಸು;
    }

    vim_free(buf);
    ಹಿಂತಿರುಗಿ nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
cs_create_cmd(ಅಕ್ಷರ *csoption, ಅಕ್ಷರ *ನಮೂನೆ)
{
    ಅಕ್ಷರ *cmd;
    ಗಿಡ್ಡ ಹುಡುಕು;
    ಅಕ್ಷರ *pat;

    ಆಯ್ಕೆ (csoption[0])
    {
    ಸ್ಥಿತಿ '0' : ಸ್ಥಿತಿ 's' :
	ಹುಡುಕು = 0;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '1' : ಸ್ಥಿತಿ 'g' :
	ಹುಡುಕು = 1;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '2' : ಸ್ಥಿತಿ 'd' :
	ಹುಡುಕು = 2;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '3' : ಸ್ಥಿತಿ 'c' :
	ಹುಡುಕು = 3;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '4' : ಸ್ಥಿತಿ 't' :
	ಹುಡುಕು = 4;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '6' : ಸ್ಥಿತಿ 'e' :
	ಹುಡುಕು = 6;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '7' : ಸ್ಥಿತಿ 'f' :
	ಹುಡುಕು = 7;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '8' : ಸ್ಥಿತಿ 'i' :
	ಹುಡುಕು = 8;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '9' : ಸ್ಥಿತಿ 'a' :
	ಹುಡುಕು = 9;
	ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ :
	(ಶೂನ್ಯ)emsg(_(e_unknown_cscope_ಹುಡುಕು_type));
	cs_usage_ಸಂದೇಶ(ಕಂಡುಹಿಡಿ_2);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = ನಮೂನೆ;
    ಆದರೆ (ಹುಡುಕು != 4 && ಹುಡುಕು != 6)
	ಆಗ VIM_ISWHITE(*pat)
	    ++pat;

    ಆದರೆ ((cmd = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(pat) + 2)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    (ಶೂನ್ಯ)sprintf(cmd, "%d%s", ಹುಡುಕು, pat);

    ಹಿಂತಿರುಗಿ cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_create_connection(ಪೂರ್ಣಾಂಕ i)
{
# ifdef UNIX
    ಪೂರ್ಣಾಂಕ		ಗಮ್ಯ_cs[2], from_cs[2];
# endif
    ಪೂರ್ಣಾಂಕ		cmdlen;
    ಅಕ್ಷರ	*prog, *cmd, *ppath = ಖಾಲಿ;
    size_t	proglen;
# ifdef MSWIN
    ಪೂರ್ಣಾಂಕ		fd;
    ಪೂರ್ಣಾಂಕ		len;
    SECURITY_ಗುಣಲಕ್ಷಣಗಳು sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# ಆದರೆ defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    ಗಮ್ಯ_cs[0] = ಗಮ್ಯ_cs[1] = from_cs[0] = from_cs[1] = -1;
    ಆದರೆ (pipe(ಗಮ್ಯ_cs) < 0 || pipe(from_cs) < 0)
    {
	(ಶೂನ್ಯ)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	ಆದರೆ (ಗಮ್ಯ_cs[0] != -1)
	    (ಶೂನ್ಯ)close(ಗಮ್ಯ_cs[0]);
	ಆದರೆ (ಗಮ್ಯ_cs[1] != -1)
	    (ಶೂನ್ಯ)close(ಗಮ್ಯ_cs[1]);
	ಆದರೆ (from_cs[0] != -1)
	    (ಶೂನ್ಯ)close(from_cs[0]);
	ಆದರೆ (from_cs[1] != -1)
	    (ಶೂನ್ಯ)close(from_cs[1]);
	ಹಿಂತಿರುಗಿ CSCOPE_FAILURE;
    }

    ಆದರೆ ((csinfo[i].pid = fork()) == -1)
    {
	(ಶೂನ್ಯ)emsg(_(e_could_not_fork_for_cscope));
	ಹೋಗು err_closing;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (csinfo[i].pid == 0)	// child: run cscope.
    {
	ಅಕ್ಷರ **argv = ಖಾಲಿ;

	ಆದರೆ (dup2(ಗಮ್ಯ_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	ಆದರೆ (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	ಆದರೆ (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(ಶೂನ್ಯ)close(ಗಮ್ಯ_cs[1]);
	(ಶೂನ್ಯ)close(from_cs[0]);
# ಇಲ್ಲದಿದ್ದರೆ
	// MSWIN
	// Create pipes to communicate with cscope
	sa.n_ಉದ್ದಳತೆ = ಗಾತ್ರ(SECURITY_ಗುಣಲಕ್ಷಣಗಳು);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ಖಾಲಿ;

	ಆದರೆ (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (ಶೂನ್ಯ)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    ಆದರೆ (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    ಆದರೆ (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    ಹಿಂತಿರುಗಿ CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	ಆದರೆ ((prog = alloc(MAXPATHL + 1)) == ಖಾಲಿ)
	{
# ifdef UNIX
	    ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
# ಇಲ್ಲದಿದ್ದರೆ
	    // MSWIN
	    ಹೋಗು err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (ಪೂರ್ಣಾಂಕ)(proglen + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(csinfo[i].fname) + 32);
	ಆದರೆ (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    ಆದರೆ ((ppath = alloc(MAXPATHL + 1)) == ಖಾಲಿ)
	    {
		vim_free(prog);
# ifdef UNIX
		ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
# ಇಲ್ಲದಿದ್ದರೆ
		// MSWIN
		ಹೋಗು err_closing;
# endif
	    }
	    cmdlen += (ಪೂರ್ಣಾಂಕ)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	ಆದರೆ (csinfo[i].flags)
	    cmdlen += (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(csinfo[i].flags);

	ಆದರೆ ((cmd = alloc(cmdlen)) == ಖಾಲಿ)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
# ಇಲ್ಲದಿದ್ದರೆ
	    // MSWIN
	    ಹೋಗು err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    ಅಕ್ಷರ	**tok = ಖಾಲಿ;
	    ಪೂರ್ಣಾಂಕ		tokc = 0;

	    ga_init2(&ga_argv, ಗಾತ್ರ(ಅಕ್ಷರ *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    ಆದರೆ (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < tokc; ++k)
	    {
		ಆದರೆ (ga_grow(&ga_argv, 1) == FAIL)
		    ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
		((ಅಕ್ಷರ **)ga_argv.ga_ದತ್ತಾಂಶ)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    ಆದರೆ (ga_grow(&ga_argv, 3) == FAIL)
		ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
	    ((ಅಕ್ಷರ **)ga_argv.ga_ದತ್ತಾಂಶ)[ga_argv.ga_len++] =
				    (ಅಕ್ಷರ *)vim_strsave((char_u *)"-dl");
	    ((ಅಕ್ಷರ **)ga_argv.ga_ದತ್ತಾಂಶ)[ga_argv.ga_len++] =
				    (ಅಕ್ಷರ *)vim_strsave((char_u *)"-f");
	    ((ಅಕ್ಷರ **)ga_argv.ga_ದತ್ತಾಂಶ)[ga_argv.ga_len++] =
				(ಅಕ್ಷರ *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    ಆದರೆ (ppath != ಖಾಲಿ)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		ಅಕ್ಷರ	*parg = alloc(plen);

		ಆದರೆ (parg == ಖಾಲಿ || ga_grow(&ga_argv, 1) == FAIL)
		    ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((ಅಕ್ಷರ **)ga_argv.ga_ದತ್ತಾಂಶ)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    ಆದರೆ (csinfo[i].flags != ಖಾಲಿ)
	    {
		ಆದರೆ (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < tokc; ++k)
		{
		    ಆದರೆ (ga_grow(&ga_argv, 1) == FAIL)
			ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
		    ((ಅಕ್ಷರ **)ga_argv.ga_ದತ್ತಾಂಶ)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    ಆದರೆ (ga_grow(&ga_argv, 1) == FAIL)
		ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
	    ((ಅಕ್ಷರ **)ga_argv.ga_ದತ್ತಾಂಶ)[ga_argv.ga_len] = ಖಾಲಿ;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  ಆದರೆ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   ಆದರೆ defined(HAVE_SETSID)
	    (ಶೂನ್ಯ)setsid();
#   ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (ಅಕ್ಷರ **)ga_argv.ga_ದತ್ತಾಂಶ;
	    ಆದರೆ (argv[0] == ಖಾಲಿ || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    ನಿರ್ಗಮಿಸು(127);
	    // NOTREACHED
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	ಆದರೆ ((csinfo[i].ಗಮ್ಯ_fp = fdopen(ಗಮ್ಯ_cs[1], "w")) == ಖಾಲಿ)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	ಆದರೆ ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == ಖಾಲಿ)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(ಶೂನ್ಯ)close(ಗಮ್ಯ_cs[0]);
	(ಶೂನ್ಯ)close(from_cs[1]);
    }
# ಇಲ್ಲದಿದ್ದರೆ
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    ಆದರೆ (csinfo[i].ppath != ಖಾಲಿ)
    {
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    ಆದರೆ (csinfo[i].flags != ಖಾಲಿ)
    {
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.w_Show_ಕಿಟಕಿ = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_ಒಳಹರಿವು  = stdin_rd;
    created = CreateProcess(ಖಾಲಿ, cmd, ಖಾಲಿ, ಖಾಲಿ, TRUE, CREATE_NEW_CONSOLE,
							ಖಾಲಿ, ಖಾಲಿ, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    ಆದರೆ (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(ಶೂನ್ಯ)emsg(_(e_could_not_spawn_cscope_process));
	ಹೋಗು err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    ಆದರೆ (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      O_ಪಠ್ಯ|_O_APPEND)) < 0)
	    || ((csinfo[i].ಗಮ್ಯ_fp = _fdopen(fd, "w")) == ಖಾಲಿ))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    ಆದರೆ (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      O_ಪಠ್ಯ|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == ಖಾಲಿ))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    ಹಿಂತಿರುಗಿ CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_ಕಂಡುಹಿಡಿ(exarg_T *eap)
{
    ಅಕ್ಷರ *opt, *pat;
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (cs_check_for_connections() == FALSE)
    {
	(ಶೂನ್ಯ)emsg(_(e_no_cscope_connections));
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಆದರೆ ((opt = strtok((ಅಕ್ಷರ *)ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ")) == ಖಾಲಿ)
    {
	cs_usage_ಸಂದೇಶ(ಕಂಡುಹಿಡಿ_2);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    pat = opt + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(opt) + 1;
    ಆದರೆ (pat >= (ಅಕ್ಷರ *)eap->arg + eap_arg_len)
    {
	cs_usage_ಸಂದೇಶ(ಕಂಡುಹಿಡಿ_2);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    ಗಾಗಿ (i = 0; i < eap_arg_len; ++i)
	ಆದರೆ (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    ಹಿಂತಿರುಗಿ cs_ಕಂಡುಹಿಡಿ_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_ಕಂಡುಹಿಡಿ_common(
    ಅಕ್ಷರ *opt,
    ಅಕ್ಷರ *pat,
    ಪೂರ್ಣಾಂಕ forceit,
    ಪೂರ್ಣಾಂಕ verbose,
    ಪೂರ್ಣಾಂಕ	use_ll ಬಳಸದ,
    char_u *cmdline ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ i;
    ಅಕ್ಷರ *cmd;
    ಪೂರ್ಣಾಂಕ *nummatches;
    ಪೂರ್ಣಾಂಕ totmatches;
# ifdef FEAT_QUICKFIX
    ಅಕ್ಷರ cmdletter;
    ಅಕ್ಷರ *qfpos;

    // get cmd letter
    ಆಯ್ಕೆ (opt[0])
    {
    ಸ್ಥಿತಿ '0' :
	cmdletter = 's';
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '1' :
	cmdletter = 'g';
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '2' :
	cmdletter = 'd';
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '3' :
	cmdletter = 'c';
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '4' :
	cmdletter = 't';
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '6' :
	cmdletter = 'e';
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '7' :
	cmdletter = 'f';
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '8' :
	cmdletter = 'i';
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '9' :
	cmdletter = 'a';
	ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ :
	cmdletter = opt[0];
    }

    qfpos = (ಅಕ್ಷರ *)vim_strchr(p_csqf, cmdletter);
    ಆದರೆ (qfpos != ಖಾಲಿ)
    {
	qfpos++;
	// next symbol must be + or -
	ಆದರೆ (strchr(CSQF_FLAGS, *qfpos) == ಖಾಲಿ)
	{
	    (ಶೂನ್ಯ)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	ಆದರೆ (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    ಆದರೆ (aborting())
		ಹಿಂತಿರುಗಿ FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    ಆದರೆ (cmd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    nummatches = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, csinfo_ಗಾತ್ರ);
    ಆದರೆ (nummatches == ಖಾಲಿ)
    {
	vim_free(cmd);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
	nummatches[i] = 0;
    totmatches = 0;
    ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
    {
	ಆದರೆ (csinfo[i].fname == ಖಾಲಿ || csinfo[i].ಗಮ್ಯ_fp == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	// send cmd to cscope
	(ಶೂನ್ಯ)fprintf(csinfo[i].ಗಮ್ಯ_fp, "%s\n", cmd);
	(ಶೂನ್ಯ)fflush(csinfo[i].ಗಮ್ಯ_fp);

	nummatches[i] = cs_cnt_matches(i);

	ಆದರೆ (nummatches[i] > -1)
	    totmatches += nummatches[i];

	ಆದರೆ (nummatches[i] == 0)
	    (ಶೂನ್ಯ)cs_read_prompt(i);
    }
    vim_free(cmd);

    ಆದರೆ (totmatches == 0)
    {
	ಆದರೆ (verbose)
	    (ಶೂನ್ಯ)semsg(_(e_no_matches_ಸಿಕ್ಕಿತು_for_cscope_query_str_ದ_str),
								     opt, pat);
	vim_free(nummatches);
	ಹಿಂತಿರುಗಿ FALSE;
    }

# ifdef FEAT_QUICKFIX
    ಆದರೆ (qfpos != ಖಾಲಿ && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = ಖಾಲಿ;
	win_T	    *wp = ಖಾಲಿ;

	ಆದರೆ (tmp == ಖಾಲಿ)
	{
	    vim_free(nummatches);
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	f = mch_fopen((ಅಕ್ಷರ *)tmp, "w");
	ಆದರೆ (f == ಖಾಲಿ)
	    semsg(_(e_cant_open_file_str), tmp);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    ಆದರೆ (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    ಆದರೆ (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, ಖಾಲಿ) > 0)
	    {
		ಆದರೆ (postponed_ವಿಭಜಿಸು != 0)
		{
		    (ಶೂನ್ಯ)win_ವಿಭಜಿಸು(postponed_ವಿಭಜಿಸು > 0 ? postponed_ವಿಭಜಿಸು : 0,
						       postponed_ವಿಭಜಿಸು_flags);
		    RESET_BINDING(curwin);
		    postponed_ವಿಭಜಿಸು = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		ಆದರೆ (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_ಬಫರ್) && wp->w_llist_ref != ಖಾಲಿ)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif // FEAT_QUICKFIX
    {
	ಅಕ್ಷರ **matches = ಖಾಲಿ, **contexts = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	ಆದರೆ (matches == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;

	(ಶೂನ್ಯ)cs_manage_matches(matches, contexts, matched, Store);

	ಹಿಂತಿರುಗಿ do_ಟ್ಯಾಗ್((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_help(exarg_T *eap ಬಳಸದ)
{
    cscmd_T *cmdp = cs_cmds;

    (ಶೂನ್ಯ)ಸಂದೇಶ_puts(_("cscope commands:\n"));
    ಆಗ (cmdp->name != ಖಾಲಿ)
    {
	ಅಕ್ಷರ *help = _(cmdp->help);
	ಪೂರ್ಣಾಂಕ  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	ಆದರೆ (space_cnt < 0)
	    space_cnt = 0;
	(ಶೂನ್ಯ)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	ಆದರೆ (strcmp(cmdp->name, "find") == 0)
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
    ಹಿಂತಿರುಗಿ 0;
}


    ಸ್ಥಿರ ಶೂನ್ಯ
clear_csinfo(ಪೂರ್ಣಾಂಕ i)
{
    csinfo[i].fname  = ಖಾಲಿ;
    csinfo[i].ppath  = ಖಾಲಿ;
    csinfo[i].flags  = ಖಾಲಿ;
# ಆದರೆ defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# ಇಲ್ಲದಿದ್ದರೆ
    csinfo[i].nVolume = 0;
    csinfo[i].n_ಸೂಚ್ಯಂಕ_ಮೇಲಿನ = 0;
    csinfo[i].n_ಸೂಚ್ಯಂಕ_ಕೆಳಗಿನ = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = ಖಾಲಿ;
    csinfo[i].ಗಮ್ಯ_fp  = ಖಾಲಿ;
# ಆದರೆ defined(MSWIN)
    csinfo[i].hProc = ಖಾಲಿ;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_ಸೇರಿಸು_filelist(
    ಅಕ್ಷರ *fname,
    ಅಕ್ಷರ *ppath,
    ಅಕ್ಷರ *flags,
    stat_T *sb ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    ಆಯ್ಕೆ (win32_fileinfo((char_u *)fname, &bhfi))
    {
	ಸ್ಥಿತಿ FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	ಸ್ಥಿತಿ FILEINFO_READ_FAIL:	// CreateFile() failed
	    ಆದರೆ (p_csverbose)
	    {
		ಅಕ್ಷರ *cant_ಸಂದೇಶ = _(e_cannot_open_cscope_database_str);
		ಅಕ್ಷರ *winmsg = GetWin32Error();

		ಆದರೆ (winmsg != ಖಾಲಿ)
		    (ಶೂನ್ಯ)semsg(cant_ಸಂದೇಶ, winmsg);
		ಇಲ್ಲದಿದ್ದರೆ
		    // subst filename if can't get error text
		    (ಶೂನ್ಯ)semsg(cant_ಸಂದೇಶ, fname);
	    }
	    ಹಿಂತಿರುಗಿ -1;

	ಸ್ಥಿತಿ FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    ಆದರೆ (p_csverbose)
		(ಶೂನ್ಯ)emsg(_(e_cannot_get_cscope_database_information));
	    ಹಿಂತಿರುಗಿ -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    ಗಾಗಿ (j = 0; j < csinfo_ಗಾತ್ರ; j++)
    {
	ಆದರೆ (csinfo[j].fname != ಖಾಲಿ
# ಆದರೆ defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# ಇಲ್ಲದಿದ್ದರೆ
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_ಸಂಖ್ಯೆ
		    && csinfo[j].n_ಸೂಚ್ಯಂಕ_ಮೇಲಿನ == bhfi.n_File_ಸೂಚ್ಯಂಕ_ಮೇಲಿನ
		    && csinfo[j].n_ಸೂಚ್ಯಂಕ_ಕೆಳಗಿನ == bhfi.n_File_ಸೂಚ್ಯಂಕ_ಕೆಳಗಿನ))
# endif
	    )
	{
	    ಆದರೆ (p_csverbose)
		(ಶೂನ್ಯ)emsg(_(e_duplicate_cscope_database_not_added));
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (csinfo[j].fname == ಖಾಲಿ && i == -1)
	    i = j; // remember first empty entry
    }

    ಆದರೆ (i == -1)
    {
	i = csinfo_ಗಾತ್ರ;
	ಆದರೆ (csinfo_ಗಾತ್ರ == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_ಗಾತ್ರ = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_ಗಾತ್ರ *= 2;
	    csinfo = vim_realloc(csinfo, ಗಾತ್ರ(csinfo_T)*csinfo_ಗಾತ್ರ);
	    ಆದರೆ (csinfo == ಖಾಲಿ)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_ಗಾತ್ರ = i;
		ಹಿಂತಿರುಗಿ -1;
	    }
	}
	ಆದರೆ (csinfo == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
	ಗಾಗಿ (j = csinfo_ಗಾತ್ರ/2; j < csinfo_ಗಾತ್ರ; j++)
	    clear_csinfo(j);
    }

    ಆದರೆ ((csinfo[i].fname = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(fname)+1)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)fname));

    ಆದರೆ (ppath != ಖಾಲಿ)
    {
	ಆದರೆ ((csinfo[i].ppath = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ppath) + 1)) == ಖಾಲಿ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    ಹಿಂತಿರುಗಿ -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ppath));
    }
    ಇಲ್ಲದಿದ್ದರೆ
	csinfo[i].ppath = ಖಾಲಿ;

    ಆದರೆ (flags != ಖಾಲಿ)
    {
	ಆದರೆ ((csinfo[i].flags = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(flags) + 1)) == ಖಾಲಿ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    ಹಿಂತಿರುಗಿ -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)flags));
    }
    ಇಲ್ಲದಿದ್ದರೆ
	csinfo[i].flags = ಖಾಲಿ;

# ಆದರೆ defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# ಇಲ್ಲದಿದ್ದರೆ
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_ಸಂಖ್ಯೆ;
    csinfo[i].n_ಸೂಚ್ಯಂಕ_ಕೆಳಗಿನ = bhfi.n_File_ಸೂಚ್ಯಂಕ_ಕೆಳಗಿನ;
    csinfo[i].n_ಸೂಚ್ಯಂಕ_ಮೇಲಿನ = bhfi.n_File_ಸೂಚ್ಯಂಕ_ಮೇಲಿನ;
# endif
    ಹಿಂತಿರುಗಿ i;
}


/*
 * Find cscope command in command table.
 */
    ಸ್ಥಿರ cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    ಅಕ್ಷರ *stok;
    size_t len;

    ಆದರೆ (eap->arg == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (ಪೂರ್ಣಾಂಕ)STRLEN(eap->arg);

    ಆದರೆ ((stok = strtok((ಅಕ್ಷರ *)(eap->arg), (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ")) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(stok);
    ಗಾಗಿ (cmdp = cs_cmds; cmdp->name != ಖಾಲಿ; ++cmdp)
    {
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(stok), cmdp->name, len) == 0)
	    ಹಿಂತಿರುಗಿ (cmdp);
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}


/*
 * Nuke em.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_kill(exarg_T *eap ಬಳಸದ)
{
    ಅಕ್ಷರ *stok;
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ ((stok = strtok((ಅಕ್ಷರ *)ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ")) == ಖಾಲಿ)
    {
	cs_usage_ಸಂದೇಶ(Kill);
	ಹಿಂತಿರುಗಿ CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    ಆದರೆ ((ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(stok) < 2 && VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(stok[0])))
	    || (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(stok[1]))))
	i = atoi(stok);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
	{
	    ಆದರೆ (csinfo[i].fname != ಖಾಲಿ && ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(csinfo[i].fname, stok))
		ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ ((i != -1) && (i >= csinfo_ಗಾತ್ರ || i < -1 || csinfo[i].fname == ಖಾಲಿ))
    {
	ಆದರೆ (p_csverbose)
	    (ಶೂನ್ಯ)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (i == -1)
	{
	    ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
	    {
		ಆದರೆ (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    cs_kill_execute(i, stok);
    }

    ಹಿಂತಿರುಗಿ 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cs_kill_execute(
    ಪೂರ್ಣಾಂಕ i,	    // cscope table index
    ಅಕ್ಷರ *cname)    // cscope database name
{
    ಆದರೆ (p_csverbose)
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
    ಸ್ಥಿರ ಅಕ್ಷರ *
cs_make_vim_style_matches(
    ಅಕ್ಷರ *fname,
    ಅಕ್ಷರ *slno,
    ಅಕ್ಷರ *ಹುಡುಕು,
    ಅಕ್ಷರ *tagstr)
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
    ಅಕ್ಷರ *buf;
    ಪೂರ್ಣಾಂಕ amt;

    ಆದರೆ (ಹುಡುಕು != ಖಾಲಿ)
    {
	amt = (ಪೂರ್ಣಾಂಕ)(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(fname) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(slno) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tagstr) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಹುಡುಕು)+6);
	ಆದರೆ ((buf = alloc(amt)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	(ಶೂನ್ಯ)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, ಹುಡುಕು);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	amt = (ಪೂರ್ಣಾಂಕ)(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(fname) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(slno) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tagstr) + 5);
	ಆದರೆ ((buf = alloc(amt)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	(ಶೂನ್ಯ)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    ಹಿಂತಿರುಗಿ buf;
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
    ಸ್ಥಿರ ಅಕ್ಷರ *
cs_manage_matches(
    ಅಕ್ಷರ **matches,
    ಅಕ್ಷರ **contexts,
    ಪೂರ್ಣಾಂಕ totmatches,
    mcmd_e cmd)
{
    ಸ್ಥಿರ ಅಕ್ಷರ **mp = ಖಾಲಿ;
    ಸ್ಥಿರ ಅಕ್ಷರ **cp = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cnt = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮುಂದಿನ = -1;
    ಅಕ್ಷರ *p = ಖಾಲಿ;

    ಆಯ್ಕೆ (cmd)
    {
    ಸ್ಥಿತಿ Store:
	ಪ್ರತಿಜ್ಞಾ(matches != ಖಾಲಿ);
	ಪ್ರತಿಜ್ಞಾ(totmatches > 0);
	ಆದರೆ (mp != ಖಾಲಿ || cp != ಖಾಲಿ)
	    (ಶೂನ್ಯ)cs_manage_matches(ಖಾಲಿ, ಖಾಲಿ, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	ಮುಂದಿನ = 0;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ Get:
	ಆದರೆ (ಮುಂದಿನ >= cnt)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	p = mp[ಮುಂದಿನ];
	ಮುಂದಿನ++;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ Free:
	ಆದರೆ (mp != ಖಾಲಿ)
	{
	    ಆದರೆ (cnt > 0)
		ಆಗ (cnt--)
		{
		    vim_free(mp[cnt]);
		    ಆದರೆ (cp != ಖಾಲಿ)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = ಖಾಲಿ;
	cp = ಖಾಲಿ;
	cnt = 0;
	ಮುಂದಿನ = 0;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ Print:
	cs_print_ಟ್ಯಾಗುಗಳು_priv(mp, cp, cnt);
	ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:	// should not reach here
	iemsg(e_fatal_error_ಒಳ_cs_manage_matches);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ p;
}


/*
 * Parse cscope output.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
cs_parse_results(
    ಪೂರ್ಣಾಂಕ cnumber,
    ಅಕ್ಷರ *buf,
    ಪೂರ್ಣಾಂಕ bufsize,
    ಅಕ್ಷರ **context,
    ಅಕ್ಷರ **linenumber,
    ಅಕ್ಷರ **ಹುಡುಕು)
{
    ಪೂರ್ಣಾಂಕ ch;
    ಅಕ್ಷರ *p;
    ಅಕ್ಷರ *name;

    ಆದರೆ (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == ಖಾಲಿ)
    {
	ಆದರೆ (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // If the line's too long for the buffer, discard it.
    ಆದರೆ ((p = strchr(buf, '\n')) == ಖಾಲಿ)
    {
	ಆಗ ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    ಆದರೆ ((name = strtok(buf, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ")) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ ((*context = strtok(ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ")) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ ((*linenumber = strtok(ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)" ")) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    *ಹುಡುಕು = *linenumber + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    ಆದರೆ (strcmp(*ಹುಡುಕು, "<unknown>") == 0)
	*ಹುಡುಕು = ಖಾಲಿ;

    name = cs_resolve_file(cnumber, name);
    ಹಿಂತಿರುಗಿ name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cs_file_results(FILE *f, ಪೂರ್ಣಾಂಕ *nummatches_a)
{
    ಪೂರ್ಣಾಂಕ i, j;
    ಅಕ್ಷರ *buf;
    ಅಕ್ಷರ *ಹುಡುಕು, *slno;
    ಅಕ್ಷರ *fullname;
    ಅಕ್ಷರ *cntx;
    ಅಕ್ಷರ *context;

    buf = alloc(CSREAD_BUFSIZE);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
    {
	ಆದರೆ (nummatches_a[i] < 1)
	    ಮುಂದುವರಿ;

	ಗಾಗಿ (j = 0; j < nummatches_a[i]; j++)
	{
	   ಆದರೆ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &ಹುಡುಕು)) == ಖಾಲಿ)
	       ಮುಂದುವರಿ;

	   context = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cntx)+5);
	   ಆದರೆ (context == ಖಾಲಿ)
	   {
	       vim_free(fullname);
	       ಮುಂದುವರಿ;
	   }

	   ಆದರೆ (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   ಇಲ್ಲದಿದ್ದರೆ
	       sprintf(context, "<<%s>>", cntx);

	   ಆದರೆ (ಹುಡುಕು == ಖಾಲಿ)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   ಇಲ್ಲದಿದ್ದರೆ
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
    ಅಕ್ಷರ *tagstr,
    ಪೂರ್ಣಾಂಕ totmatches,
    ಪೂರ್ಣಾಂಕ *nummatches_a,
    ಅಕ್ಷರ ***matches_p,
    ಅಕ್ಷರ ***cntxts_p,
    ಪೂರ್ಣಾಂಕ *matched)
{
    ಪೂರ್ಣಾಂಕ i, j;
    ಅಕ್ಷರ *buf;
    ಅಕ್ಷರ *ಹುಡುಕು, *slno;
    ಪೂರ್ಣಾಂಕ totsofar = 0;
    ಅಕ್ಷರ **matches = ಖಾಲಿ;
    ಅಕ್ಷರ **cntxts = ಖಾಲಿ;
    ಅಕ್ಷರ *fullname;
    ಅಕ್ಷರ *cntx;

    ಪ್ರತಿಜ್ಞಾ(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((matches = ALLOC_MULT(ಅಕ್ಷರ *, totmatches)) == ಖಾಲಿ)
	ಹೋಗು parse_ಹೊರಹೋಗು;
    ಆದರೆ ((cntxts = ALLOC_MULT(ಅಕ್ಷರ *, totmatches)) == ಖಾಲಿ)
	ಹೋಗು parse_ಹೊರಹೋಗು;

    ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
    {
	ಆದರೆ (nummatches_a[i] < 1)
	    ಮುಂದುವರಿ;

	ಗಾಗಿ (j = 0; j < nummatches_a[i]; j++)
	{
	   ಆದರೆ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &ಹುಡುಕು)) == ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  ಹುಡುಕು, tagstr);

	    vim_free(fullname);

	    ಆದರೆ (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (ಅಕ್ಷರ *)vim_strsave((char_u *)cntx);

	    ಆದರೆ (matches[totsofar] != ಖಾಲಿ)
		totsofar++;

	} // for all matches

	(ಶೂನ್ಯ)cs_read_prompt(i);

    } // for all cscope connections

parse_ಹೊರಹೋಗು:
    ಆದರೆ (totsofar == 0)
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
    ಸ್ಥಿರ ಅಕ್ಷರ *
cs_pathcomponents(ಅಕ್ಷರ *ಮಾರ್ಗ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಅಕ್ಷರ	*s;

    ಆದರೆ (p_cspc == 0)
	ಹಿಂತಿರುಗಿ ಮಾರ್ಗ;

    s = ಮಾರ್ಗ + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ) - 1;
    ಗಾಗಿ (i = 0; i < p_cspc; ++i)
    {
	ಆಗ (s > ಮಾರ್ಗ)
	{
	   s--;
	   ಆದರೆ (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ ((s > ಮಾರ್ಗ && *s == '/')
# ifdef MSWIN
	|| (s > ಮಾರ್ಗ && *s == '\\')
# endif
	    )
	++s;
    ಹಿಂತಿರುಗಿ s;
}

/*
 * Called from cs_manage_matches().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cs_print_ಟ್ಯಾಗುಗಳು_priv(ಅಕ್ಷರ **matches, ಅಕ್ಷರ **cntxts, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_matches)
{
    ಅಕ್ಷರ	*buf = ಖಾಲಿ;
    ಅಕ್ಷರ	*t_buf;
    ಪೂರ್ಣಾಂಕ		bufsize = 0; // Track available bufsize
    ಪೂರ್ಣಾಂಕ		newsize = 0;
    ಅಕ್ಷರ	*ptag;
    ಅಕ್ಷರ	*fname, *lno, *extra, *tbuf;
    ಪೂರ್ಣಾಂಕ		i, idx, ಅಂಕೆ;
    ಅಕ್ಷರ	*globalcntx = "GLOBAL";
    ಅಕ್ಷರ	*cntxformat = " <<%s>>";
    ಅಕ್ಷರ	*context;
    ಅಕ್ಷರ	*cstag_ಸಂದೇಶ = _("Cscope tag: %s");
    ಅಕ್ಷರ	*csfmt_str = "%4d %6s  ";

    ಪ್ರತಿಜ್ಞಾ(ಅಂಕೆ_matches > 0);

    ಆದರೆ ((tbuf = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(matches[0]) + 1)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    ಆದರೆ (ptag == ಖಾಲಿ)
    {
	vim_free(tbuf);
	ಹಿಂತಿರುಗಿ;
    }

    newsize = (ಪೂರ್ಣಾಂಕ)(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cstag_ಸಂದೇಶ) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ptag));
    buf = alloc(newsize);
    ಆದರೆ (buf != ಖಾಲಿ)
    {
	bufsize = newsize;
	(ಶೂನ್ಯ)sprintf(buf, cstag_ಸಂದೇಶ, ptag);
	ಸಂದೇಶ_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    ಸಂದೇಶ_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    ಸಂದೇಶ_advance(ಸಂದೇಶ_ಲಂಬಸಾಲು + 2);
    ಸಂದೇಶ_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    ಅಂಕೆ = 1;
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	ಆದರೆ ((tbuf = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(matches[idx]) + 1)) == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	(ಶೂನ್ಯ)strcpy(tbuf, matches[idx]);

	ಆದರೆ (strtok(tbuf, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)"\t") == ಖಾಲಿ
		|| (fname = strtok(ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)"\t")) == ಖಾಲಿ
		|| (lno = strtok(ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)"\t")) == ಖಾಲಿ)
	{
	    vim_free(tbuf);
	    ಮುಂದುವರಿ;
	}
	extra = strtok(ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)"\t");

	lno[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (ಪೂರ್ಣಾಂಕ)(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(csfmt_str) + 16 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(lno));
	ಆದರೆ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		bufsize = newsize;
	}
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    // csfmt_str = "%4d %6s  ";
	    (ಶೂನ್ಯ)sprintf(buf, csfmt_str, ಅಂಕೆ, lno);
	    ಸಂದೇಶ_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	ಸಂದೇಶ_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	ಆದರೆ (cntxts[idx] != ಖಾಲಿ)
	    context = cntxts[idx];
	ಇಲ್ಲದಿದ್ದರೆ
	    context = globalcntx;
	newsize = (ಪೂರ್ಣಾಂಕ)(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cntxformat));

	ಆದರೆ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		bufsize = newsize;
	}
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    (ಶೂನ್ಯ)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು + (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf) >= cmdline_ಅಗಲ)
		ಸಂದೇಶ_putchar('\n');
	    ಸಂದೇಶ_advance(12);
	    ಸಂದೇಶ_outtrans_long_attr((char_u *)buf, 0);
	    ಸಂದೇಶ_putchar('\n');
	}
	ಆದರೆ (extra != ಖಾಲಿ)
	{
	    ಸಂದೇಶ_advance(13);
	    ಸಂದೇಶ_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು)
	    ಸಂದೇಶ_putchar('\n');

	ui_breakcheck();
	ಆದರೆ (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    ನಿಲ್ಲಿಸು;
	}

	ಅಂಕೆ++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_read_prompt(ಪೂರ್ಣಾಂಕ i)
{
    ಪೂರ್ಣಾಂಕ		ch;
    ಅಕ್ಷರ	*buf = ಖಾಲಿ; // buffer for possible error message from cscope
    ಪೂರ್ಣಾಂಕ		bufpos = 0;
    ಪೂರ್ಣಾಂಕ		maxlen;
    ಸ್ಥಿರ ಅಕ್ಷರ *eprompt = "Press the RETURN key to continue:";
    ಪೂರ್ಣಾಂಕ		epromptlen = (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(eprompt);
    ಪೂರ್ಣಾಂಕ		n;

    // compute maximum allowed len for Cscope error message
    ಅಕ್ಷರ *cs_emsg = _(e_cscope_error_str);
    maxlen = (ಪೂರ್ಣಾಂಕ)(IOSIZE - ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cs_emsg));

    ಗಾಗಿ (;;)
    {
	ಆಗ ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    ಆದರೆ (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		ಆದರೆ (buf == ಖಾಲಿ) // lazy buffer allocation
		    buf = alloc(maxlen);
		ಆದರೆ (buf != ಖಾಲಿ)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    ಆದರೆ (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(ಶೂನ್ಯ)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(ಶೂನ್ಯ)putc('\n', csinfo[i].ಗಮ್ಯ_fp);
			(ಶೂನ್ಯ)fflush(csinfo[i].ಗಮ್ಯ_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	ಗಾಗಿ (n = 0; n < (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    ಆದರೆ (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    ಆದರೆ (ch == EOF)
	    {
		ಆದರೆ (buf != ಖಾಲಿ && buf[0] != NUL)
		    (ಶೂನ್ಯ)semsg(cs_emsg, buf);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		ಹಿಂತಿರುಗಿ CSCOPE_FAILURE;
	    }

	    ಆದರೆ (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		ನಿಲ್ಲಿಸು;
	    }
	}

	ಆದರೆ (ch == EOF)
	    ಮುಂದುವರಿ;	    // didn't find the prompt
	ನಿಲ್ಲಿಸು;		    // did find the prompt
    }

    vim_free(buf);
    ಹಿಂತಿರುಗಿ CSCOPE_SUCCESS;
}

# ಆದರೆ defined(UNIX) && defined(SIGALRM)
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
cs_release_csp(ಪೂರ್ಣಾಂಕ i, ಪೂರ್ಣಾಂಕ freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    ಆದರೆ (csinfo[i].ಗಮ್ಯ_fp != ಖಾಲಿ)
    {
	(ಶೂನ್ಯ)fputs("q\n", csinfo[i].ಗಮ್ಯ_fp);
	(ಶೂನ್ಯ)fflush(csinfo[i].ಗಮ್ಯ_fp);
    }
# ಆದರೆ defined(UNIX)
    {
	ಪೂರ್ಣಾಂಕ waitpid_errno;
	ಪೂರ್ಣಾಂಕ pstat;
	pid_t pid;

#  ಆದರೆ defined(HAVE_SIGACTION)
	ರಚನೆ sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   ಇಲ್ಲದಿದ್ದರೆ
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, ಖಾಲಿ);
#  ಇಲ್ಲದಿದ್ದರೆ
	ಪೂರ್ಣಾಂಕ waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	ಗಾಗಿ (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    ಆದರೆ (pid != 0)
		ನಿಲ್ಲಿಸು;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	ಆದರೆ (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    ಪೂರ್ಣಾಂಕ alive = TRUE;

	    ಆದರೆ (waitpid_errno == ECHILD)
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
		ಪೂರ್ಣಾಂಕ waited;

		sleep(0);
		ಗಾಗಿ (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    ಆದರೆ (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			ನಿಲ್ಲಿಸು;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    ಆದರೆ (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(ಶೂನ್ಯ)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# ಇಲ್ಲದಿದ್ದರೆ  // !UNIX
    ಆದರೆ (csinfo[i].hProc != ಖಾಲಿ)
    {
	// Give cscope a chance to exit normally
	ಆದರೆ (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    ಆದರೆ (csinfo[i].fr_fp != ಖಾಲಿ)
	(ಶೂನ್ಯ)fclose(csinfo[i].fr_fp);
    ಆದರೆ (csinfo[i].ಗಮ್ಯ_fp != ಖಾಲಿ)
	(ಶೂನ್ಯ)fclose(csinfo[i].ಗಮ್ಯ_fp);

    ಆದರೆ (freefnpp)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_reset(exarg_T *eap ಬಳಸದ)
{
    ಅಕ್ಷರ	**dblist = ಖಾಲಿ, **pplist = ಖಾಲಿ, **fllist = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ	i;
    ಅಕ್ಷರ buf[20]; // for sprintf " (#%d)"

    ಆದರೆ (csinfo_ಗಾತ್ರ == 0)
	ಹಿಂತಿರುಗಿ CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(ಅಕ್ಷರ *, csinfo_ಗಾತ್ರ);
    pplist = ALLOC_MULT(ಅಕ್ಷರ *, csinfo_ಗಾತ್ರ);
    fllist = ALLOC_MULT(ಅಕ್ಷರ *, csinfo_ಗಾತ್ರ);
    ಆದರೆ (dblist == ಖಾಲಿ || pplist == ಖಾಲಿ || fllist == ಖಾಲಿ)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	ಹಿಂತಿರುಗಿ CSCOPE_FAILURE;
    }

    ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	ಆದರೆ (csinfo[i].fname != ಖಾಲಿ)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
    {
	ಆದರೆ (dblist[i] != ಖಾಲಿ)
	{
	    cs_ಕೂಡಿಸು_common(dblist[i], pplist[i], fllist[i]);
	    ಆದರೆ (p_csverbose)
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

    ಆದರೆ (p_csverbose)
	ಸಂದೇಶ_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | ಸಂದೇಶ_HIST);
    ಹಿಂತಿರುಗಿ CSCOPE_SUCCESS;
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
    ಸ್ಥಿರ ಅಕ್ಷರ *
cs_resolve_file(ಪೂರ್ಣಾಂಕ i, ಅಕ್ಷರ *name)
{
    ಅಕ್ಷರ	*fullname;
    string_T	csdir = {ಖಾಲಿ, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    ಪೂರ್ಣಾಂಕ		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (ಪೂರ್ಣಾಂಕ)namelen + 2;
    ಆದರೆ (csinfo[i].ppath != ಖಾಲಿ)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (ಪೂರ್ಣಾಂಕ)ppathlen;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_csre && csinfo[i].fname != ಖಾಲಿ)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	ಆದರೆ (csdir.string != ಖಾಲಿ)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.ಉದ್ದಳತೆ = STRLEN(csdir.string);
	    len += (ಪೂರ್ಣಾಂಕ)csdir.ಉದ್ದಳತೆ;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    ಆದರೆ (csinfo[i].ppath != ಖಾಲಿ
	    && (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	ಆದರೆ ((fullname = alloc(len)) != ಖಾಲಿ)
	    (ಶೂನ್ಯ)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (csdir.string != ಖಾಲಿ && csinfo[i].fname != ಖಾಲಿ && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (ಅಕ್ಷರ *)concat_fnames(csdir.string, csdir.ಉದ್ದಳತೆ, (char_u *)name, namelen, TRUE, &ret);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fullname = (ಅಕ್ಷರ *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    ಹಿಂತಿರುಗಿ fullname;
}


/*
 * Show all cscope connections.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cs_show(exarg_T *eap ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (cs_cnt_connections() == 0)
	ಸಂದೇಶ_puts(_("no cscope connections\n"));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಂದೇಶ_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
	{
	    ಆದರೆ (csinfo[i].fname == ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    ಆದರೆ (csinfo[i].ppath != ಖಾಲಿ)
		(ಶೂನ್ಯ)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (ದೀರ್ಘ)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    ಇಲ್ಲದಿದ್ದರೆ
		(ಶೂನ್ಯ)smsg("%2d %-5ld  %-34s  <none>",
			   i, (ದೀರ್ಘ)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    ಹಿಂತಿರುಗಿ CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    ಶೂನ್ಯ
cs_ಕೊನೆ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < csinfo_ಗಾತ್ರ; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_ಗಾತ್ರ = 0;
}

#ಆದರೆ_ಅಂತ್ಯ	// FEAT_CSCOPE

#ಆದರೆ defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    ಶೂನ್ಯ
f_cscope_connection(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_CSCOPE
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ = 0;
    char_u	*dbpath = ಖಾಲಿ;
    char_u	*prepend = ಖಾಲಿ;
    char_u	buf[NUMBUFLEN];

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	ಅಂಕೆ = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = cs_connection(ಅಂಕೆ, dbpath, prepend);
# endif
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
