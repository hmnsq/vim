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

#अन्तर्भाव "vim.h"

स्थिर शून्य add_सन्देशः_hist(char_u *s, पूर्णाङ्क len, पूर्णाङ्क attr);
स्थिर शून्य check_सन्देशः_hist(शून्य);
स्थिर शून्य hit_return_सन्देशः(शून्य);
स्थिर शून्य सन्देशः_home_replace_attr(char_u *fname, पूर्णाङ्क attr);
स्थिर शून्य सन्देशः_puts_attr_len(वर्ण *str, पूर्णाङ्क maxlen, पूर्णाङ्क attr);
स्थिर शून्य सन्देशः_puts_display(char_u *str, पूर्णाङ्क maxlen, पूर्णाङ्क attr, पूर्णाङ्क recurse);
स्थिर शून्य सन्देशः_scroll_up(शून्य);
स्थिर शून्य inc_सन्देशः_scrolled(शून्य);
स्थिर शून्य store_sb_text(char_u **sb_str, char_u *s, पूर्णाङ्क attr, पूर्णाङ्क *sb_col, पूर्णाङ्क finish);
स्थिर शून्य t_puts(पूर्णाङ्क *t_col, char_u *t_s, char_u *s, पूर्णाङ्क attr);
स्थिर शून्य सन्देशः_puts_printf(char_u *str, पूर्णाङ्क maxlen);
स्थिर पूर्णाङ्क do_more_prompt(पूर्णाङ्क typed_char);
स्थिर शून्य सन्देशः_screen_putchar(पूर्णाङ्क c, पूर्णाङ्क attr);
स्थिर शून्य सन्देशः_moremsg(पूर्णाङ्क full);
स्थिर पूर्णाङ्क  सन्देशः_check_screen(शून्य);
स्थिर शून्य redir_write(char_u *s, पूर्णाङ्क maxlen);
#ifdef FEAT_CON_DIALOG
स्थिर char_u *सन्देशः_show_console_dialog(char_u *message, char_u *buttons, पूर्णाङ्क dfltbutton);
स्थिर पूर्णाङ्क	confirm_सन्देशः_used = FALSE;	// displaying confirm_msg
स्थिर char_u	*confirm_सन्देशः = रिक्त;		// ":confirm" message
स्थिर char_u	*confirm_सन्देशः_tail;		// tail of confirm_msg
स्थिर शून्य display_confirm_सन्देशः(शून्य);
#endif
#ifdef FEAT_EVAL
स्थिर पूर्णाङ्क emsg_to_channel_log = FALSE;
#endif

संरचना सन्देशः_hist
{
    संरचना सन्देशः_hist	*next;
    char_u		*सन्देशः;
    पूर्णाङ्क			attr;
};

स्थिर संरचना सन्देशः_hist *first_सन्देशः_hist = रिक्त;
स्थिर संरचना सन्देशः_hist *last_सन्देशः_hist = रिक्त;
स्थिर पूर्णाङ्क सन्देशः_hist_len = 0;
स्थिर पूर्णाङ्क सन्देशः_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#define MESSAGES_HIT_ENTER	0x001
#define MESSAGES_WAIT		0x002
#define MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#define MESSAGES_OPT_HIT_ENTER "hit-enter"
#define MESSAGES_OPT_WAIT "wait:"
#define MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
स्थिर पूर्णाङ्क सन्देशः_flags = MESSAGES_HIT_ENTER | MESSAGES_HISTORY;
स्थिर पूर्णाङ्क सन्देशः_wait = 0;

स्थिर FILE *verbose_fd = रिक्त;
स्थिर पूर्णाङ्क  verbose_did_open = FALSE;

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
    पूर्णाङ्क
सन्देशः(वर्ण *s)
{
    प्रत्यागम सन्देशः_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    पूर्णाङ्क
verb_सन्देशः(वर्ण *s)
{
    पूर्णाङ्क		n;

    verbose_enter();
    n = सन्देशः_attr_keep(s, 0, FALSE);
    verbose_leave();

    प्रत्यागम n;
}

    पूर्णाङ्क
सन्देशः_attr(वर्ण *s, पूर्णाङ्क attr)
{
    प्रत्यागम सन्देशः_attr_keep(s, attr, FALSE);
}

    पूर्णाङ्क
सन्देशः_attr_keep(
    वर्ण	*s,
    पूर्णाङ्क		attr,
    पूर्णाङ्क		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    स्थिर पूर्णाङ्क	entered = 0;
    पूर्णाङ्क		retval;
    char_u	*buf = रिक्त;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    जइ (!emsg_on_display && message_filtered((char_u *)s))
	प्रत्यागम TRUE;

#ifdef FEAT_EVAL
    जइ (attr == 0)
	set_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#endif

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    जइ (entered >= 3)
	प्रत्यागम TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    जइ ((char_u *)s != keep_सन्देशः
	    || (*s != '<'
		&& last_सन्देशः_hist != रिक्त
		&& last_सन्देशः_hist->सन्देशः != रिक्त
		&& STRCMP(s, last_सन्देशः_hist->सन्देशः) != 0))
	add_सन्देशः_hist((char_u *)s, -1, attr);

#ifdef FEAT_EVAL
    जइ (emsg_to_channel_log)
	// Write message in the channel log.
	ch_log(रिक्त, "ERROR: %s", s);
#endif

    // Truncate the message if needed.
    सन्देशः_start();
    buf = सन्देशः_strtrunc((char_u *)s, FALSE);
    जइ (buf != रिक्त)
	s = (वर्ण *)buf;

    सन्देशः_outtrans_attr((char_u *)s, attr);
    सन्देशः_clr_eos();
    retval = सन्देशः_end();

    जइ (keep && retval && vim_strsize((char_u *)s)
		    < (पूर्णाङ्क)(Rows - cmdline_row - 1) * cmdline_width + sc_col)
	set_keep_सन्देशः((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    प्रत्यागम retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
सन्देशः_strtrunc(
    char_u	*s,
    पूर्णाङ्क		force)	    // always truncate
{
    char_u	*buf = रिक्त;
    पूर्णाङ्क		len;
    पूर्णाङ्क		room;

    // May truncate message to avoid a hit-return prompt
    जइ ((!सन्देशः_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_active && सन्देशः_silent == 0) || force)
    {
	len = vim_strsize(s);
	जइ (सन्देशः_scrolled != 0
#ifdef HAS_MESSAGE_WINDOW
		|| in_echowindow
#endif
		)
	    // Use all the columns.
	    room = (पूर्णाङ्क)(Rows - सन्देशः_row) * cmdline_width - 1;
	अण्णहा
	    // Use up to 'showcmd' column.
	    room = (पूर्णाङ्क)(Rows - सन्देशः_row - 1) * cmdline_width + sc_col - 1;
	जइ (len > room && room > 0)
	{
	    जइ (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (room + 2) * 18;
	    अण्णहा जइ (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (room + 2) * 2;
	    अण्णहा
		len = room + 2;
	    buf = alloc(len);
	    जइ (buf != रिक्त)
		trunc_string(s, buf, room, len);
	}
    }
    प्रत्यागम buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    शून्य
trunc_string(
    char_u	*s,
    char_u	*buf,
    पूर्णाङ्क		room_in,
    पूर्णाङ्क		buflen)
{
    size_t	room = room_in - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    पूर्णाङ्क		e;
    पूर्णाङ्क		i;
    पूर्णाङ्क		n;

    जइ (*s == NUL)
    {
	जइ (buflen > 0)
	    *buf = NUL;
	प्रत्यागम;
    }

    जइ (room_in < 3)
	room = 0;
    half = room / 2;

    // First part: Start of the string.
    प्रति (e = 0; len < half && e < buflen; ++e)
    {
	जइ (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    प्रत्यागम;
	}
	n = ptr2cells(s + e);
	जइ (len + n > half)
	    भङ्ग;
	len += n;
	buf[e] = s[e];
	जइ (has_mbyte)
	    प्रति (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		जइ (++e == buflen)
		    भङ्ग;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    जइ (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	जाव (len + n > room)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    अण्णहा जइ (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (पूर्णाङ्क)STRLEN(s);
	प्रति (;;)
	{
	    करेहि
		half = half - utf_head_off(s, s + half - 1) - 1;
	    जाव (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    जइ (len + n > room || half == 0)
		भङ्ग;
	    len += n;
	    i = (पूर्णाङ्क)half;
	}
    }
    अण्णहा
    {
	प्रति (i = (पूर्णाङ्क)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= room; --i)
	    len += n;
    }


    जइ (i <= e + 3)
    {
	// text fits without truncating
	जइ (s != buf)
	{
	    len = STRLEN(s);
	    जइ (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    जइ (len < 1)
		buf[e - 1] = NUL;
	    अण्णहा
		mch_memmove(buf + e, s + e, len);
	}
    }
    अण्णहा जइ (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	जइ (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    अण्णहा
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

पूर्णाङ्क vim_snprintf(वर्ण *str, size_t str_m, ध्रुव वर्ण *fmt, ...);

    पूर्णाङ्क
smsg(ध्रुव वर्ण *s, ...)
{
    जइ (IObuff == रिक्त)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	प्रत्यागम सन्देशः((वर्ण *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((वर्ण *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    प्रत्यागम सन्देशः((वर्ण *)IObuff);
}

    पूर्णाङ्क
smsg_attr(पूर्णाङ्क attr, ध्रुव वर्ण *s, ...)
{
    जइ (IObuff == रिक्त)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	प्रत्यागम सन्देशः_attr((वर्ण *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((वर्ण *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    प्रत्यागम सन्देशः_attr((वर्ण *)IObuff, attr);
}

    पूर्णाङ्क
smsg_attr_keep(पूर्णाङ्क attr, ध्रुव वर्ण *s, ...)
{
    जइ (IObuff == रिक्त)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	प्रत्यागम सन्देशः_attr_keep((वर्ण *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((वर्ण *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    प्रत्यागम सन्देशः_attr_keep((वर्ण *)IObuff, attr, TRUE);
}

#endif

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
स्थिर पूर्णाङ्क	last_sourcing_lnum = 0;
स्थिर char_u   *last_sourcing_name = रिक्त;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    शून्य
reset_last_sourcing(शून्य)
{
    VIM_CLEAR(last_sourcing_name);
    last_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    स्थिर पूर्णाङ्क
other_sourcing_name(शून्य)
{
    जइ (HAVE_SOURCING_INFO && SOURCING_NAME != रिक्त)
    {
	जइ (last_sourcing_name != रिक्त)
	    प्रत्यागम STRCMP(SOURCING_NAME, last_sourcing_name) != 0;
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    स्थिर char_u *
get_emsg_source(शून्य)
{
    char_u	*Buf, *p;

    जइ (HAVE_SOURCING_INFO && SOURCING_NAME != रिक्त && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	जइ (sname == रिक्त)
	    sname = SOURCING_NAME;

#ifdef FEAT_EVAL
	जइ (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	अण्णहा
#endif
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	जइ (Buf != रिक्त)
	    sprintf((वर्ण *)Buf, (वर्ण *)p, sname);
	vim_free(tofree);
	प्रत्यागम Buf;
    }
    प्रत्यागम रिक्त;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    स्थिर char_u *
get_emsg_lnum(शून्य)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    जइ (SOURCING_NAME != रिक्त
	    && (other_sourcing_name() || SOURCING_LNUM != last_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	जइ (Buf != रिक्त)
	    sprintf((वर्ण *)Buf, (वर्ण *)p, (long)SOURCING_LNUM);
	प्रत्यागम Buf;
    }
    प्रत्यागम रिक्त;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    शून्य
सन्देशः_source(पूर्णाङ्क attr)
{
    char_u	*p;
    स्थिर पूर्णाङ्क	recursive = FALSE;

    // Bail out if something called here causes an error.
    जइ (recursive)
	प्रत्यागम;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_source();
    जइ (p != रिक्त)
    {
	सन्देशः_scroll = TRUE;  // this will take more than one line
	सन्देशः_attr((वर्ण *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    जइ (p != रिक्त)
    {
	सन्देशः_attr((वर्ण *)p, HL_ATTR(HLF_N));
	vim_free(p);
	last_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    जइ (SOURCING_NAME == रिक्त || other_sourcing_name())
    {
	VIM_CLEAR(last_sourcing_name);
	जइ (SOURCING_NAME != रिक्त)
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
    स्थिर पूर्णाङ्क
emsg_not_now(शून्य)
{
    जइ ((emsg_off > 0 && vim_strchr(p_debug, 'm') == रिक्त
					  && vim_strchr(p_debug, 't') == रिक्त)
#ifdef FEAT_EVAL
	    || emsg_skip > 0
#endif
	    )
	प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

#जइ defined(FEAT_EVAL)
स्थिर garray_T ignore_error_list = GA_EMPTY;

    शून्य
ignore_error_for_testing(char_u *error)
{
    जइ (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, परिमाण(char_u *), 1);

    जइ (STRCMP("RESET", error) == 0)
	ga_clear_strings(&ignore_error_list);
    अण्णहा
	ga_copy_string(&ignore_error_list, error);
}

    स्थिर पूर्णाङ्क
ignore_error(ध्रुव वर्ण *सन्देशः)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < ignore_error_list.ga_len; ++i)
	जइ (strstr(सन्देशः,
		  (वर्ण *)((char_u **)(ignore_error_list.ga_data))[i]) != रिक्त)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}
#endif

#जइ !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    शून्य
do_perror(वर्ण *सन्देशः)
{
    perror(सन्देशः);
    ++emsg_silent;
    emsg(सन्देशः);
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
    स्थिर पूर्णाङ्क
emsg_core(ध्रुव वर्ण *s)
{
    पूर्णाङ्क		attr;
    char_u	*p;
    पूर्णाङ्क		r;
#ifdef FEAT_EVAL
    पूर्णाङ्क		ignore = FALSE;
    पूर्णाङ्क		severe;
#endif

#ifdef FEAT_EVAL
    // When testing some errors are turned into a normal message.
    जइ (ignore_error(s))
	// don't call msg() if it results in a dialog
	प्रत्यागम सन्देशः_use_printf() ? FALSE : सन्देशः((वर्ण *)s);
#endif

    ++called_emsg;

#ifdef FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#endif

    जइ (!emsg_off || vim_strchr(p_debug, 't') != रिक्त)
    {
#ifdef FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	जइ (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    जइ (!ignore)
		++did_emsg;
	    प्रत्यागम TRUE;
	}

	जइ (in_assert_fails && emsg_assert_fails_सन्देशः == रिक्त)
	{
	    emsg_assert_fails_सन्देशः = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == रिक्त ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#endif

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	जइ (emsg_silent != 0)
	{
#ifdef FEAT_EVAL
	    ++did_emsg_silent;
#endif
	    जइ (emsg_noredir == 0)
	    {
		सन्देशः_start();
		p = get_emsg_source();
		जइ (p != रिक्त)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		जइ (p != रिक्त)
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
	    जइ (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#endif
#ifdef FEAT_EVAL
	    ch_log(रिक्त, "ERROR silent: %s", s);
#endif
	    प्रत्यागम TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	सन्देशः_silent = 0;
	cmd_silent = FALSE;

	जइ (global_busy)		// break :global command
	    ++global_busy;

	जइ (p_eb)
	    beep_flush();		// also includes flush_buffers()
	अण्णहा
	    flush_buffers(FLUSH_MINIMAL);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ifdef FEAT_EVAL
	++uncaught_emsg;
#endif
    }

#ifdef HAS_MESSAGE_WINDOW
    जइ (!in_echowindow)
#endif
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    जइ (सन्देशः_scrolled != 0)
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
    सन्देशः_scroll = TRUE;
    सन्देशः_source(attr);

    /*
     * Display the error message itself.
     */
    सन्देशः_nowait = FALSE;			// wait for this msg
    r = सन्देशः_attr((वर्ण *)s, attr);

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = FALSE;
#endif
    प्रत्यागम r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    पूर्णाङ्क
emsg(वर्ण *s)
{
    // Skip this if not giving error messages at the moment.
    जइ (emsg_not_now())
	प्रत्यागम TRUE;

    प्रत्यागम emsg_core(s);
}

#ifndef PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    पूर्णाङ्क
semsg(ध्रुव वर्ण *s, ...)
{
    // Skip this if not giving error messages at the moment.
    जइ (emsg_not_now())
	प्रत्यागम TRUE;

    जइ (IObuff == रिक्त)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	प्रत्यागम emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((वर्ण *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    प्रत्यागम emsg_core((वर्ण *)IObuff);
}
#endif

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    शून्य
iemsg(वर्ण *s)
{
    जइ (emsg_not_now())
	प्रत्यागम;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    emsg_core(s);
#जइ defined(ABORT_ON_INTERNAL_ERROR) && defined(FEAT_EVAL)
    set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    सन्देशः_putchar('\n');  // avoid overwriting the error message
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
    शून्य
siemsg(ध्रुव वर्ण *s, ...)
{
    जइ (emsg_not_now())
	प्रत्यागम;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    जइ (IObuff == रिक्त)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    अण्णहा
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((वर्ण *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((वर्ण *)IObuff);
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    सन्देशः_putchar('\n');  // avoid overwriting the error message
    out_flush();
    abort();
# endif
}
#endif

/*
 * Give an "Internal error" message.
 */
    शून्य
internal_error(वर्ण *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    siemsg(_(e_internal_error_str), where);
}

#जइ defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    शून्य
internal_error_no_abort(वर्ण *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    semsg(_(e_internal_error_str), where);
}
#endif

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    शून्य
emsg_invreg(पूर्णाङ्क name)
{
    semsg(_(e_invalid_register_name_str), transchar_buf(रिक्त, name));
}

#जइ defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    शून्य
emsg_namelen(वर्ण *सन्देशः, char_u *name, पूर्णाङ्क len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(सन्देशः, copy == रिक्त ? "NULL" : (वर्ण *)copy);
    vim_free(copy);
}
#endif

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    वर्ण *
सन्देशः_trunc_attr(वर्ण *s, पूर्णाङ्क force, पूर्णाङ्क attr)
{
    पूर्णाङ्क		n;
    वर्ण	*ts;

    // Add message to history before truncating
    add_सन्देशः_hist((char_u *)s, -1, attr);

    ts = (वर्ण *)सन्देशः_may_trunc(force, (char_u *)s);

    सन्देशः_hist_off = TRUE;
    n = सन्देशः_attr(ts, attr);
    सन्देशः_hist_off = FALSE;

    जइ (n)
	प्रत्यागम ts;
    प्रत्यागम रिक्त;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
सन्देशः_may_trunc(पूर्णाङ्क force, char_u *s)
{
    पूर्णाङ्क		n;
    पूर्णाङ्क		room;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    room = (पूर्णाङ्क)(Rows - cmdline_row - 1) * cmdline_width + sc_col - 1;
    जइ (room > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_active))
	    && (n = (पूर्णाङ्क)STRLEN(s) - room) > 0)
    {
	जइ (has_mbyte)
	{
	    पूर्णाङ्क	size = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    जइ (size <= room)
		प्रत्यागम s;

	    प्रति (n = 0; size >= room; )
	    {
		size -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    प्रत्यागम s;
}

    स्थिर शून्य
add_सन्देशः_hist(
    char_u	*s,
    पूर्णाङ्क		len,		// -1 for undetermined length
    पूर्णाङ्क		attr)
{
    संरचना सन्देशः_hist *p;

    जइ (सन्देशः_hist_off || सन्देशः_silent != 0)
	प्रत्यागम;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(संरचना सन्देशः_hist);
    जइ (p == रिक्त)
	प्रत्यागम;

    जइ (len < 0)
	len = (पूर्णाङ्क)STRLEN(s);
    // remove leading and trailing newlines
    जाव (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    जाव (len > 0 && s[len - 1] == '\n')
	--len;
    p->सन्देशः = vim_strnsave(s, len);
    p->next = रिक्त;
    p->attr = attr;
    जइ (last_सन्देशः_hist != रिक्त)
	last_सन्देशः_hist->next = p;
    last_सन्देशः_hist = p;
    जइ (first_सन्देशः_hist == रिक्त)
	first_सन्देशः_hist = last_सन्देशः_hist;
    ++सन्देशः_hist_len;

    check_सन्देशः_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    पूर्णाङ्क
delete_first_सन्देशः(शून्य)
{
    संरचना सन्देशः_hist *p;

    जइ (सन्देशः_hist_len <= 0)
	प्रत्यागम FAIL;
    p = first_सन्देशः_hist;
    first_सन्देशः_hist = p->next;
    जइ (first_सन्देशः_hist == रिक्त)
	last_सन्देशः_hist = रिक्त;  // history is empty
    vim_free(p->सन्देशः);
    vim_free(p);
    --सन्देशः_hist_len;
    प्रत्यागम OK;
}

    स्थिर शून्य
check_सन्देशः_hist(शून्य)
{
    // Don't let the message history get too big
    जाव (सन्देशः_hist_len > 0 && सन्देशः_hist_len > सन्देशः_hist_max)
	(शून्य)delete_first_सन्देशः();
}


    पूर्णाङ्क
messagesopt_changed(शून्य)
{
    char_u	*p;
    पूर्णाङ्क		messages_flags_new = 0;
    पूर्णाङ्क		messages_wait_new = 0;
    पूर्णाङ्क		messages_history_new = 0;

    p = p_mopt;
    जाव (*p != NUL)
    {
	जइ (STRNCMP(p, MESSAGES_OPT_HIT_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER);
	    messages_flags_new |= MESSAGES_HIT_ENTER;
	}
	अण्णहा जइ (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	अण्णहा जइ (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	जइ (*p != ',' && *p != NUL)
	    प्रत्यागम FAIL;
	जइ (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    जइ (!(messages_flags_new & (MESSAGES_HIT_ENTER | MESSAGES_WAIT)))
	प्रत्यागम FAIL;

    // "history" must be set
    जइ (!(messages_flags_new & MESSAGES_HISTORY))
	प्रत्यागम FAIL;

    जइ (messages_history_new < 0 || messages_history_new > 10000)
	प्रत्यागम FAIL;

    जइ (messages_wait_new < 0 || messages_wait_new > 10000)
	प्रत्यागम FAIL;

    सन्देशः_flags = messages_flags_new;
    सन्देशः_wait = messages_wait_new;

    सन्देशः_hist_max = messages_history_new;
    check_सन्देशः_hist();

    प्रत्यागम OK;
}

/*
 * ":messages" command.
 */
    शून्य
ex_messages(exarg_T *eap)
{
    संरचना सन्देशः_hist *p;
    char_u	    *s;
    पूर्णाङ्क		    c = 0;

    जइ (STRCMP(eap->arg, "clear") == 0)
    {
	पूर्णाङ्क keep = eap->addr_गणनम् == 0 ? 0 : eap->line2;

	जाव (सन्देशः_hist_len > keep)
	    (शून्य)delete_first_सन्देशः();
	प्रत्यागम;
    }

    जइ (*eap->arg != NUL)
    {
	emsg(_(e_invalid_argument));
	प्रत्यागम;
    }

    सन्देशः_hist_off = TRUE;

    p = first_सन्देशः_hist;
    जइ (eap->addr_गणनम् != 0)
    {
	// Count total messages
	प्रति (; p != रिक्त && !got_int; p = p->next)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	प्रति (p = first_सन्देशः_hist; p != रिक्त && !got_int && c > 0;
						    p = p->next, c--);
    }

    जइ (p == first_सन्देशः_hist)
    {
#ifdef FEAT_MULTI_LANG
	s = get_mess_lang();
#अण्णहा
	s = mch_getenv((char_u *)"LANG");
#endif
	जइ (s != रिक्त && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    सन्देशः_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    प्रति (; p != रिक्त && !got_int; p = p->next)
	जइ (p->सन्देशः != रिक्त)
	    सन्देशः_attr((वर्ण *)p->सन्देशः, p->attr);

    सन्देशः_hist_off = FALSE;
}

#जइ defined(FEAT_CON_DIALOG) || defined(FIND_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    शून्य
सन्देशः_end_prompt(शून्य)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_row = सन्देशः_row;
    सन्देशः_col = 0;
    सन्देशः_clr_eos();
    पङ्क्तयः_वामः = -1;
}
#endif

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    शून्य
wait_return(पूर्णाङ्क redraw)
{
    पूर्णाङ्क		c;
    पूर्णाङ्क		oldState;
    पूर्णाङ्क		tmpState;
    पूर्णाङ्क		had_got_int;
    पूर्णाङ्क		save_reg_recording;
    FILE	*save_scriptout;

    जइ (redraw == TRUE)
	set_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    जइ (सन्देशः_silent != 0)
	प्रत्यागम;
#ifdef HAS_MESSAGE_WINDOW
    जइ (in_echowindow)
	प्रत्यागम;
#endif

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    जइ (vgetc_busy > 0)
	प्रत्यागम;
    need_wait_return = TRUE;
    जइ (no_wait_return)
    {
	जइ (!exmode_active)
	    cmdline_row = सन्देशः_row;
	प्रत्यागम;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    जइ (quit_more)
    {
	c = CAR;		// just pretend CR was hit
	quit_more = FALSE;
	got_int = FALSE;
    }
    अण्णहा जइ (exmode_active)
    {
	सन्देशः_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    अण्णहा जइ (!stuff_empty())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    अण्णहा
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = MODE_HITRETURN;
	setmouse();
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#endif
	cmdline_row = सन्देशः_row;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	जइ (need_check_timestamps)
	    check_timestamps(FALSE);

	जइ (सन्देशः_flags & MESSAGES_HIT_ENTER)
	{
	    hit_return_सन्देशः();

	    करेहि
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
		++allow_कुञ्चिकाः;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = रिक्त;
		c = safe_vgetc();
		जइ (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_कुञ्चिकाः;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ifdef FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		जइ (कुञ्चिका_Typed && c == Ctrl_Y && clip_star.state == SELECT_DONE)
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
		जइ (कुञ्चिका_Typed && p_more && !p_cp)
		{
		    जइ (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_UP || c == K_PAGEUP)
		    {
			जइ (सन्देशः_scrolled > Rows)
			    // scroll back to show older messages
			    do_more_prompt(c);
			अण्णहा
			{
			    सन्देशः_didout = FALSE;
			    c = K_IGNORE;
			    सन्देशः_col =
#ifdef FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_width - 1 :
#endif
				0;
			}
			जइ (quit_more)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_more = FALSE;
			    got_int = FALSE;
			}
			अण्णहा जइ (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    hit_return_सन्देशः();
			}
		    }
		    अण्णहा जइ (सन्देशः_scrolled > Rows - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_DOWN || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } जाव ((had_got_int && c == Ctrl_C)
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
				    && mouse_row < सन्देशः_row
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    जइ (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(शून्य)jump_to_mouse(MOUSE_SETPOS, रिक्त, 0);
	    अण्णहा जइ (!कुञ्चिका_Typed || (vim_strchr((char_u *)"\r\n ", c) == रिक्त
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_mask);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	अण्णहा
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(सन्देशः_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    जइ (c == ':' || c == '?' || c == '/')
    {
	जइ (!exmode_active)
	    cmdline_row = सन्देशः_row;
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
    सन्देशः_check();

#ifndef MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    जइ (swapping_screen() && !termcap_active)
	newline_on_exit = TRUE;
#endif

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    पङ्क्तयः_वामः = -1;		// reset lines_left at next msg_start()
    reset_last_sourcing();
    जइ (keep_सन्देशः != रिक्त && vim_strsize(keep_सन्देशः) >=
			    (Rows - cmdline_row - 1) * cmdline_width + sc_col)
	VIM_CLEAR(keep_सन्देशः);	    // don't redisplay message, it's too long

    जइ (tmpState == MODE_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    अण्णहा जइ (!skip_redraw
	    && (redraw == TRUE || (सन्देशः_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    स्थिर शून्य
hit_return_सन्देशः(शून्य)
{
    पूर्णाङ्क		save_p_more = p_more;

    p_more = FALSE;	// don't want to see this message when scrolling back
    जइ (सन्देशः_didout)	// start on a new line
	सन्देशः_putchar('\n');
    जइ (got_int)
	सन्देशः_puts(_("Interrupt: "));

    सन्देशः_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    जइ (!सन्देशः_use_printf())
	सन्देशः_clr_eos();
    p_more = save_p_more;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    शून्य
set_keep_सन्देशः(char_u *s, पूर्णाङ्क attr)
{
    vim_free(keep_सन्देशः);
    जइ (s != रिक्त && सन्देशः_silent == 0)
	keep_सन्देशः = vim_strsave(s);
    अण्णहा
	keep_सन्देशः = रिक्त;
    keep_सन्देशः_more = FALSE;
    keep_सन्देशः_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    शून्य
set_keep_सन्देशः_from_hist(शून्य)
{
    जइ (keep_सन्देशः == रिक्त && last_सन्देशः_hist != रिक्त && सन्देशः_scrolled == 0
						      && (State & MODE_NORMAL))
	set_keep_सन्देशः(last_सन्देशः_hist->सन्देशः, last_सन्देशः_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    शून्य
सन्देशः_start(शून्य)
{
    पूर्णाङ्क		did_return = FALSE;

    जइ (सन्देशः_row < cmdline_row)
	सन्देशः_row = cmdline_row;

    जइ (!सन्देशः_silent)
    {
	VIM_CLEAR(keep_सन्देशः);
	need_fileinfo = FALSE;
    }

#ifdef FEAT_EVAL
    जइ (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	सन्देशः_clr_eos();
    }
#endif

#ifdef HAS_MESSAGE_WINDOW
    जइ (in_echowindow)
    {
	जइ (popup_message_win_visible()
		    && ((सन्देशः_col > 0 && (सन्देशः_scroll || !full_screen))
			|| in_echowindow))
	{
	    win_T *wp = popup_get_message_win();

	    // start a new line
	    curbuf = wp->w_buffer;
	    ml_append(wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_buffer;
	}
	सन्देशः_col = 0;
    }
    अण्णहा
#endif
	जइ (!सन्देशः_scroll && full_screen)	// overwrite last message
    {
	सन्देशः_row = cmdline_row;
	सन्देशः_col =
#ifdef FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_width - 1 :
#endif
	    0;
    }
    अण्णहा जइ (सन्देशः_didout || in_echowindow)
    {
	// start message on next line
	सन्देशः_putchar('\n');
	did_return = TRUE;
	जइ (exmode_active != EXMODE_NORMAL)
	    cmdline_row = सन्देशः_row;
    }
    जइ (!सन्देशः_didany || पङ्क्तयः_वामः < 0)
	सन्देशः_starthere();
    जइ (सन्देशः_silent == 0)
    {
	सन्देशः_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    जइ (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    शून्य
सन्देशः_starthere(शून्य)
{
    पङ्क्तयः_वामः = cmdline_row;
    सन्देशः_didany = FALSE;
}

    शून्य
सन्देशः_putchar(पूर्णाङ्क c)
{
    सन्देशः_putchar_attr(c, 0);
}

    शून्य
सन्देशः_putchar_attr(पूर्णाङ्क c, पूर्णाङ्क attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    जइ (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    अण्णहा
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    सन्देशः_puts_attr((वर्ण *)buf, attr);
}

    शून्य
सन्देशः_outnum(long n)
{
    वर्ण	buf[20];

    sprintf(buf, "%ld", n);
    सन्देशः_puts(buf);
}

    शून्य
सन्देशः_home_replace(char_u *fname)
{
    सन्देशः_home_replace_attr(fname, 0);
}

#जइ defined(FEAT_FIND_ID)
    शून्य
सन्देशः_home_replace_hl(char_u *fname)
{
    सन्देशः_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#endif

    स्थिर शून्य
सन्देशः_home_replace_attr(char_u *fname, पूर्णाङ्क attr)
{
    char_u	*name;

    name = home_replace_save(रिक्त, fname);
    जइ (name != रिक्त)
	सन्देशः_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    पूर्णाङ्क
सन्देशः_outtrans(char_u *str)
{
    प्रत्यागम सन्देशः_outtrans_attr(str, 0);
}

    पूर्णाङ्क
सन्देशः_outtrans_attr(char_u *str, पूर्णाङ्क attr)
{
    प्रत्यागम सन्देशः_outtrans_len_attr(str, (पूर्णाङ्क)STRLEN(str), attr);
}

    पूर्णाङ्क
सन्देशः_outtrans_len(char_u *str, पूर्णाङ्क len)
{
    प्रत्यागम सन्देशः_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
सन्देशः_outtrans_one(char_u *p, पूर्णाङ्क attr)
{
    पूर्णाङ्क		l;

    जइ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	सन्देशः_outtrans_len_attr(p, l, attr);
	प्रत्यागम p + l;
    }
    सन्देशः_puts_attr((वर्ण *)transchar_byte_buf(रिक्त, *p), attr);
    प्रत्यागम p + 1;
}

    पूर्णाङ्क
सन्देशः_outtrans_len_attr(char_u *msgstr, पूर्णाङ्क len, पूर्णाङ्क attr)
{
    पूर्णाङ्क		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_start = msgstr;
    char_u	*s;
    पूर्णाङ्क		mb_l;
    पूर्णाङ्क		c;
    पूर्णाङ्क		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    जइ (attr == 0)
	attr = HL_ATTR(HLF_सन्देशः);

    // if MSG_HIST flag set, add message to history
    जइ (attr & सन्देशः_HIST)
    {
	add_सन्देशः_hist(str, len, attr);
	attr &= ~सन्देशः_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    जइ (सन्देशः_silent == 0 && len > 0 && सन्देशः_row >= cmdline_row && सन्देशः_col == 0)
    {
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    जइ (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	सन्देशः_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    जाव (--len >= 0 && !got_int)
    {
	जइ (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	अण्णहा जइ (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	अण्णहा
	    mb_l = 1;
	जइ (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    जइ (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    अण्णहा
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		जइ (str > plain_start)
		    सन्देशः_puts_attr_len((वर्ण *)plain_start,
					       (पूर्णाङ्क)(str - plain_start), attr);
		plain_start = str + mb_l;
		सन्देशः_puts_attr((वर्ण *)transchar_buf(रिक्त, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	अण्णहा
	{
	    s = transchar_byte_buf(रिक्त, *str);
	    जइ (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		जइ (str > plain_start)
		    सन्देशः_puts_attr_len((वर्ण *)plain_start,
					       (पूर्णाङ्क)(str - plain_start), attr);
		plain_start = str + 1;
		सन्देशः_puts_attr((वर्ण *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (पूर्णाङ्क)STRLEN(s);
	    }
	    अण्णहा
		++retval;
	    ++str;
	}
    }

    जइ (str > plain_start && !got_int)
	// print the printable chars at the end
	सन्देशः_puts_attr_len((वर्ण *)plain_start, (पूर्णाङ्क)(str - plain_start), attr);

    got_int |= save_got_int;

    प्रत्यागम retval;
}

#जइ defined(FEAT_QUICKFIX)
    शून्य
सन्देशः_make(char_u *arg)
{
    पूर्णाङ्क	    i;
    स्थिर char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    प्रति (i = 5; *arg && i >= 0; --i)
	जइ (*arg++ != str[i])
	    भङ्ग;
    जइ (i < 0)
    {
	सन्देशः_putchar('\n');
	प्रति (i = 0; rs[i]; ++i)
	    सन्देशः_putchar(rs[i] - 3);
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
    पूर्णाङ्क
सन्देशः_outtrans_special(
    char_u	*strstart,
    पूर्णाङ्क		from,	// TRUE for lhs of a mapping
    पूर्णाङ्क		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    पूर्णाङ्क		retval = 0;
    वर्ण	*text;
    पूर्णाङ्क		attr;
    पूर्णाङ्क		len;

    attr = HL_ATTR(HLF_8);
    जाव (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	जइ ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    text = "<Space>";
	    ++str;
	}
	अण्णहा
	    text = (वर्ण *)str2special(&str, from, FALSE);
	जइ (text[0] != NUL && text[1] == NUL)
	    // single-byte character or illegal byte
	    text = (वर्ण *)transchar_byte_buf(रिक्त, (char_u)text[0]);
	len = vim_strsize((char_u *)text);
	जइ (maxlen > 0 && retval + len >= maxlen)
	    भङ्ग;
	// Highlight special keys
	सन्देशः_puts_attr(text, len > 1
		&& (*mb_ptr2len)((char_u *)text) <= 1 ? attr : 0);
	retval += len;
    }
    प्रत्यागम retval;
}

#जइ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    पूर्णाङ्क	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    पूर्णाङ्क	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    जाव (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    प्रत्यागम (char_u *)ga.ga_data;
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
    पूर्णाङ्क		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    पूर्णाङ्क		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    पूर्णाङ्क			c;
    स्थिर char_u	buf[7];
    char_u		*str = *sp;
    पूर्णाङ्क			modifiers = 0;
    पूर्णाङ्क			special = FALSE;

    जइ (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	जइ (p != रिक्त)
	    प्रत्यागम p;
    }

    c = *str;
    जइ ((c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	) && str[1] != NUL && str[2] != NUL)
    {
	जइ (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	जइ ((c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = TO_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	जइ (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    जइ (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	जइ (p != रिक्त)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	अण्णहा
	    // illegal byte
	    *sp = str + 1;
    }
    अण्णहा
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    जइ (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	प्रत्यागम get_special_कुञ्चिका_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    प्रत्यागम buf;
}

/*
 * Translate a key sequence into special key names.
 */
    शून्य
str2specialbuf(char_u *sp, char_u *buf, पूर्णाङ्क len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    जाव (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	जइ (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	अण्णहा
	    भङ्ग;
    }
}

/*
 * print line for :print or :list command
 */
    शून्य
सन्देशः_prt_पङ्क्तिः(char_u *s, पूर्णाङ्क list)
{
    पूर्णाङ्क		c;
    पूर्णाङ्क		col = 0;
    पूर्णाङ्क		n_extra = 0;
    पूर्णाङ्क		c_extra = 0;
    पूर्णाङ्क		c_final = 0;
    char_u	*p_extra = रिक्त;	    // init to make SASC shut up
    पूर्णाङ्क		n;
    पूर्णाङ्क		attr = 0;
    char_u	*trail = रिक्त;
    char_u	*lead = रिक्त;
    पूर्णाङ्क		in_multispace = FALSE;
    पूर्णाङ्क		multispace_pos = 0;
    पूर्णाङ्क		l;
    char_u	buf[MB_MAXBYTES + 1];

    जइ (curwin->w_p_list)
	list = TRUE;

    जइ (list)
    {
	// find start of trailing whitespace
	जइ (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    जाव (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	जइ (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != रिक्त
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    जाव (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    जइ (*lead == NUL)
		lead = रिक्त;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    जइ (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	सन्देशः_putchar(' ');

    जाव (!got_int)
    {
	जइ (n_extra > 0)
	{
	    --n_extra;
	    जइ (n_extra == 0 && c_final)
		c = c_final;
	    अण्णहा जइ (c_extra)
		c = c_extra;
	    अण्णहा
		c = *p_extra++;
	}
	अण्णहा जइ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    col += (*mb_ptr2cells)(s);
	    जइ (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    अण्णहा जइ (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		पूर्णाङ्क len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    अण्णहा
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    सन्देशः_puts((वर्ण *)buf);
	    s += l;
	    अनुवर्त;
	}
	अण्णहा
	{
	    attr = 0;
	    c = *s++;
	    जइ (list)
	    {
		in_multispace = c == ' ' && (*s == ' '
						 || (col > 0 && s[-2] == ' '));
		जइ (!in_multispace)
		    multispace_pos = 0;
	    }
	    जइ (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ifdef FEAT_VARTABS
		n_extra = tabstop_padding(col, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#अण्णहा
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
#endif
		जइ (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		अण्णहा
		{
		    पूर्णाङ्क lcs_tab1 = curwin->w_lcs_chars.tab1;
		    पूर्णाङ्क lcs_tab2 = curwin->w_lcs_chars.tab2;
		    पूर्णाङ्क lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    जइ (lead != रिक्त && s <= lead
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
	    अण्णहा जइ (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    अण्णहा जइ (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    अण्णहा जइ (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(रिक्त, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    अण्णहा जइ (c == ' ')
	    {
		जइ (lead != रिक्त && s <= lead && in_multispace
			&& curwin->w_lcs_chars.leadmultispace != रिक्त)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    जइ (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		अण्णहा जइ (lead != रिक्त && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		अण्णहा जइ (trail != रिक्त && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		अण्णहा जइ (in_multispace
			&& curwin->w_lcs_chars.multispace != रिक्त)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    जइ (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		अण्णहा जइ (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	जइ (c == NUL)
	    भङ्ग;

	सन्देशः_putchar_attr(c, attr);
	col++;
    }
    सन्देशः_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    स्थिर char_u *
screen_puts_mbyte(char_u *s, पूर्णाङ्क l, पूर्णाङ्क attr)
{
    पूर्णाङ्क		cw;

    सन्देशः_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    जइ (cw > 1 && (
#ifdef FEAT_RIGHTLEFT
		cmdmsg_rl ? सन्देशः_col <= 1 :
#endif
		सन्देशः_col == cmdline_width - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	सन्देशः_screen_putchar('>', HL_ATTR(HLF_AT));
	प्रत्यागम s;
    }

    screen_puts_len(s, l, सन्देशः_row, cmdline_col_off + सन्देशः_col, attr);
#ifdef FEAT_RIGHTLEFT
    जइ (cmdmsg_rl)
    {
	सन्देशः_col -= cw;
	जइ (सन्देशः_col == 0)
	{
	    सन्देशः_col = cmdline_width;
	    ++सन्देशः_row;
	}
    }
    अण्णहा
#endif
    {
	सन्देशः_col += cw;
	जइ (सन्देशः_col >= cmdline_width)
	{
	    सन्देशः_col = 0;
	    ++सन्देशः_row;
	}
    }
    प्रत्यागम s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    शून्य
सन्देशः_puts(वर्ण *s)
{
    सन्देशः_puts_attr(s, 0);
}

    शून्य
सन्देशः_puts_title(वर्ण *s)
{
    सन्देशः_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    स्थिर शून्य
सन्देशः_outtrans_long_len_attr(char_u *longstr, पूर्णाङ्क len, पूर्णाङ्क attr)
{
    पूर्णाङ्क		slen = len;
    पूर्णाङ्क		room;

    room = cmdline_width - सन्देशः_col;
    जइ (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	सन्देशः_outtrans_len_attr(longstr, slen, attr);
	सन्देशः_puts_attr("...", HL_ATTR(HLF_8));
    }
    सन्देशः_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    शून्य
सन्देशः_outtrans_long_attr(char_u *longstr, पूर्णाङ्क attr)
{
    सन्देशः_outtrans_long_len_attr(longstr, (पूर्णाङ्क)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    शून्य
सन्देशः_puts_attr(वर्ण *s, पूर्णाङ्क attr)
{
    सन्देशः_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    स्थिर शून्य
सन्देशः_puts_attr_len(वर्ण *str, पूर्णाङ्क maxlen, पूर्णाङ्क attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    जइ (सन्देशः_silent != 0)
	प्रत्यागम;

    जइ (attr == 0)
	attr = HL_ATTR(HLF_सन्देशः);

    // if MSG_HIST flag set, add message to history
    जइ ((attr & सन्देशः_HIST) && maxlen < 0)
    {
	add_सन्देशः_hist((char_u *)str, -1, attr);
	attr &= ~सन्देशः_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    जइ (सन्देशः_scrolled != 0 && !सन्देशः_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    सन्देशः_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    जइ (सन्देशः_use_printf())
	सन्देशः_puts_printf((char_u *)str, maxlen);
    अण्णहा
	सन्देशः_puts_display((char_u *)str, maxlen, attr, FALSE);

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
    स्थिर शून्य
put_सन्देशः_win(win_T *wp, पूर्णाङ्क where, char_u *t_s, char_u *end, linenr_T lnum)
{
    char_u  *p;

    जइ (where == PUT_BELOW)
    {
	जइ (*end != NUL)
	{
	    p = vim_strnsave(t_s, end - t_s);
	    जइ (p == रिक्त)
		प्रत्यागम;
	}
	अण्णहा
	    p = t_s;
	ml_append_buf(wp->w_buffer, lnum, p, (colnr_T)0, FALSE);
	जइ (p != t_s)
	    vim_free(p);
    }
    अण्णहा
    {
	char_u *newp;

	curbuf = wp->w_buffer;
	जइ (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    जइ (newp == रिक्त)
		प्रत्यागम;
	    जइ (*end != NUL)
		newp[STRLEN(ml_get(lnum)) + (end - t_s)] = NUL;
	}
	अण्णहा
	{
	    newp = vim_strnsave(t_s, end - t_s);
	    जइ (newp == रिक्त)
		प्रत्यागम;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_buffer;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    सन्देशः_col += (पूर्णाङ्क)(end - t_s);
}
#endif

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    स्थिर शून्य
सन्देशः_puts_display(
    char_u	*str,
    पूर्णाङ्क		maxlen,
    पूर्णाङ्क		attr,
    पूर्णाङ्क		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    पूर्णाङ्क		t_col = 0;	// screen cells todo, 0 when "t_s" not used
    पूर्णाङ्क		l;
    पूर्णाङ्क		cw;
    char_u	*sb_str = str;
    पूर्णाङ्क		sb_col = सन्देशः_col;
    पूर्णाङ्क		wrap;
    पूर्णाङ्क		did_last_char;
#ifdef HAS_MESSAGE_WINDOW
    पूर्णाङ्क		where = PUT_APPEND;
    win_T	*सन्देशः_win = रिक्त;
    linenr_T    lnum = 1;

    जइ (in_echowindow)
    {
	सन्देशः_win = popup_get_message_win();

	जइ (सन्देशः_win != रिक्त)
	{
	    जइ (!popup_message_win_visible())
	    {
		जइ (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    सन्देशः_win = रिक्त;
		}
		अण्णहा
		{
		    // currently hidden, make it empty
		    curbuf = सन्देशः_win->w_buffer;
		    जाव ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
			ml_delete(1);
		    curbuf = curwin->w_buffer;
		}
	    }
	    अण्णहा
	    {
		lnum = सन्देशः_win->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;
		जइ (सन्देशः_col == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#endif

    did_wait_return = FALSE;
    जाव ((maxlen < 0 || (पूर्णाङ्क)(s - str) < maxlen) && *s != NUL)
    {
#ifdef HAS_MESSAGE_WINDOW
	// For echowindow, use full width; for regular messages, leave last column
	पूर्णाङ्क wrap_col = सन्देशः_win != रिक्त ? cmdline_width : cmdline_width - 1;
#अण्णहा
	पूर्णाङ्क wrap_col = cmdline_width - 1;
#endif

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	जइ (!recurse && सन्देशः_row >= Rows - 1 && (*s == '\n' || (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			सन्देशः_col <= 1
		      || (*s == TAB && सन्देशः_col <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && सन्देशः_col <= 2))
		    :
#endif
		      ((*s != '\r' && सन्देशः_col + t_col >= wrap_col)
		       || (*s == TAB && सन्देशः_col + t_col
			   >= (wrap_col & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && सन्देशः_col + t_col >= wrap_col - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    जइ (t_col > 0)
	    {
		// output postponed text
#ifdef HAS_MESSAGE_WINDOW
		जइ (सन्देशः_win != रिक्त)
		{
		    put_सन्देशः_win(सन्देशः_win, where, t_s, s, lnum);
		    जइ (where == PUT_BELOW)
			++lnum;
		    t_col = 0;
		    where = PUT_BELOW;
		}
		अण्णहा
#endif
		    t_puts(&t_col, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    जइ (सन्देशः_no_more && पङ्क्तयः_वामः == 0)
		भङ्ग;

#ifdef HAS_MESSAGE_WINDOW
	    जइ (सन्देशः_win == रिक्त)
#endif
		// Scroll the screen up one line.
		सन्देशः_scroll_up();

#ifdef HAS_MESSAGE_WINDOW
	    जइ (सन्देशः_win == रिक्त)
	    {
#endif
		सन्देशः_row = Rows - 2;
		जइ (सन्देशः_col >= cmdline_width)  // can happen after screen resize
		    सन्देशः_col = cmdline_width - 1;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif

	    // Display char in last column before showing more-prompt.
	    जइ (*s >= ' '
#ifdef FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#endif
	       )
	    {
		जइ (has_mbyte)
		{
		    जइ (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (पूर्णाङ्क)((str + maxlen) - s));
		    अण्णहा
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		अण्णहा
		    सन्देशः_screen_putchar(*s++, attr);
		did_last_char = TRUE;
	    }
	    अण्णहा
		did_last_char = FALSE;

	    जइ (p_more)
		// store text for scrolling back
		store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

#ifdef HAS_MESSAGE_WINDOW
	    जइ (सन्देशः_win == रिक्त)
	    {
#endif
		inc_सन्देशः_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		जइ (cmdline_row > 0 && !exmode_active)
		    --cmdline_row;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		जइ (पङ्क्तयः_वामः > 0)
		    --पङ्क्तयः_वामः;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif
	    जइ (p_more && पङ्क्तयः_वामः == 0 && State != MODE_HITRETURN
					    && !सन्देशः_no_more && !exmode_active)
	    {
#ifdef FEAT_CON_DIALOG
		जइ (do_more_prompt(NUL))
		    s = confirm_सन्देशः_tail;
#अण्णहा
		(शून्य)do_more_prompt(NUL);
#endif
		जइ (quit_more)
		    प्रत्यागम;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    जइ (did_last_char)
		अनुवर्त;
	}

	wrap = *s == '\n'
		    || सन्देशः_col + t_col >= wrap_col
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && सन्देशः_col + t_col >= wrap_col - 1);
	जइ (t_col > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ifdef HAS_MESSAGE_WINDOW
	    जइ (सन्देशः_win != रिक्त)
	    {
		put_सन्देशः_win(सन्देशः_win, where, t_s, s, lnum);
		जइ (where == PUT_BELOW)
		    ++lnum;
		t_col = 0;
		where = PUT_BELOW;
		// Reset msg_col after outputting to new line in echowindow
		जइ (wrap)
		    सन्देशः_col = 0;
	    }
	    अण्णहा
#endif
		t_puts(&t_col, t_s, s, attr);
	}

	जइ (wrap && p_more && !recurse)
	    // store text for scrolling back
	    store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	जइ (*s == '\n')		    // go to next line
	{
#ifdef HAS_MESSAGE_WINDOW
	    जइ (सन्देशः_win != रिक्त)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		जइ ((सन्देशः_win->w_buffer->b_ml.ml_flags & ML_EMPTY) == 0)
		{
		    put_सन्देशः_win(सन्देशः_win, PUT_BELOW, t_s, t_s, lnum);
		    ++lnum;
		}
		सन्देशः_col = 0;  // Reset column for new line
	    }
	    अण्णहा
#endif
		सन्देशः_didout = FALSE;	    // remember that line is empty
#ifdef FEAT_RIGHTLEFT
	    जइ (cmdmsg_rl)
		सन्देशः_col = cmdline_width - 1;
	    अण्णहा
#endif
		सन्देशः_col = 0;
	    जइ (++सन्देशः_row >= Rows)  // safety check
		सन्देशः_row = Rows - 1;
	}
	अण्णहा जइ (*s == '\r')	    // go to column 0
	{
	    सन्देशः_col = 0;
#ifdef HAS_MESSAGE_WINDOW
	    where = PUT_TRUNC;
#endif
	}
	अण्णहा जइ (*s == '\b')	    // go to previous char
	{
	    जइ (सन्देशः_col)
		--सन्देशः_col;
	}
	अण्णहा जइ (*s == TAB)	    // translate Tab into spaces
	{
#ifdef HAS_MESSAGE_WINDOW
	    जइ (सन्देशः_win != रिक्त)
		सन्देशः_col = (सन्देशः_col + 7) % 8;
	    अण्णहा
#endif
		करेहि
		    सन्देशः_screen_putchar(' ', attr);
		जाव (सन्देशः_col & 7);
	}
	अण्णहा जइ (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	अण्णहा
	{
	    जइ (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		जइ (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (पूर्णाङ्क)((str + maxlen) - s));
		अण्णहा
		    l = (*mb_ptr2len)(s);
	    }
	    अण्णहा
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    जइ (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#endif
		    (cw > 1 && सन्देशः_col + t_col >= wrap_col))
	    {
		जइ (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		अण्णहा
		    सन्देशः_screen_putchar(*s, attr);
	    }
	    अण्णहा
	    {
		// postpone this character until later
		जइ (t_col == 0)
		    t_s = s;
		t_col += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    जइ (t_col > 0)
    {
#ifdef HAS_MESSAGE_WINDOW
	जइ (सन्देशः_win != रिक्त)
	    put_सन्देशः_win(सन्देशः_win, where, t_s, s, lnum);
	अण्णहा
#endif
	    t_puts(&t_col, t_s, s, attr);
    }

#ifdef HAS_MESSAGE_WINDOW
    जइ (सन्देशः_win != रिक्त)
	popup_show_message_win();
#endif
    // Store the text for scroll back.
    जइ (p_more && !recurse)
	store_sb_text(&sb_str, s, attr, &sb_col, FALSE);

    सन्देशः_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    पूर्णाङ्क
message_filtered(char_u *सन्देशः)
{
    पूर्णाङ्क match;

    जइ (cmdmod.cmod_filter_regmatch.regprog == रिक्त)
	प्रत्यागम FALSE;
    match = vim_regexec(&cmdmod.cmod_filter_regmatch, सन्देशः, (colnr_T)0);
    प्रत्यागम cmdmod.cmod_filter_force ? match : !match;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    स्थिर शून्य
सन्देशः_scroll_up(शून्य)
{
#ifdef HAS_MESSAGE_WINDOW
    जइ (in_echowindow)
	प्रत्यागम;
#endif
#ifdef FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    जइ (gui.in_use)
	gui_undraw_cursor();
#endif
    // scrolling up always works
    mch_disable_flush();
    screen_del_पङ्क्तयः(0, 0, 1, (पूर्णाङ्क)Rows, TRUE, 0, रिक्त);
    mch_enable_flush();

    जइ (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((पूर्णाङ्क)Rows - 1, (पूर्णाङ्क)Rows, cmdline_col_off,
		cmdline_col_off + cmdline_width, ' ', ' ', HL_ATTR(HLF_सन्देशः));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	जइ (ScreenAttrs[पङ्क्तिः_Offset[Rows - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((पूर्णाङ्क)Rows - 2, (पूर्णाङ्क)Rows - 1,
				 (पूर्णाङ्क)Columns - 1, (पूर्णाङ्क)Columns,
				 ' ', ' ', HL_ATTR(HLF_सन्देशः));
    }
}

/*
 * Increment "msg_scrolled".
 */
    स्थिर शून्य
inc_सन्देशः_scrolled(शून्य)
{
#ifdef FEAT_EVAL
    जइ (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = रिक्त;

	// v:scrollstart is empty, set it to the script/function name and line
	//सङ्ख्या
	जइ (p.string == रिक्त)
	{
	    p.string = (char_u *)_("Unknown");
	    p.length = STRLEN(p.string);
	}
	अण्णहा
	{
	    size_t  tofreesize;

	    tofreesize = (पूर्णाङ्क)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    जइ (tofree != रिक्त)
	    {
		p.length = vim_snprintf_safelen((वर्ण *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (long)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	set_vim_var_string(VV_SCROLLSTART, p.string, (पूर्णाङ्क)p.length);
	vim_free(tofree);
    }
#endif
    ++सन्देशः_scrolled;
    set_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
प्रकारनिर्देश संरचना msgchunk_S msgchunk_T;
संरचना msgchunk_S
{
    msgchunk_T	*sb_next;
    msgchunk_T	*sb_prev;
    वर्ण	sb_eol;		// TRUE when line ends after this text
    पूर्णाङ्क		sb_सन्देशः_col;	// column in which text starts
    पूर्णाङ्क		sb_attr;	// text attributes
    char_u	sb_text[1];	// text to be displayed, actually longer
};

स्थिर msgchunk_T *last_msgchunk = रिक्त; // last displayed text

स्थिर msgchunk_T *सन्देशः_sb_start(msgchunk_T *mps);

प्रकारनिर्देश गणना {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_DONE
} sb_clear_T;

// When to clear text on next msg.
स्थिर sb_clear_T do_clear_sb_text = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    स्थिर शून्य
store_sb_text(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    पूर्णाङ्क		attr,
    पूर्णाङ्क		*sb_col,
    पूर्णाङ्क		finish)		// line ends
{
    msgchunk_T	*mp;

    जइ (do_clear_sb_text == SB_CLEAR_ALL
	    || do_clear_sb_text == SB_CLEAR_CMDLINE_DONE)
    {
	clear_sb_text(do_clear_sb_text == SB_CLEAR_ALL);
	सन्देशः_sb_eol();  // prevent messages from overlapping
	do_clear_sb_text = SB_CLEAR_NONE;
    }

    जइ (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_text) + (s - *sb_str) + 1);
	जइ (mp != रिक्त)
	{
	    mp->sb_eol = finish;
	    mp->sb_सन्देशः_col = *sb_col;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_text, *sb_str, s - *sb_str);

	    जइ (last_msgchunk == रिक्त)
	    {
		last_msgchunk = mp;
		mp->sb_prev = रिक्त;
	    }
	    अण्णहा
	    {
		mp->sb_prev = last_msgchunk;
		last_msgchunk->sb_next = mp;
		last_msgchunk = mp;
	    }
	    mp->sb_next = रिक्त;
	}
    }
    अण्णहा जइ (finish && last_msgchunk != रिक्त)
	last_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_col = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    शून्य
may_clear_sb_text(शून्य)
{
    do_clear_sb_text = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    शून्य
sb_text_start_cmdline(शून्य)
{
    जइ (do_clear_sb_text == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_text_restart_cmdline();
    अण्णहा
    {
	सन्देशः_sb_eol();
	do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    शून्य
sb_text_restart_cmdline(शून्य)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;

    जइ (last_msgchunk == रिक्त || last_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	प्रत्यागम;

    tofree = सन्देशः_sb_start(last_msgchunk);
    last_msgchunk = tofree->sb_prev;
    जइ (last_msgchunk != रिक्त)
	last_msgchunk->sb_next = रिक्त;
    जाव (tofree != रिक्त)
    {
	msgchunk_T *tofree_next = tofree->sb_next;

	vim_free(tofree);
	tofree = tofree_next;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    शून्य
sb_text_end_cmdline(शून्य)
{
    do_clear_sb_text = SB_CLEAR_CMDLINE_DONE;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    शून्य
clear_sb_text(पूर्णाङ्क all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    जइ (all)
	lastp = &last_msgchunk;
    अण्णहा
    {
	जइ (last_msgchunk == रिक्त)
	    प्रत्यागम;
	lastp = &सन्देशः_sb_start(last_msgchunk)->sb_prev;
    }

    जाव (*lastp != रिक्त)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    शून्य
show_sb_text(शून्य)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = सन्देशः_sb_start(last_msgchunk);
    जइ (mp == रिक्त || mp->sb_prev == रिक्त)
	vim_beep(BO_MESS);
    अण्णहा
    {
	do_more_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    स्थिर msgchunk_T *
सन्देशः_sb_start(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    जाव (mp != रिक्त && mp->sb_prev != रिक्त && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    प्रत्यागम mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    शून्य
सन्देशः_sb_eol(शून्य)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    जइ (last_msgchunk != रिक्त
	    && !(last_msgchunk->sb_text[0] == '\n'
		&& last_msgchunk->sb_text[1] == NUL))
	last_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    स्थिर msgchunk_T *
disp_sb_पङ्क्तिः(पूर्णाङ्क row, msgchunk_T *smp, पूर्णाङ्क clear_to_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    प्रति (;;)
    {
	सन्देशः_row = row;
	सन्देशः_col = mp->sb_सन्देशः_col;
	p = mp->sb_text;
	जइ (*p == '\n')	    // don't display the line break
	    ++p;
	सन्देशः_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	जइ (clear_to_eol)
	    screen_fill(row, row + 1,
		    cmdline_col_off + सन्देशः_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', HL_ATTR(HLF_सन्देशः));

	जइ (mp->sb_eol || mp->sb_next == रिक्त)
	    भङ्ग;
	mp = mp->sb_next;
    }
    प्रत्यागम mp->sb_next;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    स्थिर शून्य
t_puts(
    पूर्णाङ्क		*t_col,
    char_u	*t_s,
    char_u	*s,
    पूर्णाङ्क		attr)
{
    // output postponed text
    सन्देशः_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (पूर्णाङ्क)(s - t_s), सन्देशः_row, cmdline_col_off + सन्देशः_col,
	    attr);
    सन्देशः_col += *t_col;
    *t_col = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    जइ (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--सन्देशः_col;
    जइ (सन्देशः_col >= cmdline_width)
    {
	सन्देशः_col = 0;
	++सन्देशः_row;
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
    पूर्णाङ्क
सन्देशः_use_printf(शून्य)
{
    प्रत्यागम (!सन्देशः_check_screen()
#जइ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.in_use && !termcap_active)
# अण्णहा
	    || !termcap_active
# endif
#endif
	    || (swapping_screen() && !termcap_active)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    स्थिर शून्य
सन्देशः_puts_printf(char_u *str, पूर्णाङ्क maxlen)
{
    char_u	*s = str;
    char_u	*buf = रिक्त;
    char_u	*p = s;

#ifdef MSWIN
    जइ (!(silent_mode && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#endif
    जाव ((maxlen < 0 || (पूर्णाङ्क)(s - str) < maxlen) && *s != NUL)
    {
	जइ (!(silent_mode && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    जइ (*s == NL)
	    {
		पूर्णाङ्क n = (पूर्णाङ्क)(s - p);

		buf = alloc(n + 3);
		जइ (buf != रिक्त)
		{
		    memcpy(buf, p, n);
		    जइ (!info_message)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    जइ (info_message)   // informative message, not an error
			mch_सन्देशः((वर्ण *)buf);
		    अण्णहा
			mch_errmsg((वर्ण *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ifdef FEAT_RIGHTLEFT
	जइ (cmdmsg_rl)
	{
	    जइ (*s == CAR || *s == NL)
		सन्देशः_col = cmdline_width - 1;
	    अण्णहा
		--सन्देशः_col;
	}
	अण्णहा
#endif
	{
	    जइ (*s == CAR || *s == NL)
		सन्देशः_col = 0;
	    अण्णहा
		++सन्देशः_col;
	}
	++s;
    }

    जइ (*p != NUL && !(silent_mode && p_verbose == 0))
    {
	char_u *tofree = रिक्त;

	जइ (maxlen > 0 && vim_strlen_maxlen((वर्ण *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	जइ (p != रिक्त)
	{
	    जइ (info_message)
		mch_सन्देशः((वर्ण *)p);
	    अण्णहा
		mch_errmsg((वर्ण *)p);
	    vim_free(tofree);
	}
    }

    सन्देशः_didout = TRUE;	    // assume that line is not empty

#ifdef MSWIN
    जइ (!(silent_mode && p_verbose == 0))
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
    स्थिर पूर्णाङ्क
do_more_prompt(पूर्णाङ्क typed_char)
{
    स्थिर पूर्णाङ्क	entered = FALSE;
    पूर्णाङ्क		used_typed_char = typed_char;
    पूर्णाङ्क		oldState = State;
    पूर्णाङ्क		c;
#ifdef FEAT_CON_DIALOG
    पूर्णाङ्क		retval = FALSE;
#endif
    पूर्णाङ्क		toscroll;
    msgchunk_T	*mp_last = रिक्त;
    msgchunk_T	*mp;
    पूर्णाङ्क		i;
    पूर्णाङ्क		सन्देशः_attr;

    सन्देशः_attr = HL_ATTR(HLF_सन्देशः);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    जइ (entered || (State == MODE_HITRETURN && typed_char == 0))
	प्रत्यागम FALSE;
    entered = TRUE;

    जइ (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_last = सन्देशः_sb_start(last_msgchunk);
	प्रति (i = 0; i < Rows - 2 && mp_last != रिक्त
					     && mp_last->sb_prev != रिक्त; ++i)
	    mp_last = सन्देशः_sb_start(mp_last->sb_prev);
    }

    State = MODE_ASKMORE;
    setmouse();
    जइ (typed_char == NUL)
	सन्देशः_moremsg(FALSE);
    प्रति (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	जइ (used_typed_char != NUL)
	{
	    c = used_typed_char;	// was typed at hit-enter prompt
	    used_typed_char = NUL;
	}
	अण्णहा
	    c = get_keystroke();

#जइ defined(FEAT_MENU) && defined(FEAT_GUI)
	जइ (c == K_MENU)
	{
	    पूर्णाङ्क idx = get_menu_index(current_menu, MODE_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    जइ (idx == MENU_INDEX_INVALID)
		अनुवर्त;
	    c = *current_menu->strings[idx];
	    जइ (c != NUL && current_menu->strings[idx][1] != NUL)
		ins_typebuf(current_menu->strings[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#endif

	toscroll = 0;
	विकल्प (c)
	{
	पक्ष BS:		// scroll one line back
	पक्ष K_BS:
	पक्ष 'k':
	पक्ष K_UP:
	    toscroll = -1;
	    भङ्ग;

	पक्ष CAR:		// one extra line
	पक्ष NL:
	पक्ष 'j':
	पक्ष K_DOWN:
	    toscroll = 1;
	    भङ्ग;

	पक्ष 'u':		// Up half a page
	    toscroll = -(Rows / 2);
	    भङ्ग;

	पक्ष 'd':		// Down half a page
	    toscroll = Rows / 2;
	    भङ्ग;

	पक्ष 'b':		// one page back
	पक्ष Ctrl_B:
	पक्ष K_PAGEUP:
	    toscroll = -(Rows - 1);
	    भङ्ग;

	पक्ष ' ':		// one extra page
	पक्ष 'f':
	पक्ष Ctrl_F:
	पक्ष K_PAGEDOWN:
	पक्ष K_LEFTMOUSE:
	    toscroll = Rows - 1;
	    भङ्ग;

	पक्ष 'g':		// all the way back to the start
	    toscroll = -999999;
	    भङ्ग;

	पक्ष 'G':		// all the way to the end
	    toscroll = 999999;
	    पङ्क्तयः_वामः = 999999;
	    भङ्ग;

	पक्ष ':':		// start new command line
#ifdef FEAT_CON_DIALOG
	    जइ (!confirm_सन्देशः_used)
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
	पक्ष 'q':		// quit
	पक्ष Ctrl_C:
	पक्ष ESC:
#ifdef FEAT_CON_DIALOG
	    जइ (confirm_सन्देशः_used)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    अण्णहा
#endif
	    {
		got_int = TRUE;
		quit_more = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    पङ्क्तयः_वामः = Rows - 1;
	    भङ्ग;

#ifdef FEAT_CLIPBOARD
	पक्ष Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    जइ (clip_star.state == SELECT_DONE)
		clip_copy_modeless_selection(TRUE);
	    अनुवर्त;
#endif
	स्वभाव:		// no valid response
	    सन्देशः_moremsg(TRUE);
	    अनुवर्त;
	}

	जइ (toscroll != 0)
	{
	    जइ (toscroll < 0)
	    {
		// go to start of last line
		जइ (mp_last == रिक्त)
		    mp = सन्देशः_sb_start(last_msgchunk);
		अण्णहा जइ (mp_last->sb_prev != रिक्त)
		    mp = सन्देशः_sb_start(mp_last->sb_prev);
		अण्णहा
		    mp = रिक्त;

		// go to start of line at top of the screen
		प्रति (i = 0; i < Rows - 2 && mp != रिक्त && mp->sb_prev != रिक्त;
									  ++i)
		    mp = सन्देशः_sb_start(mp->sb_prev);

		जइ (mp != रिक्त && mp->sb_prev != रिक्त)
		{
		    // Find line to be displayed at top.
		    प्रति (i = 0; i > toscroll; --i)
		    {
			जइ (mp == रिक्त || mp->sb_prev == रिक्त)
			    भङ्ग;
			mp = सन्देशः_sb_start(mp->sb_prev);
			जइ (mp_last == रिक्त)
			    mp_last = सन्देशः_sb_start(last_msgchunk);
			अण्णहा
			    mp_last = सन्देशः_sb_start(mp_last->sb_prev);
		    }

		    जइ (toscroll == -1 && screen_ins_पङ्क्तयः(0, 0, 1,
						     (पूर्णाङ्क)Rows, 0, रिक्त) == OK)
		    {
			// display line at top
			(शून्य)disp_sb_पङ्क्तिः(0, mp, FALSE);
		    }
		    अण्णहा
		    {
			पूर्णाङ्क did_clear = screenclear();

			// redisplay all lines
			प्रति (i = 0; mp != रिक्त && i < Rows - 1; ++i)
			{
			    mp = disp_sb_पङ्क्तिः(i, mp, !did_clear);
			    ++सन्देशः_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    अण्णहा
	    {
		// First display any text that we scrolled back.
		जाव (toscroll > 0 && mp_last != रिक्त)
		{
		    // scroll up, display line at bottom
		    सन्देशः_scroll_up();
		    inc_सन्देशः_scrolled();
		    screen_fill((पूर्णाङ्क)Rows - 2, (पूर्णाङ्क)Rows - 1,
			    cmdline_col_off, cmdline_col_off + cmdline_width,
			    ' ', ' ', सन्देशः_attr);
		    mp_last = disp_sb_पङ्क्तिः((पूर्णाङ्क)Rows - 2, mp_last, FALSE);
		    --toscroll;
		}
	    }

	    जइ (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((पूर्णाङ्क)Rows - 1, (पूर्णाङ्क)Rows, cmdline_col_off,
			cmdline_col_off + cmdline_width, ' ', ' ', सन्देशः_attr);
		सन्देशः_moremsg(FALSE);
		अनुवर्त;
	    }

	    // display more text, return to caller
	    पङ्क्तयः_वामः = toscroll;
	}

	भङ्ग;
    }

    // clear the --more-- message
    screen_fill((पूर्णाङ्क)Rows - 1, (पूर्णाङ्क)Rows, cmdline_col_off,
	    cmdline_col_off + cmdline_width, ' ', ' ', सन्देशः_attr);
    State = oldState;
    setmouse();
    जइ (quit_more)
    {
	सन्देशः_row = Rows - 1;
	सन्देशः_col = 0;
    }
#ifdef FEAT_RIGHTLEFT
    अण्णहा जइ (cmdmsg_rl)
	सन्देशः_col = cmdline_width - 1;
#endif

    entered = FALSE;
#ifdef FEAT_CON_DIALOG
    प्रत्यागम retval;
#अण्णहा
    प्रत्यागम FALSE;
#endif
}

#जइ defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_सन्देशः
#  undef mch_सन्देशः
# endif

# जइ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    स्थिर शून्य
mch_errmsg_c(वर्ण *str)
{
    DWORD   nwrite = 0;
    DWORD   mode = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    जइ (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (पूर्णाङ्क)GetConsoleCP() != enc_codepage)
    {
	पूर्णाङ्क	len = (पूर्णाङ्क)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	जइ (w != रिक्त)
	{
	    WriteConsoleW(h, w, len, &nwrite, रिक्त);
	    vim_free(w);
	}
    }
    अण्णहा
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
    शून्य
mch_errmsg(वर्ण *str)
{
# जइ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    पूर्णाङ्क		len;
# endif

# जइ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    जइ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   अण्णहा
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
	प्रत्यागम;
    }
# endif

# जइ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    जइ (!(gui.in_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	प्रत्यागम;
    }
# endif

# जइ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (पूर्णाङ्क)STRLEN(str) + 1;
    जइ (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    जइ (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_data + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_data + error_ga.ga_len;
	    प्रति (;;)
	    {
		p = vim_strchr(p, '\r');
		जइ (p == रिक्त)
		    भङ्ग;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# जइ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    स्थिर शून्य
mch_सन्देशः_c(वर्ण *str)
{
    DWORD   nwrite = 0;
    DWORD   mode;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    जइ (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (पूर्णाङ्क)GetConsoleCP() != enc_codepage)
    {
	पूर्णाङ्क	len = (पूर्णाङ्क)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	जइ (w != रिक्त)
	{
	    WriteConsoleW(h, w, len, &nwrite, रिक्त);
	    vim_free(w);
	}
    }
    अण्णहा
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
    शून्य
mch_सन्देशः(वर्ण *str)
{
# जइ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    जइ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   अण्णहा
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
	प्रत्यागम;
    }
# endif

# जइ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    जइ (!(gui.in_use || gui.starting))
#  endif
    {
	mch_सन्देशः_c(str);
	प्रत्यागम;
    }
# endif
# जइ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#endif // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    स्थिर शून्य
सन्देशः_screen_putchar(पूर्णाङ्क c, पूर्णाङ्क attr)
{
    सन्देशः_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, सन्देशः_row, cmdline_col_off + सन्देशः_col, attr);
#ifdef FEAT_RIGHTLEFT
    जइ (cmdmsg_rl)
    {
	जइ (--सन्देशः_col == 0)
	{
	    सन्देशः_col = cmdline_width;
	    ++सन्देशः_row;
	}
    }
    अण्णहा
#endif
    {
	जइ (++सन्देशः_col >= cmdline_width)
	{
	    सन्देशः_col = 0;
	    ++सन्देशः_row;
	}
    }
}

    स्थिर शून्य
सन्देशः_moremsg(पूर्णाङ्क full)
{
    पूर्णाङ्क		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (पूर्णाङ्क)Rows - 1, cmdline_col_off, attr);
    जइ (full)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(पूर्णाङ्क)Rows - 1, cmdline_col_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    शून्य
repeat_message(शून्य)
{
    जइ (State == MODE_ASKMORE)
    {
	सन्देशः_moremsg(TRUE);	// display --more-- message again
	सन्देशः_row = Rows - 1;
    }
#ifdef FEAT_CON_DIALOG
    अण्णहा जइ (State == MODE_CONFIRM)
    {
	display_confirm_सन्देशः();	// display ":confirm" message again
	सन्देशः_row = Rows - 1;
    }
#endif
    अण्णहा जइ (State == MODE_EXTERNCMD)
    {
	windgoto(सन्देशः_row, cmdline_col_off + सन्देशः_col); // put cursor back
    }
    अण्णहा जइ (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	जइ (सन्देशः_row == Rows - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    सन्देशः_didout = FALSE;
	    सन्देशः_col = 0;
	    सन्देशः_clr_eos();
	}
	hit_return_सन्देशः();
	सन्देशः_row = Rows - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    स्थिर पूर्णाङ्क
सन्देशः_check_screen(शून्य)
{
    जइ (!full_screen || !screen_valid(FALSE))
	प्रत्यागम FALSE;

    जइ (सन्देशः_row >= Rows)
	सन्देशः_row = Rows - 1;
    जइ (सन्देशः_col >= cmdline_width)
	सन्देशः_col = cmdline_width - 1;
    प्रत्यागम TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    शून्य
सन्देशः_clr_eos(शून्य)
{
    जइ (सन्देशः_silent == 0)
	सन्देशः_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    शून्य
सन्देशः_clr_eos_force(शून्य)
{
#ifdef HAS_MESSAGE_WINDOW
    जइ (in_echowindow)
	प्रत्यागम;  // messages go into a popup
#endif
    जइ (सन्देशः_use_printf())
    {
	जइ (full_screen)	// only when termcap codes are valid
	{
	    जइ (*T_CD)
		out_str(T_CD);	// clear to end of display
	    अण्णहा जइ (*T_CE)
		out_str(T_CE);	// clear to end of line
	}
    }
    अण्णहा
    {
	पूर्णाङ्क सन्देशः_attr;

	सन्देशः_attr = HL_ATTR(HLF_सन्देशः);

#ifdef FEAT_RIGHTLEFT
	जइ (cmdmsg_rl)
	{
	    screen_fill(सन्देशः_row, सन्देशः_row + 1,
		    cmdline_col_off, cmdline_col_off + सन्देशः_col + 1,
		    ' ', ' ', सन्देशः_attr);
	    screen_fill(सन्देशः_row + 1, (पूर्णाङ्क)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', सन्देशः_attr);
	}
	अण्णहा
#endif
	{
	    screen_fill(सन्देशः_row, सन्देशः_row + 1,
		    cmdline_col_off + सन्देशः_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', सन्देशः_attr);
	    screen_fill(सन्देशः_row + 1, (पूर्णाङ्क)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', सन्देशः_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    शून्य
सन्देशः_clr_cmdline(शून्य)
{
    सन्देशः_row = cmdline_row;
    सन्देशः_col = 0;
    सन्देशः_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    पूर्णाङ्क
सन्देशः_end(शून्य)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    जइ (!exiting && need_wait_return && !(State & MODE_CMDLINE))
    {
	wait_return(FALSE);
	प्रत्यागम FALSE;
    }
    out_flush();
    प्रत्यागम TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    शून्य
सन्देशः_check(शून्य)
{
    जइ (सन्देशः_row == Rows - 1 && सन्देशः_col >= sc_col
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
    स्थिर शून्य
redir_write(char_u *str, पूर्णाङ्क maxlen)
{
    char_u	*s = str;
    स्थिर पूर्णाङ्क	cur_col = 0;

    // Don't do anything for displaying prompts and the like.
    जइ (redir_off)
	प्रत्यागम;

    // If 'verbosefile' is set prepare for writing in that file.
    जइ (*p_vfile != NUL && verbose_fd == रिक्त)
	verbose_open();

    जइ (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	जइ (*s != '\n' && *s != '\r')
	{
	    जाव (cur_col < सन्देशः_col)
	    {
#ifdef FEAT_EVAL
		जइ (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		अण्णहा जइ (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		अण्णहा जइ (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		अण्णहा
#endif
		    जइ (redir_fd != रिक्त)
		    fputs(" ", redir_fd);
		जइ (verbose_fd != रिक्त)
		    fputs(" ", verbose_fd);
		++cur_col;
	    }
	}

#ifdef FEAT_EVAL
	जइ (redir_execute)
	    execute_redir_str(s, maxlen);
	अण्णहा जइ (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	अण्णहा जइ (redir_vname)
	    var_redir_str(s, maxlen);
#endif

	// Write and adjust the current column.
	जाव (*s != NUL && (maxlen < 0 || (पूर्णाङ्क)(s - str) < maxlen))
	{
#ifdef FEAT_EVAL
	    जइ (!redir_reg && !redir_vname && !redir_execute)
#endif
		जइ (redir_fd != रिक्त)
		    putc(*s, redir_fd);
	    जइ (verbose_fd != रिक्त)
		putc(*s, verbose_fd);
	    जइ (*s == '\r' || *s == '\n')
		cur_col = 0;
	    अण्णहा जइ (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    अण्णहा
		++cur_col;
	    ++s;
	}

	जइ (सन्देशः_silent != 0)	// should update msg_col
	    सन्देशः_col = cur_col;
    }
}

    पूर्णाङ्क
redirecting(शून्य)
{
    प्रत्यागम redir_fd != रिक्त || *p_vfile != NUL
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#endif
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    शून्य
verbose_enter(शून्य)
{
    जइ (*p_vfile != NUL)
	++सन्देशः_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    शून्य
verbose_leave(शून्य)
{
    जइ (*p_vfile != NUL)
	जइ (--सन्देशः_silent < 0)
	    सन्देशः_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    शून्य
verbose_enter_scroll(शून्य)
{
    जइ (*p_vfile != NUL)
	++सन्देशः_silent;
    अण्णहा
	// always scroll up, don't overwrite
	सन्देशः_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    शून्य
verbose_leave_scroll(शून्य)
{
    जइ (*p_vfile != NUL)
    {
	जइ (--सन्देशः_silent < 0)
	    सन्देशः_silent = 0;
    }
    अण्णहा
	cmdline_row = सन्देशः_row;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    शून्य
verbose_stop(शून्य)
{
    जइ (verbose_fd != रिक्त)
    {
	fclose(verbose_fd);
	verbose_fd = रिक्त;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    पूर्णाङ्क
verbose_open(शून्य)
{
    जइ (verbose_fd == रिक्त && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((वर्ण *)p_vfile, "a");
	जइ (verbose_fd == रिक्त)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    प्रत्यागम FAIL;
	}
    }
    प्रत्यागम OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    शून्य
give_warning(char_u *message, पूर्णाङ्क hl)
{
    give_warning_with_source(message, hl, FALSE);
}

    शून्य
give_warning_with_source(char_u *message, पूर्णाङ्क hl, पूर्णाङ्क with_source)
{
    // Don't do this for ":silent".
    जइ (सन्देशः_silent != 0)
	प्रत्यागम;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, message, -1);
#endif
    VIM_CLEAR(keep_सन्देशः);
    जइ (hl)
	keep_सन्देशः_attr = HL_ATTR(HLF_W);
    अण्णहा
	keep_सन्देशः_attr = 0;

    जइ (with_source)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	सन्देशः_start();
	सन्देशः_source(HL_ATTR(HLF_W));
	सन्देशः_puts(" ");
	सन्देशः_puts_attr((वर्ण *)message, HL_ATTR(HLF_W) | सन्देशः_HIST);
	सन्देशः_clr_eos();
	(शून्य)सन्देशः_end();
    }
    अण्णहा जइ (सन्देशः_attr((वर्ण *)message, keep_सन्देशः_attr) && सन्देशः_scrolled == 0)
	set_keep_सन्देशः(message, keep_सन्देशः_attr);

    सन्देशः_didout = FALSE;	    // overwrite this message
    सन्देशः_nowait = TRUE;	    // don't wait for this message
    सन्देशः_col = 0;

    --no_wait_return;
}

#जइ defined(FEAT_EVAL)
    शून्य
give_warning2(char_u *message, char_u *a1, पूर्णाङ्क hl)
{
    जइ (IObuff == रिक्त)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(message, hl);
    }
    अण्णहा
    {
	vim_snprintf((वर्ण *)IObuff, IOSIZE, (वर्ण *)message, a1);
	give_warning(IObuff, hl);
    }
}
#endif

/*
 * Advance msg cursor to column "col".
 */
    शून्य
सन्देशः_advance(पूर्णाङ्क col)
{
    जइ (सन्देशः_silent != 0)	// nothing to advance to
    {
	सन्देशः_col = col;		// for redirection, may fill it up later
	प्रत्यागम;
    }
    जइ (col >= cmdline_width)	// not enough room
	col = cmdline_width - 1;
#ifdef FEAT_RIGHTLEFT
    जइ (cmdmsg_rl)
	जाव (सन्देशः_col > cmdline_width - col)
	    सन्देशः_putchar(' ');
    अण्णहा
#endif
	जाव (सन्देशः_col < col)
	    सन्देशः_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    शून्य
सन्देशः_warn_missing_clipboard(शून्य)
{
    जइ (!global_busy && !did_warn_clipboard && silence_w23_w24_सन्देशः == 0)
    {
#ifdef FEAT_CLIPBOARD
	सन्देशः(_("W23: Clipboard register not available, using register 0"));
#अण्णहा
	सन्देशः(_("W24: Clipboard register not available. See :h W24"));
#endif
	did_warn_clipboard = TRUE;
    }
}

#जइ defined(FEAT_CON_DIALOG)
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
    पूर्णाङ्क
do_dialog(
    पूर्णाङ्क		type UNUSED,
    char_u	*title UNUSED,
    char_u	*message,
    char_u	*buttons,
    पूर्णाङ्क		dfltbutton,
    char_u	*textfield UNUSED,	// IObuff for inputdialog(), NULL
					// otherwise
    पूर्णाङ्क		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    पूर्णाङ्क		oldState;
    पूर्णाङ्क		retval = 0;
    char_u	*hotkeys;
    पूर्णाङ्क		c;
    पूर्णाङ्क		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    जइ (silent_mode)
	प्रत्यागम dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    जइ (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == रिक्त)
    {
	// --gui-dialog-file: write text to a file
	जइ (gui_dialog_log(title, message))
	    c = dfltbutton;
	अण्णहा
	    c = gui_mch_dialog(type, title, message, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_row = सन्देशः_row;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	out_flush();
	gui_mch_update();

	प्रत्यागम c;
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
    hotkeys = सन्देशः_show_console_dialog(message, buttons, dfltbutton);

    जइ (hotkeys != रिक्त)
    {
	प्रति (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    विकल्प (c)
	    {
	    पक्ष CAR:		// User accepts default option
	    पक्ष NL:
		retval = dfltbutton;
		भङ्ग;
	    पक्ष Ctrl_C:	// User aborts/cancels
	    पक्ष ESC:
		retval = 0;
		भङ्ग;
	    स्वभाव:		// Could be a hotkey?
		जइ (c < 0)	// special keys are ignored here
		    अनुवर्त;
		जइ (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    भङ्ग;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		प्रति (i = 0; hotkeys[i]; ++i)
		{
		    जइ (has_mbyte)
		    {
			जइ ((*mb_ptr2char)(hotkeys + i) == c)
			    भङ्ग;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    अण्णहा
			जइ (hotkeys[i] == c)
			    भङ्ग;
		    ++retval;
		}
		जइ (hotkeys[i])
		    भङ्ग;
		// No hotkey match, so keep waiting
		अनुवर्त;
	    }
	    भङ्ग;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    सन्देशः_end_prompt();

    प्रत्यागम retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    स्थिर पूर्णाङ्क
copy_char(
    char_u	*from,
    char_u	*to,
    पूर्णाङ्क		lowercase)	// make character lower case
{
    पूर्णाङ्क		len;
    पूर्णाङ्क		c;

    जइ (has_mbyte)
    {
	जइ (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    प्रत्यागम (*mb_char2bytes)(c, to);
	}
	अण्णहा
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(to, from, (size_t)len);
	    प्रत्यागम len;
	}
    }
    अण्णहा
    {
	जइ (lowercase)
	    *to = (char_u)TOLOWER_LOC(*from);
	अण्णहा
	    *to = *from;
	प्रत्यागम 1;
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
    स्थिर char_u *
सन्देशः_show_console_dialog(
    char_u	*message,
    char_u	*buttons,
    पूर्णाङ्क		dfltbutton)
{
    पूर्णाङ्क		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    पूर्णाङ्क		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = रिक्त;
    char_u	*msgp = रिक्त;
    char_u	*hotkp = रिक्त;
    char_u	*r;
    पूर्णाङ्क		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    पूर्णाङ्क		first_hotkey = FALSE;	// first char of button is hotkey
    पूर्णाङ्क		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    प्रति (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	जाव (*r)
	{
	    जइ (*r == DLG_BUTTON_SEP)
	    {
		जइ (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    जइ (has_mbyte)
			hotkp += STRLEN(hotkp);
		    अण्णहा
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    जइ (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    जइ (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			first_hotkey = TRUE;
		}
		अण्णहा
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    जइ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    अण्णहा जइ (*r == DLG_HOTKEY_CHAR || first_hotkey)
	    {
		जइ (*r == DLG_HOTKEY_CHAR)
		    ++r;
		first_hotkey = FALSE;
		जइ (copy)
		{
		    जइ (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    अण्णहा
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		अण्णहा
		{
		    ++len;	    // '&a' -> '[a]'
		    जइ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    अण्णहा
	    {
		// everything else copy literally
		जइ (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	जइ (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	अण्णहा
	{
	    len += (पूर्णाङ्क)(STRLEN(message)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    जइ (!has_hotkey[0])
	    {
		first_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_सन्देशः);
	    confirm_सन्देशः = alloc(len);
	    जइ (confirm_सन्देशः == रिक्त)
		प्रत्यागम रिक्त;
	    *confirm_सन्देशः = NUL;
	    hotk = alloc(lenhotkey);
	    जइ (hotk == रिक्त)
		प्रत्यागम रिक्त;

	    *confirm_सन्देशः = '\n';
	    STRCPY(confirm_सन्देशः + 1, message);

	    msgp = confirm_सन्देशः + 1 + STRLEN(message);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_सन्देशः_tail = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_सन्देशः();
    प्रत्यागम hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    स्थिर शून्य
display_confirm_सन्देशः(शून्य)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_सन्देशः_used;
    जइ (confirm_सन्देशः != रिक्त)
	सन्देशः_puts_attr((वर्ण *)confirm_सन्देशः, HL_ATTR(HLF_M));
    --confirm_सन्देशः_used;
}

#endif // FEAT_CON_DIALOG

#जइ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    पूर्णाङ्क
vim_dialog_yesno(
    पूर्णाङ्क		type,
    char_u	*title,
    char_u	*message,
    पूर्णाङ्क		dflt)
{
    जइ (do_dialog(type,
		title == रिक्त ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No"), dflt, रिक्त, FALSE) == 1)
	प्रत्यागम VIM_YES;
    प्रत्यागम VIM_NO;
}

    पूर्णाङ्क
vim_dialog_yesnocancel(
    पूर्णाङ्क		type,
    char_u	*title,
    char_u	*message,
    पूर्णाङ्क		dflt)
{
    विकल्प (do_dialog(type,
		title == रिक्त ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, रिक्त, FALSE))
    {
	पक्ष 1: प्रत्यागम VIM_YES;
	पक्ष 2: प्रत्यागम VIM_NO;
    }
    प्रत्यागम VIM_CANCEL;
}

    पूर्णाङ्क
vim_dialog_yesnoallcancel(
    पूर्णाङ्क		type,
    char_u	*title,
    char_u	*message,
    पूर्णाङ्क		dflt)
{
    विकल्प (do_dialog(type,
		title == रिक्त ? (char_u *)"Question" : title,
		message,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, रिक्त, FALSE))
    {
	पक्ष 1: प्रत्यागम VIM_YES;
	पक्ष 2: प्रत्यागम VIM_NO;
	पक्ष 3: प्रत्यागम VIM_ALL;
	पक्ष 4: प्रत्यागम VIM_DISCARDALL;
    }
    प्रत्यागम VIM_CANCEL;
}

#endif // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
