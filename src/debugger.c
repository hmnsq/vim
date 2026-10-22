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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)
ਸਥਿਰ ਪੂਰਨ debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
ਸਥਿਰ ਖਾਲੀ do_setdebugtracelevel(char_u *arg);
ਸਥਿਰ ਖਾਲੀ do_checkbacktracelevel(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ do_showbacktrace(char_u *cmd);

ਸਥਿਰ char_u *debug_oldval = ਨਿਰਾ;	// old and newval for debug expressions
ਸਥਿਰ char_u *debug_newval = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ     debug_ਵਿਅੰਜਕ   = 0;	// use debug_expr

    ਪੂਰਨ
has_watchexpr(ਖਾਲੀ)
{
    ਵਾਪਸ debug_ਵਿਅੰਜਕ;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    ਖਾਲੀ
do_debug(char_u *cmd)
{
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll;
    ਪੂਰਨ		save_State = State;
    ਪੂਰਨ		save_did_emsg = did_emsg;
    ਪੂਰਨ		save_cmd_silent = cmd_silent;
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent;
    ਪੂਰਨ		save_emsg_silent = emsg_silent;
    ਪੂਰਨ		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    ਪੂਰਨ		typeahead_saved = FALSE;
    ਪੂਰਨ		save_ignore_script = 0;
    ਪੂਰਨ		save_ex_ਸਾਧਾਰਨ_busy;
    ਪੂਰਨ		n;
    char_u	*cmdline = ਨਿਰਾ;
    char_u	*p;
    char_u	*sname;
    ਅੱਖਰ	*ਪੂਛ = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ	ਆਖਰੀ_cmd = 0;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_CONT	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_ਅਗਲਾ	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_ਕਦਮ	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_ਅੰਤ	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_QUIT	5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_ਰੁਕਾਵਟ	6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_BACKTRACE	7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_ਚੌਖਟਾ	8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_ਉੱਪਰ		9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_ਥੱਲੇ	10

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ALWAYS_USE_GUI
    // Can't do this when there is no terminal for input/output.
    ਜੇ (!gui.ਵਿੱਚ_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    // Make sure we are in raw mode and start termcap mode.  Might have side
    // effects...
    settmode(TMODE_RAW);
    starttermcap();

    ++RedrawingDisabled;	// don't redisplay the window
    ++no_wait_return;		// don't wait for return
    did_emsg = FALSE;		// don't use error from debugged stuff
    cmd_silent = FALSE;		// display commands
    ਸੁਨੇਹਾ_silent = FALSE;		// display messages
    emsg_silent = FALSE;	// display error messages
    redir_off = TRUE;		// don't redirect debug commands
    save_timeout_for_debugging();   // disable  regexp timeout flag

    State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;
    debug_ਬਹੁਲਕ = TRUE;

    ਜੇ (!debug_did_ਸੁਨੇਹਾ)
	ਸੁਨੇਹਾ(_("Entering Debug mode.  Type \"cont\" to continue."));
    ਜੇ (debug_oldval != ਨਿਰਾ)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    ਜੇ (debug_newval != ਨਿਰਾ)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    ਜੇ (sname != ਨਿਰਾ)
	ਸੁਨੇਹਾ((ਅੱਖਰ *)sname);
    vim_free(sname);
    ਜੇ (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    ਨਹੀਂਤਾਂ
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    ਲਈ (;;)
    {
	ਸੁਨੇਹਾ_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_ਸਾਧਾਰਨ_busy = ex_ਸਾਧਾਰਨ_busy;
	ex_ਸਾਧਾਰਨ_busy = 0;
	ਜੇ (!debug_greedy)
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
	cmdline = getcmdline_prompt('>', ਨਿਰਾ, 0, EXPAND_NOTHING, ਨਿਰਾ);

	debug_break_level = n;
	ਜੇ (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_ਸਾਧਾਰਨ_busy = save_ex_ਸਾਧਾਰਨ_busy;

	cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	ਸੁਨੇਹਾ_starthere();
	ਜੇ (cmdline != ਨਿਰਾ)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    ਜੇ (*p != NUL)
	    {
		ਚੋਣ (*p)
		{
		    ਹਾਲਤ 'c': ਆਖਰੀ_cmd = CMD_CONT;
			      ਪੂਛ = "ont";
			      ਰੋਕੋ;
		    ਹਾਲਤ 'n': ਆਖਰੀ_cmd = CMD_ਅਗਲਾ;
			      ਪੂਛ = "ext";
			      ਰੋਕੋ;
		    ਹਾਲਤ 's': ਆਖਰੀ_cmd = CMD_ਕਦਮ;
			      ਪੂਛ = "tep";
			      ਰੋਕੋ;
		    ਹਾਲਤ 'f':
			      ਆਖਰੀ_cmd = 0;
			      ਜੇ (p[1] == 'r')
			      {
				  ਆਖਰੀ_cmd = CMD_ਚੌਖਟਾ;
				  ਪੂਛ = "rame";
			      }
			      ਨਹੀਂਤਾਂ
			      {
				  ਆਖਰੀ_cmd = CMD_ਅੰਤ;
				  ਪੂਛ = "inish";
			      }
			      ਰੋਕੋ;
		    ਹਾਲਤ 'q': ਆਖਰੀ_cmd = CMD_QUIT;
			      ਪੂਛ = "uit";
			      ਰੋਕੋ;
		    ਹਾਲਤ 'i': ਆਖਰੀ_cmd = CMD_ਰੁਕਾਵਟ;
			      ਪੂਛ = "nterrupt";
			      ਰੋਕੋ;
		    ਹਾਲਤ 'b': ਆਖਰੀ_cmd = CMD_BACKTRACE;
			      ਜੇ (p[1] == 't')
				  ਪੂਛ = "t";
			      ਨਹੀਂਤਾਂ
				  ਪੂਛ = "acktrace";
			      ਰੋਕੋ;
		    ਹਾਲਤ 'w': ਆਖਰੀ_cmd = CMD_BACKTRACE;
			      ਪੂਛ = "here";
			      ਰੋਕੋ;
		    ਹਾਲਤ 'u': ਆਖਰੀ_cmd = CMD_ਉੱਪਰ;
			      ਪੂਛ = "p";
			      ਰੋਕੋ;
		    ਹਾਲਤ 'd': ਆਖਰੀ_cmd = CMD_ਥੱਲੇ;
			      ਪੂਛ = "own";
			      ਰੋਕੋ;
		    ਮੂਲ: ਆਖਰੀ_cmd = 0;
		}
		ਜੇ (ਆਖਰੀ_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    ਜਦੋਂ (*p != NUL && *p == *ਪੂਛ)
		    {
			++p;
			++ਪੂਛ;
		    }
		    ਜੇ (ASCII_ISALPHA(*p) && ਆਖਰੀ_cmd != CMD_ਚੌਖਟਾ)
			ਆਖਰੀ_cmd = 0;
		}
	    }

	    ਜੇ (ਆਖਰੀ_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		ਚੋਣ (ਆਖਰੀ_cmd)
		{
		    ਹਾਲਤ CMD_CONT:
			debug_break_level = -1;
			ਰੋਕੋ;
		    ਹਾਲਤ CMD_ਅਗਲਾ:
			debug_break_level = ex_nesting_level;
			ਰੋਕੋ;
		    ਹਾਲਤ CMD_ਕਦਮ:
			debug_break_level = 9999;
			ਰੋਕੋ;
		    ਹਾਲਤ CMD_ਅੰਤ:
			debug_break_level = ex_nesting_level - 1;
			ਰੋਕੋ;
		    ਹਾਲਤ CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			ਰੋਕੋ;
		    ਹਾਲਤ CMD_ਰੁਕਾਵਟ:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			ਆਖਰੀ_cmd = CMD_ਕਦਮ;
			ਰੋਕੋ;
		    ਹਾਲਤ CMD_BACKTRACE:
			do_showbacktrace(cmd);
			ਜਾਰੀ;
		    ਹਾਲਤ CMD_ਚੌਖਟਾ:
			ਜੇ (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			ਨਹੀਂਤਾਂ
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			ਜਾਰੀ;
		    ਹਾਲਤ CMD_ਉੱਪਰ:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			ਜਾਰੀ;
		    ਹਾਲਤ CMD_ਥੱਲੇ:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			ਜਾਰੀ;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		ਰੋਕੋ;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (ਖਾਲੀ)do_cmdline(cmdline, getexline, ਨਿਰਾ,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	ਸਤਰਾਂ_ਖੱਬਾ = ਪੰਕਤੀਆਂ - 1;
    }
    vim_free(cmdline);

    ਜੇ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    ਸੁਨੇਹਾ_scroll = save_ਸੁਨੇਹਾ_scroll;
    restore_timeout_for_debugging();
    ਸਤਰਾਂ_ਖੱਬਾ = ਪੰਕਤੀਆਂ - 1;
    State = save_State;
    debug_ਬਹੁਲਕ = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    ਸੁਨੇਹਾ_silent = save_ਸੁਨੇਹਾ_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_ਸੁਨੇਹਾ = TRUE;
}

    ਸਥਿਰ ਪੂਰਨ
get_maxbacktrace_level(char_u *sname)
{
    ਅੱਖਰ	*p, *q;
    ਪੂਰਨ		maxbacktrace = 0;

    ਜੇ (sname == ਨਿਰਾ)
	ਵਾਪਸ 0;

    p = (ਅੱਖਰ *)sname;
    ਜਦੋਂ ((q = ਲੜੀ_ਖੋਜ(p, "..")) != ਨਿਰਾ)
    {
	p = q + 2;
	maxbacktrace++;
    }
    ਵਾਪਸ maxbacktrace;
}

    ਸਥਿਰ ਖਾਲੀ
do_setdebugtracelevel(char_u *arg)
{
    ਪੂਰਨ level;

    level = atoi((ਅੱਖਰ *)arg);
    ਜੇ (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    ਨਹੀਂਤਾਂ
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    ਸਥਿਰ ਖਾਲੀ
do_checkbacktracelevel(ਖਾਲੀ)
{
    ਜੇ (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	ਸੁਨੇਹਾ(_("frame is zero"));
    }
    ਨਹੀਂਤਾਂ
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	ਪੂਰਨ	max = get_maxbacktrace_level(sname);

	ਜੇ (debug_backtrace_level > max)
	{
	    debug_backtrace_level = max;
	    smsg(_("frame at highest level: %d"), max);
	}
	vim_free(sname);
    }
}

    ਸਥਿਰ ਖਾਲੀ
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    ਅੱਖਰ    *cur;
    ਅੱਖਰ    *ਅਗਲਾ;
    ਪੂਰਨ	    i = 0;
    ਪੂਰਨ	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    ਜੇ (sname != ਨਿਰਾ)
    {
	cur = (ਅੱਖਰ *)sname;
	ਜਦੋਂ (!got_int)
	{
	    ਅਗਲਾ = ਲੜੀ_ਖੋਜ(cur, "..");
	    ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
		*ਅਗਲਾ = NUL;
	    ਜੇ (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    ਨਹੀਂਤਾਂ
		smsg("  %d %s", max - i, cur);
	    ++i;
	    ਜੇ (ਅਗਲਾ == ਨਿਰਾ)
		ਰੋਕੋ;
	    *ਅਗਲਾ = '.';
	    cur = ਅਗਲਾ + 2;
	}
	vim_free(sname);
    }

    ਜੇ (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (ਲੰਮਾ)SOURCING_LNUM, cmd);
    ਨਹੀਂਤਾਂ
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    ਖਾਲੀ
ex_debug(exarg_T *eap)
{
    ਪੂਰਨ		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

ਸਥਿਰ char_u	*debug_breakpoint_name = ਨਿਰਾ;
ਸਥਿਰ linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
ਸਥਿਰ ਪੂਰਨ	debug_skipped;
ਸਥਿਰ char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    ਖਾਲੀ
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    ਜੇ (debug_breakpoint_name != ਨਿਰਾ)
    {
	ਜੇ (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    ਜੇ (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    ਨਹੀਂਤਾਂ
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (ਲੰਮਾ)debug_breakpoint_lnum);
	    debug_breakpoint_name = ਨਿਰਾ;
	    do_debug(eap->cmd);
	}
	ਨਹੀਂਤਾਂ
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ex_nesting_level <= debug_break_level)
    {
	ਜੇ (!eap->skip)
	    do_debug(eap->cmd);
	ਨਹੀਂਤਾਂ
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = ਨਿਰਾ;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    ਪੂਰਨ
dbg_check_skipped(exarg_T *eap)
{
    ਪੂਰਨ		prev_got_int;

    ਜੇ (!debug_skipped)
	ਵਾਪਸ FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    prev_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (ਖਾਲੀ)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= prev_got_int;
    ਵਾਪਸ TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
ਬਣਤਰ debuggy
{
    ਪੂਰਨ		dbg_nr;		// breakpoint number
    ਪੂਰਨ		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    ਪੂਰਨ		dbg_forceit;	// ! used
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		dbg_level;      // stored nested level for expr
};

ਸਥਿਰ garray_T dbg_breakp = {0, 0, ਆਕਾਰ(ਬਣਤਰ debuggy), 4, ਨਿਰਾ};
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKP(idx)		(((ਬਣਤਰ debuggy *)dbg_breakp.ga_ਅੰਕੜੇ)[idx])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUGGY(gap, idx)	(((ਬਣਤਰ debuggy *)gap->ga_ਅੰਕੜੇ)[idx])
ਸਥਿਰ ਪੂਰਨ ਆਖਰੀ_breakp = 0;	// nr of last defined breakpoint
ਸਥਿਰ ਪੂਰਨ has_ਵਿਅੰਜਕ_breakpoint = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
ਸਥਿਰ garray_T prof_ga = {0, 0, ਆਕਾਰ(ਬਣਤਰ debuggy), 4, ਨਿਰਾ};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
ਸਥਿਰ hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
ਸਥਿਰ ਪੂਰਨ prof_cache_initialized;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ profentry_S
{
    ਅੱਖਰ    pen_flags;	// cache data booleans: profiling, forceit
    char_u  pen_name[1]; // actually longer
} profentry_T;
# define PEN_FLAG_PROFILING 1
# define PEN_FLAG_FORCEIT 2
# define PEN_ਸੈੱਟ_PROFILING(pe) ((pe)->pen_flags |= PEN_FLAG_PROFILING)
# define PEN_ਸੈੱਟ_FORCEIT(pe) ((pe)->pen_flags |= PEN_FLAG_FORCEIT)
# define PEN_IS_PROFILING(pe) (((pe)->pen_flags & PEN_FLAG_PROFILING) != 0)
# define PEN_IS_FORCEIT(pe) (((pe)->pen_flags & PEN_FLAG_FORCEIT) != 0)

# define PE2HIKEY(pe)	((pe)->pen_name)
# define HIKEY2PE(p)	((profentry_T *)((p) - (offsetof(profentry_T, pen_name))))
# define HI2PE(hi)	HIKEY2PE((hi)->hi_ਕੁੰਜੀ)

ਸਥਿਰ ਖਾਲੀ prof_clear_cache(ਖਾਲੀ);
# define PROF_CLEAR_CACHE(gap) ਕਰੋ {ਜੇ ((gap) == &prof_ga) prof_clear_cache();} ਜਦੋਂ (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#ਨਹੀਂਤਾਂ
# define PROF_CLEAR_CACHE(gap) ਕਰੋ {} ਜਦੋਂ (0)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBG_FUNC	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBG_FILE	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBG_ਵਿਅੰਜਕ	3

ਸਥਿਰ linenr_T debuggy_ਲੱਭੋ(ਪੂਰਨ file,char_u *fname, linenr_T after, garray_T *gap, ਪੂਰਨ *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    ਸਥਿਰ typval_T *
eval_ਵਿਅੰਜਕ_no_emsg(ਬਣਤਰ debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_ਵਿਅੰਜਕ(bp->dbg_name, ਨਿਰਾ);
    --emsg_off;

    ਵਾਪਸ tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    ਸਥਿਰ ਪੂਰਨ
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    ਬਣਤਰ debuggy *bp;
    ਪੂਰਨ		ਇੱਥੇ = FALSE;

    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    ਜੇ (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    ਨਹੀਂਤਾਂ ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	    gap != &prof_ga &&
#ਜੇ_ਅੰਤ
	    STRNCMP(p, "here", 4) == 0)
    {
	ਜੇ (curbuf->b_ffname == ਨਿਰਾ)
	{
	    emsg(_(e_no_file_name));
	    ਵਾਪਸ FAIL;
	}
	bp->dbg_type = DBG_FILE;
	ਇੱਥੇ = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	    gap != &prof_ga &&
#ਜੇ_ਅੰਤ
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_ਵਿਅੰਜਕ;
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), p);
	ਵਾਪਸ FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    ਜੇ (ਇੱਥੇ)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    ਨਹੀਂਤਾਂ ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	    gap != &prof_ga &&
#ਜੇ_ਅੰਤ
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    ਨਹੀਂਤਾਂ
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    ਜੇ ((!ਇੱਥੇ && *p == NUL)
	    || (ਇੱਥੇ && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)p, "()") != ਨਿਰਾ))
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), arg);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    ਨਹੀਂਤਾਂ ਜੇ (ਇੱਥੇ)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    ਨਹੀਂਤਾਂ ਜੇ (bp->dbg_type == DBG_ਵਿਅੰਜਕ)
    {
	bp->dbg_name = vim_strsave(p);
	ਜੇ (bp->dbg_name != ਨਿਰਾ)
	    bp->dbg_val = eval_ਵਿਅੰਜਕ_no_emsg(bp);
    }
    ਨਹੀਂਤਾਂ
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	ਜੇ (q == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	p = expand_env_save(q);
	vim_free(q);
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	ਜੇ (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	ਨਹੀਂਤਾਂ
	    bp->dbg_name = p;
    }

    ਜੇ (bp->dbg_name == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    ਖਾਲੀ
ex_breakadd(exarg_T *eap)
{
    ਬਣਤਰ debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#ਜੇ_ਅੰਤ

    ਜੇ (dbg_parsearg(eap->arg, gap) != OK)
	ਵਾਪਸ;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    ਜੇ (bp->dbg_type != DBG_ਵਿਅੰਜਕ)
    {
	pat = file_pat_ਮੰਜ਼ਿਲ_reg_pat(bp->dbg_name, ਨਿਰਾ, ਨਿਰਾ, FALSE);
	ਜੇ (pat != ਨਿਰਾ)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	ਜੇ (pat == ਨਿਰਾ || bp->dbg_prog == ਨਿਰਾ)
	    vim_free(bp->dbg_name);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	    ਜੇ (eap->cmdidx != CMD_profile)
#ਜੇ_ਅੰਤ
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++ਆਖਰੀ_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++ਆਖਰੀ_breakp;
	++debug_tick;
	ਜੇ (gap == &dbg_breakp)
	    has_ਵਿਅੰਜਕ_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    ਖਾਲੀ
ex_debuggreedy(exarg_T *eap)
{
    ਜੇ (eap->addr_ਗਿਣਤੀ == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    ਨਹੀਂਤਾਂ
	debug_greedy = FALSE;
}

    ਸਥਿਰ ਖਾਲੀ
update_has_ਵਿਅੰਜਕ_breakpoint(ਖਾਲੀ)
{
    ਪੂਰਨ i;

    has_ਵਿਅੰਜਕ_breakpoint = FALSE;
    ਲਈ (i = 0; i < dbg_breakp.ga_len; ++i)
	ਜੇ (BREAKP(i).dbg_type == DBG_ਵਿਅੰਜਕ)
	{
	    has_ਵਿਅੰਜਕ_breakpoint = TRUE;
	    ਰੋਕੋ;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    ਪੂਰਨ
debug_has_ਵਿਅੰਜਕ_breakpoint(ਖਾਲੀ)
{
    ਵਾਪਸ has_ਵਿਅੰਜਕ_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    ਖਾਲੀ
ex_breakdel(exarg_T *eap)
{
    ਬਣਤਰ debuggy *bp, *bpi;
    ਪੂਰਨ		nr;
    ਪੂਰਨ		todel = -1;
    ਪੂਰਨ		del_all = FALSE;
    ਪੂਰਨ		i;
    linenr_T	ਉੱਤਮ_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    ਜੇ (eap->cmdidx == CMD_profdel)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	gap = &prof_ga;
#ਨਹੀਂਤਾਂ
	ex_ni(eap);
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    }

    ਜੇ (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((ਅੱਖਰ *)eap->arg);
	ਲਈ (i = 0; i < gap->ga_len; ++i)
	    ਜੇ (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		ਰੋਕੋ;
	    }
    }
    ਨਹੀਂਤਾਂ ਜੇ (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	ਜੇ (dbg_parsearg(eap->arg, gap) == FAIL)
	    ਵਾਪਸ;
	bp = &DEBUGGY(gap, gap->ga_len);
	ਲਈ (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    ਜੇ (bp->dbg_type == bpi->dbg_type
		    && STRCMP(bp->dbg_name, bpi->dbg_name) == 0
		    && (bp->dbg_lnum == bpi->dbg_lnum
			|| (bp->dbg_lnum == 0
			    && (ਉੱਤਮ_lnum == 0
				|| bpi->dbg_lnum < ਉੱਤਮ_lnum))))
	    {
		todel = i;
		ਉੱਤਮ_lnum = bpi->dbg_lnum;
	    }
	}
	vim_free(bp->dbg_name);
    }

    ਜੇ (todel < 0)
    {
	semsg(_(e_breakpoint_not_ਮਿਲਿਆ_str), eap->arg);
	ਵਾਪਸ;
    }

    ਜਦੋਂ (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (DEBUGGY(gap, todel).dbg_type == DBG_ਵਿਅੰਜਕ
		&& DEBUGGY(gap, todel).dbg_val != ਨਿਰਾ)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#ਜੇ_ਅੰਤ
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	ਜੇ (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * ਆਕਾਰ(ਬਣਤਰ debuggy));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	ਜੇ (eap->cmdidx == CMD_breakdel)
#ਜੇ_ਅੰਤ
	    ++debug_tick;
	ਜੇ (!del_all)
	    ਰੋਕੋ;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    ਜੇ (gap->ga_len == 0)
	ga_clear(gap);
    ਜੇ (gap == &dbg_breakp)
	update_has_ਵਿਅੰਜਕ_breakpoint();
}

/*
 * ":breaklist".
 */
    ਖਾਲੀ
ex_breaklist(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਬਣਤਰ debuggy *bp;
    ਪੂਰਨ		i;

    ਜੇ (dbg_breakp.ga_len == 0)
    {
	ਸੁਨੇਹਾ(_("No breakpoints defined"));
	ਵਾਪਸ;
    }

    ਲਈ (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	ਜੇ (bp->dbg_type == DBG_FILE)
	    home_replace(ਨਿਰਾ, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	ਜੇ (bp->dbg_type != DBG_ਵਿਅੰਜਕ)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (ਲੰਮਾ)bp->dbg_lnum);
	ਨਹੀਂਤਾਂ
	    smsg(_("%3d  expr %s"),
		    bp->dbg_nr, bp->dbg_name);
    }
}

/*
 * Find a breakpoint for a function or sourced file.
 * Returns line number at which to break; zero when no matching breakpoint.
 */
    linenr_T
dbg_ਲੱਭੋ_breakpoint(
    ਪੂਰਨ		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    ਵਾਪਸ debuggy_ਲੱਭੋ(file, fname, after, &dbg_breakp, ਨਿਰਾ);
}

#ਜੇ defined(FEAT_PROFILE)
# ਜੇ defined(PROF_CACHE_LOG)
ਸਥਿਰ ਪੂਰਨ ਗਿਣਤੀ_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    ਪੂਰਨ
has_profiling(
    ਪੂਰਨ		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    ਪੂਰਨ		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    ਜੇ (prof_ga.ga_len == 0 || !prof_cache_initialized)
	ਵਾਪਸ debuggy_ਲੱਭੋ(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    ਜੇ (hashp != ਨਿਰਾ)
    {
	hash = *hashp;
	ਜੇ (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    ਨਹੀਂਤਾਂ
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	ਜੇ (pe == ਨਿਰਾ)
	    ਵਾਪਸ FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	ਪੂਰਨ f;
	ਪੂਰਨ lnum = debuggy_ਲੱਭੋ(file, fname, (linenr_T)0, &prof_ga, &f);
	ਜੇ (lnum)
	{
	    PEN_ਸੈੱਟ_PROFILING(pe);
	    ਜੇ (f)
		PEN_ਸੈੱਟ_FORCEIT(pe);
	}
	hash_ਜੋੜੋ_ਵਸਤੂ(ht, hi, pe->pen_name, hash);
# ਜੇ defined(PROF_CACHE_LOG)
	ch_log(ਨਿਰਾ, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    ਨਹੀਂਤਾਂ
	pe = HI2PE(hi);
    ਜੇ (fp)
	*fp = PEN_IS_FORCEIT(pe);
# ਜੇ defined(PROF_CACHE_LOG)
    ਗਿਣਤੀ_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    ਵਾਪਸ PEN_IS_PROFILING(pe);
}

    ਸਥਿਰ ਖਾਲੀ
prof_clear_cache(ਖਾਲੀ)
{
    ਜੇ (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    ਲਈ (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	ਜੇ (ht->ht_ਵਰਤਿਆ > 0)
	{
# ਜੇ defined(PROF_CACHE_LOG)
	    ਪੂਰਨ idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(ਨਿਰਾ, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_ਵਰਤਿਆ, ਗਿਣਤੀ_lookups[idx]);
	    ਗਿਣਤੀ_lookups[idx] = 0;
# endif
	    hash_clear_all(ht, offsetof(profentry_T, pen_name));
	    hash_init(ht);
	}
    }
}
#ਜੇ_ਅੰਤ

/*
 * Common code for dbg_find_breakpoint() and has_profiling().
 */
    ਸਥਿਰ linenr_T
debuggy_ਲੱਭੋ(
    ਪੂਰਨ		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    ਪੂਰਨ		*fp)	    // if not NULL: return forceit
{
    ਬਣਤਰ debuggy *bp;
    ਪੂਰਨ		i;
    linenr_T	lnum = 0;
    char_u	*name = ਨਿਰਾ;
    char_u	*short_name = fname;
    ਪੂਰਨ		prev_got_int;

    // Return quickly when there are no breakpoints.
    ਜੇ (gap->ga_len == 0)
	ਵਾਪਸ (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    ਜੇ (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	ਜੇ (name != ਨਿਰਾ)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	ਜੇ (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_ਵਿਅੰਜਕ && (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
		gap == &prof_ga ||
#ਜੇ_ਅੰਤ
		(bp->dbg_lnum > after && (lnum == 0 || bp->dbg_lnum < lnum)))))
	{
	    // Save the value of got_int and reset it.  We don't want a
	    // previous interruption cancel matching, only hitting CTRL-C
	    // while matching should abort it.
	    prev_got_int = got_int;
	    got_int = FALSE;
	    ਜੇ ((name != ਨਿਰਾ
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		ਜੇ (fp != ਨਿਰਾ)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	    ਜੇ (lnum && gap == &prof_ga)
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਨਹੀਂਤਾਂ ਜੇ (bp->dbg_type == DBG_ਵਿਅੰਜਕ)
	{
	    typval_T *tv;
	    ਪੂਰਨ	      ਸਤਰ = FALSE;

	    tv = eval_ਵਿਅੰਜਕ_no_emsg(bp);
	    ਜੇ (tv != ਨਿਰਾ)
	    {
		ਜੇ (bp->dbg_val == ਨਿਰਾ)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(ਨਿਰਾ, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    ਸਤਰ = TRUE;
		}
		ਨਹੀਂਤਾਂ
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? ਵਿਅੰਜਕ_EQUAL : ਵਿਅੰਜਕ_IS;

		    ਜੇ (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_ਸੰਖਿਆ == FALSE)
		    {
			typval_T *v;

			ਸਤਰ = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_ਵਿਅੰਜਕ_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (bp->dbg_val != ਨਿਰਾ)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(ਨਿਰਾ, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = ਨਿਰਾ;
		ਸਤਰ = TRUE;
	    }

	    ਜੇ (ਸਤਰ)
	    {
		lnum = after > 0 ? after : 1;
		ਰੋਕੋ;
	    }
	}
#ਜੇ_ਅੰਤ
    }
    ਜੇ (name != fname)
	vim_free(name);

    ਵਾਪਸ lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    ਖਾਲੀ
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#ਜੇ_ਅੰਤ
