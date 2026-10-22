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

#include "vim.h"

static void add_msg_hist(char_u *s, ကိန်းပြည့် len, ကိန်းပြည့် attr);
static void check_msg_hist(void);
static void hit_return_msg(void);
static void msg_home_replace_attr(char_u *fname, ကိန်းပြည့် attr);
static void msg_puts_attr_len(var_na *str, ကိန်းပြည့် maxlen, ကိန်းပြည့် attr);
static void msg_puts_display(char_u *str, ကိန်းပြည့် maxlen, ကိန်းပြည့် attr, ကိန်းပြည့် recurse);
static void msg_scroll_up(void);
static void inc_msg_scrolled(void);
static void store_sb_text(char_u **sb_str, char_u *s, ကိန်းပြည့် attr, ကိန်းပြည့် *sb_col, ကိန်းပြည့် finish);
static void t_puts(ကိန်းပြည့် *t_col, char_u *t_s, char_u *s, ကိန်းပြည့် attr);
static void msg_puts_printf(char_u *str, ကိန်းပြည့် maxlen);
static ကိန်းပြည့် do_more_prompt(ကိန်းပြည့် typed_char);
static void msg_screen_putchar(ကိန်းပြည့် c, ကိန်းပြည့် attr);
static void msg_moremsg(ကိန်းပြည့် full);
static ကိန်းပြည့်  msg_check_screen(void);
static void redir_write(char_u *s, ကိန်းပြည့် maxlen);
#ifdef FEAT_CON_DIALOG
static char_u *msg_show_console_dialog(char_u *message, char_u *buttons, ကိန်းပြည့် dfltbutton);
static ကိန်းပြည့်	confirm_msg_used = FALSE;	// displaying confirm_msg
static char_u	*confirm_msg = NULL;		// ":confirm" message
static char_u	*confirm_msg_tail;		// tail of confirm_msg
static void display_confirm_msg(void);
#endif
#ifdef FEAT_EVAL
static ကိန်းပြည့် emsg_to_channel_log = FALSE;
#endif

sa_nracanaa msg_hist
{
    sa_nracanaa msg_hist	*next;
    char_u		*msg;
    ကိန်းပြည့်			attr;
};

static sa_nracanaa msg_hist *first_msg_hist = NULL;
static sa_nracanaa msg_hist *last_msg_hist = NULL;
static ကိန်းပြည့် msg_hist_len = 0;
static ကိန်းပြည့် msg_hist_max = 500;		// The default max value is 500

// flags obtained from the 'messagesopt' option
#define MESSAGES_HIT_ENTER	0x001
#define MESSAGES_WAIT		0x002
#define MESSAGES_HISTORY	0x004

// args in 'messagesopt' option
#define MESSAGES_OPT_HIT_ENTER "hit-enter"
#define MESSAGES_OPT_WAIT "wait:"
#define MESSAGES_OPT_HISTORY "history:"

// The default is "hit-enter,history:500"
static ကိန်းပြည့် msg_flags = MESSAGES_HIT_ENTER | MESSAGES_HISTORY;
static ကိန်းပြည့် msg_wait = 0;

static FILE *verbose_fd = NULL;
static ကိန်းပြည့်  verbose_did_open = FALSE;

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
    ကိန်းပြည့်
msg(var_na *s)
{
    ပြန်ပေး msg_attr_keep(s, 0, FALSE);
}

/*
 * Like msg() but keep it silent when 'verbosefile' is set.
 */
    ကိန်းပြည့်
verb_msg(var_na *s)
{
    ကိန်းပြည့်		n;

    verbose_enter();
    n = msg_attr_keep(s, 0, FALSE);
    verbose_leave();

    ပြန်ပေး n;
}

    ကိန်းပြည့်
msg_attr(var_na *s, ကိန်းပြည့် attr)
{
    ပြန်ပေး msg_attr_keep(s, attr, FALSE);
}

    ကိန်းပြည့်
msg_attr_keep(
    var_na	*s,
    ကိန်းပြည့်		attr,
    ကိန်းပြည့်		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    static ကိန်းပြည့်	entered = 0;
    ကိန်းပြည့်		retval;
    char_u	*buf = NULL;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    အကယ်၍ (!emsg_on_display && message_filtered((char_u *)s))
	ပြန်ပေး TRUE;

#ifdef FEAT_EVAL
    အကယ်၍ (attr == 0)
	set_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#endif

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    အကယ်၍ (entered >= 3)
	ပြန်ပေး TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    အကယ်၍ ((char_u *)s != keep_msg
	    || (*s != '<'
		&& last_msg_hist != NULL
		&& last_msg_hist->msg != NULL
		&& STRCMP(s, last_msg_hist->msg) != 0))
	add_msg_hist((char_u *)s, -1, attr);

#ifdef FEAT_EVAL
    အကယ်၍ (emsg_to_channel_log)
	// Write message in the channel log.
	ch_log(NULL, "ERROR: %s", s);
#endif

    // Truncate the message if needed.
    msg_start();
    buf = msg_strtrunc((char_u *)s, FALSE);
    အကယ်၍ (buf != NULL)
	s = (var_na *)buf;

    msg_outtrans_attr((char_u *)s, attr);
    msg_clr_eos();
    retval = msg_end();

    အကယ်၍ (keep && retval && vim_strsize((char_u *)s)
		    < (ကိန်းပြည့်)(Rows - cmdline_row - 1) * cmdline_width + sc_col)
	set_keep_msg((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    ပြန်ပေး retval;
}

/*
 * Truncate a string such that it can be printed without causing a scroll.
 * Returns an allocated string or NULL when no truncating is done.
 */
    char_u *
msg_strtrunc(
    char_u	*s,
    ကိန်းပြည့်		force)	    // always truncate
{
    char_u	*buf = NULL;
    ကိန်းပြည့်		len;
    ကိန်းပြည့်		room;

    // May truncate message to avoid a hit-return prompt
    အကယ်၍ ((!msg_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_active && msg_silent == 0) || force)
    {
	len = vim_strsize(s);
	အကယ်၍ (msg_scrolled != 0
#ifdef HAS_MESSAGE_WINDOW
		|| in_echowindow
#endif
		)
	    // Use all the columns.
	    room = (ကိန်းပြည့်)(Rows - msg_row) * cmdline_width - 1;
	မဟုတ်ပါက
	    // Use up to 'showcmd' column.
	    room = (ကိန်းပြည့်)(Rows - msg_row - 1) * cmdline_width + sc_col - 1;
	အကယ်၍ (len > room && room > 0)
	{
	    အကယ်၍ (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (room + 2) * 18;
	    မဟုတ်ပါက အကယ်၍ (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (room + 2) * 2;
	    မဟုတ်ပါက
		len = room + 2;
	    buf = alloc(len);
	    အကယ်၍ (buf != NULL)
		trunc_string(s, buf, room, len);
	}
    }
    ပြန်ပေး buf;
}

/*
 * Truncate a string "s" to "buf" with cell width "room".
 * "s" and "buf" may be equal.
 */
    void
trunc_string(
    char_u	*s,
    char_u	*buf,
    ကိန်းပြည့်		room_in,
    ကိန်းပြည့်		buflen)
{
    size_t	room = room_in - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    ကိန်းပြည့်		e;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		n;

    အကယ်၍ (*s == NUL)
    {
	အကယ်၍ (buflen > 0)
	    *buf = NUL;
	ပြန်ပေး;
    }

    အကယ်၍ (room_in < 3)
	room = 0;
    half = room / 2;

    // First part: Start of the string.
    အတွက် (e = 0; len < half && e < buflen; ++e)
    {
	အကယ်၍ (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    ပြန်ပေး;
	}
	n = ptr2cells(s + e);
	အကယ်၍ (len + n > half)
	    bha_nga;
	len += n;
	buf[e] = s[e];
	အကယ်၍ (has_mbyte)
	    အတွက် (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		အကယ်၍ (++e == buflen)
		    bha_nga;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    အကယ်၍ (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	နေစဉ် (len + n > room)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    မဟုတ်ပါက အကယ်၍ (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (ကိန်းပြည့်)STRLEN(s);
	အတွက် (;;)
	{
	    do
		half = half - utf_head_off(s, s + half - 1) - 1;
	    နေစဉ် (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    အကယ်၍ (len + n > room || half == 0)
		bha_nga;
	    len += n;
	    i = (ကိန်းပြည့်)half;
	}
    }
    မဟုတ်ပါက
    {
	အတွက် (i = (ကိန်းပြည့်)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= room; --i)
	    len += n;
    }


    အကယ်၍ (i <= e + 3)
    {
	// text fits without truncating
	အကယ်၍ (s != buf)
	{
	    len = STRLEN(s);
	    အကယ်၍ (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    အကယ်၍ (len < 1)
		buf[e - 1] = NUL;
	    မဟုတ်ပါက
		mch_memmove(buf + e, s + e, len);
	}
    }
    မဟုတ်ပါက အကယ်၍ (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	အကယ်၍ (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    မဟုတ်ပါက
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

ကိန်းပြည့် vim_snprintf(var_na *str, size_t str_m, const var_na *fmt, ...);

    ကိန်းပြည့်
smsg(const var_na *s, ...)
{
    အကယ်၍ (IObuff == NULL)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ပြန်ပေး msg((var_na *)s);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((var_na *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ပြန်ပေး msg((var_na *)IObuff);
}

    ကိန်းပြည့်
smsg_attr(ကိန်းပြည့် attr, const var_na *s, ...)
{
    အကယ်၍ (IObuff == NULL)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ပြန်ပေး msg_attr((var_na *)s, attr);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((var_na *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ပြန်ပေး msg_attr((var_na *)IObuff, attr);
}

    ကိန်းပြည့်
smsg_attr_keep(ကိန်းပြည့် attr, const var_na *s, ...)
{
    အကယ်၍ (IObuff == NULL)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ပြန်ပေး msg_attr_keep((var_na *)s, attr, TRUE);
    }

    va_list arglist;

    va_start(arglist, s);
    vim_vsnprintf((var_na *)IObuff, IOSIZE, s, arglist);
    va_end(arglist);
    ပြန်ပေး msg_attr_keep((var_na *)IObuff, attr, TRUE);
}

#endif

/*
 * Remember the last sourcing name/lnum used in an error message, so that it
 * isn't printed each time when it didn't change.
 */
static ကိန်းပြည့်	last_sourcing_lnum = 0;
static char_u   *last_sourcing_name = NULL;

/*
 * Reset the last used sourcing name/lnum.  Makes sure it is displayed again
 * for the next error message;
 */
    void
reset_last_sourcing(void)
{
    VIM_CLEAR(last_sourcing_name);
    last_sourcing_lnum = 0;
}

/*
 * Return TRUE if "SOURCING_NAME" differs from "last_sourcing_name".
 */
    static ကိန်းပြည့်
other_sourcing_name(void)
{
    အကယ်၍ (HAVE_SOURCING_INFO && SOURCING_NAME != NULL)
    {
	အကယ်၍ (last_sourcing_name != NULL)
	    ပြန်ပေး STRCMP(SOURCING_NAME, last_sourcing_name) != 0;
	ပြန်ပေး TRUE;
    }
    ပြန်ပေး FALSE;
}

/*
 * Get the message about the source, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    static char_u *
get_emsg_source(void)
{
    char_u	*Buf, *p;

    အကယ်၍ (HAVE_SOURCING_INFO && SOURCING_NAME != NULL && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	အကယ်၍ (sname == NULL)
	    sname = SOURCING_NAME;

#ifdef FEAT_EVAL
	အကယ်၍ (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	မဟုတ်ပါက
#endif
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	အကယ်၍ (Buf != NULL)
	    sprintf((var_na *)Buf, (var_na *)p, sname);
	vim_free(tofree);
	ပြန်ပေး Buf;
    }
    ပြန်ပေး NULL;
}

/*
 * Get the message about the source lnum, as used for an error message.
 * Returns an allocated string with room for one more character.
 * Returns NULL when no message is to be given.
 */
    static char_u *
get_emsg_lnum(void)
{
    char_u	*Buf, *p;

    // lnum is 0 when executing a command from the command line
    // argument, we don't want a line number then
    အကယ်၍ (SOURCING_NAME != NULL
	    && (other_sourcing_name() || SOURCING_LNUM != last_sourcing_lnum)
	    && SOURCING_LNUM != 0)
    {
	p = (char_u *)_("line %4ld:");
	Buf = alloc(STRLEN(p) + 20);
	အကယ်၍ (Buf != NULL)
	    sprintf((var_na *)Buf, (var_na *)p, (long)SOURCING_LNUM);
	ပြန်ပေး Buf;
    }
    ပြန်ပေး NULL;
}

/*
 * Display name and line number for the source of an error.
 * Remember the file name and line number, so that for the next error the info
 * is only displayed if it changed.
 */
    void
msg_source(ကိန်းပြည့် attr)
{
    char_u	*p;
    static ကိန်းပြည့်	recursive = FALSE;

    // Bail out if something called here causes an error.
    အကယ်၍ (recursive)
	ပြန်ပေး;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_source();
    အကယ်၍ (p != NULL)
    {
	msg_scroll = TRUE;  // this will take more than one line
	msg_attr((var_na *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    အကယ်၍ (p != NULL)
    {
	msg_attr((var_na *)p, HL_ATTR(HLF_N));
	vim_free(p);
	last_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    အကယ်၍ (SOURCING_NAME == NULL || other_sourcing_name())
    {
	VIM_CLEAR(last_sourcing_name);
	အကယ်၍ (SOURCING_NAME != NULL)
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
    static ကိန်းပြည့်
emsg_not_now(void)
{
    အကယ်၍ ((emsg_off > 0 && vim_strchr(p_debug, 'm') == NULL
					  && vim_strchr(p_debug, 't') == NULL)
#ifdef FEAT_EVAL
	    || emsg_skip > 0
#endif
	    )
	ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}

#အကယ်၍ defined(FEAT_EVAL)
static garray_T ignore_error_list = GA_EMPTY;

    void
ignore_error_for_testing(char_u *error)
{
    အကယ်၍ (ignore_error_list.ga_itemsize == 0)
	ga_init2(&ignore_error_list, parimaa_na(char_u *), 1);

    အကယ်၍ (STRCMP("RESET", error) == 0)
	ga_clear_strings(&ignore_error_list);
    မဟုတ်ပါက
	ga_copy_string(&ignore_error_list, error);
}

    static ကိန်းပြည့်
ignore_error(const var_na *msg)
{
    ကိန်းပြည့် i;

    အတွက် (i = 0; i < ignore_error_list.ga_len; ++i)
	အကယ်၍ (strstr(msg,
		  (var_na *)((char_u **)(ignore_error_list.ga_data))[i]) != NULL)
	    ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}
#endif

#အကယ်၍ !defined(HAVE_STRERROR)
/*
 * Replacement for perror() that behaves more or less like emsg() was called.
 * v:errmsg will be set and called_emsg will be incremented.
 */
    void
do_perror(var_na *msg)
{
    perror(msg);
    ++emsg_silent;
    emsg(msg);
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
    static ကိန်းပြည့်
emsg_core(const var_na *s)
{
    ကိန်းပြည့်		attr;
    char_u	*p;
    ကိန်းပြည့်		r;
#ifdef FEAT_EVAL
    ကိန်းပြည့်		ignore = FALSE;
    ကိန်းပြည့်		severe;
#endif

#ifdef FEAT_EVAL
    // When testing some errors are turned into a normal message.
    အကယ်၍ (ignore_error(s))
	// don't call msg() if it results in a dialog
	ပြန်ပေး msg_use_printf() ? FALSE : msg((var_na *)s);
#endif

    ++called_emsg;

#ifdef FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#endif

    အကယ်၍ (!emsg_off || vim_strchr(p_debug, 't') != NULL)
    {
#ifdef FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	အကယ်၍ (cause_errthrow((char_u *)s, severe, &ignore) == TRUE)
	{
	    အကယ်၍ (!ignore)
		++did_emsg;
	    ပြန်ပေး TRUE;
	}

	အကယ်၍ (in_assert_fails && emsg_assert_fails_msg == NULL)
	{
	    emsg_assert_fails_msg = vim_strsave((char_u *)s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == NULL ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
#endif

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	အကယ်၍ (emsg_silent != 0)
	{
#ifdef FEAT_EVAL
	    ++did_emsg_silent;
#endif
	    အကယ်၍ (emsg_noredir == 0)
	    {
		msg_start();
		p = get_emsg_source();
		အကယ်၍ (p != NULL)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		အကယ်၍ (p != NULL)
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
	    အကယ်၍ (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#endif
#ifdef FEAT_EVAL
	    ch_log(NULL, "ERROR silent: %s", s);
#endif
	    ပြန်ပေး TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	msg_silent = 0;
	cmd_silent = FALSE;

	အကယ်၍ (global_busy)		// break :global command
	    ++global_busy;

	အကယ်၍ (p_eb)
	    beep_flush();		// also includes flush_buffers()
	မဟုတ်ပါက
	    flush_buffers(FLUSH_MINIMAL);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ifdef FEAT_EVAL
	++uncaught_emsg;
#endif
    }

#ifdef HAS_MESSAGE_WINDOW
    အကယ်၍ (!in_echowindow)
#endif
	emsg_on_display = TRUE;	    // remember there is an error message

    attr = HL_ATTR(HLF_E);	    // set highlight mode for error messages
    အကယ်၍ (msg_scrolled != 0)
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
    msg_scroll = TRUE;
    msg_source(attr);

    /*
     * Display the error message itself.
     */
    msg_nowait = FALSE;			// wait for this msg
    r = msg_attr((var_na *)s, attr);

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = FALSE;
#endif
    ပြန်ပေး r;
}

/*
 * Print error message "s".  Should already be translated.
 * Return TRUE if wait_return() not called.
 */
    ကိန်းပြည့်
emsg(var_na *s)
{
    // Skip this if not giving error messages at the moment.
    အကယ်၍ (emsg_not_now())
	ပြန်ပေး TRUE;

    ပြန်ပေး emsg_core(s);
}

#ifndef PROTO  // manual proto with __attribute__
/*
 * Print error message "s" with format string and variable arguments.
 * "s" should already be translated.
 * Note: caller must not use "IObuff" for "s"!
 * Return TRUE if wait_return() not called.
 */
    ကိန်းပြည့်
semsg(const var_na *s, ...)
{
    // Skip this if not giving error messages at the moment.
    အကယ်၍ (emsg_not_now())
	ပြန်ပေး TRUE;

    အကယ်၍ (IObuff == NULL)
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	ပြန်ပေး emsg_core(s);

    va_list ap;

    va_start(ap, s);
    vim_vsnprintf((var_na *)IObuff, IOSIZE, s, ap);
    va_end(ap);
    ပြန်ပေး emsg_core((var_na *)IObuff);
}
#endif

/*
 * Same as emsg(...), but abort on error when ABORT_ON_INTERNAL_ERROR is
 * defined. It is used for internal errors only, so that they can be
 * detected when fuzzing vim.
 */
    void
iemsg(var_na *s)
{
    အကယ်၍ (emsg_not_now())
	ပြန်ပေး;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    emsg_core(s);
#အကယ်၍ defined(ABORT_ON_INTERNAL_ERROR) && defined(FEAT_EVAL)
    set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
    msg_putchar('\n');  // avoid overwriting the error message
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
    void
siemsg(const var_na *s, ...)
{
    အကယ်၍ (emsg_not_now())
	ပြန်ပေး;

    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    အကယ်၍ (IObuff == NULL)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	emsg_core(s);
    }
    မဟုတ်ပါက
    {
	va_list ap;

	va_start(ap, s);
	vim_vsnprintf((var_na *)IObuff, IOSIZE, s, ap);
	va_end(ap);
	emsg_core((var_na *)IObuff);
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    msg_putchar('\n');  // avoid overwriting the error message
    out_flush();
    abort();
# endif
}
#endif

/*
 * Give an "Internal error" message.
 */
    void
internal_error(var_na *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    siemsg(_(e_internal_error_str), where);
}

#အကယ်၍ defined(FEAT_EVAL)
/*
 * Like internal_error() but do not call abort(), to avoid tests using
 * test_unknown() and test_void() causing Vim to exit.
 */
    void
internal_error_no_abort(var_na *where)
{
    // Give a generic error which is translated.  The error itself may not be
    // translated, it almost never shows.
    emsg_core(_(e_internal_error_please_report_a_bug));

    semsg(_(e_internal_error_str), where);
}
#endif

// emsg3() and emsgn() are in misc2.c to avoid warnings for the prototypes.

    void
emsg_invreg(ကိန်းပြည့် name)
{
    semsg(_(e_invalid_register_name_str), transchar_buf(NULL, name));
}

#အကယ်၍ defined(FEAT_EVAL)
/*
 * Give an error message which contains %s for "name[len]".
 */
    void
emsg_namelen(var_na *msg, char_u *name, ကိန်းပြည့် len)
{
    char_u *copy = vim_strnsave(name, len);

    semsg(msg, copy == NULL ? "NULL" : (var_na *)copy);
    vim_free(copy);
}
#endif

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.  This truncates in another way as for normal messages.
 * Careful: The string may be changed by msg_may_trunc()!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    var_na *
msg_trunc_attr(var_na *s, ကိန်းပြည့် force, ကိန်းပြည့် attr)
{
    ကိန်းပြည့်		n;
    var_na	*ts;

    // Add message to history before truncating
    add_msg_hist((char_u *)s, -1, attr);

    ts = (var_na *)msg_may_trunc(force, (char_u *)s);

    msg_hist_off = TRUE;
    n = msg_attr(ts, attr);
    msg_hist_off = FALSE;

    အကယ်၍ (n)
	ပြန်ပေး ts;
    ပြန်ပေး NULL;
}

/*
 * Check if message "s" should be truncated at the start (for filenames).
 * Return a pointer to where the truncated message starts.
 * Note: May change the message by replacing a character with '<'.
 */
    char_u *
msg_may_trunc(ကိန်းပြည့် force, char_u *s)
{
    ကိန်းပြည့်		n;
    ကိန်းပြည့်		room;

    // If 'cmdheight' is zero or something unexpected happened "room" may be
    // negative.
    room = (ကိန်းပြည့်)(Rows - cmdline_row - 1) * cmdline_width + sc_col - 1;
    အကယ်၍ (room > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_active))
	    && (n = (ကိန်းပြည့်)STRLEN(s) - room) > 0)
    {
	အကယ်၍ (has_mbyte)
	{
	    ကိန်းပြည့်	size = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    အကယ်၍ (size <= room)
		ပြန်ပေး s;

	    အတွက် (n = 0; size >= room; )
	    {
		size -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    ပြန်ပေး s;
}

    static void
add_msg_hist(
    char_u	*s,
    ကိန်းပြည့်		len,		// -1 for undetermined length
    ကိန်းပြည့်		attr)
{
    sa_nracanaa msg_hist *p;

    အကယ်၍ (msg_hist_off || msg_silent != 0)
	ပြန်ပေး;

    // allocate an entry and add the message at the end of the history
    p = ALLOC_ONE(sa_nracanaa msg_hist);
    အကယ်၍ (p == NULL)
	ပြန်ပေး;

    အကယ်၍ (len < 0)
	len = (ကိန်းပြည့်)STRLEN(s);
    // remove leading and trailing newlines
    နေစဉ် (len > 0 && *s == '\n')
    {
	++s;
	--len;
    }
    နေစဉ် (len > 0 && s[len - 1] == '\n')
	--len;
    p->msg = vim_strnsave(s, len);
    p->next = NULL;
    p->attr = attr;
    အကယ်၍ (last_msg_hist != NULL)
	last_msg_hist->next = p;
    last_msg_hist = p;
    အကယ်၍ (first_msg_hist == NULL)
	first_msg_hist = last_msg_hist;
    ++msg_hist_len;

    check_msg_hist();
}

/*
 * Delete the first (oldest) message from the history.
 * Returns FAIL if there are no messages.
 */
    ကိန်းပြည့်
delete_first_msg(void)
{
    sa_nracanaa msg_hist *p;

    အကယ်၍ (msg_hist_len <= 0)
	ပြန်ပေး FAIL;
    p = first_msg_hist;
    first_msg_hist = p->next;
    အကယ်၍ (first_msg_hist == NULL)
	last_msg_hist = NULL;  // history is empty
    vim_free(p->msg);
    vim_free(p);
    --msg_hist_len;
    ပြန်ပေး OK;
}

    static void
check_msg_hist(void)
{
    // Don't let the message history get too big
    နေစဉ် (msg_hist_len > 0 && msg_hist_len > msg_hist_max)
	(void)delete_first_msg();
}


    ကိန်းပြည့်
messagesopt_changed(void)
{
    char_u	*p;
    ကိန်းပြည့်		messages_flags_new = 0;
    ကိန်းပြည့်		messages_wait_new = 0;
    ကိန်းပြည့်		messages_history_new = 0;

    p = p_mopt;
    နေစဉ် (*p != NUL)
    {
	အကယ်၍ (STRNCMP(p, MESSAGES_OPT_HIT_ENTER,
	     STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER)) == 0)
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HIT_ENTER);
	    messages_flags_new |= MESSAGES_HIT_ENTER;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, MESSAGES_OPT_WAIT,
		  STRLEN_LITERAL(MESSAGES_OPT_WAIT)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_WAIT)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_WAIT);
	    messages_wait_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_WAIT;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, MESSAGES_OPT_HISTORY,
		  STRLEN_LITERAL(MESSAGES_OPT_HISTORY)) == 0
	    && VIM_ISDIGIT(p[STRLEN_LITERAL(MESSAGES_OPT_HISTORY)]))
	{
	    p += STRLEN_LITERAL(MESSAGES_OPT_HISTORY);
	    messages_history_new = getdigits(&p);
	    messages_flags_new |= MESSAGES_HISTORY;
	}

	အကယ်၍ (*p != ',' && *p != NUL)
	    ပြန်ပေး FAIL;
	အကယ်၍ (*p == ',')
	    ++p;
    }

    // Either "wait" or "hit-enter" is required
    အကယ်၍ (!(messages_flags_new & (MESSAGES_HIT_ENTER | MESSAGES_WAIT)))
	ပြန်ပေး FAIL;

    // "history" must be set
    အကယ်၍ (!(messages_flags_new & MESSAGES_HISTORY))
	ပြန်ပေး FAIL;

    အကယ်၍ (messages_history_new < 0 || messages_history_new > 10000)
	ပြန်ပေး FAIL;

    အကယ်၍ (messages_wait_new < 0 || messages_wait_new > 10000)
	ပြန်ပေး FAIL;

    msg_flags = messages_flags_new;
    msg_wait = messages_wait_new;

    msg_hist_max = messages_history_new;
    check_msg_hist();

    ပြန်ပေး OK;
}

/*
 * ":messages" command.
 */
    void
ex_messages(exarg_T *eap)
{
    sa_nracanaa msg_hist *p;
    char_u	    *s;
    ကိန်းပြည့်		    c = 0;

    အကယ်၍ (STRCMP(eap->arg, "clear") == 0)
    {
	ကိန်းပြည့် keep = eap->addr_count == 0 ? 0 : eap->line2;

	နေစဉ် (msg_hist_len > keep)
	    (void)delete_first_msg();
	ပြန်ပေး;
    }

    အကယ်၍ (*eap->arg != NUL)
    {
	emsg(_(e_invalid_argument));
	ပြန်ပေး;
    }

    msg_hist_off = TRUE;

    p = first_msg_hist;
    အကယ်၍ (eap->addr_count != 0)
    {
	// Count total messages
	အတွက် (; p != NULL && !got_int; p = p->next)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	အတွက် (p = first_msg_hist; p != NULL && !got_int && c > 0;
						    p = p->next, c--);
    }

    အကယ်၍ (p == first_msg_hist)
    {
#ifdef FEAT_MULTI_LANG
	s = get_mess_lang();
#မဟုတ်ပါက
	s = mch_getenv((char_u *)"LANG");
#endif
	အကယ်၍ (s != NULL && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    msg_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: The Vim Project"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    အတွက် (; p != NULL && !got_int; p = p->next)
	အကယ်၍ (p->msg != NULL)
	    msg_attr((var_na *)p->msg, p->attr);

    msg_hist_off = FALSE;
}

#အကယ်၍ defined(FEAT_CON_DIALOG) || defined(FIND_REPLACE_DIALOG)
/*
 * Call this after prompting the user.  This will avoid a hit-return message
 * and a delay.
 */
    void
msg_end_prompt(void)
{
    need_wait_return = FALSE;
    emsg_on_display = FALSE;
    cmdline_row = msg_row;
    msg_col = 0;
    msg_clr_eos();
    lines_left = -1;
}
#endif

/*
 * Wait for the user to hit a key (normally Enter).
 * If "redraw" is TRUE, clear and redraw the screen.
 * If "redraw" is FALSE, just redraw the screen.
 * If "redraw" is -1, don't redraw at all.
 */
    void
wait_return(ကိန်းပြည့် redraw)
{
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		oldState;
    ကိန်းပြည့်		tmpState;
    ကိန်းပြည့်		had_got_int;
    ကိန်းပြည့်		save_reg_recording;
    FILE	*save_scriptout;

    အကယ်၍ (redraw == TRUE)
	set_must_redraw(UPD_CLEAR);

    // If using ":silent cmd", don't wait for a return.  Also don't set
    // need_wait_return to do it later.
    အကယ်၍ (msg_silent != 0)
	ပြန်ပေး;
#ifdef HAS_MESSAGE_WINDOW
    အကယ်၍ (in_echowindow)
	ပြန်ပေး;
#endif

    /*
     * When inside vgetc(), we can't wait for a typed character at all.
     * With the global command (and some others) we only need one return at
     * the end. Adjust cmdline_row to avoid the next message overwriting the
     * last one.
     */
    အကယ်၍ (vgetc_busy > 0)
	ပြန်ပေး;
    need_wait_return = TRUE;
    အကယ်၍ (no_wait_return)
    {
	အကယ်၍ (!exmode_active)
	    cmdline_row = msg_row;
	ပြန်ပေး;
    }

    redir_off = TRUE;		// don't redirect this message
    oldState = State;
    အကယ်၍ (quit_more)
    {
	c = CAR;		// just pretend CR was hit
	quit_more = FALSE;
	got_int = FALSE;
    }
    မဟုတ်ပါက အကယ်၍ (exmode_active)
    {
	msg_puts(" ");		// make sure the cursor is on the right line
	c = CAR;		// no need for a return in ex mode
	got_int = FALSE;
    }
    မဟုတ်ပါက အကယ်၍ (!stuff_empty())
	// When there are stuffed characters, the next stuffed character will
	// dismiss the hit-enter prompt immediately and have to be put back, so
	// instead just don't show the hit-enter prompt at all.
	c = CAR;
    မဟုတ်ပါက
    {
	// Make sure the hit-return prompt is on screen when 'guioptions' was
	// just changed.
	screenalloc(FALSE);

	State = MODE_HITRETURN;
	setmouse();
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#endif
	cmdline_row = msg_row;

	// Avoid the sequence that the user types ":" at the hit-return prompt
	// to start an Ex command, but the file-changed dialog gets in the
	// way.
	အကယ်၍ (need_check_timestamps)
	    check_timestamps(FALSE);

	အကယ်၍ (msg_flags & MESSAGES_HIT_ENTER)
	{
	    hit_return_msg();

	    do
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
		++allow_keys;

		// Temporarily disable Recording. If Recording is active, the
		// character will be recorded later, since it will be added to
		// the typebuf after the loop
		save_reg_recording = reg_recording;
		save_scriptout = scriptout;
		reg_recording = 0;
		scriptout = NULL;
		c = safe_vgetc();
		အကယ်၍ (had_got_int && !global_busy)
		    got_int = FALSE;
		--no_mapping;
		--allow_keys;
		reg_recording = save_reg_recording;
		scriptout = save_scriptout;

#ifdef FEAT_CLIPBOARD
		// Strange way to allow copying (yanking) a modeless selection
		// at the hit-enter prompt.  Use CTRL-Y, because the same is
		// used in Cmdline-mode and it's harmless when there is no
		// selection.
		အကယ်၍ (KeyTyped && c == Ctrl_Y && clip_star.state == SELECT_DONE)
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
		အကယ်၍ (KeyTyped && p_more && !p_cp)
		{
		    အကယ်၍ (c == 'b' || c == Ctrl_B || c == 'k' || c == 'u' || c == 'g'
						|| c == K_UP || c == K_PAGEUP)
		    {
			အကယ်၍ (msg_scrolled > Rows)
			    // scroll back to show older messages
			    do_more_prompt(c);
			မဟုတ်ပါက
			{
			    msg_didout = FALSE;
			    c = K_IGNORE;
			    msg_col =
#ifdef FEAT_RIGHTLEFT
				cmdmsg_rl ? cmdline_width - 1 :
#endif
				0;
			}
			အကယ်၍ (quit_more)
			{
			    c = CAR;		// just pretend CR was hit
			    quit_more = FALSE;
			    got_int = FALSE;
			}
			မဟုတ်ပါက အကယ်၍ (c != K_IGNORE)
			{
			    c = K_IGNORE;
			    hit_return_msg();
			}
		    }
		    မဟုတ်ပါက အကယ်၍ (msg_scrolled > Rows - 2
			    && (c == 'j' || c == 'd' || c == 'f' || c == Ctrl_F
					    || c == K_DOWN || c == K_PAGEDOWN))
			c = K_IGNORE;
		}
	    } နေစဉ် ((had_got_int && c == Ctrl_C)
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
				    && mouse_row < msg_row
				    && (c == K_LEFTMOUSE
					|| c == K_MIDDLEMOUSE
					|| c == K_RIGHTMOUSE
					|| c == K_X1MOUSE
					|| c == K_X2MOUSE))
				);
	    ui_breakcheck();

	    // Avoid that the mouse-up event causes Visual mode to start.
	    အကယ်၍ (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE
					|| c == K_X1MOUSE || c == K_X2MOUSE)
		(void)jump_to_mouse(MOUSE_SETPOS, NULL, 0);
	    မဟုတ်ပါက အကယ်၍ (!KeyTyped || (vim_strchr((char_u *)"\r\n ", c) == NULL
						   && c != Ctrl_C && c != 'q'))
	    {
		// Put the character back in the typeahead buffer.  Don't use
		// the stuff buffer, because lmaps wouldn't work.
		ins_char_typebuf(vgetc_char, vgetc_mod_mask);
		do_redraw = TRUE;	// need a redraw even though there is
					// typeahead
	    }
	}
	မဟုတ်ပါက
	{
	    c = CAR;
	    // Wait to allow the user to verify the output.
	    do_sleep(msg_wait, TRUE);
	}
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    အကယ်၍ (c == ':' || c == '?' || c == '/')
    {
	အကယ်၍ (!exmode_active)
	    cmdline_row = msg_row;
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
    msg_check();

#ifndef MSWIN
    /*
     * On TTY-style terminals (all but Windows), an extra newline is
     * needed when switching screens on exit.
     */
    အကယ်၍ (swapping_screen() && !termcap_active)
	newline_on_exit = TRUE;
#endif

    need_wait_return = FALSE;
    did_wait_return = TRUE;
    emsg_on_display = FALSE;	// can delete error message now
    lines_left = -1;		// reset lines_left at next msg_start()
    reset_last_sourcing();
    အကယ်၍ (keep_msg != NULL && vim_strsize(keep_msg) >=
			    (Rows - cmdline_row - 1) * cmdline_width + sc_col)
	VIM_CLEAR(keep_msg);	    // don't redisplay message, it's too long

    အကယ်၍ (tmpState == MODE_SETWSIZE)  // got resize event while in vgetc()
    {
	starttermcap();		    // start termcap before redrawing
	shell_resized();
    }
    မဟုတ်ပါက အကယ်၍ (!skip_redraw
	    && (redraw == TRUE || (msg_scrolled != 0 && redraw != -1)))
    {
	starttermcap();		    // start termcap before redrawing
	redraw_later(UPD_VALID);
    }
}

/*
 * Write the hit-return prompt.
 */
    static void
hit_return_msg(void)
{
    ကိန်းပြည့်		save_p_more = p_more;

    p_more = FALSE;	// don't want to see this message when scrolling back
    အကယ်၍ (msg_didout)	// start on a new line
	msg_putchar('\n');
    အကယ်၍ (got_int)
	msg_puts(_("Interrupt: "));

    msg_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    အကယ်၍ (!msg_use_printf())
	msg_clr_eos();
    p_more = save_p_more;
}

/*
 * Set "keep_msg" to "s".  Free the old value and check for NULL pointer.
 */
    void
set_keep_msg(char_u *s, ကိန်းပြည့် attr)
{
    vim_free(keep_msg);
    အကယ်၍ (s != NULL && msg_silent == 0)
	keep_msg = vim_strsave(s);
    မဟုတ်ပါက
	keep_msg = NULL;
    keep_msg_more = FALSE;
    keep_msg_attr = attr;
}

/*
 * If there currently is a message being displayed, set "keep_msg" to it, so
 * that it will be displayed again after redraw.
 */
    void
set_keep_msg_from_hist(void)
{
    အကယ်၍ (keep_msg == NULL && last_msg_hist != NULL && msg_scrolled == 0
						      && (State & MODE_NORMAL))
	set_keep_msg(last_msg_hist->msg, last_msg_hist->attr);
}

/*
 * Prepare for outputting characters in the command line.
 */
    void
msg_start(void)
{
    ကိန်းပြည့်		did_return = FALSE;

    အကယ်၍ (msg_row < cmdline_row)
	msg_row = cmdline_row;

    အကယ်၍ (!msg_silent)
    {
	VIM_CLEAR(keep_msg);
	need_fileinfo = FALSE;
    }

#ifdef FEAT_EVAL
    အကယ်၍ (need_clr_eos)
    {
	// Halfway an ":echo" command and getting an (error) message: clear
	// any text from the command.
	need_clr_eos = FALSE;
	msg_clr_eos();
    }
#endif

#ifdef HAS_MESSAGE_WINDOW
    အကယ်၍ (in_echowindow)
    {
	အကယ်၍ (popup_message_win_visible()
		    && ((msg_col > 0 && (msg_scroll || !full_screen))
			|| in_echowindow))
	{
	    win_T *wp = popup_get_message_win();

	    // start a new line
	    curbuf = wp->w_buffer;
	    ml_append(wp->w_buffer->b_ml.ml_line_count,
					      (char_u *)"", (colnr_T)0, FALSE);
	    curbuf = curwin->w_buffer;
	}
	msg_col = 0;
    }
    မဟုတ်ပါက
#endif
	အကယ်၍ (!msg_scroll && full_screen)	// overwrite last message
    {
	msg_row = cmdline_row;
	msg_col =
#ifdef FEAT_RIGHTLEFT
	    cmdmsg_rl ? cmdline_width - 1 :
#endif
	    0;
    }
    မဟုတ်ပါက အကယ်၍ (msg_didout || in_echowindow)
    {
	// start message on next line
	msg_putchar('\n');
	did_return = TRUE;
	အကယ်၍ (exmode_active != EXMODE_NORMAL)
	    cmdline_row = msg_row;
    }
    အကယ်၍ (!msg_didany || lines_left < 0)
	msg_starthere();
    အကယ်၍ (msg_silent == 0)
    {
	msg_didout = FALSE;		    // no output on current line yet
	cursor_off();
    }

    // when redirecting, may need to start a new line.
    အကယ်၍ (!did_return)
	redir_write((char_u *)"\n", -1);
}

/*
 * Note that the current msg position is where messages start.
 */
    void
msg_starthere(void)
{
    lines_left = cmdline_row;
    msg_didany = FALSE;
}

    void
msg_putchar(ကိန်းပြည့် c)
{
    msg_putchar_attr(c, 0);
}

    void
msg_putchar_attr(ကိန်းပြည့် c, ကိန်းပြည့် attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    အကယ်၍ (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    မဟုတ်ပါက
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    msg_puts_attr((var_na *)buf, attr);
}

    void
msg_outnum(long n)
{
    var_na	buf[20];

    sprintf(buf, "%ld", n);
    msg_puts(buf);
}

    void
msg_home_replace(char_u *fname)
{
    msg_home_replace_attr(fname, 0);
}

#အကယ်၍ defined(FEAT_FIND_ID)
    void
msg_home_replace_hl(char_u *fname)
{
    msg_home_replace_attr(fname, HL_ATTR(HLF_D));
}
#endif

    static void
msg_home_replace_attr(char_u *fname, ကိန်းပြည့် attr)
{
    char_u	*name;

    name = home_replace_save(NULL, fname);
    အကယ်၍ (name != NULL)
	msg_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output up to a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    ကိန်းပြည့်
msg_outtrans(char_u *str)
{
    ပြန်ပေး msg_outtrans_attr(str, 0);
}

    ကိန်းပြည့်
msg_outtrans_attr(char_u *str, ကိန်းပြည့် attr)
{
    ပြန်ပေး msg_outtrans_len_attr(str, (ကိန်းပြည့်)STRLEN(str), attr);
}

    ကိန်းပြည့်
msg_outtrans_len(char_u *str, ကိန်းပြည့် len)
{
    ပြန်ပေး msg_outtrans_len_attr(str, len, 0);
}

/*
 * Output one character at "p".  Return pointer to the next character.
 * Handles multi-byte characters.
 */
    char_u *
msg_outtrans_one(char_u *p, ကိန်းပြည့် attr)
{
    ကိန်းပြည့်		l;

    အကယ်၍ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	msg_outtrans_len_attr(p, l, attr);
	ပြန်ပေး p + l;
    }
    msg_puts_attr((var_na *)transchar_byte_buf(NULL, *p), attr);
    ပြန်ပေး p + 1;
}

    ကိန်းပြည့်
msg_outtrans_len_attr(char_u *msgstr, ကိန်းပြည့် len, ကိန်းပြည့် attr)
{
    ကိန်းပြည့်		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_start = msgstr;
    char_u	*s;
    ကိန်းပြည့်		mb_l;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    အကယ်၍ (attr == 0)
	attr = HL_ATTR(HLF_MSG);

    // if MSG_HIST flag set, add message to history
    အကယ်၍ (attr & MSG_HIST)
    {
	add_msg_hist(str, len, attr);
	attr &= ~MSG_HIST;
    }

    // When drawing over the command line no need to clear it later or remove
    // the mode message.
    အကယ်၍ (msg_silent == 0 && len > 0 && msg_row >= cmdline_row && msg_col == 0)
    {
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    အကယ်၍ (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	msg_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    နေစဉ် (--len >= 0 && !got_int)
    {
	အကယ်၍ (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	မဟုတ်ပါက အကယ်၍ (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	မဟုတ်ပါက
	    mb_l = 1;
	အကယ်၍ (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    အကယ်၍ (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    မဟုတ်ပါက
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		အကယ်၍ (str > plain_start)
		    msg_puts_attr_len((var_na *)plain_start,
					       (ကိန်းပြည့်)(str - plain_start), attr);
		plain_start = str + mb_l;
		msg_puts_attr((var_na *)transchar_buf(NULL, c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	မဟုတ်ပါက
	{
	    s = transchar_byte_buf(NULL, *str);
	    အကယ်၍ (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		အကယ်၍ (str > plain_start)
		    msg_puts_attr_len((var_na *)plain_start,
					       (ကိန်းပြည့်)(str - plain_start), attr);
		plain_start = str + 1;
		msg_puts_attr((var_na *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (ကိန်းပြည့်)STRLEN(s);
	    }
	    မဟုတ်ပါက
		++retval;
	    ++str;
	}
    }

    အကယ်၍ (str > plain_start && !got_int)
	// print the printable chars at the end
	msg_puts_attr_len((var_na *)plain_start, (ကိန်းပြည့်)(str - plain_start), attr);

    got_int |= save_got_int;

    ပြန်ပေး retval;
}

#အကယ်၍ defined(FEAT_QUICKFIX)
    void
msg_make(char_u *arg)
{
    ကိန်းပြည့်	    i;
    static char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    အတွက် (i = 5; *arg && i >= 0; --i)
	အကယ်၍ (*arg++ != str[i])
	    bha_nga;
    အကယ်၍ (i < 0)
    {
	msg_putchar('\n');
	အတွက် (i = 0; rs[i]; ++i)
	    msg_putchar(rs[i] - 3);
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
    ကိန်းပြည့်
msg_outtrans_special(
    char_u	*strstart,
    ကိန်းပြည့်		from,	// TRUE for lhs of a mapping
    ကိန်းပြည့်		maxlen) // screen columns, 0 for unlimited
{
    char_u	*str = strstart;
    ကိန်းပြည့်		retval = 0;
    var_na	*text;
    ကိန်းပြည့်		attr;
    ကိန်းပြည့်		len;

    attr = HL_ATTR(HLF_8);
    နေစဉ် (*str != NUL)
    {
	// Leading and trailing spaces need to be displayed in <> form.
	အကယ်၍ ((str == strstart || str[1] == NUL) && *str == ' ')
	{
	    text = "<Space>";
	    ++str;
	}
	မဟုတ်ပါက
	    text = (var_na *)str2special(&str, from, FALSE);
	အကယ်၍ (text[0] != NUL && text[1] == NUL)
	    // single-byte character or illegal byte
	    text = (var_na *)transchar_byte_buf(NULL, (char_u)text[0]);
	len = vim_strsize((char_u *)text);
	အကယ်၍ (maxlen > 0 && retval + len >= maxlen)
	    bha_nga;
	// Highlight special keys
	msg_puts_attr(text, len > 1
		&& (*mb_ptr2len)((char_u *)text) <= 1 ? attr : 0);
	retval += len;
    }
    ပြန်ပေး retval;
}

#အကယ်၍ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return the lhs or rhs of a mapping, with the key codes turned into printable
 * strings, in an allocated string.
 */
    char_u *
str2special_save(
    char_u  *str,
    ကိန်းပြည့်	    replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    ကိန်းပြည့်	    replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    နေစဉ် (*p != NUL)
	ga_concat(&ga, str2special(&p, replace_spaces, replace_others));
    ga_append(&ga, NUL);
    ပြန်ပေး (char_u *)ga.ga_data;
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
    ကိန်းပြည့်		replace_spaces,	// TRUE to replace " " with "<Space>".
				// used for the lhs of mapping and keytrans().
    ကိန်းပြည့်		replace_others)	// TRUE to replace "<" with "<lt>",
				// "|" with "<Bar>", "\" with "<Bslash>".
{
    ကိန်းပြည့်			c;
    static char_u	buf[7];
    char_u		*str = *sp;
    ကိန်းပြည့်			modifiers = 0;
    ကိန်းပြည့်			special = FALSE;

    အကယ်၍ (has_mbyte)
    {
	char_u	*p;

	// Try to un-escape a multi-byte character.  Return the un-escaped
	// string if it is a multi-byte character.
	p = mb_unescape(sp);
	အကယ်၍ (p != NULL)
	    ပြန်ပေး p;
    }

    c = *str;
    အကယ်၍ ((c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	) && str[1] != NUL && str[2] != NUL)
    {
	အကယ်၍ (str[1] == KS_MODIFIER)
	{
	    modifiers = str[2];
	    str += 3;
	    c = *str;
	}
	အကယ်၍ ((c == K_SPECIAL
#ifdef FEAT_GUI
		    || c == CSI
#endif
	    ) && str[1] != NUL && str[2] != NUL)
	{
	    c = TO_SPECIAL(str[1], str[2]);
	    str += 2;
	}
	အကယ်၍ (IS_SPECIAL(c) || modifiers)	// special key
	    special = TRUE;
    }

    အကယ်၍ (has_mbyte && !IS_SPECIAL(c) && MB_BYTE2LEN(c) > 1)
    {
	char_u	*p;

	*sp = str;
	// Try to un-escape a multi-byte character after modifiers.
	p = mb_unescape(sp);
	အကယ်၍ (p != NULL)
	    // Since 'special' is TRUE the multi-byte character 'c' will be
	    // processed by get_special_key_name()
	    c = (*mb_ptr2char)(p);
	မဟုတ်ပါက
	    // illegal byte
	    *sp = str + 1;
    }
    မဟုတ်ပါက
	// single-byte character, NUL or illegal byte
	*sp = str + (*str == NUL ? 0 : 1);

    // Make special keys and C0 control characters in <> form, also <M-Space>.
    အကယ်၍ (special
	|| c < ' '
	|| (replace_spaces && c == ' ')
	|| (replace_others && (c == '<' || c == '|' || c == '\\')))
	ပြန်ပေး get_special_key_name(c, modifiers);
    buf[0] = c;
    buf[1] = NUL;
    ပြန်ပေး buf;
}

/*
 * Translate a key sequence into special key names.
 */
    void
str2specialbuf(char_u *sp, char_u *buf, ကိန်းပြည့် len)
{
    char_u     *s;
    size_t     buf_len = 0;
    size_t     s_len;

    *buf = NUL;
    နေစဉ် (*sp)
    {
	s = str2special(&sp, FALSE, FALSE);
	s_len = STRLEN(s);
	အကယ်၍ (buf_len + s_len < (size_t)len)
	{
	    STRCPY(buf + buf_len, s);
	    buf_len += s_len;
	}
	မဟုတ်ပါက
	    bha_nga;
    }
}

/*
 * print line for :print or :list command
 */
    void
msg_prt_line(char_u *s, ကိန်းပြည့် list)
{
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		col = 0;
    ကိန်းပြည့်		n_extra = 0;
    ကိန်းပြည့်		c_extra = 0;
    ကိန်းပြည့်		c_final = 0;
    char_u	*p_extra = NULL;	    // init to make SASC shut up
    ကိန်းပြည့်		n;
    ကိန်းပြည့်		attr = 0;
    char_u	*trail = NULL;
    char_u	*lead = NULL;
    ကိန်းပြည့်		in_multispace = FALSE;
    ကိန်းပြည့်		multispace_pos = 0;
    ကိန်းပြည့်		l;
    char_u	buf[MB_MAXBYTES + 1];

    အကယ်၍ (curwin->w_p_list)
	list = TRUE;

    အကယ်၍ (list)
    {
	// find start of trailing whitespace
	အကယ်၍ (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    နေစဉ် (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	အကယ်၍ (curwin->w_lcs_chars.lead
				 || curwin->w_lcs_chars.leadmultispace != NULL
				 || curwin->w_lcs_chars.leadtab1 != NUL)
	{
	    lead = s;
	    နေစဉ် (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    အကယ်၍ (*lead == NUL)
		lead = NULL;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    အကယ်၍ (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	msg_putchar(' ');

    နေစဉ် (!got_int)
    {
	အကယ်၍ (n_extra > 0)
	{
	    --n_extra;
	    အကယ်၍ (n_extra == 0 && c_final)
		c = c_final;
	    မဟုတ်ပါက အကယ်၍ (c_extra)
		c = c_extra;
	    မဟုတ်ပါက
		c = *p_extra++;
	}
	မဟုတ်ပါက အကယ်၍ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    col += (*mb_ptr2cells)(s);
	    အကယ်၍ (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    မဟုတ်ပါက အကယ်၍ (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		ကိန်းပြည့် len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    မဟုတ်ပါက
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    msg_puts((var_na *)buf);
	    s += l;
	    continue;
	}
	မဟုတ်ပါက
	{
	    attr = 0;
	    c = *s++;
	    အကယ်၍ (list)
	    {
		in_multispace = c == ' ' && (*s == ' '
						 || (col > 0 && s[-2] == ' '));
		အကယ်၍ (!in_multispace)
		    multispace_pos = 0;
	    }
	    အကယ်၍ (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ifdef FEAT_VARTABS
		n_extra = tabstop_padding(col, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#မဟုတ်ပါက
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
#endif
		အကယ်၍ (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		မဟုတ်ပါက
		{
		    ကိန်းပြည့် lcs_tab1 = curwin->w_lcs_chars.tab1;
		    ကိန်းပြည့် lcs_tab2 = curwin->w_lcs_chars.tab2;
		    ကိန်းပြည့် lcs_tab3 = curwin->w_lcs_chars.tab3;

		    // check if leadtab is set in 'listchars'
		    အကယ်၍ (lead != NULL && s <= lead
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
	    မဟုတ်ပါက အကယ်၍ (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    မဟုတ်ပါက အကယ်၍ (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    မဟုတ်ပါက အကယ်၍ (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte_buf(NULL, c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    မဟုတ်ပါက အကယ်၍ (c == ' ')
	    {
		အကယ်၍ (lead != NULL && s <= lead && in_multispace
			&& curwin->w_lcs_chars.leadmultispace != NULL)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    အကယ်၍ (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		မဟုတ်ပါက အကယ်၍ (lead != NULL && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		မဟုတ်ပါက အကယ်၍ (trail != NULL && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		မဟုတ်ပါက အကယ်၍ (in_multispace
			&& curwin->w_lcs_chars.multispace != NULL)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    အကယ်၍ (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		မဟုတ်ပါက အကယ်၍ (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	အကယ်၍ (c == NUL)
	    bha_nga;

	msg_putchar_attr(c, attr);
	col++;
    }
    msg_clr_eos();
}

/*
 * Use screen_puts() to output one multi-byte character.
 * Return the pointer "s" advanced to the next character.
 */
    static char_u *
screen_puts_mbyte(char_u *s, ကိန်းပြည့် l, ကိန်းပြည့် attr)
{
    ကိန်းပြည့်		cw;

    msg_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    အကယ်၍ (cw > 1 && (
#ifdef FEAT_RIGHTLEFT
		cmdmsg_rl ? msg_col <= 1 :
#endif
		msg_col == cmdline_width - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	msg_screen_putchar('>', HL_ATTR(HLF_AT));
	ပြန်ပေး s;
    }

    screen_puts_len(s, l, msg_row, cmdline_col_off + msg_col, attr);
#ifdef FEAT_RIGHTLEFT
    အကယ်၍ (cmdmsg_rl)
    {
	msg_col -= cw;
	အကယ်၍ (msg_col == 0)
	{
	    msg_col = cmdline_width;
	    ++msg_row;
	}
    }
    မဟုတ်ပါက
#endif
    {
	msg_col += cw;
	အကယ်၍ (msg_col >= cmdline_width)
	{
	    msg_col = 0;
	    ++msg_row;
	}
    }
    ပြန်ပေး s + l;
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    void
msg_puts(var_na *s)
{
    msg_puts_attr(s, 0);
}

    void
msg_puts_title(var_na *s)
{
    msg_puts_attr(s, HL_ATTR(HLF_T));
}

/*
 * Show a message in such a way that it always fits in the line.  Cut out a
 * part in the middle and replace it with "..." when necessary.
 * Does not handle multi-byte characters!
 */
    static void
msg_outtrans_long_len_attr(char_u *longstr, ကိန်းပြည့် len, ကိန်းပြည့် attr)
{
    ကိန်းပြည့်		slen = len;
    ကိန်းပြည့်		room;

    room = cmdline_width - msg_col;
    အကယ်၍ (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	msg_outtrans_len_attr(longstr, slen, attr);
	msg_puts_attr("...", HL_ATTR(HLF_8));
    }
    msg_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    void
msg_outtrans_long_attr(char_u *longstr, ကိန်းပြည့် attr)
{
    msg_outtrans_long_len_attr(longstr, (ကိန်းပြည့်)STRLEN(longstr), attr);
}

/*
 * Basic function for writing a message with highlight attributes.
 */
    void
msg_puts_attr(var_na *s, ကိန်းပြည့် attr)
{
    msg_puts_attr_len(s, -1, attr);
}

/*
 * Like msg_puts_attr(), but with a maximum length "maxlen" (in bytes).
 * When "maxlen" is -1 there is no maximum length.
 * When "maxlen" is >= 0 the message is not put in the history.
 */
    static void
msg_puts_attr_len(var_na *str, ကိန်းပြည့် maxlen, ကိန်းပြည့် attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    အကယ်၍ (msg_silent != 0)
	ပြန်ပေး;

    အကယ်၍ (attr == 0)
	attr = HL_ATTR(HLF_MSG);

    // if MSG_HIST flag set, add message to history
    အကယ်၍ ((attr & MSG_HIST) && maxlen < 0)
    {
	add_msg_hist((char_u *)str, -1, attr);
	attr &= ~MSG_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    အကယ်၍ (msg_scrolled != 0 && !msg_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    msg_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    အကယ်၍ (msg_use_printf())
	msg_puts_printf((char_u *)str, maxlen);
    မဟုတ်ပါက
	msg_puts_display((char_u *)str, maxlen, attr, FALSE);

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
    static void
put_msg_win(win_T *wp, ကိန်းပြည့် where, char_u *t_s, char_u *end, linenr_T lnum)
{
    char_u  *p;

    အကယ်၍ (where == PUT_BELOW)
    {
	အကယ်၍ (*end != NUL)
	{
	    p = vim_strnsave(t_s, end - t_s);
	    အကယ်၍ (p == NULL)
		ပြန်ပေး;
	}
	မဟုတ်ပါက
	    p = t_s;
	ml_append_buf(wp->w_buffer, lnum, p, (colnr_T)0, FALSE);
	အကယ်၍ (p != t_s)
	    vim_free(p);
    }
    မဟုတ်ပါက
    {
	char_u *newp;

	curbuf = wp->w_buffer;
	အကယ်၍ (where == PUT_APPEND)
	{
	    newp = concat_str(ml_get(lnum), t_s);
	    အကယ်၍ (newp == NULL)
		ပြန်ပေး;
	    အကယ်၍ (*end != NUL)
		newp[STRLEN(ml_get(lnum)) + (end - t_s)] = NUL;
	}
	မဟုတ်ပါက
	{
	    newp = vim_strnsave(t_s, end - t_s);
	    အကယ်၍ (newp == NULL)
		ပြန်ပေး;
	}
	ml_replace(lnum, newp, FALSE);
	curbuf = curwin->w_buffer;
    }
    redraw_win_later(wp, UPD_NOT_VALID);

    // set msg_col so that a newline is written if needed
    msg_col += (ကိန်းပြည့်)(end - t_s);
}
#endif

/*
 * The display part of msg_puts_attr_len().
 * May be called recursively to display scroll-back text.
 */
    static void
msg_puts_display(
    char_u	*str,
    ကိန်းပြည့်		maxlen,
    ကိန်းပြည့်		attr,
    ကိန်းပြည့်		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    ကိန်းပြည့်		t_col = 0;	// screen cells todo, 0 when "t_s" not used
    ကိန်းပြည့်		l;
    ကိန်းပြည့်		cw;
    char_u	*sb_str = str;
    ကိန်းပြည့်		sb_col = msg_col;
    ကိန်းပြည့်		wrap;
    ကိန်းပြည့်		did_last_char;
#ifdef HAS_MESSAGE_WINDOW
    ကိန်းပြည့်		where = PUT_APPEND;
    win_T	*msg_win = NULL;
    linenr_T    lnum = 1;

    အကယ်၍ (in_echowindow)
    {
	msg_win = popup_get_message_win();

	အကယ်၍ (msg_win != NULL)
	{
	    အကယ်၍ (!popup_message_win_visible())
	    {
		အကယ်၍ (*str == NL)
		{
		    // When not showing the message window and the output
		    // starts with a NL show the message normally.
		    msg_win = NULL;
		}
		မဟုတ်ပါက
		{
		    // currently hidden, make it empty
		    curbuf = msg_win->w_buffer;
		    နေစဉ် ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
			ml_delete(1);
		    curbuf = curwin->w_buffer;
		}
	    }
	    မဟုတ်ပါက
	    {
		lnum = msg_win->w_buffer->b_ml.ml_line_count;
		အကယ်၍ (msg_col == 0)
		    where = PUT_TRUNC;
	    }
	}
    }
#endif

    did_wait_return = FALSE;
    နေစဉ် ((maxlen < 0 || (ကိန်းပြည့်)(s - str) < maxlen) && *s != NUL)
    {
#ifdef HAS_MESSAGE_WINDOW
	// For echowindow, use full width; for regular messages, leave last column
	ကိန်းပြည့် wrap_col = msg_win != NULL ? cmdline_width : cmdline_width - 1;
#မဟုတ်ပါက
	ကိန်းပြည့် wrap_col = cmdline_width - 1;
#endif

	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	အကယ်၍ (!recurse && msg_row >= Rows - 1 && (*s == '\n' || (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			msg_col <= 1
		      || (*s == TAB && msg_col <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && msg_col <= 2))
		    :
#endif
		      ((*s != '\r' && msg_col + t_col >= wrap_col)
		       || (*s == TAB && msg_col + t_col
			   >= (wrap_col & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
			   && msg_col + t_col >= wrap_col - 1)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    အကယ်၍ (t_col > 0)
	    {
		// output postponed text
#ifdef HAS_MESSAGE_WINDOW
		အကယ်၍ (msg_win != NULL)
		{
		    put_msg_win(msg_win, where, t_s, s, lnum);
		    အကယ်၍ (where == PUT_BELOW)
			++lnum;
		    t_col = 0;
		    where = PUT_BELOW;
		}
		မဟုတ်ပါက
#endif
		    t_puts(&t_col, t_s, s, attr);
	    }

	    // When no more prompt and no more room, truncate here
	    အကယ်၍ (msg_no_more && lines_left == 0)
		bha_nga;

#ifdef HAS_MESSAGE_WINDOW
	    အကယ်၍ (msg_win == NULL)
#endif
		// Scroll the screen up one line.
		msg_scroll_up();

#ifdef HAS_MESSAGE_WINDOW
	    အကယ်၍ (msg_win == NULL)
	    {
#endif
		msg_row = Rows - 2;
		အကယ်၍ (msg_col >= cmdline_width)  // can happen after screen resize
		    msg_col = cmdline_width - 1;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif

	    // Display char in last column before showing more-prompt.
	    အကယ်၍ (*s >= ' '
#ifdef FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#endif
	       )
	    {
		အကယ်၍ (has_mbyte)
		{
		    အကယ်၍ (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (ကိန်းပြည့်)((str + maxlen) - s));
		    မဟုတ်ပါက
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		မဟုတ်ပါက
		    msg_screen_putchar(*s++, attr);
		did_last_char = TRUE;
	    }
	    မဟုတ်ပါက
		did_last_char = FALSE;

	    အကယ်၍ (p_more)
		// store text for scrolling back
		store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

#ifdef HAS_MESSAGE_WINDOW
	    အကယ်၍ (msg_win == NULL)
	    {
#endif
		inc_msg_scrolled();
		need_wait_return = TRUE; // may need wait_return() in main()
		redraw_cmdline = TRUE;
		အကယ်၍ (cmdline_row > 0 && !exmode_active)
		    --cmdline_row;

		/*
		 * If screen is completely filled and 'more' is set then wait
		 * for a character.
		 */
		အကယ်၍ (lines_left > 0)
		    --lines_left;
#ifdef HAS_MESSAGE_WINDOW
	    }
#endif
	    အကယ်၍ (p_more && lines_left == 0 && State != MODE_HITRETURN
					    && !msg_no_more && !exmode_active)
	    {
#ifdef FEAT_CON_DIALOG
		အကယ်၍ (do_more_prompt(NUL))
		    s = confirm_msg_tail;
#မဟုတ်ပါက
		(void)do_more_prompt(NUL);
#endif
		အကယ်၍ (quit_more)
		    ပြန်ပေး;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    အကယ်၍ (did_last_char)
		continue;
	}

	wrap = *s == '\n'
		    || msg_col + t_col >= wrap_col
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
			    && msg_col + t_col >= wrap_col - 1);
	အကယ်၍ (t_col > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	{
	    // output any postponed text
#ifdef HAS_MESSAGE_WINDOW
	    အကယ်၍ (msg_win != NULL)
	    {
		put_msg_win(msg_win, where, t_s, s, lnum);
		အကယ်၍ (where == PUT_BELOW)
		    ++lnum;
		t_col = 0;
		where = PUT_BELOW;
		// Reset msg_col after outputting to new line in echowindow
		အကယ်၍ (wrap)
		    msg_col = 0;
	    }
	    မဟုတ်ပါက
#endif
		t_puts(&t_col, t_s, s, attr);
	}

	အကယ်၍ (wrap && p_more && !recurse)
	    // store text for scrolling back
	    store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	အကယ်၍ (*s == '\n')		    // go to next line
	{
#ifdef HAS_MESSAGE_WINDOW
	    အကယ်၍ (msg_win != NULL)
	    {
		// Ignore a NL when the buffer is empty, it is used to scroll
		// up the text.
		အကယ်၍ ((msg_win->w_buffer->b_ml.ml_flags & ML_EMPTY) == 0)
		{
		    put_msg_win(msg_win, PUT_BELOW, t_s, t_s, lnum);
		    ++lnum;
		}
		msg_col = 0;  // Reset column for new line
	    }
	    မဟုတ်ပါက
#endif
		msg_didout = FALSE;	    // remember that line is empty
#ifdef FEAT_RIGHTLEFT
	    အကယ်၍ (cmdmsg_rl)
		msg_col = cmdline_width - 1;
	    မဟုတ်ပါက
#endif
		msg_col = 0;
	    အကယ်၍ (++msg_row >= Rows)  // safety check
		msg_row = Rows - 1;
	}
	မဟုတ်ပါက အကယ်၍ (*s == '\r')	    // go to column 0
	{
	    msg_col = 0;
#ifdef HAS_MESSAGE_WINDOW
	    where = PUT_TRUNC;
#endif
	}
	မဟုတ်ပါက အကယ်၍ (*s == '\b')	    // go to previous char
	{
	    အကယ်၍ (msg_col)
		--msg_col;
	}
	မဟုတ်ပါက အကယ်၍ (*s == TAB)	    // translate Tab into spaces
	{
#ifdef HAS_MESSAGE_WINDOW
	    အကယ်၍ (msg_win != NULL)
		msg_col = (msg_col + 7) % 8;
	    မဟုတ်ပါက
#endif
		do
		    msg_screen_putchar(' ', attr);
		နေစဉ် (msg_col & 7);
	}
	မဟုတ်ပါက အကယ်၍ (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	မဟုတ်ပါက
	{
	    အကယ်၍ (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		အကယ်၍ (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (ကိန်းပြည့်)((str + maxlen) - s));
		မဟုတ်ပါက
		    l = (*mb_ptr2len)(s);
	    }
	    မဟုတ်ပါက
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    အကယ်၍ (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl ||
#endif
		    (cw > 1 && msg_col + t_col >= wrap_col))
	    {
		အကယ်၍ (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		မဟုတ်ပါက
		    msg_screen_putchar(*s, attr);
	    }
	    မဟုတ်ပါက
	    {
		// postpone this character until later
		အကယ်၍ (t_col == 0)
		    t_s = s;
		t_col += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    အကယ်၍ (t_col > 0)
    {
#ifdef HAS_MESSAGE_WINDOW
	အကယ်၍ (msg_win != NULL)
	    put_msg_win(msg_win, where, t_s, s, lnum);
	မဟုတ်ပါက
#endif
	    t_puts(&t_col, t_s, s, attr);
    }

#ifdef HAS_MESSAGE_WINDOW
    အကယ်၍ (msg_win != NULL)
	popup_show_message_win();
#endif
    // Store the text for scroll back.
    အကယ်၍ (p_more && !recurse)
	store_sb_text(&sb_str, s, attr, &sb_col, FALSE);

    msg_check();
}

/*
 * Return TRUE when ":filter pattern" was used and "msg" does not match
 * "pattern".
 */
    ကိန်းပြည့်
message_filtered(char_u *msg)
{
    ကိန်းပြည့် match;

    အကယ်၍ (cmdmod.cmod_filter_regmatch.regprog == NULL)
	ပြန်ပေး FALSE;
    match = vim_regexec(&cmdmod.cmod_filter_regmatch, msg, (colnr_T)0);
    ပြန်ပေး cmdmod.cmod_filter_force ? match : !match;
}

/*
 * Scroll the screen up one line for displaying the next message line.
 */
    static void
msg_scroll_up(void)
{
#ifdef HAS_MESSAGE_WINDOW
    အကယ်၍ (in_echowindow)
	ပြန်ပေး;
#endif
#ifdef FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    အကယ်၍ (gui.in_use)
	gui_undraw_cursor();
#endif
    // scrolling up always works
    mch_disable_flush();
    screen_del_lines(0, 0, 1, (ကိန်းပြည့်)Rows, TRUE, 0, NULL);
    mch_enable_flush();

    အကယ်၍ (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((ကိန်းပြည့်)Rows - 1, (ကိန်းပြည့်)Rows, cmdline_col_off,
		cmdline_col_off + cmdline_width, ' ', ' ', HL_ATTR(HLF_MSG));

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	အကယ်၍ (ScreenAttrs[LineOffset[Rows - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((ကိန်းပြည့်)Rows - 2, (ကိန်းပြည့်)Rows - 1,
				 (ကိန်းပြည့်)Columns - 1, (ကိန်းပြည့်)Columns,
				 ' ', ' ', HL_ATTR(HLF_MSG));
    }
}

/*
 * Increment "msg_scrolled".
 */
    static void
inc_msg_scrolled(void)
{
#ifdef FEAT_EVAL
    အကယ်၍ (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	string_T    p = {SOURCING_NAME, 0};
	char_u	    *tofree = NULL;

	// v:scrollstart is empty, set it to the script/function name and line
	// number
	အကယ်၍ (p.string == NULL)
	{
	    p.string = (char_u *)_("Unknown");
	    p.length = STRLEN(p.string);
	}
	မဟုတ်ပါက
	{
	    size_t  tofreesize;

	    tofreesize = (ကိန်းပြည့်)STRLEN(p.string) + 40;
	    tofree = alloc(tofreesize);
	    အကယ်၍ (tofree != NULL)
	    {
		p.length = vim_snprintf_safelen((var_na *)tofree, tofreesize,
		    _("%s line %ld"), p.string, (long)SOURCING_LNUM);
		p.string = tofree;
	    }
	}
	set_vim_var_string(VV_SCROLLSTART, p.string, (ကိန်းပြည့်)p.length);
	vim_free(tofree);
    }
#endif
    ++msg_scrolled;
    set_must_redraw(UPD_VALID);
}

/*
 * To be able to scroll back at the "more" and "hit-enter" prompts we need to
 * store the displayed text and remember where screen lines start.
 */
typedef sa_nracanaa msgchunk_S msgchunk_T;
sa_nracanaa msgchunk_S
{
    msgchunk_T	*sb_next;
    msgchunk_T	*sb_prev;
    var_na	sb_eol;		// TRUE when line ends after this text
    ကိန်းပြည့်		sb_msg_col;	// column in which text starts
    ကိန်းပြည့်		sb_attr;	// text attributes
    char_u	sb_text[1];	// text to be displayed, actually longer
};

static msgchunk_T *last_msgchunk = NULL; // last displayed text

static msgchunk_T *msg_sb_start(msgchunk_T *mps);

typedef ga_nanaa {
    SB_CLEAR_NONE = 0,
    SB_CLEAR_ALL,
    SB_CLEAR_CMDLINE_BUSY,
    SB_CLEAR_CMDLINE_DONE
} sb_clear_T;

// When to clear text on next msg.
static sb_clear_T do_clear_sb_text = SB_CLEAR_NONE;

/*
 * Store part of a printed message for displaying when scrolling back.
 */
    static void
store_sb_text(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    ကိန်းပြည့်		attr,
    ကိန်းပြည့်		*sb_col,
    ကိန်းပြည့်		finish)		// line ends
{
    msgchunk_T	*mp;

    အကယ်၍ (do_clear_sb_text == SB_CLEAR_ALL
	    || do_clear_sb_text == SB_CLEAR_CMDLINE_DONE)
    {
	clear_sb_text(do_clear_sb_text == SB_CLEAR_ALL);
	msg_sb_eol();  // prevent messages from overlapping
	do_clear_sb_text = SB_CLEAR_NONE;
    }

    အကယ်၍ (s > *sb_str)
    {
	mp = alloc(offsetof(msgchunk_T, sb_text) + (s - *sb_str) + 1);
	အကယ်၍ (mp != NULL)
	{
	    mp->sb_eol = finish;
	    mp->sb_msg_col = *sb_col;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_text, *sb_str, s - *sb_str);

	    အကယ်၍ (last_msgchunk == NULL)
	    {
		last_msgchunk = mp;
		mp->sb_prev = NULL;
	    }
	    မဟုတ်ပါက
	    {
		mp->sb_prev = last_msgchunk;
		last_msgchunk->sb_next = mp;
		last_msgchunk = mp;
	    }
	    mp->sb_next = NULL;
	}
    }
    မဟုတ်ပါက အကယ်၍ (finish && last_msgchunk != NULL)
	last_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_col = 0;
}

/*
 * Finished showing messages, clear the scroll-back text on the next message.
 */
    void
may_clear_sb_text(void)
{
    do_clear_sb_text = SB_CLEAR_ALL;
}

/*
 * Starting to edit the command line: do not clear messages now.
 */
    void
sb_text_start_cmdline(void)
{
    အကယ်၍ (do_clear_sb_text == SB_CLEAR_CMDLINE_BUSY)
	// Invoking command line recursively: the previous-level command line
	// doesn't need to be remembered as it will be redrawn when returning
	// to that level.
	sb_text_restart_cmdline();
    မဟုတ်ပါက
    {
	msg_sb_eol();
	do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;
    }
}

/*
 * Redrawing the command line: clear the last unfinished line.
 */
    void
sb_text_restart_cmdline(void)
{
    msgchunk_T *tofree;

    // Needed when returning from nested command line.
    do_clear_sb_text = SB_CLEAR_CMDLINE_BUSY;

    အကယ်၍ (last_msgchunk == NULL || last_msgchunk->sb_eol)
	// No unfinished line: don't clear anything.
	ပြန်ပေး;

    tofree = msg_sb_start(last_msgchunk);
    last_msgchunk = tofree->sb_prev;
    အကယ်၍ (last_msgchunk != NULL)
	last_msgchunk->sb_next = NULL;
    နေစဉ် (tofree != NULL)
    {
	msgchunk_T *tofree_next = tofree->sb_next;

	vim_free(tofree);
	tofree = tofree_next;
    }
}

/*
 * Ending to edit the command line: clear old lines but the last one later.
 */
    void
sb_text_end_cmdline(void)
{
    do_clear_sb_text = SB_CLEAR_CMDLINE_DONE;
}

/*
 * Clear any text remembered for scrolling back.
 * When "all" is FALSE keep the last line.
 * Called when redrawing the screen.
 */
    void
clear_sb_text(ကိန်းပြည့် all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    အကယ်၍ (all)
	lastp = &last_msgchunk;
    မဟုတ်ပါက
    {
	အကယ်၍ (last_msgchunk == NULL)
	    ပြန်ပေး;
	lastp = &msg_sb_start(last_msgchunk)->sb_prev;
    }

    နေစဉ် (*lastp != NULL)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}

/*
 * "g<" command.
 */
    void
show_sb_text(void)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = msg_sb_start(last_msgchunk);
    အကယ်၍ (mp == NULL || mp->sb_prev == NULL)
	vim_beep(BO_MESS);
    မဟုတ်ပါက
    {
	do_more_prompt('G');
	wait_return(FALSE);
    }
}

/*
 * Move to the start of screen line in already displayed text.
 */
    static msgchunk_T *
msg_sb_start(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    နေစဉ် (mp != NULL && mp->sb_prev != NULL && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    ပြန်ပေး mp;
}

/*
 * Mark the last message chunk as finishing the line.
 */
    void
msg_sb_eol(void)
{
    // A trailing newline starts an empty line that has not been filled yet.
    // Marking it as finished would introduce an extra empty line.
    အကယ်၍ (last_msgchunk != NULL
	    && !(last_msgchunk->sb_text[0] == '\n'
		&& last_msgchunk->sb_text[1] == NUL))
	last_msgchunk->sb_eol = TRUE;
}

/*
 * Display a screen line from previously displayed text at row "row".
 * When "clear_to_eol" is set clear the rest of the screen line.
 * Returns a pointer to the text for the next line (can be NULL).
 */
    static msgchunk_T *
disp_sb_line(ကိန်းပြည့် row, msgchunk_T *smp, ကိန်းပြည့် clear_to_eol)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    အတွက် (;;)
    {
	msg_row = row;
	msg_col = mp->sb_msg_col;
	p = mp->sb_text;
	အကယ်၍ (*p == '\n')	    // don't display the line break
	    ++p;
	msg_puts_display(p, -1, mp->sb_attr, TRUE);

	// If clearing the screen did not work (e.g. because of a background
	// color and t_ut isn't set) clear until the last column here.
	အကယ်၍ (clear_to_eol)
	    screen_fill(row, row + 1,
		    cmdline_col_off + msg_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', HL_ATTR(HLF_MSG));

	အကယ်၍ (mp->sb_eol || mp->sb_next == NULL)
	    bha_nga;
	mp = mp->sb_next;
    }
    ပြန်ပေး mp->sb_next;
}

/*
 * Output any postponed text for msg_puts_attr_len().
 */
    static void
t_puts(
    ကိန်းပြည့်		*t_col,
    char_u	*t_s,
    char_u	*s,
    ကိန်းပြည့်		attr)
{
    // output postponed text
    msg_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (ကိန်းပြည့်)(s - t_s), msg_row, cmdline_col_off + msg_col,
	    attr);
    msg_col += *t_col;
    *t_col = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    အကယ်၍ (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--msg_col;
    အကယ်၍ (msg_col >= cmdline_width)
    {
	msg_col = 0;
	++msg_row;
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
    ကိန်းပြည့်
msg_use_printf(void)
{
    ပြန်ပေး (!msg_check_screen()
#အကယ်၍ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.in_use && !termcap_active)
# မဟုတ်ပါက
	    || !termcap_active
# endif
#endif
	    || (swapping_screen() && !termcap_active)
	       );
}

/*
 * Print a message when there is no valid screen.
 */
    static void
msg_puts_printf(char_u *str, ကိန်းပြည့် maxlen)
{
    char_u	*s = str;
    char_u	*buf = NULL;
    char_u	*p = s;

#ifdef MSWIN
    အကယ်၍ (!(silent_mode && p_verbose == 0))
	mch_settmode(TMODE_COOK);	// handle CR and NL correctly
#endif
    နေစဉ် ((maxlen < 0 || (ကိန်းပြည့်)(s - str) < maxlen) && *s != NUL)
    {
	အကယ်၍ (!(silent_mode && p_verbose == 0))
	{
	    // NL --> CR NL translation (for Unix, not for "--version")
	    အကယ်၍ (*s == NL)
	    {
		ကိန်းပြည့် n = (ကိန်းပြည့်)(s - p);

		buf = alloc(n + 3);
		အကယ်၍ (buf != NULL)
		{
		    memcpy(buf, p, n);
		    အကယ်၍ (!info_message)
			buf[n++] = CAR;
		    buf[n++] = NL;
		    buf[n++] = NUL;
		    အကယ်၍ (info_message)   // informative message, not an error
			mch_msg((var_na *)buf);
		    မဟုတ်ပါက
			mch_errmsg((var_na *)buf);
		    vim_free(buf);
		}
		p = s + 1;
	    }
	}

	// primitive way to compute the current column
#ifdef FEAT_RIGHTLEFT
	အကယ်၍ (cmdmsg_rl)
	{
	    အကယ်၍ (*s == CAR || *s == NL)
		msg_col = cmdline_width - 1;
	    မဟုတ်ပါက
		--msg_col;
	}
	မဟုတ်ပါက
#endif
	{
	    အကယ်၍ (*s == CAR || *s == NL)
		msg_col = 0;
	    မဟုတ်ပါက
		++msg_col;
	}
	++s;
    }

    အကယ်၍ (*p != NUL && !(silent_mode && p_verbose == 0))
    {
	char_u *tofree = NULL;

	အကယ်၍ (maxlen > 0 && vim_strlen_maxlen((var_na *)p, (size_t)maxlen)
							     >= (size_t)maxlen)
	{
	    tofree = vim_strnsave(p, (size_t)maxlen);
	    p = tofree;
	}
	အကယ်၍ (p != NULL)
	{
	    အကယ်၍ (info_message)
		mch_msg((var_na *)p);
	    မဟုတ်ပါက
		mch_errmsg((var_na *)p);
	    vim_free(tofree);
	}
    }

    msg_didout = TRUE;	    // assume that line is not empty

#ifdef MSWIN
    အကယ်၍ (!(silent_mode && p_verbose == 0))
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
    static ကိန်းပြည့်
do_more_prompt(ကိန်းပြည့် typed_char)
{
    static ကိန်းပြည့်	entered = FALSE;
    ကိန်းပြည့်		used_typed_char = typed_char;
    ကိန်းပြည့်		oldState = State;
    ကိန်းပြည့်		c;
#ifdef FEAT_CON_DIALOG
    ကိန်းပြည့်		retval = FALSE;
#endif
    ကိန်းပြည့်		toscroll;
    msgchunk_T	*mp_last = NULL;
    msgchunk_T	*mp;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		msg_attr;

    msg_attr = HL_ATTR(HLF_MSG);

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    အကယ်၍ (entered || (State == MODE_HITRETURN && typed_char == 0))
	ပြန်ပေး FALSE;
    entered = TRUE;

    အကယ်၍ (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_last = msg_sb_start(last_msgchunk);
	အတွက် (i = 0; i < Rows - 2 && mp_last != NULL
					     && mp_last->sb_prev != NULL; ++i)
	    mp_last = msg_sb_start(mp_last->sb_prev);
    }

    State = MODE_ASKMORE;
    setmouse();
    အကယ်၍ (typed_char == NUL)
	msg_moremsg(FALSE);
    အတွက် (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	အကယ်၍ (used_typed_char != NUL)
	{
	    c = used_typed_char;	// was typed at hit-enter prompt
	    used_typed_char = NUL;
	}
	မဟုတ်ပါက
	    c = get_keystroke();

#အကယ်၍ defined(FEAT_MENU) && defined(FEAT_GUI)
	အကယ်၍ (c == K_MENU)
	{
	    ကိန်းပြည့် idx = get_menu_index(current_menu, MODE_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    အကယ်၍ (idx == MENU_INDEX_INVALID)
		continue;
	    c = *current_menu->strings[idx];
	    အကယ်၍ (c != NUL && current_menu->strings[idx][1] != NUL)
		ins_typebuf(current_menu->strings[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#endif

	toscroll = 0;
	switch (c)
	{
	pak_sa BS:		// scroll one line back
	pak_sa K_BS:
	pak_sa 'k':
	pak_sa K_UP:
	    toscroll = -1;
	    bha_nga;

	pak_sa CAR:		// one extra line
	pak_sa NL:
	pak_sa 'j':
	pak_sa K_DOWN:
	    toscroll = 1;
	    bha_nga;

	pak_sa 'u':		// Up half a page
	    toscroll = -(Rows / 2);
	    bha_nga;

	pak_sa 'd':		// Down half a page
	    toscroll = Rows / 2;
	    bha_nga;

	pak_sa 'b':		// one page back
	pak_sa Ctrl_B:
	pak_sa K_PAGEUP:
	    toscroll = -(Rows - 1);
	    bha_nga;

	pak_sa ' ':		// one extra page
	pak_sa 'f':
	pak_sa Ctrl_F:
	pak_sa K_PAGEDOWN:
	pak_sa K_LEFTMOUSE:
	    toscroll = Rows - 1;
	    bha_nga;

	pak_sa 'g':		// all the way back to the start
	    toscroll = -999999;
	    bha_nga;

	pak_sa 'G':		// all the way to the end
	    toscroll = 999999;
	    lines_left = 999999;
	    bha_nga;

	pak_sa ':':		// start new command line
#ifdef FEAT_CON_DIALOG
	    အကယ်၍ (!confirm_msg_used)
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
	pak_sa 'q':		// quit
	pak_sa Ctrl_C:
	pak_sa ESC:
#ifdef FEAT_CON_DIALOG
	    အကယ်၍ (confirm_msg_used)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    မဟုတ်ပါက
#endif
	    {
		got_int = TRUE;
		quit_more = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    lines_left = Rows - 1;
	    bha_nga;

#ifdef FEAT_CLIPBOARD
	pak_sa Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    အကယ်၍ (clip_star.state == SELECT_DONE)
		clip_copy_modeless_selection(TRUE);
	    continue;
#endif
	default:		// no valid response
	    msg_moremsg(TRUE);
	    continue;
	}

	အကယ်၍ (toscroll != 0)
	{
	    အကယ်၍ (toscroll < 0)
	    {
		// go to start of last line
		အကယ်၍ (mp_last == NULL)
		    mp = msg_sb_start(last_msgchunk);
		မဟုတ်ပါက အကယ်၍ (mp_last->sb_prev != NULL)
		    mp = msg_sb_start(mp_last->sb_prev);
		မဟုတ်ပါက
		    mp = NULL;

		// go to start of line at top of the screen
		အတွက် (i = 0; i < Rows - 2 && mp != NULL && mp->sb_prev != NULL;
									  ++i)
		    mp = msg_sb_start(mp->sb_prev);

		အကယ်၍ (mp != NULL && mp->sb_prev != NULL)
		{
		    // Find line to be displayed at top.
		    အတွက် (i = 0; i > toscroll; --i)
		    {
			အကယ်၍ (mp == NULL || mp->sb_prev == NULL)
			    bha_nga;
			mp = msg_sb_start(mp->sb_prev);
			အကယ်၍ (mp_last == NULL)
			    mp_last = msg_sb_start(last_msgchunk);
			မဟုတ်ပါက
			    mp_last = msg_sb_start(mp_last->sb_prev);
		    }

		    အကယ်၍ (toscroll == -1 && screen_ins_lines(0, 0, 1,
						     (ကိန်းပြည့်)Rows, 0, NULL) == OK)
		    {
			// display line at top
			(void)disp_sb_line(0, mp, FALSE);
		    }
		    မဟုတ်ပါက
		    {
			ကိန်းပြည့် did_clear = screenclear();

			// redisplay all lines
			အတွက် (i = 0; mp != NULL && i < Rows - 1; ++i)
			{
			    mp = disp_sb_line(i, mp, !did_clear);
			    ++msg_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    မဟုတ်ပါက
	    {
		// First display any text that we scrolled back.
		နေစဉ် (toscroll > 0 && mp_last != NULL)
		{
		    // scroll up, display line at bottom
		    msg_scroll_up();
		    inc_msg_scrolled();
		    screen_fill((ကိန်းပြည့်)Rows - 2, (ကိန်းပြည့်)Rows - 1,
			    cmdline_col_off, cmdline_col_off + cmdline_width,
			    ' ', ' ', msg_attr);
		    mp_last = disp_sb_line((ကိန်းပြည့်)Rows - 2, mp_last, FALSE);
		    --toscroll;
		}
	    }

	    အကယ်၍ (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((ကိန်းပြည့်)Rows - 1, (ကိန်းပြည့်)Rows, cmdline_col_off,
			cmdline_col_off + cmdline_width, ' ', ' ', msg_attr);
		msg_moremsg(FALSE);
		continue;
	    }

	    // display more text, return to caller
	    lines_left = toscroll;
	}

	bha_nga;
    }

    // clear the --more-- message
    screen_fill((ကိန်းပြည့်)Rows - 1, (ကိန်းပြည့်)Rows, cmdline_col_off,
	    cmdline_col_off + cmdline_width, ' ', ' ', msg_attr);
    State = oldState;
    setmouse();
    အကယ်၍ (quit_more)
    {
	msg_row = Rows - 1;
	msg_col = 0;
    }
#ifdef FEAT_RIGHTLEFT
    မဟုတ်ပါက အကယ်၍ (cmdmsg_rl)
	msg_col = cmdline_width - 1;
#endif

    entered = FALSE;
#ifdef FEAT_CON_DIALOG
    ပြန်ပေး retval;
#မဟုတ်ပါက
    ပြန်ပေး FALSE;
#endif
}

#အကယ်၍ defined(USE_MCH_ERRMSG)

# ifdef mch_errmsg
#  undef mch_errmsg
# endif
# ifdef mch_msg
#  undef mch_msg
# endif

# အကယ်၍ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    static void
mch_errmsg_c(var_na *str)
{
    DWORD   nwrite = 0;
    DWORD   mode = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    အကယ်၍ (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (ကိန်းပြည့်)GetConsoleCP() != enc_codepage)
    {
	ကိန်းပြည့်	len = (ကိန်းပြည့်)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	အကယ်၍ (w != NULL)
	{
	    WriteConsoleW(h, w, len, &nwrite, NULL);
	    vim_free(w);
	}
    }
    မဟုတ်ပါက
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
    void
mch_errmsg(var_na *str)
{
# အကယ်၍ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    ကိန်းပြည့်		len;
# endif

# အကယ်၍ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stderr if it's a tty.
    // When not going to start the GUI also use stderr.
    // On Mac, when started from Finder, stderr is the console.
    အကယ်၍ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   မဟုတ်ပါက
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
	ပြန်ပေး;
    }
# endif

# အကယ်၍ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    အကယ်၍ (!(gui.in_use || gui.starting))
#  endif
    {
	mch_errmsg_c(str);
	ပြန်ပေး;
    }
# endif

# အကယ်၍ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    // avoid a delay for a message that isn't there
    emsg_on_display = FALSE;

    len = (ကိန်းပြည့်)STRLEN(str) + 1;
    အကယ်၍ (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    အကယ်၍ (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_data + error_ga.ga_len,
							  (char_u *)str, len);
#  ifdef UNIX
	// remove CR characters, they are displayed
	{
	    char_u	*p;

	    p = (char_u *)error_ga.ga_data + error_ga.ga_len;
	    အတွက် (;;)
	    {
		p = vim_strchr(p, '\r');
		အကယ်၍ (p == NULL)
		    bha_nga;
		*p = ' ';
	    }
	}
#  endif
	--len;		// don't count the NUL at the end
	error_ga.ga_len += len;
    }
# endif
}

# အကယ်၍ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
    static void
mch_msg_c(var_na *str)
{
    DWORD   nwrite = 0;
    DWORD   mode;
    HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

    အကယ်၍ (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (ကိန်းပြည့်)GetConsoleCP() != enc_codepage)
    {
	ကိန်းပြည့်	len = (ကိန်းပြည့်)STRLEN(str);
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	အကယ်၍ (w != NULL)
	{
	    WriteConsoleW(h, w, len, &nwrite, NULL);
	    vim_free(w);
	}
    }
    မဟုတ်ပါက
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
    void
mch_msg(var_na *str)
{
# အကယ်၍ (defined(UNIX) || defined(FEAT_GUI)) && !defined(ALWAYS_USE_GUI) && !defined(VIMDLL)
    // On Unix use stdout if we have a tty.  This allows "vim -h | more" and
    // uses mch_errmsg() when started from the desktop.
    // When not going to start the GUI also use stdout.
    // On Mac, when started from Finder, stderr is the console.
    အကယ်၍ (
#  ifdef UNIX
#   ifdef MACOS_X
	    (isatty(2) && strcmp("/dev/console", ttyname(2)) != 0)
#   မဟုတ်ပါက
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
	ပြန်ပေး;
    }
# endif

# အကယ်၍ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    အကယ်၍ (!(gui.in_use || gui.starting))
#  endif
    {
	mch_msg_c(str);
	ပြန်ပေး;
    }
# endif
# အကယ်၍ !defined(MSWIN) || defined(FEAT_GUI_MSWIN)
    mch_errmsg(str);
# endif
}
#endif // USE_MCH_ERRMSG

/*
 * Put a character on the screen at the current message position and advance
 * to the next position.  Only for printable ASCII!
 */
    static void
msg_screen_putchar(ကိန်းပြည့် c, ကိန်းပြည့် attr)
{
    msg_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, msg_row, cmdline_col_off + msg_col, attr);
#ifdef FEAT_RIGHTLEFT
    အကယ်၍ (cmdmsg_rl)
    {
	အကယ်၍ (--msg_col == 0)
	{
	    msg_col = cmdline_width;
	    ++msg_row;
	}
    }
    မဟုတ်ပါက
#endif
    {
	အကယ်၍ (++msg_col >= cmdline_width)
	{
	    msg_col = 0;
	    ++msg_row;
	}
    }
}

    static void
msg_moremsg(ကိန်းပြည့် full)
{
    ကိန်းပြည့်		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (ကိန်းပြည့်)Rows - 1, cmdline_col_off, attr);
    အကယ်၍ (full)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(ကိန်းပြည့်)Rows - 1, cmdline_col_off + vim_strsize(s), attr);
}

/*
 * Repeat the message for the current mode: MODE_ASKMORE, MODE_EXTERNCMD,
 * MODE_CONFIRM or exmode_active.
 */
    void
repeat_message(void)
{
    အကယ်၍ (State == MODE_ASKMORE)
    {
	msg_moremsg(TRUE);	// display --more-- message again
	msg_row = Rows - 1;
    }
#ifdef FEAT_CON_DIALOG
    မဟုတ်ပါက အကယ်၍ (State == MODE_CONFIRM)
    {
	display_confirm_msg();	// display ":confirm" message again
	msg_row = Rows - 1;
    }
#endif
    မဟုတ်ပါက အကယ်၍ (State == MODE_EXTERNCMD)
    {
	windgoto(msg_row, cmdline_col_off + msg_col); // put cursor back
    }
    မဟုတ်ပါက အကယ်၍ (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	အကယ်၍ (msg_row == Rows - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    msg_didout = FALSE;
	    msg_col = 0;
	    msg_clr_eos();
	}
	hit_return_msg();
	msg_row = Rows - 1;
    }
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    static ကိန်းပြည့်
msg_check_screen(void)
{
    အကယ်၍ (!full_screen || !screen_valid(FALSE))
	ပြန်ပေး FALSE;

    အကယ်၍ (msg_row >= Rows)
	msg_row = Rows - 1;
    အကယ်၍ (msg_col >= cmdline_width)
	msg_col = cmdline_width - 1;
    ပြန်ပေး TRUE;
}

/*
 * Clear from current message position to end of screen.
 * Skip this when ":silent" was used, no need to clear for redirection.
 */
    void
msg_clr_eos(void)
{
    အကယ်၍ (msg_silent == 0)
	msg_clr_eos_force();
}

/*
 * Clear from current message position to end of screen.
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    void
msg_clr_eos_force(void)
{
#ifdef HAS_MESSAGE_WINDOW
    အကယ်၍ (in_echowindow)
	ပြန်ပေး;  // messages go into a popup
#endif
    အကယ်၍ (msg_use_printf())
    {
	အကယ်၍ (full_screen)	// only when termcap codes are valid
	{
	    အကယ်၍ (*T_CD)
		out_str(T_CD);	// clear to end of display
	    မဟုတ်ပါက အကယ်၍ (*T_CE)
		out_str(T_CE);	// clear to end of line
	}
    }
    မဟုတ်ပါက
    {
	ကိန်းပြည့် msg_attr;

	msg_attr = HL_ATTR(HLF_MSG);

#ifdef FEAT_RIGHTLEFT
	အကယ်၍ (cmdmsg_rl)
	{
	    screen_fill(msg_row, msg_row + 1,
		    cmdline_col_off, cmdline_col_off + msg_col + 1,
		    ' ', ' ', msg_attr);
	    screen_fill(msg_row + 1, (ကိန်းပြည့်)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', msg_attr);
	}
	မဟုတ်ပါက
#endif
	{
	    screen_fill(msg_row, msg_row + 1,
		    cmdline_col_off + msg_col, cmdline_col_off + cmdline_width,
		    ' ', ' ', msg_attr);
	    screen_fill(msg_row + 1, (ကိန်းပြည့်)Rows,
		    cmdline_col_off, cmdline_col_off + cmdline_width,
		    ' ', ' ', msg_attr);
	}
    }
}

/*
 * Clear the command line.
 */
    void
msg_clr_cmdline(void)
{
    msg_row = cmdline_row;
    msg_col = 0;
    msg_clr_eos_force();
}

/*
 * end putting a message on the screen
 * call wait_return() if the message does not fit in the available space
 * return TRUE if wait_return() not called.
 */
    ကိန်းပြည့်
msg_end(void)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    အကယ်၍ (!exiting && need_wait_return && !(State & MODE_CMDLINE))
    {
	wait_return(FALSE);
	ပြန်ပေး FALSE;
    }
    out_flush();
    ပြန်ပေး TRUE;
}

/*
 * If the written message runs into the shown command or ruler, we have to
 * wait for hit-return and redraw the window later.
 */
    void
msg_check(void)
{
    အကယ်၍ (msg_row == Rows - 1 && msg_col >= sc_col
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
    static void
redir_write(char_u *str, ကိန်းပြည့် maxlen)
{
    char_u	*s = str;
    static ကိန်းပြည့်	cur_col = 0;

    // Don't do anything for displaying prompts and the like.
    အကယ်၍ (redir_off)
	ပြန်ပေး;

    // If 'verbosefile' is set prepare for writing in that file.
    အကယ်၍ (*p_vfile != NUL && verbose_fd == NULL)
	verbose_open();

    အကယ်၍ (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	အကယ်၍ (*s != '\n' && *s != '\r')
	{
	    နေစဉ် (cur_col < msg_col)
	    {
#ifdef FEAT_EVAL
		အကယ်၍ (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		မဟုတ်ပါက အကယ်၍ (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		မဟုတ်ပါက အကယ်၍ (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		မဟုတ်ပါက
#endif
		    အကယ်၍ (redir_fd != NULL)
		    fputs(" ", redir_fd);
		အကယ်၍ (verbose_fd != NULL)
		    fputs(" ", verbose_fd);
		++cur_col;
	    }
	}

#ifdef FEAT_EVAL
	အကယ်၍ (redir_execute)
	    execute_redir_str(s, maxlen);
	မဟုတ်ပါက အကယ်၍ (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	မဟုတ်ပါက အကယ်၍ (redir_vname)
	    var_redir_str(s, maxlen);
#endif

	// Write and adjust the current column.
	နေစဉ် (*s != NUL && (maxlen < 0 || (ကိန်းပြည့်)(s - str) < maxlen))
	{
#ifdef FEAT_EVAL
	    အကယ်၍ (!redir_reg && !redir_vname && !redir_execute)
#endif
		အကယ်၍ (redir_fd != NULL)
		    putc(*s, redir_fd);
	    အကယ်၍ (verbose_fd != NULL)
		putc(*s, verbose_fd);
	    အကယ်၍ (*s == '\r' || *s == '\n')
		cur_col = 0;
	    မဟုတ်ပါက အကယ်၍ (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    မဟုတ်ပါက
		++cur_col;
	    ++s;
	}

	အကယ်၍ (msg_silent != 0)	// should update msg_col
	    msg_col = cur_col;
    }
}

    ကိန်းပြည့်
redirecting(void)
{
    ပြန်ပေး redir_fd != NULL || *p_vfile != NUL
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname || redir_execute
#endif
				       ;
}

/*
 * Before giving verbose message.
 * Must always be called paired with verbose_leave()!
 */
    void
verbose_enter(void)
{
    အကယ်၍ (*p_vfile != NUL)
	++msg_silent;
}

/*
 * After giving verbose message.
 * Must always be called paired with verbose_enter()!
 */
    void
verbose_leave(void)
{
    အကယ်၍ (*p_vfile != NUL)
	အကယ်၍ (--msg_silent < 0)
	    msg_silent = 0;
}

/*
 * Like verbose_enter() and set msg_scroll when displaying the message.
 */
    void
verbose_enter_scroll(void)
{
    အကယ်၍ (*p_vfile != NUL)
	++msg_silent;
    မဟုတ်ပါက
	// always scroll up, don't overwrite
	msg_scroll = TRUE;
}

/*
 * Like verbose_leave() and set cmdline_row when displaying the message.
 */
    void
verbose_leave_scroll(void)
{
    အကယ်၍ (*p_vfile != NUL)
    {
	အကယ်၍ (--msg_silent < 0)
	    msg_silent = 0;
    }
    မဟုတ်ပါက
	cmdline_row = msg_row;
}

/*
 * Called when 'verbosefile' is set: stop writing to the file.
 */
    void
verbose_stop(void)
{
    အကယ်၍ (verbose_fd != NULL)
    {
	fclose(verbose_fd);
	verbose_fd = NULL;
    }
    verbose_did_open = FALSE;
}

/*
 * Open the file 'verbosefile'.
 * Return FAIL or OK.
 */
    ကိန်းပြည့်
verbose_open(void)
{
    အကယ်၍ (verbose_fd == NULL && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((var_na *)p_vfile, "a");
	အကယ်၍ (verbose_fd == NULL)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    ပြန်ပေး FAIL;
	}
    }
    ပြန်ပေး OK;
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    void
give_warning(char_u *message, ကိန်းပြည့် hl)
{
    give_warning_with_source(message, hl, FALSE);
}

    void
give_warning_with_source(char_u *message, ကိန်းပြည့် hl, ကိန်းပြည့် with_source)
{
    // Don't do this for ":silent".
    အကယ်၍ (msg_silent != 0)
	ပြန်ပေး;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, message, -1);
#endif
    VIM_CLEAR(keep_msg);
    အကယ်၍ (hl)
	keep_msg_attr = HL_ATTR(HLF_W);
    မဟုတ်ပါက
	keep_msg_attr = 0;

    အကယ်၍ (with_source)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	msg_start();
	msg_source(HL_ATTR(HLF_W));
	msg_puts(" ");
	msg_puts_attr((var_na *)message, HL_ATTR(HLF_W) | MSG_HIST);
	msg_clr_eos();
	(void)msg_end();
    }
    မဟုတ်ပါက အကယ်၍ (msg_attr((var_na *)message, keep_msg_attr) && msg_scrolled == 0)
	set_keep_msg(message, keep_msg_attr);

    msg_didout = FALSE;	    // overwrite this message
    msg_nowait = TRUE;	    // don't wait for this message
    msg_col = 0;

    --no_wait_return;
}

#အကယ်၍ defined(FEAT_EVAL)
    void
give_warning2(char_u *message, char_u *a1, ကိန်းပြည့် hl)
{
    အကယ်၍ (IObuff == NULL)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(message, hl);
    }
    မဟုတ်ပါက
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, (var_na *)message, a1);
	give_warning(IObuff, hl);
    }
}
#endif

/*
 * Advance msg cursor to column "col".
 */
    void
msg_advance(ကိန်းပြည့် col)
{
    အကယ်၍ (msg_silent != 0)	// nothing to advance to
    {
	msg_col = col;		// for redirection, may fill it up later
	ပြန်ပေး;
    }
    အကယ်၍ (col >= cmdline_width)	// not enough room
	col = cmdline_width - 1;
#ifdef FEAT_RIGHTLEFT
    အကယ်၍ (cmdmsg_rl)
	နေစဉ် (msg_col > cmdline_width - col)
	    msg_putchar(' ');
    မဟုတ်ပါက
#endif
	နေစဉ် (msg_col < col)
	    msg_putchar(' ');
}

/*
 * Warn about missing Clipboard Support
 */
    void
msg_warn_missing_clipboard(void)
{
    အကယ်၍ (!global_busy && !did_warn_clipboard && silence_w23_w24_msg == 0)
    {
#ifdef FEAT_CLIPBOARD
	msg(_("W23: Clipboard register not available, using register 0"));
#မဟုတ်ပါက
	msg(_("W24: Clipboard register not available. See :h W24"));
#endif
	did_warn_clipboard = TRUE;
    }
}

#အကယ်၍ defined(FEAT_CON_DIALOG)
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
    ကိန်းပြည့်
do_dialog(
    ကိန်းပြည့်		type UNUSED,
    char_u	*title UNUSED,
    char_u	*message,
    char_u	*buttons,
    ကိန်းပြည့်		dfltbutton,
    char_u	*textfield UNUSED,	// IObuff for inputdialog(), NULL
					// otherwise
    ကိန်းပြည့်		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    ကိန်းပြည့်		oldState;
    ကိန်းပြည့်		retval = 0;
    char_u	*hotkeys;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		i;
    tmode_T	save_tmode;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    အကယ်၍ (silent_mode)
	ပြန်ပေး dfltbutton;   // return default option
# endif

# ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    အကယ်၍ (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == NULL)
    {
	// --gui-dialog-file: write text to a file
	အကယ်၍ (gui_dialog_log(title, message))
	    c = dfltbutton;
	မဟုတ်ပါက
	    c = gui_mch_dialog(type, title, message, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_row = msg_row;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	out_flush();
	gui_mch_update();

	ပြန်ပေး c;
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
    hotkeys = msg_show_console_dialog(message, buttons, dfltbutton);

    အကယ်၍ (hotkeys != NULL)
    {
	အတွက် (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    switch (c)
	    {
	    pak_sa CAR:		// User accepts default option
	    pak_sa NL:
		retval = dfltbutton;
		bha_nga;
	    pak_sa Ctrl_C:	// User aborts/cancels
	    pak_sa ESC:
		retval = 0;
		bha_nga;
	    default:		// Could be a hotkey?
		အကယ်၍ (c < 0)	// special keys are ignored here
		    continue;
		အကယ်၍ (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    bha_nga;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		အတွက် (i = 0; hotkeys[i]; ++i)
		{
		    အကယ်၍ (has_mbyte)
		    {
			အကယ်၍ ((*mb_ptr2char)(hotkeys + i) == c)
			    bha_nga;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    မဟုတ်ပါက
			အကယ်၍ (hotkeys[i] == c)
			    bha_nga;
		    ++retval;
		}
		အကယ်၍ (hotkeys[i])
		    bha_nga;
		// No hotkey match, so keep waiting
		continue;
	    }
	    bha_nga;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    msg_end_prompt();

    ပြန်ပေး retval;
}

/*
 * Copy one character from "*from" to "*to", taking care of multi-byte
 * characters.  Return the length of the character in bytes.
 */
    static ကိန်းပြည့်
copy_char(
    char_u	*from,
    char_u	*to,
    ကိန်းပြည့်		lowercase)	// make character lower case
{
    ကိန်းပြည့်		len;
    ကိန်းပြည့်		c;

    အကယ်၍ (has_mbyte)
    {
	အကယ်၍ (lowercase)
	{
	    c = MB_TOLOWER((*mb_ptr2char)(from));
	    ပြန်ပေး (*mb_char2bytes)(c, to);
	}
	မဟုတ်ပါက
	{
	    len = (*mb_ptr2len)(from);
	    mch_memmove(to, from, (size_t)len);
	    ပြန်ပေး len;
	}
    }
    မဟုတ်ပါက
    {
	အကယ်၍ (lowercase)
	    *to = (char_u)TOLOWER_LOC(*from);
	မဟုတ်ပါက
	    *to = *from;
	ပြန်ပေး 1;
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
    static char_u *
msg_show_console_dialog(
    char_u	*message,
    char_u	*buttons,
    ကိန်းပြည့်		dfltbutton)
{
    ကိန်းပြည့်		len = 0;
# define HOTK_LEN (has_mbyte ? MB_MAXBYTES : 1)
    ကိန်းပြည့်		lenhotkey = HOTK_LEN;	// count first button
    char_u	*hotk = NULL;
    char_u	*msgp = NULL;
    char_u	*hotkp = NULL;
    char_u	*r;
    ကိန်းပြည့်		copy;
# define HAS_HOTKEY_LEN 30
    char_u	has_hotkey[HAS_HOTKEY_LEN];
    ကိန်းပြည့်		first_hotkey = FALSE;	// first char of button is hotkey
    ကိန်းပြည့်		idx;

    has_hotkey[0] = FALSE;

    /*
     * First loop: compute the size of memory to allocate.
     * Second loop: copy to the allocated memory.
     */
    အတွက် (copy = 0; copy <= 1; ++copy)
    {
	r = buttons;
	idx = 0;
	နေစဉ် (*r)
	{
	    အကယ်၍ (*r == DLG_BUTTON_SEP)
	    {
		အကယ်၍ (copy)
		{
		    *msgp++ = ',';
		    *msgp++ = ' ';	    // '\n' -> ', '

		    // advance to next hotkey and set default hotkey
		    အကယ်၍ (has_mbyte)
			hotkp += STRLEN(hotkp);
		    မဟုတ်ပါက
			++hotkp;
		    hotkp[copy_char(r + 1, hotkp, TRUE)] = NUL;
		    အကယ်၍ (dfltbutton)
			--dfltbutton;

		    // If no hotkey is specified first char is used.
		    အကယ်၍ (idx < HAS_HOTKEY_LEN - 1 && !has_hotkey[++idx])
			first_hotkey = TRUE;
		}
		မဟုတ်ပါက
		{
		    len += 3;		    // '\n' -> ', '; 'x' -> '(x)'
		    lenhotkey += HOTK_LEN;  // each button needs a hotkey
		    အကယ်၍ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[++idx] = FALSE;
		}
	    }
	    မဟုတ်ပါက အကယ်၍ (*r == DLG_HOTKEY_CHAR || first_hotkey)
	    {
		အကယ်၍ (*r == DLG_HOTKEY_CHAR)
		    ++r;
		first_hotkey = FALSE;
		အကယ်၍ (copy)
		{
		    အကယ်၍ (*r == DLG_HOTKEY_CHAR)		// '&&a' -> '&a'
			*msgp++ = *r;
		    မဟုတ်ပါက
		    {
			// '&a' -> '[a]'
			*msgp++ = (dfltbutton == 1) ? '[' : '(';
			msgp += copy_char(r, msgp, FALSE);
			*msgp++ = (dfltbutton == 1) ? ']' : ')';

			// redefine hotkey
			hotkp[copy_char(r, hotkp, TRUE)] = NUL;
		    }
		}
		မဟုတ်ပါက
		{
		    ++len;	    // '&a' -> '[a]'
		    အကယ်၍ (idx < HAS_HOTKEY_LEN - 1)
			has_hotkey[idx] = TRUE;
		}
	    }
	    မဟုတ်ပါက
	    {
		// everything else copy literally
		အကယ်၍ (copy)
		    msgp += copy_char(r, msgp, FALSE);
	    }

	    // advance to the next character
	    MB_PTR_ADV(r);
	}

	အကယ်၍ (copy)
	{
	    *msgp++ = ':';
	    *msgp++ = ' ';
	    *msgp = NUL;
	}
	မဟုတ်ပါက
	{
	    len += (ကိန်းပြည့်)(STRLEN(message)
			+ 2			// for the NL's
			+ STRLEN(buttons)
			+ 3);			// for the ": " and NUL
	    lenhotkey++;			// for the NUL

	    // If no hotkey is specified first char is used.
	    အကယ်၍ (!has_hotkey[0])
	    {
		first_hotkey = TRUE;
		len += 2;		// "x" -> "[x]"
	    }

	    /*
	     * Now allocate and load the strings
	     */
	    vim_free(confirm_msg);
	    confirm_msg = alloc(len);
	    အကယ်၍ (confirm_msg == NULL)
		ပြန်ပေး NULL;
	    *confirm_msg = NUL;
	    hotk = alloc(lenhotkey);
	    အကယ်၍ (hotk == NULL)
		ပြန်ပေး NULL;

	    *confirm_msg = '\n';
	    STRCPY(confirm_msg + 1, message);

	    msgp = confirm_msg + 1 + STRLEN(message);
	    hotkp = hotk;

	    // Define first default hotkey.  Keep the hotkey string NUL
	    // terminated to avoid reading past the end.
	    hotkp[copy_char(buttons, hotkp, TRUE)] = NUL;

	    // Remember where the choices start, displaying starts here when
	    // "hotkp" typed at the more prompt.
	    confirm_msg_tail = msgp;
	    *msgp++ = '\n';
	}
    }

    display_confirm_msg();
    ပြန်ပေး hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    static void
display_confirm_msg(void)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_msg_used;
    အကယ်၍ (confirm_msg != NULL)
	msg_puts_attr((var_na *)confirm_msg, HL_ATTR(HLF_M));
    --confirm_msg_used;
}

#endif // FEAT_CON_DIALOG

#အကယ်၍ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)

    ကိန်းပြည့်
vim_dialog_yesno(
    ကိန်းပြည့်		type,
    char_u	*title,
    char_u	*message,
    ကိန်းပြည့်		dflt)
{
    အကယ်၍ (do_dialog(type,
		title == NULL ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No"), dflt, NULL, FALSE) == 1)
	ပြန်ပေး VIM_YES;
    ပြန်ပေး VIM_NO;
}

    ကိန်းပြည့်
vim_dialog_yesnocancel(
    ကိန်းပြည့်		type,
    char_u	*title,
    char_u	*message,
    ကိန်းပြည့်		dflt)
{
    switch (do_dialog(type,
		title == NULL ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, NULL, FALSE))
    {
	pak_sa 1: ပြန်ပေး VIM_YES;
	pak_sa 2: ပြန်ပေး VIM_NO;
    }
    ပြန်ပေး VIM_CANCEL;
}

    ကိန်းပြည့်
vim_dialog_yesnoallcancel(
    ကိန်းပြည့်		type,
    char_u	*title,
    char_u	*message,
    ကိန်းပြည့်		dflt)
{
    switch (do_dialog(type,
		title == NULL ? (char_u *)"Question" : title,
		message,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, NULL, FALSE))
    {
	pak_sa 1: ပြန်ပေး VIM_YES;
	pak_sa 2: ပြန်ပေး VIM_NO;
	pak_sa 3: ပြန်ပေး VIM_ALL;
	pak_sa 4: ပြန်ပေး VIM_DISCARDALL;
    }
    ပြန်ပေး VIM_CANCEL;
}

#endif // FEAT_GUI_DIALOG || FEAT_CON_DIALOG
