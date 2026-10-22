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

#અન્તર્ભાવ "vim.h"

સ્થિર શૂન્ય add_સંદેશ_hist(char_u *s, પૂર્ણાંક len, પૂર્ણાંક attr);
સ્થિર શૂન્ય check_સંદેશ_hist(શૂન્ય);
સ્થિર શૂન્ય hit_return_સંદેશ(શૂન્ય);
સ્થિર શૂન્ય સંદેશ_home_replace_attr(char_u *fname, પૂર્ણાંક attr);
સ્થિર શૂન્ય સંદેશ_puts_attr_len(અક્ષર *str, પૂર્ણાંક maxlen, પૂર્ણાંક attr);
સ્થિર શૂન્ય સંદેશ_puts_display(char_u *str, પૂર્ણાંક maxlen, પૂર્ણાંક attr, પૂર્ણાંક recurse);
સ્થિર શૂન્ય સંદેશ_scroll_up(શૂન્ય);
સ્થિર શૂન્ય inc_સંદેશ_scrolled(શૂન્ય);
સ્થિર શૂન્ય store_sb_text(char_u **sb_str, char_u *s, પૂર્ણાંક attr, પૂર્ણાંક *sb_col, પૂર્ણાંક finish);
સ્થિર શૂન્ય t_puts(પૂર્ણાંક *t_col, char_u *t_s, char_u *s, પૂર્ણાંક attr);
સ્થિર શૂન્ય સંદેશ_puts_printf(char_u *str, પૂર્ણાંક maxlen);
સ્થિર પૂર્ણાંક do_more_prompt(પૂર્ણાંક typed_char);
સ્થિર શૂન્ય સંદેશ_screen_putchar(પૂર્ણાંક c, પૂર્ણાંક attr);
સ્થિર શૂન્ય સંદેશ_moremsg(પૂર્ણાંક full);
સ્થિર પૂર્ણાંક  સંદેશ_check_screen(શૂન્ય);
સ્થિર શૂન્ય redir_write(char_u *s, પૂર્ણાંક maxlen);
#ifdef FEAT_CON_DIALOG
સ્થિર char_u *સંદેશ_show_console_dialog(char_u *message, char_u *buttons, પૂર્ણાંક dfltbutton);
સ્થિર પૂર્ણાંક	confirm_સંદેશ_used = FALSE;	// displaying confirm_msg
સ્થિર char_u	*confirm_સંદેશ = ખાલી;		// ":confirm" message
સ્થિર char_u	*confirm_સંદેશ_tail;		// tail of confirm_msg
સ્થિર શૂન્ય display_confirm_સંદેશ(શૂન્ય);
#endif
#ifdef FEAT_EVAL
સ્થિર પૂર્ણાંક emsg_to_channel_log = FALSE;
#endif

સંરચના સંદેશ_hist
{
    સંરચના સંદેશ_hist	*next;
    char_u		*સંદેશ;
    પૂર્ણાંક			attr;
};

સ્થિર સંરચના સંદેશ_hist *first_સંદેશ_hist = ખાલી;
સ્થિર સંરચના સંદેશ_hist *last_સંદેશ_hist = ખાલી;
સ્થિર પૂર્ણાંક સંદેશ_hist_len = 0;
સ્થિર પૂર્ણાંક સંદેશ_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#define MESSAGES_HIT_ENTER	0x001
#define MESSAGES_WAIT		0x002
#define MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#define MESSAGES_OPT_HIT_ENTER "hit-enter"
#define MESSAGES_OPT_WAIT "wait:"
#define MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
સ્થિર પૂર્ણાંક સંદેશ_flags = MESSAGES_HIT_ENTER | MESSAGES_HISTORY;
સ્થિર પૂર્ણાંક સંદેશ_wait = 0;

સ્થિર FILE *verbose_fd = ખાલી;
સ્થિર પૂર્ણાંક  verbose_did_open = FALSE;

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
    પૂર્ણાંક
સંદેશ(અક્ષર *s)
{
    પાછા સંદેશ_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    પૂર્ણાંક
verb_સંદેશ(અક્ષર *s)
{
    પૂર્ણાંક		n;

    verbose_enter();
    n = સંદેશ_attr_keep(s, 0, FALSE);
    verbose_leave();

    પાછા n;
}

    પૂર્ણાંક
સંદેશ_attr(અક્ષર *s, પૂર્ણાંક attr)
{
    પાછા સંદેશ_attr_keep(s, attr, FALSE);
}

    પૂર્ણાંક
સંદેશ_attr_keep(
    અક્ષર	*s,
    પૂર્ણાંક		attr,
    પૂર્ણાંક		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    સ્થિર પૂર્ણાંક	entered = 0;
    પૂર્ણાંક		retval;
    char_u	*buf = ખાલી;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    જો (!emsg_on_display && message_filtered((char_u *)s))
	પાછા TRUE;

#ifdef FEAT_EVAL
    જો (attr == 0)
	set_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#endif

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    જો (entered >= 3)
	પાછા TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    જો ((char_u *)s != keep_સંદેશ
	    || (*s != '<'
		&& last_સંદેશ_hist != ખાલી
		&& last_સંદેશ_hist->સંદેશ != ખાલી
		&& STRCMP(s, last_સંદેશ_hist->સંદેશ) != 0))
	add_સંદેશ_hist((char_u *)s, -1, attr);

#ifdef FEAT_EVAL
    જો (emsg_to_channel_log)
	// Write message in the channel log.
	ch_log(ખાલી, "ERROR: %s", s);
#endif

    // Truncate the message if needed.
    સંદેશ_start();
    buf = સંદેશ_strtrunc((char_u *)s, FALSE);
    જો (buf != ખાલી)
	s = (અક્ષર *)buf;

    સંદેશ_outtrans_attr((char_u *)s, attr);
    સંદેશ_clr_eos();
    retval = સંદેશ_end();

    જો (keep && retval && vim_strsize((char_u *)s)
		    < (પૂર્ણાંક)(Rows - cmdline_row - 1) * cmdline_width + sc_col)
	set_keep_સંદેશ((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    પાછા retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
સંદેશ_strtrunc(
    char_u	*s,
    પૂર્ણાંક		force)	    // always truncate
{
    char_u	*buf = ખાલી;
    પૂર્ણાંક		len;
    પૂર્ણાંક		room;

    // May truncate message to avoid a hit-return prompt
    જો ((!સંદેશ_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_active && સંદેશ_silent == 0) || force)
    {
	len = vim_strsize(s);
	જો (સંદેશ_scrolled != 0
#ifdef HAS_MESSAGE_WINDOW
		|| in_echowindow
#endif
		)
	    // Use all the columns.
	    room = (પૂર્ણાંક)(Rows - સંદેશ_row) * cmdline_width - 1;
	નહીંતર
	    // Use up to 'showcmd' column.
	    room = (પૂર્ણાંક)(Rows - સંદેશ_row - 1) * cmdline_width + sc_col - 1;
	જો (len > room && room > 0)
	{
	    જો (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (room + 2) * 18;
	    નહીંતર જો (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (room + 2) * 2;
	    નહીંતર
		len = room + 2;
	    buf = alloc(len);
	    જો (buf != ખાલી)
		trunc_string(s, buf, room, len);
	}
    }
    પાછા buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    શૂન્ય
trunc_string(
    char_u	*s,
    char_u	*buf,
    પૂર્ણાંક		room_in,
    પૂર્ણાંક		buflen)
{
    size_t	room = room_in - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    પૂર્ણાંક		e;
    પૂર્ણાંક		i;
    પૂર્ણાંક		n;

    જો (*s == NUL)
    {
	જો (buflen > 0)
	    *buf = NUL;
	પાછા;
    }

    જો (room_in < 3)
	room = 0;
    half = room / 2;

    // First part: Start of the string.
    માટે (e = 0; len < half && e < buflen; ++e)
    {
	જો (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    પાછા;
	}
	n = ptr2cells(s + e);
	જો (len + n > half)
	    તોડો;
	len += n;
	buf[e] = s[e];
	જો (has_mbyte)
	    માટે (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		જો (++e == buflen)
		    તોડો;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    જો (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	જ્યારે (len + n > room)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    નહીંતર જો (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (પૂર્ણાંક)STRLEN(s);
	માટે (;;)
	{
	    કરો
		half = half - utf_head_off(s, s + half - 1) - 1;
	    જ્યારે (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    જો (len + n > room || half == 0)
		તોડો;
	    len += n;
	    i = (પૂર્ણાંક)half;
	}
    }
    નહીંતર
    {
	માટે (i = (પૂર્ણાંક)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= room; --i)
	    len += n;
    }


    જો (i <= e + 3)
    {
	// text fits without truncating
	જો (s != buf)
	{
	    len = STRLEN(s);
	    જો (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    જો (len < 1)
		buf[e - 1] = NUL;
	    નહીંતર
		mch_memmove(buf + e, s + e, len);
	}
    }
    નહીંતર જો (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	જો (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    નહીંતર
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

પૂર્ણાંક vim_snprintf(અક્ષર *str, size_t str_m, અચલ અક્ષર *fmt, ...);

    પૂર્ણાંક
smsg(અચલ અક્ષર *s, ...)
{
    જો (IObuff == ખાલી)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	પાછા સંદેશ((અક્ષર *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((અક્ષર *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    પાછા સંદેશ((અક્ષર *)IObuff);
}

    પૂર્ણાંક
smsg_attr(પૂર્ણાંક attr, અચલ અક્ષર *s, ...)
{
    જો (IObuff == ખાલી)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	પાછા સંદેશ_attr((અક્ષર *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((અક્ષર *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    પાછા સંદેશ_attr((અક્ષર *)IObuff, attr);
}

    પૂર્ણાંક
smsg_attr_keep(પૂર્ણાંક attr, અચલ અક્ષર *s, ...)
{
    જો (IObuff == ખાલી)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	પાછા સંદેશ_attr_keep((અક્ષર *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((અક્ષર *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    પાછા સંદેશ_attr_keep((અક્ષર *)IObuff, attr, TRUE);
}

#endif

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
સ્થિર પૂર્ણાંક	last_sourcing_lnum = 0;
સ્થિર char_u   *last_sourcing_name = ખાલી;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    શૂન્ય
reset_last_sourcing(શૂન્ય)
{
    VIM_CLEAR(last_sourcing_name);
    last_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    સ્થિર પૂર્ણાંક
other_sourcing_name(શૂન્ય)
{
    જો (HAVE_SOURCING_INFO && SOURCING_NAME != ખાલી)
    {
	જો (last_sourcing_name != ખાલી)
	    પાછા STRCMP(SOURCING_NAME, last_sourcing_name) != 0;
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    સ્થિર char_u *
get_emsg_source(શૂન્ય)
{
    char_u	*Buf, *p;

    જો (HAVE_SOURCING_INFO && SOURCING_NAME != ખાલી && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	જો (sname == ખાલી)
	    sname = SOURCING_NAME;

#ifdef FEAT_EVAL
	જો (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	નહીંતર
#endif
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	જો (Buf != ખાલી)
	    sprintf((અક્ષર *)Buf, (અક્ષર *)p, sname);
	vim_free(tofree);
	પાછા Buf;
    }
    પાછા ખાલી;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    સ્થિર char_u *
get_emsg_lnum(શૂન્ય)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    જો (SOURCING_NAME != ખાલી
	    && (other_sourcing_name() || SOURCING_LNUM != last_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	જો (Buf != ખાલી)
	    sprintf((અક્ષર *)Buf, (અક્ષર *)p, (long)SOURCING_LNUM);
	પાછા Buf;
    }
    પાછા ખાલી;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    શૂન્ય
સંદેશ_source(પૂર્ણાંક attr)
{
    char_u	*p;
    સ્થિર પૂર્ણાંક	recursive = FALSE;

    // Bail out if something called here causes an error.
    જો (recursive)
	પાછા;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_source();
    જો (p != ખાલી)
    {
	સંદેશ_scroll = TRUE;  // this will take more than one line
	સંદેશ_attr((અક્ષર *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    જો (p != ખાલી)
    {
	સંદેશ_attr((અક્ષર *)p, HL_ATTR(HLF_N));
	vim_free(p);
	last_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    જો (SOURCING_NAME == ખાલી || other_sourcing_name())
    {
	VIM_CLEAR(last_sourcing_name);
	જો (SOURCING_NAME != ખાલી)
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
    સ્થિર પૂર્ણાંક
emsg_not_now(શૂન્ય)
{
    જો ((emsg_off > 0 && vim_strchr(p_debug, 'm') == ખાલી
					  && vim_strchr(p_debug, 't') == ખાલી)
#ifdef FEAT_EVAL
	    || emsg_skip > 0
#endif
	    )
	પાછા TRUE;
    પાછા FALSE;
}

#જો defined(FEAT_EVAL)
સ્થિર garray_T ignore_error_list = GA_EMPTY;

    શૂન્ય
ignore_error_for_testing(char_u *error)
{
    જો (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, કદ(char_u *), 1);

    જો (STRCMP("RESET", error) == 0)
	ga_clear_strings(&ignore_error_list);
    નહીંતર
	ga_copy_string(&ignore_error_list, error);
}

    સ્થિર પૂર્ણાંક
ignore_error(અચલ અક્ષર *સંદેશ)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < ignore_error_list.ga_len; ++i)
	જો (strstr(સંદેશ,
		  (અક્ષર *)((char_u **)(ignore_error_list.ga_data))[i]) != ખાલી)
	    પાછા TRUE;
    પાછા FALSE;
}
#endif

#જો !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    શૂન્ય
do_perror(અક્ષર *સંદેશ)
{
    perror(સંદેશ);
    ++emsg_silent;
    emsg(સંદેશ);
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
    સ્થિર પૂર્ણાંક
emsg_core(અચલ અક્ષર *s)
{
    પૂર્ણાંક		attr;
    char_u	*p;
    પૂર્ણાંક		r;
#ifdef FEAT_EVAL
    પૂર્ણાંક		ignore = FALSE;
    પૂર્ણાંક		severe;
#endif

#ifdef FEAT_EVAL
    // When testing some errors are turned into a normal message.
    જો (ignore_error(s))
	// don't call msg() if it results in a dialog
	પાછા સંદેશ_use_printf() ? FALSE : સંદેશ((અક્ષર *)s);
#endif

    ++called_emsg;

#ifdef FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#endif

    જો (!emsg_off || vim_strchr(p_debug, 't') != ખાલી)
    {
#ifdef FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	જો (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    જો (!ignore)
		++did_emsg;
	    પાછા TRUE;
	}

	જો (in_assert_fails && emsg_assert_fails_સંદેશ == ખાલી)
	{
	    emsg_assert_fails_સંદેશ = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == ખાલી ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#endif

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	જો (emsg_silent != 0)
	{
#ifdef FEAT_EVAL
	    ++did_emsg_silent;
#endif
	    જો (emsg_noredir == 0)
	    {
		સંદેશ_start();
		p = get_emsg_source();
		જો (p != ખાલી)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		જો (p != ખાલી)
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
	    જો (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#endif
#ifdef FEAT_EVAL
	    ch_log(ખાલી, "ERROR silent: %s", s);
#endif
	    પાછા TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	સંદેશ_silent = 0;
	cmd_silent = FALSE;

	જો (global_busy)		// break :global command
	    ++global_busy;

	જો (p_eb)
	    beep_flush();		// also includes flush_buffers()
	નહીંતર
	    flush_buffers(FLUSH_MINIMAL);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ifdef FEAT_EVAL
	++uncaught_emsg;
#endif
    }

#ifdef HAS_MESSAGE_WINDOW
    જો (!in_echowindow)
#endif
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    જો (સંદેશ_scrolled != 0)
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
    સંદેશ_scroll = TRUE;
    સંદેશ_source(attr);

    /*
     * Display the error message itself.
     */
    સંદેશ_nowait = FALSE;			// wait for this msg
    r = સંદેશ_attr((અક્ષર *)s, attr);

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = FALSE;
#endif
    પાછા r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    પૂર્ણાંક
emsg(અક્ષર *s)
{
    // Skip this if not giving error messages at the moment.
    જો (emsg_not_now())
	પાછા TRUE;

    પાછા emsg_core(s);
}

#ifndef PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    પૂર્ણાંક
semsg(અચલ અક્ષર *s, ...)
{
    // Skip this if not giving error messages at the moment.
    જો (emsg_not_now())
	પાછા TRUE;

    જો (IObuff == ખાલી)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	પાછા emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((અક્ષર *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    પાછા emsg_core((અક્ષર *)IObuff);
}
#endif

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    શૂન્ય
iemsg(અક્ષર *s)
{
    જો (emsg_not_now())
	પાછા;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    emsg_core(s);
#જો defined(ABORT_ON_INTERNAL_ERROR) && defined(FEAT_EVAL)
    set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    સંદેશ_putchar('\n');  // avoid overwriting the error message
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
    શૂન્ય
siemsg(અચલ અક્ષર *s, ...)
{
    જો (emsg_not_now())
	પાછા;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    જો (IObuff == ખાલી)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    નહીંતર
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((અક્ષર *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((અક્ષર *)IObuff);
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    સંદેશ_putchar('\n');  // avoid overwriting the error message
    out_flush();
    abort();
# endif
}
#endif

/*
 * Give an "Internal error" message.
 */
    શૂન્ય
internal_error(અક્ષર *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    siemsg(_(e_internal_error_str), where);
}

#જો defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    શૂન્ય
internal_error_no_abort(અક્ષર *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    semsg(_(e_internal_error_str), where);
}
#endif

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    શૂન્ય
emsg_invreg(પૂર્ણાંક name)
{
    semsg(_(e_invalid_register_name_str), transchar_buf(ખાલી, name));
}

#જો defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    શૂન્ય
emsg_namelen(અક્ષર *સંદેશ, char_u *name, પૂર્ણાંક len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(સંદેશ, copy == ખાલી ? "NULL" : (અક્ષર *)copy);
    vim_free(copy);
}
#endif

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    અક્ષર *
સંદેશ_trunc_attr(અક્ષર *s, પૂર્ણાંક force, પૂર્ણાંક attr)
{
    પૂર્ણાંક		n;
    અક્ષર	*ts;

    // Add message to history before truncating
    add_સંદેશ_hist((char_u *)s, -1, attr);

    ts = (અક્ષર *)સંદેશ_may_trunc(force, (char_u *)s);

    સંદેશ_hist_off = TRUE;
    n = સંદેશ_attr(ts, attr);
    સંદેશ_hist_off = FALSE;

    જો (n)
	પાછા ts;
    પાછા ખાલી;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
સંદેશ_may_trunc(પૂર્ણાંક force, char_u *s)
{
    પૂર્ણાંક		n;
    પૂર્ણાંક		room;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    room = (પૂર્ણાંક)(Rows - cmdline_row - 1) * cmdline_width + sc_col - 1;
    જો (room > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_active))
	    && (n = (પૂર્ણાંક)STRLEN(s) - room) > 0)
    {
	જો (has_mbyte)
	{
	    પૂર્ણાંક	size = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    જો (size <= room)
		પાછા s;

	    માટે (n = 0; size >= room; )
	    {
		size -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    પાછા s;
}

    સ્થિર શૂન્ય
add_સંદેશ_hist(
    char_u	*s,
    પૂર્ણાંક		len,		// -1 for undetermined length
    પૂર્ણાંક		attr)
{
    સંરચના સંદેશ_hist *p;

    જો (સંદેશ_hist_off || સંદેશ_silent != 0)
	પાછા;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(સંરચના સંદેશ_hist);
    જો (p == ખાલી)
	પાછા;

    જો (len < 0)
	len = (પૂર્ણાંક)STRLEN(s);
    // remove leading and trailing newlines
    જ્યારે (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    જ્યારે (len > 0 && s[len - 1] == '\n')
	--len;
    p->સંદેશ = vim_strnsave(s, len);
    p->next = ખાલી;
    p->attr = attr;
    જો (last_સંદેશ_hist != ખાલી)
	last_સંદેશ_hist->next = p;
    last_સંદેશ_hist = p;
    જો (first_સંદેશ_hist == ખાલી)
	first_સંદેશ_hist = last_સંદેશ_hist;
    ++સંદેશ_hist_len;

    check_સંદેશ_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    પૂર્ણાંક
delete_first_સંદેશ(શૂન્ય)
{
    સંરચના સંદેશ_hist *p;

    જો (સંદેશ_hist_len <= 0)
	પાછા FAIL;
    p = first_સંદેશ_hist;
    first_સંદેશ_hist = p->next;
    જો (first_સંદેશ_hist == ખાલી)
	last_સંદેશ_hist = ખાલી;  // history is empty
    vim_free(p->સંદેશ);
    vim_free(p);
    --સંદેશ_hist_len;
    પાછા OK;
}

    સ્થિર શૂન્ય
check_સંદેશ_hist(શૂન્ય)
{
    // Don't let the message history get too big
    જ્યારે (સંદેશ_hist_len > 0 && સંદેશ_hist_len > સંદેશ_hist_max)
	(શૂન્ય)delete_first_સંદેશ();
}


    પૂર્ણાંક
messagesopt_changed(શૂન્ય)
{
    char_u	*p;
    પૂર્ણાંક		messages_flags_new = 0;
    પૂર્ણાંક		messages_wait_new = 0;
    પૂર્ણાંક		messages_history_new = 0;

    p = p_mopt;
    જ્યારે (*p != NUL)
    {
	જો (STRNCMP(p, MESSAGES_OPT_HIT_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER);
	    messages_flags_new |= MESSAGES_HIT_ENTER;
	}
	નહીંતર જો (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	નહીંતર જો (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	જો (*p != ',' && *p != NUL)
	    પાછા FAIL;
	જો (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    જો (!(messages_flags_new & (MESSAGES_HIT_ENTER | MESSAGES_WAIT)))
	પાછા FAIL;

    // "history" must be set
    જો (!(messages_flags_new & MESSAGES_HISTORY))
	પાછા FAIL;

    જો (messages_history_new < 0 || messages_history_new > 10000)
	પાછા FAIL;

    જો (messages_wait_new < 0 || messages_wait_new > 10000)
	પાછા FAIL;

    સંદેશ_flags = messages_flags_new;
    સંદેશ_wait = messages_wait_new;

    સંદેશ_hist_max = messages_history_new;
    check_સંદેશ_hist();

    પાછા OK;
}

/*
 * ":messages" command.
 */
    શૂન્ય
ex_messages(exarg_T *eap)
{
    સંરચના સંદેશ_hist *p;
    char_u	    *s;
    પૂર્ણાંક		    c = 0;

    જો (STRCMP(eap->arg, "clear") == 0)
    {
	પૂર્ણાંક keep = eap->addr_ગણતરી == 0 ? 0 : eap->line2;

	જ્યારે (સંદેશ_hist_len > keep)
	    (શૂન્ય)delete_first_સંદેશ();
	પાછા;
    }

    જો (*eap->arg != NUL)
    {
	emsg(_(e_invalid_argument));
	પાછા;
    }

    સંદેશ_hist_off = TRUE;

    p = first_સંદેશ_hist;
    જો (eap->addr_ગણતરી != 0)
    {
	// Count total messages
	માટે (; p != ખાલી && !got_int; p = p->next)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	માટે (p = first_સંદેશ_hist; p != ખાલી && !got_int && c > 0;
						    p = p->next, c--);
    }

    જો (p == first_સંદેશ_hist)
    {
#ifdef FEAT_MULTI_LANG
	s = get_mess_lang();
#નહીંતર
	s = mch_getenv((char_u *)"LANG");
#endif
	જો (s != ખાલી && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    સંદેશ_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    માટે (; p != ખાલી && !got_int; p = p->next)
	જો (p->સંદેશ != ખાલી)
	    સંદેશ_attr((અક્ષર *)p->સંદેશ, p->attr);

    સંદેશ_hist_off = FALSE;
}

#જો defined(FEAT_CON_DIALOG) || defined(FIND_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    શૂન્ય
સંદેશ_end_prompt(શૂન્ય)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_row = સંદેશ_row;
    સંદેશ_col = 0;
    સંદેશ_clr_eos();
    લીટીઓ_ડાબું = -1;
}
#endif

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    શૂન્ય
wait_return(પૂર્ણાંક redraw)
{
    પૂર્ણાંક		c;
    પૂર્ણાંક		oldState;
    પૂર્ણાંક		tmpState;
    પૂર્ણાંક		had_got_int;
    પૂર્ણાંક		save_reg_recording;
    FILE	*save_scriptout;

    જો (redraw == TRUE)
	set_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    જો (સંદેશ_silent != 0)
	પાછા;
#ifdef HAS_MESSAGE_WINDOW
    જો (in_echowindow)
	પાછા;
#endif

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    જો (vgetc_busy > 0)
	પાછા;
    need_wait_return = TRUE;
    જો (no_wait_return)
    {
	જો (!exmode_active)
	    cmdline_row = સંદેશ_row;
	પાછા;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    જો (quit_more)
    {
	c = CAR;		// just pretend CR was hit
	quit_more = FALSE;
	got_int = FALSE;
    }
    નહીંતર જો (exmode_active)
    {
	સંદેશ_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    નહીંતર જો (!stuff_empty())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    નહીંતર
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = MODE_HITRETURN;
	setmouse();
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#endif
	cmdline_row = સંદેશ_row;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	જો (need_check_timestamps)
	    check_timestamps(FALSE);

	જો (સંદેશ_flags & MESSAGES_HIT_ENTER)
	{
	    hit_return_સંદેશ();

	    કરો
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
		++allow_ચાવીઓ;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = ખાલી;
		c = safe_vgetc();
		જો (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_ચાવીઓ;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ifdef FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		જો (ચાવી_Typed && c == Ctrl_Y && clip_star.state == SELECT_DONE)
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
		જો (ચાવી_Typed && p_more && !p_cp)
		{
		    જો (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_UP || c == K_PAGEUP)
		    {
			જો (સંદેશ_scrolled > Rows)
			    // scroll back to show older messages
			    do_more_prompt(c);
			નહીંતર
			{
			    સંદેશ_didout = FALSE;
			    c = K_IGNORE;
			    સંદેશ_col =
#ifdef FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_width - 1 :
#endif
				0;
			}
			જો (quit_more)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_more = FALSE;
			    got_int = FALSE;
			}
			નહીંતર જો (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    hit_return_સંદેશ();
			}
		    }
		    નહીંતર જો (સંદેશ_scrolled > Rows - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_DOWN || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } જ્યારે ((had_got_int && c == Ctrl_C)
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
				    && mouse_row < સંદેશ_row
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    જો (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(શૂન્ય)jump_to_mouse(MOUSE_SETPOS, ખાલી, 0);
	    નહીંતર જો (!ચાવી_Typed || (vim_strchr((char_u *)"\r\n ", c) == ખાલી
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_mask);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	નહીંતર
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(સંદેશ_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    જો (c == ':' || c == '?' || c == '/')
    {
	જો (!exmode_active)
	    cmdline_row = સંદેશ_row;
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
    સંદેશ_check();

#ifndef MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    જો (swapping_screen() && !termcap_active)
	newline_on_exit = TRUE;
#endif

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    લીટીઓ_ડાબું = -1;		// reset lines_left at next msg_start()
    reset_last_sourcing();
    જો (keep_સંદેશ != ખાલી && vim_strsize(keep_સંદેશ) >=
			    (Rows - cmdline_row - 1) * cmdline_width + sc_col)
	VIM_CLEAR(keep_સંદેશ);	    // don't redisplay message, it's too long

    જો (tmpState == MODE_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    નહીંતર જો (!skip_redraw
	    && (redraw == TRUE || (સંદેશ_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    સ્થિર શૂન્ય
hit_return_સંદેશ(શૂન્ય)
{
    પૂર્ણાંક		save_p_more = p_more;

    p_more = FALSE;	// don't want to see this message when scrolling back
    જો (સંદેશ_didout)	// start on a new line
	સંદેશ_putchar('\n');
    જો (got_int)
	સંદેશ_puts(_("Interrupt: "));

    સંદેશ_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    જો (!સંદેશ_use_printf())
	સંદેશ_clr_eos();
    p_more = save_p_more;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    શૂન્ય
set_keep_સંદેશ(char_u *s, પૂર્ણાંક attr)
{
    vim_free(keep_સંદેશ);
    જો (s != ખાલી && સંદેશ_silent == 0)
	keep_સંદેશ = vim_strsave(s);
    નહીંતર
	keep_સંદેશ = ખાલી;
    keep_સંદેશ_more = FALSE;
    keep_સંદેશ_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    શૂન્ય
set_keep_સંદેશ_from_hist(શૂન્ય)
{
    જો (keep_સંદેશ == ખાલી && last_સંદેશ_hist != ખાલી && સંદેશ_scrolled == 0
						      && (State & MODE_NORMAL))
	set_keep_સંદેશ(last_સંદેશ_hist->સંદેશ, last_સંદેશ_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    શૂન્ય
સંદેશ_start(શૂન્ય)
{
    પૂર્ણાંક		did_return = FALSE;

    જો (સંદેશ_row < cmdline_row)
	સંદેશ_row = cmdline_row;

    જો (!સંદેશ_silent)
    {
	VIM_CLEAR(keep_સંદેશ);
	need_fileinfo = FALSE;
    }

#ifdef FEAT_EVAL
    જો (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	સંદેશ_clr_eos();
    }
#endif

#ifdef HAS_MESSAGE_WINDOW
    જો (in_echowindow)
    {
	જો (popup_message_win_visible()
		    && ((સંદેશ_col > 0 && (સંદેશ_scroll || !full_screen))
			|| in_echowindow))
	{
	    win_T *wp = popup_get_message_win();

	    // start a new line
	    curbuf = wp->w_buffer;
	    ml_append(wp->w_buffer->b_ml.ml_લીટી_ગણતરી,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_buffer;
	}
	સંદેશ_col = 0;
    }
    નહીંતર
#endif
	જો (!સંદેશ_scroll && full_screen)	// overwrite last message
    {
	સંદેશ_row = cmdline_row;
	સંદેશ_col =
#ifdef FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_width - 1 :
#endif
	    0;
    }
    નહીંતર જો (સંદેશ_didout || in_echowindow)
    {
	// start message on next line
	સંદેશ_putchar('\n');
	did_return = TRUE;
	જો (exmode_active != EXMODE_NORMAL)
	    cmdline_row = સંદેશ_row;
    }
    જો (!સંદેશ_didany || લીટીઓ_ડાબું < 0)
	સંદેશ_starthere();
    જો (સંદેશ_silent == 0)
    {
	સંદેશ_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    જો (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    શૂન્ય
સંદેશ_starthere(શૂન્ય)
{
    લીટીઓ_ડાબું = cmdline_row;
    સંદેશ_didany = FALSE;
}

    શૂન્ય
સંદેશ_putchar(પૂર્ણાંક c)
{
    સંદેશ_putchar_attr(c, 0);
}

    શૂન્ય
સંદેશ_putchar_attr(પૂર્ણાંક c, પૂર્ણાંક attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    જો (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    નહીંતર
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    સંદેશ_puts_attr((અક્ષર *)buf, attr);
}

    શૂન્ય
સંદેશ_outnum(long n)
{
    અક્ષર	buf[20];

    sprintf(buf, "%ld", n);
    સંદેશ_puts(buf);
}

    શૂન્ય
સંદેશ_home_replace(char_u *fname)
{
    સંદેશ_home_replace_attr(fname, 0);
}

#જો defined(FEAT_FIND_ID)
    શૂન્ય
સંદેશ_home_replace_hl(char_u *fname)
{
    સંદેશ_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#endif

    સ્થિર શૂન્ય
સંદેશ_home_replace_attr(char_u *fname, પૂર્ણાંક attr)
{
    char_u	*name;

    name = home_replace_save(ખાલી, fname);
    જો (name != ખાલી)
	સંદેશ_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    પૂર્ણાંક
સંદેશ_outtrans(char_u *str)
{
    પાછા સંદેશ_outtrans_attr(str, 0);
}

    પૂર્ણાંક
સંદેશ_outtrans_attr(char_u *str, પૂર્ણાંક attr)
{
    પાછા સંદેશ_outtrans_len_attr(str, (પૂર્ણાંક)STRLEN(str), attr);
}

    પૂર્ણાંક
સંદેશ_outtrans_len(char_u *str, પૂર્ણાંક len)
{
    પાછા સંદેશ_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
સંદેશ_outtrans_one(char_u *p, પૂર્ણાંક attr)
{
    પૂર્ણાંક		l;

    જો (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	સંદેશ_outtrans_len_attr(p, l, attr);
	પાછા p + l;
    }
    સંદેશ_puts_attr((અક્ષર *)transchar_byte_buf(ખાલી, *p), attr);
    પાછા p + 1;
}

    પૂર્ણાંક
સંદેશ_outtrans_len_attr(char_u *msgstr, પૂર્ણાંક len, પૂર્ણાંક attr)
{
    પૂર્ણાંક		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_start = msgstr;
    char_u	*s;
    પૂર્ણાંક		mb_l;
    પૂર્ણાંક		c;
    પૂર્ણાંક		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    જો (attr == 0)
	attr = HL_ATTR(HLF_સંદેશ);

    // if MSG_HIST flag set, add message to history
    જો (attr & સંદેશ_HIST)
    {
	add_સંદેશ_hist(str, len, attr);
	attr &= ~સંદેશ_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    જો (સંદેશ_silent == 0 && len > 0 && સંદેશ_row >= cmdline_row && સંદેશ_col == 0)
    {
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    જો (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	સંદેશ_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    જ્યારે (--len >= 0 && !got_int)
    {
	જો (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	નહીંતર જો (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	નહીંતર
	    mb_l = 1;
	જો (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    જો (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    નહીંતર
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		જો (str > plain_start)
		    સંદેશ_puts_attr_len((અક્ષર *)plain_start,
					       (પૂર્ણાંક)(str - plain_start), attr);
		plain_start = str + mb_l;
		સંદેશ_puts_attr((અક્ષર *)transchar_buf(ખાલી, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	નહીંતર
	{
	    s = transchar_byte_buf(ખાલી, *str);
	    જો (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		જો (str > plain_start)
		    સંદેશ_puts_attr_len((અક્ષર *)plain_start,
					       (પૂર્ણાંક)(str - plain_start), attr);
		plain_start = str + 1;
		સંદેશ_puts_attr((અક્ષર *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (પૂર્ણાંક)STRLEN(s);
	    }
	    નહીંતર
		++retval;
	    ++str;
	}
    }

    જો (str > plain_start && !got_int)
	// print the printable chars at the end
	સંદેશ_puts_attr_len((અક્ષર *)plain_start, (પૂર્ણાંક)(str - plain_start), attr);

    got_int |= save_got_int;

    પાછા retval;
}

#જો defined(FEAT_QUICKFIX)
    શૂન્ય
સંદેશ_make(char_u *arg)
{
    પૂર્ણાંક	    i;
    સ્થિર char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    માટે (i = 5; *arg && i >= 0; --i)
	જો (*arg++ != str[i])
	    તોડો;
    જો (i < 0)
    {
	સંદેશ_putchar('\n');
	માટે (i = 0; rs[i]; ++i)
	    સંદેશ_putchar(rs[i] - 3);
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
    પૂર્ણાંક
સંદેશ_outtrans_special(
    char_u	*strstart,
    પૂર્ણાંક		from,	// TRUE for lhs of a mapping
    પૂર્ણાંક		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    પૂર્ણાંક		retval = 0;
    અક્ષર	*text;
    પૂર્ણાંક		attr;
    પૂર્ણાંક		len;

    attr = HL_ATTR(HLF_8);
    જ્યારે (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	જો ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    text = "<Space>";
	    ++str;
	}
	નહીંતર
	    text = (અક્ષર *)str2special(&str, from, FALSE);
	જો (text[0] != NUL && text[1] == NUL)
	    // single-byte character or illegal byte
	    text = (અક્ષર *)transchar_byte_buf(ખાલી, (char_u)text[0]);
	len = vim_strsize((char_u *)text);
	જો (maxlen > 0 && retval + len >= maxlen)
	    તોડો;
	// Highlight special keys
	સંદેશ_puts_attr(text, len > 1
		&& (*mb_ptr2len)((char_u *)text) <= 1 ? attr : 0);
	retval += len;
    }
    પાછા retval;
}

#જો defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    પૂર્ણાંક	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    પૂર્ણાંક	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    જ્યારે (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    પાછા (char_u *)ga.ga_data;
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
    પૂર્ણાંક		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    પૂર્ણાંક		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    પૂર્ણાંક			c;
    સ્થિર char_u	buf[7];
    char_u		*str = *sp;
    પૂર્ણાંક			modifiers = 0;
    પૂર્ણાંક			special = FALSE;

    જો (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	જો (p != ખાલી)
	    પાછા p;
    }

    c = *str;
    જો ((c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	) && str[1] != NUL && str[2] != NUL)
    {
	જો (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	જો ((c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = TO_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	જો (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    જો (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	જો (p != ખાલી)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	નહીંતર
	    // illegal byte
	    *sp = str + 1;
    }
    નહીંતર
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    જો (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	પાછા get_special_ચાવી_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    પાછા buf;
}

/*
 * Translate a key sequence into special key names.
 */
    શૂન્ય
str2specialbuf(char_u *sp, char_u *buf, પૂર્ણાંક len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    જ્યારે (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	જો (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	નહીંતર
	    તોડો;
    }
}

/*
 * print line for :print or :list command
 */
    શૂન્ય
સંદેશ_prt_લીટી(char_u *s, પૂર્ણાંક list)
{
    પૂર્ણાંક		c;
    પૂર્ણાંક		col = 0;
    પૂર્ણાંક		n_extra = 0;
    પૂર્ણાંક		c_extra = 0;
    પૂર્ણાંક		c_final = 0;
    char_u	*p_extra = ખાલી;	    // init to make SASC shut up
    પૂર્ણાંક		n;
    પૂર્ણાંક		attr = 0;
    char_u	*trail = ખાલી;
    char_u	*lead = ખાલી;
    પૂર્ણાંક		in_multispace = FALSE;
    પૂર્ણાંક		multispace_pos = 0;
    પૂર્ણાંક		l;
    char_u	buf[MB_MAXBYTES + 1];

    જો (curwin->w_p_list)
	list = TRUE;

    જો (list)
    {
	// find start of trailing whitespace
	જો (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    જ્યારે (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	જો (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != ખાલી
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    જ્યારે (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    જો (*lead == NUL)
		lead = ખાલી;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    જો (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	સંદેશ_putchar(' ');

    જ્યારે (!got_int)
    {
	જો (n_extra > 0)
	{
	    --n_extra;
	    જો (n_extra == 0 && c_final)
		c = c_final;
	    નહીંતર જો (c_extra)
		c = c_extra;
	    નહીંતર
		c = *p_extra++;
	}
	નહીંતર જો (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    col += (*mb_ptr2cells)(s);
	    જો (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    નહીંતર જો (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		પૂર્ણાંક len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    નહીંતર
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    સંદેશ_puts((અક્ષર *)buf);
	    s += l;
	    ચાલુ;
	}
	નહીંતર
	{
	    attr = 0;
	    c = *s++;
	    જો (list)
	    {
		in_multispace = c == ' ' && (*s == ' '
						 || (col > 0 && s[-2] == ' '));
		જો (!in_multispace)
		    multispace_pos = 0;
	    }
	    જો (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ifdef FEAT_VARTABS
		n_extra = tabstop_padding(col, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#નહીંતર
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
#endif
		જો (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		નહીંતર
		{
		    પૂર્ણાંક lcs_tab1 = curwin->w_lcs_chars.tab1;
		    પૂર્ણાંક lcs_tab2 = curwin->w_lcs_chars.tab2;
		    પૂર્ણાંક lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    જો (lead != ખાલી && s <= lead
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
	    નહીંતર જો (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    નહીંતર જો (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    નહીંતર જો (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(ખાલી, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    નહીંતર જો (c == ' ')
	    {
		જો (lead != ખાલી && s <= lead && in_multispace
			&& curwin->w_lcs_chars.leadmultispace != ખાલી)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    જો (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		નહીંતર જો (lead != ખાલી && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		નહીંતર જો (trail != ખાલી && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		નહીંતર જો (in_multispace
			&& curwin->w_lcs_chars.multispace != ખાલી)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    જો (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		નહીંતર જો (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	જો (c == NUL)
	    તોડો;

	સંદેશ_putchar_attr(c, attr);
	col++;
    }
    સંદેશ_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    સ્થિર char_u *
screen_puts_mbyte(char_u *s, પૂર્ણાંક l, પૂર્ણાંક attr)
{
    પૂર્ણાંક		cw;

    સંદેશ_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    જો (cw > 1 && (
#ifdef FEAT_RIGHTLEFT
		cmdmsg_rl ? સંદેશ_col <= 1 :
#endif
		સંદેશ_col == cmdline_width - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	સંદેશ_screen_putchar('>', HL_ATTR(HLF_AT));
	પાછા s;
    }

    screen_puts_len(s, l, સંદેશ_row, cmdline_col_off + સંદેશ_col, attr);
#ifdef FEAT_RIGHTLEFT
    જો (cmdmsg_rl)
    {
	સંદેશ_col -= cw;
	જો (સંદેશ_col == 0)
	{
	    સંદેશ_col = cmdline_width;
	    ++સંદેશ_row;
	}
    }
    નહીંતર
#endif
    {
	સંદેશ_col += cw;
	જો (સંદેશ_col >= cmdline_width)
	{
	    સંદેશ_col = 0;
	    ++સંદેશ_row;
	}
    }
    પાછા s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    શૂન્ય
સંદેશ_puts(અક્ષર *s)
{
    સંદેશ_puts_attr(s, 0);
}

    શૂન્ય
સંદેશ_puts_title(અક્ષર *s)
{
    સંદેશ_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    સ્થિર શૂન્ય
સંદેશ_outtrans_long_len_attr(char_u *longstr, પૂર્ણાંક len, પૂર્ણાંક attr)
{
    પૂર્ણાંક		slen = len;
    પૂર્ણાંક		room;

    room = cmdline_width - સંદેશ_col;
    જો (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	સંદેશ_outtrans_len_attr(longstr, slen, attr);
	સંદેશ_puts_attr("...", HL_ATTR(HLF_8));
    }
    સંદેશ_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    શૂન્ય
સંદેશ_outtrans_long_attr(char_u *longstr, પૂર્ણાંક attr)
{
    સંદેશ_outtrans_long_len_attr(longstr, (પૂર્ણાંક)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    શૂન્ય
સંદેશ_puts_attr(અક્ષર *s, પૂર્ણાંક attr)
{
    સંદેશ_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    સ્થિર શૂન્ય
સંદેશ_puts_attr_len(અક્ષર *str, પૂર્ણાંક maxlen, પૂર્ણાંક attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    જો (સંદેશ_silent != 0)
	પાછા;

    જો (attr == 0)
	attr = HL_ATTR(HLF_સંદેશ);

    // if MSG_HIST flag set, add message to history
    જો ((attr & સંદેશ_HIST) && maxlen < 0)
    {
	add_સંદેશ_hist((char_u *)str, -1, attr);
	attr &= ~સંદેશ_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    જો (સંદેશ_scrolled != 0 && !સંદેશ_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    સંદેશ_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    જો (સંદેશ_use_printf())
	સંદેશ_puts_printf((char_u *)str, maxlen);
    નહીંતર
	સંદેશ_puts_display((char_u *)str, maxlen, attr, FALSE);

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
    સ્થિર શૂન્ય
put_સંદેશ_win(win_T *wp, પૂર્ણાંક where, char_u *t_s, char_u *end, linenr_T lnum)
{
    char_u  *p;

    જો (where == PUT_BELOW)
    {
	જો (*end != NUL)
	{
	    p = vim_strnsave(t_s, end - t_s);
	    જો (p == ખાલી)
		પાછા;
	}
	નહીંતર
	    p = t_s;
	ml_append_buf(wp->w_buffer, lnum, p, (colnr_T)0, FALSE);
	જો (p != t_s)
	    vim_free(p);
    }
    નહીંતર
    {
	char_u *newp;

	curbuf = wp->w_buffer;
	જો (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    જો (newp == ખાલી)
		પાછા;
	    જો (*end != NUL)
		newp[STRLEN(ml_get(lnum)) + (end - t_s)] = NUL;
	}
	નહીંતર
	{
	    newp = vim_strnsave(t_s, end - t_s);
	    જો (newp == ખાલી)
		પાછા;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_buffer;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    સંદેશ_col += (પૂર્ણાંક)(end - t_s);
}
#endif

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    સ્થિર શૂન્ય
સંદેશ_puts_display(
    char_u	*str,
    પૂર્ણાંક		maxlen,
    પૂર્ણાંક		attr,
    પૂર્ણાંક		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    પૂર્ણાંક		t_col = 0;	// screen cells todo, 0 when "t_s" not used
    પૂર્ણાંક		l;
    પૂર્ણાંક		cw;
    char_u	*sb_str = str;
    પૂર્ણાંક		sb_col = સંદેશ_col;
    પૂર્ણાંક		wrap;
    પૂર્ણાંક		did_last_char;
#ifdef HAS_MESSAGE_WINDOW
    પૂર્ણાંક		where = PUT_APPEND;
    win_T	*સંદેશ_win = ખાલી;
    linenr_T    lnum = 1;

    જો (in_echowindow)
    {
	સંદેશ_win = popup_get_message_win();

	જો (સંદેશ_win != ખાલી)
	{
	    જો (!popup_message_win_visible())
	    {
		જો (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    સંદેશ_win = ખાલી;
		}
		નહીંતર
		{
		    // currently hidden, make it empty
		    curbuf = સંદેશ_win->w_buffer;
		    જ્યારે ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
			ml_delete(1);
		    curbuf = curwin->w_buffer;
		}
	    }
	    નહીંતર
	    {
		lnum = સંદેશ_win->w_buffer->b_ml.ml_લીટી_ગણતરી;
		જો (સંદેશ_col == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#endif

    did_wait_return = FALSE;
    જ્યારે ((maxlen < 0 || (પૂર્ણાંક)(s - str) < maxlen) && *s != NUL)
    {
#ifdef HAS_MESSAGE_WINDOW
	// For echowindow, use full width; for regular messages, leave last column
	પૂર્ણાંક wrap_col = સંદેશ_win != ખાલી ? cmdline_width : cmdline_width - 1;
#નહીંતર
	પૂર્ણાંક wrap_col = cmdline_width - 1;
#endif

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	જો (!recurse && સંદેશ_row >= Rows - 1 && (*s == '\n' || (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			સંદેશ_col <= 1
		      || (*s == TAB && સંદેશ_col <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && સંદેશ_col <= 2))
		    :
#endif
		      ((*s != '\r' && સંદેશ_col + t_col >= wrap_col)
		       || (*s == TAB && સંદેશ_col + t_col
			   >= (wrap_col & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && સંદેશ_col + t_col >= wrap_col - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    જો (t_col > 0)
	    {
		// output postponed text
#ifdef HAS_MESSAGE_WINDOW
		જો (સંદેશ_win != ખાલી)
		{
		    put_સંદેશ_win(સંદેશ_win, where, t_s, s, lnum);
		    જો (where == PUT_BELOW)
			++lnum;
		    t_col = 0;
		    where = PUT_BELOW;
		}
		નહીંતર
#endif
		    t_puts(&t_col, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    જો (સંદેશ_no_more && લીટીઓ_ડાબું == 0)
		તોડો;

#ifdef HAS_MESSAGE_WINDOW
	    જો (સંદેશ_win == ખાલી)
#endif
		// Scroll the screen up one line.
		સંદેશ_scroll_up();

#ifdef HAS_MESSAGE_WINDOW
	    જો (સંદેશ_win == ખાલી)
	    {
#endif
		સંદેશ_row = Rows - 2;
		જો (સંદેશ_col >= cmdline_width)  // can happen after screen resize
		    સંદેશ_col = cmdline_width - 1;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif

	    // Display char in last column before showing more-prompt.
	    જો (*s >= ' '
#ifdef FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#endif
	       )
	    {
		જો (has_mbyte)
		{
		    જો (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (પૂર્ણાંક)((str + maxlen) - s));
		    નહીંતર
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		નહીંતર
		    સંદેશ_screen_putchar(*s++, attr);
		did_last_char = TRUE;
	    }
	    નહીંતર
		did_last_char = FALSE;

	    જો (p_more)
		// store text for scrolling back
		store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

#ifdef HAS_MESSAGE_WINDOW
	    જો (સંદેશ_win == ખાલી)
	    {
#endif
		inc_સંદેશ_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		જો (cmdline_row > 0 && !exmode_active)
		    --cmdline_row;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		જો (લીટીઓ_ડાબું > 0)
		    --લીટીઓ_ડાબું;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif
	    જો (p_more && લીટીઓ_ડાબું == 0 && State != MODE_HITRETURN
					    && !સંદેશ_no_more && !exmode_active)
	    {
#ifdef FEAT_CON_DIALOG
		જો (do_more_prompt(NUL))
		    s = confirm_સંદેશ_tail;
#નહીંતર
		(શૂન્ય)do_more_prompt(NUL);
#endif
		જો (quit_more)
		    પાછા;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    જો (did_last_char)
		ચાલુ;
	}

	wrap = *s == '\n'
		    || સંદેશ_col + t_col >= wrap_col
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && સંદેશ_col + t_col >= wrap_col - 1);
	જો (t_col > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ifdef HAS_MESSAGE_WINDOW
	    જો (સંદેશ_win != ખાલી)
	    {
		put_સંદેશ_win(સંદેશ_win, where, t_s, s, lnum);
		જો (where == PUT_BELOW)
		    ++lnum;
		t_col = 0;
		where = PUT_BELOW;
		// Reset msg_col after outputting to new line in echowindow
		જો (wrap)
		    સંદેશ_col = 0;
	    }
	    નહીંતર
#endif
		t_puts(&t_col, t_s, s, attr);
	}

	જો (wrap && p_more && !recurse)
	    // store text for scrolling back
	    store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	જો (*s == '\n')		    // go to next line
	{
#ifdef HAS_MESSAGE_WINDOW
	    જો (સંદેશ_win != ખાલી)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		જો ((સંદેશ_win->w_buffer->b_ml.ml_flags & ML_EMPTY) == 0)
		{
		    put_સંદેશ_win(સંદેશ_win, PUT_BELOW, t_s, t_s, lnum);
		    ++lnum;
		}
		સંદેશ_col = 0;  // Reset column for new line
	    }
	    નહીંતર
#endif
		સંદેશ_didout = FALSE;	    // remember that line is empty
#ifdef FEAT_RIGHTLEFT
	    જો (cmdmsg_rl)
		સંદેશ_col = cmdline_width - 1;
	    નહીંતર
#endif
		સંદેશ_col = 0;
	    જો (++સંદેશ_row >= Rows)  // safety check
		સંદેશ_row = Rows - 1;
	}
	નહીંતર જો (*s == '\r')	    // go to column 0
	{
	    સંદેશ_col = 0;
#ifdef HAS_MESSAGE_WINDOW
	    where = PUT_TRUNC;
#endif
	}
	નહીંતર જો (*s == '\b')	    // go to previous char
	{
	    જો (સંદેશ_col)
		--સંદેશ_col;
	}
	નહીંતર જો (*s == TAB)	    // translate Tab into spaces
	{
#ifdef HAS_MESSAGE_WINDOW
	    જો (સંદેશ_win != ખાલી)
		સંદેશ_col = (સંદેશ_col + 7) % 8;
	    નહીંતર
#endif
		કરો
		    સંદેશ_screen_putchar(' ', attr);
		જ્યારે (સંદેશ_col & 7);
	}
	નહીંતર જો (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	નહીંતર
	{
	    જો (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		જો (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (પૂર્ણાંક)((str + maxlen) - s));
		નહીંતર
		    l = (*mb_ptr2len)(s);
	    }
	    નહીંતર
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    જો (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#endif
		    (cw > 1 && સંદેશ_col + t_col >= wrap_col))
	    {
		જો (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		નહીંતર
		    સંદેશ_screen_putchar(*s, attr);
	    }
	    નહીંતર
	    {
		// postpone this character until later
		જો (t_col == 0)
		    t_s = s;
		t_col += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    જો (t_col > 0)
    {
#ifdef HAS_MESSAGE_WINDOW
	જો (સંદેશ_win != ખાલી)
	    put_સંદેશ_win(સંદેશ_win, where, t_s, s, lnum);
	નહીંતર
#endif
	    t_puts(&t_col, t_s, s, attr);
    }

#ifdef HAS_MESSAGE_WINDOW
    જો (સંદેશ_win != ખાલી)
	popup_show_message_win();
#endif
    // Store the text for scroll back.
    જો (p_more && !recurse)
	store_sb_text(&sb_str, s, attr, &sb_col, FALSE);

    સંદેશ_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    પૂર્ણાંક
message_filtered(char_u *સંદેશ)
{
    પૂર્ણાંક match;

    જો (cmdmod.cmod_filter_regmatch.regprog == ખાલી)
	પાછા FALSE;
    match = vim_regexec(&cmdmod.cmod_filter_regmatch, સંદેશ, (colnr_T)0);
    પાછા cmdmod.cmod_filter_force ? match : !match;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    સ્થિર શૂન્ય
સંદેશ_scroll_up(શૂન્ય)
{
#ifdef HAS_MESSAGE_WINDOW
    જો (in_echowindow)
	પાછા;
#endif
#ifdef FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    જો (gui.in_use)
	gui_undraw_cursor();
#endif
    // scrolling up always works
    mch_disable_flush();
    screen_del_લીટીઓ(0, 0, 1, (પૂર્ણાંક)Rows, TRUE, 0, ખાલી);
    mch_enable_flush();

    જો (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((પૂર્ણાંક)Rows - 1, (પૂર્ણાંક)Rows, cmdline_col_off,
		cmdline_col_off + cmdline_width, ' ', ' ', HL_ATTR(HLF_સંદેશ));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	જો (ScreenAttrs[લીટી_Offset[Rows - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((પૂર્ણાંક)Rows - 2, (પૂર્ણાંક)Rows - 1,
				 (પૂર્ણાંક)Columns - 1, (પૂર્ણાંક)Columns,
				 ' ', ' ', HL_ATTR(HLF_સંદેશ));
    }
}

/*
 * Increment "msg_scrolled".
 */
    સ્થિર શૂન્ય
inc_સંદેશ_scrolled(શૂન્ય)
{
#ifdef FEAT_EVAL
    જો (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = ખાલી;

	// v:scrollstart is empty, set it to the script/function name and line
	//સંખ્યા
	જો (p.string == ખાલી)
	{
	    p.string = (char_u *)_("Unknown");
	    p.length = STRLEN(p.string);
	}
	નહીંતર
	{
	    size_t  tofreesize;

	    tofreesize = (પૂર્ણાંક)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    જો (tofree != ખાલી)
	    {
		p.length = vim_snprintf_safelen((અક્ષર *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (long)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	set_vim_var_string(VV_SCROLLSTART, p.string, (પૂર્ણાંક)p.length);
	vim_free(tofree);
    }
#endif
    ++સંદેશ_scrolled;
    set_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
પ્રકારવ્યાખ્યા સંરચના msgchunk_S msgchunk_T;
સંરચના msgchunk_S
{
    msgchunk_T	*sb_next;
    msgchunk_T	*sb_prev;
    અક્ષર	sb_eol;		// TRUE when line ends after this text
    પૂર્ણાંક		sb_સંદેશ_col;	// column in which text starts
    પૂર્ણાંક		sb_attr;	// text attributes
    char_u	sb_text[1];	// text to be displayed, actually longer
};

સ્થિર msgchunk_T *last_msgchunk = ખાલી; // last displayed text

સ્થિર msgchunk_T *સંદેશ_sb_start(msgchunk_T *mps);

પ્રકારવ્યાખ્યા ગણના {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_DONE
} sb_clear_T;

// When to clear text on next msg.
સ્થિર sb_clear_T do_clear_sb_text = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    સ્થિર શૂન્ય
store_sb_text(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    પૂર્ણાંક		attr,
    પૂર્ણાંક		*sb_col,
    પૂર્ણાંક		finish)		// line ends
{
    msgchunk_T	*mp;

    જો (do_clear_sb_text == SB_CLEAR_ALL
	    || do_clear_sb_text == SB_CLEAR_CMDLINE_DONE)
    {
	clear_sb_text(do_clear_sb_text == SB_CLEAR_ALL);
	સંદેશ_sb_eol();  // prevent messages from overlapping
	do_clear_sb_text = SB_CLEAR_NONE;
    }

    જો (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_text) + (s - *sb_str) + 1);
	જો (mp != ખાલી)
	{
	    mp->sb_eol = finish;
	    mp->sb_સંદેશ_col = *sb_col;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_text, *sb_str, s - *sb_str);

	    જો (last_msgchunk == ખાલી)
	    {
		last_msgchunk = mp;
		mp->sb_prev = ખાલી;
	    }
	    નહીંતર
	    {
		mp->sb_prev = last_msgchunk;
		last_msgchunk->sb_next = mp;
		last_msgchunk = mp;
	    }
	    mp->sb_next = ખાલી;
	}
    }
    નહીંતર જો (finish && last_msgchunk != ખાલી)
	last_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_col = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    શૂન્ય
may_clear_sb_text(શૂન્ય)
{
    do_clear_sb_text = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    શૂન્ય
sb_text_start_cmdline(શૂન્ય)
{
    જો (do_clear_sb_text == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_text_restart_cmdline();
    નહીંતર
    {
	સંદેશ_sb_eol();
	do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    શૂન્ય
sb_text_restart_cmdline(શૂન્ય)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;

    જો (last_msgchunk == ખાલી || last_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	પાછા;

    tofree = સંદેશ_sb_start(last_msgchunk);
    last_msgchunk = tofree->sb_prev;
    જો (last_msgchunk != ખાલી)
	last_msgchunk->sb_next = ખાલી;
    જ્યારે (tofree != ખાલી)
    {
	msgchunk_T *tofree_next = tofree->sb_next;

	vim_free(tofree);
	tofree = tofree_next;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    શૂન્ય
sb_text_end_cmdline(શૂન્ય)
{
    do_clear_sb_text = SB_CLEAR_CMDLINE_DONE;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    શૂન્ય
clear_sb_text(પૂર્ણાંક all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    જો (all)
	lastp = &last_msgchunk;
    નહીંતર
    {
	જો (last_msgchunk == ખાલી)
	    પાછા;
	lastp = &સંદેશ_sb_start(last_msgchunk)->sb_prev;
    }

    જ્યારે (*lastp != ખાલી)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    શૂન્ય
show_sb_text(શૂન્ય)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = સંદેશ_sb_start(last_msgchunk);
    જો (mp == ખાલી || mp->sb_prev == ખાલી)
	vim_beep(BO_MESS);
    નહીંતર
    {
	do_more_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    સ્થિર msgchunk_T *
સંદેશ_sb_start(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    જ્યારે (mp != ખાલી && mp->sb_prev != ખાલી && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    પાછા mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    શૂન્ય
સંદેશ_sb_eol(શૂન્ય)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    જો (last_msgchunk != ખાલી
	    && !(last_msgchunk->sb_text[0] == '\n'
		&& last_msgchunk->sb_text[1] == NUL))
	last_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    સ્થિર msgchunk_T *
disp_sb_લીટી(પૂર્ણાંક row, msgchunk_T *smp, પૂર્ણાંક clear_to_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    માટે (;;)
    {
	સંદેશ_row = row;
	સંદેશ_col = mp->sb_સંદેશ_col;
	p = mp->sb_text;
	જો (*p == '\n')	    // don't display the line break
	    ++p;
	સંદેશ_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	જો (clear_to_eol)
	    screen_fill(row, row + 1,
		    cmdline_col_off + સંદેશ_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', HL_ATTR(HLF_સંદેશ));

	જો (mp->sb_eol || mp->sb_next == ખાલી)
	    તોડો;
	mp = mp->sb_next;
    }
    પાછા mp->sb_next;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    સ્થિર શૂન્ય
t_puts(
    પૂર્ણાંક		*t_col,
    char_u	*t_s,
    char_u	*s,
    પૂર્ણાંક		attr)
{
    // output postponed text
    સંદેશ_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (પૂર્ણાંક)(s - t_s), સંદેશ_row, cmdline_col_off + સંદેશ_col,
	    attr);
    સંદેશ_col += *t_col;
    *t_col = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    જો (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--સંદેશ_col;
    જો (સંદેશ_col >= cmdline_width)
    {
	સંદેશ_col = 0;
	++સંદેશ_row;
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
    પૂર્ણાંક
સંદેશ_use_printf(શૂન્ય)
{
    પાછા (!સંદેશ_check_screen()
#જો defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.in_use && !termcap_active)
# નહીંતર
	    || !termcap_active
# endif
#endif
	    || (swapping_screen() && !termcap_active)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    સ્થિર શૂન્ય
સંદેશ_puts_printf(char_u *str, પૂર્ણાંક maxlen)
{
    char_u	*s = str;
    char_u	*buf = ખાલી;
    char_u	*p = s;

#ifdef MSWIN
    જો (!(silent_mode && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#endif
    જ્યારે ((maxlen < 0 || (પૂર્ણાંક)(s - str) < maxlen) && *s != NUL)
    {
	જો (!(silent_mode && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    જો (*s == NL)
	    {
		પૂર્ણાંક n = (પૂર્ણાંક)(s - p);

		buf = alloc(n + 3);
		જો (buf != ખાલી)
		{
		    memcpy(buf, p, n);
		    જો (!info_message)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    જો (info_message)   // informative message, not an error
			mch_સંદેશ((અક્ષર *)buf);
		    નહીંતર
			mch_errmsg((અક્ષર *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ifdef FEAT_RIGHTLEFT
	જો (cmdmsg_rl)
	{
	    જો (*s == CAR || *s == NL)
		સંદેશ_col = cmdline_width - 1;
	    નહીંતર
		--સંદેશ_col;
	}
	નહીંતર
#endif
	{
	    જો (*s == CAR || *s == NL)
		સંદેશ_col = 0;
	    નહીંતર
		++સંદેશ_col;
	}
	++s;
    }

    જો (*p != NUL && !(silent_mode && p_verbose == 0))
    {
	char_u *tofree = ખાલી;

	જો (maxlen > 0 && vim_strlen_maxlen((અક્ષર *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	જો (p != ખાલી)
	{
	    જો (info_message)
		mch_સંદેશ((અક્ષર *)p);
	    નહીંતર
		mch_errmsg((અક્ષર *)p);
	    vim_free(tofree);
	}
    }

    સંદેશ_didout = TRUE;	    // assume that line is not empty

#ifdef MSWIN
    જો (!(silent_mode && p_verbose == 0))
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
    સ્થિર પૂર્ણાંક
do_more_prompt(પૂર્ણાંક typed_char)
{
    સ્થિર પૂર્ણાંક	entered = FALSE;
    પૂર્ણાંક		used_typed_char = typed_char;
    પૂર્ણાંક		oldState = State;
    પૂર્ણાંક		c;
#ifdef FEAT_CON_DIALOG
    પૂર્ણાંક		retval = FALSE;
#endif
    પૂર્ણાંક		toscroll;
    msgchunk_T	*mp_last = ખાલી;
    msgchunk_T	*mp;
    પૂર્ણાંક		i;
    પૂર્ણાંક		સંદેશ_attr;

    સંદેશ_attr = HL_ATTR(HLF_સંદેશ);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    જો (entered || (State == MODE_HITRETURN && typed_char == 0))
	પાછા FALSE;
    entered = TRUE;

    જો (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_last = સંદેશ_sb_start(last_msgchunk);
	માટે (i = 0; i < Rows - 2 && mp_last != ખાલી
					     && mp_last->sb_prev != ખાલી; ++i)
	    mp_last = સંદેશ_sb_start(mp_last->sb_prev);
    }

    State = MODE_ASKMORE;
    setmouse();
    જો (typed_char == NUL)
	સંદેશ_moremsg(FALSE);
    માટે (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	જો (used_typed_char != NUL)
	{
	    c = used_typed_char;	// was typed at hit-enter prompt
	    used_typed_char = NUL;
	}
	નહીંતર
	    c = get_keystroke();

#જો defined(FEAT_MENU) && defined(FEAT_GUI)
	જો (c == K_MENU)
	{
	    પૂર્ણાંક idx = get_menu_index(current_menu, MODE_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    જો (idx == MENU_INDEX_INVALID)
		ચાલુ;
	    c = *current_menu->strings[idx];
	    જો (c != NUL && current_menu->strings[idx][1] != NUL)
		ins_typebuf(current_menu->strings[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#endif

	toscroll = 0;
	પસંદગી (c)
	{
	સ્થિતિ BS:		// scroll one line back
	સ્થિતિ K_BS:
	સ્થિતિ 'k':
	સ્થિતિ K_UP:
	    toscroll = -1;
	    તોડો;

	સ્થિતિ CAR:		// one extra line
	સ્થિતિ NL:
	સ્થિતિ 'j':
	સ્થિતિ K_DOWN:
	    toscroll = 1;
	    તોડો;

	સ્થિતિ 'u':		// Up half a page
	    toscroll = -(Rows / 2);
	    તોડો;

	સ્થિતિ 'd':		// Down half a page
	    toscroll = Rows / 2;
	    તોડો;

	સ્થિતિ 'b':		// one page back
	સ્થિતિ Ctrl_B:
	સ્થિતિ K_PAGEUP:
	    toscroll = -(Rows - 1);
	    તોડો;

	સ્થિતિ ' ':		// one extra page
	સ્થિતિ 'f':
	સ્થિતિ Ctrl_F:
	સ્થિતિ K_PAGEDOWN:
	સ્થિતિ K_LEFTMOUSE:
	    toscroll = Rows - 1;
	    તોડો;

	સ્થિતિ 'g':		// all the way back to the start
	    toscroll = -999999;
	    તોડો;

	સ્થિતિ 'G':		// all the way to the end
	    toscroll = 999999;
	    લીટીઓ_ડાબું = 999999;
	    તોડો;

	સ્થિતિ ':':		// start new command line
#ifdef FEAT_CON_DIALOG
	    જો (!confirm_સંદેશ_used)
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
	સ્થિતિ 'q':		// quit
	સ્થિતિ Ctrl_C:
	સ્થિતિ ESC:
#ifdef FEAT_CON_DIALOG
	    જો (confirm_સંદેશ_used)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    નહીંતર
#endif
	    {
		got_int = TRUE;
		quit_more = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    લીટીઓ_ડાબું = Rows - 1;
	    તોડો;

#ifdef FEAT_CLIPBOARD
	સ્થિતિ Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    જો (clip_star.state == SELECT_DONE)
		clip_copy_modeless_selection(TRUE);
	    ચાલુ;
#endif
	મૂળભૂત:		// no valid response
	    સંદેશ_moremsg(TRUE);
	    ચાલુ;
	}

	જો (toscroll != 0)
	{
	    જો (toscroll < 0)
	    {
		// go to start of last line
		જો (mp_last == ખાલી)
		    mp = સંદેશ_sb_start(last_msgchunk);
		નહીંતર જો (mp_last->sb_prev != ખાલી)
		    mp = સંદેશ_sb_start(mp_last->sb_prev);
		નહીંતર
		    mp = ખાલી;

		// go to start of line at top of the screen
		માટે (i = 0; i < Rows - 2 && mp != ખાલી && mp->sb_prev != ખાલી;
									  ++i)
		    mp = સંદેશ_sb_start(mp->sb_prev);

		જો (mp != ખાલી && mp->sb_prev != ખાલી)
		{
		    // Find line to be displayed at top.
		    માટે (i = 0; i > toscroll; --i)
		    {
			જો (mp == ખાલી || mp->sb_prev == ખાલી)
			    તોડો;
			mp = સંદેશ_sb_start(mp->sb_prev);
			જો (mp_last == ખાલી)
			    mp_last = સંદેશ_sb_start(last_msgchunk);
			નહીંતર
			    mp_last = સંદેશ_sb_start(mp_last->sb_prev);
		    }

		    જો (toscroll == -1 && screen_ins_લીટીઓ(0, 0, 1,
						     (પૂર્ણાંક)Rows, 0, ખાલી) == OK)
		    {
			// display line at top
			(શૂન્ય)disp_sb_લીટી(0, mp, FALSE);
		    }
		    નહીંતર
		    {
			પૂર્ણાંક did_clear = screenclear();

			// redisplay all lines
			માટે (i = 0; mp != ખાલી && i < Rows - 1; ++i)
			{
			    mp = disp_sb_લીટી(i, mp, !did_clear);
			    ++સંદેશ_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    નહીંતર
	    {
		// First display any text that we scrolled back.
		જ્યારે (toscroll > 0 && mp_last != ખાલી)
		{
		    // scroll up, display line at bottom
		    સંદેશ_scroll_up();
		    inc_સંદેશ_scrolled();
		    screen_fill((પૂર્ણાંક)Rows - 2, (પૂર્ણાંક)Rows - 1,
			    cmdline_col_off, cmdline_col_off + cmdline_width,
			    ' ', ' ', સંદેશ_attr);
		    mp_last = disp_sb_લીટી((પૂર્ણાંક)Rows - 2, mp_last, FALSE);
		    --toscroll;
		}
	    }

	    જો (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((પૂર્ણાંક)Rows - 1, (પૂર્ણાંક)Rows, cmdline_col_off,
			cmdline_col_off + cmdline_width, ' ', ' ', સંદેશ_attr);
		સંદેશ_moremsg(FALSE);
		ચાલુ;
	    }

	    // display more text, return to caller
	    લીટીઓ_ડાબું = toscroll;
	}

	તોડો;
    }

    // clear the --more-- message
    screen_fill((પૂર્ણાંક)Rows - 1, (પૂર્ણાંક)Rows, cmdline_col_off,
	    cmdline_col_off + cmdline_width, ' ', ' ', સંદેશ_attr);
    State = oldState;
    setmouse();
    જો (quit_more)
    {
	સંદેશ_row = Rows - 1;
	સંદેશ_col = 0;
    }
#ifdef FEAT_RIGHTLEFT
    નહીંતર જો (cmdmsg_rl)
	સંદેશ_col = cmdline_width - 1;
#endif

    entered = FALSE;
#ifdef FEAT_CON_DIALOG
    પાછા retval;
#નહીંતર
    પાછા FALSE;
#endif
}

#જો defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_સંદેશ
#  undef mch_સંદેશ
# endif

# જો defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    સ્થિર શૂન્ય
mch_errmsg_c(અક્ષર *str)
{
    DWORD   nwrite = 0;
    DWORD   mode = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    જો (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (પૂર્ણાંક)GetConsoleCP() != enc_codepage)
    {
	પૂર્ણાંક	len = (પૂર્ણાંક)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	જો (w != ખાલી)
	{
	    WriteConsoleW(h, w, len, &nwrite, ખાલી);
	    vim_free(w);
	}
    }
    નહીંતર
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
    શૂન્ય
mch_errmsg(અક્ષર *str)
{
# જો !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    પૂર્ણાંક		len;
# endif

# જો (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    જો (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   નહીંતર
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
	પાછા;
    }
# endif

# જો defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    જો (!(gui.in_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	પાછા;
    }
# endif

# જો !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (પૂર્ણાંક)STRLEN(str) + 1;
    જો (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    જો (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_data + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_data + error_ga.ga_len;
	    માટે (;;)
	    {
		p = vim_strchr(p, '\r');
		જો (p == ખાલી)
		    તોડો;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# જો defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    સ્થિર શૂન્ય
mch_સંદેશ_c(અક્ષર *str)
{
    DWORD   nwrite = 0;
    DWORD   mode;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    જો (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (પૂર્ણાંક)GetConsoleCP() != enc_codepage)
    {
	પૂર્ણાંક	len = (પૂર્ણાંક)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	જો (w != ખાલી)
	{
	    WriteConsoleW(h, w, len, &nwrite, ખાલી);
	    vim_free(w);
	}
    }
    નહીંતર
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
    શૂન્ય
mch_સંદેશ(અક્ષર *str)
{
# જો (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    જો (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   નહીંતર
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
	પાછા;
    }
# endif

# જો defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    જો (!(gui.in_use || gui.starting))
#  endif
    {
	mch_સંદેશ_c(str);
	પાછા;
    }
# endif
# જો !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#endif // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    સ્થિર શૂન્ય
સંદેશ_screen_putchar(પૂર્ણાંક c, પૂર્ણાંક attr)
{
    સંદેશ_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, સંદેશ_row, cmdline_col_off + સંદેશ_col, attr);
#ifdef FEAT_RIGHTLEFT
    જો (cmdmsg_rl)
    {
	જો (--સંદેશ_col == 0)
	{
	    સંદેશ_col = cmdline_width;
	    ++સંદેશ_row;
	}
    }
    નહીંતર
#endif
    {
	જો (++સંદેશ_col >= cmdline_width)
	{
	    સંદેશ_col = 0;
	    ++સંદેશ_row;
	}
    }
}

    સ્થિર શૂન્ય
સંદેશ_moremsg(પૂર્ણાંક full)
{
    પૂર્ણાંક		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (પૂર્ણાંક)Rows - 1, cmdline_col_off, attr);
    જો (full)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(પૂર્ણાંક)Rows - 1, cmdline_col_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    શૂન્ય
repeat_message(શૂન્ય)
{
    જો (State == MODE_ASKMORE)
    {
	સંદેશ_moremsg(TRUE);	// display --more-- message again
	સંદેશ_row = Rows - 1;
    }
#ifdef FEAT_CON_DIALOG
    નહીંતર જો (State == MODE_CONFIRM)
    {
	display_confirm_સંદેશ();	// display ":confirm" message again
	સંદેશ_row = Rows - 1;
    }
#endif
    નહીંતર જો (State == MODE_EXTERNCMD)
    {
	windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col); // put cursor back
    }
    નહીંતર જો (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	જો (સંદેશ_row == Rows - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    સંદેશ_didout = FALSE;
	    સંદેશ_col = 0;
	    સંદેશ_clr_eos();
	}
	hit_return_સંદેશ();
	સંદેશ_row = Rows - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    સ્થિર પૂર્ણાંક
સંદેશ_check_screen(શૂન્ય)
{
    જો (!full_screen || !screen_valid(FALSE))
	પાછા FALSE;

    જો (સંદેશ_row >= Rows)
	સંદેશ_row = Rows - 1;
    જો (સંદેશ_col >= cmdline_width)
	સંદેશ_col = cmdline_width - 1;
    પાછા TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    શૂન્ય
સંદેશ_clr_eos(શૂન્ય)
{
    જો (સંદેશ_silent == 0)
	સંદેશ_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    શૂન્ય
સંદેશ_clr_eos_force(શૂન્ય)
{
#ifdef HAS_MESSAGE_WINDOW
    જો (in_echowindow)
	પાછા;  // messages go into a popup
#endif
    જો (સંદેશ_use_printf())
    {
	જો (full_screen)	// only when termcap codes are valid
	{
	    જો (*T_CD)
		out_str(T_CD);	// clear to end of display
	    નહીંતર જો (*T_CE)
		out_str(T_CE);	// clear to end of line
	}
    }
    નહીંતર
    {
	પૂર્ણાંક સંદેશ_attr;

	સંદેશ_attr = HL_ATTR(HLF_સંદેશ);

#ifdef FEAT_RIGHTLEFT
	જો (cmdmsg_rl)
	{
	    screen_fill(સંદેશ_row, સંદેશ_row + 1,
		    cmdline_col_off, cmdline_col_off + સંદેશ_col + 1,
		    ' ', ' ', સંદેશ_attr);
	    screen_fill(સંદેશ_row + 1, (પૂર્ણાંક)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', સંદેશ_attr);
	}
	નહીંતર
#endif
	{
	    screen_fill(સંદેશ_row, સંદેશ_row + 1,
		    cmdline_col_off + સંદેશ_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', સંદેશ_attr);
	    screen_fill(સંદેશ_row + 1, (પૂર્ણાંક)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', સંદેશ_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    શૂન્ય
સંદેશ_clr_cmdline(શૂન્ય)
{
    સંદેશ_row = cmdline_row;
    સંદેશ_col = 0;
    સંદેશ_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    પૂર્ણાંક
સંદેશ_end(શૂન્ય)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    જો (!exiting && need_wait_return && !(State & MODE_CMDLINE))
    {
	wait_return(FALSE);
	પાછા FALSE;
    }
    out_flush();
    પાછા TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    શૂન્ય
સંદેશ_check(શૂન્ય)
{
    જો (સંદેશ_row == Rows - 1 && સંદેશ_col >= sc_col
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
    સ્થિર શૂન્ય
redir_write(char_u *str, પૂર્ણાંક maxlen)
{
    char_u	*s = str;
    સ્થિર પૂર્ણાંક	cur_col = 0;

    // Don't do anything for displaying prompts and the like.
    જો (redir_off)
	પાછા;

    // If 'verbosefile' is set prepare for writing in that file.
    જો (*p_vfile != NUL && verbose_fd == ખાલી)
	verbose_open();

    જો (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	જો (*s != '\n' && *s != '\r')
	{
	    જ્યારે (cur_col < સંદેશ_col)
	    {
#ifdef FEAT_EVAL
		જો (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		નહીંતર જો (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		નહીંતર જો (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		નહીંતર
#endif
		    જો (redir_fd != ખાલી)
		    fputs(" ", redir_fd);
		જો (verbose_fd != ખાલી)
		    fputs(" ", verbose_fd);
		++cur_col;
	    }
	}

#ifdef FEAT_EVAL
	જો (redir_execute)
	    execute_redir_str(s, maxlen);
	નહીંતર જો (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	નહીંતર જો (redir_vname)
	    var_redir_str(s, maxlen);
#endif

	// Write and adjust the current column.
	જ્યારે (*s != NUL && (maxlen < 0 || (પૂર્ણાંક)(s - str) < maxlen))
	{
#ifdef FEAT_EVAL
	    જો (!redir_reg && !redir_vname && !redir_execute)
#endif
		જો (redir_fd != ખાલી)
		    putc(*s, redir_fd);
	    જો (verbose_fd != ખાલી)
		putc(*s, verbose_fd);
	    જો (*s == '\r' || *s == '\n')
		cur_col = 0;
	    નહીંતર જો (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    નહીંતર
		++cur_col;
	    ++s;
	}

	જો (સંદેશ_silent != 0)	// should update msg_col
	    સંદેશ_col = cur_col;
    }
}

    પૂર્ણાંક
redirecting(શૂન્ય)
{
    પાછા redir_fd != ખાલી || *p_vfile != NUL
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#endif
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    શૂન્ય
verbose_enter(શૂન્ય)
{
    જો (*p_vfile != NUL)
	++સંદેશ_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    શૂન્ય
verbose_leave(શૂન્ય)
{
    જો (*p_vfile != NUL)
	જો (--સંદેશ_silent < 0)
	    સંદેશ_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    શૂન્ય
verbose_enter_scroll(શૂન્ય)
{
    જો (*p_vfile != NUL)
	++સંદેશ_silent;
    નહીંતર
	// always scroll up, don't overwrite
	સંદેશ_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    શૂન્ય
verbose_leave_scroll(શૂન્ય)
{
    જો (*p_vfile != NUL)
    {
	જો (--સંદેશ_silent < 0)
	    સંદેશ_silent = 0;
    }
    નહીંતર
	cmdline_row = સંદેશ_row;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    શૂન્ય
verbose_stop(શૂન્ય)
{
    જો (verbose_fd != ખાલી)
    {
	fclose(verbose_fd);
	verbose_fd = ખાલી;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    પૂર્ણાંક
verbose_open(શૂન્ય)
{
    જો (verbose_fd == ખાલી && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((અક્ષર *)p_vfile, "a");
	જો (verbose_fd == ખાલી)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    પાછા FAIL;
	}
    }
    પાછા OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    શૂન્ય
give_warning(char_u *message, પૂર્ણાંક hl)
{
    give_warning_with_source(message, hl, FALSE);
}

    શૂન્ય
give_warning_with_source(char_u *message, પૂર્ણાંક hl, પૂર્ણાંક with_source)
{
    // Don't do this for ":silent".
    જો (સંદેશ_silent != 0)
	પાછા;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, message, -1);
#endif
    VIM_CLEAR(keep_સંદેશ);
    જો (hl)
	keep_સંદેશ_attr = HL_ATTR(HLF_W);
    નહીંતર
	keep_સંદેશ_attr = 0;

    જો (with_source)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	સંદેશ_start();
	સંદેશ_source(HL_ATTR(HLF_W));
	સંદેશ_puts(" ");
	સંદેશ_puts_attr((અક્ષર *)message, HL_ATTR(HLF_W) | સંદેશ_HIST);
	સંદેશ_clr_eos();
	(શૂન્ય)સંદેશ_end();
    }
    નહીંતર જો (સંદેશ_attr((અક્ષર *)message, keep_સંદેશ_attr) && સંદેશ_scrolled == 0)
	set_keep_સંદેશ(message, keep_સંદેશ_attr);

    સંદેશ_didout = FALSE;	    // overwrite this message
    સંદેશ_nowait = TRUE;	    // don't wait for this message
    સંદેશ_col = 0;

    --no_wait_return;
}

#જો defined(FEAT_EVAL)
    શૂન્ય
give_warning2(char_u *message, char_u *a1, પૂર્ણાંક hl)
{
    જો (IObuff == ખાલી)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(message, hl);
    }
    નહીંતર
    {
	vim_snprintf((અક્ષર *)IObuff, IOSIZE, (અક્ષર *)message, a1);
	give_warning(IObuff, hl);
    }
}
#endif

/*
 * Advance msg cursor to column "col".
 */
    શૂન્ય
સંદેશ_advance(પૂર્ણાંક col)
{
    જો (સંદેશ_silent != 0)	// nothing to advance to
    {
	સંદેશ_col = col;		// for redirection, may fill it up later
	પાછા;
    }
    જો (col >= cmdline_width)	// not enough room
	col = cmdline_width - 1;
#ifdef FEAT_RIGHTLEFT
    જો (cmdmsg_rl)
	જ્યારે (સંદેશ_col > cmdline_width - col)
	    સંદેશ_putchar(' ');
    નહીંતર
#endif
	જ્યારે (સંદેશ_col < col)
	    સંદેશ_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    શૂન્ય
સંદેશ_warn_missing_clipboard(શૂન્ય)
{
    જો (!global_busy && !did_warn_clipboard && silence_w23_w24_સંદેશ == 0)
    {
#ifdef FEAT_CLIPBOARD
	સંદેશ(_("W23: Clipboard register not available, using register 0"));
#નહીંતર
	સંદેશ(_("W24: Clipboard register not available. See :h W24"));
#endif
	did_warn_clipboard = TRUE;
    }
}

#જો defined(FEAT_CON_DIALOG)
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
    પૂર્ણાંક
do_dialog(
    પૂર્ણાંક		type UNUSED,
    char_u	*title UNUSED,
    char_u	*message,
    char_u	*buttons,
    પૂર્ણાંક		dfltbutton,
    char_u	*textfield UNUSED,	// IObuff for inputdialog(), NULL
					// otherwise
    પૂર્ણાંક		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    પૂર્ણાંક		oldState;
    પૂર્ણાંક		retval = 0;
    char_u	*hotkeys;
    પૂર્ણાંક		c;
    પૂર્ણાંક		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    જો (silent_mode)
	પાછા dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    જો (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == ખાલી)
    {
	// --gui-dialog-file: write text to a file
	જો (gui_dialog_log(title, message))
	    c = dfltbutton;
	નહીંતર
	    c = gui_mch_dialog(type, title, message, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_row = સંદેશ_row;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	out_flush();
	gui_mch_update();

	પાછા c;
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
    hotkeys = સંદેશ_show_console_dialog(message, buttons, dfltbutton);

    જો (hotkeys != ખાલી)
    {
	માટે (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    પસંદગી (c)
	    {
	    સ્થિતિ CAR:		// User accepts default option
	    સ્થિતિ NL:
		retval = dfltbutton;
		તોડો;
	    સ્થિતિ Ctrl_C:	// User aborts/cancels
	    સ્થિતિ ESC:
		retval = 0;
		તોડો;
	    મૂળભૂત:		// Could be a hotkey?
		જો (c < 0)	// special keys are ignored here
		    ચાલુ;
		જો (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    તોડો;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		માટે (i = 0; hotkeys[i]; ++i)
		{
		    જો (has_mbyte)
		    {
			જો ((*mb_ptr2char)(hotkeys + i) == c)
			    તોડો;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    નહીંતર
			જો (hotkeys[i] == c)
			    તોડો;
		    ++retval;
		}
		જો (hotkeys[i])
		    તોડો;
		// No hotkey match, so keep waiting
		ચાલુ;
	    }
	    તોડો;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    સંદેશ_end_prompt();

    પાછા retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    સ્થિર પૂર્ણાંક
copy_char(
    char_u	*from,
    char_u	*to,
    પૂર્ણાંક		lowercase)	// make character lower case
{
    પૂર્ણાંક		len;
    પૂર્ણાંક		c;

    જો (has_mbyte)
    {
	જો (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    પાછા (*mb_char2bytes)(c, to);
	}
	નહીંતર
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(to, from, (size_t)len);
	    પાછા len;
	}
    }
    નહીંતર
    {
	જો (lowercase)
	    *to = (char_u)TOLOWER_LOC(*from);
	નહીંતર
	    *to = *from;
	પાછા 1;
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
    સ્થિર char_u *
સંદેશ_show_console_dialog(
    char_u	*message,
    char_u	*buttons,
    પૂર્ણાંક		dfltbutton)
{
    પૂર્ણાંક		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    પૂર્ણાંક		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = ખાલી;
    char_u	*msgp = ખાલી;
    char_u	*hotkp = ખાલી;
    char_u	*r;
    પૂર્ણાંક		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    પૂર્ણાંક		first_hotkey = FALSE;	// first char of button is hotkey
    પૂર્ણાંક		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    માટે (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	જ્યારે (*r)
	{
	    જો (*r == DLG_BUTTON_SEP)
	    {
		જો (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    જો (has_mbyte)
			hotkp += STRLEN(hotkp);
		    નહીંતર
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    જો (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    જો (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			first_hotkey = TRUE;
		}
		નહીંતર
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    જો (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    નહીંતર જો (*r == DLG_HOTKEY_CHAR || first_hotkey)
	    {
		જો (*r == DLG_HOTKEY_CHAR)
		    ++r;
		first_hotkey = FALSE;
		જો (copy)
		{
		    જો (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    નહીંતર
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		નહીંતર
		{
		    ++len;	    // '&a' -> '[a]'
		    જો (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    નહીંતર
	    {
		// everything else copy literally
		જો (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	જો (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	નહીંતર
	{
	    len += (પૂર્ણાંક)(STRLEN(message)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    જો (!has_hotkey[0])
	    {
		first_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_સંદેશ);
	    confirm_સંદેશ = alloc(len);
	    જો (confirm_સંદેશ == ખાલી)
		પાછા ખાલી;
	    *confirm_સંદેશ = NUL;
	    hotk = alloc(lenhotkey);
	    જો (hotk == ખાલી)
		પાછા ખાલી;

	    *confirm_સંદેશ = '\n';
	    STRCPY(confirm_સંદેશ + 1, message);

	    msgp = confirm_સંદેશ + 1 + STRLEN(message);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_સંદેશ_tail = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_સંદેશ();
    પાછા hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    સ્થિર શૂન્ય
display_confirm_સંદેશ(શૂન્ય)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_સંદેશ_used;
    જો (confirm_સંદેશ != ખાલી)
	સંદેશ_puts_attr((અક્ષર *)confirm_સંદેશ, HL_ATTR(HLF_M));
    --confirm_સંદેશ_used;
}

#endif // FEAT_CON_DIALOG

#જો defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    પૂર્ણાંક
vim_dialog_yesno(
    પૂર્ણાંક		type,
    char_u	*title,
    char_u	*message,
    પૂર્ણાંક		dflt)
{
    જો (do_dialog(type,
		title == ખાલી ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No"), dflt, ખાલી, FALSE) == 1)
	પાછા VIM_YES;
    પાછા VIM_NO;
}

    પૂર્ણાંક
vim_dialog_yesnocancel(
    પૂર્ણાંક		type,
    char_u	*title,
    char_u	*message,
    પૂર્ણાંક		dflt)
{
    પસંદગી (do_dialog(type,
		title == ખાલી ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, ખાલી, FALSE))
    {
	સ્થિતિ 1: પાછા VIM_YES;
	સ્થિતિ 2: પાછા VIM_NO;
    }
    પાછા VIM_CANCEL;
}

    પૂર્ણાંક
vim_dialog_yesnoallcancel(
    પૂર્ણાંક		type,
    char_u	*title,
    char_u	*message,
    પૂર્ણાંક		dflt)
{
    પસંદગી (do_dialog(type,
		title == ખાલી ? (char_u *)"Question" : title,
		message,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, ખાલી, FALSE))
    {
	સ્થિતિ 1: પાછા VIM_YES;
	સ્થિતિ 2: પાછા VIM_NO;
	સ્થિતિ 3: પાછા VIM_ALL;
	સ્થિતિ 4: પાછા VIM_DISCARDALL;
    }
    પાછા VIM_CANCEL;
}

#endif // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
