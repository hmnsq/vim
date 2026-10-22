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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
ಸ್ಥಿರ ಶೂನ್ಯ do_setdebugtracelevel(char_u *arg);
ಸ್ಥಿರ ಶೂನ್ಯ do_checkbacktracelevel(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ do_showbacktrace(char_u *cmd);

ಸ್ಥಿರ char_u *debug_oldval = ರಿಕ್ತ;	// old and newval for debug expressions
ಸ್ಥಿರ char_u *debug_newval = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ     debug_ಅಭಿವ್ಯಕ್ತಿ   = 0;	// use debug_expr

    ಪೂರ್ಣಾಙ್ಕ
has_watchexpr(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ debug_ಅಭಿವ್ಯಕ್ತಿ;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    ಶೂನ್ಯ
do_debug(char_u *cmd)
{
    ಪೂರ್ಣಾಙ್ಕ		save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
    ಪೂರ್ಣಾಙ್ಕ		save_State = State;
    ಪೂರ್ಣಾಙ್ಕ		save_did_emsg = did_emsg;
    ಪೂರ್ಣಾಙ್ಕ		save_cmd_silent = cmd_silent;
    ಪೂರ್ಣಾಙ್ಕ		save_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent;
    ಪೂರ್ಣಾಙ್ಕ		save_emsg_silent = emsg_silent;
    ಪೂರ್ಣಾಙ್ಕ		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    ಪೂರ್ಣಾಙ್ಕ		typeahead_saved = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		save_ignore_script = 0;
    ಪೂರ್ಣಾಙ್ಕ		save_ex_normal_busy;
    ಪೂರ್ಣಾಙ್ಕ		n;
    char_u	*cmdline = ರಿಕ್ತ;
    char_u	*p;
    char_u	*sname;
    ವರ್ಣ	*tail = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	last_cmd = 0;
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
    ಯದಿ (!gui.in_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	ಪ್ರತ್ಯಾಗಮ;
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
    ಸಂದೇಶ_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = MODE_NORMAL;
    debug_mode = TRUE;

    ಯದಿ (!debug_did_ಸಂದೇಶ)
	ಸಂದೇಶ(_("Entering Debug mode.  Type \"cont\" to continue."));
    ಯದಿ (debug_oldval != ರಿಕ್ತ)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    ಯದಿ (debug_newval != ರಿಕ್ತ)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    ಯದಿ (sname != ರಿಕ್ತ)
	ಸಂದೇಶ((ವರ್ಣ *)sname);
    vim_free(sname);
    ಯದಿ (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    ಅನ್ಯಥಾ
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    ಪ್ರತಿ (;;)
    {
	ಸಂದೇಶ_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_normal_busy = ex_normal_busy;
	ex_normal_busy = 0;
	ಯದಿ (!debug_greedy)
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
	cmdline = getcmdline_prompt('>', ರಿಕ್ತ, 0, EXPAND_NOTHING, ರಿಕ್ತ);

	debug_break_level = n;
	ಯದಿ (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_normal_busy = save_ex_normal_busy;

	cmdline_row = ಸಂದೇಶ_row;
	ಸಂದೇಶ_starthere();
	ಯದಿ (cmdline != ರಿಕ್ತ)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    ಯದಿ (*p != NUL)
	    {
		ವಿಕಲ್ಪ (*p)
		{
		    ಪಕ್ಷ 'c': last_cmd = CMD_CONT;
			      tail = "ont";
			      ಭಙ್ಗ;
		    ಪಕ್ಷ 'n': last_cmd = CMD_NEXT;
			      tail = "ext";
			      ಭಙ್ಗ;
		    ಪಕ್ಷ 's': last_cmd = CMD_STEP;
			      tail = "tep";
			      ಭಙ್ಗ;
		    ಪಕ್ಷ 'f':
			      last_cmd = 0;
			      ಯದಿ (p[1] == 'r')
			      {
				  last_cmd = CMD_FRAME;
				  tail = "rame";
			      }
			      ಅನ್ಯಥಾ
			      {
				  last_cmd = CMD_FINISH;
				  tail = "inish";
			      }
			      ಭಙ್ಗ;
		    ಪಕ್ಷ 'q': last_cmd = CMD_QUIT;
			      tail = "uit";
			      ಭಙ್ಗ;
		    ಪಕ್ಷ 'i': last_cmd = CMD_INTERRUPT;
			      tail = "nterrupt";
			      ಭಙ್ಗ;
		    ಪಕ್ಷ 'b': last_cmd = CMD_BACKTRACE;
			      ಯದಿ (p[1] == 't')
				  tail = "t";
			      ಅನ್ಯಥಾ
				  tail = "acktrace";
			      ಭಙ್ಗ;
		    ಪಕ್ಷ 'w': last_cmd = CMD_BACKTRACE;
			      tail = "here";
			      ಭಙ್ಗ;
		    ಪಕ್ಷ 'u': last_cmd = CMD_UP;
			      tail = "p";
			      ಭಙ್ಗ;
		    ಪಕ್ಷ 'd': last_cmd = CMD_DOWN;
			      tail = "own";
			      ಭಙ್ಗ;
		    ಸ್ವಭಾವ: last_cmd = 0;
		}
		ಯದಿ (last_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    ಯಾವತ್ (*p != NUL && *p == *tail)
		    {
			++p;
			++tail;
		    }
		    ಯದಿ (ASCII_ISALPHA(*p) && last_cmd != CMD_FRAME)
			last_cmd = 0;
		}
	    }

	    ಯದಿ (last_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		ವಿಕಲ್ಪ (last_cmd)
		{
		    ಪಕ್ಷ CMD_CONT:
			debug_break_level = -1;
			ಭಙ್ಗ;
		    ಪಕ್ಷ CMD_NEXT:
			debug_break_level = ex_nesting_level;
			ಭಙ್ಗ;
		    ಪಕ್ಷ CMD_STEP:
			debug_break_level = 9999;
			ಭಙ್ಗ;
		    ಪಕ್ಷ CMD_FINISH:
			debug_break_level = ex_nesting_level - 1;
			ಭಙ್ಗ;
		    ಪಕ್ಷ CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			ಭಙ್ಗ;
		    ಪಕ್ಷ CMD_INTERRUPT:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			last_cmd = CMD_STEP;
			ಭಙ್ಗ;
		    ಪಕ್ಷ CMD_BACKTRACE:
			do_showbacktrace(cmd);
			ಅನುವರ್ತ;
		    ಪಕ್ಷ CMD_FRAME:
			ಯದಿ (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			ಅನ್ಯಥಾ
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			ಅನುವರ್ತ;
		    ಪಕ್ಷ CMD_UP:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			ಅನುವರ್ತ;
		    ಪಕ್ಷ CMD_DOWN:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			ಅನುವರ್ತ;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		ಭಙ್ಗ;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (ಶೂನ್ಯ)do_cmdline(cmdline, getexline, ರಿಕ್ತ,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	ಸಾಲುಗಳು_ಎಡ = Rows - 1;
    }
    vim_free(cmdline);

    ಯದಿ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    ಸಂದೇಶ_scroll = save_ಸಂದೇಶ_scroll;
    restore_timeout_for_debugging();
    ಸಾಲುಗಳು_ಎಡ = Rows - 1;
    State = save_State;
    debug_mode = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    ಸಂದೇಶ_silent = save_ಸಂದೇಶ_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_ಸಂದೇಶ = TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_maxbacktrace_level(char_u *sname)
{
    ವರ್ಣ	*p, *q;
    ಪೂರ್ಣಾಙ್ಕ		maxbacktrace = 0;

    ಯದಿ (sname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    p = (ವರ್ಣ *)sname;
    ಯಾವತ್ ((q = strstr(p, "..")) != ರಿಕ್ತ)
    {
	p = q + 2;
	maxbacktrace++;
    }
    ಪ್ರತ್ಯಾಗಮ maxbacktrace;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_setdebugtracelevel(char_u *arg)
{
    ಪೂರ್ಣಾಙ್ಕ level;

    level = atoi((ವರ್ಣ *)arg);
    ಯದಿ (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    ಅನ್ಯಥಾ
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_checkbacktracelevel(ಶೂನ್ಯ)
{
    ಯದಿ (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	ಸಂದೇಶ(_("frame is zero"));
    }
    ಅನ್ಯಥಾ
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	ಪೂರ್ಣಾಙ್ಕ	max = get_maxbacktrace_level(sname);

	ಯದಿ (debug_backtrace_level > max)
	{
	    debug_backtrace_level = max;
	    smsg(_("frame at highest level: %d"), max);
	}
	vim_free(sname);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    ವರ್ಣ    *cur;
    ವರ್ಣ    *next;
    ಪೂರ್ಣಾಙ್ಕ	    i = 0;
    ಪೂರ್ಣಾಙ್ಕ	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    ಯದಿ (sname != ರಿಕ್ತ)
    {
	cur = (ವರ್ಣ *)sname;
	ಯಾವತ್ (!got_int)
	{
	    next = strstr(cur, "..");
	    ಯದಿ (next != ರಿಕ್ತ)
		*next = NUL;
	    ಯದಿ (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    ಅನ್ಯಥಾ
		smsg("  %d %s", max - i, cur);
	    ++i;
	    ಯದಿ (next == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    *next = '.';
	    cur = next + 2;
	}
	vim_free(sname);
    }

    ಯದಿ (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (long)SOURCING_LNUM, cmd);
    ಅನ್ಯಥಾ
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    ಶೂನ್ಯ
ex_debug(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

ಸ್ಥಿರ char_u	*debug_breakpoint_name = ರಿಕ್ತ;
ಸ್ಥಿರ linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	debug_skipped;
ಸ್ಥಿರ char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    ಶೂನ್ಯ
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    ಯದಿ (debug_breakpoint_name != ರಿಕ್ತ)
    {
	ಯದಿ (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    ಯದಿ (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    ಅನ್ಯಥಾ
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (long)debug_breakpoint_lnum);
	    debug_breakpoint_name = ರಿಕ್ತ;
	    do_debug(eap->cmd);
	}
	ಅನ್ಯಥಾ
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (ex_nesting_level <= debug_break_level)
    {
	ಯದಿ (!eap->skip)
	    do_debug(eap->cmd);
	ಅನ್ಯಥಾ
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = ರಿಕ್ತ;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    ಪೂರ್ಣಾಙ್ಕ
dbg_check_skipped(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		prev_got_int;

    ಯದಿ (!debug_skipped)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    prev_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (ಶೂನ್ಯ)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= prev_got_int;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
ಸಂರಚನಾ debuggy
{
    ಪೂರ್ಣಾಙ್ಕ		dbg_nr;		// breakpoint number
    ಪೂರ್ಣಾಙ್ಕ		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    ಪೂರ್ಣಾಙ್ಕ		dbg_forceit;	// ! used
#ifdef FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#endif
    ಪೂರ್ಣಾಙ್ಕ		dbg_level;      // stored nested level for expr
};

ಸ್ಥಿರ garray_T dbg_breakp = {0, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ debuggy), 4, ರಿಕ್ತ};
#define BREAKP(idx)		(((ಸಂರಚನಾ debuggy *)dbg_breakp.ga_data)[idx])
#define DEBUGGY(gap, idx)	(((ಸಂರಚನಾ debuggy *)gap->ga_data)[idx])
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ last_breakp = 0;	// nr of last defined breakpoint
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint = FALSE;

#ifdef FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
ಸ್ಥಿರ garray_T prof_ga = {0, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ debuggy), 4, ರಿಕ್ತ};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
ಸ್ಥಿರ hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prof_cache_initialized;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ profentry_S
{
    ವರ್ಣ    pen_flags;	// cache data booleans: profiling, forceit
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
# define HI2PE(hi)	HIKEY2PE((hi)->hi_ಕೀಲಿ)

ಸ್ಥಿರ ಶೂನ್ಯ prof_clear_cache(ಶೂನ್ಯ);
# define PROF_CLEAR_CACHE(gap) ಕುರು {ಯದಿ ((gap) == &prof_ga) prof_clear_cache();} ಯಾವತ್ (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#ಅನ್ಯಥಾ
# define PROF_CLEAR_CACHE(gap) ಕುರು {} ಯಾವತ್ (0)
#endif
#define DBG_FUNC	1
#define DBG_FILE	2
#define DBG_ಅಭಿವ್ಯಕ್ತಿ	3

ಸ್ಥಿರ linenr_T debuggy_find(ಪೂರ್ಣಾಙ್ಕ file,char_u *fname, linenr_T after, garray_T *gap, ಪೂರ್ಣಾಙ್ಕ *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    ಸ್ಥಿರ typval_T *
eval_ಅಭಿವ್ಯಕ್ತಿ_no_emsg(ಸಂರಚನಾ debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_ಅಭಿವ್ಯಕ್ತಿ(bp->dbg_name, ರಿಕ್ತ);
    --emsg_off;

    ಪ್ರತ್ಯಾಗಮ tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    ಸಂರಚನಾ debuggy *bp;
    ಪೂರ್ಣಾಙ್ಕ		here = FALSE;

    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    ಯದಿ (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    ಅನ್ಯಥಾ ಯದಿ (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "here", 4) == 0)
    {
	ಯದಿ (curbuf->b_ffname == ರಿಕ್ತ)
	{
	    emsg(_(e_no_file_name));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	bp->dbg_type = DBG_FILE;
	here = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_ಅಭಿವ್ಯಕ್ತಿ;
    ಅನ್ಯಥಾ
    {
	semsg(_(e_invalid_argument_str), p);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    ಯದಿ (here)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    ಅನ್ಯಥಾ ಯದಿ (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    ಅನ್ಯಥಾ
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    ಯದಿ ((!here && *p == NUL)
	    || (here && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && strstr((ವರ್ಣ *)p, "()") != ರಿಕ್ತ))
    {
	semsg(_(e_invalid_argument_str), arg);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    ಅನ್ಯಥಾ ಯದಿ (here)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    ಅನ್ಯಥಾ ಯದಿ (bp->dbg_type == DBG_ಅಭಿವ್ಯಕ್ತಿ)
    {
	bp->dbg_name = vim_strsave(p);
	ಯದಿ (bp->dbg_name != ರಿಕ್ತ)
	    bp->dbg_val = eval_ಅಭಿವ್ಯಕ್ತಿ_no_emsg(bp);
    }
    ಅನ್ಯಥಾ
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	ಯದಿ (q == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	p = expand_env_save(q);
	vim_free(q);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	ಅನ್ಯಥಾ
	    bp->dbg_name = p;
    }

    ಯದಿ (bp->dbg_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    ಶೂನ್ಯ
ex_breakadd(exarg_T *eap)
{
    ಸಂರಚನಾ debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#ifdef FEAT_PROFILE
    ಯದಿ (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#endif

    ಯದಿ (dbg_parsearg(eap->arg, gap) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    ಯದಿ (bp->dbg_type != DBG_ಅಭಿವ್ಯಕ್ತಿ)
    {
	pat = file_pat_to_reg_pat(bp->dbg_name, ರಿಕ್ತ, ರಿಕ್ತ, FALSE);
	ಯದಿ (pat != ರಿಕ್ತ)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	ಯದಿ (pat == ರಿಕ್ತ || bp->dbg_prog == ರಿಕ್ತ)
	    vim_free(bp->dbg_name);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#ifdef FEAT_PROFILE
	    ಯದಿ (eap->cmdidx != CMD_profile)
#endif
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++last_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    ಅನ್ಯಥಾ
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++last_breakp;
	++debug_tick;
	ಯದಿ (gap == &dbg_breakp)
	    has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    ಶೂನ್ಯ
ex_debuggreedy(exarg_T *eap)
{
    ಯದಿ (eap->addr_ಎಣಿಕೆ == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    ಅನ್ಯಥಾ
	debug_greedy = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
update_has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint = FALSE;
    ಪ್ರತಿ (i = 0; i < dbg_breakp.ga_len; ++i)
	ಯದಿ (BREAKP(i).dbg_type == DBG_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint = TRUE;
	    ಭಙ್ಗ;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    ಪೂರ್ಣಾಙ್ಕ
debug_has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    ಶೂನ್ಯ
ex_breakdel(exarg_T *eap)
{
    ಸಂರಚನಾ debuggy *bp, *bpi;
    ಪೂರ್ಣಾಙ್ಕ		nr;
    ಪೂರ್ಣಾಙ್ಕ		todel = -1;
    ಪೂರ್ಣಾಙ್ಕ		del_all = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		i;
    linenr_T	best_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    ಯದಿ (eap->cmdidx == CMD_profdel)
    {
#ifdef FEAT_PROFILE
	gap = &prof_ga;
#ಅನ್ಯಥಾ
	ex_ni(eap);
	ಪ್ರತ್ಯಾಗಮ;
#endif
    }

    ಯದಿ (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((ವರ್ಣ *)eap->arg);
	ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	    ಯದಿ (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		ಭಙ್ಗ;
	    }
    }
    ಅನ್ಯಥಾ ಯದಿ (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	ಯದಿ (dbg_parsearg(eap->arg, gap) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	bp = &DEBUGGY(gap, gap->ga_len);
	ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    ಯದಿ (bp->dbg_type == bpi->dbg_type
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

    ಯದಿ (todel < 0)
    {
	semsg(_(e_breakpoint_not_found_str), eap->arg);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯಾವತ್ (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#ifdef FEAT_EVAL
	ಯದಿ (DEBUGGY(gap, todel).dbg_type == DBG_ಅಭಿವ್ಯಕ್ತಿ
		&& DEBUGGY(gap, todel).dbg_val != ರಿಕ್ತ)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#endif
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	ಯದಿ (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * ಪರಿಮಾಣ(ಸಂರಚನಾ debuggy));
#ifdef FEAT_PROFILE
	ಯದಿ (eap->cmdidx == CMD_breakdel)
#endif
	    ++debug_tick;
	ಯದಿ (!del_all)
	    ಭಙ್ಗ;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    ಯದಿ (gap->ga_len == 0)
	ga_clear(gap);
    ಯದಿ (gap == &dbg_breakp)
	update_has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint();
}

/*
 * ":breaklist".
 */
    ಶೂನ್ಯ
ex_breaklist(exarg_T *eap UNUSED)
{
    ಸಂರಚನಾ debuggy *bp;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (dbg_breakp.ga_len == 0)
    {
	ಸಂದೇಶ(_("No breakpoints defined"));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	ಯದಿ (bp->dbg_type == DBG_FILE)
	    home_replace(ರಿಕ್ತ, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	ಯದಿ (bp->dbg_type != DBG_ಅಭಿವ್ಯಕ್ತಿ)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (long)bp->dbg_lnum);
	ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    ಪ್ರತ್ಯಾಗಮ debuggy_find(file, fname, after, &dbg_breakp, ರಿಕ್ತ);
}

#ಯದಿ defined(FEAT_PROFILE)
# ಯದಿ defined(PROF_CACHE_LOG)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_profiling(
    ಪೂರ್ಣಾಙ್ಕ		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    ಪೂರ್ಣಾಙ್ಕ		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    ಯದಿ (prof_ga.ga_len == 0 || !prof_cache_initialized)
	ಪ್ರತ್ಯಾಗಮ debuggy_find(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    ಯದಿ (hashp != ರಿಕ್ತ)
    {
	hash = *hashp;
	ಯದಿ (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    ಅನ್ಯಥಾ
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    ಯದಿ (HASHITEM_EMPTY(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	ಯದಿ (pe == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	ಪೂರ್ಣಾಙ್ಕ f;
	ಪೂರ್ಣಾಙ್ಕ lnum = debuggy_find(file, fname, (linenr_T)0, &prof_ga, &f);
	ಯದಿ (lnum)
	{
	    PEN_SET_PROFILING(pe);
	    ಯದಿ (f)
		PEN_SET_FORCEIT(pe);
	}
	hash_add_item(ht, hi, pe->pen_name, hash);
# ಯದಿ defined(PROF_CACHE_LOG)
	ch_log(ರಿಕ್ತ, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    ಅನ್ಯಥಾ
	pe = HI2PE(hi);
    ಯದಿ (fp)
	*fp = PEN_IS_FORCEIT(pe);
# ಯದಿ defined(PROF_CACHE_LOG)
    ಎಣಿಕೆ_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    ಪ್ರತ್ಯಾಗಮ PEN_IS_PROFILING(pe);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prof_clear_cache(ಶೂನ್ಯ)
{
    ಯದಿ (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    ಪ್ರತಿ (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	ಯದಿ (ht->ht_used > 0)
	{
# ಯದಿ defined(PROF_CACHE_LOG)
	    ಪೂರ್ಣಾಙ್ಕ idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(ರಿಕ್ತ, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_used, ಎಣಿಕೆ_lookups[idx]);
	    ಎಣಿಕೆ_lookups[idx] = 0;
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
    ಸ್ಥಿರ linenr_T
debuggy_find(
    ಪೂರ್ಣಾಙ್ಕ		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    ಪೂರ್ಣಾಙ್ಕ		*fp)	    // if not NULL: return forceit
{
    ಸಂರಚನಾ debuggy *bp;
    ಪೂರ್ಣಾಙ್ಕ		i;
    linenr_T	lnum = 0;
    char_u	*name = ರಿಕ್ತ;
    char_u	*short_name = fname;
    ಪೂರ್ಣಾಙ್ಕ		prev_got_int;

    // Return quickly when there are no breakpoints.
    ಯದಿ (gap->ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    ಯದಿ (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	ಯದಿ (name != ರಿಕ್ತ)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	ಯದಿ (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_ಅಭಿವ್ಯಕ್ತಿ && (
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
	    ಯದಿ ((name != ರಿಕ್ತ
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		ಯದಿ (fp != ರಿಕ್ತ)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#ifdef FEAT_PROFILE
	    ಯದಿ (lnum && gap == &prof_ga)
		ಭಙ್ಗ;
#endif
	}
#ifdef FEAT_EVAL
	ಅನ್ಯಥಾ ಯದಿ (bp->dbg_type == DBG_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    typval_T *tv;
	    ಪೂರ್ಣಾಙ್ಕ	      ಸಾಲು = FALSE;

	    tv = eval_ಅಭಿವ್ಯಕ್ತಿ_no_emsg(bp);
	    ಯದಿ (tv != ರಿಕ್ತ)
	    {
		ಯದಿ (bp->dbg_val == ರಿಕ್ತ)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(ರಿಕ್ತ, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    ಸಾಲು = TRUE;
		}
		ಅನ್ಯಥಾ
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? ಅಭಿವ್ಯಕ್ತಿ_EQUAL : ಅಭಿವ್ಯಕ್ತಿ_IS;

		    ಯದಿ (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_ಸಂಖ್ಯೆ == FALSE)
		    {
			typval_T *v;

			ಸಾಲು = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_ಅಭಿವ್ಯಕ್ತಿ_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (bp->dbg_val != ರಿಕ್ತ)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(ರಿಕ್ತ, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = ರಿಕ್ತ;
		ಸಾಲು = TRUE;
	    }

	    ಯದಿ (ಸಾಲು)
	    {
		lnum = after > 0 ? after : 1;
		ಭಙ್ಗ;
	    }
	}
#endif
    }
    ಯದಿ (name != fname)
	vim_free(name);

    ಪ್ರತ್ಯಾಗಮ lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    ಶೂನ್ಯ
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#endif
