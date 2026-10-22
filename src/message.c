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

#ଅନ୍ତର୍ଭାଵ "vim.h"

ସ୍ଥିର ଶୂନ୍ୟ add_ସନ୍ଦେଶ_hist(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr);
ସ୍ଥିର ଶୂନ୍ୟ check_ସନ୍ଦେଶ_hist(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ hit_return_ସନ୍ଦେଶ(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ସନ୍ଦେଶ_home_replace_attr(char_u *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr);
ସ୍ଥିର ଶୂନ୍ୟ ସନ୍ଦେଶ_puts_attr_len(ଅକ୍ଷର *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr);
ସ୍ଥିର ଶୂନ୍ୟ ସନ୍ଦେଶ_puts_display(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା recurse);
ସ୍ଥିର ଶୂନ୍ୟ ସନ୍ଦେଶ_scroll_up(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ inc_ସନ୍ଦେଶ_scrolled(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ store_sb_text(char_u **sb_str, char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *sb_col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା finish);
ସ୍ଥିର ଶୂନ୍ୟ t_puts(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *t_col, char_u *t_s, char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr);
ସ୍ଥିର ଶୂନ୍ୟ ସନ୍ଦେଶ_puts_printf(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlen);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_more_prompt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା typed_char);
ସ୍ଥିର ଶୂନ୍ୟ ସନ୍ଦେଶ_screen_putchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr);
ସ୍ଥିର ଶୂନ୍ୟ ସନ୍ଦେଶ_moremsg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା full);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ସନ୍ଦେଶ_check_screen(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ redir_write(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlen);
#ifdef FEAT_CON_DIALOG
ସ୍ଥିର char_u *ସନ୍ଦେଶ_show_console_dialog(char_u *message, char_u *buttons, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dfltbutton);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	confirm_ସନ୍ଦେଶ_used = FALSE;	// displaying confirm_msg
ସ୍ଥିର char_u	*confirm_ସନ୍ଦେଶ = ଖାଲି;		// ":confirm" message
ସ୍ଥିର char_u	*confirm_ସନ୍ଦେଶ_tail;		// tail of confirm_msg
ସ୍ଥିର ଶୂନ୍ୟ display_confirm_ସନ୍ଦେଶ(ଶୂନ୍ୟ);
#endif
#ifdef FEAT_EVAL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା emsg_to_channel_log = FALSE;
#endif

ସଂରଚନା ସନ୍ଦେଶ_hist
{
    ସଂରଚନା ସନ୍ଦେଶ_hist	*next;
    char_u		*ସନ୍ଦେଶ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			attr;
};

ସ୍ଥିର ସଂରଚନା ସନ୍ଦେଶ_hist *first_ସନ୍ଦେଶ_hist = ଖାଲି;
ସ୍ଥିର ସଂରଚନା ସନ୍ଦେଶ_hist *last_ସନ୍ଦେଶ_hist = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_hist_len = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#define MESSAGES_HIT_ENTER	0x001
#define MESSAGES_WAIT		0x002
#define MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#define MESSAGES_OPT_HIT_ENTER "hit-enter"
#define MESSAGES_OPT_WAIT "wait:"
#define MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_flags = MESSAGES_HIT_ENTER | MESSAGES_HISTORY;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_wait = 0;

ସ୍ଥିର FILE *verbose_fd = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  verbose_did_open = FALSE;

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଦେଶ(ଅକ୍ଷର *s)
{
    ଫେରନ୍ତୁ ସନ୍ଦେଶ_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
verb_ସନ୍ଦେଶ(ଅକ୍ଷର *s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    verbose_enter();
    n = ସନ୍ଦେଶ_attr_keep(s, 0, FALSE);
    verbose_leave();

    ଫେରନ୍ତୁ n;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଦେଶ_attr(ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ଫେରନ୍ତୁ ସନ୍ଦେଶ_attr_keep(s, attr, FALSE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଦେଶ_attr_keep(
    ଅକ୍ଷର	*s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	entered = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
    char_u	*buf = ଖାଲି;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    ଯଦି (!emsg_on_display && message_filtered((char_u *)s))
	ଫେରନ୍ତୁ TRUE;

#ifdef FEAT_EVAL
    ଯଦି (attr == 0)
	set_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#endif

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    ଯଦି (entered >= 3)
	ଫେରନ୍ତୁ TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    ଯଦି ((char_u *)s != keep_ସନ୍ଦେଶ
	    || (*s != '<'
		&& last_ସନ୍ଦେଶ_hist != ଖାଲି
		&& last_ସନ୍ଦେଶ_hist->ସନ୍ଦେଶ != ଖାଲି
		&& STRCMP(s, last_ସନ୍ଦେଶ_hist->ସନ୍ଦେଶ) != 0))
	add_ସନ୍ଦେଶ_hist((char_u *)s, -1, attr);

#ifdef FEAT_EVAL
    ଯଦି (emsg_to_channel_log)
	// Write message in the channel log.
	ch_log(ଖାଲି, "ERROR: %s", s);
#endif

    // Truncate the message if needed.
    ସନ୍ଦେଶ_start();
    buf = ସନ୍ଦେଶ_strtrunc((char_u *)s, FALSE);
    ଯଦି (buf != ଖାଲି)
	s = (ଅକ୍ଷର *)buf;

    ସନ୍ଦେଶ_outtrans_attr((char_u *)s, attr);
    ସନ୍ଦେଶ_clr_eos();
    retval = ସନ୍ଦେଶ_end();

    ଯଦି (keep && retval && vim_strsize((char_u *)s)
		    < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(Rows - cmdline_row - 1) * cmdline_width + sc_col)
	set_keep_ସନ୍ଦେଶ((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    ଫେରନ୍ତୁ retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
ସନ୍ଦେଶ_strtrunc(
    char_u	*s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force)	    // always truncate
{
    char_u	*buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room;

    // May truncate message to avoid a hit-return prompt
    ଯଦି ((!ସନ୍ଦେଶ_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_active && ସନ୍ଦେଶ_silent == 0) || force)
    {
	len = vim_strsize(s);
	ଯଦି (ସନ୍ଦେଶ_scrolled != 0
#ifdef HAS_MESSAGE_WINDOW
		|| in_echowindow
#endif
		)
	    // Use all the columns.
	    room = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(Rows - ସନ୍ଦେଶ_row) * cmdline_width - 1;
	ଅନ୍ୟଥା
	    // Use up to 'showcmd' column.
	    room = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(Rows - ସନ୍ଦେଶ_row - 1) * cmdline_width + sc_col - 1;
	ଯଦି (len > room && room > 0)
	{
	    ଯଦି (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (room + 2) * 18;
	    ଅନ୍ୟଥା ଯଦି (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (room + 2) * 2;
	    ଅନ୍ୟଥା
		len = room + 2;
	    buf = alloc(len);
	    ଯଦି (buf != ଖାଲି)
		trunc_string(s, buf, room, len);
	}
    }
    ଫେରନ୍ତୁ buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    ଶୂନ୍ୟ
trunc_string(
    char_u	*s,
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room_in,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflen)
{
    size_t	room = room_in - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		e;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    ଯଦି (*s == NUL)
    {
	ଯଦି (buflen > 0)
	    *buf = NUL;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (room_in < 3)
	room = 0;
    half = room / 2;

    // First part: Start of the string.
    ପାଇଁ (e = 0; len < half && e < buflen; ++e)
    {
	ଯଦି (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    ଫେରନ୍ତୁ;
	}
	n = ptr2cells(s + e);
	ଯଦି (len + n > half)
	    ଭାଙ୍ଗ;
	len += n;
	buf[e] = s[e];
	ଯଦି (has_mbyte)
	    ପାଇଁ (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		ଯଦି (++e == buflen)
		    ଭାଙ୍ଗ;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    ଯଦି (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	ଯେତେବେଳେ (len + n > room)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    ଅନ୍ୟଥା ଯଦି (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s);
	ପାଇଁ (;;)
	{
	    କର
		half = half - utf_head_off(s, s + half - 1) - 1;
	    ଯେତେବେଳେ (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    ଯଦି (len + n > room || half == 0)
		ଭାଙ୍ଗ;
	    len += n;
	    i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)half;
	}
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= room; --i)
	    len += n;
    }


    ଯଦି (i <= e + 3)
    {
	// text fits without truncating
	ଯଦି (s != buf)
	{
	    len = STRLEN(s);
	    ଯଦି (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    ଯଦି (len < 1)
		buf[e - 1] = NUL;
	    ଅନ୍ୟଥା
		mch_memmove(buf + e, s + e, len);
	}
    }
    ଅନ୍ୟଥା ଯଦି (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	ଯଦି (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    ଅନ୍ୟଥା
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim_snprintf(ଅକ୍ଷର *str, size_t str_m, ଅଚଳ ଅକ୍ଷର *fmt, ...);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
smsg(ଅଚଳ ଅକ୍ଷର *s, ...)
{
    ଯଦି (IObuff == ଖାଲି)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ଫେରନ୍ତୁ ସନ୍ଦେଶ((ଅକ୍ଷର *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ଅକ୍ଷର *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ଫେରନ୍ତୁ ସନ୍ଦେଶ((ଅକ୍ଷର *)IObuff);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
smsg_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr, ଅଚଳ ଅକ୍ଷର *s, ...)
{
    ଯଦି (IObuff == ଖାଲି)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ଫେରନ୍ତୁ ସନ୍ଦେଶ_attr((ଅକ୍ଷର *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ଅକ୍ଷର *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ଫେରନ୍ତୁ ସନ୍ଦେଶ_attr((ଅକ୍ଷର *)IObuff, attr);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
smsg_attr_keep(ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr, ଅଚଳ ଅକ୍ଷର *s, ...)
{
    ଯଦି (IObuff == ଖାଲି)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ଫେରନ୍ତୁ ସନ୍ଦେଶ_attr_keep((ଅକ୍ଷର *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ଅକ୍ଷର *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ଫେରନ୍ତୁ ସନ୍ଦେଶ_attr_keep((ଅକ୍ଷର *)IObuff, attr, TRUE);
}

#endif

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	last_sourcing_lnum = 0;
ସ୍ଥିର char_u   *last_sourcing_name = ଖାଲି;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    ଶୂନ୍ୟ
reset_last_sourcing(ଶୂନ୍ୟ)
{
    VIM_CLEAR(last_sourcing_name);
    last_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
other_sourcing_name(ଶୂନ୍ୟ)
{
    ଯଦି (HAVE_SOURCING_INFO && SOURCING_NAME != ଖାଲି)
    {
	ଯଦି (last_sourcing_name != ଖାଲି)
	    ଫେରନ୍ତୁ STRCMP(SOURCING_NAME, last_sourcing_name) != 0;
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ସ୍ଥିର char_u *
get_emsg_source(ଶୂନ୍ୟ)
{
    char_u	*Buf, *p;

    ଯଦି (HAVE_SOURCING_INFO && SOURCING_NAME != ଖାଲି && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	ଯଦି (sname == ଖାଲି)
	    sname = SOURCING_NAME;

#ifdef FEAT_EVAL
	ଯଦି (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	ଅନ୍ୟଥା
#endif
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	ଯଦି (Buf != ଖାଲି)
	    sprintf((ଅକ୍ଷର *)Buf, (ଅକ୍ଷର *)p, sname);
	vim_free(tofree);
	ଫେରନ୍ତୁ Buf;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ସ୍ଥିର char_u *
get_emsg_lnum(ଶୂନ୍ୟ)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    ଯଦି (SOURCING_NAME != ଖାଲି
	    && (other_sourcing_name() || SOURCING_LNUM != last_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	ଯଦି (Buf != ଖାଲି)
	    sprintf((ଅକ୍ଷର *)Buf, (ଅକ୍ଷର *)p, (long)SOURCING_LNUM);
	ଫେରନ୍ତୁ Buf;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_source(ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    char_u	*p;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = FALSE;

    // Bail out if something called here causes an error.
    ଯଦି (recursive)
	ଫେରନ୍ତୁ;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_source();
    ଯଦି (p != ଖାଲି)
    {
	ସନ୍ଦେଶ_scroll = TRUE;  // this will take more than one line
	ସନ୍ଦେଶ_attr((ଅକ୍ଷର *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    ଯଦି (p != ଖାଲି)
    {
	ସନ୍ଦେଶ_attr((ଅକ୍ଷର *)p, HL_ATTR(HLF_N));
	vim_free(p);
	last_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    ଯଦି (SOURCING_NAME == ଖାଲି || other_sourcing_name())
    {
	VIM_CLEAR(last_sourcing_name);
	ଯଦି (SOURCING_NAME != ଖାଲି)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
emsg_not_now(ଶୂନ୍ୟ)
{
    ଯଦି ((emsg_off > 0 && vim_strchr(p_debug, 'm') == ଖାଲି
					  && vim_strchr(p_debug, 't') == ଖାଲି)
#ifdef FEAT_EVAL
	    || emsg_skip > 0
#endif
	    )
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

#ଯଦି defined(FEAT_EVAL)
ସ୍ଥିର garray_T ignore_error_list = GA_EMPTY;

    ଶୂନ୍ୟ
ignore_error_for_testing(char_u *error)
{
    ଯଦି (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, ଆକାର(char_u *), 1);

    ଯଦି (STRCMP("RESET", error) == 0)
	ga_clear_strings(&ignore_error_list);
    ଅନ୍ୟଥା
	ga_copy_string(&ignore_error_list, error);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ignore_error(ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < ignore_error_list.ga_len; ++i)
	ଯଦି (strstr(ସନ୍ଦେଶ,
		  (ଅକ୍ଷର *)((char_u **)(ignore_error_list.ga_data))[i]) != ଖାଲି)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}
#endif

#ଯଦି !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    ଶୂନ୍ୟ
do_perror(ଅକ୍ଷର *ସନ୍ଦେଶ)
{
    perror(ସନ୍ଦେଶ);
    ++emsg_silent;
    emsg(ସନ୍ଦେଶ);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
emsg_core(ଅଚଳ ଅକ୍ଷର *s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ignore = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		severe;
#endif

#ifdef FEAT_EVAL
    // When testing some errors are turned into a normal message.
    ଯଦି (ignore_error(s))
	// don't call msg() if it results in a dialog
	ଫେରନ୍ତୁ ସନ୍ଦେଶ_use_printf() ? FALSE : ସନ୍ଦେଶ((ଅକ୍ଷର *)s);
#endif

    ++called_emsg;

#ifdef FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#endif

    ଯଦି (!emsg_off || vim_strchr(p_debug, 't') != ଖାଲି)
    {
#ifdef FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	ଯଦି (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    ଯଦି (!ignore)
		++did_emsg;
	    ଫେରନ୍ତୁ TRUE;
	}

	ଯଦି (in_assert_fails && emsg_assert_fails_ସନ୍ଦେଶ == ଖାଲି)
	{
	    emsg_assert_fails_ସନ୍ଦେଶ = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == ଖାଲି ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#endif

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	ଯଦି (emsg_silent != 0)
	{
#ifdef FEAT_EVAL
	    ++did_emsg_silent;
#endif
	    ଯଦି (emsg_noredir == 0)
	    {
		ସନ୍ଦେଶ_start();
		p = get_emsg_source();
		ଯଦି (p != ଖାଲି)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		ଯଦି (p != ଖାଲି)
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
	    ଯଦି (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#endif
#ifdef FEAT_EVAL
	    ch_log(ଖାଲି, "ERROR silent: %s", s);
#endif
	    ଫେରନ୍ତୁ TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	ସନ୍ଦେଶ_silent = 0;
	cmd_silent = FALSE;

	ଯଦି (global_busy)		// break :global command
	    ++global_busy;

	ଯଦି (p_eb)
	    beep_flush();		// also includes flush_buffers()
	ଅନ୍ୟଥା
	    flush_buffers(FLUSH_MINIMAL);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ifdef FEAT_EVAL
	++uncaught_emsg;
#endif
    }

#ifdef HAS_MESSAGE_WINDOW
    ଯଦି (!in_echowindow)
#endif
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    ଯଦି (ସନ୍ଦେଶ_scrolled != 0)
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
    ସନ୍ଦେଶ_scroll = TRUE;
    ସନ୍ଦେଶ_source(attr);

    /*
     * Display the error message itself.
     */
    ସନ୍ଦେଶ_nowait = FALSE;			// wait for this msg
    r = ସନ୍ଦେଶ_attr((ଅକ୍ଷର *)s, attr);

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = FALSE;
#endif
    ଫେରନ୍ତୁ r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
emsg(ଅକ୍ଷର *s)
{
    // Skip this if not giving error messages at the moment.
    ଯଦି (emsg_not_now())
	ଫେରନ୍ତୁ TRUE;

    ଫେରନ୍ତୁ emsg_core(s);
}

#ifndef PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
semsg(ଅଚଳ ଅକ୍ଷର *s, ...)
{
    // Skip this if not giving error messages at the moment.
    ଯଦି (emsg_not_now())
	ଫେରନ୍ତୁ TRUE;

    ଯଦି (IObuff == ଖାଲି)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ଫେରନ୍ତୁ emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((ଅକ୍ଷର *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    ଫେରନ୍ତୁ emsg_core((ଅକ୍ଷର *)IObuff);
}
#endif

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    ଶୂନ୍ୟ
iemsg(ଅକ୍ଷର *s)
{
    ଯଦି (emsg_not_now())
	ଫେରନ୍ତୁ;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    emsg_core(s);
#ଯଦି defined(ABORT_ON_INTERNAL_ERROR) && defined(FEAT_EVAL)
    set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    ସନ୍ଦେଶ_putchar('\n');  // avoid overwriting the error message
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
    ଶୂନ୍ୟ
siemsg(ଅଚଳ ଅକ୍ଷର *s, ...)
{
    ଯଦି (emsg_not_now())
	ଫେରନ୍ତୁ;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    ଯଦି (IObuff == ଖାଲି)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    ଅନ୍ୟଥା
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((ଅକ୍ଷର *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((ଅକ୍ଷର *)IObuff);
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    ସନ୍ଦେଶ_putchar('\n');  // avoid overwriting the error message
    out_flush();
    abort();
# endif
}
#endif

/*
 * Give an "Internal error" message.
 */
    ଶୂନ୍ୟ
internal_error(ଅକ୍ଷର *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    siemsg(_(e_internal_error_str), where);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    ଶୂନ୍ୟ
internal_error_no_abort(ଅକ୍ଷର *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    semsg(_(e_internal_error_str), where);
}
#endif

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    ଶୂନ୍ୟ
emsg_invreg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା name)
{
    semsg(_(e_invalid_register_name_str), transchar_buf(ଖାଲି, name));
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    ଶୂନ୍ୟ
emsg_namelen(ଅକ୍ଷର *ସନ୍ଦେଶ, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(ସନ୍ଦେଶ, copy == ଖାଲି ? "NULL" : (ଅକ୍ଷର *)copy);
    vim_free(copy);
}
#endif

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    ଅକ୍ଷର *
ସନ୍ଦେଶ_trunc_attr(ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ଅକ୍ଷର	*ts;

    // Add message to history before truncating
    add_ସନ୍ଦେଶ_hist((char_u *)s, -1, attr);

    ts = (ଅକ୍ଷର *)ସନ୍ଦେଶ_may_trunc(force, (char_u *)s);

    ସନ୍ଦେଶ_hist_off = TRUE;
    n = ସନ୍ଦେଶ_attr(ts, attr);
    ସନ୍ଦେଶ_hist_off = FALSE;

    ଯଦି (n)
	ଫେରନ୍ତୁ ts;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
ସନ୍ଦେଶ_may_trunc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା force, char_u *s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    room = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(Rows - cmdline_row - 1) * cmdline_width + sc_col - 1;
    ଯଦି (room > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_active))
	    && (n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s) - room) > 0)
    {
	ଯଦି (has_mbyte)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	size = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    ଯଦି (size <= room)
		ଫେରନ୍ତୁ s;

	    ପାଇଁ (n = 0; size >= room; )
	    {
		size -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    ଫେରନ୍ତୁ s;
}

    ସ୍ଥିର ଶୂନ୍ୟ
add_ସନ୍ଦେଶ_hist(
    char_u	*s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,		// -1 for undetermined length
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr)
{
    ସଂରଚନା ସନ୍ଦେଶ_hist *p;

    ଯଦି (ସନ୍ଦେଶ_hist_off || ସନ୍ଦେଶ_silent != 0)
	ଫେରନ୍ତୁ;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(ସଂରଚନା ସନ୍ଦେଶ_hist);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (len < 0)
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s);
    // remove leading and trailing newlines
    ଯେତେବେଳେ (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    ଯେତେବେଳେ (len > 0 && s[len - 1] == '\n')
	--len;
    p->ସନ୍ଦେଶ = vim_strnsave(s, len);
    p->next = ଖାଲି;
    p->attr = attr;
    ଯଦି (last_ସନ୍ଦେଶ_hist != ଖାଲି)
	last_ସନ୍ଦେଶ_hist->next = p;
    last_ସନ୍ଦେଶ_hist = p;
    ଯଦି (first_ସନ୍ଦେଶ_hist == ଖାଲି)
	first_ସନ୍ଦେଶ_hist = last_ସନ୍ଦେଶ_hist;
    ++ସନ୍ଦେଶ_hist_len;

    check_ସନ୍ଦେଶ_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
delete_first_ସନ୍ଦେଶ(ଶୂନ୍ୟ)
{
    ସଂରଚନା ସନ୍ଦେଶ_hist *p;

    ଯଦି (ସନ୍ଦେଶ_hist_len <= 0)
	ଫେରନ୍ତୁ FAIL;
    p = first_ସନ୍ଦେଶ_hist;
    first_ସନ୍ଦେଶ_hist = p->next;
    ଯଦି (first_ସନ୍ଦେଶ_hist == ଖାଲି)
	last_ସନ୍ଦେଶ_hist = ଖାଲି;  // history is empty
    vim_free(p->ସନ୍ଦେଶ);
    vim_free(p);
    --ସନ୍ଦେଶ_hist_len;
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ଶୂନ୍ୟ
check_ସନ୍ଦେଶ_hist(ଶୂନ୍ୟ)
{
    // Don't let the message history get too big
    ଯେତେବେଳେ (ସନ୍ଦେଶ_hist_len > 0 && ସନ୍ଦେଶ_hist_len > ସନ୍ଦେଶ_hist_max)
	(ଶୂନ୍ୟ)delete_first_ସନ୍ଦେଶ();
}


    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
messagesopt_changed(ଶୂନ୍ୟ)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		messages_flags_new = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		messages_wait_new = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		messages_history_new = 0;

    p = p_mopt;
    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (STRNCMP(p, MESSAGES_OPT_HIT_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER);
	    messages_flags_new |= MESSAGES_HIT_ENTER;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	ଯଦି (*p != ',' && *p != NUL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    ଯଦି (!(messages_flags_new & (MESSAGES_HIT_ENTER | MESSAGES_WAIT)))
	ଫେରନ୍ତୁ FAIL;

    // "history" must be set
    ଯଦି (!(messages_flags_new & MESSAGES_HISTORY))
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (messages_history_new < 0 || messages_history_new > 10000)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (messages_wait_new < 0 || messages_wait_new > 10000)
	ଫେରନ୍ତୁ FAIL;

    ସନ୍ଦେଶ_flags = messages_flags_new;
    ସନ୍ଦେଶ_wait = messages_wait_new;

    ସନ୍ଦେଶ_hist_max = messages_history_new;
    check_ସନ୍ଦେଶ_hist();

    ଫେରନ୍ତୁ OK;
}

/*
 * ":messages" command.
 */
    ଶୂନ୍ୟ
ex_messages(exarg_T *eap)
{
    ସଂରଚନା ସନ୍ଦେଶ_hist *p;
    char_u	    *s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    c = 0;

    ଯଦି (STRCMP(eap->arg, "clear") == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା keep = eap->addr_ଗଣନା == 0 ? 0 : eap->line2;

	ଯେତେବେଳେ (ସନ୍ଦେଶ_hist_len > keep)
	    (ଶୂନ୍ୟ)delete_first_ସନ୍ଦେଶ();
	ଫେରନ୍ତୁ;
    }

    ଯଦି (*eap->arg != NUL)
    {
	emsg(_(e_invalid_argument));
	ଫେରନ୍ତୁ;
    }

    ସନ୍ଦେଶ_hist_off = TRUE;

    p = first_ସନ୍ଦେଶ_hist;
    ଯଦି (eap->addr_ଗଣନା != 0)
    {
	// Count total messages
	ପାଇଁ (; p != ଖାଲି && !got_int; p = p->next)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	ପାଇଁ (p = first_ସନ୍ଦେଶ_hist; p != ଖାଲି && !got_int && c > 0;
						    p = p->next, c--);
    }

    ଯଦି (p == first_ସନ୍ଦେଶ_hist)
    {
#ifdef FEAT_MULTI_LANG
	s = get_mess_lang();
#ଅନ୍ୟଥା
	s = mch_getenv((char_u *)"LANG");
#endif
	ଯଦି (s != ଖାଲି && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    ସନ୍ଦେଶ_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    ପାଇଁ (; p != ଖାଲି && !got_int; p = p->next)
	ଯଦି (p->ସନ୍ଦେଶ != ଖାଲି)
	    ସନ୍ଦେଶ_attr((ଅକ୍ଷର *)p->ସନ୍ଦେଶ, p->attr);

    ସନ୍ଦେଶ_hist_off = FALSE;
}

#ଯଦି defined(FEAT_CON_DIALOG) || defined(FIND_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_end_prompt(ଶୂନ୍ୟ)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_row = ସନ୍ଦେଶ_row;
    ସନ୍ଦେଶ_col = 0;
    ସନ୍ଦେଶ_clr_eos();
    ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = -1;
}
#endif

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    ଶୂନ୍ୟ
wait_return(ପୂର୍ଣ୍ଣସଂଖ୍ୟା redraw)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oldState;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tmpState;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_got_int;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_reg_recording;
    FILE	*save_scriptout;

    ଯଦି (redraw == TRUE)
	set_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    ଯଦି (ସନ୍ଦେଶ_silent != 0)
	ଫେରନ୍ତୁ;
#ifdef HAS_MESSAGE_WINDOW
    ଯଦି (in_echowindow)
	ଫେରନ୍ତୁ;
#endif

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    ଯଦି (vgetc_busy > 0)
	ଫେରନ୍ତୁ;
    need_wait_return = TRUE;
    ଯଦି (no_wait_return)
    {
	ଯଦି (!exmode_active)
	    cmdline_row = ସନ୍ଦେଶ_row;
	ଫେରନ୍ତୁ;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    ଯଦି (quit_more)
    {
	c = CAR;		// just pretend CR was hit
	quit_more = FALSE;
	got_int = FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (exmode_active)
    {
	ସନ୍ଦେଶ_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (!stuff_empty())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    ଅନ୍ୟଥା
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = MODE_HITRETURN;
	setmouse();
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#endif
	cmdline_row = ସନ୍ଦେଶ_row;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	ଯଦି (need_check_timestamps)
	    check_timestamps(FALSE);

	ଯଦି (ସନ୍ଦେଶ_flags & MESSAGES_HIT_ENTER)
	{
	    hit_return_ସନ୍ଦେଶ();

	    କର
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
		++allow_ଚାବିଗୁଡ଼ିକ;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = ଖାଲି;
		c = safe_vgetc();
		ଯଦି (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_ଚାବିଗୁଡ଼ିକ;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ifdef FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		ଯଦି (ଚାବି_Typed && c == Ctrl_Y && clip_star.state == SELECT_DONE)
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
		ଯଦି (ଚାବି_Typed && p_more && !p_cp)
		{
		    ଯଦି (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_UP || c == K_PAGEUP)
		    {
			ଯଦି (ସନ୍ଦେଶ_scrolled > Rows)
			    // scroll back to show older messages
			    do_more_prompt(c);
			ଅନ୍ୟଥା
			{
			    ସନ୍ଦେଶ_didout = FALSE;
			    c = K_IGNORE;
			    ସନ୍ଦେଶ_col =
#ifdef FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_width - 1 :
#endif
				0;
			}
			ଯଦି (quit_more)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_more = FALSE;
			    got_int = FALSE;
			}
			ଅନ୍ୟଥା ଯଦି (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    hit_return_ସନ୍ଦେଶ();
			}
		    }
		    ଅନ୍ୟଥା ଯଦି (ସନ୍ଦେଶ_scrolled > Rows - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_DOWN || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } ଯେତେବେଳେ ((had_got_int && c == Ctrl_C)
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
				    && mouse_row < ସନ୍ଦେଶ_row
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    ଯଦି (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(ଶୂନ୍ୟ)jump_to_mouse(MOUSE_SETPOS, ଖାଲି, 0);
	    ଅନ୍ୟଥା ଯଦି (!ଚାବି_Typed || (vim_strchr((char_u *)"\r\n ", c) == ଖାଲି
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_mask);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	ଅନ୍ୟଥା
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(ସନ୍ଦେଶ_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    ଯଦି (c == ':' || c == '?' || c == '/')
    {
	ଯଦି (!exmode_active)
	    cmdline_row = ସନ୍ଦେଶ_row;
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
    ସନ୍ଦେଶ_check();

#ifndef MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    ଯଦି (swapping_screen() && !termcap_active)
	newline_on_exit = TRUE;
#endif

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = -1;		// reset lines_left at next msg_start()
    reset_last_sourcing();
    ଯଦି (keep_ସନ୍ଦେଶ != ଖାଲି && vim_strsize(keep_ସନ୍ଦେଶ) >=
			    (Rows - cmdline_row - 1) * cmdline_width + sc_col)
	VIM_CLEAR(keep_ସନ୍ଦେଶ);	    // don't redisplay message, it's too long

    ଯଦି (tmpState == MODE_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    ଅନ୍ୟଥା ଯଦି (!skip_redraw
	    && (redraw == TRUE || (ସନ୍ଦେଶ_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
hit_return_ସନ୍ଦେଶ(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_more = p_more;

    p_more = FALSE;	// don't want to see this message when scrolling back
    ଯଦି (ସନ୍ଦେଶ_didout)	// start on a new line
	ସନ୍ଦେଶ_putchar('\n');
    ଯଦି (got_int)
	ସନ୍ଦେଶ_puts(_("Interrupt: "));

    ସନ୍ଦେଶ_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    ଯଦି (!ସନ୍ଦେଶ_use_printf())
	ସନ୍ଦେଶ_clr_eos();
    p_more = save_p_more;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    ଶୂନ୍ୟ
set_keep_ସନ୍ଦେଶ(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    vim_free(keep_ସନ୍ଦେଶ);
    ଯଦି (s != ଖାଲି && ସନ୍ଦେଶ_silent == 0)
	keep_ସନ୍ଦେଶ = vim_strsave(s);
    ଅନ୍ୟଥା
	keep_ସନ୍ଦେଶ = ଖାଲି;
    keep_ସନ୍ଦେଶ_more = FALSE;
    keep_ସନ୍ଦେଶ_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    ଶୂନ୍ୟ
set_keep_ସନ୍ଦେଶ_from_hist(ଶୂନ୍ୟ)
{
    ଯଦି (keep_ସନ୍ଦେଶ == ଖାଲି && last_ସନ୍ଦେଶ_hist != ଖାଲି && ସନ୍ଦେଶ_scrolled == 0
						      && (State & MODE_NORMAL))
	set_keep_ସନ୍ଦେଶ(last_ସନ୍ଦେଶ_hist->ସନ୍ଦେଶ, last_ସନ୍ଦେଶ_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_start(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_return = FALSE;

    ଯଦି (ସନ୍ଦେଶ_row < cmdline_row)
	ସନ୍ଦେଶ_row = cmdline_row;

    ଯଦି (!ସନ୍ଦେଶ_silent)
    {
	VIM_CLEAR(keep_ସନ୍ଦେଶ);
	need_fileinfo = FALSE;
    }

#ifdef FEAT_EVAL
    ଯଦି (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	ସନ୍ଦେଶ_clr_eos();
    }
#endif

#ifdef HAS_MESSAGE_WINDOW
    ଯଦି (in_echowindow)
    {
	ଯଦି (popup_message_win_visible()
		    && ((ସନ୍ଦେଶ_col > 0 && (ସନ୍ଦେଶ_scroll || !full_screen))
			|| in_echowindow))
	{
	    win_T *wp = popup_get_message_win();

	    // start a new line
	    curbuf = wp->w_buffer;
	    ml_append(wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_buffer;
	}
	ସନ୍ଦେଶ_col = 0;
    }
    ଅନ୍ୟଥା
#endif
	ଯଦି (!ସନ୍ଦେଶ_scroll && full_screen)	// overwrite last message
    {
	ସନ୍ଦେଶ_row = cmdline_row;
	ସନ୍ଦେଶ_col =
#ifdef FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_width - 1 :
#endif
	    0;
    }
    ଅନ୍ୟଥା ଯଦି (ସନ୍ଦେଶ_didout || in_echowindow)
    {
	// start message on next line
	ସନ୍ଦେଶ_putchar('\n');
	did_return = TRUE;
	ଯଦି (exmode_active != EXMODE_NORMAL)
	    cmdline_row = ସନ୍ଦେଶ_row;
    }
    ଯଦି (!ସନ୍ଦେଶ_didany || ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ < 0)
	ସନ୍ଦେଶ_starthere();
    ଯଦି (ସନ୍ଦେଶ_silent == 0)
    {
	ସନ୍ଦେଶ_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    ଯଦି (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_starthere(ଶୂନ୍ୟ)
{
    ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = cmdline_row;
    ସନ୍ଦେଶ_didany = FALSE;
}

    ଶୂନ୍ୟ
ସନ୍ଦେଶ_putchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ସନ୍ଦେଶ_putchar_attr(c, 0);
}

    ଶୂନ୍ୟ
ସନ୍ଦେଶ_putchar_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    ଯଦି (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    ଅନ୍ୟଥା
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)buf, attr);
}

    ଶୂନ୍ୟ
ସନ୍ଦେଶ_outnum(long n)
{
    ଅକ୍ଷର	buf[20];

    sprintf(buf, "%ld", n);
    ସନ୍ଦେଶ_puts(buf);
}

    ଶୂନ୍ୟ
ସନ୍ଦେଶ_home_replace(char_u *fname)
{
    ସନ୍ଦେଶ_home_replace_attr(fname, 0);
}

#ଯଦି defined(FEAT_FIND_ID)
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_home_replace_hl(char_u *fname)
{
    ସନ୍ଦେଶ_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
ସନ୍ଦେଶ_home_replace_attr(char_u *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    char_u	*name;

    name = home_replace_save(ଖାଲି, fname);
    ଯଦି (name != ଖାଲି)
	ସନ୍ଦେଶ_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଦେଶ_outtrans(char_u *str)
{
    ଫେରନ୍ତୁ ସନ୍ଦେଶ_outtrans_attr(str, 0);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଦେଶ_outtrans_attr(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ଫେରନ୍ତୁ ସନ୍ଦେଶ_outtrans_len_attr(str, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str), attr);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଦେଶ_outtrans_len(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ଫେରନ୍ତୁ ସନ୍ଦେଶ_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
ସନ୍ଦେଶ_outtrans_one(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    ଯଦି (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	ସନ୍ଦେଶ_outtrans_len_attr(p, l, attr);
	ଫେରନ୍ତୁ p + l;
    }
    ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)transchar_byte_buf(ଖାଲି, *p), attr);
    ଫେରନ୍ତୁ p + 1;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଦେଶ_outtrans_len_attr(char_u *msgstr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_start = msgstr;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mb_l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    ଯଦି (attr == 0)
	attr = HL_ATTR(HLF_ସନ୍ଦେଶ);

    // if MSG_HIST flag set, add message to history
    ଯଦି (attr & ସନ୍ଦେଶ_HIST)
    {
	add_ସନ୍ଦେଶ_hist(str, len, attr);
	attr &= ~ସନ୍ଦେଶ_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    ଯଦି (ସନ୍ଦେଶ_silent == 0 && len > 0 && ସନ୍ଦେଶ_row >= cmdline_row && ସନ୍ଦେଶ_col == 0)
    {
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    ଯଦି (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	ସନ୍ଦେଶ_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    ଯେତେବେଳେ (--len >= 0 && !got_int)
    {
	ଯଦି (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	ଅନ୍ୟଥା ଯଦି (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	ଅନ୍ୟଥା
	    mb_l = 1;
	ଯଦି (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    ଯଦି (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    ଅନ୍ୟଥା
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		ଯଦି (str > plain_start)
		    ସନ୍ଦେଶ_puts_attr_len((ଅକ୍ଷର *)plain_start,
					       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(str - plain_start), attr);
		plain_start = str + mb_l;
		ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)transchar_buf(ଖାଲି, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	ଅନ୍ୟଥା
	{
	    s = transchar_byte_buf(ଖାଲି, *str);
	    ଯଦି (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		ଯଦି (str > plain_start)
		    ସନ୍ଦେଶ_puts_attr_len((ଅକ୍ଷର *)plain_start,
					       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(str - plain_start), attr);
		plain_start = str + 1;
		ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s);
	    }
	    ଅନ୍ୟଥା
		++retval;
	    ++str;
	}
    }

    ଯଦି (str > plain_start && !got_int)
	// print the printable chars at the end
	ସନ୍ଦେଶ_puts_attr_len((ଅକ୍ଷର *)plain_start, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(str - plain_start), attr);

    got_int |= save_got_int;

    ଫେରନ୍ତୁ retval;
}

#ଯଦି defined(FEAT_QUICKFIX)
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_make(char_u *arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ସ୍ଥିର char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    ପାଇଁ (i = 5; *arg && i >= 0; --i)
	ଯଦି (*arg++ != str[i])
	    ଭାଙ୍ଗ;
    ଯଦି (i < 0)
    {
	ସନ୍ଦେଶ_putchar('\n');
	ପାଇଁ (i = 0; rs[i]; ++i)
	    ସନ୍ଦେଶ_putchar(rs[i] - 3);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଦେଶ_outtrans_special(
    char_u	*strstart,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		from,	// TRUE for lhs of a mapping
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = 0;
    ଅକ୍ଷର	*text;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    attr = HL_ATTR(HLF_8);
    ଯେତେବେଳେ (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	ଯଦି ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    text = "<Space>";
	    ++str;
	}
	ଅନ୍ୟଥା
	    text = (ଅକ୍ଷର *)str2special(&str, from, FALSE);
	ଯଦି (text[0] != NUL && text[1] == NUL)
	    // single-byte character or illegal byte
	    text = (ଅକ୍ଷର *)transchar_byte_buf(ଖାଲି, (char_u)text[0]);
	len = vim_strsize((char_u *)text);
	ଯଦି (maxlen > 0 && retval + len >= maxlen)
	    ଭାଙ୍ଗ;
	// Highlight special keys
	ସନ୍ଦେଶ_puts_attr(text, len > 1
		&& (*mb_ptr2len)((char_u *)text) <= 1 ? attr : 0);
	retval += len;
    }
    ଫେରନ୍ତୁ retval;
}

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    ଯେତେବେଳେ (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    ଫେରନ୍ତୁ (char_u *)ga.ga_data;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			c;
    ସ୍ଥିର char_u	buf[7];
    char_u		*str = *sp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			modifiers = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			special = FALSE;

    ଯଦି (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	ଯଦି (p != ଖାଲି)
	    ଫେରନ୍ତୁ p;
    }

    c = *str;
    ଯଦି ((c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	) && str[1] != NUL && str[2] != NUL)
    {
	ଯଦି (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	ଯଦି ((c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = TO_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	ଯଦି (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    ଯଦି (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	ଯଦି (p != ଖାଲି)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	ଅନ୍ୟଥା
	    // illegal byte
	    *sp = str + 1;
    }
    ଅନ୍ୟଥା
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    ଯଦି (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	ଫେରନ୍ତୁ get_special_ଚାବି_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    ଫେରନ୍ତୁ buf;
}

/*
 * Translate a key sequence into special key names.
 */
    ଶୂନ୍ୟ
str2specialbuf(char_u *sp, char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    ଯେତେବେଳେ (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	ଯଦି (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }
}

/*
 * print line for :print or :list command
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_prt_ଧାଡ଼ି(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା list)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n_extra = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c_extra = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c_final = 0;
    char_u	*p_extra = ଖାଲି;	    // init to make SASC shut up
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr = 0;
    char_u	*trail = ଖାଲି;
    char_u	*lead = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_multispace = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		multispace_pos = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    char_u	buf[MB_MAXBYTES + 1];

    ଯଦି (curwin->w_p_list)
	list = TRUE;

    ଯଦି (list)
    {
	// find start of trailing whitespace
	ଯଦି (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    ଯେତେବେଳେ (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	ଯଦି (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != ଖାଲି
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    ଯେତେବେଳେ (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    ଯଦି (*lead == NUL)
		lead = ଖାଲି;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    ଯଦି (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	ସନ୍ଦେଶ_putchar(' ');

    ଯେତେବେଳେ (!got_int)
    {
	ଯଦି (n_extra > 0)
	{
	    --n_extra;
	    ଯଦି (n_extra == 0 && c_final)
		c = c_final;
	    ଅନ୍ୟଥା ଯଦି (c_extra)
		c = c_extra;
	    ଅନ୍ୟଥା
		c = *p_extra++;
	}
	ଅନ୍ୟଥା ଯଦି (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    col += (*mb_ptr2cells)(s);
	    ଯଦି (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    ଅନ୍ୟଥା ଯଦି (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    ଅନ୍ୟଥା
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)buf);
	    s += l;
	    ଜାରି;
	}
	ଅନ୍ୟଥା
	{
	    attr = 0;
	    c = *s++;
	    ଯଦି (list)
	    {
		in_multispace = c == ' ' && (*s == ' '
						 || (col > 0 && s[-2] == ' '));
		ଯଦି (!in_multispace)
		    multispace_pos = 0;
	    }
	    ଯଦି (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ifdef FEAT_VARTABS
		n_extra = tabstop_padding(col, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#ଅନ୍ୟଥା
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
#endif
		ଯଦି (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		ଅନ୍ୟଥା
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lcs_tab1 = curwin->w_lcs_chars.tab1;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lcs_tab2 = curwin->w_lcs_chars.tab2;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    ଯଦି (lead != ଖାଲି && s <= lead
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
	    ଅନ୍ୟଥା ଯଦି (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    ଅନ୍ୟଥା ଯଦି (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    ଅନ୍ୟଥା ଯଦି (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(ଖାଲି, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    ଅନ୍ୟଥା ଯଦି (c == ' ')
	    {
		ଯଦି (lead != ଖାଲି && s <= lead && in_multispace
			&& curwin->w_lcs_chars.leadmultispace != ଖାଲି)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    ଯଦି (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		ଅନ୍ୟଥା ଯଦି (lead != ଖାଲି && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		ଅନ୍ୟଥା ଯଦି (trail != ଖାଲି && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		ଅନ୍ୟଥା ଯଦି (in_multispace
			&& curwin->w_lcs_chars.multispace != ଖାଲି)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    ଯଦି (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		ଅନ୍ୟଥା ଯଦି (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	ଯଦି (c == NUL)
	    ଭାଙ୍ଗ;

	ସନ୍ଦେଶ_putchar_attr(c, attr);
	col++;
    }
    ସନ୍ଦେଶ_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    ସ୍ଥିର char_u *
screen_puts_mbyte(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cw;

    ସନ୍ଦେଶ_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    ଯଦି (cw > 1 && (
#ifdef FEAT_RIGHTLEFT
		cmdmsg_rl ? ସନ୍ଦେଶ_col <= 1 :
#endif
		ସନ୍ଦେଶ_col == cmdline_width - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	ସନ୍ଦେଶ_screen_putchar('>', HL_ATTR(HLF_AT));
	ଫେରନ୍ତୁ s;
    }

    screen_puts_len(s, l, ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col, attr);
#ifdef FEAT_RIGHTLEFT
    ଯଦି (cmdmsg_rl)
    {
	ସନ୍ଦେଶ_col -= cw;
	ଯଦି (ସନ୍ଦେଶ_col == 0)
	{
	    ସନ୍ଦେଶ_col = cmdline_width;
	    ++ସନ୍ଦେଶ_row;
	}
    }
    ଅନ୍ୟଥା
#endif
    {
	ସନ୍ଦେଶ_col += cw;
	ଯଦି (ସନ୍ଦେଶ_col >= cmdline_width)
	{
	    ସନ୍ଦେଶ_col = 0;
	    ++ସନ୍ଦେଶ_row;
	}
    }
    ଫେରନ୍ତୁ s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_puts(ଅକ୍ଷର *s)
{
    ସନ୍ଦେଶ_puts_attr(s, 0);
}

    ଶୂନ୍ୟ
ସନ୍ଦେଶ_puts_title(ଅକ୍ଷର *s)
{
    ସନ୍ଦେଶ_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ସନ୍ଦେଶ_outtrans_long_len_attr(char_u *longstr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		slen = len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room;

    room = cmdline_width - ସନ୍ଦେଶ_col;
    ଯଦି (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	ସନ୍ଦେଶ_outtrans_len_attr(longstr, slen, attr);
	ସନ୍ଦେଶ_puts_attr("...", HL_ATTR(HLF_8));
    }
    ସନ୍ଦେଶ_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    ଶୂନ୍ୟ
ସନ୍ଦେଶ_outtrans_long_attr(char_u *longstr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ସନ୍ଦେଶ_outtrans_long_len_attr(longstr, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_puts_attr(ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ସନ୍ଦେଶ_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ସନ୍ଦେଶ_puts_attr_len(ଅକ୍ଷର *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    ଯଦି (ସନ୍ଦେଶ_silent != 0)
	ଫେରନ୍ତୁ;

    ଯଦି (attr == 0)
	attr = HL_ATTR(HLF_ସନ୍ଦେଶ);

    // if MSG_HIST flag set, add message to history
    ଯଦି ((attr & ସନ୍ଦେଶ_HIST) && maxlen < 0)
    {
	add_ସନ୍ଦେଶ_hist((char_u *)str, -1, attr);
	attr &= ~ସନ୍ଦେଶ_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    ଯଦି (ସନ୍ଦେଶ_scrolled != 0 && !ସନ୍ଦେଶ_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    ସନ୍ଦେଶ_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    ଯଦି (ସନ୍ଦେଶ_use_printf())
	ସନ୍ଦେଶ_puts_printf((char_u *)str, maxlen);
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ_puts_display((char_u *)str, maxlen, attr, FALSE);

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
    ସ୍ଥିର ଶୂନ୍ୟ
put_ସନ୍ଦେଶ_win(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା where, char_u *t_s, char_u *end, linenr_T lnum)
{
    char_u  *p;

    ଯଦି (where == PUT_BELOW)
    {
	ଯଦି (*end != NUL)
	{
	    p = vim_strnsave(t_s, end - t_s);
	    ଯଦି (p == ଖାଲି)
		ଫେରନ୍ତୁ;
	}
	ଅନ୍ୟଥା
	    p = t_s;
	ml_append_buf(wp->w_buffer, lnum, p, (colnr_T)0, FALSE);
	ଯଦି (p != t_s)
	    vim_free(p);
    }
    ଅନ୍ୟଥା
    {
	char_u *newp;

	curbuf = wp->w_buffer;
	ଯଦି (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    ଯଦି (newp == ଖାଲି)
		ଫେରନ୍ତୁ;
	    ଯଦି (*end != NUL)
		newp[STRLEN(ml_get(lnum)) + (end - t_s)] = NUL;
	}
	ଅନ୍ୟଥା
	{
	    newp = vim_strnsave(t_s, end - t_s);
	    ଯଦି (newp == ଖାଲି)
		ଫେରନ୍ତୁ;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_buffer;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    ସନ୍ଦେଶ_col += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - t_s);
}
#endif

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ସନ୍ଦେଶ_puts_display(
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		t_col = 0;	// screen cells todo, 0 when "t_s" not used
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cw;
    char_u	*sb_str = str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sb_col = ସନ୍ଦେଶ_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wrap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_last_char;
#ifdef HAS_MESSAGE_WINDOW
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		where = PUT_APPEND;
    win_T	*ସନ୍ଦେଶ_win = ଖାଲି;
    linenr_T    lnum = 1;

    ଯଦି (in_echowindow)
    {
	ସନ୍ଦେଶ_win = popup_get_message_win();

	ଯଦି (ସନ୍ଦେଶ_win != ଖାଲି)
	{
	    ଯଦି (!popup_message_win_visible())
	    {
		ଯଦି (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    ସନ୍ଦେଶ_win = ଖାଲି;
		}
		ଅନ୍ୟଥା
		{
		    // currently hidden, make it empty
		    curbuf = ସନ୍ଦେଶ_win->w_buffer;
		    ଯେତେବେଳେ ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
			ml_delete(1);
		    curbuf = curwin->w_buffer;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		lnum = ସନ୍ଦେଶ_win->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
		ଯଦି (ସନ୍ଦେଶ_col == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#endif

    did_wait_return = FALSE;
    ଯେତେବେଳେ ((maxlen < 0 || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - str) < maxlen) && *s != NUL)
    {
#ifdef HAS_MESSAGE_WINDOW
	// For echowindow, use full width; for regular messages, leave last column
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା wrap_col = ସନ୍ଦେଶ_win != ଖାଲି ? cmdline_width : cmdline_width - 1;
#ଅନ୍ୟଥା
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା wrap_col = cmdline_width - 1;
#endif

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	ଯଦି (!recurse && ସନ୍ଦେଶ_row >= Rows - 1 && (*s == '\n' || (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			ସନ୍ଦେଶ_col <= 1
		      || (*s == TAB && ସନ୍ଦେଶ_col <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && ସନ୍ଦେଶ_col <= 2))
		    :
#endif
		      ((*s != '\r' && ସନ୍ଦେଶ_col + t_col >= wrap_col)
		       || (*s == TAB && ସନ୍ଦେଶ_col + t_col
			   >= (wrap_col & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && ସନ୍ଦେଶ_col + t_col >= wrap_col - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    ଯଦି (t_col > 0)
	    {
		// output postponed text
#ifdef HAS_MESSAGE_WINDOW
		ଯଦି (ସନ୍ଦେଶ_win != ଖାଲି)
		{
		    put_ସନ୍ଦେଶ_win(ସନ୍ଦେଶ_win, where, t_s, s, lnum);
		    ଯଦି (where == PUT_BELOW)
			++lnum;
		    t_col = 0;
		    where = PUT_BELOW;
		}
		ଅନ୍ୟଥା
#endif
		    t_puts(&t_col, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    ଯଦି (ସନ୍ଦେଶ_no_more && ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ == 0)
		ଭାଙ୍ଗ;

#ifdef HAS_MESSAGE_WINDOW
	    ଯଦି (ସନ୍ଦେଶ_win == ଖାଲି)
#endif
		// Scroll the screen up one line.
		ସନ୍ଦେଶ_scroll_up();

#ifdef HAS_MESSAGE_WINDOW
	    ଯଦି (ସନ୍ଦେଶ_win == ଖାଲି)
	    {
#endif
		ସନ୍ଦେଶ_row = Rows - 2;
		ଯଦି (ସନ୍ଦେଶ_col >= cmdline_width)  // can happen after screen resize
		    ସନ୍ଦେଶ_col = cmdline_width - 1;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif

	    // Display char in last column before showing more-prompt.
	    ଯଦି (*s >= ' '
#ifdef FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#endif
	       )
	    {
		ଯଦି (has_mbyte)
		{
		    ଯଦି (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((str + maxlen) - s));
		    ଅନ୍ୟଥା
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		ଅନ୍ୟଥା
		    ସନ୍ଦେଶ_screen_putchar(*s++, attr);
		did_last_char = TRUE;
	    }
	    ଅନ୍ୟଥା
		did_last_char = FALSE;

	    ଯଦି (p_more)
		// store text for scrolling back
		store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

#ifdef HAS_MESSAGE_WINDOW
	    ଯଦି (ସନ୍ଦେଶ_win == ଖାଲି)
	    {
#endif
		inc_ସନ୍ଦେଶ_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		ଯଦି (cmdline_row > 0 && !exmode_active)
		    --cmdline_row;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ > 0)
		    --ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif
	    ଯଦି (p_more && ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ == 0 && State != MODE_HITRETURN
					    && !ସନ୍ଦେଶ_no_more && !exmode_active)
	    {
#ifdef FEAT_CON_DIALOG
		ଯଦି (do_more_prompt(NUL))
		    s = confirm_ସନ୍ଦେଶ_tail;
#ଅନ୍ୟଥା
		(ଶୂନ୍ୟ)do_more_prompt(NUL);
#endif
		ଯଦି (quit_more)
		    ଫେରନ୍ତୁ;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    ଯଦି (did_last_char)
		ଜାରି;
	}

	wrap = *s == '\n'
		    || ସନ୍ଦେଶ_col + t_col >= wrap_col
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && ସନ୍ଦେଶ_col + t_col >= wrap_col - 1);
	ଯଦି (t_col > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ifdef HAS_MESSAGE_WINDOW
	    ଯଦି (ସନ୍ଦେଶ_win != ଖାଲି)
	    {
		put_ସନ୍ଦେଶ_win(ସନ୍ଦେଶ_win, where, t_s, s, lnum);
		ଯଦି (where == PUT_BELOW)
		    ++lnum;
		t_col = 0;
		where = PUT_BELOW;
		// Reset msg_col after outputting to new line in echowindow
		ଯଦି (wrap)
		    ସନ୍ଦେଶ_col = 0;
	    }
	    ଅନ୍ୟଥା
#endif
		t_puts(&t_col, t_s, s, attr);
	}

	ଯଦି (wrap && p_more && !recurse)
	    // store text for scrolling back
	    store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	ଯଦି (*s == '\n')		    // go to next line
	{
#ifdef HAS_MESSAGE_WINDOW
	    ଯଦି (ସନ୍ଦେଶ_win != ଖାଲି)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		ଯଦି ((ସନ୍ଦେଶ_win->w_buffer->b_ml.ml_flags & ML_EMPTY) == 0)
		{
		    put_ସନ୍ଦେଶ_win(ସନ୍ଦେଶ_win, PUT_BELOW, t_s, t_s, lnum);
		    ++lnum;
		}
		ସନ୍ଦେଶ_col = 0;  // Reset column for new line
	    }
	    ଅନ୍ୟଥା
#endif
		ସନ୍ଦେଶ_didout = FALSE;	    // remember that line is empty
#ifdef FEAT_RIGHTLEFT
	    ଯଦି (cmdmsg_rl)
		ସନ୍ଦେଶ_col = cmdline_width - 1;
	    ଅନ୍ୟଥା
#endif
		ସନ୍ଦେଶ_col = 0;
	    ଯଦି (++ସନ୍ଦେଶ_row >= Rows)  // safety check
		ସନ୍ଦେଶ_row = Rows - 1;
	}
	ଅନ୍ୟଥା ଯଦି (*s == '\r')	    // go to column 0
	{
	    ସନ୍ଦେଶ_col = 0;
#ifdef HAS_MESSAGE_WINDOW
	    where = PUT_TRUNC;
#endif
	}
	ଅନ୍ୟଥା ଯଦି (*s == '\b')	    // go to previous char
	{
	    ଯଦି (ସନ୍ଦେଶ_col)
		--ସନ୍ଦେଶ_col;
	}
	ଅନ୍ୟଥା ଯଦି (*s == TAB)	    // translate Tab into spaces
	{
#ifdef HAS_MESSAGE_WINDOW
	    ଯଦି (ସନ୍ଦେଶ_win != ଖାଲି)
		ସନ୍ଦେଶ_col = (ସନ୍ଦେଶ_col + 7) % 8;
	    ଅନ୍ୟଥା
#endif
		କର
		    ସନ୍ଦେଶ_screen_putchar(' ', attr);
		ଯେତେବେଳେ (ସନ୍ଦେଶ_col & 7);
	}
	ଅନ୍ୟଥା ଯଦି (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	ଅନ୍ୟଥା
	{
	    ଯଦି (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		ଯଦି (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((str + maxlen) - s));
		ଅନ୍ୟଥା
		    l = (*mb_ptr2len)(s);
	    }
	    ଅନ୍ୟଥା
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    ଯଦି (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#endif
		    (cw > 1 && ସନ୍ଦେଶ_col + t_col >= wrap_col))
	    {
		ଯଦି (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		ଅନ୍ୟଥା
		    ସନ୍ଦେଶ_screen_putchar(*s, attr);
	    }
	    ଅନ୍ୟଥା
	    {
		// postpone this character until later
		ଯଦି (t_col == 0)
		    t_s = s;
		t_col += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    ଯଦି (t_col > 0)
    {
#ifdef HAS_MESSAGE_WINDOW
	ଯଦି (ସନ୍ଦେଶ_win != ଖାଲି)
	    put_ସନ୍ଦେଶ_win(ସନ୍ଦେଶ_win, where, t_s, s, lnum);
	ଅନ୍ୟଥା
#endif
	    t_puts(&t_col, t_s, s, attr);
    }

#ifdef HAS_MESSAGE_WINDOW
    ଯଦି (ସନ୍ଦେଶ_win != ଖାଲି)
	popup_show_message_win();
#endif
    // Store the text for scroll back.
    ଯଦି (p_more && !recurse)
	store_sb_text(&sb_str, s, attr, &sb_col, FALSE);

    ସନ୍ଦେଶ_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
message_filtered(char_u *ସନ୍ଦେଶ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା match;

    ଯଦି (cmdmod.cmod_filter_regmatch.regprog == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    match = vim_regexec(&cmdmod.cmod_filter_regmatch, ସନ୍ଦେଶ, (colnr_T)0);
    ଫେରନ୍ତୁ cmdmod.cmod_filter_force ? match : !match;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ସନ୍ଦେଶ_scroll_up(ଶୂନ୍ୟ)
{
#ifdef HAS_MESSAGE_WINDOW
    ଯଦି (in_echowindow)
	ଫେରନ୍ତୁ;
#endif
#ifdef FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    ଯଦି (gui.in_use)
	gui_undraw_cursor();
#endif
    // scrolling up always works
    mch_disable_flush();
    screen_del_ଧାଡ଼ିଗୁଡ଼ିକ(0, 0, 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, TRUE, 0, ଖାଲି);
    mch_enable_flush();

    ଯଦି (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, cmdline_col_off,
		cmdline_col_off + cmdline_width, ' ', ' ', HL_ATTR(HLF_ସନ୍ଦେଶ));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	ଯଦି (ScreenAttrs[ଧାଡ଼ି_Offset[Rows - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 2, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1,
				 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns - 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns,
				 ' ', ' ', HL_ATTR(HLF_ସନ୍ଦେଶ));
    }
}

/*
 * Increment "msg_scrolled".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
inc_ସନ୍ଦେଶ_scrolled(ଶୂନ୍ୟ)
{
#ifdef FEAT_EVAL
    ଯଦି (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = ଖାଲି;

	// v:scrollstart is empty, set it to the script/function name and line
	//ସଂଖ୍ୟା
	ଯଦି (p.string == ଖାଲି)
	{
	    p.string = (char_u *)_("Unknown");
	    p.length = STRLEN(p.string);
	}
	ଅନ୍ୟଥା
	{
	    size_t  tofreesize;

	    tofreesize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    ଯଦି (tofree != ଖାଲି)
	    {
		p.length = vim_snprintf_safelen((ଅକ୍ଷର *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (long)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	set_vim_var_string(VV_SCROLLSTART, p.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p.length);
	vim_free(tofree);
    }
#endif
    ++ସନ୍ଦେଶ_scrolled;
    set_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା msgchunk_S msgchunk_T;
ସଂରଚନା msgchunk_S
{
    msgchunk_T	*sb_next;
    msgchunk_T	*sb_prev;
    ଅକ୍ଷର	sb_eol;		// TRUE when line ends after this text
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sb_ସନ୍ଦେଶ_col;	// column in which text starts
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sb_attr;	// text attributes
    char_u	sb_text[1];	// text to be displayed, actually longer
};

ସ୍ଥିର msgchunk_T *last_msgchunk = ଖାଲି; // last displayed text

ସ୍ଥିର msgchunk_T *ସନ୍ଦେଶ_sb_start(msgchunk_T *mps);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_DONE
} sb_clear_T;

// When to clear text on next msg.
ସ୍ଥିର sb_clear_T do_clear_sb_text = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
store_sb_text(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*sb_col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		finish)		// line ends
{
    msgchunk_T	*mp;

    ଯଦି (do_clear_sb_text == SB_CLEAR_ALL
	    || do_clear_sb_text == SB_CLEAR_CMDLINE_DONE)
    {
	clear_sb_text(do_clear_sb_text == SB_CLEAR_ALL);
	ସନ୍ଦେଶ_sb_eol();  // prevent messages from overlapping
	do_clear_sb_text = SB_CLEAR_NONE;
    }

    ଯଦି (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_text) + (s - *sb_str) + 1);
	ଯଦି (mp != ଖାଲି)
	{
	    mp->sb_eol = finish;
	    mp->sb_ସନ୍ଦେଶ_col = *sb_col;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_text, *sb_str, s - *sb_str);

	    ଯଦି (last_msgchunk == ଖାଲି)
	    {
		last_msgchunk = mp;
		mp->sb_prev = ଖାଲି;
	    }
	    ଅନ୍ୟଥା
	    {
		mp->sb_prev = last_msgchunk;
		last_msgchunk->sb_next = mp;
		last_msgchunk = mp;
	    }
	    mp->sb_next = ଖାଲି;
	}
    }
    ଅନ୍ୟଥା ଯଦି (finish && last_msgchunk != ଖାଲି)
	last_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_col = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    ଶୂନ୍ୟ
may_clear_sb_text(ଶୂନ୍ୟ)
{
    do_clear_sb_text = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    ଶୂନ୍ୟ
sb_text_start_cmdline(ଶୂନ୍ୟ)
{
    ଯଦି (do_clear_sb_text == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_text_restart_cmdline();
    ଅନ୍ୟଥା
    {
	ସନ୍ଦେଶ_sb_eol();
	do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    ଶୂନ୍ୟ
sb_text_restart_cmdline(ଶୂନ୍ୟ)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;

    ଯଦି (last_msgchunk == ଖାଲି || last_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	ଫେରନ୍ତୁ;

    tofree = ସନ୍ଦେଶ_sb_start(last_msgchunk);
    last_msgchunk = tofree->sb_prev;
    ଯଦି (last_msgchunk != ଖାଲି)
	last_msgchunk->sb_next = ଖାଲି;
    ଯେତେବେଳେ (tofree != ଖାଲି)
    {
	msgchunk_T *tofree_next = tofree->sb_next;

	vim_free(tofree);
	tofree = tofree_next;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    ଶୂନ୍ୟ
sb_text_end_cmdline(ଶୂନ୍ୟ)
{
    do_clear_sb_text = SB_CLEAR_CMDLINE_DONE;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    ଶୂନ୍ୟ
clear_sb_text(ପୂର୍ଣ୍ଣସଂଖ୍ୟା all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    ଯଦି (all)
	lastp = &last_msgchunk;
    ଅନ୍ୟଥା
    {
	ଯଦି (last_msgchunk == ଖାଲି)
	    ଫେରନ୍ତୁ;
	lastp = &ସନ୍ଦେଶ_sb_start(last_msgchunk)->sb_prev;
    }

    ଯେତେବେଳେ (*lastp != ଖାଲି)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    ଶୂନ୍ୟ
show_sb_text(ଶୂନ୍ୟ)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = ସନ୍ଦେଶ_sb_start(last_msgchunk);
    ଯଦି (mp == ଖାଲି || mp->sb_prev == ଖାଲି)
	vim_beep(BO_MESS);
    ଅନ୍ୟଥା
    {
	do_more_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    ସ୍ଥିର msgchunk_T *
ସନ୍ଦେଶ_sb_start(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    ଯେତେବେଳେ (mp != ଖାଲି && mp->sb_prev != ଖାଲି && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    ଫେରନ୍ତୁ mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_sb_eol(ଶୂନ୍ୟ)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    ଯଦି (last_msgchunk != ଖାଲି
	    && !(last_msgchunk->sb_text[0] == '\n'
		&& last_msgchunk->sb_text[1] == NUL))
	last_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    ସ୍ଥିର msgchunk_T *
disp_sb_ଧାଡ଼ି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, msgchunk_T *smp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା clear_to_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    ପାଇଁ (;;)
    {
	ସନ୍ଦେଶ_row = row;
	ସନ୍ଦେଶ_col = mp->sb_ସନ୍ଦେଶ_col;
	p = mp->sb_text;
	ଯଦି (*p == '\n')	    // don't display the line break
	    ++p;
	ସନ୍ଦେଶ_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	ଯଦି (clear_to_eol)
	    screen_fill(row, row + 1,
		    cmdline_col_off + ସନ୍ଦେଶ_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', HL_ATTR(HLF_ସନ୍ଦେଶ));

	ଯଦି (mp->sb_eol || mp->sb_next == ଖାଲି)
	    ଭାଙ୍ଗ;
	mp = mp->sb_next;
    }
    ଫେରନ୍ତୁ mp->sb_next;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
t_puts(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*t_col,
    char_u	*t_s,
    char_u	*s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr)
{
    // output postponed text
    ସନ୍ଦେଶ_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - t_s), ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col,
	    attr);
    ସନ୍ଦେଶ_col += *t_col;
    *t_col = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    ଯଦି (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--ସନ୍ଦେଶ_col;
    ଯଦି (ସନ୍ଦେଶ_col >= cmdline_width)
    {
	ସନ୍ଦେଶ_col = 0;
	++ସନ୍ଦେଶ_row;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଦେଶ_use_printf(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (!ସନ୍ଦେଶ_check_screen()
#ଯଦି defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.in_use && !termcap_active)
# ଅନ୍ୟଥା
	    || !termcap_active
# endif
#endif
	    || (swapping_screen() && !termcap_active)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ସନ୍ଦେଶ_puts_printf(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlen)
{
    char_u	*s = str;
    char_u	*buf = ଖାଲି;
    char_u	*p = s;

#ifdef MSWIN
    ଯଦି (!(silent_mode && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#endif
    ଯେତେବେଳେ ((maxlen < 0 || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - str) < maxlen) && *s != NUL)
    {
	ଯଦି (!(silent_mode && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    ଯଦି (*s == NL)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - p);

		buf = alloc(n + 3);
		ଯଦି (buf != ଖାଲି)
		{
		    memcpy(buf, p, n);
		    ଯଦି (!info_message)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    ଯଦି (info_message)   // informative message, not an error
			mch_ସନ୍ଦେଶ((ଅକ୍ଷର *)buf);
		    ଅନ୍ୟଥା
			mch_errmsg((ଅକ୍ଷର *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ifdef FEAT_RIGHTLEFT
	ଯଦି (cmdmsg_rl)
	{
	    ଯଦି (*s == CAR || *s == NL)
		ସନ୍ଦେଶ_col = cmdline_width - 1;
	    ଅନ୍ୟଥା
		--ସନ୍ଦେଶ_col;
	}
	ଅନ୍ୟଥା
#endif
	{
	    ଯଦି (*s == CAR || *s == NL)
		ସନ୍ଦେଶ_col = 0;
	    ଅନ୍ୟଥା
		++ସନ୍ଦେଶ_col;
	}
	++s;
    }

    ଯଦି (*p != NUL && !(silent_mode && p_verbose == 0))
    {
	char_u *tofree = ଖାଲି;

	ଯଦି (maxlen > 0 && vim_strlen_maxlen((ଅକ୍ଷର *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	ଯଦି (p != ଖାଲି)
	{
	    ଯଦି (info_message)
		mch_ସନ୍ଦେଶ((ଅକ୍ଷର *)p);
	    ଅନ୍ୟଥା
		mch_errmsg((ଅକ୍ଷର *)p);
	    vim_free(tofree);
	}
    }

    ସନ୍ଦେଶ_didout = TRUE;	    // assume that line is not empty

#ifdef MSWIN
    ଯଦି (!(silent_mode && p_verbose == 0))
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_more_prompt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା typed_char)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	entered = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		used_typed_char = typed_char;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oldState = State;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
#ifdef FEAT_CON_DIALOG
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		toscroll;
    msgchunk_T	*mp_last = ଖାଲି;
    msgchunk_T	*mp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସନ୍ଦେଶ_attr;

    ସନ୍ଦେଶ_attr = HL_ATTR(HLF_ସନ୍ଦେଶ);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    ଯଦି (entered || (State == MODE_HITRETURN && typed_char == 0))
	ଫେରନ୍ତୁ FALSE;
    entered = TRUE;

    ଯଦି (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_last = ସନ୍ଦେଶ_sb_start(last_msgchunk);
	ପାଇଁ (i = 0; i < Rows - 2 && mp_last != ଖାଲି
					     && mp_last->sb_prev != ଖାଲି; ++i)
	    mp_last = ସନ୍ଦେଶ_sb_start(mp_last->sb_prev);
    }

    State = MODE_ASKMORE;
    setmouse();
    ଯଦି (typed_char == NUL)
	ସନ୍ଦେଶ_moremsg(FALSE);
    ପାଇଁ (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	ଯଦି (used_typed_char != NUL)
	{
	    c = used_typed_char;	// was typed at hit-enter prompt
	    used_typed_char = NUL;
	}
	ଅନ୍ୟଥା
	    c = get_keystroke();

#ଯଦି defined(FEAT_MENU) && defined(FEAT_GUI)
	ଯଦି (c == K_MENU)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = get_menu_index(current_menu, MODE_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    ଯଦି (idx == MENU_INDEX_INVALID)
		ଜାରି;
	    c = *current_menu->strings[idx];
	    ଯଦି (c != NUL && current_menu->strings[idx][1] != NUL)
		ins_typebuf(current_menu->strings[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#endif

	toscroll = 0;
	ବାଛ (c)
	{
	ସ୍ଥିତି BS:		// scroll one line back
	ସ୍ଥିତି K_BS:
	ସ୍ଥିତି 'k':
	ସ୍ଥିତି K_UP:
	    toscroll = -1;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CAR:		// one extra line
	ସ୍ଥିତି NL:
	ସ୍ଥିତି 'j':
	ସ୍ଥିତି K_DOWN:
	    toscroll = 1;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି 'u':		// Up half a page
	    toscroll = -(Rows / 2);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି 'd':		// Down half a page
	    toscroll = Rows / 2;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି 'b':		// one page back
	ସ୍ଥିତି Ctrl_B:
	ସ୍ଥିତି K_PAGEUP:
	    toscroll = -(Rows - 1);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ' ':		// one extra page
	ସ୍ଥିତି 'f':
	ସ୍ଥିତି Ctrl_F:
	ସ୍ଥିତି K_PAGEDOWN:
	ସ୍ଥିତି K_LEFTMOUSE:
	    toscroll = Rows - 1;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି 'g':		// all the way back to the start
	    toscroll = -999999;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି 'G':		// all the way to the end
	    toscroll = 999999;
	    ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = 999999;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ':':		// start new command line
#ifdef FEAT_CON_DIALOG
	    ଯଦି (!confirm_ସନ୍ଦେଶ_used)
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
	ସ୍ଥିତି 'q':		// quit
	ସ୍ଥିତି Ctrl_C:
	ସ୍ଥିତି ESC:
#ifdef FEAT_CON_DIALOG
	    ଯଦି (confirm_ସନ୍ଦେଶ_used)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    ଅନ୍ୟଥା
#endif
	    {
		got_int = TRUE;
		quit_more = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = Rows - 1;
	    ଭାଙ୍ଗ;

#ifdef FEAT_CLIPBOARD
	ସ୍ଥିତି Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    ଯଦି (clip_star.state == SELECT_DONE)
		clip_copy_modeless_selection(TRUE);
	    ଜାରି;
#endif
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:		// no valid response
	    ସନ୍ଦେଶ_moremsg(TRUE);
	    ଜାରି;
	}

	ଯଦି (toscroll != 0)
	{
	    ଯଦି (toscroll < 0)
	    {
		// go to start of last line
		ଯଦି (mp_last == ଖାଲି)
		    mp = ସନ୍ଦେଶ_sb_start(last_msgchunk);
		ଅନ୍ୟଥା ଯଦି (mp_last->sb_prev != ଖାଲି)
		    mp = ସନ୍ଦେଶ_sb_start(mp_last->sb_prev);
		ଅନ୍ୟଥା
		    mp = ଖାଲି;

		// go to start of line at top of the screen
		ପାଇଁ (i = 0; i < Rows - 2 && mp != ଖାଲି && mp->sb_prev != ଖାଲି;
									  ++i)
		    mp = ସନ୍ଦେଶ_sb_start(mp->sb_prev);

		ଯଦି (mp != ଖାଲି && mp->sb_prev != ଖାଲି)
		{
		    // Find line to be displayed at top.
		    ପାଇଁ (i = 0; i > toscroll; --i)
		    {
			ଯଦି (mp == ଖାଲି || mp->sb_prev == ଖାଲି)
			    ଭାଙ୍ଗ;
			mp = ସନ୍ଦେଶ_sb_start(mp->sb_prev);
			ଯଦି (mp_last == ଖାଲି)
			    mp_last = ସନ୍ଦେଶ_sb_start(last_msgchunk);
			ଅନ୍ୟଥା
			    mp_last = ସନ୍ଦେଶ_sb_start(mp_last->sb_prev);
		    }

		    ଯଦି (toscroll == -1 && screen_ins_ଧାଡ଼ିଗୁଡ଼ିକ(0, 0, 1,
						     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, 0, ଖାଲି) == OK)
		    {
			// display line at top
			(ଶୂନ୍ୟ)disp_sb_ଧାଡ଼ି(0, mp, FALSE);
		    }
		    ଅନ୍ୟଥା
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_clear = screenclear();

			// redisplay all lines
			ପାଇଁ (i = 0; mp != ଖାଲି && i < Rows - 1; ++i)
			{
			    mp = disp_sb_ଧାଡ଼ି(i, mp, !did_clear);
			    ++ସନ୍ଦେଶ_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		// First display any text that we scrolled back.
		ଯେତେବେଳେ (toscroll > 0 && mp_last != ଖାଲି)
		{
		    // scroll up, display line at bottom
		    ସନ୍ଦେଶ_scroll_up();
		    inc_ସନ୍ଦେଶ_scrolled();
		    screen_fill((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 2, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1,
			    cmdline_col_off, cmdline_col_off + cmdline_width,
			    ' ', ' ', ସନ୍ଦେଶ_attr);
		    mp_last = disp_sb_ଧାଡ଼ି((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 2, mp_last, FALSE);
		    --toscroll;
		}
	    }

	    ଯଦି (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, cmdline_col_off,
			cmdline_col_off + cmdline_width, ' ', ' ', ସନ୍ଦେଶ_attr);
		ସନ୍ଦେଶ_moremsg(FALSE);
		ଜାରି;
	    }

	    // display more text, return to caller
	    ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = toscroll;
	}

	ଭାଙ୍ଗ;
    }

    // clear the --more-- message
    screen_fill((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, cmdline_col_off,
	    cmdline_col_off + cmdline_width, ' ', ' ', ସନ୍ଦେଶ_attr);
    State = oldState;
    setmouse();
    ଯଦି (quit_more)
    {
	ସନ୍ଦେଶ_row = Rows - 1;
	ସନ୍ଦେଶ_col = 0;
    }
#ifdef FEAT_RIGHTLEFT
    ଅନ୍ୟଥା ଯଦି (cmdmsg_rl)
	ସନ୍ଦେଶ_col = cmdline_width - 1;
#endif

    entered = FALSE;
#ifdef FEAT_CON_DIALOG
    ଫେରନ୍ତୁ retval;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FALSE;
#endif
}

#ଯଦି defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_ସନ୍ଦେଶ
#  undef mch_ସନ୍ଦେଶ
# endif

# ଯଦି defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ସ୍ଥିର ଶୂନ୍ୟ
mch_errmsg_c(ଅକ୍ଷର *str)
{
    DWORD   nwrite = 0;
    DWORD   mode = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    ଯଦି (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GetConsoleCP() != enc_codepage)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	ଯଦି (w != ଖାଲି)
	{
	    WriteConsoleW(h, w, len, &nwrite, ଖାଲି);
	    vim_free(w);
	}
    }
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
mch_errmsg(ଅକ୍ଷର *str)
{
# ଯଦି !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
# endif

# ଯଦି (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    ଯଦି (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   ଅନ୍ୟଥା
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
	ଫେରନ୍ତୁ;
    }
# endif

# ଯଦି defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ଯଦି (!(gui.in_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	ଫେରନ୍ତୁ;
    }
# endif

# ଯଦି !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str) + 1;
    ଯଦି (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    ଯଦି (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_data + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_data + error_ga.ga_len;
	    ପାଇଁ (;;)
	    {
		p = vim_strchr(p, '\r');
		ଯଦି (p == ଖାଲି)
		    ଭାଙ୍ଗ;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# ଯଦି defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ସ୍ଥିର ଶୂନ୍ୟ
mch_ସନ୍ଦେଶ_c(ଅକ୍ଷର *str)
{
    DWORD   nwrite = 0;
    DWORD   mode;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    ଯଦି (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GetConsoleCP() != enc_codepage)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	ଯଦି (w != ଖାଲି)
	{
	    WriteConsoleW(h, w, len, &nwrite, ଖାଲି);
	    vim_free(w);
	}
    }
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
mch_ସନ୍ଦେଶ(ଅକ୍ଷର *str)
{
# ଯଦି (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    ଯଦି (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   ଅନ୍ୟଥା
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
	ଫେରନ୍ତୁ;
    }
# endif

# ଯଦି defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ଯଦି (!(gui.in_use || gui.starting))
#  endif
    {
	mch_ସନ୍ଦେଶ_c(str);
	ଫେରନ୍ତୁ;
    }
# endif
# ଯଦି !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#endif // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ସନ୍ଦେଶ_screen_putchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ସନ୍ଦେଶ_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col, attr);
#ifdef FEAT_RIGHTLEFT
    ଯଦି (cmdmsg_rl)
    {
	ଯଦି (--ସନ୍ଦେଶ_col == 0)
	{
	    ସନ୍ଦେଶ_col = cmdline_width;
	    ++ସନ୍ଦେଶ_row;
	}
    }
    ଅନ୍ୟଥା
#endif
    {
	ଯଦି (++ସନ୍ଦେଶ_col >= cmdline_width)
	{
	    ସନ୍ଦେଶ_col = 0;
	    ++ସନ୍ଦେଶ_row;
	}
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
ସନ୍ଦେଶ_moremsg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା full)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, cmdline_col_off, attr);
    ଯଦି (full)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, cmdline_col_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    ଶୂନ୍ୟ
repeat_message(ଶୂନ୍ୟ)
{
    ଯଦି (State == MODE_ASKMORE)
    {
	ସନ୍ଦେଶ_moremsg(TRUE);	// display --more-- message again
	ସନ୍ଦେଶ_row = Rows - 1;
    }
#ifdef FEAT_CON_DIALOG
    ଅନ୍ୟଥା ଯଦି (State == MODE_CONFIRM)
    {
	display_confirm_ସନ୍ଦେଶ();	// display ":confirm" message again
	ସନ୍ଦେଶ_row = Rows - 1;
    }
#endif
    ଅନ୍ୟଥା ଯଦି (State == MODE_EXTERNCMD)
    {
	windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col); // put cursor back
    }
    ଅନ୍ୟଥା ଯଦି (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	ଯଦି (ସନ୍ଦେଶ_row == Rows - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    ସନ୍ଦେଶ_didout = FALSE;
	    ସନ୍ଦେଶ_col = 0;
	    ସନ୍ଦେଶ_clr_eos();
	}
	hit_return_ସନ୍ଦେଶ();
	ସନ୍ଦେଶ_row = Rows - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଦେଶ_check_screen(ଶୂନ୍ୟ)
{
    ଯଦି (!full_screen || !screen_valid(FALSE))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (ସନ୍ଦେଶ_row >= Rows)
	ସନ୍ଦେଶ_row = Rows - 1;
    ଯଦି (ସନ୍ଦେଶ_col >= cmdline_width)
	ସନ୍ଦେଶ_col = cmdline_width - 1;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_clr_eos(ଶୂନ୍ୟ)
{
    ଯଦି (ସନ୍ଦେଶ_silent == 0)
	ସନ୍ଦେଶ_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_clr_eos_force(ଶୂନ୍ୟ)
{
#ifdef HAS_MESSAGE_WINDOW
    ଯଦି (in_echowindow)
	ଫେରନ୍ତୁ;  // messages go into a popup
#endif
    ଯଦି (ସନ୍ଦେଶ_use_printf())
    {
	ଯଦି (full_screen)	// only when termcap codes are valid
	{
	    ଯଦି (*T_CD)
		out_str(T_CD);	// clear to end of display
	    ଅନ୍ୟଥା ଯଦି (*T_CE)
		out_str(T_CE);	// clear to end of line
	}
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_attr;

	ସନ୍ଦେଶ_attr = HL_ATTR(HLF_ସନ୍ଦେଶ);

#ifdef FEAT_RIGHTLEFT
	ଯଦି (cmdmsg_rl)
	{
	    screen_fill(ସନ୍ଦେଶ_row, ସନ୍ଦେଶ_row + 1,
		    cmdline_col_off, cmdline_col_off + ସନ୍ଦେଶ_col + 1,
		    ' ', ' ', ସନ୍ଦେଶ_attr);
	    screen_fill(ସନ୍ଦେଶ_row + 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', ସନ୍ଦେଶ_attr);
	}
	ଅନ୍ୟଥା
#endif
	{
	    screen_fill(ସନ୍ଦେଶ_row, ସନ୍ଦେଶ_row + 1,
		    cmdline_col_off + ସନ୍ଦେଶ_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', ସନ୍ଦେଶ_attr);
	    screen_fill(ସନ୍ଦେଶ_row + 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', ସନ୍ଦେଶ_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_clr_cmdline(ଶୂନ୍ୟ)
{
    ସନ୍ଦେଶ_row = cmdline_row;
    ସନ୍ଦେଶ_col = 0;
    ସନ୍ଦେଶ_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସନ୍ଦେଶ_end(ଶୂନ୍ୟ)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    ଯଦି (!exiting && need_wait_return && !(State & MODE_CMDLINE))
    {
	wait_return(FALSE);
	ଫେରନ୍ତୁ FALSE;
    }
    out_flush();
    ଫେରନ୍ତୁ TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_check(ଶୂନ୍ୟ)
{
    ଯଦି (ସନ୍ଦେଶ_row == Rows - 1 && ସନ୍ଦେଶ_col >= sc_col
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
    ସ୍ଥିର ଶୂନ୍ୟ
redir_write(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlen)
{
    char_u	*s = str;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cur_col = 0;

    // Don't do anything for displaying prompts and the like.
    ଯଦି (redir_off)
	ଫେରନ୍ତୁ;

    // If 'verbosefile' is set prepare for writing in that file.
    ଯଦି (*p_vfile != NUL && verbose_fd == ଖାଲି)
	verbose_open();

    ଯଦି (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	ଯଦି (*s != '\n' && *s != '\r')
	{
	    ଯେତେବେଳେ (cur_col < ସନ୍ଦେଶ_col)
	    {
#ifdef FEAT_EVAL
		ଯଦି (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		ଅନ୍ୟଥା ଯଦି (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		ଅନ୍ୟଥା ଯଦି (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		ଅନ୍ୟଥା
#endif
		    ଯଦି (redir_fd != ଖାଲି)
		    fputs(" ", redir_fd);
		ଯଦି (verbose_fd != ଖାଲି)
		    fputs(" ", verbose_fd);
		++cur_col;
	    }
	}

#ifdef FEAT_EVAL
	ଯଦି (redir_execute)
	    execute_redir_str(s, maxlen);
	ଅନ୍ୟଥା ଯଦି (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	ଅନ୍ୟଥା ଯଦି (redir_vname)
	    var_redir_str(s, maxlen);
#endif

	// Write and adjust the current column.
	ଯେତେବେଳେ (*s != NUL && (maxlen < 0 || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - str) < maxlen))
	{
#ifdef FEAT_EVAL
	    ଯଦି (!redir_reg && !redir_vname && !redir_execute)
#endif
		ଯଦି (redir_fd != ଖାଲି)
		    putc(*s, redir_fd);
	    ଯଦି (verbose_fd != ଖାଲି)
		putc(*s, verbose_fd);
	    ଯଦି (*s == '\r' || *s == '\n')
		cur_col = 0;
	    ଅନ୍ୟଥା ଯଦି (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    ଅନ୍ୟଥା
		++cur_col;
	    ++s;
	}

	ଯଦି (ସନ୍ଦେଶ_silent != 0)	// should update msg_col
	    ସନ୍ଦେଶ_col = cur_col;
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
redirecting(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ redir_fd != ଖାଲି || *p_vfile != NUL
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#endif
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    ଶୂନ୍ୟ
verbose_enter(ଶୂନ୍ୟ)
{
    ଯଦି (*p_vfile != NUL)
	++ସନ୍ଦେଶ_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    ଶୂନ୍ୟ
verbose_leave(ଶୂନ୍ୟ)
{
    ଯଦି (*p_vfile != NUL)
	ଯଦି (--ସନ୍ଦେଶ_silent < 0)
	    ସନ୍ଦେଶ_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    ଶୂନ୍ୟ
verbose_enter_scroll(ଶୂନ୍ୟ)
{
    ଯଦି (*p_vfile != NUL)
	++ସନ୍ଦେଶ_silent;
    ଅନ୍ୟଥା
	// always scroll up, don't overwrite
	ସନ୍ଦେଶ_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    ଶୂନ୍ୟ
verbose_leave_scroll(ଶୂନ୍ୟ)
{
    ଯଦି (*p_vfile != NUL)
    {
	ଯଦି (--ସନ୍ଦେଶ_silent < 0)
	    ସନ୍ଦେଶ_silent = 0;
    }
    ଅନ୍ୟଥା
	cmdline_row = ସନ୍ଦେଶ_row;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    ଶୂନ୍ୟ
verbose_stop(ଶୂନ୍ୟ)
{
    ଯଦି (verbose_fd != ଖାଲି)
    {
	fclose(verbose_fd);
	verbose_fd = ଖାଲି;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
verbose_open(ଶୂନ୍ୟ)
{
    ଯଦି (verbose_fd == ଖାଲି && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((ଅକ୍ଷର *)p_vfile, "a");
	ଯଦି (verbose_fd == ଖାଲି)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    ଶୂନ୍ୟ
give_warning(char_u *message, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hl)
{
    give_warning_with_source(message, hl, FALSE);
}

    ଶୂନ୍ୟ
give_warning_with_source(char_u *message, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା with_source)
{
    // Don't do this for ":silent".
    ଯଦି (ସନ୍ଦେଶ_silent != 0)
	ଫେରନ୍ତୁ;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, message, -1);
#endif
    VIM_CLEAR(keep_ସନ୍ଦେଶ);
    ଯଦି (hl)
	keep_ସନ୍ଦେଶ_attr = HL_ATTR(HLF_W);
    ଅନ୍ୟଥା
	keep_ସନ୍ଦେଶ_attr = 0;

    ଯଦି (with_source)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	ସନ୍ଦେଶ_start();
	ସନ୍ଦେଶ_source(HL_ATTR(HLF_W));
	ସନ୍ଦେଶ_puts(" ");
	ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)message, HL_ATTR(HLF_W) | ସନ୍ଦେଶ_HIST);
	ସନ୍ଦେଶ_clr_eos();
	(ଶୂନ୍ୟ)ସନ୍ଦେଶ_end();
    }
    ଅନ୍ୟଥା ଯଦି (ସନ୍ଦେଶ_attr((ଅକ୍ଷର *)message, keep_ସନ୍ଦେଶ_attr) && ସନ୍ଦେଶ_scrolled == 0)
	set_keep_ସନ୍ଦେଶ(message, keep_ସନ୍ଦେଶ_attr);

    ସନ୍ଦେଶ_didout = FALSE;	    // overwrite this message
    ସନ୍ଦେଶ_nowait = TRUE;	    // don't wait for this message
    ସନ୍ଦେଶ_col = 0;

    --no_wait_return;
}

#ଯଦି defined(FEAT_EVAL)
    ଶୂନ୍ୟ
give_warning2(char_u *message, char_u *a1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hl)
{
    ଯଦି (IObuff == ଖାଲି)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(message, hl);
    }
    ଅନ୍ୟଥା
    {
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, (ଅକ୍ଷର *)message, a1);
	give_warning(IObuff, hl);
    }
}
#endif

/*
 * Advance msg cursor to column "col".
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_advance(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ଯଦି (ସନ୍ଦେଶ_silent != 0)	// nothing to advance to
    {
	ସନ୍ଦେଶ_col = col;		// for redirection, may fill it up later
	ଫେରନ୍ତୁ;
    }
    ଯଦି (col >= cmdline_width)	// not enough room
	col = cmdline_width - 1;
#ifdef FEAT_RIGHTLEFT
    ଯଦି (cmdmsg_rl)
	ଯେତେବେଳେ (ସନ୍ଦେଶ_col > cmdline_width - col)
	    ସନ୍ଦେଶ_putchar(' ');
    ଅନ୍ୟଥା
#endif
	ଯେତେବେଳେ (ସନ୍ଦେଶ_col < col)
	    ସନ୍ଦେଶ_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    ଶୂନ୍ୟ
ସନ୍ଦେଶ_warn_missing_clipboard(ଶୂନ୍ୟ)
{
    ଯଦି (!global_busy && !did_warn_clipboard && silence_w23_w24_ସନ୍ଦେଶ == 0)
    {
#ifdef FEAT_CLIPBOARD
	ସନ୍ଦେଶ(_("W23: Clipboard register not available, using register 0"));
#ଅନ୍ୟଥା
	ସନ୍ଦେଶ(_("W24: Clipboard register not available. See :h W24"));
#endif
	did_warn_clipboard = TRUE;
    }
}

#ଯଦି defined(FEAT_CON_DIALOG)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_dialog(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type UNUSED,
    char_u	*title UNUSED,
    char_u	*message,
    char_u	*buttons,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dfltbutton,
    char_u	*textfield UNUSED,	// IObuff for inputdialog(), NULL
					// otherwise
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oldState;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = 0;
    char_u	*hotkeys;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    ଯଦି (silent_mode)
	ଫେରନ୍ତୁ dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    ଯଦି (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == ଖାଲି)
    {
	// --gui-dialog-file: write text to a file
	ଯଦି (gui_dialog_log(title, message))
	    c = dfltbutton;
	ଅନ୍ୟଥା
	    c = gui_mch_dialog(type, title, message, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_row = ସନ୍ଦେଶ_row;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	out_flush();
	gui_mch_update();

	ଫେରନ୍ତୁ c;
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
    hotkeys = ସନ୍ଦେଶ_show_console_dialog(message, buttons, dfltbutton);

    ଯଦି (hotkeys != ଖାଲି)
    {
	ପାଇଁ (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    ବାଛ (c)
	    {
	    ସ୍ଥିତି CAR:		// User accepts default option
	    ସ୍ଥିତି NL:
		retval = dfltbutton;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Ctrl_C:	// User aborts/cancels
	    ସ୍ଥିତି ESC:
		retval = 0;
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:		// Could be a hotkey?
		ଯଦି (c < 0)	// special keys are ignored here
		    ଜାରି;
		ଯଦି (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    ଭାଙ୍ଗ;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		ପାଇଁ (i = 0; hotkeys[i]; ++i)
		{
		    ଯଦି (has_mbyte)
		    {
			ଯଦି ((*mb_ptr2char)(hotkeys + i) == c)
			    ଭାଙ୍ଗ;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    ଅନ୍ୟଥା
			ଯଦି (hotkeys[i] == c)
			    ଭାଙ୍ଗ;
		    ++retval;
		}
		ଯଦି (hotkeys[i])
		    ଭାଙ୍ଗ;
		// No hotkey match, so keep waiting
		ଜାରି;
	    }
	    ଭାଙ୍ଗ;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    ସନ୍ଦେଶ_end_prompt();

    ଫେରନ୍ତୁ retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copy_char(
    char_u	*from,
    char_u	*to,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lowercase)	// make character lower case
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (has_mbyte)
    {
	ଯଦି (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    ଫେରନ୍ତୁ (*mb_char2bytes)(c, to);
	}
	ଅନ୍ୟଥା
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(to, from, (size_t)len);
	    ଫେରନ୍ତୁ len;
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (lowercase)
	    *to = (char_u)TOLOWER_LOC(*from);
	ଅନ୍ୟଥା
	    *to = *from;
	ଫେରନ୍ତୁ 1;
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
    ସ୍ଥିର char_u *
ସନ୍ଦେଶ_show_console_dialog(
    char_u	*message,
    char_u	*buttons,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dfltbutton)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = ଖାଲି;
    char_u	*msgp = ଖାଲି;
    char_u	*hotkp = ଖାଲି;
    char_u	*r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first_hotkey = FALSE;	// first char of button is hotkey
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    ପାଇଁ (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	ଯେତେବେଳେ (*r)
	{
	    ଯଦି (*r == DLG_BUTTON_SEP)
	    {
		ଯଦି (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    ଯଦି (has_mbyte)
			hotkp += STRLEN(hotkp);
		    ଅନ୍ୟଥା
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    ଯଦି (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    ଯଦି (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			first_hotkey = TRUE;
		}
		ଅନ୍ୟଥା
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    ଯଦି (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (*r == DLG_HOTKEY_CHAR || first_hotkey)
	    {
		ଯଦି (*r == DLG_HOTKEY_CHAR)
		    ++r;
		first_hotkey = FALSE;
		ଯଦି (copy)
		{
		    ଯଦି (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    ଅନ୍ୟଥା
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		ଅନ୍ୟଥା
		{
		    ++len;	    // '&a' -> '[a]'
		    ଯଦି (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		// everything else copy literally
		ଯଦି (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	ଯଦି (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	ଅନ୍ୟଥା
	{
	    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(STRLEN(message)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    ଯଦି (!has_hotkey[0])
	    {
		first_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_ସନ୍ଦେଶ);
	    confirm_ସନ୍ଦେଶ = alloc(len);
	    ଯଦି (confirm_ସନ୍ଦେଶ == ଖାଲି)
		ଫେରନ୍ତୁ ଖାଲି;
	    *confirm_ସନ୍ଦେଶ = NUL;
	    hotk = alloc(lenhotkey);
	    ଯଦି (hotk == ଖାଲି)
		ଫେରନ୍ତୁ ଖାଲି;

	    *confirm_ସନ୍ଦେଶ = '\n';
	    STRCPY(confirm_ସନ୍ଦେଶ + 1, message);

	    msgp = confirm_ସନ୍ଦେଶ + 1 + STRLEN(message);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_ସନ୍ଦେଶ_tail = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_ସନ୍ଦେଶ();
    ଫେରନ୍ତୁ hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
display_confirm_ସନ୍ଦେଶ(ଶୂନ୍ୟ)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_ସନ୍ଦେଶ_used;
    ଯଦି (confirm_ସନ୍ଦେଶ != ଖାଲି)
	ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)confirm_ସନ୍ଦେଶ, HL_ATTR(HLF_M));
    --confirm_ସନ୍ଦେଶ_used;
}

#endif // FEAT_CON_DIALOG

#ଯଦି defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_dialog_yesno(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,
    char_u	*title,
    char_u	*message,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dflt)
{
    ଯଦି (do_dialog(type,
		title == ଖାଲି ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No"), dflt, ଖାଲି, FALSE) == 1)
	ଫେରନ୍ତୁ VIM_YES;
    ଫେରନ୍ତୁ VIM_NO;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_dialog_yesnocancel(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,
    char_u	*title,
    char_u	*message,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dflt)
{
    ବାଛ (do_dialog(type,
		title == ଖାଲି ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, ଖାଲି, FALSE))
    {
	ସ୍ଥିତି 1: ଫେରନ୍ତୁ VIM_YES;
	ସ୍ଥିତି 2: ଫେରନ୍ତୁ VIM_NO;
    }
    ଫେରନ୍ତୁ VIM_CANCEL;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_dialog_yesnoallcancel(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,
    char_u	*title,
    char_u	*message,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dflt)
{
    ବାଛ (do_dialog(type,
		title == ଖାଲି ? (char_u *)"Question" : title,
		message,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, ଖାଲି, FALSE))
    {
	ସ୍ଥିତି 1: ଫେରନ୍ତୁ VIM_YES;
	ସ୍ଥିତି 2: ଫେରନ୍ତୁ VIM_NO;
	ସ୍ଥିତି 3: ଫେରନ୍ତୁ VIM_ALL;
	ସ୍ଥିତି 4: ଫେରନ୍ତୁ VIM_DISCARDALL;
    }
    ଫେରନ୍ତୁ VIM_CANCEL;
}

#endif // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
