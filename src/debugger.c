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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)
স্থির পূর্ণসংখ্যা debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
স্থির শূন্য do_setdebugtracelevel(char_u *arg);
স্থির শূন্য do_checkbacktracelevel(শূন্য);
স্থির শূন্য do_showbacktrace(char_u *cmd);

স্থির char_u *debug_oldval = ফাঁকা;	// old and newval for debug expressions
স্থির char_u *debug_newval = ফাঁকা;
স্থির পূর্ণসংখ্যা     debug_রাশিমালা   = 0;	// use debug_expr

    পূর্ণসংখ্যা
has_watchexpr(শূন্য)
{
    ফেরত debug_রাশিমালা;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    শূন্য
do_debug(char_u *cmd)
{
    পূর্ণসংখ্যা		save_বার্তা_scroll = বার্তা_scroll;
    পূর্ণসংখ্যা		save_State = State;
    পূর্ণসংখ্যা		save_did_emsg = did_emsg;
    পূর্ণসংখ্যা		save_cmd_silent = cmd_silent;
    পূর্ণসংখ্যা		save_বার্তা_silent = বার্তা_silent;
    পূর্ণসংখ্যা		save_emsg_silent = emsg_silent;
    পূর্ণসংখ্যা		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    পূর্ণসংখ্যা		typeahead_saved = FALSE;
    পূর্ণসংখ্যা		save_ignore_script = 0;
    পূর্ণসংখ্যা		save_ex_normal_busy;
    পূর্ণসংখ্যা		n;
    char_u	*cmdline = ফাঁকা;
    char_u	*p;
    char_u	*sname;
    অক্ষর	*লেজ = ফাঁকা;
    স্থির পূর্ণসংখ্যা	last_cmd = 0;
#define CMD_CONT	1
#define CMD_পরবর্তী	2
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
    যদি (!gui.in_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	ফেরত;
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
    বার্তা_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = MODE_NORMAL;
    debug_mode = TRUE;

    যদি (!debug_did_বার্তা)
	বার্তা(_("Entering Debug mode.  Type \"cont\" to continue."));
    যদি (debug_oldval != ফাঁকা)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    যদি (debug_newval != ফাঁকা)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    যদি (sname != ফাঁকা)
	বার্তা((অক্ষর *)sname);
    vim_free(sname);
    যদি (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    অন্যথা
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    জন্য (;;)
    {
	বার্তা_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_normal_busy = ex_normal_busy;
	ex_normal_busy = 0;
	যদি (!debug_greedy)
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
	cmdline = getcmdline_prompt('>', ফাঁকা, 0, EXPAND_NOTHING, ফাঁকা);

	debug_break_level = n;
	যদি (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_normal_busy = save_ex_normal_busy;

	cmdline_row = বার্তা_row;
	বার্তা_starthere();
	যদি (cmdline != ফাঁকা)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    যদি (*p != NUL)
	    {
		নির্বাচন (*p)
		{
		    ক্ষেত্র 'c': last_cmd = CMD_CONT;
			      লেজ = "ont";
			      বিরতি;
		    ক্ষেত্র 'n': last_cmd = CMD_পরবর্তী;
			      লেজ = "ext";
			      বিরতি;
		    ক্ষেত্র 's': last_cmd = CMD_STEP;
			      লেজ = "tep";
			      বিরতি;
		    ক্ষেত্র 'f':
			      last_cmd = 0;
			      যদি (p[1] == 'r')
			      {
				  last_cmd = CMD_FRAME;
				  লেজ = "rame";
			      }
			      অন্যথা
			      {
				  last_cmd = CMD_FINISH;
				  লেজ = "inish";
			      }
			      বিরতি;
		    ক্ষেত্র 'q': last_cmd = CMD_QUIT;
			      লেজ = "uit";
			      বিরতি;
		    ক্ষেত্র 'i': last_cmd = CMD_INTERRUPT;
			      লেজ = "nterrupt";
			      বিরতি;
		    ক্ষেত্র 'b': last_cmd = CMD_BACKTRACE;
			      যদি (p[1] == 't')
				  লেজ = "t";
			      অন্যথা
				  লেজ = "acktrace";
			      বিরতি;
		    ক্ষেত্র 'w': last_cmd = CMD_BACKTRACE;
			      লেজ = "here";
			      বিরতি;
		    ক্ষেত্র 'u': last_cmd = CMD_UP;
			      লেজ = "p";
			      বিরতি;
		    ক্ষেত্র 'd': last_cmd = CMD_DOWN;
			      লেজ = "own";
			      বিরতি;
		    পূর্বনির্ধারিত: last_cmd = 0;
		}
		যদি (last_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    যখন (*p != NUL && *p == *লেজ)
		    {
			++p;
			++লেজ;
		    }
		    যদি (ASCII_ISALPHA(*p) && last_cmd != CMD_FRAME)
			last_cmd = 0;
		}
	    }

	    যদি (last_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		নির্বাচন (last_cmd)
		{
		    ক্ষেত্র CMD_CONT:
			debug_break_level = -1;
			বিরতি;
		    ক্ষেত্র CMD_পরবর্তী:
			debug_break_level = ex_nesting_level;
			বিরতি;
		    ক্ষেত্র CMD_STEP:
			debug_break_level = 9999;
			বিরতি;
		    ক্ষেত্র CMD_FINISH:
			debug_break_level = ex_nesting_level - 1;
			বিরতি;
		    ক্ষেত্র CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			বিরতি;
		    ক্ষেত্র CMD_INTERRUPT:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			last_cmd = CMD_STEP;
			বিরতি;
		    ক্ষেত্র CMD_BACKTRACE:
			do_showbacktrace(cmd);
			চালু;
		    ক্ষেত্র CMD_FRAME:
			যদি (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			অন্যথা
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			চালু;
		    ক্ষেত্র CMD_UP:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			চালু;
		    ক্ষেত্র CMD_DOWN:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			চালু;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		বিরতি;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (শূন্য)do_cmdline(cmdline, getexline, ফাঁকা,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	সারিগুলি_বাম = Rows - 1;
    }
    vim_free(cmdline);

    যদি (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    বার্তা_scroll = save_বার্তা_scroll;
    restore_timeout_for_debugging();
    সারিগুলি_বাম = Rows - 1;
    State = save_State;
    debug_mode = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    বার্তা_silent = save_বার্তা_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_বার্তা = TRUE;
}

    স্থির পূর্ণসংখ্যা
get_maxbacktrace_level(char_u *sname)
{
    অক্ষর	*p, *q;
    পূর্ণসংখ্যা		maxbacktrace = 0;

    যদি (sname == ফাঁকা)
	ফেরত 0;

    p = (অক্ষর *)sname;
    যখন ((q = strstr(p, "..")) != ফাঁকা)
    {
	p = q + 2;
	maxbacktrace++;
    }
    ফেরত maxbacktrace;
}

    স্থির শূন্য
do_setdebugtracelevel(char_u *arg)
{
    পূর্ণসংখ্যা level;

    level = atoi((অক্ষর *)arg);
    যদি (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    অন্যথা
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    স্থির শূন্য
do_checkbacktracelevel(শূন্য)
{
    যদি (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	বার্তা(_("frame is zero"));
    }
    অন্যথা
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	পূর্ণসংখ্যা	max = get_maxbacktrace_level(sname);

	যদি (debug_backtrace_level > max)
	{
	    debug_backtrace_level = max;
	    smsg(_("frame at highest level: %d"), max);
	}
	vim_free(sname);
    }
}

    স্থির শূন্য
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    অক্ষর    *cur;
    অক্ষর    *পরবর্তী;
    পূর্ণসংখ্যা	    i = 0;
    পূর্ণসংখ্যা	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    যদি (sname != ফাঁকা)
    {
	cur = (অক্ষর *)sname;
	যখন (!got_int)
	{
	    পরবর্তী = strstr(cur, "..");
	    যদি (পরবর্তী != ফাঁকা)
		*পরবর্তী = NUL;
	    যদি (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    অন্যথা
		smsg("  %d %s", max - i, cur);
	    ++i;
	    যদি (পরবর্তী == ফাঁকা)
		বিরতি;
	    *পরবর্তী = '.';
	    cur = পরবর্তী + 2;
	}
	vim_free(sname);
    }

    যদি (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (long)SOURCING_LNUM, cmd);
    অন্যথা
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    শূন্য
ex_debug(exarg_T *eap)
{
    পূর্ণসংখ্যা		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

স্থির char_u	*debug_breakpoint_name = ফাঁকা;
স্থির linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
স্থির পূর্ণসংখ্যা	debug_skipped;
স্থির char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    শূন্য
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    যদি (debug_breakpoint_name != ফাঁকা)
    {
	যদি (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    যদি (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    অন্যথা
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (long)debug_breakpoint_lnum);
	    debug_breakpoint_name = ফাঁকা;
	    do_debug(eap->cmd);
	}
	অন্যথা
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = ফাঁকা;
	}
    }
    অন্যথা যদি (ex_nesting_level <= debug_break_level)
    {
	যদি (!eap->skip)
	    do_debug(eap->cmd);
	অন্যথা
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = ফাঁকা;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    পূর্ণসংখ্যা
dbg_check_skipped(exarg_T *eap)
{
    পূর্ণসংখ্যা		পূর্ব_got_int;

    যদি (!debug_skipped)
	ফেরত FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    পূর্ব_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (শূন্য)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= পূর্ব_got_int;
    ফেরত TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
কাঠামো debuggy
{
    পূর্ণসংখ্যা		dbg_nr;		// breakpoint number
    পূর্ণসংখ্যা		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    পূর্ণসংখ্যা		dbg_forceit;	// ! used
#ifdef FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#endif
    পূর্ণসংখ্যা		dbg_level;      // stored nested level for expr
};

স্থির garray_T dbg_breakp = {0, 0, আকার(কাঠামো debuggy), 4, ফাঁকা};
#define BREAKP(idx)		(((কাঠামো debuggy *)dbg_breakp.ga_তথ্য)[idx])
#define DEBUGGY(gap, idx)	(((কাঠামো debuggy *)gap->ga_তথ্য)[idx])
স্থির পূর্ণসংখ্যা last_breakp = 0;	// nr of last defined breakpoint
স্থির পূর্ণসংখ্যা has_রাশিমালা_breakpoint = FALSE;

#ifdef FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
স্থির garray_T prof_ga = {0, 0, আকার(কাঠামো debuggy), 4, ফাঁকা};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
স্থির hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
স্থির পূর্ণসংখ্যা prof_cache_initialized;
প্রকারলেখ কাঠামো profentry_S
{
    অক্ষর    pen_flags;	// cache data booleans: profiling, forceit
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
# define HI2PE(hi)	HIKEY2PE((hi)->hi_চাবি)

স্থির শূন্য prof_clear_cache(শূন্য);
# define PROF_CLEAR_CACHE(gap) করো {যদি ((gap) == &prof_ga) prof_clear_cache();} যখন (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#অন্যথা
# define PROF_CLEAR_CACHE(gap) করো {} যখন (0)
#endif
#define DBG_FUNC	1
#define DBG_FILE	2
#define DBG_রাশিমালা	3

স্থির linenr_T debuggy_find(পূর্ণসংখ্যা file,char_u *fname, linenr_T after, garray_T *gap, পূর্ণসংখ্যা *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    স্থির typval_T *
eval_রাশিমালা_no_emsg(কাঠামো debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_রাশিমালা(bp->dbg_name, ফাঁকা);
    --emsg_off;

    ফেরত tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    স্থির পূর্ণসংখ্যা
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    কাঠামো debuggy *bp;
    পূর্ণসংখ্যা		here = FALSE;

    যদি (ga_grow(gap, 1) == FAIL)
	ফেরত FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    যদি (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    অন্যথা যদি (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    অন্যথা যদি (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "here", 4) == 0)
    {
	যদি (curbuf->b_ffname == ফাঁকা)
	{
	    emsg(_(e_no_file_name));
	    ফেরত FAIL;
	}
	bp->dbg_type = DBG_FILE;
	here = TRUE;
    }
    অন্যথা যদি (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_রাশিমালা;
    অন্যথা
    {
	semsg(_(e_invalid_argument_str), p);
	ফেরত FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    যদি (here)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    অন্যথা যদি (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    অন্যথা
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    যদি ((!here && *p == NUL)
	    || (here && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && strstr((অক্ষর *)p, "()") != ফাঁকা))
    {
	semsg(_(e_invalid_argument_str), arg);
	ফেরত FAIL;
    }

    যদি (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    অন্যথা যদি (here)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    অন্যথা যদি (bp->dbg_type == DBG_রাশিমালা)
    {
	bp->dbg_name = vim_strsave(p);
	যদি (bp->dbg_name != ফাঁকা)
	    bp->dbg_val = eval_রাশিমালা_no_emsg(bp);
    }
    অন্যথা
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	যদি (q == ফাঁকা)
	    ফেরত FAIL;
	p = expand_env_save(q);
	vim_free(q);
	যদি (p == ফাঁকা)
	    ফেরত FAIL;
	যদি (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	অন্যথা
	    bp->dbg_name = p;
    }

    যদি (bp->dbg_name == ফাঁকা)
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    শূন্য
ex_breakadd(exarg_T *eap)
{
    কাঠামো debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#ifdef FEAT_PROFILE
    যদি (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#endif

    যদি (dbg_parsearg(eap->arg, gap) != OK)
	ফেরত;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    যদি (bp->dbg_type != DBG_রাশিমালা)
    {
	pat = file_pat_to_reg_pat(bp->dbg_name, ফাঁকা, ফাঁকা, FALSE);
	যদি (pat != ফাঁকা)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	যদি (pat == ফাঁকা || bp->dbg_prog == ফাঁকা)
	    vim_free(bp->dbg_name);
	অন্যথা
	{
	    যদি (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#ifdef FEAT_PROFILE
	    যদি (eap->cmdidx != CMD_profile)
#endif
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++last_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    অন্যথা
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++last_breakp;
	++debug_tick;
	যদি (gap == &dbg_breakp)
	    has_রাশিমালা_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    শূন্য
ex_debuggreedy(exarg_T *eap)
{
    যদি (eap->addr_গুনতি == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    অন্যথা
	debug_greedy = FALSE;
}

    স্থির শূন্য
update_has_রাশিমালা_breakpoint(শূন্য)
{
    পূর্ণসংখ্যা i;

    has_রাশিমালা_breakpoint = FALSE;
    জন্য (i = 0; i < dbg_breakp.ga_len; ++i)
	যদি (BREAKP(i).dbg_type == DBG_রাশিমালা)
	{
	    has_রাশিমালা_breakpoint = TRUE;
	    বিরতি;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    পূর্ণসংখ্যা
debug_has_রাশিমালা_breakpoint(শূন্য)
{
    ফেরত has_রাশিমালা_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    শূন্য
ex_breakdel(exarg_T *eap)
{
    কাঠামো debuggy *bp, *bpi;
    পূর্ণসংখ্যা		nr;
    পূর্ণসংখ্যা		todel = -1;
    পূর্ণসংখ্যা		del_all = FALSE;
    পূর্ণসংখ্যা		i;
    linenr_T	best_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    যদি (eap->cmdidx == CMD_profdel)
    {
#ifdef FEAT_PROFILE
	gap = &prof_ga;
#অন্যথা
	ex_ni(eap);
	ফেরত;
#endif
    }

    যদি (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((অক্ষর *)eap->arg);
	জন্য (i = 0; i < gap->ga_len; ++i)
	    যদি (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		বিরতি;
	    }
    }
    অন্যথা যদি (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    অন্যথা
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	যদি (dbg_parsearg(eap->arg, gap) == FAIL)
	    ফেরত;
	bp = &DEBUGGY(gap, gap->ga_len);
	জন্য (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    যদি (bp->dbg_type == bpi->dbg_type
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

    যদি (todel < 0)
    {
	semsg(_(e_breakpoint_not_found_str), eap->arg);
	ফেরত;
    }

    যখন (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#ifdef FEAT_EVAL
	যদি (DEBUGGY(gap, todel).dbg_type == DBG_রাশিমালা
		&& DEBUGGY(gap, todel).dbg_val != ফাঁকা)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#endif
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	যদি (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * আকার(কাঠামো debuggy));
#ifdef FEAT_PROFILE
	যদি (eap->cmdidx == CMD_breakdel)
#endif
	    ++debug_tick;
	যদি (!del_all)
	    বিরতি;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    যদি (gap->ga_len == 0)
	ga_clear(gap);
    যদি (gap == &dbg_breakp)
	update_has_রাশিমালা_breakpoint();
}

/*
 * ":breaklist".
 */
    শূন্য
ex_breaklist(exarg_T *eap UNUSED)
{
    কাঠামো debuggy *bp;
    পূর্ণসংখ্যা		i;

    যদি (dbg_breakp.ga_len == 0)
    {
	বার্তা(_("No breakpoints defined"));
	ফেরত;
    }

    জন্য (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	যদি (bp->dbg_type == DBG_FILE)
	    home_replace(ফাঁকা, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	যদি (bp->dbg_type != DBG_রাশিমালা)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (long)bp->dbg_lnum);
	অন্যথা
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
    পূর্ণসংখ্যা		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    ফেরত debuggy_find(file, fname, after, &dbg_breakp, ফাঁকা);
}

#যদি defined(FEAT_PROFILE)
# যদি defined(PROF_CACHE_LOG)
স্থির পূর্ণসংখ্যা গুনতি_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    পূর্ণসংখ্যা
has_profiling(
    পূর্ণসংখ্যা		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    পূর্ণসংখ্যা		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    যদি (prof_ga.ga_len == 0 || !prof_cache_initialized)
	ফেরত debuggy_find(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    যদি (hashp != ফাঁকা)
    {
	hash = *hashp;
	যদি (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    অন্যথা
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    যদি (HASHITEM_EMPTY(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	যদি (pe == ফাঁকা)
	    ফেরত FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	পূর্ণসংখ্যা f;
	পূর্ণসংখ্যা lnum = debuggy_find(file, fname, (linenr_T)0, &prof_ga, &f);
	যদি (lnum)
	{
	    PEN_SET_PROFILING(pe);
	    যদি (f)
		PEN_SET_FORCEIT(pe);
	}
	hash_add_item(ht, hi, pe->pen_name, hash);
# যদি defined(PROF_CACHE_LOG)
	ch_log(ফাঁকা, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    অন্যথা
	pe = HI2PE(hi);
    যদি (fp)
	*fp = PEN_IS_FORCEIT(pe);
# যদি defined(PROF_CACHE_LOG)
    গুনতি_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    ফেরত PEN_IS_PROFILING(pe);
}

    স্থির শূন্য
prof_clear_cache(শূন্য)
{
    যদি (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    জন্য (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	যদি (ht->ht_used > 0)
	{
# যদি defined(PROF_CACHE_LOG)
	    পূর্ণসংখ্যা idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(ফাঁকা, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_used, গুনতি_lookups[idx]);
	    গুনতি_lookups[idx] = 0;
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
    স্থির linenr_T
debuggy_find(
    পূর্ণসংখ্যা		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    পূর্ণসংখ্যা		*fp)	    // if not NULL: return forceit
{
    কাঠামো debuggy *bp;
    পূর্ণসংখ্যা		i;
    linenr_T	lnum = 0;
    char_u	*name = ফাঁকা;
    char_u	*short_name = fname;
    পূর্ণসংখ্যা		পূর্ব_got_int;

    // Return quickly when there are no breakpoints.
    যদি (gap->ga_len == 0)
	ফেরত (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    যদি (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	যদি (name != ফাঁকা)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    জন্য (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	যদি (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_রাশিমালা && (
#ifdef FEAT_PROFILE
		gap == &prof_ga ||
#endif
		(bp->dbg_lnum > after && (lnum == 0 || bp->dbg_lnum < lnum)))))
	{
	    // Save the value of got_int and reset it.  We don't want a
	    // previous interruption cancel matching, only hitting CTRL-C
	    // while matching should abort it.
	    পূর্ব_got_int = got_int;
	    got_int = FALSE;
	    যদি ((name != ফাঁকা
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		যদি (fp != ফাঁকা)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= পূর্ব_got_int;
#ifdef FEAT_PROFILE
	    যদি (lnum && gap == &prof_ga)
		বিরতি;
#endif
	}
#ifdef FEAT_EVAL
	অন্যথা যদি (bp->dbg_type == DBG_রাশিমালা)
	{
	    typval_T *tv;
	    পূর্ণসংখ্যা	      সারি = FALSE;

	    tv = eval_রাশিমালা_no_emsg(bp);
	    যদি (tv != ফাঁকা)
	    {
		যদি (bp->dbg_val == ফাঁকা)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(ফাঁকা, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    সারি = TRUE;
		}
		অন্যথা
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? রাশিমালা_EQUAL : রাশিমালা_IS;

		    যদি (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_সংখ্যা == FALSE)
		    {
			typval_T *v;

			সারি = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_রাশিমালা_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    অন্যথা যদি (bp->dbg_val != ফাঁকা)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(ফাঁকা, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = ফাঁকা;
		সারি = TRUE;
	    }

	    যদি (সারি)
	    {
		lnum = after > 0 ? after : 1;
		বিরতি;
	    }
	}
#endif
    }
    যদি (name != fname)
	vim_free(name);

    ফেরত lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    শূন্য
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#endif
