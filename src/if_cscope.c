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

#شامل "vim.h"

#اگر defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# اگر defined(UNIX)
#  include <sys/wait.h>
# endif

# اگر defined (MSWIN)
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

تعریفنوع ساختار {
    نویسه *  name;
    صحیح     (*func)(exarg_T *eap);
    نویسه *  help;
    نویسه *  usage;
    صحیح	    cansplit;		// if supports splitting window
} cscmd_T;

تعریفنوع ساختار csi {
    نویسه *	    fname;	// cscope db name
    نویسه *	    ppath;	// path to prepend (the -P option)
    نویسه *	    flags;	// additional cscope flags/options (e.g, -p2)
# اگر defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# وگرنه
#  اگر defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_Index_بالا;	// st_ino has no meaning in the Windows
    DWORD	    n_Index_کم;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    to_fp;	// to cscope: FILE.
} csinfo_T;

تعریفنوع شمارش { Add, Find, Help, Kill, Reset, Show } csid_e;

تعریفنوع شمارش {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

ایستا صحیح	    cs_add(exarg_T *eap);
ایستا صحیح	    cs_add_common(نویسه *, نویسه *, نویسه *);
ایستا صحیح	    cs_check_for_connections(تهی);
ایستا صحیح	    cs_check_for_tags(تهی);
ایستا صحیح	    cs_cnt_connections(تهی);
ایستا صحیح	    cs_create_connection(صحیح i);
# ifdef FEAT_QUICKFIX
ایستا تهی	    cs_file_results(FILE *, صحیح *);
# endif
ایستا تهی	    cs_fill_results(نویسه *, صحیح , صحیح *, نویسه ***,
			نویسه ***, صحیح *);
ایستا صحیح	    cs_find(exarg_T *eap);
ایستا صحیح	    cs_find_common(نویسه *opt, نویسه *pat, صحیح, صحیح, صحیح, char_u *cmdline);
ایستا صحیح	    cs_help(exarg_T *eap);
ایستا صحیح	    cs_درج_filelist(نویسه *, نویسه *, نویسه *,
			stat_T *);
ایستا صحیح	    cs_kill(exarg_T *eap);
ایستا تهی	    cs_kill_execute(صحیح, نویسه *);
ایستا cscmd_T *    cs_lookup_cmd(exarg_T *eap);
ایستا نویسه *	    cs_make_vim_style_matches(نویسه *, نویسه *,
			نویسه *, نویسه *);
ایستا نویسه *	    cs_manage_matches(نویسه **, نویسه **, صحیح, mcmd_e);
ایستا تهی	    cs_print_tags_priv(نویسه **, نویسه **, صحیح);
ایستا صحیح	    cs_read_prompt(صحیح);
ایستا تهی	    cs_release_csp(صحیح, صحیح freefnpp);
ایستا صحیح	    cs_reset(exarg_T *eap);
ایستا نویسه *	    cs_resolve_file(صحیح, نویسه *);
ایستا صحیح	    cs_show(exarg_T *eap);


ایستا csinfo_T *   csinfo = پوچ;
ایستا صحیح	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

ایستا صحیح	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
ایستا cscmd_T	    cs_cmds[] =
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
    { پوچ, پوچ, پوچ, پوچ, 0 }
};

    ایستا تهی
cs_usage_پیام(csid_e x)
{
    (تهی)semsg(_(e_usage_cscope_str), cs_cmds[(صحیح)x].usage);
}

ایستا شمارش
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
get_cscope_name(expand_T *xp UNUSED, صحیح idx)
{
    صحیح		current_idx;
    صحیح		i;

    انتخاب (expand_what)
    {
    حالت EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	بازگشت (char_u *)cs_cmds[idx].name;
    حالت EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	برای (i = 0, current_idx = 0; cs_cmds[i].name != پوچ; i++)
	    اگر (cs_cmds[i].cansplit)
		اگر (current_idx++ == idx)
		    بشکن;
	بازگشت (char_u *)cs_cmds[i].name;
    حالت EXP_CSCOPE_FIND:
	{
	    ثابت نویسه *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", پوچ
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    بازگشت (char_u *)query_type[idx];
	}
    حالت EXP_CSCOPE_KILL:
	{
	    ایستا نویسه	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    برای (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		اگر (csinfo[i].fname == پوچ)
		    ادامه;
		اگر (current_idx++ == idx)
		{
		    vim_snprintf(connection, اندازه(connection), "%d", i);
		    بازگشت (char_u *)connection;
		}
	    }
	    بازگشت (current_idx == idx && idx > 0) ? (char_u *)"-1" : پوچ;
	}
    پیشفرض:
	بازگشت پوچ;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    تهی
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

    اگر (*arg == NUL)
	بازگشت;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    اگر (*p == NUL)
	بازگشت;

    // past first word
    xp->xp_pattern = skipwhite(p);
    اگر (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    وگرنه اگر (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    وگرنه اگر (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    وگرنه اگر (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    وگرنه
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    ایستا تهی
do_cscope_general(
    exarg_T	*eap,
    صحیح		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    اگر (check_restricted())
	بازگشت;

    اگر ((cmdp = cs_lookup_cmd(eap)) == پوچ)
    {
	cs_help(eap);
	بازگشت;
    }

    اگر (make_split)
    {
	اگر (!cmdp->cansplit)
	{
	    (تهی)پیام_puts(_("This cscope command does not support splitting the window.\n"));
	    بازگشت;
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
    تهی
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    تهی
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    تهی
ex_cstag(exarg_T *eap)
{
    صحیح ret = FALSE;

    اگر (*eap->arg == NUL)
    {
	(تهی)emsg(_(e_usage_cstag_ident));
	بازگشت;
    }

    انتخاب (p_csto)
    {
    حالت 0 :
	اگر (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (نویسه *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    اگر (ret == FALSE)
	    {
		cs_free_tags();
		اگر (پیام_col)
		    پیام_putchar('\n');

		اگر (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	وگرنه اگر (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	بشکن;
    حالت 1 :
	اگر (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    اگر (ret == FALSE)
	    {
		اگر (پیام_col)
		    پیام_putchar('\n');

		اگر (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (نویسه *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    اگر (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	وگرنه اگر (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (نویسه *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    اگر (ret == FALSE)
		cs_free_tags();
	}
	بشکن;
    پیشفرض :
	بشکن;
    }

    اگر (!ret)
    {
	(تهی)emsg(_(e_cstag_tag_not_founc));
# اگر defined(FEAT_QUICKFIX)
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
    صحیح
cs_fgets(char_u *buf, صحیح size)
{
    نویسه *p;

    اگر ((p = cs_manage_matches(پوچ, پوچ, -1, Get)) == پوچ)
	بازگشت TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    بازگشت FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    تهی
cs_free_tags(تهی)
{
    cs_manage_matches(پوچ, پوچ, -1, Free);
}


/*
 * Called from do_tag().
 */
    تهی
cs_print_tags(تهی)
{
    cs_manage_matches(پوچ, پوچ, -1, Print);
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
# اگر defined(FEAT_EVAL)
    ایستا صحیح
cs_connection(صحیح رقم, char_u *dbpath, char_u *ppath)
{
    صحیح i;

    اگر (رقم < 0 || رقم > 4 || (رقم > 0 && !dbpath))
	بازگشت FALSE;

    برای (i = 0; i < csinfo_size; i++)
    {
	اگر (!csinfo[i].fname)
	    ادامه;

	اگر (رقم == 0)
	    بازگشت TRUE;

	انتخاب (رقم)
	{
	حالت 1:
	    اگر (strstr(csinfo[i].fname, (نویسه *)dbpath))
		بازگشت TRUE;
	    بشکن;
	حالت 2:
	    اگر (strcmp(csinfo[i].fname, (نویسه *)dbpath) == 0)
		بازگشت TRUE;
	    بشکن;
	حالت 3:
	    اگر (strstr(csinfo[i].fname, (نویسه *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (نویسه *)ppath))))
		بازگشت TRUE;
	    بشکن;
	حالت 4:
	    اگر ((strcmp(csinfo[i].fname, (نویسه *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (نویسه *)ppath) == 0))))
		بازگشت TRUE;
	    بشکن;
	}
    }

    بازگشت FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    ایستا صحیح
cs_add(exarg_T *eap UNUSED)
{
    نویسه *fname, *ppath, *flags = پوچ;

    اگر ((fname = strtok((نویسه *)پوچ, (ثابت نویسه *)" ")) == پوچ)
    {
	cs_usage_پیام(Add);
	بازگشت CSCOPE_FAILURE;
    }
    اگر ((ppath = strtok((نویسه *)پوچ, (ثابت نویسه *)" ")) != پوچ)
	flags = strtok((نویسه *)پوچ, (ثابت نویسه *)" ");

    بازگشت cs_add_common(fname, ppath, flags);
}

    ایستا تهی
cs_stat_emsg(نویسه *fname)
{
    صحیح err = errno;
    (تهی)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    ایستا صحیح
cs_add_common(
    نویسه *arg1,	    // filename - may contain environment variables
    نویسه *arg2,	    // prepend path - may contain environment variables
    نویسه *flags)
{
    stat_T	statbuf;
    صحیح		ret;
    نویسه	*fname = پوچ;
    نویسه	*fname2 = پوچ;
    نویسه	*ppath = پوچ;
    صحیح		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = پوچ;

    // get the filename (arg1), expand it, and try to stat it
    اگر ((fname = alloc(MAXPATHL + 1)) == پوچ)
	برو add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (تهی)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    اگر (fname == پوچ)
	برو add_err;
    fname = (نویسه *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    اگر (ret < 0)
    {
staterr:
	اگر (p_csverbose)
	    cs_stat_emsg(fname);
	برو add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    اگر (arg2 != پوچ)
    {
	stat_T	    statbuf2;

	اگر ((ppath = alloc(MAXPATHL + 1)) == پوچ)
	    برو add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	اگر (ret < 0)
	    برو staterr;
    }

    // if filename is a directory, append the cscope database name to it
    اگر (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	اگر (fname2 == پوچ)
	    برو add_err;

	وقتی (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    اگر (fname[0] == '\0')
		بشکن;
	}
	اگر (fname[0] == '\0')
	    (تهی)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	وگرنه
	    (تهی)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	اگر (ret < 0)
	{
	    اگر (p_csverbose)
		cs_stat_emsg(fname2);
	    برو add_err;
	}

	i = cs_درج_filelist(fname2, ppath, flags, &statbuf);
    }
    وگرنه اگر (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_درج_filelist(fname, ppath, flags, &statbuf);
    }
    وگرنه
    {
	اگر (p_csverbose)
	    (تهی)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	برو add_err;
    }

    اگر (i != -1)
    {
	اگر (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    برو add_err;
	}

	اگر (p_csverbose)
	{
	    پیام_clr_eos();
	    (تهی)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    بازگشت CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    بازگشت CSCOPE_FAILURE;
}


    ایستا صحیح
cs_check_for_connections(تهی)
{
    بازگشت (cs_cnt_connections() > 0);
}


    ایستا صحیح
cs_check_for_tags(تهی)
{
    بازگشت (p_tags[0] != NUL && curbuf->b_p_tags != پوچ);
}


/*
 * Count the number of cscope connections.
 */
    ایستا صحیح
cs_cnt_connections(تهی)
{
    صحیح i;
    صحیح cnt = 0;

    برای (i = 0; i < csinfo_size; i++)
	اگر (csinfo[i].fname != پوچ)
	    cnt++;
    بازگشت cnt;
}

    ایستا تهی
cs_reading_emsg(
    صحیح idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    ایستا صحیح
cs_cnt_matches(صحیح idx)
{
    نویسه *stok;
    نویسه *buf;
    صحیح nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    اگر (buf == پوچ)
	بازگشت 0;
    برای (;;)
    {
	اگر (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    اگر (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    بازگشت -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	اگر (strstr((ثابت نویسه *)buf, "Unable to search database") != پوچ)
	    بشکن;
	اگر ((stok = strtok(buf, (ثابت نویسه *)" ")) == پوچ)
	    ادامه;
	اگر (strstr((ثابت نویسه *)stok, "cscope:") == پوچ)
	    ادامه;

	اگر ((stok = strtok(پوچ, (ثابت نویسه *)" ")) == پوچ)
	    ادامه;
	nlines = atoi(stok);
	اگر (nlines < 0)
	{
	    nlines = 0;
	    بشکن;
	}

	اگر ((stok = strtok(پوچ, (ثابت نویسه *)" ")) == پوچ)
	    ادامه;
	اگر (strncmp((ثابت نویسه *)stok, "lines", 5))
	    ادامه;

	بشکن;
    }

    vim_free(buf);
    بازگشت nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    ایستا نویسه *
cs_create_cmd(نویسه *csoption, نویسه *pattern)
{
    نویسه *cmd;
    short جستجو;
    نویسه *pat;

    انتخاب (csoption[0])
    {
    حالت '0' : حالت 's' :
	جستجو = 0;
	بشکن;
    حالت '1' : حالت 'g' :
	جستجو = 1;
	بشکن;
    حالت '2' : حالت 'd' :
	جستجو = 2;
	بشکن;
    حالت '3' : حالت 'c' :
	جستجو = 3;
	بشکن;
    حالت '4' : حالت 't' :
	جستجو = 4;
	بشکن;
    حالت '6' : حالت 'e' :
	جستجو = 6;
	بشکن;
    حالت '7' : حالت 'f' :
	جستجو = 7;
	بشکن;
    حالت '8' : حالت 'i' :
	جستجو = 8;
	بشکن;
    حالت '9' : حالت 'a' :
	جستجو = 9;
	بشکن;
    پیشفرض :
	(تهی)emsg(_(e_unknown_cscope_جستجو_type));
	cs_usage_پیام(Find);
	بازگشت پوچ;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    اگر (جستجو != 4 && جستجو != 6)
	وقتی VIM_ISWHITE(*pat)
	    ++pat;

    اگر ((cmd = alloc(strlen(pat) + 2)) == پوچ)
	بازگشت پوچ;

    (تهی)sprintf(cmd, "%d%s", جستجو, pat);

    بازگشت cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    ایستا صحیح
cs_create_connection(صحیح i)
{
# ifdef UNIX
    صحیح		to_cs[2], from_cs[2];
# endif
    صحیح		cmdlen;
    نویسه	*prog, *cmd, *ppath = پوچ;
    size_t	proglen;
# ifdef MSWIN
    صحیح		fd;
    صحیح		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# اگر defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    اگر (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(تهی)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	اگر (to_cs[0] != -1)
	    (تهی)close(to_cs[0]);
	اگر (to_cs[1] != -1)
	    (تهی)close(to_cs[1]);
	اگر (from_cs[0] != -1)
	    (تهی)close(from_cs[0]);
	اگر (from_cs[1] != -1)
	    (تهی)close(from_cs[1]);
	بازگشت CSCOPE_FAILURE;
    }

    اگر ((csinfo[i].pid = fork()) == -1)
    {
	(تهی)emsg(_(e_could_not_fork_for_cscope));
	برو err_closing;
    }
    وگرنه اگر (csinfo[i].pid == 0)	// child: run cscope.
    {
	نویسه **argv = پوچ;

	اگر (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	اگر (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	اگر (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(تهی)close(to_cs[1]);
	(تهی)close(from_cs[0]);
# وگرنه
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = اندازه(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = پوچ;

	اگر (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (تهی)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    اگر (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    اگر (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    بازگشت CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	اگر ((prog = alloc(MAXPATHL + 1)) == پوچ)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# وگرنه
	    // MSWIN
	    برو err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (صحیح)(proglen + strlen(csinfo[i].fname) + 32);
	اگر (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    اگر ((ppath = alloc(MAXPATHL + 1)) == پوچ)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# وگرنه
		// MSWIN
		برو err_closing;
# endif
	    }
	    cmdlen += (صحیح)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	اگر (csinfo[i].flags)
	    cmdlen += (صحیح)strlen(csinfo[i].flags);

	اگر ((cmd = alloc(cmdlen)) == پوچ)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# وگرنه
	    // MSWIN
	    برو err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    نویسه	**tok = پوچ;
	    صحیح		tokc = 0;

	    ga_init2(&ga_argv, اندازه(نویسه *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    اگر (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    برای (صحیح k = 0; k < tokc; ++k)
	    {
		اگر (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((نویسه **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    اگر (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((نویسه **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (نویسه *)vim_strsave((char_u *)"-dl");
	    ((نویسه **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (نویسه *)vim_strsave((char_u *)"-f");
	    ((نویسه **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(نویسه *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    اگر (ppath != پوچ)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		نویسه	*parg = alloc(plen);

		اگر (parg == پوچ || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((نویسه **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    اگر (csinfo[i].flags != پوچ)
	    {
		اگر (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		برای (صحیح k = 0; k < tokc; ++k)
		{
		    اگر (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((نویسه **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    اگر (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((نویسه **)ga_argv.ga_data)[ga_argv.ga_len] = پوچ;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  اگر defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   اگر defined(HAVE_SETSID)
	    (تهی)setsid();
#   وگرنه
	    اگر (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (نویسه **)ga_argv.ga_data;
	    اگر (argv[0] == پوچ || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    وگرنه	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	اگر ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == پوچ)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	اگر ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == پوچ)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(تهی)close(to_cs[0]);
	(تهی)close(from_cs[1]);
    }
# وگرنه
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    اگر (csinfo[i].ppath != پوچ)
    {
	len = (صحیح)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    اگر (csinfo[i].flags != پوچ)
    {
	len = (صحیح)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_درون_داد  = stdin_rd;
    created = CreateProcess(پوچ, cmd, پوچ, پوچ, TRUE, CREATE_NEW_CONSOLE,
							پوچ, پوچ, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    اگر (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(تهی)emsg(_(e_could_not_spawn_cscope_process));
	برو err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    اگر (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == پوچ))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    اگر (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == پوچ))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    بازگشت CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    ایستا صحیح
cs_find(exarg_T *eap)
{
    نویسه *opt, *pat;
    صحیح i;

    اگر (cs_check_for_connections() == FALSE)
    {
	(تهی)emsg(_(e_no_cscope_connections));
	بازگشت FALSE;
    }

    اگر ((opt = strtok((نویسه *)پوچ, (ثابت نویسه *)" ")) == پوچ)
    {
	cs_usage_پیام(Find);
	بازگشت FALSE;
    }

    pat = opt + strlen(opt) + 1;
    اگر (pat >= (نویسه *)eap->arg + eap_arg_len)
    {
	cs_usage_پیام(Find);
	بازگشت FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    برای (i = 0; i < eap_arg_len; ++i)
	اگر (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    بازگشت cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    ایستا صحیح
cs_find_common(
    نویسه *opt,
    نویسه *pat,
    صحیح forceit,
    صحیح verbose,
    صحیح	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    صحیح i;
    نویسه *cmd;
    صحیح *nummatches;
    صحیح totmatches;
# ifdef FEAT_QUICKFIX
    نویسه cmdletter;
    نویسه *qfpos;

    // get cmd letter
    انتخاب (opt[0])
    {
    حالت '0' :
	cmdletter = 's';
	بشکن;
    حالت '1' :
	cmdletter = 'g';
	بشکن;
    حالت '2' :
	cmdletter = 'd';
	بشکن;
    حالت '3' :
	cmdletter = 'c';
	بشکن;
    حالت '4' :
	cmdletter = 't';
	بشکن;
    حالت '6' :
	cmdletter = 'e';
	بشکن;
    حالت '7' :
	cmdletter = 'f';
	بشکن;
    حالت '8' :
	cmdletter = 'i';
	بشکن;
    حالت '9' :
	cmdletter = 'a';
	بشکن;
    پیشفرض :
	cmdletter = opt[0];
    }

    qfpos = (نویسه *)vim_strchr(p_csqf, cmdletter);
    اگر (qfpos != پوچ)
    {
	qfpos++;
	// next symbol must be + or -
	اگر (strchr(CSQF_FLAGS, *qfpos) == پوچ)
	{
	    (تهی)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    بازگشت FALSE;
	}

	اگر (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    اگر (aborting())
		بازگشت FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    اگر (cmd == پوچ)
	بازگشت FALSE;

    nummatches = ALLOC_MULT(صحیح, csinfo_size);
    اگر (nummatches == پوچ)
    {
	vim_free(cmd);
	بازگشت FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    برای (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    برای (i = 0; i < csinfo_size; i++)
    {
	اگر (csinfo[i].fname == پوچ || csinfo[i].to_fp == پوچ)
	    ادامه;

	// send cmd to cscope
	(تهی)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(تهی)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	اگر (nummatches[i] > -1)
	    totmatches += nummatches[i];

	اگر (nummatches[i] == 0)
	    (تهی)cs_read_prompt(i);
    }
    vim_free(cmd);

    اگر (totmatches == 0)
    {
	اگر (verbose)
	    (تهی)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	بازگشت FALSE;
    }

# ifdef FEAT_QUICKFIX
    اگر (qfpos != پوچ && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = پوچ;
	win_T	    *wp = پوچ;

	اگر (tmp == پوچ)
	{
	    vim_free(nummatches);
	    بازگشت FALSE;
	}

	f = mch_fopen((نویسه *)tmp, "w");
	اگر (f == پوچ)
	    semsg(_(e_cant_open_file_str), tmp);
	وگرنه
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    اگر (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    اگر (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, پوچ) > 0)
	    {
		اگر (postponed_split != 0)
		{
		    (تهی)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		اگر (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != پوچ)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	بازگشت TRUE;
    }
    وگرنه
# endif // FEAT_QUICKFIX
    {
	نویسه **matches = پوچ, **contexts = پوچ;
	صحیح matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	اگر (matches == پوچ)
	    بازگشت FALSE;

	(تهی)cs_manage_matches(matches, contexts, matched, Store);

	بازگشت do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    ایستا صحیح
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (تهی)پیام_puts(_("cscope commands:\n"));
    وقتی (cmdp->name != پوچ)
    {
	نویسه *help = _(cmdp->help);
	صحیح  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	اگر (space_cnt < 0)
	    space_cnt = 0;
	(تهی)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	اگر (strcmp(cmdp->name, "find") == 0)
	    پیام_puts(_("\n"
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
    بازگشت 0;
}


    ایستا تهی
clear_csinfo(صحیح i)
{
    csinfo[i].fname  = پوچ;
    csinfo[i].ppath  = پوچ;
    csinfo[i].flags  = پوچ;
# اگر defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# وگرنه
    csinfo[i].nVolume = 0;
    csinfo[i].n_Index_بالا = 0;
    csinfo[i].n_Index_کم = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = پوچ;
    csinfo[i].to_fp  = پوچ;
# اگر defined(MSWIN)
    csinfo[i].hProc = پوچ;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    ایستا صحیح
cs_درج_filelist(
    نویسه *fname,
    نویسه *ppath,
    نویسه *flags,
    stat_T *sb UNUSED)
{
    صحیح	    i;
    صحیح	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    انتخاب (win32_fileinfo((char_u *)fname, &bhfi))
    {
	حالت FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	حالت FILEINFO_READ_FAIL:	// CreateFile() failed
	    اگر (p_csverbose)
	    {
		نویسه *cant_پیام = _(e_cannot_open_cscope_database_str);
		نویسه *winmsg = GetWin32Error();

		اگر (winmsg != پوچ)
		    (تهی)semsg(cant_پیام, winmsg);
		وگرنه
		    // subst filename if can't get error text
		    (تهی)semsg(cant_پیام, fname);
	    }
	    بازگشت -1;

	حالت FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    اگر (p_csverbose)
		(تهی)emsg(_(e_cannot_get_cscope_database_information));
	    بازگشت -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    برای (j = 0; j < csinfo_size; j++)
    {
	اگر (csinfo[j].fname != پوچ
# اگر defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# وگرنه
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_عدد
		    && csinfo[j].n_Index_بالا == bhfi.n_File_Index_بالا
		    && csinfo[j].n_Index_کم == bhfi.n_File_Index_کم))
# endif
	    )
	{
	    اگر (p_csverbose)
		(تهی)emsg(_(e_duplicate_cscope_database_not_added));
	    بازگشت -1;
	}

	اگر (csinfo[j].fname == پوچ && i == -1)
	    i = j; // remember first empty entry
    }

    اگر (i == -1)
    {
	i = csinfo_size;
	اگر (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	وگرنه
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, اندازه(csinfo_T)*csinfo_size);
	    اگر (csinfo == پوچ)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		بازگشت -1;
	    }
	}
	اگر (csinfo == پوچ)
	    بازگشت -1;
	برای (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    اگر ((csinfo[i].fname = alloc(strlen(fname)+1)) == پوچ)
	بازگشت -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((ثابت نویسه *)fname));

    اگر (ppath != پوچ)
    {
	اگر ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == پوچ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    بازگشت -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((ثابت نویسه *)ppath));
    }
    وگرنه
	csinfo[i].ppath = پوچ;

    اگر (flags != پوچ)
    {
	اگر ((csinfo[i].flags = alloc(strlen(flags) + 1)) == پوچ)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    بازگشت -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((ثابت نویسه *)flags));
    }
    وگرنه
	csinfo[i].flags = پوچ;

# اگر defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# وگرنه
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_عدد;
    csinfo[i].n_Index_کم = bhfi.n_File_Index_کم;
    csinfo[i].n_Index_بالا = bhfi.n_File_Index_بالا;
# endif
    بازگشت i;
}


/*
 * Find cscope command in command table.
 */
    ایستا cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    نویسه *stok;
    size_t len;

    اگر (eap->arg == پوچ)
	بازگشت پوچ;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (صحیح)STRLEN(eap->arg);

    اگر ((stok = strtok((نویسه *)(eap->arg), (ثابت نویسه *)" ")) == پوچ)
	بازگشت پوچ;

    len = strlen(stok);
    برای (cmdp = cs_cmds; cmdp->name != پوچ; ++cmdp)
    {
	اگر (strncmp((ثابت نویسه *)(stok), cmdp->name, len) == 0)
	    بازگشت (cmdp);
    }
    بازگشت پوچ;
}


/*
 * Nuke em.
 */
    ایستا صحیح
cs_kill(exarg_T *eap UNUSED)
{
    نویسه *stok;
    صحیح i;

    اگر ((stok = strtok((نویسه *)پوچ, (ثابت نویسه *)" ")) == پوچ)
    {
	cs_usage_پیام(Kill);
	بازگشت CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    اگر ((strlen(stok) < 2 && VIM_ISDIGIT((صحیح)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((صحیح)(stok[1]))))
	i = atoi(stok);
    وگرنه
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	برای (i = 0; i < csinfo_size; i++)
	{
	    اگر (csinfo[i].fname != پوچ && strstr(csinfo[i].fname, stok))
		بشکن;
	}
    }

    اگر ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == پوچ))
    {
	اگر (p_csverbose)
	    (تهی)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    وگرنه
    {
	اگر (i == -1)
	{
	    برای (i = 0; i < csinfo_size; i++)
	    {
		اگر (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	وگرنه
	    cs_kill_execute(i, stok);
    }

    بازگشت 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    ایستا تهی
cs_kill_execute(
    صحیح i,	    // cscope table index
    نویسه *cname)    // cscope database name
{
    اگر (p_csverbose)
    {
	پیام_clr_eos();
	(تهی)smsg_attr(HL_ATTR(HLF_R) | پیام_HIST,
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
    ایستا نویسه *
cs_make_vim_style_matches(
    نویسه *fname,
    نویسه *slno,
    نویسه *جستجو,
    نویسه *tagstr)
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
    نویسه *buf;
    صحیح amt;

    اگر (جستجو != پوچ)
    {
	amt = (صحیح)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(جستجو)+6);
	اگر ((buf = alloc(amt)) == پوچ)
	    بازگشت پوچ;

	(تهی)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, جستجو);
    }
    وگرنه
    {
	amt = (صحیح)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	اگر ((buf = alloc(amt)) == پوچ)
	    بازگشت پوچ;

	(تهی)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    بازگشت buf;
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
    ایستا نویسه *
cs_manage_matches(
    نویسه **matches,
    نویسه **contexts,
    صحیح totmatches,
    mcmd_e cmd)
{
    ایستا نویسه **mp = پوچ;
    ایستا نویسه **cp = پوچ;
    ایستا صحیح cnt = -1;
    ایستا صحیح next = -1;
    نویسه *p = پوچ;

    انتخاب (cmd)
    {
    حالت Store:
	أكد(matches != پوچ);
	أكد(totmatches > 0);
	اگر (mp != پوچ || cp != پوچ)
	    (تهی)cs_manage_matches(پوچ, پوچ, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	بشکن;
    حالت Get:
	اگر (next >= cnt)
	    بازگشت پوچ;

	p = mp[next];
	next++;
	بشکن;
    حالت Free:
	اگر (mp != پوچ)
	{
	    اگر (cnt > 0)
		وقتی (cnt--)
		{
		    vim_free(mp[cnt]);
		    اگر (cp != پوچ)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = پوچ;
	cp = پوچ;
	cnt = 0;
	next = 0;
	بشکن;
    حالت Print:
	cs_print_tags_priv(mp, cp, cnt);
	بشکن;
    پیشفرض:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	بازگشت پوچ;
    }

    بازگشت p;
}


/*
 * Parse cscope output.
 */
    ایستا نویسه *
cs_parse_results(
    صحیح cnumber,
    نویسه *buf,
    صحیح bufsize,
    نویسه **context,
    نویسه **linenumber,
    نویسه **جستجو)
{
    صحیح ch;
    نویسه *p;
    نویسه *name;

    اگر (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == پوچ)
    {
	اگر (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	بازگشت پوچ;
    }

    // If the line's too long for the buffer, discard it.
    اگر ((p = strchr(buf, '\n')) == پوچ)
    {
	وقتی ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	بازگشت پوچ;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    اگر ((name = strtok(buf, (ثابت نویسه *)" ")) == پوچ)
	بازگشت پوچ;
    اگر ((*context = strtok(پوچ, (ثابت نویسه *)" ")) == پوچ)
	بازگشت پوچ;
    اگر ((*linenumber = strtok(پوچ, (ثابت نویسه *)" ")) == پوچ)
	بازگشت پوچ;
    *جستجو = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    اگر (strcmp(*جستجو, "<unknown>") == 0)
	*جستجو = پوچ;

    name = cs_resolve_file(cnumber, name);
    بازگشت name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    ایستا تهی
cs_file_results(FILE *f, صحیح *nummatches_a)
{
    صحیح i, j;
    نویسه *buf;
    نویسه *جستجو, *slno;
    نویسه *fullname;
    نویسه *cntx;
    نویسه *context;

    buf = alloc(CSREAD_BUFSIZE);
    اگر (buf == پوچ)
	بازگشت;

    برای (i = 0; i < csinfo_size; i++)
    {
	اگر (nummatches_a[i] < 1)
	    ادامه;

	برای (j = 0; j < nummatches_a[i]; j++)
	{
	   اگر ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &جستجو)) == پوچ)
	       ادامه;

	   context = alloc(strlen(cntx)+5);
	   اگر (context == پوچ)
	   {
	       vim_free(fullname);
	       ادامه;
	   }

	   اگر (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   وگرنه
	       sprintf(context, "<<%s>>", cntx);

	   اگر (جستجو == پوچ)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   وگرنه
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, جستجو);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(تهی)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    ایستا تهی
cs_fill_results(
    نویسه *tagstr,
    صحیح totmatches,
    صحیح *nummatches_a,
    نویسه ***matches_p,
    نویسه ***cntxts_p,
    صحیح *matched)
{
    صحیح i, j;
    نویسه *buf;
    نویسه *جستجو, *slno;
    صحیح totsofar = 0;
    نویسه **matches = پوچ;
    نویسه **cntxts = پوچ;
    نویسه *fullname;
    نویسه *cntx;

    أكد(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    اگر (buf == پوچ)
	بازگشت;

    اگر ((matches = ALLOC_MULT(نویسه *, totmatches)) == پوچ)
	برو parse_out;
    اگر ((cntxts = ALLOC_MULT(نویسه *, totmatches)) == پوچ)
	برو parse_out;

    برای (i = 0; i < csinfo_size; i++)
    {
	اگر (nummatches_a[i] < 1)
	    ادامه;

	برای (j = 0; j < nummatches_a[i]; j++)
	{
	   اگر ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &جستجو)) == پوچ)
		ادامه;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  جستجو, tagstr);

	    vim_free(fullname);

	    اگر (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = پوچ;
	    وگرنه
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (نویسه *)vim_strsave((char_u *)cntx);

	    اگر (matches[totsofar] != پوچ)
		totsofar++;

	} // for all matches

	(تهی)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    اگر (totsofar == 0)
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
    ایستا نویسه *
cs_pathcomponents(نویسه *path)
{
    صحیح		i;
    نویسه	*s;

    اگر (p_cspc == 0)
	بازگشت path;

    s = path + strlen(path) - 1;
    برای (i = 0; i < p_cspc; ++i)
    {
	وقتی (s > path)
	{
	   s--;
	   اگر (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      بشکن;
	}
    }
    اگر ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    بازگشت s;
}

/*
 * Called from cs_manage_matches().
 */
    ایستا تهی
cs_print_tags_priv(نویسه **matches, نویسه **cntxts, صحیح رقم_matches)
{
    نویسه	*buf = پوچ;
    نویسه	*t_buf;
    صحیح		bufsize = 0; // Track available bufsize
    صحیح		newsize = 0;
    نویسه	*ptag;
    نویسه	*fname, *lno, *extra, *tbuf;
    صحیح		i, idx, رقم;
    نویسه	*globalcntx = "GLOBAL";
    نویسه	*cntxformat = " <<%s>>";
    نویسه	*context;
    نویسه	*cstag_پیام = _("Cscope tag: %s");
    نویسه	*csfmt_str = "%4d %6s  ";

    أكد(رقم_matches > 0);

    اگر ((tbuf = alloc(strlen(matches[0]) + 1)) == پوچ)
	بازگشت;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    اگر (ptag == پوچ)
    {
	vim_free(tbuf);
	بازگشت;
    }

    newsize = (صحیح)(strlen(cstag_پیام) + strlen(ptag));
    buf = alloc(newsize);
    اگر (buf != پوچ)
    {
	bufsize = newsize;
	(تهی)sprintf(buf, cstag_پیام, ptag);
	پیام_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    پیام_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    پیام_advance(پیام_col + 2);
    پیام_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    رقم = 1;
    برای (i = 0; i < رقم_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	اگر ((tbuf = alloc(strlen(matches[idx]) + 1)) == پوچ)
	    ادامه;
	(تهی)strcpy(tbuf, matches[idx]);

	اگر (strtok(tbuf, (ثابت نویسه *)"\t") == پوچ
		|| (fname = strtok(پوچ, (ثابت نویسه *)"\t")) == پوچ
		|| (lno = strtok(پوچ, (ثابت نویسه *)"\t")) == پوچ)
	{
	    vim_free(tbuf);
	    ادامه;
	}
	extra = strtok(پوچ, (ثابت نویسه *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (صحیح)(strlen(csfmt_str) + 16 + strlen(lno));
	اگر (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    اگر (buf == پوچ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    وگرنه
		bufsize = newsize;
	}
	اگر (buf != پوچ)
	{
	    // csfmt_str = "%4d %6s  ";
	    (تهی)sprintf(buf, csfmt_str, رقم, lno);
	    پیام_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	پیام_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	اگر (cntxts[idx] != پوچ)
	    context = cntxts[idx];
	وگرنه
	    context = globalcntx;
	newsize = (صحیح)(strlen(context) + strlen(cntxformat));

	اگر (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    اگر (buf == پوچ)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    وگرنه
		bufsize = newsize;
	}
	اگر (buf != پوچ)
	{
	    (تهی)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    اگر (پیام_col + (صحیح)strlen(buf) >= cmdline_width)
		پیام_putchar('\n');
	    پیام_advance(12);
	    پیام_outtrans_long_attr((char_u *)buf, 0);
	    پیام_putchar('\n');
	}
	اگر (extra != پوچ)
	{
	    پیام_advance(13);
	    پیام_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	اگر (پیام_col)
	    پیام_putchar('\n');

	ui_breakcheck();
	اگر (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    بشکن;
	}

	رقم++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    ایستا صحیح
cs_read_prompt(صحیح i)
{
    صحیح		ch;
    نویسه	*buf = پوچ; // buffer for possible error message from cscope
    صحیح		bufpos = 0;
    صحیح		maxlen;
    ایستا نویسه *eprompt = "Press the RETURN key to continue:";
    صحیح		epromptlen = (صحیح)strlen(eprompt);
    صحیح		n;

    // compute maximum allowed len for Cscope error message
    نویسه *cs_emsg = _(e_cscope_error_str);
    maxlen = (صحیح)(IOSIZE - strlen(cs_emsg));

    برای (;;)
    {
	وقتی ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    اگر (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		اگر (buf == پوچ) // lazy buffer allocation
		    buf = alloc(maxlen);
		اگر (buf != پوچ)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    اگر (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(تهی)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(تهی)putc('\n', csinfo[i].to_fp);
			(تهی)fflush(csinfo[i].to_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	برای (n = 0; n < (صحیح)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    اگر (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    اگر (ch == EOF)
	    {
		اگر (buf != پوچ && buf[0] != NUL)
		    (تهی)semsg(cs_emsg, buf);
		وگرنه اگر (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		بازگشت CSCOPE_FAILURE;
	    }

	    اگر (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		بشکن;
	    }
	}

	اگر (ch == EOF)
	    ادامه;	    // didn't find the prompt
	بشکن;		    // did find the prompt
    }

    vim_free(buf);
    بازگشت CSCOPE_SUCCESS;
}

# اگر defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    ایستا تهی
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    ایستا تهی
cs_release_csp(صحیح i, صحیح freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    اگر (csinfo[i].to_fp != پوچ)
    {
	(تهی)fputs("q\n", csinfo[i].to_fp);
	(تهی)fflush(csinfo[i].to_fp);
    }
# اگر defined(UNIX)
    {
	صحیح waitpid_errno;
	صحیح pstat;
	pid_t pid;

#  اگر defined(HAVE_SIGACTION)
	ساختار sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   وگرنه
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, پوچ);
#  وگرنه
	صحیح waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	برای (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    اگر (pid != 0)
		بشکن;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	اگر (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    صحیح alive = TRUE;

	    اگر (waitpid_errno == ECHILD)
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
		صحیح waited;

		sleep(0);
		برای (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    اگر (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			بشکن;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    اگر (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(تهی)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# وگرنه  // !UNIX
    اگر (csinfo[i].hProc != پوچ)
    {
	// Give cscope a chance to exit normally
	اگر (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    اگر (csinfo[i].fr_fp != پوچ)
	(تهی)fclose(csinfo[i].fr_fp);
    اگر (csinfo[i].to_fp != پوچ)
	(تهی)fclose(csinfo[i].to_fp);

    اگر (freefnpp)
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
    ایستا صحیح
cs_reset(exarg_T *eap UNUSED)
{
    نویسه	**dblist = پوچ, **pplist = پوچ, **fllist = پوچ;
    صحیح	i;
    نویسه buf[20]; // for sprintf " (#%d)"

    اگر (csinfo_size == 0)
	بازگشت CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(نویسه *, csinfo_size);
    pplist = ALLOC_MULT(نویسه *, csinfo_size);
    fllist = ALLOC_MULT(نویسه *, csinfo_size);
    اگر (dblist == پوچ || pplist == پوچ || fllist == پوچ)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	بازگشت CSCOPE_FAILURE;
    }

    برای (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	اگر (csinfo[i].fname != پوچ)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    برای (i = 0; i < csinfo_size; i++)
    {
	اگر (dblist[i] != پوچ)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    اگر (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		پیام_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    اگر (p_csverbose)
	پیام_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | پیام_HIST);
    بازگشت CSCOPE_SUCCESS;
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
    ایستا نویسه *
cs_resolve_file(صحیح i, نویسه *name)
{
    نویسه	*fullname;
    string_T	csdir = {پوچ, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    صحیح		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (صحیح)namelen + 2;
    اگر (csinfo[i].ppath != پوچ)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (صحیح)ppathlen;
    }
    وگرنه اگر (p_csre && csinfo[i].fname != پوچ)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	اگر (csdir.string != پوچ)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (صحیح)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    اگر (csinfo[i].ppath != پوچ
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	اگر ((fullname = alloc(len)) != پوچ)
	    (تهی)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    وگرنه اگر (csdir.string != پوچ && csinfo[i].fname != پوچ && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (نویسه *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    وگرنه
    {
	fullname = (نویسه *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    بازگشت fullname;
}


/*
 * Show all cscope connections.
 */
    ایستا صحیح
cs_show(exarg_T *eap UNUSED)
{
    صحیح i;

    اگر (cs_cnt_connections() == 0)
	پیام_puts(_("no cscope connections\n"));
    وگرنه
    {
	پیام_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	برای (i = 0; i < csinfo_size; i++)
	{
	    اگر (csinfo[i].fname == پوچ)
		ادامه;

	    اگر (csinfo[i].ppath != پوچ)
		(تهی)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    وگرنه
		(تهی)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    بازگشت CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    تهی
cs_end(تهی)
{
    صحیح i;

    برای (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#اگر defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    تهی
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    صحیح		رقم = 0;
    char_u	*dbpath = پوچ;
    char_u	*prepend = پوچ;
    char_u	buf[NUMBUFLEN];

    اگر (in_vim9script()
	    && (check_for_opt_عدد_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	رقم = (صحیح)tv_get_عدد(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_عدد = cs_connection(رقم, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
