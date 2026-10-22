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

#अन्तर्भाव "vim.h"

#सचे defined(FEAT_EVAL)
स्थिर पूर्णाङ्क debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
स्थिर शून्य do_setdebugtracelevel(char_u *arg);
स्थिर शून्य do_checkbacktracelevel(शून्य);
स्थिर शून्य do_showbacktrace(char_u *cmd);

स्थिर char_u *debug_oldval = रिक्त;	// old and newval for debug expressions
स्थिर char_u *debug_newval = रिक्त;
स्थिर पूर्णाङ्क     debug_व्यञ्जकम्   = 0;	// use debug_expr

    पूर्णाङ्क
has_watchexpr(शून्य)
{
    पटिनिवत्त debug_व्यञ्जकम्;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    शून्य
do_debug(char_u *cmd)
{
    पूर्णाङ्क		save_सन्देशः_scroll = सन्देशः_scroll;
    पूर्णाङ्क		save_State = State;
    पूर्णाङ्क		save_did_emsg = did_emsg;
    पूर्णाङ्क		save_cmd_silent = cmd_silent;
    पूर्णाङ्क		save_सन्देशः_silent = सन्देशः_silent;
    पूर्णाङ्क		save_emsg_silent = emsg_silent;
    पूर्णाङ्क		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    पूर्णाङ्क		typeahead_saved = FALSE;
    पूर्णाङ्क		save_ignore_script = 0;
    पूर्णाङ्क		save_ex_normal_busy;
    पूर्णाङ्क		n;
    char_u	*cmdline = रिक्त;
    char_u	*p;
    char_u	*sname;
    वर्ण	*tail = रिक्त;
    स्थिर पूर्णाङ्क	last_cmd = 0;
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
    सचे (!gui.in_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	पटिनिवत्त;
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
    सन्देशः_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = MODE_NORMAL;
    debug_mode = TRUE;

    सचे (!debug_did_सन्देशः)
	सन्देशः(_("Entering Debug mode.  Type \"cont\" to continue."));
    सचे (debug_oldval != रिक्त)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    सचे (debug_newval != रिक्त)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    सचे (sname != रिक्त)
	सन्देशः((वर्ण *)sname);
    vim_free(sname);
    सचे (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    अञ्ञथा
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    अत्थाय (;;)
    {
	सन्देशः_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_normal_busy = ex_normal_busy;
	ex_normal_busy = 0;
	सचे (!debug_greedy)
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
	cmdline = getcmdline_prompt('>', रिक्त, 0, EXPAND_NOTHING, रिक्त);

	debug_break_level = n;
	सचे (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_normal_busy = save_ex_normal_busy;

	cmdline_row = सन्देशः_row;
	सन्देशः_starthere();
	सचे (cmdline != रिक्त)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    सचे (*p != NUL)
	    {
		विकल्प (*p)
		{
		    पक्ष 'c': last_cmd = CMD_CONT;
			      tail = "ont";
			      भङ्ग;
		    पक्ष 'n': last_cmd = CMD_NEXT;
			      tail = "ext";
			      भङ्ग;
		    पक्ष 's': last_cmd = CMD_STEP;
			      tail = "tep";
			      भङ्ग;
		    पक्ष 'f':
			      last_cmd = 0;
			      सचे (p[1] == 'r')
			      {
				  last_cmd = CMD_FRAME;
				  tail = "rame";
			      }
			      अञ्ञथा
			      {
				  last_cmd = CMD_FINISH;
				  tail = "inish";
			      }
			      भङ्ग;
		    पक्ष 'q': last_cmd = CMD_QUIT;
			      tail = "uit";
			      भङ्ग;
		    पक्ष 'i': last_cmd = CMD_INTERRUPT;
			      tail = "nterrupt";
			      भङ्ग;
		    पक्ष 'b': last_cmd = CMD_BACKTRACE;
			      सचे (p[1] == 't')
				  tail = "t";
			      अञ्ञथा
				  tail = "acktrace";
			      भङ्ग;
		    पक्ष 'w': last_cmd = CMD_BACKTRACE;
			      tail = "here";
			      भङ्ग;
		    पक्ष 'u': last_cmd = CMD_UP;
			      tail = "p";
			      भङ्ग;
		    पक्ष 'd': last_cmd = CMD_DOWN;
			      tail = "own";
			      भङ्ग;
		    स्वभाव: last_cmd = 0;
		}
		सचे (last_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    याव (*p != NUL && *p == *tail)
		    {
			++p;
			++tail;
		    }
		    सचे (ASCII_ISALPHA(*p) && last_cmd != CMD_FRAME)
			last_cmd = 0;
		}
	    }

	    सचे (last_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		विकल्प (last_cmd)
		{
		    पक्ष CMD_CONT:
			debug_break_level = -1;
			भङ्ग;
		    पक्ष CMD_NEXT:
			debug_break_level = ex_nesting_level;
			भङ्ग;
		    पक्ष CMD_STEP:
			debug_break_level = 9999;
			भङ्ग;
		    पक्ष CMD_FINISH:
			debug_break_level = ex_nesting_level - 1;
			भङ्ग;
		    पक्ष CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			भङ्ग;
		    पक्ष CMD_INTERRUPT:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			last_cmd = CMD_STEP;
			भङ्ग;
		    पक्ष CMD_BACKTRACE:
			do_showbacktrace(cmd);
			अनुवर्त;
		    पक्ष CMD_FRAME:
			सचे (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			अञ्ञथा
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			अनुवर्त;
		    पक्ष CMD_UP:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			अनुवर्त;
		    पक्ष CMD_DOWN:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			अनुवर्त;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		भङ्ग;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (शून्य)do_cmdline(cmdline, getexline, रिक्त,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	पङ्क्तयः_वामः = Rows - 1;
    }
    vim_free(cmdline);

    सचे (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    सन्देशः_scroll = save_सन्देशः_scroll;
    restore_timeout_for_debugging();
    पङ्क्तयः_वामः = Rows - 1;
    State = save_State;
    debug_mode = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    सन्देशः_silent = save_सन्देशः_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_सन्देशः = TRUE;
}

    स्थिर पूर्णाङ्क
get_maxbacktrace_level(char_u *sname)
{
    वर्ण	*p, *q;
    पूर्णाङ्क		maxbacktrace = 0;

    सचे (sname == रिक्त)
	पटिनिवत्त 0;

    p = (वर्ण *)sname;
    याव ((q = strstr(p, "..")) != रिक्त)
    {
	p = q + 2;
	maxbacktrace++;
    }
    पटिनिवत्त maxbacktrace;
}

    स्थिर शून्य
do_setdebugtracelevel(char_u *arg)
{
    पूर्णाङ्क level;

    level = atoi((वर्ण *)arg);
    सचे (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    अञ्ञथा
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    स्थिर शून्य
do_checkbacktracelevel(शून्य)
{
    सचे (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	सन्देशः(_("frame is zero"));
    }
    अञ्ञथा
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	पूर्णाङ्क	max = get_maxbacktrace_level(sname);

	सचे (debug_backtrace_level > max)
	{
	    debug_backtrace_level = max;
	    smsg(_("frame at highest level: %d"), max);
	}
	vim_free(sname);
    }
}

    स्थिर शून्य
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    वर्ण    *cur;
    वर्ण    *next;
    पूर्णाङ्क	    i = 0;
    पूर्णाङ्क	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    सचे (sname != रिक्त)
    {
	cur = (वर्ण *)sname;
	याव (!got_int)
	{
	    next = strstr(cur, "..");
	    सचे (next != रिक्त)
		*next = NUL;
	    सचे (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    अञ्ञथा
		smsg("  %d %s", max - i, cur);
	    ++i;
	    सचे (next == रिक्त)
		भङ्ग;
	    *next = '.';
	    cur = next + 2;
	}
	vim_free(sname);
    }

    सचे (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (long)SOURCING_LNUM, cmd);
    अञ्ञथा
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    शून्य
ex_debug(exarg_T *eap)
{
    पूर्णाङ्क		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

स्थिर char_u	*debug_breakpoint_name = रिक्त;
स्थिर linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
स्थिर पूर्णाङ्क	debug_skipped;
स्थिर char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    शून्य
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    सचे (debug_breakpoint_name != रिक्त)
    {
	सचे (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    सचे (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    अञ्ञथा
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (long)debug_breakpoint_lnum);
	    debug_breakpoint_name = रिक्त;
	    do_debug(eap->cmd);
	}
	अञ्ञथा
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = रिक्त;
	}
    }
    अञ्ञथा सचे (ex_nesting_level <= debug_break_level)
    {
	सचे (!eap->skip)
	    do_debug(eap->cmd);
	अञ्ञथा
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = रिक्त;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    पूर्णाङ्क
dbg_check_skipped(exarg_T *eap)
{
    पूर्णाङ्क		prev_got_int;

    सचे (!debug_skipped)
	पटिनिवत्त FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    prev_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (शून्य)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= prev_got_int;
    पटिनिवत्त TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
संरचना debuggy
{
    पूर्णाङ्क		dbg_nr;		// breakpoint number
    पूर्णाङ्क		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    पूर्णाङ्क		dbg_forceit;	// ! used
#ifdef FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#endif
    पूर्णाङ्क		dbg_level;      // stored nested level for expr
};

स्थिर garray_T dbg_breakp = {0, 0, परिमाण(संरचना debuggy), 4, रिक्त};
#define BREAKP(idx)		(((संरचना debuggy *)dbg_breakp.ga_data)[idx])
#define DEBUGGY(gap, idx)	(((संरचना debuggy *)gap->ga_data)[idx])
स्थिर पूर्णाङ्क last_breakp = 0;	// nr of last defined breakpoint
स्थिर पूर्णाङ्क has_व्यञ्जकम्_breakpoint = FALSE;

#ifdef FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
स्थिर garray_T prof_ga = {0, 0, परिमाण(संरचना debuggy), 4, रिक्त};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
स्थिर hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
स्थिर पूर्णाङ्क prof_cache_initialized;
प्रकारनिर्देश संरचना profentry_S
{
    वर्ण    pen_flags;	// cache data booleans: profiling, forceit
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
# define HI2PE(hi)	HIKEY2PE((hi)->hi_कुञ्चिका)

स्थिर शून्य prof_clear_cache(शून्य);
# define PROF_CLEAR_CACHE(gap) करोहि {सचे ((gap) == &prof_ga) prof_clear_cache();} याव (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#अञ्ञथा
# define PROF_CLEAR_CACHE(gap) करोहि {} याव (0)
#endif
#define DBG_FUNC	1
#define DBG_FILE	2
#define DBG_व्यञ्जकम्	3

स्थिर linenr_T debuggy_find(पूर्णाङ्क file,char_u *fname, linenr_T after, garray_T *gap, पूर्णाङ्क *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    स्थिर typval_T *
eval_व्यञ्जकम्_no_emsg(संरचना debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_व्यञ्जकम्(bp->dbg_name, रिक्त);
    --emsg_off;

    पटिनिवत्त tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    स्थिर पूर्णाङ्क
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    संरचना debuggy *bp;
    पूर्णाङ्क		here = FALSE;

    सचे (ga_grow(gap, 1) == FAIL)
	पटिनिवत्त FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    सचे (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    अञ्ञथा सचे (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    अञ्ञथा सचे (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "here", 4) == 0)
    {
	सचे (curbuf->b_ffname == रिक्त)
	{
	    emsg(_(e_no_file_name));
	    पटिनिवत्त FAIL;
	}
	bp->dbg_type = DBG_FILE;
	here = TRUE;
    }
    अञ्ञथा सचे (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_व्यञ्जकम्;
    अञ्ञथा
    {
	semsg(_(e_invalid_argument_str), p);
	पटिनिवत्त FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    सचे (here)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    अञ्ञथा सचे (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    अञ्ञथा
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    सचे ((!here && *p == NUL)
	    || (here && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && strstr((वर्ण *)p, "()") != रिक्त))
    {
	semsg(_(e_invalid_argument_str), arg);
	पटिनिवत्त FAIL;
    }

    सचे (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    अञ्ञथा सचे (here)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    अञ्ञथा सचे (bp->dbg_type == DBG_व्यञ्जकम्)
    {
	bp->dbg_name = vim_strsave(p);
	सचे (bp->dbg_name != रिक्त)
	    bp->dbg_val = eval_व्यञ्जकम्_no_emsg(bp);
    }
    अञ्ञथा
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	सचे (q == रिक्त)
	    पटिनिवत्त FAIL;
	p = expand_env_save(q);
	vim_free(q);
	सचे (p == रिक्त)
	    पटिनिवत्त FAIL;
	सचे (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	अञ्ञथा
	    bp->dbg_name = p;
    }

    सचे (bp->dbg_name == रिक्त)
	पटिनिवत्त FAIL;
    पटिनिवत्त OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    शून्य
ex_breakadd(exarg_T *eap)
{
    संरचना debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#ifdef FEAT_PROFILE
    सचे (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#endif

    सचे (dbg_parsearg(eap->arg, gap) != OK)
	पटिनिवत्त;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    सचे (bp->dbg_type != DBG_व्यञ्जकम्)
    {
	pat = file_pat_to_reg_pat(bp->dbg_name, रिक्त, रिक्त, FALSE);
	सचे (pat != रिक्त)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	सचे (pat == रिक्त || bp->dbg_prog == रिक्त)
	    vim_free(bp->dbg_name);
	अञ्ञथा
	{
	    सचे (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#ifdef FEAT_PROFILE
	    सचे (eap->cmdidx != CMD_profile)
#endif
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++last_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    अञ्ञथा
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++last_breakp;
	++debug_tick;
	सचे (gap == &dbg_breakp)
	    has_व्यञ्जकम्_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    शून्य
ex_debuggreedy(exarg_T *eap)
{
    सचे (eap->addr_गणनम् == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    अञ्ञथा
	debug_greedy = FALSE;
}

    स्थिर शून्य
update_has_व्यञ्जकम्_breakpoint(शून्य)
{
    पूर्णाङ्क i;

    has_व्यञ्जकम्_breakpoint = FALSE;
    अत्थाय (i = 0; i < dbg_breakp.ga_len; ++i)
	सचे (BREAKP(i).dbg_type == DBG_व्यञ्जकम्)
	{
	    has_व्यञ्जकम्_breakpoint = TRUE;
	    भङ्ग;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    पूर्णाङ्क
debug_has_व्यञ्जकम्_breakpoint(शून्य)
{
    पटिनिवत्त has_व्यञ्जकम्_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    शून्य
ex_breakdel(exarg_T *eap)
{
    संरचना debuggy *bp, *bpi;
    पूर्णाङ्क		nr;
    पूर्णाङ्क		todel = -1;
    पूर्णाङ्क		del_all = FALSE;
    पूर्णाङ्क		i;
    linenr_T	best_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    सचे (eap->cmdidx == CMD_profdel)
    {
#ifdef FEAT_PROFILE
	gap = &prof_ga;
#अञ्ञथा
	ex_ni(eap);
	पटिनिवत्त;
#endif
    }

    सचे (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((वर्ण *)eap->arg);
	अत्थाय (i = 0; i < gap->ga_len; ++i)
	    सचे (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		भङ्ग;
	    }
    }
    अञ्ञथा सचे (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    अञ्ञथा
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	सचे (dbg_parsearg(eap->arg, gap) == FAIL)
	    पटिनिवत्त;
	bp = &DEBUGGY(gap, gap->ga_len);
	अत्थाय (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    सचे (bp->dbg_type == bpi->dbg_type
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

    सचे (todel < 0)
    {
	semsg(_(e_breakpoint_not_found_str), eap->arg);
	पटिनिवत्त;
    }

    याव (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#ifdef FEAT_EVAL
	सचे (DEBUGGY(gap, todel).dbg_type == DBG_व्यञ्जकम्
		&& DEBUGGY(gap, todel).dbg_val != रिक्त)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#endif
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	सचे (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * परिमाण(संरचना debuggy));
#ifdef FEAT_PROFILE
	सचे (eap->cmdidx == CMD_breakdel)
#endif
	    ++debug_tick;
	सचे (!del_all)
	    भङ्ग;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    सचे (gap->ga_len == 0)
	ga_clear(gap);
    सचे (gap == &dbg_breakp)
	update_has_व्यञ्जकम्_breakpoint();
}

/*
 * ":breaklist".
 */
    शून्य
ex_breaklist(exarg_T *eap UNUSED)
{
    संरचना debuggy *bp;
    पूर्णाङ्क		i;

    सचे (dbg_breakp.ga_len == 0)
    {
	सन्देशः(_("No breakpoints defined"));
	पटिनिवत्त;
    }

    अत्थाय (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	सचे (bp->dbg_type == DBG_FILE)
	    home_replace(रिक्त, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	सचे (bp->dbg_type != DBG_व्यञ्जकम्)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (long)bp->dbg_lnum);
	अञ्ञथा
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
    पूर्णाङ्क		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    पटिनिवत्त debuggy_find(file, fname, after, &dbg_breakp, रिक्त);
}

#सचे defined(FEAT_PROFILE)
# सचे defined(PROF_CACHE_LOG)
स्थिर पूर्णाङ्क गणनम्_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    पूर्णाङ्क
has_profiling(
    पूर्णाङ्क		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    पूर्णाङ्क		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    सचे (prof_ga.ga_len == 0 || !prof_cache_initialized)
	पटिनिवत्त debuggy_find(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    सचे (hashp != रिक्त)
    {
	hash = *hashp;
	सचे (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    अञ्ञथा
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    सचे (HASHITEM_EMPTY(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	सचे (pe == रिक्त)
	    पटिनिवत्त FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	पूर्णाङ्क f;
	पूर्णाङ्क lnum = debuggy_find(file, fname, (linenr_T)0, &prof_ga, &f);
	सचे (lnum)
	{
	    PEN_SET_PROFILING(pe);
	    सचे (f)
		PEN_SET_FORCEIT(pe);
	}
	hash_add_item(ht, hi, pe->pen_name, hash);
# सचे defined(PROF_CACHE_LOG)
	ch_log(रिक्त, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    अञ्ञथा
	pe = HI2PE(hi);
    सचे (fp)
	*fp = PEN_IS_FORCEIT(pe);
# सचे defined(PROF_CACHE_LOG)
    गणनम्_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    पटिनिवत्त PEN_IS_PROFILING(pe);
}

    स्थिर शून्य
prof_clear_cache(शून्य)
{
    सचे (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    अत्थाय (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	सचे (ht->ht_used > 0)
	{
# सचे defined(PROF_CACHE_LOG)
	    पूर्णाङ्क idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(रिक्त, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_used, गणनम्_lookups[idx]);
	    गणनम्_lookups[idx] = 0;
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
    स्थिर linenr_T
debuggy_find(
    पूर्णाङ्क		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    पूर्णाङ्क		*fp)	    // if not NULL: return forceit
{
    संरचना debuggy *bp;
    पूर्णाङ्क		i;
    linenr_T	lnum = 0;
    char_u	*name = रिक्त;
    char_u	*short_name = fname;
    पूर्णाङ्क		prev_got_int;

    // Return quickly when there are no breakpoints.
    सचे (gap->ga_len == 0)
	पटिनिवत्त (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    सचे (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	सचे (name != रिक्त)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    अत्थाय (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	सचे (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_व्यञ्जकम् && (
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
	    सचे ((name != रिक्त
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		सचे (fp != रिक्त)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#ifdef FEAT_PROFILE
	    सचे (lnum && gap == &prof_ga)
		भङ्ग;
#endif
	}
#ifdef FEAT_EVAL
	अञ्ञथा सचे (bp->dbg_type == DBG_व्यञ्जकम्)
	{
	    typval_T *tv;
	    पूर्णाङ्क	      पङ्क्तिः = FALSE;

	    tv = eval_व्यञ्जकम्_no_emsg(bp);
	    सचे (tv != रिक्त)
	    {
		सचे (bp->dbg_val == रिक्त)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(रिक्त, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    पङ्क्तिः = TRUE;
		}
		अञ्ञथा
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? व्यञ्जकम्_EQUAL : व्यञ्जकम्_IS;

		    सचे (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_सङ्ख्या == FALSE)
		    {
			typval_T *v;

			पङ्क्तिः = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_व्यञ्जकम्_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    अञ्ञथा सचे (bp->dbg_val != रिक्त)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(रिक्त, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = रिक्त;
		पङ्क्तिः = TRUE;
	    }

	    सचे (पङ्क्तिः)
	    {
		lnum = after > 0 ? after : 1;
		भङ्ग;
	    }
	}
#endif
    }
    सचे (name != fname)
	vim_free(name);

    पटिनिवत्त lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    शून्य
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#endif
