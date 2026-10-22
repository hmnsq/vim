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

#include "vim.h"

#만약 defined(FEAT_EVAL)

// Log file opened with ch_logfile().
static FILE *log_fd = NULL;
static char_u *log_name = NULL;
#ifdef FEAT_RELTIME
static proftime_T log_start;
#endif

    void
ch_logfile(char_u *fname, char_u *opt)
{
    FILE	*file = NULL;
    char	*mode = "a";

    만약 (log_fd != NULL)
    {
	만약 (*fname != NUL)
	    ch_log(NULL, "closing this logfile, opening %s", fname);
	아니면
	    ch_log(NULL, "closing logfile %s", log_name);
	fclose(log_fd);
    }

    // The "a" flag overrules the "w" flag.
    만약 (vim_strchr(opt, 'a') == NULL && vim_strchr(opt, 'w') != NULL)
	mode = "w";
    ch_log_output = vim_strchr(opt, 'o') != NULL ? LOG_ALWAYS : FALSE;

    만약 (*fname != NUL)
    {
	file = fopen((char *)fname, mode);
	만약 (file == NULL)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    반환;
	}
	vim_free(log_name);
	log_name = vim_strsave(fname);
    }
    log_fd = file;

    만약 (log_fd != NULL)
    {
	fprintf(log_fd, "==== start log session %s ====\n",
						 get_ctime(time(NULL), FALSE));
	// flush now, if fork/exec follows it could be written twice
	fflush(log_fd);
#ifdef FEAT_RELTIME
	profile_start(&log_start);
#endif
    }
}

    정수
ch_log_active(void)
{
    반환 log_fd != NULL;
}

    static void
ch_log_lead(const char *what, channel_T *ch UNUSED, ch_part_T part UNUSED)
{
    만약 (log_fd == NULL)
	반환;

#ifdef FEAT_RELTIME
    proftime_T log_now;

    profile_start(&log_now);
    profile_sub(&log_now, &log_start);
    fprintf(log_fd, "%s ", profile_msg(&log_now));
#endif
#ifdef FEAT_JOB_CHANNEL
    만약 (ch != NULL)
    {
	만약 (part < PART_COUNT)
	    fprintf(log_fd, "%son %d(%s): ", what, ch->ch_id, ch_part_names[part]);
	아니면
	    fprintf(log_fd, "%son %d: ", what, ch->ch_id);
    }
    아니면
#endif
	fprintf(log_fd, "%s: ", what);
}

#ifndef PROTO  // prototype is in proto.h
    void
ch_log(channel_T *ch, const char *fmt, ...)
{
    만약 (log_fd == NULL)
	반환;

    va_list ap;

    ch_log_lead("", ch, PART_COUNT);
    va_start(ap, fmt);
    vfprintf(log_fd, fmt, ap);
    va_end(ap);
    fputc('\n', log_fd);
    fflush(log_fd);
    did_repeated_msg = 0;
}

    void
ch_error(channel_T *ch, const char *fmt, ...)
{
    만약 (log_fd == NULL)
	반환;

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

#만약 defined(FEAT_JOB_CHANNEL)
/*
 * Log a message "buf[len]" for channel "ch" part "part".
 * Only to be called when ch_log_active() returns TRUE.
 */
    void
ch_log_literal(
	char	    *lead,
	channel_T   *ch,
	ch_part_T   part,
	char_u	    *buf,
	정수	    len)
{
    ch_log_lead(lead, ch, part);
    fprintf(log_fd, "'");
    vim_ignored = (정수)fwrite(buf, len, 1, log_fd);
    fprintf(log_fd, "'\n");
    fflush(log_fd);
}
#endif

/*
 * "ch_log()" function
 */
    void
f_ch_log(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*msg;
    channel_T	*channel = NULL;

    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_chan_or_job_arg(argvars, 1) == FAIL))
	반환;

    msg = tv_get_string(&argvars[0]);
#만약 defined(FEAT_JOB_CHANNEL)
    만약 (argvars[1].v_type != VAR_UNKNOWN)
	channel = get_channel_arg(&argvars[1], FALSE, FALSE, 0);
#endif

    // Prepend "ch_log()" to make it easier to find these entries in the
    // logfile.
    ch_log(channel, "ch_log(): %s", msg);
}

/*
 * "ch_logfile()" function
 */
    void
f_ch_logfile(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *fname;
    char_u *opt = (char_u *)"";
    char_u buf[NUMBUFLEN];

    // Don't open a file in restricted mode.
    만약 (check_restricted() || check_secure())
	반환;

    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	반환;

    fname = tv_get_string(&argvars[0]);
    만약 (argvars[1].v_type == VAR_STRING)
	opt = tv_get_string_buf(&argvars[1], buf);
    ch_logfile(fname, opt);
}

#endif // FEAT_EVAL
