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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

ਸਥਿਰ ਅੱਖਰ	*get_ਸਿਰਾ_emsg(cstack_T *cstack);

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
#ਜੇ 0
// Expressions used for testing during the development phase.
# define THROW_ON_ERROR		(!eval_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ("$VIMNOERRTHROW"))
# define THROW_ON_ਰੁਕਾਵਟ	(!eval_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ("$VIMNOINTTHROW"))
# define THROW_TEST
#ਨਹੀਂਤਾਂ
// Values used for the Vim release.
# define THROW_ON_ERROR		TRUE
# define THROW_ON_ERROR_TRUE
# define THROW_ON_ਰੁਕਾਵਟ	TRUE
# define THROW_ON_ਰੁਕਾਵਟ_TRUE
#ਜੇ_ਅੰਤ

/*
 * When several errors appear in a row, setting "force_abort" is delayed until
 * the failing command returned.  "cause_abort" is set to TRUE meanwhile, in
 * order to indicate that situation.  This is useful when "force_abort" was set
 * during execution of a function call from an expression: the aborting of the
 * expression evaluation is done without producing any error messages, but all
 * error messages on parsing errors during the expression evaluation are given
 * (even if a try conditional is active).
 */
ਸਥਿਰ ਪੂਰਨ cause_abort = FALSE;

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
    ਪੂਰਨ
aborting(ਖਾਲੀ)
{
    ਵਾਪਸ (did_emsg && force_abort) || got_int || did_throw;
}

/*
 * The value of "force_abort" is temporarily reset by the first emsg() call
 * during an expression evaluation, and "cause_abort" is used instead.  It might
 * be necessary to restore "force_abort" even before the throw point for the
 * error message has been reached.  update_force_abort() should be called then.
 */
    ਖਾਲੀ
update_force_abort(ਖਾਲੀ)
{
    ਜੇ (cause_abort)
	force_abort = TRUE;
}

/*
 * Return TRUE if a command with a subcommand resulting in "retcode" should
 * abort the script processing.  Can be used to suppress an autocommand after
 * execution of a failing subcommand as long as the error message has not been
 * displayed and actually caused the abortion.
 */
    ਪੂਰਨ
should_abort(ਪੂਰਨ retcode)
{
    ਵਾਪਸ ((retcode == FAIL && trylevel != 0 && !emsg_silent) || aborting());
}

/*
 * Return TRUE if a function with the "abort" flag should not be considered
 * ended on an error.  This means that parsing commands is continued in order
 * to find finally clauses to be executed, and that some errors in skipped
 * commands are still reported.
 */
    ਪੂਰਨ
aborted_ਵਿੱਚ_try(ਖਾਲੀ)
{
    // This function is only called after an error.  In this case, "force_abort"
    // determines whether searching for finally clauses is necessary.
    ਵਾਪਸ force_abort;
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
    ਪੂਰਨ
cause_errthrow(
    char_u	*mesg,
    ਪੂਰਨ		severe,
    ਪੂਰਨ		*ignore)
{
    msglist_T	*elem;
    msglist_T	**plist;

    /*
     * Do nothing when displaying the interrupt message or reporting an
     * uncaught exception (which has already been discarded then) at the top
     * level.  Also when no exception can be thrown.  The message will be
     * displayed by emsg().
     */
    ਜੇ (suppress_errthrow)
	ਵਾਪਸ FALSE;

    /*
     * If emsg() has not been called previously, temporarily reset
     * "force_abort" until the throw point for error messages has been
     * reached.  This ensures that aborting() returns the same value for all
     * errors that appear in the same command.  This means particularly that
     * for parsing errors during expression evaluation emsg() will be called
     * multiply, even when the expression is evaluated from a finally clause
     * that was activated due to an aborting error, interrupt, or exception.
     */
    ਜੇ (!did_emsg)
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
    ਜੇ (((trylevel == 0 && !cause_abort) || emsg_silent) && !did_throw)
	ਵਾਪਸ FALSE;

    /*
     * Ignore an interrupt message when inside a try conditional or when an
     * exception is being thrown or when an error in a try conditional or
     * throw has been detected previously.  This is important in order that an
     * interrupt exception is catchable by the innermost try conditional and
     * not replaced by an interrupt message error exception.
     */
    ਜੇ (mesg == (char_u *)_(e_interrupted))
    {
	*ignore = TRUE;
	ਵਾਪਸ TRUE;
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
    ਜੇ (did_throw)
    {
	// When discarding an interrupt exception, reset got_int to prevent the
	// same interrupt being converted to an exception again and discarding
	// the error exception we are about to throw here.
	ਜੇ (current_exception->type == ET_ਰੁਕਾਵਟ)
	    got_int = FALSE;
	discard_current_exception();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ THROW_TEST
    ਜੇ (!THROW_ON_ERROR)
    {
	/*
	 * Print error message immediately without searching for a matching
	 * catch clause; just finally clauses are executed before the script
	 * is terminated.
	 */
	ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
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
	ਜੇ (ਸੁਨੇਹਾ_list != ਨਿਰਾ)
	{
	    plist = ਸੁਨੇਹਾ_list;
	    ਜਦੋਂ (*plist != ਨਿਰਾ)
		plist = &(*plist)->ਅਗਲਾ;

	    elem = ALLOC_CLEAR_ONE(msglist_T);
	    ਜੇ (elem == ਨਿਰਾ)
	    {
		suppress_errthrow = TRUE;
		emsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ));
	    }
	    ਨਹੀਂਤਾਂ
	    {
		elem->ਸੁਨੇਹਾ = (ਅੱਖਰ *)vim_strsave(mesg);
		ਜੇ (elem->ਸੁਨੇਹਾ == ਨਿਰਾ)
		{
		    vim_free(elem);
		    suppress_errthrow = TRUE;
		    emsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ));
		}
		ਨਹੀਂਤਾਂ
		{
		    elem->ਅਗਲਾ = ਨਿਰਾ;
		    elem->throw_ਸੁਨੇਹਾ = ਨਿਰਾ;
		    *plist = elem;
		    ਜੇ (plist == ਸੁਨੇਹਾ_list || severe)
		    {
			ਅੱਖਰ	    *tmsg;

			// Skip the extra "Vim " prefix for message "E457".
			tmsg = elem->ਸੁਨੇਹਾ;
			ਜੇ (STRNCMP(tmsg, "Vim E", 5) == 0
				&& VIM_ISDIGIT(tmsg[5])
				&& VIM_ISDIGIT(tmsg[6])
				&& VIM_ISDIGIT(tmsg[7])
				&& tmsg[8] == ':'
				&& tmsg[9] == ' ')
			    (*ਸੁਨੇਹਾ_list)->throw_ਸੁਨੇਹਾ = &tmsg[4];
			ਨਹੀਂਤਾਂ
			    (*ਸੁਨੇਹਾ_list)->throw_ਸੁਨੇਹਾ = tmsg;
		    }

		    // Get the source name and lnum now, it may change before
		    // reaching do_errthrow().
		    elem->sfile = estack_sfile(ESTACK_NONE);
		    elem->slnum = SOURCING_LNUM;
		    elem->ਸੁਨੇਹਾ_compiling = estack_compiling;
		}
	    }
	}
	ਵਾਪਸ TRUE;
    }
}

/*
 * Free a "msg_list" and the messages it contains.
 */
    ਸਥਿਰ ਖਾਲੀ
free_msglist(msglist_T *l)
{
    msglist_T  *messages, *ਅਗਲਾ;

    messages = l;
    ਜਦੋਂ (messages != ਨਿਰਾ)
    {
	ਅਗਲਾ = messages->ਅਗਲਾ;
	vim_free(messages->ਸੁਨੇਹਾ);
	vim_free(messages->sfile);
	vim_free(messages);
	messages = ਅਗਲਾ;
    }
}

/*
 * Free global "*msg_list" and the messages it contains, then set "*msg_list"
 * to NULL.
 */
    ਖਾਲੀ
free_global_msglist(ਖਾਲੀ)
{
    free_msglist(*ਸੁਨੇਹਾ_list);
    *ਸੁਨੇਹਾ_list = ਨਿਰਾ;
}

/*
 * Throw the message specified in the call to cause_errthrow() above as an
 * error exception.  If cstack is NULL, postpone the throw until do_cmdline()
 * has returned (see do_one_cmd()).
 */
    ਖਾਲੀ
do_errthrow(cstack_T *cstack, char_u *cmdname)
{
    /*
     * Ensure that all commands in nested function calls and sourced files
     * are aborted immediately.
     */
    ਜੇ (cause_abort)
    {
	cause_abort = FALSE;
	force_abort = TRUE;
    }

    // If no exception is to be thrown or the conversion should be done after
    // returning to a previous invocation of do_one_cmd(), do nothing.
    ਜੇ (ਸੁਨੇਹਾ_list == ਨਿਰਾ || *ਸੁਨੇਹਾ_list == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (throw_exception(*ਸੁਨੇਹਾ_list, ET_ERROR, cmdname) == FAIL)
	free_msglist(*ਸੁਨੇਹਾ_list);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (cstack != ਨਿਰਾ)
	    do_throw(cstack);
	ਨਹੀਂਤਾਂ
	    need_rethrow = TRUE;
    }
    *ਸੁਨੇਹਾ_list = ਨਿਰਾ;
}

/*
 * do_intthrow(): Replace the current exception by an interrupt or interrupt
 * exception if appropriate.  Return TRUE if the current exception is discarded,
 * FALSE otherwise.
 */
    ਪੂਰਨ
do_intthrow(cstack_T *cstack)
{
    /*
     * If no interrupt occurred or no try conditional is active and no exception
     * is being thrown, do nothing (for compatibility of non-EH scripts).
     */
    ਜੇ (!got_int || (trylevel == 0 && !did_throw))
	ਵਾਪਸ FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ THROW_TEST	// avoid warning for condition always true
    ਜੇ (!THROW_ON_ਰੁਕਾਵਟ)
    {
	/*
	 * The interrupt aborts everything except for executing finally clauses.
	 * Discard any user or error or interrupt exception currently being
	 * thrown.
	 */
	ਜੇ (did_throw)
	    discard_current_exception();
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	/*
	 * Throw an interrupt exception, so that everything will be aborted
	 * (except for executing finally clauses), until the interrupt exception
	 * is caught; if still uncaught at the top level, the script processing
	 * will be terminated then.  -  If an interrupt exception is already
	 * being thrown, do nothing.
	 *
	 */
	ਜੇ (did_throw)
	{
	    ਜੇ (current_exception->type == ET_ਰੁਕਾਵਟ)
		ਵਾਪਸ FALSE;

	    // An interrupt exception replaces any user or error exception.
	    discard_current_exception();
	}
	ਜੇ (throw_exception("Vim:Interrupt", ET_ਰੁਕਾਵਟ, ਨਿਰਾ) != FAIL)
	    do_throw(cstack);
    }

    ਵਾਪਸ TRUE;
}

/*
 * Get an exception message that is to be stored in current_exception->value.
 */
    ਅੱਖਰ *
get_exception_string(
    ਖਾਲੀ	*ਮੁੱਲ,
    except_type_T type,
    char_u	*cmdname,
    ਪੂਰਨ		*should_free)
{
    ਅੱਖਰ	*ret;
    ਅੱਖਰ	*mesg;
    ਪੂਰਨ		cmdlen;
    ਅੱਖਰ	*p, *val;

    ਜੇ (type == ET_ERROR)
    {
	*should_free = TRUE;
	mesg = ((msglist_T *)ਮੁੱਲ)->throw_ਸੁਨੇਹਾ;
	ਜੇ (cmdname != ਨਿਰਾ && *cmdname != NUL)
	{
	    cmdlen = (ਪੂਰਨ)STRLEN(cmdname);
	    ret = (ਅੱਖਰ *)vim_strnsave((char_u *)"Vim(",
						4 + cmdlen + 2 + STRLEN(mesg));
	    ਜੇ (ret == ਨਿਰਾ)
		ਵਾਪਸ ret;
	    STRCPY(&ret[4], cmdname);
	    STRCPY(&ret[4 + cmdlen], "):");
	    val = ret + 4 + cmdlen + 2;
	}
	ਨਹੀਂਤਾਂ
	{
	    ret = (ਅੱਖਰ *)vim_strnsave((char_u *)"Vim:", 4 + STRLEN(mesg));
	    ਜੇ (ret == ਨਿਰਾ)
		ਵਾਪਸ ret;
	    val = ret + 4;
	}

	// msg_add_fname may have been used to prefix the message with a file
	// name in quotes.  In the exception value, put the file name in
	// parentheses and move it to the end.
	ਲਈ (p = mesg; ; p++)
	{
	    ਜੇ (*p == NUL
		    || (*p == 'E'
			&& VIM_ISDIGIT(p[1])
			&& (p[2] == ':'
			    || (VIM_ISDIGIT(p[2])
				&& (p[3] == ':'
				    || (VIM_ISDIGIT(p[3])
					&& p[4] == ':'))))))
	    {
		ਜੇ (*p == NUL || p == mesg)
		    STRCAT(val, mesg);  // 'E123' missing or at beginning
		ਨਹੀਂਤਾਂ
		{
		    // '"filename" E123: message text'
		    ਜੇ (mesg[0] != '"' || p-2 < &mesg[1] ||
			    p[-2] != '"' || p[-1] != ' ')
			// "E123:" is part of the file name.
			ਜਾਰੀ;

		    STRCAT(val, p);
		    p[-2] = NUL;
		    sprintf((ਅੱਖਰ *)(val + STRLEN(p)), " (%s)", &mesg[1]);
		    p[-2] = '"';
		}
		ਰੋਕੋ;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	*should_free = FALSE;
	ret = ਮੁੱਲ;
    }

    ਵਾਪਸ ret;
}


/*
 * Throw a new exception.  Return FAIL when out of memory or it was tried to
 * throw an illegal user exception.  "value" is the exception string for a
 * user or interrupt exception, or points to a message list in case of an
 * error exception.
 */
    ਪੂਰਨ
throw_exception(ਖਾਲੀ *ਮੁੱਲ, except_type_T type, char_u *cmdname)
{
    except_T	*excp;
    ਪੂਰਨ		should_free;

    /*
     * Disallow faking Interrupt or error exceptions as user exceptions.  They
     * would be treated differently from real interrupt or error exceptions
     * when no active try block is found, see do_cmdline().
     */
    ਜੇ (type == ET_USER)
    {
	ਜੇ (STRNCMP((char_u *)ਮੁੱਲ, "Vim", 3) == 0
		&& (((char_u *)ਮੁੱਲ)[3] == NUL || ((char_u *)ਮੁੱਲ)[3] == ':'
		    || ((char_u *)ਮੁੱਲ)[3] == '('))
	{
	    emsg(_(e_cannot_throw_exceptions_with_vim_ਅਗੇਤਰ));
	    ਜਾਓ fail;
	}
    }

    excp = ALLOC_ONE(except_T);
    ਜੇ (excp == ਨਿਰਾ)
	ਜਾਓ nomem;

    ਜੇ (type == ET_ERROR)
	// Store the original message and prefix the exception value with
	// "Vim:" or, if a command name is given, "Vim(cmdname):".
	excp->messages = (msglist_T *)ਮੁੱਲ;

    excp->ਮੁੱਲ = get_exception_string(ਮੁੱਲ, type, cmdname, &should_free);
    ਜੇ (excp->ਮੁੱਲ == ਨਿਰਾ && should_free)
	ਜਾਓ nomem;

    excp->type = type;
    ਜੇ (type == ET_ERROR && ((msglist_T *)ਮੁੱਲ)->sfile != ਨਿਰਾ)
    {
	msglist_T *ਇੰਦਰਾਜ = (msglist_T *)ਮੁੱਲ;

	excp->throw_name = ਇੰਦਰਾਜ->sfile;
	ਇੰਦਰਾਜ->sfile = ਨਿਰਾ;
	excp->throw_lnum = ਇੰਦਰਾਜ->slnum;
    }
    ਨਹੀਂਤਾਂ
    {
	excp->throw_name = estack_sfile(ESTACK_NONE);
	ਜੇ (excp->throw_name == ਨਿਰਾ)
	    excp->throw_name = vim_strsave((char_u *)"");
	ਜੇ (excp->throw_name == ਨਿਰਾ)
	{
	    ਜੇ (should_free)
		vim_free(excp->ਮੁੱਲ);
	    ਜਾਓ nomem;
	}
	excp->throw_lnum = SOURCING_LNUM;
    }

    excp->stacktrace = stacktrace_create();
    ਜੇ (excp->stacktrace != ਨਿਰਾ)
	excp->stacktrace->lv_refcount = 1;

    ਜੇ (p_verbose >= 13 || debug_break_level > 0)
    {
	ਪੂਰਨ	save_ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent;

	ਜੇ (debug_break_level > 0)
	    ਸੁਨੇਹਾ_silent = FALSE;		// display messages
	ਨਹੀਂਤਾਂ
	    verbose_enter();
	++no_wait_return;
	ਜੇ (debug_break_level > 0 || *p_vfile == NUL)
	    ਸੁਨੇਹਾ_scroll = TRUE;	    // always scroll up, don't overwrite

	smsg(_("Exception thrown: %s"), excp->ਮੁੱਲ);
	ਸੁਨੇਹਾ_puts("\n");   // don't overwrite this either

	ਜੇ (debug_break_level > 0 || *p_vfile == NUL)
	    cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	--no_wait_return;
	ਜੇ (debug_break_level > 0)
	    ਸੁਨੇਹਾ_silent = save_ਸੁਨੇਹਾ_silent;
	ਨਹੀਂਤਾਂ
	    verbose_leave();
    }

    current_exception = excp;
    ਵਾਪਸ OK;

nomem:
    vim_free(excp);
    suppress_errthrow = TRUE;
    emsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ));
fail:
    current_exception = ਨਿਰਾ;
    ਵਾਪਸ FAIL;
}

/*
 * Discard an exception.  "was_finished" is set when the exception has been
 * caught and the catch clause has been ended normally.
 */
    ਸਥਿਰ ਖਾਲੀ
discard_exception(except_T *excp, ਪੂਰਨ was_ਮੁਕੰਮਲ)
{
    char_u		*saved_IObuff;

    ਜੇ (current_exception == excp)
	current_exception = ਨਿਰਾ;
    ਜੇ (excp == ਨਿਰਾ)
    {
	ਅੰਦਰੂਨੀ_error("discard_exception()");
	ਵਾਪਸ;
    }

    ਜੇ (p_verbose >= 13 || debug_break_level > 0)
    {
	ਪੂਰਨ	save_ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent;

	saved_IObuff = vim_strsave(IObuff);
	ਜੇ (debug_break_level > 0)
	    ਸੁਨੇਹਾ_silent = FALSE;		// display messages
	ਨਹੀਂਤਾਂ
	    verbose_enter();
	++no_wait_return;
	ਜੇ (debug_break_level > 0 || *p_vfile == NUL)
	    ਸੁਨੇਹਾ_scroll = TRUE;	    // always scroll up, don't overwrite
	smsg(was_ਮੁਕੰਮਲ
		    ? _("Exception finished: %s")
		    : _("Exception discarded: %s"),
		excp->ਮੁੱਲ);
	ਸੁਨੇਹਾ_puts("\n");   // don't overwrite this either
	ਜੇ (debug_break_level > 0 || *p_vfile == NUL)
	    cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	--no_wait_return;
	ਜੇ (debug_break_level > 0)
	    ਸੁਨੇਹਾ_silent = save_ਸੁਨੇਹਾ_silent;
	ਨਹੀਂਤਾਂ
	    verbose_leave();
	STRCPY(IObuff, saved_IObuff);
	vim_free(saved_IObuff);
    }
    ਜੇ (excp->type != ET_ਰੁਕਾਵਟ)
	vim_free(excp->ਮੁੱਲ);
    ਜੇ (excp->type == ET_ERROR)
	free_msglist(excp->messages);
    vim_free(excp->throw_name);
    list_unref(excp->stacktrace);
    vim_free(excp);
}

/*
 * Discard the exception currently being thrown.
 */
    ਖਾਲੀ
discard_current_exception(ਖਾਲੀ)
{
    ਜੇ (current_exception != ਨਿਰਾ)
	discard_exception(current_exception, FALSE);
    did_throw = FALSE;
    need_rethrow = FALSE;
}

/*
 * Put an exception on the caught stack.
 */
    ਖਾਲੀ
catch_exception(except_T *excp)
{
    excp->caught = caught_ਢੇਰ;
    caught_ਢੇਰ = excp;
    ਸੈੱਟ_vim_var_string(VV_EXCEPTION, (char_u *)excp->ਮੁੱਲ, -1);
    ਸੈੱਟ_vim_var_list(VV_STACKTRACE, excp->stacktrace);
    ਜੇ (*excp->throw_name != NUL)
    {
	size_t	IObufflen;

	ਜੇ (excp->throw_lnum != 0)
	    IObufflen = vim_snprintf_safelen((ਅੱਖਰ *)IObuff, IOSIZE, _("%s, line %ld"),
				    excp->throw_name, (ਲੰਮਾ)excp->throw_lnum);
	ਨਹੀਂਤਾਂ
	    IObufflen = vim_snprintf_safelen((ਅੱਖਰ *)IObuff, IOSIZE, "%s", excp->throw_name);
	ਸੈੱਟ_vim_var_string(VV_THROWPOINT, IObuff, (ਪੂਰਨ)IObufflen);
    }
    ਨਹੀਂਤਾਂ
	// throw_name not set on an exception from a command that was typed.
	ਸੈੱਟ_vim_var_string(VV_THROWPOINT, ਨਿਰਾ, -1);

    ਜੇ (p_verbose >= 13 || debug_break_level > 0)
    {
	ਪੂਰਨ	save_ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent;

	ਜੇ (debug_break_level > 0)
	    ਸੁਨੇਹਾ_silent = FALSE;		// display messages
	ਨਹੀਂਤਾਂ
	    verbose_enter();
	++no_wait_return;
	ਜੇ (debug_break_level > 0 || *p_vfile == NUL)
	    ਸੁਨੇਹਾ_scroll = TRUE;	    // always scroll up, don't overwrite

	smsg(_("Exception caught: %s"), excp->ਮੁੱਲ);
	ਸੁਨੇਹਾ_puts("\n");   // don't overwrite this either

	ਜੇ (debug_break_level > 0 || *p_vfile == NUL)
	    cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	--no_wait_return;
	ਜੇ (debug_break_level > 0)
	    ਸੁਨੇਹਾ_silent = save_ਸੁਨੇਹਾ_silent;
	ਨਹੀਂਤਾਂ
	    verbose_leave();
    }
}

/*
 * Remove an exception from the caught stack.
 */
    ਖਾਲੀ
ਅੰਤ_exception(except_T *excp)
{
    ਜੇ (excp != caught_ਢੇਰ)
	ਅੰਦਰੂਨੀ_error("finish_exception()");
    caught_ਢੇਰ = caught_ਢੇਰ->caught;
    ਜੇ (caught_ਢੇਰ != ਨਿਰਾ)
    {
	ਸੈੱਟ_vim_var_string(VV_EXCEPTION, (char_u *)caught_ਢੇਰ->ਮੁੱਲ, -1);
	ਸੈੱਟ_vim_var_list(VV_STACKTRACE, caught_ਢੇਰ->stacktrace);
	ਜੇ (*caught_ਢੇਰ->throw_name != NUL)
	{
	    size_t  IObufflen;

	    ਜੇ (caught_ਢੇਰ->throw_lnum != 0)
		IObufflen = vim_snprintf_safelen((ਅੱਖਰ *)IObuff, IOSIZE,
			_("%s, line %ld"), caught_ਢੇਰ->throw_name,
			(ਲੰਮਾ)caught_ਢੇਰ->throw_lnum);
	    ਨਹੀਂਤਾਂ
		IObufflen = vim_snprintf_safelen((ਅੱਖਰ *)IObuff, IOSIZE, "%s",
						    caught_ਢੇਰ->throw_name);
	    ਸੈੱਟ_vim_var_string(VV_THROWPOINT, IObuff, (ਪੂਰਨ)IObufflen);
	}
	ਨਹੀਂਤਾਂ
	    // throw_name not set on an exception from a command that was
	    // typed.
	    ਸੈੱਟ_vim_var_string(VV_THROWPOINT, ਨਿਰਾ, -1);
    }
    ਨਹੀਂਤਾਂ
    {
	ਸੈੱਟ_vim_var_string(VV_EXCEPTION, ਨਿਰਾ, -1);
	ਸੈੱਟ_vim_var_string(VV_THROWPOINT, ਨਿਰਾ, -1);
	ਸੈੱਟ_vim_var_list(VV_STACKTRACE, ਨਿਰਾ);
    }

    // Discard the exception, but use the finish message for 'verbose'.
    discard_exception(excp, TRUE);
}

/*
 * Save the current exception state in "estate"
 */
    ਖਾਲੀ
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
    ਖਾਲੀ
exception_state_restore(exception_state_T *estate)
{
    // Handle any outstanding exceptions before restoring the state
    ਜੇ (did_throw)
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
    ਖਾਲੀ
exception_state_clear(ਖਾਲੀ)
{
    current_exception = ਨਿਰਾ;
    did_throw = FALSE;
    need_rethrow = FALSE;
    trylevel = 0;
    did_emsg = 0;
}

/*
 * Flags specifying the message displayed by report_pending.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RP_MAKE		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RP_RESUME	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RP_DISCARD	2

/*
 * Report information about something pending in a finally clause if required by
 * the 'verbose' option or when debugging.  "action" tells whether something is
 * made pending or something pending is resumed or discarded.  "pending" tells
 * what is pending.  "value" specifies the return value for a pending ":return"
 * or the exception value for a pending exception.
 */
    ਸਥਿਰ ਖਾਲੀ
report_pending(ਪੂਰਨ action, ਪੂਰਨ pending, ਖਾਲੀ *ਮੁੱਲ)
{
    ਅੱਖਰ	*mesg;
    ਅੱਖਰ	*s;
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_silent;


    ਚੋਣ (action)
    {
	ਹਾਲਤ RP_MAKE:
	    mesg = _("%s made pending");
	    ਰੋਕੋ;
	ਹਾਲਤ RP_RESUME:
	    mesg = _("%s resumed");
	    ਰੋਕੋ;
	// case RP_DISCARD:
	ਮੂਲ:
	    mesg = _("%s discarded");
	    ਰੋਕੋ;
    }

    ਚੋਣ (pending)
    {
	ਹਾਲਤ CSTP_NONE:
	    ਵਾਪਸ;

	ਹਾਲਤ CSTP_CONTINUE:
	    s = ":continue";
	    ਰੋਕੋ;
	ਹਾਲਤ CSTP_BREAK:
	    s = ":break";
	    ਰੋਕੋ;
	ਹਾਲਤ CSTP_ਅੰਤ:
	    s = ":finish";
	    ਰੋਕੋ;
	ਹਾਲਤ CSTP_RETURN:
	    // ":return" command producing value, allocated
	    s = (ਅੱਖਰ *)get_return_cmd(ਮੁੱਲ);
	    ਰੋਕੋ;

	ਮੂਲ:
	    ਜੇ (pending & CSTP_THROW)
	    {
		vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, mesg, _("Exception"));
		mesg = (ਅੱਖਰ *)vim_strnsave(IObuff, STRLEN(IObuff) + 4);
		STRCAT(mesg, ": %s");
		s = (ਅੱਖਰ *)((except_T *)ਮੁੱਲ)->ਮੁੱਲ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((pending & CSTP_ERROR) && (pending & CSTP_ਰੁਕਾਵਟ))
		s = _("Error and interrupt");
	    ਨਹੀਂਤਾਂ ਜੇ (pending & CSTP_ERROR)
		s = _("Error");
	    ਨਹੀਂਤਾਂ // if (pending & CSTP_INTERRUPT)
		s = _("Interrupt");
    }

    save_ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent;
    ਜੇ (debug_break_level > 0)
	ਸੁਨੇਹਾ_silent = FALSE;	// display messages
    ++no_wait_return;
    ਸੁਨੇਹਾ_scroll = TRUE;		// always scroll up, don't overwrite
    smsg(mesg, s);
    ਸੁਨੇਹਾ_puts("\n");   // don't overwrite this either
    cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
    --no_wait_return;
    ਜੇ (debug_break_level > 0)
	ਸੁਨੇਹਾ_silent = save_ਸੁਨੇਹਾ_silent;

    ਜੇ (pending == CSTP_RETURN)
	vim_free(s);
    ਨਹੀਂਤਾਂ ਜੇ (pending & CSTP_THROW)
	vim_free(mesg);
}

/*
 * If something is made pending in a finally clause, report it if required by
 * the 'verbose' option or when debugging.
 */
    ਖਾਲੀ
report_make_pending(ਪੂਰਨ pending, ਖਾਲੀ *ਮੁੱਲ)
{
    ਜੇ (p_verbose >= 14 || debug_break_level > 0)
    {
	ਜੇ (debug_break_level <= 0)
	    verbose_enter();
	report_pending(RP_MAKE, pending, ਮੁੱਲ);
	ਜੇ (debug_break_level <= 0)
	    verbose_leave();
    }
}

/*
 * If something pending in a finally clause is resumed at the ":endtry", report
 * it if required by the 'verbose' option or when debugging.
 */
    ਸਥਿਰ ਖਾਲੀ
report_resume_pending(ਪੂਰਨ pending, ਖਾਲੀ *ਮੁੱਲ)
{
    ਜੇ (p_verbose >= 14 || debug_break_level > 0)
    {
	ਜੇ (debug_break_level <= 0)
	    verbose_enter();
	report_pending(RP_RESUME, pending, ਮੁੱਲ);
	ਜੇ (debug_break_level <= 0)
	    verbose_leave();
    }
}

/*
 * If something pending in a finally clause is discarded, report it if required
 * by the 'verbose' option or when debugging.
 */
    ਸਥਿਰ ਖਾਲੀ
report_discard_pending(ਪੂਰਨ pending, ਖਾਲੀ *ਮੁੱਲ)
{
    ਜੇ (p_verbose >= 14 || debug_break_level > 0)
    {
	ਜੇ (debug_break_level <= 0)
	    verbose_enter();
	report_pending(RP_DISCARD, pending, ਮੁੱਲ);
	ਜੇ (debug_break_level <= 0)
	    verbose_leave();
    }
}

/*
 * Return TRUE if "arg" is only a variable, register, environment variable,
 * option name or string.
 */
    ਪੂਰਨ
cmd_is_name_only(char_u *arg)
{
    char_u  *p = arg;
    char_u  *alias = ਨਿਰਾ;
    ਪੂਰਨ	    name_only = FALSE;

    ਜੇ (*p == '@')
    {
	++p;
	ਜੇ (*p != NUL)
	    ++p;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*p == '\'' || *p == '"')
    {
	ਪੂਰਨ	    r;

	ਜੇ (*p == '"')
	    r = eval_string(&p, ਨਿਰਾ, FALSE, FALSE);
	ਨਹੀਂਤਾਂ
	    r = eval_lit_string(&p, ਨਿਰਾ, FALSE, FALSE);
	ਜੇ (r == FAIL)
	    ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (*p == '&')
	{
	    ++p;
	    ਜੇ (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
		p += 2;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '$')
	    ++p;
	(ਖਾਲੀ)get_name_len(&p, &alias, FALSE, FALSE);
    }
    name_only = ਸਿਰੇ_excmd2(arg, skipwhite(p));
    vim_free(alias);
    ਵਾਪਸ name_only;
}

/*
 * ":eval".
 */
    ਖਾਲੀ
ex_eval(exarg_T *eap)
{
    typval_T	tv;
    evalarg_T	evalarg;
    ਪੂਰਨ		name_only = FALSE;
    ਲੰਮਾ	lnum = SOURCING_LNUM;

    ਜੇ (ਵਿੱਚ_vim9script())
	name_only = cmd_is_name_only(eap->arg);

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    ਜੇ (eval0(eap->arg, &tv, eap, &evalarg) == OK)
    {
	clear_tv(&tv);
	ਜੇ (ਵਿੱਚ_vim9script() && name_only
		&& (evalarg.eval_tofree == ਨਿਰਾ
		    || ਸਿਰੇ_excmd2(evalarg.eval_tofree,
					      skipwhite(evalarg.eval_tofree))))
	{
	    SOURCING_LNUM = lnum;
	    semsg(_(e_ਵਿਅੰਜਕ_without_effect_str), eap->arg);
	}
    }

    clear_evalarg(&evalarg, eap);
}

/*
 * Start a new scope/block.  Caller should have checked that cs_idx is not
 * exceeding CSTACK_LEN.
 */
    ਸਥਿਰ ਖਾਲੀ
enter_ਖੰਡ(cstack_T *cstack)
{
    ++cstack->cs_idx;
    ਜੇ (ਵਿੱਚ_vim9script() && current_sctx.sc_sid > 0)
    {
	scriptitem_T *si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);

	cstack->cs_script_var_len[cstack->cs_idx] = si->sn_var_vals.ga_len;
	cstack->cs_ਖੰਡ_id[cstack->cs_idx] = ++si->sn_ਆਖਰੀ_ਖੰਡ_id;
	si->sn_current_ਖੰਡ_id = si->sn_ਆਖਰੀ_ਖੰਡ_id;
    }
    ਨਹੀਂਤਾਂ
    {
	// Just in case in_vim9script() does not return the same value when the
	// block ends.
	cstack->cs_script_var_len[cstack->cs_idx] = 0;
	cstack->cs_ਖੰਡ_id[cstack->cs_idx] = 0;
    }
}

    ਸਥਿਰ ਖਾਲੀ
leave_ਖੰਡ(cstack_T *cstack)
{
    ਜੇ (ਵਿੱਚ_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	scriptitem_T	*si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
	ਪੂਰਨ		i;
	ਪੂਰਨ		func_defined =
			       cstack->cs_flags[cstack->cs_idx] & CSF_FUNC_DEF;

	ਲਈ (i = cstack->cs_script_var_len[cstack->cs_idx];
					       i < si->sn_var_vals.ga_len; ++i)
	{
	    svar_T	*sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + i;

	    // sv_name is set to NULL if it was already removed.  This happens
	    // when it was defined in an inner block and no functions were
	    // defined there.
	    ਜੇ (sv->sv_name != ਨਿਰਾ)
		// Remove a variable declared inside the block, if it still
		// exists, from sn_vars and move the value into sn_all_vars
		// if "func_defined" is non-zero.
		hide_script_var(si, i, func_defined);
	}

	ਜੇ (cstack->cs_idx == 0)
	    si->sn_current_ਖੰਡ_id = 0;
	ਨਹੀਂਤਾਂ
	    si->sn_current_ਖੰਡ_id = cstack->cs_ਖੰਡ_id[cstack->cs_idx - 1];
    }
    --cstack->cs_idx;
}

/*
 * ":if".
 */
    ਖਾਲੀ
ex_if(exarg_T *eap)
{
    ਪੂਰਨ		error;
    ਪੂਰਨ		skip;
    ਪੂਰਨ		ਨਤੀਜਾ;
    cstack_T	*cstack = eap->cstack;

    ਜੇ (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_if_nesting_too_deep);
    ਨਹੀਂਤਾਂ
    {
	enter_ਖੰਡ(cstack);
	cstack->cs_flags[cstack->cs_idx] = 0;

	/*
	 * Don't do something after an error, interrupt, or throw, or when
	 * there is a surrounding conditional and it was not active.
	 */
	skip = did_emsg || got_int || did_throw || (cstack->cs_idx > 0
		&& !(cstack->cs_flags[cstack->cs_idx - 1] & CSF_ਸਰਗਰਮ));

	ਨਤੀਜਾ = eval_ਮੰਜ਼ਿਲ_bool(eap->arg, &error, eap, skip, FALSE);

	ਜੇ (!skip && !error)
	{
	    ਜੇ (ਨਤੀਜਾ)
		cstack->cs_flags[cstack->cs_idx] = CSF_ਸਰਗਰਮ | CSF_TRUE;
	}
	ਨਹੀਂਤਾਂ
	    // set TRUE, so this conditional will never get active
	    cstack->cs_flags[cstack->cs_idx] = CSF_TRUE;
    }
}

/*
 * ":endif".
 */
    ਖਾਲੀ
ex_endif(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;

    ਜੇ (cmdmod_error(FALSE))
	ਵਾਪਸ;
    did_endif = TRUE;
    ਜੇ (cstack->cs_idx < 0
	    || (cstack->cs_flags[cstack->cs_idx]
				& (CSF_WHILE | CSF_FOR | CSF_TRY | CSF_ਖੰਡ)))
	eap->errmsg = _(e_endif_without_if);
    ਨਹੀਂਤਾਂ
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
	ਜੇ (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRUE)
						    && dbg_check_skipped(eap))
	    (ਖਾਲੀ)do_intthrow(cstack);

	leave_ਖੰਡ(cstack);
    }
}

/*
 * ":else" and ":elseif".
 */
    ਖਾਲੀ
ex_else(exarg_T *eap)
{
    ਪੂਰਨ		error;
    ਪੂਰਨ		skip;
    ਪੂਰਨ		ਨਤੀਜਾ;
    cstack_T	*cstack = eap->cstack;

    /*
     * Don't do something after an error, interrupt, or throw, or when there is
     * a surrounding conditional and it was not active.
     */
    skip = did_emsg || got_int || did_throw || (cstack->cs_idx > 0
	    && !(cstack->cs_flags[cstack->cs_idx - 1] & CSF_ਸਰਗਰਮ));

    ਜੇ (cstack->cs_idx < 0
	    || (cstack->cs_flags[cstack->cs_idx]
				& (CSF_WHILE | CSF_FOR | CSF_TRY | CSF_ਖੰਡ)))
    {
	ਜੇ (eap->cmdidx == CMD_else)
	{
	    eap->errmsg = _(e_else_without_if);
	    ਵਾਪਸ;
	}
	eap->errmsg = _(e_elseif_without_if);
	skip = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (cstack->cs_flags[cstack->cs_idx] & CSF_ELSE)
    {
	ਜੇ (eap->cmdidx == CMD_else)
	{
	    eap->errmsg = _(e_ਗੁਣਜ_else);
	    ਵਾਪਸ;
	}
	eap->errmsg = _(e_elseif_after_else);
	skip = TRUE;
    }

    ਜੇ (cstack->cs_idx >= 0)
    {
	// Variables declared in the previous block can no longer be
	// used.  Needs to be done before setting "cs_flags".
	leave_ਖੰਡ(cstack);
	enter_ਖੰਡ(cstack);
    }

    // if skipping or the ":if" was TRUE, reset ACTIVE, otherwise set it
    ਜੇ (skip || cstack->cs_flags[cstack->cs_idx] & CSF_TRUE)
    {
	ਜੇ (eap->errmsg == ਨਿਰਾ)
	    cstack->cs_flags[cstack->cs_idx] = CSF_TRUE;
	skip = TRUE;	// don't evaluate an ":elseif"
    }
    ਨਹੀਂਤਾਂ
	cstack->cs_flags[cstack->cs_idx] = CSF_ਸਰਗਰਮ;

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
    ਜੇ (!skip && dbg_check_skipped(eap) && got_int)
    {
	(ਖਾਲੀ)do_intthrow(cstack);
	skip = TRUE;
    }

    ਜੇ (eap->cmdidx == CMD_elseif)
    {
	// When skipping we ignore most errors, but a missing expression is
	// wrong, perhaps it should have been "else".
	// A double quote here is the start of a string, not a comment.
	ਜੇ (skip && *eap->arg != '"' && ਸਿਰੇ_excmd(*eap->arg))
	    semsg(_(e_invalid_ਵਿਅੰਜਕ_str), eap->arg);
	ਨਹੀਂਤਾਂ
	    ਨਤੀਜਾ = eval_ਮੰਜ਼ਿਲ_bool(eap->arg, &error, eap, skip, FALSE);

	// When throwing error exceptions, we want to throw always the first
	// of several errors in a row.  This is what actually happens when
	// a conditional error was detected above and there is another failure
	// when parsing the expression.  Since the skip flag is set in this
	// case, the parsing error will be ignored by emsg().
	ਜੇ (!skip && !error)
	{
	    ਜੇ (ਨਤੀਜਾ)
		cstack->cs_flags[cstack->cs_idx] = CSF_ਸਰਗਰਮ | CSF_TRUE;
	    ਨਹੀਂਤਾਂ
		cstack->cs_flags[cstack->cs_idx] = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (eap->errmsg == ਨਿਰਾ)
	    // set TRUE, so this conditional will never get active
	    cstack->cs_flags[cstack->cs_idx] = CSF_TRUE;
    }
    ਨਹੀਂਤਾਂ
	cstack->cs_flags[cstack->cs_idx] |= CSF_ELSE;
}

/*
 * Handle ":while" and ":for".
 */
    ਖਾਲੀ
ex_while(exarg_T *eap)
{
    ਪੂਰਨ		error;
    ਪੂਰਨ		skip;
    ਪੂਰਨ		ਨਤੀਜਾ;
    cstack_T	*cstack = eap->cstack;
    ਪੂਰਨ		prev_cs_flags = 0;

    ਜੇ (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_while_for_nesting_too_deep);
    ਨਹੀਂਤਾਂ
    {
	/*
	 * The loop flag is set when we have jumped back from the matching
	 * ":endwhile" or ":endfor".  When not set, need to initialise this
	 * cstack entry.
	 */
	ਜੇ ((cstack->cs_lflags & CSL_HAD_LOOP) == 0)
	{
	    enter_ਖੰਡ(cstack);
	    ++cstack->cs_looplevel;
	    cstack->cs_ਸਤਰ[cstack->cs_idx] = -1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ਵਿੱਚ_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid))
	    {
		scriptitem_T	*si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
		ਪੂਰਨ		i;
		ਪੂਰਨ		ਪਹਿਲਾ;
		ਪੂਰਨ		func_defined = cstack->cs_flags[cstack->cs_idx]
								& CSF_FUNC_DEF;

		// Any variables defined in the previous round are no longer
		// visible.  Keep the first one for ":for", it is the loop
		// variable that we reuse every time around.
		// Do this backwards, so that vars defined in a later round are
		// found first.
		ਪਹਿਲਾ = cstack->cs_script_var_len[cstack->cs_idx];
		ਜੇ (eap->cmdidx == CMD_for)
		{
		    forinfo_T	*fi = cstack->cs_forinfo[cstack->cs_idx];

		    ਪਹਿਲਾ += fi == ਨਿਰਾ || fi->fi_varcount == 0
							 ? 1 : fi->fi_varcount;
		}
		ਲਈ (i = si->sn_var_vals.ga_len - 1; i >= ਪਹਿਲਾ; --i)
		{
		    svar_T	*sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + i;

		    // sv_name is set to NULL if it was already removed.  This
		    // happens when it was defined in an inner block and no
		    // functions were defined there.
		    ਜੇ (sv->sv_name != ਨਿਰਾ)
			// Remove a variable declared inside the block, if it
			// still exists, from sn_vars.
			hide_script_var(si, i, func_defined);
		}

		// Start a new block ID, so that variables defined inside the
		// loop are created new and not shared with the previous loop.
		// Matters when used in a closure.
		cstack->cs_ਖੰਡ_id[cstack->cs_idx] = ++si->sn_ਆਖਰੀ_ਖੰਡ_id;
		si->sn_current_ਖੰਡ_id = si->sn_ਆਖਰੀ_ਖੰਡ_id;
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
		&& !(cstack->cs_flags[cstack->cs_idx - 1] & CSF_ਸਰਗਰਮ));
	ਜੇ (eap->cmdidx == CMD_while)
	{
	    /*
	     * ":while bool-expr"
	     */
	    ਨਤੀਜਾ = eval_ਮੰਜ਼ਿਲ_bool(eap->arg, &error, eap, skip, FALSE);
	}
	ਨਹੀਂਤਾਂ
	{
	    forinfo_T	*fi;
	    evalarg_T	evalarg;

	    /*
	     * ":for var in list-expr"
	     */
	    fill_evalarg_from_eap(&evalarg, eap, skip);
	    ਜੇ ((cstack->cs_lflags & CSL_HAD_LOOP) != 0)
	    {
		// Jumping here from a ":continue" or ":endfor": use the
		// previously evaluated list.
		fi = cstack->cs_forinfo[cstack->cs_idx];
		error = FALSE;

		// the "in expr" is not used, skip over it
		skip_for_ਸਤਰਾਂ(fi, &evalarg);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਲੰਮਾ save_lnum = SOURCING_LNUM;

		// Evaluate the argument and get the info in a structure.
		fi = eval_for_ਸਤਰ(eap->arg, &error, eap, &evalarg);
		cstack->cs_forinfo[cstack->cs_idx] = fi;

		// Errors should use the first line number.
		SOURCING_LNUM = save_lnum;
	    }

	    // use the element at the start of the list and advance
	    ਜੇ (!error && fi != ਨਿਰਾ && !skip)
		ਨਤੀਜਾ = ਅਗਲਾ_for_ਵਸਤੂ(fi, eap->arg);
	    ਨਹੀਂਤਾਂ
		ਨਤੀਜਾ = FALSE;
	    ਜੇ (fi != ਨਿਰਾ)
		// OR all the cs_flags together, if a function was defined in
		// any round then the loop variable may have been used.
		fi->fi_cs_flags |= prev_cs_flags;

	    ਜੇ (!ਨਤੀਜਾ)
	    {
		// If a function was defined in any round then set the
		// CSF_FUNC_DEF flag now, so that it's seen by leave_block().
		ਜੇ (fi != ਨਿਰਾ && (fi->fi_cs_flags & CSF_FUNC_DEF))
		    cstack->cs_flags[cstack->cs_idx] |= CSF_FUNC_DEF;

		free_for_info(fi);
		cstack->cs_forinfo[cstack->cs_idx] = ਨਿਰਾ;
	    }
	    clear_evalarg(&evalarg, eap);
	}

	/*
	 * If this cstack entry was just initialised and is active, set the
	 * loop flag, so do_cmdline() will set the line number in cs_line[].
	 * If executing the command a second time, clear the loop flag.
	 */
	ਜੇ (!skip && !error && ਨਤੀਜਾ)
	{
	    cstack->cs_flags[cstack->cs_idx] |= (CSF_ਸਰਗਰਮ | CSF_TRUE);
	    cstack->cs_lflags ^= CSL_HAD_LOOP;
	}
	ਨਹੀਂਤਾਂ
	{
	    cstack->cs_lflags &= ~CSL_HAD_LOOP;
	    // If the ":while" evaluates to FALSE or ":for" is past the end of
	    // the list, show the debug prompt at the ":endwhile"/":endfor" as
	    // if there was a ":break" in a ":while"/":for" evaluating to
	    // TRUE.
	    ਜੇ (!skip && !error)
		cstack->cs_flags[cstack->cs_idx] |= CSF_TRUE;
	}
    }
}

/*
 * ":continue"
 */
    ਖਾਲੀ
ex_continue(exarg_T *eap)
{
    ਪੂਰਨ		idx;
    cstack_T	*cstack = eap->cstack;

    ਜੇ (cstack->cs_looplevel <= 0 || cstack->cs_idx < 0)
	eap->errmsg = _(e_continue_without_while_or_for);
    ਨਹੀਂਤਾਂ
    {
	// Try to find the matching ":while".  This might stop at a try
	// conditional not in its finally clause (which is then to be executed
	// next).  Therefore, inactivate all conditionals except the ":while"
	// itself (if reached).
	idx = cleanup_conditionals(cstack, CSF_WHILE | CSF_FOR, FALSE);
	ਜੇ (idx >= 0 && (cstack->cs_flags[idx] & (CSF_WHILE | CSF_FOR)))
	{
	    rewind_conditionals(cstack, idx, CSF_TRY, &cstack->cs_trylevel);

	    /*
	     * Set CSL_HAD_CONT, so do_cmdline() will jump back to the
	     * matching ":while".
	     */
	    cstack->cs_lflags |= CSL_HAD_CONT;	// let do_cmdline() handle it
	}
	ਨਹੀਂਤਾਂ
	{
	    // If a try conditional not in its finally clause is reached first,
	    // make the ":continue" pending for execution at the ":endtry".
	    cstack->cs_pending[idx] = CSTP_CONTINUE;
	    report_make_pending(CSTP_CONTINUE, ਨਿਰਾ);
	}
    }
}

/*
 * ":break"
 */
    ਖਾਲੀ
ex_break(exarg_T *eap)
{
    ਪੂਰਨ		idx;
    cstack_T	*cstack = eap->cstack;

    ਜੇ (cstack->cs_looplevel <= 0 || cstack->cs_idx < 0)
	eap->errmsg = _(e_break_without_while_or_for);
    ਨਹੀਂਤਾਂ
    {
	// Inactivate conditionals until the matching ":while" or a try
	// conditional not in its finally clause (which is then to be
	// executed next) is found.  In the latter case, make the ":break"
	// pending for execution at the ":endtry".
	idx = cleanup_conditionals(cstack, CSF_WHILE | CSF_FOR, TRUE);
	ਜੇ (idx >= 0 && !(cstack->cs_flags[idx] & (CSF_WHILE | CSF_FOR)))
	{
	    cstack->cs_pending[idx] = CSTP_BREAK;
	    report_make_pending(CSTP_BREAK, ਨਿਰਾ);
	}
    }
}

/*
 * ":endwhile" and ":endfor"
 */
    ਖਾਲੀ
ex_endwhile(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;
    ਪੂਰਨ		idx;
    ਅੱਖਰ	*err;
    ਪੂਰਨ		csf;
    ਪੂਰਨ		fl;

    ਜੇ (cmdmod_error(TRUE))
	ਵਾਪਸ;

    ਜੇ (eap->cmdidx == CMD_endwhile)
    {
	err = e_endwhile_without_while;
	csf = CSF_WHILE;
    }
    ਨਹੀਂਤਾਂ
    {
	err = e_endfor_without_for;
	csf = CSF_FOR;
    }

    ਜੇ (cstack->cs_looplevel <= 0 || cstack->cs_idx < 0)
	eap->errmsg = _(err);
    ਨਹੀਂਤਾਂ
    {
	fl = cstack->cs_flags[cstack->cs_idx];
	ਜੇ (!(fl & csf))
	{
	    // If we are in a ":while" or ":for" but used the wrong endloop
	    // command, do not rewind to the next enclosing ":for"/":while".
	    ਜੇ (fl & CSF_WHILE)
		eap->errmsg = _(e_using_endfor_with_while);
	    ਨਹੀਂਤਾਂ ਜੇ (fl & CSF_FOR)
		eap->errmsg = _(e_using_endwhile_with_for);
	}
	ਜੇ (!(fl & (CSF_WHILE | CSF_FOR)))
	{
	    ਜੇ (!(fl & CSF_TRY))
		eap->errmsg = _(e_missing_endif);
	    ਨਹੀਂਤਾਂ ਜੇ (fl & CSF_FINALLY)
		eap->errmsg = _(e_missing_endtry);
	    // Try to find the matching ":while" and report what's missing.
	    ਲਈ (idx = cstack->cs_idx; idx > 0; --idx)
	    {
		fl =  cstack->cs_flags[idx];
		ਜੇ ((fl & CSF_TRY) && !(fl & CSF_FINALLY))
		{
		    // Give up at a try conditional not in its finally clause.
		    // Ignore the ":endwhile"/":endfor".
		    eap->errmsg = _(err);
		    ਵਾਪਸ;
		}
		ਜੇ (fl & csf)
		    ਰੋਕੋ;
	    }
	    // Cleanup and rewind all contained (and unclosed) conditionals.
	    (ਖਾਲੀ)cleanup_conditionals(cstack, CSF_WHILE | CSF_FOR, FALSE);
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
	ਨਹੀਂਤਾਂ ਜੇ (cstack->cs_flags[cstack->cs_idx] & CSF_TRUE
		&& !(cstack->cs_flags[cstack->cs_idx] & CSF_ਸਰਗਰਮ)
		&& dbg_check_skipped(eap))
	    (ਖਾਲੀ)do_intthrow(cstack);

	// Set loop flag, so do_cmdline() will jump back to the matching
	// ":while" or ":for".
	cstack->cs_lflags |= CSL_HAD_ENDLOOP;
    }
}

/*
 * "{" start of a block in Vim9 script
 */
    ਖਾਲੀ
ex_ਖੰਡ(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;

    ਜੇ (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_ਖੰਡ_nesting_too_deep);
    ਨਹੀਂਤਾਂ
    {
	enter_ਖੰਡ(cstack);
	cstack->cs_flags[cstack->cs_idx] = CSF_ਖੰਡ | CSF_ਸਰਗਰਮ | CSF_TRUE;
    }
}

/*
 * "}" end of a block in Vim9 script
 */
    ਖਾਲੀ
ex_endblock(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;

    ਜੇ (cstack->cs_idx < 0
	    || (cstack->cs_flags[cstack->cs_idx] & CSF_ਖੰਡ) == 0)
	eap->errmsg = _(e_endblock_without_ਖੰਡ);
    ਨਹੀਂਤਾਂ
	leave_ਖੰਡ(cstack);
}

    ਪੂਰਨ
inside_ਖੰਡ(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;
    ਪੂਰਨ		i;

    // Control flow commands handle '|' themselves and must not be treated
    // as if inside a block even when they are - otherwise '|' separators
    // in "try | catch | endtry" etc. would not be recognized.
    ਚੋਣ (eap->cmdidx)
    {
	ਹਾਲਤ CMD_if:
	ਹਾਲਤ CMD_elseif:
	ਹਾਲਤ CMD_else:
	ਹਾਲਤ CMD_endif:
	ਹਾਲਤ CMD_while:
	ਹਾਲਤ CMD_endwhile:
	ਹਾਲਤ CMD_for:
	ਹਾਲਤ CMD_endfor:
	ਹਾਲਤ CMD_try:
	ਹਾਲਤ CMD_catch:
	ਹਾਲਤ CMD_finally:
	ਹਾਲਤ CMD_endtry:
	    ਵਾਪਸ FALSE;
	ਮੂਲ:
	    ਰੋਕੋ;
    }

    ਲਈ (i = 0; i <= cstack->cs_idx; ++i)
	ਜੇ (cstack->cs_flags[i] & CSF_ਖੰਡ)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * ":throw expr"
 */
    ਖਾਲੀ
ex_throw(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*ਮੁੱਲ;

    ਜੇ (*arg != NUL && *arg != '|' && *arg != '\n')
	ਮੁੱਲ = eval_ਮੰਜ਼ਿਲ_string_skip(arg, eap, eap->skip);
    ਨਹੀਂਤਾਂ
    {
	emsg(_(e_ਦਲੀਲ_required));
	ਮੁੱਲ = ਨਿਰਾ;
    }

    // On error or when an exception is thrown during argument evaluation, do
    // not throw.
    ਜੇ (!eap->skip && ਮੁੱਲ != ਨਿਰਾ)
    {
	ਜੇ (throw_exception(ਮੁੱਲ, ET_USER, ਨਿਰਾ) == FAIL)
	    vim_free(ਮੁੱਲ);
	ਨਹੀਂਤਾਂ
	    do_throw(eap->cstack);
    }
}

/*
 * Throw the current exception through the specified cstack.  Common routine
 * for ":throw" (user exception) and error and interrupt exceptions.  Also
 * used for rethrowing an uncaught exception.
 */
    ਖਾਲੀ
do_throw(cstack_T *cstack)
{
    ਪੂਰਨ		idx;
    ਪੂਰਨ		inactivate_try = FALSE;

    /*
     * Cleanup and inactivate up to the next surrounding try conditional that
     * is not in its finally clause.  Normally, do not inactivate the try
     * conditional itself, so that its ACTIVE flag can be tested below.  But
     * if a previous error or interrupt has not been converted to an exception,
     * inactivate the try conditional, too, as if the conversion had been done,
     * and reset the did_emsg or got_int flag, so this won't happen again at
     * the next surrounding try conditional.
     */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ THROW_ON_ERROR_TRUE
    ਜੇ (did_emsg && !THROW_ON_ERROR)
    {
	inactivate_try = TRUE;
	did_emsg = FALSE;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ THROW_ON_ਰੁਕਾਵਟ_TRUE
    ਜੇ (got_int && !THROW_ON_ਰੁਕਾਵਟ)
    {
	inactivate_try = TRUE;
	got_int = FALSE;
    }
#ਜੇ_ਅੰਤ
    idx = cleanup_conditionals(cstack, 0, inactivate_try);
    ਜੇ (idx >= 0)
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
	ਜੇ (!(cstack->cs_flags[idx] & CSF_CAUGHT))
	{
	    ਜੇ (cstack->cs_flags[idx] & CSF_ਸਰਗਰਮ)
		cstack->cs_flags[idx] |= CSF_THROWN;
	    ਨਹੀਂਤਾਂ
		// THROWN may have already been set for a catchable exception
		// that has been discarded.  Ensure it is reset for the new
		// exception.
		cstack->cs_flags[idx] &= ~CSF_THROWN;
	}
	cstack->cs_flags[idx] &= ~CSF_ਸਰਗਰਮ;
	cstack->cs_exception[idx] = current_exception;
    }
#ਜੇ 0
    // TODO: Add optimization below.  Not yet done because of interface
    // problems to eval.c and ex_cmds2.c. (Servatius)
    ਨਹੀਂਤਾਂ
    {
	/*
	 * There are no catch clauses to check or finally clauses to execute.
	 * End the current script or function.  The exception will be rethrown
	 * in the caller.
	 */
	ਜੇ (getline_equal(eap->getline, eap->cookie, get_func_ਸਤਰ))
	    current_funccal->returned = TRUE;
	elseif (eap->get_func_ਸਤਰ == getsourceline)
	    ((ਬਣਤਰ ਸਰੋਤ_cookie *)eap->cookie)->ਮੁਕੰਮਲ = TRUE;
    }
#ਜੇ_ਅੰਤ

    did_throw = TRUE;
}

/*
 * ":try"
 */
    ਖਾਲੀ
ex_try(exarg_T *eap)
{
    ਪੂਰਨ		skip;
    cstack_T	*cstack = eap->cstack;

    ਜੇ (cmdmod_error(FALSE))
	ਵਾਪਸ;

    ਜੇ (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_try_nesting_too_deep);
    ਨਹੀਂਤਾਂ
    {
	enter_ਖੰਡ(cstack);
	++cstack->cs_trylevel;
	cstack->cs_flags[cstack->cs_idx] = CSF_TRY;
	cstack->cs_pending[cstack->cs_idx] = CSTP_NONE;

	/*
	 * Don't do something after an error, interrupt, or throw, or when there
	 * is a surrounding conditional and it was not active.
	 */
	skip = did_emsg || got_int || did_throw || (cstack->cs_idx > 0
		&& !(cstack->cs_flags[cstack->cs_idx - 1] & CSF_ਸਰਗਰਮ));

	ਜੇ (!skip)
	{
	    // Set ACTIVE and TRUE.  TRUE means that the corresponding ":catch"
	    // commands should check for a match if an exception is thrown and
	    // that the finally clause needs to be executed.
	    cstack->cs_flags[cstack->cs_idx] |= CSF_ਸਰਗਰਮ | CSF_TRUE;

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
	    ਜੇ (emsg_silent)
	    {
		eslist_T	*elem;

		elem = ALLOC_ONE(ਬਣਤਰ eslist_elem);
		ਜੇ (elem == ਨਿਰਾ)
		    emsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ));
		ਨਹੀਂਤਾਂ
		{
		    elem->saved_emsg_silent = emsg_silent;
		    elem->ਅਗਲਾ = cstack->cs_emsg_silent_list;
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
    ਖਾਲੀ
ex_catch(exarg_T *eap)
{
    ਪੂਰਨ		idx = 0;
    ਪੂਰਨ		give_ਉੱਪਰ = FALSE;
    ਪੂਰਨ		skip = FALSE;
    ਪੂਰਨ		caught = FALSE;
    char_u	*ਸਿਰਾ;
    ਪੂਰਨ		save_char = 0;
    char_u	*save_cpo;
    regmatch_T	regmatch;
    ਪੂਰਨ		prev_got_int;
    cstack_T	*cstack = eap->cstack;
    char_u	*pat;

    ਜੇ (cmdmod_error(FALSE))
	ਵਾਪਸ;

    ਜੇ (cstack->cs_trylevel <= 0 || cstack->cs_idx < 0)
    {
	eap->errmsg = _(e_catch_without_try);
	give_ਉੱਪਰ = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
	{
	    // Report what's missing if the matching ":try" is not in its
	    // finally clause.
	    eap->errmsg = get_ਸਿਰਾ_emsg(cstack);
	    skip = TRUE;
	}
	ਲਈ (idx = cstack->cs_idx; idx > 0; --idx)
	    ਜੇ (cstack->cs_flags[idx] & CSF_TRY)
		ਰੋਕੋ;
	ਜੇ (cstack->cs_flags[idx] & CSF_TRY)
	    cstack->cs_flags[idx] |= CSF_CATCH;
	ਜੇ (cstack->cs_flags[idx] & CSF_FINALLY)
	{
	    // Give up for a ":catch" after ":finally" and ignore it.
	    // Just parse.
	    eap->errmsg = _(e_catch_after_finally);
	    give_ਉੱਪਰ = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    rewind_conditionals(cstack, idx, CSF_WHILE | CSF_FOR,
						       &cstack->cs_looplevel);
    }

    ਜੇ (ਸਿਰੇ_excmd2(eap->cmd, eap->arg))   // no argument, catch all errors
    {
	pat = (char_u *)".*";
	ਸਿਰਾ = ਨਿਰਾ;
	eap->nextcmd = ਲੱਭੋ_nextcmd(eap->arg);
    }
    ਨਹੀਂਤਾਂ
    {
	pat = eap->arg + 1;
	ਸਿਰਾ = skip_regexp_err(pat, *eap->arg, TRUE);
	ਜੇ (ਸਿਰਾ == ਨਿਰਾ)
	    give_ਉੱਪਰ = TRUE;
    }

    ਜੇ (!give_ਉੱਪਰ)
    {
	/*
	 * Don't do something when no exception has been thrown or when the
	 * corresponding try block never got active (because of an inactive
	 * surrounding conditional or after an error or interrupt or throw).
	 */
	ਜੇ (!did_throw || !(cstack->cs_flags[idx] & CSF_TRUE))
	    skip = TRUE;

	/*
	 * Check for a match only if an exception is thrown but not caught by
	 * a previous ":catch".  An exception that has replaced a discarded
	 * exception is not checked (THROWN is not set then).
	 */
	ਜੇ (!skip && (cstack->cs_flags[idx] & CSF_THROWN)
		&& !(cstack->cs_flags[idx] & CSF_CAUGHT))
	{
	    ਜੇ (ਸਿਰਾ != ਨਿਰਾ && *ਸਿਰਾ != NUL
				      && !ਸਿਰੇ_excmd2(ਸਿਰਾ, skipwhite(ਸਿਰਾ + 1)))
	    {
		semsg(_(e_trailing_characters_str), ਸਿਰਾ);
		ਵਾਪਸ;
	    }

	    // When debugging or a breakpoint was encountered, display the
	    // debug prompt (if not already done) before checking for a match.
	    // This is a helpful hint for the user when the regular expression
	    // matching fails.  Handle a ">quit" debug command as if an
	    // interrupt had occurred before the ":catch".  That is, discard
	    // the original exception, replace it by an interrupt exception,
	    // and don't catch it in this try block.
	    ਜੇ (!dbg_check_skipped(eap) || !do_intthrow(cstack))
	    {
		// Terminate the pattern and avoid the 'l' flag in 'cpoptions'
		// while compiling it.
		ਜੇ (ਸਿਰਾ != ਨਿਰਾ)
		{
		    save_char = *ਸਿਰਾ;
		    *ਸਿਰਾ = NUL;
		}
		save_cpo  = p_cpo;
		p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;
		// Disable error messages, it will make current_exception
		// invalid.
		++emsg_off;
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
		--emsg_off;
		regmatch.rm_ic = FALSE;
		ਜੇ (ਸਿਰਾ != ਨਿਰਾ)
		    *ਸਿਰਾ = save_char;
		p_cpo = save_cpo;
		ਜੇ (regmatch.regprog == ਨਿਰਾ)
		    semsg(_(e_invalid_ਦਲੀਲ_str), pat);
		ਨਹੀਂਤਾਂ
		{
		    /*
		     * Save the value of got_int and reset it.  We don't want
		     * a previous interruption cancel matching, only hitting
		     * CTRL-C while matching should abort it.
		     */
		    prev_got_int = got_int;
		    got_int = FALSE;
		    caught = vim_regexec_nl(&regmatch,
			       (char_u *)current_exception->ਮੁੱਲ, (colnr_T)0);
		    got_int |= prev_got_int;
		    vim_regfree(regmatch.regprog);
		}
	    }
	}

	ਜੇ (caught)
	{
	    // Make this ":catch" clause active and reset did_emsg, got_int,
	    // and did_throw.  Put the exception on the caught stack.
	    cstack->cs_flags[idx] |= CSF_ਸਰਗਰਮ | CSF_CAUGHT;
	    did_emsg = got_int = did_throw = FALSE;
	    catch_exception((except_T *)cstack->cs_exception[idx]);

	    ਜੇ (cstack->cs_idx >= 0
			       && (cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
	    {
		// Variables declared in the previous block can no longer be
		// used.
		leave_ਖੰਡ(cstack);
		enter_ਖੰਡ(cstack);
	    }

	    // It's mandatory that the current exception is stored in the cstack
	    // so that it can be discarded at the next ":catch", ":finally", or
	    // ":endtry" or when the catch clause is left by a ":continue",
	    // ":break", ":return", ":finish", error, interrupt, or another
	    // exception.
	    ਜੇ (cstack->cs_exception[cstack->cs_idx] != current_exception)
		ਅੰਦਰੂਨੀ_error("ex_catch()");
	}
	ਨਹੀਂਤਾਂ
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

    ਜੇ (ਸਿਰਾ != ਨਿਰਾ)
	eap->nextcmd = ਲੱਭੋ_nextcmd(ਸਿਰਾ);
}

/*
 * ":finally"
 */
    ਖਾਲੀ
ex_finally(exarg_T *eap)
{
    ਪੂਰਨ		idx;
    ਪੂਰਨ		skip = FALSE;
    ਪੂਰਨ		pending = CSTP_NONE;
    cstack_T	*cstack = eap->cstack;

    ਜੇ (cmdmod_error(FALSE))
	ਵਾਪਸ;

    ਲਈ (idx = cstack->cs_idx; idx >= 0; --idx)
	ਜੇ (cstack->cs_flags[idx] & CSF_TRY)
	    ਰੋਕੋ;
    ਜੇ (cstack->cs_trylevel <= 0 || idx < 0)
    {
	eap->errmsg = _(e_finally_without_try);
	ਵਾਪਸ;
    }

    ਜੇ (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
    {
	eap->errmsg = get_ਸਿਰਾ_emsg(cstack);
	// Make this error pending, so that the commands in the following
	// finally clause can be executed.  This overrules also a pending
	// ":continue", ":break", ":return", or ":finish".
	pending = CSTP_ERROR;
    }

    ਜੇ (cstack->cs_flags[idx] & CSF_FINALLY)
    {
	// Give up for a multiple ":finally" and ignore it.
	eap->errmsg = _(e_ਗੁਣਜ_finally);
	ਵਾਪਸ;
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

    ਜੇ (!skip)
    {
	// When debugging or a breakpoint was encountered, display the
	// debug prompt (if not already done).  The user then knows that the
	// finally clause is executed.
	ਜੇ (dbg_check_skipped(eap))
	{
	    // Handle a ">quit" debug command as if an interrupt had
	    // occurred before the ":finally".  That is, discard the
	    // original exception and replace it by an interrupt
	    // exception.
	    (ਖਾਲੀ)do_intthrow(cstack);
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

	ਜੇ (cstack->cs_idx >= 0
			   && (cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
	{
	    // Variables declared in the previous block can no longer be
	    // used.
	    leave_ਖੰਡ(cstack);
	    enter_ਖੰਡ(cstack);
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
	ਜੇ (pending == CSTP_ERROR || did_emsg || got_int || did_throw)
	{
	    ਜੇ (cstack->cs_pending[cstack->cs_idx] == CSTP_RETURN)
	    {
		report_discard_pending(CSTP_RETURN,
				       cstack->cs_rettv[cstack->cs_idx]);
		discard_pending_return(cstack->cs_rettv[cstack->cs_idx]);
	    }
	    ਜੇ (pending == CSTP_ERROR && !did_emsg)
		pending |= (THROW_ON_ERROR) ? CSTP_THROW : 0;
	    ਨਹੀਂਤਾਂ
		pending |= did_throw ? CSTP_THROW : 0;
	    pending |= did_emsg  ? CSTP_ERROR     : 0;
	    pending |= got_int   ? CSTP_ਰੁਕਾਵਟ : 0;
	    cstack->cs_pending[cstack->cs_idx] = pending;

	    // It's mandatory that the current exception is stored in the
	    // cstack so that it can be rethrown at the ":endtry" or be
	    // discarded if the finally clause is left by a ":continue",
	    // ":break", ":return", ":finish", error, interrupt, or another
	    // exception.  When emsg() is called for a missing ":endif" or
	    // a missing ":endwhile"/":endfor" detected here, the
	    // exception will be discarded.
	    ਜੇ (did_throw && cstack->cs_exception[cstack->cs_idx]
						     != current_exception)
		ਅੰਦਰੂਨੀ_error("ex_finally()");
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
    ਖਾਲੀ
ex_endtry(exarg_T *eap)
{
    ਪੂਰਨ		idx;
    ਪੂਰਨ		skip;
    ਪੂਰਨ		rethrow = FALSE;
    ਪੂਰਨ		pending = CSTP_NONE;
    ਖਾਲੀ	*rettv = ਨਿਰਾ;
    cstack_T	*cstack = eap->cstack;

    ਜੇ (cmdmod_error(FALSE))
	ਵਾਪਸ;

    ਲਈ (idx = cstack->cs_idx; idx >= 0; --idx)
	ਜੇ (cstack->cs_flags[idx] & CSF_TRY)
	    ਰੋਕੋ;
    ਜੇ (cstack->cs_trylevel <= 0 || idx < 0)
    {
	eap->errmsg = _(e_endtry_without_try);
	ਵਾਪਸ;
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

    ਜੇ (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
    {
	eap->errmsg = get_ਸਿਰਾ_emsg(cstack);

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
	ਜੇ (did_throw)
	    discard_current_exception();

	// report eap->errmsg, also when there already was an error
	did_emsg = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	idx = cstack->cs_idx;

	// Check the flags only when not in a skipped block.
	ਜੇ (!skip && ਵਿੱਚ_vim9script()
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
	ਜੇ (did_throw && (cstack->cs_flags[idx] & CSF_TRUE)
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
    ਜੇ ((rethrow || (!skip && !(cstack->cs_flags[idx] & CSF_FINALLY)
		    && !cstack->cs_pending[idx]))
	    && dbg_check_skipped(eap))
    {
	// Handle a ">quit" debug command as if an interrupt had occurred
	// before the ":endtry".  That is, throw an interrupt exception and
	// set "skip" and "rethrow".
	ਜੇ (got_int)
	{
	    skip = TRUE;
	    (ਖਾਲੀ)do_intthrow(cstack);
	    // The do_intthrow() call may have reset did_throw or
	    // cstack->cs_pending[idx].
	    rethrow = FALSE;
	    ਜੇ (did_throw && !(cstack->cs_flags[idx] & CSF_FINALLY))
		rethrow = TRUE;
	}
    }

    /*
     * If a ":return" is pending, we need to resume it after closing the
     * try conditional; remember the return value.  If there was a finally
     * clause making an exception pending, we need to rethrow it.  Make it
     * the exception currently being thrown.
     */
    ਜੇ (!skip)
    {
	pending = cstack->cs_pending[idx];
	cstack->cs_pending[idx] = CSTP_NONE;
	ਜੇ (pending == CSTP_RETURN)
	    rettv = cstack->cs_rettv[idx];
	ਨਹੀਂਤਾਂ ਜੇ (pending & CSTP_THROW)
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
    (ਖਾਲੀ)cleanup_conditionals(cstack, CSF_TRY | CSF_SILENT, TRUE);

    ਜੇ (cstack->cs_idx >= 0 && (cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
	leave_ਖੰਡ(cstack);
    --cstack->cs_trylevel;

    ਜੇ (!skip)
    {
	report_resume_pending(pending,
		    (pending == CSTP_RETURN) ? rettv :
		    (pending & CSTP_THROW) ? (ਖਾਲੀ *)current_exception : ਨਿਰਾ);
	ਚੋਣ (pending)
	{
	    ਹਾਲਤ CSTP_NONE:
		ਰੋਕੋ;

	    // Reactivate a pending ":continue", ":break", ":return",
	    // ":finish" from the try block or a catch clause of this try
	    // conditional.  This is skipped, if there was an error in an
	    // (unskipped) conditional command or an interrupt afterwards
	    // or if the finally clause is present and executed a new error,
	    // interrupt, throw, ":continue", ":break", ":return", or
	    // ":finish".
	    ਹਾਲਤ CSTP_CONTINUE:
		ex_continue(eap);
		ਰੋਕੋ;
	    ਹਾਲਤ CSTP_BREAK:
		ex_break(eap);
		ਰੋਕੋ;
	    ਹਾਲਤ CSTP_RETURN:
		do_return(eap, FALSE, FALSE, rettv);
		ਰੋਕੋ;
	    ਹਾਲਤ CSTP_ਅੰਤ:
		do_ਅੰਤ(eap, FALSE);
		ਰੋਕੋ;

	    // When the finally clause was entered due to an error,
	    // interrupt or throw (as opposed to a ":continue", ":break",
	    // ":return", or ":finish"), restore the pending values of
	    // did_emsg, got_int, and did_throw.  This is skipped, if there
	    // was a new error, interrupt, throw, ":continue", ":break",
	    // ":return", or ":finish".  in the finally clause.
	    ਮੂਲ:
		ਜੇ (pending & CSTP_ERROR)
		    did_emsg = TRUE;
		ਜੇ (pending & CSTP_ਰੁਕਾਵਟ)
		    got_int = TRUE;
		ਜੇ (pending & CSTP_THROW)
		    rethrow = TRUE;
		ਰੋਕੋ;
	}
    }

    ਜੇ (rethrow)
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
    ਖਾਲੀ
enter_cleanup(cleanup_T *csp)
{
    ਪੂਰਨ		pending = CSTP_NONE;

    /*
     * Postpone did_emsg, got_int, did_throw.  The pending values will be
     * restored by leave_cleanup() except if there was an aborting error,
     * interrupt, or uncaught exception after this function ends.
     */
    ਜੇ (did_emsg || got_int || did_throw || need_rethrow)
    {
	csp->pending = (did_emsg     ? CSTP_ERROR     : 0)
		     | (got_int      ? CSTP_ਰੁਕਾਵਟ : 0)
		     | (did_throw    ? CSTP_THROW     : 0)
		     | (need_rethrow ? CSTP_THROW     : 0);

	// If we are currently throwing an exception (did_throw), save it as
	// well.  On an error not yet converted to an exception, update
	// "force_abort" and reset "cause_abort" (as do_errthrow() would do).
	// This is needed for the do_cmdline() call that is going to be made
	// for autocommand execution.  We need not save *msg_list because
	// there is an extra instance for every call of do_cmdline(), anyway.
	ਜੇ (did_throw || need_rethrow)
	{
	    csp->exception = current_exception;
	    current_exception = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    csp->exception = ਨਿਰਾ;
	    ਜੇ (did_emsg)
	    {
		force_abort |= cause_abort;
		cause_abort = FALSE;
	    }
	}
	did_emsg = got_int = did_throw = need_rethrow = FALSE;

	// Report if required by the 'verbose' option or when debugging.
	report_make_pending(pending, csp->exception);
    }
    ਨਹੀਂਤਾਂ
    {
	csp->pending = CSTP_NONE;
	csp->exception = ਨਿਰਾ;
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
    ਖਾਲੀ
leave_cleanup(cleanup_T *csp)
{
    ਪੂਰਨ		pending = csp->pending;

    ਜੇ (pending == CSTP_NONE)	// nothing to do
	ਵਾਪਸ;

    // If there was an aborting error, an interrupt, or an uncaught exception
    // after the corresponding call to enter_cleanup(), discard what has been
    // made pending by it.  Report this to the user if required by the
    // 'verbose' option or when debugging.
    ਜੇ (aborting() || need_rethrow)
    {
	ਜੇ (pending & CSTP_THROW)
	    // Cancel the pending exception (includes report).
	    discard_exception(csp->exception, FALSE);
	ਨਹੀਂਤਾਂ
	    report_discard_pending(pending, ਨਿਰਾ);

	// If an error was about to be converted to an exception when
	// enter_cleanup() was called, free the message list.
	ਜੇ (ਸੁਨੇਹਾ_list != ਨਿਰਾ)
	    free_global_msglist();
    }

    /*
     * If there was no new error, interrupt, or throw between the calls
     * to enter_cleanup() and leave_cleanup(), restore the pending
     * error/interrupt/exception state.
     */
    ਨਹੀਂਤਾਂ
    {
	/*
	 * If there was an exception being thrown when enter_cleanup() was
	 * called, we need to rethrow it.  Make it the exception currently
	 * being thrown.
	 */
	ਜੇ (pending & CSTP_THROW)
	    current_exception = csp->exception;

	/*
	 * If an error was about to be converted to an exception when
	 * enter_cleanup() was called, let "cause_abort" take the part of
	 * "force_abort" (as done by cause_errthrow()).
	 */
	ਨਹੀਂਤਾਂ ਜੇ (pending & CSTP_ERROR)
	{
	    cause_abort = force_abort;
	    force_abort = FALSE;
	}

	/*
	 * Restore the pending values of did_emsg, got_int, and did_throw.
	 */
	ਜੇ (pending & CSTP_ERROR)
	    did_emsg = TRUE;
	ਜੇ (pending & CSTP_ਰੁਕਾਵਟ)
	    got_int = TRUE;
	ਜੇ (pending & CSTP_THROW)
	    need_rethrow = TRUE;    // did_throw will be set by do_one_cmd()

	// Report if required by the 'verbose' option or when debugging.
	report_resume_pending(pending,
		   (pending & CSTP_THROW) ? (ਖਾਲੀ *)current_exception : ਨਿਰਾ);
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
    ਪੂਰਨ
cleanup_conditionals(
    cstack_T   *cstack,
    ਪੂਰਨ		searched_cond,
    ਪੂਰਨ		inclusive)
{
    ਪੂਰਨ		idx;
    ਪੂਰਨ		stop = FALSE;

    ਲਈ (idx = cstack->cs_idx; idx >= 0; --idx)
    {
	ਜੇ (cstack->cs_flags[idx] & CSF_TRY)
	{
	    /*
	     * Discard anything pending in a finally clause and continue the
	     * search.  There may also be a pending ":continue", ":break",
	     * ":return", or ":finish" before the finally clause.  We must not
	     * discard it, unless an error or interrupt occurred afterwards.
	     */
	    ਜੇ (did_emsg || got_int || (cstack->cs_flags[idx] & CSF_FINALLY))
	    {
		ਚੋਣ (cstack->cs_pending[idx])
		{
		    ਹਾਲਤ CSTP_NONE:
			ਰੋਕੋ;

		    ਹਾਲਤ CSTP_CONTINUE:
		    ਹਾਲਤ CSTP_BREAK:
		    ਹਾਲਤ CSTP_ਅੰਤ:
			report_discard_pending(cstack->cs_pending[idx], ਨਿਰਾ);
			cstack->cs_pending[idx] = CSTP_NONE;
			ਰੋਕੋ;

		    ਹਾਲਤ CSTP_RETURN:
			report_discard_pending(CSTP_RETURN,
						      cstack->cs_rettv[idx]);
			discard_pending_return(cstack->cs_rettv[idx]);
			cstack->cs_pending[idx] = CSTP_NONE;
			ਰੋਕੋ;

		    ਮੂਲ:
			ਜੇ (cstack->cs_flags[idx] & CSF_FINALLY)
			{
			    ਜੇ ((cstack->cs_pending[idx] & CSTP_THROW)
				    && cstack->cs_exception[idx] != ਨਿਰਾ)
			    {
				// Cancel the pending exception.  This is in the
				// finally clause, so that the stack of the
				// caught exceptions is not involved.
				discard_exception(
					(except_T *)cstack->cs_exception[idx],
					FALSE);
			    }
			    ਨਹੀਂਤਾਂ
				report_discard_pending(cstack->cs_pending[idx],
					ਨਿਰਾ);
			    cstack->cs_pending[idx] = CSTP_NONE;
			}
			ਰੋਕੋ;
		}
	    }

	    /*
	     * Stop at a try conditional not in its finally clause.  If this try
	     * conditional is in an active catch clause, finish the caught
	     * exception.
	     */
	    ਜੇ (!(cstack->cs_flags[idx] & CSF_FINALLY))
	    {
		ਜੇ ((cstack->cs_flags[idx] & CSF_ਸਰਗਰਮ)
			&& (cstack->cs_flags[idx] & CSF_CAUGHT)
			&& !(cstack->cs_flags[idx] & CSF_ਮੁਕੰਮਲ))
		{
		    ਅੰਤ_exception((except_T *)cstack->cs_exception[idx]);
		    cstack->cs_flags[idx] |= CSF_ਮੁਕੰਮਲ;
		}
		// Stop at this try conditional - except the try block never
		// got active (because of an inactive surrounding conditional
		// or when the ":try" appeared after an error or interrupt or
		// throw).
		ਜੇ (cstack->cs_flags[idx] & CSF_TRUE)
		{
		    ਜੇ (searched_cond == 0 && !inclusive)
			ਰੋਕੋ;
		    stop = TRUE;
		}
	    }
	}

	// Stop on the searched conditional type (even when the surrounding
	// conditional is not active or something has been made pending).
	// If "inclusive" is TRUE and "searched_cond" is CSF_TRY|CSF_SILENT,
	// check first whether "emsg_silent" needs to be restored.
	ਜੇ (cstack->cs_flags[idx] & searched_cond)
	{
	    ਜੇ (!inclusive)
		ਰੋਕੋ;
	    stop = TRUE;
	}
	cstack->cs_flags[idx] &= ~CSF_ਸਰਗਰਮ;
	ਜੇ (stop && searched_cond != (CSF_TRY | CSF_SILENT))
	    ਰੋਕੋ;

	/*
	 * When leaving a try conditional that reset "emsg_silent" on its
	 * entry after saving the original value, restore that value here and
	 * free the memory used to store it.
	 */
	ਜੇ ((cstack->cs_flags[idx] & CSF_TRY)
		&& (cstack->cs_flags[idx] & CSF_SILENT))
	{
	    eslist_T	*elem;

	    elem = cstack->cs_emsg_silent_list;
	    cstack->cs_emsg_silent_list = elem->ਅਗਲਾ;
	    emsg_silent = elem->saved_emsg_silent;
	    vim_free(elem);
	    cstack->cs_flags[idx] &= ~CSF_SILENT;
	}
	ਜੇ (stop)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ idx;
}

/*
 * Return an appropriate error message for a missing endwhile/endfor/endif.
 */
   ਸਥਿਰ ਅੱਖਰ *
get_ਸਿਰਾ_emsg(cstack_T *cstack)
{
    ਜੇ (cstack->cs_flags[cstack->cs_idx] & CSF_WHILE)
	ਵਾਪਸ _(e_missing_endwhile);
    ਜੇ (cstack->cs_flags[cstack->cs_idx] & CSF_FOR)
	ਵਾਪਸ _(e_missing_endfor);
    ਵਾਪਸ _(e_missing_endif);
}


/*
 * Rewind conditionals until index "idx" is reached.  "cond_type" and
 * "cond_level" specify a conditional type and the address of a level variable
 * which is to be decremented with each skipped conditional of the specified
 * type.
 * Also free "for info" structures where needed.
 */
    ਖਾਲੀ
rewind_conditionals(
    cstack_T   *cstack,
    ਪੂਰਨ		idx,
    ਪੂਰਨ		cond_type,
    ਪੂਰਨ		*cond_level)
{
    ਜਦੋਂ (cstack->cs_idx > idx)
    {
	ਜੇ (cstack->cs_flags[cstack->cs_idx] & cond_type)
	    --*cond_level;
	ਜੇ (cstack->cs_flags[cstack->cs_idx] & CSF_FOR)
	    free_for_info(cstack->cs_forinfo[cstack->cs_idx]);
	leave_ਖੰਡ(cstack);
    }
}

/*
 * ":endfunction" or ":enddef" when not after a ":function"
 */
    ਖਾਲੀ
ex_endfunction(exarg_T *eap)
{
    ਜੇ (eap->cmdidx == CMD_enddef)
	semsg(_(e_str_not_inside_function), ":enddef");
    ਨਹੀਂਤਾਂ
	semsg(_(e_str_not_inside_function), ":endfunction");
}

/*
 * Return TRUE if the string "p" looks like a ":while" or ":for" command.
 */
    ਪੂਰਨ
has_loop_cmd(char_u *p)
{
    ਪੂਰਨ		len;

    // skip modifiers, white space and ':'
    ਲਈ (;;)
    {
	ਜਦੋਂ (*p == ' ' || *p == '\t' || *p == ':')
	    ++p;
	len = modifier_len(p);
	ਜੇ (len == 0)
	    ਰੋਕੋ;
	p += len;
    }
    ਜੇ ((p[0] == 'w' && p[1] == 'h')
	    || (p[0] == 'f' && p[1] == 'o' && p[2] == 'r'))
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
