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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# જો defined(UNIX)
#  include <sys/wait.h>
# endif

# જો defined (MSWIN)
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

પ્રકારવ્યાખ્યા સંરચના {
    અક્ષર *  name;
    પૂર્ણાંક     (*func)(exarg_T *eap);
    અક્ષર *  help;
    અક્ષર *  usage;
    પૂર્ણાંક	    cansplit;		// if supports splitting window
} cscmd_T;

પ્રકારવ્યાખ્યા સંરચના csi {
    અક્ષર *	    fname;	// cscope db name
    અક્ષર *	    ppath;	// path to prepend (the -P option)
    અક્ષર *	    flags;	// additional cscope flags/options (e.g, -p2)
# જો defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# નહીંતર
#  જો defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_Index_ઉપલું;	// st_ino has no meaning in the Windows
    DWORD	    n_Index_નીચલું;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    to_fp;	// to cscope: FILE.
} csinfo_T;

પ્રકારવ્યાખ્યા ગણના { Add, Find, Help, Kill, Reset, Show } csid_e;

પ્રકારવ્યાખ્યા ગણના {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

સ્થિર પૂર્ણાંક	    cs_add(exarg_T *eap);
સ્થિર પૂર્ણાંક	    cs_add_common(અક્ષર *, અક્ષર *, અક્ષર *);
સ્થિર પૂર્ણાંક	    cs_check_for_connections(શૂન્ય);
સ્થિર પૂર્ણાંક	    cs_check_for_tags(શૂન્ય);
સ્થિર પૂર્ણાંક	    cs_cnt_connections(શૂન્ય);
સ્થિર પૂર્ણાંક	    cs_create_connection(પૂર્ણાંક i);
# ifdef FEAT_QUICKFIX
સ્થિર શૂન્ય	    cs_file_results(FILE *, પૂર્ણાંક *);
# endif
સ્થિર શૂન્ય	    cs_fill_results(અક્ષર *, પૂર્ણાંક , પૂર્ણાંક *, અક્ષર ***,
			અક્ષર ***, પૂર્ણાંક *);
સ્થિર પૂર્ણાંક	    cs_find(exarg_T *eap);
સ્થિર પૂર્ણાંક	    cs_find_common(અક્ષર *opt, અક્ષર *pat, પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક, char_u *cmdline);
સ્થિર પૂર્ણાંક	    cs_help(exarg_T *eap);
સ્થિર પૂર્ણાંક	    cs_ઉમેરો_filelist(અક્ષર *, અક્ષર *, અક્ષર *,
			stat_T *);
સ્થિર પૂર્ણાંક	    cs_kill(exarg_T *eap);
સ્થિર શૂન્ય	    cs_kill_execute(પૂર્ણાંક, અક્ષર *);
સ્થિર cscmd_T *    cs_lookup_cmd(exarg_T *eap);
સ્થિર અક્ષર *	    cs_make_vim_style_matches(અક્ષર *, અક્ષર *,
			અક્ષર *, અક્ષર *);
સ્થિર અક્ષર *	    cs_manage_matches(અક્ષર **, અક્ષર **, પૂર્ણાંક, mcmd_e);
સ્થિર શૂન્ય	    cs_print_tags_priv(અક્ષર **, અક્ષર **, પૂર્ણાંક);
સ્થિર પૂર્ણાંક	    cs_read_prompt(પૂર્ણાંક);
સ્થિર શૂન્ય	    cs_release_csp(પૂર્ણાંક, પૂર્ણાંક freefnpp);
સ્થિર પૂર્ણાંક	    cs_reset(exarg_T *eap);
સ્થિર અક્ષર *	    cs_resolve_file(પૂર્ણાંક, અક્ષર *);
સ્થિર પૂર્ણાંક	    cs_show(exarg_T *eap);


સ્થિર csinfo_T *   csinfo = ખાલી;
સ્થિર પૂર્ણાંક	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

સ્થિર પૂર્ણાંક	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
સ્થિર cscmd_T	    cs_cmds[] =
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
    { ખાલી, ખાલી, ખાલી, ખાલી, 0 }
};

    સ્થિર શૂન્ય
cs_usage_સંદેશ(csid_e x)
{
    (શૂન્ય)semsg(_(e_usage_cscope_str), cs_cmds[(પૂર્ણાંક)x].usage);
}

સ્થિર ગણના
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
get_cscope_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    પૂર્ણાંક		current_idx;
    પૂર્ણાંક		i;

    પસંદગી (expand_what)
    {
    સ્થિતિ EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	પાછા (char_u *)cs_cmds[idx].name;
    સ્થિતિ EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	માટે (i = 0, current_idx = 0; cs_cmds[i].name != ખાલી; i++)
	    જો (cs_cmds[i].cansplit)
		જો (current_idx++ == idx)
		    તોડો;
	પાછા (char_u *)cs_cmds[i].name;
    સ્થિતિ EXP_CSCOPE_FIND:
	{
	    અચલ અક્ષર *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", ખાલી
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    પાછા (char_u *)query_type[idx];
	}
    સ્થિતિ EXP_CSCOPE_KILL:
	{
	    સ્થિર અક્ષર	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    માટે (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		જો (csinfo[i].fname == ખાલી)
		    ચાલુ;
		જો (current_idx++ == idx)
		{
		    vim_snprintf(connection, કદ(connection), "%d", i);
		    પાછા (char_u *)connection;
		}
	    }
	    પાછા (current_idx == idx && idx > 0) ? (char_u *)"-1" : ખાલી;
	}
    મૂળભૂત:
	પાછા ખાલી;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    શૂન્ય
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

    જો (*arg == NUL)
	પાછા;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    જો (*p == NUL)
	પાછા;

    // past first word
    xp->xp_pattern = skipwhite(p);
    જો (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    નહીંતર જો (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    નહીંતર જો (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    નહીંતર જો (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    નહીંતર
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    સ્થિર શૂન્ય
do_cscope_general(
    exarg_T	*eap,
    પૂર્ણાંક		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    જો (check_restricted())
	પાછા;

    જો ((cmdp = cs_lookup_cmd(eap)) == ખાલી)
    {
	cs_help(eap);
	પાછા;
    }

    જો (make_split)
    {
	જો (!cmdp->cansplit)
	{
	    (શૂન્ય)સંદેશ_puts(_("This cscope command does not support splitting the window.\n"));
	    પાછા;
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
    શૂન્ય
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    શૂન્ય
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    શૂન્ય
ex_cstag(exarg_T *eap)
{
    પૂર્ણાંક ret = FALSE;

    જો (*eap->arg == NUL)
    {
	(શૂન્ય)emsg(_(e_usage_cstag_ident));
	પાછા;
    }

    પસંદગી (p_csto)
    {
    સ્થિતિ 0 :
	જો (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (અક્ષર *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    જો (ret == FALSE)
	    {
		cs_free_tags();
		જો (સંદેશ_col)
		    સંદેશ_putchar('\n');

		જો (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	નહીંતર જો (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	તોડો;
    સ્થિતિ 1 :
	જો (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    જો (ret == FALSE)
	    {
		જો (સંદેશ_col)
		    સંદેશ_putchar('\n');

		જો (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (અક્ષર *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    જો (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	નહીંતર જો (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (અક્ષર *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    જો (ret == FALSE)
		cs_free_tags();
	}
	તોડો;
    મૂળભૂત :
	તોડો;
    }

    જો (!ret)
    {
	(શૂન્ય)emsg(_(e_cstag_tag_not_founc));
# જો defined(FEAT_QUICKFIX)
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
    પૂર્ણાંક
cs_fgets(char_u *buf, પૂર્ણાંક size)
{
    અક્ષર *p;

    જો ((p = cs_manage_matches(ખાલી, ખાલી, -1, Get)) == ખાલી)
	પાછા TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    પાછા FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    શૂન્ય
cs_free_tags(શૂન્ય)
{
    cs_manage_matches(ખાલી, ખાલી, -1, Free);
}


/*
 * Called from do_tag().
 */
    શૂન્ય
cs_print_tags(શૂન્ય)
{
    cs_manage_matches(ખાલી, ખાલી, -1, Print);
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
# જો defined(FEAT_EVAL)
    સ્થિર પૂર્ણાંક
cs_connection(પૂર્ણાંક અંક, char_u *dbpath, char_u *ppath)
{
    પૂર્ણાંક i;

    જો (અંક < 0 || અંક > 4 || (અંક > 0 && !dbpath))
	પાછા FALSE;

    માટે (i = 0; i < csinfo_size; i++)
    {
	જો (!csinfo[i].fname)
	    ચાલુ;

	જો (અંક == 0)
	    પાછા TRUE;

	પસંદગી (અંક)
	{
	સ્થિતિ 1:
	    જો (strstr(csinfo[i].fname, (અક્ષર *)dbpath))
		પાછા TRUE;
	    તોડો;
	સ્થિતિ 2:
	    જો (strcmp(csinfo[i].fname, (અક્ષર *)dbpath) == 0)
		પાછા TRUE;
	    તોડો;
	સ્થિતિ 3:
	    જો (strstr(csinfo[i].fname, (અક્ષર *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (અક્ષર *)ppath))))
		પાછા TRUE;
	    તોડો;
	સ્થિતિ 4:
	    જો ((strcmp(csinfo[i].fname, (અક્ષર *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (અક્ષર *)ppath) == 0))))
		પાછા TRUE;
	    તોડો;
	}
    }

    પાછા FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    સ્થિર પૂર્ણાંક
cs_add(exarg_T *eap UNUSED)
{
    અક્ષર *fname, *ppath, *flags = ખાલી;

    જો ((fname = strtok((અક્ષર *)ખાલી, (અચલ અક્ષર *)" ")) == ખાલી)
    {
	cs_usage_સંદેશ(Add);
	પાછા CSCOPE_FAILURE;
    }
    જો ((ppath = strtok((અક્ષર *)ખાલી, (અચલ અક્ષર *)" ")) != ખાલી)
	flags = strtok((અક્ષર *)ખાલી, (અચલ અક્ષર *)" ");

    પાછા cs_add_common(fname, ppath, flags);
}

    સ્થિર શૂન્ય
cs_stat_emsg(અક્ષર *fname)
{
    પૂર્ણાંક err = errno;
    (શૂન્ય)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    સ્થિર પૂર્ણાંક
cs_add_common(
    અક્ષર *arg1,	    // filename - may contain environment variables
    અક્ષર *arg2,	    // prepend path - may contain environment variables
    અક્ષર *flags)
{
    stat_T	statbuf;
    પૂર્ણાંક		ret;
    અક્ષર	*fname = ખાલી;
    અક્ષર	*fname2 = ખાલી;
    અક્ષર	*ppath = ખાલી;
    પૂર્ણાંક		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = ખાલી;

    // get the filename (arg1), expand it, and try to stat it
    જો ((fname = alloc(MAXPATHL + 1)) == ખાલી)
	જાઓ add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (શૂન્ય)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    જો (fname == ખાલી)
	જાઓ add_err;
    fname = (અક્ષર *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    જો (ret < 0)
    {
staterr:
	જો (p_csverbose)
	    cs_stat_emsg(fname);
	જાઓ add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    જો (arg2 != ખાલી)
    {
	stat_T	    statbuf2;

	જો ((ppath = alloc(MAXPATHL + 1)) == ખાલી)
	    જાઓ add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	જો (ret < 0)
	    જાઓ staterr;
    }

    // if filename is a directory, append the cscope database name to it
    જો (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	જો (fname2 == ખાલી)
	    જાઓ add_err;

	જ્યારે (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    જો (fname[0] == '\0')
		તોડો;
	}
	જો (fname[0] == '\0')
	    (શૂન્ય)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	નહીંતર
	    (શૂન્ય)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	જો (ret < 0)
	{
	    જો (p_csverbose)
		cs_stat_emsg(fname2);
	    જાઓ add_err;
	}

	i = cs_ઉમેરો_filelist(fname2, ppath, flags, &statbuf);
    }
    નહીંતર જો (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_ઉમેરો_filelist(fname, ppath, flags, &statbuf);
    }
    નહીંતર
    {
	જો (p_csverbose)
	    (શૂન્ય)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	જાઓ add_err;
    }

    જો (i != -1)
    {
	જો (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    જાઓ add_err;
	}

	જો (p_csverbose)
	{
	    સંદેશ_clr_eos();
	    (શૂન્ય)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    પાછા CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    પાછા CSCOPE_FAILURE;
}


    સ્થિર પૂર્ણાંક
cs_check_for_connections(શૂન્ય)
{
    પાછા (cs_cnt_connections() > 0);
}


    સ્થિર પૂર્ણાંક
cs_check_for_tags(શૂન્ય)
{
    પાછા (p_tags[0] != NUL && curbuf->b_p_tags != ખાલી);
}


/*
 * Count the number of cscope connections.
 */
    સ્થિર પૂર્ણાંક
cs_cnt_connections(શૂન્ય)
{
    પૂર્ણાંક i;
    પૂર્ણાંક cnt = 0;

    માટે (i = 0; i < csinfo_size; i++)
	જો (csinfo[i].fname != ખાલી)
	    cnt++;
    પાછા cnt;
}

    સ્થિર શૂન્ય
cs_reading_emsg(
    પૂર્ણાંક idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    સ્થિર પૂર્ણાંક
cs_cnt_matches(પૂર્ણાંક idx)
{
    અક્ષર *stok;
    અક્ષર *buf;
    પૂર્ણાંક nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    જો (buf == ખાલી)
	પાછા 0;
    માટે (;;)
    {
	જો (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    જો (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    પાછા -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	જો (strstr((અચલ અક્ષર *)buf, "Unable to search database") != ખાલી)
	    તોડો;
	જો ((stok = strtok(buf, (અચલ અક્ષર *)" ")) == ખાલી)
	    ચાલુ;
	જો (strstr((અચલ અક્ષર *)stok, "cscope:") == ખાલી)
	    ચાલુ;

	જો ((stok = strtok(ખાલી, (અચલ અક્ષર *)" ")) == ખાલી)
	    ચાલુ;
	nlines = atoi(stok);
	જો (nlines < 0)
	{
	    nlines = 0;
	    તોડો;
	}

	જો ((stok = strtok(ખાલી, (અચલ અક્ષર *)" ")) == ખાલી)
	    ચાલુ;
	જો (strncmp((અચલ અક્ષર *)stok, "lines", 5))
	    ચાલુ;

	તોડો;
    }

    vim_free(buf);
    પાછા nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    સ્થિર અક્ષર *
cs_create_cmd(અક્ષર *csoption, અક્ષર *pattern)
{
    અક્ષર *cmd;
    short શોધ;
    અક્ષર *pat;

    પસંદગી (csoption[0])
    {
    સ્થિતિ '0' : સ્થિતિ 's' :
	શોધ = 0;
	તોડો;
    સ્થિતિ '1' : સ્થિતિ 'g' :
	શોધ = 1;
	તોડો;
    સ્થિતિ '2' : સ્થિતિ 'd' :
	શોધ = 2;
	તોડો;
    સ્થિતિ '3' : સ્થિતિ 'c' :
	શોધ = 3;
	તોડો;
    સ્થિતિ '4' : સ્થિતિ 't' :
	શોધ = 4;
	તોડો;
    સ્થિતિ '6' : સ્થિતિ 'e' :
	શોધ = 6;
	તોડો;
    સ્થિતિ '7' : સ્થિતિ 'f' :
	શોધ = 7;
	તોડો;
    સ્થિતિ '8' : સ્થિતિ 'i' :
	શોધ = 8;
	તોડો;
    સ્થિતિ '9' : સ્થિતિ 'a' :
	શોધ = 9;
	તોડો;
    મૂળભૂત :
	(શૂન્ય)emsg(_(e_unknown_cscope_શોધ_type));
	cs_usage_સંદેશ(Find);
	પાછા ખાલી;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    જો (શોધ != 4 && શોધ != 6)
	જ્યારે VIM_ISWHITE(*pat)
	    ++pat;

    જો ((cmd = alloc(strlen(pat) + 2)) == ખાલી)
	પાછા ખાલી;

    (શૂન્ય)sprintf(cmd, "%d%s", શોધ, pat);

    પાછા cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    સ્થિર પૂર્ણાંક
cs_create_connection(પૂર્ણાંક i)
{
# ifdef UNIX
    પૂર્ણાંક		to_cs[2], from_cs[2];
# endif
    પૂર્ણાંક		cmdlen;
    અક્ષર	*prog, *cmd, *ppath = ખાલી;
    size_t	proglen;
# ifdef MSWIN
    પૂર્ણાંક		fd;
    પૂર્ણાંક		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# જો defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    જો (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(શૂન્ય)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	જો (to_cs[0] != -1)
	    (શૂન્ય)close(to_cs[0]);
	જો (to_cs[1] != -1)
	    (શૂન્ય)close(to_cs[1]);
	જો (from_cs[0] != -1)
	    (શૂન્ય)close(from_cs[0]);
	જો (from_cs[1] != -1)
	    (શૂન્ય)close(from_cs[1]);
	પાછા CSCOPE_FAILURE;
    }

    જો ((csinfo[i].pid = fork()) == -1)
    {
	(શૂન્ય)emsg(_(e_could_not_fork_for_cscope));
	જાઓ err_closing;
    }
    નહીંતર જો (csinfo[i].pid == 0)	// child: run cscope.
    {
	અક્ષર **argv = ખાલી;

	જો (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	જો (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	જો (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(શૂન્ય)close(to_cs[1]);
	(શૂન્ય)close(from_cs[0]);
# નહીંતર
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = કદ(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ખાલી;

	જો (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (શૂન્ય)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    જો (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    જો (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    પાછા CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	જો ((prog = alloc(MAXPATHL + 1)) == ખાલી)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# નહીંતર
	    // MSWIN
	    જાઓ err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (પૂર્ણાંક)(proglen + strlen(csinfo[i].fname) + 32);
	જો (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    જો ((ppath = alloc(MAXPATHL + 1)) == ખાલી)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# નહીંતર
		// MSWIN
		જાઓ err_closing;
# endif
	    }
	    cmdlen += (પૂર્ણાંક)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	જો (csinfo[i].flags)
	    cmdlen += (પૂર્ણાંક)strlen(csinfo[i].flags);

	જો ((cmd = alloc(cmdlen)) == ખાલી)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# નહીંતર
	    // MSWIN
	    જાઓ err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    અક્ષર	**tok = ખાલી;
	    પૂર્ણાંક		tokc = 0;

	    ga_init2(&ga_argv, કદ(અક્ષર *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    જો (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    માટે (પૂર્ણાંક k = 0; k < tokc; ++k)
	    {
		જો (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((અક્ષર **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    જો (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((અક્ષર **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (અક્ષર *)vim_strsave((char_u *)"-dl");
	    ((અક્ષર **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (અક્ષર *)vim_strsave((char_u *)"-f");
	    ((અક્ષર **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(અક્ષર *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    જો (ppath != ખાલી)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		અક્ષર	*parg = alloc(plen);

		જો (parg == ખાલી || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((અક્ષર **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    જો (csinfo[i].flags != ખાલી)
	    {
		જો (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		માટે (પૂર્ણાંક k = 0; k < tokc; ++k)
		{
		    જો (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((અક્ષર **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    જો (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((અક્ષર **)ga_argv.ga_data)[ga_argv.ga_len] = ખાલી;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  જો defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   જો defined(HAVE_SETSID)
	    (શૂન્ય)setsid();
#   નહીંતર
	    જો (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (અક્ષર **)ga_argv.ga_data;
	    જો (argv[0] == ખાલી || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    નહીંતર	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	જો ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == ખાલી)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	જો ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == ખાલી)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(શૂન્ય)close(to_cs[0]);
	(શૂન્ય)close(from_cs[1]);
    }
# નહીંતર
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    જો (csinfo[i].ppath != ખાલી)
    {
	len = (પૂર્ણાંક)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    જો (csinfo[i].flags != ખાલી)
    {
	len = (પૂર્ણાંક)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_આગત  = stdin_rd;
    created = CreateProcess(ખાલી, cmd, ખાલી, ખાલી, TRUE, CREATE_NEW_CONSOLE,
							ખાલી, ખાલી, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    જો (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(શૂન્ય)emsg(_(e_could_not_spawn_cscope_process));
	જાઓ err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    જો (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == ખાલી))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    જો (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == ખાલી))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    પાછા CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    સ્થિર પૂર્ણાંક
cs_find(exarg_T *eap)
{
    અક્ષર *opt, *pat;
    પૂર્ણાંક i;

    જો (cs_check_for_connections() == FALSE)
    {
	(શૂન્ય)emsg(_(e_no_cscope_connections));
	પાછા FALSE;
    }

    જો ((opt = strtok((અક્ષર *)ખાલી, (અચલ અક્ષર *)" ")) == ખાલી)
    {
	cs_usage_સંદેશ(Find);
	પાછા FALSE;
    }

    pat = opt + strlen(opt) + 1;
    જો (pat >= (અક્ષર *)eap->arg + eap_arg_len)
    {
	cs_usage_સંદેશ(Find);
	પાછા FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    માટે (i = 0; i < eap_arg_len; ++i)
	જો (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    પાછા cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    સ્થિર પૂર્ણાંક
cs_find_common(
    અક્ષર *opt,
    અક્ષર *pat,
    પૂર્ણાંક forceit,
    પૂર્ણાંક verbose,
    પૂર્ણાંક	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    પૂર્ણાંક i;
    અક્ષર *cmd;
    પૂર્ણાંક *nummatches;
    પૂર્ણાંક totmatches;
# ifdef FEAT_QUICKFIX
    અક્ષર cmdletter;
    અક્ષર *qfpos;

    // get cmd letter
    પસંદગી (opt[0])
    {
    સ્થિતિ '0' :
	cmdletter = 's';
	તોડો;
    સ્થિતિ '1' :
	cmdletter = 'g';
	તોડો;
    સ્થિતિ '2' :
	cmdletter = 'd';
	તોડો;
    સ્થિતિ '3' :
	cmdletter = 'c';
	તોડો;
    સ્થિતિ '4' :
	cmdletter = 't';
	તોડો;
    સ્થિતિ '6' :
	cmdletter = 'e';
	તોડો;
    સ્થિતિ '7' :
	cmdletter = 'f';
	તોડો;
    સ્થિતિ '8' :
	cmdletter = 'i';
	તોડો;
    સ્થિતિ '9' :
	cmdletter = 'a';
	તોડો;
    મૂળભૂત :
	cmdletter = opt[0];
    }

    qfpos = (અક્ષર *)vim_strchr(p_csqf, cmdletter);
    જો (qfpos != ખાલી)
    {
	qfpos++;
	// next symbol must be + or -
	જો (strchr(CSQF_FLAGS, *qfpos) == ખાલી)
	{
	    (શૂન્ય)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    પાછા FALSE;
	}

	જો (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    જો (aborting())
		પાછા FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    જો (cmd == ખાલી)
	પાછા FALSE;

    nummatches = ALLOC_MULT(પૂર્ણાંક, csinfo_size);
    જો (nummatches == ખાલી)
    {
	vim_free(cmd);
	પાછા FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    માટે (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    માટે (i = 0; i < csinfo_size; i++)
    {
	જો (csinfo[i].fname == ખાલી || csinfo[i].to_fp == ખાલી)
	    ચાલુ;

	// send cmd to cscope
	(શૂન્ય)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(શૂન્ય)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	જો (nummatches[i] > -1)
	    totmatches += nummatches[i];

	જો (nummatches[i] == 0)
	    (શૂન્ય)cs_read_prompt(i);
    }
    vim_free(cmd);

    જો (totmatches == 0)
    {
	જો (verbose)
	    (શૂન્ય)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	પાછા FALSE;
    }

# ifdef FEAT_QUICKFIX
    જો (qfpos != ખાલી && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = ખાલી;
	win_T	    *wp = ખાલી;

	જો (tmp == ખાલી)
	{
	    vim_free(nummatches);
	    પાછા FALSE;
	}

	f = mch_fopen((અક્ષર *)tmp, "w");
	જો (f == ખાલી)
	    semsg(_(e_cant_open_file_str), tmp);
	નહીંતર
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    જો (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    જો (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, ખાલી) > 0)
	    {
		જો (postponed_split != 0)
		{
		    (શૂન્ય)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		જો (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != ખાલી)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	પાછા TRUE;
    }
    નહીંતર
# endif // FEAT_QUICKFIX
    {
	અક્ષર **matches = ખાલી, **contexts = ખાલી;
	પૂર્ણાંક matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	જો (matches == ખાલી)
	    પાછા FALSE;

	(શૂન્ય)cs_manage_matches(matches, contexts, matched, Store);

	પાછા do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    સ્થિર પૂર્ણાંક
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (શૂન્ય)સંદેશ_puts(_("cscope commands:\n"));
    જ્યારે (cmdp->name != ખાલી)
    {
	અક્ષર *help = _(cmdp->help);
	પૂર્ણાંક  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	જો (space_cnt < 0)
	    space_cnt = 0;
	(શૂન્ય)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	જો (strcmp(cmdp->name, "find") == 0)
	    સંદેશ_puts(_("\n"
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
    પાછા 0;
}


    સ્થિર શૂન્ય
clear_csinfo(પૂર્ણાંક i)
{
    csinfo[i].fname  = ખાલી;
    csinfo[i].ppath  = ખાલી;
    csinfo[i].flags  = ખાલી;
# જો defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# નહીંતર
    csinfo[i].nVolume = 0;
    csinfo[i].n_Index_ઉપલું = 0;
    csinfo[i].n_Index_નીચલું = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = ખાલી;
    csinfo[i].to_fp  = ખાલી;
# જો defined(MSWIN)
    csinfo[i].hProc = ખાલી;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    સ્થિર પૂર્ણાંક
cs_ઉમેરો_filelist(
    અક્ષર *fname,
    અક્ષર *ppath,
    અક્ષર *flags,
    stat_T *sb UNUSED)
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    પસંદગી (win32_fileinfo((char_u *)fname, &bhfi))
    {
	સ્થિતિ FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	સ્થિતિ FILEINFO_READ_FAIL:	// CreateFile() failed
	    જો (p_csverbose)
	    {
		અક્ષર *cant_સંદેશ = _(e_cannot_open_cscope_database_str);
		અક્ષર *winmsg = GetWin32Error();

		જો (winmsg != ખાલી)
		    (શૂન્ય)semsg(cant_સંદેશ, winmsg);
		નહીંતર
		    // subst filename if can't get error text
		    (શૂન્ય)semsg(cant_સંદેશ, fname);
	    }
	    પાછા -1;

	સ્થિતિ FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    જો (p_csverbose)
		(શૂન્ય)emsg(_(e_cannot_get_cscope_database_information));
	    પાછા -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    માટે (j = 0; j < csinfo_size; j++)
    {
	જો (csinfo[j].fname != ખાલી
# જો defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# નહીંતર
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_સંખ્યા
		    && csinfo[j].n_Index_ઉપલું == bhfi.n_File_Index_ઉપલું
		    && csinfo[j].n_Index_નીચલું == bhfi.n_File_Index_નીચલું))
# endif
	    )
	{
	    જો (p_csverbose)
		(શૂન્ય)emsg(_(e_duplicate_cscope_database_not_added));
	    પાછા -1;
	}

	જો (csinfo[j].fname == ખાલી && i == -1)
	    i = j; // remember first empty entry
    }

    જો (i == -1)
    {
	i = csinfo_size;
	જો (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	નહીંતર
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, કદ(csinfo_T)*csinfo_size);
	    જો (csinfo == ખાલી)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		પાછા -1;
	    }
	}
	જો (csinfo == ખાલી)
	    પાછા -1;
	માટે (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    જો ((csinfo[i].fname = alloc(strlen(fname)+1)) == ખાલી)
	પાછા -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((અચલ અક્ષર *)fname));

    જો (ppath != ખાલી)
    {
	જો ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == ખાલી)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    પાછા -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((અચલ અક્ષર *)ppath));
    }
    નહીંતર
	csinfo[i].ppath = ખાલી;

    જો (flags != ખાલી)
    {
	જો ((csinfo[i].flags = alloc(strlen(flags) + 1)) == ખાલી)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    પાછા -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((અચલ અક્ષર *)flags));
    }
    નહીંતર
	csinfo[i].flags = ખાલી;

# જો defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# નહીંતર
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_સંખ્યા;
    csinfo[i].n_Index_નીચલું = bhfi.n_File_Index_નીચલું;
    csinfo[i].n_Index_ઉપલું = bhfi.n_File_Index_ઉપલું;
# endif
    પાછા i;
}


/*
 * Find cscope command in command table.
 */
    સ્થિર cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    અક્ષર *stok;
    size_t len;

    જો (eap->arg == ખાલી)
	પાછા ખાલી;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (પૂર્ણાંક)STRLEN(eap->arg);

    જો ((stok = strtok((અક્ષર *)(eap->arg), (અચલ અક્ષર *)" ")) == ખાલી)
	પાછા ખાલી;

    len = strlen(stok);
    માટે (cmdp = cs_cmds; cmdp->name != ખાલી; ++cmdp)
    {
	જો (strncmp((અચલ અક્ષર *)(stok), cmdp->name, len) == 0)
	    પાછા (cmdp);
    }
    પાછા ખાલી;
}


/*
 * Nuke em.
 */
    સ્થિર પૂર્ણાંક
cs_kill(exarg_T *eap UNUSED)
{
    અક્ષર *stok;
    પૂર્ણાંક i;

    જો ((stok = strtok((અક્ષર *)ખાલી, (અચલ અક્ષર *)" ")) == ખાલી)
    {
	cs_usage_સંદેશ(Kill);
	પાછા CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    જો ((strlen(stok) < 2 && VIM_ISDIGIT((પૂર્ણાંક)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((પૂર્ણાંક)(stok[1]))))
	i = atoi(stok);
    નહીંતર
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	માટે (i = 0; i < csinfo_size; i++)
	{
	    જો (csinfo[i].fname != ખાલી && strstr(csinfo[i].fname, stok))
		તોડો;
	}
    }

    જો ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == ખાલી))
    {
	જો (p_csverbose)
	    (શૂન્ય)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    નહીંતર
    {
	જો (i == -1)
	{
	    માટે (i = 0; i < csinfo_size; i++)
	    {
		જો (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	નહીંતર
	    cs_kill_execute(i, stok);
    }

    પાછા 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    સ્થિર શૂન્ય
cs_kill_execute(
    પૂર્ણાંક i,	    // cscope table index
    અક્ષર *cname)    // cscope database name
{
    જો (p_csverbose)
    {
	સંદેશ_clr_eos();
	(શૂન્ય)smsg_attr(HL_ATTR(HLF_R) | સંદેશ_HIST,
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
    સ્થિર અક્ષર *
cs_make_vim_style_matches(
    અક્ષર *fname,
    અક્ષર *slno,
    અક્ષર *શોધ,
    અક્ષર *tagstr)
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
    અક્ષર *buf;
    પૂર્ણાંક amt;

    જો (શોધ != ખાલી)
    {
	amt = (પૂર્ણાંક)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(શોધ)+6);
	જો ((buf = alloc(amt)) == ખાલી)
	    પાછા ખાલી;

	(શૂન્ય)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, શોધ);
    }
    નહીંતર
    {
	amt = (પૂર્ણાંક)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	જો ((buf = alloc(amt)) == ખાલી)
	    પાછા ખાલી;

	(શૂન્ય)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    પાછા buf;
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
    સ્થિર અક્ષર *
cs_manage_matches(
    અક્ષર **matches,
    અક્ષર **contexts,
    પૂર્ણાંક totmatches,
    mcmd_e cmd)
{
    સ્થિર અક્ષર **mp = ખાલી;
    સ્થિર અક્ષર **cp = ખાલી;
    સ્થિર પૂર્ણાંક cnt = -1;
    સ્થિર પૂર્ણાંક next = -1;
    અક્ષર *p = ખાલી;

    પસંદગી (cmd)
    {
    સ્થિતિ Store:
	પ્રતિજ્ઞા(matches != ખાલી);
	પ્રતિજ્ઞા(totmatches > 0);
	જો (mp != ખાલી || cp != ખાલી)
	    (શૂન્ય)cs_manage_matches(ખાલી, ખાલી, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	તોડો;
    સ્થિતિ Get:
	જો (next >= cnt)
	    પાછા ખાલી;

	p = mp[next];
	next++;
	તોડો;
    સ્થિતિ Free:
	જો (mp != ખાલી)
	{
	    જો (cnt > 0)
		જ્યારે (cnt--)
		{
		    vim_free(mp[cnt]);
		    જો (cp != ખાલી)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = ખાલી;
	cp = ખાલી;
	cnt = 0;
	next = 0;
	તોડો;
    સ્થિતિ Print:
	cs_print_tags_priv(mp, cp, cnt);
	તોડો;
    મૂળભૂત:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	પાછા ખાલી;
    }

    પાછા p;
}


/*
 * Parse cscope output.
 */
    સ્થિર અક્ષર *
cs_parse_results(
    પૂર્ણાંક cnumber,
    અક્ષર *buf,
    પૂર્ણાંક bufsize,
    અક્ષર **context,
    અક્ષર **linenumber,
    અક્ષર **શોધ)
{
    પૂર્ણાંક ch;
    અક્ષર *p;
    અક્ષર *name;

    જો (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == ખાલી)
    {
	જો (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	પાછા ખાલી;
    }

    // If the line's too long for the buffer, discard it.
    જો ((p = strchr(buf, '\n')) == ખાલી)
    {
	જ્યારે ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	પાછા ખાલી;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    જો ((name = strtok(buf, (અચલ અક્ષર *)" ")) == ખાલી)
	પાછા ખાલી;
    જો ((*context = strtok(ખાલી, (અચલ અક્ષર *)" ")) == ખાલી)
	પાછા ખાલી;
    જો ((*linenumber = strtok(ખાલી, (અચલ અક્ષર *)" ")) == ખાલી)
	પાછા ખાલી;
    *શોધ = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    જો (strcmp(*શોધ, "<unknown>") == 0)
	*શોધ = ખાલી;

    name = cs_resolve_file(cnumber, name);
    પાછા name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    સ્થિર શૂન્ય
cs_file_results(FILE *f, પૂર્ણાંક *nummatches_a)
{
    પૂર્ણાંક i, j;
    અક્ષર *buf;
    અક્ષર *શોધ, *slno;
    અક્ષર *fullname;
    અક્ષર *cntx;
    અક્ષર *context;

    buf = alloc(CSREAD_BUFSIZE);
    જો (buf == ખાલી)
	પાછા;

    માટે (i = 0; i < csinfo_size; i++)
    {
	જો (nummatches_a[i] < 1)
	    ચાલુ;

	માટે (j = 0; j < nummatches_a[i]; j++)
	{
	   જો ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &શોધ)) == ખાલી)
	       ચાલુ;

	   context = alloc(strlen(cntx)+5);
	   જો (context == ખાલી)
	   {
	       vim_free(fullname);
	       ચાલુ;
	   }

	   જો (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   નહીંતર
	       sprintf(context, "<<%s>>", cntx);

	   જો (શોધ == ખાલી)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   નહીંતર
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, શોધ);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(શૂન્ય)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    સ્થિર શૂન્ય
cs_fill_results(
    અક્ષર *tagstr,
    પૂર્ણાંક totmatches,
    પૂર્ણાંક *nummatches_a,
    અક્ષર ***matches_p,
    અક્ષર ***cntxts_p,
    પૂર્ણાંક *matched)
{
    પૂર્ણાંક i, j;
    અક્ષર *buf;
    અક્ષર *શોધ, *slno;
    પૂર્ણાંક totsofar = 0;
    અક્ષર **matches = ખાલી;
    અક્ષર **cntxts = ખાલી;
    અક્ષર *fullname;
    અક્ષર *cntx;

    પ્રતિજ્ઞા(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    જો (buf == ખાલી)
	પાછા;

    જો ((matches = ALLOC_MULT(અક્ષર *, totmatches)) == ખાલી)
	જાઓ parse_out;
    જો ((cntxts = ALLOC_MULT(અક્ષર *, totmatches)) == ખાલી)
	જાઓ parse_out;

    માટે (i = 0; i < csinfo_size; i++)
    {
	જો (nummatches_a[i] < 1)
	    ચાલુ;

	માટે (j = 0; j < nummatches_a[i]; j++)
	{
	   જો ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &શોધ)) == ખાલી)
		ચાલુ;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  શોધ, tagstr);

	    vim_free(fullname);

	    જો (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = ખાલી;
	    નહીંતર
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (અક્ષર *)vim_strsave((char_u *)cntx);

	    જો (matches[totsofar] != ખાલી)
		totsofar++;

	} // for all matches

	(શૂન્ય)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    જો (totsofar == 0)
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
    સ્થિર અક્ષર *
cs_pathcomponents(અક્ષર *path)
{
    પૂર્ણાંક		i;
    અક્ષર	*s;

    જો (p_cspc == 0)
	પાછા path;

    s = path + strlen(path) - 1;
    માટે (i = 0; i < p_cspc; ++i)
    {
	જ્યારે (s > path)
	{
	   s--;
	   જો (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      તોડો;
	}
    }
    જો ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    પાછા s;
}

/*
 * Called from cs_manage_matches().
 */
    સ્થિર શૂન્ય
cs_print_tags_priv(અક્ષર **matches, અક્ષર **cntxts, પૂર્ણાંક અંક_matches)
{
    અક્ષર	*buf = ખાલી;
    અક્ષર	*t_buf;
    પૂર્ણાંક		bufsize = 0; // Track available bufsize
    પૂર્ણાંક		newsize = 0;
    અક્ષર	*ptag;
    અક્ષર	*fname, *lno, *extra, *tbuf;
    પૂર્ણાંક		i, idx, અંક;
    અક્ષર	*globalcntx = "GLOBAL";
    અક્ષર	*cntxformat = " <<%s>>";
    અક્ષર	*context;
    અક્ષર	*cstag_સંદેશ = _("Cscope tag: %s");
    અક્ષર	*csfmt_str = "%4d %6s  ";

    પ્રતિજ્ઞા(અંક_matches > 0);

    જો ((tbuf = alloc(strlen(matches[0]) + 1)) == ખાલી)
	પાછા;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    જો (ptag == ખાલી)
    {
	vim_free(tbuf);
	પાછા;
    }

    newsize = (પૂર્ણાંક)(strlen(cstag_સંદેશ) + strlen(ptag));
    buf = alloc(newsize);
    જો (buf != ખાલી)
    {
	bufsize = newsize;
	(શૂન્ય)sprintf(buf, cstag_સંદેશ, ptag);
	સંદેશ_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    સંદેશ_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    સંદેશ_advance(સંદેશ_col + 2);
    સંદેશ_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    અંક = 1;
    માટે (i = 0; i < અંક_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	જો ((tbuf = alloc(strlen(matches[idx]) + 1)) == ખાલી)
	    ચાલુ;
	(શૂન્ય)strcpy(tbuf, matches[idx]);

	જો (strtok(tbuf, (અચલ અક્ષર *)"\t") == ખાલી
		|| (fname = strtok(ખાલી, (અચલ અક્ષર *)"\t")) == ખાલી
		|| (lno = strtok(ખાલી, (અચલ અક્ષર *)"\t")) == ખાલી)
	{
	    vim_free(tbuf);
	    ચાલુ;
	}
	extra = strtok(ખાલી, (અચલ અક્ષર *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (પૂર્ણાંક)(strlen(csfmt_str) + 16 + strlen(lno));
	જો (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    જો (buf == ખાલી)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    નહીંતર
		bufsize = newsize;
	}
	જો (buf != ખાલી)
	{
	    // csfmt_str = "%4d %6s  ";
	    (શૂન્ય)sprintf(buf, csfmt_str, અંક, lno);
	    સંદેશ_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	સંદેશ_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	જો (cntxts[idx] != ખાલી)
	    context = cntxts[idx];
	નહીંતર
	    context = globalcntx;
	newsize = (પૂર્ણાંક)(strlen(context) + strlen(cntxformat));

	જો (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    જો (buf == ખાલી)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    નહીંતર
		bufsize = newsize;
	}
	જો (buf != ખાલી)
	{
	    (શૂન્ય)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    જો (સંદેશ_col + (પૂર્ણાંક)strlen(buf) >= cmdline_width)
		સંદેશ_putchar('\n');
	    સંદેશ_advance(12);
	    સંદેશ_outtrans_long_attr((char_u *)buf, 0);
	    સંદેશ_putchar('\n');
	}
	જો (extra != ખાલી)
	{
	    સંદેશ_advance(13);
	    સંદેશ_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	જો (સંદેશ_col)
	    સંદેશ_putchar('\n');

	ui_breakcheck();
	જો (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    તોડો;
	}

	અંક++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    સ્થિર પૂર્ણાંક
cs_read_prompt(પૂર્ણાંક i)
{
    પૂર્ણાંક		ch;
    અક્ષર	*buf = ખાલી; // buffer for possible error message from cscope
    પૂર્ણાંક		bufpos = 0;
    પૂર્ણાંક		maxlen;
    સ્થિર અક્ષર *eprompt = "Press the RETURN key to continue:";
    પૂર્ણાંક		epromptlen = (પૂર્ણાંક)strlen(eprompt);
    પૂર્ણાંક		n;

    // compute maximum allowed len for Cscope error message
    અક્ષર *cs_emsg = _(e_cscope_error_str);
    maxlen = (પૂર્ણાંક)(IOSIZE - strlen(cs_emsg));

    માટે (;;)
    {
	જ્યારે ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    જો (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		જો (buf == ખાલી) // lazy buffer allocation
		    buf = alloc(maxlen);
		જો (buf != ખાલી)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    જો (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(શૂન્ય)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(શૂન્ય)putc('\n', csinfo[i].to_fp);
			(શૂન્ય)fflush(csinfo[i].to_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	માટે (n = 0; n < (પૂર્ણાંક)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    જો (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    જો (ch == EOF)
	    {
		જો (buf != ખાલી && buf[0] != NUL)
		    (શૂન્ય)semsg(cs_emsg, buf);
		નહીંતર જો (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		પાછા CSCOPE_FAILURE;
	    }

	    જો (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		તોડો;
	    }
	}

	જો (ch == EOF)
	    ચાલુ;	    // didn't find the prompt
	તોડો;		    // did find the prompt
    }

    vim_free(buf);
    પાછા CSCOPE_SUCCESS;
}

# જો defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    સ્થિર શૂન્ય
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    સ્થિર શૂન્ય
cs_release_csp(પૂર્ણાંક i, પૂર્ણાંક freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    જો (csinfo[i].to_fp != ખાલી)
    {
	(શૂન્ય)fputs("q\n", csinfo[i].to_fp);
	(શૂન્ય)fflush(csinfo[i].to_fp);
    }
# જો defined(UNIX)
    {
	પૂર્ણાંક waitpid_errno;
	પૂર્ણાંક pstat;
	pid_t pid;

#  જો defined(HAVE_SIGACTION)
	સંરચના sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   નહીંતર
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, ખાલી);
#  નહીંતર
	પૂર્ણાંક waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	માટે (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    જો (pid != 0)
		તોડો;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	જો (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    પૂર્ણાંક alive = TRUE;

	    જો (waitpid_errno == ECHILD)
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
		પૂર્ણાંક waited;

		sleep(0);
		માટે (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    જો (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			તોડો;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    જો (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(શૂન્ય)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# નહીંતર  // !UNIX
    જો (csinfo[i].hProc != ખાલી)
    {
	// Give cscope a chance to exit normally
	જો (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    જો (csinfo[i].fr_fp != ખાલી)
	(શૂન્ય)fclose(csinfo[i].fr_fp);
    જો (csinfo[i].to_fp != ખાલી)
	(શૂન્ય)fclose(csinfo[i].to_fp);

    જો (freefnpp)
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
    સ્થિર પૂર્ણાંક
cs_reset(exarg_T *eap UNUSED)
{
    અક્ષર	**dblist = ખાલી, **pplist = ખાલી, **fllist = ખાલી;
    પૂર્ણાંક	i;
    અક્ષર buf[20]; // for sprintf " (#%d)"

    જો (csinfo_size == 0)
	પાછા CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(અક્ષર *, csinfo_size);
    pplist = ALLOC_MULT(અક્ષર *, csinfo_size);
    fllist = ALLOC_MULT(અક્ષર *, csinfo_size);
    જો (dblist == ખાલી || pplist == ખાલી || fllist == ખાલી)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	પાછા CSCOPE_FAILURE;
    }

    માટે (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	જો (csinfo[i].fname != ખાલી)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    માટે (i = 0; i < csinfo_size; i++)
    {
	જો (dblist[i] != ખાલી)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    જો (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		સંદેશ_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    જો (p_csverbose)
	સંદેશ_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | સંદેશ_HIST);
    પાછા CSCOPE_SUCCESS;
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
    સ્થિર અક્ષર *
cs_resolve_file(પૂર્ણાંક i, અક્ષર *name)
{
    અક્ષર	*fullname;
    string_T	csdir = {ખાલી, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    પૂર્ણાંક		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (પૂર્ણાંક)namelen + 2;
    જો (csinfo[i].ppath != ખાલી)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (પૂર્ણાંક)ppathlen;
    }
    નહીંતર જો (p_csre && csinfo[i].fname != ખાલી)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	જો (csdir.string != ખાલી)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (પૂર્ણાંક)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    જો (csinfo[i].ppath != ખાલી
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	જો ((fullname = alloc(len)) != ખાલી)
	    (શૂન્ય)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    નહીંતર જો (csdir.string != ખાલી && csinfo[i].fname != ખાલી && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (અક્ષર *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    નહીંતર
    {
	fullname = (અક્ષર *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    પાછા fullname;
}


/*
 * Show all cscope connections.
 */
    સ્થિર પૂર્ણાંક
cs_show(exarg_T *eap UNUSED)
{
    પૂર્ણાંક i;

    જો (cs_cnt_connections() == 0)
	સંદેશ_puts(_("no cscope connections\n"));
    નહીંતર
    {
	સંદેશ_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	માટે (i = 0; i < csinfo_size; i++)
	{
	    જો (csinfo[i].fname == ખાલી)
		ચાલુ;

	    જો (csinfo[i].ppath != ખાલી)
		(શૂન્ય)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    નહીંતર
		(શૂન્ય)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    પાછા CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    શૂન્ય
cs_end(શૂન્ય)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#જો defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    શૂન્ય
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    પૂર્ણાંક		અંક = 0;
    char_u	*dbpath = ખાલી;
    char_u	*prepend = ખાલી;
    char_u	buf[NUMBUFLEN];

    જો (in_vim9script()
	    && (check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	અંક = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	જો (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_સંખ્યા = cs_connection(અંક, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
