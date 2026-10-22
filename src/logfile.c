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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_EVAL)

// Log file opened with ch_logfile().
স্থির FILE *log_fd = রিক্ত;
স্থির char_u *log_name = রিক্ত;
#ifdef FEAT_RELTIME
স্থির proftime_T log_start;
#endif

    শূন্য
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = রিক্ত;
    ৱর্ণ	*mode = "a";

    যদি (log_fd != রিক্ত)
    {
	যদি (*fname != NUL)
	    ch_log(রিক্ত, "closing this logfile, opening %s", fname);
	অন্যথা
	    ch_log(রিক্ত, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    যদি (vim_strchr(opt, 'a') == রিক্ত && vim_strchr(opt, 'w') != রিক্ত)
	mode = "w";
    ch_log_output = vim_strchr(opt, 'o') != রিক্ত ? LOG_ALWAYS : FALSE;

    যদি (*fname != NUL)
    {
	file = fopen((ৱর্ণ *)fname, mode);
	যদি (file == রিক্ত)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ঘূৰাই;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    যদি (log_fd != রিক্ত)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(time(রিক্ত), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#ifdef FEAT_RELTIME
	profile_start(&log_start);
#endif
    }
}

    পূর্ণাঙ্ক
ch_log_active(শূন্য)
{
    ঘূৰাই log_fd != রিক্ত;
}

    স্থির শূন্য
ch_log_lead(ধ্রুৱ ৱর্ণ *what, channel_T *ch UNUSED, ch_part_T part UNUSED)
{
    যদি (log_fd == রিক্ত)
	ঘূৰাই;

#ifdef FEAT_RELTIME
    proftime_T log_now;

    profile_start(&log_now);
    profile_sub(&log_now, &log_start);
    fprintf(log_fd, "%s ", profile_বাৰ্তা(&log_now));
#endif
#ifdef FEAT_JOB_CHANNEL
    যদি (ch != রিক্ত)
    {
	যদি (part < PART_গুনতি)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_names[part]);
	অন্যথা
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    অন্যথা
#endif
	fprintf(log_fd, "%s: ", what);
}

#ifndef PROTO  // prototype is in proto.h
    শূন্য
ch_log(channel_T *ch, ধ্রুৱ ৱর্ণ *fmt, ...)
{
    যদি (log_fd == রিক্ত)
	ঘূৰাই;

    va_list ap;

    ch_log_lead("", ch, PART_গুনতি);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_বাৰ্তা = 0;
}

    শূন্য
ch_error(channel_T *ch, ধ্রুৱ ৱর্ণ *fmt, ...)
{
    যদি (log_fd == রিক্ত)
	ঘূৰাই;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_গুনতি);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_বাৰ্তা = 0;
}
#endif

#যদি defined(FEAT_JOB_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    শূন্য
ch_log_literal(
	ৱর্ণ	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	পূর্ণাঙ্ক	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (পূর্ণাঙ্ক)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#endif

/*
 * "ch_log()" function
 */
    শূন্য
f_ch_log(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*বাৰ্তা;
    channel_T	*channel = রিক্ত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_job_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    বাৰ্তা = tv_get_string(&argvars[0]);
#যদি defined(FEAT_JOB_CHANNEL)
    যদি (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#endif

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", বাৰ্তা);
}

/*
 * "ch_logfile()" function
 */
    শূন্য
f_ch_logfile(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    যদি (check_restricted() || check_secure())
	ঘূৰাই;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    fname = tv_get_string(&argvars[0]);
    যদি (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#endif // FEAT_EVAL
