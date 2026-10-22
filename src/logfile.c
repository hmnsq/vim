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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

// Log file opened with ch_logfile().
ಸ್ಥಿರ FILE *log_fd = ರಿಕ್ತ;
ಸ್ಥಿರ char_u *log_name = ರಿಕ್ತ;
#ifdef FEAT_RELTIME
ಸ್ಥಿರ proftime_T log_start;
#endif

    ಶೂನ್ಯ
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = ರಿಕ್ತ;
    ವರ್ಣ	*mode = "a";

    ಯದಿ (log_fd != ರಿಕ್ತ)
    {
	ಯದಿ (*fname != NUL)
	    ch_log(ರಿಕ್ತ, "closing this logfile, opening %s", fname);
	ಅನ್ಯಥಾ
	    ch_log(ರಿಕ್ತ, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    ಯದಿ (vim_strchr(opt, 'a') == ರಿಕ್ತ && vim_strchr(opt, 'w') != ರಿಕ್ತ)
	mode = "w";
    ch_log_output = vim_strchr(opt, 'o') != ರಿಕ್ತ ? LOG_ALWAYS : FALSE;

    ಯದಿ (*fname != NUL)
    {
	file = fopen((ವರ್ಣ *)fname, mode);
	ಯದಿ (file == ರಿಕ್ತ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    ಯದಿ (log_fd != ರಿಕ್ತ)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(time(ರಿಕ್ತ), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#ifdef FEAT_RELTIME
	profile_start(&log_start);
#endif
    }
}

    ಪೂರ್ಣಾಙ್ಕ
ch_log_active(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ log_fd != ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ch_log_lead(ಧ್ರುವ ವರ್ಣ *what, channel_T *ch UNUSED, ch_part_T part UNUSED)
{
    ಯದಿ (log_fd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_RELTIME
    proftime_T log_now;

    profile_start(&log_now);
    profile_sub(&log_now, &log_start);
    fprintf(log_fd, "%s ", profile_ಸಂದೇಶ(&log_now));
#endif
#ifdef FEAT_JOB_CHANNEL
    ಯದಿ (ch != ರಿಕ್ತ)
    {
	ಯದಿ (part < PART_ಎಣಿಕೆ)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_names[part]);
	ಅನ್ಯಥಾ
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    ಅನ್ಯಥಾ
#endif
	fprintf(log_fd, "%s: ", what);
}

#ifndef PROTO  // prototype is in proto.h
    ಶೂನ್ಯ
ch_log(channel_T *ch, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
    ಯದಿ (log_fd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    va_list ap;

    ch_log_lead("", ch, PART_ಎಣಿಕೆ);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_ಸಂದೇಶ = 0;
}

    ಶೂನ್ಯ
ch_error(channel_T *ch, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
    ಯದಿ (log_fd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_ಎಣಿಕೆ);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_ಸಂದೇಶ = 0;
}
#endif

#ಯದಿ defined(FEAT_JOB_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    ಶೂನ್ಯ
ch_log_literal(
	ವರ್ಣ	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	ಪೂರ್ಣಾಙ್ಕ	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (ಪೂರ್ಣಾಙ್ಕ)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#endif

/*
 * "ch_log()" function
 */
    ಶೂನ್ಯ
f_ch_log(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*ಸಂದೇಶ;
    channel_T	*channel = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_job_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಸಂದೇಶ = tv_get_string(&argvars[0]);
#ಯದಿ defined(FEAT_JOB_CHANNEL)
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#endif

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", ಸಂದೇಶ);
}

/*
 * "ch_logfile()" function
 */
    ಶೂನ್ಯ
f_ch_logfile(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    fname = tv_get_string(&argvars[0]);
    ಯದಿ (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#endif // FEAT_EVAL
