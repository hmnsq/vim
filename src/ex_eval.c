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

#अन्तर्भाव "vim.h"

#सचे defined(FEAT_EVAL)

स्थिर वर्ण	*get_end_emsg(cstack_T *cstack);

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
#सचे 0
// Expressions used for testing during the development phase.
# define THROW_ON_ERROR		(!eval_to_सङ्ख्या("$VIMNOERRTHROW"))
# define THROW_ON_INTERRUPT	(!eval_to_सङ्ख्या("$VIMNOINTTHROW"))
# define THROW_TEST
#अञ्ञथा
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
स्थिर पूर्णाङ्क cause_abort = FALSE;

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
    पूर्णाङ्क
aborting(शून्य)
{
    पटिनिवत्त (did_emsg && force_abort) || got_int || did_throw;
}

/*
 * The value of "force_abort" is temporarily reset by the first emsg() call
 * during an expression evaluation, and "cause_abort" is used instead.  It might
 * be necessary to restore "force_abort" even before the throw point for the
 * error message has been reached.  update_force_abort() should be called then.
 */
    शून्य
update_force_abort(शून्य)
{
    सचे (cause_abort)
	force_abort = TRUE;
}

/*
 * Return TRUE if a command with a subcommand resulting in "retcode" should
 * abort the script processing.  Can be used to suppress an autocommand after
 * execution of a failing subcommand as long as the error message has not been
 * displayed and actually caused the abortion.
 */
    पूर्णाङ्क
should_abort(पूर्णाङ्क retcode)
{
    पटिनिवत्त ((retcode == FAIL && trylevel != 0 && !emsg_silent) || aborting());
}

/*
 * Return TRUE if a function with the "abort" flag should not be considered
 * ended on an error.  This means that parsing commands is continued in order
 * to find finally clauses to be executed, and that some errors in skipped
 * commands are still reported.
 */
    पूर्णाङ्क
aborted_in_try(शून्य)
{
    // This function is only called after an error.  In this case, "force_abort"
    // determines whether searching for finally clauses is necessary.
    पटिनिवत्त force_abort;
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
    पूर्णाङ्क
cause_errthrow(
    char_u	*mesg,
    पूर्णाङ्क		severe,
    पूर्णाङ्क		*ignore)
{
    msglist_T	*elem;
    msglist_T	**plist;

    /*
     * Do nothing when displaying the interrupt message or reporting an
     * uncaught exception (which has already been discarded then) at the top
     * level.  Also when no exception can be thrown.  The message will be
     * displayed by emsg().
     */
    सचे (suppress_errthrow)
	पटिनिवत्त FALSE;

    /*
     * If emsg() has not been called previously, temporarily reset
     * "force_abort" until the throw point for error messages has been
     * reached.  This ensures that aborting() returns the same value for all
     * errors that appear in the same command.  This means particularly that
     * for parsing errors during expression evaluation emsg() will be called
     * multiply, even when the expression is evaluated from a finally clause
     * that was activated due to an aborting error, interrupt, or exception.
     */
    सचे (!did_emsg)
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
    सचे (((trylevel == 0 && !cause_abort) || emsg_silent) && !did_throw)
	पटिनिवत्त FALSE;

    /*
     * Ignore an interrupt message when inside a try conditional or when an
     * exception is being thrown or when an error in a try conditional or
     * throw has been detected previously.  This is important in order that an
     * interrupt exception is catchable by the innermost try conditional and
     * not replaced by an interrupt message error exception.
     */
    सचे (mesg == (char_u *)_(e_interrupted))
    {
	*ignore = TRUE;
	पटिनिवत्त TRUE;
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
    सचे (did_throw)
    {
	// When discarding an interrupt exception, reset got_int to prevent the
	// same interrupt being converted to an exception again and discarding
	// the error exception we are about to throw here.
	सचे (current_exception->type == ET_INTERRUPT)
	    got_int = FALSE;
	discard_current_exception();
    }

#ifdef THROW_TEST
    सचे (!THROW_ON_ERROR)
    {
	/*
	 * Print error message immediately without searching for a matching
	 * catch clause; just finally clauses are executed before the script
	 * is terminated.
	 */
	पटिनिवत्त FALSE;
    }
    अञ्ञथा
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
	सचे (सन्देशः_list != रिक्त)
	{
	    plist = सन्देशः_list;
	    याव (*plist != रिक्त)
		plist = &(*plist)->next;

	    elem = ALLOC_CLEAR_ONE(msglist_T);
	    सचे (elem == रिक्त)
	    {
		suppress_errthrow = TRUE;
		emsg(_(e_out_of_memory));
	    }
	    अञ्ञथा
	    {
		elem->सन्देशः = (वर्ण *)vim_strsave(mesg);
		सचे (elem->सन्देशः == रिक्त)
		{
		    vim_free(elem);
		    suppress_errthrow = TRUE;
		    emsg(_(e_out_of_memory));
		}
		अञ्ञथा
		{
		    elem->next = रिक्त;
		    elem->throw_सन्देशः = रिक्त;
		    *plist = elem;
		    सचे (plist == सन्देशः_list || severe)
		    {
			वर्ण	    *tmsg;

			// Skip the extra "Vim " prefix for message "E457".
			tmsg = elem->सन्देशः;
			सचे (STRNCMP(tmsg, "Vim E", 5) == 0
				&& VIM_ISDIGIT(tmsg[5])
				&& VIM_ISDIGIT(tmsg[6])
				&& VIM_ISDIGIT(tmsg[7])
				&& tmsg[8] == ':'
				&& tmsg[9] == ' ')
			    (*सन्देशः_list)->throw_सन्देशः = &tmsg[4];
			अञ्ञथा
			    (*सन्देशः_list)->throw_सन्देशः = tmsg;
		    }

		    // Get the source name and lnum now, it may change before
		    // reaching do_errthrow().
		    elem->sfile = estack_sfile(ESTACK_NONE);
		    elem->slnum = SOURCING_LNUM;
		    elem->सन्देशः_compiling = estack_compiling;
		}
	    }
	}
	पटिनिवत्त TRUE;
    }
}

/*
 * Free a "msg_list" and the messages it contains.
 */
    स्थिर शून्य
free_msglist(msglist_T *l)
{
    msglist_T  *messages, *next;

    messages = l;
    याव (messages != रिक्त)
    {
	next = messages->next;
	vim_free(messages->सन्देशः);
	vim_free(messages->sfile);
	vim_free(messages);
	messages = next;
    }
}

/*
 * Free global "*msg_list" and the messages it contains, then set "*msg_list"
 * to NULL.
 */
    शून्य
free_global_msglist(शून्य)
{
    free_msglist(*सन्देशः_list);
    *सन्देशः_list = रिक्त;
}

/*
 * Throw the message specified in the call to cause_errthrow() above as an
 * error exception.  If cstack is NULL, postpone the throw until do_cmdline()
 * has returned (see do_one_cmd()).
 */
    शून्य
do_errthrow(cstack_T *cstack, char_u *cmdname)
{
    /*
     * Ensure that all commands in nested function calls and sourced files
     * are aborted immediately.
     */
    सचे (cause_abort)
    {
	cause_abort = FALSE;
	force_abort = TRUE;
    }

    // If no exception is to be thrown or the conversion should be done after
    // returning to a previous invocation of do_one_cmd(), do nothing.
    सचे (सन्देशः_list == रिक्त || *सन्देशः_list == रिक्त)
	पटिनिवत्त;

    सचे (throw_exception(*सन्देशः_list, ET_ERROR, cmdname) == FAIL)
	free_msglist(*सन्देशः_list);
    अञ्ञथा
    {
	सचे (cstack != रिक्त)
	    do_throw(cstack);
	अञ्ञथा
	    need_rethrow = TRUE;
    }
    *सन्देशः_list = रिक्त;
}

/*
 * do_intthrow(): Replace the current exception by an interrupt or interrupt
 * exception if appropriate.  Return TRUE if the current exception is discarded,
 * FALSE otherwise.
 */
    पूर्णाङ्क
do_intthrow(cstack_T *cstack)
{
    /*
     * If no interrupt occurred or no try conditional is active and no exception
     * is being thrown, do nothing (for compatibility of non-EH scripts).
     */
    सचे (!got_int || (trylevel == 0 && !did_throw))
	पटिनिवत्त FALSE;

#ifdef THROW_TEST	// avoid warning for condition always true
    सचे (!THROW_ON_INTERRUPT)
    {
	/*
	 * The interrupt aborts everything except for executing finally clauses.
	 * Discard any user or error or interrupt exception currently being
	 * thrown.
	 */
	सचे (did_throw)
	    discard_current_exception();
    }
    अञ्ञथा
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
	सचे (did_throw)
	{
	    सचे (current_exception->type == ET_INTERRUPT)
		पटिनिवत्त FALSE;

	    // An interrupt exception replaces any user or error exception.
	    discard_current_exception();
	}
	सचे (throw_exception("Vim:Interrupt", ET_INTERRUPT, रिक्त) != FAIL)
	    do_throw(cstack);
    }

    पटिनिवत्त TRUE;
}

/*
 * Get an exception message that is to be stored in current_exception->value.
 */
    वर्ण *
get_exception_string(
    शून्य	*मानम्,
    except_type_T type,
    char_u	*cmdname,
    पूर्णाङ्क		*should_free)
{
    वर्ण	*ret;
    वर्ण	*mesg;
    पूर्णाङ्क		cmdlen;
    वर्ण	*p, *val;

    सचे (type == ET_ERROR)
    {
	*should_free = TRUE;
	mesg = ((msglist_T *)मानम्)->throw_सन्देशः;
	सचे (cmdname != रिक्त && *cmdname != NUL)
	{
	    cmdlen = (पूर्णाङ्क)STRLEN(cmdname);
	    ret = (वर्ण *)vim_strnsave((char_u *)"Vim(",
						4 + cmdlen + 2 + STRLEN(mesg));
	    सचे (ret == रिक्त)
		पटिनिवत्त ret;
	    STRCPY(&ret[4], cmdname);
	    STRCPY(&ret[4 + cmdlen], "):");
	    val = ret + 4 + cmdlen + 2;
	}
	अञ्ञथा
	{
	    ret = (वर्ण *)vim_strnsave((char_u *)"Vim:", 4 + STRLEN(mesg));
	    सचे (ret == रिक्त)
		पटिनिवत्त ret;
	    val = ret + 4;
	}

	// msg_add_fname may have been used to prefix the message with a file
	// name in quotes.  In the exception value, put the file name in
	// parentheses and move it to the end.
	अत्थाय (p = mesg; ; p++)
	{
	    सचे (*p == NUL
		    || (*p == 'E'
			&& VIM_ISDIGIT(p[1])
			&& (p[2] == ':'
			    || (VIM_ISDIGIT(p[2])
				&& (p[3] == ':'
				    || (VIM_ISDIGIT(p[3])
					&& p[4] == ':'))))))
	    {
		सचे (*p == NUL || p == mesg)
		    STRCAT(val, mesg);  // 'E123' missing or at beginning
		अञ्ञथा
		{
		    // '"filename" E123: message text'
		    सचे (mesg[0] != '"' || p-2 < &mesg[1] ||
			    p[-2] != '"' || p[-1] != ' ')
			// "E123:" is part of the file name.
			अनुवर्त;

		    STRCAT(val, p);
		    p[-2] = NUL;
		    sprintf((वर्ण *)(val + STRLEN(p)), " (%s)", &mesg[1]);
		    p[-2] = '"';
		}
		भङ्ग;
	    }
	}
    }
    अञ्ञथा
    {
	*should_free = FALSE;
	ret = मानम्;
    }

    पटिनिवत्त ret;
}


/*
 * Throw a new exception.  Return FAIL when out of memory or it was tried to
 * throw an illegal user exception.  "value" is the exception string for a
 * user or interrupt exception, or points to a message list in case of an
 * error exception.
 */
    पूर्णाङ्क
throw_exception(शून्य *मानम्, except_type_T type, char_u *cmdname)
{
    except_T	*excp;
    पूर्णाङ्क		should_free;

    /*
     * Disallow faking Interrupt or error exceptions as user exceptions.  They
     * would be treated differently from real interrupt or error exceptions
     * when no active try block is found, see do_cmdline().
     */
    सचे (type == ET_USER)
    {
	सचे (STRNCMP((char_u *)मानम्, "Vim", 3) == 0
		&& (((char_u *)मानम्)[3] == NUL || ((char_u *)मानम्)[3] == ':'
		    || ((char_u *)मानम्)[3] == '('))
	{
	    emsg(_(e_cannot_throw_exceptions_with_vim_prefix));
	    गच्छ fail;
	}
    }

    excp = ALLOC_ONE(except_T);
    सचे (excp == रिक्त)
	गच्छ nomem;

    सचे (type == ET_ERROR)
	// Store the original message and prefix the exception value with
	// "Vim:" or, if a command name is given, "Vim(cmdname):".
	excp->messages = (msglist_T *)मानम्;

    excp->मानम् = get_exception_string(मानम्, type, cmdname, &should_free);
    सचे (excp->मानम् == रिक्त && should_free)
	गच्छ nomem;

    excp->type = type;
    सचे (type == ET_ERROR && ((msglist_T *)मानम्)->sfile != रिक्त)
    {
	msglist_T *entry = (msglist_T *)मानम्;

	excp->throw_name = entry->sfile;
	entry->sfile = रिक्त;
	excp->throw_lnum = entry->slnum;
    }
    अञ्ञथा
    {
	excp->throw_name = estack_sfile(ESTACK_NONE);
	सचे (excp->throw_name == रिक्त)
	    excp->throw_name = vim_strsave((char_u *)"");
	सचे (excp->throw_name == रिक्त)
	{
	    सचे (should_free)
		vim_free(excp->मानम्);
	    गच्छ nomem;
	}
	excp->throw_lnum = SOURCING_LNUM;
    }

    excp->stacktrace = stacktrace_create();
    सचे (excp->stacktrace != रिक्त)
	excp->stacktrace->lv_refcount = 1;

    सचे (p_verbose >= 13 || debug_break_level > 0)
    {
	पूर्णाङ्क	save_सन्देशः_silent = सन्देशः_silent;

	सचे (debug_break_level > 0)
	    सन्देशः_silent = FALSE;		// display messages
	अञ्ञथा
	    verbose_enter();
	++no_wait_return;
	सचे (debug_break_level > 0 || *p_vfile == NUL)
	    सन्देशः_scroll = TRUE;	    // always scroll up, don't overwrite

	smsg(_("Exception thrown: %s"), excp->मानम्);
	सन्देशः_puts("\n");   // don't overwrite this either

	सचे (debug_break_level > 0 || *p_vfile == NUL)
	    cmdline_row = सन्देशः_row;
	--no_wait_return;
	सचे (debug_break_level > 0)
	    सन्देशः_silent = save_सन्देशः_silent;
	अञ्ञथा
	    verbose_leave();
    }

    current_exception = excp;
    पटिनिवत्त OK;

nomem:
    vim_free(excp);
    suppress_errthrow = TRUE;
    emsg(_(e_out_of_memory));
fail:
    current_exception = रिक्त;
    पटिनिवत्त FAIL;
}

/*
 * Discard an exception.  "was_finished" is set when the exception has been
 * caught and the catch clause has been ended normally.
 */
    स्थिर शून्य
discard_exception(except_T *excp, पूर्णाङ्क was_finished)
{
    char_u		*saved_IObuff;

    सचे (current_exception == excp)
	current_exception = रिक्त;
    सचे (excp == रिक्त)
    {
	internal_error("discard_exception()");
	पटिनिवत्त;
    }

    सचे (p_verbose >= 13 || debug_break_level > 0)
    {
	पूर्णाङ्क	save_सन्देशः_silent = सन्देशः_silent;

	saved_IObuff = vim_strsave(IObuff);
	सचे (debug_break_level > 0)
	    सन्देशः_silent = FALSE;		// display messages
	अञ्ञथा
	    verbose_enter();
	++no_wait_return;
	सचे (debug_break_level > 0 || *p_vfile == NUL)
	    सन्देशः_scroll = TRUE;	    // always scroll up, don't overwrite
	smsg(was_finished
		    ? _("Exception finished: %s")
		    : _("Exception discarded: %s"),
		excp->मानम्);
	सन्देशः_puts("\n");   // don't overwrite this either
	सचे (debug_break_level > 0 || *p_vfile == NUL)
	    cmdline_row = सन्देशः_row;
	--no_wait_return;
	सचे (debug_break_level > 0)
	    सन्देशः_silent = save_सन्देशः_silent;
	अञ्ञथा
	    verbose_leave();
	STRCPY(IObuff, saved_IObuff);
	vim_free(saved_IObuff);
    }
    सचे (excp->type != ET_INTERRUPT)
	vim_free(excp->मानम्);
    सचे (excp->type == ET_ERROR)
	free_msglist(excp->messages);
    vim_free(excp->throw_name);
    list_unref(excp->stacktrace);
    vim_free(excp);
}

/*
 * Discard the exception currently being thrown.
 */
    शून्य
discard_current_exception(शून्य)
{
    सचे (current_exception != रिक्त)
	discard_exception(current_exception, FALSE);
    did_throw = FALSE;
    need_rethrow = FALSE;
}

/*
 * Put an exception on the caught stack.
 */
    शून्य
catch_exception(except_T *excp)
{
    excp->caught = caught_राशिः;
    caught_राशिः = excp;
    set_vim_var_string(VV_EXCEPTION, (char_u *)excp->मानम्, -1);
    set_vim_var_list(VV_STACKTRACE, excp->stacktrace);
    सचे (*excp->throw_name != NUL)
    {
	size_t	IObufflen;

	सचे (excp->throw_lnum != 0)
	    IObufflen = vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE, _("%s, line %ld"),
				    excp->throw_name, (long)excp->throw_lnum);
	अञ्ञथा
	    IObufflen = vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE, "%s", excp->throw_name);
	set_vim_var_string(VV_THROWPOINT, IObuff, (पूर्णाङ्क)IObufflen);
    }
    अञ्ञथा
	// throw_name not set on an exception from a command that was typed.
	set_vim_var_string(VV_THROWPOINT, रिक्त, -1);

    सचे (p_verbose >= 13 || debug_break_level > 0)
    {
	पूर्णाङ्क	save_सन्देशः_silent = सन्देशः_silent;

	सचे (debug_break_level > 0)
	    सन्देशः_silent = FALSE;		// display messages
	अञ्ञथा
	    verbose_enter();
	++no_wait_return;
	सचे (debug_break_level > 0 || *p_vfile == NUL)
	    सन्देशः_scroll = TRUE;	    // always scroll up, don't overwrite

	smsg(_("Exception caught: %s"), excp->मानम्);
	सन्देशः_puts("\n");   // don't overwrite this either

	सचे (debug_break_level > 0 || *p_vfile == NUL)
	    cmdline_row = सन्देशः_row;
	--no_wait_return;
	सचे (debug_break_level > 0)
	    सन्देशः_silent = save_सन्देशः_silent;
	अञ्ञथा
	    verbose_leave();
    }
}

/*
 * Remove an exception from the caught stack.
 */
    शून्य
finish_exception(except_T *excp)
{
    सचे (excp != caught_राशिः)
	internal_error("finish_exception()");
    caught_राशिः = caught_राशिः->caught;
    सचे (caught_राशिः != रिक्त)
    {
	set_vim_var_string(VV_EXCEPTION, (char_u *)caught_राशिः->मानम्, -1);
	set_vim_var_list(VV_STACKTRACE, caught_राशिः->stacktrace);
	सचे (*caught_राशिः->throw_name != NUL)
	{
	    size_t  IObufflen;

	    सचे (caught_राशिः->throw_lnum != 0)
		IObufflen = vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE,
			_("%s, line %ld"), caught_राशिः->throw_name,
			(long)caught_राशिः->throw_lnum);
	    अञ्ञथा
		IObufflen = vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE, "%s",
						    caught_राशिः->throw_name);
	    set_vim_var_string(VV_THROWPOINT, IObuff, (पूर्णाङ्क)IObufflen);
	}
	अञ्ञथा
	    // throw_name not set on an exception from a command that was
	    // typed.
	    set_vim_var_string(VV_THROWPOINT, रिक्त, -1);
    }
    अञ्ञथा
    {
	set_vim_var_string(VV_EXCEPTION, रिक्त, -1);
	set_vim_var_string(VV_THROWPOINT, रिक्त, -1);
	set_vim_var_list(VV_STACKTRACE, रिक्त);
    }

    // Discard the exception, but use the finish message for 'verbose'.
    discard_exception(excp, TRUE);
}

/*
 * Save the current exception state in "estate"
 */
    शून्य
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
    शून्य
exception_state_restore(exception_state_T *estate)
{
    // Handle any outstanding exceptions before restoring the state
    सचे (did_throw)
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
    शून्य
exception_state_clear(शून्य)
{
    current_exception = रिक्त;
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
    स्थिर शून्य
report_pending(पूर्णाङ्क action, पूर्णाङ्क pending, शून्य *मानम्)
{
    वर्ण	*mesg;
    वर्ण	*s;
    पूर्णाङ्क		save_सन्देशः_silent;


    विकल्प (action)
    {
	पक्ष RP_MAKE:
	    mesg = _("%s made pending");
	    भङ्ग;
	पक्ष RP_RESUME:
	    mesg = _("%s resumed");
	    भङ्ग;
	// case RP_DISCARD:
	स्वभाव:
	    mesg = _("%s discarded");
	    भङ्ग;
    }

    विकल्प (pending)
    {
	पक्ष CSTP_NONE:
	    पटिनिवत्त;

	पक्ष CSTP_CONTINUE:
	    s = ":continue";
	    भङ्ग;
	पक्ष CSTP_BREAK:
	    s = ":break";
	    भङ्ग;
	पक्ष CSTP_FINISH:
	    s = ":finish";
	    भङ्ग;
	पक्ष CSTP_RETURN:
	    // ":return" command producing value, allocated
	    s = (वर्ण *)get_return_cmd(मानम्);
	    भङ्ग;

	स्वभाव:
	    सचे (pending & CSTP_THROW)
	    {
		vim_snprintf((वर्ण *)IObuff, IOSIZE, mesg, _("Exception"));
		mesg = (वर्ण *)vim_strnsave(IObuff, STRLEN(IObuff) + 4);
		STRCAT(mesg, ": %s");
		s = (वर्ण *)((except_T *)मानम्)->मानम्;
	    }
	    अञ्ञथा सचे ((pending & CSTP_ERROR) && (pending & CSTP_INTERRUPT))
		s = _("Error and interrupt");
	    अञ्ञथा सचे (pending & CSTP_ERROR)
		s = _("Error");
	    अञ्ञथा // if (pending & CSTP_INTERRUPT)
		s = _("Interrupt");
    }

    save_सन्देशः_silent = सन्देशः_silent;
    सचे (debug_break_level > 0)
	सन्देशः_silent = FALSE;	// display messages
    ++no_wait_return;
    सन्देशः_scroll = TRUE;		// always scroll up, don't overwrite
    smsg(mesg, s);
    सन्देशः_puts("\n");   // don't overwrite this either
    cmdline_row = सन्देशः_row;
    --no_wait_return;
    सचे (debug_break_level > 0)
	सन्देशः_silent = save_सन्देशः_silent;

    सचे (pending == CSTP_RETURN)
	vim_free(s);
    अञ्ञथा सचे (pending & CSTP_THROW)
	vim_free(mesg);
}

/*
 * If something is made pending in a finally clause, report it if required by
 * the 'verbose' option or when debugging.
 */
    शून्य
report_make_pending(पूर्णाङ्क pending, शून्य *मानम्)
{
    सचे (p_verbose >= 14 || debug_break_level > 0)
    {
	सचे (debug_break_level <= 0)
	    verbose_enter();
	report_pending(RP_MAKE, pending, मानम्);
	सचे (debug_break_level <= 0)
	    verbose_leave();
    }
}

/*
 * If something pending in a finally clause is resumed at the ":endtry", report
 * it if required by the 'verbose' option or when debugging.
 */
    स्थिर शून्य
report_resume_pending(पूर्णाङ्क pending, शून्य *मानम्)
{
    सचे (p_verbose >= 14 || debug_break_level > 0)
    {
	सचे (debug_break_level <= 0)
	    verbose_enter();
	report_pending(RP_RESUME, pending, मानम्);
	सचे (debug_break_level <= 0)
	    verbose_leave();
    }
}

/*
 * If something pending in a finally clause is discarded, report it if required
 * by the 'verbose' option or when debugging.
 */
    स्थिर शून्य
report_discard_pending(पूर्णाङ्क pending, शून्य *मानम्)
{
    सचे (p_verbose >= 14 || debug_break_level > 0)
    {
	सचे (debug_break_level <= 0)
	    verbose_enter();
	report_pending(RP_DISCARD, pending, मानम्);
	सचे (debug_break_level <= 0)
	    verbose_leave();
    }
}

/*
 * Return TRUE if "arg" is only a variable, register, environment variable,
 * option name or string.
 */
    पूर्णाङ्क
cmd_is_name_only(char_u *arg)
{
    char_u  *p = arg;
    char_u  *alias = रिक्त;
    पूर्णाङ्क	    name_only = FALSE;

    सचे (*p == '@')
    {
	++p;
	सचे (*p != NUL)
	    ++p;
    }
    अञ्ञथा सचे (*p == '\'' || *p == '"')
    {
	पूर्णाङ्क	    r;

	सचे (*p == '"')
	    r = eval_string(&p, रिक्त, FALSE, FALSE);
	अञ्ञथा
	    r = eval_lit_string(&p, रिक्त, FALSE, FALSE);
	सचे (r == FAIL)
	    पटिनिवत्त FALSE;
    }
    अञ्ञथा
    {
	सचे (*p == '&')
	{
	    ++p;
	    सचे (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
		p += 2;
	}
	अञ्ञथा सचे (*p == '$')
	    ++p;
	(शून्य)get_name_len(&p, &alias, FALSE, FALSE);
    }
    name_only = ends_excmd2(arg, skipwhite(p));
    vim_free(alias);
    पटिनिवत्त name_only;
}

/*
 * ":eval".
 */
    शून्य
ex_eval(exarg_T *eap)
{
    typval_T	tv;
    evalarg_T	evalarg;
    पूर्णाङ्क		name_only = FALSE;
    long	lnum = SOURCING_LNUM;

    सचे (in_vim9script())
	name_only = cmd_is_name_only(eap->arg);

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    सचे (eval0(eap->arg, &tv, eap, &evalarg) == OK)
    {
	clear_tv(&tv);
	सचे (in_vim9script() && name_only
		&& (evalarg.eval_tofree == रिक्त
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
    स्थिर शून्य
enter_block(cstack_T *cstack)
{
    ++cstack->cs_idx;
    सचे (in_vim9script() && current_sctx.sc_sid > 0)
    {
	scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	cstack->cs_script_var_len[cstack->cs_idx] = si->sn_var_vals.ga_len;
	cstack->cs_block_id[cstack->cs_idx] = ++si->sn_last_block_id;
	si->sn_current_block_id = si->sn_last_block_id;
    }
    अञ्ञथा
    {
	// Just in case in_vim9script() does not return the same value when the
	// block ends.
	cstack->cs_script_var_len[cstack->cs_idx] = 0;
	cstack->cs_block_id[cstack->cs_idx] = 0;
    }
}

    स्थिर शून्य
leave_block(cstack_T *cstack)
{
    सचे (in_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	scriptitem_T	*si = SCRIPT_ITEM(current_sctx.sc_sid);
	पूर्णाङ्क		i;
	पूर्णाङ्क		func_defined =
			       cstack->cs_flags[cstack->cs_idx] & CSF_FUNC_DEF;

	अत्थाय (i = cstack->cs_script_var_len[cstack->cs_idx];
					       i < si->sn_var_vals.ga_len; ++i)
	{
	    svar_T	*sv = ((svar_T *)si->sn_var_vals.ga_data) + i;

	    // sv_name is set to NULL if it was already removed.  This happens
	    // when it was defined in an inner block and no functions were
	    // defined there.
	    सचे (sv->sv_name != रिक्त)
		// Remove a variable declared inside the block, if it still
		// exists, from sn_vars and move the value into sn_all_vars
		// if "func_defined" is non-zero.
		hide_script_var(si, i, func_defined);
	}

	सचे (cstack->cs_idx == 0)
	    si->sn_current_block_id = 0;
	अञ्ञथा
	    si->sn_current_block_id = cstack->cs_block_id[cstack->cs_idx - 1];
    }
    --cstack->cs_idx;
}

/*
 * ":if".
 */
    शून्य
ex_if(exarg_T *eap)
{
    पूर्णाङ्क		error;
    पूर्णाङ्क		skip;
    पूर्णाङ्क		result;
    cstack_T	*cstack = eap->cstack;

    सचे (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_if_nesting_too_deep);
    अञ्ञथा
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

	सचे (!skip && !error)
	{
	    सचे (result)
		cstack->cs_flags[cstack->cs_idx] = CSF_ACTIVE | CSF_TRUE;
	}
	अञ्ञथा
	    // set TRUE, so this conditional will never get active
	    cstack->cs_flags[cstack->cs_idx] = CSF_TRUE;
    }
}

/*
 * ":endif".
 */
    शून्य
ex_endif(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;

    सचे (cmdmod_error(FALSE))
	पटिनिवत्त;
    did_endif = TRUE;
    सचे (cstack->cs_idx < 0
	    || (cstack->cs_flags[cstack->cs_idx]
				& (CSF_WHILE | CSF_FOR | CSF_TRY | CSF_BLOCK)))
	eap->errmsg = _(e_endif_without_if);
    अञ्ञथा
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
	सचे (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRUE)
						    && dbg_check_skipped(eap))
	    (शून्य)do_intthrow(cstack);

	leave_block(cstack);
    }
}

/*
 * ":else" and ":elseif".
 */
    शून्य
ex_else(exarg_T *eap)
{
    पूर्णाङ्क		error;
    पूर्णाङ्क		skip;
    पूर्णाङ्क		result;
    cstack_T	*cstack = eap->cstack;

    /*
     * Don't do something after an error, interrupt, or throw, or when there is
     * a surrounding conditional and it was not active.
     */
    skip = did_emsg || got_int || did_throw || (cstack->cs_idx > 0
	    && !(cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE));

    सचे (cstack->cs_idx < 0
	    || (cstack->cs_flags[cstack->cs_idx]
				& (CSF_WHILE | CSF_FOR | CSF_TRY | CSF_BLOCK)))
    {
	सचे (eap->cmdidx == CMD_else)
	{
	    eap->errmsg = _(e_else_without_if);
	    पटिनिवत्त;
	}
	eap->errmsg = _(e_elseif_without_if);
	skip = TRUE;
    }
    अञ्ञथा सचे (cstack->cs_flags[cstack->cs_idx] & CSF_ELSE)
    {
	सचे (eap->cmdidx == CMD_else)
	{
	    eap->errmsg = _(e_multiple_else);
	    पटिनिवत्त;
	}
	eap->errmsg = _(e_elseif_after_else);
	skip = TRUE;
    }

    सचे (cstack->cs_idx >= 0)
    {
	// Variables declared in the previous block can no longer be
	// used.  Needs to be done before setting "cs_flags".
	leave_block(cstack);
	enter_block(cstack);
    }

    // if skipping or the ":if" was TRUE, reset ACTIVE, otherwise set it
    सचे (skip || cstack->cs_flags[cstack->cs_idx] & CSF_TRUE)
    {
	सचे (eap->errmsg == रिक्त)
	    cstack->cs_flags[cstack->cs_idx] = CSF_TRUE;
	skip = TRUE;	// don't evaluate an ":elseif"
    }
    अञ्ञथा
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
    सचे (!skip && dbg_check_skipped(eap) && got_int)
    {
	(शून्य)do_intthrow(cstack);
	skip = TRUE;
    }

    सचे (eap->cmdidx == CMD_elseif)
    {
	// When skipping we ignore most errors, but a missing expression is
	// wrong, perhaps it should have been "else".
	// A double quote here is the start of a string, not a comment.
	सचे (skip && *eap->arg != '"' && ends_excmd(*eap->arg))
	    semsg(_(e_invalid_expression_str), eap->arg);
	अञ्ञथा
	    result = eval_to_bool(eap->arg, &error, eap, skip, FALSE);

	// When throwing error exceptions, we want to throw always the first
	// of several errors in a row.  This is what actually happens when
	// a conditional error was detected above and there is another failure
	// when parsing the expression.  Since the skip flag is set in this
	// case, the parsing error will be ignored by emsg().
	सचे (!skip && !error)
	{
	    सचे (result)
		cstack->cs_flags[cstack->cs_idx] = CSF_ACTIVE | CSF_TRUE;
	    अञ्ञथा
		cstack->cs_flags[cstack->cs_idx] = 0;
	}
	अञ्ञथा सचे (eap->errmsg == रिक्त)
	    // set TRUE, so this conditional will never get active
	    cstack->cs_flags[cstack->cs_idx] = CSF_TRUE;
    }
    अञ्ञथा
	cstack->cs_flags[cstack->cs_idx] |= CSF_ELSE;
}

/*
 * Handle ":while" and ":for".
 */
    शून्य
ex_while(exarg_T *eap)
{
    पूर्णाङ्क		error;
    पूर्णाङ्क		skip;
    पूर्णाङ्क		result;
    cstack_T	*cstack = eap->cstack;
    पूर्णाङ्क		prev_cs_flags = 0;

    सचे (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_while_for_nesting_too_deep);
    अञ्ञथा
    {
	/*
	 * The loop flag is set when we have jumped back from the matching
	 * ":endwhile" or ":endfor".  When not set, need to initialise this
	 * cstack entry.
	 */
	सचे ((cstack->cs_lflags & CSL_HAD_LOOP) == 0)
	{
	    enter_block(cstack);
	    ++cstack->cs_looplevel;
	    cstack->cs_पङ्क्तिः[cstack->cs_idx] = -1;
	}
	अञ्ञथा
	{
	    सचे (in_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid))
	    {
		scriptitem_T	*si = SCRIPT_ITEM(current_sctx.sc_sid);
		पूर्णाङ्क		i;
		पूर्णाङ्क		first;
		पूर्णाङ्क		func_defined = cstack->cs_flags[cstack->cs_idx]
								& CSF_FUNC_DEF;

		// Any variables defined in the previous round are no longer
		// visible.  Keep the first one for ":for", it is the loop
		// variable that we reuse every time around.
		// Do this backwards, so that vars defined in a later round are
		// found first.
		first = cstack->cs_script_var_len[cstack->cs_idx];
		सचे (eap->cmdidx == CMD_for)
		{
		    forinfo_T	*fi = cstack->cs_forinfo[cstack->cs_idx];

		    first += fi == रिक्त || fi->fi_varcount == 0
							 ? 1 : fi->fi_varcount;
		}
		अत्थाय (i = si->sn_var_vals.ga_len - 1; i >= first; --i)
		{
		    svar_T	*sv = ((svar_T *)si->sn_var_vals.ga_data) + i;

		    // sv_name is set to NULL if it was already removed.  This
		    // happens when it was defined in an inner block and no
		    // functions were defined there.
		    सचे (sv->sv_name != रिक्त)
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
	सचे (eap->cmdidx == CMD_while)
	{
	    /*
	     * ":while bool-expr"
	     */
	    result = eval_to_bool(eap->arg, &error, eap, skip, FALSE);
	}
	अञ्ञथा
	{
	    forinfo_T	*fi;
	    evalarg_T	evalarg;

	    /*
	     * ":for var in list-expr"
	     */
	    fill_evalarg_from_eap(&evalarg, eap, skip);
	    सचे ((cstack->cs_lflags & CSL_HAD_LOOP) != 0)
	    {
		// Jumping here from a ":continue" or ":endfor": use the
		// previously evaluated list.
		fi = cstack->cs_forinfo[cstack->cs_idx];
		error = FALSE;

		// the "in expr" is not used, skip over it
		skip_for_पङ्क्तयः(fi, &evalarg);
	    }
	    अञ्ञथा
	    {
		long save_lnum = SOURCING_LNUM;

		// Evaluate the argument and get the info in a structure.
		fi = eval_for_पङ्क्तिः(eap->arg, &error, eap, &evalarg);
		cstack->cs_forinfo[cstack->cs_idx] = fi;

		// Errors should use the first line number.
		SOURCING_LNUM = save_lnum;
	    }

	    // use the element at the start of the list and advance
	    सचे (!error && fi != रिक्त && !skip)
		result = next_for_item(fi, eap->arg);
	    अञ्ञथा
		result = FALSE;
	    सचे (fi != रिक्त)
		// OR all the cs_flags together, if a function was defined in
		// any round then the loop variable may have been used.
		fi->fi_cs_flags |= prev_cs_flags;

	    सचे (!result)
	    {
		// If a function was defined in any round then set the
		// CSF_FUNC_DEF flag now, so that it's seen by leave_block().
		सचे (fi != रिक्त && (fi->fi_cs_flags & CSF_FUNC_DEF))
		    cstack->cs_flags[cstack->cs_idx] |= CSF_FUNC_DEF;

		free_for_info(fi);
		cstack->cs_forinfo[cstack->cs_idx] = रिक्त;
	    }
	    clear_evalarg(&evalarg, eap);
	}

	/*
	 * If this cstack entry was just initialised and is active, set the
	 * loop flag, so do_cmdline() will set the line number in cs_line[].
	 * If executing the command a second time, clear the loop flag.
	 */
	सचे (!skip && !error && result)
	{
	    cstack->cs_flags[cstack->cs_idx] |= (CSF_ACTIVE | CSF_TRUE);
	    cstack->cs_lflags ^= CSL_HAD_LOOP;
	}
	अञ्ञथा
	{
	    cstack->cs_lflags &= ~CSL_HAD_LOOP;
	    // If the ":while" evaluates to FALSE or ":for" is past the end of
	    // the list, show the debug prompt at the ":endwhile"/":endfor" as
	    // if there was a ":break" in a ":while"/":for" evaluating to
	    // TRUE.
	    सचे (!skip && !error)
		cstack->cs_flags[cstack->cs_idx] |= CSF_TRUE;
	}
    }
}

/*
 * ":continue"
 */
    शून्य
ex_continue(exarg_T *eap)
{
    पूर्णाङ्क		idx;
    cstack_T	*cstack = eap->cstack;

    सचे (cstack->cs_looplevel <= 0 || cstack->cs_idx < 0)
	eap->errmsg = _(e_continue_without_while_or_for);
    अञ्ञथा
    {
	// Try to find the matching ":while".  This might stop at a try
	// conditional not in its finally clause (which is then to be executed
	// next).  Therefore, inactivate all conditionals except the ":while"
	// itself (if reached).
	idx = cleanup_conditionals(cstack, CSF_WHILE | CSF_FOR, FALSE);
	सचे (idx >= 0 && (cstack->cs_flags[idx] & (CSF_WHILE | CSF_FOR)))
	{
	    rewind_conditionals(cstack, idx, CSF_TRY, &cstack->cs_trylevel);

	    /*
	     * Set CSL_HAD_CONT, so do_cmdline() will jump back to the
	     * matching ":while".
	     */
	    cstack->cs_lflags |= CSL_HAD_CONT;	// let do_cmdline() handle it
	}
	अञ्ञथा
	{
	    // If a try conditional not in its finally clause is reached first,
	    // make the ":continue" pending for execution at the ":endtry".
	    cstack->cs_pending[idx] = CSTP_CONTINUE;
	    report_make_pending(CSTP_CONTINUE, रिक्त);
	}
    }
}

/*
 * ":break"
 */
    शून्य
ex_break(exarg_T *eap)
{
    पूर्णाङ्क		idx;
    cstack_T	*cstack = eap->cstack;

    सचे (cstack->cs_looplevel <= 0 || cstack->cs_idx < 0)
	eap->errmsg = _(e_break_without_while_or_for);
    अञ्ञथा
    {
	// Inactivate conditionals until the matching ":while" or a try
	// conditional not in its finally clause (which is then to be
	// executed next) is found.  In the latter case, make the ":break"
	// pending for execution at the ":endtry".
	idx = cleanup_conditionals(cstack, CSF_WHILE | CSF_FOR, TRUE);
	सचे (idx >= 0 && !(cstack->cs_flags[idx] & (CSF_WHILE | CSF_FOR)))
	{
	    cstack->cs_pending[idx] = CSTP_BREAK;
	    report_make_pending(CSTP_BREAK, रिक्त);
	}
    }
}

/*
 * ":endwhile" and ":endfor"
 */
    शून्य
ex_endwhile(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;
    पूर्णाङ्क		idx;
    वर्ण	*err;
    पूर्णाङ्क		csf;
    पूर्णाङ्क		fl;

    सचे (cmdmod_error(TRUE))
	पटिनिवत्त;

    सचे (eap->cmdidx == CMD_endwhile)
    {
	err = e_endwhile_without_while;
	csf = CSF_WHILE;
    }
    अञ्ञथा
    {
	err = e_endfor_without_for;
	csf = CSF_FOR;
    }

    सचे (cstack->cs_looplevel <= 0 || cstack->cs_idx < 0)
	eap->errmsg = _(err);
    अञ्ञथा
    {
	fl = cstack->cs_flags[cstack->cs_idx];
	सचे (!(fl & csf))
	{
	    // If we are in a ":while" or ":for" but used the wrong endloop
	    // command, do not rewind to the next enclosing ":for"/":while".
	    सचे (fl & CSF_WHILE)
		eap->errmsg = _(e_using_endfor_with_while);
	    अञ्ञथा सचे (fl & CSF_FOR)
		eap->errmsg = _(e_using_endwhile_with_for);
	}
	सचे (!(fl & (CSF_WHILE | CSF_FOR)))
	{
	    सचे (!(fl & CSF_TRY))
		eap->errmsg = _(e_missing_endif);
	    अञ्ञथा सचे (fl & CSF_FINALLY)
		eap->errmsg = _(e_missing_endtry);
	    // Try to find the matching ":while" and report what's missing.
	    अत्थाय (idx = cstack->cs_idx; idx > 0; --idx)
	    {
		fl =  cstack->cs_flags[idx];
		सचे ((fl & CSF_TRY) && !(fl & CSF_FINALLY))
		{
		    // Give up at a try conditional not in its finally clause.
		    // Ignore the ":endwhile"/":endfor".
		    eap->errmsg = _(err);
		    पटिनिवत्त;
		}
		सचे (fl & csf)
		    भङ्ग;
	    }
	    // Cleanup and rewind all contained (and unclosed) conditionals.
	    (शून्य)cleanup_conditionals(cstack, CSF_WHILE | CSF_FOR, FALSE);
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
	अञ्ञथा सचे (cstack->cs_flags[cstack->cs_idx] & CSF_TRUE
		&& !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE)
		&& dbg_check_skipped(eap))
	    (शून्य)do_intthrow(cstack);

	// Set loop flag, so do_cmdline() will jump back to the matching
	// ":while" or ":for".
	cstack->cs_lflags |= CSL_HAD_ENDLOOP;
    }
}

/*
 * "{" start of a block in Vim9 script
 */
    शून्य
ex_block(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;

    सचे (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_block_nesting_too_deep);
    अञ्ञथा
    {
	enter_block(cstack);
	cstack->cs_flags[cstack->cs_idx] = CSF_BLOCK | CSF_ACTIVE | CSF_TRUE;
    }
}

/*
 * "}" end of a block in Vim9 script
 */
    शून्य
ex_endblock(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;

    सचे (cstack->cs_idx < 0
	    || (cstack->cs_flags[cstack->cs_idx] & CSF_BLOCK) == 0)
	eap->errmsg = _(e_endblock_without_block);
    अञ्ञथा
	leave_block(cstack);
}

    पूर्णाङ्क
inside_block(exarg_T *eap)
{
    cstack_T	*cstack = eap->cstack;
    पूर्णाङ्क		i;

    // Control flow commands handle '|' themselves and must not be treated
    // as if inside a block even when they are - otherwise '|' separators
    // in "try | catch | endtry" etc. would not be recognized.
    विकल्प (eap->cmdidx)
    {
	पक्ष CMD_if:
	पक्ष CMD_elseif:
	पक्ष CMD_else:
	पक्ष CMD_endif:
	पक्ष CMD_while:
	पक्ष CMD_endwhile:
	पक्ष CMD_for:
	पक्ष CMD_endfor:
	पक्ष CMD_try:
	पक्ष CMD_catch:
	पक्ष CMD_finally:
	पक्ष CMD_endtry:
	    पटिनिवत्त FALSE;
	स्वभाव:
	    भङ्ग;
    }

    अत्थाय (i = 0; i <= cstack->cs_idx; ++i)
	सचे (cstack->cs_flags[i] & CSF_BLOCK)
	    पटिनिवत्त TRUE;
    पटिनिवत्त FALSE;
}

/*
 * ":throw expr"
 */
    शून्य
ex_throw(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*मानम्;

    सचे (*arg != NUL && *arg != '|' && *arg != '\n')
	मानम् = eval_to_string_skip(arg, eap, eap->skip);
    अञ्ञथा
    {
	emsg(_(e_argument_required));
	मानम् = रिक्त;
    }

    // On error or when an exception is thrown during argument evaluation, do
    // not throw.
    सचे (!eap->skip && मानम् != रिक्त)
    {
	सचे (throw_exception(मानम्, ET_USER, रिक्त) == FAIL)
	    vim_free(मानम्);
	अञ्ञथा
	    do_throw(eap->cstack);
    }
}

/*
 * Throw the current exception through the specified cstack.  Common routine
 * for ":throw" (user exception) and error and interrupt exceptions.  Also
 * used for rethrowing an uncaught exception.
 */
    शून्य
do_throw(cstack_T *cstack)
{
    पूर्णाङ्क		idx;
    पूर्णाङ्क		inactivate_try = FALSE;

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
    सचे (did_emsg && !THROW_ON_ERROR)
    {
	inactivate_try = TRUE;
	did_emsg = FALSE;
    }
#endif
#ifndef THROW_ON_INTERRUPT_TRUE
    सचे (got_int && !THROW_ON_INTERRUPT)
    {
	inactivate_try = TRUE;
	got_int = FALSE;
    }
#endif
    idx = cleanup_conditionals(cstack, 0, inactivate_try);
    सचे (idx >= 0)
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
	सचे (!(cstack->cs_flags[idx] & CSF_CAUGHT))
	{
	    सचे (cstack->cs_flags[idx] & CSF_ACTIVE)
		cstack->cs_flags[idx] |= CSF_THROWN;
	    अञ्ञथा
		// THROWN may have already been set for a catchable exception
		// that has been discarded.  Ensure it is reset for the new
		// exception.
		cstack->cs_flags[idx] &= ~CSF_THROWN;
	}
	cstack->cs_flags[idx] &= ~CSF_ACTIVE;
	cstack->cs_exception[idx] = current_exception;
    }
#सचे 0
    // TODO: Add optimization below.  Not yet done because of interface
    // problems to eval.c and ex_cmds2.c. (Servatius)
    अञ्ञथा
    {
	/*
	 * There are no catch clauses to check or finally clauses to execute.
	 * End the current script or function.  The exception will be rethrown
	 * in the caller.
	 */
	सचे (getline_equal(eap->getline, eap->cookie, get_func_पङ्क्तिः))
	    current_funccal->returned = TRUE;
	elseif (eap->get_func_पङ्क्तिः == getsourceline)
	    ((संरचना source_cookie *)eap->cookie)->finished = TRUE;
    }
#endif

    did_throw = TRUE;
}

/*
 * ":try"
 */
    शून्य
ex_try(exarg_T *eap)
{
    पूर्णाङ्क		skip;
    cstack_T	*cstack = eap->cstack;

    सचे (cmdmod_error(FALSE))
	पटिनिवत्त;

    सचे (cstack->cs_idx == CSTACK_LEN - 1)
	eap->errmsg = _(e_try_nesting_too_deep);
    अञ्ञथा
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

	सचे (!skip)
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
	    सचे (emsg_silent)
	    {
		eslist_T	*elem;

		elem = ALLOC_ONE(संरचना eslist_elem);
		सचे (elem == रिक्त)
		    emsg(_(e_out_of_memory));
		अञ्ञथा
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
    शून्य
ex_catch(exarg_T *eap)
{
    पूर्णाङ्क		idx = 0;
    पूर्णाङ्क		give_up = FALSE;
    पूर्णाङ्क		skip = FALSE;
    पूर्णाङ्क		caught = FALSE;
    char_u	*end;
    पूर्णाङ्क		save_char = 0;
    char_u	*save_cpo;
    regmatch_T	regmatch;
    पूर्णाङ्क		prev_got_int;
    cstack_T	*cstack = eap->cstack;
    char_u	*pat;

    सचे (cmdmod_error(FALSE))
	पटिनिवत्त;

    सचे (cstack->cs_trylevel <= 0 || cstack->cs_idx < 0)
    {
	eap->errmsg = _(e_catch_without_try);
	give_up = TRUE;
    }
    अञ्ञथा
    {
	सचे (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
	{
	    // Report what's missing if the matching ":try" is not in its
	    // finally clause.
	    eap->errmsg = get_end_emsg(cstack);
	    skip = TRUE;
	}
	अत्थाय (idx = cstack->cs_idx; idx > 0; --idx)
	    सचे (cstack->cs_flags[idx] & CSF_TRY)
		भङ्ग;
	सचे (cstack->cs_flags[idx] & CSF_TRY)
	    cstack->cs_flags[idx] |= CSF_CATCH;
	सचे (cstack->cs_flags[idx] & CSF_FINALLY)
	{
	    // Give up for a ":catch" after ":finally" and ignore it.
	    // Just parse.
	    eap->errmsg = _(e_catch_after_finally);
	    give_up = TRUE;
	}
	अञ्ञथा
	    rewind_conditionals(cstack, idx, CSF_WHILE | CSF_FOR,
						       &cstack->cs_looplevel);
    }

    सचे (ends_excmd2(eap->cmd, eap->arg))   // no argument, catch all errors
    {
	pat = (char_u *)".*";
	end = रिक्त;
	eap->nextcmd = find_nextcmd(eap->arg);
    }
    अञ्ञथा
    {
	pat = eap->arg + 1;
	end = skip_regexp_err(pat, *eap->arg, TRUE);
	सचे (end == रिक्त)
	    give_up = TRUE;
    }

    सचे (!give_up)
    {
	/*
	 * Don't do something when no exception has been thrown or when the
	 * corresponding try block never got active (because of an inactive
	 * surrounding conditional or after an error or interrupt or throw).
	 */
	सचे (!did_throw || !(cstack->cs_flags[idx] & CSF_TRUE))
	    skip = TRUE;

	/*
	 * Check for a match only if an exception is thrown but not caught by
	 * a previous ":catch".  An exception that has replaced a discarded
	 * exception is not checked (THROWN is not set then).
	 */
	सचे (!skip && (cstack->cs_flags[idx] & CSF_THROWN)
		&& !(cstack->cs_flags[idx] & CSF_CAUGHT))
	{
	    सचे (end != रिक्त && *end != NUL
				      && !ends_excmd2(end, skipwhite(end + 1)))
	    {
		semsg(_(e_trailing_characters_str), end);
		पटिनिवत्त;
	    }

	    // When debugging or a breakpoint was encountered, display the
	    // debug prompt (if not already done) before checking for a match.
	    // This is a helpful hint for the user when the regular expression
	    // matching fails.  Handle a ">quit" debug command as if an
	    // interrupt had occurred before the ":catch".  That is, discard
	    // the original exception, replace it by an interrupt exception,
	    // and don't catch it in this try block.
	    सचे (!dbg_check_skipped(eap) || !do_intthrow(cstack))
	    {
		// Terminate the pattern and avoid the 'l' flag in 'cpoptions'
		// while compiling it.
		सचे (end != रिक्त)
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
		सचे (end != रिक्त)
		    *end = save_char;
		p_cpo = save_cpo;
		सचे (regmatch.regprog == रिक्त)
		    semsg(_(e_invalid_argument_str), pat);
		अञ्ञथा
		{
		    /*
		     * Save the value of got_int and reset it.  We don't want
		     * a previous interruption cancel matching, only hitting
		     * CTRL-C while matching should abort it.
		     */
		    prev_got_int = got_int;
		    got_int = FALSE;
		    caught = vim_regexec_nl(&regmatch,
			       (char_u *)current_exception->मानम्, (colnr_T)0);
		    got_int |= prev_got_int;
		    vim_regfree(regmatch.regprog);
		}
	    }
	}

	सचे (caught)
	{
	    // Make this ":catch" clause active and reset did_emsg, got_int,
	    // and did_throw.  Put the exception on the caught stack.
	    cstack->cs_flags[idx] |= CSF_ACTIVE | CSF_CAUGHT;
	    did_emsg = got_int = did_throw = FALSE;
	    catch_exception((except_T *)cstack->cs_exception[idx]);

	    सचे (cstack->cs_idx >= 0
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
	    सचे (cstack->cs_exception[cstack->cs_idx] != current_exception)
		internal_error("ex_catch()");
	}
	अञ्ञथा
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

    सचे (end != रिक्त)
	eap->nextcmd = find_nextcmd(end);
}

/*
 * ":finally"
 */
    शून्य
ex_finally(exarg_T *eap)
{
    पूर्णाङ्क		idx;
    पूर्णाङ्क		skip = FALSE;
    पूर्णाङ्क		pending = CSTP_NONE;
    cstack_T	*cstack = eap->cstack;

    सचे (cmdmod_error(FALSE))
	पटिनिवत्त;

    अत्थाय (idx = cstack->cs_idx; idx >= 0; --idx)
	सचे (cstack->cs_flags[idx] & CSF_TRY)
	    भङ्ग;
    सचे (cstack->cs_trylevel <= 0 || idx < 0)
    {
	eap->errmsg = _(e_finally_without_try);
	पटिनिवत्त;
    }

    सचे (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
    {
	eap->errmsg = get_end_emsg(cstack);
	// Make this error pending, so that the commands in the following
	// finally clause can be executed.  This overrules also a pending
	// ":continue", ":break", ":return", or ":finish".
	pending = CSTP_ERROR;
    }

    सचे (cstack->cs_flags[idx] & CSF_FINALLY)
    {
	// Give up for a multiple ":finally" and ignore it.
	eap->errmsg = _(e_multiple_finally);
	पटिनिवत्त;
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

    सचे (!skip)
    {
	// When debugging or a breakpoint was encountered, display the
	// debug prompt (if not already done).  The user then knows that the
	// finally clause is executed.
	सचे (dbg_check_skipped(eap))
	{
	    // Handle a ">quit" debug command as if an interrupt had
	    // occurred before the ":finally".  That is, discard the
	    // original exception and replace it by an interrupt
	    // exception.
	    (शून्य)do_intthrow(cstack);
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

	सचे (cstack->cs_idx >= 0
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
	सचे (pending == CSTP_ERROR || did_emsg || got_int || did_throw)
	{
	    सचे (cstack->cs_pending[cstack->cs_idx] == CSTP_RETURN)
	    {
		report_discard_pending(CSTP_RETURN,
				       cstack->cs_rettv[cstack->cs_idx]);
		discard_pending_return(cstack->cs_rettv[cstack->cs_idx]);
	    }
	    सचे (pending == CSTP_ERROR && !did_emsg)
		pending |= (THROW_ON_ERROR) ? CSTP_THROW : 0;
	    अञ्ञथा
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
	    सचे (did_throw && cstack->cs_exception[cstack->cs_idx]
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
    शून्य
ex_endtry(exarg_T *eap)
{
    पूर्णाङ्क		idx;
    पूर्णाङ्क		skip;
    पूर्णाङ्क		rethrow = FALSE;
    पूर्णाङ्क		pending = CSTP_NONE;
    शून्य	*rettv = रिक्त;
    cstack_T	*cstack = eap->cstack;

    सचे (cmdmod_error(FALSE))
	पटिनिवत्त;

    अत्थाय (idx = cstack->cs_idx; idx >= 0; --idx)
	सचे (cstack->cs_flags[idx] & CSF_TRY)
	    भङ्ग;
    सचे (cstack->cs_trylevel <= 0 || idx < 0)
    {
	eap->errmsg = _(e_endtry_without_try);
	पटिनिवत्त;
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

    सचे (!(cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
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
	सचे (did_throw)
	    discard_current_exception();

	// report eap->errmsg, also when there already was an error
	did_emsg = FALSE;
    }
    अञ्ञथा
    {
	idx = cstack->cs_idx;

	// Check the flags only when not in a skipped block.
	सचे (!skip && in_vim9script()
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
	सचे (did_throw && (cstack->cs_flags[idx] & CSF_TRUE)
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
    सचे ((rethrow || (!skip && !(cstack->cs_flags[idx] & CSF_FINALLY)
		    && !cstack->cs_pending[idx]))
	    && dbg_check_skipped(eap))
    {
	// Handle a ">quit" debug command as if an interrupt had occurred
	// before the ":endtry".  That is, throw an interrupt exception and
	// set "skip" and "rethrow".
	सचे (got_int)
	{
	    skip = TRUE;
	    (शून्य)do_intthrow(cstack);
	    // The do_intthrow() call may have reset did_throw or
	    // cstack->cs_pending[idx].
	    rethrow = FALSE;
	    सचे (did_throw && !(cstack->cs_flags[idx] & CSF_FINALLY))
		rethrow = TRUE;
	}
    }

    /*
     * If a ":return" is pending, we need to resume it after closing the
     * try conditional; remember the return value.  If there was a finally
     * clause making an exception pending, we need to rethrow it.  Make it
     * the exception currently being thrown.
     */
    सचे (!skip)
    {
	pending = cstack->cs_pending[idx];
	cstack->cs_pending[idx] = CSTP_NONE;
	सचे (pending == CSTP_RETURN)
	    rettv = cstack->cs_rettv[idx];
	अञ्ञथा सचे (pending & CSTP_THROW)
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
    (शून्य)cleanup_conditionals(cstack, CSF_TRY | CSF_SILENT, TRUE);

    सचे (cstack->cs_idx >= 0 && (cstack->cs_flags[cstack->cs_idx] & CSF_TRY))
	leave_block(cstack);
    --cstack->cs_trylevel;

    सचे (!skip)
    {
	report_resume_pending(pending,
		    (pending == CSTP_RETURN) ? rettv :
		    (pending & CSTP_THROW) ? (शून्य *)current_exception : रिक्त);
	विकल्प (pending)
	{
	    पक्ष CSTP_NONE:
		भङ्ग;

	    // Reactivate a pending ":continue", ":break", ":return",
	    // ":finish" from the try block or a catch clause of this try
	    // conditional.  This is skipped, if there was an error in an
	    // (unskipped) conditional command or an interrupt afterwards
	    // or if the finally clause is present and executed a new error,
	    // interrupt, throw, ":continue", ":break", ":return", or
	    // ":finish".
	    पक्ष CSTP_CONTINUE:
		ex_continue(eap);
		भङ्ग;
	    पक्ष CSTP_BREAK:
		ex_break(eap);
		भङ्ग;
	    पक्ष CSTP_RETURN:
		do_return(eap, FALSE, FALSE, rettv);
		भङ्ग;
	    पक्ष CSTP_FINISH:
		do_finish(eap, FALSE);
		भङ्ग;

	    // When the finally clause was entered due to an error,
	    // interrupt or throw (as opposed to a ":continue", ":break",
	    // ":return", or ":finish"), restore the pending values of
	    // did_emsg, got_int, and did_throw.  This is skipped, if there
	    // was a new error, interrupt, throw, ":continue", ":break",
	    // ":return", or ":finish".  in the finally clause.
	    स्वभाव:
		सचे (pending & CSTP_ERROR)
		    did_emsg = TRUE;
		सचे (pending & CSTP_INTERRUPT)
		    got_int = TRUE;
		सचे (pending & CSTP_THROW)
		    rethrow = TRUE;
		भङ्ग;
	}
    }

    सचे (rethrow)
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
    शून्य
enter_cleanup(cleanup_T *csp)
{
    पूर्णाङ्क		pending = CSTP_NONE;

    /*
     * Postpone did_emsg, got_int, did_throw.  The pending values will be
     * restored by leave_cleanup() except if there was an aborting error,
     * interrupt, or uncaught exception after this function ends.
     */
    सचे (did_emsg || got_int || did_throw || need_rethrow)
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
	सचे (did_throw || need_rethrow)
	{
	    csp->exception = current_exception;
	    current_exception = रिक्त;
	}
	अञ्ञथा
	{
	    csp->exception = रिक्त;
	    सचे (did_emsg)
	    {
		force_abort |= cause_abort;
		cause_abort = FALSE;
	    }
	}
	did_emsg = got_int = did_throw = need_rethrow = FALSE;

	// Report if required by the 'verbose' option or when debugging.
	report_make_pending(pending, csp->exception);
    }
    अञ्ञथा
    {
	csp->pending = CSTP_NONE;
	csp->exception = रिक्त;
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
    शून्य
leave_cleanup(cleanup_T *csp)
{
    पूर्णाङ्क		pending = csp->pending;

    सचे (pending == CSTP_NONE)	// nothing to do
	पटिनिवत्त;

    // If there was an aborting error, an interrupt, or an uncaught exception
    // after the corresponding call to enter_cleanup(), discard what has been
    // made pending by it.  Report this to the user if required by the
    // 'verbose' option or when debugging.
    सचे (aborting() || need_rethrow)
    {
	सचे (pending & CSTP_THROW)
	    // Cancel the pending exception (includes report).
	    discard_exception(csp->exception, FALSE);
	अञ्ञथा
	    report_discard_pending(pending, रिक्त);

	// If an error was about to be converted to an exception when
	// enter_cleanup() was called, free the message list.
	सचे (सन्देशः_list != रिक्त)
	    free_global_msglist();
    }

    /*
     * If there was no new error, interrupt, or throw between the calls
     * to enter_cleanup() and leave_cleanup(), restore the pending
     * error/interrupt/exception state.
     */
    अञ्ञथा
    {
	/*
	 * If there was an exception being thrown when enter_cleanup() was
	 * called, we need to rethrow it.  Make it the exception currently
	 * being thrown.
	 */
	सचे (pending & CSTP_THROW)
	    current_exception = csp->exception;

	/*
	 * If an error was about to be converted to an exception when
	 * enter_cleanup() was called, let "cause_abort" take the part of
	 * "force_abort" (as done by cause_errthrow()).
	 */
	अञ्ञथा सचे (pending & CSTP_ERROR)
	{
	    cause_abort = force_abort;
	    force_abort = FALSE;
	}

	/*
	 * Restore the pending values of did_emsg, got_int, and did_throw.
	 */
	सचे (pending & CSTP_ERROR)
	    did_emsg = TRUE;
	सचे (pending & CSTP_INTERRUPT)
	    got_int = TRUE;
	सचे (pending & CSTP_THROW)
	    need_rethrow = TRUE;    // did_throw will be set by do_one_cmd()

	// Report if required by the 'verbose' option or when debugging.
	report_resume_pending(pending,
		   (pending & CSTP_THROW) ? (शून्य *)current_exception : रिक्त);
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
    पूर्णाङ्क
cleanup_conditionals(
    cstack_T   *cstack,
    पूर्णाङ्क		searched_cond,
    पूर्णाङ्क		inclusive)
{
    पूर्णाङ्क		idx;
    पूर्णाङ्क		stop = FALSE;

    अत्थाय (idx = cstack->cs_idx; idx >= 0; --idx)
    {
	सचे (cstack->cs_flags[idx] & CSF_TRY)
	{
	    /*
	     * Discard anything pending in a finally clause and continue the
	     * search.  There may also be a pending ":continue", ":break",
	     * ":return", or ":finish" before the finally clause.  We must not
	     * discard it, unless an error or interrupt occurred afterwards.
	     */
	    सचे (did_emsg || got_int || (cstack->cs_flags[idx] & CSF_FINALLY))
	    {
		विकल्प (cstack->cs_pending[idx])
		{
		    पक्ष CSTP_NONE:
			भङ्ग;

		    पक्ष CSTP_CONTINUE:
		    पक्ष CSTP_BREAK:
		    पक्ष CSTP_FINISH:
			report_discard_pending(cstack->cs_pending[idx], रिक्त);
			cstack->cs_pending[idx] = CSTP_NONE;
			भङ्ग;

		    पक्ष CSTP_RETURN:
			report_discard_pending(CSTP_RETURN,
						      cstack->cs_rettv[idx]);
			discard_pending_return(cstack->cs_rettv[idx]);
			cstack->cs_pending[idx] = CSTP_NONE;
			भङ्ग;

		    स्वभाव:
			सचे (cstack->cs_flags[idx] & CSF_FINALLY)
			{
			    सचे ((cstack->cs_pending[idx] & CSTP_THROW)
				    && cstack->cs_exception[idx] != रिक्त)
			    {
				// Cancel the pending exception.  This is in the
				// finally clause, so that the stack of the
				// caught exceptions is not involved.
				discard_exception(
					(except_T *)cstack->cs_exception[idx],
					FALSE);
			    }
			    अञ्ञथा
				report_discard_pending(cstack->cs_pending[idx],
					रिक्त);
			    cstack->cs_pending[idx] = CSTP_NONE;
			}
			भङ्ग;
		}
	    }

	    /*
	     * Stop at a try conditional not in its finally clause.  If this try
	     * conditional is in an active catch clause, finish the caught
	     * exception.
	     */
	    सचे (!(cstack->cs_flags[idx] & CSF_FINALLY))
	    {
		सचे ((cstack->cs_flags[idx] & CSF_ACTIVE)
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
		सचे (cstack->cs_flags[idx] & CSF_TRUE)
		{
		    सचे (searched_cond == 0 && !inclusive)
			भङ्ग;
		    stop = TRUE;
		}
	    }
	}

	// Stop on the searched conditional type (even when the surrounding
	// conditional is not active or something has been made pending).
	// If "inclusive" is TRUE and "searched_cond" is CSF_TRY|CSF_SILENT,
	// check first whether "emsg_silent" needs to be restored.
	सचे (cstack->cs_flags[idx] & searched_cond)
	{
	    सचे (!inclusive)
		भङ्ग;
	    stop = TRUE;
	}
	cstack->cs_flags[idx] &= ~CSF_ACTIVE;
	सचे (stop && searched_cond != (CSF_TRY | CSF_SILENT))
	    भङ्ग;

	/*
	 * When leaving a try conditional that reset "emsg_silent" on its
	 * entry after saving the original value, restore that value here and
	 * free the memory used to store it.
	 */
	सचे ((cstack->cs_flags[idx] & CSF_TRY)
		&& (cstack->cs_flags[idx] & CSF_SILENT))
	{
	    eslist_T	*elem;

	    elem = cstack->cs_emsg_silent_list;
	    cstack->cs_emsg_silent_list = elem->next;
	    emsg_silent = elem->saved_emsg_silent;
	    vim_free(elem);
	    cstack->cs_flags[idx] &= ~CSF_SILENT;
	}
	सचे (stop)
	    भङ्ग;
    }
    पटिनिवत्त idx;
}

/*
 * Return an appropriate error message for a missing endwhile/endfor/endif.
 */
   स्थिर वर्ण *
get_end_emsg(cstack_T *cstack)
{
    सचे (cstack->cs_flags[cstack->cs_idx] & CSF_WHILE)
	पटिनिवत्त _(e_missing_endwhile);
    सचे (cstack->cs_flags[cstack->cs_idx] & CSF_FOR)
	पटिनिवत्त _(e_missing_endfor);
    पटिनिवत्त _(e_missing_endif);
}


/*
 * Rewind conditionals until index "idx" is reached.  "cond_type" and
 * "cond_level" specify a conditional type and the address of a level variable
 * which is to be decremented with each skipped conditional of the specified
 * type.
 * Also free "for info" structures where needed.
 */
    शून्य
rewind_conditionals(
    cstack_T   *cstack,
    पूर्णाङ्क		idx,
    पूर्णाङ्क		cond_type,
    पूर्णाङ्क		*cond_level)
{
    याव (cstack->cs_idx > idx)
    {
	सचे (cstack->cs_flags[cstack->cs_idx] & cond_type)
	    --*cond_level;
	सचे (cstack->cs_flags[cstack->cs_idx] & CSF_FOR)
	    free_for_info(cstack->cs_forinfo[cstack->cs_idx]);
	leave_block(cstack);
    }
}

/*
 * ":endfunction" or ":enddef" when not after a ":function"
 */
    शून्य
ex_endfunction(exarg_T *eap)
{
    सचे (eap->cmdidx == CMD_enddef)
	semsg(_(e_str_not_inside_function), ":enddef");
    अञ्ञथा
	semsg(_(e_str_not_inside_function), ":endfunction");
}

/*
 * Return TRUE if the string "p" looks like a ":while" or ":for" command.
 */
    पूर्णाङ्क
has_loop_cmd(char_u *p)
{
    पूर्णाङ्क		len;

    // skip modifiers, white space and ':'
    अत्थाय (;;)
    {
	याव (*p == ' ' || *p == '\t' || *p == ':')
	    ++p;
	len = modifier_len(p);
	सचे (len == 0)
	    भङ्ग;
	p += len;
    }
    सचे ((p[0] == 'w' && p[1] == 'h')
	    || (p[0] == 'f' && p[1] == 'o' && p[2] == 'r'))
	पटिनिवत्त TRUE;
    पटिनिवत्त FALSE;
}

#endif // FEAT_EVAL
