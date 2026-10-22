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

#यदि defined(FEAT_EVAL)
स्थिर पूर्णाङ्क debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
स्थिर शून्य do_setdebugtracelevel(char_u *arg);
स्थिर शून्य do_checkbacktracelevel(शून्य);
स्थिर शून्य do_showbacktrace(char_u *cmd);

स्थिर char_u *debug_oldval = रिक्त;	// old and newval for debug expressions
स्थिर char_u *debug_newval = रिक्त;
स्थिर पूर्णाङ्क     debug_अभिव्यक्ति   = 0;	// use debug_expr

    पूर्णाङ्क
has_watchexpr(शून्य)
{
    फर्काऊ debug_अभिव्यक्ति;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    शून्य
do_debug(char_u *cmd)
{
    पूर्णाङ्क		save_सन्देश_scroll = सन्देश_scroll;
    पूर्णाङ्क		save_State = State;
    पूर्णाङ्क		save_did_emsg = did_emsg;
    पूर्णाङ्क		save_cmd_silent = cmd_silent;
    पूर्णाङ्क		save_सन्देश_silent = सन्देश_silent;
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
    यदि (!gui.in_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	फर्काऊ;
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
    सन्देश_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = MODE_NORMAL;
    debug_mode = TRUE;

    यदि (!debug_did_सन्देश)
	सन्देश(_("Entering Debug mode.  Type \"cont\" to continue."));
    यदि (debug_oldval != रिक्त)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    यदि (debug_newval != रिक्त)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    यदि (sname != रिक्त)
	सन्देश((वर्ण *)sname);
    vim_free(sname);
    यदि (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    अन्यथा
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    लागि (;;)
    {
	सन्देश_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_normal_busy = ex_normal_busy;
	ex_normal_busy = 0;
	यदि (!debug_greedy)
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
	यदि (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_normal_busy = save_ex_normal_busy;

	cmdline_row = सन्देश_row;
	सन्देश_starthere();
	यदि (cmdline != रिक्त)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    यदि (*p != NUL)
	    {
		विकल्प (*p)
		{
		    पक्ष 'c': last_cmd = CMD_CONT;
			      tail = "ont";
			      रोक;
		    पक्ष 'n': last_cmd = CMD_NEXT;
			      tail = "ext";
			      रोक;
		    पक्ष 's': last_cmd = CMD_STEP;
			      tail = "tep";
			      रोक;
		    पक्ष 'f':
			      last_cmd = 0;
			      यदि (p[1] == 'r')
			      {
				  last_cmd = CMD_FRAME;
				  tail = "rame";
			      }
			      अन्यथा
			      {
				  last_cmd = CMD_FINISH;
				  tail = "inish";
			      }
			      रोक;
		    पक्ष 'q': last_cmd = CMD_QUIT;
			      tail = "uit";
			      रोक;
		    पक्ष 'i': last_cmd = CMD_INTERRUPT;
			      tail = "nterrupt";
			      रोक;
		    पक्ष 'b': last_cmd = CMD_BACKTRACE;
			      यदि (p[1] == 't')
				  tail = "t";
			      अन्यथा
				  tail = "acktrace";
			      रोक;
		    पक्ष 'w': last_cmd = CMD_BACKTRACE;
			      tail = "here";
			      रोक;
		    पक्ष 'u': last_cmd = CMD_UP;
			      tail = "p";
			      रोक;
		    पक्ष 'd': last_cmd = CMD_DOWN;
			      tail = "own";
			      रोक;
		    स्वभाव: last_cmd = 0;
		}
		यदि (last_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    जबसम्म (*p != NUL && *p == *tail)
		    {
			++p;
			++tail;
		    }
		    यदि (ASCII_ISALPHA(*p) && last_cmd != CMD_FRAME)
			last_cmd = 0;
		}
	    }

	    यदि (last_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		विकल्प (last_cmd)
		{
		    पक्ष CMD_CONT:
			debug_break_level = -1;
			रोक;
		    पक्ष CMD_NEXT:
			debug_break_level = ex_nesting_level;
			रोक;
		    पक्ष CMD_STEP:
			debug_break_level = 9999;
			रोक;
		    पक्ष CMD_FINISH:
			debug_break_level = ex_nesting_level - 1;
			रोक;
		    पक्ष CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			रोक;
		    पक्ष CMD_INTERRUPT:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			last_cmd = CMD_STEP;
			रोक;
		    पक्ष CMD_BACKTRACE:
			do_showbacktrace(cmd);
			जारी;
		    पक्ष CMD_FRAME:
			यदि (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			अन्यथा
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			जारी;
		    पक्ष CMD_UP:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			जारी;
		    पक्ष CMD_DOWN:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			जारी;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		रोक;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (शून्य)do_cmdline(cmdline, getexline, रिक्त,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	पङ्क्तिहरू_बायाँ = Rows - 1;
    }
    vim_free(cmdline);

    यदि (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    सन्देश_scroll = save_सन्देश_scroll;
    restore_timeout_for_debugging();
    पङ्क्तिहरू_बायाँ = Rows - 1;
    State = save_State;
    debug_mode = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    सन्देश_silent = save_सन्देश_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_सन्देश = TRUE;
}

    स्थिर पूर्णाङ्क
get_maxbacktrace_level(char_u *sname)
{
    वर्ण	*p, *q;
    पूर्णाङ्क		maxbacktrace = 0;

    यदि (sname == रिक्त)
	फर्काऊ 0;

    p = (वर्ण *)sname;
    जबसम्म ((q = म_माला(p, "..")) != रिक्त)
    {
	p = q + 2;
	maxbacktrace++;
    }
    फर्काऊ maxbacktrace;
}

    स्थिर शून्य
do_setdebugtracelevel(char_u *arg)
{
    पूर्णाङ्क level;

    level = म_से_प((वर्ण *)arg);
    यदि (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    अन्यथा
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    स्थिर शून्य
do_checkbacktracelevel(शून्य)
{
    यदि (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	सन्देश(_("frame is zero"));
    }
    अन्यथा
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	पूर्णाङ्क	max = get_maxbacktrace_level(sname);

	यदि (debug_backtrace_level > max)
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
    यदि (sname != रिक्त)
    {
	cur = (वर्ण *)sname;
	जबसम्म (!got_int)
	{
	    next = म_माला(cur, "..");
	    यदि (next != रिक्त)
		*next = NUL;
	    यदि (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    अन्यथा
		smsg("  %d %s", max - i, cur);
	    ++i;
	    यदि (next == रिक्त)
		रोक;
	    *next = '.';
	    cur = next + 2;
	}
	vim_free(sname);
    }

    यदि (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (दीर्घ)SOURCING_LNUM, cmd);
    अन्यथा
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
    यदि (debug_breakpoint_name != रिक्त)
    {
	यदि (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    यदि (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    अन्यथा
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (दीर्घ)debug_breakpoint_lnum);
	    debug_breakpoint_name = रिक्त;
	    do_debug(eap->cmd);
	}
	अन्यथा
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = रिक्त;
	}
    }
    अन्यथा यदि (ex_nesting_level <= debug_break_level)
    {
	यदि (!eap->skip)
	    do_debug(eap->cmd);
	अन्यथा
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

    यदि (!debug_skipped)
	फर्काऊ FALSE;

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
    फर्काऊ TRUE;
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
स्थिर पूर्णाङ्क has_अभिव्यक्ति_breakpoint = FALSE;

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
# define HIKEY2PE(p)	((profentry_T *)((p) - (दुरत्व(profentry_T, pen_name))))
# define HI2PE(hi)	HIKEY2PE((hi)->hi_साँचो)

स्थिर शून्य prof_clear_cache(शून्य);
# define PROF_CLEAR_CACHE(gap) गर {यदि ((gap) == &prof_ga) prof_clear_cache();} जबसम्म (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#अन्यथा
# define PROF_CLEAR_CACHE(gap) गर {} जबसम्म (0)
#endif
#define DBG_FUNC	1
#define DBG_FILE	2
#define DBG_अभिव्यक्ति	3

स्थिर linenr_T debuggy_find(पूर्णाङ्क file,char_u *fname, linenr_T after, garray_T *gap, पूर्णाङ्क *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    स्थिर typval_T *
eval_अभिव्यक्ति_no_emsg(संरचना debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_अभिव्यक्ति(bp->dbg_name, रिक्त);
    --emsg_off;

    फर्काऊ tv;
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

    यदि (ga_grow(gap, 1) == FAIL)
	फर्काऊ FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    यदि (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    अन्यथा यदि (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    अन्यथा यदि (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "here", 4) == 0)
    {
	यदि (curbuf->b_ffname == रिक्त)
	{
	    emsg(_(e_no_file_name));
	    फर्काऊ FAIL;
	}
	bp->dbg_type = DBG_FILE;
	here = TRUE;
    }
    अन्यथा यदि (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_अभिव्यक्ति;
    अन्यथा
    {
	semsg(_(e_invalid_argument_str), p);
	फर्काऊ FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    यदि (here)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    अन्यथा यदि (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    अन्यथा
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    यदि ((!here && *p == NUL)
	    || (here && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && म_माला((वर्ण *)p, "()") != रिक्त))
    {
	semsg(_(e_invalid_argument_str), arg);
	फर्काऊ FAIL;
    }

    यदि (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    अन्यथा यदि (here)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    अन्यथा यदि (bp->dbg_type == DBG_अभिव्यक्ति)
    {
	bp->dbg_name = vim_strsave(p);
	यदि (bp->dbg_name != रिक्त)
	    bp->dbg_val = eval_अभिव्यक्ति_no_emsg(bp);
    }
    अन्यथा
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	यदि (q == रिक्त)
	    फर्काऊ FAIL;
	p = expand_env_save(q);
	vim_free(q);
	यदि (p == रिक्त)
	    फर्काऊ FAIL;
	यदि (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	अन्यथा
	    bp->dbg_name = p;
    }

    यदि (bp->dbg_name == रिक्त)
	फर्काऊ FAIL;
    फर्काऊ OK;
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
    यदि (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#endif

    यदि (dbg_parsearg(eap->arg, gap) != OK)
	फर्काऊ;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    यदि (bp->dbg_type != DBG_अभिव्यक्ति)
    {
	pat = file_pat_to_reg_pat(bp->dbg_name, रिक्त, रिक्त, FALSE);
	यदि (pat != रिक्त)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	यदि (pat == रिक्त || bp->dbg_prog == रिक्त)
	    vim_free(bp->dbg_name);
	अन्यथा
	{
	    यदि (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#ifdef FEAT_PROFILE
	    यदि (eap->cmdidx != CMD_profile)
#endif
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++last_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    अन्यथा
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++last_breakp;
	++debug_tick;
	यदि (gap == &dbg_breakp)
	    has_अभिव्यक्ति_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    शून्य
ex_debuggreedy(exarg_T *eap)
{
    यदि (eap->addr_गन्ती == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    अन्यथा
	debug_greedy = FALSE;
}

    स्थिर शून्य
update_has_अभिव्यक्ति_breakpoint(शून्य)
{
    पूर्णाङ्क i;

    has_अभिव्यक्ति_breakpoint = FALSE;
    लागि (i = 0; i < dbg_breakp.ga_len; ++i)
	यदि (BREAKP(i).dbg_type == DBG_अभिव्यक्ति)
	{
	    has_अभिव्यक्ति_breakpoint = TRUE;
	    रोक;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    पूर्णाङ्क
debug_has_अभिव्यक्ति_breakpoint(शून्य)
{
    फर्काऊ has_अभिव्यक्ति_breakpoint;
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
    यदि (eap->cmdidx == CMD_profdel)
    {
#ifdef FEAT_PROFILE
	gap = &prof_ga;
#अन्यथा
	ex_ni(eap);
	फर्काऊ;
#endif
    }

    यदि (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = म_से_द((वर्ण *)eap->arg);
	लागि (i = 0; i < gap->ga_len; ++i)
	    यदि (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		रोक;
	    }
    }
    अन्यथा यदि (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    अन्यथा
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	यदि (dbg_parsearg(eap->arg, gap) == FAIL)
	    फर्काऊ;
	bp = &DEBUGGY(gap, gap->ga_len);
	लागि (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    यदि (bp->dbg_type == bpi->dbg_type
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

    यदि (todel < 0)
    {
	semsg(_(e_breakpoint_not_found_str), eap->arg);
	फर्काऊ;
    }

    जबसम्म (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#ifdef FEAT_EVAL
	यदि (DEBUGGY(gap, todel).dbg_type == DBG_अभिव्यक्ति
		&& DEBUGGY(gap, todel).dbg_val != रिक्त)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#endif
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	यदि (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * परिमाण(संरचना debuggy));
#ifdef FEAT_PROFILE
	यदि (eap->cmdidx == CMD_breakdel)
#endif
	    ++debug_tick;
	यदि (!del_all)
	    रोक;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    यदि (gap->ga_len == 0)
	ga_clear(gap);
    यदि (gap == &dbg_breakp)
	update_has_अभिव्यक्ति_breakpoint();
}

/*
 * ":breaklist".
 */
    शून्य
ex_breaklist(exarg_T *eap UNUSED)
{
    संरचना debuggy *bp;
    पूर्णाङ्क		i;

    यदि (dbg_breakp.ga_len == 0)
    {
	सन्देश(_("No breakpoints defined"));
	फर्काऊ;
    }

    लागि (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	यदि (bp->dbg_type == DBG_FILE)
	    home_replace(रिक्त, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	यदि (bp->dbg_type != DBG_अभिव्यक्ति)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (दीर्घ)bp->dbg_lnum);
	अन्यथा
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
    फर्काऊ debuggy_find(file, fname, after, &dbg_breakp, रिक्त);
}

#यदि defined(FEAT_PROFILE)
# यदि defined(PROF_CACHE_LOG)
स्थिर पूर्णाङ्क गन्ती_lookups[2];
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
    यदि (prof_ga.ga_len == 0 || !prof_cache_initialized)
	फर्काऊ debuggy_find(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    यदि (hashp != रिक्त)
    {
	hash = *hashp;
	यदि (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    अन्यथा
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    यदि (HASHITEM_EMPTY(hi))
    {
	pe = alloc(दुरत्व(profentry_T, pen_name) + STRLEN(fname) + 1);
	यदि (pe == रिक्त)
	    फर्काऊ FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	पूर्णाङ्क f;
	पूर्णाङ्क lnum = debuggy_find(file, fname, (linenr_T)0, &prof_ga, &f);
	यदि (lnum)
	{
	    PEN_SET_PROFILING(pe);
	    यदि (f)
		PEN_SET_FORCEIT(pe);
	}
	hash_add_item(ht, hi, pe->pen_name, hash);
# यदि defined(PROF_CACHE_LOG)
	ch_log(रिक्त, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    अन्यथा
	pe = HI2PE(hi);
    यदि (fp)
	*fp = PEN_IS_FORCEIT(pe);
# यदि defined(PROF_CACHE_LOG)
    गन्ती_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    फर्काऊ PEN_IS_PROFILING(pe);
}

    स्थिर शून्य
prof_clear_cache(शून्य)
{
    यदि (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    लागि (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	यदि (ht->ht_used > 0)
	{
# यदि defined(PROF_CACHE_LOG)
	    पूर्णाङ्क idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(रिक्त, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_used, गन्ती_lookups[idx]);
	    गन्ती_lookups[idx] = 0;
# endif
	    hash_clear_all(ht, दुरत्व(profentry_T, pen_name));
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
    यदि (gap->ga_len == 0)
	फर्काऊ (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    यदि (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	यदि (name != रिक्त)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    लागि (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	यदि (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_अभिव्यक्ति && (
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
	    यदि ((name != रिक्त
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		यदि (fp != रिक्त)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#ifdef FEAT_PROFILE
	    यदि (lnum && gap == &prof_ga)
		रोक;
#endif
	}
#ifdef FEAT_EVAL
	अन्यथा यदि (bp->dbg_type == DBG_अभिव्यक्ति)
	{
	    typval_T *tv;
	    पूर्णाङ्क	      पङ्क्ति = FALSE;

	    tv = eval_अभिव्यक्ति_no_emsg(bp);
	    यदि (tv != रिक्त)
	    {
		यदि (bp->dbg_val == रिक्त)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(रिक्त, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    पङ्क्ति = TRUE;
		}
		अन्यथा
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? अभिव्यक्ति_EQUAL : अभिव्यक्ति_IS;

		    यदि (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_सङ्ख्या == FALSE)
		    {
			typval_T *v;

			पङ्क्ति = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_अभिव्यक्ति_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    अन्यथा यदि (bp->dbg_val != रिक्त)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(रिक्त, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = रिक्त;
		पङ्क्ति = TRUE;
	    }

	    यदि (पङ्क्ति)
	    {
		lnum = after > 0 ? after : 1;
		रोक;
	    }
	}
#endif
    }
    यदि (name != fname)
	vim_free(name);

    फर्काऊ lnum;
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
