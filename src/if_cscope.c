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

#अन्तर्भाव "vim.h"

#सचे defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# सचे defined(UNIX)
#  include <sys/wait.h>
# endif

# सचे defined (MSWIN)
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

प्रकारनिर्देश संरचना {
    वर्ण *  name;
    पूर्णाङ्क     (*func)(exarg_T *eap);
    वर्ण *  help;
    वर्ण *  usage;
    पूर्णाङ्क	    cansplit;		// if supports splitting window
} cscmd_T;

प्रकारनिर्देश संरचना csi {
    वर्ण *	    fname;	// cscope db name
    वर्ण *	    ppath;	// path to prepend (the -P option)
    वर्ण *	    flags;	// additional cscope flags/options (e.g, -p2)
# सचे defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# अञ्ञथा
#  सचे defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_Index_उच्चः;	// st_ino has no meaning in the Windows
    DWORD	    n_Index_नीचः;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    to_fp;	// to cscope: FILE.
} csinfo_T;

प्रकारनिर्देश गणना { Add, Find, Help, Kill, Reset, Show } csid_e;

प्रकारनिर्देश गणना {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

स्थिर पूर्णाङ्क	    cs_add(exarg_T *eap);
स्थिर पूर्णाङ्क	    cs_add_common(वर्ण *, वर्ण *, वर्ण *);
स्थिर पूर्णाङ्क	    cs_check_for_connections(शून्य);
स्थिर पूर्णाङ्क	    cs_check_for_tags(शून्य);
स्थिर पूर्णाङ्क	    cs_cnt_connections(शून्य);
स्थिर पूर्णाङ्क	    cs_create_connection(पूर्णाङ्क i);
# ifdef FEAT_QUICKFIX
स्थिर शून्य	    cs_file_results(FILE *, पूर्णाङ्क *);
# endif
स्थिर शून्य	    cs_fill_results(वर्ण *, पूर्णाङ्क , पूर्णाङ्क *, वर्ण ***,
			वर्ण ***, पूर्णाङ्क *);
स्थिर पूर्णाङ्क	    cs_find(exarg_T *eap);
स्थिर पूर्णाङ्क	    cs_find_common(वर्ण *opt, वर्ण *pat, पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क, char_u *cmdline);
स्थिर पूर्णाङ्क	    cs_help(exarg_T *eap);
स्थिर पूर्णाङ्क	    cs_निवेशय_filelist(वर्ण *, वर्ण *, वर्ण *,
			stat_T *);
स्थिर पूर्णाङ्क	    cs_kill(exarg_T *eap);
स्थिर शून्य	    cs_kill_execute(पूर्णाङ्क, वर्ण *);
स्थिर cscmd_T *    cs_lookup_cmd(exarg_T *eap);
स्थिर वर्ण *	    cs_make_vim_style_matches(वर्ण *, वर्ण *,
			वर्ण *, वर्ण *);
स्थिर वर्ण *	    cs_manage_matches(वर्ण **, वर्ण **, पूर्णाङ्क, mcmd_e);
स्थिर शून्य	    cs_print_tags_priv(वर्ण **, वर्ण **, पूर्णाङ्क);
स्थिर पूर्णाङ्क	    cs_read_prompt(पूर्णाङ्क);
स्थिर शून्य	    cs_release_csp(पूर्णाङ्क, पूर्णाङ्क freefnpp);
स्थिर पूर्णाङ्क	    cs_reset(exarg_T *eap);
स्थिर वर्ण *	    cs_resolve_file(पूर्णाङ्क, वर्ण *);
स्थिर पूर्णाङ्क	    cs_show(exarg_T *eap);


स्थिर csinfo_T *   csinfo = रिक्त;
स्थिर पूर्णाङ्क	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

स्थिर पूर्णाङ्क	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
स्थिर cscmd_T	    cs_cmds[] =
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
    { रिक्त, रिक्त, रिक्त, रिक्त, 0 }
};

    स्थिर शून्य
cs_usage_सन्देशः(csid_e x)
{
    (शून्य)semsg(_(e_usage_cscope_str), cs_cmds[(पूर्णाङ्क)x].usage);
}

स्थिर गणना
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
get_cscope_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    पूर्णाङ्क		current_idx;
    पूर्णाङ्क		i;

    विकल्प (expand_what)
    {
    पक्ष EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	पटिनिवत्त (char_u *)cs_cmds[idx].name;
    पक्ष EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	अत्थाय (i = 0, current_idx = 0; cs_cmds[i].name != रिक्त; i++)
	    सचे (cs_cmds[i].cansplit)
		सचे (current_idx++ == idx)
		    भङ्ग;
	पटिनिवत्त (char_u *)cs_cmds[i].name;
    पक्ष EXP_CSCOPE_FIND:
	{
	    ध्रुव वर्ण *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", रिक्त
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    पटिनिवत्त (char_u *)query_type[idx];
	}
    पक्ष EXP_CSCOPE_KILL:
	{
	    स्थिर वर्ण	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    अत्थाय (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		सचे (csinfo[i].fname == रिक्त)
		    अनुवर्त;
		सचे (current_idx++ == idx)
		{
		    vim_snprintf(connection, परिमाण(connection), "%d", i);
		    पटिनिवत्त (char_u *)connection;
		}
	    }
	    पटिनिवत्त (current_idx == idx && idx > 0) ? (char_u *)"-1" : रिक्त;
	}
    स्वभाव:
	पटिनिवत्त रिक्त;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    शून्य
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

    सचे (*arg == NUL)
	पटिनिवत्त;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    सचे (*p == NUL)
	पटिनिवत्त;

    // past first word
    xp->xp_pattern = skipwhite(p);
    सचे (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    अञ्ञथा सचे (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    अञ्ञथा सचे (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    अञ्ञथा सचे (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    अञ्ञथा
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    स्थिर शून्य
do_cscope_general(
    exarg_T	*eap,
    पूर्णाङ्क		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    सचे (check_restricted())
	पटिनिवत्त;

    सचे ((cmdp = cs_lookup_cmd(eap)) == रिक्त)
    {
	cs_help(eap);
	पटिनिवत्त;
    }

    सचे (make_split)
    {
	सचे (!cmdp->cansplit)
	{
	    (शून्य)सन्देशः_puts(_("This cscope command does not support splitting the window.\n"));
	    पटिनिवत्त;
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
    शून्य
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    शून्य
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    शून्य
ex_cstag(exarg_T *eap)
{
    पूर्णाङ्क ret = FALSE;

    सचे (*eap->arg == NUL)
    {
	(शून्य)emsg(_(e_usage_cstag_ident));
	पटिनिवत्त;
    }

    विकल्प (p_csto)
    {
    पक्ष 0 :
	सचे (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (वर्ण *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    सचे (ret == FALSE)
	    {
		cs_free_tags();
		सचे (सन्देशः_col)
		    सन्देशः_putchar('\n');

		सचे (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	अञ्ञथा सचे (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	भङ्ग;
    पक्ष 1 :
	सचे (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    सचे (ret == FALSE)
	    {
		सचे (सन्देशः_col)
		    सन्देशः_putchar('\n');

		सचे (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (वर्ण *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    सचे (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	अञ्ञथा सचे (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (वर्ण *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    सचे (ret == FALSE)
		cs_free_tags();
	}
	भङ्ग;
    स्वभाव :
	भङ्ग;
    }

    सचे (!ret)
    {
	(शून्य)emsg(_(e_cstag_tag_not_founc));
# सचे defined(FEAT_QUICKFIX)
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
    पूर्णाङ्क
cs_fgets(char_u *buf, पूर्णाङ्क size)
{
    वर्ण *p;

    सचे ((p = cs_manage_matches(रिक्त, रिक्त, -1, Get)) == रिक्त)
	पटिनिवत्त TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    पटिनिवत्त FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    शून्य
cs_free_tags(शून्य)
{
    cs_manage_matches(रिक्त, रिक्त, -1, Free);
}


/*
 * Called from do_tag().
 */
    शून्य
cs_print_tags(शून्य)
{
    cs_manage_matches(रिक्त, रिक्त, -1, Print);
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
# सचे defined(FEAT_EVAL)
    स्थिर पूर्णाङ्क
cs_connection(पूर्णाङ्क अङ्कः, char_u *dbpath, char_u *ppath)
{
    पूर्णाङ्क i;

    सचे (अङ्कः < 0 || अङ्कः > 4 || (अङ्कः > 0 && !dbpath))
	पटिनिवत्त FALSE;

    अत्थाय (i = 0; i < csinfo_size; i++)
    {
	सचे (!csinfo[i].fname)
	    अनुवर्त;

	सचे (अङ्कः == 0)
	    पटिनिवत्त TRUE;

	विकल्प (अङ्कः)
	{
	पक्ष 1:
	    सचे (strstr(csinfo[i].fname, (वर्ण *)dbpath))
		पटिनिवत्त TRUE;
	    भङ्ग;
	पक्ष 2:
	    सचे (strcmp(csinfo[i].fname, (वर्ण *)dbpath) == 0)
		पटिनिवत्त TRUE;
	    भङ्ग;
	पक्ष 3:
	    सचे (strstr(csinfo[i].fname, (वर्ण *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (वर्ण *)ppath))))
		पटिनिवत्त TRUE;
	    भङ्ग;
	पक्ष 4:
	    सचे ((strcmp(csinfo[i].fname, (वर्ण *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (वर्ण *)ppath) == 0))))
		पटिनिवत्त TRUE;
	    भङ्ग;
	}
    }

    पटिनिवत्त FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    स्थिर पूर्णाङ्क
cs_add(exarg_T *eap UNUSED)
{
    वर्ण *fname, *ppath, *flags = रिक्त;

    सचे ((fname = strtok((वर्ण *)रिक्त, (ध्रुव वर्ण *)" ")) == रिक्त)
    {
	cs_usage_सन्देशः(Add);
	पटिनिवत्त CSCOPE_FAILURE;
    }
    सचे ((ppath = strtok((वर्ण *)रिक्त, (ध्रुव वर्ण *)" ")) != रिक्त)
	flags = strtok((वर्ण *)रिक्त, (ध्रुव वर्ण *)" ");

    पटिनिवत्त cs_add_common(fname, ppath, flags);
}

    स्थिर शून्य
cs_stat_emsg(वर्ण *fname)
{
    पूर्णाङ्क err = errno;
    (शून्य)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    स्थिर पूर्णाङ्क
cs_add_common(
    वर्ण *arg1,	    // filename - may contain environment variables
    वर्ण *arg2,	    // prepend path - may contain environment variables
    वर्ण *flags)
{
    stat_T	statbuf;
    पूर्णाङ्क		ret;
    वर्ण	*fname = रिक्त;
    वर्ण	*fname2 = रिक्त;
    वर्ण	*ppath = रिक्त;
    पूर्णाङ्क		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = रिक्त;

    // get the filename (arg1), expand it, and try to stat it
    सचे ((fname = alloc(MAXPATHL + 1)) == रिक्त)
	गच्छ add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (शून्य)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    सचे (fname == रिक्त)
	गच्छ add_err;
    fname = (वर्ण *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    सचे (ret < 0)
    {
staterr:
	सचे (p_csverbose)
	    cs_stat_emsg(fname);
	गच्छ add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    सचे (arg2 != रिक्त)
    {
	stat_T	    statbuf2;

	सचे ((ppath = alloc(MAXPATHL + 1)) == रिक्त)
	    गच्छ add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	सचे (ret < 0)
	    गच्छ staterr;
    }

    // if filename is a directory, append the cscope database name to it
    सचे (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	सचे (fname2 == रिक्त)
	    गच्छ add_err;

	याव (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    सचे (fname[0] == '\0')
		भङ्ग;
	}
	सचे (fname[0] == '\0')
	    (शून्य)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	अञ्ञथा
	    (शून्य)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	सचे (ret < 0)
	{
	    सचे (p_csverbose)
		cs_stat_emsg(fname2);
	    गच्छ add_err;
	}

	i = cs_निवेशय_filelist(fname2, ppath, flags, &statbuf);
    }
    अञ्ञथा सचे (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_निवेशय_filelist(fname, ppath, flags, &statbuf);
    }
    अञ्ञथा
    {
	सचे (p_csverbose)
	    (शून्य)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	गच्छ add_err;
    }

    सचे (i != -1)
    {
	सचे (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    गच्छ add_err;
	}

	सचे (p_csverbose)
	{
	    सन्देशः_clr_eos();
	    (शून्य)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    पटिनिवत्त CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    पटिनिवत्त CSCOPE_FAILURE;
}


    स्थिर पूर्णाङ्क
cs_check_for_connections(शून्य)
{
    पटिनिवत्त (cs_cnt_connections() > 0);
}


    स्थिर पूर्णाङ्क
cs_check_for_tags(शून्य)
{
    पटिनिवत्त (p_tags[0] != NUL && curbuf->b_p_tags != रिक्त);
}


/*
 * Count the number of cscope connections.
 */
    स्थिर पूर्णाङ्क
cs_cnt_connections(शून्य)
{
    पूर्णाङ्क i;
    पूर्णाङ्क cnt = 0;

    अत्थाय (i = 0; i < csinfo_size; i++)
	सचे (csinfo[i].fname != रिक्त)
	    cnt++;
    पटिनिवत्त cnt;
}

    स्थिर शून्य
cs_reading_emsg(
    पूर्णाङ्क idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    स्थिर पूर्णाङ्क
cs_cnt_matches(पूर्णाङ्क idx)
{
    वर्ण *stok;
    वर्ण *buf;
    पूर्णाङ्क nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    सचे (buf == रिक्त)
	पटिनिवत्त 0;
    अत्थाय (;;)
    {
	सचे (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    सचे (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    पटिनिवत्त -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	सचे (strstr((ध्रुव वर्ण *)buf, "Unable to search database") != रिक्त)
	    भङ्ग;
	सचे ((stok = strtok(buf, (ध्रुव वर्ण *)" ")) == रिक्त)
	    अनुवर्त;
	सचे (strstr((ध्रुव वर्ण *)stok, "cscope:") == रिक्त)
	    अनुवर्त;

	सचे ((stok = strtok(रिक्त, (ध्रुव वर्ण *)" ")) == रिक्त)
	    अनुवर्त;
	nlines = atoi(stok);
	सचे (nlines < 0)
	{
	    nlines = 0;
	    भङ्ग;
	}

	सचे ((stok = strtok(रिक्त, (ध्रुव वर्ण *)" ")) == रिक्त)
	    अनुवर्त;
	सचे (strncmp((ध्रुव वर्ण *)stok, "lines", 5))
	    अनुवर्त;

	भङ्ग;
    }

    vim_free(buf);
    पटिनिवत्त nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    स्थिर वर्ण *
cs_create_cmd(वर्ण *csoption, वर्ण *pattern)
{
    वर्ण *cmd;
    short अन्वेषणम्;
    वर्ण *pat;

    विकल्प (csoption[0])
    {
    पक्ष '0' : पक्ष 's' :
	अन्वेषणम् = 0;
	भङ्ग;
    पक्ष '1' : पक्ष 'g' :
	अन्वेषणम् = 1;
	भङ्ग;
    पक्ष '2' : पक्ष 'd' :
	अन्वेषणम् = 2;
	भङ्ग;
    पक्ष '3' : पक्ष 'c' :
	अन्वेषणम् = 3;
	भङ्ग;
    पक्ष '4' : पक्ष 't' :
	अन्वेषणम् = 4;
	भङ्ग;
    पक्ष '6' : पक्ष 'e' :
	अन्वेषणम् = 6;
	भङ्ग;
    पक्ष '7' : पक्ष 'f' :
	अन्वेषणम् = 7;
	भङ्ग;
    पक्ष '8' : पक्ष 'i' :
	अन्वेषणम् = 8;
	भङ्ग;
    पक्ष '9' : पक्ष 'a' :
	अन्वेषणम् = 9;
	भङ्ग;
    स्वभाव :
	(शून्य)emsg(_(e_unknown_cscope_अन्वेषणम्_type));
	cs_usage_सन्देशः(Find);
	पटिनिवत्त रिक्त;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    सचे (अन्वेषणम् != 4 && अन्वेषणम् != 6)
	याव VIM_ISWHITE(*pat)
	    ++pat;

    सचे ((cmd = alloc(strlen(pat) + 2)) == रिक्त)
	पटिनिवत्त रिक्त;

    (शून्य)sprintf(cmd, "%d%s", अन्वेषणम्, pat);

    पटिनिवत्त cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    स्थिर पूर्णाङ्क
cs_create_connection(पूर्णाङ्क i)
{
# ifdef UNIX
    पूर्णाङ्क		to_cs[2], from_cs[2];
# endif
    पूर्णाङ्क		cmdlen;
    वर्ण	*prog, *cmd, *ppath = रिक्त;
    size_t	proglen;
# ifdef MSWIN
    पूर्णाङ्क		fd;
    पूर्णाङ्क		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# सचे defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    सचे (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(शून्य)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	सचे (to_cs[0] != -1)
	    (शून्य)close(to_cs[0]);
	सचे (to_cs[1] != -1)
	    (शून्य)close(to_cs[1]);
	सचे (from_cs[0] != -1)
	    (शून्य)close(from_cs[0]);
	सचे (from_cs[1] != -1)
	    (शून्य)close(from_cs[1]);
	पटिनिवत्त CSCOPE_FAILURE;
    }

    सचे ((csinfo[i].pid = fork()) == -1)
    {
	(शून्य)emsg(_(e_could_not_fork_for_cscope));
	गच्छ err_closing;
    }
    अञ्ञथा सचे (csinfo[i].pid == 0)	// child: run cscope.
    {
	वर्ण **argv = रिक्त;

	सचे (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	सचे (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	सचे (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(शून्य)close(to_cs[1]);
	(शून्य)close(from_cs[0]);
# अञ्ञथा
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = परिमाण(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = रिक्त;

	सचे (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (शून्य)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    सचे (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    सचे (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    पटिनिवत्त CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	सचे ((prog = alloc(MAXPATHL + 1)) == रिक्त)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# अञ्ञथा
	    // MSWIN
	    गच्छ err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (पूर्णाङ्क)(proglen + strlen(csinfo[i].fname) + 32);
	सचे (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    सचे ((ppath = alloc(MAXPATHL + 1)) == रिक्त)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# अञ्ञथा
		// MSWIN
		गच्छ err_closing;
# endif
	    }
	    cmdlen += (पूर्णाङ्क)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	सचे (csinfo[i].flags)
	    cmdlen += (पूर्णाङ्क)strlen(csinfo[i].flags);

	सचे ((cmd = alloc(cmdlen)) == रिक्त)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# अञ्ञथा
	    // MSWIN
	    गच्छ err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    वर्ण	**tok = रिक्त;
	    पूर्णाङ्क		tokc = 0;

	    ga_init2(&ga_argv, परिमाण(वर्ण *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    सचे (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    अत्थाय (पूर्णाङ्क k = 0; k < tokc; ++k)
	    {
		सचे (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((वर्ण **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    सचे (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((वर्ण **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (वर्ण *)vim_strsave((char_u *)"-dl");
	    ((वर्ण **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (वर्ण *)vim_strsave((char_u *)"-f");
	    ((वर्ण **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(वर्ण *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    सचे (ppath != रिक्त)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		वर्ण	*parg = alloc(plen);

		सचे (parg == रिक्त || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((वर्ण **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    सचे (csinfo[i].flags != रिक्त)
	    {
		सचे (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		अत्थाय (पूर्णाङ्क k = 0; k < tokc; ++k)
		{
		    सचे (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((वर्ण **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    सचे (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((वर्ण **)ga_argv.ga_data)[ga_argv.ga_len] = रिक्त;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  सचे defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   सचे defined(HAVE_SETSID)
	    (शून्य)setsid();
#   अञ्ञथा
	    सचे (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (वर्ण **)ga_argv.ga_data;
	    सचे (argv[0] == रिक्त || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    अञ्ञथा	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	सचे ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == रिक्त)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	सचे ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == रिक्त)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(शून्य)close(to_cs[0]);
	(शून्य)close(from_cs[1]);
    }
# अञ्ञथा
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    सचे (csinfo[i].ppath != रिक्त)
    {
	len = (पूर्णाङ्क)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    सचे (csinfo[i].flags != रिक्त)
    {
	len = (पूर्णाङ्क)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_निवेशः  = stdin_rd;
    created = CreateProcess(रिक्त, cmd, रिक्त, रिक्त, TRUE, CREATE_NEW_CONSOLE,
							रिक्त, रिक्त, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    सचे (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(शून्य)emsg(_(e_could_not_spawn_cscope_process));
	गच्छ err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    सचे (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == रिक्त))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    सचे (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == रिक्त))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    पटिनिवत्त CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    स्थिर पूर्णाङ्क
cs_find(exarg_T *eap)
{
    वर्ण *opt, *pat;
    पूर्णाङ्क i;

    सचे (cs_check_for_connections() == FALSE)
    {
	(शून्य)emsg(_(e_no_cscope_connections));
	पटिनिवत्त FALSE;
    }

    सचे ((opt = strtok((वर्ण *)रिक्त, (ध्रुव वर्ण *)" ")) == रिक्त)
    {
	cs_usage_सन्देशः(Find);
	पटिनिवत्त FALSE;
    }

    pat = opt + strlen(opt) + 1;
    सचे (pat >= (वर्ण *)eap->arg + eap_arg_len)
    {
	cs_usage_सन्देशः(Find);
	पटिनिवत्त FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    अत्थाय (i = 0; i < eap_arg_len; ++i)
	सचे (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    पटिनिवत्त cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    स्थिर पूर्णाङ्क
cs_find_common(
    वर्ण *opt,
    वर्ण *pat,
    पूर्णाङ्क forceit,
    पूर्णाङ्क verbose,
    पूर्णाङ्क	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    पूर्णाङ्क i;
    वर्ण *cmd;
    पूर्णाङ्क *nummatches;
    पूर्णाङ्क totmatches;
# ifdef FEAT_QUICKFIX
    वर्ण cmdletter;
    वर्ण *qfpos;

    // get cmd letter
    विकल्प (opt[0])
    {
    पक्ष '0' :
	cmdletter = 's';
	भङ्ग;
    पक्ष '1' :
	cmdletter = 'g';
	भङ्ग;
    पक्ष '2' :
	cmdletter = 'd';
	भङ्ग;
    पक्ष '3' :
	cmdletter = 'c';
	भङ्ग;
    पक्ष '4' :
	cmdletter = 't';
	भङ्ग;
    पक्ष '6' :
	cmdletter = 'e';
	भङ्ग;
    पक्ष '7' :
	cmdletter = 'f';
	भङ्ग;
    पक्ष '8' :
	cmdletter = 'i';
	भङ्ग;
    पक्ष '9' :
	cmdletter = 'a';
	भङ्ग;
    स्वभाव :
	cmdletter = opt[0];
    }

    qfpos = (वर्ण *)vim_strchr(p_csqf, cmdletter);
    सचे (qfpos != रिक्त)
    {
	qfpos++;
	// next symbol must be + or -
	सचे (strchr(CSQF_FLAGS, *qfpos) == रिक्त)
	{
	    (शून्य)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    पटिनिवत्त FALSE;
	}

	सचे (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    सचे (aborting())
		पटिनिवत्त FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    सचे (cmd == रिक्त)
	पटिनिवत्त FALSE;

    nummatches = ALLOC_MULT(पूर्णाङ्क, csinfo_size);
    सचे (nummatches == रिक्त)
    {
	vim_free(cmd);
	पटिनिवत्त FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    अत्थाय (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    अत्थाय (i = 0; i < csinfo_size; i++)
    {
	सचे (csinfo[i].fname == रिक्त || csinfo[i].to_fp == रिक्त)
	    अनुवर्त;

	// send cmd to cscope
	(शून्य)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(शून्य)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	सचे (nummatches[i] > -1)
	    totmatches += nummatches[i];

	सचे (nummatches[i] == 0)
	    (शून्य)cs_read_prompt(i);
    }
    vim_free(cmd);

    सचे (totmatches == 0)
    {
	सचे (verbose)
	    (शून्य)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	पटिनिवत्त FALSE;
    }

# ifdef FEAT_QUICKFIX
    सचे (qfpos != रिक्त && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = रिक्त;
	win_T	    *wp = रिक्त;

	सचे (tmp == रिक्त)
	{
	    vim_free(nummatches);
	    पटिनिवत्त FALSE;
	}

	f = mch_fopen((वर्ण *)tmp, "w");
	सचे (f == रिक्त)
	    semsg(_(e_cant_open_file_str), tmp);
	अञ्ञथा
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    सचे (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    सचे (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, रिक्त) > 0)
	    {
		सचे (postponed_split != 0)
		{
		    (शून्य)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		सचे (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != रिक्त)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	पटिनिवत्त TRUE;
    }
    अञ्ञथा
# endif // FEAT_QUICKFIX
    {
	वर्ण **matches = रिक्त, **contexts = रिक्त;
	पूर्णाङ्क matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	सचे (matches == रिक्त)
	    पटिनिवत्त FALSE;

	(शून्य)cs_manage_matches(matches, contexts, matched, Store);

	पटिनिवत्त do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    स्थिर पूर्णाङ्क
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (शून्य)सन्देशः_puts(_("cscope commands:\n"));
    याव (cmdp->name != रिक्त)
    {
	वर्ण *help = _(cmdp->help);
	पूर्णाङ्क  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	सचे (space_cnt < 0)
	    space_cnt = 0;
	(शून्य)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	सचे (strcmp(cmdp->name, "find") == 0)
	    सन्देशः_puts(_("\n"
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
    पटिनिवत्त 0;
}


    स्थिर शून्य
clear_csinfo(पूर्णाङ्क i)
{
    csinfo[i].fname  = रिक्त;
    csinfo[i].ppath  = रिक्त;
    csinfo[i].flags  = रिक्त;
# सचे defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# अञ्ञथा
    csinfo[i].nVolume = 0;
    csinfo[i].n_Index_उच्चः = 0;
    csinfo[i].n_Index_नीचः = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = रिक्त;
    csinfo[i].to_fp  = रिक्त;
# सचे defined(MSWIN)
    csinfo[i].hProc = रिक्त;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    स्थिर पूर्णाङ्क
cs_निवेशय_filelist(
    वर्ण *fname,
    वर्ण *ppath,
    वर्ण *flags,
    stat_T *sb UNUSED)
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    विकल्प (win32_fileinfo((char_u *)fname, &bhfi))
    {
	पक्ष FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	पक्ष FILEINFO_READ_FAIL:	// CreateFile() failed
	    सचे (p_csverbose)
	    {
		वर्ण *cant_सन्देशः = _(e_cannot_open_cscope_database_str);
		वर्ण *winmsg = GetWin32Error();

		सचे (winmsg != रिक्त)
		    (शून्य)semsg(cant_सन्देशः, winmsg);
		अञ्ञथा
		    // subst filename if can't get error text
		    (शून्य)semsg(cant_सन्देशः, fname);
	    }
	    पटिनिवत्त -1;

	पक्ष FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    सचे (p_csverbose)
		(शून्य)emsg(_(e_cannot_get_cscope_database_information));
	    पटिनिवत्त -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    अत्थाय (j = 0; j < csinfo_size; j++)
    {
	सचे (csinfo[j].fname != रिक्त
# सचे defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# अञ्ञथा
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_सङ्ख्या
		    && csinfo[j].n_Index_उच्चः == bhfi.n_File_Index_उच्चः
		    && csinfo[j].n_Index_नीचः == bhfi.n_File_Index_नीचः))
# endif
	    )
	{
	    सचे (p_csverbose)
		(शून्य)emsg(_(e_duplicate_cscope_database_not_added));
	    पटिनिवत्त -1;
	}

	सचे (csinfo[j].fname == रिक्त && i == -1)
	    i = j; // remember first empty entry
    }

    सचे (i == -1)
    {
	i = csinfo_size;
	सचे (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	अञ्ञथा
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, परिमाण(csinfo_T)*csinfo_size);
	    सचे (csinfo == रिक्त)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		पटिनिवत्त -1;
	    }
	}
	सचे (csinfo == रिक्त)
	    पटिनिवत्त -1;
	अत्थाय (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    सचे ((csinfo[i].fname = alloc(strlen(fname)+1)) == रिक्त)
	पटिनिवत्त -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((ध्रुव वर्ण *)fname));

    सचे (ppath != रिक्त)
    {
	सचे ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == रिक्त)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    पटिनिवत्त -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((ध्रुव वर्ण *)ppath));
    }
    अञ्ञथा
	csinfo[i].ppath = रिक्त;

    सचे (flags != रिक्त)
    {
	सचे ((csinfo[i].flags = alloc(strlen(flags) + 1)) == रिक्त)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    पटिनिवत्त -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((ध्रुव वर्ण *)flags));
    }
    अञ्ञथा
	csinfo[i].flags = रिक्त;

# सचे defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# अञ्ञथा
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_सङ्ख्या;
    csinfo[i].n_Index_नीचः = bhfi.n_File_Index_नीचः;
    csinfo[i].n_Index_उच्चः = bhfi.n_File_Index_उच्चः;
# endif
    पटिनिवत्त i;
}


/*
 * Find cscope command in command table.
 */
    स्थिर cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    वर्ण *stok;
    size_t len;

    सचे (eap->arg == रिक्त)
	पटिनिवत्त रिक्त;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (पूर्णाङ्क)STRLEN(eap->arg);

    सचे ((stok = strtok((वर्ण *)(eap->arg), (ध्रुव वर्ण *)" ")) == रिक्त)
	पटिनिवत्त रिक्त;

    len = strlen(stok);
    अत्थाय (cmdp = cs_cmds; cmdp->name != रिक्त; ++cmdp)
    {
	सचे (strncmp((ध्रुव वर्ण *)(stok), cmdp->name, len) == 0)
	    पटिनिवत्त (cmdp);
    }
    पटिनिवत्त रिक्त;
}


/*
 * Nuke em.
 */
    स्थिर पूर्णाङ्क
cs_kill(exarg_T *eap UNUSED)
{
    वर्ण *stok;
    पूर्णाङ्क i;

    सचे ((stok = strtok((वर्ण *)रिक्त, (ध्रुव वर्ण *)" ")) == रिक्त)
    {
	cs_usage_सन्देशः(Kill);
	पटिनिवत्त CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    सचे ((strlen(stok) < 2 && VIM_ISDIGIT((पूर्णाङ्क)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((पूर्णाङ्क)(stok[1]))))
	i = atoi(stok);
    अञ्ञथा
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	अत्थाय (i = 0; i < csinfo_size; i++)
	{
	    सचे (csinfo[i].fname != रिक्त && strstr(csinfo[i].fname, stok))
		भङ्ग;
	}
    }

    सचे ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == रिक्त))
    {
	सचे (p_csverbose)
	    (शून्य)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    अञ्ञथा
    {
	सचे (i == -1)
	{
	    अत्थाय (i = 0; i < csinfo_size; i++)
	    {
		सचे (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	अञ्ञथा
	    cs_kill_execute(i, stok);
    }

    पटिनिवत्त 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    स्थिर शून्य
cs_kill_execute(
    पूर्णाङ्क i,	    // cscope table index
    वर्ण *cname)    // cscope database name
{
    सचे (p_csverbose)
    {
	सन्देशः_clr_eos();
	(शून्य)smsg_attr(HL_ATTR(HLF_R) | सन्देशः_HIST,
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
    स्थिर वर्ण *
cs_make_vim_style_matches(
    वर्ण *fname,
    वर्ण *slno,
    वर्ण *अन्वेषणम्,
    वर्ण *tagstr)
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
    वर्ण *buf;
    पूर्णाङ्क amt;

    सचे (अन्वेषणम् != रिक्त)
    {
	amt = (पूर्णाङ्क)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(अन्वेषणम्)+6);
	सचे ((buf = alloc(amt)) == रिक्त)
	    पटिनिवत्त रिक्त;

	(शून्य)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, अन्वेषणम्);
    }
    अञ्ञथा
    {
	amt = (पूर्णाङ्क)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	सचे ((buf = alloc(amt)) == रिक्त)
	    पटिनिवत्त रिक्त;

	(शून्य)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    पटिनिवत्त buf;
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
    स्थिर वर्ण *
cs_manage_matches(
    वर्ण **matches,
    वर्ण **contexts,
    पूर्णाङ्क totmatches,
    mcmd_e cmd)
{
    स्थिर वर्ण **mp = रिक्त;
    स्थिर वर्ण **cp = रिक्त;
    स्थिर पूर्णाङ्क cnt = -1;
    स्थिर पूर्णाङ्क next = -1;
    वर्ण *p = रिक्त;

    विकल्प (cmd)
    {
    पक्ष Store:
	प्रतिज्ञा(matches != रिक्त);
	प्रतिज्ञा(totmatches > 0);
	सचे (mp != रिक्त || cp != रिक्त)
	    (शून्य)cs_manage_matches(रिक्त, रिक्त, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	भङ्ग;
    पक्ष Get:
	सचे (next >= cnt)
	    पटिनिवत्त रिक्त;

	p = mp[next];
	next++;
	भङ्ग;
    पक्ष Free:
	सचे (mp != रिक्त)
	{
	    सचे (cnt > 0)
		याव (cnt--)
		{
		    vim_free(mp[cnt]);
		    सचे (cp != रिक्त)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = रिक्त;
	cp = रिक्त;
	cnt = 0;
	next = 0;
	भङ्ग;
    पक्ष Print:
	cs_print_tags_priv(mp, cp, cnt);
	भङ्ग;
    स्वभाव:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	पटिनिवत्त रिक्त;
    }

    पटिनिवत्त p;
}


/*
 * Parse cscope output.
 */
    स्थिर वर्ण *
cs_parse_results(
    पूर्णाङ्क cnumber,
    वर्ण *buf,
    पूर्णाङ्क bufsize,
    वर्ण **context,
    वर्ण **linenumber,
    वर्ण **अन्वेषणम्)
{
    पूर्णाङ्क ch;
    वर्ण *p;
    वर्ण *name;

    सचे (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == रिक्त)
    {
	सचे (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	पटिनिवत्त रिक्त;
    }

    // If the line's too long for the buffer, discard it.
    सचे ((p = strchr(buf, '\n')) == रिक्त)
    {
	याव ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	पटिनिवत्त रिक्त;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    सचे ((name = strtok(buf, (ध्रुव वर्ण *)" ")) == रिक्त)
	पटिनिवत्त रिक्त;
    सचे ((*context = strtok(रिक्त, (ध्रुव वर्ण *)" ")) == रिक्त)
	पटिनिवत्त रिक्त;
    सचे ((*linenumber = strtok(रिक्त, (ध्रुव वर्ण *)" ")) == रिक्त)
	पटिनिवत्त रिक्त;
    *अन्वेषणम् = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    सचे (strcmp(*अन्वेषणम्, "<unknown>") == 0)
	*अन्वेषणम् = रिक्त;

    name = cs_resolve_file(cnumber, name);
    पटिनिवत्त name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    स्थिर शून्य
cs_file_results(FILE *f, पूर्णाङ्क *nummatches_a)
{
    पूर्णाङ्क i, j;
    वर्ण *buf;
    वर्ण *अन्वेषणम्, *slno;
    वर्ण *fullname;
    वर्ण *cntx;
    वर्ण *context;

    buf = alloc(CSREAD_BUFSIZE);
    सचे (buf == रिक्त)
	पटिनिवत्त;

    अत्थाय (i = 0; i < csinfo_size; i++)
    {
	सचे (nummatches_a[i] < 1)
	    अनुवर्त;

	अत्थाय (j = 0; j < nummatches_a[i]; j++)
	{
	   सचे ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &अन्वेषणम्)) == रिक्त)
	       अनुवर्त;

	   context = alloc(strlen(cntx)+5);
	   सचे (context == रिक्त)
	   {
	       vim_free(fullname);
	       अनुवर्त;
	   }

	   सचे (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   अञ्ञथा
	       sprintf(context, "<<%s>>", cntx);

	   सचे (अन्वेषणम् == रिक्त)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   अञ्ञथा
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, अन्वेषणम्);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(शून्य)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    स्थिर शून्य
cs_fill_results(
    वर्ण *tagstr,
    पूर्णाङ्क totmatches,
    पूर्णाङ्क *nummatches_a,
    वर्ण ***matches_p,
    वर्ण ***cntxts_p,
    पूर्णाङ्क *matched)
{
    पूर्णाङ्क i, j;
    वर्ण *buf;
    वर्ण *अन्वेषणम्, *slno;
    पूर्णाङ्क totsofar = 0;
    वर्ण **matches = रिक्त;
    वर्ण **cntxts = रिक्त;
    वर्ण *fullname;
    वर्ण *cntx;

    प्रतिज्ञा(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    सचे (buf == रिक्त)
	पटिनिवत्त;

    सचे ((matches = ALLOC_MULT(वर्ण *, totmatches)) == रिक्त)
	गच्छ parse_out;
    सचे ((cntxts = ALLOC_MULT(वर्ण *, totmatches)) == रिक्त)
	गच्छ parse_out;

    अत्थाय (i = 0; i < csinfo_size; i++)
    {
	सचे (nummatches_a[i] < 1)
	    अनुवर्त;

	अत्थाय (j = 0; j < nummatches_a[i]; j++)
	{
	   सचे ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &अन्वेषणम्)) == रिक्त)
		अनुवर्त;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  अन्वेषणम्, tagstr);

	    vim_free(fullname);

	    सचे (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = रिक्त;
	    अञ्ञथा
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (वर्ण *)vim_strsave((char_u *)cntx);

	    सचे (matches[totsofar] != रिक्त)
		totsofar++;

	} // for all matches

	(शून्य)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    सचे (totsofar == 0)
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
    स्थिर वर्ण *
cs_pathcomponents(वर्ण *path)
{
    पूर्णाङ्क		i;
    वर्ण	*s;

    सचे (p_cspc == 0)
	पटिनिवत्त path;

    s = path + strlen(path) - 1;
    अत्थाय (i = 0; i < p_cspc; ++i)
    {
	याव (s > path)
	{
	   s--;
	   सचे (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      भङ्ग;
	}
    }
    सचे ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    पटिनिवत्त s;
}

/*
 * Called from cs_manage_matches().
 */
    स्थिर शून्य
cs_print_tags_priv(वर्ण **matches, वर्ण **cntxts, पूर्णाङ्क अङ्कः_matches)
{
    वर्ण	*buf = रिक्त;
    वर्ण	*t_buf;
    पूर्णाङ्क		bufsize = 0; // Track available bufsize
    पूर्णाङ्क		newsize = 0;
    वर्ण	*ptag;
    वर्ण	*fname, *lno, *extra, *tbuf;
    पूर्णाङ्क		i, idx, अङ्कः;
    वर्ण	*globalcntx = "GLOBAL";
    वर्ण	*cntxformat = " <<%s>>";
    वर्ण	*context;
    वर्ण	*cstag_सन्देशः = _("Cscope tag: %s");
    वर्ण	*csfmt_str = "%4d %6s  ";

    प्रतिज्ञा(अङ्कः_matches > 0);

    सचे ((tbuf = alloc(strlen(matches[0]) + 1)) == रिक्त)
	पटिनिवत्त;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    सचे (ptag == रिक्त)
    {
	vim_free(tbuf);
	पटिनिवत्त;
    }

    newsize = (पूर्णाङ्क)(strlen(cstag_सन्देशः) + strlen(ptag));
    buf = alloc(newsize);
    सचे (buf != रिक्त)
    {
	bufsize = newsize;
	(शून्य)sprintf(buf, cstag_सन्देशः, ptag);
	सन्देशः_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    सन्देशः_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    सन्देशः_advance(सन्देशः_col + 2);
    सन्देशः_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    अङ्कः = 1;
    अत्थाय (i = 0; i < अङ्कः_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	सचे ((tbuf = alloc(strlen(matches[idx]) + 1)) == रिक्त)
	    अनुवर्त;
	(शून्य)strcpy(tbuf, matches[idx]);

	सचे (strtok(tbuf, (ध्रुव वर्ण *)"\t") == रिक्त
		|| (fname = strtok(रिक्त, (ध्रुव वर्ण *)"\t")) == रिक्त
		|| (lno = strtok(रिक्त, (ध्रुव वर्ण *)"\t")) == रिक्त)
	{
	    vim_free(tbuf);
	    अनुवर्त;
	}
	extra = strtok(रिक्त, (ध्रुव वर्ण *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (पूर्णाङ्क)(strlen(csfmt_str) + 16 + strlen(lno));
	सचे (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    सचे (buf == रिक्त)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    अञ्ञथा
		bufsize = newsize;
	}
	सचे (buf != रिक्त)
	{
	    // csfmt_str = "%4d %6s  ";
	    (शून्य)sprintf(buf, csfmt_str, अङ्कः, lno);
	    सन्देशः_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	सन्देशः_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	सचे (cntxts[idx] != रिक्त)
	    context = cntxts[idx];
	अञ्ञथा
	    context = globalcntx;
	newsize = (पूर्णाङ्क)(strlen(context) + strlen(cntxformat));

	सचे (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    सचे (buf == रिक्त)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    अञ्ञथा
		bufsize = newsize;
	}
	सचे (buf != रिक्त)
	{
	    (शून्य)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    सचे (सन्देशः_col + (पूर्णाङ्क)strlen(buf) >= cmdline_width)
		सन्देशः_putchar('\n');
	    सन्देशः_advance(12);
	    सन्देशः_outtrans_long_attr((char_u *)buf, 0);
	    सन्देशः_putchar('\n');
	}
	सचे (extra != रिक्त)
	{
	    सन्देशः_advance(13);
	    सन्देशः_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	सचे (सन्देशः_col)
	    सन्देशः_putchar('\n');

	ui_breakcheck();
	सचे (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    भङ्ग;
	}

	अङ्कः++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    स्थिर पूर्णाङ्क
cs_read_prompt(पूर्णाङ्क i)
{
    पूर्णाङ्क		ch;
    वर्ण	*buf = रिक्त; // buffer for possible error message from cscope
    पूर्णाङ्क		bufpos = 0;
    पूर्णाङ्क		maxlen;
    स्थिर वर्ण *eprompt = "Press the RETURN key to continue:";
    पूर्णाङ्क		epromptlen = (पूर्णाङ्क)strlen(eprompt);
    पूर्णाङ्क		n;

    // compute maximum allowed len for Cscope error message
    वर्ण *cs_emsg = _(e_cscope_error_str);
    maxlen = (पूर्णाङ्क)(IOSIZE - strlen(cs_emsg));

    अत्थाय (;;)
    {
	याव ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    सचे (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		सचे (buf == रिक्त) // lazy buffer allocation
		    buf = alloc(maxlen);
		सचे (buf != रिक्त)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    सचे (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(शून्य)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(शून्य)putc('\n', csinfo[i].to_fp);
			(शून्य)fflush(csinfo[i].to_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	अत्थाय (n = 0; n < (पूर्णाङ्क)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    सचे (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    सचे (ch == EOF)
	    {
		सचे (buf != रिक्त && buf[0] != NUL)
		    (शून्य)semsg(cs_emsg, buf);
		अञ्ञथा सचे (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		पटिनिवत्त CSCOPE_FAILURE;
	    }

	    सचे (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		भङ्ग;
	    }
	}

	सचे (ch == EOF)
	    अनुवर्त;	    // didn't find the prompt
	भङ्ग;		    // did find the prompt
    }

    vim_free(buf);
    पटिनिवत्त CSCOPE_SUCCESS;
}

# सचे defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    स्थिर शून्य
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    स्थिर शून्य
cs_release_csp(पूर्णाङ्क i, पूर्णाङ्क freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    सचे (csinfo[i].to_fp != रिक्त)
    {
	(शून्य)fputs("q\n", csinfo[i].to_fp);
	(शून्य)fflush(csinfo[i].to_fp);
    }
# सचे defined(UNIX)
    {
	पूर्णाङ्क waitpid_errno;
	पूर्णाङ्क pstat;
	pid_t pid;

#  सचे defined(HAVE_SIGACTION)
	संरचना sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   अञ्ञथा
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, रिक्त);
#  अञ्ञथा
	पूर्णाङ्क waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	अत्थाय (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    सचे (pid != 0)
		भङ्ग;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	सचे (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    पूर्णाङ्क alive = TRUE;

	    सचे (waitpid_errno == ECHILD)
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
		पूर्णाङ्क waited;

		sleep(0);
		अत्थाय (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    सचे (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			भङ्ग;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    सचे (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(शून्य)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# अञ्ञथा  // !UNIX
    सचे (csinfo[i].hProc != रिक्त)
    {
	// Give cscope a chance to exit normally
	सचे (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    सचे (csinfo[i].fr_fp != रिक्त)
	(शून्य)fclose(csinfo[i].fr_fp);
    सचे (csinfo[i].to_fp != रिक्त)
	(शून्य)fclose(csinfo[i].to_fp);

    सचे (freefnpp)
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
    स्थिर पूर्णाङ्क
cs_reset(exarg_T *eap UNUSED)
{
    वर्ण	**dblist = रिक्त, **pplist = रिक्त, **fllist = रिक्त;
    पूर्णाङ्क	i;
    वर्ण buf[20]; // for sprintf " (#%d)"

    सचे (csinfo_size == 0)
	पटिनिवत्त CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(वर्ण *, csinfo_size);
    pplist = ALLOC_MULT(वर्ण *, csinfo_size);
    fllist = ALLOC_MULT(वर्ण *, csinfo_size);
    सचे (dblist == रिक्त || pplist == रिक्त || fllist == रिक्त)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	पटिनिवत्त CSCOPE_FAILURE;
    }

    अत्थाय (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	सचे (csinfo[i].fname != रिक्त)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    अत्थाय (i = 0; i < csinfo_size; i++)
    {
	सचे (dblist[i] != रिक्त)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    सचे (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		सन्देशः_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    सचे (p_csverbose)
	सन्देशः_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | सन्देशः_HIST);
    पटिनिवत्त CSCOPE_SUCCESS;
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
    स्थिर वर्ण *
cs_resolve_file(पूर्णाङ्क i, वर्ण *name)
{
    वर्ण	*fullname;
    string_T	csdir = {रिक्त, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    पूर्णाङ्क		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (पूर्णाङ्क)namelen + 2;
    सचे (csinfo[i].ppath != रिक्त)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (पूर्णाङ्क)ppathlen;
    }
    अञ्ञथा सचे (p_csre && csinfo[i].fname != रिक्त)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	सचे (csdir.string != रिक्त)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (पूर्णाङ्क)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    सचे (csinfo[i].ppath != रिक्त
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	सचे ((fullname = alloc(len)) != रिक्त)
	    (शून्य)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    अञ्ञथा सचे (csdir.string != रिक्त && csinfo[i].fname != रिक्त && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (वर्ण *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    अञ्ञथा
    {
	fullname = (वर्ण *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    पटिनिवत्त fullname;
}


/*
 * Show all cscope connections.
 */
    स्थिर पूर्णाङ्क
cs_show(exarg_T *eap UNUSED)
{
    पूर्णाङ्क i;

    सचे (cs_cnt_connections() == 0)
	सन्देशः_puts(_("no cscope connections\n"));
    अञ्ञथा
    {
	सन्देशः_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	अत्थाय (i = 0; i < csinfo_size; i++)
	{
	    सचे (csinfo[i].fname == रिक्त)
		अनुवर्त;

	    सचे (csinfo[i].ppath != रिक्त)
		(शून्य)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    अञ्ञथा
		(शून्य)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    पटिनिवत्त CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    शून्य
cs_end(शून्य)
{
    पूर्णाङ्क i;

    अत्थाय (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#सचे defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    शून्य
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    पूर्णाङ्क		अङ्कः = 0;
    char_u	*dbpath = रिक्त;
    char_u	*prepend = रिक्त;
    char_u	buf[NUMBUFLEN];

    सचे (in_vim9script()
	    && (check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	पटिनिवत्त;

    सचे (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	अङ्कः = (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	सचे (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_सङ्ख्या = cs_connection(अङ्कः, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
