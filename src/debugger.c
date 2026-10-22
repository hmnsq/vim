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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)
நிலையான முழு debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
நிலையான வெறுமை do_setdebugtracelevel(char_u *arg);
நிலையான வெறுமை do_checkbacktracelevel(வெறுமை);
நிலையான வெறுமை do_showbacktrace(char_u *cmd);

நிலையான char_u *debug_oldval = வெற்று;	// old and newval for debug expressions
நிலையான char_u *debug_newval = வெற்று;
நிலையான முழு     debug_கோவை   = 0;	// use debug_expr

    முழு
has_watchexpr(வெறுமை)
{
    திரும்பு debug_கோவை;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    வெறுமை
do_debug(char_u *cmd)
{
    முழு		save_செய்தி_scroll = செய்தி_scroll;
    முழு		save_State = State;
    முழு		save_did_emsg = did_emsg;
    முழு		save_cmd_silent = cmd_silent;
    முழு		save_செய்தி_silent = செய்தி_silent;
    முழு		save_emsg_silent = emsg_silent;
    முழு		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    முழு		typeahead_saved = FALSE;
    முழு		save_ignore_script = 0;
    முழு		save_ex_normal_busy;
    முழு		n;
    char_u	*cmdline = வெற்று;
    char_u	*p;
    char_u	*sname;
    எழுத்து	*tail = வெற்று;
    நிலையான முழு	last_cmd = 0;
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
    எனில் (!gui.in_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	திரும்பு;
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
    செய்தி_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = MODE_NORMAL;
    debug_mode = TRUE;

    எனில் (!debug_did_செய்தி)
	செய்தி(_("Entering Debug mode.  Type \"cont\" to continue."));
    எனில் (debug_oldval != வெற்று)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    எனில் (debug_newval != வெற்று)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    எனில் (sname != வெற்று)
	செய்தி((எழுத்து *)sname);
    vim_free(sname);
    எனில் (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    இல்லையெனில்
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    க்கு (;;)
    {
	செய்தி_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_normal_busy = ex_normal_busy;
	ex_normal_busy = 0;
	எனில் (!debug_greedy)
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
	cmdline = getcmdline_prompt('>', வெற்று, 0, EXPAND_NOTHING, வெற்று);

	debug_break_level = n;
	எனில் (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_normal_busy = save_ex_normal_busy;

	cmdline_row = செய்தி_row;
	செய்தி_starthere();
	எனில் (cmdline != வெற்று)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    எனில் (*p != NUL)
	    {
		தேர்வு (*p)
		{
		    நிலை 'c': last_cmd = CMD_CONT;
			      tail = "ont";
			      நிறுத்து;
		    நிலை 'n': last_cmd = CMD_NEXT;
			      tail = "ext";
			      நிறுத்து;
		    நிலை 's': last_cmd = CMD_STEP;
			      tail = "tep";
			      நிறுத்து;
		    நிலை 'f':
			      last_cmd = 0;
			      எனில் (p[1] == 'r')
			      {
				  last_cmd = CMD_FRAME;
				  tail = "rame";
			      }
			      இல்லையெனில்
			      {
				  last_cmd = CMD_FINISH;
				  tail = "inish";
			      }
			      நிறுத்து;
		    நிலை 'q': last_cmd = CMD_QUIT;
			      tail = "uit";
			      நிறுத்து;
		    நிலை 'i': last_cmd = CMD_INTERRUPT;
			      tail = "nterrupt";
			      நிறுத்து;
		    நிலை 'b': last_cmd = CMD_BACKTRACE;
			      எனில் (p[1] == 't')
				  tail = "t";
			      இல்லையெனில்
				  tail = "acktrace";
			      நிறுத்து;
		    நிலை 'w': last_cmd = CMD_BACKTRACE;
			      tail = "here";
			      நிறுத்து;
		    நிலை 'u': last_cmd = CMD_UP;
			      tail = "p";
			      நிறுத்து;
		    நிலை 'd': last_cmd = CMD_DOWN;
			      tail = "own";
			      நிறுத்து;
		    இயல்பு: last_cmd = 0;
		}
		எனில் (last_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    போது (*p != NUL && *p == *tail)
		    {
			++p;
			++tail;
		    }
		    எனில் (ASCII_ISALPHA(*p) && last_cmd != CMD_FRAME)
			last_cmd = 0;
		}
	    }

	    எனில் (last_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		தேர்வு (last_cmd)
		{
		    நிலை CMD_CONT:
			debug_break_level = -1;
			நிறுத்து;
		    நிலை CMD_NEXT:
			debug_break_level = ex_nesting_level;
			நிறுத்து;
		    நிலை CMD_STEP:
			debug_break_level = 9999;
			நிறுத்து;
		    நிலை CMD_FINISH:
			debug_break_level = ex_nesting_level - 1;
			நிறுத்து;
		    நிலை CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			நிறுத்து;
		    நிலை CMD_INTERRUPT:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			last_cmd = CMD_STEP;
			நிறுத்து;
		    நிலை CMD_BACKTRACE:
			do_showbacktrace(cmd);
			தொடர்;
		    நிலை CMD_FRAME:
			எனில் (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			இல்லையெனில்
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			தொடர்;
		    நிலை CMD_UP:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			தொடர்;
		    நிலை CMD_DOWN:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			தொடர்;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		நிறுத்து;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (வெறுமை)do_cmdline(cmdline, getexline, வெற்று,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	வரிகள்_இடது = Rows - 1;
    }
    vim_free(cmdline);

    எனில் (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    செய்தி_scroll = save_செய்தி_scroll;
    restore_timeout_for_debugging();
    வரிகள்_இடது = Rows - 1;
    State = save_State;
    debug_mode = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    செய்தி_silent = save_செய்தி_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_செய்தி = TRUE;
}

    நிலையான முழு
get_maxbacktrace_level(char_u *sname)
{
    எழுத்து	*p, *q;
    முழு		maxbacktrace = 0;

    எனில் (sname == வெற்று)
	திரும்பு 0;

    p = (எழுத்து *)sname;
    போது ((q = strstr(p, "..")) != வெற்று)
    {
	p = q + 2;
	maxbacktrace++;
    }
    திரும்பு maxbacktrace;
}

    நிலையான வெறுமை
do_setdebugtracelevel(char_u *arg)
{
    முழு level;

    level = atoi((எழுத்து *)arg);
    எனில் (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    இல்லையெனில்
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    நிலையான வெறுமை
do_checkbacktracelevel(வெறுமை)
{
    எனில் (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	செய்தி(_("frame is zero"));
    }
    இல்லையெனில்
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	முழு	max = get_maxbacktrace_level(sname);

	எனில் (debug_backtrace_level > max)
	{
	    debug_backtrace_level = max;
	    smsg(_("frame at highest level: %d"), max);
	}
	vim_free(sname);
    }
}

    நிலையான வெறுமை
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    எழுத்து    *cur;
    எழுத்து    *next;
    முழு	    i = 0;
    முழு	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    எனில் (sname != வெற்று)
    {
	cur = (எழுத்து *)sname;
	போது (!got_int)
	{
	    next = strstr(cur, "..");
	    எனில் (next != வெற்று)
		*next = NUL;
	    எனில் (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    இல்லையெனில்
		smsg("  %d %s", max - i, cur);
	    ++i;
	    எனில் (next == வெற்று)
		நிறுத்து;
	    *next = '.';
	    cur = next + 2;
	}
	vim_free(sname);
    }

    எனில் (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (long)SOURCING_LNUM, cmd);
    இல்லையெனில்
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    வெறுமை
ex_debug(exarg_T *eap)
{
    முழு		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

நிலையான char_u	*debug_breakpoint_name = வெற்று;
நிலையான linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
நிலையான முழு	debug_skipped;
நிலையான char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    வெறுமை
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    எனில் (debug_breakpoint_name != வெற்று)
    {
	எனில் (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    எனில் (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    இல்லையெனில்
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (long)debug_breakpoint_lnum);
	    debug_breakpoint_name = வெற்று;
	    do_debug(eap->cmd);
	}
	இல்லையெனில்
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = வெற்று;
	}
    }
    இல்லையெனில் எனில் (ex_nesting_level <= debug_break_level)
    {
	எனில் (!eap->skip)
	    do_debug(eap->cmd);
	இல்லையெனில்
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = வெற்று;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    முழு
dbg_check_skipped(exarg_T *eap)
{
    முழு		prev_got_int;

    எனில் (!debug_skipped)
	திரும்பு FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    prev_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (வெறுமை)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= prev_got_int;
    திரும்பு TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
கட்டமைப்பு debuggy
{
    முழு		dbg_nr;		// breakpoint number
    முழு		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    முழு		dbg_forceit;	// ! used
#ifdef FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#endif
    முழு		dbg_level;      // stored nested level for expr
};

நிலையான garray_T dbg_breakp = {0, 0, அளவு(கட்டமைப்பு debuggy), 4, வெற்று};
#define BREAKP(idx)		(((கட்டமைப்பு debuggy *)dbg_breakp.ga_data)[idx])
#define DEBUGGY(gap, idx)	(((கட்டமைப்பு debuggy *)gap->ga_data)[idx])
நிலையான முழு last_breakp = 0;	// nr of last defined breakpoint
நிலையான முழு has_கோவை_breakpoint = FALSE;

#ifdef FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
நிலையான garray_T prof_ga = {0, 0, அளவு(கட்டமைப்பு debuggy), 4, வெற்று};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
நிலையான hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
நிலையான முழு prof_cache_initialized;
வகைவரை கட்டமைப்பு profentry_S
{
    எழுத்து    pen_flags;	// cache data booleans: profiling, forceit
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
# define HI2PE(hi)	HIKEY2PE((hi)->hi_சாவி)

நிலையான வெறுமை prof_clear_cache(வெறுமை);
# define PROF_CLEAR_CACHE(gap) செய் {எனில் ((gap) == &prof_ga) prof_clear_cache();} போது (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#இல்லையெனில்
# define PROF_CLEAR_CACHE(gap) செய் {} போது (0)
#endif
#define DBG_FUNC	1
#define DBG_FILE	2
#define DBG_கோவை	3

நிலையான linenr_T debuggy_find(முழு file,char_u *fname, linenr_T after, garray_T *gap, முழு *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    நிலையான typval_T *
eval_கோவை_no_emsg(கட்டமைப்பு debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_கோவை(bp->dbg_name, வெற்று);
    --emsg_off;

    திரும்பு tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    நிலையான முழு
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    கட்டமைப்பு debuggy *bp;
    முழு		here = FALSE;

    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    எனில் (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    இல்லையெனில் எனில் (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    இல்லையெனில் எனில் (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "here", 4) == 0)
    {
	எனில் (curbuf->b_ffname == வெற்று)
	{
	    emsg(_(e_no_file_name));
	    திரும்பு FAIL;
	}
	bp->dbg_type = DBG_FILE;
	here = TRUE;
    }
    இல்லையெனில் எனில் (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_கோவை;
    இல்லையெனில்
    {
	semsg(_(e_invalid_argument_str), p);
	திரும்பு FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    எனில் (here)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    இல்லையெனில் எனில் (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    இல்லையெனில்
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    எனில் ((!here && *p == NUL)
	    || (here && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && strstr((எழுத்து *)p, "()") != வெற்று))
    {
	semsg(_(e_invalid_argument_str), arg);
	திரும்பு FAIL;
    }

    எனில் (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    இல்லையெனில் எனில் (here)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    இல்லையெனில் எனில் (bp->dbg_type == DBG_கோவை)
    {
	bp->dbg_name = vim_strsave(p);
	எனில் (bp->dbg_name != வெற்று)
	    bp->dbg_val = eval_கோவை_no_emsg(bp);
    }
    இல்லையெனில்
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	எனில் (q == வெற்று)
	    திரும்பு FAIL;
	p = expand_env_save(q);
	vim_free(q);
	எனில் (p == வெற்று)
	    திரும்பு FAIL;
	எனில் (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	இல்லையெனில்
	    bp->dbg_name = p;
    }

    எனில் (bp->dbg_name == வெற்று)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    வெறுமை
ex_breakadd(exarg_T *eap)
{
    கட்டமைப்பு debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#ifdef FEAT_PROFILE
    எனில் (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#endif

    எனில் (dbg_parsearg(eap->arg, gap) != OK)
	திரும்பு;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    எனில் (bp->dbg_type != DBG_கோவை)
    {
	pat = file_pat_to_reg_pat(bp->dbg_name, வெற்று, வெற்று, FALSE);
	எனில் (pat != வெற்று)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	எனில் (pat == வெற்று || bp->dbg_prog == வெற்று)
	    vim_free(bp->dbg_name);
	இல்லையெனில்
	{
	    எனில் (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#ifdef FEAT_PROFILE
	    எனில் (eap->cmdidx != CMD_profile)
#endif
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++last_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    இல்லையெனில்
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++last_breakp;
	++debug_tick;
	எனில் (gap == &dbg_breakp)
	    has_கோவை_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    வெறுமை
ex_debuggreedy(exarg_T *eap)
{
    எனில் (eap->addr_மொத்தம் == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    இல்லையெனில்
	debug_greedy = FALSE;
}

    நிலையான வெறுமை
update_has_கோவை_breakpoint(வெறுமை)
{
    முழு i;

    has_கோவை_breakpoint = FALSE;
    க்கு (i = 0; i < dbg_breakp.ga_len; ++i)
	எனில் (BREAKP(i).dbg_type == DBG_கோவை)
	{
	    has_கோவை_breakpoint = TRUE;
	    நிறுத்து;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    முழு
debug_has_கோவை_breakpoint(வெறுமை)
{
    திரும்பு has_கோவை_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    வெறுமை
ex_breakdel(exarg_T *eap)
{
    கட்டமைப்பு debuggy *bp, *bpi;
    முழு		nr;
    முழு		todel = -1;
    முழு		del_all = FALSE;
    முழு		i;
    linenr_T	best_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    எனில் (eap->cmdidx == CMD_profdel)
    {
#ifdef FEAT_PROFILE
	gap = &prof_ga;
#இல்லையெனில்
	ex_ni(eap);
	திரும்பு;
#endif
    }

    எனில் (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((எழுத்து *)eap->arg);
	க்கு (i = 0; i < gap->ga_len; ++i)
	    எனில் (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		நிறுத்து;
	    }
    }
    இல்லையெனில் எனில் (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    இல்லையெனில்
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	எனில் (dbg_parsearg(eap->arg, gap) == FAIL)
	    திரும்பு;
	bp = &DEBUGGY(gap, gap->ga_len);
	க்கு (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    எனில் (bp->dbg_type == bpi->dbg_type
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

    எனில் (todel < 0)
    {
	semsg(_(e_breakpoint_not_found_str), eap->arg);
	திரும்பு;
    }

    போது (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#ifdef FEAT_EVAL
	எனில் (DEBUGGY(gap, todel).dbg_type == DBG_கோவை
		&& DEBUGGY(gap, todel).dbg_val != வெற்று)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#endif
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	எனில் (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * அளவு(கட்டமைப்பு debuggy));
#ifdef FEAT_PROFILE
	எனில் (eap->cmdidx == CMD_breakdel)
#endif
	    ++debug_tick;
	எனில் (!del_all)
	    நிறுத்து;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    எனில் (gap->ga_len == 0)
	ga_clear(gap);
    எனில் (gap == &dbg_breakp)
	update_has_கோவை_breakpoint();
}

/*
 * ":breaklist".
 */
    வெறுமை
ex_breaklist(exarg_T *eap UNUSED)
{
    கட்டமைப்பு debuggy *bp;
    முழு		i;

    எனில் (dbg_breakp.ga_len == 0)
    {
	செய்தி(_("No breakpoints defined"));
	திரும்பு;
    }

    க்கு (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	எனில் (bp->dbg_type == DBG_FILE)
	    home_replace(வெற்று, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	எனில் (bp->dbg_type != DBG_கோவை)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (long)bp->dbg_lnum);
	இல்லையெனில்
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
    முழு		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    திரும்பு debuggy_find(file, fname, after, &dbg_breakp, வெற்று);
}

#எனில் defined(FEAT_PROFILE)
# எனில் defined(PROF_CACHE_LOG)
நிலையான முழு மொத்தம்_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    முழு
has_profiling(
    முழு		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    முழு		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    எனில் (prof_ga.ga_len == 0 || !prof_cache_initialized)
	திரும்பு debuggy_find(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    எனில் (hashp != வெற்று)
    {
	hash = *hashp;
	எனில் (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    இல்லையெனில்
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    எனில் (HASHITEM_EMPTY(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	எனில் (pe == வெற்று)
	    திரும்பு FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	முழு f;
	முழு lnum = debuggy_find(file, fname, (linenr_T)0, &prof_ga, &f);
	எனில் (lnum)
	{
	    PEN_SET_PROFILING(pe);
	    எனில் (f)
		PEN_SET_FORCEIT(pe);
	}
	hash_add_item(ht, hi, pe->pen_name, hash);
# எனில் defined(PROF_CACHE_LOG)
	ch_log(வெற்று, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    இல்லையெனில்
	pe = HI2PE(hi);
    எனில் (fp)
	*fp = PEN_IS_FORCEIT(pe);
# எனில் defined(PROF_CACHE_LOG)
    மொத்தம்_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    திரும்பு PEN_IS_PROFILING(pe);
}

    நிலையான வெறுமை
prof_clear_cache(வெறுமை)
{
    எனில் (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    க்கு (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	எனில் (ht->ht_used > 0)
	{
# எனில் defined(PROF_CACHE_LOG)
	    முழு idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(வெற்று, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_used, மொத்தம்_lookups[idx]);
	    மொத்தம்_lookups[idx] = 0;
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
    நிலையான linenr_T
debuggy_find(
    முழு		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    முழு		*fp)	    // if not NULL: return forceit
{
    கட்டமைப்பு debuggy *bp;
    முழு		i;
    linenr_T	lnum = 0;
    char_u	*name = வெற்று;
    char_u	*short_name = fname;
    முழு		prev_got_int;

    // Return quickly when there are no breakpoints.
    எனில் (gap->ga_len == 0)
	திரும்பு (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    எனில் (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	எனில் (name != வெற்று)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    க்கு (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	எனில் (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_கோவை && (
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
	    எனில் ((name != வெற்று
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		எனில் (fp != வெற்று)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#ifdef FEAT_PROFILE
	    எனில் (lnum && gap == &prof_ga)
		நிறுத்து;
#endif
	}
#ifdef FEAT_EVAL
	இல்லையெனில் எனில் (bp->dbg_type == DBG_கோவை)
	{
	    typval_T *tv;
	    முழு	      வரி = FALSE;

	    tv = eval_கோவை_no_emsg(bp);
	    எனில் (tv != வெற்று)
	    {
		எனில் (bp->dbg_val == வெற்று)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(வெற்று, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    வரி = TRUE;
		}
		இல்லையெனில்
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? கோவை_EQUAL : கோவை_IS;

		    எனில் (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_எண் == FALSE)
		    {
			typval_T *v;

			வரி = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_கோவை_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    இல்லையெனில் எனில் (bp->dbg_val != வெற்று)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(வெற்று, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = வெற்று;
		வரி = TRUE;
	    }

	    எனில் (வரி)
	    {
		lnum = after > 0 ? after : 1;
		நிறுத்து;
	    }
	}
#endif
    }
    எனில் (name != fname)
	vim_free(name);

    திரும்பு lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    வெறுமை
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#endif
