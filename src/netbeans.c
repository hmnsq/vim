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

#include "vim.h"

#если defined(FEAT_NETBEANS_INTG)

#ifndef MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#endif

#include "version.h"

#define GUARDED		10000 // typenr for "guarded" annotation
#define GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#define MAX_COLOR_LENGTH 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#define VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#define VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
static char *ExtEdProtocolVersion = "2.5";

static long pos2off(buf_T *, pos_T *);
static pos_T *off2pos(buf_T *, long);
static pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
static long get_buf_size(buf_T *);
static целое netbeans_keystring(char_u *keystr);
static void special_keys(char_u *args);

static целое getConnInfo(char *file, char **host, char **port, char **password);

static void nb_init_graphics(void);
static void coloncmd(char *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
static void nb_set_curbuf(buf_T *buf);
static void nb_parse_cmd(char_u *);
static целое  nb_do_cmd(целое, char_u *, целое, целое, char_u *);
static void nb_send(char *buf, char *fun);
static void nb_free(void);

#define NETBEANS_OPEN (channel_can_write_to(nb_channel))
static channel_T *nb_channel = NULL;

static целое r_cmdno;			// current command number for reply
static целое dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ifdef NBDEBUG
# include "nbdebug.c"
#endif

static целое needupdate = 0;
static целое inAtomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    static целое
nb_is_safe_string(char_u *str, char_u *allowed)
{
    если (str == NULL)
	вернуть FALSE;
    для (char_u *p = str; *p != NUL; p++)
	если (vim_strchr(allowed, *p) == NULL)
	    вернуть FALSE;
    вернуть TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    static void
nb_channel_closed(void)
{
    nb_channel = NULL;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    static void
netbeans_close(void)
{
    если (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	если (nb_channel != NULL)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = NULL;
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

    static целое
netbeans_connect(char *params, целое doabort)
{
    целое		port;
    char	buf[32];
    char	*hostname = NULL;
    char	*address = NULL;
    char	*password = NULL;
    char	*fname;
    char	*arg = NULL;

    если (*params == '=')
    {
	// "=fname": Read info from specified file.
	если (getConnInfo(params + 1, &hostname, &address, &password) == FAIL)
	    вернуть FAIL;
    }
    иначе
    {
	если (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	если (arg == NULL && (fname = getenv("__NETBEANS_CONINFO")) != NULL)
	{
	    // "": get info from file specified in environment
	    если (getConnInfo(fname, &hostname, &address, &password) == FAIL)
		вернуть FAIL;
	}
	иначе
	{
	    если (arg != NULL)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		address = strchr(hostname, ':');
		если (address != NULL)
		{
		    *address++ = '\0';
		    password = strchr(address, ':');
		    если (password != NULL)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    если (hostname == NULL || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    если (address == NULL)
		address = getenv("__NETBEANS_SOCKET");
	    если (password == NULL)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    если (hostname != NULL)
		hostname = (char *)vim_strsave((char_u *)hostname);
	    если (address != NULL)
		address = (char *)vim_strsave((char_u *)address);
	    если (password != NULL)
		password = (char *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    если (hostname == NULL || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (char *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    если (address == NULL || *address == '\0')
    {
	vim_free(address);
	address = (char *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    если (password == NULL || *password == '\0')
    {
	vim_free(password);
	password = (char *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    если (hostname != NULL && address != NULL && password != NULL)
    {
	port = atoi(address);
	nb_channel = channel_open(hostname, port, 3000, nb_channel_closed);
	если (nb_channel != NULL)
	{
	    // success
#ifdef FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#endif

	    // success, login
	    vim_snprintf(buf, sizeof(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", ExtEdProtocolVersion);
	    nb_send(buf, "externaleditor_version");
	}
    }

    если (nb_channel == NULL && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(address);
    vim_free(password);
    вернуть NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    static целое
getConnInfo(char *file, char **host, char **port, char **auth)
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
    если (mch_stat(file, &st) == 0 && (st.st_mode & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_mode_for_netbeans_connection_info_file_str),
									file);
	вернуть FAIL;
    }
#endif

    fp = mch_fopen(file, "r");
    если (fp == NULL)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	вернуть FAIL;
    }

    // Read the file. There should be one of each parameter
    пока ((lp = (char_u *)fgets((char *)buf, BUFSIZ, fp)) != NULL)
    {
	если ((nlp = vim_strchr(lp, '\n')) != NULL)
	    *nlp = 0;	    // strip off the trailing newline

	если (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (char *)vim_strsave(&buf[5]);
	}
	иначе если (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*port);
	    *port = (char *)vim_strsave(&buf[5]);
	}
	иначе если (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (char *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    вернуть OK;
}


struct keyqueue
{
    char_u	    *keystr;
    struct keyqueue *next;
    struct keyqueue *prev;
};

typedef struct keyqueue keyQ_T;

static keyQ_T keyHead; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    static void
postpone_keycommand(char_u *keystr)
{
    keyQ_T *node;

    node = ALLOC_ONE(keyQ_T);
    если (node == NULL)
	вернуть;  // out of memory, drop the key

    если (keyHead.next == NULL) // initialize circular queue
    {
	keyHead.next = &keyHead;
	keyHead.prev = &keyHead;
    }

    // insert node at tail of queue
    node->next = &keyHead;
    node->prev = keyHead.prev;
    keyHead.prev->next = node;
    keyHead.prev = node;

    node->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    static void
handle_key_queue(void)
{
    целое postponed = FALSE;

    пока (!postponed && keyHead.next && keyHead.next != &keyHead)
    {
	// first, unlink the node
	keyQ_T *node = keyHead.next;
	keyHead.next = node->next;
	node->next->prev = node->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	если (node->keystr != NULL)
	    postponed = !netbeans_keystring(node->keystr);
	vim_free(node->keystr);

	// Finally, dispose of the node
	vim_free(node);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    void
netbeans_parse_messages(void)
{
    readq_T	*node;
    char_u	*buffer;
    char_u	*p;
    целое		own_node;

    пока (nb_channel != NULL)
    {
	node = channel_peek(nb_channel, PART_SOCK);
	если (node == NULL)
	    break;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_first_nl(node);
	если (p == NULL)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    если (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		вернуть;
	    continue;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	если (*p == NUL)
	{
	    own_node = TRUE;
	    buffer = channel_get(nb_channel, PART_SOCK, NULL);
	    // "node" is now invalid!
	}
	иначе
	{
	    own_node = FALSE;
	    buffer = node->rq_buffer;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(buffer);

	если (own_node)
	    // buffer finished, dispose of it
	    vim_free(buffer);
	иначе если (nb_channel != NULL)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (целое)(p - buffer));
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
    static void
nb_parse_cmd(char_u *cmd)
{
    char	*verb;
    char	*q;
    целое		bufno;
    целое		isfunc = -1;

    если (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    если (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_column = false;

	// The IDE is breaking the connection.
	netbeans_close();
	вернуть;
    }

    bufno = strtol((char *)cmd, &verb, 10);

    если (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	вернуть;
    }
    ++verb; // skip colon

    для (q = verb; *q; q++)
    {
	если (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    break;
	}
	иначе если (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    break;
	}
    }

    если (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_in_str), cmd);
	вернуть;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (char *)skipwhite((char_u *)q);

    если (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
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

struct nbbuf_struct
{
    buf_T		*bufp;
    unsigned целое	 fireChanges:1;
    unsigned целое	 initDone:1;
    unsigned целое	 insertDone:1;
    unsigned целое	 modified:1;
    целое			 nbbuf_number;
    char		*displayname;
    целое			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

typedef struct nbbuf_struct nbbuf_T;

static nbbuf_T *buf_list = NULL;
static целое buf_list_size = 0;	// size of buf_list
static целое buf_list_used = 0;	// nr of entries in buf_list actually in use

static char **globalsignmap = NULL;
static целое globalsignmaplen = 0;
static целое globalsignmapused = 0;

static целое  mapsigntype(nbbuf_T *, целое localsigntype);
static void addsigntype(nbbuf_T *, целое localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
static void print_read_msg(nbbuf_T *buf);
static void print_save_msg(nbbuf_T *buf, off_T nchars);

static целое curPCtype = -1;

/*
 * Free netbeans resources.
 */
    static void
nb_free(void)
{
    keyQ_T *key_node = keyHead.next;
    nbbuf_T buf;
    целое i;

    // free the netbeans buffer list
    для (i = 0; i < buf_list_used; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	если (buf.bufp != NULL && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_size = 0;
    buf_list_used = 0;

    // free the queued key commands
    пока (key_node != NULL && key_node != &keyHead)
    {
	keyQ_T *next = key_node->next;
	vim_free(key_node->keystr);
	vim_free(key_node);
	если (next == &keyHead)
	{
	    keyHead.next = &keyHead;
	    keyHead.prev = &keyHead;
	    break;
	}
	key_node = next;
    }

    // free the queued netbeans commands
    если (nb_channel != NULL)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    static целое
nb_getbufno(buf_T *bufp)
{
    целое i;

    для (i = 0; i < buf_list_used; i++)
	если (buf_list[i].bufp == bufp)
	    вернуть i;
    вернуть -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    целое
isNetbeansBuffer(buf_T *bufp)
{
    вернуть NETBEANS_OPEN && bufp->b_netbeans_file;
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
    целое
isNetbeansModified(buf_T *bufp)
{
    если (isNetbeansBuffer(bufp))
    {
	целое bufno = nb_getbufno(bufp);

	если (bufno > 0)
	    вернуть buf_list[bufno].modified;
	иначе
	    вернуть FALSE;
    }
    иначе
	вернуть FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    static nbbuf_T *
nb_get_buf(целое bufno)
{
    // find or create a buffer with the given number
    целое incr;

    если (bufno <= 0)
	вернуть NULL;

    если (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * sizeof(nbbuf_T));
	buf_list_size = 100;
    }
    если (bufno >= buf_list_used) // new
    {
	если (bufno >= buf_list_size) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_size + 90;
	    buf_list_size += incr;
	    bufsize = buf_list_size * sizeof(nbbuf_T);
	    если (bufsize == 0 || bufsize / sizeof(nbbuf_T)
						      != (size_t)buf_list_size)
	    {
		// list size overflow, bail out
		вернуть NULL;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    если (buf_list == NULL)
	    {
		vim_free(t_buf_list);
		buf_list_size = 0;
		вернуть NULL;
	    }
	    vim_memset(buf_list + buf_list_size - incr, 0,
						      incr * sizeof(nbbuf_T));
	}

	пока (buf_list_used <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_used].fireChanges = 1;
	    ++buf_list_used;
	}
    }

    вернуть buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    static целое
count_changed_buffers(void)
{
    buf_T	*bufp;
    целое		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	если (bufp->b_changed)
	    ++n;
    вернуть n;
}

/*
 * End the netbeans session.
 */
    void
netbeans_end(void)
{
    целое i;
    static char buf[128];

    если (!NETBEANS_OPEN)
	вернуть;

    для (i = 0; i < buf_list_used; i++)
    {
	если (!buf_list[i].bufp)
	    continue;
	если (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, NULL);
	buf_list[i].bufp = NULL;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    static void
nb_send(char *buf, char *fun)
{
    если (nb_channel != NULL)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (целое)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    static void
nb_reply_nil(целое cmdno)
{
    char reply[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    если (!NETBEANS_OPEN)
	вернуть;

    sprintf(reply, "%d\n", cmdno);
    nb_send(reply, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    static void
nb_reply_text(целое cmdno, char_u *result)
{
    char_u *reply;

    nbdebug(("REP %d: %s\n", cmdno, (char *)result));

    reply = alloc(STRLEN(result) + 32);
    sprintf((char *)reply, "%d %s\n", cmdno, (char *)result);
    nb_send((char *)reply, "nb_reply_text");

    vim_free(reply);
}


/*
 * Send a response with a number result code.
 */
    static void
nb_reply_nr(целое cmdno, long result)
{
    char reply[32];

    nbdebug(("REP %d: %ld\n", cmdno, result));

    sprintf(reply, "%d %ld\n", cmdno, result);
    nb_send(reply, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    static char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    если (buf == NULL)
	вернуть NULL;
    для (; *p; p++)
    {
	switch (*p)
	{
	    case '\"':
	    case '\\':
		*q++ = '\\'; *q++ = *p; break;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    case '\n':
		*q++ = '\\'; *q++ = 'n'; break;
	    case '\r':
		*q++ = '\\'; *q++ = 'r'; break;
	    default:
		*q++ = *p;
		break;
	}
    }
    *q = '\0';

    вернуть buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    static char *
nb_unquote(char_u *p, char_u **endp)
{
    char *result = 0;
    char *q;
    целое done = 0;

    // result is never longer than input
    result = alloc_clear(STRLEN(p) + 1);
    если (result == NULL)
	вернуть NULL;

    если (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	result[0] = NUL;
	вернуть result;
    }

    для (q = result; !done && *p != NUL;)
    {
	switch (*p)
	{
	    case '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		done = 1;
		break;

	    case '\\':
		++p;
		switch (*p)
		{
		    case '\\':	*q++ = '\\';	break;
		    case 'n':	*q++ = '\n';	break;
		    case 't':	*q++ = '\t';	break;
		    case 'r':	*q++ = '\r';	break;
		    case '"':	*q++ = '"';	break;
		    case NUL:	--p;		break;
		    // default: skip over illegal chars
		}
		++p;
		break;

	    default:
		*q++ = *p++;
	}
    }

    если (endp != NULL)
	*endp = p;

    вернуть result;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    static void
nb_partialremove(linenr_T lnum, colnr_T first, colnr_T last)
{
    char_u *oldtext, *newtext;
    целое oldlen;
    целое lastbyte = last;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    если (first >= (colnr_T)oldlen || oldlen == 0)  // just in case
	вернуть;
    если (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - first));
    если (newtext == NULL)
	вернуть;

    mch_memmove(newtext, oldtext, first);
    STRMOVE(newtext + first, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    static void
nb_joinlines(linenr_T first, linenr_T other)
{
    целое len_first, len_other;
    char_u *p;

    len_first = ml_get_len(first);
    len_other = ml_get_len(other);
    p = alloc(len_first + len_other + 1);
    если (p == NULL)
	вернуть;

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
    static целое
nb_do_cmd(
    целое		bufno,
    char_u	*cmd,
    целое		func,
    целое		cmdno,
    char_u	*args)	    // points to space before arguments or NUL
{
    целое		do_update = 0;
    long	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    static целое	skip = 0;
    целое		retval = OK;
    char	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (char *)args));

    если (func)
    {
// =====================================================================
	если (streq((char *)cmd, "getModified"))
	{
	    если (buf == NULL || buf->bufp == NULL)
		// Return the number of buffers that are modified.
		nb_reply_nr(cmdno, (long)count_changed_buffers());
	    иначе
		// Return whether the buffer is modified.
		nb_reply_nr(cmdno, (long)(buf->bufp->b_changed
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	иначе если (streq((char *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_reply_nr(cmdno, (long)count_changed_buffers());
// =====================================================================
	}
	иначе если (streq((char *)cmd, "getCursor"))
	{
	    char_u text[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((char *)text, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (long)curwin->w_cursor.lnum,
		    (целое)curwin->w_cursor.col,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_reply_text(cmdno, text);
// =====================================================================
	}
	иначе если (streq((char *)cmd, "getAnno"))
	{
	    long linenum = 0;
#ifdef FEAT_SIGNS
	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_getanno));
		retval = FAIL;
	    }
	    иначе
	    {
		целое serNum;

		cp = (char *)args;
		serNum = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (long)buf_findsign(buf->bufp, serNum, NULL);
	    }
#endif
	    nb_reply_nr(cmdno, linenum);
// =====================================================================
	}
	иначе если (streq((char *)cmd, "getLength"))
	{
	    long len = 0;

	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_buffer_identifier_in_getlength));
		retval = FAIL;
	    }
	    иначе
	    {
		len = get_buf_size(buf->bufp);
	    }
	    nb_reply_nr(cmdno, len);
// =====================================================================
	}
	иначе если (streq((char *)cmd, "getText"))
	{
	    long	len;
	    linenr_T	nlines;
	    char_u	*text = NULL;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*line;

	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_buffer_identifier_in_gettext));
		retval = FAIL;
	    }
	    иначе
	    {
		len = get_buf_size(buf->bufp);
		nlines = buf->bufp->b_ml.ml_line_count;
		text = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		если (text == NULL)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		иначе
		{
		    p = text;
		    *p++ = '\"';
		    для (; lno <= nlines ; lno++)
		    {
			line = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			если (line != NULL)
			{
			    STRCPY(p, line);
			    p += STRLEN(line);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(line);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    если (text == NULL)
		nb_reply_text(cmdno, (char_u *)"");
	    иначе
	    {
		nb_reply_text(cmdno, text);
		vim_free(text);
	    }
// =====================================================================
	}
	иначе если (streq((char *)cmd, "remove"))
	{
	    long count;
	    pos_T first, last;
	    pos_T *pos;
	    pos_T *next;
	    linenr_T del_from_lnum, del_to_lnum;  // lines to be deleted as a whole
	    целое oldFire = netbeansFireChanges;
	    целое oldSuppress = netbeansSuppressNoLines;
	    целое wasChanged;

	    если (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (char *) cmd));
		nb_reply_nil(cmdno);
		вернуть OK;
	    }

	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_buffer_identifier_in_remove));
		retval = FAIL;
	    }
	    иначе
	    {
		netbeansFireChanges = FALSE;
		netbeansSuppressNoLines = TRUE;

		nb_set_curbuf(buf->bufp);
		wasChanged = buf->bufp->b_changed;
		cp = (char *)args;
		off = strtol(cp, &cp, 10);
		count = strtol(cp, &cp, 10);
		args = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		если (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeansSuppressNoLines = oldSuppress;
		    вернуть FAIL;
		}
		first = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      first.lnum, first.col));
		pos = off2pos(buf->bufp, off+count-1);
		если (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeansSuppressNoLines = oldSuppress;
		    вернуть FAIL;
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
		next = off2pos(buf->bufp, off + count);

		// Remove part of the first line.
		если (first.col != 0
				|| (next != NULL && first.lnum == next->lnum))
		{
		    если (first.lnum != last.lnum
			    || (next != NULL && first.lnum != next->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(first.lnum, first.col,
							     (colnr_T)MAXCOL);
			если (first.lnum == last.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(first.lnum, next->lnum);
			    del_to_lnum = next->lnum;
			}
		    }
		    иначе
		    {
			// remove within one line
			nb_partialremove(first.lnum, first.col, last.col);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		если (first.lnum != last.lnum && next != NULL
			&& next->col != 0 && last.lnum == next->lnum)
		{
		    nb_partialremove(last.lnum, 0, last.col);
		    если (del_from_lnum > first.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(first.lnum, last.lnum);
		    }
		    иначе
			// First line is deleted as a whole, keep the last
			// line.
			--del_to_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		если (first.lnum != last.lnum && del_from_lnum > first.lnum
			&& next != NULL && last.lnum != next->lnum)
		{
		    nb_joinlines(first.lnum, next->lnum);
		    del_to_lnum = next->lnum;
		}

		// Delete whole lines if there are any.
		если (del_to_lnum >= del_from_lnum)
		{
		    целое i;

		    // delete signs from the lines being deleted
		    для (i = del_from_lnum; i <= del_to_lnum; i++)
		    {
			целое id = buf_findsign_id(buf->bufp, (linenr_T)i, NULL);
			если (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, NULL);
			}
			иначе
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_to_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.col = 0;
		    del_lines(del_to_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = first;
		check_cursor_lnum();
		buf->bufp->b_changed = wasChanged; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeansSuppressNoLines = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_reply_nil(cmdno);
// =====================================================================
	}
	иначе если (streq((char *)cmd, "insert"))
	{
	    char_u	*to_free;

	    если (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (char *) cmd));
		nb_reply_nil(cmdno);
		вернуть OK;
	    }

	    // get offset
	    cp = (char *)args;
	    off = strtol(cp, &cp, 10);
	    args = (char_u *)cp;

	    // get text to be inserted
	    args = skipwhite(args);
	    args = to_free = (char_u *)nb_unquote(args, NULL);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_buffer_identifier_in_insert));
		retval = FAIL;
	    }
	    иначе если (args != NULL)
	    {
		целое	ff_detected = EOL_UNKNOWN;
		целое	buf_was_empty = (buf->bufp->b_ml.ml_flags & ML_EMPTY);
		size_t	len = 0;
		целое	added = 0;
		целое	oldFire = netbeansFireChanges;
		целое	old_b_changed;
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
		если (pos != NULL)
		{
		    если (pos->lnum <= 0)
			lnum_start = 1;
		    иначе
			lnum_start = pos->lnum;
		}
		иначе
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    если (buf_was_empty)
			lnum_start = 1;	    // above empty line
		    иначе
			lnum_start = curbuf->b_ml.ml_line_count + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_start;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		пока (*args != NUL)
		{
		    nlp = vim_strchr(args, '\n');
		    если (nlp == NULL)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(args);
		    }
		    иначе
		    {
			len = nlp - args;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			если (nlp > args && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			иначе
			    ff_detected = EOL_UNIX;
		    }
		    args[len] = NUL;

		    если (lnum == lnum_start
			    && ((pos != NULL && pos->col > 0)
				|| (lnum == 1 && buf_was_empty)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			целое	col = pos == NULL ? 0 : pos->col;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			если (newline != NULL)
			{
			    mch_memmove(newline, oldline, (size_t)col);
			    newline[col] = NUL;
			    STRCAT(newline, args);
			    STRCAT(newline, oldline + col);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    иначе
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), args,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    если (nlp == NULL)
			break;
		    ++lnum;
		    args = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_lines_mark(lnum_start - 1, (long)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		если (buf_was_empty)
		{
		    если (ff_detected == EOL_UNKNOWN)
#если defined(MSWIN)
			ff_detected = EOL_DOS;
#иначе
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
	иначе
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_reply_nil(cmdno);
	    retval = FAIL;
	}
    }
    иначе // Not a function; no reply required.
    {
// =====================================================================
	если (streq((char *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    если (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_buffer_identifier_in_create));
		вернуть FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, NULL, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->insertDone = FALSE;
#если defined(FEAT_MENU) && defined(FEAT_GUI)
	    если (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	иначе если (streq((char *)cmd, "insertDone"))
	{
	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    иначе
	    {
		buf->bufp->b_start_eol = *args == 'T';
		buf->insertDone = TRUE;
		args += 2;
		buf->bufp->b_p_ro = *args == 'T';
		print_read_msg(buf);
	    }
// =====================================================================
	}
	иначе если (streq((char *)cmd, "saveDone"))
	{
	    long savedChars = atol((char *)args);

	    если (buf == NULL || buf->bufp == NULL)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    иначе
		print_save_msg(buf, savedChars);
// =====================================================================
	}
	иначе если (streq((char *)cmd, "startDocumentListen"))
	{
	    если (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_startdocumentlisten));
		вернуть FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	иначе если (streq((char *)cmd, "stopDocumentListen"))
	{
	    если (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_stopdocumentlisten));
		вернуть FAIL;
	    }
	    buf->fireChanges = 0;
	    если (buf->bufp != NULL && buf->bufp->b_was_netbeans_file)
	    {
		если (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_lost_for_buffer_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_lost_for_buffer_nr),
							   buf->bufp->b_fnum);
		}
		иначе
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
	иначе если (streq((char *)cmd, "setTitle"))
	{
	    если (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_buffer_identifier_in_settitle));
		вернуть FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, NULL);
// =====================================================================
	}
	иначе если (streq((char *)cmd, "initDone"))
	{
	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_buffer_identifier_in_initdone));
		вернуть FAIL;
	    }
	    do_update = 1;
	    buf->initDone = TRUE;
	    nb_set_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_key_queue();
// =====================================================================
	}
	иначе если (streq((char *)cmd, "setBufferNumber")
		|| streq((char *)cmd, "putBufferNumber"))
	{
	    char_u	*path;
	    buf_T	*bufp;

	    если (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_buffer_identifier_in_setbuffernumber));
		вернуть FAIL;
	    }
	    path = (char_u *)nb_unquote(args, NULL);
	    если (path == NULL)
		вернуть FAIL;
	    bufp = buflist_findname(path);
	    vim_free(path);
	    если (bufp == NULL)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", args));
		semsg(_(e_file_str_not_found_in_setbuffernumber), args);
		вернуть FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_number = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    если (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    иначе
	    {
		buf->initDone = TRUE;

		// handle any postponed key commands
		handle_key_queue();
	    }

// =====================================================================
	}
	иначе если (streq((char *)cmd, "setFullName"))
	{
	    если (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_buffer_identifier_in_setfullname));
		вернуть FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, NULL);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#если defined(FEAT_MENU) && defined(FEAT_GUI)
	    если (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	иначе если (streq((char *)cmd, "editFile"))
	{
	    если (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_buffer_identifier_in_editfile));
		вернуть FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, NULL);
	    do_ecmd(0, (char_u *)buf->displayname, NULL, NULL, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->initDone = TRUE;
	    do_update = 1;
	    maketitle();
#если defined(FEAT_MENU) && defined(FEAT_GUI)
	    если (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	иначе если (streq((char *)cmd, "setVisible"))
	{
	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		если (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setvisible));
		вернуть FAIL;
	    }
	    если (streq((char *)args, "T") && buf->bufp != curbuf)
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
		если (gui.in_use)
		    gui_mch_set_foreground();
#endif
	    }
// =====================================================================
	}
	иначе если (streq((char *)cmd, "raise"))
	{
#ifdef FEAT_GUI
	    // Bring gvim to the foreground.
	    если (gui.in_use)
		gui_mch_set_foreground();
#endif
// =====================================================================
	}
	иначе если (streq((char *)cmd, "setModified"))
	{
	    целое prev_b_changed;

	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		если (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setmodified));
		вернуть FAIL;
	    }
	    prev_b_changed = buf->bufp->b_changed;
	    если (streq((char *)args, "T"))
		buf->bufp->b_changed = TRUE;
	    иначе
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		если (buf->bufp->b_ffname != NULL
			&& mch_stat((char *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_time(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_changed = FALSE;
	    }
	    buf->modified = buf->bufp->b_changed;
	    если (prev_b_changed != buf->bufp->b_changed)
	    {
		check_status(buf->bufp);
		redraw_tabline = TRUE;
#если defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	иначе если (streq((char *)cmd, "setModtime"))
	{
	    если (buf == NULL || buf->bufp == NULL)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    иначе
	    {
		buf->bufp->b_mtime = atoi((char *)args);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	иначе если (streq((char *)cmd, "setReadOnly"))
	{
	    если (buf == NULL || buf->bufp == NULL)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    иначе если (streq((char *)args, "T"))
		buf->bufp->b_p_ro = TRUE;
	    иначе
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	иначе если (streq((char *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	иначе если (streq((char *)cmd, "showBalloon"))
	{
#если defined(FEAT_BEVAL_GUI)
	    static char	*text = NULL;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    если (balloonEval != NULL)
	    {
		vim_free(text);
		text = nb_unquote(args, NULL);
		если (text != NULL)
		    gui_mch_post_balloon(balloonEval, (char_u *)text);
	    }
#endif
// =====================================================================
	}
	иначе если (streq((char *)cmd, "setDot"))
	{
	    pos_T *pos;
#ifdef NBDEBUG
	    char_u *s;
#endif

	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_buffer_identifier_in_setdot));
		вернуть FAIL;
	    }

	    nb_set_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    если (VIsual_active)
		end_visual_mode();
#ifdef NBDEBUG
	    s = args;
#endif
	    pos = get_off_or_lnum(buf->bufp, &args);
	    если (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#ifdef FEAT_FOLDING
		foldOpenCursor();
#endif
	    }
	    иначе
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
	    если (State == MODE_HITRETURN || State == MODE_ASKMORE)
	    {
#ifdef FEAT_GUI_GTK
		если (gui.in_use && gtk_main_level() > 0)
		    gtk_main_quit();
#endif
	    }
// =====================================================================
	}
	иначе если (streq((char *)cmd, "close"))
	{
#ifdef NBDEBUG
	    char *name = "<NONE>";
#endif

	    если (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_buffer_identifier_in_close));
		вернуть FAIL;
	    }

#ifdef NBDEBUG
	    если (buf->displayname != NULL)
		name = buf->displayname;
#endif
	    если (buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		если (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    если (buf->bufp != NULL)
		do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = NULL;
	    buf->initDone = FALSE;
	    do_update = 1;
// =====================================================================
	}
	иначе если (streq((char *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	иначе если (streq((char *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	иначе если (streq((char *)cmd, "defineAnnoType"))
	{
#ifdef FEAT_SIGNS
	    целое typeNum;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    целое parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    если (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_buffer_identifier_in_defineannotype));
		вернуть FAIL;
	    }

	    cp = (char *)args;
	    typeNum = strtol(cp, &cp, 10);
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
	    если (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(args);
	    bg = vim_strsave(p);
	    если (STRLEN(fg) > MAX_COLOR_LENGTH || STRLEN(bg) > MAX_COLOR_LENGTH)
	    {
		emsg(_(e_highlighting_color_name_too_long_in_defineAnnoType));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    иначе если (!nb_is_safe_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_safe_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_safe_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_in_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    иначе если (typeName != NULL && tooltip != NULL && glyphFile != NULL)
		addsigntype(buf, typeNum, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    если (parse_error)
		вернуть FAIL;

#endif
// =====================================================================
	}
	иначе если (streq((char *)cmd, "addAnno"))
	{
#ifdef FEAT_SIGNS
	    целое serNum;
	    целое localTypeNum;
	    целое typeNum;
	    pos_T *pos;

	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_addanno));
		вернуть FAIL;
	    }

	    do_update = 1;

	    cp = (char *)args;
	    serNum = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    localTypeNum = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    typeNum = mapsigntype(buf, localTypeNum);

	    pos = get_off_or_lnum(buf->bufp, &args);

	    cp = (char *)args;
	    vim_ignored = (целое)strtol(cp, &cp, 10);
	    args = (char_u *)cp;
# ifdef NBDEBUG
	    если (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    если (serNum >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		вернуть FAIL;
	    }
	    если (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   serNum, pos->lnum, typeNum, buf->bufp->b_fnum);
		если (typeNum == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", serNum,
						       buf->bufp->b_fnum);
	    }
#endif
// =====================================================================
	}
	иначе если (streq((char *)cmd, "removeAnno"))
	{
#ifdef FEAT_SIGNS
	    целое serNum;

	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		вернуть FAIL;
	    }
	    do_update = 1;
	    cp = (char *)args;
	    serNum = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     serNum, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#endif
// =====================================================================
	}
	иначе если (streq((char *)cmd, "moveAnnoToFront"))
	{
#ifdef FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#endif
// =====================================================================
	}
	иначе если (streq((char *)cmd, "guard") || streq((char *)cmd, "unguard"))
	{
	    целое len;
	    pos_T first;
	    pos_T last;
	    pos_T *pos;
	    целое un = (cmd[0] == 'u');
	    static целое guardId = GUARDEDOFFSET;

	    если (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (char *) cmd));
		вернуть OK;
	    }

	    nb_init_graphics();

	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		вернуть FAIL;
	    }
	    nb_set_curbuf(buf->bufp);
	    cp = (char *)args;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, NULL, 10);
	    args = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    если (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    иначе
	    {
		first = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		если (pos != NULL && pos->col == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		если (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		иначе
		{
		    long lnum;
		    last = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     first.lnum, first.col,
			     last.lnum, last.col));
#ifdef FEAT_SIGNS
		    для (lnum = first.lnum; lnum <= last.lnum; lnum++)
		    {
			если (un)
			{
			    // never used
			}
			иначе
			{
			    если (buf_findsigntype_id(buf->bufp, lnum,
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
	иначе если (streq((char *)cmd, "startAtomic"))
	{
	    inAtomic = 1;
// =====================================================================
	}
	иначе если (streq((char *)cmd, "endAtomic"))
	{
	    inAtomic = 0;
	    если (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	иначе если (streq((char *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		вернуть FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    если (bufIsChanged(buf->bufp))
	    {
		// Only write if the buffer can be written.
		если (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != NULL
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    если (!bufref_valid(&bufref))
			buf->bufp = NULL;
		}
	    }
	    иначе
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	иначе если (streq((char *)cmd, "netbeansBuffer"))
	{
	    если (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		вернуть FAIL;
	    }
	    если (*args == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    иначе
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	иначе если (streq((char *)cmd, "specialKeys"))
	{
	    special_keys(args);
// =====================================================================
	}
	иначе если (streq((char *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	иначе если (streq((char *)cmd, "version"))
	{
	    // not used yet
	}
	иначе
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    если (inAtomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    если (buf != NULL && buf->initDone && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	out_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	если (State == MODE_HITRETURN || State == MODE_ASKMORE)
	{
#ifdef FEAT_GUI_GTK
	    если (gui.in_use && gtk_main_level() > 0)
		gtk_main_quit();
#endif
	}
    }

    вернуть retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    static void
nb_set_curbuf(buf_T *buf)
{
    если (curbuf == buf)
	вернуть;

    если (buf_jump_open_win(buf) != NULL)
	вернуть;
    если ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != NULL)
	вернуть;
    set_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    static void
coloncmd(char *cmd, ...)
{
    char buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, sizeof(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, NULL, NULL, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    out_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    static void
special_keys(char_u *args)
{
    char *save_str = nb_unquote(args, NULL);
    char *tok = strtok(save_str, " ");
    char *sep;
#define KEYBUFLEN 64
    char keybuf[KEYBUFLEN];
    char cmdbuf[256];

    пока (tok != NULL)
    {
	целое i = 0;

	если ((sep = strchr(tok, '-')) != NULL)
	{
	    *sep = NUL;
	    пока (*tok && i + 2 < KEYBUFLEN)
	    {
		switch (*tok)
		{
		    case 'A':
		    case 'M':
		    case 'C':
		    case 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			break;
		}
		tok++;
	    }
	    tok++;
	}

	если (strlen(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    целое safe = TRUE;
	    для (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		если (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    safe = FALSE;
		    break;
		}
	    }
	    если (safe)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, sizeof(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, MODE_NORMAL, FALSE);
	    }
	}
	tok = strtok(NULL, " ");
    }
    vim_free(save_str);
}

    void
ex_nbclose(exarg_T *eap UNUSED)
{
    netbeans_close();
}

    void
ex_nbkey(exarg_T *eap)
{
    (void)netbeans_keystring(eap->arg);
}

    void
ex_nbstart(
    exarg_T	*eap)
{
#ifdef FEAT_GUI
# если !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    если (gui.in_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	вернуть;
    }
# endif
#endif
    netbeans_open((char *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    static void
nb_init_graphics(void)
{
    static целое did_init = FALSE;

    если (did_init)
	вернуть;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    static void
netbeans_keyname(целое key, char *buf)
{
    char *name = 0;
    char namebuf[2];
    целое ctrl  = 0;
    целое shift = 0;
    целое alt   = 0;

    если (mod_mask & MOD_MASK_CTRL)
	ctrl = 1;
    если (mod_mask & MOD_MASK_SHIFT)
	shift = 1;
    если (mod_mask & MOD_MASK_ALT)
	alt = 1;


    switch (key)
    {
	case K_F1:		name = "F1";		break;
	case K_S_F1:	name = "F1";	shift = 1;	break;
	case K_F2:		name = "F2";		break;
	case K_S_F2:	name = "F2";	shift = 1;	break;
	case K_F3:		name = "F3";		break;
	case K_S_F3:	name = "F3";	shift = 1;	break;
	case K_F4:		name = "F4";		break;
	case K_S_F4:	name = "F4";	shift = 1;	break;
	case K_F5:		name = "F5";		break;
	case K_S_F5:	name = "F5";	shift = 1;	break;
	case K_F6:		name = "F6";		break;
	case K_S_F6:	name = "F6";	shift = 1;	break;
	case K_F7:		name = "F7";		break;
	case K_S_F7:	name = "F7";	shift = 1;	break;
	case K_F8:		name = "F8";		break;
	case K_S_F8:	name = "F8";	shift = 1;	break;
	case K_F9:		name = "F9";		break;
	case K_S_F9:	name = "F9";	shift = 1;	break;
	case K_F10:		name = "F10";		break;
	case K_S_F10:	name = "F10";	shift = 1;	break;
	case K_F11:		name = "F11";		break;
	case K_S_F11:	name = "F11";	shift = 1;	break;
	case K_F12:		name = "F12";		break;
	case K_S_F12:	name = "F12";	shift = 1;	break;
	default:
			если (key >= ' ' && key <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = key;
			    namebuf[1] = NUL;
			}
			иначе
			    name = "X";
			break;
    }

    buf[0] = '\0';
    если (ctrl)
	strcat(buf, "C");
    если (shift)
	strcat(buf, "S");
    если (alt)
	strcat(buf, "M"); // META
    если (ctrl || shift || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#если defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    void
netbeans_beval_cb(
	BalloonEval	*beval,
	целое		 state UNUSED)
{
    win_T	*wp;
    char_u	*text;
    linenr_T	lnum;
    целое		col;
    char	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    если (!can_use_beval() || !NETBEANS_OPEN)
	вернуть;

    если (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) != OK)
	вернуть;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    если (text != NULL && text[0] != NUL && STRLEN(text) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	если (buf != NULL)
	{
	    p = nb_quote(text);
	    если (p != NULL)
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
    целое
netbeans_active(void)
{
    вернуть NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    void
netbeans_open(char *params, целое doabort)
{
    char *cmd = "0:startupDone=0\n";

    если (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	вернуть;
    }

    если (netbeans_connect(params, doabort) != OK)
	вернуть;

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
    void
netbeans_send_disconnect(void)
{
    char buf[128];

    если (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#если defined(FEAT_EVAL)
    целое
set_ref_in_nb_channel(целое copyID)
{
    целое abort = FALSE;
    typval_T tv;

    если (nb_channel == NULL)
	вернуть FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
    вернуть abort;
}
#endif

#если defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    void
netbeans_frame_moved(целое new_x, целое new_y)
{
    char buf[128];

    если (!NETBEANS_OPEN)
	вернуть;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (целое)Columns, (целое)Rows, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#endif

/*
 * Tell netbeans the user opened or activated a file.
 */
    void
netbeans_file_activated(buf_T *bufp)
{
    целое bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    char    buffer[2*MAXPATHL];
    char_u  *q;

    если (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	вернуть;

    q = nb_quote(bufp->b_ffname);
    если (q == NULL)
	вернуть;
    если (bp == NULL)
    {
	vim_free(q);
	вернуть;
    }

    vim_snprintf(buffer, sizeof(buffer),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (char *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    void
netbeans_file_opened(buf_T *bufp)
{
    целое bufno = nb_getbufno(bufp);
    char    buffer[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    целое	    bnum;

    если (!NETBEANS_OPEN)
	вернуть;

    q = nb_quote(bufp->b_ffname);
    если (q == NULL)
	вернуть;
    если (bp != NULL)
	bnum = bufno;
    иначе
	bnum = 0;

    vim_snprintf(buffer, sizeof(buffer), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (char *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
    если (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	last_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    void
netbeans_file_killed(buf_T *bufp)
{
    целое		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    char	buffer[2*MAXPATHL];

    если (!NETBEANS_OPEN || bufno == -1)
	вернуть;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(buffer, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_killed");

    если (nbbuf != NULL)
	nbbuf->bufp = NULL;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    static nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, целое *bufnop)
{
    целое		bufno;
    nbbuf_T	*nbbuf;

    если (!NETBEANS_OPEN || !netbeansFireChanges)
	вернуть NULL;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    если (bufno <= 0)
	вернуть NULL;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    если (nbbuf != NULL && !nbbuf->fireChanges)
	вернуть NULL;		// changes in this buffer are not reported

    *bufnop = bufno;
    вернуть nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    void
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    char_u	*txt,
    целое		newlen)
{
    char_u	*buf;
    целое		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;
    char_u	*p;
    char_u	*newtxt;

    если (!NETBEANS_OPEN)
	вернуть;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    если (nbbuf == NULL)
	вернуть;

    // Don't mark as modified for initial read
    если (nbbuf->insertDone)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.col = col;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    если (p != NULL)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((char *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((char *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    void
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    long	len)
{
    char_u	buf[128];
    целое		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;

    если (!NETBEANS_OPEN)
	вернуть;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    если (nbbuf == NULL)
	вернуть;

    если (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	вернуть;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.col = col;

    off = pos2off(bufp, &pos);

    sprintf((char *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((char *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    void
netbeans_unmodified(buf_T *bufp UNUSED)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    void
netbeans_button_release(целое button)
{
    char	buf[128];
    целое		bufno;

    если (!NETBEANS_OPEN)
	вернуть;

    bufno = nb_getbufno(curbuf);

    если (bufno < 0 || curwin == NULL || curwin->w_buffer != curbuf)
	вернуть;

    целое col = mouse_col - curwin->w_wincol
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
    целое
netbeans_keycommand(целое key)
{
    char	keyName[60];

    netbeans_keyname(key, keyName);
    вернуть netbeans_keystring((char_u *)keyName);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    static целое
netbeans_keystring(char_u *keyName)
{
    char	buf[2*MAXPATHL];
    целое		bufno = nb_getbufno(curbuf);
    long	off;
    char_u	*q;

    если (!NETBEANS_OPEN)
	вернуть TRUE;

    если (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == NULL ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	если (q == NULL)
	    вернуть TRUE;
	vim_snprintf(buf, sizeof(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	если (curbuf->b_ffname != NULL)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(keyName);
	вернуть FALSE;
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
    vim_snprintf(buf, sizeof(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, keyName);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, sizeof(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, keyName,
		off, (long)curwin->w_cursor.lnum, (long)curwin->w_cursor.col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    вернуть TRUE;
}


/*
 * Send a save event to netbeans.
 */
    void
netbeans_save_buffer(buf_T *bufp)
{
    char_u	buf[64];
    целое		bufno;
    nbbuf_T	*nbbuf;

    если (!NETBEANS_OPEN)
	вернуть;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    если (nbbuf == NULL)
	вернуть;

    nbbuf->modified = 0;

    sprintf((char *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((char *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    void
netbeans_deleted_all_lines(buf_T *bufp)
{
    char_u	buf[64];
    целое		bufno;
    nbbuf_T	*nbbuf;

    если (!NETBEANS_OPEN)
	вернуть;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    если (nbbuf == NULL)
	вернуть;

    // Don't mark as modified for initial read
    если (nbbuf->insertDone)
	nbbuf->modified = 1;

    sprintf((char *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    целое
netbeans_is_guarded(linenr_T top, linenr_T bot)
{
    sign_entry_T	*p;
    целое			lnum;

    если (!NETBEANS_OPEN)
	вернуть FALSE;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
	если (p->se_id >= GUARDEDOFFSET)
	    для (lnum = top + 1; lnum < bot; lnum++)
		если (lnum == p->se_lnum)
		    вернуть TRUE;

    вернуть FALSE;
}

#если defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    void
netbeans_draw_multisign_indicator(целое row)
{
    целое i;
    целое y;
    целое x;

    если (!NETBEANS_OPEN)
	вернуть;

    x = 0;
    y = row * gui.char_height + 2;

    для (i = 0; i < gui.char_height - 3; i++)
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

#если defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    void
netbeans_draw_multisign_indicator(целое row)
{
    целое i;
    целое y;
    целое x;
# если GTK_CHECK_VERSION(3,0,0)
    cairo_t *cr = NULL;
# иначе
    GdkDrawable *drawable = gui.drawarea->window;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    если (!NETBEANS_OPEN)
	вернуть;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# иначе
    y = row * gui.char_height + 2;
# endif

# если GTK_CHECK_VERSION(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_height);
    cr = cairo_create(surf);
#  иначе
    cr = cairo_create(gui.surface);
#  endif
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    для (i = 0; i < gui.char_height - 3; i++)
# если GTK_CHECK_VERSION(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# иначе
    gdk_draw_point(drawable, gui.text_gc, x+2, y++);
# endif

# если GTK_CHECK_VERSION(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# иначе
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

# если GTK_CHECK_VERSION(3,0,0)
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
    void
netbeans_gutter_click(linenr_T lnum)
{
    sign_entry_T	*p;

    если (!NETBEANS_OPEN)
	вернуть;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
    {
	если (p->se_lnum == lnum && p->se_next && p->se_next->se_lnum == lnum)
	{
	    sign_entry_T *tail;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    если (p->se_prev)
		p->se_prev->se_next = p->se_next;
	    иначе
		curbuf->b_signlist = p->se_next;
	    p->se_next->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    для (tail = p->se_next;
		  tail->se_next && tail->se_next->se_lnum == lnum
				       && tail->se_next->se_id < GUARDEDOFFSET;
		  tail = tail->se_next)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_next = tail->se_next;
	    если (tail->se_next)
		tail->se_next->se_prev = p;
	    p->se_prev = tail;
	    tail->se_next = p;
	    update_debug_sign(curbuf, lnum);
	    break;
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
    static void
addsigntype(
    nbbuf_T	*buf,
    целое		typeNum,
    char_u	*typeName,
    char_u	*tooltip UNUSED,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    целое i, j;
    целое use_fg = (*fg && STRCMP(fg, "none") != 0);
    целое use_bg = (*bg && STRCMP(bg, "none") != 0);

    для (i = 0; i < globalsignmapused; i++)
	если (STRCMP(typeName, globalsignmap[i]) == 0)
	    break;

    если (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    typeNum, typeName, tooltip, glyphFile, fg, bg));
	если (use_fg || use_bg)
	{
	    char fgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    char bgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    char *ptr;
	    целое value;

	    value = strtol((char *)fg, &ptr, 10);
	    если (ptr != (char *)fg)
		sprintf(fgbuf, "guifg=#%06x", value & 0xFFFFFF);
	    иначе
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    value = strtol((char *)bg, &ptr, 10);
	    если (ptr != (char *)bg)
		sprintf(bgbuf, "guibg=#%06x", value & 0xFFFFFF);
	    иначе
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    если (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    иначе если (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    иначе
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	иначе
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	если (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = typeNum;

	если (globalsignmapused == globalsignmaplen)
	{
	    если (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(char *, globalsignmaplen);
	    }
	    иначе    // grow it
	    {
		целое incr;
		целое oldlen = globalsignmaplen;
		char **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * sizeof(char *));
		если (globalsignmap == NULL)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    вернуть;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * sizeof(char *));
	    }
	}

	globalsignmap[i] = (char *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    для (j = 0; j < buf->signmapused; j++)
	если (buf->signmap[j] == i + 1)
	    вернуть;

    // add to local map
    если (buf->signmapused == buf->signmaplen)
    {
	если (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(целое, buf->signmaplen);
	}
	иначе    // grow it
	{
	    целое incr;
	    целое oldlen = buf->signmaplen;
	    целое *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * sizeof(целое));
	    если (buf->signmap == NULL)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		вернуть;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * sizeof(целое));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    static целое
mapsigntype(nbbuf_T *buf, целое localsigntype)
{
    если (--localsigntype >= 0 && localsigntype < buf->signmapused)
	вернуть buf->signmap[localsigntype];

    вернуть 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    static long
get_buf_size(buf_T *bufp)
{
    linenr_T	lnum;
    long	char_count = 0;
    целое		eol_size;
    long	last_check = 100000L;

    если (bufp->b_ml.ml_flags & ML_EMPTY)
	вернуть 0;

    если (get_fileformat(bufp) == EOL_DOS)
	eol_size = 2;
    иначе
	eol_size = 1;
    для (lnum = 1; lnum <= bufp->b_ml.ml_line_count; ++lnum)
    {
	char_count += ml_get_buf_len(bufp, lnum) + eol_size;
	// Check for a CTRL-C every 100000 characters
	если (char_count > last_check)
	{
	    ui_breakcheck();
	    если (got_int)
		вернуть char_count;
	    last_check = char_count + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    если (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_count -= eol_size;

    вернуть char_count;
}

/*
 * Convert character offset to lnum,col
 */
    static pos_T *
off2pos(buf_T *buf, long offset)
{
    linenr_T	 lnum;
    static pos_T pos;

    pos.lnum = 0;
    pos.col = 0;
    pos.coladd = 0;

    если (!(buf->b_ml.ml_flags & ML_EMPTY))
    {
	если ((lnum = ml_find_line_or_offset(buf, (linenr_T)0, &offset)) < 0)
	    вернуть NULL;
	pos.lnum = lnum;
	pos.col = offset;
    }

    вернуть &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    static pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    static pos_T	mypos;
    long		off;

    off = strtol((char *)*argp, (char **)argp, 10);
    если (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.col = strtol((char *)*argp, (char **)argp, 10);
	mypos.coladd = 0;
	вернуть &mypos;
    }
    вернуть off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    static long
pos2off(buf_T *buf, pos_T *pos)
{
    long	 offset = 0;

    если (buf->b_ml.ml_flags & ML_EMPTY)
	вернуть 0;

    если ((offset = ml_find_line_or_offset(buf, pos->lnum, 0)) < 0)
	вернуть 0;
    offset += pos->col;

    вернуть offset;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    static void
print_read_msg(nbbuf_T *buf)
{
    целое	    lnum = buf->bufp->b_ml.ml_line_count;
    off_T   nchars = buf->bufp->b_orig_size;
    char_u  c;

    msg_add_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    если (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    если (!buf->bufp->b_start_eol)
    {
	STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    msg_add_lines(c, (long)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_msg);
    msg_scrolled_ign = TRUE;
    msg_trunc_attr((char *)IObuff, FALSE, 0);
    msg_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    static void
print_save_msg(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    если (nchars >= 0)
    {
	// put fname in IObuff with quotes
	msg_add_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	msg_add_lines(c, buf->bufp->b_ml.ml_line_count,
						buf->bufp->b_orig_size);

	VIM_CLEAR(keep_msg);
	msg_scrolled_ign = TRUE;
	p = (char_u *)msg_trunc_attr((char *)IObuff, FALSE, 0);
	если ((msg_scrolled && !need_wait_return) || !buf->initDone)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_msg(p, 0);
	}
	msg_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    иначе
    {
	char msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_add_bang_to_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#endif // defined(FEAT_NETBEANS_INTG)
