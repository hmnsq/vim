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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

// Log file opened with ch_logfile().
ਸਥਿਰ FILE *log_fd = ਨਿਰਾ;
ਸਥਿਰ char_u *log_name = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
ਸਥਿਰ proftime_T log_ਸ਼ੁਰੂ;
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = ਨਿਰਾ;
    ਅੱਖਰ	*ਬਹੁਲਕ = "a";

    ਜੇ (log_fd != ਨਿਰਾ)
    {
	ਜੇ (*fname != NUL)
	    ch_log(ਨਿਰਾ, "closing this logfile, opening %s", fname);
	ਨਹੀਂਤਾਂ
	    ch_log(ਨਿਰਾ, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    ਜੇ (vim_strchr(opt, 'a') == ਨਿਰਾ && vim_strchr(opt, 'w') != ਨਿਰਾ)
	ਬਹੁਲਕ = "w";
    ch_log_output = vim_strchr(opt, 'o') != ਨਿਰਾ ? LOG_ALWAYS : FALSE;

    ਜੇ (*fname != NUL)
    {
	file = fopen((ਅੱਖਰ *)fname, ਬਹੁਲਕ);
	ਜੇ (file == ਨਿਰਾ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ਵਾਪਸ;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    ਜੇ (log_fd != ਨਿਰਾ)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(ਸਮਾਂ(ਨਿਰਾ), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	profile_ਸ਼ੁਰੂ(&log_ਸ਼ੁਰੂ);
#ਜੇ_ਅੰਤ
    }
}

    ਪੂਰਨ
ch_log_ਸਰਗਰਮ(ਖਾਲੀ)
{
    ਵਾਪਸ log_fd != ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
ch_log_lead(ਅਚਲ ਅੱਖਰ *what, channel_T *ch ਅਣਵਰਤਿਆ, ch_part_T part ਅਣਵਰਤਿਆ)
{
    ਜੇ (log_fd == ਨਿਰਾ)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    proftime_T log_now;

    profile_ਸ਼ੁਰੂ(&log_now);
    profile_sub(&log_now, &log_ਸ਼ੁਰੂ);
    fprintf(log_fd, "%s ", profile_ਸੁਨੇਹਾ(&log_now));
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਜੇ (ch != ਨਿਰਾ)
    {
	ਜੇ (part < PART_ਗਿਣਤੀ)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_ਨਾਂ[part]);
	ਨਹੀਂਤਾਂ
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	fprintf(log_fd, "%s: ", what);
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO  // prototype is in proto.h
    ਖਾਲੀ
ch_log(channel_T *ch, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
    ਜੇ (log_fd == ਨਿਰਾ)
	ਵਾਪਸ;

    va_list ap;

    ch_log_lead("", ch, PART_ਗਿਣਤੀ);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_ਸੁਨੇਹਾ = 0;
}

    ਖਾਲੀ
ch_error(channel_T *ch, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
    ਜੇ (log_fd == ਨਿਰਾ)
	ਵਾਪਸ;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_ਗਿਣਤੀ);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_ਸੁਨੇਹਾ = 0;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    ਖਾਲੀ
ch_log_literal(
	ਅੱਖਰ	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	ਪੂਰਨ	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (ਪੂਰਨ)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#ਜੇ_ਅੰਤ

/*
 * "ch_log()" function
 */
    ਖਾਲੀ
f_ch_log(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    char_u	*ਸੁਨੇਹਾ;
    channel_T	*channel = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_ਕੰਮ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਸੁਨੇਹਾ = tv_get_string(&argvars[0]);
#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#ਜੇ_ਅੰਤ

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", ਸੁਨੇਹਾ);
}

/*
 * "ch_logfile()" function
 */
    ਖਾਲੀ
f_ch_logfile(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    fname = tv_get_string(&argvars[0]);
    ਜੇ (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
