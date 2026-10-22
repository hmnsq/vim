/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Implements logging.  Originally intended for the channel feature, which is
 * why the "ch_" prefix is used.  Also useful for any kind of low-level and
 * async debugging.
 */

#अन्तर्भाव "vim.h"

#सचे defined(FEAT_EVAL)

// Log file opened with ch_logfile().
स्थिर FILE *log_fd = रिक्त;
स्थिर char_u *log_name = रिक्त;
#ifdef FEAT_RELTIME
स्थिर proftime_T log_start;
#endif

    शून्य
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = रिक्त;
    वर्ण	*mode = "a";

    सचे (log_fd != रिक्त)
    {
	सचे (*fname != NUL)
	    ch_log(रिक्त, "closing this logfile, opening %s", fname);
	अञ्ञथा
	    ch_log(रिक्त, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    सचे (vim_strchr(opt, 'a') == रिक्त && vim_strchr(opt, 'w') != रिक्त)
	mode = "w";
    ch_log_output = vim_strchr(opt, 'o') != रिक्त ? LOG_ALWAYS : FALSE;

    सचे (*fname != NUL)
    {
	file = fopen((वर्ण *)fname, mode);
	सचे (file == रिक्त)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    पटिनिवत्त;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    सचे (log_fd != रिक्त)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(time(रिक्त), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#ifdef FEAT_RELTIME
	profile_start(&log_start);
#endif
    }
}

    पूर्णाङ्क
ch_log_active(शून्य)
{
    पटिनिवत्त log_fd != रिक्त;
}

    स्थिर शून्य
ch_log_lead(ध्रुव वर्ण *what, channel_T *ch UNUSED, ch_part_T part UNUSED)
{
    सचे (log_fd == रिक्त)
	पटिनिवत्त;

#ifdef FEAT_RELTIME
    proftime_T log_now;

    profile_start(&log_now);
    profile_sub(&log_now, &log_start);
    fprintf(log_fd, "%s ", profile_सन्देशः(&log_now));
#endif
#ifdef FEAT_JOB_CHANNEL
    सचे (ch != रिक्त)
    {
	सचे (part < PART_गणनम्)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_names[part]);
	अञ्ञथा
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    अञ्ञथा
#endif
	fprintf(log_fd, "%s: ", what);
}

#ifndef PROTO  // prototype is in proto.h
    शून्य
ch_log(channel_T *ch, ध्रुव वर्ण *fmt, ...)
{
    सचे (log_fd == रिक्त)
	पटिनिवत्त;

    va_list ap;

    ch_log_lead("", ch, PART_गणनम्);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_सन्देशः = 0;
}

    शून्य
ch_error(channel_T *ch, ध्रुव वर्ण *fmt, ...)
{
    सचे (log_fd == रिक्त)
	पटिनिवत्त;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_गणनम्);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_सन्देशः = 0;
}
#endif

#सचे defined(FEAT_JOB_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    शून्य
ch_log_literal(
	वर्ण	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	पूर्णाङ्क	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (पूर्णाङ्क)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#endif

/*
 * "ch_log()" function
 */
    शून्य
f_ch_log(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*सन्देशः;
    channel_T	*channel = रिक्त;

    सचे (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_job_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    सन्देशः = tv_get_string(&argvars[0]);
#सचे defined(FEAT_JOB_CHANNEL)
    सचे (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#endif

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", सन्देशः);
}

/*
 * "ch_logfile()" function
 */
    शून्य
f_ch_logfile(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    सचे (check_restricted() || check_secure())
	पटिनिवत्त;

    सचे (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    fname = tv_get_string(&argvars[0]);
    सचे (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#endif // FEAT_EVAL
