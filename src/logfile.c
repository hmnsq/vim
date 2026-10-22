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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

// Log file opened with ch_logfile().
സ്ഥിതം FILE *log_fd = ശൂന്യ;
സ്ഥിതം char_u *log_name = ശൂന്യ;
#ifdef FEAT_RELTIME
സ്ഥിതം proftime_T log_start;
#endif

    ശൂന്യം
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = ശൂന്യ;
    അക്ഷരം	*mode = "a";

    എങ്കിൽ (log_fd != ശൂന്യ)
    {
	എങ്കിൽ (*fname != NUL)
	    ch_log(ശൂന്യ, "closing this logfile, opening %s", fname);
	അല്ലെങ്കിൽ
	    ch_log(ശൂന്യ, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    എങ്കിൽ (vim_strchr(opt, 'a') == ശൂന്യ && vim_strchr(opt, 'w') != ശൂന്യ)
	mode = "w";
    ch_log_output = vim_strchr(opt, 'o') != ശൂന്യ ? LOG_ALWAYS : FALSE;

    എങ്കിൽ (*fname != NUL)
    {
	file = fopen((അക്ഷരം *)fname, mode);
	എങ്കിൽ (file == ശൂന്യ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    തിരികെ;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    എങ്കിൽ (log_fd != ശൂന്യ)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(time(ശൂന്യ), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#ifdef FEAT_RELTIME
	profile_start(&log_start);
#endif
    }
}

    പൂർണ്ണസംഖ്യ
ch_log_active(ശൂന്യം)
{
    തിരികെ log_fd != ശൂന്യ;
}

    സ്ഥിതം ശൂന്യം
ch_log_lead(സ്ഥിരം അക്ഷരം *what, channel_T *ch UNUSED, ch_part_T part UNUSED)
{
    എങ്കിൽ (log_fd == ശൂന്യ)
	തിരികെ;

#ifdef FEAT_RELTIME
    proftime_T log_now;

    profile_start(&log_now);
    profile_sub(&log_now, &log_start);
    fprintf(log_fd, "%s ", profile_സന്ദേശം(&log_now));
#endif
#ifdef FEAT_JOB_CHANNEL
    എങ്കിൽ (ch != ശൂന്യ)
    {
	എങ്കിൽ (part < PART_എണ്ണം)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_names[part]);
	അല്ലെങ്കിൽ
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    അല്ലെങ്കിൽ
#endif
	fprintf(log_fd, "%s: ", what);
}

#ifndef PROTO  // prototype is in proto.h
    ശൂന്യം
ch_log(channel_T *ch, സ്ഥിരം അക്ഷരം *fmt, ...)
{
    എങ്കിൽ (log_fd == ശൂന്യ)
	തിരികെ;

    va_list ap;

    ch_log_lead("", ch, PART_എണ്ണം);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_സന്ദേശം = 0;
}

    ശൂന്യം
ch_error(channel_T *ch, സ്ഥിരം അക്ഷരം *fmt, ...)
{
    എങ്കിൽ (log_fd == ശൂന്യ)
	തിരികെ;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_എണ്ണം);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_സന്ദേശം = 0;
}
#endif

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    ശൂന്യം
ch_log_literal(
	അക്ഷരം	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	പൂർണ്ണസംഖ്യ	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (പൂർണ്ണസംഖ്യ)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#endif

/*
 * "ch_log()" function
 */
    ശൂന്യം
f_ch_log(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*സന്ദേശം;
    channel_T	*channel = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_job_arg(argvars, 1) == FAIL))
	തിരികെ;

    സന്ദേശം = tv_get_string(&argvars[0]);
#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#endif

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", സന്ദേശം);
}

/*
 * "ch_logfile()" function
 */
    ശൂന്യം
f_ch_logfile(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    fname = tv_get_string(&argvars[0]);
    എങ്കിൽ (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#endif // FEAT_EVAL
