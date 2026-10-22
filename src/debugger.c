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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)
ایستا صحیح debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
ایستا تهی do_setdebugtracelevel(char_u *arg);
ایستا تهی do_checkbacktracelevel(تهی);
ایستا تهی do_showbacktrace(char_u *cmd);

ایستا char_u *debug_oldval = پوچ;	// old and newval for debug expressions
ایستا char_u *debug_newval = پوچ;
ایستا صحیح     debug_عبارت   = 0;	// use debug_expr

    صحیح
has_watchexpr(تهی)
{
    بازگشت debug_عبارت;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    تهی
do_debug(char_u *cmd)
{
    صحیح		save_پیام_scroll = پیام_scroll;
    صحیح		save_State = State;
    صحیح		save_did_emsg = did_emsg;
    صحیح		save_cmd_silent = cmd_silent;
    صحیح		save_پیام_silent = پیام_silent;
    صحیح		save_emsg_silent = emsg_silent;
    صحیح		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    صحیح		typeahead_saved = FALSE;
    صحیح		save_ignore_script = 0;
    صحیح		save_ex_normal_busy;
    صحیح		n;
    char_u	*cmdline = پوچ;
    char_u	*p;
    char_u	*sname;
    نویسه	*tail = پوچ;
    ایستا صحیح	last_cmd = 0;
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
    اگر (!gui.in_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	بازگشت;
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
    پیام_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = MODE_NORMAL;
    debug_mode = TRUE;

    اگر (!debug_did_پیام)
	پیام(_("Entering Debug mode.  Type \"cont\" to continue."));
    اگر (debug_oldval != پوچ)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    اگر (debug_newval != پوچ)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    اگر (sname != پوچ)
	پیام((نویسه *)sname);
    vim_free(sname);
    اگر (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    وگرنه
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    برای (;;)
    {
	پیام_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_normal_busy = ex_normal_busy;
	ex_normal_busy = 0;
	اگر (!debug_greedy)
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
	cmdline = getcmdline_prompt('>', پوچ, 0, EXPAND_NOTHING, پوچ);

	debug_break_level = n;
	اگر (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_normal_busy = save_ex_normal_busy;

	cmdline_row = پیام_row;
	پیام_starthere();
	اگر (cmdline != پوچ)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    اگر (*p != NUL)
	    {
		انتخاب (*p)
		{
		    حالت 'c': last_cmd = CMD_CONT;
			      tail = "ont";
			      بشکن;
		    حالت 'n': last_cmd = CMD_NEXT;
			      tail = "ext";
			      بشکن;
		    حالت 's': last_cmd = CMD_STEP;
			      tail = "tep";
			      بشکن;
		    حالت 'f':
			      last_cmd = 0;
			      اگر (p[1] == 'r')
			      {
				  last_cmd = CMD_FRAME;
				  tail = "rame";
			      }
			      وگرنه
			      {
				  last_cmd = CMD_FINISH;
				  tail = "inish";
			      }
			      بشکن;
		    حالت 'q': last_cmd = CMD_QUIT;
			      tail = "uit";
			      بشکن;
		    حالت 'i': last_cmd = CMD_INTERRUPT;
			      tail = "nterrupt";
			      بشکن;
		    حالت 'b': last_cmd = CMD_BACKTRACE;
			      اگر (p[1] == 't')
				  tail = "t";
			      وگرنه
				  tail = "acktrace";
			      بشکن;
		    حالت 'w': last_cmd = CMD_BACKTRACE;
			      tail = "here";
			      بشکن;
		    حالت 'u': last_cmd = CMD_UP;
			      tail = "p";
			      بشکن;
		    حالت 'd': last_cmd = CMD_DOWN;
			      tail = "own";
			      بشکن;
		    پیشفرض: last_cmd = 0;
		}
		اگر (last_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    وقتی (*p != NUL && *p == *tail)
		    {
			++p;
			++tail;
		    }
		    اگر (ASCII_ISALPHA(*p) && last_cmd != CMD_FRAME)
			last_cmd = 0;
		}
	    }

	    اگر (last_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		انتخاب (last_cmd)
		{
		    حالت CMD_CONT:
			debug_break_level = -1;
			بشکن;
		    حالت CMD_NEXT:
			debug_break_level = ex_nesting_level;
			بشکن;
		    حالت CMD_STEP:
			debug_break_level = 9999;
			بشکن;
		    حالت CMD_FINISH:
			debug_break_level = ex_nesting_level - 1;
			بشکن;
		    حالت CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			بشکن;
		    حالت CMD_INTERRUPT:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			last_cmd = CMD_STEP;
			بشکن;
		    حالت CMD_BACKTRACE:
			do_showbacktrace(cmd);
			ادامه;
		    حالت CMD_FRAME:
			اگر (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			وگرنه
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			ادامه;
		    حالت CMD_UP:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			ادامه;
		    حالت CMD_DOWN:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			ادامه;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		بشکن;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (تهی)do_cmdline(cmdline, getexline, پوچ,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	سطرها_چپ = Rows - 1;
    }
    vim_free(cmdline);

    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    پیام_scroll = save_پیام_scroll;
    restore_timeout_for_debugging();
    سطرها_چپ = Rows - 1;
    State = save_State;
    debug_mode = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    پیام_silent = save_پیام_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_پیام = TRUE;
}

    ایستا صحیح
get_maxbacktrace_level(char_u *sname)
{
    نویسه	*p, *q;
    صحیح		maxbacktrace = 0;

    اگر (sname == پوچ)
	بازگشت 0;

    p = (نویسه *)sname;
    وقتی ((q = strstr(p, "..")) != پوچ)
    {
	p = q + 2;
	maxbacktrace++;
    }
    بازگشت maxbacktrace;
}

    ایستا تهی
do_setdebugtracelevel(char_u *arg)
{
    صحیح level;

    level = atoi((نویسه *)arg);
    اگر (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    وگرنه
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    ایستا تهی
do_checkbacktracelevel(تهی)
{
    اگر (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	پیام(_("frame is zero"));
    }
    وگرنه
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	صحیح	max = get_maxbacktrace_level(sname);

	اگر (debug_backtrace_level > max)
	{
	    debug_backtrace_level = max;
	    smsg(_("frame at highest level: %d"), max);
	}
	vim_free(sname);
    }
}

    ایستا تهی
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    نویسه    *cur;
    نویسه    *next;
    صحیح	    i = 0;
    صحیح	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    اگر (sname != پوچ)
    {
	cur = (نویسه *)sname;
	وقتی (!got_int)
	{
	    next = strstr(cur, "..");
	    اگر (next != پوچ)
		*next = NUL;
	    اگر (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    وگرنه
		smsg("  %d %s", max - i, cur);
	    ++i;
	    اگر (next == پوچ)
		بشکن;
	    *next = '.';
	    cur = next + 2;
	}
	vim_free(sname);
    }

    اگر (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (long)SOURCING_LNUM, cmd);
    وگرنه
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    تهی
ex_debug(exarg_T *eap)
{
    صحیح		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

ایستا char_u	*debug_breakpoint_name = پوچ;
ایستا linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
ایستا صحیح	debug_skipped;
ایستا char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    تهی
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    اگر (debug_breakpoint_name != پوچ)
    {
	اگر (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    اگر (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    وگرنه
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (long)debug_breakpoint_lnum);
	    debug_breakpoint_name = پوچ;
	    do_debug(eap->cmd);
	}
	وگرنه
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = پوچ;
	}
    }
    وگرنه اگر (ex_nesting_level <= debug_break_level)
    {
	اگر (!eap->skip)
	    do_debug(eap->cmd);
	وگرنه
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = پوچ;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    صحیح
dbg_check_skipped(exarg_T *eap)
{
    صحیح		prev_got_int;

    اگر (!debug_skipped)
	بازگشت FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    prev_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (تهی)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= prev_got_int;
    بازگشت TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
ساختار debuggy
{
    صحیح		dbg_nr;		// breakpoint number
    صحیح		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    صحیح		dbg_forceit;	// ! used
#ifdef FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#endif
    صحیح		dbg_level;      // stored nested level for expr
};

ایستا garray_T dbg_breakp = {0, 0, اندازه(ساختار debuggy), 4, پوچ};
#define BREAKP(idx)		(((ساختار debuggy *)dbg_breakp.ga_data)[idx])
#define DEBUGGY(gap, idx)	(((ساختار debuggy *)gap->ga_data)[idx])
ایستا صحیح last_breakp = 0;	// nr of last defined breakpoint
ایستا صحیح has_عبارت_breakpoint = FALSE;

#ifdef FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
ایستا garray_T prof_ga = {0, 0, اندازه(ساختار debuggy), 4, پوچ};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
ایستا hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
ایستا صحیح prof_cache_initialized;
تعریفنوع ساختار profentry_S
{
    نویسه    pen_flags;	// cache data booleans: profiling, forceit
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
# define HI2PE(hi)	HIKEY2PE((hi)->hi_کلید)

ایستا تهی prof_clear_cache(تهی);
# define PROF_CLEAR_CACHE(gap) انجامبده {اگر ((gap) == &prof_ga) prof_clear_cache();} وقتی (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#وگرنه
# define PROF_CLEAR_CACHE(gap) انجامبده {} وقتی (0)
#endif
#define DBG_FUNC	1
#define DBG_FILE	2
#define DBG_عبارت	3

ایستا linenr_T debuggy_find(صحیح file,char_u *fname, linenr_T after, garray_T *gap, صحیح *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    ایستا typval_T *
eval_عبارت_no_emsg(ساختار debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_عبارت(bp->dbg_name, پوچ);
    --emsg_off;

    بازگشت tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    ایستا صحیح
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    ساختار debuggy *bp;
    صحیح		here = FALSE;

    اگر (ga_grow(gap, 1) == FAIL)
	بازگشت FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    اگر (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    وگرنه اگر (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    وگرنه اگر (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "here", 4) == 0)
    {
	اگر (curbuf->b_ffname == پوچ)
	{
	    emsg(_(e_no_file_name));
	    بازگشت FAIL;
	}
	bp->dbg_type = DBG_FILE;
	here = TRUE;
    }
    وگرنه اگر (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_عبارت;
    وگرنه
    {
	semsg(_(e_invalid_argument_str), p);
	بازگشت FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    اگر (here)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    وگرنه اگر (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    وگرنه
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    اگر ((!here && *p == NUL)
	    || (here && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && strstr((نویسه *)p, "()") != پوچ))
    {
	semsg(_(e_invalid_argument_str), arg);
	بازگشت FAIL;
    }

    اگر (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    وگرنه اگر (here)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    وگرنه اگر (bp->dbg_type == DBG_عبارت)
    {
	bp->dbg_name = vim_strsave(p);
	اگر (bp->dbg_name != پوچ)
	    bp->dbg_val = eval_عبارت_no_emsg(bp);
    }
    وگرنه
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	اگر (q == پوچ)
	    بازگشت FAIL;
	p = expand_env_save(q);
	vim_free(q);
	اگر (p == پوچ)
	    بازگشت FAIL;
	اگر (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	وگرنه
	    bp->dbg_name = p;
    }

    اگر (bp->dbg_name == پوچ)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    تهی
ex_breakadd(exarg_T *eap)
{
    ساختار debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#ifdef FEAT_PROFILE
    اگر (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#endif

    اگر (dbg_parsearg(eap->arg, gap) != OK)
	بازگشت;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    اگر (bp->dbg_type != DBG_عبارت)
    {
	pat = file_pat_to_reg_pat(bp->dbg_name, پوچ, پوچ, FALSE);
	اگر (pat != پوچ)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	اگر (pat == پوچ || bp->dbg_prog == پوچ)
	    vim_free(bp->dbg_name);
	وگرنه
	{
	    اگر (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#ifdef FEAT_PROFILE
	    اگر (eap->cmdidx != CMD_profile)
#endif
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++last_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    وگرنه
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++last_breakp;
	++debug_tick;
	اگر (gap == &dbg_breakp)
	    has_عبارت_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    تهی
ex_debuggreedy(exarg_T *eap)
{
    اگر (eap->addr_شمار == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    وگرنه
	debug_greedy = FALSE;
}

    ایستا تهی
update_has_عبارت_breakpoint(تهی)
{
    صحیح i;

    has_عبارت_breakpoint = FALSE;
    برای (i = 0; i < dbg_breakp.ga_len; ++i)
	اگر (BREAKP(i).dbg_type == DBG_عبارت)
	{
	    has_عبارت_breakpoint = TRUE;
	    بشکن;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    صحیح
debug_has_عبارت_breakpoint(تهی)
{
    بازگشت has_عبارت_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    تهی
ex_breakdel(exarg_T *eap)
{
    ساختار debuggy *bp, *bpi;
    صحیح		nr;
    صحیح		todel = -1;
    صحیح		del_all = FALSE;
    صحیح		i;
    linenr_T	best_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    اگر (eap->cmdidx == CMD_profdel)
    {
#ifdef FEAT_PROFILE
	gap = &prof_ga;
#وگرنه
	ex_ni(eap);
	بازگشت;
#endif
    }

    اگر (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((نویسه *)eap->arg);
	برای (i = 0; i < gap->ga_len; ++i)
	    اگر (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		بشکن;
	    }
    }
    وگرنه اگر (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    وگرنه
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	اگر (dbg_parsearg(eap->arg, gap) == FAIL)
	    بازگشت;
	bp = &DEBUGGY(gap, gap->ga_len);
	برای (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    اگر (bp->dbg_type == bpi->dbg_type
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

    اگر (todel < 0)
    {
	semsg(_(e_breakpoint_not_found_str), eap->arg);
	بازگشت;
    }

    وقتی (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#ifdef FEAT_EVAL
	اگر (DEBUGGY(gap, todel).dbg_type == DBG_عبارت
		&& DEBUGGY(gap, todel).dbg_val != پوچ)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#endif
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	اگر (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * اندازه(ساختار debuggy));
#ifdef FEAT_PROFILE
	اگر (eap->cmdidx == CMD_breakdel)
#endif
	    ++debug_tick;
	اگر (!del_all)
	    بشکن;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    اگر (gap->ga_len == 0)
	ga_clear(gap);
    اگر (gap == &dbg_breakp)
	update_has_عبارت_breakpoint();
}

/*
 * ":breaklist".
 */
    تهی
ex_breaklist(exarg_T *eap UNUSED)
{
    ساختار debuggy *bp;
    صحیح		i;

    اگر (dbg_breakp.ga_len == 0)
    {
	پیام(_("No breakpoints defined"));
	بازگشت;
    }

    برای (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	اگر (bp->dbg_type == DBG_FILE)
	    home_replace(پوچ, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	اگر (bp->dbg_type != DBG_عبارت)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (long)bp->dbg_lnum);
	وگرنه
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
    صحیح		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    بازگشت debuggy_find(file, fname, after, &dbg_breakp, پوچ);
}

#اگر defined(FEAT_PROFILE)
# اگر defined(PROF_CACHE_LOG)
ایستا صحیح شمار_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    صحیح
has_profiling(
    صحیح		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    صحیح		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    اگر (prof_ga.ga_len == 0 || !prof_cache_initialized)
	بازگشت debuggy_find(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    اگر (hashp != پوچ)
    {
	hash = *hashp;
	اگر (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    وگرنه
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    اگر (HASHITEM_EMPTY(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	اگر (pe == پوچ)
	    بازگشت FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	صحیح f;
	صحیح lnum = debuggy_find(file, fname, (linenr_T)0, &prof_ga, &f);
	اگر (lnum)
	{
	    PEN_SET_PROFILING(pe);
	    اگر (f)
		PEN_SET_FORCEIT(pe);
	}
	hash_add_item(ht, hi, pe->pen_name, hash);
# اگر defined(PROF_CACHE_LOG)
	ch_log(پوچ, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    وگرنه
	pe = HI2PE(hi);
    اگر (fp)
	*fp = PEN_IS_FORCEIT(pe);
# اگر defined(PROF_CACHE_LOG)
    شمار_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    بازگشت PEN_IS_PROFILING(pe);
}

    ایستا تهی
prof_clear_cache(تهی)
{
    اگر (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    برای (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	اگر (ht->ht_used > 0)
	{
# اگر defined(PROF_CACHE_LOG)
	    صحیح idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(پوچ, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_used, شمار_lookups[idx]);
	    شمار_lookups[idx] = 0;
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
    ایستا linenr_T
debuggy_find(
    صحیح		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    صحیح		*fp)	    // if not NULL: return forceit
{
    ساختار debuggy *bp;
    صحیح		i;
    linenr_T	lnum = 0;
    char_u	*name = پوچ;
    char_u	*short_name = fname;
    صحیح		prev_got_int;

    // Return quickly when there are no breakpoints.
    اگر (gap->ga_len == 0)
	بازگشت (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    اگر (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	اگر (name != پوچ)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    برای (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	اگر (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_عبارت && (
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
	    اگر ((name != پوچ
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		اگر (fp != پوچ)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#ifdef FEAT_PROFILE
	    اگر (lnum && gap == &prof_ga)
		بشکن;
#endif
	}
#ifdef FEAT_EVAL
	وگرنه اگر (bp->dbg_type == DBG_عبارت)
	{
	    typval_T *tv;
	    صحیح	      سطر = FALSE;

	    tv = eval_عبارت_no_emsg(bp);
	    اگر (tv != پوچ)
	    {
		اگر (bp->dbg_val == پوچ)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(پوچ, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    سطر = TRUE;
		}
		وگرنه
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? عبارت_EQUAL : عبارت_IS;

		    اگر (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_عدد == FALSE)
		    {
			typval_T *v;

			سطر = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_عبارت_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    وگرنه اگر (bp->dbg_val != پوچ)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(پوچ, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = پوچ;
		سطر = TRUE;
	    }

	    اگر (سطر)
	    {
		lnum = after > 0 ? after : 1;
		بشکن;
	    }
	}
#endif
    }
    اگر (name != fname)
	vim_free(name);

    بازگشت lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    تهی
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#endif
