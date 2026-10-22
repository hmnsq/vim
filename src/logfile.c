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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

// Log file opened with ch_logfile().
ساکن FILE *log_fd = تہی;
ساکن char_u *log_name = تہی;
#اگر_تعریف_شدہ FEAT_RELTIME
ساکن proftime_T log_آغاز;
#اگر_ختم

    خالی
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = تہی;
    حرف	*منوال = "a";

    اگر (log_fd != تہی)
    {
	اگر (*fname != NUL)
	    ch_log(تہی, "closing this logfile, opening %s", fname);
	ورنہ
	    ch_log(تہی, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    اگر (vim_strchr(opt, 'a') == تہی && vim_strchr(opt, 'w') != تہی)
	منوال = "w";
    ch_log_output = vim_strchr(opt, 'o') != تہی ? LOG_ALWAYS : FALSE;

    اگر (*fname != NUL)
    {
	file = fopen((حرف *)fname, منوال);
	اگر (file == تہی)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    واپس;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    اگر (log_fd != تہی)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(وقت(تہی), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#اگر_تعریف_شدہ FEAT_RELTIME
	profile_آغاز(&log_آغاز);
#اگر_ختم
    }
}

    صحیح
ch_log_فعال(خالی)
{
    واپس log_fd != تہی;
}

    ساکن خالی
ch_log_lead(ثابت حرف *what, channel_T *ch غیر_مستعمل, ch_part_T part غیر_مستعمل)
{
    اگر (log_fd == تہی)
	واپس;

#اگر_تعریف_شدہ FEAT_RELTIME
    proftime_T log_now;

    profile_آغاز(&log_now);
    profile_sub(&log_now, &log_آغاز);
    fprintf(log_fd, "%s ", profile_پیغام(&log_now));
#اگر_ختم
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    اگر (ch != تہی)
    {
	اگر (part < PART_گنتی)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_نام[part]);
	ورنہ
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    ورنہ
#اگر_ختم
	fprintf(log_fd, "%s: ", what);
}

#اگر_غیر_تعریف_شدہ PROTO  // prototype is in proto.h
    خالی
ch_log(channel_T *ch, ثابت حرف *fmt, ...)
{
    اگر (log_fd == تہی)
	واپس;

    va_list ap;

    ch_log_lead("", ch, PART_گنتی);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_پیغام = 0;
}

    خالی
ch_error(channel_T *ch, ثابت حرف *fmt, ...)
{
    اگر (log_fd == تہی)
	واپس;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_گنتی);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_پیغام = 0;
}
#اگر_ختم

#اگر defined(FEAT_کام_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    خالی
ch_log_literal(
	حرف	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	صحیح	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (صحیح)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#اگر_ختم

/*
 * "ch_log()" function
 */
    خالی
f_ch_log(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    char_u	*پیغام;
    channel_T	*channel = تہی;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_کام_arg(argvars, 1) == FAIL))
	واپس;

    پیغام = tv_get_string(&argvars[0]);
#اگر defined(FEAT_کام_CHANNEL)
    اگر (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#اگر_ختم

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", پیغام);
}

/*
 * "ch_logfile()" function
 */
    خالی
f_ch_logfile(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	واپس;

    fname = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#اگر_ختم // FEAT_EVAL
