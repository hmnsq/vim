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

#அந்தர்பாவ "vim.h"

நிலையான வெறுமை add_செய்தி_hist(char_u *s, முழு len, முழு attr);
நிலையான வெறுமை check_செய்தி_hist(வெறுமை);
நிலையான வெறுமை hit_return_செய்தி(வெறுமை);
நிலையான வெறுமை செய்தி_home_replace_attr(char_u *fname, முழு attr);
நிலையான வெறுமை செய்தி_puts_attr_len(எழுத்து *str, முழு maxlen, முழு attr);
நிலையான வெறுமை செய்தி_puts_display(char_u *str, முழு maxlen, முழு attr, முழு recurse);
நிலையான வெறுமை செய்தி_scroll_up(வெறுமை);
நிலையான வெறுமை inc_செய்தி_scrolled(வெறுமை);
நிலையான வெறுமை store_sb_text(char_u **sb_str, char_u *s, முழு attr, முழு *sb_col, முழு finish);
நிலையான வெறுமை t_puts(முழு *t_col, char_u *t_s, char_u *s, முழு attr);
நிலையான வெறுமை செய்தி_puts_printf(char_u *str, முழு maxlen);
நிலையான முழு do_more_prompt(முழு typed_char);
நிலையான வெறுமை செய்தி_screen_putchar(முழு c, முழு attr);
நிலையான வெறுமை செய்தி_moremsg(முழு full);
நிலையான முழு  செய்தி_check_screen(வெறுமை);
நிலையான வெறுமை redir_write(char_u *s, முழு maxlen);
#ifdef FEAT_CON_DIALOG
நிலையான char_u *செய்தி_show_console_dialog(char_u *message, char_u *buttons, முழு dfltbutton);
நிலையான முழு	confirm_செய்தி_used = FALSE;	// displaying confirm_msg
நிலையான char_u	*confirm_செய்தி = வெற்று;		// ":confirm" message
நிலையான char_u	*confirm_செய்தி_tail;		// tail of confirm_msg
நிலையான வெறுமை display_confirm_செய்தி(வெறுமை);
#endif
#ifdef FEAT_EVAL
நிலையான முழு emsg_to_channel_log = FALSE;
#endif

கட்டமைப்பு செய்தி_hist
{
    கட்டமைப்பு செய்தி_hist	*next;
    char_u		*செய்தி;
    முழு			attr;
};

நிலையான கட்டமைப்பு செய்தி_hist *first_செய்தி_hist = வெற்று;
நிலையான கட்டமைப்பு செய்தி_hist *last_செய்தி_hist = வெற்று;
நிலையான முழு செய்தி_hist_len = 0;
நிலையான முழு செய்தி_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#define MESSAGES_HIT_ENTER	0x001
#define MESSAGES_WAIT		0x002
#define MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#define MESSAGES_OPT_HIT_ENTER "hit-enter"
#define MESSAGES_OPT_WAIT "wait:"
#define MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
நிலையான முழு செய்தி_flags = MESSAGES_HIT_ENTER | MESSAGES_HISTORY;
நிலையான முழு செய்தி_wait = 0;

நிலையான FILE *verbose_fd = வெற்று;
நிலையான முழு  verbose_did_open = FALSE;

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
    முழு
செய்தி(எழுத்து *s)
{
    திரும்பு செய்தி_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    முழு
verb_செய்தி(எழுத்து *s)
{
    முழு		n;

    verbose_enter();
    n = செய்தி_attr_keep(s, 0, FALSE);
    verbose_leave();

    திரும்பு n;
}

    முழு
செய்தி_attr(எழுத்து *s, முழு attr)
{
    திரும்பு செய்தி_attr_keep(s, attr, FALSE);
}

    முழு
செய்தி_attr_keep(
    எழுத்து	*s,
    முழு		attr,
    முழு		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    நிலையான முழு	entered = 0;
    முழு		retval;
    char_u	*buf = வெற்று;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    எனில் (!emsg_on_display && message_filtered((char_u *)s))
	திரும்பு TRUE;

#ifdef FEAT_EVAL
    எனில் (attr == 0)
	set_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#endif

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    எனில் (entered >= 3)
	திரும்பு TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    எனில் ((char_u *)s != keep_செய்தி
	    || (*s != '<'
		&& last_செய்தி_hist != வெற்று
		&& last_செய்தி_hist->செய்தி != வெற்று
		&& STRCMP(s, last_செய்தி_hist->செய்தி) != 0))
	add_செய்தி_hist((char_u *)s, -1, attr);

#ifdef FEAT_EVAL
    எனில் (emsg_to_channel_log)
	// Write message in the channel log.
	ch_log(வெற்று, "ERROR: %s", s);
#endif

    // Truncate the message if needed.
    செய்தி_start();
    buf = செய்தி_strtrunc((char_u *)s, FALSE);
    எனில் (buf != வெற்று)
	s = (எழுத்து *)buf;

    செய்தி_outtrans_attr((char_u *)s, attr);
    செய்தி_clr_eos();
    retval = செய்தி_end();

    எனில் (keep && retval && vim_strsize((char_u *)s)
		    < (முழு)(Rows - cmdline_row - 1) * cmdline_width + sc_col)
	set_keep_செய்தி((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    திரும்பு retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
செய்தி_strtrunc(
    char_u	*s,
    முழு		force)	    // always truncate
{
    char_u	*buf = வெற்று;
    முழு		len;
    முழு		room;

    // May truncate message to avoid a hit-return prompt
    எனில் ((!செய்தி_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_active && செய்தி_silent == 0) || force)
    {
	len = vim_strsize(s);
	எனில் (செய்தி_scrolled != 0
#ifdef HAS_MESSAGE_WINDOW
		|| in_echowindow
#endif
		)
	    // Use all the columns.
	    room = (முழு)(Rows - செய்தி_row) * cmdline_width - 1;
	இல்லையெனில்
	    // Use up to 'showcmd' column.
	    room = (முழு)(Rows - செய்தி_row - 1) * cmdline_width + sc_col - 1;
	எனில் (len > room && room > 0)
	{
	    எனில் (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (room + 2) * 18;
	    இல்லையெனில் எனில் (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (room + 2) * 2;
	    இல்லையெனில்
		len = room + 2;
	    buf = alloc(len);
	    எனில் (buf != வெற்று)
		trunc_string(s, buf, room, len);
	}
    }
    திரும்பு buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    வெறுமை
trunc_string(
    char_u	*s,
    char_u	*buf,
    முழு		room_in,
    முழு		buflen)
{
    size_t	room = room_in - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    முழு		e;
    முழு		i;
    முழு		n;

    எனில் (*s == NUL)
    {
	எனில் (buflen > 0)
	    *buf = NUL;
	திரும்பு;
    }

    எனில் (room_in < 3)
	room = 0;
    half = room / 2;

    // First part: Start of the string.
    க்கு (e = 0; len < half && e < buflen; ++e)
    {
	எனில் (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    திரும்பு;
	}
	n = ptr2cells(s + e);
	எனில் (len + n > half)
	    நிறுத்து;
	len += n;
	buf[e] = s[e];
	எனில் (has_mbyte)
	    க்கு (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		எனில் (++e == buflen)
		    நிறுத்து;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    எனில் (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	போது (len + n > room)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    இல்லையெனில் எனில் (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (முழு)STRLEN(s);
	க்கு (;;)
	{
	    செய்
		half = half - utf_head_off(s, s + half - 1) - 1;
	    போது (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    எனில் (len + n > room || half == 0)
		நிறுத்து;
	    len += n;
	    i = (முழு)half;
	}
    }
    இல்லையெனில்
    {
	க்கு (i = (முழு)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= room; --i)
	    len += n;
    }


    எனில் (i <= e + 3)
    {
	// text fits without truncating
	எனில் (s != buf)
	{
	    len = STRLEN(s);
	    எனில் (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    எனில் (len < 1)
		buf[e - 1] = NUL;
	    இல்லையெனில்
		mch_memmove(buf + e, s + e, len);
	}
    }
    இல்லையெனில் எனில் (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	எனில் (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    இல்லையெனில்
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

முழு vim_snprintf(எழுத்து *str, size_t str_m, மாறிலி எழுத்து *fmt, ...);

    முழு
smsg(மாறிலி எழுத்து *s, ...)
{
    எனில் (IObuff == வெற்று)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	திரும்பு செய்தி((எழுத்து *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((எழுத்து *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    திரும்பு செய்தி((எழுத்து *)IObuff);
}

    முழு
smsg_attr(முழு attr, மாறிலி எழுத்து *s, ...)
{
    எனில் (IObuff == வெற்று)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	திரும்பு செய்தி_attr((எழுத்து *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((எழுத்து *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    திரும்பு செய்தி_attr((எழுத்து *)IObuff, attr);
}

    முழு
smsg_attr_keep(முழு attr, மாறிலி எழுத்து *s, ...)
{
    எனில் (IObuff == வெற்று)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	திரும்பு செய்தி_attr_keep((எழுத்து *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((எழுத்து *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    திரும்பு செய்தி_attr_keep((எழுத்து *)IObuff, attr, TRUE);
}

#endif

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
நிலையான முழு	last_sourcing_lnum = 0;
நிலையான char_u   *last_sourcing_name = வெற்று;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    வெறுமை
reset_last_sourcing(வெறுமை)
{
    VIM_CLEAR(last_sourcing_name);
    last_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    நிலையான முழு
other_sourcing_name(வெறுமை)
{
    எனில் (HAVE_SOURCING_INFO && SOURCING_NAME != வெற்று)
    {
	எனில் (last_sourcing_name != வெற்று)
	    திரும்பு STRCMP(SOURCING_NAME, last_sourcing_name) != 0;
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    நிலையான char_u *
get_emsg_source(வெறுமை)
{
    char_u	*Buf, *p;

    எனில் (HAVE_SOURCING_INFO && SOURCING_NAME != வெற்று && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	எனில் (sname == வெற்று)
	    sname = SOURCING_NAME;

#ifdef FEAT_EVAL
	எனில் (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	இல்லையெனில்
#endif
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	எனில் (Buf != வெற்று)
	    sprintf((எழுத்து *)Buf, (எழுத்து *)p, sname);
	vim_free(tofree);
	திரும்பு Buf;
    }
    திரும்பு வெற்று;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    நிலையான char_u *
get_emsg_lnum(வெறுமை)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    எனில் (SOURCING_NAME != வெற்று
	    && (other_sourcing_name() || SOURCING_LNUM != last_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	எனில் (Buf != வெற்று)
	    sprintf((எழுத்து *)Buf, (எழுத்து *)p, (long)SOURCING_LNUM);
	திரும்பு Buf;
    }
    திரும்பு வெற்று;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    வெறுமை
செய்தி_source(முழு attr)
{
    char_u	*p;
    நிலையான முழு	recursive = FALSE;

    // Bail out if something called here causes an error.
    எனில் (recursive)
	திரும்பு;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_source();
    எனில் (p != வெற்று)
    {
	செய்தி_scroll = TRUE;  // this will take more than one line
	செய்தி_attr((எழுத்து *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    எனில் (p != வெற்று)
    {
	செய்தி_attr((எழுத்து *)p, HL_ATTR(HLF_N));
	vim_free(p);
	last_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    எனில் (SOURCING_NAME == வெற்று || other_sourcing_name())
    {
	VIM_CLEAR(last_sourcing_name);
	எனில் (SOURCING_NAME != வெற்று)
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
    நிலையான முழு
emsg_not_now(வெறுமை)
{
    எனில் ((emsg_off > 0 && vim_strchr(p_debug, 'm') == வெற்று
					  && vim_strchr(p_debug, 't') == வெற்று)
#ifdef FEAT_EVAL
	    || emsg_skip > 0
#endif
	    )
	திரும்பு TRUE;
    திரும்பு FALSE;
}

#எனில் defined(FEAT_EVAL)
நிலையான garray_T ignore_error_list = GA_EMPTY;

    வெறுமை
ignore_error_for_testing(char_u *error)
{
    எனில் (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, அளவு(char_u *), 1);

    எனில் (STRCMP("RESET", error) == 0)
	ga_clear_strings(&ignore_error_list);
    இல்லையெனில்
	ga_copy_string(&ignore_error_list, error);
}

    நிலையான முழு
ignore_error(மாறிலி எழுத்து *செய்தி)
{
    முழு i;

    க்கு (i = 0; i < ignore_error_list.ga_len; ++i)
	எனில் (strstr(செய்தி,
		  (எழுத்து *)((char_u **)(ignore_error_list.ga_data))[i]) != வெற்று)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}
#endif

#எனில் !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    வெறுமை
do_perror(எழுத்து *செய்தி)
{
    perror(செய்தி);
    ++emsg_silent;
    emsg(செய்தி);
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
    நிலையான முழு
emsg_core(மாறிலி எழுத்து *s)
{
    முழு		attr;
    char_u	*p;
    முழு		r;
#ifdef FEAT_EVAL
    முழு		ignore = FALSE;
    முழு		severe;
#endif

#ifdef FEAT_EVAL
    // When testing some errors are turned into a normal message.
    எனில் (ignore_error(s))
	// don't call msg() if it results in a dialog
	திரும்பு செய்தி_use_printf() ? FALSE : செய்தி((எழுத்து *)s);
#endif

    ++called_emsg;

#ifdef FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#endif

    எனில் (!emsg_off || vim_strchr(p_debug, 't') != வெற்று)
    {
#ifdef FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	எனில் (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    எனில் (!ignore)
		++did_emsg;
	    திரும்பு TRUE;
	}

	எனில் (in_assert_fails && emsg_assert_fails_செய்தி == வெற்று)
	{
	    emsg_assert_fails_செய்தி = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == வெற்று ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#endif

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	எனில் (emsg_silent != 0)
	{
#ifdef FEAT_EVAL
	    ++did_emsg_silent;
#endif
	    எனில் (emsg_noredir == 0)
	    {
		செய்தி_start();
		p = get_emsg_source();
		எனில் (p != வெற்று)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		எனில் (p != வெற்று)
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
	    எனில் (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#endif
#ifdef FEAT_EVAL
	    ch_log(வெற்று, "ERROR silent: %s", s);
#endif
	    திரும்பு TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	செய்தி_silent = 0;
	cmd_silent = FALSE;

	எனில் (global_busy)		// break :global command
	    ++global_busy;

	எனில் (p_eb)
	    beep_flush();		// also includes flush_buffers()
	இல்லையெனில்
	    flush_buffers(FLUSH_MINIMAL);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ifdef FEAT_EVAL
	++uncaught_emsg;
#endif
    }

#ifdef HAS_MESSAGE_WINDOW
    எனில் (!in_echowindow)
#endif
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    எனில் (செய்தி_scrolled != 0)
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
    செய்தி_scroll = TRUE;
    செய்தி_source(attr);

    /*
     * Display the error message itself.
     */
    செய்தி_nowait = FALSE;			// wait for this msg
    r = செய்தி_attr((எழுத்து *)s, attr);

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = FALSE;
#endif
    திரும்பு r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    முழு
emsg(எழுத்து *s)
{
    // Skip this if not giving error messages at the moment.
    எனில் (emsg_not_now())
	திரும்பு TRUE;

    திரும்பு emsg_core(s);
}

#ifndef PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    முழு
semsg(மாறிலி எழுத்து *s, ...)
{
    // Skip this if not giving error messages at the moment.
    எனில் (emsg_not_now())
	திரும்பு TRUE;

    எனில் (IObuff == வெற்று)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	திரும்பு emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((எழுத்து *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    திரும்பு emsg_core((எழுத்து *)IObuff);
}
#endif

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    வெறுமை
iemsg(எழுத்து *s)
{
    எனில் (emsg_not_now())
	திரும்பு;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    emsg_core(s);
#எனில் defined(ABORT_ON_INTERNAL_ERROR) && defined(FEAT_EVAL)
    set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    செய்தி_putchar('\n');  // avoid overwriting the error message
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
    வெறுமை
siemsg(மாறிலி எழுத்து *s, ...)
{
    எனில் (emsg_not_now())
	திரும்பு;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    எனில் (IObuff == வெற்று)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    இல்லையெனில்
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((எழுத்து *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((எழுத்து *)IObuff);
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    செய்தி_putchar('\n');  // avoid overwriting the error message
    out_flush();
    abort();
# endif
}
#endif

/*
 * Give an "Internal error" message.
 */
    வெறுமை
internal_error(எழுத்து *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    siemsg(_(e_internal_error_str), where);
}

#எனில் defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    வெறுமை
internal_error_no_abort(எழுத்து *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    semsg(_(e_internal_error_str), where);
}
#endif

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    வெறுமை
emsg_invreg(முழு name)
{
    semsg(_(e_invalid_register_name_str), transchar_buf(வெற்று, name));
}

#எனில் defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    வெறுமை
emsg_namelen(எழுத்து *செய்தி, char_u *name, முழு len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(செய்தி, copy == வெற்று ? "NULL" : (எழுத்து *)copy);
    vim_free(copy);
}
#endif

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    எழுத்து *
செய்தி_trunc_attr(எழுத்து *s, முழு force, முழு attr)
{
    முழு		n;
    எழுத்து	*ts;

    // Add message to history before truncating
    add_செய்தி_hist((char_u *)s, -1, attr);

    ts = (எழுத்து *)செய்தி_may_trunc(force, (char_u *)s);

    செய்தி_hist_off = TRUE;
    n = செய்தி_attr(ts, attr);
    செய்தி_hist_off = FALSE;

    எனில் (n)
	திரும்பு ts;
    திரும்பு வெற்று;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
செய்தி_may_trunc(முழு force, char_u *s)
{
    முழு		n;
    முழு		room;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    room = (முழு)(Rows - cmdline_row - 1) * cmdline_width + sc_col - 1;
    எனில் (room > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_active))
	    && (n = (முழு)STRLEN(s) - room) > 0)
    {
	எனில் (has_mbyte)
	{
	    முழு	size = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    எனில் (size <= room)
		திரும்பு s;

	    க்கு (n = 0; size >= room; )
	    {
		size -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    திரும்பு s;
}

    நிலையான வெறுமை
add_செய்தி_hist(
    char_u	*s,
    முழு		len,		// -1 for undetermined length
    முழு		attr)
{
    கட்டமைப்பு செய்தி_hist *p;

    எனில் (செய்தி_hist_off || செய்தி_silent != 0)
	திரும்பு;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(கட்டமைப்பு செய்தி_hist);
    எனில் (p == வெற்று)
	திரும்பு;

    எனில் (len < 0)
	len = (முழு)STRLEN(s);
    // remove leading and trailing newlines
    போது (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    போது (len > 0 && s[len - 1] == '\n')
	--len;
    p->செய்தி = vim_strnsave(s, len);
    p->next = வெற்று;
    p->attr = attr;
    எனில் (last_செய்தி_hist != வெற்று)
	last_செய்தி_hist->next = p;
    last_செய்தி_hist = p;
    எனில் (first_செய்தி_hist == வெற்று)
	first_செய்தி_hist = last_செய்தி_hist;
    ++செய்தி_hist_len;

    check_செய்தி_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    முழு
delete_first_செய்தி(வெறுமை)
{
    கட்டமைப்பு செய்தி_hist *p;

    எனில் (செய்தி_hist_len <= 0)
	திரும்பு FAIL;
    p = first_செய்தி_hist;
    first_செய்தி_hist = p->next;
    எனில் (first_செய்தி_hist == வெற்று)
	last_செய்தி_hist = வெற்று;  // history is empty
    vim_free(p->செய்தி);
    vim_free(p);
    --செய்தி_hist_len;
    திரும்பு OK;
}

    நிலையான வெறுமை
check_செய்தி_hist(வெறுமை)
{
    // Don't let the message history get too big
    போது (செய்தி_hist_len > 0 && செய்தி_hist_len > செய்தி_hist_max)
	(வெறுமை)delete_first_செய்தி();
}


    முழு
messagesopt_changed(வெறுமை)
{
    char_u	*p;
    முழு		messages_flags_new = 0;
    முழு		messages_wait_new = 0;
    முழு		messages_history_new = 0;

    p = p_mopt;
    போது (*p != NUL)
    {
	எனில் (STRNCMP(p, MESSAGES_OPT_HIT_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER);
	    messages_flags_new |= MESSAGES_HIT_ENTER;
	}
	இல்லையெனில் எனில் (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	இல்லையெனில் எனில் (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	எனில் (*p != ',' && *p != NUL)
	    திரும்பு FAIL;
	எனில் (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    எனில் (!(messages_flags_new & (MESSAGES_HIT_ENTER | MESSAGES_WAIT)))
	திரும்பு FAIL;

    // "history" must be set
    எனில் (!(messages_flags_new & MESSAGES_HISTORY))
	திரும்பு FAIL;

    எனில் (messages_history_new < 0 || messages_history_new > 10000)
	திரும்பு FAIL;

    எனில் (messages_wait_new < 0 || messages_wait_new > 10000)
	திரும்பு FAIL;

    செய்தி_flags = messages_flags_new;
    செய்தி_wait = messages_wait_new;

    செய்தி_hist_max = messages_history_new;
    check_செய்தி_hist();

    திரும்பு OK;
}

/*
 * ":messages" command.
 */
    வெறுமை
ex_messages(exarg_T *eap)
{
    கட்டமைப்பு செய்தி_hist *p;
    char_u	    *s;
    முழு		    c = 0;

    எனில் (STRCMP(eap->arg, "clear") == 0)
    {
	முழு keep = eap->addr_மொத்தம் == 0 ? 0 : eap->line2;

	போது (செய்தி_hist_len > keep)
	    (வெறுமை)delete_first_செய்தி();
	திரும்பு;
    }

    எனில் (*eap->arg != NUL)
    {
	emsg(_(e_invalid_argument));
	திரும்பு;
    }

    செய்தி_hist_off = TRUE;

    p = first_செய்தி_hist;
    எனில் (eap->addr_மொத்தம் != 0)
    {
	// Count total messages
	க்கு (; p != வெற்று && !got_int; p = p->next)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	க்கு (p = first_செய்தி_hist; p != வெற்று && !got_int && c > 0;
						    p = p->next, c--);
    }

    எனில் (p == first_செய்தி_hist)
    {
#ifdef FEAT_MULTI_LANG
	s = get_mess_lang();
#இல்லையெனில்
	s = mch_getenv((char_u *)"LANG");
#endif
	எனில் (s != வெற்று && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    செய்தி_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    க்கு (; p != வெற்று && !got_int; p = p->next)
	எனில் (p->செய்தி != வெற்று)
	    செய்தி_attr((எழுத்து *)p->செய்தி, p->attr);

    செய்தி_hist_off = FALSE;
}

#எனில் defined(FEAT_CON_DIALOG) || defined(FIND_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    வெறுமை
செய்தி_end_prompt(வெறுமை)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_row = செய்தி_row;
    செய்தி_col = 0;
    செய்தி_clr_eos();
    வரிகள்_இடது = -1;
}
#endif

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    வெறுமை
wait_return(முழு redraw)
{
    முழு		c;
    முழு		oldState;
    முழு		tmpState;
    முழு		had_got_int;
    முழு		save_reg_recording;
    FILE	*save_scriptout;

    எனில் (redraw == TRUE)
	set_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    எனில் (செய்தி_silent != 0)
	திரும்பு;
#ifdef HAS_MESSAGE_WINDOW
    எனில் (in_echowindow)
	திரும்பு;
#endif

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    எனில் (vgetc_busy > 0)
	திரும்பு;
    need_wait_return = TRUE;
    எனில் (no_wait_return)
    {
	எனில் (!exmode_active)
	    cmdline_row = செய்தி_row;
	திரும்பு;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    எனில் (quit_more)
    {
	c = CAR;		// just pretend CR was hit
	quit_more = FALSE;
	got_int = FALSE;
    }
    இல்லையெனில் எனில் (exmode_active)
    {
	செய்தி_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    இல்லையெனில் எனில் (!stuff_empty())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    இல்லையெனில்
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = MODE_HITRETURN;
	setmouse();
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#endif
	cmdline_row = செய்தி_row;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	எனில் (need_check_timestamps)
	    check_timestamps(FALSE);

	எனில் (செய்தி_flags & MESSAGES_HIT_ENTER)
	{
	    hit_return_செய்தி();

	    செய்
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
		++allow_சாவிகள்;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = வெற்று;
		c = safe_vgetc();
		எனில் (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_சாவிகள்;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ifdef FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		எனில் (சாவி_Typed && c == Ctrl_Y && clip_star.state == SELECT_DONE)
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
		எனில் (சாவி_Typed && p_more && !p_cp)
		{
		    எனில் (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_UP || c == K_PAGEUP)
		    {
			எனில் (செய்தி_scrolled > Rows)
			    // scroll back to show older messages
			    do_more_prompt(c);
			இல்லையெனில்
			{
			    செய்தி_didout = FALSE;
			    c = K_IGNORE;
			    செய்தி_col =
#ifdef FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_width - 1 :
#endif
				0;
			}
			எனில் (quit_more)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_more = FALSE;
			    got_int = FALSE;
			}
			இல்லையெனில் எனில் (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    hit_return_செய்தி();
			}
		    }
		    இல்லையெனில் எனில் (செய்தி_scrolled > Rows - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_DOWN || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } போது ((had_got_int && c == Ctrl_C)
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
				    && mouse_row < செய்தி_row
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    எனில் (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(வெறுமை)jump_to_mouse(MOUSE_SETPOS, வெற்று, 0);
	    இல்லையெனில் எனில் (!சாவி_Typed || (vim_strchr((char_u *)"\r\n ", c) == வெற்று
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_mask);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	இல்லையெனில்
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(செய்தி_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    எனில் (c == ':' || c == '?' || c == '/')
    {
	எனில் (!exmode_active)
	    cmdline_row = செய்தி_row;
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
    செய்தி_check();

#ifndef MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    எனில் (swapping_screen() && !termcap_active)
	newline_on_exit = TRUE;
#endif

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    வரிகள்_இடது = -1;		// reset lines_left at next msg_start()
    reset_last_sourcing();
    எனில் (keep_செய்தி != வெற்று && vim_strsize(keep_செய்தி) >=
			    (Rows - cmdline_row - 1) * cmdline_width + sc_col)
	VIM_CLEAR(keep_செய்தி);	    // don't redisplay message, it's too long

    எனில் (tmpState == MODE_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    இல்லையெனில் எனில் (!skip_redraw
	    && (redraw == TRUE || (செய்தி_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    நிலையான வெறுமை
hit_return_செய்தி(வெறுமை)
{
    முழு		save_p_more = p_more;

    p_more = FALSE;	// don't want to see this message when scrolling back
    எனில் (செய்தி_didout)	// start on a new line
	செய்தி_putchar('\n');
    எனில் (got_int)
	செய்தி_puts(_("Interrupt: "));

    செய்தி_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    எனில் (!செய்தி_use_printf())
	செய்தி_clr_eos();
    p_more = save_p_more;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    வெறுமை
set_keep_செய்தி(char_u *s, முழு attr)
{
    vim_free(keep_செய்தி);
    எனில் (s != வெற்று && செய்தி_silent == 0)
	keep_செய்தி = vim_strsave(s);
    இல்லையெனில்
	keep_செய்தி = வெற்று;
    keep_செய்தி_more = FALSE;
    keep_செய்தி_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    வெறுமை
set_keep_செய்தி_from_hist(வெறுமை)
{
    எனில் (keep_செய்தி == வெற்று && last_செய்தி_hist != வெற்று && செய்தி_scrolled == 0
						      && (State & MODE_NORMAL))
	set_keep_செய்தி(last_செய்தி_hist->செய்தி, last_செய்தி_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    வெறுமை
செய்தி_start(வெறுமை)
{
    முழு		did_return = FALSE;

    எனில் (செய்தி_row < cmdline_row)
	செய்தி_row = cmdline_row;

    எனில் (!செய்தி_silent)
    {
	VIM_CLEAR(keep_செய்தி);
	need_fileinfo = FALSE;
    }

#ifdef FEAT_EVAL
    எனில் (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	செய்தி_clr_eos();
    }
#endif

#ifdef HAS_MESSAGE_WINDOW
    எனில் (in_echowindow)
    {
	எனில் (popup_message_win_visible()
		    && ((செய்தி_col > 0 && (செய்தி_scroll || !full_screen))
			|| in_echowindow))
	{
	    win_T *wp = popup_get_message_win();

	    // start a new line
	    curbuf = wp->w_buffer;
	    ml_append(wp->w_buffer->b_ml.ml_வரி_மொத்தம்,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_buffer;
	}
	செய்தி_col = 0;
    }
    இல்லையெனில்
#endif
	எனில் (!செய்தி_scroll && full_screen)	// overwrite last message
    {
	செய்தி_row = cmdline_row;
	செய்தி_col =
#ifdef FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_width - 1 :
#endif
	    0;
    }
    இல்லையெனில் எனில் (செய்தி_didout || in_echowindow)
    {
	// start message on next line
	செய்தி_putchar('\n');
	did_return = TRUE;
	எனில் (exmode_active != EXMODE_NORMAL)
	    cmdline_row = செய்தி_row;
    }
    எனில் (!செய்தி_didany || வரிகள்_இடது < 0)
	செய்தி_starthere();
    எனில் (செய்தி_silent == 0)
    {
	செய்தி_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    எனில் (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    வெறுமை
செய்தி_starthere(வெறுமை)
{
    வரிகள்_இடது = cmdline_row;
    செய்தி_didany = FALSE;
}

    வெறுமை
செய்தி_putchar(முழு c)
{
    செய்தி_putchar_attr(c, 0);
}

    வெறுமை
செய்தி_putchar_attr(முழு c, முழு attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    எனில் (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    இல்லையெனில்
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    செய்தி_puts_attr((எழுத்து *)buf, attr);
}

    வெறுமை
செய்தி_outnum(long n)
{
    எழுத்து	buf[20];

    sprintf(buf, "%ld", n);
    செய்தி_puts(buf);
}

    வெறுமை
செய்தி_home_replace(char_u *fname)
{
    செய்தி_home_replace_attr(fname, 0);
}

#எனில் defined(FEAT_FIND_ID)
    வெறுமை
செய்தி_home_replace_hl(char_u *fname)
{
    செய்தி_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#endif

    நிலையான வெறுமை
செய்தி_home_replace_attr(char_u *fname, முழு attr)
{
    char_u	*name;

    name = home_replace_save(வெற்று, fname);
    எனில் (name != வெற்று)
	செய்தி_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    முழு
செய்தி_outtrans(char_u *str)
{
    திரும்பு செய்தி_outtrans_attr(str, 0);
}

    முழு
செய்தி_outtrans_attr(char_u *str, முழு attr)
{
    திரும்பு செய்தி_outtrans_len_attr(str, (முழு)STRLEN(str), attr);
}

    முழு
செய்தி_outtrans_len(char_u *str, முழு len)
{
    திரும்பு செய்தி_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
செய்தி_outtrans_one(char_u *p, முழு attr)
{
    முழு		l;

    எனில் (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	செய்தி_outtrans_len_attr(p, l, attr);
	திரும்பு p + l;
    }
    செய்தி_puts_attr((எழுத்து *)transchar_byte_buf(வெற்று, *p), attr);
    திரும்பு p + 1;
}

    முழு
செய்தி_outtrans_len_attr(char_u *msgstr, முழு len, முழு attr)
{
    முழு		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_start = msgstr;
    char_u	*s;
    முழு		mb_l;
    முழு		c;
    முழு		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    எனில் (attr == 0)
	attr = HL_ATTR(HLF_செய்தி);

    // if MSG_HIST flag set, add message to history
    எனில் (attr & செய்தி_HIST)
    {
	add_செய்தி_hist(str, len, attr);
	attr &= ~செய்தி_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    எனில் (செய்தி_silent == 0 && len > 0 && செய்தி_row >= cmdline_row && செய்தி_col == 0)
    {
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    எனில் (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	செய்தி_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    போது (--len >= 0 && !got_int)
    {
	எனில் (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	இல்லையெனில் எனில் (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	இல்லையெனில்
	    mb_l = 1;
	எனில் (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    எனில் (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    இல்லையெனில்
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		எனில் (str > plain_start)
		    செய்தி_puts_attr_len((எழுத்து *)plain_start,
					       (முழு)(str - plain_start), attr);
		plain_start = str + mb_l;
		செய்தி_puts_attr((எழுத்து *)transchar_buf(வெற்று, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	இல்லையெனில்
	{
	    s = transchar_byte_buf(வெற்று, *str);
	    எனில் (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		எனில் (str > plain_start)
		    செய்தி_puts_attr_len((எழுத்து *)plain_start,
					       (முழு)(str - plain_start), attr);
		plain_start = str + 1;
		செய்தி_puts_attr((எழுத்து *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (முழு)STRLEN(s);
	    }
	    இல்லையெனில்
		++retval;
	    ++str;
	}
    }

    எனில் (str > plain_start && !got_int)
	// print the printable chars at the end
	செய்தி_puts_attr_len((எழுத்து *)plain_start, (முழு)(str - plain_start), attr);

    got_int |= save_got_int;

    திரும்பு retval;
}

#எனில் defined(FEAT_QUICKFIX)
    வெறுமை
செய்தி_make(char_u *arg)
{
    முழு	    i;
    நிலையான char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    க்கு (i = 5; *arg && i >= 0; --i)
	எனில் (*arg++ != str[i])
	    நிறுத்து;
    எனில் (i < 0)
    {
	செய்தி_putchar('\n');
	க்கு (i = 0; rs[i]; ++i)
	    செய்தி_putchar(rs[i] - 3);
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
    முழு
செய்தி_outtrans_special(
    char_u	*strstart,
    முழு		from,	// TRUE for lhs of a mapping
    முழு		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    முழு		retval = 0;
    எழுத்து	*text;
    முழு		attr;
    முழு		len;

    attr = HL_ATTR(HLF_8);
    போது (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	எனில் ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    text = "<Space>";
	    ++str;
	}
	இல்லையெனில்
	    text = (எழுத்து *)str2special(&str, from, FALSE);
	எனில் (text[0] != NUL && text[1] == NUL)
	    // single-byte character or illegal byte
	    text = (எழுத்து *)transchar_byte_buf(வெற்று, (char_u)text[0]);
	len = vim_strsize((char_u *)text);
	எனில் (maxlen > 0 && retval + len >= maxlen)
	    நிறுத்து;
	// Highlight special keys
	செய்தி_puts_attr(text, len > 1
		&& (*mb_ptr2len)((char_u *)text) <= 1 ? attr : 0);
	retval += len;
    }
    திரும்பு retval;
}

#எனில் defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    முழு	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    முழு	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    போது (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    திரும்பு (char_u *)ga.ga_data;
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
    முழு		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    முழு		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    முழு			c;
    நிலையான char_u	buf[7];
    char_u		*str = *sp;
    முழு			modifiers = 0;
    முழு			special = FALSE;

    எனில் (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	எனில் (p != வெற்று)
	    திரும்பு p;
    }

    c = *str;
    எனில் ((c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	) && str[1] != NUL && str[2] != NUL)
    {
	எனில் (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	எனில் ((c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = TO_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	எனில் (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    எனில் (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	எனில் (p != வெற்று)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	இல்லையெனில்
	    // illegal byte
	    *sp = str + 1;
    }
    இல்லையெனில்
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    எனில் (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	திரும்பு get_special_சாவி_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    திரும்பு buf;
}

/*
 * Translate a key sequence into special key names.
 */
    வெறுமை
str2specialbuf(char_u *sp, char_u *buf, முழு len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    போது (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	எனில் (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	இல்லையெனில்
	    நிறுத்து;
    }
}

/*
 * print line for :print or :list command
 */
    வெறுமை
செய்தி_prt_வரி(char_u *s, முழு list)
{
    முழு		c;
    முழு		col = 0;
    முழு		n_extra = 0;
    முழு		c_extra = 0;
    முழு		c_final = 0;
    char_u	*p_extra = வெற்று;	    // init to make SASC shut up
    முழு		n;
    முழு		attr = 0;
    char_u	*trail = வெற்று;
    char_u	*lead = வெற்று;
    முழு		in_multispace = FALSE;
    முழு		multispace_pos = 0;
    முழு		l;
    char_u	buf[MB_MAXBYTES + 1];

    எனில் (curwin->w_p_list)
	list = TRUE;

    எனில் (list)
    {
	// find start of trailing whitespace
	எனில் (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    போது (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	எனில் (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != வெற்று
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    போது (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    எனில் (*lead == NUL)
		lead = வெற்று;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    எனில் (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	செய்தி_putchar(' ');

    போது (!got_int)
    {
	எனில் (n_extra > 0)
	{
	    --n_extra;
	    எனில் (n_extra == 0 && c_final)
		c = c_final;
	    இல்லையெனில் எனில் (c_extra)
		c = c_extra;
	    இல்லையெனில்
		c = *p_extra++;
	}
	இல்லையெனில் எனில் (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    col += (*mb_ptr2cells)(s);
	    எனில் (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    இல்லையெனில் எனில் (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		முழு len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    இல்லையெனில்
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    செய்தி_puts((எழுத்து *)buf);
	    s += l;
	    தொடர்;
	}
	இல்லையெனில்
	{
	    attr = 0;
	    c = *s++;
	    எனில் (list)
	    {
		in_multispace = c == ' ' && (*s == ' '
						 || (col > 0 && s[-2] == ' '));
		எனில் (!in_multispace)
		    multispace_pos = 0;
	    }
	    எனில் (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ifdef FEAT_VARTABS
		n_extra = tabstop_padding(col, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#இல்லையெனில்
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
#endif
		எனில் (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		இல்லையெனில்
		{
		    முழு lcs_tab1 = curwin->w_lcs_chars.tab1;
		    முழு lcs_tab2 = curwin->w_lcs_chars.tab2;
		    முழு lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    எனில் (lead != வெற்று && s <= lead
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
	    இல்லையெனில் எனில் (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    இல்லையெனில் எனில் (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    இல்லையெனில் எனில் (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(வெற்று, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    இல்லையெனில் எனில் (c == ' ')
	    {
		எனில் (lead != வெற்று && s <= lead && in_multispace
			&& curwin->w_lcs_chars.leadmultispace != வெற்று)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    எனில் (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		இல்லையெனில் எனில் (lead != வெற்று && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		இல்லையெனில் எனில் (trail != வெற்று && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		இல்லையெனில் எனில் (in_multispace
			&& curwin->w_lcs_chars.multispace != வெற்று)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    எனில் (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		இல்லையெனில் எனில் (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	எனில் (c == NUL)
	    நிறுத்து;

	செய்தி_putchar_attr(c, attr);
	col++;
    }
    செய்தி_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    நிலையான char_u *
screen_puts_mbyte(char_u *s, முழு l, முழு attr)
{
    முழு		cw;

    செய்தி_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    எனில் (cw > 1 && (
#ifdef FEAT_RIGHTLEFT
		cmdmsg_rl ? செய்தி_col <= 1 :
#endif
		செய்தி_col == cmdline_width - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	செய்தி_screen_putchar('>', HL_ATTR(HLF_AT));
	திரும்பு s;
    }

    screen_puts_len(s, l, செய்தி_row, cmdline_col_off + செய்தி_col, attr);
#ifdef FEAT_RIGHTLEFT
    எனில் (cmdmsg_rl)
    {
	செய்தி_col -= cw;
	எனில் (செய்தி_col == 0)
	{
	    செய்தி_col = cmdline_width;
	    ++செய்தி_row;
	}
    }
    இல்லையெனில்
#endif
    {
	செய்தி_col += cw;
	எனில் (செய்தி_col >= cmdline_width)
	{
	    செய்தி_col = 0;
	    ++செய்தி_row;
	}
    }
    திரும்பு s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    வெறுமை
செய்தி_puts(எழுத்து *s)
{
    செய்தி_puts_attr(s, 0);
}

    வெறுமை
செய்தி_puts_title(எழுத்து *s)
{
    செய்தி_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    நிலையான வெறுமை
செய்தி_outtrans_long_len_attr(char_u *longstr, முழு len, முழு attr)
{
    முழு		slen = len;
    முழு		room;

    room = cmdline_width - செய்தி_col;
    எனில் (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	செய்தி_outtrans_len_attr(longstr, slen, attr);
	செய்தி_puts_attr("...", HL_ATTR(HLF_8));
    }
    செய்தி_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    வெறுமை
செய்தி_outtrans_long_attr(char_u *longstr, முழு attr)
{
    செய்தி_outtrans_long_len_attr(longstr, (முழு)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    வெறுமை
செய்தி_puts_attr(எழுத்து *s, முழு attr)
{
    செய்தி_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    நிலையான வெறுமை
செய்தி_puts_attr_len(எழுத்து *str, முழு maxlen, முழு attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    எனில் (செய்தி_silent != 0)
	திரும்பு;

    எனில் (attr == 0)
	attr = HL_ATTR(HLF_செய்தி);

    // if MSG_HIST flag set, add message to history
    எனில் ((attr & செய்தி_HIST) && maxlen < 0)
    {
	add_செய்தி_hist((char_u *)str, -1, attr);
	attr &= ~செய்தி_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    எனில் (செய்தி_scrolled != 0 && !செய்தி_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    செய்தி_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    எனில் (செய்தி_use_printf())
	செய்தி_puts_printf((char_u *)str, maxlen);
    இல்லையெனில்
	செய்தி_puts_display((char_u *)str, maxlen, attr, FALSE);

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
    நிலையான வெறுமை
put_செய்தி_win(win_T *wp, முழு where, char_u *t_s, char_u *end, linenr_T lnum)
{
    char_u  *p;

    எனில் (where == PUT_BELOW)
    {
	எனில் (*end != NUL)
	{
	    p = vim_strnsave(t_s, end - t_s);
	    எனில் (p == வெற்று)
		திரும்பு;
	}
	இல்லையெனில்
	    p = t_s;
	ml_append_buf(wp->w_buffer, lnum, p, (colnr_T)0, FALSE);
	எனில் (p != t_s)
	    vim_free(p);
    }
    இல்லையெனில்
    {
	char_u *newp;

	curbuf = wp->w_buffer;
	எனில் (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    எனில் (newp == வெற்று)
		திரும்பு;
	    எனில் (*end != NUL)
		newp[STRLEN(ml_get(lnum)) + (end - t_s)] = NUL;
	}
	இல்லையெனில்
	{
	    newp = vim_strnsave(t_s, end - t_s);
	    எனில் (newp == வெற்று)
		திரும்பு;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_buffer;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    செய்தி_col += (முழு)(end - t_s);
}
#endif

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    நிலையான வெறுமை
செய்தி_puts_display(
    char_u	*str,
    முழு		maxlen,
    முழு		attr,
    முழு		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    முழு		t_col = 0;	// screen cells todo, 0 when "t_s" not used
    முழு		l;
    முழு		cw;
    char_u	*sb_str = str;
    முழு		sb_col = செய்தி_col;
    முழு		wrap;
    முழு		did_last_char;
#ifdef HAS_MESSAGE_WINDOW
    முழு		where = PUT_APPEND;
    win_T	*செய்தி_win = வெற்று;
    linenr_T    lnum = 1;

    எனில் (in_echowindow)
    {
	செய்தி_win = popup_get_message_win();

	எனில் (செய்தி_win != வெற்று)
	{
	    எனில் (!popup_message_win_visible())
	    {
		எனில் (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    செய்தி_win = வெற்று;
		}
		இல்லையெனில்
		{
		    // currently hidden, make it empty
		    curbuf = செய்தி_win->w_buffer;
		    போது ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
			ml_delete(1);
		    curbuf = curwin->w_buffer;
		}
	    }
	    இல்லையெனில்
	    {
		lnum = செய்தி_win->w_buffer->b_ml.ml_வரி_மொத்தம்;
		எனில் (செய்தி_col == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#endif

    did_wait_return = FALSE;
    போது ((maxlen < 0 || (முழு)(s - str) < maxlen) && *s != NUL)
    {
#ifdef HAS_MESSAGE_WINDOW
	// For echowindow, use full width; for regular messages, leave last column
	முழு wrap_col = செய்தி_win != வெற்று ? cmdline_width : cmdline_width - 1;
#இல்லையெனில்
	முழு wrap_col = cmdline_width - 1;
#endif

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	எனில் (!recurse && செய்தி_row >= Rows - 1 && (*s == '\n' || (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			செய்தி_col <= 1
		      || (*s == TAB && செய்தி_col <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && செய்தி_col <= 2))
		    :
#endif
		      ((*s != '\r' && செய்தி_col + t_col >= wrap_col)
		       || (*s == TAB && செய்தி_col + t_col
			   >= (wrap_col & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && செய்தி_col + t_col >= wrap_col - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    எனில் (t_col > 0)
	    {
		// output postponed text
#ifdef HAS_MESSAGE_WINDOW
		எனில் (செய்தி_win != வெற்று)
		{
		    put_செய்தி_win(செய்தி_win, where, t_s, s, lnum);
		    எனில் (where == PUT_BELOW)
			++lnum;
		    t_col = 0;
		    where = PUT_BELOW;
		}
		இல்லையெனில்
#endif
		    t_puts(&t_col, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    எனில் (செய்தி_no_more && வரிகள்_இடது == 0)
		நிறுத்து;

#ifdef HAS_MESSAGE_WINDOW
	    எனில் (செய்தி_win == வெற்று)
#endif
		// Scroll the screen up one line.
		செய்தி_scroll_up();

#ifdef HAS_MESSAGE_WINDOW
	    எனில் (செய்தி_win == வெற்று)
	    {
#endif
		செய்தி_row = Rows - 2;
		எனில் (செய்தி_col >= cmdline_width)  // can happen after screen resize
		    செய்தி_col = cmdline_width - 1;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif

	    // Display char in last column before showing more-prompt.
	    எனில் (*s >= ' '
#ifdef FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#endif
	       )
	    {
		எனில் (has_mbyte)
		{
		    எனில் (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (முழு)((str + maxlen) - s));
		    இல்லையெனில்
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		இல்லையெனில்
		    செய்தி_screen_putchar(*s++, attr);
		did_last_char = TRUE;
	    }
	    இல்லையெனில்
		did_last_char = FALSE;

	    எனில் (p_more)
		// store text for scrolling back
		store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

#ifdef HAS_MESSAGE_WINDOW
	    எனில் (செய்தி_win == வெற்று)
	    {
#endif
		inc_செய்தி_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		எனில் (cmdline_row > 0 && !exmode_active)
		    --cmdline_row;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		எனில் (வரிகள்_இடது > 0)
		    --வரிகள்_இடது;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif
	    எனில் (p_more && வரிகள்_இடது == 0 && State != MODE_HITRETURN
					    && !செய்தி_no_more && !exmode_active)
	    {
#ifdef FEAT_CON_DIALOG
		எனில் (do_more_prompt(NUL))
		    s = confirm_செய்தி_tail;
#இல்லையெனில்
		(வெறுமை)do_more_prompt(NUL);
#endif
		எனில் (quit_more)
		    திரும்பு;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    எனில் (did_last_char)
		தொடர்;
	}

	wrap = *s == '\n'
		    || செய்தி_col + t_col >= wrap_col
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && செய்தி_col + t_col >= wrap_col - 1);
	எனில் (t_col > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ifdef HAS_MESSAGE_WINDOW
	    எனில் (செய்தி_win != வெற்று)
	    {
		put_செய்தி_win(செய்தி_win, where, t_s, s, lnum);
		எனில் (where == PUT_BELOW)
		    ++lnum;
		t_col = 0;
		where = PUT_BELOW;
		// Reset msg_col after outputting to new line in echowindow
		எனில் (wrap)
		    செய்தி_col = 0;
	    }
	    இல்லையெனில்
#endif
		t_puts(&t_col, t_s, s, attr);
	}

	எனில் (wrap && p_more && !recurse)
	    // store text for scrolling back
	    store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	எனில் (*s == '\n')		    // go to next line
	{
#ifdef HAS_MESSAGE_WINDOW
	    எனில் (செய்தி_win != வெற்று)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		எனில் ((செய்தி_win->w_buffer->b_ml.ml_flags & ML_EMPTY) == 0)
		{
		    put_செய்தி_win(செய்தி_win, PUT_BELOW, t_s, t_s, lnum);
		    ++lnum;
		}
		செய்தி_col = 0;  // Reset column for new line
	    }
	    இல்லையெனில்
#endif
		செய்தி_didout = FALSE;	    // remember that line is empty
#ifdef FEAT_RIGHTLEFT
	    எனில் (cmdmsg_rl)
		செய்தி_col = cmdline_width - 1;
	    இல்லையெனில்
#endif
		செய்தி_col = 0;
	    எனில் (++செய்தி_row >= Rows)  // safety check
		செய்தி_row = Rows - 1;
	}
	இல்லையெனில் எனில் (*s == '\r')	    // go to column 0
	{
	    செய்தி_col = 0;
#ifdef HAS_MESSAGE_WINDOW
	    where = PUT_TRUNC;
#endif
	}
	இல்லையெனில் எனில் (*s == '\b')	    // go to previous char
	{
	    எனில் (செய்தி_col)
		--செய்தி_col;
	}
	இல்லையெனில் எனில் (*s == TAB)	    // translate Tab into spaces
	{
#ifdef HAS_MESSAGE_WINDOW
	    எனில் (செய்தி_win != வெற்று)
		செய்தி_col = (செய்தி_col + 7) % 8;
	    இல்லையெனில்
#endif
		செய்
		    செய்தி_screen_putchar(' ', attr);
		போது (செய்தி_col & 7);
	}
	இல்லையெனில் எனில் (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	இல்லையெனில்
	{
	    எனில் (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		எனில் (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (முழு)((str + maxlen) - s));
		இல்லையெனில்
		    l = (*mb_ptr2len)(s);
	    }
	    இல்லையெனில்
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    எனில் (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#endif
		    (cw > 1 && செய்தி_col + t_col >= wrap_col))
	    {
		எனில் (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		இல்லையெனில்
		    செய்தி_screen_putchar(*s, attr);
	    }
	    இல்லையெனில்
	    {
		// postpone this character until later
		எனில் (t_col == 0)
		    t_s = s;
		t_col += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    எனில் (t_col > 0)
    {
#ifdef HAS_MESSAGE_WINDOW
	எனில் (செய்தி_win != வெற்று)
	    put_செய்தி_win(செய்தி_win, where, t_s, s, lnum);
	இல்லையெனில்
#endif
	    t_puts(&t_col, t_s, s, attr);
    }

#ifdef HAS_MESSAGE_WINDOW
    எனில் (செய்தி_win != வெற்று)
	popup_show_message_win();
#endif
    // Store the text for scroll back.
    எனில் (p_more && !recurse)
	store_sb_text(&sb_str, s, attr, &sb_col, FALSE);

    செய்தி_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    முழு
message_filtered(char_u *செய்தி)
{
    முழு match;

    எனில் (cmdmod.cmod_filter_regmatch.regprog == வெற்று)
	திரும்பு FALSE;
    match = vim_regexec(&cmdmod.cmod_filter_regmatch, செய்தி, (colnr_T)0);
    திரும்பு cmdmod.cmod_filter_force ? match : !match;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    நிலையான வெறுமை
செய்தி_scroll_up(வெறுமை)
{
#ifdef HAS_MESSAGE_WINDOW
    எனில் (in_echowindow)
	திரும்பு;
#endif
#ifdef FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    எனில் (gui.in_use)
	gui_undraw_cursor();
#endif
    // scrolling up always works
    mch_disable_flush();
    screen_del_வரிகள்(0, 0, 1, (முழு)Rows, TRUE, 0, வெற்று);
    mch_enable_flush();

    எனில் (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((முழு)Rows - 1, (முழு)Rows, cmdline_col_off,
		cmdline_col_off + cmdline_width, ' ', ' ', HL_ATTR(HLF_செய்தி));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	எனில் (ScreenAttrs[வரி_Offset[Rows - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((முழு)Rows - 2, (முழு)Rows - 1,
				 (முழு)Columns - 1, (முழு)Columns,
				 ' ', ' ', HL_ATTR(HLF_செய்தி));
    }
}

/*
 * Increment "msg_scrolled".
 */
    நிலையான வெறுமை
inc_செய்தி_scrolled(வெறுமை)
{
#ifdef FEAT_EVAL
    எனில் (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = வெற்று;

	// v:scrollstart is empty, set it to the script/function name and line
	//எண்
	எனில் (p.string == வெற்று)
	{
	    p.string = (char_u *)_("Unknown");
	    p.length = STRLEN(p.string);
	}
	இல்லையெனில்
	{
	    size_t  tofreesize;

	    tofreesize = (முழு)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    எனில் (tofree != வெற்று)
	    {
		p.length = vim_snprintf_safelen((எழுத்து *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (long)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	set_vim_var_string(VV_SCROLLSTART, p.string, (முழு)p.length);
	vim_free(tofree);
    }
#endif
    ++செய்தி_scrolled;
    set_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
வகைவரை கட்டமைப்பு msgchunk_S msgchunk_T;
கட்டமைப்பு msgchunk_S
{
    msgchunk_T	*sb_next;
    msgchunk_T	*sb_prev;
    எழுத்து	sb_eol;		// TRUE when line ends after this text
    முழு		sb_செய்தி_col;	// column in which text starts
    முழு		sb_attr;	// text attributes
    char_u	sb_text[1];	// text to be displayed, actually longer
};

நிலையான msgchunk_T *last_msgchunk = வெற்று; // last displayed text

நிலையான msgchunk_T *செய்தி_sb_start(msgchunk_T *mps);

வகைவரை எண்ணிக்கை {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_DONE
} sb_clear_T;

// When to clear text on next msg.
நிலையான sb_clear_T do_clear_sb_text = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    நிலையான வெறுமை
store_sb_text(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    முழு		attr,
    முழு		*sb_col,
    முழு		finish)		// line ends
{
    msgchunk_T	*mp;

    எனில் (do_clear_sb_text == SB_CLEAR_ALL
	    || do_clear_sb_text == SB_CLEAR_CMDLINE_DONE)
    {
	clear_sb_text(do_clear_sb_text == SB_CLEAR_ALL);
	செய்தி_sb_eol();  // prevent messages from overlapping
	do_clear_sb_text = SB_CLEAR_NONE;
    }

    எனில் (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_text) + (s - *sb_str) + 1);
	எனில் (mp != வெற்று)
	{
	    mp->sb_eol = finish;
	    mp->sb_செய்தி_col = *sb_col;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_text, *sb_str, s - *sb_str);

	    எனில் (last_msgchunk == வெற்று)
	    {
		last_msgchunk = mp;
		mp->sb_prev = வெற்று;
	    }
	    இல்லையெனில்
	    {
		mp->sb_prev = last_msgchunk;
		last_msgchunk->sb_next = mp;
		last_msgchunk = mp;
	    }
	    mp->sb_next = வெற்று;
	}
    }
    இல்லையெனில் எனில் (finish && last_msgchunk != வெற்று)
	last_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_col = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    வெறுமை
may_clear_sb_text(வெறுமை)
{
    do_clear_sb_text = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    வெறுமை
sb_text_start_cmdline(வெறுமை)
{
    எனில் (do_clear_sb_text == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_text_restart_cmdline();
    இல்லையெனில்
    {
	செய்தி_sb_eol();
	do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    வெறுமை
sb_text_restart_cmdline(வெறுமை)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;

    எனில் (last_msgchunk == வெற்று || last_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	திரும்பு;

    tofree = செய்தி_sb_start(last_msgchunk);
    last_msgchunk = tofree->sb_prev;
    எனில் (last_msgchunk != வெற்று)
	last_msgchunk->sb_next = வெற்று;
    போது (tofree != வெற்று)
    {
	msgchunk_T *tofree_next = tofree->sb_next;

	vim_free(tofree);
	tofree = tofree_next;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    வெறுமை
sb_text_end_cmdline(வெறுமை)
{
    do_clear_sb_text = SB_CLEAR_CMDLINE_DONE;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    வெறுமை
clear_sb_text(முழு all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    எனில் (all)
	lastp = &last_msgchunk;
    இல்லையெனில்
    {
	எனில் (last_msgchunk == வெற்று)
	    திரும்பு;
	lastp = &செய்தி_sb_start(last_msgchunk)->sb_prev;
    }

    போது (*lastp != வெற்று)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    வெறுமை
show_sb_text(வெறுமை)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = செய்தி_sb_start(last_msgchunk);
    எனில் (mp == வெற்று || mp->sb_prev == வெற்று)
	vim_beep(BO_MESS);
    இல்லையெனில்
    {
	do_more_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    நிலையான msgchunk_T *
செய்தி_sb_start(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    போது (mp != வெற்று && mp->sb_prev != வெற்று && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    திரும்பு mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    வெறுமை
செய்தி_sb_eol(வெறுமை)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    எனில் (last_msgchunk != வெற்று
	    && !(last_msgchunk->sb_text[0] == '\n'
		&& last_msgchunk->sb_text[1] == NUL))
	last_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    நிலையான msgchunk_T *
disp_sb_வரி(முழு row, msgchunk_T *smp, முழு clear_to_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    க்கு (;;)
    {
	செய்தி_row = row;
	செய்தி_col = mp->sb_செய்தி_col;
	p = mp->sb_text;
	எனில் (*p == '\n')	    // don't display the line break
	    ++p;
	செய்தி_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	எனில் (clear_to_eol)
	    screen_fill(row, row + 1,
		    cmdline_col_off + செய்தி_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', HL_ATTR(HLF_செய்தி));

	எனில் (mp->sb_eol || mp->sb_next == வெற்று)
	    நிறுத்து;
	mp = mp->sb_next;
    }
    திரும்பு mp->sb_next;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    நிலையான வெறுமை
t_puts(
    முழு		*t_col,
    char_u	*t_s,
    char_u	*s,
    முழு		attr)
{
    // output postponed text
    செய்தி_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (முழு)(s - t_s), செய்தி_row, cmdline_col_off + செய்தி_col,
	    attr);
    செய்தி_col += *t_col;
    *t_col = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    எனில் (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--செய்தி_col;
    எனில் (செய்தி_col >= cmdline_width)
    {
	செய்தி_col = 0;
	++செய்தி_row;
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
    முழு
செய்தி_use_printf(வெறுமை)
{
    திரும்பு (!செய்தி_check_screen()
#எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.in_use && !termcap_active)
# இல்லையெனில்
	    || !termcap_active
# endif
#endif
	    || (swapping_screen() && !termcap_active)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    நிலையான வெறுமை
செய்தி_puts_printf(char_u *str, முழு maxlen)
{
    char_u	*s = str;
    char_u	*buf = வெற்று;
    char_u	*p = s;

#ifdef MSWIN
    எனில் (!(silent_mode && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#endif
    போது ((maxlen < 0 || (முழு)(s - str) < maxlen) && *s != NUL)
    {
	எனில் (!(silent_mode && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    எனில் (*s == NL)
	    {
		முழு n = (முழு)(s - p);

		buf = alloc(n + 3);
		எனில் (buf != வெற்று)
		{
		    memcpy(buf, p, n);
		    எனில் (!info_message)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    எனில் (info_message)   // informative message, not an error
			mch_செய்தி((எழுத்து *)buf);
		    இல்லையெனில்
			mch_errmsg((எழுத்து *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ifdef FEAT_RIGHTLEFT
	எனில் (cmdmsg_rl)
	{
	    எனில் (*s == CAR || *s == NL)
		செய்தி_col = cmdline_width - 1;
	    இல்லையெனில்
		--செய்தி_col;
	}
	இல்லையெனில்
#endif
	{
	    எனில் (*s == CAR || *s == NL)
		செய்தி_col = 0;
	    இல்லையெனில்
		++செய்தி_col;
	}
	++s;
    }

    எனில் (*p != NUL && !(silent_mode && p_verbose == 0))
    {
	char_u *tofree = வெற்று;

	எனில் (maxlen > 0 && vim_strlen_maxlen((எழுத்து *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	எனில் (p != வெற்று)
	{
	    எனில் (info_message)
		mch_செய்தி((எழுத்து *)p);
	    இல்லையெனில்
		mch_errmsg((எழுத்து *)p);
	    vim_free(tofree);
	}
    }

    செய்தி_didout = TRUE;	    // assume that line is not empty

#ifdef MSWIN
    எனில் (!(silent_mode && p_verbose == 0))
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
    நிலையான முழு
do_more_prompt(முழு typed_char)
{
    நிலையான முழு	entered = FALSE;
    முழு		used_typed_char = typed_char;
    முழு		oldState = State;
    முழு		c;
#ifdef FEAT_CON_DIALOG
    முழு		retval = FALSE;
#endif
    முழு		toscroll;
    msgchunk_T	*mp_last = வெற்று;
    msgchunk_T	*mp;
    முழு		i;
    முழு		செய்தி_attr;

    செய்தி_attr = HL_ATTR(HLF_செய்தி);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    எனில் (entered || (State == MODE_HITRETURN && typed_char == 0))
	திரும்பு FALSE;
    entered = TRUE;

    எனில் (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_last = செய்தி_sb_start(last_msgchunk);
	க்கு (i = 0; i < Rows - 2 && mp_last != வெற்று
					     && mp_last->sb_prev != வெற்று; ++i)
	    mp_last = செய்தி_sb_start(mp_last->sb_prev);
    }

    State = MODE_ASKMORE;
    setmouse();
    எனில் (typed_char == NUL)
	செய்தி_moremsg(FALSE);
    க்கு (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	எனில் (used_typed_char != NUL)
	{
	    c = used_typed_char;	// was typed at hit-enter prompt
	    used_typed_char = NUL;
	}
	இல்லையெனில்
	    c = get_keystroke();

#எனில் defined(FEAT_MENU) && defined(FEAT_GUI)
	எனில் (c == K_MENU)
	{
	    முழு idx = get_menu_index(current_menu, MODE_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    எனில் (idx == MENU_INDEX_INVALID)
		தொடர்;
	    c = *current_menu->strings[idx];
	    எனில் (c != NUL && current_menu->strings[idx][1] != NUL)
		ins_typebuf(current_menu->strings[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#endif

	toscroll = 0;
	தேர்வு (c)
	{
	நிலை BS:		// scroll one line back
	நிலை K_BS:
	நிலை 'k':
	நிலை K_UP:
	    toscroll = -1;
	    நிறுத்து;

	நிலை CAR:		// one extra line
	நிலை NL:
	நிலை 'j':
	நிலை K_DOWN:
	    toscroll = 1;
	    நிறுத்து;

	நிலை 'u':		// Up half a page
	    toscroll = -(Rows / 2);
	    நிறுத்து;

	நிலை 'd':		// Down half a page
	    toscroll = Rows / 2;
	    நிறுத்து;

	நிலை 'b':		// one page back
	நிலை Ctrl_B:
	நிலை K_PAGEUP:
	    toscroll = -(Rows - 1);
	    நிறுத்து;

	நிலை ' ':		// one extra page
	நிலை 'f':
	நிலை Ctrl_F:
	நிலை K_PAGEDOWN:
	நிலை K_LEFTMOUSE:
	    toscroll = Rows - 1;
	    நிறுத்து;

	நிலை 'g':		// all the way back to the start
	    toscroll = -999999;
	    நிறுத்து;

	நிலை 'G':		// all the way to the end
	    toscroll = 999999;
	    வரிகள்_இடது = 999999;
	    நிறுத்து;

	நிலை ':':		// start new command line
#ifdef FEAT_CON_DIALOG
	    எனில் (!confirm_செய்தி_used)
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
	நிலை 'q':		// quit
	நிலை Ctrl_C:
	நிலை ESC:
#ifdef FEAT_CON_DIALOG
	    எனில் (confirm_செய்தி_used)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    இல்லையெனில்
#endif
	    {
		got_int = TRUE;
		quit_more = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    வரிகள்_இடது = Rows - 1;
	    நிறுத்து;

#ifdef FEAT_CLIPBOARD
	நிலை Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    எனில் (clip_star.state == SELECT_DONE)
		clip_copy_modeless_selection(TRUE);
	    தொடர்;
#endif
	இயல்பு:		// no valid response
	    செய்தி_moremsg(TRUE);
	    தொடர்;
	}

	எனில் (toscroll != 0)
	{
	    எனில் (toscroll < 0)
	    {
		// go to start of last line
		எனில் (mp_last == வெற்று)
		    mp = செய்தி_sb_start(last_msgchunk);
		இல்லையெனில் எனில் (mp_last->sb_prev != வெற்று)
		    mp = செய்தி_sb_start(mp_last->sb_prev);
		இல்லையெனில்
		    mp = வெற்று;

		// go to start of line at top of the screen
		க்கு (i = 0; i < Rows - 2 && mp != வெற்று && mp->sb_prev != வெற்று;
									  ++i)
		    mp = செய்தி_sb_start(mp->sb_prev);

		எனில் (mp != வெற்று && mp->sb_prev != வெற்று)
		{
		    // Find line to be displayed at top.
		    க்கு (i = 0; i > toscroll; --i)
		    {
			எனில் (mp == வெற்று || mp->sb_prev == வெற்று)
			    நிறுத்து;
			mp = செய்தி_sb_start(mp->sb_prev);
			எனில் (mp_last == வெற்று)
			    mp_last = செய்தி_sb_start(last_msgchunk);
			இல்லையெனில்
			    mp_last = செய்தி_sb_start(mp_last->sb_prev);
		    }

		    எனில் (toscroll == -1 && screen_ins_வரிகள்(0, 0, 1,
						     (முழு)Rows, 0, வெற்று) == OK)
		    {
			// display line at top
			(வெறுமை)disp_sb_வரி(0, mp, FALSE);
		    }
		    இல்லையெனில்
		    {
			முழு did_clear = screenclear();

			// redisplay all lines
			க்கு (i = 0; mp != வெற்று && i < Rows - 1; ++i)
			{
			    mp = disp_sb_வரி(i, mp, !did_clear);
			    ++செய்தி_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    இல்லையெனில்
	    {
		// First display any text that we scrolled back.
		போது (toscroll > 0 && mp_last != வெற்று)
		{
		    // scroll up, display line at bottom
		    செய்தி_scroll_up();
		    inc_செய்தி_scrolled();
		    screen_fill((முழு)Rows - 2, (முழு)Rows - 1,
			    cmdline_col_off, cmdline_col_off + cmdline_width,
			    ' ', ' ', செய்தி_attr);
		    mp_last = disp_sb_வரி((முழு)Rows - 2, mp_last, FALSE);
		    --toscroll;
		}
	    }

	    எனில் (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((முழு)Rows - 1, (முழு)Rows, cmdline_col_off,
			cmdline_col_off + cmdline_width, ' ', ' ', செய்தி_attr);
		செய்தி_moremsg(FALSE);
		தொடர்;
	    }

	    // display more text, return to caller
	    வரிகள்_இடது = toscroll;
	}

	நிறுத்து;
    }

    // clear the --more-- message
    screen_fill((முழு)Rows - 1, (முழு)Rows, cmdline_col_off,
	    cmdline_col_off + cmdline_width, ' ', ' ', செய்தி_attr);
    State = oldState;
    setmouse();
    எனில் (quit_more)
    {
	செய்தி_row = Rows - 1;
	செய்தி_col = 0;
    }
#ifdef FEAT_RIGHTLEFT
    இல்லையெனில் எனில் (cmdmsg_rl)
	செய்தி_col = cmdline_width - 1;
#endif

    entered = FALSE;
#ifdef FEAT_CON_DIALOG
    திரும்பு retval;
#இல்லையெனில்
    திரும்பு FALSE;
#endif
}

#எனில் defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_செய்தி
#  undef mch_செய்தி
# endif

# எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    நிலையான வெறுமை
mch_errmsg_c(எழுத்து *str)
{
    DWORD   nwrite = 0;
    DWORD   mode = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    எனில் (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (முழு)GetConsoleCP() != enc_codepage)
    {
	முழு	len = (முழு)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	எனில் (w != வெற்று)
	{
	    WriteConsoleW(h, w, len, &nwrite, வெற்று);
	    vim_free(w);
	}
    }
    இல்லையெனில்
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
    வெறுமை
mch_errmsg(எழுத்து *str)
{
# எனில் !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    முழு		len;
# endif

# எனில் (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    எனில் (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   இல்லையெனில்
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
	திரும்பு;
    }
# endif

# எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    எனில் (!(gui.in_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	திரும்பு;
    }
# endif

# எனில் !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (முழு)STRLEN(str) + 1;
    எனில் (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    எனில் (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_data + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_data + error_ga.ga_len;
	    க்கு (;;)
	    {
		p = vim_strchr(p, '\r');
		எனில் (p == வெற்று)
		    நிறுத்து;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    நிலையான வெறுமை
mch_செய்தி_c(எழுத்து *str)
{
    DWORD   nwrite = 0;
    DWORD   mode;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    எனில் (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (முழு)GetConsoleCP() != enc_codepage)
    {
	முழு	len = (முழு)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	எனில் (w != வெற்று)
	{
	    WriteConsoleW(h, w, len, &nwrite, வெற்று);
	    vim_free(w);
	}
    }
    இல்லையெனில்
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
    வெறுமை
mch_செய்தி(எழுத்து *str)
{
# எனில் (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    எனில் (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   இல்லையெனில்
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
	திரும்பு;
    }
# endif

# எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    எனில் (!(gui.in_use || gui.starting))
#  endif
    {
	mch_செய்தி_c(str);
	திரும்பு;
    }
# endif
# எனில் !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#endif // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    நிலையான வெறுமை
செய்தி_screen_putchar(முழு c, முழு attr)
{
    செய்தி_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, செய்தி_row, cmdline_col_off + செய்தி_col, attr);
#ifdef FEAT_RIGHTLEFT
    எனில் (cmdmsg_rl)
    {
	எனில் (--செய்தி_col == 0)
	{
	    செய்தி_col = cmdline_width;
	    ++செய்தி_row;
	}
    }
    இல்லையெனில்
#endif
    {
	எனில் (++செய்தி_col >= cmdline_width)
	{
	    செய்தி_col = 0;
	    ++செய்தி_row;
	}
    }
}

    நிலையான வெறுமை
செய்தி_moremsg(முழு full)
{
    முழு		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (முழு)Rows - 1, cmdline_col_off, attr);
    எனில் (full)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(முழு)Rows - 1, cmdline_col_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    வெறுமை
repeat_message(வெறுமை)
{
    எனில் (State == MODE_ASKMORE)
    {
	செய்தி_moremsg(TRUE);	// display --more-- message again
	செய்தி_row = Rows - 1;
    }
#ifdef FEAT_CON_DIALOG
    இல்லையெனில் எனில் (State == MODE_CONFIRM)
    {
	display_confirm_செய்தி();	// display ":confirm" message again
	செய்தி_row = Rows - 1;
    }
#endif
    இல்லையெனில் எனில் (State == MODE_EXTERNCMD)
    {
	windgoto(செய்தி_row, cmdline_col_off + செய்தி_col); // put cursor back
    }
    இல்லையெனில் எனில் (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	எனில் (செய்தி_row == Rows - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    செய்தி_didout = FALSE;
	    செய்தி_col = 0;
	    செய்தி_clr_eos();
	}
	hit_return_செய்தி();
	செய்தி_row = Rows - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    நிலையான முழு
செய்தி_check_screen(வெறுமை)
{
    எனில் (!full_screen || !screen_valid(FALSE))
	திரும்பு FALSE;

    எனில் (செய்தி_row >= Rows)
	செய்தி_row = Rows - 1;
    எனில் (செய்தி_col >= cmdline_width)
	செய்தி_col = cmdline_width - 1;
    திரும்பு TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    வெறுமை
செய்தி_clr_eos(வெறுமை)
{
    எனில் (செய்தி_silent == 0)
	செய்தி_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    வெறுமை
செய்தி_clr_eos_force(வெறுமை)
{
#ifdef HAS_MESSAGE_WINDOW
    எனில் (in_echowindow)
	திரும்பு;  // messages go into a popup
#endif
    எனில் (செய்தி_use_printf())
    {
	எனில் (full_screen)	// only when termcap codes are valid
	{
	    எனில் (*T_CD)
		out_str(T_CD);	// clear to end of display
	    இல்லையெனில் எனில் (*T_CE)
		out_str(T_CE);	// clear to end of line
	}
    }
    இல்லையெனில்
    {
	முழு செய்தி_attr;

	செய்தி_attr = HL_ATTR(HLF_செய்தி);

#ifdef FEAT_RIGHTLEFT
	எனில் (cmdmsg_rl)
	{
	    screen_fill(செய்தி_row, செய்தி_row + 1,
		    cmdline_col_off, cmdline_col_off + செய்தி_col + 1,
		    ' ', ' ', செய்தி_attr);
	    screen_fill(செய்தி_row + 1, (முழு)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', செய்தி_attr);
	}
	இல்லையெனில்
#endif
	{
	    screen_fill(செய்தி_row, செய்தி_row + 1,
		    cmdline_col_off + செய்தி_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', செய்தி_attr);
	    screen_fill(செய்தி_row + 1, (முழு)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', செய்தி_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    வெறுமை
செய்தி_clr_cmdline(வெறுமை)
{
    செய்தி_row = cmdline_row;
    செய்தி_col = 0;
    செய்தி_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    முழு
செய்தி_end(வெறுமை)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    எனில் (!exiting && need_wait_return && !(State & MODE_CMDLINE))
    {
	wait_return(FALSE);
	திரும்பு FALSE;
    }
    out_flush();
    திரும்பு TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    வெறுமை
செய்தி_check(வெறுமை)
{
    எனில் (செய்தி_row == Rows - 1 && செய்தி_col >= sc_col
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
    நிலையான வெறுமை
redir_write(char_u *str, முழு maxlen)
{
    char_u	*s = str;
    நிலையான முழு	cur_col = 0;

    // Don't do anything for displaying prompts and the like.
    எனில் (redir_off)
	திரும்பு;

    // If 'verbosefile' is set prepare for writing in that file.
    எனில் (*p_vfile != NUL && verbose_fd == வெற்று)
	verbose_open();

    எனில் (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	எனில் (*s != '\n' && *s != '\r')
	{
	    போது (cur_col < செய்தி_col)
	    {
#ifdef FEAT_EVAL
		எனில் (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		இல்லையெனில் எனில் (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		இல்லையெனில் எனில் (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		இல்லையெனில்
#endif
		    எனில் (redir_fd != வெற்று)
		    fputs(" ", redir_fd);
		எனில் (verbose_fd != வெற்று)
		    fputs(" ", verbose_fd);
		++cur_col;
	    }
	}

#ifdef FEAT_EVAL
	எனில் (redir_execute)
	    execute_redir_str(s, maxlen);
	இல்லையெனில் எனில் (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	இல்லையெனில் எனில் (redir_vname)
	    var_redir_str(s, maxlen);
#endif

	// Write and adjust the current column.
	போது (*s != NUL && (maxlen < 0 || (முழு)(s - str) < maxlen))
	{
#ifdef FEAT_EVAL
	    எனில் (!redir_reg && !redir_vname && !redir_execute)
#endif
		எனில் (redir_fd != வெற்று)
		    putc(*s, redir_fd);
	    எனில் (verbose_fd != வெற்று)
		putc(*s, verbose_fd);
	    எனில் (*s == '\r' || *s == '\n')
		cur_col = 0;
	    இல்லையெனில் எனில் (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    இல்லையெனில்
		++cur_col;
	    ++s;
	}

	எனில் (செய்தி_silent != 0)	// should update msg_col
	    செய்தி_col = cur_col;
    }
}

    முழு
redirecting(வெறுமை)
{
    திரும்பு redir_fd != வெற்று || *p_vfile != NUL
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#endif
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    வெறுமை
verbose_enter(வெறுமை)
{
    எனில் (*p_vfile != NUL)
	++செய்தி_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    வெறுமை
verbose_leave(வெறுமை)
{
    எனில் (*p_vfile != NUL)
	எனில் (--செய்தி_silent < 0)
	    செய்தி_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    வெறுமை
verbose_enter_scroll(வெறுமை)
{
    எனில் (*p_vfile != NUL)
	++செய்தி_silent;
    இல்லையெனில்
	// always scroll up, don't overwrite
	செய்தி_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    வெறுமை
verbose_leave_scroll(வெறுமை)
{
    எனில் (*p_vfile != NUL)
    {
	எனில் (--செய்தி_silent < 0)
	    செய்தி_silent = 0;
    }
    இல்லையெனில்
	cmdline_row = செய்தி_row;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    வெறுமை
verbose_stop(வெறுமை)
{
    எனில் (verbose_fd != வெற்று)
    {
	fclose(verbose_fd);
	verbose_fd = வெற்று;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    முழு
verbose_open(வெறுமை)
{
    எனில் (verbose_fd == வெற்று && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((எழுத்து *)p_vfile, "a");
	எனில் (verbose_fd == வெற்று)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    திரும்பு FAIL;
	}
    }
    திரும்பு OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    வெறுமை
give_warning(char_u *message, முழு hl)
{
    give_warning_with_source(message, hl, FALSE);
}

    வெறுமை
give_warning_with_source(char_u *message, முழு hl, முழு with_source)
{
    // Don't do this for ":silent".
    எனில் (செய்தி_silent != 0)
	திரும்பு;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, message, -1);
#endif
    VIM_CLEAR(keep_செய்தி);
    எனில் (hl)
	keep_செய்தி_attr = HL_ATTR(HLF_W);
    இல்லையெனில்
	keep_செய்தி_attr = 0;

    எனில் (with_source)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	செய்தி_start();
	செய்தி_source(HL_ATTR(HLF_W));
	செய்தி_puts(" ");
	செய்தி_puts_attr((எழுத்து *)message, HL_ATTR(HLF_W) | செய்தி_HIST);
	செய்தி_clr_eos();
	(வெறுமை)செய்தி_end();
    }
    இல்லையெனில் எனில் (செய்தி_attr((எழுத்து *)message, keep_செய்தி_attr) && செய்தி_scrolled == 0)
	set_keep_செய்தி(message, keep_செய்தி_attr);

    செய்தி_didout = FALSE;	    // overwrite this message
    செய்தி_nowait = TRUE;	    // don't wait for this message
    செய்தி_col = 0;

    --no_wait_return;
}

#எனில் defined(FEAT_EVAL)
    வெறுமை
give_warning2(char_u *message, char_u *a1, முழு hl)
{
    எனில் (IObuff == வெற்று)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(message, hl);
    }
    இல்லையெனில்
    {
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, (எழுத்து *)message, a1);
	give_warning(IObuff, hl);
    }
}
#endif

/*
 * Advance msg cursor to column "col".
 */
    வெறுமை
செய்தி_advance(முழு col)
{
    எனில் (செய்தி_silent != 0)	// nothing to advance to
    {
	செய்தி_col = col;		// for redirection, may fill it up later
	திரும்பு;
    }
    எனில் (col >= cmdline_width)	// not enough room
	col = cmdline_width - 1;
#ifdef FEAT_RIGHTLEFT
    எனில் (cmdmsg_rl)
	போது (செய்தி_col > cmdline_width - col)
	    செய்தி_putchar(' ');
    இல்லையெனில்
#endif
	போது (செய்தி_col < col)
	    செய்தி_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    வெறுமை
செய்தி_warn_missing_clipboard(வெறுமை)
{
    எனில் (!global_busy && !did_warn_clipboard && silence_w23_w24_செய்தி == 0)
    {
#ifdef FEAT_CLIPBOARD
	செய்தி(_("W23: Clipboard register not available, using register 0"));
#இல்லையெனில்
	செய்தி(_("W24: Clipboard register not available. See :h W24"));
#endif
	did_warn_clipboard = TRUE;
    }
}

#எனில் defined(FEAT_CON_DIALOG)
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
    முழு
do_dialog(
    முழு		type UNUSED,
    char_u	*title UNUSED,
    char_u	*message,
    char_u	*buttons,
    முழு		dfltbutton,
    char_u	*textfield UNUSED,	// IObuff for inputdialog(), NULL
					// otherwise
    முழு		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    முழு		oldState;
    முழு		retval = 0;
    char_u	*hotkeys;
    முழு		c;
    முழு		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    எனில் (silent_mode)
	திரும்பு dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    எனில் (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == வெற்று)
    {
	// --gui-dialog-file: write text to a file
	எனில் (gui_dialog_log(title, message))
	    c = dfltbutton;
	இல்லையெனில்
	    c = gui_mch_dialog(type, title, message, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_row = செய்தி_row;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	out_flush();
	gui_mch_update();

	திரும்பு c;
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
    hotkeys = செய்தி_show_console_dialog(message, buttons, dfltbutton);

    எனில் (hotkeys != வெற்று)
    {
	க்கு (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    தேர்வு (c)
	    {
	    நிலை CAR:		// User accepts default option
	    நிலை NL:
		retval = dfltbutton;
		நிறுத்து;
	    நிலை Ctrl_C:	// User aborts/cancels
	    நிலை ESC:
		retval = 0;
		நிறுத்து;
	    இயல்பு:		// Could be a hotkey?
		எனில் (c < 0)	// special keys are ignored here
		    தொடர்;
		எனில் (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    நிறுத்து;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		க்கு (i = 0; hotkeys[i]; ++i)
		{
		    எனில் (has_mbyte)
		    {
			எனில் ((*mb_ptr2char)(hotkeys + i) == c)
			    நிறுத்து;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    இல்லையெனில்
			எனில் (hotkeys[i] == c)
			    நிறுத்து;
		    ++retval;
		}
		எனில் (hotkeys[i])
		    நிறுத்து;
		// No hotkey match, so keep waiting
		தொடர்;
	    }
	    நிறுத்து;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    செய்தி_end_prompt();

    திரும்பு retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    நிலையான முழு
copy_char(
    char_u	*from,
    char_u	*to,
    முழு		lowercase)	// make character lower case
{
    முழு		len;
    முழு		c;

    எனில் (has_mbyte)
    {
	எனில் (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    திரும்பு (*mb_char2bytes)(c, to);
	}
	இல்லையெனில்
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(to, from, (size_t)len);
	    திரும்பு len;
	}
    }
    இல்லையெனில்
    {
	எனில் (lowercase)
	    *to = (char_u)TOLOWER_LOC(*from);
	இல்லையெனில்
	    *to = *from;
	திரும்பு 1;
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
    நிலையான char_u *
செய்தி_show_console_dialog(
    char_u	*message,
    char_u	*buttons,
    முழு		dfltbutton)
{
    முழு		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    முழு		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = வெற்று;
    char_u	*msgp = வெற்று;
    char_u	*hotkp = வெற்று;
    char_u	*r;
    முழு		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    முழு		first_hotkey = FALSE;	// first char of button is hotkey
    முழு		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    க்கு (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	போது (*r)
	{
	    எனில் (*r == DLG_BUTTON_SEP)
	    {
		எனில் (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    எனில் (has_mbyte)
			hotkp += STRLEN(hotkp);
		    இல்லையெனில்
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    எனில் (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    எனில் (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			first_hotkey = TRUE;
		}
		இல்லையெனில்
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    எனில் (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    இல்லையெனில் எனில் (*r == DLG_HOTKEY_CHAR || first_hotkey)
	    {
		எனில் (*r == DLG_HOTKEY_CHAR)
		    ++r;
		first_hotkey = FALSE;
		எனில் (copy)
		{
		    எனில் (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    இல்லையெனில்
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		இல்லையெனில்
		{
		    ++len;	    // '&a' -> '[a]'
		    எனில் (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    இல்லையெனில்
	    {
		// everything else copy literally
		எனில் (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	எனில் (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	இல்லையெனில்
	{
	    len += (முழு)(STRLEN(message)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    எனில் (!has_hotkey[0])
	    {
		first_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_செய்தி);
	    confirm_செய்தி = alloc(len);
	    எனில் (confirm_செய்தி == வெற்று)
		திரும்பு வெற்று;
	    *confirm_செய்தி = NUL;
	    hotk = alloc(lenhotkey);
	    எனில் (hotk == வெற்று)
		திரும்பு வெற்று;

	    *confirm_செய்தி = '\n';
	    STRCPY(confirm_செய்தி + 1, message);

	    msgp = confirm_செய்தி + 1 + STRLEN(message);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_செய்தி_tail = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_செய்தி();
    திரும்பு hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    நிலையான வெறுமை
display_confirm_செய்தி(வெறுமை)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_செய்தி_used;
    எனில் (confirm_செய்தி != வெற்று)
	செய்தி_puts_attr((எழுத்து *)confirm_செய்தி, HL_ATTR(HLF_M));
    --confirm_செய்தி_used;
}

#endif // FEAT_CON_DIALOG

#எனில் defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    முழு
vim_dialog_yesno(
    முழு		type,
    char_u	*title,
    char_u	*message,
    முழு		dflt)
{
    எனில் (do_dialog(type,
		title == வெற்று ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No"), dflt, வெற்று, FALSE) == 1)
	திரும்பு VIM_YES;
    திரும்பு VIM_NO;
}

    முழு
vim_dialog_yesnocancel(
    முழு		type,
    char_u	*title,
    char_u	*message,
    முழு		dflt)
{
    தேர்வு (do_dialog(type,
		title == வெற்று ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, வெற்று, FALSE))
    {
	நிலை 1: திரும்பு VIM_YES;
	நிலை 2: திரும்பு VIM_NO;
    }
    திரும்பு VIM_CANCEL;
}

    முழு
vim_dialog_yesnoallcancel(
    முழு		type,
    char_u	*title,
    char_u	*message,
    முழு		dflt)
{
    தேர்வு (do_dialog(type,
		title == வெற்று ? (char_u *)"Question" : title,
		message,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, வெற்று, FALSE))
    {
	நிலை 1: திரும்பு VIM_YES;
	நிலை 2: திரும்பு VIM_NO;
	நிலை 3: திரும்பு VIM_ALL;
	நிலை 4: திரும்பு VIM_DISCARDALL;
    }
    திரும்பு VIM_CANCEL;
}

#endif // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
