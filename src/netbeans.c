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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_NETBEANS_INTG)

#ifndef MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#endif

#অন্তর্ভাব "version.h"

#define GUARDED		10000 // typenr for "guarded" annotation
#define GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#define MAX_COLOR_LENGTH 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#define VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#define VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
স্থির বর্ণ *ExtEdProtocolVersion = "2.5";

স্থির long pos2off(buf_T *, pos_T *);
স্থির pos_T *off2pos(buf_T *, long);
স্থির pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
স্থির long get_buf_size(buf_T *);
স্থির পূর্ণাঙ্ক netbeans_keystring(char_u *keystr);
স্থির শূন্য special_চাবিগুলি(char_u *args);

স্থির পূর্ণাঙ্ক getConnInfo(বর্ণ *file, বর্ণ **host, বর্ণ **port, বর্ণ **password);

স্থির শূন্য nb_init_graphics(শূন্য);
স্থির শূন্য coloncmd(বর্ণ *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
স্থির শূন্য nb_set_curbuf(buf_T *buf);
স্থির শূন্য nb_parse_cmd(char_u *);
স্থির পূর্ণাঙ্ক  nb_do_cmd(পূর্ণাঙ্ক, char_u *, পূর্ণাঙ্ক, পূর্ণাঙ্ক, char_u *);
স্থির শূন্য nb_send(বর্ণ *buf, বর্ণ *fun);
স্থির শূন্য nb_free(শূন্য);

#define NETBEANS_OPEN (channel_can_write_to(nb_channel))
স্থির channel_T *nb_channel = রিক্ত;

স্থির পূর্ণাঙ্ক r_cmdno;			// current command number for reply
স্থির পূর্ণাঙ্ক dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ifdef NBDEBUG
# include "nbdebug.c"
#endif

স্থির পূর্ণাঙ্ক needupdate = 0;
স্থির পূর্ণাঙ্ক inAtomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    স্থির পূর্ণাঙ্ক
nb_is_safe_string(char_u *str, char_u *allowed)
{
    যদি (str == রিক্ত)
	প্রত্যাগম FALSE;
    প্রতি (char_u *p = str; *p != NUL; p++)
	যদি (vim_strchr(allowed, *p) == রিক্ত)
	    প্রত্যাগম FALSE;
    প্রত্যাগম TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    স্থির শূন্য
nb_channel_closed(শূন্য)
{
    nb_channel = রিক্ত;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    স্থির শূন্য
netbeans_close(শূন্য)
{
    যদি (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	যদি (nb_channel != রিক্ত)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = রিক্ত;
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

    স্থির পূর্ণাঙ্ক
netbeans_connect(বর্ণ *params, পূর্ণাঙ্ক doabort)
{
    পূর্ণাঙ্ক		port;
    বর্ণ	buf[32];
    বর্ণ	*hostname = রিক্ত;
    বর্ণ	*address = রিক্ত;
    বর্ণ	*password = রিক্ত;
    বর্ণ	*fname;
    বর্ণ	*arg = রিক্ত;

    যদি (*params == '=')
    {
	// "=fname": Read info from specified file.
	যদি (getConnInfo(params + 1, &hostname, &address, &password) == FAIL)
	    প্রত্যাগম FAIL;
    }
    অন্যথা
    {
	যদি (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	যদি (arg == রিক্ত && (fname = getenv("__NETBEANS_CONINFO")) != রিক্ত)
	{
	    // "": get info from file specified in environment
	    যদি (getConnInfo(fname, &hostname, &address, &password) == FAIL)
		প্রত্যাগম FAIL;
	}
	অন্যথা
	{
	    যদি (arg != রিক্ত)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		address = strchr(hostname, ':');
		যদি (address != রিক্ত)
		{
		    *address++ = '\0';
		    password = strchr(address, ':');
		    যদি (password != রিক্ত)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    যদি (hostname == রিক্ত || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    যদি (address == রিক্ত)
		address = getenv("__NETBEANS_SOCKET");
	    যদি (password == রিক্ত)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    যদি (hostname != রিক্ত)
		hostname = (বর্ণ *)vim_strsave((char_u *)hostname);
	    যদি (address != রিক্ত)
		address = (বর্ণ *)vim_strsave((char_u *)address);
	    যদি (password != রিক্ত)
		password = (বর্ণ *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    যদি (hostname == রিক্ত || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (বর্ণ *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    যদি (address == রিক্ত || *address == '\0')
    {
	vim_free(address);
	address = (বর্ণ *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    যদি (password == রিক্ত || *password == '\0')
    {
	vim_free(password);
	password = (বর্ণ *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    যদি (hostname != রিক্ত && address != রিক্ত && password != রিক্ত)
    {
	port = atoi(address);
	nb_channel = channel_open(hostname, port, 3000, nb_channel_closed);
	যদি (nb_channel != রিক্ত)
	{
	    // success
#ifdef FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#endif

	    // success, login
	    vim_snprintf(buf, পরিমাণ(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", ExtEdProtocolVersion);
	    nb_send(buf, "externaleditor_version");
	}
    }

    যদি (nb_channel == রিক্ত && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(address);
    vim_free(password);
    প্রত্যাগম NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    স্থির পূর্ণাঙ্ক
getConnInfo(বর্ণ *file, বর্ণ **host, বর্ণ **port, বর্ণ **auth)
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
    যদি (mch_stat(file, &st) == 0 && (st.st_mode & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_mode_for_netbeans_connection_info_file_str),
									file);
	প্রত্যাগম FAIL;
    }
#endif

    fp = mch_fopen(file, "r");
    যদি (fp == রিক্ত)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	প্রত্যাগম FAIL;
    }

    // Read the file. There should be one of each parameter
    যাবত্ ((lp = (char_u *)fgets((বর্ণ *)buf, BUFSIZ, fp)) != রিক্ত)
    {
	যদি ((nlp = vim_strchr(lp, '\n')) != রিক্ত)
	    *nlp = 0;	    // strip off the trailing newline

	যদি (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (বর্ণ *)vim_strsave(&buf[5]);
	}
	অন্যথা যদি (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*port);
	    *port = (বর্ণ *)vim_strsave(&buf[5]);
	}
	অন্যথা যদি (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (বর্ণ *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    প্রত্যাগম OK;
}


সংরচনা keyqueue
{
    char_u	    *keystr;
    সংরচনা keyqueue *next;
    সংরচনা keyqueue *prev;
};

প্রকারনির্দেশ সংরচনা keyqueue চাবি_Q_T;

স্থির চাবি_Q_T চাবি_Head; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    স্থির শূন্য
postpone_keycommand(char_u *keystr)
{
    চাবি_Q_T *গ্রন্থি;

    গ্রন্থি = ALLOC_ONE(চাবি_Q_T);
    যদি (গ্রন্থি == রিক্ত)
	প্রত্যাগম;  // out of memory, drop the key

    যদি (চাবি_Head.next == রিক্ত) // initialize circular queue
    {
	চাবি_Head.next = &চাবি_Head;
	চাবি_Head.prev = &চাবি_Head;
    }

    // insert node at tail of queue
    গ্রন্থি->next = &চাবি_Head;
    গ্রন্থি->prev = চাবি_Head.prev;
    চাবি_Head.prev->next = গ্রন্থি;
    চাবি_Head.prev = গ্রন্থি;

    গ্রন্থি->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    স্থির শূন্য
handle_চাবি_queue(শূন্য)
{
    পূর্ণাঙ্ক postponed = FALSE;

    যাবত্ (!postponed && চাবি_Head.next && চাবি_Head.next != &চাবি_Head)
    {
	// first, unlink the node
	চাবি_Q_T *গ্রন্থি = চাবি_Head.next;
	চাবি_Head.next = গ্রন্থি->next;
	গ্রন্থি->next->prev = গ্রন্থি->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	যদি (গ্রন্থি->keystr != রিক্ত)
	    postponed = !netbeans_keystring(গ্রন্থি->keystr);
	vim_free(গ্রন্থি->keystr);

	// Finally, dispose of the node
	vim_free(গ্রন্থি);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    শূন্য
netbeans_parse_messages(শূন্য)
{
    readq_T	*গ্রন্থি;
    char_u	*buffer;
    char_u	*p;
    পূর্ণাঙ্ক		own_গ্রন্থি;

    যাবত্ (nb_channel != রিক্ত)
    {
	গ্রন্থি = channel_peek(nb_channel, PART_SOCK);
	যদি (গ্রন্থি == রিক্ত)
	    ভঙ্গ;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_first_nl(গ্রন্থি);
	যদি (p == রিক্ত)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    যদি (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		প্রত্যাগম;
	    অনুবর্ত;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	যদি (*p == NUL)
	{
	    own_গ্রন্থি = TRUE;
	    buffer = channel_get(nb_channel, PART_SOCK, রিক্ত);
	    // "node" is now invalid!
	}
	অন্যথা
	{
	    own_গ্রন্থি = FALSE;
	    buffer = গ্রন্থি->rq_buffer;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(buffer);

	যদি (own_গ্রন্থি)
	    // buffer finished, dispose of it
	    vim_free(buffer);
	অন্যথা যদি (nb_channel != রিক্ত)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (পূর্ণাঙ্ক)(p - buffer));
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
    স্থির শূন্য
nb_parse_cmd(char_u *cmd)
{
    বর্ণ	*verb;
    বর্ণ	*q;
    পূর্ণাঙ্ক		bufno;
    পূর্ণাঙ্ক		isfunc = -1;

    যদি (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    যদি (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_column = false;

	// The IDE is breaking the connection.
	netbeans_close();
	প্রত্যাগম;
    }

    bufno = strtol((বর্ণ *)cmd, &verb, 10);

    যদি (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	প্রত্যাগম;
    }
    ++verb; // skip colon

    প্রতি (q = verb; *q; q++)
    {
	যদি (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    ভঙ্গ;
	}
	অন্যথা যদি (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    ভঙ্গ;
	}
    }

    যদি (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_in_str), cmd);
	প্রত্যাগম;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (বর্ণ *)skipwhite((char_u *)q);

    যদি (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
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

সংরচনা nbbuf_struct
{
    buf_T		*bufp;
    unsigned পূর্ণাঙ্ক	 fireChanges:1;
    unsigned পূর্ণাঙ্ক	 initDone:1;
    unsigned পূর্ণাঙ্ক	 ঢোকাও_Done:1;
    unsigned পূর্ণাঙ্ক	 modified:1;
    পূর্ণাঙ্ক			 nbbuf_সংখ্যা;
    বর্ণ		*displayname;
    পূর্ণাঙ্ক			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

প্রকারনির্দেশ সংরচনা nbbuf_struct nbbuf_T;

স্থির nbbuf_T *buf_list = রিক্ত;
স্থির পূর্ণাঙ্ক buf_list_size = 0;	// size of buf_list
স্থির পূর্ণাঙ্ক buf_list_used = 0;	// nr of entries in buf_list actually in use

স্থির বর্ণ **globalsignmap = রিক্ত;
স্থির পূর্ণাঙ্ক globalsignmaplen = 0;
স্থির পূর্ণাঙ্ক globalsignmapused = 0;

স্থির পূর্ণাঙ্ক  mapsigntype(nbbuf_T *, পূর্ণাঙ্ক localsigntype);
স্থির শূন্য addsigntype(nbbuf_T *, পূর্ণাঙ্ক localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
স্থির শূন্য print_read_বার্তা(nbbuf_T *buf);
স্থির শূন্য print_save_বার্তা(nbbuf_T *buf, off_T nchars);

স্থির পূর্ণাঙ্ক curPCtype = -1;

/*
 * Free netbeans resources.
 */
    স্থির শূন্য
nb_free(শূন্য)
{
    চাবি_Q_T *চাবি_গ্রন্থি = চাবি_Head.next;
    nbbuf_T buf;
    পূর্ণাঙ্ক i;

    // free the netbeans buffer list
    প্রতি (i = 0; i < buf_list_used; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	যদি (buf.bufp != রিক্ত && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_size = 0;
    buf_list_used = 0;

    // free the queued key commands
    যাবত্ (চাবি_গ্রন্থি != রিক্ত && চাবি_গ্রন্থি != &চাবি_Head)
    {
	চাবি_Q_T *next = চাবি_গ্রন্থি->next;
	vim_free(চাবি_গ্রন্থি->keystr);
	vim_free(চাবি_গ্রন্থি);
	যদি (next == &চাবি_Head)
	{
	    চাবি_Head.next = &চাবি_Head;
	    চাবি_Head.prev = &চাবি_Head;
	    ভঙ্গ;
	}
	চাবি_গ্রন্থি = next;
    }

    // free the queued netbeans commands
    যদি (nb_channel != রিক্ত)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    স্থির পূর্ণাঙ্ক
nb_getbufno(buf_T *bufp)
{
    পূর্ণাঙ্ক i;

    প্রতি (i = 0; i < buf_list_used; i++)
	যদি (buf_list[i].bufp == bufp)
	    প্রত্যাগম i;
    প্রত্যাগম -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    পূর্ণাঙ্ক
isNetbeansBuffer(buf_T *bufp)
{
    প্রত্যাগম NETBEANS_OPEN && bufp->b_netbeans_file;
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
    পূর্ণাঙ্ক
isNetbeansModified(buf_T *bufp)
{
    যদি (isNetbeansBuffer(bufp))
    {
	পূর্ণাঙ্ক bufno = nb_getbufno(bufp);

	যদি (bufno > 0)
	    প্রত্যাগম buf_list[bufno].modified;
	অন্যথা
	    প্রত্যাগম FALSE;
    }
    অন্যথা
	প্রত্যাগম FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    স্থির nbbuf_T *
nb_get_buf(পূর্ণাঙ্ক bufno)
{
    // find or create a buffer with the given number
    পূর্ণাঙ্ক incr;

    যদি (bufno <= 0)
	প্রত্যাগম রিক্ত;

    যদি (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * পরিমাণ(nbbuf_T));
	buf_list_size = 100;
    }
    যদি (bufno >= buf_list_used) // new
    {
	যদি (bufno >= buf_list_size) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_size + 90;
	    buf_list_size += incr;
	    bufsize = buf_list_size * পরিমাণ(nbbuf_T);
	    যদি (bufsize == 0 || bufsize / পরিমাণ(nbbuf_T)
						      != (size_t)buf_list_size)
	    {
		// list size overflow, bail out
		প্রত্যাগম রিক্ত;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    যদি (buf_list == রিক্ত)
	    {
		vim_free(t_buf_list);
		buf_list_size = 0;
		প্রত্যাগম রিক্ত;
	    }
	    vim_memset(buf_list + buf_list_size - incr, 0,
						      incr * পরিমাণ(nbbuf_T));
	}

	যাবত্ (buf_list_used <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_used].fireChanges = 1;
	    ++buf_list_used;
	}
    }

    প্রত্যাগম buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    স্থির পূর্ণাঙ্ক
গুনতি_changed_buffers(শূন্য)
{
    buf_T	*bufp;
    পূর্ণাঙ্ক		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	যদি (bufp->b_changed)
	    ++n;
    প্রত্যাগম n;
}

/*
 * End the netbeans session.
 */
    শূন্য
netbeans_end(শূন্য)
{
    পূর্ণাঙ্ক i;
    স্থির বর্ণ buf[128];

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    প্রতি (i = 0; i < buf_list_used; i++)
    {
	যদি (!buf_list[i].bufp)
	    অনুবর্ত;
	যদি (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, রিক্ত);
	buf_list[i].bufp = রিক্ত;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    স্থির শূন্য
nb_send(বর্ণ *buf, বর্ণ *fun)
{
    যদি (nb_channel != রিক্ত)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (পূর্ণাঙ্ক)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    স্থির শূন্য
nb_reply_nil(পূর্ণাঙ্ক cmdno)
{
    বর্ণ reply[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    sprintf(reply, "%d\n", cmdno);
    nb_send(reply, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    স্থির শূন্য
nb_reply_text(পূর্ণাঙ্ক cmdno, char_u *result)
{
    char_u *reply;

    nbdebug(("REP %d: %s\n", cmdno, (বর্ণ *)result));

    reply = alloc(STRLEN(result) + 32);
    sprintf((বর্ণ *)reply, "%d %s\n", cmdno, (বর্ণ *)result);
    nb_send((বর্ণ *)reply, "nb_reply_text");

    vim_free(reply);
}


/*
 * Send a response with a number result code.
 */
    স্থির শূন্য
nb_reply_nr(পূর্ণাঙ্ক cmdno, long result)
{
    বর্ণ reply[32];

    nbdebug(("REP %d: %ld\n", cmdno, result));

    sprintf(reply, "%d %ld\n", cmdno, result);
    nb_send(reply, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    স্থির char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    যদি (buf == রিক্ত)
	প্রত্যাগম রিক্ত;
    প্রতি (; *p; p++)
    {
	বিকল্প (*p)
	{
	    পক্ষ '\"':
	    পক্ষ '\\':
		*q++ = '\\'; *q++ = *p; ভঙ্গ;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    পক্ষ '\n':
		*q++ = '\\'; *q++ = 'n'; ভঙ্গ;
	    পক্ষ '\r':
		*q++ = '\\'; *q++ = 'r'; ভঙ্গ;
	    স্বভাব:
		*q++ = *p;
		ভঙ্গ;
	}
    }
    *q = '\0';

    প্রত্যাগম buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    স্থির বর্ণ *
nb_unquote(char_u *p, char_u **endp)
{
    বর্ণ *result = 0;
    বর্ণ *q;
    পূর্ণাঙ্ক done = 0;

    // result is never longer than input
    result = alloc_clear(STRLEN(p) + 1);
    যদি (result == রিক্ত)
	প্রত্যাগম রিক্ত;

    যদি (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	result[0] = NUL;
	প্রত্যাগম result;
    }

    প্রতি (q = result; !done && *p != NUL;)
    {
	বিকল্প (*p)
	{
	    পক্ষ '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		done = 1;
		ভঙ্গ;

	    পক্ষ '\\':
		++p;
		বিকল্প (*p)
		{
		    পক্ষ '\\':	*q++ = '\\';	ভঙ্গ;
		    পক্ষ 'n':	*q++ = '\n';	ভঙ্গ;
		    পক্ষ 't':	*q++ = '\t';	ভঙ্গ;
		    পক্ষ 'r':	*q++ = '\r';	ভঙ্গ;
		    পক্ষ '"':	*q++ = '"';	ভঙ্গ;
		    পক্ষ NUL:	--p;		ভঙ্গ;
		    // default: skip over illegal chars
		}
		++p;
		ভঙ্গ;

	    স্বভাব:
		*q++ = *p++;
	}
    }

    যদি (endp != রিক্ত)
	*endp = p;

    প্রত্যাগম result;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    স্থির শূন্য
nb_partialremove(linenr_T lnum, colnr_T first, colnr_T last)
{
    char_u *oldtext, *newtext;
    পূর্ণাঙ্ক oldlen;
    পূর্ণাঙ্ক lastbyte = last;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    যদি (first >= (colnr_T)oldlen || oldlen == 0)  // just in case
	প্রত্যাগম;
    যদি (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - first));
    যদি (newtext == রিক্ত)
	প্রত্যাগম;

    mch_memmove(newtext, oldtext, first);
    STRMOVE(newtext + first, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    স্থির শূন্য
nb_joinlines(linenr_T first, linenr_T other)
{
    পূর্ণাঙ্ক len_first, len_other;
    char_u *p;

    len_first = ml_get_len(first);
    len_other = ml_get_len(other);
    p = alloc(len_first + len_other + 1);
    যদি (p == রিক্ত)
	প্রত্যাগম;

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
    স্থির পূর্ণাঙ্ক
nb_do_cmd(
    পূর্ণাঙ্ক		bufno,
    char_u	*cmd,
    পূর্ণাঙ্ক		func,
    পূর্ণাঙ্ক		cmdno,
    char_u	*args)	    // points to space before arguments or NUL
{
    পূর্ণাঙ্ক		do_update = 0;
    long	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    স্থির পূর্ণাঙ্ক	skip = 0;
    পূর্ণাঙ্ক		retval = OK;
    বর্ণ	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (বর্ণ *)args));

    যদি (func)
    {
// =====================================================================
	যদি (streq((বর্ণ *)cmd, "getModified"))
	{
	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
		// Return the number of buffers that are modified.
		nb_reply_nr(cmdno, (long)গুনতি_changed_buffers());
	    অন্যথা
		// Return whether the buffer is modified.
		nb_reply_nr(cmdno, (long)(buf->bufp->b_changed
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_reply_nr(cmdno, (long)গুনতি_changed_buffers());
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "getCursor"))
	{
	    char_u text[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((বর্ণ *)text, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (long)curwin->w_cursor.lnum,
		    (পূর্ণাঙ্ক)curwin->w_cursor.col,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_reply_text(cmdno, text);
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "getAnno"))
	{
	    long linenum = 0;
#ifdef FEAT_SIGNS
	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_getanno));
		retval = FAIL;
	    }
	    অন্যথা
	    {
		পূর্ণাঙ্ক ser_অঙ্ক;

		cp = (বর্ণ *)args;
		ser_অঙ্ক = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (long)buf_findsign(buf->bufp, ser_অঙ্ক, রিক্ত);
	    }
#endif
	    nb_reply_nr(cmdno, linenum);
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "getLength"))
	{
	    long len = 0;

	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_buffer_identifier_in_getlength));
		retval = FAIL;
	    }
	    অন্যথা
	    {
		len = get_buf_size(buf->bufp);
	    }
	    nb_reply_nr(cmdno, len);
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "getText"))
	{
	    long	len;
	    linenr_T	nlines;
	    char_u	*text = রিক্ত;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*সারি;

	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_buffer_identifier_in_gettext));
		retval = FAIL;
	    }
	    অন্যথা
	    {
		len = get_buf_size(buf->bufp);
		nlines = buf->bufp->b_ml.ml_সারি_গুনতি;
		text = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		যদি (text == রিক্ত)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		অন্যথা
		{
		    p = text;
		    *p++ = '\"';
		    প্রতি (; lno <= nlines ; lno++)
		    {
			সারি = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			যদি (সারি != রিক্ত)
			{
			    STRCPY(p, সারি);
			    p += STRLEN(সারি);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(সারি);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    যদি (text == রিক্ত)
		nb_reply_text(cmdno, (char_u *)"");
	    অন্যথা
	    {
		nb_reply_text(cmdno, text);
		vim_free(text);
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "remove"))
	{
	    long গুনতি;
	    pos_T first, last;
	    pos_T *pos;
	    pos_T *next;
	    linenr_T del_from_lnum, del_to_lnum;  // lines to be deleted as a whole
	    পূর্ণাঙ্ক oldFire = netbeansFireChanges;
	    পূর্ণাঙ্ক oldSuppress = netbeans_Suppress_No_সারিগুলি;
	    পূর্ণাঙ্ক wasChanged;

	    যদি (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (বর্ণ *) cmd));
		nb_reply_nil(cmdno);
		প্রত্যাগম OK;
	    }

	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_buffer_identifier_in_remove));
		retval = FAIL;
	    }
	    অন্যথা
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_সারিগুলি = TRUE;

		nb_set_curbuf(buf->bufp);
		wasChanged = buf->bufp->b_changed;
		cp = (বর্ণ *)args;
		off = strtol(cp, &cp, 10);
		গুনতি = strtol(cp, &cp, 10);
		args = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		যদি (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_সারিগুলি = oldSuppress;
		    প্রত্যাগম FAIL;
		}
		first = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      first.lnum, first.col));
		pos = off2pos(buf->bufp, off+গুনতি-1);
		যদি (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_সারিগুলি = oldSuppress;
		    প্রত্যাগম FAIL;
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
		next = off2pos(buf->bufp, off + গুনতি);

		// Remove part of the first line.
		যদি (first.col != 0
				|| (next != রিক্ত && first.lnum == next->lnum))
		{
		    যদি (first.lnum != last.lnum
			    || (next != রিক্ত && first.lnum != next->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(first.lnum, first.col,
							     (colnr_T)MAXCOL);
			যদি (first.lnum == last.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(first.lnum, next->lnum);
			    del_to_lnum = next->lnum;
			}
		    }
		    অন্যথা
		    {
			// remove within one line
			nb_partialremove(first.lnum, first.col, last.col);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		যদি (first.lnum != last.lnum && next != রিক্ত
			&& next->col != 0 && last.lnum == next->lnum)
		{
		    nb_partialremove(last.lnum, 0, last.col);
		    যদি (del_from_lnum > first.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(first.lnum, last.lnum);
		    }
		    অন্যথা
			// First line is deleted as a whole, keep the last
			// line.
			--del_to_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		যদি (first.lnum != last.lnum && del_from_lnum > first.lnum
			&& next != রিক্ত && last.lnum != next->lnum)
		{
		    nb_joinlines(first.lnum, next->lnum);
		    del_to_lnum = next->lnum;
		}

		// Delete whole lines if there are any.
		যদি (del_to_lnum >= del_from_lnum)
		{
		    পূর্ণাঙ্ক i;

		    // delete signs from the lines being deleted
		    প্রতি (i = del_from_lnum; i <= del_to_lnum; i++)
		    {
			পূর্ণাঙ্ক id = buf_findsign_id(buf->bufp, (linenr_T)i, রিক্ত);
			যদি (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, রিক্ত);
			}
			অন্যথা
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_to_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.col = 0;
		    del_সারিগুলি(del_to_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = first;
		check_cursor_lnum();
		buf->bufp->b_changed = wasChanged; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_সারিগুলি = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_reply_nil(cmdno);
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "insert"))
	{
	    char_u	*to_free;

	    যদি (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (বর্ণ *) cmd));
		nb_reply_nil(cmdno);
		প্রত্যাগম OK;
	    }

	    // get offset
	    cp = (বর্ণ *)args;
	    off = strtol(cp, &cp, 10);
	    args = (char_u *)cp;

	    // get text to be inserted
	    args = skipwhite(args);
	    args = to_free = (char_u *)nb_unquote(args, রিক্ত);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_buffer_identifier_in_ঢোকাও));
		retval = FAIL;
	    }
	    অন্যথা যদি (args != রিক্ত)
	    {
		পূর্ণাঙ্ক	ff_detected = EOL_UNKNOWN;
		পূর্ণাঙ্ক	buf_was_empty = (buf->bufp->b_ml.ml_flags & ML_EMPTY);
		size_t	len = 0;
		পূর্ণাঙ্ক	added = 0;
		পূর্ণাঙ্ক	oldFire = netbeansFireChanges;
		পূর্ণাঙ্ক	old_b_changed;
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
		যদি (pos != রিক্ত)
		{
		    যদি (pos->lnum <= 0)
			lnum_start = 1;
		    অন্যথা
			lnum_start = pos->lnum;
		}
		অন্যথা
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    যদি (buf_was_empty)
			lnum_start = 1;	    // above empty line
		    অন্যথা
			lnum_start = curbuf->b_ml.ml_সারি_গুনতি + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_start;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		যাবত্ (*args != NUL)
		{
		    nlp = vim_strchr(args, '\n');
		    যদি (nlp == রিক্ত)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(args);
		    }
		    অন্যথা
		    {
			len = nlp - args;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			যদি (nlp > args && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			অন্যথা
			    ff_detected = EOL_UNIX;
		    }
		    args[len] = NUL;

		    যদি (lnum == lnum_start
			    && ((pos != রিক্ত && pos->col > 0)
				|| (lnum == 1 && buf_was_empty)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			পূর্ণাঙ্ক	col = pos == রিক্ত ? 0 : pos->col;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			যদি (newline != রিক্ত)
			{
			    mch_memmove(newline, oldline, (size_t)col);
			    newline[col] = NUL;
			    STRCAT(newline, args);
			    STRCAT(newline, oldline + col);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    অন্যথা
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), args,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    যদি (nlp == রিক্ত)
			ভঙ্গ;
		    ++lnum;
		    args = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_সারিগুলি_mark(lnum_start - 1, (long)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		যদি (buf_was_empty)
		{
		    যদি (ff_detected == EOL_UNKNOWN)
#যদি defined(MSWIN)
			ff_detected = EOL_DOS;
#অন্যথা
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
	অন্যথা
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_reply_nil(cmdno);
	    retval = FAIL;
	}
    }
    অন্যথা // Not a function; no reply required.
    {
// =====================================================================
	যদি (streq((বর্ণ *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    যদি (buf == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_buffer_identifier_in_create));
		প্রত্যাগম FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, রিক্ত, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->ঢোকাও_Done = FALSE;
#যদি defined(FEAT_MENU) && defined(FEAT_GUI)
	    যদি (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "insertDone"))
	{
	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    অন্যথা
	    {
		buf->bufp->b_start_eol = *args == 'T';
		buf->ঢোকাও_Done = TRUE;
		args += 2;
		buf->bufp->b_p_ro = *args == 'T';
		print_read_বার্তা(buf);
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "saveDone"))
	{
	    long savedChars = atol((বর্ণ *)args);

	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    অন্যথা
		print_save_বার্তা(buf, savedChars);
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "startDocumentListen"))
	{
	    যদি (buf == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_startdocumentlisten));
		প্রত্যাগম FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "stopDocumentListen"))
	{
	    যদি (buf == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_stopdocumentlisten));
		প্রত্যাগম FAIL;
	    }
	    buf->fireChanges = 0;
	    যদি (buf->bufp != রিক্ত && buf->bufp->b_was_netbeans_file)
	    {
		যদি (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_lost_for_buffer_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_lost_for_buffer_nr),
							   buf->bufp->b_fnum);
		}
		অন্যথা
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
	অন্যথা যদি (streq((বর্ণ *)cmd, "setTitle"))
	{
	    যদি (buf == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_buffer_identifier_in_settitle));
		প্রত্যাগম FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, রিক্ত);
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "initDone"))
	{
	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_buffer_identifier_in_initdone));
		প্রত্যাগম FAIL;
	    }
	    do_update = 1;
	    buf->initDone = TRUE;
	    nb_set_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_চাবি_queue();
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "setBufferNumber")
		|| streq((বর্ণ *)cmd, "putBufferNumber"))
	{
	    char_u	*path;
	    buf_T	*bufp;

	    যদি (buf == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_buffer_identifier_in_setbuffernumber));
		প্রত্যাগম FAIL;
	    }
	    path = (char_u *)nb_unquote(args, রিক্ত);
	    যদি (path == রিক্ত)
		প্রত্যাগম FAIL;
	    bufp = buflist_findname(path);
	    vim_free(path);
	    যদি (bufp == রিক্ত)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", args));
		semsg(_(e_file_str_not_found_in_setbuffernumber), args);
		প্রত্যাগম FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_সংখ্যা = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    যদি (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    অন্যথা
	    {
		buf->initDone = TRUE;

		// handle any postponed key commands
		handle_চাবি_queue();
	    }

// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "setFullName"))
	{
	    যদি (buf == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_buffer_identifier_in_setfullname));
		প্রত্যাগম FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, রিক্ত);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#যদি defined(FEAT_MENU) && defined(FEAT_GUI)
	    যদি (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "editFile"))
	{
	    যদি (buf == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_buffer_identifier_in_editfile));
		প্রত্যাগম FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, রিক্ত);
	    do_ecmd(0, (char_u *)buf->displayname, রিক্ত, রিক্ত, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->initDone = TRUE;
	    do_update = 1;
	    maketitle();
#যদি defined(FEAT_MENU) && defined(FEAT_GUI)
	    যদি (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "setVisible"))
	{
	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		যদি (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setvisible));
		প্রত্যাগম FAIL;
	    }
	    যদি (streq((বর্ণ *)args, "T") && buf->bufp != curbuf)
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
		যদি (gui.in_use)
		    gui_mch_set_foreground();
#endif
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "raise"))
	{
#ifdef FEAT_GUI
	    // Bring gvim to the foreground.
	    যদি (gui.in_use)
		gui_mch_set_foreground();
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "setModified"))
	{
	    পূর্ণাঙ্ক prev_b_changed;

	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		যদি (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setmodified));
		প্রত্যাগম FAIL;
	    }
	    prev_b_changed = buf->bufp->b_changed;
	    যদি (streq((বর্ণ *)args, "T"))
		buf->bufp->b_changed = TRUE;
	    অন্যথা
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		যদি (buf->bufp->b_ffname != রিক্ত
			&& mch_stat((বর্ণ *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_time(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_changed = FALSE;
	    }
	    buf->modified = buf->bufp->b_changed;
	    যদি (prev_b_changed != buf->bufp->b_changed)
	    {
		check_status(buf->bufp);
		redraw_tabline = TRUE;
#যদি defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "setModtime"))
	{
	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    অন্যথা
	    {
		buf->bufp->b_mtime = atoi((বর্ণ *)args);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "setReadOnly"))
	{
	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    অন্যথা যদি (streq((বর্ণ *)args, "T"))
		buf->bufp->b_p_ro = TRUE;
	    অন্যথা
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "showBalloon"))
	{
#যদি defined(FEAT_BEVAL_GUI)
	    স্থির বর্ণ	*text = রিক্ত;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    যদি (balloonEval != রিক্ত)
	    {
		vim_free(text);
		text = nb_unquote(args, রিক্ত);
		যদি (text != রিক্ত)
		    gui_mch_post_balloon(balloonEval, (char_u *)text);
	    }
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "setDot"))
	{
	    pos_T *pos;
#ifdef NBDEBUG
	    char_u *s;
#endif

	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_buffer_identifier_in_setdot));
		প্রত্যাগম FAIL;
	    }

	    nb_set_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    যদি (VIsual_active)
		end_visual_mode();
#ifdef NBDEBUG
	    s = args;
#endif
	    pos = get_off_or_lnum(buf->bufp, &args);
	    যদি (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#ifdef FEAT_FOLDING
		foldOpenCursor();
#endif
	    }
	    অন্যথা
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
	    যদি (State == MODE_HITRETURN || State == MODE_ASKMORE)
	    {
#ifdef FEAT_GUI_GTK
		যদি (gui.in_use && gtk_main_level() > 0)
		    gtk_main_quit();
#endif
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "close"))
	{
#ifdef NBDEBUG
	    বর্ণ *name = "<NONE>";
#endif

	    যদি (buf == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_buffer_identifier_in_close));
		প্রত্যাগম FAIL;
	    }

#ifdef NBDEBUG
	    যদি (buf->displayname != রিক্ত)
		name = buf->displayname;
#endif
	    যদি (buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		যদি (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    যদি (buf->bufp != রিক্ত)
		do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = রিক্ত;
	    buf->initDone = FALSE;
	    do_update = 1;
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "defineAnnoType"))
	{
#ifdef FEAT_SIGNS
	    পূর্ণাঙ্ক type_অঙ্ক;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    পূর্ণাঙ্ক parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    যদি (buf == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_buffer_identifier_in_defineannotype));
		প্রত্যাগম FAIL;
	    }

	    cp = (বর্ণ *)args;
	    type_অঙ্ক = strtol(cp, &cp, 10);
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
	    যদি (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(args);
	    bg = vim_strsave(p);
	    যদি (STRLEN(fg) > MAX_COLOR_LENGTH || STRLEN(bg) > MAX_COLOR_LENGTH)
	    {
		emsg(_(e_highlighting_color_name_too_long_in_defineAnnoType));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    অন্যথা যদি (!nb_is_safe_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_safe_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_safe_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_in_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    অন্যথা যদি (typeName != রিক্ত && tooltip != রিক্ত && glyphFile != রিক্ত)
		addsigntype(buf, type_অঙ্ক, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    যদি (parse_error)
		প্রত্যাগম FAIL;

#endif
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "addAnno"))
	{
#ifdef FEAT_SIGNS
	    পূর্ণাঙ্ক ser_অঙ্ক;
	    পূর্ণাঙ্ক local_Type_অঙ্ক;
	    পূর্ণাঙ্ক type_অঙ্ক;
	    pos_T *pos;

	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_addanno));
		প্রত্যাগম FAIL;
	    }

	    do_update = 1;

	    cp = (বর্ণ *)args;
	    ser_অঙ্ক = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_অঙ্ক = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    type_অঙ্ক = mapsigntype(buf, local_Type_অঙ্ক);

	    pos = get_off_or_lnum(buf->bufp, &args);

	    cp = (বর্ণ *)args;
	    vim_ignored = (পূর্ণাঙ্ক)strtol(cp, &cp, 10);
	    args = (char_u *)cp;
# ifdef NBDEBUG
	    যদি (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    যদি (ser_অঙ্ক >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		প্রত্যাগম FAIL;
	    }
	    যদি (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_অঙ্ক, pos->lnum, type_অঙ্ক, buf->bufp->b_fnum);
		যদি (type_অঙ্ক == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_অঙ্ক,
						       buf->bufp->b_fnum);
	    }
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "removeAnno"))
	{
#ifdef FEAT_SIGNS
	    পূর্ণাঙ্ক ser_অঙ্ক;

	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		প্রত্যাগম FAIL;
	    }
	    do_update = 1;
	    cp = (বর্ণ *)args;
	    ser_অঙ্ক = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_অঙ্ক, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "moveAnnoToFront"))
	{
#ifdef FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "guard") || streq((বর্ণ *)cmd, "unguard"))
	{
	    পূর্ণাঙ্ক len;
	    pos_T first;
	    pos_T last;
	    pos_T *pos;
	    পূর্ণাঙ্ক un = (cmd[0] == 'u');
	    স্থির পূর্ণাঙ্ক guardId = GUARDEDOFFSET;

	    যদি (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (বর্ণ *) cmd));
		প্রত্যাগম OK;
	    }

	    nb_init_graphics();

	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		প্রত্যাগম FAIL;
	    }
	    nb_set_curbuf(buf->bufp);
	    cp = (বর্ণ *)args;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, রিক্ত, 10);
	    args = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    যদি (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    অন্যথা
	    {
		first = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		যদি (pos != রিক্ত && pos->col == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		যদি (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		অন্যথা
		{
		    long lnum;
		    last = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     first.lnum, first.col,
			     last.lnum, last.col));
#ifdef FEAT_SIGNS
		    প্রতি (lnum = first.lnum; lnum <= last.lnum; lnum++)
		    {
			যদি (un)
			{
			    // never used
			}
			অন্যথা
			{
			    যদি (buf_findsigntype_id(buf->bufp, lnum,
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
	অন্যথা যদি (streq((বর্ণ *)cmd, "startAtomic"))
	{
	    inAtomic = 1;
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "endAtomic"))
	{
	    inAtomic = 0;
	    যদি (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		প্রত্যাগম FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    যদি (bufIsChanged(buf->bufp))
	    {
		// Only write if the buffer can be written.
		যদি (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != রিক্ত
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    যদি (!bufref_valid(&bufref))
			buf->bufp = রিক্ত;
		}
	    }
	    অন্যথা
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "netbeansBuffer"))
	{
	    যদি (buf == রিক্ত || buf->bufp == রিক্ত)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		প্রত্যাগম FAIL;
	    }
	    যদি (*args == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    অন্যথা
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "specialKeys"))
	{
	    special_চাবিগুলি(args);
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	অন্যথা যদি (streq((বর্ণ *)cmd, "version"))
	{
	    // not used yet
	}
	অন্যথা
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    যদি (inAtomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    যদি (buf != রিক্ত && buf->initDone && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	out_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	যদি (State == MODE_HITRETURN || State == MODE_ASKMORE)
	{
#ifdef FEAT_GUI_GTK
	    যদি (gui.in_use && gtk_main_level() > 0)
		gtk_main_quit();
#endif
	}
    }

    প্রত্যাগম retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    স্থির শূন্য
nb_set_curbuf(buf_T *buf)
{
    যদি (curbuf == buf)
	প্রত্যাগম;

    যদি (buf_jump_open_win(buf) != রিক্ত)
	প্রত্যাগম;
    যদি ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != রিক্ত)
	প্রত্যাগম;
    set_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    স্থির শূন্য
coloncmd(বর্ণ *cmd, ...)
{
    বর্ণ buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, পরিমাণ(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, রিক্ত, রিক্ত, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    out_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    স্থির শূন্য
special_চাবিগুলি(char_u *args)
{
    বর্ণ *save_str = nb_unquote(args, রিক্ত);
    বর্ণ *tok = strtok(save_str, " ");
    বর্ণ *sep;
#define KEYBUFLEN 64
    বর্ণ keybuf[KEYBUFLEN];
    বর্ণ cmdbuf[256];

    যাবত্ (tok != রিক্ত)
    {
	পূর্ণাঙ্ক i = 0;

	যদি ((sep = strchr(tok, '-')) != রিক্ত)
	{
	    *sep = NUL;
	    যাবত্ (*tok && i + 2 < KEYBUFLEN)
	    {
		বিকল্প (*tok)
		{
		    পক্ষ 'A':
		    পক্ষ 'M':
		    পক্ষ 'C':
		    পক্ষ 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			ভঙ্গ;
		}
		tok++;
	    }
	    tok++;
	}

	যদি (strlen(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    পূর্ণাঙ্ক safe = TRUE;
	    প্রতি (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		যদি (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    safe = FALSE;
		    ভঙ্গ;
		}
	    }
	    যদি (safe)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, পরিমাণ(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, MODE_NORMAL, FALSE);
	    }
	}
	tok = strtok(রিক্ত, " ");
    }
    vim_free(save_str);
}

    শূন্য
ex_nbclose(exarg_T *eap UNUSED)
{
    netbeans_close();
}

    শূন্য
ex_nbkey(exarg_T *eap)
{
    (শূন্য)netbeans_keystring(eap->arg);
}

    শূন্য
ex_nbstart(
    exarg_T	*eap)
{
#ifdef FEAT_GUI
# যদি !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    যদি (gui.in_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	প্রত্যাগম;
    }
# endif
#endif
    netbeans_open((বর্ণ *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    স্থির শূন্য
nb_init_graphics(শূন্য)
{
    স্থির পূর্ণাঙ্ক did_init = FALSE;

    যদি (did_init)
	প্রত্যাগম;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    স্থির শূন্য
netbeans_keyname(পূর্ণাঙ্ক চাবি, বর্ণ *buf)
{
    বর্ণ *name = 0;
    বর্ণ namebuf[2];
    পূর্ণাঙ্ক ctrl  = 0;
    পূর্ণাঙ্ক shift = 0;
    পূর্ণাঙ্ক alt   = 0;

    যদি (mod_mask & MOD_MASK_CTRL)
	ctrl = 1;
    যদি (mod_mask & MOD_MASK_SHIFT)
	shift = 1;
    যদি (mod_mask & MOD_MASK_ALT)
	alt = 1;


    বিকল্প (চাবি)
    {
	পক্ষ K_F1:		name = "F1";		ভঙ্গ;
	পক্ষ K_S_F1:	name = "F1";	shift = 1;	ভঙ্গ;
	পক্ষ K_F2:		name = "F2";		ভঙ্গ;
	পক্ষ K_S_F2:	name = "F2";	shift = 1;	ভঙ্গ;
	পক্ষ K_F3:		name = "F3";		ভঙ্গ;
	পক্ষ K_S_F3:	name = "F3";	shift = 1;	ভঙ্গ;
	পক্ষ K_F4:		name = "F4";		ভঙ্গ;
	পক্ষ K_S_F4:	name = "F4";	shift = 1;	ভঙ্গ;
	পক্ষ K_F5:		name = "F5";		ভঙ্গ;
	পক্ষ K_S_F5:	name = "F5";	shift = 1;	ভঙ্গ;
	পক্ষ K_F6:		name = "F6";		ভঙ্গ;
	পক্ষ K_S_F6:	name = "F6";	shift = 1;	ভঙ্গ;
	পক্ষ K_F7:		name = "F7";		ভঙ্গ;
	পক্ষ K_S_F7:	name = "F7";	shift = 1;	ভঙ্গ;
	পক্ষ K_F8:		name = "F8";		ভঙ্গ;
	পক্ষ K_S_F8:	name = "F8";	shift = 1;	ভঙ্গ;
	পক্ষ K_F9:		name = "F9";		ভঙ্গ;
	পক্ষ K_S_F9:	name = "F9";	shift = 1;	ভঙ্গ;
	পক্ষ K_F10:		name = "F10";		ভঙ্গ;
	পক্ষ K_S_F10:	name = "F10";	shift = 1;	ভঙ্গ;
	পক্ষ K_F11:		name = "F11";		ভঙ্গ;
	পক্ষ K_S_F11:	name = "F11";	shift = 1;	ভঙ্গ;
	পক্ষ K_F12:		name = "F12";		ভঙ্গ;
	পক্ষ K_S_F12:	name = "F12";	shift = 1;	ভঙ্গ;
	স্বভাব:
			যদি (চাবি >= ' ' && চাবি <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = চাবি;
			    namebuf[1] = NUL;
			}
			অন্যথা
			    name = "X";
			ভঙ্গ;
    }

    buf[0] = '\0';
    যদি (ctrl)
	strcat(buf, "C");
    যদি (shift)
	strcat(buf, "S");
    যদি (alt)
	strcat(buf, "M"); // META
    যদি (ctrl || shift || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#যদি defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    শূন্য
netbeans_beval_cb(
	BalloonEval	*beval,
	পূর্ণাঙ্ক		 state UNUSED)
{
    win_T	*wp;
    char_u	*text;
    linenr_T	lnum;
    পূর্ণাঙ্ক		col;
    বর্ণ	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    যদি (!can_use_beval() || !NETBEANS_OPEN)
	প্রত্যাগম;

    যদি (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) != OK)
	প্রত্যাগম;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    যদি (text != রিক্ত && text[0] != NUL && STRLEN(text) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	যদি (buf != রিক্ত)
	{
	    p = nb_quote(text);
	    যদি (p != রিক্ত)
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
    পূর্ণাঙ্ক
netbeans_active(শূন্য)
{
    প্রত্যাগম NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    শূন্য
netbeans_open(বর্ণ *params, পূর্ণাঙ্ক doabort)
{
    বর্ণ *cmd = "0:startupDone=0\n";

    যদি (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	প্রত্যাগম;
    }

    যদি (netbeans_connect(params, doabort) != OK)
	প্রত্যাগম;

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
    শূন্য
netbeans_send_disconnect(শূন্য)
{
    বর্ণ buf[128];

    যদি (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#যদি defined(FEAT_EVAL)
    পূর্ণাঙ্ক
set_ref_in_nb_channel(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক abort = FALSE;
    typval_T tv;

    যদি (nb_channel == রিক্ত)
	প্রত্যাগম FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = set_ref_in_item(&tv, copyID, রিক্ত, রিক্ত, রিক্ত);
    প্রত্যাগম abort;
}
#endif

#যদি defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    শূন্য
netbeans_frame_moved(পূর্ণাঙ্ক new_x, পূর্ণাঙ্ক new_y)
{
    বর্ণ buf[128];

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (পূর্ণাঙ্ক)Columns, (পূর্ণাঙ্ক)Rows, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#endif

/*
 * Tell netbeans the user opened or activated a file.
 */
    শূন্য
netbeans_file_activated(buf_T *bufp)
{
    পূর্ণাঙ্ক bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    বর্ণ    buffer[2*MAXPATHL];
    char_u  *q;

    যদি (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	প্রত্যাগম;

    q = nb_quote(bufp->b_ffname);
    যদি (q == রিক্ত)
	প্রত্যাগম;
    যদি (bp == রিক্ত)
    {
	vim_free(q);
	প্রত্যাগম;
    }

    vim_snprintf(buffer, পরিমাণ(buffer),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (বর্ণ *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    শূন্য
netbeans_file_opened(buf_T *bufp)
{
    পূর্ণাঙ্ক bufno = nb_getbufno(bufp);
    বর্ণ    buffer[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    পূর্ণাঙ্ক	    bnum;

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    q = nb_quote(bufp->b_ffname);
    যদি (q == রিক্ত)
	প্রত্যাগম;
    যদি (bp != রিক্ত)
	bnum = bufno;
    অন্যথা
	bnum = 0;

    vim_snprintf(buffer, পরিমাণ(buffer), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (বর্ণ *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
    যদি (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	last_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    শূন্য
netbeans_file_killed(buf_T *bufp)
{
    পূর্ণাঙ্ক		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    বর্ণ	buffer[2*MAXPATHL];

    যদি (!NETBEANS_OPEN || bufno == -1)
	প্রত্যাগম;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(buffer, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_killed");

    যদি (nbbuf != রিক্ত)
	nbbuf->bufp = রিক্ত;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    স্থির nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, পূর্ণাঙ্ক *bufnop)
{
    পূর্ণাঙ্ক		bufno;
    nbbuf_T	*nbbuf;

    যদি (!NETBEANS_OPEN || !netbeansFireChanges)
	প্রত্যাগম রিক্ত;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    যদি (bufno <= 0)
	প্রত্যাগম রিক্ত;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    যদি (nbbuf != রিক্ত && !nbbuf->fireChanges)
	প্রত্যাগম রিক্ত;		// changes in this buffer are not reported

    *bufnop = bufno;
    প্রত্যাগম nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    শূন্য
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    char_u	*txt,
    পূর্ণাঙ্ক		newlen)
{
    char_u	*buf;
    পূর্ণাঙ্ক		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;
    char_u	*p;
    char_u	*newtxt;

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    যদি (nbbuf == রিক্ত)
	প্রত্যাগম;

    // Don't mark as modified for initial read
    যদি (nbbuf->ঢোকাও_Done)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.col = col;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    যদি (p != রিক্ত)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((বর্ণ *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((বর্ণ *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    শূন্য
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    long	len)
{
    char_u	buf[128];
    পূর্ণাঙ্ক		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    যদি (nbbuf == রিক্ত)
	প্রত্যাগম;

    যদি (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	প্রত্যাগম;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.col = col;

    off = pos2off(bufp, &pos);

    sprintf((বর্ণ *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((বর্ণ *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    শূন্য
netbeans_unmodified(buf_T *bufp UNUSED)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    শূন্য
netbeans_button_release(পূর্ণাঙ্ক button)
{
    বর্ণ	buf[128];
    পূর্ণাঙ্ক		bufno;

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    bufno = nb_getbufno(curbuf);

    যদি (bufno < 0 || curwin == রিক্ত || curwin->w_buffer != curbuf)
	প্রত্যাগম;

    পূর্ণাঙ্ক col = mouse_col - curwin->w_wincol
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
    পূর্ণাঙ্ক
netbeans_keycommand(পূর্ণাঙ্ক চাবি)
{
    বর্ণ	চাবি_Name[60];

    netbeans_keyname(চাবি, চাবি_Name);
    প্রত্যাগম netbeans_keystring((char_u *)চাবি_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    স্থির পূর্ণাঙ্ক
netbeans_keystring(char_u *চাবি_Name)
{
    বর্ণ	buf[2*MAXPATHL];
    পূর্ণাঙ্ক		bufno = nb_getbufno(curbuf);
    long	off;
    char_u	*q;

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম TRUE;

    যদি (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == রিক্ত ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	যদি (q == রিক্ত)
	    প্রত্যাগম TRUE;
	vim_snprintf(buf, পরিমাণ(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	যদি (curbuf->b_ffname != রিক্ত)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(চাবি_Name);
	প্রত্যাগম FALSE;
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
    vim_snprintf(buf, পরিমাণ(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, চাবি_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, পরিমাণ(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, চাবি_Name,
		off, (long)curwin->w_cursor.lnum, (long)curwin->w_cursor.col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    প্রত্যাগম TRUE;
}


/*
 * Send a save event to netbeans.
 */
    শূন্য
netbeans_save_buffer(buf_T *bufp)
{
    char_u	buf[64];
    পূর্ণাঙ্ক		bufno;
    nbbuf_T	*nbbuf;

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    যদি (nbbuf == রিক্ত)
	প্রত্যাগম;

    nbbuf->modified = 0;

    sprintf((বর্ণ *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((বর্ণ *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    শূন্য
netbeans_deleted_all_সারিগুলি(buf_T *bufp)
{
    char_u	buf[64];
    পূর্ণাঙ্ক		bufno;
    nbbuf_T	*nbbuf;

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    যদি (nbbuf == রিক্ত)
	প্রত্যাগম;

    // Don't mark as modified for initial read
    যদি (nbbuf->ঢোকাও_Done)
	nbbuf->modified = 1;

    sprintf((বর্ণ *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    পূর্ণাঙ্ক
netbeans_is_guarded(linenr_T শীর্ষ, linenr_T bot)
{
    sign_entry_T	*p;
    পূর্ণাঙ্ক			lnum;

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম FALSE;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
	যদি (p->se_id >= GUARDEDOFFSET)
	    প্রতি (lnum = শীর্ষ + 1; lnum < bot; lnum++)
		যদি (lnum == p->se_lnum)
		    প্রত্যাগম TRUE;

    প্রত্যাগম FALSE;
}

#যদি defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    শূন্য
netbeans_draw_multisign_indicator(পূর্ণাঙ্ক row)
{
    পূর্ণাঙ্ক i;
    পূর্ণাঙ্ক y;
    পূর্ণাঙ্ক x;

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    x = 0;
    y = row * gui.char_height + 2;

    প্রতি (i = 0; i < gui.char_height - 3; i++)
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

#যদি defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    শূন্য
netbeans_draw_multisign_indicator(পূর্ণাঙ্ক row)
{
    পূর্ণাঙ্ক i;
    পূর্ণাঙ্ক y;
    পূর্ণাঙ্ক x;
# যদি GTK_CHECK_VERSION(3,0,0)
    cairo_t *cr = রিক্ত;
# অন্যথা
    GdkDrawable *drawable = gui.drawarea->window;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# অন্যথা
    y = row * gui.char_height + 2;
# endif

# যদি GTK_CHECK_VERSION(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_height);
    cr = cairo_create(surf);
#  অন্যথা
    cr = cairo_create(gui.surface);
#  endif
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    প্রতি (i = 0; i < gui.char_height - 3; i++)
# যদি GTK_CHECK_VERSION(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# অন্যথা
    gdk_draw_point(drawable, gui.text_gc, x+2, y++);
# endif

# যদি GTK_CHECK_VERSION(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# অন্যথা
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

# যদি GTK_CHECK_VERSION(3,0,0)
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
    শূন্য
netbeans_gutter_click(linenr_T lnum)
{
    sign_entry_T	*p;

    যদি (!NETBEANS_OPEN)
	প্রত্যাগম;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
    {
	যদি (p->se_lnum == lnum && p->se_next && p->se_next->se_lnum == lnum)
	{
	    sign_entry_T *tail;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    যদি (p->se_prev)
		p->se_prev->se_next = p->se_next;
	    অন্যথা
		curbuf->b_signlist = p->se_next;
	    p->se_next->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    প্রতি (tail = p->se_next;
		  tail->se_next && tail->se_next->se_lnum == lnum
				       && tail->se_next->se_id < GUARDEDOFFSET;
		  tail = tail->se_next)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_next = tail->se_next;
	    যদি (tail->se_next)
		tail->se_next->se_prev = p;
	    p->se_prev = tail;
	    tail->se_next = p;
	    update_debug_sign(curbuf, lnum);
	    ভঙ্গ;
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
    স্থির শূন্য
addsigntype(
    nbbuf_T	*buf,
    পূর্ণাঙ্ক		type_অঙ্ক,
    char_u	*typeName,
    char_u	*tooltip UNUSED,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    পূর্ণাঙ্ক i, j;
    পূর্ণাঙ্ক use_fg = (*fg && STRCMP(fg, "none") != 0);
    পূর্ণাঙ্ক use_bg = (*bg && STRCMP(bg, "none") != 0);

    প্রতি (i = 0; i < globalsignmapused; i++)
	যদি (STRCMP(typeName, globalsignmap[i]) == 0)
	    ভঙ্গ;

    যদি (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_অঙ্ক, typeName, tooltip, glyphFile, fg, bg));
	যদি (use_fg || use_bg)
	{
	    বর্ণ fgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    বর্ণ bgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    বর্ণ *ptr;
	    পূর্ণাঙ্ক মান;

	    মান = strtol((বর্ণ *)fg, &ptr, 10);
	    যদি (ptr != (বর্ণ *)fg)
		sprintf(fgbuf, "guifg=#%06x", মান & 0xFFFFFF);
	    অন্যথা
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    মান = strtol((বর্ণ *)bg, &ptr, 10);
	    যদি (ptr != (বর্ণ *)bg)
		sprintf(bgbuf, "guibg=#%06x", মান & 0xFFFFFF);
	    অন্যথা
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    যদি (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    অন্যথা যদি (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    অন্যথা
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	অন্যথা
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	যদি (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_অঙ্ক;

	যদি (globalsignmapused == globalsignmaplen)
	{
	    যদি (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(বর্ণ *, globalsignmaplen);
	    }
	    অন্যথা    // grow it
	    {
		পূর্ণাঙ্ক incr;
		পূর্ণাঙ্ক oldlen = globalsignmaplen;
		বর্ণ **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * পরিমাণ(বর্ণ *));
		যদি (globalsignmap == রিক্ত)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    প্রত্যাগম;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * পরিমাণ(বর্ণ *));
	    }
	}

	globalsignmap[i] = (বর্ণ *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    প্রতি (j = 0; j < buf->signmapused; j++)
	যদি (buf->signmap[j] == i + 1)
	    প্রত্যাগম;

    // add to local map
    যদি (buf->signmapused == buf->signmaplen)
    {
	যদি (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(পূর্ণাঙ্ক, buf->signmaplen);
	}
	অন্যথা    // grow it
	{
	    পূর্ণাঙ্ক incr;
	    পূর্ণাঙ্ক oldlen = buf->signmaplen;
	    পূর্ণাঙ্ক *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * পরিমাণ(পূর্ণাঙ্ক));
	    যদি (buf->signmap == রিক্ত)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		প্রত্যাগম;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * পরিমাণ(পূর্ণাঙ্ক));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    স্থির পূর্ণাঙ্ক
mapsigntype(nbbuf_T *buf, পূর্ণাঙ্ক localsigntype)
{
    যদি (--localsigntype >= 0 && localsigntype < buf->signmapused)
	প্রত্যাগম buf->signmap[localsigntype];

    প্রত্যাগম 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    স্থির long
get_buf_size(buf_T *bufp)
{
    linenr_T	lnum;
    long	char_গুনতি = 0;
    পূর্ণাঙ্ক		eol_size;
    long	last_check = 100000L;

    যদি (bufp->b_ml.ml_flags & ML_EMPTY)
	প্রত্যাগম 0;

    যদি (get_fileformat(bufp) == EOL_DOS)
	eol_size = 2;
    অন্যথা
	eol_size = 1;
    প্রতি (lnum = 1; lnum <= bufp->b_ml.ml_সারি_গুনতি; ++lnum)
    {
	char_গুনতি += ml_get_buf_len(bufp, lnum) + eol_size;
	// Check for a CTRL-C every 100000 characters
	যদি (char_গুনতি > last_check)
	{
	    ui_breakcheck();
	    যদি (got_int)
		প্রত্যাগম char_গুনতি;
	    last_check = char_গুনতি + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    যদি (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_গুনতি -= eol_size;

    প্রত্যাগম char_গুনতি;
}

/*
 * Convert character offset to lnum,col
 */
    স্থির pos_T *
off2pos(buf_T *buf, long offset)
{
    linenr_T	 lnum;
    স্থির pos_T pos;

    pos.lnum = 0;
    pos.col = 0;
    pos.coladd = 0;

    যদি (!(buf->b_ml.ml_flags & ML_EMPTY))
    {
	যদি ((lnum = ml_find_সারি_or_offset(buf, (linenr_T)0, &offset)) < 0)
	    প্রত্যাগম রিক্ত;
	pos.lnum = lnum;
	pos.col = offset;
    }

    প্রত্যাগম &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    স্থির pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    স্থির pos_T	mypos;
    long		off;

    off = strtol((বর্ণ *)*argp, (বর্ণ **)argp, 10);
    যদি (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.col = strtol((বর্ণ *)*argp, (বর্ণ **)argp, 10);
	mypos.coladd = 0;
	প্রত্যাগম &mypos;
    }
    প্রত্যাগম off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    স্থির long
pos2off(buf_T *buf, pos_T *pos)
{
    long	 offset = 0;

    যদি (buf->b_ml.ml_flags & ML_EMPTY)
	প্রত্যাগম 0;

    যদি ((offset = ml_find_সারি_or_offset(buf, pos->lnum, 0)) < 0)
	প্রত্যাগম 0;
    offset += pos->col;

    প্রত্যাগম offset;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    স্থির শূন্য
print_read_বার্তা(nbbuf_T *buf)
{
    পূর্ণাঙ্ক	    lnum = buf->bufp->b_ml.ml_সারি_গুনতি;
    off_T   nchars = buf->bufp->b_orig_size;
    char_u  c;

    বার্তা_add_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    যদি (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    যদি (!buf->bufp->b_start_eol)
    {
	STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    বার্তা_add_সারিগুলি(c, (long)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_বার্তা);
    বার্তা_scrolled_ign = TRUE;
    বার্তা_trunc_attr((বর্ণ *)IObuff, FALSE, 0);
    বার্তা_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    স্থির শূন্য
print_save_বার্তা(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    যদি (nchars >= 0)
    {
	// put fname in IObuff with quotes
	বার্তা_add_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	বার্তা_add_সারিগুলি(c, buf->bufp->b_ml.ml_সারি_গুনতি,
						buf->bufp->b_orig_size);

	VIM_CLEAR(keep_বার্তা);
	বার্তা_scrolled_ign = TRUE;
	p = (char_u *)বার্তা_trunc_attr((বর্ণ *)IObuff, FALSE, 0);
	যদি ((বার্তা_scrolled && !need_wait_return) || !buf->initDone)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_বার্তা(p, 0);
	}
	বার্তা_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    অন্যথা
    {
	বর্ণ msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_add_bang_to_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#endif // defined(FEAT_NETBEANS_INTG)
