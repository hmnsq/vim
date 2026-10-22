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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_NETBEANS_INTG)

#ifndef MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#endif

#અન્તર્ભાવ "version.h"

#define GUARDED		10000 // typenr for "guarded" annotation
#define GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#define MAX_COLOR_LENGTH 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#define VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#define VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
સ્થિર અક્ષર *ExtEdProtocolVersion = "2.5";

સ્થિર long pos2off(buf_T *, pos_T *);
સ્થિર pos_T *off2pos(buf_T *, long);
સ્થિર pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
સ્થિર long get_buf_size(buf_T *);
સ્થિર પૂર્ણાંક netbeans_keystring(char_u *keystr);
સ્થિર શૂન્ય special_ચાવીઓ(char_u *args);

સ્થિર પૂર્ણાંક getConnInfo(અક્ષર *file, અક્ષર **host, અક્ષર **port, અક્ષર **password);

સ્થિર શૂન્ય nb_init_graphics(શૂન્ય);
સ્થિર શૂન્ય coloncmd(અક્ષર *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
સ્થિર શૂન્ય nb_set_curbuf(buf_T *buf);
સ્થિર શૂન્ય nb_parse_cmd(char_u *);
સ્થિર પૂર્ણાંક  nb_do_cmd(પૂર્ણાંક, char_u *, પૂર્ણાંક, પૂર્ણાંક, char_u *);
સ્થિર શૂન્ય nb_send(અક્ષર *buf, અક્ષર *fun);
સ્થિર શૂન્ય nb_free(શૂન્ય);

#define NETBEANS_OPEN (channel_can_write_to(nb_channel))
સ્થિર channel_T *nb_channel = ખાલી;

સ્થિર પૂર્ણાંક r_cmdno;			// current command number for reply
સ્થિર પૂર્ણાંક dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ifdef NBDEBUG
# include "nbdebug.c"
#endif

સ્થિર પૂર્ણાંક needupdate = 0;
સ્થિર પૂર્ણાંક inAtomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    સ્થિર પૂર્ણાંક
nb_is_safe_string(char_u *str, char_u *allowed)
{
    જો (str == ખાલી)
	પાછા FALSE;
    માટે (char_u *p = str; *p != NUL; p++)
	જો (vim_strchr(allowed, *p) == ખાલી)
	    પાછા FALSE;
    પાછા TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    સ્થિર શૂન્ય
nb_channel_closed(શૂન્ય)
{
    nb_channel = ખાલી;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    સ્થિર શૂન્ય
netbeans_close(શૂન્ય)
{
    જો (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	જો (nb_channel != ખાલી)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = ખાલી;
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

    સ્થિર પૂર્ણાંક
netbeans_connect(અક્ષર *params, પૂર્ણાંક doabort)
{
    પૂર્ણાંક		port;
    અક્ષર	buf[32];
    અક્ષર	*hostname = ખાલી;
    અક્ષર	*address = ખાલી;
    અક્ષર	*password = ખાલી;
    અક્ષર	*fname;
    અક્ષર	*arg = ખાલી;

    જો (*params == '=')
    {
	// "=fname": Read info from specified file.
	જો (getConnInfo(params + 1, &hostname, &address, &password) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર
    {
	જો (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	જો (arg == ખાલી && (fname = getenv("__NETBEANS_CONINFO")) != ખાલી)
	{
	    // "": get info from file specified in environment
	    જો (getConnInfo(fname, &hostname, &address, &password) == FAIL)
		પાછા FAIL;
	}
	નહીંતર
	{
	    જો (arg != ખાલી)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		address = strchr(hostname, ':');
		જો (address != ખાલી)
		{
		    *address++ = '\0';
		    password = strchr(address, ':');
		    જો (password != ખાલી)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    જો (hostname == ખાલી || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    જો (address == ખાલી)
		address = getenv("__NETBEANS_SOCKET");
	    જો (password == ખાલી)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    જો (hostname != ખાલી)
		hostname = (અક્ષર *)vim_strsave((char_u *)hostname);
	    જો (address != ખાલી)
		address = (અક્ષર *)vim_strsave((char_u *)address);
	    જો (password != ખાલી)
		password = (અક્ષર *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    જો (hostname == ખાલી || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (અક્ષર *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    જો (address == ખાલી || *address == '\0')
    {
	vim_free(address);
	address = (અક્ષર *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    જો (password == ખાલી || *password == '\0')
    {
	vim_free(password);
	password = (અક્ષર *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    જો (hostname != ખાલી && address != ખાલી && password != ખાલી)
    {
	port = atoi(address);
	nb_channel = channel_open(hostname, port, 3000, nb_channel_closed);
	જો (nb_channel != ખાલી)
	{
	    // success
#ifdef FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#endif

	    // success, login
	    vim_snprintf(buf, કદ(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", ExtEdProtocolVersion);
	    nb_send(buf, "externaleditor_version");
	}
    }

    જો (nb_channel == ખાલી && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(address);
    vim_free(password);
    પાછા NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    સ્થિર પૂર્ણાંક
getConnInfo(અક્ષર *file, અક્ષર **host, અક્ષર **port, અક્ષર **auth)
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
    જો (mch_stat(file, &st) == 0 && (st.st_mode & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_mode_for_netbeans_connection_info_file_str),
									file);
	પાછા FAIL;
    }
#endif

    fp = mch_fopen(file, "r");
    જો (fp == ખાલી)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	પાછા FAIL;
    }

    // Read the file. There should be one of each parameter
    જ્યારે ((lp = (char_u *)fgets((અક્ષર *)buf, BUFSIZ, fp)) != ખાલી)
    {
	જો ((nlp = vim_strchr(lp, '\n')) != ખાલી)
	    *nlp = 0;	    // strip off the trailing newline

	જો (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (અક્ષર *)vim_strsave(&buf[5]);
	}
	નહીંતર જો (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*port);
	    *port = (અક્ષર *)vim_strsave(&buf[5]);
	}
	નહીંતર જો (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (અક્ષર *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    પાછા OK;
}


સંરચના keyqueue
{
    char_u	    *keystr;
    સંરચના keyqueue *next;
    સંરચના keyqueue *prev;
};

પ્રકારવ્યાખ્યા સંરચના keyqueue ચાવી_Q_T;

સ્થિર ચાવી_Q_T ચાવી_Head; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    સ્થિર શૂન્ય
postpone_keycommand(char_u *keystr)
{
    ચાવી_Q_T *ગાંઠ;

    ગાંઠ = ALLOC_ONE(ચાવી_Q_T);
    જો (ગાંઠ == ખાલી)
	પાછા;  // out of memory, drop the key

    જો (ચાવી_Head.next == ખાલી) // initialize circular queue
    {
	ચાવી_Head.next = &ચાવી_Head;
	ચાવી_Head.prev = &ચાવી_Head;
    }

    // insert node at tail of queue
    ગાંઠ->next = &ચાવી_Head;
    ગાંઠ->prev = ચાવી_Head.prev;
    ચાવી_Head.prev->next = ગાંઠ;
    ચાવી_Head.prev = ગાંઠ;

    ગાંઠ->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    સ્થિર શૂન્ય
handle_ચાવી_queue(શૂન્ય)
{
    પૂર્ણાંક postponed = FALSE;

    જ્યારે (!postponed && ચાવી_Head.next && ચાવી_Head.next != &ચાવી_Head)
    {
	// first, unlink the node
	ચાવી_Q_T *ગાંઠ = ચાવી_Head.next;
	ચાવી_Head.next = ગાંઠ->next;
	ગાંઠ->next->prev = ગાંઠ->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	જો (ગાંઠ->keystr != ખાલી)
	    postponed = !netbeans_keystring(ગાંઠ->keystr);
	vim_free(ગાંઠ->keystr);

	// Finally, dispose of the node
	vim_free(ગાંઠ);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    શૂન્ય
netbeans_parse_messages(શૂન્ય)
{
    readq_T	*ગાંઠ;
    char_u	*buffer;
    char_u	*p;
    પૂર્ણાંક		own_ગાંઠ;

    જ્યારે (nb_channel != ખાલી)
    {
	ગાંઠ = channel_peek(nb_channel, PART_SOCK);
	જો (ગાંઠ == ખાલી)
	    તોડો;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_first_nl(ગાંઠ);
	જો (p == ખાલી)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    જો (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		પાછા;
	    ચાલુ;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	જો (*p == NUL)
	{
	    own_ગાંઠ = TRUE;
	    buffer = channel_get(nb_channel, PART_SOCK, ખાલી);
	    // "node" is now invalid!
	}
	નહીંતર
	{
	    own_ગાંઠ = FALSE;
	    buffer = ગાંઠ->rq_buffer;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(buffer);

	જો (own_ગાંઠ)
	    // buffer finished, dispose of it
	    vim_free(buffer);
	નહીંતર જો (nb_channel != ખાલી)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (પૂર્ણાંક)(p - buffer));
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
    સ્થિર શૂન્ય
nb_parse_cmd(char_u *cmd)
{
    અક્ષર	*verb;
    અક્ષર	*q;
    પૂર્ણાંક		bufno;
    પૂર્ણાંક		isfunc = -1;

    જો (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    જો (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_column = false;

	// The IDE is breaking the connection.
	netbeans_close();
	પાછા;
    }

    bufno = strtol((અક્ષર *)cmd, &verb, 10);

    જો (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	પાછા;
    }
    ++verb; // skip colon

    માટે (q = verb; *q; q++)
    {
	જો (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    તોડો;
	}
	નહીંતર જો (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    તોડો;
	}
    }

    જો (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_in_str), cmd);
	પાછા;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (અક્ષર *)skipwhite((char_u *)q);

    જો (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
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

સંરચના nbbuf_struct
{
    buf_T		*bufp;
    unsigned પૂર્ણાંક	 fireChanges:1;
    unsigned પૂર્ણાંક	 initDone:1;
    unsigned પૂર્ણાંક	 ઉમેરો_Done:1;
    unsigned પૂર્ણાંક	 modified:1;
    પૂર્ણાંક			 nbbuf_સંખ્યા;
    અક્ષર		*displayname;
    પૂર્ણાંક			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

પ્રકારવ્યાખ્યા સંરચના nbbuf_struct nbbuf_T;

સ્થિર nbbuf_T *buf_list = ખાલી;
સ્થિર પૂર્ણાંક buf_list_size = 0;	// size of buf_list
સ્થિર પૂર્ણાંક buf_list_used = 0;	// nr of entries in buf_list actually in use

સ્થિર અક્ષર **globalsignmap = ખાલી;
સ્થિર પૂર્ણાંક globalsignmaplen = 0;
સ્થિર પૂર્ણાંક globalsignmapused = 0;

સ્થિર પૂર્ણાંક  mapsigntype(nbbuf_T *, પૂર્ણાંક localsigntype);
સ્થિર શૂન્ય addsigntype(nbbuf_T *, પૂર્ણાંક localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
સ્થિર શૂન્ય print_read_સંદેશ(nbbuf_T *buf);
સ્થિર શૂન્ય print_save_સંદેશ(nbbuf_T *buf, off_T nchars);

સ્થિર પૂર્ણાંક curPCtype = -1;

/*
 * Free netbeans resources.
 */
    સ્થિર શૂન્ય
nb_free(શૂન્ય)
{
    ચાવી_Q_T *ચાવી_ગાંઠ = ચાવી_Head.next;
    nbbuf_T buf;
    પૂર્ણાંક i;

    // free the netbeans buffer list
    માટે (i = 0; i < buf_list_used; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	જો (buf.bufp != ખાલી && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_size = 0;
    buf_list_used = 0;

    // free the queued key commands
    જ્યારે (ચાવી_ગાંઠ != ખાલી && ચાવી_ગાંઠ != &ચાવી_Head)
    {
	ચાવી_Q_T *next = ચાવી_ગાંઠ->next;
	vim_free(ચાવી_ગાંઠ->keystr);
	vim_free(ચાવી_ગાંઠ);
	જો (next == &ચાવી_Head)
	{
	    ચાવી_Head.next = &ચાવી_Head;
	    ચાવી_Head.prev = &ચાવી_Head;
	    તોડો;
	}
	ચાવી_ગાંઠ = next;
    }

    // free the queued netbeans commands
    જો (nb_channel != ખાલી)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    સ્થિર પૂર્ણાંક
nb_getbufno(buf_T *bufp)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < buf_list_used; i++)
	જો (buf_list[i].bufp == bufp)
	    પાછા i;
    પાછા -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    પૂર્ણાંક
isNetbeansBuffer(buf_T *bufp)
{
    પાછા NETBEANS_OPEN && bufp->b_netbeans_file;
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
    પૂર્ણાંક
isNetbeansModified(buf_T *bufp)
{
    જો (isNetbeansBuffer(bufp))
    {
	પૂર્ણાંક bufno = nb_getbufno(bufp);

	જો (bufno > 0)
	    પાછા buf_list[bufno].modified;
	નહીંતર
	    પાછા FALSE;
    }
    નહીંતર
	પાછા FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    સ્થિર nbbuf_T *
nb_get_buf(પૂર્ણાંક bufno)
{
    // find or create a buffer with the given number
    પૂર્ણાંક incr;

    જો (bufno <= 0)
	પાછા ખાલી;

    જો (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * કદ(nbbuf_T));
	buf_list_size = 100;
    }
    જો (bufno >= buf_list_used) // new
    {
	જો (bufno >= buf_list_size) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_size + 90;
	    buf_list_size += incr;
	    bufsize = buf_list_size * કદ(nbbuf_T);
	    જો (bufsize == 0 || bufsize / કદ(nbbuf_T)
						      != (size_t)buf_list_size)
	    {
		// list size overflow, bail out
		પાછા ખાલી;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    જો (buf_list == ખાલી)
	    {
		vim_free(t_buf_list);
		buf_list_size = 0;
		પાછા ખાલી;
	    }
	    vim_memset(buf_list + buf_list_size - incr, 0,
						      incr * કદ(nbbuf_T));
	}

	જ્યારે (buf_list_used <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_used].fireChanges = 1;
	    ++buf_list_used;
	}
    }

    પાછા buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    સ્થિર પૂર્ણાંક
ગણતરી_changed_buffers(શૂન્ય)
{
    buf_T	*bufp;
    પૂર્ણાંક		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	જો (bufp->b_changed)
	    ++n;
    પાછા n;
}

/*
 * End the netbeans session.
 */
    શૂન્ય
netbeans_end(શૂન્ય)
{
    પૂર્ણાંક i;
    સ્થિર અક્ષર buf[128];

    જો (!NETBEANS_OPEN)
	પાછા;

    માટે (i = 0; i < buf_list_used; i++)
    {
	જો (!buf_list[i].bufp)
	    ચાલુ;
	જો (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, ખાલી);
	buf_list[i].bufp = ખાલી;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    સ્થિર શૂન્ય
nb_send(અક્ષર *buf, અક્ષર *fun)
{
    જો (nb_channel != ખાલી)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (પૂર્ણાંક)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    સ્થિર શૂન્ય
nb_reply_nil(પૂર્ણાંક cmdno)
{
    અક્ષર reply[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    જો (!NETBEANS_OPEN)
	પાછા;

    sprintf(reply, "%d\n", cmdno);
    nb_send(reply, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    સ્થિર શૂન્ય
nb_reply_text(પૂર્ણાંક cmdno, char_u *result)
{
    char_u *reply;

    nbdebug(("REP %d: %s\n", cmdno, (અક્ષર *)result));

    reply = alloc(STRLEN(result) + 32);
    sprintf((અક્ષર *)reply, "%d %s\n", cmdno, (અક્ષર *)result);
    nb_send((અક્ષર *)reply, "nb_reply_text");

    vim_free(reply);
}


/*
 * Send a response with a number result code.
 */
    સ્થિર શૂન્ય
nb_reply_nr(પૂર્ણાંક cmdno, long result)
{
    અક્ષર reply[32];

    nbdebug(("REP %d: %ld\n", cmdno, result));

    sprintf(reply, "%d %ld\n", cmdno, result);
    nb_send(reply, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    સ્થિર char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    જો (buf == ખાલી)
	પાછા ખાલી;
    માટે (; *p; p++)
    {
	પસંદગી (*p)
	{
	    સ્થિતિ '\"':
	    સ્થિતિ '\\':
		*q++ = '\\'; *q++ = *p; તોડો;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    સ્થિતિ '\n':
		*q++ = '\\'; *q++ = 'n'; તોડો;
	    સ્થિતિ '\r':
		*q++ = '\\'; *q++ = 'r'; તોડો;
	    મૂળભૂત:
		*q++ = *p;
		તોડો;
	}
    }
    *q = '\0';

    પાછા buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    સ્થિર અક્ષર *
nb_unquote(char_u *p, char_u **endp)
{
    અક્ષર *result = 0;
    અક્ષર *q;
    પૂર્ણાંક done = 0;

    // result is never longer than input
    result = alloc_clear(STRLEN(p) + 1);
    જો (result == ખાલી)
	પાછા ખાલી;

    જો (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	result[0] = NUL;
	પાછા result;
    }

    માટે (q = result; !done && *p != NUL;)
    {
	પસંદગી (*p)
	{
	    સ્થિતિ '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		done = 1;
		તોડો;

	    સ્થિતિ '\\':
		++p;
		પસંદગી (*p)
		{
		    સ્થિતિ '\\':	*q++ = '\\';	તોડો;
		    સ્થિતિ 'n':	*q++ = '\n';	તોડો;
		    સ્થિતિ 't':	*q++ = '\t';	તોડો;
		    સ્થિતિ 'r':	*q++ = '\r';	તોડો;
		    સ્થિતિ '"':	*q++ = '"';	તોડો;
		    સ્થિતિ NUL:	--p;		તોડો;
		    // default: skip over illegal chars
		}
		++p;
		તોડો;

	    મૂળભૂત:
		*q++ = *p++;
	}
    }

    જો (endp != ખાલી)
	*endp = p;

    પાછા result;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    સ્થિર શૂન્ય
nb_partialremove(linenr_T lnum, colnr_T first, colnr_T last)
{
    char_u *oldtext, *newtext;
    પૂર્ણાંક oldlen;
    પૂર્ણાંક lastbyte = last;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    જો (first >= (colnr_T)oldlen || oldlen == 0)  // just in case
	પાછા;
    જો (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - first));
    જો (newtext == ખાલી)
	પાછા;

    mch_memmove(newtext, oldtext, first);
    STRMOVE(newtext + first, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    સ્થિર શૂન્ય
nb_joinlines(linenr_T first, linenr_T other)
{
    પૂર્ણાંક len_first, len_other;
    char_u *p;

    len_first = ml_get_len(first);
    len_other = ml_get_len(other);
    p = alloc(len_first + len_other + 1);
    જો (p == ખાલી)
	પાછા;

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
    સ્થિર પૂર્ણાંક
nb_do_cmd(
    પૂર્ણાંક		bufno,
    char_u	*cmd,
    પૂર્ણાંક		func,
    પૂર્ણાંક		cmdno,
    char_u	*args)	    // points to space before arguments or NUL
{
    પૂર્ણાંક		do_update = 0;
    long	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    સ્થિર પૂર્ણાંક	skip = 0;
    પૂર્ણાંક		retval = OK;
    અક્ષર	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (અક્ષર *)args));

    જો (func)
    {
// =====================================================================
	જો (streq((અક્ષર *)cmd, "getModified"))
	{
	    જો (buf == ખાલી || buf->bufp == ખાલી)
		// Return the number of buffers that are modified.
		nb_reply_nr(cmdno, (long)ગણતરી_changed_buffers());
	    નહીંતર
		// Return whether the buffer is modified.
		nb_reply_nr(cmdno, (long)(buf->bufp->b_changed
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_reply_nr(cmdno, (long)ગણતરી_changed_buffers());
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "getCursor"))
	{
	    char_u text[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((અક્ષર *)text, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (long)curwin->w_cursor.lnum,
		    (પૂર્ણાંક)curwin->w_cursor.col,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_reply_text(cmdno, text);
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "getAnno"))
	{
	    long linenum = 0;
#ifdef FEAT_SIGNS
	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_getanno));
		retval = FAIL;
	    }
	    નહીંતર
	    {
		પૂર્ણાંક ser_અંક;

		cp = (અક્ષર *)args;
		ser_અંક = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (long)buf_findsign(buf->bufp, ser_અંક, ખાલી);
	    }
#endif
	    nb_reply_nr(cmdno, linenum);
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "getLength"))
	{
	    long len = 0;

	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_buffer_identifier_in_getlength));
		retval = FAIL;
	    }
	    નહીંતર
	    {
		len = get_buf_size(buf->bufp);
	    }
	    nb_reply_nr(cmdno, len);
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "getText"))
	{
	    long	len;
	    linenr_T	nlines;
	    char_u	*text = ખાલી;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*લીટી;

	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_buffer_identifier_in_gettext));
		retval = FAIL;
	    }
	    નહીંતર
	    {
		len = get_buf_size(buf->bufp);
		nlines = buf->bufp->b_ml.ml_લીટી_ગણતરી;
		text = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		જો (text == ખાલી)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		નહીંતર
		{
		    p = text;
		    *p++ = '\"';
		    માટે (; lno <= nlines ; lno++)
		    {
			લીટી = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			જો (લીટી != ખાલી)
			{
			    STRCPY(p, લીટી);
			    p += STRLEN(લીટી);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(લીટી);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    જો (text == ખાલી)
		nb_reply_text(cmdno, (char_u *)"");
	    નહીંતર
	    {
		nb_reply_text(cmdno, text);
		vim_free(text);
	    }
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "remove"))
	{
	    long ગણતરી;
	    pos_T first, last;
	    pos_T *pos;
	    pos_T *next;
	    linenr_T del_from_lnum, del_to_lnum;  // lines to be deleted as a whole
	    પૂર્ણાંક oldFire = netbeansFireChanges;
	    પૂર્ણાંક oldSuppress = netbeans_Suppress_No_લીટીઓ;
	    પૂર્ણાંક wasChanged;

	    જો (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (અક્ષર *) cmd));
		nb_reply_nil(cmdno);
		પાછા OK;
	    }

	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_buffer_identifier_in_remove));
		retval = FAIL;
	    }
	    નહીંતર
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_લીટીઓ = TRUE;

		nb_set_curbuf(buf->bufp);
		wasChanged = buf->bufp->b_changed;
		cp = (અક્ષર *)args;
		off = strtol(cp, &cp, 10);
		ગણતરી = strtol(cp, &cp, 10);
		args = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		જો (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_લીટીઓ = oldSuppress;
		    પાછા FAIL;
		}
		first = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      first.lnum, first.col));
		pos = off2pos(buf->bufp, off+ગણતરી-1);
		જો (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_લીટીઓ = oldSuppress;
		    પાછા FAIL;
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
		next = off2pos(buf->bufp, off + ગણતરી);

		// Remove part of the first line.
		જો (first.col != 0
				|| (next != ખાલી && first.lnum == next->lnum))
		{
		    જો (first.lnum != last.lnum
			    || (next != ખાલી && first.lnum != next->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(first.lnum, first.col,
							     (colnr_T)MAXCOL);
			જો (first.lnum == last.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(first.lnum, next->lnum);
			    del_to_lnum = next->lnum;
			}
		    }
		    નહીંતર
		    {
			// remove within one line
			nb_partialremove(first.lnum, first.col, last.col);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		જો (first.lnum != last.lnum && next != ખાલી
			&& next->col != 0 && last.lnum == next->lnum)
		{
		    nb_partialremove(last.lnum, 0, last.col);
		    જો (del_from_lnum > first.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(first.lnum, last.lnum);
		    }
		    નહીંતર
			// First line is deleted as a whole, keep the last
			// line.
			--del_to_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		જો (first.lnum != last.lnum && del_from_lnum > first.lnum
			&& next != ખાલી && last.lnum != next->lnum)
		{
		    nb_joinlines(first.lnum, next->lnum);
		    del_to_lnum = next->lnum;
		}

		// Delete whole lines if there are any.
		જો (del_to_lnum >= del_from_lnum)
		{
		    પૂર્ણાંક i;

		    // delete signs from the lines being deleted
		    માટે (i = del_from_lnum; i <= del_to_lnum; i++)
		    {
			પૂર્ણાંક id = buf_findsign_id(buf->bufp, (linenr_T)i, ખાલી);
			જો (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, ખાલી);
			}
			નહીંતર
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_to_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.col = 0;
		    del_લીટીઓ(del_to_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = first;
		check_cursor_lnum();
		buf->bufp->b_changed = wasChanged; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_લીટીઓ = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_reply_nil(cmdno);
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "insert"))
	{
	    char_u	*to_free;

	    જો (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (અક્ષર *) cmd));
		nb_reply_nil(cmdno);
		પાછા OK;
	    }

	    // get offset
	    cp = (અક્ષર *)args;
	    off = strtol(cp, &cp, 10);
	    args = (char_u *)cp;

	    // get text to be inserted
	    args = skipwhite(args);
	    args = to_free = (char_u *)nb_unquote(args, ખાલી);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_buffer_identifier_in_ઉમેરો));
		retval = FAIL;
	    }
	    નહીંતર જો (args != ખાલી)
	    {
		પૂર્ણાંક	ff_detected = EOL_UNKNOWN;
		પૂર્ણાંક	buf_was_empty = (buf->bufp->b_ml.ml_flags & ML_EMPTY);
		size_t	len = 0;
		પૂર્ણાંક	added = 0;
		પૂર્ણાંક	oldFire = netbeansFireChanges;
		પૂર્ણાંક	old_b_changed;
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
		જો (pos != ખાલી)
		{
		    જો (pos->lnum <= 0)
			lnum_start = 1;
		    નહીંતર
			lnum_start = pos->lnum;
		}
		નહીંતર
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    જો (buf_was_empty)
			lnum_start = 1;	    // above empty line
		    નહીંતર
			lnum_start = curbuf->b_ml.ml_લીટી_ગણતરી + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_start;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		જ્યારે (*args != NUL)
		{
		    nlp = vim_strchr(args, '\n');
		    જો (nlp == ખાલી)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(args);
		    }
		    નહીંતર
		    {
			len = nlp - args;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			જો (nlp > args && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			નહીંતર
			    ff_detected = EOL_UNIX;
		    }
		    args[len] = NUL;

		    જો (lnum == lnum_start
			    && ((pos != ખાલી && pos->col > 0)
				|| (lnum == 1 && buf_was_empty)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			પૂર્ણાંક	col = pos == ખાલી ? 0 : pos->col;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			જો (newline != ખાલી)
			{
			    mch_memmove(newline, oldline, (size_t)col);
			    newline[col] = NUL;
			    STRCAT(newline, args);
			    STRCAT(newline, oldline + col);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    નહીંતર
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), args,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    જો (nlp == ખાલી)
			તોડો;
		    ++lnum;
		    args = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_લીટીઓ_mark(lnum_start - 1, (long)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		જો (buf_was_empty)
		{
		    જો (ff_detected == EOL_UNKNOWN)
#જો defined(MSWIN)
			ff_detected = EOL_DOS;
#નહીંતર
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
	નહીંતર
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_reply_nil(cmdno);
	    retval = FAIL;
	}
    }
    નહીંતર // Not a function; no reply required.
    {
// =====================================================================
	જો (streq((અક્ષર *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    જો (buf == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_buffer_identifier_in_create));
		પાછા FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, ખાલી, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->ઉમેરો_Done = FALSE;
#જો defined(FEAT_MENU) && defined(FEAT_GUI)
	    જો (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "insertDone"))
	{
	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    નહીંતર
	    {
		buf->bufp->b_start_eol = *args == 'T';
		buf->ઉમેરો_Done = TRUE;
		args += 2;
		buf->bufp->b_p_ro = *args == 'T';
		print_read_સંદેશ(buf);
	    }
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "saveDone"))
	{
	    long savedChars = atol((અક્ષર *)args);

	    જો (buf == ખાલી || buf->bufp == ખાલી)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    નહીંતર
		print_save_સંદેશ(buf, savedChars);
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "startDocumentListen"))
	{
	    જો (buf == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_startdocumentlisten));
		પાછા FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "stopDocumentListen"))
	{
	    જો (buf == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_stopdocumentlisten));
		પાછા FAIL;
	    }
	    buf->fireChanges = 0;
	    જો (buf->bufp != ખાલી && buf->bufp->b_was_netbeans_file)
	    {
		જો (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_lost_for_buffer_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_lost_for_buffer_nr),
							   buf->bufp->b_fnum);
		}
		નહીંતર
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
	નહીંતર જો (streq((અક્ષર *)cmd, "setTitle"))
	{
	    જો (buf == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_buffer_identifier_in_settitle));
		પાછા FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ખાલી);
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "initDone"))
	{
	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_buffer_identifier_in_initdone));
		પાછા FAIL;
	    }
	    do_update = 1;
	    buf->initDone = TRUE;
	    nb_set_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_ચાવી_queue();
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "setBufferNumber")
		|| streq((અક્ષર *)cmd, "putBufferNumber"))
	{
	    char_u	*path;
	    buf_T	*bufp;

	    જો (buf == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_buffer_identifier_in_setbuffernumber));
		પાછા FAIL;
	    }
	    path = (char_u *)nb_unquote(args, ખાલી);
	    જો (path == ખાલી)
		પાછા FAIL;
	    bufp = buflist_findname(path);
	    vim_free(path);
	    જો (bufp == ખાલી)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", args));
		semsg(_(e_file_str_not_found_in_setbuffernumber), args);
		પાછા FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_સંખ્યા = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    જો (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    નહીંતર
	    {
		buf->initDone = TRUE;

		// handle any postponed key commands
		handle_ચાવી_queue();
	    }

// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "setFullName"))
	{
	    જો (buf == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_buffer_identifier_in_setfullname));
		પાછા FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ખાલી);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#જો defined(FEAT_MENU) && defined(FEAT_GUI)
	    જો (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "editFile"))
	{
	    જો (buf == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_buffer_identifier_in_editfile));
		પાછા FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ખાલી);
	    do_ecmd(0, (char_u *)buf->displayname, ખાલી, ખાલી, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->initDone = TRUE;
	    do_update = 1;
	    maketitle();
#જો defined(FEAT_MENU) && defined(FEAT_GUI)
	    જો (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "setVisible"))
	{
	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		જો (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setvisible));
		પાછા FAIL;
	    }
	    જો (streq((અક્ષર *)args, "T") && buf->bufp != curbuf)
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
		જો (gui.in_use)
		    gui_mch_set_foreground();
#endif
	    }
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "raise"))
	{
#ifdef FEAT_GUI
	    // Bring gvim to the foreground.
	    જો (gui.in_use)
		gui_mch_set_foreground();
#endif
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "setModified"))
	{
	    પૂર્ણાંક prev_b_changed;

	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		જો (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setmodified));
		પાછા FAIL;
	    }
	    prev_b_changed = buf->bufp->b_changed;
	    જો (streq((અક્ષર *)args, "T"))
		buf->bufp->b_changed = TRUE;
	    નહીંતર
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		જો (buf->bufp->b_ffname != ખાલી
			&& mch_stat((અક્ષર *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_time(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_changed = FALSE;
	    }
	    buf->modified = buf->bufp->b_changed;
	    જો (prev_b_changed != buf->bufp->b_changed)
	    {
		check_status(buf->bufp);
		redraw_tabline = TRUE;
#જો defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "setModtime"))
	{
	    જો (buf == ખાલી || buf->bufp == ખાલી)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    નહીંતર
	    {
		buf->bufp->b_mtime = atoi((અક્ષર *)args);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "setReadOnly"))
	{
	    જો (buf == ખાલી || buf->bufp == ખાલી)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    નહીંતર જો (streq((અક્ષર *)args, "T"))
		buf->bufp->b_p_ro = TRUE;
	    નહીંતર
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "showBalloon"))
	{
#જો defined(FEAT_BEVAL_GUI)
	    સ્થિર અક્ષર	*text = ખાલી;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    જો (balloonEval != ખાલી)
	    {
		vim_free(text);
		text = nb_unquote(args, ખાલી);
		જો (text != ખાલી)
		    gui_mch_post_balloon(balloonEval, (char_u *)text);
	    }
#endif
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "setDot"))
	{
	    pos_T *pos;
#ifdef NBDEBUG
	    char_u *s;
#endif

	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_buffer_identifier_in_setdot));
		પાછા FAIL;
	    }

	    nb_set_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    જો (VIsual_active)
		end_visual_mode();
#ifdef NBDEBUG
	    s = args;
#endif
	    pos = get_off_or_lnum(buf->bufp, &args);
	    જો (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#ifdef FEAT_FOLDING
		foldOpenCursor();
#endif
	    }
	    નહીંતર
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
	    જો (State == MODE_HITRETURN || State == MODE_ASKMORE)
	    {
#ifdef FEAT_GUI_GTK
		જો (gui.in_use && gtk_main_level() > 0)
		    gtk_main_quit();
#endif
	    }
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "close"))
	{
#ifdef NBDEBUG
	    અક્ષર *name = "<NONE>";
#endif

	    જો (buf == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_buffer_identifier_in_close));
		પાછા FAIL;
	    }

#ifdef NBDEBUG
	    જો (buf->displayname != ખાલી)
		name = buf->displayname;
#endif
	    જો (buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		જો (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    જો (buf->bufp != ખાલી)
		do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = ખાલી;
	    buf->initDone = FALSE;
	    do_update = 1;
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "defineAnnoType"))
	{
#ifdef FEAT_SIGNS
	    પૂર્ણાંક type_અંક;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    પૂર્ણાંક parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    જો (buf == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_buffer_identifier_in_defineannotype));
		પાછા FAIL;
	    }

	    cp = (અક્ષર *)args;
	    type_અંક = strtol(cp, &cp, 10);
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
	    જો (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(args);
	    bg = vim_strsave(p);
	    જો (STRLEN(fg) > MAX_COLOR_LENGTH || STRLEN(bg) > MAX_COLOR_LENGTH)
	    {
		emsg(_(e_highlighting_color_name_too_long_in_defineAnnoType));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    નહીંતર જો (!nb_is_safe_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_safe_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_safe_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_in_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    નહીંતર જો (typeName != ખાલી && tooltip != ખાલી && glyphFile != ખાલી)
		addsigntype(buf, type_અંક, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    જો (parse_error)
		પાછા FAIL;

#endif
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "addAnno"))
	{
#ifdef FEAT_SIGNS
	    પૂર્ણાંક ser_અંક;
	    પૂર્ણાંક local_Type_અંક;
	    પૂર્ણાંક type_અંક;
	    pos_T *pos;

	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_addanno));
		પાછા FAIL;
	    }

	    do_update = 1;

	    cp = (અક્ષર *)args;
	    ser_અંક = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_અંક = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    type_અંક = mapsigntype(buf, local_Type_અંક);

	    pos = get_off_or_lnum(buf->bufp, &args);

	    cp = (અક્ષર *)args;
	    vim_ignored = (પૂર્ણાંક)strtol(cp, &cp, 10);
	    args = (char_u *)cp;
# ifdef NBDEBUG
	    જો (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    જો (ser_અંક >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		પાછા FAIL;
	    }
	    જો (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_અંક, pos->lnum, type_અંક, buf->bufp->b_fnum);
		જો (type_અંક == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_અંક,
						       buf->bufp->b_fnum);
	    }
#endif
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "removeAnno"))
	{
#ifdef FEAT_SIGNS
	    પૂર્ણાંક ser_અંક;

	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		પાછા FAIL;
	    }
	    do_update = 1;
	    cp = (અક્ષર *)args;
	    ser_અંક = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_અંક, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#endif
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "moveAnnoToFront"))
	{
#ifdef FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#endif
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "guard") || streq((અક્ષર *)cmd, "unguard"))
	{
	    પૂર્ણાંક len;
	    pos_T first;
	    pos_T last;
	    pos_T *pos;
	    પૂર્ણાંક un = (cmd[0] == 'u');
	    સ્થિર પૂર્ણાંક guardId = GUARDEDOFFSET;

	    જો (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (અક્ષર *) cmd));
		પાછા OK;
	    }

	    nb_init_graphics();

	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		પાછા FAIL;
	    }
	    nb_set_curbuf(buf->bufp);
	    cp = (અક્ષર *)args;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, ખાલી, 10);
	    args = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    જો (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    નહીંતર
	    {
		first = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		જો (pos != ખાલી && pos->col == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		જો (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		નહીંતર
		{
		    long lnum;
		    last = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     first.lnum, first.col,
			     last.lnum, last.col));
#ifdef FEAT_SIGNS
		    માટે (lnum = first.lnum; lnum <= last.lnum; lnum++)
		    {
			જો (un)
			{
			    // never used
			}
			નહીંતર
			{
			    જો (buf_findsigntype_id(buf->bufp, lnum,
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
	નહીંતર જો (streq((અક્ષર *)cmd, "startAtomic"))
	{
	    inAtomic = 1;
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "endAtomic"))
	{
	    inAtomic = 0;
	    જો (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		પાછા FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    જો (bufIsChanged(buf->bufp))
	    {
		// Only write if the buffer can be written.
		જો (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != ખાલી
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    જો (!bufref_valid(&bufref))
			buf->bufp = ખાલી;
		}
	    }
	    નહીંતર
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "netbeansBuffer"))
	{
	    જો (buf == ખાલી || buf->bufp == ખાલી)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		પાછા FAIL;
	    }
	    જો (*args == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    નહીંતર
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "specialKeys"))
	{
	    special_ચાવીઓ(args);
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	નહીંતર જો (streq((અક્ષર *)cmd, "version"))
	{
	    // not used yet
	}
	નહીંતર
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    જો (inAtomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    જો (buf != ખાલી && buf->initDone && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	out_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	જો (State == MODE_HITRETURN || State == MODE_ASKMORE)
	{
#ifdef FEAT_GUI_GTK
	    જો (gui.in_use && gtk_main_level() > 0)
		gtk_main_quit();
#endif
	}
    }

    પાછા retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    સ્થિર શૂન્ય
nb_set_curbuf(buf_T *buf)
{
    જો (curbuf == buf)
	પાછા;

    જો (buf_jump_open_win(buf) != ખાલી)
	પાછા;
    જો ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != ખાલી)
	પાછા;
    set_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    સ્થિર શૂન્ય
coloncmd(અક્ષર *cmd, ...)
{
    અક્ષર buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, કદ(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, ખાલી, ખાલી, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    out_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    સ્થિર શૂન્ય
special_ચાવીઓ(char_u *args)
{
    અક્ષર *save_str = nb_unquote(args, ખાલી);
    અક્ષર *tok = strtok(save_str, " ");
    અક્ષર *sep;
#define KEYBUFLEN 64
    અક્ષર keybuf[KEYBUFLEN];
    અક્ષર cmdbuf[256];

    જ્યારે (tok != ખાલી)
    {
	પૂર્ણાંક i = 0;

	જો ((sep = strchr(tok, '-')) != ખાલી)
	{
	    *sep = NUL;
	    જ્યારે (*tok && i + 2 < KEYBUFLEN)
	    {
		પસંદગી (*tok)
		{
		    સ્થિતિ 'A':
		    સ્થિતિ 'M':
		    સ્થિતિ 'C':
		    સ્થિતિ 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			તોડો;
		}
		tok++;
	    }
	    tok++;
	}

	જો (strlen(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    પૂર્ણાંક safe = TRUE;
	    માટે (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		જો (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    safe = FALSE;
		    તોડો;
		}
	    }
	    જો (safe)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, કદ(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, MODE_NORMAL, FALSE);
	    }
	}
	tok = strtok(ખાલી, " ");
    }
    vim_free(save_str);
}

    શૂન્ય
ex_nbclose(exarg_T *eap UNUSED)
{
    netbeans_close();
}

    શૂન્ય
ex_nbkey(exarg_T *eap)
{
    (શૂન્ય)netbeans_keystring(eap->arg);
}

    શૂન્ય
ex_nbstart(
    exarg_T	*eap)
{
#ifdef FEAT_GUI
# જો !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    જો (gui.in_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	પાછા;
    }
# endif
#endif
    netbeans_open((અક્ષર *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    સ્થિર શૂન્ય
nb_init_graphics(શૂન્ય)
{
    સ્થિર પૂર્ણાંક did_init = FALSE;

    જો (did_init)
	પાછા;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    સ્થિર શૂન્ય
netbeans_keyname(પૂર્ણાંક ચાવી, અક્ષર *buf)
{
    અક્ષર *name = 0;
    અક્ષર namebuf[2];
    પૂર્ણાંક ctrl  = 0;
    પૂર્ણાંક shift = 0;
    પૂર્ણાંક alt   = 0;

    જો (mod_mask & MOD_MASK_CTRL)
	ctrl = 1;
    જો (mod_mask & MOD_MASK_SHIFT)
	shift = 1;
    જો (mod_mask & MOD_MASK_ALT)
	alt = 1;


    પસંદગી (ચાવી)
    {
	સ્થિતિ K_F1:		name = "F1";		તોડો;
	સ્થિતિ K_S_F1:	name = "F1";	shift = 1;	તોડો;
	સ્થિતિ K_F2:		name = "F2";		તોડો;
	સ્થિતિ K_S_F2:	name = "F2";	shift = 1;	તોડો;
	સ્થિતિ K_F3:		name = "F3";		તોડો;
	સ્થિતિ K_S_F3:	name = "F3";	shift = 1;	તોડો;
	સ્થિતિ K_F4:		name = "F4";		તોડો;
	સ્થિતિ K_S_F4:	name = "F4";	shift = 1;	તોડો;
	સ્થિતિ K_F5:		name = "F5";		તોડો;
	સ્થિતિ K_S_F5:	name = "F5";	shift = 1;	તોડો;
	સ્થિતિ K_F6:		name = "F6";		તોડો;
	સ્થિતિ K_S_F6:	name = "F6";	shift = 1;	તોડો;
	સ્થિતિ K_F7:		name = "F7";		તોડો;
	સ્થિતિ K_S_F7:	name = "F7";	shift = 1;	તોડો;
	સ્થિતિ K_F8:		name = "F8";		તોડો;
	સ્થિતિ K_S_F8:	name = "F8";	shift = 1;	તોડો;
	સ્થિતિ K_F9:		name = "F9";		તોડો;
	સ્થિતિ K_S_F9:	name = "F9";	shift = 1;	તોડો;
	સ્થિતિ K_F10:		name = "F10";		તોડો;
	સ્થિતિ K_S_F10:	name = "F10";	shift = 1;	તોડો;
	સ્થિતિ K_F11:		name = "F11";		તોડો;
	સ્થિતિ K_S_F11:	name = "F11";	shift = 1;	તોડો;
	સ્થિતિ K_F12:		name = "F12";		તોડો;
	સ્થિતિ K_S_F12:	name = "F12";	shift = 1;	તોડો;
	મૂળભૂત:
			જો (ચાવી >= ' ' && ચાવી <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = ચાવી;
			    namebuf[1] = NUL;
			}
			નહીંતર
			    name = "X";
			તોડો;
    }

    buf[0] = '\0';
    જો (ctrl)
	strcat(buf, "C");
    જો (shift)
	strcat(buf, "S");
    જો (alt)
	strcat(buf, "M"); // META
    જો (ctrl || shift || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#જો defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    શૂન્ય
netbeans_beval_cb(
	BalloonEval	*beval,
	પૂર્ણાંક		 state UNUSED)
{
    win_T	*wp;
    char_u	*text;
    linenr_T	lnum;
    પૂર્ણાંક		col;
    અક્ષર	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    જો (!can_use_beval() || !NETBEANS_OPEN)
	પાછા;

    જો (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) != OK)
	પાછા;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    જો (text != ખાલી && text[0] != NUL && STRLEN(text) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	જો (buf != ખાલી)
	{
	    p = nb_quote(text);
	    જો (p != ખાલી)
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
    પૂર્ણાંક
netbeans_active(શૂન્ય)
{
    પાછા NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    શૂન્ય
netbeans_open(અક્ષર *params, પૂર્ણાંક doabort)
{
    અક્ષર *cmd = "0:startupDone=0\n";

    જો (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	પાછા;
    }

    જો (netbeans_connect(params, doabort) != OK)
	પાછા;

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
    શૂન્ય
netbeans_send_disconnect(શૂન્ય)
{
    અક્ષર buf[128];

    જો (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#જો defined(FEAT_EVAL)
    પૂર્ણાંક
set_ref_in_nb_channel(પૂર્ણાંક copyID)
{
    પૂર્ણાંક abort = FALSE;
    typval_T tv;

    જો (nb_channel == ખાલી)
	પાછા FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = set_ref_in_item(&tv, copyID, ખાલી, ખાલી, ખાલી);
    પાછા abort;
}
#endif

#જો defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    શૂન્ય
netbeans_frame_moved(પૂર્ણાંક new_x, પૂર્ણાંક new_y)
{
    અક્ષર buf[128];

    જો (!NETBEANS_OPEN)
	પાછા;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (પૂર્ણાંક)Columns, (પૂર્ણાંક)Rows, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#endif

/*
 * Tell netbeans the user opened or activated a file.
 */
    શૂન્ય
netbeans_file_activated(buf_T *bufp)
{
    પૂર્ણાંક bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    અક્ષર    buffer[2*MAXPATHL];
    char_u  *q;

    જો (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	પાછા;

    q = nb_quote(bufp->b_ffname);
    જો (q == ખાલી)
	પાછા;
    જો (bp == ખાલી)
    {
	vim_free(q);
	પાછા;
    }

    vim_snprintf(buffer, કદ(buffer),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (અક્ષર *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    શૂન્ય
netbeans_file_opened(buf_T *bufp)
{
    પૂર્ણાંક bufno = nb_getbufno(bufp);
    અક્ષર    buffer[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    પૂર્ણાંક	    bnum;

    જો (!NETBEANS_OPEN)
	પાછા;

    q = nb_quote(bufp->b_ffname);
    જો (q == ખાલી)
	પાછા;
    જો (bp != ખાલી)
	bnum = bufno;
    નહીંતર
	bnum = 0;

    vim_snprintf(buffer, કદ(buffer), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (અક્ષર *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
    જો (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	last_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    શૂન્ય
netbeans_file_killed(buf_T *bufp)
{
    પૂર્ણાંક		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    અક્ષર	buffer[2*MAXPATHL];

    જો (!NETBEANS_OPEN || bufno == -1)
	પાછા;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(buffer, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_killed");

    જો (nbbuf != ખાલી)
	nbbuf->bufp = ખાલી;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    સ્થિર nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, પૂર્ણાંક *bufnop)
{
    પૂર્ણાંક		bufno;
    nbbuf_T	*nbbuf;

    જો (!NETBEANS_OPEN || !netbeansFireChanges)
	પાછા ખાલી;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    જો (bufno <= 0)
	પાછા ખાલી;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    જો (nbbuf != ખાલી && !nbbuf->fireChanges)
	પાછા ખાલી;		// changes in this buffer are not reported

    *bufnop = bufno;
    પાછા nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    શૂન્ય
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    char_u	*txt,
    પૂર્ણાંક		newlen)
{
    char_u	*buf;
    પૂર્ણાંક		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;
    char_u	*p;
    char_u	*newtxt;

    જો (!NETBEANS_OPEN)
	પાછા;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    જો (nbbuf == ખાલી)
	પાછા;

    // Don't mark as modified for initial read
    જો (nbbuf->ઉમેરો_Done)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.col = col;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    જો (p != ખાલી)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((અક્ષર *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((અક્ષર *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    શૂન્ય
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    long	len)
{
    char_u	buf[128];
    પૂર્ણાંક		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;

    જો (!NETBEANS_OPEN)
	પાછા;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    જો (nbbuf == ખાલી)
	પાછા;

    જો (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	પાછા;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.col = col;

    off = pos2off(bufp, &pos);

    sprintf((અક્ષર *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((અક્ષર *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    શૂન્ય
netbeans_unmodified(buf_T *bufp UNUSED)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    શૂન્ય
netbeans_button_release(પૂર્ણાંક button)
{
    અક્ષર	buf[128];
    પૂર્ણાંક		bufno;

    જો (!NETBEANS_OPEN)
	પાછા;

    bufno = nb_getbufno(curbuf);

    જો (bufno < 0 || curwin == ખાલી || curwin->w_buffer != curbuf)
	પાછા;

    પૂર્ણાંક col = mouse_col - curwin->w_wincol
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
    પૂર્ણાંક
netbeans_keycommand(પૂર્ણાંક ચાવી)
{
    અક્ષર	ચાવી_Name[60];

    netbeans_keyname(ચાવી, ચાવી_Name);
    પાછા netbeans_keystring((char_u *)ચાવી_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    સ્થિર પૂર્ણાંક
netbeans_keystring(char_u *ચાવી_Name)
{
    અક્ષર	buf[2*MAXPATHL];
    પૂર્ણાંક		bufno = nb_getbufno(curbuf);
    long	off;
    char_u	*q;

    જો (!NETBEANS_OPEN)
	પાછા TRUE;

    જો (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == ખાલી ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	જો (q == ખાલી)
	    પાછા TRUE;
	vim_snprintf(buf, કદ(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	જો (curbuf->b_ffname != ખાલી)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(ચાવી_Name);
	પાછા FALSE;
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
    vim_snprintf(buf, કદ(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, ચાવી_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, કદ(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, ચાવી_Name,
		off, (long)curwin->w_cursor.lnum, (long)curwin->w_cursor.col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    પાછા TRUE;
}


/*
 * Send a save event to netbeans.
 */
    શૂન્ય
netbeans_save_buffer(buf_T *bufp)
{
    char_u	buf[64];
    પૂર્ણાંક		bufno;
    nbbuf_T	*nbbuf;

    જો (!NETBEANS_OPEN)
	પાછા;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    જો (nbbuf == ખાલી)
	પાછા;

    nbbuf->modified = 0;

    sprintf((અક્ષર *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((અક્ષર *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    શૂન્ય
netbeans_deleted_all_લીટીઓ(buf_T *bufp)
{
    char_u	buf[64];
    પૂર્ણાંક		bufno;
    nbbuf_T	*nbbuf;

    જો (!NETBEANS_OPEN)
	પાછા;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    જો (nbbuf == ખાલી)
	પાછા;

    // Don't mark as modified for initial read
    જો (nbbuf->ઉમેરો_Done)
	nbbuf->modified = 1;

    sprintf((અક્ષર *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    પૂર્ણાંક
netbeans_is_guarded(linenr_T ટોચ, linenr_T bot)
{
    sign_entry_T	*p;
    પૂર્ણાંક			lnum;

    જો (!NETBEANS_OPEN)
	પાછા FALSE;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
	જો (p->se_id >= GUARDEDOFFSET)
	    માટે (lnum = ટોચ + 1; lnum < bot; lnum++)
		જો (lnum == p->se_lnum)
		    પાછા TRUE;

    પાછા FALSE;
}

#જો defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    શૂન્ય
netbeans_draw_multisign_indicator(પૂર્ણાંક row)
{
    પૂર્ણાંક i;
    પૂર્ણાંક y;
    પૂર્ણાંક x;

    જો (!NETBEANS_OPEN)
	પાછા;

    x = 0;
    y = row * gui.char_height + 2;

    માટે (i = 0; i < gui.char_height - 3; i++)
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

#જો defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    શૂન્ય
netbeans_draw_multisign_indicator(પૂર્ણાંક row)
{
    પૂર્ણાંક i;
    પૂર્ણાંક y;
    પૂર્ણાંક x;
# જો GTK_CHECK_VERSION(3,0,0)
    cairo_t *cr = ખાલી;
# નહીંતર
    GdkDrawable *drawable = gui.drawarea->window;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    જો (!NETBEANS_OPEN)
	પાછા;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# નહીંતર
    y = row * gui.char_height + 2;
# endif

# જો GTK_CHECK_VERSION(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_height);
    cr = cairo_create(surf);
#  નહીંતર
    cr = cairo_create(gui.surface);
#  endif
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    માટે (i = 0; i < gui.char_height - 3; i++)
# જો GTK_CHECK_VERSION(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# નહીંતર
    gdk_draw_point(drawable, gui.text_gc, x+2, y++);
# endif

# જો GTK_CHECK_VERSION(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# નહીંતર
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

# જો GTK_CHECK_VERSION(3,0,0)
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
    શૂન્ય
netbeans_gutter_click(linenr_T lnum)
{
    sign_entry_T	*p;

    જો (!NETBEANS_OPEN)
	પાછા;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
    {
	જો (p->se_lnum == lnum && p->se_next && p->se_next->se_lnum == lnum)
	{
	    sign_entry_T *tail;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    જો (p->se_prev)
		p->se_prev->se_next = p->se_next;
	    નહીંતર
		curbuf->b_signlist = p->se_next;
	    p->se_next->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    માટે (tail = p->se_next;
		  tail->se_next && tail->se_next->se_lnum == lnum
				       && tail->se_next->se_id < GUARDEDOFFSET;
		  tail = tail->se_next)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_next = tail->se_next;
	    જો (tail->se_next)
		tail->se_next->se_prev = p;
	    p->se_prev = tail;
	    tail->se_next = p;
	    update_debug_sign(curbuf, lnum);
	    તોડો;
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
    સ્થિર શૂન્ય
addsigntype(
    nbbuf_T	*buf,
    પૂર્ણાંક		type_અંક,
    char_u	*typeName,
    char_u	*tooltip UNUSED,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    પૂર્ણાંક i, j;
    પૂર્ણાંક use_fg = (*fg && STRCMP(fg, "none") != 0);
    પૂર્ણાંક use_bg = (*bg && STRCMP(bg, "none") != 0);

    માટે (i = 0; i < globalsignmapused; i++)
	જો (STRCMP(typeName, globalsignmap[i]) == 0)
	    તોડો;

    જો (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_અંક, typeName, tooltip, glyphFile, fg, bg));
	જો (use_fg || use_bg)
	{
	    અક્ષર fgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    અક્ષર bgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    અક્ષર *ptr;
	    પૂર્ણાંક કિંમત;

	    કિંમત = strtol((અક્ષર *)fg, &ptr, 10);
	    જો (ptr != (અક્ષર *)fg)
		sprintf(fgbuf, "guifg=#%06x", કિંમત & 0xFFFFFF);
	    નહીંતર
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    કિંમત = strtol((અક્ષર *)bg, &ptr, 10);
	    જો (ptr != (અક્ષર *)bg)
		sprintf(bgbuf, "guibg=#%06x", કિંમત & 0xFFFFFF);
	    નહીંતર
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    જો (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    નહીંતર જો (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    નહીંતર
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	નહીંતર
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	જો (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_અંક;

	જો (globalsignmapused == globalsignmaplen)
	{
	    જો (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(અક્ષર *, globalsignmaplen);
	    }
	    નહીંતર    // grow it
	    {
		પૂર્ણાંક incr;
		પૂર્ણાંક oldlen = globalsignmaplen;
		અક્ષર **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * કદ(અક્ષર *));
		જો (globalsignmap == ખાલી)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    પાછા;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * કદ(અક્ષર *));
	    }
	}

	globalsignmap[i] = (અક્ષર *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    માટે (j = 0; j < buf->signmapused; j++)
	જો (buf->signmap[j] == i + 1)
	    પાછા;

    // add to local map
    જો (buf->signmapused == buf->signmaplen)
    {
	જો (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(પૂર્ણાંક, buf->signmaplen);
	}
	નહીંતર    // grow it
	{
	    પૂર્ણાંક incr;
	    પૂર્ણાંક oldlen = buf->signmaplen;
	    પૂર્ણાંક *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * કદ(પૂર્ણાંક));
	    જો (buf->signmap == ખાલી)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		પાછા;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * કદ(પૂર્ણાંક));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    સ્થિર પૂર્ણાંક
mapsigntype(nbbuf_T *buf, પૂર્ણાંક localsigntype)
{
    જો (--localsigntype >= 0 && localsigntype < buf->signmapused)
	પાછા buf->signmap[localsigntype];

    પાછા 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    સ્થિર long
get_buf_size(buf_T *bufp)
{
    linenr_T	lnum;
    long	char_ગણતરી = 0;
    પૂર્ણાંક		eol_size;
    long	last_check = 100000L;

    જો (bufp->b_ml.ml_flags & ML_EMPTY)
	પાછા 0;

    જો (get_fileformat(bufp) == EOL_DOS)
	eol_size = 2;
    નહીંતર
	eol_size = 1;
    માટે (lnum = 1; lnum <= bufp->b_ml.ml_લીટી_ગણતરી; ++lnum)
    {
	char_ગણતરી += ml_get_buf_len(bufp, lnum) + eol_size;
	// Check for a CTRL-C every 100000 characters
	જો (char_ગણતરી > last_check)
	{
	    ui_breakcheck();
	    જો (got_int)
		પાછા char_ગણતરી;
	    last_check = char_ગણતરી + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    જો (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_ગણતરી -= eol_size;

    પાછા char_ગણતરી;
}

/*
 * Convert character offset to lnum,col
 */
    સ્થિર pos_T *
off2pos(buf_T *buf, long offset)
{
    linenr_T	 lnum;
    સ્થિર pos_T pos;

    pos.lnum = 0;
    pos.col = 0;
    pos.coladd = 0;

    જો (!(buf->b_ml.ml_flags & ML_EMPTY))
    {
	જો ((lnum = ml_find_લીટી_or_offset(buf, (linenr_T)0, &offset)) < 0)
	    પાછા ખાલી;
	pos.lnum = lnum;
	pos.col = offset;
    }

    પાછા &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    સ્થિર pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    સ્થિર pos_T	mypos;
    long		off;

    off = strtol((અક્ષર *)*argp, (અક્ષર **)argp, 10);
    જો (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.col = strtol((અક્ષર *)*argp, (અક્ષર **)argp, 10);
	mypos.coladd = 0;
	પાછા &mypos;
    }
    પાછા off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    સ્થિર long
pos2off(buf_T *buf, pos_T *pos)
{
    long	 offset = 0;

    જો (buf->b_ml.ml_flags & ML_EMPTY)
	પાછા 0;

    જો ((offset = ml_find_લીટી_or_offset(buf, pos->lnum, 0)) < 0)
	પાછા 0;
    offset += pos->col;

    પાછા offset;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    સ્થિર શૂન્ય
print_read_સંદેશ(nbbuf_T *buf)
{
    પૂર્ણાંક	    lnum = buf->bufp->b_ml.ml_લીટી_ગણતરી;
    off_T   nchars = buf->bufp->b_orig_size;
    char_u  c;

    સંદેશ_add_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    જો (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    જો (!buf->bufp->b_start_eol)
    {
	STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    સંદેશ_add_લીટીઓ(c, (long)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_સંદેશ);
    સંદેશ_scrolled_ign = TRUE;
    સંદેશ_trunc_attr((અક્ષર *)IObuff, FALSE, 0);
    સંદેશ_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    સ્થિર શૂન્ય
print_save_સંદેશ(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    જો (nchars >= 0)
    {
	// put fname in IObuff with quotes
	સંદેશ_add_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	સંદેશ_add_લીટીઓ(c, buf->bufp->b_ml.ml_લીટી_ગણતરી,
						buf->bufp->b_orig_size);

	VIM_CLEAR(keep_સંદેશ);
	સંદેશ_scrolled_ign = TRUE;
	p = (char_u *)સંદેશ_trunc_attr((અક્ષર *)IObuff, FALSE, 0);
	જો ((સંદેશ_scrolled && !need_wait_return) || !buf->initDone)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_સંદેશ(p, 0);
	}
	સંદેશ_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    નહીંતર
    {
	અક્ષર msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_add_bang_to_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#endif // defined(FEAT_NETBEANS_INTG)
