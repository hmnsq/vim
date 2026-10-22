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
#  ifndef WIN32_LEAN_AND_اوسط
#   define WIN32_LEAN_AND_اوسط
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

قسمتعریف ساخت {
    حرف *  name;
    صحیح     (*func)(exarg_T *eap);
    حرف *  help;
    حرف *  usage;
    صحیح	    cansplit;		// if supports splitting window
} cscmd_T;

قسمتعریف ساخت csi {
    حرف *	    fname;	// cscope db name
    حرف *	    ppath;	// path to prepend (the -P option)
    حرف *	    flags;	// additional cscope flags/options (e.g, -p2)
# اگر defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# ورنہ
#  اگر defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_اشاریہ_بلند;	// st_ino has no meaning in the Windows
    DWORD	    n_اشاریہ_کم;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    مقصد_fp;	// to cscope: FILE.
} csinfo_T;

قسمتعریف شمار { جوڑو_2, ڈھونڈو_2, Help, Kill, Reset, Show } csid_e;

قسمتعریف شمار {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

ساکن صحیح	    cs_جوڑو(exarg_T *eap);
ساکن صحیح	    cs_جوڑو_common(حرف *, حرف *, حرف *);
ساکن صحیح	    cs_check_for_connections(خالی);
ساکن صحیح	    cs_check_for_لیبل_فہرست(خالی);
ساکن صحیح	    cs_cnt_connections(خالی);
ساکن صحیح	    cs_create_connection(صحیح i);
# ifdef FEAT_QUICKFIX
ساکن خالی	    cs_file_results(FILE *, صحیح *);
# endif
ساکن خالی	    cs_fill_results(حرف *, صحیح , صحیح *, حرف ***,
			حرف ***, صحیح *);
ساکن صحیح	    cs_ڈھونڈو(exarg_T *eap);
ساکن صحیح	    cs_ڈھونڈو_common(حرف *opt, حرف *pat, صحیح, صحیح, صحیح, char_u *cmdline);
ساکن صحیح	    cs_help(exarg_T *eap);
ساکن صحیح	    cs_داخل_کرو_filelist(حرف *, حرف *, حرف *,
			stat_T *);
ساکن صحیح	    cs_kill(exarg_T *eap);
ساکن خالی	    cs_kill_execute(صحیح, حرف *);
ساکن cscmd_T *    cs_lookup_cmd(exarg_T *eap);
ساکن حرف *	    cs_make_vim_style_matches(حرف *, حرف *,
			حرف *, حرف *);
ساکن حرف *	    cs_manage_matches(حرف **, حرف **, صحیح, mcmd_e);
ساکن خالی	    cs_print_لیبل_فہرست_priv(حرف **, حرف **, صحیح);
ساکن صحیح	    cs_read_prompt(صحیح);
ساکن خالی	    cs_release_csp(صحیح, صحیح freefnpp);
ساکن صحیح	    cs_reset(exarg_T *eap);
ساکن حرف *	    cs_resolve_file(صحیح, حرف *);
ساکن صحیح	    cs_show(exarg_T *eap);


ساکن csinfo_T *   csinfo = تہی;
ساکن صحیح	    csinfo_سائز = 0;	// number of items allocated in
					// csinfo[]

ساکن صحیح	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
ساکن cscmd_T	    cs_cmds[] =
{
    { "add",	cs_جوڑو,
		N_("Add a new database"),     "add file|dir [pre-path] [flags]", 0 },
    { "find",	cs_ڈھونڈو,
		N_("Query for a pattern"),    "find a|c|d|e|f|g|i|s|t name", 1 },
    { "help",	cs_help,
		N_("Show this message"),      "help", 0 },
    { "kill",	cs_kill,
		N_("Kill a connection"),      "kill #", 0 },
    { "reset",	cs_reset,
		N_("Reinit all connections"), "reset", 0 },
    { "show",	cs_show,
		N_("Show connections"),       "show", 0 },
    { تہی, تہی, تہی, تہی, 0 }
};

    ساکن خالی
cs_usage_پیغام(csid_e x)
{
    (خالی)semsg(_(e_usage_cscope_str), cs_cmds[(صحیح)x].usage);
}

ساکن شمار
{
    EXP_CSCOPE_SUBCMD,	// expand ":cscope" sub-commands
    EXP_SCSCOPE_SUBCMD,	// expand ":scscope" sub-commands
    EXP_CSCOPE_ڈھونڈو,	// expand ":cscope find" arguments
    EXP_CSCOPE_KILL	// expand ":cscope kill" arguments
} expand_what;

/*
 * Function given to ExpandGeneric() to obtain the cscope command
 * expansion.
 */
    char_u *
get_cscope_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    صحیح		current_idx;
    صحیح		i;

    انتخاب (expand_what)
    {
    حالت EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	واپس (char_u *)cs_cmds[idx].name;
    حالت EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	برائے (i = 0, current_idx = 0; cs_cmds[i].name != تہی; i++)
	    اگر (cs_cmds[i].cansplit)
		اگر (current_idx++ == idx)
		    توڑو;
	واپس (char_u *)cs_cmds[i].name;
    حالت EXP_CSCOPE_ڈھونڈو:
	{
	    ثابت حرف *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", تہی
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    واپس (char_u *)query_type[idx];
	}
    حالت EXP_CSCOPE_KILL:
	{
	    ساکن حرف	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    برائے (i = 0, current_idx = 0; i < csinfo_سائز; i++)
	    {
		اگر (csinfo[i].fname == تہی)
		    جاری;
		اگر (current_idx++ == idx)
		{
		    vim_snprintf(connection, حجم(connection), "%d", i);
		    واپس (char_u *)connection;
		}
	    }
	    واپس (current_idx == idx && idx > 0) ? (char_u *)"-1" : تہی;
	}
    طےشدہ:
	واپس تہی;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    خالی
سیٹ_context_اندر_cscope_cmd(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_CSCOPE;
    xp->xp_نمونہ = arg;
    expand_what = (cmdidx == CMD_scscope)
			? EXP_SCSCOPE_SUBCMD : EXP_CSCOPE_SUBCMD;

    اگر (*arg == NUL)
	واپس;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    اگر (*p == NUL)
	واپس;

    // past first word
    xp->xp_نمونہ = skipwhite(p);
    اگر (*skiptowhite(xp->xp_نمونہ) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    ورنہ اگر (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    ورنہ اگر (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    ورنہ اگر (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_ڈھونڈو;
    ورنہ
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    ساکن خالی
do_cscope_general(
    exarg_T	*eap,
    صحیح		make_بانٹو غیر_مستعمل) // whether to split window
{
    cscmd_T *cmdp;

    اگر (check_restricted())
	واپس;

    اگر ((cmdp = cs_lookup_cmd(eap)) == تہی)
    {
	cs_help(eap);
	واپس;
    }

    اگر (make_بانٹو)
    {
	اگر (!cmdp->cansplit)
	{
	    (خالی)پیغام_puts(_("This cscope command does not support splitting the window.\n"));
	    واپس;
	}
	postponed_بانٹو = -1;
	postponed_بانٹو_flags = cmdmod.cmod_بانٹو;
	postponed_بانٹو_tab = cmdmod.cmod_tab;
    }

    cmdp->func(eap);

    postponed_بانٹو_flags = 0;
    postponed_بانٹو_tab = 0;
}

/*
 * Implementation of ":cscope" and ":lcscope"
 */
    خالی
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    خالی
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    خالی
ex_cstag(exarg_T *eap)
{
    صحیح ret = FALSE;

    اگر (*eap->arg == NUL)
    {
	(خالی)emsg(_(e_usage_cstag_ident));
	واپس;
    }

    انتخاب (p_csto)
    {
    حالت 0 :
	اگر (cs_check_for_connections())
	{
	    ret = cs_ڈھونڈو_common("g", (حرف *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    اگر (ret == FALSE)
	    {
		cs_free_لیبل_فہرست();
		اگر (پیغام_ستون)
		    پیغام_putchar('\n');

		اگر (cs_check_for_لیبل_فہرست())
		    ret = do_لیبل(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	ورنہ اگر (cs_check_for_لیبل_فہرست())
	{
	    ret = do_لیبل(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	توڑو;
    حالت 1 :
	اگر (cs_check_for_لیبل_فہرست())
	{
	    ret = do_لیبل(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    اگر (ret == FALSE)
	    {
		اگر (پیغام_ستون)
		    پیغام_putchar('\n');

		اگر (cs_check_for_connections())
		{
		    ret = cs_ڈھونڈو_common("g", (حرف *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    اگر (ret == FALSE)
			cs_free_لیبل_فہرست();
		}
	    }
	}
	ورنہ اگر (cs_check_for_connections())
	{
	    ret = cs_ڈھونڈو_common("g", (حرف *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    اگر (ret == FALSE)
		cs_free_لیبل_فہرست();
	}
	توڑو;
    طےشدہ :
	توڑو;
    }

    اگر (!ret)
    {
	(خالی)emsg(_(e_cstag_لیبل_not_founc));
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
cs_fgets(char_u *buf, صحیح سائز)
{
    حرف *p;

    اگر ((p = cs_manage_matches(تہی, تہی, -1, Get)) == تہی)
	واپس TRUE;
    vim_strncpy(buf, (char_u *)p, سائز - 1);

    واپس FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    خالی
cs_free_لیبل_فہرست(خالی)
{
    cs_manage_matches(تہی, تہی, -1, Free);
}


/*
 * Called from do_tag().
 */
    خالی
cs_print_لیبل_فہرست(خالی)
{
    cs_manage_matches(تہی, تہی, -1, Print);
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
    ساکن صحیح
cs_connection(صحیح عدد, char_u *dbpath, char_u *ppath)
{
    صحیح i;

    اگر (عدد < 0 || عدد > 4 || (عدد > 0 && !dbpath))
	واپس FALSE;

    برائے (i = 0; i < csinfo_سائز; i++)
    {
	اگر (!csinfo[i].fname)
	    جاری;

	اگر (عدد == 0)
	    واپس TRUE;

	انتخاب (عدد)
	{
	حالت 1:
	    اگر (لڑی_تلاش(csinfo[i].fname, (حرف *)dbpath))
		واپس TRUE;
	    توڑو;
	حالت 2:
	    اگر (strcmp(csinfo[i].fname, (حرف *)dbpath) == 0)
		واپس TRUE;
	    توڑو;
	حالت 3:
	    اگر (لڑی_تلاش(csinfo[i].fname, (حرف *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && لڑی_تلاش(csinfo[i].ppath, (حرف *)ppath))))
		واپس TRUE;
	    توڑو;
	حالت 4:
	    اگر ((strcmp(csinfo[i].fname, (حرف *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (حرف *)ppath) == 0))))
		واپس TRUE;
	    توڑو;
	}
    }

    واپس FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    ساکن صحیح
cs_جوڑو(exarg_T *eap غیر_مستعمل)
{
    حرف *fname, *ppath, *flags = تہی;

    اگر ((fname = strtok((حرف *)تہی, (ثابت حرف *)" ")) == تہی)
    {
	cs_usage_پیغام(جوڑو_2);
	واپس CSCOPE_FAILURE;
    }
    اگر ((ppath = strtok((حرف *)تہی, (ثابت حرف *)" ")) != تہی)
	flags = strtok((حرف *)تہی, (ثابت حرف *)" ");

    واپس cs_جوڑو_common(fname, ppath, flags);
}

    ساکن خالی
cs_stat_emsg(حرف *fname)
{
    صحیح err = errno;
    (خالی)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    ساکن صحیح
cs_جوڑو_common(
    حرف *arg1,	    // filename - may contain environment variables
    حرف *arg2,	    // prepend path - may contain environment variables
    حرف *flags)
{
    stat_T	statbuf;
    صحیح		ret;
    حرف	*fname = تہی;
    حرف	*fname2 = تہی;
    حرف	*ppath = تہی;
    صحیح		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = تہی;

    // get the filename (arg1), expand it, and try to stat it
    اگر ((fname = alloc(MAXPATHL + 1)) == تہی)
	جاؤ جوڑو_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (خالی)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    اگر (fname == تہی)
	جاؤ جوڑو_err;
    fname = (حرف *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    اگر (ret < 0)
    {
staterr:
	اگر (p_csverbose)
	    cs_stat_emsg(fname);
	جاؤ جوڑو_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    اگر (arg2 != تہی)
    {
	stat_T	    statbuf2;

	اگر ((ppath = alloc(MAXPATHL + 1)) == تہی)
	    جاؤ جوڑو_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	اگر (ret < 0)
	    جاؤ staterr;
    }

    // if filename is a directory, append the cscope database name to it
    اگر (S_ISDIR(statbuf.st_منوال))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + لڑی_لمبائی(fname) + 2);
	اگر (fname2 == تہی)
	    جاؤ جوڑو_err;

	جب (fname[لڑی_لمبائی(fname)-1] == '/'
# ifdef MSWIN
		|| fname[لڑی_لمبائی(fname)-1] == '\\'
# endif
		)
	{
	    fname[لڑی_لمبائی(fname)-1] = '\0';
	    اگر (fname[0] == '\0')
		توڑو;
	}
	اگر (fname[0] == '\0')
	    (خالی)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	ورنہ
	    (خالی)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	اگر (ret < 0)
	{
	    اگر (p_csverbose)
		cs_stat_emsg(fname2);
	    جاؤ جوڑو_err;
	}

	i = cs_داخل_کرو_filelist(fname2, ppath, flags, &statbuf);
    }
    ورنہ اگر (S_ISREG(statbuf.st_منوال) || S_ISLNK(statbuf.st_منوال))
    {
	i = cs_داخل_کرو_filelist(fname, ppath, flags, &statbuf);
    }
    ورنہ
    {
	اگر (p_csverbose)
	    (خالی)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	جاؤ جوڑو_err;
    }

    اگر (i != -1)
    {
	اگر (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    جاؤ جوڑو_err;
	}

	اگر (p_csverbose)
	{
	    پیغام_clr_eos();
	    (خالی)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    واپس CSCOPE_SUCCESS;

جوڑو_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    واپس CSCOPE_FAILURE;
}


    ساکن صحیح
cs_check_for_connections(خالی)
{
    واپس (cs_cnt_connections() > 0);
}


    ساکن صحیح
cs_check_for_لیبل_فہرست(خالی)
{
    واپس (p_لیبل_فہرست[0] != NUL && curbuf->b_p_لیبل_فہرست != تہی);
}


/*
 * Count the number of cscope connections.
 */
    ساکن صحیح
cs_cnt_connections(خالی)
{
    صحیح i;
    صحیح cnt = 0;

    برائے (i = 0; i < csinfo_سائز; i++)
	اگر (csinfo[i].fname != تہی)
	    cnt++;
    واپس cnt;
}

    ساکن خالی
cs_reading_emsg(
    صحیح idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    ساکن صحیح
cs_cnt_matches(صحیح idx)
{
    حرف *stok;
    حرف *buf;
    صحیح nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    اگر (buf == تہی)
	واپس 0;
    برائے (;;)
    {
	اگر (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    اگر (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    واپس -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	اگر (لڑی_تلاش((ثابت حرف *)buf, "Unable to search database") != تہی)
	    توڑو;
	اگر ((stok = strtok(buf, (ثابت حرف *)" ")) == تہی)
	    جاری;
	اگر (لڑی_تلاش((ثابت حرف *)stok, "cscope:") == تہی)
	    جاری;

	اگر ((stok = strtok(تہی, (ثابت حرف *)" ")) == تہی)
	    جاری;
	nlines = atoi(stok);
	اگر (nlines < 0)
	{
	    nlines = 0;
	    توڑو;
	}

	اگر ((stok = strtok(تہی, (ثابت حرف *)" ")) == تہی)
	    جاری;
	اگر (لڑی_موازنہ((ثابت حرف *)stok, "lines", 5))
	    جاری;

	توڑو;
    }

    vim_free(buf);
    واپس nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    ساکن حرف *
cs_create_cmd(حرف *csoption, حرف *نمونہ)
{
    حرف *cmd;
    مختصر تلاش;
    حرف *pat;

    انتخاب (csoption[0])
    {
    حالت '0' : حالت 's' :
	تلاش = 0;
	توڑو;
    حالت '1' : حالت 'g' :
	تلاش = 1;
	توڑو;
    حالت '2' : حالت 'd' :
	تلاش = 2;
	توڑو;
    حالت '3' : حالت 'c' :
	تلاش = 3;
	توڑو;
    حالت '4' : حالت 't' :
	تلاش = 4;
	توڑو;
    حالت '6' : حالت 'e' :
	تلاش = 6;
	توڑو;
    حالت '7' : حالت 'f' :
	تلاش = 7;
	توڑو;
    حالت '8' : حالت 'i' :
	تلاش = 8;
	توڑو;
    حالت '9' : حالت 'a' :
	تلاش = 9;
	توڑو;
    طےشدہ :
	(خالی)emsg(_(e_unknown_cscope_تلاش_type));
	cs_usage_پیغام(ڈھونڈو_2);
	واپس تہی;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = نمونہ;
    اگر (تلاش != 4 && تلاش != 6)
	جب VIM_ISWHITE(*pat)
	    ++pat;

    اگر ((cmd = alloc(لڑی_لمبائی(pat) + 2)) == تہی)
	واپس تہی;

    (خالی)sprintf(cmd, "%d%s", تلاش, pat);

    واپس cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    ساکن صحیح
cs_create_connection(صحیح i)
{
# ifdef UNIX
    صحیح		مقصد_cs[2], from_cs[2];
# endif
    صحیح		cmdlen;
    حرف	*prog, *cmd, *ppath = تہی;
    size_t	proglen;
# ifdef MSWIN
    صحیح		fd;
    صحیح		len;
    SECURITY_اوصاف sa;
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
    مقصد_cs[0] = مقصد_cs[1] = from_cs[0] = from_cs[1] = -1;
    اگر (pipe(مقصد_cs) < 0 || pipe(from_cs) < 0)
    {
	(خالی)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	اگر (مقصد_cs[0] != -1)
	    (خالی)close(مقصد_cs[0]);
	اگر (مقصد_cs[1] != -1)
	    (خالی)close(مقصد_cs[1]);
	اگر (from_cs[0] != -1)
	    (خالی)close(from_cs[0]);
	اگر (from_cs[1] != -1)
	    (خالی)close(from_cs[1]);
	واپس CSCOPE_FAILURE;
    }

    اگر ((csinfo[i].pid = fork()) == -1)
    {
	(خالی)emsg(_(e_could_not_fork_for_cscope));
	جاؤ err_closing;
    }
    ورنہ اگر (csinfo[i].pid == 0)	// child: run cscope.
    {
	حرف **argv = تہی;

	اگر (dup2(مقصد_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	اگر (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	اگر (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(خالی)close(مقصد_cs[1]);
	(خالی)close(from_cs[0]);
# ورنہ
	// MSWIN
	// Create pipes to communicate with cscope
	sa.n_لمبائی = حجم(SECURITY_اوصاف);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = تہی;

	اگر (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (خالی)emsg(_(e_could_not_create_cscope_pipes));
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
	    واپس CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	اگر ((prog = alloc(MAXPATHL + 1)) == تہی)
	{
# ifdef UNIX
	    باہر_نکلو(EXIT_FAILURE);
# ورنہ
	    // MSWIN
	    جاؤ err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (صحیح)(proglen + لڑی_لمبائی(csinfo[i].fname) + 32);
	اگر (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    اگر ((ppath = alloc(MAXPATHL + 1)) == تہی)
	    {
		vim_free(prog);
# ifdef UNIX
		باہر_نکلو(EXIT_FAILURE);
# ورنہ
		// MSWIN
		جاؤ err_closing;
# endif
	    }
	    cmdlen += (صحیح)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	اگر (csinfo[i].flags)
	    cmdlen += (صحیح)لڑی_لمبائی(csinfo[i].flags);

	اگر ((cmd = alloc(cmdlen)) == تہی)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    باہر_نکلو(EXIT_FAILURE);
# ورنہ
	    // MSWIN
	    جاؤ err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    حرف	**tok = تہی;
	    صحیح		tokc = 0;

	    ga_init2(&ga_argv, حجم(حرف *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    اگر (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		باہر_نکلو(EXIT_FAILURE);
	    برائے (صحیح k = 0; k < tokc; ++k)
	    {
		اگر (ga_grow(&ga_argv, 1) == FAIL)
		    باہر_نکلو(EXIT_FAILURE);
		((حرف **)ga_argv.ga_ڈیٹا)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    اگر (ga_grow(&ga_argv, 3) == FAIL)
		باہر_نکلو(EXIT_FAILURE);
	    ((حرف **)ga_argv.ga_ڈیٹا)[ga_argv.ga_len++] =
				    (حرف *)vim_strsave((char_u *)"-dl");
	    ((حرف **)ga_argv.ga_ڈیٹا)[ga_argv.ga_len++] =
				    (حرف *)vim_strsave((char_u *)"-f");
	    ((حرف **)ga_argv.ga_ڈیٹا)[ga_argv.ga_len++] =
				(حرف *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    اگر (ppath != تہی)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		حرف	*parg = alloc(plen);

		اگر (parg == تہی || ga_grow(&ga_argv, 1) == FAIL)
		    باہر_نکلو(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((حرف **)ga_argv.ga_ڈیٹا)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    اگر (csinfo[i].flags != تہی)
	    {
		اگر (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    باہر_نکلو(EXIT_FAILURE);
		برائے (صحیح k = 0; k < tokc; ++k)
		{
		    اگر (ga_grow(&ga_argv, 1) == FAIL)
			باہر_نکلو(EXIT_FAILURE);
		    ((حرف **)ga_argv.ga_ڈیٹا)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    اگر (ga_grow(&ga_argv, 1) == FAIL)
		باہر_نکلو(EXIT_FAILURE);
	    ((حرف **)ga_argv.ga_ڈیٹا)[ga_argv.ga_len] = تہی;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  اگر defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   اگر defined(HAVE_SETSID)
	    (خالی)setsid();
#   ورنہ
	    اگر (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (حرف **)ga_argv.ga_ڈیٹا;
	    اگر (argv[0] == تہی || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    باہر_نکلو(127);
	    // NOTREACHED
	}
    }
    ورنہ	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	اگر ((csinfo[i].مقصد_fp = fdopen(مقصد_cs[1], "w")) == تہی)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	اگر ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == تہی)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(خالی)close(مقصد_cs[0]);
	(خالی)close(from_cs[1]);
    }
# ورنہ
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    اگر (csinfo[i].ppath != تہی)
    {
	len = (صحیح)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    اگر (csinfo[i].flags != تہی)
    {
	len = (صحیح)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.w_Show_کھڑکی = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_داخلہ  = stdin_rd;
    created = CreateProcess(تہی, cmd, تہی, تہی, TRUE, CREATE_NEW_CONSOLE,
							تہی, تہی, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    اگر (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(خالی)emsg(_(e_could_not_spawn_cscope_process));
	جاؤ err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    اگر (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      O_متن|_O_APPEND)) < 0)
	    || ((csinfo[i].مقصد_fp = _fdopen(fd, "w")) == تہی))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    اگر (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      O_متن|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == تہی))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    واپس CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    ساکن صحیح
cs_ڈھونڈو(exarg_T *eap)
{
    حرف *opt, *pat;
    صحیح i;

    اگر (cs_check_for_connections() == FALSE)
    {
	(خالی)emsg(_(e_no_cscope_connections));
	واپس FALSE;
    }

    اگر ((opt = strtok((حرف *)تہی, (ثابت حرف *)" ")) == تہی)
    {
	cs_usage_پیغام(ڈھونڈو_2);
	واپس FALSE;
    }

    pat = opt + لڑی_لمبائی(opt) + 1;
    اگر (pat >= (حرف *)eap->arg + eap_arg_len)
    {
	cs_usage_پیغام(ڈھونڈو_2);
	واپس FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    برائے (i = 0; i < eap_arg_len; ++i)
	اگر (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    واپس cs_ڈھونڈو_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    ساکن صحیح
cs_ڈھونڈو_common(
    حرف *opt,
    حرف *pat,
    صحیح forceit,
    صحیح verbose,
    صحیح	use_ll غیر_مستعمل,
    char_u *cmdline غیر_مستعمل)
{
    صحیح i;
    حرف *cmd;
    صحیح *nummatches;
    صحیح totmatches;
# ifdef FEAT_QUICKFIX
    حرف cmdletter;
    حرف *qfpos;

    // get cmd letter
    انتخاب (opt[0])
    {
    حالت '0' :
	cmdletter = 's';
	توڑو;
    حالت '1' :
	cmdletter = 'g';
	توڑو;
    حالت '2' :
	cmdletter = 'd';
	توڑو;
    حالت '3' :
	cmdletter = 'c';
	توڑو;
    حالت '4' :
	cmdletter = 't';
	توڑو;
    حالت '6' :
	cmdletter = 'e';
	توڑو;
    حالت '7' :
	cmdletter = 'f';
	توڑو;
    حالت '8' :
	cmdletter = 'i';
	توڑو;
    حالت '9' :
	cmdletter = 'a';
	توڑو;
    طےشدہ :
	cmdletter = opt[0];
    }

    qfpos = (حرف *)vim_strchr(p_csqf, cmdletter);
    اگر (qfpos != تہی)
    {
	qfpos++;
	// next symbol must be + or -
	اگر (strchr(CSQF_FLAGS, *qfpos) == تہی)
	{
	    (خالی)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    واپس FALSE;
	}

	اگر (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    اگر (aborting())
		واپس FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    اگر (cmd == تہی)
	واپس FALSE;

    nummatches = ALLOC_MULT(صحیح, csinfo_سائز);
    اگر (nummatches == تہی)
    {
	vim_free(cmd);
	واپس FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    برائے (i = 0; i < csinfo_سائز; i++)
	nummatches[i] = 0;
    totmatches = 0;
    برائے (i = 0; i < csinfo_سائز; i++)
    {
	اگر (csinfo[i].fname == تہی || csinfo[i].مقصد_fp == تہی)
	    جاری;

	// send cmd to cscope
	(خالی)fprintf(csinfo[i].مقصد_fp, "%s\n", cmd);
	(خالی)fflush(csinfo[i].مقصد_fp);

	nummatches[i] = cs_cnt_matches(i);

	اگر (nummatches[i] > -1)
	    totmatches += nummatches[i];

	اگر (nummatches[i] == 0)
	    (خالی)cs_read_prompt(i);
    }
    vim_free(cmd);

    اگر (totmatches == 0)
    {
	اگر (verbose)
	    (خالی)semsg(_(e_no_matches_ملا_for_cscope_query_str_کا_str),
								     opt, pat);
	vim_free(nummatches);
	واپس FALSE;
    }

# ifdef FEAT_QUICKFIX
    اگر (qfpos != تہی && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = تہی;
	win_T	    *wp = تہی;

	اگر (tmp == تہی)
	{
	    vim_free(nummatches);
	    واپس FALSE;
	}

	f = mch_fopen((حرف *)tmp, "w");
	اگر (f == تہی)
	    semsg(_(e_cant_open_file_str), tmp);
	ورنہ
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    اگر (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    اگر (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, تہی) > 0)
	    {
		اگر (postponed_بانٹو != 0)
		{
		    (خالی)win_بانٹو(postponed_بانٹو > 0 ? postponed_بانٹو : 0,
						       postponed_بانٹو_flags);
		    RESET_BINDING(curwin);
		    postponed_بانٹو = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		اگر (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_بفر) && wp->w_llist_ref != تہی)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	واپس TRUE;
    }
    ورنہ
# endif // FEAT_QUICKFIX
    {
	حرف **matches = تہی, **contexts = تہی;
	صحیح matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	اگر (matches == تہی)
	    واپس FALSE;

	(خالی)cs_manage_matches(matches, contexts, matched, Store);

	واپس do_لیبل((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    ساکن صحیح
cs_help(exarg_T *eap غیر_مستعمل)
{
    cscmd_T *cmdp = cs_cmds;

    (خالی)پیغام_puts(_("cscope commands:\n"));
    جب (cmdp->name != تہی)
    {
	حرف *help = _(cmdp->help);
	صحیح  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	اگر (space_cnt < 0)
	    space_cnt = 0;
	(خالی)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	اگر (strcmp(cmdp->name, "find") == 0)
	    پیغام_puts(_("\n"
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
    واپس 0;
}


    ساکن خالی
clear_csinfo(صحیح i)
{
    csinfo[i].fname  = تہی;
    csinfo[i].ppath  = تہی;
    csinfo[i].flags  = تہی;
# اگر defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# ورنہ
    csinfo[i].nVolume = 0;
    csinfo[i].n_اشاریہ_بلند = 0;
    csinfo[i].n_اشاریہ_کم = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = تہی;
    csinfo[i].مقصد_fp  = تہی;
# اگر defined(MSWIN)
    csinfo[i].hProc = تہی;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    ساکن صحیح
cs_داخل_کرو_filelist(
    حرف *fname,
    حرف *ppath,
    حرف *flags,
    stat_T *sb غیر_مستعمل)
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
		حرف *cant_پیغام = _(e_cannot_open_cscope_database_str);
		حرف *winmsg = GetWin32Error();

		اگر (winmsg != تہی)
		    (خالی)semsg(cant_پیغام, winmsg);
		ورنہ
		    // subst filename if can't get error text
		    (خالی)semsg(cant_پیغام, fname);
	    }
	    واپس -1;

	حالت FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    اگر (p_csverbose)
		(خالی)emsg(_(e_cannot_get_cscope_database_information));
	    واپس -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    برائے (j = 0; j < csinfo_سائز; j++)
    {
	اگر (csinfo[j].fname != تہی
# اگر defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# ورنہ
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_نمبر
		    && csinfo[j].n_اشاریہ_بلند == bhfi.n_File_اشاریہ_بلند
		    && csinfo[j].n_اشاریہ_کم == bhfi.n_File_اشاریہ_کم))
# endif
	    )
	{
	    اگر (p_csverbose)
		(خالی)emsg(_(e_duplicate_cscope_database_not_added));
	    واپس -1;
	}

	اگر (csinfo[j].fname == تہی && i == -1)
	    i = j; // remember first empty entry
    }

    اگر (i == -1)
    {
	i = csinfo_سائز;
	اگر (csinfo_سائز == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_سائز = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	ورنہ
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_سائز *= 2;
	    csinfo = vim_realloc(csinfo, حجم(csinfo_T)*csinfo_سائز);
	    اگر (csinfo == تہی)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_سائز = i;
		واپس -1;
	    }
	}
	اگر (csinfo == تہی)
	    واپس -1;
	برائے (j = csinfo_سائز/2; j < csinfo_سائز; j++)
	    clear_csinfo(j);
    }

    اگر ((csinfo[i].fname = alloc(لڑی_لمبائی(fname)+1)) == تہی)
	واپس -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, لڑی_لمبائی((ثابت حرف *)fname));

    اگر (ppath != تہی)
    {
	اگر ((csinfo[i].ppath = alloc(لڑی_لمبائی(ppath) + 1)) == تہی)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    واپس -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, لڑی_لمبائی((ثابت حرف *)ppath));
    }
    ورنہ
	csinfo[i].ppath = تہی;

    اگر (flags != تہی)
    {
	اگر ((csinfo[i].flags = alloc(لڑی_لمبائی(flags) + 1)) == تہی)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    واپس -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, لڑی_لمبائی((ثابت حرف *)flags));
    }
    ورنہ
	csinfo[i].flags = تہی;

# اگر defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# ورنہ
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_نمبر;
    csinfo[i].n_اشاریہ_کم = bhfi.n_File_اشاریہ_کم;
    csinfo[i].n_اشاریہ_بلند = bhfi.n_File_اشاریہ_بلند;
# endif
    واپس i;
}


/*
 * Find cscope command in command table.
 */
    ساکن cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    حرف *stok;
    size_t len;

    اگر (eap->arg == تہی)
	واپس تہی;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (صحیح)STRLEN(eap->arg);

    اگر ((stok = strtok((حرف *)(eap->arg), (ثابت حرف *)" ")) == تہی)
	واپس تہی;

    len = لڑی_لمبائی(stok);
    برائے (cmdp = cs_cmds; cmdp->name != تہی; ++cmdp)
    {
	اگر (لڑی_موازنہ((ثابت حرف *)(stok), cmdp->name, len) == 0)
	    واپس (cmdp);
    }
    واپس تہی;
}


/*
 * Nuke em.
 */
    ساکن صحیح
cs_kill(exarg_T *eap غیر_مستعمل)
{
    حرف *stok;
    صحیح i;

    اگر ((stok = strtok((حرف *)تہی, (ثابت حرف *)" ")) == تہی)
    {
	cs_usage_پیغام(Kill);
	واپس CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    اگر ((لڑی_لمبائی(stok) < 2 && VIM_ISDIGIT((صحیح)(stok[0])))
	    || (لڑی_لمبائی(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((صحیح)(stok[1]))))
	i = atoi(stok);
    ورنہ
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	برائے (i = 0; i < csinfo_سائز; i++)
	{
	    اگر (csinfo[i].fname != تہی && لڑی_تلاش(csinfo[i].fname, stok))
		توڑو;
	}
    }

    اگر ((i != -1) && (i >= csinfo_سائز || i < -1 || csinfo[i].fname == تہی))
    {
	اگر (p_csverbose)
	    (خالی)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    ورنہ
    {
	اگر (i == -1)
	{
	    برائے (i = 0; i < csinfo_سائز; i++)
	    {
		اگر (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	ورنہ
	    cs_kill_execute(i, stok);
    }

    واپس 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    ساکن خالی
cs_kill_execute(
    صحیح i,	    // cscope table index
    حرف *cname)    // cscope database name
{
    اگر (p_csverbose)
    {
	پیغام_clr_eos();
	(خالی)smsg_attr(HL_ATTR(HLF_R) | پیغام_HIST,
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
    ساکن حرف *
cs_make_vim_style_matches(
    حرف *fname,
    حرف *slno,
    حرف *تلاش,
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
    صحیح amt;

    اگر (تلاش != تہی)
    {
	amt = (صحیح)(لڑی_لمبائی(fname) + لڑی_لمبائی(slno) + لڑی_لمبائی(tagstr) + لڑی_لمبائی(تلاش)+6);
	اگر ((buf = alloc(amt)) == تہی)
	    واپس تہی;

	(خالی)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, تلاش);
    }
    ورنہ
    {
	amt = (صحیح)(لڑی_لمبائی(fname) + لڑی_لمبائی(slno) + لڑی_لمبائی(tagstr) + 5);
	اگر ((buf = alloc(amt)) == تہی)
	    واپس تہی;

	(خالی)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    واپس buf;
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
    ساکن حرف *
cs_manage_matches(
    حرف **matches,
    حرف **contexts,
    صحیح totmatches,
    mcmd_e cmd)
{
    ساکن حرف **mp = تہی;
    ساکن حرف **cp = تہی;
    ساکن صحیح cnt = -1;
    ساکن صحیح اگلا = -1;
    حرف *p = تہی;

    انتخاب (cmd)
    {
    حالت Store:
	أكد(matches != تہی);
	أكد(totmatches > 0);
	اگر (mp != تہی || cp != تہی)
	    (خالی)cs_manage_matches(تہی, تہی, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	اگلا = 0;
	توڑو;
    حالت Get:
	اگر (اگلا >= cnt)
	    واپس تہی;

	p = mp[اگلا];
	اگلا++;
	توڑو;
    حالت Free:
	اگر (mp != تہی)
	{
	    اگر (cnt > 0)
		جب (cnt--)
		{
		    vim_free(mp[cnt]);
		    اگر (cp != تہی)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = تہی;
	cp = تہی;
	cnt = 0;
	اگلا = 0;
	توڑو;
    حالت Print:
	cs_print_لیبل_فہرست_priv(mp, cp, cnt);
	توڑو;
    طےشدہ:	// should not reach here
	iemsg(e_fatal_error_اندر_cs_manage_matches);
	واپس تہی;
    }

    واپس p;
}


/*
 * Parse cscope output.
 */
    ساکن حرف *
cs_parse_results(
    صحیح cnumber,
    حرف *buf,
    صحیح bufsize,
    حرف **context,
    حرف **linenumber,
    حرف **تلاش)
{
    صحیح ch;
    حرف *p;
    حرف *name;

    اگر (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == تہی)
    {
	اگر (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	واپس تہی;
    }

    // If the line's too long for the buffer, discard it.
    اگر ((p = strchr(buf, '\n')) == تہی)
    {
	جب ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	واپس تہی;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    اگر ((name = strtok(buf, (ثابت حرف *)" ")) == تہی)
	واپس تہی;
    اگر ((*context = strtok(تہی, (ثابت حرف *)" ")) == تہی)
	واپس تہی;
    اگر ((*linenumber = strtok(تہی, (ثابت حرف *)" ")) == تہی)
	واپس تہی;
    *تلاش = *linenumber + لڑی_لمبائی(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    اگر (strcmp(*تلاش, "<unknown>") == 0)
	*تلاش = تہی;

    name = cs_resolve_file(cnumber, name);
    واپس name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    ساکن خالی
cs_file_results(FILE *f, صحیح *nummatches_a)
{
    صحیح i, j;
    حرف *buf;
    حرف *تلاش, *slno;
    حرف *fullname;
    حرف *cntx;
    حرف *context;

    buf = alloc(CSREAD_BUFSIZE);
    اگر (buf == تہی)
	واپس;

    برائے (i = 0; i < csinfo_سائز; i++)
    {
	اگر (nummatches_a[i] < 1)
	    جاری;

	برائے (j = 0; j < nummatches_a[i]; j++)
	{
	   اگر ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &تلاش)) == تہی)
	       جاری;

	   context = alloc(لڑی_لمبائی(cntx)+5);
	   اگر (context == تہی)
	   {
	       vim_free(fullname);
	       جاری;
	   }

	   اگر (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   ورنہ
	       sprintf(context, "<<%s>>", cntx);

	   اگر (تلاش == تہی)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   ورنہ
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, تلاش);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(خالی)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    ساکن خالی
cs_fill_results(
    حرف *tagstr,
    صحیح totmatches,
    صحیح *nummatches_a,
    حرف ***matches_p,
    حرف ***cntxts_p,
    صحیح *matched)
{
    صحیح i, j;
    حرف *buf;
    حرف *تلاش, *slno;
    صحیح totsofar = 0;
    حرف **matches = تہی;
    حرف **cntxts = تہی;
    حرف *fullname;
    حرف *cntx;

    أكد(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    اگر (buf == تہی)
	واپس;

    اگر ((matches = ALLOC_MULT(حرف *, totmatches)) == تہی)
	جاؤ parse_برآمد;
    اگر ((cntxts = ALLOC_MULT(حرف *, totmatches)) == تہی)
	جاؤ parse_برآمد;

    برائے (i = 0; i < csinfo_سائز; i++)
    {
	اگر (nummatches_a[i] < 1)
	    جاری;

	برائے (j = 0; j < nummatches_a[i]; j++)
	{
	   اگر ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &تلاش)) == تہی)
		جاری;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  تلاش, tagstr);

	    vim_free(fullname);

	    اگر (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = تہی;
	    ورنہ
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (حرف *)vim_strsave((char_u *)cntx);

	    اگر (matches[totsofar] != تہی)
		totsofar++;

	} // for all matches

	(خالی)cs_read_prompt(i);

    } // for all cscope connections

parse_برآمد:
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
    ساکن حرف *
cs_pathcomponents(حرف *راستہ)
{
    صحیح		i;
    حرف	*s;

    اگر (p_cspc == 0)
	واپس راستہ;

    s = راستہ + لڑی_لمبائی(راستہ) - 1;
    برائے (i = 0; i < p_cspc; ++i)
    {
	جب (s > راستہ)
	{
	   s--;
	   اگر (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      توڑو;
	}
    }
    اگر ((s > راستہ && *s == '/')
# ifdef MSWIN
	|| (s > راستہ && *s == '\\')
# endif
	    )
	++s;
    واپس s;
}

/*
 * Called from cs_manage_matches().
 */
    ساکن خالی
cs_print_لیبل_فہرست_priv(حرف **matches, حرف **cntxts, صحیح عدد_matches)
{
    حرف	*buf = تہی;
    حرف	*t_buf;
    صحیح		bufsize = 0; // Track available bufsize
    صحیح		newsize = 0;
    حرف	*ptag;
    حرف	*fname, *lno, *extra, *tbuf;
    صحیح		i, idx, عدد;
    حرف	*globalcntx = "GLOBAL";
    حرف	*cntxformat = " <<%s>>";
    حرف	*context;
    حرف	*cstag_پیغام = _("Cscope tag: %s");
    حرف	*csfmt_str = "%4d %6s  ";

    أكد(عدد_matches > 0);

    اگر ((tbuf = alloc(لڑی_لمبائی(matches[0]) + 1)) == تہی)
	واپس;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    اگر (ptag == تہی)
    {
	vim_free(tbuf);
	واپس;
    }

    newsize = (صحیح)(لڑی_لمبائی(cstag_پیغام) + لڑی_لمبائی(ptag));
    buf = alloc(newsize);
    اگر (buf != تہی)
    {
	bufsize = newsize;
	(خالی)sprintf(buf, cstag_پیغام, ptag);
	پیغام_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    پیغام_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    پیغام_advance(پیغام_ستون + 2);
    پیغام_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    عدد = 1;
    برائے (i = 0; i < عدد_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	اگر ((tbuf = alloc(لڑی_لمبائی(matches[idx]) + 1)) == تہی)
	    جاری;
	(خالی)strcpy(tbuf, matches[idx]);

	اگر (strtok(tbuf, (ثابت حرف *)"\t") == تہی
		|| (fname = strtok(تہی, (ثابت حرف *)"\t")) == تہی
		|| (lno = strtok(تہی, (ثابت حرف *)"\t")) == تہی)
	{
	    vim_free(tbuf);
	    جاری;
	}
	extra = strtok(تہی, (ثابت حرف *)"\t");

	lno[لڑی_لمبائی(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (صحیح)(لڑی_لمبائی(csfmt_str) + 16 + لڑی_لمبائی(lno));
	اگر (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    اگر (buf == تہی)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ورنہ
		bufsize = newsize;
	}
	اگر (buf != تہی)
	{
	    // csfmt_str = "%4d %6s  ";
	    (خالی)sprintf(buf, csfmt_str, عدد, lno);
	    پیغام_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	پیغام_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	اگر (cntxts[idx] != تہی)
	    context = cntxts[idx];
	ورنہ
	    context = globalcntx;
	newsize = (صحیح)(لڑی_لمبائی(context) + لڑی_لمبائی(cntxformat));

	اگر (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    اگر (buf == تہی)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ورنہ
		bufsize = newsize;
	}
	اگر (buf != تہی)
	{
	    (خالی)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    اگر (پیغام_ستون + (صحیح)لڑی_لمبائی(buf) >= cmdline_چوڑائی)
		پیغام_putchar('\n');
	    پیغام_advance(12);
	    پیغام_outtrans_long_attr((char_u *)buf, 0);
	    پیغام_putchar('\n');
	}
	اگر (extra != تہی)
	{
	    پیغام_advance(13);
	    پیغام_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	اگر (پیغام_ستون)
	    پیغام_putchar('\n');

	ui_breakcheck();
	اگر (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    توڑو;
	}

	عدد++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    ساکن صحیح
cs_read_prompt(صحیح i)
{
    صحیح		ch;
    حرف	*buf = تہی; // buffer for possible error message from cscope
    صحیح		bufpos = 0;
    صحیح		maxlen;
    ساکن حرف *eprompt = "Press the RETURN key to continue:";
    صحیح		epromptlen = (صحیح)لڑی_لمبائی(eprompt);
    صحیح		n;

    // compute maximum allowed len for Cscope error message
    حرف *cs_emsg = _(e_cscope_error_str);
    maxlen = (صحیح)(IOSIZE - لڑی_لمبائی(cs_emsg));

    برائے (;;)
    {
	جب ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    اگر (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		اگر (buf == تہی) // lazy buffer allocation
		    buf = alloc(maxlen);
		اگر (buf != تہی)
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
			(خالی)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(خالی)putc('\n', csinfo[i].مقصد_fp);
			(خالی)fflush(csinfo[i].مقصد_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	برائے (n = 0; n < (صحیح)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    اگر (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    اگر (ch == EOF)
	    {
		اگر (buf != تہی && buf[0] != NUL)
		    (خالی)semsg(cs_emsg, buf);
		ورنہ اگر (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		واپس CSCOPE_FAILURE;
	    }

	    اگر (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		توڑو;
	    }
	}

	اگر (ch == EOF)
	    جاری;	    // didn't find the prompt
	توڑو;		    // did find the prompt
    }

    vim_free(buf);
    واپس CSCOPE_SUCCESS;
}

# اگر defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    ساکن خالی
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    ساکن خالی
cs_release_csp(صحیح i, صحیح freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    اگر (csinfo[i].مقصد_fp != تہی)
    {
	(خالی)fputs("q\n", csinfo[i].مقصد_fp);
	(خالی)fflush(csinfo[i].مقصد_fp);
    }
# اگر defined(UNIX)
    {
	صحیح waitpid_errno;
	صحیح pstat;
	pid_t pid;

#  اگر defined(HAVE_SIGACTION)
	ساخت sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   ورنہ
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, تہی);
#  ورنہ
	صحیح waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	برائے (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    اگر (pid != 0)
		توڑو;  // break unless the process is still running
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
		برائے (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    اگر (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			توڑو;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    اگر (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(خالی)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# ورنہ  // !UNIX
    اگر (csinfo[i].hProc != تہی)
    {
	// Give cscope a chance to exit normally
	اگر (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    اگر (csinfo[i].fr_fp != تہی)
	(خالی)fclose(csinfo[i].fr_fp);
    اگر (csinfo[i].مقصد_fp != تہی)
	(خالی)fclose(csinfo[i].مقصد_fp);

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
    ساکن صحیح
cs_reset(exarg_T *eap غیر_مستعمل)
{
    حرف	**dblist = تہی, **pplist = تہی, **fllist = تہی;
    صحیح	i;
    حرف buf[20]; // for sprintf " (#%d)"

    اگر (csinfo_سائز == 0)
	واپس CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(حرف *, csinfo_سائز);
    pplist = ALLOC_MULT(حرف *, csinfo_سائز);
    fllist = ALLOC_MULT(حرف *, csinfo_سائز);
    اگر (dblist == تہی || pplist == تہی || fllist == تہی)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	واپس CSCOPE_FAILURE;
    }

    برائے (i = 0; i < csinfo_سائز; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	اگر (csinfo[i].fname != تہی)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    برائے (i = 0; i < csinfo_سائز; i++)
    {
	اگر (dblist[i] != تہی)
	{
	    cs_جوڑو_common(dblist[i], pplist[i], fllist[i]);
	    اگر (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		پیغام_puts_attr(buf, HL_ATTR(HLF_R));
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
	پیغام_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | پیغام_HIST);
    واپس CSCOPE_SUCCESS;
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
    ساکن حرف *
cs_resolve_file(صحیح i, حرف *name)
{
    حرف	*fullname;
    string_T	csdir = {تہی, 0};
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
    اگر (csinfo[i].ppath != تہی)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (صحیح)ppathlen;
    }
    ورنہ اگر (p_csre && csinfo[i].fname != تہی)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	اگر (csdir.string != تہی)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.لمبائی = STRLEN(csdir.string);
	    len += (صحیح)csdir.لمبائی;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    اگر (csinfo[i].ppath != تہی
	    && (لڑی_موازنہ(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	اگر ((fullname = alloc(len)) != تہی)
	    (خالی)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    ورنہ اگر (csdir.string != تہی && csinfo[i].fname != تہی && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (حرف *)concat_fnames(csdir.string, csdir.لمبائی, (char_u *)name, namelen, TRUE, &ret);
    }
    ورنہ
    {
	fullname = (حرف *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    واپس fullname;
}


/*
 * Show all cscope connections.
 */
    ساکن صحیح
cs_show(exarg_T *eap غیر_مستعمل)
{
    صحیح i;

    اگر (cs_cnt_connections() == 0)
	پیغام_puts(_("no cscope connections\n"));
    ورنہ
    {
	پیغام_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	برائے (i = 0; i < csinfo_سائز; i++)
	{
	    اگر (csinfo[i].fname == تہی)
		جاری;

	    اگر (csinfo[i].ppath != تہی)
		(خالی)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (طویل)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    ورنہ
		(خالی)smsg("%2d %-5ld  %-34s  <none>",
			   i, (طویل)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    واپس CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    خالی
cs_انتہا(خالی)
{
    صحیح i;

    برائے (i = 0; i < csinfo_سائز; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_سائز = 0;
}

#اگر_ختم	// FEAT_CSCOPE

#اگر defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    خالی
f_cscope_connection(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_CSCOPE
    صحیح		عدد = 0;
    char_u	*dbpath = تہی;
    char_u	*prepend = تہی;
    char_u	buf[NUMBUFLEN];

    اگر (اندر_vim9script()
	    && (check_for_opt_نمبر_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	عدد = (صحیح)tv_get_نمبر(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_نمبر = cs_connection(عدد, dbpath, prepend);
# endif
}

#اگر_ختم // FEAT_EVAL
