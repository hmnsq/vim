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

#जर defined(FEAT_EVAL)

// Log file opened with ch_logfile().
स्थिर खाता *log_fd = रिक्त;
स्थिर char_u *log_name = रिक्त;
#ifdef FEAT_RELTIME
स्थिर proftime_T log_start;
#endif

    शून्य
ch_logfile(char_u *fname, char_u *opt)
{
    खाता	*file = रिक्त;
    अक्षर	*mode = "a";

    जर (log_fd != रिक्त)
    {
	जर (*fname != NUL)
	    ch_log(रिक्त, "closing this logfile, opening %s", fname);
	अन्यथा
	    ch_log(रिक्त, "closing logfile %s", log_name);
	ख_बंद(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    जर (vim_strchr(opt, 'a') == रिक्त && vim_strchr(opt, 'w') != रिक्त)
	mode = "w";
    ch_log_output = vim_strchr(opt, 'o') != रिक्त ? LOG_ALWAYS : FALSE;

    जर (*fname != NUL)
    {
	file = ख_खोलो((अक्षर *)fname, mode);
	जर (file == रिक्त)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    परत;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    जर (log_fd != रिक्त)
    {
	ख_लिखो(log_fd, "==== start log session %s ====\n",
						 get_ctime(समय(रिक्त), FALSE));
	// flush now, if fork/exec follows it could be written twice
	ख_साफ(log_fd);
#ifdef FEAT_RELTIME
	profile_start(&log_start);
#endif
    }
}

    पूर्णांक
ch_log_active(शून्य)
{
    परत log_fd != रिक्त;
}

    स्थिर शून्य
ch_log_lead(स्थिरांक अक्षर *what, channel_T *ch UNUSED, ch_part_T part UNUSED)
{
    जर (log_fd == रिक्त)
	परत;

#ifdef FEAT_RELTIME
    proftime_T log_now;

    profile_start(&log_now);
    profile_sub(&log_now, &log_start);
    ख_लिखो(log_fd, "%s ", profile_संदेश(&log_now));
#endif
#ifdef FEAT_JOB_CHANNEL
    जर (ch != रिक्त)
    {
	जर (part < PART_मोजणी)
	    ख_लिखो(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_names[part]);
	अन्यथा
	    ख_लिखो(log_fd, "%son %d: ", what, ch->ch_id);
    }
    अन्यथा
#endif
	ख_लिखो(log_fd, "%s: ", what);
}

#ifndef PROTO  // prototype is in proto.h
    शून्य
ch_log(channel_T *ch, स्थिरांक अक्षर *fmt, ...)
{
    जर (log_fd == रिक्त)
	परत;

    बहु_सूची ap;

    ch_log_lead("", ch, PART_मोजणी);
    बहु_शुरू(ap, fmt);
    भख_लिखो(log_fd, fmt, ap);
    बहु_पूर्ण(ap);
    ख_अक्षर_दो('\n', log_fd);
    ख_साफ(log_fd);
    did_repeated_संदेश = 0;
}

    शून्य
ch_error(channel_T *ch, स्थिरांक अक्षर *fmt, ...)
{
    जर (log_fd == रिक्त)
	परत;

    बहु_सूची ap;

    ch_log_lead("ERR ", ch, PART_मोजणी);
    बहु_शुरू(ap, fmt);
    भख_लिखो(log_fd, fmt, ap);
    बहु_पूर्ण(ap);
    ख_अक्षर_दो('\n', log_fd);
    ख_साफ(log_fd);
    did_repeated_संदेश = 0;
}
#endif

#जर defined(FEAT_JOB_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    शून्य
ch_log_literal(
	अक्षर	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	पूर्णांक	    len)
{
    ch_log_lead(lead, ch, part);
    ख_लिखो(log_fd, "'");
    vim_ignored = (पूर्णांक)ख_डालो(buf, len, 1, log_fd);
    ख_लिखो(log_fd, "'\n");
    ख_साफ(log_fd);
}
#endif

/*
 * "ch_log()" function
 */
    शून्य
f_ch_log(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*संदेश;
    channel_T	*channel = रिक्त;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_job_arg(argvars, 1) == FAIL))
	परत;

    संदेश = tv_get_string(&argvars[0]);
#जर defined(FEAT_JOB_CHANNEL)
    जर (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#endif

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", संदेश);
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
    जर (check_restricted() || check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	परत;

    fname = tv_get_string(&argvars[0]);
    जर (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#endif // FEAT_EVAL
