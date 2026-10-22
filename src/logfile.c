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

#ضمن "vim.h"

#𐡄𐡍 defined(FEAT_EVAL)

// Log file opened with ch_logfile().
ساكن FILE *log_fd = عدم;
ساكن char_u *log_name = عدم;
#ifdef FEAT_RELTIME
ساكن proftime_T log_start;
#endif

    فراغ
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = عدم;
    حرف	*mode = "a";

    𐡄𐡍 (log_fd != عدم)
    {
	𐡄𐡍 (*fname != NUL)
	    ch_log(عدم, "closing this logfile, opening %s", fname);
	وإلا
	    ch_log(عدم, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    𐡄𐡍 (vim_strchr(opt, 'a') == عدم && vim_strchr(opt, 'w') != عدم)
	mode = "w";
    ch_log_output = vim_strchr(opt, 'o') != عدم ? LOG_ALWAYS : FALSE;

    𐡄𐡍 (*fname != NUL)
    {
	file = fopen((حرف *)fname, mode);
	𐡄𐡍 (file == عدم)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    أرجع;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    𐡄𐡍 (log_fd != عدم)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(time(عدم), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#ifdef FEAT_RELTIME
	profile_start(&log_start);
#endif
    }
}

    صحيح
ch_log_active(فراغ)
{
    أرجع log_fd != عدم;
}

    ساكن فراغ
ch_log_lead(ثابت حرف *what, channel_T *ch UNUSED, ch_part_T part UNUSED)
{
    𐡄𐡍 (log_fd == عدم)
	أرجع;

#ifdef FEAT_RELTIME
    proftime_T log_now;

    profile_start(&log_now);
    profile_sub(&log_now, &log_start);
    fprintf(log_fd, "%s ", profile_msg(&log_now));
#endif
#ifdef FEAT_JOB_CHANNEL
    𐡄𐡍 (ch != عدم)
    {
	𐡄𐡍 (part < PART_COUNT)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_names[part]);
	وإلا
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    وإلا
#endif
	fprintf(log_fd, "%s: ", what);
}

#ifndef PROTO  // prototype is in proto.h
    فراغ
ch_log(channel_T *ch, ثابت حرف *fmt, ...)
{
    𐡄𐡍 (log_fd == عدم)
	أرجع;

    va_list ap;

    ch_log_lead("", ch, PART_COUNT);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_msg = 0;
}

    فراغ
ch_error(channel_T *ch, ثابت حرف *fmt, ...)
{
    𐡄𐡍 (log_fd == عدم)
	أرجع;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_COUNT);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_msg = 0;
}
#endif

#𐡄𐡍 defined(FEAT_JOB_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    فراغ
ch_log_literal(
	حرف	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	صحيح	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (صحيح)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#endif

/*
 * "ch_log()" function
 */
    فراغ
f_ch_log(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*msg;
    channel_T	*channel = عدم;

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_job_arg(argvars, 1) == FAIL))
	أرجع;

    msg = tv_get_string(&argvars[0]);
#𐡄𐡍 defined(FEAT_JOB_CHANNEL)
    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#endif

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", msg);
}

/*
 * "ch_logfile()" function
 */
    فراغ
f_ch_logfile(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    𐡄𐡍 (check_restricted() || check_secure())
	أرجع;

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    fname = tv_get_string(&argvars[0]);
    𐡄𐡍 (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#endif // FEAT_EVAL
