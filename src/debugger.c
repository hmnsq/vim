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
ساکن صحیح debug_greedy = FALSE;	// batch mode debugging: don't save
					// and restore typeahead.
ساکن خالی do_setdebugtracelevel(char_u *arg);
ساکن خالی do_checkbacktracelevel(خالی);
ساکن خالی do_showbacktrace(char_u *cmd);

ساکن char_u *debug_oldval = تہی;	// old and newval for debug expressions
ساکن char_u *debug_newval = تہی;
ساکن صحیح     debug_اظہاریہ   = 0;	// use debug_expr

    صحیح
has_watchexpr(خالی)
{
    واپس debug_اظہاریہ;
}

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    خالی
do_debug(char_u *cmd)
{
    صحیح		save_پیغام_scroll = پیغام_scroll;
    صحیح		save_State = State;
    صحیح		save_did_emsg = did_emsg;
    صحیح		save_cmd_silent = cmd_silent;
    صحیح		save_پیغام_silent = پیغام_silent;
    صحیح		save_emsg_silent = emsg_silent;
    صحیح		save_redir_off = redir_off;
    tasave_T	typeaheadbuf;
    صحیح		typeahead_saved = FALSE;
    صحیح		save_ignore_script = 0;
    صحیح		save_ex_معمول_busy;
    صحیح		n;
    char_u	*cmdline = تہی;
    char_u	*p;
    char_u	*sname;
    حرف	*دم = تہی;
    ساکن صحیح	آخری_cmd = 0;
#تعریف_کرو CMD_CONT	1
#تعریف_کرو CMD_اگلا	2
#تعریف_کرو CMD_قدم	3
#تعریف_کرو CMD_اختتام	4
#تعریف_کرو CMD_QUIT	5
#تعریف_کرو CMD_مداخلت	6
#تعریف_کرو CMD_BACKTRACE	7
#تعریف_کرو CMD_چوکھٹا	8
#تعریف_کرو CMD_اوپر		9
#تعریف_کرو CMD_نیچے_کو	10

#اگر_تعریف_شدہ ALWAYS_USE_GUI
    // Can't do this when there is no terminal for input/output.
    اگر (!gui.اندر_use)
    {
	// Break as soon as possible.
	debug_break_level = 9999;
	واپس;
    }
#اگر_ختم

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

    State = منوال_معمول;
    debug_منوال = TRUE;

    اگر (!debug_did_پیغام)
	پیغام(_("Entering Debug mode.  Type \"cont\" to continue."));
    اگر (debug_oldval != تہی)
    {
	smsg(_("Oldval = \"%s\""), debug_oldval);
	VIM_CLEAR(debug_oldval);
    }
    اگر (debug_newval != تہی)
    {
	smsg(_("Newval = \"%s\""), debug_newval);
	VIM_CLEAR(debug_newval);
    }
    sname = estack_sfile(ESTACK_NONE);
    اگر (sname != تہی)
	پیغام((حرف *)sname);
    vim_free(sname);
    اگر (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), SOURCING_LNUM, cmd);
    ورنہ
	smsg(_("cmd: %s"), cmd);

    // Repeat getting a command and executing it.
    برائے (;;)
    {
	پیغام_scroll = TRUE;
	need_wait_return = FALSE;

	// Save the current typeahead buffer and replace it with an empty one.
	// This makes sure we get input from the user here and don't interfere
	// with the commands being executed.  Reset "ex_normal_busy" to avoid
	// the side effects of using ":normal". Save the stuff buffer and make
	// it empty. Set ignore_script to avoid reading from script input.
	save_ex_معمول_busy = ex_معمول_busy;
	ex_معمول_busy = 0;
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
	cmdline = getcmdline_prompt('>', تہی, 0, EXPAND_NOTHING, تہی);

	debug_break_level = n;
	اگر (typeahead_saved)
	{
	    restore_typeahead(&typeaheadbuf, TRUE);
	    ignore_script = save_ignore_script;
	}
	ex_معمول_busy = save_ex_معمول_busy;

	cmdline_صف = پیغام_صف;
	پیغام_starthere();
	اگر (cmdline != تہی)
	{
	    // If this is a debug command, set "last_cmd".
	    // If not, reset "last_cmd".
	    // For a blank line use previous command.
	    p = skipwhite(cmdline);
	    اگر (*p != NUL)
	    {
		انتخاب (*p)
		{
		    حالت 'c': آخری_cmd = CMD_CONT;
			      دم = "ont";
			      توڑو;
		    حالت 'n': آخری_cmd = CMD_اگلا;
			      دم = "ext";
			      توڑو;
		    حالت 's': آخری_cmd = CMD_قدم;
			      دم = "tep";
			      توڑو;
		    حالت 'f':
			      آخری_cmd = 0;
			      اگر (p[1] == 'r')
			      {
				  آخری_cmd = CMD_چوکھٹا;
				  دم = "rame";
			      }
			      ورنہ
			      {
				  آخری_cmd = CMD_اختتام;
				  دم = "inish";
			      }
			      توڑو;
		    حالت 'q': آخری_cmd = CMD_QUIT;
			      دم = "uit";
			      توڑو;
		    حالت 'i': آخری_cmd = CMD_مداخلت;
			      دم = "nterrupt";
			      توڑو;
		    حالت 'b': آخری_cmd = CMD_BACKTRACE;
			      اگر (p[1] == 't')
				  دم = "t";
			      ورنہ
				  دم = "acktrace";
			      توڑو;
		    حالت 'w': آخری_cmd = CMD_BACKTRACE;
			      دم = "here";
			      توڑو;
		    حالت 'u': آخری_cmd = CMD_اوپر;
			      دم = "p";
			      توڑو;
		    حالت 'd': آخری_cmd = CMD_نیچے_کو;
			      دم = "own";
			      توڑو;
		    طےشدہ: آخری_cmd = 0;
		}
		اگر (آخری_cmd != 0)
		{
		    // Check that the tail matches.
		    ++p;
		    جب (*p != NUL && *p == *دم)
		    {
			++p;
			++دم;
		    }
		    اگر (ASCII_ISALPHA(*p) && آخری_cmd != CMD_چوکھٹا)
			آخری_cmd = 0;
		}
	    }

	    اگر (آخری_cmd != 0)
	    {
		// Execute debug command: decide where to break next and
		// return.
		انتخاب (آخری_cmd)
		{
		    حالت CMD_CONT:
			debug_break_level = -1;
			توڑو;
		    حالت CMD_اگلا:
			debug_break_level = ex_nesting_level;
			توڑو;
		    حالت CMD_قدم:
			debug_break_level = 9999;
			توڑو;
		    حالت CMD_اختتام:
			debug_break_level = ex_nesting_level - 1;
			توڑو;
		    حالت CMD_QUIT:
			got_int = TRUE;
			debug_break_level = -1;
			توڑو;
		    حالت CMD_مداخلت:
			got_int = TRUE;
			debug_break_level = 9999;
			// Do not repeat ">interrupt" cmd, continue stepping.
			آخری_cmd = CMD_قدم;
			توڑو;
		    حالت CMD_BACKTRACE:
			do_showbacktrace(cmd);
			جاری;
		    حالت CMD_چوکھٹا:
			اگر (*p == NUL)
			{
			    do_showbacktrace(cmd);
			}
			ورنہ
			{
			    p = skipwhite(p);
			    do_setdebugtracelevel(p);
			}
			جاری;
		    حالت CMD_اوپر:
			debug_backtrace_level++;
			do_checkbacktracelevel();
			جاری;
		    حالت CMD_نیچے_کو:
			debug_backtrace_level--;
			do_checkbacktracelevel();
			جاری;
		}
		// Going out reset backtrace_level
		debug_backtrace_level = 0;
		توڑو;
	    }

	    // don't debug this command
	    n = debug_break_level;
	    debug_break_level = -1;
	    (خالی)do_cmdline(cmdline, getexline, تہی,
						DOCMD_VERBOSE|DOCMD_EXCRESET);
	    debug_break_level = n;
	}
	سطریں_بایاں = صفیں - 1;
    }
    vim_free(cmdline);

    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(UPD_NOT_VALID);
    need_wait_return = FALSE;
    پیغام_scroll = save_پیغام_scroll;
    restore_timeout_for_debugging();
    سطریں_بایاں = صفیں - 1;
    State = save_State;
    debug_منوال = FALSE;
    did_emsg = save_did_emsg;
    cmd_silent = save_cmd_silent;
    پیغام_silent = save_پیغام_silent;
    emsg_silent = save_emsg_silent;
    redir_off = save_redir_off;

    // Only print the message again when typing a command before coming back
    // here.
    debug_did_پیغام = TRUE;
}

    ساکن صحیح
get_maxbacktrace_level(char_u *sname)
{
    حرف	*p, *q;
    صحیح		maxbacktrace = 0;

    اگر (sname == تہی)
	واپس 0;

    p = (حرف *)sname;
    جب ((q = لڑی_تلاش(p, "..")) != تہی)
    {
	p = q + 2;
	maxbacktrace++;
    }
    واپس maxbacktrace;
}

    ساکن خالی
do_setdebugtracelevel(char_u *arg)
{
    صحیح level;

    level = atoi((حرف *)arg);
    اگر (*arg == '+' || level < 0)
	debug_backtrace_level += level;
    ورنہ
	debug_backtrace_level = level;

    do_checkbacktracelevel();
}

    ساکن خالی
do_checkbacktracelevel(خالی)
{
    اگر (debug_backtrace_level < 0)
    {
	debug_backtrace_level = 0;
	پیغام(_("frame is zero"));
    }
    ورنہ
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

    ساکن خالی
do_showbacktrace(char_u *cmd)
{
    char_u  *sname;
    حرف    *cur;
    حرف    *اگلا;
    صحیح	    i = 0;
    صحیح	    max;

    sname = estack_sfile(ESTACK_NONE);
    max = get_maxbacktrace_level(sname);
    اگر (sname != تہی)
    {
	cur = (حرف *)sname;
	جب (!got_int)
	{
	    اگلا = لڑی_تلاش(cur, "..");
	    اگر (اگلا != تہی)
		*اگلا = NUL;
	    اگر (i == max - debug_backtrace_level)
		smsg("->%d %s", max - i, cur);
	    ورنہ
		smsg("  %d %s", max - i, cur);
	    ++i;
	    اگر (اگلا == تہی)
		توڑو;
	    *اگلا = '.';
	    cur = اگلا + 2;
	}
	vim_free(sname);
    }

    اگر (SOURCING_LNUM != 0)
	smsg(_("line %ld: %s"), (طویل)SOURCING_LNUM, cmd);
    ورنہ
	smsg(_("cmd: %s"), cmd);
}

/*
 * ":debug".
 */
    خالی
ex_debug(exarg_T *eap)
{
    صحیح		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

ساکن char_u	*debug_breakpoint_name = تہی;
ساکن linenr_T	debug_breakpoint_lnum;

/*
 * When debugging or a breakpoint is set on a skipped command, no debug prompt
 * is shown by do_one_cmd().  This situation is indicated by debug_skipped, and
 * debug_skipped_name is then set to the source name in the breakpoint case.  If
 * a skipped command decides itself that a debug prompt should be displayed, it
 * can do so by calling dbg_check_skipped().
 */
ساکن صحیح	debug_skipped;
ساکن char_u	*debug_skipped_name;

/*
 * Go to debug mode when a breakpoint was encountered or "ex_nesting_level" is
 * at or below the break level.  But only when the line is actually
 * executed.  Return TRUE and set breakpoint_name for skipped commands that
 * decide to execute something themselves.
 * Called from do_one_cmd() before executing a command.
 */
    خالی
dbg_check_breakpoint(exarg_T *eap)
{
    char_u	*p;

    debug_skipped = FALSE;
    اگر (debug_breakpoint_name != تہی)
    {
	اگر (!eap->skip)
	{
	    // replace K_SNR with "<SNR>"
	    اگر (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == KE_SNR)
		p = (char_u *)"<SNR>";
	    ورنہ
		p = (char_u *)"";
	    smsg(_("Breakpoint in \"%s%s\" line %ld"),
		    p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (طویل)debug_breakpoint_lnum);
	    debug_breakpoint_name = تہی;
	    do_debug(eap->cmd);
	}
	ورنہ
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = debug_breakpoint_name;
	    debug_breakpoint_name = تہی;
	}
    }
    ورنہ اگر (ex_nesting_level <= debug_break_level)
    {
	اگر (!eap->skip)
	    do_debug(eap->cmd);
	ورنہ
	{
	    debug_skipped = TRUE;
	    debug_skipped_name = تہی;
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
    صحیح		پچھلا_got_int;

    اگر (!debug_skipped)
	واپس FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cause flushing the input buffer.
    پچھلا_got_int = got_int;
    got_int = FALSE;
    debug_breakpoint_name = debug_skipped_name;
    // eap->skip is TRUE
    eap->skip = FALSE;
    (خالی)dbg_check_breakpoint(eap);
    eap->skip = TRUE;
    got_int |= پچھلا_got_int;
    واپس TRUE;
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
ساخت debuggy
{
    صحیح		dbg_nr;		// breakpoint number
    صحیح		dbg_type;	// DBG_FUNC, DBG_FILE or DBG_EXPR
    char_u	*dbg_name;	// function, expression or file name
    regprog_T	*dbg_prog;	// regexp program
    linenr_T	dbg_lnum;	// line number in function or file
    صحیح		dbg_forceit;	// ! used
#اگر_تعریف_شدہ FEAT_EVAL
    typval_T    *dbg_val;       // last result of watchexpression
#اگر_ختم
    صحیح		dbg_level;      // stored nested level for expr
};

ساکن garray_T dbg_breakp = {0, 0, حجم(ساخت debuggy), 4, تہی};
#تعریف_کرو BREAKP(idx)		(((ساخت debuggy *)dbg_breakp.ga_ڈیٹا)[idx])
#تعریف_کرو DEBUGGY(gap, idx)	(((ساخت debuggy *)gap->ga_ڈیٹا)[idx])
ساکن صحیح آخری_breakp = 0;	// nr of last defined breakpoint
ساکن صحیح has_اظہاریہ_breakpoint = FALSE;

#اگر_تعریف_شدہ FEAT_PROFILE
// Profiling uses file and func names similar to breakpoints.
ساکن garray_T prof_ga = {0, 0, حجم(ساخت debuggy), 4, تہی};

// Profiling caches results of regexp lookups for function/script name.
# define N_PROF_HTAB 2
ساکن hashtab_T prof_cache[N_PROF_HTAB];
# define PROF_HTAB_FUNCS 0
# define PROF_HTAB_FILES 1
ساکن صحیح prof_cache_initialized;
قسمتعریف ساخت profentry_S
{
    حرف    pen_flags;	// cache data booleans: profiling, forceit
    char_u  pen_name[1]; // actually longer
} profentry_T;
# define PEN_FLAG_PROFILING 1
# define PEN_FLAG_FORCEIT 2
# define PEN_سیٹ_PROFILING(pe) ((pe)->pen_flags |= PEN_FLAG_PROFILING)
# define PEN_سیٹ_FORCEIT(pe) ((pe)->pen_flags |= PEN_FLAG_FORCEIT)
# define PEN_IS_PROFILING(pe) (((pe)->pen_flags & PEN_FLAG_PROFILING) != 0)
# define PEN_IS_FORCEIT(pe) (((pe)->pen_flags & PEN_FLAG_FORCEIT) != 0)

# define PE2HIKEY(pe)	((pe)->pen_name)
# define HIKEY2PE(p)	((profentry_T *)((p) - (offsetof(profentry_T, pen_name))))
# define HI2PE(hi)	HIKEY2PE((hi)->hi_کلید)

ساکن خالی prof_clear_cache(خالی);
# define PROF_CLEAR_CACHE(gap) کرو {اگر ((gap) == &prof_ga) prof_clear_cache();} جب (0)
// Can enable to get some info about profile caching
// #define PROF_CACHE_LOG
#ورنہ
# define PROF_CLEAR_CACHE(gap) کرو {} جب (0)
#اگر_ختم
#تعریف_کرو DBG_FUNC	1
#تعریف_کرو DBG_FILE	2
#تعریف_کرو DBG_اظہاریہ	3

ساکن linenr_T debuggy_ڈھونڈو(صحیح file,char_u *fname, linenr_T after, garray_T *gap, صحیح *fp);

/*
 * Evaluate the "bp->dbg_name" expression and return the result.
 * Disables error messages.
 */
    ساکن typval_T *
eval_اظہاریہ_no_emsg(ساخت debuggy *bp)
{
    typval_T	*tv;

    // Disable error messages, a bad expression would make Vim unusable.
    ++emsg_off;
    tv = eval_اظہاریہ(bp->dbg_name, تہی);
    --emsg_off;

    واپس tv;
}

/*
 * Parse the arguments of ":profile", ":breakadd" or ":breakdel" and put them
 * in the entry just after the last one in dbg_breakp.  Note that "dbg_name"
 * is allocated.
 * Returns FAIL for failure.
 */
    ساکن صحیح
dbg_parsearg(
    char_u	*arg,
    garray_T	*gap)	    // either &dbg_breakp or &prof_ga
{
    char_u	*p = arg;
    char_u	*q;
    ساخت debuggy *bp;
    صحیح		یہاں = FALSE;

    اگر (ga_grow(gap, 1) == FAIL)
	واپس FAIL;
    bp = &DEBUGGY(gap, gap->ga_len);

    // Find "func" or "file".
    اگر (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    ورنہ اگر (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    ورنہ اگر (
#اگر_تعریف_شدہ FEAT_PROFILE
	    gap != &prof_ga &&
#اگر_ختم
	    STRNCMP(p, "here", 4) == 0)
    {
	اگر (curbuf->b_ffname == تہی)
	{
	    emsg(_(e_no_file_name));
	    واپس FAIL;
	}
	bp->dbg_type = DBG_FILE;
	یہاں = TRUE;
    }
    ورنہ اگر (
#اگر_تعریف_شدہ FEAT_PROFILE
	    gap != &prof_ga &&
#اگر_ختم
	    STRNCMP(p, "expr", 4) == 0)
	bp->dbg_type = DBG_اظہاریہ;
    ورنہ
    {
	semsg(_(e_invalid_دلیل_str), p);
	واپس FAIL;
    }
    p = skipwhite(p + 4);

    // Find optional line number.
    اگر (یہاں)
	bp->dbg_lnum = curwin->w_cursor.lnum;
    ورنہ اگر (
#اگر_تعریف_شدہ FEAT_PROFILE
	    gap != &prof_ga &&
#اگر_ختم
	    VIM_ISDIGIT(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    ورنہ
	bp->dbg_lnum = 0;

    // Find the function or file name.  Don't accept a function name with ().
    اگر ((!یہاں && *p == NUL)
	    || (یہاں && *p != NUL)
	    || (bp->dbg_type == DBG_FUNC && لڑی_تلاش((حرف *)p, "()") != تہی))
    {
	semsg(_(e_invalid_دلیل_str), arg);
	واپس FAIL;
    }

    اگر (bp->dbg_type == DBG_FUNC)
	bp->dbg_name = vim_strsave(STRNCMP(p, "g:", 2) == 0 ? p + 2 : p);
    ورنہ اگر (یہاں)
	bp->dbg_name = vim_strsave(curbuf->b_ffname);
    ورنہ اگر (bp->dbg_type == DBG_اظہاریہ)
    {
	bp->dbg_name = vim_strsave(p);
	اگر (bp->dbg_name != تہی)
	    bp->dbg_val = eval_اظہاریہ_no_emsg(bp);
    }
    ورنہ
    {
	// Expand the file name in the same way as do_source().  This means
	// doing it twice, so that $DIR/file gets expanded when $DIR is
	// "~/dir".
	q = expand_env_save(p);
	اگر (q == تہی)
	    واپس FAIL;
	p = expand_env_save(q);
	vim_free(q);
	اگر (p == تہی)
	    واپس FAIL;
	اگر (*p != '*')
	{
	    bp->dbg_name = fix_fname(p);
	    vim_free(p);
	}
	ورنہ
	    bp->dbg_name = p;
    }

    اگر (bp->dbg_name == تہی)
	واپس FAIL;
    واپس OK;
}

/*
 * ":breakadd".  Also used for ":profile".
 */
    خالی
ex_breakadd(exarg_T *eap)
{
    ساخت debuggy *bp;
    char_u	*pat;
    garray_T	*gap;

    gap = &dbg_breakp;
#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (eap->cmdidx == CMD_profile)
	gap = &prof_ga;
#اگر_ختم

    اگر (dbg_parsearg(eap->arg, gap) != OK)
	واپس;

    bp = &DEBUGGY(gap, gap->ga_len);
    bp->dbg_forceit = eap->forceit;

    اگر (bp->dbg_type != DBG_اظہاریہ)
    {
	pat = file_pat_مقصد_reg_pat(bp->dbg_name, تہی, تہی, FALSE);
	اگر (pat != تہی)
	{
	    bp->dbg_prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    vim_free(pat);
	}
	اگر (pat == تہی || bp->dbg_prog == تہی)
	    vim_free(bp->dbg_name);
	ورنہ
	{
	    اگر (bp->dbg_lnum == 0)	// default line number is 1
		bp->dbg_lnum = 1;
#اگر_تعریف_شدہ FEAT_PROFILE
	    اگر (eap->cmdidx != CMD_profile)
#اگر_ختم
	    {
		DEBUGGY(gap, gap->ga_len).dbg_nr = ++آخری_breakp;
		++debug_tick;
	    }
	    ++gap->ga_len;
	    PROF_CLEAR_CACHE(gap);
	}
    }
    ورنہ
    {
	// DBG_EXPR
	DEBUGGY(gap, gap->ga_len++).dbg_nr = ++آخری_breakp;
	++debug_tick;
	اگر (gap == &dbg_breakp)
	    has_اظہاریہ_breakpoint = TRUE;
    }
}

/*
 * ":debuggreedy".
 */
    خالی
ex_debuggreedy(exarg_T *eap)
{
    اگر (eap->addr_گنتی == 0 || eap->line2 != 0)
	debug_greedy = TRUE;
    ورنہ
	debug_greedy = FALSE;
}

    ساکن خالی
update_has_اظہاریہ_breakpoint(خالی)
{
    صحیح i;

    has_اظہاریہ_breakpoint = FALSE;
    برائے (i = 0; i < dbg_breakp.ga_len; ++i)
	اگر (BREAKP(i).dbg_type == DBG_اظہاریہ)
	{
	    has_اظہاریہ_breakpoint = TRUE;
	    توڑو;
	}
}

/*
 * Return TRUE if there is any expression breakpoint.
 */
    صحیح
debug_has_اظہاریہ_breakpoint(خالی)
{
    واپس has_اظہاریہ_breakpoint;
}

/*
 * ":breakdel" and ":profdel".
 */
    خالی
ex_breakdel(exarg_T *eap)
{
    ساخت debuggy *bp, *bpi;
    صحیح		nr;
    صحیح		todel = -1;
    صحیح		del_all = FALSE;
    صحیح		i;
    linenr_T	بہترین_lnum = 0;
    garray_T	*gap;

    gap = &dbg_breakp;
    اگر (eap->cmdidx == CMD_profdel)
    {
#اگر_تعریف_شدہ FEAT_PROFILE
	gap = &prof_ga;
#ورنہ
	ex_ni(eap);
	واپس;
#اگر_ختم
    }

    اگر (vim_isdigit(*eap->arg))
    {
	// ":breakdel {nr}"
	nr = atol((حرف *)eap->arg);
	برائے (i = 0; i < gap->ga_len; ++i)
	    اگر (DEBUGGY(gap, i).dbg_nr == nr)
	    {
		todel = i;
		توڑو;
	    }
    }
    ورنہ اگر (*eap->arg == '*')
    {
	todel = 0;
	del_all = TRUE;
    }
    ورنہ
    {
	// ":breakdel {func|file|expr} [lnum] {name}"
	اگر (dbg_parsearg(eap->arg, gap) == FAIL)
	    واپس;
	bp = &DEBUGGY(gap, gap->ga_len);
	برائے (i = 0; i < gap->ga_len; ++i)
	{
	    bpi = &DEBUGGY(gap, i);
	    اگر (bp->dbg_type == bpi->dbg_type
		    && STRCMP(bp->dbg_name, bpi->dbg_name) == 0
		    && (bp->dbg_lnum == bpi->dbg_lnum
			|| (bp->dbg_lnum == 0
			    && (بہترین_lnum == 0
				|| bpi->dbg_lnum < بہترین_lnum))))
	    {
		todel = i;
		بہترین_lnum = bpi->dbg_lnum;
	    }
	}
	vim_free(bp->dbg_name);
    }

    اگر (todel < 0)
    {
	semsg(_(e_breakpoint_not_ملا_str), eap->arg);
	واپس;
    }

    جب (gap->ga_len > 0)
    {
	vim_free(DEBUGGY(gap, todel).dbg_name);
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (DEBUGGY(gap, todel).dbg_type == DBG_اظہاریہ
		&& DEBUGGY(gap, todel).dbg_val != تہی)
	    free_tv(DEBUGGY(gap, todel).dbg_val);
#اگر_ختم
	vim_regfree(DEBUGGY(gap, todel).dbg_prog);
	--gap->ga_len;
	اگر (todel < gap->ga_len)
	    mch_memmove(&DEBUGGY(gap, todel), &DEBUGGY(gap, todel + 1),
		    (gap->ga_len - todel) * حجم(ساخت debuggy));
#اگر_تعریف_شدہ FEAT_PROFILE
	اگر (eap->cmdidx == CMD_breakdel)
#اگر_ختم
	    ++debug_tick;
	اگر (!del_all)
	    توڑو;
    }
    PROF_CLEAR_CACHE(gap);

    // If all breakpoints were removed clear the array.
    اگر (gap->ga_len == 0)
	ga_clear(gap);
    اگر (gap == &dbg_breakp)
	update_has_اظہاریہ_breakpoint();
}

/*
 * ":breaklist".
 */
    خالی
ex_breaklist(exarg_T *eap غیر_مستعمل)
{
    ساخت debuggy *bp;
    صحیح		i;

    اگر (dbg_breakp.ga_len == 0)
    {
	پیغام(_("No breakpoints defined"));
	واپس;
    }

    برائے (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	bp = &BREAKP(i);
	اگر (bp->dbg_type == DBG_FILE)
	    home_replace(تہی, bp->dbg_name, NameBuff, MAXPATHL, TRUE);
	اگر (bp->dbg_type != DBG_اظہاریہ)
	    smsg(_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_type == DBG_FUNC ? bp->dbg_name : NameBuff,
		    (طویل)bp->dbg_lnum);
	ورنہ
	    smsg(_("%3d  expr %s"),
		    bp->dbg_nr, bp->dbg_name);
    }
}

/*
 * Find a breakpoint for a function or sourced file.
 * Returns line number at which to break; zero when no matching breakpoint.
 */
    linenr_T
dbg_ڈھونڈو_breakpoint(
    صحیح		file,	    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after)	    // after this line number
{
    واپس debuggy_ڈھونڈو(file, fname, after, &dbg_breakp, تہی);
}

#اگر defined(FEAT_PROFILE)
# اگر defined(PROF_CACHE_LOG)
ساکن صحیح گنتی_lookups[2];
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
	واپس debuggy_ڈھونڈو(file, fname, (linenr_T)0, &prof_ga, fp)
							     != (linenr_T)0;

    hash_T hash;
    اگر (hashp != تہی)
    {
	hash = *hashp;
	اگر (hash == 0)
	{
	    hash = hash_hash(fname);
	    *hashp = hash;
	}
    }
    ورنہ
	hash = hash_hash(fname);

    hashtab_T *ht = &prof_cache[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS];
    hashitem_T *hi = hash_lookup(ht, fname, hash);
    profentry_T *pe;
    اگر (HASHITEM_خالی_خانے(hi))
    {
	pe = alloc(offsetof(profentry_T, pen_name) + STRLEN(fname) + 1);
	اگر (pe == تہی)
	    واپس FALSE;
	STRCPY(pe->pen_name, fname);
	pe->pen_flags = 0;
	// run debuggy_find and capture return and forceit
	صحیح f;
	صحیح lnum = debuggy_ڈھونڈو(file, fname, (linenr_T)0, &prof_ga, &f);
	اگر (lnum)
	{
	    PEN_سیٹ_PROFILING(pe);
	    اگر (f)
		PEN_سیٹ_FORCEIT(pe);
	}
	hash_جوڑو_شے(ht, hi, pe->pen_name, hash);
# اگر defined(PROF_CACHE_LOG)
	ch_log(تہی, "has_profiling: %s %s forceit %s, profile %s",
	       file ? "file" : "func", fname,
	       PEN_IS_FORCEIT(pe) ? "true" : "false",
	       PEN_IS_PROFILING(pe) ? "true" : "false");
# endif
    }
    ورنہ
	pe = HI2PE(hi);
    اگر (fp)
	*fp = PEN_IS_FORCEIT(pe);
# اگر defined(PROF_CACHE_LOG)
    گنتی_lookups[file ? PROF_HTAB_FILES : PROF_HTAB_FUNCS]++;
# endif
    واپس PEN_IS_PROFILING(pe);
}

    ساکن خالی
prof_clear_cache(خالی)
{
    اگر (!prof_cache_initialized)
    {
	hash_init(&prof_cache[PROF_HTAB_FUNCS]);
	hash_init(&prof_cache[PROF_HTAB_FILES]);
	prof_cache_initialized = TRUE;
    }

    hashtab_T *ht;
    برائے (ht = &prof_cache[0]; ht < &prof_cache[N_PROF_HTAB]; ht++)
    {
	اگر (ht->ht_مستعمل > 0)
	{
# اگر defined(PROF_CACHE_LOG)
	    صحیح idx = ht == &prof_cache[PROF_HTAB_FUNCS]
		      ? PROF_HTAB_FUNCS : PROF_HTAB_FILES;
	    ch_log(تہی, "prof_clear_cache: %s, used: %ld, lookups: %d",
		   idx == PROF_HTAB_FUNCS ? "function" : "script",
		   ht->ht_مستعمل, گنتی_lookups[idx]);
	    گنتی_lookups[idx] = 0;
# endif
	    hash_clear_all(ht, offsetof(profentry_T, pen_name));
	    hash_init(ht);
	}
    }
}
#اگر_ختم

/*
 * Common code for dbg_find_breakpoint() and has_profiling().
 */
    ساکن linenr_T
debuggy_ڈھونڈو(
    صحیح		is_file,    // TRUE for a file, FALSE for a function
    char_u	*fname,	    // file or function name
    linenr_T	after,	    // after this line number
    garray_T	*gap,	    // either &dbg_breakp or &prof_ga
    صحیح		*fp)	    // if not NULL: return forceit
{
    ساخت debuggy *bp;
    صحیح		i;
    linenr_T	lnum = 0;
    char_u	*name = تہی;
    char_u	*short_name = fname;
    صحیح		پچھلا_got_int;

    // Return quickly when there are no breakpoints.
    اگر (gap->ga_len == 0)
	واپس (linenr_T)0;

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
	اگر (name != تہی)
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    برائے (i = 0; i < gap->ga_len; ++i)
    {
	// Skip entries that are not useful or are for a line that is beyond
	// an already found breakpoint.
	bp = &DEBUGGY(gap, i);
	اگر (((bp->dbg_type == DBG_FILE) == is_file
		    && bp->dbg_type != DBG_اظہاریہ && (
#اگر_تعریف_شدہ FEAT_PROFILE
		gap == &prof_ga ||
#اگر_ختم
		(bp->dbg_lnum > after && (lnum == 0 || bp->dbg_lnum < lnum)))))
	{
	    // Save the value of got_int and reset it.  We don't want a
	    // previous interruption cancel matching, only hitting CTRL-C
	    // while matching should abort it.
	    پچھلا_got_int = got_int;
	    got_int = FALSE;
	    اگر ((name != تہی
		   && vim_regexec_prog(&bp->dbg_prog, FALSE, name, (colnr_T)0))
		    || vim_regexec_prog(&bp->dbg_prog, FALSE,
						       short_name, (colnr_T)0))
	    {
		lnum = bp->dbg_lnum;
		اگر (fp != تہی)
		    *fp = bp->dbg_forceit;
	    }
	    got_int |= پچھلا_got_int;
#اگر_تعریف_شدہ FEAT_PROFILE
	    اگر (lnum && gap == &prof_ga)
		توڑو;
#اگر_ختم
	}
#اگر_تعریف_شدہ FEAT_EVAL
	ورنہ اگر (bp->dbg_type == DBG_اظہاریہ)
	{
	    typval_T *tv;
	    صحیح	      سطر = FALSE;

	    tv = eval_اظہاریہ_no_emsg(bp);
	    اگر (tv != تہی)
	    {
		اگر (bp->dbg_val == تہی)
		{
		    vim_free(debug_oldval);
		    debug_oldval = typval_tostring(تہی, TRUE);
		    bp->dbg_val = tv;
		    vim_free(debug_newval);
		    debug_newval = typval_tostring(bp->dbg_val, TRUE);
		    سطر = TRUE;
		}
		ورنہ
		{
		    // Use "==" instead of "is" for strings, that is what we
		    // always have done.
		    exprtype_T	type = tv->v_type == VAR_STRING
							? اظہاریہ_EQUAL : اظہاریہ_IS;

		    اگر (typval_compare(tv, bp->dbg_val, type, FALSE) == OK
			    && tv->vval.v_نمبر == FALSE)
		    {
			typval_T *v;

			سطر = TRUE;
			vim_free(debug_oldval);
			debug_oldval = typval_tostring(bp->dbg_val, TRUE);
			// Need to evaluate again, typval_compare() overwrites
			// "tv".
			v = eval_اظہاریہ_no_emsg(bp);
			vim_free(debug_newval);
			debug_newval = typval_tostring(v, TRUE);
			free_tv(bp->dbg_val);
			bp->dbg_val = v;
		    }
		    free_tv(tv);
		}
	    }
	    ورنہ اگر (bp->dbg_val != تہی)
	    {
		vim_free(debug_oldval);
		debug_oldval = typval_tostring(bp->dbg_val, TRUE);
		vim_free(debug_newval);
		debug_newval = typval_tostring(تہی, TRUE);
		free_tv(bp->dbg_val);
		bp->dbg_val = تہی;
		سطر = TRUE;
	    }

	    اگر (سطر)
	    {
		lnum = after > 0 ? after : 1;
		توڑو;
	    }
	}
#اگر_ختم
    }
    اگر (name != fname)
	vim_free(name);

    واپس lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    خالی
dbg_breakpoint(char_u *name, linenr_T lnum)
{
    // We need to check if this line is actually executed in do_one_cmd()
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#اگر_ختم
