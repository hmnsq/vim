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

#जर defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# जर defined(UNIX)
#  include <sys/wait.h>
# endif

# जर defined (MSWIN)
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

प्रकारव्याख्या संरचना {
    अक्षर *  name;
    पूर्णांक     (*func)(exarg_T *eap);
    अक्षर *  help;
    अक्षर *  usage;
    पूर्णांक	    cansplit;		// if supports splitting window
} cscmd_T;

प्रकारव्याख्या संरचना csi {
    अक्षर *	    fname;	// cscope db name
    अक्षर *	    ppath;	// path to prepend (the -P option)
    अक्षर *	    flags;	// additional cscope flags/options (e.g, -p2)
# जर defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# अन्यथा
#  जर defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_Index_वरचा;	// st_ino has no meaning in the Windows
    DWORD	    n_Index_खालचा;
#  endif
# endif

    खाता *	    fr_fp;	// from cscope: FILE.
    खाता *	    to_fp;	// to cscope: FILE.
} csinfo_T;

प्रकारव्याख्या गणना { Add, Find, Help, Kill, Reset, Show } csid_e;

प्रकारव्याख्या गणना {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

स्थिर पूर्णांक	    cs_add(exarg_T *eap);
स्थिर पूर्णांक	    cs_add_common(अक्षर *, अक्षर *, अक्षर *);
स्थिर पूर्णांक	    cs_check_for_connections(शून्य);
स्थिर पूर्णांक	    cs_check_for_tags(शून्य);
स्थिर पूर्णांक	    cs_cnt_connections(शून्य);
स्थिर पूर्णांक	    cs_create_connection(पूर्णांक i);
# ifdef FEAT_QUICKFIX
स्थिर शून्य	    cs_file_results(खाता *, पूर्णांक *);
# endif
स्थिर शून्य	    cs_fill_results(अक्षर *, पूर्णांक , पूर्णांक *, अक्षर ***,
			अक्षर ***, पूर्णांक *);
स्थिर पूर्णांक	    cs_find(exarg_T *eap);
स्थिर पूर्णांक	    cs_find_common(अक्षर *opt, अक्षर *pat, पूर्णांक, पूर्णांक, पूर्णांक, char_u *cmdline);
स्थिर पूर्णांक	    cs_help(exarg_T *eap);
स्थिर पूर्णांक	    cs_घाला_filelist(अक्षर *, अक्षर *, अक्षर *,
			stat_T *);
स्थिर पूर्णांक	    cs_kill(exarg_T *eap);
स्थिर शून्य	    cs_kill_execute(पूर्णांक, अक्षर *);
स्थिर cscmd_T *    cs_lookup_cmd(exarg_T *eap);
स्थिर अक्षर *	    cs_make_vim_style_matches(अक्षर *, अक्षर *,
			अक्षर *, अक्षर *);
स्थिर अक्षर *	    cs_manage_matches(अक्षर **, अक्षर **, पूर्णांक, mcmd_e);
स्थिर शून्य	    cs_print_tags_priv(अक्षर **, अक्षर **, पूर्णांक);
स्थिर पूर्णांक	    cs_read_prompt(पूर्णांक);
स्थिर शून्य	    cs_release_csp(पूर्णांक, पूर्णांक freefnpp);
स्थिर पूर्णांक	    cs_reset(exarg_T *eap);
स्थिर अक्षर *	    cs_resolve_file(पूर्णांक, अक्षर *);
स्थिर पूर्णांक	    cs_show(exarg_T *eap);


स्थिर csinfo_T *   csinfo = रिक्त;
स्थिर पूर्णांक	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

स्थिर पूर्णांक	    eap_arg_len;    // length of eap->arg, set in
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
cs_usage_संदेश(csid_e x)
{
    (शून्य)semsg(_(e_usage_cscope_str), cs_cmds[(पूर्णांक)x].usage);
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
get_cscope_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    पूर्णांक		current_idx;
    पूर्णांक		i;

    निवड (expand_what)
    {
    स्थिती EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	परत (char_u *)cs_cmds[idx].name;
    स्थिती EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	साठी (i = 0, current_idx = 0; cs_cmds[i].name != रिक्त; i++)
	    जर (cs_cmds[i].cansplit)
		जर (current_idx++ == idx)
		    थांबा;
	परत (char_u *)cs_cmds[i].name;
    स्थिती EXP_CSCOPE_FIND:
	{
	    स्थिरांक अक्षर *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", रिक्त
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    परत (char_u *)query_type[idx];
	}
    स्थिती EXP_CSCOPE_KILL:
	{
	    स्थिर अक्षर	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    साठी (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		जर (csinfo[i].fname == रिक्त)
		    सुरूठेवा;
		जर (current_idx++ == idx)
		{
		    vim_snprintf(connection, आकार(connection), "%d", i);
		    परत (char_u *)connection;
		}
	    }
	    परत (current_idx == idx && idx > 0) ? (char_u *)"-1" : रिक्त;
	}
    पूर्वनिर्धारित:
	परत रिक्त;
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

    जर (*arg == NUL)
	परत;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    जर (*p == NUL)
	परत;

    // past first word
    xp->xp_pattern = skipwhite(p);
    जर (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    अन्यथा जर (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    अन्यथा जर (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    अन्यथा जर (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    अन्यथा
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    स्थिर शून्य
do_cscope_general(
    exarg_T	*eap,
    पूर्णांक		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    जर (check_restricted())
	परत;

    जर ((cmdp = cs_lookup_cmd(eap)) == रिक्त)
    {
	cs_help(eap);
	परत;
    }

    जर (make_split)
    {
	जर (!cmdp->cansplit)
	{
	    (शून्य)संदेश_puts(_("This cscope command does not support splitting the window.\n"));
	    परत;
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
    पूर्णांक ret = FALSE;

    जर (*eap->arg == NUL)
    {
	(शून्य)emsg(_(e_usage_cstag_ident));
	परत;
    }

    निवड (p_csto)
    {
    स्थिती 0 :
	जर (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (अक्षर *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    जर (ret == FALSE)
	    {
		cs_free_tags();
		जर (संदेश_col)
		    संदेश_putchar('\n');

		जर (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	अन्यथा जर (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	थांबा;
    स्थिती 1 :
	जर (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    जर (ret == FALSE)
	    {
		जर (संदेश_col)
		    संदेश_putchar('\n');

		जर (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (अक्षर *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    जर (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	अन्यथा जर (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (अक्षर *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    जर (ret == FALSE)
		cs_free_tags();
	}
	थांबा;
    पूर्वनिर्धारित :
	थांबा;
    }

    जर (!ret)
    {
	(शून्य)emsg(_(e_cstag_tag_not_founc));
# जर defined(FEAT_QUICKFIX)
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
    पूर्णांक
cs_fgets(char_u *buf, पूर्णांक size)
{
    अक्षर *p;

    जर ((p = cs_manage_matches(रिक्त, रिक्त, -1, Get)) == रिक्त)
	परत TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    परत FALSE;
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
# जर defined(FEAT_EVAL)
    स्थिर पूर्णांक
cs_connection(पूर्णांक अंक, char_u *dbpath, char_u *ppath)
{
    पूर्णांक i;

    जर (अंक < 0 || अंक > 4 || (अंक > 0 && !dbpath))
	परत FALSE;

    साठी (i = 0; i < csinfo_size; i++)
    {
	जर (!csinfo[i].fname)
	    सुरूठेवा;

	जर (अंक == 0)
	    परत TRUE;

	निवड (अंक)
	{
	स्थिती 1:
	    जर (म_माला(csinfo[i].fname, (अक्षर *)dbpath))
		परत TRUE;
	    थांबा;
	स्थिती 2:
	    जर (म_भेद(csinfo[i].fname, (अक्षर *)dbpath) == 0)
		परत TRUE;
	    थांबा;
	स्थिती 3:
	    जर (म_माला(csinfo[i].fname, (अक्षर *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && म_माला(csinfo[i].ppath, (अक्षर *)ppath))))
		परत TRUE;
	    थांबा;
	स्थिती 4:
	    जर ((म_भेद(csinfo[i].fname, (अक्षर *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (म_भेद(csinfo[i].ppath, (अक्षर *)ppath) == 0))))
		परत TRUE;
	    थांबा;
	}
    }

    परत FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    स्थिर पूर्णांक
cs_add(exarg_T *eap UNUSED)
{
    अक्षर *fname, *ppath, *flags = रिक्त;

    जर ((fname = म_मोहर((अक्षर *)रिक्त, (स्थिरांक अक्षर *)" ")) == रिक्त)
    {
	cs_usage_संदेश(Add);
	परत CSCOPE_FAILURE;
    }
    जर ((ppath = म_मोहर((अक्षर *)रिक्त, (स्थिरांक अक्षर *)" ")) != रिक्त)
	flags = म_मोहर((अक्षर *)रिक्त, (स्थिरांक अक्षर *)" ");

    परत cs_add_common(fname, ppath, flags);
}

    स्थिर शून्य
cs_stat_emsg(अक्षर *fname)
{
    पूर्णांक err = त्रुटि_सं;
    (शून्य)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    स्थिर पूर्णांक
cs_add_common(
    अक्षर *arg1,	    // filename - may contain environment variables
    अक्षर *arg2,	    // prepend path - may contain environment variables
    अक्षर *flags)
{
    stat_T	statbuf;
    पूर्णांक		ret;
    अक्षर	*fname = रिक्त;
    अक्षर	*fname2 = रिक्त;
    अक्षर	*ppath = रिक्त;
    पूर्णांक		i;
    माप_प्रकार	len;
    माप_प्रकार	usedlen = 0;
    char_u	*fbuf = रिक्त;

    // get the filename (arg1), expand it, and try to stat it
    जर ((fname = alloc(MAXPATHL + 1)) == रिक्त)
	जा add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (शून्य)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    जर (fname == रिक्त)
	जा add_err;
    fname = (अक्षर *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    जर (ret < 0)
    {
staterr:
	जर (p_csverbose)
	    cs_stat_emsg(fname);
	जा add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    जर (arg2 != रिक्त)
    {
	stat_T	    statbuf2;

	जर ((ppath = alloc(MAXPATHL + 1)) == रिक्त)
	    जा add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	जर (ret < 0)
	    जा staterr;
    }

    // if filename is a directory, append the cscope database name to it
    जर (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + म_माप(fname) + 2);
	जर (fname2 == रिक्त)
	    जा add_err;

	पर्यंत (fname[म_माप(fname)-1] == '/'
# ifdef MSWIN
		|| fname[म_माप(fname)-1] == '\\'
# endif
		)
	{
	    fname[म_माप(fname)-1] = '\0';
	    जर (fname[0] == '\0')
		थांबा;
	}
	जर (fname[0] == '\0')
	    (शून्य)प्र_लिखो(fname2, "/%s", CSCOPE_DBFILE);
	अन्यथा
	    (शून्य)प्र_लिखो(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	जर (ret < 0)
	{
	    जर (p_csverbose)
		cs_stat_emsg(fname2);
	    जा add_err;
	}

	i = cs_घाला_filelist(fname2, ppath, flags, &statbuf);
    }
    अन्यथा जर (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_घाला_filelist(fname, ppath, flags, &statbuf);
    }
    अन्यथा
    {
	जर (p_csverbose)
	    (शून्य)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	जा add_err;
    }

    जर (i != -1)
    {
	जर (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    जा add_err;
	}

	जर (p_csverbose)
	{
	    संदेश_clr_eos();
	    (शून्य)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    परत CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    परत CSCOPE_FAILURE;
}


    स्थिर पूर्णांक
cs_check_for_connections(शून्य)
{
    परत (cs_cnt_connections() > 0);
}


    स्थिर पूर्णांक
cs_check_for_tags(शून्य)
{
    परत (p_tags[0] != NUL && curbuf->b_p_tags != रिक्त);
}


/*
 * Count the number of cscope connections.
 */
    स्थिर पूर्णांक
cs_cnt_connections(शून्य)
{
    पूर्णांक i;
    पूर्णांक cnt = 0;

    साठी (i = 0; i < csinfo_size; i++)
	जर (csinfo[i].fname != रिक्त)
	    cnt++;
    परत cnt;
}

    स्थिर शून्य
cs_reading_emsg(
    पूर्णांक idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    स्थिर पूर्णांक
cs_cnt_matches(पूर्णांक idx)
{
    अक्षर *stok;
    अक्षर *buf;
    पूर्णांक nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    जर (buf == रिक्त)
	परत 0;
    साठी (;;)
    {
	जर (!ख_माला_लो(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    जर (ख_पूर्ण(csinfo[idx].fr_fp))
		त्रुटि_सं = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    परत -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	जर (म_माला((स्थिरांक अक्षर *)buf, "Unable to search database") != रिक्त)
	    थांबा;
	जर ((stok = म_मोहर(buf, (स्थिरांक अक्षर *)" ")) == रिक्त)
	    सुरूठेवा;
	जर (म_माला((स्थिरांक अक्षर *)stok, "cscope:") == रिक्त)
	    सुरूठेवा;

	जर ((stok = म_मोहर(रिक्त, (स्थिरांक अक्षर *)" ")) == रिक्त)
	    सुरूठेवा;
	nlines = म_से_प(stok);
	जर (nlines < 0)
	{
	    nlines = 0;
	    थांबा;
	}

	जर ((stok = म_मोहर(रिक्त, (स्थिरांक अक्षर *)" ")) == रिक्त)
	    सुरूठेवा;
	जर (म_भेदन((स्थिरांक अक्षर *)stok, "lines", 5))
	    सुरूठेवा;

	थांबा;
    }

    vim_free(buf);
    परत nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    स्थिर अक्षर *
cs_create_cmd(अक्षर *csoption, अक्षर *pattern)
{
    अक्षर *cmd;
    लघु शोध;
    अक्षर *pat;

    निवड (csoption[0])
    {
    स्थिती '0' : स्थिती 's' :
	शोध = 0;
	थांबा;
    स्थिती '1' : स्थिती 'g' :
	शोध = 1;
	थांबा;
    स्थिती '2' : स्थिती 'd' :
	शोध = 2;
	थांबा;
    स्थिती '3' : स्थिती 'c' :
	शोध = 3;
	थांबा;
    स्थिती '4' : स्थिती 't' :
	शोध = 4;
	थांबा;
    स्थिती '6' : स्थिती 'e' :
	शोध = 6;
	थांबा;
    स्थिती '7' : स्थिती 'f' :
	शोध = 7;
	थांबा;
    स्थिती '8' : स्थिती 'i' :
	शोध = 8;
	थांबा;
    स्थिती '9' : स्थिती 'a' :
	शोध = 9;
	थांबा;
    पूर्वनिर्धारित :
	(शून्य)emsg(_(e_unknown_cscope_शोध_type));
	cs_usage_संदेश(Find);
	परत रिक्त;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    जर (शोध != 4 && शोध != 6)
	पर्यंत VIM_ISWHITE(*pat)
	    ++pat;

    जर ((cmd = alloc(म_माप(pat) + 2)) == रिक्त)
	परत रिक्त;

    (शून्य)प्र_लिखो(cmd, "%d%s", शोध, pat);

    परत cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    स्थिर पूर्णांक
cs_create_connection(पूर्णांक i)
{
# ifdef UNIX
    पूर्णांक		to_cs[2], from_cs[2];
# endif
    पूर्णांक		cmdlen;
    अक्षर	*prog, *cmd, *ppath = रिक्त;
    माप_प्रकार	proglen;
# ifdef MSWIN
    पूर्णांक		fd;
    पूर्णांक		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# जर defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    जर (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(शून्य)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	जर (to_cs[0] != -1)
	    (शून्य)बंद(to_cs[0]);
	जर (to_cs[1] != -1)
	    (शून्य)बंद(to_cs[1]);
	जर (from_cs[0] != -1)
	    (शून्य)बंद(from_cs[0]);
	जर (from_cs[1] != -1)
	    (शून्य)बंद(from_cs[1]);
	परत CSCOPE_FAILURE;
    }

    जर ((csinfo[i].pid = विभाजन()) == -1)
    {
	(शून्य)emsg(_(e_could_not_fork_for_cscope));
	जा err_closing;
    }
    अन्यथा जर (csinfo[i].pid == 0)	// child: run cscope.
    {
	अक्षर **argv = रिक्त;

	जर (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	जर (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	जर (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(शून्य)बंद(to_cs[1]);
	(शून्य)बंद(from_cs[0]);
# अन्यथा
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = आकार(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = रिक्त;

	जर (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (शून्य)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    जर (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    जर (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    परत CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	जर ((prog = alloc(MAXPATHL + 1)) == रिक्त)
	{
# ifdef UNIX
	    निकास(निकास_त्रुटि);
# अन्यथा
	    // MSWIN
	    जा err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (पूर्णांक)(proglen + म_माप(csinfo[i].fname) + 32);
	जर (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    जर ((ppath = alloc(MAXPATHL + 1)) == रिक्त)
	    {
		vim_free(prog);
# ifdef UNIX
		निकास(निकास_त्रुटि);
# अन्यथा
		// MSWIN
		जा err_closing;
# endif
	    }
	    cmdlen += (पूर्णांक)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	जर (csinfo[i].flags)
	    cmdlen += (पूर्णांक)म_माप(csinfo[i].flags);

	जर ((cmd = alloc(cmdlen)) == रिक्त)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    निकास(निकास_त्रुटि);
# अन्यथा
	    // MSWIN
	    जा err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    अक्षर	**tok = रिक्त;
	    पूर्णांक		tokc = 0;

	    ga_init2(&ga_argv, आकार(अक्षर *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    जर (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		निकास(निकास_त्रुटि);
	    साठी (पूर्णांक k = 0; k < tokc; ++k)
	    {
		जर (ga_grow(&ga_argv, 1) == FAIL)
		    निकास(निकास_त्रुटि);
		((अक्षर **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    जर (ga_grow(&ga_argv, 3) == FAIL)
		निकास(निकास_त्रुटि);
	    ((अक्षर **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (अक्षर *)vim_strsave((char_u *)"-dl");
	    ((अक्षर **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (अक्षर *)vim_strsave((char_u *)"-f");
	    ((अक्षर **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(अक्षर *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    जर (ppath != रिक्त)
	    {
		माप_प्रकार	plen = STRLEN(ppath) + 3;
		अक्षर	*parg = alloc(plen);

		जर (parg == रिक्त || ga_grow(&ga_argv, 1) == FAIL)
		    निकास(निकास_त्रुटि);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((अक्षर **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    जर (csinfo[i].flags != रिक्त)
	    {
		जर (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    निकास(निकास_त्रुटि);
		साठी (पूर्णांक k = 0; k < tokc; ++k)
		{
		    जर (ga_grow(&ga_argv, 1) == FAIL)
			निकास(निकास_त्रुटि);
		    ((अक्षर **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    जर (ga_grow(&ga_argv, 1) == FAIL)
		निकास(निकास_त्रुटि);
	    ((अक्षर **)ga_argv.ga_data)[ga_argv.ga_len] = रिक्त;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  जर defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   जर defined(HAVE_SETSID)
	    (शून्य)setsid();
#   अन्यथा
	    जर (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (अक्षर **)ga_argv.ga_data;
	    जर (argv[0] == रिक्त || execvp(argv[0], argv) == -1)
	    {
		ख_लिखो(मानक_त्रुटि, "%s\n",
		    _("cs_create_connection exec failed"));
		ख_साफ(मानक_त्रुटि);
	    }

	    निकास(127);
	    // NOTREACHED
	}
    }
    अन्यथा	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	जर ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == रिक्त)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	जर ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == रिक्त)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(शून्य)बंद(to_cs[0]);
	(शून्य)बंद(from_cs[1]);
    }
# अन्यथा
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    जर (csinfo[i].ppath != रिक्त)
    {
	len = (पूर्णांक)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    जर (csinfo[i].flags != रिक्त)
    {
	len = (पूर्णांक)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_आदान  = stdin_rd;
    created = CreateProcess(रिक्त, cmd, रिक्त, रिक्त, TRUE, CREATE_NEW_CONSOLE,
							रिक्त, रिक्त, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    जर (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(शून्य)emsg(_(e_could_not_spawn_cscope_process));
	जा err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    जर (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == रिक्त))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    जर (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == रिक्त))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    परत CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    स्थिर पूर्णांक
cs_find(exarg_T *eap)
{
    अक्षर *opt, *pat;
    पूर्णांक i;

    जर (cs_check_for_connections() == FALSE)
    {
	(शून्य)emsg(_(e_no_cscope_connections));
	परत FALSE;
    }

    जर ((opt = म_मोहर((अक्षर *)रिक्त, (स्थिरांक अक्षर *)" ")) == रिक्त)
    {
	cs_usage_संदेश(Find);
	परत FALSE;
    }

    pat = opt + म_माप(opt) + 1;
    जर (pat >= (अक्षर *)eap->arg + eap_arg_len)
    {
	cs_usage_संदेश(Find);
	परत FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    साठी (i = 0; i < eap_arg_len; ++i)
	जर (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    परत cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    स्थिर पूर्णांक
cs_find_common(
    अक्षर *opt,
    अक्षर *pat,
    पूर्णांक forceit,
    पूर्णांक verbose,
    पूर्णांक	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    पूर्णांक i;
    अक्षर *cmd;
    पूर्णांक *nummatches;
    पूर्णांक totmatches;
# ifdef FEAT_QUICKFIX
    अक्षर cmdletter;
    अक्षर *qfpos;

    // get cmd letter
    निवड (opt[0])
    {
    स्थिती '0' :
	cmdletter = 's';
	थांबा;
    स्थिती '1' :
	cmdletter = 'g';
	थांबा;
    स्थिती '2' :
	cmdletter = 'd';
	थांबा;
    स्थिती '3' :
	cmdletter = 'c';
	थांबा;
    स्थिती '4' :
	cmdletter = 't';
	थांबा;
    स्थिती '6' :
	cmdletter = 'e';
	थांबा;
    स्थिती '7' :
	cmdletter = 'f';
	थांबा;
    स्थिती '8' :
	cmdletter = 'i';
	थांबा;
    स्थिती '9' :
	cmdletter = 'a';
	थांबा;
    पूर्वनिर्धारित :
	cmdletter = opt[0];
    }

    qfpos = (अक्षर *)vim_strchr(p_csqf, cmdletter);
    जर (qfpos != रिक्त)
    {
	qfpos++;
	// next symbol must be + or -
	जर (म_अक्षर(CSQF_FLAGS, *qfpos) == रिक्त)
	{
	    (शून्य)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    परत FALSE;
	}

	जर (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    जर (aborting())
		परत FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    जर (cmd == रिक्त)
	परत FALSE;

    nummatches = ALLOC_MULT(पूर्णांक, csinfo_size);
    जर (nummatches == रिक्त)
    {
	vim_free(cmd);
	परत FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    साठी (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    साठी (i = 0; i < csinfo_size; i++)
    {
	जर (csinfo[i].fname == रिक्त || csinfo[i].to_fp == रिक्त)
	    सुरूठेवा;

	// send cmd to cscope
	(शून्य)ख_लिखो(csinfo[i].to_fp, "%s\n", cmd);
	(शून्य)ख_साफ(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	जर (nummatches[i] > -1)
	    totmatches += nummatches[i];

	जर (nummatches[i] == 0)
	    (शून्य)cs_read_prompt(i);
    }
    vim_free(cmd);

    जर (totmatches == 0)
    {
	जर (verbose)
	    (शून्य)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	परत FALSE;
    }

# ifdef FEAT_QUICKFIX
    जर (qfpos != रिक्त && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	खाता	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = रिक्त;
	win_T	    *wp = रिक्त;

	जर (tmp == रिक्त)
	{
	    vim_free(nummatches);
	    परत FALSE;
	}

	f = mch_fopen((अक्षर *)tmp, "w");
	जर (f == रिक्त)
	    semsg(_(e_cant_open_file_str), tmp);
	अन्यथा
	{
	    cs_file_results(f, nummatches);
	    ख_बंद(f);
	    जर (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    जर (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, रिक्त) > 0)
	    {
		जर (postponed_split != 0)
		{
		    (शून्य)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		जर (use_ll)
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
	परत TRUE;
    }
    अन्यथा
# endif // FEAT_QUICKFIX
    {
	अक्षर **matches = रिक्त, **contexts = रिक्त;
	पूर्णांक matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	जर (matches == रिक्त)
	    परत FALSE;

	(शून्य)cs_manage_matches(matches, contexts, matched, Store);

	परत do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    स्थिर पूर्णांक
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (शून्य)संदेश_puts(_("cscope commands:\n"));
    पर्यंत (cmdp->name != रिक्त)
    {
	अक्षर *help = _(cmdp->help);
	पूर्णांक  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	जर (space_cnt < 0)
	    space_cnt = 0;
	(शून्य)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	जर (म_भेद(cmdp->name, "find") == 0)
	    संदेश_puts(_("\n"
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
    परत 0;
}


    स्थिर शून्य
clear_csinfo(पूर्णांक i)
{
    csinfo[i].fname  = रिक्त;
    csinfo[i].ppath  = रिक्त;
    csinfo[i].flags  = रिक्त;
# जर defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# अन्यथा
    csinfo[i].nVolume = 0;
    csinfo[i].n_Index_वरचा = 0;
    csinfo[i].n_Index_खालचा = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = रिक्त;
    csinfo[i].to_fp  = रिक्त;
# जर defined(MSWIN)
    csinfo[i].hProc = रिक्त;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    स्थिर पूर्णांक
cs_घाला_filelist(
    अक्षर *fname,
    अक्षर *ppath,
    अक्षर *flags,
    stat_T *sb UNUSED)
{
    पूर्णांक	    i;
    पूर्णांक	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    निवड (win32_fileinfo((char_u *)fname, &bhfi))
    {
	स्थिती FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	स्थिती FILEINFO_READ_FAIL:	// CreateFile() failed
	    जर (p_csverbose)
	    {
		अक्षर *cant_संदेश = _(e_cannot_open_cscope_database_str);
		अक्षर *winmsg = GetWin32Error();

		जर (winmsg != रिक्त)
		    (शून्य)semsg(cant_संदेश, winmsg);
		अन्यथा
		    // subst filename if can't get error text
		    (शून्य)semsg(cant_संदेश, fname);
	    }
	    परत -1;

	स्थिती FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    जर (p_csverbose)
		(शून्य)emsg(_(e_cannot_get_cscope_database_information));
	    परत -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    साठी (j = 0; j < csinfo_size; j++)
    {
	जर (csinfo[j].fname != रिक्त
# जर defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# अन्यथा
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_संख्या
		    && csinfo[j].n_Index_वरचा == bhfi.n_File_Index_वरचा
		    && csinfo[j].n_Index_खालचा == bhfi.n_File_Index_खालचा))
# endif
	    )
	{
	    जर (p_csverbose)
		(शून्य)emsg(_(e_duplicate_cscope_database_not_added));
	    परत -1;
	}

	जर (csinfo[j].fname == रिक्त && i == -1)
	    i = j; // remember first empty entry
    }

    जर (i == -1)
    {
	i = csinfo_size;
	जर (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	अन्यथा
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, आकार(csinfo_T)*csinfo_size);
	    जर (csinfo == रिक्त)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		परत -1;
	    }
	}
	जर (csinfo == रिक्त)
	    परत -1;
	साठी (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    जर ((csinfo[i].fname = alloc(म_माप(fname)+1)) == रिक्त)
	परत -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, म_माप((स्थिरांक अक्षर *)fname));

    जर (ppath != रिक्त)
    {
	जर ((csinfo[i].ppath = alloc(म_माप(ppath) + 1)) == रिक्त)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    परत -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, म_माप((स्थिरांक अक्षर *)ppath));
    }
    अन्यथा
	csinfo[i].ppath = रिक्त;

    जर (flags != रिक्त)
    {
	जर ((csinfo[i].flags = alloc(म_माप(flags) + 1)) == रिक्त)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    परत -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, म_माप((स्थिरांक अक्षर *)flags));
    }
    अन्यथा
	csinfo[i].flags = रिक्त;

# जर defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# अन्यथा
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_संख्या;
    csinfo[i].n_Index_खालचा = bhfi.n_File_Index_खालचा;
    csinfo[i].n_Index_वरचा = bhfi.n_File_Index_वरचा;
# endif
    परत i;
}


/*
 * Find cscope command in command table.
 */
    स्थिर cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    अक्षर *stok;
    माप_प्रकार len;

    जर (eap->arg == रिक्त)
	परत रिक्त;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (पूर्णांक)STRLEN(eap->arg);

    जर ((stok = म_मोहर((अक्षर *)(eap->arg), (स्थिरांक अक्षर *)" ")) == रिक्त)
	परत रिक्त;

    len = म_माप(stok);
    साठी (cmdp = cs_cmds; cmdp->name != रिक्त; ++cmdp)
    {
	जर (म_भेदन((स्थिरांक अक्षर *)(stok), cmdp->name, len) == 0)
	    परत (cmdp);
    }
    परत रिक्त;
}


/*
 * Nuke em.
 */
    स्थिर पूर्णांक
cs_kill(exarg_T *eap UNUSED)
{
    अक्षर *stok;
    पूर्णांक i;

    जर ((stok = म_मोहर((अक्षर *)रिक्त, (स्थिरांक अक्षर *)" ")) == रिक्त)
    {
	cs_usage_संदेश(Kill);
	परत CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    जर ((म_माप(stok) < 2 && VIM_ISDIGIT((पूर्णांक)(stok[0])))
	    || (म_माप(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((पूर्णांक)(stok[1]))))
	i = म_से_प(stok);
    अन्यथा
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	साठी (i = 0; i < csinfo_size; i++)
	{
	    जर (csinfo[i].fname != रिक्त && म_माला(csinfo[i].fname, stok))
		थांबा;
	}
    }

    जर ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == रिक्त))
    {
	जर (p_csverbose)
	    (शून्य)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    अन्यथा
    {
	जर (i == -1)
	{
	    साठी (i = 0; i < csinfo_size; i++)
	    {
		जर (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	अन्यथा
	    cs_kill_execute(i, stok);
    }

    परत 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    स्थिर शून्य
cs_kill_execute(
    पूर्णांक i,	    // cscope table index
    अक्षर *cname)    // cscope database name
{
    जर (p_csverbose)
    {
	संदेश_clr_eos();
	(शून्य)smsg_attr(HL_ATTR(HLF_R) | संदेश_HIST,
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
    स्थिर अक्षर *
cs_make_vim_style_matches(
    अक्षर *fname,
    अक्षर *slno,
    अक्षर *शोध,
    अक्षर *tagstr)
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
    अक्षर *buf;
    पूर्णांक amt;

    जर (शोध != रिक्त)
    {
	amt = (पूर्णांक)(म_माप(fname) + म_माप(slno) + म_माप(tagstr) + म_माप(शोध)+6);
	जर ((buf = alloc(amt)) == रिक्त)
	    परत रिक्त;

	(शून्य)प्र_लिखो(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, शोध);
    }
    अन्यथा
    {
	amt = (पूर्णांक)(म_माप(fname) + म_माप(slno) + म_माप(tagstr) + 5);
	जर ((buf = alloc(amt)) == रिक्त)
	    परत रिक्त;

	(शून्य)प्र_लिखो(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    परत buf;
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
    स्थिर अक्षर *
cs_manage_matches(
    अक्षर **matches,
    अक्षर **contexts,
    पूर्णांक totmatches,
    mcmd_e cmd)
{
    स्थिर अक्षर **mp = रिक्त;
    स्थिर अक्षर **cp = रिक्त;
    स्थिर पूर्णांक cnt = -1;
    स्थिर पूर्णांक next = -1;
    अक्षर *p = रिक्त;

    निवड (cmd)
    {
    स्थिती Store:
	प्रतिज्ञा(matches != रिक्त);
	प्रतिज्ञा(totmatches > 0);
	जर (mp != रिक्त || cp != रिक्त)
	    (शून्य)cs_manage_matches(रिक्त, रिक्त, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	थांबा;
    स्थिती Get:
	जर (next >= cnt)
	    परत रिक्त;

	p = mp[next];
	next++;
	थांबा;
    स्थिती Free:
	जर (mp != रिक्त)
	{
	    जर (cnt > 0)
		पर्यंत (cnt--)
		{
		    vim_free(mp[cnt]);
		    जर (cp != रिक्त)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = रिक्त;
	cp = रिक्त;
	cnt = 0;
	next = 0;
	थांबा;
    स्थिती Print:
	cs_print_tags_priv(mp, cp, cnt);
	थांबा;
    पूर्वनिर्धारित:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	परत रिक्त;
    }

    परत p;
}


/*
 * Parse cscope output.
 */
    स्थिर अक्षर *
cs_parse_results(
    पूर्णांक cnumber,
    अक्षर *buf,
    पूर्णांक bufsize,
    अक्षर **context,
    अक्षर **linenumber,
    अक्षर **शोध)
{
    पूर्णांक ch;
    अक्षर *p;
    अक्षर *name;

    जर (ख_माला_लो(buf, bufsize, csinfo[cnumber].fr_fp) == रिक्त)
    {
	जर (ख_पूर्ण(csinfo[cnumber].fr_fp))
	    त्रुटि_सं = EIO;

	cs_reading_emsg(cnumber);

	परत रिक्त;
    }

    // If the line's too long for the buffer, discard it.
    जर ((p = म_अक्षर(buf, '\n')) == रिक्त)
    {
	पर्यंत ((ch = अ_लो(csinfo[cnumber].fr_fp)) != खातापूर्ण && ch != '\n')
	    ;
	परत रिक्त;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    जर ((name = म_मोहर(buf, (स्थिरांक अक्षर *)" ")) == रिक्त)
	परत रिक्त;
    जर ((*context = म_मोहर(रिक्त, (स्थिरांक अक्षर *)" ")) == रिक्त)
	परत रिक्त;
    जर ((*linenumber = म_मोहर(रिक्त, (स्थिरांक अक्षर *)" ")) == रिक्त)
	परत रिक्त;
    *शोध = *linenumber + म_माप(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    जर (म_भेद(*शोध, "<unknown>") == 0)
	*शोध = रिक्त;

    name = cs_resolve_file(cnumber, name);
    परत name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    स्थिर शून्य
cs_file_results(खाता *f, पूर्णांक *nummatches_a)
{
    पूर्णांक i, j;
    अक्षर *buf;
    अक्षर *शोध, *slno;
    अक्षर *fullname;
    अक्षर *cntx;
    अक्षर *context;

    buf = alloc(CSREAD_BUFSIZE);
    जर (buf == रिक्त)
	परत;

    साठी (i = 0; i < csinfo_size; i++)
    {
	जर (nummatches_a[i] < 1)
	    सुरूठेवा;

	साठी (j = 0; j < nummatches_a[i]; j++)
	{
	   जर ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &शोध)) == रिक्त)
	       सुरूठेवा;

	   context = alloc(म_माप(cntx)+5);
	   जर (context == रिक्त)
	   {
	       vim_free(fullname);
	       सुरूठेवा;
	   }

	   जर (म_भेद(cntx, "<global>")==0)
	       म_नकल(context, "<<global>>");
	   अन्यथा
	       प्र_लिखो(context, "<<%s>>", cntx);

	   जर (शोध == रिक्त)
	       ख_लिखो(f, "%s\t%s\t%s\n", fullname, slno, context);
	   अन्यथा
	       ख_लिखो(f, "%s\t%s\t%s %s\n", fullname, slno, context, शोध);

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
    अक्षर *tagstr,
    पूर्णांक totmatches,
    पूर्णांक *nummatches_a,
    अक्षर ***matches_p,
    अक्षर ***cntxts_p,
    पूर्णांक *matched)
{
    पूर्णांक i, j;
    अक्षर *buf;
    अक्षर *शोध, *slno;
    पूर्णांक totsofar = 0;
    अक्षर **matches = रिक्त;
    अक्षर **cntxts = रिक्त;
    अक्षर *fullname;
    अक्षर *cntx;

    प्रतिज्ञा(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    जर (buf == रिक्त)
	परत;

    जर ((matches = ALLOC_MULT(अक्षर *, totmatches)) == रिक्त)
	जा parse_out;
    जर ((cntxts = ALLOC_MULT(अक्षर *, totmatches)) == रिक्त)
	जा parse_out;

    साठी (i = 0; i < csinfo_size; i++)
    {
	जर (nummatches_a[i] < 1)
	    सुरूठेवा;

	साठी (j = 0; j < nummatches_a[i]; j++)
	{
	   जर ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &शोध)) == रिक्त)
		सुरूठेवा;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  शोध, tagstr);

	    vim_free(fullname);

	    जर (म_भेद(cntx, "<global>") == 0)
		cntxts[totsofar] = रिक्त;
	    अन्यथा
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (अक्षर *)vim_strsave((char_u *)cntx);

	    जर (matches[totsofar] != रिक्त)
		totsofar++;

	} // for all matches

	(शून्य)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    जर (totsofar == 0)
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
    स्थिर अक्षर *
cs_pathcomponents(अक्षर *path)
{
    पूर्णांक		i;
    अक्षर	*s;

    जर (p_cspc == 0)
	परत path;

    s = path + म_माप(path) - 1;
    साठी (i = 0; i < p_cspc; ++i)
    {
	पर्यंत (s > path)
	{
	   s--;
	   जर (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      थांबा;
	}
    }
    जर ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    परत s;
}

/*
 * Called from cs_manage_matches().
 */
    स्थिर शून्य
cs_print_tags_priv(अक्षर **matches, अक्षर **cntxts, पूर्णांक अंक_matches)
{
    अक्षर	*buf = रिक्त;
    अक्षर	*t_buf;
    पूर्णांक		bufsize = 0; // Track available bufsize
    पूर्णांक		newsize = 0;
    अक्षर	*ptag;
    अक्षर	*fname, *lno, *extra, *tbuf;
    पूर्णांक		i, idx, अंक;
    अक्षर	*globalcntx = "GLOBAL";
    अक्षर	*cntxformat = " <<%s>>";
    अक्षर	*context;
    अक्षर	*cstag_संदेश = _("Cscope tag: %s");
    अक्षर	*csfmt_str = "%4d %6s  ";

    प्रतिज्ञा(अंक_matches > 0);

    जर ((tbuf = alloc(म_माप(matches[0]) + 1)) == रिक्त)
	परत;

    म_नकल(tbuf, matches[0]);
    ptag = म_मोहर(tbuf, "\t");
    जर (ptag == रिक्त)
    {
	vim_free(tbuf);
	परत;
    }

    newsize = (पूर्णांक)(म_माप(cstag_संदेश) + म_माप(ptag));
    buf = alloc(newsize);
    जर (buf != रिक्त)
    {
	bufsize = newsize;
	(शून्य)प्र_लिखो(buf, cstag_संदेश, ptag);
	संदेश_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    संदेश_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    संदेश_advance(संदेश_col + 2);
    संदेश_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    अंक = 1;
    साठी (i = 0; i < अंक_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	जर ((tbuf = alloc(म_माप(matches[idx]) + 1)) == रिक्त)
	    सुरूठेवा;
	(शून्य)म_नकल(tbuf, matches[idx]);

	जर (म_मोहर(tbuf, (स्थिरांक अक्षर *)"\t") == रिक्त
		|| (fname = म_मोहर(रिक्त, (स्थिरांक अक्षर *)"\t")) == रिक्त
		|| (lno = म_मोहर(रिक्त, (स्थिरांक अक्षर *)"\t")) == रिक्त)
	{
	    vim_free(tbuf);
	    सुरूठेवा;
	}
	extra = म_मोहर(रिक्त, (स्थिरांक अक्षर *)"\t");

	lno[म_माप(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (पूर्णांक)(म_माप(csfmt_str) + 16 + म_माप(lno));
	जर (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    जर (buf == रिक्त)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    अन्यथा
		bufsize = newsize;
	}
	जर (buf != रिक्त)
	{
	    // csfmt_str = "%4d %6s  ";
	    (शून्य)प्र_लिखो(buf, csfmt_str, अंक, lno);
	    संदेश_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	संदेश_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	जर (cntxts[idx] != रिक्त)
	    context = cntxts[idx];
	अन्यथा
	    context = globalcntx;
	newsize = (पूर्णांक)(म_माप(context) + म_माप(cntxformat));

	जर (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    जर (buf == रिक्त)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    अन्यथा
		bufsize = newsize;
	}
	जर (buf != रिक्त)
	{
	    (शून्य)प्र_लिखो(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    जर (संदेश_col + (पूर्णांक)म_माप(buf) >= cmdline_width)
		संदेश_putchar('\n');
	    संदेश_advance(12);
	    संदेश_outtrans_long_attr((char_u *)buf, 0);
	    संदेश_putchar('\n');
	}
	जर (extra != रिक्त)
	{
	    संदेश_advance(13);
	    संदेश_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	जर (संदेश_col)
	    संदेश_putchar('\n');

	ui_breakcheck();
	जर (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    थांबा;
	}

	अंक++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    स्थिर पूर्णांक
cs_read_prompt(पूर्णांक i)
{
    पूर्णांक		ch;
    अक्षर	*buf = रिक्त; // buffer for possible error message from cscope
    पूर्णांक		bufpos = 0;
    पूर्णांक		maxlen;
    स्थिर अक्षर *eprompt = "Press the RETURN key to continue:";
    पूर्णांक		epromptlen = (पूर्णांक)म_माप(eprompt);
    पूर्णांक		n;

    // compute maximum allowed len for Cscope error message
    अक्षर *cs_emsg = _(e_cscope_error_str);
    maxlen = (पूर्णांक)(IOSIZE - म_माप(cs_emsg));

    साठी (;;)
    {
	पर्यंत ((ch = अ_लो(csinfo[i].fr_fp)) != खातापूर्ण && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    जर (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		जर (buf == रिक्त) // lazy buffer allocation
		    buf = alloc(maxlen);
		जर (buf != रिक्त)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    जर (bufpos >= epromptlen
			    && म_भेद(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(शून्य)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(शून्य)अ_दो('\n', csinfo[i].to_fp);
			(शून्य)ख_साफ(csinfo[i].to_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	साठी (n = 0; n < (पूर्णांक)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    जर (n > 0)
		ch = अ_लो(csinfo[i].fr_fp);
	    जर (ch == खातापूर्ण)
	    {
		जर (buf != रिक्त && buf[0] != NUL)
		    (शून्य)semsg(cs_emsg, buf);
		अन्यथा जर (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		परत CSCOPE_FAILURE;
	    }

	    जर (ch != CSCOPE_PROMPT[n])
	    {
		ch = खातापूर्ण;
		थांबा;
	    }
	}

	जर (ch == खातापूर्ण)
	    सुरूठेवा;	    // didn't find the prompt
	थांबा;		    // did find the prompt
    }

    vim_free(buf);
    परत CSCOPE_SUCCESS;
}

# जर defined(UNIX) && defined(SIGALRM)
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
cs_release_csp(पूर्णांक i, पूर्णांक freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    जर (csinfo[i].to_fp != रिक्त)
    {
	(शून्य)ख_माला_दो("q\n", csinfo[i].to_fp);
	(शून्य)ख_साफ(csinfo[i].to_fp);
    }
# जर defined(UNIX)
    {
	पूर्णांक waitpid_errno;
	पूर्णांक pstat;
	pid_t pid;

#  जर defined(HAVE_SIGACTION)
	संरचना sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   अन्यथा
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = त्रुटि_सं;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, रिक्त);
#  अन्यथा
	पूर्णांक waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	साठी (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = त्रुटि_सं;
	    जर (pid != 0)
		थांबा;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	जर (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    पूर्णांक alive = TRUE;

	    जर (waitpid_errno == ECHILD)
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
		पूर्णांक waited;

		sleep(0);
		साठी (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    जर (समाप्त(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			थांबा;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    जर (alive)
#  endif
	    {
		समाप्त(csinfo[i].pid, SIGKILL);
		(शून्य)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# अन्यथा  // !UNIX
    जर (csinfo[i].hProc != रिक्त)
    {
	// Give cscope a chance to exit normally
	जर (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    जर (csinfo[i].fr_fp != रिक्त)
	(शून्य)ख_बंद(csinfo[i].fr_fp);
    जर (csinfo[i].to_fp != रिक्त)
	(शून्य)ख_बंद(csinfo[i].to_fp);

    जर (freefnpp)
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
    स्थिर पूर्णांक
cs_reset(exarg_T *eap UNUSED)
{
    अक्षर	**dblist = रिक्त, **pplist = रिक्त, **fllist = रिक्त;
    पूर्णांक	i;
    अक्षर buf[20]; // for sprintf " (#%d)"

    जर (csinfo_size == 0)
	परत CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(अक्षर *, csinfo_size);
    pplist = ALLOC_MULT(अक्षर *, csinfo_size);
    fllist = ALLOC_MULT(अक्षर *, csinfo_size);
    जर (dblist == रिक्त || pplist == रिक्त || fllist == रिक्त)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	परत CSCOPE_FAILURE;
    }

    साठी (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	जर (csinfo[i].fname != रिक्त)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    साठी (i = 0; i < csinfo_size; i++)
    {
	जर (dblist[i] != रिक्त)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    जर (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		प्र_लिखो(buf, " (#%d)", i);
		संदेश_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    जर (p_csverbose)
	संदेश_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | संदेश_HIST);
    परत CSCOPE_SUCCESS;
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
    स्थिर अक्षर *
cs_resolve_file(पूर्णांक i, अक्षर *name)
{
    अक्षर	*fullname;
    string_T	csdir = {रिक्त, 0};
    माप_प्रकार	namelen;
    माप_प्रकार	ppathlen = 0;
    पूर्णांक		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (पूर्णांक)namelen + 2;
    जर (csinfo[i].ppath != रिक्त)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (पूर्णांक)ppathlen;
    }
    अन्यथा जर (p_csre && csinfo[i].fname != रिक्त)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	जर (csdir.string != रिक्त)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (पूर्णांक)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    जर (csinfo[i].ppath != रिक्त
	    && (म_भेदन(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	जर ((fullname = alloc(len)) != रिक्त)
	    (शून्य)प्र_लिखो(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    अन्यथा जर (csdir.string != रिक्त && csinfo[i].fname != रिक्त && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (अक्षर *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    अन्यथा
    {
	fullname = (अक्षर *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    परत fullname;
}


/*
 * Show all cscope connections.
 */
    स्थिर पूर्णांक
cs_show(exarg_T *eap UNUSED)
{
    पूर्णांक i;

    जर (cs_cnt_connections() == 0)
	संदेश_puts(_("no cscope connections\n"));
    अन्यथा
    {
	संदेश_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	साठी (i = 0; i < csinfo_size; i++)
	{
	    जर (csinfo[i].fname == रिक्त)
		सुरूठेवा;

	    जर (csinfo[i].ppath != रिक्त)
		(शून्य)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (दीर्घ)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    अन्यथा
		(शून्य)smsg("%2d %-5ld  %-34s  <none>",
			   i, (दीर्घ)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    परत CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    शून्य
cs_end(शून्य)
{
    पूर्णांक i;

    साठी (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#जर defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    शून्य
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    पूर्णांक		अंक = 0;
    char_u	*dbpath = रिक्त;
    char_u	*prepend = रिक्त;
    char_u	buf[NUMBUFLEN];

    जर (in_vim9script()
	    && (check_for_opt_संख्या_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	परत;

    जर (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	अंक = (पूर्णांक)tv_get_संख्या(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	जर (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_संख्या = cs_connection(अंक, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
