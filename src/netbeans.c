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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_NETBEANS_INTG)

#ifndef MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#endif

#ଅନ୍ତର୍ଭାଵ "version.h"

#define GUARDED		10000 // typenr for "guarded" annotation
#define GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#define MAX_COLOR_LENGTH 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#define VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#define VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
ସ୍ଥିର ଅକ୍ଷର *ExtEdProtocolVersion = "2.5";

ସ୍ଥିର long pos2off(buf_T *, pos_T *);
ସ୍ଥିର pos_T *off2pos(buf_T *, long);
ସ୍ଥିର pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
ସ୍ଥିର long get_buf_size(buf_T *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା netbeans_keystring(char_u *keystr);
ସ୍ଥିର ଶୂନ୍ୟ special_ଚାବିଗୁଡ଼ିକ(char_u *args);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା getConnInfo(ଅକ୍ଷର *file, ଅକ୍ଷର **host, ଅକ୍ଷର **port, ଅକ୍ଷର **password);

ସ୍ଥିର ଶୂନ୍ୟ nb_init_graphics(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ coloncmd(ଅକ୍ଷର *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ସ୍ଥିର ଶୂନ୍ୟ nb_set_curbuf(buf_T *buf);
ସ୍ଥିର ଶୂନ୍ୟ nb_parse_cmd(char_u *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  nb_do_cmd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, char_u *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, char_u *);
ସ୍ଥିର ଶୂନ୍ୟ nb_send(ଅକ୍ଷର *buf, ଅକ୍ଷର *fun);
ସ୍ଥିର ଶୂନ୍ୟ nb_free(ଶୂନ୍ୟ);

#define NETBEANS_OPEN (channel_can_write_to(nb_channel))
ସ୍ଥିର channel_T *nb_channel = ଖାଲି;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା r_cmdno;			// current command number for reply
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ifdef NBDEBUG
# include "nbdebug.c"
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା needupdate = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା inAtomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nb_is_safe_string(char_u *str, char_u *allowed)
{
    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    ପାଇଁ (char_u *p = str; *p != NUL; p++)
	ଯଦି (vim_strchr(allowed, *p) == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nb_channel_closed(ଶୂନ୍ୟ)
{
    nb_channel = ଖାଲି;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
netbeans_close(ଶୂନ୍ୟ)
{
    ଯଦି (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	ଯଦି (nb_channel != ଖାଲି)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = ଖାଲି;
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

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
netbeans_connect(ଅକ୍ଷର *params, ପୂର୍ଣ୍ଣସଂଖ୍ୟା doabort)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		port;
    ଅକ୍ଷର	buf[32];
    ଅକ୍ଷର	*hostname = ଖାଲି;
    ଅକ୍ଷର	*address = ଖାଲି;
    ଅକ୍ଷର	*password = ଖାଲି;
    ଅକ୍ଷର	*fname;
    ଅକ୍ଷର	*arg = ଖାଲି;

    ଯଦି (*params == '=')
    {
	// "=fname": Read info from specified file.
	ଯଦି (getConnInfo(params + 1, &hostname, &address, &password) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	ଯଦି (arg == ଖାଲି && (fname = getenv("__NETBEANS_CONINFO")) != ଖାଲି)
	{
	    // "": get info from file specified in environment
	    ଯଦି (getConnInfo(fname, &hostname, &address, &password) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (arg != ଖାଲି)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		address = strchr(hostname, ':');
		ଯଦି (address != ଖାଲି)
		{
		    *address++ = '\0';
		    password = strchr(address, ':');
		    ଯଦି (password != ଖାଲି)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    ଯଦି (hostname == ଖାଲି || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    ଯଦି (address == ଖାଲି)
		address = getenv("__NETBEANS_SOCKET");
	    ଯଦି (password == ଖାଲି)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    ଯଦି (hostname != ଖାଲି)
		hostname = (ଅକ୍ଷର *)vim_strsave((char_u *)hostname);
	    ଯଦି (address != ଖାଲି)
		address = (ଅକ୍ଷର *)vim_strsave((char_u *)address);
	    ଯଦି (password != ଖାଲି)
		password = (ଅକ୍ଷର *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    ଯଦି (hostname == ଖାଲି || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (ଅକ୍ଷର *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    ଯଦି (address == ଖାଲି || *address == '\0')
    {
	vim_free(address);
	address = (ଅକ୍ଷର *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    ଯଦି (password == ଖାଲି || *password == '\0')
    {
	vim_free(password);
	password = (ଅକ୍ଷର *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    ଯଦି (hostname != ଖାଲି && address != ଖାଲି && password != ଖାଲି)
    {
	port = atoi(address);
	nb_channel = channel_open(hostname, port, 3000, nb_channel_closed);
	ଯଦି (nb_channel != ଖାଲି)
	{
	    // success
#ifdef FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#endif

	    // success, login
	    vim_snprintf(buf, ଆକାର(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", ExtEdProtocolVersion);
	    nb_send(buf, "externaleditor_version");
	}
    }

    ଯଦି (nb_channel == ଖାଲି && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(address);
    vim_free(password);
    ଫେରନ୍ତୁ NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getConnInfo(ଅକ୍ଷର *file, ଅକ୍ଷର **host, ଅକ୍ଷର **port, ଅକ୍ଷର **auth)
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
    ଯଦି (mch_stat(file, &st) == 0 && (st.st_mode & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_mode_for_netbeans_connection_info_file_str),
									file);
	ଫେରନ୍ତୁ FAIL;
    }
#endif

    fp = mch_fopen(file, "r");
    ଯଦି (fp == ଖାଲି)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	ଫେରନ୍ତୁ FAIL;
    }

    // Read the file. There should be one of each parameter
    ଯେତେବେଳେ ((lp = (char_u *)fgets((ଅକ୍ଷର *)buf, BUFSIZ, fp)) != ଖାଲି)
    {
	ଯଦି ((nlp = vim_strchr(lp, '\n')) != ଖାଲି)
	    *nlp = 0;	    // strip off the trailing newline

	ଯଦି (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (ଅକ୍ଷର *)vim_strsave(&buf[5]);
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*port);
	    *port = (ଅକ୍ଷର *)vim_strsave(&buf[5]);
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (ଅକ୍ଷର *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    ଫେରନ୍ତୁ OK;
}


ସଂରଚନା keyqueue
{
    char_u	    *keystr;
    ସଂରଚନା keyqueue *next;
    ସଂରଚନା keyqueue *prev;
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା keyqueue ଚାବି_Q_T;

ସ୍ଥିର ଚାବି_Q_T ଚାବି_Head; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
postpone_keycommand(char_u *keystr)
{
    ଚାବି_Q_T *ଗଣ୍ଠି;

    ଗଣ୍ଠି = ALLOC_ONE(ଚାବି_Q_T);
    ଯଦି (ଗଣ୍ଠି == ଖାଲି)
	ଫେରନ୍ତୁ;  // out of memory, drop the key

    ଯଦି (ଚାବି_Head.next == ଖାଲି) // initialize circular queue
    {
	ଚାବି_Head.next = &ଚାବି_Head;
	ଚାବି_Head.prev = &ଚାବି_Head;
    }

    // insert node at tail of queue
    ଗଣ୍ଠି->next = &ଚାବି_Head;
    ଗଣ୍ଠି->prev = ଚାବି_Head.prev;
    ଚାବି_Head.prev->next = ଗଣ୍ଠି;
    ଚାବି_Head.prev = ଗଣ୍ଠି;

    ଗଣ୍ଠି->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_ଚାବି_queue(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା postponed = FALSE;

    ଯେତେବେଳେ (!postponed && ଚାବି_Head.next && ଚାବି_Head.next != &ଚାବି_Head)
    {
	// first, unlink the node
	ଚାବି_Q_T *ଗଣ୍ଠି = ଚାବି_Head.next;
	ଚାବି_Head.next = ଗଣ୍ଠି->next;
	ଗଣ୍ଠି->next->prev = ଗଣ୍ଠି->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	ଯଦି (ଗଣ୍ଠି->keystr != ଖାଲି)
	    postponed = !netbeans_keystring(ଗଣ୍ଠି->keystr);
	vim_free(ଗଣ୍ଠି->keystr);

	// Finally, dispose of the node
	vim_free(ଗଣ୍ଠି);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    ଶୂନ୍ୟ
netbeans_parse_messages(ଶୂନ୍ୟ)
{
    readq_T	*ଗଣ୍ଠି;
    char_u	*buffer;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		own_ଗଣ୍ଠି;

    ଯେତେବେଳେ (nb_channel != ଖାଲି)
    {
	ଗଣ୍ଠି = channel_peek(nb_channel, PART_SOCK);
	ଯଦି (ଗଣ୍ଠି == ଖାଲି)
	    ଭାଙ୍ଗ;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_first_nl(ଗଣ୍ଠି);
	ଯଦି (p == ଖାଲି)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    ଯଦି (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		ଫେରନ୍ତୁ;
	    ଜାରି;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	ଯଦି (*p == NUL)
	{
	    own_ଗଣ୍ଠି = TRUE;
	    buffer = channel_get(nb_channel, PART_SOCK, ଖାଲି);
	    // "node" is now invalid!
	}
	ଅନ୍ୟଥା
	{
	    own_ଗଣ୍ଠି = FALSE;
	    buffer = ଗଣ୍ଠି->rq_buffer;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(buffer);

	ଯଦି (own_ଗଣ୍ଠି)
	    // buffer finished, dispose of it
	    vim_free(buffer);
	ଅନ୍ୟଥା ଯଦି (nb_channel != ଖାଲି)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - buffer));
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
    ସ୍ଥିର ଶୂନ୍ୟ
nb_parse_cmd(char_u *cmd)
{
    ଅକ୍ଷର	*verb;
    ଅକ୍ଷର	*q;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufno;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		isfunc = -1;

    ଯଦି (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    ଯଦି (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_column = false;

	// The IDE is breaking the connection.
	netbeans_close();
	ଫେରନ୍ତୁ;
    }

    bufno = strtol((ଅକ୍ଷର *)cmd, &verb, 10);

    ଯଦି (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	ଫେରନ୍ତୁ;
    }
    ++verb; // skip colon

    ପାଇଁ (q = verb; *q; q++)
    {
	ଯଦି (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    ଭାଙ୍ଗ;
	}
    }

    ଯଦି (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_in_str), cmd);
	ଫେରନ୍ତୁ;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (ଅକ୍ଷର *)skipwhite((char_u *)q);

    ଯଦି (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
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

ସଂରଚନା nbbuf_struct
{
    buf_T		*bufp;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା	 fireChanges:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା	 initDone:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା	 ଭର୍ତ୍ତି_Done:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା	 modified:1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			 nbbuf_ସଂଖ୍ୟା;
    ଅକ୍ଷର		*displayname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା nbbuf_struct nbbuf_T;

ସ୍ଥିର nbbuf_T *buf_list = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା buf_list_size = 0;	// size of buf_list
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା buf_list_used = 0;	// nr of entries in buf_list actually in use

ସ୍ଥିର ଅକ୍ଷର **globalsignmap = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା globalsignmaplen = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା globalsignmapused = 0;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  mapsigntype(nbbuf_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା localsigntype);
ସ୍ଥିର ଶୂନ୍ୟ addsigntype(nbbuf_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
ସ୍ଥିର ଶୂନ୍ୟ print_read_ସନ୍ଦେଶ(nbbuf_T *buf);
ସ୍ଥିର ଶୂନ୍ୟ print_save_ସନ୍ଦେଶ(nbbuf_T *buf, off_T nchars);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା curPCtype = -1;

/*
 * Free netbeans resources.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nb_free(ଶୂନ୍ୟ)
{
    ଚାବି_Q_T *ଚାବି_ଗଣ୍ଠି = ଚାବି_Head.next;
    nbbuf_T buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    // free the netbeans buffer list
    ପାଇଁ (i = 0; i < buf_list_used; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	ଯଦି (buf.bufp != ଖାଲି && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_size = 0;
    buf_list_used = 0;

    // free the queued key commands
    ଯେତେବେଳେ (ଚାବି_ଗଣ୍ଠି != ଖାଲି && ଚାବି_ଗଣ୍ଠି != &ଚାବି_Head)
    {
	ଚାବି_Q_T *next = ଚାବି_ଗଣ୍ଠି->next;
	vim_free(ଚାବି_ଗଣ୍ଠି->keystr);
	vim_free(ଚାବି_ଗଣ୍ଠି);
	ଯଦି (next == &ଚାବି_Head)
	{
	    ଚାବି_Head.next = &ଚାବି_Head;
	    ଚାବି_Head.prev = &ଚାବି_Head;
	    ଭାଙ୍ଗ;
	}
	ଚାବି_ଗଣ୍ଠି = next;
    }

    // free the queued netbeans commands
    ଯଦି (nb_channel != ଖାଲି)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nb_getbufno(buf_T *bufp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < buf_list_used; i++)
	ଯଦି (buf_list[i].bufp == bufp)
	    ଫେରନ୍ତୁ i;
    ଫେରନ୍ତୁ -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
isNetbeansBuffer(buf_T *bufp)
{
    ଫେରନ୍ତୁ NETBEANS_OPEN && bufp->b_netbeans_file;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
isNetbeansModified(buf_T *bufp)
{
    ଯଦି (isNetbeansBuffer(bufp))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufno = nb_getbufno(bufp);

	ଯଦି (bufno > 0)
	    ଫେରନ୍ତୁ buf_list[bufno].modified;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ FALSE;
    }
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    ସ୍ଥିର nbbuf_T *
nb_get_buf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufno)
{
    // find or create a buffer with the given number
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା incr;

    ଯଦି (bufno <= 0)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * ଆକାର(nbbuf_T));
	buf_list_size = 100;
    }
    ଯଦି (bufno >= buf_list_used) // new
    {
	ଯଦି (bufno >= buf_list_size) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_size + 90;
	    buf_list_size += incr;
	    bufsize = buf_list_size * ଆକାର(nbbuf_T);
	    ଯଦି (bufsize == 0 || bufsize / ଆକାର(nbbuf_T)
						      != (size_t)buf_list_size)
	    {
		// list size overflow, bail out
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    ଯଦି (buf_list == ଖାଲି)
	    {
		vim_free(t_buf_list);
		buf_list_size = 0;
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    vim_memset(buf_list + buf_list_size - incr, 0,
						      incr * ଆକାର(nbbuf_T));
	}

	ଯେତେବେଳେ (buf_list_used <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_used].fireChanges = 1;
	    ++buf_list_used;
	}
    }

    ଫେରନ୍ତୁ buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଗଣନା_changed_buffers(ଶୂନ୍ୟ)
{
    buf_T	*bufp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	ଯଦି (bufp->b_changed)
	    ++n;
    ଫେରନ୍ତୁ n;
}

/*
 * End the netbeans session.
 */
    ଶୂନ୍ୟ
netbeans_end(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ସ୍ଥିର ଅକ୍ଷର buf[128];

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < buf_list_used; i++)
    {
	ଯଦି (!buf_list[i].bufp)
	    ଜାରି;
	ଯଦି (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, ଖାଲି);
	buf_list[i].bufp = ଖାଲି;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nb_send(ଅକ୍ଷର *buf, ଅକ୍ଷର *fun)
{
    ଯଦି (nb_channel != ଖାଲି)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nb_reply_nil(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdno)
{
    ଅକ୍ଷର reply[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    sprintf(reply, "%d\n", cmdno);
    nb_send(reply, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nb_reply_text(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdno, char_u *result)
{
    char_u *reply;

    nbdebug(("REP %d: %s\n", cmdno, (ଅକ୍ଷର *)result));

    reply = alloc(STRLEN(result) + 32);
    sprintf((ଅକ୍ଷର *)reply, "%d %s\n", cmdno, (ଅକ୍ଷର *)result);
    nb_send((ଅକ୍ଷର *)reply, "nb_reply_text");

    vim_free(reply);
}


/*
 * Send a response with a number result code.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nb_reply_nr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdno, long result)
{
    ଅକ୍ଷର reply[32];

    nbdebug(("REP %d: %ld\n", cmdno, result));

    sprintf(reply, "%d %ld\n", cmdno, result);
    nb_send(reply, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    ସ୍ଥିର char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ପାଇଁ (; *p; p++)
    {
	ବାଛ (*p)
	{
	    ସ୍ଥିତି '\"':
	    ସ୍ଥିତି '\\':
		*q++ = '\\'; *q++ = *p; ଭାଙ୍ଗ;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    ସ୍ଥିତି '\n':
		*q++ = '\\'; *q++ = 'n'; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '\r':
		*q++ = '\\'; *q++ = 'r'; ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		*q++ = *p;
		ଭାଙ୍ଗ;
	}
    }
    *q = '\0';

    ଫେରନ୍ତୁ buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    ସ୍ଥିର ଅକ୍ଷର *
nb_unquote(char_u *p, char_u **endp)
{
    ଅକ୍ଷର *result = 0;
    ଅକ୍ଷର *q;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା done = 0;

    // result is never longer than input
    result = alloc_clear(STRLEN(p) + 1);
    ଯଦି (result == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	result[0] = NUL;
	ଫେରନ୍ତୁ result;
    }

    ପାଇଁ (q = result; !done && *p != NUL;)
    {
	ବାଛ (*p)
	{
	    ସ୍ଥିତି '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		done = 1;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି '\\':
		++p;
		ବାଛ (*p)
		{
		    ସ୍ଥିତି '\\':	*q++ = '\\';	ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'n':	*q++ = '\n';	ଭାଙ୍ଗ;
		    ସ୍ଥିତି 't':	*q++ = '\t';	ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'r':	*q++ = '\r';	ଭାଙ୍ଗ;
		    ସ୍ଥିତି '"':	*q++ = '"';	ଭାଙ୍ଗ;
		    ସ୍ଥିତି NUL:	--p;		ଭାଙ୍ଗ;
		    // default: skip over illegal chars
		}
		++p;
		ଭାଙ୍ଗ;

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		*q++ = *p++;
	}
    }

    ଯଦି (endp != ଖାଲି)
	*endp = p;

    ଫେରନ୍ତୁ result;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nb_partialremove(linenr_T lnum, colnr_T first, colnr_T last)
{
    char_u *oldtext, *newtext;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lastbyte = last;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    ଯଦି (first >= (colnr_T)oldlen || oldlen == 0)  // just in case
	ଫେରନ୍ତୁ;
    ଯଦି (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - first));
    ଯଦି (newtext == ଖାଲି)
	ଫେରନ୍ତୁ;

    mch_memmove(newtext, oldtext, first);
    STRMOVE(newtext + first, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nb_joinlines(linenr_T first, linenr_T other)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len_first, len_other;
    char_u *p;

    len_first = ml_get_len(first);
    len_other = ml_get_len(other);
    p = alloc(len_first + len_other + 1);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nb_do_cmd(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufno,
    char_u	*cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		func,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdno,
    char_u	*args)	    // points to space before arguments or NUL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_update = 0;
    long	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	skip = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    ଅକ୍ଷର	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (ଅକ୍ଷର *)args));

    ଯଦି (func)
    {
// =====================================================================
	ଯଦି (streq((ଅକ୍ଷର *)cmd, "getModified"))
	{
	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
		// Return the number of buffers that are modified.
		nb_reply_nr(cmdno, (long)ଗଣନା_changed_buffers());
	    ଅନ୍ୟଥା
		// Return whether the buffer is modified.
		nb_reply_nr(cmdno, (long)(buf->bufp->b_changed
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_reply_nr(cmdno, (long)ଗଣନା_changed_buffers());
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "getCursor"))
	{
	    char_u text[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((ଅକ୍ଷର *)text, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (long)curwin->w_cursor.lnum,
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_cursor.col,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_reply_text(cmdno, text);
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "getAnno"))
	{
	    long linenum = 0;
#ifdef FEAT_SIGNS
	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_getanno));
		retval = FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ser_ଅଙ୍କ;

		cp = (ଅକ୍ଷର *)args;
		ser_ଅଙ୍କ = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (long)buf_findsign(buf->bufp, ser_ଅଙ୍କ, ଖାଲି);
	    }
#endif
	    nb_reply_nr(cmdno, linenum);
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "getLength"))
	{
	    long len = 0;

	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_buffer_identifier_in_getlength));
		retval = FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		len = get_buf_size(buf->bufp);
	    }
	    nb_reply_nr(cmdno, len);
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "getText"))
	{
	    long	len;
	    linenr_T	nlines;
	    char_u	*text = ଖାଲି;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*ଧାଡ଼ି;

	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_buffer_identifier_in_gettext));
		retval = FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		len = get_buf_size(buf->bufp);
		nlines = buf->bufp->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
		text = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		ଯଦି (text == ଖାଲି)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		ଅନ୍ୟଥା
		{
		    p = text;
		    *p++ = '\"';
		    ପାଇଁ (; lno <= nlines ; lno++)
		    {
			ଧାଡ଼ି = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			ଯଦି (ଧାଡ଼ି != ଖାଲି)
			{
			    STRCPY(p, ଧାଡ଼ି);
			    p += STRLEN(ଧାଡ଼ି);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(ଧାଡ଼ି);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    ଯଦି (text == ଖାଲି)
		nb_reply_text(cmdno, (char_u *)"");
	    ଅନ୍ୟଥା
	    {
		nb_reply_text(cmdno, text);
		vim_free(text);
	    }
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "remove"))
	{
	    long ଗଣନା_2;
	    pos_T first, last;
	    pos_T *pos;
	    pos_T *next;
	    linenr_T del_from_lnum, del_to_lnum;  // lines to be deleted as a whole
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldFire = netbeansFireChanges;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldSuppress = netbeans_Suppress_No_ଧାଡ଼ିଗୁଡ଼ିକ;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା wasChanged;

	    ଯଦି (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (ଅକ୍ଷର *) cmd));
		nb_reply_nil(cmdno);
		ଫେରନ୍ତୁ OK;
	    }

	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_buffer_identifier_in_remove));
		retval = FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_ଧାଡ଼ିଗୁଡ଼ିକ = TRUE;

		nb_set_curbuf(buf->bufp);
		wasChanged = buf->bufp->b_changed;
		cp = (ଅକ୍ଷର *)args;
		off = strtol(cp, &cp, 10);
		ଗଣନା_2 = strtol(cp, &cp, 10);
		args = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		ଯଦି (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_ଧାଡ଼ିଗୁଡ଼ିକ = oldSuppress;
		    ଫେରନ୍ତୁ FAIL;
		}
		first = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      first.lnum, first.col));
		pos = off2pos(buf->bufp, off+ଗଣନା_2-1);
		ଯଦି (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_reply_text(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_ଧାଡ଼ିଗୁଡ଼ିକ = oldSuppress;
		    ଫେରନ୍ତୁ FAIL;
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
		next = off2pos(buf->bufp, off + ଗଣନା_2);

		// Remove part of the first line.
		ଯଦି (first.col != 0
				|| (next != ଖାଲି && first.lnum == next->lnum))
		{
		    ଯଦି (first.lnum != last.lnum
			    || (next != ଖାଲି && first.lnum != next->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(first.lnum, first.col,
							     (colnr_T)MAXCOL);
			ଯଦି (first.lnum == last.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(first.lnum, next->lnum);
			    del_to_lnum = next->lnum;
			}
		    }
		    ଅନ୍ୟଥା
		    {
			// remove within one line
			nb_partialremove(first.lnum, first.col, last.col);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		ଯଦି (first.lnum != last.lnum && next != ଖାଲି
			&& next->col != 0 && last.lnum == next->lnum)
		{
		    nb_partialremove(last.lnum, 0, last.col);
		    ଯଦି (del_from_lnum > first.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(first.lnum, last.lnum);
		    }
		    ଅନ୍ୟଥା
			// First line is deleted as a whole, keep the last
			// line.
			--del_to_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		ଯଦି (first.lnum != last.lnum && del_from_lnum > first.lnum
			&& next != ଖାଲି && last.lnum != next->lnum)
		{
		    nb_joinlines(first.lnum, next->lnum);
		    del_to_lnum = next->lnum;
		}

		// Delete whole lines if there are any.
		ଯଦି (del_to_lnum >= del_from_lnum)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

		    // delete signs from the lines being deleted
		    ପାଇଁ (i = del_from_lnum; i <= del_to_lnum; i++)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା id = buf_findsign_id(buf->bufp, (linenr_T)i, ଖାଲି);
			ଯଦି (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, ଖାଲି);
			}
			ଅନ୍ୟଥା
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_to_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.col = 0;
		    del_ଧାଡ଼ିଗୁଡ଼ିକ(del_to_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = first;
		check_cursor_lnum();
		buf->bufp->b_changed = wasChanged; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_ଧାଡ଼ିଗୁଡ଼ିକ = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_reply_nil(cmdno);
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "insert"))
	{
	    char_u	*to_free;

	    ଯଦି (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (ଅକ୍ଷର *) cmd));
		nb_reply_nil(cmdno);
		ଫେରନ୍ତୁ OK;
	    }

	    // get offset
	    cp = (ଅକ୍ଷର *)args;
	    off = strtol(cp, &cp, 10);
	    args = (char_u *)cp;

	    // get text to be inserted
	    args = skipwhite(args);
	    args = to_free = (char_u *)nb_unquote(args, ଖାଲି);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_buffer_identifier_in_ଭର୍ତ୍ତି));
		retval = FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (args != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ff_detected = EOL_UNKNOWN;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	buf_was_empty = (buf->bufp->b_ml.ml_flags & ML_EMPTY);
		size_t	len = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	added = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	oldFire = netbeansFireChanges;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	old_b_changed;
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
		ଯଦି (pos != ଖାଲି)
		{
		    ଯଦି (pos->lnum <= 0)
			lnum_start = 1;
		    ଅନ୍ୟଥା
			lnum_start = pos->lnum;
		}
		ଅନ୍ୟଥା
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    ଯଦି (buf_was_empty)
			lnum_start = 1;	    // above empty line
		    ଅନ୍ୟଥା
			lnum_start = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_start;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		ଯେତେବେଳେ (*args != NUL)
		{
		    nlp = vim_strchr(args, '\n');
		    ଯଦି (nlp == ଖାଲି)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(args);
		    }
		    ଅନ୍ୟଥା
		    {
			len = nlp - args;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			ଯଦି (nlp > args && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			ଅନ୍ୟଥା
			    ff_detected = EOL_UNIX;
		    }
		    args[len] = NUL;

		    ଯଦି (lnum == lnum_start
			    && ((pos != ଖାଲି && pos->col > 0)
				|| (lnum == 1 && buf_was_empty)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col = pos == ଖାଲି ? 0 : pos->col;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			ଯଦି (newline != ଖାଲି)
			{
			    mch_memmove(newline, oldline, (size_t)col);
			    newline[col] = NUL;
			    STRCAT(newline, args);
			    STRCAT(newline, oldline + col);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    ଅନ୍ୟଥା
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), args,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    ଯଦି (nlp == ଖାଲି)
			ଭାଙ୍ଗ;
		    ++lnum;
		    args = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark(lnum_start - 1, (long)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		ଯଦି (buf_was_empty)
		{
		    ଯଦି (ff_detected == EOL_UNKNOWN)
#ଯଦି defined(MSWIN)
			ff_detected = EOL_DOS;
#ଅନ୍ୟଥା
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
	ଅନ୍ୟଥା
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_reply_nil(cmdno);
	    retval = FAIL;
	}
    }
    ଅନ୍ୟଥା // Not a function; no reply required.
    {
// =====================================================================
	ଯଦି (streq((ଅକ୍ଷର *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    ଯଦି (buf == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_buffer_identifier_in_create));
		ଫେରନ୍ତୁ FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, ଖାଲି, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->ଭର୍ତ୍ତି_Done = FALSE;
#ଯଦି defined(FEAT_MENU) && defined(FEAT_GUI)
	    ଯଦି (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "insertDone"))
	{
	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    ଅନ୍ୟଥା
	    {
		buf->bufp->b_start_eol = *args == 'T';
		buf->ଭର୍ତ୍ତି_Done = TRUE;
		args += 2;
		buf->bufp->b_p_ro = *args == 'T';
		print_read_ସନ୍ଦେଶ(buf);
	    }
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "saveDone"))
	{
	    long savedChars = atol((ଅକ୍ଷର *)args);

	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    ଅନ୍ୟଥା
		print_save_ସନ୍ଦେଶ(buf, savedChars);
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "startDocumentListen"))
	{
	    ଯଦି (buf == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_startdocumentlisten));
		ଫେରନ୍ତୁ FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "stopDocumentListen"))
	{
	    ଯଦି (buf == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_buffer_identifier_in_stopdocumentlisten));
		ଫେରନ୍ତୁ FAIL;
	    }
	    buf->fireChanges = 0;
	    ଯଦି (buf->bufp != ଖାଲି && buf->bufp->b_was_netbeans_file)
	    {
		ଯଦି (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_lost_for_buffer_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_lost_for_buffer_nr),
							   buf->bufp->b_fnum);
		}
		ଅନ୍ୟଥା
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
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "setTitle"))
	{
	    ଯଦି (buf == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_buffer_identifier_in_settitle));
		ଫେରନ୍ତୁ FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ଖାଲି);
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "initDone"))
	{
	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_buffer_identifier_in_initdone));
		ଫେରନ୍ତୁ FAIL;
	    }
	    do_update = 1;
	    buf->initDone = TRUE;
	    nb_set_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_ଚାବି_queue();
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "setBufferNumber")
		|| streq((ଅକ୍ଷର *)cmd, "putBufferNumber"))
	{
	    char_u	*path;
	    buf_T	*bufp;

	    ଯଦି (buf == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_buffer_identifier_in_setbuffernumber));
		ଫେରନ୍ତୁ FAIL;
	    }
	    path = (char_u *)nb_unquote(args, ଖାଲି);
	    ଯଦି (path == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	    bufp = buflist_findname(path);
	    vim_free(path);
	    ଯଦି (bufp == ଖାଲି)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", args));
		semsg(_(e_file_str_not_found_in_setbuffernumber), args);
		ଫେରନ୍ତୁ FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_ସଂଖ୍ୟା = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    ଯଦି (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    ଅନ୍ୟଥା
	    {
		buf->initDone = TRUE;

		// handle any postponed key commands
		handle_ଚାବି_queue();
	    }

// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "setFullName"))
	{
	    ଯଦି (buf == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_buffer_identifier_in_setfullname));
		ଫେରନ୍ତୁ FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ଖାଲି);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#ଯଦି defined(FEAT_MENU) && defined(FEAT_GUI)
	    ଯଦି (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "editFile"))
	{
	    ଯଦି (buf == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_buffer_identifier_in_editfile));
		ଫେରନ୍ତୁ FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ଖାଲି);
	    do_ecmd(0, (char_u *)buf->displayname, ଖାଲି, ଖାଲି, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->initDone = TRUE;
	    do_update = 1;
	    maketitle();
#ଯଦି defined(FEAT_MENU) && defined(FEAT_GUI)
	    ଯଦି (gui.in_use)
		gui_update_menus(0);
#endif
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "setVisible"))
	{
	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ଯଦି (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setvisible));
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (streq((ଅକ୍ଷର *)args, "T") && buf->bufp != curbuf)
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
		ଯଦି (gui.in_use)
		    gui_mch_set_foreground();
#endif
	    }
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "raise"))
	{
#ifdef FEAT_GUI
	    // Bring gvim to the foreground.
	    ଯଦି (gui.in_use)
		gui_mch_set_foreground();
#endif
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "setModified"))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_b_changed;

	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ଯଦି (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_setmodified));
		ଫେରନ୍ତୁ FAIL;
	    }
	    prev_b_changed = buf->bufp->b_changed;
	    ଯଦି (streq((ଅକ୍ଷର *)args, "T"))
		buf->bufp->b_changed = TRUE;
	    ଅନ୍ୟଥା
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		ଯଦି (buf->bufp->b_ffname != ଖାଲି
			&& mch_stat((ଅକ୍ଷର *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_time(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_changed = FALSE;
	    }
	    buf->modified = buf->bufp->b_changed;
	    ଯଦି (prev_b_changed != buf->bufp->b_changed)
	    {
		check_status(buf->bufp);
		redraw_tabline = TRUE;
#ଯଦି defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#endif
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "setModtime"))
	{
	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    ଅନ୍ୟଥା
	    {
		buf->bufp->b_mtime = atoi((ଅକ୍ଷର *)args);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "setReadOnly"))
	{
	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)args, "T"))
		buf->bufp->b_p_ro = TRUE;
	    ଅନ୍ୟଥା
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "showBalloon"))
	{
#ଯଦି defined(FEAT_BEVAL_GUI)
	    ସ୍ଥିର ଅକ୍ଷର	*text = ଖାଲି;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    ଯଦି (balloonEval != ଖାଲି)
	    {
		vim_free(text);
		text = nb_unquote(args, ଖାଲି);
		ଯଦି (text != ଖାଲି)
		    gui_mch_post_balloon(balloonEval, (char_u *)text);
	    }
#endif
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "setDot"))
	{
	    pos_T *pos;
#ifdef NBDEBUG
	    char_u *s;
#endif

	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_buffer_identifier_in_setdot));
		ଫେରନ୍ତୁ FAIL;
	    }

	    nb_set_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    ଯଦି (VIsual_active)
		end_visual_mode();
#ifdef NBDEBUG
	    s = args;
#endif
	    pos = get_off_or_lnum(buf->bufp, &args);
	    ଯଦି (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#ifdef FEAT_FOLDING
		foldOpenCursor();
#endif
	    }
	    ଅନ୍ୟଥା
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
	    ଯଦି (State == MODE_HITRETURN || State == MODE_ASKMORE)
	    {
#ifdef FEAT_GUI_GTK
		ଯଦି (gui.in_use && gtk_main_level() > 0)
		    gtk_main_quit();
#endif
	    }
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "close"))
	{
#ifdef NBDEBUG
	    ଅକ୍ଷର *name = "<NONE>";
#endif

	    ଯଦି (buf == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_buffer_identifier_in_close));
		ଫେରନ୍ତୁ FAIL;
	    }

#ifdef NBDEBUG
	    ଯଦି (buf->displayname != ଖାଲି)
		name = buf->displayname;
#endif
	    ଯଦି (buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ଯଦି (p_verbose > 0)
		    emsg(_(e_invalid_buffer_identifier_in_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    ଯଦି (buf->bufp != ଖାଲି)
		do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = ଖାଲି;
	    buf->initDone = FALSE;
	    do_update = 1;
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "defineAnnoType"))
	{
#ifdef FEAT_SIGNS
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_ଅଙ୍କ;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    ଯଦି (buf == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_buffer_identifier_in_defineannotype));
		ଫେରନ୍ତୁ FAIL;
	    }

	    cp = (ଅକ୍ଷର *)args;
	    type_ଅଙ୍କ = strtol(cp, &cp, 10);
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
	    ଯଦି (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(args);
	    bg = vim_strsave(p);
	    ଯଦି (STRLEN(fg) > MAX_COLOR_LENGTH || STRLEN(bg) > MAX_COLOR_LENGTH)
	    {
		emsg(_(e_highlighting_color_name_too_long_in_defineAnnoType));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (!nb_is_safe_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_safe_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_safe_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_in_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (typeName != ଖାଲି && tooltip != ଖାଲି && glyphFile != ଖାଲି)
		addsigntype(buf, type_ଅଙ୍କ, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    ଯଦି (parse_error)
		ଫେରନ୍ତୁ FAIL;

#endif
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "addAnno"))
	{
#ifdef FEAT_SIGNS
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ser_ଅଙ୍କ;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା local_Type_ଅଙ୍କ;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_ଅଙ୍କ;
	    pos_T *pos;

	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_buffer_identifier_in_addanno));
		ଫେରନ୍ତୁ FAIL;
	    }

	    do_update = 1;

	    cp = (ଅକ୍ଷର *)args;
	    ser_ଅଙ୍କ = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_ଅଙ୍କ = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    type_ଅଙ୍କ = mapsigntype(buf, local_Type_ଅଙ୍କ);

	    pos = get_off_or_lnum(buf->bufp, &args);

	    cp = (ଅକ୍ଷର *)args;
	    vim_ignored = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strtol(cp, &cp, 10);
	    args = (char_u *)cp;
# ifdef NBDEBUG
	    ଯଦି (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    ଯଦି (ser_ଅଙ୍କ >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_ଅଙ୍କ, pos->lnum, type_ଅଙ୍କ, buf->bufp->b_fnum);
		ଯଦି (type_ଅଙ୍କ == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_ଅଙ୍କ,
						       buf->bufp->b_fnum);
	    }
#endif
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "removeAnno"))
	{
#ifdef FEAT_SIGNS
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ser_ଅଙ୍କ;

	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		ଫେରନ୍ତୁ FAIL;
	    }
	    do_update = 1;
	    cp = (ଅକ୍ଷର *)args;
	    ser_ଅଙ୍କ = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_ଅଙ୍କ, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#endif
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "moveAnnoToFront"))
	{
#ifdef FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#endif
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "guard") || streq((ଅକ୍ଷର *)cmd, "unguard"))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
	    pos_T first;
	    pos_T last;
	    pos_T *pos;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା un = (cmd[0] == 'u');
	    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା guardId = GUARDEDOFFSET;

	    ଯଦି (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (ଅକ୍ଷର *) cmd));
		ଫେରନ୍ତୁ OK;
	    }

	    nb_init_graphics();

	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ଫେରନ୍ତୁ FAIL;
	    }
	    nb_set_curbuf(buf->bufp);
	    cp = (ଅକ୍ଷର *)args;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, ଖାଲି, 10);
	    args = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    ଯଦି (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    ଅନ୍ୟଥା
	    {
		first = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		ଯଦି (pos != ଖାଲି && pos->col == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		ଯଦି (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		ଅନ୍ୟଥା
		{
		    long lnum;
		    last = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     first.lnum, first.col,
			     last.lnum, last.col));
#ifdef FEAT_SIGNS
		    ପାଇଁ (lnum = first.lnum; lnum <= last.lnum; lnum++)
		    {
			ଯଦି (un)
			{
			    // never used
			}
			ଅନ୍ୟଥା
			{
			    ଯଦି (buf_findsigntype_id(buf->bufp, lnum,
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
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "startAtomic"))
	{
	    inAtomic = 1;
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "endAtomic"))
	{
	    inAtomic = 0;
	    ଯଦି (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ଫେରନ୍ତୁ FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    ଯଦି (bufIsChanged(buf->bufp))
	    {
		// Only write if the buffer can be written.
		ଯଦି (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != ଖାଲି
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    ଯଦି (!bufref_valid(&bufref))
			buf->bufp = ଖାଲି;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "netbeansBuffer"))
	{
	    ଯଦି (buf == ଖାଲି || buf->bufp == ଖାଲି)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (*args == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    ଅନ୍ୟଥା
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "specialKeys"))
	{
	    special_ଚାବିଗୁଡ଼ିକ(args);
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	ଅନ୍ୟଥା ଯଦି (streq((ଅକ୍ଷର *)cmd, "version"))
	{
	    // not used yet
	}
	ଅନ୍ୟଥା
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    ଯଦି (inAtomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    ଯଦି (buf != ଖାଲି && buf->initDone && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	out_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	ଯଦି (State == MODE_HITRETURN || State == MODE_ASKMORE)
	{
#ifdef FEAT_GUI_GTK
	    ଯଦି (gui.in_use && gtk_main_level() > 0)
		gtk_main_quit();
#endif
	}
    }

    ଫେରନ୍ତୁ retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nb_set_curbuf(buf_T *buf)
{
    ଯଦି (curbuf == buf)
	ଫେରନ୍ତୁ;

    ଯଦି (buf_jump_open_win(buf) != ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != ଖାଲି)
	ଫେରନ୍ତୁ;
    set_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
coloncmd(ଅକ୍ଷର *cmd, ...)
{
    ଅକ୍ଷର buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, ଆକାର(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, ଖାଲି, ଖାଲି, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    out_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
special_ଚାବିଗୁଡ଼ିକ(char_u *args)
{
    ଅକ୍ଷର *save_str = nb_unquote(args, ଖାଲି);
    ଅକ୍ଷର *tok = strtok(save_str, " ");
    ଅକ୍ଷର *sep;
#define KEYBUFLEN 64
    ଅକ୍ଷର keybuf[KEYBUFLEN];
    ଅକ୍ଷର cmdbuf[256];

    ଯେତେବେଳେ (tok != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;

	ଯଦି ((sep = strchr(tok, '-')) != ଖାଲି)
	{
	    *sep = NUL;
	    ଯେତେବେଳେ (*tok && i + 2 < KEYBUFLEN)
	    {
		ବାଛ (*tok)
		{
		    ସ୍ଥିତି 'A':
		    ସ୍ଥିତି 'M':
		    ସ୍ଥିତି 'C':
		    ସ୍ଥିତି 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			ଭାଙ୍ଗ;
		}
		tok++;
	    }
	    tok++;
	}

	ଯଦି (strlen(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe = TRUE;
	    ପାଇଁ (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		ଯଦି (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    safe = FALSE;
		    ଭାଙ୍ଗ;
		}
	    }
	    ଯଦି (safe)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, ଆକାର(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, MODE_NORMAL, FALSE);
	    }
	}
	tok = strtok(ଖାଲି, " ");
    }
    vim_free(save_str);
}

    ଶୂନ୍ୟ
ex_nbclose(exarg_T *eap UNUSED)
{
    netbeans_close();
}

    ଶୂନ୍ୟ
ex_nbkey(exarg_T *eap)
{
    (ଶୂନ୍ୟ)netbeans_keystring(eap->arg);
}

    ଶୂନ୍ୟ
ex_nbstart(
    exarg_T	*eap)
{
#ifdef FEAT_GUI
# ଯଦି !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    ଯଦି (gui.in_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	ଫେରନ୍ତୁ;
    }
# endif
#endif
    netbeans_open((ଅକ୍ଷର *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nb_init_graphics(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_init = FALSE;

    ଯଦି (did_init)
	ଫେରନ୍ତୁ;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
netbeans_keyname(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି, ଅକ୍ଷର *buf)
{
    ଅକ୍ଷର *name = 0;
    ଅକ୍ଷର namebuf[2];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl  = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା shift = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା alt   = 0;

    ଯଦି (mod_mask & MOD_MASK_CTRL)
	ctrl = 1;
    ଯଦି (mod_mask & MOD_MASK_SHIFT)
	shift = 1;
    ଯଦି (mod_mask & MOD_MASK_ALT)
	alt = 1;


    ବାଛ (ଚାବି)
    {
	ସ୍ଥିତି K_F1:		name = "F1";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F1:	name = "F1";	shift = 1;	ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F2:		name = "F2";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F2:	name = "F2";	shift = 1;	ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F3:		name = "F3";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F3:	name = "F3";	shift = 1;	ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F4:		name = "F4";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F4:	name = "F4";	shift = 1;	ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F5:		name = "F5";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F5:	name = "F5";	shift = 1;	ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F6:		name = "F6";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F6:	name = "F6";	shift = 1;	ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F7:		name = "F7";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F7:	name = "F7";	shift = 1;	ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F8:		name = "F8";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F8:	name = "F8";	shift = 1;	ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F9:		name = "F9";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F9:	name = "F9";	shift = 1;	ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F10:		name = "F10";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F10:	name = "F10";	shift = 1;	ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F11:		name = "F11";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F11:	name = "F11";	shift = 1;	ଭାଙ୍ଗ;
	ସ୍ଥିତି K_F12:		name = "F12";		ଭାଙ୍ଗ;
	ସ୍ଥିତି K_S_F12:	name = "F12";	shift = 1;	ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଯଦି (ଚାବି >= ' ' && ଚାବି <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = ଚାବି;
			    namebuf[1] = NUL;
			}
			ଅନ୍ୟଥା
			    name = "X";
			ଭାଙ୍ଗ;
    }

    buf[0] = '\0';
    ଯଦି (ctrl)
	strcat(buf, "C");
    ଯଦି (shift)
	strcat(buf, "S");
    ଯଦି (alt)
	strcat(buf, "M"); // META
    ଯଦି (ctrl || shift || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#ଯଦି defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    ଶୂନ୍ୟ
netbeans_beval_cb(
	BalloonEval	*beval,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 state UNUSED)
{
    win_T	*wp;
    char_u	*text;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ଅକ୍ଷର	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    ଯଦି (!can_use_beval() || !NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    ଯଦି (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) != OK)
	ଫେରନ୍ତୁ;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    ଯଦି (text != ଖାଲି && text[0] != NUL && STRLEN(text) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	ଯଦି (buf != ଖାଲି)
	{
	    p = nb_quote(text);
	    ଯଦି (p != ଖାଲି)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
netbeans_active(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    ଶୂନ୍ୟ
netbeans_open(ଅକ୍ଷର *params, ପୂର୍ଣ୍ଣସଂଖ୍ୟା doabort)
{
    ଅକ୍ଷର *cmd = "0:startupDone=0\n";

    ଯଦି (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (netbeans_connect(params, doabort) != OK)
	ଫେରନ୍ତୁ;

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
    ଶୂନ୍ୟ
netbeans_send_disconnect(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର buf[128];

    ଯଦି (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#ଯଦି defined(FEAT_EVAL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_nb_channel(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା abort = FALSE;
    typval_T tv;

    ଯଦି (nb_channel == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = set_ref_in_item(&tv, copyID, ଖାଲି, ଖାଲି, ଖାଲି);
    ଫେରନ୍ତୁ abort;
}
#endif

#ଯଦି defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    ଶୂନ୍ୟ
netbeans_frame_moved(ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_y)
{
    ଅକ୍ଷର buf[128];

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#endif

/*
 * Tell netbeans the user opened or activated a file.
 */
    ଶୂନ୍ୟ
netbeans_file_activated(buf_T *bufp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    ଅକ୍ଷର    buffer[2*MAXPATHL];
    char_u  *q;

    ଯଦି (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	ଫେରନ୍ତୁ;

    q = nb_quote(bufp->b_ffname);
    ଯଦି (q == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (bp == ଖାଲି)
    {
	vim_free(q);
	ଫେରନ୍ତୁ;
    }

    vim_snprintf(buffer, ଆକାର(buffer),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (ଅକ୍ଷର *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    ଶୂନ୍ୟ
netbeans_file_opened(buf_T *bufp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufno = nb_getbufno(bufp);
    ଅକ୍ଷର    buffer[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    bnum;

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    q = nb_quote(bufp->b_ffname);
    ଯଦି (q == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (bp != ଖାଲି)
	bnum = bufno;
    ଅନ୍ୟଥା
	bnum = 0;

    vim_snprintf(buffer, ଆକାର(buffer), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (ଅକ୍ଷର *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_opened");
    ଯଦି (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	last_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    ଶୂନ୍ୟ
netbeans_file_killed(buf_T *bufp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    ଅକ୍ଷର	buffer[2*MAXPATHL];

    ଯଦି (!NETBEANS_OPEN || bufno == -1)
	ଫେରନ୍ତୁ;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(buffer, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", buffer));

    nb_send(buffer, "netbeans_file_killed");

    ଯଦି (nbbuf != ଖାଲି)
	nbbuf->bufp = ଖାଲି;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    ସ୍ଥିର nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *bufnop)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufno;
    nbbuf_T	*nbbuf;

    ଯଦି (!NETBEANS_OPEN || !netbeansFireChanges)
	ଫେରନ୍ତୁ ଖାଲି;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    ଯଦି (bufno <= 0)
	ଫେରନ୍ତୁ ଖାଲି;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    ଯଦି (nbbuf != ଖାଲି && !nbbuf->fireChanges)
	ଫେରନ୍ତୁ ଖାଲି;		// changes in this buffer are not reported

    *bufnop = bufno;
    ଫେରନ୍ତୁ nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    ଶୂନ୍ୟ
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    char_u	*txt,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newlen)
{
    char_u	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;
    char_u	*p;
    char_u	*newtxt;

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ଯଦି (nbbuf == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Don't mark as modified for initial read
    ଯଦି (nbbuf->ଭର୍ତ୍ତି_Done)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.col = col;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    ଯଦି (p != ଖାଲି)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((ଅକ୍ଷର *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((ଅକ୍ଷର *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    ଶୂନ୍ୟ
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	col,
    long	len)
{
    char_u	buf[128];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ଯଦି (nbbuf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	ଫେରନ୍ତୁ;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.col = col;

    off = pos2off(bufp, &pos);

    sprintf((ଅକ୍ଷର *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((ଅକ୍ଷର *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    ଶୂନ୍ୟ
netbeans_unmodified(buf_T *bufp UNUSED)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    ଶୂନ୍ୟ
netbeans_button_release(ପୂର୍ଣ୍ଣସଂଖ୍ୟା button)
{
    ଅକ୍ଷର	buf[128];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufno;

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    bufno = nb_getbufno(curbuf);

    ଯଦି (bufno < 0 || curwin == ଖାଲି || curwin->w_buffer != curbuf)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = mouse_col - curwin->w_wincol
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
netbeans_keycommand(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି)
{
    ଅକ୍ଷର	ଚାବି_Name[60];

    netbeans_keyname(ଚାବି, ଚାବି_Name);
    ଫେରନ୍ତୁ netbeans_keystring((char_u *)ଚାବି_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
netbeans_keystring(char_u *ଚାବି_Name)
{
    ଅକ୍ଷର	buf[2*MAXPATHL];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufno = nb_getbufno(curbuf);
    long	off;
    char_u	*q;

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ TRUE;

    ଯଦି (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == ଖାଲି ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	ଯଦି (q == ଖାଲି)
	    ଫେରନ୍ତୁ TRUE;
	vim_snprintf(buf, ଆକାର(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	ଯଦି (curbuf->b_ffname != ଖାଲି)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(ଚାବି_Name);
	ଫେରନ୍ତୁ FALSE;
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
    vim_snprintf(buf, ଆକାର(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, ଚାବି_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, ଆକାର(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, ଚାବି_Name,
		off, (long)curwin->w_cursor.lnum, (long)curwin->w_cursor.col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    ଫେରନ୍ତୁ TRUE;
}


/*
 * Send a save event to netbeans.
 */
    ଶୂନ୍ୟ
netbeans_save_buffer(buf_T *bufp)
{
    char_u	buf[64];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufno;
    nbbuf_T	*nbbuf;

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ଯଦି (nbbuf == ଖାଲି)
	ଫେରନ୍ତୁ;

    nbbuf->modified = 0;

    sprintf((ଅକ୍ଷର *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((ଅକ୍ଷର *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    ଶୂନ୍ୟ
netbeans_deleted_all_ଧାଡ଼ିଗୁଡ଼ିକ(buf_T *bufp)
{
    char_u	buf[64];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufno;
    nbbuf_T	*nbbuf;

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ଯଦି (nbbuf == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Don't mark as modified for initial read
    ଯଦି (nbbuf->ଭର୍ତ୍ତି_Done)
	nbbuf->modified = 1;

    sprintf((ଅକ୍ଷର *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
netbeans_is_guarded(linenr_T ଶୀର୍ଷ, linenr_T bot)
{
    sign_entry_T	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			lnum;

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ FALSE;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
	ଯଦି (p->se_id >= GUARDEDOFFSET)
	    ପାଇଁ (lnum = ଶୀର୍ଷ + 1; lnum < bot; lnum++)
		ଯଦି (lnum == p->se_lnum)
		    ଫେରନ୍ତୁ TRUE;

    ଫେରନ୍ତୁ FALSE;
}

#ଯଦି defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    ଶୂନ୍ୟ
netbeans_draw_multisign_indicator(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା y;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x;

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    x = 0;
    y = row * gui.char_height + 2;

    ପାଇଁ (i = 0; i < gui.char_height - 3; i++)
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

#ଯଦି defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    ଶୂନ୍ୟ
netbeans_draw_multisign_indicator(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା y;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x;
# ଯଦି GTK_CHECK_VERSION(3,0,0)
    cairo_t *cr = ଖାଲି;
# ଅନ୍ୟଥା
    GdkDrawable *drawable = gui.drawarea->window;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# ଅନ୍ୟଥା
    y = row * gui.char_height + 2;
# endif

# ଯଦି GTK_CHECK_VERSION(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_height);
    cr = cairo_create(surf);
#  ଅନ୍ୟଥା
    cr = cairo_create(gui.surface);
#  endif
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    ପାଇଁ (i = 0; i < gui.char_height - 3; i++)
# ଯଦି GTK_CHECK_VERSION(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# ଅନ୍ୟଥା
    gdk_draw_point(drawable, gui.text_gc, x+2, y++);
# endif

# ଯଦି GTK_CHECK_VERSION(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# ଅନ୍ୟଥା
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

# ଯଦି GTK_CHECK_VERSION(3,0,0)
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
    ଶୂନ୍ୟ
netbeans_gutter_click(linenr_T lnum)
{
    sign_entry_T	*p;

    ଯଦି (!NETBEANS_OPEN)
	ଫେରନ୍ତୁ;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
    {
	ଯଦି (p->se_lnum == lnum && p->se_next && p->se_next->se_lnum == lnum)
	{
	    sign_entry_T *tail;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    ଯଦି (p->se_prev)
		p->se_prev->se_next = p->se_next;
	    ଅନ୍ୟଥା
		curbuf->b_signlist = p->se_next;
	    p->se_next->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    ପାଇଁ (tail = p->se_next;
		  tail->se_next && tail->se_next->se_lnum == lnum
				       && tail->se_next->se_id < GUARDEDOFFSET;
		  tail = tail->se_next)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_next = tail->se_next;
	    ଯଦି (tail->se_next)
		tail->se_next->se_prev = p;
	    p->se_prev = tail;
	    tail->se_next = p;
	    update_debug_sign(curbuf, lnum);
	    ଭାଙ୍ଗ;
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
    ସ୍ଥିର ଶୂନ୍ୟ
addsigntype(
    nbbuf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type_ଅଙ୍କ,
    char_u	*typeName,
    char_u	*tooltip UNUSED,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_fg = (*fg && STRCMP(fg, "none") != 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_bg = (*bg && STRCMP(bg, "none") != 0);

    ପାଇଁ (i = 0; i < globalsignmapused; i++)
	ଯଦି (STRCMP(typeName, globalsignmap[i]) == 0)
	    ଭାଙ୍ଗ;

    ଯଦି (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_ଅଙ୍କ, typeName, tooltip, glyphFile, fg, bg));
	ଯଦି (use_fg || use_bg)
	{
	    ଅକ୍ଷର fgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    ଅକ୍ଷର bgbuf[2 * (8 + MAX_COLOR_LENGTH) + 1];
	    ଅକ୍ଷର *ptr;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ;

	    ମୂଲ୍ୟ = strtol((ଅକ୍ଷର *)fg, &ptr, 10);
	    ଯଦି (ptr != (ଅକ୍ଷର *)fg)
		sprintf(fgbuf, "guifg=#%06x", ମୂଲ୍ୟ & 0xFFFFFF);
	    ଅନ୍ୟଥା
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    ମୂଲ୍ୟ = strtol((ଅକ୍ଷର *)bg, &ptr, 10);
	    ଯଦି (ptr != (ଅକ୍ଷର *)bg)
		sprintf(bgbuf, "guibg=#%06x", ମୂଲ୍ୟ & 0xFFFFFF);
	    ଅନ୍ୟଥା
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    ଯଦି (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    ଅନ୍ୟଥା ଯଦି (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    ଅନ୍ୟଥା
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	ଅନ୍ୟଥା
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	ଯଦି (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_ଅଙ୍କ;

	ଯଦି (globalsignmapused == globalsignmaplen)
	{
	    ଯଦି (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(ଅକ୍ଷର *, globalsignmaplen);
	    }
	    ଅନ୍ୟଥା    // grow it
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା incr;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldlen = globalsignmaplen;
		ଅକ୍ଷର **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * ଆକାର(ଅକ୍ଷର *));
		ଯଦି (globalsignmap == ଖାଲି)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    ଫେରନ୍ତୁ;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * ଆକାର(ଅକ୍ଷର *));
	    }
	}

	globalsignmap[i] = (ଅକ୍ଷର *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    ପାଇଁ (j = 0; j < buf->signmapused; j++)
	ଯଦି (buf->signmap[j] == i + 1)
	    ଫେରନ୍ତୁ;

    // add to local map
    ଯଦି (buf->signmapused == buf->signmaplen)
    {
	ଯଦି (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, buf->signmaplen);
	}
	ଅନ୍ୟଥା    // grow it
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା incr;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldlen = buf->signmaplen;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
	    ଯଦି (buf->signmap == ଖାଲି)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		ଫେରନ୍ତୁ;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mapsigntype(nbbuf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା localsigntype)
{
    ଯଦି (--localsigntype >= 0 && localsigntype < buf->signmapused)
	ଫେରନ୍ତୁ buf->signmap[localsigntype];

    ଫେରନ୍ତୁ 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    ସ୍ଥିର long
get_buf_size(buf_T *bufp)
{
    linenr_T	lnum;
    long	char_ଗଣନା = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eol_size;
    long	last_check = 100000L;

    ଯଦି (bufp->b_ml.ml_flags & ML_EMPTY)
	ଫେରନ୍ତୁ 0;

    ଯଦି (get_fileformat(bufp) == EOL_DOS)
	eol_size = 2;
    ଅନ୍ୟଥା
	eol_size = 1;
    ପାଇଁ (lnum = 1; lnum <= bufp->b_ml.ml_ଧାଡ଼ି_ଗଣନା; ++lnum)
    {
	char_ଗଣନା += ml_get_buf_len(bufp, lnum) + eol_size;
	// Check for a CTRL-C every 100000 characters
	ଯଦି (char_ଗଣନା > last_check)
	{
	    ui_breakcheck();
	    ଯଦି (got_int)
		ଫେରନ୍ତୁ char_ଗଣନା;
	    last_check = char_ଗଣନା + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    ଯଦି (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_ଗଣନା -= eol_size;

    ଫେରନ୍ତୁ char_ଗଣନା;
}

/*
 * Convert character offset to lnum,col
 */
    ସ୍ଥିର pos_T *
off2pos(buf_T *buf, long offset)
{
    linenr_T	 lnum;
    ସ୍ଥିର pos_T pos;

    pos.lnum = 0;
    pos.col = 0;
    pos.coladd = 0;

    ଯଦି (!(buf->b_ml.ml_flags & ML_EMPTY))
    {
	ଯଦି ((lnum = ml_find_ଧାଡ଼ି_or_offset(buf, (linenr_T)0, &offset)) < 0)
	    ଫେରନ୍ତୁ ଖାଲି;
	pos.lnum = lnum;
	pos.col = offset;
    }

    ଫେରନ୍ତୁ &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    ସ୍ଥିର pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    ସ୍ଥିର pos_T	mypos;
    long		off;

    off = strtol((ଅକ୍ଷର *)*argp, (ଅକ୍ଷର **)argp, 10);
    ଯଦି (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.col = strtol((ଅକ୍ଷର *)*argp, (ଅକ୍ଷର **)argp, 10);
	mypos.coladd = 0;
	ଫେରନ୍ତୁ &mypos;
    }
    ଫେରନ୍ତୁ off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    ସ୍ଥିର long
pos2off(buf_T *buf, pos_T *pos)
{
    long	 offset = 0;

    ଯଦି (buf->b_ml.ml_flags & ML_EMPTY)
	ଫେରନ୍ତୁ 0;

    ଯଦି ((offset = ml_find_ଧାଡ଼ି_or_offset(buf, pos->lnum, 0)) < 0)
	ଫେରନ୍ତୁ 0;
    offset += pos->col;

    ଫେରନ୍ତୁ offset;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
print_read_ସନ୍ଦେଶ(nbbuf_T *buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    lnum = buf->bufp->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    off_T   nchars = buf->bufp->b_orig_size;
    char_u  c;

    ସନ୍ଦେଶ_add_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    ଯଦି (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    ଯଦି (!buf->bufp->b_start_eol)
    {
	STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    ସନ୍ଦେଶ_add_ଧାଡ଼ିଗୁଡ଼ିକ(c, (long)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_ସନ୍ଦେଶ);
    ସନ୍ଦେଶ_scrolled_ign = TRUE;
    ସନ୍ଦେଶ_trunc_attr((ଅକ୍ଷର *)IObuff, FALSE, 0);
    ସନ୍ଦେଶ_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
print_save_ସନ୍ଦେଶ(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    ଯଦି (nchars >= 0)
    {
	// put fname in IObuff with quotes
	ସନ୍ଦେଶ_add_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	ସନ୍ଦେଶ_add_ଧାଡ଼ିଗୁଡ଼ିକ(c, buf->bufp->b_ml.ml_ଧାଡ଼ି_ଗଣନା,
						buf->bufp->b_orig_size);

	VIM_CLEAR(keep_ସନ୍ଦେଶ);
	ସନ୍ଦେଶ_scrolled_ign = TRUE;
	p = (char_u *)ସନ୍ଦେଶ_trunc_attr((ଅକ୍ଷର *)IObuff, FALSE, 0);
	ଯଦି ((ସନ୍ଦେଶ_scrolled && !need_wait_return) || !buf->initDone)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_ସନ୍ଦେଶ(p, 0);
	}
	ସନ୍ଦେଶ_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    ଅନ୍ୟଥା
    {
	ଅକ୍ଷର msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_add_bang_to_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#endif // defined(FEAT_NETBEANS_INTG)
