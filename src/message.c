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

#నిర్వచించు సందేశం_FILE		// don't include prototype for smsg()

#అన్తర్భావ "vim.h"

స్థిర శూన్య కూడు_సందేశం_hist(char_u *s, పూర్ణాంకం len, పూర్ణాంకం attr);
స్థిర శూన్య check_సందేశం_hist(శూన్య);
స్థిర శూన్య తగిలింది_return_సందేశం(శూన్య);
స్థిర శూన్య సందేశం_home_replace_attr(char_u *fname, పూర్ణాంకం attr);
స్థిర శూన్య సందేశం_puts_attr_len(అక్షరం *str, పూర్ణాంకం maxlen, పూర్ణాంకం attr);
స్థిర శూన్య సందేశం_puts_display(char_u *str, పూర్ణాంకం maxlen, పూర్ణాంకం attr, పూర్ణాంకం recurse);
స్థిర శూన్య సందేశం_scroll_పైకి(శూన్య);
స్థిర శూన్య inc_సందేశం_scrolled(శూన్య);
స్థిర శూన్య store_sb_పాఠ్యం(char_u **sb_str, char_u *s, పూర్ణాంకం attr, పూర్ణాంకం *sb_నిలువు, పూర్ణాంకం ముగింపు);
స్థిర శూన్య t_puts(పూర్ణాంకం *t_నిలువు, char_u *t_s, char_u *s, పూర్ణాంకం attr);
స్థిర శూన్య సందేశం_puts_printf(char_u *str, పూర్ణాంకం maxlen);
స్థిర పూర్ణాంకం do_మరిన్ని_prompt(పూర్ణాంకం typed_char);
స్థిర శూన్య సందేశం_screen_putchar(పూర్ణాంకం c, పూర్ణాంకం attr);
స్థిర శూన్య సందేశం_moremsg(పూర్ణాంకం నిండిన);
స్థిర పూర్ణాంకం  సందేశం_check_screen(శూన్య);
స్థిర శూన్య redir_write(char_u *s, పూర్ణాంకం maxlen);
#నిర్వచితమైతే FEAT_CON_DIALOG
స్థిర char_u *సందేశం_show_console_dialog(char_u *సందేశం_2, char_u *buttons, పూర్ణాంకం dfltbutton);
స్థిర పూర్ణాంకం	confirm_సందేశం_వాడిన = FALSE;	// displaying confirm_msg
స్థిర char_u	*confirm_సందేశం = ఖాళీ;		// ":confirm" message
స్థిర char_u	*confirm_సందేశం_తోక;		// tail of confirm_msg
స్థిర శూన్య display_confirm_సందేశం(శూన్య);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
స్థిర పూర్ణాంకం emsg_గమ్యం_channel_log = FALSE;
#అయితే_ముగింపు

నిర్మాణం సందేశం_hist
{
    నిర్మాణం సందేశం_hist	*తదుపరి;
    char_u		*సందేశం;
    పూర్ణాంకం			attr;
};

స్థిర నిర్మాణం సందేశం_hist *మొదటి_సందేశం_hist = ఖాళీ;
స్థిర నిర్మాణం సందేశం_hist *చివరి_సందేశం_hist = ఖాళీ;
స్థిర పూర్ణాంకం సందేశం_hist_len = 0;
స్థిర పూర్ణాంకం సందేశం_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#నిర్వచించు MESSAGES_తగిలింది_ENTER	0x001
#నిర్వచించు MESSAGES_WAIT		0x002
#నిర్వచించు MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#నిర్వచించు MESSAGES_OPT_తగిలింది_ENTER "hit-enter"
#నిర్వచించు MESSAGES_OPT_WAIT "wait:"
#నిర్వచించు MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
స్థిర పూర్ణాంకం సందేశం_flags = MESSAGES_తగిలింది_ENTER | MESSAGES_HISTORY;
స్థిర పూర్ణాంకం సందేశం_wait = 0;

స్థిర FILE *verbose_fd = ఖాళీ;
స్థిర పూర్ణాంకం  verbose_did_open = FALSE;

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
    పూర్ణాంకం
సందేశం(అక్షరం *s)
{
    తిరిగి సందేశం_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    పూర్ణాంకం
verb_సందేశం(అక్షరం *s)
{
    పూర్ణాంకం		n;

    verbose_enter();
    n = సందేశం_attr_keep(s, 0, FALSE);
    verbose_leave();

    తిరిగి n;
}

    పూర్ణాంకం
సందేశం_attr(అక్షరం *s, పూర్ణాంకం attr)
{
    తిరిగి సందేశం_attr_keep(s, attr, FALSE);
}

    పూర్ణాంకం
సందేశం_attr_keep(
    అక్షరం	*s,
    పూర్ణాంకం		attr,
    పూర్ణాంకం		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    స్థిర పూర్ణాంకం	entered = 0;
    పూర్ణాంకం		retval;
    char_u	*buf = ఖాళీ;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    ఒకవేళ (!emsg_on_display && సందేశం_filtered((char_u *)s))
	తిరిగి TRUE;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (attr == 0)
	సమితి_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#అయితే_ముగింపు

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    ఒకవేళ (entered >= 3)
	తిరిగి TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    ఒకవేళ ((char_u *)s != keep_సందేశం
	    || (*s != '<'
		&& చివరి_సందేశం_hist != ఖాళీ
		&& చివరి_సందేశం_hist->సందేశం != ఖాళీ
		&& STRCMP(s, చివరి_సందేశం_hist->సందేశం) != 0))
	కూడు_సందేశం_hist((char_u *)s, -1, attr);

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (emsg_గమ్యం_channel_log)
	// Write message in the channel log.
	ch_log(ఖాళీ, "ERROR: %s", s);
#అయితే_ముగింపు

    // Truncate the message if needed.
    సందేశం_ప్రారంభం();
    buf = సందేశం_strtrunc((char_u *)s, FALSE);
    ఒకవేళ (buf != ఖాళీ)
	s = (అక్షరం *)buf;

    సందేశం_outtrans_attr((char_u *)s, attr);
    సందేశం_clr_eos();
    retval = సందేశం_చివర();

    ఒకవేళ (keep && retval && vim_strsize((char_u *)s)
		    < (పూర్ణాంకం)(అడ్డువరుసలు - cmdline_అడ్డువరుస - 1) * cmdline_వెడల్పు + sc_నిలువు)
	సమితి_keep_సందేశం((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    తిరిగి retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
సందేశం_strtrunc(
    char_u	*s,
    పూర్ణాంకం		force)	    // always truncate
{
    char_u	*buf = ఖాళీ;
    పూర్ణాంకం		len;
    పూర్ణాంకం		స్థలం;

    // May truncate message to avoid a hit-return prompt
    ఒకవేళ ((!సందేశం_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_చురుకైన && సందేశం_silent == 0) || force)
    {
	len = vim_strsize(s);
	ఒకవేళ (సందేశం_scrolled != 0
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
		|| లోపల_echowindow
#అయితే_ముగింపు
		)
	    // Use all the columns.
	    స్థలం = (పూర్ణాంకం)(అడ్డువరుసలు - సందేశం_అడ్డువరుస) * cmdline_వెడల్పు - 1;
	లేకపోతే
	    // Use up to 'showcmd' column.
	    స్థలం = (పూర్ణాంకం)(అడ్డువరుసలు - సందేశం_అడ్డువరుస - 1) * cmdline_వెడల్పు + sc_నిలువు - 1;
	ఒకవేళ (len > స్థలం && స్థలం > 0)
	{
	    ఒకవేళ (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (స్థలం + 2) * 18;
	    లేకపోతే ఒకవేళ (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (స్థలం + 2) * 2;
	    లేకపోతే
		len = స్థలం + 2;
	    buf = alloc(len);
	    ఒకవేళ (buf != ఖాళీ)
		trunc_string(s, buf, స్థలం, len);
	}
    }
    తిరిగి buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    శూన్య
trunc_string(
    char_u	*s,
    char_u	*buf,
    పూర్ణాంకం		స్థలం_లోపల,
    పూర్ణాంకం		buflen)
{
    size_t	స్థలం = స్థలం_లోపల - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    పూర్ణాంకం		e;
    పూర్ణాంకం		i;
    పూర్ణాంకం		n;

    ఒకవేళ (*s == NUL)
    {
	ఒకవేళ (buflen > 0)
	    *buf = NUL;
	తిరిగి;
    }

    ఒకవేళ (స్థలం_లోపల < 3)
	స్థలం = 0;
    half = స్థలం / 2;

    // First part: Start of the string.
    కొరకు (e = 0; len < half && e < buflen; ++e)
    {
	ఒకవేళ (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    తిరిగి;
	}
	n = ptr2cells(s + e);
	ఒకవేళ (len + n > half)
	    ఆపు;
	len += n;
	buf[e] = s[e];
	ఒకవేళ (has_mbyte)
	    కొరకు (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		ఒకవేళ (++e == buflen)
		    ఆపు;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    ఒకవేళ (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	అయితే (len + n > స్థలం)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    లేకపోతే ఒకవేళ (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (పూర్ణాంకం)STRLEN(s);
	కొరకు (;;)
	{
	    చేయి
		half = half - utf_తల_off(s, s + half - 1) - 1;
	    అయితే (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    ఒకవేళ (len + n > స్థలం || half == 0)
		ఆపు;
	    len += n;
	    i = (పూర్ణాంకం)half;
	}
    }
    లేకపోతే
    {
	కొరకు (i = (పూర్ణాంకం)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= స్థలం; --i)
	    len += n;
    }


    ఒకవేళ (i <= e + 3)
    {
	// text fits without truncating
	ఒకవేళ (s != buf)
	{
	    len = STRLEN(s);
	    ఒకవేళ (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    ఒకవేళ (len < 1)
		buf[e - 1] = NUL;
	    లేకపోతే
		mch_memmove(buf + e, s + e, len);
	}
    }
    లేకపోతే ఒకవేళ (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	ఒకవేళ (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    లేకపోతే
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
#నిర్వచితం_కాకపోతే PROTO

పూర్ణాంకం vim_snprintf(అక్షరం *str, size_t str_m, స్థిరరాశి అక్షరం *fmt, ...);

    పూర్ణాంకం
smsg(స్థిరరాశి అక్షరం *s, ...)
{
    ఒకవేళ (IObuff == ఖాళీ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	తిరిగి సందేశం((అక్షరం *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((అక్షరం *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    తిరిగి సందేశం((అక్షరం *)IObuff);
}

    పూర్ణాంకం
smsg_attr(పూర్ణాంకం attr, స్థిరరాశి అక్షరం *s, ...)
{
    ఒకవేళ (IObuff == ఖాళీ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	తిరిగి సందేశం_attr((అక్షరం *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((అక్షరం *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    తిరిగి సందేశం_attr((అక్షరం *)IObuff, attr);
}

    పూర్ణాంకం
smsg_attr_keep(పూర్ణాంకం attr, స్థిరరాశి అక్షరం *s, ...)
{
    ఒకవేళ (IObuff == ఖాళీ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	తిరిగి సందేశం_attr_keep((అక్షరం *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((అక్షరం *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    తిరిగి సందేశం_attr_keep((అక్షరం *)IObuff, attr, TRUE);
}

#అయితే_ముగింపు

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
స్థిర పూర్ణాంకం	చివరి_sourcing_lnum = 0;
స్థిర char_u   *చివరి_sourcing_name = ఖాళీ;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    శూన్య
reset_చివరి_sourcing(శూన్య)
{
    VIM_CLEAR(చివరి_sourcing_name);
    చివరి_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    స్థిర పూర్ణాంకం
other_sourcing_name(శూన్య)
{
    ఒకవేళ (HAVE_SOURCING_INFO && SOURCING_NAME != ఖాళీ)
    {
	ఒకవేళ (చివరి_sourcing_name != ఖాళీ)
	    తిరిగి STRCMP(SOURCING_NAME, చివరి_sourcing_name) != 0;
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    స్థిర char_u *
get_emsg_ఆధారం(శూన్య)
{
    char_u	*Buf, *p;

    ఒకవేళ (HAVE_SOURCING_INFO && SOURCING_NAME != ఖాళీ && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	ఒకవేళ (sname == ఖాళీ)
	    sname = SOURCING_NAME;

#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	లేకపోతే
#అయితే_ముగింపు
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	ఒకవేళ (Buf != ఖాళీ)
	    sprintf((అక్షరం *)Buf, (అక్షరం *)p, sname);
	vim_free(tofree);
	తిరిగి Buf;
    }
    తిరిగి ఖాళీ;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    స్థిర char_u *
get_emsg_lnum(శూన్య)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    ఒకవేళ (SOURCING_NAME != ఖాళీ
	    && (other_sourcing_name() || SOURCING_LNUM != చివరి_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	ఒకవేళ (Buf != ఖాళీ)
	    sprintf((అక్షరం *)Buf, (అక్షరం *)p, (దీర్ఘ)SOURCING_LNUM);
	తిరిగి Buf;
    }
    తిరిగి ఖాళీ;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    శూన్య
సందేశం_ఆధారం(పూర్ణాంకం attr)
{
    char_u	*p;
    స్థిర పూర్ణాంకం	recursive = FALSE;

    // Bail out if something called here causes an error.
    ఒకవేళ (recursive)
	తిరిగి;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_ఆధారం();
    ఒకవేళ (p != ఖాళీ)
    {
	సందేశం_scroll = TRUE;  // this will take more than one line
	సందేశం_attr((అక్షరం *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    ఒకవేళ (p != ఖాళీ)
    {
	సందేశం_attr((అక్షరం *)p, HL_ATTR(HLF_N));
	vim_free(p);
	చివరి_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    ఒకవేళ (SOURCING_NAME == ఖాళీ || other_sourcing_name())
    {
	VIM_CLEAR(చివరి_sourcing_name);
	ఒకవేళ (SOURCING_NAME != ఖాళీ)
	    చివరి_sourcing_name = vim_strsave(SOURCING_NAME);
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
    స్థిర పూర్ణాంకం
emsg_not_now(శూన్య)
{
    ఒకవేళ ((emsg_off > 0 && vim_strchr(p_debug, 'm') == ఖాళీ
					  && vim_strchr(p_debug, 't') == ఖాళీ)
#నిర్వచితమైతే FEAT_EVAL
	    || emsg_skip > 0
#అయితే_ముగింపు
	    )
	తిరిగి TRUE;
    తిరిగి FALSE;
}

#ఒకవేళ defined(FEAT_EVAL)
స్థిర garray_T ignore_error_list = GA_ఖాళీలు;

    శూన్య
ignore_error_for_testing(char_u *error)
{
    ఒకవేళ (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, పరిమాణం(char_u *), 1);

    ఒకవేళ (STRCMP("RESET", error) == 0)
	ga_clear_అక్షరశ్రేణులు(&ignore_error_list);
    లేకపోతే
	ga_copy_string(&ignore_error_list, error);
}

    స్థిర పూర్ణాంకం
ignore_error(స్థిరరాశి అక్షరం *సందేశం)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < ignore_error_list.ga_len; ++i)
	ఒకవేళ (అక్షరశ్రేణి_శోధన(సందేశం,
		  (అక్షరం *)((char_u **)(ignore_error_list.ga_దత్తాంశం))[i]) != ఖాళీ)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}
#అయితే_ముగింపు

#ఒకవేళ !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    శూన్య
do_perror(అక్షరం *సందేశం)
{
    perror(సందేశం);
    ++emsg_silent;
    emsg(సందేశం);
    --emsg_silent;
}
#అయితే_ముగింపు

/*
 * emsg_core() - display an error message
 *
 * Rings the bell, if appropriate, and calls message() to do the real work
 * When terminal not initialized (yet) mch_errmsg(..) is used.
 *
 * Return TRUE if wait_return() not called.
 * Note: caller must check 'emsg_not_now()' before calling this.
 */
    స్థిర పూర్ణాంకం
emsg_core(స్థిరరాశి అక్షరం *s)
{
    పూర్ణాంకం		attr;
    char_u	*p;
    పూర్ణాంకం		r;
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం		ignore = FALSE;
    పూర్ణాంకం		severe;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    // When testing some errors are turned into a normal message.
    ఒకవేళ (ignore_error(s))
	// don't call msg() if it results in a dialog
	తిరిగి సందేశం_use_printf() ? FALSE : సందేశం((అక్షరం *)s);
#అయితే_ముగింపు

    ++called_emsg;

#నిర్వచితమైతే FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#అయితే_ముగింపు

    ఒకవేళ (!emsg_off || vim_strchr(p_debug, 't') != ఖాళీ)
    {
#నిర్వచితమైతే FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	ఒకవేళ (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    ఒకవేళ (!ignore)
		++did_emsg;
	    తిరిగి TRUE;
	}

	ఒకవేళ (లోపల_assert_fails && emsg_assert_fails_సందేశం == ఖాళీ)
	{
	    emsg_assert_fails_సందేశం = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == ఖాళీ ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	సమితి_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#అయితే_ముగింపు

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	ఒకవేళ (emsg_silent != 0)
	{
#నిర్వచితమైతే FEAT_EVAL
	    ++did_emsg_silent;
#అయితే_ముగింపు
	    ఒకవేళ (emsg_noredir == 0)
	    {
		సందేశం_ప్రారంభం();
		p = get_emsg_ఆధారం();
		ఒకవేళ (p != ఖాళీ)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		ఒకవేళ (p != ఖాళీ)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		redir_write((char_u *)s, -1);
	    }
#నిర్వచితమైతే FEAT_EVAL
	    // Only increment did_emsg_def when :silent! wasn't used inside the
	    // :def function.
	    ఒకవేళ (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	    ch_log(ఖాళీ, "ERROR silent: %s", s);
#అయితే_ముగింపు
	    తిరిగి TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	సందేశం_silent = 0;
	cmd_silent = FALSE;

	ఒకవేళ (global_busy)		// break :global command
	    ++global_busy;

	ఒకవేళ (p_eb)
	    beep_flush();		// also includes flush_buffers()
	లేకపోతే
	    flush_buffers(FLUSH_కనీస);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#నిర్వచితమైతే FEAT_EVAL
	++uncaught_emsg;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే HAS_సందేశం_కిటికీ
    ఒకవేళ (!లోపల_echowindow)
#అయితే_ముగింపు
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    ఒకవేళ (సందేశం_scrolled != 0)
	need_wait_return = TRUE;    // needed in case emsg() is called after
				    // wait_return() has reset need_wait_return
				    // and a redraw is expected because
				    // msg_scrolled is non-zero

#నిర్వచితమైతే FEAT_పని_CHANNEL
    emsg_గమ్యం_channel_log = TRUE;
#అయితే_ముగింపు
    /*
     * Display name and line number for the source of the error.
     */
    సందేశం_scroll = TRUE;
    సందేశం_ఆధారం(attr);

    /*
     * Display the error message itself.
     */
    సందేశం_nowait = FALSE;			// wait for this msg
    r = సందేశం_attr((అక్షరం *)s, attr);

#నిర్వచితమైతే FEAT_పని_CHANNEL
    emsg_గమ్యం_channel_log = FALSE;
#అయితే_ముగింపు
    తిరిగి r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    పూర్ణాంకం
emsg(అక్షరం *s)
{
    // Skip this if not giving error messages at the moment.
    ఒకవేళ (emsg_not_now())
	తిరిగి TRUE;

    తిరిగి emsg_core(s);
}

#నిర్వచితం_కాకపోతే PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    పూర్ణాంకం
semsg(స్థిరరాశి అక్షరం *s, ...)
{
    // Skip this if not giving error messages at the moment.
    ఒకవేళ (emsg_not_now())
	తిరిగి TRUE;

    ఒకవేళ (IObuff == ఖాళీ)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	తిరిగి emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((అక్షరం *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    తిరిగి emsg_core((అక్షరం *)IObuff);
}
#అయితే_ముగింపు

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    శూన్య
iemsg(అక్షరం *s)
{
    ఒకవేళ (emsg_not_now())
	తిరిగి;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_అంతర్గత_error_please_report_a_bug));

    emsg_core(s);
#ఒకవేళ defined(ABORT_ON_అంతర్గత_ERROR) && defined(FEAT_EVAL)
    సమితి_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    సందేశం_putchar('\n');  // avoid overwriting the error message
    బహిర్గతం_flush();
    abort();
#అయితే_ముగింపు
}

#నిర్వచితం_కాకపోతే PROTO  // manual proto with __attribute__
/*
 * Same as semsg(...) but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 * Note: caller must not pass 'IObuff' as 1st argument.
 */
    శూన్య
siemsg(స్థిరరాశి అక్షరం *s, ...)
{
    ఒకవేళ (emsg_not_now())
	తిరిగి;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_అంతర్గత_error_please_report_a_bug));

    ఒకవేళ (IObuff == ఖాళీ)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    లేకపోతే
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((అక్షరం *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((అక్షరం *)IObuff);
    }
# ifdef ABORT_ON_అంతర్గత_ERROR
    సందేశం_putchar('\n');  // avoid overwriting the error message
    బహిర్గతం_flush();
    abort();
# endif
}
#అయితే_ముగింపు

/*
 * Give an "Internal error" message.
 */
    శూన్య
అంతర్గత_error(అక్షరం *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_అంతర్గత_error_please_report_a_bug));

    siemsg(_(e_అంతర్గత_error_str), where);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    శూన్య
అంతర్గత_error_no_abort(అక్షరం *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_అంతర్గత_error_please_report_a_bug));

    semsg(_(e_అంతర్గత_error_str), where);
}
#అయితే_ముగింపు

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    శూన్య
emsg_invreg(పూర్ణాంకం name)
{
    semsg(_(e_invalid_రిజిస్టరు_name_str), transchar_buf(ఖాళీ, name));
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    శూన్య
emsg_namelen(అక్షరం *సందేశం, char_u *name, పూర్ణాంకం len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(సందేశం, copy == ఖాళీ ? "NULL" : (అక్షరం *)copy);
    vim_free(copy);
}
#అయితే_ముగింపు

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    అక్షరం *
సందేశం_trunc_attr(అక్షరం *s, పూర్ణాంకం force, పూర్ణాంకం attr)
{
    పూర్ణాంకం		n;
    అక్షరం	*ts;

    // Add message to history before truncating
    కూడు_సందేశం_hist((char_u *)s, -1, attr);

    ts = (అక్షరం *)సందేశం_may_trunc(force, (char_u *)s);

    సందేశం_hist_off = TRUE;
    n = సందేశం_attr(ts, attr);
    సందేశం_hist_off = FALSE;

    ఒకవేళ (n)
	తిరిగి ts;
    తిరిగి ఖాళీ;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
సందేశం_may_trunc(పూర్ణాంకం force, char_u *s)
{
    పూర్ణాంకం		n;
    పూర్ణాంకం		స్థలం;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    స్థలం = (పూర్ణాంకం)(అడ్డువరుసలు - cmdline_అడ్డువరుస - 1) * cmdline_వెడల్పు + sc_నిలువు - 1;
    ఒకవేళ (స్థలం > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_చురుకైన))
	    && (n = (పూర్ణాంకం)STRLEN(s) - స్థలం) > 0)
    {
	ఒకవేళ (has_mbyte)
	{
	    పూర్ణాంకం	కొలత = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    ఒకవేళ (కొలత <= స్థలం)
		తిరిగి s;

	    కొరకు (n = 0; కొలత >= స్థలం; )
	    {
		కొలత -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    తిరిగి s;
}

    స్థిర శూన్య
కూడు_సందేశం_hist(
    char_u	*s,
    పూర్ణాంకం		len,		// -1 for undetermined length
    పూర్ణాంకం		attr)
{
    నిర్మాణం సందేశం_hist *p;

    ఒకవేళ (సందేశం_hist_off || సందేశం_silent != 0)
	తిరిగి;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(నిర్మాణం సందేశం_hist);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    ఒకవేళ (len < 0)
	len = (పూర్ణాంకం)STRLEN(s);
    // remove leading and trailing newlines
    అయితే (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    అయితే (len > 0 && s[len - 1] == '\n')
	--len;
    p->సందేశం = vim_strnsave(s, len);
    p->తదుపరి = ఖాళీ;
    p->attr = attr;
    ఒకవేళ (చివరి_సందేశం_hist != ఖాళీ)
	చివరి_సందేశం_hist->తదుపరి = p;
    చివరి_సందేశం_hist = p;
    ఒకవేళ (మొదటి_సందేశం_hist == ఖాళీ)
	మొదటి_సందేశం_hist = చివరి_సందేశం_hist;
    ++సందేశం_hist_len;

    check_సందేశం_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    పూర్ణాంకం
delete_మొదటి_సందేశం(శూన్య)
{
    నిర్మాణం సందేశం_hist *p;

    ఒకవేళ (సందేశం_hist_len <= 0)
	తిరిగి FAIL;
    p = మొదటి_సందేశం_hist;
    మొదటి_సందేశం_hist = p->తదుపరి;
    ఒకవేళ (మొదటి_సందేశం_hist == ఖాళీ)
	చివరి_సందేశం_hist = ఖాళీ;  // history is empty
    vim_free(p->సందేశం);
    vim_free(p);
    --సందేశం_hist_len;
    తిరిగి OK;
}

    స్థిర శూన్య
check_సందేశం_hist(శూన్య)
{
    // Don't let the message history get too big
    అయితే (సందేశం_hist_len > 0 && సందేశం_hist_len > సందేశం_hist_max)
	(శూన్య)delete_మొదటి_సందేశం();
}


    పూర్ణాంకం
messagesopt_మారింది(శూన్య)
{
    char_u	*p;
    పూర్ణాంకం		messages_flags_new = 0;
    పూర్ణాంకం		messages_wait_new = 0;
    పూర్ణాంకం		messages_history_new = 0;

    p = p_mopt;
    అయితే (*p != NUL)
    {
	ఒకవేళ (STRNCMP(p, MESSAGES_OPT_తగిలింది_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_తగిలింది_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_తగిలింది_ENTER);
	    messages_flags_new |= MESSAGES_తగిలింది_ENTER;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	ఒకవేళ (*p != ',' && *p != NUL)
	    తిరిగి FAIL;
	ఒకవేళ (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    ఒకవేళ (!(messages_flags_new & (MESSAGES_తగిలింది_ENTER | MESSAGES_WAIT)))
	తిరిగి FAIL;

    // "history" must be set
    ఒకవేళ (!(messages_flags_new & MESSAGES_HISTORY))
	తిరిగి FAIL;

    ఒకవేళ (messages_history_new < 0 || messages_history_new > 10000)
	తిరిగి FAIL;

    ఒకవేళ (messages_wait_new < 0 || messages_wait_new > 10000)
	తిరిగి FAIL;

    సందేశం_flags = messages_flags_new;
    సందేశం_wait = messages_wait_new;

    సందేశం_hist_max = messages_history_new;
    check_సందేశం_hist();

    తిరిగి OK;
}

/*
 * ":messages" command.
 */
    శూన్య
ex_messages(exarg_T *eap)
{
    నిర్మాణం సందేశం_hist *p;
    char_u	    *s;
    పూర్ణాంకం		    c = 0;

    ఒకవేళ (STRCMP(eap->arg, "clear") == 0)
    {
	పూర్ణాంకం keep = eap->addr_లెక్క == 0 ? 0 : eap->line2;

	అయితే (సందేశం_hist_len > keep)
	    (శూన్య)delete_మొదటి_సందేశం();
	తిరిగి;
    }

    ఒకవేళ (*eap->arg != NUL)
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి;
    }

    సందేశం_hist_off = TRUE;

    p = మొదటి_సందేశం_hist;
    ఒకవేళ (eap->addr_లెక్క != 0)
    {
	// Count total messages
	కొరకు (; p != ఖాళీ && !got_int; p = p->తదుపరి)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	కొరకు (p = మొదటి_సందేశం_hist; p != ఖాళీ && !got_int && c > 0;
						    p = p->తదుపరి, c--);
    }

    ఒకవేళ (p == మొదటి_సందేశం_hist)
    {
#నిర్వచితమైతే FEAT_MULTI_LANG
	s = get_mess_lang();
#లేకపోతే
	s = mch_getenv((char_u *)"LANG");
#అయితే_ముగింపు
	ఒకవేళ (s != ఖాళీ && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    సందేశం_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    కొరకు (; p != ఖాళీ && !got_int; p = p->తదుపరి)
	ఒకవేళ (p->సందేశం != ఖాళీ)
	    సందేశం_attr((అక్షరం *)p->సందేశం, p->attr);

    సందేశం_hist_off = FALSE;
}

#ఒకవేళ defined(FEAT_CON_DIALOG) || defined(కనుగొను_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    శూన్య
సందేశం_చివర_prompt(శూన్య)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
    సందేశం_నిలువు = 0;
    సందేశం_clr_eos();
    పంక్తులు_ఎడమ = -1;
}
#అయితే_ముగింపు

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    శూన్య
wait_return(పూర్ణాంకం redraw)
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		oldState;
    పూర్ణాంకం		tmpState;
    పూర్ణాంకం		had_got_int;
    పూర్ణాంకం		save_reg_recording;
    FILE	*save_scriptout;

    ఒకవేళ (redraw == TRUE)
	సమితి_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    ఒకవేళ (సందేశం_silent != 0)
	తిరిగి;
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
    ఒకవేళ (లోపల_echowindow)
	తిరిగి;
#అయితే_ముగింపు

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    ఒకవేళ (vgetc_busy > 0)
	తిరిగి;
    need_wait_return = TRUE;
    ఒకవేళ (no_wait_return)
    {
	ఒకవేళ (!exmode_చురుకైన)
	    cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
	తిరిగి;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    ఒకవేళ (quit_మరిన్ని)
    {
	c = CAR;		// just pretend CR was hit
	quit_మరిన్ని = FALSE;
	got_int = FALSE;
    }
    లేకపోతే ఒకవేళ (exmode_చురుకైన)
    {
	సందేశం_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    లేకపోతే ఒకవేళ (!stuff_ఖాళీలు())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    లేకపోతే
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = బహుళకం_HITRETURN;
	setmouse();
#నిర్వచితమైతే USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#అయితే_ముగింపు
	cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	ఒకవేళ (need_check_timestamps)
	    check_timestamps(FALSE);

	ఒకవేళ (సందేశం_flags & MESSAGES_తగిలింది_ENTER)
	{
	    తగిలింది_return_సందేశం();

	    చేయి
	    {
#నిర్వచితమైతే FEAT_IMAGE
		// Make sure to update image placements (since their row offset
		// will have been changed).
		draw_image_placements();
#అయితే_ముగింపు
		// Remember "got_int", if it is set vgetc() probably returns a
		// CTRL-C, but we need to loop then.
		had_got_int = got_int;

		// Don't do mappings here, we put the character back in the
		// typeahead buffer.
		++no_mapping;
		++allow_కీలకాలు;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = ఖాళీ;
		c = సురక్షిత_vgetc();
		ఒకవేళ (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_కీలకాలు;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#నిర్వచితమైతే FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		ఒకవేళ (కీలకం_Typed && c == Ctrl_Y && clip_నక్షత్రం.state == SELECT_పూర్తి)
		{
		    clip_copy_modeless_selection(TRUE);
		    c = K_IGNORE;
		}
#అయితే_ముగింపు

		/*
		* Allow scrolling back in the messages.
		* Also accept scroll-down commands when messages fill the
		* screen, to avoid that typing one 'j' too many makes the
		* messages disappear.
		*/
		ఒకవేళ (కీలకం_Typed && p_మరిన్ని && !p_cp)
		{
		    ఒకవేళ (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_పైకి || c == K_PAGEUP)
		    {
			ఒకవేళ (సందేశం_scrolled > అడ్డువరుసలు)
			    // scroll back to show older messages
			    do_మరిన్ని_prompt(c);
			లేకపోతే
			{
			    సందేశం_didout = FALSE;
			    c = K_IGNORE;
			    సందేశం_నిలువు =
#నిర్వచితమైతే FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_వెడల్పు - 1 :
#అయితే_ముగింపు
				0;
			}
			ఒకవేళ (quit_మరిన్ని)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_మరిన్ని = FALSE;
			    got_int = FALSE;
			}
			లేకపోతే ఒకవేళ (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    తగిలింది_return_సందేశం();
			}
		    }
		    లేకపోతే ఒకవేళ (సందేశం_scrolled > అడ్డువరుసలు - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_కిందికి || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } అయితే ((had_got_int && c == Ctrl_C)
				|| c == K_IGNORE
#నిర్వచితమైతే FEAT_GUI
				|| c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
#అయితే_ముగింపు
				|| c == K_LEFTDRAG   || c == K_LEFTRELEASE
				|| c == K_MIDDLEDRAG || c == K_MIDDLERELEASE
				|| c == K_RIGHTDRAG  || c == K_RIGHTRELEASE
				|| c == K_MOUSELEFT  || c == K_MOUSERIGHT
				|| c == K_MOUSEDOWN  || c == K_MOUSEUP
				|| c == K_MOUSEMOVE
				|| (!mouse_has(MOUSE_RETURN)
				    && mouse_అడ్డువరుస < సందేశం_అడ్డువరుస
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    ఒకవేళ (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(శూన్య)jump_గమ్యం_mouse(MOUSE_SETPOS, ఖాళీ, 0);
	    లేకపోతే ఒకవేళ (!కీలకం_Typed || (vim_strchr((char_u *)"\r\n ", c) == ఖాళీ
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_ముసుగు);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	లేకపోతే
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(సందేశం_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    ఒకవేళ (c == ':' || c == '?' || c == '/')
    {
	ఒకవేళ (!exmode_చురుకైన)
	    cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
	skip_redraw = TRUE;	    // skip redraw once
	do_redraw = FALSE;
#నిర్వచితమైతే FEAT_TERMINAL
	skip_పదం_loop = TRUE;
#అయితే_ముగింపు
    }

    /*
     * If the window size changed set_shellsize() will redraw the screen.
     * Otherwise the screen is only redrawn if 'redraw' is set and no ':'
     * typed.
     */
    tmpState = State;
    State = oldState;		    // restore State before set_shellsize
    setmouse();
    సందేశం_check();

#నిర్వచితం_కాకపోతే MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    ఒకవేళ (swapping_screen() && !termcap_చురుకైన)
	newline_on_exit = TRUE;
#అయితే_ముగింపు

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    పంక్తులు_ఎడమ = -1;		// reset lines_left at next msg_start()
    reset_చివరి_sourcing();
    ఒకవేళ (keep_సందేశం != ఖాళీ && vim_strsize(keep_సందేశం) >=
			    (అడ్డువరుసలు - cmdline_అడ్డువరుస - 1) * cmdline_వెడల్పు + sc_నిలువు)
	VIM_CLEAR(keep_సందేశం);	    // don't redisplay message, it's too long

    ఒకవేళ (tmpState == బహుళకం_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    లేకపోతే ఒకవేళ (!skip_redraw
	    && (redraw == TRUE || (సందేశం_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    స్థిర శూన్య
తగిలింది_return_సందేశం(శూన్య)
{
    పూర్ణాంకం		save_p_మరిన్ని = p_మరిన్ని;

    p_మరిన్ని = FALSE;	// don't want to see this message when scrolling back
    ఒకవేళ (సందేశం_didout)	// start on a new line
	సందేశం_putchar('\n');
    ఒకవేళ (got_int)
	సందేశం_puts(_("Interrupt: "));

    సందేశం_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    ఒకవేళ (!సందేశం_use_printf())
	సందేశం_clr_eos();
    p_మరిన్ని = save_p_మరిన్ని;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    శూన్య
సమితి_keep_సందేశం(char_u *s, పూర్ణాంకం attr)
{
    vim_free(keep_సందేశం);
    ఒకవేళ (s != ఖాళీ && సందేశం_silent == 0)
	keep_సందేశం = vim_strsave(s);
    లేకపోతే
	keep_సందేశం = ఖాళీ;
    keep_సందేశం_మరిన్ని = FALSE;
    keep_సందేశం_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    శూన్య
సమితి_keep_సందేశం_from_hist(శూన్య)
{
    ఒకవేళ (keep_సందేశం == ఖాళీ && చివరి_సందేశం_hist != ఖాళీ && సందేశం_scrolled == 0
						      && (State & బహుళకం_సాధారణ))
	సమితి_keep_సందేశం(చివరి_సందేశం_hist->సందేశం, చివరి_సందేశం_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    శూన్య
సందేశం_ప్రారంభం(శూన్య)
{
    పూర్ణాంకం		did_return = FALSE;

    ఒకవేళ (సందేశం_అడ్డువరుస < cmdline_అడ్డువరుస)
	సందేశం_అడ్డువరుస = cmdline_అడ్డువరుస;

    ఒకవేళ (!సందేశం_silent)
    {
	VIM_CLEAR(keep_సందేశం);
	need_fileinfo = FALSE;
    }

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	సందేశం_clr_eos();
    }
#అయితే_ముగింపు

#నిర్వచితమైతే HAS_సందేశం_కిటికీ
    ఒకవేళ (లోపల_echowindow)
    {
	ఒకవేళ (popup_సందేశం_win_visible()
		    && ((సందేశం_నిలువు > 0 && (సందేశం_scroll || !నిండిన_screen))
			|| లోపల_echowindow))
	{
	    win_T *wp = popup_get_సందేశం_win();

	    // start a new line
	    curbuf = wp->w_బఫరు;
	    ml_append(wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_బఫరు;
	}
	సందేశం_నిలువు = 0;
    }
    లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (!సందేశం_scroll && నిండిన_screen)	// overwrite last message
    {
	సందేశం_అడ్డువరుస = cmdline_అడ్డువరుస;
	సందేశం_నిలువు =
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_వెడల్పు - 1 :
#అయితే_ముగింపు
	    0;
    }
    లేకపోతే ఒకవేళ (సందేశం_didout || లోపల_echowindow)
    {
	// start message on next line
	సందేశం_putchar('\n');
	did_return = TRUE;
	ఒకవేళ (exmode_చురుకైన != EXMODE_సాధారణ)
	    cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
    }
    ఒకవేళ (!సందేశం_didany || పంక్తులు_ఎడమ < 0)
	సందేశం_starthere();
    ఒకవేళ (సందేశం_silent == 0)
    {
	సందేశం_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    ఒకవేళ (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    శూన్య
సందేశం_starthere(శూన్య)
{
    పంక్తులు_ఎడమ = cmdline_అడ్డువరుస;
    సందేశం_didany = FALSE;
}

    శూన్య
సందేశం_putchar(పూర్ణాంకం c)
{
    సందేశం_putchar_attr(c, 0);
}

    శూన్య
సందేశం_putchar_attr(పూర్ణాంకం c, పూర్ణాంకం attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    ఒకవేళ (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_రెండవ(c);
	buf[2] = K_మూడవ(c);
	buf[3] = NUL;
    }
    లేకపోతే
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    సందేశం_puts_attr((అక్షరం *)buf, attr);
}

    శూన్య
సందేశం_outnum(దీర్ఘ n)
{
    అక్షరం	buf[20];

    sprintf(buf, "%ld", n);
    సందేశం_puts(buf);
}

    శూన్య
సందేశం_home_replace(char_u *fname)
{
    సందేశం_home_replace_attr(fname, 0);
}

#ఒకవేళ defined(FEAT_కనుగొను_ID)
    శూన్య
సందేశం_home_replace_hl(char_u *fname)
{
    సందేశం_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#అయితే_ముగింపు

    స్థిర శూన్య
సందేశం_home_replace_attr(char_u *fname, పూర్ణాంకం attr)
{
    char_u	*name;

    name = home_replace_save(ఖాళీ, fname);
    ఒకవేళ (name != ఖాళీ)
	సందేశం_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    పూర్ణాంకం
సందేశం_outtrans(char_u *str)
{
    తిరిగి సందేశం_outtrans_attr(str, 0);
}

    పూర్ణాంకం
సందేశం_outtrans_attr(char_u *str, పూర్ణాంకం attr)
{
    తిరిగి సందేశం_outtrans_len_attr(str, (పూర్ణాంకం)STRLEN(str), attr);
}

    పూర్ణాంకం
సందేశం_outtrans_len(char_u *str, పూర్ణాంకం len)
{
    తిరిగి సందేశం_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
సందేశం_outtrans_one(char_u *p, పూర్ణాంకం attr)
{
    పూర్ణాంకం		l;

    ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	సందేశం_outtrans_len_attr(p, l, attr);
	తిరిగి p + l;
    }
    సందేశం_puts_attr((అక్షరం *)transchar_byte_buf(ఖాళీ, *p), attr);
    తిరిగి p + 1;
}

    పూర్ణాంకం
సందేశం_outtrans_len_attr(char_u *msgstr, పూర్ణాంకం len, పూర్ణాంకం attr)
{
    పూర్ణాంకం		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_ప్రారంభం = msgstr;
    char_u	*s;
    పూర్ణాంకం		mb_l;
    పూర్ణాంకం		c;
    పూర్ణాంకం		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    ఒకవేళ (attr == 0)
	attr = HL_ATTR(HLF_సందేశం);

    // if MSG_HIST flag set, add message to history
    ఒకవేళ (attr & సందేశం_HIST)
    {
	కూడు_సందేశం_hist(str, len, attr);
	attr &= ~సందేశం_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    ఒకవేళ (సందేశం_silent == 0 && len > 0 && సందేశం_అడ్డువరుస >= cmdline_అడ్డువరుస && సందేశం_నిలువు == 0)
    {
	clear_cmdline = FALSE;
	బహుళకం_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    ఒకవేళ (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	సందేశం_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    అయితే (--len >= 0 && !got_int)
    {
	ఒకవేళ (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	లేకపోతే ఒకవేళ (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	లేకపోతే
	    mb_l = 1;
	ఒకవేళ (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    ఒకవేళ (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    లేకపోతే
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		ఒకవేళ (str > plain_ప్రారంభం)
		    సందేశం_puts_attr_len((అక్షరం *)plain_ప్రారంభం,
					       (పూర్ణాంకం)(str - plain_ప్రారంభం), attr);
		plain_ప్రారంభం = str + mb_l;
		సందేశం_puts_attr((అక్షరం *)transchar_buf(ఖాళీ, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	లేకపోతే
	{
	    s = transchar_byte_buf(ఖాళీ, *str);
	    ఒకవేళ (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		ఒకవేళ (str > plain_ప్రారంభం)
		    సందేశం_puts_attr_len((అక్షరం *)plain_ప్రారంభం,
					       (పూర్ణాంకం)(str - plain_ప్రారంభం), attr);
		plain_ప్రారంభం = str + 1;
		సందేశం_puts_attr((అక్షరం *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (పూర్ణాంకం)STRLEN(s);
	    }
	    లేకపోతే
		++retval;
	    ++str;
	}
    }

    ఒకవేళ (str > plain_ప్రారంభం && !got_int)
	// print the printable chars at the end
	సందేశం_puts_attr_len((అక్షరం *)plain_ప్రారంభం, (పూర్ణాంకం)(str - plain_ప్రారంభం), attr);

    got_int |= save_got_int;

    తిరిగి retval;
}

#ఒకవేళ defined(FEAT_QUICKFIX)
    శూన్య
సందేశం_make(char_u *arg)
{
    పూర్ణాంకం	    i;
    స్థిర char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    కొరకు (i = 5; *arg && i >= 0; --i)
	ఒకవేళ (*arg++ != str[i])
	    ఆపు;
    ఒకవేళ (i < 0)
    {
	సందేశం_putchar('\n');
	కొరకు (i = 0; rs[i]; ++i)
	    సందేశం_putchar(rs[i] - 3);
    }
}
#అయితే_ముగింపు

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
    పూర్ణాంకం
సందేశం_outtrans_special(
    char_u	*strstart,
    పూర్ణాంకం		from,	// TRUE for lhs of a mapping
    పూర్ణాంకం		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    పూర్ణాంకం		retval = 0;
    అక్షరం	*పాఠ్యం;
    పూర్ణాంకం		attr;
    పూర్ణాంకం		len;

    attr = HL_ATTR(HLF_8);
    అయితే (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	ఒకవేళ ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    పాఠ్యం = "<Space>";
	    ++str;
	}
	లేకపోతే
	    పాఠ్యం = (అక్షరం *)str2special(&str, from, FALSE);
	ఒకవేళ (పాఠ్యం[0] != NUL && పాఠ్యం[1] == NUL)
	    // single-byte character or illegal byte
	    పాఠ్యం = (అక్షరం *)transchar_byte_buf(ఖాళీ, (char_u)పాఠ్యం[0]);
	len = vim_strsize((char_u *)పాఠ్యం);
	ఒకవేళ (maxlen > 0 && retval + len >= maxlen)
	    ఆపు;
	// Highlight special keys
	సందేశం_puts_attr(పాఠ్యం, len > 1
		&& (*mb_ptr2len)((char_u *)పాఠ్యం) <= 1 ? attr : 0);
	retval += len;
    }
    తిరిగి retval;
}

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    పూర్ణాంకం	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    పూర్ణాంకం	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    అయితే (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    తిరిగి (char_u *)ga.ga_దత్తాంశం;
}
#అయితే_ముగింపు

/*
 * Return the printable string for the key codes at "*sp".
 * On illegal byte return a string with only that byte.
 * Used for translating the lhs or rhs of a mapping to printable chars.
 * Advances "sp" to the next code.
 */
    char_u *
str2special(
    char_u	**sp,
    పూర్ణాంకం		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    పూర్ణాంకం		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    పూర్ణాంకం			c;
    స్థిర char_u	buf[7];
    char_u		*str = *sp;
    పూర్ణాంకం			modifiers = 0;
    పూర్ణాంకం			special = FALSE;

    ఒకవేళ (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	ఒకవేళ (p != ఖాళీ)
	    తిరిగి p;
    }

    c = *str;
    ఒకవేళ ((c == K_SPECIAL
#నిర్వచితమైతే FEAT_GUI
		|| c == CSI
#అయితే_ముగింపు
	) && str[1] != NUL && str[2] != NUL)
    {
	ఒకవేళ (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	ఒకవేళ ((c == K_SPECIAL
#నిర్వచితమైతే FEAT_GUI
		    || c == CSI
#అయితే_ముగింపు
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = గమ్యం_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	ఒకవేళ (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    ఒకవేళ (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	ఒకవేళ (p != ఖాళీ)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	లేకపోతే
	    // illegal byte
	    *sp = str + 1;
    }
    లేకపోతే
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    ఒకవేళ (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	తిరిగి get_special_కీలకం_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    తిరిగి buf;
}

/*
 * Translate a key sequence into special key names.
 */
    శూన్య
str2specialbuf(char_u *sp, char_u *buf, పూర్ణాంకం len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    అయితే (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	ఒకవేళ (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	లేకపోతే
	    ఆపు;
    }
}

/*
 * print line for :print or :list command
 */
    శూన్య
సందేశం_prt_పంక్తి(char_u *s, పూర్ణాంకం list)
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		నిలువు = 0;
    పూర్ణాంకం		n_extra = 0;
    పూర్ణాంకం		c_extra = 0;
    పూర్ణాంకం		c_final = 0;
    char_u	*p_extra = ఖాళీ;	    // init to make SASC shut up
    పూర్ణాంకం		n;
    పూర్ణాంకం		attr = 0;
    char_u	*trail = ఖాళీ;
    char_u	*lead = ఖాళీ;
    పూర్ణాంకం		లోపల_multispace = FALSE;
    పూర్ణాంకం		multispace_pos = 0;
    పూర్ణాంకం		l;
    char_u	buf[MB_MAXBYTES + 1];

    ఒకవేళ (curwin->w_p_list)
	list = TRUE;

    ఒకవేళ (list)
    {
	// find start of trailing whitespace
	ఒకవేళ (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    అయితే (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	ఒకవేళ (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != ఖాళీ
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    అయితే (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    ఒకవేళ (*lead == NUL)
		lead = ఖాళీ;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    ఒకవేళ (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	సందేశం_putchar(' ');

    అయితే (!got_int)
    {
	ఒకవేళ (n_extra > 0)
	{
	    --n_extra;
	    ఒకవేళ (n_extra == 0 && c_final)
		c = c_final;
	    లేకపోతే ఒకవేళ (c_extra)
		c = c_extra;
	    లేకపోతే
		c = *p_extra++;
	}
	లేకపోతే ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    నిలువు += (*mb_ptr2cells)(s);
	    ఒకవేళ (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    లేకపోతే ఒకవేళ (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		పూర్ణాంకం len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    లేకపోతే
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    సందేశం_puts((అక్షరం *)buf);
	    s += l;
	    కొనసాగు;
	}
	లేకపోతే
	{
	    attr = 0;
	    c = *s++;
	    ఒకవేళ (list)
	    {
		లోపల_multispace = c == ' ' && (*s == ' '
						 || (నిలువు > 0 && s[-2] == ' '));
		ఒకవేళ (!లోపల_multispace)
		    multispace_pos = 0;
	    }
	    ఒకవేళ (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#నిర్వచితమైతే FEAT_VARTABS
		n_extra = tabstop_padding(నిలువు, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#లేకపోతే
		n_extra = curbuf->b_p_ts - నిలువు % curbuf->b_p_ts - 1;
#అయితే_ముగింపు
		ఒకవేళ (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		లేకపోతే
		{
		    పూర్ణాంకం lcs_tab1 = curwin->w_lcs_chars.tab1;
		    పూర్ణాంకం lcs_tab2 = curwin->w_lcs_chars.tab2;
		    పూర్ణాంకం lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    ఒకవేళ (lead != ఖాళీ && s <= lead
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
	    లేకపోతే ఒకవేళ (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    లేకపోతే ఒకవేళ (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    లేకపోతే ఒకవేళ (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(ఖాళీ, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    లేకపోతే ఒకవేళ (c == ' ')
	    {
		ఒకవేళ (lead != ఖాళీ && s <= lead && లోపల_multispace
			&& curwin->w_lcs_chars.leadmultispace != ఖాళీ)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    ఒకవేళ (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		లేకపోతే ఒకవేళ (lead != ఖాళీ && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		లేకపోతే ఒకవేళ (trail != ఖాళీ && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		లేకపోతే ఒకవేళ (లోపల_multispace
			&& curwin->w_lcs_chars.multispace != ఖాళీ)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    ఒకవేళ (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		లేకపోతే ఒకవేళ (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	ఒకవేళ (c == NUL)
	    ఆపు;

	సందేశం_putchar_attr(c, attr);
	నిలువు++;
    }
    సందేశం_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    స్థిర char_u *
screen_puts_mbyte(char_u *s, పూర్ణాంకం l, పూర్ణాంకం attr)
{
    పూర్ణాంకం		cw;

    సందేశం_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    ఒకవేళ (cw > 1 && (
#నిర్వచితమైతే FEAT_RIGHTLEFT
		cmdmsg_rl ? సందేశం_నిలువు <= 1 :
#అయితే_ముగింపు
		సందేశం_నిలువు == cmdline_వెడల్పు - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	సందేశం_screen_putchar('>', HL_ATTR(HLF_AT));
	తిరిగి s;
    }

    screen_puts_len(s, l, సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు, attr);
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (cmdmsg_rl)
    {
	సందేశం_నిలువు -= cw;
	ఒకవేళ (సందేశం_నిలువు == 0)
	{
	    సందేశం_నిలువు = cmdline_వెడల్పు;
	    ++సందేశం_అడ్డువరుస;
	}
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	సందేశం_నిలువు += cw;
	ఒకవేళ (సందేశం_నిలువు >= cmdline_వెడల్పు)
	{
	    సందేశం_నిలువు = 0;
	    ++సందేశం_అడ్డువరుస;
	}
    }
    తిరిగి s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    శూన్య
సందేశం_puts(అక్షరం *s)
{
    సందేశం_puts_attr(s, 0);
}

    శూన్య
సందేశం_puts_title(అక్షరం *s)
{
    సందేశం_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    స్థిర శూన్య
సందేశం_outtrans_long_len_attr(char_u *longstr, పూర్ణాంకం len, పూర్ణాంకం attr)
{
    పూర్ణాంకం		slen = len;
    పూర్ణాంకం		స్థలం;

    స్థలం = cmdline_వెడల్పు - సందేశం_నిలువు;
    ఒకవేళ (len > స్థలం && స్థలం >= 20)
    {
	slen = (స్థలం - 3) / 2;
	సందేశం_outtrans_len_attr(longstr, slen, attr);
	సందేశం_puts_attr("...", HL_ATTR(HLF_8));
    }
    సందేశం_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    శూన్య
సందేశం_outtrans_long_attr(char_u *longstr, పూర్ణాంకం attr)
{
    సందేశం_outtrans_long_len_attr(longstr, (పూర్ణాంకం)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    శూన్య
సందేశం_puts_attr(అక్షరం *s, పూర్ణాంకం attr)
{
    సందేశం_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    స్థిర శూన్య
సందేశం_puts_attr_len(అక్షరం *str, పూర్ణాంకం maxlen, పూర్ణాంకం attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    ఒకవేళ (సందేశం_silent != 0)
	తిరిగి;

    ఒకవేళ (attr == 0)
	attr = HL_ATTR(HLF_సందేశం);

    // if MSG_HIST flag set, add message to history
    ఒకవేళ ((attr & సందేశం_HIST) && maxlen < 0)
    {
	కూడు_సందేశం_hist((char_u *)str, -1, attr);
	attr &= ~సందేశం_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    ఒకవేళ (సందేశం_scrolled != 0 && !సందేశం_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    సందేశం_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    ఒకవేళ (సందేశం_use_printf())
	సందేశం_puts_printf((char_u *)str, maxlen);
    లేకపోతే
	సందేశం_puts_display((char_u *)str, maxlen, attr, FALSE);

    need_fileinfo = FALSE;
}

// values for "where"
#నిర్వచించు PUT_APPEND 0		// append to "lnum"
#నిర్వచించు PUT_TRUNC 1		// replace "lnum"
#నిర్వచించు PUT_క్రింద 2		// add below "lnum"
				//
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
/*
 * Put text "t_s" until "end" in the message window.
 * "where" specifies where to put the text.
 */
    స్థిర శూన్య
put_సందేశం_win(win_T *wp, పూర్ణాంకం where, char_u *t_s, char_u *చివర, linenr_T lnum)
{
    char_u  *p;

    ఒకవేళ (where == PUT_క్రింద)
    {
	ఒకవేళ (*చివర != NUL)
	{
	    p = vim_strnsave(t_s, చివర - t_s);
	    ఒకవేళ (p == ఖాళీ)
		తిరిగి;
	}
	లేకపోతే
	    p = t_s;
	ml_append_buf(wp->w_బఫరు, lnum, p, (colnr_T)0, FALSE);
	ఒకవేళ (p != t_s)
	    vim_free(p);
    }
    లేకపోతే
    {
	char_u *newp;

	curbuf = wp->w_బఫరు;
	ఒకవేళ (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    ఒకవేళ (newp == ఖాళీ)
		తిరిగి;
	    ఒకవేళ (*చివర != NUL)
		newp[STRLEN(ml_get(lnum)) + (చివర - t_s)] = NUL;
	}
	లేకపోతే
	{
	    newp = vim_strnsave(t_s, చివర - t_s);
	    ఒకవేళ (newp == ఖాళీ)
		తిరిగి;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_బఫరు;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    సందేశం_నిలువు += (పూర్ణాంకం)(చివర - t_s);
}
#అయితే_ముగింపు

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    స్థిర శూన్య
సందేశం_puts_display(
    char_u	*str,
    పూర్ణాంకం		maxlen,
    పూర్ణాంకం		attr,
    పూర్ణాంకం		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    పూర్ణాంకం		t_నిలువు = 0;	// screen cells todo, 0 when "t_s" not used
    పూర్ణాంకం		l;
    పూర్ణాంకం		cw;
    char_u	*sb_str = str;
    పూర్ణాంకం		sb_నిలువు = సందేశం_నిలువు;
    పూర్ణాంకం		wrap;
    పూర్ణాంకం		did_చివరి_char;
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
    పూర్ణాంకం		where = PUT_APPEND;
    win_T	*సందేశం_win = ఖాళీ;
    linenr_T    lnum = 1;

    ఒకవేళ (లోపల_echowindow)
    {
	సందేశం_win = popup_get_సందేశం_win();

	ఒకవేళ (సందేశం_win != ఖాళీ)
	{
	    ఒకవేళ (!popup_సందేశం_win_visible())
	    {
		ఒకవేళ (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    సందేశం_win = ఖాళీ;
		}
		లేకపోతే
		{
		    // currently hidden, make it empty
		    curbuf = సందేశం_win->w_బఫరు;
		    అయితే ((curbuf->b_ml.ml_flags & ML_ఖాళీలు) == 0)
			ml_delete(1);
		    curbuf = curwin->w_బఫరు;
		}
	    }
	    లేకపోతే
	    {
		lnum = సందేశం_win->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
		ఒకవేళ (సందేశం_నిలువు == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#అయితే_ముగింపు

    did_wait_return = FALSE;
    అయితే ((maxlen < 0 || (పూర్ణాంకం)(s - str) < maxlen) && *s != NUL)
    {
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	// For echowindow, use full width; for regular messages, leave last column
	పూర్ణాంకం wrap_నిలువు = సందేశం_win != ఖాళీ ? cmdline_వెడల్పు : cmdline_వెడల్పు - 1;
#లేకపోతే
	పూర్ణాంకం wrap_నిలువు = cmdline_వెడల్పు - 1;
#అయితే_ముగింపు

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	ఒకవేళ (!recurse && సందేశం_అడ్డువరుస >= అడ్డువరుసలు - 1 && (*s == '\n' || (
#నిర్వచితమైతే FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			సందేశం_నిలువు <= 1
		      || (*s == TAB && సందేశం_నిలువు <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && సందేశం_నిలువు <= 2))
		    :
#అయితే_ముగింపు
		      ((*s != '\r' && సందేశం_నిలువు + t_నిలువు >= wrap_నిలువు)
		       || (*s == TAB && సందేశం_నిలువు + t_నిలువు
			   >= (wrap_నిలువు & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && సందేశం_నిలువు + t_నిలువు >= wrap_నిలువు - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    ఒకవేళ (t_నిలువు > 0)
	    {
		// output postponed text
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
		ఒకవేళ (సందేశం_win != ఖాళీ)
		{
		    put_సందేశం_win(సందేశం_win, where, t_s, s, lnum);
		    ఒకవేళ (where == PUT_క్రింద)
			++lnum;
		    t_నిలువు = 0;
		    where = PUT_క్రింద;
		}
		లేకపోతే
#అయితే_ముగింపు
		    t_puts(&t_నిలువు, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    ఒకవేళ (సందేశం_no_మరిన్ని && పంక్తులు_ఎడమ == 0)
		ఆపు;

#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    ఒకవేళ (సందేశం_win == ఖాళీ)
#అయితే_ముగింపు
		// Scroll the screen up one line.
		సందేశం_scroll_పైకి();

#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    ఒకవేళ (సందేశం_win == ఖాళీ)
	    {
#అయితే_ముగింపు
		సందేశం_అడ్డువరుస = అడ్డువరుసలు - 2;
		ఒకవేళ (సందేశం_నిలువు >= cmdline_వెడల్పు)  // can happen after screen resize
		    సందేశం_నిలువు = cmdline_వెడల్పు - 1;
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    }
#అయితే_ముగింపు

	    // Display char in last column before showing more-prompt.
	    ఒకవేళ (*s >= ' '
#నిర్వచితమైతే FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#అయితే_ముగింపు
	       )
	    {
		ఒకవేళ (has_mbyte)
		{
		    ఒకవేళ (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (పూర్ణాంకం)((str + maxlen) - s));
		    లేకపోతే
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		లేకపోతే
		    సందేశం_screen_putchar(*s++, attr);
		did_చివరి_char = TRUE;
	    }
	    లేకపోతే
		did_చివరి_char = FALSE;

	    ఒకవేళ (p_మరిన్ని)
		// store text for scrolling back
		store_sb_పాఠ్యం(&sb_str, s, attr, &sb_నిలువు, TRUE);

#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    ఒకవేళ (సందేశం_win == ఖాళీ)
	    {
#అయితే_ముగింపు
		inc_సందేశం_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		ఒకవేళ (cmdline_అడ్డువరుస > 0 && !exmode_చురుకైన)
		    --cmdline_అడ్డువరుస;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		ఒకవేళ (పంక్తులు_ఎడమ > 0)
		    --పంక్తులు_ఎడమ;
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    }
#అయితే_ముగింపు
	    ఒకవేళ (p_మరిన్ని && పంక్తులు_ఎడమ == 0 && State != బహుళకం_HITRETURN
					    && !సందేశం_no_మరిన్ని && !exmode_చురుకైన)
	    {
#నిర్వచితమైతే FEAT_CON_DIALOG
		ఒకవేళ (do_మరిన్ని_prompt(NUL))
		    s = confirm_సందేశం_తోక;
#లేకపోతే
		(శూన్య)do_మరిన్ని_prompt(NUL);
#అయితే_ముగింపు
		ఒకవేళ (quit_మరిన్ని)
		    తిరిగి;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    ఒకవేళ (did_చివరి_char)
		కొనసాగు;
	}

	wrap = *s == '\n'
		    || సందేశం_నిలువు + t_నిలువు >= wrap_నిలువు
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && సందేశం_నిలువు + t_నిలువు >= wrap_నిలువు - 1);
	ఒకవేళ (t_నిలువు > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    ఒకవేళ (సందేశం_win != ఖాళీ)
	    {
		put_సందేశం_win(సందేశం_win, where, t_s, s, lnum);
		ఒకవేళ (where == PUT_క్రింద)
		    ++lnum;
		t_నిలువు = 0;
		where = PUT_క్రింద;
		// Reset msg_col after outputting to new line in echowindow
		ఒకవేళ (wrap)
		    సందేశం_నిలువు = 0;
	    }
	    లేకపోతే
#అయితే_ముగింపు
		t_puts(&t_నిలువు, t_s, s, attr);
	}

	ఒకవేళ (wrap && p_మరిన్ని && !recurse)
	    // store text for scrolling back
	    store_sb_పాఠ్యం(&sb_str, s, attr, &sb_నిలువు, TRUE);

	ఒకవేళ (*s == '\n')		    // go to next line
	{
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    ఒకవేళ (సందేశం_win != ఖాళీ)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		ఒకవేళ ((సందేశం_win->w_బఫరు->b_ml.ml_flags & ML_ఖాళీలు) == 0)
		{
		    put_సందేశం_win(సందేశం_win, PUT_క్రింద, t_s, t_s, lnum);
		    ++lnum;
		}
		సందేశం_నిలువు = 0;  // Reset column for new line
	    }
	    లేకపోతే
#అయితే_ముగింపు
		సందేశం_didout = FALSE;	    // remember that line is empty
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    ఒకవేళ (cmdmsg_rl)
		సందేశం_నిలువు = cmdline_వెడల్పు - 1;
	    లేకపోతే
#అయితే_ముగింపు
		సందేశం_నిలువు = 0;
	    ఒకవేళ (++సందేశం_అడ్డువరుస >= అడ్డువరుసలు)  // safety check
		సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
	}
	లేకపోతే ఒకవేళ (*s == '\r')	    // go to column 0
	{
	    సందేశం_నిలువు = 0;
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    where = PUT_TRUNC;
#అయితే_ముగింపు
	}
	లేకపోతే ఒకవేళ (*s == '\b')	    // go to previous char
	{
	    ఒకవేళ (సందేశం_నిలువు)
		--సందేశం_నిలువు;
	}
	లేకపోతే ఒకవేళ (*s == TAB)	    // translate Tab into spaces
	{
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    ఒకవేళ (సందేశం_win != ఖాళీ)
		సందేశం_నిలువు = (సందేశం_నిలువు + 7) % 8;
	    లేకపోతే
#అయితే_ముగింపు
		చేయి
		    సందేశం_screen_putchar(' ', attr);
		అయితే (సందేశం_నిలువు & 7);
	}
	లేకపోతే ఒకవేళ (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	లేకపోతే
	{
	    ఒకవేళ (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		ఒకవేళ (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (పూర్ణాంకం)((str + maxlen) - s));
		లేకపోతే
		    l = (*mb_ptr2len)(s);
	    }
	    లేకపోతే
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    ఒకవేళ (
#నిర్వచితమైతే FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#అయితే_ముగింపు
		    (cw > 1 && సందేశం_నిలువు + t_నిలువు >= wrap_నిలువు))
	    {
		ఒకవేళ (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		లేకపోతే
		    సందేశం_screen_putchar(*s, attr);
	    }
	    లేకపోతే
	    {
		// postpone this character until later
		ఒకవేళ (t_నిలువు == 0)
		    t_s = s;
		t_నిలువు += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    ఒకవేళ (t_నిలువు > 0)
    {
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	ఒకవేళ (సందేశం_win != ఖాళీ)
	    put_సందేశం_win(సందేశం_win, where, t_s, s, lnum);
	లేకపోతే
#అయితే_ముగింపు
	    t_puts(&t_నిలువు, t_s, s, attr);
    }

#నిర్వచితమైతే HAS_సందేశం_కిటికీ
    ఒకవేళ (సందేశం_win != ఖాళీ)
	popup_show_సందేశం_win();
#అయితే_ముగింపు
    // Store the text for scroll back.
    ఒకవేళ (p_మరిన్ని && !recurse)
	store_sb_పాఠ్యం(&sb_str, s, attr, &sb_నిలువు, FALSE);

    సందేశం_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    పూర్ణాంకం
సందేశం_filtered(char_u *సందేశం)
{
    పూర్ణాంకం సరిపోలిక;

    ఒకవేళ (cmdmod.cmod_filter_regmatch.regprog == ఖాళీ)
	తిరిగి FALSE;
    సరిపోలిక = vim_regexec(&cmdmod.cmod_filter_regmatch, సందేశం, (colnr_T)0);
    తిరిగి cmdmod.cmod_filter_force ? సరిపోలిక : !సరిపోలిక;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    స్థిర శూన్య
సందేశం_scroll_పైకి(శూన్య)
{
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
    ఒకవేళ (లోపల_echowindow)
	తిరిగి;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    ఒకవేళ (gui.లోపల_use)
	gui_undraw_cursor();
#అయితే_ముగింపు
    // scrolling up always works
    mch_disable_flush();
    screen_del_పంక్తులు(0, 0, 1, (పూర్ణాంకం)అడ్డువరుసలు, TRUE, 0, ఖాళీ);
    mch_enable_flush();

    ఒకవేళ (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((పూర్ణాంకం)అడ్డువరుసలు - 1, (పూర్ణాంకం)అడ్డువరుసలు, cmdline_నిలువు_off,
		cmdline_నిలువు_off + cmdline_వెడల్పు, ' ', ' ', HL_ATTR(HLF_సందేశం));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	ఒకవేళ (ScreenAttrs[పంక్తి_స్థానభ్రంశం[అడ్డువరుసలు - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((పూర్ణాంకం)అడ్డువరుసలు - 2, (పూర్ణాంకం)అడ్డువరుసలు - 1,
				 (పూర్ణాంకం)Columns - 1, (పూర్ణాంకం)Columns,
				 ' ', ' ', HL_ATTR(HLF_సందేశం));
    }
}

/*
 * Increment "msg_scrolled".
 */
    స్థిర శూన్య
inc_సందేశం_scrolled(శూన్య)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = ఖాళీ;

	// v:scrollstart is empty, set it to the script/function name and line
	//సంఖ్య
	ఒకవేళ (p.string == ఖాళీ)
	{
	    p.string = (char_u *)_("Unknown");
	    p.నిడివి = STRLEN(p.string);
	}
	లేకపోతే
	{
	    size_t  tofreesize;

	    tofreesize = (పూర్ణాంకం)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    ఒకవేళ (tofree != ఖాళీ)
	    {
		p.నిడివి = vim_snprintf_safelen((అక్షరం *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (దీర్ఘ)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	సమితి_vim_var_string(VV_SCROLLSTART, p.string, (పూర్ణాంకం)p.నిడివి);
	vim_free(tofree);
    }
#అయితే_ముగింపు
    ++సందేశం_scrolled;
    సమితి_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
రకనిర్వచనం నిర్మాణం msgchunk_S msgchunk_T;
నిర్మాణం msgchunk_S
{
    msgchunk_T	*sb_తదుపరి;
    msgchunk_T	*sb_prev;
    అక్షరం	sb_eol;		// TRUE when line ends after this text
    పూర్ణాంకం		sb_సందేశం_నిలువు;	// column in which text starts
    పూర్ణాంకం		sb_attr;	// text attributes
    char_u	sb_పాఠ్యం[1];	// text to be displayed, actually longer
};

స్థిర msgchunk_T *చివరి_msgchunk = ఖాళీ; // last displayed text

స్థిర msgchunk_T *సందేశం_sb_ప్రారంభం(msgchunk_T *mps);

రకనిర్వచనం గణన {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_పూర్తి
} sb_clear_T;

// When to clear text on next msg.
స్థిర sb_clear_T do_clear_sb_పాఠ్యం = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    స్థిర శూన్య
store_sb_పాఠ్యం(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    పూర్ణాంకం		attr,
    పూర్ణాంకం		*sb_నిలువు,
    పూర్ణాంకం		ముగింపు)		// line ends
{
    msgchunk_T	*mp;

    ఒకవేళ (do_clear_sb_పాఠ్యం == SB_CLEAR_ALL
	    || do_clear_sb_పాఠ్యం == SB_CLEAR_CMDLINE_పూర్తి)
    {
	clear_sb_పాఠ్యం(do_clear_sb_పాఠ్యం == SB_CLEAR_ALL);
	సందేశం_sb_eol();  // prevent messages from overlapping
	do_clear_sb_పాఠ్యం = SB_CLEAR_NONE;
    }

    ఒకవేళ (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_పాఠ్యం) + (s - *sb_str) + 1);
	ఒకవేళ (mp != ఖాళీ)
	{
	    mp->sb_eol = ముగింపు;
	    mp->sb_సందేశం_నిలువు = *sb_నిలువు;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_పాఠ్యం, *sb_str, s - *sb_str);

	    ఒకవేళ (చివరి_msgchunk == ఖాళీ)
	    {
		చివరి_msgchunk = mp;
		mp->sb_prev = ఖాళీ;
	    }
	    లేకపోతే
	    {
		mp->sb_prev = చివరి_msgchunk;
		చివరి_msgchunk->sb_తదుపరి = mp;
		చివరి_msgchunk = mp;
	    }
	    mp->sb_తదుపరి = ఖాళీ;
	}
    }
    లేకపోతే ఒకవేళ (ముగింపు && చివరి_msgchunk != ఖాళీ)
	చివరి_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_నిలువు = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    శూన్య
may_clear_sb_పాఠ్యం(శూన్య)
{
    do_clear_sb_పాఠ్యం = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    శూన్య
sb_పాఠ్యం_ప్రారంభం_cmdline(శూన్య)
{
    ఒకవేళ (do_clear_sb_పాఠ్యం == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_పాఠ్యం_restart_cmdline();
    లేకపోతే
    {
	సందేశం_sb_eol();
	do_clear_sb_పాఠ్యం = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    శూన్య
sb_పాఠ్యం_restart_cmdline(శూన్య)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_పాఠ్యం = SB_CLEAR_CMDLINE_BUSY;

    ఒకవేళ (చివరి_msgchunk == ఖాళీ || చివరి_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	తిరిగి;

    tofree = సందేశం_sb_ప్రారంభం(చివరి_msgchunk);
    చివరి_msgchunk = tofree->sb_prev;
    ఒకవేళ (చివరి_msgchunk != ఖాళీ)
	చివరి_msgchunk->sb_తదుపరి = ఖాళీ;
    అయితే (tofree != ఖాళీ)
    {
	msgchunk_T *tofree_తదుపరి = tofree->sb_తదుపరి;

	vim_free(tofree);
	tofree = tofree_తదుపరి;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    శూన్య
sb_పాఠ్యం_చివర_cmdline(శూన్య)
{
    do_clear_sb_పాఠ్యం = SB_CLEAR_CMDLINE_పూర్తి;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    శూన్య
clear_sb_పాఠ్యం(పూర్ణాంకం all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    ఒకవేళ (all)
	lastp = &చివరి_msgchunk;
    లేకపోతే
    {
	ఒకవేళ (చివరి_msgchunk == ఖాళీ)
	    తిరిగి;
	lastp = &సందేశం_sb_ప్రారంభం(చివరి_msgchunk)->sb_prev;
    }

    అయితే (*lastp != ఖాళీ)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    శూన్య
show_sb_పాఠ్యం(శూన్య)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = సందేశం_sb_ప్రారంభం(చివరి_msgchunk);
    ఒకవేళ (mp == ఖాళీ || mp->sb_prev == ఖాళీ)
	vim_beep(BO_MESS);
    లేకపోతే
    {
	do_మరిన్ని_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    స్థిర msgchunk_T *
సందేశం_sb_ప్రారంభం(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    అయితే (mp != ఖాళీ && mp->sb_prev != ఖాళీ && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    తిరిగి mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    శూన్య
సందేశం_sb_eol(శూన్య)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    ఒకవేళ (చివరి_msgchunk != ఖాళీ
	    && !(చివరి_msgchunk->sb_పాఠ్యం[0] == '\n'
		&& చివరి_msgchunk->sb_పాఠ్యం[1] == NUL))
	చివరి_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    స్థిర msgchunk_T *
disp_sb_పంక్తి(పూర్ణాంకం అడ్డువరుస, msgchunk_T *smp, పూర్ణాంకం clear_గమ్యం_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    కొరకు (;;)
    {
	సందేశం_అడ్డువరుస = అడ్డువరుస;
	సందేశం_నిలువు = mp->sb_సందేశం_నిలువు;
	p = mp->sb_పాఠ్యం;
	ఒకవేళ (*p == '\n')	    // don't display the line break
	    ++p;
	సందేశం_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	ఒకవేళ (clear_గమ్యం_eol)
	    screen_fill(అడ్డువరుస, అడ్డువరుస + 1,
		    cmdline_నిలువు_off + సందేశం_నిలువు, cmdline_నిలువు_off + cmdline_వెడల్పు,
		    ' ', ' ', HL_ATTR(HLF_సందేశం));

	ఒకవేళ (mp->sb_eol || mp->sb_తదుపరి == ఖాళీ)
	    ఆపు;
	mp = mp->sb_తదుపరి;
    }
    తిరిగి mp->sb_తదుపరి;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    స్థిర శూన్య
t_puts(
    పూర్ణాంకం		*t_నిలువు,
    char_u	*t_s,
    char_u	*s,
    పూర్ణాంకం		attr)
{
    // output postponed text
    సందేశం_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (పూర్ణాంకం)(s - t_s), సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు,
	    attr);
    సందేశం_నిలువు += *t_నిలువు;
    *t_నిలువు = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    ఒకవేళ (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--సందేశం_నిలువు;
    ఒకవేళ (సందేశం_నిలువు >= cmdline_వెడల్పు)
    {
	సందేశం_నిలువు = 0;
	++సందేశం_అడ్డువరుస;
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
    పూర్ణాంకం
సందేశం_use_printf(శూన్య)
{
    తిరిగి (!సందేశం_check_screen()
#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.లోపల_use && !termcap_చురుకైన)
# లేకపోతే
	    || !termcap_చురుకైన
# endif
#అయితే_ముగింపు
	    || (swapping_screen() && !termcap_చురుకైన)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    స్థిర శూన్య
సందేశం_puts_printf(char_u *str, పూర్ణాంకం maxlen)
{
    char_u	*s = str;
    char_u	*buf = ఖాళీ;
    char_u	*p = s;

#నిర్వచితమైతే MSWIN
    ఒకవేళ (!(silent_బహుళకం && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#అయితే_ముగింపు
    అయితే ((maxlen < 0 || (పూర్ణాంకం)(s - str) < maxlen) && *s != NUL)
    {
	ఒకవేళ (!(silent_బహుళకం && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    ఒకవేళ (*s == NL)
	    {
		పూర్ణాంకం n = (పూర్ణాంకం)(s - p);

		buf = alloc(n + 3);
		ఒకవేళ (buf != ఖాళీ)
		{
		    జ్ఞాపకం_నకలు(buf, p, n);
		    ఒకవేళ (!info_సందేశం)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    ఒకవేళ (info_సందేశం)   // informative message, not an error
			mch_సందేశం((అక్షరం *)buf);
		    లేకపోతే
			mch_errmsg((అక్షరం *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (cmdmsg_rl)
	{
	    ఒకవేళ (*s == CAR || *s == NL)
		సందేశం_నిలువు = cmdline_వెడల్పు - 1;
	    లేకపోతే
		--సందేశం_నిలువు;
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    ఒకవేళ (*s == CAR || *s == NL)
		సందేశం_నిలువు = 0;
	    లేకపోతే
		++సందేశం_నిలువు;
	}
	++s;
    }

    ఒకవేళ (*p != NUL && !(silent_బహుళకం && p_verbose == 0))
    {
	char_u *tofree = ఖాళీ;

	ఒకవేళ (maxlen > 0 && vim_strlen_maxlen((అక్షరం *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	ఒకవేళ (p != ఖాళీ)
	{
	    ఒకవేళ (info_సందేశం)
		mch_సందేశం((అక్షరం *)p);
	    లేకపోతే
		mch_errmsg((అక్షరం *)p);
	    vim_free(tofree);
	}
    }

    సందేశం_didout = TRUE;	    // assume that line is not empty

#నిర్వచితమైతే MSWIN
    ఒకవేళ (!(silent_బహుళకం && p_verbose == 0))
	mch_settmode(TMODE_RAW);
#అయితే_ముగింపు
}

/*
 * Show the more-prompt and handle the user response.
 * This takes care of scrolling back and displaying previously displayed text.
 * When at hit-enter prompt "typed_char" is the already typed character,
 * otherwise it's NUL.
 * Returns TRUE when jumping ahead to "confirm_msg_tail".
 */
    స్థిర పూర్ణాంకం
do_మరిన్ని_prompt(పూర్ణాంకం typed_char)
{
    స్థిర పూర్ణాంకం	entered = FALSE;
    పూర్ణాంకం		వాడిన_typed_char = typed_char;
    పూర్ణాంకం		oldState = State;
    పూర్ణాంకం		c;
#నిర్వచితమైతే FEAT_CON_DIALOG
    పూర్ణాంకం		retval = FALSE;
#అయితే_ముగింపు
    పూర్ణాంకం		toscroll;
    msgchunk_T	*mp_చివరి = ఖాళీ;
    msgchunk_T	*mp;
    పూర్ణాంకం		i;
    పూర్ణాంకం		సందేశం_attr;

    సందేశం_attr = HL_ATTR(HLF_సందేశం);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    ఒకవేళ (entered || (State == బహుళకం_HITRETURN && typed_char == 0))
	తిరిగి FALSE;
    entered = TRUE;

    ఒకవేళ (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_చివరి = సందేశం_sb_ప్రారంభం(చివరి_msgchunk);
	కొరకు (i = 0; i < అడ్డువరుసలు - 2 && mp_చివరి != ఖాళీ
					     && mp_చివరి->sb_prev != ఖాళీ; ++i)
	    mp_చివరి = సందేశం_sb_ప్రారంభం(mp_చివరి->sb_prev);
    }

    State = బహుళకం_ASKMORE;
    setmouse();
    ఒకవేళ (typed_char == NUL)
	సందేశం_moremsg(FALSE);
    కొరకు (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	ఒకవేళ (వాడిన_typed_char != NUL)
	{
	    c = వాడిన_typed_char;	// was typed at hit-enter prompt
	    వాడిన_typed_char = NUL;
	}
	లేకపోతే
	    c = get_keystroke();

#ఒకవేళ defined(FEAT_MENU) && defined(FEAT_GUI)
	ఒకవేళ (c == K_MENU)
	{
	    పూర్ణాంకం idx = get_menu_సూచీ(current_menu, బహుళకం_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    ఒకవేళ (idx == MENU_సూచీ_INVALID)
		కొనసాగు;
	    c = *current_menu->అక్షరశ్రేణులు[idx];
	    ఒకవేళ (c != NUL && current_menu->అక్షరశ్రేణులు[idx][1] != NUL)
		ins_typebuf(current_menu->అక్షరశ్రేణులు[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#అయితే_ముగింపు

	toscroll = 0;
	ఎంపిక (c)
	{
	స్థితి BS:		// scroll one line back
	స్థితి K_BS:
	స్థితి 'k':
	స్థితి K_పైకి:
	    toscroll = -1;
	    ఆపు;

	స్థితి CAR:		// one extra line
	స్థితి NL:
	స్థితి 'j':
	స్థితి K_కిందికి:
	    toscroll = 1;
	    ఆపు;

	స్థితి 'u':		// Up half a page
	    toscroll = -(అడ్డువరుసలు / 2);
	    ఆపు;

	స్థితి 'd':		// Down half a page
	    toscroll = అడ్డువరుసలు / 2;
	    ఆపు;

	స్థితి 'b':		// one page back
	స్థితి Ctrl_B:
	స్థితి K_PAGEUP:
	    toscroll = -(అడ్డువరుసలు - 1);
	    ఆపు;

	స్థితి ' ':		// one extra page
	స్థితి 'f':
	స్థితి Ctrl_F:
	స్థితి K_PAGEDOWN:
	స్థితి K_LEFTMOUSE:
	    toscroll = అడ్డువరుసలు - 1;
	    ఆపు;

	స్థితి 'g':		// all the way back to the start
	    toscroll = -999999;
	    ఆపు;

	స్థితి 'G':		// all the way to the end
	    toscroll = 999999;
	    పంక్తులు_ఎడమ = 999999;
	    ఆపు;

	స్థితి ':':		// start new command line
#నిర్వచితమైతే FEAT_CON_DIALOG
	    ఒకవేళ (!confirm_సందేశం_వాడిన)
#అయితే_ముగింపు
	    {
		// Since got_int is set all typeahead will be flushed, but we
		// want to keep this ':', remember that in a special way.
		typeahead_noflush(':');
#నిర్వచితమైతే FEAT_TERMINAL
		skip_పదం_loop = TRUE;
#అయితే_ముగింపు
		cmdline_అడ్డువరుస = అడ్డువరుసలు - 1;		// put ':' on this line
		skip_redraw = TRUE;		// skip redraw once
		need_wait_return = FALSE;	// don't wait in main()
	    }
	    // FALLTHROUGH
	స్థితి 'q':		// quit
	స్థితి Ctrl_C:
	స్థితి ESC:
#నిర్వచితమైతే FEAT_CON_DIALOG
	    ఒకవేళ (confirm_సందేశం_వాడిన)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		got_int = TRUE;
		quit_మరిన్ని = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    పంక్తులు_ఎడమ = అడ్డువరుసలు - 1;
	    ఆపు;

#నిర్వచితమైతే FEAT_CLIPBOARD
	స్థితి Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    ఒకవేళ (clip_నక్షత్రం.state == SELECT_పూర్తి)
		clip_copy_modeless_selection(TRUE);
	    కొనసాగు;
#అయితే_ముగింపు
	అప్రమేయ:		// no valid response
	    సందేశం_moremsg(TRUE);
	    కొనసాగు;
	}

	ఒకవేళ (toscroll != 0)
	{
	    ఒకవేళ (toscroll < 0)
	    {
		// go to start of last line
		ఒకవేళ (mp_చివరి == ఖాళీ)
		    mp = సందేశం_sb_ప్రారంభం(చివరి_msgchunk);
		లేకపోతే ఒకవేళ (mp_చివరి->sb_prev != ఖాళీ)
		    mp = సందేశం_sb_ప్రారంభం(mp_చివరి->sb_prev);
		లేకపోతే
		    mp = ఖాళీ;

		// go to start of line at top of the screen
		కొరకు (i = 0; i < అడ్డువరుసలు - 2 && mp != ఖాళీ && mp->sb_prev != ఖాళీ;
									  ++i)
		    mp = సందేశం_sb_ప్రారంభం(mp->sb_prev);

		ఒకవేళ (mp != ఖాళీ && mp->sb_prev != ఖాళీ)
		{
		    // Find line to be displayed at top.
		    కొరకు (i = 0; i > toscroll; --i)
		    {
			ఒకవేళ (mp == ఖాళీ || mp->sb_prev == ఖాళీ)
			    ఆపు;
			mp = సందేశం_sb_ప్రారంభం(mp->sb_prev);
			ఒకవేళ (mp_చివరి == ఖాళీ)
			    mp_చివరి = సందేశం_sb_ప్రారంభం(చివరి_msgchunk);
			లేకపోతే
			    mp_చివరి = సందేశం_sb_ప్రారంభం(mp_చివరి->sb_prev);
		    }

		    ఒకవేళ (toscroll == -1 && screen_ins_పంక్తులు(0, 0, 1,
						     (పూర్ణాంకం)అడ్డువరుసలు, 0, ఖాళీ) == OK)
		    {
			// display line at top
			(శూన్య)disp_sb_పంక్తి(0, mp, FALSE);
		    }
		    లేకపోతే
		    {
			పూర్ణాంకం did_clear = screenclear();

			// redisplay all lines
			కొరకు (i = 0; mp != ఖాళీ && i < అడ్డువరుసలు - 1; ++i)
			{
			    mp = disp_sb_పంక్తి(i, mp, !did_clear);
			    ++సందేశం_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    లేకపోతే
	    {
		// First display any text that we scrolled back.
		అయితే (toscroll > 0 && mp_చివరి != ఖాళీ)
		{
		    // scroll up, display line at bottom
		    సందేశం_scroll_పైకి();
		    inc_సందేశం_scrolled();
		    screen_fill((పూర్ణాంకం)అడ్డువరుసలు - 2, (పూర్ణాంకం)అడ్డువరుసలు - 1,
			    cmdline_నిలువు_off, cmdline_నిలువు_off + cmdline_వెడల్పు,
			    ' ', ' ', సందేశం_attr);
		    mp_చివరి = disp_sb_పంక్తి((పూర్ణాంకం)అడ్డువరుసలు - 2, mp_చివరి, FALSE);
		    --toscroll;
		}
	    }

	    ఒకవేళ (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((పూర్ణాంకం)అడ్డువరుసలు - 1, (పూర్ణాంకం)అడ్డువరుసలు, cmdline_నిలువు_off,
			cmdline_నిలువు_off + cmdline_వెడల్పు, ' ', ' ', సందేశం_attr);
		సందేశం_moremsg(FALSE);
		కొనసాగు;
	    }

	    // display more text, return to caller
	    పంక్తులు_ఎడమ = toscroll;
	}

	ఆపు;
    }

    // clear the --more-- message
    screen_fill((పూర్ణాంకం)అడ్డువరుసలు - 1, (పూర్ణాంకం)అడ్డువరుసలు, cmdline_నిలువు_off,
	    cmdline_నిలువు_off + cmdline_వెడల్పు, ' ', ' ', సందేశం_attr);
    State = oldState;
    setmouse();
    ఒకవేళ (quit_మరిన్ని)
    {
	సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
	సందేశం_నిలువు = 0;
    }
#నిర్వచితమైతే FEAT_RIGHTLEFT
    లేకపోతే ఒకవేళ (cmdmsg_rl)
	సందేశం_నిలువు = cmdline_వెడల్పు - 1;
#అయితే_ముగింపు

    entered = FALSE;
#నిర్వచితమైతే FEAT_CON_DIALOG
    తిరిగి retval;
#లేకపోతే
    తిరిగి FALSE;
#అయితే_ముగింపు
}

#ఒకవేళ defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_సందేశం
#  undef mch_సందేశం
# endif

# ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    స్థిర శూన్య
mch_errmsg_c(అక్షరం *str)
{
    DWORD   nwrite = 0;
    DWORD   బహుళకం = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    ఒకవేళ (Get_Console_బహుళకం(h, &బహుళకం) && enc_codepage >= 0
	    && (పూర్ణాంకం)GetConsoleCP() != enc_codepage)
    {
	పూర్ణాంకం	len = (పూర్ణాంకం)STRLEN(str);
	WCHAR	*w = enc_గమ్యం_utf16((char_u *)str, &len);

	ఒకవేళ (w != ఖాళీ)
	{
	    WriteConsoleW(h, w, len, &nwrite, ఖాళీ);
	    vim_free(w);
	}
    }
    లేకపోతే
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
    శూన్య
mch_errmsg(అక్షరం *str)
{
# ఒకవేళ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    పూర్ణాంకం		len;
# endif

# ఒకవేళ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    ఒకవేళ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   లేకపోతే
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.లోపల_use || gui.starting)
#  endif
	    )
    {
	fprintf(stderr, "%s", str);
	తిరిగి;
    }
# endif

# ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ఒకవేళ (!(gui.లోపల_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	తిరిగి;
    }
# endif

# ఒకవేళ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (పూర్ణాంకం)STRLEN(str) + 1;
    ఒకవేళ (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    ఒకవేళ (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_దత్తాంశం + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_దత్తాంశం + error_ga.ga_len;
	    కొరకు (;;)
	    {
		p = vim_strchr(p, '\r');
		ఒకవేళ (p == ఖాళీ)
		    ఆపు;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    స్థిర శూన్య
mch_సందేశం_c(అక్షరం *str)
{
    DWORD   nwrite = 0;
    DWORD   బహుళకం;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    ఒకవేళ (Get_Console_బహుళకం(h, &బహుళకం) && enc_codepage >= 0
	    && (పూర్ణాంకం)GetConsoleCP() != enc_codepage)
    {
	పూర్ణాంకం	len = (పూర్ణాంకం)STRLEN(str);
	WCHAR	*w = enc_గమ్యం_utf16((char_u *)str, &len);

	ఒకవేళ (w != ఖాళీ)
	{
	    WriteConsoleW(h, w, len, &nwrite, ఖాళీ);
	    vim_free(w);
	}
    }
    లేకపోతే
    {
	రాయి("%s", str);
    }
}
# endif

/*
 * Give a message.  To be used when the screen hasn't been initialized yet.
 * When there is no tty, collect messages until the GUI has started and they
 * can be displayed in a message box.
 */
    శూన్య
mch_సందేశం(అక్షరం *str)
{
# ఒకవేళ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    ఒకవేళ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   లేకపోతే
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.లోపల_use || gui.starting)
#  endif
	    )
    {
	రాయి("%s", str);
	తిరిగి;
    }
# endif

# ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ఒకవేళ (!(gui.లోపల_use || gui.starting))
#  endif
    {
	mch_సందేశం_c(str);
	తిరిగి;
    }
# endif
# ఒకవేళ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#అయితే_ముగింపు // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    స్థిర శూన్య
సందేశం_screen_putchar(పూర్ణాంకం c, పూర్ణాంకం attr)
{
    సందేశం_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు, attr);
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (cmdmsg_rl)
    {
	ఒకవేళ (--సందేశం_నిలువు == 0)
	{
	    సందేశం_నిలువు = cmdline_వెడల్పు;
	    ++సందేశం_అడ్డువరుస;
	}
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	ఒకవేళ (++సందేశం_నిలువు >= cmdline_వెడల్పు)
	{
	    సందేశం_నిలువు = 0;
	    ++సందేశం_అడ్డువరుస;
	}
    }
}

    స్థిర శూన్య
సందేశం_moremsg(పూర్ణాంకం నిండిన)
{
    పూర్ణాంకం		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (పూర్ణాంకం)అడ్డువరుసలు - 1, cmdline_నిలువు_off, attr);
    ఒకవేళ (నిండిన)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(పూర్ణాంకం)అడ్డువరుసలు - 1, cmdline_నిలువు_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    శూన్య
repeat_సందేశం(శూన్య)
{
    ఒకవేళ (State == బహుళకం_ASKMORE)
    {
	సందేశం_moremsg(TRUE);	// display --more-- message again
	సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
    }
#నిర్వచితమైతే FEAT_CON_DIALOG
    లేకపోతే ఒకవేళ (State == బహుళకం_CONFIRM)
    {
	display_confirm_సందేశం();	// display ":confirm" message again
	సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
    }
#అయితే_ముగింపు
    లేకపోతే ఒకవేళ (State == బహుళకం_EXTERNCMD)
    {
	windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు); // put cursor back
    }
    లేకపోతే ఒకవేళ (State == బహుళకం_HITRETURN || State == బహుళకం_SETWSIZE)
    {
	ఒకవేళ (సందేశం_అడ్డువరుస == అడ్డువరుసలు - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    సందేశం_didout = FALSE;
	    సందేశం_నిలువు = 0;
	    సందేశం_clr_eos();
	}
	తగిలింది_return_సందేశం();
	సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    స్థిర పూర్ణాంకం
సందేశం_check_screen(శూన్య)
{
    ఒకవేళ (!నిండిన_screen || !screen_valid(FALSE))
	తిరిగి FALSE;

    ఒకవేళ (సందేశం_అడ్డువరుస >= అడ్డువరుసలు)
	సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
    ఒకవేళ (సందేశం_నిలువు >= cmdline_వెడల్పు)
	సందేశం_నిలువు = cmdline_వెడల్పు - 1;
    తిరిగి TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    శూన్య
సందేశం_clr_eos(శూన్య)
{
    ఒకవేళ (సందేశం_silent == 0)
	సందేశం_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    శూన్య
సందేశం_clr_eos_force(శూన్య)
{
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
    ఒకవేళ (లోపల_echowindow)
	తిరిగి;  // messages go into a popup
#అయితే_ముగింపు
    ఒకవేళ (సందేశం_use_printf())
    {
	ఒకవేళ (నిండిన_screen)	// only when termcap codes are valid
	{
	    ఒకవేళ (*T_CD)
		బహిర్గతం_str(T_CD);	// clear to end of display
	    లేకపోతే ఒకవేళ (*T_CE)
		బహిర్గతం_str(T_CE);	// clear to end of line
	}
    }
    లేకపోతే
    {
	పూర్ణాంకం సందేశం_attr;

	సందేశం_attr = HL_ATTR(HLF_సందేశం);

#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (cmdmsg_rl)
	{
	    screen_fill(సందేశం_అడ్డువరుస, సందేశం_అడ్డువరుస + 1,
		    cmdline_నిలువు_off, cmdline_నిలువు_off + సందేశం_నిలువు + 1,
		    ' ', ' ', సందేశం_attr);
	    screen_fill(సందేశం_అడ్డువరుస + 1, (పూర్ణాంకం)అడ్డువరుసలు,
		    cmdline_నిలువు_off, cmdline_నిలువు_off + cmdline_వెడల్పు,
		    ' ', ' ', సందేశం_attr);
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    screen_fill(సందేశం_అడ్డువరుస, సందేశం_అడ్డువరుస + 1,
		    cmdline_నిలువు_off + సందేశం_నిలువు, cmdline_నిలువు_off + cmdline_వెడల్పు,
		    ' ', ' ', సందేశం_attr);
	    screen_fill(సందేశం_అడ్డువరుస + 1, (పూర్ణాంకం)అడ్డువరుసలు,
		    cmdline_నిలువు_off, cmdline_నిలువు_off + cmdline_వెడల్పు,
		    ' ', ' ', సందేశం_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    శూన్య
సందేశం_clr_cmdline(శూన్య)
{
    సందేశం_అడ్డువరుస = cmdline_అడ్డువరుస;
    సందేశం_నిలువు = 0;
    సందేశం_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    పూర్ణాంకం
సందేశం_చివర(శూన్య)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    ఒకవేళ (!exiting && need_wait_return && !(State & బహుళకం_CMDLINE))
    {
	wait_return(FALSE);
	తిరిగి FALSE;
    }
    బహిర్గతం_flush();
    తిరిగి TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    శూన్య
సందేశం_check(శూన్య)
{
    ఒకవేళ (సందేశం_అడ్డువరుస == అడ్డువరుసలు - 1 && సందేశం_నిలువు >= sc_నిలువు
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
		&& !లోపల_echowindow
#అయితే_ముగింపు
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
    స్థిర శూన్య
redir_write(char_u *str, పూర్ణాంకం maxlen)
{
    char_u	*s = str;
    స్థిర పూర్ణాంకం	cur_నిలువు = 0;

    // Don't do anything for displaying prompts and the like.
    ఒకవేళ (redir_off)
	తిరిగి;

    // If 'verbosefile' is set prepare for writing in that file.
    ఒకవేళ (*p_vfile != NUL && verbose_fd == ఖాళీ)
	verbose_open();

    ఒకవేళ (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	ఒకవేళ (*s != '\n' && *s != '\r')
	{
	    అయితే (cur_నిలువు < సందేశం_నిలువు)
	    {
#నిర్వచితమైతే FEAT_EVAL
		ఒకవేళ (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		లేకపోతే ఒకవేళ (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		లేకపోతే ఒకవేళ (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		లేకపోతే
#అయితే_ముగింపు
		    ఒకవేళ (redir_fd != ఖాళీ)
		    fputs(" ", redir_fd);
		ఒకవేళ (verbose_fd != ఖాళీ)
		    fputs(" ", verbose_fd);
		++cur_నిలువు;
	    }
	}

#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (redir_execute)
	    execute_redir_str(s, maxlen);
	లేకపోతే ఒకవేళ (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	లేకపోతే ఒకవేళ (redir_vname)
	    var_redir_str(s, maxlen);
#అయితే_ముగింపు

	// Write and adjust the current column.
	అయితే (*s != NUL && (maxlen < 0 || (పూర్ణాంకం)(s - str) < maxlen))
	{
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (!redir_reg && !redir_vname && !redir_execute)
#అయితే_ముగింపు
		ఒకవేళ (redir_fd != ఖాళీ)
		    putc(*s, redir_fd);
	    ఒకవేళ (verbose_fd != ఖాళీ)
		putc(*s, verbose_fd);
	    ఒకవేళ (*s == '\r' || *s == '\n')
		cur_నిలువు = 0;
	    లేకపోతే ఒకవేళ (*s == '\t')
		cur_నిలువు += (8 - cur_నిలువు % 8);
	    లేకపోతే
		++cur_నిలువు;
	    ++s;
	}

	ఒకవేళ (సందేశం_silent != 0)	// should update msg_col
	    సందేశం_నిలువు = cur_నిలువు;
    }
}

    పూర్ణాంకం
redirecting(శూన్య)
{
    తిరిగి redir_fd != ఖాళీ || *p_vfile != NUL
#నిర్వచితమైతే FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#అయితే_ముగింపు
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    శూన్య
verbose_enter(శూన్య)
{
    ఒకవేళ (*p_vfile != NUL)
	++సందేశం_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    శూన్య
verbose_leave(శూన్య)
{
    ఒకవేళ (*p_vfile != NUL)
	ఒకవేళ (--సందేశం_silent < 0)
	    సందేశం_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    శూన్య
verbose_enter_scroll(శూన్య)
{
    ఒకవేళ (*p_vfile != NUL)
	++సందేశం_silent;
    లేకపోతే
	// always scroll up, don't overwrite
	సందేశం_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    శూన్య
verbose_leave_scroll(శూన్య)
{
    ఒకవేళ (*p_vfile != NUL)
    {
	ఒకవేళ (--సందేశం_silent < 0)
	    సందేశం_silent = 0;
    }
    లేకపోతే
	cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    శూన్య
verbose_stop(శూన్య)
{
    ఒకవేళ (verbose_fd != ఖాళీ)
    {
	fclose(verbose_fd);
	verbose_fd = ఖాళీ;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    పూర్ణాంకం
verbose_open(శూన్య)
{
    ఒకవేళ (verbose_fd == ఖాళీ && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((అక్షరం *)p_vfile, "a");
	ఒకవేళ (verbose_fd == ఖాళీ)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    తిరిగి FAIL;
	}
    }
    తిరిగి OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    శూన్య
give_warning(char_u *సందేశం_2, పూర్ణాంకం hl)
{
    give_warning_with_ఆధారం(సందేశం_2, hl, FALSE);
}

    శూన్య
give_warning_with_ఆధారం(char_u *సందేశం_2, పూర్ణాంకం hl, పూర్ణాంకం with_ఆధారం)
{
    // Don't do this for ":silent".
    ఒకవేళ (సందేశం_silent != 0)
	తిరిగి;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#నిర్వచితమైతే FEAT_EVAL
    సమితి_vim_var_string(VV_WARNINGMSG, సందేశం_2, -1);
#అయితే_ముగింపు
    VIM_CLEAR(keep_సందేశం);
    ఒకవేళ (hl)
	keep_సందేశం_attr = HL_ATTR(HLF_W);
    లేకపోతే
	keep_సందేశం_attr = 0;

    ఒకవేళ (with_ఆధారం)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	సందేశం_ప్రారంభం();
	సందేశం_ఆధారం(HL_ATTR(HLF_W));
	సందేశం_puts(" ");
	సందేశం_puts_attr((అక్షరం *)సందేశం_2, HL_ATTR(HLF_W) | సందేశం_HIST);
	సందేశం_clr_eos();
	(శూన్య)సందేశం_చివర();
    }
    లేకపోతే ఒకవేళ (సందేశం_attr((అక్షరం *)సందేశం_2, keep_సందేశం_attr) && సందేశం_scrolled == 0)
	సమితి_keep_సందేశం(సందేశం_2, keep_సందేశం_attr);

    సందేశం_didout = FALSE;	    // overwrite this message
    సందేశం_nowait = TRUE;	    // don't wait for this message
    సందేశం_నిలువు = 0;

    --no_wait_return;
}

#ఒకవేళ defined(FEAT_EVAL)
    శూన్య
give_warning2(char_u *సందేశం_2, char_u *a1, పూర్ణాంకం hl)
{
    ఒకవేళ (IObuff == ఖాళీ)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(సందేశం_2, hl);
    }
    లేకపోతే
    {
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, (అక్షరం *)సందేశం_2, a1);
	give_warning(IObuff, hl);
    }
}
#అయితే_ముగింపు

/*
 * Advance msg cursor to column "col".
 */
    శూన్య
సందేశం_advance(పూర్ణాంకం నిలువు)
{
    ఒకవేళ (సందేశం_silent != 0)	// nothing to advance to
    {
	సందేశం_నిలువు = నిలువు;		// for redirection, may fill it up later
	తిరిగి;
    }
    ఒకవేళ (నిలువు >= cmdline_వెడల్పు)	// not enough room
	నిలువు = cmdline_వెడల్పు - 1;
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (cmdmsg_rl)
	అయితే (సందేశం_నిలువు > cmdline_వెడల్పు - నిలువు)
	    సందేశం_putchar(' ');
    లేకపోతే
#అయితే_ముగింపు
	అయితే (సందేశం_నిలువు < నిలువు)
	    సందేశం_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    శూన్య
సందేశం_warn_missing_clipboard(శూన్య)
{
    ఒకవేళ (!global_busy && !did_warn_clipboard && silence_w23_w24_సందేశం == 0)
    {
#నిర్వచితమైతే FEAT_CLIPBOARD
	సందేశం(_("W23: Clipboard register not available, using register 0"));
#లేకపోతే
	సందేశం(_("W24: Clipboard register not available. See :h W24"));
#అయితే_ముగింపు
	did_warn_clipboard = TRUE;
    }
}

#ఒకవేళ defined(FEAT_CON_DIALOG)
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
    పూర్ణాంకం
do_dialog(
    పూర్ణాంకం		type వాడని,
    char_u	*title వాడని,
    char_u	*సందేశం_2,
    char_u	*buttons,
    పూర్ణాంకం		dfltbutton,
    char_u	*textfield వాడని,	// IObuff for inputdialog(), NULL
					// otherwise
    పూర్ణాంకం		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    పూర్ణాంకం		oldState;
    పూర్ణాంకం		retval = 0;
    char_u	*hotkeys;
    పూర్ణాంకం		c;
    పూర్ణాంకం		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    ఒకవేళ (silent_బహుళకం)
	తిరిగి dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    ఒకవేళ (gui.లోపల_use && vim_strchr(p_go, GO_CONDIALOG) == ఖాళీ)
    {
	// --gui-dialog-file: write text to a file
	ఒకవేళ (gui_dialog_log(title, సందేశం_2))
	    c = dfltbutton;
	లేకపోతే
	    c = gui_mch_dialog(type, title, సందేశం_2, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	బహిర్గతం_flush();
	gui_mch_update();

	తిరిగి c;
    }
# endif

    oldState = State;
    State = బహుళకం_CONFIRM;
    setmouse();

    // Ensure raw mode here.
    save_tmode = cur_tmode;
    settmode(TMODE_RAW);

    /*
     * Since we wait for a keypress, don't make the
     * user press RETURN as well afterwards.
     */
    ++no_wait_return;
    hotkeys = సందేశం_show_console_dialog(సందేశం_2, buttons, dfltbutton);

    ఒకవేళ (hotkeys != ఖాళీ)
    {
	కొరకు (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    ఎంపిక (c)
	    {
	    స్థితి CAR:		// User accepts default option
	    స్థితి NL:
		retval = dfltbutton;
		ఆపు;
	    స్థితి Ctrl_C:	// User aborts/cancels
	    స్థితి ESC:
		retval = 0;
		ఆపు;
	    అప్రమేయ:		// Could be a hotkey?
		ఒకవేళ (c < 0)	// special keys are ignored here
		    కొనసాగు;
		ఒకవేళ (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    ఆపు;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		కొరకు (i = 0; hotkeys[i]; ++i)
		{
		    ఒకవేళ (has_mbyte)
		    {
			ఒకవేళ ((*mb_ptr2char)(hotkeys + i) == c)
			    ఆపు;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    లేకపోతే
			ఒకవేళ (hotkeys[i] == c)
			    ఆపు;
		    ++retval;
		}
		ఒకవేళ (hotkeys[i])
		    ఆపు;
		// No hotkey match, so keep waiting
		కొనసాగు;
	    }
	    ఆపు;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    సందేశం_చివర_prompt();

    తిరిగి retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    స్థిర పూర్ణాంకం
copy_char(
    char_u	*from,
    char_u	*గమ్యం,
    పూర్ణాంకం		lowercase)	// make character lower case
{
    పూర్ణాంకం		len;
    పూర్ణాంకం		c;

    ఒకవేళ (has_mbyte)
    {
	ఒకవేళ (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    తిరిగి (*mb_char2bytes)(c, గమ్యం);
	}
	లేకపోతే
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(గమ్యం, from, (size_t)len);
	    తిరిగి len;
	}
    }
    లేకపోతే
    {
	ఒకవేళ (lowercase)
	    *గమ్యం = (char_u)TOLOWER_LOC(*from);
	లేకపోతే
	    *గమ్యం = *from;
	తిరిగి 1;
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
    స్థిర char_u *
సందేశం_show_console_dialog(
    char_u	*సందేశం_2,
    char_u	*buttons,
    పూర్ణాంకం		dfltbutton)
{
    పూర్ణాంకం		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    పూర్ణాంకం		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = ఖాళీ;
    char_u	*msgp = ఖాళీ;
    char_u	*hotkp = ఖాళీ;
    char_u	*r;
    పూర్ణాంకం		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    పూర్ణాంకం		మొదటి_hotkey = FALSE;	// first char of button is hotkey
    పూర్ణాంకం		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    కొరకు (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	అయితే (*r)
	{
	    ఒకవేళ (*r == DLG_BUTTON_SEP)
	    {
		ఒకవేళ (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    ఒకవేళ (has_mbyte)
			hotkp += STRLEN(hotkp);
		    లేకపోతే
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    ఒకవేళ (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    ఒకవేళ (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			మొదటి_hotkey = TRUE;
		}
		లేకపోతే
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    ఒకవేళ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    లేకపోతే ఒకవేళ (*r == DLG_HOTKEY_CHAR || మొదటి_hotkey)
	    {
		ఒకవేళ (*r == DLG_HOTKEY_CHAR)
		    ++r;
		మొదటి_hotkey = FALSE;
		ఒకవేళ (copy)
		{
		    ఒకవేళ (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    లేకపోతే
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		లేకపోతే
		{
		    ++len;	    // '&a' -> '[a]'
		    ఒకవేళ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    లేకపోతే
	    {
		// everything else copy literally
		ఒకవేళ (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	ఒకవేళ (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	లేకపోతే
	{
	    len += (పూర్ణాంకం)(STRLEN(సందేశం_2)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    ఒకవేళ (!has_hotkey[0])
	    {
		మొదటి_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_సందేశం);
	    confirm_సందేశం = alloc(len);
	    ఒకవేళ (confirm_సందేశం == ఖాళీ)
		తిరిగి ఖాళీ;
	    *confirm_సందేశం = NUL;
	    hotk = alloc(lenhotkey);
	    ఒకవేళ (hotk == ఖాళీ)
		తిరిగి ఖాళీ;

	    *confirm_సందేశం = '\n';
	    STRCPY(confirm_సందేశం + 1, సందేశం_2);

	    msgp = confirm_సందేశం + 1 + STRLEN(సందేశం_2);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_సందేశం_తోక = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_సందేశం();
    తిరిగి hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    స్థిర శూన్య
display_confirm_సందేశం(శూన్య)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_సందేశం_వాడిన;
    ఒకవేళ (confirm_సందేశం != ఖాళీ)
	సందేశం_puts_attr((అక్షరం *)confirm_సందేశం, HL_ATTR(HLF_M));
    --confirm_సందేశం_వాడిన;
}

#అయితే_ముగింపు // FEAT_CON_DIALOG

#ఒకవేళ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    పూర్ణాంకం
vim_dialog_yesno(
    పూర్ణాంకం		type,
    char_u	*title,
    char_u	*సందేశం_2,
    పూర్ణాంకం		dflt)
{
    ఒకవేళ (do_dialog(type,
		title == ఖాళీ ? (char_u *)_("Question") : title,
		సందేశం_2,
		(char_u *)_("&Yes\n&No"), dflt, ఖాళీ, FALSE) == 1)
	తిరిగి VIM_YES;
    తిరిగి VIM_NO;
}

    పూర్ణాంకం
vim_dialog_yesnocancel(
    పూర్ణాంకం		type,
    char_u	*title,
    char_u	*సందేశం_2,
    పూర్ణాంకం		dflt)
{
    ఎంపిక (do_dialog(type,
		title == ఖాళీ ? (char_u *)_("Question") : title,
		సందేశం_2,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, ఖాళీ, FALSE))
    {
	స్థితి 1: తిరిగి VIM_YES;
	స్థితి 2: తిరిగి VIM_NO;
    }
    తిరిగి VIM_CANCEL;
}

    పూర్ణాంకం
vim_dialog_yesnoallcancel(
    పూర్ణాంకం		type,
    char_u	*title,
    char_u	*సందేశం_2,
    పూర్ణాంకం		dflt)
{
    ఎంపిక (do_dialog(type,
		title == ఖాళీ ? (char_u *)"Question" : title,
		సందేశం_2,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, ఖాళీ, FALSE))
    {
	స్థితి 1: తిరిగి VIM_YES;
	స్థితి 2: తిరిగి VIM_NO;
	స్థితి 3: తిరిగి VIM_ALL;
	స్థితి 4: తిరిగి VIM_DISCARDALL;
    }
    తిరిగి VIM_CANCEL;
}

#అయితే_ముగింపు // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
