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

#অন্তর্ভাব "vim.h"

স্থির শূন্য add_বার্তা_hist(char_u *s, পূর্ণাঙ্ক len, পূর্ণাঙ্ক attr);
স্থির শূন্য check_বার্তা_hist(শূন্য);
স্থির শূন্য hit_return_বার্তা(শূন্য);
স্থির শূন্য বার্তা_home_replace_attr(char_u *fname, পূর্ণাঙ্ক attr);
স্থির শূন্য বার্তা_puts_attr_len(বর্ণ *str, পূর্ণাঙ্ক maxlen, পূর্ণাঙ্ক attr);
স্থির শূন্য বার্তা_puts_display(char_u *str, পূর্ণাঙ্ক maxlen, পূর্ণাঙ্ক attr, পূর্ণাঙ্ক recurse);
স্থির শূন্য বার্তা_scroll_up(শূন্য);
স্থির শূন্য inc_বার্তা_scrolled(শূন্য);
স্থির শূন্য store_sb_text(char_u **sb_str, char_u *s, পূর্ণাঙ্ক attr, পূর্ণাঙ্ক *sb_col, পূর্ণাঙ্ক finish);
স্থির শূন্য t_puts(পূর্ণাঙ্ক *t_col, char_u *t_s, char_u *s, পূর্ণাঙ্ক attr);
স্থির শূন্য বার্তা_puts_printf(char_u *str, পূর্ণাঙ্ক maxlen);
স্থির পূর্ণাঙ্ক do_more_prompt(পূর্ণাঙ্ক typed_char);
স্থির শূন্য বার্তা_screen_putchar(পূর্ণাঙ্ক c, পূর্ণাঙ্ক attr);
স্থির শূন্য বার্তা_moremsg(পূর্ণাঙ্ক full);
স্থির পূর্ণাঙ্ক  বার্তা_check_screen(শূন্য);
স্থির শূন্য redir_write(char_u *s, পূর্ণাঙ্ক maxlen);
#ifdef FEAT_CON_DIALOG
স্থির char_u *বার্তা_show_console_dialog(char_u *message, char_u *buttons, পূর্ণাঙ্ক dfltbutton);
স্থির পূর্ণাঙ্ক	confirm_বার্তা_used = FALSE;	// displaying confirm_msg
স্থির char_u	*confirm_বার্তা = রিক্ত;		// ":confirm" message
স্থির char_u	*confirm_বার্তা_tail;		// tail of confirm_msg
স্থির শূন্য display_confirm_বার্তা(শূন্য);
#endif
#ifdef FEAT_EVAL
স্থির পূর্ণাঙ্ক emsg_to_channel_log = FALSE;
#endif

সংরচনা বার্তা_hist
{
    সংরচনা বার্তা_hist	*next;
    char_u		*বার্তা;
    পূর্ণাঙ্ক			attr;
};

স্থির সংরচনা বার্তা_hist *first_বার্তা_hist = রিক্ত;
স্থির সংরচনা বার্তা_hist *last_বার্তা_hist = রিক্ত;
স্থির পূর্ণাঙ্ক বার্তা_hist_len = 0;
স্থির পূর্ণাঙ্ক বার্তা_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#define MESSAGES_HIT_ENTER	0x001
#define MESSAGES_WAIT		0x002
#define MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#define MESSAGES_OPT_HIT_ENTER "hit-enter"
#define MESSAGES_OPT_WAIT "wait:"
#define MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
স্থির পূর্ণাঙ্ক বার্তা_flags = MESSAGES_HIT_ENTER | MESSAGES_HISTORY;
স্থির পূর্ণাঙ্ক বার্তা_wait = 0;

স্থির FILE *verbose_fd = রিক্ত;
স্থির পূর্ণাঙ্ক  verbose_did_open = FALSE;

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
    পূর্ণাঙ্ক
বার্তা(বর্ণ *s)
{
    প্রত্যাগম বার্তা_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    পূর্ণাঙ্ক
verb_বার্তা(বর্ণ *s)
{
    পূর্ণাঙ্ক		n;

    verbose_enter();
    n = বার্তা_attr_keep(s, 0, FALSE);
    verbose_leave();

    প্রত্যাগম n;
}

    পূর্ণাঙ্ক
বার্তা_attr(বর্ণ *s, পূর্ণাঙ্ক attr)
{
    প্রত্যাগম বার্তা_attr_keep(s, attr, FALSE);
}

    পূর্ণাঙ্ক
বার্তা_attr_keep(
    বর্ণ	*s,
    পূর্ণাঙ্ক		attr,
    পূর্ণাঙ্ক		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    স্থির পূর্ণাঙ্ক	entered = 0;
    পূর্ণাঙ্ক		retval;
    char_u	*buf = রিক্ত;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    যদি (!emsg_on_display && message_filtered((char_u *)s))
	প্রত্যাগম TRUE;

#ifdef FEAT_EVAL
    যদি (attr == 0)
	set_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#endif

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    যদি (entered >= 3)
	প্রত্যাগম TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    যদি ((char_u *)s != keep_বার্তা
	    || (*s != '<'
		&& last_বার্তা_hist != রিক্ত
		&& last_বার্তা_hist->বার্তা != রিক্ত
		&& STRCMP(s, last_বার্তা_hist->বার্তা) != 0))
	add_বার্তা_hist((char_u *)s, -1, attr);

#ifdef FEAT_EVAL
    যদি (emsg_to_channel_log)
	// Write message in the channel log.
	ch_log(রিক্ত, "ERROR: %s", s);
#endif

    // Truncate the message if needed.
    বার্তা_start();
    buf = বার্তা_strtrunc((char_u *)s, FALSE);
    যদি (buf != রিক্ত)
	s = (বর্ণ *)buf;

    বার্তা_outtrans_attr((char_u *)s, attr);
    বার্তা_clr_eos();
    retval = বার্তা_end();

    যদি (keep && retval && vim_strsize((char_u *)s)
		    < (পূর্ণাঙ্ক)(Rows - cmdline_row - 1) * cmdline_width + sc_col)
	set_keep_বার্তা((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    প্রত্যাগম retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
বার্তা_strtrunc(
    char_u	*s,
    পূর্ণাঙ্ক		force)	    // always truncate
{
    char_u	*buf = রিক্ত;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		room;

    // May truncate message to avoid a hit-return prompt
    যদি ((!বার্তা_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_active && বার্তা_silent == 0) || force)
    {
	len = vim_strsize(s);
	যদি (বার্তা_scrolled != 0
#ifdef HAS_MESSAGE_WINDOW
		|| in_echowindow
#endif
		)
	    // Use all the columns.
	    room = (পূর্ণাঙ্ক)(Rows - বার্তা_row) * cmdline_width - 1;
	অন্যথা
	    // Use up to 'showcmd' column.
	    room = (পূর্ণাঙ্ক)(Rows - বার্তা_row - 1) * cmdline_width + sc_col - 1;
	যদি (len > room && room > 0)
	{
	    যদি (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (room + 2) * 18;
	    অন্যথা যদি (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (room + 2) * 2;
	    অন্যথা
		len = room + 2;
	    buf = alloc(len);
	    যদি (buf != রিক্ত)
		trunc_string(s, buf, room, len);
	}
    }
    প্রত্যাগম buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    শূন্য
trunc_string(
    char_u	*s,
    char_u	*buf,
    পূর্ণাঙ্ক		room_in,
    পূর্ণাঙ্ক		buflen)
{
    size_t	room = room_in - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    পূর্ণাঙ্ক		e;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		n;

    যদি (*s == NUL)
    {
	যদি (buflen > 0)
	    *buf = NUL;
	প্রত্যাগম;
    }

    যদি (room_in < 3)
	room = 0;
    half = room / 2;

    // First part: Start of the string.
    প্রতি (e = 0; len < half && e < buflen; ++e)
    {
	যদি (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    প্রত্যাগম;
	}
	n = ptr2cells(s + e);
	যদি (len + n > half)
	    ভঙ্গ;
	len += n;
	buf[e] = s[e];
	যদি (has_mbyte)
	    প্রতি (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		যদি (++e == buflen)
		    ভঙ্গ;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    যদি (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	যাবত্ (len + n > room)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    অন্যথা যদি (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (পূর্ণাঙ্ক)STRLEN(s);
	প্রতি (;;)
	{
	    কুরু
		half = half - utf_head_off(s, s + half - 1) - 1;
	    যাবত্ (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    যদি (len + n > room || half == 0)
		ভঙ্গ;
	    len += n;
	    i = (পূর্ণাঙ্ক)half;
	}
    }
    অন্যথা
    {
	প্রতি (i = (পূর্ণাঙ্ক)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= room; --i)
	    len += n;
    }


    যদি (i <= e + 3)
    {
	// text fits without truncating
	যদি (s != buf)
	{
	    len = STRLEN(s);
	    যদি (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    যদি (len < 1)
		buf[e - 1] = NUL;
	    অন্যথা
		mch_memmove(buf + e, s + e, len);
	}
    }
    অন্যথা যদি (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	যদি (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    অন্যথা
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

পূর্ণাঙ্ক vim_snprintf(বর্ণ *str, size_t str_m, ধ্রুব বর্ণ *fmt, ...);

    পূর্ণাঙ্ক
smsg(ধ্রুব বর্ণ *s, ...)
{
    যদি (IObuff == রিক্ত)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	প্রত্যাগম বার্তা((বর্ণ *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((বর্ণ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    প্রত্যাগম বার্তা((বর্ণ *)IObuff);
}

    পূর্ণাঙ্ক
smsg_attr(পূর্ণাঙ্ক attr, ধ্রুব বর্ণ *s, ...)
{
    যদি (IObuff == রিক্ত)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	প্রত্যাগম বার্তা_attr((বর্ণ *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((বর্ণ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    প্রত্যাগম বার্তা_attr((বর্ণ *)IObuff, attr);
}

    পূর্ণাঙ্ক
smsg_attr_keep(পূর্ণাঙ্ক attr, ধ্রুব বর্ণ *s, ...)
{
    যদি (IObuff == রিক্ত)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	প্রত্যাগম বার্তা_attr_keep((বর্ণ *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((বর্ণ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    প্রত্যাগম বার্তা_attr_keep((বর্ণ *)IObuff, attr, TRUE);
}

#endif

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
স্থির পূর্ণাঙ্ক	last_sourcing_lnum = 0;
স্থির char_u   *last_sourcing_name = রিক্ত;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    শূন্য
reset_last_sourcing(শূন্য)
{
    VIM_CLEAR(last_sourcing_name);
    last_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    স্থির পূর্ণাঙ্ক
other_sourcing_name(শূন্য)
{
    যদি (HAVE_SOURCING_INFO && SOURCING_NAME != রিক্ত)
    {
	যদি (last_sourcing_name != রিক্ত)
	    প্রত্যাগম STRCMP(SOURCING_NAME, last_sourcing_name) != 0;
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    স্থির char_u *
get_emsg_source(শূন্য)
{
    char_u	*Buf, *p;

    যদি (HAVE_SOURCING_INFO && SOURCING_NAME != রিক্ত && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	যদি (sname == রিক্ত)
	    sname = SOURCING_NAME;

#ifdef FEAT_EVAL
	যদি (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	অন্যথা
#endif
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	যদি (Buf != রিক্ত)
	    sprintf((বর্ণ *)Buf, (বর্ণ *)p, sname);
	vim_free(tofree);
	প্রত্যাগম Buf;
    }
    প্রত্যাগম রিক্ত;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    স্থির char_u *
get_emsg_lnum(শূন্য)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    যদি (SOURCING_NAME != রিক্ত
	    && (other_sourcing_name() || SOURCING_LNUM != last_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	যদি (Buf != রিক্ত)
	    sprintf((বর্ণ *)Buf, (বর্ণ *)p, (long)SOURCING_LNUM);
	প্রত্যাগম Buf;
    }
    প্রত্যাগম রিক্ত;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    শূন্য
বার্তা_source(পূর্ণাঙ্ক attr)
{
    char_u	*p;
    স্থির পূর্ণাঙ্ক	recursive = FALSE;

    // Bail out if something called here causes an error.
    যদি (recursive)
	প্রত্যাগম;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_source();
    যদি (p != রিক্ত)
    {
	বার্তা_scroll = TRUE;  // this will take more than one line
	বার্তা_attr((বর্ণ *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    যদি (p != রিক্ত)
    {
	বার্তা_attr((বর্ণ *)p, HL_ATTR(HLF_N));
	vim_free(p);
	last_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    যদি (SOURCING_NAME == রিক্ত || other_sourcing_name())
    {
	VIM_CLEAR(last_sourcing_name);
	যদি (SOURCING_NAME != রিক্ত)
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
    স্থির পূর্ণাঙ্ক
emsg_not_now(শূন্য)
{
    যদি ((emsg_off > 0 && vim_strchr(p_debug, 'm') == রিক্ত
					  && vim_strchr(p_debug, 't') == রিক্ত)
#ifdef FEAT_EVAL
	    || emsg_skip > 0
#endif
	    )
	প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}

#যদি defined(FEAT_EVAL)
স্থির garray_T ignore_error_list = GA_EMPTY;

    শূন্য
ignore_error_for_testing(char_u *error)
{
    যদি (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, পরিমাণ(char_u *), 1);

    যদি (STRCMP("RESET", error) == 0)
	ga_clear_strings(&ignore_error_list);
    অন্যথা
	ga_copy_string(&ignore_error_list, error);
}

    স্থির পূর্ণাঙ্ক
ignore_error(ধ্রুব বর্ণ *বার্তা)
{
    পূর্ণাঙ্ক i;

    প্রতি (i = 0; i < ignore_error_list.ga_len; ++i)
	যদি (strstr(বার্তা,
		  (বর্ণ *)((char_u **)(ignore_error_list.ga_data))[i]) != রিক্ত)
	    প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}
#endif

#যদি !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    শূন্য
do_perror(বর্ণ *বার্তা)
{
    perror(বার্তা);
    ++emsg_silent;
    emsg(বার্তা);
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
    স্থির পূর্ণাঙ্ক
emsg_core(ধ্রুব বর্ণ *s)
{
    পূর্ণাঙ্ক		attr;
    char_u	*p;
    পূর্ণাঙ্ক		r;
#ifdef FEAT_EVAL
    পূর্ণাঙ্ক		ignore = FALSE;
    পূর্ণাঙ্ক		severe;
#endif

#ifdef FEAT_EVAL
    // When testing some errors are turned into a normal message.
    যদি (ignore_error(s))
	// don't call msg() if it results in a dialog
	প্রত্যাগম বার্তা_use_printf() ? FALSE : বার্তা((বর্ণ *)s);
#endif

    ++called_emsg;

#ifdef FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#endif

    যদি (!emsg_off || vim_strchr(p_debug, 't') != রিক্ত)
    {
#ifdef FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	যদি (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    যদি (!ignore)
		++did_emsg;
	    প্রত্যাগম TRUE;
	}

	যদি (in_assert_fails && emsg_assert_fails_বার্তা == রিক্ত)
	{
	    emsg_assert_fails_বার্তা = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == রিক্ত ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#endif

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	যদি (emsg_silent != 0)
	{
#ifdef FEAT_EVAL
	    ++did_emsg_silent;
#endif
	    যদি (emsg_noredir == 0)
	    {
		বার্তা_start();
		p = get_emsg_source();
		যদি (p != রিক্ত)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		যদি (p != রিক্ত)
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
	    যদি (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#endif
#ifdef FEAT_EVAL
	    ch_log(রিক্ত, "ERROR silent: %s", s);
#endif
	    প্রত্যাগম TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	বার্তা_silent = 0;
	cmd_silent = FALSE;

	যদি (global_busy)		// break :global command
	    ++global_busy;

	যদি (p_eb)
	    beep_flush();		// also includes flush_buffers()
	অন্যথা
	    flush_buffers(FLUSH_MINIMAL);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ifdef FEAT_EVAL
	++uncaught_emsg;
#endif
    }

#ifdef HAS_MESSAGE_WINDOW
    যদি (!in_echowindow)
#endif
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    যদি (বার্তা_scrolled != 0)
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
    বার্তা_scroll = TRUE;
    বার্তা_source(attr);

    /*
     * Display the error message itself.
     */
    বার্তা_nowait = FALSE;			// wait for this msg
    r = বার্তা_attr((বর্ণ *)s, attr);

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = FALSE;
#endif
    প্রত্যাগম r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    পূর্ণাঙ্ক
emsg(বর্ণ *s)
{
    // Skip this if not giving error messages at the moment.
    যদি (emsg_not_now())
	প্রত্যাগম TRUE;

    প্রত্যাগম emsg_core(s);
}

#ifndef PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    পূর্ণাঙ্ক
semsg(ধ্রুব বর্ণ *s, ...)
{
    // Skip this if not giving error messages at the moment.
    যদি (emsg_not_now())
	প্রত্যাগম TRUE;

    যদি (IObuff == রিক্ত)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	প্রত্যাগম emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((বর্ণ *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    প্রত্যাগম emsg_core((বর্ণ *)IObuff);
}
#endif

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    শূন্য
iemsg(বর্ণ *s)
{
    যদি (emsg_not_now())
	প্রত্যাগম;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    emsg_core(s);
#যদি defined(ABORT_ON_INTERNAL_ERROR) && defined(FEAT_EVAL)
    set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    বার্তা_putchar('\n');  // avoid overwriting the error message
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
    শূন্য
siemsg(ধ্রুব বর্ণ *s, ...)
{
    যদি (emsg_not_now())
	প্রত্যাগম;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    যদি (IObuff == রিক্ত)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    অন্যথা
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((বর্ণ *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((বর্ণ *)IObuff);
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    বার্তা_putchar('\n');  // avoid overwriting the error message
    out_flush();
    abort();
# endif
}
#endif

/*
 * Give an "Internal error" message.
 */
    শূন্য
internal_error(বর্ণ *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    siemsg(_(e_internal_error_str), where);
}

#যদি defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    শূন্য
internal_error_no_abort(বর্ণ *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    semsg(_(e_internal_error_str), where);
}
#endif

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    শূন্য
emsg_invreg(পূর্ণাঙ্ক name)
{
    semsg(_(e_invalid_register_name_str), transchar_buf(রিক্ত, name));
}

#যদি defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    শূন্য
emsg_namelen(বর্ণ *বার্তা, char_u *name, পূর্ণাঙ্ক len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(বার্তা, copy == রিক্ত ? "NULL" : (বর্ণ *)copy);
    vim_free(copy);
}
#endif

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    বর্ণ *
বার্তা_trunc_attr(বর্ণ *s, পূর্ণাঙ্ক force, পূর্ণাঙ্ক attr)
{
    পূর্ণাঙ্ক		n;
    বর্ণ	*ts;

    // Add message to history before truncating
    add_বার্তা_hist((char_u *)s, -1, attr);

    ts = (বর্ণ *)বার্তা_may_trunc(force, (char_u *)s);

    বার্তা_hist_off = TRUE;
    n = বার্তা_attr(ts, attr);
    বার্তা_hist_off = FALSE;

    যদি (n)
	প্রত্যাগম ts;
    প্রত্যাগম রিক্ত;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
বার্তা_may_trunc(পূর্ণাঙ্ক force, char_u *s)
{
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		room;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    room = (পূর্ণাঙ্ক)(Rows - cmdline_row - 1) * cmdline_width + sc_col - 1;
    যদি (room > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_active))
	    && (n = (পূর্ণাঙ্ক)STRLEN(s) - room) > 0)
    {
	যদি (has_mbyte)
	{
	    পূর্ণাঙ্ক	size = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    যদি (size <= room)
		প্রত্যাগম s;

	    প্রতি (n = 0; size >= room; )
	    {
		size -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    প্রত্যাগম s;
}

    স্থির শূন্য
add_বার্তা_hist(
    char_u	*s,
    পূর্ণাঙ্ক		len,		// -1 for undetermined length
    পূর্ণাঙ্ক		attr)
{
    সংরচনা বার্তা_hist *p;

    যদি (বার্তা_hist_off || বার্তা_silent != 0)
	প্রত্যাগম;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(সংরচনা বার্তা_hist);
    যদি (p == রিক্ত)
	প্রত্যাগম;

    যদি (len < 0)
	len = (পূর্ণাঙ্ক)STRLEN(s);
    // remove leading and trailing newlines
    যাবত্ (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    যাবত্ (len > 0 && s[len - 1] == '\n')
	--len;
    p->বার্তা = vim_strnsave(s, len);
    p->next = রিক্ত;
    p->attr = attr;
    যদি (last_বার্তা_hist != রিক্ত)
	last_বার্তা_hist->next = p;
    last_বার্তা_hist = p;
    যদি (first_বার্তা_hist == রিক্ত)
	first_বার্তা_hist = last_বার্তা_hist;
    ++বার্তা_hist_len;

    check_বার্তা_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    পূর্ণাঙ্ক
delete_first_বার্তা(শূন্য)
{
    সংরচনা বার্তা_hist *p;

    যদি (বার্তা_hist_len <= 0)
	প্রত্যাগম FAIL;
    p = first_বার্তা_hist;
    first_বার্তা_hist = p->next;
    যদি (first_বার্তা_hist == রিক্ত)
	last_বার্তা_hist = রিক্ত;  // history is empty
    vim_free(p->বার্তা);
    vim_free(p);
    --বার্তা_hist_len;
    প্রত্যাগম OK;
}

    স্থির শূন্য
check_বার্তা_hist(শূন্য)
{
    // Don't let the message history get too big
    যাবত্ (বার্তা_hist_len > 0 && বার্তা_hist_len > বার্তা_hist_max)
	(শূন্য)delete_first_বার্তা();
}


    পূর্ণাঙ্ক
messagesopt_changed(শূন্য)
{
    char_u	*p;
    পূর্ণাঙ্ক		messages_flags_new = 0;
    পূর্ণাঙ্ক		messages_wait_new = 0;
    পূর্ণাঙ্ক		messages_history_new = 0;

    p = p_mopt;
    যাবত্ (*p != NUL)
    {
	যদি (STRNCMP(p, MESSAGES_OPT_HIT_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER);
	    messages_flags_new |= MESSAGES_HIT_ENTER;
	}
	অন্যথা যদি (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	অন্যথা যদি (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	যদি (*p != ',' && *p != NUL)
	    প্রত্যাগম FAIL;
	যদি (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    যদি (!(messages_flags_new & (MESSAGES_HIT_ENTER | MESSAGES_WAIT)))
	প্রত্যাগম FAIL;

    // "history" must be set
    যদি (!(messages_flags_new & MESSAGES_HISTORY))
	প্রত্যাগম FAIL;

    যদি (messages_history_new < 0 || messages_history_new > 10000)
	প্রত্যাগম FAIL;

    যদি (messages_wait_new < 0 || messages_wait_new > 10000)
	প্রত্যাগম FAIL;

    বার্তা_flags = messages_flags_new;
    বার্তা_wait = messages_wait_new;

    বার্তা_hist_max = messages_history_new;
    check_বার্তা_hist();

    প্রত্যাগম OK;
}

/*
 * ":messages" command.
 */
    শূন্য
ex_messages(exarg_T *eap)
{
    সংরচনা বার্তা_hist *p;
    char_u	    *s;
    পূর্ণাঙ্ক		    c = 0;

    যদি (STRCMP(eap->arg, "clear") == 0)
    {
	পূর্ণাঙ্ক keep = eap->addr_গুনতি == 0 ? 0 : eap->line2;

	যাবত্ (বার্তা_hist_len > keep)
	    (শূন্য)delete_first_বার্তা();
	প্রত্যাগম;
    }

    যদি (*eap->arg != NUL)
    {
	emsg(_(e_invalid_argument));
	প্রত্যাগম;
    }

    বার্তা_hist_off = TRUE;

    p = first_বার্তা_hist;
    যদি (eap->addr_গুনতি != 0)
    {
	// Count total messages
	প্রতি (; p != রিক্ত && !got_int; p = p->next)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	প্রতি (p = first_বার্তা_hist; p != রিক্ত && !got_int && c > 0;
						    p = p->next, c--);
    }

    যদি (p == first_বার্তা_hist)
    {
#ifdef FEAT_MULTI_LANG
	s = get_mess_lang();
#অন্যথা
	s = mch_getenv((char_u *)"LANG");
#endif
	যদি (s != রিক্ত && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    বার্তা_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    প্রতি (; p != রিক্ত && !got_int; p = p->next)
	যদি (p->বার্তা != রিক্ত)
	    বার্তা_attr((বর্ণ *)p->বার্তা, p->attr);

    বার্তা_hist_off = FALSE;
}

#যদি defined(FEAT_CON_DIALOG) || defined(FIND_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    শূন্য
বার্তা_end_prompt(শূন্য)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_row = বার্তা_row;
    বার্তা_col = 0;
    বার্তা_clr_eos();
    সারিগুলি_বাম = -1;
}
#endif

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    শূন্য
wait_return(পূর্ণাঙ্ক redraw)
{
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		oldState;
    পূর্ণাঙ্ক		tmpState;
    পূর্ণাঙ্ক		had_got_int;
    পূর্ণাঙ্ক		save_reg_recording;
    FILE	*save_scriptout;

    যদি (redraw == TRUE)
	set_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    যদি (বার্তা_silent != 0)
	প্রত্যাগম;
#ifdef HAS_MESSAGE_WINDOW
    যদি (in_echowindow)
	প্রত্যাগম;
#endif

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    যদি (vgetc_busy > 0)
	প্রত্যাগম;
    need_wait_return = TRUE;
    যদি (no_wait_return)
    {
	যদি (!exmode_active)
	    cmdline_row = বার্তা_row;
	প্রত্যাগম;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    যদি (quit_more)
    {
	c = CAR;		// just pretend CR was hit
	quit_more = FALSE;
	got_int = FALSE;
    }
    অন্যথা যদি (exmode_active)
    {
	বার্তা_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    অন্যথা যদি (!stuff_empty())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    অন্যথা
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = MODE_HITRETURN;
	setmouse();
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#endif
	cmdline_row = বার্তা_row;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	যদি (need_check_timestamps)
	    check_timestamps(FALSE);

	যদি (বার্তা_flags & MESSAGES_HIT_ENTER)
	{
	    hit_return_বার্তা();

	    কুরু
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
		++allow_চাবিগুলি;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = রিক্ত;
		c = safe_vgetc();
		যদি (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_চাবিগুলি;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ifdef FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		যদি (চাবি_Typed && c == Ctrl_Y && clip_star.state == SELECT_DONE)
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
		যদি (চাবি_Typed && p_more && !p_cp)
		{
		    যদি (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_UP || c == K_PAGEUP)
		    {
			যদি (বার্তা_scrolled > Rows)
			    // scroll back to show older messages
			    do_more_prompt(c);
			অন্যথা
			{
			    বার্তা_didout = FALSE;
			    c = K_IGNORE;
			    বার্তা_col =
#ifdef FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_width - 1 :
#endif
				0;
			}
			যদি (quit_more)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_more = FALSE;
			    got_int = FALSE;
			}
			অন্যথা যদি (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    hit_return_বার্তা();
			}
		    }
		    অন্যথা যদি (বার্তা_scrolled > Rows - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_DOWN || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } যাবত্ ((had_got_int && c == Ctrl_C)
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
				    && mouse_row < বার্তা_row
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    যদি (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(শূন্য)jump_to_mouse(MOUSE_SETPOS, রিক্ত, 0);
	    অন্যথা যদি (!চাবি_Typed || (vim_strchr((char_u *)"\r\n ", c) == রিক্ত
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_mask);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	অন্যথা
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(বার্তা_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    যদি (c == ':' || c == '?' || c == '/')
    {
	যদি (!exmode_active)
	    cmdline_row = বার্তা_row;
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
    বার্তা_check();

#ifndef MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    যদি (swapping_screen() && !termcap_active)
	newline_on_exit = TRUE;
#endif

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    সারিগুলি_বাম = -1;		// reset lines_left at next msg_start()
    reset_last_sourcing();
    যদি (keep_বার্তা != রিক্ত && vim_strsize(keep_বার্তা) >=
			    (Rows - cmdline_row - 1) * cmdline_width + sc_col)
	VIM_CLEAR(keep_বার্তা);	    // don't redisplay message, it's too long

    যদি (tmpState == MODE_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    অন্যথা যদি (!skip_redraw
	    && (redraw == TRUE || (বার্তা_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    স্থির শূন্য
hit_return_বার্তা(শূন্য)
{
    পূর্ণাঙ্ক		save_p_more = p_more;

    p_more = FALSE;	// don't want to see this message when scrolling back
    যদি (বার্তা_didout)	// start on a new line
	বার্তা_putchar('\n');
    যদি (got_int)
	বার্তা_puts(_("Interrupt: "));

    বার্তা_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    যদি (!বার্তা_use_printf())
	বার্তা_clr_eos();
    p_more = save_p_more;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    শূন্য
set_keep_বার্তা(char_u *s, পূর্ণাঙ্ক attr)
{
    vim_free(keep_বার্তা);
    যদি (s != রিক্ত && বার্তা_silent == 0)
	keep_বার্তা = vim_strsave(s);
    অন্যথা
	keep_বার্তা = রিক্ত;
    keep_বার্তা_more = FALSE;
    keep_বার্তা_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    শূন্য
set_keep_বার্তা_from_hist(শূন্য)
{
    যদি (keep_বার্তা == রিক্ত && last_বার্তা_hist != রিক্ত && বার্তা_scrolled == 0
						      && (State & MODE_NORMAL))
	set_keep_বার্তা(last_বার্তা_hist->বার্তা, last_বার্তা_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    শূন্য
বার্তা_start(শূন্য)
{
    পূর্ণাঙ্ক		did_return = FALSE;

    যদি (বার্তা_row < cmdline_row)
	বার্তা_row = cmdline_row;

    যদি (!বার্তা_silent)
    {
	VIM_CLEAR(keep_বার্তা);
	need_fileinfo = FALSE;
    }

#ifdef FEAT_EVAL
    যদি (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	বার্তা_clr_eos();
    }
#endif

#ifdef HAS_MESSAGE_WINDOW
    যদি (in_echowindow)
    {
	যদি (popup_message_win_visible()
		    && ((বার্তা_col > 0 && (বার্তা_scroll || !full_screen))
			|| in_echowindow))
	{
	    win_T *wp = popup_get_message_win();

	    // start a new line
	    curbuf = wp->w_buffer;
	    ml_append(wp->w_buffer->b_ml.ml_সারি_গুনতি,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_buffer;
	}
	বার্তা_col = 0;
    }
    অন্যথা
#endif
	যদি (!বার্তা_scroll && full_screen)	// overwrite last message
    {
	বার্তা_row = cmdline_row;
	বার্তা_col =
#ifdef FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_width - 1 :
#endif
	    0;
    }
    অন্যথা যদি (বার্তা_didout || in_echowindow)
    {
	// start message on next line
	বার্তা_putchar('\n');
	did_return = TRUE;
	যদি (exmode_active != EXMODE_NORMAL)
	    cmdline_row = বার্তা_row;
    }
    যদি (!বার্তা_didany || সারিগুলি_বাম < 0)
	বার্তা_starthere();
    যদি (বার্তা_silent == 0)
    {
	বার্তা_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    যদি (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    শূন্য
বার্তা_starthere(শূন্য)
{
    সারিগুলি_বাম = cmdline_row;
    বার্তা_didany = FALSE;
}

    শূন্য
বার্তা_putchar(পূর্ণাঙ্ক c)
{
    বার্তা_putchar_attr(c, 0);
}

    শূন্য
বার্তা_putchar_attr(পূর্ণাঙ্ক c, পূর্ণাঙ্ক attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    যদি (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    অন্যথা
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    বার্তা_puts_attr((বর্ণ *)buf, attr);
}

    শূন্য
বার্তা_outnum(long n)
{
    বর্ণ	buf[20];

    sprintf(buf, "%ld", n);
    বার্তা_puts(buf);
}

    শূন্য
বার্তা_home_replace(char_u *fname)
{
    বার্তা_home_replace_attr(fname, 0);
}

#যদি defined(FEAT_FIND_ID)
    শূন্য
বার্তা_home_replace_hl(char_u *fname)
{
    বার্তা_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#endif

    স্থির শূন্য
বার্তা_home_replace_attr(char_u *fname, পূর্ণাঙ্ক attr)
{
    char_u	*name;

    name = home_replace_save(রিক্ত, fname);
    যদি (name != রিক্ত)
	বার্তা_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    পূর্ণাঙ্ক
বার্তা_outtrans(char_u *str)
{
    প্রত্যাগম বার্তা_outtrans_attr(str, 0);
}

    পূর্ণাঙ্ক
বার্তা_outtrans_attr(char_u *str, পূর্ণাঙ্ক attr)
{
    প্রত্যাগম বার্তা_outtrans_len_attr(str, (পূর্ণাঙ্ক)STRLEN(str), attr);
}

    পূর্ণাঙ্ক
বার্তা_outtrans_len(char_u *str, পূর্ণাঙ্ক len)
{
    প্রত্যাগম বার্তা_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
বার্তা_outtrans_one(char_u *p, পূর্ণাঙ্ক attr)
{
    পূর্ণাঙ্ক		l;

    যদি (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	বার্তা_outtrans_len_attr(p, l, attr);
	প্রত্যাগম p + l;
    }
    বার্তা_puts_attr((বর্ণ *)transchar_byte_buf(রিক্ত, *p), attr);
    প্রত্যাগম p + 1;
}

    পূর্ণাঙ্ক
বার্তা_outtrans_len_attr(char_u *msgstr, পূর্ণাঙ্ক len, পূর্ণাঙ্ক attr)
{
    পূর্ণাঙ্ক		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_start = msgstr;
    char_u	*s;
    পূর্ণাঙ্ক		mb_l;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    যদি (attr == 0)
	attr = HL_ATTR(HLF_বার্তা);

    // if MSG_HIST flag set, add message to history
    যদি (attr & বার্তা_HIST)
    {
	add_বার্তা_hist(str, len, attr);
	attr &= ~বার্তা_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    যদি (বার্তা_silent == 0 && len > 0 && বার্তা_row >= cmdline_row && বার্তা_col == 0)
    {
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    যদি (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	বার্তা_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    যাবত্ (--len >= 0 && !got_int)
    {
	যদি (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	অন্যথা যদি (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	অন্যথা
	    mb_l = 1;
	যদি (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    যদি (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    অন্যথা
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		যদি (str > plain_start)
		    বার্তা_puts_attr_len((বর্ণ *)plain_start,
					       (পূর্ণাঙ্ক)(str - plain_start), attr);
		plain_start = str + mb_l;
		বার্তা_puts_attr((বর্ণ *)transchar_buf(রিক্ত, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	অন্যথা
	{
	    s = transchar_byte_buf(রিক্ত, *str);
	    যদি (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		যদি (str > plain_start)
		    বার্তা_puts_attr_len((বর্ণ *)plain_start,
					       (পূর্ণাঙ্ক)(str - plain_start), attr);
		plain_start = str + 1;
		বার্তা_puts_attr((বর্ণ *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (পূর্ণাঙ্ক)STRLEN(s);
	    }
	    অন্যথা
		++retval;
	    ++str;
	}
    }

    যদি (str > plain_start && !got_int)
	// print the printable chars at the end
	বার্তা_puts_attr_len((বর্ণ *)plain_start, (পূর্ণাঙ্ক)(str - plain_start), attr);

    got_int |= save_got_int;

    প্রত্যাগম retval;
}

#যদি defined(FEAT_QUICKFIX)
    শূন্য
বার্তা_make(char_u *arg)
{
    পূর্ণাঙ্ক	    i;
    স্থির char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    প্রতি (i = 5; *arg && i >= 0; --i)
	যদি (*arg++ != str[i])
	    ভঙ্গ;
    যদি (i < 0)
    {
	বার্তা_putchar('\n');
	প্রতি (i = 0; rs[i]; ++i)
	    বার্তা_putchar(rs[i] - 3);
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
    পূর্ণাঙ্ক
বার্তা_outtrans_special(
    char_u	*strstart,
    পূর্ণাঙ্ক		from,	// TRUE for lhs of a mapping
    পূর্ণাঙ্ক		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    পূর্ণাঙ্ক		retval = 0;
    বর্ণ	*text;
    পূর্ণাঙ্ক		attr;
    পূর্ণাঙ্ক		len;

    attr = HL_ATTR(HLF_8);
    যাবত্ (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	যদি ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    text = "<Space>";
	    ++str;
	}
	অন্যথা
	    text = (বর্ণ *)str2special(&str, from, FALSE);
	যদি (text[0] != NUL && text[1] == NUL)
	    // single-byte character or illegal byte
	    text = (বর্ণ *)transchar_byte_buf(রিক্ত, (char_u)text[0]);
	len = vim_strsize((char_u *)text);
	যদি (maxlen > 0 && retval + len >= maxlen)
	    ভঙ্গ;
	// Highlight special keys
	বার্তা_puts_attr(text, len > 1
		&& (*mb_ptr2len)((char_u *)text) <= 1 ? attr : 0);
	retval += len;
    }
    প্রত্যাগম retval;
}

#যদি defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    পূর্ণাঙ্ক	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    পূর্ণাঙ্ক	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    যাবত্ (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    প্রত্যাগম (char_u *)ga.ga_data;
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
    পূর্ণাঙ্ক		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    পূর্ণাঙ্ক		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    পূর্ণাঙ্ক			c;
    স্থির char_u	buf[7];
    char_u		*str = *sp;
    পূর্ণাঙ্ক			modifiers = 0;
    পূর্ণাঙ্ক			special = FALSE;

    যদি (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	যদি (p != রিক্ত)
	    প্রত্যাগম p;
    }

    c = *str;
    যদি ((c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	) && str[1] != NUL && str[2] != NUL)
    {
	যদি (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	যদি ((c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = TO_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	যদি (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    যদি (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	যদি (p != রিক্ত)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	অন্যথা
	    // illegal byte
	    *sp = str + 1;
    }
    অন্যথা
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    যদি (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	প্রত্যাগম get_special_চাবি_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    প্রত্যাগম buf;
}

/*
 * Translate a key sequence into special key names.
 */
    শূন্য
str2specialbuf(char_u *sp, char_u *buf, পূর্ণাঙ্ক len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    যাবত্ (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	যদি (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	অন্যথা
	    ভঙ্গ;
    }
}

/*
 * print line for :print or :list command
 */
    শূন্য
বার্তা_prt_সারি(char_u *s, পূর্ণাঙ্ক list)
{
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		col = 0;
    পূর্ণাঙ্ক		n_extra = 0;
    পূর্ণাঙ্ক		c_extra = 0;
    পূর্ণাঙ্ক		c_final = 0;
    char_u	*p_extra = রিক্ত;	    // init to make SASC shut up
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		attr = 0;
    char_u	*trail = রিক্ত;
    char_u	*lead = রিক্ত;
    পূর্ণাঙ্ক		in_multispace = FALSE;
    পূর্ণাঙ্ক		multispace_pos = 0;
    পূর্ণাঙ্ক		l;
    char_u	buf[MB_MAXBYTES + 1];

    যদি (curwin->w_p_list)
	list = TRUE;

    যদি (list)
    {
	// find start of trailing whitespace
	যদি (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    যাবত্ (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	যদি (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != রিক্ত
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    যাবত্ (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    যদি (*lead == NUL)
		lead = রিক্ত;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    যদি (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	বার্তা_putchar(' ');

    যাবত্ (!got_int)
    {
	যদি (n_extra > 0)
	{
	    --n_extra;
	    যদি (n_extra == 0 && c_final)
		c = c_final;
	    অন্যথা যদি (c_extra)
		c = c_extra;
	    অন্যথা
		c = *p_extra++;
	}
	অন্যথা যদি (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    col += (*mb_ptr2cells)(s);
	    যদি (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    অন্যথা যদি (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		পূর্ণাঙ্ক len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    অন্যথা
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    বার্তা_puts((বর্ণ *)buf);
	    s += l;
	    অনুবর্ত;
	}
	অন্যথা
	{
	    attr = 0;
	    c = *s++;
	    যদি (list)
	    {
		in_multispace = c == ' ' && (*s == ' '
						 || (col > 0 && s[-2] == ' '));
		যদি (!in_multispace)
		    multispace_pos = 0;
	    }
	    যদি (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ifdef FEAT_VARTABS
		n_extra = tabstop_padding(col, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#অন্যথা
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
#endif
		যদি (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		অন্যথা
		{
		    পূর্ণাঙ্ক lcs_tab1 = curwin->w_lcs_chars.tab1;
		    পূর্ণাঙ্ক lcs_tab2 = curwin->w_lcs_chars.tab2;
		    পূর্ণাঙ্ক lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    যদি (lead != রিক্ত && s <= lead
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
	    অন্যথা যদি (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    অন্যথা যদি (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    অন্যথা যদি (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(রিক্ত, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    অন্যথা যদি (c == ' ')
	    {
		যদি (lead != রিক্ত && s <= lead && in_multispace
			&& curwin->w_lcs_chars.leadmultispace != রিক্ত)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    যদি (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		অন্যথা যদি (lead != রিক্ত && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		অন্যথা যদি (trail != রিক্ত && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		অন্যথা যদি (in_multispace
			&& curwin->w_lcs_chars.multispace != রিক্ত)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    যদি (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		অন্যথা যদি (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	যদি (c == NUL)
	    ভঙ্গ;

	বার্তা_putchar_attr(c, attr);
	col++;
    }
    বার্তা_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    স্থির char_u *
screen_puts_mbyte(char_u *s, পূর্ণাঙ্ক l, পূর্ণাঙ্ক attr)
{
    পূর্ণাঙ্ক		cw;

    বার্তা_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    যদি (cw > 1 && (
#ifdef FEAT_RIGHTLEFT
		cmdmsg_rl ? বার্তা_col <= 1 :
#endif
		বার্তা_col == cmdline_width - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	বার্তা_screen_putchar('>', HL_ATTR(HLF_AT));
	প্রত্যাগম s;
    }

    screen_puts_len(s, l, বার্তা_row, cmdline_col_off + বার্তা_col, attr);
#ifdef FEAT_RIGHTLEFT
    যদি (cmdmsg_rl)
    {
	বার্তা_col -= cw;
	যদি (বার্তা_col == 0)
	{
	    বার্তা_col = cmdline_width;
	    ++বার্তা_row;
	}
    }
    অন্যথা
#endif
    {
	বার্তা_col += cw;
	যদি (বার্তা_col >= cmdline_width)
	{
	    বার্তা_col = 0;
	    ++বার্তা_row;
	}
    }
    প্রত্যাগম s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    শূন্য
বার্তা_puts(বর্ণ *s)
{
    বার্তা_puts_attr(s, 0);
}

    শূন্য
বার্তা_puts_title(বর্ণ *s)
{
    বার্তা_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    স্থির শূন্য
বার্তা_outtrans_long_len_attr(char_u *longstr, পূর্ণাঙ্ক len, পূর্ণাঙ্ক attr)
{
    পূর্ণাঙ্ক		slen = len;
    পূর্ণাঙ্ক		room;

    room = cmdline_width - বার্তা_col;
    যদি (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	বার্তা_outtrans_len_attr(longstr, slen, attr);
	বার্তা_puts_attr("...", HL_ATTR(HLF_8));
    }
    বার্তা_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    শূন্য
বার্তা_outtrans_long_attr(char_u *longstr, পূর্ণাঙ্ক attr)
{
    বার্তা_outtrans_long_len_attr(longstr, (পূর্ণাঙ্ক)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    শূন্য
বার্তা_puts_attr(বর্ণ *s, পূর্ণাঙ্ক attr)
{
    বার্তা_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    স্থির শূন্য
বার্তা_puts_attr_len(বর্ণ *str, পূর্ণাঙ্ক maxlen, পূর্ণাঙ্ক attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    যদি (বার্তা_silent != 0)
	প্রত্যাগম;

    যদি (attr == 0)
	attr = HL_ATTR(HLF_বার্তা);

    // if MSG_HIST flag set, add message to history
    যদি ((attr & বার্তা_HIST) && maxlen < 0)
    {
	add_বার্তা_hist((char_u *)str, -1, attr);
	attr &= ~বার্তা_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    যদি (বার্তা_scrolled != 0 && !বার্তা_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    বার্তা_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    যদি (বার্তা_use_printf())
	বার্তা_puts_printf((char_u *)str, maxlen);
    অন্যথা
	বার্তা_puts_display((char_u *)str, maxlen, attr, FALSE);

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
    স্থির শূন্য
put_বার্তা_win(win_T *wp, পূর্ণাঙ্ক where, char_u *t_s, char_u *end, linenr_T lnum)
{
    char_u  *p;

    যদি (where == PUT_BELOW)
    {
	যদি (*end != NUL)
	{
	    p = vim_strnsave(t_s, end - t_s);
	    যদি (p == রিক্ত)
		প্রত্যাগম;
	}
	অন্যথা
	    p = t_s;
	ml_append_buf(wp->w_buffer, lnum, p, (colnr_T)0, FALSE);
	যদি (p != t_s)
	    vim_free(p);
    }
    অন্যথা
    {
	char_u *newp;

	curbuf = wp->w_buffer;
	যদি (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    যদি (newp == রিক্ত)
		প্রত্যাগম;
	    যদি (*end != NUL)
		newp[STRLEN(ml_get(lnum)) + (end - t_s)] = NUL;
	}
	অন্যথা
	{
	    newp = vim_strnsave(t_s, end - t_s);
	    যদি (newp == রিক্ত)
		প্রত্যাগম;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_buffer;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    বার্তা_col += (পূর্ণাঙ্ক)(end - t_s);
}
#endif

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    স্থির শূন্য
বার্তা_puts_display(
    char_u	*str,
    পূর্ণাঙ্ক		maxlen,
    পূর্ণাঙ্ক		attr,
    পূর্ণাঙ্ক		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    পূর্ণাঙ্ক		t_col = 0;	// screen cells todo, 0 when "t_s" not used
    পূর্ণাঙ্ক		l;
    পূর্ণাঙ্ক		cw;
    char_u	*sb_str = str;
    পূর্ণাঙ্ক		sb_col = বার্তা_col;
    পূর্ণাঙ্ক		wrap;
    পূর্ণাঙ্ক		did_last_char;
#ifdef HAS_MESSAGE_WINDOW
    পূর্ণাঙ্ক		where = PUT_APPEND;
    win_T	*বার্তা_win = রিক্ত;
    linenr_T    lnum = 1;

    যদি (in_echowindow)
    {
	বার্তা_win = popup_get_message_win();

	যদি (বার্তা_win != রিক্ত)
	{
	    যদি (!popup_message_win_visible())
	    {
		যদি (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    বার্তা_win = রিক্ত;
		}
		অন্যথা
		{
		    // currently hidden, make it empty
		    curbuf = বার্তা_win->w_buffer;
		    যাবত্ ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
			ml_delete(1);
		    curbuf = curwin->w_buffer;
		}
	    }
	    অন্যথা
	    {
		lnum = বার্তা_win->w_buffer->b_ml.ml_সারি_গুনতি;
		যদি (বার্তা_col == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#endif

    did_wait_return = FALSE;
    যাবত্ ((maxlen < 0 || (পূর্ণাঙ্ক)(s - str) < maxlen) && *s != NUL)
    {
#ifdef HAS_MESSAGE_WINDOW
	// For echowindow, use full width; for regular messages, leave last column
	পূর্ণাঙ্ক wrap_col = বার্তা_win != রিক্ত ? cmdline_width : cmdline_width - 1;
#অন্যথা
	পূর্ণাঙ্ক wrap_col = cmdline_width - 1;
#endif

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	যদি (!recurse && বার্তা_row >= Rows - 1 && (*s == '\n' || (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			বার্তা_col <= 1
		      || (*s == TAB && বার্তা_col <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && বার্তা_col <= 2))
		    :
#endif
		      ((*s != '\r' && বার্তা_col + t_col >= wrap_col)
		       || (*s == TAB && বার্তা_col + t_col
			   >= (wrap_col & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && বার্তা_col + t_col >= wrap_col - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    যদি (t_col > 0)
	    {
		// output postponed text
#ifdef HAS_MESSAGE_WINDOW
		যদি (বার্তা_win != রিক্ত)
		{
		    put_বার্তা_win(বার্তা_win, where, t_s, s, lnum);
		    যদি (where == PUT_BELOW)
			++lnum;
		    t_col = 0;
		    where = PUT_BELOW;
		}
		অন্যথা
#endif
		    t_puts(&t_col, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    যদি (বার্তা_no_more && সারিগুলি_বাম == 0)
		ভঙ্গ;

#ifdef HAS_MESSAGE_WINDOW
	    যদি (বার্তা_win == রিক্ত)
#endif
		// Scroll the screen up one line.
		বার্তা_scroll_up();

#ifdef HAS_MESSAGE_WINDOW
	    যদি (বার্তা_win == রিক্ত)
	    {
#endif
		বার্তা_row = Rows - 2;
		যদি (বার্তা_col >= cmdline_width)  // can happen after screen resize
		    বার্তা_col = cmdline_width - 1;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif

	    // Display char in last column before showing more-prompt.
	    যদি (*s >= ' '
#ifdef FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#endif
	       )
	    {
		যদি (has_mbyte)
		{
		    যদি (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (পূর্ণাঙ্ক)((str + maxlen) - s));
		    অন্যথা
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		অন্যথা
		    বার্তা_screen_putchar(*s++, attr);
		did_last_char = TRUE;
	    }
	    অন্যথা
		did_last_char = FALSE;

	    যদি (p_more)
		// store text for scrolling back
		store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

#ifdef HAS_MESSAGE_WINDOW
	    যদি (বার্তা_win == রিক্ত)
	    {
#endif
		inc_বার্তা_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		যদি (cmdline_row > 0 && !exmode_active)
		    --cmdline_row;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		যদি (সারিগুলি_বাম > 0)
		    --সারিগুলি_বাম;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif
	    যদি (p_more && সারিগুলি_বাম == 0 && State != MODE_HITRETURN
					    && !বার্তা_no_more && !exmode_active)
	    {
#ifdef FEAT_CON_DIALOG
		যদি (do_more_prompt(NUL))
		    s = confirm_বার্তা_tail;
#অন্যথা
		(শূন্য)do_more_prompt(NUL);
#endif
		যদি (quit_more)
		    প্রত্যাগম;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    যদি (did_last_char)
		অনুবর্ত;
	}

	wrap = *s == '\n'
		    || বার্তা_col + t_col >= wrap_col
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && বার্তা_col + t_col >= wrap_col - 1);
	যদি (t_col > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ifdef HAS_MESSAGE_WINDOW
	    যদি (বার্তা_win != রিক্ত)
	    {
		put_বার্তা_win(বার্তা_win, where, t_s, s, lnum);
		যদি (where == PUT_BELOW)
		    ++lnum;
		t_col = 0;
		where = PUT_BELOW;
		// Reset msg_col after outputting to new line in echowindow
		যদি (wrap)
		    বার্তা_col = 0;
	    }
	    অন্যথা
#endif
		t_puts(&t_col, t_s, s, attr);
	}

	যদি (wrap && p_more && !recurse)
	    // store text for scrolling back
	    store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	যদি (*s == '\n')		    // go to next line
	{
#ifdef HAS_MESSAGE_WINDOW
	    যদি (বার্তা_win != রিক্ত)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		যদি ((বার্তা_win->w_buffer->b_ml.ml_flags & ML_EMPTY) == 0)
		{
		    put_বার্তা_win(বার্তা_win, PUT_BELOW, t_s, t_s, lnum);
		    ++lnum;
		}
		বার্তা_col = 0;  // Reset column for new line
	    }
	    অন্যথা
#endif
		বার্তা_didout = FALSE;	    // remember that line is empty
#ifdef FEAT_RIGHTLEFT
	    যদি (cmdmsg_rl)
		বার্তা_col = cmdline_width - 1;
	    অন্যথা
#endif
		বার্তা_col = 0;
	    যদি (++বার্তা_row >= Rows)  // safety check
		বার্তা_row = Rows - 1;
	}
	অন্যথা যদি (*s == '\r')	    // go to column 0
	{
	    বার্তা_col = 0;
#ifdef HAS_MESSAGE_WINDOW
	    where = PUT_TRUNC;
#endif
	}
	অন্যথা যদি (*s == '\b')	    // go to previous char
	{
	    যদি (বার্তা_col)
		--বার্তা_col;
	}
	অন্যথা যদি (*s == TAB)	    // translate Tab into spaces
	{
#ifdef HAS_MESSAGE_WINDOW
	    যদি (বার্তা_win != রিক্ত)
		বার্তা_col = (বার্তা_col + 7) % 8;
	    অন্যথা
#endif
		কুরু
		    বার্তা_screen_putchar(' ', attr);
		যাবত্ (বার্তা_col & 7);
	}
	অন্যথা যদি (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	অন্যথা
	{
	    যদি (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		যদি (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (পূর্ণাঙ্ক)((str + maxlen) - s));
		অন্যথা
		    l = (*mb_ptr2len)(s);
	    }
	    অন্যথা
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    যদি (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#endif
		    (cw > 1 && বার্তা_col + t_col >= wrap_col))
	    {
		যদি (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		অন্যথা
		    বার্তা_screen_putchar(*s, attr);
	    }
	    অন্যথা
	    {
		// postpone this character until later
		যদি (t_col == 0)
		    t_s = s;
		t_col += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    যদি (t_col > 0)
    {
#ifdef HAS_MESSAGE_WINDOW
	যদি (বার্তা_win != রিক্ত)
	    put_বার্তা_win(বার্তা_win, where, t_s, s, lnum);
	অন্যথা
#endif
	    t_puts(&t_col, t_s, s, attr);
    }

#ifdef HAS_MESSAGE_WINDOW
    যদি (বার্তা_win != রিক্ত)
	popup_show_message_win();
#endif
    // Store the text for scroll back.
    যদি (p_more && !recurse)
	store_sb_text(&sb_str, s, attr, &sb_col, FALSE);

    বার্তা_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    পূর্ণাঙ্ক
message_filtered(char_u *বার্তা)
{
    পূর্ণাঙ্ক match;

    যদি (cmdmod.cmod_filter_regmatch.regprog == রিক্ত)
	প্রত্যাগম FALSE;
    match = vim_regexec(&cmdmod.cmod_filter_regmatch, বার্তা, (colnr_T)0);
    প্রত্যাগম cmdmod.cmod_filter_force ? match : !match;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    স্থির শূন্য
বার্তা_scroll_up(শূন্য)
{
#ifdef HAS_MESSAGE_WINDOW
    যদি (in_echowindow)
	প্রত্যাগম;
#endif
#ifdef FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    যদি (gui.in_use)
	gui_undraw_cursor();
#endif
    // scrolling up always works
    mch_disable_flush();
    screen_del_সারিগুলি(0, 0, 1, (পূর্ণাঙ্ক)Rows, TRUE, 0, রিক্ত);
    mch_enable_flush();

    যদি (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((পূর্ণাঙ্ক)Rows - 1, (পূর্ণাঙ্ক)Rows, cmdline_col_off,
		cmdline_col_off + cmdline_width, ' ', ' ', HL_ATTR(HLF_বার্তা));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	যদি (ScreenAttrs[সারি_Offset[Rows - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((পূর্ণাঙ্ক)Rows - 2, (পূর্ণাঙ্ক)Rows - 1,
				 (পূর্ণাঙ্ক)Columns - 1, (পূর্ণাঙ্ক)Columns,
				 ' ', ' ', HL_ATTR(HLF_বার্তা));
    }
}

/*
 * Increment "msg_scrolled".
 */
    স্থির শূন্য
inc_বার্তা_scrolled(শূন্য)
{
#ifdef FEAT_EVAL
    যদি (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = রিক্ত;

	// v:scrollstart is empty, set it to the script/function name and line
	//সংখ্যা
	যদি (p.string == রিক্ত)
	{
	    p.string = (char_u *)_("Unknown");
	    p.length = STRLEN(p.string);
	}
	অন্যথা
	{
	    size_t  tofreesize;

	    tofreesize = (পূর্ণাঙ্ক)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    যদি (tofree != রিক্ত)
	    {
		p.length = vim_snprintf_safelen((বর্ণ *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (long)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	set_vim_var_string(VV_SCROLLSTART, p.string, (পূর্ণাঙ্ক)p.length);
	vim_free(tofree);
    }
#endif
    ++বার্তা_scrolled;
    set_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
প্রকারনির্দেশ সংরচনা msgchunk_S msgchunk_T;
সংরচনা msgchunk_S
{
    msgchunk_T	*sb_next;
    msgchunk_T	*sb_prev;
    বর্ণ	sb_eol;		// TRUE when line ends after this text
    পূর্ণাঙ্ক		sb_বার্তা_col;	// column in which text starts
    পূর্ণাঙ্ক		sb_attr;	// text attributes
    char_u	sb_text[1];	// text to be displayed, actually longer
};

স্থির msgchunk_T *last_msgchunk = রিক্ত; // last displayed text

স্থির msgchunk_T *বার্তা_sb_start(msgchunk_T *mps);

প্রকারনির্দেশ গণনা {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_DONE
} sb_clear_T;

// When to clear text on next msg.
স্থির sb_clear_T do_clear_sb_text = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    স্থির শূন্য
store_sb_text(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    পূর্ণাঙ্ক		attr,
    পূর্ণাঙ্ক		*sb_col,
    পূর্ণাঙ্ক		finish)		// line ends
{
    msgchunk_T	*mp;

    যদি (do_clear_sb_text == SB_CLEAR_ALL
	    || do_clear_sb_text == SB_CLEAR_CMDLINE_DONE)
    {
	clear_sb_text(do_clear_sb_text == SB_CLEAR_ALL);
	বার্তা_sb_eol();  // prevent messages from overlapping
	do_clear_sb_text = SB_CLEAR_NONE;
    }

    যদি (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_text) + (s - *sb_str) + 1);
	যদি (mp != রিক্ত)
	{
	    mp->sb_eol = finish;
	    mp->sb_বার্তা_col = *sb_col;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_text, *sb_str, s - *sb_str);

	    যদি (last_msgchunk == রিক্ত)
	    {
		last_msgchunk = mp;
		mp->sb_prev = রিক্ত;
	    }
	    অন্যথা
	    {
		mp->sb_prev = last_msgchunk;
		last_msgchunk->sb_next = mp;
		last_msgchunk = mp;
	    }
	    mp->sb_next = রিক্ত;
	}
    }
    অন্যথা যদি (finish && last_msgchunk != রিক্ত)
	last_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_col = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    শূন্য
may_clear_sb_text(শূন্য)
{
    do_clear_sb_text = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    শূন্য
sb_text_start_cmdline(শূন্য)
{
    যদি (do_clear_sb_text == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_text_restart_cmdline();
    অন্যথা
    {
	বার্তা_sb_eol();
	do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    শূন্য
sb_text_restart_cmdline(শূন্য)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;

    যদি (last_msgchunk == রিক্ত || last_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	প্রত্যাগম;

    tofree = বার্তা_sb_start(last_msgchunk);
    last_msgchunk = tofree->sb_prev;
    যদি (last_msgchunk != রিক্ত)
	last_msgchunk->sb_next = রিক্ত;
    যাবত্ (tofree != রিক্ত)
    {
	msgchunk_T *tofree_next = tofree->sb_next;

	vim_free(tofree);
	tofree = tofree_next;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    শূন্য
sb_text_end_cmdline(শূন্য)
{
    do_clear_sb_text = SB_CLEAR_CMDLINE_DONE;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    শূন্য
clear_sb_text(পূর্ণাঙ্ক all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    যদি (all)
	lastp = &last_msgchunk;
    অন্যথা
    {
	যদি (last_msgchunk == রিক্ত)
	    প্রত্যাগম;
	lastp = &বার্তা_sb_start(last_msgchunk)->sb_prev;
    }

    যাবত্ (*lastp != রিক্ত)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    শূন্য
show_sb_text(শূন্য)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = বার্তা_sb_start(last_msgchunk);
    যদি (mp == রিক্ত || mp->sb_prev == রিক্ত)
	vim_beep(BO_MESS);
    অন্যথা
    {
	do_more_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    স্থির msgchunk_T *
বার্তা_sb_start(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    যাবত্ (mp != রিক্ত && mp->sb_prev != রিক্ত && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    প্রত্যাগম mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    শূন্য
বার্তা_sb_eol(শূন্য)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    যদি (last_msgchunk != রিক্ত
	    && !(last_msgchunk->sb_text[0] == '\n'
		&& last_msgchunk->sb_text[1] == NUL))
	last_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    স্থির msgchunk_T *
disp_sb_সারি(পূর্ণাঙ্ক row, msgchunk_T *smp, পূর্ণাঙ্ক clear_to_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    প্রতি (;;)
    {
	বার্তা_row = row;
	বার্তা_col = mp->sb_বার্তা_col;
	p = mp->sb_text;
	যদি (*p == '\n')	    // don't display the line break
	    ++p;
	বার্তা_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	যদি (clear_to_eol)
	    screen_fill(row, row + 1,
		    cmdline_col_off + বার্তা_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', HL_ATTR(HLF_বার্তা));

	যদি (mp->sb_eol || mp->sb_next == রিক্ত)
	    ভঙ্গ;
	mp = mp->sb_next;
    }
    প্রত্যাগম mp->sb_next;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    স্থির শূন্য
t_puts(
    পূর্ণাঙ্ক		*t_col,
    char_u	*t_s,
    char_u	*s,
    পূর্ণাঙ্ক		attr)
{
    // output postponed text
    বার্তা_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (পূর্ণাঙ্ক)(s - t_s), বার্তা_row, cmdline_col_off + বার্তা_col,
	    attr);
    বার্তা_col += *t_col;
    *t_col = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    যদি (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--বার্তা_col;
    যদি (বার্তা_col >= cmdline_width)
    {
	বার্তা_col = 0;
	++বার্তা_row;
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
    পূর্ণাঙ্ক
বার্তা_use_printf(শূন্য)
{
    প্রত্যাগম (!বার্তা_check_screen()
#যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.in_use && !termcap_active)
# অন্যথা
	    || !termcap_active
# endif
#endif
	    || (swapping_screen() && !termcap_active)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    স্থির শূন্য
বার্তা_puts_printf(char_u *str, পূর্ণাঙ্ক maxlen)
{
    char_u	*s = str;
    char_u	*buf = রিক্ত;
    char_u	*p = s;

#ifdef MSWIN
    যদি (!(silent_mode && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#endif
    যাবত্ ((maxlen < 0 || (পূর্ণাঙ্ক)(s - str) < maxlen) && *s != NUL)
    {
	যদি (!(silent_mode && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    যদি (*s == NL)
	    {
		পূর্ণাঙ্ক n = (পূর্ণাঙ্ক)(s - p);

		buf = alloc(n + 3);
		যদি (buf != রিক্ত)
		{
		    memcpy(buf, p, n);
		    যদি (!info_message)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    যদি (info_message)   // informative message, not an error
			mch_বার্তা((বর্ণ *)buf);
		    অন্যথা
			mch_errmsg((বর্ণ *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ifdef FEAT_RIGHTLEFT
	যদি (cmdmsg_rl)
	{
	    যদি (*s == CAR || *s == NL)
		বার্তা_col = cmdline_width - 1;
	    অন্যথা
		--বার্তা_col;
	}
	অন্যথা
#endif
	{
	    যদি (*s == CAR || *s == NL)
		বার্তা_col = 0;
	    অন্যথা
		++বার্তা_col;
	}
	++s;
    }

    যদি (*p != NUL && !(silent_mode && p_verbose == 0))
    {
	char_u *tofree = রিক্ত;

	যদি (maxlen > 0 && vim_strlen_maxlen((বর্ণ *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	যদি (p != রিক্ত)
	{
	    যদি (info_message)
		mch_বার্তা((বর্ণ *)p);
	    অন্যথা
		mch_errmsg((বর্ণ *)p);
	    vim_free(tofree);
	}
    }

    বার্তা_didout = TRUE;	    // assume that line is not empty

#ifdef MSWIN
    যদি (!(silent_mode && p_verbose == 0))
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
    স্থির পূর্ণাঙ্ক
do_more_prompt(পূর্ণাঙ্ক typed_char)
{
    স্থির পূর্ণাঙ্ক	entered = FALSE;
    পূর্ণাঙ্ক		used_typed_char = typed_char;
    পূর্ণাঙ্ক		oldState = State;
    পূর্ণাঙ্ক		c;
#ifdef FEAT_CON_DIALOG
    পূর্ণাঙ্ক		retval = FALSE;
#endif
    পূর্ণাঙ্ক		toscroll;
    msgchunk_T	*mp_last = রিক্ত;
    msgchunk_T	*mp;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		বার্তা_attr;

    বার্তা_attr = HL_ATTR(HLF_বার্তা);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    যদি (entered || (State == MODE_HITRETURN && typed_char == 0))
	প্রত্যাগম FALSE;
    entered = TRUE;

    যদি (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_last = বার্তা_sb_start(last_msgchunk);
	প্রতি (i = 0; i < Rows - 2 && mp_last != রিক্ত
					     && mp_last->sb_prev != রিক্ত; ++i)
	    mp_last = বার্তা_sb_start(mp_last->sb_prev);
    }

    State = MODE_ASKMORE;
    setmouse();
    যদি (typed_char == NUL)
	বার্তা_moremsg(FALSE);
    প্রতি (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	যদি (used_typed_char != NUL)
	{
	    c = used_typed_char;	// was typed at hit-enter prompt
	    used_typed_char = NUL;
	}
	অন্যথা
	    c = get_keystroke();

#যদি defined(FEAT_MENU) && defined(FEAT_GUI)
	যদি (c == K_MENU)
	{
	    পূর্ণাঙ্ক idx = get_menu_index(current_menu, MODE_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    যদি (idx == MENU_INDEX_INVALID)
		অনুবর্ত;
	    c = *current_menu->strings[idx];
	    যদি (c != NUL && current_menu->strings[idx][1] != NUL)
		ins_typebuf(current_menu->strings[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#endif

	toscroll = 0;
	বিকল্প (c)
	{
	পক্ষ BS:		// scroll one line back
	পক্ষ K_BS:
	পক্ষ 'k':
	পক্ষ K_UP:
	    toscroll = -1;
	    ভঙ্গ;

	পক্ষ CAR:		// one extra line
	পক্ষ NL:
	পক্ষ 'j':
	পক্ষ K_DOWN:
	    toscroll = 1;
	    ভঙ্গ;

	পক্ষ 'u':		// Up half a page
	    toscroll = -(Rows / 2);
	    ভঙ্গ;

	পক্ষ 'd':		// Down half a page
	    toscroll = Rows / 2;
	    ভঙ্গ;

	পক্ষ 'b':		// one page back
	পক্ষ Ctrl_B:
	পক্ষ K_PAGEUP:
	    toscroll = -(Rows - 1);
	    ভঙ্গ;

	পক্ষ ' ':		// one extra page
	পক্ষ 'f':
	পক্ষ Ctrl_F:
	পক্ষ K_PAGEDOWN:
	পক্ষ K_LEFTMOUSE:
	    toscroll = Rows - 1;
	    ভঙ্গ;

	পক্ষ 'g':		// all the way back to the start
	    toscroll = -999999;
	    ভঙ্গ;

	পক্ষ 'G':		// all the way to the end
	    toscroll = 999999;
	    সারিগুলি_বাম = 999999;
	    ভঙ্গ;

	পক্ষ ':':		// start new command line
#ifdef FEAT_CON_DIALOG
	    যদি (!confirm_বার্তা_used)
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
	পক্ষ 'q':		// quit
	পক্ষ Ctrl_C:
	পক্ষ ESC:
#ifdef FEAT_CON_DIALOG
	    যদি (confirm_বার্তা_used)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    অন্যথা
#endif
	    {
		got_int = TRUE;
		quit_more = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    সারিগুলি_বাম = Rows - 1;
	    ভঙ্গ;

#ifdef FEAT_CLIPBOARD
	পক্ষ Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    যদি (clip_star.state == SELECT_DONE)
		clip_copy_modeless_selection(TRUE);
	    অনুবর্ত;
#endif
	স্বভাব:		// no valid response
	    বার্তা_moremsg(TRUE);
	    অনুবর্ত;
	}

	যদি (toscroll != 0)
	{
	    যদি (toscroll < 0)
	    {
		// go to start of last line
		যদি (mp_last == রিক্ত)
		    mp = বার্তা_sb_start(last_msgchunk);
		অন্যথা যদি (mp_last->sb_prev != রিক্ত)
		    mp = বার্তা_sb_start(mp_last->sb_prev);
		অন্যথা
		    mp = রিক্ত;

		// go to start of line at top of the screen
		প্রতি (i = 0; i < Rows - 2 && mp != রিক্ত && mp->sb_prev != রিক্ত;
									  ++i)
		    mp = বার্তা_sb_start(mp->sb_prev);

		যদি (mp != রিক্ত && mp->sb_prev != রিক্ত)
		{
		    // Find line to be displayed at top.
		    প্রতি (i = 0; i > toscroll; --i)
		    {
			যদি (mp == রিক্ত || mp->sb_prev == রিক্ত)
			    ভঙ্গ;
			mp = বার্তা_sb_start(mp->sb_prev);
			যদি (mp_last == রিক্ত)
			    mp_last = বার্তা_sb_start(last_msgchunk);
			অন্যথা
			    mp_last = বার্তা_sb_start(mp_last->sb_prev);
		    }

		    যদি (toscroll == -1 && screen_ins_সারিগুলি(0, 0, 1,
						     (পূর্ণাঙ্ক)Rows, 0, রিক্ত) == OK)
		    {
			// display line at top
			(শূন্য)disp_sb_সারি(0, mp, FALSE);
		    }
		    অন্যথা
		    {
			পূর্ণাঙ্ক did_clear = screenclear();

			// redisplay all lines
			প্রতি (i = 0; mp != রিক্ত && i < Rows - 1; ++i)
			{
			    mp = disp_sb_সারি(i, mp, !did_clear);
			    ++বার্তা_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    অন্যথা
	    {
		// First display any text that we scrolled back.
		যাবত্ (toscroll > 0 && mp_last != রিক্ত)
		{
		    // scroll up, display line at bottom
		    বার্তা_scroll_up();
		    inc_বার্তা_scrolled();
		    screen_fill((পূর্ণাঙ্ক)Rows - 2, (পূর্ণাঙ্ক)Rows - 1,
			    cmdline_col_off, cmdline_col_off + cmdline_width,
			    ' ', ' ', বার্তা_attr);
		    mp_last = disp_sb_সারি((পূর্ণাঙ্ক)Rows - 2, mp_last, FALSE);
		    --toscroll;
		}
	    }

	    যদি (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((পূর্ণাঙ্ক)Rows - 1, (পূর্ণাঙ্ক)Rows, cmdline_col_off,
			cmdline_col_off + cmdline_width, ' ', ' ', বার্তা_attr);
		বার্তা_moremsg(FALSE);
		অনুবর্ত;
	    }

	    // display more text, return to caller
	    সারিগুলি_বাম = toscroll;
	}

	ভঙ্গ;
    }

    // clear the --more-- message
    screen_fill((পূর্ণাঙ্ক)Rows - 1, (পূর্ণাঙ্ক)Rows, cmdline_col_off,
	    cmdline_col_off + cmdline_width, ' ', ' ', বার্তা_attr);
    State = oldState;
    setmouse();
    যদি (quit_more)
    {
	বার্তা_row = Rows - 1;
	বার্তা_col = 0;
    }
#ifdef FEAT_RIGHTLEFT
    অন্যথা যদি (cmdmsg_rl)
	বার্তা_col = cmdline_width - 1;
#endif

    entered = FALSE;
#ifdef FEAT_CON_DIALOG
    প্রত্যাগম retval;
#অন্যথা
    প্রত্যাগম FALSE;
#endif
}

#যদি defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_বার্তা
#  undef mch_বার্তা
# endif

# যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    স্থির শূন্য
mch_errmsg_c(বর্ণ *str)
{
    DWORD   nwrite = 0;
    DWORD   mode = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    যদি (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (পূর্ণাঙ্ক)GetConsoleCP() != enc_codepage)
    {
	পূর্ণাঙ্ক	len = (পূর্ণাঙ্ক)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	যদি (w != রিক্ত)
	{
	    WriteConsoleW(h, w, len, &nwrite, রিক্ত);
	    vim_free(w);
	}
    }
    অন্যথা
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
    শূন্য
mch_errmsg(বর্ণ *str)
{
# যদি !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    পূর্ণাঙ্ক		len;
# endif

# যদি (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    যদি (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   অন্যথা
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
	প্রত্যাগম;
    }
# endif

# যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    যদি (!(gui.in_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	প্রত্যাগম;
    }
# endif

# যদি !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (পূর্ণাঙ্ক)STRLEN(str) + 1;
    যদি (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    যদি (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_data + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_data + error_ga.ga_len;
	    প্রতি (;;)
	    {
		p = vim_strchr(p, '\r');
		যদি (p == রিক্ত)
		    ভঙ্গ;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    স্থির শূন্য
mch_বার্তা_c(বর্ণ *str)
{
    DWORD   nwrite = 0;
    DWORD   mode;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    যদি (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (পূর্ণাঙ্ক)GetConsoleCP() != enc_codepage)
    {
	পূর্ণাঙ্ক	len = (পূর্ণাঙ্ক)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	যদি (w != রিক্ত)
	{
	    WriteConsoleW(h, w, len, &nwrite, রিক্ত);
	    vim_free(w);
	}
    }
    অন্যথা
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
    শূন্য
mch_বার্তা(বর্ণ *str)
{
# যদি (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    যদি (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   অন্যথা
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
	প্রত্যাগম;
    }
# endif

# যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    যদি (!(gui.in_use || gui.starting))
#  endif
    {
	mch_বার্তা_c(str);
	প্রত্যাগম;
    }
# endif
# যদি !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#endif // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    স্থির শূন্য
বার্তা_screen_putchar(পূর্ণাঙ্ক c, পূর্ণাঙ্ক attr)
{
    বার্তা_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, বার্তা_row, cmdline_col_off + বার্তা_col, attr);
#ifdef FEAT_RIGHTLEFT
    যদি (cmdmsg_rl)
    {
	যদি (--বার্তা_col == 0)
	{
	    বার্তা_col = cmdline_width;
	    ++বার্তা_row;
	}
    }
    অন্যথা
#endif
    {
	যদি (++বার্তা_col >= cmdline_width)
	{
	    বার্তা_col = 0;
	    ++বার্তা_row;
	}
    }
}

    স্থির শূন্য
বার্তা_moremsg(পূর্ণাঙ্ক full)
{
    পূর্ণাঙ্ক		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (পূর্ণাঙ্ক)Rows - 1, cmdline_col_off, attr);
    যদি (full)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(পূর্ণাঙ্ক)Rows - 1, cmdline_col_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    শূন্য
repeat_message(শূন্য)
{
    যদি (State == MODE_ASKMORE)
    {
	বার্তা_moremsg(TRUE);	// display --more-- message again
	বার্তা_row = Rows - 1;
    }
#ifdef FEAT_CON_DIALOG
    অন্যথা যদি (State == MODE_CONFIRM)
    {
	display_confirm_বার্তা();	// display ":confirm" message again
	বার্তা_row = Rows - 1;
    }
#endif
    অন্যথা যদি (State == MODE_EXTERNCMD)
    {
	windgoto(বার্তা_row, cmdline_col_off + বার্তা_col); // put cursor back
    }
    অন্যথা যদি (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	যদি (বার্তা_row == Rows - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    বার্তা_didout = FALSE;
	    বার্তা_col = 0;
	    বার্তা_clr_eos();
	}
	hit_return_বার্তা();
	বার্তা_row = Rows - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    স্থির পূর্ণাঙ্ক
বার্তা_check_screen(শূন্য)
{
    যদি (!full_screen || !screen_valid(FALSE))
	প্রত্যাগম FALSE;

    যদি (বার্তা_row >= Rows)
	বার্তা_row = Rows - 1;
    যদি (বার্তা_col >= cmdline_width)
	বার্তা_col = cmdline_width - 1;
    প্রত্যাগম TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    শূন্য
বার্তা_clr_eos(শূন্য)
{
    যদি (বার্তা_silent == 0)
	বার্তা_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    শূন্য
বার্তা_clr_eos_force(শূন্য)
{
#ifdef HAS_MESSAGE_WINDOW
    যদি (in_echowindow)
	প্রত্যাগম;  // messages go into a popup
#endif
    যদি (বার্তা_use_printf())
    {
	যদি (full_screen)	// only when termcap codes are valid
	{
	    যদি (*T_CD)
		out_str(T_CD);	// clear to end of display
	    অন্যথা যদি (*T_CE)
		out_str(T_CE);	// clear to end of line
	}
    }
    অন্যথা
    {
	পূর্ণাঙ্ক বার্তা_attr;

	বার্তা_attr = HL_ATTR(HLF_বার্তা);

#ifdef FEAT_RIGHTLEFT
	যদি (cmdmsg_rl)
	{
	    screen_fill(বার্তা_row, বার্তা_row + 1,
		    cmdline_col_off, cmdline_col_off + বার্তা_col + 1,
		    ' ', ' ', বার্তা_attr);
	    screen_fill(বার্তা_row + 1, (পূর্ণাঙ্ক)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', বার্তা_attr);
	}
	অন্যথা
#endif
	{
	    screen_fill(বার্তা_row, বার্তা_row + 1,
		    cmdline_col_off + বার্তা_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', বার্তা_attr);
	    screen_fill(বার্তা_row + 1, (পূর্ণাঙ্ক)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', বার্তা_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    শূন্য
বার্তা_clr_cmdline(শূন্য)
{
    বার্তা_row = cmdline_row;
    বার্তা_col = 0;
    বার্তা_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    পূর্ণাঙ্ক
বার্তা_end(শূন্য)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    যদি (!exiting && need_wait_return && !(State & MODE_CMDLINE))
    {
	wait_return(FALSE);
	প্রত্যাগম FALSE;
    }
    out_flush();
    প্রত্যাগম TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    শূন্য
বার্তা_check(শূন্য)
{
    যদি (বার্তা_row == Rows - 1 && বার্তা_col >= sc_col
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
    স্থির শূন্য
redir_write(char_u *str, পূর্ণাঙ্ক maxlen)
{
    char_u	*s = str;
    স্থির পূর্ণাঙ্ক	cur_col = 0;

    // Don't do anything for displaying prompts and the like.
    যদি (redir_off)
	প্রত্যাগম;

    // If 'verbosefile' is set prepare for writing in that file.
    যদি (*p_vfile != NUL && verbose_fd == রিক্ত)
	verbose_open();

    যদি (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	যদি (*s != '\n' && *s != '\r')
	{
	    যাবত্ (cur_col < বার্তা_col)
	    {
#ifdef FEAT_EVAL
		যদি (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		অন্যথা যদি (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		অন্যথা যদি (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		অন্যথা
#endif
		    যদি (redir_fd != রিক্ত)
		    fputs(" ", redir_fd);
		যদি (verbose_fd != রিক্ত)
		    fputs(" ", verbose_fd);
		++cur_col;
	    }
	}

#ifdef FEAT_EVAL
	যদি (redir_execute)
	    execute_redir_str(s, maxlen);
	অন্যথা যদি (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	অন্যথা যদি (redir_vname)
	    var_redir_str(s, maxlen);
#endif

	// Write and adjust the current column.
	যাবত্ (*s != NUL && (maxlen < 0 || (পূর্ণাঙ্ক)(s - str) < maxlen))
	{
#ifdef FEAT_EVAL
	    যদি (!redir_reg && !redir_vname && !redir_execute)
#endif
		যদি (redir_fd != রিক্ত)
		    putc(*s, redir_fd);
	    যদি (verbose_fd != রিক্ত)
		putc(*s, verbose_fd);
	    যদি (*s == '\r' || *s == '\n')
		cur_col = 0;
	    অন্যথা যদি (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    অন্যথা
		++cur_col;
	    ++s;
	}

	যদি (বার্তা_silent != 0)	// should update msg_col
	    বার্তা_col = cur_col;
    }
}

    পূর্ণাঙ্ক
redirecting(শূন্য)
{
    প্রত্যাগম redir_fd != রিক্ত || *p_vfile != NUL
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#endif
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    শূন্য
verbose_enter(শূন্য)
{
    যদি (*p_vfile != NUL)
	++বার্তা_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    শূন্য
verbose_leave(শূন্য)
{
    যদি (*p_vfile != NUL)
	যদি (--বার্তা_silent < 0)
	    বার্তা_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    শূন্য
verbose_enter_scroll(শূন্য)
{
    যদি (*p_vfile != NUL)
	++বার্তা_silent;
    অন্যথা
	// always scroll up, don't overwrite
	বার্তা_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    শূন্য
verbose_leave_scroll(শূন্য)
{
    যদি (*p_vfile != NUL)
    {
	যদি (--বার্তা_silent < 0)
	    বার্তা_silent = 0;
    }
    অন্যথা
	cmdline_row = বার্তা_row;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    শূন্য
verbose_stop(শূন্য)
{
    যদি (verbose_fd != রিক্ত)
    {
	fclose(verbose_fd);
	verbose_fd = রিক্ত;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    পূর্ণাঙ্ক
verbose_open(শূন্য)
{
    যদি (verbose_fd == রিক্ত && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((বর্ণ *)p_vfile, "a");
	যদি (verbose_fd == রিক্ত)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    প্রত্যাগম FAIL;
	}
    }
    প্রত্যাগম OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    শূন্য
give_warning(char_u *message, পূর্ণাঙ্ক hl)
{
    give_warning_with_source(message, hl, FALSE);
}

    শূন্য
give_warning_with_source(char_u *message, পূর্ণাঙ্ক hl, পূর্ণাঙ্ক with_source)
{
    // Don't do this for ":silent".
    যদি (বার্তা_silent != 0)
	প্রত্যাগম;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, message, -1);
#endif
    VIM_CLEAR(keep_বার্তা);
    যদি (hl)
	keep_বার্তা_attr = HL_ATTR(HLF_W);
    অন্যথা
	keep_বার্তা_attr = 0;

    যদি (with_source)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	বার্তা_start();
	বার্তা_source(HL_ATTR(HLF_W));
	বার্তা_puts(" ");
	বার্তা_puts_attr((বর্ণ *)message, HL_ATTR(HLF_W) | বার্তা_HIST);
	বার্তা_clr_eos();
	(শূন্য)বার্তা_end();
    }
    অন্যথা যদি (বার্তা_attr((বর্ণ *)message, keep_বার্তা_attr) && বার্তা_scrolled == 0)
	set_keep_বার্তা(message, keep_বার্তা_attr);

    বার্তা_didout = FALSE;	    // overwrite this message
    বার্তা_nowait = TRUE;	    // don't wait for this message
    বার্তা_col = 0;

    --no_wait_return;
}

#যদি defined(FEAT_EVAL)
    শূন্য
give_warning2(char_u *message, char_u *a1, পূর্ণাঙ্ক hl)
{
    যদি (IObuff == রিক্ত)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(message, hl);
    }
    অন্যথা
    {
	vim_snprintf((বর্ণ *)IObuff, IOSIZE, (বর্ণ *)message, a1);
	give_warning(IObuff, hl);
    }
}
#endif

/*
 * Advance msg cursor to column "col".
 */
    শূন্য
বার্তা_advance(পূর্ণাঙ্ক col)
{
    যদি (বার্তা_silent != 0)	// nothing to advance to
    {
	বার্তা_col = col;		// for redirection, may fill it up later
	প্রত্যাগম;
    }
    যদি (col >= cmdline_width)	// not enough room
	col = cmdline_width - 1;
#ifdef FEAT_RIGHTLEFT
    যদি (cmdmsg_rl)
	যাবত্ (বার্তা_col > cmdline_width - col)
	    বার্তা_putchar(' ');
    অন্যথা
#endif
	যাবত্ (বার্তা_col < col)
	    বার্তা_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    শূন্য
বার্তা_warn_missing_clipboard(শূন্য)
{
    যদি (!global_busy && !did_warn_clipboard && silence_w23_w24_বার্তা == 0)
    {
#ifdef FEAT_CLIPBOARD
	বার্তা(_("W23: Clipboard register not available, using register 0"));
#অন্যথা
	বার্তা(_("W24: Clipboard register not available. See :h W24"));
#endif
	did_warn_clipboard = TRUE;
    }
}

#যদি defined(FEAT_CON_DIALOG)
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
    পূর্ণাঙ্ক
do_dialog(
    পূর্ণাঙ্ক		type UNUSED,
    char_u	*title UNUSED,
    char_u	*message,
    char_u	*buttons,
    পূর্ণাঙ্ক		dfltbutton,
    char_u	*textfield UNUSED,	// IObuff for inputdialog(), NULL
					// otherwise
    পূর্ণাঙ্ক		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    পূর্ণাঙ্ক		oldState;
    পূর্ণাঙ্ক		retval = 0;
    char_u	*hotkeys;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    যদি (silent_mode)
	প্রত্যাগম dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    যদি (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == রিক্ত)
    {
	// --gui-dialog-file: write text to a file
	যদি (gui_dialog_log(title, message))
	    c = dfltbutton;
	অন্যথা
	    c = gui_mch_dialog(type, title, message, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_row = বার্তা_row;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	out_flush();
	gui_mch_update();

	প্রত্যাগম c;
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
    hotkeys = বার্তা_show_console_dialog(message, buttons, dfltbutton);

    যদি (hotkeys != রিক্ত)
    {
	প্রতি (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    বিকল্প (c)
	    {
	    পক্ষ CAR:		// User accepts default option
	    পক্ষ NL:
		retval = dfltbutton;
		ভঙ্গ;
	    পক্ষ Ctrl_C:	// User aborts/cancels
	    পক্ষ ESC:
		retval = 0;
		ভঙ্গ;
	    স্বভাব:		// Could be a hotkey?
		যদি (c < 0)	// special keys are ignored here
		    অনুবর্ত;
		যদি (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    ভঙ্গ;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		প্রতি (i = 0; hotkeys[i]; ++i)
		{
		    যদি (has_mbyte)
		    {
			যদি ((*mb_ptr2char)(hotkeys + i) == c)
			    ভঙ্গ;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    অন্যথা
			যদি (hotkeys[i] == c)
			    ভঙ্গ;
		    ++retval;
		}
		যদি (hotkeys[i])
		    ভঙ্গ;
		// No hotkey match, so keep waiting
		অনুবর্ত;
	    }
	    ভঙ্গ;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    বার্তা_end_prompt();

    প্রত্যাগম retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    স্থির পূর্ণাঙ্ক
copy_char(
    char_u	*from,
    char_u	*to,
    পূর্ণাঙ্ক		lowercase)	// make character lower case
{
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		c;

    যদি (has_mbyte)
    {
	যদি (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    প্রত্যাগম (*mb_char2bytes)(c, to);
	}
	অন্যথা
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(to, from, (size_t)len);
	    প্রত্যাগম len;
	}
    }
    অন্যথা
    {
	যদি (lowercase)
	    *to = (char_u)TOLOWER_LOC(*from);
	অন্যথা
	    *to = *from;
	প্রত্যাগম 1;
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
    স্থির char_u *
বার্তা_show_console_dialog(
    char_u	*message,
    char_u	*buttons,
    পূর্ণাঙ্ক		dfltbutton)
{
    পূর্ণাঙ্ক		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    পূর্ণাঙ্ক		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = রিক্ত;
    char_u	*msgp = রিক্ত;
    char_u	*hotkp = রিক্ত;
    char_u	*r;
    পূর্ণাঙ্ক		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    পূর্ণাঙ্ক		first_hotkey = FALSE;	// first char of button is hotkey
    পূর্ণাঙ্ক		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    প্রতি (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	যাবত্ (*r)
	{
	    যদি (*r == DLG_BUTTON_SEP)
	    {
		যদি (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    যদি (has_mbyte)
			hotkp += STRLEN(hotkp);
		    অন্যথা
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    যদি (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    যদি (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			first_hotkey = TRUE;
		}
		অন্যথা
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    যদি (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    অন্যথা যদি (*r == DLG_HOTKEY_CHAR || first_hotkey)
	    {
		যদি (*r == DLG_HOTKEY_CHAR)
		    ++r;
		first_hotkey = FALSE;
		যদি (copy)
		{
		    যদি (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    অন্যথা
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		অন্যথা
		{
		    ++len;	    // '&a' -> '[a]'
		    যদি (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    অন্যথা
	    {
		// everything else copy literally
		যদি (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	যদি (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	অন্যথা
	{
	    len += (পূর্ণাঙ্ক)(STRLEN(message)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    যদি (!has_hotkey[0])
	    {
		first_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_বার্তা);
	    confirm_বার্তা = alloc(len);
	    যদি (confirm_বার্তা == রিক্ত)
		প্রত্যাগম রিক্ত;
	    *confirm_বার্তা = NUL;
	    hotk = alloc(lenhotkey);
	    যদি (hotk == রিক্ত)
		প্রত্যাগম রিক্ত;

	    *confirm_বার্তা = '\n';
	    STRCPY(confirm_বার্তা + 1, message);

	    msgp = confirm_বার্তা + 1 + STRLEN(message);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_বার্তা_tail = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_বার্তা();
    প্রত্যাগম hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    স্থির শূন্য
display_confirm_বার্তা(শূন্য)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_বার্তা_used;
    যদি (confirm_বার্তা != রিক্ত)
	বার্তা_puts_attr((বর্ণ *)confirm_বার্তা, HL_ATTR(HLF_M));
    --confirm_বার্তা_used;
}

#endif // FEAT_CON_DIALOG

#যদি defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    পূর্ণাঙ্ক
vim_dialog_yesno(
    পূর্ণাঙ্ক		type,
    char_u	*title,
    char_u	*message,
    পূর্ণাঙ্ক		dflt)
{
    যদি (do_dialog(type,
		title == রিক্ত ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No"), dflt, রিক্ত, FALSE) == 1)
	প্রত্যাগম VIM_YES;
    প্রত্যাগম VIM_NO;
}

    পূর্ণাঙ্ক
vim_dialog_yesnocancel(
    পূর্ণাঙ্ক		type,
    char_u	*title,
    char_u	*message,
    পূর্ণাঙ্ক		dflt)
{
    বিকল্প (do_dialog(type,
		title == রিক্ত ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, রিক্ত, FALSE))
    {
	পক্ষ 1: প্রত্যাগম VIM_YES;
	পক্ষ 2: প্রত্যাগম VIM_NO;
    }
    প্রত্যাগম VIM_CANCEL;
}

    পূর্ণাঙ্ক
vim_dialog_yesnoallcancel(
    পূর্ণাঙ্ক		type,
    char_u	*title,
    char_u	*message,
    পূর্ণাঙ্ক		dflt)
{
    বিকল্প (do_dialog(type,
		title == রিক্ত ? (char_u *)"Question" : title,
		message,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, রিক্ত, FALSE))
    {
	পক্ষ 1: প্রত্যাগম VIM_YES;
	পক্ষ 2: প্রত্যাগম VIM_NO;
	পক্ষ 3: প্রত্যাগম VIM_ALL;
	পক্ষ 4: প্রত্যাগম VIM_DISCARDALL;
    }
    প্রত্যাগম VIM_CANCEL;
}

#endif // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
