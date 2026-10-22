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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)
સ્થિર પૂર્ણાંક debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
સ્થિર શૂન્ય do_setdebugtracelevel(char_u *arg);
સ્થિર શૂન્ય do_checkbacktracelevel(શૂન્ય);
સ્થિર શૂન્ય do_showbacktrace(char_u *cmd);

સ્થિર char_u *debug_oldval = ખાલી;	// old and newval for debug expressions
સ્થિર char_u *debug_newval = ખાલી;
સ્થિર પૂર્ણાંક     debug_પદાવલિ   = 0;	// use debug_expr

    પૂર્ણાંક
has_watchexpr(શૂન્ય)
{
    પાછા debug_પદાવલિ;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    શૂન્ય
do_debug(char_u *cmd)
{
    પૂર્ણાંક		save_સંદેશ_scroll = સંદેશ_scroll;
    પૂર્ણાંક		save_State = State;
    પૂર્ણાંક		save_did_emsg = did_emsg;
    પૂર્ણાંક		save_cmd_silent = cmd_silent;
    પૂર્ણાંક		save_સંદેશ_silent = સંદેશ_silent;
    પૂર્ણાંક		save_emsg_silent = emsg_silent;
    પૂર્ણાંક		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    પૂર્ણાંક		typeahead_saved = FALSE;
    પૂર્ણાંક		save_ignore_script = 0;
    પૂર્ણાંક		save_ex_normal_busy;
    પૂર્ણાંક		n;
    char_u	*cmdline = ખાલી;
    char_u	*p;
    char_u	*sname;
    અક્ષર	*tail = ખાલી;
    સ્થિર પૂર્ણાંક	last_cmd = 0;
#define CMD_CONT	1
#define CMD_NEXT	2
#define CMD_STEP	3
#define CMD_FINISH	4
#define CMD_QUIT	5
#define CMD_INTERRUPT	6
#define CMD_BACKTRACE	7
#define CMD_FRAME	8
#define CMD_UP		9
#define CMD_DOWN	10

#ifdef ALWAYS_USE_GUI
    // Can't do this when there is no terminal for input/output.
    જો (!gui.in_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	પાછા;
    }
#endif

    // Make sure we are in raw mode and start termcap mode.  Might have side
    // effects...
    settmode(TMODE_RAW);
    starttermcap();

    ++RedrawingDisabled;	// don't redisplay the window
    ++no_wait_return;		// don't wait for return
    did_emsg = FALSE;		// don't use error from debugged stuff
    cmd_silent = FALSE;		// display commands
    સંદેશ_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = MODE_NORMAL;
    debug_mode = TRUE;

    જો (!debug_did_સંદેશ)
	સંદેશ(_("Entering Debug mode.  Type \"cont\" to continue."));
    જો (debug_oldval != ખાલી)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    જો (debug_newval != ખાલી)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    જો (sname != ખાલી)
	સંદેશ((અક્ષર *)sname);
    vim_free(sname);
    જો (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    નહીંતર
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    માટે (;;)
    {
	સંદેશ_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_normal_busy = ex_normal_busy;
	ex_normal_busy = 0;
	જો (!debug_greedy)
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
	cmdline = getcmdline_prompt('>', ખાલી, 0, EXPAND_NOTHING, ખાલી);

	debug_break_level = n;
	જો (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_normal_busy = save_ex_normal_busy;

	cmdline_row = સંદેશ_row;
	સંદેશ_starthere();
	જો (cmdline != ખાલી)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    જો (*p != NUL)
	    {
		પસંદગી (*p)
		{
		    સ્થિતિ 'c': last_cmd = CMD_CONT;
			      tail = "ont";
			      તોડો;
		    સ્થિતિ 'n': last_cmd = CMD_NEXT;
			      tail = "ext";
			      તોડો;
		    સ્થિતિ 's': last_cmd = CMD_STEP;
			      tail = "tep";
			      તોડો;
		    સ્થિતિ 'f':
			      last_cmd = 0;
			      જો (p[1] == 'r')
			      {
				  last_cmd = CMD_FRAME;
				  tail = "rame";
			      }
			      નહીંતર
			      {
				  last_cmd = CMD_FINISH;
				  tail = "inish";
			      }
			      તોડો;
		    સ્થિતિ 'q': last_cmd = CMD_QUIT;
			      tail = "uit";
			      તોડો;
		    સ્થિતિ 'i': last_cmd = CMD_INTERRUPT;
			      tail = "nterrupt";
			      તોડો;
		    સ્થિતિ 'b': last_cmd = CMD_BACKTRACE;
			      જો (p[1] == 't')
				  tail = "t";
			      નહીંતર
				  tail = "acktrace";
			      તોડો;
		    સ્થિતિ 'w': last_cmd = CMD_BACKTRACE;
			      tail = "here";
			      તોડો;
		    સ્થિતિ 'u': last_cmd = CMD_UP;
			      tail = "p";
			      તોડો;
		    સ્થિતિ 'd': last_cmd = CMD_DOWN;
			      tail = "own";
			      તોડો;
		    મૂળભૂત: last_cmd = 0;
		}
		જો (last_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    જ્યારે (*p != NUL && *p == *tail)
		    {
			++p;
			++tail;
		    }
		    જો (ASCII_ISALPHA(*p) && last_cmd != CMD_FRAME)
			last_cmd = 0;
		}
	    }

	    જો (last_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		પસંદગી (last_cmd)
		{
		    સ્થિતિ CMD_CONT:
			debug_break_level = -1;
			તોડો;
		    સ્થિતિ CMD_NEXT:
			debug_break_level = ex_nesting_level;
			તોડો;
		    સ્થિતિ CMD_STEP:
			debug_break_level = 9999;
			તોડો;
		    સ્થિતિ CMD_FINISH:
			debug_break_level = ex_nesting_level - 1;
			તોડો;
		    સ્થિતિ CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			તોડો;
		    સ્થિતિ CMD_INTERRUPT:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			last_cmd = CMD_STEP;
			તોડો;
		    સ્થિતિ CMD_BACKTRACE:
			do_showbacktrace(cmd);
			ચાલુ;
		    સ્થિતિ CMD_FRAME:
			જો (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			નહીંતર
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			ચાલુ;
		    સ્થિતિ CMD_UP:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			ચાલુ;
		    સ્થિતિ CMD_DOWN:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			ચાલુ;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		તોડો;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (શૂન્ય)do_cmdline(cmdline, getexline, ખાલી,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	લીટીઓ_ડાબું = Rows - 1;
    }
    vim_free(cmdline);

    જો (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    સંદેશ_scroll = save_સંદેશ_scroll;
    restore_timeout_for_debugging();
    લીટીઓ_ડાબું = Rows - 1;
    State = save_State;
    debug_mode = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    સંદેશ_silent = save_સંદેશ_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_સંદેશ = TRUE;
}

    સ્થિર પૂર્ણાંક
get_maxbacktrace_level(char_u *sname)
{
    અક્ષર	*p, *q;
    પૂર્ણાંક		maxbacktrace = 0;

    જો (sname == ખાલી)
	પાછા 0;

    p = (અક્ષર *)sname;
    જ્યારે ((q = strstr(p, "..")) != ખાલી)
    {
	p = q + 2;
	maxbacktrace++;
    }
    પાછા maxbacktrace;
}

    સ્થિર શૂન્ય
do_setdebugtracelevel(char_u *arg)
{
    પૂર્ણાંક level;

    level = atoi((અક્ષર *)arg);
    જો (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    નહીંતર
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    સ્થિર શૂન્ય
do_checkbacktracelevel(શૂન્ય)
{
    જો (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	સંદેશ(_("frame is zero"));
    }
    નહીંતર
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	પૂર્ણાંક	max = get_maxbacktrace_level(sname);

	જો (debug_backtrace_level > max)
	{
	    debug_backtrace_level = max;
	    smsg(_("frame at highest level: %d"), max);
	}
	vim_free(sname);
    }
}

    સ્થિર શૂન્ય
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    અક્ષર    *cur;
    અક્ષર    *next;
    પૂર્ણાંક	    i = 0;
    પૂર્ણાંક	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    જો (sname != ખાલી)
    {
	cur = (અક્ષર *)sname;
	જ્યારે (!got_int)
	{
	    next = strstr(cur, "..");
	    જો (next != ખાલી)
		*next = NUL;
	    જો (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    નહીંતર
		smsg("  %d %s", max - i, cur);
	    ++i;
	    જો (next == ખાલી)
		તોડો;
	    *next = '.';
	    cur = next + 2;
	}
	vim_free(sname);
    }

    જો (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (long)SOURCING_LNUM, cmd);
    નહીંતર
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    શૂન્ય
ex_debug(exarg_T *eap)
{
    પૂર્ણાંક		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

સ્થિર char_u	*debug_breakpoint_name = ખાલી;
સ્થિર linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
સ્થિર પૂર્ણાંક	debug_skipped;
સ્થિર char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    શૂન્ય
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    જો (debug_breakpoint_name != ખાલી)
    {
	જો (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    જો (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    નહીંતર
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (long)debug_breakpoint_lnum);
	    debug_breakpoint_name = ખાલી;
	    do_debug(eap->cmd);
	}
	નહીંતર
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = ખાલી;
	}
    }
    નહીંતર જો (ex_nesting_level <= debug_break_level)
    {
	જો (!eap->skip)
	    do_debug(eap->cmd);
	નહીંતર
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = ખાલી;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    પૂર્ણાંક
dbg_check_skipped(exarg_T *eap)
{
    પૂર્ણાંક		prev_got_int;

    જો (!debug_skipped)
	પાછા FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    prev_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (શૂન્ય)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= prev_got_int;
    પાછા TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
સંરચના debuggy
{
    પૂર્ણાંક		dbg_nr;		// breakpoint number
    પૂર્ણાંક		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    પૂર્ણાંક		dbg_forceit;	// ! used
#ifdef FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#endif
    પૂર્ણાંક		dbg_level;      // stored nested level for expr
};

સ્થિર garray_T dbg_breakp = {0, 0, કદ(સંરચના debuggy), 4, ખાલી};
#define BREAKP(idx)		(((સંરચના debuggy *)dbg_breakp.ga_data)[idx])
#define DEBUGGY(gap, idx)	(((સંરચના debuggy *)gap->ga_data)[idx])
સ્થિર પૂર્ણાંક last_breakp = 0;	// nr of last defined breakpoint
સ્થિર પૂર્ણાંક has_પદાવલિ_breakpoint = FALSE;

#ifdef FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
સ્થિર garray_T prof_ga = {0, 0, કદ(સંરચના debuggy), 4, ખાલી};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
સ્થિર hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
સ્થિર પૂર્ણાંક prof_cache_initialized;
પ્રકારવ્યાખ્યા સંરચના profentry_S
{
    અક્ષર    pen_flags;	// cache data booleans: profiling, forceit
    char_u  pen_name[1]; // actually longer
} profentry_T;
# define PEN_FLAG_PROFILING 1
# define PEN_FLAG_FORCEIT 2
# define PEN_SET_PROFILING(pe) ((pe)->pen_flags |= PEN_FLAG_PROFILING)
# define PEN_SET_FORCEIT(pe) ((pe)->pen_flags |= PEN_FLAG_FORCEIT)
# define PEN_IS_PROFILING(pe) (((pe)->pen_flags & PEN_FLAG_PROFILING) != 0)
# define PEN_IS_FORCEIT(pe) (((pe)->pen_flags & PEN_FLAG_FORCEIT) != 0)

# define PE2HIKEY(pe)	((pe)->pen_name)
# define HIKEY2PE(p)	((profentry_T *)((p) - (offsetof(profentry_T, pen_name))))
# define HI2PE(hi)	HIKEY2PE((hi)->hi_ચાવી)

સ્થિર શૂન્ય prof_clear_cache(શૂન્ય);
# define PROF_CLEAR_CACHE(gap) કરો {જો ((gap) == &prof_ga) prof_clear_cache();} જ્યારે (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#નહીંતર
# define PROF_CLEAR_CACHE(gap) કરો {} જ્યારે (0)
#endif
#define DBG_FUNC	1
#define DBG_FILE	2
#define DBG_પદાવલિ	3

સ્થિર linenr_T debuggy_find(પૂર્ણાંક file,char_u *fname, linenr_T after, garray_T *gap, પૂર્ણાંક *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    સ્થિર typval_T *
eval_પદાવલિ_no_emsg(સંરચના debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_પદાવલિ(bp->dbg_name, ખાલી);
    --emsg_off;

    પાછા tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    સ્થિર પૂર્ણાંક
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    સંરચના debuggy *bp;
    પૂર્ણાંક		here = FALSE;

    જો (ga_grow(gap, 1) == FAIL)
	પાછા FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    જો (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    નહીંતર જો (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    નહીંતર જો (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "here", 4) == 0)
    {
	જો (curbuf->b_ffname == ખાલી)
	{
	    emsg(_(e_no_file_name));
	    પાછા FAIL;
	}
	bp->dbg_type = DBG_FILE;
	here = TRUE;
    }
    નહીંતર જો (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_પદાવલિ;
    નહીંતર
    {
	semsg(_(e_invalid_argument_str), p);
	પાછા FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    જો (here)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    નહીંતર જો (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    નહીંતર
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    જો ((!here && *p == NUL)
	    || (here && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && strstr((અક્ષર *)p, "()") != ખાલી))
    {
	semsg(_(e_invalid_argument_str), arg);
	પાછા FAIL;
    }

    જો (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    નહીંતર જો (here)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    નહીંતર જો (bp->dbg_type == DBG_પદાવલિ)
    {
	bp->dbg_name = vim_strsave(p);
	જો (bp->dbg_name != ખાલી)
	    bp->dbg_val = eval_પદાવલિ_no_emsg(bp);
    }
    નહીંતર
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	જો (q == ખાલી)
	    પાછા FAIL;
	p = expand_env_save(q);
	vim_free(q);
	જો (p == ખાલી)
	    પાછા FAIL;
	જો (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	નહીંતર
	    bp->dbg_name = p;
    }

    જો (bp->dbg_name == ખાલી)
	પાછા FAIL;
    પાછા OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    શૂન્ય
ex_breakadd(exarg_T *eap)
{
    સંરચના debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#ifdef FEAT_PROFILE
    જો (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#endif

    જો (dbg_parsearg(eap->arg, gap) != OK)
	પાછા;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    જો (bp->dbg_type != DBG_પદાવલિ)
    {
	pat = file_pat_to_reg_pat(bp->dbg_name, ખાલી, ખાલી, FALSE);
	જો (pat != ખાલી)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	જો (pat == ખાલી || bp->dbg_prog == ખાલી)
	    vim_free(bp->dbg_name);
	નહીંતર
	{
	    જો (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#ifdef FEAT_PROFILE
	    જો (eap->cmdidx != CMD_profile)
#endif
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++last_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    નહીંતર
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++last_breakp;
	++debug_tick;
	જો (gap == &dbg_breakp)
	    has_પદાવલિ_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    શૂન્ય
ex_debuggreedy(exarg_T *eap)
{
    જો (eap->addr_ગણતરી == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    નહીંતર
	debug_greedy = FALSE;
}

    સ્થિર શૂન્ય
update_has_પદાવલિ_breakpoint(શૂન્ય)
{
    પૂર્ણાંક i;

    has_પદાવલિ_breakpoint = FALSE;
    માટે (i = 0; i < dbg_breakp.ga_len; ++i)
	જો (BREAKP(i).dbg_type == DBG_પદાવલિ)
	{
	    has_પદાવલિ_breakpoint = TRUE;
	    તોડો;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    પૂર્ણાંક
debug_has_પદાવલિ_breakpoint(શૂન્ય)
{
    પાછા has_પદાવલિ_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    શૂન્ય
ex_breakdel(exarg_T *eap)
{
    સંરચના debuggy *bp, *bpi;
    પૂર્ણાંક		nr;
    પૂર્ણાંક		todel = -1;
    પૂર્ણાંક		del_all = FALSE;
    પૂર્ણાંક		i;
    linenr_T	best_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    જો (eap->cmdidx == CMD_profdel)
    {
#ifdef FEAT_PROFILE
	gap = &prof_ga;
#નહીંતર
	ex_ni(eap);
	પાછા;
#endif
    }

    જો (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((અક્ષર *)eap->arg);
	માટે (i = 0; i < gap->ga_len; ++i)
	    જો (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		તોડો;
	    }
    }
    નહીંતર જો (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    નહીંતર
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	જો (dbg_parsearg(eap->arg, gap) == FAIL)
	    પાછા;
	bp = &DEBUGGY(gap, gap->ga_len);
	માટે (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    જો (bp->dbg_type == bpi->dbg_type
		    && STRCMP(bp->dbg_name, bpi->dbg_name) == 0
		    && (bp->dbg_lnum == bpi->dbg_lnum
			|| (bp->dbg_lnum == 0
			    && (best_lnum == 0
				|| bpi->dbg_lnum < best_lnum))))
	    {
		todel = i;
		best_lnum = bpi->dbg_lnum;
	    }
	}
	vim_free(bp->dbg_name);
    }

    જો (todel < 0)
    {
	semsg(_(e_breakpoint_not_found_str), eap->arg);
	પાછા;
    }

    જ્યારે (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#ifdef FEAT_EVAL
	જો (DEBUGGY(gap, todel).dbg_type == DBG_પદાવલિ
		&& DEBUGGY(gap, todel).dbg_val != ખાલી)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#endif
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	જો (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * કદ(સંરચના debuggy));
#ifdef FEAT_PROFILE
	જો (eap->cmdidx == CMD_breakdel)
#endif
	    ++debug_tick;
	જો (!del_all)
	    તોડો;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    જો (gap->ga_len == 0)
	ga_clear(gap);
    જો (gap == &dbg_breakp)
	update_has_પદાવલિ_breakpoint();
}

/*
 * ":breaklist".
 */
    શૂન્ય
ex_breaklist(exarg_T *eap UNUSED)
{
    સંરચના debuggy *bp;
    પૂર્ણાંક		i;

    જો (dbg_breakp.ga_len == 0)
    {
	સંદેશ(_("No breakpoints defined"));
	પાછા;
    }

    માટે (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	જો (bp->dbg_type == DBG_FILE)
	    home_replace(ખાલી, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	જો (bp->dbg_type != DBG_પદાવલિ)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (long)bp->dbg_lnum);
	નહીંતર
	    smsg(_("%3d  expr %s"),
		    bp->dbg_nr, bp->dbg_name);
    }
}

/*
 * Find a breakpoint for a function or sourced file.
 * Returns line number at which to break; zero when no matching breakpoint.
 */
    linenr_T
dbg_find_breakpoint(
    પૂર્ણાંક		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    પાછા debuggy_find(file, fname, after, &dbg_breakp, ખાલી);
}

#જો defined(FEAT_PROFILE)
# જો defined(PROF_CACHE_LOG)
સ્થિર પૂર્ણાંક ગણતરી_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    પૂર્ણાંક
has_profiling(
    પૂર્ણાંક		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    પૂર્ણાંક		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    જો (prof_ga.ga_len == 0 || !prof_cache_initialized)
	પાછા debuggy_find(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    જો (hashp != ખાલી)
    {
	hash = *hashp;
	જો (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    નહીંતર
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    જો (HASHITEM_EMPTY(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	જો (pe == ખાલી)
	    પાછા FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	પૂર્ણાંક f;
	પૂર્ણાંક lnum = debuggy_find(file, fname, (linenr_T)0, &prof_ga, &f);
	જો (lnum)
	{
	    PEN_SET_PROFILING(pe);
	    જો (f)
		PEN_SET_FORCEIT(pe);
	}
	hash_add_item(ht, hi, pe->pen_name, hash);
# જો defined(PROF_CACHE_LOG)
	ch_log(ખાલી, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    નહીંતર
	pe = HI2PE(hi);
    જો (fp)
	*fp = PEN_IS_FORCEIT(pe);
# જો defined(PROF_CACHE_LOG)
    ગણતરી_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    પાછા PEN_IS_PROFILING(pe);
}

    સ્થિર શૂન્ય
prof_clear_cache(શૂન્ય)
{
    જો (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    માટે (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	જો (ht->ht_used > 0)
	{
# જો defined(PROF_CACHE_LOG)
	    પૂર્ણાંક idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(ખાલી, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_used, ગણતરી_lookups[idx]);
	    ગણતરી_lookups[idx] = 0;
# endif
	    hash_clear_all(ht, offsetof(profentry_T, pen_name));
	    hash_init(ht);
	}
    }
}
#endif

/*
 * Common code for dbg_find_breakpoint() and has_profiling().
 */
    સ્થિર linenr_T
debuggy_find(
    પૂર્ણાંક		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    પૂર્ણાંક		*fp)	    // if not NULL: return forceit
{
    સંરચના debuggy *bp;
    પૂર્ણાંક		i;
    linenr_T	lnum = 0;
    char_u	*name = ખાલી;
    char_u	*short_name = fname;
    પૂર્ણાંક		prev_got_int;

    // Return quickly when there are no breakpoints.
    જો (gap->ga_len == 0)
	પાછા (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    જો (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	જો (name != ખાલી)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    માટે (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	જો (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_પદાવલિ && (
#ifdef FEAT_PROFILE
		gap == &prof_ga ||
#endif
		(bp->dbg_lnum > after && (lnum == 0 || bp->dbg_lnum < lnum)))))
	{
	    // Save the value of got_int and reset it.  We don't want a
	    // previous interruption cancel matching, only hitting CTRL-C
	    // while matching should abort it.
	    prev_got_int = got_int;
	    got_int = FALSE;
	    જો ((name != ખાલી
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		જો (fp != ખાલી)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#ifdef FEAT_PROFILE
	    જો (lnum && gap == &prof_ga)
		તોડો;
#endif
	}
#ifdef FEAT_EVAL
	નહીંતર જો (bp->dbg_type == DBG_પદાવલિ)
	{
	    typval_T *tv;
	    પૂર્ણાંક	      લીટી = FALSE;

	    tv = eval_પદાવલિ_no_emsg(bp);
	    જો (tv != ખાલી)
	    {
		જો (bp->dbg_val == ખાલી)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(ખાલી, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    લીટી = TRUE;
		}
		નહીંતર
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? પદાવલિ_EQUAL : પદાવલિ_IS;

		    જો (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_સંખ્યા == FALSE)
		    {
			typval_T *v;

			લીટી = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_પદાવલિ_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    નહીંતર જો (bp->dbg_val != ખાલી)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(ખાલી, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = ખાલી;
		લીટી = TRUE;
	    }

	    જો (લીટી)
	    {
		lnum = after > 0 ? after : 1;
		તોડો;
	    }
	}
#endif
    }
    જો (name != fname)
	vim_free(name);

    પાછા lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    શૂન્ય
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#endif
