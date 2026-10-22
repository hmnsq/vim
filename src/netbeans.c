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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_NETBEANS_INTG)

#ifndef MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#endif

#அந்தர்பாவ "version.h"

#define GUARDED		10000 // typenr for "guarded" annotation
#define GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#define MAX_COLOR_LENGTH 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#define VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#define VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
நிலையான எழுத்து *ExtEdProtocolVersion = "2.5";

நிலையான long pos2off(buf_T *, pos_T *);
நிலையான pos_T *off2pos(buf_T *, long);
நிலையான pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
நிலையான long get_buf_size(buf_T *);
நிலையான முழு netbeans_keystring(char_u *keystr);
நிலையான வெறுமை special_சாவிகள்(char_u *args);

நிலையான முழு getConnInfo(எழுத்து *file, எழுத்து **host, எழுத்து **port, எழுத்து **password);

நிலையான வெறுமை nb_init_graphics(வெறுமை);
நிலையான வெறுமை coloncmd(எழுத்து *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
நிலையான வெறுமை nb_set_curbuf(buf_T *buf);
நிலையான வெறுமை nb_parse_cmd(char_u *);
நிலையான முழு  nb_do_cmd(முழு, char_u *, முழு, முழு, char_u *);
நிலையான வெறுமை nb_send(எழுத்து *buf, எழுத்து *fun);
நிலையான வெறுமை nb_free(வெறுமை);

#define NETBEANS_OPEN (channel_can_write_to(nb_channel))
நிலையான channel_T *nb_channel = வெற்று;

நிலையான முழு r_cmdno;			// current command number for reply
நிலையான முழு dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ifdef NBDEBUG
# include "nbdebug.c"
#endif

நிலையான முழு needupdate = 0;
நிலையான முழு inAtomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    நிலையான முழு
nb_is_safe_string(char_u *str, char_u *allowed)
{
    எனில் (str == வெற்று)
	திரும்பு FALSE;
    க்கு (char_u *p = str; *p != NUL; p++)
	எனில் (vim_strchr(allowed, *p) == வெற்று)
	    திரும்பு FALSE;
    திரும்பு TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    நிலையான வெறுமை
nb_channel_closed(வெறுமை)
{
    nb_channel = வெற்று;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    நிலையான வெறுமை
netbeans_close(வெறுமை)
{
    எனில் (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	எனில் (nb_channel != வெற்று)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = வெற்று;
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

    நிலையான முழு
netbeans_connect(எழுத்து *params, முழு doabort)
{
    முழு		port;
    எழுத்து	buf[32];
    எழுத்து	*hostname = வெற்று;
    எழுத்து	*address = வெற்று;
    எழுத்து	*password = வெற்று;
    எழுத்து	*fname;
    எழுத்து	*arg = வெற்று;

    எனில் (*params == '=')
    {
	// "=fname": Read info from specified file.
	எனில் (getConnInfo(params + 1, &hostname, &address, &password) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	எனில் (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	எனில் (arg == வெற்று && (fname = getenv("__NETBEANS_CONINFO")) != வெற்று)
	{
	    // "": get info from file specified in environment
	    எனில் (getConnInfo(fname, &hostname, &address, &password) == FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில்
	{
	    எனில் (arg != வெற்று)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		address = strchr(hostname, ':');
		எனில் (address != வெற்று)
		{
		    *address++ = '\0';
		    password = strchr(address, ':');
		    எனில் (password != வெற்று)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    எனில் (hostname == வெற்று || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    எனில் (address == வெற்று)
		address = getenv("__NETBEANS_SOCKET");
	    எனில் (password == வெற்று)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    எனில் (hostname != வெற்று)
		hostname = (எழுத்து *)vim_strsave((char_u *)hostname);
	    எனில் (address != வெற்று)
		address = (எழுத்து *)vim_strsave((char_u *)address);
	    எனில் (password != வெற்று)
		password = (எழுத்து *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    எனில் (hostname == வெற்று || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (எழுத்து *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    எனில் (address == வெற்று || *address == '\0')
    {
	vim_free(address);
	address = (எழுத்து *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    எனில் (password == வெற்று || *password == '\0')
    {
	vim_free(password);
	password = (எழுத்து *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    எனில் (hostname != வெற்று && address != வெற்று && password != வெற்று)
    {
	port = atoi(address);
	nb_channel = channel_open(hostname, port, 3000, nb_channel_closed);
	எனில் (nb_channel != வெற்று)
	{
	    // success
#ifdef FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#endif

	    // success, login
	    vim_snprintf(buf, அளவு(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", ExtEdProtocolVersion);
	    nb_send(buf, "externaleditor_version");
	}
    }

    எனில் (nb_channel == வெற்று && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(address);
    vim_free(password);
    திரும்பு NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    நிலையான முழு
getConnInfo(எழுத்து *file, எழுத்து **host, எழுத்து **port, எழுத்து **auth)
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
    எனில் (mch_stat(file, &st) == 0 && (st.st_mode & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_mode_for_netbeans_connection_info_file_str),
									file);
	திரும்பு FAIL;
    }
#endif

    fp = mch_fopen(file, "r");
    எனில் (fp == வெற்று)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	திரும்பு FAIL;
    }

    // Read the file. There should be one of each parameter
    போது ((lp = (char_u *)fgets((எழுத்து *)buf, BUFSIZ, fp)) != வெற்று)
    {
	எனில் ((nlp = vim_strchr(lp, '\n')) != வெற்று)
	    *nlp = 0;	    // strip off the trailing newline

	எனில் (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (எழுத்து *)vim_strsave(&buf[5]);
	}
	இல்லையெனில் எனில் (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*port);
	    *port = (எழுத்து *)vim_strsave(&buf[5]);
	}
	இல்லையெனில் எனில் (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (எழுத்து *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    திரும்பு OK;
}


கட்டமைப்பு keyqueue
{
    char_u	    *keystr;
    கட்டமைப்பு keyqueue *next;
    கட்டமைப்பு keyqueue *prev;
};

வகைவரை கட்டமைப்பு keyqueue சாவி_Q_T;

நிலையான சாவி_Q_T சாவி_Head; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    நிலையான வெறுமை
postpone_keycommand(char_u *keystr)
{
    சாவி_Q_T *முனை;

    முனை = ALLOC_ONE(சாவி_Q_T);
    எனில் (முனை == வெற்று)
	திரும்பு;  // out of memory, drop the key

    எனில் (சாவி_Head.next == வெற்று) // initialize circular queue
    {
	சாவி_Head.next = &சாவி_Head;
	சாவி_Head.prev = &சாவி_Head;
    }

    // insert node at tail of queue
    முனை->next = &சாவி_Head;
    முனை->prev = சாவி_Head.prev;
    சாவி_Head.prev->next = முனை;
    சாவி_Head.prev = முனை;

    முனை->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    நிலையான வெறுமை
handle_சாவி_queue(வெறுமை)
{
    முழு postponed = FALSE;

    போது (!postponed && சாவி_Head.next && சாவி_Head.next != &சாவி_Head)
    {
	// first, unlink the node
	சாவி_Q_T *முனை = சாவி_Head.next;
	சாவி_Head.next = முனை->next;
	முனை->next->prev = முனை->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	எனில் (முனை->keystr != வெற்று)
	    postponed = !netbeans_keystring(முனை->keystr);
	vim_free(முனை->keystr);

	// Finally, dispose of the node
	vim_free(முனை);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    வெறுமை
netbeans_parse_messages(வெறுமை)
{
    readq_T	*முனை;
    char_u	*buffer;
    char_u	*p;
    முழு		own_முனை;

    போது (nb_channel != வெற்று)
    {
	முனை = channel_peek(nb_channel, PART_SOCK);
	எனில் (முனை == வெற்று)
	    நிறுத்து;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_first_nl(முனை);
	எனில் (p == வெற்று)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    எனில் (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		திரும்பு;
	    தொடர்;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	எனில் (*p == NUL)
	{
	    own_முனை = TRUE;
	    buffer = channel_get(nb_channel, PART_SOCK, வெற்று);
	    // "node" is now invalid!
	}
	இல்லையெனில்
	{
	    own_முனை = FALSE;
	    buffer = முனை->rq_buffer;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(buffer);

	எனில் (own_முனை)
	    // buffer finished, dispose of it
	    vim_free(buffer);
	இல்லையெனில் எனில் (nb_channel != வெற்று)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (முழு)(p - buffer));
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
    நிலையான வெறுமை
nb_parse_cmd(char_u *cmd)
{
    எழுத்து	*verb;
    எழுத்து	*q;
    முழு		bufno;
    முழு		isfunc = -1;

    எனில் (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    எனில் (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_column = false;

	// The IDE is breaking the connection.
	netbeans_close();
	திரும்பு;
    }

    bufno = strtol((எழுத்து *)cmd, &verb, 10);

    எனில் (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	திரும்பு;
    }
    ++verb; // skip colon

    க்கு (q = verb; *q; q++)
    {
	எனில் (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    நிறுத்து;
	}
	இல்லையெனில் எனில் (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    நிறுத்து;
	}
    }

    எனில் (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_in_str), cmd);
	திரும்பு;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (எழுத்து *)skipwhite((char_u *)q);

    எனில் (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
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

கட்டமைப்பு nbbuf_struct
{
    buf_T		*bufp;
    unsigned முழு	 fireChanges:1;
    unsigned முழு	 initDone:1;
    unsigned முழு	 செருகு_Done:1;
    unsigned முழு	 modified:1;
    முழு			 nbbuf_எண்;
    எழுத்து		*displayname;
    முழு			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

வகைவரை கட்டமைப்பு nbbuf_struct nbbuf_T;

நிலையான nbbuf_T *buf_list = வெற்று;
நிலையான முழு buf_list_size = 0;	// size of buf_list
நிலையான முழு buf_list_used = 0;	// nr of entries in buf_list actually in use

நிலையான எழுத்து **globalsignmap = வெற்று;
நிலையான முழு globalsignmaplen = 0;
நிலையான முழு globalsignmapused = 0;

நிலையான முழு  mapsigntype(nbbuf_T *, முழு localsigntype);
நிலையான வெறுமை addsigntype(nbbuf_T *, முழு localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
நிலையான வெறுமை print_read_செய்தி(nbbuf_T *buf);
நிலையான வெறுமை print_save_செய்தி(nbbuf_T *buf, off_T nchars);

நிலையான முழு curPCtype = -1;

/*
 * Free netbeans resources.
 */
    நிலையான வெறுமை
nb_free(வெறுமை)
{
    சாவி_Q_T *சாவி_முனை = சாவி_Head.next;
    nbbuf_T buf;
    முழு i;

    // free the netbeans buffer list
    க்கு (i = 0; i < buf_list_used; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	எனில் (buf.bufp != வெற்று && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_size = 0;
    buf_list_used = 0;

    // free the queued key commands
    போது (சாவி_முனை != வெற்று && சாவி_முனை != &சாவி_Head)
    {
	சாவி_Q_T *next = சாவி_முனை->next;
	vim_free(சாவி_முனை->keystr);
	vim_free(சாவி_முனை);
	எனில் (next == &சாவி_Head)
	{
	    சாவி_Head.next = &சாவி_Head;
	    சாவி_Head.prev = &சாவி_Head;
	    நிறுத்து;
	}
	சாவி_முனை = next;
    }

    // free the queued netbeans commands
    எனில் (nb_channel != வெற்று)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    நிலையான முழு
nb_getbufno(buf_T *bufp)
{
    முழு i;

    க்கு (i = 0; i < buf_list_used; i++)
	எனில் (buf_list[i].bufp == bufp)
	    திரும்பு i;
    திரும்பு -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    முழு
isNetbeansBuffer(buf_T *bufp)
{
    திரும்பு NETBEANS_OPEN && bufp->b_netbeans_file;
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
    முழு
isNetbeansModified(buf_T *bufp)
{
    எனில் (isNetbeansBuffer(bufp))
    {
	முழு bufno = nb_getbufno(bufp);

	எனில் (bufno > 0)
	    திரும்பு buf_list[bufno].modified;
	இல்லையெனில்
	    திரும்பு FALSE;
    }
    இல்லையெனில்
	திரும்பு FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    நிலையான nbbuf_T *
nb_get_buf(முழு bufno)
{
    // find or create a buffer with the given number
    முழு incr;

    எனில் (bufno <= 0)
	திரும்பு வெற்று;

    எனில் (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * அளவு(nbbuf_T));
	buf_list_size = 100;
    }
    எனில் (bufno >= buf_list_used) // new
    {
	எனில் (bufno >= buf_list_size) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_size + 90;
	    buf_list_size += incr;
	    bufsize = buf_list_size * அளவு(nbbuf_T);
	    எனில் (bufsize == 0 || bufsize / அளவு(nbbuf_T)
						      != (size_t)buf_list_size)
	    {
		// list size overflow, bail out
		திரும்பு வெற்று;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    எனில் (buf_list == வெற்று)
	    {
		vim_free(t_buf_list);
		buf_list_size = 0;
		திரும்பு வெற்று;
	    }
	    vim_memset(buf_list + buf_list_size - incr, 0,
						      incr * அளவு(nbbuf_T));
	}

	போது (buf_list_used <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_used].fireChanges = 1;
	    ++buf_list_used;
	}
    }

    திரும்பு buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    நிலையான முழு
மொத்தம்_changed_buffers(வெறுமை)
{
    buf_T	*bufp;
    முழு		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	எனில் (bufp->b_changed)
	    ++n;
    திரும்பு n;
}

/*
 * End the netbeans session.
 */
    வெறுமை
netbeans_end(வெறுமை)
{
    முழு i;
    நிலையான எழுத்து buf[128];

    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    க்கு (i = 0; i < buf_list_used; i++)
    {
	எனில் (!buf_list[i].bufp)
	    தொடர்;
	எனில் (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, வெற்று);
	buf_list[i].bufp = வெற்று;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    நிலையான வெறுமை
nb_send(எழுத்து *buf, எழுத்து *fun)
{
    எனில் (nb_channel != வெற்று)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (முழு)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    நிலையான வெறுமை
nb_reply_nil(முழு cmdno)
{
    எழுத்து reply[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    sprintf(reply, "%d\n", cmdno);
    nb_send(reply, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    நிலையான வெறுமை
nb_reply_text(முழு cmdno, char_u *result)
{
    char_u *reply;

    nbdebug(("REP %d: %s\n", cmdno, (எழுத்து *)result));

    reply = alloc(STRLEN(result) + 32);
    sprintf((எழுத்து *)reply, "%d %s\n", cmdno, (எழுத்து *)result);
    nb_send((எழுத்து *)reply, "nb_reply_text");

    vim_free(reply);
}


/*
 * Send a response with a number result code.
 */
    நிலையான வெறுமை
nb_reply_nr(முழு cmdno, long result)
{
    எழுத்து reply[32];

    nbdebug(("REP %d: %ld\n", cmdno, result));

    sprintf(reply, "%d %ld\n", cmdno, result);
    nb_send(reply, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    நிலையான char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    எனில் (buf == வெற்று)
	திரும்பு வெற்று;
    க்கு (; *p; p++)
    {
	தேர்வு (*p)
	{
	    நிலை '\"':
	    நிலை '\\':
		*q++ = '\\'; *q++ = *p; நிறுத்து;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    நிலை '\n':
		*q++ = '\\'; *q++ = 'n'; நிறுத்து;
	    நிலை '\r':
		*q++ = '\\'; *q++ = 'r'; நிறுத்து;
	    இயல்பு:
		*q++ = *p;
		நிறுத்து;
	}
    }
    *q = '\0';

    திரும்பு buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    நிலையான எழுத்து *
nb_unquote(char_u *p, char_u **endp)
{
    எழுத்து *result = 0;
    எழுத்து *q;
    முழு done = 0;

    // result is never longer than input
    result = alloc_clear(STRLEN(p) + 1);
    எனில் (result == வெற்று)
	திரும்பு வெற்று;

    எனில் (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	result[0] = NUL;
	திரும்பு result;
    }

    க்கு (q = result; !done && *p != NUL;)
    {
	தேர்வு (*p)
	{
	    நிலை '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		done = 1;
		நிறுத்து;

	    நிலை '\\':
		++p;
		தேர்வு (*p)
		{
		    நிலை '\\':	*q++ = '\\';	நிறுத்து;
		    நிலை 'n':	*q++ = '\n';	நிறுத்து;
		    நிலை 't':	*q++ = '\t';	நிறுத்து;
		    நிலை 'r':	*q++ = '\r';	நிறுத்து;
		    நிலை '"':	*q++ = '"';	நிறுத்து;
		    நிலை NUL:	--p;		நிறுத்து;
		    // default: skip over illegal chars
		}
		++p;
		நிறுத்து;

	    இயல்பு:
		*q++ = *p++;
	}
    }

    எனில் (endp != வெற்று)
	*endp = p;

    திரும்பு result;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    நிலையான வெறுமை
nb_partialremove(linenr_T lnum, colnr_T first, colnr_T last)
{
    char_u *oldtext, *newtext;
    முழு oldlen;
    முழு lastbyte = last;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    எனில் (first >= (colnr_T)oldlen || oldlen == 0)  // just in case
	திரும்பு;
    எனில் (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - first));
    எனில் (newtext == வெற்று)
	திரும்பு;

    mch_memmove(newtext, oldtext, first);
    STRMOVE(newtext + first, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    நிலையான வெறுமை
nb_joinlines(linenr_T first, linenr_T other)
{
    முழு len_first, len_other;
    char_u *p;

    len_first = ml_get_len(first);
    len_other = ml_get_len(other);
    p = alloc(len_first + len_other + 1);
    எனில் (p == வெற்று)
	திரும்பு;

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
    நிலையான முழு
nb_do_cmd(
    முழு		bufno,
    char_u	*cmd,
    முழு		func,
    முழு		cmdno,
    char_u	*args)	    // points to space before arguments or NUL
{
    முழு		do_update = 0;
    long	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    நிலையான முழு	skip = 0;
    முழு		retval = OK;
    எழுத்து	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (எழுத்து *)args));

    எனில் (func)
    {
// =====================================================================
	எனில் (streq((எழுத்து *)cmd, "getModified"))
	{
	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
		// Return the number of buffers that are modified.
		nb_reply_nr(cmdno, (long)மொத்தம்_changed_buffers());
	    இல்லையெனில்
		// Return whether the buffer is modified.
		nb_reply_nr(cmdno, (long)(buf->bufp->b_changed
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_reply_nr(cmdno, (long)மொத்தம்_changed_buffers());
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "getCursor"))
	{
	    char_u text[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((எழுத்து *)text, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (long)curwin->w_cursor.lnum,
		    (முழு)curwin->w_cursor.col,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_reply_text(cmdno, text);
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "getAnno"))
	{
	    long linenum = 0;
#ifdef FEAT_SIGNS
	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_getanno));
		retval = FAIL;
	    }
	    இல்லையெனில்
	    {
		முழு ser_இலக்கம்;

		cp = (எழுத்து *)args;
		ser_இலக்கம் = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (long)buf_findsign(buf->bufp, ser_இலக்கம், வெற்று);
	    }
#endif
	    nb_reply_nr(cmdno, linenum);
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "getLength"))
	{
	    long len = 0;

	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_buffer_identifier_in_getlength));
		retval = FAIL;
	    }
	    இல்லையெனில்
	    {
		len = get_buf_size(buf->bufp);
	    }
	    nb_reply_nr(cmdno, len);
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "getText"))
	{
	    long	len;
	    linenr_T	nlines;
	    char_u	*text = வெற்று;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*வரி;

	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_buffer_identifier_in_gettext));
		retval = FAIL;
	    }
	    இல்லையெனில்
	    {
		len = get_buf_size(buf->bufp);
		nlines = buf->bufp->b_ml.ml_வரி_மொத்தம்;
		text = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		எனில் (text == வெற்று)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		இல்லையெனில்
		{
		    p = text;
		    *p++ = '\"';
		    க்கு (; lno <= nlines ; lno++)
		    {
			வரி = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			எனில் (வரி != வெற்று)
			{
			    STRCPY(p, வரி);
			    p += STRLEN(வரி);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(வரி);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    எனில் (text == வெற்று)
		nb_reply_text(cmdno, (char_u *)"");
	    இல்லையெனில்
	    {
		nb_reply_text(cmdno, text);
		vim_free(text);
	    }
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "remove"))
	{
	    long மொத்தம்;
	    pos_T first, last;
	    pos_T *pos;
	    pos_T *next;
	    linenr_T del_from_lnum, del_to_lnum;  // lines to be deleted as a whole
	    முழு oldFire = netbeansFireChanges;
	    முழு oldSuppress = netbeans_Suppress_No_வரிகள்;
	    முழு wasChanged;

	    எனில் (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (எழுத்து *) cmd));
		nb_reply_nil(cmdno);
		திரும்பு OK;
	    }

	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_buffer_identifier_in_remove));
		retval = FAIL;
	    }
	    இல்லையெனில்
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_வரிகள் = TRUE;

		nb_set_curbuf(buf->bufp);
		wasChanged = buf->bufp->b_changed;
		cp = (எழுத்து *)args;
		off = strtol(cp, &cp, 10);
		மொத்தம் = strtol(cp, &cp, 10);
		args = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		எனில் (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_வரிகள் = oldSuppress;
		    திரும்பு FAIL;
		}
		first = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      first.lnum, first.col));
		pos = off2pos(buf->bufp, off+மொத்தம்-1);
		எனில் (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_வரிகள் = oldSuppress;
		    திரும்பு FAIL;
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
		next = off2pos(buf->bufp, off + மொத்தம்);

		// Remove part of the first line.
		எனில் (first.col != 0
				|| (next != வெற்று && first.lnum == next->lnum))
		{
		    எனில் (first.lnum != last.lnum
			    || (next != வெற்று && first.lnum != next->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(first.lnum, first.col,
							     (colnr_T)MAXCOL);
			எனில் (first.lnum == last.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(first.lnum, next->lnum);
			    del_to_lnum = next->lnum;
			}
		    }
		    இல்லையெனில்
		    {
			// remove within one line
			nb_partialremove(first.lnum, first.col, last.col);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		எனில் (first.lnum != last.lnum && next != வெற்று
			&& next->col != 0 && last.lnum == next->lnum)
		{
		    nb_partialremove(last.lnum, 0, last.col);
		    எனில் (del_from_lnum > first.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(first.lnum, last.lnum);
		    }
		    இல்லையெனில்
			// First line is deleted as a whole, keep the last
			// line.
			--del_to_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		எனில் (first.lnum != last.lnum && del_from_lnum > first.lnum
			&& next != வெற்று && last.lnum != next->lnum)
		{
		    nb_joinlines(first.lnum, next->lnum);
		    del_to_lnum = next->lnum;
		}

		// Delete whole lines if there are any.
		எனில் (del_to_lnum >= del_from_lnum)
		{
		    முழு i;

		    // delete signs from the lines being deleted
		    க்கு (i = del_from_lnum; i <= del_to_lnum; i++)
		    {
			முழு id = buf_findsign_id(buf->bufp, (linenr_T)i, வெற்று);
			எனில் (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, வெற்று);
			}
			இல்லையெனில்
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_to_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.col = 0;
		    del_வரிகள்(del_to_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = first;
		check_cursor_lnum();
		buf->bufp->b_changed = wasChanged; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_வரிகள் = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_reply_nil(cmdno);
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "insert"))
	{
	    char_u	*to_free;

	    எனில் (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (எழுத்து *) cmd));
		nb_reply_nil(cmdno);
		திரும்பு OK;
	    }

	    // get offset
	    cp = (எழுத்து *)args;
	    off = strtol(cp, &cp, 10);
	    args = (char_u *)cp;

	    // get text to be inserted
	    args = skipwhite(args);
	    args = to_free = (char_u *)nb_unquote(args, வெற்று);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_buffer_identifier_in_செருகு));
		retval = FAIL;
	    }
	    இல்லையெனில் எனில் (args != வெற்று)
	    {
		முழு	ff_detected = EOL_UNKNOWN;
		முழு	buf_was_empty = (buf->bufp->b_ml.ml_flags & ML_EMPTY);
		size_t	len = 0;
		முழு	added = 0;
		முழு	oldFire = netbeansFireChanges;
		முழு	old_b_changed;
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
		எனில் (pos != வெற்று)
		{
		    எனில் (pos->lnum <= 0)
			lnum_start = 1;
		    இல்லையெனில்
			lnum_start = pos->lnum;
		}
		இல்லையெனில்
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    எனில் (buf_was_empty)
			lnum_start = 1;	    // above empty line
		    இல்லையெனில்
			lnum_start = curbuf->b_ml.ml_வரி_மொத்தம் + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_start;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		போது (*args != NUL)
		{
		    nlp = vim_strchr(args, '\n');
		    எனில் (nlp == வெற்று)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(args);
		    }
		    இல்லையெனில்
		    {
			len = nlp - args;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			எனில் (nlp > args && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			இல்லையெனில்
			    ff_detected = EOL_UNIX;
		    }
		    args[len] = NUL;

		    எனில் (lnum == lnum_start
			    && ((pos != வெற்று && pos->col > 0)
				|| (lnum == 1 && buf_was_empty)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			முழு	col = pos == வெற்று ? 0 : pos->col;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			எனில் (newline != வெற்று)
			{
			    mch_memmove(newline, oldline, (size_t)col);
			    newline[col] = NUL;
			    STRCAT(newline, args);
			    STRCAT(newline, oldline + col);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    இல்லையெனில்
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), args,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    எனில் (nlp == வெற்று)
			நிறுத்து;
		    ++lnum;
		    args = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_வரிகள்_mark(lnum_start - 1, (long)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		எனில் (buf_was_empty)
		{
		    எனில் (ff_detected == EOL_UNKNOWN)
#எனில் defined(MSWIN)
			ff_detected = EOL_DOS;
#இல்லையெனில்
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
	இல்லையெனில்
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_reply_nil(cmdno);
	    retval = FAIL;
	}
    }
    இல்லையெனில் // Not a function; no reply required.
    {
// =====================================================================
	எனில் (streq((எழுத்து *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    எனில் (buf == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_buffer_identifier_in_create));
		திரும்பு FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, வெற்று, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->செருகு_Done = FALSE;
#எனில் defined(FEAT_MENU) && defined(FEAT_GUI)
	    எனில் (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "insertDone"))
	{
	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    இல்லையெனில்
	    {
		buf->bufp->b_start_eol = *args == 'T';
		buf->செருகு_Done = TRUE;
		args += 2;
		buf->bufp->b_p_ro = *args == 'T';
		print_read_செய்தி(buf);
	    }
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "saveDone"))
	{
	    long savedChars = atol((எழுத்து *)args);

	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    இல்லையெனில்
		print_save_செய்தி(buf, savedChars);
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "startDocumentListen"))
	{
	    எனில் (buf == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_startdocumentlisten));
		திரும்பு FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "stopDocumentListen"))
	{
	    எனில் (buf == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_stopdocumentlisten));
		திரும்பு FAIL;
	    }
	    buf->fireChanges = 0;
	    எனில் (buf->bufp != வெற்று && buf->bufp->b_was_netbeans_file)
	    {
		எனில் (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_lost_for_buffer_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_lost_for_buffer_nr),
							   buf->bufp->b_fnum);
		}
		இல்லையெனில்
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
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "setTitle"))
	{
	    எனில் (buf == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_buffer_identifier_in_settitle));
		திரும்பு FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, வெற்று);
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "initDone"))
	{
	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_buffer_identifier_in_initdone));
		திரும்பு FAIL;
	    }
	    do_update = 1;
	    buf->initDone = TRUE;
	    nb_set_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_சாவி_queue();
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "setBufferNumber")
		|| streq((எழுத்து *)cmd, "putBufferNumber"))
	{
	    char_u	*path;
	    buf_T	*bufp;

	    எனில் (buf == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_buffer_identifier_in_setbuffernumber));
		திரும்பு FAIL;
	    }
	    path = (char_u *)nb_unquote(args, வெற்று);
	    எனில் (path == வெற்று)
		திரும்பு FAIL;
	    bufp = buflist_findname(path);
	    vim_free(path);
	    எனில் (bufp == வெற்று)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", args));
		semsg(_(e_file_str_not_found_in_setbuffernumber), args);
		திரும்பு FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_எண் = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    எனில் (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    இல்லையெனில்
	    {
		buf->initDone = TRUE;

		// handle any postponed key commands
		handle_சாவி_queue();
	    }

// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "setFullName"))
	{
	    எனில் (buf == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_buffer_identifier_in_setfullname));
		திரும்பு FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, வெற்று);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#எனில் defined(FEAT_MENU) && defined(FEAT_GUI)
	    எனில் (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "editFile"))
	{
	    எனில் (buf == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_buffer_identifier_in_editfile));
		திரும்பு FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, வெற்று);
	    do_ecmd(0, (char_u *)buf->displayname, வெற்று, வெற்று, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->initDone = TRUE;
	    do_update = 1;
	    maketitle();
#எனில் defined(FEAT_MENU) && defined(FEAT_GUI)
	    எனில் (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "setVisible"))
	{
	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		எனில் (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setvisible));
		திரும்பு FAIL;
	    }
	    எனில் (streq((எழுத்து *)args, "T") && buf->bufp != curbuf)
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
		எனில் (gui.in_use)
		    gui_mch_set_foreground();
#endif
	    }
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "raise"))
	{
#ifdef FEAT_GUI
	    // Bring gvim to the foreground.
	    எனில் (gui.in_use)
		gui_mch_set_foreground();
#endif
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "setModified"))
	{
	    முழு prev_b_changed;

	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		எனில் (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setmodified));
		திரும்பு FAIL;
	    }
	    prev_b_changed = buf->bufp->b_changed;
	    எனில் (streq((எழுத்து *)args, "T"))
		buf->bufp->b_changed = TRUE;
	    இல்லையெனில்
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		எனில் (buf->bufp->b_ffname != வெற்று
			&& mch_stat((எழுத்து *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_time(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_changed = FALSE;
	    }
	    buf->modified = buf->bufp->b_changed;
	    எனில் (prev_b_changed != buf->bufp->b_changed)
	    {
		check_status(buf->bufp);
		redraw_tabline = TRUE;
#எனில் defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "setModtime"))
	{
	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    இல்லையெனில்
	    {
		buf->bufp->b_mtime = atoi((எழுத்து *)args);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "setReadOnly"))
	{
	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    இல்லையெனில் எனில் (streq((எழுத்து *)args, "T"))
		buf->bufp->b_p_ro = TRUE;
	    இல்லையெனில்
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "showBalloon"))
	{
#எனில் defined(FEAT_BEVAL_GUI)
	    நிலையான எழுத்து	*text = வெற்று;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    எனில் (balloonEval != வெற்று)
	    {
		vim_free(text);
		text = nb_unquote(args, வெற்று);
		எனில் (text != வெற்று)
		    gui_mch_post_balloon(balloonEval, (char_u *)text);
	    }
#endif
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "setDot"))
	{
	    pos_T *pos;
#ifdef NBDEBUG
	    char_u *s;
#endif

	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_buffer_identifier_in_setdot));
		திரும்பு FAIL;
	    }

	    nb_set_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    எனில் (VIsual_active)
		end_visual_mode();
#ifdef NBDEBUG
	    s = args;
#endif
	    pos = get_off_or_lnum(buf->bufp, &args);
	    எனில் (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#ifdef FEAT_FOLDING
		foldOpenCursor();
#endif
	    }
	    இல்லையெனில்
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
	    எனில் (State == MODE_HITRETURN || State == MODE_ASKMORE)
	    {
#ifdef FEAT_GUI_GTK
		எனில் (gui.in_use && gtk_main_level() > 0)
		    gtk_main_quit();
#endif
	    }
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "close"))
	{
#ifdef NBDEBUG
	    எழுத்து *name = "<NONE>";
#endif

	    எனில் (buf == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_buffer_identifier_in_close));
		திரும்பு FAIL;
	    }

#ifdef NBDEBUG
	    எனில் (buf->displayname != வெற்று)
		name = buf->displayname;
#endif
	    எனில் (buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		எனில் (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    எனில் (buf->bufp != வெற்று)
		do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = வெற்று;
	    buf->initDone = FALSE;
	    do_update = 1;
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "defineAnnoType"))
	{
#ifdef FEAT_SIGNS
	    முழு type_இலக்கம்;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    முழு parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    எனில் (buf == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_buffer_identifier_in_defineannotype));
		திரும்பு FAIL;
	    }

	    cp = (எழுத்து *)args;
	    type_இலக்கம் = strtol(cp, &cp, 10);
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
	    எனில் (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(args);
	    bg = vim_strsave(p);
	    எனில் (STRLEN(fg) > MAX_COLOR_LENGTH || STRLEN(bg) > MAX_COLOR_LENGTH)
	    {
		emsg(_(e_highlighting_color_name_too_long_in_defineAnnoType));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    இல்லையெனில் எனில் (!nb_is_safe_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_safe_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_safe_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_in_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    இல்லையெனில் எனில் (typeName != வெற்று && tooltip != வெற்று && glyphFile != வெற்று)
		addsigntype(buf, type_இலக்கம், typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    எனில் (parse_error)
		திரும்பு FAIL;

#endif
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "addAnno"))
	{
#ifdef FEAT_SIGNS
	    முழு ser_இலக்கம்;
	    முழு local_Type_இலக்கம்;
	    முழு type_இலக்கம்;
	    pos_T *pos;

	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_addanno));
		திரும்பு FAIL;
	    }

	    do_update = 1;

	    cp = (எழுத்து *)args;
	    ser_இலக்கம் = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_இலக்கம் = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    type_இலக்கம் = mapsigntype(buf, local_Type_இலக்கம்);

	    pos = get_off_or_lnum(buf->bufp, &args);

	    cp = (எழுத்து *)args;
	    vim_ignored = (முழு)strtol(cp, &cp, 10);
	    args = (char_u *)cp;
# ifdef NBDEBUG
	    எனில் (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    எனில் (ser_இலக்கம் >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		திரும்பு FAIL;
	    }
	    எனில் (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_இலக்கம், pos->lnum, type_இலக்கம், buf->bufp->b_fnum);
		எனில் (type_இலக்கம் == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_இலக்கம்,
						       buf->bufp->b_fnum);
	    }
#endif
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "removeAnno"))
	{
#ifdef FEAT_SIGNS
	    முழு ser_இலக்கம்;

	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		திரும்பு FAIL;
	    }
	    do_update = 1;
	    cp = (எழுத்து *)args;
	    ser_இலக்கம் = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_இலக்கம், buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#endif
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "moveAnnoToFront"))
	{
#ifdef FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#endif
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "guard") || streq((எழுத்து *)cmd, "unguard"))
	{
	    முழு len;
	    pos_T first;
	    pos_T last;
	    pos_T *pos;
	    முழு un = (cmd[0] == 'u');
	    நிலையான முழு guardId = GUARDEDOFFSET;

	    எனில் (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (எழுத்து *) cmd));
		திரும்பு OK;
	    }

	    nb_init_graphics();

	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		திரும்பு FAIL;
	    }
	    nb_set_curbuf(buf->bufp);
	    cp = (எழுத்து *)args;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, வெற்று, 10);
	    args = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    எனில் (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    இல்லையெனில்
	    {
		first = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		எனில் (pos != வெற்று && pos->col == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		எனில் (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		இல்லையெனில்
		{
		    long lnum;
		    last = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     first.lnum, first.col,
			     last.lnum, last.col));
#ifdef FEAT_SIGNS
		    க்கு (lnum = first.lnum; lnum <= last.lnum; lnum++)
		    {
			எனில் (un)
			{
			    // never used
			}
			இல்லையெனில்
			{
			    எனில் (buf_findsigntype_id(buf->bufp, lnum,
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
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "startAtomic"))
	{
	    inAtomic = 1;
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "endAtomic"))
	{
	    inAtomic = 0;
	    எனில் (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		திரும்பு FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    எனில் (bufIsChanged(buf->bufp))
	    {
		// Only write if the buffer can be written.
		எனில் (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != வெற்று
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    எனில் (!bufref_valid(&bufref))
			buf->bufp = வெற்று;
		}
	    }
	    இல்லையெனில்
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "netbeansBuffer"))
	{
	    எனில் (buf == வெற்று || buf->bufp == வெற்று)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		திரும்பு FAIL;
	    }
	    எனில் (*args == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    இல்லையெனில்
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "specialKeys"))
	{
	    special_சாவிகள்(args);
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	இல்லையெனில் எனில் (streq((எழுத்து *)cmd, "version"))
	{
	    // not used yet
	}
	இல்லையெனில்
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    எனில் (inAtomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    எனில் (buf != வெற்று && buf->initDone && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	out_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	எனில் (State == MODE_HITRETURN || State == MODE_ASKMORE)
	{
#ifdef FEAT_GUI_GTK
	    எனில் (gui.in_use && gtk_main_level() > 0)
		gtk_main_quit();
#endif
	}
    }

    திரும்பு retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    நிலையான வெறுமை
nb_set_curbuf(buf_T *buf)
{
    எனில் (curbuf == buf)
	திரும்பு;

    எனில் (buf_jump_open_win(buf) != வெற்று)
	திரும்பு;
    எனில் ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != வெற்று)
	திரும்பு;
    set_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    நிலையான வெறுமை
coloncmd(எழுத்து *cmd, ...)
{
    எழுத்து buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, அளவு(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, வெற்று, வெற்று, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    out_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    நிலையான வெறுமை
special_சாவிகள்(char_u *args)
{
    எழுத்து *save_str = nb_unquote(args, வெற்று);
    எழுத்து *tok = strtok(save_str, " ");
    எழுத்து *sep;
#define KEYBUFLEN 64
    எழுத்து keybuf[KEYBUFLEN];
    எழுத்து cmdbuf[256];

    போது (tok != வெற்று)
    {
	முழு i = 0;

	எனில் ((sep = strchr(tok, '-')) != வெற்று)
	{
	    *sep = NUL;
	    போது (*tok && i + 2 < KEYBUFLEN)
	    {
		தேர்வு (*tok)
		{
		    நிலை 'A':
		    நிலை 'M':
		    நிலை 'C':
		    நிலை 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			நிறுத்து;
		}
		tok++;
	    }
	    tok++;
	}

	எனில் (strlen(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    முழு safe = TRUE;
	    க்கு (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		எனில் (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    safe = FALSE;
		    நிறுத்து;
		}
	    }
	    எனில் (safe)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, அளவு(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, MODE_NORMAL, FALSE);
	    }
	}
	tok = strtok(வெற்று, " ");
    }
    vim_free(save_str);
}

    வெறுமை
ex_nbclose(exarg_T *eap UNUSED)
{
    netbeans_close();
}

    வெறுமை
ex_nbkey(exarg_T *eap)
{
    (வெறுமை)netbeans_keystring(eap->arg);
}

    வெறுமை
ex_nbstart(
    exarg_T	*eap)
{
#ifdef FEAT_GUI
# எனில் !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    எனில் (gui.in_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	திரும்பு;
    }
# endif
#endif
    netbeans_open((எழுத்து *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    நிலையான வெறுமை
nb_init_graphics(வெறுமை)
{
    நிலையான முழு did_init = FALSE;

    எனில் (did_init)
	திரும்பு;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    நிலையான வெறுமை
netbeans_keyname(முழு சாவி, எழுத்து *buf)
{
    எழுத்து *name = 0;
    எழுத்து namebuf[2];
    முழு ctrl  = 0;
    முழு shift = 0;
    முழு alt   = 0;

    எனில் (mod_mask & MOD_MASK_CTRL)
	ctrl = 1;
    எனில் (mod_mask & MOD_MASK_SHIFT)
	shift = 1;
    எனில் (mod_mask & MOD_MASK_ALT)
	alt = 1;


    தேர்வு (சாவி)
    {
	நிலை K_F1:		name = "F1";		நிறுத்து;
	நிலை K_S_F1:	name = "F1";	shift = 1;	நிறுத்து;
	நிலை K_F2:		name = "F2";		நிறுத்து;
	நிலை K_S_F2:	name = "F2";	shift = 1;	நிறுத்து;
	நிலை K_F3:		name = "F3";		நிறுத்து;
	நிலை K_S_F3:	name = "F3";	shift = 1;	நிறுத்து;
	நிலை K_F4:		name = "F4";		நிறுத்து;
	நிலை K_S_F4:	name = "F4";	shift = 1;	நிறுத்து;
	நிலை K_F5:		name = "F5";		நிறுத்து;
	நிலை K_S_F5:	name = "F5";	shift = 1;	நிறுத்து;
	நிலை K_F6:		name = "F6";		நிறுத்து;
	நிலை K_S_F6:	name = "F6";	shift = 1;	நிறுத்து;
	நிலை K_F7:		name = "F7";		நிறுத்து;
	நிலை K_S_F7:	name = "F7";	shift = 1;	நிறுத்து;
	நிலை K_F8:		name = "F8";		நிறுத்து;
	நிலை K_S_F8:	name = "F8";	shift = 1;	நிறுத்து;
	நிலை K_F9:		name = "F9";		நிறுத்து;
	நிலை K_S_F9:	name = "F9";	shift = 1;	நிறுத்து;
	நிலை K_F10:		name = "F10";		நிறுத்து;
	நிலை K_S_F10:	name = "F10";	shift = 1;	நிறுத்து;
	நிலை K_F11:		name = "F11";		நிறுத்து;
	நிலை K_S_F11:	name = "F11";	shift = 1;	நிறுத்து;
	நிலை K_F12:		name = "F12";		நிறுத்து;
	நிலை K_S_F12:	name = "F12";	shift = 1;	நிறுத்து;
	இயல்பு:
			எனில் (சாவி >= ' ' && சாவி <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = சாவி;
			    namebuf[1] = NUL;
			}
			இல்லையெனில்
			    name = "X";
			நிறுத்து;
    }

    buf[0] = '\0';
    எனில் (ctrl)
	strcat(buf, "C");
    எனில் (shift)
	strcat(buf, "S");
    எனில் (alt)
	strcat(buf, "M"); // META
    எனில் (ctrl || shift || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#எனில் defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    வெறுமை
netbeans_beval_cb(
	BalloonEval	*beval,
	முழு		 state UNUSED)
{
    win_T	*wp;
    char_u	*text;
    linenr_T	lnum;
    முழு		col;
    எழுத்து	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    எனில் (!can_use_beval() || !NETBEANS_OPEN)
	திரும்பு;

    எனில் (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) != OK)
	திரும்பு;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    எனில் (text != வெற்று && text[0] != NUL && STRLEN(text) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	எனில் (buf != வெற்று)
	{
	    p = nb_quote(text);
	    எனில் (p != வெற்று)
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
    முழு
netbeans_active(வெறுமை)
{
    திரும்பு NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    வெறுமை
netbeans_open(எழுத்து *params, முழு doabort)
{
    எழுத்து *cmd = "0:startupDone=0\n";

    எனில் (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	திரும்பு;
    }

    எனில் (netbeans_connect(params, doabort) != OK)
	திரும்பு;

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
    வெறுமை
netbeans_send_disconnect(வெறுமை)
{
    எழுத்து buf[128];

    எனில் (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#எனில் defined(FEAT_EVAL)
    முழு
set_ref_in_nb_channel(முழு copyID)
{
    முழு abort = FALSE;
    typval_T tv;

    எனில் (nb_channel == வெற்று)
	திரும்பு FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = set_ref_in_item(&tv, copyID, வெற்று, வெற்று, வெற்று);
    திரும்பு abort;
}
#endif

#எனில் defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    வெறுமை
netbeans_frame_moved(முழு new_x, முழு new_y)
{
    எழுத்து buf[128];

    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (முழு)Columns, (முழு)Rows, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#endif

/*
 * Tell netbeans the user opened or activated a file.
 */
    வெறுமை
netbeans_file_activated(buf_T *bufp)
{
    முழு bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    எழுத்து    buffer[2*MAXPATHL];
    char_u  *q;

    எனில் (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	திரும்பு;

    q = nb_quote(bufp->b_ffname);
    எனில் (q == வெற்று)
	திரும்பு;
    எனில் (bp == வெற்று)
    {
	vim_free(q);
	திரும்பு;
    }

    vim_snprintf(buffer, அளவு(buffer),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (எழுத்து *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    வெறுமை
netbeans_file_opened(buf_T *bufp)
{
    முழு bufno = nb_getbufno(bufp);
    எழுத்து    buffer[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    முழு	    bnum;

    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    q = nb_quote(bufp->b_ffname);
    எனில் (q == வெற்று)
	திரும்பு;
    எனில் (bp != வெற்று)
	bnum = bufno;
    இல்லையெனில்
	bnum = 0;

    vim_snprintf(buffer, அளவு(buffer), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (எழுத்து *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
    எனில் (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	last_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    வெறுமை
netbeans_file_killed(buf_T *bufp)
{
    முழு		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    எழுத்து	buffer[2*MAXPATHL];

    எனில் (!NETBEANS_OPEN || bufno == -1)
	திரும்பு;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(buffer, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_killed");

    எனில் (nbbuf != வெற்று)
	nbbuf->bufp = வெற்று;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    நிலையான nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, முழு *bufnop)
{
    முழு		bufno;
    nbbuf_T	*nbbuf;

    எனில் (!NETBEANS_OPEN || !netbeansFireChanges)
	திரும்பு வெற்று;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    எனில் (bufno <= 0)
	திரும்பு வெற்று;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    எனில் (nbbuf != வெற்று && !nbbuf->fireChanges)
	திரும்பு வெற்று;		// changes in this buffer are not reported

    *bufnop = bufno;
    திரும்பு nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    வெறுமை
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    char_u	*txt,
    முழு		newlen)
{
    char_u	*buf;
    முழு		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;
    char_u	*p;
    char_u	*newtxt;

    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    எனில் (nbbuf == வெற்று)
	திரும்பு;

    // Don't mark as modified for initial read
    எனில் (nbbuf->செருகு_Done)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.col = col;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    எனில் (p != வெற்று)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((எழுத்து *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((எழுத்து *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    வெறுமை
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    long	len)
{
    char_u	buf[128];
    முழு		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;

    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    எனில் (nbbuf == வெற்று)
	திரும்பு;

    எனில் (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	திரும்பு;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.col = col;

    off = pos2off(bufp, &pos);

    sprintf((எழுத்து *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((எழுத்து *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    வெறுமை
netbeans_unmodified(buf_T *bufp UNUSED)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    வெறுமை
netbeans_button_release(முழு button)
{
    எழுத்து	buf[128];
    முழு		bufno;

    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    bufno = nb_getbufno(curbuf);

    எனில் (bufno < 0 || curwin == வெற்று || curwin->w_buffer != curbuf)
	திரும்பு;

    முழு col = mouse_col - curwin->w_wincol
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
    முழு
netbeans_keycommand(முழு சாவி)
{
    எழுத்து	சாவி_Name[60];

    netbeans_keyname(சாவி, சாவி_Name);
    திரும்பு netbeans_keystring((char_u *)சாவி_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    நிலையான முழு
netbeans_keystring(char_u *சாவி_Name)
{
    எழுத்து	buf[2*MAXPATHL];
    முழு		bufno = nb_getbufno(curbuf);
    long	off;
    char_u	*q;

    எனில் (!NETBEANS_OPEN)
	திரும்பு TRUE;

    எனில் (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == வெற்று ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	எனில் (q == வெற்று)
	    திரும்பு TRUE;
	vim_snprintf(buf, அளவு(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	எனில் (curbuf->b_ffname != வெற்று)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(சாவி_Name);
	திரும்பு FALSE;
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
    vim_snprintf(buf, அளவு(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, சாவி_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, அளவு(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, சாவி_Name,
		off, (long)curwin->w_cursor.lnum, (long)curwin->w_cursor.col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    திரும்பு TRUE;
}


/*
 * Send a save event to netbeans.
 */
    வெறுமை
netbeans_save_buffer(buf_T *bufp)
{
    char_u	buf[64];
    முழு		bufno;
    nbbuf_T	*nbbuf;

    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    எனில் (nbbuf == வெற்று)
	திரும்பு;

    nbbuf->modified = 0;

    sprintf((எழுத்து *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((எழுத்து *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    வெறுமை
netbeans_deleted_all_வரிகள்(buf_T *bufp)
{
    char_u	buf[64];
    முழு		bufno;
    nbbuf_T	*nbbuf;

    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    எனில் (nbbuf == வெற்று)
	திரும்பு;

    // Don't mark as modified for initial read
    எனில் (nbbuf->செருகு_Done)
	nbbuf->modified = 1;

    sprintf((எழுத்து *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    முழு
netbeans_is_guarded(linenr_T உச்சி, linenr_T bot)
{
    sign_entry_T	*p;
    முழு			lnum;

    எனில் (!NETBEANS_OPEN)
	திரும்பு FALSE;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
	எனில் (p->se_id >= GUARDEDOFFSET)
	    க்கு (lnum = உச்சி + 1; lnum < bot; lnum++)
		எனில் (lnum == p->se_lnum)
		    திரும்பு TRUE;

    திரும்பு FALSE;
}

#எனில் defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    வெறுமை
netbeans_draw_multisign_indicator(முழு row)
{
    முழு i;
    முழு y;
    முழு x;

    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    x = 0;
    y = row * gui.char_height + 2;

    க்கு (i = 0; i < gui.char_height - 3; i++)
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

#எனில் defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    வெறுமை
netbeans_draw_multisign_indicator(முழு row)
{
    முழு i;
    முழு y;
    முழு x;
# எனில் GTK_CHECK_VERSION(3,0,0)
    cairo_t *cr = வெற்று;
# இல்லையெனில்
    GdkDrawable *drawable = gui.drawarea->window;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# இல்லையெனில்
    y = row * gui.char_height + 2;
# endif

# எனில் GTK_CHECK_VERSION(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_height);
    cr = cairo_create(surf);
#  இல்லையெனில்
    cr = cairo_create(gui.surface);
#  endif
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    க்கு (i = 0; i < gui.char_height - 3; i++)
# எனில் GTK_CHECK_VERSION(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# இல்லையெனில்
    gdk_draw_point(drawable, gui.text_gc, x+2, y++);
# endif

# எனில் GTK_CHECK_VERSION(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# இல்லையெனில்
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

# எனில் GTK_CHECK_VERSION(3,0,0)
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
    வெறுமை
netbeans_gutter_click(linenr_T lnum)
{
    sign_entry_T	*p;

    எனில் (!NETBEANS_OPEN)
	திரும்பு;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
    {
	எனில் (p->se_lnum == lnum && p->se_next && p->se_next->se_lnum == lnum)
	{
	    sign_entry_T *tail;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    எனில் (p->se_prev)
		p->se_prev->se_next = p->se_next;
	    இல்லையெனில்
		curbuf->b_signlist = p->se_next;
	    p->se_next->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    க்கு (tail = p->se_next;
		  tail->se_next && tail->se_next->se_lnum == lnum
				       && tail->se_next->se_id < GUARDEDOFFSET;
		  tail = tail->se_next)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_next = tail->se_next;
	    எனில் (tail->se_next)
		tail->se_next->se_prev = p;
	    p->se_prev = tail;
	    tail->se_next = p;
	    update_debug_sign(curbuf, lnum);
	    நிறுத்து;
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
    நிலையான வெறுமை
addsigntype(
    nbbuf_T	*buf,
    முழு		type_இலக்கம்,
    char_u	*typeName,
    char_u	*tooltip UNUSED,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    முழு i, j;
    முழு use_fg = (*fg && STRCMP(fg, "none") != 0);
    முழு use_bg = (*bg && STRCMP(bg, "none") != 0);

    க்கு (i = 0; i < globalsignmapused; i++)
	எனில் (STRCMP(typeName, globalsignmap[i]) == 0)
	    நிறுத்து;

    எனில் (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_இலக்கம், typeName, tooltip, glyphFile, fg, bg));
	எனில் (use_fg || use_bg)
	{
	    எழுத்து fgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    எழுத்து bgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    எழுத்து *ptr;
	    முழு மதிப்பு;

	    மதிப்பு = strtol((எழுத்து *)fg, &ptr, 10);
	    எனில் (ptr != (எழுத்து *)fg)
		sprintf(fgbuf, "guifg=#%06x", மதிப்பு & 0xFFFFFF);
	    இல்லையெனில்
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    மதிப்பு = strtol((எழுத்து *)bg, &ptr, 10);
	    எனில் (ptr != (எழுத்து *)bg)
		sprintf(bgbuf, "guibg=#%06x", மதிப்பு & 0xFFFFFF);
	    இல்லையெனில்
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    எனில் (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    இல்லையெனில் எனில் (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    இல்லையெனில்
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	இல்லையெனில்
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	எனில் (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_இலக்கம்;

	எனில் (globalsignmapused == globalsignmaplen)
	{
	    எனில் (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(எழுத்து *, globalsignmaplen);
	    }
	    இல்லையெனில்    // grow it
	    {
		முழு incr;
		முழு oldlen = globalsignmaplen;
		எழுத்து **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * அளவு(எழுத்து *));
		எனில் (globalsignmap == வெற்று)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    திரும்பு;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * அளவு(எழுத்து *));
	    }
	}

	globalsignmap[i] = (எழுத்து *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    க்கு (j = 0; j < buf->signmapused; j++)
	எனில் (buf->signmap[j] == i + 1)
	    திரும்பு;

    // add to local map
    எனில் (buf->signmapused == buf->signmaplen)
    {
	எனில் (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(முழு, buf->signmaplen);
	}
	இல்லையெனில்    // grow it
	{
	    முழு incr;
	    முழு oldlen = buf->signmaplen;
	    முழு *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * அளவு(முழு));
	    எனில் (buf->signmap == வெற்று)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		திரும்பு;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * அளவு(முழு));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    நிலையான முழு
mapsigntype(nbbuf_T *buf, முழு localsigntype)
{
    எனில் (--localsigntype >= 0 && localsigntype < buf->signmapused)
	திரும்பு buf->signmap[localsigntype];

    திரும்பு 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    நிலையான long
get_buf_size(buf_T *bufp)
{
    linenr_T	lnum;
    long	char_மொத்தம் = 0;
    முழு		eol_size;
    long	last_check = 100000L;

    எனில் (bufp->b_ml.ml_flags & ML_EMPTY)
	திரும்பு 0;

    எனில் (get_fileformat(bufp) == EOL_DOS)
	eol_size = 2;
    இல்லையெனில்
	eol_size = 1;
    க்கு (lnum = 1; lnum <= bufp->b_ml.ml_வரி_மொத்தம்; ++lnum)
    {
	char_மொத்தம் += ml_get_buf_len(bufp, lnum) + eol_size;
	// Check for a CTRL-C every 100000 characters
	எனில் (char_மொத்தம் > last_check)
	{
	    ui_breakcheck();
	    எனில் (got_int)
		திரும்பு char_மொத்தம்;
	    last_check = char_மொத்தம் + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    எனில் (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_மொத்தம் -= eol_size;

    திரும்பு char_மொத்தம்;
}

/*
 * Convert character offset to lnum,col
 */
    நிலையான pos_T *
off2pos(buf_T *buf, long offset)
{
    linenr_T	 lnum;
    நிலையான pos_T pos;

    pos.lnum = 0;
    pos.col = 0;
    pos.coladd = 0;

    எனில் (!(buf->b_ml.ml_flags & ML_EMPTY))
    {
	எனில் ((lnum = ml_find_வரி_or_offset(buf, (linenr_T)0, &offset)) < 0)
	    திரும்பு வெற்று;
	pos.lnum = lnum;
	pos.col = offset;
    }

    திரும்பு &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    நிலையான pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    நிலையான pos_T	mypos;
    long		off;

    off = strtol((எழுத்து *)*argp, (எழுத்து **)argp, 10);
    எனில் (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.col = strtol((எழுத்து *)*argp, (எழுத்து **)argp, 10);
	mypos.coladd = 0;
	திரும்பு &mypos;
    }
    திரும்பு off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    நிலையான long
pos2off(buf_T *buf, pos_T *pos)
{
    long	 offset = 0;

    எனில் (buf->b_ml.ml_flags & ML_EMPTY)
	திரும்பு 0;

    எனில் ((offset = ml_find_வரி_or_offset(buf, pos->lnum, 0)) < 0)
	திரும்பு 0;
    offset += pos->col;

    திரும்பு offset;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    நிலையான வெறுமை
print_read_செய்தி(nbbuf_T *buf)
{
    முழு	    lnum = buf->bufp->b_ml.ml_வரி_மொத்தம்;
    off_T   nchars = buf->bufp->b_orig_size;
    char_u  c;

    செய்தி_add_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    எனில் (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    எனில் (!buf->bufp->b_start_eol)
    {
	STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    செய்தி_add_வரிகள்(c, (long)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_செய்தி);
    செய்தி_scrolled_ign = TRUE;
    செய்தி_trunc_attr((எழுத்து *)IObuff, FALSE, 0);
    செய்தி_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    நிலையான வெறுமை
print_save_செய்தி(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    எனில் (nchars >= 0)
    {
	// put fname in IObuff with quotes
	செய்தி_add_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	செய்தி_add_வரிகள்(c, buf->bufp->b_ml.ml_வரி_மொத்தம்,
						buf->bufp->b_orig_size);

	VIM_CLEAR(keep_செய்தி);
	செய்தி_scrolled_ign = TRUE;
	p = (char_u *)செய்தி_trunc_attr((எழுத்து *)IObuff, FALSE, 0);
	எனில் ((செய்தி_scrolled && !need_wait_return) || !buf->initDone)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_செய்தி(p, 0);
	}
	செய்தி_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    இல்லையெனில்
    {
	எழுத்து msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_add_bang_to_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#endif // defined(FEAT_NETBEANS_INTG)
