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

#ಆದರೆ defined(FEAT_EVAL)

// Log file opened with ch_logfile().
ಸ್ಥಿರ FILE *log_fd = ಖಾಲಿ;
ಸ್ಥಿರ char_u *log_name = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
ಸ್ಥಿರ proftime_T log_ಆರಂಭ;
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = ಖಾಲಿ;
    ಅಕ್ಷರ	*ಬಹುಲಕ = "a";

    ಆದರೆ (log_fd != ಖಾಲಿ)
    {
	ಆದರೆ (*fname != NUL)
	    ch_log(ಖಾಲಿ, "closing this logfile, opening %s", fname);
	ಇಲ್ಲದಿದ್ದರೆ
	    ch_log(ಖಾಲಿ, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    ಆದರೆ (vim_strchr(opt, 'a') == ಖಾಲಿ && vim_strchr(opt, 'w') != ಖಾಲಿ)
	ಬಹುಲಕ = "w";
    ch_log_output = vim_strchr(opt, 'o') != ಖಾಲಿ ? LOG_ALWAYS : FALSE;

    ಆದರೆ (*fname != NUL)
    {
	file = fopen((ಅಕ್ಷರ *)fname, ಬಹುಲಕ);
	ಆದರೆ (file == ಖಾಲಿ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಹಿಂತಿರುಗಿ;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    ಆದರೆ (log_fd != ಖಾಲಿ)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(ಸಮಯ(ಖಾಲಿ), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	profile_ಆರಂಭ(&log_ಆರಂಭ);
#ಆದರೆ_ಅಂತ್ಯ
    }
}

    ಪೂರ್ಣಾಂಕ
ch_log_ಸಕ್ರಿಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ log_fd != ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ch_log_lead(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *what, channel_T *ch ಬಳಸದ, ch_part_T part ಬಳಸದ)
{
    ಆದರೆ (log_fd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    proftime_T log_now;

    profile_ಆರಂಭ(&log_now);
    profile_sub(&log_now, &log_ಆರಂಭ);
    fprintf(log_fd, "%s ", profile_ಸಂದೇಶ(&log_now));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (ch != ಖಾಲಿ)
    {
	ಆದರೆ (part < PART_ಎಣಿಕೆ)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_ಹೆಸರುಗಳು[part]);
	ಇಲ್ಲದಿದ್ದರೆ
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	fprintf(log_fd, "%s: ", what);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO  // prototype is in proto.h
    ಶೂನ್ಯ
ch_log(channel_T *ch, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
    ಆದರೆ (log_fd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

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
ch_error(channel_T *ch, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
    ಆದರೆ (log_fd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_ಎಣಿಕೆ);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_ಸಂದೇಶ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    ಶೂನ್ಯ
ch_log_literal(
	ಅಕ್ಷರ	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	ಪೂರ್ಣಾಂಕ	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (ಪೂರ್ಣಾಂಕ)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * "ch_log()" function
 */
    ಶೂನ್ಯ
f_ch_log(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    char_u	*ಸಂದೇಶ;
    channel_T	*channel = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_ಕೆಲಸ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಸಂದೇಶ = tv_get_string(&argvars[0]);
#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#ಆದರೆ_ಅಂತ್ಯ

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", ಸಂದೇಶ);
}

/*
 * "ch_logfile()" function
 */
    ಶೂನ್ಯ
f_ch_logfile(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    fname = tv_get_string(&argvars[0]);
    ಆದರೆ (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
