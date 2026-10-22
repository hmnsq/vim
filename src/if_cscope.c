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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_CSCOPE)

# include <sys/types.h>
# include <sys/stat.h>
# ଯଦି defined(UNIX)
#  include <sys/wait.h>
# endif

# ଯଦି defined (MSWIN)
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଅକ୍ଷର *  name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     (*func)(exarg_T *eap);
    ଅକ୍ଷର *  help;
    ଅକ୍ଷର *  usage;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cansplit;		// if supports splitting window
} cscmd_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା csi {
    ଅକ୍ଷର *	    fname;	// cscope db name
    ଅକ୍ଷର *	    ppath;	// path to prepend (the -P option)
    ଅକ୍ଷର *	    flags;	// additional cscope flags/options (e.g, -p2)
# ଯଦି defined(UNIX)
    pid_t	    pid;	// PID of the connected cscope process.
    dev_t	    st_dev;	// ID of dev containing cscope db
    ino_t	    st_ino;	// inode number of cscope db
# ଅନ୍ୟଥା
#  ଯଦି defined(MSWIN)
    DWORD	    pid;	// PID of the connected cscope process.
    HANDLE	    hProc;	// cscope process handle
    DWORD	    nVolume;	// Volume serial number, instead of st_dev
    DWORD	    n_Index_ଉପର;	// st_ino has no meaning in the Windows
    DWORD	    n_Index_ତଳ;
#  endif
# endif

    FILE *	    fr_fp;	// from cscope: FILE.
    FILE *	    to_fp;	// to cscope: FILE.
} csinfo_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା { Add, Find, Help, Kill, Reset, Show } csid_e;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    Store,
    Get,
    Free,
    Print
} mcmd_e;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_add(exarg_T *eap);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_add_common(ଅକ୍ଷର *, ଅକ୍ଷର *, ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_check_for_connections(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_check_for_tags(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_cnt_connections(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_create_connection(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i);
# ifdef FEAT_QUICKFIX
ସ୍ଥିର ଶୂନ୍ୟ	    cs_file_results(FILE *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *);
# endif
ସ୍ଥିର ଶୂନ୍ୟ	    cs_fill_results(ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା , ପୂର୍ଣ୍ଣସଂଖ୍ୟା *, ଅକ୍ଷର ***,
			ଅକ୍ଷର ***, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_find(exarg_T *eap);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_find_common(ଅକ୍ଷର *opt, ଅକ୍ଷର *pat, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, char_u *cmdline);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_help(exarg_T *eap);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_ଭର୍ତ୍ତି_filelist(ଅକ୍ଷର *, ଅକ୍ଷର *, ଅକ୍ଷର *,
			stat_T *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_kill(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	    cs_kill_execute(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅକ୍ଷର *);
ସ୍ଥିର cscmd_T *    cs_lookup_cmd(exarg_T *eap);
ସ୍ଥିର ଅକ୍ଷର *	    cs_make_vim_style_matches(ଅକ୍ଷର *, ଅକ୍ଷର *,
			ଅକ୍ଷର *, ଅକ୍ଷର *);
ସ୍ଥିର ଅକ୍ଷର *	    cs_manage_matches(ଅକ୍ଷର **, ଅକ୍ଷର **, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, mcmd_e);
ସ୍ଥିର ଶୂନ୍ୟ	    cs_print_tags_priv(ଅକ୍ଷର **, ଅକ୍ଷର **, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_read_prompt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଶୂନ୍ୟ	    cs_release_csp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା freefnpp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_reset(exarg_T *eap);
ସ୍ଥିର ଅକ୍ଷର *	    cs_resolve_file(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cs_show(exarg_T *eap);


ସ୍ଥିର csinfo_T *   csinfo = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    csinfo_size = 0;	// number of items allocated in
					// csinfo[]

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    eap_arg_len;    // length of eap->arg, set in
				    // cs_lookup_cmd()
ସ୍ଥିର cscmd_T	    cs_cmds[] =
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
    { ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, 0 }
};

    ସ୍ଥିର ଶୂନ୍ୟ
cs_usage_ସନ୍ଦେଶ(csid_e x)
{
    (ଶୂନ୍ୟ)semsg(_(e_usage_cscope_str), cs_cmds[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)x].usage);
}

ସ୍ଥିର ଗଣନା
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
get_cscope_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ବାଛ (expand_what)
    {
    ସ୍ଥିତି EXP_CSCOPE_SUBCMD:
	// Complete with sub-commands of ":cscope":
	// add, find, help, kill, reset, show
	ଫେରନ୍ତୁ (char_u *)cs_cmds[idx].name;
    ସ୍ଥିତି EXP_SCSCOPE_SUBCMD:
	// Complete with sub-commands of ":scscope": same sub-commands as
	// ":cscope" but skip commands which don't support split windows
	ପାଇଁ (i = 0, current_idx = 0; cs_cmds[i].name != ଖାଲି; i++)
	    ଯଦି (cs_cmds[i].cansplit)
		ଯଦି (current_idx++ == idx)
		    ଭାଙ୍ଗ;
	ଫେରନ୍ତୁ (char_u *)cs_cmds[i].name;
    ସ୍ଥିତି EXP_CSCOPE_FIND:
	{
	    ଅଚଳ ଅକ୍ଷର *query_type[] =
	    {
		"a", "c", "d", "e", "f", "g", "i", "s", "t", ଖାଲି
	    };

	    // Complete with query type of ":cscope find {query_type}".
	    // {query_type} can be letters (c, d, ... a) or numbers (0, 1,
	    // ..., 9) but only complete with letters, since numbers are
	    // redundant.
	    ଫେରନ୍ତୁ (char_u *)query_type[idx];
	}
    ସ୍ଥିତି EXP_CSCOPE_KILL:
	{
	    ସ୍ଥିର ଅକ୍ଷର	connection[5];

	    // ":cscope kill" accepts connection numbers or partial names of
	    // the pathname of the cscope database as argument.  Only complete
	    // with connection numbers. -1 can also be used to kill all
	    // connections.
	    ପାଇଁ (i = 0, current_idx = 0; i < csinfo_size; i++)
	    {
		ଯଦି (csinfo[i].fname == ଖାଲି)
		    ଜାରି;
		ଯଦି (current_idx++ == idx)
		{
		    vim_snprintf(connection, ଆକାର(connection), "%d", i);
		    ଫେରନ୍ତୁ (char_u *)connection;
		}
	    }
	    ଫେରନ୍ତୁ (current_idx == idx && idx > 0) ? (char_u *)"-1" : ଖାଲି;
	}
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ଫେରନ୍ତୁ ଖାଲି;
    }
}

/*
 * Handle command line completion for :cscope command.
 */
    ଶୂନ୍ୟ
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

    ଯଦି (*arg == NUL)
	ଫେରନ୍ତୁ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ;

    // past first word
    xp->xp_pattern = skipwhite(p);
    ଯଦି (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "add", p - arg) == 0)
	xp->xp_context = EXPAND_FILES;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "kill", p - arg) == 0)
	expand_what = EXP_CSCOPE_KILL;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "find", p - arg) == 0)
	expand_what = EXP_CSCOPE_FIND;
    ଅନ୍ୟଥା
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Find the command, print help if invalid, and then call the corresponding
 * command function.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_cscope_general(
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		make_split UNUSED) // whether to split window
{
    cscmd_T *cmdp;

    ଯଦି (check_restricted())
	ଫେରନ୍ତୁ;

    ଯଦି ((cmdp = cs_lookup_cmd(eap)) == ଖାଲି)
    {
	cs_help(eap);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (make_split)
    {
	ଯଦି (!cmdp->cansplit)
	{
	    (ଶୂନ୍ୟ)ସନ୍ଦେଶ_puts(_("This cscope command does not support splitting the window.\n"));
	    ଫେରନ୍ତୁ;
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
    ଶୂନ୍ୟ
ex_cscope(exarg_T *eap)
{
    do_cscope_general(eap, FALSE);
}

/*
 * Implementation of ":scscope". Same as ex_cscope(), but splits window, too.
 */
    ଶୂନ୍ୟ
ex_scscope(exarg_T *eap)
{
    do_cscope_general(eap, TRUE);
}

/*
 * Implementation of ":cstag"
 */
    ଶୂନ୍ୟ
ex_cstag(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = FALSE;

    ଯଦି (*eap->arg == NUL)
    {
	(ଶୂନ୍ୟ)emsg(_(e_usage_cstag_ident));
	ଫେରନ୍ତୁ;
    }

    ବାଛ (p_csto)
    {
    ସ୍ଥିତି 0 :
	ଯଦି (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (ଅକ୍ଷର *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ଯଦି (ret == FALSE)
	    {
		cs_free_tags();
		ଯଦି (ସନ୍ଦେଶ_col)
		    ସନ୍ଦେଶ_putchar('\n');

		ଯଦି (cs_check_for_tags())
		    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	}
	ଭାଙ୍ଗ;
    ସ୍ଥିତି 1 :
	ଯଦି (cs_check_for_tags())
	{
	    ret = do_tag(eap->arg, DT_JUMP, 0, eap->forceit, FALSE);
	    ଯଦି (ret == FALSE)
	    {
		ଯଦି (ସନ୍ଦେଶ_col)
		    ସନ୍ଦେଶ_putchar('\n');

		ଯଦି (cs_check_for_connections())
		{
		    ret = cs_find_common("g", (ଅକ୍ଷର *)(eap->arg), eap->forceit,
						FALSE, FALSE, *eap->cmdlinep);
		    ଯଦି (ret == FALSE)
			cs_free_tags();
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (cs_check_for_connections())
	{
	    ret = cs_find_common("g", (ଅକ୍ଷର *)(eap->arg), eap->forceit, FALSE,
						       FALSE, *eap->cmdlinep);
	    ଯଦି (ret == FALSE)
		cs_free_tags();
	}
	ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ :
	ଭାଙ୍ଗ;
    }

    ଯଦି (!ret)
    {
	(ଶୂନ୍ୟ)emsg(_(e_cstag_tag_not_founc));
# ଯଦି defined(FEAT_QUICKFIX)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_fgets(char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା size)
{
    ଅକ୍ଷର *p;

    ଯଦି ((p = cs_manage_matches(ଖାଲି, ଖାଲି, -1, Get)) == ଖାଲି)
	ଫେରନ୍ତୁ TRUE;
    vim_strncpy(buf, (char_u *)p, size - 1);

    ଫେରନ୍ତୁ FALSE;
}


/*
 * Called only from do_tag(), when popping the tag stack.
 */
    ଶୂନ୍ୟ
cs_free_tags(ଶୂନ୍ୟ)
{
    cs_manage_matches(ଖାଲି, ଖାଲି, -1, Free);
}


/*
 * Called from do_tag().
 */
    ଶୂନ୍ୟ
cs_print_tags(ଶୂନ୍ୟ)
{
    cs_manage_matches(ଖାଲି, ଖାଲି, -1, Print);
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
# ଯଦି defined(FEAT_EVAL)
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_connection(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ, char_u *dbpath, char_u *ppath)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (ଅଙ୍କ < 0 || ଅଙ୍କ > 4 || (ଅଙ୍କ > 0 && !dbpath))
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (i = 0; i < csinfo_size; i++)
    {
	ଯଦି (!csinfo[i].fname)
	    ଜାରି;

	ଯଦି (ଅଙ୍କ == 0)
	    ଫେରନ୍ତୁ TRUE;

	ବାଛ (ଅଙ୍କ)
	{
	ସ୍ଥିତି 1:
	    ଯଦି (strstr(csinfo[i].fname, (ଅକ୍ଷର *)dbpath))
		ଫେରନ୍ତୁ TRUE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 2:
	    ଯଦି (strcmp(csinfo[i].fname, (ଅକ୍ଷର *)dbpath) == 0)
		ଫେରନ୍ତୁ TRUE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 3:
	    ଯଦି (strstr(csinfo[i].fname, (ଅକ୍ଷର *)dbpath)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && strstr(csinfo[i].ppath, (ଅକ୍ଷର *)ppath))))
		ଫେରନ୍ତୁ TRUE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 4:
	    ଯଦି ((strcmp(csinfo[i].fname, (ଅକ୍ଷର *)dbpath) == 0)
		    && ((!ppath && !csinfo[i].ppath)
			|| (ppath
			    && csinfo[i].ppath
			    && (strcmp(csinfo[i].ppath, (ଅକ୍ଷର *)ppath) == 0))))
		ଫେରନ୍ତୁ TRUE;
	    ଭାଙ୍ଗ;
	}
    }

    ଫେରନ୍ତୁ FALSE;
}

# endif


/*
 * PRIVATE functions
 ****************************************************************************/

/*
 * Add cscope database or a directory name (to look for cscope.out)
 * to the cscope connection list.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_add(exarg_T *eap UNUSED)
{
    ଅକ୍ଷର *fname, *ppath, *flags = ଖାଲି;

    ଯଦି ((fname = strtok((ଅକ୍ଷର *)ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)" ")) == ଖାଲି)
    {
	cs_usage_ସନ୍ଦେଶ(Add);
	ଫେରନ୍ତୁ CSCOPE_FAILURE;
    }
    ଯଦି ((ppath = strtok((ଅକ୍ଷର *)ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)" ")) != ଖାଲି)
	flags = strtok((ଅକ୍ଷର *)ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)" ");

    ଫେରନ୍ତୁ cs_add_common(fname, ppath, flags);
}

    ସ୍ଥିର ଶୂନ୍ୟ
cs_stat_emsg(ଅକ୍ଷର *fname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = errno;
    (ଶୂନ୍ୟ)semsg(_(e_stat_str_error_nr), fname, err);
}


/*
 * The common routine to add a new cscope connection.  Called by
 * cs_add() and cs_reset().  I really don't like to do this, but this
 * routine uses a number of goto statements.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_add_common(
    ଅକ୍ଷର *arg1,	    // filename - may contain environment variables
    ଅକ୍ଷର *arg2,	    // prepend path - may contain environment variables
    ଅକ୍ଷର *flags)
{
    stat_T	statbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    ଅକ୍ଷର	*fname = ଖାଲି;
    ଅକ୍ଷର	*fname2 = ଖାଲି;
    ଅକ୍ଷର	*ppath = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    size_t	len;
    size_t	usedlen = 0;
    char_u	*fbuf = ଖାଲି;

    // get the filename (arg1), expand it, and try to stat it
    ଯଦି ((fname = alloc(MAXPATHL + 1)) == ଖାଲି)
	ଯାଅ add_err;

    len = expand_env((char_u *)arg1, (char_u *)fname, MAXPATHL);
    fbuf = (char_u *)fname;
    (ଶୂନ୍ୟ)modify_fname((char_u *)":p", FALSE, &usedlen,
					      (char_u **)&fname, &fbuf, &len);
    ଯଦି (fname == ଖାଲି)
	ଯାଅ add_err;
    fname = (ଅକ୍ଷର *)vim_strnsave((char_u *)fname, len);
    vim_free(fbuf);

    ret = mch_stat(fname, &statbuf);
    ଯଦି (ret < 0)
    {
staterr:
	ଯଦି (p_csverbose)
	    cs_stat_emsg(fname);
	ଯାଅ add_err;
    }

    // get the prepend path (arg2), expand it, and try to stat it
    ଯଦି (arg2 != ଖାଲି)
    {
	stat_T	    statbuf2;

	ଯଦି ((ppath = alloc(MAXPATHL + 1)) == ଖାଲି)
	    ଯାଅ add_err;

	expand_env((char_u *)arg2, (char_u *)ppath, MAXPATHL);
	ret = mch_stat(ppath, &statbuf2);
	ଯଦି (ret < 0)
	    ଯାଅ staterr;
    }

    // if filename is a directory, append the cscope database name to it
    ଯଦି (S_ISDIR(statbuf.st_mode))
    {
	fname2 = alloc(STRLEN_LITERAL(CSCOPE_DBFILE) + strlen(fname) + 2);
	ଯଦି (fname2 == ଖାଲି)
	    ଯାଅ add_err;

	ଯେତେବେଳେ (fname[strlen(fname)-1] == '/'
# ifdef MSWIN
		|| fname[strlen(fname)-1] == '\\'
# endif
		)
	{
	    fname[strlen(fname)-1] = '\0';
	    ଯଦି (fname[0] == '\0')
		ଭାଙ୍ଗ;
	}
	ଯଦି (fname[0] == '\0')
	    (ଶୂନ୍ୟ)sprintf(fname2, "/%s", CSCOPE_DBFILE);
	ଅନ୍ୟଥା
	    (ଶୂନ୍ୟ)sprintf(fname2, "%s/%s", fname, CSCOPE_DBFILE);

	ret = mch_stat(fname2, &statbuf);
	ଯଦି (ret < 0)
	{
	    ଯଦି (p_csverbose)
		cs_stat_emsg(fname2);
	    ଯାଅ add_err;
	}

	i = cs_ଭର୍ତ୍ତି_filelist(fname2, ppath, flags, &statbuf);
    }
    ଅନ୍ୟଥା ଯଦି (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
	i = cs_ଭର୍ତ୍ତି_filelist(fname, ppath, flags, &statbuf);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (p_csverbose)
	    (ଶୂନ୍ୟ)semsg(_(e_str_is_not_directory_or_valid_cscope_database),
									fname);
	ଯାଅ add_err;
    }

    ଯଦି (i != -1)
    {
	ଯଦି (cs_create_connection(i) == CSCOPE_FAILURE
		|| cs_read_prompt(i) == CSCOPE_FAILURE)
	{
	    cs_release_csp(i, TRUE);
	    ଯାଅ add_err;
	}

	ଯଦି (p_csverbose)
	{
	    ସନ୍ଦେଶ_clr_eos();
	    (ଶୂନ୍ୟ)smsg_attr(HL_ATTR(HLF_R),
			    _("Added cscope database %s"),
			    csinfo[i].fname);
	}
    }

    vim_free(fname);
    vim_free(fname2);
    vim_free(ppath);
    ଫେରନ୍ତୁ CSCOPE_SUCCESS;

add_err:
    vim_free(fname2);
    vim_free(fname);
    vim_free(ppath);
    ଫେରନ୍ତୁ CSCOPE_FAILURE;
}


    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_check_for_connections(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (cs_cnt_connections() > 0);
}


    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_check_for_tags(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (p_tags[0] != NUL && curbuf->b_p_tags != ଖାଲି);
}


/*
 * Count the number of cscope connections.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_cnt_connections(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt = 0;

    ପାଇଁ (i = 0; i < csinfo_size; i++)
	ଯଦି (csinfo[i].fname != ଖାଲି)
	    cnt++;
    ଫେରନ୍ତୁ cnt;
}

    ସ୍ଥିର ଶୂନ୍ୟ
cs_reading_emsg(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)	// connection index
{
    semsg(_(e_error_reading_cscope_connection_nr), idx);
}

# define	CSREAD_BUFSIZE	2048
/*
 * Count the number of matches for a given cscope connection.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_cnt_matches(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଅକ୍ଷର *stok;
    ଅକ୍ଷର *buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nlines = 0;

    buf = alloc(CSREAD_BUFSIZE);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ 0;
    ପାଇଁ (;;)
    {
	ଯଦି (!fgets(buf, CSREAD_BUFSIZE, csinfo[idx].fr_fp))
	{
	    ଯଦି (feof(csinfo[idx].fr_fp))
		errno = EIO;

	    cs_reading_emsg(idx);

	    vim_free(buf);
	    ଫେରନ୍ତୁ -1;
	}

	/*
	 * If the database is out of date, or there's some other problem,
	 * cscope will output error messages before the number-of-lines output.
	 * Display/discard any output that doesn't match what we want.
	 * Accept "\S*cscope: X lines", also matches "mlcscope".
	 * Bail out for the "Unable to search" error.
	 */
	ଯଦି (strstr((ଅଚଳ ଅକ୍ଷର *)buf, "Unable to search database") != ଖାଲି)
	    ଭାଙ୍ଗ;
	ଯଦି ((stok = strtok(buf, (ଅଚଳ ଅକ୍ଷର *)" ")) == ଖାଲି)
	    ଜାରି;
	ଯଦି (strstr((ଅଚଳ ଅକ୍ଷର *)stok, "cscope:") == ଖାଲି)
	    ଜାରି;

	ଯଦି ((stok = strtok(ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)" ")) == ଖାଲି)
	    ଜାରି;
	nlines = atoi(stok);
	ଯଦି (nlines < 0)
	{
	    nlines = 0;
	    ଭାଙ୍ଗ;
	}

	ଯଦି ((stok = strtok(ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)" ")) == ଖାଲି)
	    ଜାରି;
	ଯଦି (strncmp((ଅଚଳ ଅକ୍ଷର *)stok, "lines", 5))
	    ଜାରି;

	ଭାଙ୍ଗ;
    }

    vim_free(buf);
    ଫେରନ୍ତୁ nlines;
}


/*
 * Creates the actual cscope command query from what the user entered.
 */
    ସ୍ଥିର ଅକ୍ଷର *
cs_create_cmd(ଅକ୍ଷର *csoption, ଅକ୍ଷର *pattern)
{
    ଅକ୍ଷର *cmd;
    short ସନ୍ଧାନ;
    ଅକ୍ଷର *pat;

    ବାଛ (csoption[0])
    {
    ସ୍ଥିତି '0' : ସ୍ଥିତି 's' :
	ସନ୍ଧାନ = 0;
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '1' : ସ୍ଥିତି 'g' :
	ସନ୍ଧାନ = 1;
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '2' : ସ୍ଥିତି 'd' :
	ସନ୍ଧାନ = 2;
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '3' : ସ୍ଥିତି 'c' :
	ସନ୍ଧାନ = 3;
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '4' : ସ୍ଥିତି 't' :
	ସନ୍ଧାନ = 4;
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '6' : ସ୍ଥିତି 'e' :
	ସନ୍ଧାନ = 6;
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '7' : ସ୍ଥିତି 'f' :
	ସନ୍ଧାନ = 7;
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '8' : ସ୍ଥିତି 'i' :
	ସନ୍ଧାନ = 8;
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '9' : ସ୍ଥିତି 'a' :
	ସନ୍ଧାନ = 9;
	ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ :
	(ଶୂନ୍ୟ)emsg(_(e_unknown_cscope_ସନ୍ଧାନ_type));
	cs_usage_ସନ୍ଦେଶ(Find);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Skip white space before the pattern, except for text and pattern search,
    // they may want to use the leading white space.
    pat = pattern;
    ଯଦି (ସନ୍ଧାନ != 4 && ସନ୍ଧାନ != 6)
	ଯେତେବେଳେ VIM_ISWHITE(*pat)
	    ++pat;

    ଯଦି ((cmd = alloc(strlen(pat) + 2)) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    (ଶୂନ୍ୟ)sprintf(cmd, "%d%s", ସନ୍ଧାନ, pat);

    ଫେରନ୍ତୁ cmd;
}


/*
 * This piece of code was taken/adapted from nvi.  do we need to add
 * the BSD license notice?
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_create_connection(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
{
# ifdef UNIX
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		to_cs[2], from_cs[2];
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdlen;
    ଅକ୍ଷର	*prog, *cmd, *ppath = ଖାଲି;
    size_t	proglen;
# ifdef MSWIN
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL	pipe_stdin = FALSE, pipe_stdout = FALSE;
    HANDLE	stdin_rd, stdout_rd;
    HANDLE	stdout_wr, stdin_wr;
    BOOL	created;
# endif

# ଯଦି defined(UNIX)
    /*
     * Cscope reads from to_cs[0] and writes to from_cs[1]; vi reads from
     * from_cs[0] and writes to to_cs[1].
     */
    to_cs[0] = to_cs[1] = from_cs[0] = from_cs[1] = -1;
    ଯଦି (pipe(to_cs) < 0 || pipe(from_cs) < 0)
    {
	(ଶୂନ୍ୟ)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	ଯଦି (to_cs[0] != -1)
	    (ଶୂନ୍ୟ)close(to_cs[0]);
	ଯଦି (to_cs[1] != -1)
	    (ଶୂନ୍ୟ)close(to_cs[1]);
	ଯଦି (from_cs[0] != -1)
	    (ଶୂନ୍ୟ)close(from_cs[0]);
	ଯଦି (from_cs[1] != -1)
	    (ଶୂନ୍ୟ)close(from_cs[1]);
	ଫେରନ୍ତୁ CSCOPE_FAILURE;
    }

    ଯଦି ((csinfo[i].pid = fork()) == -1)
    {
	(ଶୂନ୍ୟ)emsg(_(e_could_not_fork_for_cscope));
	ଯାଅ err_closing;
    }
    ଅନ୍ୟଥା ଯଦି (csinfo[i].pid == 0)	// child: run cscope.
    {
	ଅକ୍ଷର **argv = ଖାଲି;

	ଯଦି (dup2(to_cs[0], STDIN_FILENO) == -1)
	    PERROR("cs_create_connection 1");
	ଯଦି (dup2(from_cs[1], STDOUT_FILENO) == -1)
	    PERROR("cs_create_connection 2");
	ଯଦି (dup2(from_cs[1], STDERR_FILENO) == -1)
	    PERROR("cs_create_connection 3");

	// close unused
	(ଶୂନ୍ୟ)close(to_cs[1]);
	(ଶୂନ୍ୟ)close(from_cs[0]);
# ଅନ୍ୟଥା
	// MSWIN
	// Create pipes to communicate with cscope
	sa.nLength = ଆକାର(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ଖାଲି;

	ଯଦି (!(pipe_stdin = CreatePipe(&stdin_rd, &stdin_wr, &sa, 0))
		|| !(pipe_stdout = CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)))
	{
	    (ଶୂନ୍ୟ)emsg(_(e_could_not_create_cscope_pipes));
err_closing:
	    ଯଦି (pipe_stdin)
	    {
		CloseHandle(stdin_rd);
		CloseHandle(stdin_wr);
	    }
	    ଯଦି (pipe_stdout)
	    {
		CloseHandle(stdout_rd);
		CloseHandle(stdout_wr);
	    }
	    ଫେରନ୍ତୁ CSCOPE_FAILURE;
	}
# endif
	// expand the cscope exec for env var's
	ଯଦି ((prog = alloc(MAXPATHL + 1)) == ଖାଲି)
	{
# ifdef UNIX
	    exit(EXIT_FAILURE);
# ଅନ୍ୟଥା
	    // MSWIN
	    ଯାଅ err_closing;
# endif
	}
	proglen = expand_env(p_csprg, (char_u *)prog, MAXPATHL);

	// alloc space to hold the cscope command
	cmdlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(proglen + strlen(csinfo[i].fname) + 32);
	ଯଦି (csinfo[i].ppath)
	{
	    // expand the prepend path for env var's
	    ଯଦି ((ppath = alloc(MAXPATHL + 1)) == ଖାଲି)
	    {
		vim_free(prog);
# ifdef UNIX
		exit(EXIT_FAILURE);
# ଅନ୍ୟଥା
		// MSWIN
		ଯାଅ err_closing;
# endif
	    }
	    cmdlen += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)expand_env((char_u *)csinfo[i].ppath, (char_u *)ppath, MAXPATHL);
	}

	ଯଦି (csinfo[i].flags)
	    cmdlen += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(csinfo[i].flags);

	ଯଦି ((cmd = alloc(cmdlen)) == ଖାଲି)
	{
	    vim_free(prog);
	    vim_free(ppath);
# ifdef UNIX
	    exit(EXIT_FAILURE);
# ଅନ୍ୟଥା
	    // MSWIN
	    ଯାଅ err_closing;
# endif
	}

	// run the cscope command
# ifdef UNIX
	{
	    garray_T	ga_argv;
	    ଅକ୍ଷର	**tok = ଖାଲି;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tokc = 0;

	    ga_init2(&ga_argv, ଆକାର(ଅକ୍ଷର *), 8);

	    // 'cscopeprg' may be multi-word (e.g. "cscope --foo");
	    // split it into separate argv entries.
	    ଯଦି (build_argv_from_string((char_u *)prog, &tok, &tokc) == FAIL)
		exit(EXIT_FAILURE);
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < tokc; ++k)
	    {
		ଯଦି (ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		((ଅକ୍ଷର **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
	    }
	    VIM_CLEAR(tok);

	    // Literal arguments: "-dl", "-f", fname.
	    ଯଦି (ga_grow(&ga_argv, 3) == FAIL)
		exit(EXIT_FAILURE);
	    ((ଅକ୍ଷର **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (ଅକ୍ଷର *)vim_strsave((char_u *)"-dl");
	    ((ଅକ୍ଷର **)ga_argv.ga_data)[ga_argv.ga_len++] =
				    (ଅକ୍ଷର *)vim_strsave((char_u *)"-f");
	    ((ଅକ୍ଷର **)ga_argv.ga_data)[ga_argv.ga_len++] =
				(ଅକ୍ଷର *)vim_strsave((char_u *)csinfo[i].fname);

	    // "-P<ppath>" as a single argv entry, if set.
	    ଯଦି (ppath != ଖାଲି)
	    {
		size_t	plen = STRLEN(ppath) + 3;
		ଅକ୍ଷର	*parg = alloc(plen);

		ଯଦି (parg == ଖାଲି || ga_grow(&ga_argv, 1) == FAIL)
		    exit(EXIT_FAILURE);
		vim_snprintf(parg, plen, "-P%s", ppath);
		((ଅକ୍ଷର **)ga_argv.ga_data)[ga_argv.ga_len++] = parg;
	    }

	    // 'flags' is intended to be a sequence of cscope tokens
	    // like "-q -i somefile"; split it the same way as prog.
	    ଯଦି (csinfo[i].flags != ଖାଲି)
	    {
		ଯଦି (build_argv_from_string((char_u *)csinfo[i].flags,
							&tok, &tokc) == FAIL)
		    exit(EXIT_FAILURE);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < tokc; ++k)
		{
		    ଯଦି (ga_grow(&ga_argv, 1) == FAIL)
			exit(EXIT_FAILURE);
		    ((ଅକ୍ଷର **)ga_argv.ga_data)[ga_argv.ga_len++] = tok[k];
		}
		vim_free(tok);
	    }

	    // NULL-terminate argv.
	    ଯଦି (ga_grow(&ga_argv, 1) == FAIL)
		exit(EXIT_FAILURE);
	    ((ଅକ୍ଷର **)ga_argv.ga_data)[ga_argv.ga_len] = ଖାଲି;

	    vim_free(prog);
	    vim_free(ppath);
	    vim_free(cmd);

#  ଯଦି defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	    // Change our process group to avoid cscope receiving SIGWINCH.
#   ଯଦି defined(HAVE_SETSID)
	    (ଶୂନ୍ୟ)setsid();
#   ଅନ୍ୟଥା
	    ଯଦି (setpgid(0, 0) == -1)
		PERROR(_("cs_create_connection setpgid failed"));
#   endif
#  endif

	    argv = (ଅକ୍ଷର **)ga_argv.ga_data;
	    ଯଦି (argv[0] == ଖାଲି || execvp(argv[0], argv) == -1)
	    {
		fprintf(stderr, "%s\n",
		    _("cs_create_connection exec failed"));
		fflush(stderr);
	    }

	    exit(127);
	    // NOTREACHED
	}
    }
    ଅନ୍ୟଥା	// parent.
    {
	/*
	 * Save the file descriptors for later duplication, and
	 * reopen as streams.
	 */
	ଯଦି ((csinfo[i].to_fp = fdopen(to_cs[1], "w")) == ଖାଲି)
	    PERROR(_("cs_create_connection: fdopen for to_fp failed"));
	ଯଦି ((csinfo[i].fr_fp = fdopen(from_cs[0], "r")) == ଖାଲି)
	    PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

	// close unused
	(ଶୂନ୍ୟ)close(to_cs[0]);
	(ଶୂନ୍ୟ)close(from_cs[1]);
    }
# ଅନ୍ୟଥା
    // MSWIN
    vim_snprintf(cmd, cmdlen, "%s -dl -f %s", prog, csinfo[i].fname);
    ଯଦି (csinfo[i].ppath != ଖାଲି)
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " -P%s", csinfo[i].ppath);
    }
    ଯଦି (csinfo[i].flags != ଖାଲି)
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(cmd);
	vim_snprintf(cmd + len, cmdlen - len, " %s", csinfo[i].flags);
    }
    vim_free(ppath);

    // Create a new process to run cscope and use pipes to talk with it
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // Hide child application window
    si.hStdOutput = stdout_wr;
    si.hStdError  = stdout_wr;
    si.h_Std_ନିବେଶ  = stdin_rd;
    created = CreateProcess(ଖାଲି, cmd, ଖାଲି, ଖାଲି, TRUE, CREATE_NEW_CONSOLE,
							ଖାଲି, ଖାଲି, &si, &pi);
    vim_free(prog);
    vim_free(cmd);

    ଯଦି (!created)
    {
	PERROR(_("cs_create_connection exec failed"));
	(ଶୂନ୍ୟ)emsg(_(e_could_not_spawn_cscope_process));
	ଯାଅ err_closing;
    }
    // else
    csinfo[i].pid = pi.dwProcessId;
    csinfo[i].hProc = pi.hProcess;
    CloseHandle(pi.hThread);

    // TODO - tidy up after failure to create files on pipe handles.
    ଯଦି (((fd = _open_osfhandle((intptr_t)stdin_wr,
						      _O_TEXT|_O_APPEND)) < 0)
	    || ((csinfo[i].to_fp = _fdopen(fd, "w")) == ଖାଲି))
	PERROR(_("cs_create_connection: fdopen for to_fp failed"));
    ଯଦି (((fd = _open_osfhandle((intptr_t)stdout_rd,
						      _O_TEXT|_O_RDONLY)) < 0)
	    || ((csinfo[i].fr_fp = _fdopen(fd, "r")) == ଖାଲି))
	PERROR(_("cs_create_connection: fdopen for fr_fp failed"));

    // Close handles for file descriptors inherited by the cscope process
    CloseHandle(stdin_rd);
    CloseHandle(stdout_wr);

# endif // !UNIX

    ଫେରନ୍ତୁ CSCOPE_SUCCESS;
}


/*
 * Query cscope using command line interface.  Parse the output and use tselect
 * to allow choices.  Like Nvi, creates a pipe to send to/from query/cscope.
 *
 * returns TRUE if we jump to a tag or abort, FALSE if not.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_find(exarg_T *eap)
{
    ଅକ୍ଷର *opt, *pat;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (cs_check_for_connections() == FALSE)
    {
	(ଶୂନ୍ୟ)emsg(_(e_no_cscope_connections));
	ଫେରନ୍ତୁ FALSE;
    }

    ଯଦି ((opt = strtok((ଅକ୍ଷର *)ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)" ")) == ଖାଲି)
    {
	cs_usage_ସନ୍ଦେଶ(Find);
	ଫେରନ୍ତୁ FALSE;
    }

    pat = opt + strlen(opt) + 1;
    ଯଦି (pat >= (ଅକ୍ଷର *)eap->arg + eap_arg_len)
    {
	cs_usage_ସନ୍ଦେଶ(Find);
	ଫେରନ୍ତୁ FALSE;
    }

    /*
     * Let's replace the NULs written by strtok() with spaces - we need the
     * spaces to correctly display the quickfix/location list window's title.
     */
    ପାଇଁ (i = 0; i < eap_arg_len; ++i)
	ଯଦି (NUL == eap->arg[i])
	    eap->arg[i] = ' ';

    ଫେରନ୍ତୁ cs_find_common(opt, pat, eap->forceit, TRUE,
				  eap->cmdidx == CMD_lcscope, *eap->cmdlinep);
}


/*
 * Common code for cscope find, shared by cs_find() and ex_cstag().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_find_common(
    ଅକ୍ଷର *opt,
    ଅକ୍ଷର *pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	use_ll UNUSED,
    char_u *cmdline UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ଅକ୍ଷର *cmd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *nummatches;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା totmatches;
# ifdef FEAT_QUICKFIX
    ଅକ୍ଷର cmdletter;
    ଅକ୍ଷର *qfpos;

    // get cmd letter
    ବାଛ (opt[0])
    {
    ସ୍ଥିତି '0' :
	cmdletter = 's';
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '1' :
	cmdletter = 'g';
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '2' :
	cmdletter = 'd';
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '3' :
	cmdletter = 'c';
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '4' :
	cmdletter = 't';
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '6' :
	cmdletter = 'e';
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '7' :
	cmdletter = 'f';
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '8' :
	cmdletter = 'i';
	ଭାଙ୍ଗ;
    ସ୍ଥିତି '9' :
	cmdletter = 'a';
	ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ :
	cmdletter = opt[0];
    }

    qfpos = (ଅକ୍ଷର *)vim_strchr(p_csqf, cmdletter);
    ଯଦି (qfpos != ଖାଲି)
    {
	qfpos++;
	// next symbol must be + or -
	ଯଦି (strchr(CSQF_FLAGS, *qfpos) == ଖାଲି)
	{
	    (ଶୂନ୍ୟ)semsg(_(e_invalid_cscopequickfix_flag_chr_for_chr),
							 *qfpos, *(qfpos - 1));
	    ଫେରନ୍ତୁ FALSE;
	}

	ଯଦି (*qfpos != '0'
		&& apply_autocmds(EVENT_QUICKFIXCMDPRE, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf))
	{
#  ifdef FEAT_EVAL
	    ଯଦି (aborting())
		ଫେରନ୍ତୁ FALSE;
#  endif
	}
    }
# endif

    // create the actual command to send to cscope
    cmd = cs_create_cmd(opt, pat);
    ଯଦି (cmd == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    nummatches = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, csinfo_size);
    ଯଦି (nummatches == ଖାଲି)
    {
	vim_free(cmd);
	ଫେରନ୍ତୁ FALSE;
    }

    // Send query to all open connections, then count the total number
    // of matches so we can alloc all in one swell foop.
    ପାଇଁ (i = 0; i < csinfo_size; i++)
	nummatches[i] = 0;
    totmatches = 0;
    ପାଇଁ (i = 0; i < csinfo_size; i++)
    {
	ଯଦି (csinfo[i].fname == ଖାଲି || csinfo[i].to_fp == ଖାଲି)
	    ଜାରି;

	// send cmd to cscope
	(ଶୂନ୍ୟ)fprintf(csinfo[i].to_fp, "%s\n", cmd);
	(ଶୂନ୍ୟ)fflush(csinfo[i].to_fp);

	nummatches[i] = cs_cnt_matches(i);

	ଯଦି (nummatches[i] > -1)
	    totmatches += nummatches[i];

	ଯଦି (nummatches[i] == 0)
	    (ଶୂନ୍ୟ)cs_read_prompt(i);
    }
    vim_free(cmd);

    ଯଦି (totmatches == 0)
    {
	ଯଦି (verbose)
	    (ଶୂନ୍ୟ)semsg(_(e_no_matches_found_for_cscope_query_str_of_str),
								     opt, pat);
	vim_free(nummatches);
	ଫେରନ୍ତୁ FALSE;
    }

# ifdef FEAT_QUICKFIX
    ଯଦି (qfpos != ଖାଲି && *qfpos != '0' && totmatches > 0)
    {
	// fill error list
	FILE	    *f;
	char_u	    *tmp = vim_tempname('c', TRUE);
	qf_info_T   *qi = ଖାଲି;
	win_T	    *wp = ଖାଲି;

	ଯଦି (tmp == ଖାଲି)
	{
	    vim_free(nummatches);
	    ଫେରନ୍ତୁ FALSE;
	}

	f = mch_fopen((ଅକ୍ଷର *)tmp, "w");
	ଯଦି (f == ଖାଲି)
	    semsg(_(e_cant_open_file_str), tmp);
	ଅନ୍ୟଥା
	{
	    cs_file_results(f, nummatches);
	    fclose(f);
	    ଯଦି (use_ll)	    // Use location list
		wp = curwin;
	    // '-' starts a new error list
	    ଯଦି (qf_init(wp, tmp, (char_u *)"%f%*\\t%l%*\\t%m",
					  *qfpos == '-', cmdline, ଖାଲି) > 0)
	    {
		ଯଦି (postponed_split != 0)
		{
		    (ଶୂନ୍ୟ)win_split(postponed_split > 0 ? postponed_split : 0,
						       postponed_split_flags);
		    RESET_BINDING(curwin);
		    postponed_split = 0;
		}

		apply_autocmds(EVENT_QUICKFIXCMDPOST, (char_u *)"cscope",
					       curbuf->b_fname, TRUE, curbuf);
		ଯଦି (use_ll)
		    /*
		     * In the location list window, use the displayed location
		     * list. Otherwise, use the location list for the window.
		     */
		    qi = (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != ଖାଲି)
			?  wp->w_llist_ref : wp->w_llist;
		qf_jump(qi, 0, 0, forceit);
	    }
	}
	mch_remove(tmp);
	vim_free(tmp);
	vim_free(nummatches);
	ଫେରନ୍ତୁ TRUE;
    }
    ଅନ୍ୟଥା
# endif // FEAT_QUICKFIX
    {
	ଅକ୍ଷର **matches = ଖାଲି, **contexts = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା matched = 0;

	// read output
	cs_fill_results(pat, totmatches, nummatches, &matches,
							 &contexts, &matched);
	vim_free(nummatches);
	ଯଦି (matches == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;

	(ଶୂନ୍ୟ)cs_manage_matches(matches, contexts, matched, Store);

	ଫେରନ୍ତୁ do_tag((char_u *)pat, DT_CSCOPE, 0, forceit, verbose);
    }

}

/*
 * Print help.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_help(exarg_T *eap UNUSED)
{
    cscmd_T *cmdp = cs_cmds;

    (ଶୂନ୍ୟ)ସନ୍ଦେଶ_puts(_("cscope commands:\n"));
    ଯେତେବେଳେ (cmdp->name != ଖାଲି)
    {
	ଅକ୍ଷର *help = _(cmdp->help);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା  space_cnt = 30 - vim_strsize((char_u *)help);

	// Use %*s rather than %30s to ensure proper alignment in utf-8
	ଯଦି (space_cnt < 0)
	    space_cnt = 0;
	(ଶୂନ୍ୟ)smsg(_("%-5s: %s%*s (Usage: %s)"),
				      cmdp->name,
				      help, space_cnt, " ",
				      cmdp->usage);
	ଯଦି (strcmp(cmdp->name, "find") == 0)
	    ସନ୍ଦେଶ_puts(_("\n"
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
    ଫେରନ୍ତୁ 0;
}


    ସ୍ଥିର ଶୂନ୍ୟ
clear_csinfo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
{
    csinfo[i].fname  = ଖାଲି;
    csinfo[i].ppath  = ଖାଲି;
    csinfo[i].flags  = ଖାଲି;
# ଯଦି defined(UNIX)
    csinfo[i].st_dev = (dev_t)0;
    csinfo[i].st_ino = (ino_t)0;
# ଅନ୍ୟଥା
    csinfo[i].nVolume = 0;
    csinfo[i].n_Index_ଉପର = 0;
    csinfo[i].n_Index_ତଳ = 0;
# endif
    csinfo[i].pid    = 0;
    csinfo[i].fr_fp  = ଖାଲି;
    csinfo[i].to_fp  = ଖାଲି;
# ଯଦି defined(MSWIN)
    csinfo[i].hProc = ଖାଲି;
# endif
}

/*
 * Insert a new cscope database filename into the filelist.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_ଭର୍ତ୍ତି_filelist(
    ଅକ୍ଷର *fname,
    ଅକ୍ଷର *ppath,
    ଅକ୍ଷର *flags,
    stat_T *sb UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    j;
# ifndef UNIX
    BY_HANDLE_FILE_INFORMATION bhfi;

    ବାଛ (win32_fileinfo((char_u *)fname, &bhfi))
    {
	ସ୍ଥିତି FILEINFO_ENC_FAIL:		// enc_to_utf16() failed
	ସ୍ଥିତି FILEINFO_READ_FAIL:	// CreateFile() failed
	    ଯଦି (p_csverbose)
	    {
		ଅକ୍ଷର *cant_ସନ୍ଦେଶ = _(e_cannot_open_cscope_database_str);
		ଅକ୍ଷର *winmsg = GetWin32Error();

		ଯଦି (winmsg != ଖାଲି)
		    (ଶୂନ୍ୟ)semsg(cant_ସନ୍ଦେଶ, winmsg);
		ଅନ୍ୟଥା
		    // subst filename if can't get error text
		    (ଶୂନ୍ୟ)semsg(cant_ସନ୍ଦେଶ, fname);
	    }
	    ଫେରନ୍ତୁ -1;

	ସ୍ଥିତି FILEINFO_INFO_FAIL:    // GetFileInformationByHandle() failed
	    ଯଦି (p_csverbose)
		(ଶୂନ୍ୟ)emsg(_(e_cannot_get_cscope_database_information));
	    ଫେରନ୍ତୁ -1;
    }
# endif

    i = -1; // can be set to the index of an empty item in csinfo
    ପାଇଁ (j = 0; j < csinfo_size; j++)
    {
	ଯଦି (csinfo[j].fname != ଖାଲି
# ଯଦି defined(UNIX)
	    && csinfo[j].st_dev == sb->st_dev && csinfo[j].st_ino == sb->st_ino
# ଅନ୍ୟଥା
	    // compare pathnames first
	    && ((fullpathcmp((char_u *)csinfo[j].fname,
			(char_u *)fname, FALSE, TRUE) & FPC_SAME)
		// test index file attributes too
		|| (csinfo[j].nVolume == bhfi.dw_Volume_Serial_ସଂଖ୍ୟା
		    && csinfo[j].n_Index_ଉପର == bhfi.n_File_Index_ଉପର
		    && csinfo[j].n_Index_ତଳ == bhfi.n_File_Index_ତଳ))
# endif
	    )
	{
	    ଯଦି (p_csverbose)
		(ଶୂନ୍ୟ)emsg(_(e_duplicate_cscope_database_not_added));
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (csinfo[j].fname == ଖାଲି && i == -1)
	    i = j; // remember first empty entry
    }

    ଯଦି (i == -1)
    {
	i = csinfo_size;
	ଯଦି (csinfo_size == 0)
	{
	    // First time allocation: allocate only 1 connection. It should
	    // be enough for most users.  If more is needed, csinfo will be
	    // reallocated.
	    csinfo_size = 1;
	    csinfo = ALLOC_CLEAR_ONE(csinfo_T);
	}
	ଅନ୍ୟଥା
	{
	    csinfo_T *t_csinfo = csinfo;

	    // Reallocate space for more connections.
	    csinfo_size *= 2;
	    csinfo = vim_realloc(csinfo, ଆକାର(csinfo_T)*csinfo_size);
	    ଯଦି (csinfo == ଖାଲି)
	    {
		// allocation failure, so keep the old infos
		csinfo = t_csinfo;
		csinfo_size = i;
		ଫେରନ୍ତୁ -1;
	    }
	}
	ଯଦି (csinfo == ଖାଲି)
	    ଫେରନ୍ତୁ -1;
	ପାଇଁ (j = csinfo_size/2; j < csinfo_size; j++)
	    clear_csinfo(j);
    }

    ଯଦି ((csinfo[i].fname = alloc(strlen(fname)+1)) == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    vim_strncpy((char_u *)csinfo[i].fname, (char_u *)fname, strlen((ଅଚଳ ଅକ୍ଷର *)fname));

    ଯଦି (ppath != ଖାଲି)
    {
	ଯଦି ((csinfo[i].ppath = alloc(strlen(ppath) + 1)) == ଖାଲି)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    ଫେରନ୍ତୁ -1;
	}
	vim_strncpy((char_u *)csinfo[i].ppath, (char_u *)ppath, strlen((ଅଚଳ ଅକ୍ଷର *)ppath));
    }
    ଅନ୍ୟଥା
	csinfo[i].ppath = ଖାଲି;

    ଯଦି (flags != ଖାଲି)
    {
	ଯଦି ((csinfo[i].flags = alloc(strlen(flags) + 1)) == ଖାଲି)
	{
	    VIM_CLEAR(csinfo[i].fname);
	    VIM_CLEAR(csinfo[i].ppath);
	    ଫେରନ୍ତୁ -1;
	}
	vim_strncpy((char_u *)csinfo[i].flags, (char_u *)flags, strlen((ଅଚଳ ଅକ୍ଷର *)flags));
    }
    ଅନ୍ୟଥା
	csinfo[i].flags = ଖାଲି;

# ଯଦି defined(UNIX)
    csinfo[i].st_dev = sb->st_dev;
    csinfo[i].st_ino = sb->st_ino;

# ଅନ୍ୟଥା
    csinfo[i].nVolume = bhfi.dw_Volume_Serial_ସଂଖ୍ୟା;
    csinfo[i].n_Index_ତଳ = bhfi.n_File_Index_ତଳ;
    csinfo[i].n_Index_ଉପର = bhfi.n_File_Index_ଉପର;
# endif
    ଫେରନ୍ତୁ i;
}


/*
 * Find cscope command in command table.
 */
    ସ୍ଥିର cscmd_T *
cs_lookup_cmd(exarg_T *eap)
{
    cscmd_T *cmdp;
    ଅକ୍ଷର *stok;
    size_t len;

    ଯଦି (eap->arg == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Store length of eap->arg before it gets modified by strtok().
    eap_arg_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(eap->arg);

    ଯଦି ((stok = strtok((ଅକ୍ଷର *)(eap->arg), (ଅଚଳ ଅକ୍ଷର *)" ")) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    len = strlen(stok);
    ପାଇଁ (cmdp = cs_cmds; cmdp->name != ଖାଲି; ++cmdp)
    {
	ଯଦି (strncmp((ଅଚଳ ଅକ୍ଷର *)(stok), cmdp->name, len) == 0)
	    ଫେରନ୍ତୁ (cmdp);
    }
    ଫେରନ୍ତୁ ଖାଲି;
}


/*
 * Nuke em.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_kill(exarg_T *eap UNUSED)
{
    ଅକ୍ଷର *stok;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି ((stok = strtok((ଅକ୍ଷର *)ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)" ")) == ଖାଲି)
    {
	cs_usage_ସନ୍ଦେଶ(Kill);
	ଫେରନ୍ତୁ CSCOPE_FAILURE;
    }

    // only single digit positive and negative integers are allowed
    ଯଦି ((strlen(stok) < 2 && VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(stok[0])))
	    || (strlen(stok) < 3 && stok[0] == '-'
					      && VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(stok[1]))))
	i = atoi(stok);
    ଅନ୍ୟଥା
    {
	// It must be part of a name.  We will try to find a match
	// within all the names in the csinfo data structure
	ପାଇଁ (i = 0; i < csinfo_size; i++)
	{
	    ଯଦି (csinfo[i].fname != ଖାଲି && strstr(csinfo[i].fname, stok))
		ଭାଙ୍ଗ;
	}
    }

    ଯଦି ((i != -1) && (i >= csinfo_size || i < -1 || csinfo[i].fname == ଖାଲି))
    {
	ଯଦି (p_csverbose)
	    (ଶୂନ୍ୟ)semsg(_(e_cscope_connection_str_not_founc), stok);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (i == -1)
	{
	    ପାଇଁ (i = 0; i < csinfo_size; i++)
	    {
		ଯଦି (csinfo[i].fname)
		    cs_kill_execute(i, csinfo[i].fname);
	    }
	}
	ଅନ୍ୟଥା
	    cs_kill_execute(i, stok);
    }

    ଫେରନ୍ତୁ 0;
}


/*
 * Actually kills a specific cscope connection.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cs_kill_execute(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i,	    // cscope table index
    ଅକ୍ଷର *cname)    // cscope database name
{
    ଯଦି (p_csverbose)
    {
	ସନ୍ଦେଶ_clr_eos();
	(ଶୂନ୍ୟ)smsg_attr(HL_ATTR(HLF_R) | ସନ୍ଦେଶ_HIST,
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
    ସ୍ଥିର ଅକ୍ଷର *
cs_make_vim_style_matches(
    ଅକ୍ଷର *fname,
    ଅକ୍ଷର *slno,
    ଅକ୍ଷର *ସନ୍ଧାନ,
    ଅକ୍ଷର *tagstr)
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
    ଅକ୍ଷର *buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା amt;

    ଯଦି (ସନ୍ଧାନ != ଖାଲି)
    {
	amt = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(strlen(fname) + strlen(slno) + strlen(tagstr) + strlen(ସନ୍ଧାନ)+6);
	ଯଦି ((buf = alloc(amt)) == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	(ଶୂନ୍ୟ)sprintf(buf, "%s\t%s\t%s;\"\t%s", tagstr, fname, slno, ସନ୍ଧାନ);
    }
    ଅନ୍ୟଥା
    {
	amt = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(strlen(fname) + strlen(slno) + strlen(tagstr) + 5);
	ଯଦି ((buf = alloc(amt)) == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	(ଶୂନ୍ୟ)sprintf(buf, "%s\t%s\t%s;\"", tagstr, fname, slno);
    }

    ଫେରନ୍ତୁ buf;
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
    ସ୍ଥିର ଅକ୍ଷର *
cs_manage_matches(
    ଅକ୍ଷର **matches,
    ଅକ୍ଷର **contexts,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା totmatches,
    mcmd_e cmd)
{
    ସ୍ଥିର ଅକ୍ଷର **mp = ଖାଲି;
    ସ୍ଥିର ଅକ୍ଷର **cp = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt = -1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା next = -1;
    ଅକ୍ଷର *p = ଖାଲି;

    ବାଛ (cmd)
    {
    ସ୍ଥିତି Store:
	ପ୍ରତିଜ୍ଞା(matches != ଖାଲି);
	ପ୍ରତିଜ୍ଞା(totmatches > 0);
	ଯଦି (mp != ଖାଲି || cp != ଖାଲି)
	    (ଶୂନ୍ୟ)cs_manage_matches(ଖାଲି, ଖାଲି, -1, Free);
	mp = matches;
	cp = contexts;
	cnt = totmatches;
	next = 0;
	ଭାଙ୍ଗ;
    ସ୍ଥିତି Get:
	ଯଦି (next >= cnt)
	    ଫେରନ୍ତୁ ଖାଲି;

	p = mp[next];
	next++;
	ଭାଙ୍ଗ;
    ସ୍ଥିତି Free:
	ଯଦି (mp != ଖାଲି)
	{
	    ଯଦି (cnt > 0)
		ଯେତେବେଳେ (cnt--)
		{
		    vim_free(mp[cnt]);
		    ଯଦି (cp != ଖାଲି)
			vim_free(cp[cnt]);
		}
	    vim_free(mp);
	    vim_free(cp);
	}
	mp = ଖାଲି;
	cp = ଖାଲି;
	cnt = 0;
	next = 0;
	ଭାଙ୍ଗ;
    ସ୍ଥିତି Print:
	cs_print_tags_priv(mp, cp, cnt);
	ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	// should not reach here
	iemsg(e_fatal_error_in_cs_manage_matches);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ p;
}


/*
 * Parse cscope output.
 */
    ସ୍ଥିର ଅକ୍ଷର *
cs_parse_results(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnumber,
    ଅକ୍ଷର *buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufsize,
    ଅକ୍ଷର **context,
    ଅକ୍ଷର **linenumber,
    ଅକ୍ଷର **ସନ୍ଧାନ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch;
    ଅକ୍ଷର *p;
    ଅକ୍ଷର *name;

    ଯଦି (fgets(buf, bufsize, csinfo[cnumber].fr_fp) == ଖାଲି)
    {
	ଯଦି (feof(csinfo[cnumber].fr_fp))
	    errno = EIO;

	cs_reading_emsg(cnumber);

	ଫେରନ୍ତୁ ଖାଲି;
    }

    // If the line's too long for the buffer, discard it.
    ଯଦି ((p = strchr(buf, '\n')) == ଖାଲି)
    {
	ଯେତେବେଳେ ((ch = getc(csinfo[cnumber].fr_fp)) != EOF && ch != '\n')
	    ;
	ଫେରନ୍ତୁ ଖାଲି;
    }
    *p = '\0';

    /*
     * cscope output is in the following format:
     *
     *	<filename> <context> <line number> <pattern>
     */
    ଯଦି ((name = strtok(buf, (ଅଚଳ ଅକ୍ଷର *)" ")) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି ((*context = strtok(ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)" ")) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି ((*linenumber = strtok(ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)" ")) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    *ସନ୍ଧାନ = *linenumber + strlen(*linenumber) + 1;	// +1 to skip \0

    // --- nvi ---
    // If the file is older than the cscope database, that is,
    // the database was built since the file was last modified,
    // or there wasn't a search string, use the line number.
    ଯଦି (strcmp(*ସନ୍ଧାନ, "<unknown>") == 0)
	*ସନ୍ଧାନ = ଖାଲି;

    name = cs_resolve_file(cnumber, name);
    ଫେରନ୍ତୁ name;
}

# ifdef FEAT_QUICKFIX
/*
 * Write cscope find results to file.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cs_file_results(FILE *f, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *nummatches_a)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j;
    ଅକ୍ଷର *buf;
    ଅକ୍ଷର *ସନ୍ଧାନ, *slno;
    ଅକ୍ଷର *fullname;
    ଅକ୍ଷର *cntx;
    ଅକ୍ଷର *context;

    buf = alloc(CSREAD_BUFSIZE);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < csinfo_size; i++)
    {
	ଯଦି (nummatches_a[i] < 1)
	    ଜାରି;

	ପାଇଁ (j = 0; j < nummatches_a[i]; j++)
	{
	   ଯଦି ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &ସନ୍ଧାନ)) == ଖାଲି)
	       ଜାରି;

	   context = alloc(strlen(cntx)+5);
	   ଯଦି (context == ଖାଲି)
	   {
	       vim_free(fullname);
	       ଜାରି;
	   }

	   ଯଦି (strcmp(cntx, "<global>")==0)
	       strcpy(context, "<<global>>");
	   ଅନ୍ୟଥା
	       sprintf(context, "<<%s>>", cntx);

	   ଯଦି (ସନ୍ଧାନ == ଖାଲି)
	       fprintf(f, "%s\t%s\t%s\n", fullname, slno, context);
	   ଅନ୍ୟଥା
	       fprintf(f, "%s\t%s\t%s %s\n", fullname, slno, context, ସନ୍ଧାନ);

	   vim_free(context);
	   vim_free(fullname);
	} // for all matches

	(ଶୂନ୍ୟ)cs_read_prompt(i);

    } // for all cscope connections
    vim_free(buf);
}
# endif

/*
 * Get parsed cscope output and calls cs_make_vim_style_matches to convert
 * into ctags format.
 * When there are no matches sets "*matches_p" to NULL.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cs_fill_results(
    ଅକ୍ଷର *tagstr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା totmatches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *nummatches_a,
    ଅକ୍ଷର ***matches_p,
    ଅକ୍ଷର ***cntxts_p,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *matched)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j;
    ଅକ୍ଷର *buf;
    ଅକ୍ଷର *ସନ୍ଧାନ, *slno;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା totsofar = 0;
    ଅକ୍ଷର **matches = ଖାଲି;
    ଅକ୍ଷର **cntxts = ଖାଲି;
    ଅକ୍ଷର *fullname;
    ଅକ୍ଷର *cntx;

    ପ୍ରତିଜ୍ଞା(totmatches > 0);

    buf = alloc(CSREAD_BUFSIZE);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି ((matches = ALLOC_MULT(ଅକ୍ଷର *, totmatches)) == ଖାଲି)
	ଯାଅ parse_out;
    ଯଦି ((cntxts = ALLOC_MULT(ଅକ୍ଷର *, totmatches)) == ଖାଲି)
	ଯାଅ parse_out;

    ପାଇଁ (i = 0; i < csinfo_size; i++)
    {
	ଯଦି (nummatches_a[i] < 1)
	    ଜାରି;

	ପାଇଁ (j = 0; j < nummatches_a[i]; j++)
	{
	   ଯଦି ((fullname = cs_parse_results(i, buf, CSREAD_BUFSIZE, &cntx,
			   &slno, &ସନ୍ଧାନ)) == ଖାଲି)
		ଜାରି;

	    matches[totsofar] = cs_make_vim_style_matches(fullname, slno,
							  ସନ୍ଧାନ, tagstr);

	    vim_free(fullname);

	    ଯଦି (strcmp(cntx, "<global>") == 0)
		cntxts[totsofar] = ଖାଲି;
	    ଅନ୍ୟଥା
		// note: if vim_strsave returns NULL, then the context
		// will be "<global>", which is misleading.
		cntxts[totsofar] = (ଅକ୍ଷର *)vim_strsave((char_u *)cntx);

	    ଯଦି (matches[totsofar] != ଖାଲି)
		totsofar++;

	} // for all matches

	(ଶୂନ୍ୟ)cs_read_prompt(i);

    } // for all cscope connections

parse_out:
    ଯଦି (totsofar == 0)
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
    ସ୍ଥିର ଅକ୍ଷର *
cs_pathcomponents(ଅକ୍ଷର *path)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ଅକ୍ଷର	*s;

    ଯଦି (p_cspc == 0)
	ଫେରନ୍ତୁ path;

    s = path + strlen(path) - 1;
    ପାଇଁ (i = 0; i < p_cspc; ++i)
    {
	ଯେତେବେଳେ (s > path)
	{
	   s--;
	   ଯଦି (*s == '/'
# ifdef MSWIN
		|| *s == '\\'
# endif
		)
	      ଭାଙ୍ଗ;
	}
    }
    ଯଦି ((s > path && *s == '/')
# ifdef MSWIN
	|| (s > path && *s == '\\')
# endif
	    )
	++s;
    ଫେରନ୍ତୁ s;
}

/*
 * Called from cs_manage_matches().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cs_print_tags_priv(ଅକ୍ଷର **matches, ଅକ୍ଷର **cntxts, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_matches)
{
    ଅକ୍ଷର	*buf = ଖାଲି;
    ଅକ୍ଷର	*t_buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufsize = 0; // Track available bufsize
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newsize = 0;
    ଅକ୍ଷର	*ptag;
    ଅକ୍ଷର	*fname, *lno, *extra, *tbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, idx, ଅଙ୍କ;
    ଅକ୍ଷର	*globalcntx = "GLOBAL";
    ଅକ୍ଷର	*cntxformat = " <<%s>>";
    ଅକ୍ଷର	*context;
    ଅକ୍ଷର	*cstag_ସନ୍ଦେଶ = _("Cscope tag: %s");
    ଅକ୍ଷର	*csfmt_str = "%4d %6s  ";

    ପ୍ରତିଜ୍ଞା(ଅଙ୍କ_matches > 0);

    ଯଦି ((tbuf = alloc(strlen(matches[0]) + 1)) == ଖାଲି)
	ଫେରନ୍ତୁ;

    strcpy(tbuf, matches[0]);
    ptag = strtok(tbuf, "\t");
    ଯଦି (ptag == ଖାଲି)
    {
	vim_free(tbuf);
	ଫେରନ୍ତୁ;
    }

    newsize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(strlen(cstag_ସନ୍ଦେଶ) + strlen(ptag));
    buf = alloc(newsize);
    ଯଦି (buf != ଖାଲି)
    {
	bufsize = newsize;
	(ଶୂନ୍ୟ)sprintf(buf, cstag_ସନ୍ଦେଶ, ptag);
	ସନ୍ଦେଶ_puts_attr(buf, HL_ATTR(HLF_T));
    }

    vim_free(tbuf);

    ସନ୍ଦେଶ_puts_attr(_("\n   #   line"), HL_ATTR(HLF_T));    // strlen is 7
    ସନ୍ଦେଶ_advance(ସନ୍ଦେଶ_col + 2);
    ସନ୍ଦେଶ_puts_attr(_("filename / context / line\n"), HL_ATTR(HLF_T));

    ଅଙ୍କ = 1;
    ପାଇଁ (i = 0; i < ଅଙ୍କ_matches; i++)
    {
	idx = i;

	// if we really wanted to, we could avoid this malloc and strcpy
	// by parsing matches[i] on the fly and placing stuff into buf
	// directly, but that's too much of a hassle
	ଯଦି ((tbuf = alloc(strlen(matches[idx]) + 1)) == ଖାଲି)
	    ଜାରି;
	(ଶୂନ୍ୟ)strcpy(tbuf, matches[idx]);

	ଯଦି (strtok(tbuf, (ଅଚଳ ଅକ୍ଷର *)"\t") == ଖାଲି
		|| (fname = strtok(ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)"\t")) == ଖାଲି
		|| (lno = strtok(ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)"\t")) == ଖାଲି)
	{
	    vim_free(tbuf);
	    ଜାରି;
	}
	extra = strtok(ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)"\t");

	lno[strlen(lno)-2] = '\0';  // ignore ;" at the end

	// hopefully 'num' (num of matches) will be less than 10^16
	newsize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(strlen(csfmt_str) + 16 + strlen(lno));
	ଯଦି (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ଯଦି (buf == ଖାଲି)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ଅନ୍ୟଥା
		bufsize = newsize;
	}
	ଯଦି (buf != ଖାଲି)
	{
	    // csfmt_str = "%4d %6s  ";
	    (ଶୂନ୍ୟ)sprintf(buf, csfmt_str, ଅଙ୍କ, lno);
	    ସନ୍ଦେଶ_puts_attr(buf, HL_ATTR(HLF_CM));
	}
	ସନ୍ଦେଶ_outtrans_long_attr((char_u *)cs_pathcomponents(fname),
							      HL_ATTR(HLF_CM));

	// compute the required space for the context
	ଯଦି (cntxts[idx] != ଖାଲି)
	    context = cntxts[idx];
	ଅନ୍ୟଥା
	    context = globalcntx;
	newsize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(strlen(context) + strlen(cntxformat));

	ଯଦି (bufsize < newsize)
	{
	    t_buf = buf;
	    buf = vim_realloc(buf, newsize);
	    ଯଦି (buf == ଖାଲି)
	    {
		bufsize = 0;
		vim_free(t_buf);
	    }
	    ଅନ୍ୟଥା
		bufsize = newsize;
	}
	ଯଦି (buf != ଖାଲି)
	{
	    (ଶୂନ୍ୟ)sprintf(buf, cntxformat, context);

	    // print the context only if it fits on the same line
	    ଯଦି (ସନ୍ଦେଶ_col + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(buf) >= cmdline_width)
		ସନ୍ଦେଶ_putchar('\n');
	    ସନ୍ଦେଶ_advance(12);
	    ସନ୍ଦେଶ_outtrans_long_attr((char_u *)buf, 0);
	    ସନ୍ଦେଶ_putchar('\n');
	}
	ଯଦି (extra != ଖାଲି)
	{
	    ସନ୍ଦେଶ_advance(13);
	    ସନ୍ଦେଶ_outtrans_long_attr((char_u *)extra, 0);
	}

	vim_free(tbuf); // only after printing extra due to strtok use

	ଯଦି (ସନ୍ଦେଶ_col)
	    ସନ୍ଦେଶ_putchar('\n');

	ui_breakcheck();
	ଯଦି (got_int)
	{
	    got_int = FALSE;	// don't print any more matches
	    ଭାଙ୍ଗ;
	}

	ଅଙ୍କ++;
    } // for all matches

    vim_free(buf);
}


/*
 * Read a cscope prompt (basically, skip over the ">> ").
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_read_prompt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ch;
    ଅକ୍ଷର	*buf = ଖାଲି; // buffer for possible error message from cscope
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufpos = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen;
    ସ୍ଥିର ଅକ୍ଷର *eprompt = "Press the RETURN key to continue:";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		epromptlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(eprompt);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    // compute maximum allowed len for Cscope error message
    ଅକ୍ଷର *cs_emsg = _(e_cscope_error_str);
    maxlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(IOSIZE - strlen(cs_emsg));

    ପାଇଁ (;;)
    {
	ଯେତେବେଳେ ((ch = getc(csinfo[i].fr_fp)) != EOF && ch != CSCOPE_PROMPT[0])
	    // if there is room and char is printable
	    ଯଦି (bufpos < maxlen - 1 && vim_isprintc(ch))
	    {
		ଯଦି (buf == ଖାଲି) // lazy buffer allocation
		    buf = alloc(maxlen);
		ଯଦି (buf != ଖାଲି)
		{
		    // append character to the message
		    buf[bufpos++] = ch;
		    buf[bufpos] = NUL;
		    ଯଦି (bufpos >= epromptlen
			    && strcmp(&buf[bufpos - epromptlen], eprompt) == 0)
		    {
			// remove eprompt from buf
			buf[bufpos - epromptlen] = NUL;

			// print message to user
			(ଶୂନ୍ୟ)semsg(cs_emsg, buf);

			// send RETURN to cscope
			(ଶୂନ୍ୟ)putc('\n', csinfo[i].to_fp);
			(ଶୂନ୍ୟ)fflush(csinfo[i].to_fp);

			// clear buf
			bufpos = 0;
			buf[bufpos] = NUL;
		    }
		}
	    }

	ପାଇଁ (n = 0; n < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(CSCOPE_PROMPT); ++n)
	{
	    ଯଦି (n > 0)
		ch = getc(csinfo[i].fr_fp);
	    ଯଦି (ch == EOF)
	    {
		ଯଦି (buf != ଖାଲି && buf[0] != NUL)
		    (ଶୂନ୍ୟ)semsg(cs_emsg, buf);
		ଅନ୍ୟଥା ଯଦି (p_csverbose)
		    cs_reading_emsg(i); // don't have additional information
		cs_release_csp(i, TRUE);
		vim_free(buf);
		ଫେରନ୍ତୁ CSCOPE_FAILURE;
	    }

	    ଯଦି (ch != CSCOPE_PROMPT[n])
	    {
		ch = EOF;
		ଭାଙ୍ଗ;
	    }
	}

	ଯଦି (ch == EOF)
	    ଜାରି;	    // didn't find the prompt
	ଭାଙ୍ଗ;		    // did find the prompt
    }

    vim_free(buf);
    ଫେରନ୍ତୁ CSCOPE_SUCCESS;
}

# ଯଦି defined(UNIX) && defined(SIGALRM)
/*
 * Used to catch and ignore SIGALRM below.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
sig_handler SIGDEFARG(sigarg)
{
    // do nothing
}
# endif

/*
 * Does the actual free'ing for the cs ptr with an optional flag of whether
 * or not to free the filename.  Called by cs_kill and cs_reset.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cs_release_csp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, ପୂର୍ଣ୍ଣସଂଖ୍ୟା freefnpp)
{
    /*
     * Trying to exit normally (not sure whether it is fit to UNIX cscope
     */
    ଯଦି (csinfo[i].to_fp != ଖାଲି)
    {
	(ଶୂନ୍ୟ)fputs("q\n", csinfo[i].to_fp);
	(ଶୂନ୍ୟ)fflush(csinfo[i].to_fp);
    }
# ଯଦି defined(UNIX)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା waitpid_errno;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pstat;
	pid_t pid;

#  ଯଦି defined(HAVE_SIGACTION)
	ସଂରଚନା sigaction sa, old;

	// Use sigaction() to limit the waiting time to two seconds.
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sig_handler;
#   ifdef SA_NODEFER
	sa.sa_flags = SA_NODEFER;
#   ଅନ୍ୟଥା
	sa.sa_flags = 0;
#   endif
	sigaction(SIGALRM, &sa, &old);
	alarm(2); // 2 sec timeout

	// Block until cscope exits or until timer expires
	pid = waitpid(csinfo[i].pid, &pstat, 0);
	waitpid_errno = errno;

	// cancel pending alarm if still there and restore signal
	alarm(0);
	sigaction(SIGALRM, &old, ଖାଲି);
#  ଅନ୍ୟଥା
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା waited;

	// Can't use sigaction(), loop for two seconds.  First yield the CPU
	// to give cscope a chance to exit quickly.
	sleep(0);
	ପାଇଁ (waited = 0; waited < 40; ++waited)
	{
	    pid = waitpid(csinfo[i].pid, &pstat, WNOHANG);
	    waitpid_errno = errno;
	    ଯଦି (pid != 0)
		ଭାଙ୍ଗ;  // break unless the process is still running
	    mch_delay(50L, 0); // sleep 50 ms
	}
#  endif
	/*
	 * If the cscope process is still running: kill it.
	 * Safety check: If the PID would be zero here, the entire X session
	 * would be killed.  -1 and 1 are dangerous as well.
	 */
	ଯଦି (pid < 0 && csinfo[i].pid > 1)
	{
#  ifdef ECHILD
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା alive = TRUE;

	    ଯଦି (waitpid_errno == ECHILD)
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
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା waited;

		sleep(0);
		ପାଇଁ (waited = 0; waited < 40; ++waited)
		{
		    // Check whether cscope process is still alive
		    ଯଦି (kill(csinfo[i].pid, 0) != 0)
		    {
			alive = FALSE; // cscope process no longer exists
			ଭାଙ୍ଗ;
		    }
		    mch_delay(50L, 0); // sleep 50 ms
		}
	    }
	    ଯଦି (alive)
#  endif
	    {
		kill(csinfo[i].pid, SIGKILL);
		(ଶୂନ୍ୟ)waitpid(csinfo[i].pid, &pstat, 0);
	    }
	}
    }
# ଅନ୍ୟଥା  // !UNIX
    ଯଦି (csinfo[i].hProc != ଖାଲି)
    {
	// Give cscope a chance to exit normally
	ଯଦି (WaitForSingleObject(csinfo[i].hProc, 1000) == WAIT_TIMEOUT)
	    TerminateProcess(csinfo[i].hProc, 0);
	CloseHandle(csinfo[i].hProc);
    }
# endif

    ଯଦି (csinfo[i].fr_fp != ଖାଲି)
	(ଶୂନ୍ୟ)fclose(csinfo[i].fr_fp);
    ଯଦି (csinfo[i].to_fp != ଖାଲି)
	(ଶୂନ୍ୟ)fclose(csinfo[i].to_fp);

    ଯଦି (freefnpp)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_reset(exarg_T *eap UNUSED)
{
    ଅକ୍ଷର	**dblist = ଖାଲି, **pplist = ଖାଲି, **fllist = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
    ଅକ୍ଷର buf[20]; // for sprintf " (#%d)"

    ଯଦି (csinfo_size == 0)
	ଫେରନ୍ତୁ CSCOPE_SUCCESS;

    // malloc our db and ppath list
    dblist = ALLOC_MULT(ଅକ୍ଷର *, csinfo_size);
    pplist = ALLOC_MULT(ଅକ୍ଷର *, csinfo_size);
    fllist = ALLOC_MULT(ଅକ୍ଷର *, csinfo_size);
    ଯଦି (dblist == ଖାଲି || pplist == ଖାଲି || fllist == ଖାଲି)
    {
	vim_free(dblist);
	vim_free(pplist);
	vim_free(fllist);
	ଫେରନ୍ତୁ CSCOPE_FAILURE;
    }

    ପାଇଁ (i = 0; i < csinfo_size; i++)
    {
	dblist[i] = csinfo[i].fname;
	pplist[i] = csinfo[i].ppath;
	fllist[i] = csinfo[i].flags;
	ଯଦି (csinfo[i].fname != ଖାଲି)
	    cs_release_csp(i, FALSE);
    }

    // rebuild the cscope connection list
    ପାଇଁ (i = 0; i < csinfo_size; i++)
    {
	ଯଦି (dblist[i] != ଖାଲି)
	{
	    cs_add_common(dblist[i], pplist[i], fllist[i]);
	    ଯଦି (p_csverbose)
	    {
		// don't use smsg_attr() because we want to display the
		// connection number in the same line as
		// "Added cscope database..."
		sprintf(buf, " (#%d)", i);
		ସନ୍ଦେଶ_puts_attr(buf, HL_ATTR(HLF_R));
	    }
	}
	vim_free(dblist[i]);
	vim_free(pplist[i]);
	vim_free(fllist[i]);
    }
    vim_free(dblist);
    vim_free(pplist);
    vim_free(fllist);

    ଯଦି (p_csverbose)
	ସନ୍ଦେଶ_attr(_("All cscope databases reset"), HL_ATTR(HLF_R) | ସନ୍ଦେଶ_HIST);
    ଫେରନ୍ତୁ CSCOPE_SUCCESS;
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
    ସ୍ଥିର ଅକ୍ଷର *
cs_resolve_file(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, ଅକ୍ଷର *name)
{
    ଅକ୍ଷର	*fullname;
    string_T	csdir = {ଖାଲି, 0};
    size_t	namelen;
    size_t	ppathlen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    /*
     * Ppath is freed when we destroy the cscope connection.
     * Fullname is freed after cs_make_vim_style_matches, after it's been
     * copied into the tag buffer used by Vim.
     */
    namelen = STRLEN(name);
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)namelen + 2;
    ଯଦି (csinfo[i].ppath != ଖାଲି)
    {
	ppathlen = STRLEN(csinfo[i].ppath);
	len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ppathlen;
    }
    ଅନ୍ୟଥା ଯଦି (p_csre && csinfo[i].fname != ଖାଲି)
    {
	// If 'cscoperelative' is set and ppath is not set, use cscope.out
	// path in path resolution.
	csdir.string = alloc(MAXPATHL);
	ଯଦି (csdir.string != ଖାଲି)
	{
	    vim_strncpy(csdir.string, (char_u *)csinfo[i].fname,
					  gettail((char_u *)csinfo[i].fname)
						 - (char_u *)csinfo[i].fname);
	    csdir.length = STRLEN(csdir.string);
	    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)csdir.length;
	}
    }

    // Note/example: this won't work if the cscope output already starts
    // "../.." and the prefix path is also "../..".  if something like this
    // happens, you are screwed up and need to fix how you're using cscope.
    ଯଦି (csinfo[i].ppath != ଖାଲି
	    && (strncmp(name, csinfo[i].ppath, ppathlen) != 0)
	    && (name[0] != '/')
# ifdef MSWIN
	    && name[0] != '\\' && name[1] != ':'
# endif
       )
    {
	ଯଦି ((fullname = alloc(len)) != ଖାଲି)
	    (ଶୂନ୍ୟ)sprintf(fullname, "%s/%s", csinfo[i].ppath, name);
    }
    ଅନ୍ୟଥା ଯଦି (csdir.string != ଖାଲି && csinfo[i].fname != ଖାଲି && *csdir.string != NUL)
    {
	string_T    ret;

	// Check for csdir to be non empty to avoid empty path concatenated to
	// cscope output.
	fullname = (ଅକ୍ଷର *)concat_fnames(csdir.string, csdir.length, (char_u *)name, namelen, TRUE, &ret);
    }
    ଅନ୍ୟଥା
    {
	fullname = (ଅକ୍ଷର *)vim_strnsave((char_u *)name, namelen);
    }

    vim_free(csdir.string);
    ଫେରନ୍ତୁ fullname;
}


/*
 * Show all cscope connections.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cs_show(exarg_T *eap UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (cs_cnt_connections() == 0)
	ସନ୍ଦେଶ_puts(_("no cscope connections\n"));
    ଅନ୍ୟଥା
    {
	ସନ୍ଦେଶ_puts_attr(
	    _(" # pid    database name                       prepend path\n"),
	    HL_ATTR(HLF_T));
	ପାଇଁ (i = 0; i < csinfo_size; i++)
	{
	    ଯଦି (csinfo[i].fname == ଖାଲି)
		ଜାରି;

	    ଯଦି (csinfo[i].ppath != ଖାଲି)
		(ଶୂନ୍ୟ)smsg("%2d %-5ld  %-34s  %-32s",
		    i, (long)csinfo[i].pid, csinfo[i].fname, csinfo[i].ppath);
	    ଅନ୍ୟଥା
		(ଶୂନ୍ୟ)smsg("%2d %-5ld  %-34s  <none>",
			   i, (long)csinfo[i].pid, csinfo[i].fname);
	}
    }

    wait_return(FALSE);
    ଫେରନ୍ତୁ CSCOPE_SUCCESS;
}


/*
 * Only called when VIM exits to quit any cscope sessions.
 */
    ଶୂନ୍ୟ
cs_end(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < csinfo_size; i++)
	cs_release_csp(i, TRUE);
    vim_free(csinfo);
    csinfo_size = 0;
}

#endif	// FEAT_CSCOPE

#ଯଦି defined(FEAT_EVAL)

/*
 * "cscope_connection([{num} , {dbpath} [, {prepend}]])" function
 *
 * Checks the existence of a cscope connection.
 */
    ଶୂନ୍ୟ
f_cscope_connection(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CSCOPE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ = 0;
    char_u	*dbpath = ଖାଲି;
    char_u	*prepend = ଖାଲି;
    char_u	buf[NUMBUFLEN];

    ଯଦି (in_vim9script()
	    && (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && (check_for_opt_string_arg(argvars, 1) == FAIL
			|| (argvars[1].v_type != VAR_UNKNOWN
			    && check_for_opt_string_arg(argvars, 2) == FAIL)))))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN
	    && argvars[1].v_type != VAR_UNKNOWN)
    {
	ଅଙ୍କ = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]);
	dbpath = tv_get_string(&argvars[1]);
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	    prepend = tv_get_string_buf(&argvars[2], buf);
    }

    rettv->vval.v_ସଂଖ୍ୟା = cs_connection(ଅଙ୍କ, dbpath, prepend);
# endif
}

#endif // FEAT_EVAL
