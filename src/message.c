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

#അന്തര്ഭാവ "vim.h"

സ്ഥിതം ശൂന്യം add_സന്ദേശം_hist(char_u *s, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ attr);
സ്ഥിതം ശൂന്യം check_സന്ദേശം_hist(ശൂന്യം);
സ്ഥിതം ശൂന്യം hit_return_സന്ദേശം(ശൂന്യം);
സ്ഥിതം ശൂന്യം സന്ദേശം_home_replace_attr(char_u *fname, പൂർണ്ണസംഖ്യ attr);
സ്ഥിതം ശൂന്യം സന്ദേശം_puts_attr_len(അക്ഷരം *str, പൂർണ്ണസംഖ്യ maxlen, പൂർണ്ണസംഖ്യ attr);
സ്ഥിതം ശൂന്യം സന്ദേശം_puts_display(char_u *str, പൂർണ്ണസംഖ്യ maxlen, പൂർണ്ണസംഖ്യ attr, പൂർണ്ണസംഖ്യ recurse);
സ്ഥിതം ശൂന്യം സന്ദേശം_scroll_up(ശൂന്യം);
സ്ഥിതം ശൂന്യം inc_സന്ദേശം_scrolled(ശൂന്യം);
സ്ഥിതം ശൂന്യം store_sb_text(char_u **sb_str, char_u *s, പൂർണ്ണസംഖ്യ attr, പൂർണ്ണസംഖ്യ *sb_col, പൂർണ്ണസംഖ്യ finish);
സ്ഥിതം ശൂന്യം t_puts(പൂർണ്ണസംഖ്യ *t_col, char_u *t_s, char_u *s, പൂർണ്ണസംഖ്യ attr);
സ്ഥിതം ശൂന്യം സന്ദേശം_puts_printf(char_u *str, പൂർണ്ണസംഖ്യ maxlen);
സ്ഥിതം പൂർണ്ണസംഖ്യ do_more_prompt(പൂർണ്ണസംഖ്യ typed_char);
സ്ഥിതം ശൂന്യം സന്ദേശം_screen_putchar(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ attr);
സ്ഥിതം ശൂന്യം സന്ദേശം_moremsg(പൂർണ്ണസംഖ്യ full);
സ്ഥിതം പൂർണ്ണസംഖ്യ  സന്ദേശം_check_screen(ശൂന്യം);
സ്ഥിതം ശൂന്യം redir_write(char_u *s, പൂർണ്ണസംഖ്യ maxlen);
#ifdef FEAT_CON_DIALOG
സ്ഥിതം char_u *സന്ദേശം_show_console_dialog(char_u *message, char_u *buttons, പൂർണ്ണസംഖ്യ dfltbutton);
സ്ഥിതം പൂർണ്ണസംഖ്യ	confirm_സന്ദേശം_used = FALSE;	// displaying confirm_msg
സ്ഥിതം char_u	*confirm_സന്ദേശം = ശൂന്യ;		// ":confirm" message
സ്ഥിതം char_u	*confirm_സന്ദേശം_tail;		// tail of confirm_msg
സ്ഥിതം ശൂന്യം display_confirm_സന്ദേശം(ശൂന്യം);
#endif
#ifdef FEAT_EVAL
സ്ഥിതം പൂർണ്ണസംഖ്യ emsg_to_channel_log = FALSE;
#endif

ഘടന സന്ദേശം_hist
{
    ഘടന സന്ദേശം_hist	*next;
    char_u		*സന്ദേശം;
    പൂർണ്ണസംഖ്യ			attr;
};

സ്ഥിതം ഘടന സന്ദേശം_hist *first_സന്ദേശം_hist = ശൂന്യ;
സ്ഥിതം ഘടന സന്ദേശം_hist *last_സന്ദേശം_hist = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ സന്ദേശം_hist_len = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ സന്ദേശം_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#define MESSAGES_HIT_ENTER	0x001
#define MESSAGES_WAIT		0x002
#define MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#define MESSAGES_OPT_HIT_ENTER "hit-enter"
#define MESSAGES_OPT_WAIT "wait:"
#define MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
സ്ഥിതം പൂർണ്ണസംഖ്യ സന്ദേശം_flags = MESSAGES_HIT_ENTER | MESSAGES_HISTORY;
സ്ഥിതം പൂർണ്ണസംഖ്യ സന്ദേശം_wait = 0;

സ്ഥിതം FILE *verbose_fd = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ  verbose_did_open = FALSE;

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
    പൂർണ്ണസംഖ്യ
സന്ദേശം(അക്ഷരം *s)
{
    തിരികെ സന്ദേശം_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    പൂർണ്ണസംഖ്യ
verb_സന്ദേശം(അക്ഷരം *s)
{
    പൂർണ്ണസംഖ്യ		n;

    verbose_enter();
    n = സന്ദേശം_attr_keep(s, 0, FALSE);
    verbose_leave();

    തിരികെ n;
}

    പൂർണ്ണസംഖ്യ
സന്ദേശം_attr(അക്ഷരം *s, പൂർണ്ണസംഖ്യ attr)
{
    തിരികെ സന്ദേശം_attr_keep(s, attr, FALSE);
}

    പൂർണ്ണസംഖ്യ
സന്ദേശം_attr_keep(
    അക്ഷരം	*s,
    പൂർണ്ണസംഖ്യ		attr,
    പൂർണ്ണസംഖ്യ		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	entered = 0;
    പൂർണ്ണസംഖ്യ		retval;
    char_u	*buf = ശൂന്യ;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    എങ്കിൽ (!emsg_on_display && message_filtered((char_u *)s))
	തിരികെ TRUE;

#ifdef FEAT_EVAL
    എങ്കിൽ (attr == 0)
	set_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#endif

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    എങ്കിൽ (entered >= 3)
	തിരികെ TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    എങ്കിൽ ((char_u *)s != keep_സന്ദേശം
	    || (*s != '<'
		&& last_സന്ദേശം_hist != ശൂന്യ
		&& last_സന്ദേശം_hist->സന്ദേശം != ശൂന്യ
		&& STRCMP(s, last_സന്ദേശം_hist->സന്ദേശം) != 0))
	add_സന്ദേശം_hist((char_u *)s, -1, attr);

#ifdef FEAT_EVAL
    എങ്കിൽ (emsg_to_channel_log)
	// Write message in the channel log.
	ch_log(ശൂന്യ, "ERROR: %s", s);
#endif

    // Truncate the message if needed.
    സന്ദേശം_start();
    buf = സന്ദേശം_strtrunc((char_u *)s, FALSE);
    എങ്കിൽ (buf != ശൂന്യ)
	s = (അക്ഷരം *)buf;

    സന്ദേശം_outtrans_attr((char_u *)s, attr);
    സന്ദേശം_clr_eos();
    retval = സന്ദേശം_end();

    എങ്കിൽ (keep && retval && vim_strsize((char_u *)s)
		    < (പൂർണ്ണസംഖ്യ)(Rows - cmdline_row - 1) * cmdline_width + sc_col)
	set_keep_സന്ദേശം((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    തിരികെ retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
സന്ദേശം_strtrunc(
    char_u	*s,
    പൂർണ്ണസംഖ്യ		force)	    // always truncate
{
    char_u	*buf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		room;

    // May truncate message to avoid a hit-return prompt
    എങ്കിൽ ((!സന്ദേശം_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_active && സന്ദേശം_silent == 0) || force)
    {
	len = vim_strsize(s);
	എങ്കിൽ (സന്ദേശം_scrolled != 0
#ifdef HAS_MESSAGE_WINDOW
		|| in_echowindow
#endif
		)
	    // Use all the columns.
	    room = (പൂർണ്ണസംഖ്യ)(Rows - സന്ദേശം_row) * cmdline_width - 1;
	അല്ലെങ്കിൽ
	    // Use up to 'showcmd' column.
	    room = (പൂർണ്ണസംഖ്യ)(Rows - സന്ദേശം_row - 1) * cmdline_width + sc_col - 1;
	എങ്കിൽ (len > room && room > 0)
	{
	    എങ്കിൽ (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (room + 2) * 18;
	    അല്ലെങ്കിൽ എങ്കിൽ (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (room + 2) * 2;
	    അല്ലെങ്കിൽ
		len = room + 2;
	    buf = alloc(len);
	    എങ്കിൽ (buf != ശൂന്യ)
		trunc_string(s, buf, room, len);
	}
    }
    തിരികെ buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    ശൂന്യം
trunc_string(
    char_u	*s,
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		room_in,
    പൂർണ്ണസംഖ്യ		buflen)
{
    size_t	room = room_in - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    പൂർണ്ണസംഖ്യ		e;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		n;

    എങ്കിൽ (*s == NUL)
    {
	എങ്കിൽ (buflen > 0)
	    *buf = NUL;
	തിരികെ;
    }

    എങ്കിൽ (room_in < 3)
	room = 0;
    half = room / 2;

    // First part: Start of the string.
    വേണ്ടി (e = 0; len < half && e < buflen; ++e)
    {
	എങ്കിൽ (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    തിരികെ;
	}
	n = ptr2cells(s + e);
	എങ്കിൽ (len + n > half)
	    നിർത്തുക;
	len += n;
	buf[e] = s[e];
	എങ്കിൽ (has_mbyte)
	    വേണ്ടി (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		എങ്കിൽ (++e == buflen)
		    നിർത്തുക;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    എങ്കിൽ (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	ആകുമ്പോൾ (len + n > room)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (പൂർണ്ണസംഖ്യ)STRLEN(s);
	വേണ്ടി (;;)
	{
	    ചെയ്യുക
		half = half - utf_head_off(s, s + half - 1) - 1;
	    ആകുമ്പോൾ (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    എങ്കിൽ (len + n > room || half == 0)
		നിർത്തുക;
	    len += n;
	    i = (പൂർണ്ണസംഖ്യ)half;
	}
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (i = (പൂർണ്ണസംഖ്യ)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= room; --i)
	    len += n;
    }


    എങ്കിൽ (i <= e + 3)
    {
	// text fits without truncating
	എങ്കിൽ (s != buf)
	{
	    len = STRLEN(s);
	    എങ്കിൽ (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    എങ്കിൽ (len < 1)
		buf[e - 1] = NUL;
	    അല്ലെങ്കിൽ
		mch_memmove(buf + e, s + e, len);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	എങ്കിൽ (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    അല്ലെങ്കിൽ
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

പൂർണ്ണസംഖ്യ vim_snprintf(അക്ഷരം *str, size_t str_m, സ്ഥിരം അക്ഷരം *fmt, ...);

    പൂർണ്ണസംഖ്യ
smsg(സ്ഥിരം അക്ഷരം *s, ...)
{
    എങ്കിൽ (IObuff == ശൂന്യ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	തിരികെ സന്ദേശം((അക്ഷരം *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((അക്ഷരം *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    തിരികെ സന്ദേശം((അക്ഷരം *)IObuff);
}

    പൂർണ്ണസംഖ്യ
smsg_attr(പൂർണ്ണസംഖ്യ attr, സ്ഥിരം അക്ഷരം *s, ...)
{
    എങ്കിൽ (IObuff == ശൂന്യ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	തിരികെ സന്ദേശം_attr((അക്ഷരം *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((അക്ഷരം *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    തിരികെ സന്ദേശം_attr((അക്ഷരം *)IObuff, attr);
}

    പൂർണ്ണസംഖ്യ
smsg_attr_keep(പൂർണ്ണസംഖ്യ attr, സ്ഥിരം അക്ഷരം *s, ...)
{
    എങ്കിൽ (IObuff == ശൂന്യ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	തിരികെ സന്ദേശം_attr_keep((അക്ഷരം *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((അക്ഷരം *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    തിരികെ സന്ദേശം_attr_keep((അക്ഷരം *)IObuff, attr, TRUE);
}

#endif

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ	last_sourcing_lnum = 0;
സ്ഥിതം char_u   *last_sourcing_name = ശൂന്യ;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    ശൂന്യം
reset_last_sourcing(ശൂന്യം)
{
    VIM_CLEAR(last_sourcing_name);
    last_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
other_sourcing_name(ശൂന്യം)
{
    എങ്കിൽ (HAVE_SOURCING_INFO && SOURCING_NAME != ശൂന്യ)
    {
	എങ്കിൽ (last_sourcing_name != ശൂന്യ)
	    തിരികെ STRCMP(SOURCING_NAME, last_sourcing_name) != 0;
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    സ്ഥിതം char_u *
get_emsg_source(ശൂന്യം)
{
    char_u	*Buf, *p;

    എങ്കിൽ (HAVE_SOURCING_INFO && SOURCING_NAME != ശൂന്യ && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	എങ്കിൽ (sname == ശൂന്യ)
	    sname = SOURCING_NAME;

#ifdef FEAT_EVAL
	എങ്കിൽ (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	അല്ലെങ്കിൽ
#endif
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	എങ്കിൽ (Buf != ശൂന്യ)
	    sprintf((അക്ഷരം *)Buf, (അക്ഷരം *)p, sname);
	vim_free(tofree);
	തിരികെ Buf;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    സ്ഥിതം char_u *
get_emsg_lnum(ശൂന്യം)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    എങ്കിൽ (SOURCING_NAME != ശൂന്യ
	    && (other_sourcing_name() || SOURCING_LNUM != last_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	എങ്കിൽ (Buf != ശൂന്യ)
	    sprintf((അക്ഷരം *)Buf, (അക്ഷരം *)p, (long)SOURCING_LNUM);
	തിരികെ Buf;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    ശൂന്യം
സന്ദേശം_source(പൂർണ്ണസംഖ്യ attr)
{
    char_u	*p;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = FALSE;

    // Bail out if something called here causes an error.
    എങ്കിൽ (recursive)
	തിരികെ;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_source();
    എങ്കിൽ (p != ശൂന്യ)
    {
	സന്ദേശം_scroll = TRUE;  // this will take more than one line
	സന്ദേശം_attr((അക്ഷരം *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    എങ്കിൽ (p != ശൂന്യ)
    {
	സന്ദേശം_attr((അക്ഷരം *)p, HL_ATTR(HLF_N));
	vim_free(p);
	last_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    എങ്കിൽ (SOURCING_NAME == ശൂന്യ || other_sourcing_name())
    {
	VIM_CLEAR(last_sourcing_name);
	എങ്കിൽ (SOURCING_NAME != ശൂന്യ)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
emsg_not_now(ശൂന്യം)
{
    എങ്കിൽ ((emsg_off > 0 && vim_strchr(p_debug, 'm') == ശൂന്യ
					  && vim_strchr(p_debug, 't') == ശൂന്യ)
#ifdef FEAT_EVAL
	    || emsg_skip > 0
#endif
	    )
	തിരികെ TRUE;
    തിരികെ FALSE;
}

#എങ്കിൽ defined(FEAT_EVAL)
സ്ഥിതം garray_T ignore_error_list = GA_EMPTY;

    ശൂന്യം
ignore_error_for_testing(char_u *error)
{
    എങ്കിൽ (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, വലുപ്പം(char_u *), 1);

    എങ്കിൽ (STRCMP("RESET", error) == 0)
	ga_clear_strings(&ignore_error_list);
    അല്ലെങ്കിൽ
	ga_copy_string(&ignore_error_list, error);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ignore_error(സ്ഥിരം അക്ഷരം *സന്ദേശം)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < ignore_error_list.ga_len; ++i)
	എങ്കിൽ (strstr(സന്ദേശം,
		  (അക്ഷരം *)((char_u **)(ignore_error_list.ga_data))[i]) != ശൂന്യ)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}
#endif

#എങ്കിൽ !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    ശൂന്യം
do_perror(അക്ഷരം *സന്ദേശം)
{
    perror(സന്ദേശം);
    ++emsg_silent;
    emsg(സന്ദേശം);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
emsg_core(സ്ഥിരം അക്ഷരം *s)
{
    പൂർണ്ണസംഖ്യ		attr;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		r;
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		ignore = FALSE;
    പൂർണ്ണസംഖ്യ		severe;
#endif

#ifdef FEAT_EVAL
    // When testing some errors are turned into a normal message.
    എങ്കിൽ (ignore_error(s))
	// don't call msg() if it results in a dialog
	തിരികെ സന്ദേശം_use_printf() ? FALSE : സന്ദേശം((അക്ഷരം *)s);
#endif

    ++called_emsg;

#ifdef FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#endif

    എങ്കിൽ (!emsg_off || vim_strchr(p_debug, 't') != ശൂന്യ)
    {
#ifdef FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	എങ്കിൽ (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    എങ്കിൽ (!ignore)
		++did_emsg;
	    തിരികെ TRUE;
	}

	എങ്കിൽ (in_assert_fails && emsg_assert_fails_സന്ദേശം == ശൂന്യ)
	{
	    emsg_assert_fails_സന്ദേശം = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == ശൂന്യ ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#endif

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	എങ്കിൽ (emsg_silent != 0)
	{
#ifdef FEAT_EVAL
	    ++did_emsg_silent;
#endif
	    എങ്കിൽ (emsg_noredir == 0)
	    {
		സന്ദേശം_start();
		p = get_emsg_source();
		എങ്കിൽ (p != ശൂന്യ)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		എങ്കിൽ (p != ശൂന്യ)
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
	    എങ്കിൽ (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#endif
#ifdef FEAT_EVAL
	    ch_log(ശൂന്യ, "ERROR silent: %s", s);
#endif
	    തിരികെ TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	സന്ദേശം_silent = 0;
	cmd_silent = FALSE;

	എങ്കിൽ (global_busy)		// break :global command
	    ++global_busy;

	എങ്കിൽ (p_eb)
	    beep_flush();		// also includes flush_buffers()
	അല്ലെങ്കിൽ
	    flush_buffers(FLUSH_MINIMAL);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ifdef FEAT_EVAL
	++uncaught_emsg;
#endif
    }

#ifdef HAS_MESSAGE_WINDOW
    എങ്കിൽ (!in_echowindow)
#endif
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    എങ്കിൽ (സന്ദേശം_scrolled != 0)
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
    സന്ദേശം_scroll = TRUE;
    സന്ദേശം_source(attr);

    /*
     * Display the error message itself.
     */
    സന്ദേശം_nowait = FALSE;			// wait for this msg
    r = സന്ദേശം_attr((അക്ഷരം *)s, attr);

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = FALSE;
#endif
    തിരികെ r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    പൂർണ്ണസംഖ്യ
emsg(അക്ഷരം *s)
{
    // Skip this if not giving error messages at the moment.
    എങ്കിൽ (emsg_not_now())
	തിരികെ TRUE;

    തിരികെ emsg_core(s);
}

#ifndef PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    പൂർണ്ണസംഖ്യ
semsg(സ്ഥിരം അക്ഷരം *s, ...)
{
    // Skip this if not giving error messages at the moment.
    എങ്കിൽ (emsg_not_now())
	തിരികെ TRUE;

    എങ്കിൽ (IObuff == ശൂന്യ)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	തിരികെ emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((അക്ഷരം *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    തിരികെ emsg_core((അക്ഷരം *)IObuff);
}
#endif

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    ശൂന്യം
iemsg(അക്ഷരം *s)
{
    എങ്കിൽ (emsg_not_now())
	തിരികെ;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    emsg_core(s);
#എങ്കിൽ defined(ABORT_ON_INTERNAL_ERROR) && defined(FEAT_EVAL)
    set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    സന്ദേശം_putchar('\n');  // avoid overwriting the error message
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
    ശൂന്യം
siemsg(സ്ഥിരം അക്ഷരം *s, ...)
{
    എങ്കിൽ (emsg_not_now())
	തിരികെ;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    എങ്കിൽ (IObuff == ശൂന്യ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    അല്ലെങ്കിൽ
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((അക്ഷരം *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((അക്ഷരം *)IObuff);
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    സന്ദേശം_putchar('\n');  // avoid overwriting the error message
    out_flush();
    abort();
# endif
}
#endif

/*
 * Give an "Internal error" message.
 */
    ശൂന്യം
internal_error(അക്ഷരം *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    siemsg(_(e_internal_error_str), where);
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    ശൂന്യം
internal_error_no_abort(അക്ഷരം *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    semsg(_(e_internal_error_str), where);
}
#endif

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    ശൂന്യം
emsg_invreg(പൂർണ്ണസംഖ്യ name)
{
    semsg(_(e_invalid_register_name_str), transchar_buf(ശൂന്യ, name));
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    ശൂന്യം
emsg_namelen(അക്ഷരം *സന്ദേശം, char_u *name, പൂർണ്ണസംഖ്യ len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(സന്ദേശം, copy == ശൂന്യ ? "NULL" : (അക്ഷരം *)copy);
    vim_free(copy);
}
#endif

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    അക്ഷരം *
സന്ദേശം_trunc_attr(അക്ഷരം *s, പൂർണ്ണസംഖ്യ force, പൂർണ്ണസംഖ്യ attr)
{
    പൂർണ്ണസംഖ്യ		n;
    അക്ഷരം	*ts;

    // Add message to history before truncating
    add_സന്ദേശം_hist((char_u *)s, -1, attr);

    ts = (അക്ഷരം *)സന്ദേശം_may_trunc(force, (char_u *)s);

    സന്ദേശം_hist_off = TRUE;
    n = സന്ദേശം_attr(ts, attr);
    സന്ദേശം_hist_off = FALSE;

    എങ്കിൽ (n)
	തിരികെ ts;
    തിരികെ ശൂന്യ;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
സന്ദേശം_may_trunc(പൂർണ്ണസംഖ്യ force, char_u *s)
{
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		room;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    room = (പൂർണ്ണസംഖ്യ)(Rows - cmdline_row - 1) * cmdline_width + sc_col - 1;
    എങ്കിൽ (room > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_active))
	    && (n = (പൂർണ്ണസംഖ്യ)STRLEN(s) - room) > 0)
    {
	എങ്കിൽ (has_mbyte)
	{
	    പൂർണ്ണസംഖ്യ	size = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    എങ്കിൽ (size <= room)
		തിരികെ s;

	    വേണ്ടി (n = 0; size >= room; )
	    {
		size -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    തിരികെ s;
}

    സ്ഥിതം ശൂന്യം
add_സന്ദേശം_hist(
    char_u	*s,
    പൂർണ്ണസംഖ്യ		len,		// -1 for undetermined length
    പൂർണ്ണസംഖ്യ		attr)
{
    ഘടന സന്ദേശം_hist *p;

    എങ്കിൽ (സന്ദേശം_hist_off || സന്ദേശം_silent != 0)
	തിരികെ;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(ഘടന സന്ദേശം_hist);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (len < 0)
	len = (പൂർണ്ണസംഖ്യ)STRLEN(s);
    // remove leading and trailing newlines
    ആകുമ്പോൾ (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    ആകുമ്പോൾ (len > 0 && s[len - 1] == '\n')
	--len;
    p->സന്ദേശം = vim_strnsave(s, len);
    p->next = ശൂന്യ;
    p->attr = attr;
    എങ്കിൽ (last_സന്ദേശം_hist != ശൂന്യ)
	last_സന്ദേശം_hist->next = p;
    last_സന്ദേശം_hist = p;
    എങ്കിൽ (first_സന്ദേശം_hist == ശൂന്യ)
	first_സന്ദേശം_hist = last_സന്ദേശം_hist;
    ++സന്ദേശം_hist_len;

    check_സന്ദേശം_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    പൂർണ്ണസംഖ്യ
delete_first_സന്ദേശം(ശൂന്യം)
{
    ഘടന സന്ദേശം_hist *p;

    എങ്കിൽ (സന്ദേശം_hist_len <= 0)
	തിരികെ FAIL;
    p = first_സന്ദേശം_hist;
    first_സന്ദേശം_hist = p->next;
    എങ്കിൽ (first_സന്ദേശം_hist == ശൂന്യ)
	last_സന്ദേശം_hist = ശൂന്യ;  // history is empty
    vim_free(p->സന്ദേശം);
    vim_free(p);
    --സന്ദേശം_hist_len;
    തിരികെ OK;
}

    സ്ഥിതം ശൂന്യം
check_സന്ദേശം_hist(ശൂന്യം)
{
    // Don't let the message history get too big
    ആകുമ്പോൾ (സന്ദേശം_hist_len > 0 && സന്ദേശം_hist_len > സന്ദേശം_hist_max)
	(ശൂന്യം)delete_first_സന്ദേശം();
}


    പൂർണ്ണസംഖ്യ
messagesopt_changed(ശൂന്യം)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		messages_flags_new = 0;
    പൂർണ്ണസംഖ്യ		messages_wait_new = 0;
    പൂർണ്ണസംഖ്യ		messages_history_new = 0;

    p = p_mopt;
    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (STRNCMP(p, MESSAGES_OPT_HIT_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER);
	    messages_flags_new |= MESSAGES_HIT_ENTER;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	എങ്കിൽ (*p != ',' && *p != NUL)
	    തിരികെ FAIL;
	എങ്കിൽ (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    എങ്കിൽ (!(messages_flags_new & (MESSAGES_HIT_ENTER | MESSAGES_WAIT)))
	തിരികെ FAIL;

    // "history" must be set
    എങ്കിൽ (!(messages_flags_new & MESSAGES_HISTORY))
	തിരികെ FAIL;

    എങ്കിൽ (messages_history_new < 0 || messages_history_new > 10000)
	തിരികെ FAIL;

    എങ്കിൽ (messages_wait_new < 0 || messages_wait_new > 10000)
	തിരികെ FAIL;

    സന്ദേശം_flags = messages_flags_new;
    സന്ദേശം_wait = messages_wait_new;

    സന്ദേശം_hist_max = messages_history_new;
    check_സന്ദേശം_hist();

    തിരികെ OK;
}

/*
 * ":messages" command.
 */
    ശൂന്യം
ex_messages(exarg_T *eap)
{
    ഘടന സന്ദേശം_hist *p;
    char_u	    *s;
    പൂർണ്ണസംഖ്യ		    c = 0;

    എങ്കിൽ (STRCMP(eap->arg, "clear") == 0)
    {
	പൂർണ്ണസംഖ്യ keep = eap->addr_എണ്ണം == 0 ? 0 : eap->line2;

	ആകുമ്പോൾ (സന്ദേശം_hist_len > keep)
	    (ശൂന്യം)delete_first_സന്ദേശം();
	തിരികെ;
    }

    എങ്കിൽ (*eap->arg != NUL)
    {
	emsg(_(e_invalid_argument));
	തിരികെ;
    }

    സന്ദേശം_hist_off = TRUE;

    p = first_സന്ദേശം_hist;
    എങ്കിൽ (eap->addr_എണ്ണം != 0)
    {
	// Count total messages
	വേണ്ടി (; p != ശൂന്യ && !got_int; p = p->next)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	വേണ്ടി (p = first_സന്ദേശം_hist; p != ശൂന്യ && !got_int && c > 0;
						    p = p->next, c--);
    }

    എങ്കിൽ (p == first_സന്ദേശം_hist)
    {
#ifdef FEAT_MULTI_LANG
	s = get_mess_lang();
#അല്ലെങ്കിൽ
	s = mch_getenv((char_u *)"LANG");
#endif
	എങ്കിൽ (s != ശൂന്യ && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    സന്ദേശം_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    വേണ്ടി (; p != ശൂന്യ && !got_int; p = p->next)
	എങ്കിൽ (p->സന്ദേശം != ശൂന്യ)
	    സന്ദേശം_attr((അക്ഷരം *)p->സന്ദേശം, p->attr);

    സന്ദേശം_hist_off = FALSE;
}

#എങ്കിൽ defined(FEAT_CON_DIALOG) || defined(FIND_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    ശൂന്യം
സന്ദേശം_end_prompt(ശൂന്യം)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_row = സന്ദേശം_row;
    സന്ദേശം_col = 0;
    സന്ദേശം_clr_eos();
    വരികൾ_ഇടത് = -1;
}
#endif

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    ശൂന്യം
wait_return(പൂർണ്ണസംഖ്യ redraw)
{
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		oldState;
    പൂർണ്ണസംഖ്യ		tmpState;
    പൂർണ്ണസംഖ്യ		had_got_int;
    പൂർണ്ണസംഖ്യ		save_reg_recording;
    FILE	*save_scriptout;

    എങ്കിൽ (redraw == TRUE)
	set_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    എങ്കിൽ (സന്ദേശം_silent != 0)
	തിരികെ;
#ifdef HAS_MESSAGE_WINDOW
    എങ്കിൽ (in_echowindow)
	തിരികെ;
#endif

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    എങ്കിൽ (vgetc_busy > 0)
	തിരികെ;
    need_wait_return = TRUE;
    എങ്കിൽ (no_wait_return)
    {
	എങ്കിൽ (!exmode_active)
	    cmdline_row = സന്ദേശം_row;
	തിരികെ;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    എങ്കിൽ (quit_more)
    {
	c = CAR;		// just pretend CR was hit
	quit_more = FALSE;
	got_int = FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (exmode_active)
    {
	സന്ദേശം_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!stuff_empty())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    അല്ലെങ്കിൽ
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = MODE_HITRETURN;
	setmouse();
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#endif
	cmdline_row = സന്ദേശം_row;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	എങ്കിൽ (need_check_timestamps)
	    check_timestamps(FALSE);

	എങ്കിൽ (സന്ദേശം_flags & MESSAGES_HIT_ENTER)
	{
	    hit_return_സന്ദേശം();

	    ചെയ്യുക
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
		++allow_താക്കോലുകൾ;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = ശൂന്യ;
		c = safe_vgetc();
		എങ്കിൽ (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_താക്കോലുകൾ;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ifdef FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		എങ്കിൽ (താക്കോൽ_Typed && c == Ctrl_Y && clip_star.state == SELECT_DONE)
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
		എങ്കിൽ (താക്കോൽ_Typed && p_more && !p_cp)
		{
		    എങ്കിൽ (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_UP || c == K_PAGEUP)
		    {
			എങ്കിൽ (സന്ദേശം_scrolled > Rows)
			    // scroll back to show older messages
			    do_more_prompt(c);
			അല്ലെങ്കിൽ
			{
			    സന്ദേശം_didout = FALSE;
			    c = K_IGNORE;
			    സന്ദേശം_col =
#ifdef FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_width - 1 :
#endif
				0;
			}
			എങ്കിൽ (quit_more)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_more = FALSE;
			    got_int = FALSE;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    hit_return_സന്ദേശം();
			}
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (സന്ദേശം_scrolled > Rows - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_DOWN || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } ആകുമ്പോൾ ((had_got_int && c == Ctrl_C)
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
				    && mouse_row < സന്ദേശം_row
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    എങ്കിൽ (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(ശൂന്യം)jump_to_mouse(MOUSE_SETPOS, ശൂന്യ, 0);
	    അല്ലെങ്കിൽ എങ്കിൽ (!താക്കോൽ_Typed || (vim_strchr((char_u *)"\r\n ", c) == ശൂന്യ
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_mask);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	അല്ലെങ്കിൽ
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(സന്ദേശം_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    എങ്കിൽ (c == ':' || c == '?' || c == '/')
    {
	എങ്കിൽ (!exmode_active)
	    cmdline_row = സന്ദേശം_row;
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
    സന്ദേശം_check();

#ifndef MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    എങ്കിൽ (swapping_screen() && !termcap_active)
	newline_on_exit = TRUE;
#endif

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    വരികൾ_ഇടത് = -1;		// reset lines_left at next msg_start()
    reset_last_sourcing();
    എങ്കിൽ (keep_സന്ദേശം != ശൂന്യ && vim_strsize(keep_സന്ദേശം) >=
			    (Rows - cmdline_row - 1) * cmdline_width + sc_col)
	VIM_CLEAR(keep_സന്ദേശം);	    // don't redisplay message, it's too long

    എങ്കിൽ (tmpState == MODE_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!skip_redraw
	    && (redraw == TRUE || (സന്ദേശം_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    സ്ഥിതം ശൂന്യം
hit_return_സന്ദേശം(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		save_p_more = p_more;

    p_more = FALSE;	// don't want to see this message when scrolling back
    എങ്കിൽ (സന്ദേശം_didout)	// start on a new line
	സന്ദേശം_putchar('\n');
    എങ്കിൽ (got_int)
	സന്ദേശം_puts(_("Interrupt: "));

    സന്ദേശം_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    എങ്കിൽ (!സന്ദേശം_use_printf())
	സന്ദേശം_clr_eos();
    p_more = save_p_more;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    ശൂന്യം
set_keep_സന്ദേശം(char_u *s, പൂർണ്ണസംഖ്യ attr)
{
    vim_free(keep_സന്ദേശം);
    എങ്കിൽ (s != ശൂന്യ && സന്ദേശം_silent == 0)
	keep_സന്ദേശം = vim_strsave(s);
    അല്ലെങ്കിൽ
	keep_സന്ദേശം = ശൂന്യ;
    keep_സന്ദേശം_more = FALSE;
    keep_സന്ദേശം_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    ശൂന്യം
set_keep_സന്ദേശം_from_hist(ശൂന്യം)
{
    എങ്കിൽ (keep_സന്ദേശം == ശൂന്യ && last_സന്ദേശം_hist != ശൂന്യ && സന്ദേശം_scrolled == 0
						      && (State & MODE_NORMAL))
	set_keep_സന്ദേശം(last_സന്ദേശം_hist->സന്ദേശം, last_സന്ദേശം_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    ശൂന്യം
സന്ദേശം_start(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		did_return = FALSE;

    എങ്കിൽ (സന്ദേശം_row < cmdline_row)
	സന്ദേശം_row = cmdline_row;

    എങ്കിൽ (!സന്ദേശം_silent)
    {
	VIM_CLEAR(keep_സന്ദേശം);
	need_fileinfo = FALSE;
    }

#ifdef FEAT_EVAL
    എങ്കിൽ (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	സന്ദേശം_clr_eos();
    }
#endif

#ifdef HAS_MESSAGE_WINDOW
    എങ്കിൽ (in_echowindow)
    {
	എങ്കിൽ (popup_message_win_visible()
		    && ((സന്ദേശം_col > 0 && (സന്ദേശം_scroll || !full_screen))
			|| in_echowindow))
	{
	    win_T *wp = popup_get_message_win();

	    // start a new line
	    curbuf = wp->w_buffer;
	    ml_append(wp->w_buffer->b_ml.ml_വരി_എണ്ണം,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_buffer;
	}
	സന്ദേശം_col = 0;
    }
    അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (!സന്ദേശം_scroll && full_screen)	// overwrite last message
    {
	സന്ദേശം_row = cmdline_row;
	സന്ദേശം_col =
#ifdef FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_width - 1 :
#endif
	    0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (സന്ദേശം_didout || in_echowindow)
    {
	// start message on next line
	സന്ദേശം_putchar('\n');
	did_return = TRUE;
	എങ്കിൽ (exmode_active != EXMODE_NORMAL)
	    cmdline_row = സന്ദേശം_row;
    }
    എങ്കിൽ (!സന്ദേശം_didany || വരികൾ_ഇടത് < 0)
	സന്ദേശം_starthere();
    എങ്കിൽ (സന്ദേശം_silent == 0)
    {
	സന്ദേശം_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    എങ്കിൽ (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    ശൂന്യം
സന്ദേശം_starthere(ശൂന്യം)
{
    വരികൾ_ഇടത് = cmdline_row;
    സന്ദേശം_didany = FALSE;
}

    ശൂന്യം
സന്ദേശം_putchar(പൂർണ്ണസംഖ്യ c)
{
    സന്ദേശം_putchar_attr(c, 0);
}

    ശൂന്യം
സന്ദേശം_putchar_attr(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    എങ്കിൽ (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    അല്ലെങ്കിൽ
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    സന്ദേശം_puts_attr((അക്ഷരം *)buf, attr);
}

    ശൂന്യം
സന്ദേശം_outnum(long n)
{
    അക്ഷരം	buf[20];

    sprintf(buf, "%ld", n);
    സന്ദേശം_puts(buf);
}

    ശൂന്യം
സന്ദേശം_home_replace(char_u *fname)
{
    സന്ദേശം_home_replace_attr(fname, 0);
}

#എങ്കിൽ defined(FEAT_FIND_ID)
    ശൂന്യം
സന്ദേശം_home_replace_hl(char_u *fname)
{
    സന്ദേശം_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#endif

    സ്ഥിതം ശൂന്യം
സന്ദേശം_home_replace_attr(char_u *fname, പൂർണ്ണസംഖ്യ attr)
{
    char_u	*name;

    name = home_replace_save(ശൂന്യ, fname);
    എങ്കിൽ (name != ശൂന്യ)
	സന്ദേശം_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    പൂർണ്ണസംഖ്യ
സന്ദേശം_outtrans(char_u *str)
{
    തിരികെ സന്ദേശം_outtrans_attr(str, 0);
}

    പൂർണ്ണസംഖ്യ
സന്ദേശം_outtrans_attr(char_u *str, പൂർണ്ണസംഖ്യ attr)
{
    തിരികെ സന്ദേശം_outtrans_len_attr(str, (പൂർണ്ണസംഖ്യ)STRLEN(str), attr);
}

    പൂർണ്ണസംഖ്യ
സന്ദേശം_outtrans_len(char_u *str, പൂർണ്ണസംഖ്യ len)
{
    തിരികെ സന്ദേശം_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
സന്ദേശം_outtrans_one(char_u *p, പൂർണ്ണസംഖ്യ attr)
{
    പൂർണ്ണസംഖ്യ		l;

    എങ്കിൽ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	സന്ദേശം_outtrans_len_attr(p, l, attr);
	തിരികെ p + l;
    }
    സന്ദേശം_puts_attr((അക്ഷരം *)transchar_byte_buf(ശൂന്യ, *p), attr);
    തിരികെ p + 1;
}

    പൂർണ്ണസംഖ്യ
സന്ദേശം_outtrans_len_attr(char_u *msgstr, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ attr)
{
    പൂർണ്ണസംഖ്യ		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_start = msgstr;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		mb_l;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    എങ്കിൽ (attr == 0)
	attr = HL_ATTR(HLF_സന്ദേശം);

    // if MSG_HIST flag set, add message to history
    എങ്കിൽ (attr & സന്ദേശം_HIST)
    {
	add_സന്ദേശം_hist(str, len, attr);
	attr &= ~സന്ദേശം_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    എങ്കിൽ (സന്ദേശം_silent == 0 && len > 0 && സന്ദേശം_row >= cmdline_row && സന്ദേശം_col == 0)
    {
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    എങ്കിൽ (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	സന്ദേശം_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    ആകുമ്പോൾ (--len >= 0 && !got_int)
    {
	എങ്കിൽ (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	അല്ലെങ്കിൽ
	    mb_l = 1;
	എങ്കിൽ (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    എങ്കിൽ (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    അല്ലെങ്കിൽ
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		എങ്കിൽ (str > plain_start)
		    സന്ദേശം_puts_attr_len((അക്ഷരം *)plain_start,
					       (പൂർണ്ണസംഖ്യ)(str - plain_start), attr);
		plain_start = str + mb_l;
		സന്ദേശം_puts_attr((അക്ഷരം *)transchar_buf(ശൂന്യ, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	അല്ലെങ്കിൽ
	{
	    s = transchar_byte_buf(ശൂന്യ, *str);
	    എങ്കിൽ (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		എങ്കിൽ (str > plain_start)
		    സന്ദേശം_puts_attr_len((അക്ഷരം *)plain_start,
					       (പൂർണ്ണസംഖ്യ)(str - plain_start), attr);
		plain_start = str + 1;
		സന്ദേശം_puts_attr((അക്ഷരം *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (പൂർണ്ണസംഖ്യ)STRLEN(s);
	    }
	    അല്ലെങ്കിൽ
		++retval;
	    ++str;
	}
    }

    എങ്കിൽ (str > plain_start && !got_int)
	// print the printable chars at the end
	സന്ദേശം_puts_attr_len((അക്ഷരം *)plain_start, (പൂർണ്ണസംഖ്യ)(str - plain_start), attr);

    got_int |= save_got_int;

    തിരികെ retval;
}

#എങ്കിൽ defined(FEAT_QUICKFIX)
    ശൂന്യം
സന്ദേശം_make(char_u *arg)
{
    പൂർണ്ണസംഖ്യ	    i;
    സ്ഥിതം char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    വേണ്ടി (i = 5; *arg && i >= 0; --i)
	എങ്കിൽ (*arg++ != str[i])
	    നിർത്തുക;
    എങ്കിൽ (i < 0)
    {
	സന്ദേശം_putchar('\n');
	വേണ്ടി (i = 0; rs[i]; ++i)
	    സന്ദേശം_putchar(rs[i] - 3);
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
    പൂർണ്ണസംഖ്യ
സന്ദേശം_outtrans_special(
    char_u	*strstart,
    പൂർണ്ണസംഖ്യ		from,	// TRUE for lhs of a mapping
    പൂർണ്ണസംഖ്യ		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    പൂർണ്ണസംഖ്യ		retval = 0;
    അക്ഷരം	*text;
    പൂർണ്ണസംഖ്യ		attr;
    പൂർണ്ണസംഖ്യ		len;

    attr = HL_ATTR(HLF_8);
    ആകുമ്പോൾ (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	എങ്കിൽ ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    text = "<Space>";
	    ++str;
	}
	അല്ലെങ്കിൽ
	    text = (അക്ഷരം *)str2special(&str, from, FALSE);
	എങ്കിൽ (text[0] != NUL && text[1] == NUL)
	    // single-byte character or illegal byte
	    text = (അക്ഷരം *)transchar_byte_buf(ശൂന്യ, (char_u)text[0]);
	len = vim_strsize((char_u *)text);
	എങ്കിൽ (maxlen > 0 && retval + len >= maxlen)
	    നിർത്തുക;
	// Highlight special keys
	സന്ദേശം_puts_attr(text, len > 1
		&& (*mb_ptr2len)((char_u *)text) <= 1 ? attr : 0);
	retval += len;
    }
    തിരികെ retval;
}

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    പൂർണ്ണസംഖ്യ	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    പൂർണ്ണസംഖ്യ	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    ആകുമ്പോൾ (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    തിരികെ (char_u *)ga.ga_data;
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
    പൂർണ്ണസംഖ്യ		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    പൂർണ്ണസംഖ്യ		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    പൂർണ്ണസംഖ്യ			c;
    സ്ഥിതം char_u	buf[7];
    char_u		*str = *sp;
    പൂർണ്ണസംഖ്യ			modifiers = 0;
    പൂർണ്ണസംഖ്യ			special = FALSE;

    എങ്കിൽ (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	എങ്കിൽ (p != ശൂന്യ)
	    തിരികെ p;
    }

    c = *str;
    എങ്കിൽ ((c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	) && str[1] != NUL && str[2] != NUL)
    {
	എങ്കിൽ (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	എങ്കിൽ ((c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = TO_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	എങ്കിൽ (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    എങ്കിൽ (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	എങ്കിൽ (p != ശൂന്യ)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	അല്ലെങ്കിൽ
	    // illegal byte
	    *sp = str + 1;
    }
    അല്ലെങ്കിൽ
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    എങ്കിൽ (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	തിരികെ get_special_താക്കോൽ_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    തിരികെ buf;
}

/*
 * Translate a key sequence into special key names.
 */
    ശൂന്യം
str2specialbuf(char_u *sp, char_u *buf, പൂർണ്ണസംഖ്യ len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    ആകുമ്പോൾ (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	എങ്കിൽ (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }
}

/*
 * print line for :print or :list command
 */
    ശൂന്യം
സന്ദേശം_prt_വരി(char_u *s, പൂർണ്ണസംഖ്യ list)
{
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		col = 0;
    പൂർണ്ണസംഖ്യ		n_extra = 0;
    പൂർണ്ണസംഖ്യ		c_extra = 0;
    പൂർണ്ണസംഖ്യ		c_final = 0;
    char_u	*p_extra = ശൂന്യ;	    // init to make SASC shut up
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		attr = 0;
    char_u	*trail = ശൂന്യ;
    char_u	*lead = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		in_multispace = FALSE;
    പൂർണ്ണസംഖ്യ		multispace_pos = 0;
    പൂർണ്ണസംഖ്യ		l;
    char_u	buf[MB_MAXBYTES + 1];

    എങ്കിൽ (curwin->w_p_list)
	list = TRUE;

    എങ്കിൽ (list)
    {
	// find start of trailing whitespace
	എങ്കിൽ (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    ആകുമ്പോൾ (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	എങ്കിൽ (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != ശൂന്യ
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    ആകുമ്പോൾ (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    എങ്കിൽ (*lead == NUL)
		lead = ശൂന്യ;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    എങ്കിൽ (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	സന്ദേശം_putchar(' ');

    ആകുമ്പോൾ (!got_int)
    {
	എങ്കിൽ (n_extra > 0)
	{
	    --n_extra;
	    എങ്കിൽ (n_extra == 0 && c_final)
		c = c_final;
	    അല്ലെങ്കിൽ എങ്കിൽ (c_extra)
		c = c_extra;
	    അല്ലെങ്കിൽ
		c = *p_extra++;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    col += (*mb_ptr2cells)(s);
	    എങ്കിൽ (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		പൂർണ്ണസംഖ്യ len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    അല്ലെങ്കിൽ
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    സന്ദേശം_puts((അക്ഷരം *)buf);
	    s += l;
	    തുടരുക;
	}
	അല്ലെങ്കിൽ
	{
	    attr = 0;
	    c = *s++;
	    എങ്കിൽ (list)
	    {
		in_multispace = c == ' ' && (*s == ' '
						 || (col > 0 && s[-2] == ' '));
		എങ്കിൽ (!in_multispace)
		    multispace_pos = 0;
	    }
	    എങ്കിൽ (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ifdef FEAT_VARTABS
		n_extra = tabstop_padding(col, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#അല്ലെങ്കിൽ
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
#endif
		എങ്കിൽ (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		അല്ലെങ്കിൽ
		{
		    പൂർണ്ണസംഖ്യ lcs_tab1 = curwin->w_lcs_chars.tab1;
		    പൂർണ്ണസംഖ്യ lcs_tab2 = curwin->w_lcs_chars.tab2;
		    പൂർണ്ണസംഖ്യ lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    എങ്കിൽ (lead != ശൂന്യ && s <= lead
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
	    അല്ലെങ്കിൽ എങ്കിൽ (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(ശൂന്യ, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c == ' ')
	    {
		എങ്കിൽ (lead != ശൂന്യ && s <= lead && in_multispace
			&& curwin->w_lcs_chars.leadmultispace != ശൂന്യ)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    എങ്കിൽ (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (lead != ശൂന്യ && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (trail != ശൂന്യ && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (in_multispace
			&& curwin->w_lcs_chars.multispace != ശൂന്യ)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    എങ്കിൽ (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	എങ്കിൽ (c == NUL)
	    നിർത്തുക;

	സന്ദേശം_putchar_attr(c, attr);
	col++;
    }
    സന്ദേശം_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    സ്ഥിതം char_u *
screen_puts_mbyte(char_u *s, പൂർണ്ണസംഖ്യ l, പൂർണ്ണസംഖ്യ attr)
{
    പൂർണ്ണസംഖ്യ		cw;

    സന്ദേശം_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    എങ്കിൽ (cw > 1 && (
#ifdef FEAT_RIGHTLEFT
		cmdmsg_rl ? സന്ദേശം_col <= 1 :
#endif
		സന്ദേശം_col == cmdline_width - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	സന്ദേശം_screen_putchar('>', HL_ATTR(HLF_AT));
	തിരികെ s;
    }

    screen_puts_len(s, l, സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col, attr);
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (cmdmsg_rl)
    {
	സന്ദേശം_col -= cw;
	എങ്കിൽ (സന്ദേശം_col == 0)
	{
	    സന്ദേശം_col = cmdline_width;
	    ++സന്ദേശം_row;
	}
    }
    അല്ലെങ്കിൽ
#endif
    {
	സന്ദേശം_col += cw;
	എങ്കിൽ (സന്ദേശം_col >= cmdline_width)
	{
	    സന്ദേശം_col = 0;
	    ++സന്ദേശം_row;
	}
    }
    തിരികെ s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    ശൂന്യം
സന്ദേശം_puts(അക്ഷരം *s)
{
    സന്ദേശം_puts_attr(s, 0);
}

    ശൂന്യം
സന്ദേശം_puts_title(അക്ഷരം *s)
{
    സന്ദേശം_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    സ്ഥിതം ശൂന്യം
സന്ദേശം_outtrans_long_len_attr(char_u *longstr, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ attr)
{
    പൂർണ്ണസംഖ്യ		slen = len;
    പൂർണ്ണസംഖ്യ		room;

    room = cmdline_width - സന്ദേശം_col;
    എങ്കിൽ (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	സന്ദേശം_outtrans_len_attr(longstr, slen, attr);
	സന്ദേശം_puts_attr("...", HL_ATTR(HLF_8));
    }
    സന്ദേശം_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    ശൂന്യം
സന്ദേശം_outtrans_long_attr(char_u *longstr, പൂർണ്ണസംഖ്യ attr)
{
    സന്ദേശം_outtrans_long_len_attr(longstr, (പൂർണ്ണസംഖ്യ)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    ശൂന്യം
സന്ദേശം_puts_attr(അക്ഷരം *s, പൂർണ്ണസംഖ്യ attr)
{
    സന്ദേശം_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    സ്ഥിതം ശൂന്യം
സന്ദേശം_puts_attr_len(അക്ഷരം *str, പൂർണ്ണസംഖ്യ maxlen, പൂർണ്ണസംഖ്യ attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    എങ്കിൽ (സന്ദേശം_silent != 0)
	തിരികെ;

    എങ്കിൽ (attr == 0)
	attr = HL_ATTR(HLF_സന്ദേശം);

    // if MSG_HIST flag set, add message to history
    എങ്കിൽ ((attr & സന്ദേശം_HIST) && maxlen < 0)
    {
	add_സന്ദേശം_hist((char_u *)str, -1, attr);
	attr &= ~സന്ദേശം_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    എങ്കിൽ (സന്ദേശം_scrolled != 0 && !സന്ദേശം_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    സന്ദേശം_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    എങ്കിൽ (സന്ദേശം_use_printf())
	സന്ദേശം_puts_printf((char_u *)str, maxlen);
    അല്ലെങ്കിൽ
	സന്ദേശം_puts_display((char_u *)str, maxlen, attr, FALSE);

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
    സ്ഥിതം ശൂന്യം
put_സന്ദേശം_win(win_T *wp, പൂർണ്ണസംഖ്യ where, char_u *t_s, char_u *end, linenr_T lnum)
{
    char_u  *p;

    എങ്കിൽ (where == PUT_BELOW)
    {
	എങ്കിൽ (*end != NUL)
	{
	    p = vim_strnsave(t_s, end - t_s);
	    എങ്കിൽ (p == ശൂന്യ)
		തിരികെ;
	}
	അല്ലെങ്കിൽ
	    p = t_s;
	ml_append_buf(wp->w_buffer, lnum, p, (colnr_T)0, FALSE);
	എങ്കിൽ (p != t_s)
	    vim_free(p);
    }
    അല്ലെങ്കിൽ
    {
	char_u *newp;

	curbuf = wp->w_buffer;
	എങ്കിൽ (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    എങ്കിൽ (newp == ശൂന്യ)
		തിരികെ;
	    എങ്കിൽ (*end != NUL)
		newp[STRLEN(ml_get(lnum)) + (end - t_s)] = NUL;
	}
	അല്ലെങ്കിൽ
	{
	    newp = vim_strnsave(t_s, end - t_s);
	    എങ്കിൽ (newp == ശൂന്യ)
		തിരികെ;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_buffer;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    സന്ദേശം_col += (പൂർണ്ണസംഖ്യ)(end - t_s);
}
#endif

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    സ്ഥിതം ശൂന്യം
സന്ദേശം_puts_display(
    char_u	*str,
    പൂർണ്ണസംഖ്യ		maxlen,
    പൂർണ്ണസംഖ്യ		attr,
    പൂർണ്ണസംഖ്യ		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    പൂർണ്ണസംഖ്യ		t_col = 0;	// screen cells todo, 0 when "t_s" not used
    പൂർണ്ണസംഖ്യ		l;
    പൂർണ്ണസംഖ്യ		cw;
    char_u	*sb_str = str;
    പൂർണ്ണസംഖ്യ		sb_col = സന്ദേശം_col;
    പൂർണ്ണസംഖ്യ		wrap;
    പൂർണ്ണസംഖ്യ		did_last_char;
#ifdef HAS_MESSAGE_WINDOW
    പൂർണ്ണസംഖ്യ		where = PUT_APPEND;
    win_T	*സന്ദേശം_win = ശൂന്യ;
    linenr_T    lnum = 1;

    എങ്കിൽ (in_echowindow)
    {
	സന്ദേശം_win = popup_get_message_win();

	എങ്കിൽ (സന്ദേശം_win != ശൂന്യ)
	{
	    എങ്കിൽ (!popup_message_win_visible())
	    {
		എങ്കിൽ (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    സന്ദേശം_win = ശൂന്യ;
		}
		അല്ലെങ്കിൽ
		{
		    // currently hidden, make it empty
		    curbuf = സന്ദേശം_win->w_buffer;
		    ആകുമ്പോൾ ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
			ml_delete(1);
		    curbuf = curwin->w_buffer;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		lnum = സന്ദേശം_win->w_buffer->b_ml.ml_വരി_എണ്ണം;
		എങ്കിൽ (സന്ദേശം_col == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#endif

    did_wait_return = FALSE;
    ആകുമ്പോൾ ((maxlen < 0 || (പൂർണ്ണസംഖ്യ)(s - str) < maxlen) && *s != NUL)
    {
#ifdef HAS_MESSAGE_WINDOW
	// For echowindow, use full width; for regular messages, leave last column
	പൂർണ്ണസംഖ്യ wrap_col = സന്ദേശം_win != ശൂന്യ ? cmdline_width : cmdline_width - 1;
#അല്ലെങ്കിൽ
	പൂർണ്ണസംഖ്യ wrap_col = cmdline_width - 1;
#endif

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	എങ്കിൽ (!recurse && സന്ദേശം_row >= Rows - 1 && (*s == '\n' || (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			സന്ദേശം_col <= 1
		      || (*s == TAB && സന്ദേശം_col <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && സന്ദേശം_col <= 2))
		    :
#endif
		      ((*s != '\r' && സന്ദേശം_col + t_col >= wrap_col)
		       || (*s == TAB && സന്ദേശം_col + t_col
			   >= (wrap_col & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && സന്ദേശം_col + t_col >= wrap_col - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    എങ്കിൽ (t_col > 0)
	    {
		// output postponed text
#ifdef HAS_MESSAGE_WINDOW
		എങ്കിൽ (സന്ദേശം_win != ശൂന്യ)
		{
		    put_സന്ദേശം_win(സന്ദേശം_win, where, t_s, s, lnum);
		    എങ്കിൽ (where == PUT_BELOW)
			++lnum;
		    t_col = 0;
		    where = PUT_BELOW;
		}
		അല്ലെങ്കിൽ
#endif
		    t_puts(&t_col, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    എങ്കിൽ (സന്ദേശം_no_more && വരികൾ_ഇടത് == 0)
		നിർത്തുക;

#ifdef HAS_MESSAGE_WINDOW
	    എങ്കിൽ (സന്ദേശം_win == ശൂന്യ)
#endif
		// Scroll the screen up one line.
		സന്ദേശം_scroll_up();

#ifdef HAS_MESSAGE_WINDOW
	    എങ്കിൽ (സന്ദേശം_win == ശൂന്യ)
	    {
#endif
		സന്ദേശം_row = Rows - 2;
		എങ്കിൽ (സന്ദേശം_col >= cmdline_width)  // can happen after screen resize
		    സന്ദേശം_col = cmdline_width - 1;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif

	    // Display char in last column before showing more-prompt.
	    എങ്കിൽ (*s >= ' '
#ifdef FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#endif
	       )
	    {
		എങ്കിൽ (has_mbyte)
		{
		    എങ്കിൽ (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (പൂർണ്ണസംഖ്യ)((str + maxlen) - s));
		    അല്ലെങ്കിൽ
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		അല്ലെങ്കിൽ
		    സന്ദേശം_screen_putchar(*s++, attr);
		did_last_char = TRUE;
	    }
	    അല്ലെങ്കിൽ
		did_last_char = FALSE;

	    എങ്കിൽ (p_more)
		// store text for scrolling back
		store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

#ifdef HAS_MESSAGE_WINDOW
	    എങ്കിൽ (സന്ദേശം_win == ശൂന്യ)
	    {
#endif
		inc_സന്ദേശം_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		എങ്കിൽ (cmdline_row > 0 && !exmode_active)
		    --cmdline_row;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		എങ്കിൽ (വരികൾ_ഇടത് > 0)
		    --വരികൾ_ഇടത്;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif
	    എങ്കിൽ (p_more && വരികൾ_ഇടത് == 0 && State != MODE_HITRETURN
					    && !സന്ദേശം_no_more && !exmode_active)
	    {
#ifdef FEAT_CON_DIALOG
		എങ്കിൽ (do_more_prompt(NUL))
		    s = confirm_സന്ദേശം_tail;
#അല്ലെങ്കിൽ
		(ശൂന്യം)do_more_prompt(NUL);
#endif
		എങ്കിൽ (quit_more)
		    തിരികെ;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    എങ്കിൽ (did_last_char)
		തുടരുക;
	}

	wrap = *s == '\n'
		    || സന്ദേശം_col + t_col >= wrap_col
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && സന്ദേശം_col + t_col >= wrap_col - 1);
	എങ്കിൽ (t_col > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ifdef HAS_MESSAGE_WINDOW
	    എങ്കിൽ (സന്ദേശം_win != ശൂന്യ)
	    {
		put_സന്ദേശം_win(സന്ദേശം_win, where, t_s, s, lnum);
		എങ്കിൽ (where == PUT_BELOW)
		    ++lnum;
		t_col = 0;
		where = PUT_BELOW;
		// Reset msg_col after outputting to new line in echowindow
		എങ്കിൽ (wrap)
		    സന്ദേശം_col = 0;
	    }
	    അല്ലെങ്കിൽ
#endif
		t_puts(&t_col, t_s, s, attr);
	}

	എങ്കിൽ (wrap && p_more && !recurse)
	    // store text for scrolling back
	    store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	എങ്കിൽ (*s == '\n')		    // go to next line
	{
#ifdef HAS_MESSAGE_WINDOW
	    എങ്കിൽ (സന്ദേശം_win != ശൂന്യ)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		എങ്കിൽ ((സന്ദേശം_win->w_buffer->b_ml.ml_flags & ML_EMPTY) == 0)
		{
		    put_സന്ദേശം_win(സന്ദേശം_win, PUT_BELOW, t_s, t_s, lnum);
		    ++lnum;
		}
		സന്ദേശം_col = 0;  // Reset column for new line
	    }
	    അല്ലെങ്കിൽ
#endif
		സന്ദേശം_didout = FALSE;	    // remember that line is empty
#ifdef FEAT_RIGHTLEFT
	    എങ്കിൽ (cmdmsg_rl)
		സന്ദേശം_col = cmdline_width - 1;
	    അല്ലെങ്കിൽ
#endif
		സന്ദേശം_col = 0;
	    എങ്കിൽ (++സന്ദേശം_row >= Rows)  // safety check
		സന്ദേശം_row = Rows - 1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*s == '\r')	    // go to column 0
	{
	    സന്ദേശം_col = 0;
#ifdef HAS_MESSAGE_WINDOW
	    where = PUT_TRUNC;
#endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*s == '\b')	    // go to previous char
	{
	    എങ്കിൽ (സന്ദേശം_col)
		--സന്ദേശം_col;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*s == TAB)	    // translate Tab into spaces
	{
#ifdef HAS_MESSAGE_WINDOW
	    എങ്കിൽ (സന്ദേശം_win != ശൂന്യ)
		സന്ദേശം_col = (സന്ദേശം_col + 7) % 8;
	    അല്ലെങ്കിൽ
#endif
		ചെയ്യുക
		    സന്ദേശം_screen_putchar(' ', attr);
		ആകുമ്പോൾ (സന്ദേശം_col & 7);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		എങ്കിൽ (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (പൂർണ്ണസംഖ്യ)((str + maxlen) - s));
		അല്ലെങ്കിൽ
		    l = (*mb_ptr2len)(s);
	    }
	    അല്ലെങ്കിൽ
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    എങ്കിൽ (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#endif
		    (cw > 1 && സന്ദേശം_col + t_col >= wrap_col))
	    {
		എങ്കിൽ (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		അല്ലെങ്കിൽ
		    സന്ദേശം_screen_putchar(*s, attr);
	    }
	    അല്ലെങ്കിൽ
	    {
		// postpone this character until later
		എങ്കിൽ (t_col == 0)
		    t_s = s;
		t_col += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    എങ്കിൽ (t_col > 0)
    {
#ifdef HAS_MESSAGE_WINDOW
	എങ്കിൽ (സന്ദേശം_win != ശൂന്യ)
	    put_സന്ദേശം_win(സന്ദേശം_win, where, t_s, s, lnum);
	അല്ലെങ്കിൽ
#endif
	    t_puts(&t_col, t_s, s, attr);
    }

#ifdef HAS_MESSAGE_WINDOW
    എങ്കിൽ (സന്ദേശം_win != ശൂന്യ)
	popup_show_message_win();
#endif
    // Store the text for scroll back.
    എങ്കിൽ (p_more && !recurse)
	store_sb_text(&sb_str, s, attr, &sb_col, FALSE);

    സന്ദേശം_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    പൂർണ്ണസംഖ്യ
message_filtered(char_u *സന്ദേശം)
{
    പൂർണ്ണസംഖ്യ match;

    എങ്കിൽ (cmdmod.cmod_filter_regmatch.regprog == ശൂന്യ)
	തിരികെ FALSE;
    match = vim_regexec(&cmdmod.cmod_filter_regmatch, സന്ദേശം, (colnr_T)0);
    തിരികെ cmdmod.cmod_filter_force ? match : !match;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    സ്ഥിതം ശൂന്യം
സന്ദേശം_scroll_up(ശൂന്യം)
{
#ifdef HAS_MESSAGE_WINDOW
    എങ്കിൽ (in_echowindow)
	തിരികെ;
#endif
#ifdef FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    എങ്കിൽ (gui.in_use)
	gui_undraw_cursor();
#endif
    // scrolling up always works
    mch_disable_flush();
    screen_del_വരികൾ(0, 0, 1, (പൂർണ്ണസംഖ്യ)Rows, TRUE, 0, ശൂന്യ);
    mch_enable_flush();

    എങ്കിൽ (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((പൂർണ്ണസംഖ്യ)Rows - 1, (പൂർണ്ണസംഖ്യ)Rows, cmdline_col_off,
		cmdline_col_off + cmdline_width, ' ', ' ', HL_ATTR(HLF_സന്ദേശം));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	എങ്കിൽ (ScreenAttrs[വരി_Offset[Rows - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((പൂർണ്ണസംഖ്യ)Rows - 2, (പൂർണ്ണസംഖ്യ)Rows - 1,
				 (പൂർണ്ണസംഖ്യ)Columns - 1, (പൂർണ്ണസംഖ്യ)Columns,
				 ' ', ' ', HL_ATTR(HLF_സന്ദേശം));
    }
}

/*
 * Increment "msg_scrolled".
 */
    സ്ഥിതം ശൂന്യം
inc_സന്ദേശം_scrolled(ശൂന്യം)
{
#ifdef FEAT_EVAL
    എങ്കിൽ (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = ശൂന്യ;

	// v:scrollstart is empty, set it to the script/function name and line
	//സംഖ്യ
	എങ്കിൽ (p.string == ശൂന്യ)
	{
	    p.string = (char_u *)_("Unknown");
	    p.length = STRLEN(p.string);
	}
	അല്ലെങ്കിൽ
	{
	    size_t  tofreesize;

	    tofreesize = (പൂർണ്ണസംഖ്യ)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    എങ്കിൽ (tofree != ശൂന്യ)
	    {
		p.length = vim_snprintf_safelen((അക്ഷരം *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (long)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	set_vim_var_string(VV_SCROLLSTART, p.string, (പൂർണ്ണസംഖ്യ)p.length);
	vim_free(tofree);
    }
#endif
    ++സന്ദേശം_scrolled;
    set_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
തരനിർവചനം ഘടന msgchunk_S msgchunk_T;
ഘടന msgchunk_S
{
    msgchunk_T	*sb_next;
    msgchunk_T	*sb_prev;
    അക്ഷരം	sb_eol;		// TRUE when line ends after this text
    പൂർണ്ണസംഖ്യ		sb_സന്ദേശം_col;	// column in which text starts
    പൂർണ്ണസംഖ്യ		sb_attr;	// text attributes
    char_u	sb_text[1];	// text to be displayed, actually longer
};

സ്ഥിതം msgchunk_T *last_msgchunk = ശൂന്യ; // last displayed text

സ്ഥിതം msgchunk_T *സന്ദേശം_sb_start(msgchunk_T *mps);

തരനിർവചനം എണ്ണൽ {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_DONE
} sb_clear_T;

// When to clear text on next msg.
സ്ഥിതം sb_clear_T do_clear_sb_text = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    സ്ഥിതം ശൂന്യം
store_sb_text(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    പൂർണ്ണസംഖ്യ		attr,
    പൂർണ്ണസംഖ്യ		*sb_col,
    പൂർണ്ണസംഖ്യ		finish)		// line ends
{
    msgchunk_T	*mp;

    എങ്കിൽ (do_clear_sb_text == SB_CLEAR_ALL
	    || do_clear_sb_text == SB_CLEAR_CMDLINE_DONE)
    {
	clear_sb_text(do_clear_sb_text == SB_CLEAR_ALL);
	സന്ദേശം_sb_eol();  // prevent messages from overlapping
	do_clear_sb_text = SB_CLEAR_NONE;
    }

    എങ്കിൽ (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_text) + (s - *sb_str) + 1);
	എങ്കിൽ (mp != ശൂന്യ)
	{
	    mp->sb_eol = finish;
	    mp->sb_സന്ദേശം_col = *sb_col;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_text, *sb_str, s - *sb_str);

	    എങ്കിൽ (last_msgchunk == ശൂന്യ)
	    {
		last_msgchunk = mp;
		mp->sb_prev = ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ
	    {
		mp->sb_prev = last_msgchunk;
		last_msgchunk->sb_next = mp;
		last_msgchunk = mp;
	    }
	    mp->sb_next = ശൂന്യ;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (finish && last_msgchunk != ശൂന്യ)
	last_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_col = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    ശൂന്യം
may_clear_sb_text(ശൂന്യം)
{
    do_clear_sb_text = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    ശൂന്യം
sb_text_start_cmdline(ശൂന്യം)
{
    എങ്കിൽ (do_clear_sb_text == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_text_restart_cmdline();
    അല്ലെങ്കിൽ
    {
	സന്ദേശം_sb_eol();
	do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    ശൂന്യം
sb_text_restart_cmdline(ശൂന്യം)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;

    എങ്കിൽ (last_msgchunk == ശൂന്യ || last_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	തിരികെ;

    tofree = സന്ദേശം_sb_start(last_msgchunk);
    last_msgchunk = tofree->sb_prev;
    എങ്കിൽ (last_msgchunk != ശൂന്യ)
	last_msgchunk->sb_next = ശൂന്യ;
    ആകുമ്പോൾ (tofree != ശൂന്യ)
    {
	msgchunk_T *tofree_next = tofree->sb_next;

	vim_free(tofree);
	tofree = tofree_next;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    ശൂന്യം
sb_text_end_cmdline(ശൂന്യം)
{
    do_clear_sb_text = SB_CLEAR_CMDLINE_DONE;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    ശൂന്യം
clear_sb_text(പൂർണ്ണസംഖ്യ all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    എങ്കിൽ (all)
	lastp = &last_msgchunk;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (last_msgchunk == ശൂന്യ)
	    തിരികെ;
	lastp = &സന്ദേശം_sb_start(last_msgchunk)->sb_prev;
    }

    ആകുമ്പോൾ (*lastp != ശൂന്യ)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    ശൂന്യം
show_sb_text(ശൂന്യം)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = സന്ദേശം_sb_start(last_msgchunk);
    എങ്കിൽ (mp == ശൂന്യ || mp->sb_prev == ശൂന്യ)
	vim_beep(BO_MESS);
    അല്ലെങ്കിൽ
    {
	do_more_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    സ്ഥിതം msgchunk_T *
സന്ദേശം_sb_start(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    ആകുമ്പോൾ (mp != ശൂന്യ && mp->sb_prev != ശൂന്യ && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    തിരികെ mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    ശൂന്യം
സന്ദേശം_sb_eol(ശൂന്യം)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    എങ്കിൽ (last_msgchunk != ശൂന്യ
	    && !(last_msgchunk->sb_text[0] == '\n'
		&& last_msgchunk->sb_text[1] == NUL))
	last_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    സ്ഥിതം msgchunk_T *
disp_sb_വരി(പൂർണ്ണസംഖ്യ row, msgchunk_T *smp, പൂർണ്ണസംഖ്യ clear_to_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    വേണ്ടി (;;)
    {
	സന്ദേശം_row = row;
	സന്ദേശം_col = mp->sb_സന്ദേശം_col;
	p = mp->sb_text;
	എങ്കിൽ (*p == '\n')	    // don't display the line break
	    ++p;
	സന്ദേശം_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	എങ്കിൽ (clear_to_eol)
	    screen_fill(row, row + 1,
		    cmdline_col_off + സന്ദേശം_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', HL_ATTR(HLF_സന്ദേശം));

	എങ്കിൽ (mp->sb_eol || mp->sb_next == ശൂന്യ)
	    നിർത്തുക;
	mp = mp->sb_next;
    }
    തിരികെ mp->sb_next;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    സ്ഥിതം ശൂന്യം
t_puts(
    പൂർണ്ണസംഖ്യ		*t_col,
    char_u	*t_s,
    char_u	*s,
    പൂർണ്ണസംഖ്യ		attr)
{
    // output postponed text
    സന്ദേശം_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (പൂർണ്ണസംഖ്യ)(s - t_s), സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col,
	    attr);
    സന്ദേശം_col += *t_col;
    *t_col = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    എങ്കിൽ (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--സന്ദേശം_col;
    എങ്കിൽ (സന്ദേശം_col >= cmdline_width)
    {
	സന്ദേശം_col = 0;
	++സന്ദേശം_row;
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
    പൂർണ്ണസംഖ്യ
സന്ദേശം_use_printf(ശൂന്യം)
{
    തിരികെ (!സന്ദേശം_check_screen()
#എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.in_use && !termcap_active)
# അല്ലെങ്കിൽ
	    || !termcap_active
# endif
#endif
	    || (swapping_screen() && !termcap_active)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    സ്ഥിതം ശൂന്യം
സന്ദേശം_puts_printf(char_u *str, പൂർണ്ണസംഖ്യ maxlen)
{
    char_u	*s = str;
    char_u	*buf = ശൂന്യ;
    char_u	*p = s;

#ifdef MSWIN
    എങ്കിൽ (!(silent_mode && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#endif
    ആകുമ്പോൾ ((maxlen < 0 || (പൂർണ്ണസംഖ്യ)(s - str) < maxlen) && *s != NUL)
    {
	എങ്കിൽ (!(silent_mode && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    എങ്കിൽ (*s == NL)
	    {
		പൂർണ്ണസംഖ്യ n = (പൂർണ്ണസംഖ്യ)(s - p);

		buf = alloc(n + 3);
		എങ്കിൽ (buf != ശൂന്യ)
		{
		    memcpy(buf, p, n);
		    എങ്കിൽ (!info_message)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    എങ്കിൽ (info_message)   // informative message, not an error
			mch_സന്ദേശം((അക്ഷരം *)buf);
		    അല്ലെങ്കിൽ
			mch_errmsg((അക്ഷരം *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (cmdmsg_rl)
	{
	    എങ്കിൽ (*s == CAR || *s == NL)
		സന്ദേശം_col = cmdline_width - 1;
	    അല്ലെങ്കിൽ
		--സന്ദേശം_col;
	}
	അല്ലെങ്കിൽ
#endif
	{
	    എങ്കിൽ (*s == CAR || *s == NL)
		സന്ദേശം_col = 0;
	    അല്ലെങ്കിൽ
		++സന്ദേശം_col;
	}
	++s;
    }

    എങ്കിൽ (*p != NUL && !(silent_mode && p_verbose == 0))
    {
	char_u *tofree = ശൂന്യ;

	എങ്കിൽ (maxlen > 0 && vim_strlen_maxlen((അക്ഷരം *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	എങ്കിൽ (p != ശൂന്യ)
	{
	    എങ്കിൽ (info_message)
		mch_സന്ദേശം((അക്ഷരം *)p);
	    അല്ലെങ്കിൽ
		mch_errmsg((അക്ഷരം *)p);
	    vim_free(tofree);
	}
    }

    സന്ദേശം_didout = TRUE;	    // assume that line is not empty

#ifdef MSWIN
    എങ്കിൽ (!(silent_mode && p_verbose == 0))
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_more_prompt(പൂർണ്ണസംഖ്യ typed_char)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	entered = FALSE;
    പൂർണ്ണസംഖ്യ		used_typed_char = typed_char;
    പൂർണ്ണസംഖ്യ		oldState = State;
    പൂർണ്ണസംഖ്യ		c;
#ifdef FEAT_CON_DIALOG
    പൂർണ്ണസംഖ്യ		retval = FALSE;
#endif
    പൂർണ്ണസംഖ്യ		toscroll;
    msgchunk_T	*mp_last = ശൂന്യ;
    msgchunk_T	*mp;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		സന്ദേശം_attr;

    സന്ദേശം_attr = HL_ATTR(HLF_സന്ദേശം);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    എങ്കിൽ (entered || (State == MODE_HITRETURN && typed_char == 0))
	തിരികെ FALSE;
    entered = TRUE;

    എങ്കിൽ (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_last = സന്ദേശം_sb_start(last_msgchunk);
	വേണ്ടി (i = 0; i < Rows - 2 && mp_last != ശൂന്യ
					     && mp_last->sb_prev != ശൂന്യ; ++i)
	    mp_last = സന്ദേശം_sb_start(mp_last->sb_prev);
    }

    State = MODE_ASKMORE;
    setmouse();
    എങ്കിൽ (typed_char == NUL)
	സന്ദേശം_moremsg(FALSE);
    വേണ്ടി (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	എങ്കിൽ (used_typed_char != NUL)
	{
	    c = used_typed_char;	// was typed at hit-enter prompt
	    used_typed_char = NUL;
	}
	അല്ലെങ്കിൽ
	    c = get_keystroke();

#എങ്കിൽ defined(FEAT_MENU) && defined(FEAT_GUI)
	എങ്കിൽ (c == K_MENU)
	{
	    പൂർണ്ണസംഖ്യ idx = get_menu_index(current_menu, MODE_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    എങ്കിൽ (idx == MENU_INDEX_INVALID)
		തുടരുക;
	    c = *current_menu->strings[idx];
	    എങ്കിൽ (c != NUL && current_menu->strings[idx][1] != NUL)
		ins_typebuf(current_menu->strings[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#endif

	toscroll = 0;
	തിരഞ്ഞെടുപ്പ് (c)
	{
	അവസ്ഥ BS:		// scroll one line back
	അവസ്ഥ K_BS:
	അവസ്ഥ 'k':
	അവസ്ഥ K_UP:
	    toscroll = -1;
	    നിർത്തുക;

	അവസ്ഥ CAR:		// one extra line
	അവസ്ഥ NL:
	അവസ്ഥ 'j':
	അവസ്ഥ K_DOWN:
	    toscroll = 1;
	    നിർത്തുക;

	അവസ്ഥ 'u':		// Up half a page
	    toscroll = -(Rows / 2);
	    നിർത്തുക;

	അവസ്ഥ 'd':		// Down half a page
	    toscroll = Rows / 2;
	    നിർത്തുക;

	അവസ്ഥ 'b':		// one page back
	അവസ്ഥ Ctrl_B:
	അവസ്ഥ K_PAGEUP:
	    toscroll = -(Rows - 1);
	    നിർത്തുക;

	അവസ്ഥ ' ':		// one extra page
	അവസ്ഥ 'f':
	അവസ്ഥ Ctrl_F:
	അവസ്ഥ K_PAGEDOWN:
	അവസ്ഥ K_LEFTMOUSE:
	    toscroll = Rows - 1;
	    നിർത്തുക;

	അവസ്ഥ 'g':		// all the way back to the start
	    toscroll = -999999;
	    നിർത്തുക;

	അവസ്ഥ 'G':		// all the way to the end
	    toscroll = 999999;
	    വരികൾ_ഇടത് = 999999;
	    നിർത്തുക;

	അവസ്ഥ ':':		// start new command line
#ifdef FEAT_CON_DIALOG
	    എങ്കിൽ (!confirm_സന്ദേശം_used)
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
	അവസ്ഥ 'q':		// quit
	അവസ്ഥ Ctrl_C:
	അവസ്ഥ ESC:
#ifdef FEAT_CON_DIALOG
	    എങ്കിൽ (confirm_സന്ദേശം_used)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    അല്ലെങ്കിൽ
#endif
	    {
		got_int = TRUE;
		quit_more = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    വരികൾ_ഇടത് = Rows - 1;
	    നിർത്തുക;

#ifdef FEAT_CLIPBOARD
	അവസ്ഥ Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    എങ്കിൽ (clip_star.state == SELECT_DONE)
		clip_copy_modeless_selection(TRUE);
	    തുടരുക;
#endif
	സ്ഥിരസ്ഥിതി:		// no valid response
	    സന്ദേശം_moremsg(TRUE);
	    തുടരുക;
	}

	എങ്കിൽ (toscroll != 0)
	{
	    എങ്കിൽ (toscroll < 0)
	    {
		// go to start of last line
		എങ്കിൽ (mp_last == ശൂന്യ)
		    mp = സന്ദേശം_sb_start(last_msgchunk);
		അല്ലെങ്കിൽ എങ്കിൽ (mp_last->sb_prev != ശൂന്യ)
		    mp = സന്ദേശം_sb_start(mp_last->sb_prev);
		അല്ലെങ്കിൽ
		    mp = ശൂന്യ;

		// go to start of line at top of the screen
		വേണ്ടി (i = 0; i < Rows - 2 && mp != ശൂന്യ && mp->sb_prev != ശൂന്യ;
									  ++i)
		    mp = സന്ദേശം_sb_start(mp->sb_prev);

		എങ്കിൽ (mp != ശൂന്യ && mp->sb_prev != ശൂന്യ)
		{
		    // Find line to be displayed at top.
		    വേണ്ടി (i = 0; i > toscroll; --i)
		    {
			എങ്കിൽ (mp == ശൂന്യ || mp->sb_prev == ശൂന്യ)
			    നിർത്തുക;
			mp = സന്ദേശം_sb_start(mp->sb_prev);
			എങ്കിൽ (mp_last == ശൂന്യ)
			    mp_last = സന്ദേശം_sb_start(last_msgchunk);
			അല്ലെങ്കിൽ
			    mp_last = സന്ദേശം_sb_start(mp_last->sb_prev);
		    }

		    എങ്കിൽ (toscroll == -1 && screen_ins_വരികൾ(0, 0, 1,
						     (പൂർണ്ണസംഖ്യ)Rows, 0, ശൂന്യ) == OK)
		    {
			// display line at top
			(ശൂന്യം)disp_sb_വരി(0, mp, FALSE);
		    }
		    അല്ലെങ്കിൽ
		    {
			പൂർണ്ണസംഖ്യ did_clear = screenclear();

			// redisplay all lines
			വേണ്ടി (i = 0; mp != ശൂന്യ && i < Rows - 1; ++i)
			{
			    mp = disp_sb_വരി(i, mp, !did_clear);
			    ++സന്ദേശം_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		// First display any text that we scrolled back.
		ആകുമ്പോൾ (toscroll > 0 && mp_last != ശൂന്യ)
		{
		    // scroll up, display line at bottom
		    സന്ദേശം_scroll_up();
		    inc_സന്ദേശം_scrolled();
		    screen_fill((പൂർണ്ണസംഖ്യ)Rows - 2, (പൂർണ്ണസംഖ്യ)Rows - 1,
			    cmdline_col_off, cmdline_col_off + cmdline_width,
			    ' ', ' ', സന്ദേശം_attr);
		    mp_last = disp_sb_വരി((പൂർണ്ണസംഖ്യ)Rows - 2, mp_last, FALSE);
		    --toscroll;
		}
	    }

	    എങ്കിൽ (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((പൂർണ്ണസംഖ്യ)Rows - 1, (പൂർണ്ണസംഖ്യ)Rows, cmdline_col_off,
			cmdline_col_off + cmdline_width, ' ', ' ', സന്ദേശം_attr);
		സന്ദേശം_moremsg(FALSE);
		തുടരുക;
	    }

	    // display more text, return to caller
	    വരികൾ_ഇടത് = toscroll;
	}

	നിർത്തുക;
    }

    // clear the --more-- message
    screen_fill((പൂർണ്ണസംഖ്യ)Rows - 1, (പൂർണ്ണസംഖ്യ)Rows, cmdline_col_off,
	    cmdline_col_off + cmdline_width, ' ', ' ', സന്ദേശം_attr);
    State = oldState;
    setmouse();
    എങ്കിൽ (quit_more)
    {
	സന്ദേശം_row = Rows - 1;
	സന്ദേശം_col = 0;
    }
#ifdef FEAT_RIGHTLEFT
    അല്ലെങ്കിൽ എങ്കിൽ (cmdmsg_rl)
	സന്ദേശം_col = cmdline_width - 1;
#endif

    entered = FALSE;
#ifdef FEAT_CON_DIALOG
    തിരികെ retval;
#അല്ലെങ്കിൽ
    തിരികെ FALSE;
#endif
}

#എങ്കിൽ defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_സന്ദേശം
#  undef mch_സന്ദേശം
# endif

# എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    സ്ഥിതം ശൂന്യം
mch_errmsg_c(അക്ഷരം *str)
{
    DWORD   nwrite = 0;
    DWORD   mode = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    എങ്കിൽ (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (പൂർണ്ണസംഖ്യ)GetConsoleCP() != enc_codepage)
    {
	പൂർണ്ണസംഖ്യ	len = (പൂർണ്ണസംഖ്യ)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	എങ്കിൽ (w != ശൂന്യ)
	{
	    WriteConsoleW(h, w, len, &nwrite, ശൂന്യ);
	    vim_free(w);
	}
    }
    അല്ലെങ്കിൽ
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
    ശൂന്യം
mch_errmsg(അക്ഷരം *str)
{
# എങ്കിൽ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    പൂർണ്ണസംഖ്യ		len;
# endif

# എങ്കിൽ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    എങ്കിൽ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   അല്ലെങ്കിൽ
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
	തിരികെ;
    }
# endif

# എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    എങ്കിൽ (!(gui.in_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	തിരികെ;
    }
# endif

# എങ്കിൽ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (പൂർണ്ണസംഖ്യ)STRLEN(str) + 1;
    എങ്കിൽ (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    എങ്കിൽ (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_data + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_data + error_ga.ga_len;
	    വേണ്ടി (;;)
	    {
		p = vim_strchr(p, '\r');
		എങ്കിൽ (p == ശൂന്യ)
		    നിർത്തുക;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    സ്ഥിതം ശൂന്യം
mch_സന്ദേശം_c(അക്ഷരം *str)
{
    DWORD   nwrite = 0;
    DWORD   mode;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    എങ്കിൽ (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (പൂർണ്ണസംഖ്യ)GetConsoleCP() != enc_codepage)
    {
	പൂർണ്ണസംഖ്യ	len = (പൂർണ്ണസംഖ്യ)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	എങ്കിൽ (w != ശൂന്യ)
	{
	    WriteConsoleW(h, w, len, &nwrite, ശൂന്യ);
	    vim_free(w);
	}
    }
    അല്ലെങ്കിൽ
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
    ശൂന്യം
mch_സന്ദേശം(അക്ഷരം *str)
{
# എങ്കിൽ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    എങ്കിൽ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   അല്ലെങ്കിൽ
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
	തിരികെ;
    }
# endif

# എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    എങ്കിൽ (!(gui.in_use || gui.starting))
#  endif
    {
	mch_സന്ദേശം_c(str);
	തിരികെ;
    }
# endif
# എങ്കിൽ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#endif // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    സ്ഥിതം ശൂന്യം
സന്ദേശം_screen_putchar(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ attr)
{
    സന്ദേശം_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col, attr);
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (cmdmsg_rl)
    {
	എങ്കിൽ (--സന്ദേശം_col == 0)
	{
	    സന്ദേശം_col = cmdline_width;
	    ++സന്ദേശം_row;
	}
    }
    അല്ലെങ്കിൽ
#endif
    {
	എങ്കിൽ (++സന്ദേശം_col >= cmdline_width)
	{
	    സന്ദേശം_col = 0;
	    ++സന്ദേശം_row;
	}
    }
}

    സ്ഥിതം ശൂന്യം
സന്ദേശം_moremsg(പൂർണ്ണസംഖ്യ full)
{
    പൂർണ്ണസംഖ്യ		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (പൂർണ്ണസംഖ്യ)Rows - 1, cmdline_col_off, attr);
    എങ്കിൽ (full)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(പൂർണ്ണസംഖ്യ)Rows - 1, cmdline_col_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    ശൂന്യം
repeat_message(ശൂന്യം)
{
    എങ്കിൽ (State == MODE_ASKMORE)
    {
	സന്ദേശം_moremsg(TRUE);	// display --more-- message again
	സന്ദേശം_row = Rows - 1;
    }
#ifdef FEAT_CON_DIALOG
    അല്ലെങ്കിൽ എങ്കിൽ (State == MODE_CONFIRM)
    {
	display_confirm_സന്ദേശം();	// display ":confirm" message again
	സന്ദേശം_row = Rows - 1;
    }
#endif
    അല്ലെങ്കിൽ എങ്കിൽ (State == MODE_EXTERNCMD)
    {
	windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col); // put cursor back
    }
    അല്ലെങ്കിൽ എങ്കിൽ (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	എങ്കിൽ (സന്ദേശം_row == Rows - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    സന്ദേശം_didout = FALSE;
	    സന്ദേശം_col = 0;
	    സന്ദേശം_clr_eos();
	}
	hit_return_സന്ദേശം();
	സന്ദേശം_row = Rows - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
സന്ദേശം_check_screen(ശൂന്യം)
{
    എങ്കിൽ (!full_screen || !screen_valid(FALSE))
	തിരികെ FALSE;

    എങ്കിൽ (സന്ദേശം_row >= Rows)
	സന്ദേശം_row = Rows - 1;
    എങ്കിൽ (സന്ദേശം_col >= cmdline_width)
	സന്ദേശം_col = cmdline_width - 1;
    തിരികെ TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    ശൂന്യം
സന്ദേശം_clr_eos(ശൂന്യം)
{
    എങ്കിൽ (സന്ദേശം_silent == 0)
	സന്ദേശം_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    ശൂന്യം
സന്ദേശം_clr_eos_force(ശൂന്യം)
{
#ifdef HAS_MESSAGE_WINDOW
    എങ്കിൽ (in_echowindow)
	തിരികെ;  // messages go into a popup
#endif
    എങ്കിൽ (സന്ദേശം_use_printf())
    {
	എങ്കിൽ (full_screen)	// only when termcap codes are valid
	{
	    എങ്കിൽ (*T_CD)
		out_str(T_CD);	// clear to end of display
	    അല്ലെങ്കിൽ എങ്കിൽ (*T_CE)
		out_str(T_CE);	// clear to end of line
	}
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ സന്ദേശം_attr;

	സന്ദേശം_attr = HL_ATTR(HLF_സന്ദേശം);

#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (cmdmsg_rl)
	{
	    screen_fill(സന്ദേശം_row, സന്ദേശം_row + 1,
		    cmdline_col_off, cmdline_col_off + സന്ദേശം_col + 1,
		    ' ', ' ', സന്ദേശം_attr);
	    screen_fill(സന്ദേശം_row + 1, (പൂർണ്ണസംഖ്യ)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', സന്ദേശം_attr);
	}
	അല്ലെങ്കിൽ
#endif
	{
	    screen_fill(സന്ദേശം_row, സന്ദേശം_row + 1,
		    cmdline_col_off + സന്ദേശം_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', സന്ദേശം_attr);
	    screen_fill(സന്ദേശം_row + 1, (പൂർണ്ണസംഖ്യ)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', സന്ദേശം_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    ശൂന്യം
സന്ദേശം_clr_cmdline(ശൂന്യം)
{
    സന്ദേശം_row = cmdline_row;
    സന്ദേശം_col = 0;
    സന്ദേശം_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    പൂർണ്ണസംഖ്യ
സന്ദേശം_end(ശൂന്യം)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    എങ്കിൽ (!exiting && need_wait_return && !(State & MODE_CMDLINE))
    {
	wait_return(FALSE);
	തിരികെ FALSE;
    }
    out_flush();
    തിരികെ TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    ശൂന്യം
സന്ദേശം_check(ശൂന്യം)
{
    എങ്കിൽ (സന്ദേശം_row == Rows - 1 && സന്ദേശം_col >= sc_col
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
    സ്ഥിതം ശൂന്യം
redir_write(char_u *str, പൂർണ്ണസംഖ്യ maxlen)
{
    char_u	*s = str;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	cur_col = 0;

    // Don't do anything for displaying prompts and the like.
    എങ്കിൽ (redir_off)
	തിരികെ;

    // If 'verbosefile' is set prepare for writing in that file.
    എങ്കിൽ (*p_vfile != NUL && verbose_fd == ശൂന്യ)
	verbose_open();

    എങ്കിൽ (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	എങ്കിൽ (*s != '\n' && *s != '\r')
	{
	    ആകുമ്പോൾ (cur_col < സന്ദേശം_col)
	    {
#ifdef FEAT_EVAL
		എങ്കിൽ (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		അല്ലെങ്കിൽ എങ്കിൽ (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		അല്ലെങ്കിൽ എങ്കിൽ (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		അല്ലെങ്കിൽ
#endif
		    എങ്കിൽ (redir_fd != ശൂന്യ)
		    fputs(" ", redir_fd);
		എങ്കിൽ (verbose_fd != ശൂന്യ)
		    fputs(" ", verbose_fd);
		++cur_col;
	    }
	}

#ifdef FEAT_EVAL
	എങ്കിൽ (redir_execute)
	    execute_redir_str(s, maxlen);
	അല്ലെങ്കിൽ എങ്കിൽ (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	അല്ലെങ്കിൽ എങ്കിൽ (redir_vname)
	    var_redir_str(s, maxlen);
#endif

	// Write and adjust the current column.
	ആകുമ്പോൾ (*s != NUL && (maxlen < 0 || (പൂർണ്ണസംഖ്യ)(s - str) < maxlen))
	{
#ifdef FEAT_EVAL
	    എങ്കിൽ (!redir_reg && !redir_vname && !redir_execute)
#endif
		എങ്കിൽ (redir_fd != ശൂന്യ)
		    putc(*s, redir_fd);
	    എങ്കിൽ (verbose_fd != ശൂന്യ)
		putc(*s, verbose_fd);
	    എങ്കിൽ (*s == '\r' || *s == '\n')
		cur_col = 0;
	    അല്ലെങ്കിൽ എങ്കിൽ (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    അല്ലെങ്കിൽ
		++cur_col;
	    ++s;
	}

	എങ്കിൽ (സന്ദേശം_silent != 0)	// should update msg_col
	    സന്ദേശം_col = cur_col;
    }
}

    പൂർണ്ണസംഖ്യ
redirecting(ശൂന്യം)
{
    തിരികെ redir_fd != ശൂന്യ || *p_vfile != NUL
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#endif
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    ശൂന്യം
verbose_enter(ശൂന്യം)
{
    എങ്കിൽ (*p_vfile != NUL)
	++സന്ദേശം_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    ശൂന്യം
verbose_leave(ശൂന്യം)
{
    എങ്കിൽ (*p_vfile != NUL)
	എങ്കിൽ (--സന്ദേശം_silent < 0)
	    സന്ദേശം_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    ശൂന്യം
verbose_enter_scroll(ശൂന്യം)
{
    എങ്കിൽ (*p_vfile != NUL)
	++സന്ദേശം_silent;
    അല്ലെങ്കിൽ
	// always scroll up, don't overwrite
	സന്ദേശം_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    ശൂന്യം
verbose_leave_scroll(ശൂന്യം)
{
    എങ്കിൽ (*p_vfile != NUL)
    {
	എങ്കിൽ (--സന്ദേശം_silent < 0)
	    സന്ദേശം_silent = 0;
    }
    അല്ലെങ്കിൽ
	cmdline_row = സന്ദേശം_row;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    ശൂന്യം
verbose_stop(ശൂന്യം)
{
    എങ്കിൽ (verbose_fd != ശൂന്യ)
    {
	fclose(verbose_fd);
	verbose_fd = ശൂന്യ;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    പൂർണ്ണസംഖ്യ
verbose_open(ശൂന്യം)
{
    എങ്കിൽ (verbose_fd == ശൂന്യ && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((അക്ഷരം *)p_vfile, "a");
	എങ്കിൽ (verbose_fd == ശൂന്യ)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    തിരികെ FAIL;
	}
    }
    തിരികെ OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    ശൂന്യം
give_warning(char_u *message, പൂർണ്ണസംഖ്യ hl)
{
    give_warning_with_source(message, hl, FALSE);
}

    ശൂന്യം
give_warning_with_source(char_u *message, പൂർണ്ണസംഖ്യ hl, പൂർണ്ണസംഖ്യ with_source)
{
    // Don't do this for ":silent".
    എങ്കിൽ (സന്ദേശം_silent != 0)
	തിരികെ;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, message, -1);
#endif
    VIM_CLEAR(keep_സന്ദേശം);
    എങ്കിൽ (hl)
	keep_സന്ദേശം_attr = HL_ATTR(HLF_W);
    അല്ലെങ്കിൽ
	keep_സന്ദേശം_attr = 0;

    എങ്കിൽ (with_source)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	സന്ദേശം_start();
	സന്ദേശം_source(HL_ATTR(HLF_W));
	സന്ദേശം_puts(" ");
	സന്ദേശം_puts_attr((അക്ഷരം *)message, HL_ATTR(HLF_W) | സന്ദേശം_HIST);
	സന്ദേശം_clr_eos();
	(ശൂന്യം)സന്ദേശം_end();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (സന്ദേശം_attr((അക്ഷരം *)message, keep_സന്ദേശം_attr) && സന്ദേശം_scrolled == 0)
	set_keep_സന്ദേശം(message, keep_സന്ദേശം_attr);

    സന്ദേശം_didout = FALSE;	    // overwrite this message
    സന്ദേശം_nowait = TRUE;	    // don't wait for this message
    സന്ദേശം_col = 0;

    --no_wait_return;
}

#എങ്കിൽ defined(FEAT_EVAL)
    ശൂന്യം
give_warning2(char_u *message, char_u *a1, പൂർണ്ണസംഖ്യ hl)
{
    എങ്കിൽ (IObuff == ശൂന്യ)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(message, hl);
    }
    അല്ലെങ്കിൽ
    {
	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, (അക്ഷരം *)message, a1);
	give_warning(IObuff, hl);
    }
}
#endif

/*
 * Advance msg cursor to column "col".
 */
    ശൂന്യം
സന്ദേശം_advance(പൂർണ്ണസംഖ്യ col)
{
    എങ്കിൽ (സന്ദേശം_silent != 0)	// nothing to advance to
    {
	സന്ദേശം_col = col;		// for redirection, may fill it up later
	തിരികെ;
    }
    എങ്കിൽ (col >= cmdline_width)	// not enough room
	col = cmdline_width - 1;
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (cmdmsg_rl)
	ആകുമ്പോൾ (സന്ദേശം_col > cmdline_width - col)
	    സന്ദേശം_putchar(' ');
    അല്ലെങ്കിൽ
#endif
	ആകുമ്പോൾ (സന്ദേശം_col < col)
	    സന്ദേശം_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    ശൂന്യം
സന്ദേശം_warn_missing_clipboard(ശൂന്യം)
{
    എങ്കിൽ (!global_busy && !did_warn_clipboard && silence_w23_w24_സന്ദേശം == 0)
    {
#ifdef FEAT_CLIPBOARD
	സന്ദേശം(_("W23: Clipboard register not available, using register 0"));
#അല്ലെങ്കിൽ
	സന്ദേശം(_("W24: Clipboard register not available. See :h W24"));
#endif
	did_warn_clipboard = TRUE;
    }
}

#എങ്കിൽ defined(FEAT_CON_DIALOG)
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
    പൂർണ്ണസംഖ്യ
do_dialog(
    പൂർണ്ണസംഖ്യ		type UNUSED,
    char_u	*title UNUSED,
    char_u	*message,
    char_u	*buttons,
    പൂർണ്ണസംഖ്യ		dfltbutton,
    char_u	*textfield UNUSED,	// IObuff for inputdialog(), NULL
					// otherwise
    പൂർണ്ണസംഖ്യ		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    പൂർണ്ണസംഖ്യ		oldState;
    പൂർണ്ണസംഖ്യ		retval = 0;
    char_u	*hotkeys;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    എങ്കിൽ (silent_mode)
	തിരികെ dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    എങ്കിൽ (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == ശൂന്യ)
    {
	// --gui-dialog-file: write text to a file
	എങ്കിൽ (gui_dialog_log(title, message))
	    c = dfltbutton;
	അല്ലെങ്കിൽ
	    c = gui_mch_dialog(type, title, message, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_row = സന്ദേശം_row;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	out_flush();
	gui_mch_update();

	തിരികെ c;
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
    hotkeys = സന്ദേശം_show_console_dialog(message, buttons, dfltbutton);

    എങ്കിൽ (hotkeys != ശൂന്യ)
    {
	വേണ്ടി (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    തിരഞ്ഞെടുപ്പ് (c)
	    {
	    അവസ്ഥ CAR:		// User accepts default option
	    അവസ്ഥ NL:
		retval = dfltbutton;
		നിർത്തുക;
	    അവസ്ഥ Ctrl_C:	// User aborts/cancels
	    അവസ്ഥ ESC:
		retval = 0;
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:		// Could be a hotkey?
		എങ്കിൽ (c < 0)	// special keys are ignored here
		    തുടരുക;
		എങ്കിൽ (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    നിർത്തുക;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		വേണ്ടി (i = 0; hotkeys[i]; ++i)
		{
		    എങ്കിൽ (has_mbyte)
		    {
			എങ്കിൽ ((*mb_ptr2char)(hotkeys + i) == c)
			    നിർത്തുക;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    അല്ലെങ്കിൽ
			എങ്കിൽ (hotkeys[i] == c)
			    നിർത്തുക;
		    ++retval;
		}
		എങ്കിൽ (hotkeys[i])
		    നിർത്തുക;
		// No hotkey match, so keep waiting
		തുടരുക;
	    }
	    നിർത്തുക;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    സന്ദേശം_end_prompt();

    തിരികെ retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
copy_char(
    char_u	*from,
    char_u	*to,
    പൂർണ്ണസംഖ്യ		lowercase)	// make character lower case
{
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (has_mbyte)
    {
	എങ്കിൽ (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    തിരികെ (*mb_char2bytes)(c, to);
	}
	അല്ലെങ്കിൽ
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(to, from, (size_t)len);
	    തിരികെ len;
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (lowercase)
	    *to = (char_u)TOLOWER_LOC(*from);
	അല്ലെങ്കിൽ
	    *to = *from;
	തിരികെ 1;
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
    സ്ഥിതം char_u *
സന്ദേശം_show_console_dialog(
    char_u	*message,
    char_u	*buttons,
    പൂർണ്ണസംഖ്യ		dfltbutton)
{
    പൂർണ്ണസംഖ്യ		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    പൂർണ്ണസംഖ്യ		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = ശൂന്യ;
    char_u	*msgp = ശൂന്യ;
    char_u	*hotkp = ശൂന്യ;
    char_u	*r;
    പൂർണ്ണസംഖ്യ		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    പൂർണ്ണസംഖ്യ		first_hotkey = FALSE;	// first char of button is hotkey
    പൂർണ്ണസംഖ്യ		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    വേണ്ടി (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	ആകുമ്പോൾ (*r)
	{
	    എങ്കിൽ (*r == DLG_BUTTON_SEP)
	    {
		എങ്കിൽ (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    എങ്കിൽ (has_mbyte)
			hotkp += STRLEN(hotkp);
		    അല്ലെങ്കിൽ
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    എങ്കിൽ (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    എങ്കിൽ (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			first_hotkey = TRUE;
		}
		അല്ലെങ്കിൽ
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    എങ്കിൽ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*r == DLG_HOTKEY_CHAR || first_hotkey)
	    {
		എങ്കിൽ (*r == DLG_HOTKEY_CHAR)
		    ++r;
		first_hotkey = FALSE;
		എങ്കിൽ (copy)
		{
		    എങ്കിൽ (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    അല്ലെങ്കിൽ
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    ++len;	    // '&a' -> '[a]'
		    എങ്കിൽ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		// everything else copy literally
		എങ്കിൽ (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	എങ്കിൽ (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	അല്ലെങ്കിൽ
	{
	    len += (പൂർണ്ണസംഖ്യ)(STRLEN(message)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    എങ്കിൽ (!has_hotkey[0])
	    {
		first_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_സന്ദേശം);
	    confirm_സന്ദേശം = alloc(len);
	    എങ്കിൽ (confirm_സന്ദേശം == ശൂന്യ)
		തിരികെ ശൂന്യ;
	    *confirm_സന്ദേശം = NUL;
	    hotk = alloc(lenhotkey);
	    എങ്കിൽ (hotk == ശൂന്യ)
		തിരികെ ശൂന്യ;

	    *confirm_സന്ദേശം = '\n';
	    STRCPY(confirm_സന്ദേശം + 1, message);

	    msgp = confirm_സന്ദേശം + 1 + STRLEN(message);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_സന്ദേശം_tail = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_സന്ദേശം();
    തിരികെ hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    സ്ഥിതം ശൂന്യം
display_confirm_സന്ദേശം(ശൂന്യം)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_സന്ദേശം_used;
    എങ്കിൽ (confirm_സന്ദേശം != ശൂന്യ)
	സന്ദേശം_puts_attr((അക്ഷരം *)confirm_സന്ദേശം, HL_ATTR(HLF_M));
    --confirm_സന്ദേശം_used;
}

#endif // FEAT_CON_DIALOG

#എങ്കിൽ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    പൂർണ്ണസംഖ്യ
vim_dialog_yesno(
    പൂർണ്ണസംഖ്യ		type,
    char_u	*title,
    char_u	*message,
    പൂർണ്ണസംഖ്യ		dflt)
{
    എങ്കിൽ (do_dialog(type,
		title == ശൂന്യ ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No"), dflt, ശൂന്യ, FALSE) == 1)
	തിരികെ VIM_YES;
    തിരികെ VIM_NO;
}

    പൂർണ്ണസംഖ്യ
vim_dialog_yesnocancel(
    പൂർണ്ണസംഖ്യ		type,
    char_u	*title,
    char_u	*message,
    പൂർണ്ണസംഖ്യ		dflt)
{
    തിരഞ്ഞെടുപ്പ് (do_dialog(type,
		title == ശൂന്യ ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, ശൂന്യ, FALSE))
    {
	അവസ്ഥ 1: തിരികെ VIM_YES;
	അവസ്ഥ 2: തിരികെ VIM_NO;
    }
    തിരികെ VIM_CANCEL;
}

    പൂർണ്ണസംഖ്യ
vim_dialog_yesnoallcancel(
    പൂർണ്ണസംഖ്യ		type,
    char_u	*title,
    char_u	*message,
    പൂർണ്ണസംഖ്യ		dflt)
{
    തിരഞ്ഞെടുപ്പ് (do_dialog(type,
		title == ശൂന്യ ? (char_u *)"Question" : title,
		message,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, ശൂന്യ, FALSE))
    {
	അവസ്ഥ 1: തിരികെ VIM_YES;
	അവസ്ഥ 2: തിരികെ VIM_NO;
	അവസ്ഥ 3: തിരികെ VIM_ALL;
	അവസ്ഥ 4: തിരികെ VIM_DISCARDALL;
    }
    തിരികെ VIM_CANCEL;
}

#endif // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
