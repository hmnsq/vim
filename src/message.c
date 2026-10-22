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

#وصف_ڪريو پيغام_FILE		// don't include prototype for smsg()

#ضمن "vim.h"

ساكن فراغ جوڙيو_نياپو_hist(char_u *s, صحيح len, صحيح attr);
ساكن فراغ check_نياپو_hist(فراغ);
ساكن فراغ ڪاميابي_return_نياپو(فراغ);
ساكن فراغ نياپو_home_replace_attr(char_u *fname, صحيح attr);
ساكن فراغ نياپو_puts_attr_len(حرف *str, صحيح maxlen, صحيح attr);
ساكن فراغ نياپو_puts_display(char_u *str, صحيح maxlen, صحيح attr, صحيح recurse);
ساكن فراغ نياپو_scroll_مٿي(فراغ);
ساكن فراغ inc_نياپو_scrolled(فراغ);
ساكن فراغ store_sb_متن(char_u **sb_str, char_u *s, صحيح attr, صحيح *sb_کانو, صحيح پڄاڻي);
ساكن فراغ t_puts(صحيح *t_کانو, char_u *t_s, char_u *s, صحيح attr);
ساكن فراغ نياپو_puts_printf(char_u *str, صحيح maxlen);
ساكن صحيح do_وڌيڪ_prompt(صحيح typed_char);
ساكن فراغ نياپو_screen_putchar(صحيح c, صحيح attr);
ساكن فراغ نياپو_moremsg(صحيح ڀريل);
ساكن صحيح  نياپو_check_screen(فراغ);
ساكن فراغ redir_write(char_u *s, صحيح maxlen);
#جيڪڏهن_وصف_ٿيل FEAT_CON_DIALOG
ساكن char_u *نياپو_show_console_dialog(char_u *پيغام, char_u *buttons, صحيح dfltbutton);
ساكن صحيح	confirm_نياپو_استعمال_ٿيل = FALSE;	// displaying confirm_msg
ساكن char_u	*confirm_نياپو = عدم;		// ":confirm" message
ساكن char_u	*confirm_نياپو_پڇ;		// tail of confirm_msg
ساكن فراغ display_confirm_نياپو(فراغ);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
ساكن صحيح emsg_منزل_channel_log = FALSE;
#جيڪڏهن_پورو

بنية نياپو_hist
{
    بنية نياپو_hist	*ايندڙ;
    char_u		*نياپو;
    صحيح			attr;
};

ساكن بنية نياپو_hist *پهريون_نياپو_hist = عدم;
ساكن بنية نياپو_hist *آخري_نياپو_hist = عدم;
ساكن صحيح نياپو_hist_len = 0;
ساكن صحيح نياپو_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#وصف_ڪريو MESSAGES_ڪاميابي_ENTER	0x001
#وصف_ڪريو MESSAGES_WAIT		0x002
#وصف_ڪريو MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#وصف_ڪريو MESSAGES_OPT_ڪاميابي_ENTER "hit-enter"
#وصف_ڪريو MESSAGES_OPT_WAIT "wait:"
#وصف_ڪريو MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
ساكن صحيح نياپو_flags = MESSAGES_ڪاميابي_ENTER | MESSAGES_HISTORY;
ساكن صحيح نياپو_wait = 0;

ساكن FILE *verbose_fd = عدم;
ساكن صحيح  verbose_did_open = FALSE;

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
    صحيح
نياپو(حرف *s)
{
    أرجع نياپو_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    صحيح
verb_نياپو(حرف *s)
{
    صحيح		n;

    verbose_enter();
    n = نياپو_attr_keep(s, 0, FALSE);
    verbose_leave();

    أرجع n;
}

    صحيح
نياپو_attr(حرف *s, صحيح attr)
{
    أرجع نياپو_attr_keep(s, attr, FALSE);
}

    صحيح
نياپو_attr_keep(
    حرف	*s,
    صحيح		attr,
    صحيح		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    ساكن صحيح	entered = 0;
    صحيح		retval;
    char_u	*buf = عدم;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    إذا (!emsg_on_display && پيغام_filtered((char_u *)s))
	أرجع TRUE;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (attr == 0)
	سيٽ_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#جيڪڏهن_پورو

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    إذا (entered >= 3)
	أرجع TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    إذا ((char_u *)s != keep_نياپو
	    || (*s != '<'
		&& آخري_نياپو_hist != عدم
		&& آخري_نياپو_hist->نياپو != عدم
		&& STRCMP(s, آخري_نياپو_hist->نياپو) != 0))
	جوڙيو_نياپو_hist((char_u *)s, -1, attr);

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (emsg_منزل_channel_log)
	// Write message in the channel log.
	ch_log(عدم, "ERROR: %s", s);
#جيڪڏهن_پورو

    // Truncate the message if needed.
    نياپو_شروعات();
    buf = نياپو_strtrunc((char_u *)s, FALSE);
    إذا (buf != عدم)
	s = (حرف *)buf;

    نياپو_outtrans_attr((char_u *)s, attr);
    نياپو_clr_eos();
    retval = نياپو_پڇاڙي();

    إذا (keep && retval && vim_strsize((char_u *)s)
		    < (صحيح)(صفون - cmdline_صف - 1) * cmdline_ويڪر + sc_کانو)
	سيٽ_keep_نياپو((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    أرجع retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
نياپو_strtrunc(
    char_u	*s,
    صحيح		force)	    // always truncate
{
    char_u	*buf = عدم;
    صحيح		len;
    صحيح		گنجائش;

    // May truncate message to avoid a hit-return prompt
    إذا ((!نياپو_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_سرگرم && نياپو_silent == 0) || force)
    {
	len = vim_strsize(s);
	إذا (نياپو_scrolled != 0
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
		|| اندريون_echowindow
#جيڪڏهن_پورو
		)
	    // Use all the columns.
	    گنجائش = (صحيح)(صفون - نياپو_صف) * cmdline_ويڪر - 1;
	وإلا
	    // Use up to 'showcmd' column.
	    گنجائش = (صحيح)(صفون - نياپو_صف - 1) * cmdline_ويڪر + sc_کانو - 1;
	إذا (len > گنجائش && گنجائش > 0)
	{
	    إذا (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (گنجائش + 2) * 18;
	    وإلا إذا (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (گنجائش + 2) * 2;
	    وإلا
		len = گنجائش + 2;
	    buf = alloc(len);
	    إذا (buf != عدم)
		trunc_string(s, buf, گنجائش, len);
	}
    }
    أرجع buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    فراغ
trunc_string(
    char_u	*s,
    char_u	*buf,
    صحيح		گنجائش_اندريون,
    صحيح		buflen)
{
    size_t	گنجائش = گنجائش_اندريون - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    صحيح		e;
    صحيح		i;
    صحيح		n;

    إذا (*s == NUL)
    {
	إذا (buflen > 0)
	    *buf = NUL;
	أرجع;
    }

    إذا (گنجائش_اندريون < 3)
	گنجائش = 0;
    half = گنجائش / 2;

    // First part: Start of the string.
    لكل (e = 0; len < half && e < buflen; ++e)
    {
	إذا (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    أرجع;
	}
	n = ptr2cells(s + e);
	إذا (len + n > half)
	    اكسر;
	len += n;
	buf[e] = s[e];
	إذا (has_mbyte)
	    لكل (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		إذا (++e == buflen)
		    اكسر;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    إذا (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	طالما (len + n > گنجائش)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    وإلا إذا (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (صحيح)STRLEN(s);
	لكل (;;)
	{
	    افعل
		half = half - utf_سر_off(s, s + half - 1) - 1;
	    طالما (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    إذا (len + n > گنجائش || half == 0)
		اكسر;
	    len += n;
	    i = (صحيح)half;
	}
    }
    وإلا
    {
	لكل (i = (صحيح)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= گنجائش; --i)
	    len += n;
    }


    إذا (i <= e + 3)
    {
	// text fits without truncating
	إذا (s != buf)
	{
	    len = STRLEN(s);
	    إذا (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    إذا (len < 1)
		buf[e - 1] = NUL;
	    وإلا
		mch_memmove(buf + e, s + e, len);
	}
    }
    وإلا إذا (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	إذا (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    وإلا
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
#جيڪڏهن_وصف_نه_ٿيل PROTO

صحيح vim_snprintf(حرف *str, size_t str_m, ثابت حرف *fmt, ...);

    صحيح
smsg(ثابت حرف *s, ...)
{
    إذا (IObuff == عدم)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	أرجع نياپو((حرف *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((حرف *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    أرجع نياپو((حرف *)IObuff);
}

    صحيح
smsg_attr(صحيح attr, ثابت حرف *s, ...)
{
    إذا (IObuff == عدم)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	أرجع نياپو_attr((حرف *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((حرف *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    أرجع نياپو_attr((حرف *)IObuff, attr);
}

    صحيح
smsg_attr_keep(صحيح attr, ثابت حرف *s, ...)
{
    إذا (IObuff == عدم)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	أرجع نياپو_attr_keep((حرف *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((حرف *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    أرجع نياپو_attr_keep((حرف *)IObuff, attr, TRUE);
}

#جيڪڏهن_پورو

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
ساكن صحيح	آخري_sourcing_lnum = 0;
ساكن char_u   *آخري_sourcing_name = عدم;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    فراغ
reset_آخري_sourcing(فراغ)
{
    VIM_CLEAR(آخري_sourcing_name);
    آخري_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    ساكن صحيح
other_sourcing_name(فراغ)
{
    إذا (HAVE_SOURCING_INFO && SOURCING_NAME != عدم)
    {
	إذا (آخري_sourcing_name != عدم)
	    أرجع STRCMP(SOURCING_NAME, آخري_sourcing_name) != 0;
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ساكن char_u *
get_emsg_ذريعو(فراغ)
{
    char_u	*Buf, *p;

    إذا (HAVE_SOURCING_INFO && SOURCING_NAME != عدم && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	إذا (sname == عدم)
	    sname = SOURCING_NAME;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	وإلا
#جيڪڏهن_پورو
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	إذا (Buf != عدم)
	    sprintf((حرف *)Buf, (حرف *)p, sname);
	vim_free(tofree);
	أرجع Buf;
    }
    أرجع عدم;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ساكن char_u *
get_emsg_lnum(فراغ)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    إذا (SOURCING_NAME != عدم
	    && (other_sourcing_name() || SOURCING_LNUM != آخري_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	إذا (Buf != عدم)
	    sprintf((حرف *)Buf, (حرف *)p, (ڊگهو)SOURCING_LNUM);
	أرجع Buf;
    }
    أرجع عدم;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    فراغ
نياپو_ذريعو(صحيح attr)
{
    char_u	*p;
    ساكن صحيح	recursive = FALSE;

    // Bail out if something called here causes an error.
    إذا (recursive)
	أرجع;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_ذريعو();
    إذا (p != عدم)
    {
	نياپو_scroll = TRUE;  // this will take more than one line
	نياپو_attr((حرف *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    إذا (p != عدم)
    {
	نياپو_attr((حرف *)p, HL_ATTR(HLF_N));
	vim_free(p);
	آخري_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    إذا (SOURCING_NAME == عدم || other_sourcing_name())
    {
	VIM_CLEAR(آخري_sourcing_name);
	إذا (SOURCING_NAME != عدم)
	    آخري_sourcing_name = vim_strsave(SOURCING_NAME);
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
    ساكن صحيح
emsg_not_now(فراغ)
{
    إذا ((emsg_off > 0 && vim_strchr(p_debug, 'm') == عدم
					  && vim_strchr(p_debug, 't') == عدم)
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    || emsg_skip > 0
#جيڪڏهن_پورو
	    )
	أرجع TRUE;
    أرجع FALSE;
}

#إذا defined(FEAT_EVAL)
ساكن garray_T ignore_error_list = GA_خالي_جايون;

    فراغ
ignore_error_for_testing(char_u *error)
{
    إذا (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, حجم(char_u *), 1);

    إذا (STRCMP("RESET", error) == 0)
	ga_clear_لڙيون(&ignore_error_list);
    وإلا
	ga_copy_string(&ignore_error_list, error);
}

    ساكن صحيح
ignore_error(ثابت حرف *نياپو)
{
    صحيح i;

    لكل (i = 0; i < ignore_error_list.ga_len; ++i)
	إذا (لڙي_ڳولا(نياپو,
		  (حرف *)((char_u **)(ignore_error_list.ga_معلومات))[i]) != عدم)
	    أرجع TRUE;
    أرجع FALSE;
}
#جيڪڏهن_پورو

#إذا !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    فراغ
do_perror(حرف *نياپو)
{
    perror(نياپو);
    ++emsg_silent;
    emsg(نياپو);
    --emsg_silent;
}
#جيڪڏهن_پورو

/*
 * emsg_core() - display an error message
 *
 * Rings the bell, if appropriate, and calls message() to do the real work
 * When terminal not initialized (yet) mch_errmsg(..) is used.
 *
 * Return TRUE if wait_return() not called.
 * Note: caller must check 'emsg_not_now()' before calling this.
 */
    ساكن صحيح
emsg_core(ثابت حرف *s)
{
    صحيح		attr;
    char_u	*p;
    صحيح		r;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    صحيح		ignore = FALSE;
    صحيح		severe;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    // When testing some errors are turned into a normal message.
    إذا (ignore_error(s))
	// don't call msg() if it results in a dialog
	أرجع نياپو_use_printf() ? FALSE : نياپو((حرف *)s);
#جيڪڏهن_پورو

    ++called_emsg;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#جيڪڏهن_پورو

    إذا (!emsg_off || vim_strchr(p_debug, 't') != عدم)
    {
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	إذا (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    إذا (!ignore)
		++did_emsg;
	    أرجع TRUE;
	}

	إذا (اندريون_assert_fails && emsg_assert_fails_نياپو == عدم)
	{
	    emsg_assert_fails_نياپو = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == عدم ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	سيٽ_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#جيڪڏهن_پورو

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	إذا (emsg_silent != 0)
	{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    ++did_emsg_silent;
#جيڪڏهن_پورو
	    إذا (emsg_noredir == 0)
	    {
		نياپو_شروعات();
		p = get_emsg_ذريعو();
		إذا (p != عدم)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		إذا (p != عدم)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		redir_write((char_u *)s, -1);
	    }
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    // Only increment did_emsg_def when :silent! wasn't used inside the
	    // :def function.
	    إذا (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    ch_log(عدم, "ERROR silent: %s", s);
#جيڪڏهن_پورو
	    أرجع TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	نياپو_silent = 0;
	cmd_silent = FALSE;

	إذا (global_busy)		// break :global command
	    ++global_busy;

	إذا (p_eb)
	    beep_flush();		// also includes flush_buffers()
	وإلا
	    flush_buffers(FLUSH_گهٽتر);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	++uncaught_emsg;
#جيڪڏهن_پورو
    }

#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
    إذا (!اندريون_echowindow)
#جيڪڏهن_پورو
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    إذا (نياپو_scrolled != 0)
	need_wait_return = TRUE;    // needed in case emsg() is called after
				    // wait_return() has reset need_wait_return
				    // and a redraw is expected because
				    // msg_scrolled is non-zero

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    emsg_منزل_channel_log = TRUE;
#جيڪڏهن_پورو
    /*
     * Display name and line number for the source of the error.
     */
    نياپو_scroll = TRUE;
    نياپو_ذريعو(attr);

    /*
     * Display the error message itself.
     */
    نياپو_nowait = FALSE;			// wait for this msg
    r = نياپو_attr((حرف *)s, attr);

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    emsg_منزل_channel_log = FALSE;
#جيڪڏهن_پورو
    أرجع r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    صحيح
emsg(حرف *s)
{
    // Skip this if not giving error messages at the moment.
    إذا (emsg_not_now())
	أرجع TRUE;

    أرجع emsg_core(s);
}

#جيڪڏهن_وصف_نه_ٿيل PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    صحيح
semsg(ثابت حرف *s, ...)
{
    // Skip this if not giving error messages at the moment.
    إذا (emsg_not_now())
	أرجع TRUE;

    إذا (IObuff == عدم)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	أرجع emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((حرف *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    أرجع emsg_core((حرف *)IObuff);
}
#جيڪڏهن_پورو

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    فراغ
iemsg(حرف *s)
{
    إذا (emsg_not_now())
	أرجع;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_اندروني_error_please_report_a_bug));

    emsg_core(s);
#إذا defined(ABORT_ON_اندروني_ERROR) && defined(FEAT_EVAL)
    سيٽ_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    نياپو_putchar('\n');  // avoid overwriting the error message
    ٻاهريون_flush();
    abort();
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_نه_ٿيل PROTO  // manual proto with __attribute__
/*
 * Same as semsg(...) but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 * Note: caller must not pass 'IObuff' as 1st argument.
 */
    فراغ
siemsg(ثابت حرف *s, ...)
{
    إذا (emsg_not_now())
	أرجع;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_اندروني_error_please_report_a_bug));

    إذا (IObuff == عدم)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    وإلا
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((حرف *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((حرف *)IObuff);
    }
# ifdef ABORT_ON_اندروني_ERROR
    نياپو_putchar('\n');  // avoid overwriting the error message
    ٻاهريون_flush();
    abort();
# endif
}
#جيڪڏهن_پورو

/*
 * Give an "Internal error" message.
 */
    فراغ
اندروني_error(حرف *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_اندروني_error_please_report_a_bug));

    siemsg(_(e_اندروني_error_str), where);
}

#إذا defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    فراغ
اندروني_error_no_abort(حرف *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_اندروني_error_please_report_a_bug));

    semsg(_(e_اندروني_error_str), where);
}
#جيڪڏهن_پورو

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    فراغ
emsg_invreg(صحيح name)
{
    semsg(_(e_invalid_رجسٽر_name_str), transchar_buf(عدم, name));
}

#إذا defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    فراغ
emsg_namelen(حرف *نياپو, char_u *name, صحيح len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(نياپو, copy == عدم ? "NULL" : (حرف *)copy);
    vim_free(copy);
}
#جيڪڏهن_پورو

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    حرف *
نياپو_trunc_attr(حرف *s, صحيح force, صحيح attr)
{
    صحيح		n;
    حرف	*ts;

    // Add message to history before truncating
    جوڙيو_نياپو_hist((char_u *)s, -1, attr);

    ts = (حرف *)نياپو_may_trunc(force, (char_u *)s);

    نياپو_hist_off = TRUE;
    n = نياپو_attr(ts, attr);
    نياپو_hist_off = FALSE;

    إذا (n)
	أرجع ts;
    أرجع عدم;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
نياپو_may_trunc(صحيح force, char_u *s)
{
    صحيح		n;
    صحيح		گنجائش;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    گنجائش = (صحيح)(صفون - cmdline_صف - 1) * cmdline_ويڪر + sc_کانو - 1;
    إذا (گنجائش > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_سرگرم))
	    && (n = (صحيح)STRLEN(s) - گنجائش) > 0)
    {
	إذا (has_mbyte)
	{
	    صحيح	سائيز = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    إذا (سائيز <= گنجائش)
		أرجع s;

	    لكل (n = 0; سائيز >= گنجائش; )
	    {
		سائيز -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    أرجع s;
}

    ساكن فراغ
جوڙيو_نياپو_hist(
    char_u	*s,
    صحيح		len,		// -1 for undetermined length
    صحيح		attr)
{
    بنية نياپو_hist *p;

    إذا (نياپو_hist_off || نياپو_silent != 0)
	أرجع;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(بنية نياپو_hist);
    إذا (p == عدم)
	أرجع;

    إذا (len < 0)
	len = (صحيح)STRLEN(s);
    // remove leading and trailing newlines
    طالما (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    طالما (len > 0 && s[len - 1] == '\n')
	--len;
    p->نياپو = vim_strnsave(s, len);
    p->ايندڙ = عدم;
    p->attr = attr;
    إذا (آخري_نياپو_hist != عدم)
	آخري_نياپو_hist->ايندڙ = p;
    آخري_نياپو_hist = p;
    إذا (پهريون_نياپو_hist == عدم)
	پهريون_نياپو_hist = آخري_نياپو_hist;
    ++نياپو_hist_len;

    check_نياپو_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    صحيح
delete_پهريون_نياپو(فراغ)
{
    بنية نياپو_hist *p;

    إذا (نياپو_hist_len <= 0)
	أرجع FAIL;
    p = پهريون_نياپو_hist;
    پهريون_نياپو_hist = p->ايندڙ;
    إذا (پهريون_نياپو_hist == عدم)
	آخري_نياپو_hist = عدم;  // history is empty
    vim_free(p->نياپو);
    vim_free(p);
    --نياپو_hist_len;
    أرجع OK;
}

    ساكن فراغ
check_نياپو_hist(فراغ)
{
    // Don't let the message history get too big
    طالما (نياپو_hist_len > 0 && نياپو_hist_len > نياپو_hist_max)
	(فراغ)delete_پهريون_نياپو();
}


    صحيح
messagesopt_بدليو(فراغ)
{
    char_u	*p;
    صحيح		messages_flags_new = 0;
    صحيح		messages_wait_new = 0;
    صحيح		messages_history_new = 0;

    p = p_mopt;
    طالما (*p != NUL)
    {
	إذا (STRNCMP(p, MESSAGES_OPT_ڪاميابي_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_ڪاميابي_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_ڪاميابي_ENTER);
	    messages_flags_new |= MESSAGES_ڪاميابي_ENTER;
	}
	وإلا إذا (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	وإلا إذا (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	إذا (*p != ',' && *p != NUL)
	    أرجع FAIL;
	إذا (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    إذا (!(messages_flags_new & (MESSAGES_ڪاميابي_ENTER | MESSAGES_WAIT)))
	أرجع FAIL;

    // "history" must be set
    إذا (!(messages_flags_new & MESSAGES_HISTORY))
	أرجع FAIL;

    إذا (messages_history_new < 0 || messages_history_new > 10000)
	أرجع FAIL;

    إذا (messages_wait_new < 0 || messages_wait_new > 10000)
	أرجع FAIL;

    نياپو_flags = messages_flags_new;
    نياپو_wait = messages_wait_new;

    نياپو_hist_max = messages_history_new;
    check_نياپو_hist();

    أرجع OK;
}

/*
 * ":messages" command.
 */
    فراغ
ex_messages(exarg_T *eap)
{
    بنية نياپو_hist *p;
    char_u	    *s;
    صحيح		    c = 0;

    إذا (STRCMP(eap->arg, "clear") == 0)
    {
	صحيح keep = eap->addr_ڳڻپ == 0 ? 0 : eap->line2;

	طالما (نياپو_hist_len > keep)
	    (فراغ)delete_پهريون_نياپو();
	أرجع;
    }

    إذا (*eap->arg != NUL)
    {
	emsg(_(e_invalid_دليل));
	أرجع;
    }

    نياپو_hist_off = TRUE;

    p = پهريون_نياپو_hist;
    إذا (eap->addr_ڳڻپ != 0)
    {
	// Count total messages
	لكل (; p != عدم && !got_int; p = p->ايندڙ)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	لكل (p = پهريون_نياپو_hist; p != عدم && !got_int && c > 0;
						    p = p->ايندڙ, c--);
    }

    إذا (p == پهريون_نياپو_hist)
    {
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
	s = get_mess_lang();
#وإلا
	s = mch_getenv((char_u *)"LANG");
#جيڪڏهن_پورو
	إذا (s != عدم && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    نياپو_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    لكل (; p != عدم && !got_int; p = p->ايندڙ)
	إذا (p->نياپو != عدم)
	    نياپو_attr((حرف *)p->نياپو, p->attr);

    نياپو_hist_off = FALSE;
}

#إذا defined(FEAT_CON_DIALOG) || defined(ڳوليو_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    فراغ
نياپو_پڇاڙي_prompt(فراغ)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_صف = نياپو_صف;
    نياپو_کانو = 0;
    نياپو_clr_eos();
    سٽون_کاٻو = -1;
}
#جيڪڏهن_پورو

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    فراغ
wait_return(صحيح redraw)
{
    صحيح		c;
    صحيح		oldState;
    صحيح		tmpState;
    صحيح		had_got_int;
    صحيح		save_reg_recording;
    FILE	*save_scriptout;

    إذا (redraw == TRUE)
	سيٽ_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    إذا (نياپو_silent != 0)
	أرجع;
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
    إذا (اندريون_echowindow)
	أرجع;
#جيڪڏهن_پورو

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    إذا (vgetc_busy > 0)
	أرجع;
    need_wait_return = TRUE;
    إذا (no_wait_return)
    {
	إذا (!exmode_سرگرم)
	    cmdline_صف = نياپو_صف;
	أرجع;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    إذا (quit_وڌيڪ)
    {
	c = CAR;		// just pretend CR was hit
	quit_وڌيڪ = FALSE;
	got_int = FALSE;
    }
    وإلا إذا (exmode_سرگرم)
    {
	نياپو_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    وإلا إذا (!stuff_خالي_جايون())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    وإلا
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = گهڻائي_HITRETURN;
	setmouse();
#جيڪڏهن_وصف_ٿيل USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#جيڪڏهن_پورو
	cmdline_صف = نياپو_صف;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	إذا (need_check_timestamps)
	    check_timestamps(FALSE);

	إذا (نياپو_flags & MESSAGES_ڪاميابي_ENTER)
	{
	    ڪاميابي_return_نياپو();

	    افعل
	    {
#جيڪڏهن_وصف_ٿيل FEAT_IMAGE
		// Make sure to update image placements (since their row offset
		// will have been changed).
		draw_image_placements();
#جيڪڏهن_پورو
		// Remember "got_int", if it is set vgetc() probably returns a
		// CTRL-C, but we need to loop then.
		had_got_int = got_int;

		// Don't do mappings here, we put the character back in the
		// typeahead buffer.
		++no_mapping;
		++allow_ڪنجيون;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = عدم;
		c = محفوظ_vgetc();
		إذا (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_ڪنجيون;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		إذا (ڪنجي_Typed && c == Ctrl_Y && clip_تارو.state == SELECT_پورو)
		{
		    clip_copy_modeless_selection(TRUE);
		    c = K_IGNORE;
		}
#جيڪڏهن_پورو

		/*
		* Allow scrolling back in the messages.
		* Also accept scroll-down commands when messages fill the
		* screen, to avoid that typing one 'j' too many makes the
		* messages disappear.
		*/
		إذا (ڪنجي_Typed && p_وڌيڪ && !p_cp)
		{
		    إذا (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_مٿي || c == K_PAGEUP)
		    {
			إذا (نياپو_scrolled > صفون)
			    // scroll back to show older messages
			    do_وڌيڪ_prompt(c);
			وإلا
			{
			    نياپو_didout = FALSE;
			    c = K_IGNORE;
			    نياپو_کانو =
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_ويڪر - 1 :
#جيڪڏهن_پورو
				0;
			}
			إذا (quit_وڌيڪ)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_وڌيڪ = FALSE;
			    got_int = FALSE;
			}
			وإلا إذا (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    ڪاميابي_return_نياپو();
			}
		    }
		    وإلا إذا (نياپو_scrolled > صفون - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_هيٺ_ڏانهن || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } طالما ((had_got_int && c == Ctrl_C)
				|| c == K_IGNORE
#جيڪڏهن_وصف_ٿيل FEAT_GUI
				|| c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
#جيڪڏهن_پورو
				|| c == K_LEFTDRAG   || c == K_LEFTRELEASE
				|| c == K_MIDDLEDRAG || c == K_MIDDLERELEASE
				|| c == K_RIGHTDRAG  || c == K_RIGHTRELEASE
				|| c == K_MOUSELEFT  || c == K_MOUSERIGHT
				|| c == K_MOUSEDOWN  || c == K_MOUSEUP
				|| c == K_MOUSEMOVE
				|| (!mouse_has(MOUSE_RETURN)
				    && mouse_صف < نياپو_صف
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    إذا (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(فراغ)jump_منزل_mouse(MOUSE_SETPOS, عدم, 0);
	    وإلا إذا (!ڪنجي_Typed || (vim_strchr((char_u *)"\r\n ", c) == عدم
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_نقاب);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	وإلا
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(نياپو_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    إذا (c == ':' || c == '?' || c == '/')
    {
	إذا (!exmode_سرگرم)
	    cmdline_صف = نياپو_صف;
	skip_redraw = TRUE;	    // skip redraw once
	do_redraw = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
	skip_رڪن_loop = TRUE;
#جيڪڏهن_پورو
    }

    /*
     * If the window size changed set_shellsize() will redraw the screen.
     * Otherwise the screen is only redrawn if 'redraw' is set and no ':'
     * typed.
     */
    tmpState = State;
    State = oldState;		    // restore State before set_shellsize
    setmouse();
    نياپو_check();

#جيڪڏهن_وصف_نه_ٿيل MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    إذا (swapping_screen() && !termcap_سرگرم)
	newline_on_exit = TRUE;
#جيڪڏهن_پورو

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    سٽون_کاٻو = -1;		// reset lines_left at next msg_start()
    reset_آخري_sourcing();
    إذا (keep_نياپو != عدم && vim_strsize(keep_نياپو) >=
			    (صفون - cmdline_صف - 1) * cmdline_ويڪر + sc_کانو)
	VIM_CLEAR(keep_نياپو);	    // don't redisplay message, it's too long

    إذا (tmpState == گهڻائي_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    وإلا إذا (!skip_redraw
	    && (redraw == TRUE || (نياپو_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    ساكن فراغ
ڪاميابي_return_نياپو(فراغ)
{
    صحيح		save_p_وڌيڪ = p_وڌيڪ;

    p_وڌيڪ = FALSE;	// don't want to see this message when scrolling back
    إذا (نياپو_didout)	// start on a new line
	نياپو_putchar('\n');
    إذا (got_int)
	نياپو_puts(_("Interrupt: "));

    نياپو_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    إذا (!نياپو_use_printf())
	نياپو_clr_eos();
    p_وڌيڪ = save_p_وڌيڪ;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    فراغ
سيٽ_keep_نياپو(char_u *s, صحيح attr)
{
    vim_free(keep_نياپو);
    إذا (s != عدم && نياپو_silent == 0)
	keep_نياپو = vim_strsave(s);
    وإلا
	keep_نياپو = عدم;
    keep_نياپو_وڌيڪ = FALSE;
    keep_نياپو_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    فراغ
سيٽ_keep_نياپو_from_hist(فراغ)
{
    إذا (keep_نياپو == عدم && آخري_نياپو_hist != عدم && نياپو_scrolled == 0
						      && (State & گهڻائي_عام))
	سيٽ_keep_نياپو(آخري_نياپو_hist->نياپو, آخري_نياپو_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    فراغ
نياپو_شروعات(فراغ)
{
    صحيح		did_return = FALSE;

    إذا (نياپو_صف < cmdline_صف)
	نياپو_صف = cmdline_صف;

    إذا (!نياپو_silent)
    {
	VIM_CLEAR(keep_نياپو);
	need_fileinfo = FALSE;
    }

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	نياپو_clr_eos();
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
    إذا (اندريون_echowindow)
    {
	إذا (popup_پيغام_win_visible()
		    && ((نياپو_کانو > 0 && (نياپو_scroll || !ڀريل_screen))
			|| اندريون_echowindow))
	{
	    win_T *wp = popup_get_پيغام_win();

	    // start a new line
	    curbuf = wp->w_بفر;
	    ml_append(wp->w_بفر->b_ml.ml_سٽ_ڳڻپ,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_بفر;
	}
	نياپو_کانو = 0;
    }
    وإلا
#جيڪڏهن_پورو
	إذا (!نياپو_scroll && ڀريل_screen)	// overwrite last message
    {
	نياپو_صف = cmdline_صف;
	نياپو_کانو =
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_ويڪر - 1 :
#جيڪڏهن_پورو
	    0;
    }
    وإلا إذا (نياپو_didout || اندريون_echowindow)
    {
	// start message on next line
	نياپو_putchar('\n');
	did_return = TRUE;
	إذا (exmode_سرگرم != EXMODE_عام)
	    cmdline_صف = نياپو_صف;
    }
    إذا (!نياپو_didany || سٽون_کاٻو < 0)
	نياپو_starthere();
    إذا (نياپو_silent == 0)
    {
	نياپو_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    إذا (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    فراغ
نياپو_starthere(فراغ)
{
    سٽون_کاٻو = cmdline_صف;
    نياپو_didany = FALSE;
}

    فراغ
نياپو_putchar(صحيح c)
{
    نياپو_putchar_attr(c, 0);
}

    فراغ
نياپو_putchar_attr(صحيح c, صحيح attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    إذا (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_ٻيو(c);
	buf[2] = K_ٽيون(c);
	buf[3] = NUL;
    }
    وإلا
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    نياپو_puts_attr((حرف *)buf, attr);
}

    فراغ
نياپو_outnum(ڊگهو n)
{
    حرف	buf[20];

    sprintf(buf, "%ld", n);
    نياپو_puts(buf);
}

    فراغ
نياپو_home_replace(char_u *fname)
{
    نياپو_home_replace_attr(fname, 0);
}

#إذا defined(FEAT_ڳوليو_ID)
    فراغ
نياپو_home_replace_hl(char_u *fname)
{
    نياپو_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#جيڪڏهن_پورو

    ساكن فراغ
نياپو_home_replace_attr(char_u *fname, صحيح attr)
{
    char_u	*name;

    name = home_replace_save(عدم, fname);
    إذا (name != عدم)
	نياپو_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    صحيح
نياپو_outtrans(char_u *str)
{
    أرجع نياپو_outtrans_attr(str, 0);
}

    صحيح
نياپو_outtrans_attr(char_u *str, صحيح attr)
{
    أرجع نياپو_outtrans_len_attr(str, (صحيح)STRLEN(str), attr);
}

    صحيح
نياپو_outtrans_len(char_u *str, صحيح len)
{
    أرجع نياپو_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
نياپو_outtrans_one(char_u *p, صحيح attr)
{
    صحيح		l;

    إذا (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	نياپو_outtrans_len_attr(p, l, attr);
	أرجع p + l;
    }
    نياپو_puts_attr((حرف *)transchar_byte_buf(عدم, *p), attr);
    أرجع p + 1;
}

    صحيح
نياپو_outtrans_len_attr(char_u *msgstr, صحيح len, صحيح attr)
{
    صحيح		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_شروعات = msgstr;
    char_u	*s;
    صحيح		mb_l;
    صحيح		c;
    صحيح		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    إذا (attr == 0)
	attr = HL_ATTR(HLF_نياپو);

    // if MSG_HIST flag set, add message to history
    إذا (attr & نياپو_HIST)
    {
	جوڙيو_نياپو_hist(str, len, attr);
	attr &= ~نياپو_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    إذا (نياپو_silent == 0 && len > 0 && نياپو_صف >= cmdline_صف && نياپو_کانو == 0)
    {
	clear_cmdline = FALSE;
	گهڻائي_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    إذا (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	نياپو_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    طالما (--len >= 0 && !got_int)
    {
	إذا (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	وإلا إذا (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	وإلا
	    mb_l = 1;
	إذا (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    إذا (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    وإلا
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		إذا (str > plain_شروعات)
		    نياپو_puts_attr_len((حرف *)plain_شروعات,
					       (صحيح)(str - plain_شروعات), attr);
		plain_شروعات = str + mb_l;
		نياپو_puts_attr((حرف *)transchar_buf(عدم, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	وإلا
	{
	    s = transchar_byte_buf(عدم, *str);
	    إذا (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		إذا (str > plain_شروعات)
		    نياپو_puts_attr_len((حرف *)plain_شروعات,
					       (صحيح)(str - plain_شروعات), attr);
		plain_شروعات = str + 1;
		نياپو_puts_attr((حرف *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (صحيح)STRLEN(s);
	    }
	    وإلا
		++retval;
	    ++str;
	}
    }

    إذا (str > plain_شروعات && !got_int)
	// print the printable chars at the end
	نياپو_puts_attr_len((حرف *)plain_شروعات, (صحيح)(str - plain_شروعات), attr);

    got_int |= save_got_int;

    أرجع retval;
}

#إذا defined(FEAT_QUICKFIX)
    فراغ
نياپو_make(char_u *arg)
{
    صحيح	    i;
    ساكن char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    لكل (i = 5; *arg && i >= 0; --i)
	إذا (*arg++ != str[i])
	    اكسر;
    إذا (i < 0)
    {
	نياپو_putchar('\n');
	لكل (i = 0; rs[i]; ++i)
	    نياپو_putchar(rs[i] - 3);
    }
}
#جيڪڏهن_پورو

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
    صحيح
نياپو_outtrans_special(
    char_u	*strstart,
    صحيح		from,	// TRUE for lhs of a mapping
    صحيح		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    صحيح		retval = 0;
    حرف	*متن;
    صحيح		attr;
    صحيح		len;

    attr = HL_ATTR(HLF_8);
    طالما (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	إذا ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    متن = "<Space>";
	    ++str;
	}
	وإلا
	    متن = (حرف *)str2special(&str, from, FALSE);
	إذا (متن[0] != NUL && متن[1] == NUL)
	    // single-byte character or illegal byte
	    متن = (حرف *)transchar_byte_buf(عدم, (char_u)متن[0]);
	len = vim_strsize((char_u *)متن);
	إذا (maxlen > 0 && retval + len >= maxlen)
	    اكسر;
	// Highlight special keys
	نياپو_puts_attr(متن, len > 1
		&& (*mb_ptr2len)((char_u *)متن) <= 1 ? attr : 0);
	retval += len;
    }
    أرجع retval;
}

#إذا defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    صحيح	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    صحيح	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    طالما (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    أرجع (char_u *)ga.ga_معلومات;
}
#جيڪڏهن_پورو

/*
 * Return the printable string for the key codes at "*sp".
 * On illegal byte return a string with only that byte.
 * Used for translating the lhs or rhs of a mapping to printable chars.
 * Advances "sp" to the next code.
 */
    char_u *
str2special(
    char_u	**sp,
    صحيح		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    صحيح		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    صحيح			c;
    ساكن char_u	buf[7];
    char_u		*str = *sp;
    صحيح			modifiers = 0;
    صحيح			special = FALSE;

    إذا (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	إذا (p != عدم)
	    أرجع p;
    }

    c = *str;
    إذا ((c == K_SPECIAL
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		|| c == CSI
#جيڪڏهن_پورو
	) && str[1] != NUL && str[2] != NUL)
    {
	إذا (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	إذا ((c == K_SPECIAL
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		    || c == CSI
#جيڪڏهن_پورو
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = منزل_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	إذا (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    إذا (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	إذا (p != عدم)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	وإلا
	    // illegal byte
	    *sp = str + 1;
    }
    وإلا
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    إذا (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	أرجع get_special_ڪنجي_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    أرجع buf;
}

/*
 * Translate a key sequence into special key names.
 */
    فراغ
str2specialbuf(char_u *sp, char_u *buf, صحيح len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    طالما (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	إذا (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	وإلا
	    اكسر;
    }
}

/*
 * print line for :print or :list command
 */
    فراغ
نياپو_prt_سٽ(char_u *s, صحيح list)
{
    صحيح		c;
    صحيح		کانو = 0;
    صحيح		n_extra = 0;
    صحيح		c_extra = 0;
    صحيح		c_final = 0;
    char_u	*p_extra = عدم;	    // init to make SASC shut up
    صحيح		n;
    صحيح		attr = 0;
    char_u	*trail = عدم;
    char_u	*lead = عدم;
    صحيح		اندريون_multispace = FALSE;
    صحيح		multispace_pos = 0;
    صحيح		l;
    char_u	buf[MB_MAXBYTES + 1];

    إذا (curwin->w_p_list)
	list = TRUE;

    إذا (list)
    {
	// find start of trailing whitespace
	إذا (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    طالما (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	إذا (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != عدم
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    طالما (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    إذا (*lead == NUL)
		lead = عدم;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    إذا (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	نياپو_putchar(' ');

    طالما (!got_int)
    {
	إذا (n_extra > 0)
	{
	    --n_extra;
	    إذا (n_extra == 0 && c_final)
		c = c_final;
	    وإلا إذا (c_extra)
		c = c_extra;
	    وإلا
		c = *p_extra++;
	}
	وإلا إذا (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    کانو += (*mb_ptr2cells)(s);
	    إذا (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    وإلا إذا (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		صحيح len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    وإلا
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    نياپو_puts((حرف *)buf);
	    s += l;
	    استمر;
	}
	وإلا
	{
	    attr = 0;
	    c = *s++;
	    إذا (list)
	    {
		اندريون_multispace = c == ' ' && (*s == ' '
						 || (کانو > 0 && s[-2] == ' '));
		إذا (!اندريون_multispace)
		    multispace_pos = 0;
	    }
	    إذا (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
		n_extra = tabstop_padding(کانو, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#وإلا
		n_extra = curbuf->b_p_ts - کانو % curbuf->b_p_ts - 1;
#جيڪڏهن_پورو
		إذا (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		وإلا
		{
		    صحيح lcs_tab1 = curwin->w_lcs_chars.tab1;
		    صحيح lcs_tab2 = curwin->w_lcs_chars.tab2;
		    صحيح lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    إذا (lead != عدم && s <= lead
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
	    وإلا إذا (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    وإلا إذا (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    وإلا إذا (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(عدم, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    وإلا إذا (c == ' ')
	    {
		إذا (lead != عدم && s <= lead && اندريون_multispace
			&& curwin->w_lcs_chars.leadmultispace != عدم)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    إذا (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		وإلا إذا (lead != عدم && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		وإلا إذا (trail != عدم && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		وإلا إذا (اندريون_multispace
			&& curwin->w_lcs_chars.multispace != عدم)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    إذا (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		وإلا إذا (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	إذا (c == NUL)
	    اكسر;

	نياپو_putchar_attr(c, attr);
	کانو++;
    }
    نياپو_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    ساكن char_u *
screen_puts_mbyte(char_u *s, صحيح l, صحيح attr)
{
    صحيح		cw;

    نياپو_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    إذا (cw > 1 && (
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
		cmdmsg_rl ? نياپو_کانو <= 1 :
#جيڪڏهن_پورو
		نياپو_کانو == cmdline_ويڪر - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	نياپو_screen_putchar('>', HL_ATTR(HLF_AT));
	أرجع s;
    }

    screen_puts_len(s, l, نياپو_صف, cmdline_کانو_off + نياپو_کانو, attr);
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (cmdmsg_rl)
    {
	نياپو_کانو -= cw;
	إذا (نياپو_کانو == 0)
	{
	    نياپو_کانو = cmdline_ويڪر;
	    ++نياپو_صف;
	}
    }
    وإلا
#جيڪڏهن_پورو
    {
	نياپو_کانو += cw;
	إذا (نياپو_کانو >= cmdline_ويڪر)
	{
	    نياپو_کانو = 0;
	    ++نياپو_صف;
	}
    }
    أرجع s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    فراغ
نياپو_puts(حرف *s)
{
    نياپو_puts_attr(s, 0);
}

    فراغ
نياپو_puts_title(حرف *s)
{
    نياپو_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    ساكن فراغ
نياپو_outtrans_long_len_attr(char_u *longstr, صحيح len, صحيح attr)
{
    صحيح		slen = len;
    صحيح		گنجائش;

    گنجائش = cmdline_ويڪر - نياپو_کانو;
    إذا (len > گنجائش && گنجائش >= 20)
    {
	slen = (گنجائش - 3) / 2;
	نياپو_outtrans_len_attr(longstr, slen, attr);
	نياپو_puts_attr("...", HL_ATTR(HLF_8));
    }
    نياپو_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    فراغ
نياپو_outtrans_long_attr(char_u *longstr, صحيح attr)
{
    نياپو_outtrans_long_len_attr(longstr, (صحيح)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    فراغ
نياپو_puts_attr(حرف *s, صحيح attr)
{
    نياپو_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    ساكن فراغ
نياپو_puts_attr_len(حرف *str, صحيح maxlen, صحيح attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    إذا (نياپو_silent != 0)
	أرجع;

    إذا (attr == 0)
	attr = HL_ATTR(HLF_نياپو);

    // if MSG_HIST flag set, add message to history
    إذا ((attr & نياپو_HIST) && maxlen < 0)
    {
	جوڙيو_نياپو_hist((char_u *)str, -1, attr);
	attr &= ~نياپو_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    إذا (نياپو_scrolled != 0 && !نياپو_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    نياپو_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    إذا (نياپو_use_printf())
	نياپو_puts_printf((char_u *)str, maxlen);
    وإلا
	نياپو_puts_display((char_u *)str, maxlen, attr, FALSE);

    need_fileinfo = FALSE;
}

// values for "where"
#وصف_ڪريو PUT_APPEND 0		// append to "lnum"
#وصف_ڪريو PUT_TRUNC 1		// replace "lnum"
#وصف_ڪريو PUT_هيٺ 2		// add below "lnum"
				//
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
/*
 * Put text "t_s" until "end" in the message window.
 * "where" specifies where to put the text.
 */
    ساكن فراغ
put_نياپو_win(win_T *wp, صحيح where, char_u *t_s, char_u *پڇاڙي, linenr_T lnum)
{
    char_u  *p;

    إذا (where == PUT_هيٺ)
    {
	إذا (*پڇاڙي != NUL)
	{
	    p = vim_strnsave(t_s, پڇاڙي - t_s);
	    إذا (p == عدم)
		أرجع;
	}
	وإلا
	    p = t_s;
	ml_append_buf(wp->w_بفر, lnum, p, (colnr_T)0, FALSE);
	إذا (p != t_s)
	    vim_free(p);
    }
    وإلا
    {
	char_u *newp;

	curbuf = wp->w_بفر;
	إذا (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    إذا (newp == عدم)
		أرجع;
	    إذا (*پڇاڙي != NUL)
		newp[STRLEN(ml_get(lnum)) + (پڇاڙي - t_s)] = NUL;
	}
	وإلا
	{
	    newp = vim_strnsave(t_s, پڇاڙي - t_s);
	    إذا (newp == عدم)
		أرجع;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_بفر;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    نياپو_کانو += (صحيح)(پڇاڙي - t_s);
}
#جيڪڏهن_پورو

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    ساكن فراغ
نياپو_puts_display(
    char_u	*str,
    صحيح		maxlen,
    صحيح		attr,
    صحيح		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    صحيح		t_کانو = 0;	// screen cells todo, 0 when "t_s" not used
    صحيح		l;
    صحيح		cw;
    char_u	*sb_str = str;
    صحيح		sb_کانو = نياپو_کانو;
    صحيح		wrap;
    صحيح		did_آخري_char;
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
    صحيح		where = PUT_APPEND;
    win_T	*نياپو_win = عدم;
    linenr_T    lnum = 1;

    إذا (اندريون_echowindow)
    {
	نياپو_win = popup_get_پيغام_win();

	إذا (نياپو_win != عدم)
	{
	    إذا (!popup_پيغام_win_visible())
	    {
		إذا (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    نياپو_win = عدم;
		}
		وإلا
		{
		    // currently hidden, make it empty
		    curbuf = نياپو_win->w_بفر;
		    طالما ((curbuf->b_ml.ml_flags & ML_خالي_جايون) == 0)
			ml_delete(1);
		    curbuf = curwin->w_بفر;
		}
	    }
	    وإلا
	    {
		lnum = نياپو_win->w_بفر->b_ml.ml_سٽ_ڳڻپ;
		إذا (نياپو_کانو == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#جيڪڏهن_پورو

    did_wait_return = FALSE;
    طالما ((maxlen < 0 || (صحيح)(s - str) < maxlen) && *s != NUL)
    {
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	// For echowindow, use full width; for regular messages, leave last column
	صحيح wrap_کانو = نياپو_win != عدم ? cmdline_ويڪر : cmdline_ويڪر - 1;
#وإلا
	صحيح wrap_کانو = cmdline_ويڪر - 1;
#جيڪڏهن_پورو

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	إذا (!recurse && نياپو_صف >= صفون - 1 && (*s == '\n' || (
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			نياپو_کانو <= 1
		      || (*s == TAB && نياپو_کانو <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && نياپو_کانو <= 2))
		    :
#جيڪڏهن_پورو
		      ((*s != '\r' && نياپو_کانو + t_کانو >= wrap_کانو)
		       || (*s == TAB && نياپو_کانو + t_کانو
			   >= (wrap_کانو & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && نياپو_کانو + t_کانو >= wrap_کانو - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    إذا (t_کانو > 0)
	    {
		// output postponed text
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
		إذا (نياپو_win != عدم)
		{
		    put_نياپو_win(نياپو_win, where, t_s, s, lnum);
		    إذا (where == PUT_هيٺ)
			++lnum;
		    t_کانو = 0;
		    where = PUT_هيٺ;
		}
		وإلا
#جيڪڏهن_پورو
		    t_puts(&t_کانو, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    إذا (نياپو_no_وڌيڪ && سٽون_کاٻو == 0)
		اكسر;

#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    إذا (نياپو_win == عدم)
#جيڪڏهن_پورو
		// Scroll the screen up one line.
		نياپو_scroll_مٿي();

#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    إذا (نياپو_win == عدم)
	    {
#جيڪڏهن_پورو
		نياپو_صف = صفون - 2;
		إذا (نياپو_کانو >= cmdline_ويڪر)  // can happen after screen resize
		    نياپو_کانو = cmdline_ويڪر - 1;
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    }
#جيڪڏهن_پورو

	    // Display char in last column before showing more-prompt.
	    إذا (*s >= ' '
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#جيڪڏهن_پورو
	       )
	    {
		إذا (has_mbyte)
		{
		    إذا (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (صحيح)((str + maxlen) - s));
		    وإلا
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		وإلا
		    نياپو_screen_putchar(*s++, attr);
		did_آخري_char = TRUE;
	    }
	    وإلا
		did_آخري_char = FALSE;

	    إذا (p_وڌيڪ)
		// store text for scrolling back
		store_sb_متن(&sb_str, s, attr, &sb_کانو, TRUE);

#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    إذا (نياپو_win == عدم)
	    {
#جيڪڏهن_پورو
		inc_نياپو_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		إذا (cmdline_صف > 0 && !exmode_سرگرم)
		    --cmdline_صف;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		إذا (سٽون_کاٻو > 0)
		    --سٽون_کاٻو;
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    }
#جيڪڏهن_پورو
	    إذا (p_وڌيڪ && سٽون_کاٻو == 0 && State != گهڻائي_HITRETURN
					    && !نياپو_no_وڌيڪ && !exmode_سرگرم)
	    {
#جيڪڏهن_وصف_ٿيل FEAT_CON_DIALOG
		إذا (do_وڌيڪ_prompt(NUL))
		    s = confirm_نياپو_پڇ;
#وإلا
		(فراغ)do_وڌيڪ_prompt(NUL);
#جيڪڏهن_پورو
		إذا (quit_وڌيڪ)
		    أرجع;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    إذا (did_آخري_char)
		استمر;
	}

	wrap = *s == '\n'
		    || نياپو_کانو + t_کانو >= wrap_کانو
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && نياپو_کانو + t_کانو >= wrap_کانو - 1);
	إذا (t_کانو > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    إذا (نياپو_win != عدم)
	    {
		put_نياپو_win(نياپو_win, where, t_s, s, lnum);
		إذا (where == PUT_هيٺ)
		    ++lnum;
		t_کانو = 0;
		where = PUT_هيٺ;
		// Reset msg_col after outputting to new line in echowindow
		إذا (wrap)
		    نياپو_کانو = 0;
	    }
	    وإلا
#جيڪڏهن_پورو
		t_puts(&t_کانو, t_s, s, attr);
	}

	إذا (wrap && p_وڌيڪ && !recurse)
	    // store text for scrolling back
	    store_sb_متن(&sb_str, s, attr, &sb_کانو, TRUE);

	إذا (*s == '\n')		    // go to next line
	{
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    إذا (نياپو_win != عدم)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		إذا ((نياپو_win->w_بفر->b_ml.ml_flags & ML_خالي_جايون) == 0)
		{
		    put_نياپو_win(نياپو_win, PUT_هيٺ, t_s, t_s, lnum);
		    ++lnum;
		}
		نياپو_کانو = 0;  // Reset column for new line
	    }
	    وإلا
#جيڪڏهن_پورو
		نياپو_didout = FALSE;	    // remember that line is empty
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	    إذا (cmdmsg_rl)
		نياپو_کانو = cmdline_ويڪر - 1;
	    وإلا
#جيڪڏهن_پورو
		نياپو_کانو = 0;
	    إذا (++نياپو_صف >= صفون)  // safety check
		نياپو_صف = صفون - 1;
	}
	وإلا إذا (*s == '\r')	    // go to column 0
	{
	    نياپو_کانو = 0;
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    where = PUT_TRUNC;
#جيڪڏهن_پورو
	}
	وإلا إذا (*s == '\b')	    // go to previous char
	{
	    إذا (نياپو_کانو)
		--نياپو_کانو;
	}
	وإلا إذا (*s == TAB)	    // translate Tab into spaces
	{
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    إذا (نياپو_win != عدم)
		نياپو_کانو = (نياپو_کانو + 7) % 8;
	    وإلا
#جيڪڏهن_پورو
		افعل
		    نياپو_screen_putchar(' ', attr);
		طالما (نياپو_کانو & 7);
	}
	وإلا إذا (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	وإلا
	{
	    إذا (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		إذا (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (صحيح)((str + maxlen) - s));
		وإلا
		    l = (*mb_ptr2len)(s);
	    }
	    وإلا
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    إذا (
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#جيڪڏهن_پورو
		    (cw > 1 && نياپو_کانو + t_کانو >= wrap_کانو))
	    {
		إذا (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		وإلا
		    نياپو_screen_putchar(*s, attr);
	    }
	    وإلا
	    {
		// postpone this character until later
		إذا (t_کانو == 0)
		    t_s = s;
		t_کانو += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    إذا (t_کانو > 0)
    {
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	إذا (نياپو_win != عدم)
	    put_نياپو_win(نياپو_win, where, t_s, s, lnum);
	وإلا
#جيڪڏهن_پورو
	    t_puts(&t_کانو, t_s, s, attr);
    }

#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
    إذا (نياپو_win != عدم)
	popup_show_پيغام_win();
#جيڪڏهن_پورو
    // Store the text for scroll back.
    إذا (p_وڌيڪ && !recurse)
	store_sb_متن(&sb_str, s, attr, &sb_کانو, FALSE);

    نياپو_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    صحيح
پيغام_filtered(char_u *نياپو)
{
    صحيح ميلاپ;

    إذا (cmdmod.cmod_filter_regmatch.regprog == عدم)
	أرجع FALSE;
    ميلاپ = vim_regexec(&cmdmod.cmod_filter_regmatch, نياپو, (colnr_T)0);
    أرجع cmdmod.cmod_filter_force ? ميلاپ : !ميلاپ;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    ساكن فراغ
نياپو_scroll_مٿي(فراغ)
{
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
    إذا (اندريون_echowindow)
	أرجع;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    إذا (gui.اندريون_use)
	gui_undraw_cursor();
#جيڪڏهن_پورو
    // scrolling up always works
    mch_disable_flush();
    screen_del_سٽون(0, 0, 1, (صحيح)صفون, TRUE, 0, عدم);
    mch_enable_flush();

    إذا (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((صحيح)صفون - 1, (صحيح)صفون, cmdline_کانو_off,
		cmdline_کانو_off + cmdline_ويڪر, ' ', ' ', HL_ATTR(HLF_نياپو));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	إذا (ScreenAttrs[سٽ_وٿي[صفون - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((صحيح)صفون - 2, (صحيح)صفون - 1,
				 (صحيح)Columns - 1, (صحيح)Columns,
				 ' ', ' ', HL_ATTR(HLF_نياپو));
    }
}

/*
 * Increment "msg_scrolled".
 */
    ساكن فراغ
inc_نياپو_scrolled(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = عدم;

	// v:scrollstart is empty, set it to the script/function name and line
	//عدد
	إذا (p.string == عدم)
	{
	    p.string = (char_u *)_("Unknown");
	    p.ڊيگهائي = STRLEN(p.string);
	}
	وإلا
	{
	    size_t  tofreesize;

	    tofreesize = (صحيح)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    إذا (tofree != عدم)
	    {
		p.ڊيگهائي = vim_snprintf_safelen((حرف *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (ڊگهو)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	سيٽ_vim_var_string(VV_SCROLLSTART, p.string, (صحيح)p.ڊيگهائي);
	vim_free(tofree);
    }
#جيڪڏهن_پورو
    ++نياپو_scrolled;
    سيٽ_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
تعريفنوع بنية msgchunk_S msgchunk_T;
بنية msgchunk_S
{
    msgchunk_T	*sb_ايندڙ;
    msgchunk_T	*sb_prev;
    حرف	sb_eol;		// TRUE when line ends after this text
    صحيح		sb_نياپو_کانو;	// column in which text starts
    صحيح		sb_attr;	// text attributes
    char_u	sb_متن[1];	// text to be displayed, actually longer
};

ساكن msgchunk_T *آخري_msgchunk = عدم; // last displayed text

ساكن msgchunk_T *نياپو_sb_شروعات(msgchunk_T *mps);

تعريفنوع تعداد {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_پورو
} sb_clear_T;

// When to clear text on next msg.
ساكن sb_clear_T do_clear_sb_متن = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    ساكن فراغ
store_sb_متن(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    صحيح		attr,
    صحيح		*sb_کانو,
    صحيح		پڄاڻي)		// line ends
{
    msgchunk_T	*mp;

    إذا (do_clear_sb_متن == SB_CLEAR_ALL
	    || do_clear_sb_متن == SB_CLEAR_CMDLINE_پورو)
    {
	clear_sb_متن(do_clear_sb_متن == SB_CLEAR_ALL);
	نياپو_sb_eol();  // prevent messages from overlapping
	do_clear_sb_متن = SB_CLEAR_NONE;
    }

    إذا (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_متن) + (s - *sb_str) + 1);
	إذا (mp != عدم)
	{
	    mp->sb_eol = پڄاڻي;
	    mp->sb_نياپو_کانو = *sb_کانو;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_متن, *sb_str, s - *sb_str);

	    إذا (آخري_msgchunk == عدم)
	    {
		آخري_msgchunk = mp;
		mp->sb_prev = عدم;
	    }
	    وإلا
	    {
		mp->sb_prev = آخري_msgchunk;
		آخري_msgchunk->sb_ايندڙ = mp;
		آخري_msgchunk = mp;
	    }
	    mp->sb_ايندڙ = عدم;
	}
    }
    وإلا إذا (پڄاڻي && آخري_msgchunk != عدم)
	آخري_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_کانو = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    فراغ
may_clear_sb_متن(فراغ)
{
    do_clear_sb_متن = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    فراغ
sb_متن_شروعات_cmdline(فراغ)
{
    إذا (do_clear_sb_متن == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_متن_restart_cmdline();
    وإلا
    {
	نياپو_sb_eol();
	do_clear_sb_متن = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    فراغ
sb_متن_restart_cmdline(فراغ)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_متن = SB_CLEAR_CMDLINE_BUSY;

    إذا (آخري_msgchunk == عدم || آخري_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	أرجع;

    tofree = نياپو_sb_شروعات(آخري_msgchunk);
    آخري_msgchunk = tofree->sb_prev;
    إذا (آخري_msgchunk != عدم)
	آخري_msgchunk->sb_ايندڙ = عدم;
    طالما (tofree != عدم)
    {
	msgchunk_T *tofree_ايندڙ = tofree->sb_ايندڙ;

	vim_free(tofree);
	tofree = tofree_ايندڙ;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    فراغ
sb_متن_پڇاڙي_cmdline(فراغ)
{
    do_clear_sb_متن = SB_CLEAR_CMDLINE_پورو;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    فراغ
clear_sb_متن(صحيح all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    إذا (all)
	lastp = &آخري_msgchunk;
    وإلا
    {
	إذا (آخري_msgchunk == عدم)
	    أرجع;
	lastp = &نياپو_sb_شروعات(آخري_msgchunk)->sb_prev;
    }

    طالما (*lastp != عدم)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    فراغ
show_sb_متن(فراغ)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = نياپو_sb_شروعات(آخري_msgchunk);
    إذا (mp == عدم || mp->sb_prev == عدم)
	vim_beep(BO_MESS);
    وإلا
    {
	do_وڌيڪ_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    ساكن msgchunk_T *
نياپو_sb_شروعات(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    طالما (mp != عدم && mp->sb_prev != عدم && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    أرجع mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    فراغ
نياپو_sb_eol(فراغ)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    إذا (آخري_msgchunk != عدم
	    && !(آخري_msgchunk->sb_متن[0] == '\n'
		&& آخري_msgchunk->sb_متن[1] == NUL))
	آخري_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    ساكن msgchunk_T *
disp_sb_سٽ(صحيح صف, msgchunk_T *smp, صحيح clear_منزل_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    لكل (;;)
    {
	نياپو_صف = صف;
	نياپو_کانو = mp->sb_نياپو_کانو;
	p = mp->sb_متن;
	إذا (*p == '\n')	    // don't display the line break
	    ++p;
	نياپو_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	إذا (clear_منزل_eol)
	    screen_fill(صف, صف + 1,
		    cmdline_کانو_off + نياپو_کانو, cmdline_کانو_off + cmdline_ويڪر,
		    ' ', ' ', HL_ATTR(HLF_نياپو));

	إذا (mp->sb_eol || mp->sb_ايندڙ == عدم)
	    اكسر;
	mp = mp->sb_ايندڙ;
    }
    أرجع mp->sb_ايندڙ;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    ساكن فراغ
t_puts(
    صحيح		*t_کانو,
    char_u	*t_s,
    char_u	*s,
    صحيح		attr)
{
    // output postponed text
    نياپو_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (صحيح)(s - t_s), نياپو_صف, cmdline_کانو_off + نياپو_کانو,
	    attr);
    نياپو_کانو += *t_کانو;
    *t_کانو = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    إذا (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--نياپو_کانو;
    إذا (نياپو_کانو >= cmdline_ويڪر)
    {
	نياپو_کانو = 0;
	++نياپو_صف;
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
    صحيح
نياپو_use_printf(فراغ)
{
    أرجع (!نياپو_check_screen()
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.اندريون_use && !termcap_سرگرم)
# وإلا
	    || !termcap_سرگرم
# endif
#جيڪڏهن_پورو
	    || (swapping_screen() && !termcap_سرگرم)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    ساكن فراغ
نياپو_puts_printf(char_u *str, صحيح maxlen)
{
    char_u	*s = str;
    char_u	*buf = عدم;
    char_u	*p = s;

#جيڪڏهن_وصف_ٿيل MSWIN
    إذا (!(silent_گهڻائي && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#جيڪڏهن_پورو
    طالما ((maxlen < 0 || (صحيح)(s - str) < maxlen) && *s != NUL)
    {
	إذا (!(silent_گهڻائي && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    إذا (*s == NL)
	    {
		صحيح n = (صحيح)(s - p);

		buf = alloc(n + 3);
		إذا (buf != عدم)
		{
		    يادگيري_نقل(buf, p, n);
		    إذا (!info_پيغام)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    إذا (info_پيغام)   // informative message, not an error
			mch_نياپو((حرف *)buf);
		    وإلا
			mch_errmsg((حرف *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (cmdmsg_rl)
	{
	    إذا (*s == CAR || *s == NL)
		نياپو_کانو = cmdline_ويڪر - 1;
	    وإلا
		--نياپو_کانو;
	}
	وإلا
#جيڪڏهن_پورو
	{
	    إذا (*s == CAR || *s == NL)
		نياپو_کانو = 0;
	    وإلا
		++نياپو_کانو;
	}
	++s;
    }

    إذا (*p != NUL && !(silent_گهڻائي && p_verbose == 0))
    {
	char_u *tofree = عدم;

	إذا (maxlen > 0 && vim_strlen_maxlen((حرف *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	إذا (p != عدم)
	{
	    إذا (info_پيغام)
		mch_نياپو((حرف *)p);
	    وإلا
		mch_errmsg((حرف *)p);
	    vim_free(tofree);
	}
    }

    نياپو_didout = TRUE;	    // assume that line is not empty

#جيڪڏهن_وصف_ٿيل MSWIN
    إذا (!(silent_گهڻائي && p_verbose == 0))
	mch_settmode(TMODE_RAW);
#جيڪڏهن_پورو
}

/*
 * Show the more-prompt and handle the user response.
 * This takes care of scrolling back and displaying previously displayed text.
 * When at hit-enter prompt "typed_char" is the already typed character,
 * otherwise it's NUL.
 * Returns TRUE when jumping ahead to "confirm_msg_tail".
 */
    ساكن صحيح
do_وڌيڪ_prompt(صحيح typed_char)
{
    ساكن صحيح	entered = FALSE;
    صحيح		استعمال_ٿيل_typed_char = typed_char;
    صحيح		oldState = State;
    صحيح		c;
#جيڪڏهن_وصف_ٿيل FEAT_CON_DIALOG
    صحيح		retval = FALSE;
#جيڪڏهن_پورو
    صحيح		toscroll;
    msgchunk_T	*mp_آخري = عدم;
    msgchunk_T	*mp;
    صحيح		i;
    صحيح		نياپو_attr;

    نياپو_attr = HL_ATTR(HLF_نياپو);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    إذا (entered || (State == گهڻائي_HITRETURN && typed_char == 0))
	أرجع FALSE;
    entered = TRUE;

    إذا (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_آخري = نياپو_sb_شروعات(آخري_msgchunk);
	لكل (i = 0; i < صفون - 2 && mp_آخري != عدم
					     && mp_آخري->sb_prev != عدم; ++i)
	    mp_آخري = نياپو_sb_شروعات(mp_آخري->sb_prev);
    }

    State = گهڻائي_ASKMORE;
    setmouse();
    إذا (typed_char == NUL)
	نياپو_moremsg(FALSE);
    لكل (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	إذا (استعمال_ٿيل_typed_char != NUL)
	{
	    c = استعمال_ٿيل_typed_char;	// was typed at hit-enter prompt
	    استعمال_ٿيل_typed_char = NUL;
	}
	وإلا
	    c = get_keystroke();

#إذا defined(FEAT_MENU) && defined(FEAT_GUI)
	إذا (c == K_MENU)
	{
	    صحيح idx = get_menu_اشاريو(current_menu, گهڻائي_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    إذا (idx == MENU_اشاريو_INVALID)
		استمر;
	    c = *current_menu->لڙيون[idx];
	    إذا (c != NUL && current_menu->لڙيون[idx][1] != NUL)
		ins_typebuf(current_menu->لڙيون[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#جيڪڏهن_پورو

	toscroll = 0;
	بدل (c)
	{
	حالة BS:		// scroll one line back
	حالة K_BS:
	حالة 'k':
	حالة K_مٿي:
	    toscroll = -1;
	    اكسر;

	حالة CAR:		// one extra line
	حالة NL:
	حالة 'j':
	حالة K_هيٺ_ڏانهن:
	    toscroll = 1;
	    اكسر;

	حالة 'u':		// Up half a page
	    toscroll = -(صفون / 2);
	    اكسر;

	حالة 'd':		// Down half a page
	    toscroll = صفون / 2;
	    اكسر;

	حالة 'b':		// one page back
	حالة Ctrl_B:
	حالة K_PAGEUP:
	    toscroll = -(صفون - 1);
	    اكسر;

	حالة ' ':		// one extra page
	حالة 'f':
	حالة Ctrl_F:
	حالة K_PAGEDOWN:
	حالة K_LEFTMOUSE:
	    toscroll = صفون - 1;
	    اكسر;

	حالة 'g':		// all the way back to the start
	    toscroll = -999999;
	    اكسر;

	حالة 'G':		// all the way to the end
	    toscroll = 999999;
	    سٽون_کاٻو = 999999;
	    اكسر;

	حالة ':':		// start new command line
#جيڪڏهن_وصف_ٿيل FEAT_CON_DIALOG
	    إذا (!confirm_نياپو_استعمال_ٿيل)
#جيڪڏهن_پورو
	    {
		// Since got_int is set all typeahead will be flushed, but we
		// want to keep this ':', remember that in a special way.
		typeahead_noflush(':');
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
		skip_رڪن_loop = TRUE;
#جيڪڏهن_پورو
		cmdline_صف = صفون - 1;		// put ':' on this line
		skip_redraw = TRUE;		// skip redraw once
		need_wait_return = FALSE;	// don't wait in main()
	    }
	    // FALLTHROUGH
	حالة 'q':		// quit
	حالة Ctrl_C:
	حالة ESC:
#جيڪڏهن_وصف_ٿيل FEAT_CON_DIALOG
	    إذا (confirm_نياپو_استعمال_ٿيل)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    وإلا
#جيڪڏهن_پورو
	    {
		got_int = TRUE;
		quit_وڌيڪ = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    سٽون_کاٻو = صفون - 1;
	    اكسر;

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
	حالة Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    إذا (clip_تارو.state == SELECT_پورو)
		clip_copy_modeless_selection(TRUE);
	    استمر;
#جيڪڏهن_پورو
	افتراضي:		// no valid response
	    نياپو_moremsg(TRUE);
	    استمر;
	}

	إذا (toscroll != 0)
	{
	    إذا (toscroll < 0)
	    {
		// go to start of last line
		إذا (mp_آخري == عدم)
		    mp = نياپو_sb_شروعات(آخري_msgchunk);
		وإلا إذا (mp_آخري->sb_prev != عدم)
		    mp = نياپو_sb_شروعات(mp_آخري->sb_prev);
		وإلا
		    mp = عدم;

		// go to start of line at top of the screen
		لكل (i = 0; i < صفون - 2 && mp != عدم && mp->sb_prev != عدم;
									  ++i)
		    mp = نياپو_sb_شروعات(mp->sb_prev);

		إذا (mp != عدم && mp->sb_prev != عدم)
		{
		    // Find line to be displayed at top.
		    لكل (i = 0; i > toscroll; --i)
		    {
			إذا (mp == عدم || mp->sb_prev == عدم)
			    اكسر;
			mp = نياپو_sb_شروعات(mp->sb_prev);
			إذا (mp_آخري == عدم)
			    mp_آخري = نياپو_sb_شروعات(آخري_msgchunk);
			وإلا
			    mp_آخري = نياپو_sb_شروعات(mp_آخري->sb_prev);
		    }

		    إذا (toscroll == -1 && screen_ins_سٽون(0, 0, 1,
						     (صحيح)صفون, 0, عدم) == OK)
		    {
			// display line at top
			(فراغ)disp_sb_سٽ(0, mp, FALSE);
		    }
		    وإلا
		    {
			صحيح did_clear = screenclear();

			// redisplay all lines
			لكل (i = 0; mp != عدم && i < صفون - 1; ++i)
			{
			    mp = disp_sb_سٽ(i, mp, !did_clear);
			    ++نياپو_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    وإلا
	    {
		// First display any text that we scrolled back.
		طالما (toscroll > 0 && mp_آخري != عدم)
		{
		    // scroll up, display line at bottom
		    نياپو_scroll_مٿي();
		    inc_نياپو_scrolled();
		    screen_fill((صحيح)صفون - 2, (صحيح)صفون - 1,
			    cmdline_کانو_off, cmdline_کانو_off + cmdline_ويڪر,
			    ' ', ' ', نياپو_attr);
		    mp_آخري = disp_sb_سٽ((صحيح)صفون - 2, mp_آخري, FALSE);
		    --toscroll;
		}
	    }

	    إذا (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((صحيح)صفون - 1, (صحيح)صفون, cmdline_کانو_off,
			cmdline_کانو_off + cmdline_ويڪر, ' ', ' ', نياپو_attr);
		نياپو_moremsg(FALSE);
		استمر;
	    }

	    // display more text, return to caller
	    سٽون_کاٻو = toscroll;
	}

	اكسر;
    }

    // clear the --more-- message
    screen_fill((صحيح)صفون - 1, (صحيح)صفون, cmdline_کانو_off,
	    cmdline_کانو_off + cmdline_ويڪر, ' ', ' ', نياپو_attr);
    State = oldState;
    setmouse();
    إذا (quit_وڌيڪ)
    {
	نياپو_صف = صفون - 1;
	نياپو_کانو = 0;
    }
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    وإلا إذا (cmdmsg_rl)
	نياپو_کانو = cmdline_ويڪر - 1;
#جيڪڏهن_پورو

    entered = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_CON_DIALOG
    أرجع retval;
#وإلا
    أرجع FALSE;
#جيڪڏهن_پورو
}

#إذا defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_نياپو
#  undef mch_نياپو
# endif

# إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ساكن فراغ
mch_errmsg_c(حرف *str)
{
    DWORD   nwrite = 0;
    DWORD   گهڻائي = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    إذا (Get_Console_گهڻائي(h, &گهڻائي) && enc_codepage >= 0
	    && (صحيح)GetConsoleCP() != enc_codepage)
    {
	صحيح	len = (صحيح)STRLEN(str);
	WCHAR	*w = enc_منزل_utf16((char_u *)str, &len);

	إذا (w != عدم)
	{
	    WriteConsoleW(h, w, len, &nwrite, عدم);
	    vim_free(w);
	}
    }
    وإلا
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
    فراغ
mch_errmsg(حرف *str)
{
# إذا !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    صحيح		len;
# endif

# إذا (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    إذا (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   وإلا
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.اندريون_use || gui.starting)
#  endif
	    )
    {
	fprintf(stderr, "%s", str);
	أرجع;
    }
# endif

# إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    إذا (!(gui.اندريون_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	أرجع;
    }
# endif

# إذا !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (صحيح)STRLEN(str) + 1;
    إذا (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    إذا (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_معلومات + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_معلومات + error_ga.ga_len;
	    لكل (;;)
	    {
		p = vim_strchr(p, '\r');
		إذا (p == عدم)
		    اكسر;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ساكن فراغ
mch_نياپو_c(حرف *str)
{
    DWORD   nwrite = 0;
    DWORD   گهڻائي;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    إذا (Get_Console_گهڻائي(h, &گهڻائي) && enc_codepage >= 0
	    && (صحيح)GetConsoleCP() != enc_codepage)
    {
	صحيح	len = (صحيح)STRLEN(str);
	WCHAR	*w = enc_منزل_utf16((char_u *)str, &len);

	إذا (w != عدم)
	{
	    WriteConsoleW(h, w, len, &nwrite, عدم);
	    vim_free(w);
	}
    }
    وإلا
    {
	لکو("%s", str);
    }
}
# endif

/*
 * Give a message.  To be used when the screen hasn't been initialized yet.
 * When there is no tty, collect messages until the GUI has started and they
 * can be displayed in a message box.
 */
    فراغ
mch_نياپو(حرف *str)
{
# إذا (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    إذا (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   وإلا
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.اندريون_use || gui.starting)
#  endif
	    )
    {
	لکو("%s", str);
	أرجع;
    }
# endif

# إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    إذا (!(gui.اندريون_use || gui.starting))
#  endif
    {
	mch_نياپو_c(str);
	أرجع;
    }
# endif
# إذا !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#جيڪڏهن_پورو // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    ساكن فراغ
نياپو_screen_putchar(صحيح c, صحيح attr)
{
    نياپو_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, نياپو_صف, cmdline_کانو_off + نياپو_کانو, attr);
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (cmdmsg_rl)
    {
	إذا (--نياپو_کانو == 0)
	{
	    نياپو_کانو = cmdline_ويڪر;
	    ++نياپو_صف;
	}
    }
    وإلا
#جيڪڏهن_پورو
    {
	إذا (++نياپو_کانو >= cmdline_ويڪر)
	{
	    نياپو_کانو = 0;
	    ++نياپو_صف;
	}
    }
}

    ساكن فراغ
نياپو_moremsg(صحيح ڀريل)
{
    صحيح		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (صحيح)صفون - 1, cmdline_کانو_off, attr);
    إذا (ڀريل)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(صحيح)صفون - 1, cmdline_کانو_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    فراغ
repeat_پيغام(فراغ)
{
    إذا (State == گهڻائي_ASKMORE)
    {
	نياپو_moremsg(TRUE);	// display --more-- message again
	نياپو_صف = صفون - 1;
    }
#جيڪڏهن_وصف_ٿيل FEAT_CON_DIALOG
    وإلا إذا (State == گهڻائي_CONFIRM)
    {
	display_confirm_نياپو();	// display ":confirm" message again
	نياپو_صف = صفون - 1;
    }
#جيڪڏهن_پورو
    وإلا إذا (State == گهڻائي_EXTERNCMD)
    {
	windgoto(نياپو_صف, cmdline_کانو_off + نياپو_کانو); // put cursor back
    }
    وإلا إذا (State == گهڻائي_HITRETURN || State == گهڻائي_SETWSIZE)
    {
	إذا (نياپو_صف == صفون - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    نياپو_didout = FALSE;
	    نياپو_کانو = 0;
	    نياپو_clr_eos();
	}
	ڪاميابي_return_نياپو();
	نياپو_صف = صفون - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    ساكن صحيح
نياپو_check_screen(فراغ)
{
    إذا (!ڀريل_screen || !screen_valid(FALSE))
	أرجع FALSE;

    إذا (نياپو_صف >= صفون)
	نياپو_صف = صفون - 1;
    إذا (نياپو_کانو >= cmdline_ويڪر)
	نياپو_کانو = cmdline_ويڪر - 1;
    أرجع TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    فراغ
نياپو_clr_eos(فراغ)
{
    إذا (نياپو_silent == 0)
	نياپو_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    فراغ
نياپو_clr_eos_force(فراغ)
{
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
    إذا (اندريون_echowindow)
	أرجع;  // messages go into a popup
#جيڪڏهن_پورو
    إذا (نياپو_use_printf())
    {
	إذا (ڀريل_screen)	// only when termcap codes are valid
	{
	    إذا (*T_CD)
		ٻاهريون_str(T_CD);	// clear to end of display
	    وإلا إذا (*T_CE)
		ٻاهريون_str(T_CE);	// clear to end of line
	}
    }
    وإلا
    {
	صحيح نياپو_attr;

	نياپو_attr = HL_ATTR(HLF_نياپو);

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (cmdmsg_rl)
	{
	    screen_fill(نياپو_صف, نياپو_صف + 1,
		    cmdline_کانو_off, cmdline_کانو_off + نياپو_کانو + 1,
		    ' ', ' ', نياپو_attr);
	    screen_fill(نياپو_صف + 1, (صحيح)صفون,
		    cmdline_کانو_off, cmdline_کانو_off + cmdline_ويڪر,
		    ' ', ' ', نياپو_attr);
	}
	وإلا
#جيڪڏهن_پورو
	{
	    screen_fill(نياپو_صف, نياپو_صف + 1,
		    cmdline_کانو_off + نياپو_کانو, cmdline_کانو_off + cmdline_ويڪر,
		    ' ', ' ', نياپو_attr);
	    screen_fill(نياپو_صف + 1, (صحيح)صفون,
		    cmdline_کانو_off, cmdline_کانو_off + cmdline_ويڪر,
		    ' ', ' ', نياپو_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    فراغ
نياپو_clr_cmdline(فراغ)
{
    نياپو_صف = cmdline_صف;
    نياپو_کانو = 0;
    نياپو_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    صحيح
نياپو_پڇاڙي(فراغ)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    إذا (!exiting && need_wait_return && !(State & گهڻائي_CMDLINE))
    {
	wait_return(FALSE);
	أرجع FALSE;
    }
    ٻاهريون_flush();
    أرجع TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    فراغ
نياپو_check(فراغ)
{
    إذا (نياپو_صف == صفون - 1 && نياپو_کانو >= sc_کانو
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
		&& !اندريون_echowindow
#جيڪڏهن_پورو
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
    ساكن فراغ
redir_write(char_u *str, صحيح maxlen)
{
    char_u	*s = str;
    ساكن صحيح	cur_کانو = 0;

    // Don't do anything for displaying prompts and the like.
    إذا (redir_off)
	أرجع;

    // If 'verbosefile' is set prepare for writing in that file.
    إذا (*p_vfile != NUL && verbose_fd == عدم)
	verbose_open();

    إذا (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	إذا (*s != '\n' && *s != '\r')
	{
	    طالما (cur_کانو < نياپو_کانو)
	    {
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		إذا (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		وإلا إذا (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		وإلا إذا (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		وإلا
#جيڪڏهن_پورو
		    إذا (redir_fd != عدم)
		    fputs(" ", redir_fd);
		إذا (verbose_fd != عدم)
		    fputs(" ", verbose_fd);
		++cur_کانو;
	    }
	}

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (redir_execute)
	    execute_redir_str(s, maxlen);
	وإلا إذا (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	وإلا إذا (redir_vname)
	    var_redir_str(s, maxlen);
#جيڪڏهن_پورو

	// Write and adjust the current column.
	طالما (*s != NUL && (maxlen < 0 || (صحيح)(s - str) < maxlen))
	{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    إذا (!redir_reg && !redir_vname && !redir_execute)
#جيڪڏهن_پورو
		إذا (redir_fd != عدم)
		    putc(*s, redir_fd);
	    إذا (verbose_fd != عدم)
		putc(*s, verbose_fd);
	    إذا (*s == '\r' || *s == '\n')
		cur_کانو = 0;
	    وإلا إذا (*s == '\t')
		cur_کانو += (8 - cur_کانو % 8);
	    وإلا
		++cur_کانو;
	    ++s;
	}

	إذا (نياپو_silent != 0)	// should update msg_col
	    نياپو_کانو = cur_کانو;
    }
}

    صحيح
redirecting(فراغ)
{
    أرجع redir_fd != عدم || *p_vfile != NUL
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#جيڪڏهن_پورو
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    فراغ
verbose_enter(فراغ)
{
    إذا (*p_vfile != NUL)
	++نياپو_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    فراغ
verbose_leave(فراغ)
{
    إذا (*p_vfile != NUL)
	إذا (--نياپو_silent < 0)
	    نياپو_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    فراغ
verbose_enter_scroll(فراغ)
{
    إذا (*p_vfile != NUL)
	++نياپو_silent;
    وإلا
	// always scroll up, don't overwrite
	نياپو_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    فراغ
verbose_leave_scroll(فراغ)
{
    إذا (*p_vfile != NUL)
    {
	إذا (--نياپو_silent < 0)
	    نياپو_silent = 0;
    }
    وإلا
	cmdline_صف = نياپو_صف;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    فراغ
verbose_stop(فراغ)
{
    إذا (verbose_fd != عدم)
    {
	fclose(verbose_fd);
	verbose_fd = عدم;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    صحيح
verbose_open(فراغ)
{
    إذا (verbose_fd == عدم && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((حرف *)p_vfile, "a");
	إذا (verbose_fd == عدم)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    فراغ
give_warning(char_u *پيغام, صحيح hl)
{
    give_warning_with_ذريعو(پيغام, hl, FALSE);
}

    فراغ
give_warning_with_ذريعو(char_u *پيغام, صحيح hl, صحيح with_ذريعو)
{
    // Don't do this for ":silent".
    إذا (نياپو_silent != 0)
	أرجع;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    سيٽ_vim_var_string(VV_WARNINGMSG, پيغام, -1);
#جيڪڏهن_پورو
    VIM_CLEAR(keep_نياپو);
    إذا (hl)
	keep_نياپو_attr = HL_ATTR(HLF_W);
    وإلا
	keep_نياپو_attr = 0;

    إذا (with_ذريعو)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	نياپو_شروعات();
	نياپو_ذريعو(HL_ATTR(HLF_W));
	نياپو_puts(" ");
	نياپو_puts_attr((حرف *)پيغام, HL_ATTR(HLF_W) | نياپو_HIST);
	نياپو_clr_eos();
	(فراغ)نياپو_پڇاڙي();
    }
    وإلا إذا (نياپو_attr((حرف *)پيغام, keep_نياپو_attr) && نياپو_scrolled == 0)
	سيٽ_keep_نياپو(پيغام, keep_نياپو_attr);

    نياپو_didout = FALSE;	    // overwrite this message
    نياپو_nowait = TRUE;	    // don't wait for this message
    نياپو_کانو = 0;

    --no_wait_return;
}

#إذا defined(FEAT_EVAL)
    فراغ
give_warning2(char_u *پيغام, char_u *a1, صحيح hl)
{
    إذا (IObuff == عدم)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(پيغام, hl);
    }
    وإلا
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, (حرف *)پيغام, a1);
	give_warning(IObuff, hl);
    }
}
#جيڪڏهن_پورو

/*
 * Advance msg cursor to column "col".
 */
    فراغ
نياپو_advance(صحيح کانو)
{
    إذا (نياپو_silent != 0)	// nothing to advance to
    {
	نياپو_کانو = کانو;		// for redirection, may fill it up later
	أرجع;
    }
    إذا (کانو >= cmdline_ويڪر)	// not enough room
	کانو = cmdline_ويڪر - 1;
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (cmdmsg_rl)
	طالما (نياپو_کانو > cmdline_ويڪر - کانو)
	    نياپو_putchar(' ');
    وإلا
#جيڪڏهن_پورو
	طالما (نياپو_کانو < کانو)
	    نياپو_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    فراغ
نياپو_warn_missing_clipboard(فراغ)
{
    إذا (!global_busy && !did_warn_clipboard && silence_w23_w24_نياپو == 0)
    {
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
	نياپو(_("W23: Clipboard register not available, using register 0"));
#وإلا
	نياپو(_("W24: Clipboard register not available. See :h W24"));
#جيڪڏهن_پورو
	did_warn_clipboard = TRUE;
    }
}

#إذا defined(FEAT_CON_DIALOG)
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
    صحيح
do_dialog(
    صحيح		type اڻ_استعمال_ٿيل,
    char_u	*title اڻ_استعمال_ٿيل,
    char_u	*پيغام,
    char_u	*buttons,
    صحيح		dfltbutton,
    char_u	*textfield اڻ_استعمال_ٿيل,	// IObuff for inputdialog(), NULL
					// otherwise
    صحيح		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    صحيح		oldState;
    صحيح		retval = 0;
    char_u	*hotkeys;
    صحيح		c;
    صحيح		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    إذا (silent_گهڻائي)
	أرجع dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    إذا (gui.اندريون_use && vim_strchr(p_go, GO_CONDIALOG) == عدم)
    {
	// --gui-dialog-file: write text to a file
	إذا (gui_dialog_log(title, پيغام))
	    c = dfltbutton;
	وإلا
	    c = gui_mch_dialog(type, title, پيغام, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_صف = نياپو_صف;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	ٻاهريون_flush();
	gui_mch_update();

	أرجع c;
    }
# endif

    oldState = State;
    State = گهڻائي_CONFIRM;
    setmouse();

    // Ensure raw mode here.
    save_tmode = cur_tmode;
    settmode(TMODE_RAW);

    /*
     * Since we wait for a keypress, don't make the
     * user press RETURN as well afterwards.
     */
    ++no_wait_return;
    hotkeys = نياپو_show_console_dialog(پيغام, buttons, dfltbutton);

    إذا (hotkeys != عدم)
    {
	لكل (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    بدل (c)
	    {
	    حالة CAR:		// User accepts default option
	    حالة NL:
		retval = dfltbutton;
		اكسر;
	    حالة Ctrl_C:	// User aborts/cancels
	    حالة ESC:
		retval = 0;
		اكسر;
	    افتراضي:		// Could be a hotkey?
		إذا (c < 0)	// special keys are ignored here
		    استمر;
		إذا (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    اكسر;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		لكل (i = 0; hotkeys[i]; ++i)
		{
		    إذا (has_mbyte)
		    {
			إذا ((*mb_ptr2char)(hotkeys + i) == c)
			    اكسر;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    وإلا
			إذا (hotkeys[i] == c)
			    اكسر;
		    ++retval;
		}
		إذا (hotkeys[i])
		    اكسر;
		// No hotkey match, so keep waiting
		استمر;
	    }
	    اكسر;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    نياپو_پڇاڙي_prompt();

    أرجع retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    ساكن صحيح
copy_char(
    char_u	*from,
    char_u	*منزل,
    صحيح		lowercase)	// make character lower case
{
    صحيح		len;
    صحيح		c;

    إذا (has_mbyte)
    {
	إذا (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    أرجع (*mb_char2bytes)(c, منزل);
	}
	وإلا
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(منزل, from, (size_t)len);
	    أرجع len;
	}
    }
    وإلا
    {
	إذا (lowercase)
	    *منزل = (char_u)TOLOWER_LOC(*from);
	وإلا
	    *منزل = *from;
	أرجع 1;
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
    ساكن char_u *
نياپو_show_console_dialog(
    char_u	*پيغام,
    char_u	*buttons,
    صحيح		dfltbutton)
{
    صحيح		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    صحيح		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = عدم;
    char_u	*msgp = عدم;
    char_u	*hotkp = عدم;
    char_u	*r;
    صحيح		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    صحيح		پهريون_hotkey = FALSE;	// first char of button is hotkey
    صحيح		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    لكل (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	طالما (*r)
	{
	    إذا (*r == DLG_BUTTON_SEP)
	    {
		إذا (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    إذا (has_mbyte)
			hotkp += STRLEN(hotkp);
		    وإلا
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    إذا (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    إذا (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			پهريون_hotkey = TRUE;
		}
		وإلا
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    إذا (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    وإلا إذا (*r == DLG_HOTKEY_CHAR || پهريون_hotkey)
	    {
		إذا (*r == DLG_HOTKEY_CHAR)
		    ++r;
		پهريون_hotkey = FALSE;
		إذا (copy)
		{
		    إذا (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    وإلا
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		وإلا
		{
		    ++len;	    // '&a' -> '[a]'
		    إذا (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    وإلا
	    {
		// everything else copy literally
		إذا (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	إذا (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	وإلا
	{
	    len += (صحيح)(STRLEN(پيغام)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    إذا (!has_hotkey[0])
	    {
		پهريون_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_نياپو);
	    confirm_نياپو = alloc(len);
	    إذا (confirm_نياپو == عدم)
		أرجع عدم;
	    *confirm_نياپو = NUL;
	    hotk = alloc(lenhotkey);
	    إذا (hotk == عدم)
		أرجع عدم;

	    *confirm_نياپو = '\n';
	    STRCPY(confirm_نياپو + 1, پيغام);

	    msgp = confirm_نياپو + 1 + STRLEN(پيغام);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_نياپو_پڇ = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_نياپو();
    أرجع hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    ساكن فراغ
display_confirm_نياپو(فراغ)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_نياپو_استعمال_ٿيل;
    إذا (confirm_نياپو != عدم)
	نياپو_puts_attr((حرف *)confirm_نياپو, HL_ATTR(HLF_M));
    --confirm_نياپو_استعمال_ٿيل;
}

#جيڪڏهن_پورو // FEAT_CON_DIALOG

#إذا defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    صحيح
vim_dialog_yesno(
    صحيح		type,
    char_u	*title,
    char_u	*پيغام,
    صحيح		dflt)
{
    إذا (do_dialog(type,
		title == عدم ? (char_u *)_("Question") : title,
		پيغام,
		(char_u *)_("&Yes\n&No"), dflt, عدم, FALSE) == 1)
	أرجع VIM_YES;
    أرجع VIM_NO;
}

    صحيح
vim_dialog_yesnocancel(
    صحيح		type,
    char_u	*title,
    char_u	*پيغام,
    صحيح		dflt)
{
    بدل (do_dialog(type,
		title == عدم ? (char_u *)_("Question") : title,
		پيغام,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, عدم, FALSE))
    {
	حالة 1: أرجع VIM_YES;
	حالة 2: أرجع VIM_NO;
    }
    أرجع VIM_CANCEL;
}

    صحيح
vim_dialog_yesnoallcancel(
    صحيح		type,
    char_u	*title,
    char_u	*پيغام,
    صحيح		dflt)
{
    بدل (do_dialog(type,
		title == عدم ? (char_u *)"Question" : title,
		پيغام,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, عدم, FALSE))
    {
	حالة 1: أرجع VIM_YES;
	حالة 2: أرجع VIM_NO;
	حالة 3: أرجع VIM_ALL;
	حالة 4: أرجع VIM_DISCARDALL;
    }
    أرجع VIM_CANCEL;
}

#جيڪڏهن_پورو // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
