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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਨੇਹਾ_FILE		// don't include prototype for smsg()

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ ਖਾਲੀ ਜੋੜੋ_ਸੁਨੇਹਾ_hist(char_u *s, ਪੂਰਨ len, ਪੂਰਨ attr);
ਸਥਿਰ ਖਾਲੀ check_ਸੁਨੇਹਾ_hist(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ਪ੍ਰਾਪਤੀ_return_ਸੁਨੇਹਾ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ਸੁਨੇਹਾ_home_replace_attr(char_u *fname, ਪੂਰਨ attr);
ਸਥਿਰ ਖਾਲੀ ਸੁਨੇਹਾ_puts_attr_len(ਅੱਖਰ *str, ਪੂਰਨ maxlen, ਪੂਰਨ attr);
ਸਥਿਰ ਖਾਲੀ ਸੁਨੇਹਾ_puts_display(char_u *str, ਪੂਰਨ maxlen, ਪੂਰਨ attr, ਪੂਰਨ recurse);
ਸਥਿਰ ਖਾਲੀ ਸੁਨੇਹਾ_scroll_ਉੱਪਰ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ inc_ਸੁਨੇਹਾ_scrolled(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ store_sb_ਪਾਠ(char_u **sb_str, char_u *s, ਪੂਰਨ attr, ਪੂਰਨ *sb_ਖਾਨਾ, ਪੂਰਨ ਅੰਤ);
ਸਥਿਰ ਖਾਲੀ t_puts(ਪੂਰਨ *t_ਖਾਨਾ, char_u *t_s, char_u *s, ਪੂਰਨ attr);
ਸਥਿਰ ਖਾਲੀ ਸੁਨੇਹਾ_puts_printf(char_u *str, ਪੂਰਨ maxlen);
ਸਥਿਰ ਪੂਰਨ do_ਹੋਰ_prompt(ਪੂਰਨ typed_char);
ਸਥਿਰ ਖਾਲੀ ਸੁਨੇਹਾ_screen_putchar(ਪੂਰਨ c, ਪੂਰਨ attr);
ਸਥਿਰ ਖਾਲੀ ਸੁਨੇਹਾ_moremsg(ਪੂਰਨ ਭਰਿਆ);
ਸਥਿਰ ਪੂਰਨ  ਸੁਨੇਹਾ_check_screen(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ redir_write(char_u *s, ਪੂਰਨ maxlen);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CON_DIALOG
ਸਥਿਰ char_u *ਸੁਨੇਹਾ_show_console_dialog(char_u *ਸੁਨੇਹਾ_2, char_u *buttons, ਪੂਰਨ dfltbutton);
ਸਥਿਰ ਪੂਰਨ	confirm_ਸੁਨੇਹਾ_ਵਰਤਿਆ = FALSE;	// displaying confirm_msg
ਸਥਿਰ char_u	*confirm_ਸੁਨੇਹਾ = ਨਿਰਾ;		// ":confirm" message
ਸਥਿਰ char_u	*confirm_ਸੁਨੇਹਾ_ਪੂਛ;		// tail of confirm_msg
ਸਥਿਰ ਖਾਲੀ display_confirm_ਸੁਨੇਹਾ(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਪੂਰਨ emsg_ਮੰਜ਼ਿਲ_channel_log = FALSE;
#ਜੇ_ਅੰਤ

ਬਣਤਰ ਸੁਨੇਹਾ_hist
{
    ਬਣਤਰ ਸੁਨੇਹਾ_hist	*ਅਗਲਾ;
    char_u		*ਸੁਨੇਹਾ;
    ਪੂਰਨ			attr;
};

ਸਥਿਰ ਬਣਤਰ ਸੁਨੇਹਾ_hist *ਪਹਿਲਾ_ਸੁਨੇਹਾ_hist = ਨਿਰਾ;
ਸਥਿਰ ਬਣਤਰ ਸੁਨੇਹਾ_hist *ਆਖਰੀ_ਸੁਨੇਹਾ_hist = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ ਸੁਨੇਹਾ_hist_len = 0;
ਸਥਿਰ ਪੂਰਨ ਸੁਨੇਹਾ_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MESSAGES_ਪ੍ਰਾਪਤੀ_ENTER	0x001
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MESSAGES_WAIT		0x002
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MESSAGES_OPT_ਪ੍ਰਾਪਤੀ_ENTER "hit-enter"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MESSAGES_OPT_WAIT "wait:"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
ਸਥਿਰ ਪੂਰਨ ਸੁਨੇਹਾ_flags = MESSAGES_ਪ੍ਰਾਪਤੀ_ENTER | MESSAGES_HISTORY;
ਸਥਿਰ ਪੂਰਨ ਸੁਨੇਹਾ_wait = 0;

ਸਥਿਰ FILE *verbose_fd = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ  verbose_did_open = FALSE;

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
    ਪੂਰਨ
ਸੁਨੇਹਾ(ਅੱਖਰ *s)
{
    ਵਾਪਸ ਸੁਨੇਹਾ_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    ਪੂਰਨ
verb_ਸੁਨੇਹਾ(ਅੱਖਰ *s)
{
    ਪੂਰਨ		n;

    verbose_enter();
    n = ਸੁਨੇਹਾ_attr_keep(s, 0, FALSE);
    verbose_leave();

    ਵਾਪਸ n;
}

    ਪੂਰਨ
ਸੁਨੇਹਾ_attr(ਅੱਖਰ *s, ਪੂਰਨ attr)
{
    ਵਾਪਸ ਸੁਨੇਹਾ_attr_keep(s, attr, FALSE);
}

    ਪੂਰਨ
ਸੁਨੇਹਾ_attr_keep(
    ਅੱਖਰ	*s,
    ਪੂਰਨ		attr,
    ਪੂਰਨ		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    ਸਥਿਰ ਪੂਰਨ	entered = 0;
    ਪੂਰਨ		retval;
    char_u	*buf = ਨਿਰਾ;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    ਜੇ (!emsg_on_display && ਸੁਨੇਹਾ_filtered((char_u *)s))
	ਵਾਪਸ TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (attr == 0)
	ਸੈੱਟ_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#ਜੇ_ਅੰਤ

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    ਜੇ (entered >= 3)
	ਵਾਪਸ TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    ਜੇ ((char_u *)s != keep_ਸੁਨੇਹਾ
	    || (*s != '<'
		&& ਆਖਰੀ_ਸੁਨੇਹਾ_hist != ਨਿਰਾ
		&& ਆਖਰੀ_ਸੁਨੇਹਾ_hist->ਸੁਨੇਹਾ != ਨਿਰਾ
		&& STRCMP(s, ਆਖਰੀ_ਸੁਨੇਹਾ_hist->ਸੁਨੇਹਾ) != 0))
	ਜੋੜੋ_ਸੁਨੇਹਾ_hist((char_u *)s, -1, attr);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (emsg_ਮੰਜ਼ਿਲ_channel_log)
	// Write message in the channel log.
	ch_log(ਨਿਰਾ, "ERROR: %s", s);
#ਜੇ_ਅੰਤ

    // Truncate the message if needed.
    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    buf = ਸੁਨੇਹਾ_strtrunc((char_u *)s, FALSE);
    ਜੇ (buf != ਨਿਰਾ)
	s = (ਅੱਖਰ *)buf;

    ਸੁਨੇਹਾ_outtrans_attr((char_u *)s, attr);
    ਸੁਨੇਹਾ_clr_eos();
    retval = ਸੁਨੇਹਾ_ਸਿਰਾ();

    ਜੇ (keep && retval && vim_strsize((char_u *)s)
		    < (ਪੂਰਨ)(ਪੰਕਤੀਆਂ - cmdline_ਪੰਕਤੀ - 1) * cmdline_ਚੌੜਾਈ + sc_ਖਾਨਾ)
	ਸੈੱਟ_keep_ਸੁਨੇਹਾ((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    ਵਾਪਸ retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
ਸੁਨੇਹਾ_strtrunc(
    char_u	*s,
    ਪੂਰਨ		force)	    // always truncate
{
    char_u	*buf = ਨਿਰਾ;
    ਪੂਰਨ		len;
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼;

    // May truncate message to avoid a hit-return prompt
    ਜੇ ((!ਸੁਨੇਹਾ_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_ਸਰਗਰਮ && ਸੁਨੇਹਾ_silent == 0) || force)
    {
	len = vim_strsize(s);
	ਜੇ (ਸੁਨੇਹਾ_scrolled != 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
		|| ਵਿੱਚ_echowindow
#ਜੇ_ਅੰਤ
		)
	    // Use all the columns.
	    ਗੁੰਜਾਇਸ਼ = (ਪੂਰਨ)(ਪੰਕਤੀਆਂ - ਸੁਨੇਹਾ_ਪੰਕਤੀ) * cmdline_ਚੌੜਾਈ - 1;
	ਨਹੀਂਤਾਂ
	    // Use up to 'showcmd' column.
	    ਗੁੰਜਾਇਸ਼ = (ਪੂਰਨ)(ਪੰਕਤੀਆਂ - ਸੁਨੇਹਾ_ਪੰਕਤੀ - 1) * cmdline_ਚੌੜਾਈ + sc_ਖਾਨਾ - 1;
	ਜੇ (len > ਗੁੰਜਾਇਸ਼ && ਗੁੰਜਾਇਸ਼ > 0)
	{
	    ਜੇ (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (ਗੁੰਜਾਇਸ਼ + 2) * 18;
	    ਨਹੀਂਤਾਂ ਜੇ (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (ਗੁੰਜਾਇਸ਼ + 2) * 2;
	    ਨਹੀਂਤਾਂ
		len = ਗੁੰਜਾਇਸ਼ + 2;
	    buf = alloc(len);
	    ਜੇ (buf != ਨਿਰਾ)
		trunc_string(s, buf, ਗੁੰਜਾਇਸ਼, len);
	}
    }
    ਵਾਪਸ buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    ਖਾਲੀ
trunc_string(
    char_u	*s,
    char_u	*buf,
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼_ਵਿੱਚ,
    ਪੂਰਨ		buflen)
{
    size_t	ਗੁੰਜਾਇਸ਼ = ਗੁੰਜਾਇਸ਼_ਵਿੱਚ - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    ਪੂਰਨ		e;
    ਪੂਰਨ		i;
    ਪੂਰਨ		n;

    ਜੇ (*s == NUL)
    {
	ਜੇ (buflen > 0)
	    *buf = NUL;
	ਵਾਪਸ;
    }

    ਜੇ (ਗੁੰਜਾਇਸ਼_ਵਿੱਚ < 3)
	ਗੁੰਜਾਇਸ਼ = 0;
    half = ਗੁੰਜਾਇਸ਼ / 2;

    // First part: Start of the string.
    ਲਈ (e = 0; len < half && e < buflen; ++e)
    {
	ਜੇ (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    ਵਾਪਸ;
	}
	n = ptr2cells(s + e);
	ਜੇ (len + n > half)
	    ਰੋਕੋ;
	len += n;
	buf[e] = s[e];
	ਜੇ (has_mbyte)
	    ਲਈ (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		ਜੇ (++e == buflen)
		    ਰੋਕੋ;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    ਜੇ (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	ਜਦੋਂ (len + n > ਗੁੰਜਾਇਸ਼)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (ਪੂਰਨ)STRLEN(s);
	ਲਈ (;;)
	{
	    ਕਰੋ
		half = half - utf_ਸਿਰ_off(s, s + half - 1) - 1;
	    ਜਦੋਂ (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    ਜੇ (len + n > ਗੁੰਜਾਇਸ਼ || half == 0)
		ਰੋਕੋ;
	    len += n;
	    i = (ਪੂਰਨ)half;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (i = (ਪੂਰਨ)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= ਗੁੰਜਾਇਸ਼; --i)
	    len += n;
    }


    ਜੇ (i <= e + 3)
    {
	// text fits without truncating
	ਜੇ (s != buf)
	{
	    len = STRLEN(s);
	    ਜੇ (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    ਜੇ (len < 1)
		buf[e - 1] = NUL;
	    ਨਹੀਂਤਾਂ
		mch_memmove(buf + e, s + e, len);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	ਜੇ (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    ਨਹੀਂਤਾਂ
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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO

ਪੂਰਨ vim_snprintf(ਅੱਖਰ *str, size_t str_m, ਅਚਲ ਅੱਖਰ *fmt, ...);

    ਪੂਰਨ
smsg(ਅਚਲ ਅੱਖਰ *s, ...)
{
    ਜੇ (IObuff == ਨਿਰਾ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ਵਾਪਸ ਸੁਨੇਹਾ((ਅੱਖਰ *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ਅੱਖਰ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ਵਾਪਸ ਸੁਨੇਹਾ((ਅੱਖਰ *)IObuff);
}

    ਪੂਰਨ
smsg_attr(ਪੂਰਨ attr, ਅਚਲ ਅੱਖਰ *s, ...)
{
    ਜੇ (IObuff == ਨਿਰਾ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ਵਾਪਸ ਸੁਨੇਹਾ_attr((ਅੱਖਰ *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ਅੱਖਰ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ਵਾਪਸ ਸੁਨੇਹਾ_attr((ਅੱਖਰ *)IObuff, attr);
}

    ਪੂਰਨ
smsg_attr_keep(ਪੂਰਨ attr, ਅਚਲ ਅੱਖਰ *s, ...)
{
    ਜੇ (IObuff == ਨਿਰਾ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ਵਾਪਸ ਸੁਨੇਹਾ_attr_keep((ਅੱਖਰ *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ਅੱਖਰ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ਵਾਪਸ ਸੁਨੇਹਾ_attr_keep((ਅੱਖਰ *)IObuff, attr, TRUE);
}

#ਜੇ_ਅੰਤ

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
ਸਥਿਰ ਪੂਰਨ	ਆਖਰੀ_sourcing_lnum = 0;
ਸਥਿਰ char_u   *ਆਖਰੀ_sourcing_name = ਨਿਰਾ;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    ਖਾਲੀ
reset_ਆਖਰੀ_sourcing(ਖਾਲੀ)
{
    VIM_CLEAR(ਆਖਰੀ_sourcing_name);
    ਆਖਰੀ_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    ਸਥਿਰ ਪੂਰਨ
other_sourcing_name(ਖਾਲੀ)
{
    ਜੇ (HAVE_SOURCING_INFO && SOURCING_NAME != ਨਿਰਾ)
    {
	ਜੇ (ਆਖਰੀ_sourcing_name != ਨਿਰਾ)
	    ਵਾਪਸ STRCMP(SOURCING_NAME, ਆਖਰੀ_sourcing_name) != 0;
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ਸਥਿਰ char_u *
get_emsg_ਸਰੋਤ(ਖਾਲੀ)
{
    char_u	*Buf, *p;

    ਜੇ (HAVE_SOURCING_INFO && SOURCING_NAME != ਨਿਰਾ && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	ਜੇ (sname == ਨਿਰਾ)
	    sname = SOURCING_NAME;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	ਜੇ (Buf != ਨਿਰਾ)
	    sprintf((ਅੱਖਰ *)Buf, (ਅੱਖਰ *)p, sname);
	vim_free(tofree);
	ਵਾਪਸ Buf;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ਸਥਿਰ char_u *
get_emsg_lnum(ਖਾਲੀ)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    ਜੇ (SOURCING_NAME != ਨਿਰਾ
	    && (other_sourcing_name() || SOURCING_LNUM != ਆਖਰੀ_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	ਜੇ (Buf != ਨਿਰਾ)
	    sprintf((ਅੱਖਰ *)Buf, (ਅੱਖਰ *)p, (ਲੰਮਾ)SOURCING_LNUM);
	ਵਾਪਸ Buf;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_ਸਰੋਤ(ਪੂਰਨ attr)
{
    char_u	*p;
    ਸਥਿਰ ਪੂਰਨ	recursive = FALSE;

    // Bail out if something called here causes an error.
    ਜੇ (recursive)
	ਵਾਪਸ;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_ਸਰੋਤ();
    ਜੇ (p != ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ_scroll = TRUE;  // this will take more than one line
	ਸੁਨੇਹਾ_attr((ਅੱਖਰ *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    ਜੇ (p != ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ_attr((ਅੱਖਰ *)p, HL_ATTR(HLF_N));
	vim_free(p);
	ਆਖਰੀ_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    ਜੇ (SOURCING_NAME == ਨਿਰਾ || other_sourcing_name())
    {
	VIM_CLEAR(ਆਖਰੀ_sourcing_name);
	ਜੇ (SOURCING_NAME != ਨਿਰਾ)
	    ਆਖਰੀ_sourcing_name = vim_strsave(SOURCING_NAME);
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
    ਸਥਿਰ ਪੂਰਨ
emsg_not_now(ਖਾਲੀ)
{
    ਜੇ ((emsg_off > 0 && vim_strchr(p_debug, 'm') == ਨਿਰਾ
					  && vim_strchr(p_debug, 't') == ਨਿਰਾ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    || emsg_skip > 0
#ਜੇ_ਅੰਤ
	    )
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

#ਜੇ defined(FEAT_EVAL)
ਸਥਿਰ garray_T ignore_error_list = GA_ਖਾਲੀ_ਥਾਵਾਂ;

    ਖਾਲੀ
ignore_error_for_testing(char_u *error)
{
    ਜੇ (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, ਆਕਾਰ(char_u *), 1);

    ਜੇ (STRCMP("RESET", error) == 0)
	ga_clear_ਲੜੀਆਂ(&ignore_error_list);
    ਨਹੀਂਤਾਂ
	ga_copy_string(&ignore_error_list, error);
}

    ਸਥਿਰ ਪੂਰਨ
ignore_error(ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < ignore_error_list.ga_len; ++i)
	ਜੇ (ਲੜੀ_ਖੋਜ(ਸੁਨੇਹਾ,
		  (ਅੱਖਰ *)((char_u **)(ignore_error_list.ga_ਅੰਕੜੇ))[i]) != ਨਿਰਾ)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

#ਜੇ !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    ਖਾਲੀ
do_perror(ਅੱਖਰ *ਸੁਨੇਹਾ)
{
    perror(ਸੁਨੇਹਾ);
    ++emsg_silent;
    emsg(ਸੁਨੇਹਾ);
    --emsg_silent;
}
#ਜੇ_ਅੰਤ

/*
 * emsg_core() - display an error message
 *
 * Rings the bell, if appropriate, and calls message() to do the real work
 * When terminal not initialized (yet) mch_errmsg(..) is used.
 *
 * Return TRUE if wait_return() not called.
 * Note: caller must check 'emsg_not_now()' before calling this.
 */
    ਸਥਿਰ ਪੂਰਨ
emsg_core(ਅਚਲ ਅੱਖਰ *s)
{
    ਪੂਰਨ		attr;
    char_u	*p;
    ਪੂਰਨ		r;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		ignore = FALSE;
    ਪੂਰਨ		severe;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // When testing some errors are turned into a normal message.
    ਜੇ (ignore_error(s))
	// don't call msg() if it results in a dialog
	ਵਾਪਸ ਸੁਨੇਹਾ_use_printf() ? FALSE : ਸੁਨੇਹਾ((ਅੱਖਰ *)s);
#ਜੇ_ਅੰਤ

    ++called_emsg;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#ਜੇ_ਅੰਤ

    ਜੇ (!emsg_off || vim_strchr(p_debug, 't') != ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	ਜੇ (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    ਜੇ (!ignore)
		++did_emsg;
	    ਵਾਪਸ TRUE;
	}

	ਜੇ (ਵਿੱਚ_assert_fails && emsg_assert_fails_ਸੁਨੇਹਾ == ਨਿਰਾ)
	{
	    emsg_assert_fails_ਸੁਨੇਹਾ = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == ਨਿਰਾ ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	ਸੈੱਟ_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#ਜੇ_ਅੰਤ

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	ਜੇ (emsg_silent != 0)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ++did_emsg_silent;
#ਜੇ_ਅੰਤ
	    ਜੇ (emsg_noredir == 0)
	    {
		ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
		p = get_emsg_ਸਰੋਤ();
		ਜੇ (p != ਨਿਰਾ)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		ਜੇ (p != ਨਿਰਾ)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		redir_write((char_u *)s, -1);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    // Only increment did_emsg_def when :silent! wasn't used inside the
	    // :def function.
	    ਜੇ (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ch_log(ਨਿਰਾ, "ERROR silent: %s", s);
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	ਸੁਨੇਹਾ_silent = 0;
	cmd_silent = FALSE;

	ਜੇ (global_busy)		// break :global command
	    ++global_busy;

	ਜੇ (p_eb)
	    beep_flush();		// also includes flush_buffers()
	ਨਹੀਂਤਾਂ
	    flush_buffers(FLUSH_ਨਿਊਨਤਮ);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	++uncaught_emsg;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
    ਜੇ (!ਵਿੱਚ_echowindow)
#ਜੇ_ਅੰਤ
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    ਜੇ (ਸੁਨੇਹਾ_scrolled != 0)
	need_wait_return = TRUE;    // needed in case emsg() is called after
				    // wait_return() has reset need_wait_return
				    // and a redraw is expected because
				    // msg_scrolled is non-zero

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    emsg_ਮੰਜ਼ਿਲ_channel_log = TRUE;
#ਜੇ_ਅੰਤ
    /*
     * Display name and line number for the source of the error.
     */
    ਸੁਨੇਹਾ_scroll = TRUE;
    ਸੁਨੇਹਾ_ਸਰੋਤ(attr);

    /*
     * Display the error message itself.
     */
    ਸੁਨੇਹਾ_nowait = FALSE;			// wait for this msg
    r = ਸੁਨੇਹਾ_attr((ਅੱਖਰ *)s, attr);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    emsg_ਮੰਜ਼ਿਲ_channel_log = FALSE;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    ਪੂਰਨ
emsg(ਅੱਖਰ *s)
{
    // Skip this if not giving error messages at the moment.
    ਜੇ (emsg_not_now())
	ਵਾਪਸ TRUE;

    ਵਾਪਸ emsg_core(s);
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    ਪੂਰਨ
semsg(ਅਚਲ ਅੱਖਰ *s, ...)
{
    // Skip this if not giving error messages at the moment.
    ਜੇ (emsg_not_now())
	ਵਾਪਸ TRUE;

    ਜੇ (IObuff == ਨਿਰਾ)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ਵਾਪਸ emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((ਅੱਖਰ *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    ਵਾਪਸ emsg_core((ਅੱਖਰ *)IObuff);
}
#ਜੇ_ਅੰਤ

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    ਖਾਲੀ
iemsg(ਅੱਖਰ *s)
{
    ਜੇ (emsg_not_now())
	ਵਾਪਸ;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_ਅੰਦਰੂਨੀ_error_please_report_a_bug));

    emsg_core(s);
#ਜੇ defined(ABORT_ON_ਅੰਦਰੂਨੀ_ERROR) && defined(FEAT_EVAL)
    ਸੈੱਟ_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    ਸੁਨੇਹਾ_putchar('\n');  // avoid overwriting the error message
    ਬਾਹਰਲਾ_flush();
    abort();
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO  // manual proto with __attribute__
/*
 * Same as semsg(...) but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 * Note: caller must not pass 'IObuff' as 1st argument.
 */
    ਖਾਲੀ
siemsg(ਅਚਲ ਅੱਖਰ *s, ...)
{
    ਜੇ (emsg_not_now())
	ਵਾਪਸ;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_ਅੰਦਰੂਨੀ_error_please_report_a_bug));

    ਜੇ (IObuff == ਨਿਰਾ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    ਨਹੀਂਤਾਂ
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((ਅੱਖਰ *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((ਅੱਖਰ *)IObuff);
    }
# ifdef ABORT_ON_ਅੰਦਰੂਨੀ_ERROR
    ਸੁਨੇਹਾ_putchar('\n');  // avoid overwriting the error message
    ਬਾਹਰਲਾ_flush();
    abort();
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Give an "Internal error" message.
 */
    ਖਾਲੀ
ਅੰਦਰੂਨੀ_error(ਅੱਖਰ *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_ਅੰਦਰੂਨੀ_error_please_report_a_bug));

    siemsg(_(e_ਅੰਦਰੂਨੀ_error_str), where);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    ਖਾਲੀ
ਅੰਦਰੂਨੀ_error_no_abort(ਅੱਖਰ *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_ਅੰਦਰੂਨੀ_error_please_report_a_bug));

    semsg(_(e_ਅੰਦਰੂਨੀ_error_str), where);
}
#ਜੇ_ਅੰਤ

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    ਖਾਲੀ
emsg_invreg(ਪੂਰਨ name)
{
    semsg(_(e_invalid_ਰਜਿਸਟਰ_name_str), transchar_buf(ਨਿਰਾ, name));
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    ਖਾਲੀ
emsg_namelen(ਅੱਖਰ *ਸੁਨੇਹਾ, char_u *name, ਪੂਰਨ len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(ਸੁਨੇਹਾ, copy == ਨਿਰਾ ? "NULL" : (ਅੱਖਰ *)copy);
    vim_free(copy);
}
#ਜੇ_ਅੰਤ

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    ਅੱਖਰ *
ਸੁਨੇਹਾ_trunc_attr(ਅੱਖਰ *s, ਪੂਰਨ force, ਪੂਰਨ attr)
{
    ਪੂਰਨ		n;
    ਅੱਖਰ	*ts;

    // Add message to history before truncating
    ਜੋੜੋ_ਸੁਨੇਹਾ_hist((char_u *)s, -1, attr);

    ts = (ਅੱਖਰ *)ਸੁਨੇਹਾ_may_trunc(force, (char_u *)s);

    ਸੁਨੇਹਾ_hist_off = TRUE;
    n = ਸੁਨੇਹਾ_attr(ts, attr);
    ਸੁਨੇਹਾ_hist_off = FALSE;

    ਜੇ (n)
	ਵਾਪਸ ts;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
ਸੁਨੇਹਾ_may_trunc(ਪੂਰਨ force, char_u *s)
{
    ਪੂਰਨ		n;
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    ਗੁੰਜਾਇਸ਼ = (ਪੂਰਨ)(ਪੰਕਤੀਆਂ - cmdline_ਪੰਕਤੀ - 1) * cmdline_ਚੌੜਾਈ + sc_ਖਾਨਾ - 1;
    ਜੇ (ਗੁੰਜਾਇਸ਼ > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_ਸਰਗਰਮ))
	    && (n = (ਪੂਰਨ)STRLEN(s) - ਗੁੰਜਾਇਸ਼) > 0)
    {
	ਜੇ (has_mbyte)
	{
	    ਪੂਰਨ	ਮਾਪ = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    ਜੇ (ਮਾਪ <= ਗੁੰਜਾਇਸ਼)
		ਵਾਪਸ s;

	    ਲਈ (n = 0; ਮਾਪ >= ਗੁੰਜਾਇਸ਼; )
	    {
		ਮਾਪ -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    ਵਾਪਸ s;
}

    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_ਸੁਨੇਹਾ_hist(
    char_u	*s,
    ਪੂਰਨ		len,		// -1 for undetermined length
    ਪੂਰਨ		attr)
{
    ਬਣਤਰ ਸੁਨੇਹਾ_hist *p;

    ਜੇ (ਸੁਨੇਹਾ_hist_off || ਸੁਨੇਹਾ_silent != 0)
	ਵਾਪਸ;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(ਬਣਤਰ ਸੁਨੇਹਾ_hist);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (len < 0)
	len = (ਪੂਰਨ)STRLEN(s);
    // remove leading and trailing newlines
    ਜਦੋਂ (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    ਜਦੋਂ (len > 0 && s[len - 1] == '\n')
	--len;
    p->ਸੁਨੇਹਾ = vim_strnsave(s, len);
    p->ਅਗਲਾ = ਨਿਰਾ;
    p->attr = attr;
    ਜੇ (ਆਖਰੀ_ਸੁਨੇਹਾ_hist != ਨਿਰਾ)
	ਆਖਰੀ_ਸੁਨੇਹਾ_hist->ਅਗਲਾ = p;
    ਆਖਰੀ_ਸੁਨੇਹਾ_hist = p;
    ਜੇ (ਪਹਿਲਾ_ਸੁਨੇਹਾ_hist == ਨਿਰਾ)
	ਪਹਿਲਾ_ਸੁਨੇਹਾ_hist = ਆਖਰੀ_ਸੁਨੇਹਾ_hist;
    ++ਸੁਨੇਹਾ_hist_len;

    check_ਸੁਨੇਹਾ_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    ਪੂਰਨ
delete_ਪਹਿਲਾ_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
    ਬਣਤਰ ਸੁਨੇਹਾ_hist *p;

    ਜੇ (ਸੁਨੇਹਾ_hist_len <= 0)
	ਵਾਪਸ FAIL;
    p = ਪਹਿਲਾ_ਸੁਨੇਹਾ_hist;
    ਪਹਿਲਾ_ਸੁਨੇਹਾ_hist = p->ਅਗਲਾ;
    ਜੇ (ਪਹਿਲਾ_ਸੁਨੇਹਾ_hist == ਨਿਰਾ)
	ਆਖਰੀ_ਸੁਨੇਹਾ_hist = ਨਿਰਾ;  // history is empty
    vim_free(p->ਸੁਨੇਹਾ);
    vim_free(p);
    --ਸੁਨੇਹਾ_hist_len;
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਖਾਲੀ
check_ਸੁਨੇਹਾ_hist(ਖਾਲੀ)
{
    // Don't let the message history get too big
    ਜਦੋਂ (ਸੁਨੇਹਾ_hist_len > 0 && ਸੁਨੇਹਾ_hist_len > ਸੁਨੇਹਾ_hist_max)
	(ਖਾਲੀ)delete_ਪਹਿਲਾ_ਸੁਨੇਹਾ();
}


    ਪੂਰਨ
messagesopt_ਬਦਲਿਆ(ਖਾਲੀ)
{
    char_u	*p;
    ਪੂਰਨ		messages_flags_new = 0;
    ਪੂਰਨ		messages_wait_new = 0;
    ਪੂਰਨ		messages_history_new = 0;

    p = p_mopt;
    ਜਦੋਂ (*p != NUL)
    {
	ਜੇ (STRNCMP(p, MESSAGES_OPT_ਪ੍ਰਾਪਤੀ_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_ਪ੍ਰਾਪਤੀ_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_ਪ੍ਰਾਪਤੀ_ENTER);
	    messages_flags_new |= MESSAGES_ਪ੍ਰਾਪਤੀ_ENTER;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	ਜੇ (*p != ',' && *p != NUL)
	    ਵਾਪਸ FAIL;
	ਜੇ (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    ਜੇ (!(messages_flags_new & (MESSAGES_ਪ੍ਰਾਪਤੀ_ENTER | MESSAGES_WAIT)))
	ਵਾਪਸ FAIL;

    // "history" must be set
    ਜੇ (!(messages_flags_new & MESSAGES_HISTORY))
	ਵਾਪਸ FAIL;

    ਜੇ (messages_history_new < 0 || messages_history_new > 10000)
	ਵਾਪਸ FAIL;

    ਜੇ (messages_wait_new < 0 || messages_wait_new > 10000)
	ਵਾਪਸ FAIL;

    ਸੁਨੇਹਾ_flags = messages_flags_new;
    ਸੁਨੇਹਾ_wait = messages_wait_new;

    ਸੁਨੇਹਾ_hist_max = messages_history_new;
    check_ਸੁਨੇਹਾ_hist();

    ਵਾਪਸ OK;
}

/*
 * ":messages" command.
 */
    ਖਾਲੀ
ex_messages(exarg_T *eap)
{
    ਬਣਤਰ ਸੁਨੇਹਾ_hist *p;
    char_u	    *s;
    ਪੂਰਨ		    c = 0;

    ਜੇ (STRCMP(eap->arg, "clear") == 0)
    {
	ਪੂਰਨ keep = eap->addr_ਗਿਣਤੀ == 0 ? 0 : eap->line2;

	ਜਦੋਂ (ਸੁਨੇਹਾ_hist_len > keep)
	    (ਖਾਲੀ)delete_ਪਹਿਲਾ_ਸੁਨੇਹਾ();
	ਵਾਪਸ;
    }

    ਜੇ (*eap->arg != NUL)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ;
    }

    ਸੁਨੇਹਾ_hist_off = TRUE;

    p = ਪਹਿਲਾ_ਸੁਨੇਹਾ_hist;
    ਜੇ (eap->addr_ਗਿਣਤੀ != 0)
    {
	// Count total messages
	ਲਈ (; p != ਨਿਰਾ && !got_int; p = p->ਅਗਲਾ)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	ਲਈ (p = ਪਹਿਲਾ_ਸੁਨੇਹਾ_hist; p != ਨਿਰਾ && !got_int && c > 0;
						    p = p->ਅਗਲਾ, c--);
    }

    ਜੇ (p == ਪਹਿਲਾ_ਸੁਨੇਹਾ_hist)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	s = get_mess_lang();
#ਨਹੀਂਤਾਂ
	s = mch_getenv((char_u *)"LANG");
#ਜੇ_ਅੰਤ
	ਜੇ (s != ਨਿਰਾ && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    ਸੁਨੇਹਾ_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    ਲਈ (; p != ਨਿਰਾ && !got_int; p = p->ਅਗਲਾ)
	ਜੇ (p->ਸੁਨੇਹਾ != ਨਿਰਾ)
	    ਸੁਨੇਹਾ_attr((ਅੱਖਰ *)p->ਸੁਨੇਹਾ, p->attr);

    ਸੁਨੇਹਾ_hist_off = FALSE;
}

#ਜੇ defined(FEAT_CON_DIALOG) || defined(ਲੱਭੋ_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_ਸਿਰਾ_prompt(ਖਾਲੀ)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    ਸੁਨੇਹਾ_clr_eos();
    ਸਤਰਾਂ_ਖੱਬਾ = -1;
}
#ਜੇ_ਅੰਤ

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    ਖਾਲੀ
wait_return(ਪੂਰਨ redraw)
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		oldState;
    ਪੂਰਨ		tmpState;
    ਪੂਰਨ		had_got_int;
    ਪੂਰਨ		save_reg_recording;
    FILE	*save_scriptout;

    ਜੇ (redraw == TRUE)
	ਸੈੱਟ_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    ਜੇ (ਸੁਨੇਹਾ_silent != 0)
	ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
    ਜੇ (ਵਿੱਚ_echowindow)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    ਜੇ (vgetc_busy > 0)
	ਵਾਪਸ;
    need_wait_return = TRUE;
    ਜੇ (no_wait_return)
    {
	ਜੇ (!exmode_ਸਰਗਰਮ)
	    cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	ਵਾਪਸ;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    ਜੇ (quit_ਹੋਰ)
    {
	c = CAR;		// just pretend CR was hit
	quit_ਹੋਰ = FALSE;
	got_int = FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (exmode_ਸਰਗਰਮ)
    {
	ਸੁਨੇਹਾ_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!stuff_ਖਾਲੀ_ਥਾਵਾਂ())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    ਨਹੀਂਤਾਂ
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = ਬਹੁਲਕ_HITRETURN;
	setmouse();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#ਜੇ_ਅੰਤ
	cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	ਜੇ (need_check_timestamps)
	    check_timestamps(FALSE);

	ਜੇ (ਸੁਨੇਹਾ_flags & MESSAGES_ਪ੍ਰਾਪਤੀ_ENTER)
	{
	    ਪ੍ਰਾਪਤੀ_return_ਸੁਨੇਹਾ();

	    ਕਰੋ
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
		// Make sure to update image placements (since their row offset
		// will have been changed).
		draw_image_placements();
#ਜੇ_ਅੰਤ
		// Remember "got_int", if it is set vgetc() probably returns a
		// CTRL-C, but we need to loop then.
		had_got_int = got_int;

		// Don't do mappings here, we put the character back in the
		// typeahead buffer.
		++no_mapping;
		++allow_ਕੁੰਜੀਆਂ;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = ਨਿਰਾ;
		c = ਸੁਰੱਖਿਅਤ_vgetc();
		ਜੇ (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_ਕੁੰਜੀਆਂ;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		ਜੇ (ਕੁੰਜੀ_Typed && c == Ctrl_Y && clip_ਤਾਰਾ.state == SELECT_ਪੂਰਾ)
		{
		    clip_copy_modeless_selection(TRUE);
		    c = K_IGNORE;
		}
#ਜੇ_ਅੰਤ

		/*
		* Allow scrolling back in the messages.
		* Also accept scroll-down commands when messages fill the
		* screen, to avoid that typing one 'j' too many makes the
		* messages disappear.
		*/
		ਜੇ (ਕੁੰਜੀ_Typed && p_ਹੋਰ && !p_cp)
		{
		    ਜੇ (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_ਉੱਪਰ || c == K_PAGEUP)
		    {
			ਜੇ (ਸੁਨੇਹਾ_scrolled > ਪੰਕਤੀਆਂ)
			    // scroll back to show older messages
			    do_ਹੋਰ_prompt(c);
			ਨਹੀਂਤਾਂ
			{
			    ਸੁਨੇਹਾ_didout = FALSE;
			    c = K_IGNORE;
			    ਸੁਨੇਹਾ_ਖਾਨਾ =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_ਚੌੜਾਈ - 1 :
#ਜੇ_ਅੰਤ
				0;
			}
			ਜੇ (quit_ਹੋਰ)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_ਹੋਰ = FALSE;
			    got_int = FALSE;
			}
			ਨਹੀਂਤਾਂ ਜੇ (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    ਪ੍ਰਾਪਤੀ_return_ਸੁਨੇਹਾ();
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (ਸੁਨੇਹਾ_scrolled > ਪੰਕਤੀਆਂ - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_ਥੱਲੇ || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } ਜਦੋਂ ((had_got_int && c == Ctrl_C)
				|| c == K_IGNORE
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
				|| c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
#ਜੇ_ਅੰਤ
				|| c == K_LEFTDRAG   || c == K_LEFTRELEASE
				|| c == K_MIDDLEDRAG || c == K_MIDDLERELEASE
				|| c == K_RIGHTDRAG  || c == K_RIGHTRELEASE
				|| c == K_MOUSELEFT  || c == K_MOUSERIGHT
				|| c == K_MOUSEDOWN  || c == K_MOUSEUP
				|| c == K_MOUSEMOVE
				|| (!mouse_has(MOUSE_RETURN)
				    && mouse_ਪੰਕਤੀ < ਸੁਨੇਹਾ_ਪੰਕਤੀ
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    ਜੇ (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(ਖਾਲੀ)jump_ਮੰਜ਼ਿਲ_mouse(MOUSE_SETPOS, ਨਿਰਾ, 0);
	    ਨਹੀਂਤਾਂ ਜੇ (!ਕੁੰਜੀ_Typed || (vim_strchr((char_u *)"\r\n ", c) == ਨਿਰਾ
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_ਨਕਾਬ);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(ਸੁਨੇਹਾ_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    ਜੇ (c == ':' || c == '?' || c == '/')
    {
	ਜੇ (!exmode_ਸਰਗਰਮ)
	    cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	skip_redraw = TRUE;	    // skip redraw once
	do_redraw = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	skip_ਪਦ_loop = TRUE;
#ਜੇ_ਅੰਤ
    }

    /*
     * If the window size changed set_shellsize() will redraw the screen.
     * Otherwise the screen is only redrawn if 'redraw' is set and no ':'
     * typed.
     */
    tmpState = State;
    State = oldState;		    // restore State before set_shellsize
    setmouse();
    ਸੁਨੇਹਾ_check();

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    ਜੇ (swapping_screen() && !termcap_ਸਰਗਰਮ)
	newline_on_exit = TRUE;
#ਜੇ_ਅੰਤ

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    ਸਤਰਾਂ_ਖੱਬਾ = -1;		// reset lines_left at next msg_start()
    reset_ਆਖਰੀ_sourcing();
    ਜੇ (keep_ਸੁਨੇਹਾ != ਨਿਰਾ && vim_strsize(keep_ਸੁਨੇਹਾ) >=
			    (ਪੰਕਤੀਆਂ - cmdline_ਪੰਕਤੀ - 1) * cmdline_ਚੌੜਾਈ + sc_ਖਾਨਾ)
	VIM_CLEAR(keep_ਸੁਨੇਹਾ);	    // don't redisplay message, it's too long

    ਜੇ (tmpState == ਬਹੁਲਕ_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    ਨਹੀਂਤਾਂ ਜੇ (!skip_redraw
	    && (redraw == TRUE || (ਸੁਨੇਹਾ_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪ੍ਰਾਪਤੀ_return_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
    ਪੂਰਨ		save_p_ਹੋਰ = p_ਹੋਰ;

    p_ਹੋਰ = FALSE;	// don't want to see this message when scrolling back
    ਜੇ (ਸੁਨੇਹਾ_didout)	// start on a new line
	ਸੁਨੇਹਾ_putchar('\n');
    ਜੇ (got_int)
	ਸੁਨੇਹਾ_puts(_("Interrupt: "));

    ਸੁਨੇਹਾ_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    ਜੇ (!ਸੁਨੇਹਾ_use_printf())
	ਸੁਨੇਹਾ_clr_eos();
    p_ਹੋਰ = save_p_ਹੋਰ;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    ਖਾਲੀ
ਸੈੱਟ_keep_ਸੁਨੇਹਾ(char_u *s, ਪੂਰਨ attr)
{
    vim_free(keep_ਸੁਨੇਹਾ);
    ਜੇ (s != ਨਿਰਾ && ਸੁਨੇਹਾ_silent == 0)
	keep_ਸੁਨੇਹਾ = vim_strsave(s);
    ਨਹੀਂਤਾਂ
	keep_ਸੁਨੇਹਾ = ਨਿਰਾ;
    keep_ਸੁਨੇਹਾ_ਹੋਰ = FALSE;
    keep_ਸੁਨੇਹਾ_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    ਖਾਲੀ
ਸੈੱਟ_keep_ਸੁਨੇਹਾ_from_hist(ਖਾਲੀ)
{
    ਜੇ (keep_ਸੁਨੇਹਾ == ਨਿਰਾ && ਆਖਰੀ_ਸੁਨੇਹਾ_hist != ਨਿਰਾ && ਸੁਨੇਹਾ_scrolled == 0
						      && (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ))
	ਸੈੱਟ_keep_ਸੁਨੇਹਾ(ਆਖਰੀ_ਸੁਨੇਹਾ_hist->ਸੁਨੇਹਾ, ਆਖਰੀ_ਸੁਨੇਹਾ_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
    ਪੂਰਨ		did_return = FALSE;

    ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ < cmdline_ਪੰਕਤੀ)
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ;

    ਜੇ (!ਸੁਨੇਹਾ_silent)
    {
	VIM_CLEAR(keep_ਸੁਨੇਹਾ);
	need_fileinfo = FALSE;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	ਸੁਨੇਹਾ_clr_eos();
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
    ਜੇ (ਵਿੱਚ_echowindow)
    {
	ਜੇ (popup_ਸੁਨੇਹਾ_win_visible()
		    && ((ਸੁਨੇਹਾ_ਖਾਨਾ > 0 && (ਸੁਨੇਹਾ_scroll || !ਭਰਿਆ_screen))
			|| ਵਿੱਚ_echowindow))
	{
	    win_T *wp = popup_get_ਸੁਨੇਹਾ_win();

	    // start a new line
	    curbuf = wp->w_ਬਫਰ;
	    ml_append(wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_ਬਫਰ;
	}
	ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (!ਸੁਨੇਹਾ_scroll && ਭਰਿਆ_screen)	// overwrite last message
    {
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ;
	ਸੁਨੇਹਾ_ਖਾਨਾ =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_ਚੌੜਾਈ - 1 :
#ਜੇ_ਅੰਤ
	    0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਸੁਨੇਹਾ_didout || ਵਿੱਚ_echowindow)
    {
	// start message on next line
	ਸੁਨੇਹਾ_putchar('\n');
	did_return = TRUE;
	ਜੇ (exmode_ਸਰਗਰਮ != EXMODE_ਸਾਧਾਰਨ)
	    cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
    }
    ਜੇ (!ਸੁਨੇਹਾ_didany || ਸਤਰਾਂ_ਖੱਬਾ < 0)
	ਸੁਨੇਹਾ_starthere();
    ਜੇ (ਸੁਨੇਹਾ_silent == 0)
    {
	ਸੁਨੇਹਾ_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    ਜੇ (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_starthere(ਖਾਲੀ)
{
    ਸਤਰਾਂ_ਖੱਬਾ = cmdline_ਪੰਕਤੀ;
    ਸੁਨੇਹਾ_didany = FALSE;
}

    ਖਾਲੀ
ਸੁਨੇਹਾ_putchar(ਪੂਰਨ c)
{
    ਸੁਨੇਹਾ_putchar_attr(c, 0);
}

    ਖਾਲੀ
ਸੁਨੇਹਾ_putchar_attr(ਪੂਰਨ c, ਪੂਰਨ attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    ਜੇ (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_ਦੂਜਾ(c);
	buf[2] = K_ਤੀਜਾ(c);
	buf[3] = NUL;
    }
    ਨਹੀਂਤਾਂ
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)buf, attr);
}

    ਖਾਲੀ
ਸੁਨੇਹਾ_outnum(ਲੰਮਾ n)
{
    ਅੱਖਰ	buf[20];

    sprintf(buf, "%ld", n);
    ਸੁਨੇਹਾ_puts(buf);
}

    ਖਾਲੀ
ਸੁਨੇਹਾ_home_replace(char_u *fname)
{
    ਸੁਨੇਹਾ_home_replace_attr(fname, 0);
}

#ਜੇ defined(FEAT_ਲੱਭੋ_ID)
    ਖਾਲੀ
ਸੁਨੇਹਾ_home_replace_hl(char_u *fname)
{
    ਸੁਨੇਹਾ_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_home_replace_attr(char_u *fname, ਪੂਰਨ attr)
{
    char_u	*name;

    name = home_replace_save(ਨਿਰਾ, fname);
    ਜੇ (name != ਨਿਰਾ)
	ਸੁਨੇਹਾ_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    ਪੂਰਨ
ਸੁਨੇਹਾ_outtrans(char_u *str)
{
    ਵਾਪਸ ਸੁਨੇਹਾ_outtrans_attr(str, 0);
}

    ਪੂਰਨ
ਸੁਨੇਹਾ_outtrans_attr(char_u *str, ਪੂਰਨ attr)
{
    ਵਾਪਸ ਸੁਨੇਹਾ_outtrans_len_attr(str, (ਪੂਰਨ)STRLEN(str), attr);
}

    ਪੂਰਨ
ਸੁਨੇਹਾ_outtrans_len(char_u *str, ਪੂਰਨ len)
{
    ਵਾਪਸ ਸੁਨੇਹਾ_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
ਸੁਨੇਹਾ_outtrans_one(char_u *p, ਪੂਰਨ attr)
{
    ਪੂਰਨ		l;

    ਜੇ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	ਸੁਨੇਹਾ_outtrans_len_attr(p, l, attr);
	ਵਾਪਸ p + l;
    }
    ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)transchar_byte_buf(ਨਿਰਾ, *p), attr);
    ਵਾਪਸ p + 1;
}

    ਪੂਰਨ
ਸੁਨੇਹਾ_outtrans_len_attr(char_u *msgstr, ਪੂਰਨ len, ਪੂਰਨ attr)
{
    ਪੂਰਨ		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_ਸ਼ੁਰੂ = msgstr;
    char_u	*s;
    ਪੂਰਨ		mb_l;
    ਪੂਰਨ		c;
    ਪੂਰਨ		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    ਜੇ (attr == 0)
	attr = HL_ATTR(HLF_ਸੁਨੇਹਾ);

    // if MSG_HIST flag set, add message to history
    ਜੇ (attr & ਸੁਨੇਹਾ_HIST)
    {
	ਜੋੜੋ_ਸੁਨੇਹਾ_hist(str, len, attr);
	attr &= ~ਸੁਨੇਹਾ_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    ਜੇ (ਸੁਨੇਹਾ_silent == 0 && len > 0 && ਸੁਨੇਹਾ_ਪੰਕਤੀ >= cmdline_ਪੰਕਤੀ && ਸੁਨੇਹਾ_ਖਾਨਾ == 0)
    {
	clear_cmdline = FALSE;
	ਬਹੁਲਕ_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    ਜੇ (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	ਸੁਨੇਹਾ_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    ਜਦੋਂ (--len >= 0 && !got_int)
    {
	ਜੇ (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	ਨਹੀਂਤਾਂ
	    mb_l = 1;
	ਜੇ (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    ਜੇ (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    ਨਹੀਂਤਾਂ
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		ਜੇ (str > plain_ਸ਼ੁਰੂ)
		    ਸੁਨੇਹਾ_puts_attr_len((ਅੱਖਰ *)plain_ਸ਼ੁਰੂ,
					       (ਪੂਰਨ)(str - plain_ਸ਼ੁਰੂ), attr);
		plain_ਸ਼ੁਰੂ = str + mb_l;
		ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)transchar_buf(ਨਿਰਾ, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	ਨਹੀਂਤਾਂ
	{
	    s = transchar_byte_buf(ਨਿਰਾ, *str);
	    ਜੇ (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		ਜੇ (str > plain_ਸ਼ੁਰੂ)
		    ਸੁਨੇਹਾ_puts_attr_len((ਅੱਖਰ *)plain_ਸ਼ੁਰੂ,
					       (ਪੂਰਨ)(str - plain_ਸ਼ੁਰੂ), attr);
		plain_ਸ਼ੁਰੂ = str + 1;
		ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (ਪੂਰਨ)STRLEN(s);
	    }
	    ਨਹੀਂਤਾਂ
		++retval;
	    ++str;
	}
    }

    ਜੇ (str > plain_ਸ਼ੁਰੂ && !got_int)
	// print the printable chars at the end
	ਸੁਨੇਹਾ_puts_attr_len((ਅੱਖਰ *)plain_ਸ਼ੁਰੂ, (ਪੂਰਨ)(str - plain_ਸ਼ੁਰੂ), attr);

    got_int |= save_got_int;

    ਵਾਪਸ retval;
}

#ਜੇ defined(FEAT_QUICKFIX)
    ਖਾਲੀ
ਸੁਨੇਹਾ_make(char_u *arg)
{
    ਪੂਰਨ	    i;
    ਸਥਿਰ char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    ਲਈ (i = 5; *arg && i >= 0; --i)
	ਜੇ (*arg++ != str[i])
	    ਰੋਕੋ;
    ਜੇ (i < 0)
    {
	ਸੁਨੇਹਾ_putchar('\n');
	ਲਈ (i = 0; rs[i]; ++i)
	    ਸੁਨੇਹਾ_putchar(rs[i] - 3);
    }
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
ਸੁਨੇਹਾ_outtrans_special(
    char_u	*strstart,
    ਪੂਰਨ		from,	// TRUE for lhs of a mapping
    ਪੂਰਨ		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    ਪੂਰਨ		retval = 0;
    ਅੱਖਰ	*ਪਾਠ;
    ਪੂਰਨ		attr;
    ਪੂਰਨ		len;

    attr = HL_ATTR(HLF_8);
    ਜਦੋਂ (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	ਜੇ ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    ਪਾਠ = "<Space>";
	    ++str;
	}
	ਨਹੀਂਤਾਂ
	    ਪਾਠ = (ਅੱਖਰ *)str2special(&str, from, FALSE);
	ਜੇ (ਪਾਠ[0] != NUL && ਪਾਠ[1] == NUL)
	    // single-byte character or illegal byte
	    ਪਾਠ = (ਅੱਖਰ *)transchar_byte_buf(ਨਿਰਾ, (char_u)ਪਾਠ[0]);
	len = vim_strsize((char_u *)ਪਾਠ);
	ਜੇ (maxlen > 0 && retval + len >= maxlen)
	    ਰੋਕੋ;
	// Highlight special keys
	ਸੁਨੇਹਾ_puts_attr(ਪਾਠ, len > 1
		&& (*mb_ptr2len)((char_u *)ਪਾਠ) <= 1 ? attr : 0);
	retval += len;
    }
    ਵਾਪਸ retval;
}

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    ਪੂਰਨ	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    ਪੂਰਨ	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    ਜਦੋਂ (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    ਵਾਪਸ (char_u *)ga.ga_ਅੰਕੜੇ;
}
#ਜੇ_ਅੰਤ

/*
 * Return the printable string for the key codes at "*sp".
 * On illegal byte return a string with only that byte.
 * Used for translating the lhs or rhs of a mapping to printable chars.
 * Advances "sp" to the next code.
 */
    char_u *
str2special(
    char_u	**sp,
    ਪੂਰਨ		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    ਪੂਰਨ		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    ਪੂਰਨ			c;
    ਸਥਿਰ char_u	buf[7];
    char_u		*str = *sp;
    ਪੂਰਨ			modifiers = 0;
    ਪੂਰਨ			special = FALSE;

    ਜੇ (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	ਜੇ (p != ਨਿਰਾ)
	    ਵਾਪਸ p;
    }

    c = *str;
    ਜੇ ((c == K_SPECIAL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		|| c == CSI
#ਜੇ_ਅੰਤ
	) && str[1] != NUL && str[2] != NUL)
    {
	ਜੇ (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	ਜੇ ((c == K_SPECIAL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    || c == CSI
#ਜੇ_ਅੰਤ
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = ਮੰਜ਼ਿਲ_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	ਜੇ (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    ਜੇ (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	ਜੇ (p != ਨਿਰਾ)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	ਨਹੀਂਤਾਂ
	    // illegal byte
	    *sp = str + 1;
    }
    ਨਹੀਂਤਾਂ
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    ਜੇ (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	ਵਾਪਸ get_special_ਕੁੰਜੀ_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    ਵਾਪਸ buf;
}

/*
 * Translate a key sequence into special key names.
 */
    ਖਾਲੀ
str2specialbuf(char_u *sp, char_u *buf, ਪੂਰਨ len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    ਜਦੋਂ (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	ਜੇ (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }
}

/*
 * print line for :print or :list command
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_prt_ਸਤਰ(char_u *s, ਪੂਰਨ list)
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		ਖਾਨਾ = 0;
    ਪੂਰਨ		n_extra = 0;
    ਪੂਰਨ		c_extra = 0;
    ਪੂਰਨ		c_final = 0;
    char_u	*p_extra = ਨਿਰਾ;	    // init to make SASC shut up
    ਪੂਰਨ		n;
    ਪੂਰਨ		attr = 0;
    char_u	*trail = ਨਿਰਾ;
    char_u	*lead = ਨਿਰਾ;
    ਪੂਰਨ		ਵਿੱਚ_multispace = FALSE;
    ਪੂਰਨ		multispace_pos = 0;
    ਪੂਰਨ		l;
    char_u	buf[MB_MAXBYTES + 1];

    ਜੇ (curwin->w_p_list)
	list = TRUE;

    ਜੇ (list)
    {
	// find start of trailing whitespace
	ਜੇ (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    ਜਦੋਂ (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	ਜੇ (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != ਨਿਰਾ
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    ਜਦੋਂ (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    ਜੇ (*lead == NUL)
		lead = ਨਿਰਾ;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    ਜੇ (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	ਸੁਨੇਹਾ_putchar(' ');

    ਜਦੋਂ (!got_int)
    {
	ਜੇ (n_extra > 0)
	{
	    --n_extra;
	    ਜੇ (n_extra == 0 && c_final)
		c = c_final;
	    ਨਹੀਂਤਾਂ ਜੇ (c_extra)
		c = c_extra;
	    ਨਹੀਂਤਾਂ
		c = *p_extra++;
	}
	ਨਹੀਂਤਾਂ ਜੇ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    ਖਾਨਾ += (*mb_ptr2cells)(s);
	    ਜੇ (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		ਪੂਰਨ len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)buf);
	    s += l;
	    ਜਾਰੀ;
	}
	ਨਹੀਂਤਾਂ
	{
	    attr = 0;
	    c = *s++;
	    ਜੇ (list)
	    {
		ਵਿੱਚ_multispace = c == ' ' && (*s == ' '
						 || (ਖਾਨਾ > 0 && s[-2] == ' '));
		ਜੇ (!ਵਿੱਚ_multispace)
		    multispace_pos = 0;
	    }
	    ਜੇ (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		n_extra = tabstop_padding(ਖਾਨਾ, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#ਨਹੀਂਤਾਂ
		n_extra = curbuf->b_p_ts - ਖਾਨਾ % curbuf->b_p_ts - 1;
#ਜੇ_ਅੰਤ
		ਜੇ (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਪੂਰਨ lcs_tab1 = curwin->w_lcs_chars.tab1;
		    ਪੂਰਨ lcs_tab2 = curwin->w_lcs_chars.tab2;
		    ਪੂਰਨ lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    ਜੇ (lead != ਨਿਰਾ && s <= lead
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
	    ਨਹੀਂਤਾਂ ਜੇ (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(ਨਿਰਾ, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c == ' ')
	    {
		ਜੇ (lead != ਨਿਰਾ && s <= lead && ਵਿੱਚ_multispace
			&& curwin->w_lcs_chars.leadmultispace != ਨਿਰਾ)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    ਜੇ (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		ਨਹੀਂਤਾਂ ਜੇ (lead != ਨਿਰਾ && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		ਨਹੀਂਤਾਂ ਜੇ (trail != ਨਿਰਾ && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_multispace
			&& curwin->w_lcs_chars.multispace != ਨਿਰਾ)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    ਜੇ (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		ਨਹੀਂਤਾਂ ਜੇ (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	ਜੇ (c == NUL)
	    ਰੋਕੋ;

	ਸੁਨੇਹਾ_putchar_attr(c, attr);
	ਖਾਨਾ++;
    }
    ਸੁਨੇਹਾ_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    ਸਥਿਰ char_u *
screen_puts_mbyte(char_u *s, ਪੂਰਨ l, ਪੂਰਨ attr)
{
    ਪੂਰਨ		cw;

    ਸੁਨੇਹਾ_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    ਜੇ (cw > 1 && (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		cmdmsg_rl ? ਸੁਨੇਹਾ_ਖਾਨਾ <= 1 :
#ਜੇ_ਅੰਤ
		ਸੁਨੇਹਾ_ਖਾਨਾ == cmdline_ਚੌੜਾਈ - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	ਸੁਨੇਹਾ_screen_putchar('>', HL_ATTR(HLF_AT));
	ਵਾਪਸ s;
    }

    screen_puts_len(s, l, ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ, attr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (cmdmsg_rl)
    {
	ਸੁਨੇਹਾ_ਖਾਨਾ -= cw;
	ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ == 0)
	{
	    ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ;
	    ++ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	}
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	ਸੁਨੇਹਾ_ਖਾਨਾ += cw;
	ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ >= cmdline_ਚੌੜਾਈ)
	{
	    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
	    ++ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	}
    }
    ਵਾਪਸ s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_puts(ਅੱਖਰ *s)
{
    ਸੁਨੇਹਾ_puts_attr(s, 0);
}

    ਖਾਲੀ
ਸੁਨੇਹਾ_puts_title(ਅੱਖਰ *s)
{
    ਸੁਨੇਹਾ_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_outtrans_long_len_attr(char_u *longstr, ਪੂਰਨ len, ਪੂਰਨ attr)
{
    ਪੂਰਨ		slen = len;
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼;

    ਗੁੰਜਾਇਸ਼ = cmdline_ਚੌੜਾਈ - ਸੁਨੇਹਾ_ਖਾਨਾ;
    ਜੇ (len > ਗੁੰਜਾਇਸ਼ && ਗੁੰਜਾਇਸ਼ >= 20)
    {
	slen = (ਗੁੰਜਾਇਸ਼ - 3) / 2;
	ਸੁਨੇਹਾ_outtrans_len_attr(longstr, slen, attr);
	ਸੁਨੇਹਾ_puts_attr("...", HL_ATTR(HLF_8));
    }
    ਸੁਨੇਹਾ_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    ਖਾਲੀ
ਸੁਨੇਹਾ_outtrans_long_attr(char_u *longstr, ਪੂਰਨ attr)
{
    ਸੁਨੇਹਾ_outtrans_long_len_attr(longstr, (ਪੂਰਨ)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_puts_attr(ਅੱਖਰ *s, ਪੂਰਨ attr)
{
    ਸੁਨੇਹਾ_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_puts_attr_len(ਅੱਖਰ *str, ਪੂਰਨ maxlen, ਪੂਰਨ attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    ਜੇ (ਸੁਨੇਹਾ_silent != 0)
	ਵਾਪਸ;

    ਜੇ (attr == 0)
	attr = HL_ATTR(HLF_ਸੁਨੇਹਾ);

    // if MSG_HIST flag set, add message to history
    ਜੇ ((attr & ਸੁਨੇਹਾ_HIST) && maxlen < 0)
    {
	ਜੋੜੋ_ਸੁਨੇਹਾ_hist((char_u *)str, -1, attr);
	attr &= ~ਸੁਨੇਹਾ_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    ਜੇ (ਸੁਨੇਹਾ_scrolled != 0 && !ਸੁਨੇਹਾ_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    ਸੁਨੇਹਾ_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    ਜੇ (ਸੁਨੇਹਾ_use_printf())
	ਸੁਨੇਹਾ_puts_printf((char_u *)str, maxlen);
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_puts_display((char_u *)str, maxlen, attr, FALSE);

    need_fileinfo = FALSE;
}

// values for "where"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUT_APPEND 0		// append to "lnum"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUT_TRUNC 1		// replace "lnum"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUT_ਹੇਠਾਂ 2		// add below "lnum"
				//
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
/*
 * Put text "t_s" until "end" in the message window.
 * "where" specifies where to put the text.
 */
    ਸਥਿਰ ਖਾਲੀ
put_ਸੁਨੇਹਾ_win(win_T *wp, ਪੂਰਨ where, char_u *t_s, char_u *ਸਿਰਾ, linenr_T lnum)
{
    char_u  *p;

    ਜੇ (where == PUT_ਹੇਠਾਂ)
    {
	ਜੇ (*ਸਿਰਾ != NUL)
	{
	    p = vim_strnsave(t_s, ਸਿਰਾ - t_s);
	    ਜੇ (p == ਨਿਰਾ)
		ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ
	    p = t_s;
	ml_append_buf(wp->w_ਬਫਰ, lnum, p, (colnr_T)0, FALSE);
	ਜੇ (p != t_s)
	    vim_free(p);
    }
    ਨਹੀਂਤਾਂ
    {
	char_u *newp;

	curbuf = wp->w_ਬਫਰ;
	ਜੇ (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    ਜੇ (newp == ਨਿਰਾ)
		ਵਾਪਸ;
	    ਜੇ (*ਸਿਰਾ != NUL)
		newp[STRLEN(ml_get(lnum)) + (ਸਿਰਾ - t_s)] = NUL;
	}
	ਨਹੀਂਤਾਂ
	{
	    newp = vim_strnsave(t_s, ਸਿਰਾ - t_s);
	    ਜੇ (newp == ਨਿਰਾ)
		ਵਾਪਸ;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_ਬਫਰ;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    ਸੁਨੇਹਾ_ਖਾਨਾ += (ਪੂਰਨ)(ਸਿਰਾ - t_s);
}
#ਜੇ_ਅੰਤ

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_puts_display(
    char_u	*str,
    ਪੂਰਨ		maxlen,
    ਪੂਰਨ		attr,
    ਪੂਰਨ		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    ਪੂਰਨ		t_ਖਾਨਾ = 0;	// screen cells todo, 0 when "t_s" not used
    ਪੂਰਨ		l;
    ਪੂਰਨ		cw;
    char_u	*sb_str = str;
    ਪੂਰਨ		sb_ਖਾਨਾ = ਸੁਨੇਹਾ_ਖਾਨਾ;
    ਪੂਰਨ		wrap;
    ਪੂਰਨ		did_ਆਖਰੀ_char;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
    ਪੂਰਨ		where = PUT_APPEND;
    win_T	*ਸੁਨੇਹਾ_win = ਨਿਰਾ;
    linenr_T    lnum = 1;

    ਜੇ (ਵਿੱਚ_echowindow)
    {
	ਸੁਨੇਹਾ_win = popup_get_ਸੁਨੇਹਾ_win();

	ਜੇ (ਸੁਨੇਹਾ_win != ਨਿਰਾ)
	{
	    ਜੇ (!popup_ਸੁਨੇਹਾ_win_visible())
	    {
		ਜੇ (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    ਸੁਨੇਹਾ_win = ਨਿਰਾ;
		}
		ਨਹੀਂਤਾਂ
		{
		    // currently hidden, make it empty
		    curbuf = ਸੁਨੇਹਾ_win->w_ਬਫਰ;
		    ਜਦੋਂ ((curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ) == 0)
			ml_delete(1);
		    curbuf = curwin->w_ਬਫਰ;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		lnum = ਸੁਨੇਹਾ_win->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#ਜੇ_ਅੰਤ

    did_wait_return = FALSE;
    ਜਦੋਂ ((maxlen < 0 || (ਪੂਰਨ)(s - str) < maxlen) && *s != NUL)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	// For echowindow, use full width; for regular messages, leave last column
	ਪੂਰਨ wrap_ਖਾਨਾ = ਸੁਨੇਹਾ_win != ਨਿਰਾ ? cmdline_ਚੌੜਾਈ : cmdline_ਚੌੜਾਈ - 1;
#ਨਹੀਂਤਾਂ
	ਪੂਰਨ wrap_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - 1;
#ਜੇ_ਅੰਤ

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	ਜੇ (!recurse && ਸੁਨੇਹਾ_ਪੰਕਤੀ >= ਪੰਕਤੀਆਂ - 1 && (*s == '\n' || (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			ਸੁਨੇਹਾ_ਖਾਨਾ <= 1
		      || (*s == TAB && ਸੁਨੇਹਾ_ਖਾਨਾ <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && ਸੁਨੇਹਾ_ਖਾਨਾ <= 2))
		    :
#ਜੇ_ਅੰਤ
		      ((*s != '\r' && ਸੁਨੇਹਾ_ਖਾਨਾ + t_ਖਾਨਾ >= wrap_ਖਾਨਾ)
		       || (*s == TAB && ਸੁਨੇਹਾ_ਖਾਨਾ + t_ਖਾਨਾ
			   >= (wrap_ਖਾਨਾ & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && ਸੁਨੇਹਾ_ਖਾਨਾ + t_ਖਾਨਾ >= wrap_ਖਾਨਾ - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    ਜੇ (t_ਖਾਨਾ > 0)
	    {
		// output postponed text
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
		ਜੇ (ਸੁਨੇਹਾ_win != ਨਿਰਾ)
		{
		    put_ਸੁਨੇਹਾ_win(ਸੁਨੇਹਾ_win, where, t_s, s, lnum);
		    ਜੇ (where == PUT_ਹੇਠਾਂ)
			++lnum;
		    t_ਖਾਨਾ = 0;
		    where = PUT_ਹੇਠਾਂ;
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    t_puts(&t_ਖਾਨਾ, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    ਜੇ (ਸੁਨੇਹਾ_no_ਹੋਰ && ਸਤਰਾਂ_ਖੱਬਾ == 0)
		ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    ਜੇ (ਸੁਨੇਹਾ_win == ਨਿਰਾ)
#ਜੇ_ਅੰਤ
		// Scroll the screen up one line.
		ਸੁਨੇਹਾ_scroll_ਉੱਪਰ();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    ਜੇ (ਸੁਨੇਹਾ_win == ਨਿਰਾ)
	    {
#ਜੇ_ਅੰਤ
		ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 2;
		ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ >= cmdline_ਚੌੜਾਈ)  // can happen after screen resize
		    ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    }
#ਜੇ_ਅੰਤ

	    // Display char in last column before showing more-prompt.
	    ਜੇ (*s >= ' '
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#ਜੇ_ਅੰਤ
	       )
	    {
		ਜੇ (has_mbyte)
		{
		    ਜੇ (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (ਪੂਰਨ)((str + maxlen) - s));
		    ਨਹੀਂਤਾਂ
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ_screen_putchar(*s++, attr);
		did_ਆਖਰੀ_char = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
		did_ਆਖਰੀ_char = FALSE;

	    ਜੇ (p_ਹੋਰ)
		// store text for scrolling back
		store_sb_ਪਾਠ(&sb_str, s, attr, &sb_ਖਾਨਾ, TRUE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    ਜੇ (ਸੁਨੇਹਾ_win == ਨਿਰਾ)
	    {
#ਜੇ_ਅੰਤ
		inc_ਸੁਨੇਹਾ_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		ਜੇ (cmdline_ਪੰਕਤੀ > 0 && !exmode_ਸਰਗਰਮ)
		    --cmdline_ਪੰਕਤੀ;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		ਜੇ (ਸਤਰਾਂ_ਖੱਬਾ > 0)
		    --ਸਤਰਾਂ_ਖੱਬਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (p_ਹੋਰ && ਸਤਰਾਂ_ਖੱਬਾ == 0 && State != ਬਹੁਲਕ_HITRETURN
					    && !ਸੁਨੇਹਾ_no_ਹੋਰ && !exmode_ਸਰਗਰਮ)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CON_DIALOG
		ਜੇ (do_ਹੋਰ_prompt(NUL))
		    s = confirm_ਸੁਨੇਹਾ_ਪੂਛ;
#ਨਹੀਂਤਾਂ
		(ਖਾਲੀ)do_ਹੋਰ_prompt(NUL);
#ਜੇ_ਅੰਤ
		ਜੇ (quit_ਹੋਰ)
		    ਵਾਪਸ;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    ਜੇ (did_ਆਖਰੀ_char)
		ਜਾਰੀ;
	}

	wrap = *s == '\n'
		    || ਸੁਨੇਹਾ_ਖਾਨਾ + t_ਖਾਨਾ >= wrap_ਖਾਨਾ
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && ਸੁਨੇਹਾ_ਖਾਨਾ + t_ਖਾਨਾ >= wrap_ਖਾਨਾ - 1);
	ਜੇ (t_ਖਾਨਾ > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    ਜੇ (ਸੁਨੇਹਾ_win != ਨਿਰਾ)
	    {
		put_ਸੁਨੇਹਾ_win(ਸੁਨੇਹਾ_win, where, t_s, s, lnum);
		ਜੇ (where == PUT_ਹੇਠਾਂ)
		    ++lnum;
		t_ਖਾਨਾ = 0;
		where = PUT_ਹੇਠਾਂ;
		// Reset msg_col after outputting to new line in echowindow
		ਜੇ (wrap)
		    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		t_puts(&t_ਖਾਨਾ, t_s, s, attr);
	}

	ਜੇ (wrap && p_ਹੋਰ && !recurse)
	    // store text for scrolling back
	    store_sb_ਪਾਠ(&sb_str, s, attr, &sb_ਖਾਨਾ, TRUE);

	ਜੇ (*s == '\n')		    // go to next line
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    ਜੇ (ਸੁਨੇਹਾ_win != ਨਿਰਾ)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		ਜੇ ((ਸੁਨੇਹਾ_win->w_ਬਫਰ->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ) == 0)
		{
		    put_ਸੁਨੇਹਾ_win(ਸੁਨੇਹਾ_win, PUT_ਹੇਠਾਂ, t_s, t_s, lnum);
		    ++lnum;
		}
		ਸੁਨੇਹਾ_ਖਾਨਾ = 0;  // Reset column for new line
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਸੁਨੇਹਾ_didout = FALSE;	    // remember that line is empty
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    ਜੇ (cmdmsg_rl)
		ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - 1;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
	    ਜੇ (++ਸੁਨੇਹਾ_ਪੰਕਤੀ >= ਪੰਕਤੀਆਂ)  // safety check
		ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*s == '\r')	    // go to column 0
	{
	    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    where = PUT_TRUNC;
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ ਜੇ (*s == '\b')	    // go to previous char
	{
	    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ)
		--ਸੁਨੇਹਾ_ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*s == TAB)	    // translate Tab into spaces
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    ਜੇ (ਸੁਨੇਹਾ_win != ਨਿਰਾ)
		ਸੁਨੇਹਾ_ਖਾਨਾ = (ਸੁਨੇਹਾ_ਖਾਨਾ + 7) % 8;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਕਰੋ
		    ਸੁਨੇਹਾ_screen_putchar(' ', attr);
		ਜਦੋਂ (ਸੁਨੇਹਾ_ਖਾਨਾ & 7);
	}
	ਨਹੀਂਤਾਂ ਜੇ (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		ਜੇ (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (ਪੂਰਨ)((str + maxlen) - s));
		ਨਹੀਂਤਾਂ
		    l = (*mb_ptr2len)(s);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#ਜੇ_ਅੰਤ
		    (cw > 1 && ਸੁਨੇਹਾ_ਖਾਨਾ + t_ਖਾਨਾ >= wrap_ਖਾਨਾ))
	    {
		ਜੇ (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ_screen_putchar(*s, attr);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// postpone this character until later
		ਜੇ (t_ਖਾਨਾ == 0)
		    t_s = s;
		t_ਖਾਨਾ += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    ਜੇ (t_ਖਾਨਾ > 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	ਜੇ (ਸੁਨੇਹਾ_win != ਨਿਰਾ)
	    put_ਸੁਨੇਹਾ_win(ਸੁਨੇਹਾ_win, where, t_s, s, lnum);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    t_puts(&t_ਖਾਨਾ, t_s, s, attr);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
    ਜੇ (ਸੁਨੇਹਾ_win != ਨਿਰਾ)
	popup_show_ਸੁਨੇਹਾ_win();
#ਜੇ_ਅੰਤ
    // Store the text for scroll back.
    ਜੇ (p_ਹੋਰ && !recurse)
	store_sb_ਪਾਠ(&sb_str, s, attr, &sb_ਖਾਨਾ, FALSE);

    ਸੁਨੇਹਾ_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    ਪੂਰਨ
ਸੁਨੇਹਾ_filtered(char_u *ਸੁਨੇਹਾ)
{
    ਪੂਰਨ ਮੇਲ;

    ਜੇ (cmdmod.cmod_filter_regmatch.regprog == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਮੇਲ = vim_regexec(&cmdmod.cmod_filter_regmatch, ਸੁਨੇਹਾ, (colnr_T)0);
    ਵਾਪਸ cmdmod.cmod_filter_force ? ਮੇਲ : !ਮੇਲ;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_scroll_ਉੱਪਰ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
    ਜੇ (ਵਿੱਚ_echowindow)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_undraw_cursor();
#ਜੇ_ਅੰਤ
    // scrolling up always works
    mch_disable_flush();
    screen_del_ਸਤਰਾਂ(0, 0, 1, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, TRUE, 0, ਨਿਰਾ);
    mch_enable_flush();

    ਜੇ (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, cmdline_ਖਾਨਾ_off,
		cmdline_ਖਾਨਾ_off + cmdline_ਚੌੜਾਈ, ' ', ' ', HL_ATTR(HLF_ਸੁਨੇਹਾ));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	ਜੇ (ScreenAttrs[ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀਆਂ - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 2, (ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1,
				 (ਪੂਰਨ)Columns - 1, (ਪੂਰਨ)Columns,
				 ' ', ' ', HL_ATTR(HLF_ਸੁਨੇਹਾ));
    }
}

/*
 * Increment "msg_scrolled".
 */
    ਸਥਿਰ ਖਾਲੀ
inc_ਸੁਨੇਹਾ_scrolled(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = ਨਿਰਾ;

	// v:scrollstart is empty, set it to the script/function name and line
	//ਸੰਖਿਆ
	ਜੇ (p.string == ਨਿਰਾ)
	{
	    p.string = (char_u *)_("Unknown");
	    p.ਲੰਮਾਈ = STRLEN(p.string);
	}
	ਨਹੀਂਤਾਂ
	{
	    size_t  tofreesize;

	    tofreesize = (ਪੂਰਨ)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    ਜੇ (tofree != ਨਿਰਾ)
	    {
		p.ਲੰਮਾਈ = vim_snprintf_safelen((ਅੱਖਰ *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (ਲੰਮਾ)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	ਸੈੱਟ_vim_var_string(VV_SCROLLSTART, p.string, (ਪੂਰਨ)p.ਲੰਮਾਈ);
	vim_free(tofree);
    }
#ਜੇ_ਅੰਤ
    ++ਸੁਨੇਹਾ_scrolled;
    ਸੈੱਟ_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ msgchunk_S msgchunk_T;
ਬਣਤਰ msgchunk_S
{
    msgchunk_T	*sb_ਅਗਲਾ;
    msgchunk_T	*sb_prev;
    ਅੱਖਰ	sb_eol;		// TRUE when line ends after this text
    ਪੂਰਨ		sb_ਸੁਨੇਹਾ_ਖਾਨਾ;	// column in which text starts
    ਪੂਰਨ		sb_attr;	// text attributes
    char_u	sb_ਪਾਠ[1];	// text to be displayed, actually longer
};

ਸਥਿਰ msgchunk_T *ਆਖਰੀ_msgchunk = ਨਿਰਾ; // last displayed text

ਸਥਿਰ msgchunk_T *ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(msgchunk_T *mps);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_ਪੂਰਾ
} sb_clear_T;

// When to clear text on next msg.
ਸਥਿਰ sb_clear_T do_clear_sb_ਪਾਠ = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    ਸਥਿਰ ਖਾਲੀ
store_sb_ਪਾਠ(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    ਪੂਰਨ		attr,
    ਪੂਰਨ		*sb_ਖਾਨਾ,
    ਪੂਰਨ		ਅੰਤ)		// line ends
{
    msgchunk_T	*mp;

    ਜੇ (do_clear_sb_ਪਾਠ == SB_CLEAR_ALL
	    || do_clear_sb_ਪਾਠ == SB_CLEAR_CMDLINE_ਪੂਰਾ)
    {
	clear_sb_ਪਾਠ(do_clear_sb_ਪਾਠ == SB_CLEAR_ALL);
	ਸੁਨੇਹਾ_sb_eol();  // prevent messages from overlapping
	do_clear_sb_ਪਾਠ = SB_CLEAR_NONE;
    }

    ਜੇ (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_ਪਾਠ) + (s - *sb_str) + 1);
	ਜੇ (mp != ਨਿਰਾ)
	{
	    mp->sb_eol = ਅੰਤ;
	    mp->sb_ਸੁਨੇਹਾ_ਖਾਨਾ = *sb_ਖਾਨਾ;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_ਪਾਠ, *sb_str, s - *sb_str);

	    ਜੇ (ਆਖਰੀ_msgchunk == ਨਿਰਾ)
	    {
		ਆਖਰੀ_msgchunk = mp;
		mp->sb_prev = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		mp->sb_prev = ਆਖਰੀ_msgchunk;
		ਆਖਰੀ_msgchunk->sb_ਅਗਲਾ = mp;
		ਆਖਰੀ_msgchunk = mp;
	    }
	    mp->sb_ਅਗਲਾ = ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਅੰਤ && ਆਖਰੀ_msgchunk != ਨਿਰਾ)
	ਆਖਰੀ_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_ਖਾਨਾ = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    ਖਾਲੀ
may_clear_sb_ਪਾਠ(ਖਾਲੀ)
{
    do_clear_sb_ਪਾਠ = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    ਖਾਲੀ
sb_ਪਾਠ_ਸ਼ੁਰੂ_cmdline(ਖਾਲੀ)
{
    ਜੇ (do_clear_sb_ਪਾਠ == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_ਪਾਠ_restart_cmdline();
    ਨਹੀਂਤਾਂ
    {
	ਸੁਨੇਹਾ_sb_eol();
	do_clear_sb_ਪਾਠ = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    ਖਾਲੀ
sb_ਪਾਠ_restart_cmdline(ਖਾਲੀ)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_ਪਾਠ = SB_CLEAR_CMDLINE_BUSY;

    ਜੇ (ਆਖਰੀ_msgchunk == ਨਿਰਾ || ਆਖਰੀ_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	ਵਾਪਸ;

    tofree = ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(ਆਖਰੀ_msgchunk);
    ਆਖਰੀ_msgchunk = tofree->sb_prev;
    ਜੇ (ਆਖਰੀ_msgchunk != ਨਿਰਾ)
	ਆਖਰੀ_msgchunk->sb_ਅਗਲਾ = ਨਿਰਾ;
    ਜਦੋਂ (tofree != ਨਿਰਾ)
    {
	msgchunk_T *tofree_ਅਗਲਾ = tofree->sb_ਅਗਲਾ;

	vim_free(tofree);
	tofree = tofree_ਅਗਲਾ;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    ਖਾਲੀ
sb_ਪਾਠ_ਸਿਰਾ_cmdline(ਖਾਲੀ)
{
    do_clear_sb_ਪਾਠ = SB_CLEAR_CMDLINE_ਪੂਰਾ;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    ਖਾਲੀ
clear_sb_ਪਾਠ(ਪੂਰਨ all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    ਜੇ (all)
	lastp = &ਆਖਰੀ_msgchunk;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਆਖਰੀ_msgchunk == ਨਿਰਾ)
	    ਵਾਪਸ;
	lastp = &ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(ਆਖਰੀ_msgchunk)->sb_prev;
    }

    ਜਦੋਂ (*lastp != ਨਿਰਾ)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    ਖਾਲੀ
show_sb_ਪਾਠ(ਖਾਲੀ)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(ਆਖਰੀ_msgchunk);
    ਜੇ (mp == ਨਿਰਾ || mp->sb_prev == ਨਿਰਾ)
	vim_beep(BO_MESS);
    ਨਹੀਂਤਾਂ
    {
	do_ਹੋਰ_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    ਸਥਿਰ msgchunk_T *
ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    ਜਦੋਂ (mp != ਨਿਰਾ && mp->sb_prev != ਨਿਰਾ && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    ਵਾਪਸ mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_sb_eol(ਖਾਲੀ)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    ਜੇ (ਆਖਰੀ_msgchunk != ਨਿਰਾ
	    && !(ਆਖਰੀ_msgchunk->sb_ਪਾਠ[0] == '\n'
		&& ਆਖਰੀ_msgchunk->sb_ਪਾਠ[1] == NUL))
	ਆਖਰੀ_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    ਸਥਿਰ msgchunk_T *
disp_sb_ਸਤਰ(ਪੂਰਨ ਪੰਕਤੀ, msgchunk_T *smp, ਪੂਰਨ clear_ਮੰਜ਼ਿਲ_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    ਲਈ (;;)
    {
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀ;
	ਸੁਨੇਹਾ_ਖਾਨਾ = mp->sb_ਸੁਨੇਹਾ_ਖਾਨਾ;
	p = mp->sb_ਪਾਠ;
	ਜੇ (*p == '\n')	    // don't display the line break
	    ++p;
	ਸੁਨੇਹਾ_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	ਜੇ (clear_ਮੰਜ਼ਿਲ_eol)
	    screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1,
		    cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ, cmdline_ਖਾਨਾ_off + cmdline_ਚੌੜਾਈ,
		    ' ', ' ', HL_ATTR(HLF_ਸੁਨੇਹਾ));

	ਜੇ (mp->sb_eol || mp->sb_ਅਗਲਾ == ਨਿਰਾ)
	    ਰੋਕੋ;
	mp = mp->sb_ਅਗਲਾ;
    }
    ਵਾਪਸ mp->sb_ਅਗਲਾ;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    ਸਥਿਰ ਖਾਲੀ
t_puts(
    ਪੂਰਨ		*t_ਖਾਨਾ,
    char_u	*t_s,
    char_u	*s,
    ਪੂਰਨ		attr)
{
    // output postponed text
    ਸੁਨੇਹਾ_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (ਪੂਰਨ)(s - t_s), ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ,
	    attr);
    ਸੁਨੇਹਾ_ਖਾਨਾ += *t_ਖਾਨਾ;
    *t_ਖਾਨਾ = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    ਜੇ (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--ਸੁਨੇਹਾ_ਖਾਨਾ;
    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ >= cmdline_ਚੌੜਾਈ)
    {
	ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
	++ਸੁਨੇਹਾ_ਪੰਕਤੀ;
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
    ਪੂਰਨ
ਸੁਨੇਹਾ_use_printf(ਖਾਲੀ)
{
    ਵਾਪਸ (!ਸੁਨੇਹਾ_check_screen()
#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.ਵਿੱਚ_use && !termcap_ਸਰਗਰਮ)
# ਨਹੀਂਤਾਂ
	    || !termcap_ਸਰਗਰਮ
# endif
#ਜੇ_ਅੰਤ
	    || (swapping_screen() && !termcap_ਸਰਗਰਮ)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_puts_printf(char_u *str, ਪੂਰਨ maxlen)
{
    char_u	*s = str;
    char_u	*buf = ਨਿਰਾ;
    char_u	*p = s;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (!(silent_ਬਹੁਲਕ && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#ਜੇ_ਅੰਤ
    ਜਦੋਂ ((maxlen < 0 || (ਪੂਰਨ)(s - str) < maxlen) && *s != NUL)
    {
	ਜੇ (!(silent_ਬਹੁਲਕ && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    ਜੇ (*s == NL)
	    {
		ਪੂਰਨ n = (ਪੂਰਨ)(s - p);

		buf = alloc(n + 3);
		ਜੇ (buf != ਨਿਰਾ)
		{
		    ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, p, n);
		    ਜੇ (!info_ਸੁਨੇਹਾ)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    ਜੇ (info_ਸੁਨੇਹਾ)   // informative message, not an error
			mch_ਸੁਨੇਹਾ((ਅੱਖਰ *)buf);
		    ਨਹੀਂਤਾਂ
			mch_errmsg((ਅੱਖਰ *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (cmdmsg_rl)
	{
	    ਜੇ (*s == CAR || *s == NL)
		ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - 1;
	    ਨਹੀਂਤਾਂ
		--ਸੁਨੇਹਾ_ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    ਜੇ (*s == CAR || *s == NL)
		ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
	    ਨਹੀਂਤਾਂ
		++ਸੁਨੇਹਾ_ਖਾਨਾ;
	}
	++s;
    }

    ਜੇ (*p != NUL && !(silent_ਬਹੁਲਕ && p_verbose == 0))
    {
	char_u *tofree = ਨਿਰਾ;

	ਜੇ (maxlen > 0 && vim_strlen_maxlen((ਅੱਖਰ *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਜੇ (info_ਸੁਨੇਹਾ)
		mch_ਸੁਨੇਹਾ((ਅੱਖਰ *)p);
	    ਨਹੀਂਤਾਂ
		mch_errmsg((ਅੱਖਰ *)p);
	    vim_free(tofree);
	}
    }

    ਸੁਨੇਹਾ_didout = TRUE;	    // assume that line is not empty

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (!(silent_ਬਹੁਲਕ && p_verbose == 0))
	mch_settmode(TMODE_RAW);
#ਜੇ_ਅੰਤ
}

/*
 * Show the more-prompt and handle the user response.
 * This takes care of scrolling back and displaying previously displayed text.
 * When at hit-enter prompt "typed_char" is the already typed character,
 * otherwise it's NUL.
 * Returns TRUE when jumping ahead to "confirm_msg_tail".
 */
    ਸਥਿਰ ਪੂਰਨ
do_ਹੋਰ_prompt(ਪੂਰਨ typed_char)
{
    ਸਥਿਰ ਪੂਰਨ	entered = FALSE;
    ਪੂਰਨ		ਵਰਤਿਆ_typed_char = typed_char;
    ਪੂਰਨ		oldState = State;
    ਪੂਰਨ		c;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CON_DIALOG
    ਪੂਰਨ		retval = FALSE;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		toscroll;
    msgchunk_T	*mp_ਆਖਰੀ = ਨਿਰਾ;
    msgchunk_T	*mp;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਸੁਨੇਹਾ_attr;

    ਸੁਨੇਹਾ_attr = HL_ATTR(HLF_ਸੁਨੇਹਾ);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    ਜੇ (entered || (State == ਬਹੁਲਕ_HITRETURN && typed_char == 0))
	ਵਾਪਸ FALSE;
    entered = TRUE;

    ਜੇ (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_ਆਖਰੀ = ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(ਆਖਰੀ_msgchunk);
	ਲਈ (i = 0; i < ਪੰਕਤੀਆਂ - 2 && mp_ਆਖਰੀ != ਨਿਰਾ
					     && mp_ਆਖਰੀ->sb_prev != ਨਿਰਾ; ++i)
	    mp_ਆਖਰੀ = ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(mp_ਆਖਰੀ->sb_prev);
    }

    State = ਬਹੁਲਕ_ASKMORE;
    setmouse();
    ਜੇ (typed_char == NUL)
	ਸੁਨੇਹਾ_moremsg(FALSE);
    ਲਈ (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	ਜੇ (ਵਰਤਿਆ_typed_char != NUL)
	{
	    c = ਵਰਤਿਆ_typed_char;	// was typed at hit-enter prompt
	    ਵਰਤਿਆ_typed_char = NUL;
	}
	ਨਹੀਂਤਾਂ
	    c = get_keystroke();

#ਜੇ defined(FEAT_MENU) && defined(FEAT_GUI)
	ਜੇ (c == K_MENU)
	{
	    ਪੂਰਨ idx = get_menu_ਸੂਚਕਾਂਕ(current_menu, ਬਹੁਲਕ_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    ਜੇ (idx == MENU_ਸੂਚਕਾਂਕ_INVALID)
		ਜਾਰੀ;
	    c = *current_menu->ਲੜੀਆਂ[idx];
	    ਜੇ (c != NUL && current_menu->ਲੜੀਆਂ[idx][1] != NUL)
		ins_typebuf(current_menu->ਲੜੀਆਂ[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#ਜੇ_ਅੰਤ

	toscroll = 0;
	ਚੋਣ (c)
	{
	ਹਾਲਤ BS:		// scroll one line back
	ਹਾਲਤ K_BS:
	ਹਾਲਤ 'k':
	ਹਾਲਤ K_ਉੱਪਰ:
	    toscroll = -1;
	    ਰੋਕੋ;

	ਹਾਲਤ CAR:		// one extra line
	ਹਾਲਤ NL:
	ਹਾਲਤ 'j':
	ਹਾਲਤ K_ਥੱਲੇ:
	    toscroll = 1;
	    ਰੋਕੋ;

	ਹਾਲਤ 'u':		// Up half a page
	    toscroll = -(ਪੰਕਤੀਆਂ / 2);
	    ਰੋਕੋ;

	ਹਾਲਤ 'd':		// Down half a page
	    toscroll = ਪੰਕਤੀਆਂ / 2;
	    ਰੋਕੋ;

	ਹਾਲਤ 'b':		// one page back
	ਹਾਲਤ Ctrl_B:
	ਹਾਲਤ K_PAGEUP:
	    toscroll = -(ਪੰਕਤੀਆਂ - 1);
	    ਰੋਕੋ;

	ਹਾਲਤ ' ':		// one extra page
	ਹਾਲਤ 'f':
	ਹਾਲਤ Ctrl_F:
	ਹਾਲਤ K_PAGEDOWN:
	ਹਾਲਤ K_LEFTMOUSE:
	    toscroll = ਪੰਕਤੀਆਂ - 1;
	    ਰੋਕੋ;

	ਹਾਲਤ 'g':		// all the way back to the start
	    toscroll = -999999;
	    ਰੋਕੋ;

	ਹਾਲਤ 'G':		// all the way to the end
	    toscroll = 999999;
	    ਸਤਰਾਂ_ਖੱਬਾ = 999999;
	    ਰੋਕੋ;

	ਹਾਲਤ ':':		// start new command line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CON_DIALOG
	    ਜੇ (!confirm_ਸੁਨੇਹਾ_ਵਰਤਿਆ)
#ਜੇ_ਅੰਤ
	    {
		// Since got_int is set all typeahead will be flushed, but we
		// want to keep this ':', remember that in a special way.
		typeahead_noflush(':');
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
		skip_ਪਦ_loop = TRUE;
#ਜੇ_ਅੰਤ
		cmdline_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;		// put ':' on this line
		skip_redraw = TRUE;		// skip redraw once
		need_wait_return = FALSE;	// don't wait in main()
	    }
	    // FALLTHROUGH
	ਹਾਲਤ 'q':		// quit
	ਹਾਲਤ Ctrl_C:
	ਹਾਲਤ ESC:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CON_DIALOG
	    ਜੇ (confirm_ਸੁਨੇਹਾ_ਵਰਤਿਆ)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		got_int = TRUE;
		quit_ਹੋਰ = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    ਸਤਰਾਂ_ਖੱਬਾ = ਪੰਕਤੀਆਂ - 1;
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	ਹਾਲਤ Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    ਜੇ (clip_ਤਾਰਾ.state == SELECT_ਪੂਰਾ)
		clip_copy_modeless_selection(TRUE);
	    ਜਾਰੀ;
#ਜੇ_ਅੰਤ
	ਮੂਲ:		// no valid response
	    ਸੁਨੇਹਾ_moremsg(TRUE);
	    ਜਾਰੀ;
	}

	ਜੇ (toscroll != 0)
	{
	    ਜੇ (toscroll < 0)
	    {
		// go to start of last line
		ਜੇ (mp_ਆਖਰੀ == ਨਿਰਾ)
		    mp = ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(ਆਖਰੀ_msgchunk);
		ਨਹੀਂਤਾਂ ਜੇ (mp_ਆਖਰੀ->sb_prev != ਨਿਰਾ)
		    mp = ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(mp_ਆਖਰੀ->sb_prev);
		ਨਹੀਂਤਾਂ
		    mp = ਨਿਰਾ;

		// go to start of line at top of the screen
		ਲਈ (i = 0; i < ਪੰਕਤੀਆਂ - 2 && mp != ਨਿਰਾ && mp->sb_prev != ਨਿਰਾ;
									  ++i)
		    mp = ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(mp->sb_prev);

		ਜੇ (mp != ਨਿਰਾ && mp->sb_prev != ਨਿਰਾ)
		{
		    // Find line to be displayed at top.
		    ਲਈ (i = 0; i > toscroll; --i)
		    {
			ਜੇ (mp == ਨਿਰਾ || mp->sb_prev == ਨਿਰਾ)
			    ਰੋਕੋ;
			mp = ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(mp->sb_prev);
			ਜੇ (mp_ਆਖਰੀ == ਨਿਰਾ)
			    mp_ਆਖਰੀ = ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(ਆਖਰੀ_msgchunk);
			ਨਹੀਂਤਾਂ
			    mp_ਆਖਰੀ = ਸੁਨੇਹਾ_sb_ਸ਼ੁਰੂ(mp_ਆਖਰੀ->sb_prev);
		    }

		    ਜੇ (toscroll == -1 && screen_ins_ਸਤਰਾਂ(0, 0, 1,
						     (ਪੂਰਨ)ਪੰਕਤੀਆਂ, 0, ਨਿਰਾ) == OK)
		    {
			// display line at top
			(ਖਾਲੀ)disp_sb_ਸਤਰ(0, mp, FALSE);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਪੂਰਨ did_clear = screenclear();

			// redisplay all lines
			ਲਈ (i = 0; mp != ਨਿਰਾ && i < ਪੰਕਤੀਆਂ - 1; ++i)
			{
			    mp = disp_sb_ਸਤਰ(i, mp, !did_clear);
			    ++ਸੁਨੇਹਾ_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// First display any text that we scrolled back.
		ਜਦੋਂ (toscroll > 0 && mp_ਆਖਰੀ != ਨਿਰਾ)
		{
		    // scroll up, display line at bottom
		    ਸੁਨੇਹਾ_scroll_ਉੱਪਰ();
		    inc_ਸੁਨੇਹਾ_scrolled();
		    screen_fill((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 2, (ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1,
			    cmdline_ਖਾਨਾ_off, cmdline_ਖਾਨਾ_off + cmdline_ਚੌੜਾਈ,
			    ' ', ' ', ਸੁਨੇਹਾ_attr);
		    mp_ਆਖਰੀ = disp_sb_ਸਤਰ((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 2, mp_ਆਖਰੀ, FALSE);
		    --toscroll;
		}
	    }

	    ਜੇ (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, cmdline_ਖਾਨਾ_off,
			cmdline_ਖਾਨਾ_off + cmdline_ਚੌੜਾਈ, ' ', ' ', ਸੁਨੇਹਾ_attr);
		ਸੁਨੇਹਾ_moremsg(FALSE);
		ਜਾਰੀ;
	    }

	    // display more text, return to caller
	    ਸਤਰਾਂ_ਖੱਬਾ = toscroll;
	}

	ਰੋਕੋ;
    }

    // clear the --more-- message
    screen_fill((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, cmdline_ਖਾਨਾ_off,
	    cmdline_ਖਾਨਾ_off + cmdline_ਚੌੜਾਈ, ' ', ' ', ਸੁਨੇਹਾ_attr);
    State = oldState;
    setmouse();
    ਜੇ (quit_ਹੋਰ)
    {
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
	ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਨਹੀਂਤਾਂ ਜੇ (cmdmsg_rl)
	ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - 1;
#ਜੇ_ਅੰਤ

    entered = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CON_DIALOG
    ਵਾਪਸ retval;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_ਸੁਨੇਹਾ
#  undef mch_ਸੁਨੇਹਾ
# endif

# ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ਸਥਿਰ ਖਾਲੀ
mch_errmsg_c(ਅੱਖਰ *str)
{
    DWORD   nwrite = 0;
    DWORD   ਬਹੁਲਕ = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    ਜੇ (Get_Console_ਬਹੁਲਕ(h, &ਬਹੁਲਕ) && enc_codepage >= 0
	    && (ਪੂਰਨ)GetConsoleCP() != enc_codepage)
    {
	ਪੂਰਨ	len = (ਪੂਰਨ)STRLEN(str);
	WCHAR	*w = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)str, &len);

	ਜੇ (w != ਨਿਰਾ)
	{
	    WriteConsoleW(h, w, len, &nwrite, ਨਿਰਾ);
	    vim_free(w);
	}
    }
    ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
mch_errmsg(ਅੱਖਰ *str)
{
# ਜੇ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    ਪੂਰਨ		len;
# endif

# ਜੇ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    ਜੇ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   ਨਹੀਂਤਾਂ
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.ਵਿੱਚ_use || gui.starting)
#  endif
	    )
    {
	fprintf(stderr, "%s", str);
	ਵਾਪਸ;
    }
# endif

# ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ਜੇ (!(gui.ਵਿੱਚ_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	ਵਾਪਸ;
    }
# endif

# ਜੇ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (ਪੂਰਨ)STRLEN(str) + 1;
    ਜੇ (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    ਜੇ (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_ਅੰਕੜੇ + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_ਅੰਕੜੇ + error_ga.ga_len;
	    ਲਈ (;;)
	    {
		p = vim_strchr(p, '\r');
		ਜੇ (p == ਨਿਰਾ)
		    ਰੋਕੋ;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ਸਥਿਰ ਖਾਲੀ
mch_ਸੁਨੇਹਾ_c(ਅੱਖਰ *str)
{
    DWORD   nwrite = 0;
    DWORD   ਬਹੁਲਕ;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    ਜੇ (Get_Console_ਬਹੁਲਕ(h, &ਬਹੁਲਕ) && enc_codepage >= 0
	    && (ਪੂਰਨ)GetConsoleCP() != enc_codepage)
    {
	ਪੂਰਨ	len = (ਪੂਰਨ)STRLEN(str);
	WCHAR	*w = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)str, &len);

	ਜੇ (w != ਨਿਰਾ)
	{
	    WriteConsoleW(h, w, len, &nwrite, ਨਿਰਾ);
	    vim_free(w);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਿਖੋ("%s", str);
    }
}
# endif

/*
 * Give a message.  To be used when the screen hasn't been initialized yet.
 * When there is no tty, collect messages until the GUI has started and they
 * can be displayed in a message box.
 */
    ਖਾਲੀ
mch_ਸੁਨੇਹਾ(ਅੱਖਰ *str)
{
# ਜੇ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    ਜੇ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   ਨਹੀਂਤਾਂ
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.ਵਿੱਚ_use || gui.starting)
#  endif
	    )
    {
	ਲਿਖੋ("%s", str);
	ਵਾਪਸ;
    }
# endif

# ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ਜੇ (!(gui.ਵਿੱਚ_use || gui.starting))
#  endif
    {
	mch_ਸੁਨੇਹਾ_c(str);
	ਵਾਪਸ;
    }
# endif
# ਜੇ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#ਜੇ_ਅੰਤ // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_screen_putchar(ਪੂਰਨ c, ਪੂਰਨ attr)
{
    ਸੁਨੇਹਾ_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ, attr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (cmdmsg_rl)
    {
	ਜੇ (--ਸੁਨੇਹਾ_ਖਾਨਾ == 0)
	{
	    ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ;
	    ++ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	}
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	ਜੇ (++ਸੁਨੇਹਾ_ਖਾਨਾ >= cmdline_ਚੌੜਾਈ)
	{
	    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
	    ++ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_moremsg(ਪੂਰਨ ਭਰਿਆ)
{
    ਪੂਰਨ		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, cmdline_ਖਾਨਾ_off, attr);
    ਜੇ (ਭਰਿਆ)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, cmdline_ਖਾਨਾ_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    ਖਾਲੀ
repeat_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
    ਜੇ (State == ਬਹੁਲਕ_ASKMORE)
    {
	ਸੁਨੇਹਾ_moremsg(TRUE);	// display --more-- message again
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CON_DIALOG
    ਨਹੀਂਤਾਂ ਜੇ (State == ਬਹੁਲਕ_CONFIRM)
    {
	display_confirm_ਸੁਨੇਹਾ();	// display ":confirm" message again
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
    }
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ ਜੇ (State == ਬਹੁਲਕ_EXTERNCMD)
    {
	windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ); // put cursor back
    }
    ਨਹੀਂਤਾਂ ਜੇ (State == ਬਹੁਲਕ_HITRETURN || State == ਬਹੁਲਕ_SETWSIZE)
    {
	ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ == ਪੰਕਤੀਆਂ - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    ਸੁਨੇਹਾ_didout = FALSE;
	    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
	    ਸੁਨੇਹਾ_clr_eos();
	}
	ਪ੍ਰਾਪਤੀ_return_ਸੁਨੇਹਾ();
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੁਨੇਹਾ_check_screen(ਖਾਲੀ)
{
    ਜੇ (!ਭਰਿਆ_screen || !screen_valid(FALSE))
	ਵਾਪਸ FALSE;

    ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ >= ਪੰਕਤੀਆਂ)
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ >= cmdline_ਚੌੜਾਈ)
	ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - 1;
    ਵਾਪਸ TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_clr_eos(ਖਾਲੀ)
{
    ਜੇ (ਸੁਨੇਹਾ_silent == 0)
	ਸੁਨੇਹਾ_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_clr_eos_force(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
    ਜੇ (ਵਿੱਚ_echowindow)
	ਵਾਪਸ;  // messages go into a popup
#ਜੇ_ਅੰਤ
    ਜੇ (ਸੁਨੇਹਾ_use_printf())
    {
	ਜੇ (ਭਰਿਆ_screen)	// only when termcap codes are valid
	{
	    ਜੇ (*T_CD)
		ਬਾਹਰਲਾ_str(T_CD);	// clear to end of display
	    ਨਹੀਂਤਾਂ ਜੇ (*T_CE)
		ਬਾਹਰਲਾ_str(T_CE);	// clear to end of line
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ ਸੁਨੇਹਾ_attr;

	ਸੁਨੇਹਾ_attr = HL_ATTR(HLF_ਸੁਨੇਹਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (cmdmsg_rl)
	{
	    screen_fill(ਸੁਨੇਹਾ_ਪੰਕਤੀ, ਸੁਨੇਹਾ_ਪੰਕਤੀ + 1,
		    cmdline_ਖਾਨਾ_off, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ + 1,
		    ' ', ' ', ਸੁਨੇਹਾ_attr);
	    screen_fill(ਸੁਨੇਹਾ_ਪੰਕਤੀ + 1, (ਪੂਰਨ)ਪੰਕਤੀਆਂ,
		    cmdline_ਖਾਨਾ_off, cmdline_ਖਾਨਾ_off + cmdline_ਚੌੜਾਈ,
		    ' ', ' ', ਸੁਨੇਹਾ_attr);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    screen_fill(ਸੁਨੇਹਾ_ਪੰਕਤੀ, ਸੁਨੇਹਾ_ਪੰਕਤੀ + 1,
		    cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ, cmdline_ਖਾਨਾ_off + cmdline_ਚੌੜਾਈ,
		    ' ', ' ', ਸੁਨੇਹਾ_attr);
	    screen_fill(ਸੁਨੇਹਾ_ਪੰਕਤੀ + 1, (ਪੂਰਨ)ਪੰਕਤੀਆਂ,
		    cmdline_ਖਾਨਾ_off, cmdline_ਖਾਨਾ_off + cmdline_ਚੌੜਾਈ,
		    ' ', ' ', ਸੁਨੇਹਾ_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_clr_cmdline(ਖਾਲੀ)
{
    ਸੁਨੇਹਾ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ;
    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    ਸੁਨੇਹਾ_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    ਪੂਰਨ
ਸੁਨੇਹਾ_ਸਿਰਾ(ਖਾਲੀ)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    ਜੇ (!exiting && need_wait_return && !(State & ਬਹੁਲਕ_CMDLINE))
    {
	wait_return(FALSE);
	ਵਾਪਸ FALSE;
    }
    ਬਾਹਰਲਾ_flush();
    ਵਾਪਸ TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_check(ਖਾਲੀ)
{
    ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ == ਪੰਕਤੀਆਂ - 1 && ਸੁਨੇਹਾ_ਖਾਨਾ >= sc_ਖਾਨਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
		&& !ਵਿੱਚ_echowindow
#ਜੇ_ਅੰਤ
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
    ਸਥਿਰ ਖਾਲੀ
redir_write(char_u *str, ਪੂਰਨ maxlen)
{
    char_u	*s = str;
    ਸਥਿਰ ਪੂਰਨ	cur_ਖਾਨਾ = 0;

    // Don't do anything for displaying prompts and the like.
    ਜੇ (redir_off)
	ਵਾਪਸ;

    // If 'verbosefile' is set prepare for writing in that file.
    ਜੇ (*p_vfile != NUL && verbose_fd == ਨਿਰਾ)
	verbose_open();

    ਜੇ (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	ਜੇ (*s != '\n' && *s != '\r')
	{
	    ਜਦੋਂ (cur_ਖਾਨਾ < ਸੁਨੇਹਾ_ਖਾਨਾ)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਜੇ (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		ਨਹੀਂਤਾਂ ਜੇ (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		ਨਹੀਂਤਾਂ ਜੇ (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    ਜੇ (redir_fd != ਨਿਰਾ)
		    fputs(" ", redir_fd);
		ਜੇ (verbose_fd != ਨਿਰਾ)
		    fputs(" ", verbose_fd);
		++cur_ਖਾਨਾ;
	    }
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (redir_execute)
	    execute_redir_str(s, maxlen);
	ਨਹੀਂਤਾਂ ਜੇ (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	ਨਹੀਂਤਾਂ ਜੇ (redir_vname)
	    var_redir_str(s, maxlen);
#ਜੇ_ਅੰਤ

	// Write and adjust the current column.
	ਜਦੋਂ (*s != NUL && (maxlen < 0 || (ਪੂਰਨ)(s - str) < maxlen))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (!redir_reg && !redir_vname && !redir_execute)
#ਜੇ_ਅੰਤ
		ਜੇ (redir_fd != ਨਿਰਾ)
		    putc(*s, redir_fd);
	    ਜੇ (verbose_fd != ਨਿਰਾ)
		putc(*s, verbose_fd);
	    ਜੇ (*s == '\r' || *s == '\n')
		cur_ਖਾਨਾ = 0;
	    ਨਹੀਂਤਾਂ ਜੇ (*s == '\t')
		cur_ਖਾਨਾ += (8 - cur_ਖਾਨਾ % 8);
	    ਨਹੀਂਤਾਂ
		++cur_ਖਾਨਾ;
	    ++s;
	}

	ਜੇ (ਸੁਨੇਹਾ_silent != 0)	// should update msg_col
	    ਸੁਨੇਹਾ_ਖਾਨਾ = cur_ਖਾਨਾ;
    }
}

    ਪੂਰਨ
redirecting(ਖਾਲੀ)
{
    ਵਾਪਸ redir_fd != ਨਿਰਾ || *p_vfile != NUL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#ਜੇ_ਅੰਤ
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    ਖਾਲੀ
verbose_enter(ਖਾਲੀ)
{
    ਜੇ (*p_vfile != NUL)
	++ਸੁਨੇਹਾ_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    ਖਾਲੀ
verbose_leave(ਖਾਲੀ)
{
    ਜੇ (*p_vfile != NUL)
	ਜੇ (--ਸੁਨੇਹਾ_silent < 0)
	    ਸੁਨੇਹਾ_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    ਖਾਲੀ
verbose_enter_scroll(ਖਾਲੀ)
{
    ਜੇ (*p_vfile != NUL)
	++ਸੁਨੇਹਾ_silent;
    ਨਹੀਂਤਾਂ
	// always scroll up, don't overwrite
	ਸੁਨੇਹਾ_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    ਖਾਲੀ
verbose_leave_scroll(ਖਾਲੀ)
{
    ਜੇ (*p_vfile != NUL)
    {
	ਜੇ (--ਸੁਨੇਹਾ_silent < 0)
	    ਸੁਨੇਹਾ_silent = 0;
    }
    ਨਹੀਂਤਾਂ
	cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    ਖਾਲੀ
verbose_stop(ਖਾਲੀ)
{
    ਜੇ (verbose_fd != ਨਿਰਾ)
    {
	fclose(verbose_fd);
	verbose_fd = ਨਿਰਾ;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    ਪੂਰਨ
verbose_open(ਖਾਲੀ)
{
    ਜੇ (verbose_fd == ਨਿਰਾ && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((ਅੱਖਰ *)p_vfile, "a");
	ਜੇ (verbose_fd == ਨਿਰਾ)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    ਖਾਲੀ
give_warning(char_u *ਸੁਨੇਹਾ_2, ਪੂਰਨ hl)
{
    give_warning_with_ਸਰੋਤ(ਸੁਨੇਹਾ_2, hl, FALSE);
}

    ਖਾਲੀ
give_warning_with_ਸਰੋਤ(char_u *ਸੁਨੇਹਾ_2, ਪੂਰਨ hl, ਪੂਰਨ with_ਸਰੋਤ)
{
    // Don't do this for ":silent".
    ਜੇ (ਸੁਨੇਹਾ_silent != 0)
	ਵਾਪਸ;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸੈੱਟ_vim_var_string(VV_WARNINGMSG, ਸੁਨੇਹਾ_2, -1);
#ਜੇ_ਅੰਤ
    VIM_CLEAR(keep_ਸੁਨੇਹਾ);
    ਜੇ (hl)
	keep_ਸੁਨੇਹਾ_attr = HL_ATTR(HLF_W);
    ਨਹੀਂਤਾਂ
	keep_ਸੁਨੇਹਾ_attr = 0;

    ਜੇ (with_ਸਰੋਤ)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	ਸੁਨੇਹਾ_ਸਰੋਤ(HL_ATTR(HLF_W));
	ਸੁਨੇਹਾ_puts(" ");
	ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)ਸੁਨੇਹਾ_2, HL_ATTR(HLF_W) | ਸੁਨੇਹਾ_HIST);
	ਸੁਨੇਹਾ_clr_eos();
	(ਖਾਲੀ)ਸੁਨੇਹਾ_ਸਿਰਾ();
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਸੁਨੇਹਾ_attr((ਅੱਖਰ *)ਸੁਨੇਹਾ_2, keep_ਸੁਨੇਹਾ_attr) && ਸੁਨੇਹਾ_scrolled == 0)
	ਸੈੱਟ_keep_ਸੁਨੇਹਾ(ਸੁਨੇਹਾ_2, keep_ਸੁਨੇਹਾ_attr);

    ਸੁਨੇਹਾ_didout = FALSE;	    // overwrite this message
    ਸੁਨੇਹਾ_nowait = TRUE;	    // don't wait for this message
    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;

    --no_wait_return;
}

#ਜੇ defined(FEAT_EVAL)
    ਖਾਲੀ
give_warning2(char_u *ਸੁਨੇਹਾ_2, char_u *a1, ਪੂਰਨ hl)
{
    ਜੇ (IObuff == ਨਿਰਾ)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(ਸੁਨੇਹਾ_2, hl);
    }
    ਨਹੀਂਤਾਂ
    {
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, (ਅੱਖਰ *)ਸੁਨੇਹਾ_2, a1);
	give_warning(IObuff, hl);
    }
}
#ਜੇ_ਅੰਤ

/*
 * Advance msg cursor to column "col".
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_advance(ਪੂਰਨ ਖਾਨਾ)
{
    ਜੇ (ਸੁਨੇਹਾ_silent != 0)	// nothing to advance to
    {
	ਸੁਨੇਹਾ_ਖਾਨਾ = ਖਾਨਾ;		// for redirection, may fill it up later
	ਵਾਪਸ;
    }
    ਜੇ (ਖਾਨਾ >= cmdline_ਚੌੜਾਈ)	// not enough room
	ਖਾਨਾ = cmdline_ਚੌੜਾਈ - 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (cmdmsg_rl)
	ਜਦੋਂ (ਸੁਨੇਹਾ_ਖਾਨਾ > cmdline_ਚੌੜਾਈ - ਖਾਨਾ)
	    ਸੁਨੇਹਾ_putchar(' ');
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜਦੋਂ (ਸੁਨੇਹਾ_ਖਾਨਾ < ਖਾਨਾ)
	    ਸੁਨੇਹਾ_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    ਖਾਲੀ
ਸੁਨੇਹਾ_warn_missing_clipboard(ਖਾਲੀ)
{
    ਜੇ (!global_busy && !did_warn_clipboard && silence_w23_w24_ਸੁਨੇਹਾ == 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	ਸੁਨੇਹਾ(_("W23: Clipboard register not available, using register 0"));
#ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ(_("W24: Clipboard register not available. See :h W24"));
#ਜੇ_ਅੰਤ
	did_warn_clipboard = TRUE;
    }
}

#ਜੇ defined(FEAT_CON_DIALOG)
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
    ਪੂਰਨ
do_dialog(
    ਪੂਰਨ		type ਅਣਵਰਤਿਆ,
    char_u	*title ਅਣਵਰਤਿਆ,
    char_u	*ਸੁਨੇਹਾ_2,
    char_u	*buttons,
    ਪੂਰਨ		dfltbutton,
    char_u	*textfield ਅਣਵਰਤਿਆ,	// IObuff for inputdialog(), NULL
					// otherwise
    ਪੂਰਨ		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    ਪੂਰਨ		oldState;
    ਪੂਰਨ		retval = 0;
    char_u	*hotkeys;
    ਪੂਰਨ		c;
    ਪੂਰਨ		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    ਜੇ (silent_ਬਹੁਲਕ)
	ਵਾਪਸ dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    ਜੇ (gui.ਵਿੱਚ_use && vim_strchr(p_go, GO_CONDIALOG) == ਨਿਰਾ)
    {
	// --gui-dialog-file: write text to a file
	ਜੇ (gui_dialog_log(title, ਸੁਨੇਹਾ_2))
	    c = dfltbutton;
	ਨਹੀਂਤਾਂ
	    c = gui_mch_dialog(type, title, ਸੁਨੇਹਾ_2, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	ਬਾਹਰਲਾ_flush();
	gui_mch_update();

	ਵਾਪਸ c;
    }
# endif

    oldState = State;
    State = ਬਹੁਲਕ_CONFIRM;
    setmouse();

    // Ensure raw mode here.
    save_tmode = cur_tmode;
    settmode(TMODE_RAW);

    /*
     * Since we wait for a keypress, don't make the
     * user press RETURN as well afterwards.
     */
    ++no_wait_return;
    hotkeys = ਸੁਨੇਹਾ_show_console_dialog(ਸੁਨੇਹਾ_2, buttons, dfltbutton);

    ਜੇ (hotkeys != ਨਿਰਾ)
    {
	ਲਈ (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    ਚੋਣ (c)
	    {
	    ਹਾਲਤ CAR:		// User accepts default option
	    ਹਾਲਤ NL:
		retval = dfltbutton;
		ਰੋਕੋ;
	    ਹਾਲਤ Ctrl_C:	// User aborts/cancels
	    ਹਾਲਤ ESC:
		retval = 0;
		ਰੋਕੋ;
	    ਮੂਲ:		// Could be a hotkey?
		ਜੇ (c < 0)	// special keys are ignored here
		    ਜਾਰੀ;
		ਜੇ (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    ਰੋਕੋ;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		ਲਈ (i = 0; hotkeys[i]; ++i)
		{
		    ਜੇ (has_mbyte)
		    {
			ਜੇ ((*mb_ptr2char)(hotkeys + i) == c)
			    ਰੋਕੋ;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    ਨਹੀਂਤਾਂ
			ਜੇ (hotkeys[i] == c)
			    ਰੋਕੋ;
		    ++retval;
		}
		ਜੇ (hotkeys[i])
		    ਰੋਕੋ;
		// No hotkey match, so keep waiting
		ਜਾਰੀ;
	    }
	    ਰੋਕੋ;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    ਸੁਨੇਹਾ_ਸਿਰਾ_prompt();

    ਵਾਪਸ retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    ਸਥਿਰ ਪੂਰਨ
copy_char(
    char_u	*from,
    char_u	*ਮੰਜ਼ਿਲ,
    ਪੂਰਨ		lowercase)	// make character lower case
{
    ਪੂਰਨ		len;
    ਪੂਰਨ		c;

    ਜੇ (has_mbyte)
    {
	ਜੇ (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    ਵਾਪਸ (*mb_char2bytes)(c, ਮੰਜ਼ਿਲ);
	}
	ਨਹੀਂਤਾਂ
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(ਮੰਜ਼ਿਲ, from, (size_t)len);
	    ਵਾਪਸ len;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (lowercase)
	    *ਮੰਜ਼ਿਲ = (char_u)TOLOWER_LOC(*from);
	ਨਹੀਂਤਾਂ
	    *ਮੰਜ਼ਿਲ = *from;
	ਵਾਪਸ 1;
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
    ਸਥਿਰ char_u *
ਸੁਨੇਹਾ_show_console_dialog(
    char_u	*ਸੁਨੇਹਾ_2,
    char_u	*buttons,
    ਪੂਰਨ		dfltbutton)
{
    ਪੂਰਨ		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    ਪੂਰਨ		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = ਨਿਰਾ;
    char_u	*msgp = ਨਿਰਾ;
    char_u	*hotkp = ਨਿਰਾ;
    char_u	*r;
    ਪੂਰਨ		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    ਪੂਰਨ		ਪਹਿਲਾ_hotkey = FALSE;	// first char of button is hotkey
    ਪੂਰਨ		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    ਲਈ (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	ਜਦੋਂ (*r)
	{
	    ਜੇ (*r == DLG_BUTTON_SEP)
	    {
		ਜੇ (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    ਜੇ (has_mbyte)
			hotkp += STRLEN(hotkp);
		    ਨਹੀਂਤਾਂ
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    ਜੇ (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    ਜੇ (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			ਪਹਿਲਾ_hotkey = TRUE;
		}
		ਨਹੀਂਤਾਂ
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    ਜੇ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*r == DLG_HOTKEY_CHAR || ਪਹਿਲਾ_hotkey)
	    {
		ਜੇ (*r == DLG_HOTKEY_CHAR)
		    ++r;
		ਪਹਿਲਾ_hotkey = FALSE;
		ਜੇ (copy)
		{
		    ਜੇ (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    ਨਹੀਂਤਾਂ
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ++len;	    // '&a' -> '[a]'
		    ਜੇ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// everything else copy literally
		ਜੇ (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	ਜੇ (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	ਨਹੀਂਤਾਂ
	{
	    len += (ਪੂਰਨ)(STRLEN(ਸੁਨੇਹਾ_2)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    ਜੇ (!has_hotkey[0])
	    {
		ਪਹਿਲਾ_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_ਸੁਨੇਹਾ);
	    confirm_ਸੁਨੇਹਾ = alloc(len);
	    ਜੇ (confirm_ਸੁਨੇਹਾ == ਨਿਰਾ)
		ਵਾਪਸ ਨਿਰਾ;
	    *confirm_ਸੁਨੇਹਾ = NUL;
	    hotk = alloc(lenhotkey);
	    ਜੇ (hotk == ਨਿਰਾ)
		ਵਾਪਸ ਨਿਰਾ;

	    *confirm_ਸੁਨੇਹਾ = '\n';
	    STRCPY(confirm_ਸੁਨੇਹਾ + 1, ਸੁਨੇਹਾ_2);

	    msgp = confirm_ਸੁਨੇਹਾ + 1 + STRLEN(ਸੁਨੇਹਾ_2);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_ਸੁਨੇਹਾ_ਪੂਛ = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_ਸੁਨੇਹਾ();
    ਵਾਪਸ hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    ਸਥਿਰ ਖਾਲੀ
display_confirm_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_ਸੁਨੇਹਾ_ਵਰਤਿਆ;
    ਜੇ (confirm_ਸੁਨੇਹਾ != ਨਿਰਾ)
	ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)confirm_ਸੁਨੇਹਾ, HL_ATTR(HLF_M));
    --confirm_ਸੁਨੇਹਾ_ਵਰਤਿਆ;
}

#ਜੇ_ਅੰਤ // FEAT_CON_DIALOG

#ਜੇ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    ਪੂਰਨ
vim_dialog_yesno(
    ਪੂਰਨ		type,
    char_u	*title,
    char_u	*ਸੁਨੇਹਾ_2,
    ਪੂਰਨ		dflt)
{
    ਜੇ (do_dialog(type,
		title == ਨਿਰਾ ? (char_u *)_("Question") : title,
		ਸੁਨੇਹਾ_2,
		(char_u *)_("&Yes\n&No"), dflt, ਨਿਰਾ, FALSE) == 1)
	ਵਾਪਸ VIM_YES;
    ਵਾਪਸ VIM_NO;
}

    ਪੂਰਨ
vim_dialog_yesnocancel(
    ਪੂਰਨ		type,
    char_u	*title,
    char_u	*ਸੁਨੇਹਾ_2,
    ਪੂਰਨ		dflt)
{
    ਚੋਣ (do_dialog(type,
		title == ਨਿਰਾ ? (char_u *)_("Question") : title,
		ਸੁਨੇਹਾ_2,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, ਨਿਰਾ, FALSE))
    {
	ਹਾਲਤ 1: ਵਾਪਸ VIM_YES;
	ਹਾਲਤ 2: ਵਾਪਸ VIM_NO;
    }
    ਵਾਪਸ VIM_CANCEL;
}

    ਪੂਰਨ
vim_dialog_yesnoallcancel(
    ਪੂਰਨ		type,
    char_u	*title,
    char_u	*ਸੁਨੇਹਾ_2,
    ਪੂਰਨ		dflt)
{
    ਚੋਣ (do_dialog(type,
		title == ਨਿਰਾ ? (char_u *)"Question" : title,
		ਸੁਨੇਹਾ_2,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, ਨਿਰਾ, FALSE))
    {
	ਹਾਲਤ 1: ਵਾਪਸ VIM_YES;
	ਹਾਲਤ 2: ਵਾਪਸ VIM_NO;
	ਹਾਲਤ 3: ਵਾਪਸ VIM_ALL;
	ਹਾਲਤ 4: ਵਾਪਸ VIM_DISCARDALL;
    }
    ਵਾਪਸ VIM_CANCEL;
}

#ਜੇ_ਅੰਤ // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
