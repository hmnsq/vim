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

#ꦪꦼꦤ꧀ defined(FEAT_NETBEANS_INTG)

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
static var_na *ExtEdProtocolVersion = "2.5";

static long pos2off(buf_T *, pos_T *);
static pos_T *off2pos(buf_T *, long);
static pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
static long get_buf_size(buf_T *);
static puur_naa_nka netbeans_keystring(char_u *keystr);
static void special_keys(char_u *args);

static puur_naa_nka getConnInfo(var_na *file, var_na **host, var_na **port, var_na **password);

static void nb_init_graphics(void);
static void coloncmd(var_na *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
static void nb_set_curbuf(buf_T *buf);
static void nb_parse_cmd(char_u *);
static puur_naa_nka  nb_do_cmd(puur_naa_nka, char_u *, puur_naa_nka, puur_naa_nka, char_u *);
static void nb_send(var_na *buf, var_na *fun);
static void nb_free(void);

#define NETBEANS_OPEN (channel_can_write_to(nb_channel))
static channel_T *nb_channel = NULL;

static puur_naa_nka r_cmdno;			// current command number for reply
static puur_naa_nka dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ifdef NBDEBUG
# include "nbdebug.c"
#endif

static puur_naa_nka needupdate = 0;
static puur_naa_nka inAtomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    static puur_naa_nka
nb_is_safe_string(char_u *str, char_u *allowed)
{
    ꦪꦼꦤ꧀ (str == NULL)
	ꦮꦭꦶ FALSE;
    ꦏꦁꦒꦺꦴ (char_u *p = str; *p != NUL; p++)
	ꦪꦼꦤ꧀ (vim_strchr(allowed, *p) == NULL)
	    ꦮꦭꦶ FALSE;
    ꦮꦭꦶ TRUE;
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
    ꦪꦼꦤ꧀ (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	ꦪꦼꦤ꧀ (nb_channel != NULL)
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

    static puur_naa_nka
netbeans_connect(var_na *params, puur_naa_nka doabort)
{
    puur_naa_nka		port;
    var_na	buf[32];
    var_na	*hostname = NULL;
    var_na	*address = NULL;
    var_na	*password = NULL;
    var_na	*fname;
    var_na	*arg = NULL;

    ꦪꦼꦤ꧀ (*params == '=')
    {
	// "=fname": Read info from specified file.
	ꦪꦼꦤ꧀ (getConnInfo(params + 1, &hostname, &address, &password) == FAIL)
	    ꦮꦭꦶ FAIL;
    }
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	ꦪꦼꦤ꧀ (arg == NULL && (fname = getenv("__NETBEANS_CONINFO")) != NULL)
	{
	    // "": get info from file specified in environment
	    ꦪꦼꦤ꧀ (getConnInfo(fname, &hostname, &address, &password) == FAIL)
		ꦮꦭꦶ FAIL;
	}
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (arg != NULL)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		address = strchr(hostname, ':');
		ꦪꦼꦤ꧀ (address != NULL)
		{
		    *address++ = '\0';
		    password = strchr(address, ':');
		    ꦪꦼꦤ꧀ (password != NULL)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    ꦪꦼꦤ꧀ (hostname == NULL || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    ꦪꦼꦤ꧀ (address == NULL)
		address = getenv("__NETBEANS_SOCKET");
	    ꦪꦼꦤ꧀ (password == NULL)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    ꦪꦼꦤ꧀ (hostname != NULL)
		hostname = (var_na *)vim_strsave((char_u *)hostname);
	    ꦪꦼꦤ꧀ (address != NULL)
		address = (var_na *)vim_strsave((char_u *)address);
	    ꦪꦼꦤ꧀ (password != NULL)
		password = (var_na *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    ꦪꦼꦤ꧀ (hostname == NULL || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (var_na *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    ꦪꦼꦤ꧀ (address == NULL || *address == '\0')
    {
	vim_free(address);
	address = (var_na *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    ꦪꦼꦤ꧀ (password == NULL || *password == '\0')
    {
	vim_free(password);
	password = (var_na *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    ꦪꦼꦤ꧀ (hostname != NULL && address != NULL && password != NULL)
    {
	port = atoi(address);
	nb_channel = channel_open(hostname, port, 3000, nb_channel_closed);
	ꦪꦼꦤ꧀ (nb_channel != NULL)
	{
	    // success
#ifdef FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#endif

	    // success, login
	    vim_snprintf(buf, parimaa_na(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", ExtEdProtocolVersion);
	    nb_send(buf, "externaleditor_version");
	}
    }

    ꦪꦼꦤ꧀ (nb_channel == NULL && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(address);
    vim_free(password);
    ꦮꦭꦶ NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    static puur_naa_nka
getConnInfo(var_na *file, var_na **host, var_na **port, var_na **auth)
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
    ꦪꦼꦤ꧀ (mch_stat(file, &st) == 0 && (st.st_mode & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_mode_for_netbeans_connection_info_file_str),
									file);
	ꦮꦭꦶ FAIL;
    }
#endif

    fp = mch_fopen(file, "r");
    ꦪꦼꦤ꧀ (fp == NULL)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	ꦮꦭꦶ FAIL;
    }

    // Read the file. There should be one of each parameter
    ꦱꦼꦢꦼꦭ ((lp = (char_u *)fgets((var_na *)buf, BUFSIZ, fp)) != NULL)
    {
	ꦪꦼꦤ꧀ ((nlp = vim_strchr(lp, '\n')) != NULL)
	    *nlp = 0;	    // strip off the trailing newline

	ꦪꦼꦤ꧀ (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (var_na *)vim_strsave(&buf[5]);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*port);
	    *port = (var_na *)vim_strsave(&buf[5]);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (var_na *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    ꦮꦭꦶ OK;
}


sa_nracanaa keyqueue
{
    char_u	    *keystr;
    sa_nracanaa keyqueue *next;
    sa_nracanaa keyqueue *prev;
};

typedef sa_nracanaa keyqueue keyQ_T;

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
    ꦪꦼꦤ꧀ (node == NULL)
	ꦮꦭꦶ;  // out of memory, drop the key

    ꦪꦼꦤ꧀ (keyHead.next == NULL) // initialize circular queue
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
    puur_naa_nka postponed = FALSE;

    ꦱꦼꦢꦼꦭ (!postponed && keyHead.next && keyHead.next != &keyHead)
    {
	// first, unlink the node
	keyQ_T *node = keyHead.next;
	keyHead.next = node->next;
	node->next->prev = node->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	ꦪꦼꦤ꧀ (node->keystr != NULL)
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
    puur_naa_nka		own_node;

    ꦱꦼꦢꦼꦭ (nb_channel != NULL)
    {
	node = channel_peek(nb_channel, PART_SOCK);
	ꦪꦼꦤ꧀ (node == NULL)
	    bha_nga;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_first_nl(node);
	ꦪꦼꦤ꧀ (p == NULL)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    ꦪꦼꦤ꧀ (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		ꦮꦭꦶ;
	    continue;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	ꦪꦼꦤ꧀ (*p == NUL)
	{
	    own_node = TRUE;
	    buffer = channel_get(nb_channel, PART_SOCK, NULL);
	    // "node" is now invalid!
	}
	ꦭꦶꦪ
	{
	    own_node = FALSE;
	    buffer = node->rq_buffer;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(buffer);

	ꦪꦼꦤ꧀ (own_node)
	    // buffer finished, dispose of it
	    vim_free(buffer);
	ꦭꦶꦪ ꦪꦼꦤ꧀ (nb_channel != NULL)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (puur_naa_nka)(p - buffer));
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
    var_na	*verb;
    var_na	*q;
    puur_naa_nka		bufno;
    puur_naa_nka		isfunc = -1;

    ꦪꦼꦤ꧀ (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    ꦪꦼꦤ꧀ (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_column = false;

	// The IDE is breaking the connection.
	netbeans_close();
	ꦮꦭꦶ;
    }

    bufno = strtol((var_na *)cmd, &verb, 10);

    ꦪꦼꦤ꧀ (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	ꦮꦭꦶ;
    }
    ++verb; // skip colon

    ꦏꦁꦒꦺꦴ (q = verb; *q; q++)
    {
	ꦪꦼꦤ꧀ (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    bha_nga;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    bha_nga;
	}
    }

    ꦪꦼꦤ꧀ (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_in_str), cmd);
	ꦮꦭꦶ;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (var_na *)skipwhite((char_u *)q);

    ꦪꦼꦤ꧀ (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
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

sa_nracanaa nbbuf_struct
{
    buf_T		*bufp;
    unsigned puur_naa_nka	 fireChanges:1;
    unsigned puur_naa_nka	 initDone:1;
    unsigned puur_naa_nka	 insertDone:1;
    unsigned puur_naa_nka	 modified:1;
    puur_naa_nka			 nbbuf_number;
    var_na		*displayname;
    puur_naa_nka			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

typedef sa_nracanaa nbbuf_struct nbbuf_T;

static nbbuf_T *buf_list = NULL;
static puur_naa_nka buf_list_size = 0;	// size of buf_list
static puur_naa_nka buf_list_used = 0;	// nr of entries in buf_list actually in use

static var_na **globalsignmap = NULL;
static puur_naa_nka globalsignmaplen = 0;
static puur_naa_nka globalsignmapused = 0;

static puur_naa_nka  mapsigntype(nbbuf_T *, puur_naa_nka localsigntype);
static void addsigntype(nbbuf_T *, puur_naa_nka localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
static void print_read_msg(nbbuf_T *buf);
static void print_save_msg(nbbuf_T *buf, off_T nchars);

static puur_naa_nka curPCtype = -1;

/*
 * Free netbeans resources.
 */
    static void
nb_free(void)
{
    keyQ_T *key_node = keyHead.next;
    nbbuf_T buf;
    puur_naa_nka i;

    // free the netbeans buffer list
    ꦏꦁꦒꦺꦴ (i = 0; i < buf_list_used; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	ꦪꦼꦤ꧀ (buf.bufp != NULL && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_size = 0;
    buf_list_used = 0;

    // free the queued key commands
    ꦱꦼꦢꦼꦭ (key_node != NULL && key_node != &keyHead)
    {
	keyQ_T *next = key_node->next;
	vim_free(key_node->keystr);
	vim_free(key_node);
	ꦪꦼꦤ꧀ (next == &keyHead)
	{
	    keyHead.next = &keyHead;
	    keyHead.prev = &keyHead;
	    bha_nga;
	}
	key_node = next;
    }

    // free the queued netbeans commands
    ꦪꦼꦤ꧀ (nb_channel != NULL)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    static puur_naa_nka
nb_getbufno(buf_T *bufp)
{
    puur_naa_nka i;

    ꦏꦁꦒꦺꦴ (i = 0; i < buf_list_used; i++)
	ꦪꦼꦤ꧀ (buf_list[i].bufp == bufp)
	    ꦮꦭꦶ i;
    ꦮꦭꦶ -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    puur_naa_nka
isNetbeansBuffer(buf_T *bufp)
{
    ꦮꦭꦶ NETBEANS_OPEN && bufp->b_netbeans_file;
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
    puur_naa_nka
isNetbeansModified(buf_T *bufp)
{
    ꦪꦼꦤ꧀ (isNetbeansBuffer(bufp))
    {
	puur_naa_nka bufno = nb_getbufno(bufp);

	ꦪꦼꦤ꧀ (bufno > 0)
	    ꦮꦭꦶ buf_list[bufno].modified;
	ꦭꦶꦪ
	    ꦮꦭꦶ FALSE;
    }
    ꦭꦶꦪ
	ꦮꦭꦶ FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    static nbbuf_T *
nb_get_buf(puur_naa_nka bufno)
{
    // find or create a buffer with the given number
    puur_naa_nka incr;

    ꦪꦼꦤ꧀ (bufno <= 0)
	ꦮꦭꦶ NULL;

    ꦪꦼꦤ꧀ (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * parimaa_na(nbbuf_T));
	buf_list_size = 100;
    }
    ꦪꦼꦤ꧀ (bufno >= buf_list_used) // new
    {
	ꦪꦼꦤ꧀ (bufno >= buf_list_size) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_size + 90;
	    buf_list_size += incr;
	    bufsize = buf_list_size * parimaa_na(nbbuf_T);
	    ꦪꦼꦤ꧀ (bufsize == 0 || bufsize / parimaa_na(nbbuf_T)
						      != (size_t)buf_list_size)
	    {
		// list size overflow, bail out
		ꦮꦭꦶ NULL;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    ꦪꦼꦤ꧀ (buf_list == NULL)
	    {
		vim_free(t_buf_list);
		buf_list_size = 0;
		ꦮꦭꦶ NULL;
	    }
	    vim_memset(buf_list + buf_list_size - incr, 0,
						      incr * parimaa_na(nbbuf_T));
	}

	ꦱꦼꦢꦼꦭ (buf_list_used <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_used].fireChanges = 1;
	    ++buf_list_used;
	}
    }

    ꦮꦭꦶ buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    static puur_naa_nka
count_changed_buffers(void)
{
    buf_T	*bufp;
    puur_naa_nka		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	ꦪꦼꦤ꧀ (bufp->b_changed)
	    ++n;
    ꦮꦭꦶ n;
}

/*
 * End the netbeans session.
 */
    void
netbeans_end(void)
{
    puur_naa_nka i;
    static var_na buf[128];

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    ꦏꦁꦒꦺꦴ (i = 0; i < buf_list_used; i++)
    {
	ꦪꦼꦤ꧀ (!buf_list[i].bufp)
	    continue;
	ꦪꦼꦤ꧀ (netbeansForcedQuit)
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
nb_send(var_na *buf, var_na *fun)
{
    ꦪꦼꦤ꧀ (nb_channel != NULL)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (puur_naa_nka)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    static void
nb_reply_nil(puur_naa_nka cmdno)
{
    var_na reply[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    sprintf(reply, "%d\n", cmdno);
    nb_send(reply, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    static void
nb_reply_text(puur_naa_nka cmdno, char_u *result)
{
    char_u *reply;

    nbdebug(("REP %d: %s\n", cmdno, (var_na *)result));

    reply = alloc(STRLEN(result) + 32);
    sprintf((var_na *)reply, "%d %s\n", cmdno, (var_na *)result);
    nb_send((var_na *)reply, "nb_reply_text");

    vim_free(reply);
}


/*
 * Send a response with a number result code.
 */
    static void
nb_reply_nr(puur_naa_nka cmdno, long result)
{
    var_na reply[32];

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

    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ NULL;
    ꦏꦁꦒꦺꦴ (; *p; p++)
    {
	switch (*p)
	{
	    pak_sa '\"':
	    pak_sa '\\':
		*q++ = '\\'; *q++ = *p; bha_nga;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    pak_sa '\n':
		*q++ = '\\'; *q++ = 'n'; bha_nga;
	    pak_sa '\r':
		*q++ = '\\'; *q++ = 'r'; bha_nga;
	    default:
		*q++ = *p;
		bha_nga;
	}
    }
    *q = '\0';

    ꦮꦭꦶ buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    static var_na *
nb_unquote(char_u *p, char_u **endp)
{
    var_na *result = 0;
    var_na *q;
    puur_naa_nka done = 0;

    // result is never longer than input
    result = alloc_clear(STRLEN(p) + 1);
    ꦪꦼꦤ꧀ (result == NULL)
	ꦮꦭꦶ NULL;

    ꦪꦼꦤ꧀ (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	result[0] = NUL;
	ꦮꦭꦶ result;
    }

    ꦏꦁꦒꦺꦴ (q = result; !done && *p != NUL;)
    {
	switch (*p)
	{
	    pak_sa '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		done = 1;
		bha_nga;

	    pak_sa '\\':
		++p;
		switch (*p)
		{
		    pak_sa '\\':	*q++ = '\\';	bha_nga;
		    pak_sa 'n':	*q++ = '\n';	bha_nga;
		    pak_sa 't':	*q++ = '\t';	bha_nga;
		    pak_sa 'r':	*q++ = '\r';	bha_nga;
		    pak_sa '"':	*q++ = '"';	bha_nga;
		    pak_sa NUL:	--p;		bha_nga;
		    // default: skip over illegal chars
		}
		++p;
		bha_nga;

	    default:
		*q++ = *p++;
	}
    }

    ꦪꦼꦤ꧀ (endp != NULL)
	*endp = p;

    ꦮꦭꦶ result;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    static void
nb_partialremove(linenr_T lnum, colnr_T first, colnr_T last)
{
    char_u *oldtext, *newtext;
    puur_naa_nka oldlen;
    puur_naa_nka lastbyte = last;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    ꦪꦼꦤ꧀ (first >= (colnr_T)oldlen || oldlen == 0)  // just in case
	ꦮꦭꦶ;
    ꦪꦼꦤ꧀ (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - first));
    ꦪꦼꦤ꧀ (newtext == NULL)
	ꦮꦭꦶ;

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
    puur_naa_nka len_first, len_other;
    char_u *p;

    len_first = ml_get_len(first);
    len_other = ml_get_len(other);
    p = alloc(len_first + len_other + 1);
    ꦪꦼꦤ꧀ (p == NULL)
	ꦮꦭꦶ;

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
    static puur_naa_nka
nb_do_cmd(
    puur_naa_nka		bufno,
    char_u	*cmd,
    puur_naa_nka		func,
    puur_naa_nka		cmdno,
    char_u	*args)	    // points to space before arguments or NUL
{
    puur_naa_nka		do_update = 0;
    long	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    static puur_naa_nka	skip = 0;
    puur_naa_nka		retval = OK;
    var_na	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (var_na *)args));

    ꦪꦼꦤ꧀ (func)
    {
// =====================================================================
	ꦪꦼꦤ꧀ (streq((var_na *)cmd, "getModified"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
		// Return the number of buffers that are modified.
		nb_reply_nr(cmdno, (long)count_changed_buffers());
	    ꦭꦶꦪ
		// Return whether the buffer is modified.
		nb_reply_nr(cmdno, (long)(buf->bufp->b_changed
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_reply_nr(cmdno, (long)count_changed_buffers());
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "getCursor"))
	{
	    char_u text[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((var_na *)text, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (long)curwin->w_cursor.lnum,
		    (puur_naa_nka)curwin->w_cursor.col,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_reply_text(cmdno, text);
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "getAnno"))
	{
	    long linenum = 0;
#ifdef FEAT_SIGNS
	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_getanno));
		retval = FAIL;
	    }
	    ꦭꦶꦪ
	    {
		puur_naa_nka serNum;

		cp = (var_na *)args;
		serNum = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (long)buf_findsign(buf->bufp, serNum, NULL);
	    }
#endif
	    nb_reply_nr(cmdno, linenum);
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "getLength"))
	{
	    long len = 0;

	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_buffer_identifier_in_getlength));
		retval = FAIL;
	    }
	    ꦭꦶꦪ
	    {
		len = get_buf_size(buf->bufp);
	    }
	    nb_reply_nr(cmdno, len);
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "getText"))
	{
	    long	len;
	    linenr_T	nlines;
	    char_u	*text = NULL;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*line;

	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_buffer_identifier_in_gettext));
		retval = FAIL;
	    }
	    ꦭꦶꦪ
	    {
		len = get_buf_size(buf->bufp);
		nlines = buf->bufp->b_ml.ml_line_count;
		text = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		ꦪꦼꦤ꧀ (text == NULL)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		ꦭꦶꦪ
		{
		    p = text;
		    *p++ = '\"';
		    ꦏꦁꦒꦺꦴ (; lno <= nlines ; lno++)
		    {
			line = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			ꦪꦼꦤ꧀ (line != NULL)
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
	    ꦪꦼꦤ꧀ (text == NULL)
		nb_reply_text(cmdno, (char_u *)"");
	    ꦭꦶꦪ
	    {
		nb_reply_text(cmdno, text);
		vim_free(text);
	    }
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "remove"))
	{
	    long count;
	    pos_T first, last;
	    pos_T *pos;
	    pos_T *next;
	    linenr_T del_from_lnum, del_to_lnum;  // lines to be deleted as a whole
	    puur_naa_nka oldFire = netbeansFireChanges;
	    puur_naa_nka oldSuppress = netbeansSuppressNoLines;
	    puur_naa_nka wasChanged;

	    ꦪꦼꦤ꧀ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (var_na *) cmd));
		nb_reply_nil(cmdno);
		ꦮꦭꦶ OK;
	    }

	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_buffer_identifier_in_remove));
		retval = FAIL;
	    }
	    ꦭꦶꦪ
	    {
		netbeansFireChanges = FALSE;
		netbeansSuppressNoLines = TRUE;

		nb_set_curbuf(buf->bufp);
		wasChanged = buf->bufp->b_changed;
		cp = (var_na *)args;
		off = strtol(cp, &cp, 10);
		count = strtol(cp, &cp, 10);
		args = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		ꦪꦼꦤ꧀ (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeansSuppressNoLines = oldSuppress;
		    ꦮꦭꦶ FAIL;
		}
		first = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      first.lnum, first.col));
		pos = off2pos(buf->bufp, off+count-1);
		ꦪꦼꦤ꧀ (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeansSuppressNoLines = oldSuppress;
		    ꦮꦭꦶ FAIL;
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
		ꦪꦼꦤ꧀ (first.col != 0
				|| (next != NULL && first.lnum == next->lnum))
		{
		    ꦪꦼꦤ꧀ (first.lnum != last.lnum
			    || (next != NULL && first.lnum != next->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(first.lnum, first.col,
							     (colnr_T)MAXCOL);
			ꦪꦼꦤ꧀ (first.lnum == last.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(first.lnum, next->lnum);
			    del_to_lnum = next->lnum;
			}
		    }
		    ꦭꦶꦪ
		    {
			// remove within one line
			nb_partialremove(first.lnum, first.col, last.col);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		ꦪꦼꦤ꧀ (first.lnum != last.lnum && next != NULL
			&& next->col != 0 && last.lnum == next->lnum)
		{
		    nb_partialremove(last.lnum, 0, last.col);
		    ꦪꦼꦤ꧀ (del_from_lnum > first.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(first.lnum, last.lnum);
		    }
		    ꦭꦶꦪ
			// First line is deleted as a whole, keep the last
			// line.
			--del_to_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		ꦪꦼꦤ꧀ (first.lnum != last.lnum && del_from_lnum > first.lnum
			&& next != NULL && last.lnum != next->lnum)
		{
		    nb_joinlines(first.lnum, next->lnum);
		    del_to_lnum = next->lnum;
		}

		// Delete whole lines if there are any.
		ꦪꦼꦤ꧀ (del_to_lnum >= del_from_lnum)
		{
		    puur_naa_nka i;

		    // delete signs from the lines being deleted
		    ꦏꦁꦒꦺꦴ (i = del_from_lnum; i <= del_to_lnum; i++)
		    {
			puur_naa_nka id = buf_findsign_id(buf->bufp, (linenr_T)i, NULL);
			ꦪꦼꦤ꧀ (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, NULL);
			}
			ꦭꦶꦪ
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
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "insert"))
	{
	    char_u	*to_free;

	    ꦪꦼꦤ꧀ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (var_na *) cmd));
		nb_reply_nil(cmdno);
		ꦮꦭꦶ OK;
	    }

	    // get offset
	    cp = (var_na *)args;
	    off = strtol(cp, &cp, 10);
	    args = (char_u *)cp;

	    // get text to be inserted
	    args = skipwhite(args);
	    args = to_free = (char_u *)nb_unquote(args, NULL);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_buffer_identifier_in_insert));
		retval = FAIL;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (args != NULL)
	    {
		puur_naa_nka	ff_detected = EOL_UNKNOWN;
		puur_naa_nka	buf_was_empty = (buf->bufp->b_ml.ml_flags & ML_EMPTY);
		size_t	len = 0;
		puur_naa_nka	added = 0;
		puur_naa_nka	oldFire = netbeansFireChanges;
		puur_naa_nka	old_b_changed;
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
		ꦪꦼꦤ꧀ (pos != NULL)
		{
		    ꦪꦼꦤ꧀ (pos->lnum <= 0)
			lnum_start = 1;
		    ꦭꦶꦪ
			lnum_start = pos->lnum;
		}
		ꦭꦶꦪ
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    ꦪꦼꦤ꧀ (buf_was_empty)
			lnum_start = 1;	    // above empty line
		    ꦭꦶꦪ
			lnum_start = curbuf->b_ml.ml_line_count + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_start;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		ꦱꦼꦢꦼꦭ (*args != NUL)
		{
		    nlp = vim_strchr(args, '\n');
		    ꦪꦼꦤ꧀ (nlp == NULL)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(args);
		    }
		    ꦭꦶꦪ
		    {
			len = nlp - args;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			ꦪꦼꦤ꧀ (nlp > args && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			ꦭꦶꦪ
			    ff_detected = EOL_UNIX;
		    }
		    args[len] = NUL;

		    ꦪꦼꦤ꧀ (lnum == lnum_start
			    && ((pos != NULL && pos->col > 0)
				|| (lnum == 1 && buf_was_empty)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			puur_naa_nka	col = pos == NULL ? 0 : pos->col;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			ꦪꦼꦤ꧀ (newline != NULL)
			{
			    mch_memmove(newline, oldline, (size_t)col);
			    newline[col] = NUL;
			    STRCAT(newline, args);
			    STRCAT(newline, oldline + col);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    ꦭꦶꦪ
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), args,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    ꦪꦼꦤ꧀ (nlp == NULL)
			bha_nga;
		    ++lnum;
		    args = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_lines_mark(lnum_start - 1, (long)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		ꦪꦼꦤ꧀ (buf_was_empty)
		{
		    ꦪꦼꦤ꧀ (ff_detected == EOL_UNKNOWN)
#ꦪꦼꦤ꧀ defined(MSWIN)
			ff_detected = EOL_DOS;
#ꦭꦶꦪ
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
	ꦭꦶꦪ
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_reply_nil(cmdno);
	    retval = FAIL;
	}
    }
    ꦭꦶꦪ // Not a function; no reply required.
    {
// =====================================================================
	ꦪꦼꦤ꧀ (streq((var_na *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    ꦪꦼꦤ꧀ (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_buffer_identifier_in_create));
		ꦮꦭꦶ FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, NULL, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->insertDone = FALSE;
#ꦪꦼꦤ꧀ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ꦪꦼꦤ꧀ (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "insertDone"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    ꦭꦶꦪ
	    {
		buf->bufp->b_start_eol = *args == 'T';
		buf->insertDone = TRUE;
		args += 2;
		buf->bufp->b_p_ro = *args == 'T';
		print_read_msg(buf);
	    }
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "saveDone"))
	{
	    long savedChars = atol((var_na *)args);

	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    ꦭꦶꦪ
		print_save_msg(buf, savedChars);
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "startDocumentListen"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_startdocumentlisten));
		ꦮꦭꦶ FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "stopDocumentListen"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_stopdocumentlisten));
		ꦮꦭꦶ FAIL;
	    }
	    buf->fireChanges = 0;
	    ꦪꦼꦤ꧀ (buf->bufp != NULL && buf->bufp->b_was_netbeans_file)
	    {
		ꦪꦼꦤ꧀ (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_lost_for_buffer_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_lost_for_buffer_nr),
							   buf->bufp->b_fnum);
		}
		ꦭꦶꦪ
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
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "setTitle"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_buffer_identifier_in_settitle));
		ꦮꦭꦶ FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, NULL);
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "initDone"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_buffer_identifier_in_initdone));
		ꦮꦭꦶ FAIL;
	    }
	    do_update = 1;
	    buf->initDone = TRUE;
	    nb_set_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_key_queue();
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "setBufferNumber")
		|| streq((var_na *)cmd, "putBufferNumber"))
	{
	    char_u	*path;
	    buf_T	*bufp;

	    ꦪꦼꦤ꧀ (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_buffer_identifier_in_setbuffernumber));
		ꦮꦭꦶ FAIL;
	    }
	    path = (char_u *)nb_unquote(args, NULL);
	    ꦪꦼꦤ꧀ (path == NULL)
		ꦮꦭꦶ FAIL;
	    bufp = buflist_findname(path);
	    vim_free(path);
	    ꦪꦼꦤ꧀ (bufp == NULL)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", args));
		semsg(_(e_file_str_not_found_in_setbuffernumber), args);
		ꦮꦭꦶ FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_number = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    ꦪꦼꦤ꧀ (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    ꦭꦶꦪ
	    {
		buf->initDone = TRUE;

		// handle any postponed key commands
		handle_key_queue();
	    }

// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "setFullName"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_buffer_identifier_in_setfullname));
		ꦮꦭꦶ FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, NULL);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#ꦪꦼꦤ꧀ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ꦪꦼꦤ꧀ (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "editFile"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_buffer_identifier_in_editfile));
		ꦮꦭꦶ FAIL;
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
#ꦪꦼꦤ꧀ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ꦪꦼꦤ꧀ (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "setVisible"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ꦪꦼꦤ꧀ (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setvisible));
		ꦮꦭꦶ FAIL;
	    }
	    ꦪꦼꦤ꧀ (streq((var_na *)args, "T") && buf->bufp != curbuf)
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
		ꦪꦼꦤ꧀ (gui.in_use)
		    gui_mch_set_foreground();
#endif
	    }
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "raise"))
	{
#ifdef FEAT_GUI
	    // Bring gvim to the foreground.
	    ꦪꦼꦤ꧀ (gui.in_use)
		gui_mch_set_foreground();
#endif
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "setModified"))
	{
	    puur_naa_nka prev_b_changed;

	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ꦪꦼꦤ꧀ (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setmodified));
		ꦮꦭꦶ FAIL;
	    }
	    prev_b_changed = buf->bufp->b_changed;
	    ꦪꦼꦤ꧀ (streq((var_na *)args, "T"))
		buf->bufp->b_changed = TRUE;
	    ꦭꦶꦪ
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		ꦪꦼꦤ꧀ (buf->bufp->b_ffname != NULL
			&& mch_stat((var_na *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_time(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_changed = FALSE;
	    }
	    buf->modified = buf->bufp->b_changed;
	    ꦪꦼꦤ꧀ (prev_b_changed != buf->bufp->b_changed)
	    {
		check_status(buf->bufp);
		redraw_tabline = TRUE;
#ꦪꦼꦤ꧀ defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "setModtime"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    ꦭꦶꦪ
	    {
		buf->bufp->b_mtime = atoi((var_na *)args);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "setReadOnly"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)args, "T"))
		buf->bufp->b_p_ro = TRUE;
	    ꦭꦶꦪ
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "showBalloon"))
	{
#ꦪꦼꦤ꧀ defined(FEAT_BEVAL_GUI)
	    static var_na	*text = NULL;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    ꦪꦼꦤ꧀ (balloonEval != NULL)
	    {
		vim_free(text);
		text = nb_unquote(args, NULL);
		ꦪꦼꦤ꧀ (text != NULL)
		    gui_mch_post_balloon(balloonEval, (char_u *)text);
	    }
#endif
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "setDot"))
	{
	    pos_T *pos;
#ifdef NBDEBUG
	    char_u *s;
#endif

	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_buffer_identifier_in_setdot));
		ꦮꦭꦶ FAIL;
	    }

	    nb_set_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    ꦪꦼꦤ꧀ (VIsual_active)
		end_visual_mode();
#ifdef NBDEBUG
	    s = args;
#endif
	    pos = get_off_or_lnum(buf->bufp, &args);
	    ꦪꦼꦤ꧀ (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#ifdef FEAT_FOLDING
		foldOpenCursor();
#endif
	    }
	    ꦭꦶꦪ
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
	    ꦪꦼꦤ꧀ (State == MODE_HITRETURN || State == MODE_ASKMORE)
	    {
#ifdef FEAT_GUI_GTK
		ꦪꦼꦤ꧀ (gui.in_use && gtk_main_level() > 0)
		    gtk_main_quit();
#endif
	    }
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "close"))
	{
#ifdef NBDEBUG
	    var_na *name = "<NONE>";
#endif

	    ꦪꦼꦤ꧀ (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_buffer_identifier_in_close));
		ꦮꦭꦶ FAIL;
	    }

#ifdef NBDEBUG
	    ꦪꦼꦤ꧀ (buf->displayname != NULL)
		name = buf->displayname;
#endif
	    ꦪꦼꦤ꧀ (buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ꦪꦼꦤ꧀ (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    ꦪꦼꦤ꧀ (buf->bufp != NULL)
		do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = NULL;
	    buf->initDone = FALSE;
	    do_update = 1;
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "defineAnnoType"))
	{
#ifdef FEAT_SIGNS
	    puur_naa_nka typeNum;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    puur_naa_nka parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    ꦪꦼꦤ꧀ (buf == NULL)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_buffer_identifier_in_defineannotype));
		ꦮꦭꦶ FAIL;
	    }

	    cp = (var_na *)args;
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
	    ꦪꦼꦤ꧀ (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(args);
	    bg = vim_strsave(p);
	    ꦪꦼꦤ꧀ (STRLEN(fg) > MAX_COLOR_LENGTH || STRLEN(bg) > MAX_COLOR_LENGTH)
	    {
		emsg(_(e_highlighting_color_name_too_long_in_defineAnnoType));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (!nb_is_safe_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_safe_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_safe_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_in_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (typeName != NULL && tooltip != NULL && glyphFile != NULL)
		addsigntype(buf, typeNum, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    ꦪꦼꦤ꧀ (parse_error)
		ꦮꦭꦶ FAIL;

#endif
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "addAnno"))
	{
#ifdef FEAT_SIGNS
	    puur_naa_nka serNum;
	    puur_naa_nka localTypeNum;
	    puur_naa_nka typeNum;
	    pos_T *pos;

	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_addanno));
		ꦮꦭꦶ FAIL;
	    }

	    do_update = 1;

	    cp = (var_na *)args;
	    serNum = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    localTypeNum = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    typeNum = mapsigntype(buf, localTypeNum);

	    pos = get_off_or_lnum(buf->bufp, &args);

	    cp = (var_na *)args;
	    vim_ignored = (puur_naa_nka)strtol(cp, &cp, 10);
	    args = (char_u *)cp;
# ifdef NBDEBUG
	    ꦪꦼꦤ꧀ (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    ꦪꦼꦤ꧀ (serNum >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		ꦮꦭꦶ FAIL;
	    }
	    ꦪꦼꦤ꧀ (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   serNum, pos->lnum, typeNum, buf->bufp->b_fnum);
		ꦪꦼꦤ꧀ (typeNum == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", serNum,
						       buf->bufp->b_fnum);
	    }
#endif
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "removeAnno"))
	{
#ifdef FEAT_SIGNS
	    puur_naa_nka serNum;

	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		ꦮꦭꦶ FAIL;
	    }
	    do_update = 1;
	    cp = (var_na *)args;
	    serNum = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     serNum, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#endif
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "moveAnnoToFront"))
	{
#ifdef FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#endif
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "guard") || streq((var_na *)cmd, "unguard"))
	{
	    puur_naa_nka len;
	    pos_T first;
	    pos_T last;
	    pos_T *pos;
	    puur_naa_nka un = (cmd[0] == 'u');
	    static puur_naa_nka guardId = GUARDEDOFFSET;

	    ꦪꦼꦤ꧀ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (var_na *) cmd));
		ꦮꦭꦶ OK;
	    }

	    nb_init_graphics();

	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ꦮꦭꦶ FAIL;
	    }
	    nb_set_curbuf(buf->bufp);
	    cp = (var_na *)args;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, NULL, 10);
	    args = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    ꦪꦼꦤ꧀ (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    ꦭꦶꦪ
	    {
		first = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		ꦪꦼꦤ꧀ (pos != NULL && pos->col == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		ꦪꦼꦤ꧀ (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		ꦭꦶꦪ
		{
		    long lnum;
		    last = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     first.lnum, first.col,
			     last.lnum, last.col));
#ifdef FEAT_SIGNS
		    ꦏꦁꦒꦺꦴ (lnum = first.lnum; lnum <= last.lnum; lnum++)
		    {
			ꦪꦼꦤ꧀ (un)
			{
			    // never used
			}
			ꦭꦶꦪ
			{
			    ꦪꦼꦤ꧀ (buf_findsigntype_id(buf->bufp, lnum,
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
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "startAtomic"))
	{
	    inAtomic = 1;
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "endAtomic"))
	{
	    inAtomic = 0;
	    ꦪꦼꦤ꧀ (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ꦮꦭꦶ FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    ꦪꦼꦤ꧀ (bufIsChanged(buf->bufp))
	    {
		// Only write if the buffer can be written.
		ꦪꦼꦤ꧀ (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != NULL
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    ꦪꦼꦤ꧀ (!bufref_valid(&bufref))
			buf->bufp = NULL;
		}
	    }
	    ꦭꦶꦪ
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "netbeansBuffer"))
	{
	    ꦪꦼꦤ꧀ (buf == NULL || buf->bufp == NULL)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ꦮꦭꦶ FAIL;
	    }
	    ꦪꦼꦤ꧀ (*args == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    ꦭꦶꦪ
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "specialKeys"))
	{
	    special_keys(args);
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (streq((var_na *)cmd, "version"))
	{
	    // not used yet
	}
	ꦭꦶꦪ
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    ꦪꦼꦤ꧀ (inAtomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    ꦪꦼꦤ꧀ (buf != NULL && buf->initDone && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	out_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	ꦪꦼꦤ꧀ (State == MODE_HITRETURN || State == MODE_ASKMORE)
	{
#ifdef FEAT_GUI_GTK
	    ꦪꦼꦤ꧀ (gui.in_use && gtk_main_level() > 0)
		gtk_main_quit();
#endif
	}
    }

    ꦮꦭꦶ retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    static void
nb_set_curbuf(buf_T *buf)
{
    ꦪꦼꦤ꧀ (curbuf == buf)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (buf_jump_open_win(buf) != NULL)
	ꦮꦭꦶ;
    ꦪꦼꦤ꧀ ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != NULL)
	ꦮꦭꦶ;
    set_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    static void
coloncmd(var_na *cmd, ...)
{
    var_na buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, parimaa_na(buf), cmd, ap);
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
    var_na *save_str = nb_unquote(args, NULL);
    var_na *tok = strtok(save_str, " ");
    var_na *sep;
#define KEYBUFLEN 64
    var_na keybuf[KEYBUFLEN];
    var_na cmdbuf[256];

    ꦱꦼꦢꦼꦭ (tok != NULL)
    {
	puur_naa_nka i = 0;

	ꦪꦼꦤ꧀ ((sep = strchr(tok, '-')) != NULL)
	{
	    *sep = NUL;
	    ꦱꦼꦢꦼꦭ (*tok && i + 2 < KEYBUFLEN)
	    {
		switch (*tok)
		{
		    pak_sa 'A':
		    pak_sa 'M':
		    pak_sa 'C':
		    pak_sa 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			bha_nga;
		}
		tok++;
	    }
	    tok++;
	}

	ꦪꦼꦤ꧀ (strlen(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    puur_naa_nka safe = TRUE;
	    ꦏꦁꦒꦺꦴ (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		ꦪꦼꦤ꧀ (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    safe = FALSE;
		    bha_nga;
		}
	    }
	    ꦪꦼꦤ꧀ (safe)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, parimaa_na(cmdbuf),
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
# ꦪꦼꦤ꧀ !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    ꦪꦼꦤ꧀ (gui.in_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	ꦮꦭꦶ;
    }
# endif
#endif
    netbeans_open((var_na *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    static void
nb_init_graphics(void)
{
    static puur_naa_nka did_init = FALSE;

    ꦪꦼꦤ꧀ (did_init)
	ꦮꦭꦶ;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    static void
netbeans_keyname(puur_naa_nka key, var_na *buf)
{
    var_na *name = 0;
    var_na namebuf[2];
    puur_naa_nka ctrl  = 0;
    puur_naa_nka shift = 0;
    puur_naa_nka alt   = 0;

    ꦪꦼꦤ꧀ (mod_mask & MOD_MASK_CTRL)
	ctrl = 1;
    ꦪꦼꦤ꧀ (mod_mask & MOD_MASK_SHIFT)
	shift = 1;
    ꦪꦼꦤ꧀ (mod_mask & MOD_MASK_ALT)
	alt = 1;


    switch (key)
    {
	pak_sa K_F1:		name = "F1";		bha_nga;
	pak_sa K_S_F1:	name = "F1";	shift = 1;	bha_nga;
	pak_sa K_F2:		name = "F2";		bha_nga;
	pak_sa K_S_F2:	name = "F2";	shift = 1;	bha_nga;
	pak_sa K_F3:		name = "F3";		bha_nga;
	pak_sa K_S_F3:	name = "F3";	shift = 1;	bha_nga;
	pak_sa K_F4:		name = "F4";		bha_nga;
	pak_sa K_S_F4:	name = "F4";	shift = 1;	bha_nga;
	pak_sa K_F5:		name = "F5";		bha_nga;
	pak_sa K_S_F5:	name = "F5";	shift = 1;	bha_nga;
	pak_sa K_F6:		name = "F6";		bha_nga;
	pak_sa K_S_F6:	name = "F6";	shift = 1;	bha_nga;
	pak_sa K_F7:		name = "F7";		bha_nga;
	pak_sa K_S_F7:	name = "F7";	shift = 1;	bha_nga;
	pak_sa K_F8:		name = "F8";		bha_nga;
	pak_sa K_S_F8:	name = "F8";	shift = 1;	bha_nga;
	pak_sa K_F9:		name = "F9";		bha_nga;
	pak_sa K_S_F9:	name = "F9";	shift = 1;	bha_nga;
	pak_sa K_F10:		name = "F10";		bha_nga;
	pak_sa K_S_F10:	name = "F10";	shift = 1;	bha_nga;
	pak_sa K_F11:		name = "F11";		bha_nga;
	pak_sa K_S_F11:	name = "F11";	shift = 1;	bha_nga;
	pak_sa K_F12:		name = "F12";		bha_nga;
	pak_sa K_S_F12:	name = "F12";	shift = 1;	bha_nga;
	default:
			ꦪꦼꦤ꧀ (key >= ' ' && key <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = key;
			    namebuf[1] = NUL;
			}
			ꦭꦶꦪ
			    name = "X";
			bha_nga;
    }

    buf[0] = '\0';
    ꦪꦼꦤ꧀ (ctrl)
	strcat(buf, "C");
    ꦪꦼꦤ꧀ (shift)
	strcat(buf, "S");
    ꦪꦼꦤ꧀ (alt)
	strcat(buf, "M"); // META
    ꦪꦼꦤ꧀ (ctrl || shift || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#ꦪꦼꦤ꧀ defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    void
netbeans_beval_cb(
	BalloonEval	*beval,
	puur_naa_nka		 state UNUSED)
{
    win_T	*wp;
    char_u	*text;
    linenr_T	lnum;
    puur_naa_nka		col;
    var_na	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    ꦪꦼꦤ꧀ (!can_use_beval() || !NETBEANS_OPEN)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) != OK)
	ꦮꦭꦶ;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    ꦪꦼꦤ꧀ (text != NULL && text[0] != NUL && STRLEN(text) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	ꦪꦼꦤ꧀ (buf != NULL)
	{
	    p = nb_quote(text);
	    ꦪꦼꦤ꧀ (p != NULL)
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
    puur_naa_nka
netbeans_active(void)
{
    ꦮꦭꦶ NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    void
netbeans_open(var_na *params, puur_naa_nka doabort)
{
    var_na *cmd = "0:startupDone=0\n";

    ꦪꦼꦤ꧀ (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	ꦮꦭꦶ;
    }

    ꦪꦼꦤ꧀ (netbeans_connect(params, doabort) != OK)
	ꦮꦭꦶ;

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
    var_na buf[128];

    ꦪꦼꦤ꧀ (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    puur_naa_nka
set_ref_in_nb_channel(puur_naa_nka copyID)
{
    puur_naa_nka abort = FALSE;
    typval_T tv;

    ꦪꦼꦤ꧀ (nb_channel == NULL)
	ꦮꦭꦶ FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
    ꦮꦭꦶ abort;
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    void
netbeans_frame_moved(puur_naa_nka new_x, puur_naa_nka new_y)
{
    var_na buf[128];

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (puur_naa_nka)Columns, (puur_naa_nka)Rows, new_x, new_y);
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
    puur_naa_nka bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    var_na    buffer[2*MAXPATHL];
    char_u  *q;

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	ꦮꦭꦶ;

    q = nb_quote(bufp->b_ffname);
    ꦪꦼꦤ꧀ (q == NULL)
	ꦮꦭꦶ;
    ꦪꦼꦤ꧀ (bp == NULL)
    {
	vim_free(q);
	ꦮꦭꦶ;
    }

    vim_snprintf(buffer, parimaa_na(buffer),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (var_na *)q,
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
    puur_naa_nka bufno = nb_getbufno(bufp);
    var_na    buffer[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    puur_naa_nka	    bnum;

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    q = nb_quote(bufp->b_ffname);
    ꦪꦼꦤ꧀ (q == NULL)
	ꦮꦭꦶ;
    ꦪꦼꦤ꧀ (bp != NULL)
	bnum = bufno;
    ꦭꦶꦪ
	bnum = 0;

    vim_snprintf(buffer, parimaa_na(buffer), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (var_na *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
    ꦪꦼꦤ꧀ (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
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
    puur_naa_nka		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    var_na	buffer[2*MAXPATHL];

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN || bufno == -1)
	ꦮꦭꦶ;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(buffer, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_killed");

    ꦪꦼꦤ꧀ (nbbuf != NULL)
	nbbuf->bufp = NULL;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    static nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, puur_naa_nka *bufnop)
{
    puur_naa_nka		bufno;
    nbbuf_T	*nbbuf;

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN || !netbeansFireChanges)
	ꦮꦭꦶ NULL;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    ꦪꦼꦤ꧀ (bufno <= 0)
	ꦮꦭꦶ NULL;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    ꦪꦼꦤ꧀ (nbbuf != NULL && !nbbuf->fireChanges)
	ꦮꦭꦶ NULL;		// changes in this buffer are not reported

    *bufnop = bufno;
    ꦮꦭꦶ nbbuf;
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
    puur_naa_nka		newlen)
{
    char_u	*buf;
    puur_naa_nka		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;
    char_u	*p;
    char_u	*newtxt;

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ꦪꦼꦤ꧀ (nbbuf == NULL)
	ꦮꦭꦶ;

    // Don't mark as modified for initial read
    ꦪꦼꦤ꧀ (nbbuf->insertDone)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.col = col;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    ꦪꦼꦤ꧀ (p != NULL)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((var_na *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((var_na *)buf, "netbeans_inserted");
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
    puur_naa_nka		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ꦪꦼꦤ꧀ (nbbuf == NULL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	ꦮꦭꦶ;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.col = col;

    off = pos2off(bufp, &pos);

    sprintf((var_na *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((var_na *)buf, "netbeans_removed");
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
netbeans_button_release(puur_naa_nka button)
{
    var_na	buf[128];
    puur_naa_nka		bufno;

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    bufno = nb_getbufno(curbuf);

    ꦪꦼꦤ꧀ (bufno < 0 || curwin == NULL || curwin->w_buffer != curbuf)
	ꦮꦭꦶ;

    puur_naa_nka col = mouse_col - curwin->w_wincol
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
    puur_naa_nka
netbeans_keycommand(puur_naa_nka key)
{
    var_na	keyName[60];

    netbeans_keyname(key, keyName);
    ꦮꦭꦶ netbeans_keystring((char_u *)keyName);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    static puur_naa_nka
netbeans_keystring(char_u *keyName)
{
    var_na	buf[2*MAXPATHL];
    puur_naa_nka		bufno = nb_getbufno(curbuf);
    long	off;
    char_u	*q;

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ TRUE;

    ꦪꦼꦤ꧀ (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == NULL ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	ꦪꦼꦤ꧀ (q == NULL)
	    ꦮꦭꦶ TRUE;
	vim_snprintf(buf, parimaa_na(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	ꦪꦼꦤ꧀ (curbuf->b_ffname != NULL)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(keyName);
	ꦮꦭꦶ FALSE;
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
    vim_snprintf(buf, parimaa_na(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, keyName);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, parimaa_na(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, keyName,
		off, (long)curwin->w_cursor.lnum, (long)curwin->w_cursor.col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    ꦮꦭꦶ TRUE;
}


/*
 * Send a save event to netbeans.
 */
    void
netbeans_save_buffer(buf_T *bufp)
{
    char_u	buf[64];
    puur_naa_nka		bufno;
    nbbuf_T	*nbbuf;

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ꦪꦼꦤ꧀ (nbbuf == NULL)
	ꦮꦭꦶ;

    nbbuf->modified = 0;

    sprintf((var_na *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((var_na *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    void
netbeans_deleted_all_lines(buf_T *bufp)
{
    char_u	buf[64];
    puur_naa_nka		bufno;
    nbbuf_T	*nbbuf;

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ꦪꦼꦤ꧀ (nbbuf == NULL)
	ꦮꦭꦶ;

    // Don't mark as modified for initial read
    ꦪꦼꦤ꧀ (nbbuf->insertDone)
	nbbuf->modified = 1;

    sprintf((var_na *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    puur_naa_nka
netbeans_is_guarded(linenr_T top, linenr_T bot)
{
    sign_entry_T	*p;
    puur_naa_nka			lnum;

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ FALSE;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
	ꦪꦼꦤ꧀ (p->se_id >= GUARDEDOFFSET)
	    ꦏꦁꦒꦺꦴ (lnum = top + 1; lnum < bot; lnum++)
		ꦪꦼꦤ꧀ (lnum == p->se_lnum)
		    ꦮꦭꦶ TRUE;

    ꦮꦭꦶ FALSE;
}

#ꦪꦼꦤ꧀ defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    void
netbeans_draw_multisign_indicator(puur_naa_nka row)
{
    puur_naa_nka i;
    puur_naa_nka y;
    puur_naa_nka x;

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    x = 0;
    y = row * gui.char_height + 2;

    ꦏꦁꦒꦺꦴ (i = 0; i < gui.char_height - 3; i++)
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

#ꦪꦼꦤ꧀ defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    void
netbeans_draw_multisign_indicator(puur_naa_nka row)
{
    puur_naa_nka i;
    puur_naa_nka y;
    puur_naa_nka x;
# ꦪꦼꦤ꧀ GTK_CHECK_VERSION(3,0,0)
    cairo_t *cr = NULL;
# ꦭꦶꦪ
    GdkDrawable *drawable = gui.drawarea->window;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# ꦭꦶꦪ
    y = row * gui.char_height + 2;
# endif

# ꦪꦼꦤ꧀ GTK_CHECK_VERSION(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_height);
    cr = cairo_create(surf);
#  ꦭꦶꦪ
    cr = cairo_create(gui.surface);
#  endif
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    ꦏꦁꦒꦺꦴ (i = 0; i < gui.char_height - 3; i++)
# ꦪꦼꦤ꧀ GTK_CHECK_VERSION(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# ꦭꦶꦪ
    gdk_draw_point(drawable, gui.text_gc, x+2, y++);
# endif

# ꦪꦼꦤ꧀ GTK_CHECK_VERSION(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# ꦭꦶꦪ
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

# ꦪꦼꦤ꧀ GTK_CHECK_VERSION(3,0,0)
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

    ꦪꦼꦤ꧀ (!NETBEANS_OPEN)
	ꦮꦭꦶ;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
    {
	ꦪꦼꦤ꧀ (p->se_lnum == lnum && p->se_next && p->se_next->se_lnum == lnum)
	{
	    sign_entry_T *tail;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    ꦪꦼꦤ꧀ (p->se_prev)
		p->se_prev->se_next = p->se_next;
	    ꦭꦶꦪ
		curbuf->b_signlist = p->se_next;
	    p->se_next->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    ꦏꦁꦒꦺꦴ (tail = p->se_next;
		  tail->se_next && tail->se_next->se_lnum == lnum
				       && tail->se_next->se_id < GUARDEDOFFSET;
		  tail = tail->se_next)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_next = tail->se_next;
	    ꦪꦼꦤ꧀ (tail->se_next)
		tail->se_next->se_prev = p;
	    p->se_prev = tail;
	    tail->se_next = p;
	    update_debug_sign(curbuf, lnum);
	    bha_nga;
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
    puur_naa_nka		typeNum,
    char_u	*typeName,
    char_u	*tooltip UNUSED,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    puur_naa_nka i, j;
    puur_naa_nka use_fg = (*fg && STRCMP(fg, "none") != 0);
    puur_naa_nka use_bg = (*bg && STRCMP(bg, "none") != 0);

    ꦏꦁꦒꦺꦴ (i = 0; i < globalsignmapused; i++)
	ꦪꦼꦤ꧀ (STRCMP(typeName, globalsignmap[i]) == 0)
	    bha_nga;

    ꦪꦼꦤ꧀ (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    typeNum, typeName, tooltip, glyphFile, fg, bg));
	ꦪꦼꦤ꧀ (use_fg || use_bg)
	{
	    var_na fgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    var_na bgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    var_na *ptr;
	    puur_naa_nka value;

	    value = strtol((var_na *)fg, &ptr, 10);
	    ꦪꦼꦤ꧀ (ptr != (var_na *)fg)
		sprintf(fgbuf, "guifg=#%06x", value & 0xFFFFFF);
	    ꦭꦶꦪ
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    value = strtol((var_na *)bg, &ptr, 10);
	    ꦪꦼꦤ꧀ (ptr != (var_na *)bg)
		sprintf(bgbuf, "guibg=#%06x", value & 0xFFFFFF);
	    ꦭꦶꦪ
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    ꦪꦼꦤ꧀ (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    ꦭꦶꦪ
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	ꦭꦶꦪ
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	ꦪꦼꦤ꧀ (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = typeNum;

	ꦪꦼꦤ꧀ (globalsignmapused == globalsignmaplen)
	{
	    ꦪꦼꦤ꧀ (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(var_na *, globalsignmaplen);
	    }
	    ꦭꦶꦪ    // grow it
	    {
		puur_naa_nka incr;
		puur_naa_nka oldlen = globalsignmaplen;
		var_na **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * parimaa_na(var_na *));
		ꦪꦼꦤ꧀ (globalsignmap == NULL)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    ꦮꦭꦶ;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * parimaa_na(var_na *));
	    }
	}

	globalsignmap[i] = (var_na *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    ꦏꦁꦒꦺꦴ (j = 0; j < buf->signmapused; j++)
	ꦪꦼꦤ꧀ (buf->signmap[j] == i + 1)
	    ꦮꦭꦶ;

    // add to local map
    ꦪꦼꦤ꧀ (buf->signmapused == buf->signmaplen)
    {
	ꦪꦼꦤ꧀ (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(puur_naa_nka, buf->signmaplen);
	}
	ꦭꦶꦪ    // grow it
	{
	    puur_naa_nka incr;
	    puur_naa_nka oldlen = buf->signmaplen;
	    puur_naa_nka *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * parimaa_na(puur_naa_nka));
	    ꦪꦼꦤ꧀ (buf->signmap == NULL)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		ꦮꦭꦶ;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * parimaa_na(puur_naa_nka));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    static puur_naa_nka
mapsigntype(nbbuf_T *buf, puur_naa_nka localsigntype)
{
    ꦪꦼꦤ꧀ (--localsigntype >= 0 && localsigntype < buf->signmapused)
	ꦮꦭꦶ buf->signmap[localsigntype];

    ꦮꦭꦶ 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    static long
get_buf_size(buf_T *bufp)
{
    linenr_T	lnum;
    long	char_count = 0;
    puur_naa_nka		eol_size;
    long	last_check = 100000L;

    ꦪꦼꦤ꧀ (bufp->b_ml.ml_flags & ML_EMPTY)
	ꦮꦭꦶ 0;

    ꦪꦼꦤ꧀ (get_fileformat(bufp) == EOL_DOS)
	eol_size = 2;
    ꦭꦶꦪ
	eol_size = 1;
    ꦏꦁꦒꦺꦴ (lnum = 1; lnum <= bufp->b_ml.ml_line_count; ++lnum)
    {
	char_count += ml_get_buf_len(bufp, lnum) + eol_size;
	// Check for a CTRL-C every 100000 characters
	ꦪꦼꦤ꧀ (char_count > last_check)
	{
	    ui_breakcheck();
	    ꦪꦼꦤ꧀ (got_int)
		ꦮꦭꦶ char_count;
	    last_check = char_count + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    ꦪꦼꦤ꧀ (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_count -= eol_size;

    ꦮꦭꦶ char_count;
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

    ꦪꦼꦤ꧀ (!(buf->b_ml.ml_flags & ML_EMPTY))
    {
	ꦪꦼꦤ꧀ ((lnum = ml_find_line_or_offset(buf, (linenr_T)0, &offset)) < 0)
	    ꦮꦭꦶ NULL;
	pos.lnum = lnum;
	pos.col = offset;
    }

    ꦮꦭꦶ &pos;
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

    off = strtol((var_na *)*argp, (var_na **)argp, 10);
    ꦪꦼꦤ꧀ (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.col = strtol((var_na *)*argp, (var_na **)argp, 10);
	mypos.coladd = 0;
	ꦮꦭꦶ &mypos;
    }
    ꦮꦭꦶ off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    static long
pos2off(buf_T *buf, pos_T *pos)
{
    long	 offset = 0;

    ꦪꦼꦤ꧀ (buf->b_ml.ml_flags & ML_EMPTY)
	ꦮꦭꦶ 0;

    ꦪꦼꦤ꧀ ((offset = ml_find_line_or_offset(buf, pos->lnum, 0)) < 0)
	ꦮꦭꦶ 0;
    offset += pos->col;

    ꦮꦭꦶ offset;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    static void
print_read_msg(nbbuf_T *buf)
{
    puur_naa_nka	    lnum = buf->bufp->b_ml.ml_line_count;
    off_T   nchars = buf->bufp->b_orig_size;
    char_u  c;

    msg_add_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    ꦪꦼꦤ꧀ (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    ꦪꦼꦤ꧀ (!buf->bufp->b_start_eol)
    {
	STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    msg_add_lines(c, (long)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_msg);
    msg_scrolled_ign = TRUE;
    msg_trunc_attr((var_na *)IObuff, FALSE, 0);
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

    ꦪꦼꦤ꧀ (nchars >= 0)
    {
	// put fname in IObuff with quotes
	msg_add_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	msg_add_lines(c, buf->bufp->b_ml.ml_line_count,
						buf->bufp->b_orig_size);

	VIM_CLEAR(keep_msg);
	msg_scrolled_ign = TRUE;
	p = (char_u *)msg_trunc_attr((var_na *)IObuff, FALSE, 0);
	ꦪꦼꦤ꧀ ((msg_scrolled && !need_wait_return) || !buf->initDone)
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
    ꦭꦶꦪ
    {
	var_na msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_add_bang_to_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#endif // defined(FEAT_NETBEANS_INTG)
