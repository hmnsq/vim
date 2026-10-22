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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
ସ୍ଥିର ଶୂନ୍ୟ do_setdebugtracelevel(char_u *arg);
ସ୍ଥିର ଶୂନ୍ୟ do_checkbacktracelevel(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ do_showbacktrace(char_u *cmd);

ସ୍ଥିର char_u *debug_oldval = ଖାଲି;	// old and newval for debug expressions
ସ୍ଥିର char_u *debug_newval = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା     debug_ପଦସମଷ୍ଟି   = 0;	// use debug_expr

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_watchexpr(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ debug_ପଦସମଷ୍ଟି;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    ଶୂନ୍ୟ
do_debug(char_u *cmd)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ସନ୍ଦେଶ_scroll = ସନ୍ଦେଶ_scroll;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_State = State;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_did_emsg = did_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cmd_silent = cmd_silent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ସନ୍ଦେଶ_silent = ସନ୍ଦେଶ_silent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_emsg_silent = emsg_silent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		typeahead_saved = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ignore_script = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ex_normal_busy;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    char_u	*cmdline = ଖାଲି;
    char_u	*p;
    char_u	*sname;
    ଅକ୍ଷର	*tail = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	last_cmd = 0;
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
    ଯଦି (!gui.in_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	ଫେରନ୍ତୁ;
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
    ସନ୍ଦେଶ_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = MODE_NORMAL;
    debug_mode = TRUE;

    ଯଦି (!debug_did_ସନ୍ଦେଶ)
	ସନ୍ଦେଶ(_("Entering Debug mode.  Type \"cont\" to continue."));
    ଯଦି (debug_oldval != ଖାଲି)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    ଯଦି (debug_newval != ଖାଲି)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    ଯଦି (sname != ଖାଲି)
	ସନ୍ଦେଶ((ଅକ୍ଷର *)sname);
    vim_free(sname);
    ଯଦି (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    ଅନ୍ୟଥା
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    ପାଇଁ (;;)
    {
	ସନ୍ଦେଶ_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_normal_busy = ex_normal_busy;
	ex_normal_busy = 0;
	ଯଦି (!debug_greedy)
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
	cmdline = getcmdline_prompt('>', ଖାଲି, 0, EXPAND_NOTHING, ଖାଲି);

	debug_break_level = n;
	ଯଦି (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_normal_busy = save_ex_normal_busy;

	cmdline_row = ସନ୍ଦେଶ_row;
	ସନ୍ଦେଶ_starthere();
	ଯଦି (cmdline != ଖାଲି)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    ଯଦି (*p != NUL)
	    {
		ବାଛ (*p)
		{
		    ସ୍ଥିତି 'c': last_cmd = CMD_CONT;
			      tail = "ont";
			      ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'n': last_cmd = CMD_NEXT;
			      tail = "ext";
			      ଭାଙ୍ଗ;
		    ସ୍ଥିତି 's': last_cmd = CMD_STEP;
			      tail = "tep";
			      ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'f':
			      last_cmd = 0;
			      ଯଦି (p[1] == 'r')
			      {
				  last_cmd = CMD_FRAME;
				  tail = "rame";
			      }
			      ଅନ୍ୟଥା
			      {
				  last_cmd = CMD_FINISH;
				  tail = "inish";
			      }
			      ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'q': last_cmd = CMD_QUIT;
			      tail = "uit";
			      ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'i': last_cmd = CMD_INTERRUPT;
			      tail = "nterrupt";
			      ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'b': last_cmd = CMD_BACKTRACE;
			      ଯଦି (p[1] == 't')
				  tail = "t";
			      ଅନ୍ୟଥା
				  tail = "acktrace";
			      ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'w': last_cmd = CMD_BACKTRACE;
			      tail = "here";
			      ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'u': last_cmd = CMD_UP;
			      tail = "p";
			      ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'd': last_cmd = CMD_DOWN;
			      tail = "own";
			      ଭାଙ୍ଗ;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: last_cmd = 0;
		}
		ଯଦି (last_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    ଯେତେବେଳେ (*p != NUL && *p == *tail)
		    {
			++p;
			++tail;
		    }
		    ଯଦି (ASCII_ISALPHA(*p) && last_cmd != CMD_FRAME)
			last_cmd = 0;
		}
	    }

	    ଯଦି (last_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		ବାଛ (last_cmd)
		{
		    ସ୍ଥିତି CMD_CONT:
			debug_break_level = -1;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି CMD_NEXT:
			debug_break_level = ex_nesting_level;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି CMD_STEP:
			debug_break_level = 9999;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି CMD_FINISH:
			debug_break_level = ex_nesting_level - 1;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି CMD_INTERRUPT:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			last_cmd = CMD_STEP;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି CMD_BACKTRACE:
			do_showbacktrace(cmd);
			ଜାରି;
		    ସ୍ଥିତି CMD_FRAME:
			ଯଦି (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			ଅନ୍ୟଥା
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			ଜାରି;
		    ସ୍ଥିତି CMD_UP:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			ଜାରି;
		    ସ୍ଥିତି CMD_DOWN:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			ଜାରି;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		ଭାଙ୍ଗ;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (ଶୂନ୍ୟ)do_cmdline(cmdline, getexline, ଖାଲି,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = Rows - 1;
    }
    vim_free(cmdline);

    ଯଦି (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    ସନ୍ଦେଶ_scroll = save_ସନ୍ଦେଶ_scroll;
    restore_timeout_for_debugging();
    ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = Rows - 1;
    State = save_State;
    debug_mode = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    ସନ୍ଦେଶ_silent = save_ସନ୍ଦେଶ_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_ସନ୍ଦେଶ = TRUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_maxbacktrace_level(char_u *sname)
{
    ଅକ୍ଷର	*p, *q;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxbacktrace = 0;

    ଯଦି (sname == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    p = (ଅକ୍ଷର *)sname;
    ଯେତେବେଳେ ((q = strstr(p, "..")) != ଖାଲି)
    {
	p = q + 2;
	maxbacktrace++;
    }
    ଫେରନ୍ତୁ maxbacktrace;
}

    ସ୍ଥିର ଶୂନ୍ୟ
do_setdebugtracelevel(char_u *arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା level;

    level = atoi((ଅକ୍ଷର *)arg);
    ଯଦି (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    ଅନ୍ୟଥା
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    ସ୍ଥିର ଶୂନ୍ୟ
do_checkbacktracelevel(ଶୂନ୍ୟ)
{
    ଯଦି (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	ସନ୍ଦେଶ(_("frame is zero"));
    }
    ଅନ୍ୟଥା
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	max = get_maxbacktrace_level(sname);

	ଯଦି (debug_backtrace_level > max)
	{
	    debug_backtrace_level = max;
	    smsg(_("frame at highest level: %d"), max);
	}
	vim_free(sname);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    ଅକ୍ଷର    *cur;
    ଅକ୍ଷର    *next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    ଯଦି (sname != ଖାଲି)
    {
	cur = (ଅକ୍ଷର *)sname;
	ଯେତେବେଳେ (!got_int)
	{
	    next = strstr(cur, "..");
	    ଯଦି (next != ଖାଲି)
		*next = NUL;
	    ଯଦି (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    ଅନ୍ୟଥା
		smsg("  %d %s", max - i, cur);
	    ++i;
	    ଯଦି (next == ଖାଲି)
		ଭାଙ୍ଗ;
	    *next = '.';
	    cur = next + 2;
	}
	vim_free(sname);
    }

    ଯଦି (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (long)SOURCING_LNUM, cmd);
    ଅନ୍ୟଥା
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    ଶୂନ୍ୟ
ex_debug(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

ସ୍ଥିର char_u	*debug_breakpoint_name = ଖାଲି;
ସ୍ଥିର linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	debug_skipped;
ସ୍ଥିର char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    ଶୂନ୍ୟ
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    ଯଦି (debug_breakpoint_name != ଖାଲି)
    {
	ଯଦି (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    ଯଦି (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    ଅନ୍ୟଥା
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (long)debug_breakpoint_lnum);
	    debug_breakpoint_name = ଖାଲି;
	    do_debug(eap->cmd);
	}
	ଅନ୍ୟଥା
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = ଖାଲି;
	}
    }
    ଅନ୍ୟଥା ଯଦି (ex_nesting_level <= debug_break_level)
    {
	ଯଦି (!eap->skip)
	    do_debug(eap->cmd);
	ଅନ୍ୟଥା
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = ଖାଲି;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dbg_check_skipped(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_got_int;

    ଯଦି (!debug_skipped)
	ଫେରନ୍ତୁ FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    prev_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (ଶୂନ୍ୟ)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= prev_got_int;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
ସଂରଚନା debuggy
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dbg_nr;		// breakpoint number
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dbg_forceit;	// ! used
#ifdef FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dbg_level;      // stored nested level for expr
};

ସ୍ଥିର garray_T dbg_breakp = {0, 0, ଆକାର(ସଂରଚନା debuggy), 4, ଖାଲି};
#define BREAKP(idx)		(((ସଂରଚନା debuggy *)dbg_breakp.ga_data)[idx])
#define DEBUGGY(gap, idx)	(((ସଂରଚନା debuggy *)gap->ga_data)[idx])
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_breakp = 0;	// nr of last defined breakpoint
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_ପଦସମଷ୍ଟି_breakpoint = FALSE;

#ifdef FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
ସ୍ଥିର garray_T prof_ga = {0, 0, ଆକାର(ସଂରଚନା debuggy), 4, ଖାଲି};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
ସ୍ଥିର hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prof_cache_initialized;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା profentry_S
{
    ଅକ୍ଷର    pen_flags;	// cache data booleans: profiling, forceit
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
# define HI2PE(hi)	HIKEY2PE((hi)->hi_ଚାବି)

ସ୍ଥିର ଶୂନ୍ୟ prof_clear_cache(ଶୂନ୍ୟ);
# define PROF_CLEAR_CACHE(gap) କର {ଯଦି ((gap) == &prof_ga) prof_clear_cache();} ଯେତେବେଳେ (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#ଅନ୍ୟଥା
# define PROF_CLEAR_CACHE(gap) କର {} ଯେତେବେଳେ (0)
#endif
#define DBG_FUNC	1
#define DBG_FILE	2
#define DBG_ପଦସମଷ୍ଟି	3

ସ୍ଥିର linenr_T debuggy_find(ପୂର୍ଣ୍ଣସଂଖ୍ୟା file,char_u *fname, linenr_T after, garray_T *gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    ସ୍ଥିର typval_T *
eval_ପଦସମଷ୍ଟି_no_emsg(ସଂରଚନା debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_ପଦସମଷ୍ଟି(bp->dbg_name, ଖାଲି);
    --emsg_off;

    ଫେରନ୍ତୁ tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    ସଂରଚନା debuggy *bp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		here = FALSE;

    ଯଦି (ga_grow(gap, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    ଯଦି (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    ଅନ୍ୟଥା ଯଦି (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "here", 4) == 0)
    {
	ଯଦି (curbuf->b_ffname == ଖାଲି)
	{
	    emsg(_(e_no_file_name));
	    ଫେରନ୍ତୁ FAIL;
	}
	bp->dbg_type = DBG_FILE;
	here = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_ପଦସମଷ୍ଟି;
    ଅନ୍ୟଥା
    {
	semsg(_(e_invalid_argument_str), p);
	ଫେରନ୍ତୁ FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    ଯଦି (here)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    ଅନ୍ୟଥା ଯଦି (
#ifdef FEAT_PROFILE
	    gap != &prof_ga &&
#endif
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    ଅନ୍ୟଥା
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    ଯଦି ((!here && *p == NUL)
	    || (here && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && strstr((ଅକ୍ଷର *)p, "()") != ଖାଲି))
    {
	semsg(_(e_invalid_argument_str), arg);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    ଅନ୍ୟଥା ଯଦି (here)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    ଅନ୍ୟଥା ଯଦି (bp->dbg_type == DBG_ପଦସମଷ୍ଟି)
    {
	bp->dbg_name = vim_strsave(p);
	ଯଦି (bp->dbg_name != ଖାଲି)
	    bp->dbg_val = eval_ପଦସମଷ୍ଟି_no_emsg(bp);
    }
    ଅନ୍ୟଥା
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	ଯଦି (q == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	p = expand_env_save(q);
	vim_free(q);
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	ଅନ୍ୟଥା
	    bp->dbg_name = p;
    }

    ଯଦି (bp->dbg_name == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    ଶୂନ୍ୟ
ex_breakadd(exarg_T *eap)
{
    ସଂରଚନା debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#ifdef FEAT_PROFILE
    ଯଦି (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#endif

    ଯଦି (dbg_parsearg(eap->arg, gap) != OK)
	ଫେରନ୍ତୁ;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    ଯଦି (bp->dbg_type != DBG_ପଦସମଷ୍ଟି)
    {
	pat = file_pat_to_reg_pat(bp->dbg_name, ଖାଲି, ଖାଲି, FALSE);
	ଯଦି (pat != ଖାଲି)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	ଯଦି (pat == ଖାଲି || bp->dbg_prog == ଖାଲି)
	    vim_free(bp->dbg_name);
	ଅନ୍ୟଥା
	{
	    ଯଦି (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#ifdef FEAT_PROFILE
	    ଯଦି (eap->cmdidx != CMD_profile)
#endif
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++last_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    ଅନ୍ୟଥା
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++last_breakp;
	++debug_tick;
	ଯଦି (gap == &dbg_breakp)
	    has_ପଦସମଷ୍ଟି_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    ଶୂନ୍ୟ
ex_debuggreedy(exarg_T *eap)
{
    ଯଦି (eap->addr_ଗଣନା == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    ଅନ୍ୟଥା
	debug_greedy = FALSE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
update_has_ପଦସମଷ୍ଟି_breakpoint(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    has_ପଦସମଷ୍ଟି_breakpoint = FALSE;
    ପାଇଁ (i = 0; i < dbg_breakp.ga_len; ++i)
	ଯଦି (BREAKP(i).dbg_type == DBG_ପଦସମଷ୍ଟି)
	{
	    has_ପଦସମଷ୍ଟି_breakpoint = TRUE;
	    ଭାଙ୍ଗ;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
debug_has_ପଦସମଷ୍ଟି_breakpoint(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ has_ପଦସମଷ୍ଟି_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    ଶୂନ୍ୟ
ex_breakdel(exarg_T *eap)
{
    ସଂରଚନା debuggy *bp, *bpi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todel = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		del_all = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    linenr_T	best_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    ଯଦି (eap->cmdidx == CMD_profdel)
    {
#ifdef FEAT_PROFILE
	gap = &prof_ga;
#ଅନ୍ୟଥା
	ex_ni(eap);
	ଫେରନ୍ତୁ;
#endif
    }

    ଯଦି (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((ଅକ୍ଷର *)eap->arg);
	ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	    ଯଦି (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		ଭାଙ୍ଗ;
	    }
    }
    ଅନ୍ୟଥା ଯଦି (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    ଅନ୍ୟଥା
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	ଯଦି (dbg_parsearg(eap->arg, gap) == FAIL)
	    ଫେରନ୍ତୁ;
	bp = &DEBUGGY(gap, gap->ga_len);
	ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    ଯଦି (bp->dbg_type == bpi->dbg_type
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

    ଯଦି (todel < 0)
    {
	semsg(_(e_breakpoint_not_found_str), eap->arg);
	ଫେରନ୍ତୁ;
    }

    ଯେତେବେଳେ (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#ifdef FEAT_EVAL
	ଯଦି (DEBUGGY(gap, todel).dbg_type == DBG_ପଦସମଷ୍ଟି
		&& DEBUGGY(gap, todel).dbg_val != ଖାଲି)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#endif
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	ଯଦି (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * ଆକାର(ସଂରଚନା debuggy));
#ifdef FEAT_PROFILE
	ଯଦି (eap->cmdidx == CMD_breakdel)
#endif
	    ++debug_tick;
	ଯଦି (!del_all)
	    ଭାଙ୍ଗ;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    ଯଦି (gap->ga_len == 0)
	ga_clear(gap);
    ଯଦି (gap == &dbg_breakp)
	update_has_ପଦସମଷ୍ଟି_breakpoint();
}

/*
 * ":breaklist".
 */
    ଶୂନ୍ୟ
ex_breaklist(exarg_T *eap UNUSED)
{
    ସଂରଚନା debuggy *bp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (dbg_breakp.ga_len == 0)
    {
	ସନ୍ଦେଶ(_("No breakpoints defined"));
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	ଯଦି (bp->dbg_type == DBG_FILE)
	    home_replace(ଖାଲି, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	ଯଦି (bp->dbg_type != DBG_ପଦସମଷ୍ଟି)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (long)bp->dbg_lnum);
	ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    ଫେରନ୍ତୁ debuggy_find(file, fname, after, &dbg_breakp, ଖାଲି);
}

#ଯଦି defined(FEAT_PROFILE)
# ଯଦି defined(PROF_CACHE_LOG)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_profiling(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    ଯଦି (prof_ga.ga_len == 0 || !prof_cache_initialized)
	ଫେରନ୍ତୁ debuggy_find(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    ଯଦି (hashp != ଖାଲି)
    {
	hash = *hashp;
	ଯଦି (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    ଅନ୍ୟଥା
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    ଯଦି (HASHITEM_EMPTY(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	ଯଦି (pe == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା f;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା lnum = debuggy_find(file, fname, (linenr_T)0, &prof_ga, &f);
	ଯଦି (lnum)
	{
	    PEN_SET_PROFILING(pe);
	    ଯଦି (f)
		PEN_SET_FORCEIT(pe);
	}
	hash_add_item(ht, hi, pe->pen_name, hash);
# ଯଦି defined(PROF_CACHE_LOG)
	ch_log(ଖାଲି, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    ଅନ୍ୟଥା
	pe = HI2PE(hi);
    ଯଦି (fp)
	*fp = PEN_IS_FORCEIT(pe);
# ଯଦି defined(PROF_CACHE_LOG)
    ଗଣନା_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    ଫେରନ୍ତୁ PEN_IS_PROFILING(pe);
}

    ସ୍ଥିର ଶୂନ୍ୟ
prof_clear_cache(ଶୂନ୍ୟ)
{
    ଯଦି (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    ପାଇଁ (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	ଯଦି (ht->ht_used > 0)
	{
# ଯଦି defined(PROF_CACHE_LOG)
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(ଖାଲି, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_used, ଗଣନା_lookups[idx]);
	    ଗଣନା_lookups[idx] = 0;
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
    ସ୍ଥିର linenr_T
debuggy_find(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*fp)	    // if not NULL: return forceit
{
    ସଂରଚନା debuggy *bp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    linenr_T	lnum = 0;
    char_u	*name = ଖାଲି;
    char_u	*short_name = fname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_got_int;

    // Return quickly when there are no breakpoints.
    ଯଦି (gap->ga_len == 0)
	ଫେରନ୍ତୁ (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    ଯଦି (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	ଯଦି (name != ଖାଲି)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	ଯଦି (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_ପଦସମଷ୍ଟି && (
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
	    ଯଦି ((name != ଖାଲି
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		ଯଦି (fp != ଖାଲି)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#ifdef FEAT_PROFILE
	    ଯଦି (lnum && gap == &prof_ga)
		ଭାଙ୍ଗ;
#endif
	}
#ifdef FEAT_EVAL
	ଅନ୍ୟଥା ଯଦି (bp->dbg_type == DBG_ପଦସମଷ୍ଟି)
	{
	    typval_T *tv;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	      ଧାଡ଼ି = FALSE;

	    tv = eval_ପଦସମଷ୍ଟି_no_emsg(bp);
	    ଯଦି (tv != ଖାଲି)
	    {
		ଯଦି (bp->dbg_val == ଖାଲି)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(ଖାଲି, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    ଧାଡ଼ି = TRUE;
		}
		ଅନ୍ୟଥା
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? ପଦସମଷ୍ଟି_EQUAL : ପଦସମଷ୍ଟି_IS;

		    ଯଦି (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_ସଂଖ୍ୟା == FALSE)
		    {
			typval_T *v;

			ଧାଡ଼ି = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_ପଦସମଷ୍ଟି_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (bp->dbg_val != ଖାଲି)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(ଖାଲି, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = ଖାଲି;
		ଧାଡ଼ି = TRUE;
	    }

	    ଯଦି (ଧାଡ଼ି)
	    {
		lnum = after > 0 ? after : 1;
		ଭାଙ୍ଗ;
	    }
	}
#endif
    }
    ଯଦି (name != fname)
	vim_free(name);

    ଫେରନ୍ତୁ lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    ଶୂନ୍ୟ
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#endif
