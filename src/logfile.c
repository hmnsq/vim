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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

// Log file opened with ch_logfile().
స్థిర FILE *log_fd = ఖాళీ;
స్థిర char_u *log_name = ఖాళీ;
#నిర్వచితమైతే FEAT_RELTIME
స్థిర proftime_T log_ప్రారంభం;
#అయితే_ముగింపు

    శూన్య
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = ఖాళీ;
    అక్షరం	*బహుళకం = "a";

    ఒకవేళ (log_fd != ఖాళీ)
    {
	ఒకవేళ (*fname != NUL)
	    ch_log(ఖాళీ, "closing this logfile, opening %s", fname);
	లేకపోతే
	    ch_log(ఖాళీ, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    ఒకవేళ (vim_strchr(opt, 'a') == ఖాళీ && vim_strchr(opt, 'w') != ఖాళీ)
	బహుళకం = "w";
    ch_log_output = vim_strchr(opt, 'o') != ఖాళీ ? LOG_ALWAYS : FALSE;

    ఒకవేళ (*fname != NUL)
    {
	file = fopen((అక్షరం *)fname, బహుళకం);
	ఒకవేళ (file == ఖాళీ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    తిరిగి;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    ఒకవేళ (log_fd != ఖాళీ)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(సమయం(ఖాళీ), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#నిర్వచితమైతే FEAT_RELTIME
	profile_ప్రారంభం(&log_ప్రారంభం);
#అయితే_ముగింపు
    }
}

    పూర్ణాంకం
ch_log_చురుకైన(శూన్య)
{
    తిరిగి log_fd != ఖాళీ;
}

    స్థిర శూన్య
ch_log_lead(స్థిరరాశి అక్షరం *what, channel_T *ch వాడని, ch_part_T part వాడని)
{
    ఒకవేళ (log_fd == ఖాళీ)
	తిరిగి;

#నిర్వచితమైతే FEAT_RELTIME
    proftime_T log_now;

    profile_ప్రారంభం(&log_now);
    profile_sub(&log_now, &log_ప్రారంభం);
    fprintf(log_fd, "%s ", profile_సందేశం(&log_now));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
    ఒకవేళ (ch != ఖాళీ)
    {
	ఒకవేళ (part < PART_లెక్క)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_పేర్లు[part]);
	లేకపోతే
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    లేకపోతే
#అయితే_ముగింపు
	fprintf(log_fd, "%s: ", what);
}

#నిర్వచితం_కాకపోతే PROTO  // prototype is in proto.h
    శూన్య
ch_log(channel_T *ch, స్థిరరాశి అక్షరం *fmt, ...)
{
    ఒకవేళ (log_fd == ఖాళీ)
	తిరిగి;

    va_list ap;

    ch_log_lead("", ch, PART_లెక్క);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_సందేశం = 0;
}

    శూన్య
ch_error(channel_T *ch, స్థిరరాశి అక్షరం *fmt, ...)
{
    ఒకవేళ (log_fd == ఖాళీ)
	తిరిగి;

    va_list ap;

    ch_log_lead("ERR ", ch, PART_లెక్క);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_సందేశం = 0;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_పని_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    శూన్య
ch_log_literal(
	అక్షరం	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	పూర్ణాంకం	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (పూర్ణాంకం)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#అయితే_ముగింపు

/*
 * "ch_log()" function
 */
    శూన్య
f_ch_log(typval_T *argvars, typval_T *rettv వాడని)
{
    char_u	*సందేశం;
    channel_T	*channel = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_పని_arg(argvars, 1) == FAIL))
	తిరిగి;

    సందేశం = tv_get_string(&argvars[0]);
#ఒకవేళ defined(FEAT_పని_CHANNEL)
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#అయితే_ముగింపు

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", సందేశం);
}

/*
 * "ch_logfile()" function
 */
    శూన్య
f_ch_logfile(typval_T *argvars, typval_T *rettv వాడని)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    fname = tv_get_string(&argvars[0]);
    ఒకవేళ (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#అయితే_ముగింపు // FEAT_EVAL
