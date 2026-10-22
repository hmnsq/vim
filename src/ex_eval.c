/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * ex_eval.c: functions for Ex command line for the +eval feature.
 */

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

ایستا نویسه	*get_end_emsg(cstack_T *cstack);

/*
 * Exception handling terms:
 *
 *	:try		":try" command		\
 *	    ...		try block		|
 *	:catch RE	":catch" command	|
 *	    ...		catch clause		|- try conditional
 *	:finally	":finally" command	|
 *	    ...		finally clause		|
 *	:endtry		":endtry" command	/
 *
 * The try conditional may have any number of catch clauses and at most one
 * finally clause.  A ":throw" command can be inside the try block, a catch
 * clause, the finally clause, or in a function called or script sourced from
 * there or even outside the try conditional.  Try conditionals may be nested.
 */

/*
 * Configuration whether an exception is thrown on error or interrupt.  When
 * the preprocessor macros below evaluate to FALSE, an error (did_emsg) or
 * interrupt (got_int) under an active try conditional terminates the script
 * after the non-active finally clauses of all active try conditionals have been
 * executed.  Otherwise, errors and/or interrupts are converted into catchable
 * exceptions (did_throw additionally set), which terminate the script only if
 * not caught.  For user exceptions, only did_throw is set.  (Note: got_int can
 * be set asynchronously afterwards by a SIGINT, so did_throw && got_int is not
 * a reliant test that the exception currently being thrown is an interrupt
 * exception.  Similarly, did_emsg can be set afterwards on an error in an
 * (unskipped) conditional command inside an inactive conditional, so did_throw
 * && did_emsg is not a reliant test that the exception currently being thrown
 * is an error exception.)  -  The macros can be defined as expressions checking
 * for a variable that is allowed to be changed during execution of a script.
 */
#اگر 0
// Expressions used for testing during the development phase.
# define THROW_ON_ERROR		(!eval_to_عدد("$VIMNOERRTHROW"))
# define THROW_ON_INTERRUPT	(!eval_to_عدد("$VIMNOINTTHROW"))
# define THROW_TEST
#وگرنه
// Values used for the Vim release.
# define THROW_ON_ERROR		TRUE
# define THROW_ON_ERROR_TRUE
# define THROW_ON_INTERRUPT	TRUE
# define THROW_ON_INTERRUPT_TRUE
#endif

/*
 * When several errors appear in a row, setting "force_abort" is delayed until
 * the failing command returned.  "cause_abort" is set to TRUE meanwhile, in
 * order to indicate that situation.  This is useful when "force_abort" was set
 * during execution of a function call from an expression: the aborting of the
 * expression evaluation is done without producing any error messages, but all
 * error messages on parsing errors during the expression evaluation are given
 * (even if a try conditional is active).
 */
ایستا صحیح cause_abort = FALSE;

/*
 * Return TRUE when immediately aborting on error, or when an interrupt
 * occurred or an exception was thrown but not caught.  Use for ":{range}call"
 * to check whether an aborted function that does not handle a range itself
 * should be called again for the next line in the range.  Also used for
 * cancelling expression evaluation after a function call caused an immediate
 * abort.  Note that the first emsg() call temporarily resets "force_abort"
 * until the throw point for error messages has been reached.  That is, during
 * cancellation of an expression evaluation after an aborting function call or
 * due to a parsing error, aborting() always returns the same value.
 * "got_int" is also set by calling interrupt().
 */
    صحیح
aborting(تهی)
{
    بازگشت (did_emsg && force_abort) || got_int || did_throw;
}

/*
 * The value of "force_abort" is temporarily reset by the first emsg() call
 * during an expression evaluation, and "cause_abort" is used instead.  It might
 * be necessary to restore "force_abort" even before the throw point for the
 * error message has been reached.  update_force_abort() should be called then.
 */
    تهی
update_force_abort(تهی)
{
    اگر (cause_abort)
	force_abort = TRUE;
}

/*
 * Return TRUE if a command with a subcommand resulting in "retcode" should
 * abort the script processing.  Can be used to suppress an autocommand after
 * execution of a failing subcommand as long as the error message has not been
 * displayed and actually caused the abortion.
 */
    صحیح
should_abort(صحیح retcode)
{
    بازگشت ((retcode == FAIL && trylevel != 0 && !emsg_silent) || aborting());
}

/*
 * Return TRUE if a function with the "abort" flag should not be considered
 * ended on an error.  This means that parsing commands is continued in order
 * to find finally clauses to be executed, and that some errors in skipped
 * commands are still reported.
 */
    صحیح
aborted_in_try(تهی)
{
    // This function is only called after an error.  In this case, "force_abort"
    // determines whether searching for finally clauses is necessary.
    بازگشت force_abort;
}

/*
 * cause_errthrow(): Cause a throw of an error exception if appropriate.
 * Return TRUE if the error message should not be displayed by emsg().
 * Sets "ignore", if the emsg() call should be ignored completely.
 *
 * When several messages appear in the same command, the first is usually the
 * most specific one and used as the exception value.  The "severe" flag can be
 * set to TRUE, if a later but severer message should be used instead.
 */
    صحیح
cause_errthrow(
    char_u	*mesg,
    صحیح		severe,
    صحیح		*ignore)
{
    msglist_T	*elem;
    msglist_T	**plist;

    /*
     * Do nothing when displaying the interrupt message or reporting an
     * uncaught exception (which has already been discarded then) at the top
     * level.  Also when no exception can be thrown.  The message will be
     * displayed by emsg().
     */
    اگر (suppress_errthrow)
	بازگشت FALSE;

    /*
     * If emsg() has not been called previously, temporarily reset
     * "force_abort" until the throw point for error messages has been
     * reached.  This ensures that aborting() returns the same value for all
     * errors that appear in the same command.  This means particularly that
     * for parsing errors during expression evaluation emsg() will be called
     * multiply, even when the expression is evaluated from a finally clause
     * that was activated due to an aborting error, interrupt, or exception.
     */
    اگر (!did_emsg)
    {
	cause_abort = force_abort;
	force_abort = FALSE;
    }

    /*
     * If no try conditional is active and no exception is being thrown and
     * there has not been an error in a try conditional or a throw so far, do
     * nothing (for compatibility of non-EH scripts).  The message will then
     * be displayed by emsg().  When ":silent!" was used and we are not
     * currently throwing an exception, do nothing.  The message text will
     * then be stored to v:errmsg by emsg() without displaying it.
     */
    اگر (((trylevel == 0 && !cause_abort) || emsg_silent) && !did_throw)
	بازگشت FALSE;

    /*
     * Ignore an interrupt message when inside a try conditional or when an
     * exception is being thrown or when an error in a try conditional or
     * throw has been detected previously.  This is important in order that an
     * interrupt exception is catchable by the innermost try conditional and
     * not replaced by an interrupt message error exception.
     */
    اگر (mesg == (char_u *)_(e_interrupted))
    {
	*ignore = TRUE;
	بازگشت TRUE;
    }

    /*
     * Ensure that all commands in nested function calls and sourced files
     * are aborted immediately.
     */
    cause_abort = TRUE;

    /*
     * When an exception is being thrown, some commands (like conditionals) are
     * not skipped.  Errors in those commands may affect what of the subsequent
     * commands are regarded part of catch and finally clauses.  Catching the
     * exception would then cause execution of commands not intended by the
     * user, who wouldn't even get aware of the problem.  Therefore, discard the
     * exception currently being thrown to prevent it from being caught.  Just
     * execute finally clauses and terminate.
     */
    اگر (did_throw)
    {
	// When discarding an interrupt exception, reset got_int to prevent the
	// same interrupt being converted to an exception again and discarding
	// the error exception we are about to throw here.
	اگر (current_exception->type == ET_INTERRUPT)
	    got_int = FALSE;
	discard_current_exception();
    }

#ifdef THROW_TEST
    اگر (!THROW_ON_ERROR)
    {
	/*
	 * Print error message immediately without searching for a matching
	 * catch clause; just finally clauses are executed before the script
	 * is terminated.
	 */
	بازگشت FALSE;
    }
    وگرنه
#endif
    {
	/*
	 * Prepare the throw of an error exception, so that everything will
	 * be aborted (except for executing finally clauses), until the error
	 * exception is caught; if still uncaught at the top level, the error
	 * message will be displayed and the script processing terminated
	 * then.  -  This function has no access to the conditional stack.
	 * Thus, the actual throw is made after the failing command has
	 * returned.  -  Throw only the first of several errors in a row, except
	 * a severe error is following.
	 */
	اگر (پیام_list != پوچ)
	{
	    plist = پیام_list;
	    وقتی (*plist != پوچ)
		plist = &(*plist)->next;

	    elem = ALLOC_CLEAR_ONE(msglist_T);
	    اگر (elem == پوچ)
	    {
		suppress_errthrow = TRUE;
		emsg(_(e_out_of_memory));
	    }
	    وگرنه
	    {
		elem->پیام = (نویسه *)vim_strsave(mesg);
		اگر (elem->پیام == پوچ)
		{
		    vim_free(elem);
		    suppress_errthrow = TRUE;
		    emsg(_(e_out_of_memory));
		}
		وگرنه
		{
		    elem->next = پوچ;
		    elem->throw_پیام = پوچ;
		    *plist = elem;
		    اگر (plist == پیام_list || severe)
		    {
			نویسه	    *tmsg;

			// Skip the extra "Vim " prefix for message "E457".
			tmsg = elem->پیام;
			اگر (STRNCMP(tmsg, "Vim E", 5) == 0
				&& VIM_ISDIGIT(tmsg[5])
				&& VIM_ISDIGIT(tmsg[6])
				&& VIM_ISDIGIT(tmsg[7])
				&& tmsg[8] == ':'
				&& tmsg[9] == ' ')
			    (*پیام_list)->throw_پیام = &tmsg[4];
			وگرنه
			    (*پیام_list)->throw_پیام = tmsg;
		    }

		    // Get the source name and lnum now, it may change before
		    // reaching do_errthrow().
		    elem->sfile = estack_sfile(ESTACK_NONE);
		    elem->slnum = SOURCING_LNUM;
		    elem->پیام_compiling = estack_compiling;
		}
	    }
	}
	بازگشت TRUE;
    }
}

/*
 * Free a "msg_list" and the messages it contains.
 */
    ایستا تهی
free_msglist(msglist_T *l)
{
    msglist_T  *messages, *next;

    messages = l;
    وقتی (messages != پوچ)
    {
	next = messages->next;
	vim_free(messages->پیام);
	vim_free(messages->sfile);
	vim_free(messages);
	messages = next;
    }
}

/*
 * Free global "*msg_list" and the messages it contains, then set "*msg_list"
 * to NULL.
 */
    تهی
free_global_msglist(تهی)
{
    free_msglist(*پیام_list);
    *پیام_list = پوچ;
}

/*
 * Throw the message specified in the call to cause_errthrow() above as an
 * error exception.  If cstack is NULL, postpone the throw until do_cmdline()
 * has returned (see do_one_cmd()).
 */
    تهی
do_errthrow(cstack_T *cstack, char_u *cmdname)
{
    /*
     * Ensure that all commands in nested function calls and sourced files
     * are aborted immediately.
     */
    اگر (cause_abort)
    {
	cause_abort = FALSE;
	force_abort = TRUE;
    }

    // If no exception is to be thrown or the conversion should be done after
    // returning to a previous invocation of do_one_cmd(), do nothing.
    اگر (پیام_list == پوچ || *پیام_list == پوچ)
	بازگشت;

    اگر (throw_exception(*پیام_list, ET_ERROR, cmdname) == FAIL)
	free_msglist(*پیام_list);
    وگرنه
    {
	اگر (cstack != پوچ)
	    do_throw(cstack);
	وگرنه
	    need_rethrow = TRUE;
    }
    *پیام_list = پوچ;
}

/*
 * do_intthrow(): Replace the current exception by an interrupt or interrupt
 * exception if appropriate.  Return TRUE if the current exception is discarded,
 * FALSE otherwise.
 */
    صحیح
do_intthrow(cstack_T *cstack)
{
    /*
     * If no interrupt occurred or no try conditional is active and no exception
     * is being thrown, do nothing (for compatibility of non-EH scripts).
     */
    اگر (!got_int || (trylevel == 0 && !did_throw))
	بازگشت FALSE;

#ifdef THROW_TEST	// avoid warning for condition always true
    اگر (!THROW_ON_INTERRUPT)
    {
	/*
	 * The interrupt aborts everything except for executing finally clauses.
	 * Discard any user or error or interrupt exception currently being
	 * thrown.
	 */
	اگر (did_throw)
	    discard_current_exception();
    }
    وگرنه
#endif
    {
	/*
	 * Throw an interrupt exception, so that everything will be aborted
	 * (except for executing finally clauses), until the interrupt exception
	 * is caught; if still uncaught at the top level, the script processing
	 * will be terminated then.  -  If an interrupt exception is already
	 * being thrown, do nothing.
	 *
	 */
	اگر (did_throw)
	{
	    اگر (current_exception->type == ET_INTERRUPT)
		بازگشت FALSE;

	    // An interrupt exception replaces any user or error exception.
	    discard_current_exception();
	}
	اگر (throw_exception("Vim:Interrupt", ET_INTERRUPT, پوچ) != FAIL)
	    do_throw(cstack);
    }

    بازگشت TRUE;
}

/*
 * Get an exception message that is to be stored in current_exception->value.
 */
    نویسه *
get_exception_string(
    تهی	*مقدار,
    except_type_T type,
    char_u	*cmdname,
    صحیح		*should_free)
{
    نویسه	*ret;
    نویسه	*mesg;
    صحیح		cmdlen;
    نویسه	*p, *val;

    اگر (type == ET_ERROR)
    {
	*should_free = TRUE;
	mesg = ((msglist_T *)مقدار)->throw_پیام;
	اگر (cmdname != پوچ && *cmdname != NUL)
	{
	    cmdlen = (صحیح)STRLEN(cmdname);
	    ret = (نویسه *)vim_strnsave((char_u *)"Vim(",
						4 + cmdlen + 2 + STRLEN(mesg));
	    اگر (ret == پوچ)
		بازگشت ret;
	    STRCPY(&ret[4], cmdname);
	    STRCPY(&ret[4 + cmdlen], "):");
	    val = ret + 4 + cmdlen + 2;
	}
	وگرنه
	{
	    ret = (نویسه *)vim_strnsave((char_u *)"Vim:", 4 + STRLEN(mesg));
	    اگر (ret == پوچ)
		بازگشت ret;
	    val = ret + 4;
	}

	// msg_add_fname may have been used to prefix the message with a file
	// name in quotes.  In the exception value, put the file name in
	// parentheses and move it to the end.
	برای (p = mesg; ; p++)
	{
	    اگر (*p == NUL
		    || (*p == 'E'
			&& VIM_ISDIGIT(p[1])
			&& (p[2] == ':'
			    || (VIM_ISDIGIT(p[2])
				&& (p[3] == ':'
				    || (VIM_ISDIGIT(p[3])
					&& p[4] == ':'))))))
	    {
		اگر (*p == NUL || p == mesg)
		    STRCAT(val, mesg);  // 'E123' missing or at beginning
		وگرنه
		{
		    // '"filename" E123: message text'
		    اگر (mesg[0] != '"' || p-2 < &mesg[1] ||
			    p[-2] != '"' || p[-1] != ' ')
			// "E123:" is part of the file name.
			ادامه;

		    STRCAT(val, p);
		    p[-2] = NUL;
		    sprintf((نویسه *)(val + STRLEN(p)), " (%s)", &mesg[1]);
		    p[-2] = '"';
		}
		بشکن;
	    }
	}
    }
    وگرنه
    {
	*should_free = FALSE;
	ret = مقدار;
    }

    بازگشت ret;
}


/*
 * Throw a new exception.  Return FAIL when out of memory or it was tried to
 * throw an illegal user exception.  "value" is the exception string for a
 * user or interrupt exception, or points to a message list in case of an
 * error exception.
 */
    صحیح
throw_exception(تهی *مقدار, except_type_T type, char_u *cmdname)
{
    except_T	*excp;
    صحیح		should_free;

    /*
     * Disallow faking Interrupt or error exceptions as user exceptions.  They
     * would be treated differently from real interrupt or error exceptions
     * when no active try block is found, see do_cmdline().
     */
    اگر (type == ET_USER)
    {
	اگر (STRNCMP((char_u *)مقدار, "Vim", 3) == 0
		&& (((char_u *)مقدار)[3] == NUL || ((char_u *)مقدار)[3] == ':'
		    || ((char_u *)مقدار)[3] == '('))
	{
	    emsg(_(e_cannot_throw_exceptions_with_vim_prefix));
	    برو fail;
	}
    }

    excp = ALLOC_ONE(except_T);
    اگر (excp == پوچ)
	برو nomem;

    اگر (type == ET_ERROR)
	// Store the original message and prefix the exception value with
	// "Vim:" or, if a command name is given, "Vim(cmdname):".
	excp->messages = (msglist_T *)مقدار;

    excp->مقدار = get_exception_string(مقدار, type, cmdname, &should_free);
    اگر (excp->مقدار == پوچ && should_free)
	برو nomem;

    excp->type = type;
    اگر (type == ET_ERROR && ((msglist_T *)مقدار)->sfile != پوچ)
    {
	msglist_T *entry = (msglist_T *)مقدار;

	excp->throw_name = entry->sfile;
	entry->sfile = پوچ;
	excp->throw_lnum = entry->slnum;
    }
    وگرنه
    {
	excp->throw_name = estack_sfile(ESTACK_NONE);
	اگر (excp->throw_name == پوچ)
	    excp->throw_name = vim_strsave((char_u *)"");
	اگر (excp->throw_name == پوچ)
	{
	    اگر (should_free)
		vim_free(excp->مقدار);
	    برو nomem;
	}
	excp->throw_lnum = SOURCING_LNUM;
    }

    excp->stacktrace = stacktrace_create();
    اگر (excp->stacktrace != پوچ)
	excp->stacktrace->lv_refcount = 1;

    اگر (p_verbose >= 13 || debug_break_level > 0)
    {
	صحیح	save_پیام_silent = پیام_silent;

	اگر (debug_break_level > 0)
	    پیام_silent = FALSE;		// display messages
	وگرنه
	    verbose_enter();
	++no_wait_return;
	اگر (debug_break_level > 0 || *p_vfile == NUL)
	    پیام_scroll = TRUE;	    // always scroll up, don't overwrite

	smsg(_("Exception thrown: %s"), excp->مقدار);
	پیام_puts("\n");   // don't overwrite this either

	اگر (debug_break_level > 0 || *p_vfile == NUL)
	    cmdline_row = پیام_row;
	--no_wait_return;
	اگر (debug_break_level > 0)
	    پیام_silent = save_پیام_silent;
	وگرنه
	    verbose_leave();
    }

    current_exception = excp;
    بازگشت OK;

nomem:
    vim_free(excp);
    suppress_errthrow = TRUE;
    emsg(_(e_out_of_memory));
fail:
    current_exception = پوچ;
    بازگشت FAIL;
}

/*
 * Discard an exception.  "was_finished" is set when the exception has been
 * caught and the catch clause has been ended normally.
 */
    ایستا تهی
discard_exception(except_T *excp, صحیح was_finished)
{
    char_u		*saved_IObuff;

    اگر (current_exception == excp)
	current_exception = پوچ;
    اگر (excp == پوچ)
    {
	internal_error("discard_exception()");
	بازگشت;
    }

    اگر (p_verbose >= 13 || debug_break_level > 0)
    {
	صحیح	save_پیام_silent = پیام_silent;

	saved_IObuff = vim_strsave(IObuff);
	اگر (debug_break_level > 0)
	    پیام_silent = FALSE;		// display messages
	وگرنه
	    verbose_enter();
	++no_wait_return;
	اگر (debug_break_level > 0 || *p_vfile == NUL)
	    پیام_scroll = TRUE;	    // always scroll up, don't overwrite
	smsg(was_finished
		    ? _("Exception finished: %s")
		    : _("Exception discarded: %s"),
		excp->مقدار);
	پیام_puts("\n");   // don't overwrite this either
	اگر (debug_break_level > 0 || *p_vfile == NUL)
	    cmdline_row = پیام_row;
	--no_wait_return;
	اگر (debug_break_level > 0)
	    پیام_silent = save_پیام_silent;
	وگرنه
	    verbose_leave();
	STRCPY(IObuff, saved_IObuff);
	vim_free(saved_IObuff);
    }
    اگر (excp->type != ET_INTERRUPT)
	vim_free(excp->مقدار);
    اگر (excp->type == ET_ERROR)
	free_msglist(excp->messages);
    vim_free(excp->throw_name);
    list_unref(excp->stacktrace);
    vim_free(excp);
}

/*
 * Discard the exception currently being thrown.
 */
    تهی
discard_current_exception(تهی)
{
    اگر (current_exception != پوچ)
	discard_exception(current_exception, FALSE);
    did_throw = FALSE;
    need_rethrow = FALSE;
}

/*
 * Put an exception on the caught stack.
 */
    تهی
catch_exception(except_T *excp)
{
    excp->caught = caught_پشته;
    caught_پشته = excp;
    set_vim_var_string(VV_EXCEPTION, (char_u *)excp->مقدار, -1);
    set_vim_var_list(VV_STACKTRACE, excp->stacktrace);
    اگر (*excp->throw_name != NUL)
    {
	size_t	IObufflen;

	اگر (excp->throw_lnum != 0)
	    IObufflen = vim_snprintf_safelen((نویسه *)IObuff, IOSIZE, _("%s, line %ld"),
				    excp->throw_name, (long)excp->throw_lnum);
	وگرنه
	    IObufflen = vim_snprintf_safelen((نویسه *)IObuff, IOSIZE, "%s", excp->throw_name);
	set_vim_var_string(VV_THROWPOINT, IObuff, (صحیح)IObufflen);
    }
    وگرنه
	// throw_name not set on an exception from a command that was typed.
	set_vim_var_string(VV_THROWPOINT, پوچ, -1);

    اگر (p_verbose >= 13 || debug_break_level > 0)
    {
	صحیح	save_پیام_silent = پیام_silent;

	اگر (debug_break_level > 0)
	    پیام_silent = FALSE;		// display messages
	وگرنه
	    verbose_enter();
	++no_wait_return;
	اگر (debug_break_level > 0 || *p_vfile == NUL)
	    پیام_scroll = TRUE;	    // always scroll up, don't overwrite

	smsg(_("Exception caught: %s"), excp->مقدار);
	پیام_puts("\n");   // don't overwrite this either

	اگر (debug_break_level > 0 || *p_vfile == NUL)
	    cmdline_row = پیام_row;
	--no_wait_return;
	اگر (debug_break_level > 0)
	    پیام_silent = save_پیام_silent;
	وگرنه
	    verbose_leave();
    }
}

/*
 * Remove an exception from the caught stack.
 */
    تهی
finish_exception(except_T *excp)
{
    اگر (excp != caught_پشته)
	internal_error("finish_exception()");
    caught_پشته = caught_پشته->caught;
    اگر (caught_پشته != پوچ)
    {
	set_vim_var_string(VV_EXCEPTION, (char_u *)caught_پشته->مقدار, -1);
	set_vim_var_list(VV_STACKTRACE, caught_پشته->stacktrace);
	اگر (*caught_پشته->throw_name != NUL)
	{
	    size_t  IObufflen;

	    اگر (caught_پشته->throw_lnum != 0)
		IObufflen = vim_snprintf_safelen((نویسه *)IObuff, IOSIZE,
			_("%s, line %ld"), caught_پشته->throw_name,
			(long)caught_پشته->throw_lnum);
	    وگرنه
		IObufflen = vim_snprintf_safelen((نویسه *)IObuff, IOSIZE, "%s",
						    caught_پشته->throw_name);
	    set_vim_var_string(VV_THROWPOINT, IObuff, (صحیح)IObufflen);
	}
	وگرنه
	    // throw_name not set on an exception from a command that was
	    // typed.
	    set_vim_var_string(VV_THROWPOINT, پوچ, -1);
    }
    وگرنه
    {
	set_vim_var_string(VV_EXCEPTION, پوچ, -1);
	set_vim_var_string(VV_THROWPOINT, پوچ, -1);
	set_vim_var_list(VV_STACKTRACE, پوچ);
    }

    // Discard the exception, but use the finish message for 'verbose'.
    discard_exception(excp, TRUE);
}

/*
 * Save the current exception state in "estate"
 */
    تهی
exception_state_save(exception_state_T *estate)
{
    estate->estate_current_exception = current_exception;
    estate->estate_did_throw = did_throw;
    estate->estate_need_rethrow = need_rethrow;
    estate->estate_trylevel = trylevel;
    estate->estate_did_emsg = did_emsg;
}

/*
 * Restore the current exception state from "estate"
 */
    تهی
exception_state_restore(exception_state_T *estate)
{
    // Handle any outstanding exceptions before restoring the state
    اگر (did_throw)
	handle_did_throw();
    current_exception = estate->estate_current_exception;
    did_throw = estate->estate_did_throw;
    need_rethrow = estate->estate_need_rethrow;
    trylevel = estate->estate_trylevel;
    did_emsg = estate->estate_did_emsg;
}

/*
 * Clear the current exception state
 */
    تهی
exception_state_clear(تهی)
{
    current_exception = پوچ;
    did_throw = FALSE;
    need_rethrow = FALSE;
    trylevel = 0;
    did_emsg = 0;
}

/*
 * Flags specifying the message displayed by report_pending.
 */
#define RP_MAKE		0
#define RP_RESUME	1
#define RP_DISCARD	2

/*
 * Report information about something pending in a finally clause if required by
 * the 'verbose' option or when debugging.  "action" tells whether something is
 * made pending or something pending is resumed or discarded.  "pending" tells
 * what is pending.  "value" specifies the return value for a pending ":return"
 * or the exception value for a pending exception.
 */
    ایستا تهی
report_pending(صحیح action, صحیح pending, تهی *مقدار)
{
    نویسه	*mesg;
    نویسه	*s;
    صحیح		save_پیام_silent;


    انتخاب (action)
    {
	حالت RP_MAKE:
	    mesg = _("%s made pending");
	    بشکن;
	حالت RP_RESUME:
	    mesg = _("%s resumed");
	    بشکن;
	// case RP_DISCARD:
	پیشفرض:
	    mesg = _("%s discarded");
	    بشکن;
    }

    انتخاب (pending)
    {
	حالت CSTP_NONE:
	    بازگشت;

	حالت CSTP_CONTINUE:
	    s = ":continue";
	    بشکن;
	حالت CSTP_BREAK:
	    s = ":break";
	    بشکن;
	حالت CSTP_FINISH:
	    s = ":finish";
	    بشکن;
	حالت CSTP_RETURN:
	    // ":return" command producing value, allocated
	    s = (نویسه *)get_return_cmd(مقدار);
	    بشکن;

	پیشفرض:
	    اگر (pending & CSTP_THROW)
	    {
		vim_snprintf((نویسه *)IObuff, IOSIZE, mesg, _("Exception"));
		mesg = (نویسه *)vim_strnsave(IObuff, STRLEN(IObuff) + 4);
		STRCAT(mesg, ": %s");
		s = (نویسه *)((except_T *)مقدار)->مقدار;
	    }
	    وگرنه اگر ((pending & CSTP_ERROR) && (pending & CSTP_INTERRUPT))
		s = _("Error and interrupt");
	    وگرنه اگر (pending & CSTP_ERROR)
		s = _("Error");
	    وگرنه // if (pending & CSTP_INTERRUPT)
		s = _("Interrupt");
    }

    save_پیام_silent = پیام_silent;
    اگر (debug_break_level > 0)
	پیام_silent = FALSE;	// display messages
    ++no_wait_return;
    پیام_scroll = TRUE;		// always scroll up, don't overwrite
    smsg(mesg, s);
    پیام_puts("\n");   // don't overwrite this either
    cmdline_row = پیام_row;
    --no_wait_return;
    اگر (debug_break_level > 0)
	پیام_silent = save_پیام_silent;

    اگر (pending == CSTP_RETURN)
	vim_free(s);
    وگرنه اگر (pending & CSTP_THROW)
	vim_free(mesg);
}

/*
 * If something is made pending in a finally clause, report it if required by
 * the 'verbose' option or when debugging.
 */
    تهی
report_make_pending(صحیح pending, تهی *مقدار)
{
    اگر (p_verbose >= 14 || debug_break_level > 0)
    {
	اگر (debug_break_level <= 0)
	    verbose_enter();
	report_pending(RP_MAKE, pending, مقدار);
	اگر (debug_break_level <= 0)
	    verbose_leave();
    }
}

/*
 * If something pending in a finally clause is resumed at the ":endtry", report
 * it if required by the 'verbose' option or when debugging.
 */
    ایستا تهی
report_resume_pending(صحیح pending, تهی *مقدار)
{
    اگر (p_verbose >= 14 || debug_break_level > 0)
    {
	اگر (debug_break_level <= 0)
	    verbose_enter();
	report_pending(RP_RESUME, pending, مقدار);
	اگر (debug_break_level <= 0)
	    verbose_leave();
    }
}

/*
 * If something pending in a finally clause is discarded, report it if required
 * by the 'verbose' option or when debugging.
 */
    ایستا تهی
report_discard_pending(صحیح pending, تهی *مقدار)
{
    اگر (p_verbose >= 14 || debug_break_level > 0)
    {
	اگر (debug_break_level <= 0)
	    verbose_enter();
	report_pending(RP_DISCARD, pending, مقدار);
	اگر (debug_break_level <= 0)
	    verbose_leave();
    }
}

/*
 * Return TRUE if "arg" is only a variable, register, environment variable,
 * option name or string.
 */
    صحیح
cmd_is_name_only(char_u *arg)
{
    char_u  *p = arg;
    char_u  *alias = پوچ;
    صحیح	    name_only = FALSE;

    اگر (*p == '@')
    {
	++p;
	اگر (*p != NUL)
	    ++p;
    }
    وگرنه اگر (*p == '\'' || *p == '"')
    {
	صحیح	    r;

	اگر (*p == '"')
	    r = eval_string(&p, پوچ, FALSE, FALSE);
	وگرنه
	    r = eval_lit_string(&p, پوچ, FALSE, FALSE);
	اگر (r == FAIL)
	    بازگشت FALSE;
    }
    وگرنه
    {
	اگر (*p == '&')
	{
	    ++p;
	    اگر (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
		p += 2;
	}
	وگرنه اگر (*p == '$')
	    ++p;
	(تهی)get_name_len(&p, &alias, FALSE, FALSE);
    }
    name_only = ends_excmd2(arg, skipwhite(p));
    vim_free(alias);
    بازگشت name_only;
}

/*
 * ":eval".
 */
    تهی
ex_eval(exarg_T *eap)
{
    typval_T	tv;
    evalarg_T	evalarg;
    صحیح		name_only = FALSE;
    long	lnum = SOURCING_LNUM;

    اگر (in_vim9script())
	name_only = cmd_is_name_only(eap->arg);

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    اگر (eval0(eap->arg, &tv, eap, &evalarg) == OK)
    {
	clear_tv(&tv);
	اگر (in_vim9script() && name_only
		&& (evalarg.eval_tofree == پوچ
		    || ends_excmd2(evalarg.eval_tofree,
					      skipwhite(evalarg.eval_tofree))))
	{
	    SOURCING_LNUM = lnum;
	    semsg(_(e_expression_without_effect_str), eap->arg);
	}
    }

    clear_evalarg(&evalarg, eap);
}

/*
 * Start a new scope/block.  Caller should have checked that cs_idx is not
 * exceeding CSTACK_LEN.
 */
    ایستا تهی
enter_block(cstack_T *cstack)
{
    ++cstack->cs_idx;
    اگر (in_vim9script() && current_sctx.sc_sid > 0)
    {
	scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	cstack->cs_script_var_len[cstack->cs_idx] = si->sn_var_vals.ga_len;
	cstack->cs_block_id[cstack->cs_idx] = ++si->sn_last_block_id;
	si->sn_current_block_id = si->sn_last_block_id;
    }
    وگرنه
    {
	// Just in case in_vim9script() does not return the same value when the
	// block ends.
	cstack->cs_script_var_len[cstack->cs_idx] = 0;
	cstack->cs_block_id[cstack->cs_idx] = 0;
    }
}

    ایستا تهی
leave_block(cstack_T *cstack)
{
    اگر (in_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	scriptitem_T	*si = SCRIPT_ITEM(current_sctx.sc_sid);
	صحیح		i;
	صحیح		func_defined =
			       cstack->cs_flags[cstack->cs_idx] & CSF_FUNC_DEF;

	برای (i = cstack->cs_script_var_len[cstack->cs_idx];
					       i < si->sn_var_vals.ga_len; ++i)
	{
	    svar_T	*sv = ((svar_T *)si->sn_var_vals.ga_data) + i;

	    // sv_name is set to NULL if it was already removed.  This happens
	    // when it was defined in an inner block and no functions were
	    // defined there.
	    اگر (sv->sv_name != پوچ)
		// Remove a variable declared inside the block, if it still
		// exists, from sn_vars and move the value into sn_all_vars
		// if "func_defined" is non-zero.
		hide_script_var(si, i, func_defined);
	}

	اگر (cstack->cs_idx == 0)
	    si->sn_current_block_id = 0;
	وگرنه
	    si->sn_current_block_id = cstack->cs_block_id[cstack->cs_idx - 1];
    }
    --cstack->cs_idx;
}

/*
 * ":if".
 */
    تهی
ex_if(exarg_T *eap)
{
    صحیح		error;
    صحیح		skip;
    صحیح		result;
    cstack_T	*cstack = eap->cstack;

    اگر (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_if_nesting_too_deep);
    وگرنه
    {
	enter_block(cstack);
	cstack->cs_flags[cstack->cs_idx] = 0;

	/*
	 * Don't do something after an error, interrupt, or throw, or when
	 * there is a surrounding conditional and it was not active.
	 */
	skip = did_emsg || got_int || did_throw || (cstack->cs_idx > 0
		&& !(cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE));

	result = eval_to_bool(eap->arg, &error, eap, skip, FALSE);

	اگر (!skip && !error)
	{
	    اگر (result)
		cstack->cs_flags[cstack->cs_idx] = CSF_ACTIVE | CSF_TRUE;
	}
	وگرنه
	    // set TRUE, so this conditional will never get active
	    cstack->cs_flags[cstack->cs_idx] = CSF_TRUE;
    }
}

/*
 * ":endif".
 */
    تهی
ex_endif(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;

    اگر (cmdmod_error(FALSE))
	بازگشت;
    did_endif = TRUE;
    اگر (cstack->cs_idx < 0
	    || (cstack->cs_flags[cstack->cs_idx]
				& (CSF_WHILE | CSF_FOR | CSF_TRY | CSF_BLOCK)))
	eap->errmsg = _(e_endif_without_if);
    وگرنه
    {
	/*
	 * When debugging or a breakpoint was encountered, display the debug
	 * prompt (if not already done).  This shows the user that an ":endif"
	 * is executed when the ":if" or a previous ":elseif" was not TRUE.
	 * Handle a ">quit" debug command as if an interrupt had occurred before
	 * the ":endif".  That is, throw an interrupt exception if appropriate.
	 * Doing this here prevents an exception for a parsing error being
	 * discarded by throwing the interrupt exception later on.
	 */
	اگر (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRUE)
						    && dbg_check_skipped(eap))
	    (تهی)do_intthrow(cstack);

	leave_block(cstack);
    }
}

/*
 * ":else" and ":elseif".
 */
    تهی
ex_else(exarg_T *eap)
{
    صحیح		error;
    صحیح		skip;
    صحیح		result;
    cstack_T	*cstack = eap->cstack;

    /*
     * Don't do something after an error, interrupt, or throw, or when there is
     * a surrounding conditional and it was not active.
     */
    skip = did_emsg || got_int || did_throw || (cstack->cs_idx > 0
	    && !(cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE));

    اگر (cstack->cs_idx < 0
	    || (cstack->cs_flags[cstack->cs_idx]
				& (CSF_WHILE | CSF_FOR | CSF_TRY | CSF_BLOCK)))
    {
	اگر (eap->cmdidx == CMD_else)
	{
	    eap->errmsg = _(e_else_without_if);
	    بازگشت;
	}
	eap->errmsg = _(e_elseif_without_if);
	skip = TRUE;
    }
    وگرنه اگر (cstack->cs_flags[cstack->cs_idx] & CSF_ELSE)
    {
	اگر (eap->cmdidx == CMD_else)
	{
	    eap->errmsg = _(e_multiple_else);
	    بازگشت;
	}
	eap->errmsg = _(e_elseif_after_else);
	skip = TRUE;
    }

    اگر (cstack->cs_idx >= 0)
    {
	// Variables declared in the previous block can no longer be
	// used.  Needs to be done before setting "cs_flags".
	leave_block(cstack);
	enter_block(cstack);
    }

    // if skipping or the ":if" was TRUE, reset ACTIVE, otherwise set it
    اگر (skip || cstack->cs_flags[cstack->cs_idx] & CSF_TRUE)
    {
	اگر (eap->errmsg == پوچ)
	    cstack->cs_flags[cstack->cs_idx] = CSF_TRUE;
	skip = TRUE;	// don't evaluate an ":elseif"
    }
    وگرنه
	cstack->cs_flags[cstack->cs_idx] = CSF_ACTIVE;

    /*
     * When debugging or a breakpoint was encountered, display the debug prompt
     * (if not already done).  This shows the user that an ":else" or ":elseif"
     * is executed when the ":if" or previous ":elseif" was not TRUE.  Handle
     * a ">quit" debug command as if an interrupt had occurred before the
     * ":else" or ":elseif".  That is, set "skip" and throw an interrupt
     * exception if appropriate.  Doing this here prevents that an exception
     * for a parsing errors is discarded when throwing the interrupt exception
     * later on.
     */
    اگر (!skip && dbg_check_skipped(eap) && got_int)
    {
	(تهی)do_intthrow(cstack);
	skip = TRUE;
    }

    اگر (eap->cmdidx == CMD_elseif)
    {
	// When skipping we ignore most errors, but a missing expression is
	// wrong, perhaps it should have been "else".
	// A double quote here is the start of a string, not a comment.
	اگر (skip && *eap->arg != '"' && ends_excmd(*eap->arg))
	    semsg(_(e_invalid_expression_str), eap->arg);
	وگرنه
	    result = eval_to_bool(eap->arg, &error, eap, skip, FALSE);

	// When throwing error exceptions, we want to throw always the first
	// of several errors in a row.  This is what actually happens when
	// a conditional error was detected above and there is another failure
	// when parsing the expression.  Since the skip flag is set in this
	// case, the parsing error will be ignored by emsg().
	اگر (!skip && !error)
	{
	    اگر (result)
		cstack->cs_flags[cstack->cs_idx] = CSF_ACTIVE | CSF_TRUE;
	    وگرنه
		cstack->cs_flags[cstack->cs_idx] = 0;
	}
	وگرنه اگر (eap->errmsg == پوچ)
	    // set TRUE, so this conditional will never get active
	    cstack->cs_flags[cstack->cs_idx] = CSF_TRUE;
    }
    وگرنه
	cstack->cs_flags[cstack->cs_idx] |= CSF_ELSE;
}

/*
 * Handle ":while" and ":for".
 */
    تهی
ex_while(exarg_T *eap)
{
    صحیح		error;
    صحیح		skip;
    صحیح		result;
    cstack_T	*cstack = eap->cstack;
    صحیح		prev_cs_flags = 0;

    اگر (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_while_for_nesting_too_deep);
    وگرنه
    {
	/*
	 * The loop flag is set when we have jumped back from the matching
	 * ":endwhile" or ":endfor".  When not set, need to initialise this
	 * cstack entry.
	 */
	اگر ((cstack->cs_lflags & CSL_HAD_LOOP) == 0)
	{
	    enter_block(cstack);
	    ++cstack->cs_looplevel;
	    cstack->cs_سطر[cstack->cs_idx] = -1;
	}
	وگرنه
	{
	    اگر (in_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid))
	    {
		scriptitem_T	*si = SCRIPT_ITEM(current_sctx.sc_sid);
		صحیح		i;
		صحیح		first;
		صحیح		func_defined = cstack->cs_flags[cstack->cs_idx]
								& CSF_FUNC_DEF;

		// Any variables defined in the previous round are no longer
		// visible.  Keep the first one for ":for", it is the loop
		// variable that we reuse every time around.
		// Do this backwards, so that vars defined in a later round are
		// found first.
		first = cstack->cs_script_var_len[cstack->cs_idx];
		اگر (eap->cmdidx == CMD_for)
		{
		    forinfo_T	*fi = cstack->cs_forinfo[cstack->cs_idx];

		    first += fi == پوچ || fi->fi_varcount == 0
							 ? 1 : fi->fi_varcount;
		}
		برای (i = si->sn_var_vals.ga_len - 1; i >= first; --i)
		{
		    svar_T	*sv = ((svar_T *)si->sn_var_vals.ga_data) + i;

		    // sv_name is set to NULL if it was already removed.  This
		    // happens when it was defined in an inner block and no
		    // functions were defined there.
		    اگر (sv->sv_name != پوچ)
			// Remove a variable declared inside the block, if it
			// still exists, from sn_vars.
			hide_script_var(si, i, func_defined);
		}

		// Start a new block ID, so that variables defined inside the
		// loop are created new and not shared with the previous loop.
		// Matters when used in a closure.
		cstack->cs_block_id[cstack->cs_idx] = ++si->sn_last_block_id;
		si->sn_current_block_id = si->sn_last_block_id;
	    }
	}
	prev_cs_flags = cstack->cs_flags[cstack->cs_idx];
	cstack->cs_flags[cstack->cs_idx] =
			       eap->cmdidx == CMD_while ? CSF_WHILE : CSF_FOR;

	/*
	 * Don't do something after an error, interrupt, or throw, or when
	 * there is a surrounding conditional and it was not active.
	 */
	skip = did_emsg || got_int || did_throw || (cstack->cs_idx > 0
		&& !(cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE));
	اگر (eap->cmdidx == CMD_while)
	{
	    /*
	     * ":while bool-expr"
	     */
	    result = eval_to_bool(eap->arg, &error, eap, skip, FALSE);
	}
	وگرنه
	{
	    forinfo_T	*fi;
	    evalarg_T	evalarg;

	    /*
	     * ":for var in list-expr"
	     */
	    fill_evalarg_from_eap(&evalarg, eap, skip);
	    اگر ((cstack->cs_lflags & CSL_HAD_LOOP) != 0)
	    {
		// Jumping here from a ":continue" or ":endfor": use the
		// previously evaluated list.
		fi = cstack->cs_forinfo[cstack->cs_idx];
		error = FALSE;

		// the "in expr" is not used, skip over it
		skip_for_سطرها(fi, &evalarg);
	    }
	    وگرنه
	    {
		long save_lnum = SOURCING_LNUM;

		// Evaluate the argument and get the info in a structure.
		fi = eval_for_سطر(eap->arg, &error, eap, &evalarg);
		cstack->cs_forinfo[cstack->cs_idx] = fi;

		// Errors should use the first line number.
		SOURCING_LNUM = save_lnum;
	    }

	    // use the element at the start of the list and advance
	    اگر (!error && fi != پوچ && !skip)
		result = next_for_item(fi, eap->arg);
	    وگرنه
		result = FALSE;
	    اگر (fi != پوچ)
		// OR all the cs_flags together, if a function was defined in
		// any round then the loop variable may have been used.
		fi->fi_cs_flags |= prev_cs_flags;

	    اگر (!result)
	    {
		// If a function was defined in any round then set the
		// CSF_FUNC_DEF flag now, so that it's seen by leave_block().
		اگر (fi != پوچ && (fi->fi_cs_flags & CSF_FUNC_DEF))
		    cstack->cs_flags[cstack->cs_idx] |= CSF_FUNC_DEF;

		free_for_info(fi);
		cstack->cs_forinfo[cstack->cs_idx] = پوچ;
	    }
	    clear_evalarg(&evalarg, eap);
	}

	/*
	 * If this cstack entry was just initialised and is active, set the
	 * loop flag, so do_cmdline() will set the line number in cs_line[].
	 * If executing the command a second time, clear the loop flag.
	 */
	اگر (!skip && !error && result)
	{
	    cstack->cs_flags[cstack->cs_idx] |= (CSF_ACTIVE | CSF_TRUE);
	    cstack->cs_lflags ^= CSL_HAD_LOOP;
	}
	وگرنه
	{
	    cstack->cs_lflags &= ~CSL_HAD_LOOP;
	    // If the ":while" evaluates to FALSE or ":for" is past the end of
	    // the list, show the debug prompt at the ":endwhile"/":endfor" as
	    // if there was a ":break" in a ":while"/":for" evaluating to
	    // TRUE.
	    اگر (!skip && !error)
		cstack->cs_flags[cstack->cs_idx] |= CSF_TRUE;
	}
    }
}

/*
 * ":continue"
 */
    تهی
ex_continue(exarg_T *eap)
{
    صحیح		idx;
    cstack_T	*cstack = eap->cstack;

    اگر (cstack->cs_looplevel <= 0 || cstack->cs_idx < 0)
	eap->errmsg = _(e_continue_without_while_or_for);
    وگرنه
    {
	// Try to find the matching ":while".  This might stop at a try
	// conditional not in its finally clause (which is then to be executed
	// next).  Therefore, inactivate all conditionals except the ":while"
	// itself (if reached).
	idx = cleanup_conditionals(cstack, CSF_WHILE | CSF_FOR, FALSE);
	اگر (idx >= 0 && (cstack->cs_flags[idx] & (CSF_WHILE | CSF_FOR)))
	{
	    rewind_conditionals(cstack, idx, CSF_TRY, &cstack->cs_trylevel);

	    /*
	     * Set CSL_HAD_CONT, so do_cmdline() will jump back to the
	     * matching ":while".
	     */
	    cstack->cs_lflags |= CSL_HAD_CONT;	// let do_cmdline() handle it
	}
	وگرنه
	{
	    // If a try conditional not in its finally clause is reached first,
	    // make the ":continue" pending for execution at the ":endtry".
	    cstack->cs_pending[idx] = CSTP_CONTINUE;
	    report_make_pending(CSTP_CONTINUE, پوچ);
	}
    }
}

/*
 * ":break"
 */
    تهی
ex_break(exarg_T *eap)
{
    صحیح		idx;
    cstack_T	*cstack = eap->cstack;

    اگر (cstack->cs_looplevel <= 0 || cstack->cs_idx < 0)
	eap->errmsg = _(e_break_without_while_or_for);
    وگرنه
    {
	// Inactivate conditionals until the matching ":while" or a try
	// conditional not in its finally clause (which is then to be
	// executed next) is found.  In the latter case, make the ":break"
	// pending for execution at the ":endtry".
	idx = cleanup_conditionals(cstack, CSF_WHILE | CSF_FOR, TRUE);
	اگر (idx >= 0 && !(cstack->cs_flags[idx] & (CSF_WHILE | CSF_FOR)))
	{
	    cstack->cs_pending[idx] = CSTP_BREAK;
	    report_make_pending(CSTP_BREAK, پوچ);
	}
    }
}

/*
 * ":endwhile" and ":endfor"
 */
    تهی
ex_endwhile(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;
    صحیح		idx;
    نویسه	*err;
    صحیح		csf;
    صحیح		fl;

    اگر (cmdmod_error(TRUE))
	بازگشت;

    اگر (eap->cmdidx == CMD_endwhile)
    {
	err = e_endwhile_without_while;
	csf = CSF_WHILE;
    }
    وگرنه
    {
	err = e_endfor_without_for;
	csf = CSF_FOR;
    }

    اگر (cstack->cs_looplevel <= 0 || cstack->cs_idx < 0)
	eap->errmsg = _(err);
    وگرنه
    {
	fl = cstack->cs_flags[cstack->cs_idx];
	اگر (!(fl & csf))
	{
	    // If we are in a ":while" or ":for" but used the wrong endloop
	    // command, do not rewind to the next enclosing ":for"/":while".
	    اگر (fl & CSF_WHILE)
		eap->errmsg = _(e_using_endfor_with_while);
	    وگرنه اگر (fl & CSF_FOR)
		eap->errmsg = _(e_using_endwhile_with_for);
	}
	اگر (!(fl & (CSF_WHILE | CSF_FOR)))
	{
	    اگر (!(fl & CSF_TRY))
		eap->errmsg = _(e_missing_endif);
	    وگرنه اگر (fl & CSF_FINALLY)
		eap->errmsg = _(e_missing_endtry);
	    // Try to find the matching ":while" and report what's missing.
	    برای (idx = cstack->cs_idx; idx > 0; --idx)
	    {
		fl =  cstack->cs_flags[idx];
		اگر ((fl & CSF_TRY) && !(fl & CSF_FINALLY))
		{
		    // Give up at a try conditional not in its finally clause.
		    // Ignore the ":endwhile"/":endfor".
		    eap->errmsg = _(err);
		    بازگشت;
		}
		اگر (fl & csf)
		    بشکن;
	    }
	    // Cleanup and rewind all contained (and unclosed) conditionals.
	    (تهی)cleanup_conditionals(cstack, CSF_WHILE | CSF_FOR, FALSE);
	    rewind_conditionals(cstack, idx, CSF_TRY, &cstack->cs_trylevel);
	}

	/*
	 * When debugging or a breakpoint was encountered, display the debug
	 * prompt (if not already done).  This shows the user that an
	 * ":endwhile"/":endfor" is executed when the ":while" was not TRUE or
	 * after a ":break".  Handle a ">quit" debug command as if an
	 * interrupt had occurred before the ":endwhile"/":endfor".  That is,
	 * throw an interrupt exception if appropriate.  Doing this here
	 * prevents that an exception for a parsing error is discarded when
	 * throwing the interrupt exception later on.
	 */
	وگرنه اگر (cstack->cs_flags[cstack->cs_idx] & CSF_TRUE
		&& !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE)
		&& dbg_check_skipped(eap))
	    (تهی)do_intthrow(cstack);

	// Set loop flag, so do_cmdline() will jump back to the matching
	// ":while" or ":for".
	cstack->cs_lflags |= CSL_HAD_ENDLOOP;
    }
}

/*
 * "{" start of a block in Vim9 script
 */
    تهی
ex_block(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;

    اگر (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_block_nesting_too_deep);
    وگرنه
    {
	enter_block(cstack);
	cstack->cs_flags[cstack->cs_idx] = CSF_BLOCK | CSF_ACTIVE | CSF_TRUE;
    }
}

/*
 * "}" end of a block in Vim9 script
 */
    تهی
ex_endblock(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;

    اگر (cstack->cs_idx < 0
	    || (cstack->cs_flags[cstack->cs_idx] & CSF_BLOCK) == 0)
	eap->errmsg = _(e_endblock_without_block);
    وگرنه
	leave_block(cstack);
}

    صحیح
inside_block(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;
    صحیح		i;

    // Control flow commands handle '|' themselves and must not be treated
    // as if inside a block even when they are - otherwise '|' separators
    // in "try | catch | endtry" etc. would not be recognized.
    انتخاب (eap->cmdidx)
    {
	حالت CMD_if:
	حالت CMD_elseif:
	حالت CMD_else:
	حالت CMD_endif:
	حالت CMD_while:
	حالت CMD_endwhile:
	حالت CMD_for:
	حالت CMD_endfor:
	حالت CMD_try:
	حالت CMD_catch:
	حالت CMD_finally:
	حالت CMD_endtry:
	    بازگشت FALSE;
	پیشفرض:
	    بشکن;
    }

    برای (i = 0; i <= cstack->cs_idx; ++i)
	اگر (cstack->cs_flags[i] & CSF_BLOCK)
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * ":throw expr"
 */
    تهی
ex_throw(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*مقدار;

    اگر (*arg != NUL && *arg != '|' && *arg != '\n')
	مقدار = eval_to_string_skip(arg, eap, eap->skip);
    وگرنه
    {
	emsg(_(e_argument_required));
	مقدار = پوچ;
    }

    // On error or when an exception is thrown during argument evaluation, do
    // not throw.
    اگر (!eap->skip && مقدار != پوچ)
    {
	اگر (throw_exception(مقدار, ET_USER, پوچ) == FAIL)
	    vim_free(مقدار);
	وگرنه
	    do_throw(eap->cstack);
    }
}

/*
 * Throw the current exception through the specified cstack.  Common routine
 * for ":throw" (user exception) and error and interrupt exceptions.  Also
 * used for rethrowing an uncaught exception.
 */
    تهی
do_throw(cstack_T *cstack)
{
    صحیح		idx;
    صحیح		inactivate_try = FALSE;

    /*
     * Cleanup and inactivate up to the next surrounding try conditional that
     * is not in its finally clause.  Normally, do not inactivate the try
     * conditional itself, so that its ACTIVE flag can be tested below.  But
     * if a previous error or interrupt has not been converted to an exception,
     * inactivate the try conditional, too, as if the conversion had been done,
     * and reset the did_emsg or got_int flag, so this won't happen again at
     * the next surrounding try conditional.
     */
#ifndef THROW_ON_ERROR_TRUE
    اگر (did_emsg && !THROW_ON_ERROR)
    {
	inactivate_try = TRUE;
	did_emsg = FALSE;
    }
#endif
#ifndef THROW_ON_INTERRUPT_TRUE
    اگر (got_int && !THROW_ON_INTERRUPT)
    {
	inactivate_try = TRUE;
	got_int = FALSE;
    }
#endif
    idx = cleanup_conditionals(cstack, 0, inactivate_try);
    اگر (idx >= 0)
    {
	/*
	 * If this try conditional is active and we are before its first
	 * ":catch", set THROWN so that the ":catch" commands will check
	 * whether the exception matches.  When the exception came from any of
	 * the catch clauses, it will be made pending at the ":finally" (if
	 * present) and rethrown at the ":endtry".  This will also happen if
	 * the try conditional is inactive.  This is the case when we are
	 * throwing an exception due to an error or interrupt on the way from
	 * a preceding ":continue", ":break", ":return", ":finish", error or
	 * interrupt (not converted to an exception) to the finally clause or
	 * from a preceding throw of a user or error or interrupt exception to
	 * the matching catch clause or the finally clause.
	 */
	اگر (!(cstack->cs_flags[idx] & CSF_CAUGHT))
	{
	    اگر (cstack->cs_flags[idx] & CSF_ACTIVE)
		cstack->cs_flags[idx] |= CSF_THROWN;
	    وگرنه
		// THROWN may have already been set for a catchable exception
		// that has been discarded.  Ensure it is reset for the new
		// exception.
		cstack->cs_flags[idx] &= ~CSF_THROWN;
	}
	cstack->cs_flags[idx] &= ~CSF_ACTIVE;
	cstack->cs_exception[idx] = current_exception;
    }
#اگر 0
    // TODO: Add optimization below.  Not yet done because of interface
    // problems to eval.c and ex_cmds2.c. (Servatius)
    وگرنه
    {
	/*
	 * There are no catch clauses to check or finally clauses to execute.
	 * End the current script or function.  The exception will be rethrown
	 * in the caller.
	 */
	اگر (getline_equal(eap->getline, eap->cookie, get_func_سطر))
	    current_funccal->returned = TRUE;
	elseif (eap->get_func_سطر == getsourceline)
	    ((ساختار source_cookie *)eap->cookie)->finished = TRUE;
    }
#endif

    did_throw = TRUE;
}

/*
 * ":try"
 */
    تهی
ex_try(exarg_T *eap)
{
    صحیح		skip;
    cstack_T	*cstack = eap->cstack;

    اگر (cmdmod_error(FALSE))
	بازگشت;

    اگر (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_try_nesting_too_deep);
    وگرنه
    {
	enter_block(cstack);
	++cstack->cs_trylevel;
	cstack->cs_flags[cstack->cs_idx] = CSF_TRY;
	cstack->cs_pending[cstack->cs_idx] = CSTP_NONE;

	/*
	 * Don't do something after an error, interrupt, or throw, or when there
	 * is a surrounding conditional and it was not active.
	 */
	skip = did_emsg || got_int || did_throw || (cstack->cs_idx > 0
		&& !(cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE));

	اگر (!skip)
	{
	    // Set ACTIVE and TRUE.  TRUE means that the corresponding ":catch"
	    // commands should check for a match if an exception is thrown and
	    // that the finally clause needs to be executed.
	    cstack->cs_flags[cstack->cs_idx] |= CSF_ACTIVE | CSF_TRUE;

	    /*
	     * ":silent!", even when used in a try conditional, disables
	     * displaying of error messages and conversion of errors to
	     * exceptions.  When the silent commands again open a try
	     * conditional, save "emsg_silent" and reset it so that errors are
	     * again converted to exceptions.  The value is restored when that
	     * try conditional is left.  If it is left normally, the commands
	     * following the ":endtry" are again silent.  If it is left by
	     * a ":continue", ":break", ":return", or ":finish", the commands
	     * executed next are again silent.  If it is left due to an
	     * aborting error, an interrupt, or an exception, restoring
	     * "emsg_silent" does not matter since we are already in the
	     * aborting state and/or the exception has already been thrown.
	     * The effect is then just freeing the memory that was allocated
	     * to save the value.
	     */
	    اگر (emsg_silent)
	    {
		eslist_T	*elem;

		elem = ALLOC_ONE(ساختار eslist_elem);
		اگر (elem == پوچ)
		    emsg(_(e_out_of_memory));
		وگرنه
		{
		    elem->saved_emsg_silent = emsg_silent;
		    elem->next = cstack->cs_emsg_silent_list;
		    cstack->cs_emsg_silent_list = elem;
		    cstack->cs_flags[cstack->cs_idx] |= CSF_SILENT;
		    emsg_silent = 0;
		}
	    }
	}

    }
}

/*
 * ":catch /{pattern}/" and ":catch"
 */
    تهی
ex_catch(exarg_T *eap)
{
    صحیح		idx = 0;
    صحیح		give_up = FALSE;
    صحیح		skip = FALSE;
    صحیح		caught = FALSE;
    char_u	*end;
    صحیح		save_char = 0;
    char_u	*save_cpo;
    regmatch_T	regmatch;
    صحیح		prev_got_int;
    cstack_T	*cstack = eap->cstack;
    char_u	*pat;

    اگر (cmdmod_error(FALSE))
	بازگشت;

    اگر (cstack->cs_trylevel <= 0 || cstack->cs_idx < 0)
    {
	eap->errmsg = _(e_catch_without_try);
	give_up = TRUE;
    }
    وگرنه
    {
	اگر (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
	{
	    // Report what's missing if the matching ":try" is not in its
	    // finally clause.
	    eap->errmsg = get_end_emsg(cstack);
	    skip = TRUE;
	}
	برای (idx = cstack->cs_idx; idx > 0; --idx)
	    اگر (cstack->cs_flags[idx] & CSF_TRY)
		بشکن;
	اگر (cstack->cs_flags[idx] & CSF_TRY)
	    cstack->cs_flags[idx] |= CSF_CATCH;
	اگر (cstack->cs_flags[idx] & CSF_FINALLY)
	{
	    // Give up for a ":catch" after ":finally" and ignore it.
	    // Just parse.
	    eap->errmsg = _(e_catch_after_finally);
	    give_up = TRUE;
	}
	وگرنه
	    rewind_conditionals(cstack, idx, CSF_WHILE | CSF_FOR,
						       &cstack->cs_looplevel);
    }

    اگر (ends_excmd2(eap->cmd, eap->arg))   // no argument, catch all errors
    {
	pat = (char_u *)".*";
	end = پوچ;
	eap->nextcmd = find_nextcmd(eap->arg);
    }
    وگرنه
    {
	pat = eap->arg + 1;
	end = skip_regexp_err(pat, *eap->arg, TRUE);
	اگر (end == پوچ)
	    give_up = TRUE;
    }

    اگر (!give_up)
    {
	/*
	 * Don't do something when no exception has been thrown or when the
	 * corresponding try block never got active (because of an inactive
	 * surrounding conditional or after an error or interrupt or throw).
	 */
	اگر (!did_throw || !(cstack->cs_flags[idx] & CSF_TRUE))
	    skip = TRUE;

	/*
	 * Check for a match only if an exception is thrown but not caught by
	 * a previous ":catch".  An exception that has replaced a discarded
	 * exception is not checked (THROWN is not set then).
	 */
	اگر (!skip && (cstack->cs_flags[idx] & CSF_THROWN)
		&& !(cstack->cs_flags[idx] & CSF_CAUGHT))
	{
	    اگر (end != پوچ && *end != NUL
				      && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		semsg(_(e_trailing_characters_str), end);
		بازگشت;
	    }

	    // When debugging or a breakpoint was encountered, display the
	    // debug prompt (if not already done) before checking for a match.
	    // This is a helpful hint for the user when the regular expression
	    // matching fails.  Handle a ">quit" debug command as if an
	    // interrupt had occurred before the ":catch".  That is, discard
	    // the original exception, replace it by an interrupt exception,
	    // and don't catch it in this try block.
	    اگر (!dbg_check_skipped(eap) || !do_intthrow(cstack))
	    {
		// Terminate the pattern and avoid the 'l' flag in 'cpoptions'
		// while compiling it.
		اگر (end != پوچ)
		{
		    save_char = *end;
		    *end = NUL;
		}
		save_cpo  = p_cpo;
		p_cpo = empty_option;
		// Disable error messages, it will make current_exception
		// invalid.
		++emsg_off;
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
		--emsg_off;
		regmatch.rm_ic = FALSE;
		اگر (end != پوچ)
		    *end = save_char;
		p_cpo = save_cpo;
		اگر (regmatch.regprog == پوچ)
		    semsg(_(e_invalid_argument_str), pat);
		وگرنه
		{
		    /*
		     * Save the value of got_int and reset it.  We don't want
		     * a previous interruption cancel matching, only hitting
		     * CTRL-C while matching should abort it.
		     */
		    prev_got_int = got_int;
		    got_int = FALSE;
		    caught = vim_regexec_nl(&regmatch,
			       (char_u *)current_exception->مقدار, (colnr_T)0);
		    got_int |= prev_got_int;
		    vim_regfree(regmatch.regprog);
		}
	    }
	}

	اگر (caught)
	{
	    // Make this ":catch" clause active and reset did_emsg, got_int,
	    // and did_throw.  Put the exception on the caught stack.
	    cstack->cs_flags[idx] |= CSF_ACTIVE | CSF_CAUGHT;
	    did_emsg = got_int = did_throw = FALSE;
	    catch_exception((except_T *)cstack->cs_exception[idx]);

	    اگر (cstack->cs_idx >= 0
			       && (cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
	    {
		// Variables declared in the previous block can no longer be
		// used.
		leave_block(cstack);
		enter_block(cstack);
	    }

	    // It's mandatory that the current exception is stored in the cstack
	    // so that it can be discarded at the next ":catch", ":finally", or
	    // ":endtry" or when the catch clause is left by a ":continue",
	    // ":break", ":return", ":finish", error, interrupt, or another
	    // exception.
	    اگر (cstack->cs_exception[cstack->cs_idx] != current_exception)
		internal_error("ex_catch()");
	}
	وگرنه
	{
	    /*
	     * If there is a preceding catch clause and it caught the exception,
	     * finish the exception now.  This happens also after errors except
	     * when this ":catch" was after the ":finally" or not within
	     * a ":try".  Make the try conditional inactive so that the
	     * following catch clauses are skipped.  On an error or interrupt
	     * after the preceding try block or catch clause was left by
	     * a ":continue", ":break", ":return", or ":finish", discard the
	     * pending action.
	     */
	    cleanup_conditionals(cstack, CSF_TRY, TRUE);
	}
    }

    اگر (end != پوچ)
	eap->nextcmd = find_nextcmd(end);
}

/*
 * ":finally"
 */
    تهی
ex_finally(exarg_T *eap)
{
    صحیح		idx;
    صحیح		skip = FALSE;
    صحیح		pending = CSTP_NONE;
    cstack_T	*cstack = eap->cstack;

    اگر (cmdmod_error(FALSE))
	بازگشت;

    برای (idx = cstack->cs_idx; idx >= 0; --idx)
	اگر (cstack->cs_flags[idx] & CSF_TRY)
	    بشکن;
    اگر (cstack->cs_trylevel <= 0 || idx < 0)
    {
	eap->errmsg = _(e_finally_without_try);
	بازگشت;
    }

    اگر (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
    {
	eap->errmsg = get_end_emsg(cstack);
	// Make this error pending, so that the commands in the following
	// finally clause can be executed.  This overrules also a pending
	// ":continue", ":break", ":return", or ":finish".
	pending = CSTP_ERROR;
    }

    اگر (cstack->cs_flags[idx] & CSF_FINALLY)
    {
	// Give up for a multiple ":finally" and ignore it.
	eap->errmsg = _(e_multiple_finally);
	بازگشت;
    }
    rewind_conditionals(cstack, idx, CSF_WHILE | CSF_FOR,
						   &cstack->cs_looplevel);

    /*
     * Don't do something when the corresponding try block never got active
     * (because of an inactive surrounding conditional or after an error or
     * interrupt or throw) or for a ":finally" without ":try" or a multiple
     * ":finally".  After every other error (did_emsg or the conditional
     * errors detected above) or after an interrupt (got_int) or an
     * exception (did_throw), the finally clause must be executed.
     */
    skip = !(cstack->cs_flags[cstack->cs_idx] & CSF_TRUE);

    اگر (!skip)
    {
	// When debugging or a breakpoint was encountered, display the
	// debug prompt (if not already done).  The user then knows that the
	// finally clause is executed.
	اگر (dbg_check_skipped(eap))
	{
	    // Handle a ">quit" debug command as if an interrupt had
	    // occurred before the ":finally".  That is, discard the
	    // original exception and replace it by an interrupt
	    // exception.
	    (تهی)do_intthrow(cstack);
	}

	/*
	 * If there is a preceding catch clause and it caught the exception,
	 * finish the exception now.  This happens also after errors except
	 * when this is a multiple ":finally" or one not within a ":try".
	 * After an error or interrupt, this also discards a pending
	 * ":continue", ":break", ":finish", or ":return" from the preceding
	 * try block or catch clause.
	 */
	cleanup_conditionals(cstack, CSF_TRY, FALSE);

	اگر (cstack->cs_idx >= 0
			   && (cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
	{
	    // Variables declared in the previous block can no longer be
	    // used.
	    leave_block(cstack);
	    enter_block(cstack);
	}

	/*
	 * Make did_emsg, got_int, did_throw pending.  If set, they overrule
	 * a pending ":continue", ":break", ":return", or ":finish".  Then
	 * we have particularly to discard a pending return value (as done
	 * by the call to cleanup_conditionals() above when did_emsg or
	 * got_int is set).  The pending values are restored by the
	 * ":endtry", except if there is a new error, interrupt, exception,
	 * ":continue", ":break", ":return", or ":finish" in the following
	 * finally clause.  A missing ":endwhile", ":endfor" or ":endif"
	 * detected here is treated as if did_emsg and did_throw had
	 * already been set, respectively in case that the error is not
	 * converted to an exception, did_throw had already been unset.
	 * We must not set did_emsg here since that would suppress the
	 * error message.
	 */
	اگر (pending == CSTP_ERROR || did_emsg || got_int || did_throw)
	{
	    اگر (cstack->cs_pending[cstack->cs_idx] == CSTP_RETURN)
	    {
		report_discard_pending(CSTP_RETURN,
				       cstack->cs_rettv[cstack->cs_idx]);
		discard_pending_return(cstack->cs_rettv[cstack->cs_idx]);
	    }
	    اگر (pending == CSTP_ERROR && !did_emsg)
		pending |= (THROW_ON_ERROR) ? CSTP_THROW : 0;
	    وگرنه
		pending |= did_throw ? CSTP_THROW : 0;
	    pending |= did_emsg  ? CSTP_ERROR     : 0;
	    pending |= got_int   ? CSTP_INTERRUPT : 0;
	    cstack->cs_pending[cstack->cs_idx] = pending;

	    // It's mandatory that the current exception is stored in the
	    // cstack so that it can be rethrown at the ":endtry" or be
	    // discarded if the finally clause is left by a ":continue",
	    // ":break", ":return", ":finish", error, interrupt, or another
	    // exception.  When emsg() is called for a missing ":endif" or
	    // a missing ":endwhile"/":endfor" detected here, the
	    // exception will be discarded.
	    اگر (did_throw && cstack->cs_exception[cstack->cs_idx]
						     != current_exception)
		internal_error("ex_finally()");
	}

	/*
	 * Set CSL_HAD_FINA, so do_cmdline() will reset did_emsg,
	 * got_int, and did_throw and make the finally clause active.
	 * This will happen after emsg() has been called for a missing
	 * ":endif" or a missing ":endwhile"/":endfor" detected here, so
	 * that the following finally clause will be executed even then.
	 */
	cstack->cs_lflags |= CSL_HAD_FINA;
    }
}

/*
 * ":endtry"
 */
    تهی
ex_endtry(exarg_T *eap)
{
    صحیح		idx;
    صحیح		skip;
    صحیح		rethrow = FALSE;
    صحیح		pending = CSTP_NONE;
    تهی	*rettv = پوچ;
    cstack_T	*cstack = eap->cstack;

    اگر (cmdmod_error(FALSE))
	بازگشت;

    برای (idx = cstack->cs_idx; idx >= 0; --idx)
	اگر (cstack->cs_flags[idx] & CSF_TRY)
	    بشکن;
    اگر (cstack->cs_trylevel <= 0 || idx < 0)
    {
	eap->errmsg = _(e_endtry_without_try);
	بازگشت;
    }

    /*
     * Don't do something after an error, interrupt or throw in the try
     * block, catch clause, or finally clause preceding this ":endtry" or
     * when an error or interrupt occurred after a ":continue", ":break",
     * ":return", or ":finish" in a try block or catch clause preceding this
     * ":endtry" or when the try block never got active (because of an
     * inactive surrounding conditional or after an error or interrupt or
     * throw) or when there is a surrounding conditional and it has been
     * made inactive by a ":continue", ":break", ":return", or ":finish" in
     * the finally clause.  The latter case need not be tested since then
     * anything pending has already been discarded. */
    skip = did_emsg || got_int || did_throw
			     || !(cstack->cs_flags[cstack->cs_idx] & CSF_TRUE);

    اگر (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
    {
	eap->errmsg = get_end_emsg(cstack);

	// Find the matching ":try" and report what's missing.
	rewind_conditionals(cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack->cs_looplevel);
	skip = TRUE;

	/*
	 * If an exception is being thrown, discard it to prevent it from
	 * being rethrown at the end of this function.  It would be
	 * discarded by the error message, anyway.  Resets did_throw.
	 * This does not affect the script termination due to the error
	 * since "trylevel" is decremented after emsg() has been called.
	 */
	اگر (did_throw)
	    discard_current_exception();

	// report eap->errmsg, also when there already was an error
	did_emsg = FALSE;
    }
    وگرنه
    {
	idx = cstack->cs_idx;

	// Check the flags only when not in a skipped block.
	اگر (!skip && in_vim9script()
		     && (cstack->cs_flags[idx] & (CSF_CATCH|CSF_FINALLY)) == 0)
	{
	    // try/endtry without any catch or finally: give an error and
	    // continue.
	    eap->errmsg = _(e_missing_catch_or_finally);
	}

	/*
	 * If we stopped with the exception currently being thrown at this
	 * try conditional since we didn't know that it doesn't have
	 * a finally clause, we need to rethrow it after closing the try
	 * conditional.
	 */
	اگر (did_throw && (cstack->cs_flags[idx] & CSF_TRUE)
		&& !(cstack->cs_flags[idx] & CSF_FINALLY))
	    rethrow = TRUE;
    }

    // If there was no finally clause, show the user when debugging or
    // a breakpoint was encountered that the end of the try conditional has
    // been reached: display the debug prompt (if not already done).  Do
    // this on normal control flow or when an exception was thrown, but not
    // on an interrupt or error not converted to an exception or when
    // a ":break", ":continue", ":return", or ":finish" is pending.  These
    // actions are carried out immediately.
    اگر ((rethrow || (!skip && !(cstack->cs_flags[idx] & CSF_FINALLY)
		    && !cstack->cs_pending[idx]))
	    && dbg_check_skipped(eap))
    {
	// Handle a ">quit" debug command as if an interrupt had occurred
	// before the ":endtry".  That is, throw an interrupt exception and
	// set "skip" and "rethrow".
	اگر (got_int)
	{
	    skip = TRUE;
	    (تهی)do_intthrow(cstack);
	    // The do_intthrow() call may have reset did_throw or
	    // cstack->cs_pending[idx].
	    rethrow = FALSE;
	    اگر (did_throw && !(cstack->cs_flags[idx] & CSF_FINALLY))
		rethrow = TRUE;
	}
    }

    /*
     * If a ":return" is pending, we need to resume it after closing the
     * try conditional; remember the return value.  If there was a finally
     * clause making an exception pending, we need to rethrow it.  Make it
     * the exception currently being thrown.
     */
    اگر (!skip)
    {
	pending = cstack->cs_pending[idx];
	cstack->cs_pending[idx] = CSTP_NONE;
	اگر (pending == CSTP_RETURN)
	    rettv = cstack->cs_rettv[idx];
	وگرنه اگر (pending & CSTP_THROW)
	    current_exception = cstack->cs_exception[idx];
    }

    /*
     * Discard anything pending on an error, interrupt, or throw in the
     * finally clause.  If there was no ":finally", discard a pending
     * ":continue", ":break", ":return", or ":finish" if an error or
     * interrupt occurred afterwards, but before the ":endtry" was reached.
     * If an exception was caught by the last of the catch clauses and there
     * was no finally clause, finish the exception now.  This happens also
     * after errors except when this ":endtry" is not within a ":try".
     * Restore "emsg_silent" if it has been reset by this try conditional.
     */
    (تهی)cleanup_conditionals(cstack, CSF_TRY | CSF_SILENT, TRUE);

    اگر (cstack->cs_idx >= 0 && (cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
	leave_block(cstack);
    --cstack->cs_trylevel;

    اگر (!skip)
    {
	report_resume_pending(pending,
		    (pending == CSTP_RETURN) ? rettv :
		    (pending & CSTP_THROW) ? (تهی *)current_exception : پوچ);
	انتخاب (pending)
	{
	    حالت CSTP_NONE:
		بشکن;

	    // Reactivate a pending ":continue", ":break", ":return",
	    // ":finish" from the try block or a catch clause of this try
	    // conditional.  This is skipped, if there was an error in an
	    // (unskipped) conditional command or an interrupt afterwards
	    // or if the finally clause is present and executed a new error,
	    // interrupt, throw, ":continue", ":break", ":return", or
	    // ":finish".
	    حالت CSTP_CONTINUE:
		ex_continue(eap);
		بشکن;
	    حالت CSTP_BREAK:
		ex_break(eap);
		بشکن;
	    حالت CSTP_RETURN:
		do_return(eap, FALSE, FALSE, rettv);
		بشکن;
	    حالت CSTP_FINISH:
		do_finish(eap, FALSE);
		بشکن;

	    // When the finally clause was entered due to an error,
	    // interrupt or throw (as opposed to a ":continue", ":break",
	    // ":return", or ":finish"), restore the pending values of
	    // did_emsg, got_int, and did_throw.  This is skipped, if there
	    // was a new error, interrupt, throw, ":continue", ":break",
	    // ":return", or ":finish".  in the finally clause.
	    پیشفرض:
		اگر (pending & CSTP_ERROR)
		    did_emsg = TRUE;
		اگر (pending & CSTP_INTERRUPT)
		    got_int = TRUE;
		اگر (pending & CSTP_THROW)
		    rethrow = TRUE;
		بشکن;
	}
    }

    اگر (rethrow)
	// Rethrow the current exception (within this cstack).
	do_throw(cstack);
}

/*
 * enter_cleanup() and leave_cleanup()
 *
 * Functions to be called before/after invoking a sequence of autocommands for
 * cleanup for a failed command.  (Failure means here that a call to emsg()
 * has been made, an interrupt occurred, or there is an uncaught exception
 * from a previous autocommand execution of the same command.)
 *
 * Call enter_cleanup() with a pointer to a cleanup_T and pass the same
 * pointer to leave_cleanup().  The cleanup_T structure stores the pending
 * error/interrupt/exception state.
 */

/*
 * This function works a bit like ex_finally() except that there was not
 * actually an extra try block around the part that failed and an error or
 * interrupt has not (yet) been converted to an exception.  This function
 * saves the error/interrupt/ exception state and prepares for the call to
 * do_cmdline() that is going to be made for the cleanup autocommand
 * execution.
 */
    تهی
enter_cleanup(cleanup_T *csp)
{
    صحیح		pending = CSTP_NONE;

    /*
     * Postpone did_emsg, got_int, did_throw.  The pending values will be
     * restored by leave_cleanup() except if there was an aborting error,
     * interrupt, or uncaught exception after this function ends.
     */
    اگر (did_emsg || got_int || did_throw || need_rethrow)
    {
	csp->pending = (did_emsg     ? CSTP_ERROR     : 0)
		     | (got_int      ? CSTP_INTERRUPT : 0)
		     | (did_throw    ? CSTP_THROW     : 0)
		     | (need_rethrow ? CSTP_THROW     : 0);

	// If we are currently throwing an exception (did_throw), save it as
	// well.  On an error not yet converted to an exception, update
	// "force_abort" and reset "cause_abort" (as do_errthrow() would do).
	// This is needed for the do_cmdline() call that is going to be made
	// for autocommand execution.  We need not save *msg_list because
	// there is an extra instance for every call of do_cmdline(), anyway.
	اگر (did_throw || need_rethrow)
	{
	    csp->exception = current_exception;
	    current_exception = پوچ;
	}
	وگرنه
	{
	    csp->exception = پوچ;
	    اگر (did_emsg)
	    {
		force_abort |= cause_abort;
		cause_abort = FALSE;
	    }
	}
	did_emsg = got_int = did_throw = need_rethrow = FALSE;

	// Report if required by the 'verbose' option or when debugging.
	report_make_pending(pending, csp->exception);
    }
    وگرنه
    {
	csp->pending = CSTP_NONE;
	csp->exception = پوچ;
    }
}

/*
 * See comment above enter_cleanup() for how this function is used.
 *
 * This function is a bit like ex_endtry() except that there was not actually
 * an extra try block around the part that failed and an error or interrupt
 * had not (yet) been converted to an exception when the cleanup autocommand
 * sequence was invoked.
 *
 * This function has to be called with the address of the cleanup_T structure
 * filled by enter_cleanup() as an argument; it restores the error/interrupt/
 * exception state saved by that function - except there was an aborting
 * error, an interrupt or an uncaught exception during execution of the
 * cleanup autocommands.  In the latter case, the saved error/interrupt/
 * exception state is discarded.
 */
    تهی
leave_cleanup(cleanup_T *csp)
{
    صحیح		pending = csp->pending;

    اگر (pending == CSTP_NONE)	// nothing to do
	بازگشت;

    // If there was an aborting error, an interrupt, or an uncaught exception
    // after the corresponding call to enter_cleanup(), discard what has been
    // made pending by it.  Report this to the user if required by the
    // 'verbose' option or when debugging.
    اگر (aborting() || need_rethrow)
    {
	اگر (pending & CSTP_THROW)
	    // Cancel the pending exception (includes report).
	    discard_exception(csp->exception, FALSE);
	وگرنه
	    report_discard_pending(pending, پوچ);

	// If an error was about to be converted to an exception when
	// enter_cleanup() was called, free the message list.
	اگر (پیام_list != پوچ)
	    free_global_msglist();
    }

    /*
     * If there was no new error, interrupt, or throw between the calls
     * to enter_cleanup() and leave_cleanup(), restore the pending
     * error/interrupt/exception state.
     */
    وگرنه
    {
	/*
	 * If there was an exception being thrown when enter_cleanup() was
	 * called, we need to rethrow it.  Make it the exception currently
	 * being thrown.
	 */
	اگر (pending & CSTP_THROW)
	    current_exception = csp->exception;

	/*
	 * If an error was about to be converted to an exception when
	 * enter_cleanup() was called, let "cause_abort" take the part of
	 * "force_abort" (as done by cause_errthrow()).
	 */
	وگرنه اگر (pending & CSTP_ERROR)
	{
	    cause_abort = force_abort;
	    force_abort = FALSE;
	}

	/*
	 * Restore the pending values of did_emsg, got_int, and did_throw.
	 */
	اگر (pending & CSTP_ERROR)
	    did_emsg = TRUE;
	اگر (pending & CSTP_INTERRUPT)
	    got_int = TRUE;
	اگر (pending & CSTP_THROW)
	    need_rethrow = TRUE;    // did_throw will be set by do_one_cmd()

	// Report if required by the 'verbose' option or when debugging.
	report_resume_pending(pending,
		   (pending & CSTP_THROW) ? (تهی *)current_exception : پوچ);
    }
}


/*
 * Make conditionals inactive and discard what's pending in finally clauses
 * until the conditional type searched for or a try conditional not in its
 * finally clause is reached.  If this is in an active catch clause, finish
 * the caught exception.
 * Return the cstack index where the search stopped.
 * Values used for "searched_cond" are (CSF_WHILE | CSF_FOR) or CSF_TRY or 0,
 * the latter meaning the innermost try conditional not in its finally clause.
 * "inclusive" tells whether the conditional searched for should be made
 * inactive itself (a try conditional not in its finally clause possibly find
 * before is always made inactive).  If "inclusive" is TRUE and
 * "searched_cond" is CSF_TRY|CSF_SILENT, the saved former value of
 * "emsg_silent", if reset when the try conditional finally reached was
 * entered, is restored (used by ex_endtry()).  This is normally done only
 * when such a try conditional is left.
 */
    صحیح
cleanup_conditionals(
    cstack_T   *cstack,
    صحیح		searched_cond,
    صحیح		inclusive)
{
    صحیح		idx;
    صحیح		stop = FALSE;

    برای (idx = cstack->cs_idx; idx >= 0; --idx)
    {
	اگر (cstack->cs_flags[idx] & CSF_TRY)
	{
	    /*
	     * Discard anything pending in a finally clause and continue the
	     * search.  There may also be a pending ":continue", ":break",
	     * ":return", or ":finish" before the finally clause.  We must not
	     * discard it, unless an error or interrupt occurred afterwards.
	     */
	    اگر (did_emsg || got_int || (cstack->cs_flags[idx] & CSF_FINALLY))
	    {
		انتخاب (cstack->cs_pending[idx])
		{
		    حالت CSTP_NONE:
			بشکن;

		    حالت CSTP_CONTINUE:
		    حالت CSTP_BREAK:
		    حالت CSTP_FINISH:
			report_discard_pending(cstack->cs_pending[idx], پوچ);
			cstack->cs_pending[idx] = CSTP_NONE;
			بشکن;

		    حالت CSTP_RETURN:
			report_discard_pending(CSTP_RETURN,
						      cstack->cs_rettv[idx]);
			discard_pending_return(cstack->cs_rettv[idx]);
			cstack->cs_pending[idx] = CSTP_NONE;
			بشکن;

		    پیشفرض:
			اگر (cstack->cs_flags[idx] & CSF_FINALLY)
			{
			    اگر ((cstack->cs_pending[idx] & CSTP_THROW)
				    && cstack->cs_exception[idx] != پوچ)
			    {
				// Cancel the pending exception.  This is in the
				// finally clause, so that the stack of the
				// caught exceptions is not involved.
				discard_exception(
					(except_T *)cstack->cs_exception[idx],
					FALSE);
			    }
			    وگرنه
				report_discard_pending(cstack->cs_pending[idx],
					پوچ);
			    cstack->cs_pending[idx] = CSTP_NONE;
			}
			بشکن;
		}
	    }

	    /*
	     * Stop at a try conditional not in its finally clause.  If this try
	     * conditional is in an active catch clause, finish the caught
	     * exception.
	     */
	    اگر (!(cstack->cs_flags[idx] & CSF_FINALLY))
	    {
		اگر ((cstack->cs_flags[idx] & CSF_ACTIVE)
			&& (cstack->cs_flags[idx] & CSF_CAUGHT)
			&& !(cstack->cs_flags[idx] & CSF_FINISHED))
		{
		    finish_exception((except_T *)cstack->cs_exception[idx]);
		    cstack->cs_flags[idx] |= CSF_FINISHED;
		}
		// Stop at this try conditional - except the try block never
		// got active (because of an inactive surrounding conditional
		// or when the ":try" appeared after an error or interrupt or
		// throw).
		اگر (cstack->cs_flags[idx] & CSF_TRUE)
		{
		    اگر (searched_cond == 0 && !inclusive)
			بشکن;
		    stop = TRUE;
		}
	    }
	}

	// Stop on the searched conditional type (even when the surrounding
	// conditional is not active or something has been made pending).
	// If "inclusive" is TRUE and "searched_cond" is CSF_TRY|CSF_SILENT,
	// check first whether "emsg_silent" needs to be restored.
	اگر (cstack->cs_flags[idx] & searched_cond)
	{
	    اگر (!inclusive)
		بشکن;
	    stop = TRUE;
	}
	cstack->cs_flags[idx] &= ~CSF_ACTIVE;
	اگر (stop && searched_cond != (CSF_TRY | CSF_SILENT))
	    بشکن;

	/*
	 * When leaving a try conditional that reset "emsg_silent" on its
	 * entry after saving the original value, restore that value here and
	 * free the memory used to store it.
	 */
	اگر ((cstack->cs_flags[idx] & CSF_TRY)
		&& (cstack->cs_flags[idx] & CSF_SILENT))
	{
	    eslist_T	*elem;

	    elem = cstack->cs_emsg_silent_list;
	    cstack->cs_emsg_silent_list = elem->next;
	    emsg_silent = elem->saved_emsg_silent;
	    vim_free(elem);
	    cstack->cs_flags[idx] &= ~CSF_SILENT;
	}
	اگر (stop)
	    بشکن;
    }
    بازگشت idx;
}

/*
 * Return an appropriate error message for a missing endwhile/endfor/endif.
 */
   ایستا نویسه *
get_end_emsg(cstack_T *cstack)
{
    اگر (cstack->cs_flags[cstack->cs_idx] & CSF_WHILE)
	بازگشت _(e_missing_endwhile);
    اگر (cstack->cs_flags[cstack->cs_idx] & CSF_FOR)
	بازگشت _(e_missing_endfor);
    بازگشت _(e_missing_endif);
}


/*
 * Rewind conditionals until index "idx" is reached.  "cond_type" and
 * "cond_level" specify a conditional type and the address of a level variable
 * which is to be decremented with each skipped conditional of the specified
 * type.
 * Also free "for info" structures where needed.
 */
    تهی
rewind_conditionals(
    cstack_T   *cstack,
    صحیح		idx,
    صحیح		cond_type,
    صحیح		*cond_level)
{
    وقتی (cstack->cs_idx > idx)
    {
	اگر (cstack->cs_flags[cstack->cs_idx] & cond_type)
	    --*cond_level;
	اگر (cstack->cs_flags[cstack->cs_idx] & CSF_FOR)
	    free_for_info(cstack->cs_forinfo[cstack->cs_idx]);
	leave_block(cstack);
    }
}

/*
 * ":endfunction" or ":enddef" when not after a ":function"
 */
    تهی
ex_endfunction(exarg_T *eap)
{
    اگر (eap->cmdidx == CMD_enddef)
	semsg(_(e_str_not_inside_function), ":enddef");
    وگرنه
	semsg(_(e_str_not_inside_function), ":endfunction");
}

/*
 * Return TRUE if the string "p" looks like a ":while" or ":for" command.
 */
    صحیح
has_loop_cmd(char_u *p)
{
    صحیح		len;

    // skip modifiers, white space and ':'
    برای (;;)
    {
	وقتی (*p == ' ' || *p == '\t' || *p == ':')
	    ++p;
	len = modifier_len(p);
	اگر (len == 0)
	    بشکن;
	p += len;
    }
    اگر ((p[0] == 'w' && p[1] == 'h')
	    || (p[0] == 'f' && p[1] == 'o' && p[2] == 'r'))
	بازگشت TRUE;
    بازگشت FALSE;
}

#endif // FEAT_EVAL
