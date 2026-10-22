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

#تعریف_کرو پیغام_FILE		// don't include prototype for smsg()

#شامل "vim.h"

ساکن خالی جوڑو_پیغام_hist(char_u *s, صحیح len, صحیح attr);
ساکن خالی check_پیغام_hist(خالی);
ساکن خالی کامیابی_return_پیغام(خالی);
ساکن خالی پیغام_home_replace_attr(char_u *fname, صحیح attr);
ساکن خالی پیغام_puts_attr_len(حرف *str, صحیح maxlen, صحیح attr);
ساکن خالی پیغام_puts_display(char_u *str, صحیح maxlen, صحیح attr, صحیح recurse);
ساکن خالی پیغام_scroll_اوپر(خالی);
ساکن خالی inc_پیغام_scrolled(خالی);
ساکن خالی store_sb_متن(char_u **sb_str, char_u *s, صحیح attr, صحیح *sb_ستون, صحیح اختتام);
ساکن خالی t_puts(صحیح *t_ستون, char_u *t_s, char_u *s, صحیح attr);
ساکن خالی پیغام_puts_printf(char_u *str, صحیح maxlen);
ساکن صحیح do_مزید_prompt(صحیح typed_char);
ساکن خالی پیغام_screen_putchar(صحیح c, صحیح attr);
ساکن خالی پیغام_moremsg(صحیح بھرا);
ساکن صحیح  پیغام_check_screen(خالی);
ساکن خالی redir_write(char_u *s, صحیح maxlen);
#اگر_تعریف_شدہ FEAT_CON_DIALOG
ساکن char_u *پیغام_show_console_dialog(char_u *پیغام_2, char_u *buttons, صحیح dfltbutton);
ساکن صحیح	confirm_پیغام_مستعمل = FALSE;	// displaying confirm_msg
ساکن char_u	*confirm_پیغام = تہی;		// ":confirm" message
ساکن char_u	*confirm_پیغام_دم;		// tail of confirm_msg
ساکن خالی display_confirm_پیغام(خالی);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
ساکن صحیح emsg_مقصد_channel_log = FALSE;
#اگر_ختم

ساخت پیغام_hist
{
    ساخت پیغام_hist	*اگلا;
    char_u		*پیغام;
    صحیح			attr;
};

ساکن ساخت پیغام_hist *پہلا_پیغام_hist = تہی;
ساکن ساخت پیغام_hist *آخری_پیغام_hist = تہی;
ساکن صحیح پیغام_hist_len = 0;
ساکن صحیح پیغام_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#تعریف_کرو MESSAGES_کامیابی_ENTER	0x001
#تعریف_کرو MESSAGES_WAIT		0x002
#تعریف_کرو MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#تعریف_کرو MESSAGES_OPT_کامیابی_ENTER "hit-enter"
#تعریف_کرو MESSAGES_OPT_WAIT "wait:"
#تعریف_کرو MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
ساکن صحیح پیغام_flags = MESSAGES_کامیابی_ENTER | MESSAGES_HISTORY;
ساکن صحیح پیغام_wait = 0;

ساکن FILE *verbose_fd = تہی;
ساکن صحیح  verbose_did_open = FALSE;

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
    صحیح
پیغام(حرف *s)
{
    واپس پیغام_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    صحیح
verb_پیغام(حرف *s)
{
    صحیح		n;

    verbose_enter();
    n = پیغام_attr_keep(s, 0, FALSE);
    verbose_leave();

    واپس n;
}

    صحیح
پیغام_attr(حرف *s, صحیح attr)
{
    واپس پیغام_attr_keep(s, attr, FALSE);
}

    صحیح
پیغام_attr_keep(
    حرف	*s,
    صحیح		attr,
    صحیح		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    ساکن صحیح	entered = 0;
    صحیح		retval;
    char_u	*buf = تہی;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    اگر (!emsg_on_display && پیغام_filtered((char_u *)s))
	واپس TRUE;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (attr == 0)
	سیٹ_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#اگر_ختم

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    اگر (entered >= 3)
	واپس TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    اگر ((char_u *)s != keep_پیغام
	    || (*s != '<'
		&& آخری_پیغام_hist != تہی
		&& آخری_پیغام_hist->پیغام != تہی
		&& STRCMP(s, آخری_پیغام_hist->پیغام) != 0))
	جوڑو_پیغام_hist((char_u *)s, -1, attr);

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (emsg_مقصد_channel_log)
	// Write message in the channel log.
	ch_log(تہی, "ERROR: %s", s);
#اگر_ختم

    // Truncate the message if needed.
    پیغام_آغاز();
    buf = پیغام_strtrunc((char_u *)s, FALSE);
    اگر (buf != تہی)
	s = (حرف *)buf;

    پیغام_outtrans_attr((char_u *)s, attr);
    پیغام_clr_eos();
    retval = پیغام_انتہا();

    اگر (keep && retval && vim_strsize((char_u *)s)
		    < (صحیح)(صفیں - cmdline_صف - 1) * cmdline_چوڑائی + sc_ستون)
	سیٹ_keep_پیغام((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    واپس retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
پیغام_strtrunc(
    char_u	*s,
    صحیح		force)	    // always truncate
{
    char_u	*buf = تہی;
    صحیح		len;
    صحیح		گنجائش;

    // May truncate message to avoid a hit-return prompt
    اگر ((!پیغام_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_فعال && پیغام_silent == 0) || force)
    {
	len = vim_strsize(s);
	اگر (پیغام_scrolled != 0
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
		|| اندر_echowindow
#اگر_ختم
		)
	    // Use all the columns.
	    گنجائش = (صحیح)(صفیں - پیغام_صف) * cmdline_چوڑائی - 1;
	ورنہ
	    // Use up to 'showcmd' column.
	    گنجائش = (صحیح)(صفیں - پیغام_صف - 1) * cmdline_چوڑائی + sc_ستون - 1;
	اگر (len > گنجائش && گنجائش > 0)
	{
	    اگر (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (گنجائش + 2) * 18;
	    ورنہ اگر (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (گنجائش + 2) * 2;
	    ورنہ
		len = گنجائش + 2;
	    buf = alloc(len);
	    اگر (buf != تہی)
		trunc_string(s, buf, گنجائش, len);
	}
    }
    واپس buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    خالی
trunc_string(
    char_u	*s,
    char_u	*buf,
    صحیح		گنجائش_اندر,
    صحیح		buflen)
{
    size_t	گنجائش = گنجائش_اندر - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    صحیح		e;
    صحیح		i;
    صحیح		n;

    اگر (*s == NUL)
    {
	اگر (buflen > 0)
	    *buf = NUL;
	واپس;
    }

    اگر (گنجائش_اندر < 3)
	گنجائش = 0;
    half = گنجائش / 2;

    // First part: Start of the string.
    برائے (e = 0; len < half && e < buflen; ++e)
    {
	اگر (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    واپس;
	}
	n = ptr2cells(s + e);
	اگر (len + n > half)
	    توڑو;
	len += n;
	buf[e] = s[e];
	اگر (has_mbyte)
	    برائے (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		اگر (++e == buflen)
		    توڑو;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    اگر (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	جب (len + n > گنجائش)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    ورنہ اگر (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (صحیح)STRLEN(s);
	برائے (;;)
	{
	    کرو
		half = half - utf_سر_off(s, s + half - 1) - 1;
	    جب (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    اگر (len + n > گنجائش || half == 0)
		توڑو;
	    len += n;
	    i = (صحیح)half;
	}
    }
    ورنہ
    {
	برائے (i = (صحیح)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= گنجائش; --i)
	    len += n;
    }


    اگر (i <= e + 3)
    {
	// text fits without truncating
	اگر (s != buf)
	{
	    len = STRLEN(s);
	    اگر (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    اگر (len < 1)
		buf[e - 1] = NUL;
	    ورنہ
		mch_memmove(buf + e, s + e, len);
	}
    }
    ورنہ اگر (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	اگر (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    ورنہ
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
#اگر_غیر_تعریف_شدہ PROTO

صحیح vim_snprintf(حرف *str, size_t str_m, ثابت حرف *fmt, ...);

    صحیح
smsg(ثابت حرف *s, ...)
{
    اگر (IObuff == تہی)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	واپس پیغام((حرف *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((حرف *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    واپس پیغام((حرف *)IObuff);
}

    صحیح
smsg_attr(صحیح attr, ثابت حرف *s, ...)
{
    اگر (IObuff == تہی)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	واپس پیغام_attr((حرف *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((حرف *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    واپس پیغام_attr((حرف *)IObuff, attr);
}

    صحیح
smsg_attr_keep(صحیح attr, ثابت حرف *s, ...)
{
    اگر (IObuff == تہی)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	واپس پیغام_attr_keep((حرف *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((حرف *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    واپس پیغام_attr_keep((حرف *)IObuff, attr, TRUE);
}

#اگر_ختم

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
ساکن صحیح	آخری_sourcing_lnum = 0;
ساکن char_u   *آخری_sourcing_name = تہی;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    خالی
reset_آخری_sourcing(خالی)
{
    VIM_CLEAR(آخری_sourcing_name);
    آخری_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    ساکن صحیح
other_sourcing_name(خالی)
{
    اگر (HAVE_SOURCING_INFO && SOURCING_NAME != تہی)
    {
	اگر (آخری_sourcing_name != تہی)
	    واپس STRCMP(SOURCING_NAME, آخری_sourcing_name) != 0;
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ساکن char_u *
get_emsg_ماخذ(خالی)
{
    char_u	*Buf, *p;

    اگر (HAVE_SOURCING_INFO && SOURCING_NAME != تہی && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	اگر (sname == تہی)
	    sname = SOURCING_NAME;

#اگر_تعریف_شدہ FEAT_EVAL
	اگر (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	ورنہ
#اگر_ختم
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	اگر (Buf != تہی)
	    sprintf((حرف *)Buf, (حرف *)p, sname);
	vim_free(tofree);
	واپس Buf;
    }
    واپس تہی;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    ساکن char_u *
get_emsg_lnum(خالی)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    اگر (SOURCING_NAME != تہی
	    && (other_sourcing_name() || SOURCING_LNUM != آخری_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	اگر (Buf != تہی)
	    sprintf((حرف *)Buf, (حرف *)p, (طویل)SOURCING_LNUM);
	واپس Buf;
    }
    واپس تہی;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    خالی
پیغام_ماخذ(صحیح attr)
{
    char_u	*p;
    ساکن صحیح	recursive = FALSE;

    // Bail out if something called here causes an error.
    اگر (recursive)
	واپس;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_ماخذ();
    اگر (p != تہی)
    {
	پیغام_scroll = TRUE;  // this will take more than one line
	پیغام_attr((حرف *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    اگر (p != تہی)
    {
	پیغام_attr((حرف *)p, HL_ATTR(HLF_N));
	vim_free(p);
	آخری_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    اگر (SOURCING_NAME == تہی || other_sourcing_name())
    {
	VIM_CLEAR(آخری_sourcing_name);
	اگر (SOURCING_NAME != تہی)
	    آخری_sourcing_name = vim_strsave(SOURCING_NAME);
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
    ساکن صحیح
emsg_not_now(خالی)
{
    اگر ((emsg_off > 0 && vim_strchr(p_debug, 'm') == تہی
					  && vim_strchr(p_debug, 't') == تہی)
#اگر_تعریف_شدہ FEAT_EVAL
	    || emsg_skip > 0
#اگر_ختم
	    )
	واپس TRUE;
    واپس FALSE;
}

#اگر defined(FEAT_EVAL)
ساکن garray_T ignore_error_list = GA_خالی_خانے;

    خالی
ignore_error_for_testing(char_u *error)
{
    اگر (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, حجم(char_u *), 1);

    اگر (STRCMP("RESET", error) == 0)
	ga_clear_لڑیاں(&ignore_error_list);
    ورنہ
	ga_copy_string(&ignore_error_list, error);
}

    ساکن صحیح
ignore_error(ثابت حرف *پیغام)
{
    صحیح i;

    برائے (i = 0; i < ignore_error_list.ga_len; ++i)
	اگر (لڑی_تلاش(پیغام,
		  (حرف *)((char_u **)(ignore_error_list.ga_ڈیٹا))[i]) != تہی)
	    واپس TRUE;
    واپس FALSE;
}
#اگر_ختم

#اگر !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    خالی
do_perror(حرف *پیغام)
{
    perror(پیغام);
    ++emsg_silent;
    emsg(پیغام);
    --emsg_silent;
}
#اگر_ختم

/*
 * emsg_core() - display an error message
 *
 * Rings the bell, if appropriate, and calls message() to do the real work
 * When terminal not initialized (yet) mch_errmsg(..) is used.
 *
 * Return TRUE if wait_return() not called.
 * Note: caller must check 'emsg_not_now()' before calling this.
 */
    ساکن صحیح
emsg_core(ثابت حرف *s)
{
    صحیح		attr;
    char_u	*p;
    صحیح		r;
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح		ignore = FALSE;
    صحیح		severe;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    // When testing some errors are turned into a normal message.
    اگر (ignore_error(s))
	// don't call msg() if it results in a dialog
	واپس پیغام_use_printf() ? FALSE : پیغام((حرف *)s);
#اگر_ختم

    ++called_emsg;

#اگر_تعریف_شدہ FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#اگر_ختم

    اگر (!emsg_off || vim_strchr(p_debug, 't') != تہی)
    {
#اگر_تعریف_شدہ FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	اگر (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    اگر (!ignore)
		++did_emsg;
	    واپس TRUE;
	}

	اگر (اندر_assert_fails && emsg_assert_fails_پیغام == تہی)
	{
	    emsg_assert_fails_پیغام = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == تہی ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	سیٹ_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#اگر_ختم

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	اگر (emsg_silent != 0)
	{
#اگر_تعریف_شدہ FEAT_EVAL
	    ++did_emsg_silent;
#اگر_ختم
	    اگر (emsg_noredir == 0)
	    {
		پیغام_آغاز();
		p = get_emsg_ماخذ();
		اگر (p != تہی)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		اگر (p != تہی)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		redir_write((char_u *)s, -1);
	    }
#اگر_تعریف_شدہ FEAT_EVAL
	    // Only increment did_emsg_def when :silent! wasn't used inside the
	    // :def function.
	    اگر (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	    ch_log(تہی, "ERROR silent: %s", s);
#اگر_ختم
	    واپس TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	پیغام_silent = 0;
	cmd_silent = FALSE;

	اگر (global_busy)		// break :global command
	    ++global_busy;

	اگر (p_eb)
	    beep_flush();		// also includes flush_buffers()
	ورنہ
	    flush_buffers(FLUSH_اقل);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#اگر_تعریف_شدہ FEAT_EVAL
	++uncaught_emsg;
#اگر_ختم
    }

#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
    اگر (!اندر_echowindow)
#اگر_ختم
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    اگر (پیغام_scrolled != 0)
	need_wait_return = TRUE;    // needed in case emsg() is called after
				    // wait_return() has reset need_wait_return
				    // and a redraw is expected because
				    // msg_scrolled is non-zero

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    emsg_مقصد_channel_log = TRUE;
#اگر_ختم
    /*
     * Display name and line number for the source of the error.
     */
    پیغام_scroll = TRUE;
    پیغام_ماخذ(attr);

    /*
     * Display the error message itself.
     */
    پیغام_nowait = FALSE;			// wait for this msg
    r = پیغام_attr((حرف *)s, attr);

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    emsg_مقصد_channel_log = FALSE;
#اگر_ختم
    واپس r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    صحیح
emsg(حرف *s)
{
    // Skip this if not giving error messages at the moment.
    اگر (emsg_not_now())
	واپس TRUE;

    واپس emsg_core(s);
}

#اگر_غیر_تعریف_شدہ PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    صحیح
semsg(ثابت حرف *s, ...)
{
    // Skip this if not giving error messages at the moment.
    اگر (emsg_not_now())
	واپس TRUE;

    اگر (IObuff == تہی)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	واپس emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((حرف *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    واپس emsg_core((حرف *)IObuff);
}
#اگر_ختم

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    خالی
iemsg(حرف *s)
{
    اگر (emsg_not_now())
	واپس;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_اندرونی_error_please_report_a_bug));

    emsg_core(s);
#اگر defined(ABORT_ON_اندرونی_ERROR) && defined(FEAT_EVAL)
    سیٹ_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    پیغام_putchar('\n');  // avoid overwriting the error message
    برآمد_flush();
    abort();
#اگر_ختم
}

#اگر_غیر_تعریف_شدہ PROTO  // manual proto with __attribute__
/*
 * Same as semsg(...) but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 * Note: caller must not pass 'IObuff' as 1st argument.
 */
    خالی
siemsg(ثابت حرف *s, ...)
{
    اگر (emsg_not_now())
	واپس;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_اندرونی_error_please_report_a_bug));

    اگر (IObuff == تہی)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    ورنہ
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((حرف *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((حرف *)IObuff);
    }
# ifdef ABORT_ON_اندرونی_ERROR
    پیغام_putchar('\n');  // avoid overwriting the error message
    برآمد_flush();
    abort();
# endif
}
#اگر_ختم

/*
 * Give an "Internal error" message.
 */
    خالی
اندرونی_error(حرف *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_اندرونی_error_please_report_a_bug));

    siemsg(_(e_اندرونی_error_str), where);
}

#اگر defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    خالی
اندرونی_error_no_abort(حرف *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_اندرونی_error_please_report_a_bug));

    semsg(_(e_اندرونی_error_str), where);
}
#اگر_ختم

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    خالی
emsg_invreg(صحیح name)
{
    semsg(_(e_invalid_رجسٹر_name_str), transchar_buf(تہی, name));
}

#اگر defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    خالی
emsg_namelen(حرف *پیغام, char_u *name, صحیح len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(پیغام, copy == تہی ? "NULL" : (حرف *)copy);
    vim_free(copy);
}
#اگر_ختم

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    حرف *
پیغام_trunc_attr(حرف *s, صحیح force, صحیح attr)
{
    صحیح		n;
    حرف	*ts;

    // Add message to history before truncating
    جوڑو_پیغام_hist((char_u *)s, -1, attr);

    ts = (حرف *)پیغام_may_trunc(force, (char_u *)s);

    پیغام_hist_off = TRUE;
    n = پیغام_attr(ts, attr);
    پیغام_hist_off = FALSE;

    اگر (n)
	واپس ts;
    واپس تہی;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
پیغام_may_trunc(صحیح force, char_u *s)
{
    صحیح		n;
    صحیح		گنجائش;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    گنجائش = (صحیح)(صفیں - cmdline_صف - 1) * cmdline_چوڑائی + sc_ستون - 1;
    اگر (گنجائش > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_فعال))
	    && (n = (صحیح)STRLEN(s) - گنجائش) > 0)
    {
	اگر (has_mbyte)
	{
	    صحیح	سائز = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    اگر (سائز <= گنجائش)
		واپس s;

	    برائے (n = 0; سائز >= گنجائش; )
	    {
		سائز -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    واپس s;
}

    ساکن خالی
جوڑو_پیغام_hist(
    char_u	*s,
    صحیح		len,		// -1 for undetermined length
    صحیح		attr)
{
    ساخت پیغام_hist *p;

    اگر (پیغام_hist_off || پیغام_silent != 0)
	واپس;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(ساخت پیغام_hist);
    اگر (p == تہی)
	واپس;

    اگر (len < 0)
	len = (صحیح)STRLEN(s);
    // remove leading and trailing newlines
    جب (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    جب (len > 0 && s[len - 1] == '\n')
	--len;
    p->پیغام = vim_strnsave(s, len);
    p->اگلا = تہی;
    p->attr = attr;
    اگر (آخری_پیغام_hist != تہی)
	آخری_پیغام_hist->اگلا = p;
    آخری_پیغام_hist = p;
    اگر (پہلا_پیغام_hist == تہی)
	پہلا_پیغام_hist = آخری_پیغام_hist;
    ++پیغام_hist_len;

    check_پیغام_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    صحیح
delete_پہلا_پیغام(خالی)
{
    ساخت پیغام_hist *p;

    اگر (پیغام_hist_len <= 0)
	واپس FAIL;
    p = پہلا_پیغام_hist;
    پہلا_پیغام_hist = p->اگلا;
    اگر (پہلا_پیغام_hist == تہی)
	آخری_پیغام_hist = تہی;  // history is empty
    vim_free(p->پیغام);
    vim_free(p);
    --پیغام_hist_len;
    واپس OK;
}

    ساکن خالی
check_پیغام_hist(خالی)
{
    // Don't let the message history get too big
    جب (پیغام_hist_len > 0 && پیغام_hist_len > پیغام_hist_max)
	(خالی)delete_پہلا_پیغام();
}


    صحیح
messagesopt_بدلا(خالی)
{
    char_u	*p;
    صحیح		messages_flags_new = 0;
    صحیح		messages_wait_new = 0;
    صحیح		messages_history_new = 0;

    p = p_mopt;
    جب (*p != NUL)
    {
	اگر (STRNCMP(p, MESSAGES_OPT_کامیابی_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_کامیابی_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_کامیابی_ENTER);
	    messages_flags_new |= MESSAGES_کامیابی_ENTER;
	}
	ورنہ اگر (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	ورنہ اگر (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	اگر (*p != ',' && *p != NUL)
	    واپس FAIL;
	اگر (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    اگر (!(messages_flags_new & (MESSAGES_کامیابی_ENTER | MESSAGES_WAIT)))
	واپس FAIL;

    // "history" must be set
    اگر (!(messages_flags_new & MESSAGES_HISTORY))
	واپس FAIL;

    اگر (messages_history_new < 0 || messages_history_new > 10000)
	واپس FAIL;

    اگر (messages_wait_new < 0 || messages_wait_new > 10000)
	واپس FAIL;

    پیغام_flags = messages_flags_new;
    پیغام_wait = messages_wait_new;

    پیغام_hist_max = messages_history_new;
    check_پیغام_hist();

    واپس OK;
}

/*
 * ":messages" command.
 */
    خالی
ex_messages(exarg_T *eap)
{
    ساخت پیغام_hist *p;
    char_u	    *s;
    صحیح		    c = 0;

    اگر (STRCMP(eap->arg, "clear") == 0)
    {
	صحیح keep = eap->addr_گنتی == 0 ? 0 : eap->line2;

	جب (پیغام_hist_len > keep)
	    (خالی)delete_پہلا_پیغام();
	واپس;
    }

    اگر (*eap->arg != NUL)
    {
	emsg(_(e_invalid_دلیل));
	واپس;
    }

    پیغام_hist_off = TRUE;

    p = پہلا_پیغام_hist;
    اگر (eap->addr_گنتی != 0)
    {
	// Count total messages
	برائے (; p != تہی && !got_int; p = p->اگلا)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	برائے (p = پہلا_پیغام_hist; p != تہی && !got_int && c > 0;
						    p = p->اگلا, c--);
    }

    اگر (p == پہلا_پیغام_hist)
    {
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	s = get_mess_lang();
#ورنہ
	s = mch_getenv((char_u *)"LANG");
#اگر_ختم
	اگر (s != تہی && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    پیغام_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    برائے (; p != تہی && !got_int; p = p->اگلا)
	اگر (p->پیغام != تہی)
	    پیغام_attr((حرف *)p->پیغام, p->attr);

    پیغام_hist_off = FALSE;
}

#اگر defined(FEAT_CON_DIALOG) || defined(ڈھونڈو_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    خالی
پیغام_انتہا_prompt(خالی)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_صف = پیغام_صف;
    پیغام_ستون = 0;
    پیغام_clr_eos();
    سطریں_بایاں = -1;
}
#اگر_ختم

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    خالی
wait_return(صحیح redraw)
{
    صحیح		c;
    صحیح		oldState;
    صحیح		tmpState;
    صحیح		had_got_int;
    صحیح		save_reg_recording;
    FILE	*save_scriptout;

    اگر (redraw == TRUE)
	سیٹ_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    اگر (پیغام_silent != 0)
	واپس;
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
    اگر (اندر_echowindow)
	واپس;
#اگر_ختم

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    اگر (vgetc_busy > 0)
	واپس;
    need_wait_return = TRUE;
    اگر (no_wait_return)
    {
	اگر (!exmode_فعال)
	    cmdline_صف = پیغام_صف;
	واپس;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    اگر (quit_مزید)
    {
	c = CAR;		// just pretend CR was hit
	quit_مزید = FALSE;
	got_int = FALSE;
    }
    ورنہ اگر (exmode_فعال)
    {
	پیغام_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    ورنہ اگر (!stuff_خالی_خانے())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    ورنہ
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = منوال_HITRETURN;
	setmouse();
#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#اگر_ختم
	cmdline_صف = پیغام_صف;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	اگر (need_check_timestamps)
	    check_timestamps(FALSE);

	اگر (پیغام_flags & MESSAGES_کامیابی_ENTER)
	{
	    کامیابی_return_پیغام();

	    کرو
	    {
#اگر_تعریف_شدہ FEAT_IMAGE
		// Make sure to update image placements (since their row offset
		// will have been changed).
		draw_image_placements();
#اگر_ختم
		// Remember "got_int", if it is set vgetc() probably returns a
		// CTRL-C, but we need to loop then.
		had_got_int = got_int;

		// Don't do mappings here, we put the character back in the
		// typeahead buffer.
		++no_mapping;
		++allow_کلیدیں;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = تہی;
		c = محفوظ_vgetc();
		اگر (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_کلیدیں;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#اگر_تعریف_شدہ FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		اگر (کلید_Typed && c == Ctrl_Y && clip_ستارہ.state == SELECT_مکمل)
		{
		    clip_copy_modeless_selection(TRUE);
		    c = K_IGNORE;
		}
#اگر_ختم

		/*
		* Allow scrolling back in the messages.
		* Also accept scroll-down commands when messages fill the
		* screen, to avoid that typing one 'j' too many makes the
		* messages disappear.
		*/
		اگر (کلید_Typed && p_مزید && !p_cp)
		{
		    اگر (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_اوپر || c == K_PAGEUP)
		    {
			اگر (پیغام_scrolled > صفیں)
			    // scroll back to show older messages
			    do_مزید_prompt(c);
			ورنہ
			{
			    پیغام_didout = FALSE;
			    c = K_IGNORE;
			    پیغام_ستون =
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_چوڑائی - 1 :
#اگر_ختم
				0;
			}
			اگر (quit_مزید)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_مزید = FALSE;
			    got_int = FALSE;
			}
			ورنہ اگر (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    کامیابی_return_پیغام();
			}
		    }
		    ورنہ اگر (پیغام_scrolled > صفیں - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_نیچے_کو || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } جب ((had_got_int && c == Ctrl_C)
				|| c == K_IGNORE
#اگر_تعریف_شدہ FEAT_GUI
				|| c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR
#اگر_ختم
				|| c == K_LEFTDRAG   || c == K_LEFTRELEASE
				|| c == K_MIDDLEDRAG || c == K_MIDDLERELEASE
				|| c == K_RIGHTDRAG  || c == K_RIGHTRELEASE
				|| c == K_MOUSELEFT  || c == K_MOUSERIGHT
				|| c == K_MOUSEDOWN  || c == K_MOUSEUP
				|| c == K_MOUSEMOVE
				|| (!mouse_has(MOUSE_RETURN)
				    && mouse_صف < پیغام_صف
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    اگر (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(خالی)jump_مقصد_mouse(MOUSE_SETPOS, تہی, 0);
	    ورنہ اگر (!کلید_Typed || (vim_strchr((char_u *)"\r\n ", c) == تہی
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_نقاب);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	ورنہ
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(پیغام_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    اگر (c == ':' || c == '?' || c == '/')
    {
	اگر (!exmode_فعال)
	    cmdline_صف = پیغام_صف;
	skip_redraw = TRUE;	    // skip redraw once
	do_redraw = FALSE;
#اگر_تعریف_شدہ FEAT_TERMINAL
	skip_رکن_loop = TRUE;
#اگر_ختم
    }

    /*
     * If the window size changed set_shellsize() will redraw the screen.
     * Otherwise the screen is only redrawn if 'redraw' is set and no ':'
     * typed.
     */
    tmpState = State;
    State = oldState;		    // restore State before set_shellsize
    setmouse();
    پیغام_check();

#اگر_غیر_تعریف_شدہ MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    اگر (swapping_screen() && !termcap_فعال)
	newline_on_exit = TRUE;
#اگر_ختم

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    سطریں_بایاں = -1;		// reset lines_left at next msg_start()
    reset_آخری_sourcing();
    اگر (keep_پیغام != تہی && vim_strsize(keep_پیغام) >=
			    (صفیں - cmdline_صف - 1) * cmdline_چوڑائی + sc_ستون)
	VIM_CLEAR(keep_پیغام);	    // don't redisplay message, it's too long

    اگر (tmpState == منوال_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    ورنہ اگر (!skip_redraw
	    && (redraw == TRUE || (پیغام_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    ساکن خالی
کامیابی_return_پیغام(خالی)
{
    صحیح		save_p_مزید = p_مزید;

    p_مزید = FALSE;	// don't want to see this message when scrolling back
    اگر (پیغام_didout)	// start on a new line
	پیغام_putchar('\n');
    اگر (got_int)
	پیغام_puts(_("Interrupt: "));

    پیغام_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    اگر (!پیغام_use_printf())
	پیغام_clr_eos();
    p_مزید = save_p_مزید;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    خالی
سیٹ_keep_پیغام(char_u *s, صحیح attr)
{
    vim_free(keep_پیغام);
    اگر (s != تہی && پیغام_silent == 0)
	keep_پیغام = vim_strsave(s);
    ورنہ
	keep_پیغام = تہی;
    keep_پیغام_مزید = FALSE;
    keep_پیغام_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    خالی
سیٹ_keep_پیغام_from_hist(خالی)
{
    اگر (keep_پیغام == تہی && آخری_پیغام_hist != تہی && پیغام_scrolled == 0
						      && (State & منوال_معمول))
	سیٹ_keep_پیغام(آخری_پیغام_hist->پیغام, آخری_پیغام_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    خالی
پیغام_آغاز(خالی)
{
    صحیح		did_return = FALSE;

    اگر (پیغام_صف < cmdline_صف)
	پیغام_صف = cmdline_صف;

    اگر (!پیغام_silent)
    {
	VIM_CLEAR(keep_پیغام);
	need_fileinfo = FALSE;
    }

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	پیغام_clr_eos();
    }
#اگر_ختم

#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
    اگر (اندر_echowindow)
    {
	اگر (popup_پیغام_win_visible()
		    && ((پیغام_ستون > 0 && (پیغام_scroll || !بھرا_screen))
			|| اندر_echowindow))
	{
	    win_T *wp = popup_get_پیغام_win();

	    // start a new line
	    curbuf = wp->w_بفر;
	    ml_append(wp->w_بفر->b_ml.ml_سطر_گنتی,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_بفر;
	}
	پیغام_ستون = 0;
    }
    ورنہ
#اگر_ختم
	اگر (!پیغام_scroll && بھرا_screen)	// overwrite last message
    {
	پیغام_صف = cmdline_صف;
	پیغام_ستون =
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_چوڑائی - 1 :
#اگر_ختم
	    0;
    }
    ورنہ اگر (پیغام_didout || اندر_echowindow)
    {
	// start message on next line
	پیغام_putchar('\n');
	did_return = TRUE;
	اگر (exmode_فعال != EXMODE_معمول)
	    cmdline_صف = پیغام_صف;
    }
    اگر (!پیغام_didany || سطریں_بایاں < 0)
	پیغام_starthere();
    اگر (پیغام_silent == 0)
    {
	پیغام_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    اگر (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    خالی
پیغام_starthere(خالی)
{
    سطریں_بایاں = cmdline_صف;
    پیغام_didany = FALSE;
}

    خالی
پیغام_putchar(صحیح c)
{
    پیغام_putchar_attr(c, 0);
}

    خالی
پیغام_putchar_attr(صحیح c, صحیح attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    اگر (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_دوسرا(c);
	buf[2] = K_تیسرا(c);
	buf[3] = NUL;
    }
    ورنہ
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    پیغام_puts_attr((حرف *)buf, attr);
}

    خالی
پیغام_outnum(طویل n)
{
    حرف	buf[20];

    sprintf(buf, "%ld", n);
    پیغام_puts(buf);
}

    خالی
پیغام_home_replace(char_u *fname)
{
    پیغام_home_replace_attr(fname, 0);
}

#اگر defined(FEAT_ڈھونڈو_ID)
    خالی
پیغام_home_replace_hl(char_u *fname)
{
    پیغام_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#اگر_ختم

    ساکن خالی
پیغام_home_replace_attr(char_u *fname, صحیح attr)
{
    char_u	*name;

    name = home_replace_save(تہی, fname);
    اگر (name != تہی)
	پیغام_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    صحیح
پیغام_outtrans(char_u *str)
{
    واپس پیغام_outtrans_attr(str, 0);
}

    صحیح
پیغام_outtrans_attr(char_u *str, صحیح attr)
{
    واپس پیغام_outtrans_len_attr(str, (صحیح)STRLEN(str), attr);
}

    صحیح
پیغام_outtrans_len(char_u *str, صحیح len)
{
    واپس پیغام_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
پیغام_outtrans_one(char_u *p, صحیح attr)
{
    صحیح		l;

    اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	پیغام_outtrans_len_attr(p, l, attr);
	واپس p + l;
    }
    پیغام_puts_attr((حرف *)transchar_byte_buf(تہی, *p), attr);
    واپس p + 1;
}

    صحیح
پیغام_outtrans_len_attr(char_u *msgstr, صحیح len, صحیح attr)
{
    صحیح		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_آغاز = msgstr;
    char_u	*s;
    صحیح		mb_l;
    صحیح		c;
    صحیح		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    اگر (attr == 0)
	attr = HL_ATTR(HLF_پیغام);

    // if MSG_HIST flag set, add message to history
    اگر (attr & پیغام_HIST)
    {
	جوڑو_پیغام_hist(str, len, attr);
	attr &= ~پیغام_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    اگر (پیغام_silent == 0 && len > 0 && پیغام_صف >= cmdline_صف && پیغام_ستون == 0)
    {
	clear_cmdline = FALSE;
	منوال_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    اگر (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	پیغام_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    جب (--len >= 0 && !got_int)
    {
	اگر (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	ورنہ اگر (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	ورنہ
	    mb_l = 1;
	اگر (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    اگر (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    ورنہ
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		اگر (str > plain_آغاز)
		    پیغام_puts_attr_len((حرف *)plain_آغاز,
					       (صحیح)(str - plain_آغاز), attr);
		plain_آغاز = str + mb_l;
		پیغام_puts_attr((حرف *)transchar_buf(تہی, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	ورنہ
	{
	    s = transchar_byte_buf(تہی, *str);
	    اگر (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		اگر (str > plain_آغاز)
		    پیغام_puts_attr_len((حرف *)plain_آغاز,
					       (صحیح)(str - plain_آغاز), attr);
		plain_آغاز = str + 1;
		پیغام_puts_attr((حرف *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (صحیح)STRLEN(s);
	    }
	    ورنہ
		++retval;
	    ++str;
	}
    }

    اگر (str > plain_آغاز && !got_int)
	// print the printable chars at the end
	پیغام_puts_attr_len((حرف *)plain_آغاز, (صحیح)(str - plain_آغاز), attr);

    got_int |= save_got_int;

    واپس retval;
}

#اگر defined(FEAT_QUICKFIX)
    خالی
پیغام_make(char_u *arg)
{
    صحیح	    i;
    ساکن char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    برائے (i = 5; *arg && i >= 0; --i)
	اگر (*arg++ != str[i])
	    توڑو;
    اگر (i < 0)
    {
	پیغام_putchar('\n');
	برائے (i = 0; rs[i]; ++i)
	    پیغام_putchar(rs[i] - 3);
    }
}
#اگر_ختم

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
    صحیح
پیغام_outtrans_special(
    char_u	*strstart,
    صحیح		from,	// TRUE for lhs of a mapping
    صحیح		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    صحیح		retval = 0;
    حرف	*متن;
    صحیح		attr;
    صحیح		len;

    attr = HL_ATTR(HLF_8);
    جب (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	اگر ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    متن = "<Space>";
	    ++str;
	}
	ورنہ
	    متن = (حرف *)str2special(&str, from, FALSE);
	اگر (متن[0] != NUL && متن[1] == NUL)
	    // single-byte character or illegal byte
	    متن = (حرف *)transchar_byte_buf(تہی, (char_u)متن[0]);
	len = vim_strsize((char_u *)متن);
	اگر (maxlen > 0 && retval + len >= maxlen)
	    توڑو;
	// Highlight special keys
	پیغام_puts_attr(متن, len > 1
		&& (*mb_ptr2len)((char_u *)متن) <= 1 ? attr : 0);
	retval += len;
    }
    واپس retval;
}

#اگر defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    صحیح	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    صحیح	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    جب (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    واپس (char_u *)ga.ga_ڈیٹا;
}
#اگر_ختم

/*
 * Return the printable string for the key codes at "*sp".
 * On illegal byte return a string with only that byte.
 * Used for translating the lhs or rhs of a mapping to printable chars.
 * Advances "sp" to the next code.
 */
    char_u *
str2special(
    char_u	**sp,
    صحیح		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    صحیح		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    صحیح			c;
    ساکن char_u	buf[7];
    char_u		*str = *sp;
    صحیح			modifiers = 0;
    صحیح			special = FALSE;

    اگر (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	اگر (p != تہی)
	    واپس p;
    }

    c = *str;
    اگر ((c == K_SPECIAL
#اگر_تعریف_شدہ FEAT_GUI
		|| c == CSI
#اگر_ختم
	) && str[1] != NUL && str[2] != NUL)
    {
	اگر (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	اگر ((c == K_SPECIAL
#اگر_تعریف_شدہ FEAT_GUI
		    || c == CSI
#اگر_ختم
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = مقصد_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	اگر (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    اگر (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	اگر (p != تہی)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	ورنہ
	    // illegal byte
	    *sp = str + 1;
    }
    ورنہ
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    اگر (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	واپس get_special_کلید_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    واپس buf;
}

/*
 * Translate a key sequence into special key names.
 */
    خالی
str2specialbuf(char_u *sp, char_u *buf, صحیح len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    جب (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	اگر (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	ورنہ
	    توڑو;
    }
}

/*
 * print line for :print or :list command
 */
    خالی
پیغام_prt_سطر(char_u *s, صحیح list)
{
    صحیح		c;
    صحیح		ستون = 0;
    صحیح		n_extra = 0;
    صحیح		c_extra = 0;
    صحیح		c_final = 0;
    char_u	*p_extra = تہی;	    // init to make SASC shut up
    صحیح		n;
    صحیح		attr = 0;
    char_u	*trail = تہی;
    char_u	*lead = تہی;
    صحیح		اندر_multispace = FALSE;
    صحیح		multispace_pos = 0;
    صحیح		l;
    char_u	buf[MB_MAXBYTES + 1];

    اگر (curwin->w_p_list)
	list = TRUE;

    اگر (list)
    {
	// find start of trailing whitespace
	اگر (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    جب (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	اگر (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != تہی
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    جب (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    اگر (*lead == NUL)
		lead = تہی;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    اگر (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	پیغام_putchar(' ');

    جب (!got_int)
    {
	اگر (n_extra > 0)
	{
	    --n_extra;
	    اگر (n_extra == 0 && c_final)
		c = c_final;
	    ورنہ اگر (c_extra)
		c = c_extra;
	    ورنہ
		c = *p_extra++;
	}
	ورنہ اگر (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    ستون += (*mb_ptr2cells)(s);
	    اگر (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    ورنہ اگر (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		صحیح len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    ورنہ
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    پیغام_puts((حرف *)buf);
	    s += l;
	    جاری;
	}
	ورنہ
	{
	    attr = 0;
	    c = *s++;
	    اگر (list)
	    {
		اندر_multispace = c == ' ' && (*s == ' '
						 || (ستون > 0 && s[-2] == ' '));
		اگر (!اندر_multispace)
		    multispace_pos = 0;
	    }
	    اگر (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#اگر_تعریف_شدہ FEAT_VARTABS
		n_extra = tabstop_padding(ستون, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#ورنہ
		n_extra = curbuf->b_p_ts - ستون % curbuf->b_p_ts - 1;
#اگر_ختم
		اگر (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		ورنہ
		{
		    صحیح lcs_tab1 = curwin->w_lcs_chars.tab1;
		    صحیح lcs_tab2 = curwin->w_lcs_chars.tab2;
		    صحیح lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    اگر (lead != تہی && s <= lead
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
	    ورنہ اگر (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    ورنہ اگر (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    ورنہ اگر (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(تہی, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    ورنہ اگر (c == ' ')
	    {
		اگر (lead != تہی && s <= lead && اندر_multispace
			&& curwin->w_lcs_chars.leadmultispace != تہی)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    اگر (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		ورنہ اگر (lead != تہی && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		ورنہ اگر (trail != تہی && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		ورنہ اگر (اندر_multispace
			&& curwin->w_lcs_chars.multispace != تہی)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    اگر (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		ورنہ اگر (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	اگر (c == NUL)
	    توڑو;

	پیغام_putchar_attr(c, attr);
	ستون++;
    }
    پیغام_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    ساکن char_u *
screen_puts_mbyte(char_u *s, صحیح l, صحیح attr)
{
    صحیح		cw;

    پیغام_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    اگر (cw > 1 && (
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		cmdmsg_rl ? پیغام_ستون <= 1 :
#اگر_ختم
		پیغام_ستون == cmdline_چوڑائی - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	پیغام_screen_putchar('>', HL_ATTR(HLF_AT));
	واپس s;
    }

    screen_puts_len(s, l, پیغام_صف, cmdline_ستون_off + پیغام_ستون, attr);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (cmdmsg_rl)
    {
	پیغام_ستون -= cw;
	اگر (پیغام_ستون == 0)
	{
	    پیغام_ستون = cmdline_چوڑائی;
	    ++پیغام_صف;
	}
    }
    ورنہ
#اگر_ختم
    {
	پیغام_ستون += cw;
	اگر (پیغام_ستون >= cmdline_چوڑائی)
	{
	    پیغام_ستون = 0;
	    ++پیغام_صف;
	}
    }
    واپس s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    خالی
پیغام_puts(حرف *s)
{
    پیغام_puts_attr(s, 0);
}

    خالی
پیغام_puts_title(حرف *s)
{
    پیغام_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    ساکن خالی
پیغام_outtrans_long_len_attr(char_u *longstr, صحیح len, صحیح attr)
{
    صحیح		slen = len;
    صحیح		گنجائش;

    گنجائش = cmdline_چوڑائی - پیغام_ستون;
    اگر (len > گنجائش && گنجائش >= 20)
    {
	slen = (گنجائش - 3) / 2;
	پیغام_outtrans_len_attr(longstr, slen, attr);
	پیغام_puts_attr("...", HL_ATTR(HLF_8));
    }
    پیغام_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    خالی
پیغام_outtrans_long_attr(char_u *longstr, صحیح attr)
{
    پیغام_outtrans_long_len_attr(longstr, (صحیح)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    خالی
پیغام_puts_attr(حرف *s, صحیح attr)
{
    پیغام_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    ساکن خالی
پیغام_puts_attr_len(حرف *str, صحیح maxlen, صحیح attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    اگر (پیغام_silent != 0)
	واپس;

    اگر (attr == 0)
	attr = HL_ATTR(HLF_پیغام);

    // if MSG_HIST flag set, add message to history
    اگر ((attr & پیغام_HIST) && maxlen < 0)
    {
	جوڑو_پیغام_hist((char_u *)str, -1, attr);
	attr &= ~پیغام_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    اگر (پیغام_scrolled != 0 && !پیغام_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    پیغام_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    اگر (پیغام_use_printf())
	پیغام_puts_printf((char_u *)str, maxlen);
    ورنہ
	پیغام_puts_display((char_u *)str, maxlen, attr, FALSE);

    need_fileinfo = FALSE;
}

// values for "where"
#تعریف_کرو PUT_APPEND 0		// append to "lnum"
#تعریف_کرو PUT_TRUNC 1		// replace "lnum"
#تعریف_کرو PUT_نیچے 2		// add below "lnum"
				//
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
/*
 * Put text "t_s" until "end" in the message window.
 * "where" specifies where to put the text.
 */
    ساکن خالی
put_پیغام_win(win_T *wp, صحیح where, char_u *t_s, char_u *انتہا, linenr_T lnum)
{
    char_u  *p;

    اگر (where == PUT_نیچے)
    {
	اگر (*انتہا != NUL)
	{
	    p = vim_strnsave(t_s, انتہا - t_s);
	    اگر (p == تہی)
		واپس;
	}
	ورنہ
	    p = t_s;
	ml_append_buf(wp->w_بفر, lnum, p, (colnr_T)0, FALSE);
	اگر (p != t_s)
	    vim_free(p);
    }
    ورنہ
    {
	char_u *newp;

	curbuf = wp->w_بفر;
	اگر (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    اگر (newp == تہی)
		واپس;
	    اگر (*انتہا != NUL)
		newp[STRLEN(ml_get(lnum)) + (انتہا - t_s)] = NUL;
	}
	ورنہ
	{
	    newp = vim_strnsave(t_s, انتہا - t_s);
	    اگر (newp == تہی)
		واپس;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_بفر;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    پیغام_ستون += (صحیح)(انتہا - t_s);
}
#اگر_ختم

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    ساکن خالی
پیغام_puts_display(
    char_u	*str,
    صحیح		maxlen,
    صحیح		attr,
    صحیح		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    صحیح		t_ستون = 0;	// screen cells todo, 0 when "t_s" not used
    صحیح		l;
    صحیح		cw;
    char_u	*sb_str = str;
    صحیح		sb_ستون = پیغام_ستون;
    صحیح		wrap;
    صحیح		did_آخری_char;
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
    صحیح		where = PUT_APPEND;
    win_T	*پیغام_win = تہی;
    linenr_T    lnum = 1;

    اگر (اندر_echowindow)
    {
	پیغام_win = popup_get_پیغام_win();

	اگر (پیغام_win != تہی)
	{
	    اگر (!popup_پیغام_win_visible())
	    {
		اگر (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    پیغام_win = تہی;
		}
		ورنہ
		{
		    // currently hidden, make it empty
		    curbuf = پیغام_win->w_بفر;
		    جب ((curbuf->b_ml.ml_flags & ML_خالی_خانے) == 0)
			ml_delete(1);
		    curbuf = curwin->w_بفر;
		}
	    }
	    ورنہ
	    {
		lnum = پیغام_win->w_بفر->b_ml.ml_سطر_گنتی;
		اگر (پیغام_ستون == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#اگر_ختم

    did_wait_return = FALSE;
    جب ((maxlen < 0 || (صحیح)(s - str) < maxlen) && *s != NUL)
    {
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	// For echowindow, use full width; for regular messages, leave last column
	صحیح wrap_ستون = پیغام_win != تہی ? cmdline_چوڑائی : cmdline_چوڑائی - 1;
#ورنہ
	صحیح wrap_ستون = cmdline_چوڑائی - 1;
#اگر_ختم

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	اگر (!recurse && پیغام_صف >= صفیں - 1 && (*s == '\n' || (
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			پیغام_ستون <= 1
		      || (*s == TAB && پیغام_ستون <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && پیغام_ستون <= 2))
		    :
#اگر_ختم
		      ((*s != '\r' && پیغام_ستون + t_ستون >= wrap_ستون)
		       || (*s == TAB && پیغام_ستون + t_ستون
			   >= (wrap_ستون & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && پیغام_ستون + t_ستون >= wrap_ستون - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    اگر (t_ستون > 0)
	    {
		// output postponed text
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
		اگر (پیغام_win != تہی)
		{
		    put_پیغام_win(پیغام_win, where, t_s, s, lnum);
		    اگر (where == PUT_نیچے)
			++lnum;
		    t_ستون = 0;
		    where = PUT_نیچے;
		}
		ورنہ
#اگر_ختم
		    t_puts(&t_ستون, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    اگر (پیغام_no_مزید && سطریں_بایاں == 0)
		توڑو;

#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	    اگر (پیغام_win == تہی)
#اگر_ختم
		// Scroll the screen up one line.
		پیغام_scroll_اوپر();

#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	    اگر (پیغام_win == تہی)
	    {
#اگر_ختم
		پیغام_صف = صفیں - 2;
		اگر (پیغام_ستون >= cmdline_چوڑائی)  // can happen after screen resize
		    پیغام_ستون = cmdline_چوڑائی - 1;
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	    }
#اگر_ختم

	    // Display char in last column before showing more-prompt.
	    اگر (*s >= ' '
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#اگر_ختم
	       )
	    {
		اگر (has_mbyte)
		{
		    اگر (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (صحیح)((str + maxlen) - s));
		    ورنہ
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		ورنہ
		    پیغام_screen_putchar(*s++, attr);
		did_آخری_char = TRUE;
	    }
	    ورنہ
		did_آخری_char = FALSE;

	    اگر (p_مزید)
		// store text for scrolling back
		store_sb_متن(&sb_str, s, attr, &sb_ستون, TRUE);

#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	    اگر (پیغام_win == تہی)
	    {
#اگر_ختم
		inc_پیغام_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		اگر (cmdline_صف > 0 && !exmode_فعال)
		    --cmdline_صف;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		اگر (سطریں_بایاں > 0)
		    --سطریں_بایاں;
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	    }
#اگر_ختم
	    اگر (p_مزید && سطریں_بایاں == 0 && State != منوال_HITRETURN
					    && !پیغام_no_مزید && !exmode_فعال)
	    {
#اگر_تعریف_شدہ FEAT_CON_DIALOG
		اگر (do_مزید_prompt(NUL))
		    s = confirm_پیغام_دم;
#ورنہ
		(خالی)do_مزید_prompt(NUL);
#اگر_ختم
		اگر (quit_مزید)
		    واپس;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    اگر (did_آخری_char)
		جاری;
	}

	wrap = *s == '\n'
		    || پیغام_ستون + t_ستون >= wrap_ستون
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && پیغام_ستون + t_ستون >= wrap_ستون - 1);
	اگر (t_ستون > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	    اگر (پیغام_win != تہی)
	    {
		put_پیغام_win(پیغام_win, where, t_s, s, lnum);
		اگر (where == PUT_نیچے)
		    ++lnum;
		t_ستون = 0;
		where = PUT_نیچے;
		// Reset msg_col after outputting to new line in echowindow
		اگر (wrap)
		    پیغام_ستون = 0;
	    }
	    ورنہ
#اگر_ختم
		t_puts(&t_ستون, t_s, s, attr);
	}

	اگر (wrap && p_مزید && !recurse)
	    // store text for scrolling back
	    store_sb_متن(&sb_str, s, attr, &sb_ستون, TRUE);

	اگر (*s == '\n')		    // go to next line
	{
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	    اگر (پیغام_win != تہی)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		اگر ((پیغام_win->w_بفر->b_ml.ml_flags & ML_خالی_خانے) == 0)
		{
		    put_پیغام_win(پیغام_win, PUT_نیچے, t_s, t_s, lnum);
		    ++lnum;
		}
		پیغام_ستون = 0;  // Reset column for new line
	    }
	    ورنہ
#اگر_ختم
		پیغام_didout = FALSE;	    // remember that line is empty
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    اگر (cmdmsg_rl)
		پیغام_ستون = cmdline_چوڑائی - 1;
	    ورنہ
#اگر_ختم
		پیغام_ستون = 0;
	    اگر (++پیغام_صف >= صفیں)  // safety check
		پیغام_صف = صفیں - 1;
	}
	ورنہ اگر (*s == '\r')	    // go to column 0
	{
	    پیغام_ستون = 0;
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	    where = PUT_TRUNC;
#اگر_ختم
	}
	ورنہ اگر (*s == '\b')	    // go to previous char
	{
	    اگر (پیغام_ستون)
		--پیغام_ستون;
	}
	ورنہ اگر (*s == TAB)	    // translate Tab into spaces
	{
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	    اگر (پیغام_win != تہی)
		پیغام_ستون = (پیغام_ستون + 7) % 8;
	    ورنہ
#اگر_ختم
		کرو
		    پیغام_screen_putchar(' ', attr);
		جب (پیغام_ستون & 7);
	}
	ورنہ اگر (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	ورنہ
	{
	    اگر (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		اگر (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (صحیح)((str + maxlen) - s));
		ورنہ
		    l = (*mb_ptr2len)(s);
	    }
	    ورنہ
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    اگر (
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#اگر_ختم
		    (cw > 1 && پیغام_ستون + t_ستون >= wrap_ستون))
	    {
		اگر (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		ورنہ
		    پیغام_screen_putchar(*s, attr);
	    }
	    ورنہ
	    {
		// postpone this character until later
		اگر (t_ستون == 0)
		    t_s = s;
		t_ستون += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    اگر (t_ستون > 0)
    {
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	اگر (پیغام_win != تہی)
	    put_پیغام_win(پیغام_win, where, t_s, s, lnum);
	ورنہ
#اگر_ختم
	    t_puts(&t_ستون, t_s, s, attr);
    }

#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
    اگر (پیغام_win != تہی)
	popup_show_پیغام_win();
#اگر_ختم
    // Store the text for scroll back.
    اگر (p_مزید && !recurse)
	store_sb_متن(&sb_str, s, attr, &sb_ستون, FALSE);

    پیغام_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    صحیح
پیغام_filtered(char_u *پیغام)
{
    صحیح مطابقت;

    اگر (cmdmod.cmod_filter_regmatch.regprog == تہی)
	واپس FALSE;
    مطابقت = vim_regexec(&cmdmod.cmod_filter_regmatch, پیغام, (colnr_T)0);
    واپس cmdmod.cmod_filter_force ? مطابقت : !مطابقت;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    ساکن خالی
پیغام_scroll_اوپر(خالی)
{
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
    اگر (اندر_echowindow)
	واپس;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    اگر (gui.اندر_use)
	gui_undraw_cursor();
#اگر_ختم
    // scrolling up always works
    mch_disable_flush();
    screen_del_سطریں(0, 0, 1, (صحیح)صفیں, TRUE, 0, تہی);
    mch_enable_flush();

    اگر (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((صحیح)صفیں - 1, (صحیح)صفیں, cmdline_ستون_off,
		cmdline_ستون_off + cmdline_چوڑائی, ' ', ' ', HL_ATTR(HLF_پیغام));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	اگر (ScreenAttrs[سطر_انحراف[صفیں - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((صحیح)صفیں - 2, (صحیح)صفیں - 1,
				 (صحیح)Columns - 1, (صحیح)Columns,
				 ' ', ' ', HL_ATTR(HLF_پیغام));
    }
}

/*
 * Increment "msg_scrolled".
 */
    ساکن خالی
inc_پیغام_scrolled(خالی)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = تہی;

	// v:scrollstart is empty, set it to the script/function name and line
	//نمبر
	اگر (p.string == تہی)
	{
	    p.string = (char_u *)_("Unknown");
	    p.لمبائی = STRLEN(p.string);
	}
	ورنہ
	{
	    size_t  tofreesize;

	    tofreesize = (صحیح)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    اگر (tofree != تہی)
	    {
		p.لمبائی = vim_snprintf_safelen((حرف *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (طویل)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	سیٹ_vim_var_string(VV_SCROLLSTART, p.string, (صحیح)p.لمبائی);
	vim_free(tofree);
    }
#اگر_ختم
    ++پیغام_scrolled;
    سیٹ_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
قسمتعریف ساخت msgchunk_S msgchunk_T;
ساخت msgchunk_S
{
    msgchunk_T	*sb_اگلا;
    msgchunk_T	*sb_پچھلا;
    حرف	sb_eol;		// TRUE when line ends after this text
    صحیح		sb_پیغام_ستون;	// column in which text starts
    صحیح		sb_attr;	// text attributes
    char_u	sb_متن[1];	// text to be displayed, actually longer
};

ساکن msgchunk_T *آخری_msgchunk = تہی; // last displayed text

ساکن msgchunk_T *پیغام_sb_آغاز(msgchunk_T *mps);

قسمتعریف شمار {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_مکمل
} sb_clear_T;

// When to clear text on next msg.
ساکن sb_clear_T do_clear_sb_متن = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    ساکن خالی
store_sb_متن(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    صحیح		attr,
    صحیح		*sb_ستون,
    صحیح		اختتام)		// line ends
{
    msgchunk_T	*mp;

    اگر (do_clear_sb_متن == SB_CLEAR_ALL
	    || do_clear_sb_متن == SB_CLEAR_CMDLINE_مکمل)
    {
	clear_sb_متن(do_clear_sb_متن == SB_CLEAR_ALL);
	پیغام_sb_eol();  // prevent messages from overlapping
	do_clear_sb_متن = SB_CLEAR_NONE;
    }

    اگر (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_متن) + (s - *sb_str) + 1);
	اگر (mp != تہی)
	{
	    mp->sb_eol = اختتام;
	    mp->sb_پیغام_ستون = *sb_ستون;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_متن, *sb_str, s - *sb_str);

	    اگر (آخری_msgchunk == تہی)
	    {
		آخری_msgchunk = mp;
		mp->sb_پچھلا = تہی;
	    }
	    ورنہ
	    {
		mp->sb_پچھلا = آخری_msgchunk;
		آخری_msgchunk->sb_اگلا = mp;
		آخری_msgchunk = mp;
	    }
	    mp->sb_اگلا = تہی;
	}
    }
    ورنہ اگر (اختتام && آخری_msgchunk != تہی)
	آخری_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_ستون = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    خالی
may_clear_sb_متن(خالی)
{
    do_clear_sb_متن = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    خالی
sb_متن_آغاز_cmdline(خالی)
{
    اگر (do_clear_sb_متن == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_متن_restart_cmdline();
    ورنہ
    {
	پیغام_sb_eol();
	do_clear_sb_متن = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    خالی
sb_متن_restart_cmdline(خالی)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_متن = SB_CLEAR_CMDLINE_BUSY;

    اگر (آخری_msgchunk == تہی || آخری_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	واپس;

    tofree = پیغام_sb_آغاز(آخری_msgchunk);
    آخری_msgchunk = tofree->sb_پچھلا;
    اگر (آخری_msgchunk != تہی)
	آخری_msgchunk->sb_اگلا = تہی;
    جب (tofree != تہی)
    {
	msgchunk_T *tofree_اگلا = tofree->sb_اگلا;

	vim_free(tofree);
	tofree = tofree_اگلا;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    خالی
sb_متن_انتہا_cmdline(خالی)
{
    do_clear_sb_متن = SB_CLEAR_CMDLINE_مکمل;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    خالی
clear_sb_متن(صحیح all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    اگر (all)
	lastp = &آخری_msgchunk;
    ورنہ
    {
	اگر (آخری_msgchunk == تہی)
	    واپس;
	lastp = &پیغام_sb_آغاز(آخری_msgchunk)->sb_پچھلا;
    }

    جب (*lastp != تہی)
    {
	mp = (*lastp)->sb_پچھلا;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    خالی
show_sb_متن(خالی)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = پیغام_sb_آغاز(آخری_msgchunk);
    اگر (mp == تہی || mp->sb_پچھلا == تہی)
	vim_beep(BO_MESS);
    ورنہ
    {
	do_مزید_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    ساکن msgchunk_T *
پیغام_sb_آغاز(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    جب (mp != تہی && mp->sb_پچھلا != تہی && !mp->sb_پچھلا->sb_eol)
	mp = mp->sb_پچھلا;
    واپس mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    خالی
پیغام_sb_eol(خالی)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    اگر (آخری_msgchunk != تہی
	    && !(آخری_msgchunk->sb_متن[0] == '\n'
		&& آخری_msgchunk->sb_متن[1] == NUL))
	آخری_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    ساکن msgchunk_T *
disp_sb_سطر(صحیح صف, msgchunk_T *smp, صحیح clear_مقصد_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    برائے (;;)
    {
	پیغام_صف = صف;
	پیغام_ستون = mp->sb_پیغام_ستون;
	p = mp->sb_متن;
	اگر (*p == '\n')	    // don't display the line break
	    ++p;
	پیغام_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	اگر (clear_مقصد_eol)
	    screen_fill(صف, صف + 1,
		    cmdline_ستون_off + پیغام_ستون, cmdline_ستون_off + cmdline_چوڑائی,
		    ' ', ' ', HL_ATTR(HLF_پیغام));

	اگر (mp->sb_eol || mp->sb_اگلا == تہی)
	    توڑو;
	mp = mp->sb_اگلا;
    }
    واپس mp->sb_اگلا;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    ساکن خالی
t_puts(
    صحیح		*t_ستون,
    char_u	*t_s,
    char_u	*s,
    صحیح		attr)
{
    // output postponed text
    پیغام_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (صحیح)(s - t_s), پیغام_صف, cmdline_ستون_off + پیغام_ستون,
	    attr);
    پیغام_ستون += *t_ستون;
    *t_ستون = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    اگر (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--پیغام_ستون;
    اگر (پیغام_ستون >= cmdline_چوڑائی)
    {
	پیغام_ستون = 0;
	++پیغام_صف;
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
    صحیح
پیغام_use_printf(خالی)
{
    واپس (!پیغام_check_screen()
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.اندر_use && !termcap_فعال)
# ورنہ
	    || !termcap_فعال
# endif
#اگر_ختم
	    || (swapping_screen() && !termcap_فعال)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    ساکن خالی
پیغام_puts_printf(char_u *str, صحیح maxlen)
{
    char_u	*s = str;
    char_u	*buf = تہی;
    char_u	*p = s;

#اگر_تعریف_شدہ MSWIN
    اگر (!(silent_منوال && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#اگر_ختم
    جب ((maxlen < 0 || (صحیح)(s - str) < maxlen) && *s != NUL)
    {
	اگر (!(silent_منوال && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    اگر (*s == NL)
	    {
		صحیح n = (صحیح)(s - p);

		buf = alloc(n + 3);
		اگر (buf != تہی)
		{
		    حافظہ_نقل(buf, p, n);
		    اگر (!info_پیغام)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    اگر (info_پیغام)   // informative message, not an error
			mch_پیغام((حرف *)buf);
		    ورنہ
			mch_errmsg((حرف *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (cmdmsg_rl)
	{
	    اگر (*s == CAR || *s == NL)
		پیغام_ستون = cmdline_چوڑائی - 1;
	    ورنہ
		--پیغام_ستون;
	}
	ورنہ
#اگر_ختم
	{
	    اگر (*s == CAR || *s == NL)
		پیغام_ستون = 0;
	    ورنہ
		++پیغام_ستون;
	}
	++s;
    }

    اگر (*p != NUL && !(silent_منوال && p_verbose == 0))
    {
	char_u *tofree = تہی;

	اگر (maxlen > 0 && vim_strlen_maxlen((حرف *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	اگر (p != تہی)
	{
	    اگر (info_پیغام)
		mch_پیغام((حرف *)p);
	    ورنہ
		mch_errmsg((حرف *)p);
	    vim_free(tofree);
	}
    }

    پیغام_didout = TRUE;	    // assume that line is not empty

#اگر_تعریف_شدہ MSWIN
    اگر (!(silent_منوال && p_verbose == 0))
	mch_settmode(TMODE_RAW);
#اگر_ختم
}

/*
 * Show the more-prompt and handle the user response.
 * This takes care of scrolling back and displaying previously displayed text.
 * When at hit-enter prompt "typed_char" is the already typed character,
 * otherwise it's NUL.
 * Returns TRUE when jumping ahead to "confirm_msg_tail".
 */
    ساکن صحیح
do_مزید_prompt(صحیح typed_char)
{
    ساکن صحیح	entered = FALSE;
    صحیح		مستعمل_typed_char = typed_char;
    صحیح		oldState = State;
    صحیح		c;
#اگر_تعریف_شدہ FEAT_CON_DIALOG
    صحیح		retval = FALSE;
#اگر_ختم
    صحیح		toscroll;
    msgchunk_T	*mp_آخری = تہی;
    msgchunk_T	*mp;
    صحیح		i;
    صحیح		پیغام_attr;

    پیغام_attr = HL_ATTR(HLF_پیغام);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    اگر (entered || (State == منوال_HITRETURN && typed_char == 0))
	واپس FALSE;
    entered = TRUE;

    اگر (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_آخری = پیغام_sb_آغاز(آخری_msgchunk);
	برائے (i = 0; i < صفیں - 2 && mp_آخری != تہی
					     && mp_آخری->sb_پچھلا != تہی; ++i)
	    mp_آخری = پیغام_sb_آغاز(mp_آخری->sb_پچھلا);
    }

    State = منوال_ASKMORE;
    setmouse();
    اگر (typed_char == NUL)
	پیغام_moremsg(FALSE);
    برائے (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	اگر (مستعمل_typed_char != NUL)
	{
	    c = مستعمل_typed_char;	// was typed at hit-enter prompt
	    مستعمل_typed_char = NUL;
	}
	ورنہ
	    c = get_keystroke();

#اگر defined(FEAT_MENU) && defined(FEAT_GUI)
	اگر (c == K_MENU)
	{
	    صحیح idx = get_menu_اشاریہ(current_menu, منوال_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    اگر (idx == MENU_اشاریہ_INVALID)
		جاری;
	    c = *current_menu->لڑیاں[idx];
	    اگر (c != NUL && current_menu->لڑیاں[idx][1] != NUL)
		ins_typebuf(current_menu->لڑیاں[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#اگر_ختم

	toscroll = 0;
	انتخاب (c)
	{
	حالت BS:		// scroll one line back
	حالت K_BS:
	حالت 'k':
	حالت K_اوپر:
	    toscroll = -1;
	    توڑو;

	حالت CAR:		// one extra line
	حالت NL:
	حالت 'j':
	حالت K_نیچے_کو:
	    toscroll = 1;
	    توڑو;

	حالت 'u':		// Up half a page
	    toscroll = -(صفیں / 2);
	    توڑو;

	حالت 'd':		// Down half a page
	    toscroll = صفیں / 2;
	    توڑو;

	حالت 'b':		// one page back
	حالت Ctrl_B:
	حالت K_PAGEUP:
	    toscroll = -(صفیں - 1);
	    توڑو;

	حالت ' ':		// one extra page
	حالت 'f':
	حالت Ctrl_F:
	حالت K_PAGEDOWN:
	حالت K_LEFTMOUSE:
	    toscroll = صفیں - 1;
	    توڑو;

	حالت 'g':		// all the way back to the start
	    toscroll = -999999;
	    توڑو;

	حالت 'G':		// all the way to the end
	    toscroll = 999999;
	    سطریں_بایاں = 999999;
	    توڑو;

	حالت ':':		// start new command line
#اگر_تعریف_شدہ FEAT_CON_DIALOG
	    اگر (!confirm_پیغام_مستعمل)
#اگر_ختم
	    {
		// Since got_int is set all typeahead will be flushed, but we
		// want to keep this ':', remember that in a special way.
		typeahead_noflush(':');
#اگر_تعریف_شدہ FEAT_TERMINAL
		skip_رکن_loop = TRUE;
#اگر_ختم
		cmdline_صف = صفیں - 1;		// put ':' on this line
		skip_redraw = TRUE;		// skip redraw once
		need_wait_return = FALSE;	// don't wait in main()
	    }
	    // FALLTHROUGH
	حالت 'q':		// quit
	حالت Ctrl_C:
	حالت ESC:
#اگر_تعریف_شدہ FEAT_CON_DIALOG
	    اگر (confirm_پیغام_مستعمل)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    ورنہ
#اگر_ختم
	    {
		got_int = TRUE;
		quit_مزید = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    سطریں_بایاں = صفیں - 1;
	    توڑو;

#اگر_تعریف_شدہ FEAT_CLIPBOARD
	حالت Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    اگر (clip_ستارہ.state == SELECT_مکمل)
		clip_copy_modeless_selection(TRUE);
	    جاری;
#اگر_ختم
	طےشدہ:		// no valid response
	    پیغام_moremsg(TRUE);
	    جاری;
	}

	اگر (toscroll != 0)
	{
	    اگر (toscroll < 0)
	    {
		// go to start of last line
		اگر (mp_آخری == تہی)
		    mp = پیغام_sb_آغاز(آخری_msgchunk);
		ورنہ اگر (mp_آخری->sb_پچھلا != تہی)
		    mp = پیغام_sb_آغاز(mp_آخری->sb_پچھلا);
		ورنہ
		    mp = تہی;

		// go to start of line at top of the screen
		برائے (i = 0; i < صفیں - 2 && mp != تہی && mp->sb_پچھلا != تہی;
									  ++i)
		    mp = پیغام_sb_آغاز(mp->sb_پچھلا);

		اگر (mp != تہی && mp->sb_پچھلا != تہی)
		{
		    // Find line to be displayed at top.
		    برائے (i = 0; i > toscroll; --i)
		    {
			اگر (mp == تہی || mp->sb_پچھلا == تہی)
			    توڑو;
			mp = پیغام_sb_آغاز(mp->sb_پچھلا);
			اگر (mp_آخری == تہی)
			    mp_آخری = پیغام_sb_آغاز(آخری_msgchunk);
			ورنہ
			    mp_آخری = پیغام_sb_آغاز(mp_آخری->sb_پچھلا);
		    }

		    اگر (toscroll == -1 && screen_ins_سطریں(0, 0, 1,
						     (صحیح)صفیں, 0, تہی) == OK)
		    {
			// display line at top
			(خالی)disp_sb_سطر(0, mp, FALSE);
		    }
		    ورنہ
		    {
			صحیح did_clear = screenclear();

			// redisplay all lines
			برائے (i = 0; mp != تہی && i < صفیں - 1; ++i)
			{
			    mp = disp_sb_سطر(i, mp, !did_clear);
			    ++پیغام_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    ورنہ
	    {
		// First display any text that we scrolled back.
		جب (toscroll > 0 && mp_آخری != تہی)
		{
		    // scroll up, display line at bottom
		    پیغام_scroll_اوپر();
		    inc_پیغام_scrolled();
		    screen_fill((صحیح)صفیں - 2, (صحیح)صفیں - 1,
			    cmdline_ستون_off, cmdline_ستون_off + cmdline_چوڑائی,
			    ' ', ' ', پیغام_attr);
		    mp_آخری = disp_sb_سطر((صحیح)صفیں - 2, mp_آخری, FALSE);
		    --toscroll;
		}
	    }

	    اگر (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((صحیح)صفیں - 1, (صحیح)صفیں, cmdline_ستون_off,
			cmdline_ستون_off + cmdline_چوڑائی, ' ', ' ', پیغام_attr);
		پیغام_moremsg(FALSE);
		جاری;
	    }

	    // display more text, return to caller
	    سطریں_بایاں = toscroll;
	}

	توڑو;
    }

    // clear the --more-- message
    screen_fill((صحیح)صفیں - 1, (صحیح)صفیں, cmdline_ستون_off,
	    cmdline_ستون_off + cmdline_چوڑائی, ' ', ' ', پیغام_attr);
    State = oldState;
    setmouse();
    اگر (quit_مزید)
    {
	پیغام_صف = صفیں - 1;
	پیغام_ستون = 0;
    }
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    ورنہ اگر (cmdmsg_rl)
	پیغام_ستون = cmdline_چوڑائی - 1;
#اگر_ختم

    entered = FALSE;
#اگر_تعریف_شدہ FEAT_CON_DIALOG
    واپس retval;
#ورنہ
    واپس FALSE;
#اگر_ختم
}

#اگر defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_پیغام
#  undef mch_پیغام
# endif

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ساکن خالی
mch_errmsg_c(حرف *str)
{
    DWORD   nwrite = 0;
    DWORD   منوال = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    اگر (Get_Console_منوال(h, &منوال) && enc_codepage >= 0
	    && (صحیح)GetConsoleCP() != enc_codepage)
    {
	صحیح	len = (صحیح)STRLEN(str);
	WCHAR	*w = enc_مقصد_utf16((char_u *)str, &len);

	اگر (w != تہی)
	{
	    WriteConsoleW(h, w, len, &nwrite, تہی);
	    vim_free(w);
	}
    }
    ورنہ
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
    خالی
mch_errmsg(حرف *str)
{
# اگر !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    صحیح		len;
# endif

# اگر (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    اگر (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   ورنہ
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.اندر_use || gui.starting)
#  endif
	    )
    {
	fprintf(stderr, "%s", str);
	واپس;
    }
# endif

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    اگر (!(gui.اندر_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	واپس;
    }
# endif

# اگر !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (صحیح)STRLEN(str) + 1;
    اگر (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    اگر (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_ڈیٹا + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_ڈیٹا + error_ga.ga_len;
	    برائے (;;)
	    {
		p = vim_strchr(p, '\r');
		اگر (p == تہی)
		    توڑو;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    ساکن خالی
mch_پیغام_c(حرف *str)
{
    DWORD   nwrite = 0;
    DWORD   منوال;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    اگر (Get_Console_منوال(h, &منوال) && enc_codepage >= 0
	    && (صحیح)GetConsoleCP() != enc_codepage)
    {
	صحیح	len = (صحیح)STRLEN(str);
	WCHAR	*w = enc_مقصد_utf16((char_u *)str, &len);

	اگر (w != تہی)
	{
	    WriteConsoleW(h, w, len, &nwrite, تہی);
	    vim_free(w);
	}
    }
    ورنہ
    {
	لکھو("%s", str);
    }
}
# endif

/*
 * Give a message.  To be used when the screen hasn't been initialized yet.
 * When there is no tty, collect messages until the GUI has started and they
 * can be displayed in a message box.
 */
    خالی
mch_پیغام(حرف *str)
{
# اگر (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    اگر (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   ورنہ
	    isatty(2)
#   endif
#   ifdef FEAT_GUI
	    ||
#   endif
#  endif
#  ifdef FEAT_GUI
	    !(gui.اندر_use || gui.starting)
#  endif
	    )
    {
	لکھو("%s", str);
	واپس;
    }
# endif

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    اگر (!(gui.اندر_use || gui.starting))
#  endif
    {
	mch_پیغام_c(str);
	واپس;
    }
# endif
# اگر !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#اگر_ختم // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    ساکن خالی
پیغام_screen_putchar(صحیح c, صحیح attr)
{
    پیغام_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, پیغام_صف, cmdline_ستون_off + پیغام_ستون, attr);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (cmdmsg_rl)
    {
	اگر (--پیغام_ستون == 0)
	{
	    پیغام_ستون = cmdline_چوڑائی;
	    ++پیغام_صف;
	}
    }
    ورنہ
#اگر_ختم
    {
	اگر (++پیغام_ستون >= cmdline_چوڑائی)
	{
	    پیغام_ستون = 0;
	    ++پیغام_صف;
	}
    }
}

    ساکن خالی
پیغام_moremsg(صحیح بھرا)
{
    صحیح		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (صحیح)صفیں - 1, cmdline_ستون_off, attr);
    اگر (بھرا)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(صحیح)صفیں - 1, cmdline_ستون_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    خالی
repeat_پیغام(خالی)
{
    اگر (State == منوال_ASKMORE)
    {
	پیغام_moremsg(TRUE);	// display --more-- message again
	پیغام_صف = صفیں - 1;
    }
#اگر_تعریف_شدہ FEAT_CON_DIALOG
    ورنہ اگر (State == منوال_CONFIRM)
    {
	display_confirm_پیغام();	// display ":confirm" message again
	پیغام_صف = صفیں - 1;
    }
#اگر_ختم
    ورنہ اگر (State == منوال_EXTERNCMD)
    {
	windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون); // put cursor back
    }
    ورنہ اگر (State == منوال_HITRETURN || State == منوال_SETWSIZE)
    {
	اگر (پیغام_صف == صفیں - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    پیغام_didout = FALSE;
	    پیغام_ستون = 0;
	    پیغام_clr_eos();
	}
	کامیابی_return_پیغام();
	پیغام_صف = صفیں - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    ساکن صحیح
پیغام_check_screen(خالی)
{
    اگر (!بھرا_screen || !screen_valid(FALSE))
	واپس FALSE;

    اگر (پیغام_صف >= صفیں)
	پیغام_صف = صفیں - 1;
    اگر (پیغام_ستون >= cmdline_چوڑائی)
	پیغام_ستون = cmdline_چوڑائی - 1;
    واپس TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    خالی
پیغام_clr_eos(خالی)
{
    اگر (پیغام_silent == 0)
	پیغام_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    خالی
پیغام_clr_eos_force(خالی)
{
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
    اگر (اندر_echowindow)
	واپس;  // messages go into a popup
#اگر_ختم
    اگر (پیغام_use_printf())
    {
	اگر (بھرا_screen)	// only when termcap codes are valid
	{
	    اگر (*T_CD)
		برآمد_str(T_CD);	// clear to end of display
	    ورنہ اگر (*T_CE)
		برآمد_str(T_CE);	// clear to end of line
	}
    }
    ورنہ
    {
	صحیح پیغام_attr;

	پیغام_attr = HL_ATTR(HLF_پیغام);

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (cmdmsg_rl)
	{
	    screen_fill(پیغام_صف, پیغام_صف + 1,
		    cmdline_ستون_off, cmdline_ستون_off + پیغام_ستون + 1,
		    ' ', ' ', پیغام_attr);
	    screen_fill(پیغام_صف + 1, (صحیح)صفیں,
		    cmdline_ستون_off, cmdline_ستون_off + cmdline_چوڑائی,
		    ' ', ' ', پیغام_attr);
	}
	ورنہ
#اگر_ختم
	{
	    screen_fill(پیغام_صف, پیغام_صف + 1,
		    cmdline_ستون_off + پیغام_ستون, cmdline_ستون_off + cmdline_چوڑائی,
		    ' ', ' ', پیغام_attr);
	    screen_fill(پیغام_صف + 1, (صحیح)صفیں,
		    cmdline_ستون_off, cmdline_ستون_off + cmdline_چوڑائی,
		    ' ', ' ', پیغام_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    خالی
پیغام_clr_cmdline(خالی)
{
    پیغام_صف = cmdline_صف;
    پیغام_ستون = 0;
    پیغام_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    صحیح
پیغام_انتہا(خالی)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    اگر (!exiting && need_wait_return && !(State & منوال_CMDLINE))
    {
	wait_return(FALSE);
	واپس FALSE;
    }
    برآمد_flush();
    واپس TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    خالی
پیغام_check(خالی)
{
    اگر (پیغام_صف == صفیں - 1 && پیغام_ستون >= sc_ستون
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
		&& !اندر_echowindow
#اگر_ختم
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
    ساکن خالی
redir_write(char_u *str, صحیح maxlen)
{
    char_u	*s = str;
    ساکن صحیح	cur_ستون = 0;

    // Don't do anything for displaying prompts and the like.
    اگر (redir_off)
	واپس;

    // If 'verbosefile' is set prepare for writing in that file.
    اگر (*p_vfile != NUL && verbose_fd == تہی)
	verbose_open();

    اگر (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	اگر (*s != '\n' && *s != '\r')
	{
	    جب (cur_ستون < پیغام_ستون)
	    {
#اگر_تعریف_شدہ FEAT_EVAL
		اگر (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		ورنہ اگر (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		ورنہ اگر (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		ورنہ
#اگر_ختم
		    اگر (redir_fd != تہی)
		    fputs(" ", redir_fd);
		اگر (verbose_fd != تہی)
		    fputs(" ", verbose_fd);
		++cur_ستون;
	    }
	}

#اگر_تعریف_شدہ FEAT_EVAL
	اگر (redir_execute)
	    execute_redir_str(s, maxlen);
	ورنہ اگر (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	ورنہ اگر (redir_vname)
	    var_redir_str(s, maxlen);
#اگر_ختم

	// Write and adjust the current column.
	جب (*s != NUL && (maxlen < 0 || (صحیح)(s - str) < maxlen))
	{
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (!redir_reg && !redir_vname && !redir_execute)
#اگر_ختم
		اگر (redir_fd != تہی)
		    putc(*s, redir_fd);
	    اگر (verbose_fd != تہی)
		putc(*s, verbose_fd);
	    اگر (*s == '\r' || *s == '\n')
		cur_ستون = 0;
	    ورنہ اگر (*s == '\t')
		cur_ستون += (8 - cur_ستون % 8);
	    ورنہ
		++cur_ستون;
	    ++s;
	}

	اگر (پیغام_silent != 0)	// should update msg_col
	    پیغام_ستون = cur_ستون;
    }
}

    صحیح
redirecting(خالی)
{
    واپس redir_fd != تہی || *p_vfile != NUL
#اگر_تعریف_شدہ FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#اگر_ختم
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    خالی
verbose_enter(خالی)
{
    اگر (*p_vfile != NUL)
	++پیغام_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    خالی
verbose_leave(خالی)
{
    اگر (*p_vfile != NUL)
	اگر (--پیغام_silent < 0)
	    پیغام_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    خالی
verbose_enter_scroll(خالی)
{
    اگر (*p_vfile != NUL)
	++پیغام_silent;
    ورنہ
	// always scroll up, don't overwrite
	پیغام_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    خالی
verbose_leave_scroll(خالی)
{
    اگر (*p_vfile != NUL)
    {
	اگر (--پیغام_silent < 0)
	    پیغام_silent = 0;
    }
    ورنہ
	cmdline_صف = پیغام_صف;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    خالی
verbose_stop(خالی)
{
    اگر (verbose_fd != تہی)
    {
	fclose(verbose_fd);
	verbose_fd = تہی;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    صحیح
verbose_open(خالی)
{
    اگر (verbose_fd == تہی && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((حرف *)p_vfile, "a");
	اگر (verbose_fd == تہی)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    واپس FAIL;
	}
    }
    واپس OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    خالی
give_warning(char_u *پیغام_2, صحیح hl)
{
    give_warning_with_ماخذ(پیغام_2, hl, FALSE);
}

    خالی
give_warning_with_ماخذ(char_u *پیغام_2, صحیح hl, صحیح with_ماخذ)
{
    // Don't do this for ":silent".
    اگر (پیغام_silent != 0)
	واپس;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#اگر_تعریف_شدہ FEAT_EVAL
    سیٹ_vim_var_string(VV_WARNINGMSG, پیغام_2, -1);
#اگر_ختم
    VIM_CLEAR(keep_پیغام);
    اگر (hl)
	keep_پیغام_attr = HL_ATTR(HLF_W);
    ورنہ
	keep_پیغام_attr = 0;

    اگر (with_ماخذ)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	پیغام_آغاز();
	پیغام_ماخذ(HL_ATTR(HLF_W));
	پیغام_puts(" ");
	پیغام_puts_attr((حرف *)پیغام_2, HL_ATTR(HLF_W) | پیغام_HIST);
	پیغام_clr_eos();
	(خالی)پیغام_انتہا();
    }
    ورنہ اگر (پیغام_attr((حرف *)پیغام_2, keep_پیغام_attr) && پیغام_scrolled == 0)
	سیٹ_keep_پیغام(پیغام_2, keep_پیغام_attr);

    پیغام_didout = FALSE;	    // overwrite this message
    پیغام_nowait = TRUE;	    // don't wait for this message
    پیغام_ستون = 0;

    --no_wait_return;
}

#اگر defined(FEAT_EVAL)
    خالی
give_warning2(char_u *پیغام_2, char_u *a1, صحیح hl)
{
    اگر (IObuff == تہی)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(پیغام_2, hl);
    }
    ورنہ
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, (حرف *)پیغام_2, a1);
	give_warning(IObuff, hl);
    }
}
#اگر_ختم

/*
 * Advance msg cursor to column "col".
 */
    خالی
پیغام_advance(صحیح ستون)
{
    اگر (پیغام_silent != 0)	// nothing to advance to
    {
	پیغام_ستون = ستون;		// for redirection, may fill it up later
	واپس;
    }
    اگر (ستون >= cmdline_چوڑائی)	// not enough room
	ستون = cmdline_چوڑائی - 1;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (cmdmsg_rl)
	جب (پیغام_ستون > cmdline_چوڑائی - ستون)
	    پیغام_putchar(' ');
    ورنہ
#اگر_ختم
	جب (پیغام_ستون < ستون)
	    پیغام_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    خالی
پیغام_warn_missing_clipboard(خالی)
{
    اگر (!global_busy && !did_warn_clipboard && silence_w23_w24_پیغام == 0)
    {
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	پیغام(_("W23: Clipboard register not available, using register 0"));
#ورنہ
	پیغام(_("W24: Clipboard register not available. See :h W24"));
#اگر_ختم
	did_warn_clipboard = TRUE;
    }
}

#اگر defined(FEAT_CON_DIALOG)
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
    صحیح
do_dialog(
    صحیح		type غیر_مستعمل,
    char_u	*title غیر_مستعمل,
    char_u	*پیغام_2,
    char_u	*buttons,
    صحیح		dfltbutton,
    char_u	*textfield غیر_مستعمل,	// IObuff for inputdialog(), NULL
					// otherwise
    صحیح		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    صحیح		oldState;
    صحیح		retval = 0;
    char_u	*hotkeys;
    صحیح		c;
    صحیح		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    اگر (silent_منوال)
	واپس dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    اگر (gui.اندر_use && vim_strchr(p_go, GO_CONDIALOG) == تہی)
    {
	// --gui-dialog-file: write text to a file
	اگر (gui_dialog_log(title, پیغام_2))
	    c = dfltbutton;
	ورنہ
	    c = gui_mch_dialog(type, title, پیغام_2, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_صف = پیغام_صف;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	برآمد_flush();
	gui_mch_update();

	واپس c;
    }
# endif

    oldState = State;
    State = منوال_CONFIRM;
    setmouse();

    // Ensure raw mode here.
    save_tmode = cur_tmode;
    settmode(TMODE_RAW);

    /*
     * Since we wait for a keypress, don't make the
     * user press RETURN as well afterwards.
     */
    ++no_wait_return;
    hotkeys = پیغام_show_console_dialog(پیغام_2, buttons, dfltbutton);

    اگر (hotkeys != تہی)
    {
	برائے (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    انتخاب (c)
	    {
	    حالت CAR:		// User accepts default option
	    حالت NL:
		retval = dfltbutton;
		توڑو;
	    حالت Ctrl_C:	// User aborts/cancels
	    حالت ESC:
		retval = 0;
		توڑو;
	    طےشدہ:		// Could be a hotkey?
		اگر (c < 0)	// special keys are ignored here
		    جاری;
		اگر (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    توڑو;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		برائے (i = 0; hotkeys[i]; ++i)
		{
		    اگر (has_mbyte)
		    {
			اگر ((*mb_ptr2char)(hotkeys + i) == c)
			    توڑو;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    ورنہ
			اگر (hotkeys[i] == c)
			    توڑو;
		    ++retval;
		}
		اگر (hotkeys[i])
		    توڑو;
		// No hotkey match, so keep waiting
		جاری;
	    }
	    توڑو;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    پیغام_انتہا_prompt();

    واپس retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    ساکن صحیح
copy_char(
    char_u	*from,
    char_u	*مقصد,
    صحیح		lowercase)	// make character lower case
{
    صحیح		len;
    صحیح		c;

    اگر (has_mbyte)
    {
	اگر (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    واپس (*mb_char2bytes)(c, مقصد);
	}
	ورنہ
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(مقصد, from, (size_t)len);
	    واپس len;
	}
    }
    ورنہ
    {
	اگر (lowercase)
	    *مقصد = (char_u)TOLOWER_LOC(*from);
	ورنہ
	    *مقصد = *from;
	واپس 1;
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
    ساکن char_u *
پیغام_show_console_dialog(
    char_u	*پیغام_2,
    char_u	*buttons,
    صحیح		dfltbutton)
{
    صحیح		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    صحیح		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = تہی;
    char_u	*msgp = تہی;
    char_u	*hotkp = تہی;
    char_u	*r;
    صحیح		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    صحیح		پہلا_hotkey = FALSE;	// first char of button is hotkey
    صحیح		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    برائے (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	جب (*r)
	{
	    اگر (*r == DLG_BUTTON_SEP)
	    {
		اگر (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    اگر (has_mbyte)
			hotkp += STRLEN(hotkp);
		    ورنہ
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    اگر (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    اگر (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			پہلا_hotkey = TRUE;
		}
		ورنہ
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    اگر (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    ورنہ اگر (*r == DLG_HOTKEY_CHAR || پہلا_hotkey)
	    {
		اگر (*r == DLG_HOTKEY_CHAR)
		    ++r;
		پہلا_hotkey = FALSE;
		اگر (copy)
		{
		    اگر (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    ورنہ
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		ورنہ
		{
		    ++len;	    // '&a' -> '[a]'
		    اگر (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    ورنہ
	    {
		// everything else copy literally
		اگر (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	اگر (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	ورنہ
	{
	    len += (صحیح)(STRLEN(پیغام_2)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    اگر (!has_hotkey[0])
	    {
		پہلا_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_پیغام);
	    confirm_پیغام = alloc(len);
	    اگر (confirm_پیغام == تہی)
		واپس تہی;
	    *confirm_پیغام = NUL;
	    hotk = alloc(lenhotkey);
	    اگر (hotk == تہی)
		واپس تہی;

	    *confirm_پیغام = '\n';
	    STRCPY(confirm_پیغام + 1, پیغام_2);

	    msgp = confirm_پیغام + 1 + STRLEN(پیغام_2);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_پیغام_دم = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_پیغام();
    واپس hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    ساکن خالی
display_confirm_پیغام(خالی)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_پیغام_مستعمل;
    اگر (confirm_پیغام != تہی)
	پیغام_puts_attr((حرف *)confirm_پیغام, HL_ATTR(HLF_M));
    --confirm_پیغام_مستعمل;
}

#اگر_ختم // FEAT_CON_DIALOG

#اگر defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    صحیح
vim_dialog_yesno(
    صحیح		type,
    char_u	*title,
    char_u	*پیغام_2,
    صحیح		dflt)
{
    اگر (do_dialog(type,
		title == تہی ? (char_u *)_("Question") : title,
		پیغام_2,
		(char_u *)_("&Yes\n&No"), dflt, تہی, FALSE) == 1)
	واپس VIM_YES;
    واپس VIM_NO;
}

    صحیح
vim_dialog_yesnocancel(
    صحیح		type,
    char_u	*title,
    char_u	*پیغام_2,
    صحیح		dflt)
{
    انتخاب (do_dialog(type,
		title == تہی ? (char_u *)_("Question") : title,
		پیغام_2,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, تہی, FALSE))
    {
	حالت 1: واپس VIM_YES;
	حالت 2: واپس VIM_NO;
    }
    واپس VIM_CANCEL;
}

    صحیح
vim_dialog_yesnoallcancel(
    صحیح		type,
    char_u	*title,
    char_u	*پیغام_2,
    صحیح		dflt)
{
    انتخاب (do_dialog(type,
		title == تہی ? (char_u *)"Question" : title,
		پیغام_2,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, تہی, FALSE))
    {
	حالت 1: واپس VIM_YES;
	حالت 2: واپس VIM_NO;
	حالت 3: واپس VIM_ALL;
	حالت 4: واپس VIM_DISCARDALL;
    }
    واپس VIM_CANCEL;
}

#اگر_ختم // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
