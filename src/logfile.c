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

#إذا defined(FEAT_EVAL)

// Log file opened with ch_logfile().
ساكن FILE *log_fd = عدم;
ساكن char_u *log_name = عدم;
#جے_تعریف_شدہ FEAT_RELTIME
ساكن proftime_T log_شروع;
#جے_انت

    فراغ
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = عدم;
    حرف	*منوال = "a";

    إذا (log_fd != عدم)
    {
	إذا (*fname != NUL)
	    ch_log(عدم, "closing this logfile, opening %s", fname);
	وإلا
	    ch_log(عدم, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    إذا (vim_strchr(opt, 'a') == عدم && vim_strchr(opt, 'w') != عدم)
	منوال = "w";
    ch_log_output = vim_strchr(opt, 'o') != عدم ? LOG_ALWAYS : FALSE;

    إذا (*fname != NUL)
    {
	file = fopen((حرف *)fname, منوال);
	إذا (file == عدم)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    أرجع;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    إذا (log_fd != عدم)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(ویلا(عدم), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#جے_تعریف_شدہ FEAT_RELTIME
	profile_شروع(&log_شروع);
#جے_انت
    }
}

    صحيح
ch_log_سرگرم(فراغ)
{
    أرجع log_fd != عدم;
}

    ساكن فراغ
ch_log_lead(ثابت حرف *what, channel_T *ch بن_ورتیا, ch_part_T part بن_ورتیا)
{
    إذا (log_fd == عدم)
	أرجع;

#جے_تعریف_شدہ FEAT_RELTIME
    proftime_T log_now;

    profile_شروع(&log_now);
    profile_sub(&log_now, &log_شروع);
    fprintf(log_fd, "%s ", profile_پیغام(&log_now));
#جے_انت
#جے_تعریف_شدہ FEAT_کم_CHANNEL
    إذا (ch != عدم)
    {
	إذا (part < PART_گنتی)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_ناں[part]);
	وإلا
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    وإلا
#جے_انت
	fprintf(log_fd, "%s: ", what);
}

#جے_بنا_تعریف PROTO  // prototype is in proto.h
    فراغ
ch_log(channel_T *ch, ثابت حرف *fmt, ...)
{
    إذا (log_fd == عدم)
	أرجع;

    va_list ap;

    ch_log_lead("", ch, PART_گنتی);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_پیغام = 0;
}

    فراغ
ch_error(channel_T *ch, ثابت حرف *fmt, ...)
{
    إذا (log_fd == عدم)
	أرجع;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_گنتی);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_پیغام = 0;
}
#جے_انت

#إذا defined(FEAT_کم_CHANNEL)
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
#جے_انت

/*
 * "ch_log()" function
 */
    فراغ
f_ch_log(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    char_u	*پیغام;
    channel_T	*channel = عدم;

    إذا (وچ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_کم_arg(argvars, 1) == FAIL))
	أرجع;

    پیغام = tv_get_string(&argvars[0]);
#إذا defined(FEAT_کم_CHANNEL)
    إذا (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#جے_انت

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", پیغام);
}

/*
 * "ch_logfile()" function
 */
    فراغ
f_ch_logfile(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (وچ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    fname = tv_get_string(&argvars[0]);
    إذا (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#جے_انت // FEAT_EVAL
