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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_NETBEANS_INTG)

#ifndef MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#endif

#अन्तर्भाव "version.h"

#define GUARDED		10000 // typenr for "guarded" annotation
#define GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#define MAX_COLOR_LENGTH 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#define VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#define VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
स्थिर वर्ण *ExtEdProtocolVersion = "2.5";

स्थिर long pos2off(buf_T *, pos_T *);
स्थिर pos_T *off2pos(buf_T *, long);
स्थिर pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
स्थिर long get_buf_size(buf_T *);
स्थिर पूर्णाङ्क netbeans_keystring(char_u *keystr);
स्थिर शून्य special_कुंजियाँ(char_u *args);

स्थिर पूर्णाङ्क getConnInfo(वर्ण *file, वर्ण **host, वर्ण **port, वर्ण **password);

स्थिर शून्य nb_init_graphics(शून्य);
स्थिर शून्य coloncmd(वर्ण *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
स्थिर शून्य nb_set_curbuf(buf_T *buf);
स्थिर शून्य nb_parse_cmd(char_u *);
स्थिर पूर्णाङ्क  nb_do_cmd(पूर्णाङ्क, char_u *, पूर्णाङ्क, पूर्णाङ्क, char_u *);
स्थिर शून्य nb_send(वर्ण *buf, वर्ण *fun);
स्थिर शून्य nb_free(शून्य);

#define NETBEANS_OPEN (channel_can_write_to(nb_channel))
स्थिर channel_T *nb_channel = रिक्त;

स्थिर पूर्णाङ्क r_cmdno;			// current command number for reply
स्थिर पूर्णाङ्क dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ifdef NBDEBUG
# include "nbdebug.c"
#endif

स्थिर पूर्णाङ्क needupdate = 0;
स्थिर पूर्णाङ्क inAtomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    स्थिर पूर्णाङ्क
nb_is_safe_string(char_u *str, char_u *allowed)
{
    यदि (str == रिक्त)
	प्रत्यागम FALSE;
    प्रति (char_u *p = str; *p != NUL; p++)
	यदि (vim_strchr(allowed, *p) == रिक्त)
	    प्रत्यागम FALSE;
    प्रत्यागम TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    स्थिर शून्य
nb_channel_closed(शून्य)
{
    nb_channel = रिक्त;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    स्थिर शून्य
netbeans_close(शून्य)
{
    यदि (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	यदि (nb_channel != रिक्त)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = रिक्त;
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

    स्थिर पूर्णाङ्क
netbeans_connect(वर्ण *params, पूर्णाङ्क doabort)
{
    पूर्णाङ्क		port;
    वर्ण	buf[32];
    वर्ण	*hostname = रिक्त;
    वर्ण	*address = रिक्त;
    वर्ण	*password = रिक्त;
    वर्ण	*fname;
    वर्ण	*arg = रिक्त;

    यदि (*params == '=')
    {
	// "=fname": Read info from specified file.
	यदि (getConnInfo(params + 1, &hostname, &address, &password) == FAIL)
	    प्रत्यागम FAIL;
    }
    अन्यथा
    {
	यदि (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	यदि (arg == रिक्त && (fname = getenv("__NETBEANS_CONINFO")) != रिक्त)
	{
	    // "": get info from file specified in environment
	    यदि (getConnInfo(fname, &hostname, &address, &password) == FAIL)
		प्रत्यागम FAIL;
	}
	अन्यथा
	{
	    यदि (arg != रिक्त)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		address = strchr(hostname, ':');
		यदि (address != रिक्त)
		{
		    *address++ = '\0';
		    password = strchr(address, ':');
		    यदि (password != रिक्त)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    यदि (hostname == रिक्त || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    यदि (address == रिक्त)
		address = getenv("__NETBEANS_SOCKET");
	    यदि (password == रिक्त)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    यदि (hostname != रिक्त)
		hostname = (वर्ण *)vim_strsave((char_u *)hostname);
	    यदि (address != रिक्त)
		address = (वर्ण *)vim_strsave((char_u *)address);
	    यदि (password != रिक्त)
		password = (वर्ण *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    यदि (hostname == रिक्त || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (वर्ण *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    यदि (address == रिक्त || *address == '\0')
    {
	vim_free(address);
	address = (वर्ण *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    यदि (password == रिक्त || *password == '\0')
    {
	vim_free(password);
	password = (वर्ण *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    यदि (hostname != रिक्त && address != रिक्त && password != रिक्त)
    {
	port = atoi(address);
	nb_channel = channel_open(hostname, port, 3000, nb_channel_closed);
	यदि (nb_channel != रिक्त)
	{
	    // success
#ifdef FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#endif

	    // success, login
	    vim_snprintf(buf, परिमाण(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", ExtEdProtocolVersion);
	    nb_send(buf, "externaleditor_version");
	}
    }

    यदि (nb_channel == रिक्त && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(address);
    vim_free(password);
    प्रत्यागम NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    स्थिर पूर्णाङ्क
getConnInfo(वर्ण *file, वर्ण **host, वर्ण **port, वर्ण **auth)
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
    यदि (mch_stat(file, &st) == 0 && (st.st_mode & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_mode_for_netbeans_connection_info_file_str),
									file);
	प्रत्यागम FAIL;
    }
#endif

    fp = mch_fopen(file, "r");
    यदि (fp == रिक्त)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	प्रत्यागम FAIL;
    }

    // Read the file. There should be one of each parameter
    यावत् ((lp = (char_u *)fgets((वर्ण *)buf, BUFSIZ, fp)) != रिक्त)
    {
	यदि ((nlp = vim_strchr(lp, '\n')) != रिक्त)
	    *nlp = 0;	    // strip off the trailing newline

	यदि (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (वर्ण *)vim_strsave(&buf[5]);
	}
	अन्यथा यदि (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*port);
	    *port = (वर्ण *)vim_strsave(&buf[5]);
	}
	अन्यथा यदि (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (वर्ण *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    प्रत्यागम OK;
}


संरचना keyqueue
{
    char_u	    *keystr;
    संरचना keyqueue *next;
    संरचना keyqueue *prev;
};

प्रकारनिर्देश संरचना keyqueue कुंजी_Q_T;

स्थिर कुंजी_Q_T कुंजी_Head; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    स्थिर शून्य
postpone_keycommand(char_u *keystr)
{
    कुंजी_Q_T *गाँठ;

    गाँठ = ALLOC_ONE(कुंजी_Q_T);
    यदि (गाँठ == रिक्त)
	प्रत्यागम;  // out of memory, drop the key

    यदि (कुंजी_Head.next == रिक्त) // initialize circular queue
    {
	कुंजी_Head.next = &कुंजी_Head;
	कुंजी_Head.prev = &कुंजी_Head;
    }

    // insert node at tail of queue
    गाँठ->next = &कुंजी_Head;
    गाँठ->prev = कुंजी_Head.prev;
    कुंजी_Head.prev->next = गाँठ;
    कुंजी_Head.prev = गाँठ;

    गाँठ->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    स्थिर शून्य
handle_कुंजी_queue(शून्य)
{
    पूर्णाङ्क postponed = FALSE;

    यावत् (!postponed && कुंजी_Head.next && कुंजी_Head.next != &कुंजी_Head)
    {
	// first, unlink the node
	कुंजी_Q_T *गाँठ = कुंजी_Head.next;
	कुंजी_Head.next = गाँठ->next;
	गाँठ->next->prev = गाँठ->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	यदि (गाँठ->keystr != रिक्त)
	    postponed = !netbeans_keystring(गाँठ->keystr);
	vim_free(गाँठ->keystr);

	// Finally, dispose of the node
	vim_free(गाँठ);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    शून्य
netbeans_parse_messages(शून्य)
{
    readq_T	*गाँठ;
    char_u	*buffer;
    char_u	*p;
    पूर्णाङ्क		own_गाँठ;

    यावत् (nb_channel != रिक्त)
    {
	गाँठ = channel_peek(nb_channel, PART_SOCK);
	यदि (गाँठ == रिक्त)
	    भङ्ग;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_first_nl(गाँठ);
	यदि (p == रिक्त)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    यदि (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		प्रत्यागम;
	    अनुवर्त;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	यदि (*p == NUL)
	{
	    own_गाँठ = TRUE;
	    buffer = channel_get(nb_channel, PART_SOCK, रिक्त);
	    // "node" is now invalid!
	}
	अन्यथा
	{
	    own_गाँठ = FALSE;
	    buffer = गाँठ->rq_buffer;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(buffer);

	यदि (own_गाँठ)
	    // buffer finished, dispose of it
	    vim_free(buffer);
	अन्यथा यदि (nb_channel != रिक्त)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (पूर्णाङ्क)(p - buffer));
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
    स्थिर शून्य
nb_parse_cmd(char_u *cmd)
{
    वर्ण	*verb;
    वर्ण	*q;
    पूर्णाङ्क		bufno;
    पूर्णाङ्क		isfunc = -1;

    यदि (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    यदि (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_column = false;

	// The IDE is breaking the connection.
	netbeans_close();
	प्रत्यागम;
    }

    bufno = strtol((वर्ण *)cmd, &verb, 10);

    यदि (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	प्रत्यागम;
    }
    ++verb; // skip colon

    प्रति (q = verb; *q; q++)
    {
	यदि (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    भङ्ग;
	}
	अन्यथा यदि (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    भङ्ग;
	}
    }

    यदि (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_in_str), cmd);
	प्रत्यागम;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (वर्ण *)skipwhite((char_u *)q);

    यदि (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
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

संरचना nbbuf_struct
{
    buf_T		*bufp;
    unsigned पूर्णाङ्क	 fireChanges:1;
    unsigned पूर्णाङ्क	 initDone:1;
    unsigned पूर्णाङ्क	 जोड़ो_Done:1;
    unsigned पूर्णाङ्क	 modified:1;
    पूर्णाङ्क			 nbbuf_संख्या;
    वर्ण		*displayname;
    पूर्णाङ्क			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

प्रकारनिर्देश संरचना nbbuf_struct nbbuf_T;

स्थिर nbbuf_T *buf_list = रिक्त;
स्थिर पूर्णाङ्क buf_list_size = 0;	// size of buf_list
स्थिर पूर्णाङ्क buf_list_used = 0;	// nr of entries in buf_list actually in use

स्थिर वर्ण **globalsignmap = रिक्त;
स्थिर पूर्णाङ्क globalsignmaplen = 0;
स्थिर पूर्णाङ्क globalsignmapused = 0;

स्थिर पूर्णाङ्क  mapsigntype(nbbuf_T *, पूर्णाङ्क localsigntype);
स्थिर शून्य addsigntype(nbbuf_T *, पूर्णाङ्क localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
स्थिर शून्य print_read_संदेश(nbbuf_T *buf);
स्थिर शून्य print_save_संदेश(nbbuf_T *buf, off_T nchars);

स्थिर पूर्णाङ्क curPCtype = -1;

/*
 * Free netbeans resources.
 */
    स्थिर शून्य
nb_free(शून्य)
{
    कुंजी_Q_T *कुंजी_गाँठ = कुंजी_Head.next;
    nbbuf_T buf;
    पूर्णाङ्क i;

    // free the netbeans buffer list
    प्रति (i = 0; i < buf_list_used; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	यदि (buf.bufp != रिक्त && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_size = 0;
    buf_list_used = 0;

    // free the queued key commands
    यावत् (कुंजी_गाँठ != रिक्त && कुंजी_गाँठ != &कुंजी_Head)
    {
	कुंजी_Q_T *next = कुंजी_गाँठ->next;
	vim_free(कुंजी_गाँठ->keystr);
	vim_free(कुंजी_गाँठ);
	यदि (next == &कुंजी_Head)
	{
	    कुंजी_Head.next = &कुंजी_Head;
	    कुंजी_Head.prev = &कुंजी_Head;
	    भङ्ग;
	}
	कुंजी_गाँठ = next;
    }

    // free the queued netbeans commands
    यदि (nb_channel != रिक्त)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    स्थिर पूर्णाङ्क
nb_getbufno(buf_T *bufp)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < buf_list_used; i++)
	यदि (buf_list[i].bufp == bufp)
	    प्रत्यागम i;
    प्रत्यागम -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    पूर्णाङ्क
isNetbeansBuffer(buf_T *bufp)
{
    प्रत्यागम NETBEANS_OPEN && bufp->b_netbeans_file;
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
    पूर्णाङ्क
isNetbeansModified(buf_T *bufp)
{
    यदि (isNetbeansBuffer(bufp))
    {
	पूर्णाङ्क bufno = nb_getbufno(bufp);

	यदि (bufno > 0)
	    प्रत्यागम buf_list[bufno].modified;
	अन्यथा
	    प्रत्यागम FALSE;
    }
    अन्यथा
	प्रत्यागम FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    स्थिर nbbuf_T *
nb_get_buf(पूर्णाङ्क bufno)
{
    // find or create a buffer with the given number
    पूर्णाङ्क incr;

    यदि (bufno <= 0)
	प्रत्यागम रिक्त;

    यदि (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * परिमाण(nbbuf_T));
	buf_list_size = 100;
    }
    यदि (bufno >= buf_list_used) // new
    {
	यदि (bufno >= buf_list_size) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_size + 90;
	    buf_list_size += incr;
	    bufsize = buf_list_size * परिमाण(nbbuf_T);
	    यदि (bufsize == 0 || bufsize / परिमाण(nbbuf_T)
						      != (size_t)buf_list_size)
	    {
		// list size overflow, bail out
		प्रत्यागम रिक्त;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    यदि (buf_list == रिक्त)
	    {
		vim_free(t_buf_list);
		buf_list_size = 0;
		प्रत्यागम रिक्त;
	    }
	    vim_memset(buf_list + buf_list_size - incr, 0,
						      incr * परिमाण(nbbuf_T));
	}

	यावत् (buf_list_used <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_used].fireChanges = 1;
	    ++buf_list_used;
	}
    }

    प्रत्यागम buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    स्थिर पूर्णाङ्क
गिनती_changed_buffers(शून्य)
{
    buf_T	*bufp;
    पूर्णाङ्क		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	यदि (bufp->b_changed)
	    ++n;
    प्रत्यागम n;
}

/*
 * End the netbeans session.
 */
    शून्य
netbeans_end(शून्य)
{
    पूर्णाङ्क i;
    स्थिर वर्ण buf[128];

    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    प्रति (i = 0; i < buf_list_used; i++)
    {
	यदि (!buf_list[i].bufp)
	    अनुवर्त;
	यदि (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, रिक्त);
	buf_list[i].bufp = रिक्त;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    स्थिर शून्य
nb_send(वर्ण *buf, वर्ण *fun)
{
    यदि (nb_channel != रिक्त)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (पूर्णाङ्क)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    स्थिर शून्य
nb_reply_nil(पूर्णाङ्क cmdno)
{
    वर्ण reply[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    sprintf(reply, "%d\n", cmdno);
    nb_send(reply, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    स्थिर शून्य
nb_reply_text(पूर्णाङ्क cmdno, char_u *result)
{
    char_u *reply;

    nbdebug(("REP %d: %s\n", cmdno, (वर्ण *)result));

    reply = alloc(STRLEN(result) + 32);
    sprintf((वर्ण *)reply, "%d %s\n", cmdno, (वर्ण *)result);
    nb_send((वर्ण *)reply, "nb_reply_text");

    vim_free(reply);
}


/*
 * Send a response with a number result code.
 */
    स्थिर शून्य
nb_reply_nr(पूर्णाङ्क cmdno, long result)
{
    वर्ण reply[32];

    nbdebug(("REP %d: %ld\n", cmdno, result));

    sprintf(reply, "%d %ld\n", cmdno, result);
    nb_send(reply, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    स्थिर char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    यदि (buf == रिक्त)
	प्रत्यागम रिक्त;
    प्रति (; *p; p++)
    {
	विकल्प (*p)
	{
	    पक्ष '\"':
	    पक्ष '\\':
		*q++ = '\\'; *q++ = *p; भङ्ग;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    पक्ष '\n':
		*q++ = '\\'; *q++ = 'n'; भङ्ग;
	    पक्ष '\r':
		*q++ = '\\'; *q++ = 'r'; भङ्ग;
	    स्वभाव:
		*q++ = *p;
		भङ्ग;
	}
    }
    *q = '\0';

    प्रत्यागम buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    स्थिर वर्ण *
nb_unquote(char_u *p, char_u **endp)
{
    वर्ण *result = 0;
    वर्ण *q;
    पूर्णाङ्क done = 0;

    // result is never longer than input
    result = alloc_clear(STRLEN(p) + 1);
    यदि (result == रिक्त)
	प्रत्यागम रिक्त;

    यदि (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	result[0] = NUL;
	प्रत्यागम result;
    }

    प्रति (q = result; !done && *p != NUL;)
    {
	विकल्प (*p)
	{
	    पक्ष '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		done = 1;
		भङ्ग;

	    पक्ष '\\':
		++p;
		विकल्प (*p)
		{
		    पक्ष '\\':	*q++ = '\\';	भङ्ग;
		    पक्ष 'n':	*q++ = '\n';	भङ्ग;
		    पक्ष 't':	*q++ = '\t';	भङ्ग;
		    पक्ष 'r':	*q++ = '\r';	भङ्ग;
		    पक्ष '"':	*q++ = '"';	भङ्ग;
		    पक्ष NUL:	--p;		भङ्ग;
		    // default: skip over illegal chars
		}
		++p;
		भङ्ग;

	    स्वभाव:
		*q++ = *p++;
	}
    }

    यदि (endp != रिक्त)
	*endp = p;

    प्रत्यागम result;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    स्थिर शून्य
nb_partialremove(linenr_T lnum, colnr_T first, colnr_T last)
{
    char_u *oldtext, *newtext;
    पूर्णाङ्क oldlen;
    पूर्णाङ्क lastbyte = last;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    यदि (first >= (colnr_T)oldlen || oldlen == 0)  // just in case
	प्रत्यागम;
    यदि (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - first));
    यदि (newtext == रिक्त)
	प्रत्यागम;

    mch_memmove(newtext, oldtext, first);
    STRMOVE(newtext + first, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    स्थिर शून्य
nb_joinlines(linenr_T first, linenr_T other)
{
    पूर्णाङ्क len_first, len_other;
    char_u *p;

    len_first = ml_get_len(first);
    len_other = ml_get_len(other);
    p = alloc(len_first + len_other + 1);
    यदि (p == रिक्त)
	प्रत्यागम;

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
    स्थिर पूर्णाङ्क
nb_do_cmd(
    पूर्णाङ्क		bufno,
    char_u	*cmd,
    पूर्णाङ्क		func,
    पूर्णाङ्क		cmdno,
    char_u	*args)	    // points to space before arguments or NUL
{
    पूर्णाङ्क		do_update = 0;
    long	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    स्थिर पूर्णाङ्क	skip = 0;
    पूर्णाङ्क		retval = OK;
    वर्ण	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (वर्ण *)args));

    यदि (func)
    {
// =====================================================================
	यदि (streq((वर्ण *)cmd, "getModified"))
	{
	    यदि (buf == रिक्त || buf->bufp == रिक्त)
		// Return the number of buffers that are modified.
		nb_reply_nr(cmdno, (long)गिनती_changed_buffers());
	    अन्यथा
		// Return whether the buffer is modified.
		nb_reply_nr(cmdno, (long)(buf->bufp->b_changed
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_reply_nr(cmdno, (long)गिनती_changed_buffers());
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "getCursor"))
	{
	    char_u text[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((वर्ण *)text, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (long)curwin->w_cursor.lnum,
		    (पूर्णाङ्क)curwin->w_cursor.col,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_reply_text(cmdno, text);
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "getAnno"))
	{
	    long linenum = 0;
#ifdef FEAT_SIGNS
	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_getanno));
		retval = FAIL;
	    }
	    अन्यथा
	    {
		पूर्णाङ्क ser_अंक;

		cp = (वर्ण *)args;
		ser_अंक = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (long)buf_findsign(buf->bufp, ser_अंक, रिक्त);
	    }
#endif
	    nb_reply_nr(cmdno, linenum);
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "getLength"))
	{
	    long len = 0;

	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_buffer_identifier_in_getlength));
		retval = FAIL;
	    }
	    अन्यथा
	    {
		len = get_buf_size(buf->bufp);
	    }
	    nb_reply_nr(cmdno, len);
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "getText"))
	{
	    long	len;
	    linenr_T	nlines;
	    char_u	*text = रिक्त;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*पंक्ति;

	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_buffer_identifier_in_gettext));
		retval = FAIL;
	    }
	    अन्यथा
	    {
		len = get_buf_size(buf->bufp);
		nlines = buf->bufp->b_ml.ml_पंक्ति_गिनती;
		text = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		यदि (text == रिक्त)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		अन्यथा
		{
		    p = text;
		    *p++ = '\"';
		    प्रति (; lno <= nlines ; lno++)
		    {
			पंक्ति = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			यदि (पंक्ति != रिक्त)
			{
			    STRCPY(p, पंक्ति);
			    p += STRLEN(पंक्ति);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(पंक्ति);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    यदि (text == रिक्त)
		nb_reply_text(cmdno, (char_u *)"");
	    अन्यथा
	    {
		nb_reply_text(cmdno, text);
		vim_free(text);
	    }
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "remove"))
	{
	    long गिनती;
	    pos_T first, last;
	    pos_T *pos;
	    pos_T *next;
	    linenr_T del_from_lnum, del_to_lnum;  // lines to be deleted as a whole
	    पूर्णाङ्क oldFire = netbeansFireChanges;
	    पूर्णाङ्क oldSuppress = netbeans_Suppress_No_पंक्तियाँ;
	    पूर्णाङ्क wasChanged;

	    यदि (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (वर्ण *) cmd));
		nb_reply_nil(cmdno);
		प्रत्यागम OK;
	    }

	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_buffer_identifier_in_remove));
		retval = FAIL;
	    }
	    अन्यथा
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_पंक्तियाँ = TRUE;

		nb_set_curbuf(buf->bufp);
		wasChanged = buf->bufp->b_changed;
		cp = (वर्ण *)args;
		off = strtol(cp, &cp, 10);
		गिनती = strtol(cp, &cp, 10);
		args = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		यदि (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_पंक्तियाँ = oldSuppress;
		    प्रत्यागम FAIL;
		}
		first = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      first.lnum, first.col));
		pos = off2pos(buf->bufp, off+गिनती-1);
		यदि (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_पंक्तियाँ = oldSuppress;
		    प्रत्यागम FAIL;
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
		next = off2pos(buf->bufp, off + गिनती);

		// Remove part of the first line.
		यदि (first.col != 0
				|| (next != रिक्त && first.lnum == next->lnum))
		{
		    यदि (first.lnum != last.lnum
			    || (next != रिक्त && first.lnum != next->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(first.lnum, first.col,
							     (colnr_T)MAXCOL);
			यदि (first.lnum == last.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(first.lnum, next->lnum);
			    del_to_lnum = next->lnum;
			}
		    }
		    अन्यथा
		    {
			// remove within one line
			nb_partialremove(first.lnum, first.col, last.col);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		यदि (first.lnum != last.lnum && next != रिक्त
			&& next->col != 0 && last.lnum == next->lnum)
		{
		    nb_partialremove(last.lnum, 0, last.col);
		    यदि (del_from_lnum > first.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(first.lnum, last.lnum);
		    }
		    अन्यथा
			// First line is deleted as a whole, keep the last
			// line.
			--del_to_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		यदि (first.lnum != last.lnum && del_from_lnum > first.lnum
			&& next != रिक्त && last.lnum != next->lnum)
		{
		    nb_joinlines(first.lnum, next->lnum);
		    del_to_lnum = next->lnum;
		}

		// Delete whole lines if there are any.
		यदि (del_to_lnum >= del_from_lnum)
		{
		    पूर्णाङ्क i;

		    // delete signs from the lines being deleted
		    प्रति (i = del_from_lnum; i <= del_to_lnum; i++)
		    {
			पूर्णाङ्क id = buf_findsign_id(buf->bufp, (linenr_T)i, रिक्त);
			यदि (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, रिक्त);
			}
			अन्यथा
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_to_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.col = 0;
		    del_पंक्तियाँ(del_to_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = first;
		check_cursor_lnum();
		buf->bufp->b_changed = wasChanged; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_पंक्तियाँ = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_reply_nil(cmdno);
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "insert"))
	{
	    char_u	*to_free;

	    यदि (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (वर्ण *) cmd));
		nb_reply_nil(cmdno);
		प्रत्यागम OK;
	    }

	    // get offset
	    cp = (वर्ण *)args;
	    off = strtol(cp, &cp, 10);
	    args = (char_u *)cp;

	    // get text to be inserted
	    args = skipwhite(args);
	    args = to_free = (char_u *)nb_unquote(args, रिक्त);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_buffer_identifier_in_जोड़ो));
		retval = FAIL;
	    }
	    अन्यथा यदि (args != रिक्त)
	    {
		पूर्णाङ्क	ff_detected = EOL_UNKNOWN;
		पूर्णाङ्क	buf_was_empty = (buf->bufp->b_ml.ml_flags & ML_EMPTY);
		size_t	len = 0;
		पूर्णाङ्क	added = 0;
		पूर्णाङ्क	oldFire = netbeansFireChanges;
		पूर्णाङ्क	old_b_changed;
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
		यदि (pos != रिक्त)
		{
		    यदि (pos->lnum <= 0)
			lnum_start = 1;
		    अन्यथा
			lnum_start = pos->lnum;
		}
		अन्यथा
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    यदि (buf_was_empty)
			lnum_start = 1;	    // above empty line
		    अन्यथा
			lnum_start = curbuf->b_ml.ml_पंक्ति_गिनती + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_start;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		यावत् (*args != NUL)
		{
		    nlp = vim_strchr(args, '\n');
		    यदि (nlp == रिक्त)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(args);
		    }
		    अन्यथा
		    {
			len = nlp - args;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			यदि (nlp > args && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			अन्यथा
			    ff_detected = EOL_UNIX;
		    }
		    args[len] = NUL;

		    यदि (lnum == lnum_start
			    && ((pos != रिक्त && pos->col > 0)
				|| (lnum == 1 && buf_was_empty)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			पूर्णाङ्क	col = pos == रिक्त ? 0 : pos->col;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			यदि (newline != रिक्त)
			{
			    mch_memmove(newline, oldline, (size_t)col);
			    newline[col] = NUL;
			    STRCAT(newline, args);
			    STRCAT(newline, oldline + col);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    अन्यथा
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), args,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    यदि (nlp == रिक्त)
			भङ्ग;
		    ++lnum;
		    args = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_पंक्तियाँ_mark(lnum_start - 1, (long)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		यदि (buf_was_empty)
		{
		    यदि (ff_detected == EOL_UNKNOWN)
#यदि defined(MSWIN)
			ff_detected = EOL_DOS;
#अन्यथा
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
	अन्यथा
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_reply_nil(cmdno);
	    retval = FAIL;
	}
    }
    अन्यथा // Not a function; no reply required.
    {
// =====================================================================
	यदि (streq((वर्ण *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    यदि (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_buffer_identifier_in_create));
		प्रत्यागम FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, रिक्त, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->जोड़ो_Done = FALSE;
#यदि defined(FEAT_MENU) && defined(FEAT_GUI)
	    यदि (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "insertDone"))
	{
	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    अन्यथा
	    {
		buf->bufp->b_start_eol = *args == 'T';
		buf->जोड़ो_Done = TRUE;
		args += 2;
		buf->bufp->b_p_ro = *args == 'T';
		print_read_संदेश(buf);
	    }
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "saveDone"))
	{
	    long savedChars = atol((वर्ण *)args);

	    यदि (buf == रिक्त || buf->bufp == रिक्त)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    अन्यथा
		print_save_संदेश(buf, savedChars);
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "startDocumentListen"))
	{
	    यदि (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_startdocumentlisten));
		प्रत्यागम FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "stopDocumentListen"))
	{
	    यदि (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_stopdocumentlisten));
		प्रत्यागम FAIL;
	    }
	    buf->fireChanges = 0;
	    यदि (buf->bufp != रिक्त && buf->bufp->b_was_netbeans_file)
	    {
		यदि (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_lost_for_buffer_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_lost_for_buffer_nr),
							   buf->bufp->b_fnum);
		}
		अन्यथा
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
	अन्यथा यदि (streq((वर्ण *)cmd, "setTitle"))
	{
	    यदि (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_buffer_identifier_in_settitle));
		प्रत्यागम FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, रिक्त);
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "initDone"))
	{
	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_buffer_identifier_in_initdone));
		प्रत्यागम FAIL;
	    }
	    do_update = 1;
	    buf->initDone = TRUE;
	    nb_set_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_कुंजी_queue();
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "setBufferNumber")
		|| streq((वर्ण *)cmd, "putBufferNumber"))
	{
	    char_u	*path;
	    buf_T	*bufp;

	    यदि (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_buffer_identifier_in_setbuffernumber));
		प्रत्यागम FAIL;
	    }
	    path = (char_u *)nb_unquote(args, रिक्त);
	    यदि (path == रिक्त)
		प्रत्यागम FAIL;
	    bufp = buflist_findname(path);
	    vim_free(path);
	    यदि (bufp == रिक्त)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", args));
		semsg(_(e_file_str_not_found_in_setbuffernumber), args);
		प्रत्यागम FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_संख्या = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    यदि (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    अन्यथा
	    {
		buf->initDone = TRUE;

		// handle any postponed key commands
		handle_कुंजी_queue();
	    }

// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "setFullName"))
	{
	    यदि (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_buffer_identifier_in_setfullname));
		प्रत्यागम FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, रिक्त);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#यदि defined(FEAT_MENU) && defined(FEAT_GUI)
	    यदि (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "editFile"))
	{
	    यदि (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_buffer_identifier_in_editfile));
		प्रत्यागम FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, रिक्त);
	    do_ecmd(0, (char_u *)buf->displayname, रिक्त, रिक्त, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->initDone = TRUE;
	    do_update = 1;
	    maketitle();
#यदि defined(FEAT_MENU) && defined(FEAT_GUI)
	    यदि (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "setVisible"))
	{
	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		यदि (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setvisible));
		प्रत्यागम FAIL;
	    }
	    यदि (streq((वर्ण *)args, "T") && buf->bufp != curbuf)
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
		यदि (gui.in_use)
		    gui_mch_set_foreground();
#endif
	    }
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "raise"))
	{
#ifdef FEAT_GUI
	    // Bring gvim to the foreground.
	    यदि (gui.in_use)
		gui_mch_set_foreground();
#endif
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "setModified"))
	{
	    पूर्णाङ्क prev_b_changed;

	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		यदि (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setmodified));
		प्रत्यागम FAIL;
	    }
	    prev_b_changed = buf->bufp->b_changed;
	    यदि (streq((वर्ण *)args, "T"))
		buf->bufp->b_changed = TRUE;
	    अन्यथा
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		यदि (buf->bufp->b_ffname != रिक्त
			&& mch_stat((वर्ण *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_time(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_changed = FALSE;
	    }
	    buf->modified = buf->bufp->b_changed;
	    यदि (prev_b_changed != buf->bufp->b_changed)
	    {
		check_status(buf->bufp);
		redraw_tabline = TRUE;
#यदि defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "setModtime"))
	{
	    यदि (buf == रिक्त || buf->bufp == रिक्त)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    अन्यथा
	    {
		buf->bufp->b_mtime = atoi((वर्ण *)args);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "setReadOnly"))
	{
	    यदि (buf == रिक्त || buf->bufp == रिक्त)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    अन्यथा यदि (streq((वर्ण *)args, "T"))
		buf->bufp->b_p_ro = TRUE;
	    अन्यथा
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "showBalloon"))
	{
#यदि defined(FEAT_BEVAL_GUI)
	    स्थिर वर्ण	*text = रिक्त;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    यदि (balloonEval != रिक्त)
	    {
		vim_free(text);
		text = nb_unquote(args, रिक्त);
		यदि (text != रिक्त)
		    gui_mch_post_balloon(balloonEval, (char_u *)text);
	    }
#endif
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "setDot"))
	{
	    pos_T *pos;
#ifdef NBDEBUG
	    char_u *s;
#endif

	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_buffer_identifier_in_setdot));
		प्रत्यागम FAIL;
	    }

	    nb_set_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    यदि (VIsual_active)
		end_visual_mode();
#ifdef NBDEBUG
	    s = args;
#endif
	    pos = get_off_or_lnum(buf->bufp, &args);
	    यदि (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#ifdef FEAT_FOLDING
		foldOpenCursor();
#endif
	    }
	    अन्यथा
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
	    यदि (State == MODE_HITRETURN || State == MODE_ASKMORE)
	    {
#ifdef FEAT_GUI_GTK
		यदि (gui.in_use && gtk_main_level() > 0)
		    gtk_main_quit();
#endif
	    }
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "close"))
	{
#ifdef NBDEBUG
	    वर्ण *name = "<NONE>";
#endif

	    यदि (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_buffer_identifier_in_close));
		प्रत्यागम FAIL;
	    }

#ifdef NBDEBUG
	    यदि (buf->displayname != रिक्त)
		name = buf->displayname;
#endif
	    यदि (buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		यदि (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    यदि (buf->bufp != रिक्त)
		do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = रिक्त;
	    buf->initDone = FALSE;
	    do_update = 1;
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "defineAnnoType"))
	{
#ifdef FEAT_SIGNS
	    पूर्णाङ्क type_अंक;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    पूर्णाङ्क parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    यदि (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_buffer_identifier_in_defineannotype));
		प्रत्यागम FAIL;
	    }

	    cp = (वर्ण *)args;
	    type_अंक = strtol(cp, &cp, 10);
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
	    यदि (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(args);
	    bg = vim_strsave(p);
	    यदि (STRLEN(fg) > MAX_COLOR_LENGTH || STRLEN(bg) > MAX_COLOR_LENGTH)
	    {
		emsg(_(e_highlighting_color_name_too_long_in_defineAnnoType));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    अन्यथा यदि (!nb_is_safe_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_safe_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_safe_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_in_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    अन्यथा यदि (typeName != रिक्त && tooltip != रिक्त && glyphFile != रिक्त)
		addsigntype(buf, type_अंक, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    यदि (parse_error)
		प्रत्यागम FAIL;

#endif
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "addAnno"))
	{
#ifdef FEAT_SIGNS
	    पूर्णाङ्क ser_अंक;
	    पूर्णाङ्क local_Type_अंक;
	    पूर्णाङ्क type_अंक;
	    pos_T *pos;

	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_addanno));
		प्रत्यागम FAIL;
	    }

	    do_update = 1;

	    cp = (वर्ण *)args;
	    ser_अंक = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_अंक = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    type_अंक = mapsigntype(buf, local_Type_अंक);

	    pos = get_off_or_lnum(buf->bufp, &args);

	    cp = (वर्ण *)args;
	    vim_ignored = (पूर्णाङ्क)strtol(cp, &cp, 10);
	    args = (char_u *)cp;
# ifdef NBDEBUG
	    यदि (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    यदि (ser_अंक >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		प्रत्यागम FAIL;
	    }
	    यदि (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_अंक, pos->lnum, type_अंक, buf->bufp->b_fnum);
		यदि (type_अंक == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_अंक,
						       buf->bufp->b_fnum);
	    }
#endif
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "removeAnno"))
	{
#ifdef FEAT_SIGNS
	    पूर्णाङ्क ser_अंक;

	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		प्रत्यागम FAIL;
	    }
	    do_update = 1;
	    cp = (वर्ण *)args;
	    ser_अंक = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_अंक, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#endif
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "moveAnnoToFront"))
	{
#ifdef FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#endif
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "guard") || streq((वर्ण *)cmd, "unguard"))
	{
	    पूर्णाङ्क len;
	    pos_T first;
	    pos_T last;
	    pos_T *pos;
	    पूर्णाङ्क un = (cmd[0] == 'u');
	    स्थिर पूर्णाङ्क guardId = GUARDEDOFFSET;

	    यदि (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (वर्ण *) cmd));
		प्रत्यागम OK;
	    }

	    nb_init_graphics();

	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		प्रत्यागम FAIL;
	    }
	    nb_set_curbuf(buf->bufp);
	    cp = (वर्ण *)args;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, रिक्त, 10);
	    args = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    यदि (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    अन्यथा
	    {
		first = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		यदि (pos != रिक्त && pos->col == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		यदि (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		अन्यथा
		{
		    long lnum;
		    last = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     first.lnum, first.col,
			     last.lnum, last.col));
#ifdef FEAT_SIGNS
		    प्रति (lnum = first.lnum; lnum <= last.lnum; lnum++)
		    {
			यदि (un)
			{
			    // never used
			}
			अन्यथा
			{
			    यदि (buf_findsigntype_id(buf->bufp, lnum,
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
	अन्यथा यदि (streq((वर्ण *)cmd, "startAtomic"))
	{
	    inAtomic = 1;
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "endAtomic"))
	{
	    inAtomic = 0;
	    यदि (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		प्रत्यागम FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    यदि (bufIsChanged(buf->bufp))
	    {
		// Only write if the buffer can be written.
		यदि (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != रिक्त
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    यदि (!bufref_valid(&bufref))
			buf->bufp = रिक्त;
		}
	    }
	    अन्यथा
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "netbeansBuffer"))
	{
	    यदि (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		प्रत्यागम FAIL;
	    }
	    यदि (*args == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    अन्यथा
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "specialKeys"))
	{
	    special_कुंजियाँ(args);
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	अन्यथा यदि (streq((वर्ण *)cmd, "version"))
	{
	    // not used yet
	}
	अन्यथा
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    यदि (inAtomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    यदि (buf != रिक्त && buf->initDone && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	out_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	यदि (State == MODE_HITRETURN || State == MODE_ASKMORE)
	{
#ifdef FEAT_GUI_GTK
	    यदि (gui.in_use && gtk_main_level() > 0)
		gtk_main_quit();
#endif
	}
    }

    प्रत्यागम retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    स्थिर शून्य
nb_set_curbuf(buf_T *buf)
{
    यदि (curbuf == buf)
	प्रत्यागम;

    यदि (buf_jump_open_win(buf) != रिक्त)
	प्रत्यागम;
    यदि ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != रिक्त)
	प्रत्यागम;
    set_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    स्थिर शून्य
coloncmd(वर्ण *cmd, ...)
{
    वर्ण buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, परिमाण(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, रिक्त, रिक्त, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    out_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    स्थिर शून्य
special_कुंजियाँ(char_u *args)
{
    वर्ण *save_str = nb_unquote(args, रिक्त);
    वर्ण *tok = strtok(save_str, " ");
    वर्ण *sep;
#define KEYBUFLEN 64
    वर्ण keybuf[KEYBUFLEN];
    वर्ण cmdbuf[256];

    यावत् (tok != रिक्त)
    {
	पूर्णाङ्क i = 0;

	यदि ((sep = strchr(tok, '-')) != रिक्त)
	{
	    *sep = NUL;
	    यावत् (*tok && i + 2 < KEYBUFLEN)
	    {
		विकल्प (*tok)
		{
		    पक्ष 'A':
		    पक्ष 'M':
		    पक्ष 'C':
		    पक्ष 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			भङ्ग;
		}
		tok++;
	    }
	    tok++;
	}

	यदि (strlen(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    पूर्णाङ्क safe = TRUE;
	    प्रति (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		यदि (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    safe = FALSE;
		    भङ्ग;
		}
	    }
	    यदि (safe)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, परिमाण(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, MODE_NORMAL, FALSE);
	    }
	}
	tok = strtok(रिक्त, " ");
    }
    vim_free(save_str);
}

    शून्य
ex_nbclose(exarg_T *eap UNUSED)
{
    netbeans_close();
}

    शून्य
ex_nbkey(exarg_T *eap)
{
    (शून्य)netbeans_keystring(eap->arg);
}

    शून्य
ex_nbstart(
    exarg_T	*eap)
{
#ifdef FEAT_GUI
# यदि !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    यदि (gui.in_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	प्रत्यागम;
    }
# endif
#endif
    netbeans_open((वर्ण *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    स्थिर शून्य
nb_init_graphics(शून्य)
{
    स्थिर पूर्णाङ्क did_init = FALSE;

    यदि (did_init)
	प्रत्यागम;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    स्थिर शून्य
netbeans_keyname(पूर्णाङ्क कुंजी, वर्ण *buf)
{
    वर्ण *name = 0;
    वर्ण namebuf[2];
    पूर्णाङ्क ctrl  = 0;
    पूर्णाङ्क shift = 0;
    पूर्णाङ्क alt   = 0;

    यदि (mod_mask & MOD_MASK_CTRL)
	ctrl = 1;
    यदि (mod_mask & MOD_MASK_SHIFT)
	shift = 1;
    यदि (mod_mask & MOD_MASK_ALT)
	alt = 1;


    विकल्प (कुंजी)
    {
	पक्ष K_F1:		name = "F1";		भङ्ग;
	पक्ष K_S_F1:	name = "F1";	shift = 1;	भङ्ग;
	पक्ष K_F2:		name = "F2";		भङ्ग;
	पक्ष K_S_F2:	name = "F2";	shift = 1;	भङ्ग;
	पक्ष K_F3:		name = "F3";		भङ्ग;
	पक्ष K_S_F3:	name = "F3";	shift = 1;	भङ्ग;
	पक्ष K_F4:		name = "F4";		भङ्ग;
	पक्ष K_S_F4:	name = "F4";	shift = 1;	भङ्ग;
	पक्ष K_F5:		name = "F5";		भङ्ग;
	पक्ष K_S_F5:	name = "F5";	shift = 1;	भङ्ग;
	पक्ष K_F6:		name = "F6";		भङ्ग;
	पक्ष K_S_F6:	name = "F6";	shift = 1;	भङ्ग;
	पक्ष K_F7:		name = "F7";		भङ्ग;
	पक्ष K_S_F7:	name = "F7";	shift = 1;	भङ्ग;
	पक्ष K_F8:		name = "F8";		भङ्ग;
	पक्ष K_S_F8:	name = "F8";	shift = 1;	भङ्ग;
	पक्ष K_F9:		name = "F9";		भङ्ग;
	पक्ष K_S_F9:	name = "F9";	shift = 1;	भङ्ग;
	पक्ष K_F10:		name = "F10";		भङ्ग;
	पक्ष K_S_F10:	name = "F10";	shift = 1;	भङ्ग;
	पक्ष K_F11:		name = "F11";		भङ्ग;
	पक्ष K_S_F11:	name = "F11";	shift = 1;	भङ्ग;
	पक्ष K_F12:		name = "F12";		भङ्ग;
	पक्ष K_S_F12:	name = "F12";	shift = 1;	भङ्ग;
	स्वभाव:
			यदि (कुंजी >= ' ' && कुंजी <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = कुंजी;
			    namebuf[1] = NUL;
			}
			अन्यथा
			    name = "X";
			भङ्ग;
    }

    buf[0] = '\0';
    यदि (ctrl)
	strcat(buf, "C");
    यदि (shift)
	strcat(buf, "S");
    यदि (alt)
	strcat(buf, "M"); // META
    यदि (ctrl || shift || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#यदि defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    शून्य
netbeans_beval_cb(
	BalloonEval	*beval,
	पूर्णाङ्क		 state UNUSED)
{
    win_T	*wp;
    char_u	*text;
    linenr_T	lnum;
    पूर्णाङ्क		col;
    वर्ण	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    यदि (!can_use_beval() || !NETBEANS_OPEN)
	प्रत्यागम;

    यदि (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) != OK)
	प्रत्यागम;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    यदि (text != रिक्त && text[0] != NUL && STRLEN(text) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	यदि (buf != रिक्त)
	{
	    p = nb_quote(text);
	    यदि (p != रिक्त)
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
    पूर्णाङ्क
netbeans_active(शून्य)
{
    प्रत्यागम NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    शून्य
netbeans_open(वर्ण *params, पूर्णाङ्क doabort)
{
    वर्ण *cmd = "0:startupDone=0\n";

    यदि (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	प्रत्यागम;
    }

    यदि (netbeans_connect(params, doabort) != OK)
	प्रत्यागम;

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
    शून्य
netbeans_send_disconnect(शून्य)
{
    वर्ण buf[128];

    यदि (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#यदि defined(FEAT_EVAL)
    पूर्णाङ्क
set_ref_in_nb_channel(पूर्णाङ्क copyID)
{
    पूर्णाङ्क abort = FALSE;
    typval_T tv;

    यदि (nb_channel == रिक्त)
	प्रत्यागम FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
    प्रत्यागम abort;
}
#endif

#यदि defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    शून्य
netbeans_frame_moved(पूर्णाङ्क new_x, पूर्णाङ्क new_y)
{
    वर्ण buf[128];

    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (पूर्णाङ्क)Columns, (पूर्णाङ्क)Rows, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#endif

/*
 * Tell netbeans the user opened or activated a file.
 */
    शून्य
netbeans_file_activated(buf_T *bufp)
{
    पूर्णाङ्क bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    वर्ण    buffer[2*MAXPATHL];
    char_u  *q;

    यदि (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	प्रत्यागम;

    q = nb_quote(bufp->b_ffname);
    यदि (q == रिक्त)
	प्रत्यागम;
    यदि (bp == रिक्त)
    {
	vim_free(q);
	प्रत्यागम;
    }

    vim_snprintf(buffer, परिमाण(buffer),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (वर्ण *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    शून्य
netbeans_file_opened(buf_T *bufp)
{
    पूर्णाङ्क bufno = nb_getbufno(bufp);
    वर्ण    buffer[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    पूर्णाङ्क	    bnum;

    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    q = nb_quote(bufp->b_ffname);
    यदि (q == रिक्त)
	प्रत्यागम;
    यदि (bp != रिक्त)
	bnum = bufno;
    अन्यथा
	bnum = 0;

    vim_snprintf(buffer, परिमाण(buffer), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (वर्ण *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
    यदि (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	last_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    शून्य
netbeans_file_killed(buf_T *bufp)
{
    पूर्णाङ्क		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    वर्ण	buffer[2*MAXPATHL];

    यदि (!NETBEANS_OPEN || bufno == -1)
	प्रत्यागम;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(buffer, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_killed");

    यदि (nbbuf != रिक्त)
	nbbuf->bufp = रिक्त;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    स्थिर nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, पूर्णाङ्क *bufnop)
{
    पूर्णाङ्क		bufno;
    nbbuf_T	*nbbuf;

    यदि (!NETBEANS_OPEN || !netbeansFireChanges)
	प्रत्यागम रिक्त;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    यदि (bufno <= 0)
	प्रत्यागम रिक्त;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    यदि (nbbuf != रिक्त && !nbbuf->fireChanges)
	प्रत्यागम रिक्त;		// changes in this buffer are not reported

    *bufnop = bufno;
    प्रत्यागम nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    शून्य
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    char_u	*txt,
    पूर्णाङ्क		newlen)
{
    char_u	*buf;
    पूर्णाङ्क		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;
    char_u	*p;
    char_u	*newtxt;

    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    यदि (nbbuf == रिक्त)
	प्रत्यागम;

    // Don't mark as modified for initial read
    यदि (nbbuf->जोड़ो_Done)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.col = col;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    यदि (p != रिक्त)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((वर्ण *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((वर्ण *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    शून्य
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    long	len)
{
    char_u	buf[128];
    पूर्णाङ्क		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;

    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    यदि (nbbuf == रिक्त)
	प्रत्यागम;

    यदि (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	प्रत्यागम;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.col = col;

    off = pos2off(bufp, &pos);

    sprintf((वर्ण *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((वर्ण *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    शून्य
netbeans_unmodified(buf_T *bufp UNUSED)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    शून्य
netbeans_button_release(पूर्णाङ्क button)
{
    वर्ण	buf[128];
    पूर्णाङ्क		bufno;

    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    bufno = nb_getbufno(curbuf);

    यदि (bufno < 0 || curwin == रिक्त || curwin->w_buffer != curbuf)
	प्रत्यागम;

    पूर्णाङ्क col = mouse_col - curwin->w_wincol
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
    पूर्णाङ्क
netbeans_keycommand(पूर्णाङ्क कुंजी)
{
    वर्ण	कुंजी_Name[60];

    netbeans_keyname(कुंजी, कुंजी_Name);
    प्रत्यागम netbeans_keystring((char_u *)कुंजी_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    स्थिर पूर्णाङ्क
netbeans_keystring(char_u *कुंजी_Name)
{
    वर्ण	buf[2*MAXPATHL];
    पूर्णाङ्क		bufno = nb_getbufno(curbuf);
    long	off;
    char_u	*q;

    यदि (!NETBEANS_OPEN)
	प्रत्यागम TRUE;

    यदि (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == रिक्त ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	यदि (q == रिक्त)
	    प्रत्यागम TRUE;
	vim_snprintf(buf, परिमाण(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	यदि (curbuf->b_ffname != रिक्त)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(कुंजी_Name);
	प्रत्यागम FALSE;
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
    vim_snprintf(buf, परिमाण(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, कुंजी_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, परिमाण(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, कुंजी_Name,
		off, (long)curwin->w_cursor.lnum, (long)curwin->w_cursor.col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    प्रत्यागम TRUE;
}


/*
 * Send a save event to netbeans.
 */
    शून्य
netbeans_save_buffer(buf_T *bufp)
{
    char_u	buf[64];
    पूर्णाङ्क		bufno;
    nbbuf_T	*nbbuf;

    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    यदि (nbbuf == रिक्त)
	प्रत्यागम;

    nbbuf->modified = 0;

    sprintf((वर्ण *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((वर्ण *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    शून्य
netbeans_deleted_all_पंक्तियाँ(buf_T *bufp)
{
    char_u	buf[64];
    पूर्णाङ्क		bufno;
    nbbuf_T	*nbbuf;

    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    यदि (nbbuf == रिक्त)
	प्रत्यागम;

    // Don't mark as modified for initial read
    यदि (nbbuf->जोड़ो_Done)
	nbbuf->modified = 1;

    sprintf((वर्ण *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    पूर्णाङ्क
netbeans_is_guarded(linenr_T शीर्ष, linenr_T bot)
{
    sign_entry_T	*p;
    पूर्णाङ्क			lnum;

    यदि (!NETBEANS_OPEN)
	प्रत्यागम FALSE;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
	यदि (p->se_id >= GUARDEDOFFSET)
	    प्रति (lnum = शीर्ष + 1; lnum < bot; lnum++)
		यदि (lnum == p->se_lnum)
		    प्रत्यागम TRUE;

    प्रत्यागम FALSE;
}

#यदि defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    शून्य
netbeans_draw_multisign_indicator(पूर्णाङ्क row)
{
    पूर्णाङ्क i;
    पूर्णाङ्क y;
    पूर्णाङ्क x;

    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    x = 0;
    y = row * gui.char_height + 2;

    प्रति (i = 0; i < gui.char_height - 3; i++)
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

#यदि defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    शून्य
netbeans_draw_multisign_indicator(पूर्णाङ्क row)
{
    पूर्णाङ्क i;
    पूर्णाङ्क y;
    पूर्णाङ्क x;
# यदि GTK_CHECK_VERSION(3,0,0)
    cairo_t *cr = रिक्त;
# अन्यथा
    GdkDrawable *drawable = gui.drawarea->window;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# अन्यथा
    y = row * gui.char_height + 2;
# endif

# यदि GTK_CHECK_VERSION(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_height);
    cr = cairo_create(surf);
#  अन्यथा
    cr = cairo_create(gui.surface);
#  endif
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    प्रति (i = 0; i < gui.char_height - 3; i++)
# यदि GTK_CHECK_VERSION(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# अन्यथा
    gdk_draw_point(drawable, gui.text_gc, x+2, y++);
# endif

# यदि GTK_CHECK_VERSION(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# अन्यथा
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

# यदि GTK_CHECK_VERSION(3,0,0)
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
    शून्य
netbeans_gutter_click(linenr_T lnum)
{
    sign_entry_T	*p;

    यदि (!NETBEANS_OPEN)
	प्रत्यागम;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
    {
	यदि (p->se_lnum == lnum && p->se_next && p->se_next->se_lnum == lnum)
	{
	    sign_entry_T *tail;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    यदि (p->se_prev)
		p->se_prev->se_next = p->se_next;
	    अन्यथा
		curbuf->b_signlist = p->se_next;
	    p->se_next->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    प्रति (tail = p->se_next;
		  tail->se_next && tail->se_next->se_lnum == lnum
				       && tail->se_next->se_id < GUARDEDOFFSET;
		  tail = tail->se_next)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_next = tail->se_next;
	    यदि (tail->se_next)
		tail->se_next->se_prev = p;
	    p->se_prev = tail;
	    tail->se_next = p;
	    update_debug_sign(curbuf, lnum);
	    भङ्ग;
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
    स्थिर शून्य
addsigntype(
    nbbuf_T	*buf,
    पूर्णाङ्क		type_अंक,
    char_u	*typeName,
    char_u	*tooltip UNUSED,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    पूर्णाङ्क i, j;
    पूर्णाङ्क use_fg = (*fg && STRCMP(fg, "none") != 0);
    पूर्णाङ्क use_bg = (*bg && STRCMP(bg, "none") != 0);

    प्रति (i = 0; i < globalsignmapused; i++)
	यदि (STRCMP(typeName, globalsignmap[i]) == 0)
	    भङ्ग;

    यदि (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_अंक, typeName, tooltip, glyphFile, fg, bg));
	यदि (use_fg || use_bg)
	{
	    वर्ण fgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    वर्ण bgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    वर्ण *ptr;
	    पूर्णाङ्क मूल्य;

	    मूल्य = strtol((वर्ण *)fg, &ptr, 10);
	    यदि (ptr != (वर्ण *)fg)
		sprintf(fgbuf, "guifg=#%06x", मूल्य & 0xFFFFFF);
	    अन्यथा
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    मूल्य = strtol((वर्ण *)bg, &ptr, 10);
	    यदि (ptr != (वर्ण *)bg)
		sprintf(bgbuf, "guibg=#%06x", मूल्य & 0xFFFFFF);
	    अन्यथा
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    यदि (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    अन्यथा यदि (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    अन्यथा
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	अन्यथा
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	यदि (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_अंक;

	यदि (globalsignmapused == globalsignmaplen)
	{
	    यदि (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(वर्ण *, globalsignmaplen);
	    }
	    अन्यथा    // grow it
	    {
		पूर्णाङ्क incr;
		पूर्णाङ्क oldlen = globalsignmaplen;
		वर्ण **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * परिमाण(वर्ण *));
		यदि (globalsignmap == रिक्त)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    प्रत्यागम;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * परिमाण(वर्ण *));
	    }
	}

	globalsignmap[i] = (वर्ण *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    प्रति (j = 0; j < buf->signmapused; j++)
	यदि (buf->signmap[j] == i + 1)
	    प्रत्यागम;

    // add to local map
    यदि (buf->signmapused == buf->signmaplen)
    {
	यदि (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(पूर्णाङ्क, buf->signmaplen);
	}
	अन्यथा    // grow it
	{
	    पूर्णाङ्क incr;
	    पूर्णाङ्क oldlen = buf->signmaplen;
	    पूर्णाङ्क *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * परिमाण(पूर्णाङ्क));
	    यदि (buf->signmap == रिक्त)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		प्रत्यागम;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * परिमाण(पूर्णाङ्क));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    स्थिर पूर्णाङ्क
mapsigntype(nbbuf_T *buf, पूर्णाङ्क localsigntype)
{
    यदि (--localsigntype >= 0 && localsigntype < buf->signmapused)
	प्रत्यागम buf->signmap[localsigntype];

    प्रत्यागम 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    स्थिर long
get_buf_size(buf_T *bufp)
{
    linenr_T	lnum;
    long	char_गिनती = 0;
    पूर्णाङ्क		eol_size;
    long	last_check = 100000L;

    यदि (bufp->b_ml.ml_flags & ML_EMPTY)
	प्रत्यागम 0;

    यदि (get_fileformat(bufp) == EOL_DOS)
	eol_size = 2;
    अन्यथा
	eol_size = 1;
    प्रति (lnum = 1; lnum <= bufp->b_ml.ml_पंक्ति_गिनती; ++lnum)
    {
	char_गिनती += ml_get_buf_len(bufp, lnum) + eol_size;
	// Check for a CTRL-C every 100000 characters
	यदि (char_गिनती > last_check)
	{
	    ui_breakcheck();
	    यदि (got_int)
		प्रत्यागम char_गिनती;
	    last_check = char_गिनती + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    यदि (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_गिनती -= eol_size;

    प्रत्यागम char_गिनती;
}

/*
 * Convert character offset to lnum,col
 */
    स्थिर pos_T *
off2pos(buf_T *buf, long offset)
{
    linenr_T	 lnum;
    स्थिर pos_T pos;

    pos.lnum = 0;
    pos.col = 0;
    pos.coladd = 0;

    यदि (!(buf->b_ml.ml_flags & ML_EMPTY))
    {
	यदि ((lnum = ml_find_पंक्ति_or_offset(buf, (linenr_T)0, &offset)) < 0)
	    प्रत्यागम रिक्त;
	pos.lnum = lnum;
	pos.col = offset;
    }

    प्रत्यागम &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    स्थिर pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    स्थिर pos_T	mypos;
    long		off;

    off = strtol((वर्ण *)*argp, (वर्ण **)argp, 10);
    यदि (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.col = strtol((वर्ण *)*argp, (वर्ण **)argp, 10);
	mypos.coladd = 0;
	प्रत्यागम &mypos;
    }
    प्रत्यागम off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    स्थिर long
pos2off(buf_T *buf, pos_T *pos)
{
    long	 offset = 0;

    यदि (buf->b_ml.ml_flags & ML_EMPTY)
	प्रत्यागम 0;

    यदि ((offset = ml_find_पंक्ति_or_offset(buf, pos->lnum, 0)) < 0)
	प्रत्यागम 0;
    offset += pos->col;

    प्रत्यागम offset;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    स्थिर शून्य
print_read_संदेश(nbbuf_T *buf)
{
    पूर्णाङ्क	    lnum = buf->bufp->b_ml.ml_पंक्ति_गिनती;
    off_T   nchars = buf->bufp->b_orig_size;
    char_u  c;

    संदेश_add_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    यदि (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    यदि (!buf->bufp->b_start_eol)
    {
	STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    संदेश_add_पंक्तियाँ(c, (long)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_संदेश);
    संदेश_scrolled_ign = TRUE;
    संदेश_trunc_attr((वर्ण *)IObuff, FALSE, 0);
    संदेश_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    स्थिर शून्य
print_save_संदेश(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    यदि (nchars >= 0)
    {
	// put fname in IObuff with quotes
	संदेश_add_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	संदेश_add_पंक्तियाँ(c, buf->bufp->b_ml.ml_पंक्ति_गिनती,
						buf->bufp->b_orig_size);

	VIM_CLEAR(keep_संदेश);
	संदेश_scrolled_ign = TRUE;
	p = (char_u *)संदेश_trunc_attr((वर्ण *)IObuff, FALSE, 0);
	यदि ((संदेश_scrolled && !need_wait_return) || !buf->initDone)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_संदेश(p, 0);
	}
	संदेश_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    अन्यथा
    {
	वर्ण msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_add_bang_to_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#endif // defined(FEAT_NETBEANS_INTG)
