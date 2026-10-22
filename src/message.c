/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * message.c: functions for displaying messages on the command line
 */

#define MESSAGE_FILE		// don't include prototype for smsg()

#شامل "vim.h"

ایستا تهی add_پیام_hist(char_u *s, صحیح len, صحیح attr);
ایستا تهی check_پیام_hist(تهی);
ایستا تهی hit_return_پیام(تهی);
ایستا تهی پیام_home_replace_attr(char_u *fname, صحیح attr);
ایستا تهی پیام_puts_attr_len(نویسه *str, صحیح maxlen, صحیح attr);
ایستا تهی پیام_puts_display(char_u *str, صحیح maxlen, صحیح attr, صحیح recurse);
ایستا تهی پیام_scroll_up(تهی);
ایستا تهی inc_پیام_scrolled(تهی);
ایستا تهی store_sb_text(char_u **sb_str, char_u *s, صحیح attr, صحیح *sb_col, صحیح finish);
ایستا تهی t_puts(صحیح *t_col, char_u *t_s, char_u *s, صحیح attr);
ایستا تهی پیام_puts_printf(char_u *str, صحیح maxlen);
ایستا صحیح do_more_prompt(صحیح typed_char);
ایستا تهی پیام_screen_putchar(صحیح c, صحیح attr);
ایستا تهی پیام_moremsg(صحیح full);
ایستا صحیح  پیام_check_screen(تهی);
ایستا تهی redir_write(char_u *s, صحیح maxlen);
#ifdef FEAT_CON_DIALOG
ایستا char_u *پیام_show_console_dialog(char_u *message, char_u *buttons, صحیح dfltbutton);
ایستا صحیح	confirm_پیام_used = FALSE;	// displaying confirm_msg
ایستا char_u	*confirm_پیام = پوچ;		// ":confirm" message
ایستا char_u	*confirm_پیام_tail;		// tail of confirm_msg
ایستا تهی display_confirm_پیام(تهی);
#endif
#ifdef FEAT_EVAL
ایستا صحیح emsg_to_channel_log = FALSE;
#endif

ساختار پیام_hist
{
    ساختار پیام_hist	*next;
    char_u		*پیام;
    صحیح			attr;
};

ایستا ساختار پیام_hist *first_پیام_hist = پوچ;
ایستا ساختار پیام_hist *last_پیام_hist = پوچ;
ایستا صحیح پیام_hist_len = 0;
ایستا صحیح پیام_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#define MESSAGES_HIT_ENTER	0x001
#define MESSAGES_WAIT		0x002
#define MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#define MESSAGES_OPT_HIT_ENTER "hit-enter"
#define MESSAGES_OPT_WAIT "wait:"
#define MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
ایستا صحیح پیام_flags = MESSAGES_HIT_ENTER | MESSAGES_HISTORY;
ایستا صحیح پیام_wait = 0;

ایستا FILE *verbose_fd = پوچ;
ایستا صحیح  verbose_did_open = FALSE;

/*
 * When writing messages to the screen, there are many different situations.
 * A number of variables is used to remember the current state:
 * msg_didany	    TRUE when messages were written since the last time the
 *		    user reacted to a prompt.
 *		    Reset: After hitting a key for the hit-return prompt,
 *		    hitting <CR> for the command line or input().
 *		    Set: When any message is written to the screen.
 * msg_didout	    TRUE when something was written to the current line.
 *		    Reset: When advancing to the next line, when the current
 *		    text can be overwritten.
 *		    Set: When any message is written to the screen.
 * msg_nowait	    No extra delay for the last drawn message.
 *		    Used in normal_cmd() before the mode message is drawn.
 * emsg_on_display  There was an error message recently.  Indicates that there
 *		    should be a delay before redrawing.
 * msg_scroll	    The next message should not overwrite the current one.
 * msg_scrolled	    How many lines the screen has been scrolled (because of
 *		    messages).  Used in update_screen() to scroll the screen
 *		    back.  Incremented each time the screen scrolls a line.
 * msg_scrolled_ign  TRUE when msg_scrolled is non-zero and msg_puts_attr()
 *		    writes something without scrolling should not make
 *		    need_wait_return to be set.  This is a hack to make ":ts"
 *		    work without an extra prompt.
 * lines_left	    Number of lines available for messages before the
 *		    more-prompt is to be given.  -1 when not set.
 * need_wait_return TRUE when the hit-return prompt is needed.
 *		    Reset: After giving the hit-return prompt, when the user
 *		    has answered some other prompt.
 *		    Set: When the ruler or typeahead display is overwritten,
 *		    scrolling the screen for some message.
 * keep_msg	    Message to be displayed after redrawing the screen, in
 *		    main_loop().
 *		    This is an allocated string or NULL when not used.
 */

/*
 * msg(s) - displays the string 's' on the status line
 * When terminal not initialized (yet) mch_errmsg(..) is used.
 * return TRUE if wait_return() not called
 */
    صحیح
پیام(نویسه *s)
{
    بازگشت پیام_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    صحیح
verb_پیام(نویسه *s)
{
    صحیح		n;

    verbose_enter();
    n = پیام_attr_keep(s, 0, FALSE);
    verbose_leave();

    بازگشت n;
}

    صحیح
پیام_attr(نویسه *s, صحیح attr)
{
    بازگشت پیام_attr_keep(s, attr, FALSE);
}

    صحیح
پیام_attr_keep(
    نویسه	*s,
    صحیح		attr,
    صحیح		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    ایستا صحیح	entered = 0;
    صحیح		retval;
    char_u	*buf = پوچ;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    اگر (!emsg_on_display && message_filtered((char_u *)s))
	بازگشت TRUE;

#ifdef FEAT_EVAL
    اگر (attr == 0)
	set_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#endif

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    اگر (entered >= 3)
	بازگشت TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    اگر ((char_u *)s != keep_پیام
	    || (*s != '<'
		&& last_پیام_hist != پوچ
		&& last_پیام_hist->پیام != پوچ
		&& STRCMP(s, last_پیام_hist->پیام) != 0))
	add_پیام_hist((char_u *)s, -1, attr);

#ifdef FEAT_EVAL
    اگر (emsg_to_channel_log)
	// Write message in the channel log.
	ch_log(پوچ, "ERROR: %s", s);
#endif

    // Truncate the message if needed.
    پیام_start();
    buf = پیام_strtrunc((char_u *)s, FALSE);
    اگر (buf != پوچ)
	s = (نویسه *)buf;

    پیام_outtrans_attr((char_u *)s, attr);
    پیام_clr_eos();
    retval = پیام_end();

    اگر (keep && retval && vim_strsize((char_u *)s)
		    < (صحیح)(Rows - cmdline_row - 1) * cmdline_width + sc_col)
	set_keep_پیام((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    بازگشت retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
پیام_strtrunc(
    char_u	*s,
    صحیح		force)	    // always truncate
{
    char_u	*buf = پوچ;
    صحیح		len;
    صحیح		room;

    // May truncate message to avoid a hit-return prompt
    اگر ((!پیام_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_active && پیام_silent == 0) || force)
    {
	len = vim_strsize(s);
	اگر (پیام_scrolled != 0
#ifdef HAS_MESSAGE_WINDOW
		|| in_echowindow
#endif
		)
	    // Use all the columns.
	    room = (صحیح)(Rows - پیام_row) * cmdline_width - 1;
	وگرنه
	    // Use up to 'showcmd' column.
	    room = (صحیح)(Rows - پیام_row - 1) * cmdline_width + sc_col - 1;
	اگر (len > room && room > 0)
	{
	    اگر (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (room + 2) * 18;
	    وگرنه اگر (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (room + 2) * 2;
	    وگرنه
		len = room + 2;
	    buf = alloc(len);
	    اگر (buf != پوچ)
		trunc_string(s, buf, room, len);
	}
    }
    بازگشت buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    تهی
trunc_string(
    char_u	*s,
    char_u	*buf,
    صحیح		room_in,
    صحیح		buflen)
{
    size_t	room = room_in - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    صحیح		e;
    صحیح		i;
    صحیح		n;

    اگر (*s == NUL)
    {
	اگر (buflen > 0)
	    *buf = NUL;
	بازگشت;
    }

    اگر (room_in < 3)
	room = 0;
    half = room / 2;

    // First part: Start of the string.
    برای (e = 0; len < half && e < buflen; ++e)
    {
	اگر (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    بازگشت;
	}
	n = ptr2cells(s + e);
	اگر (len + n > half)
	    بشکن;
	len += n;
	buf[e] = s[e];
	اگر (has_mbyte)
	    برای (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		اگر (++e == buflen)
		    بشکن;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    اگر (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	وقتی (len + n > room)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    وگرنه اگر (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (صحیح)STRLEN(s);
	برای (;;)
	{
	    انجامبده
		half = half - utf_head_off(s, s + half - 1) - 1;
	    وقتی (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    اگر (len + n > room || half == 0)
		بشکن;
	    len += n;
	    i = (صحیح)half;
	}
    }
    وگرنه
    {
	برای (i = (صحیح)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= room; --i)
	    len += n;
    }


    اگر (i <= e + 3)
    {
	// text fits without truncating
	اگر (s != buf)
	{
	    len = STRLEN(s);
	    اگر (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    اگر (len < 1)
		buf[e - 1] = NUL;
	    وگرنه
		mch_memmove(buf + e, s + e, len);
	}
    }
    وگرنه اگر (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	اگر (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    وگرنه
    {
	// can't fit in the "...", just truncate it
	buf[buflen - 1] = NUL;
    }
}

/*
 * Automatic prototype generation does not understand this function.
 * Note: Caller of smsg() and smsg_attr() must check the resulting string is
 * shorter than IOSIZE!!!
 */
#ifndef PROTO

صحیح vim_snprintf(نویسه *str, size_t str_m, ثابت نویسه *fmt, ...);

    صحیح
smsg(ثابت نویسه *s, ...)
{
    اگر (IObuff == پوچ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	بازگشت پیام((نویسه *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((نویسه *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    بازگشت پیام((نویسه *)IObuff);
}

    صحیح
smsg_attr(صحیح attr, ثابت نویسه *s, ...)
{
    اگر (IObuff == پوچ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	بازگشت پیام_attr((نویسه *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((نویسه *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    بازگشت پیام_attr((نویسه *)IObuff, attr);
}

    صحیح
smsg_attr_keep(صحیح attr, ثابت نویسه *s, ...)
{
    اگر (IObuff == پوچ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	بازگشت پیام_attr_keep((نویسه *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((نویسه *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    بازگشت پیام_attr_keep((نویسه *)IObuff, attr, TRUE);
}

#endif

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
ایستا صحیح	last_sourcing_lnum = 0;
ایستا char_u   *last_sourcing_name = پوچ;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    تهی
reset_last_sourcing(تهی)
{
    VIM_CLEAR(last_sourcing_name);
    last_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    ایستا صحیح
other_sourcing_name(تهی)
{
    اگر (HAVE_SOURCING_INFO && SOURCING_NAME != پوچ)
    {
	اگر (last_sourcing_name != پوچ)
	    بازگشت STRCMP(SOURCING_NAME, last_sourcing_name) != 0;
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ایستا char_u *
get_emsg_source(تهی)
{
    char_u	*Buf, *p;

    اگر (HAVE_SOURCING_INFO && SOURCING_NAME != پوچ && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	اگر (sname == پوچ)
	    sname = SOURCING_NAME;

#ifdef FEAT_EVAL
	اگر (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	وگرنه
#endif
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	اگر (Buf != پوچ)
	    sprintf((نویسه *)Buf, (نویسه *)p, sname);
	vim_free(tofree);
	بازگشت Buf;
    }
    بازگشت پوچ;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ایستا char_u *
get_emsg_lnum(تهی)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    اگر (SOURCING_NAME != پوچ
	    && (other_sourcing_name() || SOURCING_LNUM != last_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	اگر (Buf != پوچ)
	    sprintf((نویسه *)Buf, (نویسه *)p, (long)SOURCING_LNUM);
	بازگشت Buf;
    }
    بازگشت پوچ;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    تهی
پیام_source(صحیح attr)
{
    char_u	*p;
    ایستا صحیح	recursive = FALSE;

    // Bail out if something called here causes an error.
    اگر (recursive)
	بازگشت;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_source();
    اگر (p != پوچ)
    {
	پیام_scroll = TRUE;  // this will take more than one line
	پیام_attr((نویسه *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    اگر (p != پوچ)
    {
	پیام_attr((نویسه *)p, HL_ATTR(HLF_N));
	vim_free(p);
	last_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    اگر (SOURCING_NAME == پوچ || other_sourcing_name())
    {
	VIM_CLEAR(last_sourcing_name);
	اگر (SOURCING_NAME != پوچ)
	    last_sourcing_name = vim_strsave(SOURCING_NAME);
    }
    --no_wait_return;

    recursive = FALSE;
}

/*
 * Return TRUE if not giving error messages right now:
 * If "emsg_off" is set: no error messages at the moment.
 * If "msg" is in 'debug': do error message but without side effects.
 * If "emsg_skip" is set: never do error messages.
 */
    ایستا صحیح
emsg_not_now(تهی)
{
    اگر ((emsg_off > 0 && vim_strchr(p_debug, 'm') == پوچ
					  && vim_strchr(p_debug, 't') == پوچ)
#ifdef FEAT_EVAL
	    || emsg_skip > 0
#endif
	    )
	بازگشت TRUE;
    بازگشت FALSE;
}

#اگر defined(FEAT_EVAL)
ایستا garray_T ignore_error_list = GA_EMPTY;

    تهی
ignore_error_for_testing(char_u *error)
{
    اگر (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, اندازه(char_u *), 1);

    اگر (STRCMP("RESET", error) == 0)
	ga_clear_strings(&ignore_error_list);
    وگرنه
	ga_copy_string(&ignore_error_list, error);
}

    ایستا صحیح
ignore_error(ثابت نویسه *پیام)
{
    صحیح i;

    برای (i = 0; i < ignore_error_list.ga_len; ++i)
	اگر (strstr(پیام,
		  (نویسه *)((char_u **)(ignore_error_list.ga_data))[i]) != پوچ)
	    بازگشت TRUE;
    بازگشت FALSE;
}
#endif

#اگر !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    تهی
do_perror(نویسه *پیام)
{
    perror(پیام);
    ++emsg_silent;
    emsg(پیام);
    --emsg_silent;
}
#endif

/*
 * emsg_core() - display an error message
 *
 * Rings the bell, if appropriate, and calls message() to do the real work
 * When terminal not initialized (yet) mch_errmsg(..) is used.
 *
 * Return TRUE if wait_return() not called.
 * Note: caller must check 'emsg_not_now()' before calling this.
 */
    ایستا صحیح
emsg_core(ثابت نویسه *s)
{
    صحیح		attr;
    char_u	*p;
    صحیح		r;
#ifdef FEAT_EVAL
    صحیح		ignore = FALSE;
    صحیح		severe;
#endif

#ifdef FEAT_EVAL
    // When testing some errors are turned into a normal message.
    اگر (ignore_error(s))
	// don't call msg() if it results in a dialog
	بازگشت پیام_use_printf() ? FALSE : پیام((نویسه *)s);
#endif

    ++called_emsg;

#ifdef FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#endif

    اگر (!emsg_off || vim_strchr(p_debug, 't') != پوچ)
    {
#ifdef FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	اگر (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    اگر (!ignore)
		++did_emsg;
	    بازگشت TRUE;
	}

	اگر (in_assert_fails && emsg_assert_fails_پیام == پوچ)
	{
	    emsg_assert_fails_پیام = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == پوچ ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#endif

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	اگر (emsg_silent != 0)
	{
#ifdef FEAT_EVAL
	    ++did_emsg_silent;
#endif
	    اگر (emsg_noredir == 0)
	    {
		پیام_start();
		p = get_emsg_source();
		اگر (p != پوچ)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		اگر (p != پوچ)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		redir_write((char_u *)s, -1);
	    }
#ifdef FEAT_EVAL
	    // Only increment did_emsg_def when :silent! wasn't used inside the
	    // :def function.
	    اگر (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#endif
#ifdef FEAT_EVAL
	    ch_log(پوچ, "ERROR silent: %s", s);
#endif
	    بازگشت TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	پیام_silent = 0;
	cmd_silent = FALSE;

	اگر (global_busy)		// break :global command
	    ++global_busy;

	اگر (p_eb)
	    beep_flush();		// also includes flush_buffers()
	وگرنه
	    flush_buffers(FLUSH_MINIMAL);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ifdef FEAT_EVAL
	++uncaught_emsg;
#endif
    }

#ifdef HAS_MESSAGE_WINDOW
    اگر (!in_echowindow)
#endif
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    اگر (پیام_scrolled != 0)
	need_wait_return = TRUE;    // needed in case emsg() is called after
				    // wait_return() has reset need_wait_return
				    // and a redraw is expected because
				    // msg_scrolled is non-zero

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = TRUE;
#endif
    /*
     * Display name and line number for the source of the error.
     */
    پیام_scroll = TRUE;
    پیام_source(attr);

    /*
     * Display the error message itself.
     */
    پیام_nowait = FALSE;			// wait for this msg
    r = پیام_attr((نویسه *)s, attr);

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = FALSE;
#endif
    بازگشت r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    صحیح
emsg(نویسه *s)
{
    // Skip this if not giving error messages at the moment.
    اگر (emsg_not_now())
	بازگشت TRUE;

    بازگشت emsg_core(s);
}

#ifndef PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    صحیح
semsg(ثابت نویسه *s, ...)
{
    // Skip this if not giving error messages at the moment.
    اگر (emsg_not_now())
	بازگشت TRUE;

    اگر (IObuff == پوچ)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	بازگشت emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((نویسه *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    بازگشت emsg_core((نویسه *)IObuff);
}
#endif

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    تهی
iemsg(نویسه *s)
{
    اگر (emsg_not_now())
	بازگشت;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    emsg_core(s);
#اگر defined(ABORT_ON_INTERNAL_ERROR) && defined(FEAT_EVAL)
    set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    پیام_putchar('\n');  // avoid overwriting the error message
    out_flush();
    abort();
#endif
}

#ifndef PROTO  // manual proto with __attribute__
/*
 * Same as semsg(...) but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 * Note: caller must not pass 'IObuff' as 1st argument.
 */
    تهی
siemsg(ثابت نویسه *s, ...)
{
    اگر (emsg_not_now())
	بازگشت;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    اگر (IObuff == پوچ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    وگرنه
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((نویسه *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((نویسه *)IObuff);
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    پیام_putchar('\n');  // avoid overwriting the error message
    out_flush();
    abort();
# endif
}
#endif

/*
 * Give an "Internal error" message.
 */
    تهی
internal_error(نویسه *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    siemsg(_(e_internal_error_str), where);
}

#اگر defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    تهی
internal_error_no_abort(نویسه *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    semsg(_(e_internal_error_str), where);
}
#endif

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    تهی
emsg_invreg(صحیح name)
{
    semsg(_(e_invalid_register_name_str), transchar_buf(پوچ, name));
}

#اگر defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    تهی
emsg_namelen(نویسه *پیام, char_u *name, صحیح len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(پیام, copy == پوچ ? "NULL" : (نویسه *)copy);
    vim_free(copy);
}
#endif

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    نویسه *
پیام_trunc_attr(نویسه *s, صحیح force, صحیح attr)
{
    صحیح		n;
    نویسه	*ts;

    // Add message to history before truncating
    add_پیام_hist((char_u *)s, -1, attr);

    ts = (نویسه *)پیام_may_trunc(force, (char_u *)s);

    پیام_hist_off = TRUE;
    n = پیام_attr(ts, attr);
    پیام_hist_off = FALSE;

    اگر (n)
	بازگشت ts;
    بازگشت پوچ;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
پیام_may_trunc(صحیح force, char_u *s)
{
    صحیح		n;
    صحیح		room;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    room = (صحیح)(Rows - cmdline_row - 1) * cmdline_width + sc_col - 1;
    اگر (room > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_active))
	    && (n = (صحیح)STRLEN(s) - room) > 0)
    {
	اگر (has_mbyte)
	{
	    صحیح	size = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    اگر (size <= room)
		بازگشت s;

	    برای (n = 0; size >= room; )
	    {
		size -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    بازگشت s;
}

    ایستا تهی
add_پیام_hist(
    char_u	*s,
    صحیح		len,		// -1 for undetermined length
    صحیح		attr)
{
    ساختار پیام_hist *p;

    اگر (پیام_hist_off || پیام_silent != 0)
	بازگشت;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(ساختار پیام_hist);
    اگر (p == پوچ)
	بازگشت;

    اگر (len < 0)
	len = (صحیح)STRLEN(s);
    // remove leading and trailing newlines
    وقتی (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    وقتی (len > 0 && s[len - 1] == '\n')
	--len;
    p->پیام = vim_strnsave(s, len);
    p->next = پوچ;
    p->attr = attr;
    اگر (last_پیام_hist != پوچ)
	last_پیام_hist->next = p;
    last_پیام_hist = p;
    اگر (first_پیام_hist == پوچ)
	first_پیام_hist = last_پیام_hist;
    ++پیام_hist_len;

    check_پیام_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    صحیح
delete_first_پیام(تهی)
{
    ساختار پیام_hist *p;

    اگر (پیام_hist_len <= 0)
	بازگشت FAIL;
    p = first_پیام_hist;
    first_پیام_hist = p->next;
    اگر (first_پیام_hist == پوچ)
	last_پیام_hist = پوچ;  // history is empty
    vim_free(p->پیام);
    vim_free(p);
    --پیام_hist_len;
    بازگشت OK;
}

    ایستا تهی
check_پیام_hist(تهی)
{
    // Don't let the message history get too big
    وقتی (پیام_hist_len > 0 && پیام_hist_len > پیام_hist_max)
	(تهی)delete_first_پیام();
}


    صحیح
messagesopt_changed(تهی)
{
    char_u	*p;
    صحیح		messages_flags_new = 0;
    صحیح		messages_wait_new = 0;
    صحیح		messages_history_new = 0;

    p = p_mopt;
    وقتی (*p != NUL)
    {
	اگر (STRNCMP(p, MESSAGES_OPT_HIT_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER);
	    messages_flags_new |= MESSAGES_HIT_ENTER;
	}
	وگرنه اگر (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	وگرنه اگر (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	اگر (*p != ',' && *p != NUL)
	    بازگشت FAIL;
	اگر (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    اگر (!(messages_flags_new & (MESSAGES_HIT_ENTER | MESSAGES_WAIT)))
	بازگشت FAIL;

    // "history" must be set
    اگر (!(messages_flags_new & MESSAGES_HISTORY))
	بازگشت FAIL;

    اگر (messages_history_new < 0 || messages_history_new > 10000)
	بازگشت FAIL;

    اگر (messages_wait_new < 0 || messages_wait_new > 10000)
	بازگشت FAIL;

    پیام_flags = messages_flags_new;
    پیام_wait = messages_wait_new;

    پیام_hist_max = messages_history_new;
    check_پیام_hist();

    بازگشت OK;
}

/*
 * ":messages" command.
 */
    تهی
ex_messages(exarg_T *eap)
{
    ساختار پیام_hist *p;
    char_u	    *s;
    صحیح		    c = 0;

    اگر (STRCMP(eap->arg, "clear") == 0)
    {
	صحیح keep = eap->addr_شمار == 0 ? 0 : eap->line2;

	وقتی (پیام_hist_len > keep)
	    (تهی)delete_first_پیام();
	بازگشت;
    }

    اگر (*eap->arg != NUL)
    {
	emsg(_(e_invalid_argument));
	بازگشت;
    }

    پیام_hist_off = TRUE;

    p = first_پیام_hist;
    اگر (eap->addr_شمار != 0)
    {
	// Count total messages
	برای (; p != پوچ && !got_int; p = p->next)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	برای (p = first_پیام_hist; p != پوچ && !got_int && c > 0;
						    p = p->next, c--);
    }

    اگر (p == first_پیام_hist)
    {
#ifdef FEAT_MULTI_LANG
	s = get_mess_lang();
#وگرنه
	s = mch_getenv((char_u *)"LANG");
#endif
	اگر (s != پوچ && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    پیام_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    برای (; p != پوچ && !got_int; p = p->next)
	اگر (p->پیام != پوچ)
	    پیام_attr((نویسه *)p->پیام, p->attr);

    پیام_hist_off = FALSE;
}

#اگر defined(FEAT_CON_DIALOG) || defined(FIND_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    تهی
پیام_end_prompt(تهی)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_row = پیام_row;
    پیام_col = 0;
    پیام_clr_eos();
    سطرها_چپ = -1;
}
#endif

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    تهی
wait_return(صحیح redraw)
{
    صحیح		c;
    صحیح		oldState;
    صحیح		tmpState;
    صحیح		had_got_int;
    صحیح		save_reg_recording;
    FILE	*save_scriptout;

    اگر (redraw == TRUE)
	set_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    اگر (پیام_silent != 0)
	بازگشت;
#ifdef HAS_MESSAGE_WINDOW
    اگر (in_echowindow)
	بازگشت;
#endif

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    اگر (vgetc_busy > 0)
	بازگشت;
    need_wait_return = TRUE;
    اگر (no_wait_return)
    {
	اگر (!exmode_active)
	    cmdline_row = پیام_row;
	بازگشت;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    اگر (quit_more)
    {
	c = CAR;		// just pretend CR was hit
	quit_more = FALSE;
	got_int = FALSE;
    }
    وگرنه اگر (exmode_active)
    {
	پیام_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    وگرنه اگر (!stuff_empty())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    وگرنه
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = MODE_HITRETURN;
	setmouse();
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#endif
	cmdline_row = پیام_row;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	اگر (need_check_timestamps)
	    check_timestamps(FALSE);

	اگر (پیام_flags & MESSAGES_HIT_ENTER)
	{
	    hit_return_پیام();

	    انجامبده
	    {
#ifdef FEAT_IMAGE
		// Make sure to update image placements (since their row offset
		// will have been changed).
		draw_image_placements();
#endif
		// Remember "got_int", if it is set vgetc() probably returns a
		// CTRL-C, but we need to loop then.
		had_got_int = got_int;

		// Don't do mappings here, we put the character back in the
		// typeahead buffer.
		++no_mapping;
		++allow_کلیدها;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = پوچ;
		c = safe_vgetc();
		اگر (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_کلیدها;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ifdef FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		اگر (کلید_Typed && c == Ctrl_Y && clip_star.state == SELECT_DONE)
		{
		    clip_copy_modeless_selection(TRUE);
		    c = K_IGNORE;
		}
#endif

		/*
		* Allow scrolling back in the messages.
		* Also accept scroll-down commands when messages fill the
		* screen, to avoid that typing one 'j' too many makes the
		* messages disappear.
		*/
		اگر (کلید_Typed && p_more && !p_cp)
		{
		    اگر (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_UP || c == K_PAGEUP)
		    {
			اگر (پیام_scrolled > Rows)
			    // scroll back to show older messages
			    do_more_prompt(c);
			وگرنه
			{
			    پیام_didout = FALSE;
			    c = K_IGNORE;
			    پیام_col =
#ifdef FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_width - 1 :
#endif
				0;
			}
			اگر (quit_more)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_more = FALSE;
			    got_int = FALSE;
			}
			وگرنه اگر (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    hit_return_پیام();
			}
		    }
		    وگرنه اگر (پیام_scrolled > Rows - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_DOWN || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } وقتی ((had_got_int && c == Ctrl_C)
				|| c == K_IGNORE
#ifdef FEAT_GUI
				|| c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
#endif
				|| c == K_LEFTDRAG   || c == K_LEFTRELEASE
				|| c == K_MIDDLEDRAG || c == K_MIDDLERELEASE
				|| c == K_RIGHTDRAG  || c == K_RIGHTRELEASE
				|| c == K_MOUSELEFT  || c == K_MOUSERIGHT
				|| c == K_MOUSEDOWN  || c == K_MOUSEUP
				|| c == K_MOUSEMOVE
				|| (!mouse_has(MOUSE_RETURN)
				    && mouse_row < پیام_row
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    اگر (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(تهی)jump_to_mouse(MOUSE_SETPOS, پوچ, 0);
	    وگرنه اگر (!کلید_Typed || (vim_strchr((char_u *)"\r\n ", c) == پوچ
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_mask);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	وگرنه
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(پیام_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    اگر (c == ':' || c == '?' || c == '/')
    {
	اگر (!exmode_active)
	    cmdline_row = پیام_row;
	skip_redraw = TRUE;	    // skip redraw once
	do_redraw = FALSE;
#ifdef FEAT_TERMINAL
	skip_term_loop = TRUE;
#endif
    }

    /*
     * If the window size changed set_shellsize() will redraw the screen.
     * Otherwise the screen is only redrawn if 'redraw' is set and no ':'
     * typed.
     */
    tmpState = State;
    State = oldState;		    // restore State before set_shellsize
    setmouse();
    پیام_check();

#ifndef MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    اگر (swapping_screen() && !termcap_active)
	newline_on_exit = TRUE;
#endif

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    سطرها_چپ = -1;		// reset lines_left at next msg_start()
    reset_last_sourcing();
    اگر (keep_پیام != پوچ && vim_strsize(keep_پیام) >=
			    (Rows - cmdline_row - 1) * cmdline_width + sc_col)
	VIM_CLEAR(keep_پیام);	    // don't redisplay message, it's too long

    اگر (tmpState == MODE_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    وگرنه اگر (!skip_redraw
	    && (redraw == TRUE || (پیام_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    ایستا تهی
hit_return_پیام(تهی)
{
    صحیح		save_p_more = p_more;

    p_more = FALSE;	// don't want to see this message when scrolling back
    اگر (پیام_didout)	// start on a new line
	پیام_putchar('\n');
    اگر (got_int)
	پیام_puts(_("Interrupt: "));

    پیام_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    اگر (!پیام_use_printf())
	پیام_clr_eos();
    p_more = save_p_more;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    تهی
set_keep_پیام(char_u *s, صحیح attr)
{
    vim_free(keep_پیام);
    اگر (s != پوچ && پیام_silent == 0)
	keep_پیام = vim_strsave(s);
    وگرنه
	keep_پیام = پوچ;
    keep_پیام_more = FALSE;
    keep_پیام_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    تهی
set_keep_پیام_from_hist(تهی)
{
    اگر (keep_پیام == پوچ && last_پیام_hist != پوچ && پیام_scrolled == 0
						      && (State & MODE_NORMAL))
	set_keep_پیام(last_پیام_hist->پیام, last_پیام_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    تهی
پیام_start(تهی)
{
    صحیح		did_return = FALSE;

    اگر (پیام_row < cmdline_row)
	پیام_row = cmdline_row;

    اگر (!پیام_silent)
    {
	VIM_CLEAR(keep_پیام);
	need_fileinfo = FALSE;
    }

#ifdef FEAT_EVAL
    اگر (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	پیام_clr_eos();
    }
#endif

#ifdef HAS_MESSAGE_WINDOW
    اگر (in_echowindow)
    {
	اگر (popup_message_win_visible()
		    && ((پیام_col > 0 && (پیام_scroll || !full_screen))
			|| in_echowindow))
	{
	    win_T *wp = popup_get_message_win();

	    // start a new line
	    curbuf = wp->w_buffer;
	    ml_append(wp->w_buffer->b_ml.ml_سطر_شمار,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_buffer;
	}
	پیام_col = 0;
    }
    وگرنه
#endif
	اگر (!پیام_scroll && full_screen)	// overwrite last message
    {
	پیام_row = cmdline_row;
	پیام_col =
#ifdef FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_width - 1 :
#endif
	    0;
    }
    وگرنه اگر (پیام_didout || in_echowindow)
    {
	// start message on next line
	پیام_putchar('\n');
	did_return = TRUE;
	اگر (exmode_active != EXMODE_NORMAL)
	    cmdline_row = پیام_row;
    }
    اگر (!پیام_didany || سطرها_چپ < 0)
	پیام_starthere();
    اگر (پیام_silent == 0)
    {
	پیام_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    اگر (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    تهی
پیام_starthere(تهی)
{
    سطرها_چپ = cmdline_row;
    پیام_didany = FALSE;
}

    تهی
پیام_putchar(صحیح c)
{
    پیام_putchar_attr(c, 0);
}

    تهی
پیام_putchar_attr(صحیح c, صحیح attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    اگر (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    وگرنه
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    پیام_puts_attr((نویسه *)buf, attr);
}

    تهی
پیام_outnum(long n)
{
    نویسه	buf[20];

    sprintf(buf, "%ld", n);
    پیام_puts(buf);
}

    تهی
پیام_home_replace(char_u *fname)
{
    پیام_home_replace_attr(fname, 0);
}

#اگر defined(FEAT_FIND_ID)
    تهی
پیام_home_replace_hl(char_u *fname)
{
    پیام_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#endif

    ایستا تهی
پیام_home_replace_attr(char_u *fname, صحیح attr)
{
    char_u	*name;

    name = home_replace_save(پوچ, fname);
    اگر (name != پوچ)
	پیام_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    صحیح
پیام_outtrans(char_u *str)
{
    بازگشت پیام_outtrans_attr(str, 0);
}

    صحیح
پیام_outtrans_attr(char_u *str, صحیح attr)
{
    بازگشت پیام_outtrans_len_attr(str, (صحیح)STRLEN(str), attr);
}

    صحیح
پیام_outtrans_len(char_u *str, صحیح len)
{
    بازگشت پیام_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
پیام_outtrans_one(char_u *p, صحیح attr)
{
    صحیح		l;

    اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	پیام_outtrans_len_attr(p, l, attr);
	بازگشت p + l;
    }
    پیام_puts_attr((نویسه *)transchar_byte_buf(پوچ, *p), attr);
    بازگشت p + 1;
}

    صحیح
پیام_outtrans_len_attr(char_u *msgstr, صحیح len, صحیح attr)
{
    صحیح		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_start = msgstr;
    char_u	*s;
    صحیح		mb_l;
    صحیح		c;
    صحیح		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    اگر (attr == 0)
	attr = HL_ATTR(HLF_پیام);

    // if MSG_HIST flag set, add message to history
    اگر (attr & پیام_HIST)
    {
	add_پیام_hist(str, len, attr);
	attr &= ~پیام_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    اگر (پیام_silent == 0 && len > 0 && پیام_row >= cmdline_row && پیام_col == 0)
    {
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    اگر (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	پیام_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    وقتی (--len >= 0 && !got_int)
    {
	اگر (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	وگرنه اگر (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	وگرنه
	    mb_l = 1;
	اگر (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    اگر (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    وگرنه
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		اگر (str > plain_start)
		    پیام_puts_attr_len((نویسه *)plain_start,
					       (صحیح)(str - plain_start), attr);
		plain_start = str + mb_l;
		پیام_puts_attr((نویسه *)transchar_buf(پوچ, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	وگرنه
	{
	    s = transchar_byte_buf(پوچ, *str);
	    اگر (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		اگر (str > plain_start)
		    پیام_puts_attr_len((نویسه *)plain_start,
					       (صحیح)(str - plain_start), attr);
		plain_start = str + 1;
		پیام_puts_attr((نویسه *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (صحیح)STRLEN(s);
	    }
	    وگرنه
		++retval;
	    ++str;
	}
    }

    اگر (str > plain_start && !got_int)
	// print the printable chars at the end
	پیام_puts_attr_len((نویسه *)plain_start, (صحیح)(str - plain_start), attr);

    got_int |= save_got_int;

    بازگشت retval;
}

#اگر defined(FEAT_QUICKFIX)
    تهی
پیام_make(char_u *arg)
{
    صحیح	    i;
    ایستا char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    برای (i = 5; *arg && i >= 0; --i)
	اگر (*arg++ != str[i])
	    بشکن;
    اگر (i < 0)
    {
	پیام_putchar('\n');
	برای (i = 0; rs[i]; ++i)
	    پیام_putchar(rs[i] - 3);
    }
}
#endif

/*
 * Output the string 'str' up to a NUL character.
 * Return the number of characters it takes on the screen.
 *
 * If K_SPECIAL is encountered, then it is taken in conjunction with the
 * following character and shown as <F1>, <S-Up> etc.  Any other character
 * which is not printable shown in <> form.
 * If 'from' is TRUE (lhs of a mapping), a space is shown as <Space>.
 * If a character is displayed in one of these special ways, is also
 * highlighted (its highlight name is '8' in the p_hl variable).
 * Otherwise characters are not highlighted.
 * This function is used to show mappings, where we want to see how to type
 * the character/string -- webb
 */
    صحیح
پیام_outtrans_special(
    char_u	*strstart,
    صحیح		from,	// TRUE for lhs of a mapping
    صحیح		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    صحیح		retval = 0;
    نویسه	*text;
    صحیح		attr;
    صحیح		len;

    attr = HL_ATTR(HLF_8);
    وقتی (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	اگر ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    text = "<Space>";
	    ++str;
	}
	وگرنه
	    text = (نویسه *)str2special(&str, from, FALSE);
	اگر (text[0] != NUL && text[1] == NUL)
	    // single-byte character or illegal byte
	    text = (نویسه *)transchar_byte_buf(پوچ, (char_u)text[0]);
	len = vim_strsize((char_u *)text);
	اگر (maxlen > 0 && retval + len >= maxlen)
	    بشکن;
	// Highlight special keys
	پیام_puts_attr(text, len > 1
		&& (*mb_ptr2len)((char_u *)text) <= 1 ? attr : 0);
	retval += len;
    }
    بازگشت retval;
}

#اگر defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    صحیح	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    صحیح	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    وقتی (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    بازگشت (char_u *)ga.ga_data;
}
#endif

/*
 * Return the printable string for the key codes at "*sp".
 * On illegal byte return a string with only that byte.
 * Used for translating the lhs or rhs of a mapping to printable chars.
 * Advances "sp" to the next code.
 */
    char_u *
str2special(
    char_u	**sp,
    صحیح		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    صحیح		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    صحیح			c;
    ایستا char_u	buf[7];
    char_u		*str = *sp;
    صحیح			modifiers = 0;
    صحیح			special = FALSE;

    اگر (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	اگر (p != پوچ)
	    بازگشت p;
    }

    c = *str;
    اگر ((c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	) && str[1] != NUL && str[2] != NUL)
    {
	اگر (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	اگر ((c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = TO_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	اگر (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    اگر (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	اگر (p != پوچ)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	وگرنه
	    // illegal byte
	    *sp = str + 1;
    }
    وگرنه
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    اگر (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	بازگشت get_special_کلید_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    بازگشت buf;
}

/*
 * Translate a key sequence into special key names.
 */
    تهی
str2specialbuf(char_u *sp, char_u *buf, صحیح len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    وقتی (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	اگر (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	وگرنه
	    بشکن;
    }
}

/*
 * print line for :print or :list command
 */
    تهی
پیام_prt_سطر(char_u *s, صحیح list)
{
    صحیح		c;
    صحیح		col = 0;
    صحیح		n_extra = 0;
    صحیح		c_extra = 0;
    صحیح		c_final = 0;
    char_u	*p_extra = پوچ;	    // init to make SASC shut up
    صحیح		n;
    صحیح		attr = 0;
    char_u	*trail = پوچ;
    char_u	*lead = پوچ;
    صحیح		in_multispace = FALSE;
    صحیح		multispace_pos = 0;
    صحیح		l;
    char_u	buf[MB_MAXBYTES + 1];

    اگر (curwin->w_p_list)
	list = TRUE;

    اگر (list)
    {
	// find start of trailing whitespace
	اگر (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    وقتی (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	اگر (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != پوچ
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    وقتی (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    اگر (*lead == NUL)
		lead = پوچ;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    اگر (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	پیام_putchar(' ');

    وقتی (!got_int)
    {
	اگر (n_extra > 0)
	{
	    --n_extra;
	    اگر (n_extra == 0 && c_final)
		c = c_final;
	    وگرنه اگر (c_extra)
		c = c_extra;
	    وگرنه
		c = *p_extra++;
	}
	وگرنه اگر (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    col += (*mb_ptr2cells)(s);
	    اگر (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    وگرنه اگر (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		صحیح len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    وگرنه
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    پیام_puts((نویسه *)buf);
	    s += l;
	    ادامه;
	}
	وگرنه
	{
	    attr = 0;
	    c = *s++;
	    اگر (list)
	    {
		in_multispace = c == ' ' && (*s == ' '
						 || (col > 0 && s[-2] == ' '));
		اگر (!in_multispace)
		    multispace_pos = 0;
	    }
	    اگر (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ifdef FEAT_VARTABS
		n_extra = tabstop_padding(col, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#وگرنه
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
#endif
		اگر (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		وگرنه
		{
		    صحیح lcs_tab1 = curwin->w_lcs_chars.tab1;
		    صحیح lcs_tab2 = curwin->w_lcs_chars.tab2;
		    صحیح lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    اگر (lead != پوچ && s <= lead
					&& curwin->w_lcs_chars.leadtab1 != NUL)
		    {
			lcs_tab1 = curwin->w_lcs_chars.leadtab1;
			lcs_tab2 = curwin->w_lcs_chars.leadtab2;
			lcs_tab3 = curwin->w_lcs_chars.leadtab3;
		    }

		    c = (n_extra == 0 && lcs_tab3) ? lcs_tab3 : lcs_tab1;
		    c_extra = lcs_tab2;
		    c_final = lcs_tab3;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	    وگرنه اگر (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    وگرنه اگر (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    وگرنه اگر (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(پوچ, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    وگرنه اگر (c == ' ')
	    {
		اگر (lead != پوچ && s <= lead && in_multispace
			&& curwin->w_lcs_chars.leadmultispace != پوچ)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    اگر (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		وگرنه اگر (lead != پوچ && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		وگرنه اگر (trail != پوچ && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		وگرنه اگر (in_multispace
			&& curwin->w_lcs_chars.multispace != پوچ)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    اگر (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		وگرنه اگر (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	اگر (c == NUL)
	    بشکن;

	پیام_putchar_attr(c, attr);
	col++;
    }
    پیام_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    ایستا char_u *
screen_puts_mbyte(char_u *s, صحیح l, صحیح attr)
{
    صحیح		cw;

    پیام_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    اگر (cw > 1 && (
#ifdef FEAT_RIGHTLEFT
		cmdmsg_rl ? پیام_col <= 1 :
#endif
		پیام_col == cmdline_width - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	پیام_screen_putchar('>', HL_ATTR(HLF_AT));
	بازگشت s;
    }

    screen_puts_len(s, l, پیام_row, cmdline_col_off + پیام_col, attr);
#ifdef FEAT_RIGHTLEFT
    اگر (cmdmsg_rl)
    {
	پیام_col -= cw;
	اگر (پیام_col == 0)
	{
	    پیام_col = cmdline_width;
	    ++پیام_row;
	}
    }
    وگرنه
#endif
    {
	پیام_col += cw;
	اگر (پیام_col >= cmdline_width)
	{
	    پیام_col = 0;
	    ++پیام_row;
	}
    }
    بازگشت s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    تهی
پیام_puts(نویسه *s)
{
    پیام_puts_attr(s, 0);
}

    تهی
پیام_puts_title(نویسه *s)
{
    پیام_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    ایستا تهی
پیام_outtrans_long_len_attr(char_u *longstr, صحیح len, صحیح attr)
{
    صحیح		slen = len;
    صحیح		room;

    room = cmdline_width - پیام_col;
    اگر (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	پیام_outtrans_len_attr(longstr, slen, attr);
	پیام_puts_attr("...", HL_ATTR(HLF_8));
    }
    پیام_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    تهی
پیام_outtrans_long_attr(char_u *longstr, صحیح attr)
{
    پیام_outtrans_long_len_attr(longstr, (صحیح)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    تهی
پیام_puts_attr(نویسه *s, صحیح attr)
{
    پیام_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    ایستا تهی
پیام_puts_attr_len(نویسه *str, صحیح maxlen, صحیح attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    اگر (پیام_silent != 0)
	بازگشت;

    اگر (attr == 0)
	attr = HL_ATTR(HLF_پیام);

    // if MSG_HIST flag set, add message to history
    اگر ((attr & پیام_HIST) && maxlen < 0)
    {
	add_پیام_hist((char_u *)str, -1, attr);
	attr &= ~پیام_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    اگر (پیام_scrolled != 0 && !پیام_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    پیام_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    اگر (پیام_use_printf())
	پیام_puts_printf((char_u *)str, maxlen);
    وگرنه
	پیام_puts_display((char_u *)str, maxlen, attr, FALSE);

    need_fileinfo = FALSE;
}

// values for "where"
#define PUT_APPEND 0		// append to "lnum"
#define PUT_TRUNC 1		// replace "lnum"
#define PUT_BELOW 2		// add below "lnum"
				//
#ifdef HAS_MESSAGE_WINDOW
/*
 * Put text "t_s" until "end" in the message window.
 * "where" specifies where to put the text.
 */
    ایستا تهی
put_پیام_win(win_T *wp, صحیح where, char_u *t_s, char_u *end, linenr_T lnum)
{
    char_u  *p;

    اگر (where == PUT_BELOW)
    {
	اگر (*end != NUL)
	{
	    p = vim_strnsave(t_s, end - t_s);
	    اگر (p == پوچ)
		بازگشت;
	}
	وگرنه
	    p = t_s;
	ml_append_buf(wp->w_buffer, lnum, p, (colnr_T)0, FALSE);
	اگر (p != t_s)
	    vim_free(p);
    }
    وگرنه
    {
	char_u *newp;

	curbuf = wp->w_buffer;
	اگر (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    اگر (newp == پوچ)
		بازگشت;
	    اگر (*end != NUL)
		newp[STRLEN(ml_get(lnum)) + (end - t_s)] = NUL;
	}
	وگرنه
	{
	    newp = vim_strnsave(t_s, end - t_s);
	    اگر (newp == پوچ)
		بازگشت;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_buffer;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    پیام_col += (صحیح)(end - t_s);
}
#endif

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    ایستا تهی
پیام_puts_display(
    char_u	*str,
    صحیح		maxlen,
    صحیح		attr,
    صحیح		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    صحیح		t_col = 0;	// screen cells todo, 0 when "t_s" not used
    صحیح		l;
    صحیح		cw;
    char_u	*sb_str = str;
    صحیح		sb_col = پیام_col;
    صحیح		wrap;
    صحیح		did_last_char;
#ifdef HAS_MESSAGE_WINDOW
    صحیح		where = PUT_APPEND;
    win_T	*پیام_win = پوچ;
    linenr_T    lnum = 1;

    اگر (in_echowindow)
    {
	پیام_win = popup_get_message_win();

	اگر (پیام_win != پوچ)
	{
	    اگر (!popup_message_win_visible())
	    {
		اگر (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    پیام_win = پوچ;
		}
		وگرنه
		{
		    // currently hidden, make it empty
		    curbuf = پیام_win->w_buffer;
		    وقتی ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
			ml_delete(1);
		    curbuf = curwin->w_buffer;
		}
	    }
	    وگرنه
	    {
		lnum = پیام_win->w_buffer->b_ml.ml_سطر_شمار;
		اگر (پیام_col == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#endif

    did_wait_return = FALSE;
    وقتی ((maxlen < 0 || (صحیح)(s - str) < maxlen) && *s != NUL)
    {
#ifdef HAS_MESSAGE_WINDOW
	// For echowindow, use full width; for regular messages, leave last column
	صحیح wrap_col = پیام_win != پوچ ? cmdline_width : cmdline_width - 1;
#وگرنه
	صحیح wrap_col = cmdline_width - 1;
#endif

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	اگر (!recurse && پیام_row >= Rows - 1 && (*s == '\n' || (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			پیام_col <= 1
		      || (*s == TAB && پیام_col <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && پیام_col <= 2))
		    :
#endif
		      ((*s != '\r' && پیام_col + t_col >= wrap_col)
		       || (*s == TAB && پیام_col + t_col
			   >= (wrap_col & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && پیام_col + t_col >= wrap_col - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    اگر (t_col > 0)
	    {
		// output postponed text
#ifdef HAS_MESSAGE_WINDOW
		اگر (پیام_win != پوچ)
		{
		    put_پیام_win(پیام_win, where, t_s, s, lnum);
		    اگر (where == PUT_BELOW)
			++lnum;
		    t_col = 0;
		    where = PUT_BELOW;
		}
		وگرنه
#endif
		    t_puts(&t_col, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    اگر (پیام_no_more && سطرها_چپ == 0)
		بشکن;

#ifdef HAS_MESSAGE_WINDOW
	    اگر (پیام_win == پوچ)
#endif
		// Scroll the screen up one line.
		پیام_scroll_up();

#ifdef HAS_MESSAGE_WINDOW
	    اگر (پیام_win == پوچ)
	    {
#endif
		پیام_row = Rows - 2;
		اگر (پیام_col >= cmdline_width)  // can happen after screen resize
		    پیام_col = cmdline_width - 1;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif

	    // Display char in last column before showing more-prompt.
	    اگر (*s >= ' '
#ifdef FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#endif
	       )
	    {
		اگر (has_mbyte)
		{
		    اگر (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (صحیح)((str + maxlen) - s));
		    وگرنه
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		وگرنه
		    پیام_screen_putchar(*s++, attr);
		did_last_char = TRUE;
	    }
	    وگرنه
		did_last_char = FALSE;

	    اگر (p_more)
		// store text for scrolling back
		store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

#ifdef HAS_MESSAGE_WINDOW
	    اگر (پیام_win == پوچ)
	    {
#endif
		inc_پیام_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		اگر (cmdline_row > 0 && !exmode_active)
		    --cmdline_row;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		اگر (سطرها_چپ > 0)
		    --سطرها_چپ;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif
	    اگر (p_more && سطرها_چپ == 0 && State != MODE_HITRETURN
					    && !پیام_no_more && !exmode_active)
	    {
#ifdef FEAT_CON_DIALOG
		اگر (do_more_prompt(NUL))
		    s = confirm_پیام_tail;
#وگرنه
		(تهی)do_more_prompt(NUL);
#endif
		اگر (quit_more)
		    بازگشت;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    اگر (did_last_char)
		ادامه;
	}

	wrap = *s == '\n'
		    || پیام_col + t_col >= wrap_col
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && پیام_col + t_col >= wrap_col - 1);
	اگر (t_col > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ifdef HAS_MESSAGE_WINDOW
	    اگر (پیام_win != پوچ)
	    {
		put_پیام_win(پیام_win, where, t_s, s, lnum);
		اگر (where == PUT_BELOW)
		    ++lnum;
		t_col = 0;
		where = PUT_BELOW;
		// Reset msg_col after outputting to new line in echowindow
		اگر (wrap)
		    پیام_col = 0;
	    }
	    وگرنه
#endif
		t_puts(&t_col, t_s, s, attr);
	}

	اگر (wrap && p_more && !recurse)
	    // store text for scrolling back
	    store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	اگر (*s == '\n')		    // go to next line
	{
#ifdef HAS_MESSAGE_WINDOW
	    اگر (پیام_win != پوچ)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		اگر ((پیام_win->w_buffer->b_ml.ml_flags & ML_EMPTY) == 0)
		{
		    put_پیام_win(پیام_win, PUT_BELOW, t_s, t_s, lnum);
		    ++lnum;
		}
		پیام_col = 0;  // Reset column for new line
	    }
	    وگرنه
#endif
		پیام_didout = FALSE;	    // remember that line is empty
#ifdef FEAT_RIGHTLEFT
	    اگر (cmdmsg_rl)
		پیام_col = cmdline_width - 1;
	    وگرنه
#endif
		پیام_col = 0;
	    اگر (++پیام_row >= Rows)  // safety check
		پیام_row = Rows - 1;
	}
	وگرنه اگر (*s == '\r')	    // go to column 0
	{
	    پیام_col = 0;
#ifdef HAS_MESSAGE_WINDOW
	    where = PUT_TRUNC;
#endif
	}
	وگرنه اگر (*s == '\b')	    // go to previous char
	{
	    اگر (پیام_col)
		--پیام_col;
	}
	وگرنه اگر (*s == TAB)	    // translate Tab into spaces
	{
#ifdef HAS_MESSAGE_WINDOW
	    اگر (پیام_win != پوچ)
		پیام_col = (پیام_col + 7) % 8;
	    وگرنه
#endif
		انجامبده
		    پیام_screen_putchar(' ', attr);
		وقتی (پیام_col & 7);
	}
	وگرنه اگر (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	وگرنه
	{
	    اگر (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		اگر (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (صحیح)((str + maxlen) - s));
		وگرنه
		    l = (*mb_ptr2len)(s);
	    }
	    وگرنه
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    اگر (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#endif
		    (cw > 1 && پیام_col + t_col >= wrap_col))
	    {
		اگر (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		وگرنه
		    پیام_screen_putchar(*s, attr);
	    }
	    وگرنه
	    {
		// postpone this character until later
		اگر (t_col == 0)
		    t_s = s;
		t_col += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    اگر (t_col > 0)
    {
#ifdef HAS_MESSAGE_WINDOW
	اگر (پیام_win != پوچ)
	    put_پیام_win(پیام_win, where, t_s, s, lnum);
	وگرنه
#endif
	    t_puts(&t_col, t_s, s, attr);
    }

#ifdef HAS_MESSAGE_WINDOW
    اگر (پیام_win != پوچ)
	popup_show_message_win();
#endif
    // Store the text for scroll back.
    اگر (p_more && !recurse)
	store_sb_text(&sb_str, s, attr, &sb_col, FALSE);

    پیام_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    صحیح
message_filtered(char_u *پیام)
{
    صحیح match;

    اگر (cmdmod.cmod_filter_regmatch.regprog == پوچ)
	بازگشت FALSE;
    match = vim_regexec(&cmdmod.cmod_filter_regmatch, پیام, (colnr_T)0);
    بازگشت cmdmod.cmod_filter_force ? match : !match;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    ایستا تهی
پیام_scroll_up(تهی)
{
#ifdef HAS_MESSAGE_WINDOW
    اگر (in_echowindow)
	بازگشت;
#endif
#ifdef FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    اگر (gui.in_use)
	gui_undraw_cursor();
#endif
    // scrolling up always works
    mch_disable_flush();
    screen_del_سطرها(0, 0, 1, (صحیح)Rows, TRUE, 0, پوچ);
    mch_enable_flush();

    اگر (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((صحیح)Rows - 1, (صحیح)Rows, cmdline_col_off,
		cmdline_col_off + cmdline_width, ' ', ' ', HL_ATTR(HLF_پیام));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	اگر (ScreenAttrs[سطر_Offset[Rows - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((صحیح)Rows - 2, (صحیح)Rows - 1,
				 (صحیح)Columns - 1, (صحیح)Columns,
				 ' ', ' ', HL_ATTR(HLF_پیام));
    }
}

/*
 * Increment "msg_scrolled".
 */
    ایستا تهی
inc_پیام_scrolled(تهی)
{
#ifdef FEAT_EVAL
    اگر (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = پوچ;

	// v:scrollstart is empty, set it to the script/function name and line
	//عدد
	اگر (p.string == پوچ)
	{
	    p.string = (char_u *)_("Unknown");
	    p.length = STRLEN(p.string);
	}
	وگرنه
	{
	    size_t  tofreesize;

	    tofreesize = (صحیح)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    اگر (tofree != پوچ)
	    {
		p.length = vim_snprintf_safelen((نویسه *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (long)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	set_vim_var_string(VV_SCROLLSTART, p.string, (صحیح)p.length);
	vim_free(tofree);
    }
#endif
    ++پیام_scrolled;
    set_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
تعریفنوع ساختار msgchunk_S msgchunk_T;
ساختار msgchunk_S
{
    msgchunk_T	*sb_next;
    msgchunk_T	*sb_prev;
    نویسه	sb_eol;		// TRUE when line ends after this text
    صحیح		sb_پیام_col;	// column in which text starts
    صحیح		sb_attr;	// text attributes
    char_u	sb_text[1];	// text to be displayed, actually longer
};

ایستا msgchunk_T *last_msgchunk = پوچ; // last displayed text

ایستا msgchunk_T *پیام_sb_start(msgchunk_T *mps);

تعریفنوع شمارش {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_DONE
} sb_clear_T;

// When to clear text on next msg.
ایستا sb_clear_T do_clear_sb_text = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    ایستا تهی
store_sb_text(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    صحیح		attr,
    صحیح		*sb_col,
    صحیح		finish)		// line ends
{
    msgchunk_T	*mp;

    اگر (do_clear_sb_text == SB_CLEAR_ALL
	    || do_clear_sb_text == SB_CLEAR_CMDLINE_DONE)
    {
	clear_sb_text(do_clear_sb_text == SB_CLEAR_ALL);
	پیام_sb_eol();  // prevent messages from overlapping
	do_clear_sb_text = SB_CLEAR_NONE;
    }

    اگر (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_text) + (s - *sb_str) + 1);
	اگر (mp != پوچ)
	{
	    mp->sb_eol = finish;
	    mp->sb_پیام_col = *sb_col;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_text, *sb_str, s - *sb_str);

	    اگر (last_msgchunk == پوچ)
	    {
		last_msgchunk = mp;
		mp->sb_prev = پوچ;
	    }
	    وگرنه
	    {
		mp->sb_prev = last_msgchunk;
		last_msgchunk->sb_next = mp;
		last_msgchunk = mp;
	    }
	    mp->sb_next = پوچ;
	}
    }
    وگرنه اگر (finish && last_msgchunk != پوچ)
	last_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_col = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    تهی
may_clear_sb_text(تهی)
{
    do_clear_sb_text = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    تهی
sb_text_start_cmdline(تهی)
{
    اگر (do_clear_sb_text == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_text_restart_cmdline();
    وگرنه
    {
	پیام_sb_eol();
	do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    تهی
sb_text_restart_cmdline(تهی)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;

    اگر (last_msgchunk == پوچ || last_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	بازگشت;

    tofree = پیام_sb_start(last_msgchunk);
    last_msgchunk = tofree->sb_prev;
    اگر (last_msgchunk != پوچ)
	last_msgchunk->sb_next = پوچ;
    وقتی (tofree != پوچ)
    {
	msgchunk_T *tofree_next = tofree->sb_next;

	vim_free(tofree);
	tofree = tofree_next;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    تهی
sb_text_end_cmdline(تهی)
{
    do_clear_sb_text = SB_CLEAR_CMDLINE_DONE;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    تهی
clear_sb_text(صحیح all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    اگر (all)
	lastp = &last_msgchunk;
    وگرنه
    {
	اگر (last_msgchunk == پوچ)
	    بازگشت;
	lastp = &پیام_sb_start(last_msgchunk)->sb_prev;
    }

    وقتی (*lastp != پوچ)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    تهی
show_sb_text(تهی)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = پیام_sb_start(last_msgchunk);
    اگر (mp == پوچ || mp->sb_prev == پوچ)
	vim_beep(BO_MESS);
    وگرنه
    {
	do_more_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    ایستا msgchunk_T *
پیام_sb_start(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    وقتی (mp != پوچ && mp->sb_prev != پوچ && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    بازگشت mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    تهی
پیام_sb_eol(تهی)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    اگر (last_msgchunk != پوچ
	    && !(last_msgchunk->sb_text[0] == '\n'
		&& last_msgchunk->sb_text[1] == NUL))
	last_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    ایستا msgchunk_T *
disp_sb_سطر(صحیح row, msgchunk_T *smp, صحیح clear_to_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    برای (;;)
    {
	پیام_row = row;
	پیام_col = mp->sb_پیام_col;
	p = mp->sb_text;
	اگر (*p == '\n')	    // don't display the line break
	    ++p;
	پیام_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	اگر (clear_to_eol)
	    screen_fill(row, row + 1,
		    cmdline_col_off + پیام_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', HL_ATTR(HLF_پیام));

	اگر (mp->sb_eol || mp->sb_next == پوچ)
	    بشکن;
	mp = mp->sb_next;
    }
    بازگشت mp->sb_next;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    ایستا تهی
t_puts(
    صحیح		*t_col,
    char_u	*t_s,
    char_u	*s,
    صحیح		attr)
{
    // output postponed text
    پیام_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (صحیح)(s - t_s), پیام_row, cmdline_col_off + پیام_col,
	    attr);
    پیام_col += *t_col;
    *t_col = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    اگر (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--پیام_col;
    اگر (پیام_col >= cmdline_width)
    {
	پیام_col = 0;
	++پیام_row;
    }
}

/*
 * Returns TRUE when messages should be printed with mch_errmsg().
 * This is used when there is no valid screen, so we can see error messages.
 * If termcap is not active, we may be writing in an alternate console
 * window, cursor positioning may not work correctly (window size may be
 * different, e.g. for Win32 console) or we just don't know where the
 * cursor is.
 */
    صحیح
پیام_use_printf(تهی)
{
    بازگشت (!پیام_check_screen()
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.in_use && !termcap_active)
# وگرنه
	    || !termcap_active
# endif
#endif
	    || (swapping_screen() && !termcap_active)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    ایستا تهی
پیام_puts_printf(char_u *str, صحیح maxlen)
{
    char_u	*s = str;
    char_u	*buf = پوچ;
    char_u	*p = s;

#ifdef MSWIN
    اگر (!(silent_mode && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#endif
    وقتی ((maxlen < 0 || (صحیح)(s - str) < maxlen) && *s != NUL)
    {
	اگر (!(silent_mode && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    اگر (*s == NL)
	    {
		صحیح n = (صحیح)(s - p);

		buf = alloc(n + 3);
		اگر (buf != پوچ)
		{
		    memcpy(buf, p, n);
		    اگر (!info_message)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    اگر (info_message)   // informative message, not an error
			mch_پیام((نویسه *)buf);
		    وگرنه
			mch_errmsg((نویسه *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ifdef FEAT_RIGHTLEFT
	اگر (cmdmsg_rl)
	{
	    اگر (*s == CAR || *s == NL)
		پیام_col = cmdline_width - 1;
	    وگرنه
		--پیام_col;
	}
	وگرنه
#endif
	{
	    اگر (*s == CAR || *s == NL)
		پیام_col = 0;
	    وگرنه
		++پیام_col;
	}
	++s;
    }

    اگر (*p != NUL && !(silent_mode && p_verbose == 0))
    {
	char_u *tofree = پوچ;

	اگر (maxlen > 0 && vim_strlen_maxlen((نویسه *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	اگر (p != پوچ)
	{
	    اگر (info_message)
		mch_پیام((نویسه *)p);
	    وگرنه
		mch_errmsg((نویسه *)p);
	    vim_free(tofree);
	}
    }

    پیام_didout = TRUE;	    // assume that line is not empty

#ifdef MSWIN
    اگر (!(silent_mode && p_verbose == 0))
	mch_settmode(TMODE_RAW);
#endif
}

/*
 * Show the more-prompt and handle the user response.
 * This takes care of scrolling back and displaying previously displayed text.
 * When at hit-enter prompt "typed_char" is the already typed character,
 * otherwise it's NUL.
 * Returns TRUE when jumping ahead to "confirm_msg_tail".
 */
    ایستا صحیح
do_more_prompt(صحیح typed_char)
{
    ایستا صحیح	entered = FALSE;
    صحیح		used_typed_char = typed_char;
    صحیح		oldState = State;
    صحیح		c;
#ifdef FEAT_CON_DIALOG
    صحیح		retval = FALSE;
#endif
    صحیح		toscroll;
    msgchunk_T	*mp_last = پوچ;
    msgchunk_T	*mp;
    صحیح		i;
    صحیح		پیام_attr;

    پیام_attr = HL_ATTR(HLF_پیام);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    اگر (entered || (State == MODE_HITRETURN && typed_char == 0))
	بازگشت FALSE;
    entered = TRUE;

    اگر (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_last = پیام_sb_start(last_msgchunk);
	برای (i = 0; i < Rows - 2 && mp_last != پوچ
					     && mp_last->sb_prev != پوچ; ++i)
	    mp_last = پیام_sb_start(mp_last->sb_prev);
    }

    State = MODE_ASKMORE;
    setmouse();
    اگر (typed_char == NUL)
	پیام_moremsg(FALSE);
    برای (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	اگر (used_typed_char != NUL)
	{
	    c = used_typed_char;	// was typed at hit-enter prompt
	    used_typed_char = NUL;
	}
	وگرنه
	    c = get_keystroke();

#اگر defined(FEAT_MENU) && defined(FEAT_GUI)
	اگر (c == K_MENU)
	{
	    صحیح idx = get_menu_index(current_menu, MODE_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    اگر (idx == MENU_INDEX_INVALID)
		ادامه;
	    c = *current_menu->strings[idx];
	    اگر (c != NUL && current_menu->strings[idx][1] != NUL)
		ins_typebuf(current_menu->strings[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#endif

	toscroll = 0;
	انتخاب (c)
	{
	حالت BS:		// scroll one line back
	حالت K_BS:
	حالت 'k':
	حالت K_UP:
	    toscroll = -1;
	    بشکن;

	حالت CAR:		// one extra line
	حالت NL:
	حالت 'j':
	حالت K_DOWN:
	    toscroll = 1;
	    بشکن;

	حالت 'u':		// Up half a page
	    toscroll = -(Rows / 2);
	    بشکن;

	حالت 'd':		// Down half a page
	    toscroll = Rows / 2;
	    بشکن;

	حالت 'b':		// one page back
	حالت Ctrl_B:
	حالت K_PAGEUP:
	    toscroll = -(Rows - 1);
	    بشکن;

	حالت ' ':		// one extra page
	حالت 'f':
	حالت Ctrl_F:
	حالت K_PAGEDOWN:
	حالت K_LEFTMOUSE:
	    toscroll = Rows - 1;
	    بشکن;

	حالت 'g':		// all the way back to the start
	    toscroll = -999999;
	    بشکن;

	حالت 'G':		// all the way to the end
	    toscroll = 999999;
	    سطرها_چپ = 999999;
	    بشکن;

	حالت ':':		// start new command line
#ifdef FEAT_CON_DIALOG
	    اگر (!confirm_پیام_used)
#endif
	    {
		// Since got_int is set all typeahead will be flushed, but we
		// want to keep this ':', remember that in a special way.
		typeahead_noflush(':');
#ifdef FEAT_TERMINAL
		skip_term_loop = TRUE;
#endif
		cmdline_row = Rows - 1;		// put ':' on this line
		skip_redraw = TRUE;		// skip redraw once
		need_wait_return = FALSE;	// don't wait in main()
	    }
	    // FALLTHROUGH
	حالت 'q':		// quit
	حالت Ctrl_C:
	حالت ESC:
#ifdef FEAT_CON_DIALOG
	    اگر (confirm_پیام_used)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    وگرنه
#endif
	    {
		got_int = TRUE;
		quit_more = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    سطرها_چپ = Rows - 1;
	    بشکن;

#ifdef FEAT_CLIPBOARD
	حالت Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    اگر (clip_star.state == SELECT_DONE)
		clip_copy_modeless_selection(TRUE);
	    ادامه;
#endif
	پیشفرض:		// no valid response
	    پیام_moremsg(TRUE);
	    ادامه;
	}

	اگر (toscroll != 0)
	{
	    اگر (toscroll < 0)
	    {
		// go to start of last line
		اگر (mp_last == پوچ)
		    mp = پیام_sb_start(last_msgchunk);
		وگرنه اگر (mp_last->sb_prev != پوچ)
		    mp = پیام_sb_start(mp_last->sb_prev);
		وگرنه
		    mp = پوچ;

		// go to start of line at top of the screen
		برای (i = 0; i < Rows - 2 && mp != پوچ && mp->sb_prev != پوچ;
									  ++i)
		    mp = پیام_sb_start(mp->sb_prev);

		اگر (mp != پوچ && mp->sb_prev != پوچ)
		{
		    // Find line to be displayed at top.
		    برای (i = 0; i > toscroll; --i)
		    {
			اگر (mp == پوچ || mp->sb_prev == پوچ)
			    بشکن;
			mp = پیام_sb_start(mp->sb_prev);
			اگر (mp_last == پوچ)
			    mp_last = پیام_sb_start(last_msgchunk);
			وگرنه
			    mp_last = پیام_sb_start(mp_last->sb_prev);
		    }

		    اگر (toscroll == -1 && screen_ins_سطرها(0, 0, 1,
						     (صحیح)Rows, 0, پوچ) == OK)
		    {
			// display line at top
			(تهی)disp_sb_سطر(0, mp, FALSE);
		    }
		    وگرنه
		    {
			صحیح did_clear = screenclear();

			// redisplay all lines
			برای (i = 0; mp != پوچ && i < Rows - 1; ++i)
			{
			    mp = disp_sb_سطر(i, mp, !did_clear);
			    ++پیام_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    وگرنه
	    {
		// First display any text that we scrolled back.
		وقتی (toscroll > 0 && mp_last != پوچ)
		{
		    // scroll up, display line at bottom
		    پیام_scroll_up();
		    inc_پیام_scrolled();
		    screen_fill((صحیح)Rows - 2, (صحیح)Rows - 1,
			    cmdline_col_off, cmdline_col_off + cmdline_width,
			    ' ', ' ', پیام_attr);
		    mp_last = disp_sb_سطر((صحیح)Rows - 2, mp_last, FALSE);
		    --toscroll;
		}
	    }

	    اگر (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((صحیح)Rows - 1, (صحیح)Rows, cmdline_col_off,
			cmdline_col_off + cmdline_width, ' ', ' ', پیام_attr);
		پیام_moremsg(FALSE);
		ادامه;
	    }

	    // display more text, return to caller
	    سطرها_چپ = toscroll;
	}

	بشکن;
    }

    // clear the --more-- message
    screen_fill((صحیح)Rows - 1, (صحیح)Rows, cmdline_col_off,
	    cmdline_col_off + cmdline_width, ' ', ' ', پیام_attr);
    State = oldState;
    setmouse();
    اگر (quit_more)
    {
	پیام_row = Rows - 1;
	پیام_col = 0;
    }
#ifdef FEAT_RIGHTLEFT
    وگرنه اگر (cmdmsg_rl)
	پیام_col = cmdline_width - 1;
#endif

    entered = FALSE;
#ifdef FEAT_CON_DIALOG
    بازگشت retval;
#وگرنه
    بازگشت FALSE;
#endif
}

#اگر defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_پیام
#  undef mch_پیام
# endif

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ایستا تهی
mch_errmsg_c(نویسه *str)
{
    DWORD   nwrite = 0;
    DWORD   mode = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    اگر (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (صحیح)GetConsoleCP() != enc_codepage)
    {
	صحیح	len = (صحیح)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	اگر (w != پوچ)
	{
	    WriteConsoleW(h, w, len, &nwrite, پوچ);
	    vim_free(w);
	}
    }
    وگرنه
    {
	fprintf(stderr, "%s", str);
    }
}
# endif

/*
 * Give an error message.  To be used when the screen hasn't been initialized
 * yet.  When stderr can't be used, collect error messages until the GUI has
 * started and they can be displayed in a message box.
 */
    تهی
mch_errmsg(نویسه *str)
{
# اگر !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    صحیح		len;
# endif

# اگر (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    اگر (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   وگرنه
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.in_use || gui.starting)
#  endif
	    )
    {
	fprintf(stderr, "%s", str);
	بازگشت;
    }
# endif

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    اگر (!(gui.in_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	بازگشت;
    }
# endif

# اگر !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (صحیح)STRLEN(str) + 1;
    اگر (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    اگر (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_data + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_data + error_ga.ga_len;
	    برای (;;)
	    {
		p = vim_strchr(p, '\r');
		اگر (p == پوچ)
		    بشکن;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ایستا تهی
mch_پیام_c(نویسه *str)
{
    DWORD   nwrite = 0;
    DWORD   mode;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    اگر (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (صحیح)GetConsoleCP() != enc_codepage)
    {
	صحیح	len = (صحیح)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	اگر (w != پوچ)
	{
	    WriteConsoleW(h, w, len, &nwrite, پوچ);
	    vim_free(w);
	}
    }
    وگرنه
    {
	printf("%s", str);
    }
}
# endif

/*
 * Give a message.  To be used when the screen hasn't been initialized yet.
 * When there is no tty, collect messages until the GUI has started and they
 * can be displayed in a message box.
 */
    تهی
mch_پیام(نویسه *str)
{
# اگر (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    اگر (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   وگرنه
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.in_use || gui.starting)
#  endif
	    )
    {
	printf("%s", str);
	بازگشت;
    }
# endif

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    اگر (!(gui.in_use || gui.starting))
#  endif
    {
	mch_پیام_c(str);
	بازگشت;
    }
# endif
# اگر !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#endif // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    ایستا تهی
پیام_screen_putchar(صحیح c, صحیح attr)
{
    پیام_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, پیام_row, cmdline_col_off + پیام_col, attr);
#ifdef FEAT_RIGHTLEFT
    اگر (cmdmsg_rl)
    {
	اگر (--پیام_col == 0)
	{
	    پیام_col = cmdline_width;
	    ++پیام_row;
	}
    }
    وگرنه
#endif
    {
	اگر (++پیام_col >= cmdline_width)
	{
	    پیام_col = 0;
	    ++پیام_row;
	}
    }
}

    ایستا تهی
پیام_moremsg(صحیح full)
{
    صحیح		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (صحیح)Rows - 1, cmdline_col_off, attr);
    اگر (full)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(صحیح)Rows - 1, cmdline_col_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    تهی
repeat_message(تهی)
{
    اگر (State == MODE_ASKMORE)
    {
	پیام_moremsg(TRUE);	// display --more-- message again
	پیام_row = Rows - 1;
    }
#ifdef FEAT_CON_DIALOG
    وگرنه اگر (State == MODE_CONFIRM)
    {
	display_confirm_پیام();	// display ":confirm" message again
	پیام_row = Rows - 1;
    }
#endif
    وگرنه اگر (State == MODE_EXTERNCMD)
    {
	windgoto(پیام_row, cmdline_col_off + پیام_col); // put cursor back
    }
    وگرنه اگر (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	اگر (پیام_row == Rows - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    پیام_didout = FALSE;
	    پیام_col = 0;
	    پیام_clr_eos();
	}
	hit_return_پیام();
	پیام_row = Rows - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    ایستا صحیح
پیام_check_screen(تهی)
{
    اگر (!full_screen || !screen_valid(FALSE))
	بازگشت FALSE;

    اگر (پیام_row >= Rows)
	پیام_row = Rows - 1;
    اگر (پیام_col >= cmdline_width)
	پیام_col = cmdline_width - 1;
    بازگشت TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    تهی
پیام_clr_eos(تهی)
{
    اگر (پیام_silent == 0)
	پیام_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    تهی
پیام_clr_eos_force(تهی)
{
#ifdef HAS_MESSAGE_WINDOW
    اگر (in_echowindow)
	بازگشت;  // messages go into a popup
#endif
    اگر (پیام_use_printf())
    {
	اگر (full_screen)	// only when termcap codes are valid
	{
	    اگر (*T_CD)
		out_str(T_CD);	// clear to end of display
	    وگرنه اگر (*T_CE)
		out_str(T_CE);	// clear to end of line
	}
    }
    وگرنه
    {
	صحیح پیام_attr;

	پیام_attr = HL_ATTR(HLF_پیام);

#ifdef FEAT_RIGHTLEFT
	اگر (cmdmsg_rl)
	{
	    screen_fill(پیام_row, پیام_row + 1,
		    cmdline_col_off, cmdline_col_off + پیام_col + 1,
		    ' ', ' ', پیام_attr);
	    screen_fill(پیام_row + 1, (صحیح)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', پیام_attr);
	}
	وگرنه
#endif
	{
	    screen_fill(پیام_row, پیام_row + 1,
		    cmdline_col_off + پیام_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', پیام_attr);
	    screen_fill(پیام_row + 1, (صحیح)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', پیام_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    تهی
پیام_clr_cmdline(تهی)
{
    پیام_row = cmdline_row;
    پیام_col = 0;
    پیام_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    صحیح
پیام_end(تهی)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    اگر (!exiting && need_wait_return && !(State & MODE_CMDLINE))
    {
	wait_return(FALSE);
	بازگشت FALSE;
    }
    out_flush();
    بازگشت TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    تهی
پیام_check(تهی)
{
    اگر (پیام_row == Rows - 1 && پیام_col >= sc_col
#ifdef HAS_MESSAGE_WINDOW
		&& !in_echowindow
#endif
	    )
    {
	need_wait_return = TRUE;
	redraw_cmdline = TRUE;
    }
}

/*
 * May write a string to the redirection file.
 * When "maxlen" is -1 write the whole string, otherwise up to "maxlen" bytes.
 */
    ایستا تهی
redir_write(char_u *str, صحیح maxlen)
{
    char_u	*s = str;
    ایستا صحیح	cur_col = 0;

    // Don't do anything for displaying prompts and the like.
    اگر (redir_off)
	بازگشت;

    // If 'verbosefile' is set prepare for writing in that file.
    اگر (*p_vfile != NUL && verbose_fd == پوچ)
	verbose_open();

    اگر (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	اگر (*s != '\n' && *s != '\r')
	{
	    وقتی (cur_col < پیام_col)
	    {
#ifdef FEAT_EVAL
		اگر (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		وگرنه اگر (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		وگرنه اگر (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		وگرنه
#endif
		    اگر (redir_fd != پوچ)
		    fputs(" ", redir_fd);
		اگر (verbose_fd != پوچ)
		    fputs(" ", verbose_fd);
		++cur_col;
	    }
	}

#ifdef FEAT_EVAL
	اگر (redir_execute)
	    execute_redir_str(s, maxlen);
	وگرنه اگر (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	وگرنه اگر (redir_vname)
	    var_redir_str(s, maxlen);
#endif

	// Write and adjust the current column.
	وقتی (*s != NUL && (maxlen < 0 || (صحیح)(s - str) < maxlen))
	{
#ifdef FEAT_EVAL
	    اگر (!redir_reg && !redir_vname && !redir_execute)
#endif
		اگر (redir_fd != پوچ)
		    putc(*s, redir_fd);
	    اگر (verbose_fd != پوچ)
		putc(*s, verbose_fd);
	    اگر (*s == '\r' || *s == '\n')
		cur_col = 0;
	    وگرنه اگر (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    وگرنه
		++cur_col;
	    ++s;
	}

	اگر (پیام_silent != 0)	// should update msg_col
	    پیام_col = cur_col;
    }
}

    صحیح
redirecting(تهی)
{
    بازگشت redir_fd != پوچ || *p_vfile != NUL
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#endif
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    تهی
verbose_enter(تهی)
{
    اگر (*p_vfile != NUL)
	++پیام_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    تهی
verbose_leave(تهی)
{
    اگر (*p_vfile != NUL)
	اگر (--پیام_silent < 0)
	    پیام_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    تهی
verbose_enter_scroll(تهی)
{
    اگر (*p_vfile != NUL)
	++پیام_silent;
    وگرنه
	// always scroll up, don't overwrite
	پیام_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    تهی
verbose_leave_scroll(تهی)
{
    اگر (*p_vfile != NUL)
    {
	اگر (--پیام_silent < 0)
	    پیام_silent = 0;
    }
    وگرنه
	cmdline_row = پیام_row;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    تهی
verbose_stop(تهی)
{
    اگر (verbose_fd != پوچ)
    {
	fclose(verbose_fd);
	verbose_fd = پوچ;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    صحیح
verbose_open(تهی)
{
    اگر (verbose_fd == پوچ && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((نویسه *)p_vfile, "a");
	اگر (verbose_fd == پوچ)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    بازگشت FAIL;
	}
    }
    بازگشت OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    تهی
give_warning(char_u *message, صحیح hl)
{
    give_warning_with_source(message, hl, FALSE);
}

    تهی
give_warning_with_source(char_u *message, صحیح hl, صحیح with_source)
{
    // Don't do this for ":silent".
    اگر (پیام_silent != 0)
	بازگشت;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, message, -1);
#endif
    VIM_CLEAR(keep_پیام);
    اگر (hl)
	keep_پیام_attr = HL_ATTR(HLF_W);
    وگرنه
	keep_پیام_attr = 0;

    اگر (with_source)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	پیام_start();
	پیام_source(HL_ATTR(HLF_W));
	پیام_puts(" ");
	پیام_puts_attr((نویسه *)message, HL_ATTR(HLF_W) | پیام_HIST);
	پیام_clr_eos();
	(تهی)پیام_end();
    }
    وگرنه اگر (پیام_attr((نویسه *)message, keep_پیام_attr) && پیام_scrolled == 0)
	set_keep_پیام(message, keep_پیام_attr);

    پیام_didout = FALSE;	    // overwrite this message
    پیام_nowait = TRUE;	    // don't wait for this message
    پیام_col = 0;

    --no_wait_return;
}

#اگر defined(FEAT_EVAL)
    تهی
give_warning2(char_u *message, char_u *a1, صحیح hl)
{
    اگر (IObuff == پوچ)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(message, hl);
    }
    وگرنه
    {
	vim_snprintf((نویسه *)IObuff, IOSIZE, (نویسه *)message, a1);
	give_warning(IObuff, hl);
    }
}
#endif

/*
 * Advance msg cursor to column "col".
 */
    تهی
پیام_advance(صحیح col)
{
    اگر (پیام_silent != 0)	// nothing to advance to
    {
	پیام_col = col;		// for redirection, may fill it up later
	بازگشت;
    }
    اگر (col >= cmdline_width)	// not enough room
	col = cmdline_width - 1;
#ifdef FEAT_RIGHTLEFT
    اگر (cmdmsg_rl)
	وقتی (پیام_col > cmdline_width - col)
	    پیام_putchar(' ');
    وگرنه
#endif
	وقتی (پیام_col < col)
	    پیام_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    تهی
پیام_warn_missing_clipboard(تهی)
{
    اگر (!global_busy && !did_warn_clipboard && silence_w23_w24_پیام == 0)
    {
#ifdef FEAT_CLIPBOARD
	پیام(_("W23: Clipboard register not available, using register 0"));
#وگرنه
	پیام(_("W24: Clipboard register not available. See :h W24"));
#endif
	did_warn_clipboard = TRUE;
    }
}

#اگر defined(FEAT_CON_DIALOG)
/*
 * Used for "confirm()" function, and the :confirm command prefix.
 * Versions which haven't got flexible dialogs yet, and console
 * versions, get this generic handler which uses the command line.
 *
 * type  = one of:
 *	   VIM_QUESTION, VIM_INFO, VIM_WARNING, VIM_ERROR or VIM_GENERIC
 * title = title string (can be NULL for default)
 * (neither used in console dialogs at the moment)
 *
 * Format of the "buttons" string:
 * "Button1Name\nButton2Name\nButton3Name"
 * The first button should normally be the default/accept
 * The second button should be the 'Cancel' button
 * Other buttons- use your imagination!
 * A '&' in a button name becomes a shortcut, so each '&' should be before a
 * different letter.
 *
 * Returns 0 if cancelled, otherwise the nth button (1-indexed).
 */
    صحیح
do_dialog(
    صحیح		type UNUSED,
    char_u	*title UNUSED,
    char_u	*message,
    char_u	*buttons,
    صحیح		dfltbutton,
    char_u	*textfield UNUSED,	// IObuff for inputdialog(), NULL
					// otherwise
    صحیح		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    صحیح		oldState;
    صحیح		retval = 0;
    char_u	*hotkeys;
    صحیح		c;
    صحیح		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    اگر (silent_mode)
	بازگشت dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    اگر (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == پوچ)
    {
	// --gui-dialog-file: write text to a file
	اگر (gui_dialog_log(title, message))
	    c = dfltbutton;
	وگرنه
	    c = gui_mch_dialog(type, title, message, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_row = پیام_row;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	out_flush();
	gui_mch_update();

	بازگشت c;
    }
# endif

    oldState = State;
    State = MODE_CONFIRM;
    setmouse();

    // Ensure raw mode here.
    save_tmode = cur_tmode;
    settmode(TMODE_RAW);

    /*
     * Since we wait for a keypress, don't make the
     * user press RETURN as well afterwards.
     */
    ++no_wait_return;
    hotkeys = پیام_show_console_dialog(message, buttons, dfltbutton);

    اگر (hotkeys != پوچ)
    {
	برای (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    انتخاب (c)
	    {
	    حالت CAR:		// User accepts default option
	    حالت NL:
		retval = dfltbutton;
		بشکن;
	    حالت Ctrl_C:	// User aborts/cancels
	    حالت ESC:
		retval = 0;
		بشکن;
	    پیشفرض:		// Could be a hotkey?
		اگر (c < 0)	// special keys are ignored here
		    ادامه;
		اگر (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    بشکن;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		برای (i = 0; hotkeys[i]; ++i)
		{
		    اگر (has_mbyte)
		    {
			اگر ((*mb_ptr2char)(hotkeys + i) == c)
			    بشکن;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    وگرنه
			اگر (hotkeys[i] == c)
			    بشکن;
		    ++retval;
		}
		اگر (hotkeys[i])
		    بشکن;
		// No hotkey match, so keep waiting
		ادامه;
	    }
	    بشکن;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    پیام_end_prompt();

    بازگشت retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    ایستا صحیح
copy_char(
    char_u	*from,
    char_u	*to,
    صحیح		lowercase)	// make character lower case
{
    صحیح		len;
    صحیح		c;

    اگر (has_mbyte)
    {
	اگر (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    بازگشت (*mb_char2bytes)(c, to);
	}
	وگرنه
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(to, from, (size_t)len);
	    بازگشت len;
	}
    }
    وگرنه
    {
	اگر (lowercase)
	    *to = (char_u)TOLOWER_LOC(*from);
	وگرنه
	    *to = *from;
	بازگشت 1;
    }
}

/*
 * Format the dialog string, and display it at the bottom of
 * the screen. Return a string of hotkey chars (if defined) for
 * each 'button'. If a button has no hotkey defined, the first character of
 * the button is used.
 * The hotkeys can be multi-byte characters, but without combining chars.
 *
 * Returns an allocated string with hotkeys, or NULL for error.
 */
    ایستا char_u *
پیام_show_console_dialog(
    char_u	*message,
    char_u	*buttons,
    صحیح		dfltbutton)
{
    صحیح		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    صحیح		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = پوچ;
    char_u	*msgp = پوچ;
    char_u	*hotkp = پوچ;
    char_u	*r;
    صحیح		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    صحیح		first_hotkey = FALSE;	// first char of button is hotkey
    صحیح		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    برای (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	وقتی (*r)
	{
	    اگر (*r == DLG_BUTTON_SEP)
	    {
		اگر (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    اگر (has_mbyte)
			hotkp += STRLEN(hotkp);
		    وگرنه
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    اگر (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    اگر (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			first_hotkey = TRUE;
		}
		وگرنه
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    اگر (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    وگرنه اگر (*r == DLG_HOTKEY_CHAR || first_hotkey)
	    {
		اگر (*r == DLG_HOTKEY_CHAR)
		    ++r;
		first_hotkey = FALSE;
		اگر (copy)
		{
		    اگر (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    وگرنه
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		وگرنه
		{
		    ++len;	    // '&a' -> '[a]'
		    اگر (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    وگرنه
	    {
		// everything else copy literally
		اگر (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	اگر (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	وگرنه
	{
	    len += (صحیح)(STRLEN(message)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    اگر (!has_hotkey[0])
	    {
		first_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_پیام);
	    confirm_پیام = alloc(len);
	    اگر (confirm_پیام == پوچ)
		بازگشت پوچ;
	    *confirm_پیام = NUL;
	    hotk = alloc(lenhotkey);
	    اگر (hotk == پوچ)
		بازگشت پوچ;

	    *confirm_پیام = '\n';
	    STRCPY(confirm_پیام + 1, message);

	    msgp = confirm_پیام + 1 + STRLEN(message);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_پیام_tail = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_پیام();
    بازگشت hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    ایستا تهی
display_confirm_پیام(تهی)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_پیام_used;
    اگر (confirm_پیام != پوچ)
	پیام_puts_attr((نویسه *)confirm_پیام, HL_ATTR(HLF_M));
    --confirm_پیام_used;
}

#endif // FEAT_CON_DIALOG

#اگر defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    صحیح
vim_dialog_yesno(
    صحیح		type,
    char_u	*title,
    char_u	*message,
    صحیح		dflt)
{
    اگر (do_dialog(type,
		title == پوچ ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No"), dflt, پوچ, FALSE) == 1)
	بازگشت VIM_YES;
    بازگشت VIM_NO;
}

    صحیح
vim_dialog_yesnocancel(
    صحیح		type,
    char_u	*title,
    char_u	*message,
    صحیح		dflt)
{
    انتخاب (do_dialog(type,
		title == پوچ ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, پوچ, FALSE))
    {
	حالت 1: بازگشت VIM_YES;
	حالت 2: بازگشت VIM_NO;
    }
    بازگشت VIM_CANCEL;
}

    صحیح
vim_dialog_yesnoallcancel(
    صحیح		type,
    char_u	*title,
    char_u	*message,
    صحیح		dflt)
{
    انتخاب (do_dialog(type,
		title == پوچ ? (char_u *)"Question" : title,
		message,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, پوچ, FALSE))
    {
	حالت 1: بازگشت VIM_YES;
	حالت 2: بازگشت VIM_NO;
	حالت 3: بازگشت VIM_ALL;
	حالت 4: بازگشت VIM_DISCARDALL;
    }
    بازگشت VIM_CANCEL;
}

#endif // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
