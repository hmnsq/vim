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

#nếu defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# nếu defined(UNIX)
#  include <sys/wait.h>
# endif

# nếu defined (MSWIN)
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

typedef struct {
    char *  name;
    số_nguyên     (*func)(exarg_T *eap);
    char *  help;
    char *  usage;
    số_nguyên	    cansplit;		// if supports splitting window
} cscmd_T;

typedef struct csi {
    char *	    fname;	// cscope db name
    char *	    ppath;	// path to prepend (the -P option)
    char *	    flags;	// additional cscope flags/options (e.g, -p2)
# nếu defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# không_thì
#  nếu defined(MSWIN)
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

typedef enum { Add, Find, Help, Kill, Reset, Show } csid_e;

typedef enum {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

static số_nguyên	    cs_add(exarg_T *eap);
static số_nguyên	    cs_add_common(char *, char *, char *);
static số_nguyên	    cs_check_for_connections(void);
static số_nguyên	    cs_check_for_tags(void);
static số_nguyên	    cs_cnt_connections(void);
static số_nguyên	    cs_create_connection(số_nguyên i);
# ifdef FEAT_QUICKFIX
static void	    cs_file_results(FILE *, số_nguyên *);
# endif
static void	    cs_fill_results(char *, số_nguyên , số_nguyên *, char ***,
			char ***, số_nguyên *);
static số_nguyên	    cs_find(exarg_T *eap);
static số_nguyên	    cs_find_common(char *opt, char *pat, số_nguyên, số_nguyên, số_nguyên, char_u *cmdline);
static số_nguyên	    cs_help(exarg_T *eap);
static số_nguyên	    cs_insert_filelist(char *, char *, char *,
			stat_T *);
static số_nguyên	    cs_kill(exarg_T *eap);
static void	    cs_kill_execute(số_nguyên, char *);
static cscmd_T *    cs_lookup_cmd(exarg_T *eap);
static char *	    cs_make_vim_style_matches(char *, char *,
			char *, char *);
static char *	    cs_manage_matches(char **, char **, số_nguyên, mcmd_e);
static void	    cs_print_tags_priv(char **, char **, số_nguyên);
static số_nguyên	    cs_read_prompt(số_nguyên);
static void	    cs_release_csp(số_nguyên, số_nguyên freefnpp);
static số_nguyên	    cs_reset(exarg_T *eap);
static char *	    cs_resolve_file(số_nguyên, char *);
static số_nguyên	    cs_show(exarg_T *eap);


static csinfo_T *   csinfo = NULL;
static số_nguyên	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

static số_nguyên	    eap_arg_len;    // length of eap->arg, set in
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
    (void)semsg(_(e_usage_cscope_str), cs_cmds[(số_nguyên)x].usage);
}

static enum
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
get_cscope_name(expand_T *xp UNUSED, số_nguyên idx)
{
    số_nguyên		current_idx;
    số_nguyên		i;

    switch (expand_what)
    {
    case EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	trả_về (char_u *)cs_cmds[idx].name;
    case EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	cho (i = 0, current_idx = 0; cs_cmds[i].name != NULL; i++)
	    nếu (cs_cmds[i].cansplit)
		nếu (current_idx++ == idx)
		    break;
	trả_về (char_u *)cs_cmds[i].name;
    case EXP_CSCOPE_FIND:
	{
	    const char *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", NULL
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    trả_về (char_u *)query_type[idx];
	}
    case EXP_CSCOPE_KILL:
	{
	    static char	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    cho (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		nếu (csinfo[i].fname == NULL)
		    continue;
		nếu (current_idx++ == idx)
		{
		    vim_snprintf(connection, sizeof(connection), "%d", i);
		    trả_về (char_u *)connection;
		}
	    }
	    trả_về (current_idx == idx && idx > 0) ? (char_u *)"-1" : NULL;
	}
    default:
	trả_về NULL;
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

    nếu (*arg == NUL)
	trả_về;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    nếu (*p == NUL)
	trả_về;

    // past first word
    xp->xp_pattern = skipwhite(p);
    nếu (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    không_thì nếu (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    không_thì nếu (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    không_thì nếu (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    không_thì
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    static void
do_cscope_general(
    exarg_T	*eap,
    số_nguyên		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    nếu (check_restricted())
	trả_về;

    nếu ((cmdp = cs_lookup_cmd(eap)) == NULL)
    {
	cs_help(eap);
	trả_về;
    }

    nếu (make_split)
    {
	nếu (!cmdp->cansplit)
	{
	    (void)msg_puts(_("This cscope command does not support splitting the window.\n"));
	    trả_về;
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
    số_nguyên ret = FALSE;

    nếu (*eap->arg == NUL)
    {
	(void)emsg(_(e_usage_cstag_ident));
	trả_về;
    }

    switch (p_csto)
    {
    case 0 :
	nếu (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (char *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    nếu (ret == FALSE)
	    {
		cs_free_tags();
		nếu (msg_col)
		    msg_putchar('\n');

		nếu (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	không_thì nếu (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	break;
    case 1 :
	nếu (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    nếu (ret == FALSE)
	    {
		nếu (msg_col)
		    msg_putchar('\n');

		nếu (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (char *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    nếu (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	không_thì nếu (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (char *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    nếu (ret == FALSE)
		cs_free_tags();
	}
	break;
    default :
	break;
    }

    nếu (!ret)
    {
	(void)emsg(_(e_cstag_tag_not_founc));
# nếu defined(FEAT_QUICKFIX)
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
    số_nguyên
cs_fgets(char_u *buf, số_nguyên size)
{
    char *p;

    nếu ((p = cs_manage_matches(NULL, NULL, -1, Get)) == NULL)
	trả_về TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    trả_về FALSE;
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
# nếu defined(FEAT_EVAL)
    static số_nguyên
cs_connection(số_nguyên num, char_u *dbpath, char_u *ppath)
{
    số_nguyên i;

    nếu (num < 0 || num > 4 || (num > 0 && !dbpath))
	trả_về FALSE;

    cho (i = 0; i < csinfo_size; i++)
    {
	nếu (!csinfo[i].fname)
	    continue;

	nếu (num == 0)
	    trả_về TRUE;

	switch (num)
	{
	case 1:
	    nếu (strstr(csinfo[i].fname, (char *)dbpath))
		trả_về TRUE;
	    break;
	case 2:
	    nếu (strcmp(csinfo[i].fname, (char *)dbpath) == 0)
		trả_về TRUE;
	    break;
	case 3:
	    nếu (strstr(csinfo[i].fname, (char *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (char *)ppath))))
		trả_về TRUE;
	    break;
	case 4:
	    nếu ((strcmp(csinfo[i].fname, (char *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (char *)ppath) == 0))))
		trả_về TRUE;
	    break;
	}
    }

    trả_về FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    static số_nguyên
cs_add(exarg_T *eap UNUSED)
{
    char *fname, *ppath, *flags = NULL;

    nếu ((fname = strtok((char *)NULL, (const char *)" ")) == NULL)
    {
	cs_usage_msg(Add);
	trả_về CSCOPE_FAILURE;
    }
    nếu ((ppath = strtok((char *)NULL, (const char *)" ")) != NULL)
	flags = strtok((char *)NULL, (const char *)" ");

    trả_về cs_add_common(fname, ppath, flags);
}

    static void
cs_stat_emsg(char *fname)
{
    số_nguyên err = errno;
    (void)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    static số_nguyên
cs_add_common(
    char *arg1,	    // filename - may contain environment variables
    char *arg2,	    // prepend path - may contain environment variables
    char *flags)
{
    stat_T	statbuf;
    số_nguyên		ret;
    char	*fname = NULL;
    char	*fname2 = NULL;
    char	*ppath = NULL;
    số_nguyên		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = NULL;

    // get the filename (arg1), expand it, and try to stat it
    nếu ((fname = alloc(MAXPATHL + 1)) == NULL)
	goto add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (void)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    nếu (fname == NULL)
	goto add_err;
    fname = (char *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    nếu (ret < 0)
    {
staterr:
	nếu (p_csverbose)
	    cs_stat_emsg(fname);
	goto add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    nếu (arg2 != NULL)
    {
	stat_T	    statbuf2;

	nếu ((ppath = alloc(MAXPATHL + 1)) == NULL)
	    goto add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	nếu (ret < 0)
	    goto staterr;
    }

    // if filename is a directory, append the cscope database name to it
    nếu (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	nếu (fname2 == NULL)
	    goto add_err;

	trong_khi (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    nếu (fname[0] == '\0')
		break;
	}
	nếu (fname[0] == '\0')
	    (void)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	không_thì
	    (void)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	nếu (ret < 0)
	{
	    nếu (p_csverbose)
		cs_stat_emsg(fname2);
	    goto add_err;
	}

	i = cs_insert_filelist(fname2, ppath, flags, &statbuf);
    }
    không_thì nếu (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_insert_filelist(fname, ppath, flags, &statbuf);
    }
    không_thì
    {
	nếu (p_csverbose)
	    (void)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	goto add_err;
    }

    nếu (i != -1)
    {
	nếu (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    goto add_err;
	}

	nếu (p_csverbose)
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
    trả_về CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    trả_về CSCOPE_FAILURE;
}


    static số_nguyên
cs_check_for_connections(void)
{
    trả_về (cs_cnt_connections() > 0);
}


    static số_nguyên
cs_check_for_tags(void)
{
    trả_về (p_tags[0] != NUL && curbuf->b_p_tags != NULL);
}


/*
 * Count the number of cscope connections.
 */
    static số_nguyên
cs_cnt_connections(void)
{
    số_nguyên i;
    số_nguyên cnt = 0;

    cho (i = 0; i < csinfo_size; i++)
	nếu (csinfo[i].fname != NULL)
	    cnt++;
    trả_về cnt;
}

    static void
cs_reading_emsg(
    số_nguyên idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    static số_nguyên
cs_cnt_matches(số_nguyên idx)
{
    char *stok;
    char *buf;
    số_nguyên nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    nếu (buf == NULL)
	trả_về 0;
    cho (;;)
    {
	nếu (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    nếu (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    trả_về -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	nếu (strstr((const char *)buf, "Unable to search database") != NULL)
	    break;
	nếu ((stok = strtok(buf, (const char *)" ")) == NULL)
	    continue;
	nếu (strstr((const char *)stok, "cscope:") == NULL)
	    continue;

	nếu ((stok = strtok(NULL, (const char *)" ")) == NULL)
	    continue;
	nlines = atoi(stok);
	nếu (nlines < 0)
	{
	    nlines = 0;
	    break;
	}

	nếu ((stok = strtok(NULL, (const char *)" ")) == NULL)
	    continue;
	nếu (strncmp((const char *)stok, "lines", 5))
	    continue;

	break;
    }

    vim_free(buf);
    trả_về nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    static char *
cs_create_cmd(char *csoption, char *pattern)
{
    char *cmd;
    short search;
    char *pat;

    switch (csoption[0])
    {
    case '0' : case 's' :
	search = 0;
	break;
    case '1' : case 'g' :
	search = 1;
	break;
    case '2' : case 'd' :
	search = 2;
	break;
    case '3' : case 'c' :
	search = 3;
	break;
    case '4' : case 't' :
	search = 4;
	break;
    case '6' : case 'e' :
	search = 6;
	break;
    case '7' : case 'f' :
	search = 7;
	break;
    case '8' : case 'i' :
	search = 8;
	break;
    case '9' : case 'a' :
	search = 9;
	break;
    default :
	(void)emsg(_(e_unknown_cscope_search_type));
	cs_usage_msg(Find);
	trả_về NULL;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    nếu (search != 4 && search != 6)
	trong_khi VIM_ISWHITE(*pat)
	    ++pat;

    nếu ((cmd = alloc(strlen(pat) + 2)) == NULL)
	trả_về NULL;

    (void)sprintf(cmd, "%d%s", search, pat);

    trả_về cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    static số_nguyên
cs_create_connection(số_nguyên i)
{
# ifdef UNIX
    số_nguyên		to_cs[2], from_cs[2];
# endif
    số_nguyên		cmdlen;
    char	*prog, *cmd, *ppath = NULL;
    size_t	proglen;
# ifdef MSWIN
    số_nguyên		fd;
    số_nguyên		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# nếu defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    nếu (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(void)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	nếu (to_cs[0] != -1)
	    (void)close(to_cs[0]);
	nếu (to_cs[1] != -1)
	    (void)close(to_cs[1]);
	nếu (from_cs[0] != -1)
	    (void)close(from_cs[0]);
	nếu (from_cs[1] != -1)
	    (void)close(from_cs[1]);
	trả_về CSCOPE_FAILURE;
    }

    nếu ((csinfo[i].pid = fork()) == -1)
    {
	(void)emsg(_(e_could_not_fork_for_cscope));
	goto err_closing;
    }
    không_thì nếu (csinfo[i].pid == 0)	// child: run cscope.
    {
	char **argv = NULL;

	nếu (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	nếu (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	nếu (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(void)close(to_cs[1]);
	(void)close(from_cs[0]);
# không_thì
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	nếu (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (void)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    nếu (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    nếu (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    trả_về CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	nếu ((prog = alloc(MAXPATHL + 1)) == NULL)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# không_thì
	    // MSWIN
	    goto err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (số_nguyên)(proglen + strlen(csinfo[i].fname) + 32);
	nếu (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    nếu ((ppath = alloc(MAXPATHL + 1)) == NULL)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# không_thì
		// MSWIN
		goto err_closing;
# endif
	    }
	    cmdlen += (số_nguyên)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	nếu (csinfo[i].flags)
	    cmdlen += (số_nguyên)strlen(csinfo[i].flags);

	nếu ((cmd = alloc(cmdlen)) == NULL)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# không_thì
	    // MSWIN
	    goto err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    char	**tok = NULL;
	    số_nguyên		tokc = 0;

	    ga_init2(&ga_argv, sizeof(char *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    nếu (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    cho (số_nguyên k = 0; k < tokc; ++k)
	    {
		nếu (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((char **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    nếu (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((char **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (char *)vim_strsave((char_u *)"-dl");
	    ((char **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (char *)vim_strsave((char_u *)"-f");
	    ((char **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(char *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    nếu (ppath != NULL)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		char	*parg = alloc(plen);

		nếu (parg == NULL || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((char **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    nếu (csinfo[i].flags != NULL)
	    {
		nếu (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		cho (số_nguyên k = 0; k < tokc; ++k)
		{
		    nếu (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((char **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    nếu (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((char **)ga_argv.ga_data)[ga_argv.ga_len] = NULL;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  nếu defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   nếu defined(HAVE_SETSID)
	    (void)setsid();
#   không_thì
	    nếu (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (char **)ga_argv.ga_data;
	    nếu (argv[0] == NULL || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    không_thì	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	nếu ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == NULL)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	nếu ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == NULL)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(void)close(to_cs[0]);
	(void)close(from_cs[1]);
    }
# không_thì
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    nếu (csinfo[i].ppath != NULL)
    {
	len = (số_nguyên)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    nếu (csinfo[i].flags != NULL)
    {
	len = (số_nguyên)STRLEN(cmd);
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

    nếu (!created)
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
    nếu (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == NULL))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    nếu (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == NULL))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    trả_về CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    static số_nguyên
cs_find(exarg_T *eap)
{
    char *opt, *pat;
    số_nguyên i;

    nếu (cs_check_for_connections() == FALSE)
    {
	(void)emsg(_(e_no_cscope_connections));
	trả_về FALSE;
    }

    nếu ((opt = strtok((char *)NULL, (const char *)" ")) == NULL)
    {
	cs_usage_msg(Find);
	trả_về FALSE;
    }

    pat = opt + strlen(opt) + 1;
    nếu (pat >= (char *)eap->arg + eap_arg_len)
    {
	cs_usage_msg(Find);
	trả_về FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    cho (i = 0; i < eap_arg_len; ++i)
	nếu (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    trả_về cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    static số_nguyên
cs_find_common(
    char *opt,
    char *pat,
    số_nguyên forceit,
    số_nguyên verbose,
    số_nguyên	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    số_nguyên i;
    char *cmd;
    số_nguyên *nummatches;
    số_nguyên totmatches;
# ifdef FEAT_QUICKFIX
    char cmdletter;
    char *qfpos;

    // get cmd letter
    switch (opt[0])
    {
    case '0' :
	cmdletter = 's';
	break;
    case '1' :
	cmdletter = 'g';
	break;
    case '2' :
	cmdletter = 'd';
	break;
    case '3' :
	cmdletter = 'c';
	break;
    case '4' :
	cmdletter = 't';
	break;
    case '6' :
	cmdletter = 'e';
	break;
    case '7' :
	cmdletter = 'f';
	break;
    case '8' :
	cmdletter = 'i';
	break;
    case '9' :
	cmdletter = 'a';
	break;
    default :
	cmdletter = opt[0];
    }

    qfpos = (char *)vim_strchr(p_csqf, cmdletter);
    nếu (qfpos != NULL)
    {
	qfpos++;
	// next symbol must be + or -
	nếu (strchr(CSQF_FLAGS, *qfpos) == NULL)
	{
	    (void)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    trả_về FALSE;
	}

	nếu (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    nếu (aborting())
		trả_về FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    nếu (cmd == NULL)
	trả_về FALSE;

    nummatches = ALLOC_MULT(số_nguyên, csinfo_size);
    nếu (nummatches == NULL)
    {
	vim_free(cmd);
	trả_về FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    cho (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    cho (i = 0; i < csinfo_size; i++)
    {
	nếu (csinfo[i].fname == NULL || csinfo[i].to_fp == NULL)
	    continue;

	// send cmd to cscope
	(void)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(void)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	nếu (nummatches[i] > -1)
	    totmatches += nummatches[i];

	nếu (nummatches[i] == 0)
	    (void)cs_read_prompt(i);
    }
    vim_free(cmd);

    nếu (totmatches == 0)
    {
	nếu (verbose)
	    (void)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	trả_về FALSE;
    }

# ifdef FEAT_QUICKFIX
    nếu (qfpos != NULL && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = NULL;
	win_T	    *wp = NULL;

	nếu (tmp == NULL)
	{
	    vim_free(nummatches);
	    trả_về FALSE;
	}

	f = mch_fopen((char *)tmp, "w");
	nếu (f == NULL)
	    semsg(_(e_cant_open_file_str), tmp);
	không_thì
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    nếu (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    nếu (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, NULL) > 0)
	    {
		nếu (postponed_split != 0)
		{
		    (void)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		nếu (use_ll)
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
	trả_về TRUE;
    }
    không_thì
# endif // FEAT_QUICKFIX
    {
	char **matches = NULL, **contexts = NULL;
	số_nguyên matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	nếu (matches == NULL)
	    trả_về FALSE;

	(void)cs_manage_matches(matches, contexts, matched, Store);

	trả_về do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    static số_nguyên
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (void)msg_puts(_("cscope commands:\n"));
    trong_khi (cmdp->name != NULL)
    {
	char *help = _(cmdp->help);
	số_nguyên  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	nếu (space_cnt < 0)
	    space_cnt = 0;
	(void)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	nếu (strcmp(cmdp->name, "find") == 0)
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
    trả_về 0;
}


    static void
clear_csinfo(số_nguyên i)
{
    csinfo[i].fname  = NULL;
    csinfo[i].ppath  = NULL;
    csinfo[i].flags  = NULL;
# nếu defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# không_thì
    csinfo[i].nVolume = 0;
    csinfo[i].nIndexHigh = 0;
    csinfo[i].nIndexLow = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = NULL;
    csinfo[i].to_fp  = NULL;
# nếu defined(MSWIN)
    csinfo[i].hProc = NULL;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    static số_nguyên
cs_insert_filelist(
    char *fname,
    char *ppath,
    char *flags,
    stat_T *sb UNUSED)
{
    số_nguyên	    i;
    số_nguyên	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    switch (win32_fileinfo((char_u *)fname, &bhfi))
    {
	case FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	case FILEINFO_READ_FAIL:	// CreateFile() failed
	    nếu (p_csverbose)
	    {
		char *cant_msg = _(e_cannot_open_cscope_database_str);
		char *winmsg = GetWin32Error();

		nếu (winmsg != NULL)
		    (void)semsg(cant_msg, winmsg);
		không_thì
		    // subst filename if can't get error text
		    (void)semsg(cant_msg, fname);
	    }
	    trả_về -1;

	case FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    nếu (p_csverbose)
		(void)emsg(_(e_cannot_get_cscope_database_information));
	    trả_về -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    cho (j = 0; j < csinfo_size; j++)
    {
	nếu (csinfo[j].fname != NULL
# nếu defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# không_thì
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
	    nếu (p_csverbose)
		(void)emsg(_(e_duplicate_cscope_database_not_added));
	    trả_về -1;
	}

	nếu (csinfo[j].fname == NULL && i == -1)
	    i = j; // remember first empty entry
    }

    nếu (i == -1)
    {
	i = csinfo_size;
	nếu (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	không_thì
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, sizeof(csinfo_T)*csinfo_size);
	    nếu (csinfo == NULL)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		trả_về -1;
	    }
	}
	nếu (csinfo == NULL)
	    trả_về -1;
	cho (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    nếu ((csinfo[i].fname = alloc(strlen(fname)+1)) == NULL)
	trả_về -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((const char *)fname));

    nếu (ppath != NULL)
    {
	nếu ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == NULL)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    trả_về -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((const char *)ppath));
    }
    không_thì
	csinfo[i].ppath = NULL;

    nếu (flags != NULL)
    {
	nếu ((csinfo[i].flags = alloc(strlen(flags) + 1)) == NULL)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    trả_về -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((const char *)flags));
    }
    không_thì
	csinfo[i].flags = NULL;

# nếu defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# không_thì
    csinfo[i].nVolume = bhfi.dwVolumeSerialNumber;
    csinfo[i].nIndexLow = bhfi.nFileIndexLow;
    csinfo[i].nIndexHigh = bhfi.nFileIndexHigh;
# endif
    trả_về i;
}


/*
 * Find cscope command in command table.
 */
    static cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    char *stok;
    size_t len;

    nếu (eap->arg == NULL)
	trả_về NULL;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (số_nguyên)STRLEN(eap->arg);

    nếu ((stok = strtok((char *)(eap->arg), (const char *)" ")) == NULL)
	trả_về NULL;

    len = strlen(stok);
    cho (cmdp = cs_cmds; cmdp->name != NULL; ++cmdp)
    {
	nếu (strncmp((const char *)(stok), cmdp->name, len) == 0)
	    trả_về (cmdp);
    }
    trả_về NULL;
}


/*
 * Nuke em.
 */
    static số_nguyên
cs_kill(exarg_T *eap UNUSED)
{
    char *stok;
    số_nguyên i;

    nếu ((stok = strtok((char *)NULL, (const char *)" ")) == NULL)
    {
	cs_usage_msg(Kill);
	trả_về CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    nếu ((strlen(stok) < 2 && VIM_ISDIGIT((số_nguyên)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((số_nguyên)(stok[1]))))
	i = atoi(stok);
    không_thì
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	cho (i = 0; i < csinfo_size; i++)
	{
	    nếu (csinfo[i].fname != NULL && strstr(csinfo[i].fname, stok))
		break;
	}
    }

    nếu ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == NULL))
    {
	nếu (p_csverbose)
	    (void)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    không_thì
    {
	nếu (i == -1)
	{
	    cho (i = 0; i < csinfo_size; i++)
	    {
		nếu (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	không_thì
	    cs_kill_execute(i, stok);
    }

    trả_về 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    static void
cs_kill_execute(
    số_nguyên i,	    // cscope table index
    char *cname)    // cscope database name
{
    nếu (p_csverbose)
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
    static char *
cs_make_vim_style_matches(
    char *fname,
    char *slno,
    char *search,
    char *tagstr)
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
    char *buf;
    số_nguyên amt;

    nếu (search != NULL)
    {
	amt = (số_nguyên)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(search)+6);
	nếu ((buf = alloc(amt)) == NULL)
	    trả_về NULL;

	(void)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, search);
    }
    không_thì
    {
	amt = (số_nguyên)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	nếu ((buf = alloc(amt)) == NULL)
	    trả_về NULL;

	(void)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    trả_về buf;
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
    static char *
cs_manage_matches(
    char **matches,
    char **contexts,
    số_nguyên totmatches,
    mcmd_e cmd)
{
    static char **mp = NULL;
    static char **cp = NULL;
    static số_nguyên cnt = -1;
    static số_nguyên next = -1;
    char *p = NULL;

    switch (cmd)
    {
    case Store:
	assert(matches != NULL);
	assert(totmatches > 0);
	nếu (mp != NULL || cp != NULL)
	    (void)cs_manage_matches(NULL, NULL, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	break;
    case Get:
	nếu (next >= cnt)
	    trả_về NULL;

	p = mp[next];
	next++;
	break;
    case Free:
	nếu (mp != NULL)
	{
	    nếu (cnt > 0)
		trong_khi (cnt--)
		{
		    vim_free(mp[cnt]);
		    nếu (cp != NULL)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = NULL;
	cp = NULL;
	cnt = 0;
	next = 0;
	break;
    case Print:
	cs_print_tags_priv(mp, cp, cnt);
	break;
    default:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	trả_về NULL;
    }

    trả_về p;
}


/*
 * Parse cscope output.
 */
    static char *
cs_parse_results(
    số_nguyên cnumber,
    char *buf,
    số_nguyên bufsize,
    char **context,
    char **linenumber,
    char **search)
{
    số_nguyên ch;
    char *p;
    char *name;

    nếu (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == NULL)
    {
	nếu (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	trả_về NULL;
    }

    // If the line's too long for the buffer, discard it.
    nếu ((p = strchr(buf, '\n')) == NULL)
    {
	trong_khi ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	trả_về NULL;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    nếu ((name = strtok(buf, (const char *)" ")) == NULL)
	trả_về NULL;
    nếu ((*context = strtok(NULL, (const char *)" ")) == NULL)
	trả_về NULL;
    nếu ((*linenumber = strtok(NULL, (const char *)" ")) == NULL)
	trả_về NULL;
    *search = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    nếu (strcmp(*search, "<unknown>") == 0)
	*search = NULL;

    name = cs_resolve_file(cnumber, name);
    trả_về name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    static void
cs_file_results(FILE *f, số_nguyên *nummatches_a)
{
    số_nguyên i, j;
    char *buf;
    char *search, *slno;
    char *fullname;
    char *cntx;
    char *context;

    buf = alloc(CSREAD_BUFSIZE);
    nếu (buf == NULL)
	trả_về;

    cho (i = 0; i < csinfo_size; i++)
    {
	nếu (nummatches_a[i] < 1)
	    continue;

	cho (j = 0; j < nummatches_a[i]; j++)
	{
	   nếu ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &search)) == NULL)
	       continue;

	   context = alloc(strlen(cntx)+5);
	   nếu (context == NULL)
	   {
	       vim_free(fullname);
	       continue;
	   }

	   nếu (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   không_thì
	       sprintf(context, "<<%s>>", cntx);

	   nếu (search == NULL)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   không_thì
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
    char *tagstr,
    số_nguyên totmatches,
    số_nguyên *nummatches_a,
    char ***matches_p,
    char ***cntxts_p,
    số_nguyên *matched)
{
    số_nguyên i, j;
    char *buf;
    char *search, *slno;
    số_nguyên totsofar = 0;
    char **matches = NULL;
    char **cntxts = NULL;
    char *fullname;
    char *cntx;

    assert(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    nếu (buf == NULL)
	trả_về;

    nếu ((matches = ALLOC_MULT(char *, totmatches)) == NULL)
	goto parse_out;
    nếu ((cntxts = ALLOC_MULT(char *, totmatches)) == NULL)
	goto parse_out;

    cho (i = 0; i < csinfo_size; i++)
    {
	nếu (nummatches_a[i] < 1)
	    continue;

	cho (j = 0; j < nummatches_a[i]; j++)
	{
	   nếu ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &search)) == NULL)
		continue;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  search, tagstr);

	    vim_free(fullname);

	    nếu (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = NULL;
	    không_thì
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (char *)vim_strsave((char_u *)cntx);

	    nếu (matches[totsofar] != NULL)
		totsofar++;

	} // for all matches

	(void)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    nếu (totsofar == 0)
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
    static char *
cs_pathcomponents(char *path)
{
    số_nguyên		i;
    char	*s;

    nếu (p_cspc == 0)
	trả_về path;

    s = path + strlen(path) - 1;
    cho (i = 0; i < p_cspc; ++i)
    {
	trong_khi (s > path)
	{
	   s--;
	   nếu (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      break;
	}
    }
    nếu ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    trả_về s;
}

/*
 * Called from cs_manage_matches().
 */
    static void
cs_print_tags_priv(char **matches, char **cntxts, số_nguyên num_matches)
{
    char	*buf = NULL;
    char	*t_buf;
    số_nguyên		bufsize = 0; // Track available bufsize
    số_nguyên		newsize = 0;
    char	*ptag;
    char	*fname, *lno, *extra, *tbuf;
    số_nguyên		i, idx, num;
    char	*globalcntx = "GLOBAL";
    char	*cntxformat = " <<%s>>";
    char	*context;
    char	*cstag_msg = _("Cscope tag: %s");
    char	*csfmt_str = "%4d %6s  ";

    assert(num_matches > 0);

    nếu ((tbuf = alloc(strlen(matches[0]) + 1)) == NULL)
	trả_về;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    nếu (ptag == NULL)
    {
	vim_free(tbuf);
	trả_về;
    }

    newsize = (số_nguyên)(strlen(cstag_msg) + strlen(ptag));
    buf = alloc(newsize);
    nếu (buf != NULL)
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
    cho (i = 0; i < num_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	nếu ((tbuf = alloc(strlen(matches[idx]) + 1)) == NULL)
	    continue;
	(void)strcpy(tbuf, matches[idx]);

	nếu (strtok(tbuf, (const char *)"\t") == NULL
		|| (fname = strtok(NULL, (const char *)"\t")) == NULL
		|| (lno = strtok(NULL, (const char *)"\t")) == NULL)
	{
	    vim_free(tbuf);
	    continue;
	}
	extra = strtok(NULL, (const char *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (số_nguyên)(strlen(csfmt_str) + 16 + strlen(lno));
	nếu (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    nếu (buf == NULL)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    không_thì
		bufsize = newsize;
	}
	nếu (buf != NULL)
	{
	    // csfmt_str = "%4d %6s  ";
	    (void)sprintf(buf, csfmt_str, num, lno);
	    msg_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	msg_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	nếu (cntxts[idx] != NULL)
	    context = cntxts[idx];
	không_thì
	    context = globalcntx;
	newsize = (số_nguyên)(strlen(context) + strlen(cntxformat));

	nếu (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    nếu (buf == NULL)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    không_thì
		bufsize = newsize;
	}
	nếu (buf != NULL)
	{
	    (void)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    nếu (msg_col + (số_nguyên)strlen(buf) >= cmdline_width)
		msg_putchar('\n');
	    msg_advance(12);
	    msg_outtrans_long_attr((char_u *)buf, 0);
	    msg_putchar('\n');
	}
	nếu (extra != NULL)
	{
	    msg_advance(13);
	    msg_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	nếu (msg_col)
	    msg_putchar('\n');

	ui_breakcheck();
	nếu (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    break;
	}

	num++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    static số_nguyên
cs_read_prompt(số_nguyên i)
{
    số_nguyên		ch;
    char	*buf = NULL; // buffer for possible error message from cscope
    số_nguyên		bufpos = 0;
    số_nguyên		maxlen;
    static char *eprompt = "Press the RETURN key to continue:";
    số_nguyên		epromptlen = (số_nguyên)strlen(eprompt);
    số_nguyên		n;

    // compute maximum allowed len for Cscope error message
    char *cs_emsg = _(e_cscope_error_str);
    maxlen = (số_nguyên)(IOSIZE - strlen(cs_emsg));

    cho (;;)
    {
	trong_khi ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    nếu (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		nếu (buf == NULL) // lazy buffer allocation
		    buf = alloc(maxlen);
		nếu (buf != NULL)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    nếu (bufpos >= epromptlen
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

	cho (n = 0; n < (số_nguyên)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    nếu (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    nếu (ch == EOF)
	    {
		nếu (buf != NULL && buf[0] != NUL)
		    (void)semsg(cs_emsg, buf);
		không_thì nếu (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		trả_về CSCOPE_FAILURE;
	    }

	    nếu (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		break;
	    }
	}

	nếu (ch == EOF)
	    continue;	    // didn't find the prompt
	break;		    // did find the prompt
    }

    vim_free(buf);
    trả_về CSCOPE_SUCCESS;
}

# nếu defined(UNIX) && defined(SIGALRM)
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
cs_release_csp(số_nguyên i, số_nguyên freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    nếu (csinfo[i].to_fp != NULL)
    {
	(void)fputs("q\n", csinfo[i].to_fp);
	(void)fflush(csinfo[i].to_fp);
    }
# nếu defined(UNIX)
    {
	số_nguyên waitpid_errno;
	số_nguyên pstat;
	pid_t pid;

#  nếu defined(HAVE_SIGACTION)
	struct sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   không_thì
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
#  không_thì
	số_nguyên waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	cho (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    nếu (pid != 0)
		break;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	nếu (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    số_nguyên alive = TRUE;

	    nếu (waitpid_errno == ECHILD)
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
		số_nguyên waited;

		sleep(0);
		cho (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    nếu (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			break;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    nếu (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(void)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# không_thì  // !UNIX
    nếu (csinfo[i].hProc != NULL)
    {
	// Give cscope a chance to exit normally
	nếu (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    nếu (csinfo[i].fr_fp != NULL)
	(void)fclose(csinfo[i].fr_fp);
    nếu (csinfo[i].to_fp != NULL)
	(void)fclose(csinfo[i].to_fp);

    nếu (freefnpp)
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
    static số_nguyên
cs_reset(exarg_T *eap UNUSED)
{
    char	**dblist = NULL, **pplist = NULL, **fllist = NULL;
    số_nguyên	i;
    char buf[20]; // for sprintf " (#%d)"

    nếu (csinfo_size == 0)
	trả_về CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(char *, csinfo_size);
    pplist = ALLOC_MULT(char *, csinfo_size);
    fllist = ALLOC_MULT(char *, csinfo_size);
    nếu (dblist == NULL || pplist == NULL || fllist == NULL)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	trả_về CSCOPE_FAILURE;
    }

    cho (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	nếu (csinfo[i].fname != NULL)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    cho (i = 0; i < csinfo_size; i++)
    {
	nếu (dblist[i] != NULL)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    nếu (p_csverbose)
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

    nếu (p_csverbose)
	msg_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | MSG_HIST);
    trả_về CSCOPE_SUCCESS;
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
    static char *
cs_resolve_file(số_nguyên i, char *name)
{
    char	*fullname;
    string_T	csdir = {NULL, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    số_nguyên		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (số_nguyên)namelen + 2;
    nếu (csinfo[i].ppath != NULL)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (số_nguyên)ppathlen;
    }
    không_thì nếu (p_csre && csinfo[i].fname != NULL)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	nếu (csdir.string != NULL)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (số_nguyên)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    nếu (csinfo[i].ppath != NULL
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	nếu ((fullname = alloc(len)) != NULL)
	    (void)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    không_thì nếu (csdir.string != NULL && csinfo[i].fname != NULL && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (char *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    không_thì
    {
	fullname = (char *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    trả_về fullname;
}


/*
 * Show all cscope connections.
 */
    static số_nguyên
cs_show(exarg_T *eap UNUSED)
{
    số_nguyên i;

    nếu (cs_cnt_connections() == 0)
	msg_puts(_("no cscope connections\n"));
    không_thì
    {
	msg_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	cho (i = 0; i < csinfo_size; i++)
	{
	    nếu (csinfo[i].fname == NULL)
		continue;

	    nếu (csinfo[i].ppath != NULL)
		(void)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    không_thì
		(void)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    trả_về CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    void
cs_end(void)
{
    số_nguyên i;

    cho (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#nếu defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    void
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    số_nguyên		num = 0;
    char_u	*dbpath = NULL;
    char_u	*prepend = NULL;
    char_u	buf[NUMBUFLEN];

    nếu (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	trả_về;

    nếu (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	num = (số_nguyên)tv_get_number(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	nếu (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_number = cs_connection(num, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
