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

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ ಶೂನ್ಯ add_ಸಂದೇಶ_hist(char_u *s, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ check_ಸಂದೇಶ_hist(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ hit_return_ಸಂದೇಶ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_home_replace_attr(char_u *fname, ಪೂರ್ಣಾಙ್ಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_puts_attr_len(ವರ್ಣ *str, ಪೂರ್ಣಾಙ್ಕ maxlen, ಪೂರ್ಣಾಙ್ಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_puts_display(char_u *str, ಪೂರ್ಣಾಙ್ಕ maxlen, ಪೂರ್ಣಾಙ್ಕ attr, ಪೂರ್ಣಾಙ್ಕ recurse);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_scroll_up(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ inc_ಸಂದೇಶ_scrolled(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ store_sb_text(char_u **sb_str, char_u *s, ಪೂರ್ಣಾಙ್ಕ attr, ಪೂರ್ಣಾಙ್ಕ *sb_col, ಪೂರ್ಣಾಙ್ಕ finish);
ಸ್ಥಿರ ಶೂನ್ಯ t_puts(ಪೂರ್ಣಾಙ್ಕ *t_col, char_u *t_s, char_u *s, ಪೂರ್ಣಾಙ್ಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_puts_printf(char_u *str, ಪೂರ್ಣಾಙ್ಕ maxlen);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ do_more_prompt(ಪೂರ್ಣಾಙ್ಕ typed_char);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_screen_putchar(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_moremsg(ಪೂರ್ಣಾಙ್ಕ full);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  ಸಂದೇಶ_check_screen(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ redir_write(char_u *s, ಪೂರ್ಣಾಙ್ಕ maxlen);
#ifdef FEAT_CON_DIALOG
ಸ್ಥಿರ char_u *ಸಂದೇಶ_show_console_dialog(char_u *message, char_u *buttons, ಪೂರ್ಣಾಙ್ಕ dfltbutton);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	confirm_ಸಂದೇಶ_used = FALSE;	// displaying confirm_msg
ಸ್ಥಿರ char_u	*confirm_ಸಂದೇಶ = ರಿಕ್ತ;		// ":confirm" message
ಸ್ಥಿರ char_u	*confirm_ಸಂದೇಶ_tail;		// tail of confirm_msg
ಸ್ಥಿರ ಶೂನ್ಯ display_confirm_ಸಂದೇಶ(ಶೂನ್ಯ);
#endif
#ifdef FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ emsg_to_channel_log = FALSE;
#endif

ಸಂರಚನಾ ಸಂದೇಶ_hist
{
    ಸಂರಚನಾ ಸಂದೇಶ_hist	*next;
    char_u		*ಸಂದೇಶ;
    ಪೂರ್ಣಾಙ್ಕ			attr;
};

ಸ್ಥಿರ ಸಂರಚನಾ ಸಂದೇಶ_hist *first_ಸಂದೇಶ_hist = ರಿಕ್ತ;
ಸ್ಥಿರ ಸಂರಚನಾ ಸಂದೇಶ_hist *last_ಸಂದೇಶ_hist = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ_hist_len = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#define MESSAGES_HIT_ENTER	0x001
#define MESSAGES_WAIT		0x002
#define MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#define MESSAGES_OPT_HIT_ENTER "hit-enter"
#define MESSAGES_OPT_WAIT "wait:"
#define MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ_flags = MESSAGES_HIT_ENTER | MESSAGES_HISTORY;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ_wait = 0;

ಸ್ಥಿರ FILE *verbose_fd = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  verbose_did_open = FALSE;

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
    ಪೂರ್ಣಾಙ್ಕ
ಸಂದೇಶ(ವರ್ಣ *s)
{
    ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    ಪೂರ್ಣಾಙ್ಕ
verb_ಸಂದೇಶ(ವರ್ಣ *s)
{
    ಪೂರ್ಣಾಙ್ಕ		n;

    verbose_enter();
    n = ಸಂದೇಶ_attr_keep(s, 0, FALSE);
    verbose_leave();

    ಪ್ರತ್ಯಾಗಮ n;
}

    ಪೂರ್ಣಾಙ್ಕ
ಸಂದೇಶ_attr(ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_attr_keep(s, attr, FALSE);
}

    ಪೂರ್ಣಾಙ್ಕ
ಸಂದೇಶ_attr_keep(
    ವರ್ಣ	*s,
    ಪೂರ್ಣಾಙ್ಕ		attr,
    ಪೂರ್ಣಾಙ್ಕ		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	entered = 0;
    ಪೂರ್ಣಾಙ್ಕ		retval;
    char_u	*buf = ರಿಕ್ತ;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    ಯದಿ (!emsg_on_display && message_filtered((char_u *)s))
	ಪ್ರತ್ಯಾಗಮ TRUE;

#ifdef FEAT_EVAL
    ಯದಿ (attr == 0)
	set_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#endif

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    ಯದಿ (entered >= 3)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    ಯದಿ ((char_u *)s != keep_ಸಂದೇಶ
	    || (*s != '<'
		&& last_ಸಂದೇಶ_hist != ರಿಕ್ತ
		&& last_ಸಂದೇಶ_hist->ಸಂದೇಶ != ರಿಕ್ತ
		&& STRCMP(s, last_ಸಂದೇಶ_hist->ಸಂದೇಶ) != 0))
	add_ಸಂದೇಶ_hist((char_u *)s, -1, attr);

#ifdef FEAT_EVAL
    ಯದಿ (emsg_to_channel_log)
	// Write message in the channel log.
	ch_log(ರಿಕ್ತ, "ERROR: %s", s);
#endif

    // Truncate the message if needed.
    ಸಂದೇಶ_start();
    buf = ಸಂದೇಶ_strtrunc((char_u *)s, FALSE);
    ಯದಿ (buf != ರಿಕ್ತ)
	s = (ವರ್ಣ *)buf;

    ಸಂದೇಶ_outtrans_attr((char_u *)s, attr);
    ಸಂದೇಶ_clr_eos();
    retval = ಸಂದೇಶ_end();

    ಯದಿ (keep && retval && vim_strsize((char_u *)s)
		    < (ಪೂರ್ಣಾಙ್ಕ)(Rows - cmdline_row - 1) * cmdline_width + sc_col)
	set_keep_ಸಂದೇಶ((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
ಸಂದೇಶ_strtrunc(
    char_u	*s,
    ಪೂರ್ಣಾಙ್ಕ		force)	    // always truncate
{
    char_u	*buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		room;

    // May truncate message to avoid a hit-return prompt
    ಯದಿ ((!ಸಂದೇಶ_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_active && ಸಂದೇಶ_silent == 0) || force)
    {
	len = vim_strsize(s);
	ಯದಿ (ಸಂದೇಶ_scrolled != 0
#ifdef HAS_MESSAGE_WINDOW
		|| in_echowindow
#endif
		)
	    // Use all the columns.
	    room = (ಪೂರ್ಣಾಙ್ಕ)(Rows - ಸಂದೇಶ_row) * cmdline_width - 1;
	ಅನ್ಯಥಾ
	    // Use up to 'showcmd' column.
	    room = (ಪೂರ್ಣಾಙ್ಕ)(Rows - ಸಂದೇಶ_row - 1) * cmdline_width + sc_col - 1;
	ಯದಿ (len > room && room > 0)
	{
	    ಯದಿ (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (room + 2) * 18;
	    ಅನ್ಯಥಾ ಯದಿ (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (room + 2) * 2;
	    ಅನ್ಯಥಾ
		len = room + 2;
	    buf = alloc(len);
	    ಯದಿ (buf != ರಿಕ್ತ)
		trunc_string(s, buf, room, len);
	}
    }
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    ಶೂನ್ಯ
trunc_string(
    char_u	*s,
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		room_in,
    ಪೂರ್ಣಾಙ್ಕ		buflen)
{
    size_t	room = room_in - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    ಪೂರ್ಣಾಙ್ಕ		e;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯದಿ (*s == NUL)
    {
	ಯದಿ (buflen > 0)
	    *buf = NUL;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (room_in < 3)
	room = 0;
    half = room / 2;

    // First part: Start of the string.
    ಪ್ರತಿ (e = 0; len < half && e < buflen; ++e)
    {
	ಯದಿ (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	n = ptr2cells(s + e);
	ಯದಿ (len + n > half)
	    ಭಙ್ಗ;
	len += n;
	buf[e] = s[e];
	ಯದಿ (has_mbyte)
	    ಪ್ರತಿ (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		ಯದಿ (++e == buflen)
		    ಭಙ್ಗ;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    ಯದಿ (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	ಯಾವತ್ (len + n > room)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);
	ಪ್ರತಿ (;;)
	{
	    ಕುರು
		half = half - utf_head_off(s, s + half - 1) - 1;
	    ಯಾವತ್ (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    ಯದಿ (len + n > room || half == 0)
		ಭಙ್ಗ;
	    len += n;
	    i = (ಪೂರ್ಣಾಙ್ಕ)half;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= room; --i)
	    len += n;
    }


    ಯದಿ (i <= e + 3)
    {
	// text fits without truncating
	ಯದಿ (s != buf)
	{
	    len = STRLEN(s);
	    ಯದಿ (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    ಯದಿ (len < 1)
		buf[e - 1] = NUL;
	    ಅನ್ಯಥಾ
		mch_memmove(buf + e, s + e, len);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	ಯದಿ (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    ಅನ್ಯಥಾ
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

ಪೂರ್ಣಾಙ್ಕ vim_snprintf(ವರ್ಣ *str, size_t str_m, ಧ್ರುವ ವರ್ಣ *fmt, ...);

    ಪೂರ್ಣಾಙ್ಕ
smsg(ಧ್ರುವ ವರ್ಣ *s, ...)
{
    ಯದಿ (IObuff == ರಿಕ್ತ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ((ವರ್ಣ *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ವರ್ಣ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ((ವರ್ಣ *)IObuff);
}

    ಪೂರ್ಣಾಙ್ಕ
smsg_attr(ಪೂರ್ಣಾಙ್ಕ attr, ಧ್ರುವ ವರ್ಣ *s, ...)
{
    ಯದಿ (IObuff == ರಿಕ್ತ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_attr((ವರ್ಣ *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ವರ್ಣ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_attr((ವರ್ಣ *)IObuff, attr);
}

    ಪೂರ್ಣಾಙ್ಕ
smsg_attr_keep(ಪೂರ್ಣಾಙ್ಕ attr, ಧ್ರುವ ವರ್ಣ *s, ...)
{
    ಯದಿ (IObuff == ರಿಕ್ತ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_attr_keep((ವರ್ಣ *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ವರ್ಣ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_attr_keep((ವರ್ಣ *)IObuff, attr, TRUE);
}

#endif

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	last_sourcing_lnum = 0;
ಸ್ಥಿರ char_u   *last_sourcing_name = ರಿಕ್ತ;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    ಶೂನ್ಯ
reset_last_sourcing(ಶೂನ್ಯ)
{
    VIM_CLEAR(last_sourcing_name);
    last_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
other_sourcing_name(ಶೂನ್ಯ)
{
    ಯದಿ (HAVE_SOURCING_INFO && SOURCING_NAME != ರಿಕ್ತ)
    {
	ಯದಿ (last_sourcing_name != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ STRCMP(SOURCING_NAME, last_sourcing_name) != 0;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ಸ್ಥಿರ char_u *
get_emsg_source(ಶೂನ್ಯ)
{
    char_u	*Buf, *p;

    ಯದಿ (HAVE_SOURCING_INFO && SOURCING_NAME != ರಿಕ್ತ && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	ಯದಿ (sname == ರಿಕ್ತ)
	    sname = SOURCING_NAME;

#ifdef FEAT_EVAL
	ಯದಿ (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	ಅನ್ಯಥಾ
#endif
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	ಯದಿ (Buf != ರಿಕ್ತ)
	    sprintf((ವರ್ಣ *)Buf, (ವರ್ಣ *)p, sname);
	vim_free(tofree);
	ಪ್ರತ್ಯಾಗಮ Buf;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ಸ್ಥಿರ char_u *
get_emsg_lnum(ಶೂನ್ಯ)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    ಯದಿ (SOURCING_NAME != ರಿಕ್ತ
	    && (other_sourcing_name() || SOURCING_LNUM != last_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	ಯದಿ (Buf != ರಿಕ್ತ)
	    sprintf((ವರ್ಣ *)Buf, (ವರ್ಣ *)p, (long)SOURCING_LNUM);
	ಪ್ರತ್ಯಾಗಮ Buf;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_source(ಪೂರ್ಣಾಙ್ಕ attr)
{
    char_u	*p;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = FALSE;

    // Bail out if something called here causes an error.
    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_source();
    ಯದಿ (p != ರಿಕ್ತ)
    {
	ಸಂದೇಶ_scroll = TRUE;  // this will take more than one line
	ಸಂದೇಶ_attr((ವರ್ಣ *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    ಯದಿ (p != ರಿಕ್ತ)
    {
	ಸಂದೇಶ_attr((ವರ್ಣ *)p, HL_ATTR(HLF_N));
	vim_free(p);
	last_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    ಯದಿ (SOURCING_NAME == ರಿಕ್ತ || other_sourcing_name())
    {
	VIM_CLEAR(last_sourcing_name);
	ಯದಿ (SOURCING_NAME != ರಿಕ್ತ)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
emsg_not_now(ಶೂನ್ಯ)
{
    ಯದಿ ((emsg_off > 0 && vim_strchr(p_debug, 'm') == ರಿಕ್ತ
					  && vim_strchr(p_debug, 't') == ರಿಕ್ತ)
#ifdef FEAT_EVAL
	    || emsg_skip > 0
#endif
	    )
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ಯದಿ defined(FEAT_EVAL)
ಸ್ಥಿರ garray_T ignore_error_list = GA_EMPTY;

    ಶೂನ್ಯ
ignore_error_for_testing(char_u *error)
{
    ಯದಿ (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, ಪರಿಮಾಣ(char_u *), 1);

    ಯದಿ (STRCMP("RESET", error) == 0)
	ga_clear_strings(&ignore_error_list);
    ಅನ್ಯಥಾ
	ga_copy_string(&ignore_error_list, error);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ignore_error(ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < ignore_error_list.ga_len; ++i)
	ಯದಿ (strstr(ಸಂದೇಶ,
		  (ವರ್ಣ *)((char_u **)(ignore_error_list.ga_data))[i]) != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

#ಯದಿ !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    ಶೂನ್ಯ
do_perror(ವರ್ಣ *ಸಂದೇಶ)
{
    perror(ಸಂದೇಶ);
    ++emsg_silent;
    emsg(ಸಂದೇಶ);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
emsg_core(ಧ್ರುವ ವರ್ಣ *s)
{
    ಪೂರ್ಣಾಙ್ಕ		attr;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		r;
#ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ		ignore = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		severe;
#endif

#ifdef FEAT_EVAL
    // When testing some errors are turned into a normal message.
    ಯದಿ (ignore_error(s))
	// don't call msg() if it results in a dialog
	ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_use_printf() ? FALSE : ಸಂದೇಶ((ವರ್ಣ *)s);
#endif

    ++called_emsg;

#ifdef FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#endif

    ಯದಿ (!emsg_off || vim_strchr(p_debug, 't') != ರಿಕ್ತ)
    {
#ifdef FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	ಯದಿ (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    ಯದಿ (!ignore)
		++did_emsg;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}

	ಯದಿ (in_assert_fails && emsg_assert_fails_ಸಂದೇಶ == ರಿಕ್ತ)
	{
	    emsg_assert_fails_ಸಂದೇಶ = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == ರಿಕ್ತ ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#endif

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	ಯದಿ (emsg_silent != 0)
	{
#ifdef FEAT_EVAL
	    ++did_emsg_silent;
#endif
	    ಯದಿ (emsg_noredir == 0)
	    {
		ಸಂದೇಶ_start();
		p = get_emsg_source();
		ಯದಿ (p != ರಿಕ್ತ)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		ಯದಿ (p != ರಿಕ್ತ)
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
	    ಯದಿ (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#endif
#ifdef FEAT_EVAL
	    ch_log(ರಿಕ್ತ, "ERROR silent: %s", s);
#endif
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	ಸಂದೇಶ_silent = 0;
	cmd_silent = FALSE;

	ಯದಿ (global_busy)		// break :global command
	    ++global_busy;

	ಯದಿ (p_eb)
	    beep_flush();		// also includes flush_buffers()
	ಅನ್ಯಥಾ
	    flush_buffers(FLUSH_MINIMAL);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ifdef FEAT_EVAL
	++uncaught_emsg;
#endif
    }

#ifdef HAS_MESSAGE_WINDOW
    ಯದಿ (!in_echowindow)
#endif
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    ಯದಿ (ಸಂದೇಶ_scrolled != 0)
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
    ಸಂದೇಶ_scroll = TRUE;
    ಸಂದೇಶ_source(attr);

    /*
     * Display the error message itself.
     */
    ಸಂದೇಶ_nowait = FALSE;			// wait for this msg
    r = ಸಂದೇಶ_attr((ವರ್ಣ *)s, attr);

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = FALSE;
#endif
    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    ಪೂರ್ಣಾಙ್ಕ
emsg(ವರ್ಣ *s)
{
    // Skip this if not giving error messages at the moment.
    ಯದಿ (emsg_not_now())
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಪ್ರತ್ಯಾಗಮ emsg_core(s);
}

#ifndef PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    ಪೂರ್ಣಾಙ್ಕ
semsg(ಧ್ರುವ ವರ್ಣ *s, ...)
{
    // Skip this if not giving error messages at the moment.
    ಯದಿ (emsg_not_now())
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಯದಿ (IObuff == ರಿಕ್ತ)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ಪ್ರತ್ಯಾಗಮ emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((ವರ್ಣ *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    ಪ್ರತ್ಯಾಗಮ emsg_core((ವರ್ಣ *)IObuff);
}
#endif

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    ಶೂನ್ಯ
iemsg(ವರ್ಣ *s)
{
    ಯದಿ (emsg_not_now())
	ಪ್ರತ್ಯಾಗಮ;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    emsg_core(s);
#ಯದಿ defined(ABORT_ON_INTERNAL_ERROR) && defined(FEAT_EVAL)
    set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    ಸಂದೇಶ_putchar('\n');  // avoid overwriting the error message
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
    ಶೂನ್ಯ
siemsg(ಧ್ರುವ ವರ್ಣ *s, ...)
{
    ಯದಿ (emsg_not_now())
	ಪ್ರತ್ಯಾಗಮ;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    ಯದಿ (IObuff == ರಿಕ್ತ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    ಅನ್ಯಥಾ
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((ವರ್ಣ *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((ವರ್ಣ *)IObuff);
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    ಸಂದೇಶ_putchar('\n');  // avoid overwriting the error message
    out_flush();
    abort();
# endif
}
#endif

/*
 * Give an "Internal error" message.
 */
    ಶೂನ್ಯ
internal_error(ವರ್ಣ *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    siemsg(_(e_internal_error_str), where);
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    ಶೂನ್ಯ
internal_error_no_abort(ವರ್ಣ *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    semsg(_(e_internal_error_str), where);
}
#endif

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    ಶೂನ್ಯ
emsg_invreg(ಪೂರ್ಣಾಙ್ಕ name)
{
    semsg(_(e_invalid_register_name_str), transchar_buf(ರಿಕ್ತ, name));
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    ಶೂನ್ಯ
emsg_namelen(ವರ್ಣ *ಸಂದೇಶ, char_u *name, ಪೂರ್ಣಾಙ್ಕ len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(ಸಂದೇಶ, copy == ರಿಕ್ತ ? "NULL" : (ವರ್ಣ *)copy);
    vim_free(copy);
}
#endif

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    ವರ್ಣ *
ಸಂದೇಶ_trunc_attr(ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ force, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    ವರ್ಣ	*ts;

    // Add message to history before truncating
    add_ಸಂದೇಶ_hist((char_u *)s, -1, attr);

    ts = (ವರ್ಣ *)ಸಂದೇಶ_may_trunc(force, (char_u *)s);

    ಸಂದೇಶ_hist_off = TRUE;
    n = ಸಂದೇಶ_attr(ts, attr);
    ಸಂದೇಶ_hist_off = FALSE;

    ಯದಿ (n)
	ಪ್ರತ್ಯಾಗಮ ts;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
ಸಂದೇಶ_may_trunc(ಪೂರ್ಣಾಙ್ಕ force, char_u *s)
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		room;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    room = (ಪೂರ್ಣಾಙ್ಕ)(Rows - cmdline_row - 1) * cmdline_width + sc_col - 1;
    ಯದಿ (room > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_active))
	    && (n = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s) - room) > 0)
    {
	ಯದಿ (has_mbyte)
	{
	    ಪೂರ್ಣಾಙ್ಕ	size = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    ಯದಿ (size <= room)
		ಪ್ರತ್ಯಾಗಮ s;

	    ಪ್ರತಿ (n = 0; size >= room; )
	    {
		size -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
add_ಸಂದೇಶ_hist(
    char_u	*s,
    ಪೂರ್ಣಾಙ್ಕ		len,		// -1 for undetermined length
    ಪೂರ್ಣಾಙ್ಕ		attr)
{
    ಸಂರಚನಾ ಸಂದೇಶ_hist *p;

    ಯದಿ (ಸಂದೇಶ_hist_off || ಸಂದೇಶ_silent != 0)
	ಪ್ರತ್ಯಾಗಮ;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(ಸಂರಚನಾ ಸಂದೇಶ_hist);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (len < 0)
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);
    // remove leading and trailing newlines
    ಯಾವತ್ (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    ಯಾವತ್ (len > 0 && s[len - 1] == '\n')
	--len;
    p->ಸಂದೇಶ = vim_strnsave(s, len);
    p->next = ರಿಕ್ತ;
    p->attr = attr;
    ಯದಿ (last_ಸಂದೇಶ_hist != ರಿಕ್ತ)
	last_ಸಂದೇಶ_hist->next = p;
    last_ಸಂದೇಶ_hist = p;
    ಯದಿ (first_ಸಂದೇಶ_hist == ರಿಕ್ತ)
	first_ಸಂದೇಶ_hist = last_ಸಂದೇಶ_hist;
    ++ಸಂದೇಶ_hist_len;

    check_ಸಂದೇಶ_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    ಪೂರ್ಣಾಙ್ಕ
delete_first_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಸಂರಚನಾ ಸಂದೇಶ_hist *p;

    ಯದಿ (ಸಂದೇಶ_hist_len <= 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    p = first_ಸಂದೇಶ_hist;
    first_ಸಂದೇಶ_hist = p->next;
    ಯದಿ (first_ಸಂದೇಶ_hist == ರಿಕ್ತ)
	last_ಸಂದೇಶ_hist = ರಿಕ್ತ;  // history is empty
    vim_free(p->ಸಂದೇಶ);
    vim_free(p);
    --ಸಂದೇಶ_hist_len;
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
check_ಸಂದೇಶ_hist(ಶೂನ್ಯ)
{
    // Don't let the message history get too big
    ಯಾವತ್ (ಸಂದೇಶ_hist_len > 0 && ಸಂದೇಶ_hist_len > ಸಂದೇಶ_hist_max)
	(ಶೂನ್ಯ)delete_first_ಸಂದೇಶ();
}


    ಪೂರ್ಣಾಙ್ಕ
messagesopt_changed(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		messages_flags_new = 0;
    ಪೂರ್ಣಾಙ್ಕ		messages_wait_new = 0;
    ಪೂರ್ಣಾಙ್ಕ		messages_history_new = 0;

    p = p_mopt;
    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (STRNCMP(p, MESSAGES_OPT_HIT_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER);
	    messages_flags_new |= MESSAGES_HIT_ENTER;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	ಯದಿ (*p != ',' && *p != NUL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    ಯದಿ (!(messages_flags_new & (MESSAGES_HIT_ENTER | MESSAGES_WAIT)))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // "history" must be set
    ಯದಿ (!(messages_flags_new & MESSAGES_HISTORY))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (messages_history_new < 0 || messages_history_new > 10000)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (messages_wait_new < 0 || messages_wait_new > 10000)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಸಂದೇಶ_flags = messages_flags_new;
    ಸಂದೇಶ_wait = messages_wait_new;

    ಸಂದೇಶ_hist_max = messages_history_new;
    check_ಸಂದೇಶ_hist();

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * ":messages" command.
 */
    ಶೂನ್ಯ
ex_messages(exarg_T *eap)
{
    ಸಂರಚನಾ ಸಂದೇಶ_hist *p;
    char_u	    *s;
    ಪೂರ್ಣಾಙ್ಕ		    c = 0;

    ಯದಿ (STRCMP(eap->arg, "clear") == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ keep = eap->addr_ಎಣಿಕೆ == 0 ? 0 : eap->line2;

	ಯಾವತ್ (ಸಂದೇಶ_hist_len > keep)
	    (ಶೂನ್ಯ)delete_first_ಸಂದೇಶ();
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (*eap->arg != NUL)
    {
	emsg(_(e_invalid_argument));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಸಂದೇಶ_hist_off = TRUE;

    p = first_ಸಂದೇಶ_hist;
    ಯದಿ (eap->addr_ಎಣಿಕೆ != 0)
    {
	// Count total messages
	ಪ್ರತಿ (; p != ರಿಕ್ತ && !got_int; p = p->next)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	ಪ್ರತಿ (p = first_ಸಂದೇಶ_hist; p != ರಿಕ್ತ && !got_int && c > 0;
						    p = p->next, c--);
    }

    ಯದಿ (p == first_ಸಂದೇಶ_hist)
    {
#ifdef FEAT_MULTI_LANG
	s = get_mess_lang();
#ಅನ್ಯಥಾ
	s = mch_getenv((char_u *)"LANG");
#endif
	ಯದಿ (s != ರಿಕ್ತ && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    ಸಂದೇಶ_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    ಪ್ರತಿ (; p != ರಿಕ್ತ && !got_int; p = p->next)
	ಯದಿ (p->ಸಂದೇಶ != ರಿಕ್ತ)
	    ಸಂದೇಶ_attr((ವರ್ಣ *)p->ಸಂದೇಶ, p->attr);

    ಸಂದೇಶ_hist_off = FALSE;
}

#ಯದಿ defined(FEAT_CON_DIALOG) || defined(FIND_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_end_prompt(ಶೂನ್ಯ)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_row = ಸಂದೇಶ_row;
    ಸಂದೇಶ_col = 0;
    ಸಂದೇಶ_clr_eos();
    ಸಾಲುಗಳು_ಎಡ = -1;
}
#endif

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    ಶೂನ್ಯ
wait_return(ಪೂರ್ಣಾಙ್ಕ redraw)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		oldState;
    ಪೂರ್ಣಾಙ್ಕ		tmpState;
    ಪೂರ್ಣಾಙ್ಕ		had_got_int;
    ಪೂರ್ಣಾಙ್ಕ		save_reg_recording;
    FILE	*save_scriptout;

    ಯದಿ (redraw == TRUE)
	set_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    ಯದಿ (ಸಂದೇಶ_silent != 0)
	ಪ್ರತ್ಯಾಗಮ;
#ifdef HAS_MESSAGE_WINDOW
    ಯದಿ (in_echowindow)
	ಪ್ರತ್ಯಾಗಮ;
#endif

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    ಯದಿ (vgetc_busy > 0)
	ಪ್ರತ್ಯಾಗಮ;
    need_wait_return = TRUE;
    ಯದಿ (no_wait_return)
    {
	ಯದಿ (!exmode_active)
	    cmdline_row = ಸಂದೇಶ_row;
	ಪ್ರತ್ಯಾಗಮ;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    ಯದಿ (quit_more)
    {
	c = CAR;		// just pretend CR was hit
	quit_more = FALSE;
	got_int = FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (exmode_active)
    {
	ಸಂದೇಶ_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (!stuff_empty())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    ಅನ್ಯಥಾ
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = MODE_HITRETURN;
	setmouse();
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#endif
	cmdline_row = ಸಂದೇಶ_row;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	ಯದಿ (need_check_timestamps)
	    check_timestamps(FALSE);

	ಯದಿ (ಸಂದೇಶ_flags & MESSAGES_HIT_ENTER)
	{
	    hit_return_ಸಂದೇಶ();

	    ಕುರು
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
		++allow_ಕೀಲಿಗಳು;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = ರಿಕ್ತ;
		c = safe_vgetc();
		ಯದಿ (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_ಕೀಲಿಗಳು;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ifdef FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		ಯದಿ (ಕೀಲಿ_Typed && c == Ctrl_Y && clip_star.state == SELECT_DONE)
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
		ಯದಿ (ಕೀಲಿ_Typed && p_more && !p_cp)
		{
		    ಯದಿ (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_UP || c == K_PAGEUP)
		    {
			ಯದಿ (ಸಂದೇಶ_scrolled > Rows)
			    // scroll back to show older messages
			    do_more_prompt(c);
			ಅನ್ಯಥಾ
			{
			    ಸಂದೇಶ_didout = FALSE;
			    c = K_IGNORE;
			    ಸಂದೇಶ_col =
#ifdef FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_width - 1 :
#endif
				0;
			}
			ಯದಿ (quit_more)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_more = FALSE;
			    got_int = FALSE;
			}
			ಅನ್ಯಥಾ ಯದಿ (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    hit_return_ಸಂದೇಶ();
			}
		    }
		    ಅನ್ಯಥಾ ಯದಿ (ಸಂದೇಶ_scrolled > Rows - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_DOWN || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } ಯಾವತ್ ((had_got_int && c == Ctrl_C)
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
				    && mouse_row < ಸಂದೇಶ_row
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    ಯದಿ (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(ಶೂನ್ಯ)jump_to_mouse(MOUSE_SETPOS, ರಿಕ್ತ, 0);
	    ಅನ್ಯಥಾ ಯದಿ (!ಕೀಲಿ_Typed || (vim_strchr((char_u *)"\r\n ", c) == ರಿಕ್ತ
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_mask);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	ಅನ್ಯಥಾ
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(ಸಂದೇಶ_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    ಯದಿ (c == ':' || c == '?' || c == '/')
    {
	ಯದಿ (!exmode_active)
	    cmdline_row = ಸಂದೇಶ_row;
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
    ಸಂದೇಶ_check();

#ifndef MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    ಯದಿ (swapping_screen() && !termcap_active)
	newline_on_exit = TRUE;
#endif

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    ಸಾಲುಗಳು_ಎಡ = -1;		// reset lines_left at next msg_start()
    reset_last_sourcing();
    ಯದಿ (keep_ಸಂದೇಶ != ರಿಕ್ತ && vim_strsize(keep_ಸಂದೇಶ) >=
			    (Rows - cmdline_row - 1) * cmdline_width + sc_col)
	VIM_CLEAR(keep_ಸಂದೇಶ);	    // don't redisplay message, it's too long

    ಯದಿ (tmpState == MODE_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    ಅನ್ಯಥಾ ಯದಿ (!skip_redraw
	    && (redraw == TRUE || (ಸಂದೇಶ_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hit_return_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		save_p_more = p_more;

    p_more = FALSE;	// don't want to see this message when scrolling back
    ಯದಿ (ಸಂದೇಶ_didout)	// start on a new line
	ಸಂದೇಶ_putchar('\n');
    ಯದಿ (got_int)
	ಸಂದೇಶ_puts(_("Interrupt: "));

    ಸಂದೇಶ_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    ಯದಿ (!ಸಂದೇಶ_use_printf())
	ಸಂದೇಶ_clr_eos();
    p_more = save_p_more;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    ಶೂನ್ಯ
set_keep_ಸಂದೇಶ(char_u *s, ಪೂರ್ಣಾಙ್ಕ attr)
{
    vim_free(keep_ಸಂದೇಶ);
    ಯದಿ (s != ರಿಕ್ತ && ಸಂದೇಶ_silent == 0)
	keep_ಸಂದೇಶ = vim_strsave(s);
    ಅನ್ಯಥಾ
	keep_ಸಂದೇಶ = ರಿಕ್ತ;
    keep_ಸಂದೇಶ_more = FALSE;
    keep_ಸಂದೇಶ_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    ಶೂನ್ಯ
set_keep_ಸಂದೇಶ_from_hist(ಶೂನ್ಯ)
{
    ಯದಿ (keep_ಸಂದೇಶ == ರಿಕ್ತ && last_ಸಂದೇಶ_hist != ರಿಕ್ತ && ಸಂದೇಶ_scrolled == 0
						      && (State & MODE_NORMAL))
	set_keep_ಸಂದೇಶ(last_ಸಂದೇಶ_hist->ಸಂದೇಶ, last_ಸಂದೇಶ_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_start(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		did_return = FALSE;

    ಯದಿ (ಸಂದೇಶ_row < cmdline_row)
	ಸಂದೇಶ_row = cmdline_row;

    ಯದಿ (!ಸಂದೇಶ_silent)
    {
	VIM_CLEAR(keep_ಸಂದೇಶ);
	need_fileinfo = FALSE;
    }

#ifdef FEAT_EVAL
    ಯದಿ (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	ಸಂದೇಶ_clr_eos();
    }
#endif

#ifdef HAS_MESSAGE_WINDOW
    ಯದಿ (in_echowindow)
    {
	ಯದಿ (popup_message_win_visible()
		    && ((ಸಂದೇಶ_col > 0 && (ಸಂದೇಶ_scroll || !full_screen))
			|| in_echowindow))
	{
	    win_T *wp = popup_get_message_win();

	    // start a new line
	    curbuf = wp->w_buffer;
	    ml_append(wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_buffer;
	}
	ಸಂದೇಶ_col = 0;
    }
    ಅನ್ಯಥಾ
#endif
	ಯದಿ (!ಸಂದೇಶ_scroll && full_screen)	// overwrite last message
    {
	ಸಂದೇಶ_row = cmdline_row;
	ಸಂದೇಶ_col =
#ifdef FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_width - 1 :
#endif
	    0;
    }
    ಅನ್ಯಥಾ ಯದಿ (ಸಂದೇಶ_didout || in_echowindow)
    {
	// start message on next line
	ಸಂದೇಶ_putchar('\n');
	did_return = TRUE;
	ಯದಿ (exmode_active != EXMODE_NORMAL)
	    cmdline_row = ಸಂದೇಶ_row;
    }
    ಯದಿ (!ಸಂದೇಶ_didany || ಸಾಲುಗಳು_ಎಡ < 0)
	ಸಂದೇಶ_starthere();
    ಯದಿ (ಸಂದೇಶ_silent == 0)
    {
	ಸಂದೇಶ_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    ಯದಿ (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_starthere(ಶೂನ್ಯ)
{
    ಸಾಲುಗಳು_ಎಡ = cmdline_row;
    ಸಂದೇಶ_didany = FALSE;
}

    ಶೂನ್ಯ
ಸಂದೇಶ_putchar(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಸಂದೇಶ_putchar_attr(c, 0);
}

    ಶೂನ್ಯ
ಸಂದೇಶ_putchar_attr(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    ಯದಿ (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    ಅನ್ಯಥಾ
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    ಸಂದೇಶ_puts_attr((ವರ್ಣ *)buf, attr);
}

    ಶೂನ್ಯ
ಸಂದೇಶ_outnum(long n)
{
    ವರ್ಣ	buf[20];

    sprintf(buf, "%ld", n);
    ಸಂದೇಶ_puts(buf);
}

    ಶೂನ್ಯ
ಸಂದೇಶ_home_replace(char_u *fname)
{
    ಸಂದೇಶ_home_replace_attr(fname, 0);
}

#ಯದಿ defined(FEAT_FIND_ID)
    ಶೂನ್ಯ
ಸಂದೇಶ_home_replace_hl(char_u *fname)
{
    ಸಂದೇಶ_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_home_replace_attr(char_u *fname, ಪೂರ್ಣಾಙ್ಕ attr)
{
    char_u	*name;

    name = home_replace_save(ರಿಕ್ತ, fname);
    ಯದಿ (name != ರಿಕ್ತ)
	ಸಂದೇಶ_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    ಪೂರ್ಣಾಙ್ಕ
ಸಂದೇಶ_outtrans(char_u *str)
{
    ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_outtrans_attr(str, 0);
}

    ಪೂರ್ಣಾಙ್ಕ
ಸಂದೇಶ_outtrans_attr(char_u *str, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_outtrans_len_attr(str, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str), attr);
}

    ಪೂರ್ಣಾಙ್ಕ
ಸಂದೇಶ_outtrans_len(char_u *str, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
ಸಂದೇಶ_outtrans_one(char_u *p, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಪೂರ್ಣಾಙ್ಕ		l;

    ಯದಿ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	ಸಂದೇಶ_outtrans_len_attr(p, l, attr);
	ಪ್ರತ್ಯಾಗಮ p + l;
    }
    ಸಂದೇಶ_puts_attr((ವರ್ಣ *)transchar_byte_buf(ರಿಕ್ತ, *p), attr);
    ಪ್ರತ್ಯಾಗಮ p + 1;
}

    ಪೂರ್ಣಾಙ್ಕ
ಸಂದೇಶ_outtrans_len_attr(char_u *msgstr, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಪೂರ್ಣಾಙ್ಕ		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_start = msgstr;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		mb_l;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    ಯದಿ (attr == 0)
	attr = HL_ATTR(HLF_ಸಂದೇಶ);

    // if MSG_HIST flag set, add message to history
    ಯದಿ (attr & ಸಂದೇಶ_HIST)
    {
	add_ಸಂದೇಶ_hist(str, len, attr);
	attr &= ~ಸಂದೇಶ_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    ಯದಿ (ಸಂದೇಶ_silent == 0 && len > 0 && ಸಂದೇಶ_row >= cmdline_row && ಸಂದೇಶ_col == 0)
    {
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    ಯದಿ (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	ಸಂದೇಶ_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    ಯಾವತ್ (--len >= 0 && !got_int)
    {
	ಯದಿ (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	ಅನ್ಯಥಾ
	    mb_l = 1;
	ಯದಿ (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    ಯದಿ (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    ಅನ್ಯಥಾ
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		ಯದಿ (str > plain_start)
		    ಸಂದೇಶ_puts_attr_len((ವರ್ಣ *)plain_start,
					       (ಪೂರ್ಣಾಙ್ಕ)(str - plain_start), attr);
		plain_start = str + mb_l;
		ಸಂದೇಶ_puts_attr((ವರ್ಣ *)transchar_buf(ರಿಕ್ತ, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	ಅನ್ಯಥಾ
	{
	    s = transchar_byte_buf(ರಿಕ್ತ, *str);
	    ಯದಿ (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		ಯದಿ (str > plain_start)
		    ಸಂದೇಶ_puts_attr_len((ವರ್ಣ *)plain_start,
					       (ಪೂರ್ಣಾಙ್ಕ)(str - plain_start), attr);
		plain_start = str + 1;
		ಸಂದೇಶ_puts_attr((ವರ್ಣ *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);
	    }
	    ಅನ್ಯಥಾ
		++retval;
	    ++str;
	}
    }

    ಯದಿ (str > plain_start && !got_int)
	// print the printable chars at the end
	ಸಂದೇಶ_puts_attr_len((ವರ್ಣ *)plain_start, (ಪೂರ್ಣಾಙ್ಕ)(str - plain_start), attr);

    got_int |= save_got_int;

    ಪ್ರತ್ಯಾಗಮ retval;
}

#ಯದಿ defined(FEAT_QUICKFIX)
    ಶೂನ್ಯ
ಸಂದೇಶ_make(char_u *arg)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಸ್ಥಿರ char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    ಪ್ರತಿ (i = 5; *arg && i >= 0; --i)
	ಯದಿ (*arg++ != str[i])
	    ಭಙ್ಗ;
    ಯದಿ (i < 0)
    {
	ಸಂದೇಶ_putchar('\n');
	ಪ್ರತಿ (i = 0; rs[i]; ++i)
	    ಸಂದೇಶ_putchar(rs[i] - 3);
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
    ಪೂರ್ಣಾಙ್ಕ
ಸಂದೇಶ_outtrans_special(
    char_u	*strstart,
    ಪೂರ್ಣಾಙ್ಕ		from,	// TRUE for lhs of a mapping
    ಪೂರ್ಣಾಙ್ಕ		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    ಪೂರ್ಣಾಙ್ಕ		retval = 0;
    ವರ್ಣ	*text;
    ಪೂರ್ಣಾಙ್ಕ		attr;
    ಪೂರ್ಣಾಙ್ಕ		len;

    attr = HL_ATTR(HLF_8);
    ಯಾವತ್ (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	ಯದಿ ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    text = "<Space>";
	    ++str;
	}
	ಅನ್ಯಥಾ
	    text = (ವರ್ಣ *)str2special(&str, from, FALSE);
	ಯದಿ (text[0] != NUL && text[1] == NUL)
	    // single-byte character or illegal byte
	    text = (ವರ್ಣ *)transchar_byte_buf(ರಿಕ್ತ, (char_u)text[0]);
	len = vim_strsize((char_u *)text);
	ಯದಿ (maxlen > 0 && retval + len >= maxlen)
	    ಭಙ್ಗ;
	// Highlight special keys
	ಸಂದೇಶ_puts_attr(text, len > 1
		&& (*mb_ptr2len)((char_u *)text) <= 1 ? attr : 0);
	retval += len;
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    ಪೂರ್ಣಾಙ್ಕ	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    ಪೂರ್ಣಾಙ್ಕ	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    ಯಾವತ್ (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    ಪ್ರತ್ಯಾಗಮ (char_u *)ga.ga_data;
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
    ಪೂರ್ಣಾಙ್ಕ		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    ಪೂರ್ಣಾಙ್ಕ		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    ಪೂರ್ಣಾಙ್ಕ			c;
    ಸ್ಥಿರ char_u	buf[7];
    char_u		*str = *sp;
    ಪೂರ್ಣಾಙ್ಕ			modifiers = 0;
    ಪೂರ್ಣಾಙ್ಕ			special = FALSE;

    ಯದಿ (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	ಯದಿ (p != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ p;
    }

    c = *str;
    ಯದಿ ((c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	) && str[1] != NUL && str[2] != NUL)
    {
	ಯದಿ (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	ಯದಿ ((c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = TO_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	ಯದಿ (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    ಯದಿ (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	ಯದಿ (p != ರಿಕ್ತ)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	ಅನ್ಯಥಾ
	    // illegal byte
	    *sp = str + 1;
    }
    ಅನ್ಯಥಾ
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    ಯದಿ (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	ಪ್ರತ್ಯಾಗಮ get_special_ಕೀಲಿ_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Translate a key sequence into special key names.
 */
    ಶೂನ್ಯ
str2specialbuf(char_u *sp, char_u *buf, ಪೂರ್ಣಾಙ್ಕ len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    ಯಾವತ್ (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	ಯದಿ (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }
}

/*
 * print line for :print or :list command
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_prt_ಸಾಲು(char_u *s, ಪೂರ್ಣಾಙ್ಕ list)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		col = 0;
    ಪೂರ್ಣಾಙ್ಕ		n_extra = 0;
    ಪೂರ್ಣಾಙ್ಕ		c_extra = 0;
    ಪೂರ್ಣಾಙ್ಕ		c_final = 0;
    char_u	*p_extra = ರಿಕ್ತ;	    // init to make SASC shut up
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		attr = 0;
    char_u	*trail = ರಿಕ್ತ;
    char_u	*lead = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		in_multispace = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		multispace_pos = 0;
    ಪೂರ್ಣಾಙ್ಕ		l;
    char_u	buf[MB_MAXBYTES + 1];

    ಯದಿ (curwin->w_p_list)
	list = TRUE;

    ಯದಿ (list)
    {
	// find start of trailing whitespace
	ಯದಿ (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    ಯಾವತ್ (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	ಯದಿ (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != ರಿಕ್ತ
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    ಯಾವತ್ (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    ಯದಿ (*lead == NUL)
		lead = ರಿಕ್ತ;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    ಯದಿ (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	ಸಂದೇಶ_putchar(' ');

    ಯಾವತ್ (!got_int)
    {
	ಯದಿ (n_extra > 0)
	{
	    --n_extra;
	    ಯದಿ (n_extra == 0 && c_final)
		c = c_final;
	    ಅನ್ಯಥಾ ಯದಿ (c_extra)
		c = c_extra;
	    ಅನ್ಯಥಾ
		c = *p_extra++;
	}
	ಅನ್ಯಥಾ ಯದಿ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    col += (*mb_ptr2cells)(s);
	    ಯದಿ (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    ಅನ್ಯಥಾ ಯದಿ (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		ಪೂರ್ಣಾಙ್ಕ len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    ಅನ್ಯಥಾ
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    ಸಂದೇಶ_puts((ವರ್ಣ *)buf);
	    s += l;
	    ಅನುವರ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    attr = 0;
	    c = *s++;
	    ಯದಿ (list)
	    {
		in_multispace = c == ' ' && (*s == ' '
						 || (col > 0 && s[-2] == ' '));
		ಯದಿ (!in_multispace)
		    multispace_pos = 0;
	    }
	    ಯದಿ (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ifdef FEAT_VARTABS
		n_extra = tabstop_padding(col, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#ಅನ್ಯಥಾ
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
#endif
		ಯದಿ (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		ಅನ್ಯಥಾ
		{
		    ಪೂರ್ಣಾಙ್ಕ lcs_tab1 = curwin->w_lcs_chars.tab1;
		    ಪೂರ್ಣಾಙ್ಕ lcs_tab2 = curwin->w_lcs_chars.tab2;
		    ಪೂರ್ಣಾಙ್ಕ lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    ಯದಿ (lead != ರಿಕ್ತ && s <= lead
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
	    ಅನ್ಯಥಾ ಯದಿ (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(ರಿಕ್ತ, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c == ' ')
	    {
		ಯದಿ (lead != ರಿಕ್ತ && s <= lead && in_multispace
			&& curwin->w_lcs_chars.leadmultispace != ರಿಕ್ತ)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    ಯದಿ (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		ಅನ್ಯಥಾ ಯದಿ (lead != ರಿಕ್ತ && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		ಅನ್ಯಥಾ ಯದಿ (trail != ರಿಕ್ತ && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		ಅನ್ಯಥಾ ಯದಿ (in_multispace
			&& curwin->w_lcs_chars.multispace != ರಿಕ್ತ)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    ಯದಿ (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		ಅನ್ಯಥಾ ಯದಿ (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	ಯದಿ (c == NUL)
	    ಭಙ್ಗ;

	ಸಂದೇಶ_putchar_attr(c, attr);
	col++;
    }
    ಸಂದೇಶ_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    ಸ್ಥಿರ char_u *
screen_puts_mbyte(char_u *s, ಪೂರ್ಣಾಙ್ಕ l, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಪೂರ್ಣಾಙ್ಕ		cw;

    ಸಂದೇಶ_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    ಯದಿ (cw > 1 && (
#ifdef FEAT_RIGHTLEFT
		cmdmsg_rl ? ಸಂದೇಶ_col <= 1 :
#endif
		ಸಂದೇಶ_col == cmdline_width - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	ಸಂದೇಶ_screen_putchar('>', HL_ATTR(HLF_AT));
	ಪ್ರತ್ಯಾಗಮ s;
    }

    screen_puts_len(s, l, ಸಂದೇಶ_row, cmdline_col_off + ಸಂದೇಶ_col, attr);
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (cmdmsg_rl)
    {
	ಸಂದೇಶ_col -= cw;
	ಯದಿ (ಸಂದೇಶ_col == 0)
	{
	    ಸಂದೇಶ_col = cmdline_width;
	    ++ಸಂದೇಶ_row;
	}
    }
    ಅನ್ಯಥಾ
#endif
    {
	ಸಂದೇಶ_col += cw;
	ಯದಿ (ಸಂದೇಶ_col >= cmdline_width)
	{
	    ಸಂದೇಶ_col = 0;
	    ++ಸಂದೇಶ_row;
	}
    }
    ಪ್ರತ್ಯಾಗಮ s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_puts(ವರ್ಣ *s)
{
    ಸಂದೇಶ_puts_attr(s, 0);
}

    ಶೂನ್ಯ
ಸಂದೇಶ_puts_title(ವರ್ಣ *s)
{
    ಸಂದೇಶ_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_outtrans_long_len_attr(char_u *longstr, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಪೂರ್ಣಾಙ್ಕ		slen = len;
    ಪೂರ್ಣಾಙ್ಕ		room;

    room = cmdline_width - ಸಂದೇಶ_col;
    ಯದಿ (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	ಸಂದೇಶ_outtrans_len_attr(longstr, slen, attr);
	ಸಂದೇಶ_puts_attr("...", HL_ATTR(HLF_8));
    }
    ಸಂದೇಶ_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    ಶೂನ್ಯ
ಸಂದೇಶ_outtrans_long_attr(char_u *longstr, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಸಂದೇಶ_outtrans_long_len_attr(longstr, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_puts_attr(ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಸಂದೇಶ_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_puts_attr_len(ವರ್ಣ *str, ಪೂರ್ಣಾಙ್ಕ maxlen, ಪೂರ್ಣಾಙ್ಕ attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    ಯದಿ (ಸಂದೇಶ_silent != 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (attr == 0)
	attr = HL_ATTR(HLF_ಸಂದೇಶ);

    // if MSG_HIST flag set, add message to history
    ಯದಿ ((attr & ಸಂದೇಶ_HIST) && maxlen < 0)
    {
	add_ಸಂದೇಶ_hist((char_u *)str, -1, attr);
	attr &= ~ಸಂದೇಶ_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    ಯದಿ (ಸಂದೇಶ_scrolled != 0 && !ಸಂದೇಶ_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    ಸಂದೇಶ_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    ಯದಿ (ಸಂದೇಶ_use_printf())
	ಸಂದೇಶ_puts_printf((char_u *)str, maxlen);
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_puts_display((char_u *)str, maxlen, attr, FALSE);

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
    ಸ್ಥಿರ ಶೂನ್ಯ
put_ಸಂದೇಶ_win(win_T *wp, ಪೂರ್ಣಾಙ್ಕ where, char_u *t_s, char_u *end, linenr_T lnum)
{
    char_u  *p;

    ಯದಿ (where == PUT_BELOW)
    {
	ಯದಿ (*end != NUL)
	{
	    p = vim_strnsave(t_s, end - t_s);
	    ಯದಿ (p == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಅನ್ಯಥಾ
	    p = t_s;
	ml_append_buf(wp->w_buffer, lnum, p, (colnr_T)0, FALSE);
	ಯದಿ (p != t_s)
	    vim_free(p);
    }
    ಅನ್ಯಥಾ
    {
	char_u *newp;

	curbuf = wp->w_buffer;
	ಯದಿ (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    ಯದಿ (newp == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	    ಯದಿ (*end != NUL)
		newp[STRLEN(ml_get(lnum)) + (end - t_s)] = NUL;
	}
	ಅನ್ಯಥಾ
	{
	    newp = vim_strnsave(t_s, end - t_s);
	    ಯದಿ (newp == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_buffer;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    ಸಂದೇಶ_col += (ಪೂರ್ಣಾಙ್ಕ)(end - t_s);
}
#endif

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_puts_display(
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		maxlen,
    ಪೂರ್ಣಾಙ್ಕ		attr,
    ಪೂರ್ಣಾಙ್ಕ		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    ಪೂರ್ಣಾಙ್ಕ		t_col = 0;	// screen cells todo, 0 when "t_s" not used
    ಪೂರ್ಣಾಙ್ಕ		l;
    ಪೂರ್ಣಾಙ್ಕ		cw;
    char_u	*sb_str = str;
    ಪೂರ್ಣಾಙ್ಕ		sb_col = ಸಂದೇಶ_col;
    ಪೂರ್ಣಾಙ್ಕ		wrap;
    ಪೂರ್ಣಾಙ್ಕ		did_last_char;
#ifdef HAS_MESSAGE_WINDOW
    ಪೂರ್ಣಾಙ್ಕ		where = PUT_APPEND;
    win_T	*ಸಂದೇಶ_win = ರಿಕ್ತ;
    linenr_T    lnum = 1;

    ಯದಿ (in_echowindow)
    {
	ಸಂದೇಶ_win = popup_get_message_win();

	ಯದಿ (ಸಂದೇಶ_win != ರಿಕ್ತ)
	{
	    ಯದಿ (!popup_message_win_visible())
	    {
		ಯದಿ (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    ಸಂದೇಶ_win = ರಿಕ್ತ;
		}
		ಅನ್ಯಥಾ
		{
		    // currently hidden, make it empty
		    curbuf = ಸಂದೇಶ_win->w_buffer;
		    ಯಾವತ್ ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
			ml_delete(1);
		    curbuf = curwin->w_buffer;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		lnum = ಸಂದೇಶ_win->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		ಯದಿ (ಸಂದೇಶ_col == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#endif

    did_wait_return = FALSE;
    ಯಾವತ್ ((maxlen < 0 || (ಪೂರ್ಣಾಙ್ಕ)(s - str) < maxlen) && *s != NUL)
    {
#ifdef HAS_MESSAGE_WINDOW
	// For echowindow, use full width; for regular messages, leave last column
	ಪೂರ್ಣಾಙ್ಕ wrap_col = ಸಂದೇಶ_win != ರಿಕ್ತ ? cmdline_width : cmdline_width - 1;
#ಅನ್ಯಥಾ
	ಪೂರ್ಣಾಙ್ಕ wrap_col = cmdline_width - 1;
#endif

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	ಯದಿ (!recurse && ಸಂದೇಶ_row >= Rows - 1 && (*s == '\n' || (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			ಸಂದೇಶ_col <= 1
		      || (*s == TAB && ಸಂದೇಶ_col <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && ಸಂದೇಶ_col <= 2))
		    :
#endif
		      ((*s != '\r' && ಸಂದೇಶ_col + t_col >= wrap_col)
		       || (*s == TAB && ಸಂದೇಶ_col + t_col
			   >= (wrap_col & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && ಸಂದೇಶ_col + t_col >= wrap_col - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    ಯದಿ (t_col > 0)
	    {
		// output postponed text
#ifdef HAS_MESSAGE_WINDOW
		ಯದಿ (ಸಂದೇಶ_win != ರಿಕ್ತ)
		{
		    put_ಸಂದೇಶ_win(ಸಂದೇಶ_win, where, t_s, s, lnum);
		    ಯದಿ (where == PUT_BELOW)
			++lnum;
		    t_col = 0;
		    where = PUT_BELOW;
		}
		ಅನ್ಯಥಾ
#endif
		    t_puts(&t_col, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    ಯದಿ (ಸಂದೇಶ_no_more && ಸಾಲುಗಳು_ಎಡ == 0)
		ಭಙ್ಗ;

#ifdef HAS_MESSAGE_WINDOW
	    ಯದಿ (ಸಂದೇಶ_win == ರಿಕ್ತ)
#endif
		// Scroll the screen up one line.
		ಸಂದೇಶ_scroll_up();

#ifdef HAS_MESSAGE_WINDOW
	    ಯದಿ (ಸಂದೇಶ_win == ರಿಕ್ತ)
	    {
#endif
		ಸಂದೇಶ_row = Rows - 2;
		ಯದಿ (ಸಂದೇಶ_col >= cmdline_width)  // can happen after screen resize
		    ಸಂದೇಶ_col = cmdline_width - 1;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif

	    // Display char in last column before showing more-prompt.
	    ಯದಿ (*s >= ' '
#ifdef FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#endif
	       )
	    {
		ಯದಿ (has_mbyte)
		{
		    ಯದಿ (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (ಪೂರ್ಣಾಙ್ಕ)((str + maxlen) - s));
		    ಅನ್ಯಥಾ
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		ಅನ್ಯಥಾ
		    ಸಂದೇಶ_screen_putchar(*s++, attr);
		did_last_char = TRUE;
	    }
	    ಅನ್ಯಥಾ
		did_last_char = FALSE;

	    ಯದಿ (p_more)
		// store text for scrolling back
		store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

#ifdef HAS_MESSAGE_WINDOW
	    ಯದಿ (ಸಂದೇಶ_win == ರಿಕ್ತ)
	    {
#endif
		inc_ಸಂದೇಶ_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		ಯದಿ (cmdline_row > 0 && !exmode_active)
		    --cmdline_row;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		ಯದಿ (ಸಾಲುಗಳು_ಎಡ > 0)
		    --ಸಾಲುಗಳು_ಎಡ;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif
	    ಯದಿ (p_more && ಸಾಲುಗಳು_ಎಡ == 0 && State != MODE_HITRETURN
					    && !ಸಂದೇಶ_no_more && !exmode_active)
	    {
#ifdef FEAT_CON_DIALOG
		ಯದಿ (do_more_prompt(NUL))
		    s = confirm_ಸಂದೇಶ_tail;
#ಅನ್ಯಥಾ
		(ಶೂನ್ಯ)do_more_prompt(NUL);
#endif
		ಯದಿ (quit_more)
		    ಪ್ರತ್ಯಾಗಮ;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    ಯದಿ (did_last_char)
		ಅನುವರ್ತ;
	}

	wrap = *s == '\n'
		    || ಸಂದೇಶ_col + t_col >= wrap_col
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && ಸಂದೇಶ_col + t_col >= wrap_col - 1);
	ಯದಿ (t_col > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ifdef HAS_MESSAGE_WINDOW
	    ಯದಿ (ಸಂದೇಶ_win != ರಿಕ್ತ)
	    {
		put_ಸಂದೇಶ_win(ಸಂದೇಶ_win, where, t_s, s, lnum);
		ಯದಿ (where == PUT_BELOW)
		    ++lnum;
		t_col = 0;
		where = PUT_BELOW;
		// Reset msg_col after outputting to new line in echowindow
		ಯದಿ (wrap)
		    ಸಂದೇಶ_col = 0;
	    }
	    ಅನ್ಯಥಾ
#endif
		t_puts(&t_col, t_s, s, attr);
	}

	ಯದಿ (wrap && p_more && !recurse)
	    // store text for scrolling back
	    store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	ಯದಿ (*s == '\n')		    // go to next line
	{
#ifdef HAS_MESSAGE_WINDOW
	    ಯದಿ (ಸಂದೇಶ_win != ರಿಕ್ತ)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		ಯದಿ ((ಸಂದೇಶ_win->w_buffer->b_ml.ml_flags & ML_EMPTY) == 0)
		{
		    put_ಸಂದೇಶ_win(ಸಂದೇಶ_win, PUT_BELOW, t_s, t_s, lnum);
		    ++lnum;
		}
		ಸಂದೇಶ_col = 0;  // Reset column for new line
	    }
	    ಅನ್ಯಥಾ
#endif
		ಸಂದೇಶ_didout = FALSE;	    // remember that line is empty
#ifdef FEAT_RIGHTLEFT
	    ಯದಿ (cmdmsg_rl)
		ಸಂದೇಶ_col = cmdline_width - 1;
	    ಅನ್ಯಥಾ
#endif
		ಸಂದೇಶ_col = 0;
	    ಯದಿ (++ಸಂದೇಶ_row >= Rows)  // safety check
		ಸಂದೇಶ_row = Rows - 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (*s == '\r')	    // go to column 0
	{
	    ಸಂದೇಶ_col = 0;
#ifdef HAS_MESSAGE_WINDOW
	    where = PUT_TRUNC;
#endif
	}
	ಅನ್ಯಥಾ ಯದಿ (*s == '\b')	    // go to previous char
	{
	    ಯದಿ (ಸಂದೇಶ_col)
		--ಸಂದೇಶ_col;
	}
	ಅನ್ಯಥಾ ಯದಿ (*s == TAB)	    // translate Tab into spaces
	{
#ifdef HAS_MESSAGE_WINDOW
	    ಯದಿ (ಸಂದೇಶ_win != ರಿಕ್ತ)
		ಸಂದೇಶ_col = (ಸಂದೇಶ_col + 7) % 8;
	    ಅನ್ಯಥಾ
#endif
		ಕುರು
		    ಸಂದೇಶ_screen_putchar(' ', attr);
		ಯಾವತ್ (ಸಂದೇಶ_col & 7);
	}
	ಅನ್ಯಥಾ ಯದಿ (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		ಯದಿ (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (ಪೂರ್ಣಾಙ್ಕ)((str + maxlen) - s));
		ಅನ್ಯಥಾ
		    l = (*mb_ptr2len)(s);
	    }
	    ಅನ್ಯಥಾ
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    ಯದಿ (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#endif
		    (cw > 1 && ಸಂದೇಶ_col + t_col >= wrap_col))
	    {
		ಯದಿ (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		ಅನ್ಯಥಾ
		    ಸಂದೇಶ_screen_putchar(*s, attr);
	    }
	    ಅನ್ಯಥಾ
	    {
		// postpone this character until later
		ಯದಿ (t_col == 0)
		    t_s = s;
		t_col += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    ಯದಿ (t_col > 0)
    {
#ifdef HAS_MESSAGE_WINDOW
	ಯದಿ (ಸಂದೇಶ_win != ರಿಕ್ತ)
	    put_ಸಂದೇಶ_win(ಸಂದೇಶ_win, where, t_s, s, lnum);
	ಅನ್ಯಥಾ
#endif
	    t_puts(&t_col, t_s, s, attr);
    }

#ifdef HAS_MESSAGE_WINDOW
    ಯದಿ (ಸಂದೇಶ_win != ರಿಕ್ತ)
	popup_show_message_win();
#endif
    // Store the text for scroll back.
    ಯದಿ (p_more && !recurse)
	store_sb_text(&sb_str, s, attr, &sb_col, FALSE);

    ಸಂದೇಶ_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    ಪೂರ್ಣಾಙ್ಕ
message_filtered(char_u *ಸಂದೇಶ)
{
    ಪೂರ್ಣಾಙ್ಕ match;

    ಯದಿ (cmdmod.cmod_filter_regmatch.regprog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    match = vim_regexec(&cmdmod.cmod_filter_regmatch, ಸಂದೇಶ, (colnr_T)0);
    ಪ್ರತ್ಯಾಗಮ cmdmod.cmod_filter_force ? match : !match;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_scroll_up(ಶೂನ್ಯ)
{
#ifdef HAS_MESSAGE_WINDOW
    ಯದಿ (in_echowindow)
	ಪ್ರತ್ಯಾಗಮ;
#endif
#ifdef FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    ಯದಿ (gui.in_use)
	gui_undraw_cursor();
#endif
    // scrolling up always works
    mch_disable_flush();
    screen_del_ಸಾಲುಗಳು(0, 0, 1, (ಪೂರ್ಣಾಙ್ಕ)Rows, TRUE, 0, ರಿಕ್ತ);
    mch_enable_flush();

    ಯದಿ (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((ಪೂರ್ಣಾಙ್ಕ)Rows - 1, (ಪೂರ್ಣಾಙ್ಕ)Rows, cmdline_col_off,
		cmdline_col_off + cmdline_width, ' ', ' ', HL_ATTR(HLF_ಸಂದೇಶ));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	ಯದಿ (ScreenAttrs[ಸಾಲು_Offset[Rows - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((ಪೂರ್ಣಾಙ್ಕ)Rows - 2, (ಪೂರ್ಣಾಙ್ಕ)Rows - 1,
				 (ಪೂರ್ಣಾಙ್ಕ)Columns - 1, (ಪೂರ್ಣಾಙ್ಕ)Columns,
				 ' ', ' ', HL_ATTR(HLF_ಸಂದೇಶ));
    }
}

/*
 * Increment "msg_scrolled".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
inc_ಸಂದೇಶ_scrolled(ಶೂನ್ಯ)
{
#ifdef FEAT_EVAL
    ಯದಿ (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = ರಿಕ್ತ;

	// v:scrollstart is empty, set it to the script/function name and line
	//ಸಂಖ್ಯೆ
	ಯದಿ (p.string == ರಿಕ್ತ)
	{
	    p.string = (char_u *)_("Unknown");
	    p.length = STRLEN(p.string);
	}
	ಅನ್ಯಥಾ
	{
	    size_t  tofreesize;

	    tofreesize = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    ಯದಿ (tofree != ರಿಕ್ತ)
	    {
		p.length = vim_snprintf_safelen((ವರ್ಣ *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (long)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	set_vim_var_string(VV_SCROLLSTART, p.string, (ಪೂರ್ಣಾಙ್ಕ)p.length);
	vim_free(tofree);
    }
#endif
    ++ಸಂದೇಶ_scrolled;
    set_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ msgchunk_S msgchunk_T;
ಸಂರಚನಾ msgchunk_S
{
    msgchunk_T	*sb_next;
    msgchunk_T	*sb_prev;
    ವರ್ಣ	sb_eol;		// TRUE when line ends after this text
    ಪೂರ್ಣಾಙ್ಕ		sb_ಸಂದೇಶ_col;	// column in which text starts
    ಪೂರ್ಣಾಙ್ಕ		sb_attr;	// text attributes
    char_u	sb_text[1];	// text to be displayed, actually longer
};

ಸ್ಥಿರ msgchunk_T *last_msgchunk = ರಿಕ್ತ; // last displayed text

ಸ್ಥಿರ msgchunk_T *ಸಂದೇಶ_sb_start(msgchunk_T *mps);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_DONE
} sb_clear_T;

// When to clear text on next msg.
ಸ್ಥಿರ sb_clear_T do_clear_sb_text = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
store_sb_text(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    ಪೂರ್ಣಾಙ್ಕ		attr,
    ಪೂರ್ಣಾಙ್ಕ		*sb_col,
    ಪೂರ್ಣಾಙ್ಕ		finish)		// line ends
{
    msgchunk_T	*mp;

    ಯದಿ (do_clear_sb_text == SB_CLEAR_ALL
	    || do_clear_sb_text == SB_CLEAR_CMDLINE_DONE)
    {
	clear_sb_text(do_clear_sb_text == SB_CLEAR_ALL);
	ಸಂದೇಶ_sb_eol();  // prevent messages from overlapping
	do_clear_sb_text = SB_CLEAR_NONE;
    }

    ಯದಿ (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_text) + (s - *sb_str) + 1);
	ಯದಿ (mp != ರಿಕ್ತ)
	{
	    mp->sb_eol = finish;
	    mp->sb_ಸಂದೇಶ_col = *sb_col;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_text, *sb_str, s - *sb_str);

	    ಯದಿ (last_msgchunk == ರಿಕ್ತ)
	    {
		last_msgchunk = mp;
		mp->sb_prev = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ
	    {
		mp->sb_prev = last_msgchunk;
		last_msgchunk->sb_next = mp;
		last_msgchunk = mp;
	    }
	    mp->sb_next = ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (finish && last_msgchunk != ರಿಕ್ತ)
	last_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_col = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    ಶೂನ್ಯ
may_clear_sb_text(ಶೂನ್ಯ)
{
    do_clear_sb_text = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    ಶೂನ್ಯ
sb_text_start_cmdline(ಶೂನ್ಯ)
{
    ಯದಿ (do_clear_sb_text == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_text_restart_cmdline();
    ಅನ್ಯಥಾ
    {
	ಸಂದೇಶ_sb_eol();
	do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    ಶೂನ್ಯ
sb_text_restart_cmdline(ಶೂನ್ಯ)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;

    ಯದಿ (last_msgchunk == ರಿಕ್ತ || last_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	ಪ್ರತ್ಯಾಗಮ;

    tofree = ಸಂದೇಶ_sb_start(last_msgchunk);
    last_msgchunk = tofree->sb_prev;
    ಯದಿ (last_msgchunk != ರಿಕ್ತ)
	last_msgchunk->sb_next = ರಿಕ್ತ;
    ಯಾವತ್ (tofree != ರಿಕ್ತ)
    {
	msgchunk_T *tofree_next = tofree->sb_next;

	vim_free(tofree);
	tofree = tofree_next;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    ಶೂನ್ಯ
sb_text_end_cmdline(ಶೂನ್ಯ)
{
    do_clear_sb_text = SB_CLEAR_CMDLINE_DONE;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    ಶೂನ್ಯ
clear_sb_text(ಪೂರ್ಣಾಙ್ಕ all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    ಯದಿ (all)
	lastp = &last_msgchunk;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (last_msgchunk == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	lastp = &ಸಂದೇಶ_sb_start(last_msgchunk)->sb_prev;
    }

    ಯಾವತ್ (*lastp != ರಿಕ್ತ)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    ಶೂನ್ಯ
show_sb_text(ಶೂನ್ಯ)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = ಸಂದೇಶ_sb_start(last_msgchunk);
    ಯದಿ (mp == ರಿಕ್ತ || mp->sb_prev == ರಿಕ್ತ)
	vim_beep(BO_MESS);
    ಅನ್ಯಥಾ
    {
	do_more_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    ಸ್ಥಿರ msgchunk_T *
ಸಂದೇಶ_sb_start(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    ಯಾವತ್ (mp != ರಿಕ್ತ && mp->sb_prev != ರಿಕ್ತ && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    ಪ್ರತ್ಯಾಗಮ mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_sb_eol(ಶೂನ್ಯ)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    ಯದಿ (last_msgchunk != ರಿಕ್ತ
	    && !(last_msgchunk->sb_text[0] == '\n'
		&& last_msgchunk->sb_text[1] == NUL))
	last_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    ಸ್ಥಿರ msgchunk_T *
disp_sb_ಸಾಲು(ಪೂರ್ಣಾಙ್ಕ row, msgchunk_T *smp, ಪೂರ್ಣಾಙ್ಕ clear_to_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    ಪ್ರತಿ (;;)
    {
	ಸಂದೇಶ_row = row;
	ಸಂದೇಶ_col = mp->sb_ಸಂದೇಶ_col;
	p = mp->sb_text;
	ಯದಿ (*p == '\n')	    // don't display the line break
	    ++p;
	ಸಂದೇಶ_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	ಯದಿ (clear_to_eol)
	    screen_fill(row, row + 1,
		    cmdline_col_off + ಸಂದೇಶ_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', HL_ATTR(HLF_ಸಂದೇಶ));

	ಯದಿ (mp->sb_eol || mp->sb_next == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	mp = mp->sb_next;
    }
    ಪ್ರತ್ಯಾಗಮ mp->sb_next;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
t_puts(
    ಪೂರ್ಣಾಙ್ಕ		*t_col,
    char_u	*t_s,
    char_u	*s,
    ಪೂರ್ಣಾಙ್ಕ		attr)
{
    // output postponed text
    ಸಂದೇಶ_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (ಪೂರ್ಣಾಙ್ಕ)(s - t_s), ಸಂದೇಶ_row, cmdline_col_off + ಸಂದೇಶ_col,
	    attr);
    ಸಂದೇಶ_col += *t_col;
    *t_col = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    ಯದಿ (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--ಸಂದೇಶ_col;
    ಯದಿ (ಸಂದೇಶ_col >= cmdline_width)
    {
	ಸಂದೇಶ_col = 0;
	++ಸಂದೇಶ_row;
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
    ಪೂರ್ಣಾಙ್ಕ
ಸಂದೇಶ_use_printf(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (!ಸಂದೇಶ_check_screen()
#ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.in_use && !termcap_active)
# ಅನ್ಯಥಾ
	    || !termcap_active
# endif
#endif
	    || (swapping_screen() && !termcap_active)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_puts_printf(char_u *str, ಪೂರ್ಣಾಙ್ಕ maxlen)
{
    char_u	*s = str;
    char_u	*buf = ರಿಕ್ತ;
    char_u	*p = s;

#ifdef MSWIN
    ಯದಿ (!(silent_mode && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#endif
    ಯಾವತ್ ((maxlen < 0 || (ಪೂರ್ಣಾಙ್ಕ)(s - str) < maxlen) && *s != NUL)
    {
	ಯದಿ (!(silent_mode && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    ಯದಿ (*s == NL)
	    {
		ಪೂರ್ಣಾಙ್ಕ n = (ಪೂರ್ಣಾಙ್ಕ)(s - p);

		buf = alloc(n + 3);
		ಯದಿ (buf != ರಿಕ್ತ)
		{
		    memcpy(buf, p, n);
		    ಯದಿ (!info_message)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    ಯದಿ (info_message)   // informative message, not an error
			mch_ಸಂದೇಶ((ವರ್ಣ *)buf);
		    ಅನ್ಯಥಾ
			mch_errmsg((ವರ್ಣ *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (cmdmsg_rl)
	{
	    ಯದಿ (*s == CAR || *s == NL)
		ಸಂದೇಶ_col = cmdline_width - 1;
	    ಅನ್ಯಥಾ
		--ಸಂದೇಶ_col;
	}
	ಅನ್ಯಥಾ
#endif
	{
	    ಯದಿ (*s == CAR || *s == NL)
		ಸಂದೇಶ_col = 0;
	    ಅನ್ಯಥಾ
		++ಸಂದೇಶ_col;
	}
	++s;
    }

    ಯದಿ (*p != NUL && !(silent_mode && p_verbose == 0))
    {
	char_u *tofree = ರಿಕ್ತ;

	ಯದಿ (maxlen > 0 && vim_strlen_maxlen((ವರ್ಣ *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ಯದಿ (info_message)
		mch_ಸಂದೇಶ((ವರ್ಣ *)p);
	    ಅನ್ಯಥಾ
		mch_errmsg((ವರ್ಣ *)p);
	    vim_free(tofree);
	}
    }

    ಸಂದೇಶ_didout = TRUE;	    // assume that line is not empty

#ifdef MSWIN
    ಯದಿ (!(silent_mode && p_verbose == 0))
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
do_more_prompt(ಪೂರ್ಣಾಙ್ಕ typed_char)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	entered = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		used_typed_char = typed_char;
    ಪೂರ್ಣಾಙ್ಕ		oldState = State;
    ಪೂರ್ಣಾಙ್ಕ		c;
#ifdef FEAT_CON_DIALOG
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;
#endif
    ಪೂರ್ಣಾಙ್ಕ		toscroll;
    msgchunk_T	*mp_last = ರಿಕ್ತ;
    msgchunk_T	*mp;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		ಸಂದೇಶ_attr;

    ಸಂದೇಶ_attr = HL_ATTR(HLF_ಸಂದೇಶ);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    ಯದಿ (entered || (State == MODE_HITRETURN && typed_char == 0))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    entered = TRUE;

    ಯದಿ (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_last = ಸಂದೇಶ_sb_start(last_msgchunk);
	ಪ್ರತಿ (i = 0; i < Rows - 2 && mp_last != ರಿಕ್ತ
					     && mp_last->sb_prev != ರಿಕ್ತ; ++i)
	    mp_last = ಸಂದೇಶ_sb_start(mp_last->sb_prev);
    }

    State = MODE_ASKMORE;
    setmouse();
    ಯದಿ (typed_char == NUL)
	ಸಂದೇಶ_moremsg(FALSE);
    ಪ್ರತಿ (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	ಯದಿ (used_typed_char != NUL)
	{
	    c = used_typed_char;	// was typed at hit-enter prompt
	    used_typed_char = NUL;
	}
	ಅನ್ಯಥಾ
	    c = get_keystroke();

#ಯದಿ defined(FEAT_MENU) && defined(FEAT_GUI)
	ಯದಿ (c == K_MENU)
	{
	    ಪೂರ್ಣಾಙ್ಕ idx = get_menu_index(current_menu, MODE_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    ಯದಿ (idx == MENU_INDEX_INVALID)
		ಅನುವರ್ತ;
	    c = *current_menu->strings[idx];
	    ಯದಿ (c != NUL && current_menu->strings[idx][1] != NUL)
		ins_typebuf(current_menu->strings[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#endif

	toscroll = 0;
	ವಿಕಲ್ಪ (c)
	{
	ಪಕ್ಷ BS:		// scroll one line back
	ಪಕ್ಷ K_BS:
	ಪಕ್ಷ 'k':
	ಪಕ್ಷ K_UP:
	    toscroll = -1;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CAR:		// one extra line
	ಪಕ್ಷ NL:
	ಪಕ್ಷ 'j':
	ಪಕ್ಷ K_DOWN:
	    toscroll = 1;
	    ಭಙ್ಗ;

	ಪಕ್ಷ 'u':		// Up half a page
	    toscroll = -(Rows / 2);
	    ಭಙ್ಗ;

	ಪಕ್ಷ 'd':		// Down half a page
	    toscroll = Rows / 2;
	    ಭಙ್ಗ;

	ಪಕ್ಷ 'b':		// one page back
	ಪಕ್ಷ Ctrl_B:
	ಪಕ್ಷ K_PAGEUP:
	    toscroll = -(Rows - 1);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ' ':		// one extra page
	ಪಕ್ಷ 'f':
	ಪಕ್ಷ Ctrl_F:
	ಪಕ್ಷ K_PAGEDOWN:
	ಪಕ್ಷ K_LEFTMOUSE:
	    toscroll = Rows - 1;
	    ಭಙ್ಗ;

	ಪಕ್ಷ 'g':		// all the way back to the start
	    toscroll = -999999;
	    ಭಙ್ಗ;

	ಪಕ್ಷ 'G':		// all the way to the end
	    toscroll = 999999;
	    ಸಾಲುಗಳು_ಎಡ = 999999;
	    ಭಙ್ಗ;

	ಪಕ್ಷ ':':		// start new command line
#ifdef FEAT_CON_DIALOG
	    ಯದಿ (!confirm_ಸಂದೇಶ_used)
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
	ಪಕ್ಷ 'q':		// quit
	ಪಕ್ಷ Ctrl_C:
	ಪಕ್ಷ ESC:
#ifdef FEAT_CON_DIALOG
	    ಯದಿ (confirm_ಸಂದೇಶ_used)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    ಅನ್ಯಥಾ
#endif
	    {
		got_int = TRUE;
		quit_more = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    ಸಾಲುಗಳು_ಎಡ = Rows - 1;
	    ಭಙ್ಗ;

#ifdef FEAT_CLIPBOARD
	ಪಕ್ಷ Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    ಯದಿ (clip_star.state == SELECT_DONE)
		clip_copy_modeless_selection(TRUE);
	    ಅನುವರ್ತ;
#endif
	ಸ್ವಭಾವ:		// no valid response
	    ಸಂದೇಶ_moremsg(TRUE);
	    ಅನುವರ್ತ;
	}

	ಯದಿ (toscroll != 0)
	{
	    ಯದಿ (toscroll < 0)
	    {
		// go to start of last line
		ಯದಿ (mp_last == ರಿಕ್ತ)
		    mp = ಸಂದೇಶ_sb_start(last_msgchunk);
		ಅನ್ಯಥಾ ಯದಿ (mp_last->sb_prev != ರಿಕ್ತ)
		    mp = ಸಂದೇಶ_sb_start(mp_last->sb_prev);
		ಅನ್ಯಥಾ
		    mp = ರಿಕ್ತ;

		// go to start of line at top of the screen
		ಪ್ರತಿ (i = 0; i < Rows - 2 && mp != ರಿಕ್ತ && mp->sb_prev != ರಿಕ್ತ;
									  ++i)
		    mp = ಸಂದೇಶ_sb_start(mp->sb_prev);

		ಯದಿ (mp != ರಿಕ್ತ && mp->sb_prev != ರಿಕ್ತ)
		{
		    // Find line to be displayed at top.
		    ಪ್ರತಿ (i = 0; i > toscroll; --i)
		    {
			ಯದಿ (mp == ರಿಕ್ತ || mp->sb_prev == ರಿಕ್ತ)
			    ಭಙ್ಗ;
			mp = ಸಂದೇಶ_sb_start(mp->sb_prev);
			ಯದಿ (mp_last == ರಿಕ್ತ)
			    mp_last = ಸಂದೇಶ_sb_start(last_msgchunk);
			ಅನ್ಯಥಾ
			    mp_last = ಸಂದೇಶ_sb_start(mp_last->sb_prev);
		    }

		    ಯದಿ (toscroll == -1 && screen_ins_ಸಾಲುಗಳು(0, 0, 1,
						     (ಪೂರ್ಣಾಙ್ಕ)Rows, 0, ರಿಕ್ತ) == OK)
		    {
			// display line at top
			(ಶೂನ್ಯ)disp_sb_ಸಾಲು(0, mp, FALSE);
		    }
		    ಅನ್ಯಥಾ
		    {
			ಪೂರ್ಣಾಙ್ಕ did_clear = screenclear();

			// redisplay all lines
			ಪ್ರತಿ (i = 0; mp != ರಿಕ್ತ && i < Rows - 1; ++i)
			{
			    mp = disp_sb_ಸಾಲು(i, mp, !did_clear);
			    ++ಸಂದೇಶ_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// First display any text that we scrolled back.
		ಯಾವತ್ (toscroll > 0 && mp_last != ರಿಕ್ತ)
		{
		    // scroll up, display line at bottom
		    ಸಂದೇಶ_scroll_up();
		    inc_ಸಂದೇಶ_scrolled();
		    screen_fill((ಪೂರ್ಣಾಙ್ಕ)Rows - 2, (ಪೂರ್ಣಾಙ್ಕ)Rows - 1,
			    cmdline_col_off, cmdline_col_off + cmdline_width,
			    ' ', ' ', ಸಂದೇಶ_attr);
		    mp_last = disp_sb_ಸಾಲು((ಪೂರ್ಣಾಙ್ಕ)Rows - 2, mp_last, FALSE);
		    --toscroll;
		}
	    }

	    ಯದಿ (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((ಪೂರ್ಣಾಙ್ಕ)Rows - 1, (ಪೂರ್ಣಾಙ್ಕ)Rows, cmdline_col_off,
			cmdline_col_off + cmdline_width, ' ', ' ', ಸಂದೇಶ_attr);
		ಸಂದೇಶ_moremsg(FALSE);
		ಅನುವರ್ತ;
	    }

	    // display more text, return to caller
	    ಸಾಲುಗಳು_ಎಡ = toscroll;
	}

	ಭಙ್ಗ;
    }

    // clear the --more-- message
    screen_fill((ಪೂರ್ಣಾಙ್ಕ)Rows - 1, (ಪೂರ್ಣಾಙ್ಕ)Rows, cmdline_col_off,
	    cmdline_col_off + cmdline_width, ' ', ' ', ಸಂದೇಶ_attr);
    State = oldState;
    setmouse();
    ಯದಿ (quit_more)
    {
	ಸಂದೇಶ_row = Rows - 1;
	ಸಂದೇಶ_col = 0;
    }
#ifdef FEAT_RIGHTLEFT
    ಅನ್ಯಥಾ ಯದಿ (cmdmsg_rl)
	ಸಂದೇಶ_col = cmdline_width - 1;
#endif

    entered = FALSE;
#ifdef FEAT_CON_DIALOG
    ಪ್ರತ್ಯಾಗಮ retval;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

#ಯದಿ defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_ಸಂದೇಶ
#  undef mch_ಸಂದೇಶ
# endif

# ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_errmsg_c(ವರ್ಣ *str)
{
    DWORD   nwrite = 0;
    DWORD   mode = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    ಯದಿ (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (ಪೂರ್ಣಾಙ್ಕ)GetConsoleCP() != enc_codepage)
    {
	ಪೂರ್ಣಾಙ್ಕ	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	ಯದಿ (w != ರಿಕ್ತ)
	{
	    WriteConsoleW(h, w, len, &nwrite, ರಿಕ್ತ);
	    vim_free(w);
	}
    }
    ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
mch_errmsg(ವರ್ಣ *str)
{
# ಯದಿ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    ಪೂರ್ಣಾಙ್ಕ		len;
# endif

# ಯದಿ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    ಯದಿ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   ಅನ್ಯಥಾ
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
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif

# ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ಯದಿ (!(gui.in_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif

# ಯದಿ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str) + 1;
    ಯದಿ (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    ಯದಿ (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_data + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_data + error_ga.ga_len;
	    ಪ್ರತಿ (;;)
	    {
		p = vim_strchr(p, '\r');
		ಯದಿ (p == ರಿಕ್ತ)
		    ಭಙ್ಗ;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_ಸಂದೇಶ_c(ವರ್ಣ *str)
{
    DWORD   nwrite = 0;
    DWORD   mode;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    ಯದಿ (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (ಪೂರ್ಣಾಙ್ಕ)GetConsoleCP() != enc_codepage)
    {
	ಪೂರ್ಣಾಙ್ಕ	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	ಯದಿ (w != ರಿಕ್ತ)
	{
	    WriteConsoleW(h, w, len, &nwrite, ರಿಕ್ತ);
	    vim_free(w);
	}
    }
    ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
mch_ಸಂದೇಶ(ವರ್ಣ *str)
{
# ಯದಿ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    ಯದಿ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   ಅನ್ಯಥಾ
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
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif

# ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ಯದಿ (!(gui.in_use || gui.starting))
#  endif
    {
	mch_ಸಂದೇಶ_c(str);
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif
# ಯದಿ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#endif // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_screen_putchar(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಸಂದೇಶ_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, ಸಂದೇಶ_row, cmdline_col_off + ಸಂದೇಶ_col, attr);
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (cmdmsg_rl)
    {
	ಯದಿ (--ಸಂದೇಶ_col == 0)
	{
	    ಸಂದೇಶ_col = cmdline_width;
	    ++ಸಂದೇಶ_row;
	}
    }
    ಅನ್ಯಥಾ
#endif
    {
	ಯದಿ (++ಸಂದೇಶ_col >= cmdline_width)
	{
	    ಸಂದೇಶ_col = 0;
	    ++ಸಂದೇಶ_row;
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_moremsg(ಪೂರ್ಣಾಙ್ಕ full)
{
    ಪೂರ್ಣಾಙ್ಕ		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (ಪೂರ್ಣಾಙ್ಕ)Rows - 1, cmdline_col_off, attr);
    ಯದಿ (full)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(ಪೂರ್ಣಾಙ್ಕ)Rows - 1, cmdline_col_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    ಶೂನ್ಯ
repeat_message(ಶೂನ್ಯ)
{
    ಯದಿ (State == MODE_ASKMORE)
    {
	ಸಂದೇಶ_moremsg(TRUE);	// display --more-- message again
	ಸಂದೇಶ_row = Rows - 1;
    }
#ifdef FEAT_CON_DIALOG
    ಅನ್ಯಥಾ ಯದಿ (State == MODE_CONFIRM)
    {
	display_confirm_ಸಂದೇಶ();	// display ":confirm" message again
	ಸಂದೇಶ_row = Rows - 1;
    }
#endif
    ಅನ್ಯಥಾ ಯದಿ (State == MODE_EXTERNCMD)
    {
	windgoto(ಸಂದೇಶ_row, cmdline_col_off + ಸಂದೇಶ_col); // put cursor back
    }
    ಅನ್ಯಥಾ ಯದಿ (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	ಯದಿ (ಸಂದೇಶ_row == Rows - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    ಸಂದೇಶ_didout = FALSE;
	    ಸಂದೇಶ_col = 0;
	    ಸಂದೇಶ_clr_eos();
	}
	hit_return_ಸಂದೇಶ();
	ಸಂದೇಶ_row = Rows - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ಸಂದೇಶ_check_screen(ಶೂನ್ಯ)
{
    ಯದಿ (!full_screen || !screen_valid(FALSE))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (ಸಂದೇಶ_row >= Rows)
	ಸಂದೇಶ_row = Rows - 1;
    ಯದಿ (ಸಂದೇಶ_col >= cmdline_width)
	ಸಂದೇಶ_col = cmdline_width - 1;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_clr_eos(ಶೂನ್ಯ)
{
    ಯದಿ (ಸಂದೇಶ_silent == 0)
	ಸಂದೇಶ_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_clr_eos_force(ಶೂನ್ಯ)
{
#ifdef HAS_MESSAGE_WINDOW
    ಯದಿ (in_echowindow)
	ಪ್ರತ್ಯಾಗಮ;  // messages go into a popup
#endif
    ಯದಿ (ಸಂದೇಶ_use_printf())
    {
	ಯದಿ (full_screen)	// only when termcap codes are valid
	{
	    ಯದಿ (*T_CD)
		out_str(T_CD);	// clear to end of display
	    ಅನ್ಯಥಾ ಯದಿ (*T_CE)
		out_str(T_CE);	// clear to end of line
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ_attr;

	ಸಂದೇಶ_attr = HL_ATTR(HLF_ಸಂದೇಶ);

#ifdef FEAT_RIGHTLEFT
	ಯದಿ (cmdmsg_rl)
	{
	    screen_fill(ಸಂದೇಶ_row, ಸಂದೇಶ_row + 1,
		    cmdline_col_off, cmdline_col_off + ಸಂದೇಶ_col + 1,
		    ' ', ' ', ಸಂದೇಶ_attr);
	    screen_fill(ಸಂದೇಶ_row + 1, (ಪೂರ್ಣಾಙ್ಕ)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', ಸಂದೇಶ_attr);
	}
	ಅನ್ಯಥಾ
#endif
	{
	    screen_fill(ಸಂದೇಶ_row, ಸಂದೇಶ_row + 1,
		    cmdline_col_off + ಸಂದೇಶ_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', ಸಂದೇಶ_attr);
	    screen_fill(ಸಂದೇಶ_row + 1, (ಪೂರ್ಣಾಙ್ಕ)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', ಸಂದೇಶ_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_clr_cmdline(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_row = cmdline_row;
    ಸಂದೇಶ_col = 0;
    ಸಂದೇಶ_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    ಪೂರ್ಣಾಙ್ಕ
ಸಂದೇಶ_end(ಶೂನ್ಯ)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    ಯದಿ (!exiting && need_wait_return && !(State & MODE_CMDLINE))
    {
	wait_return(FALSE);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    out_flush();
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_check(ಶೂನ್ಯ)
{
    ಯದಿ (ಸಂದೇಶ_row == Rows - 1 && ಸಂದೇಶ_col >= sc_col
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
    ಸ್ಥಿರ ಶೂನ್ಯ
redir_write(char_u *str, ಪೂರ್ಣಾಙ್ಕ maxlen)
{
    char_u	*s = str;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	cur_col = 0;

    // Don't do anything for displaying prompts and the like.
    ಯದಿ (redir_off)
	ಪ್ರತ್ಯಾಗಮ;

    // If 'verbosefile' is set prepare for writing in that file.
    ಯದಿ (*p_vfile != NUL && verbose_fd == ರಿಕ್ತ)
	verbose_open();

    ಯದಿ (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	ಯದಿ (*s != '\n' && *s != '\r')
	{
	    ಯಾವತ್ (cur_col < ಸಂದೇಶ_col)
	    {
#ifdef FEAT_EVAL
		ಯದಿ (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		ಅನ್ಯಥಾ ಯದಿ (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		ಅನ್ಯಥಾ ಯದಿ (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		ಅನ್ಯಥಾ
#endif
		    ಯದಿ (redir_fd != ರಿಕ್ತ)
		    fputs(" ", redir_fd);
		ಯದಿ (verbose_fd != ರಿಕ್ತ)
		    fputs(" ", verbose_fd);
		++cur_col;
	    }
	}

#ifdef FEAT_EVAL
	ಯದಿ (redir_execute)
	    execute_redir_str(s, maxlen);
	ಅನ್ಯಥಾ ಯದಿ (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	ಅನ್ಯಥಾ ಯದಿ (redir_vname)
	    var_redir_str(s, maxlen);
#endif

	// Write and adjust the current column.
	ಯಾವತ್ (*s != NUL && (maxlen < 0 || (ಪೂರ್ಣಾಙ್ಕ)(s - str) < maxlen))
	{
#ifdef FEAT_EVAL
	    ಯದಿ (!redir_reg && !redir_vname && !redir_execute)
#endif
		ಯದಿ (redir_fd != ರಿಕ್ತ)
		    putc(*s, redir_fd);
	    ಯದಿ (verbose_fd != ರಿಕ್ತ)
		putc(*s, verbose_fd);
	    ಯದಿ (*s == '\r' || *s == '\n')
		cur_col = 0;
	    ಅನ್ಯಥಾ ಯದಿ (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    ಅನ್ಯಥಾ
		++cur_col;
	    ++s;
	}

	ಯದಿ (ಸಂದೇಶ_silent != 0)	// should update msg_col
	    ಸಂದೇಶ_col = cur_col;
    }
}

    ಪೂರ್ಣಾಙ್ಕ
redirecting(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ redir_fd != ರಿಕ್ತ || *p_vfile != NUL
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#endif
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    ಶೂನ್ಯ
verbose_enter(ಶೂನ್ಯ)
{
    ಯದಿ (*p_vfile != NUL)
	++ಸಂದೇಶ_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    ಶೂನ್ಯ
verbose_leave(ಶೂನ್ಯ)
{
    ಯದಿ (*p_vfile != NUL)
	ಯದಿ (--ಸಂದೇಶ_silent < 0)
	    ಸಂದೇಶ_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    ಶೂನ್ಯ
verbose_enter_scroll(ಶೂನ್ಯ)
{
    ಯದಿ (*p_vfile != NUL)
	++ಸಂದೇಶ_silent;
    ಅನ್ಯಥಾ
	// always scroll up, don't overwrite
	ಸಂದೇಶ_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    ಶೂನ್ಯ
verbose_leave_scroll(ಶೂನ್ಯ)
{
    ಯದಿ (*p_vfile != NUL)
    {
	ಯದಿ (--ಸಂದೇಶ_silent < 0)
	    ಸಂದೇಶ_silent = 0;
    }
    ಅನ್ಯಥಾ
	cmdline_row = ಸಂದೇಶ_row;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    ಶೂನ್ಯ
verbose_stop(ಶೂನ್ಯ)
{
    ಯದಿ (verbose_fd != ರಿಕ್ತ)
    {
	fclose(verbose_fd);
	verbose_fd = ರಿಕ್ತ;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    ಪೂರ್ಣಾಙ್ಕ
verbose_open(ಶೂನ್ಯ)
{
    ಯದಿ (verbose_fd == ರಿಕ್ತ && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((ವರ್ಣ *)p_vfile, "a");
	ಯದಿ (verbose_fd == ರಿಕ್ತ)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    ಶೂನ್ಯ
give_warning(char_u *message, ಪೂರ್ಣಾಙ್ಕ hl)
{
    give_warning_with_source(message, hl, FALSE);
}

    ಶೂನ್ಯ
give_warning_with_source(char_u *message, ಪೂರ್ಣಾಙ್ಕ hl, ಪೂರ್ಣಾಙ್ಕ with_source)
{
    // Don't do this for ":silent".
    ಯದಿ (ಸಂದೇಶ_silent != 0)
	ಪ್ರತ್ಯಾಗಮ;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, message, -1);
#endif
    VIM_CLEAR(keep_ಸಂದೇಶ);
    ಯದಿ (hl)
	keep_ಸಂದೇಶ_attr = HL_ATTR(HLF_W);
    ಅನ್ಯಥಾ
	keep_ಸಂದೇಶ_attr = 0;

    ಯದಿ (with_source)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	ಸಂದೇಶ_start();
	ಸಂದೇಶ_source(HL_ATTR(HLF_W));
	ಸಂದೇಶ_puts(" ");
	ಸಂದೇಶ_puts_attr((ವರ್ಣ *)message, HL_ATTR(HLF_W) | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_clr_eos();
	(ಶೂನ್ಯ)ಸಂದೇಶ_end();
    }
    ಅನ್ಯಥಾ ಯದಿ (ಸಂದೇಶ_attr((ವರ್ಣ *)message, keep_ಸಂದೇಶ_attr) && ಸಂದೇಶ_scrolled == 0)
	set_keep_ಸಂದೇಶ(message, keep_ಸಂದೇಶ_attr);

    ಸಂದೇಶ_didout = FALSE;	    // overwrite this message
    ಸಂದೇಶ_nowait = TRUE;	    // don't wait for this message
    ಸಂದೇಶ_col = 0;

    --no_wait_return;
}

#ಯದಿ defined(FEAT_EVAL)
    ಶೂನ್ಯ
give_warning2(char_u *message, char_u *a1, ಪೂರ್ಣಾಙ್ಕ hl)
{
    ಯದಿ (IObuff == ರಿಕ್ತ)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(message, hl);
    }
    ಅನ್ಯಥಾ
    {
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, (ವರ್ಣ *)message, a1);
	give_warning(IObuff, hl);
    }
}
#endif

/*
 * Advance msg cursor to column "col".
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_advance(ಪೂರ್ಣಾಙ್ಕ col)
{
    ಯದಿ (ಸಂದೇಶ_silent != 0)	// nothing to advance to
    {
	ಸಂದೇಶ_col = col;		// for redirection, may fill it up later
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (col >= cmdline_width)	// not enough room
	col = cmdline_width - 1;
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (cmdmsg_rl)
	ಯಾವತ್ (ಸಂದೇಶ_col > cmdline_width - col)
	    ಸಂದೇಶ_putchar(' ');
    ಅನ್ಯಥಾ
#endif
	ಯಾವತ್ (ಸಂದೇಶ_col < col)
	    ಸಂದೇಶ_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_warn_missing_clipboard(ಶೂನ್ಯ)
{
    ಯದಿ (!global_busy && !did_warn_clipboard && silence_w23_w24_ಸಂದೇಶ == 0)
    {
#ifdef FEAT_CLIPBOARD
	ಸಂದೇಶ(_("W23: Clipboard register not available, using register 0"));
#ಅನ್ಯಥಾ
	ಸಂದೇಶ(_("W24: Clipboard register not available. See :h W24"));
#endif
	did_warn_clipboard = TRUE;
    }
}

#ಯದಿ defined(FEAT_CON_DIALOG)
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
    ಪೂರ್ಣಾಙ್ಕ
do_dialog(
    ಪೂರ್ಣಾಙ್ಕ		type UNUSED,
    char_u	*title UNUSED,
    char_u	*message,
    char_u	*buttons,
    ಪೂರ್ಣಾಙ್ಕ		dfltbutton,
    char_u	*textfield UNUSED,	// IObuff for inputdialog(), NULL
					// otherwise
    ಪೂರ್ಣಾಙ್ಕ		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    ಪೂರ್ಣಾಙ್ಕ		oldState;
    ಪೂರ್ಣಾಙ್ಕ		retval = 0;
    char_u	*hotkeys;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    ಯದಿ (silent_mode)
	ಪ್ರತ್ಯಾಗಮ dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    ಯದಿ (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == ರಿಕ್ತ)
    {
	// --gui-dialog-file: write text to a file
	ಯದಿ (gui_dialog_log(title, message))
	    c = dfltbutton;
	ಅನ್ಯಥಾ
	    c = gui_mch_dialog(type, title, message, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_row = ಸಂದೇಶ_row;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	out_flush();
	gui_mch_update();

	ಪ್ರತ್ಯಾಗಮ c;
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
    hotkeys = ಸಂದೇಶ_show_console_dialog(message, buttons, dfltbutton);

    ಯದಿ (hotkeys != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    ವಿಕಲ್ಪ (c)
	    {
	    ಪಕ್ಷ CAR:		// User accepts default option
	    ಪಕ್ಷ NL:
		retval = dfltbutton;
		ಭಙ್ಗ;
	    ಪಕ್ಷ Ctrl_C:	// User aborts/cancels
	    ಪಕ್ಷ ESC:
		retval = 0;
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:		// Could be a hotkey?
		ಯದಿ (c < 0)	// special keys are ignored here
		    ಅನುವರ್ತ;
		ಯದಿ (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    ಭಙ್ಗ;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		ಪ್ರತಿ (i = 0; hotkeys[i]; ++i)
		{
		    ಯದಿ (has_mbyte)
		    {
			ಯದಿ ((*mb_ptr2char)(hotkeys + i) == c)
			    ಭಙ್ಗ;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    ಅನ್ಯಥಾ
			ಯದಿ (hotkeys[i] == c)
			    ಭಙ್ಗ;
		    ++retval;
		}
		ಯದಿ (hotkeys[i])
		    ಭಙ್ಗ;
		// No hotkey match, so keep waiting
		ಅನುವರ್ತ;
	    }
	    ಭಙ್ಗ;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    ಸಂದೇಶ_end_prompt();

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
copy_char(
    char_u	*from,
    char_u	*to,
    ಪೂರ್ಣಾಙ್ಕ		lowercase)	// make character lower case
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (has_mbyte)
    {
	ಯದಿ (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    ಪ್ರತ್ಯಾಗಮ (*mb_char2bytes)(c, to);
	}
	ಅನ್ಯಥಾ
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(to, from, (size_t)len);
	    ಪ್ರತ್ಯಾಗಮ len;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (lowercase)
	    *to = (char_u)TOLOWER_LOC(*from);
	ಅನ್ಯಥಾ
	    *to = *from;
	ಪ್ರತ್ಯಾಗಮ 1;
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
    ಸ್ಥಿರ char_u *
ಸಂದೇಶ_show_console_dialog(
    char_u	*message,
    char_u	*buttons,
    ಪೂರ್ಣಾಙ್ಕ		dfltbutton)
{
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    ಪೂರ್ಣಾಙ್ಕ		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = ರಿಕ್ತ;
    char_u	*msgp = ರಿಕ್ತ;
    char_u	*hotkp = ರಿಕ್ತ;
    char_u	*r;
    ಪೂರ್ಣಾಙ್ಕ		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    ಪೂರ್ಣಾಙ್ಕ		first_hotkey = FALSE;	// first char of button is hotkey
    ಪೂರ್ಣಾಙ್ಕ		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    ಪ್ರತಿ (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	ಯಾವತ್ (*r)
	{
	    ಯದಿ (*r == DLG_BUTTON_SEP)
	    {
		ಯದಿ (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    ಯದಿ (has_mbyte)
			hotkp += STRLEN(hotkp);
		    ಅನ್ಯಥಾ
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    ಯದಿ (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    ಯದಿ (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			first_hotkey = TRUE;
		}
		ಅನ್ಯಥಾ
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    ಯದಿ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*r == DLG_HOTKEY_CHAR || first_hotkey)
	    {
		ಯದಿ (*r == DLG_HOTKEY_CHAR)
		    ++r;
		first_hotkey = FALSE;
		ಯದಿ (copy)
		{
		    ಯದಿ (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    ಅನ್ಯಥಾ
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    ++len;	    // '&a' -> '[a]'
		    ಯದಿ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// everything else copy literally
		ಯದಿ (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	ಯದಿ (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	ಅನ್ಯಥಾ
	{
	    len += (ಪೂರ್ಣಾಙ್ಕ)(STRLEN(message)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    ಯದಿ (!has_hotkey[0])
	    {
		first_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_ಸಂದೇಶ);
	    confirm_ಸಂದೇಶ = alloc(len);
	    ಯದಿ (confirm_ಸಂದೇಶ == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    *confirm_ಸಂದೇಶ = NUL;
	    hotk = alloc(lenhotkey);
	    ಯದಿ (hotk == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	    *confirm_ಸಂದೇಶ = '\n';
	    STRCPY(confirm_ಸಂದೇಶ + 1, message);

	    msgp = confirm_ಸಂದೇಶ + 1 + STRLEN(message);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_ಸಂದೇಶ_tail = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_ಸಂದೇಶ();
    ಪ್ರತ್ಯಾಗಮ hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
display_confirm_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_ಸಂದೇಶ_used;
    ಯದಿ (confirm_ಸಂದೇಶ != ರಿಕ್ತ)
	ಸಂದೇಶ_puts_attr((ವರ್ಣ *)confirm_ಸಂದೇಶ, HL_ATTR(HLF_M));
    --confirm_ಸಂದೇಶ_used;
}

#endif // FEAT_CON_DIALOG

#ಯದಿ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    ಪೂರ್ಣಾಙ್ಕ
vim_dialog_yesno(
    ಪೂರ್ಣಾಙ್ಕ		type,
    char_u	*title,
    char_u	*message,
    ಪೂರ್ಣಾಙ್ಕ		dflt)
{
    ಯದಿ (do_dialog(type,
		title == ರಿಕ್ತ ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No"), dflt, ರಿಕ್ತ, FALSE) == 1)
	ಪ್ರತ್ಯಾಗಮ VIM_YES;
    ಪ್ರತ್ಯಾಗಮ VIM_NO;
}

    ಪೂರ್ಣಾಙ್ಕ
vim_dialog_yesnocancel(
    ಪೂರ್ಣಾಙ್ಕ		type,
    char_u	*title,
    char_u	*message,
    ಪೂರ್ಣಾಙ್ಕ		dflt)
{
    ವಿಕಲ್ಪ (do_dialog(type,
		title == ರಿಕ್ತ ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, ರಿಕ್ತ, FALSE))
    {
	ಪಕ್ಷ 1: ಪ್ರತ್ಯಾಗಮ VIM_YES;
	ಪಕ್ಷ 2: ಪ್ರತ್ಯಾಗಮ VIM_NO;
    }
    ಪ್ರತ್ಯಾಗಮ VIM_CANCEL;
}

    ಪೂರ್ಣಾಙ್ಕ
vim_dialog_yesnoallcancel(
    ಪೂರ್ಣಾಙ್ಕ		type,
    char_u	*title,
    char_u	*message,
    ಪೂರ್ಣಾಙ್ಕ		dflt)
{
    ವಿಕಲ್ಪ (do_dialog(type,
		title == ರಿಕ್ತ ? (char_u *)"Question" : title,
		message,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, ರಿಕ್ತ, FALSE))
    {
	ಪಕ್ಷ 1: ಪ್ರತ್ಯಾಗಮ VIM_YES;
	ಪಕ್ಷ 2: ಪ್ರತ್ಯಾಗಮ VIM_NO;
	ಪಕ್ಷ 3: ಪ್ರತ್ಯಾಗಮ VIM_ALL;
	ಪಕ್ಷ 4: ಪ್ರತ್ಯಾಗಮ VIM_DISCARDALL;
    }
    ಪ್ರತ್ಯಾಗಮ VIM_CANCEL;
}

#endif // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
