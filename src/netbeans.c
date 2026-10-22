/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Netbeans integration by David Weatherford
 *			Adopted for Win32 by Sergey Khorev
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Implements client side of org.netbeans.modules.emacs editor
 * integration protocol.  Be careful!  The protocol uses offsets
 * which are *between* characters, whereas vim uses line number
 * and column number which are *on* characters.
 * See ":help netbeans-protocol" for explanation.
 *
 * The Netbeans messages are received and queued in the gui event loop, or in
 * the select loop when Vim runs in a terminal. These messages are processed
 * by netbeans_parse_messages() which is invoked in the idle loop when Vim is
 * waiting for user input. The function netbeans_parse_messages() is also
 * called from the ":sleep" command, to allow the execution of test cases that
 * may not invoke the idle loop.
 */

#ضمن "vim.h"

#إذا defined(FEAT_NETBEANS_INTG)

#که_نه_تعریف_شوی MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#د_که_پای

#ضمن "version.h"

#تعریف_کړه GUARDED		10000 // typenr for "guarded" annotation
#تعریف_کړه GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#تعریف_کړه MAX_COLOR_اوږدتیا 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#تعریف_کړه VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#تعریف_کړه VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#تعریف_کړه VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
ساكن حرف *Ext_Ed_Protocol_نسخه = "2.5";

ساكن اوږد pos2off(buf_T *, pos_T *);
ساكن pos_T *off2pos(buf_T *, اوږد);
ساكن pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
ساكن اوږد get_buf_اندازه(buf_T *);
ساكن صحيح netbeans_keystring(char_u *keystr);
ساكن فراغ special_کلیګانې(char_u *دلیلونه);

ساكن صحيح getConnInfo(حرف *file, حرف **host, حرف **پورټ, حرف **password);

ساكن فراغ nb_init_graphics(فراغ);
ساكن فراغ coloncmd(حرف *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ساكن فراغ nb_سېټ_curbuf(buf_T *buf);
ساكن فراغ nb_parse_cmd(char_u *);
ساكن صحيح  nb_do_cmd(صحيح, char_u *, صحيح, صحيح, char_u *);
ساكن فراغ nb_send(حرف *buf, حرف *fun);
ساكن فراغ nb_free(فراغ);

#تعریف_کړه NETBEANS_OPEN (channel_can_write_منزل(nb_channel))
ساكن channel_T *nb_channel = عدم;

ساكن صحيح r_cmdno;			// current command number for reply
ساكن صحيح dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#که_تعریف_شوی NBDEBUG
# include "nbdebug.c"
#د_که_پای

ساكن صحيح needupdate = 0;
ساكن صحيح دننه_Atomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    ساكن صحيح
nb_is_خوندي_string(char_u *str, char_u *allowed)
{
    إذا (str == عدم)
	أرجع FALSE;
    لكل (char_u *p = str; *p != NUL; p++)
	إذا (vim_strchr(allowed, *p) == عدم)
	    أرجع FALSE;
    أرجع TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    ساكن فراغ
nb_channel_closed(فراغ)
{
    nb_channel = عدم;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    ساكن فراغ
netbeans_close(فراغ)
{
    إذا (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	إذا (nb_channel != عدم)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = عدم;
    }

#که_تعریف_شوی FEAT_BEVAL_GUI
    bevalServers &= ~BEVAL_NETBEANS;
#د_که_پای

    needupdate = 0;
    دننه_Atomic = 0;
    nb_free();

    // remove all signs and update the screen after gutter removal
    coloncmd(":sign unplace *");
    بدل_شوی_کړکۍ_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    بهر_flush_cursor(TRUE, FALSE);
}

#تعریف_کړه NB_DEF_HOST "localhost"
#تعریف_کړه NB_DEF_ADDR "3219"
#تعریف_کړه NB_DEF_PASS "changeme"

    ساكن صحيح
netbeans_connect(حرف *params, صحيح doabort)
{
    صحيح		پورټ;
    حرف	buf[32];
    حرف	*hostname = عدم;
    حرف	*پته = عدم;
    حرف	*password = عدم;
    حرف	*fname;
    حرف	*arg = عدم;

    إذا (*params == '=')
    {
	// "=fname": Read info from specified file.
	إذا (getConnInfo(params + 1, &hostname, &پته, &password) == FAIL)
	    أرجع FAIL;
    }
    وإلا
    {
	إذا (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	إذا (arg == عدم && (fname = getenv("__NETBEANS_CONINFO")) != عدم)
	{
	    // "": get info from file specified in environment
	    إذا (getConnInfo(fname, &hostname, &پته, &password) == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    إذا (arg != عدم)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		پته = strchr(hostname, ':');
		إذا (پته != عدم)
		{
		    *پته++ = '\0';
		    password = strchr(پته, ':');
		    إذا (password != عدم)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    إذا (hostname == عدم || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    إذا (پته == عدم)
		پته = getenv("__NETBEANS_SOCKET");
	    إذا (password == عدم)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    إذا (hostname != عدم)
		hostname = (حرف *)vim_strsave((char_u *)hostname);
	    إذا (پته != عدم)
		پته = (حرف *)vim_strsave((char_u *)پته);
	    إذا (password != عدم)
		password = (حرف *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    إذا (hostname == عدم || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (حرف *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    إذا (پته == عدم || *پته == '\0')
    {
	vim_free(پته);
	پته = (حرف *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    إذا (password == عدم || *password == '\0')
    {
	vim_free(password);
	password = (حرف *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    إذا (hostname != عدم && پته != عدم && password != عدم)
    {
	پورټ = atoi(پته);
	nb_channel = channel_open(hostname, پورټ, 3000, nb_channel_closed);
	إذا (nb_channel != عدم)
	{
	    // success
#که_تعریف_شوی FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#د_که_پای

	    // success, login
	    vim_snprintf(buf, حجم(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", Ext_Ed_Protocol_نسخه);
	    nb_send(buf, "externaleditor_version");
	}
    }

    إذا (nb_channel == عدم && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(پته);
    vim_free(password);
    أرجع NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    ساكن صحيح
getConnInfo(حرف *file, حرف **host, حرف **پورټ, حرف **auth)
{
    FILE *fp;
    char_u buf[BUFSIZ];
    char_u *lp;
    char_u *nlp;
#که_تعریف_شوی UNIX
    stat_T	st;

    /*
     * For Unix only accept the file when it's not accessible by others.
     * The open will then fail if we don't own the file.
     */
    إذا (mch_stat(file, &st) == 0 && (st.st_ډېرپېښ & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_ډېرپېښ_for_netbeans_connection_info_file_str),
									file);
	أرجع FAIL;
    }
#د_که_پای

    fp = mch_fopen(file, "r");
    إذا (fp == عدم)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	أرجع FAIL;
    }

    // Read the file. There should be one of each parameter
    طالما ((lp = (char_u *)fgets((حرف *)buf, BUFSIZ, fp)) != عدم)
    {
	إذا ((nlp = vim_strchr(lp, '\n')) != عدم)
	    *nlp = 0;	    // strip off the trailing newline

	إذا (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (حرف *)vim_strsave(&buf[5]);
	}
	وإلا إذا (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*پورټ);
	    *پورټ = (حرف *)vim_strsave(&buf[5]);
	}
	وإلا إذا (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (حرف *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    أرجع OK;
}


بنية keyqueue
{
    char_u	    *keystr;
    بنية keyqueue *بل;
    بنية keyqueue *prev;
};

تعريفنوع بنية keyqueue کلی_Q_T;

ساكن کلی_Q_T کلی_سر; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    ساكن فراغ
postpone_keycommand(char_u *keystr)
{
    کلی_Q_T *غوټه;

    غوټه = ALLOC_ONE(کلی_Q_T);
    إذا (غوټه == عدم)
	أرجع;  // out of memory, drop the key

    إذا (کلی_سر.بل == عدم) // initialize circular queue
    {
	کلی_سر.بل = &کلی_سر;
	کلی_سر.prev = &کلی_سر;
    }

    // insert node at tail of queue
    غوټه->بل = &کلی_سر;
    غوټه->prev = کلی_سر.prev;
    کلی_سر.prev->بل = غوټه;
    کلی_سر.prev = غوټه;

    غوټه->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    ساكن فراغ
handle_کلی_کتار(فراغ)
{
    صحيح postponed = FALSE;

    طالما (!postponed && کلی_سر.بل && کلی_سر.بل != &کلی_سر)
    {
	// first, unlink the node
	کلی_Q_T *غوټه = کلی_سر.بل;
	کلی_سر.بل = غوټه->بل;
	غوټه->بل->prev = غوټه->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	إذا (غوټه->keystr != عدم)
	    postponed = !netbeans_keystring(غوټه->keystr);
	vim_free(غوټه->keystr);

	// Finally, dispose of the node
	vim_free(غوټه);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    فراغ
netbeans_parse_messages(فراغ)
{
    readq_T	*غوټه;
    char_u	*بفر;
    char_u	*p;
    صحيح		own_غوټه;

    طالما (nb_channel != عدم)
    {
	غوټه = channel_peek(nb_channel, PART_SOCK);
	إذا (غوټه == عدم)
	    اكسر;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_لومړی_nl(غوټه);
	إذا (p == عدم)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    إذا (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		أرجع;
	    استمر;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	إذا (*p == NUL)
	{
	    own_غوټه = TRUE;
	    بفر = channel_get(nb_channel, PART_SOCK, عدم);
	    // "node" is now invalid!
	}
	وإلا
	{
	    own_غوټه = FALSE;
	    بفر = غوټه->rq_بفر;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(بفر);

	إذا (own_غوټه)
	    // buffer finished, dispose of it
	    vim_free(بفر);
	وإلا إذا (nb_channel != عدم)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (صحيح)(p - بفر));
    }
}

/*
 * Handle one NUL terminated command.
 *
 * format of a command from netbeans:
 *
 *    6:setTitle!84 "a.c"
 *
 *    bufno
 *     colon
 *      cmd
 *		!
 *		 cmdno
 *		    args
 *
 * for function calls, the ! is replaced by a /
 */
    ساكن فراغ
nb_parse_cmd(char_u *cmd)
{
    حرف	*verb;
    حرف	*q;
    صحيح		bufno;
    صحيح		isfunc = -1;

    إذا (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    إذا (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_ستنه = false;

	// The IDE is breaking the connection.
	netbeans_close();
	أرجع;
    }

    bufno = strtol((حرف *)cmd, &verb, 10);

    إذا (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	أرجع;
    }
    ++verb; // skip colon

    لكل (q = verb; *q; q++)
    {
	إذا (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    اكسر;
	}
	وإلا إذا (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    اكسر;
	}
    }

    إذا (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_دننه_str), cmd);
	أرجع;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (حرف *)skipwhite((char_u *)q);

    إذا (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
    {
#که_تعریف_شوی NBDEBUG
	/*
	 * This happens because the ExtEd can send a command or 2 after
	 * doing a stopDocumentListen command. It doesn't harm anything
	 * so I'm disabling it except for debugging.
	 */
	nbdebug(("nb_parse_cmd: Command error for \"%s\"\n", cmd));
	emsg(e_bad_return_from_nb_do_cmd);
#د_که_پای
    }
}

بنية nbbuf_struct
{
    buf_T		*bufp;
    بینښه صحيح	 fireChanges:1;
    بینښه صحيح	 init_بشپړ:1;
    بینښه صحيح	 ورننباسه_بشپړ:1;
    بینښه صحيح	 modified:1;
    صحيح			 nbbuf_شمېره;
    حرف		*displayname;
    صحيح			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

تعريفنوع بنية nbbuf_struct nbbuf_T;

ساكن nbbuf_T *buf_list = عدم;
ساكن صحيح buf_list_اندازه = 0;	// size of buf_list
ساكن صحيح buf_list_کارول_شوی = 0;	// nr of entries in buf_list actually in use

ساكن حرف **globalsignmap = عدم;
ساكن صحيح globalsignmaplen = 0;
ساكن صحيح globalsignmapused = 0;

ساكن صحيح  mapsigntype(nbbuf_T *, صحيح localsigntype);
ساكن فراغ addsigntype(nbbuf_T *, صحيح localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
ساكن فراغ print_read_پیغام(nbbuf_T *buf);
ساكن فراغ print_save_پیغام(nbbuf_T *buf, off_T nchars);

ساكن صحيح curPCtype = -1;

/*
 * Free netbeans resources.
 */
    ساكن فراغ
nb_free(فراغ)
{
    کلی_Q_T *کلی_غوټه = کلی_سر.بل;
    nbbuf_T buf;
    صحيح i;

    // free the netbeans buffer list
    لكل (i = 0; i < buf_list_کارول_شوی; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	إذا (buf.bufp != عدم && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_اندازه = 0;
    buf_list_کارول_شوی = 0;

    // free the queued key commands
    طالما (کلی_غوټه != عدم && کلی_غوټه != &کلی_سر)
    {
	کلی_Q_T *بل = کلی_غوټه->بل;
	vim_free(کلی_غوټه->keystr);
	vim_free(کلی_غوټه);
	إذا (بل == &کلی_سر)
	{
	    کلی_سر.بل = &کلی_سر;
	    کلی_سر.prev = &کلی_سر;
	    اكسر;
	}
	کلی_غوټه = بل;
    }

    // free the queued netbeans commands
    إذا (nb_channel != عدم)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    ساكن صحيح
nb_getbufno(buf_T *bufp)
{
    صحيح i;

    لكل (i = 0; i < buf_list_کارول_شوی; i++)
	إذا (buf_list[i].bufp == bufp)
	    أرجع i;
    أرجع -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    صحيح
is_Netbeans_بفر(buf_T *bufp)
{
    أرجع NETBEANS_OPEN && bufp->b_netbeans_file;
}

/*
 * NetBeans and Vim have different undo models. In Vim, the file isn't
 * changed if changes are undone via the undo command. In NetBeans, once
 * a change has been made the file is marked as modified until saved. It
 * doesn't matter if the change was undone.
 *
 * So this function is for the corner case where Vim thinks a buffer is
 * unmodified but NetBeans thinks it IS modified.
 */
    صحيح
isNetbeansModified(buf_T *bufp)
{
    إذا (is_Netbeans_بفر(bufp))
    {
	صحيح bufno = nb_getbufno(bufp);

	إذا (bufno > 0)
	    أرجع buf_list[bufno].modified;
	وإلا
	    أرجع FALSE;
    }
    وإلا
	أرجع FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    ساكن nbbuf_T *
nb_get_buf(صحيح bufno)
{
    // find or create a buffer with the given number
    صحيح incr;

    إذا (bufno <= 0)
	أرجع عدم;

    إذا (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * حجم(nbbuf_T));
	buf_list_اندازه = 100;
    }
    إذا (bufno >= buf_list_کارول_شوی) // new
    {
	إذا (bufno >= buf_list_اندازه) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_اندازه + 90;
	    buf_list_اندازه += incr;
	    bufsize = buf_list_اندازه * حجم(nbbuf_T);
	    إذا (bufsize == 0 || bufsize / حجم(nbbuf_T)
						      != (size_t)buf_list_اندازه)
	    {
		// list size overflow, bail out
		أرجع عدم;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    إذا (buf_list == عدم)
	    {
		vim_free(t_buf_list);
		buf_list_اندازه = 0;
		أرجع عدم;
	    }
	    vim_memset(buf_list + buf_list_اندازه - incr, 0,
						      incr * حجم(nbbuf_T));
	}

	طالما (buf_list_کارول_شوی <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_کارول_شوی].fireChanges = 1;
	    ++buf_list_کارول_شوی;
	}
    }

    أرجع buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    ساكن صحيح
شمېر_بدل_شوی_buffers(فراغ)
{
    buf_T	*bufp;
    صحيح		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	إذا (bufp->b_بدل_شوی)
	    ++n;
    أرجع n;
}

/*
 * End the netbeans session.
 */
    فراغ
netbeans_پای_ټکی(فراغ)
{
    صحيح i;
    ساكن حرف buf[128];

    إذا (!NETBEANS_OPEN)
	أرجع;

    لكل (i = 0; i < buf_list_کارول_شوی; i++)
    {
	إذا (!buf_list[i].bufp)
	    استمر;
	إذا (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, عدم);
	buf_list[i].bufp = عدم;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    ساكن فراغ
nb_send(حرف *buf, حرف *fun)
{
    إذا (nb_channel != عدم)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (صحيح)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    ساكن فراغ
nb_ځواب_nil(صحيح cmdno)
{
    حرف ځواب[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    إذا (!NETBEANS_OPEN)
	أرجع;

    sprintf(ځواب, "%d\n", cmdno);
    nb_send(ځواب, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    ساكن فراغ
nb_ځواب_متن(صحيح cmdno, char_u *پایله)
{
    char_u *ځواب;

    nbdebug(("REP %d: %s\n", cmdno, (حرف *)پایله));

    ځواب = alloc(STRLEN(پایله) + 32);
    sprintf((حرف *)ځواب, "%d %s\n", cmdno, (حرف *)پایله);
    nb_send((حرف *)ځواب, "nb_reply_text");

    vim_free(ځواب);
}


/*
 * Send a response with a number result code.
 */
    ساكن فراغ
nb_ځواب_nr(صحيح cmdno, اوږد پایله)
{
    حرف ځواب[32];

    nbdebug(("REP %d: %ld\n", cmdno, پایله));

    sprintf(ځواب, "%d %ld\n", cmdno, پایله);
    nb_send(ځواب, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    ساكن char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    إذا (buf == عدم)
	أرجع عدم;
    لكل (; *p; p++)
    {
	بدل (*p)
	{
	    حالة '\"':
	    حالة '\\':
		*q++ = '\\'; *q++ = *p; اكسر;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    حالة '\n':
		*q++ = '\\'; *q++ = 'n'; اكسر;
	    حالة '\r':
		*q++ = '\\'; *q++ = 'r'; اكسر;
	    افتراضي:
		*q++ = *p;
		اكسر;
	}
    }
    *q = '\0';

    أرجع buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    ساكن حرف *
nb_unquote(char_u *p, char_u **endp)
{
    حرف *پایله = 0;
    حرف *q;
    صحيح بشپړ = 0;

    // result is never longer than input
    پایله = alloc_clear(STRLEN(p) + 1);
    إذا (پایله == عدم)
	أرجع عدم;

    إذا (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	پایله[0] = NUL;
	أرجع پایله;
    }

    لكل (q = پایله; !بشپړ && *p != NUL;)
    {
	بدل (*p)
	{
	    حالة '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		بشپړ = 1;
		اكسر;

	    حالة '\\':
		++p;
		بدل (*p)
		{
		    حالة '\\':	*q++ = '\\';	اكسر;
		    حالة 'n':	*q++ = '\n';	اكسر;
		    حالة 't':	*q++ = '\t';	اكسر;
		    حالة 'r':	*q++ = '\r';	اكسر;
		    حالة '"':	*q++ = '"';	اكسر;
		    حالة NUL:	--p;		اكسر;
		    // default: skip over illegal chars
		}
		++p;
		اكسر;

	    افتراضي:
		*q++ = *p++;
	}
    }

    إذا (endp != عدم)
	*endp = p;

    أرجع پایله;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    ساكن فراغ
nb_partialremove(linenr_T lnum, colnr_T لومړی, colnr_T وروستی)
{
    char_u *oldtext, *newtext;
    صحيح oldlen;
    صحيح lastbyte = وروستی;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    إذا (لومړی >= (colnr_T)oldlen || oldlen == 0)  // just in case
	أرجع;
    إذا (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - لومړی));
    إذا (newtext == عدم)
	أرجع;

    mch_memmove(newtext, oldtext, لومړی);
    STRMOVE(newtext + لومړی, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    ساكن فراغ
nb_joinlines(linenr_T لومړی, linenr_T other)
{
    صحيح len_لومړی, len_other;
    char_u *p;

    len_لومړی = ml_get_len(لومړی);
    len_other = ml_get_len(other);
    p = alloc(len_لومړی + len_other + 1);
    إذا (p == عدم)
	أرجع;

    mch_memmove(p, ml_get(لومړی), len_لومړی);
    mch_memmove(p + len_لومړی, ml_get(other), len_other + 1);
    ml_replace(لومړی, p, FALSE);
}

#تعریف_کړه SKIP_STOP 2
#تعریف_کړه streq(a,b) (strcmp(a,b) == 0)

/*
 * Do the actual processing of a single netbeans command or function.
 * The difference between a command and function is that a function
 * gets a response (it's required) but a command does not.
 * For arguments see comment for nb_parse_cmd().
 */
    ساكن صحيح
nb_do_cmd(
    صحيح		bufno,
    char_u	*cmd,
    صحيح		func,
    صحيح		cmdno,
    char_u	*دلیلونه)	    // points to space before arguments or NUL
{
    صحيح		do_update = 0;
    اوږد	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    ساكن صحيح	skip = 0;
    صحيح		retval = OK;
    حرف	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (حرف *)دلیلونه));

    إذا (func)
    {
// =====================================================================
	إذا (streq((حرف *)cmd, "getModified"))
	{
	    إذا (buf == عدم || buf->bufp == عدم)
		// Return the number of buffers that are modified.
		nb_ځواب_nr(cmdno, (اوږد)شمېر_بدل_شوی_buffers());
	    وإلا
		// Return whether the buffer is modified.
		nb_ځواب_nr(cmdno, (اوږد)(buf->bufp->b_بدل_شوی
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_ځواب_nr(cmdno, (اوږد)شمېر_بدل_شوی_buffers());
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "getCursor"))
	{
	    char_u متن[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((حرف *)متن, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (اوږد)curwin->w_cursor.lnum,
		    (صحيح)curwin->w_cursor.ستون,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_ځواب_متن(cmdno, متن);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "getAnno"))
	{
	    اوږد linenum = 0;
#که_تعریف_شوی FEAT_SIGNS
	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_getanno));
		retval = FAIL;
	    }
	    وإلا
	    {
		صحيح ser_عدد;

		cp = (حرف *)دلیلونه;
		ser_عدد = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (اوږد)buf_findsign(buf->bufp, ser_عدد, عدم);
	    }
#د_که_پای
	    nb_ځواب_nr(cmdno, linenum);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "getLength"))
	{
	    اوږد len = 0;

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_getlength));
		retval = FAIL;
	    }
	    وإلا
	    {
		len = get_buf_اندازه(buf->bufp);
	    }
	    nb_ځواب_nr(cmdno, len);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "getText"))
	{
	    اوږد	len;
	    linenr_T	nlines;
	    char_u	*متن = عدم;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*کرښه;

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_gettext));
		retval = FAIL;
	    }
	    وإلا
	    {
		len = get_buf_اندازه(buf->bufp);
		nlines = buf->bufp->b_ml.ml_کرښه_شمېر;
		متن = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		إذا (متن == عدم)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		وإلا
		{
		    p = متن;
		    *p++ = '\"';
		    لكل (; lno <= nlines ; lno++)
		    {
			کرښه = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			إذا (کرښه != عدم)
			{
			    STRCPY(p, کرښه);
			    p += STRLEN(کرښه);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(کرښه);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    إذا (متن == عدم)
		nb_ځواب_متن(cmdno, (char_u *)"");
	    وإلا
	    {
		nb_ځواب_متن(cmdno, متن);
		vim_free(متن);
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "remove"))
	{
	    اوږد شمېر;
	    pos_T لومړی, وروستی;
	    pos_T *pos;
	    pos_T *بل;
	    linenr_T del_from_lnum, del_منزل_lnum;  // lines to be deleted as a whole
	    صحيح oldFire = netbeansFireChanges;
	    صحيح oldSuppress = netbeans_Suppress_No_کرښې;
	    صحيح was_بدل_شوی_2;

	    إذا (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (حرف *) cmd));
		nb_ځواب_nil(cmdno);
		أرجع OK;
	    }

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_remove));
		retval = FAIL;
	    }
	    وإلا
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_کرښې = TRUE;

		nb_سېټ_curbuf(buf->bufp);
		was_بدل_شوی_2 = buf->bufp->b_بدل_شوی;
		cp = (حرف *)دلیلونه;
		off = strtol(cp, &cp, 10);
		شمېر = strtol(cp, &cp, 10);
		دلیلونه = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		إذا (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_ځواب_متن(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_کرښې = oldSuppress;
		    أرجع FAIL;
		}
		لومړی = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      لومړی.lnum, لومړی.ستون));
		pos = off2pos(buf->bufp, off+شمېر-1);
		إذا (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_ځواب_متن(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_کرښې = oldSuppress;
		    أرجع FAIL;
		}
		وروستی = *pos;
		nbdebug(("    LAST POS: line %ld, col %d\n",
							وروستی.lnum, وروستی.ستون));
		del_from_lnum = لومړی.lnum;
		del_منزل_lnum = وروستی.lnum;
		do_update = 1;

		// Get the position of the first byte after the deleted
		// section.  "next" is NULL when deleting to the end of the
		// file.
		بل = off2pos(buf->bufp, off + شمېر);

		// Remove part of the first line.
		إذا (لومړی.ستون != 0
				|| (بل != عدم && لومړی.lnum == بل->lnum))
		{
		    إذا (لومړی.lnum != وروستی.lnum
			    || (بل != عدم && لومړی.lnum != بل->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(لومړی.lnum, لومړی.ستون,
							     (colnr_T)MAXCOL);
			إذا (لومړی.lnum == وروستی.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(لومړی.lnum, بل->lnum);
			    del_منزل_lnum = بل->lnum;
			}
		    }
		    وإلا
		    {
			// remove within one line
			nb_partialremove(لومړی.lnum, لومړی.ستون, وروستی.ستون);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		إذا (لومړی.lnum != وروستی.lnum && بل != عدم
			&& بل->ستون != 0 && وروستی.lnum == بل->lnum)
		{
		    nb_partialremove(وروستی.lnum, 0, وروستی.ستون);
		    إذا (del_from_lnum > لومړی.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(لومړی.lnum, وروستی.lnum);
		    }
		    وإلا
			// First line is deleted as a whole, keep the last
			// line.
			--del_منزل_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		إذا (لومړی.lnum != وروستی.lnum && del_from_lnum > لومړی.lnum
			&& بل != عدم && وروستی.lnum != بل->lnum)
		{
		    nb_joinlines(لومړی.lnum, بل->lnum);
		    del_منزل_lnum = بل->lnum;
		}

		// Delete whole lines if there are any.
		إذا (del_منزل_lnum >= del_from_lnum)
		{
		    صحيح i;

		    // delete signs from the lines being deleted
		    لكل (i = del_from_lnum; i <= del_منزل_lnum; i++)
		    {
			صحيح id = buf_findsign_id(buf->bufp, (linenr_T)i, عدم);
			إذا (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, عدم);
			}
			وإلا
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_منزل_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.ستون = 0;
		    del_کرښې(del_منزل_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = لومړی;
		check_cursor_lnum();
		buf->bufp->b_بدل_شوی = was_بدل_شوی_2; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_کرښې = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_ځواب_nil(cmdno);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "insert"))
	{
	    char_u	*منزل_free;

	    إذا (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (حرف *) cmd));
		nb_ځواب_nil(cmdno);
		أرجع OK;
	    }

	    // get offset
	    cp = (حرف *)دلیلونه;
	    off = strtol(cp, &cp, 10);
	    دلیلونه = (char_u *)cp;

	    // get text to be inserted
	    دلیلونه = skipwhite(دلیلونه);
	    دلیلونه = منزل_free = (char_u *)nb_unquote(دلیلونه, عدم);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_ورننباسه));
		retval = FAIL;
	    }
	    وإلا إذا (دلیلونه != عدم)
	    {
		صحيح	ff_detected = EOL_UNKNOWN;
		صحيح	buf_was_تشه = (buf->bufp->b_ml.ml_flags & ML_تشه);
		size_t	len = 0;
		صحيح	added = 0;
		صحيح	oldFire = netbeansFireChanges;
		صحيح	old_b_بدل_شوی;
		char_u	*nlp;
		linenr_T lnum;
		linenr_T lnum_پیل;
		pos_T	*pos;

		netbeansFireChanges = 0;

		// Jump to the buffer where we insert.  After this "curbuf"
		// can be used.
		nb_سېټ_curbuf(buf->bufp);
		old_b_بدل_شوی = curbuf->b_بدل_شوی;

		// Convert the specified character offset into a lnum/col
		// position.
		pos = off2pos(curbuf, off);
		إذا (pos != عدم)
		{
		    إذا (pos->lnum <= 0)
			lnum_پیل = 1;
		    وإلا
			lnum_پیل = pos->lnum;
		}
		وإلا
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    إذا (buf_was_تشه)
			lnum_پیل = 1;	    // above empty line
		    وإلا
			lnum_پیل = curbuf->b_ml.ml_کرښه_شمېر + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_پیل;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		طالما (*دلیلونه != NUL)
		{
		    nlp = vim_strchr(دلیلونه, '\n');
		    إذا (nlp == عدم)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(دلیلونه);
		    }
		    وإلا
		    {
			len = nlp - دلیلونه;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			إذا (nlp > دلیلونه && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			وإلا
			    ff_detected = EOL_UNIX;
		    }
		    دلیلونه[len] = NUL;

		    إذا (lnum == lnum_پیل
			    && ((pos != عدم && pos->ستون > 0)
				|| (lnum == 1 && buf_was_تشه)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			صحيح	ستون = pos == عدم ? 0 : pos->ستون;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			إذا (newline != عدم)
			{
			    mch_memmove(newline, oldline, (size_t)ستون);
			    newline[ستون] = NUL;
			    STRCAT(newline, دلیلونه);
			    STRCAT(newline, oldline + ستون);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    وإلا
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), دلیلونه,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    إذا (nlp == عدم)
			اكسر;
		    ++lnum;
		    دلیلونه = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_کرښې_mark(lnum_پیل - 1, (اوږد)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		إذا (buf_was_تشه)
		{
		    إذا (ff_detected == EOL_UNKNOWN)
#إذا defined(MSWIN)
			ff_detected = EOL_DOS;
#وإلا
			ff_detected = EOL_UNIX;
#د_که_پای
		    سېټ_fileformat(ff_detected, OPT_LOCAL);
		    curbuf->b_پیل_ffc = *curbuf->b_p_ff;
		}

		/*
		 * XXX - GRP - Is the next line right? If I've inserted
		 * text the buffer has been updated but not written. Will
		 * netbeans guarantee to write it? Even if I do a :q! ?
		 */
		curbuf->b_بدل_شوی = old_b_بدل_شوی; // logically unchanged
		netbeansFireChanges = oldFire;

		// Undo info is invalid now...
		u_clearallandblockfree(curbuf);
	    }
	    vim_free(منزل_free);
	    nb_ځواب_nil(cmdno); // or !error
	}
	وإلا
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_ځواب_nil(cmdno);
	    retval = FAIL;
	}
    }
    وإلا // Not a function; no reply required.
    {
// =====================================================================
	إذا (streq((حرف *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_create));
		أرجع FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, عدم, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->ورننباسه_بشپړ = FALSE;
#إذا defined(FEAT_MENU) && defined(FEAT_GUI)
	    إذا (gui.دننه_use)
		gui_update_menus(0);
#د_که_پای
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "insertDone"))
	{
	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    وإلا
	    {
		buf->bufp->b_پیل_eol = *دلیلونه == 'T';
		buf->ورننباسه_بشپړ = TRUE;
		دلیلونه += 2;
		buf->bufp->b_p_ro = *دلیلونه == 'T';
		print_read_پیغام(buf);
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "saveDone"))
	{
	    اوږد savedChars = atol((حرف *)دلیلونه);

	    إذا (buf == عدم || buf->bufp == عدم)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    وإلا
		print_save_پیغام(buf, savedChars);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "startDocumentListen"))
	{
	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_startdocumentlisten));
		أرجع FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "stopDocumentListen"))
	{
	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_stopdocumentlisten));
		أرجع FAIL;
	    }
	    buf->fireChanges = 0;
	    إذا (buf->bufp != عدم && buf->bufp->b_was_netbeans_file)
	    {
		إذا (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_ورک_for_بفر_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_ورک_for_بفر_nr),
							   buf->bufp->b_fnum);
		}
		وإلا
		{
		    // NetBeans uses stopDocumentListen when it stops editing
		    // a file.  It then expects the buffer in Vim to
		    // disappear.
		    do_bufdel(DOBUF_DEL, (char_u *)"", 1,
				  buf->bufp->b_fnum, buf->bufp->b_fnum, TRUE);
		    CLEAR_اشاره_ګر(buf);
		}
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setTitle"))
	{
	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_settitle));
		أرجع FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(دلیلونه, عدم);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "initDone"))
	{
	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_initdone));
		أرجع FAIL;
	    }
	    do_update = 1;
	    buf->init_بشپړ = TRUE;
	    nb_سېټ_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_کلی_کتار();
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setBufferNumber")
		|| streq((حرف *)cmd, "putBufferNumber"))
	{
	    char_u	*لار;
	    buf_T	*bufp;

	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_setbuffernumber));
		أرجع FAIL;
	    }
	    لار = (char_u *)nb_unquote(دلیلونه, عدم);
	    إذا (لار == عدم)
		أرجع FAIL;
	    bufp = buflist_findname(لار);
	    vim_free(لار);
	    إذا (bufp == عدم)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", دلیلونه));
		semsg(_(e_file_str_not_وموندل_دننه_setbuffernumber), دلیلونه);
		أرجع FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_شمېره = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    إذا (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    وإلا
	    {
		buf->init_بشپړ = TRUE;

		// handle any postponed key commands
		handle_کلی_کتار();
	    }

// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setFullName"))
	{
	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_setfullname));
		أرجع FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(دلیلونه, عدم);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#إذا defined(FEAT_MENU) && defined(FEAT_GUI)
	    إذا (gui.دننه_use)
		gui_update_menus(0);
#د_که_پای
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "editFile"))
	{
	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_editfile));
		أرجع FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(دلیلونه, عدم);
	    do_ecmd(0, (char_u *)buf->displayname, عدم, عدم, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->init_بشپړ = TRUE;
	    do_update = 1;
	    maketitle();
#إذا defined(FEAT_MENU) && defined(FEAT_GUI)
	    إذا (gui.دننه_use)
		gui_update_menus(0);
#د_که_پای
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setVisible"))
	{
	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		إذا (p_verbose > 0)
		    emsg(_(e_invalid_بفر_identifier_دننه_setvisible));
		أرجع FAIL;
	    }
	    إذا (streq((حرف *)دلیلونه, "T") && buf->bufp != curbuf)
	    {
		exarg_T exarg;
		CLEAR_ډګر(exarg);
		exarg.cmd = (char_u *)"goto";
		exarg.forceit = FALSE;
		exarg.cmdidx = CMD_USER;
		dosetvisible = TRUE;
		goto_بفر(&exarg, DOBUF_لومړی, FORWARD, buf->bufp->b_fnum);
		do_update = 1;
		dosetvisible = FALSE;

#که_تعریف_شوی FEAT_GUI
		// Side effect!!!.
		إذا (gui.دننه_use)
		    gui_mch_سېټ_foreground();
#د_که_پای
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "raise"))
	{
#که_تعریف_شوی FEAT_GUI
	    // Bring gvim to the foreground.
	    إذا (gui.دننه_use)
		gui_mch_سېټ_foreground();
#د_که_پای
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setModified"))
	{
	    صحيح prev_b_بدل_شوی;

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		إذا (p_verbose > 0)
		    emsg(_(e_invalid_بفر_identifier_دننه_setmodified));
		أرجع FAIL;
	    }
	    prev_b_بدل_شوی = buf->bufp->b_بدل_شوی;
	    إذا (streq((حرف *)دلیلونه, "T"))
		buf->bufp->b_بدل_شوی = TRUE;
	    وإلا
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		إذا (buf->bufp->b_ffname != عدم
			&& mch_stat((حرف *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_وخت(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_بدل_شوی = FALSE;
	    }
	    buf->modified = buf->bufp->b_بدل_شوی;
	    إذا (prev_b_بدل_شوی != buf->bufp->b_بدل_شوی)
	    {
		check_حالت(buf->bufp);
		redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#د_که_پای
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setModtime"))
	{
	    إذا (buf == عدم || buf->bufp == عدم)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    وإلا
	    {
		buf->bufp->b_mtime = atoi((حرف *)دلیلونه);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setReadOnly"))
	{
	    إذا (buf == عدم || buf->bufp == عدم)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    وإلا إذا (streq((حرف *)دلیلونه, "T"))
		buf->bufp->b_p_ro = TRUE;
	    وإلا
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "showBalloon"))
	{
#إذا defined(FEAT_BEVAL_GUI)
	    ساكن حرف	*متن = عدم;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    إذا (balloonEval != عدم)
	    {
		vim_free(متن);
		متن = nb_unquote(دلیلونه, عدم);
		إذا (متن != عدم)
		    gui_mch_post_balloon(balloonEval, (char_u *)متن);
	    }
#د_که_پای
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setDot"))
	{
	    pos_T *pos;
#که_تعریف_شوی NBDEBUG
	    char_u *s;
#د_که_پای

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_setdot));
		أرجع FAIL;
	    }

	    nb_سېټ_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    إذا (VIsual_فعال)
		پای_ټکی_visual_ډېرپېښ();
#که_تعریف_شوی NBDEBUG
	    s = دلیلونه;
#د_که_پای
	    pos = get_off_or_lnum(buf->bufp, &دلیلونه);
	    إذا (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#که_تعریف_شوی FEAT_FOLDING
		foldOpenCursor();
#د_که_پای
	    }
	    وإلا
	    {
		nbdebug(("    BAD POSITION in setDot: %s\n", s));
	    }

	    // gui_update_cursor(TRUE, FALSE);
	    // update_curbuf(UPD_NOT_VALID);
	    update_topline();		// scroll to show the line
	    update_screen(UPD_VALID);
	    setcursor();
	    cursor_on();
	    بهر_flush_cursor(TRUE, FALSE);

	    // Quit a hit-return or more prompt.
	    إذا (State == ډېرپېښ_HITRETURN || State == ډېرپېښ_ASKMORE)
	    {
#که_تعریف_شوی FEAT_GUI_GTK
		إذا (gui.دننه_use && gtk_main_level() > 0)
		    gtk_main_quit();
#د_که_پای
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "close"))
	{
#که_تعریف_شوی NBDEBUG
	    حرف *name = "<NONE>";
#د_که_پای

	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_close));
		أرجع FAIL;
	    }

#که_تعریف_شوی NBDEBUG
	    إذا (buf->displayname != عدم)
		name = buf->displayname;
#د_که_پای
	    إذا (buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		إذا (p_verbose > 0)
		    emsg(_(e_invalid_بفر_identifier_دننه_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#که_تعریف_شوی FEAT_GUI
	    need_mouse_correct = TRUE;
#د_که_پای
	    إذا (buf->bufp != عدم)
		do_بفر(DOBUF_WIPE, DOBUF_لومړی, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = عدم;
	    buf->init_بشپړ = FALSE;
	    do_update = 1;
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "defineAnnoType"))
	{
#که_تعریف_شوی FEAT_SIGNS
	    صحيح type_عدد;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    صحيح parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_defineannotype));
		أرجع FAIL;
	    }

	    cp = (حرف *)دلیلونه;
	    type_عدد = strtol(cp, &cp, 10);
	    دلیلونه = (char_u *)cp;
	    دلیلونه = skipwhite(دلیلونه);
	    typeName = (char_u *)nb_unquote(دلیلونه, &دلیلونه);
	    دلیلونه = skipwhite(دلیلونه + 1);
	    tooltip = (char_u *)nb_unquote(دلیلونه, &دلیلونه);
	    دلیلونه = skipwhite(دلیلونه + 1);

	    p = (char_u *)nb_unquote(دلیلونه, &دلیلونه);
	    glyphFile = vim_strsave_escaped(p, escape_chars);
	    vim_free(p);

	    دلیلونه = skipwhite(دلیلونه + 1);
	    p = skiptowhite(دلیلونه);
	    إذا (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(دلیلونه);
	    bg = vim_strsave(p);
	    إذا (STRLEN(fg) > MAX_COLOR_اوږدتیا || STRLEN(bg) > MAX_COLOR_اوږدتیا)
	    {
		emsg(_(e_highlighting_color_name_too_long_دننه_define_Anno_Type));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    وإلا إذا (!nb_is_خوندي_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_خوندي_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_خوندي_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_دننه_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    وإلا إذا (typeName != عدم && tooltip != عدم && glyphFile != عدم)
		addsigntype(buf, type_عدد, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    إذا (parse_error)
		أرجع FAIL;

#د_که_پای
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "addAnno"))
	{
#که_تعریف_شوی FEAT_SIGNS
	    صحيح ser_عدد;
	    صحيح local_Type_عدد;
	    صحيح type_عدد;
	    pos_T *pos;

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_بفر_identifier_دننه_addanno));
		أرجع FAIL;
	    }

	    do_update = 1;

	    cp = (حرف *)دلیلونه;
	    ser_عدد = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_عدد = strtol(cp, &cp, 10);
	    دلیلونه = (char_u *)cp;
	    type_عدد = mapsigntype(buf, local_Type_عدد);

	    pos = get_off_or_lnum(buf->bufp, &دلیلونه);

	    cp = (حرف *)دلیلونه;
	    vim_ignored = (صحيح)strtol(cp, &cp, 10);
	    دلیلونه = (char_u *)cp;
# ifdef NBDEBUG
	    إذا (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    إذا (ser_عدد >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		أرجع FAIL;
	    }
	    إذا (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_عدد, pos->lnum, type_عدد, buf->bufp->b_fnum);
		إذا (type_عدد == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_عدد,
						       buf->bufp->b_fnum);
	    }
#د_که_پای
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "removeAnno"))
	{
#که_تعریف_شوی FEAT_SIGNS
	    صحيح ser_عدد;

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		أرجع FAIL;
	    }
	    do_update = 1;
	    cp = (حرف *)دلیلونه;
	    ser_عدد = strtol(cp, &cp, 10);
	    دلیلونه = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_عدد, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#د_که_پای
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "moveAnnoToFront"))
	{
#که_تعریف_شوی FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#د_که_پای
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "guard") || streq((حرف *)cmd, "unguard"))
	{
	    صحيح len;
	    pos_T لومړی;
	    pos_T وروستی;
	    pos_T *pos;
	    صحيح un = (cmd[0] == 'u');
	    ساكن صحيح guardId = GUARDEDOFFSET;

	    إذا (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (حرف *) cmd));
		أرجع OK;
	    }

	    nb_init_graphics();

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		أرجع FAIL;
	    }
	    nb_سېټ_curbuf(buf->bufp);
	    cp = (حرف *)دلیلونه;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, عدم, 10);
	    دلیلونه = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    إذا (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    وإلا
	    {
		لومړی = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		إذا (pos != عدم && pos->ستون == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		إذا (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		وإلا
		{
		    اوږد lnum;
		    وروستی = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     لومړی.lnum, لومړی.ستون,
			     وروستی.lnum, وروستی.ستون));
#که_تعریف_شوی FEAT_SIGNS
		    لكل (lnum = لومړی.lnum; lnum <= وروستی.lnum; lnum++)
		    {
			إذا (un)
			{
			    // never used
			}
			وإلا
			{
			    إذا (buf_findsigntype_id(buf->bufp, lnum,
				GUARDED) == 0)
			    {
				coloncmd(
				    ":sign place %d line=%ld name=%d buffer=%d",
				     guardId++, lnum, GUARDED,
				     buf->bufp->b_fnum);
			    }
			}
		    }
#د_که_پای
		    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
		}
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "startAtomic"))
	{
	    دننه_Atomic = 1;
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "endAtomic"))
	{
	    دننه_Atomic = 0;
	    إذا (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		أرجع FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    إذا (buf_Is_بدل_شوی(buf->bufp))
	    {
		// Only write if the buffer can be written.
		إذا (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != عدم
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    سېټ_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    إذا (!bufref_valid(&bufref))
			buf->bufp = عدم;
		}
	    }
	    وإلا
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "netbeansBuffer"))
	{
	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		أرجع FAIL;
	    }
	    إذا (*دلیلونه == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    وإلا
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "specialKeys"))
	{
	    special_کلیګانې(دلیلونه);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "version"))
	{
	    // not used yet
	}
	وإلا
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    إذا (دننه_Atomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    إذا (buf != عدم && buf->init_بشپړ && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	بهر_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	إذا (State == ډېرپېښ_HITRETURN || State == ډېرپېښ_ASKMORE)
	{
#که_تعریف_شوی FEAT_GUI_GTK
	    إذا (gui.دننه_use && gtk_main_level() > 0)
		gtk_main_quit();
#د_که_پای
	}
    }

    أرجع retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    ساكن فراغ
nb_سېټ_curbuf(buf_T *buf)
{
    إذا (curbuf == buf)
	أرجع;

    إذا (buf_jump_open_win(buf) != عدم)
	أرجع;
    إذا ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != عدم)
	أرجع;
    سېټ_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    ساكن فراغ
coloncmd(حرف *cmd, ...)
{
    حرف buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, حجم(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, عدم, عدم, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    بهر_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    ساكن فراغ
special_کلیګانې(char_u *دلیلونه)
{
    حرف *save_str = nb_unquote(دلیلونه, عدم);
    حرف *tok = strtok(save_str, " ");
    حرف *sep;
#تعریف_کړه KEYBUFLEN 64
    حرف keybuf[KEYBUFLEN];
    حرف cmdbuf[256];

    طالما (tok != عدم)
    {
	صحيح i = 0;

	إذا ((sep = strchr(tok, '-')) != عدم)
	{
	    *sep = NUL;
	    طالما (*tok && i + 2 < KEYBUFLEN)
	    {
		بدل (*tok)
		{
		    حالة 'A':
		    حالة 'M':
		    حالة 'C':
		    حالة 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			اكسر;
		}
		tok++;
	    }
	    tok++;
	}

	إذا (مزي_اوږدوالی(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    صحيح خوندي = TRUE;
	    لكل (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		إذا (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    خوندي = FALSE;
		    اكسر;
		}
	    }
	    إذا (خوندي)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, حجم(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, ډېرپېښ_نورمال, FALSE);
	    }
	}
	tok = strtok(عدم, " ");
    }
    vim_free(save_str);
}

    فراغ
ex_nbclose(exarg_T *eap ناکارول_شوی)
{
    netbeans_close();
}

    فراغ
ex_nbkey(exarg_T *eap)
{
    (فراغ)netbeans_keystring(eap->arg);
}

    فراغ
ex_nbstart(
    exarg_T	*eap)
{
#که_تعریف_شوی FEAT_GUI
# إذا !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    إذا (gui.دننه_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	أرجع;
    }
# endif
#د_که_پای
    netbeans_open((حرف *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    ساكن فراغ
nb_init_graphics(فراغ)
{
    ساكن صحيح did_init = FALSE;

    إذا (did_init)
	أرجع;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    ساكن فراغ
netbeans_keyname(صحيح کلی, حرف *buf)
{
    حرف *name = 0;
    حرف namebuf[2];
    صحيح ctrl  = 0;
    صحيح ښويدنه = 0;
    صحيح alt   = 0;

    إذا (mod_ماسک & MOD_ماسک_CTRL)
	ctrl = 1;
    إذا (mod_ماسک & MOD_ماسک_ښويدنه)
	ښويدنه = 1;
    إذا (mod_ماسک & MOD_ماسک_ALT)
	alt = 1;


    بدل (کلی)
    {
	حالة K_F1:		name = "F1";		اكسر;
	حالة K_S_F1:	name = "F1";	ښويدنه = 1;	اكسر;
	حالة K_F2:		name = "F2";		اكسر;
	حالة K_S_F2:	name = "F2";	ښويدنه = 1;	اكسر;
	حالة K_F3:		name = "F3";		اكسر;
	حالة K_S_F3:	name = "F3";	ښويدنه = 1;	اكسر;
	حالة K_F4:		name = "F4";		اكسر;
	حالة K_S_F4:	name = "F4";	ښويدنه = 1;	اكسر;
	حالة K_F5:		name = "F5";		اكسر;
	حالة K_S_F5:	name = "F5";	ښويدنه = 1;	اكسر;
	حالة K_F6:		name = "F6";		اكسر;
	حالة K_S_F6:	name = "F6";	ښويدنه = 1;	اكسر;
	حالة K_F7:		name = "F7";		اكسر;
	حالة K_S_F7:	name = "F7";	ښويدنه = 1;	اكسر;
	حالة K_F8:		name = "F8";		اكسر;
	حالة K_S_F8:	name = "F8";	ښويدنه = 1;	اكسر;
	حالة K_F9:		name = "F9";		اكسر;
	حالة K_S_F9:	name = "F9";	ښويدنه = 1;	اكسر;
	حالة K_F10:		name = "F10";		اكسر;
	حالة K_S_F10:	name = "F10";	ښويدنه = 1;	اكسر;
	حالة K_F11:		name = "F11";		اكسر;
	حالة K_S_F11:	name = "F11";	ښويدنه = 1;	اكسر;
	حالة K_F12:		name = "F12";		اكسر;
	حالة K_S_F12:	name = "F12";	ښويدنه = 1;	اكسر;
	افتراضي:
			إذا (کلی >= ' ' && کلی <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = کلی;
			    namebuf[1] = NUL;
			}
			وإلا
			    name = "X";
			اكسر;
    }

    buf[0] = '\0';
    إذا (ctrl)
	strcat(buf, "C");
    إذا (ښويدنه)
	strcat(buf, "S");
    إذا (alt)
	strcat(buf, "M"); // META
    إذا (ctrl || ښويدنه || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#إذا defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    فراغ
netbeans_beval_cb(
	BalloonEval	*beval,
	صحيح		 state ناکارول_شوی)
{
    win_T	*wp;
    char_u	*متن;
    linenr_T	lnum;
    صحيح		ستون;
    حرف	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    إذا (!can_use_beval() || !NETBEANS_OPEN)
	أرجع;

    إذا (get_beval_info(beval, TRUE, &wp, &lnum, &متن, &ستون) != OK)
	أرجع;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    إذا (متن != عدم && متن[0] != NUL && STRLEN(متن) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	إذا (buf != عدم)
	{
	    p = nb_quote(متن);
	    إذا (p != عدم)
	    {
		vim_snprintf(buf, MAXPATHL * 2 + 25,
			"0:balloonText=%d \"%s\"\n", r_cmdno, p);
		vim_free(p);
	    }
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_beval_cb");
	    vim_free(buf);
	}
    }
    vim_free(متن);
}
#د_که_پای

/*
 * Return TRUE when the netbeans connection is active.
 */
    صحيح
netbeans_فعال(فراغ)
{
    أرجع NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    فراغ
netbeans_open(حرف *params, صحيح doabort)
{
    حرف *cmd = "0:startupDone=0\n";

    إذا (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	أرجع;
    }

    إذا (netbeans_connect(params, doabort) != OK)
	أرجع;

    nbdebug(("EVT: %s", cmd));
    nb_send(cmd, "netbeans_startup_done");

    // update the screen after having added the gutter
    بدل_شوی_کړکۍ_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    بهر_flush_cursor(TRUE, FALSE);
}

/*
 * Tell netbeans that we're exiting. This should be called right
 * before calling exit.
 */
    فراغ
netbeans_send_disconnect(فراغ)
{
    حرف buf[128];

    إذا (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#إذا defined(FEAT_EVAL)
    صحيح
سېټ_ref_دننه_nb_channel(صحيح copyID)
{
    صحيح abort = FALSE;
    typval_T tv;

    إذا (nb_channel == عدم)
	أرجع FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = سېټ_ref_دننه_توکی(&tv, copyID, عدم, عدم, عدم);
    أرجع abort;
}
#د_که_پای

#إذا defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    فراغ
netbeans_چوکاټ_moved(صحيح new_x, صحيح new_y)
{
    حرف buf[128];

    إذا (!NETBEANS_OPEN)
	أرجع;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (صحيح)Columns, (صحيح)لیکې, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#د_که_پای

/*
 * Tell netbeans the user opened or activated a file.
 */
    فراغ
netbeans_file_activated(buf_T *bufp)
{
    صحيح bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    حرف    بفر[2*MAXPATHL];
    char_u  *q;

    إذا (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	أرجع;

    q = nb_quote(bufp->b_ffname);
    إذا (q == عدم)
	أرجع;
    إذا (bp == عدم)
    {
	vim_free(q);
	أرجع;
    }

    vim_snprintf(بفر, حجم(بفر),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (حرف *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", بفر));

    nb_send(بفر, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    فراغ
netbeans_file_opened(buf_T *bufp)
{
    صحيح bufno = nb_getbufno(bufp);
    حرف    بفر[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    صحيح	    bnum;

    إذا (!NETBEANS_OPEN)
	أرجع;

    q = nb_quote(bufp->b_ffname);
    إذا (q == عدم)
	أرجع;
    إذا (bp != عدم)
	bnum = bufno;
    وإلا
	bnum = 0;

    vim_snprintf(بفر, حجم(بفر), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (حرف *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", بفر));

    nb_send(بفر, "netbeans_file_opened");
    إذا (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	وروستی_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    فراغ
netbeans_file_killed(buf_T *bufp)
{
    صحيح		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    حرف	بفر[2*MAXPATHL];

    إذا (!NETBEANS_OPEN || bufno == -1)
	أرجع;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(بفر, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", بفر));

    nb_send(بفر, "netbeans_file_killed");

    إذا (nbbuf != عدم)
	nbbuf->bufp = عدم;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    ساكن nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, صحيح *bufnop)
{
    صحيح		bufno;
    nbbuf_T	*nbbuf;

    إذا (!NETBEANS_OPEN || !netbeansFireChanges)
	أرجع عدم;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    إذا (bufno <= 0)
	أرجع عدم;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    إذا (nbbuf != عدم && !nbbuf->fireChanges)
	أرجع عدم;		// changes in this buffer are not reported

    *bufnop = bufno;
    أرجع nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    فراغ
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	ستون,
    char_u	*txt,
    صحيح		newlen)
{
    char_u	*buf;
    صحيح		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    اوږد	off;
    char_u	*p;
    char_u	*newtxt;

    إذا (!NETBEANS_OPEN)
	أرجع;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    إذا (nbbuf == عدم)
	أرجع;

    // Don't mark as modified for initial read
    إذا (nbbuf->ورننباسه_بشپړ)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.ستون = ستون;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    إذا (p != عدم)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((حرف *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((حرف *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    فراغ
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	ستون,
    اوږد	len)
{
    char_u	buf[128];
    صحيح		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    اوږد	off;

    إذا (!NETBEANS_OPEN)
	أرجع;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    إذا (nbbuf == عدم)
	أرجع;

    إذا (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	أرجع;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.ستون = ستون;

    off = pos2off(bufp, &pos);

    sprintf((حرف *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((حرف *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    فراغ
netbeans_unmodified(buf_T *bufp ناکارول_شوی)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    فراغ
netbeans_button_release(صحيح button)
{
    حرف	buf[128];
    صحيح		bufno;

    إذا (!NETBEANS_OPEN)
	أرجع;

    bufno = nb_getbufno(curbuf);

    إذا (bufno < 0 || curwin == عدم || curwin->w_بفر != curbuf)
	أرجع;

    صحيح ستون = mouse_ستون - curwin->w_wincol
	- ((curwin->w_p_nu || curwin->w_p_rnu) ? 9 : 1);
    اوږد off = pos2off(curbuf, &curwin->w_cursor);

    // sync the cursor position
    sprintf(buf, "%d:newDotAndMark=%d %ld %ld\n", bufno, r_cmdno, off, off);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release[newDotAndMark]");

    sprintf(buf, "%d:buttonRelease=%d %d %ld %d\n", bufno, r_cmdno,
	    button, (اوږد)curwin->w_cursor.lnum, ستون);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release");
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key code.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    صحيح
netbeans_keycommand(صحيح کلی)
{
    حرف	کلی_Name[60];

    netbeans_keyname(کلی, کلی_Name);
    أرجع netbeans_keystring((char_u *)کلی_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    ساكن صحيح
netbeans_keystring(char_u *کلی_Name)
{
    حرف	buf[2*MAXPATHL];
    صحيح		bufno = nb_getbufno(curbuf);
    اوږد	off;
    char_u	*q;

    إذا (!NETBEANS_OPEN)
	أرجع TRUE;

    إذا (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == عدم ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	إذا (q == عدم)
	    أرجع TRUE;
	vim_snprintf(buf, حجم(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	إذا (curbuf->b_ffname != عدم)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(کلی_Name);
	أرجع FALSE;
    }

    // sync the cursor position
    off = pos2off(curbuf, &curwin->w_cursor);
    sprintf(buf, "%d:newDotAndMark=%d %ld %ld\n", bufno, r_cmdno, off, off);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // To work on Win32 you must apply patch to ExtEditor module
    // from ExtEdCaret.java.diff - make EVT_newDotAndMark handler
    // more synchronous

    // now send keyCommand event
    vim_snprintf(buf, حجم(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, کلی_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, حجم(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, کلی_Name,
		off, (اوږد)curwin->w_cursor.lnum, (اوږد)curwin->w_cursor.ستون);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    أرجع TRUE;
}


/*
 * Send a save event to netbeans.
 */
    فراغ
netbeans_save_بفر(buf_T *bufp)
{
    char_u	buf[64];
    صحيح		bufno;
    nbbuf_T	*nbbuf;

    إذا (!NETBEANS_OPEN)
	أرجع;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    إذا (nbbuf == عدم)
	أرجع;

    nbbuf->modified = 0;

    sprintf((حرف *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((حرف *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    فراغ
netbeans_deleted_all_کرښې(buf_T *bufp)
{
    char_u	buf[64];
    صحيح		bufno;
    nbbuf_T	*nbbuf;

    إذا (!NETBEANS_OPEN)
	أرجع;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    إذا (nbbuf == عدم)
	أرجع;

    // Don't mark as modified for initial read
    إذا (nbbuf->ورننباسه_بشپړ)
	nbbuf->modified = 1;

    sprintf((حرف *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    صحيح
netbeans_is_guarded(linenr_T سر_2, linenr_T bot)
{
    sign_ثبت_T	*p;
    صحيح			lnum;

    إذا (!NETBEANS_OPEN)
	أرجع FALSE;

    FOR_ALL_SIGNS_دننه_BUF(curbuf, p)
	إذا (p->se_id >= GUARDEDOFFSET)
	    لكل (lnum = سر_2 + 1; lnum < bot; lnum++)
		إذا (lnum == p->se_lnum)
		    أرجع TRUE;

    أرجع FALSE;
}

#إذا defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    فراغ
netbeans_draw_multisign_indicator(صحيح لیکه)
{
    صحيح i;
    صحيح y;
    صحيح x;

    إذا (!NETBEANS_OPEN)
	أرجع;

    x = 0;
    y = لیکه * gui.char_لوړوالی + 2;

    لكل (i = 0; i < gui.char_لوړوالی - 3; i++)
	XDraw_ټکی(gui.dpy, gui.wid, gui.متن_gc, x+2, y++);

    XDraw_ټکی(gui.dpy, gui.wid, gui.متن_gc, x+0, y);
    XDraw_ټکی(gui.dpy, gui.wid, gui.متن_gc, x+2, y);
    XDraw_ټکی(gui.dpy, gui.wid, gui.متن_gc, x+4, y++);
    XDraw_ټکی(gui.dpy, gui.wid, gui.متن_gc, x+1, y);
    XDraw_ټکی(gui.dpy, gui.wid, gui.متن_gc, x+2, y);
    XDraw_ټکی(gui.dpy, gui.wid, gui.متن_gc, x+3, y++);
    XDraw_ټکی(gui.dpy, gui.wid, gui.متن_gc, x+2, y);
}
#د_که_پای // FEAT_GUI_X11

#إذا defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    فراغ
netbeans_draw_multisign_indicator(صحيح لیکه)
{
    صحيح i;
    صحيح y;
    صحيح x;
# إذا GTK_CHECK_نسخه(3,0,0)
    cairo_t *cr = عدم;
# وإلا
    GdkDrawable *drawable = gui.drawarea->کړکۍ_2;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    إذا (!NETBEANS_OPEN)
	أرجع;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# وإلا
    y = لیکه * gui.char_لوړوالی + 2;
# endif

# إذا GTK_CHECK_نسخه(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_لوړوالی);
    cr = cairo_create(surf);
#  وإلا
    cr = cairo_create(gui.surface);
#  endif
    cairo_سېټ_سرچینه_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    لكل (i = 0; i < gui.char_لوړوالی - 3; i++)
# إذا GTK_CHECK_نسخه(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# وإلا
    gdk_draw_ټکی(drawable, gui.متن_gc, x+2, y++);
# endif

# إذا GTK_CHECK_نسخه(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# وإلا
    gdk_draw_ټکی(drawable, gui.متن_gc, x+0, y);
    gdk_draw_ټکی(drawable, gui.متن_gc, x+2, y);
    gdk_draw_ټکی(drawable, gui.متن_gc, x+4, y++);
    gdk_draw_ټکی(drawable, gui.متن_gc, x+1, y);
    gdk_draw_ټکی(drawable, gui.متن_gc, x+2, y);
    gdk_draw_ټکی(drawable, gui.متن_gc, x+3, y++);
    gdk_draw_ټکی(drawable, gui.متن_gc, x+2, y);
# endif

# ifdef USE_GTK4
    cairo_fill(cr);
    gui_gtk4_جمع_کړه_multisign(surf, لیکه, 0, 5, gui.char_لوړوالی);
# endif

# إذا GTK_CHECK_نسخه(3,0,0)
    cairo_destroy(cr);
# endif
# ifdef USE_GTK4
    cairo_surface_destroy(surf);
# endif
}
#د_که_پای // FEAT_GUI_GTK

/*
 * If the mouse is clicked in the gutter of a line with multiple
 * annotations, cycle through the set of signs.
 */
    فراغ
netbeans_gutter_click(linenr_T lnum)
{
    sign_ثبت_T	*p;

    إذا (!NETBEANS_OPEN)
	أرجع;

    FOR_ALL_SIGNS_دننه_BUF(curbuf, p)
    {
	إذا (p->se_lnum == lnum && p->se_بل && p->se_بل->se_lnum == lnum)
	{
	    sign_ثبت_T *لکۍ;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    إذا (p->se_prev)
		p->se_prev->se_بل = p->se_بل;
	    وإلا
		curbuf->b_signlist = p->se_بل;
	    p->se_بل->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    لكل (لکۍ = p->se_بل;
		  لکۍ->se_بل && لکۍ->se_بل->se_lnum == lnum
				       && لکۍ->se_بل->se_id < GUARDEDOFFSET;
		  لکۍ = لکۍ->se_بل)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_بل = لکۍ->se_بل;
	    إذا (لکۍ->se_بل)
		لکۍ->se_بل->se_prev = p;
	    p->se_prev = لکۍ;
	    لکۍ->se_بل = p;
	    update_debug_sign(curbuf, lnum);
	    اكسر;
	}
    }
}

/*
 * Add a sign of the requested type at the requested location.
 *
 * Reverse engineering:
 * Apparently an annotation is defined the first time it is used in a buffer.
 * When the same annotation is used in two buffers, the second time we do not
 * need to define a new sign name but reuse the existing one.  But since the
 * ID number used in the second buffer starts counting at one again, a mapping
 * is made from the ID specifically for the buffer to the global sign name
 * (which is a number).
 *
 * globalsignmap[]	stores the signs that have been defined globally.
 * buf->signmapused[]	maps buffer-local annotation IDs to an index in
 *			globalsignmap[].
 */
    ساكن فراغ
addsigntype(
    nbbuf_T	*buf,
    صحيح		type_عدد,
    char_u	*typeName,
    char_u	*tooltip ناکارول_شوی,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    صحيح i, j;
    صحيح use_fg = (*fg && STRCMP(fg, "none") != 0);
    صحيح use_bg = (*bg && STRCMP(bg, "none") != 0);

    لكل (i = 0; i < globalsignmapused; i++)
	إذا (STRCMP(typeName, globalsignmap[i]) == 0)
	    اكسر;

    إذا (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_عدد, typeName, tooltip, glyphFile, fg, bg));
	إذا (use_fg || use_bg)
	{
	    حرف fgbuf[2 * (8 + MAX_COLOR_اوږدتیا) + 1];
	    حرف bgbuf[2 * (8 + MAX_COLOR_اوږدتیا) + 1];
	    حرف *ptr;
	    صحيح ارزښت;

	    ارزښت = strtol((حرف *)fg, &ptr, 10);
	    إذا (ptr != (حرف *)fg)
		sprintf(fgbuf, "guifg=#%06x", ارزښت & 0xFFFFFF);
	    وإلا
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    ارزښت = strtol((حرف *)bg, &ptr, 10);
	    إذا (ptr != (حرف *)bg)
		sprintf(bgbuf, "guibg=#%06x", ارزښت & 0xFFFFFF);
	    وإلا
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    إذا (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    وإلا إذا (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    وإلا
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	وإلا
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	إذا (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_عدد;

	إذا (globalsignmapused == globalsignmaplen)
	{
	    إذا (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(حرف *, globalsignmaplen);
	    }
	    وإلا    // grow it
	    {
		صحيح incr;
		صحيح oldlen = globalsignmaplen;
		حرف **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * حجم(حرف *));
		إذا (globalsignmap == عدم)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    أرجع;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * حجم(حرف *));
	    }
	}

	globalsignmap[i] = (حرف *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    لكل (j = 0; j < buf->signmapused; j++)
	إذا (buf->signmap[j] == i + 1)
	    أرجع;

    // add to local map
    إذا (buf->signmapused == buf->signmaplen)
    {
	إذا (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(صحيح, buf->signmaplen);
	}
	وإلا    // grow it
	{
	    صحيح incr;
	    صحيح oldlen = buf->signmaplen;
	    صحيح *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * حجم(صحيح));
	    إذا (buf->signmap == عدم)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		أرجع;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * حجم(صحيح));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    ساكن صحيح
mapsigntype(nbbuf_T *buf, صحيح localsigntype)
{
    إذا (--localsigntype >= 0 && localsigntype < buf->signmapused)
	أرجع buf->signmap[localsigntype];

    أرجع 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    ساكن اوږد
get_buf_اندازه(buf_T *bufp)
{
    linenr_T	lnum;
    اوږد	char_شمېر = 0;
    صحيح		eol_اندازه;
    اوږد	وروستی_check = 100000L;

    إذا (bufp->b_ml.ml_flags & ML_تشه)
	أرجع 0;

    إذا (get_fileformat(bufp) == EOL_DOS)
	eol_اندازه = 2;
    وإلا
	eol_اندازه = 1;
    لكل (lnum = 1; lnum <= bufp->b_ml.ml_کرښه_شمېر; ++lnum)
    {
	char_شمېر += ml_get_buf_len(bufp, lnum) + eol_اندازه;
	// Check for a CTRL-C every 100000 characters
	إذا (char_شمېر > وروستی_check)
	{
	    ui_breakcheck();
	    إذا (got_int)
		أرجع char_شمېر;
	    وروستی_check = char_شمېر + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    إذا (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_شمېر -= eol_اندازه;

    أرجع char_شمېر;
}

/*
 * Convert character offset to lnum,col
 */
    ساكن pos_T *
off2pos(buf_T *buf, اوږد دننه_واټن)
{
    linenr_T	 lnum;
    ساكن pos_T pos;

    pos.lnum = 0;
    pos.ستون = 0;
    pos.coladd = 0;

    إذا (!(buf->b_ml.ml_flags & ML_تشه))
    {
	إذا ((lnum = ml_پیدا_کړه_کرښه_or_دننه_واټن(buf, (linenr_T)0, &دننه_واټن)) < 0)
	    أرجع عدم;
	pos.lnum = lnum;
	pos.ستون = دننه_واټن;
    }

    أرجع &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    ساكن pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    ساكن pos_T	mypos;
    اوږد		off;

    off = strtol((حرف *)*argp, (حرف **)argp, 10);
    إذا (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.ستون = strtol((حرف *)*argp, (حرف **)argp, 10);
	mypos.coladd = 0;
	أرجع &mypos;
    }
    أرجع off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    ساكن اوږد
pos2off(buf_T *buf, pos_T *pos)
{
    اوږد	 دننه_واټن = 0;

    إذا (buf->b_ml.ml_flags & ML_تشه)
	أرجع 0;

    إذا ((دننه_واټن = ml_پیدا_کړه_کرښه_or_دننه_واټن(buf, pos->lnum, 0)) < 0)
	أرجع 0;
    دننه_واټن += pos->ستون;

    أرجع دننه_واټن;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    ساكن فراغ
print_read_پیغام(nbbuf_T *buf)
{
    صحيح	    lnum = buf->bufp->b_ml.ml_کرښه_شمېر;
    off_T   nchars = buf->bufp->b_orig_اندازه;
    char_u  c;

    پیغام_جمع_کړه_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    إذا (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    إذا (!buf->bufp->b_پیل_eol)
    {
	STRCAT(IObuff, shortmess(SHM_وروستی) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    پیغام_جمع_کړه_کرښې(c, (اوږد)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_پیغام);
    پیغام_scrolled_ign = TRUE;
    پیغام_trunc_attr((حرف *)IObuff, FALSE, 0);
    پیغام_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    ساكن فراغ
print_save_پیغام(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    إذا (nchars >= 0)
    {
	// put fname in IObuff with quotes
	پیغام_جمع_کړه_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	پیغام_جمع_کړه_کرښې(c, buf->bufp->b_ml.ml_کرښه_شمېر,
						buf->bufp->b_orig_اندازه);

	VIM_CLEAR(keep_پیغام);
	پیغام_scrolled_ign = TRUE;
	p = (char_u *)پیغام_trunc_attr((حرف *)IObuff, FALSE, 0);
	إذا ((پیغام_scrolled && !need_wait_return) || !buf->init_بشپړ)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    سېټ_keep_پیغام(p, 0);
	}
	پیغام_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    وإلا
    {
	حرف msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_جمع_کړه_bang_منزل_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#د_که_پای // defined(FEAT_NETBEANS_INTG)
