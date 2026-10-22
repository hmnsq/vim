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

#অন্তর্ভাৱ "vim.h"

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

প্রকারনির্দেশ সংরচনা {
    ৱর্ণ *  name;
    পূর্ণাঙ্ক     (*func)(exarg_T *eap);
    ৱর্ণ *  help;
    ৱর্ণ *  usage;
    পূর্ণাঙ্ক	    cansplit;		// if supports splitting window
} cscmd_T;

প্রকারনির্দেশ সংরচনা csi {
    ৱর্ণ *	    fname;	// cscope db name
    ৱর্ণ *	    ppath;	// path to prepend (the -P option)
    ৱর্ণ *	    flags;	// additional cscope flags/options (e.g, -p2)
# যদি defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# অন্যথা
#  যদি defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_Index_উপৰেৰ;	// st_ino has no meaning in the Windows
    DWORD	    n_Index_নিচেৰ;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    to_fp;	// to cscope: FILE.
} csinfo_T;

প্রকারনির্দেশ গণনা { Add, Find, Help, Kill, Reset, Show } csid_e;

প্রকারনির্দেশ গণনা {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

স্থির পূর্ণাঙ্ক	    cs_add(exarg_T *eap);
স্থির পূর্ণাঙ্ক	    cs_add_common(ৱর্ণ *, ৱর্ণ *, ৱর্ণ *);
স্থির পূর্ণাঙ্ক	    cs_check_for_connections(শূন্য);
স্থির পূর্ণাঙ্ক	    cs_check_for_tags(শূন্য);
স্থির পূর্ণাঙ্ক	    cs_cnt_connections(শূন্য);
স্থির পূর্ণাঙ্ক	    cs_create_connection(পূর্ণাঙ্ক i);
# ifdef FEAT_QUICKFIX
স্থির শূন্য	    cs_file_results(FILE *, পূর্ণাঙ্ক *);
# endif
স্থির শূন্য	    cs_fill_results(ৱর্ণ *, পূর্ণাঙ্ক , পূর্ণাঙ্ক *, ৱর্ণ ***,
			ৱর্ণ ***, পূর্ণাঙ্ক *);
স্থির পূর্ণাঙ্ক	    cs_find(exarg_T *eap);
স্থির পূর্ণাঙ্ক	    cs_find_common(ৱর্ণ *opt, ৱর্ণ *pat, পূর্ণাঙ্ক, পূর্ণাঙ্ক, পূর্ণাঙ্ক, char_u *cmdline);
স্থির পূর্ণাঙ্ক	    cs_help(exarg_T *eap);
স্থির পূর্ণাঙ্ক	    cs_ঢোকাও_filelist(ৱর্ণ *, ৱর্ণ *, ৱর্ণ *,
			stat_T *);
স্থির পূর্ণাঙ্ক	    cs_kill(exarg_T *eap);
স্থির শূন্য	    cs_kill_execute(পূর্ণাঙ্ক, ৱর্ণ *);
স্থির cscmd_T *    cs_lookup_cmd(exarg_T *eap);
স্থির ৱর্ণ *	    cs_make_vim_style_matches(ৱর্ণ *, ৱর্ণ *,
			ৱর্ণ *, ৱর্ণ *);
স্থির ৱর্ণ *	    cs_manage_matches(ৱর্ণ **, ৱর্ণ **, পূর্ণাঙ্ক, mcmd_e);
স্থির শূন্য	    cs_print_tags_priv(ৱর্ণ **, ৱর্ণ **, পূর্ণাঙ্ক);
স্থির পূর্ণাঙ্ক	    cs_read_prompt(পূর্ণাঙ্ক);
স্থির শূন্য	    cs_release_csp(পূর্ণাঙ্ক, পূর্ণাঙ্ক freefnpp);
স্থির পূর্ণাঙ্ক	    cs_reset(exarg_T *eap);
স্থির ৱর্ণ *	    cs_resolve_file(পূর্ণাঙ্ক, ৱর্ণ *);
স্থির পূর্ণাঙ্ক	    cs_show(exarg_T *eap);


স্থির csinfo_T *   csinfo = রিক্ত;
স্থির পূর্ণাঙ্ক	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

স্থির পূর্ণাঙ্ক	    eap_arg_len;    // length of eap->arg, set in
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
    { রিক্ত, রিক্ত, রিক্ত, রিক্ত, 0 }
};

    স্থির শূন্য
cs_usage_বাৰ্তা(csid_e x)
{
    (শূন্য)semsg(_(e_usage_cscope_str), cs_cmds[(পূর্ণাঙ্ক)x].usage);
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
get_cscope_name(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    পূর্ণাঙ্ক		current_idx;
    পূর্ণাঙ্ক		i;

    ৱিকল্প (expand_what)
    {
    পক্ষ EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	ঘূৰাই (char_u *)cs_cmds[idx].name;
    পক্ষ EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	বাবে (i = 0, current_idx = 0; cs_cmds[i].name != রিক্ত; i++)
	    যদি (cs_cmds[i].cansplit)
		যদি (current_idx++ == idx)
		    ভঙ্গ;
	ঘূৰাই (char_u *)cs_cmds[i].name;
    পক্ষ EXP_CSCOPE_FIND:
	{
	    ধ্রুৱ ৱর্ণ *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", রিক্ত
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    ঘূৰাই (char_u *)query_type[idx];
	}
    পক্ষ EXP_CSCOPE_KILL:
	{
	    স্থির ৱর্ণ	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    বাবে (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		যদি (csinfo[i].fname == রিক্ত)
		    অনুৱর্ত;
		যদি (current_idx++ == idx)
		{
		    vim_snprintf(connection, পরিমাণ(connection), "%d", i);
		    ঘূৰাই (char_u *)connection;
		}
	    }
	    ঘূৰাই (current_idx == idx && idx > 0) ? (char_u *)"-1" : রিক্ত;
	}
    স্ৱভাৱ:
	ঘূৰাই রিক্ত;
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
	ঘূৰাই;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    যদি (*p == NUL)
	ঘূৰাই;

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
    পূর্ণাঙ্ক		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    যদি (check_restricted())
	ঘূৰাই;

    যদি ((cmdp = cs_lookup_cmd(eap)) == রিক্ত)
    {
	cs_help(eap);
	ঘূৰাই;
    }

    যদি (make_split)
    {
	যদি (!cmdp->cansplit)
	{
	    (শূন্য)বাৰ্তা_puts(_("This cscope command does not support splitting the window.\n"));
	    ঘূৰাই;
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
    পূর্ণাঙ্ক ret = FALSE;

    যদি (*eap->arg == NUL)
    {
	(শূন্য)emsg(_(e_usage_cstag_ident));
	ঘূৰাই;
    }

    ৱিকল্প (p_csto)
    {
    পক্ষ 0 :
	যদি (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (ৱর্ণ *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    যদি (ret == FALSE)
	    {
		cs_free_tags();
		যদি (বাৰ্তা_col)
		    বাৰ্তা_putchar('\n');

		যদি (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	অন্যথা যদি (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	ভঙ্গ;
    পক্ষ 1 :
	যদি (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    যদি (ret == FALSE)
	    {
		যদি (বাৰ্তা_col)
		    বাৰ্তা_putchar('\n');

		যদি (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (ৱর্ণ *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    যদি (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	অন্যথা যদি (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (ৱর্ণ *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    যদি (ret == FALSE)
		cs_free_tags();
	}
	ভঙ্গ;
    স্ৱভাৱ :
	ভঙ্গ;
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
    পূর্ণাঙ্ক
cs_fgets(char_u *buf, পূর্ণাঙ্ক size)
{
    ৱর্ণ *p;

    যদি ((p = cs_manage_matches(রিক্ত, রিক্ত, -1, Get)) == রিক্ত)
	ঘূৰাই TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    ঘূৰাই FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    শূন্য
cs_free_tags(শূন্য)
{
    cs_manage_matches(রিক্ত, রিক্ত, -1, Free);
}


/*
 * Called from do_tag().
 */
    শূন্য
cs_print_tags(শূন্য)
{
    cs_manage_matches(রিক্ত, রিক্ত, -1, Print);
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
    স্থির পূর্ণাঙ্ক
cs_connection(পূর্ণাঙ্ক অঙ্ক, char_u *dbpath, char_u *ppath)
{
    পূর্ণাঙ্ক i;

    যদি (অঙ্ক < 0 || অঙ্ক > 4 || (অঙ্ক > 0 && !dbpath))
	ঘূৰাই FALSE;

    বাবে (i = 0; i < csinfo_size; i++)
    {
	যদি (!csinfo[i].fname)
	    অনুৱর্ত;

	যদি (অঙ্ক == 0)
	    ঘূৰাই TRUE;

	ৱিকল্প (অঙ্ক)
	{
	পক্ষ 1:
	    যদি (strstr(csinfo[i].fname, (ৱর্ণ *)dbpath))
		ঘূৰাই TRUE;
	    ভঙ্গ;
	পক্ষ 2:
	    যদি (strcmp(csinfo[i].fname, (ৱর্ণ *)dbpath) == 0)
		ঘূৰাই TRUE;
	    ভঙ্গ;
	পক্ষ 3:
	    যদি (strstr(csinfo[i].fname, (ৱর্ণ *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (ৱর্ণ *)ppath))))
		ঘূৰাই TRUE;
	    ভঙ্গ;
	পক্ষ 4:
	    যদি ((strcmp(csinfo[i].fname, (ৱর্ণ *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (ৱর্ণ *)ppath) == 0))))
		ঘূৰাই TRUE;
	    ভঙ্গ;
	}
    }

    ঘূৰাই FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    স্থির পূর্ণাঙ্ক
cs_add(exarg_T *eap UNUSED)
{
    ৱর্ণ *fname, *ppath, *flags = রিক্ত;

    যদি ((fname = strtok((ৱর্ণ *)রিক্ত, (ধ্রুৱ ৱর্ণ *)" ")) == রিক্ত)
    {
	cs_usage_বাৰ্তা(Add);
	ঘূৰাই CSCOPE_FAILURE;
    }
    যদি ((ppath = strtok((ৱর্ণ *)রিক্ত, (ধ্রুৱ ৱর্ণ *)" ")) != রিক্ত)
	flags = strtok((ৱর্ণ *)রিক্ত, (ধ্রুৱ ৱর্ণ *)" ");

    ঘূৰাই cs_add_common(fname, ppath, flags);
}

    স্থির শূন্য
cs_stat_emsg(ৱর্ণ *fname)
{
    পূর্ণাঙ্ক err = errno;
    (শূন্য)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    স্থির পূর্ণাঙ্ক
cs_add_common(
    ৱর্ণ *arg1,	    // filename - may contain environment variables
    ৱর্ণ *arg2,	    // prepend path - may contain environment variables
    ৱর্ণ *flags)
{
    stat_T	statbuf;
    পূর্ণাঙ্ক		ret;
    ৱর্ণ	*fname = রিক্ত;
    ৱর্ণ	*fname2 = রিক্ত;
    ৱর্ণ	*ppath = রিক্ত;
    পূর্ণাঙ্ক		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = রিক্ত;

    // get the filename (arg1), expand it, and try to stat it
    যদি ((fname = alloc(MAXPATHL + 1)) == রিক্ত)
	গচ্ছ add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (শূন্য)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    যদি (fname == রিক্ত)
	গচ্ছ add_err;
    fname = (ৱর্ণ *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    যদি (ret < 0)
    {
staterr:
	যদি (p_csverbose)
	    cs_stat_emsg(fname);
	গচ্ছ add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    যদি (arg2 != রিক্ত)
    {
	stat_T	    statbuf2;

	যদি ((ppath = alloc(MAXPATHL + 1)) == রিক্ত)
	    গচ্ছ add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	যদি (ret < 0)
	    গচ্ছ staterr;
    }

    // if filename is a directory, append the cscope database name to it
    যদি (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	যদি (fname2 == রিক্ত)
	    গচ্ছ add_err;

	যেতিয়ালৈ (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    যদি (fname[0] == '\0')
		ভঙ্গ;
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
	    গচ্ছ add_err;
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
	গচ্ছ add_err;
    }

    যদি (i != -1)
    {
	যদি (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    গচ্ছ add_err;
	}

	যদি (p_csverbose)
	{
	    বাৰ্তা_clr_eos();
	    (শূন্য)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    ঘূৰাই CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    ঘূৰাই CSCOPE_FAILURE;
}


    স্থির পূর্ণাঙ্ক
cs_check_for_connections(শূন্য)
{
    ঘূৰাই (cs_cnt_connections() > 0);
}


    স্থির পূর্ণাঙ্ক
cs_check_for_tags(শূন্য)
{
    ঘূৰাই (p_tags[0] != NUL && curbuf->b_p_tags != রিক্ত);
}


/*
 * Count the number of cscope connections.
 */
    স্থির পূর্ণাঙ্ক
cs_cnt_connections(শূন্য)
{
    পূর্ণাঙ্ক i;
    পূর্ণাঙ্ক cnt = 0;

    বাবে (i = 0; i < csinfo_size; i++)
	যদি (csinfo[i].fname != রিক্ত)
	    cnt++;
    ঘূৰাই cnt;
}

    স্থির শূন্য
cs_reading_emsg(
    পূর্ণাঙ্ক idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    স্থির পূর্ণাঙ্ক
cs_cnt_matches(পূর্ণাঙ্ক idx)
{
    ৱর্ণ *stok;
    ৱর্ণ *buf;
    পূর্ণাঙ্ক nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    যদি (buf == রিক্ত)
	ঘূৰাই 0;
    বাবে (;;)
    {
	যদি (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    যদি (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    ঘূৰাই -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	যদি (strstr((ধ্রুৱ ৱর্ণ *)buf, "Unable to search database") != রিক্ত)
	    ভঙ্গ;
	যদি ((stok = strtok(buf, (ধ্রুৱ ৱর্ণ *)" ")) == রিক্ত)
	    অনুৱর্ত;
	যদি (strstr((ধ্রুৱ ৱর্ণ *)stok, "cscope:") == রিক্ত)
	    অনুৱর্ত;

	যদি ((stok = strtok(রিক্ত, (ধ্রুৱ ৱর্ণ *)" ")) == রিক্ত)
	    অনুৱর্ত;
	nlines = atoi(stok);
	যদি (nlines < 0)
	{
	    nlines = 0;
	    ভঙ্গ;
	}

	যদি ((stok = strtok(রিক্ত, (ধ্রুৱ ৱর্ণ *)" ")) == রিক্ত)
	    অনুৱর্ত;
	যদি (strncmp((ধ্রুৱ ৱর্ণ *)stok, "lines", 5))
	    অনুৱর্ত;

	ভঙ্গ;
    }

    vim_free(buf);
    ঘূৰাই nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    স্থির ৱর্ণ *
cs_create_cmd(ৱর্ণ *csoption, ৱর্ণ *pattern)
{
    ৱর্ণ *cmd;
    short অনুসন্ধান;
    ৱর্ণ *pat;

    ৱিকল্প (csoption[0])
    {
    পক্ষ '0' : পক্ষ 's' :
	অনুসন্ধান = 0;
	ভঙ্গ;
    পক্ষ '1' : পক্ষ 'g' :
	অনুসন্ধান = 1;
	ভঙ্গ;
    পক্ষ '2' : পক্ষ 'd' :
	অনুসন্ধান = 2;
	ভঙ্গ;
    পক্ষ '3' : পক্ষ 'c' :
	অনুসন্ধান = 3;
	ভঙ্গ;
    পক্ষ '4' : পক্ষ 't' :
	অনুসন্ধান = 4;
	ভঙ্গ;
    পক্ষ '6' : পক্ষ 'e' :
	অনুসন্ধান = 6;
	ভঙ্গ;
    পক্ষ '7' : পক্ষ 'f' :
	অনুসন্ধান = 7;
	ভঙ্গ;
    পক্ষ '8' : পক্ষ 'i' :
	অনুসন্ধান = 8;
	ভঙ্গ;
    পক্ষ '9' : পক্ষ 'a' :
	অনুসন্ধান = 9;
	ভঙ্গ;
    স্ৱভাৱ :
	(শূন্য)emsg(_(e_unknown_cscope_অনুসন্ধান_type));
	cs_usage_বাৰ্তা(Find);
	ঘূৰাই রিক্ত;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    যদি (অনুসন্ধান != 4 && অনুসন্ধান != 6)
	যেতিয়ালৈ VIM_ISWHITE(*pat)
	    ++pat;

    যদি ((cmd = alloc(strlen(pat) + 2)) == রিক্ত)
	ঘূৰাই রিক্ত;

    (শূন্য)sprintf(cmd, "%d%s", অনুসন্ধান, pat);

    ঘূৰাই cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    স্থির পূর্ণাঙ্ক
cs_create_connection(পূর্ণাঙ্ক i)
{
# ifdef UNIX
    পূর্ণাঙ্ক		to_cs[2], from_cs[2];
# endif
    পূর্ণাঙ্ক		cmdlen;
    ৱর্ণ	*prog, *cmd, *ppath = রিক্ত;
    size_t	proglen;
# ifdef MSWIN
    পূর্ণাঙ্ক		fd;
    পূর্ণাঙ্ক		len;
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
	ঘূৰাই CSCOPE_FAILURE;
    }

    যদি ((csinfo[i].pid = fork()) == -1)
    {
	(শূন্য)emsg(_(e_could_not_fork_for_cscope));
	গচ্ছ err_closing;
    }
    অন্যথা যদি (csinfo[i].pid == 0)	// child: run cscope.
    {
	ৱর্ণ **argv = রিক্ত;

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
	sa.nLength = পরিমাণ(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = রিক্ত;

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
	    ঘূৰাই CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	যদি ((prog = alloc(MAXPATHL + 1)) == রিক্ত)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# অন্যথা
	    // MSWIN
	    গচ্ছ err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (পূর্ণাঙ্ক)(proglen + strlen(csinfo[i].fname) + 32);
	যদি (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    যদি ((ppath = alloc(MAXPATHL + 1)) == রিক্ত)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# অন্যথা
		// MSWIN
		গচ্ছ err_closing;
# endif
	    }
	    cmdlen += (পূর্ণাঙ্ক)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	যদি (csinfo[i].flags)
	    cmdlen += (পূর্ণাঙ্ক)strlen(csinfo[i].flags);

	যদি ((cmd = alloc(cmdlen)) == রিক্ত)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# অন্যথা
	    // MSWIN
	    গচ্ছ err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    ৱর্ণ	**tok = রিক্ত;
	    পূর্ণাঙ্ক		tokc = 0;

	    ga_init2(&ga_argv, পরিমাণ(ৱর্ণ *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    যদি (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    বাবে (পূর্ণাঙ্ক k = 0; k < tokc; ++k)
	    {
		যদি (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((ৱর্ণ **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    যদি (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((ৱর্ণ **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (ৱর্ণ *)vim_strsave((char_u *)"-dl");
	    ((ৱর্ণ **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (ৱর্ণ *)vim_strsave((char_u *)"-f");
	    ((ৱর্ণ **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(ৱর্ণ *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    যদি (ppath != রিক্ত)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		ৱর্ণ	*parg = alloc(plen);

		যদি (parg == রিক্ত || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((ৱর্ণ **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    যদি (csinfo[i].flags != রিক্ত)
	    {
		যদি (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		বাবে (পূর্ণাঙ্ক k = 0; k < tokc; ++k)
		{
		    যদি (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((ৱর্ণ **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    যদি (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((ৱর্ণ **)ga_argv.ga_data)[ga_argv.ga_len] = রিক্ত;

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

	    argv = (ৱর্ণ **)ga_argv.ga_data;
	    যদি (argv[0] == রিক্ত || execvp(argv[0], argv) == -1)
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
	যদি ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == রিক্ত)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	যদি ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == রিক্ত)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(শূন্য)close(to_cs[0]);
	(শূন্য)close(from_cs[1]);
    }
# অন্যথা
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    যদি (csinfo[i].ppath != রিক্ত)
    {
	len = (পূর্ণাঙ্ক)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    যদি (csinfo[i].flags != রিক্ত)
    {
	len = (পূর্ণাঙ্ক)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_প্ৰবেশ  = stdin_rd;
    created = CreateProcess(রিক্ত, cmd, রিক্ত, রিক্ত, TRUE, CREATE_NEW_CONSOLE,
							রিক্ত, রিক্ত, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    যদি (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(শূন্য)emsg(_(e_could_not_spawn_cscope_process));
	গচ্ছ err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    যদি (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == রিক্ত))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    যদি (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == রিক্ত))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    ঘূৰাই CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    স্থির পূর্ণাঙ্ক
cs_find(exarg_T *eap)
{
    ৱর্ণ *opt, *pat;
    পূর্ণাঙ্ক i;

    যদি (cs_check_for_connections() == FALSE)
    {
	(শূন্য)emsg(_(e_no_cscope_connections));
	ঘূৰাই FALSE;
    }

    যদি ((opt = strtok((ৱর্ণ *)রিক্ত, (ধ্রুৱ ৱর্ণ *)" ")) == রিক্ত)
    {
	cs_usage_বাৰ্তা(Find);
	ঘূৰাই FALSE;
    }

    pat = opt + strlen(opt) + 1;
    যদি (pat >= (ৱর্ণ *)eap->arg + eap_arg_len)
    {
	cs_usage_বাৰ্তা(Find);
	ঘূৰাই FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    বাবে (i = 0; i < eap_arg_len; ++i)
	যদি (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    ঘূৰাই cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    স্থির পূর্ণাঙ্ক
cs_find_common(
    ৱর্ণ *opt,
    ৱর্ণ *pat,
    পূর্ণাঙ্ক forceit,
    পূর্ণাঙ্ক verbose,
    পূর্ণাঙ্ক	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    পূর্ণাঙ্ক i;
    ৱর্ণ *cmd;
    পূর্ণাঙ্ক *nummatches;
    পূর্ণাঙ্ক totmatches;
# ifdef FEAT_QUICKFIX
    ৱর্ণ cmdletter;
    ৱর্ণ *qfpos;

    // get cmd letter
    ৱিকল্প (opt[0])
    {
    পক্ষ '0' :
	cmdletter = 's';
	ভঙ্গ;
    পক্ষ '1' :
	cmdletter = 'g';
	ভঙ্গ;
    পক্ষ '2' :
	cmdletter = 'd';
	ভঙ্গ;
    পক্ষ '3' :
	cmdletter = 'c';
	ভঙ্গ;
    পক্ষ '4' :
	cmdletter = 't';
	ভঙ্গ;
    পক্ষ '6' :
	cmdletter = 'e';
	ভঙ্গ;
    পক্ষ '7' :
	cmdletter = 'f';
	ভঙ্গ;
    পক্ষ '8' :
	cmdletter = 'i';
	ভঙ্গ;
    পক্ষ '9' :
	cmdletter = 'a';
	ভঙ্গ;
    স্ৱভাৱ :
	cmdletter = opt[0];
    }

    qfpos = (ৱর্ণ *)vim_strchr(p_csqf, cmdletter);
    যদি (qfpos != রিক্ত)
    {
	qfpos++;
	// next symbol must be + or -
	যদি (strchr(CSQF_FLAGS, *qfpos) == রিক্ত)
	{
	    (শূন্য)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    ঘূৰাই FALSE;
	}

	যদি (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    যদি (aborting())
		ঘূৰাই FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    যদি (cmd == রিক্ত)
	ঘূৰাই FALSE;

    nummatches = ALLOC_MULT(পূর্ণাঙ্ক, csinfo_size);
    যদি (nummatches == রিক্ত)
    {
	vim_free(cmd);
	ঘূৰাই FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    বাবে (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    বাবে (i = 0; i < csinfo_size; i++)
    {
	যদি (csinfo[i].fname == রিক্ত || csinfo[i].to_fp == রিক্ত)
	    অনুৱর্ত;

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
	ঘূৰাই FALSE;
    }

# ifdef FEAT_QUICKFIX
    যদি (qfpos != রিক্ত && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = রিক্ত;
	win_T	    *wp = রিক্ত;

	যদি (tmp == রিক্ত)
	{
	    vim_free(nummatches);
	    ঘূৰাই FALSE;
	}

	f = mch_fopen((ৱর্ণ *)tmp, "w");
	যদি (f == রিক্ত)
	    semsg(_(e_cant_open_file_str), tmp);
	অন্যথা
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    যদি (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    যদি (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, রিক্ত) > 0)
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
		    qi = (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != রিক্ত)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	ঘূৰাই TRUE;
    }
    অন্যথা
# endif // FEAT_QUICKFIX
    {
	ৱর্ণ **matches = রিক্ত, **contexts = রিক্ত;
	পূর্ণাঙ্ক matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	যদি (matches == রিক্ত)
	    ঘূৰাই FALSE;

	(শূন্য)cs_manage_matches(matches, contexts, matched, Store);

	ঘূৰাই do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    স্থির পূর্ণাঙ্ক
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (শূন্য)বাৰ্তা_puts(_("cscope commands:\n"));
    যেতিয়ালৈ (cmdp->name != রিক্ত)
    {
	ৱর্ণ *help = _(cmdp->help);
	পূর্ণাঙ্ক  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	যদি (space_cnt < 0)
	    space_cnt = 0;
	(শূন্য)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	যদি (strcmp(cmdp->name, "find") == 0)
	    বাৰ্তা_puts(_("\n"
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
    ঘূৰাই 0;
}


    স্থির শূন্য
clear_csinfo(পূর্ণাঙ্ক i)
{
    csinfo[i].fname  = রিক্ত;
    csinfo[i].ppath  = রিক্ত;
    csinfo[i].flags  = রিক্ত;
# যদি defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# অন্যথা
    csinfo[i].nVolume = 0;
    csinfo[i].n_Index_উপৰেৰ = 0;
    csinfo[i].n_Index_নিচেৰ = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = রিক্ত;
    csinfo[i].to_fp  = রিক্ত;
# যদি defined(MSWIN)
    csinfo[i].hProc = রিক্ত;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    স্থির পূর্ণাঙ্ক
cs_ঢোকাও_filelist(
    ৱর্ণ *fname,
    ৱর্ণ *ppath,
    ৱর্ণ *flags,
    stat_T *sb UNUSED)
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    ৱিকল্প (win32_fileinfo((char_u *)fname, &bhfi))
    {
	পক্ষ FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	পক্ষ FILEINFO_READ_FAIL:	// CreateFile() failed
	    যদি (p_csverbose)
	    {
		ৱর্ণ *cant_বাৰ্তা = _(e_cannot_open_cscope_database_str);
		ৱর্ণ *winmsg = GetWin32Error();

		যদি (winmsg != রিক্ত)
		    (শূন্য)semsg(cant_বাৰ্তা, winmsg);
		অন্যথা
		    // subst filename if can't get error text
		    (শূন্য)semsg(cant_বাৰ্তা, fname);
	    }
	    ঘূৰাই -1;

	পক্ষ FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    যদি (p_csverbose)
		(শূন্য)emsg(_(e_cannot_get_cscope_database_information));
	    ঘূৰাই -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    বাবে (j = 0; j < csinfo_size; j++)
    {
	যদি (csinfo[j].fname != রিক্ত
# যদি defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# অন্যথা
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_সংখ্যা
		    && csinfo[j].n_Index_উপৰেৰ == bhfi.n_File_Index_উপৰেৰ
		    && csinfo[j].n_Index_নিচেৰ == bhfi.n_File_Index_নিচেৰ))
# endif
	    )
	{
	    যদি (p_csverbose)
		(শূন্য)emsg(_(e_duplicate_cscope_database_not_added));
	    ঘূৰাই -1;
	}

	যদি (csinfo[j].fname == রিক্ত && i == -1)
	    i = j; // remember first empty entry
    }

    যদি (i == -1)
    {
	i = csinfo_size;
	যদি (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	অন্যথা
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, পরিমাণ(csinfo_T)*csinfo_size);
	    যদি (csinfo == রিক্ত)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		ঘূৰাই -1;
	    }
	}
	যদি (csinfo == রিক্ত)
	    ঘূৰাই -1;
	বাবে (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    যদি ((csinfo[i].fname = alloc(strlen(fname)+1)) == রিক্ত)
	ঘূৰাই -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((ধ্রুৱ ৱর্ণ *)fname));

    যদি (ppath != রিক্ত)
    {
	যদি ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == রিক্ত)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    ঘূৰাই -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((ধ্রুৱ ৱর্ণ *)ppath));
    }
    অন্যথা
	csinfo[i].ppath = রিক্ত;

    যদি (flags != রিক্ত)
    {
	যদি ((csinfo[i].flags = alloc(strlen(flags) + 1)) == রিক্ত)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    ঘূৰাই -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((ধ্রুৱ ৱর্ণ *)flags));
    }
    অন্যথা
	csinfo[i].flags = রিক্ত;

# যদি defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# অন্যথা
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_সংখ্যা;
    csinfo[i].n_Index_নিচেৰ = bhfi.n_File_Index_নিচেৰ;
    csinfo[i].n_Index_উপৰেৰ = bhfi.n_File_Index_উপৰেৰ;
# endif
    ঘূৰাই i;
}


/*
 * Find cscope command in command table.
 */
    স্থির cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    ৱর্ণ *stok;
    size_t len;

    যদি (eap->arg == রিক্ত)
	ঘূৰাই রিক্ত;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (পূর্ণাঙ্ক)STRLEN(eap->arg);

    যদি ((stok = strtok((ৱর্ণ *)(eap->arg), (ধ্রুৱ ৱর্ণ *)" ")) == রিক্ত)
	ঘূৰাই রিক্ত;

    len = strlen(stok);
    বাবে (cmdp = cs_cmds; cmdp->name != রিক্ত; ++cmdp)
    {
	যদি (strncmp((ধ্রুৱ ৱর্ণ *)(stok), cmdp->name, len) == 0)
	    ঘূৰাই (cmdp);
    }
    ঘূৰাই রিক্ত;
}


/*
 * Nuke em.
 */
    স্থির পূর্ণাঙ্ক
cs_kill(exarg_T *eap UNUSED)
{
    ৱর্ণ *stok;
    পূর্ণাঙ্ক i;

    যদি ((stok = strtok((ৱর্ণ *)রিক্ত, (ধ্রুৱ ৱর্ণ *)" ")) == রিক্ত)
    {
	cs_usage_বাৰ্তা(Kill);
	ঘূৰাই CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    যদি ((strlen(stok) < 2 && VIM_ISDIGIT((পূর্ণাঙ্ক)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((পূর্ণাঙ্ক)(stok[1]))))
	i = atoi(stok);
    অন্যথা
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	বাবে (i = 0; i < csinfo_size; i++)
	{
	    যদি (csinfo[i].fname != রিক্ত && strstr(csinfo[i].fname, stok))
		ভঙ্গ;
	}
    }

    যদি ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == রিক্ত))
    {
	যদি (p_csverbose)
	    (শূন্য)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    অন্যথা
    {
	যদি (i == -1)
	{
	    বাবে (i = 0; i < csinfo_size; i++)
	    {
		যদি (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	অন্যথা
	    cs_kill_execute(i, stok);
    }

    ঘূৰাই 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    স্থির শূন্য
cs_kill_execute(
    পূর্ণাঙ্ক i,	    // cscope table index
    ৱর্ণ *cname)    // cscope database name
{
    যদি (p_csverbose)
    {
	বাৰ্তা_clr_eos();
	(শূন্য)smsg_attr(HL_ATTR(HLF_R) | বাৰ্তা_HIST,
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
    স্থির ৱর্ণ *
cs_make_vim_style_matches(
    ৱর্ণ *fname,
    ৱর্ণ *slno,
    ৱর্ণ *অনুসন্ধান,
    ৱর্ণ *tagstr)
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
    ৱর্ণ *buf;
    পূর্ণাঙ্ক amt;

    যদি (অনুসন্ধান != রিক্ত)
    {
	amt = (পূর্ণাঙ্ক)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(অনুসন্ধান)+6);
	যদি ((buf = alloc(amt)) == রিক্ত)
	    ঘূৰাই রিক্ত;

	(শূন্য)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, অনুসন্ধান);
    }
    অন্যথা
    {
	amt = (পূর্ণাঙ্ক)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	যদি ((buf = alloc(amt)) == রিক্ত)
	    ঘূৰাই রিক্ত;

	(শূন্য)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    ঘূৰাই buf;
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
    স্থির ৱর্ণ *
cs_manage_matches(
    ৱর্ণ **matches,
    ৱর্ণ **contexts,
    পূর্ণাঙ্ক totmatches,
    mcmd_e cmd)
{
    স্থির ৱর্ণ **mp = রিক্ত;
    স্থির ৱর্ণ **cp = রিক্ত;
    স্থির পূর্ণাঙ্ক cnt = -1;
    স্থির পূর্ণাঙ্ক next = -1;
    ৱর্ণ *p = রিক্ত;

    ৱিকল্প (cmd)
    {
    পক্ষ Store:
	প্রতিজ্ঞা(matches != রিক্ত);
	প্রতিজ্ঞা(totmatches > 0);
	যদি (mp != রিক্ত || cp != রিক্ত)
	    (শূন্য)cs_manage_matches(রিক্ত, রিক্ত, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	ভঙ্গ;
    পক্ষ Get:
	যদি (next >= cnt)
	    ঘূৰাই রিক্ত;

	p = mp[next];
	next++;
	ভঙ্গ;
    পক্ষ Free:
	যদি (mp != রিক্ত)
	{
	    যদি (cnt > 0)
		যেতিয়ালৈ (cnt--)
		{
		    vim_free(mp[cnt]);
		    যদি (cp != রিক্ত)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = রিক্ত;
	cp = রিক্ত;
	cnt = 0;
	next = 0;
	ভঙ্গ;
    পক্ষ Print:
	cs_print_tags_priv(mp, cp, cnt);
	ভঙ্গ;
    স্ৱভাৱ:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	ঘূৰাই রিক্ত;
    }

    ঘূৰাই p;
}


/*
 * Parse cscope output.
 */
    স্থির ৱর্ণ *
cs_parse_results(
    পূর্ণাঙ্ক cnumber,
    ৱর্ণ *buf,
    পূর্ণাঙ্ক bufsize,
    ৱর্ণ **context,
    ৱর্ণ **linenumber,
    ৱর্ণ **অনুসন্ধান)
{
    পূর্ণাঙ্ক ch;
    ৱর্ণ *p;
    ৱর্ণ *name;

    যদি (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == রিক্ত)
    {
	যদি (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	ঘূৰাই রিক্ত;
    }

    // If the line's too long for the buffer, discard it.
    যদি ((p = strchr(buf, '\n')) == রিক্ত)
    {
	যেতিয়ালৈ ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	ঘূৰাই রিক্ত;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    যদি ((name = strtok(buf, (ধ্রুৱ ৱর্ণ *)" ")) == রিক্ত)
	ঘূৰাই রিক্ত;
    যদি ((*context = strtok(রিক্ত, (ধ্রুৱ ৱর্ণ *)" ")) == রিক্ত)
	ঘূৰাই রিক্ত;
    যদি ((*linenumber = strtok(রিক্ত, (ধ্রুৱ ৱর্ণ *)" ")) == রিক্ত)
	ঘূৰাই রিক্ত;
    *অনুসন্ধান = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    যদি (strcmp(*অনুসন্ধান, "<unknown>") == 0)
	*অনুসন্ধান = রিক্ত;

    name = cs_resolve_file(cnumber, name);
    ঘূৰাই name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    স্থির শূন্য
cs_file_results(FILE *f, পূর্ণাঙ্ক *nummatches_a)
{
    পূর্ণাঙ্ক i, j;
    ৱর্ণ *buf;
    ৱর্ণ *অনুসন্ধান, *slno;
    ৱর্ণ *fullname;
    ৱর্ণ *cntx;
    ৱর্ণ *context;

    buf = alloc(CSREAD_BUFSIZE);
    যদি (buf == রিক্ত)
	ঘূৰাই;

    বাবে (i = 0; i < csinfo_size; i++)
    {
	যদি (nummatches_a[i] < 1)
	    অনুৱর্ত;

	বাবে (j = 0; j < nummatches_a[i]; j++)
	{
	   যদি ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &অনুসন্ধান)) == রিক্ত)
	       অনুৱর্ত;

	   context = alloc(strlen(cntx)+5);
	   যদি (context == রিক্ত)
	   {
	       vim_free(fullname);
	       অনুৱর্ত;
	   }

	   যদি (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   অন্যথা
	       sprintf(context, "<<%s>>", cntx);

	   যদি (অনুসন্ধান == রিক্ত)
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
    ৱর্ণ *tagstr,
    পূর্ণাঙ্ক totmatches,
    পূর্ণাঙ্ক *nummatches_a,
    ৱর্ণ ***matches_p,
    ৱর্ণ ***cntxts_p,
    পূর্ণাঙ্ক *matched)
{
    পূর্ণাঙ্ক i, j;
    ৱর্ণ *buf;
    ৱর্ণ *অনুসন্ধান, *slno;
    পূর্ণাঙ্ক totsofar = 0;
    ৱর্ণ **matches = রিক্ত;
    ৱর্ণ **cntxts = রিক্ত;
    ৱর্ণ *fullname;
    ৱর্ণ *cntx;

    প্রতিজ্ঞা(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    যদি (buf == রিক্ত)
	ঘূৰাই;

    যদি ((matches = ALLOC_MULT(ৱর্ণ *, totmatches)) == রিক্ত)
	গচ্ছ parse_out;
    যদি ((cntxts = ALLOC_MULT(ৱর্ণ *, totmatches)) == রিক্ত)
	গচ্ছ parse_out;

    বাবে (i = 0; i < csinfo_size; i++)
    {
	যদি (nummatches_a[i] < 1)
	    অনুৱর্ত;

	বাবে (j = 0; j < nummatches_a[i]; j++)
	{
	   যদি ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &অনুসন্ধান)) == রিক্ত)
		অনুৱর্ত;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  অনুসন্ধান, tagstr);

	    vim_free(fullname);

	    যদি (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = রিক্ত;
	    অন্যথা
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (ৱর্ণ *)vim_strsave((char_u *)cntx);

	    যদি (matches[totsofar] != রিক্ত)
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
    স্থির ৱর্ণ *
cs_pathcomponents(ৱর্ণ *path)
{
    পূর্ণাঙ্ক		i;
    ৱর্ণ	*s;

    যদি (p_cspc == 0)
	ঘূৰাই path;

    s = path + strlen(path) - 1;
    বাবে (i = 0; i < p_cspc; ++i)
    {
	যেতিয়ালৈ (s > path)
	{
	   s--;
	   যদি (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      ভঙ্গ;
	}
    }
    যদি ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    ঘূৰাই s;
}

/*
 * Called from cs_manage_matches().
 */
    স্থির শূন্য
cs_print_tags_priv(ৱর্ণ **matches, ৱর্ণ **cntxts, পূর্ণাঙ্ক অঙ্ক_matches)
{
    ৱর্ণ	*buf = রিক্ত;
    ৱর্ণ	*t_buf;
    পূর্ণাঙ্ক		bufsize = 0; // Track available bufsize
    পূর্ণাঙ্ক		newsize = 0;
    ৱর্ণ	*ptag;
    ৱর্ণ	*fname, *lno, *extra, *tbuf;
    পূর্ণাঙ্ক		i, idx, অঙ্ক;
    ৱর্ণ	*globalcntx = "GLOBAL";
    ৱর্ণ	*cntxformat = " <<%s>>";
    ৱর্ণ	*context;
    ৱর্ণ	*cstag_বাৰ্তা = _("Cscope tag: %s");
    ৱর্ণ	*csfmt_str = "%4d %6s  ";

    প্রতিজ্ঞা(অঙ্ক_matches > 0);

    যদি ((tbuf = alloc(strlen(matches[0]) + 1)) == রিক্ত)
	ঘূৰাই;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    যদি (ptag == রিক্ত)
    {
	vim_free(tbuf);
	ঘূৰাই;
    }

    newsize = (পূর্ণাঙ্ক)(strlen(cstag_বাৰ্তা) + strlen(ptag));
    buf = alloc(newsize);
    যদি (buf != রিক্ত)
    {
	bufsize = newsize;
	(শূন্য)sprintf(buf, cstag_বাৰ্তা, ptag);
	বাৰ্তা_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    বাৰ্তা_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    বাৰ্তা_advance(বাৰ্তা_col + 2);
    বাৰ্তা_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    অঙ্ক = 1;
    বাবে (i = 0; i < অঙ্ক_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	যদি ((tbuf = alloc(strlen(matches[idx]) + 1)) == রিক্ত)
	    অনুৱর্ত;
	(শূন্য)strcpy(tbuf, matches[idx]);

	যদি (strtok(tbuf, (ধ্রুৱ ৱর্ণ *)"\t") == রিক্ত
		|| (fname = strtok(রিক্ত, (ধ্রুৱ ৱর্ণ *)"\t")) == রিক্ত
		|| (lno = strtok(রিক্ত, (ধ্রুৱ ৱর্ণ *)"\t")) == রিক্ত)
	{
	    vim_free(tbuf);
	    অনুৱর্ত;
	}
	extra = strtok(রিক্ত, (ধ্রুৱ ৱর্ণ *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (পূর্ণাঙ্ক)(strlen(csfmt_str) + 16 + strlen(lno));
	যদি (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    যদি (buf == রিক্ত)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    অন্যথা
		bufsize = newsize;
	}
	যদি (buf != রিক্ত)
	{
	    // csfmt_str = "%4d %6s  ";
	    (শূন্য)sprintf(buf, csfmt_str, অঙ্ক, lno);
	    বাৰ্তা_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	বাৰ্তা_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	যদি (cntxts[idx] != রিক্ত)
	    context = cntxts[idx];
	অন্যথা
	    context = globalcntx;
	newsize = (পূর্ণাঙ্ক)(strlen(context) + strlen(cntxformat));

	যদি (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    যদি (buf == রিক্ত)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    অন্যথা
		bufsize = newsize;
	}
	যদি (buf != রিক্ত)
	{
	    (শূন্য)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    যদি (বাৰ্তা_col + (পূর্ণাঙ্ক)strlen(buf) >= cmdline_width)
		বাৰ্তা_putchar('\n');
	    বাৰ্তা_advance(12);
	    বাৰ্তা_outtrans_long_attr((char_u *)buf, 0);
	    বাৰ্তা_putchar('\n');
	}
	যদি (extra != রিক্ত)
	{
	    বাৰ্তা_advance(13);
	    বাৰ্তা_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	যদি (বাৰ্তা_col)
	    বাৰ্তা_putchar('\n');

	ui_breakcheck();
	যদি (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    ভঙ্গ;
	}

	অঙ্ক++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    স্থির পূর্ণাঙ্ক
cs_read_prompt(পূর্ণাঙ্ক i)
{
    পূর্ণাঙ্ক		ch;
    ৱর্ণ	*buf = রিক্ত; // buffer for possible error message from cscope
    পূর্ণাঙ্ক		bufpos = 0;
    পূর্ণাঙ্ক		maxlen;
    স্থির ৱর্ণ *eprompt = "Press the RETURN key to continue:";
    পূর্ণাঙ্ক		epromptlen = (পূর্ণাঙ্ক)strlen(eprompt);
    পূর্ণাঙ্ক		n;

    // compute maximum allowed len for Cscope error message
    ৱর্ণ *cs_emsg = _(e_cscope_error_str);
    maxlen = (পূর্ণাঙ্ক)(IOSIZE - strlen(cs_emsg));

    বাবে (;;)
    {
	যেতিয়ালৈ ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    যদি (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		যদি (buf == রিক্ত) // lazy buffer allocation
		    buf = alloc(maxlen);
		যদি (buf != রিক্ত)
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

	বাবে (n = 0; n < (পূর্ণাঙ্ক)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    যদি (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    যদি (ch == EOF)
	    {
		যদি (buf != রিক্ত && buf[0] != NUL)
		    (শূন্য)semsg(cs_emsg, buf);
		অন্যথা যদি (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		ঘূৰাই CSCOPE_FAILURE;
	    }

	    যদি (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		ভঙ্গ;
	    }
	}

	যদি (ch == EOF)
	    অনুৱর্ত;	    // didn't find the prompt
	ভঙ্গ;		    // did find the prompt
    }

    vim_free(buf);
    ঘূৰাই CSCOPE_SUCCESS;
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
cs_release_csp(পূর্ণাঙ্ক i, পূর্ণাঙ্ক freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    যদি (csinfo[i].to_fp != রিক্ত)
    {
	(শূন্য)fputs("q\n", csinfo[i].to_fp);
	(শূন্য)fflush(csinfo[i].to_fp);
    }
# যদি defined(UNIX)
    {
	পূর্ণাঙ্ক waitpid_errno;
	পূর্ণাঙ্ক pstat;
	pid_t pid;

#  যদি defined(HAVE_SIGACTION)
	সংরচনা sigaction sa, old;

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
	sigaction(SIGALRM, &old, রিক্ত);
#  অন্যথা
	পূর্ণাঙ্ক waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	বাবে (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    যদি (pid != 0)
		ভঙ্গ;  // break unless the process is still running
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
	    পূর্ণাঙ্ক alive = TRUE;

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
		পূর্ণাঙ্ক waited;

		sleep(0);
		বাবে (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    যদি (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			ভঙ্গ;
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
    যদি (csinfo[i].hProc != রিক্ত)
    {
	// Give cscope a chance to exit normally
	যদি (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    যদি (csinfo[i].fr_fp != রিক্ত)
	(শূন্য)fclose(csinfo[i].fr_fp);
    যদি (csinfo[i].to_fp != রিক্ত)
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
    স্থির পূর্ণাঙ্ক
cs_reset(exarg_T *eap UNUSED)
{
    ৱর্ণ	**dblist = রিক্ত, **pplist = রিক্ত, **fllist = রিক্ত;
    পূর্ণাঙ্ক	i;
    ৱর্ণ buf[20]; // for sprintf " (#%d)"

    যদি (csinfo_size == 0)
	ঘূৰাই CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(ৱর্ণ *, csinfo_size);
    pplist = ALLOC_MULT(ৱর্ণ *, csinfo_size);
    fllist = ALLOC_MULT(ৱর্ণ *, csinfo_size);
    যদি (dblist == রিক্ত || pplist == রিক্ত || fllist == রিক্ত)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	ঘূৰাই CSCOPE_FAILURE;
    }

    বাবে (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	যদি (csinfo[i].fname != রিক্ত)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    বাবে (i = 0; i < csinfo_size; i++)
    {
	যদি (dblist[i] != রিক্ত)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    যদি (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		বাৰ্তা_puts_attr(buf, HL_ATTR(HLF_R));
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
	বাৰ্তা_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | বাৰ্তা_HIST);
    ঘূৰাই CSCOPE_SUCCESS;
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
    স্থির ৱর্ণ *
cs_resolve_file(পূর্ণাঙ্ক i, ৱর্ণ *name)
{
    ৱর্ণ	*fullname;
    string_T	csdir = {রিক্ত, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    পূর্ণাঙ্ক		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (পূর্ণাঙ্ক)namelen + 2;
    যদি (csinfo[i].ppath != রিক্ত)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (পূর্ণাঙ্ক)ppathlen;
    }
    অন্যথা যদি (p_csre && csinfo[i].fname != রিক্ত)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	যদি (csdir.string != রিক্ত)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (পূর্ণাঙ্ক)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    যদি (csinfo[i].ppath != রিক্ত
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	যদি ((fullname = alloc(len)) != রিক্ত)
	    (শূন্য)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    অন্যথা যদি (csdir.string != রিক্ত && csinfo[i].fname != রিক্ত && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (ৱর্ণ *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    অন্যথা
    {
	fullname = (ৱর্ণ *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    ঘূৰাই fullname;
}


/*
 * Show all cscope connections.
 */
    স্থির পূর্ণাঙ্ক
cs_show(exarg_T *eap UNUSED)
{
    পূর্ণাঙ্ক i;

    যদি (cs_cnt_connections() == 0)
	বাৰ্তা_puts(_("no cscope connections\n"));
    অন্যথা
    {
	বাৰ্তা_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	বাবে (i = 0; i < csinfo_size; i++)
	{
	    যদি (csinfo[i].fname == রিক্ত)
		অনুৱর্ত;

	    যদি (csinfo[i].ppath != রিক্ত)
		(শূন্য)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    অন্যথা
		(শূন্য)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    ঘূৰাই CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    শূন্য
cs_end(শূন্য)
{
    পূর্ণাঙ্ক i;

    বাবে (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
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
    পূর্ণাঙ্ক		অঙ্ক = 0;
    char_u	*dbpath = রিক্ত;
    char_u	*prepend = রিক্ত;
    char_u	buf[NUMBUFLEN];

    যদি (in_vim9script()
	    && (check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	ঘূৰাই;

    যদি (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	অঙ্ক = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_সংখ্যা = cs_connection(অঙ্ক, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
