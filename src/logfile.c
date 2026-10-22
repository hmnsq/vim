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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

// Log file opened with ch_logfile().
ସ୍ଥିର FILE *log_fd = ଖାଲି;
ସ୍ଥିର char_u *log_name = ଖାଲି;
#ifdef FEAT_RELTIME
ସ୍ଥିର proftime_T log_start;
#endif

    ଶୂନ୍ୟ
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = ଖାଲି;
    ଅକ୍ଷର	*mode = "a";

    ଯଦି (log_fd != ଖାଲି)
    {
	ଯଦି (*fname != NUL)
	    ch_log(ଖାଲି, "closing this logfile, opening %s", fname);
	ଅନ୍ୟଥା
	    ch_log(ଖାଲି, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    ଯଦି (vim_strchr(opt, 'a') == ଖାଲି && vim_strchr(opt, 'w') != ଖାଲି)
	mode = "w";
    ch_log_output = vim_strchr(opt, 'o') != ଖାଲି ? LOG_ALWAYS : FALSE;

    ଯଦି (*fname != NUL)
    {
	file = fopen((ଅକ୍ଷର *)fname, mode);
	ଯଦି (file == ଖାଲି)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ଫେରନ୍ତୁ;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    ଯଦି (log_fd != ଖାଲି)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(time(ଖାଲି), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#ifdef FEAT_RELTIME
	profile_start(&log_start);
#endif
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ch_log_active(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ log_fd != ଖାଲି;
}

    ସ୍ଥିର ଶୂନ୍ୟ
ch_log_lead(ଅଚଳ ଅକ୍ଷର *what, channel_T *ch UNUSED, ch_part_T part UNUSED)
{
    ଯଦି (log_fd == ଖାଲି)
	ଫେରନ୍ତୁ;

#ifdef FEAT_RELTIME
    proftime_T log_now;

    profile_start(&log_now);
    profile_sub(&log_now, &log_start);
    fprintf(log_fd, "%s ", profile_ସନ୍ଦେଶ(&log_now));
#endif
#ifdef FEAT_JOB_CHANNEL
    ଯଦି (ch != ଖାଲି)
    {
	ଯଦି (part < PART_ଗଣନା)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_names[part]);
	ଅନ୍ୟଥା
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    ଅନ୍ୟଥା
#endif
	fprintf(log_fd, "%s: ", what);
}

#ifndef PROTO  // prototype is in proto.h
    ଶୂନ୍ୟ
ch_log(channel_T *ch, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
    ଯଦି (log_fd == ଖାଲି)
	ଫେରନ୍ତୁ;

    va_list ap;

    ch_log_lead("", ch, PART_ଗଣନା);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_ସନ୍ଦେଶ = 0;
}

    ଶୂନ୍ୟ
ch_error(channel_T *ch, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
    ଯଦି (log_fd == ଖାଲି)
	ଫେରନ୍ତୁ;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_ଗଣନା);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_ସନ୍ଦେଶ = 0;
}
#endif

#ଯଦି defined(FEAT_JOB_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    ଶୂନ୍ୟ
ch_log_literal(
	ଅକ୍ଷର	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#endif

/*
 * "ch_log()" function
 */
    ଶୂନ୍ୟ
f_ch_log(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*ସନ୍ଦେଶ;
    channel_T	*channel = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_job_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ସନ୍ଦେଶ = tv_get_string(&argvars[0]);
#ଯଦି defined(FEAT_JOB_CHANNEL)
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#endif

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", ସନ୍ଦେଶ);
}

/*
 * "ch_logfile()" function
 */
    ଶୂନ୍ୟ
f_ch_logfile(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    fname = tv_get_string(&argvars[0]);
    ଯଦି (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#endif // FEAT_EVAL
