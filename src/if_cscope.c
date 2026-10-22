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

#якщо defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# якщо defined(UNIX)
#  include <sys/wait.h>
# endif

# якщо defined (MSWIN)
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
    ціле     (*func)(exarg_T *eap);
    char *  help;
    char *  usage;
    ціле	    cansplit;		// if supports splitting window
} cscmd_T;

typedef struct csi {
    char *	    fname;	// cscope db name
    char *	    ppath;	// path to prepend (the -P option)
    char *	    flags;	// additional cscope flags/options (e.g, -p2)
# якщо defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# інакше
#  якщо defined(MSWIN)
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

static ціле	    cs_add(exarg_T *eap);
static ціле	    cs_add_common(char *, char *, char *);
static ціле	    cs_check_for_connections(void);
static ціле	    cs_check_for_tags(void);
static ціле	    cs_cnt_connections(void);
static ціле	    cs_create_connection(ціле i);
# ifdef FEAT_QUICKFIX
static void	    cs_file_results(FILE *, ціле *);
# endif
static void	    cs_fill_results(char *, ціле , ціле *, char ***,
			char ***, ціле *);
static ціле	    cs_find(exarg_T *eap);
static ціле	    cs_find_common(char *opt, char *pat, ціле, ціле, ціле, char_u *cmdline);
static ціле	    cs_help(exarg_T *eap);
static ціле	    cs_insert_filelist(char *, char *, char *,
			stat_T *);
static ціле	    cs_kill(exarg_T *eap);
static void	    cs_kill_execute(ціле, char *);
static cscmd_T *    cs_lookup_cmd(exarg_T *eap);
static char *	    cs_make_vim_style_matches(char *, char *,
			char *, char *);
static char *	    cs_manage_matches(char **, char **, ціле, mcmd_e);
static void	    cs_print_tags_priv(char **, char **, ціле);
static ціле	    cs_read_prompt(ціле);
static void	    cs_release_csp(ціле, ціле freefnpp);
static ціле	    cs_reset(exarg_T *eap);
static char *	    cs_resolve_file(ціле, char *);
static ціле	    cs_show(exarg_T *eap);


static csinfo_T *   csinfo = NULL;
static ціле	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

static ціле	    eap_arg_len;    // length of eap->arg, set in
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
    (void)semsg(_(e_usage_cscope_str), cs_cmds[(ціле)x].usage);
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
get_cscope_name(expand_T *xp UNUSED, ціле idx)
{
    ціле		current_idx;
    ціле		i;

    switch (expand_what)
    {
    case EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	повернути (char_u *)cs_cmds[idx].name;
    case EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	для (i = 0, current_idx = 0; cs_cmds[i].name != NULL; i++)
	    якщо (cs_cmds[i].cansplit)
		якщо (current_idx++ == idx)
		    break;
	повернути (char_u *)cs_cmds[i].name;
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
	    повернути (char_u *)query_type[idx];
	}
    case EXP_CSCOPE_KILL:
	{
	    static char	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    для (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		якщо (csinfo[i].fname == NULL)
		    continue;
		якщо (current_idx++ == idx)
		{
		    vim_snprintf(connection, sizeof(connection), "%d", i);
		    повернути (char_u *)connection;
		}
	    }
	    повернути (current_idx == idx && idx > 0) ? (char_u *)"-1" : NULL;
	}
    default:
	повернути NULL;
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

    якщо (*arg == NUL)
	повернути;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    якщо (*p == NUL)
	повернути;

    // past first word
    xp->xp_pattern = skipwhite(p);
    якщо (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    інакше якщо (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    інакше якщо (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    інакше якщо (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    інакше
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    static void
do_cscope_general(
    exarg_T	*eap,
    ціле		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    якщо (check_restricted())
	повернути;

    якщо ((cmdp = cs_lookup_cmd(eap)) == NULL)
    {
	cs_help(eap);
	повернути;
    }

    якщо (make_split)
    {
	якщо (!cmdp->cansplit)
	{
	    (void)msg_puts(_("This cscope command does not support splitting the window.\n"));
	    повернути;
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
    ціле ret = FALSE;

    якщо (*eap->arg == NUL)
    {
	(void)emsg(_(e_usage_cstag_ident));
	повернути;
    }

    switch (p_csto)
    {
    case 0 :
	якщо (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (char *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    якщо (ret == FALSE)
	    {
		cs_free_tags();
		якщо (msg_col)
		    msg_putchar('\n');

		якщо (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	інакше якщо (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	break;
    case 1 :
	якщо (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    якщо (ret == FALSE)
	    {
		якщо (msg_col)
		    msg_putchar('\n');

		якщо (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (char *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    якщо (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	інакше якщо (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (char *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    якщо (ret == FALSE)
		cs_free_tags();
	}
	break;
    default :
	break;
    }

    якщо (!ret)
    {
	(void)emsg(_(e_cstag_tag_not_founc));
# якщо defined(FEAT_QUICKFIX)
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
    ціле
cs_fgets(char_u *buf, ціле size)
{
    char *p;

    якщо ((p = cs_manage_matches(NULL, NULL, -1, Get)) == NULL)
	повернути TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    повернути FALSE;
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
# якщо defined(FEAT_EVAL)
    static ціле
cs_connection(ціле num, char_u *dbpath, char_u *ppath)
{
    ціле i;

    якщо (num < 0 || num > 4 || (num > 0 && !dbpath))
	повернути FALSE;

    для (i = 0; i < csinfo_size; i++)
    {
	якщо (!csinfo[i].fname)
	    continue;

	якщо (num == 0)
	    повернути TRUE;

	switch (num)
	{
	case 1:
	    якщо (strstr(csinfo[i].fname, (char *)dbpath))
		повернути TRUE;
	    break;
	case 2:
	    якщо (strcmp(csinfo[i].fname, (char *)dbpath) == 0)
		повернути TRUE;
	    break;
	case 3:
	    якщо (strstr(csinfo[i].fname, (char *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (char *)ppath))))
		повернути TRUE;
	    break;
	case 4:
	    якщо ((strcmp(csinfo[i].fname, (char *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (char *)ppath) == 0))))
		повернути TRUE;
	    break;
	}
    }

    повернути FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    static ціле
cs_add(exarg_T *eap UNUSED)
{
    char *fname, *ppath, *flags = NULL;

    якщо ((fname = strtok((char *)NULL, (const char *)" ")) == NULL)
    {
	cs_usage_msg(Add);
	повернути CSCOPE_FAILURE;
    }
    якщо ((ppath = strtok((char *)NULL, (const char *)" ")) != NULL)
	flags = strtok((char *)NULL, (const char *)" ");

    повернути cs_add_common(fname, ppath, flags);
}

    static void
cs_stat_emsg(char *fname)
{
    ціле err = errno;
    (void)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    static ціле
cs_add_common(
    char *arg1,	    // filename - may contain environment variables
    char *arg2,	    // prepend path - may contain environment variables
    char *flags)
{
    stat_T	statbuf;
    ціле		ret;
    char	*fname = NULL;
    char	*fname2 = NULL;
    char	*ppath = NULL;
    ціле		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = NULL;

    // get the filename (arg1), expand it, and try to stat it
    якщо ((fname = alloc(MAXPATHL + 1)) == NULL)
	goto add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (void)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    якщо (fname == NULL)
	goto add_err;
    fname = (char *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    якщо (ret < 0)
    {
staterr:
	якщо (p_csverbose)
	    cs_stat_emsg(fname);
	goto add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    якщо (arg2 != NULL)
    {
	stat_T	    statbuf2;

	якщо ((ppath = alloc(MAXPATHL + 1)) == NULL)
	    goto add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	якщо (ret < 0)
	    goto staterr;
    }

    // if filename is a directory, append the cscope database name to it
    якщо (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	якщо (fname2 == NULL)
	    goto add_err;

	поки (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    якщо (fname[0] == '\0')
		break;
	}
	якщо (fname[0] == '\0')
	    (void)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	інакше
	    (void)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	якщо (ret < 0)
	{
	    якщо (p_csverbose)
		cs_stat_emsg(fname2);
	    goto add_err;
	}

	i = cs_insert_filelist(fname2, ppath, flags, &statbuf);
    }
    інакше якщо (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_insert_filelist(fname, ppath, flags, &statbuf);
    }
    інакше
    {
	якщо (p_csverbose)
	    (void)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	goto add_err;
    }

    якщо (i != -1)
    {
	якщо (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    goto add_err;
	}

	якщо (p_csverbose)
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
    повернути CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    повернути CSCOPE_FAILURE;
}


    static ціле
cs_check_for_connections(void)
{
    повернути (cs_cnt_connections() > 0);
}


    static ціле
cs_check_for_tags(void)
{
    повернути (p_tags[0] != NUL && curbuf->b_p_tags != NULL);
}


/*
 * Count the number of cscope connections.
 */
    static ціле
cs_cnt_connections(void)
{
    ціле i;
    ціле cnt = 0;

    для (i = 0; i < csinfo_size; i++)
	якщо (csinfo[i].fname != NULL)
	    cnt++;
    повернути cnt;
}

    static void
cs_reading_emsg(
    ціле idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    static ціле
cs_cnt_matches(ціле idx)
{
    char *stok;
    char *buf;
    ціле nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    якщо (buf == NULL)
	повернути 0;
    для (;;)
    {
	якщо (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    якщо (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    повернути -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	якщо (strstr((const char *)buf, "Unable to search database") != NULL)
	    break;
	якщо ((stok = strtok(buf, (const char *)" ")) == NULL)
	    continue;
	якщо (strstr((const char *)stok, "cscope:") == NULL)
	    continue;

	якщо ((stok = strtok(NULL, (const char *)" ")) == NULL)
	    continue;
	nlines = atoi(stok);
	якщо (nlines < 0)
	{
	    nlines = 0;
	    break;
	}

	якщо ((stok = strtok(NULL, (const char *)" ")) == NULL)
	    continue;
	якщо (strncmp((const char *)stok, "lines", 5))
	    continue;

	break;
    }

    vim_free(buf);
    повернути nlines;
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
	повернути NULL;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    якщо (search != 4 && search != 6)
	поки VIM_ISWHITE(*pat)
	    ++pat;

    якщо ((cmd = alloc(strlen(pat) + 2)) == NULL)
	повернути NULL;

    (void)sprintf(cmd, "%d%s", search, pat);

    повернути cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    static ціле
cs_create_connection(ціле i)
{
# ifdef UNIX
    ціле		to_cs[2], from_cs[2];
# endif
    ціле		cmdlen;
    char	*prog, *cmd, *ppath = NULL;
    size_t	proglen;
# ifdef MSWIN
    ціле		fd;
    ціле		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# якщо defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    якщо (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(void)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	якщо (to_cs[0] != -1)
	    (void)close(to_cs[0]);
	якщо (to_cs[1] != -1)
	    (void)close(to_cs[1]);
	якщо (from_cs[0] != -1)
	    (void)close(from_cs[0]);
	якщо (from_cs[1] != -1)
	    (void)close(from_cs[1]);
	повернути CSCOPE_FAILURE;
    }

    якщо ((csinfo[i].pid = fork()) == -1)
    {
	(void)emsg(_(e_could_not_fork_for_cscope));
	goto err_closing;
    }
    інакше якщо (csinfo[i].pid == 0)	// child: run cscope.
    {
	char **argv = NULL;

	якщо (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	якщо (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	якщо (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(void)close(to_cs[1]);
	(void)close(from_cs[0]);
# інакше
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	якщо (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (void)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    якщо (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    якщо (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    повернути CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	якщо ((prog = alloc(MAXPATHL + 1)) == NULL)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# інакше
	    // MSWIN
	    goto err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (ціле)(proglen + strlen(csinfo[i].fname) + 32);
	якщо (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    якщо ((ppath = alloc(MAXPATHL + 1)) == NULL)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# інакше
		// MSWIN
		goto err_closing;
# endif
	    }
	    cmdlen += (ціле)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	якщо (csinfo[i].flags)
	    cmdlen += (ціле)strlen(csinfo[i].flags);

	якщо ((cmd = alloc(cmdlen)) == NULL)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# інакше
	    // MSWIN
	    goto err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    char	**tok = NULL;
	    ціле		tokc = 0;

	    ga_init2(&ga_argv, sizeof(char *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    якщо (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    для (ціле k = 0; k < tokc; ++k)
	    {
		якщо (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((char **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    якщо (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((char **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (char *)vim_strsave((char_u *)"-dl");
	    ((char **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (char *)vim_strsave((char_u *)"-f");
	    ((char **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(char *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    якщо (ppath != NULL)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		char	*parg = alloc(plen);

		якщо (parg == NULL || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((char **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    якщо (csinfo[i].flags != NULL)
	    {
		якщо (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		для (ціле k = 0; k < tokc; ++k)
		{
		    якщо (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((char **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    якщо (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((char **)ga_argv.ga_data)[ga_argv.ga_len] = NULL;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  якщо defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   якщо defined(HAVE_SETSID)
	    (void)setsid();
#   інакше
	    якщо (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (char **)ga_argv.ga_data;
	    якщо (argv[0] == NULL || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    інакше	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	якщо ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == NULL)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	якщо ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == NULL)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(void)close(to_cs[0]);
	(void)close(from_cs[1]);
    }
# інакше
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    якщо (csinfo[i].ppath != NULL)
    {
	len = (ціле)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    якщо (csinfo[i].flags != NULL)
    {
	len = (ціле)STRLEN(cmd);
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

    якщо (!created)
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
    якщо (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == NULL))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    якщо (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == NULL))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    повернути CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    static ціле
cs_find(exarg_T *eap)
{
    char *opt, *pat;
    ціле i;

    якщо (cs_check_for_connections() == FALSE)
    {
	(void)emsg(_(e_no_cscope_connections));
	повернути FALSE;
    }

    якщо ((opt = strtok((char *)NULL, (const char *)" ")) == NULL)
    {
	cs_usage_msg(Find);
	повернути FALSE;
    }

    pat = opt + strlen(opt) + 1;
    якщо (pat >= (char *)eap->arg + eap_arg_len)
    {
	cs_usage_msg(Find);
	повернути FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    для (i = 0; i < eap_arg_len; ++i)
	якщо (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    повернути cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    static ціле
cs_find_common(
    char *opt,
    char *pat,
    ціле forceit,
    ціле verbose,
    ціле	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    ціле i;
    char *cmd;
    ціле *nummatches;
    ціле totmatches;
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
    якщо (qfpos != NULL)
    {
	qfpos++;
	// next symbol must be + or -
	якщо (strchr(CSQF_FLAGS, *qfpos) == NULL)
	{
	    (void)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    повернути FALSE;
	}

	якщо (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    якщо (aborting())
		повернути FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    якщо (cmd == NULL)
	повернути FALSE;

    nummatches = ALLOC_MULT(ціле, csinfo_size);
    якщо (nummatches == NULL)
    {
	vim_free(cmd);
	повернути FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    для (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    для (i = 0; i < csinfo_size; i++)
    {
	якщо (csinfo[i].fname == NULL || csinfo[i].to_fp == NULL)
	    continue;

	// send cmd to cscope
	(void)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(void)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	якщо (nummatches[i] > -1)
	    totmatches += nummatches[i];

	якщо (nummatches[i] == 0)
	    (void)cs_read_prompt(i);
    }
    vim_free(cmd);

    якщо (totmatches == 0)
    {
	якщо (verbose)
	    (void)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	повернути FALSE;
    }

# ifdef FEAT_QUICKFIX
    якщо (qfpos != NULL && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = NULL;
	win_T	    *wp = NULL;

	якщо (tmp == NULL)
	{
	    vim_free(nummatches);
	    повернути FALSE;
	}

	f = mch_fopen((char *)tmp, "w");
	якщо (f == NULL)
	    semsg(_(e_cant_open_file_str), tmp);
	інакше
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    якщо (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    якщо (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, NULL) > 0)
	    {
		якщо (postponed_split != 0)
		{
		    (void)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		якщо (use_ll)
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
	повернути TRUE;
    }
    інакше
# endif // FEAT_QUICKFIX
    {
	char **matches = NULL, **contexts = NULL;
	ціле matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	якщо (matches == NULL)
	    повернути FALSE;

	(void)cs_manage_matches(matches, contexts, matched, Store);

	повернути do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    static ціле
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (void)msg_puts(_("cscope commands:\n"));
    поки (cmdp->name != NULL)
    {
	char *help = _(cmdp->help);
	ціле  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	якщо (space_cnt < 0)
	    space_cnt = 0;
	(void)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	якщо (strcmp(cmdp->name, "find") == 0)
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
    повернути 0;
}


    static void
clear_csinfo(ціле i)
{
    csinfo[i].fname  = NULL;
    csinfo[i].ppath  = NULL;
    csinfo[i].flags  = NULL;
# якщо defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# інакше
    csinfo[i].nVolume = 0;
    csinfo[i].nIndexHigh = 0;
    csinfo[i].nIndexLow = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = NULL;
    csinfo[i].to_fp  = NULL;
# якщо defined(MSWIN)
    csinfo[i].hProc = NULL;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    static ціле
cs_insert_filelist(
    char *fname,
    char *ppath,
    char *flags,
    stat_T *sb UNUSED)
{
    ціле	    i;
    ціле	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    switch (win32_fileinfo((char_u *)fname, &bhfi))
    {
	case FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	case FILEINFO_READ_FAIL:	// CreateFile() failed
	    якщо (p_csverbose)
	    {
		char *cant_msg = _(e_cannot_open_cscope_database_str);
		char *winmsg = GetWin32Error();

		якщо (winmsg != NULL)
		    (void)semsg(cant_msg, winmsg);
		інакше
		    // subst filename if can't get error text
		    (void)semsg(cant_msg, fname);
	    }
	    повернути -1;

	case FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    якщо (p_csverbose)
		(void)emsg(_(e_cannot_get_cscope_database_information));
	    повернути -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    для (j = 0; j < csinfo_size; j++)
    {
	якщо (csinfo[j].fname != NULL
# якщо defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# інакше
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
	    якщо (p_csverbose)
		(void)emsg(_(e_duplicate_cscope_database_not_added));
	    повернути -1;
	}

	якщо (csinfo[j].fname == NULL && i == -1)
	    i = j; // remember first empty entry
    }

    якщо (i == -1)
    {
	i = csinfo_size;
	якщо (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	інакше
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, sizeof(csinfo_T)*csinfo_size);
	    якщо (csinfo == NULL)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		повернути -1;
	    }
	}
	якщо (csinfo == NULL)
	    повернути -1;
	для (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    якщо ((csinfo[i].fname = alloc(strlen(fname)+1)) == NULL)
	повернути -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((const char *)fname));

    якщо (ppath != NULL)
    {
	якщо ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == NULL)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    повернути -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((const char *)ppath));
    }
    інакше
	csinfo[i].ppath = NULL;

    якщо (flags != NULL)
    {
	якщо ((csinfo[i].flags = alloc(strlen(flags) + 1)) == NULL)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    повернути -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((const char *)flags));
    }
    інакше
	csinfo[i].flags = NULL;

# якщо defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# інакше
    csinfo[i].nVolume = bhfi.dwVolumeSerialNumber;
    csinfo[i].nIndexLow = bhfi.nFileIndexLow;
    csinfo[i].nIndexHigh = bhfi.nFileIndexHigh;
# endif
    повернути i;
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

    якщо (eap->arg == NULL)
	повернути NULL;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (ціле)STRLEN(eap->arg);

    якщо ((stok = strtok((char *)(eap->arg), (const char *)" ")) == NULL)
	повернути NULL;

    len = strlen(stok);
    для (cmdp = cs_cmds; cmdp->name != NULL; ++cmdp)
    {
	якщо (strncmp((const char *)(stok), cmdp->name, len) == 0)
	    повернути (cmdp);
    }
    повернути NULL;
}


/*
 * Nuke em.
 */
    static ціле
cs_kill(exarg_T *eap UNUSED)
{
    char *stok;
    ціле i;

    якщо ((stok = strtok((char *)NULL, (const char *)" ")) == NULL)
    {
	cs_usage_msg(Kill);
	повернути CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    якщо ((strlen(stok) < 2 && VIM_ISDIGIT((ціле)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((ціле)(stok[1]))))
	i = atoi(stok);
    інакше
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	для (i = 0; i < csinfo_size; i++)
	{
	    якщо (csinfo[i].fname != NULL && strstr(csinfo[i].fname, stok))
		break;
	}
    }

    якщо ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == NULL))
    {
	якщо (p_csverbose)
	    (void)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    інакше
    {
	якщо (i == -1)
	{
	    для (i = 0; i < csinfo_size; i++)
	    {
		якщо (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	інакше
	    cs_kill_execute(i, stok);
    }

    повернути 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    static void
cs_kill_execute(
    ціле i,	    // cscope table index
    char *cname)    // cscope database name
{
    якщо (p_csverbose)
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
    ціле amt;

    якщо (search != NULL)
    {
	amt = (ціле)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(search)+6);
	якщо ((buf = alloc(amt)) == NULL)
	    повернути NULL;

	(void)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, search);
    }
    інакше
    {
	amt = (ціле)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	якщо ((buf = alloc(amt)) == NULL)
	    повернути NULL;

	(void)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    повернути buf;
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
    ціле totmatches,
    mcmd_e cmd)
{
    static char **mp = NULL;
    static char **cp = NULL;
    static ціле cnt = -1;
    static ціле next = -1;
    char *p = NULL;

    switch (cmd)
    {
    case Store:
	assert(matches != NULL);
	assert(totmatches > 0);
	якщо (mp != NULL || cp != NULL)
	    (void)cs_manage_matches(NULL, NULL, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	break;
    case Get:
	якщо (next >= cnt)
	    повернути NULL;

	p = mp[next];
	next++;
	break;
    case Free:
	якщо (mp != NULL)
	{
	    якщо (cnt > 0)
		поки (cnt--)
		{
		    vim_free(mp[cnt]);
		    якщо (cp != NULL)
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
	повернути NULL;
    }

    повернути p;
}


/*
 * Parse cscope output.
 */
    static char *
cs_parse_results(
    ціле cnumber,
    char *buf,
    ціле bufsize,
    char **context,
    char **linenumber,
    char **search)
{
    ціле ch;
    char *p;
    char *name;

    якщо (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == NULL)
    {
	якщо (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	повернути NULL;
    }

    // If the line's too long for the buffer, discard it.
    якщо ((p = strchr(buf, '\n')) == NULL)
    {
	поки ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	повернути NULL;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    якщо ((name = strtok(buf, (const char *)" ")) == NULL)
	повернути NULL;
    якщо ((*context = strtok(NULL, (const char *)" ")) == NULL)
	повернути NULL;
    якщо ((*linenumber = strtok(NULL, (const char *)" ")) == NULL)
	повернути NULL;
    *search = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    якщо (strcmp(*search, "<unknown>") == 0)
	*search = NULL;

    name = cs_resolve_file(cnumber, name);
    повернути name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    static void
cs_file_results(FILE *f, ціле *nummatches_a)
{
    ціле i, j;
    char *buf;
    char *search, *slno;
    char *fullname;
    char *cntx;
    char *context;

    buf = alloc(CSREAD_BUFSIZE);
    якщо (buf == NULL)
	повернути;

    для (i = 0; i < csinfo_size; i++)
    {
	якщо (nummatches_a[i] < 1)
	    continue;

	для (j = 0; j < nummatches_a[i]; j++)
	{
	   якщо ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &search)) == NULL)
	       continue;

	   context = alloc(strlen(cntx)+5);
	   якщо (context == NULL)
	   {
	       vim_free(fullname);
	       continue;
	   }

	   якщо (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   інакше
	       sprintf(context, "<<%s>>", cntx);

	   якщо (search == NULL)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   інакше
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
    ціле totmatches,
    ціле *nummatches_a,
    char ***matches_p,
    char ***cntxts_p,
    ціле *matched)
{
    ціле i, j;
    char *buf;
    char *search, *slno;
    ціле totsofar = 0;
    char **matches = NULL;
    char **cntxts = NULL;
    char *fullname;
    char *cntx;

    assert(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    якщо (buf == NULL)
	повернути;

    якщо ((matches = ALLOC_MULT(char *, totmatches)) == NULL)
	goto parse_out;
    якщо ((cntxts = ALLOC_MULT(char *, totmatches)) == NULL)
	goto parse_out;

    для (i = 0; i < csinfo_size; i++)
    {
	якщо (nummatches_a[i] < 1)
	    continue;

	для (j = 0; j < nummatches_a[i]; j++)
	{
	   якщо ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &search)) == NULL)
		continue;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  search, tagstr);

	    vim_free(fullname);

	    якщо (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = NULL;
	    інакше
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (char *)vim_strsave((char_u *)cntx);

	    якщо (matches[totsofar] != NULL)
		totsofar++;

	} // for all matches

	(void)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    якщо (totsofar == 0)
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
    ціле		i;
    char	*s;

    якщо (p_cspc == 0)
	повернути path;

    s = path + strlen(path) - 1;
    для (i = 0; i < p_cspc; ++i)
    {
	поки (s > path)
	{
	   s--;
	   якщо (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      break;
	}
    }
    якщо ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    повернути s;
}

/*
 * Called from cs_manage_matches().
 */
    static void
cs_print_tags_priv(char **matches, char **cntxts, ціле num_matches)
{
    char	*buf = NULL;
    char	*t_buf;
    ціле		bufsize = 0; // Track available bufsize
    ціле		newsize = 0;
    char	*ptag;
    char	*fname, *lno, *extra, *tbuf;
    ціле		i, idx, num;
    char	*globalcntx = "GLOBAL";
    char	*cntxformat = " <<%s>>";
    char	*context;
    char	*cstag_msg = _("Cscope tag: %s");
    char	*csfmt_str = "%4d %6s  ";

    assert(num_matches > 0);

    якщо ((tbuf = alloc(strlen(matches[0]) + 1)) == NULL)
	повернути;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    якщо (ptag == NULL)
    {
	vim_free(tbuf);
	повернути;
    }

    newsize = (ціле)(strlen(cstag_msg) + strlen(ptag));
    buf = alloc(newsize);
    якщо (buf != NULL)
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
    для (i = 0; i < num_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	якщо ((tbuf = alloc(strlen(matches[idx]) + 1)) == NULL)
	    continue;
	(void)strcpy(tbuf, matches[idx]);

	якщо (strtok(tbuf, (const char *)"\t") == NULL
		|| (fname = strtok(NULL, (const char *)"\t")) == NULL
		|| (lno = strtok(NULL, (const char *)"\t")) == NULL)
	{
	    vim_free(tbuf);
	    continue;
	}
	extra = strtok(NULL, (const char *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (ціле)(strlen(csfmt_str) + 16 + strlen(lno));
	якщо (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    якщо (buf == NULL)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    інакше
		bufsize = newsize;
	}
	якщо (buf != NULL)
	{
	    // csfmt_str = "%4d %6s  ";
	    (void)sprintf(buf, csfmt_str, num, lno);
	    msg_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	msg_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	якщо (cntxts[idx] != NULL)
	    context = cntxts[idx];
	інакше
	    context = globalcntx;
	newsize = (ціле)(strlen(context) + strlen(cntxformat));

	якщо (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    якщо (buf == NULL)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    інакше
		bufsize = newsize;
	}
	якщо (buf != NULL)
	{
	    (void)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    якщо (msg_col + (ціле)strlen(buf) >= cmdline_width)
		msg_putchar('\n');
	    msg_advance(12);
	    msg_outtrans_long_attr((char_u *)buf, 0);
	    msg_putchar('\n');
	}
	якщо (extra != NULL)
	{
	    msg_advance(13);
	    msg_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	якщо (msg_col)
	    msg_putchar('\n');

	ui_breakcheck();
	якщо (got_int)
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
    static ціле
cs_read_prompt(ціле i)
{
    ціле		ch;
    char	*buf = NULL; // buffer for possible error message from cscope
    ціле		bufpos = 0;
    ціле		maxlen;
    static char *eprompt = "Press the RETURN key to continue:";
    ціле		epromptlen = (ціле)strlen(eprompt);
    ціле		n;

    // compute maximum allowed len for Cscope error message
    char *cs_emsg = _(e_cscope_error_str);
    maxlen = (ціле)(IOSIZE - strlen(cs_emsg));

    для (;;)
    {
	поки ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    якщо (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		якщо (buf == NULL) // lazy buffer allocation
		    buf = alloc(maxlen);
		якщо (buf != NULL)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    якщо (bufpos >= epromptlen
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

	для (n = 0; n < (ціле)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    якщо (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    якщо (ch == EOF)
	    {
		якщо (buf != NULL && buf[0] != NUL)
		    (void)semsg(cs_emsg, buf);
		інакше якщо (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		повернути CSCOPE_FAILURE;
	    }

	    якщо (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		break;
	    }
	}

	якщо (ch == EOF)
	    continue;	    // didn't find the prompt
	break;		    // did find the prompt
    }

    vim_free(buf);
    повернути CSCOPE_SUCCESS;
}

# якщо defined(UNIX) && defined(SIGALRM)
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
cs_release_csp(ціле i, ціле freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    якщо (csinfo[i].to_fp != NULL)
    {
	(void)fputs("q\n", csinfo[i].to_fp);
	(void)fflush(csinfo[i].to_fp);
    }
# якщо defined(UNIX)
    {
	ціле waitpid_errno;
	ціле pstat;
	pid_t pid;

#  якщо defined(HAVE_SIGACTION)
	struct sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   інакше
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
#  інакше
	ціле waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	для (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    якщо (pid != 0)
		break;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	якщо (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    ціле alive = TRUE;

	    якщо (waitpid_errno == ECHILD)
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
		ціле waited;

		sleep(0);
		для (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    якщо (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			break;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    якщо (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(void)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# інакше  // !UNIX
    якщо (csinfo[i].hProc != NULL)
    {
	// Give cscope a chance to exit normally
	якщо (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    якщо (csinfo[i].fr_fp != NULL)
	(void)fclose(csinfo[i].fr_fp);
    якщо (csinfo[i].to_fp != NULL)
	(void)fclose(csinfo[i].to_fp);

    якщо (freefnpp)
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
    static ціле
cs_reset(exarg_T *eap UNUSED)
{
    char	**dblist = NULL, **pplist = NULL, **fllist = NULL;
    ціле	i;
    char buf[20]; // for sprintf " (#%d)"

    якщо (csinfo_size == 0)
	повернути CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(char *, csinfo_size);
    pplist = ALLOC_MULT(char *, csinfo_size);
    fllist = ALLOC_MULT(char *, csinfo_size);
    якщо (dblist == NULL || pplist == NULL || fllist == NULL)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	повернути CSCOPE_FAILURE;
    }

    для (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	якщо (csinfo[i].fname != NULL)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    для (i = 0; i < csinfo_size; i++)
    {
	якщо (dblist[i] != NULL)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    якщо (p_csverbose)
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

    якщо (p_csverbose)
	msg_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | MSG_HIST);
    повернути CSCOPE_SUCCESS;
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
cs_resolve_file(ціле i, char *name)
{
    char	*fullname;
    string_T	csdir = {NULL, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    ціле		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (ціле)namelen + 2;
    якщо (csinfo[i].ppath != NULL)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (ціле)ppathlen;
    }
    інакше якщо (p_csre && csinfo[i].fname != NULL)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	якщо (csdir.string != NULL)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (ціле)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    якщо (csinfo[i].ppath != NULL
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	якщо ((fullname = alloc(len)) != NULL)
	    (void)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    інакше якщо (csdir.string != NULL && csinfo[i].fname != NULL && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (char *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    інакше
    {
	fullname = (char *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    повернути fullname;
}


/*
 * Show all cscope connections.
 */
    static ціле
cs_show(exarg_T *eap UNUSED)
{
    ціле i;

    якщо (cs_cnt_connections() == 0)
	msg_puts(_("no cscope connections\n"));
    інакше
    {
	msg_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	для (i = 0; i < csinfo_size; i++)
	{
	    якщо (csinfo[i].fname == NULL)
		continue;

	    якщо (csinfo[i].ppath != NULL)
		(void)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    інакше
		(void)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    повернути CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    void
cs_end(void)
{
    ціле i;

    для (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#якщо defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    void
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    ціле		num = 0;
    char_u	*dbpath = NULL;
    char_u	*prepend = NULL;
    char_u	buf[NUMBUFLEN];

    якщо (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	повернути;

    якщо (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	num = (ціле)tv_get_number(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	якщо (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_number = cs_connection(num, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
