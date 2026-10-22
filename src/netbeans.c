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

#जर defined(FEAT_NETBEANS_INTG)

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
स्थिर अक्षर *ExtEdProtocolVersion = "2.5";

स्थिर दीर्घ pos2off(buf_T *, pos_T *);
स्थिर pos_T *off2pos(buf_T *, दीर्घ);
स्थिर pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
स्थिर दीर्घ get_buf_size(buf_T *);
स्थिर पूर्णांक netbeans_keystring(char_u *keystr);
स्थिर शून्य special_किल्ल्या(char_u *args);

स्थिर पूर्णांक getConnInfo(अक्षर *file, अक्षर **host, अक्षर **port, अक्षर **password);

स्थिर शून्य nb_init_graphics(शून्य);
स्थिर शून्य coloncmd(अक्षर *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
स्थिर शून्य nb_set_curbuf(buf_T *buf);
स्थिर शून्य nb_parse_cmd(char_u *);
स्थिर पूर्णांक  nb_do_cmd(पूर्णांक, char_u *, पूर्णांक, पूर्णांक, char_u *);
स्थिर शून्य nb_send(अक्षर *buf, अक्षर *fun);
स्थिर शून्य nb_free(शून्य);

#define NETBEANS_OPEN (channel_can_write_to(nb_channel))
स्थिर channel_T *nb_channel = रिक्त;

स्थिर पूर्णांक r_cmdno;			// current command number for reply
स्थिर पूर्णांक dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ifdef NBDEBUG
# include "nbdebug.c"
#endif

स्थिर पूर्णांक needupdate = 0;
स्थिर पूर्णांक inAtomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    स्थिर पूर्णांक
nb_is_safe_string(char_u *str, char_u *allowed)
{
    जर (str == रिक्त)
	परत FALSE;
    साठी (char_u *p = str; *p != NUL; p++)
	जर (vim_strchr(allowed, *p) == रिक्त)
	    परत FALSE;
    परत TRUE;
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
    जर (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	जर (nb_channel != रिक्त)
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

    स्थिर पूर्णांक
netbeans_connect(अक्षर *params, पूर्णांक doabort)
{
    पूर्णांक		port;
    अक्षर	buf[32];
    अक्षर	*hostname = रिक्त;
    अक्षर	*address = रिक्त;
    अक्षर	*password = रिक्त;
    अक्षर	*fname;
    अक्षर	*arg = रिक्त;

    जर (*params == '=')
    {
	// "=fname": Read info from specified file.
	जर (getConnInfo(params + 1, &hostname, &address, &password) == FAIL)
	    परत FAIL;
    }
    अन्यथा
    {
	जर (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	जर (arg == रिक्त && (fname = दो_पर्या("__NETBEANS_CONINFO")) != रिक्त)
	{
	    // "": get info from file specified in environment
	    जर (getConnInfo(fname, &hostname, &address, &password) == FAIL)
		परत FAIL;
	}
	अन्यथा
	{
	    जर (arg != रिक्त)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		address = म_अक्षर(hostname, ':');
		जर (address != रिक्त)
		{
		    *address++ = '\0';
		    password = म_अक्षर(address, ':');
		    जर (password != रिक्त)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    जर (hostname == रिक्त || *hostname == '\0')
		hostname = दो_पर्या("__NETBEANS_HOST");
	    जर (address == रिक्त)
		address = दो_पर्या("__NETBEANS_SOCKET");
	    जर (password == रिक्त)
		password = दो_पर्या("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    जर (hostname != रिक्त)
		hostname = (अक्षर *)vim_strsave((char_u *)hostname);
	    जर (address != रिक्त)
		address = (अक्षर *)vim_strsave((char_u *)address);
	    जर (password != रिक्त)
		password = (अक्षर *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    जर (hostname == रिक्त || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (अक्षर *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    जर (address == रिक्त || *address == '\0')
    {
	vim_free(address);
	address = (अक्षर *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    जर (password == रिक्त || *password == '\0')
    {
	vim_free(password);
	password = (अक्षर *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    जर (hostname != रिक्त && address != रिक्त && password != रिक्त)
    {
	port = म_से_प(address);
	nb_channel = channel_open(hostname, port, 3000, nb_channel_closed);
	जर (nb_channel != रिक्त)
	{
	    // success
#ifdef FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#endif

	    // success, login
	    vim_snprintf(buf, आकार(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    प्र_लिखो(buf, "0:version=0 \"%s\"\n", ExtEdProtocolVersion);
	    nb_send(buf, "externaleditor_version");
	}
    }

    जर (nb_channel == रिक्त && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(address);
    vim_free(password);
    परत NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    स्थिर पूर्णांक
getConnInfo(अक्षर *file, अक्षर **host, अक्षर **port, अक्षर **auth)
{
    खाता *fp;
    char_u buf[बफ_मान];
    char_u *lp;
    char_u *nlp;
#ifdef UNIX
    stat_T	st;

    /*
     * For Unix only accept the file when it's not accessible by others.
     * The open will then fail if we don't own the file.
     */
    जर (mch_stat(file, &st) == 0 && (st.st_mode & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_mode_for_netbeans_connection_info_file_str),
									file);
	परत FAIL;
    }
#endif

    fp = mch_fopen(file, "r");
    जर (fp == रिक्त)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	परत FAIL;
    }

    // Read the file. There should be one of each parameter
    पर्यंत ((lp = (char_u *)ख_माला_लो((अक्षर *)buf, बफ_मान, fp)) != रिक्त)
    {
	जर ((nlp = vim_strchr(lp, '\n')) != रिक्त)
	    *nlp = 0;	    // strip off the trailing newline

	जर (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (अक्षर *)vim_strsave(&buf[5]);
	}
	अन्यथा जर (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*port);
	    *port = (अक्षर *)vim_strsave(&buf[5]);
	}
	अन्यथा जर (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (अक्षर *)vim_strsave(&buf[5]);
	}
    }
    ख_बंद(fp);

    परत OK;
}


संरचना keyqueue
{
    char_u	    *keystr;
    संरचना keyqueue *next;
    संरचना keyqueue *prev;
};

प्रकारव्याख्या संरचना keyqueue किल्ली_Q_T;

स्थिर किल्ली_Q_T किल्ली_Head; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    स्थिर शून्य
postpone_keycommand(char_u *keystr)
{
    किल्ली_Q_T *गाठ;

    गाठ = ALLOC_ONE(किल्ली_Q_T);
    जर (गाठ == रिक्त)
	परत;  // out of memory, drop the key

    जर (किल्ली_Head.next == रिक्त) // initialize circular queue
    {
	किल्ली_Head.next = &किल्ली_Head;
	किल्ली_Head.prev = &किल्ली_Head;
    }

    // insert node at tail of queue
    गाठ->next = &किल्ली_Head;
    गाठ->prev = किल्ली_Head.prev;
    किल्ली_Head.prev->next = गाठ;
    किल्ली_Head.prev = गाठ;

    गाठ->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    स्थिर शून्य
handle_किल्ली_queue(शून्य)
{
    पूर्णांक postponed = FALSE;

    पर्यंत (!postponed && किल्ली_Head.next && किल्ली_Head.next != &किल्ली_Head)
    {
	// first, unlink the node
	किल्ली_Q_T *गाठ = किल्ली_Head.next;
	किल्ली_Head.next = गाठ->next;
	गाठ->next->prev = गाठ->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	जर (गाठ->keystr != रिक्त)
	    postponed = !netbeans_keystring(गाठ->keystr);
	vim_free(गाठ->keystr);

	// Finally, dispose of the node
	vim_free(गाठ);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    शून्य
netbeans_parse_messages(शून्य)
{
    readq_T	*गाठ;
    char_u	*buffer;
    char_u	*p;
    पूर्णांक		own_गाठ;

    पर्यंत (nb_channel != रिक्त)
    {
	गाठ = channel_peek(nb_channel, PART_SOCK);
	जर (गाठ == रिक्त)
	    थांबा;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_first_nl(गाठ);
	जर (p == रिक्त)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    जर (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		परत;
	    सुरूठेवा;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	जर (*p == NUL)
	{
	    own_गाठ = TRUE;
	    buffer = channel_get(nb_channel, PART_SOCK, रिक्त);
	    // "node" is now invalid!
	}
	अन्यथा
	{
	    own_गाठ = FALSE;
	    buffer = गाठ->rq_buffer;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(buffer);

	जर (own_गाठ)
	    // buffer finished, dispose of it
	    vim_free(buffer);
	अन्यथा जर (nb_channel != रिक्त)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (पूर्णांक)(p - buffer));
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
    अक्षर	*verb;
    अक्षर	*q;
    पूर्णांक		bufno;
    पूर्णांक		isfunc = -1;

    जर (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    जर (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_column = false;

	// The IDE is breaking the connection.
	netbeans_close();
	परत;
    }

    bufno = म_से_दीर्घ((अक्षर *)cmd, &verb, 10);

    जर (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	परत;
    }
    ++verb; // skip colon

    साठी (q = verb; *q; q++)
    {
	जर (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    थांबा;
	}
	अन्यथा जर (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    थांबा;
	}
    }

    जर (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_in_str), cmd);
	परत;
    }

    r_cmdno = म_से_दीर्घ(q, &q, 10);

    q = (अक्षर *)skipwhite((char_u *)q);

    जर (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
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
    अचिन्हित पूर्णांक	 fireChanges:1;
    अचिन्हित पूर्णांक	 initDone:1;
    अचिन्हित पूर्णांक	 घाला_Done:1;
    अचिन्हित पूर्णांक	 modified:1;
    पूर्णांक			 nbbuf_संख्या;
    अक्षर		*displayname;
    पूर्णांक			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

प्रकारव्याख्या संरचना nbbuf_struct nbbuf_T;

स्थिर nbbuf_T *buf_list = रिक्त;
स्थिर पूर्णांक buf_list_size = 0;	// size of buf_list
स्थिर पूर्णांक buf_list_used = 0;	// nr of entries in buf_list actually in use

स्थिर अक्षर **globalsignmap = रिक्त;
स्थिर पूर्णांक globalsignmaplen = 0;
स्थिर पूर्णांक globalsignmapused = 0;

स्थिर पूर्णांक  mapsigntype(nbbuf_T *, पूर्णांक localsigntype);
स्थिर शून्य addsigntype(nbbuf_T *, पूर्णांक localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
स्थिर शून्य print_read_संदेश(nbbuf_T *buf);
स्थिर शून्य print_save_संदेश(nbbuf_T *buf, off_T nchars);

स्थिर पूर्णांक curPCtype = -1;

/*
 * Free netbeans resources.
 */
    स्थिर शून्य
nb_free(शून्य)
{
    किल्ली_Q_T *किल्ली_गाठ = किल्ली_Head.next;
    nbbuf_T buf;
    पूर्णांक i;

    // free the netbeans buffer list
    साठी (i = 0; i < buf_list_used; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	जर (buf.bufp != रिक्त && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_size = 0;
    buf_list_used = 0;

    // free the queued key commands
    पर्यंत (किल्ली_गाठ != रिक्त && किल्ली_गाठ != &किल्ली_Head)
    {
	किल्ली_Q_T *next = किल्ली_गाठ->next;
	vim_free(किल्ली_गाठ->keystr);
	vim_free(किल्ली_गाठ);
	जर (next == &किल्ली_Head)
	{
	    किल्ली_Head.next = &किल्ली_Head;
	    किल्ली_Head.prev = &किल्ली_Head;
	    थांबा;
	}
	किल्ली_गाठ = next;
    }

    // free the queued netbeans commands
    जर (nb_channel != रिक्त)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    स्थिर पूर्णांक
nb_getbufno(buf_T *bufp)
{
    पूर्णांक i;

    साठी (i = 0; i < buf_list_used; i++)
	जर (buf_list[i].bufp == bufp)
	    परत i;
    परत -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    पूर्णांक
isNetbeansBuffer(buf_T *bufp)
{
    परत NETBEANS_OPEN && bufp->b_netbeans_file;
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
    पूर्णांक
isNetbeansModified(buf_T *bufp)
{
    जर (isNetbeansBuffer(bufp))
    {
	पूर्णांक bufno = nb_getbufno(bufp);

	जर (bufno > 0)
	    परत buf_list[bufno].modified;
	अन्यथा
	    परत FALSE;
    }
    अन्यथा
	परत FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    स्थिर nbbuf_T *
nb_get_buf(पूर्णांक bufno)
{
    // find or create a buffer with the given number
    पूर्णांक incr;

    जर (bufno <= 0)
	परत रिक्त;

    जर (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * आकार(nbbuf_T));
	buf_list_size = 100;
    }
    जर (bufno >= buf_list_used) // new
    {
	जर (bufno >= buf_list_size) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    माप_प्रकार	bufsize;

	    incr = bufno - buf_list_size + 90;
	    buf_list_size += incr;
	    bufsize = buf_list_size * आकार(nbbuf_T);
	    जर (bufsize == 0 || bufsize / आकार(nbbuf_T)
						      != (माप_प्रकार)buf_list_size)
	    {
		// list size overflow, bail out
		परत रिक्त;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    जर (buf_list == रिक्त)
	    {
		vim_free(t_buf_list);
		buf_list_size = 0;
		परत रिक्त;
	    }
	    vim_memset(buf_list + buf_list_size - incr, 0,
						      incr * आकार(nbbuf_T));
	}

	पर्यंत (buf_list_used <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_used].fireChanges = 1;
	    ++buf_list_used;
	}
    }

    परत buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    स्थिर पूर्णांक
मोजणी_changed_buffers(शून्य)
{
    buf_T	*bufp;
    पूर्णांक		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	जर (bufp->b_changed)
	    ++n;
    परत n;
}

/*
 * End the netbeans session.
 */
    शून्य
netbeans_end(शून्य)
{
    पूर्णांक i;
    स्थिर अक्षर buf[128];

    जर (!NETBEANS_OPEN)
	परत;

    साठी (i = 0; i < buf_list_used; i++)
    {
	जर (!buf_list[i].bufp)
	    सुरूठेवा;
	जर (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    प्र_लिखो(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	प्र_लिखो(buf, "%d:killed=%d\n", i, r_cmdno);
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
nb_send(अक्षर *buf, अक्षर *fun)
{
    जर (nb_channel != रिक्त)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (पूर्णांक)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    स्थिर शून्य
nb_reply_nil(पूर्णांक cmdno)
{
    अक्षर reply[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    जर (!NETBEANS_OPEN)
	परत;

    प्र_लिखो(reply, "%d\n", cmdno);
    nb_send(reply, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    स्थिर शून्य
nb_reply_text(पूर्णांक cmdno, char_u *result)
{
    char_u *reply;

    nbdebug(("REP %d: %s\n", cmdno, (अक्षर *)result));

    reply = alloc(STRLEN(result) + 32);
    प्र_लिखो((अक्षर *)reply, "%d %s\n", cmdno, (अक्षर *)result);
    nb_send((अक्षर *)reply, "nb_reply_text");

    vim_free(reply);
}


/*
 * Send a response with a number result code.
 */
    स्थिर शून्य
nb_reply_nr(पूर्णांक cmdno, दीर्घ result)
{
    अक्षर reply[32];

    nbdebug(("REP %d: %ld\n", cmdno, result));

    प्र_लिखो(reply, "%d %ld\n", cmdno, result);
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

    जर (buf == रिक्त)
	परत रिक्त;
    साठी (; *p; p++)
    {
	निवड (*p)
	{
	    स्थिती '\"':
	    स्थिती '\\':
		*q++ = '\\'; *q++ = *p; थांबा;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    स्थिती '\n':
		*q++ = '\\'; *q++ = 'n'; थांबा;
	    स्थिती '\r':
		*q++ = '\\'; *q++ = 'r'; थांबा;
	    पूर्वनिर्धारित:
		*q++ = *p;
		थांबा;
	}
    }
    *q = '\0';

    परत buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    स्थिर अक्षर *
nb_unquote(char_u *p, char_u **endp)
{
    अक्षर *result = 0;
    अक्षर *q;
    पूर्णांक done = 0;

    // result is never longer than input
    result = alloc_clear(STRLEN(p) + 1);
    जर (result == रिक्त)
	परत रिक्त;

    जर (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	result[0] = NUL;
	परत result;
    }

    साठी (q = result; !done && *p != NUL;)
    {
	निवड (*p)
	{
	    स्थिती '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		done = 1;
		थांबा;

	    स्थिती '\\':
		++p;
		निवड (*p)
		{
		    स्थिती '\\':	*q++ = '\\';	थांबा;
		    स्थिती 'n':	*q++ = '\n';	थांबा;
		    स्थिती 't':	*q++ = '\t';	थांबा;
		    स्थिती 'r':	*q++ = '\r';	थांबा;
		    स्थिती '"':	*q++ = '"';	थांबा;
		    स्थिती NUL:	--p;		थांबा;
		    // default: skip over illegal chars
		}
		++p;
		थांबा;

	    पूर्वनिर्धारित:
		*q++ = *p++;
	}
    }

    जर (endp != रिक्त)
	*endp = p;

    परत result;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    स्थिर शून्य
nb_partialremove(linenr_T lnum, colnr_T first, colnr_T last)
{
    char_u *oldtext, *newtext;
    पूर्णांक oldlen;
    पूर्णांक lastbyte = last;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    जर (first >= (colnr_T)oldlen || oldlen == 0)  // just in case
	परत;
    जर (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - first));
    जर (newtext == रिक्त)
	परत;

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
    पूर्णांक len_first, len_other;
    char_u *p;

    len_first = ml_get_len(first);
    len_other = ml_get_len(other);
    p = alloc(len_first + len_other + 1);
    जर (p == रिक्त)
	परत;

    mch_memmove(p, ml_get(first), len_first);
    mch_memmove(p + len_first, ml_get(other), len_other + 1);
    ml_replace(first, p, FALSE);
}

#define SKIP_STOP 2
#define streq(a,b) (म_भेद(a,b) == 0)

/*
 * Do the actual processing of a single netbeans command or function.
 * The difference between a command and function is that a function
 * gets a response (it's required) but a command does not.
 * For arguments see comment for nb_parse_cmd().
 */
    स्थिर पूर्णांक
nb_do_cmd(
    पूर्णांक		bufno,
    char_u	*cmd,
    पूर्णांक		func,
    पूर्णांक		cmdno,
    char_u	*args)	    // points to space before arguments or NUL
{
    पूर्णांक		do_update = 0;
    दीर्घ	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    स्थिर पूर्णांक	skip = 0;
    पूर्णांक		retval = OK;
    अक्षर	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (अक्षर *)args));

    जर (func)
    {
// =====================================================================
	जर (streq((अक्षर *)cmd, "getModified"))
	{
	    जर (buf == रिक्त || buf->bufp == रिक्त)
		// Return the number of buffers that are modified.
		nb_reply_nr(cmdno, (दीर्घ)मोजणी_changed_buffers());
	    अन्यथा
		// Return whether the buffer is modified.
		nb_reply_nr(cmdno, (दीर्घ)(buf->bufp->b_changed
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_reply_nr(cmdno, (दीर्घ)मोजणी_changed_buffers());
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "getCursor"))
	{
	    char_u text[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    प्र_लिखो((अक्षर *)text, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (दीर्घ)curwin->w_cursor.lnum,
		    (पूर्णांक)curwin->w_cursor.col,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_reply_text(cmdno, text);
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "getAnno"))
	{
	    दीर्घ linenum = 0;
#ifdef FEAT_SIGNS
	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_getanno));
		retval = FAIL;
	    }
	    अन्यथा
	    {
		पूर्णांक ser_अंक;

		cp = (अक्षर *)args;
		ser_अंक = म_से_दीर्घ(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (दीर्घ)buf_findsign(buf->bufp, ser_अंक, रिक्त);
	    }
#endif
	    nb_reply_nr(cmdno, linenum);
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "getLength"))
	{
	    दीर्घ len = 0;

	    जर (buf == रिक्त || buf->bufp == रिक्त)
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
	अन्यथा जर (streq((अक्षर *)cmd, "getText"))
	{
	    दीर्घ	len;
	    linenr_T	nlines;
	    char_u	*text = रिक्त;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*ओळ;

	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_buffer_identifier_in_gettext));
		retval = FAIL;
	    }
	    अन्यथा
	    {
		len = get_buf_size(buf->bufp);
		nlines = buf->bufp->b_ml.ml_ओळ_मोजणी;
		text = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		जर (text == रिक्त)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		अन्यथा
		{
		    p = text;
		    *p++ = '\"';
		    साठी (; lno <= nlines ; lno++)
		    {
			ओळ = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			जर (ओळ != रिक्त)
			{
			    STRCPY(p, ओळ);
			    p += STRLEN(ओळ);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(ओळ);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    जर (text == रिक्त)
		nb_reply_text(cmdno, (char_u *)"");
	    अन्यथा
	    {
		nb_reply_text(cmdno, text);
		vim_free(text);
	    }
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "remove"))
	{
	    दीर्घ मोजणी;
	    pos_T first, last;
	    pos_T *pos;
	    pos_T *next;
	    linenr_T del_from_lnum, del_to_lnum;  // lines to be deleted as a whole
	    पूर्णांक oldFire = netbeansFireChanges;
	    पूर्णांक oldSuppress = netbeans_Suppress_No_ओळी;
	    पूर्णांक wasChanged;

	    जर (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (अक्षर *) cmd));
		nb_reply_nil(cmdno);
		परत OK;
	    }

	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_buffer_identifier_in_remove));
		retval = FAIL;
	    }
	    अन्यथा
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_ओळी = TRUE;

		nb_set_curbuf(buf->bufp);
		wasChanged = buf->bufp->b_changed;
		cp = (अक्षर *)args;
		off = म_से_दीर्घ(cp, &cp, 10);
		मोजणी = म_से_दीर्घ(cp, &cp, 10);
		args = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		जर (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_ओळी = oldSuppress;
		    परत FAIL;
		}
		first = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      first.lnum, first.col));
		pos = off2pos(buf->bufp, off+मोजणी-1);
		जर (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_ओळी = oldSuppress;
		    परत FAIL;
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
		next = off2pos(buf->bufp, off + मोजणी);

		// Remove part of the first line.
		जर (first.col != 0
				|| (next != रिक्त && first.lnum == next->lnum))
		{
		    जर (first.lnum != last.lnum
			    || (next != रिक्त && first.lnum != next->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(first.lnum, first.col,
							     (colnr_T)MAXCOL);
			जर (first.lnum == last.lnum)
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
		जर (first.lnum != last.lnum && next != रिक्त
			&& next->col != 0 && last.lnum == next->lnum)
		{
		    nb_partialremove(last.lnum, 0, last.col);
		    जर (del_from_lnum > first.lnum)
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
		जर (first.lnum != last.lnum && del_from_lnum > first.lnum
			&& next != रिक्त && last.lnum != next->lnum)
		{
		    nb_joinlines(first.lnum, next->lnum);
		    del_to_lnum = next->lnum;
		}

		// Delete whole lines if there are any.
		जर (del_to_lnum >= del_from_lnum)
		{
		    पूर्णांक i;

		    // delete signs from the lines being deleted
		    साठी (i = del_from_lnum; i <= del_to_lnum; i++)
		    {
			पूर्णांक id = buf_findsign_id(buf->bufp, (linenr_T)i, रिक्त);
			जर (id > 0)
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
		    del_ओळी(del_to_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = first;
		check_cursor_lnum();
		buf->bufp->b_changed = wasChanged; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_ओळी = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_reply_nil(cmdno);
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "insert"))
	{
	    char_u	*to_free;

	    जर (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (अक्षर *) cmd));
		nb_reply_nil(cmdno);
		परत OK;
	    }

	    // get offset
	    cp = (अक्षर *)args;
	    off = म_से_दीर्घ(cp, &cp, 10);
	    args = (char_u *)cp;

	    // get text to be inserted
	    args = skipwhite(args);
	    args = to_free = (char_u *)nb_unquote(args, रिक्त);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_buffer_identifier_in_घाला));
		retval = FAIL;
	    }
	    अन्यथा जर (args != रिक्त)
	    {
		पूर्णांक	ff_detected = EOL_UNKNOWN;
		पूर्णांक	buf_was_empty = (buf->bufp->b_ml.ml_flags & ML_EMPTY);
		माप_प्रकार	len = 0;
		पूर्णांक	added = 0;
		पूर्णांक	oldFire = netbeansFireChanges;
		पूर्णांक	old_b_changed;
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
		जर (pos != रिक्त)
		{
		    जर (pos->lnum <= 0)
			lnum_start = 1;
		    अन्यथा
			lnum_start = pos->lnum;
		}
		अन्यथा
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    जर (buf_was_empty)
			lnum_start = 1;	    // above empty line
		    अन्यथा
			lnum_start = curbuf->b_ml.ml_ओळ_मोजणी + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_start;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		पर्यंत (*args != NUL)
		{
		    nlp = vim_strchr(args, '\n');
		    जर (nlp == रिक्त)
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
			जर (nlp > args && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			अन्यथा
			    ff_detected = EOL_UNIX;
		    }
		    args[len] = NUL;

		    जर (lnum == lnum_start
			    && ((pos != रिक्त && pos->col > 0)
				|| (lnum == 1 && buf_was_empty)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			पूर्णांक	col = pos == रिक्त ? 0 : pos->col;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			जर (newline != रिक्त)
			{
			    mch_memmove(newline, oldline, (माप_प्रकार)col);
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

		    जर (nlp == रिक्त)
			थांबा;
		    ++lnum;
		    args = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_ओळी_mark(lnum_start - 1, (दीर्घ)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		जर (buf_was_empty)
		{
		    जर (ff_detected == EOL_UNKNOWN)
#जर defined(MSWIN)
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
	जर (streq((अक्षर *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    जर (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_buffer_identifier_in_create));
		परत FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, रिक्त, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->घाला_Done = FALSE;
#जर defined(FEAT_MENU) && defined(FEAT_GUI)
	    जर (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "insertDone"))
	{
	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    अन्यथा
	    {
		buf->bufp->b_start_eol = *args == 'T';
		buf->घाला_Done = TRUE;
		args += 2;
		buf->bufp->b_p_ro = *args == 'T';
		print_read_संदेश(buf);
	    }
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "saveDone"))
	{
	    दीर्घ savedChars = म_से_द((अक्षर *)args);

	    जर (buf == रिक्त || buf->bufp == रिक्त)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    अन्यथा
		print_save_संदेश(buf, savedChars);
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "startDocumentListen"))
	{
	    जर (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_startdocumentlisten));
		परत FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "stopDocumentListen"))
	{
	    जर (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_stopdocumentlisten));
		परत FAIL;
	    }
	    buf->fireChanges = 0;
	    जर (buf->bufp != रिक्त && buf->bufp->b_was_netbeans_file)
	    {
		जर (!buf->bufp->b_netbeans_file)
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
	अन्यथा जर (streq((अक्षर *)cmd, "setTitle"))
	{
	    जर (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_buffer_identifier_in_settitle));
		परत FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, रिक्त);
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "initDone"))
	{
	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_buffer_identifier_in_initdone));
		परत FAIL;
	    }
	    do_update = 1;
	    buf->initDone = TRUE;
	    nb_set_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_किल्ली_queue();
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "setBufferNumber")
		|| streq((अक्षर *)cmd, "putBufferNumber"))
	{
	    char_u	*path;
	    buf_T	*bufp;

	    जर (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_buffer_identifier_in_setbuffernumber));
		परत FAIL;
	    }
	    path = (char_u *)nb_unquote(args, रिक्त);
	    जर (path == रिक्त)
		परत FAIL;
	    bufp = buflist_findname(path);
	    vim_free(path);
	    जर (bufp == रिक्त)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", args));
		semsg(_(e_file_str_not_found_in_setbuffernumber), args);
		परत FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_संख्या = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    जर (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    अन्यथा
	    {
		buf->initDone = TRUE;

		// handle any postponed key commands
		handle_किल्ली_queue();
	    }

// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "setFullName"))
	{
	    जर (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_buffer_identifier_in_setfullname));
		परत FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, रिक्त);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#जर defined(FEAT_MENU) && defined(FEAT_GUI)
	    जर (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "editFile"))
	{
	    जर (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_buffer_identifier_in_editfile));
		परत FAIL;
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
#जर defined(FEAT_MENU) && defined(FEAT_GUI)
	    जर (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "setVisible"))
	{
	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		जर (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setvisible));
		परत FAIL;
	    }
	    जर (streq((अक्षर *)args, "T") && buf->bufp != curbuf)
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
		जर (gui.in_use)
		    gui_mch_set_foreground();
#endif
	    }
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "raise"))
	{
#ifdef FEAT_GUI
	    // Bring gvim to the foreground.
	    जर (gui.in_use)
		gui_mch_set_foreground();
#endif
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "setModified"))
	{
	    पूर्णांक prev_b_changed;

	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		जर (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setmodified));
		परत FAIL;
	    }
	    prev_b_changed = buf->bufp->b_changed;
	    जर (streq((अक्षर *)args, "T"))
		buf->bufp->b_changed = TRUE;
	    अन्यथा
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		जर (buf->bufp->b_ffname != रिक्त
			&& mch_stat((अक्षर *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_time(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_changed = FALSE;
	    }
	    buf->modified = buf->bufp->b_changed;
	    जर (prev_b_changed != buf->bufp->b_changed)
	    {
		check_status(buf->bufp);
		redraw_tabline = TRUE;
#जर defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "setModtime"))
	{
	    जर (buf == रिक्त || buf->bufp == रिक्त)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    अन्यथा
	    {
		buf->bufp->b_mtime = म_से_प((अक्षर *)args);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "setReadOnly"))
	{
	    जर (buf == रिक्त || buf->bufp == रिक्त)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    अन्यथा जर (streq((अक्षर *)args, "T"))
		buf->bufp->b_p_ro = TRUE;
	    अन्यथा
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "showBalloon"))
	{
#जर defined(FEAT_BEVAL_GUI)
	    स्थिर अक्षर	*text = रिक्त;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    जर (balloonEval != रिक्त)
	    {
		vim_free(text);
		text = nb_unquote(args, रिक्त);
		जर (text != रिक्त)
		    gui_mch_post_balloon(balloonEval, (char_u *)text);
	    }
#endif
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "setDot"))
	{
	    pos_T *pos;
#ifdef NBDEBUG
	    char_u *s;
#endif

	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_buffer_identifier_in_setdot));
		परत FAIL;
	    }

	    nb_set_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    जर (VIsual_active)
		end_visual_mode();
#ifdef NBDEBUG
	    s = args;
#endif
	    pos = get_off_or_lnum(buf->bufp, &args);
	    जर (pos)
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
	    जर (State == MODE_HITRETURN || State == MODE_ASKMORE)
	    {
#ifdef FEAT_GUI_GTK
		जर (gui.in_use && gtk_main_level() > 0)
		    gtk_main_quit();
#endif
	    }
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "close"))
	{
#ifdef NBDEBUG
	    अक्षर *name = "<NONE>";
#endif

	    जर (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_buffer_identifier_in_close));
		परत FAIL;
	    }

#ifdef NBDEBUG
	    जर (buf->displayname != रिक्त)
		name = buf->displayname;
#endif
	    जर (buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		जर (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    जर (buf->bufp != रिक्त)
		do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = रिक्त;
	    buf->initDone = FALSE;
	    do_update = 1;
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "defineAnnoType"))
	{
#ifdef FEAT_SIGNS
	    पूर्णांक type_अंक;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    पूर्णांक parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    जर (buf == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_buffer_identifier_in_defineannotype));
		परत FAIL;
	    }

	    cp = (अक्षर *)args;
	    type_अंक = म_से_दीर्घ(cp, &cp, 10);
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
	    जर (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(args);
	    bg = vim_strsave(p);
	    जर (STRLEN(fg) > MAX_COLOR_LENGTH || STRLEN(bg) > MAX_COLOR_LENGTH)
	    {
		emsg(_(e_highlighting_color_name_too_long_in_defineAnnoType));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    अन्यथा जर (!nb_is_safe_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_safe_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_safe_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_in_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    अन्यथा जर (typeName != रिक्त && tooltip != रिक्त && glyphFile != रिक्त)
		addsigntype(buf, type_अंक, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    जर (parse_error)
		परत FAIL;

#endif
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "addAnno"))
	{
#ifdef FEAT_SIGNS
	    पूर्णांक ser_अंक;
	    पूर्णांक local_Type_अंक;
	    पूर्णांक type_अंक;
	    pos_T *pos;

	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_addanno));
		परत FAIL;
	    }

	    do_update = 1;

	    cp = (अक्षर *)args;
	    ser_अंक = म_से_दीर्घ(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_अंक = म_से_दीर्घ(cp, &cp, 10);
	    args = (char_u *)cp;
	    type_अंक = mapsigntype(buf, local_Type_अंक);

	    pos = get_off_or_lnum(buf->bufp, &args);

	    cp = (अक्षर *)args;
	    vim_ignored = (पूर्णांक)म_से_दीर्घ(cp, &cp, 10);
	    args = (char_u *)cp;
# ifdef NBDEBUG
	    जर (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    जर (ser_अंक >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		परत FAIL;
	    }
	    जर (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_अंक, pos->lnum, type_अंक, buf->bufp->b_fnum);
		जर (type_अंक == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_अंक,
						       buf->bufp->b_fnum);
	    }
#endif
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "removeAnno"))
	{
#ifdef FEAT_SIGNS
	    पूर्णांक ser_अंक;

	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		परत FAIL;
	    }
	    do_update = 1;
	    cp = (अक्षर *)args;
	    ser_अंक = म_से_दीर्घ(cp, &cp, 10);
	    args = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_अंक, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#endif
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "moveAnnoToFront"))
	{
#ifdef FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#endif
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "guard") || streq((अक्षर *)cmd, "unguard"))
	{
	    पूर्णांक len;
	    pos_T first;
	    pos_T last;
	    pos_T *pos;
	    पूर्णांक un = (cmd[0] == 'u');
	    स्थिर पूर्णांक guardId = GUARDEDOFFSET;

	    जर (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (अक्षर *) cmd));
		परत OK;
	    }

	    nb_init_graphics();

	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		परत FAIL;
	    }
	    nb_set_curbuf(buf->bufp);
	    cp = (अक्षर *)args;
	    off = म_से_दीर्घ(cp, &cp, 10);
	    len = म_से_दीर्घ(cp, रिक्त, 10);
	    args = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    जर (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    अन्यथा
	    {
		first = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		जर (pos != रिक्त && pos->col == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		जर (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		अन्यथा
		{
		    दीर्घ lnum;
		    last = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     first.lnum, first.col,
			     last.lnum, last.col));
#ifdef FEAT_SIGNS
		    साठी (lnum = first.lnum; lnum <= last.lnum; lnum++)
		    {
			जर (un)
			{
			    // never used
			}
			अन्यथा
			{
			    जर (buf_findsigntype_id(buf->bufp, lnum,
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
	अन्यथा जर (streq((अक्षर *)cmd, "startAtomic"))
	{
	    inAtomic = 1;
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "endAtomic"))
	{
	    inAtomic = 0;
	    जर (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		परत FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    जर (bufIsChanged(buf->bufp))
	    {
		// Only write if the buffer can be written.
		जर (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != रिक्त
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    जर (!bufref_valid(&bufref))
			buf->bufp = रिक्त;
		}
	    }
	    अन्यथा
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "netbeansBuffer"))
	{
	    जर (buf == रिक्त || buf->bufp == रिक्त)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		परत FAIL;
	    }
	    जर (*args == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    अन्यथा
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "specialKeys"))
	{
	    special_किल्ल्या(args);
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	अन्यथा जर (streq((अक्षर *)cmd, "version"))
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
    जर (inAtomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    जर (buf != रिक्त && buf->initDone && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	out_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	जर (State == MODE_HITRETURN || State == MODE_ASKMORE)
	{
#ifdef FEAT_GUI_GTK
	    जर (gui.in_use && gtk_main_level() > 0)
		gtk_main_quit();
#endif
	}
    }

    परत retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    स्थिर शून्य
nb_set_curbuf(buf_T *buf)
{
    जर (curbuf == buf)
	परत;

    जर (buf_jump_open_win(buf) != रिक्त)
	परत;
    जर ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != रिक्त)
	परत;
    set_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    स्थिर शून्य
coloncmd(अक्षर *cmd, ...)
{
    अक्षर buf[1024];
    बहु_सूची ap;

    बहु_शुरू(ap, cmd);
    vim_vsnprintf(buf, आकार(buf), cmd, ap);
    बहु_पूर्ण(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, रिक्त, रिक्त, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    out_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    स्थिर शून्य
special_किल्ल्या(char_u *args)
{
    अक्षर *save_str = nb_unquote(args, रिक्त);
    अक्षर *tok = म_मोहर(save_str, " ");
    अक्षर *sep;
#define KEYBUFLEN 64
    अक्षर keybuf[KEYBUFLEN];
    अक्षर cmdbuf[256];

    पर्यंत (tok != रिक्त)
    {
	पूर्णांक i = 0;

	जर ((sep = म_अक्षर(tok, '-')) != रिक्त)
	{
	    *sep = NUL;
	    पर्यंत (*tok && i + 2 < KEYBUFLEN)
	    {
		निवड (*tok)
		{
		    स्थिती 'A':
		    स्थिती 'M':
		    स्थिती 'C':
		    स्थिती 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			थांबा;
		}
		tok++;
	    }
	    tok++;
	}

	जर (म_माप(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    पूर्णांक safe = TRUE;
	    साठी (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		जर (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    safe = FALSE;
		    थांबा;
		}
	    }
	    जर (safe)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, आकार(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, MODE_NORMAL, FALSE);
	    }
	}
	tok = म_मोहर(रिक्त, " ");
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
# जर !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    जर (gui.in_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	परत;
    }
# endif
#endif
    netbeans_open((अक्षर *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    स्थिर शून्य
nb_init_graphics(शून्य)
{
    स्थिर पूर्णांक did_init = FALSE;

    जर (did_init)
	परत;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    स्थिर शून्य
netbeans_keyname(पूर्णांक किल्ली, अक्षर *buf)
{
    अक्षर *name = 0;
    अक्षर namebuf[2];
    पूर्णांक ctrl  = 0;
    पूर्णांक shift = 0;
    पूर्णांक alt   = 0;

    जर (mod_mask & MOD_MASK_CTRL)
	ctrl = 1;
    जर (mod_mask & MOD_MASK_SHIFT)
	shift = 1;
    जर (mod_mask & MOD_MASK_ALT)
	alt = 1;


    निवड (किल्ली)
    {
	स्थिती K_F1:		name = "F1";		थांबा;
	स्थिती K_S_F1:	name = "F1";	shift = 1;	थांबा;
	स्थिती K_F2:		name = "F2";		थांबा;
	स्थिती K_S_F2:	name = "F2";	shift = 1;	थांबा;
	स्थिती K_F3:		name = "F3";		थांबा;
	स्थिती K_S_F3:	name = "F3";	shift = 1;	थांबा;
	स्थिती K_F4:		name = "F4";		थांबा;
	स्थिती K_S_F4:	name = "F4";	shift = 1;	थांबा;
	स्थिती K_F5:		name = "F5";		थांबा;
	स्थिती K_S_F5:	name = "F5";	shift = 1;	थांबा;
	स्थिती K_F6:		name = "F6";		थांबा;
	स्थिती K_S_F6:	name = "F6";	shift = 1;	थांबा;
	स्थिती K_F7:		name = "F7";		थांबा;
	स्थिती K_S_F7:	name = "F7";	shift = 1;	थांबा;
	स्थिती K_F8:		name = "F8";		थांबा;
	स्थिती K_S_F8:	name = "F8";	shift = 1;	थांबा;
	स्थिती K_F9:		name = "F9";		थांबा;
	स्थिती K_S_F9:	name = "F9";	shift = 1;	थांबा;
	स्थिती K_F10:		name = "F10";		थांबा;
	स्थिती K_S_F10:	name = "F10";	shift = 1;	थांबा;
	स्थिती K_F11:		name = "F11";		थांबा;
	स्थिती K_S_F11:	name = "F11";	shift = 1;	थांबा;
	स्थिती K_F12:		name = "F12";		थांबा;
	स्थिती K_S_F12:	name = "F12";	shift = 1;	थांबा;
	पूर्वनिर्धारित:
			जर (किल्ली >= ' ' && किल्ली <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = किल्ली;
			    namebuf[1] = NUL;
			}
			अन्यथा
			    name = "X";
			थांबा;
    }

    buf[0] = '\0';
    जर (ctrl)
	म_जोड़ो(buf, "C");
    जर (shift)
	म_जोड़ो(buf, "S");
    जर (alt)
	म_जोड़ो(buf, "M"); // META
    जर (ctrl || shift || alt)
	म_जोड़ो(buf, "-");
    म_जोड़ो(buf, name);
}

#जर defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    शून्य
netbeans_beval_cb(
	BalloonEval	*beval,
	पूर्णांक		 state UNUSED)
{
    win_T	*wp;
    char_u	*text;
    linenr_T	lnum;
    पूर्णांक		col;
    अक्षर	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    जर (!can_use_beval() || !NETBEANS_OPEN)
	परत;

    जर (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) != OK)
	परत;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    जर (text != रिक्त && text[0] != NUL && STRLEN(text) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	जर (buf != रिक्त)
	{
	    p = nb_quote(text);
	    जर (p != रिक्त)
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
    पूर्णांक
netbeans_active(शून्य)
{
    परत NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    शून्य
netbeans_open(अक्षर *params, पूर्णांक doabort)
{
    अक्षर *cmd = "0:startupDone=0\n";

    जर (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	परत;
    }

    जर (netbeans_connect(params, doabort) != OK)
	परत;

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
    अक्षर buf[128];

    जर (NETBEANS_OPEN)
    {
	प्र_लिखो(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#जर defined(FEAT_EVAL)
    पूर्णांक
set_ref_in_nb_channel(पूर्णांक copyID)
{
    पूर्णांक पात = FALSE;
    typval_T tv;

    जर (nb_channel == रिक्त)
	परत FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    पात = set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
    परत पात;
}
#endif

#जर defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    शून्य
netbeans_frame_moved(पूर्णांक new_x, पूर्णांक new_y)
{
    अक्षर buf[128];

    जर (!NETBEANS_OPEN)
	परत;

    प्र_लिखो(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (पूर्णांक)Columns, (पूर्णांक)Rows, new_x, new_y);
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
    पूर्णांक bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    अक्षर    buffer[2*MAXPATHL];
    char_u  *q;

    जर (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	परत;

    q = nb_quote(bufp->b_ffname);
    जर (q == रिक्त)
	परत;
    जर (bp == रिक्त)
    {
	vim_free(q);
	परत;
    }

    vim_snprintf(buffer, आकार(buffer),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (अक्षर *)q,
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
    पूर्णांक bufno = nb_getbufno(bufp);
    अक्षर    buffer[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    पूर्णांक	    bnum;

    जर (!NETBEANS_OPEN)
	परत;

    q = nb_quote(bufp->b_ffname);
    जर (q == रिक्त)
	परत;
    जर (bp != रिक्त)
	bnum = bufno;
    अन्यथा
	bnum = 0;

    vim_snprintf(buffer, आकार(buffer), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (अक्षर *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
    जर (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
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
    पूर्णांक		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    अक्षर	buffer[2*MAXPATHL];

    जर (!NETBEANS_OPEN || bufno == -1)
	परत;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    प्र_लिखो(buffer, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_killed");

    जर (nbbuf != रिक्त)
	nbbuf->bufp = रिक्त;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    स्थिर nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, पूर्णांक *bufnop)
{
    पूर्णांक		bufno;
    nbbuf_T	*nbbuf;

    जर (!NETBEANS_OPEN || !netbeansFireChanges)
	परत रिक्त;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    जर (bufno <= 0)
	परत रिक्त;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    जर (nbbuf != रिक्त && !nbbuf->fireChanges)
	परत रिक्त;		// changes in this buffer are not reported

    *bufnop = bufno;
    परत nbbuf;
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
    पूर्णांक		newlen)
{
    char_u	*buf;
    पूर्णांक		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    दीर्घ	off;
    char_u	*p;
    char_u	*newtxt;

    जर (!NETBEANS_OPEN)
	परत;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    जर (nbbuf == रिक्त)
	परत;

    // Don't mark as modified for initial read
    जर (nbbuf->घाला_Done)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.col = col;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    जर (p != रिक्त)
    {
	buf = alloc(128 + 2*newlen);
	प्र_लिखो((अक्षर *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((अक्षर *)buf, "netbeans_inserted");
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
    दीर्घ	len)
{
    char_u	buf[128];
    पूर्णांक		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    दीर्घ	off;

    जर (!NETBEANS_OPEN)
	परत;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    जर (nbbuf == रिक्त)
	परत;

    जर (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	परत;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.col = col;

    off = pos2off(bufp, &pos);

    प्र_लिखो((अक्षर *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((अक्षर *)buf, "netbeans_removed");
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
netbeans_button_release(पूर्णांक button)
{
    अक्षर	buf[128];
    पूर्णांक		bufno;

    जर (!NETBEANS_OPEN)
	परत;

    bufno = nb_getbufno(curbuf);

    जर (bufno < 0 || curwin == रिक्त || curwin->w_buffer != curbuf)
	परत;

    पूर्णांक col = mouse_col - curwin->w_wincol
	- ((curwin->w_p_nu || curwin->w_p_rnu) ? 9 : 1);
    दीर्घ off = pos2off(curbuf, &curwin->w_cursor);

    // sync the cursor position
    प्र_लिखो(buf, "%d:newDotAndMark=%d %ld %ld\n", bufno, r_cmdno, off, off);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release[newDotAndMark]");

    प्र_लिखो(buf, "%d:buttonRelease=%d %d %ld %d\n", bufno, r_cmdno,
	    button, (दीर्घ)curwin->w_cursor.lnum, col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release");
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key code.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    पूर्णांक
netbeans_keycommand(पूर्णांक किल्ली)
{
    अक्षर	किल्ली_Name[60];

    netbeans_keyname(किल्ली, किल्ली_Name);
    परत netbeans_keystring((char_u *)किल्ली_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    स्थिर पूर्णांक
netbeans_keystring(char_u *किल्ली_Name)
{
    अक्षर	buf[2*MAXPATHL];
    पूर्णांक		bufno = nb_getbufno(curbuf);
    दीर्घ	off;
    char_u	*q;

    जर (!NETBEANS_OPEN)
	परत TRUE;

    जर (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == रिक्त ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	जर (q == रिक्त)
	    परत TRUE;
	vim_snprintf(buf, आकार(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	जर (curbuf->b_ffname != रिक्त)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(किल्ली_Name);
	परत FALSE;
    }

    // sync the cursor position
    off = pos2off(curbuf, &curwin->w_cursor);
    प्र_लिखो(buf, "%d:newDotAndMark=%d %ld %ld\n", bufno, r_cmdno, off, off);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // To work on Win32 you must apply patch to ExtEditor module
    // from ExtEdCaret.java.diff - make EVT_newDotAndMark handler
    // more synchronous

    // now send keyCommand event
    vim_snprintf(buf, आकार(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, किल्ली_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, आकार(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, किल्ली_Name,
		off, (दीर्घ)curwin->w_cursor.lnum, (दीर्घ)curwin->w_cursor.col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    परत TRUE;
}


/*
 * Send a save event to netbeans.
 */
    शून्य
netbeans_save_buffer(buf_T *bufp)
{
    char_u	buf[64];
    पूर्णांक		bufno;
    nbbuf_T	*nbbuf;

    जर (!NETBEANS_OPEN)
	परत;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    जर (nbbuf == रिक्त)
	परत;

    nbbuf->modified = 0;

    प्र_लिखो((अक्षर *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((अक्षर *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    शून्य
netbeans_deleted_all_ओळी(buf_T *bufp)
{
    char_u	buf[64];
    पूर्णांक		bufno;
    nbbuf_T	*nbbuf;

    जर (!NETBEANS_OPEN)
	परत;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    जर (nbbuf == रिक्त)
	परत;

    // Don't mark as modified for initial read
    जर (nbbuf->घाला_Done)
	nbbuf->modified = 1;

    प्र_लिखो((अक्षर *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    पूर्णांक
netbeans_is_guarded(linenr_T शीर्ष, linenr_T bot)
{
    sign_entry_T	*p;
    पूर्णांक			lnum;

    जर (!NETBEANS_OPEN)
	परत FALSE;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
	जर (p->se_id >= GUARDEDOFFSET)
	    साठी (lnum = शीर्ष + 1; lnum < bot; lnum++)
		जर (lnum == p->se_lnum)
		    परत TRUE;

    परत FALSE;
}

#जर defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    शून्य
netbeans_draw_multisign_indicator(पूर्णांक row)
{
    पूर्णांक i;
    पूर्णांक y;
    पूर्णांक x;

    जर (!NETBEANS_OPEN)
	परत;

    x = 0;
    y = row * gui.char_height + 2;

    साठी (i = 0; i < gui.char_height - 3; i++)
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

#जर defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    शून्य
netbeans_draw_multisign_indicator(पूर्णांक row)
{
    पूर्णांक i;
    पूर्णांक y;
    पूर्णांक x;
# जर GTK_CHECK_VERSION(3,0,0)
    cairo_t *cr = रिक्त;
# अन्यथा
    GdkDrawable *drawable = gui.drawarea->window;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    जर (!NETBEANS_OPEN)
	परत;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# अन्यथा
    y = row * gui.char_height + 2;
# endif

# जर GTK_CHECK_VERSION(3,0,0)
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

    साठी (i = 0; i < gui.char_height - 3; i++)
# जर GTK_CHECK_VERSION(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# अन्यथा
    gdk_draw_point(drawable, gui.text_gc, x+2, y++);
# endif

# जर GTK_CHECK_VERSION(3,0,0)
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

# जर GTK_CHECK_VERSION(3,0,0)
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

    जर (!NETBEANS_OPEN)
	परत;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
    {
	जर (p->se_lnum == lnum && p->se_next && p->se_next->se_lnum == lnum)
	{
	    sign_entry_T *tail;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    जर (p->se_prev)
		p->se_prev->se_next = p->se_next;
	    अन्यथा
		curbuf->b_signlist = p->se_next;
	    p->se_next->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    साठी (tail = p->se_next;
		  tail->se_next && tail->se_next->se_lnum == lnum
				       && tail->se_next->se_id < GUARDEDOFFSET;
		  tail = tail->se_next)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_next = tail->se_next;
	    जर (tail->se_next)
		tail->se_next->se_prev = p;
	    p->se_prev = tail;
	    tail->se_next = p;
	    update_debug_sign(curbuf, lnum);
	    थांबा;
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
    पूर्णांक		type_अंक,
    char_u	*typeName,
    char_u	*tooltip UNUSED,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    पूर्णांक i, j;
    पूर्णांक use_fg = (*fg && STRCMP(fg, "none") != 0);
    पूर्णांक use_bg = (*bg && STRCMP(bg, "none") != 0);

    साठी (i = 0; i < globalsignmapused; i++)
	जर (STRCMP(typeName, globalsignmap[i]) == 0)
	    थांबा;

    जर (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_अंक, typeName, tooltip, glyphFile, fg, bg));
	जर (use_fg || use_bg)
	{
	    अक्षर fgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    अक्षर bgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    अक्षर *ptr;
	    पूर्णांक मूल्य;

	    मूल्य = म_से_दीर्घ((अक्षर *)fg, &ptr, 10);
	    जर (ptr != (अक्षर *)fg)
		प्र_लिखो(fgbuf, "guifg=#%06x", मूल्य & 0xFFFFFF);
	    अन्यथा
		प्र_लिखो(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    मूल्य = म_से_दीर्घ((अक्षर *)bg, &ptr, 10);
	    जर (ptr != (अक्षर *)bg)
		प्र_लिखो(bgbuf, "guibg=#%06x", मूल्य & 0xFFFFFF);
	    अन्यथा
		प्र_लिखो(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    जर (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    अन्यथा जर (vim_strsize(glyphFile) <= 2)
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

	जर (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_अंक;

	जर (globalsignmapused == globalsignmaplen)
	{
	    जर (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(अक्षर *, globalsignmaplen);
	    }
	    अन्यथा    // grow it
	    {
		पूर्णांक incr;
		पूर्णांक oldlen = globalsignmaplen;
		अक्षर **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * आकार(अक्षर *));
		जर (globalsignmap == रिक्त)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    परत;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * आकार(अक्षर *));
	    }
	}

	globalsignmap[i] = (अक्षर *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    साठी (j = 0; j < buf->signmapused; j++)
	जर (buf->signmap[j] == i + 1)
	    परत;

    // add to local map
    जर (buf->signmapused == buf->signmaplen)
    {
	जर (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(पूर्णांक, buf->signmaplen);
	}
	अन्यथा    // grow it
	{
	    पूर्णांक incr;
	    पूर्णांक oldlen = buf->signmaplen;
	    पूर्णांक *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * आकार(पूर्णांक));
	    जर (buf->signmap == रिक्त)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		परत;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * आकार(पूर्णांक));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    स्थिर पूर्णांक
mapsigntype(nbbuf_T *buf, पूर्णांक localsigntype)
{
    जर (--localsigntype >= 0 && localsigntype < buf->signmapused)
	परत buf->signmap[localsigntype];

    परत 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    स्थिर दीर्घ
get_buf_size(buf_T *bufp)
{
    linenr_T	lnum;
    दीर्घ	char_मोजणी = 0;
    पूर्णांक		eol_size;
    दीर्घ	last_check = 100000L;

    जर (bufp->b_ml.ml_flags & ML_EMPTY)
	परत 0;

    जर (get_fileformat(bufp) == EOL_DOS)
	eol_size = 2;
    अन्यथा
	eol_size = 1;
    साठी (lnum = 1; lnum <= bufp->b_ml.ml_ओळ_मोजणी; ++lnum)
    {
	char_मोजणी += ml_get_buf_len(bufp, lnum) + eol_size;
	// Check for a CTRL-C every 100000 characters
	जर (char_मोजणी > last_check)
	{
	    ui_breakcheck();
	    जर (got_int)
		परत char_मोजणी;
	    last_check = char_मोजणी + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    जर (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_मोजणी -= eol_size;

    परत char_मोजणी;
}

/*
 * Convert character offset to lnum,col
 */
    स्थिर pos_T *
off2pos(buf_T *buf, दीर्घ offset)
{
    linenr_T	 lnum;
    स्थिर pos_T pos;

    pos.lnum = 0;
    pos.col = 0;
    pos.coladd = 0;

    जर (!(buf->b_ml.ml_flags & ML_EMPTY))
    {
	जर ((lnum = ml_find_ओळ_or_offset(buf, (linenr_T)0, &offset)) < 0)
	    परत रिक्त;
	pos.lnum = lnum;
	pos.col = offset;
    }

    परत &pos;
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
    दीर्घ		off;

    off = म_से_दीर्घ((अक्षर *)*argp, (अक्षर **)argp, 10);
    जर (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.col = म_से_दीर्घ((अक्षर *)*argp, (अक्षर **)argp, 10);
	mypos.coladd = 0;
	परत &mypos;
    }
    परत off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    स्थिर दीर्घ
pos2off(buf_T *buf, pos_T *pos)
{
    दीर्घ	 offset = 0;

    जर (buf->b_ml.ml_flags & ML_EMPTY)
	परत 0;

    जर ((offset = ml_find_ओळ_or_offset(buf, pos->lnum, 0)) < 0)
	परत 0;
    offset += pos->col;

    परत offset;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    स्थिर शून्य
print_read_संदेश(nbbuf_T *buf)
{
    पूर्णांक	    lnum = buf->bufp->b_ml.ml_ओळ_मोजणी;
    off_T   nchars = buf->bufp->b_orig_size;
    char_u  c;

    संदेश_add_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    जर (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    जर (!buf->bufp->b_start_eol)
    {
	STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    संदेश_add_ओळी(c, (दीर्घ)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_संदेश);
    संदेश_scrolled_ign = TRUE;
    संदेश_trunc_attr((अक्षर *)IObuff, FALSE, 0);
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

    जर (nchars >= 0)
    {
	// put fname in IObuff with quotes
	संदेश_add_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	संदेश_add_ओळी(c, buf->bufp->b_ml.ml_ओळ_मोजणी,
						buf->bufp->b_orig_size);

	VIM_CLEAR(keep_संदेश);
	संदेश_scrolled_ign = TRUE;
	p = (char_u *)संदेश_trunc_attr((अक्षर *)IObuff, FALSE, 0);
	जर ((संदेश_scrolled && !need_wait_return) || !buf->initDone)
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
	अक्षर msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_add_bang_to_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#endif // defined(FEAT_NETBEANS_INTG)
