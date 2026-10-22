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

#include "vim.h"

#ຖ້າ defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# ຖ້າ defined(UNIX)
#  include <sys/wait.h>
# endif

# ຖ້າ defined (MSWIN)
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

typedef sa_nracanaa {
    var_na *  name;
    ຈຳນວນເຕັມ     (*func)(exarg_T *eap);
    var_na *  help;
    var_na *  usage;
    ຈຳນວນເຕັມ	    cansplit;		// if supports splitting window
} cscmd_T;

typedef sa_nracanaa csi {
    var_na *	    fname;	// cscope db name
    var_na *	    ppath;	// path to prepend (the -P option)
    var_na *	    flags;	// additional cscope flags/options (e.g, -p2)
# ຖ້າ defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# ບໍ່ດັ່ງນັ້ນ
#  ຖ້າ defined(MSWIN)
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

typedef ga_nanaa { Add, Find, Help, Kill, Reset, Show } csid_e;

typedef ga_nanaa {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

static ຈຳນວນເຕັມ	    cs_add(exarg_T *eap);
static ຈຳນວນເຕັມ	    cs_add_common(var_na *, var_na *, var_na *);
static ຈຳນວນເຕັມ	    cs_check_for_connections(void);
static ຈຳນວນເຕັມ	    cs_check_for_tags(void);
static ຈຳນວນເຕັມ	    cs_cnt_connections(void);
static ຈຳນວນເຕັມ	    cs_create_connection(ຈຳນວນເຕັມ i);
# ifdef FEAT_QUICKFIX
static void	    cs_file_results(FILE *, ຈຳນວນເຕັມ *);
# endif
static void	    cs_fill_results(var_na *, ຈຳນວນເຕັມ , ຈຳນວນເຕັມ *, var_na ***,
			var_na ***, ຈຳນວນເຕັມ *);
static ຈຳນວນເຕັມ	    cs_find(exarg_T *eap);
static ຈຳນວນເຕັມ	    cs_find_common(var_na *opt, var_na *pat, ຈຳນວນເຕັມ, ຈຳນວນເຕັມ, ຈຳນວນເຕັມ, char_u *cmdline);
static ຈຳນວນເຕັມ	    cs_help(exarg_T *eap);
static ຈຳນວນເຕັມ	    cs_insert_filelist(var_na *, var_na *, var_na *,
			stat_T *);
static ຈຳນວນເຕັມ	    cs_kill(exarg_T *eap);
static void	    cs_kill_execute(ຈຳນວນເຕັມ, var_na *);
static cscmd_T *    cs_lookup_cmd(exarg_T *eap);
static var_na *	    cs_make_vim_style_matches(var_na *, var_na *,
			var_na *, var_na *);
static var_na *	    cs_manage_matches(var_na **, var_na **, ຈຳນວນເຕັມ, mcmd_e);
static void	    cs_print_tags_priv(var_na **, var_na **, ຈຳນວນເຕັມ);
static ຈຳນວນເຕັມ	    cs_read_prompt(ຈຳນວນເຕັມ);
static void	    cs_release_csp(ຈຳນວນເຕັມ, ຈຳນວນເຕັມ freefnpp);
static ຈຳນວນເຕັມ	    cs_reset(exarg_T *eap);
static var_na *	    cs_resolve_file(ຈຳນວນເຕັມ, var_na *);
static ຈຳນວນເຕັມ	    cs_show(exarg_T *eap);


static csinfo_T *   csinfo = NULL;
static ຈຳນວນເຕັມ	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

static ຈຳນວນເຕັມ	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
static cscmd_T	    cs_cmds[] =
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
    { NULL, NULL, NULL, NULL, 0 }
};

    static void
cs_usage_msg(csid_e x)
{
    (void)semsg(_(e_usage_cscope_str), cs_cmds[(ຈຳນວນເຕັມ)x].usage);
}

static ga_nanaa
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
get_cscope_name(expand_T *xp UNUSED, ຈຳນວນເຕັມ idx)
{
    ຈຳນວນເຕັມ		current_idx;
    ຈຳນວນເຕັມ		i;

    switch (expand_what)
    {
    pak_sa EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	ສົ່ງຄືນ (char_u *)cs_cmds[idx].name;
    pak_sa EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	ສຳລັບ (i = 0, current_idx = 0; cs_cmds[i].name != NULL; i++)
	    ຖ້າ (cs_cmds[i].cansplit)
		ຖ້າ (current_idx++ == idx)
		    bha_nga;
	ສົ່ງຄືນ (char_u *)cs_cmds[i].name;
    pak_sa EXP_CSCOPE_FIND:
	{
	    const var_na *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", NULL
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    ສົ່ງຄືນ (char_u *)query_type[idx];
	}
    pak_sa EXP_CSCOPE_KILL:
	{
	    static var_na	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    ສຳລັບ (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		ຖ້າ (csinfo[i].fname == NULL)
		    continue;
		ຖ້າ (current_idx++ == idx)
		{
		    vim_snprintf(connection, parimaa_na(connection), "%d", i);
		    ສົ່ງຄືນ (char_u *)connection;
		}
	    }
	    ສົ່ງຄືນ (current_idx == idx && idx > 0) ? (char_u *)"-1" : NULL;
	}
    default:
	ສົ່ງຄືນ NULL;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    void
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

    ຖ້າ (*arg == NUL)
	ສົ່ງຄືນ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ຖ້າ (*p == NUL)
	ສົ່ງຄືນ;

    // past first word
    xp->xp_pattern = skipwhite(p);
    ຖ້າ (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    ບໍ່ດັ່ງນັ້ນ
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    static void
do_cscope_general(
    exarg_T	*eap,
    ຈຳນວນເຕັມ		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    ຖ້າ (check_restricted())
	ສົ່ງຄືນ;

    ຖ້າ ((cmdp = cs_lookup_cmd(eap)) == NULL)
    {
	cs_help(eap);
	ສົ່ງຄືນ;
    }

    ຖ້າ (make_split)
    {
	ຖ້າ (!cmdp->cansplit)
	{
	    (void)msg_puts(_("This cscope command does not support splitting the window.\n"));
	    ສົ່ງຄືນ;
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
    void
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    void
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    void
ex_cstag(exarg_T *eap)
{
    ຈຳນວນເຕັມ ret = FALSE;

    ຖ້າ (*eap->arg == NUL)
    {
	(void)emsg(_(e_usage_cstag_ident));
	ສົ່ງຄືນ;
    }

    switch (p_csto)
    {
    pak_sa 0 :
	ຖ້າ (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (var_na *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ຖ້າ (ret == FALSE)
	    {
		cs_free_tags();
		ຖ້າ (msg_col)
		    msg_putchar('\n');

		ຖ້າ (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	bha_nga;
    pak_sa 1 :
	ຖ້າ (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    ຖ້າ (ret == FALSE)
	    {
		ຖ້າ (msg_col)
		    msg_putchar('\n');

		ຖ້າ (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (var_na *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    ຖ້າ (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (var_na *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ຖ້າ (ret == FALSE)
		cs_free_tags();
	}
	bha_nga;
    default :
	bha_nga;
    }

    ຖ້າ (!ret)
    {
	(void)emsg(_(e_cstag_tag_not_founc));
# ຖ້າ defined(FEAT_QUICKFIX)
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
    ຈຳນວນເຕັມ
cs_fgets(char_u *buf, ຈຳນວນເຕັມ size)
{
    var_na *p;

    ຖ້າ ((p = cs_manage_matches(NULL, NULL, -1, Get)) == NULL)
	ສົ່ງຄືນ TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    ສົ່ງຄືນ FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    void
cs_free_tags(void)
{
    cs_manage_matches(NULL, NULL, -1, Free);
}


/*
 * Called from do_tag().
 */
    void
cs_print_tags(void)
{
    cs_manage_matches(NULL, NULL, -1, Print);
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
# ຖ້າ defined(FEAT_EVAL)
    static ຈຳນວນເຕັມ
cs_connection(ຈຳນວນເຕັມ num, char_u *dbpath, char_u *ppath)
{
    ຈຳນວນເຕັມ i;

    ຖ້າ (num < 0 || num > 4 || (num > 0 && !dbpath))
	ສົ່ງຄືນ FALSE;

    ສຳລັບ (i = 0; i < csinfo_size; i++)
    {
	ຖ້າ (!csinfo[i].fname)
	    continue;

	ຖ້າ (num == 0)
	    ສົ່ງຄືນ TRUE;

	switch (num)
	{
	pak_sa 1:
	    ຖ້າ (strstr(csinfo[i].fname, (var_na *)dbpath))
		ສົ່ງຄືນ TRUE;
	    bha_nga;
	pak_sa 2:
	    ຖ້າ (strcmp(csinfo[i].fname, (var_na *)dbpath) == 0)
		ສົ່ງຄືນ TRUE;
	    bha_nga;
	pak_sa 3:
	    ຖ້າ (strstr(csinfo[i].fname, (var_na *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (var_na *)ppath))))
		ສົ່ງຄືນ TRUE;
	    bha_nga;
	pak_sa 4:
	    ຖ້າ ((strcmp(csinfo[i].fname, (var_na *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (var_na *)ppath) == 0))))
		ສົ່ງຄືນ TRUE;
	    bha_nga;
	}
    }

    ສົ່ງຄືນ FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    static ຈຳນວນເຕັມ
cs_add(exarg_T *eap UNUSED)
{
    var_na *fname, *ppath, *flags = NULL;

    ຖ້າ ((fname = strtok((var_na *)NULL, (const var_na *)" ")) == NULL)
    {
	cs_usage_msg(Add);
	ສົ່ງຄືນ CSCOPE_FAILURE;
    }
    ຖ້າ ((ppath = strtok((var_na *)NULL, (const var_na *)" ")) != NULL)
	flags = strtok((var_na *)NULL, (const var_na *)" ");

    ສົ່ງຄືນ cs_add_common(fname, ppath, flags);
}

    static void
cs_stat_emsg(var_na *fname)
{
    ຈຳນວນເຕັມ err = errno;
    (void)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    static ຈຳນວນເຕັມ
cs_add_common(
    var_na *arg1,	    // filename - may contain environment variables
    var_na *arg2,	    // prepend path - may contain environment variables
    var_na *flags)
{
    stat_T	statbuf;
    ຈຳນວນເຕັມ		ret;
    var_na	*fname = NULL;
    var_na	*fname2 = NULL;
    var_na	*ppath = NULL;
    ຈຳນວນເຕັມ		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = NULL;

    // get the filename (arg1), expand it, and try to stat it
    ຖ້າ ((fname = alloc(MAXPATHL + 1)) == NULL)
	goto add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (void)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    ຖ້າ (fname == NULL)
	goto add_err;
    fname = (var_na *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    ຖ້າ (ret < 0)
    {
staterr:
	ຖ້າ (p_csverbose)
	    cs_stat_emsg(fname);
	goto add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    ຖ້າ (arg2 != NULL)
    {
	stat_T	    statbuf2;

	ຖ້າ ((ppath = alloc(MAXPATHL + 1)) == NULL)
	    goto add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	ຖ້າ (ret < 0)
	    goto staterr;
    }

    // if filename is a directory, append the cscope database name to it
    ຖ້າ (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	ຖ້າ (fname2 == NULL)
	    goto add_err;

	ໃນຂະນະທີ່ (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    ຖ້າ (fname[0] == '\0')
		bha_nga;
	}
	ຖ້າ (fname[0] == '\0')
	    (void)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	ບໍ່ດັ່ງນັ້ນ
	    (void)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	ຖ້າ (ret < 0)
	{
	    ຖ້າ (p_csverbose)
		cs_stat_emsg(fname2);
	    goto add_err;
	}

	i = cs_insert_filelist(fname2, ppath, flags, &statbuf);
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_insert_filelist(fname, ppath, flags, &statbuf);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (p_csverbose)
	    (void)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	goto add_err;
    }

    ຖ້າ (i != -1)
    {
	ຖ້າ (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    goto add_err;
	}

	ຖ້າ (p_csverbose)
	{
	    msg_clr_eos();
	    (void)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    ສົ່ງຄືນ CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    ສົ່ງຄືນ CSCOPE_FAILURE;
}


    static ຈຳນວນເຕັມ
cs_check_for_connections(void)
{
    ສົ່ງຄືນ (cs_cnt_connections() > 0);
}


    static ຈຳນວນເຕັມ
cs_check_for_tags(void)
{
    ສົ່ງຄືນ (p_tags[0] != NUL && curbuf->b_p_tags != NULL);
}


/*
 * Count the number of cscope connections.
 */
    static ຈຳນວນເຕັມ
cs_cnt_connections(void)
{
    ຈຳນວນເຕັມ i;
    ຈຳນວນເຕັມ cnt = 0;

    ສຳລັບ (i = 0; i < csinfo_size; i++)
	ຖ້າ (csinfo[i].fname != NULL)
	    cnt++;
    ສົ່ງຄືນ cnt;
}

    static void
cs_reading_emsg(
    ຈຳນວນເຕັມ idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    static ຈຳນວນເຕັມ
cs_cnt_matches(ຈຳນວນເຕັມ idx)
{
    var_na *stok;
    var_na *buf;
    ຈຳນວນເຕັມ nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    ຖ້າ (buf == NULL)
	ສົ່ງຄືນ 0;
    ສຳລັບ (;;)
    {
	ຖ້າ (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    ຖ້າ (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    ສົ່ງຄືນ -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	ຖ້າ (strstr((const var_na *)buf, "Unable to search database") != NULL)
	    bha_nga;
	ຖ້າ ((stok = strtok(buf, (const var_na *)" ")) == NULL)
	    continue;
	ຖ້າ (strstr((const var_na *)stok, "cscope:") == NULL)
	    continue;

	ຖ້າ ((stok = strtok(NULL, (const var_na *)" ")) == NULL)
	    continue;
	nlines = atoi(stok);
	ຖ້າ (nlines < 0)
	{
	    nlines = 0;
	    bha_nga;
	}

	ຖ້າ ((stok = strtok(NULL, (const var_na *)" ")) == NULL)
	    continue;
	ຖ້າ (strncmp((const var_na *)stok, "lines", 5))
	    continue;

	bha_nga;
    }

    vim_free(buf);
    ສົ່ງຄືນ nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    static var_na *
cs_create_cmd(var_na *csoption, var_na *pattern)
{
    var_na *cmd;
    short search;
    var_na *pat;

    switch (csoption[0])
    {
    pak_sa '0' : pak_sa 's' :
	search = 0;
	bha_nga;
    pak_sa '1' : pak_sa 'g' :
	search = 1;
	bha_nga;
    pak_sa '2' : pak_sa 'd' :
	search = 2;
	bha_nga;
    pak_sa '3' : pak_sa 'c' :
	search = 3;
	bha_nga;
    pak_sa '4' : pak_sa 't' :
	search = 4;
	bha_nga;
    pak_sa '6' : pak_sa 'e' :
	search = 6;
	bha_nga;
    pak_sa '7' : pak_sa 'f' :
	search = 7;
	bha_nga;
    pak_sa '8' : pak_sa 'i' :
	search = 8;
	bha_nga;
    pak_sa '9' : pak_sa 'a' :
	search = 9;
	bha_nga;
    default :
	(void)emsg(_(e_unknown_cscope_search_type));
	cs_usage_msg(Find);
	ສົ່ງຄືນ NULL;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    ຖ້າ (search != 4 && search != 6)
	ໃນຂະນະທີ່ VIM_ISWHITE(*pat)
	    ++pat;

    ຖ້າ ((cmd = alloc(strlen(pat) + 2)) == NULL)
	ສົ່ງຄືນ NULL;

    (void)sprintf(cmd, "%d%s", search, pat);

    ສົ່ງຄືນ cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    static ຈຳນວນເຕັມ
cs_create_connection(ຈຳນວນເຕັມ i)
{
# ifdef UNIX
    ຈຳນວນເຕັມ		to_cs[2], from_cs[2];
# endif
    ຈຳນວນເຕັມ		cmdlen;
    var_na	*prog, *cmd, *ppath = NULL;
    size_t	proglen;
# ifdef MSWIN
    ຈຳນວນເຕັມ		fd;
    ຈຳນວນເຕັມ		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# ຖ້າ defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    ຖ້າ (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(void)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	ຖ້າ (to_cs[0] != -1)
	    (void)close(to_cs[0]);
	ຖ້າ (to_cs[1] != -1)
	    (void)close(to_cs[1]);
	ຖ້າ (from_cs[0] != -1)
	    (void)close(from_cs[0]);
	ຖ້າ (from_cs[1] != -1)
	    (void)close(from_cs[1]);
	ສົ່ງຄືນ CSCOPE_FAILURE;
    }

    ຖ້າ ((csinfo[i].pid = fork()) == -1)
    {
	(void)emsg(_(e_could_not_fork_for_cscope));
	goto err_closing;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (csinfo[i].pid == 0)	// child: run cscope.
    {
	var_na **argv = NULL;

	ຖ້າ (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	ຖ້າ (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	ຖ້າ (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(void)close(to_cs[1]);
	(void)close(from_cs[0]);
# ບໍ່ດັ່ງນັ້ນ
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = parimaa_na(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	ຖ້າ (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (void)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    ຖ້າ (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    ຖ້າ (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    ສົ່ງຄືນ CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	ຖ້າ ((prog = alloc(MAXPATHL + 1)) == NULL)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# ບໍ່ດັ່ງນັ້ນ
	    // MSWIN
	    goto err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (ຈຳນວນເຕັມ)(proglen + strlen(csinfo[i].fname) + 32);
	ຖ້າ (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    ຖ້າ ((ppath = alloc(MAXPATHL + 1)) == NULL)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# ບໍ່ດັ່ງນັ້ນ
		// MSWIN
		goto err_closing;
# endif
	    }
	    cmdlen += (ຈຳນວນເຕັມ)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	ຖ້າ (csinfo[i].flags)
	    cmdlen += (ຈຳນວນເຕັມ)strlen(csinfo[i].flags);

	ຖ້າ ((cmd = alloc(cmdlen)) == NULL)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# ບໍ່ດັ່ງນັ້ນ
	    // MSWIN
	    goto err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    var_na	**tok = NULL;
	    ຈຳນວນເຕັມ		tokc = 0;

	    ga_init2(&ga_argv, parimaa_na(var_na *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    ຖ້າ (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    ສຳລັບ (ຈຳນວນເຕັມ k = 0; k < tokc; ++k)
	    {
		ຖ້າ (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((var_na **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    ຖ້າ (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((var_na **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (var_na *)vim_strsave((char_u *)"-dl");
	    ((var_na **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (var_na *)vim_strsave((char_u *)"-f");
	    ((var_na **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(var_na *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    ຖ້າ (ppath != NULL)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		var_na	*parg = alloc(plen);

		ຖ້າ (parg == NULL || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((var_na **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    ຖ້າ (csinfo[i].flags != NULL)
	    {
		ຖ້າ (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		ສຳລັບ (ຈຳນວນເຕັມ k = 0; k < tokc; ++k)
		{
		    ຖ້າ (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((var_na **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    ຖ້າ (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((var_na **)ga_argv.ga_data)[ga_argv.ga_len] = NULL;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  ຖ້າ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   ຖ້າ defined(HAVE_SETSID)
	    (void)setsid();
#   ບໍ່ດັ່ງນັ້ນ
	    ຖ້າ (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (var_na **)ga_argv.ga_data;
	    ຖ້າ (argv[0] == NULL || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    ບໍ່ດັ່ງນັ້ນ	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	ຖ້າ ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == NULL)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	ຖ້າ ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == NULL)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(void)close(to_cs[0]);
	(void)close(from_cs[1]);
    }
# ບໍ່ດັ່ງນັ້ນ
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    ຖ້າ (csinfo[i].ppath != NULL)
    {
	len = (ຈຳນວນເຕັມ)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    ຖ້າ (csinfo[i].flags != NULL)
    {
	len = (ຈຳນວນເຕັມ)STRLEN(cmd);
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
    created = CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NEW_CONSOLE,
							NULL, NULL, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    ຖ້າ (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(void)emsg(_(e_could_not_spawn_cscope_process));
	goto err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    ຖ້າ (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == NULL))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    ຖ້າ (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == NULL))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    ສົ່ງຄືນ CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    static ຈຳນວນເຕັມ
cs_find(exarg_T *eap)
{
    var_na *opt, *pat;
    ຈຳນວນເຕັມ i;

    ຖ້າ (cs_check_for_connections() == FALSE)
    {
	(void)emsg(_(e_no_cscope_connections));
	ສົ່ງຄືນ FALSE;
    }

    ຖ້າ ((opt = strtok((var_na *)NULL, (const var_na *)" ")) == NULL)
    {
	cs_usage_msg(Find);
	ສົ່ງຄືນ FALSE;
    }

    pat = opt + strlen(opt) + 1;
    ຖ້າ (pat >= (var_na *)eap->arg + eap_arg_len)
    {
	cs_usage_msg(Find);
	ສົ່ງຄືນ FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    ສຳລັບ (i = 0; i < eap_arg_len; ++i)
	ຖ້າ (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    ສົ່ງຄືນ cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    static ຈຳນວນເຕັມ
cs_find_common(
    var_na *opt,
    var_na *pat,
    ຈຳນວນເຕັມ forceit,
    ຈຳນວນເຕັມ verbose,
    ຈຳນວນເຕັມ	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    ຈຳນວນເຕັມ i;
    var_na *cmd;
    ຈຳນວນເຕັມ *nummatches;
    ຈຳນວນເຕັມ totmatches;
# ifdef FEAT_QUICKFIX
    var_na cmdletter;
    var_na *qfpos;

    // get cmd letter
    switch (opt[0])
    {
    pak_sa '0' :
	cmdletter = 's';
	bha_nga;
    pak_sa '1' :
	cmdletter = 'g';
	bha_nga;
    pak_sa '2' :
	cmdletter = 'd';
	bha_nga;
    pak_sa '3' :
	cmdletter = 'c';
	bha_nga;
    pak_sa '4' :
	cmdletter = 't';
	bha_nga;
    pak_sa '6' :
	cmdletter = 'e';
	bha_nga;
    pak_sa '7' :
	cmdletter = 'f';
	bha_nga;
    pak_sa '8' :
	cmdletter = 'i';
	bha_nga;
    pak_sa '9' :
	cmdletter = 'a';
	bha_nga;
    default :
	cmdletter = opt[0];
    }

    qfpos = (var_na *)vim_strchr(p_csqf, cmdletter);
    ຖ້າ (qfpos != NULL)
    {
	qfpos++;
	// next symbol must be + or -
	ຖ້າ (strchr(CSQF_FLAGS, *qfpos) == NULL)
	{
	    (void)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    ສົ່ງຄືນ FALSE;
	}

	ຖ້າ (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    ຖ້າ (aborting())
		ສົ່ງຄືນ FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    ຖ້າ (cmd == NULL)
	ສົ່ງຄືນ FALSE;

    nummatches = ALLOC_MULT(ຈຳນວນເຕັມ, csinfo_size);
    ຖ້າ (nummatches == NULL)
    {
	vim_free(cmd);
	ສົ່ງຄືນ FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    ສຳລັບ (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    ສຳລັບ (i = 0; i < csinfo_size; i++)
    {
	ຖ້າ (csinfo[i].fname == NULL || csinfo[i].to_fp == NULL)
	    continue;

	// send cmd to cscope
	(void)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(void)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	ຖ້າ (nummatches[i] > -1)
	    totmatches += nummatches[i];

	ຖ້າ (nummatches[i] == 0)
	    (void)cs_read_prompt(i);
    }
    vim_free(cmd);

    ຖ້າ (totmatches == 0)
    {
	ຖ້າ (verbose)
	    (void)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	ສົ່ງຄືນ FALSE;
    }

# ifdef FEAT_QUICKFIX
    ຖ້າ (qfpos != NULL && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = NULL;
	win_T	    *wp = NULL;

	ຖ້າ (tmp == NULL)
	{
	    vim_free(nummatches);
	    ສົ່ງຄືນ FALSE;
	}

	f = mch_fopen((var_na *)tmp, "w");
	ຖ້າ (f == NULL)
	    semsg(_(e_cant_open_file_str), tmp);
	ບໍ່ດັ່ງນັ້ນ
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    ຖ້າ (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    ຖ້າ (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, NULL) > 0)
	    {
		ຖ້າ (postponed_split != 0)
		{
		    (void)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		ຖ້າ (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != NULL)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	ສົ່ງຄືນ TRUE;
    }
    ບໍ່ດັ່ງນັ້ນ
# endif // FEAT_QUICKFIX
    {
	var_na **matches = NULL, **contexts = NULL;
	ຈຳນວນເຕັມ matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	ຖ້າ (matches == NULL)
	    ສົ່ງຄືນ FALSE;

	(void)cs_manage_matches(matches, contexts, matched, Store);

	ສົ່ງຄືນ do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    static ຈຳນວນເຕັມ
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (void)msg_puts(_("cscope commands:\n"));
    ໃນຂະນະທີ່ (cmdp->name != NULL)
    {
	var_na *help = _(cmdp->help);
	ຈຳນວນເຕັມ  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	ຖ້າ (space_cnt < 0)
	    space_cnt = 0;
	(void)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	ຖ້າ (strcmp(cmdp->name, "find") == 0)
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
    ສົ່ງຄືນ 0;
}


    static void
clear_csinfo(ຈຳນວນເຕັມ i)
{
    csinfo[i].fname  = NULL;
    csinfo[i].ppath  = NULL;
    csinfo[i].flags  = NULL;
# ຖ້າ defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# ບໍ່ດັ່ງນັ້ນ
    csinfo[i].nVolume = 0;
    csinfo[i].nIndexHigh = 0;
    csinfo[i].nIndexLow = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = NULL;
    csinfo[i].to_fp  = NULL;
# ຖ້າ defined(MSWIN)
    csinfo[i].hProc = NULL;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    static ຈຳນວນເຕັມ
cs_insert_filelist(
    var_na *fname,
    var_na *ppath,
    var_na *flags,
    stat_T *sb UNUSED)
{
    ຈຳນວນເຕັມ	    i;
    ຈຳນວນເຕັມ	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    switch (win32_fileinfo((char_u *)fname, &bhfi))
    {
	pak_sa FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	pak_sa FILEINFO_READ_FAIL:	// CreateFile() failed
	    ຖ້າ (p_csverbose)
	    {
		var_na *cant_msg = _(e_cannot_open_cscope_database_str);
		var_na *winmsg = GetWin32Error();

		ຖ້າ (winmsg != NULL)
		    (void)semsg(cant_msg, winmsg);
		ບໍ່ດັ່ງນັ້ນ
		    // subst filename if can't get error text
		    (void)semsg(cant_msg, fname);
	    }
	    ສົ່ງຄືນ -1;

	pak_sa FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    ຖ້າ (p_csverbose)
		(void)emsg(_(e_cannot_get_cscope_database_information));
	    ສົ່ງຄືນ -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    ສຳລັບ (j = 0; j < csinfo_size; j++)
    {
	ຖ້າ (csinfo[j].fname != NULL
# ຖ້າ defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# ບໍ່ດັ່ງນັ້ນ
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
	    ຖ້າ (p_csverbose)
		(void)emsg(_(e_duplicate_cscope_database_not_added));
	    ສົ່ງຄືນ -1;
	}

	ຖ້າ (csinfo[j].fname == NULL && i == -1)
	    i = j; // remember first empty entry
    }

    ຖ້າ (i == -1)
    {
	i = csinfo_size;
	ຖ້າ (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, parimaa_na(csinfo_T)*csinfo_size);
	    ຖ້າ (csinfo == NULL)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		ສົ່ງຄືນ -1;
	    }
	}
	ຖ້າ (csinfo == NULL)
	    ສົ່ງຄືນ -1;
	ສຳລັບ (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    ຖ້າ ((csinfo[i].fname = alloc(strlen(fname)+1)) == NULL)
	ສົ່ງຄືນ -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((const var_na *)fname));

    ຖ້າ (ppath != NULL)
    {
	ຖ້າ ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == NULL)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    ສົ່ງຄືນ -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((const var_na *)ppath));
    }
    ບໍ່ດັ່ງນັ້ນ
	csinfo[i].ppath = NULL;

    ຖ້າ (flags != NULL)
    {
	ຖ້າ ((csinfo[i].flags = alloc(strlen(flags) + 1)) == NULL)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    ສົ່ງຄືນ -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((const var_na *)flags));
    }
    ບໍ່ດັ່ງນັ້ນ
	csinfo[i].flags = NULL;

# ຖ້າ defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# ບໍ່ດັ່ງນັ້ນ
    csinfo[i].nVolume = bhfi.dwVolumeSerialNumber;
    csinfo[i].nIndexLow = bhfi.nFileIndexLow;
    csinfo[i].nIndexHigh = bhfi.nFileIndexHigh;
# endif
    ສົ່ງຄືນ i;
}


/*
 * Find cscope command in command table.
 */
    static cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    var_na *stok;
    size_t len;

    ຖ້າ (eap->arg == NULL)
	ສົ່ງຄືນ NULL;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (ຈຳນວນເຕັມ)STRLEN(eap->arg);

    ຖ້າ ((stok = strtok((var_na *)(eap->arg), (const var_na *)" ")) == NULL)
	ສົ່ງຄືນ NULL;

    len = strlen(stok);
    ສຳລັບ (cmdp = cs_cmds; cmdp->name != NULL; ++cmdp)
    {
	ຖ້າ (strncmp((const var_na *)(stok), cmdp->name, len) == 0)
	    ສົ່ງຄືນ (cmdp);
    }
    ສົ່ງຄືນ NULL;
}


/*
 * Nuke em.
 */
    static ຈຳນວນເຕັມ
cs_kill(exarg_T *eap UNUSED)
{
    var_na *stok;
    ຈຳນວນເຕັມ i;

    ຖ້າ ((stok = strtok((var_na *)NULL, (const var_na *)" ")) == NULL)
    {
	cs_usage_msg(Kill);
	ສົ່ງຄືນ CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    ຖ້າ ((strlen(stok) < 2 && VIM_ISDIGIT((ຈຳນວນເຕັມ)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((ຈຳນວນເຕັມ)(stok[1]))))
	i = atoi(stok);
    ບໍ່ດັ່ງນັ້ນ
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	ສຳລັບ (i = 0; i < csinfo_size; i++)
	{
	    ຖ້າ (csinfo[i].fname != NULL && strstr(csinfo[i].fname, stok))
		bha_nga;
	}
    }

    ຖ້າ ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == NULL))
    {
	ຖ້າ (p_csverbose)
	    (void)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (i == -1)
	{
	    ສຳລັບ (i = 0; i < csinfo_size; i++)
	    {
		ຖ້າ (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	    cs_kill_execute(i, stok);
    }

    ສົ່ງຄືນ 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    static void
cs_kill_execute(
    ຈຳນວນເຕັມ i,	    // cscope table index
    var_na *cname)    // cscope database name
{
    ຖ້າ (p_csverbose)
    {
	msg_clr_eos();
	(void)smsg_attr(HL_ATTR(HLF_R) | MSG_HIST,
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
    static var_na *
cs_make_vim_style_matches(
    var_na *fname,
    var_na *slno,
    var_na *search,
    var_na *tagstr)
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
    var_na *buf;
    ຈຳນວນເຕັມ amt;

    ຖ້າ (search != NULL)
    {
	amt = (ຈຳນວນເຕັມ)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(search)+6);
	ຖ້າ ((buf = alloc(amt)) == NULL)
	    ສົ່ງຄືນ NULL;

	(void)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, search);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	amt = (ຈຳນວນເຕັມ)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	ຖ້າ ((buf = alloc(amt)) == NULL)
	    ສົ່ງຄືນ NULL;

	(void)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    ສົ່ງຄືນ buf;
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
    static var_na *
cs_manage_matches(
    var_na **matches,
    var_na **contexts,
    ຈຳນວນເຕັມ totmatches,
    mcmd_e cmd)
{
    static var_na **mp = NULL;
    static var_na **cp = NULL;
    static ຈຳນວນເຕັມ cnt = -1;
    static ຈຳນວນເຕັມ next = -1;
    var_na *p = NULL;

    switch (cmd)
    {
    pak_sa Store:
	assert(matches != NULL);
	assert(totmatches > 0);
	ຖ້າ (mp != NULL || cp != NULL)
	    (void)cs_manage_matches(NULL, NULL, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	bha_nga;
    pak_sa Get:
	ຖ້າ (next >= cnt)
	    ສົ່ງຄືນ NULL;

	p = mp[next];
	next++;
	bha_nga;
    pak_sa Free:
	ຖ້າ (mp != NULL)
	{
	    ຖ້າ (cnt > 0)
		ໃນຂະນະທີ່ (cnt--)
		{
		    vim_free(mp[cnt]);
		    ຖ້າ (cp != NULL)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = NULL;
	cp = NULL;
	cnt = 0;
	next = 0;
	bha_nga;
    pak_sa Print:
	cs_print_tags_priv(mp, cp, cnt);
	bha_nga;
    default:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	ສົ່ງຄືນ NULL;
    }

    ສົ່ງຄືນ p;
}


/*
 * Parse cscope output.
 */
    static var_na *
cs_parse_results(
    ຈຳນວນເຕັມ cnumber,
    var_na *buf,
    ຈຳນວນເຕັມ bufsize,
    var_na **context,
    var_na **linenumber,
    var_na **search)
{
    ຈຳນວນເຕັມ ch;
    var_na *p;
    var_na *name;

    ຖ້າ (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == NULL)
    {
	ຖ້າ (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	ສົ່ງຄືນ NULL;
    }

    // If the line's too long for the buffer, discard it.
    ຖ້າ ((p = strchr(buf, '\n')) == NULL)
    {
	ໃນຂະນະທີ່ ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	ສົ່ງຄືນ NULL;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    ຖ້າ ((name = strtok(buf, (const var_na *)" ")) == NULL)
	ສົ່ງຄືນ NULL;
    ຖ້າ ((*context = strtok(NULL, (const var_na *)" ")) == NULL)
	ສົ່ງຄືນ NULL;
    ຖ້າ ((*linenumber = strtok(NULL, (const var_na *)" ")) == NULL)
	ສົ່ງຄືນ NULL;
    *search = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    ຖ້າ (strcmp(*search, "<unknown>") == 0)
	*search = NULL;

    name = cs_resolve_file(cnumber, name);
    ສົ່ງຄືນ name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    static void
cs_file_results(FILE *f, ຈຳນວນເຕັມ *nummatches_a)
{
    ຈຳນວນເຕັມ i, j;
    var_na *buf;
    var_na *search, *slno;
    var_na *fullname;
    var_na *cntx;
    var_na *context;

    buf = alloc(CSREAD_BUFSIZE);
    ຖ້າ (buf == NULL)
	ສົ່ງຄືນ;

    ສຳລັບ (i = 0; i < csinfo_size; i++)
    {
	ຖ້າ (nummatches_a[i] < 1)
	    continue;

	ສຳລັບ (j = 0; j < nummatches_a[i]; j++)
	{
	   ຖ້າ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &search)) == NULL)
	       continue;

	   context = alloc(strlen(cntx)+5);
	   ຖ້າ (context == NULL)
	   {
	       vim_free(fullname);
	       continue;
	   }

	   ຖ້າ (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   ບໍ່ດັ່ງນັ້ນ
	       sprintf(context, "<<%s>>", cntx);

	   ຖ້າ (search == NULL)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   ບໍ່ດັ່ງນັ້ນ
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, search);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(void)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    static void
cs_fill_results(
    var_na *tagstr,
    ຈຳນວນເຕັມ totmatches,
    ຈຳນວນເຕັມ *nummatches_a,
    var_na ***matches_p,
    var_na ***cntxts_p,
    ຈຳນວນເຕັມ *matched)
{
    ຈຳນວນເຕັມ i, j;
    var_na *buf;
    var_na *search, *slno;
    ຈຳນວນເຕັມ totsofar = 0;
    var_na **matches = NULL;
    var_na **cntxts = NULL;
    var_na *fullname;
    var_na *cntx;

    assert(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    ຖ້າ (buf == NULL)
	ສົ່ງຄືນ;

    ຖ້າ ((matches = ALLOC_MULT(var_na *, totmatches)) == NULL)
	goto parse_out;
    ຖ້າ ((cntxts = ALLOC_MULT(var_na *, totmatches)) == NULL)
	goto parse_out;

    ສຳລັບ (i = 0; i < csinfo_size; i++)
    {
	ຖ້າ (nummatches_a[i] < 1)
	    continue;

	ສຳລັບ (j = 0; j < nummatches_a[i]; j++)
	{
	   ຖ້າ ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &search)) == NULL)
		continue;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  search, tagstr);

	    vim_free(fullname);

	    ຖ້າ (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = NULL;
	    ບໍ່ດັ່ງນັ້ນ
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (var_na *)vim_strsave((char_u *)cntx);

	    ຖ້າ (matches[totsofar] != NULL)
		totsofar++;

	} // for all matches

	(void)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    ຖ້າ (totsofar == 0)
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
    static var_na *
cs_pathcomponents(var_na *path)
{
    ຈຳນວນເຕັມ		i;
    var_na	*s;

    ຖ້າ (p_cspc == 0)
	ສົ່ງຄືນ path;

    s = path + strlen(path) - 1;
    ສຳລັບ (i = 0; i < p_cspc; ++i)
    {
	ໃນຂະນະທີ່ (s > path)
	{
	   s--;
	   ຖ້າ (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      bha_nga;
	}
    }
    ຖ້າ ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    ສົ່ງຄືນ s;
}

/*
 * Called from cs_manage_matches().
 */
    static void
cs_print_tags_priv(var_na **matches, var_na **cntxts, ຈຳນວນເຕັມ num_matches)
{
    var_na	*buf = NULL;
    var_na	*t_buf;
    ຈຳນວນເຕັມ		bufsize = 0; // Track available bufsize
    ຈຳນວນເຕັມ		newsize = 0;
    var_na	*ptag;
    var_na	*fname, *lno, *extra, *tbuf;
    ຈຳນວນເຕັມ		i, idx, num;
    var_na	*globalcntx = "GLOBAL";
    var_na	*cntxformat = " <<%s>>";
    var_na	*context;
    var_na	*cstag_msg = _("Cscope tag: %s");
    var_na	*csfmt_str = "%4d %6s  ";

    assert(num_matches > 0);

    ຖ້າ ((tbuf = alloc(strlen(matches[0]) + 1)) == NULL)
	ສົ່ງຄືນ;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    ຖ້າ (ptag == NULL)
    {
	vim_free(tbuf);
	ສົ່ງຄືນ;
    }

    newsize = (ຈຳນວນເຕັມ)(strlen(cstag_msg) + strlen(ptag));
    buf = alloc(newsize);
    ຖ້າ (buf != NULL)
    {
	bufsize = newsize;
	(void)sprintf(buf, cstag_msg, ptag);
	msg_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    msg_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    msg_advance(msg_col + 2);
    msg_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    num = 1;
    ສຳລັບ (i = 0; i < num_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	ຖ້າ ((tbuf = alloc(strlen(matches[idx]) + 1)) == NULL)
	    continue;
	(void)strcpy(tbuf, matches[idx]);

	ຖ້າ (strtok(tbuf, (const var_na *)"\t") == NULL
		|| (fname = strtok(NULL, (const var_na *)"\t")) == NULL
		|| (lno = strtok(NULL, (const var_na *)"\t")) == NULL)
	{
	    vim_free(tbuf);
	    continue;
	}
	extra = strtok(NULL, (const var_na *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (ຈຳນວນເຕັມ)(strlen(csfmt_str) + 16 + strlen(lno));
	ຖ້າ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ຖ້າ (buf == NULL)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ບໍ່ດັ່ງນັ້ນ
		bufsize = newsize;
	}
	ຖ້າ (buf != NULL)
	{
	    // csfmt_str = "%4d %6s  ";
	    (void)sprintf(buf, csfmt_str, num, lno);
	    msg_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	msg_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	ຖ້າ (cntxts[idx] != NULL)
	    context = cntxts[idx];
	ບໍ່ດັ່ງນັ້ນ
	    context = globalcntx;
	newsize = (ຈຳນວນເຕັມ)(strlen(context) + strlen(cntxformat));

	ຖ້າ (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ຖ້າ (buf == NULL)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ບໍ່ດັ່ງນັ້ນ
		bufsize = newsize;
	}
	ຖ້າ (buf != NULL)
	{
	    (void)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    ຖ້າ (msg_col + (ຈຳນວນເຕັມ)strlen(buf) >= cmdline_width)
		msg_putchar('\n');
	    msg_advance(12);
	    msg_outtrans_long_attr((char_u *)buf, 0);
	    msg_putchar('\n');
	}
	ຖ້າ (extra != NULL)
	{
	    msg_advance(13);
	    msg_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	ຖ້າ (msg_col)
	    msg_putchar('\n');

	ui_breakcheck();
	ຖ້າ (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    bha_nga;
	}

	num++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    static ຈຳນວນເຕັມ
cs_read_prompt(ຈຳນວນເຕັມ i)
{
    ຈຳນວນເຕັມ		ch;
    var_na	*buf = NULL; // buffer for possible error message from cscope
    ຈຳນວນເຕັມ		bufpos = 0;
    ຈຳນວນເຕັມ		maxlen;
    static var_na *eprompt = "Press the RETURN key to continue:";
    ຈຳນວນເຕັມ		epromptlen = (ຈຳນວນເຕັມ)strlen(eprompt);
    ຈຳນວນເຕັມ		n;

    // compute maximum allowed len for Cscope error message
    var_na *cs_emsg = _(e_cscope_error_str);
    maxlen = (ຈຳນວນເຕັມ)(IOSIZE - strlen(cs_emsg));

    ສຳລັບ (;;)
    {
	ໃນຂະນະທີ່ ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    ຖ້າ (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		ຖ້າ (buf == NULL) // lazy buffer allocation
		    buf = alloc(maxlen);
		ຖ້າ (buf != NULL)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    ຖ້າ (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(void)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(void)putc('\n', csinfo[i].to_fp);
			(void)fflush(csinfo[i].to_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	ສຳລັບ (n = 0; n < (ຈຳນວນເຕັມ)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    ຖ້າ (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    ຖ້າ (ch == EOF)
	    {
		ຖ້າ (buf != NULL && buf[0] != NUL)
		    (void)semsg(cs_emsg, buf);
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		ສົ່ງຄືນ CSCOPE_FAILURE;
	    }

	    ຖ້າ (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		bha_nga;
	    }
	}

	ຖ້າ (ch == EOF)
	    continue;	    // didn't find the prompt
	bha_nga;		    // did find the prompt
    }

    vim_free(buf);
    ສົ່ງຄືນ CSCOPE_SUCCESS;
}

# ຖ້າ defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    static void
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    static void
cs_release_csp(ຈຳນວນເຕັມ i, ຈຳນວນເຕັມ freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    ຖ້າ (csinfo[i].to_fp != NULL)
    {
	(void)fputs("q\n", csinfo[i].to_fp);
	(void)fflush(csinfo[i].to_fp);
    }
# ຖ້າ defined(UNIX)
    {
	ຈຳນວນເຕັມ waitpid_errno;
	ຈຳນວນເຕັມ pstat;
	pid_t pid;

#  ຖ້າ defined(HAVE_SIGACTION)
	sa_nracanaa sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   ບໍ່ດັ່ງນັ້ນ
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, NULL);
#  ບໍ່ດັ່ງນັ້ນ
	ຈຳນວນເຕັມ waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	ສຳລັບ (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    ຖ້າ (pid != 0)
		bha_nga;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	ຖ້າ (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    ຈຳນວນເຕັມ alive = TRUE;

	    ຖ້າ (waitpid_errno == ECHILD)
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
		ຈຳນວນເຕັມ waited;

		sleep(0);
		ສຳລັບ (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    ຖ້າ (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			bha_nga;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    ຖ້າ (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(void)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# ບໍ່ດັ່ງນັ້ນ  // !UNIX
    ຖ້າ (csinfo[i].hProc != NULL)
    {
	// Give cscope a chance to exit normally
	ຖ້າ (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    ຖ້າ (csinfo[i].fr_fp != NULL)
	(void)fclose(csinfo[i].fr_fp);
    ຖ້າ (csinfo[i].to_fp != NULL)
	(void)fclose(csinfo[i].to_fp);

    ຖ້າ (freefnpp)
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
    static ຈຳນວນເຕັມ
cs_reset(exarg_T *eap UNUSED)
{
    var_na	**dblist = NULL, **pplist = NULL, **fllist = NULL;
    ຈຳນວນເຕັມ	i;
    var_na buf[20]; // for sprintf " (#%d)"

    ຖ້າ (csinfo_size == 0)
	ສົ່ງຄືນ CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(var_na *, csinfo_size);
    pplist = ALLOC_MULT(var_na *, csinfo_size);
    fllist = ALLOC_MULT(var_na *, csinfo_size);
    ຖ້າ (dblist == NULL || pplist == NULL || fllist == NULL)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	ສົ່ງຄືນ CSCOPE_FAILURE;
    }

    ສຳລັບ (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	ຖ້າ (csinfo[i].fname != NULL)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    ສຳລັບ (i = 0; i < csinfo_size; i++)
    {
	ຖ້າ (dblist[i] != NULL)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    ຖ້າ (p_csverbose)
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

    ຖ້າ (p_csverbose)
	msg_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | MSG_HIST);
    ສົ່ງຄືນ CSCOPE_SUCCESS;
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
    static var_na *
cs_resolve_file(ຈຳນວນເຕັມ i, var_na *name)
{
    var_na	*fullname;
    string_T	csdir = {NULL, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    ຈຳນວນເຕັມ		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (ຈຳນວນເຕັມ)namelen + 2;
    ຖ້າ (csinfo[i].ppath != NULL)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (ຈຳນວນເຕັມ)ppathlen;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (p_csre && csinfo[i].fname != NULL)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	ຖ້າ (csdir.string != NULL)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (ຈຳນວນເຕັມ)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    ຖ້າ (csinfo[i].ppath != NULL
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	ຖ້າ ((fullname = alloc(len)) != NULL)
	    (void)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (csdir.string != NULL && csinfo[i].fname != NULL && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (var_na *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	fullname = (var_na *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    ສົ່ງຄືນ fullname;
}


/*
 * Show all cscope connections.
 */
    static ຈຳນວນເຕັມ
cs_show(exarg_T *eap UNUSED)
{
    ຈຳນວນເຕັມ i;

    ຖ້າ (cs_cnt_connections() == 0)
	msg_puts(_("no cscope connections\n"));
    ບໍ່ດັ່ງນັ້ນ
    {
	msg_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	ສຳລັບ (i = 0; i < csinfo_size; i++)
	{
	    ຖ້າ (csinfo[i].fname == NULL)
		continue;

	    ຖ້າ (csinfo[i].ppath != NULL)
		(void)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    ບໍ່ດັ່ງນັ້ນ
		(void)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    ສົ່ງຄືນ CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    void
cs_end(void)
{
    ຈຳນວນເຕັມ i;

    ສຳລັບ (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#ຖ້າ defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    void
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    ຈຳນວນເຕັມ		num = 0;
    char_u	*dbpath = NULL;
    char_u	*prepend = NULL;
    char_u	buf[NUMBUFLEN];

    ຖ້າ (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	ສົ່ງຄືນ;

    ຖ້າ (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	num = (ຈຳນວນເຕັມ)tv_get_number(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	ຖ້າ (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_number = cs_connection(num, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
