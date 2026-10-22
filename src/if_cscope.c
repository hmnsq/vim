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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# যদি defined(UNIX)
#  include <sys/wait.h>
# endif

# যদি defined (MSWIN)
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

প্রকারলেখ কাঠামো {
    অক্ষর *  name;
    পূর্ণসংখ্যা     (*func)(exarg_T *eap);
    অক্ষর *  help;
    অক্ষর *  usage;
    পূর্ণসংখ্যা	    cansplit;		// if supports splitting window
} cscmd_T;

প্রকারলেখ কাঠামো csi {
    অক্ষর *	    fname;	// cscope db name
    অক্ষর *	    ppath;	// path to prepend (the -P option)
    অক্ষর *	    flags;	// additional cscope flags/options (e.g, -p2)
# যদি defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# অন্যথা
#  যদি defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_সূচক_উপরের;	// st_ino has no meaning in the Windows
    DWORD	    n_সূচক_নিচের;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    to_fp;	// to cscope: FILE.
} csinfo_T;

প্রকারলেখ গণনা { Add, Find, Help, Kill, Reset, Show } csid_e;

প্রকারলেখ গণনা {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

স্থির পূর্ণসংখ্যা	    cs_add(exarg_T *eap);
স্থির পূর্ণসংখ্যা	    cs_add_common(অক্ষর *, অক্ষর *, অক্ষর *);
স্থির পূর্ণসংখ্যা	    cs_check_for_connections(শূন্য);
স্থির পূর্ণসংখ্যা	    cs_check_for_tags(শূন্য);
স্থির পূর্ণসংখ্যা	    cs_cnt_connections(শূন্য);
স্থির পূর্ণসংখ্যা	    cs_create_connection(পূর্ণসংখ্যা i);
# ifdef FEAT_QUICKFIX
স্থির শূন্য	    cs_file_results(FILE *, পূর্ণসংখ্যা *);
# endif
স্থির শূন্য	    cs_fill_results(অক্ষর *, পূর্ণসংখ্যা , পূর্ণসংখ্যা *, অক্ষর ***,
			অক্ষর ***, পূর্ণসংখ্যা *);
স্থির পূর্ণসংখ্যা	    cs_find(exarg_T *eap);
স্থির পূর্ণসংখ্যা	    cs_find_common(অক্ষর *opt, অক্ষর *pat, পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা, char_u *cmdline);
স্থির পূর্ণসংখ্যা	    cs_help(exarg_T *eap);
স্থির পূর্ণসংখ্যা	    cs_ঢোকাও_filelist(অক্ষর *, অক্ষর *, অক্ষর *,
			stat_T *);
স্থির পূর্ণসংখ্যা	    cs_kill(exarg_T *eap);
স্থির শূন্য	    cs_kill_execute(পূর্ণসংখ্যা, অক্ষর *);
স্থির cscmd_T *    cs_lookup_cmd(exarg_T *eap);
স্থির অক্ষর *	    cs_make_vim_style_matches(অক্ষর *, অক্ষর *,
			অক্ষর *, অক্ষর *);
স্থির অক্ষর *	    cs_manage_matches(অক্ষর **, অক্ষর **, পূর্ণসংখ্যা, mcmd_e);
স্থির শূন্য	    cs_print_tags_priv(অক্ষর **, অক্ষর **, পূর্ণসংখ্যা);
স্থির পূর্ণসংখ্যা	    cs_read_prompt(পূর্ণসংখ্যা);
স্থির শূন্য	    cs_release_csp(পূর্ণসংখ্যা, পূর্ণসংখ্যা freefnpp);
স্থির পূর্ণসংখ্যা	    cs_reset(exarg_T *eap);
স্থির অক্ষর *	    cs_resolve_file(পূর্ণসংখ্যা, অক্ষর *);
স্থির পূর্ণসংখ্যা	    cs_show(exarg_T *eap);


স্থির csinfo_T *   csinfo = ফাঁকা;
স্থির পূর্ণসংখ্যা	    csinfo_আকার = 0;	// number of items allocated in
					// csinfo[]

স্থির পূর্ণসংখ্যা	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
স্থির cscmd_T	    cs_cmds[] =
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
    { ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, 0 }
};

    স্থির শূন্য
cs_usage_বার্তা(csid_e x)
{
    (শূন্য)semsg(_(e_usage_cscope_str), cs_cmds[(পূর্ণসংখ্যা)x].usage);
}

স্থির গণনা
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
get_cscope_name(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    পূর্ণসংখ্যা		current_idx;
    পূর্ণসংখ্যা		i;

    নির্বাচন (expand_what)
    {
    ক্ষেত্র EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	ফেরত (char_u *)cs_cmds[idx].name;
    ক্ষেত্র EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	জন্য (i = 0, current_idx = 0; cs_cmds[i].name != ফাঁকা; i++)
	    যদি (cs_cmds[i].cansplit)
		যদি (current_idx++ == idx)
		    বিরতি;
	ফেরত (char_u *)cs_cmds[i].name;
    ক্ষেত্র EXP_CSCOPE_FIND:
	{
	    ধ্রুবক অক্ষর *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", ফাঁকা
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    ফেরত (char_u *)query_type[idx];
	}
    ক্ষেত্র EXP_CSCOPE_KILL:
	{
	    স্থির অক্ষর	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    জন্য (i = 0, current_idx = 0; i < csinfo_আকার; i++)
	    {
		যদি (csinfo[i].fname == ফাঁকা)
		    চালু;
		যদি (current_idx++ == idx)
		{
		    vim_snprintf(connection, আকার(connection), "%d", i);
		    ফেরত (char_u *)connection;
		}
	    }
	    ফেরত (current_idx == idx && idx > 0) ? (char_u *)"-1" : ফাঁকা;
	}
    পূর্বনির্ধারিত:
	ফেরত ফাঁকা;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    শূন্য
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

    যদি (*arg == NUL)
	ফেরত;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    যদি (*p == NUL)
	ফেরত;

    // past first word
    xp->xp_pattern = skipwhite(p);
    যদি (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    অন্যথা যদি (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    অন্যথা যদি (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    অন্যথা যদি (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    অন্যথা
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    স্থির শূন্য
do_cscope_general(
    exarg_T	*eap,
    পূর্ণসংখ্যা		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    যদি (check_restricted())
	ফেরত;

    যদি ((cmdp = cs_lookup_cmd(eap)) == ফাঁকা)
    {
	cs_help(eap);
	ফেরত;
    }

    যদি (make_split)
    {
	যদি (!cmdp->cansplit)
	{
	    (শূন্য)বার্তা_puts(_("This cscope command does not support splitting the window.\n"));
	    ফেরত;
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
    শূন্য
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    শূন্য
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    শূন্য
ex_cstag(exarg_T *eap)
{
    পূর্ণসংখ্যা ret = FALSE;

    যদি (*eap->arg == NUL)
    {
	(শূন্য)emsg(_(e_usage_cstag_ident));
	ফেরত;
    }

    নির্বাচন (p_csto)
    {
    ক্ষেত্র 0 :
	যদি (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (অক্ষর *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    যদি (ret == FALSE)
	    {
		cs_free_tags();
		যদি (বার্তা_col)
		    বার্তা_putchar('\n');

		যদি (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	অন্যথা যদি (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	বিরতি;
    ক্ষেত্র 1 :
	যদি (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    যদি (ret == FALSE)
	    {
		যদি (বার্তা_col)
		    বার্তা_putchar('\n');

		যদি (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (অক্ষর *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    যদি (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	অন্যথা যদি (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (অক্ষর *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    যদি (ret == FALSE)
		cs_free_tags();
	}
	বিরতি;
    পূর্বনির্ধারিত :
	বিরতি;
    }

    যদি (!ret)
    {
	(শূন্য)emsg(_(e_cstag_tag_not_founc));
# যদি defined(FEAT_QUICKFIX)
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
    পূর্ণসংখ্যা
cs_fgets(char_u *buf, পূর্ণসংখ্যা আকার_2)
{
    অক্ষর *p;

    যদি ((p = cs_manage_matches(ফাঁকা, ফাঁকা, -1, Get)) == ফাঁকা)
	ফেরত TRUE;
    vim_strncpy(buf, (char_u *)p, আকার_2 - 1);

    ফেরত FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    শূন্য
cs_free_tags(শূন্য)
{
    cs_manage_matches(ফাঁকা, ফাঁকা, -1, Free);
}


/*
 * Called from do_tag().
 */
    শূন্য
cs_print_tags(শূন্য)
{
    cs_manage_matches(ফাঁকা, ফাঁকা, -1, Print);
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
# যদি defined(FEAT_EVAL)
    স্থির পূর্ণসংখ্যা
cs_connection(পূর্ণসংখ্যা অঙ্ক, char_u *dbpath, char_u *ppath)
{
    পূর্ণসংখ্যা i;

    যদি (অঙ্ক < 0 || অঙ্ক > 4 || (অঙ্ক > 0 && !dbpath))
	ফেরত FALSE;

    জন্য (i = 0; i < csinfo_আকার; i++)
    {
	যদি (!csinfo[i].fname)
	    চালু;

	যদি (অঙ্ক == 0)
	    ফেরত TRUE;

	নির্বাচন (অঙ্ক)
	{
	ক্ষেত্র 1:
	    যদি (strstr(csinfo[i].fname, (অক্ষর *)dbpath))
		ফেরত TRUE;
	    বিরতি;
	ক্ষেত্র 2:
	    যদি (strcmp(csinfo[i].fname, (অক্ষর *)dbpath) == 0)
		ফেরত TRUE;
	    বিরতি;
	ক্ষেত্র 3:
	    যদি (strstr(csinfo[i].fname, (অক্ষর *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (অক্ষর *)ppath))))
		ফেরত TRUE;
	    বিরতি;
	ক্ষেত্র 4:
	    যদি ((strcmp(csinfo[i].fname, (অক্ষর *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (অক্ষর *)ppath) == 0))))
		ফেরত TRUE;
	    বিরতি;
	}
    }

    ফেরত FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    স্থির পূর্ণসংখ্যা
cs_add(exarg_T *eap UNUSED)
{
    অক্ষর *fname, *ppath, *flags = ফাঁকা;

    যদি ((fname = strtok((অক্ষর *)ফাঁকা, (ধ্রুবক অক্ষর *)" ")) == ফাঁকা)
    {
	cs_usage_বার্তা(Add);
	ফেরত CSCOPE_FAILURE;
    }
    যদি ((ppath = strtok((অক্ষর *)ফাঁকা, (ধ্রুবক অক্ষর *)" ")) != ফাঁকা)
	flags = strtok((অক্ষর *)ফাঁকা, (ধ্রুবক অক্ষর *)" ");

    ফেরত cs_add_common(fname, ppath, flags);
}

    স্থির শূন্য
cs_stat_emsg(অক্ষর *fname)
{
    পূর্ণসংখ্যা err = errno;
    (শূন্য)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    স্থির পূর্ণসংখ্যা
cs_add_common(
    অক্ষর *arg1,	    // filename - may contain environment variables
    অক্ষর *arg2,	    // prepend path - may contain environment variables
    অক্ষর *flags)
{
    stat_T	statbuf;
    পূর্ণসংখ্যা		ret;
    অক্ষর	*fname = ফাঁকা;
    অক্ষর	*fname2 = ফাঁকা;
    অক্ষর	*ppath = ফাঁকা;
    পূর্ণসংখ্যা		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = ফাঁকা;

    // get the filename (arg1), expand it, and try to stat it
    যদি ((fname = alloc(MAXPATHL + 1)) == ফাঁকা)
	যাও add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (শূন্য)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    যদি (fname == ফাঁকা)
	যাও add_err;
    fname = (অক্ষর *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    যদি (ret < 0)
    {
staterr:
	যদি (p_csverbose)
	    cs_stat_emsg(fname);
	যাও add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    যদি (arg2 != ফাঁকা)
    {
	stat_T	    statbuf2;

	যদি ((ppath = alloc(MAXPATHL + 1)) == ফাঁকা)
	    যাও add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	যদি (ret < 0)
	    যাও staterr;
    }

    // if filename is a directory, append the cscope database name to it
    যদি (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	যদি (fname2 == ফাঁকা)
	    যাও add_err;

	যখন (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    যদি (fname[0] == '\0')
		বিরতি;
	}
	যদি (fname[0] == '\0')
	    (শূন্য)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	অন্যথা
	    (শূন্য)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	যদি (ret < 0)
	{
	    যদি (p_csverbose)
		cs_stat_emsg(fname2);
	    যাও add_err;
	}

	i = cs_ঢোকাও_filelist(fname2, ppath, flags, &statbuf);
    }
    অন্যথা যদি (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_ঢোকাও_filelist(fname, ppath, flags, &statbuf);
    }
    অন্যথা
    {
	যদি (p_csverbose)
	    (শূন্য)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	যাও add_err;
    }

    যদি (i != -1)
    {
	যদি (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    যাও add_err;
	}

	যদি (p_csverbose)
	{
	    বার্তা_clr_eos();
	    (শূন্য)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    ফেরত CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    ফেরত CSCOPE_FAILURE;
}


    স্থির পূর্ণসংখ্যা
cs_check_for_connections(শূন্য)
{
    ফেরত (cs_cnt_connections() > 0);
}


    স্থির পূর্ণসংখ্যা
cs_check_for_tags(শূন্য)
{
    ফেরত (p_tags[0] != NUL && curbuf->b_p_tags != ফাঁকা);
}


/*
 * Count the number of cscope connections.
 */
    স্থির পূর্ণসংখ্যা
cs_cnt_connections(শূন্য)
{
    পূর্ণসংখ্যা i;
    পূর্ণসংখ্যা cnt = 0;

    জন্য (i = 0; i < csinfo_আকার; i++)
	যদি (csinfo[i].fname != ফাঁকা)
	    cnt++;
    ফেরত cnt;
}

    স্থির শূন্য
cs_reading_emsg(
    পূর্ণসংখ্যা idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    স্থির পূর্ণসংখ্যা
cs_cnt_matches(পূর্ণসংখ্যা idx)
{
    অক্ষর *stok;
    অক্ষর *buf;
    পূর্ণসংখ্যা nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    যদি (buf == ফাঁকা)
	ফেরত 0;
    জন্য (;;)
    {
	যদি (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    যদি (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    ফেরত -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	যদি (strstr((ধ্রুবক অক্ষর *)buf, "Unable to search database") != ফাঁকা)
	    বিরতি;
	যদি ((stok = strtok(buf, (ধ্রুবক অক্ষর *)" ")) == ফাঁকা)
	    চালু;
	যদি (strstr((ধ্রুবক অক্ষর *)stok, "cscope:") == ফাঁকা)
	    চালু;

	যদি ((stok = strtok(ফাঁকা, (ধ্রুবক অক্ষর *)" ")) == ফাঁকা)
	    চালু;
	nlines = atoi(stok);
	যদি (nlines < 0)
	{
	    nlines = 0;
	    বিরতি;
	}

	যদি ((stok = strtok(ফাঁকা, (ধ্রুবক অক্ষর *)" ")) == ফাঁকা)
	    চালু;
	যদি (strncmp((ধ্রুবক অক্ষর *)stok, "lines", 5))
	    চালু;

	বিরতি;
    }

    vim_free(buf);
    ফেরত nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    স্থির অক্ষর *
cs_create_cmd(অক্ষর *csoption, অক্ষর *pattern)
{
    অক্ষর *cmd;
    short অনুসন্ধান;
    অক্ষর *pat;

    নির্বাচন (csoption[0])
    {
    ক্ষেত্র '0' : ক্ষেত্র 's' :
	অনুসন্ধান = 0;
	বিরতি;
    ক্ষেত্র '1' : ক্ষেত্র 'g' :
	অনুসন্ধান = 1;
	বিরতি;
    ক্ষেত্র '2' : ক্ষেত্র 'd' :
	অনুসন্ধান = 2;
	বিরতি;
    ক্ষেত্র '3' : ক্ষেত্র 'c' :
	অনুসন্ধান = 3;
	বিরতি;
    ক্ষেত্র '4' : ক্ষেত্র 't' :
	অনুসন্ধান = 4;
	বিরতি;
    ক্ষেত্র '6' : ক্ষেত্র 'e' :
	অনুসন্ধান = 6;
	বিরতি;
    ক্ষেত্র '7' : ক্ষেত্র 'f' :
	অনুসন্ধান = 7;
	বিরতি;
    ক্ষেত্র '8' : ক্ষেত্র 'i' :
	অনুসন্ধান = 8;
	বিরতি;
    ক্ষেত্র '9' : ক্ষেত্র 'a' :
	অনুসন্ধান = 9;
	বিরতি;
    পূর্বনির্ধারিত :
	(শূন্য)emsg(_(e_unknown_cscope_অনুসন্ধান_type));
	cs_usage_বার্তা(Find);
	ফেরত ফাঁকা;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    যদি (অনুসন্ধান != 4 && অনুসন্ধান != 6)
	যখন VIM_ISWHITE(*pat)
	    ++pat;

    যদি ((cmd = alloc(strlen(pat) + 2)) == ফাঁকা)
	ফেরত ফাঁকা;

    (শূন্য)sprintf(cmd, "%d%s", অনুসন্ধান, pat);

    ফেরত cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    স্থির পূর্ণসংখ্যা
cs_create_connection(পূর্ণসংখ্যা i)
{
# ifdef UNIX
    পূর্ণসংখ্যা		to_cs[2], from_cs[2];
# endif
    পূর্ণসংখ্যা		cmdlen;
    অক্ষর	*prog, *cmd, *ppath = ফাঁকা;
    size_t	proglen;
# ifdef MSWIN
    পূর্ণসংখ্যা		fd;
    পূর্ণসংখ্যা		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# যদি defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    যদি (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(শূন্য)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	যদি (to_cs[0] != -1)
	    (শূন্য)close(to_cs[0]);
	যদি (to_cs[1] != -1)
	    (শূন্য)close(to_cs[1]);
	যদি (from_cs[0] != -1)
	    (শূন্য)close(from_cs[0]);
	যদি (from_cs[1] != -1)
	    (শূন্য)close(from_cs[1]);
	ফেরত CSCOPE_FAILURE;
    }

    যদি ((csinfo[i].pid = fork()) == -1)
    {
	(শূন্য)emsg(_(e_could_not_fork_for_cscope));
	যাও err_closing;
    }
    অন্যথা যদি (csinfo[i].pid == 0)	// child: run cscope.
    {
	অক্ষর **argv = ফাঁকা;

	যদি (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	যদি (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	যদি (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(শূন্য)close(to_cs[1]);
	(শূন্য)close(from_cs[0]);
# অন্যথা
	// MSWIN
	// Create pipes to communicate with cscope
	sa.n_দৈর্ঘ্য = আকার(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ফাঁকা;

	যদি (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (শূন্য)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    যদি (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    যদি (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    ফেরত CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	যদি ((prog = alloc(MAXPATHL + 1)) == ফাঁকা)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# অন্যথা
	    // MSWIN
	    যাও err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (পূর্ণসংখ্যা)(proglen + strlen(csinfo[i].fname) + 32);
	যদি (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    যদি ((ppath = alloc(MAXPATHL + 1)) == ফাঁকা)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# অন্যথা
		// MSWIN
		যাও err_closing;
# endif
	    }
	    cmdlen += (পূর্ণসংখ্যা)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	যদি (csinfo[i].flags)
	    cmdlen += (পূর্ণসংখ্যা)strlen(csinfo[i].flags);

	যদি ((cmd = alloc(cmdlen)) == ফাঁকা)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# অন্যথা
	    // MSWIN
	    যাও err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    অক্ষর	**tok = ফাঁকা;
	    পূর্ণসংখ্যা		tokc = 0;

	    ga_init2(&ga_argv, আকার(অক্ষর *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    যদি (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    জন্য (পূর্ণসংখ্যা k = 0; k < tokc; ++k)
	    {
		যদি (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((অক্ষর **)ga_argv.ga_তথ্য)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    যদি (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((অক্ষর **)ga_argv.ga_তথ্য)[ga_argv.ga_len++] =
				    (অক্ষর *)vim_strsave((char_u *)"-dl");
	    ((অক্ষর **)ga_argv.ga_তথ্য)[ga_argv.ga_len++] =
				    (অক্ষর *)vim_strsave((char_u *)"-f");
	    ((অক্ষর **)ga_argv.ga_তথ্য)[ga_argv.ga_len++] =
				(অক্ষর *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    যদি (ppath != ফাঁকা)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		অক্ষর	*parg = alloc(plen);

		যদি (parg == ফাঁকা || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((অক্ষর **)ga_argv.ga_তথ্য)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    যদি (csinfo[i].flags != ফাঁকা)
	    {
		যদি (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		জন্য (পূর্ণসংখ্যা k = 0; k < tokc; ++k)
		{
		    যদি (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((অক্ষর **)ga_argv.ga_তথ্য)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    যদি (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((অক্ষর **)ga_argv.ga_তথ্য)[ga_argv.ga_len] = ফাঁকা;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  যদি defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   যদি defined(HAVE_SETSID)
	    (শূন্য)setsid();
#   অন্যথা
	    যদি (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (অক্ষর **)ga_argv.ga_তথ্য;
	    যদি (argv[0] == ফাঁকা || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    অন্যথা	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	যদি ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == ফাঁকা)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	যদি ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == ফাঁকা)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(শূন্য)close(to_cs[0]);
	(শূন্য)close(from_cs[1]);
    }
# অন্যথা
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    যদি (csinfo[i].ppath != ফাঁকা)
    {
	len = (পূর্ণসংখ্যা)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    যদি (csinfo[i].flags != ফাঁকা)
    {
	len = (পূর্ণসংখ্যা)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_প্রবেশ  = stdin_rd;
    created = CreateProcess(ফাঁকা, cmd, ফাঁকা, ফাঁকা, TRUE, CREATE_NEW_CONSOLE,
							ফাঁকা, ফাঁকা, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    যদি (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(শূন্য)emsg(_(e_could_not_spawn_cscope_process));
	যাও err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    যদি (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == ফাঁকা))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    যদি (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == ফাঁকা))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    ফেরত CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    স্থির পূর্ণসংখ্যা
cs_find(exarg_T *eap)
{
    অক্ষর *opt, *pat;
    পূর্ণসংখ্যা i;

    যদি (cs_check_for_connections() == FALSE)
    {
	(শূন্য)emsg(_(e_no_cscope_connections));
	ফেরত FALSE;
    }

    যদি ((opt = strtok((অক্ষর *)ফাঁকা, (ধ্রুবক অক্ষর *)" ")) == ফাঁকা)
    {
	cs_usage_বার্তা(Find);
	ফেরত FALSE;
    }

    pat = opt + strlen(opt) + 1;
    যদি (pat >= (অক্ষর *)eap->arg + eap_arg_len)
    {
	cs_usage_বার্তা(Find);
	ফেরত FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    জন্য (i = 0; i < eap_arg_len; ++i)
	যদি (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    ফেরত cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    স্থির পূর্ণসংখ্যা
cs_find_common(
    অক্ষর *opt,
    অক্ষর *pat,
    পূর্ণসংখ্যা forceit,
    পূর্ণসংখ্যা verbose,
    পূর্ণসংখ্যা	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    পূর্ণসংখ্যা i;
    অক্ষর *cmd;
    পূর্ণসংখ্যা *nummatches;
    পূর্ণসংখ্যা totmatches;
# ifdef FEAT_QUICKFIX
    অক্ষর cmdletter;
    অক্ষর *qfpos;

    // get cmd letter
    নির্বাচন (opt[0])
    {
    ক্ষেত্র '0' :
	cmdletter = 's';
	বিরতি;
    ক্ষেত্র '1' :
	cmdletter = 'g';
	বিরতি;
    ক্ষেত্র '2' :
	cmdletter = 'd';
	বিরতি;
    ক্ষেত্র '3' :
	cmdletter = 'c';
	বিরতি;
    ক্ষেত্র '4' :
	cmdletter = 't';
	বিরতি;
    ক্ষেত্র '6' :
	cmdletter = 'e';
	বিরতি;
    ক্ষেত্র '7' :
	cmdletter = 'f';
	বিরতি;
    ক্ষেত্র '8' :
	cmdletter = 'i';
	বিরতি;
    ক্ষেত্র '9' :
	cmdletter = 'a';
	বিরতি;
    পূর্বনির্ধারিত :
	cmdletter = opt[0];
    }

    qfpos = (অক্ষর *)vim_strchr(p_csqf, cmdletter);
    যদি (qfpos != ফাঁকা)
    {
	qfpos++;
	// next symbol must be + or -
	যদি (strchr(CSQF_FLAGS, *qfpos) == ফাঁকা)
	{
	    (শূন্য)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    ফেরত FALSE;
	}

	যদি (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    যদি (aborting())
		ফেরত FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    যদি (cmd == ফাঁকা)
	ফেরত FALSE;

    nummatches = ALLOC_MULT(পূর্ণসংখ্যা, csinfo_আকার);
    যদি (nummatches == ফাঁকা)
    {
	vim_free(cmd);
	ফেরত FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    জন্য (i = 0; i < csinfo_আকার; i++)
	nummatches[i] = 0;
    totmatches = 0;
    জন্য (i = 0; i < csinfo_আকার; i++)
    {
	যদি (csinfo[i].fname == ফাঁকা || csinfo[i].to_fp == ফাঁকা)
	    চালু;

	// send cmd to cscope
	(শূন্য)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(শূন্য)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	যদি (nummatches[i] > -1)
	    totmatches += nummatches[i];

	যদি (nummatches[i] == 0)
	    (শূন্য)cs_read_prompt(i);
    }
    vim_free(cmd);

    যদি (totmatches == 0)
    {
	যদি (verbose)
	    (শূন্য)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	ফেরত FALSE;
    }

# ifdef FEAT_QUICKFIX
    যদি (qfpos != ফাঁকা && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = ফাঁকা;
	win_T	    *wp = ফাঁকা;

	যদি (tmp == ফাঁকা)
	{
	    vim_free(nummatches);
	    ফেরত FALSE;
	}

	f = mch_fopen((অক্ষর *)tmp, "w");
	যদি (f == ফাঁকা)
	    semsg(_(e_cant_open_file_str), tmp);
	অন্যথা
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    যদি (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    যদি (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, ফাঁকা) > 0)
	    {
		যদি (postponed_split != 0)
		{
		    (শূন্য)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		যদি (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_বাফার) && wp->w_llist_ref != ফাঁকা)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	ফেরত TRUE;
    }
    অন্যথা
# endif // FEAT_QUICKFIX
    {
	অক্ষর **matches = ফাঁকা, **contexts = ফাঁকা;
	পূর্ণসংখ্যা matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	যদি (matches == ফাঁকা)
	    ফেরত FALSE;

	(শূন্য)cs_manage_matches(matches, contexts, matched, Store);

	ফেরত do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    স্থির পূর্ণসংখ্যা
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (শূন্য)বার্তা_puts(_("cscope commands:\n"));
    যখন (cmdp->name != ফাঁকা)
    {
	অক্ষর *help = _(cmdp->help);
	পূর্ণসংখ্যা  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	যদি (space_cnt < 0)
	    space_cnt = 0;
	(শূন্য)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	যদি (strcmp(cmdp->name, "find") == 0)
	    বার্তা_puts(_("\n"
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
    ফেরত 0;
}


    স্থির শূন্য
clear_csinfo(পূর্ণসংখ্যা i)
{
    csinfo[i].fname  = ফাঁকা;
    csinfo[i].ppath  = ফাঁকা;
    csinfo[i].flags  = ফাঁকা;
# যদি defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# অন্যথা
    csinfo[i].nVolume = 0;
    csinfo[i].n_সূচক_উপরের = 0;
    csinfo[i].n_সূচক_নিচের = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = ফাঁকা;
    csinfo[i].to_fp  = ফাঁকা;
# যদি defined(MSWIN)
    csinfo[i].hProc = ফাঁকা;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    স্থির পূর্ণসংখ্যা
cs_ঢোকাও_filelist(
    অক্ষর *fname,
    অক্ষর *ppath,
    অক্ষর *flags,
    stat_T *sb UNUSED)
{
    পূর্ণসংখ্যা	    i;
    পূর্ণসংখ্যা	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    নির্বাচন (win32_fileinfo((char_u *)fname, &bhfi))
    {
	ক্ষেত্র FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	ক্ষেত্র FILEINFO_READ_FAIL:	// CreateFile() failed
	    যদি (p_csverbose)
	    {
		অক্ষর *cant_বার্তা = _(e_cannot_open_cscope_database_str);
		অক্ষর *winmsg = GetWin32Error();

		যদি (winmsg != ফাঁকা)
		    (শূন্য)semsg(cant_বার্তা, winmsg);
		অন্যথা
		    // subst filename if can't get error text
		    (শূন্য)semsg(cant_বার্তা, fname);
	    }
	    ফেরত -1;

	ক্ষেত্র FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    যদি (p_csverbose)
		(শূন্য)emsg(_(e_cannot_get_cscope_database_information));
	    ফেরত -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    জন্য (j = 0; j < csinfo_আকার; j++)
    {
	যদি (csinfo[j].fname != ফাঁকা
# যদি defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# অন্যথা
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_সংখ্যা
		    && csinfo[j].n_সূচক_উপরের == bhfi.n_File_সূচক_উপরের
		    && csinfo[j].n_সূচক_নিচের == bhfi.n_File_সূচক_নিচের))
# endif
	    )
	{
	    যদি (p_csverbose)
		(শূন্য)emsg(_(e_duplicate_cscope_database_not_added));
	    ফেরত -1;
	}

	যদি (csinfo[j].fname == ফাঁকা && i == -1)
	    i = j; // remember first empty entry
    }

    যদি (i == -1)
    {
	i = csinfo_আকার;
	যদি (csinfo_আকার == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_আকার = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	অন্যথা
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_আকার *= 2;
	    csinfo = vim_realloc(csinfo, আকার(csinfo_T)*csinfo_আকার);
	    যদি (csinfo == ফাঁকা)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_আকার = i;
		ফেরত -1;
	    }
	}
	যদি (csinfo == ফাঁকা)
	    ফেরত -1;
	জন্য (j = csinfo_আকার/2; j < csinfo_আকার; j++)
	    clear_csinfo(j);
    }

    যদি ((csinfo[i].fname = alloc(strlen(fname)+1)) == ফাঁকা)
	ফেরত -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((ধ্রুবক অক্ষর *)fname));

    যদি (ppath != ফাঁকা)
    {
	যদি ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == ফাঁকা)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    ফেরত -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((ধ্রুবক অক্ষর *)ppath));
    }
    অন্যথা
	csinfo[i].ppath = ফাঁকা;

    যদি (flags != ফাঁকা)
    {
	যদি ((csinfo[i].flags = alloc(strlen(flags) + 1)) == ফাঁকা)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    ফেরত -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((ধ্রুবক অক্ষর *)flags));
    }
    অন্যথা
	csinfo[i].flags = ফাঁকা;

# যদি defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# অন্যথা
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_সংখ্যা;
    csinfo[i].n_সূচক_নিচের = bhfi.n_File_সূচক_নিচের;
    csinfo[i].n_সূচক_উপরের = bhfi.n_File_সূচক_উপরের;
# endif
    ফেরত i;
}


/*
 * Find cscope command in command table.
 */
    স্থির cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    অক্ষর *stok;
    size_t len;

    যদি (eap->arg == ফাঁকা)
	ফেরত ফাঁকা;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (পূর্ণসংখ্যা)STRLEN(eap->arg);

    যদি ((stok = strtok((অক্ষর *)(eap->arg), (ধ্রুবক অক্ষর *)" ")) == ফাঁকা)
	ফেরত ফাঁকা;

    len = strlen(stok);
    জন্য (cmdp = cs_cmds; cmdp->name != ফাঁকা; ++cmdp)
    {
	যদি (strncmp((ধ্রুবক অক্ষর *)(stok), cmdp->name, len) == 0)
	    ফেরত (cmdp);
    }
    ফেরত ফাঁকা;
}


/*
 * Nuke em.
 */
    স্থির পূর্ণসংখ্যা
cs_kill(exarg_T *eap UNUSED)
{
    অক্ষর *stok;
    পূর্ণসংখ্যা i;

    যদি ((stok = strtok((অক্ষর *)ফাঁকা, (ধ্রুবক অক্ষর *)" ")) == ফাঁকা)
    {
	cs_usage_বার্তা(Kill);
	ফেরত CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    যদি ((strlen(stok) < 2 && VIM_ISDIGIT((পূর্ণসংখ্যা)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((পূর্ণসংখ্যা)(stok[1]))))
	i = atoi(stok);
    অন্যথা
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	জন্য (i = 0; i < csinfo_আকার; i++)
	{
	    যদি (csinfo[i].fname != ফাঁকা && strstr(csinfo[i].fname, stok))
		বিরতি;
	}
    }

    যদি ((i != -1) && (i >= csinfo_আকার || i < -1 || csinfo[i].fname == ফাঁকা))
    {
	যদি (p_csverbose)
	    (শূন্য)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    অন্যথা
    {
	যদি (i == -1)
	{
	    জন্য (i = 0; i < csinfo_আকার; i++)
	    {
		যদি (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	অন্যথা
	    cs_kill_execute(i, stok);
    }

    ফেরত 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    স্থির শূন্য
cs_kill_execute(
    পূর্ণসংখ্যা i,	    // cscope table index
    অক্ষর *cname)    // cscope database name
{
    যদি (p_csverbose)
    {
	বার্তা_clr_eos();
	(শূন্য)smsg_attr(HL_ATTR(HLF_R) | বার্তা_HIST,
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
    স্থির অক্ষর *
cs_make_vim_style_matches(
    অক্ষর *fname,
    অক্ষর *slno,
    অক্ষর *অনুসন্ধান,
    অক্ষর *tagstr)
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
    অক্ষর *buf;
    পূর্ণসংখ্যা amt;

    যদি (অনুসন্ধান != ফাঁকা)
    {
	amt = (পূর্ণসংখ্যা)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(অনুসন্ধান)+6);
	যদি ((buf = alloc(amt)) == ফাঁকা)
	    ফেরত ফাঁকা;

	(শূন্য)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, অনুসন্ধান);
    }
    অন্যথা
    {
	amt = (পূর্ণসংখ্যা)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	যদি ((buf = alloc(amt)) == ফাঁকা)
	    ফেরত ফাঁকা;

	(শূন্য)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    ফেরত buf;
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
    স্থির অক্ষর *
cs_manage_matches(
    অক্ষর **matches,
    অক্ষর **contexts,
    পূর্ণসংখ্যা totmatches,
    mcmd_e cmd)
{
    স্থির অক্ষর **mp = ফাঁকা;
    স্থির অক্ষর **cp = ফাঁকা;
    স্থির পূর্ণসংখ্যা cnt = -1;
    স্থির পূর্ণসংখ্যা পরবর্তী = -1;
    অক্ষর *p = ফাঁকা;

    নির্বাচন (cmd)
    {
    ক্ষেত্র Store:
	প্রতিজ্ঞা(matches != ফাঁকা);
	প্রতিজ্ঞা(totmatches > 0);
	যদি (mp != ফাঁকা || cp != ফাঁকা)
	    (শূন্য)cs_manage_matches(ফাঁকা, ফাঁকা, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	পরবর্তী = 0;
	বিরতি;
    ক্ষেত্র Get:
	যদি (পরবর্তী >= cnt)
	    ফেরত ফাঁকা;

	p = mp[পরবর্তী];
	পরবর্তী++;
	বিরতি;
    ক্ষেত্র Free:
	যদি (mp != ফাঁকা)
	{
	    যদি (cnt > 0)
		যখন (cnt--)
		{
		    vim_free(mp[cnt]);
		    যদি (cp != ফাঁকা)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = ফাঁকা;
	cp = ফাঁকা;
	cnt = 0;
	পরবর্তী = 0;
	বিরতি;
    ক্ষেত্র Print:
	cs_print_tags_priv(mp, cp, cnt);
	বিরতি;
    পূর্বনির্ধারিত:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	ফেরত ফাঁকা;
    }

    ফেরত p;
}


/*
 * Parse cscope output.
 */
    স্থির অক্ষর *
cs_parse_results(
    পূর্ণসংখ্যা cnumber,
    অক্ষর *buf,
    পূর্ণসংখ্যা bufsize,
    অক্ষর **context,
    অক্ষর **linenumber,
    অক্ষর **অনুসন্ধান)
{
    পূর্ণসংখ্যা ch;
    অক্ষর *p;
    অক্ষর *name;

    যদি (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == ফাঁকা)
    {
	যদি (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	ফেরত ফাঁকা;
    }

    // If the line's too long for the buffer, discard it.
    যদি ((p = strchr(buf, '\n')) == ফাঁকা)
    {
	যখন ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	ফেরত ফাঁকা;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    যদি ((name = strtok(buf, (ধ্রুবক অক্ষর *)" ")) == ফাঁকা)
	ফেরত ফাঁকা;
    যদি ((*context = strtok(ফাঁকা, (ধ্রুবক অক্ষর *)" ")) == ফাঁকা)
	ফেরত ফাঁকা;
    যদি ((*linenumber = strtok(ফাঁকা, (ধ্রুবক অক্ষর *)" ")) == ফাঁকা)
	ফেরত ফাঁকা;
    *অনুসন্ধান = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    যদি (strcmp(*অনুসন্ধান, "<unknown>") == 0)
	*অনুসন্ধান = ফাঁকা;

    name = cs_resolve_file(cnumber, name);
    ফেরত name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    স্থির শূন্য
cs_file_results(FILE *f, পূর্ণসংখ্যা *nummatches_a)
{
    পূর্ণসংখ্যা i, j;
    অক্ষর *buf;
    অক্ষর *অনুসন্ধান, *slno;
    অক্ষর *fullname;
    অক্ষর *cntx;
    অক্ষর *context;

    buf = alloc(CSREAD_BUFSIZE);
    যদি (buf == ফাঁকা)
	ফেরত;

    জন্য (i = 0; i < csinfo_আকার; i++)
    {
	যদি (nummatches_a[i] < 1)
	    চালু;

	জন্য (j = 0; j < nummatches_a[i]; j++)
	{
	   যদি ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &অনুসন্ধান)) == ফাঁকা)
	       চালু;

	   context = alloc(strlen(cntx)+5);
	   যদি (context == ফাঁকা)
	   {
	       vim_free(fullname);
	       চালু;
	   }

	   যদি (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   অন্যথা
	       sprintf(context, "<<%s>>", cntx);

	   যদি (অনুসন্ধান == ফাঁকা)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   অন্যথা
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, অনুসন্ধান);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(শূন্য)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    স্থির শূন্য
cs_fill_results(
    অক্ষর *tagstr,
    পূর্ণসংখ্যা totmatches,
    পূর্ণসংখ্যা *nummatches_a,
    অক্ষর ***matches_p,
    অক্ষর ***cntxts_p,
    পূর্ণসংখ্যা *matched)
{
    পূর্ণসংখ্যা i, j;
    অক্ষর *buf;
    অক্ষর *অনুসন্ধান, *slno;
    পূর্ণসংখ্যা totsofar = 0;
    অক্ষর **matches = ফাঁকা;
    অক্ষর **cntxts = ফাঁকা;
    অক্ষর *fullname;
    অক্ষর *cntx;

    প্রতিজ্ঞা(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    যদি (buf == ফাঁকা)
	ফেরত;

    যদি ((matches = ALLOC_MULT(অক্ষর *, totmatches)) == ফাঁকা)
	যাও parse_out;
    যদি ((cntxts = ALLOC_MULT(অক্ষর *, totmatches)) == ফাঁকা)
	যাও parse_out;

    জন্য (i = 0; i < csinfo_আকার; i++)
    {
	যদি (nummatches_a[i] < 1)
	    চালু;

	জন্য (j = 0; j < nummatches_a[i]; j++)
	{
	   যদি ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &অনুসন্ধান)) == ফাঁকা)
		চালু;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  অনুসন্ধান, tagstr);

	    vim_free(fullname);

	    যদি (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = ফাঁকা;
	    অন্যথা
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (অক্ষর *)vim_strsave((char_u *)cntx);

	    যদি (matches[totsofar] != ফাঁকা)
		totsofar++;

	} // for all matches

	(শূন্য)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    যদি (totsofar == 0)
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
    স্থির অক্ষর *
cs_pathcomponents(অক্ষর *path)
{
    পূর্ণসংখ্যা		i;
    অক্ষর	*s;

    যদি (p_cspc == 0)
	ফেরত path;

    s = path + strlen(path) - 1;
    জন্য (i = 0; i < p_cspc; ++i)
    {
	যখন (s > path)
	{
	   s--;
	   যদি (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      বিরতি;
	}
    }
    যদি ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    ফেরত s;
}

/*
 * Called from cs_manage_matches().
 */
    স্থির শূন্য
cs_print_tags_priv(অক্ষর **matches, অক্ষর **cntxts, পূর্ণসংখ্যা অঙ্ক_matches)
{
    অক্ষর	*buf = ফাঁকা;
    অক্ষর	*t_buf;
    পূর্ণসংখ্যা		bufsize = 0; // Track available bufsize
    পূর্ণসংখ্যা		newsize = 0;
    অক্ষর	*ptag;
    অক্ষর	*fname, *lno, *extra, *tbuf;
    পূর্ণসংখ্যা		i, idx, অঙ্ক;
    অক্ষর	*globalcntx = "GLOBAL";
    অক্ষর	*cntxformat = " <<%s>>";
    অক্ষর	*context;
    অক্ষর	*cstag_বার্তা = _("Cscope tag: %s");
    অক্ষর	*csfmt_str = "%4d %6s  ";

    প্রতিজ্ঞা(অঙ্ক_matches > 0);

    যদি ((tbuf = alloc(strlen(matches[0]) + 1)) == ফাঁকা)
	ফেরত;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    যদি (ptag == ফাঁকা)
    {
	vim_free(tbuf);
	ফেরত;
    }

    newsize = (পূর্ণসংখ্যা)(strlen(cstag_বার্তা) + strlen(ptag));
    buf = alloc(newsize);
    যদি (buf != ফাঁকা)
    {
	bufsize = newsize;
	(শূন্য)sprintf(buf, cstag_বার্তা, ptag);
	বার্তা_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    বার্তা_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    বার্তা_advance(বার্তা_col + 2);
    বার্তা_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    অঙ্ক = 1;
    জন্য (i = 0; i < অঙ্ক_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	যদি ((tbuf = alloc(strlen(matches[idx]) + 1)) == ফাঁকা)
	    চালু;
	(শূন্য)strcpy(tbuf, matches[idx]);

	যদি (strtok(tbuf, (ধ্রুবক অক্ষর *)"\t") == ফাঁকা
		|| (fname = strtok(ফাঁকা, (ধ্রুবক অক্ষর *)"\t")) == ফাঁকা
		|| (lno = strtok(ফাঁকা, (ধ্রুবক অক্ষর *)"\t")) == ফাঁকা)
	{
	    vim_free(tbuf);
	    চালু;
	}
	extra = strtok(ফাঁকা, (ধ্রুবক অক্ষর *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (পূর্ণসংখ্যা)(strlen(csfmt_str) + 16 + strlen(lno));
	যদি (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    যদি (buf == ফাঁকা)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    অন্যথা
		bufsize = newsize;
	}
	যদি (buf != ফাঁকা)
	{
	    // csfmt_str = "%4d %6s  ";
	    (শূন্য)sprintf(buf, csfmt_str, অঙ্ক, lno);
	    বার্তা_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	বার্তা_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	যদি (cntxts[idx] != ফাঁকা)
	    context = cntxts[idx];
	অন্যথা
	    context = globalcntx;
	newsize = (পূর্ণসংখ্যা)(strlen(context) + strlen(cntxformat));

	যদি (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    যদি (buf == ফাঁকা)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    অন্যথা
		bufsize = newsize;
	}
	যদি (buf != ফাঁকা)
	{
	    (শূন্য)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    যদি (বার্তা_col + (পূর্ণসংখ্যা)strlen(buf) >= cmdline_width)
		বার্তা_putchar('\n');
	    বার্তা_advance(12);
	    বার্তা_outtrans_long_attr((char_u *)buf, 0);
	    বার্তা_putchar('\n');
	}
	যদি (extra != ফাঁকা)
	{
	    বার্তা_advance(13);
	    বার্তা_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	যদি (বার্তা_col)
	    বার্তা_putchar('\n');

	ui_breakcheck();
	যদি (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    বিরতি;
	}

	অঙ্ক++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    স্থির পূর্ণসংখ্যা
cs_read_prompt(পূর্ণসংখ্যা i)
{
    পূর্ণসংখ্যা		ch;
    অক্ষর	*buf = ফাঁকা; // buffer for possible error message from cscope
    পূর্ণসংখ্যা		bufpos = 0;
    পূর্ণসংখ্যা		maxlen;
    স্থির অক্ষর *eprompt = "Press the RETURN key to continue:";
    পূর্ণসংখ্যা		epromptlen = (পূর্ণসংখ্যা)strlen(eprompt);
    পূর্ণসংখ্যা		n;

    // compute maximum allowed len for Cscope error message
    অক্ষর *cs_emsg = _(e_cscope_error_str);
    maxlen = (পূর্ণসংখ্যা)(IOSIZE - strlen(cs_emsg));

    জন্য (;;)
    {
	যখন ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    যদি (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		যদি (buf == ফাঁকা) // lazy buffer allocation
		    buf = alloc(maxlen);
		যদি (buf != ফাঁকা)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    যদি (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(শূন্য)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(শূন্য)putc('\n', csinfo[i].to_fp);
			(শূন্য)fflush(csinfo[i].to_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	জন্য (n = 0; n < (পূর্ণসংখ্যা)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    যদি (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    যদি (ch == EOF)
	    {
		যদি (buf != ফাঁকা && buf[0] != NUL)
		    (শূন্য)semsg(cs_emsg, buf);
		অন্যথা যদি (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		ফেরত CSCOPE_FAILURE;
	    }

	    যদি (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		বিরতি;
	    }
	}

	যদি (ch == EOF)
	    চালু;	    // didn't find the prompt
	বিরতি;		    // did find the prompt
    }

    vim_free(buf);
    ফেরত CSCOPE_SUCCESS;
}

# যদি defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    স্থির শূন্য
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    স্থির শূন্য
cs_release_csp(পূর্ণসংখ্যা i, পূর্ণসংখ্যা freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    যদি (csinfo[i].to_fp != ফাঁকা)
    {
	(শূন্য)fputs("q\n", csinfo[i].to_fp);
	(শূন্য)fflush(csinfo[i].to_fp);
    }
# যদি defined(UNIX)
    {
	পূর্ণসংখ্যা waitpid_errno;
	পূর্ণসংখ্যা pstat;
	pid_t pid;

#  যদি defined(HAVE_SIGACTION)
	কাঠামো sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   অন্যথা
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, ফাঁকা);
#  অন্যথা
	পূর্ণসংখ্যা waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	জন্য (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    যদি (pid != 0)
		বিরতি;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	যদি (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    পূর্ণসংখ্যা alive = TRUE;

	    যদি (waitpid_errno == ECHILD)
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
		পূর্ণসংখ্যা waited;

		sleep(0);
		জন্য (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    যদি (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			বিরতি;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    যদি (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(শূন্য)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# অন্যথা  // !UNIX
    যদি (csinfo[i].hProc != ফাঁকা)
    {
	// Give cscope a chance to exit normally
	যদি (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    যদি (csinfo[i].fr_fp != ফাঁকা)
	(শূন্য)fclose(csinfo[i].fr_fp);
    যদি (csinfo[i].to_fp != ফাঁকা)
	(শূন্য)fclose(csinfo[i].to_fp);

    যদি (freefnpp)
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
    স্থির পূর্ণসংখ্যা
cs_reset(exarg_T *eap UNUSED)
{
    অক্ষর	**dblist = ফাঁকা, **pplist = ফাঁকা, **fllist = ফাঁকা;
    পূর্ণসংখ্যা	i;
    অক্ষর buf[20]; // for sprintf " (#%d)"

    যদি (csinfo_আকার == 0)
	ফেরত CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(অক্ষর *, csinfo_আকার);
    pplist = ALLOC_MULT(অক্ষর *, csinfo_আকার);
    fllist = ALLOC_MULT(অক্ষর *, csinfo_আকার);
    যদি (dblist == ফাঁকা || pplist == ফাঁকা || fllist == ফাঁকা)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	ফেরত CSCOPE_FAILURE;
    }

    জন্য (i = 0; i < csinfo_আকার; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	যদি (csinfo[i].fname != ফাঁকা)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    জন্য (i = 0; i < csinfo_আকার; i++)
    {
	যদি (dblist[i] != ফাঁকা)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    যদি (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		বার্তা_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    যদি (p_csverbose)
	বার্তা_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | বার্তা_HIST);
    ফেরত CSCOPE_SUCCESS;
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
    স্থির অক্ষর *
cs_resolve_file(পূর্ণসংখ্যা i, অক্ষর *name)
{
    অক্ষর	*fullname;
    string_T	csdir = {ফাঁকা, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    পূর্ণসংখ্যা		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (পূর্ণসংখ্যা)namelen + 2;
    যদি (csinfo[i].ppath != ফাঁকা)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (পূর্ণসংখ্যা)ppathlen;
    }
    অন্যথা যদি (p_csre && csinfo[i].fname != ফাঁকা)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	যদি (csdir.string != ফাঁকা)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.দৈর্ঘ্য = STRLEN(csdir.string);
	    len += (পূর্ণসংখ্যা)csdir.দৈর্ঘ্য;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    যদি (csinfo[i].ppath != ফাঁকা
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	যদি ((fullname = alloc(len)) != ফাঁকা)
	    (শূন্য)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    অন্যথা যদি (csdir.string != ফাঁকা && csinfo[i].fname != ফাঁকা && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (অক্ষর *)concat_fnames(csdir.string, csdir.দৈর্ঘ্য, (char_u *)name, namelen, TRUE, &ret);
    }
    অন্যথা
    {
	fullname = (অক্ষর *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    ফেরত fullname;
}


/*
 * Show all cscope connections.
 */
    স্থির পূর্ণসংখ্যা
cs_show(exarg_T *eap UNUSED)
{
    পূর্ণসংখ্যা i;

    যদি (cs_cnt_connections() == 0)
	বার্তা_puts(_("no cscope connections\n"));
    অন্যথা
    {
	বার্তা_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	জন্য (i = 0; i < csinfo_আকার; i++)
	{
	    যদি (csinfo[i].fname == ফাঁকা)
		চালু;

	    যদি (csinfo[i].ppath != ফাঁকা)
		(শূন্য)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    অন্যথা
		(শূন্য)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    ফেরত CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    শূন্য
cs_end(শূন্য)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < csinfo_আকার; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_আকার = 0;
}

#endif	// FEAT_CSCOPE

#যদি defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    শূন্য
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    পূর্ণসংখ্যা		অঙ্ক = 0;
    char_u	*dbpath = ফাঁকা;
    char_u	*prepend = ফাঁকা;
    char_u	buf[NUMBUFLEN];

    যদি (in_vim9script()
	    && (check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	অঙ্ক = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_সংখ্যা = cs_connection(অঙ্ক, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
