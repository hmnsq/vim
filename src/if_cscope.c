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

#ضمن "vim.h"

#אם defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# אם defined(UNIX)
#  include <sys/wait.h>
# endif

# אם defined (MSWIN)
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

تعريفنوع بنية {
    حرف *  name;
    صحيح     (*func)(exarg_T *eap);
    حرف *  help;
    حرف *  usage;
    صحيح	    cansplit;		// if supports splitting window
} cscmd_T;

تعريفنوع بنية csi {
    حرف *	    fname;	// cscope db name
    حرف *	    ppath;	// path to prepend (the -P option)
    حرف *	    flags;	// additional cscope flags/options (e.g, -p2)
# אם defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# אחרת
#  אם defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    nIndexHigh;	// st_ino has no meaning in the Windows
    DWORD	    nIndexLow;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    to_fp;	// to cscope: FILE.
} csinfo_T;

تعريفنوع تعداد { Add, Find, Help, Kill, Reset, Show } csid_e;

تعريفنوع تعداد {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

ساكن صحيح	    cs_add(exarg_T *eap);
ساكن صحيح	    cs_add_common(حرف *, حرف *, حرف *);
ساكن صحيح	    cs_check_for_connections(فراغ);
ساكن صحيح	    cs_check_for_tags(فراغ);
ساكن صحيح	    cs_cnt_connections(فراغ);
ساكن صحيح	    cs_create_connection(صحيح i);
# ifdef FEAT_QUICKFIX
ساكن فراغ	    cs_file_results(FILE *, صحيح *);
# endif
ساكن فراغ	    cs_fill_results(حرف *, صحيح , صحيح *, حرف ***,
			حرف ***, صحيح *);
ساكن صحيح	    cs_find(exarg_T *eap);
ساكن صحيح	    cs_find_common(حرف *opt, حرف *pat, صحيح, صحيح, صحيح, char_u *cmdline);
ساكن صحيح	    cs_help(exarg_T *eap);
ساكن صحيح	    cs_insert_filelist(حرف *, حرف *, حرف *,
			stat_T *);
ساكن صحيح	    cs_kill(exarg_T *eap);
ساكن فراغ	    cs_kill_execute(صحيح, حرف *);
ساكن cscmd_T *    cs_lookup_cmd(exarg_T *eap);
ساكن حرف *	    cs_make_vim_style_matches(حرف *, حرف *,
			حرف *, حرف *);
ساكن حرف *	    cs_manage_matches(حرف **, حرف **, صحيح, mcmd_e);
ساكن فراغ	    cs_print_tags_priv(حرف **, حرف **, صحيح);
ساكن صحيح	    cs_read_prompt(صحيح);
ساكن فراغ	    cs_release_csp(صحيح, صحيح freefnpp);
ساكن صحيح	    cs_reset(exarg_T *eap);
ساكن حرف *	    cs_resolve_file(صحيح, حرف *);
ساكن صحيح	    cs_show(exarg_T *eap);


ساكن csinfo_T *   csinfo = عدم;
ساكن صحيح	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

ساكن صحيح	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
ساكن cscmd_T	    cs_cmds[] =
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
    { عدم, عدم, عدم, عدم, 0 }
};

    ساكن فراغ
cs_usage_msg(csid_e x)
{
    (فراغ)semsg(_(e_usage_cscope_str), cs_cmds[(صحيح)x].usage);
}

ساكن تعداد
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
get_cscope_name(expand_T *xp UNUSED, صحيح idx)
{
    صحيح		current_idx;
    صحيح		i;

    بدل (expand_what)
    {
    حالة EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	החזר (char_u *)cs_cmds[idx].name;
    حالة EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	עבור (i = 0, current_idx = 0; cs_cmds[i].name != عدم; i++)
	    אם (cs_cmds[i].cansplit)
		אם (current_idx++ == idx)
		    اكسر;
	החזר (char_u *)cs_cmds[i].name;
    حالة EXP_CSCOPE_FIND:
	{
	    ثابت حرف *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", عدم
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    החזר (char_u *)query_type[idx];
	}
    حالة EXP_CSCOPE_KILL:
	{
	    ساكن حرف	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    עבור (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		אם (csinfo[i].fname == عدم)
		    استمر;
		אם (current_idx++ == idx)
		{
		    vim_snprintf(connection, حجم(connection), "%d", i);
		    החזר (char_u *)connection;
		}
	    }
	    החזר (current_idx == idx && idx > 0) ? (char_u *)"-1" : عدم;
	}
    افتراضي:
	החזר عدم;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    فراغ
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

    אם (*arg == NUL)
	החזר;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    אם (*p == NUL)
	החזר;

    // past first word
    xp->xp_pattern = skipwhite(p);
    אם (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    אחרת אם (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    אחרת אם (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    אחרת אם (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    אחרת
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    ساكن فراغ
do_cscope_general(
    exarg_T	*eap,
    صحيح		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    אם (check_restricted())
	החזר;

    אם ((cmdp = cs_lookup_cmd(eap)) == عدم)
    {
	cs_help(eap);
	החזר;
    }

    אם (make_split)
    {
	אם (!cmdp->cansplit)
	{
	    (فراغ)msg_puts(_("This cscope command does not support splitting the window.\n"));
	    החזר;
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
    فراغ
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    فراغ
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    فراغ
ex_cstag(exarg_T *eap)
{
    صحيح ret = FALSE;

    אם (*eap->arg == NUL)
    {
	(فراغ)emsg(_(e_usage_cstag_ident));
	החזר;
    }

    بدل (p_csto)
    {
    حالة 0 :
	אם (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (حرف *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    אם (ret == FALSE)
	    {
		cs_free_tags();
		אם (msg_col)
		    msg_putchar('\n');

		אם (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	אחרת אם (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	اكسر;
    حالة 1 :
	אם (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    אם (ret == FALSE)
	    {
		אם (msg_col)
		    msg_putchar('\n');

		אם (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (حرف *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    אם (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	אחרת אם (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (حرف *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    אם (ret == FALSE)
		cs_free_tags();
	}
	اكسر;
    افتراضي :
	اكسر;
    }

    אם (!ret)
    {
	(فراغ)emsg(_(e_cstag_tag_not_founc));
# אם defined(FEAT_QUICKFIX)
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
    صحيح
cs_fgets(char_u *buf, صحيح size)
{
    حرف *p;

    אם ((p = cs_manage_matches(عدم, عدم, -1, Get)) == عدم)
	החזר TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    החזר FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    فراغ
cs_free_tags(فراغ)
{
    cs_manage_matches(عدم, عدم, -1, Free);
}


/*
 * Called from do_tag().
 */
    فراغ
cs_print_tags(فراغ)
{
    cs_manage_matches(عدم, عدم, -1, Print);
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
# אם defined(FEAT_EVAL)
    ساكن صحيح
cs_connection(صحيح num, char_u *dbpath, char_u *ppath)
{
    صحيح i;

    אם (num < 0 || num > 4 || (num > 0 && !dbpath))
	החזר FALSE;

    עבור (i = 0; i < csinfo_size; i++)
    {
	אם (!csinfo[i].fname)
	    استمر;

	אם (num == 0)
	    החזר TRUE;

	بدل (num)
	{
	حالة 1:
	    אם (strstr(csinfo[i].fname, (حرف *)dbpath))
		החזר TRUE;
	    اكسر;
	حالة 2:
	    אם (strcmp(csinfo[i].fname, (حرف *)dbpath) == 0)
		החזר TRUE;
	    اكسر;
	حالة 3:
	    אם (strstr(csinfo[i].fname, (حرف *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (حرف *)ppath))))
		החזר TRUE;
	    اكسر;
	حالة 4:
	    אם ((strcmp(csinfo[i].fname, (حرف *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (حرف *)ppath) == 0))))
		החזר TRUE;
	    اكسر;
	}
    }

    החזר FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    ساكن صحيح
cs_add(exarg_T *eap UNUSED)
{
    حرف *fname, *ppath, *flags = عدم;

    אם ((fname = strtok((حرف *)عدم, (ثابت حرف *)" ")) == عدم)
    {
	cs_usage_msg(Add);
	החזר CSCOPE_FAILURE;
    }
    אם ((ppath = strtok((حرف *)عدم, (ثابت حرف *)" ")) != عدم)
	flags = strtok((حرف *)عدم, (ثابت حرف *)" ");

    החזר cs_add_common(fname, ppath, flags);
}

    ساكن فراغ
cs_stat_emsg(حرف *fname)
{
    صحيح err = errno;
    (فراغ)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    ساكن صحيح
cs_add_common(
    حرف *arg1,	    // filename - may contain environment variables
    حرف *arg2,	    // prepend path - may contain environment variables
    حرف *flags)
{
    stat_T	statbuf;
    صحيح		ret;
    حرف	*fname = عدم;
    حرف	*fname2 = عدم;
    حرف	*ppath = عدم;
    صحيح		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = عدم;

    // get the filename (arg1), expand it, and try to stat it
    אם ((fname = alloc(MAXPATHL + 1)) == عدم)
	اذهب add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (فراغ)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    אם (fname == عدم)
	اذهب add_err;
    fname = (حرف *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    אם (ret < 0)
    {
staterr:
	אם (p_csverbose)
	    cs_stat_emsg(fname);
	اذهب add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    אם (arg2 != عدم)
    {
	stat_T	    statbuf2;

	אם ((ppath = alloc(MAXPATHL + 1)) == عدم)
	    اذهب add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	אם (ret < 0)
	    اذهب staterr;
    }

    // if filename is a directory, append the cscope database name to it
    אם (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	אם (fname2 == عدم)
	    اذهب add_err;

	כלעוד (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    אם (fname[0] == '\0')
		اكسر;
	}
	אם (fname[0] == '\0')
	    (فراغ)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	אחרת
	    (فراغ)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	אם (ret < 0)
	{
	    אם (p_csverbose)
		cs_stat_emsg(fname2);
	    اذهب add_err;
	}

	i = cs_insert_filelist(fname2, ppath, flags, &statbuf);
    }
    אחרת אם (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_insert_filelist(fname, ppath, flags, &statbuf);
    }
    אחרת
    {
	אם (p_csverbose)
	    (فراغ)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	اذهب add_err;
    }

    אם (i != -1)
    {
	אם (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    اذهب add_err;
	}

	אם (p_csverbose)
	{
	    msg_clr_eos();
	    (فراغ)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    החזר CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    החזר CSCOPE_FAILURE;
}


    ساكن صحيح
cs_check_for_connections(فراغ)
{
    החזר (cs_cnt_connections() > 0);
}


    ساكن صحيح
cs_check_for_tags(فراغ)
{
    החזר (p_tags[0] != NUL && curbuf->b_p_tags != عدم);
}


/*
 * Count the number of cscope connections.
 */
    ساكن صحيح
cs_cnt_connections(فراغ)
{
    صحيح i;
    صحيح cnt = 0;

    עבור (i = 0; i < csinfo_size; i++)
	אם (csinfo[i].fname != عدم)
	    cnt++;
    החזר cnt;
}

    ساكن فراغ
cs_reading_emsg(
    صحيح idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    ساكن صحيح
cs_cnt_matches(صحيح idx)
{
    حرف *stok;
    حرف *buf;
    صحيح nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    אם (buf == عدم)
	החזר 0;
    עבור (;;)
    {
	אם (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    אם (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    החזר -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	אם (strstr((ثابت حرف *)buf, "Unable to search database") != عدم)
	    اكسر;
	אם ((stok = strtok(buf, (ثابت حرف *)" ")) == عدم)
	    استمر;
	אם (strstr((ثابت حرف *)stok, "cscope:") == عدم)
	    استمر;

	אם ((stok = strtok(عدم, (ثابت حرف *)" ")) == عدم)
	    استمر;
	nlines = atoi(stok);
	אם (nlines < 0)
	{
	    nlines = 0;
	    اكسر;
	}

	אם ((stok = strtok(عدم, (ثابت حرف *)" ")) == عدم)
	    استمر;
	אם (strncmp((ثابت حرف *)stok, "lines", 5))
	    استمر;

	اكسر;
    }

    vim_free(buf);
    החזר nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    ساكن حرف *
cs_create_cmd(حرف *csoption, حرف *pattern)
{
    حرف *cmd;
    short search;
    حرف *pat;

    بدل (csoption[0])
    {
    حالة '0' : حالة 's' :
	search = 0;
	اكسر;
    حالة '1' : حالة 'g' :
	search = 1;
	اكسر;
    حالة '2' : حالة 'd' :
	search = 2;
	اكسر;
    حالة '3' : حالة 'c' :
	search = 3;
	اكسر;
    حالة '4' : حالة 't' :
	search = 4;
	اكسر;
    حالة '6' : حالة 'e' :
	search = 6;
	اكسر;
    حالة '7' : حالة 'f' :
	search = 7;
	اكسر;
    حالة '8' : حالة 'i' :
	search = 8;
	اكسر;
    حالة '9' : حالة 'a' :
	search = 9;
	اكسر;
    افتراضي :
	(فراغ)emsg(_(e_unknown_cscope_search_type));
	cs_usage_msg(Find);
	החזר عدم;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    אם (search != 4 && search != 6)
	כלעוד VIM_ISWHITE(*pat)
	    ++pat;

    אם ((cmd = alloc(strlen(pat) + 2)) == عدم)
	החזר عدم;

    (فراغ)sprintf(cmd, "%d%s", search, pat);

    החזר cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    ساكن صحيح
cs_create_connection(صحيح i)
{
# ifdef UNIX
    صحيح		to_cs[2], from_cs[2];
# endif
    صحيح		cmdlen;
    حرف	*prog, *cmd, *ppath = عدم;
    size_t	proglen;
# ifdef MSWIN
    صحيح		fd;
    صحيح		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# אם defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    אם (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(فراغ)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	אם (to_cs[0] != -1)
	    (فراغ)close(to_cs[0]);
	אם (to_cs[1] != -1)
	    (فراغ)close(to_cs[1]);
	אם (from_cs[0] != -1)
	    (فراغ)close(from_cs[0]);
	אם (from_cs[1] != -1)
	    (فراغ)close(from_cs[1]);
	החזר CSCOPE_FAILURE;
    }

    אם ((csinfo[i].pid = fork()) == -1)
    {
	(فراغ)emsg(_(e_could_not_fork_for_cscope));
	اذهب err_closing;
    }
    אחרת אם (csinfo[i].pid == 0)	// child: run cscope.
    {
	حرف **argv = عدم;

	אם (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	אם (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	אם (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(فراغ)close(to_cs[1]);
	(فراغ)close(from_cs[0]);
# אחרת
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = حجم(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = عدم;

	אם (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (فراغ)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    אם (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    אם (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    החזר CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	אם ((prog = alloc(MAXPATHL + 1)) == عدم)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# אחרת
	    // MSWIN
	    اذهب err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (صحيح)(proglen + strlen(csinfo[i].fname) + 32);
	אם (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    אם ((ppath = alloc(MAXPATHL + 1)) == عدم)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# אחרת
		// MSWIN
		اذهب err_closing;
# endif
	    }
	    cmdlen += (صحيح)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	אם (csinfo[i].flags)
	    cmdlen += (صحيح)strlen(csinfo[i].flags);

	אם ((cmd = alloc(cmdlen)) == عدم)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# אחרת
	    // MSWIN
	    اذهب err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    حرف	**tok = عدم;
	    صحيح		tokc = 0;

	    ga_init2(&ga_argv, حجم(حرف *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    אם (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    עבור (صحيح k = 0; k < tokc; ++k)
	    {
		אם (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((حرف **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    אם (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((حرف **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (حرف *)vim_strsave((char_u *)"-dl");
	    ((حرف **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (حرف *)vim_strsave((char_u *)"-f");
	    ((حرف **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(حرف *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    אם (ppath != عدم)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		حرف	*parg = alloc(plen);

		אם (parg == عدم || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((حرف **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    אם (csinfo[i].flags != عدم)
	    {
		אם (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		עבור (صحيح k = 0; k < tokc; ++k)
		{
		    אם (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((حرف **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    אם (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((حرف **)ga_argv.ga_data)[ga_argv.ga_len] = عدم;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  אם defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   אם defined(HAVE_SETSID)
	    (فراغ)setsid();
#   אחרת
	    אם (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (حرف **)ga_argv.ga_data;
	    אם (argv[0] == عدم || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    אחרת	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	אם ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == عدم)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	אם ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == عدم)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(فراغ)close(to_cs[0]);
	(فراغ)close(from_cs[1]);
    }
# אחרת
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    אם (csinfo[i].ppath != عدم)
    {
	len = (صحيح)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    אם (csinfo[i].flags != عدم)
    {
	len = (صحيح)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.hStdInput  = stdin_rd;
    created = CreateProcess(عدم, cmd, عدم, عدم, TRUE, CREATE_NEW_CONSOLE,
							عدم, عدم, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    אם (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(فراغ)emsg(_(e_could_not_spawn_cscope_process));
	اذهب err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    אם (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == عدم))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    אם (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == عدم))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    החזר CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    ساكن صحيح
cs_find(exarg_T *eap)
{
    حرف *opt, *pat;
    صحيح i;

    אם (cs_check_for_connections() == FALSE)
    {
	(فراغ)emsg(_(e_no_cscope_connections));
	החזר FALSE;
    }

    אם ((opt = strtok((حرف *)عدم, (ثابت حرف *)" ")) == عدم)
    {
	cs_usage_msg(Find);
	החזר FALSE;
    }

    pat = opt + strlen(opt) + 1;
    אם (pat >= (حرف *)eap->arg + eap_arg_len)
    {
	cs_usage_msg(Find);
	החזר FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    עבור (i = 0; i < eap_arg_len; ++i)
	אם (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    החזר cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    ساكن صحيح
cs_find_common(
    حرف *opt,
    حرف *pat,
    صحيح forceit,
    صحيح verbose,
    صحيح	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    صحيح i;
    حرف *cmd;
    صحيح *nummatches;
    صحيح totmatches;
# ifdef FEAT_QUICKFIX
    حرف cmdletter;
    حرف *qfpos;

    // get cmd letter
    بدل (opt[0])
    {
    حالة '0' :
	cmdletter = 's';
	اكسر;
    حالة '1' :
	cmdletter = 'g';
	اكسر;
    حالة '2' :
	cmdletter = 'd';
	اكسر;
    حالة '3' :
	cmdletter = 'c';
	اكسر;
    حالة '4' :
	cmdletter = 't';
	اكسر;
    حالة '6' :
	cmdletter = 'e';
	اكسر;
    حالة '7' :
	cmdletter = 'f';
	اكسر;
    حالة '8' :
	cmdletter = 'i';
	اكسر;
    حالة '9' :
	cmdletter = 'a';
	اكسر;
    افتراضي :
	cmdletter = opt[0];
    }

    qfpos = (حرف *)vim_strchr(p_csqf, cmdletter);
    אם (qfpos != عدم)
    {
	qfpos++;
	// next symbol must be + or -
	אם (strchr(CSQF_FLAGS, *qfpos) == عدم)
	{
	    (فراغ)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    החזר FALSE;
	}

	אם (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    אם (aborting())
		החזר FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    אם (cmd == عدم)
	החזר FALSE;

    nummatches = ALLOC_MULT(صحيح, csinfo_size);
    אם (nummatches == عدم)
    {
	vim_free(cmd);
	החזר FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    עבור (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    עבור (i = 0; i < csinfo_size; i++)
    {
	אם (csinfo[i].fname == عدم || csinfo[i].to_fp == عدم)
	    استمر;

	// send cmd to cscope
	(فراغ)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(فراغ)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	אם (nummatches[i] > -1)
	    totmatches += nummatches[i];

	אם (nummatches[i] == 0)
	    (فراغ)cs_read_prompt(i);
    }
    vim_free(cmd);

    אם (totmatches == 0)
    {
	אם (verbose)
	    (فراغ)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	החזר FALSE;
    }

# ifdef FEAT_QUICKFIX
    אם (qfpos != عدم && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = عدم;
	win_T	    *wp = عدم;

	אם (tmp == عدم)
	{
	    vim_free(nummatches);
	    החזר FALSE;
	}

	f = mch_fopen((حرف *)tmp, "w");
	אם (f == عدم)
	    semsg(_(e_cant_open_file_str), tmp);
	אחרת
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    אם (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    אם (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, عدم) > 0)
	    {
		אם (postponed_split != 0)
		{
		    (فراغ)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		אם (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != عدم)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	החזר TRUE;
    }
    אחרת
# endif // FEAT_QUICKFIX
    {
	حرف **matches = عدم, **contexts = عدم;
	صحيح matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	אם (matches == عدم)
	    החזר FALSE;

	(فراغ)cs_manage_matches(matches, contexts, matched, Store);

	החזר do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    ساكن صحيح
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (فراغ)msg_puts(_("cscope commands:\n"));
    כלעוד (cmdp->name != عدم)
    {
	حرف *help = _(cmdp->help);
	صحيح  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	אם (space_cnt < 0)
	    space_cnt = 0;
	(فراغ)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	אם (strcmp(cmdp->name, "find") == 0)
	    msg_puts(_("\n"
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
    החזר 0;
}


    ساكن فراغ
clear_csinfo(صحيح i)
{
    csinfo[i].fname  = عدم;
    csinfo[i].ppath  = عدم;
    csinfo[i].flags  = عدم;
# אם defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# אחרת
    csinfo[i].nVolume = 0;
    csinfo[i].nIndexHigh = 0;
    csinfo[i].nIndexLow = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = عدم;
    csinfo[i].to_fp  = عدم;
# אם defined(MSWIN)
    csinfo[i].hProc = عدم;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    ساكن صحيح
cs_insert_filelist(
    حرف *fname,
    حرف *ppath,
    حرف *flags,
    stat_T *sb UNUSED)
{
    صحيح	    i;
    صحيح	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    بدل (win32_fileinfo((char_u *)fname, &bhfi))
    {
	حالة FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	حالة FILEINFO_READ_FAIL:	// CreateFile() failed
	    אם (p_csverbose)
	    {
		حرف *cant_msg = _(e_cannot_open_cscope_database_str);
		حرف *winmsg = GetWin32Error();

		אם (winmsg != عدم)
		    (فراغ)semsg(cant_msg, winmsg);
		אחרת
		    // subst filename if can't get error text
		    (فراغ)semsg(cant_msg, fname);
	    }
	    החזר -1;

	حالة FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    אם (p_csverbose)
		(فراغ)emsg(_(e_cannot_get_cscope_database_information));
	    החזר -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    עבור (j = 0; j < csinfo_size; j++)
    {
	אם (csinfo[j].fname != عدم
# אם defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# אחרת
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dwVolumeSerialNumber
		    && csinfo[j].nIndexHigh == bhfi.nFileIndexHigh
		    && csinfo[j].nIndexLow == bhfi.nFileIndexLow))
# endif
	    )
	{
	    אם (p_csverbose)
		(فراغ)emsg(_(e_duplicate_cscope_database_not_added));
	    החזר -1;
	}

	אם (csinfo[j].fname == عدم && i == -1)
	    i = j; // remember first empty entry
    }

    אם (i == -1)
    {
	i = csinfo_size;
	אם (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	אחרת
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, حجم(csinfo_T)*csinfo_size);
	    אם (csinfo == عدم)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		החזר -1;
	    }
	}
	אם (csinfo == عدم)
	    החזר -1;
	עבור (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    אם ((csinfo[i].fname = alloc(strlen(fname)+1)) == عدم)
	החזר -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((ثابت حرف *)fname));

    אם (ppath != عدم)
    {
	אם ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == عدم)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    החזר -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((ثابت حرف *)ppath));
    }
    אחרת
	csinfo[i].ppath = عدم;

    אם (flags != عدم)
    {
	אם ((csinfo[i].flags = alloc(strlen(flags) + 1)) == عدم)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    החזר -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((ثابت حرف *)flags));
    }
    אחרת
	csinfo[i].flags = عدم;

# אם defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# אחרת
    csinfo[i].nVolume = bhfi.dwVolumeSerialNumber;
    csinfo[i].nIndexLow = bhfi.nFileIndexLow;
    csinfo[i].nIndexHigh = bhfi.nFileIndexHigh;
# endif
    החזר i;
}


/*
 * Find cscope command in command table.
 */
    ساكن cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    حرف *stok;
    size_t len;

    אם (eap->arg == عدم)
	החזר عدم;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (صحيح)STRLEN(eap->arg);

    אם ((stok = strtok((حرف *)(eap->arg), (ثابت حرف *)" ")) == عدم)
	החזר عدم;

    len = strlen(stok);
    עבור (cmdp = cs_cmds; cmdp->name != عدم; ++cmdp)
    {
	אם (strncmp((ثابت حرف *)(stok), cmdp->name, len) == 0)
	    החזר (cmdp);
    }
    החזר عدم;
}


/*
 * Nuke em.
 */
    ساكن صحيح
cs_kill(exarg_T *eap UNUSED)
{
    حرف *stok;
    صحيح i;

    אם ((stok = strtok((حرف *)عدم, (ثابت حرف *)" ")) == عدم)
    {
	cs_usage_msg(Kill);
	החזר CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    אם ((strlen(stok) < 2 && VIM_ISDIGIT((صحيح)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((صحيح)(stok[1]))))
	i = atoi(stok);
    אחרת
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	עבור (i = 0; i < csinfo_size; i++)
	{
	    אם (csinfo[i].fname != عدم && strstr(csinfo[i].fname, stok))
		اكسر;
	}
    }

    אם ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == عدم))
    {
	אם (p_csverbose)
	    (فراغ)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    אחרת
    {
	אם (i == -1)
	{
	    עבור (i = 0; i < csinfo_size; i++)
	    {
		אם (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	אחרת
	    cs_kill_execute(i, stok);
    }

    החזר 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    ساكن فراغ
cs_kill_execute(
    صحيح i,	    // cscope table index
    حرف *cname)    // cscope database name
{
    אם (p_csverbose)
    {
	msg_clr_eos();
	(فراغ)smsg_attr(HL_ATTR(HLF_R) | MSG_HIST,
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
    ساكن حرف *
cs_make_vim_style_matches(
    حرف *fname,
    حرف *slno,
    حرف *search,
    حرف *tagstr)
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
    حرف *buf;
    صحيح amt;

    אם (search != عدم)
    {
	amt = (صحيح)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(search)+6);
	אם ((buf = alloc(amt)) == عدم)
	    החזר عدم;

	(فراغ)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, search);
    }
    אחרת
    {
	amt = (صحيح)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	אם ((buf = alloc(amt)) == عدم)
	    החזר عدم;

	(فراغ)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    החזר buf;
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
    ساكن حرف *
cs_manage_matches(
    حرف **matches,
    حرف **contexts,
    صحيح totmatches,
    mcmd_e cmd)
{
    ساكن حرف **mp = عدم;
    ساكن حرف **cp = عدم;
    ساكن صحيح cnt = -1;
    ساكن صحيح next = -1;
    حرف *p = عدم;

    بدل (cmd)
    {
    حالة Store:
	أكد(matches != عدم);
	أكد(totmatches > 0);
	אם (mp != عدم || cp != عدم)
	    (فراغ)cs_manage_matches(عدم, عدم, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	اكسر;
    حالة Get:
	אם (next >= cnt)
	    החזר عدم;

	p = mp[next];
	next++;
	اكسر;
    حالة Free:
	אם (mp != عدم)
	{
	    אם (cnt > 0)
		כלעוד (cnt--)
		{
		    vim_free(mp[cnt]);
		    אם (cp != عدم)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = عدم;
	cp = عدم;
	cnt = 0;
	next = 0;
	اكسر;
    حالة Print:
	cs_print_tags_priv(mp, cp, cnt);
	اكسر;
    افتراضي:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	החזר عدم;
    }

    החזר p;
}


/*
 * Parse cscope output.
 */
    ساكن حرف *
cs_parse_results(
    صحيح cnumber,
    حرف *buf,
    صحيح bufsize,
    حرف **context,
    حرف **linenumber,
    حرف **search)
{
    صحيح ch;
    حرف *p;
    حرف *name;

    אם (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == عدم)
    {
	אם (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	החזר عدم;
    }

    // If the line's too long for the buffer, discard it.
    אם ((p = strchr(buf, '\n')) == عدم)
    {
	כלעוד ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	החזר عدم;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    אם ((name = strtok(buf, (ثابت حرف *)" ")) == عدم)
	החזר عدم;
    אם ((*context = strtok(عدم, (ثابت حرف *)" ")) == عدم)
	החזר عدم;
    אם ((*linenumber = strtok(عدم, (ثابت حرف *)" ")) == عدم)
	החזר عدم;
    *search = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    אם (strcmp(*search, "<unknown>") == 0)
	*search = عدم;

    name = cs_resolve_file(cnumber, name);
    החזר name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    ساكن فراغ
cs_file_results(FILE *f, صحيح *nummatches_a)
{
    صحيح i, j;
    حرف *buf;
    حرف *search, *slno;
    حرف *fullname;
    حرف *cntx;
    حرف *context;

    buf = alloc(CSREAD_BUFSIZE);
    אם (buf == عدم)
	החזר;

    עבור (i = 0; i < csinfo_size; i++)
    {
	אם (nummatches_a[i] < 1)
	    استمر;

	עבור (j = 0; j < nummatches_a[i]; j++)
	{
	   אם ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &search)) == عدم)
	       استمر;

	   context = alloc(strlen(cntx)+5);
	   אם (context == عدم)
	   {
	       vim_free(fullname);
	       استمر;
	   }

	   אם (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   אחרת
	       sprintf(context, "<<%s>>", cntx);

	   אם (search == عدم)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   אחרת
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, search);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(فراغ)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    ساكن فراغ
cs_fill_results(
    حرف *tagstr,
    صحيح totmatches,
    صحيح *nummatches_a,
    حرف ***matches_p,
    حرف ***cntxts_p,
    صحيح *matched)
{
    صحيح i, j;
    حرف *buf;
    حرف *search, *slno;
    صحيح totsofar = 0;
    حرف **matches = عدم;
    حرف **cntxts = عدم;
    حرف *fullname;
    حرف *cntx;

    أكد(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    אם (buf == عدم)
	החזר;

    אם ((matches = ALLOC_MULT(حرف *, totmatches)) == عدم)
	اذهب parse_out;
    אם ((cntxts = ALLOC_MULT(حرف *, totmatches)) == عدم)
	اذهب parse_out;

    עבור (i = 0; i < csinfo_size; i++)
    {
	אם (nummatches_a[i] < 1)
	    استمر;

	עבור (j = 0; j < nummatches_a[i]; j++)
	{
	   אם ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &search)) == عدم)
		استمر;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  search, tagstr);

	    vim_free(fullname);

	    אם (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = عدم;
	    אחרת
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (حرف *)vim_strsave((char_u *)cntx);

	    אם (matches[totsofar] != عدم)
		totsofar++;

	} // for all matches

	(فراغ)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    אם (totsofar == 0)
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
    ساكن حرف *
cs_pathcomponents(حرف *path)
{
    صحيح		i;
    حرف	*s;

    אם (p_cspc == 0)
	החזר path;

    s = path + strlen(path) - 1;
    עבור (i = 0; i < p_cspc; ++i)
    {
	כלעוד (s > path)
	{
	   s--;
	   אם (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      اكسر;
	}
    }
    אם ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    החזר s;
}

/*
 * Called from cs_manage_matches().
 */
    ساكن فراغ
cs_print_tags_priv(حرف **matches, حرف **cntxts, صحيح num_matches)
{
    حرف	*buf = عدم;
    حرف	*t_buf;
    صحيح		bufsize = 0; // Track available bufsize
    صحيح		newsize = 0;
    حرف	*ptag;
    حرف	*fname, *lno, *extra, *tbuf;
    صحيح		i, idx, num;
    حرف	*globalcntx = "GLOBAL";
    حرف	*cntxformat = " <<%s>>";
    حرف	*context;
    حرف	*cstag_msg = _("Cscope tag: %s");
    حرف	*csfmt_str = "%4d %6s  ";

    أكد(num_matches > 0);

    אם ((tbuf = alloc(strlen(matches[0]) + 1)) == عدم)
	החזר;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    אם (ptag == عدم)
    {
	vim_free(tbuf);
	החזר;
    }

    newsize = (صحيح)(strlen(cstag_msg) + strlen(ptag));
    buf = alloc(newsize);
    אם (buf != عدم)
    {
	bufsize = newsize;
	(فراغ)sprintf(buf, cstag_msg, ptag);
	msg_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    msg_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    msg_advance(msg_col + 2);
    msg_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    num = 1;
    עבור (i = 0; i < num_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	אם ((tbuf = alloc(strlen(matches[idx]) + 1)) == عدم)
	    استمر;
	(فراغ)strcpy(tbuf, matches[idx]);

	אם (strtok(tbuf, (ثابت حرف *)"\t") == عدم
		|| (fname = strtok(عدم, (ثابت حرف *)"\t")) == عدم
		|| (lno = strtok(عدم, (ثابت حرف *)"\t")) == عدم)
	{
	    vim_free(tbuf);
	    استمر;
	}
	extra = strtok(عدم, (ثابت حرف *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (صحيح)(strlen(csfmt_str) + 16 + strlen(lno));
	אם (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    אם (buf == عدم)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    אחרת
		bufsize = newsize;
	}
	אם (buf != عدم)
	{
	    // csfmt_str = "%4d %6s  ";
	    (فراغ)sprintf(buf, csfmt_str, num, lno);
	    msg_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	msg_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	אם (cntxts[idx] != عدم)
	    context = cntxts[idx];
	אחרת
	    context = globalcntx;
	newsize = (صحيح)(strlen(context) + strlen(cntxformat));

	אם (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    אם (buf == عدم)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    אחרת
		bufsize = newsize;
	}
	אם (buf != عدم)
	{
	    (فراغ)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    אם (msg_col + (صحيح)strlen(buf) >= cmdline_width)
		msg_putchar('\n');
	    msg_advance(12);
	    msg_outtrans_long_attr((char_u *)buf, 0);
	    msg_putchar('\n');
	}
	אם (extra != عدم)
	{
	    msg_advance(13);
	    msg_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	אם (msg_col)
	    msg_putchar('\n');

	ui_breakcheck();
	אם (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    اكسر;
	}

	num++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    ساكن صحيح
cs_read_prompt(صحيح i)
{
    صحيح		ch;
    حرف	*buf = عدم; // buffer for possible error message from cscope
    صحيح		bufpos = 0;
    صحيح		maxlen;
    ساكن حرف *eprompt = "Press the RETURN key to continue:";
    صحيح		epromptlen = (صحيح)strlen(eprompt);
    صحيح		n;

    // compute maximum allowed len for Cscope error message
    حرف *cs_emsg = _(e_cscope_error_str);
    maxlen = (صحيح)(IOSIZE - strlen(cs_emsg));

    עבור (;;)
    {
	כלעוד ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    אם (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		אם (buf == عدم) // lazy buffer allocation
		    buf = alloc(maxlen);
		אם (buf != عدم)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    אם (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(فراغ)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(فراغ)putc('\n', csinfo[i].to_fp);
			(فراغ)fflush(csinfo[i].to_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	עבור (n = 0; n < (صحيح)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    אם (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    אם (ch == EOF)
	    {
		אם (buf != عدم && buf[0] != NUL)
		    (فراغ)semsg(cs_emsg, buf);
		אחרת אם (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		החזר CSCOPE_FAILURE;
	    }

	    אם (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		اكسر;
	    }
	}

	אם (ch == EOF)
	    استمر;	    // didn't find the prompt
	اكسر;		    // did find the prompt
    }

    vim_free(buf);
    החזר CSCOPE_SUCCESS;
}

# אם defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    ساكن فراغ
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    ساكن فراغ
cs_release_csp(صحيح i, صحيح freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    אם (csinfo[i].to_fp != عدم)
    {
	(فراغ)fputs("q\n", csinfo[i].to_fp);
	(فراغ)fflush(csinfo[i].to_fp);
    }
# אם defined(UNIX)
    {
	صحيح waitpid_errno;
	صحيح pstat;
	pid_t pid;

#  אם defined(HAVE_SIGACTION)
	بنية sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   אחרת
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, عدم);
#  אחרת
	صحيح waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	עבור (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    אם (pid != 0)
		اكسر;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	אם (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    صحيح alive = TRUE;

	    אם (waitpid_errno == ECHILD)
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
		صحيح waited;

		sleep(0);
		עבור (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    אם (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			اكسر;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    אם (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(فراغ)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# אחרת  // !UNIX
    אם (csinfo[i].hProc != عدم)
    {
	// Give cscope a chance to exit normally
	אם (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    אם (csinfo[i].fr_fp != عدم)
	(فراغ)fclose(csinfo[i].fr_fp);
    אם (csinfo[i].to_fp != عدم)
	(فراغ)fclose(csinfo[i].to_fp);

    אם (freefnpp)
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
    ساكن صحيح
cs_reset(exarg_T *eap UNUSED)
{
    حرف	**dblist = عدم, **pplist = عدم, **fllist = عدم;
    صحيح	i;
    حرف buf[20]; // for sprintf " (#%d)"

    אם (csinfo_size == 0)
	החזר CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(حرف *, csinfo_size);
    pplist = ALLOC_MULT(حرف *, csinfo_size);
    fllist = ALLOC_MULT(حرف *, csinfo_size);
    אם (dblist == عدم || pplist == عدم || fllist == عدم)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	החזר CSCOPE_FAILURE;
    }

    עבור (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	אם (csinfo[i].fname != عدم)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    עבור (i = 0; i < csinfo_size; i++)
    {
	אם (dblist[i] != عدم)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    אם (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		msg_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    אם (p_csverbose)
	msg_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | MSG_HIST);
    החזר CSCOPE_SUCCESS;
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
    ساكن حرف *
cs_resolve_file(صحيح i, حرف *name)
{
    حرف	*fullname;
    string_T	csdir = {عدم, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    صحيح		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (صحيح)namelen + 2;
    אם (csinfo[i].ppath != عدم)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (صحيح)ppathlen;
    }
    אחרת אם (p_csre && csinfo[i].fname != عدم)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	אם (csdir.string != عدم)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (صحيح)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    אם (csinfo[i].ppath != عدم
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	אם ((fullname = alloc(len)) != عدم)
	    (فراغ)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    אחרת אם (csdir.string != عدم && csinfo[i].fname != عدم && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (حرف *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    אחרת
    {
	fullname = (حرف *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    החזר fullname;
}


/*
 * Show all cscope connections.
 */
    ساكن صحيح
cs_show(exarg_T *eap UNUSED)
{
    صحيح i;

    אם (cs_cnt_connections() == 0)
	msg_puts(_("no cscope connections\n"));
    אחרת
    {
	msg_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	עבור (i = 0; i < csinfo_size; i++)
	{
	    אם (csinfo[i].fname == عدم)
		استمر;

	    אם (csinfo[i].ppath != عدم)
		(فراغ)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    אחרת
		(فراغ)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    החזר CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    فراغ
cs_end(فراغ)
{
    صحيح i;

    עבור (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#אם defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    فراغ
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    صحيح		num = 0;
    char_u	*dbpath = عدم;
    char_u	*prepend = عدم;
    char_u	buf[NUMBUFLEN];

    אם (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	החזר;

    אם (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	num = (صحيح)tv_get_number(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	אם (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_number = cs_connection(num, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
