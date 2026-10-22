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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)
സ്ഥിതം പൂർണ്ണസംഖ്യ debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
സ്ഥിതം ശൂന്യം do_setdebugtracelevel(char_u *arg);
സ്ഥിതം ശൂന്യം do_checkbacktracelevel(ശൂന്യം);
സ്ഥിതം ശൂന്യം do_showbacktrace(char_u *cmd);

സ്ഥിതം char_u *debug_oldval = ശൂന്യ;	// old and newval for debug expressions
സ്ഥിതം char_u *debug_newval = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ     debug_വ്യഞ്ജകം   = 0;	// use debug_expr

    പൂർണ്ണസംഖ്യ
has_watchexpr(ശൂന്യം)
{
    തിരികെ debug_വ്യഞ്ജകം;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    ശൂന്യം
do_debug(char_u *cmd)
{
    പൂർണ്ണസംഖ്യ		save_സന്ദേശം_scroll = സന്ദേശം_scroll;
    പൂർണ്ണസംഖ്യ		save_State = State;
    പൂർണ്ണസംഖ്യ		save_did_emsg = did_emsg;
    പൂർണ്ണസംഖ്യ		save_cmd_silent = cmd_silent;
    പൂർണ്ണസംഖ്യ		save_സന്ദേശം_silent = സന്ദേശം_silent;
    പൂർണ്ണസംഖ്യ		save_emsg_silent = emsg_silent;
    പൂർണ്ണസംഖ്യ		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    പൂർണ്ണസംഖ്യ		typeahead_saved = FALSE;
    പൂർണ്ണസംഖ്യ		save_ignore_script = 0;
    പൂർണ്ണസംഖ്യ		save_ex_normal_busy;
    പൂർണ്ണസംഖ്യ		n;
    char_u	*cmdline = ശൂന്യ;
    char_u	*p;
    char_u	*sname;
    അക്ഷരം	*tail = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	last_cmd = 0;
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
    എങ്കിൽ (!gui.in_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	തിരികെ;
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
    സന്ദേശം_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = MODE_NORMAL;
    debug_mode = TRUE;

    എങ്കിൽ (!debug_did_സന്ദേശം)
	സന്ദേശം(_("Entering Debug mode.  Type \"cont\" to continue."));
    എങ്കിൽ (debug_oldval != ശൂന്യ)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    എങ്കിൽ (debug_newval != ശൂന്യ)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    എങ്കിൽ (sname != ശൂന്യ)
	സന്ദേശം((അക്ഷരം *)sname);
    vim_free(sname);
    എങ്കിൽ (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    അല്ലെങ്കിൽ
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    വേണ്ടി (;;)
    {
	സന്ദേശം_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_normal_busy = ex_normal_busy;
	ex_normal_busy = 0;
	എങ്കിൽ (!debug_greedy)
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
	cmdline = getcmdline_prompt('>', ശൂന്യ, 0, EXPAND_NOTHING, ശൂന്യ);

	debug_break_level = n;
	എങ്കിൽ (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_normal_busy = save_ex_normal_busy;

	cmdline_row = സന്ദേശം_row;
	സന്ദേശം_starthere();
	എങ്കിൽ (cmdline != ശൂന്യ)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    എങ്കിൽ (*p != NUL)
	    {
		തിരഞ്ഞെടുപ്പ് (*p)
		{
		    അവസ്ഥ 'c': last_cmd = CMD_CONT;
			      tail = "ont";
			      നിർത്തുക;
		    അവസ്ഥ 'n': last_cmd = CMD_NEXT;
			      tail = "ext";
			      നിർത്തുക;
		    അവസ്ഥ 's': last_cmd = CMD_STEP;
			      tail = "tep";
			      നിർത്തുക;
		    അവസ്ഥ 'f':
			      last_cmd = 0;
			      എങ്കിൽ (p[1] == 'r')
			      {
				  last_cmd = CMD_FRAME;
				  tail = "rame";
			      }
			      അല്ലെങ്കിൽ
			      {
				  last_cmd = CMD_FINISH;
				  tail = "inish";
			      }
			      നിർത്തുക;
		    അവസ്ഥ 'q': last_cmd = CMD_QUIT;
			      tail = "uit";
			      നിർത്തുക;
		    അവസ്ഥ 'i': last_cmd = CMD_INTERRUPT;
			      tail = "nterrupt";
			      നിർത്തുക;
		    അവസ്ഥ 'b': last_cmd = CMD_BACKTRACE;
			      എങ്കിൽ (p[1] == 't')
				  tail = "t";
			      അല്ലെങ്കിൽ
				  tail = "acktrace";
			      നിർത്തുക;
		    അവസ്ഥ 'w': last_cmd = CMD_BACKTRACE;
			      tail = "here";
			      നിർത്തുക;
		    അവസ്ഥ 'u': last_cmd = CMD_UP;
			      tail = "p";
			      നിർത്തുക;
		    അവസ്ഥ 'd': last_cmd = CMD_DOWN;
			      tail = "own";
			      നിർത്തുക;
		    സ്ഥിരസ്ഥിതി: last_cmd = 0;
		}
		എങ്കിൽ (last_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    ആകുമ്പോൾ (*p != NUL && *p == *tail)
		    {
			++p;
			++tail;
		    }
		    എങ്കിൽ (ASCII_ISALPHA(*p) && last_cmd != CMD_FRAME)
			last_cmd = 0;
		}
	    }

	    എങ്കിൽ (last_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		തിരഞ്ഞെടുപ്പ് (last_cmd)
		{
		    അവസ്ഥ CMD_CONT:
			debug_break_level = -1;
			നിർത്തുക;
		    അവസ്ഥ CMD_NEXT:
			debug_break_level = ex_nesting_level;
			നിർത്തുക;
		    അവസ്ഥ CMD_STEP:
			debug_break_level = 9999;
			നിർത്തുക;
		    അവസ്ഥ CMD_FINISH:
			debug_break_level = ex_nesting_level - 1;
			നിർത്തുക;
		    അവസ്ഥ CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			നിർത്തുക;
		    അവസ്ഥ CMD_INTERRUPT:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			last_cmd = CMD_STEP;
			നിർത്തുക;
		    അവസ്ഥ CMD_BACKTRACE:
			do_showbacktrace(cmd);
			തുടരുക;
		    അവസ്ഥ CMD_FRAME:
			എങ്കിൽ (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			അല്ലെങ്കിൽ
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			തുടരുക;
		    അവസ്ഥ CMD_UP:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			തുടരുക;
		    അവസ്ഥ CMD_DOWN:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			തുടരുക;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		നിർത്തുക;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (ശൂന്യം)do_cmdline(cmdline, getexline, ശൂന്യ,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	വരികൾ_ഇടത് = Rows - 1;
    }
    vim_free(cmdline);

    എങ്കിൽ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    സന്ദേശം_scroll = save_സന്ദേശം_scroll;
    restore_timeout_for_debugging();
    വരികൾ_ഇടത് = Rows - 1;
    State = save_State;
    debug_mode = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    സന്ദേശം_silent = save_സന്ദേശം_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_സന്ദേശം = TRUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_maxbacktrace_level(char_u *sname)
{
    അക്ഷരം	*p, *q;
    പൂർണ്ണസംഖ്യ		maxbacktrace = 0;

    എങ്കിൽ (sname == ശൂന്യ)
	തിരികെ 0;

    p = (അക്ഷരം *)sname;
    ആകുമ്പോൾ ((q = strstr(p, "..")) != ശൂന്യ)
    {
	p = q + 2;
	maxbacktrace++;
    }
    തിരികെ maxbacktrace;
}

    സ്ഥിതം ശൂന്യം
do_setdebugtracelevel(char_u *arg)
{
    പൂർണ്ണസംഖ്യ level;

    level = atoi((അക്ഷരം *)arg);
    എങ്കിൽ (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    അല്ലെങ്കിൽ
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    സ്ഥിതം ശൂന്യം
do_checkbacktracelevel(ശൂന്യം)
{
    എങ്കിൽ (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	സന്ദേശം(_("frame is zero"));
    }
    അല്ലെങ്കിൽ
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	പൂർണ്ണസംഖ്യ	max = get_maxbacktrace_level(sname);

	എങ്കിൽ (debug_backtrace_level > max)
	{
	    debug_backtrace_level = max;
	    smsg(_("frame at highest level: %d"), max);
	}
	vim_free(sname);
    }
}

    സ്ഥിതം ശൂന്യം
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    അക്ഷരം    *cur;
    അക്ഷരം    *next;
    പൂർണ്ണസംഖ്യ	    i = 0;
    പൂർണ്ണസംഖ്യ	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    എങ്കിൽ (sname != ശൂന്യ)
    {
	cur = (അക്ഷരം *)sname;
	ആകുമ്പോൾ (!got_int)
	{
	    next = strstr(cur, "..");
	    എങ്കിൽ (next != ശൂന്യ)
		*next = NUL;
	    എങ്കിൽ (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    അല്ലെങ്കിൽ
		smsg("  %d %s", max - i, cur);
	    ++i;
	    എങ്കിൽ (next == ശൂന്യ)
		നിർത്തുക;
	    *next = '.';
	    cur = next + 2;
	}
	vim_free(sname);
    }

    എങ്കിൽ (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (long)SOURCING_LNUM, cmd);
    അല്ലെങ്കിൽ
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    ശൂന്യം
ex_debug(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

സ്ഥിതം char_u	*debug_breakpoint_name = ശൂന്യ;
സ്ഥിതം linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ	debug_skipped;
സ്ഥിതം char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    ശൂന്യം
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    എങ്കിൽ (debug_breakpoint_name != ശൂന്യ)
    {
	എങ്കിൽ (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    എങ്കിൽ (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    അല്ലെങ്കിൽ
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (long)debug_breakpoint_lnum);
	    debug_breakpoint_name = ശൂന്യ;
	    do_debug(eap->cmd);
	}
	അല്ലെങ്കിൽ
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = ശൂന്യ;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ex_nesting_level <= debug_break_level)
    {
	എങ്കിൽ (!eap->skip)
	    do_debug(eap->cmd);
	അല്ലെങ്കിൽ
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = ശൂന്യ;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    പൂർണ്ണസംഖ്യ
dbg_check_skipped(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		prev_got_int;

    എങ്കിൽ (!debug_skipped)
	തിരികെ FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    prev_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (ശൂന്യം)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= prev_got_int;
    തിരികെ TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
ഘടന debuggy
{
    പൂർണ്ണസംഖ്യ		dbg_nr;		// breakpoint number
    പൂർണ്ണസംഖ്യ		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    പൂർണ്ണസംഖ്യ		dbg_forceit;	// ! used
#ifdef FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#endif
    പൂർണ്ണസംഖ്യ		dbg_level;      // stored nested level for expr
};

സ്ഥിതം garray_T dbg_breakp = {0, 0, വലുപ്പം(ഘടന debuggy), 4, ശൂന്യ};
#define BREAKP(idx)		(((ഘടന debuggy *)dbg_breakp.ga_data)[idx])
#define DEBUGGY(gap, idx)	(((ഘടന debuggy *)gap->ga_data)[idx])
സ്ഥിതം പൂർണ്ണസംഖ്യ last_breakp = 0;	// nr of last defined breakpoint
സ്ഥിതം പൂർണ്ണസംഖ്യ has_വ്യഞ്ജകം_breakpoint = FALSE;

#ifdef FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
സ്ഥിതം garray_T prof_ga = {0, 0, വലുപ്പം(ഘടന debuggy), 4, ശൂന്യ};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
സ്ഥിതം hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
സ്ഥിതം പൂർണ്ണസംഖ്യ prof_cache_initialized;
തരനിർവചനം ഘടന profentry_S
{
    അക്ഷരം    pen_flags;	// cache data booleans: profiling, forceit
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
# define HI2PE(hi)	HIKEY2PE((hi)->hi_താക്കോൽ)

സ്ഥിതം ശൂന്യം prof_clear_cache(ശൂന്യം);
# define PROF_CLEAR_CACHE(gap) ചെയ്യുക {എങ്കിൽ ((gap) == &prof_ga) prof_clear_cache();} ആകുമ്പോൾ (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#അല്ലെങ്കിൽ
# define PROF_CLEAR_CACHE(gap) ചെയ്യുക {} ആകുമ്പോൾ (0)
#endif
#define DBG_FUNC	1
#define DBG_FILE	2
#define DBG_വ്യഞ്ജകം	3

സ്ഥിതം linenr_T debuggy_find(പൂർണ്ണസംഖ്യ file,char_u *fname, linenr_T after, garray_T *gap, പൂർണ്ണസംഖ്യ *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    സ്ഥിതം typval_T *
eval_വ്യഞ്ജകം_no_emsg(ഘടന debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_വ്യഞ്ജകം(bp->dbg_name, ശൂന്യ);
    --emsg_off;

    തിരികെ tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    ഘടന debuggy *bp;
    പൂർണ്ണസംഖ്യ		here = FALSE;

    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    എങ്കിൽ (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    അല്ലെങ്കിൽ എങ്കിൽ (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "here", 4) == 0)
    {
	എങ്കിൽ (curbuf->b_ffname == ശൂന്യ)
	{
	    emsg(_(e_no_file_name));
	    തിരികെ FAIL;
	}
	bp->dbg_type = DBG_FILE;
	here = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_വ്യഞ്ജകം;
    അല്ലെങ്കിൽ
    {
	semsg(_(e_invalid_argument_str), p);
	തിരികെ FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    എങ്കിൽ (here)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    അല്ലെങ്കിൽ എങ്കിൽ (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    അല്ലെങ്കിൽ
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    എങ്കിൽ ((!here && *p == NUL)
	    || (here && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && strstr((അക്ഷരം *)p, "()") != ശൂന്യ))
    {
	semsg(_(e_invalid_argument_str), arg);
	തിരികെ FAIL;
    }

    എങ്കിൽ (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    അല്ലെങ്കിൽ എങ്കിൽ (here)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    അല്ലെങ്കിൽ എങ്കിൽ (bp->dbg_type == DBG_വ്യഞ്ജകം)
    {
	bp->dbg_name = vim_strsave(p);
	എങ്കിൽ (bp->dbg_name != ശൂന്യ)
	    bp->dbg_val = eval_വ്യഞ്ജകം_no_emsg(bp);
    }
    അല്ലെങ്കിൽ
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	എങ്കിൽ (q == ശൂന്യ)
	    തിരികെ FAIL;
	p = expand_env_save(q);
	vim_free(q);
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ FAIL;
	എങ്കിൽ (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	അല്ലെങ്കിൽ
	    bp->dbg_name = p;
    }

    എങ്കിൽ (bp->dbg_name == ശൂന്യ)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    ശൂന്യം
ex_breakadd(exarg_T *eap)
{
    ഘടന debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#ifdef FEAT_PROFILE
    എങ്കിൽ (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#endif

    എങ്കിൽ (dbg_parsearg(eap->arg, gap) != OK)
	തിരികെ;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    എങ്കിൽ (bp->dbg_type != DBG_വ്യഞ്ജകം)
    {
	pat = file_pat_to_reg_pat(bp->dbg_name, ശൂന്യ, ശൂന്യ, FALSE);
	എങ്കിൽ (pat != ശൂന്യ)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	എങ്കിൽ (pat == ശൂന്യ || bp->dbg_prog == ശൂന്യ)
	    vim_free(bp->dbg_name);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#ifdef FEAT_PROFILE
	    എങ്കിൽ (eap->cmdidx != CMD_profile)
#endif
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++last_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    അല്ലെങ്കിൽ
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++last_breakp;
	++debug_tick;
	എങ്കിൽ (gap == &dbg_breakp)
	    has_വ്യഞ്ജകം_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    ശൂന്യം
ex_debuggreedy(exarg_T *eap)
{
    എങ്കിൽ (eap->addr_എണ്ണം == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    അല്ലെങ്കിൽ
	debug_greedy = FALSE;
}

    സ്ഥിതം ശൂന്യം
update_has_വ്യഞ്ജകം_breakpoint(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;

    has_വ്യഞ്ജകം_breakpoint = FALSE;
    വേണ്ടി (i = 0; i < dbg_breakp.ga_len; ++i)
	എങ്കിൽ (BREAKP(i).dbg_type == DBG_വ്യഞ്ജകം)
	{
	    has_വ്യഞ്ജകം_breakpoint = TRUE;
	    നിർത്തുക;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    പൂർണ്ണസംഖ്യ
debug_has_വ്യഞ്ജകം_breakpoint(ശൂന്യം)
{
    തിരികെ has_വ്യഞ്ജകം_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    ശൂന്യം
ex_breakdel(exarg_T *eap)
{
    ഘടന debuggy *bp, *bpi;
    പൂർണ്ണസംഖ്യ		nr;
    പൂർണ്ണസംഖ്യ		todel = -1;
    പൂർണ്ണസംഖ്യ		del_all = FALSE;
    പൂർണ്ണസംഖ്യ		i;
    linenr_T	best_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    എങ്കിൽ (eap->cmdidx == CMD_profdel)
    {
#ifdef FEAT_PROFILE
	gap = &prof_ga;
#അല്ലെങ്കിൽ
	ex_ni(eap);
	തിരികെ;
#endif
    }

    എങ്കിൽ (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((അക്ഷരം *)eap->arg);
	വേണ്ടി (i = 0; i < gap->ga_len; ++i)
	    എങ്കിൽ (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		നിർത്തുക;
	    }
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	എങ്കിൽ (dbg_parsearg(eap->arg, gap) == FAIL)
	    തിരികെ;
	bp = &DEBUGGY(gap, gap->ga_len);
	വേണ്ടി (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    എങ്കിൽ (bp->dbg_type == bpi->dbg_type
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

    എങ്കിൽ (todel < 0)
    {
	semsg(_(e_breakpoint_not_found_str), eap->arg);
	തിരികെ;
    }

    ആകുമ്പോൾ (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#ifdef FEAT_EVAL
	എങ്കിൽ (DEBUGGY(gap, todel).dbg_type == DBG_വ്യഞ്ജകം
		&& DEBUGGY(gap, todel).dbg_val != ശൂന്യ)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#endif
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	എങ്കിൽ (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * വലുപ്പം(ഘടന debuggy));
#ifdef FEAT_PROFILE
	എങ്കിൽ (eap->cmdidx == CMD_breakdel)
#endif
	    ++debug_tick;
	എങ്കിൽ (!del_all)
	    നിർത്തുക;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    എങ്കിൽ (gap->ga_len == 0)
	ga_clear(gap);
    എങ്കിൽ (gap == &dbg_breakp)
	update_has_വ്യഞ്ജകം_breakpoint();
}

/*
 * ":breaklist".
 */
    ശൂന്യം
ex_breaklist(exarg_T *eap UNUSED)
{
    ഘടന debuggy *bp;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (dbg_breakp.ga_len == 0)
    {
	സന്ദേശം(_("No breakpoints defined"));
	തിരികെ;
    }

    വേണ്ടി (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	എങ്കിൽ (bp->dbg_type == DBG_FILE)
	    home_replace(ശൂന്യ, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	എങ്കിൽ (bp->dbg_type != DBG_വ്യഞ്ജകം)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (long)bp->dbg_lnum);
	അല്ലെങ്കിൽ
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
    പൂർണ്ണസംഖ്യ		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    തിരികെ debuggy_find(file, fname, after, &dbg_breakp, ശൂന്യ);
}

#എങ്കിൽ defined(FEAT_PROFILE)
# എങ്കിൽ defined(PROF_CACHE_LOG)
സ്ഥിതം പൂർണ്ണസംഖ്യ എണ്ണം_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    പൂർണ്ണസംഖ്യ
has_profiling(
    പൂർണ്ണസംഖ്യ		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    പൂർണ്ണസംഖ്യ		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    എങ്കിൽ (prof_ga.ga_len == 0 || !prof_cache_initialized)
	തിരികെ debuggy_find(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    എങ്കിൽ (hashp != ശൂന്യ)
    {
	hash = *hashp;
	എങ്കിൽ (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    അല്ലെങ്കിൽ
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    എങ്കിൽ (HASHITEM_EMPTY(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	എങ്കിൽ (pe == ശൂന്യ)
	    തിരികെ FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	പൂർണ്ണസംഖ്യ f;
	പൂർണ്ണസംഖ്യ lnum = debuggy_find(file, fname, (linenr_T)0, &prof_ga, &f);
	എങ്കിൽ (lnum)
	{
	    PEN_SET_PROFILING(pe);
	    എങ്കിൽ (f)
		PEN_SET_FORCEIT(pe);
	}
	hash_add_item(ht, hi, pe->pen_name, hash);
# എങ്കിൽ defined(PROF_CACHE_LOG)
	ch_log(ശൂന്യ, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    അല്ലെങ്കിൽ
	pe = HI2PE(hi);
    എങ്കിൽ (fp)
	*fp = PEN_IS_FORCEIT(pe);
# എങ്കിൽ defined(PROF_CACHE_LOG)
    എണ്ണം_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    തിരികെ PEN_IS_PROFILING(pe);
}

    സ്ഥിതം ശൂന്യം
prof_clear_cache(ശൂന്യം)
{
    എങ്കിൽ (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    വേണ്ടി (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	എങ്കിൽ (ht->ht_used > 0)
	{
# എങ്കിൽ defined(PROF_CACHE_LOG)
	    പൂർണ്ണസംഖ്യ idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(ശൂന്യ, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_used, എണ്ണം_lookups[idx]);
	    എണ്ണം_lookups[idx] = 0;
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
    സ്ഥിതം linenr_T
debuggy_find(
    പൂർണ്ണസംഖ്യ		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    പൂർണ്ണസംഖ്യ		*fp)	    // if not NULL: return forceit
{
    ഘടന debuggy *bp;
    പൂർണ്ണസംഖ്യ		i;
    linenr_T	lnum = 0;
    char_u	*name = ശൂന്യ;
    char_u	*short_name = fname;
    പൂർണ്ണസംഖ്യ		prev_got_int;

    // Return quickly when there are no breakpoints.
    എങ്കിൽ (gap->ga_len == 0)
	തിരികെ (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    എങ്കിൽ (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	എങ്കിൽ (name != ശൂന്യ)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	എങ്കിൽ (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_വ്യഞ്ജകം && (
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
	    എങ്കിൽ ((name != ശൂന്യ
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		എങ്കിൽ (fp != ശൂന്യ)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#ifdef FEAT_PROFILE
	    എങ്കിൽ (lnum && gap == &prof_ga)
		നിർത്തുക;
#endif
	}
#ifdef FEAT_EVAL
	അല്ലെങ്കിൽ എങ്കിൽ (bp->dbg_type == DBG_വ്യഞ്ജകം)
	{
	    typval_T *tv;
	    പൂർണ്ണസംഖ്യ	      വരി = FALSE;

	    tv = eval_വ്യഞ്ജകം_no_emsg(bp);
	    എങ്കിൽ (tv != ശൂന്യ)
	    {
		എങ്കിൽ (bp->dbg_val == ശൂന്യ)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(ശൂന്യ, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    വരി = TRUE;
		}
		അല്ലെങ്കിൽ
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? വ്യഞ്ജകം_EQUAL : വ്യഞ്ജകം_IS;

		    എങ്കിൽ (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_സംഖ്യ == FALSE)
		    {
			typval_T *v;

			വരി = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_വ്യഞ്ജകം_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (bp->dbg_val != ശൂന്യ)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(ശൂന്യ, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = ശൂന്യ;
		വരി = TRUE;
	    }

	    എങ്കിൽ (വരി)
	    {
		lnum = after > 0 ? after : 1;
		നിർത്തുക;
	    }
	}
#endif
    }
    എങ്കിൽ (name != fname)
	vim_free(name);

    തിരികെ lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    ശൂന്യം
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#endif
