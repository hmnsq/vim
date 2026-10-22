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

#ಆದರೆ defined(FEAT_EVAL)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
ಸ್ಥಿರ ಶೂನ್ಯ do_setdebugtracelevel(char_u *arg);
ಸ್ಥಿರ ಶೂನ್ಯ do_checkbacktracelevel(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ do_showbacktrace(char_u *cmd);

ಸ್ಥಿರ char_u *debug_oldval = ಖಾಲಿ;	// old and newval for debug expressions
ಸ್ಥಿರ char_u *debug_newval = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ     debug_ಅಭಿವ್ಯಕ್ತಿ   = 0;	// use debug_expr

    ಪೂರ್ಣಾಂಕ
has_watchexpr(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ debug_ಅಭಿವ್ಯಕ್ತಿ;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    ಶೂನ್ಯ
do_debug(char_u *cmd)
{
    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
    ಪೂರ್ಣಾಂಕ		save_State = State;
    ಪೂರ್ಣಾಂಕ		save_did_emsg = did_emsg;
    ಪೂರ್ಣಾಂಕ		save_cmd_silent = cmd_silent;
    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent;
    ಪೂರ್ಣಾಂಕ		save_emsg_silent = emsg_silent;
    ಪೂರ್ಣಾಂಕ		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    ಪೂರ್ಣಾಂಕ		typeahead_saved = FALSE;
    ಪೂರ್ಣಾಂಕ		save_ignore_script = 0;
    ಪೂರ್ಣಾಂಕ		save_ex_ಸಾಮಾನ್ಯ_busy;
    ಪೂರ್ಣಾಂಕ		n;
    char_u	*cmdline = ಖಾಲಿ;
    char_u	*p;
    char_u	*sname;
    ಅಕ್ಷರ	*ಬಾಲ = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಕೊನೆಯ_cmd = 0;
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_CONT	1
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_ಮುಂದಿನ	2
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_ಹೆಜ್ಜೆ	3
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_ಮುಕ್ತಾಯ	4
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_QUIT	5
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_ಅಡಚಣೆ	6
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_BACKTRACE	7
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_ಚೌಕಟ್ಟು	8
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_ಮೇಲೆ		9
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_ಕೆಳಕ್ಕೆ	10

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ALWAYS_USE_GUI
    // Can't do this when there is no terminal for input/output.
    ಆದರೆ (!gui.ಒಳ_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

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

    State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;
    debug_ಬಹುಲಕ = TRUE;

    ಆದರೆ (!debug_did_ಸಂದೇಶ)
	ಸಂದೇಶ(_("Entering Debug mode.  Type \"cont\" to continue."));
    ಆದರೆ (debug_oldval != ಖಾಲಿ)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    ಆದರೆ (debug_newval != ಖಾಲಿ)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    ಆದರೆ (sname != ಖಾಲಿ)
	ಸಂದೇಶ((ಅಕ್ಷರ *)sname);
    vim_free(sname);
    ಆದರೆ (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    ಇಲ್ಲದಿದ್ದರೆ
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    ಗಾಗಿ (;;)
    {
	ಸಂದೇಶ_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_ಸಾಮಾನ್ಯ_busy = ex_ಸಾಮಾನ್ಯ_busy;
	ex_ಸಾಮಾನ್ಯ_busy = 0;
	ಆದರೆ (!debug_greedy)
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
	cmdline = getcmdline_prompt('>', ಖಾಲಿ, 0, EXPAND_NOTHING, ಖಾಲಿ);

	debug_break_level = n;
	ಆದರೆ (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_ಸಾಮಾನ್ಯ_busy = save_ex_ಸಾಮಾನ್ಯ_busy;

	cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	ಸಂದೇಶ_starthere();
	ಆದರೆ (cmdline != ಖಾಲಿ)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    ಆದರೆ (*p != NUL)
	    {
		ಆಯ್ಕೆ (*p)
		{
		    ಸ್ಥಿತಿ 'c': ಕೊನೆಯ_cmd = CMD_CONT;
			      ಬಾಲ = "ont";
			      ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'n': ಕೊನೆಯ_cmd = CMD_ಮುಂದಿನ;
			      ಬಾಲ = "ext";
			      ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 's': ಕೊನೆಯ_cmd = CMD_ಹೆಜ್ಜೆ;
			      ಬಾಲ = "tep";
			      ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'f':
			      ಕೊನೆಯ_cmd = 0;
			      ಆದರೆ (p[1] == 'r')
			      {
				  ಕೊನೆಯ_cmd = CMD_ಚೌಕಟ್ಟು;
				  ಬಾಲ = "rame";
			      }
			      ಇಲ್ಲದಿದ್ದರೆ
			      {
				  ಕೊನೆಯ_cmd = CMD_ಮುಕ್ತಾಯ;
				  ಬಾಲ = "inish";
			      }
			      ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'q': ಕೊನೆಯ_cmd = CMD_QUIT;
			      ಬಾಲ = "uit";
			      ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'i': ಕೊನೆಯ_cmd = CMD_ಅಡಚಣೆ;
			      ಬಾಲ = "nterrupt";
			      ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'b': ಕೊನೆಯ_cmd = CMD_BACKTRACE;
			      ಆದರೆ (p[1] == 't')
				  ಬಾಲ = "t";
			      ಇಲ್ಲದಿದ್ದರೆ
				  ಬಾಲ = "acktrace";
			      ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'w': ಕೊನೆಯ_cmd = CMD_BACKTRACE;
			      ಬಾಲ = "here";
			      ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'u': ಕೊನೆಯ_cmd = CMD_ಮೇಲೆ;
			      ಬಾಲ = "p";
			      ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'd': ಕೊನೆಯ_cmd = CMD_ಕೆಳಕ್ಕೆ;
			      ಬಾಲ = "own";
			      ನಿಲ್ಲಿಸು;
		    ಪೂರ್ವನಿಯೋಜಿತ: ಕೊನೆಯ_cmd = 0;
		}
		ಆದರೆ (ಕೊನೆಯ_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    ಆಗ (*p != NUL && *p == *ಬಾಲ)
		    {
			++p;
			++ಬಾಲ;
		    }
		    ಆದರೆ (ASCII_ISALPHA(*p) && ಕೊನೆಯ_cmd != CMD_ಚೌಕಟ್ಟು)
			ಕೊನೆಯ_cmd = 0;
		}
	    }

	    ಆದರೆ (ಕೊನೆಯ_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		ಆಯ್ಕೆ (ಕೊನೆಯ_cmd)
		{
		    ಸ್ಥಿತಿ CMD_CONT:
			debug_break_level = -1;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ CMD_ಮುಂದಿನ:
			debug_break_level = ex_nesting_level;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ CMD_ಹೆಜ್ಜೆ:
			debug_break_level = 9999;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ CMD_ಮುಕ್ತಾಯ:
			debug_break_level = ex_nesting_level - 1;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ CMD_ಅಡಚಣೆ:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			ಕೊನೆಯ_cmd = CMD_ಹೆಜ್ಜೆ;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ CMD_BACKTRACE:
			do_showbacktrace(cmd);
			ಮುಂದುವರಿ;
		    ಸ್ಥಿತಿ CMD_ಚೌಕಟ್ಟು:
			ಆದರೆ (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			ಮುಂದುವರಿ;
		    ಸ್ಥಿತಿ CMD_ಮೇಲೆ:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			ಮುಂದುವರಿ;
		    ಸ್ಥಿತಿ CMD_ಕೆಳಕ್ಕೆ:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			ಮುಂದುವರಿ;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		ನಿಲ್ಲಿಸು;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (ಶೂನ್ಯ)do_cmdline(cmdline, getexline, ಖಾಲಿ,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	ಸಾಲುಗಳು_ಎಡ = ಅಡ್ಡಸಾಲುಗಳು - 1;
    }
    vim_free(cmdline);

    ಆದರೆ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    ಸಂದೇಶ_scroll = save_ಸಂದೇಶ_scroll;
    restore_timeout_for_debugging();
    ಸಾಲುಗಳು_ಎಡ = ಅಡ್ಡಸಾಲುಗಳು - 1;
    State = save_State;
    debug_ಬಹುಲಕ = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    ಸಂದೇಶ_silent = save_ಸಂದೇಶ_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_ಸಂದೇಶ = TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_maxbacktrace_level(char_u *sname)
{
    ಅಕ್ಷರ	*p, *q;
    ಪೂರ್ಣಾಂಕ		maxbacktrace = 0;

    ಆದರೆ (sname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    p = (ಅಕ್ಷರ *)sname;
    ಆಗ ((q = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(p, "..")) != ಖಾಲಿ)
    {
	p = q + 2;
	maxbacktrace++;
    }
    ಹಿಂತಿರುಗಿ maxbacktrace;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_setdebugtracelevel(char_u *arg)
{
    ಪೂರ್ಣಾಂಕ level;

    level = atoi((ಅಕ್ಷರ *)arg);
    ಆದರೆ (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    ಇಲ್ಲದಿದ್ದರೆ
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_checkbacktracelevel(ಶೂನ್ಯ)
{
    ಆದರೆ (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	ಸಂದೇಶ(_("frame is zero"));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	*sname = estack_sfile(ESTACK_NONE);
	ಪೂರ್ಣಾಂಕ	max = get_maxbacktrace_level(sname);

	ಆದರೆ (debug_backtrace_level > max)
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
    ಅಕ್ಷರ    *cur;
    ಅಕ್ಷರ    *ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ	    i = 0;
    ಪೂರ್ಣಾಂಕ	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    ಆದರೆ (sname != ಖಾಲಿ)
    {
	cur = (ಅಕ್ಷರ *)sname;
	ಆಗ (!got_int)
	{
	    ಮುಂದಿನ = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(cur, "..");
	    ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
		*ಮುಂದಿನ = NUL;
	    ಆದರೆ (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    ಇಲ್ಲದಿದ್ದರೆ
		smsg("  %d %s", max - i, cur);
	    ++i;
	    ಆದರೆ (ಮುಂದಿನ == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    *ಮುಂದಿನ = '.';
	    cur = ಮುಂದಿನ + 2;
	}
	vim_free(sname);
    }

    ಆದರೆ (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (ದೀರ್ಘ)SOURCING_LNUM, cmd);
    ಇಲ್ಲದಿದ್ದರೆ
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    ಶೂನ್ಯ
ex_debug(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

ಸ್ಥಿರ char_u	*debug_breakpoint_name = ಖಾಲಿ;
ಸ್ಥಿರ linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	debug_skipped;
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
    ಆದರೆ (debug_breakpoint_name != ಖಾಲಿ)
    {
	ಆದರೆ (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    ಆದರೆ (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    ಇಲ್ಲದಿದ್ದರೆ
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (ದೀರ್ಘ)debug_breakpoint_lnum);
	    debug_breakpoint_name = ಖಾಲಿ;
	    do_debug(eap->cmd);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ex_nesting_level <= debug_break_level)
    {
	ಆದರೆ (!eap->skip)
	    do_debug(eap->cmd);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = ಖಾಲಿ;
	}
    }
}

/*
 * Go to debug mode if skipped by dbg_check_breakpoint() because eap->skip was
 * set.  Return TRUE when the debug mode is entered this time.
 */
    ಪೂರ್ಣಾಂಕ
dbg_check_skipped(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		prev_got_int;

    ಆದರೆ (!debug_skipped)
	ಹಿಂತಿರುಗಿ FALSE;

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
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
ರಚನೆ debuggy
{
    ಪೂರ್ಣಾಂಕ		dbg_nr;		// breakpoint number
    ಪೂರ್ಣಾಂಕ		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    ಪೂರ್ಣಾಂಕ		dbg_forceit;	// ! used
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		dbg_level;      // stored nested level for expr
};

ಸ್ಥಿರ garray_T dbg_breakp = {0, 0, ಗಾತ್ರ(ರಚನೆ debuggy), 4, ಖಾಲಿ};
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKP(idx)		(((ರಚನೆ debuggy *)dbg_breakp.ga_ದತ್ತಾಂಶ)[idx])
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUGGY(gap, idx)	(((ರಚನೆ debuggy *)gap->ga_ದತ್ತಾಂಶ)[idx])
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_breakp = 0;	// nr of last defined breakpoint
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
ಸ್ಥಿರ garray_T prof_ga = {0, 0, ಗಾತ್ರ(ರಚನೆ debuggy), 4, ಖಾಲಿ};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
ಸ್ಥಿರ hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prof_cache_initialized;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ profentry_S
{
    ಅಕ್ಷರ    pen_flags;	// cache data booleans: profiling, forceit
    char_u  pen_name[1]; // actually longer
} profentry_T;
# define PEN_FLAG_PROFILING 1
# define PEN_FLAG_FORCEIT 2
# define PEN_ಗಣ_PROFILING(pe) ((pe)->pen_flags |= PEN_FLAG_PROFILING)
# define PEN_ಗಣ_FORCEIT(pe) ((pe)->pen_flags |= PEN_FLAG_FORCEIT)
# define PEN_IS_PROFILING(pe) (((pe)->pen_flags & PEN_FLAG_PROFILING) != 0)
# define PEN_IS_FORCEIT(pe) (((pe)->pen_flags & PEN_FLAG_FORCEIT) != 0)

# define PE2HIKEY(pe)	((pe)->pen_name)
# define HIKEY2PE(p)	((profentry_T *)((p) - (offsetof(profentry_T, pen_name))))
# define HI2PE(hi)	HIKEY2PE((hi)->hi_ಕೀಲಿ)

ಸ್ಥಿರ ಶೂನ್ಯ prof_clear_cache(ಶೂನ್ಯ);
# define PROF_CLEAR_CACHE(gap) ಮಾಡು {ಆದರೆ ((gap) == &prof_ga) prof_clear_cache();} ಆಗ (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#ಇಲ್ಲದಿದ್ದರೆ
# define PROF_CLEAR_CACHE(gap) ಮಾಡು {} ಆಗ (0)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು DBG_FUNC	1
#ವ್ಯಾಖ್ಯಾನಿಸು DBG_FILE	2
#ವ್ಯಾಖ್ಯಾನಿಸು DBG_ಅಭಿವ್ಯಕ್ತಿ	3

ಸ್ಥಿರ linenr_T debuggy_ಕಂಡುಹಿಡಿ(ಪೂರ್ಣಾಂಕ file,char_u *fname, linenr_T after, garray_T *gap, ಪೂರ್ಣಾಂಕ *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    ಸ್ಥಿರ typval_T *
eval_ಅಭಿವ್ಯಕ್ತಿ_no_emsg(ರಚನೆ debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_ಅಭಿವ್ಯಕ್ತಿ(bp->dbg_name, ಖಾಲಿ);
    --emsg_off;

    ಹಿಂತಿರುಗಿ tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    ರಚನೆ debuggy *bp;
    ಪೂರ್ಣಾಂಕ		ಇಲ್ಲಿ = FALSE;

    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    ಆದರೆ (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	    gap != &prof_ga &&
#ಆದರೆ_ಅಂತ್ಯ
	    STRNCMP(p, "here", 4) == 0)
    {
	ಆದರೆ (curbuf->b_ffname == ಖಾಲಿ)
	{
	    emsg(_(e_no_file_name));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	bp->dbg_type = DBG_FILE;
	ಇಲ್ಲಿ = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	    gap != &prof_ga &&
#ಆದರೆ_ಅಂತ್ಯ
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_ಅಭಿವ್ಯಕ್ತಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_invalid_ವಾದ_str), p);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    ಆದರೆ (ಇಲ್ಲಿ)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	    gap != &prof_ga &&
#ಆದರೆ_ಅಂತ್ಯ
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    ಆದರೆ ((!ಇಲ್ಲಿ && *p == NUL)
	    || (ಇಲ್ಲಿ && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)p, "()") != ಖಾಲಿ))
    {
	semsg(_(e_invalid_ವಾದ_str), arg);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಇಲ್ಲಿ)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bp->dbg_type == DBG_ಅಭಿವ್ಯಕ್ತಿ)
    {
	bp->dbg_name = vim_strsave(p);
	ಆದರೆ (bp->dbg_name != ಖಾಲಿ)
	    bp->dbg_val = eval_ಅಭಿವ್ಯಕ್ತಿ_no_emsg(bp);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	ಆದರೆ (q == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	p = expand_env_save(q);
	vim_free(q);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    bp->dbg_name = p;
    }

    ಆದರೆ (bp->dbg_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    ಶೂನ್ಯ
ex_breakadd(exarg_T *eap)
{
    ರಚನೆ debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (dbg_parsearg(eap->arg, gap) != OK)
	ಹಿಂತಿರುಗಿ;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    ಆದರೆ (bp->dbg_type != DBG_ಅಭಿವ್ಯಕ್ತಿ)
    {
	pat = file_pat_ಗಮ್ಯ_reg_pat(bp->dbg_name, ಖಾಲಿ, ಖಾಲಿ, FALSE);
	ಆದರೆ (pat != ಖಾಲಿ)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	ಆದರೆ (pat == ಖಾಲಿ || bp->dbg_prog == ಖಾಲಿ)
	    vim_free(bp->dbg_name);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	    ಆದರೆ (eap->cmdidx != CMD_profile)
#ಆದರೆ_ಅಂತ್ಯ
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++ಕೊನೆಯ_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++ಕೊನೆಯ_breakp;
	++debug_tick;
	ಆದರೆ (gap == &dbg_breakp)
	    has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    ಶೂನ್ಯ
ex_debuggreedy(exarg_T *eap)
{
    ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	debug_greedy = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
update_has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint = FALSE;
    ಗಾಗಿ (i = 0; i < dbg_breakp.ga_len; ++i)
	ಆದರೆ (BREAKP(i).dbg_type == DBG_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    ಪೂರ್ಣಾಂಕ
debug_has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    ಶೂನ್ಯ
ex_breakdel(exarg_T *eap)
{
    ರಚನೆ debuggy *bp, *bpi;
    ಪೂರ್ಣಾಂಕ		nr;
    ಪೂರ್ಣಾಂಕ		todel = -1;
    ಪೂರ್ಣಾಂಕ		del_all = FALSE;
    ಪೂರ್ಣಾಂಕ		i;
    linenr_T	ಉತ್ತಮ_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    ಆದರೆ (eap->cmdidx == CMD_profdel)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	gap = &prof_ga;
#ಇಲ್ಲದಿದ್ದರೆ
	ex_ni(eap);
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((ಅಕ್ಷರ *)eap->arg);
	ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	    ಆದರೆ (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		ನಿಲ್ಲಿಸು;
	    }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	ಆದರೆ (dbg_parsearg(eap->arg, gap) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	bp = &DEBUGGY(gap, gap->ga_len);
	ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    ಆದರೆ (bp->dbg_type == bpi->dbg_type
		    && STRCMP(bp->dbg_name, bpi->dbg_name) == 0
		    && (bp->dbg_lnum == bpi->dbg_lnum
			|| (bp->dbg_lnum == 0
			    && (ಉತ್ತಮ_lnum == 0
				|| bpi->dbg_lnum < ಉತ್ತಮ_lnum))))
	    {
		todel = i;
		ಉತ್ತಮ_lnum = bpi->dbg_lnum;
	    }
	}
	vim_free(bp->dbg_name);
    }

    ಆದರೆ (todel < 0)
    {
	semsg(_(e_breakpoint_not_ಸಿಕ್ಕಿತು_str), eap->arg);
	ಹಿಂತಿರುಗಿ;
    }

    ಆಗ (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (DEBUGGY(gap, todel).dbg_type == DBG_ಅಭಿವ್ಯಕ್ತಿ
		&& DEBUGGY(gap, todel).dbg_val != ಖಾಲಿ)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#ಆದರೆ_ಅಂತ್ಯ
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	ಆದರೆ (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * ಗಾತ್ರ(ರಚನೆ debuggy));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ಆದರೆ (eap->cmdidx == CMD_breakdel)
#ಆದರೆ_ಅಂತ್ಯ
	    ++debug_tick;
	ಆದರೆ (!del_all)
	    ನಿಲ್ಲಿಸು;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    ಆದರೆ (gap->ga_len == 0)
	ga_clear(gap);
    ಆದರೆ (gap == &dbg_breakp)
	update_has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint();
}

/*
 * ":breaklist".
 */
    ಶೂನ್ಯ
ex_breaklist(exarg_T *eap ಬಳಸದ)
{
    ರಚನೆ debuggy *bp;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (dbg_breakp.ga_len == 0)
    {
	ಸಂದೇಶ(_("No breakpoints defined"));
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	ಆದರೆ (bp->dbg_type == DBG_FILE)
	    home_replace(ಖಾಲಿ, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	ಆದರೆ (bp->dbg_type != DBG_ಅಭಿವ್ಯಕ್ತಿ)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (ದೀರ್ಘ)bp->dbg_lnum);
	ಇಲ್ಲದಿದ್ದರೆ
	    smsg(_("%3d  expr %s"),
		    bp->dbg_nr, bp->dbg_name);
    }
}

/*
 * Find a breakpoint for a function or sourced file.
 * Returns line number at which to break; zero when no matching breakpoint.
 */
    linenr_T
dbg_ಕಂಡುಹಿಡಿ_breakpoint(
    ಪೂರ್ಣಾಂಕ		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    ಹಿಂತಿರುಗಿ debuggy_ಕಂಡುಹಿಡಿ(file, fname, after, &dbg_breakp, ಖಾಲಿ);
}

#ಆದರೆ defined(FEAT_PROFILE)
# ಆದರೆ defined(PROF_CACHE_LOG)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_lookups[2];
# endif
/*
 * Return TRUE if profiling is on for a function or sourced file.
 * Cache the results of debuggy_find().
 * Cache is cleared whenever prof_ga.ga_len is changed.
 */
    ಪೂರ್ಣಾಂಕ
has_profiling(
    ಪೂರ್ಣಾಂಕ		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    ಪೂರ್ಣಾಂಕ		*fp,	    // return: forceit
    hash_T	*hashp)	    // use/return fname hash, may be NULL
{
    ಆದರೆ (prof_ga.ga_len == 0 || !prof_cache_initialized)
	ಹಿಂತಿರುಗಿ debuggy_ಕಂಡುಹಿಡಿ(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    ಆದರೆ (hashp != ಖಾಲಿ)
    {
	hash = *hashp;
	ಆದರೆ (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	ಆದರೆ (pe == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	ಪೂರ್ಣಾಂಕ f;
	ಪೂರ್ಣಾಂಕ lnum = debuggy_ಕಂಡುಹಿಡಿ(file, fname, (linenr_T)0, &prof_ga, &f);
	ಆದರೆ (lnum)
	{
	    PEN_ಗಣ_PROFILING(pe);
	    ಆದರೆ (f)
		PEN_ಗಣ_FORCEIT(pe);
	}
	hash_ಕೂಡಿಸು_ಅಂಶ(ht, hi, pe->pen_name, hash);
# ಆದರೆ defined(PROF_CACHE_LOG)
	ch_log(ಖಾಲಿ, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ
	pe = HI2PE(hi);
    ಆದರೆ (fp)
	*fp = PEN_IS_FORCEIT(pe);
# ಆದರೆ defined(PROF_CACHE_LOG)
    ಎಣಿಕೆ_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    ಹಿಂತಿರುಗಿ PEN_IS_PROFILING(pe);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prof_clear_cache(ಶೂನ್ಯ)
{
    ಆದರೆ (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    ಗಾಗಿ (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	ಆದರೆ (ht->ht_ಬಳಸಿದ > 0)
	{
# ಆದರೆ defined(PROF_CACHE_LOG)
	    ಪೂರ್ಣಾಂಕ idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(ಖಾಲಿ, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_ಬಳಸಿದ, ಎಣಿಕೆ_lookups[idx]);
	    ಎಣಿಕೆ_lookups[idx] = 0;
# endif
	    hash_clear_all(ht, offsetof(profentry_T, pen_name));
	    hash_init(ht);
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Common code for dbg_find_breakpoint() and has_profiling().
 */
    ಸ್ಥಿರ linenr_T
debuggy_ಕಂಡುಹಿಡಿ(
    ಪೂರ್ಣಾಂಕ		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    ಪೂರ್ಣಾಂಕ		*fp)	    // if not NULL: return forceit
{
    ರಚನೆ debuggy *bp;
    ಪೂರ್ಣಾಂಕ		i;
    linenr_T	lnum = 0;
    char_u	*name = ಖಾಲಿ;
    char_u	*short_name = fname;
    ಪೂರ್ಣಾಂಕ		prev_got_int;

    // Return quickly when there are no breakpoints.
    ಆದರೆ (gap->ga_len == 0)
	ಹಿಂತಿರುಗಿ (linenr_T)0;

    // For a script-local function remove the prefix, so that
    // "profile func Func" matches "Func" in any script.  Otherwise it's very
    // difficult to profile/debug a script-local function.  It may match a
    // function in the wrong script, but that is much better than not being
    // able to profile/debug a function in a script with unknown ID.
    // Also match a script-specific name.
    ಆದರೆ (!is_file && fname[0] == K_SPECIAL)
    {
	short_name = vim_strchr(fname, '_') + 1;
	name = alloc(STRLEN(fname) + 3);
	ಆದರೆ (name != ಖಾಲಿ)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	ಆದರೆ (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_ಅಭಿವ್ಯಕ್ತಿ && (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
		gap == &prof_ga ||
#ಆದರೆ_ಅಂತ್ಯ
		(bp->dbg_lnum > after && (lnum == 0 || bp->dbg_lnum < lnum)))))
	{
	    // Save the value of got_int and reset it.  We don't want a
	    // previous interruption cancel matching, only hitting CTRL-C
	    // while matching should abort it.
	    prev_got_int = got_int;
	    got_int = FALSE;
	    ಆದರೆ ((name != ಖಾಲಿ
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		ಆದರೆ (fp != ಖಾಲಿ)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= prev_got_int;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	    ಆದರೆ (lnum && gap == &prof_ga)
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bp->dbg_type == DBG_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    typval_T *tv;
	    ಪೂರ್ಣಾಂಕ	      ಸಾಲು = FALSE;

	    tv = eval_ಅಭಿವ್ಯಕ್ತಿ_no_emsg(bp);
	    ಆದರೆ (tv != ಖಾಲಿ)
	    {
		ಆದರೆ (bp->dbg_val == ಖಾಲಿ)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(ಖಾಲಿ, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    ಸಾಲು = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? ಅಭಿವ್ಯಕ್ತಿ_EQUAL : ಅಭಿವ್ಯಕ್ತಿ_IS;

		    ಆದರೆ (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
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
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bp->dbg_val != ಖಾಲಿ)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(ಖಾಲಿ, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = ಖಾಲಿ;
		ಸಾಲು = TRUE;
	    }

	    ಆದರೆ (ಸಾಲು)
	    {
		lnum = after > 0 ? after : 1;
		ನಿಲ್ಲಿಸು;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (name != fname)
	vim_free(name);

    ಹಿಂತಿರುಗಿ lnum;
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
#ಆದರೆ_ಅಂತ್ಯ
