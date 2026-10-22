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

#جيڪڏهن_وصف_نه_ٿيل MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#جيڪڏهن_پورو

#ضمن "version.h"

#وصف_ڪريو GUARDED		10000 // typenr for "guarded" annotation
#وصف_ڪريو GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#وصف_ڪريو MAX_COLOR_ڊيگهائي 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#وصف_ڪريو VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#وصف_ڪريو VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#وصف_ڪريو VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
ساكن حرف *Ext_Ed_Protocol_نسخو = "2.5";

ساكن ڊگهو pos2off(buf_T *, pos_T *);
ساكن pos_T *off2pos(buf_T *, ڊگهو);
ساكن pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
ساكن ڊگهو get_buf_سائيز(buf_T *);
ساكن صحيح netbeans_keystring(char_u *keystr);
ساكن فراغ special_ڪنجيون(char_u *دليلون);

ساكن صحيح getConnInfo(حرف *file, حرف **host, حرف **پورٽ, حرف **password);

ساكن فراغ nb_init_graphics(فراغ);
ساكن فراغ coloncmd(حرف *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ساكن فراغ nb_سيٽ_curbuf(buf_T *buf);
ساكن فراغ nb_parse_cmd(char_u *);
ساكن صحيح  nb_do_cmd(صحيح, char_u *, صحيح, صحيح, char_u *);
ساكن فراغ nb_send(حرف *buf, حرف *fun);
ساكن فراغ nb_free(فراغ);

#وصف_ڪريو NETBEANS_OPEN (channel_can_write_منزل(nb_channel))
ساكن channel_T *nb_channel = عدم;

ساكن صحيح r_cmdno;			// current command number for reply
ساكن صحيح dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#جيڪڏهن_وصف_ٿيل NBDEBUG
# include "nbdebug.c"
#جيڪڏهن_پورو

ساكن صحيح needupdate = 0;
ساكن صحيح اندريون_Atomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    ساكن صحيح
nb_is_محفوظ_string(char_u *str, char_u *allowed)
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

#جيڪڏهن_وصف_ٿيل FEAT_BEVAL_GUI
    bevalServers &= ~BEVAL_NETBEANS;
#جيڪڏهن_پورو

    needupdate = 0;
    اندريون_Atomic = 0;
    nb_free();

    // remove all signs and update the screen after gutter removal
    coloncmd(":sign unplace *");
    بدليو_دري_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    ٻاهريون_flush_cursor(TRUE, FALSE);
}

#وصف_ڪريو NB_DEF_HOST "localhost"
#وصف_ڪريو NB_DEF_ADDR "3219"
#وصف_ڪريو NB_DEF_PASS "changeme"

    ساكن صحيح
netbeans_connect(حرف *params, صحيح doabort)
{
    صحيح		پورٽ;
    حرف	buf[32];
    حرف	*hostname = عدم;
    حرف	*پتو = عدم;
    حرف	*password = عدم;
    حرف	*fname;
    حرف	*arg = عدم;

    إذا (*params == '=')
    {
	// "=fname": Read info from specified file.
	إذا (getConnInfo(params + 1, &hostname, &پتو, &password) == FAIL)
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
	    إذا (getConnInfo(fname, &hostname, &پتو, &password) == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    إذا (arg != عدم)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		پتو = strchr(hostname, ':');
		إذا (پتو != عدم)
		{
		    *پتو++ = '\0';
		    password = strchr(پتو, ':');
		    إذا (password != عدم)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    إذا (hostname == عدم || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    إذا (پتو == عدم)
		پتو = getenv("__NETBEANS_SOCKET");
	    إذا (password == عدم)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    إذا (hostname != عدم)
		hostname = (حرف *)vim_strsave((char_u *)hostname);
	    إذا (پتو != عدم)
		پتو = (حرف *)vim_strsave((char_u *)پتو);
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
    إذا (پتو == عدم || *پتو == '\0')
    {
	vim_free(پتو);
	پتو = (حرف *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    إذا (password == عدم || *password == '\0')
    {
	vim_free(password);
	password = (حرف *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    إذا (hostname != عدم && پتو != عدم && password != عدم)
    {
	پورٽ = atoi(پتو);
	nb_channel = channel_open(hostname, پورٽ, 3000, nb_channel_closed);
	إذا (nb_channel != عدم)
	{
	    // success
#جيڪڏهن_وصف_ٿيل FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#جيڪڏهن_پورو

	    // success, login
	    vim_snprintf(buf, حجم(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", Ext_Ed_Protocol_نسخو);
	    nb_send(buf, "externaleditor_version");
	}
    }

    إذا (nb_channel == عدم && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(پتو);
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
getConnInfo(حرف *file, حرف **host, حرف **پورٽ, حرف **auth)
{
    FILE *fp;
    char_u buf[BUFSIZ];
    char_u *lp;
    char_u *nlp;
#جيڪڏهن_وصف_ٿيل UNIX
    stat_T	st;

    /*
     * For Unix only accept the file when it's not accessible by others.
     * The open will then fail if we don't own the file.
     */
    إذا (mch_stat(file, &st) == 0 && (st.st_گهڻائي & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_گهڻائي_for_netbeans_connection_info_file_str),
									file);
	أرجع FAIL;
    }
#جيڪڏهن_پورو

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
	    vim_free(*پورٽ);
	    *پورٽ = (حرف *)vim_strsave(&buf[5]);
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
    بنية keyqueue *ايندڙ;
    بنية keyqueue *prev;
};

تعريفنوع بنية keyqueue ڪنجي_Q_T;

ساكن ڪنجي_Q_T ڪنجي_سر; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    ساكن فراغ
postpone_keycommand(char_u *keystr)
{
    ڪنجي_Q_T *ڳنڍ;

    ڳنڍ = ALLOC_ONE(ڪنجي_Q_T);
    إذا (ڳنڍ == عدم)
	أرجع;  // out of memory, drop the key

    إذا (ڪنجي_سر.ايندڙ == عدم) // initialize circular queue
    {
	ڪنجي_سر.ايندڙ = &ڪنجي_سر;
	ڪنجي_سر.prev = &ڪنجي_سر;
    }

    // insert node at tail of queue
    ڳنڍ->ايندڙ = &ڪنجي_سر;
    ڳنڍ->prev = ڪنجي_سر.prev;
    ڪنجي_سر.prev->ايندڙ = ڳنڍ;
    ڪنجي_سر.prev = ڳنڍ;

    ڳنڍ->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    ساكن فراغ
handle_ڪنجي_قطار(فراغ)
{
    صحيح postponed = FALSE;

    طالما (!postponed && ڪنجي_سر.ايندڙ && ڪنجي_سر.ايندڙ != &ڪنجي_سر)
    {
	// first, unlink the node
	ڪنجي_Q_T *ڳنڍ = ڪنجي_سر.ايندڙ;
	ڪنجي_سر.ايندڙ = ڳنڍ->ايندڙ;
	ڳنڍ->ايندڙ->prev = ڳنڍ->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	إذا (ڳنڍ->keystr != عدم)
	    postponed = !netbeans_keystring(ڳنڍ->keystr);
	vim_free(ڳنڍ->keystr);

	// Finally, dispose of the node
	vim_free(ڳنڍ);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    فراغ
netbeans_parse_messages(فراغ)
{
    readq_T	*ڳنڍ;
    char_u	*بفر;
    char_u	*p;
    صحيح		own_ڳنڍ;

    طالما (nb_channel != عدم)
    {
	ڳنڍ = channel_peek(nb_channel, PART_SOCK);
	إذا (ڳنڍ == عدم)
	    اكسر;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_پهريون_nl(ڳنڍ);
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
	    own_ڳنڍ = TRUE;
	    بفر = channel_get(nb_channel, PART_SOCK, عدم);
	    // "node" is now invalid!
	}
	وإلا
	{
	    own_ڳنڍ = FALSE;
	    بفر = ڳنڍ->rq_بفر;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(بفر);

	إذا (own_ڳنڍ)
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
	    buf->b_has_sign_ڪالم = false;

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
	semsg(_(e_missing_bang_or_slash_اندريون_str), cmd);
	أرجع;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (حرف *)skipwhite((char_u *)q);

    إذا (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
    {
#جيڪڏهن_وصف_ٿيل NBDEBUG
	/*
	 * This happens because the ExtEd can send a command or 2 after
	 * doing a stopDocumentListen command. It doesn't harm anything
	 * so I'm disabling it except for debugging.
	 */
	nbdebug(("nb_parse_cmd: Command error for \"%s\"\n", cmd));
	emsg(e_bad_return_from_nb_do_cmd);
#جيڪڏهن_پورو
    }
}

بنية nbbuf_struct
{
    buf_T		*bufp;
    بنانشان صحيح	 fireChanges:1;
    بنانشان صحيح	 init_پورو:1;
    بنانشان صحيح	 وجهو_پورو:1;
    بنانشان صحيح	 modified:1;
    صحيح			 nbbuf_عدد;
    حرف		*displayname;
    صحيح			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

تعريفنوع بنية nbbuf_struct nbbuf_T;

ساكن nbbuf_T *buf_list = عدم;
ساكن صحيح buf_list_سائيز = 0;	// size of buf_list
ساكن صحيح buf_list_استعمال_ٿيل = 0;	// nr of entries in buf_list actually in use

ساكن حرف **globalsignmap = عدم;
ساكن صحيح globalsignmaplen = 0;
ساكن صحيح globalsignmapused = 0;

ساكن صحيح  mapsigntype(nbbuf_T *, صحيح localsigntype);
ساكن فراغ addsigntype(nbbuf_T *, صحيح localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
ساكن فراغ print_read_نياپو(nbbuf_T *buf);
ساكن فراغ print_save_نياپو(nbbuf_T *buf, off_T nchars);

ساكن صحيح curPCtype = -1;

/*
 * Free netbeans resources.
 */
    ساكن فراغ
nb_free(فراغ)
{
    ڪنجي_Q_T *ڪنجي_ڳنڍ = ڪنجي_سر.ايندڙ;
    nbbuf_T buf;
    صحيح i;

    // free the netbeans buffer list
    لكل (i = 0; i < buf_list_استعمال_ٿيل; i++)
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
    buf_list_سائيز = 0;
    buf_list_استعمال_ٿيل = 0;

    // free the queued key commands
    طالما (ڪنجي_ڳنڍ != عدم && ڪنجي_ڳنڍ != &ڪنجي_سر)
    {
	ڪنجي_Q_T *ايندڙ = ڪنجي_ڳنڍ->ايندڙ;
	vim_free(ڪنجي_ڳنڍ->keystr);
	vim_free(ڪنجي_ڳنڍ);
	إذا (ايندڙ == &ڪنجي_سر)
	{
	    ڪنجي_سر.ايندڙ = &ڪنجي_سر;
	    ڪنجي_سر.prev = &ڪنجي_سر;
	    اكسر;
	}
	ڪنجي_ڳنڍ = ايندڙ;
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

    لكل (i = 0; i < buf_list_استعمال_ٿيل; i++)
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
	buf_list_سائيز = 100;
    }
    إذا (bufno >= buf_list_استعمال_ٿيل) // new
    {
	إذا (bufno >= buf_list_سائيز) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_سائيز + 90;
	    buf_list_سائيز += incr;
	    bufsize = buf_list_سائيز * حجم(nbbuf_T);
	    إذا (bufsize == 0 || bufsize / حجم(nbbuf_T)
						      != (size_t)buf_list_سائيز)
	    {
		// list size overflow, bail out
		أرجع عدم;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    إذا (buf_list == عدم)
	    {
		vim_free(t_buf_list);
		buf_list_سائيز = 0;
		أرجع عدم;
	    }
	    vim_memset(buf_list + buf_list_سائيز - incr, 0,
						      incr * حجم(nbbuf_T));
	}

	طالما (buf_list_استعمال_ٿيل <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_استعمال_ٿيل].fireChanges = 1;
	    ++buf_list_استعمال_ٿيل;
	}
    }

    أرجع buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    ساكن صحيح
ڳڻپ_بدليو_buffers(فراغ)
{
    buf_T	*bufp;
    صحيح		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	إذا (bufp->b_بدليو)
	    ++n;
    أرجع n;
}

/*
 * End the netbeans session.
 */
    فراغ
netbeans_پڇاڙي(فراغ)
{
    صحيح i;
    ساكن حرف buf[128];

    إذا (!NETBEANS_OPEN)
	أرجع;

    لكل (i = 0; i < buf_list_استعمال_ٿيل; i++)
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
nb_جواب_nil(صحيح cmdno)
{
    حرف جواب[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    إذا (!NETBEANS_OPEN)
	أرجع;

    sprintf(جواب, "%d\n", cmdno);
    nb_send(جواب, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    ساكن فراغ
nb_جواب_متن(صحيح cmdno, char_u *نتيجو)
{
    char_u *جواب;

    nbdebug(("REP %d: %s\n", cmdno, (حرف *)نتيجو));

    جواب = alloc(STRLEN(نتيجو) + 32);
    sprintf((حرف *)جواب, "%d %s\n", cmdno, (حرف *)نتيجو);
    nb_send((حرف *)جواب, "nb_reply_text");

    vim_free(جواب);
}


/*
 * Send a response with a number result code.
 */
    ساكن فراغ
nb_جواب_nr(صحيح cmdno, ڊگهو نتيجو)
{
    حرف جواب[32];

    nbdebug(("REP %d: %ld\n", cmdno, نتيجو));

    sprintf(جواب, "%d %ld\n", cmdno, نتيجو);
    nb_send(جواب, "nb_reply_nr");
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
    حرف *نتيجو = 0;
    حرف *q;
    صحيح پورو = 0;

    // result is never longer than input
    نتيجو = alloc_clear(STRLEN(p) + 1);
    إذا (نتيجو == عدم)
	أرجع عدم;

    إذا (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	نتيجو[0] = NUL;
	أرجع نتيجو;
    }

    لكل (q = نتيجو; !پورو && *p != NUL;)
    {
	بدل (*p)
	{
	    حالة '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		پورو = 1;
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

    أرجع نتيجو;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    ساكن فراغ
nb_partialremove(linenr_T lnum, colnr_T پهريون, colnr_T آخري)
{
    char_u *oldtext, *newtext;
    صحيح oldlen;
    صحيح lastbyte = آخري;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    إذا (پهريون >= (colnr_T)oldlen || oldlen == 0)  // just in case
	أرجع;
    إذا (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - پهريون));
    إذا (newtext == عدم)
	أرجع;

    mch_memmove(newtext, oldtext, پهريون);
    STRMOVE(newtext + پهريون, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    ساكن فراغ
nb_joinlines(linenr_T پهريون, linenr_T other)
{
    صحيح len_پهريون, len_other;
    char_u *p;

    len_پهريون = ml_get_len(پهريون);
    len_other = ml_get_len(other);
    p = alloc(len_پهريون + len_other + 1);
    إذا (p == عدم)
	أرجع;

    mch_memmove(p, ml_get(پهريون), len_پهريون);
    mch_memmove(p + len_پهريون, ml_get(other), len_other + 1);
    ml_replace(پهريون, p, FALSE);
}

#وصف_ڪريو SKIP_STOP 2
#وصف_ڪريو streq(a,b) (strcmp(a,b) == 0)

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
    char_u	*دليلون)	    // points to space before arguments or NUL
{
    صحيح		do_update = 0;
    ڊگهو	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    ساكن صحيح	skip = 0;
    صحيح		retval = OK;
    حرف	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (حرف *)دليلون));

    إذا (func)
    {
// =====================================================================
	إذا (streq((حرف *)cmd, "getModified"))
	{
	    إذا (buf == عدم || buf->bufp == عدم)
		// Return the number of buffers that are modified.
		nb_جواب_nr(cmdno, (ڊگهو)ڳڻپ_بدليو_buffers());
	    وإلا
		// Return whether the buffer is modified.
		nb_جواب_nr(cmdno, (ڊگهو)(buf->bufp->b_بدليو
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_جواب_nr(cmdno, (ڊگهو)ڳڻپ_بدليو_buffers());
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
		    (ڊگهو)curwin->w_cursor.lnum,
		    (صحيح)curwin->w_cursor.کانو,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_جواب_متن(cmdno, متن);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "getAnno"))
	{
	    ڊگهو linenum = 0;
#جيڪڏهن_وصف_ٿيل FEAT_SIGNS
	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_getanno));
		retval = FAIL;
	    }
	    وإلا
	    {
		صحيح ser_انگ;

		cp = (حرف *)دليلون;
		ser_انگ = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (ڊگهو)buf_findsign(buf->bufp, ser_انگ, عدم);
	    }
#جيڪڏهن_پورو
	    nb_جواب_nr(cmdno, linenum);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "getLength"))
	{
	    ڊگهو len = 0;

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_getlength));
		retval = FAIL;
	    }
	    وإلا
	    {
		len = get_buf_سائيز(buf->bufp);
	    }
	    nb_جواب_nr(cmdno, len);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "getText"))
	{
	    ڊگهو	len;
	    linenr_T	nlines;
	    char_u	*متن = عدم;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*سٽ;

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_gettext));
		retval = FAIL;
	    }
	    وإلا
	    {
		len = get_buf_سائيز(buf->bufp);
		nlines = buf->bufp->b_ml.ml_سٽ_ڳڻپ;
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
			سٽ = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			إذا (سٽ != عدم)
			{
			    STRCPY(p, سٽ);
			    p += STRLEN(سٽ);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(سٽ);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    إذا (متن == عدم)
		nb_جواب_متن(cmdno, (char_u *)"");
	    وإلا
	    {
		nb_جواب_متن(cmdno, متن);
		vim_free(متن);
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "remove"))
	{
	    ڊگهو ڳڻپ;
	    pos_T پهريون, آخري;
	    pos_T *pos;
	    pos_T *ايندڙ;
	    linenr_T del_from_lnum, del_منزل_lnum;  // lines to be deleted as a whole
	    صحيح oldFire = netbeansFireChanges;
	    صحيح oldSuppress = netbeans_Suppress_No_سٽون;
	    صحيح was_بدليو_2;

	    إذا (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (حرف *) cmd));
		nb_جواب_nil(cmdno);
		أرجع OK;
	    }

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_remove));
		retval = FAIL;
	    }
	    وإلا
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_سٽون = TRUE;

		nb_سيٽ_curbuf(buf->bufp);
		was_بدليو_2 = buf->bufp->b_بدليو;
		cp = (حرف *)دليلون;
		off = strtol(cp, &cp, 10);
		ڳڻپ = strtol(cp, &cp, 10);
		دليلون = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		إذا (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_جواب_متن(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_سٽون = oldSuppress;
		    أرجع FAIL;
		}
		پهريون = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      پهريون.lnum, پهريون.کانو));
		pos = off2pos(buf->bufp, off+ڳڻپ-1);
		إذا (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_جواب_متن(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_سٽون = oldSuppress;
		    أرجع FAIL;
		}
		آخري = *pos;
		nbdebug(("    LAST POS: line %ld, col %d\n",
							آخري.lnum, آخري.کانو));
		del_from_lnum = پهريون.lnum;
		del_منزل_lnum = آخري.lnum;
		do_update = 1;

		// Get the position of the first byte after the deleted
		// section.  "next" is NULL when deleting to the end of the
		// file.
		ايندڙ = off2pos(buf->bufp, off + ڳڻپ);

		// Remove part of the first line.
		إذا (پهريون.کانو != 0
				|| (ايندڙ != عدم && پهريون.lnum == ايندڙ->lnum))
		{
		    إذا (پهريون.lnum != آخري.lnum
			    || (ايندڙ != عدم && پهريون.lnum != ايندڙ->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(پهريون.lnum, پهريون.کانو,
							     (colnr_T)MAXCOL);
			إذا (پهريون.lnum == آخري.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(پهريون.lnum, ايندڙ->lnum);
			    del_منزل_lnum = ايندڙ->lnum;
			}
		    }
		    وإلا
		    {
			// remove within one line
			nb_partialremove(پهريون.lnum, پهريون.کانو, آخري.کانو);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		إذا (پهريون.lnum != آخري.lnum && ايندڙ != عدم
			&& ايندڙ->کانو != 0 && آخري.lnum == ايندڙ->lnum)
		{
		    nb_partialremove(آخري.lnum, 0, آخري.کانو);
		    إذا (del_from_lnum > پهريون.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(پهريون.lnum, آخري.lnum);
		    }
		    وإلا
			// First line is deleted as a whole, keep the last
			// line.
			--del_منزل_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		إذا (پهريون.lnum != آخري.lnum && del_from_lnum > پهريون.lnum
			&& ايندڙ != عدم && آخري.lnum != ايندڙ->lnum)
		{
		    nb_joinlines(پهريون.lnum, ايندڙ->lnum);
		    del_منزل_lnum = ايندڙ->lnum;
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
		    curwin->w_cursor.کانو = 0;
		    del_سٽون(del_منزل_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = پهريون;
		check_cursor_lnum();
		buf->bufp->b_بدليو = was_بدليو_2; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_سٽون = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_جواب_nil(cmdno);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "insert"))
	{
	    char_u	*منزل_free;

	    إذا (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (حرف *) cmd));
		nb_جواب_nil(cmdno);
		أرجع OK;
	    }

	    // get offset
	    cp = (حرف *)دليلون;
	    off = strtol(cp, &cp, 10);
	    دليلون = (char_u *)cp;

	    // get text to be inserted
	    دليلون = skipwhite(دليلون);
	    دليلون = منزل_free = (char_u *)nb_unquote(دليلون, عدم);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_وجهو));
		retval = FAIL;
	    }
	    وإلا إذا (دليلون != عدم)
	    {
		صحيح	ff_detected = EOL_UNKNOWN;
		صحيح	buf_was_خالي_جايون = (buf->bufp->b_ml.ml_flags & ML_خالي_جايون);
		size_t	len = 0;
		صحيح	added = 0;
		صحيح	oldFire = netbeansFireChanges;
		صحيح	old_b_بدليو;
		char_u	*nlp;
		linenr_T lnum;
		linenr_T lnum_شروعات;
		pos_T	*pos;

		netbeansFireChanges = 0;

		// Jump to the buffer where we insert.  After this "curbuf"
		// can be used.
		nb_سيٽ_curbuf(buf->bufp);
		old_b_بدليو = curbuf->b_بدليو;

		// Convert the specified character offset into a lnum/col
		// position.
		pos = off2pos(curbuf, off);
		إذا (pos != عدم)
		{
		    إذا (pos->lnum <= 0)
			lnum_شروعات = 1;
		    وإلا
			lnum_شروعات = pos->lnum;
		}
		وإلا
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    إذا (buf_was_خالي_جايون)
			lnum_شروعات = 1;	    // above empty line
		    وإلا
			lnum_شروعات = curbuf->b_ml.ml_سٽ_ڳڻپ + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_شروعات;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		طالما (*دليلون != NUL)
		{
		    nlp = vim_strchr(دليلون, '\n');
		    إذا (nlp == عدم)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(دليلون);
		    }
		    وإلا
		    {
			len = nlp - دليلون;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			إذا (nlp > دليلون && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			وإلا
			    ff_detected = EOL_UNIX;
		    }
		    دليلون[len] = NUL;

		    إذا (lnum == lnum_شروعات
			    && ((pos != عدم && pos->کانو > 0)
				|| (lnum == 1 && buf_was_خالي_جايون)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			صحيح	کانو = pos == عدم ? 0 : pos->کانو;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			إذا (newline != عدم)
			{
			    mch_memmove(newline, oldline, (size_t)کانو);
			    newline[کانو] = NUL;
			    STRCAT(newline, دليلون);
			    STRCAT(newline, oldline + کانو);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    وإلا
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), دليلون,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    إذا (nlp == عدم)
			اكسر;
		    ++lnum;
		    دليلون = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_سٽون_mark(lnum_شروعات - 1, (ڊگهو)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		إذا (buf_was_خالي_جايون)
		{
		    إذا (ff_detected == EOL_UNKNOWN)
#إذا defined(MSWIN)
			ff_detected = EOL_DOS;
#وإلا
			ff_detected = EOL_UNIX;
#جيڪڏهن_پورو
		    سيٽ_fileformat(ff_detected, OPT_LOCAL);
		    curbuf->b_شروعات_ffc = *curbuf->b_p_ff;
		}

		/*
		 * XXX - GRP - Is the next line right? If I've inserted
		 * text the buffer has been updated but not written. Will
		 * netbeans guarantee to write it? Even if I do a :q! ?
		 */
		curbuf->b_بدليو = old_b_بدليو; // logically unchanged
		netbeansFireChanges = oldFire;

		// Undo info is invalid now...
		u_clearallandblockfree(curbuf);
	    }
	    vim_free(منزل_free);
	    nb_جواب_nil(cmdno); // or !error
	}
	وإلا
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_جواب_nil(cmdno);
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
		emsg(_(e_invalid_بفر_identifier_اندريون_create));
		أرجع FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, عدم, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->وجهو_پورو = FALSE;
#إذا defined(FEAT_MENU) && defined(FEAT_GUI)
	    إذا (gui.اندريون_use)
		gui_update_menus(0);
#جيڪڏهن_پورو
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
		buf->bufp->b_شروعات_eol = *دليلون == 'T';
		buf->وجهو_پورو = TRUE;
		دليلون += 2;
		buf->bufp->b_p_ro = *دليلون == 'T';
		print_read_نياپو(buf);
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "saveDone"))
	{
	    ڊگهو savedChars = atol((حرف *)دليلون);

	    إذا (buf == عدم || buf->bufp == عدم)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    وإلا
		print_save_نياپو(buf, savedChars);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "startDocumentListen"))
	{
	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_startdocumentlisten));
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
		emsg(_(e_invalid_بفر_identifier_اندريون_stopdocumentlisten));
		أرجع FAIL;
	    }
	    buf->fireChanges = 0;
	    إذا (buf->bufp != عدم && buf->bufp->b_was_netbeans_file)
	    {
		إذا (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_گم_for_بفر_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_گم_for_بفر_nr),
							   buf->bufp->b_fnum);
		}
		وإلا
		{
		    // NetBeans uses stopDocumentListen when it stops editing
		    // a file.  It then expects the buffer in Vim to
		    // disappear.
		    do_bufdel(DOBUF_DEL, (char_u *)"", 1,
				  buf->bufp->b_fnum, buf->bufp->b_fnum, TRUE);
		    CLEAR_اشارو(buf);
		}
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setTitle"))
	{
	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_settitle));
		أرجع FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(دليلون, عدم);
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "initDone"))
	{
	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_initdone));
		أرجع FAIL;
	    }
	    do_update = 1;
	    buf->init_پورو = TRUE;
	    nb_سيٽ_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_ڪنجي_قطار();
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setBufferNumber")
		|| streq((حرف *)cmd, "putBufferNumber"))
	{
	    char_u	*رستو;
	    buf_T	*bufp;

	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_setbuffernumber));
		أرجع FAIL;
	    }
	    رستو = (char_u *)nb_unquote(دليلون, عدم);
	    إذا (رستو == عدم)
		أرجع FAIL;
	    bufp = buflist_findname(رستو);
	    vim_free(رستو);
	    إذا (bufp == عدم)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", دليلون));
		semsg(_(e_file_str_not_مليو_اندريون_setbuffernumber), دليلون);
		أرجع FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_عدد = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    إذا (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    وإلا
	    {
		buf->init_پورو = TRUE;

		// handle any postponed key commands
		handle_ڪنجي_قطار();
	    }

// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setFullName"))
	{
	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_setfullname));
		أرجع FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(دليلون, عدم);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#إذا defined(FEAT_MENU) && defined(FEAT_GUI)
	    إذا (gui.اندريون_use)
		gui_update_menus(0);
#جيڪڏهن_پورو
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "editFile"))
	{
	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_editfile));
		أرجع FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(دليلون, عدم);
	    do_ecmd(0, (char_u *)buf->displayname, عدم, عدم, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->init_پورو = TRUE;
	    do_update = 1;
	    maketitle();
#إذا defined(FEAT_MENU) && defined(FEAT_GUI)
	    إذا (gui.اندريون_use)
		gui_update_menus(0);
#جيڪڏهن_پورو
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
		    emsg(_(e_invalid_بفر_identifier_اندريون_setvisible));
		أرجع FAIL;
	    }
	    إذا (streq((حرف *)دليلون, "T") && buf->bufp != curbuf)
	    {
		exarg_T exarg;
		CLEAR_ميدان(exarg);
		exarg.cmd = (char_u *)"goto";
		exarg.forceit = FALSE;
		exarg.cmdidx = CMD_USER;
		dosetvisible = TRUE;
		goto_بفر(&exarg, DOBUF_پهريون, FORWARD, buf->bufp->b_fnum);
		do_update = 1;
		dosetvisible = FALSE;

#جيڪڏهن_وصف_ٿيل FEAT_GUI
		// Side effect!!!.
		إذا (gui.اندريون_use)
		    gui_mch_سيٽ_foreground();
#جيڪڏهن_پورو
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "raise"))
	{
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	    // Bring gvim to the foreground.
	    إذا (gui.اندريون_use)
		gui_mch_سيٽ_foreground();
#جيڪڏهن_پورو
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setModified"))
	{
	    صحيح prev_b_بدليو;

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		إذا (p_verbose > 0)
		    emsg(_(e_invalid_بفر_identifier_اندريون_setmodified));
		أرجع FAIL;
	    }
	    prev_b_بدليو = buf->bufp->b_بدليو;
	    إذا (streq((حرف *)دليلون, "T"))
		buf->bufp->b_بدليو = TRUE;
	    وإلا
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		إذا (buf->bufp->b_ffname != عدم
			&& mch_stat((حرف *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_وقت(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_بدليو = FALSE;
	    }
	    buf->modified = buf->bufp->b_بدليو;
	    إذا (prev_b_بدليو != buf->bufp->b_بدليو)
	    {
		check_حالت(buf->bufp);
		redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#جيڪڏهن_پورو
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
		buf->bufp->b_mtime = atoi((حرف *)دليلون);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setReadOnly"))
	{
	    إذا (buf == عدم || buf->bufp == عدم)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    وإلا إذا (streq((حرف *)دليلون, "T"))
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
		متن = nb_unquote(دليلون, عدم);
		إذا (متن != عدم)
		    gui_mch_post_balloon(balloonEval, (char_u *)متن);
	    }
#جيڪڏهن_پورو
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "setDot"))
	{
	    pos_T *pos;
#جيڪڏهن_وصف_ٿيل NBDEBUG
	    char_u *s;
#جيڪڏهن_پورو

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_setdot));
		أرجع FAIL;
	    }

	    nb_سيٽ_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    إذا (VIsual_سرگرم)
		پڇاڙي_visual_گهڻائي();
#جيڪڏهن_وصف_ٿيل NBDEBUG
	    s = دليلون;
#جيڪڏهن_پورو
	    pos = get_off_or_lnum(buf->bufp, &دليلون);
	    إذا (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		foldOpenCursor();
#جيڪڏهن_پورو
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
	    ٻاهريون_flush_cursor(TRUE, FALSE);

	    // Quit a hit-return or more prompt.
	    إذا (State == گهڻائي_HITRETURN || State == گهڻائي_ASKMORE)
	    {
#جيڪڏهن_وصف_ٿيل FEAT_GUI_GTK
		إذا (gui.اندريون_use && gtk_main_level() > 0)
		    gtk_main_quit();
#جيڪڏهن_پورو
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "close"))
	{
#جيڪڏهن_وصف_ٿيل NBDEBUG
	    حرف *name = "<NONE>";
#جيڪڏهن_پورو

	    إذا (buf == عدم)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_close));
		أرجع FAIL;
	    }

#جيڪڏهن_وصف_ٿيل NBDEBUG
	    إذا (buf->displayname != عدم)
		name = buf->displayname;
#جيڪڏهن_پورو
	    إذا (buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		إذا (p_verbose > 0)
		    emsg(_(e_invalid_بفر_identifier_اندريون_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	    need_mouse_correct = TRUE;
#جيڪڏهن_پورو
	    إذا (buf->bufp != عدم)
		do_بفر(DOBUF_WIPE, DOBUF_پهريون, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = عدم;
	    buf->init_پورو = FALSE;
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
#جيڪڏهن_وصف_ٿيل FEAT_SIGNS
	    صحيح type_انگ;
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
		emsg(_(e_invalid_بفر_identifier_اندريون_defineannotype));
		أرجع FAIL;
	    }

	    cp = (حرف *)دليلون;
	    type_انگ = strtol(cp, &cp, 10);
	    دليلون = (char_u *)cp;
	    دليلون = skipwhite(دليلون);
	    typeName = (char_u *)nb_unquote(دليلون, &دليلون);
	    دليلون = skipwhite(دليلون + 1);
	    tooltip = (char_u *)nb_unquote(دليلون, &دليلون);
	    دليلون = skipwhite(دليلون + 1);

	    p = (char_u *)nb_unquote(دليلون, &دليلون);
	    glyphFile = vim_strsave_escaped(p, escape_chars);
	    vim_free(p);

	    دليلون = skipwhite(دليلون + 1);
	    p = skiptowhite(دليلون);
	    إذا (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(دليلون);
	    bg = vim_strsave(p);
	    إذا (STRLEN(fg) > MAX_COLOR_ڊيگهائي || STRLEN(bg) > MAX_COLOR_ڊيگهائي)
	    {
		emsg(_(e_highlighting_color_name_too_long_اندريون_define_Anno_Type));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    وإلا إذا (!nb_is_محفوظ_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_محفوظ_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_محفوظ_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_اندريون_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    وإلا إذا (typeName != عدم && tooltip != عدم && glyphFile != عدم)
		addsigntype(buf, type_انگ, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    إذا (parse_error)
		أرجع FAIL;

#جيڪڏهن_پورو
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "addAnno"))
	{
#جيڪڏهن_وصف_ٿيل FEAT_SIGNS
	    صحيح ser_انگ;
	    صحيح local_Type_انگ;
	    صحيح type_انگ;
	    pos_T *pos;

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_بفر_identifier_اندريون_addanno));
		أرجع FAIL;
	    }

	    do_update = 1;

	    cp = (حرف *)دليلون;
	    ser_انگ = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_انگ = strtol(cp, &cp, 10);
	    دليلون = (char_u *)cp;
	    type_انگ = mapsigntype(buf, local_Type_انگ);

	    pos = get_off_or_lnum(buf->bufp, &دليلون);

	    cp = (حرف *)دليلون;
	    vim_ignored = (صحيح)strtol(cp, &cp, 10);
	    دليلون = (char_u *)cp;
# ifdef NBDEBUG
	    إذا (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    إذا (ser_انگ >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		أرجع FAIL;
	    }
	    إذا (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_انگ, pos->lnum, type_انگ, buf->bufp->b_fnum);
		إذا (type_انگ == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_انگ,
						       buf->bufp->b_fnum);
	    }
#جيڪڏهن_پورو
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "removeAnno"))
	{
#جيڪڏهن_وصف_ٿيل FEAT_SIGNS
	    صحيح ser_انگ;

	    إذا (buf == عدم || buf->bufp == عدم)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		أرجع FAIL;
	    }
	    do_update = 1;
	    cp = (حرف *)دليلون;
	    ser_انگ = strtol(cp, &cp, 10);
	    دليلون = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_انگ, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#جيڪڏهن_پورو
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "moveAnnoToFront"))
	{
#جيڪڏهن_وصف_ٿيل FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#جيڪڏهن_پورو
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "guard") || streq((حرف *)cmd, "unguard"))
	{
	    صحيح len;
	    pos_T پهريون;
	    pos_T آخري;
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
	    nb_سيٽ_curbuf(buf->bufp);
	    cp = (حرف *)دليلون;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, عدم, 10);
	    دليلون = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    إذا (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    وإلا
	    {
		پهريون = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		إذا (pos != عدم && pos->کانو == 0)
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
		    ڊگهو lnum;
		    آخري = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     پهريون.lnum, پهريون.کانو,
			     آخري.lnum, آخري.کانو));
#جيڪڏهن_وصف_ٿيل FEAT_SIGNS
		    لكل (lnum = پهريون.lnum; lnum <= آخري.lnum; lnum++)
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
#جيڪڏهن_پورو
		    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
		}
	    }
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "startAtomic"))
	{
	    اندريون_Atomic = 1;
// =====================================================================
	}
	وإلا إذا (streq((حرف *)cmd, "endAtomic"))
	{
	    اندريون_Atomic = 0;
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
	    إذا (buf_Is_بدليو(buf->bufp))
	    {
		// Only write if the buffer can be written.
		إذا (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != عدم
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    سيٽ_bufref(&bufref, buf->bufp);
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
	    إذا (*دليلون == 'T')
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
	    special_ڪنجيون(دليلون);
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
    إذا (اندريون_Atomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    إذا (buf != عدم && buf->init_پورو && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	ٻاهريون_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	إذا (State == گهڻائي_HITRETURN || State == گهڻائي_ASKMORE)
	{
#جيڪڏهن_وصف_ٿيل FEAT_GUI_GTK
	    إذا (gui.اندريون_use && gtk_main_level() > 0)
		gtk_main_quit();
#جيڪڏهن_پورو
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
nb_سيٽ_curbuf(buf_T *buf)
{
    إذا (curbuf == buf)
	أرجع;

    إذا (buf_jump_open_win(buf) != عدم)
	أرجع;
    إذا ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != عدم)
	أرجع;
    سيٽ_curbuf(buf, DOBUF_GOTO);
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
    ٻاهريون_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    ساكن فراغ
special_ڪنجيون(char_u *دليلون)
{
    حرف *save_str = nb_unquote(دليلون, عدم);
    حرف *tok = strtok(save_str, " ");
    حرف *sep;
#وصف_ڪريو KEYBUFLEN 64
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

	إذا (لڙي_ڊيگهه(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    صحيح محفوظ = TRUE;
	    لكل (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		إذا (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    محفوظ = FALSE;
		    اكسر;
		}
	    }
	    إذا (محفوظ)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, حجم(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, گهڻائي_عام, FALSE);
	    }
	}
	tok = strtok(عدم, " ");
    }
    vim_free(save_str);
}

    فراغ
ex_nbclose(exarg_T *eap اڻ_استعمال_ٿيل)
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
#جيڪڏهن_وصف_ٿيل FEAT_GUI
# إذا !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    إذا (gui.اندريون_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	أرجع;
    }
# endif
#جيڪڏهن_پورو
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
netbeans_keyname(صحيح ڪنجي, حرف *buf)
{
    حرف *name = 0;
    حرف namebuf[2];
    صحيح ctrl  = 0;
    صحيح سرڪايو = 0;
    صحيح alt   = 0;

    إذا (mod_نقاب & MOD_نقاب_CTRL)
	ctrl = 1;
    إذا (mod_نقاب & MOD_نقاب_سرڪايو)
	سرڪايو = 1;
    إذا (mod_نقاب & MOD_نقاب_ALT)
	alt = 1;


    بدل (ڪنجي)
    {
	حالة K_F1:		name = "F1";		اكسر;
	حالة K_S_F1:	name = "F1";	سرڪايو = 1;	اكسر;
	حالة K_F2:		name = "F2";		اكسر;
	حالة K_S_F2:	name = "F2";	سرڪايو = 1;	اكسر;
	حالة K_F3:		name = "F3";		اكسر;
	حالة K_S_F3:	name = "F3";	سرڪايو = 1;	اكسر;
	حالة K_F4:		name = "F4";		اكسر;
	حالة K_S_F4:	name = "F4";	سرڪايو = 1;	اكسر;
	حالة K_F5:		name = "F5";		اكسر;
	حالة K_S_F5:	name = "F5";	سرڪايو = 1;	اكسر;
	حالة K_F6:		name = "F6";		اكسر;
	حالة K_S_F6:	name = "F6";	سرڪايو = 1;	اكسر;
	حالة K_F7:		name = "F7";		اكسر;
	حالة K_S_F7:	name = "F7";	سرڪايو = 1;	اكسر;
	حالة K_F8:		name = "F8";		اكسر;
	حالة K_S_F8:	name = "F8";	سرڪايو = 1;	اكسر;
	حالة K_F9:		name = "F9";		اكسر;
	حالة K_S_F9:	name = "F9";	سرڪايو = 1;	اكسر;
	حالة K_F10:		name = "F10";		اكسر;
	حالة K_S_F10:	name = "F10";	سرڪايو = 1;	اكسر;
	حالة K_F11:		name = "F11";		اكسر;
	حالة K_S_F11:	name = "F11";	سرڪايو = 1;	اكسر;
	حالة K_F12:		name = "F12";		اكسر;
	حالة K_S_F12:	name = "F12";	سرڪايو = 1;	اكسر;
	افتراضي:
			إذا (ڪنجي >= ' ' && ڪنجي <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = ڪنجي;
			    namebuf[1] = NUL;
			}
			وإلا
			    name = "X";
			اكسر;
    }

    buf[0] = '\0';
    إذا (ctrl)
	strcat(buf, "C");
    إذا (سرڪايو)
	strcat(buf, "S");
    إذا (alt)
	strcat(buf, "M"); // META
    إذا (ctrl || سرڪايو || alt)
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
	صحيح		 state اڻ_استعمال_ٿيل)
{
    win_T	*wp;
    char_u	*متن;
    linenr_T	lnum;
    صحيح		کانو;
    حرف	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    إذا (!can_use_beval() || !NETBEANS_OPEN)
	أرجع;

    إذا (get_beval_info(beval, TRUE, &wp, &lnum, &متن, &کانو) != OK)
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
#جيڪڏهن_پورو

/*
 * Return TRUE when the netbeans connection is active.
 */
    صحيح
netbeans_سرگرم(فراغ)
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
    بدليو_دري_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    ٻاهريون_flush_cursor(TRUE, FALSE);
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
سيٽ_ref_اندريون_nb_channel(صحيح copyID)
{
    صحيح abort = FALSE;
    typval_T tv;

    إذا (nb_channel == عدم)
	أرجع FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = سيٽ_ref_اندريون_شي(&tv, copyID, عدم, عدم, عدم);
    أرجع abort;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    فراغ
netbeans_چوکٺو_moved(صحيح new_x, صحيح new_y)
{
    حرف buf[128];

    إذا (!NETBEANS_OPEN)
	أرجع;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (صحيح)Columns, (صحيح)صفون, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#جيڪڏهن_پورو

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
	آخري_chdir_reason = "netbeans";
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
    colnr_T	کانو,
    char_u	*txt,
    صحيح		newlen)
{
    char_u	*buf;
    صحيح		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    ڊگهو	off;
    char_u	*p;
    char_u	*newtxt;

    إذا (!NETBEANS_OPEN)
	أرجع;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    إذا (nbbuf == عدم)
	أرجع;

    // Don't mark as modified for initial read
    إذا (nbbuf->وجهو_پورو)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.کانو = کانو;
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
    colnr_T	کانو,
    ڊگهو	len)
{
    char_u	buf[128];
    صحيح		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    ڊگهو	off;

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
    pos.کانو = کانو;

    off = pos2off(bufp, &pos);

    sprintf((حرف *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((حرف *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    فراغ
netbeans_unmodified(buf_T *bufp اڻ_استعمال_ٿيل)
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

    صحيح کانو = mouse_کانو - curwin->w_wincol
	- ((curwin->w_p_nu || curwin->w_p_rnu) ? 9 : 1);
    ڊگهو off = pos2off(curbuf, &curwin->w_cursor);

    // sync the cursor position
    sprintf(buf, "%d:newDotAndMark=%d %ld %ld\n", bufno, r_cmdno, off, off);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release[newDotAndMark]");

    sprintf(buf, "%d:buttonRelease=%d %d %ld %d\n", bufno, r_cmdno,
	    button, (ڊگهو)curwin->w_cursor.lnum, کانو);
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
netbeans_keycommand(صحيح ڪنجي)
{
    حرف	ڪنجي_Name[60];

    netbeans_keyname(ڪنجي, ڪنجي_Name);
    أرجع netbeans_keystring((char_u *)ڪنجي_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    ساكن صحيح
netbeans_keystring(char_u *ڪنجي_Name)
{
    حرف	buf[2*MAXPATHL];
    صحيح		bufno = nb_getbufno(curbuf);
    ڊگهو	off;
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

	postpone_keycommand(ڪنجي_Name);
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
						     bufno, r_cmdno, ڪنجي_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, حجم(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, ڪنجي_Name,
		off, (ڊگهو)curwin->w_cursor.lnum, (ڊگهو)curwin->w_cursor.کانو);
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
netbeans_deleted_all_سٽون(buf_T *bufp)
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
    إذا (nbbuf->وجهو_پورو)
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
netbeans_is_guarded(linenr_T چوٽي, linenr_T bot)
{
    sign_داخلا_T	*p;
    صحيح			lnum;

    إذا (!NETBEANS_OPEN)
	أرجع FALSE;

    FOR_ALL_SIGNS_اندريون_BUF(curbuf, p)
	إذا (p->se_id >= GUARDEDOFFSET)
	    لكل (lnum = چوٽي + 1; lnum < bot; lnum++)
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
netbeans_draw_multisign_indicator(صحيح صف)
{
    صحيح i;
    صحيح y;
    صحيح x;

    إذا (!NETBEANS_OPEN)
	أرجع;

    x = 0;
    y = صف * gui.char_اوچائي + 2;

    لكل (i = 0; i < gui.char_اوچائي - 3; i++)
	XDraw_نقطو(gui.dpy, gui.wid, gui.متن_gc, x+2, y++);

    XDraw_نقطو(gui.dpy, gui.wid, gui.متن_gc, x+0, y);
    XDraw_نقطو(gui.dpy, gui.wid, gui.متن_gc, x+2, y);
    XDraw_نقطو(gui.dpy, gui.wid, gui.متن_gc, x+4, y++);
    XDraw_نقطو(gui.dpy, gui.wid, gui.متن_gc, x+1, y);
    XDraw_نقطو(gui.dpy, gui.wid, gui.متن_gc, x+2, y);
    XDraw_نقطو(gui.dpy, gui.wid, gui.متن_gc, x+3, y++);
    XDraw_نقطو(gui.dpy, gui.wid, gui.متن_gc, x+2, y);
}
#جيڪڏهن_پورو // FEAT_GUI_X11

#إذا defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    فراغ
netbeans_draw_multisign_indicator(صحيح صف)
{
    صحيح i;
    صحيح y;
    صحيح x;
# إذا GTK_CHECK_نسخو(3,0,0)
    cairo_t *cr = عدم;
# وإلا
    GdkDrawable *drawable = gui.drawarea->دري_2;
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
    y = صف * gui.char_اوچائي + 2;
# endif

# إذا GTK_CHECK_نسخو(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_اوچائي);
    cr = cairo_create(surf);
#  وإلا
    cr = cairo_create(gui.surface);
#  endif
    cairo_سيٽ_ذريعو_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    لكل (i = 0; i < gui.char_اوچائي - 3; i++)
# إذا GTK_CHECK_نسخو(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# وإلا
    gdk_draw_نقطو(drawable, gui.متن_gc, x+2, y++);
# endif

# إذا GTK_CHECK_نسخو(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# وإلا
    gdk_draw_نقطو(drawable, gui.متن_gc, x+0, y);
    gdk_draw_نقطو(drawable, gui.متن_gc, x+2, y);
    gdk_draw_نقطو(drawable, gui.متن_gc, x+4, y++);
    gdk_draw_نقطو(drawable, gui.متن_gc, x+1, y);
    gdk_draw_نقطو(drawable, gui.متن_gc, x+2, y);
    gdk_draw_نقطو(drawable, gui.متن_gc, x+3, y++);
    gdk_draw_نقطو(drawable, gui.متن_gc, x+2, y);
# endif

# ifdef USE_GTK4
    cairo_fill(cr);
    gui_gtk4_جوڙيو_multisign(surf, صف, 0, 5, gui.char_اوچائي);
# endif

# إذا GTK_CHECK_نسخو(3,0,0)
    cairo_destroy(cr);
# endif
# ifdef USE_GTK4
    cairo_surface_destroy(surf);
# endif
}
#جيڪڏهن_پورو // FEAT_GUI_GTK

/*
 * If the mouse is clicked in the gutter of a line with multiple
 * annotations, cycle through the set of signs.
 */
    فراغ
netbeans_gutter_click(linenr_T lnum)
{
    sign_داخلا_T	*p;

    إذا (!NETBEANS_OPEN)
	أرجع;

    FOR_ALL_SIGNS_اندريون_BUF(curbuf, p)
    {
	إذا (p->se_lnum == lnum && p->se_ايندڙ && p->se_ايندڙ->se_lnum == lnum)
	{
	    sign_داخلا_T *پڇ;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    إذا (p->se_prev)
		p->se_prev->se_ايندڙ = p->se_ايندڙ;
	    وإلا
		curbuf->b_signlist = p->se_ايندڙ;
	    p->se_ايندڙ->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    لكل (پڇ = p->se_ايندڙ;
		  پڇ->se_ايندڙ && پڇ->se_ايندڙ->se_lnum == lnum
				       && پڇ->se_ايندڙ->se_id < GUARDEDOFFSET;
		  پڇ = پڇ->se_ايندڙ)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_ايندڙ = پڇ->se_ايندڙ;
	    إذا (پڇ->se_ايندڙ)
		پڇ->se_ايندڙ->se_prev = p;
	    p->se_prev = پڇ;
	    پڇ->se_ايندڙ = p;
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
    صحيح		type_انگ,
    char_u	*typeName,
    char_u	*tooltip اڻ_استعمال_ٿيل,
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
			    type_انگ, typeName, tooltip, glyphFile, fg, bg));
	إذا (use_fg || use_bg)
	{
	    حرف fgbuf[2 * (8 + MAX_COLOR_ڊيگهائي) + 1];
	    حرف bgbuf[2 * (8 + MAX_COLOR_ڊيگهائي) + 1];
	    حرف *ptr;
	    صحيح قدر;

	    قدر = strtol((حرف *)fg, &ptr, 10);
	    إذا (ptr != (حرف *)fg)
		sprintf(fgbuf, "guifg=#%06x", قدر & 0xFFFFFF);
	    وإلا
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    قدر = strtol((حرف *)bg, &ptr, 10);
	    إذا (ptr != (حرف *)bg)
		sprintf(bgbuf, "guibg=#%06x", قدر & 0xFFFFFF);
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
	    curPCtype = type_انگ;

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
    ساكن ڊگهو
get_buf_سائيز(buf_T *bufp)
{
    linenr_T	lnum;
    ڊگهو	char_ڳڻپ = 0;
    صحيح		eol_سائيز;
    ڊگهو	آخري_check = 100000L;

    إذا (bufp->b_ml.ml_flags & ML_خالي_جايون)
	أرجع 0;

    إذا (get_fileformat(bufp) == EOL_DOS)
	eol_سائيز = 2;
    وإلا
	eol_سائيز = 1;
    لكل (lnum = 1; lnum <= bufp->b_ml.ml_سٽ_ڳڻپ; ++lnum)
    {
	char_ڳڻپ += ml_get_buf_len(bufp, lnum) + eol_سائيز;
	// Check for a CTRL-C every 100000 characters
	إذا (char_ڳڻپ > آخري_check)
	{
	    ui_breakcheck();
	    إذا (got_int)
		أرجع char_ڳڻپ;
	    آخري_check = char_ڳڻپ + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    إذا (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_ڳڻپ -= eol_سائيز;

    أرجع char_ڳڻپ;
}

/*
 * Convert character offset to lnum,col
 */
    ساكن pos_T *
off2pos(buf_T *buf, ڊگهو وٿي)
{
    linenr_T	 lnum;
    ساكن pos_T pos;

    pos.lnum = 0;
    pos.کانو = 0;
    pos.coladd = 0;

    إذا (!(buf->b_ml.ml_flags & ML_خالي_جايون))
    {
	إذا ((lnum = ml_ڳوليو_سٽ_or_وٿي(buf, (linenr_T)0, &وٿي)) < 0)
	    أرجع عدم;
	pos.lnum = lnum;
	pos.کانو = وٿي;
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
    ڊگهو		off;

    off = strtol((حرف *)*argp, (حرف **)argp, 10);
    إذا (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.کانو = strtol((حرف *)*argp, (حرف **)argp, 10);
	mypos.coladd = 0;
	أرجع &mypos;
    }
    أرجع off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    ساكن ڊگهو
pos2off(buf_T *buf, pos_T *pos)
{
    ڊگهو	 وٿي = 0;

    إذا (buf->b_ml.ml_flags & ML_خالي_جايون)
	أرجع 0;

    إذا ((وٿي = ml_ڳوليو_سٽ_or_وٿي(buf, pos->lnum, 0)) < 0)
	أرجع 0;
    وٿي += pos->کانو;

    أرجع وٿي;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    ساكن فراغ
print_read_نياپو(nbbuf_T *buf)
{
    صحيح	    lnum = buf->bufp->b_ml.ml_سٽ_ڳڻپ;
    off_T   nchars = buf->bufp->b_orig_سائيز;
    char_u  c;

    نياپو_جوڙيو_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    إذا (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    إذا (!buf->bufp->b_شروعات_eol)
    {
	STRCAT(IObuff, shortmess(SHM_آخري) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    نياپو_جوڙيو_سٽون(c, (ڊگهو)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_نياپو);
    نياپو_scrolled_ign = TRUE;
    نياپو_trunc_attr((حرف *)IObuff, FALSE, 0);
    نياپو_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    ساكن فراغ
print_save_نياپو(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    إذا (nchars >= 0)
    {
	// put fname in IObuff with quotes
	نياپو_جوڙيو_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	نياپو_جوڙيو_سٽون(c, buf->bufp->b_ml.ml_سٽ_ڳڻپ,
						buf->bufp->b_orig_سائيز);

	VIM_CLEAR(keep_نياپو);
	نياپو_scrolled_ign = TRUE;
	p = (char_u *)نياپو_trunc_attr((حرف *)IObuff, FALSE, 0);
	إذا ((نياپو_scrolled && !need_wait_return) || !buf->init_پورو)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    سيٽ_keep_نياپو(p, 0);
	}
	نياپو_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    وإلا
    {
	حرف msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_جوڙيو_bang_منزل_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#جيڪڏهن_پورو // defined(FEAT_NETBEANS_INTG)
