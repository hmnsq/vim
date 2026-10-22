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

स्थिर शून्य add_संदेश_hist(char_u *s, पूर्णाङ्क len, पूर्णाङ्क attr);
स्थिर शून्य check_संदेश_hist(शून्य);
स्थिर शून्य hit_return_संदेश(शून्य);
स्थिर शून्य संदेश_home_replace_attr(char_u *fname, पूर्णाङ्क attr);
स्थिर शून्य संदेश_puts_attr_len(वर्ण *str, पूर्णाङ्क maxlen, पूर्णाङ्क attr);
स्थिर शून्य संदेश_puts_display(char_u *str, पूर्णाङ्क maxlen, पूर्णाङ्क attr, पूर्णाङ्क recurse);
स्थिर शून्य संदेश_scroll_up(शून्य);
स्थिर शून्य inc_संदेश_scrolled(शून्य);
स्थिर शून्य store_sb_text(char_u **sb_str, char_u *s, पूर्णाङ्क attr, पूर्णाङ्क *sb_col, पूर्णाङ्क finish);
स्थिर शून्य t_puts(पूर्णाङ्क *t_col, char_u *t_s, char_u *s, पूर्णाङ्क attr);
स्थिर शून्य संदेश_puts_printf(char_u *str, पूर्णाङ्क maxlen);
स्थिर पूर्णाङ्क do_more_prompt(पूर्णाङ्क typed_char);
स्थिर शून्य संदेश_screen_putchar(पूर्णाङ्क c, पूर्णाङ्क attr);
स्थिर शून्य संदेश_moremsg(पूर्णाङ्क full);
स्थिर पूर्णाङ्क  संदेश_check_screen(शून्य);
स्थिर शून्य redir_write(char_u *s, पूर्णाङ्क maxlen);
#ifdef FEAT_CON_DIALOG
स्थिर char_u *संदेश_show_console_dialog(char_u *message, char_u *buttons, पूर्णाङ्क dfltbutton);
स्थिर पूर्णाङ्क	confirm_संदेश_used = FALSE;	// displaying confirm_msg
स्थिर char_u	*confirm_संदेश = रिक्त;		// ":confirm" message
स्थिर char_u	*confirm_संदेश_tail;		// tail of confirm_msg
स्थिर शून्य display_confirm_संदेश(शून्य);
#endif
#ifdef FEAT_EVAL
स्थिर पूर्णाङ्क emsg_to_channel_log = FALSE;
#endif

संरचना संदेश_hist
{
    संरचना संदेश_hist	*next;
    char_u		*संदेश;
    पूर्णाङ्क			attr;
};

स्थिर संरचना संदेश_hist *first_संदेश_hist = रिक्त;
स्थिर संरचना संदेश_hist *last_संदेश_hist = रिक्त;
स्थिर पूर्णाङ्क संदेश_hist_len = 0;
स्थिर पूर्णाङ्क संदेश_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#define MESSAGES_HIT_ENTER	0x001
#define MESSAGES_WAIT		0x002
#define MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#define MESSAGES_OPT_HIT_ENTER "hit-enter"
#define MESSAGES_OPT_WAIT "wait:"
#define MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
स्थिर पूर्णाङ्क संदेश_flags = MESSAGES_HIT_ENTER | MESSAGES_HISTORY;
स्थिर पूर्णाङ्क संदेश_wait = 0;

स्थिर खाता *verbose_fd = रिक्त;
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
संदेश(वर्ण *s)
{
    प्रत्यागम संदेश_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    पूर्णाङ्क
verb_संदेश(वर्ण *s)
{
    पूर्णाङ्क		n;

    verbose_enter();
    n = संदेश_attr_keep(s, 0, FALSE);
    verbose_leave();

    प्रत्यागम n;
}

    पूर्णाङ्क
संदेश_attr(वर्ण *s, पूर्णाङ्क attr)
{
    प्रत्यागम संदेश_attr_keep(s, attr, FALSE);
}

    पूर्णाङ्क
संदेश_attr_keep(
    वर्ण	*s,
    पूर्णाङ्क		attr,
    पूर्णाङ्क		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    स्थिर पूर्णाङ्क	entered = 0;
    पूर्णाङ्क		retval;
    char_u	*buf = रिक्त;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    यदि (!emsg_on_display && message_filtered((char_u *)s))
	प्रत्यागम TRUE;

#ifdef FEAT_EVAL
    यदि (attr == 0)
	set_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#endif

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    यदि (entered >= 3)
	प्रत्यागम TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    यदि ((char_u *)s != keep_संदेश
	    || (*s != '<'
		&& last_संदेश_hist != रिक्त
		&& last_संदेश_hist->संदेश != रिक्त
		&& STRCMP(s, last_संदेश_hist->संदेश) != 0))
	add_संदेश_hist((char_u *)s, -1, attr);

#ifdef FEAT_EVAL
    यदि (emsg_to_channel_log)
	// Write message in the channel log.
	ch_log(रिक्त, "ERROR: %s", s);
#endif

    // Truncate the message if needed.
    संदेश_start();
    buf = संदेश_strtrunc((char_u *)s, FALSE);
    यदि (buf != रिक्त)
	s = (वर्ण *)buf;

    संदेश_outtrans_attr((char_u *)s, attr);
    संदेश_clr_eos();
    retval = संदेश_end();

    यदि (keep && retval && vim_strsize((char_u *)s)
		    < (पूर्णाङ्क)(Rows - cmdline_row - 1) * cmdline_width + sc_col)
	set_keep_संदेश((char_u *)s, 0);

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
संदेश_strtrunc(
    char_u	*s,
    पूर्णाङ्क		force)	    // always truncate
{
    char_u	*buf = रिक्त;
    पूर्णाङ्क		len;
    पूर्णाङ्क		room;

    // May truncate message to avoid a hit-return prompt
    यदि ((!संदेश_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_active && संदेश_silent == 0) || force)
    {
	len = vim_strsize(s);
	यदि (संदेश_scrolled != 0
#ifdef HAS_MESSAGE_WINDOW
		|| in_echowindow
#endif
		)
	    // Use all the columns.
	    room = (पूर्णाङ्क)(Rows - संदेश_row) * cmdline_width - 1;
	अन्यथा
	    // Use up to 'showcmd' column.
	    room = (पूर्णाङ्क)(Rows - संदेश_row - 1) * cmdline_width + sc_col - 1;
	यदि (len > room && room > 0)
	{
	    यदि (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (room + 2) * 18;
	    अन्यथा यदि (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (room + 2) * 2;
	    अन्यथा
		len = room + 2;
	    buf = alloc(len);
	    यदि (buf != रिक्त)
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
    माप_प्रकार	room = room_in - 3; // "..." takes 3 chars
    माप_प्रकार	half;
    माप_प्रकार	len = 0;
    पूर्णाङ्क		e;
    पूर्णाङ्क		i;
    पूर्णाङ्क		n;

    यदि (*s == NUL)
    {
	यदि (buflen > 0)
	    *buf = NUL;
	प्रत्यागम;
    }

    यदि (room_in < 3)
	room = 0;
    half = room / 2;

    // First part: Start of the string.
    प्रति (e = 0; len < half && e < buflen; ++e)
    {
	यदि (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    प्रत्यागम;
	}
	n = ptr2cells(s + e);
	यदि (len + n > half)
	    भङ्ग;
	len += n;
	buf[e] = s[e];
	यदि (has_mbyte)
	    प्रति (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		यदि (++e == buflen)
		    भङ्ग;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    यदि (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	यावत् (len + n > room)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    अन्यथा यदि (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (पूर्णाङ्क)STRLEN(s);
	प्रति (;;)
	{
	    कुरु
		half = half - utf_head_off(s, s + half - 1) - 1;
	    यावत् (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    यदि (len + n > room || half == 0)
		भङ्ग;
	    len += n;
	    i = (पूर्णाङ्क)half;
	}
    }
    अन्यथा
    {
	प्रति (i = (पूर्णाङ्क)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= room; --i)
	    len += n;
    }


    यदि (i <= e + 3)
    {
	// text fits without truncating
	यदि (s != buf)
	{
	    len = STRLEN(s);
	    यदि (len >= (माप_प्रकार)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    यदि (len < 1)
		buf[e - 1] = NUL;
	    अन्यथा
		mch_memmove(buf + e, s + e, len);
	}
    }
    अन्यथा यदि (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (माप_प्रकार)3);
	len = STRLEN(s + i) + 1;
	यदि (len >= (माप_प्रकार)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    अन्यथा
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

पूर्णाङ्क vim_snprintf(वर्ण *str, माप_प्रकार str_m, ध्रुव वर्ण *fmt, ...);

    पूर्णाङ्क
smsg(ध्रुव वर्ण *s, ...)
{
    यदि (IObuff == रिक्त)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	प्रत्यागम संदेश((वर्ण *)s);
    }

    बहु_सूची arglist;

    बहु_शुरू(arglist, s);
    vim_vsnprintf((वर्ण *)IObuff, IOSIZE, s, arglist);
    बहु_पूर्ण(arglist);
    प्रत्यागम संदेश((वर्ण *)IObuff);
}

    पूर्णाङ्क
smsg_attr(पूर्णाङ्क attr, ध्रुव वर्ण *s, ...)
{
    यदि (IObuff == रिक्त)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	प्रत्यागम संदेश_attr((वर्ण *)s, attr);
    }

    बहु_सूची arglist;

    बहु_शुरू(arglist, s);
    vim_vsnprintf((वर्ण *)IObuff, IOSIZE, s, arglist);
    बहु_पूर्ण(arglist);
    प्रत्यागम संदेश_attr((वर्ण *)IObuff, attr);
}

    पूर्णाङ्क
smsg_attr_keep(पूर्णाङ्क attr, ध्रुव वर्ण *s, ...)
{
    यदि (IObuff == रिक्त)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	प्रत्यागम संदेश_attr_keep((वर्ण *)s, attr, TRUE);
    }

    बहु_सूची arglist;

    बहु_शुरू(arglist, s);
    vim_vsnprintf((वर्ण *)IObuff, IOSIZE, s, arglist);
    बहु_पूर्ण(arglist);
    प्रत्यागम संदेश_attr_keep((वर्ण *)IObuff, attr, TRUE);
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
    यदि (HAVE_SOURCING_INFO && SOURCING_NAME != रिक्त)
    {
	यदि (last_sourcing_name != रिक्त)
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

    यदि (HAVE_SOURCING_INFO && SOURCING_NAME != रिक्त && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	यदि (sname == रिक्त)
	    sname = SOURCING_NAME;

#ifdef FEAT_EVAL
	यदि (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	अन्यथा
#endif
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	यदि (Buf != रिक्त)
	    प्र_लिखो((वर्ण *)Buf, (वर्ण *)p, sname);
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
    यदि (SOURCING_NAME != रिक्त
	    && (other_sourcing_name() || SOURCING_LNUM != last_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	यदि (Buf != रिक्त)
	    प्र_लिखो((वर्ण *)Buf, (वर्ण *)p, (दीर्घ)SOURCING_LNUM);
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
संदेश_source(पूर्णाङ्क attr)
{
    char_u	*p;
    स्थिर पूर्णाङ्क	recursive = FALSE;

    // Bail out if something called here causes an error.
    यदि (recursive)
	प्रत्यागम;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_source();
    यदि (p != रिक्त)
    {
	संदेश_scroll = TRUE;  // this will take more than one line
	संदेश_attr((वर्ण *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    यदि (p != रिक्त)
    {
	संदेश_attr((वर्ण *)p, HL_ATTR(HLF_N));
	vim_free(p);
	last_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    यदि (SOURCING_NAME == रिक्त || other_sourcing_name())
    {
	VIM_CLEAR(last_sourcing_name);
	यदि (SOURCING_NAME != रिक्त)
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
    यदि ((emsg_off > 0 && vim_strchr(p_debug, 'm') == रिक्त
					  && vim_strchr(p_debug, 't') == रिक्त)
#ifdef FEAT_EVAL
	    || emsg_skip > 0
#endif
	    )
	प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

#यदि defined(FEAT_EVAL)
स्थिर garray_T ignore_error_list = GA_EMPTY;

    शून्य
ignore_error_for_testing(char_u *error)
{
    यदि (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, परिमाण(char_u *), 1);

    यदि (STRCMP("RESET", error) == 0)
	ga_clear_strings(&ignore_error_list);
    अन्यथा
	ga_copy_string(&ignore_error_list, error);
}

    स्थिर पूर्णाङ्क
ignore_error(ध्रुव वर्ण *संदेश)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < ignore_error_list.ga_len; ++i)
	यदि (म_माला(संदेश,
		  (वर्ण *)((char_u **)(ignore_error_list.ga_data))[i]) != रिक्त)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}
#endif

#यदि !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    शून्य
do_perror(वर्ण *संदेश)
{
    लिखो_त्रुटि(संदेश);
    ++emsg_silent;
    emsg(संदेश);
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
    यदि (ignore_error(s))
	// don't call msg() if it results in a dialog
	प्रत्यागम संदेश_use_printf() ? FALSE : संदेश((वर्ण *)s);
#endif

    ++called_emsg;

#ifdef FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#endif

    यदि (!emsg_off || vim_strchr(p_debug, 't') != रिक्त)
    {
#ifdef FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	यदि (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    यदि (!ignore)
		++did_emsg;
	    प्रत्यागम TRUE;
	}

	यदि (in_assert_fails && emsg_assert_fails_संदेश == रिक्त)
	{
	    emsg_assert_fails_संदेश = vim_strsave((char_u *)s);
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
	यदि (emsg_silent != 0)
	{
#ifdef FEAT_EVAL
	    ++did_emsg_silent;
#endif
	    यदि (emsg_noredir == 0)
	    {
		संदेश_start();
		p = get_emsg_source();
		यदि (p != रिक्त)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		यदि (p != रिक्त)
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
	    यदि (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#endif
#ifdef FEAT_EVAL
	    ch_log(रिक्त, "ERROR silent: %s", s);
#endif
	    प्रत्यागम TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	संदेश_silent = 0;
	cmd_silent = FALSE;

	यदि (global_busy)		// break :global command
	    ++global_busy;

	यदि (p_eb)
	    beep_flush();		// also includes flush_buffers()
	अन्यथा
	    flush_buffers(FLUSH_MINIMAL);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ifdef FEAT_EVAL
	++uncaught_emsg;
#endif
    }

#ifdef HAS_MESSAGE_WINDOW
    यदि (!in_echowindow)
#endif
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    यदि (संदेश_scrolled != 0)
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
    संदेश_scroll = TRUE;
    संदेश_source(attr);

    /*
     * Display the error message itself.
     */
    संदेश_nowait = FALSE;			// wait for this msg
    r = संदेश_attr((वर्ण *)s, attr);

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
    यदि (emsg_not_now())
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
    यदि (emsg_not_now())
	प्रत्यागम TRUE;

    यदि (IObuff == रिक्त)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	प्रत्यागम emsg_core(s);

    बहु_सूची ap;

    बहु_शुरू(ap, s);
    vim_vsnprintf((वर्ण *)IObuff, IOSIZE, s, ap);
    बहु_पूर्ण(ap);
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
    यदि (emsg_not_now())
	प्रत्यागम;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    emsg_core(s);
#यदि defined(ABORT_ON_INTERNAL_ERROR) && defined(FEAT_EVAL)
    set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    संदेश_putchar('\n');  // avoid overwriting the error message
    out_flush();
    पात();
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
    यदि (emsg_not_now())
	प्रत्यागम;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    यदि (IObuff == रिक्त)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    अन्यथा
    {
	बहु_सूची ap;

	बहु_शुरू(ap, s);
	vim_vsnprintf((वर्ण *)IObuff, IOSIZE, s, ap);
	बहु_पूर्ण(ap);
	emsg_core((वर्ण *)IObuff);
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    संदेश_putchar('\n');  // avoid overwriting the error message
    out_flush();
    पात();
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

#यदि defined(FEAT_EVAL)
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

#यदि defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    शून्य
emsg_namelen(वर्ण *संदेश, char_u *name, पूर्णाङ्क len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(संदेश, copy == रिक्त ? "NULL" : (वर्ण *)copy);
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
संदेश_trunc_attr(वर्ण *s, पूर्णाङ्क force, पूर्णाङ्क attr)
{
    पूर्णाङ्क		n;
    वर्ण	*ts;

    // Add message to history before truncating
    add_संदेश_hist((char_u *)s, -1, attr);

    ts = (वर्ण *)संदेश_may_trunc(force, (char_u *)s);

    संदेश_hist_off = TRUE;
    n = संदेश_attr(ts, attr);
    संदेश_hist_off = FALSE;

    यदि (n)
	प्रत्यागम ts;
    प्रत्यागम रिक्त;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
संदेश_may_trunc(पूर्णाङ्क force, char_u *s)
{
    पूर्णाङ्क		n;
    पूर्णाङ्क		room;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    room = (पूर्णाङ्क)(Rows - cmdline_row - 1) * cmdline_width + sc_col - 1;
    यदि (room > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_active))
	    && (n = (पूर्णाङ्क)STRLEN(s) - room) > 0)
    {
	यदि (has_mbyte)
	{
	    पूर्णाङ्क	size = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    यदि (size <= room)
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
add_संदेश_hist(
    char_u	*s,
    पूर्णाङ्क		len,		// -1 for undetermined length
    पूर्णाङ्क		attr)
{
    संरचना संदेश_hist *p;

    यदि (संदेश_hist_off || संदेश_silent != 0)
	प्रत्यागम;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(संरचना संदेश_hist);
    यदि (p == रिक्त)
	प्रत्यागम;

    यदि (len < 0)
	len = (पूर्णाङ्क)STRLEN(s);
    // remove leading and trailing newlines
    यावत् (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    यावत् (len > 0 && s[len - 1] == '\n')
	--len;
    p->संदेश = vim_strnsave(s, len);
    p->next = रिक्त;
    p->attr = attr;
    यदि (last_संदेश_hist != रिक्त)
	last_संदेश_hist->next = p;
    last_संदेश_hist = p;
    यदि (first_संदेश_hist == रिक्त)
	first_संदेश_hist = last_संदेश_hist;
    ++संदेश_hist_len;

    check_संदेश_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    पूर्णाङ्क
delete_first_संदेश(शून्य)
{
    संरचना संदेश_hist *p;

    यदि (संदेश_hist_len <= 0)
	प्रत्यागम FAIL;
    p = first_संदेश_hist;
    first_संदेश_hist = p->next;
    यदि (first_संदेश_hist == रिक्त)
	last_संदेश_hist = रिक्त;  // history is empty
    vim_free(p->संदेश);
    vim_free(p);
    --संदेश_hist_len;
    प्रत्यागम OK;
}

    स्थिर शून्य
check_संदेश_hist(शून्य)
{
    // Don't let the message history get too big
    यावत् (संदेश_hist_len > 0 && संदेश_hist_len > संदेश_hist_max)
	(शून्य)delete_first_संदेश();
}


    पूर्णाङ्क
messagesopt_changed(शून्य)
{
    char_u	*p;
    पूर्णाङ्क		messages_flags_new = 0;
    पूर्णाङ्क		messages_wait_new = 0;
    पूर्णाङ्क		messages_history_new = 0;

    p = p_mopt;
    यावत् (*p != NUL)
    {
	यदि (STRNCMP(p, MESSAGES_OPT_HIT_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER);
	    messages_flags_new |= MESSAGES_HIT_ENTER;
	}
	अन्यथा यदि (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	अन्यथा यदि (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	यदि (*p != ',' && *p != NUL)
	    प्रत्यागम FAIL;
	यदि (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    यदि (!(messages_flags_new & (MESSAGES_HIT_ENTER | MESSAGES_WAIT)))
	प्रत्यागम FAIL;

    // "history" must be set
    यदि (!(messages_flags_new & MESSAGES_HISTORY))
	प्रत्यागम FAIL;

    यदि (messages_history_new < 0 || messages_history_new > 10000)
	प्रत्यागम FAIL;

    यदि (messages_wait_new < 0 || messages_wait_new > 10000)
	प्रत्यागम FAIL;

    संदेश_flags = messages_flags_new;
    संदेश_wait = messages_wait_new;

    संदेश_hist_max = messages_history_new;
    check_संदेश_hist();

    प्रत्यागम OK;
}

/*
 * ":messages" command.
 */
    शून्य
ex_messages(exarg_T *eap)
{
    संरचना संदेश_hist *p;
    char_u	    *s;
    पूर्णाङ्क		    c = 0;

    यदि (STRCMP(eap->arg, "clear") == 0)
    {
	पूर्णाङ्क keep = eap->addr_गिनती == 0 ? 0 : eap->line2;

	यावत् (संदेश_hist_len > keep)
	    (शून्य)delete_first_संदेश();
	प्रत्यागम;
    }

    यदि (*eap->arg != NUL)
    {
	emsg(_(e_invalid_argument));
	प्रत्यागम;
    }

    संदेश_hist_off = TRUE;

    p = first_संदेश_hist;
    यदि (eap->addr_गिनती != 0)
    {
	// Count total messages
	प्रति (; p != रिक्त && !got_int; p = p->next)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	प्रति (p = first_संदेश_hist; p != रिक्त && !got_int && c > 0;
						    p = p->next, c--);
    }

    यदि (p == first_संदेश_hist)
    {
#ifdef FEAT_MULTI_LANG
	s = get_mess_lang();
#अन्यथा
	s = mch_getenv((char_u *)"LANG");
#endif
	यदि (s != रिक्त && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    संदेश_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    प्रति (; p != रिक्त && !got_int; p = p->next)
	यदि (p->संदेश != रिक्त)
	    संदेश_attr((वर्ण *)p->संदेश, p->attr);

    संदेश_hist_off = FALSE;
}

#यदि defined(FEAT_CON_DIALOG) || defined(FIND_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    शून्य
संदेश_end_prompt(शून्य)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_row = संदेश_row;
    संदेश_col = 0;
    संदेश_clr_eos();
    पंक्तियाँ_बायाँ = -1;
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
    खाता	*save_scriptout;

    यदि (redraw == TRUE)
	set_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    यदि (संदेश_silent != 0)
	प्रत्यागम;
#ifdef HAS_MESSAGE_WINDOW
    यदि (in_echowindow)
	प्रत्यागम;
#endif

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    यदि (vgetc_busy > 0)
	प्रत्यागम;
    need_wait_return = TRUE;
    यदि (no_wait_return)
    {
	यदि (!exmode_active)
	    cmdline_row = संदेश_row;
	प्रत्यागम;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    यदि (quit_more)
    {
	c = CAR;		// just pretend CR was hit
	quit_more = FALSE;
	got_int = FALSE;
    }
    अन्यथा यदि (exmode_active)
    {
	संदेश_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    अन्यथा यदि (!stuff_empty())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    अन्यथा
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = MODE_HITRETURN;
	setmouse();
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#endif
	cmdline_row = संदेश_row;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	यदि (need_check_timestamps)
	    check_timestamps(FALSE);

	यदि (संदेश_flags & MESSAGES_HIT_ENTER)
	{
	    hit_return_संदेश();

	    कुरु
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
		++allow_कुंजियाँ;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = रिक्त;
		c = safe_vgetc();
		यदि (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_कुंजियाँ;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ifdef FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		यदि (कुंजी_Typed && c == Ctrl_Y && clip_star.state == SELECT_DONE)
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
		यदि (कुंजी_Typed && p_more && !p_cp)
		{
		    यदि (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_UP || c == K_PAGEUP)
		    {
			यदि (संदेश_scrolled > Rows)
			    // scroll back to show older messages
			    do_more_prompt(c);
			अन्यथा
			{
			    संदेश_didout = FALSE;
			    c = K_IGNORE;
			    संदेश_col =
#ifdef FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_width - 1 :
#endif
				0;
			}
			यदि (quit_more)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_more = FALSE;
			    got_int = FALSE;
			}
			अन्यथा यदि (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    hit_return_संदेश();
			}
		    }
		    अन्यथा यदि (संदेश_scrolled > Rows - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_DOWN || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } यावत् ((had_got_int && c == Ctrl_C)
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
				    && mouse_row < संदेश_row
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    यदि (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(शून्य)jump_to_mouse(MOUSE_SETPOS, रिक्त, 0);
	    अन्यथा यदि (!कुंजी_Typed || (vim_strchr((char_u *)"\r\n ", c) == रिक्त
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_mask);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	अन्यथा
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(संदेश_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    यदि (c == ':' || c == '?' || c == '/')
    {
	यदि (!exmode_active)
	    cmdline_row = संदेश_row;
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
    संदेश_check();

#ifndef MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    यदि (swapping_screen() && !termcap_active)
	newline_on_exit = TRUE;
#endif

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    पंक्तियाँ_बायाँ = -1;		// reset lines_left at next msg_start()
    reset_last_sourcing();
    यदि (keep_संदेश != रिक्त && vim_strsize(keep_संदेश) >=
			    (Rows - cmdline_row - 1) * cmdline_width + sc_col)
	VIM_CLEAR(keep_संदेश);	    // don't redisplay message, it's too long

    यदि (tmpState == MODE_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    अन्यथा यदि (!skip_redraw
	    && (redraw == TRUE || (संदेश_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    स्थिर शून्य
hit_return_संदेश(शून्य)
{
    पूर्णाङ्क		save_p_more = p_more;

    p_more = FALSE;	// don't want to see this message when scrolling back
    यदि (संदेश_didout)	// start on a new line
	संदेश_putchar('\n');
    यदि (got_int)
	संदेश_puts(_("Interrupt: "));

    संदेश_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    यदि (!संदेश_use_printf())
	संदेश_clr_eos();
    p_more = save_p_more;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    शून्य
set_keep_संदेश(char_u *s, पूर्णाङ्क attr)
{
    vim_free(keep_संदेश);
    यदि (s != रिक्त && संदेश_silent == 0)
	keep_संदेश = vim_strsave(s);
    अन्यथा
	keep_संदेश = रिक्त;
    keep_संदेश_more = FALSE;
    keep_संदेश_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    शून्य
set_keep_संदेश_from_hist(शून्य)
{
    यदि (keep_संदेश == रिक्त && last_संदेश_hist != रिक्त && संदेश_scrolled == 0
						      && (State & MODE_NORMAL))
	set_keep_संदेश(last_संदेश_hist->संदेश, last_संदेश_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    शून्य
संदेश_start(शून्य)
{
    पूर्णाङ्क		did_return = FALSE;

    यदि (संदेश_row < cmdline_row)
	संदेश_row = cmdline_row;

    यदि (!संदेश_silent)
    {
	VIM_CLEAR(keep_संदेश);
	need_fileinfo = FALSE;
    }

#ifdef FEAT_EVAL
    यदि (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	संदेश_clr_eos();
    }
#endif

#ifdef HAS_MESSAGE_WINDOW
    यदि (in_echowindow)
    {
	यदि (popup_message_win_visible()
		    && ((संदेश_col > 0 && (संदेश_scroll || !full_screen))
			|| in_echowindow))
	{
	    win_T *wp = popup_get_message_win();

	    // start a new line
	    curbuf = wp->w_buffer;
	    ml_append(wp->w_buffer->b_ml.ml_पंक्ति_गिनती,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_buffer;
	}
	संदेश_col = 0;
    }
    अन्यथा
#endif
	यदि (!संदेश_scroll && full_screen)	// overwrite last message
    {
	संदेश_row = cmdline_row;
	संदेश_col =
#ifdef FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_width - 1 :
#endif
	    0;
    }
    अन्यथा यदि (संदेश_didout || in_echowindow)
    {
	// start message on next line
	संदेश_putchar('\n');
	did_return = TRUE;
	यदि (exmode_active != EXMODE_NORMAL)
	    cmdline_row = संदेश_row;
    }
    यदि (!संदेश_didany || पंक्तियाँ_बायाँ < 0)
	संदेश_starthere();
    यदि (संदेश_silent == 0)
    {
	संदेश_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    यदि (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    शून्य
संदेश_starthere(शून्य)
{
    पंक्तियाँ_बायाँ = cmdline_row;
    संदेश_didany = FALSE;
}

    शून्य
संदेश_putchar(पूर्णाङ्क c)
{
    संदेश_putchar_attr(c, 0);
}

    शून्य
संदेश_putchar_attr(पूर्णाङ्क c, पूर्णाङ्क attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    यदि (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    अन्यथा
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    संदेश_puts_attr((वर्ण *)buf, attr);
}

    शून्य
संदेश_outnum(दीर्घ n)
{
    वर्ण	buf[20];

    प्र_लिखो(buf, "%ld", n);
    संदेश_puts(buf);
}

    शून्य
संदेश_home_replace(char_u *fname)
{
    संदेश_home_replace_attr(fname, 0);
}

#यदि defined(FEAT_FIND_ID)
    शून्य
संदेश_home_replace_hl(char_u *fname)
{
    संदेश_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#endif

    स्थिर शून्य
संदेश_home_replace_attr(char_u *fname, पूर्णाङ्क attr)
{
    char_u	*name;

    name = home_replace_save(रिक्त, fname);
    यदि (name != रिक्त)
	संदेश_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    पूर्णाङ्क
संदेश_outtrans(char_u *str)
{
    प्रत्यागम संदेश_outtrans_attr(str, 0);
}

    पूर्णाङ्क
संदेश_outtrans_attr(char_u *str, पूर्णाङ्क attr)
{
    प्रत्यागम संदेश_outtrans_len_attr(str, (पूर्णाङ्क)STRLEN(str), attr);
}

    पूर्णाङ्क
संदेश_outtrans_len(char_u *str, पूर्णाङ्क len)
{
    प्रत्यागम संदेश_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
संदेश_outtrans_one(char_u *p, पूर्णाङ्क attr)
{
    पूर्णाङ्क		l;

    यदि (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	संदेश_outtrans_len_attr(p, l, attr);
	प्रत्यागम p + l;
    }
    संदेश_puts_attr((वर्ण *)transchar_byte_buf(रिक्त, *p), attr);
    प्रत्यागम p + 1;
}

    पूर्णाङ्क
संदेश_outtrans_len_attr(char_u *msgstr, पूर्णाङ्क len, पूर्णाङ्क attr)
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

    यदि (attr == 0)
	attr = HL_ATTR(HLF_संदेश);

    // if MSG_HIST flag set, add message to history
    यदि (attr & संदेश_HIST)
    {
	add_संदेश_hist(str, len, attr);
	attr &= ~संदेश_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    यदि (संदेश_silent == 0 && len > 0 && संदेश_row >= cmdline_row && संदेश_col == 0)
    {
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    यदि (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	संदेश_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    यावत् (--len >= 0 && !got_int)
    {
	यदि (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	अन्यथा यदि (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	अन्यथा
	    mb_l = 1;
	यदि (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    यदि (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    अन्यथा
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		यदि (str > plain_start)
		    संदेश_puts_attr_len((वर्ण *)plain_start,
					       (पूर्णाङ्क)(str - plain_start), attr);
		plain_start = str + mb_l;
		संदेश_puts_attr((वर्ण *)transchar_buf(रिक्त, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	अन्यथा
	{
	    s = transchar_byte_buf(रिक्त, *str);
	    यदि (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		यदि (str > plain_start)
		    संदेश_puts_attr_len((वर्ण *)plain_start,
					       (पूर्णाङ्क)(str - plain_start), attr);
		plain_start = str + 1;
		संदेश_puts_attr((वर्ण *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (पूर्णाङ्क)STRLEN(s);
	    }
	    अन्यथा
		++retval;
	    ++str;
	}
    }

    यदि (str > plain_start && !got_int)
	// print the printable chars at the end
	संदेश_puts_attr_len((वर्ण *)plain_start, (पूर्णाङ्क)(str - plain_start), attr);

    got_int |= save_got_int;

    प्रत्यागम retval;
}

#यदि defined(FEAT_QUICKFIX)
    शून्य
संदेश_make(char_u *arg)
{
    पूर्णाङ्क	    i;
    स्थिर char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    प्रति (i = 5; *arg && i >= 0; --i)
	यदि (*arg++ != str[i])
	    भङ्ग;
    यदि (i < 0)
    {
	संदेश_putchar('\n');
	प्रति (i = 0; rs[i]; ++i)
	    संदेश_putchar(rs[i] - 3);
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
संदेश_outtrans_special(
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
    यावत् (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	यदि ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    text = "<Space>";
	    ++str;
	}
	अन्यथा
	    text = (वर्ण *)str2special(&str, from, FALSE);
	यदि (text[0] != NUL && text[1] == NUL)
	    // single-byte character or illegal byte
	    text = (वर्ण *)transchar_byte_buf(रिक्त, (char_u)text[0]);
	len = vim_strsize((char_u *)text);
	यदि (maxlen > 0 && retval + len >= maxlen)
	    भङ्ग;
	// Highlight special keys
	संदेश_puts_attr(text, len > 1
		&& (*mb_ptr2len)((char_u *)text) <= 1 ? attr : 0);
	retval += len;
    }
    प्रत्यागम retval;
}

#यदि defined(FEAT_EVAL) || defined(FEAT_SPELL)
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
    यावत् (*p != NUL)
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

    यदि (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	यदि (p != रिक्त)
	    प्रत्यागम p;
    }

    c = *str;
    यदि ((c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	) && str[1] != NUL && str[2] != NUL)
    {
	यदि (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	यदि ((c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = TO_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	यदि (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    यदि (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	यदि (p != रिक्त)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	अन्यथा
	    // illegal byte
	    *sp = str + 1;
    }
    अन्यथा
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    यदि (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	प्रत्यागम get_special_कुंजी_name(c, modifiers);
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
    माप_प्रकार     buf_len = 0;
    माप_प्रकार     s_len;

    *buf = NUL;
    यावत् (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	यदि (buf_len + s_len < (माप_प्रकार)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	अन्यथा
	    भङ्ग;
    }
}

/*
 * print line for :print or :list command
 */
    शून्य
संदेश_prt_पंक्ति(char_u *s, पूर्णाङ्क list)
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

    यदि (curwin->w_p_list)
	list = TRUE;

    यदि (list)
    {
	// find start of trailing whitespace
	यदि (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    यावत् (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	यदि (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != रिक्त
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    यावत् (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    यदि (*lead == NUL)
		lead = रिक्त;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    यदि (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	संदेश_putchar(' ');

    यावत् (!got_int)
    {
	यदि (n_extra > 0)
	{
	    --n_extra;
	    यदि (n_extra == 0 && c_final)
		c = c_final;
	    अन्यथा यदि (c_extra)
		c = c_extra;
	    अन्यथा
		c = *p_extra++;
	}
	अन्यथा यदि (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    col += (*mb_ptr2cells)(s);
	    यदि (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    अन्यथा यदि (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		पूर्णाङ्क len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    अन्यथा
	    {
		mch_memmove(buf, s, (माप_प्रकार)l);
		buf[l] = NUL;
	    }
	    संदेश_puts((वर्ण *)buf);
	    s += l;
	    अनुवर्त;
	}
	अन्यथा
	{
	    attr = 0;
	    c = *s++;
	    यदि (list)
	    {
		in_multispace = c == ' ' && (*s == ' '
						 || (col > 0 && s[-2] == ' '));
		यदि (!in_multispace)
		    multispace_pos = 0;
	    }
	    यदि (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ifdef FEAT_VARTABS
		n_extra = tabstop_padding(col, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#अन्यथा
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
#endif
		यदि (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		अन्यथा
		{
		    पूर्णाङ्क lcs_tab1 = curwin->w_lcs_chars.tab1;
		    पूर्णाङ्क lcs_tab2 = curwin->w_lcs_chars.tab2;
		    पूर्णाङ्क lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    यदि (lead != रिक्त && s <= lead
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
	    अन्यथा यदि (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    अन्यथा यदि (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    अन्यथा यदि (c != NUL && (n = byte2cells(c)) > 1)
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
	    अन्यथा यदि (c == ' ')
	    {
		यदि (lead != रिक्त && s <= lead && in_multispace
			&& curwin->w_lcs_chars.leadmultispace != रिक्त)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    यदि (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		अन्यथा यदि (lead != रिक्त && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		अन्यथा यदि (trail != रिक्त && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		अन्यथा यदि (in_multispace
			&& curwin->w_lcs_chars.multispace != रिक्त)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    यदि (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		अन्यथा यदि (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	यदि (c == NUL)
	    भङ्ग;

	संदेश_putchar_attr(c, attr);
	col++;
    }
    संदेश_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    स्थिर char_u *
screen_puts_mbyte(char_u *s, पूर्णाङ्क l, पूर्णाङ्क attr)
{
    पूर्णाङ्क		cw;

    संदेश_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    यदि (cw > 1 && (
#ifdef FEAT_RIGHTLEFT
		cmdmsg_rl ? संदेश_col <= 1 :
#endif
		संदेश_col == cmdline_width - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	संदेश_screen_putchar('>', HL_ATTR(HLF_AT));
	प्रत्यागम s;
    }

    screen_puts_len(s, l, संदेश_row, cmdline_col_off + संदेश_col, attr);
#ifdef FEAT_RIGHTLEFT
    यदि (cmdmsg_rl)
    {
	संदेश_col -= cw;
	यदि (संदेश_col == 0)
	{
	    संदेश_col = cmdline_width;
	    ++संदेश_row;
	}
    }
    अन्यथा
#endif
    {
	संदेश_col += cw;
	यदि (संदेश_col >= cmdline_width)
	{
	    संदेश_col = 0;
	    ++संदेश_row;
	}
    }
    प्रत्यागम s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    शून्य
संदेश_puts(वर्ण *s)
{
    संदेश_puts_attr(s, 0);
}

    शून्य
संदेश_puts_title(वर्ण *s)
{
    संदेश_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    स्थिर शून्य
संदेश_outtrans_long_len_attr(char_u *longstr, पूर्णाङ्क len, पूर्णाङ्क attr)
{
    पूर्णाङ्क		slen = len;
    पूर्णाङ्क		room;

    room = cmdline_width - संदेश_col;
    यदि (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	संदेश_outtrans_len_attr(longstr, slen, attr);
	संदेश_puts_attr("...", HL_ATTR(HLF_8));
    }
    संदेश_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    शून्य
संदेश_outtrans_long_attr(char_u *longstr, पूर्णाङ्क attr)
{
    संदेश_outtrans_long_len_attr(longstr, (पूर्णाङ्क)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    शून्य
संदेश_puts_attr(वर्ण *s, पूर्णाङ्क attr)
{
    संदेश_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    स्थिर शून्य
संदेश_puts_attr_len(वर्ण *str, पूर्णाङ्क maxlen, पूर्णाङ्क attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    यदि (संदेश_silent != 0)
	प्रत्यागम;

    यदि (attr == 0)
	attr = HL_ATTR(HLF_संदेश);

    // if MSG_HIST flag set, add message to history
    यदि ((attr & संदेश_HIST) && maxlen < 0)
    {
	add_संदेश_hist((char_u *)str, -1, attr);
	attr &= ~संदेश_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    यदि (संदेश_scrolled != 0 && !संदेश_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    संदेश_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    यदि (संदेश_use_printf())
	संदेश_puts_printf((char_u *)str, maxlen);
    अन्यथा
	संदेश_puts_display((char_u *)str, maxlen, attr, FALSE);

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
put_संदेश_win(win_T *wp, पूर्णाङ्क where, char_u *t_s, char_u *end, linenr_T lnum)
{
    char_u  *p;

    यदि (where == PUT_BELOW)
    {
	यदि (*end != NUL)
	{
	    p = vim_strnsave(t_s, end - t_s);
	    यदि (p == रिक्त)
		प्रत्यागम;
	}
	अन्यथा
	    p = t_s;
	ml_append_buf(wp->w_buffer, lnum, p, (colnr_T)0, FALSE);
	यदि (p != t_s)
	    vim_free(p);
    }
    अन्यथा
    {
	char_u *newp;

	curbuf = wp->w_buffer;
	यदि (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    यदि (newp == रिक्त)
		प्रत्यागम;
	    यदि (*end != NUL)
		newp[STRLEN(ml_get(lnum)) + (end - t_s)] = NUL;
	}
	अन्यथा
	{
	    newp = vim_strnsave(t_s, end - t_s);
	    यदि (newp == रिक्त)
		प्रत्यागम;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_buffer;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    संदेश_col += (पूर्णाङ्क)(end - t_s);
}
#endif

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    स्थिर शून्य
संदेश_puts_display(
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
    पूर्णाङ्क		sb_col = संदेश_col;
    पूर्णाङ्क		wrap;
    पूर्णाङ्क		did_last_char;
#ifdef HAS_MESSAGE_WINDOW
    पूर्णाङ्क		where = PUT_APPEND;
    win_T	*संदेश_win = रिक्त;
    linenr_T    lnum = 1;

    यदि (in_echowindow)
    {
	संदेश_win = popup_get_message_win();

	यदि (संदेश_win != रिक्त)
	{
	    यदि (!popup_message_win_visible())
	    {
		यदि (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    संदेश_win = रिक्त;
		}
		अन्यथा
		{
		    // currently hidden, make it empty
		    curbuf = संदेश_win->w_buffer;
		    यावत् ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
			ml_delete(1);
		    curbuf = curwin->w_buffer;
		}
	    }
	    अन्यथा
	    {
		lnum = संदेश_win->w_buffer->b_ml.ml_पंक्ति_गिनती;
		यदि (संदेश_col == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#endif

    did_wait_return = FALSE;
    यावत् ((maxlen < 0 || (पूर्णाङ्क)(s - str) < maxlen) && *s != NUL)
    {
#ifdef HAS_MESSAGE_WINDOW
	// For echowindow, use full width; for regular messages, leave last column
	पूर्णाङ्क wrap_col = संदेश_win != रिक्त ? cmdline_width : cmdline_width - 1;
#अन्यथा
	पूर्णाङ्क wrap_col = cmdline_width - 1;
#endif

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	यदि (!recurse && संदेश_row >= Rows - 1 && (*s == '\n' || (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			संदेश_col <= 1
		      || (*s == TAB && संदेश_col <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && संदेश_col <= 2))
		    :
#endif
		      ((*s != '\r' && संदेश_col + t_col >= wrap_col)
		       || (*s == TAB && संदेश_col + t_col
			   >= (wrap_col & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && संदेश_col + t_col >= wrap_col - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    यदि (t_col > 0)
	    {
		// output postponed text
#ifdef HAS_MESSAGE_WINDOW
		यदि (संदेश_win != रिक्त)
		{
		    put_संदेश_win(संदेश_win, where, t_s, s, lnum);
		    यदि (where == PUT_BELOW)
			++lnum;
		    t_col = 0;
		    where = PUT_BELOW;
		}
		अन्यथा
#endif
		    t_puts(&t_col, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    यदि (संदेश_no_more && पंक्तियाँ_बायाँ == 0)
		भङ्ग;

#ifdef HAS_MESSAGE_WINDOW
	    यदि (संदेश_win == रिक्त)
#endif
		// Scroll the screen up one line.
		संदेश_scroll_up();

#ifdef HAS_MESSAGE_WINDOW
	    यदि (संदेश_win == रिक्त)
	    {
#endif
		संदेश_row = Rows - 2;
		यदि (संदेश_col >= cmdline_width)  // can happen after screen resize
		    संदेश_col = cmdline_width - 1;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif

	    // Display char in last column before showing more-prompt.
	    यदि (*s >= ' '
#ifdef FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#endif
	       )
	    {
		यदि (has_mbyte)
		{
		    यदि (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (पूर्णाङ्क)((str + maxlen) - s));
		    अन्यथा
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		अन्यथा
		    संदेश_screen_putchar(*s++, attr);
		did_last_char = TRUE;
	    }
	    अन्यथा
		did_last_char = FALSE;

	    यदि (p_more)
		// store text for scrolling back
		store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

#ifdef HAS_MESSAGE_WINDOW
	    यदि (संदेश_win == रिक्त)
	    {
#endif
		inc_संदेश_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		यदि (cmdline_row > 0 && !exmode_active)
		    --cmdline_row;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		यदि (पंक्तियाँ_बायाँ > 0)
		    --पंक्तियाँ_बायाँ;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif
	    यदि (p_more && पंक्तियाँ_बायाँ == 0 && State != MODE_HITRETURN
					    && !संदेश_no_more && !exmode_active)
	    {
#ifdef FEAT_CON_DIALOG
		यदि (do_more_prompt(NUL))
		    s = confirm_संदेश_tail;
#अन्यथा
		(शून्य)do_more_prompt(NUL);
#endif
		यदि (quit_more)
		    प्रत्यागम;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    यदि (did_last_char)
		अनुवर्त;
	}

	wrap = *s == '\n'
		    || संदेश_col + t_col >= wrap_col
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && संदेश_col + t_col >= wrap_col - 1);
	यदि (t_col > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ifdef HAS_MESSAGE_WINDOW
	    यदि (संदेश_win != रिक्त)
	    {
		put_संदेश_win(संदेश_win, where, t_s, s, lnum);
		यदि (where == PUT_BELOW)
		    ++lnum;
		t_col = 0;
		where = PUT_BELOW;
		// Reset msg_col after outputting to new line in echowindow
		यदि (wrap)
		    संदेश_col = 0;
	    }
	    अन्यथा
#endif
		t_puts(&t_col, t_s, s, attr);
	}

	यदि (wrap && p_more && !recurse)
	    // store text for scrolling back
	    store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	यदि (*s == '\n')		    // go to next line
	{
#ifdef HAS_MESSAGE_WINDOW
	    यदि (संदेश_win != रिक्त)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		यदि ((संदेश_win->w_buffer->b_ml.ml_flags & ML_EMPTY) == 0)
		{
		    put_संदेश_win(संदेश_win, PUT_BELOW, t_s, t_s, lnum);
		    ++lnum;
		}
		संदेश_col = 0;  // Reset column for new line
	    }
	    अन्यथा
#endif
		संदेश_didout = FALSE;	    // remember that line is empty
#ifdef FEAT_RIGHTLEFT
	    यदि (cmdmsg_rl)
		संदेश_col = cmdline_width - 1;
	    अन्यथा
#endif
		संदेश_col = 0;
	    यदि (++संदेश_row >= Rows)  // safety check
		संदेश_row = Rows - 1;
	}
	अन्यथा यदि (*s == '\r')	    // go to column 0
	{
	    संदेश_col = 0;
#ifdef HAS_MESSAGE_WINDOW
	    where = PUT_TRUNC;
#endif
	}
	अन्यथा यदि (*s == '\b')	    // go to previous char
	{
	    यदि (संदेश_col)
		--संदेश_col;
	}
	अन्यथा यदि (*s == TAB)	    // translate Tab into spaces
	{
#ifdef HAS_MESSAGE_WINDOW
	    यदि (संदेश_win != रिक्त)
		संदेश_col = (संदेश_col + 7) % 8;
	    अन्यथा
#endif
		कुरु
		    संदेश_screen_putchar(' ', attr);
		यावत् (संदेश_col & 7);
	}
	अन्यथा यदि (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	अन्यथा
	{
	    यदि (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		यदि (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (पूर्णाङ्क)((str + maxlen) - s));
		अन्यथा
		    l = (*mb_ptr2len)(s);
	    }
	    अन्यथा
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    यदि (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#endif
		    (cw > 1 && संदेश_col + t_col >= wrap_col))
	    {
		यदि (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		अन्यथा
		    संदेश_screen_putchar(*s, attr);
	    }
	    अन्यथा
	    {
		// postpone this character until later
		यदि (t_col == 0)
		    t_s = s;
		t_col += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    यदि (t_col > 0)
    {
#ifdef HAS_MESSAGE_WINDOW
	यदि (संदेश_win != रिक्त)
	    put_संदेश_win(संदेश_win, where, t_s, s, lnum);
	अन्यथा
#endif
	    t_puts(&t_col, t_s, s, attr);
    }

#ifdef HAS_MESSAGE_WINDOW
    यदि (संदेश_win != रिक्त)
	popup_show_message_win();
#endif
    // Store the text for scroll back.
    यदि (p_more && !recurse)
	store_sb_text(&sb_str, s, attr, &sb_col, FALSE);

    संदेश_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    पूर्णाङ्क
message_filtered(char_u *संदेश)
{
    पूर्णाङ्क match;

    यदि (cmdmod.cmod_filter_regmatch.regprog == रिक्त)
	प्रत्यागम FALSE;
    match = vim_regexec(&cmdmod.cmod_filter_regmatch, संदेश, (colnr_T)0);
    प्रत्यागम cmdmod.cmod_filter_force ? match : !match;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    स्थिर शून्य
संदेश_scroll_up(शून्य)
{
#ifdef HAS_MESSAGE_WINDOW
    यदि (in_echowindow)
	प्रत्यागम;
#endif
#ifdef FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    यदि (gui.in_use)
	gui_undraw_cursor();
#endif
    // scrolling up always works
    mch_disable_flush();
    screen_del_पंक्तियाँ(0, 0, 1, (पूर्णाङ्क)Rows, TRUE, 0, रिक्त);
    mch_enable_flush();

    यदि (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((पूर्णाङ्क)Rows - 1, (पूर्णाङ्क)Rows, cmdline_col_off,
		cmdline_col_off + cmdline_width, ' ', ' ', HL_ATTR(HLF_संदेश));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	यदि (ScreenAttrs[पंक्ति_Offset[Rows - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((पूर्णाङ्क)Rows - 2, (पूर्णाङ्क)Rows - 1,
				 (पूर्णाङ्क)Columns - 1, (पूर्णाङ्क)Columns,
				 ' ', ' ', HL_ATTR(HLF_संदेश));
    }
}

/*
 * Increment "msg_scrolled".
 */
    स्थिर शून्य
inc_संदेश_scrolled(शून्य)
{
#ifdef FEAT_EVAL
    यदि (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = रिक्त;

	// v:scrollstart is empty, set it to the script/function name and line
	//संख्या
	यदि (p.string == रिक्त)
	{
	    p.string = (char_u *)_("Unknown");
	    p.length = STRLEN(p.string);
	}
	अन्यथा
	{
	    माप_प्रकार  tofreesize;

	    tofreesize = (पूर्णाङ्क)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    यदि (tofree != रिक्त)
	    {
		p.length = vim_snprintf_safelen((वर्ण *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (दीर्घ)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	set_vim_var_string(VV_SCROLLSTART, p.string, (पूर्णाङ्क)p.length);
	vim_free(tofree);
    }
#endif
    ++संदेश_scrolled;
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
    पूर्णाङ्क		sb_संदेश_col;	// column in which text starts
    पूर्णाङ्क		sb_attr;	// text attributes
    char_u	sb_text[1];	// text to be displayed, actually longer
};

स्थिर msgchunk_T *last_msgchunk = रिक्त; // last displayed text

स्थिर msgchunk_T *संदेश_sb_start(msgchunk_T *mps);

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

    यदि (do_clear_sb_text == SB_CLEAR_ALL
	    || do_clear_sb_text == SB_CLEAR_CMDLINE_DONE)
    {
	clear_sb_text(do_clear_sb_text == SB_CLEAR_ALL);
	संदेश_sb_eol();  // prevent messages from overlapping
	do_clear_sb_text = SB_CLEAR_NONE;
    }

    यदि (s > *sb_str)
    {
	mp = alloc(दुरत्व(msgchunk_T, sb_text) + (s - *sb_str) + 1);
	यदि (mp != रिक्त)
	{
	    mp->sb_eol = finish;
	    mp->sb_संदेश_col = *sb_col;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_text, *sb_str, s - *sb_str);

	    यदि (last_msgchunk == रिक्त)
	    {
		last_msgchunk = mp;
		mp->sb_prev = रिक्त;
	    }
	    अन्यथा
	    {
		mp->sb_prev = last_msgchunk;
		last_msgchunk->sb_next = mp;
		last_msgchunk = mp;
	    }
	    mp->sb_next = रिक्त;
	}
    }
    अन्यथा यदि (finish && last_msgchunk != रिक्त)
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
    यदि (do_clear_sb_text == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_text_restart_cmdline();
    अन्यथा
    {
	संदेश_sb_eol();
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

    यदि (last_msgchunk == रिक्त || last_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	प्रत्यागम;

    tofree = संदेश_sb_start(last_msgchunk);
    last_msgchunk = tofree->sb_prev;
    यदि (last_msgchunk != रिक्त)
	last_msgchunk->sb_next = रिक्त;
    यावत् (tofree != रिक्त)
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

    यदि (all)
	lastp = &last_msgchunk;
    अन्यथा
    {
	यदि (last_msgchunk == रिक्त)
	    प्रत्यागम;
	lastp = &संदेश_sb_start(last_msgchunk)->sb_prev;
    }

    यावत् (*lastp != रिक्त)
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
    mp = संदेश_sb_start(last_msgchunk);
    यदि (mp == रिक्त || mp->sb_prev == रिक्त)
	vim_beep(BO_MESS);
    अन्यथा
    {
	do_more_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    स्थिर msgchunk_T *
संदेश_sb_start(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    यावत् (mp != रिक्त && mp->sb_prev != रिक्त && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    प्रत्यागम mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    शून्य
संदेश_sb_eol(शून्य)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    यदि (last_msgchunk != रिक्त
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
disp_sb_पंक्ति(पूर्णाङ्क row, msgchunk_T *smp, पूर्णाङ्क clear_to_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    प्रति (;;)
    {
	संदेश_row = row;
	संदेश_col = mp->sb_संदेश_col;
	p = mp->sb_text;
	यदि (*p == '\n')	    // don't display the line break
	    ++p;
	संदेश_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	यदि (clear_to_eol)
	    screen_fill(row, row + 1,
		    cmdline_col_off + संदेश_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', HL_ATTR(HLF_संदेश));

	यदि (mp->sb_eol || mp->sb_next == रिक्त)
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
    संदेश_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (पूर्णाङ्क)(s - t_s), संदेश_row, cmdline_col_off + संदेश_col,
	    attr);
    संदेश_col += *t_col;
    *t_col = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    यदि (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--संदेश_col;
    यदि (संदेश_col >= cmdline_width)
    {
	संदेश_col = 0;
	++संदेश_row;
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
संदेश_use_printf(शून्य)
{
    प्रत्यागम (!संदेश_check_screen()
#यदि defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.in_use && !termcap_active)
# अन्यथा
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
संदेश_puts_printf(char_u *str, पूर्णाङ्क maxlen)
{
    char_u	*s = str;
    char_u	*buf = रिक्त;
    char_u	*p = s;

#ifdef MSWIN
    यदि (!(silent_mode && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#endif
    यावत् ((maxlen < 0 || (पूर्णाङ्क)(s - str) < maxlen) && *s != NUL)
    {
	यदि (!(silent_mode && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    यदि (*s == NL)
	    {
		पूर्णाङ्क n = (पूर्णाङ्क)(s - p);

		buf = alloc(n + 3);
		यदि (buf != रिक्त)
		{
		    स_नकल(buf, p, n);
		    यदि (!info_message)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    यदि (info_message)   // informative message, not an error
			mch_संदेश((वर्ण *)buf);
		    अन्यथा
			mch_errmsg((वर्ण *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ifdef FEAT_RIGHTLEFT
	यदि (cmdmsg_rl)
	{
	    यदि (*s == CAR || *s == NL)
		संदेश_col = cmdline_width - 1;
	    अन्यथा
		--संदेश_col;
	}
	अन्यथा
#endif
	{
	    यदि (*s == CAR || *s == NL)
		संदेश_col = 0;
	    अन्यथा
		++संदेश_col;
	}
	++s;
    }

    यदि (*p != NUL && !(silent_mode && p_verbose == 0))
    {
	char_u *tofree = रिक्त;

	यदि (maxlen > 0 && vim_strlen_maxlen((वर्ण *)p, (माप_प्रकार)maxlen)
							     >= (माप_प्रकार)maxlen)
	{
	    tofree = vim_strnsave(p, (माप_प्रकार)maxlen);
	    p = tofree;
	}
	यदि (p != रिक्त)
	{
	    यदि (info_message)
		mch_संदेश((वर्ण *)p);
	    अन्यथा
		mch_errmsg((वर्ण *)p);
	    vim_free(tofree);
	}
    }

    संदेश_didout = TRUE;	    // assume that line is not empty

#ifdef MSWIN
    यदि (!(silent_mode && p_verbose == 0))
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
    पूर्णाङ्क		संदेश_attr;

    संदेश_attr = HL_ATTR(HLF_संदेश);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    यदि (entered || (State == MODE_HITRETURN && typed_char == 0))
	प्रत्यागम FALSE;
    entered = TRUE;

    यदि (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_last = संदेश_sb_start(last_msgchunk);
	प्रति (i = 0; i < Rows - 2 && mp_last != रिक्त
					     && mp_last->sb_prev != रिक्त; ++i)
	    mp_last = संदेश_sb_start(mp_last->sb_prev);
    }

    State = MODE_ASKMORE;
    setmouse();
    यदि (typed_char == NUL)
	संदेश_moremsg(FALSE);
    प्रति (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	यदि (used_typed_char != NUL)
	{
	    c = used_typed_char;	// was typed at hit-enter prompt
	    used_typed_char = NUL;
	}
	अन्यथा
	    c = get_keystroke();

#यदि defined(FEAT_MENU) && defined(FEAT_GUI)
	यदि (c == K_MENU)
	{
	    पूर्णाङ्क idx = get_menu_index(current_menu, MODE_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    यदि (idx == MENU_INDEX_INVALID)
		अनुवर्त;
	    c = *current_menu->strings[idx];
	    यदि (c != NUL && current_menu->strings[idx][1] != NUL)
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
	    पंक्तियाँ_बायाँ = 999999;
	    भङ्ग;

	पक्ष ':':		// start new command line
#ifdef FEAT_CON_DIALOG
	    यदि (!confirm_संदेश_used)
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
	    यदि (confirm_संदेश_used)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    अन्यथा
#endif
	    {
		got_int = TRUE;
		quit_more = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    पंक्तियाँ_बायाँ = Rows - 1;
	    भङ्ग;

#ifdef FEAT_CLIPBOARD
	पक्ष Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    यदि (clip_star.state == SELECT_DONE)
		clip_copy_modeless_selection(TRUE);
	    अनुवर्त;
#endif
	स्वभाव:		// no valid response
	    संदेश_moremsg(TRUE);
	    अनुवर्त;
	}

	यदि (toscroll != 0)
	{
	    यदि (toscroll < 0)
	    {
		// go to start of last line
		यदि (mp_last == रिक्त)
		    mp = संदेश_sb_start(last_msgchunk);
		अन्यथा यदि (mp_last->sb_prev != रिक्त)
		    mp = संदेश_sb_start(mp_last->sb_prev);
		अन्यथा
		    mp = रिक्त;

		// go to start of line at top of the screen
		प्रति (i = 0; i < Rows - 2 && mp != रिक्त && mp->sb_prev != रिक्त;
									  ++i)
		    mp = संदेश_sb_start(mp->sb_prev);

		यदि (mp != रिक्त && mp->sb_prev != रिक्त)
		{
		    // Find line to be displayed at top.
		    प्रति (i = 0; i > toscroll; --i)
		    {
			यदि (mp == रिक्त || mp->sb_prev == रिक्त)
			    भङ्ग;
			mp = संदेश_sb_start(mp->sb_prev);
			यदि (mp_last == रिक्त)
			    mp_last = संदेश_sb_start(last_msgchunk);
			अन्यथा
			    mp_last = संदेश_sb_start(mp_last->sb_prev);
		    }

		    यदि (toscroll == -1 && screen_ins_पंक्तियाँ(0, 0, 1,
						     (पूर्णाङ्क)Rows, 0, रिक्त) == OK)
		    {
			// display line at top
			(शून्य)disp_sb_पंक्ति(0, mp, FALSE);
		    }
		    अन्यथा
		    {
			पूर्णाङ्क did_clear = screenclear();

			// redisplay all lines
			प्रति (i = 0; mp != रिक्त && i < Rows - 1; ++i)
			{
			    mp = disp_sb_पंक्ति(i, mp, !did_clear);
			    ++संदेश_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    अन्यथा
	    {
		// First display any text that we scrolled back.
		यावत् (toscroll > 0 && mp_last != रिक्त)
		{
		    // scroll up, display line at bottom
		    संदेश_scroll_up();
		    inc_संदेश_scrolled();
		    screen_fill((पूर्णाङ्क)Rows - 2, (पूर्णाङ्क)Rows - 1,
			    cmdline_col_off, cmdline_col_off + cmdline_width,
			    ' ', ' ', संदेश_attr);
		    mp_last = disp_sb_पंक्ति((पूर्णाङ्क)Rows - 2, mp_last, FALSE);
		    --toscroll;
		}
	    }

	    यदि (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((पूर्णाङ्क)Rows - 1, (पूर्णाङ्क)Rows, cmdline_col_off,
			cmdline_col_off + cmdline_width, ' ', ' ', संदेश_attr);
		संदेश_moremsg(FALSE);
		अनुवर्त;
	    }

	    // display more text, return to caller
	    पंक्तियाँ_बायाँ = toscroll;
	}

	भङ्ग;
    }

    // clear the --more-- message
    screen_fill((पूर्णाङ्क)Rows - 1, (पूर्णाङ्क)Rows, cmdline_col_off,
	    cmdline_col_off + cmdline_width, ' ', ' ', संदेश_attr);
    State = oldState;
    setmouse();
    यदि (quit_more)
    {
	संदेश_row = Rows - 1;
	संदेश_col = 0;
    }
#ifdef FEAT_RIGHTLEFT
    अन्यथा यदि (cmdmsg_rl)
	संदेश_col = cmdline_width - 1;
#endif

    entered = FALSE;
#ifdef FEAT_CON_DIALOG
    प्रत्यागम retval;
#अन्यथा
    प्रत्यागम FALSE;
#endif
}

#यदि defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_संदेश
#  undef mch_संदेश
# endif

# यदि defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    स्थिर शून्य
mch_errmsg_c(वर्ण *str)
{
    DWORD   nwrite = 0;
    DWORD   mode = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    यदि (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (पूर्णाङ्क)GetConsoleCP() != enc_codepage)
    {
	पूर्णाङ्क	len = (पूर्णाङ्क)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	यदि (w != रिक्त)
	{
	    WriteConsoleW(h, w, len, &nwrite, रिक्त);
	    vim_free(w);
	}
    }
    अन्यथा
    {
	ख_लिखो(मानक_त्रुटि, "%s", str);
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
# यदि !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    पूर्णाङ्क		len;
# endif

# यदि (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    यदि (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && म_भेद("/dev/console", ttyname(2)) != 0)
#   अन्यथा
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
	ख_लिखो(मानक_त्रुटि, "%s", str);
	प्रत्यागम;
    }
# endif

# यदि defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    यदि (!(gui.in_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	प्रत्यागम;
    }
# endif

# यदि !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (पूर्णाङ्क)STRLEN(str) + 1;
    यदि (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    यदि (ga_grow(&error_ga, len) == OK)
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
		यदि (p == रिक्त)
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

# यदि defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    स्थिर शून्य
mch_संदेश_c(वर्ण *str)
{
    DWORD   nwrite = 0;
    DWORD   mode;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    यदि (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (पूर्णाङ्क)GetConsoleCP() != enc_codepage)
    {
	पूर्णाङ्क	len = (पूर्णाङ्क)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	यदि (w != रिक्त)
	{
	    WriteConsoleW(h, w, len, &nwrite, रिक्त);
	    vim_free(w);
	}
    }
    अन्यथा
    {
	म_लिखो("%s", str);
    }
}
# endif

/*
 * Give a message.  To be used when the screen hasn't been initialized yet.
 * When there is no tty, collect messages until the GUI has started and they
 * can be displayed in a message box.
 */
    शून्य
mch_संदेश(वर्ण *str)
{
# यदि (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    यदि (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && म_भेद("/dev/console", ttyname(2)) != 0)
#   अन्यथा
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
	म_लिखो("%s", str);
	प्रत्यागम;
    }
# endif

# यदि defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    यदि (!(gui.in_use || gui.starting))
#  endif
    {
	mch_संदेश_c(str);
	प्रत्यागम;
    }
# endif
# यदि !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#endif // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    स्थिर शून्य
संदेश_screen_putchar(पूर्णाङ्क c, पूर्णाङ्क attr)
{
    संदेश_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, संदेश_row, cmdline_col_off + संदेश_col, attr);
#ifdef FEAT_RIGHTLEFT
    यदि (cmdmsg_rl)
    {
	यदि (--संदेश_col == 0)
	{
	    संदेश_col = cmdline_width;
	    ++संदेश_row;
	}
    }
    अन्यथा
#endif
    {
	यदि (++संदेश_col >= cmdline_width)
	{
	    संदेश_col = 0;
	    ++संदेश_row;
	}
    }
}

    स्थिर शून्य
संदेश_moremsg(पूर्णाङ्क full)
{
    पूर्णाङ्क		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (पूर्णाङ्क)Rows - 1, cmdline_col_off, attr);
    यदि (full)
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
    यदि (State == MODE_ASKMORE)
    {
	संदेश_moremsg(TRUE);	// display --more-- message again
	संदेश_row = Rows - 1;
    }
#ifdef FEAT_CON_DIALOG
    अन्यथा यदि (State == MODE_CONFIRM)
    {
	display_confirm_संदेश();	// display ":confirm" message again
	संदेश_row = Rows - 1;
    }
#endif
    अन्यथा यदि (State == MODE_EXTERNCMD)
    {
	windgoto(संदेश_row, cmdline_col_off + संदेश_col); // put cursor back
    }
    अन्यथा यदि (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	यदि (संदेश_row == Rows - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    संदेश_didout = FALSE;
	    संदेश_col = 0;
	    संदेश_clr_eos();
	}
	hit_return_संदेश();
	संदेश_row = Rows - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    स्थिर पूर्णाङ्क
संदेश_check_screen(शून्य)
{
    यदि (!full_screen || !screen_valid(FALSE))
	प्रत्यागम FALSE;

    यदि (संदेश_row >= Rows)
	संदेश_row = Rows - 1;
    यदि (संदेश_col >= cmdline_width)
	संदेश_col = cmdline_width - 1;
    प्रत्यागम TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    शून्य
संदेश_clr_eos(शून्य)
{
    यदि (संदेश_silent == 0)
	संदेश_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    शून्य
संदेश_clr_eos_force(शून्य)
{
#ifdef HAS_MESSAGE_WINDOW
    यदि (in_echowindow)
	प्रत्यागम;  // messages go into a popup
#endif
    यदि (संदेश_use_printf())
    {
	यदि (full_screen)	// only when termcap codes are valid
	{
	    यदि (*T_CD)
		out_str(T_CD);	// clear to end of display
	    अन्यथा यदि (*T_CE)
		out_str(T_CE);	// clear to end of line
	}
    }
    अन्यथा
    {
	पूर्णाङ्क संदेश_attr;

	संदेश_attr = HL_ATTR(HLF_संदेश);

#ifdef FEAT_RIGHTLEFT
	यदि (cmdmsg_rl)
	{
	    screen_fill(संदेश_row, संदेश_row + 1,
		    cmdline_col_off, cmdline_col_off + संदेश_col + 1,
		    ' ', ' ', संदेश_attr);
	    screen_fill(संदेश_row + 1, (पूर्णाङ्क)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', संदेश_attr);
	}
	अन्यथा
#endif
	{
	    screen_fill(संदेश_row, संदेश_row + 1,
		    cmdline_col_off + संदेश_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', संदेश_attr);
	    screen_fill(संदेश_row + 1, (पूर्णाङ्क)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', संदेश_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    शून्य
संदेश_clr_cmdline(शून्य)
{
    संदेश_row = cmdline_row;
    संदेश_col = 0;
    संदेश_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    पूर्णाङ्क
संदेश_end(शून्य)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    यदि (!exiting && need_wait_return && !(State & MODE_CMDLINE))
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
संदेश_check(शून्य)
{
    यदि (संदेश_row == Rows - 1 && संदेश_col >= sc_col
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
    यदि (redir_off)
	प्रत्यागम;

    // If 'verbosefile' is set prepare for writing in that file.
    यदि (*p_vfile != NUL && verbose_fd == रिक्त)
	verbose_open();

    यदि (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	यदि (*s != '\n' && *s != '\r')
	{
	    यावत् (cur_col < संदेश_col)
	    {
#ifdef FEAT_EVAL
		यदि (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		अन्यथा यदि (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		अन्यथा यदि (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		अन्यथा
#endif
		    यदि (redir_fd != रिक्त)
		    ख_माला_दो(" ", redir_fd);
		यदि (verbose_fd != रिक्त)
		    ख_माला_दो(" ", verbose_fd);
		++cur_col;
	    }
	}

#ifdef FEAT_EVAL
	यदि (redir_execute)
	    execute_redir_str(s, maxlen);
	अन्यथा यदि (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	अन्यथा यदि (redir_vname)
	    var_redir_str(s, maxlen);
#endif

	// Write and adjust the current column.
	यावत् (*s != NUL && (maxlen < 0 || (पूर्णाङ्क)(s - str) < maxlen))
	{
#ifdef FEAT_EVAL
	    यदि (!redir_reg && !redir_vname && !redir_execute)
#endif
		यदि (redir_fd != रिक्त)
		    अ_दो(*s, redir_fd);
	    यदि (verbose_fd != रिक्त)
		अ_दो(*s, verbose_fd);
	    यदि (*s == '\r' || *s == '\n')
		cur_col = 0;
	    अन्यथा यदि (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    अन्यथा
		++cur_col;
	    ++s;
	}

	यदि (संदेश_silent != 0)	// should update msg_col
	    संदेश_col = cur_col;
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
    यदि (*p_vfile != NUL)
	++संदेश_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    शून्य
verbose_leave(शून्य)
{
    यदि (*p_vfile != NUL)
	यदि (--संदेश_silent < 0)
	    संदेश_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    शून्य
verbose_enter_scroll(शून्य)
{
    यदि (*p_vfile != NUL)
	++संदेश_silent;
    अन्यथा
	// always scroll up, don't overwrite
	संदेश_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    शून्य
verbose_leave_scroll(शून्य)
{
    यदि (*p_vfile != NUL)
    {
	यदि (--संदेश_silent < 0)
	    संदेश_silent = 0;
    }
    अन्यथा
	cmdline_row = संदेश_row;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    शून्य
verbose_stop(शून्य)
{
    यदि (verbose_fd != रिक्त)
    {
	ख_बंद(verbose_fd);
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
    यदि (verbose_fd == रिक्त && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((वर्ण *)p_vfile, "a");
	यदि (verbose_fd == रिक्त)
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
    यदि (संदेश_silent != 0)
	प्रत्यागम;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, message, -1);
#endif
    VIM_CLEAR(keep_संदेश);
    यदि (hl)
	keep_संदेश_attr = HL_ATTR(HLF_W);
    अन्यथा
	keep_संदेश_attr = 0;

    यदि (with_source)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	संदेश_start();
	संदेश_source(HL_ATTR(HLF_W));
	संदेश_puts(" ");
	संदेश_puts_attr((वर्ण *)message, HL_ATTR(HLF_W) | संदेश_HIST);
	संदेश_clr_eos();
	(शून्य)संदेश_end();
    }
    अन्यथा यदि (संदेश_attr((वर्ण *)message, keep_संदेश_attr) && संदेश_scrolled == 0)
	set_keep_संदेश(message, keep_संदेश_attr);

    संदेश_didout = FALSE;	    // overwrite this message
    संदेश_nowait = TRUE;	    // don't wait for this message
    संदेश_col = 0;

    --no_wait_return;
}

#यदि defined(FEAT_EVAL)
    शून्य
give_warning2(char_u *message, char_u *a1, पूर्णाङ्क hl)
{
    यदि (IObuff == रिक्त)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(message, hl);
    }
    अन्यथा
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
संदेश_advance(पूर्णाङ्क col)
{
    यदि (संदेश_silent != 0)	// nothing to advance to
    {
	संदेश_col = col;		// for redirection, may fill it up later
	प्रत्यागम;
    }
    यदि (col >= cmdline_width)	// not enough room
	col = cmdline_width - 1;
#ifdef FEAT_RIGHTLEFT
    यदि (cmdmsg_rl)
	यावत् (संदेश_col > cmdline_width - col)
	    संदेश_putchar(' ');
    अन्यथा
#endif
	यावत् (संदेश_col < col)
	    संदेश_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    शून्य
संदेश_warn_missing_clipboard(शून्य)
{
    यदि (!global_busy && !did_warn_clipboard && silence_w23_w24_संदेश == 0)
    {
#ifdef FEAT_CLIPBOARD
	संदेश(_("W23: Clipboard register not available, using register 0"));
#अन्यथा
	संदेश(_("W24: Clipboard register not available. See :h W24"));
#endif
	did_warn_clipboard = TRUE;
    }
}

#यदि defined(FEAT_CON_DIALOG)
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
    यदि (silent_mode)
	प्रत्यागम dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    यदि (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == रिक्त)
    {
	// --gui-dialog-file: write text to a file
	यदि (gui_dialog_log(title, message))
	    c = dfltbutton;
	अन्यथा
	    c = gui_mch_dialog(type, title, message, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_row = संदेश_row;

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
    hotkeys = संदेश_show_console_dialog(message, buttons, dfltbutton);

    यदि (hotkeys != रिक्त)
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
		यदि (c < 0)	// special keys are ignored here
		    अनुवर्त;
		यदि (c == ':' && ex_cmd)
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
		    यदि (has_mbyte)
		    {
			यदि ((*mb_ptr2char)(hotkeys + i) == c)
			    भङ्ग;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    अन्यथा
			यदि (hotkeys[i] == c)
			    भङ्ग;
		    ++retval;
		}
		यदि (hotkeys[i])
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
    संदेश_end_prompt();

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

    यदि (has_mbyte)
    {
	यदि (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    प्रत्यागम (*mb_char2bytes)(c, to);
	}
	अन्यथा
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(to, from, (माप_प्रकार)len);
	    प्रत्यागम len;
	}
    }
    अन्यथा
    {
	यदि (lowercase)
	    *to = (char_u)TOLOWER_LOC(*from);
	अन्यथा
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
संदेश_show_console_dialog(
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
	यावत् (*r)
	{
	    यदि (*r == DLG_BUTTON_SEP)
	    {
		यदि (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    यदि (has_mbyte)
			hotkp += STRLEN(hotkp);
		    अन्यथा
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    यदि (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    यदि (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			first_hotkey = TRUE;
		}
		अन्यथा
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    यदि (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    अन्यथा यदि (*r == DLG_HOTKEY_CHAR || first_hotkey)
	    {
		यदि (*r == DLG_HOTKEY_CHAR)
		    ++r;
		first_hotkey = FALSE;
		यदि (copy)
		{
		    यदि (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    अन्यथा
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		अन्यथा
		{
		    ++len;	    // '&a' -> '[a]'
		    यदि (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    अन्यथा
	    {
		// everything else copy literally
		यदि (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	यदि (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	अन्यथा
	{
	    len += (पूर्णाङ्क)(STRLEN(message)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    यदि (!has_hotkey[0])
	    {
		first_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_संदेश);
	    confirm_संदेश = alloc(len);
	    यदि (confirm_संदेश == रिक्त)
		प्रत्यागम रिक्त;
	    *confirm_संदेश = NUL;
	    hotk = alloc(lenhotkey);
	    यदि (hotk == रिक्त)
		प्रत्यागम रिक्त;

	    *confirm_संदेश = '\n';
	    STRCPY(confirm_संदेश + 1, message);

	    msgp = confirm_संदेश + 1 + STRLEN(message);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_संदेश_tail = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_संदेश();
    प्रत्यागम hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    स्थिर शून्य
display_confirm_संदेश(शून्य)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_संदेश_used;
    यदि (confirm_संदेश != रिक्त)
	संदेश_puts_attr((वर्ण *)confirm_संदेश, HL_ATTR(HLF_M));
    --confirm_संदेश_used;
}

#endif // FEAT_CON_DIALOG

#यदि defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    पूर्णाङ्क
vim_dialog_yesno(
    पूर्णाङ्क		type,
    char_u	*title,
    char_u	*message,
    पूर्णाङ्क		dflt)
{
    यदि (do_dialog(type,
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
