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

#شامل "vim.h"

#اگر defined(FEAT_NETBEANS_INTG)

#ifndef MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#endif

#شامل "version.h"

#define GUARDED		10000 // typenr for "guarded" annotation
#define GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#define MAX_COLOR_LENGTH 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#define VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#define VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
ایستا نویسه *ExtEdProtocolVersion = "2.5";

ایستا long pos2off(buf_T *, pos_T *);
ایستا pos_T *off2pos(buf_T *, long);
ایستا pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
ایستا long get_buf_size(buf_T *);
ایستا صحیح netbeans_keystring(char_u *keystr);
ایستا تهی special_کلیدها(char_u *args);

ایستا صحیح getConnInfo(نویسه *file, نویسه **host, نویسه **port, نویسه **password);

ایستا تهی nb_init_graphics(تهی);
ایستا تهی coloncmd(نویسه *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ایستا تهی nb_set_curbuf(buf_T *buf);
ایستا تهی nb_parse_cmd(char_u *);
ایستا صحیح  nb_do_cmd(صحیح, char_u *, صحیح, صحیح, char_u *);
ایستا تهی nb_send(نویسه *buf, نویسه *fun);
ایستا تهی nb_free(تهی);

#define NETBEANS_OPEN (channel_can_write_to(nb_channel))
ایستا channel_T *nb_channel = پوچ;

ایستا صحیح r_cmdno;			// current command number for reply
ایستا صحیح dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ifdef NBDEBUG
# include "nbdebug.c"
#endif

ایستا صحیح needupdate = 0;
ایستا صحیح inAtomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    ایستا صحیح
nb_is_safe_string(char_u *str, char_u *allowed)
{
    اگر (str == پوچ)
	بازگشت FALSE;
    برای (char_u *p = str; *p != NUL; p++)
	اگر (vim_strchr(allowed, *p) == پوچ)
	    بازگشت FALSE;
    بازگشت TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    ایستا تهی
nb_channel_closed(تهی)
{
    nb_channel = پوچ;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    ایستا تهی
netbeans_close(تهی)
{
    اگر (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	اگر (nb_channel != پوچ)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = پوچ;
    }

#ifdef FEAT_BEVAL_GUI
    bevalServers &= ~BEVAL_NETBEANS;
#endif

    needupdate = 0;
    inAtomic = 0;
    nb_free();

    // remove all signs and update the screen after gutter removal
    coloncmd(":sign unplace *");
    changed_window_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    out_flush_cursor(TRUE, FALSE);
}

#define NB_DEF_HOST "localhost"
#define NB_DEF_ADDR "3219"
#define NB_DEF_PASS "changeme"

    ایستا صحیح
netbeans_connect(نویسه *params, صحیح doabort)
{
    صحیح		port;
    نویسه	buf[32];
    نویسه	*hostname = پوچ;
    نویسه	*address = پوچ;
    نویسه	*password = پوچ;
    نویسه	*fname;
    نویسه	*arg = پوچ;

    اگر (*params == '=')
    {
	// "=fname": Read info from specified file.
	اگر (getConnInfo(params + 1, &hostname, &address, &password) == FAIL)
	    بازگشت FAIL;
    }
    وگرنه
    {
	اگر (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	اگر (arg == پوچ && (fname = getenv("__NETBEANS_CONINFO")) != پوچ)
	{
	    // "": get info from file specified in environment
	    اگر (getConnInfo(fname, &hostname, &address, &password) == FAIL)
		بازگشت FAIL;
	}
	وگرنه
	{
	    اگر (arg != پوچ)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		address = strchr(hostname, ':');
		اگر (address != پوچ)
		{
		    *address++ = '\0';
		    password = strchr(address, ':');
		    اگر (password != پوچ)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    اگر (hostname == پوچ || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    اگر (address == پوچ)
		address = getenv("__NETBEANS_SOCKET");
	    اگر (password == پوچ)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    اگر (hostname != پوچ)
		hostname = (نویسه *)vim_strsave((char_u *)hostname);
	    اگر (address != پوچ)
		address = (نویسه *)vim_strsave((char_u *)address);
	    اگر (password != پوچ)
		password = (نویسه *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    اگر (hostname == پوچ || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (نویسه *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    اگر (address == پوچ || *address == '\0')
    {
	vim_free(address);
	address = (نویسه *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    اگر (password == پوچ || *password == '\0')
    {
	vim_free(password);
	password = (نویسه *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    اگر (hostname != پوچ && address != پوچ && password != پوچ)
    {
	port = atoi(address);
	nb_channel = channel_open(hostname, port, 3000, nb_channel_closed);
	اگر (nb_channel != پوچ)
	{
	    // success
#ifdef FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#endif

	    // success, login
	    vim_snprintf(buf, اندازه(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", ExtEdProtocolVersion);
	    nb_send(buf, "externaleditor_version");
	}
    }

    اگر (nb_channel == پوچ && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(address);
    vim_free(password);
    بازگشت NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    ایستا صحیح
getConnInfo(نویسه *file, نویسه **host, نویسه **port, نویسه **auth)
{
    FILE *fp;
    char_u buf[BUFSIZ];
    char_u *lp;
    char_u *nlp;
#ifdef UNIX
    stat_T	st;

    /*
     * For Unix only accept the file when it's not accessible by others.
     * The open will then fail if we don't own the file.
     */
    اگر (mch_stat(file, &st) == 0 && (st.st_mode & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_mode_for_netbeans_connection_info_file_str),
									file);
	بازگشت FAIL;
    }
#endif

    fp = mch_fopen(file, "r");
    اگر (fp == پوچ)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	بازگشت FAIL;
    }

    // Read the file. There should be one of each parameter
    وقتی ((lp = (char_u *)fgets((نویسه *)buf, BUFSIZ, fp)) != پوچ)
    {
	اگر ((nlp = vim_strchr(lp, '\n')) != پوچ)
	    *nlp = 0;	    // strip off the trailing newline

	اگر (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (نویسه *)vim_strsave(&buf[5]);
	}
	وگرنه اگر (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*port);
	    *port = (نویسه *)vim_strsave(&buf[5]);
	}
	وگرنه اگر (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (نویسه *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    بازگشت OK;
}


ساختار keyqueue
{
    char_u	    *keystr;
    ساختار keyqueue *next;
    ساختار keyqueue *prev;
};

تعریفنوع ساختار keyqueue کلید_Q_T;

ایستا کلید_Q_T کلید_Head; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    ایستا تهی
postpone_keycommand(char_u *keystr)
{
    کلید_Q_T *گره;

    گره = ALLOC_ONE(کلید_Q_T);
    اگر (گره == پوچ)
	بازگشت;  // out of memory, drop the key

    اگر (کلید_Head.next == پوچ) // initialize circular queue
    {
	کلید_Head.next = &کلید_Head;
	کلید_Head.prev = &کلید_Head;
    }

    // insert node at tail of queue
    گره->next = &کلید_Head;
    گره->prev = کلید_Head.prev;
    کلید_Head.prev->next = گره;
    کلید_Head.prev = گره;

    گره->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    ایستا تهی
handle_کلید_queue(تهی)
{
    صحیح postponed = FALSE;

    وقتی (!postponed && کلید_Head.next && کلید_Head.next != &کلید_Head)
    {
	// first, unlink the node
	کلید_Q_T *گره = کلید_Head.next;
	کلید_Head.next = گره->next;
	گره->next->prev = گره->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	اگر (گره->keystr != پوچ)
	    postponed = !netbeans_keystring(گره->keystr);
	vim_free(گره->keystr);

	// Finally, dispose of the node
	vim_free(گره);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    تهی
netbeans_parse_messages(تهی)
{
    readq_T	*گره;
    char_u	*buffer;
    char_u	*p;
    صحیح		own_گره;

    وقتی (nb_channel != پوچ)
    {
	گره = channel_peek(nb_channel, PART_SOCK);
	اگر (گره == پوچ)
	    بشکن;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_first_nl(گره);
	اگر (p == پوچ)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    اگر (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		بازگشت;
	    ادامه;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	اگر (*p == NUL)
	{
	    own_گره = TRUE;
	    buffer = channel_get(nb_channel, PART_SOCK, پوچ);
	    // "node" is now invalid!
	}
	وگرنه
	{
	    own_گره = FALSE;
	    buffer = گره->rq_buffer;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(buffer);

	اگر (own_گره)
	    // buffer finished, dispose of it
	    vim_free(buffer);
	وگرنه اگر (nb_channel != پوچ)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (صحیح)(p - buffer));
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
    ایستا تهی
nb_parse_cmd(char_u *cmd)
{
    نویسه	*verb;
    نویسه	*q;
    صحیح		bufno;
    صحیح		isfunc = -1;

    اگر (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    اگر (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_column = false;

	// The IDE is breaking the connection.
	netbeans_close();
	بازگشت;
    }

    bufno = strtol((نویسه *)cmd, &verb, 10);

    اگر (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	بازگشت;
    }
    ++verb; // skip colon

    برای (q = verb; *q; q++)
    {
	اگر (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    بشکن;
	}
	وگرنه اگر (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    بشکن;
	}
    }

    اگر (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_in_str), cmd);
	بازگشت;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (نویسه *)skipwhite((char_u *)q);

    اگر (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
    {
#ifdef NBDEBUG
	/*
	 * This happens because the ExtEd can send a command or 2 after
	 * doing a stopDocumentListen command. It doesn't harm anything
	 * so I'm disabling it except for debugging.
	 */
	nbdebug(("nb_parse_cmd: Command error for \"%s\"\n", cmd));
	emsg(e_bad_return_from_nb_do_cmd);
#endif
    }
}

ساختار nbbuf_struct
{
    buf_T		*bufp;
    unsigned صحیح	 fireChanges:1;
    unsigned صحیح	 initDone:1;
    unsigned صحیح	 درج_Done:1;
    unsigned صحیح	 modified:1;
    صحیح			 nbbuf_عدد;
    نویسه		*displayname;
    صحیح			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

تعریفنوع ساختار nbbuf_struct nbbuf_T;

ایستا nbbuf_T *buf_list = پوچ;
ایستا صحیح buf_list_size = 0;	// size of buf_list
ایستا صحیح buf_list_used = 0;	// nr of entries in buf_list actually in use

ایستا نویسه **globalsignmap = پوچ;
ایستا صحیح globalsignmaplen = 0;
ایستا صحیح globalsignmapused = 0;

ایستا صحیح  mapsigntype(nbbuf_T *, صحیح localsigntype);
ایستا تهی addsigntype(nbbuf_T *, صحیح localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
ایستا تهی print_read_پیام(nbbuf_T *buf);
ایستا تهی print_save_پیام(nbbuf_T *buf, off_T nchars);

ایستا صحیح curPCtype = -1;

/*
 * Free netbeans resources.
 */
    ایستا تهی
nb_free(تهی)
{
    کلید_Q_T *کلید_گره = کلید_Head.next;
    nbbuf_T buf;
    صحیح i;

    // free the netbeans buffer list
    برای (i = 0; i < buf_list_used; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	اگر (buf.bufp != پوچ && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_size = 0;
    buf_list_used = 0;

    // free the queued key commands
    وقتی (کلید_گره != پوچ && کلید_گره != &کلید_Head)
    {
	کلید_Q_T *next = کلید_گره->next;
	vim_free(کلید_گره->keystr);
	vim_free(کلید_گره);
	اگر (next == &کلید_Head)
	{
	    کلید_Head.next = &کلید_Head;
	    کلید_Head.prev = &کلید_Head;
	    بشکن;
	}
	کلید_گره = next;
    }

    // free the queued netbeans commands
    اگر (nb_channel != پوچ)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    ایستا صحیح
nb_getbufno(buf_T *bufp)
{
    صحیح i;

    برای (i = 0; i < buf_list_used; i++)
	اگر (buf_list[i].bufp == bufp)
	    بازگشت i;
    بازگشت -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    صحیح
isNetbeansBuffer(buf_T *bufp)
{
    بازگشت NETBEANS_OPEN && bufp->b_netbeans_file;
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
    صحیح
isNetbeansModified(buf_T *bufp)
{
    اگر (isNetbeansBuffer(bufp))
    {
	صحیح bufno = nb_getbufno(bufp);

	اگر (bufno > 0)
	    بازگشت buf_list[bufno].modified;
	وگرنه
	    بازگشت FALSE;
    }
    وگرنه
	بازگشت FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    ایستا nbbuf_T *
nb_get_buf(صحیح bufno)
{
    // find or create a buffer with the given number
    صحیح incr;

    اگر (bufno <= 0)
	بازگشت پوچ;

    اگر (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * اندازه(nbbuf_T));
	buf_list_size = 100;
    }
    اگر (bufno >= buf_list_used) // new
    {
	اگر (bufno >= buf_list_size) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_size + 90;
	    buf_list_size += incr;
	    bufsize = buf_list_size * اندازه(nbbuf_T);
	    اگر (bufsize == 0 || bufsize / اندازه(nbbuf_T)
						      != (size_t)buf_list_size)
	    {
		// list size overflow, bail out
		بازگشت پوچ;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    اگر (buf_list == پوچ)
	    {
		vim_free(t_buf_list);
		buf_list_size = 0;
		بازگشت پوچ;
	    }
	    vim_memset(buf_list + buf_list_size - incr, 0,
						      incr * اندازه(nbbuf_T));
	}

	وقتی (buf_list_used <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_used].fireChanges = 1;
	    ++buf_list_used;
	}
    }

    بازگشت buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    ایستا صحیح
شمار_changed_buffers(تهی)
{
    buf_T	*bufp;
    صحیح		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	اگر (bufp->b_changed)
	    ++n;
    بازگشت n;
}

/*
 * End the netbeans session.
 */
    تهی
netbeans_end(تهی)
{
    صحیح i;
    ایستا نویسه buf[128];

    اگر (!NETBEANS_OPEN)
	بازگشت;

    برای (i = 0; i < buf_list_used; i++)
    {
	اگر (!buf_list[i].bufp)
	    ادامه;
	اگر (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, پوچ);
	buf_list[i].bufp = پوچ;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    ایستا تهی
nb_send(نویسه *buf, نویسه *fun)
{
    اگر (nb_channel != پوچ)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (صحیح)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    ایستا تهی
nb_reply_nil(صحیح cmdno)
{
    نویسه reply[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    اگر (!NETBEANS_OPEN)
	بازگشت;

    sprintf(reply, "%d\n", cmdno);
    nb_send(reply, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    ایستا تهی
nb_reply_text(صحیح cmdno, char_u *result)
{
    char_u *reply;

    nbdebug(("REP %d: %s\n", cmdno, (نویسه *)result));

    reply = alloc(STRLEN(result) + 32);
    sprintf((نویسه *)reply, "%d %s\n", cmdno, (نویسه *)result);
    nb_send((نویسه *)reply, "nb_reply_text");

    vim_free(reply);
}


/*
 * Send a response with a number result code.
 */
    ایستا تهی
nb_reply_nr(صحیح cmdno, long result)
{
    نویسه reply[32];

    nbdebug(("REP %d: %ld\n", cmdno, result));

    sprintf(reply, "%d %ld\n", cmdno, result);
    nb_send(reply, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    ایستا char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    اگر (buf == پوچ)
	بازگشت پوچ;
    برای (; *p; p++)
    {
	انتخاب (*p)
	{
	    حالت '\"':
	    حالت '\\':
		*q++ = '\\'; *q++ = *p; بشکن;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    حالت '\n':
		*q++ = '\\'; *q++ = 'n'; بشکن;
	    حالت '\r':
		*q++ = '\\'; *q++ = 'r'; بشکن;
	    پیشفرض:
		*q++ = *p;
		بشکن;
	}
    }
    *q = '\0';

    بازگشت buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    ایستا نویسه *
nb_unquote(char_u *p, char_u **endp)
{
    نویسه *result = 0;
    نویسه *q;
    صحیح done = 0;

    // result is never longer than input
    result = alloc_clear(STRLEN(p) + 1);
    اگر (result == پوچ)
	بازگشت پوچ;

    اگر (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	result[0] = NUL;
	بازگشت result;
    }

    برای (q = result; !done && *p != NUL;)
    {
	انتخاب (*p)
	{
	    حالت '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		done = 1;
		بشکن;

	    حالت '\\':
		++p;
		انتخاب (*p)
		{
		    حالت '\\':	*q++ = '\\';	بشکن;
		    حالت 'n':	*q++ = '\n';	بشکن;
		    حالت 't':	*q++ = '\t';	بشکن;
		    حالت 'r':	*q++ = '\r';	بشکن;
		    حالت '"':	*q++ = '"';	بشکن;
		    حالت NUL:	--p;		بشکن;
		    // default: skip over illegal chars
		}
		++p;
		بشکن;

	    پیشفرض:
		*q++ = *p++;
	}
    }

    اگر (endp != پوچ)
	*endp = p;

    بازگشت result;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    ایستا تهی
nb_partialremove(linenr_T lnum, colnr_T first, colnr_T last)
{
    char_u *oldtext, *newtext;
    صحیح oldlen;
    صحیح lastbyte = last;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    اگر (first >= (colnr_T)oldlen || oldlen == 0)  // just in case
	بازگشت;
    اگر (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - first));
    اگر (newtext == پوچ)
	بازگشت;

    mch_memmove(newtext, oldtext, first);
    STRMOVE(newtext + first, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    ایستا تهی
nb_joinlines(linenr_T first, linenr_T other)
{
    صحیح len_first, len_other;
    char_u *p;

    len_first = ml_get_len(first);
    len_other = ml_get_len(other);
    p = alloc(len_first + len_other + 1);
    اگر (p == پوچ)
	بازگشت;

    mch_memmove(p, ml_get(first), len_first);
    mch_memmove(p + len_first, ml_get(other), len_other + 1);
    ml_replace(first, p, FALSE);
}

#define SKIP_STOP 2
#define streq(a,b) (strcmp(a,b) == 0)

/*
 * Do the actual processing of a single netbeans command or function.
 * The difference between a command and function is that a function
 * gets a response (it's required) but a command does not.
 * For arguments see comment for nb_parse_cmd().
 */
    ایستا صحیح
nb_do_cmd(
    صحیح		bufno,
    char_u	*cmd,
    صحیح		func,
    صحیح		cmdno,
    char_u	*args)	    // points to space before arguments or NUL
{
    صحیح		do_update = 0;
    long	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    ایستا صحیح	skip = 0;
    صحیح		retval = OK;
    نویسه	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (نویسه *)args));

    اگر (func)
    {
// =====================================================================
	اگر (streq((نویسه *)cmd, "getModified"))
	{
	    اگر (buf == پوچ || buf->bufp == پوچ)
		// Return the number of buffers that are modified.
		nb_reply_nr(cmdno, (long)شمار_changed_buffers());
	    وگرنه
		// Return whether the buffer is modified.
		nb_reply_nr(cmdno, (long)(buf->bufp->b_changed
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_reply_nr(cmdno, (long)شمار_changed_buffers());
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "getCursor"))
	{
	    char_u text[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((نویسه *)text, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (long)curwin->w_cursor.lnum,
		    (صحیح)curwin->w_cursor.col,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_reply_text(cmdno, text);
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "getAnno"))
	{
	    long linenum = 0;
#ifdef FEAT_SIGNS
	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_getanno));
		retval = FAIL;
	    }
	    وگرنه
	    {
		صحیح ser_رقم;

		cp = (نویسه *)args;
		ser_رقم = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (long)buf_findsign(buf->bufp, ser_رقم, پوچ);
	    }
#endif
	    nb_reply_nr(cmdno, linenum);
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "getLength"))
	{
	    long len = 0;

	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_buffer_identifier_in_getlength));
		retval = FAIL;
	    }
	    وگرنه
	    {
		len = get_buf_size(buf->bufp);
	    }
	    nb_reply_nr(cmdno, len);
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "getText"))
	{
	    long	len;
	    linenr_T	nlines;
	    char_u	*text = پوچ;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*سطر;

	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_buffer_identifier_in_gettext));
		retval = FAIL;
	    }
	    وگرنه
	    {
		len = get_buf_size(buf->bufp);
		nlines = buf->bufp->b_ml.ml_سطر_شمار;
		text = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		اگر (text == پوچ)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		وگرنه
		{
		    p = text;
		    *p++ = '\"';
		    برای (; lno <= nlines ; lno++)
		    {
			سطر = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			اگر (سطر != پوچ)
			{
			    STRCPY(p, سطر);
			    p += STRLEN(سطر);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(سطر);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    اگر (text == پوچ)
		nb_reply_text(cmdno, (char_u *)"");
	    وگرنه
	    {
		nb_reply_text(cmdno, text);
		vim_free(text);
	    }
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "remove"))
	{
	    long شمار;
	    pos_T first, last;
	    pos_T *pos;
	    pos_T *next;
	    linenr_T del_from_lnum, del_to_lnum;  // lines to be deleted as a whole
	    صحیح oldFire = netbeansFireChanges;
	    صحیح oldSuppress = netbeans_Suppress_No_سطرها;
	    صحیح wasChanged;

	    اگر (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (نویسه *) cmd));
		nb_reply_nil(cmdno);
		بازگشت OK;
	    }

	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_buffer_identifier_in_remove));
		retval = FAIL;
	    }
	    وگرنه
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_سطرها = TRUE;

		nb_set_curbuf(buf->bufp);
		wasChanged = buf->bufp->b_changed;
		cp = (نویسه *)args;
		off = strtol(cp, &cp, 10);
		شمار = strtol(cp, &cp, 10);
		args = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		اگر (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_سطرها = oldSuppress;
		    بازگشت FAIL;
		}
		first = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      first.lnum, first.col));
		pos = off2pos(buf->bufp, off+شمار-1);
		اگر (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_سطرها = oldSuppress;
		    بازگشت FAIL;
		}
		last = *pos;
		nbdebug(("    LAST POS: line %ld, col %d\n",
							last.lnum, last.col));
		del_from_lnum = first.lnum;
		del_to_lnum = last.lnum;
		do_update = 1;

		// Get the position of the first byte after the deleted
		// section.  "next" is NULL when deleting to the end of the
		// file.
		next = off2pos(buf->bufp, off + شمار);

		// Remove part of the first line.
		اگر (first.col != 0
				|| (next != پوچ && first.lnum == next->lnum))
		{
		    اگر (first.lnum != last.lnum
			    || (next != پوچ && first.lnum != next->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(first.lnum, first.col,
							     (colnr_T)MAXCOL);
			اگر (first.lnum == last.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(first.lnum, next->lnum);
			    del_to_lnum = next->lnum;
			}
		    }
		    وگرنه
		    {
			// remove within one line
			nb_partialremove(first.lnum, first.col, last.col);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		اگر (first.lnum != last.lnum && next != پوچ
			&& next->col != 0 && last.lnum == next->lnum)
		{
		    nb_partialremove(last.lnum, 0, last.col);
		    اگر (del_from_lnum > first.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(first.lnum, last.lnum);
		    }
		    وگرنه
			// First line is deleted as a whole, keep the last
			// line.
			--del_to_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		اگر (first.lnum != last.lnum && del_from_lnum > first.lnum
			&& next != پوچ && last.lnum != next->lnum)
		{
		    nb_joinlines(first.lnum, next->lnum);
		    del_to_lnum = next->lnum;
		}

		// Delete whole lines if there are any.
		اگر (del_to_lnum >= del_from_lnum)
		{
		    صحیح i;

		    // delete signs from the lines being deleted
		    برای (i = del_from_lnum; i <= del_to_lnum; i++)
		    {
			صحیح id = buf_findsign_id(buf->bufp, (linenr_T)i, پوچ);
			اگر (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, پوچ);
			}
			وگرنه
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_to_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.col = 0;
		    del_سطرها(del_to_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = first;
		check_cursor_lnum();
		buf->bufp->b_changed = wasChanged; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_سطرها = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_reply_nil(cmdno);
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "insert"))
	{
	    char_u	*to_free;

	    اگر (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (نویسه *) cmd));
		nb_reply_nil(cmdno);
		بازگشت OK;
	    }

	    // get offset
	    cp = (نویسه *)args;
	    off = strtol(cp, &cp, 10);
	    args = (char_u *)cp;

	    // get text to be inserted
	    args = skipwhite(args);
	    args = to_free = (char_u *)nb_unquote(args, پوچ);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_buffer_identifier_in_درج));
		retval = FAIL;
	    }
	    وگرنه اگر (args != پوچ)
	    {
		صحیح	ff_detected = EOL_UNKNOWN;
		صحیح	buf_was_empty = (buf->bufp->b_ml.ml_flags & ML_EMPTY);
		size_t	len = 0;
		صحیح	added = 0;
		صحیح	oldFire = netbeansFireChanges;
		صحیح	old_b_changed;
		char_u	*nlp;
		linenr_T lnum;
		linenr_T lnum_start;
		pos_T	*pos;

		netbeansFireChanges = 0;

		// Jump to the buffer where we insert.  After this "curbuf"
		// can be used.
		nb_set_curbuf(buf->bufp);
		old_b_changed = curbuf->b_changed;

		// Convert the specified character offset into a lnum/col
		// position.
		pos = off2pos(curbuf, off);
		اگر (pos != پوچ)
		{
		    اگر (pos->lnum <= 0)
			lnum_start = 1;
		    وگرنه
			lnum_start = pos->lnum;
		}
		وگرنه
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    اگر (buf_was_empty)
			lnum_start = 1;	    // above empty line
		    وگرنه
			lnum_start = curbuf->b_ml.ml_سطر_شمار + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_start;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		وقتی (*args != NUL)
		{
		    nlp = vim_strchr(args, '\n');
		    اگر (nlp == پوچ)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(args);
		    }
		    وگرنه
		    {
			len = nlp - args;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			اگر (nlp > args && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			وگرنه
			    ff_detected = EOL_UNIX;
		    }
		    args[len] = NUL;

		    اگر (lnum == lnum_start
			    && ((pos != پوچ && pos->col > 0)
				|| (lnum == 1 && buf_was_empty)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			صحیح	col = pos == پوچ ? 0 : pos->col;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			اگر (newline != پوچ)
			{
			    mch_memmove(newline, oldline, (size_t)col);
			    newline[col] = NUL;
			    STRCAT(newline, args);
			    STRCAT(newline, oldline + col);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    وگرنه
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), args,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    اگر (nlp == پوچ)
			بشکن;
		    ++lnum;
		    args = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_سطرها_mark(lnum_start - 1, (long)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		اگر (buf_was_empty)
		{
		    اگر (ff_detected == EOL_UNKNOWN)
#اگر defined(MSWIN)
			ff_detected = EOL_DOS;
#وگرنه
			ff_detected = EOL_UNIX;
#endif
		    set_fileformat(ff_detected, OPT_LOCAL);
		    curbuf->b_start_ffc = *curbuf->b_p_ff;
		}

		/*
		 * XXX - GRP - Is the next line right? If I've inserted
		 * text the buffer has been updated but not written. Will
		 * netbeans guarantee to write it? Even if I do a :q! ?
		 */
		curbuf->b_changed = old_b_changed; // logically unchanged
		netbeansFireChanges = oldFire;

		// Undo info is invalid now...
		u_clearallandblockfree(curbuf);
	    }
	    vim_free(to_free);
	    nb_reply_nil(cmdno); // or !error
	}
	وگرنه
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_reply_nil(cmdno);
	    retval = FAIL;
	}
    }
    وگرنه // Not a function; no reply required.
    {
// =====================================================================
	اگر (streq((نویسه *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    اگر (buf == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_buffer_identifier_in_create));
		بازگشت FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, پوچ, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->درج_Done = FALSE;
#اگر defined(FEAT_MENU) && defined(FEAT_GUI)
	    اگر (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "insertDone"))
	{
	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    وگرنه
	    {
		buf->bufp->b_start_eol = *args == 'T';
		buf->درج_Done = TRUE;
		args += 2;
		buf->bufp->b_p_ro = *args == 'T';
		print_read_پیام(buf);
	    }
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "saveDone"))
	{
	    long savedChars = atol((نویسه *)args);

	    اگر (buf == پوچ || buf->bufp == پوچ)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    وگرنه
		print_save_پیام(buf, savedChars);
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "startDocumentListen"))
	{
	    اگر (buf == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_startdocumentlisten));
		بازگشت FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "stopDocumentListen"))
	{
	    اگر (buf == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_stopdocumentlisten));
		بازگشت FAIL;
	    }
	    buf->fireChanges = 0;
	    اگر (buf->bufp != پوچ && buf->bufp->b_was_netbeans_file)
	    {
		اگر (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_lost_for_buffer_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_lost_for_buffer_nr),
							   buf->bufp->b_fnum);
		}
		وگرنه
		{
		    // NetBeans uses stopDocumentListen when it stops editing
		    // a file.  It then expects the buffer in Vim to
		    // disappear.
		    do_bufdel(DOBUF_DEL, (char_u *)"", 1,
				  buf->bufp->b_fnum, buf->bufp->b_fnum, TRUE);
		    CLEAR_POINTER(buf);
		}
	    }
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "setTitle"))
	{
	    اگر (buf == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_buffer_identifier_in_settitle));
		بازگشت FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, پوچ);
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "initDone"))
	{
	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_buffer_identifier_in_initdone));
		بازگشت FAIL;
	    }
	    do_update = 1;
	    buf->initDone = TRUE;
	    nb_set_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_کلید_queue();
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "setBufferNumber")
		|| streq((نویسه *)cmd, "putBufferNumber"))
	{
	    char_u	*path;
	    buf_T	*bufp;

	    اگر (buf == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_buffer_identifier_in_setbuffernumber));
		بازگشت FAIL;
	    }
	    path = (char_u *)nb_unquote(args, پوچ);
	    اگر (path == پوچ)
		بازگشت FAIL;
	    bufp = buflist_findname(path);
	    vim_free(path);
	    اگر (bufp == پوچ)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", args));
		semsg(_(e_file_str_not_found_in_setbuffernumber), args);
		بازگشت FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_عدد = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    اگر (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    وگرنه
	    {
		buf->initDone = TRUE;

		// handle any postponed key commands
		handle_کلید_queue();
	    }

// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "setFullName"))
	{
	    اگر (buf == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_buffer_identifier_in_setfullname));
		بازگشت FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, پوچ);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#اگر defined(FEAT_MENU) && defined(FEAT_GUI)
	    اگر (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "editFile"))
	{
	    اگر (buf == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_buffer_identifier_in_editfile));
		بازگشت FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, پوچ);
	    do_ecmd(0, (char_u *)buf->displayname, پوچ, پوچ, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->initDone = TRUE;
	    do_update = 1;
	    maketitle();
#اگر defined(FEAT_MENU) && defined(FEAT_GUI)
	    اگر (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "setVisible"))
	{
	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		اگر (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setvisible));
		بازگشت FAIL;
	    }
	    اگر (streq((نویسه *)args, "T") && buf->bufp != curbuf)
	    {
		exarg_T exarg;
		CLEAR_FIELD(exarg);
		exarg.cmd = (char_u *)"goto";
		exarg.forceit = FALSE;
		exarg.cmdidx = CMD_USER;
		dosetvisible = TRUE;
		goto_buffer(&exarg, DOBUF_FIRST, FORWARD, buf->bufp->b_fnum);
		do_update = 1;
		dosetvisible = FALSE;

#ifdef FEAT_GUI
		// Side effect!!!.
		اگر (gui.in_use)
		    gui_mch_set_foreground();
#endif
	    }
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "raise"))
	{
#ifdef FEAT_GUI
	    // Bring gvim to the foreground.
	    اگر (gui.in_use)
		gui_mch_set_foreground();
#endif
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "setModified"))
	{
	    صحیح prev_b_changed;

	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		اگر (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setmodified));
		بازگشت FAIL;
	    }
	    prev_b_changed = buf->bufp->b_changed;
	    اگر (streq((نویسه *)args, "T"))
		buf->bufp->b_changed = TRUE;
	    وگرنه
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		اگر (buf->bufp->b_ffname != پوچ
			&& mch_stat((نویسه *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_time(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_changed = FALSE;
	    }
	    buf->modified = buf->bufp->b_changed;
	    اگر (prev_b_changed != buf->bufp->b_changed)
	    {
		check_status(buf->bufp);
		redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "setModtime"))
	{
	    اگر (buf == پوچ || buf->bufp == پوچ)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    وگرنه
	    {
		buf->bufp->b_mtime = atoi((نویسه *)args);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "setReadOnly"))
	{
	    اگر (buf == پوچ || buf->bufp == پوچ)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    وگرنه اگر (streq((نویسه *)args, "T"))
		buf->bufp->b_p_ro = TRUE;
	    وگرنه
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "showBalloon"))
	{
#اگر defined(FEAT_BEVAL_GUI)
	    ایستا نویسه	*text = پوچ;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    اگر (balloonEval != پوچ)
	    {
		vim_free(text);
		text = nb_unquote(args, پوچ);
		اگر (text != پوچ)
		    gui_mch_post_balloon(balloonEval, (char_u *)text);
	    }
#endif
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "setDot"))
	{
	    pos_T *pos;
#ifdef NBDEBUG
	    char_u *s;
#endif

	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_buffer_identifier_in_setdot));
		بازگشت FAIL;
	    }

	    nb_set_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    اگر (VIsual_active)
		end_visual_mode();
#ifdef NBDEBUG
	    s = args;
#endif
	    pos = get_off_or_lnum(buf->bufp, &args);
	    اگر (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#ifdef FEAT_FOLDING
		foldOpenCursor();
#endif
	    }
	    وگرنه
	    {
		nbdebug(("    BAD POSITION in setDot: %s\n", s));
	    }

	    // gui_update_cursor(TRUE, FALSE);
	    // update_curbuf(UPD_NOT_VALID);
	    update_topline();		// scroll to show the line
	    update_screen(UPD_VALID);
	    setcursor();
	    cursor_on();
	    out_flush_cursor(TRUE, FALSE);

	    // Quit a hit-return or more prompt.
	    اگر (State == MODE_HITRETURN || State == MODE_ASKMORE)
	    {
#ifdef FEAT_GUI_GTK
		اگر (gui.in_use && gtk_main_level() > 0)
		    gtk_main_quit();
#endif
	    }
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "close"))
	{
#ifdef NBDEBUG
	    نویسه *name = "<NONE>";
#endif

	    اگر (buf == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_buffer_identifier_in_close));
		بازگشت FAIL;
	    }

#ifdef NBDEBUG
	    اگر (buf->displayname != پوچ)
		name = buf->displayname;
#endif
	    اگر (buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		اگر (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    اگر (buf->bufp != پوچ)
		do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = پوچ;
	    buf->initDone = FALSE;
	    do_update = 1;
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "defineAnnoType"))
	{
#ifdef FEAT_SIGNS
	    صحیح type_رقم;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    صحیح parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    اگر (buf == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_buffer_identifier_in_defineannotype));
		بازگشت FAIL;
	    }

	    cp = (نویسه *)args;
	    type_رقم = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    args = skipwhite(args);
	    typeName = (char_u *)nb_unquote(args, &args);
	    args = skipwhite(args + 1);
	    tooltip = (char_u *)nb_unquote(args, &args);
	    args = skipwhite(args + 1);

	    p = (char_u *)nb_unquote(args, &args);
	    glyphFile = vim_strsave_escaped(p, escape_chars);
	    vim_free(p);

	    args = skipwhite(args + 1);
	    p = skiptowhite(args);
	    اگر (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(args);
	    bg = vim_strsave(p);
	    اگر (STRLEN(fg) > MAX_COLOR_LENGTH || STRLEN(bg) > MAX_COLOR_LENGTH)
	    {
		emsg(_(e_highlighting_color_name_too_long_in_defineAnnoType));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    وگرنه اگر (!nb_is_safe_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_safe_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_safe_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_in_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    وگرنه اگر (typeName != پوچ && tooltip != پوچ && glyphFile != پوچ)
		addsigntype(buf, type_رقم, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    اگر (parse_error)
		بازگشت FAIL;

#endif
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "addAnno"))
	{
#ifdef FEAT_SIGNS
	    صحیح ser_رقم;
	    صحیح local_Type_رقم;
	    صحیح type_رقم;
	    pos_T *pos;

	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_addanno));
		بازگشت FAIL;
	    }

	    do_update = 1;

	    cp = (نویسه *)args;
	    ser_رقم = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_رقم = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    type_رقم = mapsigntype(buf, local_Type_رقم);

	    pos = get_off_or_lnum(buf->bufp, &args);

	    cp = (نویسه *)args;
	    vim_ignored = (صحیح)strtol(cp, &cp, 10);
	    args = (char_u *)cp;
# ifdef NBDEBUG
	    اگر (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    اگر (ser_رقم >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		بازگشت FAIL;
	    }
	    اگر (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_رقم, pos->lnum, type_رقم, buf->bufp->b_fnum);
		اگر (type_رقم == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_رقم,
						       buf->bufp->b_fnum);
	    }
#endif
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "removeAnno"))
	{
#ifdef FEAT_SIGNS
	    صحیح ser_رقم;

	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		بازگشت FAIL;
	    }
	    do_update = 1;
	    cp = (نویسه *)args;
	    ser_رقم = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_رقم, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#endif
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "moveAnnoToFront"))
	{
#ifdef FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#endif
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "guard") || streq((نویسه *)cmd, "unguard"))
	{
	    صحیح len;
	    pos_T first;
	    pos_T last;
	    pos_T *pos;
	    صحیح un = (cmd[0] == 'u');
	    ایستا صحیح guardId = GUARDEDOFFSET;

	    اگر (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (نویسه *) cmd));
		بازگشت OK;
	    }

	    nb_init_graphics();

	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		بازگشت FAIL;
	    }
	    nb_set_curbuf(buf->bufp);
	    cp = (نویسه *)args;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, پوچ, 10);
	    args = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    اگر (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    وگرنه
	    {
		first = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		اگر (pos != پوچ && pos->col == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		اگر (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		وگرنه
		{
		    long lnum;
		    last = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     first.lnum, first.col,
			     last.lnum, last.col));
#ifdef FEAT_SIGNS
		    برای (lnum = first.lnum; lnum <= last.lnum; lnum++)
		    {
			اگر (un)
			{
			    // never used
			}
			وگرنه
			{
			    اگر (buf_findsigntype_id(buf->bufp, lnum,
				GUARDED) == 0)
			    {
				coloncmd(
				    ":sign place %d line=%ld name=%d buffer=%d",
				     guardId++, lnum, GUARDED,
				     buf->bufp->b_fnum);
			    }
			}
		    }
#endif
		    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
		}
	    }
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "startAtomic"))
	{
	    inAtomic = 1;
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "endAtomic"))
	{
	    inAtomic = 0;
	    اگر (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		بازگشت FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    اگر (bufIsChanged(buf->bufp))
	    {
		// Only write if the buffer can be written.
		اگر (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != پوچ
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    اگر (!bufref_valid(&bufref))
			buf->bufp = پوچ;
		}
	    }
	    وگرنه
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "netbeansBuffer"))
	{
	    اگر (buf == پوچ || buf->bufp == پوچ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		بازگشت FAIL;
	    }
	    اگر (*args == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    وگرنه
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "specialKeys"))
	{
	    special_کلیدها(args);
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	وگرنه اگر (streq((نویسه *)cmd, "version"))
	{
	    // not used yet
	}
	وگرنه
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    اگر (inAtomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    اگر (buf != پوچ && buf->initDone && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	out_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	اگر (State == MODE_HITRETURN || State == MODE_ASKMORE)
	{
#ifdef FEAT_GUI_GTK
	    اگر (gui.in_use && gtk_main_level() > 0)
		gtk_main_quit();
#endif
	}
    }

    بازگشت retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    ایستا تهی
nb_set_curbuf(buf_T *buf)
{
    اگر (curbuf == buf)
	بازگشت;

    اگر (buf_jump_open_win(buf) != پوچ)
	بازگشت;
    اگر ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != پوچ)
	بازگشت;
    set_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    ایستا تهی
coloncmd(نویسه *cmd, ...)
{
    نویسه buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, اندازه(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, پوچ, پوچ, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    out_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    ایستا تهی
special_کلیدها(char_u *args)
{
    نویسه *save_str = nb_unquote(args, پوچ);
    نویسه *tok = strtok(save_str, " ");
    نویسه *sep;
#define KEYBUFLEN 64
    نویسه keybuf[KEYBUFLEN];
    نویسه cmdbuf[256];

    وقتی (tok != پوچ)
    {
	صحیح i = 0;

	اگر ((sep = strchr(tok, '-')) != پوچ)
	{
	    *sep = NUL;
	    وقتی (*tok && i + 2 < KEYBUFLEN)
	    {
		انتخاب (*tok)
		{
		    حالت 'A':
		    حالت 'M':
		    حالت 'C':
		    حالت 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			بشکن;
		}
		tok++;
	    }
	    tok++;
	}

	اگر (strlen(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    صحیح safe = TRUE;
	    برای (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		اگر (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    safe = FALSE;
		    بشکن;
		}
	    }
	    اگر (safe)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, اندازه(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, MODE_NORMAL, FALSE);
	    }
	}
	tok = strtok(پوچ, " ");
    }
    vim_free(save_str);
}

    تهی
ex_nbclose(exarg_T *eap UNUSED)
{
    netbeans_close();
}

    تهی
ex_nbkey(exarg_T *eap)
{
    (تهی)netbeans_keystring(eap->arg);
}

    تهی
ex_nbstart(
    exarg_T	*eap)
{
#ifdef FEAT_GUI
# اگر !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    اگر (gui.in_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	بازگشت;
    }
# endif
#endif
    netbeans_open((نویسه *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    ایستا تهی
nb_init_graphics(تهی)
{
    ایستا صحیح did_init = FALSE;

    اگر (did_init)
	بازگشت;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    ایستا تهی
netbeans_keyname(صحیح کلید, نویسه *buf)
{
    نویسه *name = 0;
    نویسه namebuf[2];
    صحیح ctrl  = 0;
    صحیح shift = 0;
    صحیح alt   = 0;

    اگر (mod_mask & MOD_MASK_CTRL)
	ctrl = 1;
    اگر (mod_mask & MOD_MASK_SHIFT)
	shift = 1;
    اگر (mod_mask & MOD_MASK_ALT)
	alt = 1;


    انتخاب (کلید)
    {
	حالت K_F1:		name = "F1";		بشکن;
	حالت K_S_F1:	name = "F1";	shift = 1;	بشکن;
	حالت K_F2:		name = "F2";		بشکن;
	حالت K_S_F2:	name = "F2";	shift = 1;	بشکن;
	حالت K_F3:		name = "F3";		بشکن;
	حالت K_S_F3:	name = "F3";	shift = 1;	بشکن;
	حالت K_F4:		name = "F4";		بشکن;
	حالت K_S_F4:	name = "F4";	shift = 1;	بشکن;
	حالت K_F5:		name = "F5";		بشکن;
	حالت K_S_F5:	name = "F5";	shift = 1;	بشکن;
	حالت K_F6:		name = "F6";		بشکن;
	حالت K_S_F6:	name = "F6";	shift = 1;	بشکن;
	حالت K_F7:		name = "F7";		بشکن;
	حالت K_S_F7:	name = "F7";	shift = 1;	بشکن;
	حالت K_F8:		name = "F8";		بشکن;
	حالت K_S_F8:	name = "F8";	shift = 1;	بشکن;
	حالت K_F9:		name = "F9";		بشکن;
	حالت K_S_F9:	name = "F9";	shift = 1;	بشکن;
	حالت K_F10:		name = "F10";		بشکن;
	حالت K_S_F10:	name = "F10";	shift = 1;	بشکن;
	حالت K_F11:		name = "F11";		بشکن;
	حالت K_S_F11:	name = "F11";	shift = 1;	بشکن;
	حالت K_F12:		name = "F12";		بشکن;
	حالت K_S_F12:	name = "F12";	shift = 1;	بشکن;
	پیشفرض:
			اگر (کلید >= ' ' && کلید <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = کلید;
			    namebuf[1] = NUL;
			}
			وگرنه
			    name = "X";
			بشکن;
    }

    buf[0] = '\0';
    اگر (ctrl)
	strcat(buf, "C");
    اگر (shift)
	strcat(buf, "S");
    اگر (alt)
	strcat(buf, "M"); // META
    اگر (ctrl || shift || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#اگر defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    تهی
netbeans_beval_cb(
	BalloonEval	*beval,
	صحیح		 state UNUSED)
{
    win_T	*wp;
    char_u	*text;
    linenr_T	lnum;
    صحیح		col;
    نویسه	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    اگر (!can_use_beval() || !NETBEANS_OPEN)
	بازگشت;

    اگر (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) != OK)
	بازگشت;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    اگر (text != پوچ && text[0] != NUL && STRLEN(text) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	اگر (buf != پوچ)
	{
	    p = nb_quote(text);
	    اگر (p != پوچ)
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
    vim_free(text);
}
#endif

/*
 * Return TRUE when the netbeans connection is active.
 */
    صحیح
netbeans_active(تهی)
{
    بازگشت NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    تهی
netbeans_open(نویسه *params, صحیح doabort)
{
    نویسه *cmd = "0:startupDone=0\n";

    اگر (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	بازگشت;
    }

    اگر (netbeans_connect(params, doabort) != OK)
	بازگشت;

    nbdebug(("EVT: %s", cmd));
    nb_send(cmd, "netbeans_startup_done");

    // update the screen after having added the gutter
    changed_window_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    out_flush_cursor(TRUE, FALSE);
}

/*
 * Tell netbeans that we're exiting. This should be called right
 * before calling exit.
 */
    تهی
netbeans_send_disconnect(تهی)
{
    نویسه buf[128];

    اگر (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#اگر defined(FEAT_EVAL)
    صحیح
set_ref_in_nb_channel(صحیح copyID)
{
    صحیح abort = FALSE;
    typval_T tv;

    اگر (nb_channel == پوچ)
	بازگشت FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = set_ref_in_item(&tv, copyID, پوچ, پوچ, پوچ);
    بازگشت abort;
}
#endif

#اگر defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    تهی
netbeans_frame_moved(صحیح new_x, صحیح new_y)
{
    نویسه buf[128];

    اگر (!NETBEANS_OPEN)
	بازگشت;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (صحیح)Columns, (صحیح)Rows, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#endif

/*
 * Tell netbeans the user opened or activated a file.
 */
    تهی
netbeans_file_activated(buf_T *bufp)
{
    صحیح bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    نویسه    buffer[2*MAXPATHL];
    char_u  *q;

    اگر (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	بازگشت;

    q = nb_quote(bufp->b_ffname);
    اگر (q == پوچ)
	بازگشت;
    اگر (bp == پوچ)
    {
	vim_free(q);
	بازگشت;
    }

    vim_snprintf(buffer, اندازه(buffer),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (نویسه *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    تهی
netbeans_file_opened(buf_T *bufp)
{
    صحیح bufno = nb_getbufno(bufp);
    نویسه    buffer[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    صحیح	    bnum;

    اگر (!NETBEANS_OPEN)
	بازگشت;

    q = nb_quote(bufp->b_ffname);
    اگر (q == پوچ)
	بازگشت;
    اگر (bp != پوچ)
	bnum = bufno;
    وگرنه
	bnum = 0;

    vim_snprintf(buffer, اندازه(buffer), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (نویسه *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
    اگر (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	last_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    تهی
netbeans_file_killed(buf_T *bufp)
{
    صحیح		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    نویسه	buffer[2*MAXPATHL];

    اگر (!NETBEANS_OPEN || bufno == -1)
	بازگشت;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(buffer, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_killed");

    اگر (nbbuf != پوچ)
	nbbuf->bufp = پوچ;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    ایستا nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, صحیح *bufnop)
{
    صحیح		bufno;
    nbbuf_T	*nbbuf;

    اگر (!NETBEANS_OPEN || !netbeansFireChanges)
	بازگشت پوچ;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    اگر (bufno <= 0)
	بازگشت پوچ;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    اگر (nbbuf != پوچ && !nbbuf->fireChanges)
	بازگشت پوچ;		// changes in this buffer are not reported

    *bufnop = bufno;
    بازگشت nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    تهی
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    char_u	*txt,
    صحیح		newlen)
{
    char_u	*buf;
    صحیح		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;
    char_u	*p;
    char_u	*newtxt;

    اگر (!NETBEANS_OPEN)
	بازگشت;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    اگر (nbbuf == پوچ)
	بازگشت;

    // Don't mark as modified for initial read
    اگر (nbbuf->درج_Done)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.col = col;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    اگر (p != پوچ)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((نویسه *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((نویسه *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    تهی
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    long	len)
{
    char_u	buf[128];
    صحیح		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;

    اگر (!NETBEANS_OPEN)
	بازگشت;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    اگر (nbbuf == پوچ)
	بازگشت;

    اگر (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	بازگشت;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.col = col;

    off = pos2off(bufp, &pos);

    sprintf((نویسه *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((نویسه *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    تهی
netbeans_unmodified(buf_T *bufp UNUSED)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    تهی
netbeans_button_release(صحیح button)
{
    نویسه	buf[128];
    صحیح		bufno;

    اگر (!NETBEANS_OPEN)
	بازگشت;

    bufno = nb_getbufno(curbuf);

    اگر (bufno < 0 || curwin == پوچ || curwin->w_buffer != curbuf)
	بازگشت;

    صحیح col = mouse_col - curwin->w_wincol
	- ((curwin->w_p_nu || curwin->w_p_rnu) ? 9 : 1);
    long off = pos2off(curbuf, &curwin->w_cursor);

    // sync the cursor position
    sprintf(buf, "%d:newDotAndMark=%d %ld %ld\n", bufno, r_cmdno, off, off);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release[newDotAndMark]");

    sprintf(buf, "%d:buttonRelease=%d %d %ld %d\n", bufno, r_cmdno,
	    button, (long)curwin->w_cursor.lnum, col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release");
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key code.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    صحیح
netbeans_keycommand(صحیح کلید)
{
    نویسه	کلید_Name[60];

    netbeans_keyname(کلید, کلید_Name);
    بازگشت netbeans_keystring((char_u *)کلید_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    ایستا صحیح
netbeans_keystring(char_u *کلید_Name)
{
    نویسه	buf[2*MAXPATHL];
    صحیح		bufno = nb_getbufno(curbuf);
    long	off;
    char_u	*q;

    اگر (!NETBEANS_OPEN)
	بازگشت TRUE;

    اگر (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == پوچ ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	اگر (q == پوچ)
	    بازگشت TRUE;
	vim_snprintf(buf, اندازه(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	اگر (curbuf->b_ffname != پوچ)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(کلید_Name);
	بازگشت FALSE;
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
    vim_snprintf(buf, اندازه(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, کلید_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, اندازه(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, کلید_Name,
		off, (long)curwin->w_cursor.lnum, (long)curwin->w_cursor.col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    بازگشت TRUE;
}


/*
 * Send a save event to netbeans.
 */
    تهی
netbeans_save_buffer(buf_T *bufp)
{
    char_u	buf[64];
    صحیح		bufno;
    nbbuf_T	*nbbuf;

    اگر (!NETBEANS_OPEN)
	بازگشت;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    اگر (nbbuf == پوچ)
	بازگشت;

    nbbuf->modified = 0;

    sprintf((نویسه *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((نویسه *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    تهی
netbeans_deleted_all_سطرها(buf_T *bufp)
{
    char_u	buf[64];
    صحیح		bufno;
    nbbuf_T	*nbbuf;

    اگر (!NETBEANS_OPEN)
	بازگشت;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    اگر (nbbuf == پوچ)
	بازگشت;

    // Don't mark as modified for initial read
    اگر (nbbuf->درج_Done)
	nbbuf->modified = 1;

    sprintf((نویسه *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    صحیح
netbeans_is_guarded(linenr_T بالاترین, linenr_T bot)
{
    sign_entry_T	*p;
    صحیح			lnum;

    اگر (!NETBEANS_OPEN)
	بازگشت FALSE;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
	اگر (p->se_id >= GUARDEDOFFSET)
	    برای (lnum = بالاترین + 1; lnum < bot; lnum++)
		اگر (lnum == p->se_lnum)
		    بازگشت TRUE;

    بازگشت FALSE;
}

#اگر defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    تهی
netbeans_draw_multisign_indicator(صحیح row)
{
    صحیح i;
    صحیح y;
    صحیح x;

    اگر (!NETBEANS_OPEN)
	بازگشت;

    x = 0;
    y = row * gui.char_height + 2;

    برای (i = 0; i < gui.char_height - 3; i++)
	XDrawPoint(gui.dpy, gui.wid, gui.text_gc, x+2, y++);

    XDrawPoint(gui.dpy, gui.wid, gui.text_gc, x+0, y);
    XDrawPoint(gui.dpy, gui.wid, gui.text_gc, x+2, y);
    XDrawPoint(gui.dpy, gui.wid, gui.text_gc, x+4, y++);
    XDrawPoint(gui.dpy, gui.wid, gui.text_gc, x+1, y);
    XDrawPoint(gui.dpy, gui.wid, gui.text_gc, x+2, y);
    XDrawPoint(gui.dpy, gui.wid, gui.text_gc, x+3, y++);
    XDrawPoint(gui.dpy, gui.wid, gui.text_gc, x+2, y);
}
#endif // FEAT_GUI_X11

#اگر defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    تهی
netbeans_draw_multisign_indicator(صحیح row)
{
    صحیح i;
    صحیح y;
    صحیح x;
# اگر GTK_CHECK_VERSION(3,0,0)
    cairo_t *cr = پوچ;
# وگرنه
    GdkDrawable *drawable = gui.drawarea->window;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    اگر (!NETBEANS_OPEN)
	بازگشت;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# وگرنه
    y = row * gui.char_height + 2;
# endif

# اگر GTK_CHECK_VERSION(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_height);
    cr = cairo_create(surf);
#  وگرنه
    cr = cairo_create(gui.surface);
#  endif
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    برای (i = 0; i < gui.char_height - 3; i++)
# اگر GTK_CHECK_VERSION(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# وگرنه
    gdk_draw_point(drawable, gui.text_gc, x+2, y++);
# endif

# اگر GTK_CHECK_VERSION(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# وگرنه
    gdk_draw_point(drawable, gui.text_gc, x+0, y);
    gdk_draw_point(drawable, gui.text_gc, x+2, y);
    gdk_draw_point(drawable, gui.text_gc, x+4, y++);
    gdk_draw_point(drawable, gui.text_gc, x+1, y);
    gdk_draw_point(drawable, gui.text_gc, x+2, y);
    gdk_draw_point(drawable, gui.text_gc, x+3, y++);
    gdk_draw_point(drawable, gui.text_gc, x+2, y);
# endif

# ifdef USE_GTK4
    cairo_fill(cr);
    gui_gtk4_add_multisign(surf, row, 0, 5, gui.char_height);
# endif

# اگر GTK_CHECK_VERSION(3,0,0)
    cairo_destroy(cr);
# endif
# ifdef USE_GTK4
    cairo_surface_destroy(surf);
# endif
}
#endif // FEAT_GUI_GTK

/*
 * If the mouse is clicked in the gutter of a line with multiple
 * annotations, cycle through the set of signs.
 */
    تهی
netbeans_gutter_click(linenr_T lnum)
{
    sign_entry_T	*p;

    اگر (!NETBEANS_OPEN)
	بازگشت;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
    {
	اگر (p->se_lnum == lnum && p->se_next && p->se_next->se_lnum == lnum)
	{
	    sign_entry_T *tail;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    اگر (p->se_prev)
		p->se_prev->se_next = p->se_next;
	    وگرنه
		curbuf->b_signlist = p->se_next;
	    p->se_next->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    برای (tail = p->se_next;
		  tail->se_next && tail->se_next->se_lnum == lnum
				       && tail->se_next->se_id < GUARDEDOFFSET;
		  tail = tail->se_next)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_next = tail->se_next;
	    اگر (tail->se_next)
		tail->se_next->se_prev = p;
	    p->se_prev = tail;
	    tail->se_next = p;
	    update_debug_sign(curbuf, lnum);
	    بشکن;
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
    ایستا تهی
addsigntype(
    nbbuf_T	*buf,
    صحیح		type_رقم,
    char_u	*typeName,
    char_u	*tooltip UNUSED,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    صحیح i, j;
    صحیح use_fg = (*fg && STRCMP(fg, "none") != 0);
    صحیح use_bg = (*bg && STRCMP(bg, "none") != 0);

    برای (i = 0; i < globalsignmapused; i++)
	اگر (STRCMP(typeName, globalsignmap[i]) == 0)
	    بشکن;

    اگر (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_رقم, typeName, tooltip, glyphFile, fg, bg));
	اگر (use_fg || use_bg)
	{
	    نویسه fgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    نویسه bgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    نویسه *ptr;
	    صحیح مقدار;

	    مقدار = strtol((نویسه *)fg, &ptr, 10);
	    اگر (ptr != (نویسه *)fg)
		sprintf(fgbuf, "guifg=#%06x", مقدار & 0xFFFFFF);
	    وگرنه
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    مقدار = strtol((نویسه *)bg, &ptr, 10);
	    اگر (ptr != (نویسه *)bg)
		sprintf(bgbuf, "guibg=#%06x", مقدار & 0xFFFFFF);
	    وگرنه
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    اگر (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    وگرنه اگر (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    وگرنه
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	وگرنه
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	اگر (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_رقم;

	اگر (globalsignmapused == globalsignmaplen)
	{
	    اگر (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(نویسه *, globalsignmaplen);
	    }
	    وگرنه    // grow it
	    {
		صحیح incr;
		صحیح oldlen = globalsignmaplen;
		نویسه **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * اندازه(نویسه *));
		اگر (globalsignmap == پوچ)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    بازگشت;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * اندازه(نویسه *));
	    }
	}

	globalsignmap[i] = (نویسه *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    برای (j = 0; j < buf->signmapused; j++)
	اگر (buf->signmap[j] == i + 1)
	    بازگشت;

    // add to local map
    اگر (buf->signmapused == buf->signmaplen)
    {
	اگر (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(صحیح, buf->signmaplen);
	}
	وگرنه    // grow it
	{
	    صحیح incr;
	    صحیح oldlen = buf->signmaplen;
	    صحیح *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * اندازه(صحیح));
	    اگر (buf->signmap == پوچ)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		بازگشت;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * اندازه(صحیح));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    ایستا صحیح
mapsigntype(nbbuf_T *buf, صحیح localsigntype)
{
    اگر (--localsigntype >= 0 && localsigntype < buf->signmapused)
	بازگشت buf->signmap[localsigntype];

    بازگشت 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    ایستا long
get_buf_size(buf_T *bufp)
{
    linenr_T	lnum;
    long	char_شمار = 0;
    صحیح		eol_size;
    long	last_check = 100000L;

    اگر (bufp->b_ml.ml_flags & ML_EMPTY)
	بازگشت 0;

    اگر (get_fileformat(bufp) == EOL_DOS)
	eol_size = 2;
    وگرنه
	eol_size = 1;
    برای (lnum = 1; lnum <= bufp->b_ml.ml_سطر_شمار; ++lnum)
    {
	char_شمار += ml_get_buf_len(bufp, lnum) + eol_size;
	// Check for a CTRL-C every 100000 characters
	اگر (char_شمار > last_check)
	{
	    ui_breakcheck();
	    اگر (got_int)
		بازگشت char_شمار;
	    last_check = char_شمار + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    اگر (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_شمار -= eol_size;

    بازگشت char_شمار;
}

/*
 * Convert character offset to lnum,col
 */
    ایستا pos_T *
off2pos(buf_T *buf, long offset)
{
    linenr_T	 lnum;
    ایستا pos_T pos;

    pos.lnum = 0;
    pos.col = 0;
    pos.coladd = 0;

    اگر (!(buf->b_ml.ml_flags & ML_EMPTY))
    {
	اگر ((lnum = ml_find_سطر_or_offset(buf, (linenr_T)0, &offset)) < 0)
	    بازگشت پوچ;
	pos.lnum = lnum;
	pos.col = offset;
    }

    بازگشت &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    ایستا pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    ایستا pos_T	mypos;
    long		off;

    off = strtol((نویسه *)*argp, (نویسه **)argp, 10);
    اگر (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.col = strtol((نویسه *)*argp, (نویسه **)argp, 10);
	mypos.coladd = 0;
	بازگشت &mypos;
    }
    بازگشت off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    ایستا long
pos2off(buf_T *buf, pos_T *pos)
{
    long	 offset = 0;

    اگر (buf->b_ml.ml_flags & ML_EMPTY)
	بازگشت 0;

    اگر ((offset = ml_find_سطر_or_offset(buf, pos->lnum, 0)) < 0)
	بازگشت 0;
    offset += pos->col;

    بازگشت offset;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    ایستا تهی
print_read_پیام(nbbuf_T *buf)
{
    صحیح	    lnum = buf->bufp->b_ml.ml_سطر_شمار;
    off_T   nchars = buf->bufp->b_orig_size;
    char_u  c;

    پیام_add_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    اگر (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    اگر (!buf->bufp->b_start_eol)
    {
	STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    پیام_add_سطرها(c, (long)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_پیام);
    پیام_scrolled_ign = TRUE;
    پیام_trunc_attr((نویسه *)IObuff, FALSE, 0);
    پیام_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    ایستا تهی
print_save_پیام(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    اگر (nchars >= 0)
    {
	// put fname in IObuff with quotes
	پیام_add_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	پیام_add_سطرها(c, buf->bufp->b_ml.ml_سطر_شمار,
						buf->bufp->b_orig_size);

	VIM_CLEAR(keep_پیام);
	پیام_scrolled_ign = TRUE;
	p = (char_u *)پیام_trunc_attr((نویسه *)IObuff, FALSE, 0);
	اگر ((پیام_scrolled && !need_wait_return) || !buf->initDone)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_پیام(p, 0);
	}
	پیام_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    وگرنه
    {
	نویسه msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_add_bang_to_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#endif // defined(FEAT_NETBEANS_INTG)
