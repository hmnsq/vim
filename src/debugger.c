/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * debugger.c: Vim script debugger functions
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)
స్థిర పూర్ణాంకం debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
స్థిర శూన్య do_setdebugtracelevel(char_u *arg);
స్థిర శూన్య do_checkbacktracelevel(శూన్య);
స్థిర శూన్య do_showbacktrace(char_u *cmd);

స్థిర char_u *debug_oldval = ఖాళీ;	// old and newval for debug expressions
స్థిర char_u *debug_newval = ఖాళీ;
స్థిర పూర్ణాంకం     debug_సమాసం   = 0;	// use debug_expr

    పూర్ణాంకం
has_watchexpr(శూన్య)
{
    తిరిగి debug_సమాసం;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    శూన్య
do_debug(char_u *cmd)
{
    పూర్ణాంకం		save_సందేశం_scroll = సందేశం_scroll;
    పూర్ణాంకం		save_State = State;
    పూర్ణాంకం		save_did_emsg = did_emsg;
    పూర్ణాంకం		save_cmd_silent = cmd_silent;
    పూర్ణాంకం		save_సందేశం_silent = సందేశం_silent;
    పూర్ణాంకం		save_emsg_silent = emsg_silent;
    పూర్ణాంకం		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    పూర్ణాంకం		typeahead_saved = FALSE;
    పూర్ణాంకం		save_ignore_script = 0;
    పూర్ణాంకం		save_ex_సాధారణ_busy;
    పూర్ణాంకం		n;
    char_u	*cmdline = ఖాళీ;
    char_u	*p;
    char_u	*sname;
    అక్షరం	*తోక = ఖాళీ;
    స్థిర పూర్ణాంకం	చివరి_cmd = 0;
#నిర్వచించు CMD_CONT	1
#నిర్వచించు CMD_తదుపరి	2
#నిర్వచించు CMD_అడుగు	3
#నిర్వచించు CMD_ముగింపు	4
#నిర్వచించు CMD_QUIT	5
#నిర్వచించు CMD_అంతరాయం	6
#నిర్వచించు CMD_BACKTRACE	7
#నిర్వచించు CMD_చట్రం	8
#నిర్వచించు CMD_పైకి		9
#నిర్వచించు CMD_కిందికి	10

#నిర్వచితమైతే ALWAYS_USE_GUI
    // Can't do this when there is no terminal for input/output.
    ఒకవేళ (!gui.లోపల_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	తిరిగి;
    }
#అయితే_ముగింపు

    // Make sure we are in raw mode and start termcap mode.  Might have side
    // effects...
    settmode(TMODE_RAW);
    starttermcap();

    ++RedrawingDisabled;	// don't redisplay the window
    ++no_wait_return;		// don't wait for return
    did_emsg = FALSE;		// don't use error from debugged stuff
    cmd_silent = FALSE;		// display commands
    సందేశం_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = బహుళకం_సాధారణ;
    debug_బహుళకం = TRUE;

    ఒకవేళ (!debug_did_సందేశం)
	సందేశం(_("Entering Debug mode.  Type \"cont\" to continue."));
    ఒకవేళ (debug_oldval != ఖాళీ)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    ఒకవేళ (debug_newval != ఖాళీ)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    ఒకవేళ (sname != ఖాళీ)
	సందేశం((అక్షరం *)sname);
    vim_free(sname);
    ఒకవేళ (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    లేకపోతే
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    కొరకు (;;)
    {
	సందేశం_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_సాధారణ_busy = ex_సాధారణ_busy;
	ex_సాధారణ_busy = 0;
	ఒకవేళ (!debug_greedy)
	{
	    save_typeahead(&typeaheadbuf);
	    typeahead_saved = TRUE;
	    save_ignore_script = ignore_script;
	    ignore_script = TRUE;
	}

	// don't debug any function call, e.g. from an expression mapping
	n = debug_break_level;
	debug_break_level = -1;

	vim_free(cmdline);
	cmdline = getcmdline_prompt('>', ఖాళీ, 0, EXPAND_NOTHING, ఖాళీ);

	debug_break_level = n;
	ఒకవేళ (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_సాధారణ_busy = save_ex_సాధారణ_busy;

	cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
	సందేశం_starthere();
	ఒకవేళ (cmdline != ఖాళీ)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    ఒకవేళ (*p != NUL)
	    {
		ఎంపిక (*p)
		{
		    స్థితి 'c': చివరి_cmd = CMD_CONT;
			      తోక = "ont";
			      ఆపు;
		    స్థితి 'n': చివరి_cmd = CMD_తదుపరి;
			      తోక = "ext";
			      ఆపు;
		    స్థితి 's': చివరి_cmd = CMD_అడుగు;
			      తోక = "tep";
			      ఆపు;
		    స్థితి 'f':
			      చివరి_cmd = 0;
			      ఒకవేళ (p[1] == 'r')
			      {
				  చివరి_cmd = CMD_చట్రం;
				  తోక = "rame";
			      }
			      లేకపోతే
			      {
				  చివరి_cmd = CMD_ముగింపు;
				  తోక = "inish";
			      }
			      ఆపు;
		    స్థితి 'q': చివరి_cmd = CMD_QUIT;
			      తోక = "uit";
			      ఆపు;
		    స్థితి 'i': చివరి_cmd = CMD_అంతరాయం;
			      తోక = "nterrupt";
			      ఆపు;
		    స్థితి 'b': చివరి_cmd = CMD_BACKTRACE;
			      ఒకవేళ (p[1] == 't')
				  తోక = "t";
			      లేకపోతే
				  తోక = "acktrace";
			      ఆపు;
		    స్థితి 'w': చివరి_cmd = CMD_BACKTRACE;
			      తోక = "here";
			      ఆపు;
		    స్థితి 'u': చివరి_cmd = CMD_పైకి;
			      తోక = "p";
			      ఆపు;
		    స్థితి 'd': చివరి_cmd = CMD_కిందికి;
			      తోక = "own";
			      ఆపు;
		    అప్రమేయ: చివరి_cmd = 0;
		}
		ఒకవేళ (చివరి_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    అయితే (*p != NUL && *p == *తోక)
		    {
			++p;
			++తోక;
		    }
		    ఒకవేళ (ASCII_ISALPHA(*p) && చివరి_cmd != CMD_చట్రం)
			చివరి_cmd = 0;
		}
	    }

	    ఒకవేళ (చివరి_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		ఎంపిక (చివరి_cmd)
		{
		    స్థితి CMD_CONT:
			debug_break_level = -1;
			ఆపు;
		    స్థితి CMD_తదుపరి:
			debug_break_level = ex_nesting_level;
			ఆపు;
		    స్థితి CMD_అడుగు:
			debug_break_level = 9999;
			ఆపు;
		    స్థితి CMD_ముగింపు:
			debug_break_level = ex_nesting_level - 1;
			ఆపు;
		    స్థితి CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			ఆపు;
		    స్థితి CMD_అంతరాయం:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			చివరి_cmd = CMD_అడుగు;
			ఆపు;
		    స్థితి CMD_BACKTRACE:
			do_showbacktrace(cmd);
			కొనసాగు;
		    స్థితి CMD_చట్రం:
			ఒకవేళ (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			లేకపోతే
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			కొనసాగు;
		    స్థితి CMD_పైకి:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			కొనసాగు;
		    స్థితి CMD_కిందికి:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			కొనసాగు;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		ఆపు;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (శూన్య)do_cmdline(cmdline, getexline, ఖాళీ,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	పంక్తులు_ఎడమ = అడ్డువరుసలు - 1;
    }
    vim_free(cmdline);

    ఒకవేళ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    సందేశం_scroll = save_సందేశం_scroll;
    restore_timeout_for_debugging();
    పంక్తులు_ఎడమ = అడ్డువరుసలు - 1;
    State = save_State;
    debug_బహుళకం = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    సందేశం_silent = save_సందేశం_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_సందేశం = TRUE;
}

    స్థిర పూర్ణాంకం
get_maxbacktrace_level(char_u *sname)
{
    అక్షరం	*p, *q;
    పూర్ణాంకం		maxbacktrace = 0;

    ఒకవేళ (sname == ఖాళీ)
	తిరిగి 0;

    p = (అక్షరం *)sname;
    అయితే ((q = అక్షరశ్రేణి_శోధన(p, "..")) != ఖాళీ)
    {
	p = q + 2;
	maxbacktrace++;
    }
    తిరిగి maxbacktrace;
}

    స్థిర శూన్య
do_setdebugtracelevel(char_u *arg)
{
    పూర్ణాంకం level;

    level = atoi((అక్షరం *)arg);
    ఒకవేళ (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    లేకపోతే
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    స్థిర శూన్య
do_checkbacktracelevel(శూన్య)
{
    ఒకవేళ (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	సందేశం(_("frame is zero"));
    }
    లేకపోతే
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	పూర్ణాంకం	max = get_maxbacktrace_level(sname);

	ఒకవేళ (debug_backtrace_level > max)
	{
	    debug_backtrace_level = max;
	    smsg(_("frame at highest level: %d"), max);
	}
	vim_free(sname);
    }
}

    స్థిర శూన్య
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    అక్షరం    *cur;
    అక్షరం    *తదుపరి;
    పూర్ణాంకం	    i = 0;
    పూర్ణాంకం	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    ఒకవేళ (sname != ఖాళీ)
    {
	cur = (అక్షరం *)sname;
	అయితే (!got_int)
	{
	    తదుపరి = అక్షరశ్రేణి_శోధన(cur, "..");
	    ఒకవేళ (తదుపరి != ఖాళీ)
		*తదుపరి = NUL;
	    ఒకవేళ (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    లేకపోతే
		smsg("  %d %s", max - i, cur);
	    ++i;
	    ఒకవేళ (తదుపరి == ఖాళీ)
		ఆపు;
	    *తదుపరి = '.';
	    cur = తదుపరి + 2;
	}
	vim_free(sname);
    }

    ఒకవేళ (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (దీర్ఘ)SOURCING_LNUM, cmd);
    లేకపోతే
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    శూన్య
ex_debug(exarg_T *eap)
{
    పూర్ణాంకం		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

స్థిర char_u	*debug_breakpoint_name = ఖాళీ;
స్థిర linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
స్థిర పూర్ణాంకం	debug_skipped;
స్థిర char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    శూన్య
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    ఒకవేళ (debug_breakpoint_name != ఖాళీ)
    {
	ఒకవేళ (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    ఒకవేళ (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    లేకపోతే
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (దీర్ఘ)debug_breakpoint_lnum);
	    debug_breakpoint_name = ఖాళీ;
	    do_debug(eap->cmd);
	}
	లేకపోతే
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = ఖాళీ;
	}
    }
    లేకపోతే ఒకవేళ (ex_nesting_level <= debug_break_level)
    {
	ఒకవేళ (!eap->skip)
	    do_debug(eap->cmd);
	లేకపోతే
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = ఖాళీ;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    పూర్ణాంకం
dbg_check_skipped(exarg_T *eap)
{
    పూర్ణాంకం		prev_got_int;

    ఒకవేళ (!debug_skipped)
	తిరిగి FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    prev_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (శూన్య)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= prev_got_int;
    తిరిగి TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
నిర్మాణం debuggy
{
    పూర్ణాంకం		dbg_nr;		// breakpoint number
    పూర్ణాంకం		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    పూర్ణాంకం		dbg_forceit;	// ! used
#నిర్వచితమైతే FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#అయితే_ముగింపు
    పూర్ణాంకం		dbg_level;      // stored nested level for expr
};

స్థిర garray_T dbg_breakp = {0, 0, పరిమాణం(నిర్మాణం debuggy), 4, ఖాళీ};
#నిర్వచించు BREAKP(idx)		(((నిర్మాణం debuggy *)dbg_breakp.ga_దత్తాంశం)[idx])
#నిర్వచించు DEBUGGY(gap, idx)	(((నిర్మాణం debuggy *)gap->ga_దత్తాంశం)[idx])
స్థిర పూర్ణాంకం చివరి_breakp = 0;	// nr of last defined breakpoint
స్థిర పూర్ణాంకం has_సమాసం_breakpoint = FALSE;

#నిర్వచితమైతే FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
స్థిర garray_T prof_ga = {0, 0, పరిమాణం(నిర్మాణం debuggy), 4, ఖాళీ};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
స్థిర hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
స్థిర పూర్ణాంకం prof_cache_initialized;
రకనిర్వచనం నిర్మాణం profentry_S
{
    అక్షరం    pen_flags;	// cache data booleans: profiling, forceit
    char_u  pen_name[1]; // actually longer
} profentry_T;
# define PEN_FLAG_PROFILING 1
# define PEN_FLAG_FORCEIT 2
# define PEN_సమితి_PROFILING(pe) ((pe)->pen_flags |= PEN_FLAG_PROFILING)
# define PEN_సమితి_FORCEIT(pe) ((pe)->pen_flags |= PEN_FLAG_FORCEIT)
# define PEN_IS_PROFILING(pe) (((pe)->pen_flags & PEN_FLAG_PROFILING) != 0)
# define PEN_IS_FORCEIT(pe) (((pe)->pen_flags & PEN_FLAG_FORCEIT) != 0)

# define PE2HIKEY(pe)	((pe)->pen_name)
# define HIKEY2PE(p)	((profentry_T *)((p) - (offsetof(profentry_T, pen_name))))
# define HI2PE(hi)	HIKEY2PE((hi)->hi_కీలకం)

స్థిర శూన్య prof_clear_cache(శూన్య);
# define PROF_CLEAR_CACHE(gap) చేయి {ఒకవేళ ((gap) == &prof_ga) prof_clear_cache();} అయితే (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#లేకపోతే
# define PROF_CLEAR_CACHE(gap) చేయి {} అయితే (0)
#అయితే_ముగింపు
#నిర్వచించు DBG_FUNC	1
#నిర్వచించు DBG_FILE	2
#నిర్వచించు DBG_సమాసం	3

స్థిర linenr_T debuggy_కనుగొను(పూర్ణాంకం file,char_u *fname, linenr_T after, garray_T *gap, పూర్ణాంకం *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    స్థిర typval_T *
eval_సమాసం_no_emsg(నిర్మాణం debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_సమాసం(bp->dbg_name, ఖాళీ);
    --emsg_off;

    తిరిగి tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    స్థిర పూర్ణాంకం
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    నిర్మాణం debuggy *bp;
    పూర్ణాంకం		ఇక్కడ = FALSE;

    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    ఒకవేళ (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    లేకపోతే ఒకవేళ (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    లేకపోతే ఒకవేళ (
#నిర్వచితమైతే FEAT_PROFILE
	    gap != &prof_ga &&
#అయితే_ముగింపు
	    STRNCMP(p, "here", 4) == 0)
    {
	ఒకవేళ (curbuf->b_ffname == ఖాళీ)
	{
	    emsg(_(e_no_file_name));
	    తిరిగి FAIL;
	}
	bp->dbg_type = DBG_FILE;
	ఇక్కడ = TRUE;
    }
    లేకపోతే ఒకవేళ (
#నిర్వచితమైతే FEAT_PROFILE
	    gap != &prof_ga &&
#అయితే_ముగింపు
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_సమాసం;
    లేకపోతే
    {
	semsg(_(e_invalid_వాదం_str), p);
	తిరిగి FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    ఒకవేళ (ఇక్కడ)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    లేకపోతే ఒకవేళ (
#నిర్వచితమైతే FEAT_PROFILE
	    gap != &prof_ga &&
#అయితే_ముగింపు
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    లేకపోతే
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    ఒకవేళ ((!ఇక్కడ && *p == NUL)
	    || (ఇక్కడ && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && అక్షరశ్రేణి_శోధన((అక్షరం *)p, "()") != ఖాళీ))
    {
	semsg(_(e_invalid_వాదం_str), arg);
	తిరిగి FAIL;
    }

    ఒకవేళ (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    లేకపోతే ఒకవేళ (ఇక్కడ)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    లేకపోతే ఒకవేళ (bp->dbg_type == DBG_సమాసం)
    {
	bp->dbg_name = vim_strsave(p);
	ఒకవేళ (bp->dbg_name != ఖాళీ)
	    bp->dbg_val = eval_సమాసం_no_emsg(bp);
    }
    లేకపోతే
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	ఒకవేళ (q == ఖాళీ)
	    తిరిగి FAIL;
	p = expand_env_save(q);
	vim_free(q);
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి FAIL;
	ఒకవేళ (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	లేకపోతే
	    bp->dbg_name = p;
    }

    ఒకవేళ (bp->dbg_name == ఖాళీ)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    శూన్య
ex_breakadd(exarg_T *eap)
{
    నిర్మాణం debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#అయితే_ముగింపు

    ఒకవేళ (dbg_parsearg(eap->arg, gap) != OK)
	తిరిగి;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    ఒకవేళ (bp->dbg_type != DBG_సమాసం)
    {
	pat = file_pat_గమ్యం_reg_pat(bp->dbg_name, ఖాళీ, ఖాళీ, FALSE);
	ఒకవేళ (pat != ఖాళీ)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	ఒకవేళ (pat == ఖాళీ || bp->dbg_prog == ఖాళీ)
	    vim_free(bp->dbg_name);
	లేకపోతే
	{
	    ఒకవేళ (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#నిర్వచితమైతే FEAT_PROFILE
	    ఒకవేళ (eap->cmdidx != CMD_profile)
#అయితే_ముగింపు
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++చివరి_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    లేకపోతే
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++చివరి_breakp;
	++debug_tick;
	ఒకవేళ (gap == &dbg_breakp)
	    has_సమాసం_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    శూన్య
ex_debuggreedy(exarg_T *eap)
{
    ఒకవేళ (eap->addr_లెక్క == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    లేకపోతే
	debug_greedy = FALSE;
}

    స్థిర శూన్య
update_has_సమాసం_breakpoint(శూన్య)
{
    పూర్ణాంకం i;

    has_సమాసం_breakpoint = FALSE;
    కొరకు (i = 0; i < dbg_breakp.ga_len; ++i)
	ఒకవేళ (BREAKP(i).dbg_type == DBG_సమాసం)
	{
	    has_సమాసం_breakpoint = TRUE;
	    ఆపు;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    పూర్ణాంకం
debug_has_సమాసం_breakpoint(శూన్య)
{
    తిరిగి has_సమాసం_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    శూన్య
ex_breakdel(exarg_T *eap)
{
    నిర్మాణం debuggy *bp, *bpi;
    పూర్ణాంకం		nr;
    పూర్ణాంకం		todel = -1;
    పూర్ణాంకం		del_all = FALSE;
    పూర్ణాంకం		i;
    linenr_T	ఉత్తమ_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    ఒకవేళ (eap->cmdidx == CMD_profdel)
    {
#నిర్వచితమైతే FEAT_PROFILE
	gap = &prof_ga;
#లేకపోతే
	ex_ni(eap);
	తిరిగి;
#అయితే_ముగింపు
    }

    ఒకవేళ (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((అక్షరం *)eap->arg);
	కొరకు (i = 0; i < gap->ga_len; ++i)
	    ఒకవేళ (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		ఆపు;
	    }
    }
    లేకపోతే ఒకవేళ (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    లేకపోతే
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	ఒకవేళ (dbg_parsearg(eap->arg, gap) == FAIL)
	    తిరిగి;
	bp = &DEBUGGY(gap, gap->ga_len);
	కొరకు (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    ఒకవేళ (bp->dbg_type == bpi->dbg_type
		    && STRCMP(bp->dbg_name, bpi->dbg_name) == 0
		    && (bp->dbg_lnum == bpi->dbg_lnum
			|| (bp->dbg_lnum == 0
			    && (ఉత్తమ_lnum == 0
				|| bpi->dbg_lnum < ఉత్తమ_lnum))))
	    {
		todel = i;
		ఉత్తమ_lnum = bpi->dbg_lnum;
	    }
	}
	vim_free(bp->dbg_name);
    }

    ఒకవేళ (todel < 0)
    {
	semsg(_(e_breakpoint_not_దొరికింది_str), eap->arg);
	తిరిగి;
    }

    అయితే (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (DEBUGGY(gap, todel).dbg_type == DBG_సమాసం
		&& DEBUGGY(gap, todel).dbg_val != ఖాళీ)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#అయితే_ముగింపు
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	ఒకవేళ (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * పరిమాణం(నిర్మాణం debuggy));
#నిర్వచితమైతే FEAT_PROFILE
	ఒకవేళ (eap->cmdidx == CMD_breakdel)
#అయితే_ముగింపు
	    ++debug_tick;
	ఒకవేళ (!del_all)
	    ఆపు;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    ఒకవేళ (gap->ga_len == 0)
	ga_clear(gap);
    ఒకవేళ (gap == &dbg_breakp)
	update_has_సమాసం_breakpoint();
}

/*
 * ":breaklist".
 */
    శూన్య
ex_breaklist(exarg_T *eap వాడని)
{
    నిర్మాణం debuggy *bp;
    పూర్ణాంకం		i;

    ఒకవేళ (dbg_breakp.ga_len == 0)
    {
	సందేశం(_("No breakpoints defined"));
	తిరిగి;
    }

    కొరకు (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	ఒకవేళ (bp->dbg_type == DBG_FILE)
	    home_replace(ఖాళీ, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	ఒకవేళ (bp->dbg_type != DBG_సమాసం)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (దీర్ఘ)bp->dbg_lnum);
	లేకపోతే
	    smsg(_("%3d  expr %s"),
		    bp->dbg_nr, bp->dbg_name);
    }
}

/*
 * Find a breakpoint for a function or sourced file.
 * Returns line number at which to break; zero when no matching breakpoint.
 */
    linenr_T
dbg_కనుగొను_breakpoint(
    పూర్ణాంకం		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    తిరిగి debuggy_కనుగొను(file, fname, after, &dbg_breakp, ఖాళీ);
}

#ఒకవేళ defined(FEAT_PROFILE)
# ఒకవేళ defined(PROF_CACHE_LOG)
స్థిర పూర్ణాంకం లెక్క_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    పూర్ణాంకం
has_profiling(
    పూర్ణాంకం		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    పూర్ణాంకం		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    ఒకవేళ (prof_ga.ga_len == 0 || !prof_cache_initialized)
	తిరిగి debuggy_కనుగొను(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    ఒకవేళ (hashp != ఖాళీ)
    {
	hash = *hashp;
	ఒకవేళ (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    లేకపోతే
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	ఒకవేళ (pe == ఖాళీ)
	    తిరిగి FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	పూర్ణాంకం f;
	పూర్ణాంకం lnum = debuggy_కనుగొను(file, fname, (linenr_T)0, &prof_ga, &f);
	ఒకవేళ (lnum)
	{
	    PEN_సమితి_PROFILING(pe);
	    ఒకవేళ (f)
		PEN_సమితి_FORCEIT(pe);
	}
	hash_కూడు_అంశం(ht, hi, pe->pen_name, hash);
# ఒకవేళ defined(PROF_CACHE_LOG)
	ch_log(ఖాళీ, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    లేకపోతే
	pe = HI2PE(hi);
    ఒకవేళ (fp)
	*fp = PEN_IS_FORCEIT(pe);
# ఒకవేళ defined(PROF_CACHE_LOG)
    లెక్క_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    తిరిగి PEN_IS_PROFILING(pe);
}

    స్థిర శూన్య
prof_clear_cache(శూన్య)
{
    ఒకవేళ (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    కొరకు (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	ఒకవేళ (ht->ht_వాడిన > 0)
	{
# ఒకవేళ defined(PROF_CACHE_LOG)
	    పూర్ణాంకం idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(ఖాళీ, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_వాడిన, లెక్క_lookups[idx]);
	    లెక్క_lookups[idx] = 0;
# endif
	    hash_clear_all(ht, offsetof(profentry_T, pen_name));
	    hash_init(ht);
	}
    }
}
#అయితే_ముగింపు

/*
 * Common code for dbg_find_breakpoint() and has_profiling().
 */
    స్థిర linenr_T
debuggy_కనుగొను(
    పూర్ణాంకం		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    పూర్ణాంకం		*fp)	    // if not NULL: return forceit
{
    నిర్మాణం debuggy *bp;
    పూర్ణాంకం		i;
    linenr_T	lnum = 0;
    char_u	*name = ఖాళీ;
    char_u	*short_name = fname;
    పూర్ణాంకం		prev_got_int;

    // Return quickly when there are no breakpoints.
    ఒకవేళ (gap->ga_len == 0)
	తిరిగి (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    ఒకవేళ (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	ఒకవేళ (name != ఖాళీ)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	ఒకవేళ (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_సమాసం && (
#నిర్వచితమైతే FEAT_PROFILE
		gap == &prof_ga ||
#అయితే_ముగింపు
		(bp->dbg_lnum > after && (lnum == 0 || bp->dbg_lnum < lnum)))))
	{
	    // Save the value of got_int and reset it.  We don't want a
	    // previous interruption cancel matching, only hitting CTRL-C
	    // while matching should abort it.
	    prev_got_int = got_int;
	    got_int = FALSE;
	    ఒకవేళ ((name != ఖాళీ
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		ఒకవేళ (fp != ఖాళీ)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#నిర్వచితమైతే FEAT_PROFILE
	    ఒకవేళ (lnum && gap == &prof_ga)
		ఆపు;
#అయితే_ముగింపు
	}
#నిర్వచితమైతే FEAT_EVAL
	లేకపోతే ఒకవేళ (bp->dbg_type == DBG_సమాసం)
	{
	    typval_T *tv;
	    పూర్ణాంకం	      పంక్తి = FALSE;

	    tv = eval_సమాసం_no_emsg(bp);
	    ఒకవేళ (tv != ఖాళీ)
	    {
		ఒకవేళ (bp->dbg_val == ఖాళీ)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(ఖాళీ, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    పంక్తి = TRUE;
		}
		లేకపోతే
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? సమాసం_EQUAL : సమాసం_IS;

		    ఒకవేళ (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_సంఖ్య == FALSE)
		    {
			typval_T *v;

			పంక్తి = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_సమాసం_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    లేకపోతే ఒకవేళ (bp->dbg_val != ఖాళీ)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(ఖాళీ, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = ఖాళీ;
		పంక్తి = TRUE;
	    }

	    ఒకవేళ (పంక్తి)
	    {
		lnum = after > 0 ? after : 1;
		ఆపు;
	    }
	}
#అయితే_ముగింపు
    }
    ఒకవేళ (name != fname)
	vim_free(name);

    తిరిగి lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    శూన్య
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#అయితే_ముగింపు
