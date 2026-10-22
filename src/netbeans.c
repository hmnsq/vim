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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_NETBEANS_INTG)

#ifndef MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#endif

#അന്തര്ഭാവ "version.h"

#define GUARDED		10000 // typenr for "guarded" annotation
#define GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#define MAX_COLOR_LENGTH 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#define VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#define VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
സ്ഥിതം അക്ഷരം *ExtEdProtocolVersion = "2.5";

സ്ഥിതം long pos2off(buf_T *, pos_T *);
സ്ഥിതം pos_T *off2pos(buf_T *, long);
സ്ഥിതം pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
സ്ഥിതം long get_buf_size(buf_T *);
സ്ഥിതം പൂർണ്ണസംഖ്യ netbeans_keystring(char_u *keystr);
സ്ഥിതം ശൂന്യം special_താക്കോലുകൾ(char_u *args);

സ്ഥിതം പൂർണ്ണസംഖ്യ getConnInfo(അക്ഷരം *file, അക്ഷരം **host, അക്ഷരം **port, അക്ഷരം **password);

സ്ഥിതം ശൂന്യം nb_init_graphics(ശൂന്യം);
സ്ഥിതം ശൂന്യം coloncmd(അക്ഷരം *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
സ്ഥിതം ശൂന്യം nb_set_curbuf(buf_T *buf);
സ്ഥിതം ശൂന്യം nb_parse_cmd(char_u *);
സ്ഥിതം പൂർണ്ണസംഖ്യ  nb_do_cmd(പൂർണ്ണസംഖ്യ, char_u *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, char_u *);
സ്ഥിതം ശൂന്യം nb_send(അക്ഷരം *buf, അക്ഷരം *fun);
സ്ഥിതം ശൂന്യം nb_free(ശൂന്യം);

#define NETBEANS_OPEN (channel_can_write_to(nb_channel))
സ്ഥിതം channel_T *nb_channel = ശൂന്യ;

സ്ഥിതം പൂർണ്ണസംഖ്യ r_cmdno;			// current command number for reply
സ്ഥിതം പൂർണ്ണസംഖ്യ dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ifdef NBDEBUG
# include "nbdebug.c"
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ needupdate = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ inAtomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nb_is_safe_string(char_u *str, char_u *allowed)
{
    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ FALSE;
    വേണ്ടി (char_u *p = str; *p != NUL; p++)
	എങ്കിൽ (vim_strchr(allowed, *p) == ശൂന്യ)
	    തിരികെ FALSE;
    തിരികെ TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    സ്ഥിതം ശൂന്യം
nb_channel_closed(ശൂന്യം)
{
    nb_channel = ശൂന്യ;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    സ്ഥിതം ശൂന്യം
netbeans_close(ശൂന്യം)
{
    എങ്കിൽ (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	എങ്കിൽ (nb_channel != ശൂന്യ)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = ശൂന്യ;
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

    സ്ഥിതം പൂർണ്ണസംഖ്യ
netbeans_connect(അക്ഷരം *params, പൂർണ്ണസംഖ്യ doabort)
{
    പൂർണ്ണസംഖ്യ		port;
    അക്ഷരം	buf[32];
    അക്ഷരം	*hostname = ശൂന്യ;
    അക്ഷരം	*address = ശൂന്യ;
    അക്ഷരം	*password = ശൂന്യ;
    അക്ഷരം	*fname;
    അക്ഷരം	*arg = ശൂന്യ;

    എങ്കിൽ (*params == '=')
    {
	// "=fname": Read info from specified file.
	എങ്കിൽ (getConnInfo(params + 1, &hostname, &address, &password) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	എങ്കിൽ (arg == ശൂന്യ && (fname = getenv("__NETBEANS_CONINFO")) != ശൂന്യ)
	{
	    // "": get info from file specified in environment
	    എങ്കിൽ (getConnInfo(fname, &hostname, &address, &password) == FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (arg != ശൂന്യ)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		address = strchr(hostname, ':');
		എങ്കിൽ (address != ശൂന്യ)
		{
		    *address++ = '\0';
		    password = strchr(address, ':');
		    എങ്കിൽ (password != ശൂന്യ)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    എങ്കിൽ (hostname == ശൂന്യ || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    എങ്കിൽ (address == ശൂന്യ)
		address = getenv("__NETBEANS_SOCKET");
	    എങ്കിൽ (password == ശൂന്യ)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    എങ്കിൽ (hostname != ശൂന്യ)
		hostname = (അക്ഷരം *)vim_strsave((char_u *)hostname);
	    എങ്കിൽ (address != ശൂന്യ)
		address = (അക്ഷരം *)vim_strsave((char_u *)address);
	    എങ്കിൽ (password != ശൂന്യ)
		password = (അക്ഷരം *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    എങ്കിൽ (hostname == ശൂന്യ || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (അക്ഷരം *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    എങ്കിൽ (address == ശൂന്യ || *address == '\0')
    {
	vim_free(address);
	address = (അക്ഷരം *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    എങ്കിൽ (password == ശൂന്യ || *password == '\0')
    {
	vim_free(password);
	password = (അക്ഷരം *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    എങ്കിൽ (hostname != ശൂന്യ && address != ശൂന്യ && password != ശൂന്യ)
    {
	port = atoi(address);
	nb_channel = channel_open(hostname, port, 3000, nb_channel_closed);
	എങ്കിൽ (nb_channel != ശൂന്യ)
	{
	    // success
#ifdef FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#endif

	    // success, login
	    vim_snprintf(buf, വലുപ്പം(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", ExtEdProtocolVersion);
	    nb_send(buf, "externaleditor_version");
	}
    }

    എങ്കിൽ (nb_channel == ശൂന്യ && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(address);
    vim_free(password);
    തിരികെ NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
getConnInfo(അക്ഷരം *file, അക്ഷരം **host, അക്ഷരം **port, അക്ഷരം **auth)
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
    എങ്കിൽ (mch_stat(file, &st) == 0 && (st.st_mode & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_mode_for_netbeans_connection_info_file_str),
									file);
	തിരികെ FAIL;
    }
#endif

    fp = mch_fopen(file, "r");
    എങ്കിൽ (fp == ശൂന്യ)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	തിരികെ FAIL;
    }

    // Read the file. There should be one of each parameter
    ആകുമ്പോൾ ((lp = (char_u *)fgets((അക്ഷരം *)buf, BUFSIZ, fp)) != ശൂന്യ)
    {
	എങ്കിൽ ((nlp = vim_strchr(lp, '\n')) != ശൂന്യ)
	    *nlp = 0;	    // strip off the trailing newline

	എങ്കിൽ (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (അക്ഷരം *)vim_strsave(&buf[5]);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*port);
	    *port = (അക്ഷരം *)vim_strsave(&buf[5]);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (അക്ഷരം *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    തിരികെ OK;
}


ഘടന keyqueue
{
    char_u	    *keystr;
    ഘടന keyqueue *next;
    ഘടന keyqueue *prev;
};

തരനിർവചനം ഘടന keyqueue താക്കോൽ_Q_T;

സ്ഥിതം താക്കോൽ_Q_T താക്കോൽ_Head; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    സ്ഥിതം ശൂന്യം
postpone_keycommand(char_u *keystr)
{
    താക്കോൽ_Q_T *കണ്ണി;

    കണ്ണി = ALLOC_ONE(താക്കോൽ_Q_T);
    എങ്കിൽ (കണ്ണി == ശൂന്യ)
	തിരികെ;  // out of memory, drop the key

    എങ്കിൽ (താക്കോൽ_Head.next == ശൂന്യ) // initialize circular queue
    {
	താക്കോൽ_Head.next = &താക്കോൽ_Head;
	താക്കോൽ_Head.prev = &താക്കോൽ_Head;
    }

    // insert node at tail of queue
    കണ്ണി->next = &താക്കോൽ_Head;
    കണ്ണി->prev = താക്കോൽ_Head.prev;
    താക്കോൽ_Head.prev->next = കണ്ണി;
    താക്കോൽ_Head.prev = കണ്ണി;

    കണ്ണി->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    സ്ഥിതം ശൂന്യം
handle_താക്കോൽ_queue(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ postponed = FALSE;

    ആകുമ്പോൾ (!postponed && താക്കോൽ_Head.next && താക്കോൽ_Head.next != &താക്കോൽ_Head)
    {
	// first, unlink the node
	താക്കോൽ_Q_T *കണ്ണി = താക്കോൽ_Head.next;
	താക്കോൽ_Head.next = കണ്ണി->next;
	കണ്ണി->next->prev = കണ്ണി->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	എങ്കിൽ (കണ്ണി->keystr != ശൂന്യ)
	    postponed = !netbeans_keystring(കണ്ണി->keystr);
	vim_free(കണ്ണി->keystr);

	// Finally, dispose of the node
	vim_free(കണ്ണി);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    ശൂന്യം
netbeans_parse_messages(ശൂന്യം)
{
    readq_T	*കണ്ണി;
    char_u	*buffer;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		own_കണ്ണി;

    ആകുമ്പോൾ (nb_channel != ശൂന്യ)
    {
	കണ്ണി = channel_peek(nb_channel, PART_SOCK);
	എങ്കിൽ (കണ്ണി == ശൂന്യ)
	    നിർത്തുക;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_first_nl(കണ്ണി);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    എങ്കിൽ (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		തിരികെ;
	    തുടരുക;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	എങ്കിൽ (*p == NUL)
	{
	    own_കണ്ണി = TRUE;
	    buffer = channel_get(nb_channel, PART_SOCK, ശൂന്യ);
	    // "node" is now invalid!
	}
	അല്ലെങ്കിൽ
	{
	    own_കണ്ണി = FALSE;
	    buffer = കണ്ണി->rq_buffer;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(buffer);

	എങ്കിൽ (own_കണ്ണി)
	    // buffer finished, dispose of it
	    vim_free(buffer);
	അല്ലെങ്കിൽ എങ്കിൽ (nb_channel != ശൂന്യ)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (പൂർണ്ണസംഖ്യ)(p - buffer));
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
    സ്ഥിതം ശൂന്യം
nb_parse_cmd(char_u *cmd)
{
    അക്ഷരം	*verb;
    അക്ഷരം	*q;
    പൂർണ്ണസംഖ്യ		bufno;
    പൂർണ്ണസംഖ്യ		isfunc = -1;

    എങ്കിൽ (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    എങ്കിൽ (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_column = false;

	// The IDE is breaking the connection.
	netbeans_close();
	തിരികെ;
    }

    bufno = strtol((അക്ഷരം *)cmd, &verb, 10);

    എങ്കിൽ (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	തിരികെ;
    }
    ++verb; // skip colon

    വേണ്ടി (q = verb; *q; q++)
    {
	എങ്കിൽ (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    നിർത്തുക;
	}
    }

    എങ്കിൽ (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_in_str), cmd);
	തിരികെ;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (അക്ഷരം *)skipwhite((char_u *)q);

    എങ്കിൽ (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
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

ഘടന nbbuf_struct
{
    buf_T		*bufp;
    unsigned പൂർണ്ണസംഖ്യ	 fireChanges:1;
    unsigned പൂർണ്ണസംഖ്യ	 initDone:1;
    unsigned പൂർണ്ണസംഖ്യ	 തിരുകുക_Done:1;
    unsigned പൂർണ്ണസംഖ്യ	 modified:1;
    പൂർണ്ണസംഖ്യ			 nbbuf_സംഖ്യ;
    അക്ഷരം		*displayname;
    പൂർണ്ണസംഖ്യ			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

തരനിർവചനം ഘടന nbbuf_struct nbbuf_T;

സ്ഥിതം nbbuf_T *buf_list = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ buf_list_size = 0;	// size of buf_list
സ്ഥിതം പൂർണ്ണസംഖ്യ buf_list_used = 0;	// nr of entries in buf_list actually in use

സ്ഥിതം അക്ഷരം **globalsignmap = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ globalsignmaplen = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ globalsignmapused = 0;

സ്ഥിതം പൂർണ്ണസംഖ്യ  mapsigntype(nbbuf_T *, പൂർണ്ണസംഖ്യ localsigntype);
സ്ഥിതം ശൂന്യം addsigntype(nbbuf_T *, പൂർണ്ണസംഖ്യ localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
സ്ഥിതം ശൂന്യം print_read_സന്ദേശം(nbbuf_T *buf);
സ്ഥിതം ശൂന്യം print_save_സന്ദേശം(nbbuf_T *buf, off_T nchars);

സ്ഥിതം പൂർണ്ണസംഖ്യ curPCtype = -1;

/*
 * Free netbeans resources.
 */
    സ്ഥിതം ശൂന്യം
nb_free(ശൂന്യം)
{
    താക്കോൽ_Q_T *താക്കോൽ_കണ്ണി = താക്കോൽ_Head.next;
    nbbuf_T buf;
    പൂർണ്ണസംഖ്യ i;

    // free the netbeans buffer list
    വേണ്ടി (i = 0; i < buf_list_used; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	എങ്കിൽ (buf.bufp != ശൂന്യ && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_size = 0;
    buf_list_used = 0;

    // free the queued key commands
    ആകുമ്പോൾ (താക്കോൽ_കണ്ണി != ശൂന്യ && താക്കോൽ_കണ്ണി != &താക്കോൽ_Head)
    {
	താക്കോൽ_Q_T *next = താക്കോൽ_കണ്ണി->next;
	vim_free(താക്കോൽ_കണ്ണി->keystr);
	vim_free(താക്കോൽ_കണ്ണി);
	എങ്കിൽ (next == &താക്കോൽ_Head)
	{
	    താക്കോൽ_Head.next = &താക്കോൽ_Head;
	    താക്കോൽ_Head.prev = &താക്കോൽ_Head;
	    നിർത്തുക;
	}
	താക്കോൽ_കണ്ണി = next;
    }

    // free the queued netbeans commands
    എങ്കിൽ (nb_channel != ശൂന്യ)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nb_getbufno(buf_T *bufp)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < buf_list_used; i++)
	എങ്കിൽ (buf_list[i].bufp == bufp)
	    തിരികെ i;
    തിരികെ -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    പൂർണ്ണസംഖ്യ
isNetbeansBuffer(buf_T *bufp)
{
    തിരികെ NETBEANS_OPEN && bufp->b_netbeans_file;
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
    പൂർണ്ണസംഖ്യ
isNetbeansModified(buf_T *bufp)
{
    എങ്കിൽ (isNetbeansBuffer(bufp))
    {
	പൂർണ്ണസംഖ്യ bufno = nb_getbufno(bufp);

	എങ്കിൽ (bufno > 0)
	    തിരികെ buf_list[bufno].modified;
	അല്ലെങ്കിൽ
	    തിരികെ FALSE;
    }
    അല്ലെങ്കിൽ
	തിരികെ FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    സ്ഥിതം nbbuf_T *
nb_get_buf(പൂർണ്ണസംഖ്യ bufno)
{
    // find or create a buffer with the given number
    പൂർണ്ണസംഖ്യ incr;

    എങ്കിൽ (bufno <= 0)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * വലുപ്പം(nbbuf_T));
	buf_list_size = 100;
    }
    എങ്കിൽ (bufno >= buf_list_used) // new
    {
	എങ്കിൽ (bufno >= buf_list_size) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_size + 90;
	    buf_list_size += incr;
	    bufsize = buf_list_size * വലുപ്പം(nbbuf_T);
	    എങ്കിൽ (bufsize == 0 || bufsize / വലുപ്പം(nbbuf_T)
						      != (size_t)buf_list_size)
	    {
		// list size overflow, bail out
		തിരികെ ശൂന്യ;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    എങ്കിൽ (buf_list == ശൂന്യ)
	    {
		vim_free(t_buf_list);
		buf_list_size = 0;
		തിരികെ ശൂന്യ;
	    }
	    vim_memset(buf_list + buf_list_size - incr, 0,
						      incr * വലുപ്പം(nbbuf_T));
	}

	ആകുമ്പോൾ (buf_list_used <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_used].fireChanges = 1;
	    ++buf_list_used;
	}
    }

    തിരികെ buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
എണ്ണം_changed_buffers(ശൂന്യം)
{
    buf_T	*bufp;
    പൂർണ്ണസംഖ്യ		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	എങ്കിൽ (bufp->b_changed)
	    ++n;
    തിരികെ n;
}

/*
 * End the netbeans session.
 */
    ശൂന്യം
netbeans_end(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;
    സ്ഥിതം അക്ഷരം buf[128];

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    വേണ്ടി (i = 0; i < buf_list_used; i++)
    {
	എങ്കിൽ (!buf_list[i].bufp)
	    തുടരുക;
	എങ്കിൽ (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, ശൂന്യ);
	buf_list[i].bufp = ശൂന്യ;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    സ്ഥിതം ശൂന്യം
nb_send(അക്ഷരം *buf, അക്ഷരം *fun)
{
    എങ്കിൽ (nb_channel != ശൂന്യ)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (പൂർണ്ണസംഖ്യ)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    സ്ഥിതം ശൂന്യം
nb_reply_nil(പൂർണ്ണസംഖ്യ cmdno)
{
    അക്ഷരം reply[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    sprintf(reply, "%d\n", cmdno);
    nb_send(reply, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    സ്ഥിതം ശൂന്യം
nb_reply_text(പൂർണ്ണസംഖ്യ cmdno, char_u *result)
{
    char_u *reply;

    nbdebug(("REP %d: %s\n", cmdno, (അക്ഷരം *)result));

    reply = alloc(STRLEN(result) + 32);
    sprintf((അക്ഷരം *)reply, "%d %s\n", cmdno, (അക്ഷരം *)result);
    nb_send((അക്ഷരം *)reply, "nb_reply_text");

    vim_free(reply);
}


/*
 * Send a response with a number result code.
 */
    സ്ഥിതം ശൂന്യം
nb_reply_nr(പൂർണ്ണസംഖ്യ cmdno, long result)
{
    അക്ഷരം reply[32];

    nbdebug(("REP %d: %ld\n", cmdno, result));

    sprintf(reply, "%d %ld\n", cmdno, result);
    nb_send(reply, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    സ്ഥിതം char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ ശൂന്യ;
    വേണ്ടി (; *p; p++)
    {
	തിരഞ്ഞെടുപ്പ് (*p)
	{
	    അവസ്ഥ '\"':
	    അവസ്ഥ '\\':
		*q++ = '\\'; *q++ = *p; നിർത്തുക;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    അവസ്ഥ '\n':
		*q++ = '\\'; *q++ = 'n'; നിർത്തുക;
	    അവസ്ഥ '\r':
		*q++ = '\\'; *q++ = 'r'; നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		*q++ = *p;
		നിർത്തുക;
	}
    }
    *q = '\0';

    തിരികെ buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    സ്ഥിതം അക്ഷരം *
nb_unquote(char_u *p, char_u **endp)
{
    അക്ഷരം *result = 0;
    അക്ഷരം *q;
    പൂർണ്ണസംഖ്യ done = 0;

    // result is never longer than input
    result = alloc_clear(STRLEN(p) + 1);
    എങ്കിൽ (result == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	result[0] = NUL;
	തിരികെ result;
    }

    വേണ്ടി (q = result; !done && *p != NUL;)
    {
	തിരഞ്ഞെടുപ്പ് (*p)
	{
	    അവസ്ഥ '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		done = 1;
		നിർത്തുക;

	    അവസ്ഥ '\\':
		++p;
		തിരഞ്ഞെടുപ്പ് (*p)
		{
		    അവസ്ഥ '\\':	*q++ = '\\';	നിർത്തുക;
		    അവസ്ഥ 'n':	*q++ = '\n';	നിർത്തുക;
		    അവസ്ഥ 't':	*q++ = '\t';	നിർത്തുക;
		    അവസ്ഥ 'r':	*q++ = '\r';	നിർത്തുക;
		    അവസ്ഥ '"':	*q++ = '"';	നിർത്തുക;
		    അവസ്ഥ NUL:	--p;		നിർത്തുക;
		    // default: skip over illegal chars
		}
		++p;
		നിർത്തുക;

	    സ്ഥിരസ്ഥിതി:
		*q++ = *p++;
	}
    }

    എങ്കിൽ (endp != ശൂന്യ)
	*endp = p;

    തിരികെ result;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    സ്ഥിതം ശൂന്യം
nb_partialremove(linenr_T lnum, colnr_T first, colnr_T last)
{
    char_u *oldtext, *newtext;
    പൂർണ്ണസംഖ്യ oldlen;
    പൂർണ്ണസംഖ്യ lastbyte = last;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    എങ്കിൽ (first >= (colnr_T)oldlen || oldlen == 0)  // just in case
	തിരികെ;
    എങ്കിൽ (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - first));
    എങ്കിൽ (newtext == ശൂന്യ)
	തിരികെ;

    mch_memmove(newtext, oldtext, first);
    STRMOVE(newtext + first, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    സ്ഥിതം ശൂന്യം
nb_joinlines(linenr_T first, linenr_T other)
{
    പൂർണ്ണസംഖ്യ len_first, len_other;
    char_u *p;

    len_first = ml_get_len(first);
    len_other = ml_get_len(other);
    p = alloc(len_first + len_other + 1);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nb_do_cmd(
    പൂർണ്ണസംഖ്യ		bufno,
    char_u	*cmd,
    പൂർണ്ണസംഖ്യ		func,
    പൂർണ്ണസംഖ്യ		cmdno,
    char_u	*args)	    // points to space before arguments or NUL
{
    പൂർണ്ണസംഖ്യ		do_update = 0;
    long	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    സ്ഥിതം പൂർണ്ണസംഖ്യ	skip = 0;
    പൂർണ്ണസംഖ്യ		retval = OK;
    അക്ഷരം	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (അക്ഷരം *)args));

    എങ്കിൽ (func)
    {
// =====================================================================
	എങ്കിൽ (streq((അക്ഷരം *)cmd, "getModified"))
	{
	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
		// Return the number of buffers that are modified.
		nb_reply_nr(cmdno, (long)എണ്ണം_changed_buffers());
	    അല്ലെങ്കിൽ
		// Return whether the buffer is modified.
		nb_reply_nr(cmdno, (long)(buf->bufp->b_changed
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_reply_nr(cmdno, (long)എണ്ണം_changed_buffers());
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "getCursor"))
	{
	    char_u text[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((അക്ഷരം *)text, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (long)curwin->w_cursor.lnum,
		    (പൂർണ്ണസംഖ്യ)curwin->w_cursor.col,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_reply_text(cmdno, text);
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "getAnno"))
	{
	    long linenum = 0;
#ifdef FEAT_SIGNS
	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_getanno));
		retval = FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		പൂർണ്ണസംഖ്യ ser_അക്കം;

		cp = (അക്ഷരം *)args;
		ser_അക്കം = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (long)buf_findsign(buf->bufp, ser_അക്കം, ശൂന്യ);
	    }
#endif
	    nb_reply_nr(cmdno, linenum);
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "getLength"))
	{
	    long len = 0;

	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_buffer_identifier_in_getlength));
		retval = FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		len = get_buf_size(buf->bufp);
	    }
	    nb_reply_nr(cmdno, len);
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "getText"))
	{
	    long	len;
	    linenr_T	nlines;
	    char_u	*text = ശൂന്യ;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*വരി;

	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_buffer_identifier_in_gettext));
		retval = FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		len = get_buf_size(buf->bufp);
		nlines = buf->bufp->b_ml.ml_വരി_എണ്ണം;
		text = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		എങ്കിൽ (text == ശൂന്യ)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		അല്ലെങ്കിൽ
		{
		    p = text;
		    *p++ = '\"';
		    വേണ്ടി (; lno <= nlines ; lno++)
		    {
			വരി = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			എങ്കിൽ (വരി != ശൂന്യ)
			{
			    STRCPY(p, വരി);
			    p += STRLEN(വരി);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(വരി);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    എങ്കിൽ (text == ശൂന്യ)
		nb_reply_text(cmdno, (char_u *)"");
	    അല്ലെങ്കിൽ
	    {
		nb_reply_text(cmdno, text);
		vim_free(text);
	    }
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "remove"))
	{
	    long എണ്ണം;
	    pos_T first, last;
	    pos_T *pos;
	    pos_T *next;
	    linenr_T del_from_lnum, del_to_lnum;  // lines to be deleted as a whole
	    പൂർണ്ണസംഖ്യ oldFire = netbeansFireChanges;
	    പൂർണ്ണസംഖ്യ oldSuppress = netbeans_Suppress_No_വരികൾ;
	    പൂർണ്ണസംഖ്യ wasChanged;

	    എങ്കിൽ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (അക്ഷരം *) cmd));
		nb_reply_nil(cmdno);
		തിരികെ OK;
	    }

	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_buffer_identifier_in_remove));
		retval = FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_വരികൾ = TRUE;

		nb_set_curbuf(buf->bufp);
		wasChanged = buf->bufp->b_changed;
		cp = (അക്ഷരം *)args;
		off = strtol(cp, &cp, 10);
		എണ്ണം = strtol(cp, &cp, 10);
		args = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		എങ്കിൽ (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_വരികൾ = oldSuppress;
		    തിരികെ FAIL;
		}
		first = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      first.lnum, first.col));
		pos = off2pos(buf->bufp, off+എണ്ണം-1);
		എങ്കിൽ (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_വരികൾ = oldSuppress;
		    തിരികെ FAIL;
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
		next = off2pos(buf->bufp, off + എണ്ണം);

		// Remove part of the first line.
		എങ്കിൽ (first.col != 0
				|| (next != ശൂന്യ && first.lnum == next->lnum))
		{
		    എങ്കിൽ (first.lnum != last.lnum
			    || (next != ശൂന്യ && first.lnum != next->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(first.lnum, first.col,
							     (colnr_T)MAXCOL);
			എങ്കിൽ (first.lnum == last.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(first.lnum, next->lnum);
			    del_to_lnum = next->lnum;
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			// remove within one line
			nb_partialremove(first.lnum, first.col, last.col);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		എങ്കിൽ (first.lnum != last.lnum && next != ശൂന്യ
			&& next->col != 0 && last.lnum == next->lnum)
		{
		    nb_partialremove(last.lnum, 0, last.col);
		    എങ്കിൽ (del_from_lnum > first.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(first.lnum, last.lnum);
		    }
		    അല്ലെങ്കിൽ
			// First line is deleted as a whole, keep the last
			// line.
			--del_to_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		എങ്കിൽ (first.lnum != last.lnum && del_from_lnum > first.lnum
			&& next != ശൂന്യ && last.lnum != next->lnum)
		{
		    nb_joinlines(first.lnum, next->lnum);
		    del_to_lnum = next->lnum;
		}

		// Delete whole lines if there are any.
		എങ്കിൽ (del_to_lnum >= del_from_lnum)
		{
		    പൂർണ്ണസംഖ്യ i;

		    // delete signs from the lines being deleted
		    വേണ്ടി (i = del_from_lnum; i <= del_to_lnum; i++)
		    {
			പൂർണ്ണസംഖ്യ id = buf_findsign_id(buf->bufp, (linenr_T)i, ശൂന്യ);
			എങ്കിൽ (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, ശൂന്യ);
			}
			അല്ലെങ്കിൽ
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_to_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.col = 0;
		    del_വരികൾ(del_to_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = first;
		check_cursor_lnum();
		buf->bufp->b_changed = wasChanged; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_വരികൾ = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_reply_nil(cmdno);
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "insert"))
	{
	    char_u	*to_free;

	    എങ്കിൽ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (അക്ഷരം *) cmd));
		nb_reply_nil(cmdno);
		തിരികെ OK;
	    }

	    // get offset
	    cp = (അക്ഷരം *)args;
	    off = strtol(cp, &cp, 10);
	    args = (char_u *)cp;

	    // get text to be inserted
	    args = skipwhite(args);
	    args = to_free = (char_u *)nb_unquote(args, ശൂന്യ);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_buffer_identifier_in_തിരുകുക));
		retval = FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (args != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ	ff_detected = EOL_UNKNOWN;
		പൂർണ്ണസംഖ്യ	buf_was_empty = (buf->bufp->b_ml.ml_flags & ML_EMPTY);
		size_t	len = 0;
		പൂർണ്ണസംഖ്യ	added = 0;
		പൂർണ്ണസംഖ്യ	oldFire = netbeansFireChanges;
		പൂർണ്ണസംഖ്യ	old_b_changed;
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
		എങ്കിൽ (pos != ശൂന്യ)
		{
		    എങ്കിൽ (pos->lnum <= 0)
			lnum_start = 1;
		    അല്ലെങ്കിൽ
			lnum_start = pos->lnum;
		}
		അല്ലെങ്കിൽ
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    എങ്കിൽ (buf_was_empty)
			lnum_start = 1;	    // above empty line
		    അല്ലെങ്കിൽ
			lnum_start = curbuf->b_ml.ml_വരി_എണ്ണം + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_start;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		ആകുമ്പോൾ (*args != NUL)
		{
		    nlp = vim_strchr(args, '\n');
		    എങ്കിൽ (nlp == ശൂന്യ)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(args);
		    }
		    അല്ലെങ്കിൽ
		    {
			len = nlp - args;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			എങ്കിൽ (nlp > args && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			അല്ലെങ്കിൽ
			    ff_detected = EOL_UNIX;
		    }
		    args[len] = NUL;

		    എങ്കിൽ (lnum == lnum_start
			    && ((pos != ശൂന്യ && pos->col > 0)
				|| (lnum == 1 && buf_was_empty)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			പൂർണ്ണസംഖ്യ	col = pos == ശൂന്യ ? 0 : pos->col;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			എങ്കിൽ (newline != ശൂന്യ)
			{
			    mch_memmove(newline, oldline, (size_t)col);
			    newline[col] = NUL;
			    STRCAT(newline, args);
			    STRCAT(newline, oldline + col);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), args,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    എങ്കിൽ (nlp == ശൂന്യ)
			നിർത്തുക;
		    ++lnum;
		    args = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_വരികൾ_mark(lnum_start - 1, (long)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		എങ്കിൽ (buf_was_empty)
		{
		    എങ്കിൽ (ff_detected == EOL_UNKNOWN)
#എങ്കിൽ defined(MSWIN)
			ff_detected = EOL_DOS;
#അല്ലെങ്കിൽ
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
	അല്ലെങ്കിൽ
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_reply_nil(cmdno);
	    retval = FAIL;
	}
    }
    അല്ലെങ്കിൽ // Not a function; no reply required.
    {
// =====================================================================
	എങ്കിൽ (streq((അക്ഷരം *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_buffer_identifier_in_create));
		തിരികെ FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, ശൂന്യ, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->തിരുകുക_Done = FALSE;
#എങ്കിൽ defined(FEAT_MENU) && defined(FEAT_GUI)
	    എങ്കിൽ (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "insertDone"))
	{
	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    അല്ലെങ്കിൽ
	    {
		buf->bufp->b_start_eol = *args == 'T';
		buf->തിരുകുക_Done = TRUE;
		args += 2;
		buf->bufp->b_p_ro = *args == 'T';
		print_read_സന്ദേശം(buf);
	    }
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "saveDone"))
	{
	    long savedChars = atol((അക്ഷരം *)args);

	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    അല്ലെങ്കിൽ
		print_save_സന്ദേശം(buf, savedChars);
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "startDocumentListen"))
	{
	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_startdocumentlisten));
		തിരികെ FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "stopDocumentListen"))
	{
	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_stopdocumentlisten));
		തിരികെ FAIL;
	    }
	    buf->fireChanges = 0;
	    എങ്കിൽ (buf->bufp != ശൂന്യ && buf->bufp->b_was_netbeans_file)
	    {
		എങ്കിൽ (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_lost_for_buffer_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_lost_for_buffer_nr),
							   buf->bufp->b_fnum);
		}
		അല്ലെങ്കിൽ
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
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "setTitle"))
	{
	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_buffer_identifier_in_settitle));
		തിരികെ FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ശൂന്യ);
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "initDone"))
	{
	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_buffer_identifier_in_initdone));
		തിരികെ FAIL;
	    }
	    do_update = 1;
	    buf->initDone = TRUE;
	    nb_set_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_താക്കോൽ_queue();
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "setBufferNumber")
		|| streq((അക്ഷരം *)cmd, "putBufferNumber"))
	{
	    char_u	*path;
	    buf_T	*bufp;

	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_buffer_identifier_in_setbuffernumber));
		തിരികെ FAIL;
	    }
	    path = (char_u *)nb_unquote(args, ശൂന്യ);
	    എങ്കിൽ (path == ശൂന്യ)
		തിരികെ FAIL;
	    bufp = buflist_findname(path);
	    vim_free(path);
	    എങ്കിൽ (bufp == ശൂന്യ)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", args));
		semsg(_(e_file_str_not_found_in_setbuffernumber), args);
		തിരികെ FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_സംഖ്യ = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    എങ്കിൽ (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    അല്ലെങ്കിൽ
	    {
		buf->initDone = TRUE;

		// handle any postponed key commands
		handle_താക്കോൽ_queue();
	    }

// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "setFullName"))
	{
	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_buffer_identifier_in_setfullname));
		തിരികെ FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ശൂന്യ);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#എങ്കിൽ defined(FEAT_MENU) && defined(FEAT_GUI)
	    എങ്കിൽ (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "editFile"))
	{
	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_buffer_identifier_in_editfile));
		തിരികെ FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ശൂന്യ);
	    do_ecmd(0, (char_u *)buf->displayname, ശൂന്യ, ശൂന്യ, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->initDone = TRUE;
	    do_update = 1;
	    maketitle();
#എങ്കിൽ defined(FEAT_MENU) && defined(FEAT_GUI)
	    എങ്കിൽ (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "setVisible"))
	{
	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		എങ്കിൽ (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setvisible));
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (streq((അക്ഷരം *)args, "T") && buf->bufp != curbuf)
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
		എങ്കിൽ (gui.in_use)
		    gui_mch_set_foreground();
#endif
	    }
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "raise"))
	{
#ifdef FEAT_GUI
	    // Bring gvim to the foreground.
	    എങ്കിൽ (gui.in_use)
		gui_mch_set_foreground();
#endif
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "setModified"))
	{
	    പൂർണ്ണസംഖ്യ prev_b_changed;

	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		എങ്കിൽ (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setmodified));
		തിരികെ FAIL;
	    }
	    prev_b_changed = buf->bufp->b_changed;
	    എങ്കിൽ (streq((അക്ഷരം *)args, "T"))
		buf->bufp->b_changed = TRUE;
	    അല്ലെങ്കിൽ
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		എങ്കിൽ (buf->bufp->b_ffname != ശൂന്യ
			&& mch_stat((അക്ഷരം *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_time(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_changed = FALSE;
	    }
	    buf->modified = buf->bufp->b_changed;
	    എങ്കിൽ (prev_b_changed != buf->bufp->b_changed)
	    {
		check_status(buf->bufp);
		redraw_tabline = TRUE;
#എങ്കിൽ defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "setModtime"))
	{
	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    അല്ലെങ്കിൽ
	    {
		buf->bufp->b_mtime = atoi((അക്ഷരം *)args);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "setReadOnly"))
	{
	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)args, "T"))
		buf->bufp->b_p_ro = TRUE;
	    അല്ലെങ്കിൽ
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "showBalloon"))
	{
#എങ്കിൽ defined(FEAT_BEVAL_GUI)
	    സ്ഥിതം അക്ഷരം	*text = ശൂന്യ;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    എങ്കിൽ (balloonEval != ശൂന്യ)
	    {
		vim_free(text);
		text = nb_unquote(args, ശൂന്യ);
		എങ്കിൽ (text != ശൂന്യ)
		    gui_mch_post_balloon(balloonEval, (char_u *)text);
	    }
#endif
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "setDot"))
	{
	    pos_T *pos;
#ifdef NBDEBUG
	    char_u *s;
#endif

	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_buffer_identifier_in_setdot));
		തിരികെ FAIL;
	    }

	    nb_set_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    എങ്കിൽ (VIsual_active)
		end_visual_mode();
#ifdef NBDEBUG
	    s = args;
#endif
	    pos = get_off_or_lnum(buf->bufp, &args);
	    എങ്കിൽ (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#ifdef FEAT_FOLDING
		foldOpenCursor();
#endif
	    }
	    അല്ലെങ്കിൽ
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
	    എങ്കിൽ (State == MODE_HITRETURN || State == MODE_ASKMORE)
	    {
#ifdef FEAT_GUI_GTK
		എങ്കിൽ (gui.in_use && gtk_main_level() > 0)
		    gtk_main_quit();
#endif
	    }
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "close"))
	{
#ifdef NBDEBUG
	    അക്ഷരം *name = "<NONE>";
#endif

	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_buffer_identifier_in_close));
		തിരികെ FAIL;
	    }

#ifdef NBDEBUG
	    എങ്കിൽ (buf->displayname != ശൂന്യ)
		name = buf->displayname;
#endif
	    എങ്കിൽ (buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		എങ്കിൽ (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    എങ്കിൽ (buf->bufp != ശൂന്യ)
		do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = ശൂന്യ;
	    buf->initDone = FALSE;
	    do_update = 1;
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "defineAnnoType"))
	{
#ifdef FEAT_SIGNS
	    പൂർണ്ണസംഖ്യ type_അക്കം;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    പൂർണ്ണസംഖ്യ parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_buffer_identifier_in_defineannotype));
		തിരികെ FAIL;
	    }

	    cp = (അക്ഷരം *)args;
	    type_അക്കം = strtol(cp, &cp, 10);
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
	    എങ്കിൽ (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(args);
	    bg = vim_strsave(p);
	    എങ്കിൽ (STRLEN(fg) > MAX_COLOR_LENGTH || STRLEN(bg) > MAX_COLOR_LENGTH)
	    {
		emsg(_(e_highlighting_color_name_too_long_in_defineAnnoType));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (!nb_is_safe_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_safe_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_safe_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_in_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (typeName != ശൂന്യ && tooltip != ശൂന്യ && glyphFile != ശൂന്യ)
		addsigntype(buf, type_അക്കം, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    എങ്കിൽ (parse_error)
		തിരികെ FAIL;

#endif
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "addAnno"))
	{
#ifdef FEAT_SIGNS
	    പൂർണ്ണസംഖ്യ ser_അക്കം;
	    പൂർണ്ണസംഖ്യ local_Type_അക്കം;
	    പൂർണ്ണസംഖ്യ type_അക്കം;
	    pos_T *pos;

	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_addanno));
		തിരികെ FAIL;
	    }

	    do_update = 1;

	    cp = (അക്ഷരം *)args;
	    ser_അക്കം = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_അക്കം = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    type_അക്കം = mapsigntype(buf, local_Type_അക്കം);

	    pos = get_off_or_lnum(buf->bufp, &args);

	    cp = (അക്ഷരം *)args;
	    vim_ignored = (പൂർണ്ണസംഖ്യ)strtol(cp, &cp, 10);
	    args = (char_u *)cp;
# ifdef NBDEBUG
	    എങ്കിൽ (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    എങ്കിൽ (ser_അക്കം >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_അക്കം, pos->lnum, type_അക്കം, buf->bufp->b_fnum);
		എങ്കിൽ (type_അക്കം == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_അക്കം,
						       buf->bufp->b_fnum);
	    }
#endif
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "removeAnno"))
	{
#ifdef FEAT_SIGNS
	    പൂർണ്ണസംഖ്യ ser_അക്കം;

	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		തിരികെ FAIL;
	    }
	    do_update = 1;
	    cp = (അക്ഷരം *)args;
	    ser_അക്കം = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_അക്കം, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#endif
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "moveAnnoToFront"))
	{
#ifdef FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#endif
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "guard") || streq((അക്ഷരം *)cmd, "unguard"))
	{
	    പൂർണ്ണസംഖ്യ len;
	    pos_T first;
	    pos_T last;
	    pos_T *pos;
	    പൂർണ്ണസംഖ്യ un = (cmd[0] == 'u');
	    സ്ഥിതം പൂർണ്ണസംഖ്യ guardId = GUARDEDOFFSET;

	    എങ്കിൽ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (അക്ഷരം *) cmd));
		തിരികെ OK;
	    }

	    nb_init_graphics();

	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		തിരികെ FAIL;
	    }
	    nb_set_curbuf(buf->bufp);
	    cp = (അക്ഷരം *)args;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, ശൂന്യ, 10);
	    args = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    എങ്കിൽ (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    അല്ലെങ്കിൽ
	    {
		first = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		എങ്കിൽ (pos != ശൂന്യ && pos->col == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		എങ്കിൽ (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		അല്ലെങ്കിൽ
		{
		    long lnum;
		    last = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     first.lnum, first.col,
			     last.lnum, last.col));
#ifdef FEAT_SIGNS
		    വേണ്ടി (lnum = first.lnum; lnum <= last.lnum; lnum++)
		    {
			എങ്കിൽ (un)
			{
			    // never used
			}
			അല്ലെങ്കിൽ
			{
			    എങ്കിൽ (buf_findsigntype_id(buf->bufp, lnum,
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
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "startAtomic"))
	{
	    inAtomic = 1;
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "endAtomic"))
	{
	    inAtomic = 0;
	    എങ്കിൽ (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		തിരികെ FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    എങ്കിൽ (bufIsChanged(buf->bufp))
	    {
		// Only write if the buffer can be written.
		എങ്കിൽ (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != ശൂന്യ
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    എങ്കിൽ (!bufref_valid(&bufref))
			buf->bufp = ശൂന്യ;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "netbeansBuffer"))
	{
	    എങ്കിൽ (buf == ശൂന്യ || buf->bufp == ശൂന്യ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (*args == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    അല്ലെങ്കിൽ
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "specialKeys"))
	{
	    special_താക്കോലുകൾ(args);
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	അല്ലെങ്കിൽ എങ്കിൽ (streq((അക്ഷരം *)cmd, "version"))
	{
	    // not used yet
	}
	അല്ലെങ്കിൽ
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    എങ്കിൽ (inAtomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    എങ്കിൽ (buf != ശൂന്യ && buf->initDone && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	out_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	എങ്കിൽ (State == MODE_HITRETURN || State == MODE_ASKMORE)
	{
#ifdef FEAT_GUI_GTK
	    എങ്കിൽ (gui.in_use && gtk_main_level() > 0)
		gtk_main_quit();
#endif
	}
    }

    തിരികെ retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    സ്ഥിതം ശൂന്യം
nb_set_curbuf(buf_T *buf)
{
    എങ്കിൽ (curbuf == buf)
	തിരികെ;

    എങ്കിൽ (buf_jump_open_win(buf) != ശൂന്യ)
	തിരികെ;
    എങ്കിൽ ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != ശൂന്യ)
	തിരികെ;
    set_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    സ്ഥിതം ശൂന്യം
coloncmd(അക്ഷരം *cmd, ...)
{
    അക്ഷരം buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, വലുപ്പം(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, ശൂന്യ, ശൂന്യ, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    out_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    സ്ഥിതം ശൂന്യം
special_താക്കോലുകൾ(char_u *args)
{
    അക്ഷരം *save_str = nb_unquote(args, ശൂന്യ);
    അക്ഷരം *tok = strtok(save_str, " ");
    അക്ഷരം *sep;
#define KEYBUFLEN 64
    അക്ഷരം keybuf[KEYBUFLEN];
    അക്ഷരം cmdbuf[256];

    ആകുമ്പോൾ (tok != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ i = 0;

	എങ്കിൽ ((sep = strchr(tok, '-')) != ശൂന്യ)
	{
	    *sep = NUL;
	    ആകുമ്പോൾ (*tok && i + 2 < KEYBUFLEN)
	    {
		തിരഞ്ഞെടുപ്പ് (*tok)
		{
		    അവസ്ഥ 'A':
		    അവസ്ഥ 'M':
		    അവസ്ഥ 'C':
		    അവസ്ഥ 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			നിർത്തുക;
		}
		tok++;
	    }
	    tok++;
	}

	എങ്കിൽ (strlen(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    പൂർണ്ണസംഖ്യ safe = TRUE;
	    വേണ്ടി (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		എങ്കിൽ (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    safe = FALSE;
		    നിർത്തുക;
		}
	    }
	    എങ്കിൽ (safe)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, വലുപ്പം(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, MODE_NORMAL, FALSE);
	    }
	}
	tok = strtok(ശൂന്യ, " ");
    }
    vim_free(save_str);
}

    ശൂന്യം
ex_nbclose(exarg_T *eap UNUSED)
{
    netbeans_close();
}

    ശൂന്യം
ex_nbkey(exarg_T *eap)
{
    (ശൂന്യം)netbeans_keystring(eap->arg);
}

    ശൂന്യം
ex_nbstart(
    exarg_T	*eap)
{
#ifdef FEAT_GUI
# എങ്കിൽ !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    എങ്കിൽ (gui.in_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	തിരികെ;
    }
# endif
#endif
    netbeans_open((അക്ഷരം *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    സ്ഥിതം ശൂന്യം
nb_init_graphics(ശൂന്യം)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ did_init = FALSE;

    എങ്കിൽ (did_init)
	തിരികെ;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    സ്ഥിതം ശൂന്യം
netbeans_keyname(പൂർണ്ണസംഖ്യ താക്കോൽ, അക്ഷരം *buf)
{
    അക്ഷരം *name = 0;
    അക്ഷരം namebuf[2];
    പൂർണ്ണസംഖ്യ ctrl  = 0;
    പൂർണ്ണസംഖ്യ shift = 0;
    പൂർണ്ണസംഖ്യ alt   = 0;

    എങ്കിൽ (mod_mask & MOD_MASK_CTRL)
	ctrl = 1;
    എങ്കിൽ (mod_mask & MOD_MASK_SHIFT)
	shift = 1;
    എങ്കിൽ (mod_mask & MOD_MASK_ALT)
	alt = 1;


    തിരഞ്ഞെടുപ്പ് (താക്കോൽ)
    {
	അവസ്ഥ K_F1:		name = "F1";		നിർത്തുക;
	അവസ്ഥ K_S_F1:	name = "F1";	shift = 1;	നിർത്തുക;
	അവസ്ഥ K_F2:		name = "F2";		നിർത്തുക;
	അവസ്ഥ K_S_F2:	name = "F2";	shift = 1;	നിർത്തുക;
	അവസ്ഥ K_F3:		name = "F3";		നിർത്തുക;
	അവസ്ഥ K_S_F3:	name = "F3";	shift = 1;	നിർത്തുക;
	അവസ്ഥ K_F4:		name = "F4";		നിർത്തുക;
	അവസ്ഥ K_S_F4:	name = "F4";	shift = 1;	നിർത്തുക;
	അവസ്ഥ K_F5:		name = "F5";		നിർത്തുക;
	അവസ്ഥ K_S_F5:	name = "F5";	shift = 1;	നിർത്തുക;
	അവസ്ഥ K_F6:		name = "F6";		നിർത്തുക;
	അവസ്ഥ K_S_F6:	name = "F6";	shift = 1;	നിർത്തുക;
	അവസ്ഥ K_F7:		name = "F7";		നിർത്തുക;
	അവസ്ഥ K_S_F7:	name = "F7";	shift = 1;	നിർത്തുക;
	അവസ്ഥ K_F8:		name = "F8";		നിർത്തുക;
	അവസ്ഥ K_S_F8:	name = "F8";	shift = 1;	നിർത്തുക;
	അവസ്ഥ K_F9:		name = "F9";		നിർത്തുക;
	അവസ്ഥ K_S_F9:	name = "F9";	shift = 1;	നിർത്തുക;
	അവസ്ഥ K_F10:		name = "F10";		നിർത്തുക;
	അവസ്ഥ K_S_F10:	name = "F10";	shift = 1;	നിർത്തുക;
	അവസ്ഥ K_F11:		name = "F11";		നിർത്തുക;
	അവസ്ഥ K_S_F11:	name = "F11";	shift = 1;	നിർത്തുക;
	അവസ്ഥ K_F12:		name = "F12";		നിർത്തുക;
	അവസ്ഥ K_S_F12:	name = "F12";	shift = 1;	നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
			എങ്കിൽ (താക്കോൽ >= ' ' && താക്കോൽ <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = താക്കോൽ;
			    namebuf[1] = NUL;
			}
			അല്ലെങ്കിൽ
			    name = "X";
			നിർത്തുക;
    }

    buf[0] = '\0';
    എങ്കിൽ (ctrl)
	strcat(buf, "C");
    എങ്കിൽ (shift)
	strcat(buf, "S");
    എങ്കിൽ (alt)
	strcat(buf, "M"); // META
    എങ്കിൽ (ctrl || shift || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#എങ്കിൽ defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    ശൂന്യം
netbeans_beval_cb(
	BalloonEval	*beval,
	പൂർണ്ണസംഖ്യ		 state UNUSED)
{
    win_T	*wp;
    char_u	*text;
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		col;
    അക്ഷരം	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    എങ്കിൽ (!can_use_beval() || !NETBEANS_OPEN)
	തിരികെ;

    എങ്കിൽ (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) != OK)
	തിരികെ;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    എങ്കിൽ (text != ശൂന്യ && text[0] != NUL && STRLEN(text) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    p = nb_quote(text);
	    എങ്കിൽ (p != ശൂന്യ)
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
    പൂർണ്ണസംഖ്യ
netbeans_active(ശൂന്യം)
{
    തിരികെ NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    ശൂന്യം
netbeans_open(അക്ഷരം *params, പൂർണ്ണസംഖ്യ doabort)
{
    അക്ഷരം *cmd = "0:startupDone=0\n";

    എങ്കിൽ (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	തിരികെ;
    }

    എങ്കിൽ (netbeans_connect(params, doabort) != OK)
	തിരികെ;

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
    ശൂന്യം
netbeans_send_disconnect(ശൂന്യം)
{
    അക്ഷരം buf[128];

    എങ്കിൽ (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#എങ്കിൽ defined(FEAT_EVAL)
    പൂർണ്ണസംഖ്യ
set_ref_in_nb_channel(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ abort = FALSE;
    typval_T tv;

    എങ്കിൽ (nb_channel == ശൂന്യ)
	തിരികെ FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = set_ref_in_item(&tv, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    തിരികെ abort;
}
#endif

#എങ്കിൽ defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    ശൂന്യം
netbeans_frame_moved(പൂർണ്ണസംഖ്യ new_x, പൂർണ്ണസംഖ്യ new_y)
{
    അക്ഷരം buf[128];

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (പൂർണ്ണസംഖ്യ)Columns, (പൂർണ്ണസംഖ്യ)Rows, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#endif

/*
 * Tell netbeans the user opened or activated a file.
 */
    ശൂന്യം
netbeans_file_activated(buf_T *bufp)
{
    പൂർണ്ണസംഖ്യ bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    അക്ഷരം    buffer[2*MAXPATHL];
    char_u  *q;

    എങ്കിൽ (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	തിരികെ;

    q = nb_quote(bufp->b_ffname);
    എങ്കിൽ (q == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (bp == ശൂന്യ)
    {
	vim_free(q);
	തിരികെ;
    }

    vim_snprintf(buffer, വലുപ്പം(buffer),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (അക്ഷരം *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    ശൂന്യം
netbeans_file_opened(buf_T *bufp)
{
    പൂർണ്ണസംഖ്യ bufno = nb_getbufno(bufp);
    അക്ഷരം    buffer[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    പൂർണ്ണസംഖ്യ	    bnum;

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    q = nb_quote(bufp->b_ffname);
    എങ്കിൽ (q == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (bp != ശൂന്യ)
	bnum = bufno;
    അല്ലെങ്കിൽ
	bnum = 0;

    vim_snprintf(buffer, വലുപ്പം(buffer), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (അക്ഷരം *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
    എങ്കിൽ (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	last_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    ശൂന്യം
netbeans_file_killed(buf_T *bufp)
{
    പൂർണ്ണസംഖ്യ		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    അക്ഷരം	buffer[2*MAXPATHL];

    എങ്കിൽ (!NETBEANS_OPEN || bufno == -1)
	തിരികെ;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(buffer, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_killed");

    എങ്കിൽ (nbbuf != ശൂന്യ)
	nbbuf->bufp = ശൂന്യ;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    സ്ഥിതം nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, പൂർണ്ണസംഖ്യ *bufnop)
{
    പൂർണ്ണസംഖ്യ		bufno;
    nbbuf_T	*nbbuf;

    എങ്കിൽ (!NETBEANS_OPEN || !netbeansFireChanges)
	തിരികെ ശൂന്യ;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    എങ്കിൽ (bufno <= 0)
	തിരികെ ശൂന്യ;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    എങ്കിൽ (nbbuf != ശൂന്യ && !nbbuf->fireChanges)
	തിരികെ ശൂന്യ;		// changes in this buffer are not reported

    *bufnop = bufno;
    തിരികെ nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    ശൂന്യം
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    char_u	*txt,
    പൂർണ്ണസംഖ്യ		newlen)
{
    char_u	*buf;
    പൂർണ്ണസംഖ്യ		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;
    char_u	*p;
    char_u	*newtxt;

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    എങ്കിൽ (nbbuf == ശൂന്യ)
	തിരികെ;

    // Don't mark as modified for initial read
    എങ്കിൽ (nbbuf->തിരുകുക_Done)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.col = col;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    എങ്കിൽ (p != ശൂന്യ)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((അക്ഷരം *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((അക്ഷരം *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    ശൂന്യം
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    long	len)
{
    char_u	buf[128];
    പൂർണ്ണസംഖ്യ		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    എങ്കിൽ (nbbuf == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	തിരികെ;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.col = col;

    off = pos2off(bufp, &pos);

    sprintf((അക്ഷരം *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((അക്ഷരം *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    ശൂന്യം
netbeans_unmodified(buf_T *bufp UNUSED)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    ശൂന്യം
netbeans_button_release(പൂർണ്ണസംഖ്യ button)
{
    അക്ഷരം	buf[128];
    പൂർണ്ണസംഖ്യ		bufno;

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    bufno = nb_getbufno(curbuf);

    എങ്കിൽ (bufno < 0 || curwin == ശൂന്യ || curwin->w_buffer != curbuf)
	തിരികെ;

    പൂർണ്ണസംഖ്യ col = mouse_col - curwin->w_wincol
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
    പൂർണ്ണസംഖ്യ
netbeans_keycommand(പൂർണ്ണസംഖ്യ താക്കോൽ)
{
    അക്ഷരം	താക്കോൽ_Name[60];

    netbeans_keyname(താക്കോൽ, താക്കോൽ_Name);
    തിരികെ netbeans_keystring((char_u *)താക്കോൽ_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
netbeans_keystring(char_u *താക്കോൽ_Name)
{
    അക്ഷരം	buf[2*MAXPATHL];
    പൂർണ്ണസംഖ്യ		bufno = nb_getbufno(curbuf);
    long	off;
    char_u	*q;

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ TRUE;

    എങ്കിൽ (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == ശൂന്യ ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	എങ്കിൽ (q == ശൂന്യ)
	    തിരികെ TRUE;
	vim_snprintf(buf, വലുപ്പം(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	എങ്കിൽ (curbuf->b_ffname != ശൂന്യ)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(താക്കോൽ_Name);
	തിരികെ FALSE;
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
    vim_snprintf(buf, വലുപ്പം(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, താക്കോൽ_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, വലുപ്പം(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, താക്കോൽ_Name,
		off, (long)curwin->w_cursor.lnum, (long)curwin->w_cursor.col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    തിരികെ TRUE;
}


/*
 * Send a save event to netbeans.
 */
    ശൂന്യം
netbeans_save_buffer(buf_T *bufp)
{
    char_u	buf[64];
    പൂർണ്ണസംഖ്യ		bufno;
    nbbuf_T	*nbbuf;

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    എങ്കിൽ (nbbuf == ശൂന്യ)
	തിരികെ;

    nbbuf->modified = 0;

    sprintf((അക്ഷരം *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((അക്ഷരം *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    ശൂന്യം
netbeans_deleted_all_വരികൾ(buf_T *bufp)
{
    char_u	buf[64];
    പൂർണ്ണസംഖ്യ		bufno;
    nbbuf_T	*nbbuf;

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    എങ്കിൽ (nbbuf == ശൂന്യ)
	തിരികെ;

    // Don't mark as modified for initial read
    എങ്കിൽ (nbbuf->തിരുകുക_Done)
	nbbuf->modified = 1;

    sprintf((അക്ഷരം *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    പൂർണ്ണസംഖ്യ
netbeans_is_guarded(linenr_T മുകൾ, linenr_T bot)
{
    sign_entry_T	*p;
    പൂർണ്ണസംഖ്യ			lnum;

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ FALSE;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
	എങ്കിൽ (p->se_id >= GUARDEDOFFSET)
	    വേണ്ടി (lnum = മുകൾ + 1; lnum < bot; lnum++)
		എങ്കിൽ (lnum == p->se_lnum)
		    തിരികെ TRUE;

    തിരികെ FALSE;
}

#എങ്കിൽ defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    ശൂന്യം
netbeans_draw_multisign_indicator(പൂർണ്ണസംഖ്യ row)
{
    പൂർണ്ണസംഖ്യ i;
    പൂർണ്ണസംഖ്യ y;
    പൂർണ്ണസംഖ്യ x;

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    x = 0;
    y = row * gui.char_height + 2;

    വേണ്ടി (i = 0; i < gui.char_height - 3; i++)
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

#എങ്കിൽ defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    ശൂന്യം
netbeans_draw_multisign_indicator(പൂർണ്ണസംഖ്യ row)
{
    പൂർണ്ണസംഖ്യ i;
    പൂർണ്ണസംഖ്യ y;
    പൂർണ്ണസംഖ്യ x;
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    cairo_t *cr = ശൂന്യ;
# അല്ലെങ്കിൽ
    GdkDrawable *drawable = gui.drawarea->window;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# അല്ലെങ്കിൽ
    y = row * gui.char_height + 2;
# endif

# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_height);
    cr = cairo_create(surf);
#  അല്ലെങ്കിൽ
    cr = cairo_create(gui.surface);
#  endif
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    വേണ്ടി (i = 0; i < gui.char_height - 3; i++)
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# അല്ലെങ്കിൽ
    gdk_draw_point(drawable, gui.text_gc, x+2, y++);
# endif

# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# അല്ലെങ്കിൽ
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

# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
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
    ശൂന്യം
netbeans_gutter_click(linenr_T lnum)
{
    sign_entry_T	*p;

    എങ്കിൽ (!NETBEANS_OPEN)
	തിരികെ;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
    {
	എങ്കിൽ (p->se_lnum == lnum && p->se_next && p->se_next->se_lnum == lnum)
	{
	    sign_entry_T *tail;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    എങ്കിൽ (p->se_prev)
		p->se_prev->se_next = p->se_next;
	    അല്ലെങ്കിൽ
		curbuf->b_signlist = p->se_next;
	    p->se_next->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    വേണ്ടി (tail = p->se_next;
		  tail->se_next && tail->se_next->se_lnum == lnum
				       && tail->se_next->se_id < GUARDEDOFFSET;
		  tail = tail->se_next)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_next = tail->se_next;
	    എങ്കിൽ (tail->se_next)
		tail->se_next->se_prev = p;
	    p->se_prev = tail;
	    tail->se_next = p;
	    update_debug_sign(curbuf, lnum);
	    നിർത്തുക;
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
    സ്ഥിതം ശൂന്യം
addsigntype(
    nbbuf_T	*buf,
    പൂർണ്ണസംഖ്യ		type_അക്കം,
    char_u	*typeName,
    char_u	*tooltip UNUSED,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    പൂർണ്ണസംഖ്യ i, j;
    പൂർണ്ണസംഖ്യ use_fg = (*fg && STRCMP(fg, "none") != 0);
    പൂർണ്ണസംഖ്യ use_bg = (*bg && STRCMP(bg, "none") != 0);

    വേണ്ടി (i = 0; i < globalsignmapused; i++)
	എങ്കിൽ (STRCMP(typeName, globalsignmap[i]) == 0)
	    നിർത്തുക;

    എങ്കിൽ (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_അക്കം, typeName, tooltip, glyphFile, fg, bg));
	എങ്കിൽ (use_fg || use_bg)
	{
	    അക്ഷരം fgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    അക്ഷരം bgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    അക്ഷരം *ptr;
	    പൂർണ്ണസംഖ്യ മൂല്യം;

	    മൂല്യം = strtol((അക്ഷരം *)fg, &ptr, 10);
	    എങ്കിൽ (ptr != (അക്ഷരം *)fg)
		sprintf(fgbuf, "guifg=#%06x", മൂല്യം & 0xFFFFFF);
	    അല്ലെങ്കിൽ
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    മൂല്യം = strtol((അക്ഷരം *)bg, &ptr, 10);
	    എങ്കിൽ (ptr != (അക്ഷരം *)bg)
		sprintf(bgbuf, "guibg=#%06x", മൂല്യം & 0xFFFFFF);
	    അല്ലെങ്കിൽ
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    എങ്കിൽ (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    അല്ലെങ്കിൽ എങ്കിൽ (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    അല്ലെങ്കിൽ
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	അല്ലെങ്കിൽ
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	എങ്കിൽ (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_അക്കം;

	എങ്കിൽ (globalsignmapused == globalsignmaplen)
	{
	    എങ്കിൽ (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(അക്ഷരം *, globalsignmaplen);
	    }
	    അല്ലെങ്കിൽ    // grow it
	    {
		പൂർണ്ണസംഖ്യ incr;
		പൂർണ്ണസംഖ്യ oldlen = globalsignmaplen;
		അക്ഷരം **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * വലുപ്പം(അക്ഷരം *));
		എങ്കിൽ (globalsignmap == ശൂന്യ)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    തിരികെ;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * വലുപ്പം(അക്ഷരം *));
	    }
	}

	globalsignmap[i] = (അക്ഷരം *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    വേണ്ടി (j = 0; j < buf->signmapused; j++)
	എങ്കിൽ (buf->signmap[j] == i + 1)
	    തിരികെ;

    // add to local map
    എങ്കിൽ (buf->signmapused == buf->signmaplen)
    {
	എങ്കിൽ (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(പൂർണ്ണസംഖ്യ, buf->signmaplen);
	}
	അല്ലെങ്കിൽ    // grow it
	{
	    പൂർണ്ണസംഖ്യ incr;
	    പൂർണ്ണസംഖ്യ oldlen = buf->signmaplen;
	    പൂർണ്ണസംഖ്യ *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * വലുപ്പം(പൂർണ്ണസംഖ്യ));
	    എങ്കിൽ (buf->signmap == ശൂന്യ)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		തിരികെ;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * വലുപ്പം(പൂർണ്ണസംഖ്യ));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mapsigntype(nbbuf_T *buf, പൂർണ്ണസംഖ്യ localsigntype)
{
    എങ്കിൽ (--localsigntype >= 0 && localsigntype < buf->signmapused)
	തിരികെ buf->signmap[localsigntype];

    തിരികെ 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    സ്ഥിതം long
get_buf_size(buf_T *bufp)
{
    linenr_T	lnum;
    long	char_എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		eol_size;
    long	last_check = 100000L;

    എങ്കിൽ (bufp->b_ml.ml_flags & ML_EMPTY)
	തിരികെ 0;

    എങ്കിൽ (get_fileformat(bufp) == EOL_DOS)
	eol_size = 2;
    അല്ലെങ്കിൽ
	eol_size = 1;
    വേണ്ടി (lnum = 1; lnum <= bufp->b_ml.ml_വരി_എണ്ണം; ++lnum)
    {
	char_എണ്ണം += ml_get_buf_len(bufp, lnum) + eol_size;
	// Check for a CTRL-C every 100000 characters
	എങ്കിൽ (char_എണ്ണം > last_check)
	{
	    ui_breakcheck();
	    എങ്കിൽ (got_int)
		തിരികെ char_എണ്ണം;
	    last_check = char_എണ്ണം + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    എങ്കിൽ (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_എണ്ണം -= eol_size;

    തിരികെ char_എണ്ണം;
}

/*
 * Convert character offset to lnum,col
 */
    സ്ഥിതം pos_T *
off2pos(buf_T *buf, long offset)
{
    linenr_T	 lnum;
    സ്ഥിതം pos_T pos;

    pos.lnum = 0;
    pos.col = 0;
    pos.coladd = 0;

    എങ്കിൽ (!(buf->b_ml.ml_flags & ML_EMPTY))
    {
	എങ്കിൽ ((lnum = ml_find_വരി_or_offset(buf, (linenr_T)0, &offset)) < 0)
	    തിരികെ ശൂന്യ;
	pos.lnum = lnum;
	pos.col = offset;
    }

    തിരികെ &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    സ്ഥിതം pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    സ്ഥിതം pos_T	mypos;
    long		off;

    off = strtol((അക്ഷരം *)*argp, (അക്ഷരം **)argp, 10);
    എങ്കിൽ (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.col = strtol((അക്ഷരം *)*argp, (അക്ഷരം **)argp, 10);
	mypos.coladd = 0;
	തിരികെ &mypos;
    }
    തിരികെ off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    സ്ഥിതം long
pos2off(buf_T *buf, pos_T *pos)
{
    long	 offset = 0;

    എങ്കിൽ (buf->b_ml.ml_flags & ML_EMPTY)
	തിരികെ 0;

    എങ്കിൽ ((offset = ml_find_വരി_or_offset(buf, pos->lnum, 0)) < 0)
	തിരികെ 0;
    offset += pos->col;

    തിരികെ offset;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    സ്ഥിതം ശൂന്യം
print_read_സന്ദേശം(nbbuf_T *buf)
{
    പൂർണ്ണസംഖ്യ	    lnum = buf->bufp->b_ml.ml_വരി_എണ്ണം;
    off_T   nchars = buf->bufp->b_orig_size;
    char_u  c;

    സന്ദേശം_add_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    എങ്കിൽ (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    എങ്കിൽ (!buf->bufp->b_start_eol)
    {
	STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    സന്ദേശം_add_വരികൾ(c, (long)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_സന്ദേശം);
    സന്ദേശം_scrolled_ign = TRUE;
    സന്ദേശം_trunc_attr((അക്ഷരം *)IObuff, FALSE, 0);
    സന്ദേശം_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    സ്ഥിതം ശൂന്യം
print_save_സന്ദേശം(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    എങ്കിൽ (nchars >= 0)
    {
	// put fname in IObuff with quotes
	സന്ദേശം_add_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	സന്ദേശം_add_വരികൾ(c, buf->bufp->b_ml.ml_വരി_എണ്ണം,
						buf->bufp->b_orig_size);

	VIM_CLEAR(keep_സന്ദേശം);
	സന്ദേശം_scrolled_ign = TRUE;
	p = (char_u *)സന്ദേശം_trunc_attr((അക്ഷരം *)IObuff, FALSE, 0);
	എങ്കിൽ ((സന്ദേശം_scrolled && !need_wait_return) || !buf->initDone)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_സന്ദേശം(p, 0);
	}
	സന്ദേശം_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    അല്ലെങ്കിൽ
    {
	അക്ഷരം msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_add_bang_to_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#endif // defined(FEAT_NETBEANS_INTG)
