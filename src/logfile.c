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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

// Log file opened with ch_logfile().
સ્થિર FILE *log_fd = ખાલી;
સ્થિર char_u *log_name = ખાલી;
#ifdef FEAT_RELTIME
સ્થિર proftime_T log_start;
#endif

    શૂન્ય
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = ખાલી;
    અક્ષર	*mode = "a";

    જો (log_fd != ખાલી)
    {
	જો (*fname != NUL)
	    ch_log(ખાલી, "closing this logfile, opening %s", fname);
	નહીંતર
	    ch_log(ખાલી, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    જો (vim_strchr(opt, 'a') == ખાલી && vim_strchr(opt, 'w') != ખાલી)
	mode = "w";
    ch_log_output = vim_strchr(opt, 'o') != ખાલી ? LOG_ALWAYS : FALSE;

    જો (*fname != NUL)
    {
	file = fopen((અક્ષર *)fname, mode);
	જો (file == ખાલી)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    પાછા;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    જો (log_fd != ખાલી)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(time(ખાલી), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#ifdef FEAT_RELTIME
	profile_start(&log_start);
#endif
    }
}

    પૂર્ણાંક
ch_log_active(શૂન્ય)
{
    પાછા log_fd != ખાલી;
}

    સ્થિર શૂન્ય
ch_log_lead(અચલ અક્ષર *what, channel_T *ch UNUSED, ch_part_T part UNUSED)
{
    જો (log_fd == ખાલી)
	પાછા;

#ifdef FEAT_RELTIME
    proftime_T log_now;

    profile_start(&log_now);
    profile_sub(&log_now, &log_start);
    fprintf(log_fd, "%s ", profile_સંદેશ(&log_now));
#endif
#ifdef FEAT_JOB_CHANNEL
    જો (ch != ખાલી)
    {
	જો (part < PART_ગણતરી)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_names[part]);
	નહીંતર
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    નહીંતર
#endif
	fprintf(log_fd, "%s: ", what);
}

#ifndef PROTO  // prototype is in proto.h
    શૂન્ય
ch_log(channel_T *ch, અચલ અક્ષર *fmt, ...)
{
    જો (log_fd == ખાલી)
	પાછા;

    va_list ap;

    ch_log_lead("", ch, PART_ગણતરી);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_સંદેશ = 0;
}

    શૂન્ય
ch_error(channel_T *ch, અચલ અક્ષર *fmt, ...)
{
    જો (log_fd == ખાલી)
	પાછા;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_ગણતરી);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_સંદેશ = 0;
}
#endif

#જો defined(FEAT_JOB_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    શૂન્ય
ch_log_literal(
	અક્ષર	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	પૂર્ણાંક	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (પૂર્ણાંક)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#endif

/*
 * "ch_log()" function
 */
    શૂન્ય
f_ch_log(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*સંદેશ;
    channel_T	*channel = ખાલી;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_job_arg(argvars, 1) == FAIL))
	પાછા;

    સંદેશ = tv_get_string(&argvars[0]);
#જો defined(FEAT_JOB_CHANNEL)
    જો (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#endif

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", સંદેશ);
}

/*
 * "ch_logfile()" function
 */
    શૂન્ય
f_ch_logfile(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	પાછા;

    fname = tv_get_string(&argvars[0]);
    જો (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#endif // FEAT_EVAL
