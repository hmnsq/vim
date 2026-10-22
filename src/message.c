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

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_FILE		// don't include prototype for smsg()

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ ಶೂನ್ಯ ಕೂಡಿಸು_ಸಂದೇಶ_hist(char_u *s, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ check_ಸಂದೇಶ_hist(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ದೊರಕಿತು_return_ಸಂದೇಶ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_home_replace_attr(char_u *fname, ಪೂರ್ಣಾಂಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_puts_attr_len(ಅಕ್ಷರ *str, ಪೂರ್ಣಾಂಕ maxlen, ಪೂರ್ಣಾಂಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_puts_display(char_u *str, ಪೂರ್ಣಾಂಕ maxlen, ಪೂರ್ಣಾಂಕ attr, ಪೂರ್ಣಾಂಕ recurse);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_scroll_ಮೇಲೆ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ inc_ಸಂದೇಶ_scrolled(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ store_sb_ಪಠ್ಯ(char_u **sb_str, char_u *s, ಪೂರ್ಣಾಂಕ attr, ಪೂರ್ಣಾಂಕ *sb_ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಮುಕ್ತಾಯ);
ಸ್ಥಿರ ಶೂನ್ಯ t_puts(ಪೂರ್ಣಾಂಕ *t_ಲಂಬಸಾಲು, char_u *t_s, char_u *s, ಪೂರ್ಣಾಂಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_puts_printf(char_u *str, ಪೂರ್ಣಾಂಕ maxlen);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_ಇನ್ನಷ್ಟು_prompt(ಪೂರ್ಣಾಂಕ typed_char);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_screen_putchar(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_moremsg(ಪೂರ್ಣಾಂಕ ತುಂಬಿದ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ಸಂದೇಶ_check_screen(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ redir_write(char_u *s, ಪೂರ್ಣಾಂಕ maxlen);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CON_DIALOG
ಸ್ಥಿರ char_u *ಸಂದೇಶ_show_console_dialog(char_u *ಸಂದೇಶ_2, char_u *buttons, ಪೂರ್ಣಾಂಕ dfltbutton);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	confirm_ಸಂದೇಶ_ಬಳಸಿದ = FALSE;	// displaying confirm_msg
ಸ್ಥಿರ char_u	*confirm_ಸಂದೇಶ = ಖಾಲಿ;		// ":confirm" message
ಸ್ಥಿರ char_u	*confirm_ಸಂದೇಶ_ಬಾಲ;		// tail of confirm_msg
ಸ್ಥಿರ ಶೂನ್ಯ display_confirm_ಸಂದೇಶ(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ emsg_ಗಮ್ಯ_channel_log = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ ಸಂದೇಶ_hist
{
    ರಚನೆ ಸಂದೇಶ_hist	*ಮುಂದಿನ;
    char_u		*ಸಂದೇಶ;
    ಪೂರ್ಣಾಂಕ			attr;
};

ಸ್ಥಿರ ರಚನೆ ಸಂದೇಶ_hist *ಮೊದಲ_ಸಂದೇಶ_hist = ಖಾಲಿ;
ಸ್ಥಿರ ರಚನೆ ಸಂದೇಶ_hist *ಕೊನೆಯ_ಸಂದೇಶ_hist = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_hist_len = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#ವ್ಯಾಖ್ಯಾನಿಸು MESSAGES_ದೊರಕಿತು_ENTER	0x001
#ವ್ಯಾಖ್ಯಾನಿಸು MESSAGES_WAIT		0x002
#ವ್ಯಾಖ್ಯಾನಿಸು MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#ವ್ಯಾಖ್ಯಾನಿಸು MESSAGES_OPT_ದೊರಕಿತು_ENTER "hit-enter"
#ವ್ಯಾಖ್ಯಾನಿಸು MESSAGES_OPT_WAIT "wait:"
#ವ್ಯಾಖ್ಯಾನಿಸು MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_flags = MESSAGES_ದೊರಕಿತು_ENTER | MESSAGES_HISTORY;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_wait = 0;

ಸ್ಥಿರ FILE *verbose_fd = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  verbose_did_open = FALSE;

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
    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ(ಅಕ್ಷರ *s)
{
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    ಪೂರ್ಣಾಂಕ
verb_ಸಂದೇಶ(ಅಕ್ಷರ *s)
{
    ಪೂರ್ಣಾಂಕ		n;

    verbose_enter();
    n = ಸಂದೇಶ_attr_keep(s, 0, FALSE);
    verbose_leave();

    ಹಿಂತಿರುಗಿ n;
}

    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_attr(ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ attr)
{
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ_attr_keep(s, attr, FALSE);
}

    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_attr_keep(
    ಅಕ್ಷರ	*s,
    ಪೂರ್ಣಾಂಕ		attr,
    ಪೂರ್ಣಾಂಕ		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	entered = 0;
    ಪೂರ್ಣಾಂಕ		retval;
    char_u	*buf = ಖಾಲಿ;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    ಆದರೆ (!emsg_on_display && ಸಂದೇಶ_filtered((char_u *)s))
	ಹಿಂತಿರುಗಿ TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (attr == 0)
	ಗಣ_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    ಆದರೆ (entered >= 3)
	ಹಿಂತಿರುಗಿ TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    ಆದರೆ ((char_u *)s != keep_ಸಂದೇಶ
	    || (*s != '<'
		&& ಕೊನೆಯ_ಸಂದೇಶ_hist != ಖಾಲಿ
		&& ಕೊನೆಯ_ಸಂದೇಶ_hist->ಸಂದೇಶ != ಖಾಲಿ
		&& STRCMP(s, ಕೊನೆಯ_ಸಂದೇಶ_hist->ಸಂದೇಶ) != 0))
	ಕೂಡಿಸು_ಸಂದೇಶ_hist((char_u *)s, -1, attr);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (emsg_ಗಮ್ಯ_channel_log)
	// Write message in the channel log.
	ch_log(ಖಾಲಿ, "ERROR: %s", s);
#ಆದರೆ_ಅಂತ್ಯ

    // Truncate the message if needed.
    ಸಂದೇಶ_ಆರಂಭ();
    buf = ಸಂದೇಶ_strtrunc((char_u *)s, FALSE);
    ಆದರೆ (buf != ಖಾಲಿ)
	s = (ಅಕ್ಷರ *)buf;

    ಸಂದೇಶ_outtrans_attr((char_u *)s, attr);
    ಸಂದೇಶ_clr_eos();
    retval = ಸಂದೇಶ_ಕೊನೆ();

    ಆದರೆ (keep && retval && vim_strsize((char_u *)s)
		    < (ಪೂರ್ಣಾಂಕ)(ಅಡ್ಡಸಾಲುಗಳು - cmdline_ಅಡ್ಡಸಾಲು - 1) * cmdline_ಅಗಲ + sc_ಲಂಬಸಾಲು)
	ಗಣ_keep_ಸಂದೇಶ((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
ಸಂದೇಶ_strtrunc(
    char_u	*s,
    ಪೂರ್ಣಾಂಕ		force)	    // always truncate
{
    char_u	*buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ;

    // May truncate message to avoid a hit-return prompt
    ಆದರೆ ((!ಸಂದೇಶ_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_ಸಕ್ರಿಯ && ಸಂದೇಶ_silent == 0) || force)
    {
	len = vim_strsize(s);
	ಆದರೆ (ಸಂದೇಶ_scrolled != 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
		|| ಒಳ_echowindow
#ಆದರೆ_ಅಂತ್ಯ
		)
	    // Use all the columns.
	    ಸ್ಥಳ = (ಪೂರ್ಣಾಂಕ)(ಅಡ್ಡಸಾಲುಗಳು - ಸಂದೇಶ_ಅಡ್ಡಸಾಲು) * cmdline_ಅಗಲ - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    // Use up to 'showcmd' column.
	    ಸ್ಥಳ = (ಪೂರ್ಣಾಂಕ)(ಅಡ್ಡಸಾಲುಗಳು - ಸಂದೇಶ_ಅಡ್ಡಸಾಲು - 1) * cmdline_ಅಗಲ + sc_ಲಂಬಸಾಲು - 1;
	ಆದರೆ (len > ಸ್ಥಳ && ಸ್ಥಳ > 0)
	{
	    ಆದರೆ (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (ಸ್ಥಳ + 2) * 18;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (ಸ್ಥಳ + 2) * 2;
	    ಇಲ್ಲದಿದ್ದರೆ
		len = ಸ್ಥಳ + 2;
	    buf = alloc(len);
	    ಆದರೆ (buf != ಖಾಲಿ)
		trunc_string(s, buf, ಸ್ಥಳ, len);
	}
    }
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    ಶೂನ್ಯ
trunc_string(
    char_u	*s,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ_ಒಳ,
    ಪೂರ್ಣಾಂಕ		buflen)
{
    size_t	ಸ್ಥಳ = ಸ್ಥಳ_ಒಳ - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    ಪೂರ್ಣಾಂಕ		e;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (*s == NUL)
    {
	ಆದರೆ (buflen > 0)
	    *buf = NUL;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಸ್ಥಳ_ಒಳ < 3)
	ಸ್ಥಳ = 0;
    half = ಸ್ಥಳ / 2;

    // First part: Start of the string.
    ಗಾಗಿ (e = 0; len < half && e < buflen; ++e)
    {
	ಆದರೆ (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    ಹಿಂತಿರುಗಿ;
	}
	n = ptr2cells(s + e);
	ಆದರೆ (len + n > half)
	    ನಿಲ್ಲಿಸು;
	len += n;
	buf[e] = s[e];
	ಆದರೆ (has_mbyte)
	    ಗಾಗಿ (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		ಆದರೆ (++e == buflen)
		    ನಿಲ್ಲಿಸು;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    ಆದರೆ (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	ಆಗ (len + n > ಸ್ಥಳ)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (ಪೂರ್ಣಾಂಕ)STRLEN(s);
	ಗಾಗಿ (;;)
	{
	    ಮಾಡು
		half = half - utf_ತಲೆ_off(s, s + half - 1) - 1;
	    ಆಗ (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    ಆದರೆ (len + n > ಸ್ಥಳ || half == 0)
		ನಿಲ್ಲಿಸು;
	    len += n;
	    i = (ಪೂರ್ಣಾಂಕ)half;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (i = (ಪೂರ್ಣಾಂಕ)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= ಸ್ಥಳ; --i)
	    len += n;
    }


    ಆದರೆ (i <= e + 3)
    {
	// text fits without truncating
	ಆದರೆ (s != buf)
	{
	    len = STRLEN(s);
	    ಆದರೆ (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    ಆದರೆ (len < 1)
		buf[e - 1] = NUL;
	    ಇಲ್ಲದಿದ್ದರೆ
		mch_memmove(buf + e, s + e, len);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	ಆದರೆ (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO

ಪೂರ್ಣಾಂಕ vim_snprintf(ಅಕ್ಷರ *str, size_t str_m, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...);

    ಪೂರ್ಣಾಂಕ
smsg(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ...)
{
    ಆದರೆ (IObuff == ಖಾಲಿ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ಹಿಂತಿರುಗಿ ಸಂದೇಶ((ಅಕ್ಷರ *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ((ಅಕ್ಷರ *)IObuff);
}

    ಪೂರ್ಣಾಂಕ
smsg_attr(ಪೂರ್ಣಾಂಕ attr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ...)
{
    ಆದರೆ (IObuff == ಖಾಲಿ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ಹಿಂತಿರುಗಿ ಸಂದೇಶ_attr((ಅಕ್ಷರ *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ_attr((ಅಕ್ಷರ *)IObuff, attr);
}

    ಪೂರ್ಣಾಂಕ
smsg_attr_keep(ಪೂರ್ಣಾಂಕ attr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ...)
{
    ಆದರೆ (IObuff == ಖಾಲಿ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ಹಿಂತಿರುಗಿ ಸಂದೇಶ_attr_keep((ಅಕ್ಷರ *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ_attr_keep((ಅಕ್ಷರ *)IObuff, attr, TRUE);
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಕೊನೆಯ_sourcing_lnum = 0;
ಸ್ಥಿರ char_u   *ಕೊನೆಯ_sourcing_name = ಖಾಲಿ;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    ಶೂನ್ಯ
reset_ಕೊನೆಯ_sourcing(ಶೂನ್ಯ)
{
    VIM_CLEAR(ಕೊನೆಯ_sourcing_name);
    ಕೊನೆಯ_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
other_sourcing_name(ಶೂನ್ಯ)
{
    ಆದರೆ (HAVE_SOURCING_INFO && SOURCING_NAME != ಖಾಲಿ)
    {
	ಆದರೆ (ಕೊನೆಯ_sourcing_name != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ STRCMP(SOURCING_NAME, ಕೊನೆಯ_sourcing_name) != 0;
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ಸ್ಥಿರ char_u *
get_emsg_ಮೂಲ(ಶೂನ್ಯ)
{
    char_u	*Buf, *p;

    ಆದರೆ (HAVE_SOURCING_INFO && SOURCING_NAME != ಖಾಲಿ && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	ಆದರೆ (sname == ಖಾಲಿ)
	    sname = SOURCING_NAME;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	ಆದರೆ (Buf != ಖಾಲಿ)
	    sprintf((ಅಕ್ಷರ *)Buf, (ಅಕ್ಷರ *)p, sname);
	vim_free(tofree);
	ಹಿಂತಿರುಗಿ Buf;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
    ಆದರೆ (SOURCING_NAME != ಖಾಲಿ
	    && (other_sourcing_name() || SOURCING_LNUM != ಕೊನೆಯ_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	ಆದರೆ (Buf != ಖಾಲಿ)
	    sprintf((ಅಕ್ಷರ *)Buf, (ಅಕ್ಷರ *)p, (ದೀರ್ಘ)SOURCING_LNUM);
	ಹಿಂತಿರುಗಿ Buf;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_ಮೂಲ(ಪೂರ್ಣಾಂಕ attr)
{
    char_u	*p;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = FALSE;

    // Bail out if something called here causes an error.
    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_ಮೂಲ();
    ಆದರೆ (p != ಖಾಲಿ)
    {
	ಸಂದೇಶ_scroll = TRUE;  // this will take more than one line
	ಸಂದೇಶ_attr((ಅಕ್ಷರ *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    ಆದರೆ (p != ಖಾಲಿ)
    {
	ಸಂದೇಶ_attr((ಅಕ್ಷರ *)p, HL_ATTR(HLF_N));
	vim_free(p);
	ಕೊನೆಯ_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    ಆದರೆ (SOURCING_NAME == ಖಾಲಿ || other_sourcing_name())
    {
	VIM_CLEAR(ಕೊನೆಯ_sourcing_name);
	ಆದರೆ (SOURCING_NAME != ಖಾಲಿ)
	    ಕೊನೆಯ_sourcing_name = vim_strsave(SOURCING_NAME);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
emsg_not_now(ಶೂನ್ಯ)
{
    ಆದರೆ ((emsg_off > 0 && vim_strchr(p_debug, 'm') == ಖಾಲಿ
					  && vim_strchr(p_debug, 't') == ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    || emsg_skip > 0
#ಆದರೆ_ಅಂತ್ಯ
	    )
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ defined(FEAT_EVAL)
ಸ್ಥಿರ garray_T ignore_error_list = GA_ಖಾಲಿಗಳು;

    ಶೂನ್ಯ
ignore_error_for_testing(char_u *error)
{
    ಆದರೆ (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, ಗಾತ್ರ(char_u *), 1);

    ಆದರೆ (STRCMP("RESET", error) == 0)
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ignore_error_list);
    ಇಲ್ಲದಿದ್ದರೆ
	ga_copy_string(&ignore_error_list, error);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ignore_error(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < ignore_error_list.ga_len; ++i)
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(ಸಂದೇಶ,
		  (ಅಕ್ಷರ *)((char_u **)(ignore_error_list.ga_ದತ್ತಾಂಶ))[i]) != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    ಶೂನ್ಯ
do_perror(ಅಕ್ಷರ *ಸಂದೇಶ)
{
    perror(ಸಂದೇಶ);
    ++emsg_silent;
    emsg(ಸಂದೇಶ);
    --emsg_silent;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * emsg_core() - display an error message
 *
 * Rings the bell, if appropriate, and calls message() to do the real work
 * When terminal not initialized (yet) mch_errmsg(..) is used.
 *
 * Return TRUE if wait_return() not called.
 * Note: caller must check 'emsg_not_now()' before calling this.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
emsg_core(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
    ಪೂರ್ಣಾಂಕ		attr;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		r;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		ignore = FALSE;
    ಪೂರ್ಣಾಂಕ		severe;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // When testing some errors are turned into a normal message.
    ಆದರೆ (ignore_error(s))
	// don't call msg() if it results in a dialog
	ಹಿಂತಿರುಗಿ ಸಂದೇಶ_use_printf() ? FALSE : ಸಂದೇಶ((ಅಕ್ಷರ *)s);
#ಆದರೆ_ಅಂತ್ಯ

    ++called_emsg;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!emsg_off || vim_strchr(p_debug, 't') != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	ಆದರೆ (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    ಆದರೆ (!ignore)
		++did_emsg;
	    ಹಿಂತಿರುಗಿ TRUE;
	}

	ಆದರೆ (ಒಳ_assert_fails && emsg_assert_fails_ಸಂದೇಶ == ಖಾಲಿ)
	{
	    emsg_assert_fails_ಸಂದೇಶ = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == ಖಾಲಿ ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	ಗಣ_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	ಆದರೆ (emsg_silent != 0)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ++did_emsg_silent;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (emsg_noredir == 0)
	    {
		ಸಂದೇಶ_ಆರಂಭ();
		p = get_emsg_ಮೂಲ();
		ಆದರೆ (p != ಖಾಲಿ)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		ಆದರೆ (p != ಖಾಲಿ)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		redir_write((char_u *)s, -1);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    // Only increment did_emsg_def when :silent! wasn't used inside the
	    // :def function.
	    ಆದರೆ (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ch_log(ಖಾಲಿ, "ERROR silent: %s", s);
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	ಸಂದೇಶ_silent = 0;
	cmd_silent = FALSE;

	ಆದರೆ (global_busy)		// break :global command
	    ++global_busy;

	ಆದರೆ (p_eb)
	    beep_flush();		// also includes flush_buffers()
	ಇಲ್ಲದಿದ್ದರೆ
	    flush_buffers(FLUSH_ಕನಿಷ್ಠತಮ);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	++uncaught_emsg;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
    ಆದರೆ (!ಒಳ_echowindow)
#ಆದರೆ_ಅಂತ್ಯ
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    ಆದರೆ (ಸಂದೇಶ_scrolled != 0)
	need_wait_return = TRUE;    // needed in case emsg() is called after
				    // wait_return() has reset need_wait_return
				    // and a redraw is expected because
				    // msg_scrolled is non-zero

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    emsg_ಗಮ್ಯ_channel_log = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    /*
     * Display name and line number for the source of the error.
     */
    ಸಂದೇಶ_scroll = TRUE;
    ಸಂದೇಶ_ಮೂಲ(attr);

    /*
     * Display the error message itself.
     */
    ಸಂದೇಶ_nowait = FALSE;			// wait for this msg
    r = ಸಂದೇಶ_attr((ಅಕ್ಷರ *)s, attr);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    emsg_ಗಮ್ಯ_channel_log = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    ಪೂರ್ಣಾಂಕ
emsg(ಅಕ್ಷರ *s)
{
    // Skip this if not giving error messages at the moment.
    ಆದರೆ (emsg_not_now())
	ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ emsg_core(s);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    ಪೂರ್ಣಾಂಕ
semsg(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ...)
{
    // Skip this if not giving error messages at the moment.
    ಆದರೆ (emsg_not_now())
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (IObuff == ಖಾಲಿ)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ಹಿಂತಿರುಗಿ emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    ಹಿಂತಿರುಗಿ emsg_core((ಅಕ್ಷರ *)IObuff);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    ಶೂನ್ಯ
iemsg(ಅಕ್ಷರ *s)
{
    ಆದರೆ (emsg_not_now())
	ಹಿಂತಿರುಗಿ;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_ಆಂತರಿಕ_error_please_report_a_bug));

    emsg_core(s);
#ಆದರೆ defined(ABORT_ON_ಆಂತರಿಕ_ERROR) && defined(FEAT_EVAL)
    ಗಣ_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    ಸಂದೇಶ_putchar('\n');  // avoid overwriting the error message
    ಹೊರಹೋಗು_flush();
    abort();
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO  // manual proto with __attribute__
/*
 * Same as semsg(...) but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 * Note: caller must not pass 'IObuff' as 1st argument.
 */
    ಶೂನ್ಯ
siemsg(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ...)
{
    ಆದರೆ (emsg_not_now())
	ಹಿಂತಿರುಗಿ;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_ಆಂತರಿಕ_error_please_report_a_bug));

    ಆದರೆ (IObuff == ಖಾಲಿ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((ಅಕ್ಷರ *)IObuff);
    }
# ifdef ABORT_ON_ಆಂತರಿಕ_ERROR
    ಸಂದೇಶ_putchar('\n');  // avoid overwriting the error message
    ಹೊರಹೋಗು_flush();
    abort();
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Give an "Internal error" message.
 */
    ಶೂನ್ಯ
ಆಂತರಿಕ_error(ಅಕ್ಷರ *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_ಆಂತರಿಕ_error_please_report_a_bug));

    siemsg(_(e_ಆಂತರಿಕ_error_str), where);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    ಶೂನ್ಯ
ಆಂತರಿಕ_error_no_abort(ಅಕ್ಷರ *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_ಆಂತರಿಕ_error_please_report_a_bug));

    semsg(_(e_ಆಂತರಿಕ_error_str), where);
}
#ಆದರೆ_ಅಂತ್ಯ

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    ಶೂನ್ಯ
emsg_invreg(ಪೂರ್ಣಾಂಕ name)
{
    semsg(_(e_invalid_ರಿಜಿಸ್ಟರ್_name_str), transchar_buf(ಖಾಲಿ, name));
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    ಶೂನ್ಯ
emsg_namelen(ಅಕ್ಷರ *ಸಂದೇಶ, char_u *name, ಪೂರ್ಣಾಂಕ len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(ಸಂದೇಶ, copy == ಖಾಲಿ ? "NULL" : (ಅಕ್ಷರ *)copy);
    vim_free(copy);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    ಅಕ್ಷರ *
ಸಂದೇಶ_trunc_attr(ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ force, ಪೂರ್ಣಾಂಕ attr)
{
    ಪೂರ್ಣಾಂಕ		n;
    ಅಕ್ಷರ	*ts;

    // Add message to history before truncating
    ಕೂಡಿಸು_ಸಂದೇಶ_hist((char_u *)s, -1, attr);

    ts = (ಅಕ್ಷರ *)ಸಂದೇಶ_may_trunc(force, (char_u *)s);

    ಸಂದೇಶ_hist_off = TRUE;
    n = ಸಂದೇಶ_attr(ts, attr);
    ಸಂದೇಶ_hist_off = FALSE;

    ಆದರೆ (n)
	ಹಿಂತಿರುಗಿ ts;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
ಸಂದೇಶ_may_trunc(ಪೂರ್ಣಾಂಕ force, char_u *s)
{
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    ಸ್ಥಳ = (ಪೂರ್ಣಾಂಕ)(ಅಡ್ಡಸಾಲುಗಳು - cmdline_ಅಡ್ಡಸಾಲು - 1) * cmdline_ಅಗಲ + sc_ಲಂಬಸಾಲು - 1;
    ಆದರೆ (ಸ್ಥಳ > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_ಸಕ್ರಿಯ))
	    && (n = (ಪೂರ್ಣಾಂಕ)STRLEN(s) - ಸ್ಥಳ) > 0)
    {
	ಆದರೆ (has_mbyte)
	{
	    ಪೂರ್ಣಾಂಕ	ಗಾತ್ರ_2 = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    ಆದರೆ (ಗಾತ್ರ_2 <= ಸ್ಥಳ)
		ಹಿಂತಿರುಗಿ s;

	    ಗಾಗಿ (n = 0; ಗಾತ್ರ_2 >= ಸ್ಥಳ; )
	    {
		ಗಾತ್ರ_2 -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    ಹಿಂತಿರುಗಿ s;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_ಸಂದೇಶ_hist(
    char_u	*s,
    ಪೂರ್ಣಾಂಕ		len,		// -1 for undetermined length
    ಪೂರ್ಣಾಂಕ		attr)
{
    ರಚನೆ ಸಂದೇಶ_hist *p;

    ಆದರೆ (ಸಂದೇಶ_hist_off || ಸಂದೇಶ_silent != 0)
	ಹಿಂತಿರುಗಿ;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(ರಚನೆ ಸಂದೇಶ_hist);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (len < 0)
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(s);
    // remove leading and trailing newlines
    ಆಗ (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    ಆಗ (len > 0 && s[len - 1] == '\n')
	--len;
    p->ಸಂದೇಶ = vim_strnsave(s, len);
    p->ಮುಂದಿನ = ಖಾಲಿ;
    p->attr = attr;
    ಆದರೆ (ಕೊನೆಯ_ಸಂದೇಶ_hist != ಖಾಲಿ)
	ಕೊನೆಯ_ಸಂದೇಶ_hist->ಮುಂದಿನ = p;
    ಕೊನೆಯ_ಸಂದೇಶ_hist = p;
    ಆದರೆ (ಮೊದಲ_ಸಂದೇಶ_hist == ಖಾಲಿ)
	ಮೊದಲ_ಸಂದೇಶ_hist = ಕೊನೆಯ_ಸಂದೇಶ_hist;
    ++ಸಂದೇಶ_hist_len;

    check_ಸಂದೇಶ_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    ಪೂರ್ಣಾಂಕ
delete_ಮೊದಲ_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ರಚನೆ ಸಂದೇಶ_hist *p;

    ಆದರೆ (ಸಂದೇಶ_hist_len <= 0)
	ಹಿಂತಿರುಗಿ FAIL;
    p = ಮೊದಲ_ಸಂದೇಶ_hist;
    ಮೊದಲ_ಸಂದೇಶ_hist = p->ಮುಂದಿನ;
    ಆದರೆ (ಮೊದಲ_ಸಂದೇಶ_hist == ಖಾಲಿ)
	ಕೊನೆಯ_ಸಂದೇಶ_hist = ಖಾಲಿ;  // history is empty
    vim_free(p->ಸಂದೇಶ);
    vim_free(p);
    --ಸಂದೇಶ_hist_len;
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
check_ಸಂದೇಶ_hist(ಶೂನ್ಯ)
{
    // Don't let the message history get too big
    ಆಗ (ಸಂದೇಶ_hist_len > 0 && ಸಂದೇಶ_hist_len > ಸಂದೇಶ_hist_max)
	(ಶೂನ್ಯ)delete_ಮೊದಲ_ಸಂದೇಶ();
}


    ಪೂರ್ಣಾಂಕ
messagesopt_ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		messages_flags_new = 0;
    ಪೂರ್ಣಾಂಕ		messages_wait_new = 0;
    ಪೂರ್ಣಾಂಕ		messages_history_new = 0;

    p = p_mopt;
    ಆಗ (*p != NUL)
    {
	ಆದರೆ (STRNCMP(p, MESSAGES_OPT_ದೊರಕಿತು_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_ದೊರಕಿತು_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_ದೊರಕಿತು_ENTER);
	    messages_flags_new |= MESSAGES_ದೊರಕಿತು_ENTER;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	ಆದರೆ (*p != ',' && *p != NUL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    ಆದರೆ (!(messages_flags_new & (MESSAGES_ದೊರಕಿತು_ENTER | MESSAGES_WAIT)))
	ಹಿಂತಿರುಗಿ FAIL;

    // "history" must be set
    ಆದರೆ (!(messages_flags_new & MESSAGES_HISTORY))
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (messages_history_new < 0 || messages_history_new > 10000)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (messages_wait_new < 0 || messages_wait_new > 10000)
	ಹಿಂತಿರುಗಿ FAIL;

    ಸಂದೇಶ_flags = messages_flags_new;
    ಸಂದೇಶ_wait = messages_wait_new;

    ಸಂದೇಶ_hist_max = messages_history_new;
    check_ಸಂದೇಶ_hist();

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * ":messages" command.
 */
    ಶೂನ್ಯ
ex_messages(exarg_T *eap)
{
    ರಚನೆ ಸಂದೇಶ_hist *p;
    char_u	    *s;
    ಪೂರ್ಣಾಂಕ		    c = 0;

    ಆದರೆ (STRCMP(eap->arg, "clear") == 0)
    {
	ಪೂರ್ಣಾಂಕ keep = eap->addr_ಎಣಿಕೆ == 0 ? 0 : eap->line2;

	ಆಗ (ಸಂದೇಶ_hist_len > keep)
	    (ಶೂನ್ಯ)delete_ಮೊದಲ_ಸಂದೇಶ();
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (*eap->arg != NUL)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ;
    }

    ಸಂದೇಶ_hist_off = TRUE;

    p = ಮೊದಲ_ಸಂದೇಶ_hist;
    ಆದರೆ (eap->addr_ಎಣಿಕೆ != 0)
    {
	// Count total messages
	ಗಾಗಿ (; p != ಖಾಲಿ && !got_int; p = p->ಮುಂದಿನ)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	ಗಾಗಿ (p = ಮೊದಲ_ಸಂದೇಶ_hist; p != ಖಾಲಿ && !got_int && c > 0;
						    p = p->ಮುಂದಿನ, c--);
    }

    ಆದರೆ (p == ಮೊದಲ_ಸಂದೇಶ_hist)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	s = get_mess_lang();
#ಇಲ್ಲದಿದ್ದರೆ
	s = mch_getenv((char_u *)"LANG");
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (s != ಖಾಲಿ && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    ಸಂದೇಶ_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    ಗಾಗಿ (; p != ಖಾಲಿ && !got_int; p = p->ಮುಂದಿನ)
	ಆದರೆ (p->ಸಂದೇಶ != ಖಾಲಿ)
	    ಸಂದೇಶ_attr((ಅಕ್ಷರ *)p->ಸಂದೇಶ, p->attr);

    ಸಂದೇಶ_hist_off = FALSE;
}

#ಆದರೆ defined(FEAT_CON_DIALOG) || defined(ಕಂಡುಹಿಡಿ_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_ಕೊನೆ_prompt(ಶೂನ್ಯ)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    ಸಂದೇಶ_clr_eos();
    ಸಾಲುಗಳು_ಎಡ = -1;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    ಶೂನ್ಯ
wait_return(ಪೂರ್ಣಾಂಕ redraw)
{
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		oldState;
    ಪೂರ್ಣಾಂಕ		tmpState;
    ಪೂರ್ಣಾಂಕ		had_got_int;
    ಪೂರ್ಣಾಂಕ		save_reg_recording;
    FILE	*save_scriptout;

    ಆದರೆ (redraw == TRUE)
	ಗಣ_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    ಆದರೆ (ಸಂದೇಶ_silent != 0)
	ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
    ಆದರೆ (ಒಳ_echowindow)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    ಆದರೆ (vgetc_busy > 0)
	ಹಿಂತಿರುಗಿ;
    need_wait_return = TRUE;
    ಆದರೆ (no_wait_return)
    {
	ಆದರೆ (!exmode_ಸಕ್ರಿಯ)
	    cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	ಹಿಂತಿರುಗಿ;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    ಆದರೆ (quit_ಇನ್ನಷ್ಟು)
    {
	c = CAR;		// just pretend CR was hit
	quit_ಇನ್ನಷ್ಟು = FALSE;
	got_int = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (exmode_ಸಕ್ರಿಯ)
    {
	ಸಂದೇಶ_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!stuff_ಖಾಲಿಗಳು())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = ಬಹುಲಕ_HITRETURN;
	setmouse();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#ಆದರೆ_ಅಂತ್ಯ
	cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	ಆದರೆ (need_check_timestamps)
	    check_timestamps(FALSE);

	ಆದರೆ (ಸಂದೇಶ_flags & MESSAGES_ದೊರಕಿತು_ENTER)
	{
	    ದೊರಕಿತು_return_ಸಂದೇಶ();

	    ಮಾಡು
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
		// Make sure to update image placements (since their row offset
		// will have been changed).
		draw_image_placements();
#ಆದರೆ_ಅಂತ್ಯ
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
		scriptout = ಖಾಲಿ;
		c = ಸುರಕ್ಷಿತ_vgetc();
		ಆದರೆ (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_ಕೀಲಿಗಳು;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		ಆದರೆ (ಕೀಲಿ_Typed && c == Ctrl_Y && clip_ನಕ್ಷತ್ರ.state == SELECT_ಮುಗಿದ)
		{
		    clip_copy_modeless_selection(TRUE);
		    c = K_IGNORE;
		}
#ಆದರೆ_ಅಂತ್ಯ

		/*
		* Allow scrolling back in the messages.
		* Also accept scroll-down commands when messages fill the
		* screen, to avoid that typing one 'j' too many makes the
		* messages disappear.
		*/
		ಆದರೆ (ಕೀಲಿ_Typed && p_ಇನ್ನಷ್ಟು && !p_cp)
		{
		    ಆದರೆ (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_ಮೇಲೆ || c == K_PAGEUP)
		    {
			ಆದರೆ (ಸಂದೇಶ_scrolled > ಅಡ್ಡಸಾಲುಗಳು)
			    // scroll back to show older messages
			    do_ಇನ್ನಷ್ಟು_prompt(c);
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಸಂದೇಶ_didout = FALSE;
			    c = K_IGNORE;
			    ಸಂದೇಶ_ಲಂಬಸಾಲು =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_ಅಗಲ - 1 :
#ಆದರೆ_ಅಂತ್ಯ
				0;
			}
			ಆದರೆ (quit_ಇನ್ನಷ್ಟು)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_ಇನ್ನಷ್ಟು = FALSE;
			    got_int = FALSE;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    ದೊರಕಿತು_return_ಸಂದೇಶ();
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಂದೇಶ_scrolled > ಅಡ್ಡಸಾಲುಗಳು - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_ಕೆಳಕ್ಕೆ || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } ಆಗ ((had_got_int && c == Ctrl_C)
				|| c == K_IGNORE
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
				|| c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
#ಆದರೆ_ಅಂತ್ಯ
				|| c == K_LEFTDRAG   || c == K_LEFTRELEASE
				|| c == K_MIDDLEDRAG || c == K_MIDDLERELEASE
				|| c == K_RIGHTDRAG  || c == K_RIGHTRELEASE
				|| c == K_MOUSELEFT  || c == K_MOUSERIGHT
				|| c == K_MOUSEDOWN  || c == K_MOUSEUP
				|| c == K_MOUSEMOVE
				|| (!mouse_has(MOUSE_RETURN)
				    && mouse_ಅಡ್ಡಸಾಲು < ಸಂದೇಶ_ಅಡ್ಡಸಾಲು
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    ಆದರೆ (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(ಶೂನ್ಯ)jump_ಗಮ್ಯ_mouse(MOUSE_SETPOS, ಖಾಲಿ, 0);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಕೀಲಿ_Typed || (vim_strchr((char_u *)"\r\n ", c) == ಖಾಲಿ
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_ಮುಖವಾಡ);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
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
    ಆದರೆ (c == ':' || c == '?' || c == '/')
    {
	ಆದರೆ (!exmode_ಸಕ್ರಿಯ)
	    cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	skip_redraw = TRUE;	    // skip redraw once
	do_redraw = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	skip_ಪದ_loop = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    ಆದರೆ (swapping_screen() && !termcap_ಸಕ್ರಿಯ)
	newline_on_exit = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    ಸಾಲುಗಳು_ಎಡ = -1;		// reset lines_left at next msg_start()
    reset_ಕೊನೆಯ_sourcing();
    ಆದರೆ (keep_ಸಂದೇಶ != ಖಾಲಿ && vim_strsize(keep_ಸಂದೇಶ) >=
			    (ಅಡ್ಡಸಾಲುಗಳು - cmdline_ಅಡ್ಡಸಾಲು - 1) * cmdline_ಅಗಲ + sc_ಲಂಬಸಾಲು)
	VIM_CLEAR(keep_ಸಂದೇಶ);	    // don't redisplay message, it's too long

    ಆದರೆ (tmpState == ಬಹುಲಕ_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!skip_redraw
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
ದೊರಕಿತು_return_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		save_p_ಇನ್ನಷ್ಟು = p_ಇನ್ನಷ್ಟು;

    p_ಇನ್ನಷ್ಟು = FALSE;	// don't want to see this message when scrolling back
    ಆದರೆ (ಸಂದೇಶ_didout)	// start on a new line
	ಸಂದೇಶ_putchar('\n');
    ಆದರೆ (got_int)
	ಸಂದೇಶ_puts(_("Interrupt: "));

    ಸಂದೇಶ_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    ಆದರೆ (!ಸಂದೇಶ_use_printf())
	ಸಂದೇಶ_clr_eos();
    p_ಇನ್ನಷ್ಟು = save_p_ಇನ್ನಷ್ಟು;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    ಶೂನ್ಯ
ಗಣ_keep_ಸಂದೇಶ(char_u *s, ಪೂರ್ಣಾಂಕ attr)
{
    vim_free(keep_ಸಂದೇಶ);
    ಆದರೆ (s != ಖಾಲಿ && ಸಂದೇಶ_silent == 0)
	keep_ಸಂದೇಶ = vim_strsave(s);
    ಇಲ್ಲದಿದ್ದರೆ
	keep_ಸಂದೇಶ = ಖಾಲಿ;
    keep_ಸಂದೇಶ_ಇನ್ನಷ್ಟು = FALSE;
    keep_ಸಂದೇಶ_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    ಶೂನ್ಯ
ಗಣ_keep_ಸಂದೇಶ_from_hist(ಶೂನ್ಯ)
{
    ಆದರೆ (keep_ಸಂದೇಶ == ಖಾಲಿ && ಕೊನೆಯ_ಸಂದೇಶ_hist != ಖಾಲಿ && ಸಂದೇಶ_scrolled == 0
						      && (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ))
	ಗಣ_keep_ಸಂದೇಶ(ಕೊನೆಯ_ಸಂದೇಶ_hist->ಸಂದೇಶ, ಕೊನೆಯ_ಸಂದೇಶ_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_ಆರಂಭ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		did_return = FALSE;

    ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು < cmdline_ಅಡ್ಡಸಾಲು)
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು;

    ಆದರೆ (!ಸಂದೇಶ_silent)
    {
	VIM_CLEAR(keep_ಸಂದೇಶ);
	need_fileinfo = FALSE;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	ಸಂದೇಶ_clr_eos();
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
    ಆದರೆ (ಒಳ_echowindow)
    {
	ಆದರೆ (popup_ಸಂದೇಶ_win_visible()
		    && ((ಸಂದೇಶ_ಲಂಬಸಾಲು > 0 && (ಸಂದೇಶ_scroll || !ತುಂಬಿದ_screen))
			|| ಒಳ_echowindow))
	{
	    win_T *wp = popup_get_ಸಂದೇಶ_win();

	    // start a new line
	    curbuf = wp->w_ಬಫರ್;
	    ml_append(wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_ಬಫರ್;
	}
	ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (!ಸಂದೇಶ_scroll && ತುಂಬಿದ_screen)	// overwrite last message
    {
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು;
	ಸಂದೇಶ_ಲಂಬಸಾಲು =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_ಅಗಲ - 1 :
#ಆದರೆ_ಅಂತ್ಯ
	    0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಂದೇಶ_didout || ಒಳ_echowindow)
    {
	// start message on next line
	ಸಂದೇಶ_putchar('\n');
	did_return = TRUE;
	ಆದರೆ (exmode_ಸಕ್ರಿಯ != EXMODE_ಸಾಮಾನ್ಯ)
	    cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
    }
    ಆದರೆ (!ಸಂದೇಶ_didany || ಸಾಲುಗಳು_ಎಡ < 0)
	ಸಂದೇಶ_starthere();
    ಆದರೆ (ಸಂದೇಶ_silent == 0)
    {
	ಸಂದೇಶ_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    ಆದರೆ (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_starthere(ಶೂನ್ಯ)
{
    ಸಾಲುಗಳು_ಎಡ = cmdline_ಅಡ್ಡಸಾಲು;
    ಸಂದೇಶ_didany = FALSE;
}

    ಶೂನ್ಯ
ಸಂದೇಶ_putchar(ಪೂರ್ಣಾಂಕ c)
{
    ಸಂದೇಶ_putchar_attr(c, 0);
}

    ಶೂನ್ಯ
ಸಂದೇಶ_putchar_attr(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    ಆದರೆ (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_ಎರಡನೇ(c);
	buf[2] = K_ಮೂರನೇ(c);
	buf[3] = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)buf, attr);
}

    ಶೂನ್ಯ
ಸಂದೇಶ_outnum(ದೀರ್ಘ n)
{
    ಅಕ್ಷರ	buf[20];

    sprintf(buf, "%ld", n);
    ಸಂದೇಶ_puts(buf);
}

    ಶೂನ್ಯ
ಸಂದೇಶ_home_replace(char_u *fname)
{
    ಸಂದೇಶ_home_replace_attr(fname, 0);
}

#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID)
    ಶೂನ್ಯ
ಸಂದೇಶ_home_replace_hl(char_u *fname)
{
    ಸಂದೇಶ_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_home_replace_attr(char_u *fname, ಪೂರ್ಣಾಂಕ attr)
{
    char_u	*name;

    name = home_replace_save(ಖಾಲಿ, fname);
    ಆದರೆ (name != ಖಾಲಿ)
	ಸಂದೇಶ_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_outtrans(char_u *str)
{
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ_outtrans_attr(str, 0);
}

    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_outtrans_attr(char_u *str, ಪೂರ್ಣಾಂಕ attr)
{
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ_outtrans_len_attr(str, (ಪೂರ್ಣಾಂಕ)STRLEN(str), attr);
}

    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_outtrans_len(char_u *str, ಪೂರ್ಣಾಂಕ len)
{
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
ಸಂದೇಶ_outtrans_one(char_u *p, ಪೂರ್ಣಾಂಕ attr)
{
    ಪೂರ್ಣಾಂಕ		l;

    ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	ಸಂದೇಶ_outtrans_len_attr(p, l, attr);
	ಹಿಂತಿರುಗಿ p + l;
    }
    ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)transchar_byte_buf(ಖಾಲಿ, *p), attr);
    ಹಿಂತಿರುಗಿ p + 1;
}

    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_outtrans_len_attr(char_u *msgstr, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ attr)
{
    ಪೂರ್ಣಾಂಕ		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_ಆರಂಭ = msgstr;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		mb_l;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    ಆದರೆ (attr == 0)
	attr = HL_ATTR(HLF_ಸಂದೇಶ);

    // if MSG_HIST flag set, add message to history
    ಆದರೆ (attr & ಸಂದೇಶ_HIST)
    {
	ಕೂಡಿಸು_ಸಂದೇಶ_hist(str, len, attr);
	attr &= ~ಸಂದೇಶ_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    ಆದರೆ (ಸಂದೇಶ_silent == 0 && len > 0 && ಸಂದೇಶ_ಅಡ್ಡಸಾಲು >= cmdline_ಅಡ್ಡಸಾಲು && ಸಂದೇಶ_ಲಂಬಸಾಲು == 0)
    {
	clear_cmdline = FALSE;
	ಬಹುಲಕ_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    ಆದರೆ (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	ಸಂದೇಶ_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    ಆಗ (--len >= 0 && !got_int)
    {
	ಆದರೆ (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	ಇಲ್ಲದಿದ್ದರೆ
	    mb_l = 1;
	ಆದರೆ (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    ಆದರೆ (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		ಆದರೆ (str > plain_ಆರಂಭ)
		    ಸಂದೇಶ_puts_attr_len((ಅಕ್ಷರ *)plain_ಆರಂಭ,
					       (ಪೂರ್ಣಾಂಕ)(str - plain_ಆರಂಭ), attr);
		plain_ಆರಂಭ = str + mb_l;
		ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)transchar_buf(ಖಾಲಿ, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    s = transchar_byte_buf(ಖಾಲಿ, *str);
	    ಆದರೆ (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		ಆದರೆ (str > plain_ಆರಂಭ)
		    ಸಂದೇಶ_puts_attr_len((ಅಕ್ಷರ *)plain_ಆರಂಭ,
					       (ಪೂರ್ಣಾಂಕ)(str - plain_ಆರಂಭ), attr);
		plain_ಆರಂಭ = str + 1;
		ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (ಪೂರ್ಣಾಂಕ)STRLEN(s);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		++retval;
	    ++str;
	}
    }

    ಆದರೆ (str > plain_ಆರಂಭ && !got_int)
	// print the printable chars at the end
	ಸಂದೇಶ_puts_attr_len((ಅಕ್ಷರ *)plain_ಆರಂಭ, (ಪೂರ್ಣಾಂಕ)(str - plain_ಆರಂಭ), attr);

    got_int |= save_got_int;

    ಹಿಂತಿರುಗಿ retval;
}

#ಆದರೆ defined(FEAT_QUICKFIX)
    ಶೂನ್ಯ
ಸಂದೇಶ_make(char_u *arg)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಸ್ಥಿರ char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    ಗಾಗಿ (i = 5; *arg && i >= 0; --i)
	ಆದರೆ (*arg++ != str[i])
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (i < 0)
    {
	ಸಂದೇಶ_putchar('\n');
	ಗಾಗಿ (i = 0; rs[i]; ++i)
	    ಸಂದೇಶ_putchar(rs[i] - 3);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_outtrans_special(
    char_u	*strstart,
    ಪೂರ್ಣಾಂಕ		from,	// TRUE for lhs of a mapping
    ಪೂರ್ಣಾಂಕ		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    ಪೂರ್ಣಾಂಕ		retval = 0;
    ಅಕ್ಷರ	*ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		attr;
    ಪೂರ್ಣಾಂಕ		len;

    attr = HL_ATTR(HLF_8);
    ಆಗ (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	ಆದರೆ ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    ಪಠ್ಯ = "<Space>";
	    ++str;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪಠ್ಯ = (ಅಕ್ಷರ *)str2special(&str, from, FALSE);
	ಆದರೆ (ಪಠ್ಯ[0] != NUL && ಪಠ್ಯ[1] == NUL)
	    // single-byte character or illegal byte
	    ಪಠ್ಯ = (ಅಕ್ಷರ *)transchar_byte_buf(ಖಾಲಿ, (char_u)ಪಠ್ಯ[0]);
	len = vim_strsize((char_u *)ಪಠ್ಯ);
	ಆದರೆ (maxlen > 0 && retval + len >= maxlen)
	    ನಿಲ್ಲಿಸು;
	// Highlight special keys
	ಸಂದೇಶ_puts_attr(ಪಠ್ಯ, len > 1
		&& (*mb_ptr2len)((char_u *)ಪಠ್ಯ) <= 1 ? attr : 0);
	retval += len;
    }
    ಹಿಂತಿರುಗಿ retval;
}

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    ಪೂರ್ಣಾಂಕ	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    ಪೂರ್ಣಾಂಕ	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    ಆಗ (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    ಹಿಂತಿರುಗಿ (char_u *)ga.ga_ದತ್ತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return the printable string for the key codes at "*sp".
 * On illegal byte return a string with only that byte.
 * Used for translating the lhs or rhs of a mapping to printable chars.
 * Advances "sp" to the next code.
 */
    char_u *
str2special(
    char_u	**sp,
    ಪೂರ್ಣಾಂಕ		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    ಪೂರ್ಣಾಂಕ		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    ಪೂರ್ಣಾಂಕ			c;
    ಸ್ಥಿರ char_u	buf[7];
    char_u		*str = *sp;
    ಪೂರ್ಣಾಂಕ			modifiers = 0;
    ಪೂರ್ಣಾಂಕ			special = FALSE;

    ಆದರೆ (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	ಆದರೆ (p != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ p;
    }

    c = *str;
    ಆದರೆ ((c == K_SPECIAL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		|| c == CSI
#ಆದರೆ_ಅಂತ್ಯ
	) && str[1] != NUL && str[2] != NUL)
    {
	ಆದರೆ (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	ಆದರೆ ((c == K_SPECIAL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    || c == CSI
#ಆದರೆ_ಅಂತ್ಯ
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = ಗಮ್ಯ_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	ಆದರೆ (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    ಆದರೆ (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	ಆದರೆ (p != ಖಾಲಿ)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	ಇಲ್ಲದಿದ್ದರೆ
	    // illegal byte
	    *sp = str + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    ಆದರೆ (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	ಹಿಂತಿರುಗಿ get_special_ಕೀಲಿ_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Translate a key sequence into special key names.
 */
    ಶೂನ್ಯ
str2specialbuf(char_u *sp, char_u *buf, ಪೂರ್ಣಾಂಕ len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    ಆಗ (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	ಆದರೆ (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }
}

/*
 * print line for :print or :list command
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_prt_ಸಾಲು(char_u *s, ಪೂರ್ಣಾಂಕ list)
{
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		n_extra = 0;
    ಪೂರ್ಣಾಂಕ		c_extra = 0;
    ಪೂರ್ಣಾಂಕ		c_final = 0;
    char_u	*p_extra = ಖಾಲಿ;	    // init to make SASC shut up
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		attr = 0;
    char_u	*trail = ಖಾಲಿ;
    char_u	*lead = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಒಳ_multispace = FALSE;
    ಪೂರ್ಣಾಂಕ		multispace_pos = 0;
    ಪೂರ್ಣಾಂಕ		l;
    char_u	buf[MB_MAXBYTES + 1];

    ಆದರೆ (curwin->w_p_list)
	list = TRUE;

    ಆದರೆ (list)
    {
	// find start of trailing whitespace
	ಆದರೆ (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    ಆಗ (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	ಆದರೆ (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != ಖಾಲಿ
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    ಆಗ (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    ಆದರೆ (*lead == NUL)
		lead = ಖಾಲಿ;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    ಆದರೆ (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	ಸಂದೇಶ_putchar(' ');

    ಆಗ (!got_int)
    {
	ಆದರೆ (n_extra > 0)
	{
	    --n_extra;
	    ಆದರೆ (n_extra == 0 && c_final)
		c = c_final;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c_extra)
		c = c_extra;
	    ಇಲ್ಲದಿದ್ದರೆ
		c = *p_extra++;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    ಲಂಬಸಾಲು += (*mb_ptr2cells)(s);
	    ಆದರೆ (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		ಪೂರ್ಣಾಂಕ len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)buf);
	    s += l;
	    ಮುಂದುವರಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    attr = 0;
	    c = *s++;
	    ಆದರೆ (list)
	    {
		ಒಳ_multispace = c == ' ' && (*s == ' '
						 || (ಲಂಬಸಾಲು > 0 && s[-2] == ' '));
		ಆದರೆ (!ಒಳ_multispace)
		    multispace_pos = 0;
	    }
	    ಆದರೆ (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		n_extra = tabstop_padding(ಲಂಬಸಾಲು, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#ಇಲ್ಲದಿದ್ದರೆ
		n_extra = curbuf->b_p_ts - ಲಂಬಸಾಲು % curbuf->b_p_ts - 1;
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪೂರ್ಣಾಂಕ lcs_tab1 = curwin->w_lcs_chars.tab1;
		    ಪೂರ್ಣಾಂಕ lcs_tab2 = curwin->w_lcs_chars.tab2;
		    ಪೂರ್ಣಾಂಕ lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    ಆದರೆ (lead != ಖಾಲಿ && s <= lead
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
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(ಖಾಲಿ, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == ' ')
	    {
		ಆದರೆ (lead != ಖಾಲಿ && s <= lead && ಒಳ_multispace
			&& curwin->w_lcs_chars.leadmultispace != ಖಾಲಿ)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    ಆದರೆ (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lead != ಖಾಲಿ && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (trail != ಖಾಲಿ && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_multispace
			&& curwin->w_lcs_chars.multispace != ಖಾಲಿ)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    ಆದರೆ (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	ಆದರೆ (c == NUL)
	    ನಿಲ್ಲಿಸು;

	ಸಂದೇಶ_putchar_attr(c, attr);
	ಲಂಬಸಾಲು++;
    }
    ಸಂದೇಶ_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    ಸ್ಥಿರ char_u *
screen_puts_mbyte(char_u *s, ಪೂರ್ಣಾಂಕ l, ಪೂರ್ಣಾಂಕ attr)
{
    ಪೂರ್ಣಾಂಕ		cw;

    ಸಂದೇಶ_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    ಆದರೆ (cw > 1 && (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		cmdmsg_rl ? ಸಂದೇಶ_ಲಂಬಸಾಲು <= 1 :
#ಆದರೆ_ಅಂತ್ಯ
		ಸಂದೇಶ_ಲಂಬಸಾಲು == cmdline_ಅಗಲ - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	ಸಂದೇಶ_screen_putchar('>', HL_ATTR(HLF_AT));
	ಹಿಂತಿರುಗಿ s;
    }

    screen_puts_len(s, l, ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು, attr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (cmdmsg_rl)
    {
	ಸಂದೇಶ_ಲಂಬಸಾಲು -= cw;
	ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು == 0)
	{
	    ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ;
	    ++ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಸಂದೇಶ_ಲಂಬಸಾಲು += cw;
	ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು >= cmdline_ಅಗಲ)
	{
	    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
	    ++ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	}
    }
    ಹಿಂತಿರುಗಿ s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_puts(ಅಕ್ಷರ *s)
{
    ಸಂದೇಶ_puts_attr(s, 0);
}

    ಶೂನ್ಯ
ಸಂದೇಶ_puts_title(ಅಕ್ಷರ *s)
{
    ಸಂದೇಶ_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_outtrans_long_len_attr(char_u *longstr, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ attr)
{
    ಪೂರ್ಣಾಂಕ		slen = len;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ;

    ಸ್ಥಳ = cmdline_ಅಗಲ - ಸಂದೇಶ_ಲಂಬಸಾಲು;
    ಆದರೆ (len > ಸ್ಥಳ && ಸ್ಥಳ >= 20)
    {
	slen = (ಸ್ಥಳ - 3) / 2;
	ಸಂದೇಶ_outtrans_len_attr(longstr, slen, attr);
	ಸಂದೇಶ_puts_attr("...", HL_ATTR(HLF_8));
    }
    ಸಂದೇಶ_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    ಶೂನ್ಯ
ಸಂದೇಶ_outtrans_long_attr(char_u *longstr, ಪೂರ್ಣಾಂಕ attr)
{
    ಸಂದೇಶ_outtrans_long_len_attr(longstr, (ಪೂರ್ಣಾಂಕ)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_puts_attr(ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ attr)
{
    ಸಂದೇಶ_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_puts_attr_len(ಅಕ್ಷರ *str, ಪೂರ್ಣಾಂಕ maxlen, ಪೂರ್ಣಾಂಕ attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    ಆದರೆ (ಸಂದೇಶ_silent != 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (attr == 0)
	attr = HL_ATTR(HLF_ಸಂದೇಶ);

    // if MSG_HIST flag set, add message to history
    ಆದರೆ ((attr & ಸಂದೇಶ_HIST) && maxlen < 0)
    {
	ಕೂಡಿಸು_ಸಂದೇಶ_hist((char_u *)str, -1, attr);
	attr &= ~ಸಂದೇಶ_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    ಆದರೆ (ಸಂದೇಶ_scrolled != 0 && !ಸಂದೇಶ_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    ಸಂದೇಶ_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    ಆದರೆ (ಸಂದೇಶ_use_printf())
	ಸಂದೇಶ_puts_printf((char_u *)str, maxlen);
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_puts_display((char_u *)str, maxlen, attr, FALSE);

    need_fileinfo = FALSE;
}

// values for "where"
#ವ್ಯಾಖ್ಯಾನಿಸು PUT_APPEND 0		// append to "lnum"
#ವ್ಯಾಖ್ಯಾನಿಸು PUT_TRUNC 1		// replace "lnum"
#ವ್ಯಾಖ್ಯಾನಿಸು PUT_ಕೆಳಗೆ 2		// add below "lnum"
				//
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
/*
 * Put text "t_s" until "end" in the message window.
 * "where" specifies where to put the text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
put_ಸಂದೇಶ_win(win_T *wp, ಪೂರ್ಣಾಂಕ where, char_u *t_s, char_u *ಕೊನೆ, linenr_T lnum)
{
    char_u  *p;

    ಆದರೆ (where == PUT_ಕೆಳಗೆ)
    {
	ಆದರೆ (*ಕೊನೆ != NUL)
	{
	    p = vim_strnsave(t_s, ಕೊನೆ - t_s);
	    ಆದರೆ (p == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    p = t_s;
	ml_append_buf(wp->w_ಬಫರ್, lnum, p, (colnr_T)0, FALSE);
	ಆದರೆ (p != t_s)
	    vim_free(p);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *newp;

	curbuf = wp->w_ಬಫರ್;
	ಆದರೆ (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    ಆದರೆ (newp == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    ಆದರೆ (*ಕೊನೆ != NUL)
		newp[STRLEN(ml_get(lnum)) + (ಕೊನೆ - t_s)] = NUL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    newp = vim_strnsave(t_s, ಕೊನೆ - t_s);
	    ಆದರೆ (newp == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_ಬಫರ್;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    ಸಂದೇಶ_ಲಂಬಸಾಲು += (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - t_s);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_puts_display(
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		maxlen,
    ಪೂರ್ಣಾಂಕ		attr,
    ಪೂರ್ಣಾಂಕ		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    ಪೂರ್ಣಾಂಕ		t_ಲಂಬಸಾಲು = 0;	// screen cells todo, 0 when "t_s" not used
    ಪೂರ್ಣಾಂಕ		l;
    ಪೂರ್ಣಾಂಕ		cw;
    char_u	*sb_str = str;
    ಪೂರ್ಣಾಂಕ		sb_ಲಂಬಸಾಲು = ಸಂದೇಶ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		wrap;
    ಪೂರ್ಣಾಂಕ		did_ಕೊನೆಯ_char;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
    ಪೂರ್ಣಾಂಕ		where = PUT_APPEND;
    win_T	*ಸಂದೇಶ_win = ಖಾಲಿ;
    linenr_T    lnum = 1;

    ಆದರೆ (ಒಳ_echowindow)
    {
	ಸಂದೇಶ_win = popup_get_ಸಂದೇಶ_win();

	ಆದರೆ (ಸಂದೇಶ_win != ಖಾಲಿ)
	{
	    ಆದರೆ (!popup_ಸಂದೇಶ_win_visible())
	    {
		ಆದರೆ (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    ಸಂದೇಶ_win = ಖಾಲಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // currently hidden, make it empty
		    curbuf = ಸಂದೇಶ_win->w_ಬಫರ್;
		    ಆಗ ((curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು) == 0)
			ml_delete(1);
		    curbuf = curwin->w_ಬಫರ್;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		lnum = ಸಂದೇಶ_win->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    did_wait_return = FALSE;
    ಆಗ ((maxlen < 0 || (ಪೂರ್ಣಾಂಕ)(s - str) < maxlen) && *s != NUL)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	// For echowindow, use full width; for regular messages, leave last column
	ಪೂರ್ಣಾಂಕ wrap_ಲಂಬಸಾಲು = ಸಂದೇಶ_win != ಖಾಲಿ ? cmdline_ಅಗಲ : cmdline_ಅಗಲ - 1;
#ಇಲ್ಲದಿದ್ದರೆ
	ಪೂರ್ಣಾಂಕ wrap_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - 1;
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	ಆದರೆ (!recurse && ಸಂದೇಶ_ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲುಗಳು - 1 && (*s == '\n' || (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			ಸಂದೇಶ_ಲಂಬಸಾಲು <= 1
		      || (*s == TAB && ಸಂದೇಶ_ಲಂಬಸಾಲು <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && ಸಂದೇಶ_ಲಂಬಸಾಲು <= 2))
		    :
#ಆದರೆ_ಅಂತ್ಯ
		      ((*s != '\r' && ಸಂದೇಶ_ಲಂಬಸಾಲು + t_ಲಂಬಸಾಲು >= wrap_ಲಂಬಸಾಲು)
		       || (*s == TAB && ಸಂದೇಶ_ಲಂಬಸಾಲು + t_ಲಂಬಸಾಲು
			   >= (wrap_ಲಂಬಸಾಲು & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && ಸಂದೇಶ_ಲಂಬಸಾಲು + t_ಲಂಬಸಾಲು >= wrap_ಲಂಬಸಾಲು - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    ಆದರೆ (t_ಲಂಬಸಾಲು > 0)
	    {
		// output postponed text
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
		ಆದರೆ (ಸಂದೇಶ_win != ಖಾಲಿ)
		{
		    put_ಸಂದೇಶ_win(ಸಂದೇಶ_win, where, t_s, s, lnum);
		    ಆದರೆ (where == PUT_ಕೆಳಗೆ)
			++lnum;
		    t_ಲಂಬಸಾಲು = 0;
		    where = PUT_ಕೆಳಗೆ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    t_puts(&t_ಲಂಬಸಾಲು, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    ಆದರೆ (ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು && ಸಾಲುಗಳು_ಎಡ == 0)
		ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    ಆದರೆ (ಸಂದೇಶ_win == ಖಾಲಿ)
#ಆದರೆ_ಅಂತ್ಯ
		// Scroll the screen up one line.
		ಸಂದೇಶ_scroll_ಮೇಲೆ();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    ಆದರೆ (ಸಂದೇಶ_win == ಖಾಲಿ)
	    {
#ಆದರೆ_ಅಂತ್ಯ
		ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 2;
		ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು >= cmdline_ಅಗಲ)  // can happen after screen resize
		    ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    // Display char in last column before showing more-prompt.
	    ಆದರೆ (*s >= ' '
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#ಆದರೆ_ಅಂತ್ಯ
	       )
	    {
		ಆದರೆ (has_mbyte)
		{
		    ಆದರೆ (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (ಪೂರ್ಣಾಂಕ)((str + maxlen) - s));
		    ಇಲ್ಲದಿದ್ದರೆ
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ_screen_putchar(*s++, attr);
		did_ಕೊನೆಯ_char = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		did_ಕೊನೆಯ_char = FALSE;

	    ಆದರೆ (p_ಇನ್ನಷ್ಟು)
		// store text for scrolling back
		store_sb_ಪಠ್ಯ(&sb_str, s, attr, &sb_ಲಂಬಸಾಲು, TRUE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    ಆದರೆ (ಸಂದೇಶ_win == ಖಾಲಿ)
	    {
#ಆದರೆ_ಅಂತ್ಯ
		inc_ಸಂದೇಶ_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		ಆದರೆ (cmdline_ಅಡ್ಡಸಾಲು > 0 && !exmode_ಸಕ್ರಿಯ)
		    --cmdline_ಅಡ್ಡಸಾಲು;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		ಆದರೆ (ಸಾಲುಗಳು_ಎಡ > 0)
		    --ಸಾಲುಗಳು_ಎಡ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (p_ಇನ್ನಷ್ಟು && ಸಾಲುಗಳು_ಎಡ == 0 && State != ಬಹುಲಕ_HITRETURN
					    && !ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು && !exmode_ಸಕ್ರಿಯ)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CON_DIALOG
		ಆದರೆ (do_ಇನ್ನಷ್ಟು_prompt(NUL))
		    s = confirm_ಸಂದೇಶ_ಬಾಲ;
#ಇಲ್ಲದಿದ್ದರೆ
		(ಶೂನ್ಯ)do_ಇನ್ನಷ್ಟು_prompt(NUL);
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (quit_ಇನ್ನಷ್ಟು)
		    ಹಿಂತಿರುಗಿ;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    ಆದರೆ (did_ಕೊನೆಯ_char)
		ಮುಂದುವರಿ;
	}

	wrap = *s == '\n'
		    || ಸಂದೇಶ_ಲಂಬಸಾಲು + t_ಲಂಬಸಾಲು >= wrap_ಲಂಬಸಾಲು
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && ಸಂದೇಶ_ಲಂಬಸಾಲು + t_ಲಂಬಸಾಲು >= wrap_ಲಂಬಸಾಲು - 1);
	ಆದರೆ (t_ಲಂಬಸಾಲು > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    ಆದರೆ (ಸಂದೇಶ_win != ಖಾಲಿ)
	    {
		put_ಸಂದೇಶ_win(ಸಂದೇಶ_win, where, t_s, s, lnum);
		ಆದರೆ (where == PUT_ಕೆಳಗೆ)
		    ++lnum;
		t_ಲಂಬಸಾಲು = 0;
		where = PUT_ಕೆಳಗೆ;
		// Reset msg_col after outputting to new line in echowindow
		ಆದರೆ (wrap)
		    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		t_puts(&t_ಲಂಬಸಾಲು, t_s, s, attr);
	}

	ಆದರೆ (wrap && p_ಇನ್ನಷ್ಟು && !recurse)
	    // store text for scrolling back
	    store_sb_ಪಠ್ಯ(&sb_str, s, attr, &sb_ಲಂಬಸಾಲು, TRUE);

	ಆದರೆ (*s == '\n')		    // go to next line
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    ಆದರೆ (ಸಂದೇಶ_win != ಖಾಲಿ)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		ಆದರೆ ((ಸಂದೇಶ_win->w_ಬಫರ್->b_ml.ml_flags & ML_ಖಾಲಿಗಳು) == 0)
		{
		    put_ಸಂದೇಶ_win(ಸಂದೇಶ_win, PUT_ಕೆಳಗೆ, t_s, t_s, lnum);
		    ++lnum;
		}
		ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;  // Reset column for new line
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಸಂದೇಶ_didout = FALSE;	    // remember that line is empty
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    ಆದರೆ (cmdmsg_rl)
		ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - 1;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
	    ಆದರೆ (++ಸಂದೇಶ_ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲುಗಳು)  // safety check
		ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '\r')	    // go to column 0
	{
	    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    where = PUT_TRUNC;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '\b')	    // go to previous char
	{
	    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು)
		--ಸಂದೇಶ_ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == TAB)	    // translate Tab into spaces
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    ಆದರೆ (ಸಂದೇಶ_win != ಖಾಲಿ)
		ಸಂದೇಶ_ಲಂಬಸಾಲು = (ಸಂದೇಶ_ಲಂಬಸಾಲು + 7) % 8;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಮಾಡು
		    ಸಂದೇಶ_screen_putchar(' ', attr);
		ಆಗ (ಸಂದೇಶ_ಲಂಬಸಾಲು & 7);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		ಆದರೆ (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (ಪೂರ್ಣಾಂಕ)((str + maxlen) - s));
		ಇಲ್ಲದಿದ್ದರೆ
		    l = (*mb_ptr2len)(s);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#ಆದರೆ_ಅಂತ್ಯ
		    (cw > 1 && ಸಂದೇಶ_ಲಂಬಸಾಲು + t_ಲಂಬಸಾಲು >= wrap_ಲಂಬಸಾಲು))
	    {
		ಆದರೆ (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ_screen_putchar(*s, attr);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// postpone this character until later
		ಆದರೆ (t_ಲಂಬಸಾಲು == 0)
		    t_s = s;
		t_ಲಂಬಸಾಲು += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    ಆದರೆ (t_ಲಂಬಸಾಲು > 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	ಆದರೆ (ಸಂದೇಶ_win != ಖಾಲಿ)
	    put_ಸಂದೇಶ_win(ಸಂದೇಶ_win, where, t_s, s, lnum);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    t_puts(&t_ಲಂಬಸಾಲು, t_s, s, attr);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
    ಆದರೆ (ಸಂದೇಶ_win != ಖಾಲಿ)
	popup_show_ಸಂದೇಶ_win();
#ಆದರೆ_ಅಂತ್ಯ
    // Store the text for scroll back.
    ಆದರೆ (p_ಇನ್ನಷ್ಟು && !recurse)
	store_sb_ಪಠ್ಯ(&sb_str, s, attr, &sb_ಲಂಬಸಾಲು, FALSE);

    ಸಂದೇಶ_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_filtered(char_u *ಸಂದೇಶ)
{
    ಪೂರ್ಣಾಂಕ ಹೊಂದಿಕೆ;

    ಆದರೆ (cmdmod.cmod_filter_regmatch.regprog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಹೊಂದಿಕೆ = vim_regexec(&cmdmod.cmod_filter_regmatch, ಸಂದೇಶ, (colnr_T)0);
    ಹಿಂತಿರುಗಿ cmdmod.cmod_filter_force ? ಹೊಂದಿಕೆ : !ಹೊಂದಿಕೆ;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_scroll_ಮೇಲೆ(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
    ಆದರೆ (ಒಳ_echowindow)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    ಆದರೆ (gui.ಒಳ_use)
	gui_undraw_cursor();
#ಆದರೆ_ಅಂತ್ಯ
    // scrolling up always works
    mch_disable_flush();
    screen_del_ಸಾಲುಗಳು(0, 0, 1, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, TRUE, 0, ಖಾಲಿ);
    mch_enable_flush();

    ಆದರೆ (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, cmdline_ಲಂಬಸಾಲು_off,
		cmdline_ಲಂಬಸಾಲು_off + cmdline_ಅಗಲ, ' ', ' ', HL_ATTR(HLF_ಸಂದೇಶ));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	ಆದರೆ (ScreenAttrs[ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲುಗಳು - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 2, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1,
				 (ಪೂರ್ಣಾಂಕ)Columns - 1, (ಪೂರ್ಣಾಂಕ)Columns,
				 ' ', ' ', HL_ATTR(HLF_ಸಂದೇಶ));
    }
}

/*
 * Increment "msg_scrolled".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
inc_ಸಂದೇಶ_scrolled(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = ಖಾಲಿ;

	// v:scrollstart is empty, set it to the script/function name and line
	//ಸಂಖ್ಯೆ
	ಆದರೆ (p.string == ಖಾಲಿ)
	{
	    p.string = (char_u *)_("Unknown");
	    p.ಉದ್ದಳತೆ = STRLEN(p.string);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    size_t  tofreesize;

	    tofreesize = (ಪೂರ್ಣಾಂಕ)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    ಆದರೆ (tofree != ಖಾಲಿ)
	    {
		p.ಉದ್ದಳತೆ = vim_snprintf_safelen((ಅಕ್ಷರ *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (ದೀರ್ಘ)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	ಗಣ_vim_var_string(VV_SCROLLSTART, p.string, (ಪೂರ್ಣಾಂಕ)p.ಉದ್ದಳತೆ);
	vim_free(tofree);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ++ಸಂದೇಶ_scrolled;
    ಗಣ_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ msgchunk_S msgchunk_T;
ರಚನೆ msgchunk_S
{
    msgchunk_T	*sb_ಮುಂದಿನ;
    msgchunk_T	*sb_prev;
    ಅಕ್ಷರ	sb_eol;		// TRUE when line ends after this text
    ಪೂರ್ಣಾಂಕ		sb_ಸಂದೇಶ_ಲಂಬಸಾಲು;	// column in which text starts
    ಪೂರ್ಣಾಂಕ		sb_attr;	// text attributes
    char_u	sb_ಪಠ್ಯ[1];	// text to be displayed, actually longer
};

ಸ್ಥಿರ msgchunk_T *ಕೊನೆಯ_msgchunk = ಖಾಲಿ; // last displayed text

ಸ್ಥಿರ msgchunk_T *ಸಂದೇಶ_sb_ಆರಂಭ(msgchunk_T *mps);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_ಮುಗಿದ
} sb_clear_T;

// When to clear text on next msg.
ಸ್ಥಿರ sb_clear_T do_clear_sb_ಪಠ್ಯ = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
store_sb_ಪಠ್ಯ(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    ಪೂರ್ಣಾಂಕ		attr,
    ಪೂರ್ಣಾಂಕ		*sb_ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಮುಕ್ತಾಯ)		// line ends
{
    msgchunk_T	*mp;

    ಆದರೆ (do_clear_sb_ಪಠ್ಯ == SB_CLEAR_ALL
	    || do_clear_sb_ಪಠ್ಯ == SB_CLEAR_CMDLINE_ಮುಗಿದ)
    {
	clear_sb_ಪಠ್ಯ(do_clear_sb_ಪಠ್ಯ == SB_CLEAR_ALL);
	ಸಂದೇಶ_sb_eol();  // prevent messages from overlapping
	do_clear_sb_ಪಠ್ಯ = SB_CLEAR_NONE;
    }

    ಆದರೆ (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_ಪಠ್ಯ) + (s - *sb_str) + 1);
	ಆದರೆ (mp != ಖಾಲಿ)
	{
	    mp->sb_eol = ಮುಕ್ತಾಯ;
	    mp->sb_ಸಂದೇಶ_ಲಂಬಸಾಲು = *sb_ಲಂಬಸಾಲು;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_ಪಠ್ಯ, *sb_str, s - *sb_str);

	    ಆದರೆ (ಕೊನೆಯ_msgchunk == ಖಾಲಿ)
	    {
		ಕೊನೆಯ_msgchunk = mp;
		mp->sb_prev = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		mp->sb_prev = ಕೊನೆಯ_msgchunk;
		ಕೊನೆಯ_msgchunk->sb_ಮುಂದಿನ = mp;
		ಕೊನೆಯ_msgchunk = mp;
	    }
	    mp->sb_ಮುಂದಿನ = ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮುಕ್ತಾಯ && ಕೊನೆಯ_msgchunk != ಖಾಲಿ)
	ಕೊನೆಯ_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_ಲಂಬಸಾಲು = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    ಶೂನ್ಯ
may_clear_sb_ಪಠ್ಯ(ಶೂನ್ಯ)
{
    do_clear_sb_ಪಠ್ಯ = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    ಶೂನ್ಯ
sb_ಪಠ್ಯ_ಆರಂಭ_cmdline(ಶೂನ್ಯ)
{
    ಆದರೆ (do_clear_sb_ಪಠ್ಯ == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_ಪಠ್ಯ_restart_cmdline();
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಂದೇಶ_sb_eol();
	do_clear_sb_ಪಠ್ಯ = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    ಶೂನ್ಯ
sb_ಪಠ್ಯ_restart_cmdline(ಶೂನ್ಯ)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_ಪಠ್ಯ = SB_CLEAR_CMDLINE_BUSY;

    ಆದರೆ (ಕೊನೆಯ_msgchunk == ಖಾಲಿ || ಕೊನೆಯ_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	ಹಿಂತಿರುಗಿ;

    tofree = ಸಂದೇಶ_sb_ಆರಂಭ(ಕೊನೆಯ_msgchunk);
    ಕೊನೆಯ_msgchunk = tofree->sb_prev;
    ಆದರೆ (ಕೊನೆಯ_msgchunk != ಖಾಲಿ)
	ಕೊನೆಯ_msgchunk->sb_ಮುಂದಿನ = ಖಾಲಿ;
    ಆಗ (tofree != ಖಾಲಿ)
    {
	msgchunk_T *tofree_ಮುಂದಿನ = tofree->sb_ಮುಂದಿನ;

	vim_free(tofree);
	tofree = tofree_ಮುಂದಿನ;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    ಶೂನ್ಯ
sb_ಪಠ್ಯ_ಕೊನೆ_cmdline(ಶೂನ್ಯ)
{
    do_clear_sb_ಪಠ್ಯ = SB_CLEAR_CMDLINE_ಮುಗಿದ;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    ಶೂನ್ಯ
clear_sb_ಪಠ್ಯ(ಪೂರ್ಣಾಂಕ all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    ಆದರೆ (all)
	lastp = &ಕೊನೆಯ_msgchunk;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಕೊನೆಯ_msgchunk == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	lastp = &ಸಂದೇಶ_sb_ಆರಂಭ(ಕೊನೆಯ_msgchunk)->sb_prev;
    }

    ಆಗ (*lastp != ಖಾಲಿ)
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
show_sb_ಪಠ್ಯ(ಶೂನ್ಯ)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = ಸಂದೇಶ_sb_ಆರಂಭ(ಕೊನೆಯ_msgchunk);
    ಆದರೆ (mp == ಖಾಲಿ || mp->sb_prev == ಖಾಲಿ)
	vim_beep(BO_MESS);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	do_ಇನ್ನಷ್ಟು_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    ಸ್ಥಿರ msgchunk_T *
ಸಂದೇಶ_sb_ಆರಂಭ(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    ಆಗ (mp != ಖಾಲಿ && mp->sb_prev != ಖಾಲಿ && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    ಹಿಂತಿರುಗಿ mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_sb_eol(ಶೂನ್ಯ)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    ಆದರೆ (ಕೊನೆಯ_msgchunk != ಖಾಲಿ
	    && !(ಕೊನೆಯ_msgchunk->sb_ಪಠ್ಯ[0] == '\n'
		&& ಕೊನೆಯ_msgchunk->sb_ಪಠ್ಯ[1] == NUL))
	ಕೊನೆಯ_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    ಸ್ಥಿರ msgchunk_T *
disp_sb_ಸಾಲು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, msgchunk_T *smp, ಪೂರ್ಣಾಂಕ clear_ಗಮ್ಯ_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    ಗಾಗಿ (;;)
    {
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
	ಸಂದೇಶ_ಲಂಬಸಾಲು = mp->sb_ಸಂದೇಶ_ಲಂಬಸಾಲು;
	p = mp->sb_ಪಠ್ಯ;
	ಆದರೆ (*p == '\n')	    // don't display the line break
	    ++p;
	ಸಂದೇಶ_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	ಆದರೆ (clear_ಗಮ್ಯ_eol)
	    screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1,
		    cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + cmdline_ಅಗಲ,
		    ' ', ' ', HL_ATTR(HLF_ಸಂದೇಶ));

	ಆದರೆ (mp->sb_eol || mp->sb_ಮುಂದಿನ == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	mp = mp->sb_ಮುಂದಿನ;
    }
    ಹಿಂತಿರುಗಿ mp->sb_ಮುಂದಿನ;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
t_puts(
    ಪೂರ್ಣಾಂಕ		*t_ಲಂಬಸಾಲು,
    char_u	*t_s,
    char_u	*s,
    ಪೂರ್ಣಾಂಕ		attr)
{
    // output postponed text
    ಸಂದೇಶ_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (ಪೂರ್ಣಾಂಕ)(s - t_s), ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು,
	    attr);
    ಸಂದೇಶ_ಲಂಬಸಾಲು += *t_ಲಂಬಸಾಲು;
    *t_ಲಂಬಸಾಲು = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    ಆದರೆ (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--ಸಂದೇಶ_ಲಂಬಸಾಲು;
    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು >= cmdline_ಅಗಲ)
    {
	ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
	++ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
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
    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_use_printf(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (!ಸಂದೇಶ_check_screen()
#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.ಒಳ_use && !termcap_ಸಕ್ರಿಯ)
# ಇಲ್ಲದಿದ್ದರೆ
	    || !termcap_ಸಕ್ರಿಯ
# endif
#ಆದರೆ_ಅಂತ್ಯ
	    || (swapping_screen() && !termcap_ಸಕ್ರಿಯ)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_puts_printf(char_u *str, ಪೂರ್ಣಾಂಕ maxlen)
{
    char_u	*s = str;
    char_u	*buf = ಖಾಲಿ;
    char_u	*p = s;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (!(silent_ಬಹುಲಕ && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#ಆದರೆ_ಅಂತ್ಯ
    ಆಗ ((maxlen < 0 || (ಪೂರ್ಣಾಂಕ)(s - str) < maxlen) && *s != NUL)
    {
	ಆದರೆ (!(silent_ಬಹುಲಕ && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    ಆದರೆ (*s == NL)
	    {
		ಪೂರ್ಣಾಂಕ n = (ಪೂರ್ಣಾಂಕ)(s - p);

		buf = alloc(n + 3);
		ಆದರೆ (buf != ಖಾಲಿ)
		{
		    ಸ್ಮರಣೆ_ನಕಲು(buf, p, n);
		    ಆದರೆ (!info_ಸಂದೇಶ)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    ಆದರೆ (info_ಸಂದೇಶ)   // informative message, not an error
			mch_ಸಂದೇಶ((ಅಕ್ಷರ *)buf);
		    ಇಲ್ಲದಿದ್ದರೆ
			mch_errmsg((ಅಕ್ಷರ *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (cmdmsg_rl)
	{
	    ಆದರೆ (*s == CAR || *s == NL)
		ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		--ಸಂದೇಶ_ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    ಆದರೆ (*s == CAR || *s == NL)
		ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		++ಸಂದೇಶ_ಲಂಬಸಾಲು;
	}
	++s;
    }

    ಆದರೆ (*p != NUL && !(silent_ಬಹುಲಕ && p_verbose == 0))
    {
	char_u *tofree = ಖಾಲಿ;

	ಆದರೆ (maxlen > 0 && vim_strlen_maxlen((ಅಕ್ಷರ *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಆದರೆ (info_ಸಂದೇಶ)
		mch_ಸಂದೇಶ((ಅಕ್ಷರ *)p);
	    ಇಲ್ಲದಿದ್ದರೆ
		mch_errmsg((ಅಕ್ಷರ *)p);
	    vim_free(tofree);
	}
    }

    ಸಂದೇಶ_didout = TRUE;	    // assume that line is not empty

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (!(silent_ಬಹುಲಕ && p_verbose == 0))
	mch_settmode(TMODE_RAW);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Show the more-prompt and handle the user response.
 * This takes care of scrolling back and displaying previously displayed text.
 * When at hit-enter prompt "typed_char" is the already typed character,
 * otherwise it's NUL.
 * Returns TRUE when jumping ahead to "confirm_msg_tail".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_ಇನ್ನಷ್ಟು_prompt(ಪೂರ್ಣಾಂಕ typed_char)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	entered = FALSE;
    ಪೂರ್ಣಾಂಕ		ಬಳಸಿದ_typed_char = typed_char;
    ಪೂರ್ಣಾಂಕ		oldState = State;
    ಪೂರ್ಣಾಂಕ		c;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CON_DIALOG
    ಪೂರ್ಣಾಂಕ		retval = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		toscroll;
    msgchunk_T	*mp_ಕೊನೆಯ = ಖಾಲಿ;
    msgchunk_T	*mp;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಸಂದೇಶ_attr;

    ಸಂದೇಶ_attr = HL_ATTR(HLF_ಸಂದೇಶ);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    ಆದರೆ (entered || (State == ಬಹುಲಕ_HITRETURN && typed_char == 0))
	ಹಿಂತಿರುಗಿ FALSE;
    entered = TRUE;

    ಆದರೆ (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_ಕೊನೆಯ = ಸಂದೇಶ_sb_ಆರಂಭ(ಕೊನೆಯ_msgchunk);
	ಗಾಗಿ (i = 0; i < ಅಡ್ಡಸಾಲುಗಳು - 2 && mp_ಕೊನೆಯ != ಖಾಲಿ
					     && mp_ಕೊನೆಯ->sb_prev != ಖಾಲಿ; ++i)
	    mp_ಕೊನೆಯ = ಸಂದೇಶ_sb_ಆರಂಭ(mp_ಕೊನೆಯ->sb_prev);
    }

    State = ಬಹುಲಕ_ASKMORE;
    setmouse();
    ಆದರೆ (typed_char == NUL)
	ಸಂದೇಶ_moremsg(FALSE);
    ಗಾಗಿ (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	ಆದರೆ (ಬಳಸಿದ_typed_char != NUL)
	{
	    c = ಬಳಸಿದ_typed_char;	// was typed at hit-enter prompt
	    ಬಳಸಿದ_typed_char = NUL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    c = get_keystroke();

#ಆದರೆ defined(FEAT_MENU) && defined(FEAT_GUI)
	ಆದರೆ (c == K_MENU)
	{
	    ಪೂರ್ಣಾಂಕ idx = get_menu_ಸೂಚ್ಯಂಕ(current_menu, ಬಹುಲಕ_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    ಆದರೆ (idx == MENU_ಸೂಚ್ಯಂಕ_INVALID)
		ಮುಂದುವರಿ;
	    c = *current_menu->ಅಕ್ಷರಸರಣಿಗಳು[idx];
	    ಆದರೆ (c != NUL && current_menu->ಅಕ್ಷರಸರಣಿಗಳು[idx][1] != NUL)
		ins_typebuf(current_menu->ಅಕ್ಷರಸರಣಿಗಳು[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#ಆದರೆ_ಅಂತ್ಯ

	toscroll = 0;
	ಆಯ್ಕೆ (c)
	{
	ಸ್ಥಿತಿ BS:		// scroll one line back
	ಸ್ಥಿತಿ K_BS:
	ಸ್ಥಿತಿ 'k':
	ಸ್ಥಿತಿ K_ಮೇಲೆ:
	    toscroll = -1;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CAR:		// one extra line
	ಸ್ಥಿತಿ NL:
	ಸ್ಥಿತಿ 'j':
	ಸ್ಥಿತಿ K_ಕೆಳಕ್ಕೆ:
	    toscroll = 1;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 'u':		// Up half a page
	    toscroll = -(ಅಡ್ಡಸಾಲುಗಳು / 2);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 'd':		// Down half a page
	    toscroll = ಅಡ್ಡಸಾಲುಗಳು / 2;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 'b':		// one page back
	ಸ್ಥಿತಿ Ctrl_B:
	ಸ್ಥಿತಿ K_PAGEUP:
	    toscroll = -(ಅಡ್ಡಸಾಲುಗಳು - 1);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ' ':		// one extra page
	ಸ್ಥಿತಿ 'f':
	ಸ್ಥಿತಿ Ctrl_F:
	ಸ್ಥಿತಿ K_PAGEDOWN:
	ಸ್ಥಿತಿ K_LEFTMOUSE:
	    toscroll = ಅಡ್ಡಸಾಲುಗಳು - 1;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 'g':		// all the way back to the start
	    toscroll = -999999;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 'G':		// all the way to the end
	    toscroll = 999999;
	    ಸಾಲುಗಳು_ಎಡ = 999999;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ':':		// start new command line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CON_DIALOG
	    ಆದರೆ (!confirm_ಸಂದೇಶ_ಬಳಸಿದ)
#ಆದರೆ_ಅಂತ್ಯ
	    {
		// Since got_int is set all typeahead will be flushed, but we
		// want to keep this ':', remember that in a special way.
		typeahead_noflush(':');
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
		skip_ಪದ_loop = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		cmdline_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;		// put ':' on this line
		skip_redraw = TRUE;		// skip redraw once
		need_wait_return = FALSE;	// don't wait in main()
	    }
	    // FALLTHROUGH
	ಸ್ಥಿತಿ 'q':		// quit
	ಸ್ಥಿತಿ Ctrl_C:
	ಸ್ಥಿತಿ ESC:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CON_DIALOG
	    ಆದರೆ (confirm_ಸಂದೇಶ_ಬಳಸಿದ)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		got_int = TRUE;
		quit_ಇನ್ನಷ್ಟು = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    ಸಾಲುಗಳು_ಎಡ = ಅಡ್ಡಸಾಲುಗಳು - 1;
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	ಸ್ಥಿತಿ Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    ಆದರೆ (clip_ನಕ್ಷತ್ರ.state == SELECT_ಮುಗಿದ)
		clip_copy_modeless_selection(TRUE);
	    ಮುಂದುವರಿ;
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ವನಿಯೋಜಿತ:		// no valid response
	    ಸಂದೇಶ_moremsg(TRUE);
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (toscroll != 0)
	{
	    ಆದರೆ (toscroll < 0)
	    {
		// go to start of last line
		ಆದರೆ (mp_ಕೊನೆಯ == ಖಾಲಿ)
		    mp = ಸಂದೇಶ_sb_ಆರಂಭ(ಕೊನೆಯ_msgchunk);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mp_ಕೊನೆಯ->sb_prev != ಖಾಲಿ)
		    mp = ಸಂದೇಶ_sb_ಆರಂಭ(mp_ಕೊನೆಯ->sb_prev);
		ಇಲ್ಲದಿದ್ದರೆ
		    mp = ಖಾಲಿ;

		// go to start of line at top of the screen
		ಗಾಗಿ (i = 0; i < ಅಡ್ಡಸಾಲುಗಳು - 2 && mp != ಖಾಲಿ && mp->sb_prev != ಖಾಲಿ;
									  ++i)
		    mp = ಸಂದೇಶ_sb_ಆರಂಭ(mp->sb_prev);

		ಆದರೆ (mp != ಖಾಲಿ && mp->sb_prev != ಖಾಲಿ)
		{
		    // Find line to be displayed at top.
		    ಗಾಗಿ (i = 0; i > toscroll; --i)
		    {
			ಆದರೆ (mp == ಖಾಲಿ || mp->sb_prev == ಖಾಲಿ)
			    ನಿಲ್ಲಿಸು;
			mp = ಸಂದೇಶ_sb_ಆರಂಭ(mp->sb_prev);
			ಆದರೆ (mp_ಕೊನೆಯ == ಖಾಲಿ)
			    mp_ಕೊನೆಯ = ಸಂದೇಶ_sb_ಆರಂಭ(ಕೊನೆಯ_msgchunk);
			ಇಲ್ಲದಿದ್ದರೆ
			    mp_ಕೊನೆಯ = ಸಂದೇಶ_sb_ಆರಂಭ(mp_ಕೊನೆಯ->sb_prev);
		    }

		    ಆದರೆ (toscroll == -1 && screen_ins_ಸಾಲುಗಳು(0, 0, 1,
						     (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, 0, ಖಾಲಿ) == OK)
		    {
			// display line at top
			(ಶೂನ್ಯ)disp_sb_ಸಾಲು(0, mp, FALSE);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಪೂರ್ಣಾಂಕ did_clear = screenclear();

			// redisplay all lines
			ಗಾಗಿ (i = 0; mp != ಖಾಲಿ && i < ಅಡ್ಡಸಾಲುಗಳು - 1; ++i)
			{
			    mp = disp_sb_ಸಾಲು(i, mp, !did_clear);
			    ++ಸಂದೇಶ_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// First display any text that we scrolled back.
		ಆಗ (toscroll > 0 && mp_ಕೊನೆಯ != ಖಾಲಿ)
		{
		    // scroll up, display line at bottom
		    ಸಂದೇಶ_scroll_ಮೇಲೆ();
		    inc_ಸಂದೇಶ_scrolled();
		    screen_fill((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 2, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1,
			    cmdline_ಲಂಬಸಾಲು_off, cmdline_ಲಂಬಸಾಲು_off + cmdline_ಅಗಲ,
			    ' ', ' ', ಸಂದೇಶ_attr);
		    mp_ಕೊನೆಯ = disp_sb_ಸಾಲು((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 2, mp_ಕೊನೆಯ, FALSE);
		    --toscroll;
		}
	    }

	    ಆದರೆ (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, cmdline_ಲಂಬಸಾಲು_off,
			cmdline_ಲಂಬಸಾಲು_off + cmdline_ಅಗಲ, ' ', ' ', ಸಂದೇಶ_attr);
		ಸಂದೇಶ_moremsg(FALSE);
		ಮುಂದುವರಿ;
	    }

	    // display more text, return to caller
	    ಸಾಲುಗಳು_ಎಡ = toscroll;
	}

	ನಿಲ್ಲಿಸು;
    }

    // clear the --more-- message
    screen_fill((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, cmdline_ಲಂಬಸಾಲು_off,
	    cmdline_ಲಂಬಸಾಲು_off + cmdline_ಅಗಲ, ' ', ' ', ಸಂದೇಶ_attr);
    State = oldState;
    setmouse();
    ಆದರೆ (quit_ಇನ್ನಷ್ಟು)
    {
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
	ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdmsg_rl)
	ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - 1;
#ಆದರೆ_ಅಂತ್ಯ

    entered = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CON_DIALOG
    ಹಿಂತಿರುಗಿ retval;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_ಸಂದೇಶ
#  undef mch_ಸಂದೇಶ
# endif

# ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_errmsg_c(ಅಕ್ಷರ *str)
{
    DWORD   nwrite = 0;
    DWORD   ಬಹುಲಕ = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    ಆದರೆ (Get_Console_ಬಹುಲಕ(h, &ಬಹುಲಕ) && enc_codepage >= 0
	    && (ಪೂರ್ಣಾಂಕ)GetConsoleCP() != enc_codepage)
    {
	ಪೂರ್ಣಾಂಕ	len = (ಪೂರ್ಣಾಂಕ)STRLEN(str);
	WCHAR	*w = enc_ಗಮ್ಯ_utf16((char_u *)str, &len);

	ಆದರೆ (w != ಖಾಲಿ)
	{
	    WriteConsoleW(h, w, len, &nwrite, ಖಾಲಿ);
	    vim_free(w);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
mch_errmsg(ಅಕ್ಷರ *str)
{
# ಆದರೆ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    ಪೂರ್ಣಾಂಕ		len;
# endif

# ಆದರೆ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    ಆದರೆ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   ಇಲ್ಲದಿದ್ದರೆ
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.ಒಳ_use || gui.starting)
#  endif
	    )
    {
	fprintf(stderr, "%s", str);
	ಹಿಂತಿರುಗಿ;
    }
# endif

# ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ಆದರೆ (!(gui.ಒಳ_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	ಹಿಂತಿರುಗಿ;
    }
# endif

# ಆದರೆ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (ಪೂರ್ಣಾಂಕ)STRLEN(str) + 1;
    ಆದರೆ (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    ಆದರೆ (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_ದತ್ತಾಂಶ + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_ದತ್ತಾಂಶ + error_ga.ga_len;
	    ಗಾಗಿ (;;)
	    {
		p = vim_strchr(p, '\r');
		ಆದರೆ (p == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_ಸಂದೇಶ_c(ಅಕ್ಷರ *str)
{
    DWORD   nwrite = 0;
    DWORD   ಬಹುಲಕ;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    ಆದರೆ (Get_Console_ಬಹುಲಕ(h, &ಬಹುಲಕ) && enc_codepage >= 0
	    && (ಪೂರ್ಣಾಂಕ)GetConsoleCP() != enc_codepage)
    {
	ಪೂರ್ಣಾಂಕ	len = (ಪೂರ್ಣಾಂಕ)STRLEN(str);
	WCHAR	*w = enc_ಗಮ್ಯ_utf16((char_u *)str, &len);

	ಆದರೆ (w != ಖಾಲಿ)
	{
	    WriteConsoleW(h, w, len, &nwrite, ಖಾಲಿ);
	    vim_free(w);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಬರೆ("%s", str);
    }
}
# endif

/*
 * Give a message.  To be used when the screen hasn't been initialized yet.
 * When there is no tty, collect messages until the GUI has started and they
 * can be displayed in a message box.
 */
    ಶೂನ್ಯ
mch_ಸಂದೇಶ(ಅಕ್ಷರ *str)
{
# ಆದರೆ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    ಆದರೆ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   ಇಲ್ಲದಿದ್ದರೆ
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.ಒಳ_use || gui.starting)
#  endif
	    )
    {
	ಬರೆ("%s", str);
	ಹಿಂತಿರುಗಿ;
    }
# endif

# ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ಆದರೆ (!(gui.ಒಳ_use || gui.starting))
#  endif
    {
	mch_ಸಂದೇಶ_c(str);
	ಹಿಂತಿರುಗಿ;
    }
# endif
# ಆದರೆ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_screen_putchar(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ attr)
{
    ಸಂದೇಶ_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು, attr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (cmdmsg_rl)
    {
	ಆದರೆ (--ಸಂದೇಶ_ಲಂಬಸಾಲು == 0)
	{
	    ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ;
	    ++ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಆದರೆ (++ಸಂದೇಶ_ಲಂಬಸಾಲು >= cmdline_ಅಗಲ)
	{
	    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
	    ++ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_moremsg(ಪೂರ್ಣಾಂಕ ತುಂಬಿದ)
{
    ಪೂರ್ಣಾಂಕ		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, cmdline_ಲಂಬಸಾಲು_off, attr);
    ಆದರೆ (ತುಂಬಿದ)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, cmdline_ಲಂಬಸಾಲು_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    ಶೂನ್ಯ
repeat_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಆದರೆ (State == ಬಹುಲಕ_ASKMORE)
    {
	ಸಂದೇಶ_moremsg(TRUE);	// display --more-- message again
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CON_DIALOG
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State == ಬಹುಲಕ_CONFIRM)
    {
	display_confirm_ಸಂದೇಶ();	// display ":confirm" message again
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State == ಬಹುಲಕ_EXTERNCMD)
    {
	windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು); // put cursor back
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State == ಬಹುಲಕ_HITRETURN || State == ಬಹುಲಕ_SETWSIZE)
    {
	ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು == ಅಡ್ಡಸಾಲುಗಳು - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    ಸಂದೇಶ_didout = FALSE;
	    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
	    ಸಂದೇಶ_clr_eos();
	}
	ದೊರಕಿತು_return_ಸಂದೇಶ();
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_check_screen(ಶೂನ್ಯ)
{
    ಆದರೆ (!ತುಂಬಿದ_screen || !screen_valid(FALSE))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲುಗಳು)
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು >= cmdline_ಅಗಲ)
	ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - 1;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_clr_eos(ಶೂನ್ಯ)
{
    ಆದರೆ (ಸಂದೇಶ_silent == 0)
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
    ಆದರೆ (ಒಳ_echowindow)
	ಹಿಂತಿರುಗಿ;  // messages go into a popup
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಸಂದೇಶ_use_printf())
    {
	ಆದರೆ (ತುಂಬಿದ_screen)	// only when termcap codes are valid
	{
	    ಆದರೆ (*T_CD)
		ಹೊರಹೋಗು_str(T_CD);	// clear to end of display
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_CE)
		ಹೊರಹೋಗು_str(T_CE);	// clear to end of line
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_attr;

	ಸಂದೇಶ_attr = HL_ATTR(HLF_ಸಂದೇಶ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (cmdmsg_rl)
	{
	    screen_fill(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, ಸಂದೇಶ_ಅಡ್ಡಸಾಲು + 1,
		    cmdline_ಲಂಬಸಾಲು_off, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು + 1,
		    ' ', ' ', ಸಂದೇಶ_attr);
	    screen_fill(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು + 1, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು,
		    cmdline_ಲಂಬಸಾಲು_off, cmdline_ಲಂಬಸಾಲು_off + cmdline_ಅಗಲ,
		    ' ', ' ', ಸಂದೇಶ_attr);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    screen_fill(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, ಸಂದೇಶ_ಅಡ್ಡಸಾಲು + 1,
		    cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + cmdline_ಅಗಲ,
		    ' ', ' ', ಸಂದೇಶ_attr);
	    screen_fill(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು + 1, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು,
		    cmdline_ಲಂಬಸಾಲು_off, cmdline_ಲಂಬಸಾಲು_off + cmdline_ಅಗಲ,
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
    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು;
    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    ಸಂದೇಶ_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    ಪೂರ್ಣಾಂಕ
ಸಂದೇಶ_ಕೊನೆ(ಶೂನ್ಯ)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    ಆದರೆ (!exiting && need_wait_return && !(State & ಬಹುಲಕ_CMDLINE))
    {
	wait_return(FALSE);
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹೊರಹೋಗು_flush();
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_check(ಶೂನ್ಯ)
{
    ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು == ಅಡ್ಡಸಾಲುಗಳು - 1 && ಸಂದೇಶ_ಲಂಬಸಾಲು >= sc_ಲಂಬಸಾಲು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
		&& !ಒಳ_echowindow
#ಆದರೆ_ಅಂತ್ಯ
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
redir_write(char_u *str, ಪೂರ್ಣಾಂಕ maxlen)
{
    char_u	*s = str;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cur_ಲಂಬಸಾಲು = 0;

    // Don't do anything for displaying prompts and the like.
    ಆದರೆ (redir_off)
	ಹಿಂತಿರುಗಿ;

    // If 'verbosefile' is set prepare for writing in that file.
    ಆದರೆ (*p_vfile != NUL && verbose_fd == ಖಾಲಿ)
	verbose_open();

    ಆದರೆ (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	ಆದರೆ (*s != '\n' && *s != '\r')
	{
	    ಆಗ (cur_ಲಂಬಸಾಲು < ಸಂದೇಶ_ಲಂಬಸಾಲು)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಆದರೆ (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (redir_fd != ಖಾಲಿ)
		    fputs(" ", redir_fd);
		ಆದರೆ (verbose_fd != ಖಾಲಿ)
		    fputs(" ", verbose_fd);
		++cur_ಲಂಬಸಾಲು;
	    }
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (redir_execute)
	    execute_redir_str(s, maxlen);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (redir_vname)
	    var_redir_str(s, maxlen);
#ಆದರೆ_ಅಂತ್ಯ

	// Write and adjust the current column.
	ಆಗ (*s != NUL && (maxlen < 0 || (ಪೂರ್ಣಾಂಕ)(s - str) < maxlen))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (!redir_reg && !redir_vname && !redir_execute)
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (redir_fd != ಖಾಲಿ)
		    putc(*s, redir_fd);
	    ಆದರೆ (verbose_fd != ಖಾಲಿ)
		putc(*s, verbose_fd);
	    ಆದರೆ (*s == '\r' || *s == '\n')
		cur_ಲಂಬಸಾಲು = 0;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '\t')
		cur_ಲಂಬಸಾಲು += (8 - cur_ಲಂಬಸಾಲು % 8);
	    ಇಲ್ಲದಿದ್ದರೆ
		++cur_ಲಂಬಸಾಲು;
	    ++s;
	}

	ಆದರೆ (ಸಂದೇಶ_silent != 0)	// should update msg_col
	    ಸಂದೇಶ_ಲಂಬಸಾಲು = cur_ಲಂಬಸಾಲು;
    }
}

    ಪೂರ್ಣಾಂಕ
redirecting(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ redir_fd != ಖಾಲಿ || *p_vfile != NUL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#ಆದರೆ_ಅಂತ್ಯ
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    ಶೂನ್ಯ
verbose_enter(ಶೂನ್ಯ)
{
    ಆದರೆ (*p_vfile != NUL)
	++ಸಂದೇಶ_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    ಶೂನ್ಯ
verbose_leave(ಶೂನ್ಯ)
{
    ಆದರೆ (*p_vfile != NUL)
	ಆದರೆ (--ಸಂದೇಶ_silent < 0)
	    ಸಂದೇಶ_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    ಶೂನ್ಯ
verbose_enter_scroll(ಶೂನ್ಯ)
{
    ಆದರೆ (*p_vfile != NUL)
	++ಸಂದೇಶ_silent;
    ಇಲ್ಲದಿದ್ದರೆ
	// always scroll up, don't overwrite
	ಸಂದೇಶ_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    ಶೂನ್ಯ
verbose_leave_scroll(ಶೂನ್ಯ)
{
    ಆದರೆ (*p_vfile != NUL)
    {
	ಆದರೆ (--ಸಂದೇಶ_silent < 0)
	    ಸಂದೇಶ_silent = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    ಶೂನ್ಯ
verbose_stop(ಶೂನ್ಯ)
{
    ಆದರೆ (verbose_fd != ಖಾಲಿ)
    {
	fclose(verbose_fd);
	verbose_fd = ಖಾಲಿ;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    ಪೂರ್ಣಾಂಕ
verbose_open(ಶೂನ್ಯ)
{
    ಆದರೆ (verbose_fd == ಖಾಲಿ && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((ಅಕ್ಷರ *)p_vfile, "a");
	ಆದರೆ (verbose_fd == ಖಾಲಿ)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    ಶೂನ್ಯ
give_warning(char_u *ಸಂದೇಶ_2, ಪೂರ್ಣಾಂಕ hl)
{
    give_warning_with_ಮೂಲ(ಸಂದೇಶ_2, hl, FALSE);
}

    ಶೂನ್ಯ
give_warning_with_ಮೂಲ(char_u *ಸಂದೇಶ_2, ಪೂರ್ಣಾಂಕ hl, ಪೂರ್ಣಾಂಕ with_ಮೂಲ)
{
    // Don't do this for ":silent".
    ಆದರೆ (ಸಂದೇಶ_silent != 0)
	ಹಿಂತಿರುಗಿ;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಗಣ_vim_var_string(VV_WARNINGMSG, ಸಂದೇಶ_2, -1);
#ಆದರೆ_ಅಂತ್ಯ
    VIM_CLEAR(keep_ಸಂದೇಶ);
    ಆದರೆ (hl)
	keep_ಸಂದೇಶ_attr = HL_ATTR(HLF_W);
    ಇಲ್ಲದಿದ್ದರೆ
	keep_ಸಂದೇಶ_attr = 0;

    ಆದರೆ (with_ಮೂಲ)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	ಸಂದೇಶ_ಆರಂಭ();
	ಸಂದೇಶ_ಮೂಲ(HL_ATTR(HLF_W));
	ಸಂದೇಶ_puts(" ");
	ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)ಸಂದೇಶ_2, HL_ATTR(HLF_W) | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_clr_eos();
	(ಶೂನ್ಯ)ಸಂದೇಶ_ಕೊನೆ();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಂದೇಶ_attr((ಅಕ್ಷರ *)ಸಂದೇಶ_2, keep_ಸಂದೇಶ_attr) && ಸಂದೇಶ_scrolled == 0)
	ಗಣ_keep_ಸಂದೇಶ(ಸಂದೇಶ_2, keep_ಸಂದೇಶ_attr);

    ಸಂದೇಶ_didout = FALSE;	    // overwrite this message
    ಸಂದೇಶ_nowait = TRUE;	    // don't wait for this message
    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;

    --no_wait_return;
}

#ಆದರೆ defined(FEAT_EVAL)
    ಶೂನ್ಯ
give_warning2(char_u *ಸಂದೇಶ_2, char_u *a1, ಪೂರ್ಣಾಂಕ hl)
{
    ಆದರೆ (IObuff == ಖಾಲಿ)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(ಸಂದೇಶ_2, hl);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, (ಅಕ್ಷರ *)ಸಂದೇಶ_2, a1);
	give_warning(IObuff, hl);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Advance msg cursor to column "col".
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_advance(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಆದರೆ (ಸಂದೇಶ_silent != 0)	// nothing to advance to
    {
	ಸಂದೇಶ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;		// for redirection, may fill it up later
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಲಂಬಸಾಲು >= cmdline_ಅಗಲ)	// not enough room
	ಲಂಬಸಾಲು = cmdline_ಅಗಲ - 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (cmdmsg_rl)
	ಆಗ (ಸಂದೇಶ_ಲಂಬಸಾಲು > cmdline_ಅಗಲ - ಲಂಬಸಾಲು)
	    ಸಂದೇಶ_putchar(' ');
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆಗ (ಸಂದೇಶ_ಲಂಬಸಾಲು < ಲಂಬಸಾಲು)
	    ಸಂದೇಶ_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    ಶೂನ್ಯ
ಸಂದೇಶ_warn_missing_clipboard(ಶೂನ್ಯ)
{
    ಆದರೆ (!global_busy && !did_warn_clipboard && silence_w23_w24_ಸಂದೇಶ == 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	ಸಂದೇಶ(_("W23: Clipboard register not available, using register 0"));
#ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ(_("W24: Clipboard register not available. See :h W24"));
#ಆದರೆ_ಅಂತ್ಯ
	did_warn_clipboard = TRUE;
    }
}

#ಆದರೆ defined(FEAT_CON_DIALOG)
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
    ಪೂರ್ಣಾಂಕ
do_dialog(
    ಪೂರ್ಣಾಂಕ		type ಬಳಸದ,
    char_u	*title ಬಳಸದ,
    char_u	*ಸಂದೇಶ_2,
    char_u	*buttons,
    ಪೂರ್ಣಾಂಕ		dfltbutton,
    char_u	*textfield ಬಳಸದ,	// IObuff for inputdialog(), NULL
					// otherwise
    ಪೂರ್ಣಾಂಕ		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    ಪೂರ್ಣಾಂಕ		oldState;
    ಪೂರ್ಣಾಂಕ		retval = 0;
    char_u	*hotkeys;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    ಆದರೆ (silent_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    ಆದರೆ (gui.ಒಳ_use && vim_strchr(p_go, GO_CONDIALOG) == ಖಾಲಿ)
    {
	// --gui-dialog-file: write text to a file
	ಆದರೆ (gui_dialog_log(title, ಸಂದೇಶ_2))
	    c = dfltbutton;
	ಇಲ್ಲದಿದ್ದರೆ
	    c = gui_mch_dialog(type, title, ಸಂದೇಶ_2, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	ಹೊರಹೋಗು_flush();
	gui_mch_update();

	ಹಿಂತಿರುಗಿ c;
    }
# endif

    oldState = State;
    State = ಬಹುಲಕ_CONFIRM;
    setmouse();

    // Ensure raw mode here.
    save_tmode = cur_tmode;
    settmode(TMODE_RAW);

    /*
     * Since we wait for a keypress, don't make the
     * user press RETURN as well afterwards.
     */
    ++no_wait_return;
    hotkeys = ಸಂದೇಶ_show_console_dialog(ಸಂದೇಶ_2, buttons, dfltbutton);

    ಆದರೆ (hotkeys != ಖಾಲಿ)
    {
	ಗಾಗಿ (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    ಆಯ್ಕೆ (c)
	    {
	    ಸ್ಥಿತಿ CAR:		// User accepts default option
	    ಸ್ಥಿತಿ NL:
		retval = dfltbutton;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Ctrl_C:	// User aborts/cancels
	    ಸ್ಥಿತಿ ESC:
		retval = 0;
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:		// Could be a hotkey?
		ಆದರೆ (c < 0)	// special keys are ignored here
		    ಮುಂದುವರಿ;
		ಆದರೆ (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    ನಿಲ್ಲಿಸು;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		ಗಾಗಿ (i = 0; hotkeys[i]; ++i)
		{
		    ಆದರೆ (has_mbyte)
		    {
			ಆದರೆ ((*mb_ptr2char)(hotkeys + i) == c)
			    ನಿಲ್ಲಿಸು;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ಆದರೆ (hotkeys[i] == c)
			    ನಿಲ್ಲಿಸು;
		    ++retval;
		}
		ಆದರೆ (hotkeys[i])
		    ನಿಲ್ಲಿಸು;
		// No hotkey match, so keep waiting
		ಮುಂದುವರಿ;
	    }
	    ನಿಲ್ಲಿಸು;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    ಸಂದೇಶ_ಕೊನೆ_prompt();

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
copy_char(
    char_u	*from,
    char_u	*ಗಮ್ಯ,
    ಪೂರ್ಣಾಂಕ		lowercase)	// make character lower case
{
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (has_mbyte)
    {
	ಆದರೆ (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    ಹಿಂತಿರುಗಿ (*mb_char2bytes)(c, ಗಮ್ಯ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(ಗಮ್ಯ, from, (size_t)len);
	    ಹಿಂತಿರುಗಿ len;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (lowercase)
	    *ಗಮ್ಯ = (char_u)TOLOWER_LOC(*from);
	ಇಲ್ಲದಿದ್ದರೆ
	    *ಗಮ್ಯ = *from;
	ಹಿಂತಿರುಗಿ 1;
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
    char_u	*ಸಂದೇಶ_2,
    char_u	*buttons,
    ಪೂರ್ಣಾಂಕ		dfltbutton)
{
    ಪೂರ್ಣಾಂಕ		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    ಪೂರ್ಣಾಂಕ		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = ಖಾಲಿ;
    char_u	*msgp = ಖಾಲಿ;
    char_u	*hotkp = ಖಾಲಿ;
    char_u	*r;
    ಪೂರ್ಣಾಂಕ		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ_hotkey = FALSE;	// first char of button is hotkey
    ಪೂರ್ಣಾಂಕ		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    ಗಾಗಿ (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	ಆಗ (*r)
	{
	    ಆದರೆ (*r == DLG_BUTTON_SEP)
	    {
		ಆದರೆ (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    ಆದರೆ (has_mbyte)
			hotkp += STRLEN(hotkp);
		    ಇಲ್ಲದಿದ್ದರೆ
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    ಆದರೆ (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    ಆದರೆ (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			ಮೊದಲ_hotkey = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    ಆದರೆ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*r == DLG_HOTKEY_CHAR || ಮೊದಲ_hotkey)
	    {
		ಆದರೆ (*r == DLG_HOTKEY_CHAR)
		    ++r;
		ಮೊದಲ_hotkey = FALSE;
		ಆದರೆ (copy)
		{
		    ಆದರೆ (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ++len;	    // '&a' -> '[a]'
		    ಆದರೆ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// everything else copy literally
		ಆದರೆ (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	ಆದರೆ (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    len += (ಪೂರ್ಣಾಂಕ)(STRLEN(ಸಂದೇಶ_2)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    ಆದರೆ (!has_hotkey[0])
	    {
		ಮೊದಲ_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_ಸಂದೇಶ);
	    confirm_ಸಂದೇಶ = alloc(len);
	    ಆದರೆ (confirm_ಸಂದೇಶ == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    *confirm_ಸಂದೇಶ = NUL;
	    hotk = alloc(lenhotkey);
	    ಆದರೆ (hotk == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	    *confirm_ಸಂದೇಶ = '\n';
	    STRCPY(confirm_ಸಂದೇಶ + 1, ಸಂದೇಶ_2);

	    msgp = confirm_ಸಂದೇಶ + 1 + STRLEN(ಸಂದೇಶ_2);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_ಸಂದೇಶ_ಬಾಲ = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_ಸಂದೇಶ();
    ಹಿಂತಿರುಗಿ hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
display_confirm_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_ಸಂದೇಶ_ಬಳಸಿದ;
    ಆದರೆ (confirm_ಸಂದೇಶ != ಖಾಲಿ)
	ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)confirm_ಸಂದೇಶ, HL_ATTR(HLF_M));
    --confirm_ಸಂದೇಶ_ಬಳಸಿದ;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_CON_DIALOG

#ಆದರೆ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    ಪೂರ್ಣಾಂಕ
vim_dialog_yesno(
    ಪೂರ್ಣಾಂಕ		type,
    char_u	*title,
    char_u	*ಸಂದೇಶ_2,
    ಪೂರ್ಣಾಂಕ		dflt)
{
    ಆದರೆ (do_dialog(type,
		title == ಖಾಲಿ ? (char_u *)_("Question") : title,
		ಸಂದೇಶ_2,
		(char_u *)_("&Yes\n&No"), dflt, ಖಾಲಿ, FALSE) == 1)
	ಹಿಂತಿರುಗಿ VIM_YES;
    ಹಿಂತಿರುಗಿ VIM_NO;
}

    ಪೂರ್ಣಾಂಕ
vim_dialog_yesnocancel(
    ಪೂರ್ಣಾಂಕ		type,
    char_u	*title,
    char_u	*ಸಂದೇಶ_2,
    ಪೂರ್ಣಾಂಕ		dflt)
{
    ಆಯ್ಕೆ (do_dialog(type,
		title == ಖಾಲಿ ? (char_u *)_("Question") : title,
		ಸಂದೇಶ_2,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, ಖಾಲಿ, FALSE))
    {
	ಸ್ಥಿತಿ 1: ಹಿಂತಿರುಗಿ VIM_YES;
	ಸ್ಥಿತಿ 2: ಹಿಂತಿರುಗಿ VIM_NO;
    }
    ಹಿಂತಿರುಗಿ VIM_CANCEL;
}

    ಪೂರ್ಣಾಂಕ
vim_dialog_yesnoallcancel(
    ಪೂರ್ಣಾಂಕ		type,
    char_u	*title,
    char_u	*ಸಂದೇಶ_2,
    ಪೂರ್ಣಾಂಕ		dflt)
{
    ಆಯ್ಕೆ (do_dialog(type,
		title == ಖಾಲಿ ? (char_u *)"Question" : title,
		ಸಂದೇಶ_2,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, ಖಾಲಿ, FALSE))
    {
	ಸ್ಥಿತಿ 1: ಹಿಂತಿರುಗಿ VIM_YES;
	ಸ್ಥಿತಿ 2: ಹಿಂತಿರುಗಿ VIM_NO;
	ಸ್ಥಿತಿ 3: ಹಿಂತಿರುಗಿ VIM_ALL;
	ಸ್ಥಿತಿ 4: ಹಿಂತಿರುಗಿ VIM_DISCARDALL;
    }
    ಹಿಂತಿರುಗಿ VIM_CANCEL;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
