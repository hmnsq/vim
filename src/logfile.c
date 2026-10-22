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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

// Log file opened with ch_logfile().
நிலையான FILE *log_fd = வெற்று;
நிலையான char_u *log_name = வெற்று;
#ifdef FEAT_RELTIME
நிலையான proftime_T log_start;
#endif

    வெறுமை
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = வெற்று;
    எழுத்து	*mode = "a";

    எனில் (log_fd != வெற்று)
    {
	எனில் (*fname != NUL)
	    ch_log(வெற்று, "closing this logfile, opening %s", fname);
	இல்லையெனில்
	    ch_log(வெற்று, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    எனில் (vim_strchr(opt, 'a') == வெற்று && vim_strchr(opt, 'w') != வெற்று)
	mode = "w";
    ch_log_output = vim_strchr(opt, 'o') != வெற்று ? LOG_ALWAYS : FALSE;

    எனில் (*fname != NUL)
    {
	file = fopen((எழுத்து *)fname, mode);
	எனில் (file == வெற்று)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    திரும்பு;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    எனில் (log_fd != வெற்று)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(time(வெற்று), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#ifdef FEAT_RELTIME
	profile_start(&log_start);
#endif
    }
}

    முழு
ch_log_active(வெறுமை)
{
    திரும்பு log_fd != வெற்று;
}

    நிலையான வெறுமை
ch_log_lead(மாறிலி எழுத்து *what, channel_T *ch UNUSED, ch_part_T part UNUSED)
{
    எனில் (log_fd == வெற்று)
	திரும்பு;

#ifdef FEAT_RELTIME
    proftime_T log_now;

    profile_start(&log_now);
    profile_sub(&log_now, &log_start);
    fprintf(log_fd, "%s ", profile_செய்தி(&log_now));
#endif
#ifdef FEAT_JOB_CHANNEL
    எனில் (ch != வெற்று)
    {
	எனில் (part < PART_மொத்தம்)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_names[part]);
	இல்லையெனில்
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    இல்லையெனில்
#endif
	fprintf(log_fd, "%s: ", what);
}

#ifndef PROTO  // prototype is in proto.h
    வெறுமை
ch_log(channel_T *ch, மாறிலி எழுத்து *fmt, ...)
{
    எனில் (log_fd == வெற்று)
	திரும்பு;

    va_list ap;

    ch_log_lead("", ch, PART_மொத்தம்);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_செய்தி = 0;
}

    வெறுமை
ch_error(channel_T *ch, மாறிலி எழுத்து *fmt, ...)
{
    எனில் (log_fd == வெற்று)
	திரும்பு;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_மொத்தம்);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_செய்தி = 0;
}
#endif

#எனில் defined(FEAT_JOB_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    வெறுமை
ch_log_literal(
	எழுத்து	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	முழு	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (முழு)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#endif

/*
 * "ch_log()" function
 */
    வெறுமை
f_ch_log(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*செய்தி;
    channel_T	*channel = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_job_arg(argvars, 1) == FAIL))
	திரும்பு;

    செய்தி = tv_get_string(&argvars[0]);
#எனில் defined(FEAT_JOB_CHANNEL)
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#endif

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", செய்தி);
}

/*
 * "ch_logfile()" function
 */
    வெறுமை
f_ch_logfile(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    fname = tv_get_string(&argvars[0]);
    எனில் (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#endif // FEAT_EVAL
