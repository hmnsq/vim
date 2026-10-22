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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# எனில் defined(UNIX)
#  include <sys/wait.h>
# endif

# எனில் defined (MSWIN)
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

வகைவரை கட்டமைப்பு {
    எழுத்து *  name;
    முழு     (*func)(exarg_T *eap);
    எழுத்து *  help;
    எழுத்து *  usage;
    முழு	    cansplit;		// if supports splitting window
} cscmd_T;

வகைவரை கட்டமைப்பு csi {
    எழுத்து *	    fname;	// cscope db name
    எழுத்து *	    ppath;	// path to prepend (the -P option)
    எழுத்து *	    flags;	// additional cscope flags/options (e.g, -p2)
# எனில் defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# இல்லையெனில்
#  எனில் defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_Index_உயர்வு;	// st_ino has no meaning in the Windows
    DWORD	    n_Index_கீழ்;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    to_fp;	// to cscope: FILE.
} csinfo_T;

வகைவரை எண்ணிக்கை { Add, Find, Help, Kill, Reset, Show } csid_e;

வகைவரை எண்ணிக்கை {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

நிலையான முழு	    cs_add(exarg_T *eap);
நிலையான முழு	    cs_add_common(எழுத்து *, எழுத்து *, எழுத்து *);
நிலையான முழு	    cs_check_for_connections(வெறுமை);
நிலையான முழு	    cs_check_for_tags(வெறுமை);
நிலையான முழு	    cs_cnt_connections(வெறுமை);
நிலையான முழு	    cs_create_connection(முழு i);
# ifdef FEAT_QUICKFIX
நிலையான வெறுமை	    cs_file_results(FILE *, முழு *);
# endif
நிலையான வெறுமை	    cs_fill_results(எழுத்து *, முழு , முழு *, எழுத்து ***,
			எழுத்து ***, முழு *);
நிலையான முழு	    cs_find(exarg_T *eap);
நிலையான முழு	    cs_find_common(எழுத்து *opt, எழுத்து *pat, முழு, முழு, முழு, char_u *cmdline);
நிலையான முழு	    cs_help(exarg_T *eap);
நிலையான முழு	    cs_செருகு_filelist(எழுத்து *, எழுத்து *, எழுத்து *,
			stat_T *);
நிலையான முழு	    cs_kill(exarg_T *eap);
நிலையான வெறுமை	    cs_kill_execute(முழு, எழுத்து *);
நிலையான cscmd_T *    cs_lookup_cmd(exarg_T *eap);
நிலையான எழுத்து *	    cs_make_vim_style_matches(எழுத்து *, எழுத்து *,
			எழுத்து *, எழுத்து *);
நிலையான எழுத்து *	    cs_manage_matches(எழுத்து **, எழுத்து **, முழு, mcmd_e);
நிலையான வெறுமை	    cs_print_tags_priv(எழுத்து **, எழுத்து **, முழு);
நிலையான முழு	    cs_read_prompt(முழு);
நிலையான வெறுமை	    cs_release_csp(முழு, முழு freefnpp);
நிலையான முழு	    cs_reset(exarg_T *eap);
நிலையான எழுத்து *	    cs_resolve_file(முழு, எழுத்து *);
நிலையான முழு	    cs_show(exarg_T *eap);


நிலையான csinfo_T *   csinfo = வெற்று;
நிலையான முழு	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

நிலையான முழு	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
நிலையான cscmd_T	    cs_cmds[] =
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
    { வெற்று, வெற்று, வெற்று, வெற்று, 0 }
};

    நிலையான வெறுமை
cs_usage_செய்தி(csid_e x)
{
    (வெறுமை)semsg(_(e_usage_cscope_str), cs_cmds[(முழு)x].usage);
}

நிலையான எண்ணிக்கை
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
get_cscope_name(expand_T *xp UNUSED, முழு idx)
{
    முழு		current_idx;
    முழு		i;

    தேர்வு (expand_what)
    {
    நிலை EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	திரும்பு (char_u *)cs_cmds[idx].name;
    நிலை EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	க்கு (i = 0, current_idx = 0; cs_cmds[i].name != வெற்று; i++)
	    எனில் (cs_cmds[i].cansplit)
		எனில் (current_idx++ == idx)
		    நிறுத்து;
	திரும்பு (char_u *)cs_cmds[i].name;
    நிலை EXP_CSCOPE_FIND:
	{
	    மாறிலி எழுத்து *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", வெற்று
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    திரும்பு (char_u *)query_type[idx];
	}
    நிலை EXP_CSCOPE_KILL:
	{
	    நிலையான எழுத்து	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    க்கு (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		எனில் (csinfo[i].fname == வெற்று)
		    தொடர்;
		எனில் (current_idx++ == idx)
		{
		    vim_snprintf(connection, அளவு(connection), "%d", i);
		    திரும்பு (char_u *)connection;
		}
	    }
	    திரும்பு (current_idx == idx && idx > 0) ? (char_u *)"-1" : வெற்று;
	}
    இயல்பு:
	திரும்பு வெற்று;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    வெறுமை
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

    எனில் (*arg == NUL)
	திரும்பு;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    எனில் (*p == NUL)
	திரும்பு;

    // past first word
    xp->xp_pattern = skipwhite(p);
    எனில் (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    இல்லையெனில் எனில் (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    இல்லையெனில் எனில் (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    இல்லையெனில் எனில் (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    இல்லையெனில்
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    நிலையான வெறுமை
do_cscope_general(
    exarg_T	*eap,
    முழு		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    எனில் (check_restricted())
	திரும்பு;

    எனில் ((cmdp = cs_lookup_cmd(eap)) == வெற்று)
    {
	cs_help(eap);
	திரும்பு;
    }

    எனில் (make_split)
    {
	எனில் (!cmdp->cansplit)
	{
	    (வெறுமை)செய்தி_puts(_("This cscope command does not support splitting the window.\n"));
	    திரும்பு;
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
    வெறுமை
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    வெறுமை
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    வெறுமை
ex_cstag(exarg_T *eap)
{
    முழு ret = FALSE;

    எனில் (*eap->arg == NUL)
    {
	(வெறுமை)emsg(_(e_usage_cstag_ident));
	திரும்பு;
    }

    தேர்வு (p_csto)
    {
    நிலை 0 :
	எனில் (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (எழுத்து *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    எனில் (ret == FALSE)
	    {
		cs_free_tags();
		எனில் (செய்தி_col)
		    செய்தி_putchar('\n');

		எனில் (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	இல்லையெனில் எனில் (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	நிறுத்து;
    நிலை 1 :
	எனில் (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    எனில் (ret == FALSE)
	    {
		எனில் (செய்தி_col)
		    செய்தி_putchar('\n');

		எனில் (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (எழுத்து *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    எனில் (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	இல்லையெனில் எனில் (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (எழுத்து *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    எனில் (ret == FALSE)
		cs_free_tags();
	}
	நிறுத்து;
    இயல்பு :
	நிறுத்து;
    }

    எனில் (!ret)
    {
	(வெறுமை)emsg(_(e_cstag_tag_not_founc));
# எனில் defined(FEAT_QUICKFIX)
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
    முழு
cs_fgets(char_u *buf, முழு size)
{
    எழுத்து *p;

    எனில் ((p = cs_manage_matches(வெற்று, வெற்று, -1, Get)) == வெற்று)
	திரும்பு TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    திரும்பு FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    வெறுமை
cs_free_tags(வெறுமை)
{
    cs_manage_matches(வெற்று, வெற்று, -1, Free);
}


/*
 * Called from do_tag().
 */
    வெறுமை
cs_print_tags(வெறுமை)
{
    cs_manage_matches(வெற்று, வெற்று, -1, Print);
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
# எனில் defined(FEAT_EVAL)
    நிலையான முழு
cs_connection(முழு இலக்கம், char_u *dbpath, char_u *ppath)
{
    முழு i;

    எனில் (இலக்கம் < 0 || இலக்கம் > 4 || (இலக்கம் > 0 && !dbpath))
	திரும்பு FALSE;

    க்கு (i = 0; i < csinfo_size; i++)
    {
	எனில் (!csinfo[i].fname)
	    தொடர்;

	எனில் (இலக்கம் == 0)
	    திரும்பு TRUE;

	தேர்வு (இலக்கம்)
	{
	நிலை 1:
	    எனில் (strstr(csinfo[i].fname, (எழுத்து *)dbpath))
		திரும்பு TRUE;
	    நிறுத்து;
	நிலை 2:
	    எனில் (strcmp(csinfo[i].fname, (எழுத்து *)dbpath) == 0)
		திரும்பு TRUE;
	    நிறுத்து;
	நிலை 3:
	    எனில் (strstr(csinfo[i].fname, (எழுத்து *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (எழுத்து *)ppath))))
		திரும்பு TRUE;
	    நிறுத்து;
	நிலை 4:
	    எனில் ((strcmp(csinfo[i].fname, (எழுத்து *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (எழுத்து *)ppath) == 0))))
		திரும்பு TRUE;
	    நிறுத்து;
	}
    }

    திரும்பு FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    நிலையான முழு
cs_add(exarg_T *eap UNUSED)
{
    எழுத்து *fname, *ppath, *flags = வெற்று;

    எனில் ((fname = strtok((எழுத்து *)வெற்று, (மாறிலி எழுத்து *)" ")) == வெற்று)
    {
	cs_usage_செய்தி(Add);
	திரும்பு CSCOPE_FAILURE;
    }
    எனில் ((ppath = strtok((எழுத்து *)வெற்று, (மாறிலி எழுத்து *)" ")) != வெற்று)
	flags = strtok((எழுத்து *)வெற்று, (மாறிலி எழுத்து *)" ");

    திரும்பு cs_add_common(fname, ppath, flags);
}

    நிலையான வெறுமை
cs_stat_emsg(எழுத்து *fname)
{
    முழு err = errno;
    (வெறுமை)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    நிலையான முழு
cs_add_common(
    எழுத்து *arg1,	    // filename - may contain environment variables
    எழுத்து *arg2,	    // prepend path - may contain environment variables
    எழுத்து *flags)
{
    stat_T	statbuf;
    முழு		ret;
    எழுத்து	*fname = வெற்று;
    எழுத்து	*fname2 = வெற்று;
    எழுத்து	*ppath = வெற்று;
    முழு		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = வெற்று;

    // get the filename (arg1), expand it, and try to stat it
    எனில் ((fname = alloc(MAXPATHL + 1)) == வெற்று)
	செல் add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (வெறுமை)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    எனில் (fname == வெற்று)
	செல் add_err;
    fname = (எழுத்து *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    எனில் (ret < 0)
    {
staterr:
	எனில் (p_csverbose)
	    cs_stat_emsg(fname);
	செல் add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    எனில் (arg2 != வெற்று)
    {
	stat_T	    statbuf2;

	எனில் ((ppath = alloc(MAXPATHL + 1)) == வெற்று)
	    செல் add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	எனில் (ret < 0)
	    செல் staterr;
    }

    // if filename is a directory, append the cscope database name to it
    எனில் (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	எனில் (fname2 == வெற்று)
	    செல் add_err;

	போது (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    எனில் (fname[0] == '\0')
		நிறுத்து;
	}
	எனில் (fname[0] == '\0')
	    (வெறுமை)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	இல்லையெனில்
	    (வெறுமை)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	எனில் (ret < 0)
	{
	    எனில் (p_csverbose)
		cs_stat_emsg(fname2);
	    செல் add_err;
	}

	i = cs_செருகு_filelist(fname2, ppath, flags, &statbuf);
    }
    இல்லையெனில் எனில் (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_செருகு_filelist(fname, ppath, flags, &statbuf);
    }
    இல்லையெனில்
    {
	எனில் (p_csverbose)
	    (வெறுமை)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	செல் add_err;
    }

    எனில் (i != -1)
    {
	எனில் (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    செல் add_err;
	}

	எனில் (p_csverbose)
	{
	    செய்தி_clr_eos();
	    (வெறுமை)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    திரும்பு CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    திரும்பு CSCOPE_FAILURE;
}


    நிலையான முழு
cs_check_for_connections(வெறுமை)
{
    திரும்பு (cs_cnt_connections() > 0);
}


    நிலையான முழு
cs_check_for_tags(வெறுமை)
{
    திரும்பு (p_tags[0] != NUL && curbuf->b_p_tags != வெற்று);
}


/*
 * Count the number of cscope connections.
 */
    நிலையான முழு
cs_cnt_connections(வெறுமை)
{
    முழு i;
    முழு cnt = 0;

    க்கு (i = 0; i < csinfo_size; i++)
	எனில் (csinfo[i].fname != வெற்று)
	    cnt++;
    திரும்பு cnt;
}

    நிலையான வெறுமை
cs_reading_emsg(
    முழு idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    நிலையான முழு
cs_cnt_matches(முழு idx)
{
    எழுத்து *stok;
    எழுத்து *buf;
    முழு nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    எனில் (buf == வெற்று)
	திரும்பு 0;
    க்கு (;;)
    {
	எனில் (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    எனில் (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    திரும்பு -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	எனில் (strstr((மாறிலி எழுத்து *)buf, "Unable to search database") != வெற்று)
	    நிறுத்து;
	எனில் ((stok = strtok(buf, (மாறிலி எழுத்து *)" ")) == வெற்று)
	    தொடர்;
	எனில் (strstr((மாறிலி எழுத்து *)stok, "cscope:") == வெற்று)
	    தொடர்;

	எனில் ((stok = strtok(வெற்று, (மாறிலி எழுத்து *)" ")) == வெற்று)
	    தொடர்;
	nlines = atoi(stok);
	எனில் (nlines < 0)
	{
	    nlines = 0;
	    நிறுத்து;
	}

	எனில் ((stok = strtok(வெற்று, (மாறிலி எழுத்து *)" ")) == வெற்று)
	    தொடர்;
	எனில் (strncmp((மாறிலி எழுத்து *)stok, "lines", 5))
	    தொடர்;

	நிறுத்து;
    }

    vim_free(buf);
    திரும்பு nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    நிலையான எழுத்து *
cs_create_cmd(எழுத்து *csoption, எழுத்து *pattern)
{
    எழுத்து *cmd;
    short தேடல்;
    எழுத்து *pat;

    தேர்வு (csoption[0])
    {
    நிலை '0' : நிலை 's' :
	தேடல் = 0;
	நிறுத்து;
    நிலை '1' : நிலை 'g' :
	தேடல் = 1;
	நிறுத்து;
    நிலை '2' : நிலை 'd' :
	தேடல் = 2;
	நிறுத்து;
    நிலை '3' : நிலை 'c' :
	தேடல் = 3;
	நிறுத்து;
    நிலை '4' : நிலை 't' :
	தேடல் = 4;
	நிறுத்து;
    நிலை '6' : நிலை 'e' :
	தேடல் = 6;
	நிறுத்து;
    நிலை '7' : நிலை 'f' :
	தேடல் = 7;
	நிறுத்து;
    நிலை '8' : நிலை 'i' :
	தேடல் = 8;
	நிறுத்து;
    நிலை '9' : நிலை 'a' :
	தேடல் = 9;
	நிறுத்து;
    இயல்பு :
	(வெறுமை)emsg(_(e_unknown_cscope_தேடல்_type));
	cs_usage_செய்தி(Find);
	திரும்பு வெற்று;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    எனில் (தேடல் != 4 && தேடல் != 6)
	போது VIM_ISWHITE(*pat)
	    ++pat;

    எனில் ((cmd = alloc(strlen(pat) + 2)) == வெற்று)
	திரும்பு வெற்று;

    (வெறுமை)sprintf(cmd, "%d%s", தேடல், pat);

    திரும்பு cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    நிலையான முழு
cs_create_connection(முழு i)
{
# ifdef UNIX
    முழு		to_cs[2], from_cs[2];
# endif
    முழு		cmdlen;
    எழுத்து	*prog, *cmd, *ppath = வெற்று;
    size_t	proglen;
# ifdef MSWIN
    முழு		fd;
    முழு		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# எனில் defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    எனில் (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(வெறுமை)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	எனில் (to_cs[0] != -1)
	    (வெறுமை)close(to_cs[0]);
	எனில் (to_cs[1] != -1)
	    (வெறுமை)close(to_cs[1]);
	எனில் (from_cs[0] != -1)
	    (வெறுமை)close(from_cs[0]);
	எனில் (from_cs[1] != -1)
	    (வெறுமை)close(from_cs[1]);
	திரும்பு CSCOPE_FAILURE;
    }

    எனில் ((csinfo[i].pid = fork()) == -1)
    {
	(வெறுமை)emsg(_(e_could_not_fork_for_cscope));
	செல் err_closing;
    }
    இல்லையெனில் எனில் (csinfo[i].pid == 0)	// child: run cscope.
    {
	எழுத்து **argv = வெற்று;

	எனில் (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	எனில் (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	எனில் (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(வெறுமை)close(to_cs[1]);
	(வெறுமை)close(from_cs[0]);
# இல்லையெனில்
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = அளவு(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = வெற்று;

	எனில் (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (வெறுமை)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    எனில் (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    எனில் (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    திரும்பு CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	எனில் ((prog = alloc(MAXPATHL + 1)) == வெற்று)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# இல்லையெனில்
	    // MSWIN
	    செல் err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (முழு)(proglen + strlen(csinfo[i].fname) + 32);
	எனில் (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    எனில் ((ppath = alloc(MAXPATHL + 1)) == வெற்று)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# இல்லையெனில்
		// MSWIN
		செல் err_closing;
# endif
	    }
	    cmdlen += (முழு)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	எனில் (csinfo[i].flags)
	    cmdlen += (முழு)strlen(csinfo[i].flags);

	எனில் ((cmd = alloc(cmdlen)) == வெற்று)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# இல்லையெனில்
	    // MSWIN
	    செல் err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    எழுத்து	**tok = வெற்று;
	    முழு		tokc = 0;

	    ga_init2(&ga_argv, அளவு(எழுத்து *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    எனில் (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    க்கு (முழு k = 0; k < tokc; ++k)
	    {
		எனில் (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((எழுத்து **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    எனில் (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((எழுத்து **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (எழுத்து *)vim_strsave((char_u *)"-dl");
	    ((எழுத்து **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (எழுத்து *)vim_strsave((char_u *)"-f");
	    ((எழுத்து **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(எழுத்து *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    எனில் (ppath != வெற்று)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		எழுத்து	*parg = alloc(plen);

		எனில் (parg == வெற்று || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((எழுத்து **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    எனில் (csinfo[i].flags != வெற்று)
	    {
		எனில் (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		க்கு (முழு k = 0; k < tokc; ++k)
		{
		    எனில் (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((எழுத்து **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    எனில் (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((எழுத்து **)ga_argv.ga_data)[ga_argv.ga_len] = வெற்று;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  எனில் defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   எனில் defined(HAVE_SETSID)
	    (வெறுமை)setsid();
#   இல்லையெனில்
	    எனில் (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (எழுத்து **)ga_argv.ga_data;
	    எனில் (argv[0] == வெற்று || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    இல்லையெனில்	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	எனில் ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == வெற்று)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	எனில் ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == வெற்று)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(வெறுமை)close(to_cs[0]);
	(வெறுமை)close(from_cs[1]);
    }
# இல்லையெனில்
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    எனில் (csinfo[i].ppath != வெற்று)
    {
	len = (முழு)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    எனில் (csinfo[i].flags != வெற்று)
    {
	len = (முழு)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_உள்ளீடு  = stdin_rd;
    created = CreateProcess(வெற்று, cmd, வெற்று, வெற்று, TRUE, CREATE_NEW_CONSOLE,
							வெற்று, வெற்று, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    எனில் (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(வெறுமை)emsg(_(e_could_not_spawn_cscope_process));
	செல் err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    எனில் (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == வெற்று))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    எனில் (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == வெற்று))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    திரும்பு CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    நிலையான முழு
cs_find(exarg_T *eap)
{
    எழுத்து *opt, *pat;
    முழு i;

    எனில் (cs_check_for_connections() == FALSE)
    {
	(வெறுமை)emsg(_(e_no_cscope_connections));
	திரும்பு FALSE;
    }

    எனில் ((opt = strtok((எழுத்து *)வெற்று, (மாறிலி எழுத்து *)" ")) == வெற்று)
    {
	cs_usage_செய்தி(Find);
	திரும்பு FALSE;
    }

    pat = opt + strlen(opt) + 1;
    எனில் (pat >= (எழுத்து *)eap->arg + eap_arg_len)
    {
	cs_usage_செய்தி(Find);
	திரும்பு FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    க்கு (i = 0; i < eap_arg_len; ++i)
	எனில் (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    திரும்பு cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    நிலையான முழு
cs_find_common(
    எழுத்து *opt,
    எழுத்து *pat,
    முழு forceit,
    முழு verbose,
    முழு	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    முழு i;
    எழுத்து *cmd;
    முழு *nummatches;
    முழு totmatches;
# ifdef FEAT_QUICKFIX
    எழுத்து cmdletter;
    எழுத்து *qfpos;

    // get cmd letter
    தேர்வு (opt[0])
    {
    நிலை '0' :
	cmdletter = 's';
	நிறுத்து;
    நிலை '1' :
	cmdletter = 'g';
	நிறுத்து;
    நிலை '2' :
	cmdletter = 'd';
	நிறுத்து;
    நிலை '3' :
	cmdletter = 'c';
	நிறுத்து;
    நிலை '4' :
	cmdletter = 't';
	நிறுத்து;
    நிலை '6' :
	cmdletter = 'e';
	நிறுத்து;
    நிலை '7' :
	cmdletter = 'f';
	நிறுத்து;
    நிலை '8' :
	cmdletter = 'i';
	நிறுத்து;
    நிலை '9' :
	cmdletter = 'a';
	நிறுத்து;
    இயல்பு :
	cmdletter = opt[0];
    }

    qfpos = (எழுத்து *)vim_strchr(p_csqf, cmdletter);
    எனில் (qfpos != வெற்று)
    {
	qfpos++;
	// next symbol must be + or -
	எனில் (strchr(CSQF_FLAGS, *qfpos) == வெற்று)
	{
	    (வெறுமை)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    திரும்பு FALSE;
	}

	எனில் (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    எனில் (aborting())
		திரும்பு FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    எனில் (cmd == வெற்று)
	திரும்பு FALSE;

    nummatches = ALLOC_MULT(முழு, csinfo_size);
    எனில் (nummatches == வெற்று)
    {
	vim_free(cmd);
	திரும்பு FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    க்கு (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    க்கு (i = 0; i < csinfo_size; i++)
    {
	எனில் (csinfo[i].fname == வெற்று || csinfo[i].to_fp == வெற்று)
	    தொடர்;

	// send cmd to cscope
	(வெறுமை)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(வெறுமை)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	எனில் (nummatches[i] > -1)
	    totmatches += nummatches[i];

	எனில் (nummatches[i] == 0)
	    (வெறுமை)cs_read_prompt(i);
    }
    vim_free(cmd);

    எனில் (totmatches == 0)
    {
	எனில் (verbose)
	    (வெறுமை)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	திரும்பு FALSE;
    }

# ifdef FEAT_QUICKFIX
    எனில் (qfpos != வெற்று && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = வெற்று;
	win_T	    *wp = வெற்று;

	எனில் (tmp == வெற்று)
	{
	    vim_free(nummatches);
	    திரும்பு FALSE;
	}

	f = mch_fopen((எழுத்து *)tmp, "w");
	எனில் (f == வெற்று)
	    semsg(_(e_cant_open_file_str), tmp);
	இல்லையெனில்
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    எனில் (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    எனில் (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, வெற்று) > 0)
	    {
		எனில் (postponed_split != 0)
		{
		    (வெறுமை)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		எனில் (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != வெற்று)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	திரும்பு TRUE;
    }
    இல்லையெனில்
# endif // FEAT_QUICKFIX
    {
	எழுத்து **matches = வெற்று, **contexts = வெற்று;
	முழு matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	எனில் (matches == வெற்று)
	    திரும்பு FALSE;

	(வெறுமை)cs_manage_matches(matches, contexts, matched, Store);

	திரும்பு do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    நிலையான முழு
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (வெறுமை)செய்தி_puts(_("cscope commands:\n"));
    போது (cmdp->name != வெற்று)
    {
	எழுத்து *help = _(cmdp->help);
	முழு  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	எனில் (space_cnt < 0)
	    space_cnt = 0;
	(வெறுமை)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	எனில் (strcmp(cmdp->name, "find") == 0)
	    செய்தி_puts(_("\n"
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
    திரும்பு 0;
}


    நிலையான வெறுமை
clear_csinfo(முழு i)
{
    csinfo[i].fname  = வெற்று;
    csinfo[i].ppath  = வெற்று;
    csinfo[i].flags  = வெற்று;
# எனில் defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# இல்லையெனில்
    csinfo[i].nVolume = 0;
    csinfo[i].n_Index_உயர்வு = 0;
    csinfo[i].n_Index_கீழ் = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = வெற்று;
    csinfo[i].to_fp  = வெற்று;
# எனில் defined(MSWIN)
    csinfo[i].hProc = வெற்று;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    நிலையான முழு
cs_செருகு_filelist(
    எழுத்து *fname,
    எழுத்து *ppath,
    எழுத்து *flags,
    stat_T *sb UNUSED)
{
    முழு	    i;
    முழு	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    தேர்வு (win32_fileinfo((char_u *)fname, &bhfi))
    {
	நிலை FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	நிலை FILEINFO_READ_FAIL:	// CreateFile() failed
	    எனில் (p_csverbose)
	    {
		எழுத்து *cant_செய்தி = _(e_cannot_open_cscope_database_str);
		எழுத்து *winmsg = GetWin32Error();

		எனில் (winmsg != வெற்று)
		    (வெறுமை)semsg(cant_செய்தி, winmsg);
		இல்லையெனில்
		    // subst filename if can't get error text
		    (வெறுமை)semsg(cant_செய்தி, fname);
	    }
	    திரும்பு -1;

	நிலை FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    எனில் (p_csverbose)
		(வெறுமை)emsg(_(e_cannot_get_cscope_database_information));
	    திரும்பு -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    க்கு (j = 0; j < csinfo_size; j++)
    {
	எனில் (csinfo[j].fname != வெற்று
# எனில் defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# இல்லையெனில்
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_எண்
		    && csinfo[j].n_Index_உயர்வு == bhfi.n_File_Index_உயர்வு
		    && csinfo[j].n_Index_கீழ் == bhfi.n_File_Index_கீழ்))
# endif
	    )
	{
	    எனில் (p_csverbose)
		(வெறுமை)emsg(_(e_duplicate_cscope_database_not_added));
	    திரும்பு -1;
	}

	எனில் (csinfo[j].fname == வெற்று && i == -1)
	    i = j; // remember first empty entry
    }

    எனில் (i == -1)
    {
	i = csinfo_size;
	எனில் (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	இல்லையெனில்
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, அளவு(csinfo_T)*csinfo_size);
	    எனில் (csinfo == வெற்று)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		திரும்பு -1;
	    }
	}
	எனில் (csinfo == வெற்று)
	    திரும்பு -1;
	க்கு (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    எனில் ((csinfo[i].fname = alloc(strlen(fname)+1)) == வெற்று)
	திரும்பு -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((மாறிலி எழுத்து *)fname));

    எனில் (ppath != வெற்று)
    {
	எனில் ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == வெற்று)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    திரும்பு -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((மாறிலி எழுத்து *)ppath));
    }
    இல்லையெனில்
	csinfo[i].ppath = வெற்று;

    எனில் (flags != வெற்று)
    {
	எனில் ((csinfo[i].flags = alloc(strlen(flags) + 1)) == வெற்று)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    திரும்பு -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((மாறிலி எழுத்து *)flags));
    }
    இல்லையெனில்
	csinfo[i].flags = வெற்று;

# எனில் defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# இல்லையெனில்
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_எண்;
    csinfo[i].n_Index_கீழ் = bhfi.n_File_Index_கீழ்;
    csinfo[i].n_Index_உயர்வு = bhfi.n_File_Index_உயர்வு;
# endif
    திரும்பு i;
}


/*
 * Find cscope command in command table.
 */
    நிலையான cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    எழுத்து *stok;
    size_t len;

    எனில் (eap->arg == வெற்று)
	திரும்பு வெற்று;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (முழு)STRLEN(eap->arg);

    எனில் ((stok = strtok((எழுத்து *)(eap->arg), (மாறிலி எழுத்து *)" ")) == வெற்று)
	திரும்பு வெற்று;

    len = strlen(stok);
    க்கு (cmdp = cs_cmds; cmdp->name != வெற்று; ++cmdp)
    {
	எனில் (strncmp((மாறிலி எழுத்து *)(stok), cmdp->name, len) == 0)
	    திரும்பு (cmdp);
    }
    திரும்பு வெற்று;
}


/*
 * Nuke em.
 */
    நிலையான முழு
cs_kill(exarg_T *eap UNUSED)
{
    எழுத்து *stok;
    முழு i;

    எனில் ((stok = strtok((எழுத்து *)வெற்று, (மாறிலி எழுத்து *)" ")) == வெற்று)
    {
	cs_usage_செய்தி(Kill);
	திரும்பு CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    எனில் ((strlen(stok) < 2 && VIM_ISDIGIT((முழு)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((முழு)(stok[1]))))
	i = atoi(stok);
    இல்லையெனில்
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	க்கு (i = 0; i < csinfo_size; i++)
	{
	    எனில் (csinfo[i].fname != வெற்று && strstr(csinfo[i].fname, stok))
		நிறுத்து;
	}
    }

    எனில் ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == வெற்று))
    {
	எனில் (p_csverbose)
	    (வெறுமை)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    இல்லையெனில்
    {
	எனில் (i == -1)
	{
	    க்கு (i = 0; i < csinfo_size; i++)
	    {
		எனில் (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	இல்லையெனில்
	    cs_kill_execute(i, stok);
    }

    திரும்பு 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    நிலையான வெறுமை
cs_kill_execute(
    முழு i,	    // cscope table index
    எழுத்து *cname)    // cscope database name
{
    எனில் (p_csverbose)
    {
	செய்தி_clr_eos();
	(வெறுமை)smsg_attr(HL_ATTR(HLF_R) | செய்தி_HIST,
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
    நிலையான எழுத்து *
cs_make_vim_style_matches(
    எழுத்து *fname,
    எழுத்து *slno,
    எழுத்து *தேடல்,
    எழுத்து *tagstr)
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
    எழுத்து *buf;
    முழு amt;

    எனில் (தேடல் != வெற்று)
    {
	amt = (முழு)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(தேடல்)+6);
	எனில் ((buf = alloc(amt)) == வெற்று)
	    திரும்பு வெற்று;

	(வெறுமை)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, தேடல்);
    }
    இல்லையெனில்
    {
	amt = (முழு)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	எனில் ((buf = alloc(amt)) == வெற்று)
	    திரும்பு வெற்று;

	(வெறுமை)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    திரும்பு buf;
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
    நிலையான எழுத்து *
cs_manage_matches(
    எழுத்து **matches,
    எழுத்து **contexts,
    முழு totmatches,
    mcmd_e cmd)
{
    நிலையான எழுத்து **mp = வெற்று;
    நிலையான எழுத்து **cp = வெற்று;
    நிலையான முழு cnt = -1;
    நிலையான முழு next = -1;
    எழுத்து *p = வெற்று;

    தேர்வு (cmd)
    {
    நிலை Store:
	ப்ரதிஜ்ஞா(matches != வெற்று);
	ப்ரதிஜ்ஞா(totmatches > 0);
	எனில் (mp != வெற்று || cp != வெற்று)
	    (வெறுமை)cs_manage_matches(வெற்று, வெற்று, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	நிறுத்து;
    நிலை Get:
	எனில் (next >= cnt)
	    திரும்பு வெற்று;

	p = mp[next];
	next++;
	நிறுத்து;
    நிலை Free:
	எனில் (mp != வெற்று)
	{
	    எனில் (cnt > 0)
		போது (cnt--)
		{
		    vim_free(mp[cnt]);
		    எனில் (cp != வெற்று)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = வெற்று;
	cp = வெற்று;
	cnt = 0;
	next = 0;
	நிறுத்து;
    நிலை Print:
	cs_print_tags_priv(mp, cp, cnt);
	நிறுத்து;
    இயல்பு:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	திரும்பு வெற்று;
    }

    திரும்பு p;
}


/*
 * Parse cscope output.
 */
    நிலையான எழுத்து *
cs_parse_results(
    முழு cnumber,
    எழுத்து *buf,
    முழு bufsize,
    எழுத்து **context,
    எழுத்து **linenumber,
    எழுத்து **தேடல்)
{
    முழு ch;
    எழுத்து *p;
    எழுத்து *name;

    எனில் (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == வெற்று)
    {
	எனில் (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	திரும்பு வெற்று;
    }

    // If the line's too long for the buffer, discard it.
    எனில் ((p = strchr(buf, '\n')) == வெற்று)
    {
	போது ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	திரும்பு வெற்று;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    எனில் ((name = strtok(buf, (மாறிலி எழுத்து *)" ")) == வெற்று)
	திரும்பு வெற்று;
    எனில் ((*context = strtok(வெற்று, (மாறிலி எழுத்து *)" ")) == வெற்று)
	திரும்பு வெற்று;
    எனில் ((*linenumber = strtok(வெற்று, (மாறிலி எழுத்து *)" ")) == வெற்று)
	திரும்பு வெற்று;
    *தேடல் = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    எனில் (strcmp(*தேடல், "<unknown>") == 0)
	*தேடல் = வெற்று;

    name = cs_resolve_file(cnumber, name);
    திரும்பு name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    நிலையான வெறுமை
cs_file_results(FILE *f, முழு *nummatches_a)
{
    முழு i, j;
    எழுத்து *buf;
    எழுத்து *தேடல், *slno;
    எழுத்து *fullname;
    எழுத்து *cntx;
    எழுத்து *context;

    buf = alloc(CSREAD_BUFSIZE);
    எனில் (buf == வெற்று)
	திரும்பு;

    க்கு (i = 0; i < csinfo_size; i++)
    {
	எனில் (nummatches_a[i] < 1)
	    தொடர்;

	க்கு (j = 0; j < nummatches_a[i]; j++)
	{
	   எனில் ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &தேடல்)) == வெற்று)
	       தொடர்;

	   context = alloc(strlen(cntx)+5);
	   எனில் (context == வெற்று)
	   {
	       vim_free(fullname);
	       தொடர்;
	   }

	   எனில் (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   இல்லையெனில்
	       sprintf(context, "<<%s>>", cntx);

	   எனில் (தேடல் == வெற்று)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   இல்லையெனில்
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, தேடல்);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(வெறுமை)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    நிலையான வெறுமை
cs_fill_results(
    எழுத்து *tagstr,
    முழு totmatches,
    முழு *nummatches_a,
    எழுத்து ***matches_p,
    எழுத்து ***cntxts_p,
    முழு *matched)
{
    முழு i, j;
    எழுத்து *buf;
    எழுத்து *தேடல், *slno;
    முழு totsofar = 0;
    எழுத்து **matches = வெற்று;
    எழுத்து **cntxts = வெற்று;
    எழுத்து *fullname;
    எழுத்து *cntx;

    ப்ரதிஜ்ஞா(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    எனில் (buf == வெற்று)
	திரும்பு;

    எனில் ((matches = ALLOC_MULT(எழுத்து *, totmatches)) == வெற்று)
	செல் parse_out;
    எனில் ((cntxts = ALLOC_MULT(எழுத்து *, totmatches)) == வெற்று)
	செல் parse_out;

    க்கு (i = 0; i < csinfo_size; i++)
    {
	எனில் (nummatches_a[i] < 1)
	    தொடர்;

	க்கு (j = 0; j < nummatches_a[i]; j++)
	{
	   எனில் ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &தேடல்)) == வெற்று)
		தொடர்;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  தேடல், tagstr);

	    vim_free(fullname);

	    எனில் (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = வெற்று;
	    இல்லையெனில்
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (எழுத்து *)vim_strsave((char_u *)cntx);

	    எனில் (matches[totsofar] != வெற்று)
		totsofar++;

	} // for all matches

	(வெறுமை)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    எனில் (totsofar == 0)
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
    நிலையான எழுத்து *
cs_pathcomponents(எழுத்து *path)
{
    முழு		i;
    எழுத்து	*s;

    எனில் (p_cspc == 0)
	திரும்பு path;

    s = path + strlen(path) - 1;
    க்கு (i = 0; i < p_cspc; ++i)
    {
	போது (s > path)
	{
	   s--;
	   எனில் (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      நிறுத்து;
	}
    }
    எனில் ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    திரும்பு s;
}

/*
 * Called from cs_manage_matches().
 */
    நிலையான வெறுமை
cs_print_tags_priv(எழுத்து **matches, எழுத்து **cntxts, முழு இலக்கம்_matches)
{
    எழுத்து	*buf = வெற்று;
    எழுத்து	*t_buf;
    முழு		bufsize = 0; // Track available bufsize
    முழு		newsize = 0;
    எழுத்து	*ptag;
    எழுத்து	*fname, *lno, *extra, *tbuf;
    முழு		i, idx, இலக்கம்;
    எழுத்து	*globalcntx = "GLOBAL";
    எழுத்து	*cntxformat = " <<%s>>";
    எழுத்து	*context;
    எழுத்து	*cstag_செய்தி = _("Cscope tag: %s");
    எழுத்து	*csfmt_str = "%4d %6s  ";

    ப்ரதிஜ்ஞா(இலக்கம்_matches > 0);

    எனில் ((tbuf = alloc(strlen(matches[0]) + 1)) == வெற்று)
	திரும்பு;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    எனில் (ptag == வெற்று)
    {
	vim_free(tbuf);
	திரும்பு;
    }

    newsize = (முழு)(strlen(cstag_செய்தி) + strlen(ptag));
    buf = alloc(newsize);
    எனில் (buf != வெற்று)
    {
	bufsize = newsize;
	(வெறுமை)sprintf(buf, cstag_செய்தி, ptag);
	செய்தி_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    செய்தி_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    செய்தி_advance(செய்தி_col + 2);
    செய்தி_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    இலக்கம் = 1;
    க்கு (i = 0; i < இலக்கம்_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	எனில் ((tbuf = alloc(strlen(matches[idx]) + 1)) == வெற்று)
	    தொடர்;
	(வெறுமை)strcpy(tbuf, matches[idx]);

	எனில் (strtok(tbuf, (மாறிலி எழுத்து *)"\t") == வெற்று
		|| (fname = strtok(வெற்று, (மாறிலி எழுத்து *)"\t")) == வெற்று
		|| (lno = strtok(வெற்று, (மாறிலி எழுத்து *)"\t")) == வெற்று)
	{
	    vim_free(tbuf);
	    தொடர்;
	}
	extra = strtok(வெற்று, (மாறிலி எழுத்து *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (முழு)(strlen(csfmt_str) + 16 + strlen(lno));
	எனில் (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    எனில் (buf == வெற்று)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    இல்லையெனில்
		bufsize = newsize;
	}
	எனில் (buf != வெற்று)
	{
	    // csfmt_str = "%4d %6s  ";
	    (வெறுமை)sprintf(buf, csfmt_str, இலக்கம், lno);
	    செய்தி_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	செய்தி_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	எனில் (cntxts[idx] != வெற்று)
	    context = cntxts[idx];
	இல்லையெனில்
	    context = globalcntx;
	newsize = (முழு)(strlen(context) + strlen(cntxformat));

	எனில் (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    எனில் (buf == வெற்று)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    இல்லையெனில்
		bufsize = newsize;
	}
	எனில் (buf != வெற்று)
	{
	    (வெறுமை)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    எனில் (செய்தி_col + (முழு)strlen(buf) >= cmdline_width)
		செய்தி_putchar('\n');
	    செய்தி_advance(12);
	    செய்தி_outtrans_long_attr((char_u *)buf, 0);
	    செய்தி_putchar('\n');
	}
	எனில் (extra != வெற்று)
	{
	    செய்தி_advance(13);
	    செய்தி_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	எனில் (செய்தி_col)
	    செய்தி_putchar('\n');

	ui_breakcheck();
	எனில் (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    நிறுத்து;
	}

	இலக்கம்++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    நிலையான முழு
cs_read_prompt(முழு i)
{
    முழு		ch;
    எழுத்து	*buf = வெற்று; // buffer for possible error message from cscope
    முழு		bufpos = 0;
    முழு		maxlen;
    நிலையான எழுத்து *eprompt = "Press the RETURN key to continue:";
    முழு		epromptlen = (முழு)strlen(eprompt);
    முழு		n;

    // compute maximum allowed len for Cscope error message
    எழுத்து *cs_emsg = _(e_cscope_error_str);
    maxlen = (முழு)(IOSIZE - strlen(cs_emsg));

    க்கு (;;)
    {
	போது ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    எனில் (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		எனில் (buf == வெற்று) // lazy buffer allocation
		    buf = alloc(maxlen);
		எனில் (buf != வெற்று)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    எனில் (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(வெறுமை)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(வெறுமை)putc('\n', csinfo[i].to_fp);
			(வெறுமை)fflush(csinfo[i].to_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	க்கு (n = 0; n < (முழு)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    எனில் (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    எனில் (ch == EOF)
	    {
		எனில் (buf != வெற்று && buf[0] != NUL)
		    (வெறுமை)semsg(cs_emsg, buf);
		இல்லையெனில் எனில் (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		திரும்பு CSCOPE_FAILURE;
	    }

	    எனில் (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		நிறுத்து;
	    }
	}

	எனில் (ch == EOF)
	    தொடர்;	    // didn't find the prompt
	நிறுத்து;		    // did find the prompt
    }

    vim_free(buf);
    திரும்பு CSCOPE_SUCCESS;
}

# எனில் defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    நிலையான வெறுமை
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    நிலையான வெறுமை
cs_release_csp(முழு i, முழு freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    எனில் (csinfo[i].to_fp != வெற்று)
    {
	(வெறுமை)fputs("q\n", csinfo[i].to_fp);
	(வெறுமை)fflush(csinfo[i].to_fp);
    }
# எனில் defined(UNIX)
    {
	முழு waitpid_errno;
	முழு pstat;
	pid_t pid;

#  எனில் defined(HAVE_SIGACTION)
	கட்டமைப்பு sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   இல்லையெனில்
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, வெற்று);
#  இல்லையெனில்
	முழு waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	க்கு (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    எனில் (pid != 0)
		நிறுத்து;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	எனில் (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    முழு alive = TRUE;

	    எனில் (waitpid_errno == ECHILD)
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
		முழு waited;

		sleep(0);
		க்கு (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    எனில் (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			நிறுத்து;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    எனில் (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(வெறுமை)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# இல்லையெனில்  // !UNIX
    எனில் (csinfo[i].hProc != வெற்று)
    {
	// Give cscope a chance to exit normally
	எனில் (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    எனில் (csinfo[i].fr_fp != வெற்று)
	(வெறுமை)fclose(csinfo[i].fr_fp);
    எனில் (csinfo[i].to_fp != வெற்று)
	(வெறுமை)fclose(csinfo[i].to_fp);

    எனில் (freefnpp)
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
    நிலையான முழு
cs_reset(exarg_T *eap UNUSED)
{
    எழுத்து	**dblist = வெற்று, **pplist = வெற்று, **fllist = வெற்று;
    முழு	i;
    எழுத்து buf[20]; // for sprintf " (#%d)"

    எனில் (csinfo_size == 0)
	திரும்பு CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(எழுத்து *, csinfo_size);
    pplist = ALLOC_MULT(எழுத்து *, csinfo_size);
    fllist = ALLOC_MULT(எழுத்து *, csinfo_size);
    எனில் (dblist == வெற்று || pplist == வெற்று || fllist == வெற்று)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	திரும்பு CSCOPE_FAILURE;
    }

    க்கு (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	எனில் (csinfo[i].fname != வெற்று)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    க்கு (i = 0; i < csinfo_size; i++)
    {
	எனில் (dblist[i] != வெற்று)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    எனில் (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		செய்தி_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    எனில் (p_csverbose)
	செய்தி_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | செய்தி_HIST);
    திரும்பு CSCOPE_SUCCESS;
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
    நிலையான எழுத்து *
cs_resolve_file(முழு i, எழுத்து *name)
{
    எழுத்து	*fullname;
    string_T	csdir = {வெற்று, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    முழு		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (முழு)namelen + 2;
    எனில் (csinfo[i].ppath != வெற்று)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (முழு)ppathlen;
    }
    இல்லையெனில் எனில் (p_csre && csinfo[i].fname != வெற்று)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	எனில் (csdir.string != வெற்று)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (முழு)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    எனில் (csinfo[i].ppath != வெற்று
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	எனில் ((fullname = alloc(len)) != வெற்று)
	    (வெறுமை)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    இல்லையெனில் எனில் (csdir.string != வெற்று && csinfo[i].fname != வெற்று && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (எழுத்து *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    இல்லையெனில்
    {
	fullname = (எழுத்து *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    திரும்பு fullname;
}


/*
 * Show all cscope connections.
 */
    நிலையான முழு
cs_show(exarg_T *eap UNUSED)
{
    முழு i;

    எனில் (cs_cnt_connections() == 0)
	செய்தி_puts(_("no cscope connections\n"));
    இல்லையெனில்
    {
	செய்தி_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	க்கு (i = 0; i < csinfo_size; i++)
	{
	    எனில் (csinfo[i].fname == வெற்று)
		தொடர்;

	    எனில் (csinfo[i].ppath != வெற்று)
		(வெறுமை)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    இல்லையெனில்
		(வெறுமை)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    திரும்பு CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    வெறுமை
cs_end(வெறுமை)
{
    முழு i;

    க்கு (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#எனில் defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    வெறுமை
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    முழு		இலக்கம் = 0;
    char_u	*dbpath = வெற்று;
    char_u	*prepend = வெற்று;
    char_u	buf[NUMBUFLEN];

    எனில் (in_vim9script()
	    && (check_for_opt_எண்_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	இலக்கம் = (முழு)tv_get_எண்(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_எண் = cs_connection(இலக்கம், dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
