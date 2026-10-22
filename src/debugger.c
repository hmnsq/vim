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

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)
ساكن صحيح debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
ساكن فراغ do_setdebugtracelevel(char_u *arg);
ساكن فراغ do_checkbacktracelevel(فراغ);
ساكن فراغ do_showbacktrace(char_u *cmd);

ساكن char_u *debug_oldval = عدم;	// old and newval for debug expressions
ساكن char_u *debug_newval = عدم;
ساكن صحيح     debug_بیان   = 0;	// use debug_expr

    صحيح
has_watchexpr(فراغ)
{
    أرجع debug_بیان;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    فراغ
do_debug(char_u *cmd)
{
    صحيح		save_پیغام_scroll = پیغام_scroll;
    صحيح		save_State = State;
    صحيح		save_did_emsg = did_emsg;
    صحيح		save_cmd_silent = cmd_silent;
    صحيح		save_پیغام_silent = پیغام_silent;
    صحيح		save_emsg_silent = emsg_silent;
    صحيح		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    صحيح		typeahead_saved = FALSE;
    صحيح		save_ignore_script = 0;
    صحيح		save_ex_نورمال_busy;
    صحيح		n;
    char_u	*cmdline = عدم;
    char_u	*p;
    char_u	*sname;
    حرف	*لکۍ = عدم;
    ساكن صحيح	وروستی_cmd = 0;
#تعریف_کړه CMD_CONT	1
#تعریف_کړه CMD_بل	2
#تعریف_کړه CMD_ګام	3
#تعریف_کړه CMD_پای	4
#تعریف_کړه CMD_QUIT	5
#تعریف_کړه CMD_مداخله	6
#تعریف_کړه CMD_BACKTRACE	7
#تعریف_کړه CMD_چوکاټ	8
#تعریف_کړه CMD_پورته		9
#تعریف_کړه CMD_ښکته	10

#که_تعریف_شوی ALWAYS_USE_GUI
    // Can't do this when there is no terminal for input/output.
    إذا (!gui.دننه_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	أرجع;
    }
#د_که_پای

    // Make sure we are in raw mode and start termcap mode.  Might have side
    // effects...
    settmode(TMODE_RAW);
    starttermcap();

    ++RedrawingDisabled;	// don't redisplay the window
    ++no_wait_return;		// don't wait for return
    did_emsg = FALSE;		// don't use error from debugged stuff
    cmd_silent = FALSE;		// display commands
    پیغام_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = ډېرپېښ_نورمال;
    debug_ډېرپېښ = TRUE;

    إذا (!debug_did_پیغام)
	پیغام(_("Entering Debug mode.  Type \"cont\" to continue."));
    إذا (debug_oldval != عدم)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    إذا (debug_newval != عدم)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    إذا (sname != عدم)
	پیغام((حرف *)sname);
    vim_free(sname);
    إذا (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    وإلا
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    لكل (;;)
    {
	پیغام_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_نورمال_busy = ex_نورمال_busy;
	ex_نورمال_busy = 0;
	إذا (!debug_greedy)
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
	cmdline = getcmdline_prompt('>', عدم, 0, EXPAND_NOTHING, عدم);

	debug_break_level = n;
	إذا (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_نورمال_busy = save_ex_نورمال_busy;

	cmdline_لیکه = پیغام_لیکه;
	پیغام_starthere();
	إذا (cmdline != عدم)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    إذا (*p != NUL)
	    {
		بدل (*p)
		{
		    حالة 'c': وروستی_cmd = CMD_CONT;
			      لکۍ = "ont";
			      اكسر;
		    حالة 'n': وروستی_cmd = CMD_بل;
			      لکۍ = "ext";
			      اكسر;
		    حالة 's': وروستی_cmd = CMD_ګام;
			      لکۍ = "tep";
			      اكسر;
		    حالة 'f':
			      وروستی_cmd = 0;
			      إذا (p[1] == 'r')
			      {
				  وروستی_cmd = CMD_چوکاټ;
				  لکۍ = "rame";
			      }
			      وإلا
			      {
				  وروستی_cmd = CMD_پای;
				  لکۍ = "inish";
			      }
			      اكسر;
		    حالة 'q': وروستی_cmd = CMD_QUIT;
			      لکۍ = "uit";
			      اكسر;
		    حالة 'i': وروستی_cmd = CMD_مداخله;
			      لکۍ = "nterrupt";
			      اكسر;
		    حالة 'b': وروستی_cmd = CMD_BACKTRACE;
			      إذا (p[1] == 't')
				  لکۍ = "t";
			      وإلا
				  لکۍ = "acktrace";
			      اكسر;
		    حالة 'w': وروستی_cmd = CMD_BACKTRACE;
			      لکۍ = "here";
			      اكسر;
		    حالة 'u': وروستی_cmd = CMD_پورته;
			      لکۍ = "p";
			      اكسر;
		    حالة 'd': وروستی_cmd = CMD_ښکته;
			      لکۍ = "own";
			      اكسر;
		    افتراضي: وروستی_cmd = 0;
		}
		إذا (وروستی_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    طالما (*p != NUL && *p == *لکۍ)
		    {
			++p;
			++لکۍ;
		    }
		    إذا (ASCII_ISALPHA(*p) && وروستی_cmd != CMD_چوکاټ)
			وروستی_cmd = 0;
		}
	    }

	    إذا (وروستی_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		بدل (وروستی_cmd)
		{
		    حالة CMD_CONT:
			debug_break_level = -1;
			اكسر;
		    حالة CMD_بل:
			debug_break_level = ex_nesting_level;
			اكسر;
		    حالة CMD_ګام:
			debug_break_level = 9999;
			اكسر;
		    حالة CMD_پای:
			debug_break_level = ex_nesting_level - 1;
			اكسر;
		    حالة CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			اكسر;
		    حالة CMD_مداخله:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			وروستی_cmd = CMD_ګام;
			اكسر;
		    حالة CMD_BACKTRACE:
			do_showbacktrace(cmd);
			استمر;
		    حالة CMD_چوکاټ:
			إذا (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			وإلا
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			استمر;
		    حالة CMD_پورته:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			استمر;
		    حالة CMD_ښکته:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			استمر;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		اكسر;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (فراغ)do_cmdline(cmdline, getexline, عدم,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	کرښې_کیڼ = لیکې - 1;
    }
    vim_free(cmdline);

    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    پیغام_scroll = save_پیغام_scroll;
    restore_timeout_for_debugging();
    کرښې_کیڼ = لیکې - 1;
    State = save_State;
    debug_ډېرپېښ = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    پیغام_silent = save_پیغام_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_پیغام = TRUE;
}

    ساكن صحيح
get_maxbacktrace_level(char_u *sname)
{
    حرف	*p, *q;
    صحيح		maxbacktrace = 0;

    إذا (sname == عدم)
	أرجع 0;

    p = (حرف *)sname;
    طالما ((q = مزي_پلټنه(p, "..")) != عدم)
    {
	p = q + 2;
	maxbacktrace++;
    }
    أرجع maxbacktrace;
}

    ساكن فراغ
do_setdebugtracelevel(char_u *arg)
{
    صحيح level;

    level = atoi((حرف *)arg);
    إذا (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    وإلا
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    ساكن فراغ
do_checkbacktracelevel(فراغ)
{
    إذا (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	پیغام(_("frame is zero"));
    }
    وإلا
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	صحيح	max = get_maxbacktrace_level(sname);

	إذا (debug_backtrace_level > max)
	{
	    debug_backtrace_level = max;
	    smsg(_("frame at highest level: %d"), max);
	}
	vim_free(sname);
    }
}

    ساكن فراغ
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    حرف    *cur;
    حرف    *بل;
    صحيح	    i = 0;
    صحيح	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    إذا (sname != عدم)
    {
	cur = (حرف *)sname;
	طالما (!got_int)
	{
	    بل = مزي_پلټنه(cur, "..");
	    إذا (بل != عدم)
		*بل = NUL;
	    إذا (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    وإلا
		smsg("  %d %s", max - i, cur);
	    ++i;
	    إذا (بل == عدم)
		اكسر;
	    *بل = '.';
	    cur = بل + 2;
	}
	vim_free(sname);
    }

    إذا (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (اوږد)SOURCING_LNUM, cmd);
    وإلا
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    فراغ
ex_debug(exarg_T *eap)
{
    صحيح		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

ساكن char_u	*debug_breakpoint_name = عدم;
ساكن linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
ساكن صحيح	debug_skipped;
ساكن char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    فراغ
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    إذا (debug_breakpoint_name != عدم)
    {
	إذا (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    إذا (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    وإلا
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (اوږد)debug_breakpoint_lnum);
	    debug_breakpoint_name = عدم;
	    do_debug(eap->cmd);
	}
	وإلا
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = عدم;
	}
    }
    وإلا إذا (ex_nesting_level <= debug_break_level)
    {
	إذا (!eap->skip)
	    do_debug(eap->cmd);
	وإلا
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = عدم;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    صحيح
dbg_check_skipped(exarg_T *eap)
{
    صحيح		prev_got_int;

    إذا (!debug_skipped)
	أرجع FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    prev_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (فراغ)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= prev_got_int;
    أرجع TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
بنية debuggy
{
    صحيح		dbg_nr;		// breakpoint number
    صحيح		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    صحيح		dbg_forceit;	// ! used
#که_تعریف_شوی FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#د_که_پای
    صحيح		dbg_level;      // stored nested level for expr
};

ساكن garray_T dbg_breakp = {0, 0, حجم(بنية debuggy), 4, عدم};
#تعریف_کړه BREAKP(idx)		(((بنية debuggy *)dbg_breakp.ga_معلومات)[idx])
#تعریف_کړه DEBUGGY(gap, idx)	(((بنية debuggy *)gap->ga_معلومات)[idx])
ساكن صحيح وروستی_breakp = 0;	// nr of last defined breakpoint
ساكن صحيح has_بیان_breakpoint = FALSE;

#که_تعریف_شوی FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
ساكن garray_T prof_ga = {0, 0, حجم(بنية debuggy), 4, عدم};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
ساكن hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
ساكن صحيح prof_cache_initialized;
تعريفنوع بنية profentry_S
{
    حرف    pen_flags;	// cache data booleans: profiling, forceit
    char_u  pen_name[1]; // actually longer
} profentry_T;
# define PEN_FLAG_PROFILING 1
# define PEN_FLAG_FORCEIT 2
# define PEN_سېټ_PROFILING(pe) ((pe)->pen_flags |= PEN_FLAG_PROFILING)
# define PEN_سېټ_FORCEIT(pe) ((pe)->pen_flags |= PEN_FLAG_FORCEIT)
# define PEN_IS_PROFILING(pe) (((pe)->pen_flags & PEN_FLAG_PROFILING) != 0)
# define PEN_IS_FORCEIT(pe) (((pe)->pen_flags & PEN_FLAG_FORCEIT) != 0)

# define PE2HIKEY(pe)	((pe)->pen_name)
# define HIKEY2PE(p)	((profentry_T *)((p) - (offsetof(profentry_T, pen_name))))
# define HI2PE(hi)	HIKEY2PE((hi)->hi_کلی)

ساكن فراغ prof_clear_cache(فراغ);
# define PROF_CLEAR_CACHE(gap) افعل {إذا ((gap) == &prof_ga) prof_clear_cache();} طالما (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#وإلا
# define PROF_CLEAR_CACHE(gap) افعل {} طالما (0)
#د_که_پای
#تعریف_کړه DBG_FUNC	1
#تعریف_کړه DBG_FILE	2
#تعریف_کړه DBG_بیان	3

ساكن linenr_T debuggy_پیدا_کړه(صحيح file,char_u *fname, linenr_T after, garray_T *gap, صحيح *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    ساكن typval_T *
eval_بیان_no_emsg(بنية debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_بیان(bp->dbg_name, عدم);
    --emsg_off;

    أرجع tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    ساكن صحيح
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    بنية debuggy *bp;
    صحيح		دلته = FALSE;

    إذا (ga_grow(gap, 1) == FAIL)
	أرجع FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    إذا (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    وإلا إذا (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    وإلا إذا (
#که_تعریف_شوی FEAT_PROFILE
	    gap != &prof_ga &&
#د_که_پای
	    STRNCMP(p, "here", 4) == 0)
    {
	إذا (curbuf->b_ffname == عدم)
	{
	    emsg(_(e_no_file_name));
	    أرجع FAIL;
	}
	bp->dbg_type = DBG_FILE;
	دلته = TRUE;
    }
    وإلا إذا (
#که_تعریف_شوی FEAT_PROFILE
	    gap != &prof_ga &&
#د_که_پای
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_بیان;
    وإلا
    {
	semsg(_(e_invalid_دلیل_str), p);
	أرجع FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    إذا (دلته)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    وإلا إذا (
#که_تعریف_شوی FEAT_PROFILE
	    gap != &prof_ga &&
#د_که_پای
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    وإلا
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    إذا ((!دلته && *p == NUL)
	    || (دلته && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && مزي_پلټنه((حرف *)p, "()") != عدم))
    {
	semsg(_(e_invalid_دلیل_str), arg);
	أرجع FAIL;
    }

    إذا (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    وإلا إذا (دلته)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    وإلا إذا (bp->dbg_type == DBG_بیان)
    {
	bp->dbg_name = vim_strsave(p);
	إذا (bp->dbg_name != عدم)
	    bp->dbg_val = eval_بیان_no_emsg(bp);
    }
    وإلا
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	إذا (q == عدم)
	    أرجع FAIL;
	p = expand_env_save(q);
	vim_free(q);
	إذا (p == عدم)
	    أرجع FAIL;
	إذا (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	وإلا
	    bp->dbg_name = p;
    }

    إذا (bp->dbg_name == عدم)
	أرجع FAIL;
    أرجع OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    فراغ
ex_breakadd(exarg_T *eap)
{
    بنية debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#که_تعریف_شوی FEAT_PROFILE
    إذا (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#د_که_پای

    إذا (dbg_parsearg(eap->arg, gap) != OK)
	أرجع;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    إذا (bp->dbg_type != DBG_بیان)
    {
	pat = file_pat_منزل_reg_pat(bp->dbg_name, عدم, عدم, FALSE);
	إذا (pat != عدم)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	إذا (pat == عدم || bp->dbg_prog == عدم)
	    vim_free(bp->dbg_name);
	وإلا
	{
	    إذا (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#که_تعریف_شوی FEAT_PROFILE
	    إذا (eap->cmdidx != CMD_profile)
#د_که_پای
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++وروستی_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    وإلا
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++وروستی_breakp;
	++debug_tick;
	إذا (gap == &dbg_breakp)
	    has_بیان_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    فراغ
ex_debuggreedy(exarg_T *eap)
{
    إذا (eap->addr_شمېر == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    وإلا
	debug_greedy = FALSE;
}

    ساكن فراغ
update_has_بیان_breakpoint(فراغ)
{
    صحيح i;

    has_بیان_breakpoint = FALSE;
    لكل (i = 0; i < dbg_breakp.ga_len; ++i)
	إذا (BREAKP(i).dbg_type == DBG_بیان)
	{
	    has_بیان_breakpoint = TRUE;
	    اكسر;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    صحيح
debug_has_بیان_breakpoint(فراغ)
{
    أرجع has_بیان_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    فراغ
ex_breakdel(exarg_T *eap)
{
    بنية debuggy *bp, *bpi;
    صحيح		nr;
    صحيح		todel = -1;
    صحيح		del_all = FALSE;
    صحيح		i;
    linenr_T	غوره_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    إذا (eap->cmdidx == CMD_profdel)
    {
#که_تعریف_شوی FEAT_PROFILE
	gap = &prof_ga;
#وإلا
	ex_ni(eap);
	أرجع;
#د_که_پای
    }

    إذا (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((حرف *)eap->arg);
	لكل (i = 0; i < gap->ga_len; ++i)
	    إذا (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		اكسر;
	    }
    }
    وإلا إذا (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    وإلا
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	إذا (dbg_parsearg(eap->arg, gap) == FAIL)
	    أرجع;
	bp = &DEBUGGY(gap, gap->ga_len);
	لكل (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    إذا (bp->dbg_type == bpi->dbg_type
		    && STRCMP(bp->dbg_name, bpi->dbg_name) == 0
		    && (bp->dbg_lnum == bpi->dbg_lnum
			|| (bp->dbg_lnum == 0
			    && (غوره_lnum == 0
				|| bpi->dbg_lnum < غوره_lnum))))
	    {
		todel = i;
		غوره_lnum = bpi->dbg_lnum;
	    }
	}
	vim_free(bp->dbg_name);
    }

    إذا (todel < 0)
    {
	semsg(_(e_breakpoint_not_وموندل_str), eap->arg);
	أرجع;
    }

    طالما (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#که_تعریف_شوی FEAT_EVAL
	إذا (DEBUGGY(gap, todel).dbg_type == DBG_بیان
		&& DEBUGGY(gap, todel).dbg_val != عدم)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#د_که_پای
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	إذا (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * حجم(بنية debuggy));
#که_تعریف_شوی FEAT_PROFILE
	إذا (eap->cmdidx == CMD_breakdel)
#د_که_پای
	    ++debug_tick;
	إذا (!del_all)
	    اكسر;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    إذا (gap->ga_len == 0)
	ga_clear(gap);
    إذا (gap == &dbg_breakp)
	update_has_بیان_breakpoint();
}

/*
 * ":breaklist".
 */
    فراغ
ex_breaklist(exarg_T *eap ناکارول_شوی)
{
    بنية debuggy *bp;
    صحيح		i;

    إذا (dbg_breakp.ga_len == 0)
    {
	پیغام(_("No breakpoints defined"));
	أرجع;
    }

    لكل (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	إذا (bp->dbg_type == DBG_FILE)
	    home_replace(عدم, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	إذا (bp->dbg_type != DBG_بیان)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (اوږد)bp->dbg_lnum);
	وإلا
	    smsg(_("%3d  expr %s"),
		    bp->dbg_nr, bp->dbg_name);
    }
}

/*
 * Find a breakpoint for a function or sourced file.
 * Returns line number at which to break; zero when no matching breakpoint.
 */
    linenr_T
dbg_پیدا_کړه_breakpoint(
    صحيح		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    أرجع debuggy_پیدا_کړه(file, fname, after, &dbg_breakp, عدم);
}

#إذا defined(FEAT_PROFILE)
# إذا defined(PROF_CACHE_LOG)
ساكن صحيح شمېر_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    صحيح
has_profiling(
    صحيح		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    صحيح		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    إذا (prof_ga.ga_len == 0 || !prof_cache_initialized)
	أرجع debuggy_پیدا_کړه(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    إذا (hashp != عدم)
    {
	hash = *hashp;
	إذا (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    وإلا
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    إذا (HASHITEM_تشه(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	إذا (pe == عدم)
	    أرجع FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	صحيح f;
	صحيح lnum = debuggy_پیدا_کړه(file, fname, (linenr_T)0, &prof_ga, &f);
	إذا (lnum)
	{
	    PEN_سېټ_PROFILING(pe);
	    إذا (f)
		PEN_سېټ_FORCEIT(pe);
	}
	hash_جمع_کړه_توکی(ht, hi, pe->pen_name, hash);
# إذا defined(PROF_CACHE_LOG)
	ch_log(عدم, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    وإلا
	pe = HI2PE(hi);
    إذا (fp)
	*fp = PEN_IS_FORCEIT(pe);
# إذا defined(PROF_CACHE_LOG)
    شمېر_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    أرجع PEN_IS_PROFILING(pe);
}

    ساكن فراغ
prof_clear_cache(فراغ)
{
    إذا (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    لكل (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	إذا (ht->ht_کارول_شوی > 0)
	{
# إذا defined(PROF_CACHE_LOG)
	    صحيح idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(عدم, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_کارول_شوی, شمېر_lookups[idx]);
	    شمېر_lookups[idx] = 0;
# endif
	    hash_clear_all(ht, offsetof(profentry_T, pen_name));
	    hash_init(ht);
	}
    }
}
#د_که_پای

/*
 * Common code for dbg_find_breakpoint() and has_profiling().
 */
    ساكن linenr_T
debuggy_پیدا_کړه(
    صحيح		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    صحيح		*fp)	    // if not NULL: return forceit
{
    بنية debuggy *bp;
    صحيح		i;
    linenr_T	lnum = 0;
    char_u	*name = عدم;
    char_u	*short_name = fname;
    صحيح		prev_got_int;

    // Return quickly when there are no breakpoints.
    إذا (gap->ga_len == 0)
	أرجع (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    إذا (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	إذا (name != عدم)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    لكل (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	إذا (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_بیان && (
#که_تعریف_شوی FEAT_PROFILE
		gap == &prof_ga ||
#د_که_پای
		(bp->dbg_lnum > after && (lnum == 0 || bp->dbg_lnum < lnum)))))
	{
	    // Save the value of got_int and reset it.  We don't want a
	    // previous interruption cancel matching, only hitting CTRL-C
	    // while matching should abort it.
	    prev_got_int = got_int;
	    got_int = FALSE;
	    إذا ((name != عدم
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		إذا (fp != عدم)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#که_تعریف_شوی FEAT_PROFILE
	    إذا (lnum && gap == &prof_ga)
		اكسر;
#د_که_پای
	}
#که_تعریف_شوی FEAT_EVAL
	وإلا إذا (bp->dbg_type == DBG_بیان)
	{
	    typval_T *tv;
	    صحيح	      کرښه = FALSE;

	    tv = eval_بیان_no_emsg(bp);
	    إذا (tv != عدم)
	    {
		إذا (bp->dbg_val == عدم)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(عدم, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    کرښه = TRUE;
		}
		وإلا
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? بیان_EQUAL : بیان_IS;

		    إذا (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_شمېره == FALSE)
		    {
			typval_T *v;

			کرښه = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_بیان_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    وإلا إذا (bp->dbg_val != عدم)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(عدم, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = عدم;
		کرښه = TRUE;
	    }

	    إذا (کرښه)
	    {
		lnum = after > 0 ? after : 1;
		اكسر;
	    }
	}
#د_که_پای
    }
    إذا (name != fname)
	vim_free(name);

    أرجع lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    فراغ
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#د_که_پای
