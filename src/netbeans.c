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
#define MAX_COLOR_দৈর্ঘ্য 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#define VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#define VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
স্থির অক্ষর *ExtEdProtocolVersion = "2.5";

স্থির long pos2off(buf_T *, pos_T *);
স্থির pos_T *off2pos(buf_T *, long);
স্থির pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
স্থির long get_buf_আকার(buf_T *);
স্থির পূর্ণসংখ্যা netbeans_keystring(char_u *keystr);
স্থির শূন্য special_চাবিগুলি(char_u *args);

স্থির পূর্ণসংখ্যা getConnInfo(অক্ষর *file, অক্ষর **host, অক্ষর **port, অক্ষর **password);

স্থির শূন্য nb_init_graphics(শূন্য);
স্থির শূন্য coloncmd(অক্ষর *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
স্থির শূন্য nb_set_curbuf(buf_T *buf);
স্থির শূন্য nb_parse_cmd(char_u *);
স্থির পূর্ণসংখ্যা  nb_do_cmd(পূর্ণসংখ্যা, char_u *, পূর্ণসংখ্যা, পূর্ণসংখ্যা, char_u *);
স্থির শূন্য nb_send(অক্ষর *buf, অক্ষর *fun);
স্থির শূন্য nb_free(শূন্য);

#define NETBEANS_OPEN (channel_can_write_to(nb_channel))
স্থির channel_T *nb_channel = ফাঁকা;

স্থির পূর্ণসংখ্যা r_cmdno;			// current command number for reply
স্থির পূর্ণসংখ্যা dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ifdef NBDEBUG
# include "nbdebug.c"
#endif

স্থির পূর্ণসংখ্যা needupdate = 0;
স্থির পূর্ণসংখ্যা inAtomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    স্থির পূর্ণসংখ্যা
nb_is_safe_string(char_u *str, char_u *allowed)
{
    যদি (str == ফাঁকা)
	ফেরত FALSE;
    জন্য (char_u *p = str; *p != NUL; p++)
	যদি (vim_strchr(allowed, *p) == ফাঁকা)
	    ফেরত FALSE;
    ফেরত TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    স্থির শূন্য
nb_channel_closed(শূন্য)
{
    nb_channel = ফাঁকা;
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
	যদি (nb_channel != ফাঁকা)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = ফাঁকা;
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

    স্থির পূর্ণসংখ্যা
netbeans_connect(অক্ষর *params, পূর্ণসংখ্যা doabort)
{
    পূর্ণসংখ্যা		port;
    অক্ষর	buf[32];
    অক্ষর	*hostname = ফাঁকা;
    অক্ষর	*address = ফাঁকা;
    অক্ষর	*password = ফাঁকা;
    অক্ষর	*fname;
    অক্ষর	*arg = ফাঁকা;

    যদি (*params == '=')
    {
	// "=fname": Read info from specified file.
	যদি (getConnInfo(params + 1, &hostname, &address, &password) == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা
    {
	যদি (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	যদি (arg == ফাঁকা && (fname = getenv("__NETBEANS_CONINFO")) != ফাঁকা)
	{
	    // "": get info from file specified in environment
	    যদি (getConnInfo(fname, &hostname, &address, &password) == FAIL)
		ফেরত FAIL;
	}
	অন্যথা
	{
	    যদি (arg != ফাঁকা)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		address = strchr(hostname, ':');
		যদি (address != ফাঁকা)
		{
		    *address++ = '\0';
		    password = strchr(address, ':');
		    যদি (password != ফাঁকা)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    যদি (hostname == ফাঁকা || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    যদি (address == ফাঁকা)
		address = getenv("__NETBEANS_SOCKET");
	    যদি (password == ফাঁকা)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    যদি (hostname != ফাঁকা)
		hostname = (অক্ষর *)vim_strsave((char_u *)hostname);
	    যদি (address != ফাঁকা)
		address = (অক্ষর *)vim_strsave((char_u *)address);
	    যদি (password != ফাঁকা)
		password = (অক্ষর *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    যদি (hostname == ফাঁকা || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (অক্ষর *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    যদি (address == ফাঁকা || *address == '\0')
    {
	vim_free(address);
	address = (অক্ষর *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    যদি (password == ফাঁকা || *password == '\0')
    {
	vim_free(password);
	password = (অক্ষর *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    যদি (hostname != ফাঁকা && address != ফাঁকা && password != ফাঁকা)
    {
	port = atoi(address);
	nb_channel = channel_open(hostname, port, 3000, nb_channel_closed);
	যদি (nb_channel != ফাঁকা)
	{
	    // success
#ifdef FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#endif

	    // success, login
	    vim_snprintf(buf, আকার(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", ExtEdProtocolVersion);
	    nb_send(buf, "externaleditor_version");
	}
    }

    যদি (nb_channel == ফাঁকা && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(address);
    vim_free(password);
    ফেরত NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    স্থির পূর্ণসংখ্যা
getConnInfo(অক্ষর *file, অক্ষর **host, অক্ষর **port, অক্ষর **auth)
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
	ফেরত FAIL;
    }
#endif

    fp = mch_fopen(file, "r");
    যদি (fp == ফাঁকা)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	ফেরত FAIL;
    }

    // Read the file. There should be one of each parameter
    যখন ((lp = (char_u *)fgets((অক্ষর *)buf, BUFSIZ, fp)) != ফাঁকা)
    {
	যদি ((nlp = vim_strchr(lp, '\n')) != ফাঁকা)
	    *nlp = 0;	    // strip off the trailing newline

	যদি (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (অক্ষর *)vim_strsave(&buf[5]);
	}
	অন্যথা যদি (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*port);
	    *port = (অক্ষর *)vim_strsave(&buf[5]);
	}
	অন্যথা যদি (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (অক্ষর *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    ফেরত OK;
}


কাঠামো keyqueue
{
    char_u	    *keystr;
    কাঠামো keyqueue *পরবর্তী;
    কাঠামো keyqueue *পূর্ব;
};

প্রকারলেখ কাঠামো keyqueue চাবি_Q_T;

স্থির চাবি_Q_T চাবি_শীর্ষ; // dummy node, header for circular queue


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
    যদি (গ্রন্থি == ফাঁকা)
	ফেরত;  // out of memory, drop the key

    যদি (চাবি_শীর্ষ.পরবর্তী == ফাঁকা) // initialize circular queue
    {
	চাবি_শীর্ষ.পরবর্তী = &চাবি_শীর্ষ;
	চাবি_শীর্ষ.পূর্ব = &চাবি_শীর্ষ;
    }

    // insert node at tail of queue
    গ্রন্থি->পরবর্তী = &চাবি_শীর্ষ;
    গ্রন্থি->পূর্ব = চাবি_শীর্ষ.পূর্ব;
    চাবি_শীর্ষ.পূর্ব->পরবর্তী = গ্রন্থি;
    চাবি_শীর্ষ.পূর্ব = গ্রন্থি;

    গ্রন্থি->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    স্থির শূন্য
handle_চাবি_queue(শূন্য)
{
    পূর্ণসংখ্যা postponed = FALSE;

    যখন (!postponed && চাবি_শীর্ষ.পরবর্তী && চাবি_শীর্ষ.পরবর্তী != &চাবি_শীর্ষ)
    {
	// first, unlink the node
	চাবি_Q_T *গ্রন্থি = চাবি_শীর্ষ.পরবর্তী;
	চাবি_শীর্ষ.পরবর্তী = গ্রন্থি->পরবর্তী;
	গ্রন্থি->পরবর্তী->পূর্ব = গ্রন্থি->পূর্ব;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	যদি (গ্রন্থি->keystr != ফাঁকা)
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
    char_u	*বাফার;
    char_u	*p;
    পূর্ণসংখ্যা		own_গ্রন্থি;

    যখন (nb_channel != ফাঁকা)
    {
	গ্রন্থি = channel_peek(nb_channel, PART_SOCK);
	যদি (গ্রন্থি == ফাঁকা)
	    বিরতি;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_first_nl(গ্রন্থি);
	যদি (p == ফাঁকা)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    যদি (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		ফেরত;
	    চালু;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	যদি (*p == NUL)
	{
	    own_গ্রন্থি = TRUE;
	    বাফার = channel_get(nb_channel, PART_SOCK, ফাঁকা);
	    // "node" is now invalid!
	}
	অন্যথা
	{
	    own_গ্রন্থি = FALSE;
	    বাফার = গ্রন্থি->rq_বাফার;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(বাফার);

	যদি (own_গ্রন্থি)
	    // buffer finished, dispose of it
	    vim_free(বাফার);
	অন্যথা যদি (nb_channel != ফাঁকা)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (পূর্ণসংখ্যা)(p - বাফার));
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
    অক্ষর	*verb;
    অক্ষর	*q;
    পূর্ণসংখ্যা		bufno;
    পূর্ণসংখ্যা		isfunc = -1;

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
	ফেরত;
    }

    bufno = strtol((অক্ষর *)cmd, &verb, 10);

    যদি (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	ফেরত;
    }
    ++verb; // skip colon

    জন্য (q = verb; *q; q++)
    {
	যদি (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    বিরতি;
	}
	অন্যথা যদি (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    বিরতি;
	}
    }

    যদি (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_in_str), cmd);
	ফেরত;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (অক্ষর *)skipwhite((char_u *)q);

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

কাঠামো nbbuf_struct
{
    buf_T		*bufp;
    unsigned পূর্ণসংখ্যা	 fireChanges:1;
    unsigned পূর্ণসংখ্যা	 initDone:1;
    unsigned পূর্ণসংখ্যা	 ঢোকাও_Done:1;
    unsigned পূর্ণসংখ্যা	 modified:1;
    পূর্ণসংখ্যা			 nbbuf_সংখ্যা;
    অক্ষর		*displayname;
    পূর্ণসংখ্যা			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

প্রকারলেখ কাঠামো nbbuf_struct nbbuf_T;

স্থির nbbuf_T *buf_list = ফাঁকা;
স্থির পূর্ণসংখ্যা buf_list_আকার = 0;	// size of buf_list
স্থির পূর্ণসংখ্যা buf_list_used = 0;	// nr of entries in buf_list actually in use

স্থির অক্ষর **globalsignmap = ফাঁকা;
স্থির পূর্ণসংখ্যা globalsignmaplen = 0;
স্থির পূর্ণসংখ্যা globalsignmapused = 0;

স্থির পূর্ণসংখ্যা  mapsigntype(nbbuf_T *, পূর্ণসংখ্যা localsigntype);
স্থির শূন্য addsigntype(nbbuf_T *, পূর্ণসংখ্যা localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
স্থির শূন্য print_read_বার্তা(nbbuf_T *buf);
স্থির শূন্য print_save_বার্তা(nbbuf_T *buf, off_T nchars);

স্থির পূর্ণসংখ্যা curPCtype = -1;

/*
 * Free netbeans resources.
 */
    স্থির শূন্য
nb_free(শূন্য)
{
    চাবি_Q_T *চাবি_গ্রন্থি = চাবি_শীর্ষ.পরবর্তী;
    nbbuf_T buf;
    পূর্ণসংখ্যা i;

    // free the netbeans buffer list
    জন্য (i = 0; i < buf_list_used; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	যদি (buf.bufp != ফাঁকা && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_আকার = 0;
    buf_list_used = 0;

    // free the queued key commands
    যখন (চাবি_গ্রন্থি != ফাঁকা && চাবি_গ্রন্থি != &চাবি_শীর্ষ)
    {
	চাবি_Q_T *পরবর্তী = চাবি_গ্রন্থি->পরবর্তী;
	vim_free(চাবি_গ্রন্থি->keystr);
	vim_free(চাবি_গ্রন্থি);
	যদি (পরবর্তী == &চাবি_শীর্ষ)
	{
	    চাবি_শীর্ষ.পরবর্তী = &চাবি_শীর্ষ;
	    চাবি_শীর্ষ.পূর্ব = &চাবি_শীর্ষ;
	    বিরতি;
	}
	চাবি_গ্রন্থি = পরবর্তী;
    }

    // free the queued netbeans commands
    যদি (nb_channel != ফাঁকা)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    স্থির পূর্ণসংখ্যা
nb_getbufno(buf_T *bufp)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < buf_list_used; i++)
	যদি (buf_list[i].bufp == bufp)
	    ফেরত i;
    ফেরত -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    পূর্ণসংখ্যা
is_Netbeans_বাফার(buf_T *bufp)
{
    ফেরত NETBEANS_OPEN && bufp->b_netbeans_file;
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
    পূর্ণসংখ্যা
isNetbeansModified(buf_T *bufp)
{
    যদি (is_Netbeans_বাফার(bufp))
    {
	পূর্ণসংখ্যা bufno = nb_getbufno(bufp);

	যদি (bufno > 0)
	    ফেরত buf_list[bufno].modified;
	অন্যথা
	    ফেরত FALSE;
    }
    অন্যথা
	ফেরত FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    স্থির nbbuf_T *
nb_get_buf(পূর্ণসংখ্যা bufno)
{
    // find or create a buffer with the given number
    পূর্ণসংখ্যা incr;

    যদি (bufno <= 0)
	ফেরত ফাঁকা;

    যদি (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * আকার(nbbuf_T));
	buf_list_আকার = 100;
    }
    যদি (bufno >= buf_list_used) // new
    {
	যদি (bufno >= buf_list_আকার) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_আকার + 90;
	    buf_list_আকার += incr;
	    bufsize = buf_list_আকার * আকার(nbbuf_T);
	    যদি (bufsize == 0 || bufsize / আকার(nbbuf_T)
						      != (size_t)buf_list_আকার)
	    {
		// list size overflow, bail out
		ফেরত ফাঁকা;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    যদি (buf_list == ফাঁকা)
	    {
		vim_free(t_buf_list);
		buf_list_আকার = 0;
		ফেরত ফাঁকা;
	    }
	    vim_memset(buf_list + buf_list_আকার - incr, 0,
						      incr * আকার(nbbuf_T));
	}

	যখন (buf_list_used <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_used].fireChanges = 1;
	    ++buf_list_used;
	}
    }

    ফেরত buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    স্থির পূর্ণসংখ্যা
গুনতি_changed_buffers(শূন্য)
{
    buf_T	*bufp;
    পূর্ণসংখ্যা		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	যদি (bufp->b_changed)
	    ++n;
    ফেরত n;
}

/*
 * End the netbeans session.
 */
    শূন্য
netbeans_end(শূন্য)
{
    পূর্ণসংখ্যা i;
    স্থির অক্ষর buf[128];

    যদি (!NETBEANS_OPEN)
	ফেরত;

    জন্য (i = 0; i < buf_list_used; i++)
    {
	যদি (!buf_list[i].bufp)
	    চালু;
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
	nb_send(buf, ফাঁকা);
	buf_list[i].bufp = ফাঁকা;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    স্থির শূন্য
nb_send(অক্ষর *buf, অক্ষর *fun)
{
    যদি (nb_channel != ফাঁকা)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (পূর্ণসংখ্যা)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    স্থির শূন্য
nb_reply_nil(পূর্ণসংখ্যা cmdno)
{
    অক্ষর reply[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    যদি (!NETBEANS_OPEN)
	ফেরত;

    sprintf(reply, "%d\n", cmdno);
    nb_send(reply, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    স্থির শূন্য
nb_reply_text(পূর্ণসংখ্যা cmdno, char_u *ফলাফল)
{
    char_u *reply;

    nbdebug(("REP %d: %s\n", cmdno, (অক্ষর *)ফলাফল));

    reply = alloc(STRLEN(ফলাফল) + 32);
    sprintf((অক্ষর *)reply, "%d %s\n", cmdno, (অক্ষর *)ফলাফল);
    nb_send((অক্ষর *)reply, "nb_reply_text");

    vim_free(reply);
}


/*
 * Send a response with a number result code.
 */
    স্থির শূন্য
nb_reply_nr(পূর্ণসংখ্যা cmdno, long ফলাফল)
{
    অক্ষর reply[32];

    nbdebug(("REP %d: %ld\n", cmdno, ফলাফল));

    sprintf(reply, "%d %ld\n", cmdno, ফলাফল);
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

    যদি (buf == ফাঁকা)
	ফেরত ফাঁকা;
    জন্য (; *p; p++)
    {
	নির্বাচন (*p)
	{
	    ক্ষেত্র '\"':
	    ক্ষেত্র '\\':
		*q++ = '\\'; *q++ = *p; বিরতি;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    ক্ষেত্র '\n':
		*q++ = '\\'; *q++ = 'n'; বিরতি;
	    ক্ষেত্র '\r':
		*q++ = '\\'; *q++ = 'r'; বিরতি;
	    পূর্বনির্ধারিত:
		*q++ = *p;
		বিরতি;
	}
    }
    *q = '\0';

    ফেরত buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    স্থির অক্ষর *
nb_unquote(char_u *p, char_u **endp)
{
    অক্ষর *ফলাফল = 0;
    অক্ষর *q;
    পূর্ণসংখ্যা done = 0;

    // result is never longer than input
    ফলাফল = alloc_clear(STRLEN(p) + 1);
    যদি (ফলাফল == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	ফলাফল[0] = NUL;
	ফেরত ফলাফল;
    }

    জন্য (q = ফলাফল; !done && *p != NUL;)
    {
	নির্বাচন (*p)
	{
	    ক্ষেত্র '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		done = 1;
		বিরতি;

	    ক্ষেত্র '\\':
		++p;
		নির্বাচন (*p)
		{
		    ক্ষেত্র '\\':	*q++ = '\\';	বিরতি;
		    ক্ষেত্র 'n':	*q++ = '\n';	বিরতি;
		    ক্ষেত্র 't':	*q++ = '\t';	বিরতি;
		    ক্ষেত্র 'r':	*q++ = '\r';	বিরতি;
		    ক্ষেত্র '"':	*q++ = '"';	বিরতি;
		    ক্ষেত্র NUL:	--p;		বিরতি;
		    // default: skip over illegal chars
		}
		++p;
		বিরতি;

	    পূর্বনির্ধারিত:
		*q++ = *p++;
	}
    }

    যদি (endp != ফাঁকা)
	*endp = p;

    ফেরত ফলাফল;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    স্থির শূন্য
nb_partialremove(linenr_T lnum, colnr_T first, colnr_T last)
{
    char_u *oldtext, *newtext;
    পূর্ণসংখ্যা oldlen;
    পূর্ণসংখ্যা lastbyte = last;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    যদি (first >= (colnr_T)oldlen || oldlen == 0)  // just in case
	ফেরত;
    যদি (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - first));
    যদি (newtext == ফাঁকা)
	ফেরত;

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
    পূর্ণসংখ্যা len_first, len_other;
    char_u *p;

    len_first = ml_get_len(first);
    len_other = ml_get_len(other);
    p = alloc(len_first + len_other + 1);
    যদি (p == ফাঁকা)
	ফেরত;

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
    স্থির পূর্ণসংখ্যা
nb_do_cmd(
    পূর্ণসংখ্যা		bufno,
    char_u	*cmd,
    পূর্ণসংখ্যা		func,
    পূর্ণসংখ্যা		cmdno,
    char_u	*args)	    // points to space before arguments or NUL
{
    পূর্ণসংখ্যা		do_update = 0;
    long	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    স্থির পূর্ণসংখ্যা	skip = 0;
    পূর্ণসংখ্যা		retval = OK;
    অক্ষর	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (অক্ষর *)args));

    যদি (func)
    {
// =====================================================================
	যদি (streq((অক্ষর *)cmd, "getModified"))
	{
	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
		// Return the number of buffers that are modified.
		nb_reply_nr(cmdno, (long)গুনতি_changed_buffers());
	    অন্যথা
		// Return whether the buffer is modified.
		nb_reply_nr(cmdno, (long)(buf->bufp->b_changed
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_reply_nr(cmdno, (long)গুনতি_changed_buffers());
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "getCursor"))
	{
	    char_u text[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((অক্ষর *)text, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (long)curwin->w_cursor.lnum,
		    (পূর্ণসংখ্যা)curwin->w_cursor.col,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_reply_text(cmdno, text);
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "getAnno"))
	{
	    long linenum = 0;
#ifdef FEAT_SIGNS
	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_বাফার_identifier_in_getanno));
		retval = FAIL;
	    }
	    অন্যথা
	    {
		পূর্ণসংখ্যা ser_অঙ্ক;

		cp = (অক্ষর *)args;
		ser_অঙ্ক = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (long)buf_findsign(buf->bufp, ser_অঙ্ক, ফাঁকা);
	    }
#endif
	    nb_reply_nr(cmdno, linenum);
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "getLength"))
	{
	    long len = 0;

	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_বাফার_identifier_in_getlength));
		retval = FAIL;
	    }
	    অন্যথা
	    {
		len = get_buf_আকার(buf->bufp);
	    }
	    nb_reply_nr(cmdno, len);
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "getText"))
	{
	    long	len;
	    linenr_T	nlines;
	    char_u	*text = ফাঁকা;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*সারি;

	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_বাফার_identifier_in_gettext));
		retval = FAIL;
	    }
	    অন্যথা
	    {
		len = get_buf_আকার(buf->bufp);
		nlines = buf->bufp->b_ml.ml_সারি_গুনতি;
		text = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		যদি (text == ফাঁকা)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		অন্যথা
		{
		    p = text;
		    *p++ = '\"';
		    জন্য (; lno <= nlines ; lno++)
		    {
			সারি = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			যদি (সারি != ফাঁকা)
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
	    যদি (text == ফাঁকা)
		nb_reply_text(cmdno, (char_u *)"");
	    অন্যথা
	    {
		nb_reply_text(cmdno, text);
		vim_free(text);
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "remove"))
	{
	    long গুনতি;
	    pos_T first, last;
	    pos_T *pos;
	    pos_T *পরবর্তী;
	    linenr_T del_from_lnum, del_to_lnum;  // lines to be deleted as a whole
	    পূর্ণসংখ্যা oldFire = netbeansFireChanges;
	    পূর্ণসংখ্যা oldSuppress = netbeans_Suppress_No_সারিগুলি;
	    পূর্ণসংখ্যা wasChanged;

	    যদি (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (অক্ষর *) cmd));
		nb_reply_nil(cmdno);
		ফেরত OK;
	    }

	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_বাফার_identifier_in_remove));
		retval = FAIL;
	    }
	    অন্যথা
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_সারিগুলি = TRUE;

		nb_set_curbuf(buf->bufp);
		wasChanged = buf->bufp->b_changed;
		cp = (অক্ষর *)args;
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
		    ফেরত FAIL;
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
		    ফেরত FAIL;
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
		পরবর্তী = off2pos(buf->bufp, off + গুনতি);

		// Remove part of the first line.
		যদি (first.col != 0
				|| (পরবর্তী != ফাঁকা && first.lnum == পরবর্তী->lnum))
		{
		    যদি (first.lnum != last.lnum
			    || (পরবর্তী != ফাঁকা && first.lnum != পরবর্তী->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(first.lnum, first.col,
							     (colnr_T)MAXCOL);
			যদি (first.lnum == last.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(first.lnum, পরবর্তী->lnum);
			    del_to_lnum = পরবর্তী->lnum;
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
		যদি (first.lnum != last.lnum && পরবর্তী != ফাঁকা
			&& পরবর্তী->col != 0 && last.lnum == পরবর্তী->lnum)
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
			&& পরবর্তী != ফাঁকা && last.lnum != পরবর্তী->lnum)
		{
		    nb_joinlines(first.lnum, পরবর্তী->lnum);
		    del_to_lnum = পরবর্তী->lnum;
		}

		// Delete whole lines if there are any.
		যদি (del_to_lnum >= del_from_lnum)
		{
		    পূর্ণসংখ্যা i;

		    // delete signs from the lines being deleted
		    জন্য (i = del_from_lnum; i <= del_to_lnum; i++)
		    {
			পূর্ণসংখ্যা id = buf_findsign_id(buf->bufp, (linenr_T)i, ফাঁকা);
			যদি (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, ফাঁকা);
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "insert"))
	{
	    char_u	*to_free;

	    যদি (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (অক্ষর *) cmd));
		nb_reply_nil(cmdno);
		ফেরত OK;
	    }

	    // get offset
	    cp = (অক্ষর *)args;
	    off = strtol(cp, &cp, 10);
	    args = (char_u *)cp;

	    // get text to be inserted
	    args = skipwhite(args);
	    args = to_free = (char_u *)nb_unquote(args, ফাঁকা);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_বাফার_identifier_in_ঢোকাও));
		retval = FAIL;
	    }
	    অন্যথা যদি (args != ফাঁকা)
	    {
		পূর্ণসংখ্যা	ff_detected = EOL_UNKNOWN;
		পূর্ণসংখ্যা	buf_was_empty = (buf->bufp->b_ml.ml_flags & ML_EMPTY);
		size_t	len = 0;
		পূর্ণসংখ্যা	added = 0;
		পূর্ণসংখ্যা	oldFire = netbeansFireChanges;
		পূর্ণসংখ্যা	old_b_changed;
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
		যদি (pos != ফাঁকা)
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
		যখন (*args != NUL)
		{
		    nlp = vim_strchr(args, '\n');
		    যদি (nlp == ফাঁকা)
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
			    && ((pos != ফাঁকা && pos->col > 0)
				|| (lnum == 1 && buf_was_empty)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			পূর্ণসংখ্যা	col = pos == ফাঁকা ? 0 : pos->col;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			যদি (newline != ফাঁকা)
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

		    যদি (nlp == ফাঁকা)
			বিরতি;
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
	যদি (streq((অক্ষর *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    যদি (buf == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_বাফার_identifier_in_create));
		ফেরত FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, ফাঁকা, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "insertDone"))
	{
	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "saveDone"))
	{
	    long savedChars = atol((অক্ষর *)args);

	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    অন্যথা
		print_save_বার্তা(buf, savedChars);
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "startDocumentListen"))
	{
	    যদি (buf == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_বাফার_identifier_in_startdocumentlisten));
		ফেরত FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "stopDocumentListen"))
	{
	    যদি (buf == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_বাফার_identifier_in_stopdocumentlisten));
		ফেরত FAIL;
	    }
	    buf->fireChanges = 0;
	    যদি (buf->bufp != ফাঁকা && buf->bufp->b_was_netbeans_file)
	    {
		যদি (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_lost_for_বাফার_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_lost_for_বাফার_nr),
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "setTitle"))
	{
	    যদি (buf == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_বাফার_identifier_in_settitle));
		ফেরত FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ফাঁকা);
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "initDone"))
	{
	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_বাফার_identifier_in_initdone));
		ফেরত FAIL;
	    }
	    do_update = 1;
	    buf->initDone = TRUE;
	    nb_set_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_চাবি_queue();
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "setBufferNumber")
		|| streq((অক্ষর *)cmd, "putBufferNumber"))
	{
	    char_u	*path;
	    buf_T	*bufp;

	    যদি (buf == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_বাফার_identifier_in_setbuffernumber));
		ফেরত FAIL;
	    }
	    path = (char_u *)nb_unquote(args, ফাঁকা);
	    যদি (path == ফাঁকা)
		ফেরত FAIL;
	    bufp = buflist_findname(path);
	    vim_free(path);
	    যদি (bufp == ফাঁকা)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", args));
		semsg(_(e_file_str_not_found_in_setbuffernumber), args);
		ফেরত FAIL;
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "setFullName"))
	{
	    যদি (buf == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_বাফার_identifier_in_setfullname));
		ফেরত FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ফাঁকা);

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
	অন্যথা যদি (streq((অক্ষর *)cmd, "editFile"))
	{
	    যদি (buf == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_বাফার_identifier_in_editfile));
		ফেরত FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(args, ফাঁকা);
	    do_ecmd(0, (char_u *)buf->displayname, ফাঁকা, ফাঁকা, ECMD_ONE,
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "setVisible"))
	{
	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		যদি (p_verbose > 0)
		    emsg(_(e_invalid_বাফার_identifier_in_setvisible));
		ফেরত FAIL;
	    }
	    যদি (streq((অক্ষর *)args, "T") && buf->bufp != curbuf)
	    {
		exarg_T exarg;
		CLEAR_FIELD(exarg);
		exarg.cmd = (char_u *)"goto";
		exarg.forceit = FALSE;
		exarg.cmdidx = CMD_USER;
		dosetvisible = TRUE;
		goto_বাফার(&exarg, DOBUF_FIRST, FORWARD, buf->bufp->b_fnum);
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "raise"))
	{
#ifdef FEAT_GUI
	    // Bring gvim to the foreground.
	    যদি (gui.in_use)
		gui_mch_set_foreground();
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "setModified"))
	{
	    পূর্ণসংখ্যা পূর্ব_b_changed;

	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		যদি (p_verbose > 0)
		    emsg(_(e_invalid_বাফার_identifier_in_setmodified));
		ফেরত FAIL;
	    }
	    পূর্ব_b_changed = buf->bufp->b_changed;
	    যদি (streq((অক্ষর *)args, "T"))
		buf->bufp->b_changed = TRUE;
	    অন্যথা
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		যদি (buf->bufp->b_ffname != ফাঁকা
			&& mch_stat((অক্ষর *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_time(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_changed = FALSE;
	    }
	    buf->modified = buf->bufp->b_changed;
	    যদি (পূর্ব_b_changed != buf->bufp->b_changed)
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "setModtime"))
	{
	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    অন্যথা
	    {
		buf->bufp->b_mtime = atoi((অক্ষর *)args);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "setReadOnly"))
	{
	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    অন্যথা যদি (streq((অক্ষর *)args, "T"))
		buf->bufp->b_p_ro = TRUE;
	    অন্যথা
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "showBalloon"))
	{
#যদি defined(FEAT_BEVAL_GUI)
	    স্থির অক্ষর	*text = ফাঁকা;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    যদি (balloonEval != ফাঁকা)
	    {
		vim_free(text);
		text = nb_unquote(args, ফাঁকা);
		যদি (text != ফাঁকা)
		    gui_mch_post_balloon(balloonEval, (char_u *)text);
	    }
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "setDot"))
	{
	    pos_T *pos;
#ifdef NBDEBUG
	    char_u *s;
#endif

	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_বাফার_identifier_in_setdot));
		ফেরত FAIL;
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "close"))
	{
#ifdef NBDEBUG
	    অক্ষর *name = "<NONE>";
#endif

	    যদি (buf == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_বাফার_identifier_in_close));
		ফেরত FAIL;
	    }

#ifdef NBDEBUG
	    যদি (buf->displayname != ফাঁকা)
		name = buf->displayname;
#endif
	    যদি (buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		যদি (p_verbose > 0)
		    emsg(_(e_invalid_বাফার_identifier_in_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    যদি (buf->bufp != ফাঁকা)
		do_বাফার(DOBUF_WIPE, DOBUF_FIRST, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = ফাঁকা;
	    buf->initDone = FALSE;
	    do_update = 1;
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "defineAnnoType"))
	{
#ifdef FEAT_SIGNS
	    পূর্ণসংখ্যা type_অঙ্ক;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    পূর্ণসংখ্যা parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    যদি (buf == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_বাফার_identifier_in_defineannotype));
		ফেরত FAIL;
	    }

	    cp = (অক্ষর *)args;
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
	    যদি (STRLEN(fg) > MAX_COLOR_দৈর্ঘ্য || STRLEN(bg) > MAX_COLOR_দৈর্ঘ্য)
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
	    অন্যথা যদি (typeName != ফাঁকা && tooltip != ফাঁকা && glyphFile != ফাঁকা)
		addsigntype(buf, type_অঙ্ক, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    যদি (parse_error)
		ফেরত FAIL;

#endif
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "addAnno"))
	{
#ifdef FEAT_SIGNS
	    পূর্ণসংখ্যা ser_অঙ্ক;
	    পূর্ণসংখ্যা local_Type_অঙ্ক;
	    পূর্ণসংখ্যা type_অঙ্ক;
	    pos_T *pos;

	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_বাফার_identifier_in_addanno));
		ফেরত FAIL;
	    }

	    do_update = 1;

	    cp = (অক্ষর *)args;
	    ser_অঙ্ক = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_অঙ্ক = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    type_অঙ্ক = mapsigntype(buf, local_Type_অঙ্ক);

	    pos = get_off_or_lnum(buf->bufp, &args);

	    cp = (অক্ষর *)args;
	    vim_ignored = (পূর্ণসংখ্যা)strtol(cp, &cp, 10);
	    args = (char_u *)cp;
# ifdef NBDEBUG
	    যদি (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    যদি (ser_অঙ্ক >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		ফেরত FAIL;
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "removeAnno"))
	{
#ifdef FEAT_SIGNS
	    পূর্ণসংখ্যা ser_অঙ্ক;

	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		ফেরত FAIL;
	    }
	    do_update = 1;
	    cp = (অক্ষর *)args;
	    ser_অঙ্ক = strtol(cp, &cp, 10);
	    args = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_অঙ্ক, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "moveAnnoToFront"))
	{
#ifdef FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#endif
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "guard") || streq((অক্ষর *)cmd, "unguard"))
	{
	    পূর্ণসংখ্যা len;
	    pos_T first;
	    pos_T last;
	    pos_T *pos;
	    পূর্ণসংখ্যা un = (cmd[0] == 'u');
	    স্থির পূর্ণসংখ্যা guardId = GUARDEDOFFSET;

	    যদি (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (অক্ষর *) cmd));
		ফেরত OK;
	    }

	    nb_init_graphics();

	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ফেরত FAIL;
	    }
	    nb_set_curbuf(buf->bufp);
	    cp = (অক্ষর *)args;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, ফাঁকা, 10);
	    args = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    যদি (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    অন্যথা
	    {
		first = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		যদি (pos != ফাঁকা && pos->col == 0)
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
		    জন্য (lnum = first.lnum; lnum <= last.lnum; lnum++)
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "startAtomic"))
	{
	    inAtomic = 1;
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "endAtomic"))
	{
	    inAtomic = 0;
	    যদি (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ফেরত FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    যদি (bufIsChanged(buf->bufp))
	    {
		// Only write if the buffer can be written.
		যদি (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != ফাঁকা
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    set_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    যদি (!bufref_valid(&bufref))
			buf->bufp = ফাঁকা;
		}
	    }
	    অন্যথা
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "netbeansBuffer"))
	{
	    যদি (buf == ফাঁকা || buf->bufp == ফাঁকা)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ফেরত FAIL;
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
	অন্যথা যদি (streq((অক্ষর *)cmd, "specialKeys"))
	{
	    special_চাবিগুলি(args);
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	অন্যথা যদি (streq((অক্ষর *)cmd, "version"))
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
    যদি (buf != ফাঁকা && buf->initDone && do_update)
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

    ফেরত retval;
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
	ফেরত;

    যদি (buf_jump_open_win(buf) != ফাঁকা)
	ফেরত;
    যদি ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != ফাঁকা)
	ফেরত;
    set_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    স্থির শূন্য
coloncmd(অক্ষর *cmd, ...)
{
    অক্ষর buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, আকার(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, ফাঁকা, ফাঁকা, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    out_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    স্থির শূন্য
special_চাবিগুলি(char_u *args)
{
    অক্ষর *save_str = nb_unquote(args, ফাঁকা);
    অক্ষর *tok = strtok(save_str, " ");
    অক্ষর *sep;
#define KEYBUFLEN 64
    অক্ষর keybuf[KEYBUFLEN];
    অক্ষর cmdbuf[256];

    যখন (tok != ফাঁকা)
    {
	পূর্ণসংখ্যা i = 0;

	যদি ((sep = strchr(tok, '-')) != ফাঁকা)
	{
	    *sep = NUL;
	    যখন (*tok && i + 2 < KEYBUFLEN)
	    {
		নির্বাচন (*tok)
		{
		    ক্ষেত্র 'A':
		    ক্ষেত্র 'M':
		    ক্ষেত্র 'C':
		    ক্ষেত্র 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			বিরতি;
		}
		tok++;
	    }
	    tok++;
	}

	যদি (strlen(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    পূর্ণসংখ্যা safe = TRUE;
	    জন্য (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		যদি (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    safe = FALSE;
		    বিরতি;
		}
	    }
	    যদি (safe)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, আকার(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, MODE_NORMAL, FALSE);
	    }
	}
	tok = strtok(ফাঁকা, " ");
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
	ফেরত;
    }
# endif
#endif
    netbeans_open((অক্ষর *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    স্থির শূন্য
nb_init_graphics(শূন্য)
{
    স্থির পূর্ণসংখ্যা did_init = FALSE;

    যদি (did_init)
	ফেরত;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    স্থির শূন্য
netbeans_keyname(পূর্ণসংখ্যা চাবি, অক্ষর *buf)
{
    অক্ষর *name = 0;
    অক্ষর namebuf[2];
    পূর্ণসংখ্যা ctrl  = 0;
    পূর্ণসংখ্যা shift = 0;
    পূর্ণসংখ্যা alt   = 0;

    যদি (mod_mask & MOD_MASK_CTRL)
	ctrl = 1;
    যদি (mod_mask & MOD_MASK_SHIFT)
	shift = 1;
    যদি (mod_mask & MOD_MASK_ALT)
	alt = 1;


    নির্বাচন (চাবি)
    {
	ক্ষেত্র K_F1:		name = "F1";		বিরতি;
	ক্ষেত্র K_S_F1:	name = "F1";	shift = 1;	বিরতি;
	ক্ষেত্র K_F2:		name = "F2";		বিরতি;
	ক্ষেত্র K_S_F2:	name = "F2";	shift = 1;	বিরতি;
	ক্ষেত্র K_F3:		name = "F3";		বিরতি;
	ক্ষেত্র K_S_F3:	name = "F3";	shift = 1;	বিরতি;
	ক্ষেত্র K_F4:		name = "F4";		বিরতি;
	ক্ষেত্র K_S_F4:	name = "F4";	shift = 1;	বিরতি;
	ক্ষেত্র K_F5:		name = "F5";		বিরতি;
	ক্ষেত্র K_S_F5:	name = "F5";	shift = 1;	বিরতি;
	ক্ষেত্র K_F6:		name = "F6";		বিরতি;
	ক্ষেত্র K_S_F6:	name = "F6";	shift = 1;	বিরতি;
	ক্ষেত্র K_F7:		name = "F7";		বিরতি;
	ক্ষেত্র K_S_F7:	name = "F7";	shift = 1;	বিরতি;
	ক্ষেত্র K_F8:		name = "F8";		বিরতি;
	ক্ষেত্র K_S_F8:	name = "F8";	shift = 1;	বিরতি;
	ক্ষেত্র K_F9:		name = "F9";		বিরতি;
	ক্ষেত্র K_S_F9:	name = "F9";	shift = 1;	বিরতি;
	ক্ষেত্র K_F10:		name = "F10";		বিরতি;
	ক্ষেত্র K_S_F10:	name = "F10";	shift = 1;	বিরতি;
	ক্ষেত্র K_F11:		name = "F11";		বিরতি;
	ক্ষেত্র K_S_F11:	name = "F11";	shift = 1;	বিরতি;
	ক্ষেত্র K_F12:		name = "F12";		বিরতি;
	ক্ষেত্র K_S_F12:	name = "F12";	shift = 1;	বিরতি;
	পূর্বনির্ধারিত:
			যদি (চাবি >= ' ' && চাবি <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = চাবি;
			    namebuf[1] = NUL;
			}
			অন্যথা
			    name = "X";
			বিরতি;
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
	পূর্ণসংখ্যা		 state UNUSED)
{
    win_T	*wp;
    char_u	*text;
    linenr_T	lnum;
    পূর্ণসংখ্যা		col;
    অক্ষর	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    যদি (!can_use_beval() || !NETBEANS_OPEN)
	ফেরত;

    যদি (get_beval_info(beval, TRUE, &wp, &lnum, &text, &col) != OK)
	ফেরত;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    যদি (text != ফাঁকা && text[0] != NUL && STRLEN(text) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	যদি (buf != ফাঁকা)
	{
	    p = nb_quote(text);
	    যদি (p != ফাঁকা)
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
    পূর্ণসংখ্যা
netbeans_active(শূন্য)
{
    ফেরত NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    শূন্য
netbeans_open(অক্ষর *params, পূর্ণসংখ্যা doabort)
{
    অক্ষর *cmd = "0:startupDone=0\n";

    যদি (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	ফেরত;
    }

    যদি (netbeans_connect(params, doabort) != OK)
	ফেরত;

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
    অক্ষর buf[128];

    যদি (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#যদি defined(FEAT_EVAL)
    পূর্ণসংখ্যা
set_ref_in_nb_channel(পূর্ণসংখ্যা copyID)
{
    পূর্ণসংখ্যা abort = FALSE;
    typval_T tv;

    যদি (nb_channel == ফাঁকা)
	ফেরত FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = set_ref_in_item(&tv, copyID, ফাঁকা, ফাঁকা, ফাঁকা);
    ফেরত abort;
}
#endif

#যদি defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    শূন্য
netbeans_frame_moved(পূর্ণসংখ্যা new_x, পূর্ণসংখ্যা new_y)
{
    অক্ষর buf[128];

    যদি (!NETBEANS_OPEN)
	ফেরত;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (পূর্ণসংখ্যা)Columns, (পূর্ণসংখ্যা)Rows, new_x, new_y);
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
    পূর্ণসংখ্যা bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    অক্ষর    বাফার[2*MAXPATHL];
    char_u  *q;

    যদি (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	ফেরত;

    q = nb_quote(bufp->b_ffname);
    যদি (q == ফাঁকা)
	ফেরত;
    যদি (bp == ফাঁকা)
    {
	vim_free(q);
	ফেরত;
    }

    vim_snprintf(বাফার, আকার(বাফার),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (অক্ষর *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", বাফার));

    nb_send(বাফার, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    শূন্য
netbeans_file_opened(buf_T *bufp)
{
    পূর্ণসংখ্যা bufno = nb_getbufno(bufp);
    অক্ষর    বাফার[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    পূর্ণসংখ্যা	    bnum;

    যদি (!NETBEANS_OPEN)
	ফেরত;

    q = nb_quote(bufp->b_ffname);
    যদি (q == ফাঁকা)
	ফেরত;
    যদি (bp != ফাঁকা)
	bnum = bufno;
    অন্যথা
	bnum = 0;

    vim_snprintf(বাফার, আকার(বাফার), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (অক্ষর *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", বাফার));

    nb_send(বাফার, "netbeans_file_opened");
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
    পূর্ণসংখ্যা		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    অক্ষর	বাফার[2*MAXPATHL];

    যদি (!NETBEANS_OPEN || bufno == -1)
	ফেরত;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(বাফার, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", বাফার));

    nb_send(বাফার, "netbeans_file_killed");

    যদি (nbbuf != ফাঁকা)
	nbbuf->bufp = ফাঁকা;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    স্থির nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, পূর্ণসংখ্যা *bufnop)
{
    পূর্ণসংখ্যা		bufno;
    nbbuf_T	*nbbuf;

    যদি (!NETBEANS_OPEN || !netbeansFireChanges)
	ফেরত ফাঁকা;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    যদি (bufno <= 0)
	ফেরত ফাঁকা;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    যদি (nbbuf != ফাঁকা && !nbbuf->fireChanges)
	ফেরত ফাঁকা;		// changes in this buffer are not reported

    *bufnop = bufno;
    ফেরত nbbuf;
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
    পূর্ণসংখ্যা		newlen)
{
    char_u	*buf;
    পূর্ণসংখ্যা		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;
    char_u	*p;
    char_u	*newtxt;

    যদি (!NETBEANS_OPEN)
	ফেরত;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    যদি (nbbuf == ফাঁকা)
	ফেরত;

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
    যদি (p != ফাঁকা)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((অক্ষর *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((অক্ষর *)buf, "netbeans_inserted");
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
    পূর্ণসংখ্যা		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    long	off;

    যদি (!NETBEANS_OPEN)
	ফেরত;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    যদি (nbbuf == ফাঁকা)
	ফেরত;

    যদি (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	ফেরত;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.col = col;

    off = pos2off(bufp, &pos);

    sprintf((অক্ষর *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((অক্ষর *)buf, "netbeans_removed");
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
netbeans_button_release(পূর্ণসংখ্যা button)
{
    অক্ষর	buf[128];
    পূর্ণসংখ্যা		bufno;

    যদি (!NETBEANS_OPEN)
	ফেরত;

    bufno = nb_getbufno(curbuf);

    যদি (bufno < 0 || curwin == ফাঁকা || curwin->w_বাফার != curbuf)
	ফেরত;

    পূর্ণসংখ্যা col = mouse_col - curwin->w_wincol
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
    পূর্ণসংখ্যা
netbeans_keycommand(পূর্ণসংখ্যা চাবি)
{
    অক্ষর	চাবি_Name[60];

    netbeans_keyname(চাবি, চাবি_Name);
    ফেরত netbeans_keystring((char_u *)চাবি_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    স্থির পূর্ণসংখ্যা
netbeans_keystring(char_u *চাবি_Name)
{
    অক্ষর	buf[2*MAXPATHL];
    পূর্ণসংখ্যা		bufno = nb_getbufno(curbuf);
    long	off;
    char_u	*q;

    যদি (!NETBEANS_OPEN)
	ফেরত TRUE;

    যদি (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == ফাঁকা ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	যদি (q == ফাঁকা)
	    ফেরত TRUE;
	vim_snprintf(buf, আকার(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	যদি (curbuf->b_ffname != ফাঁকা)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(চাবি_Name);
	ফেরত FALSE;
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
    vim_snprintf(buf, আকার(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, চাবি_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, আকার(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, চাবি_Name,
		off, (long)curwin->w_cursor.lnum, (long)curwin->w_cursor.col);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    ফেরত TRUE;
}


/*
 * Send a save event to netbeans.
 */
    শূন্য
netbeans_save_বাফার(buf_T *bufp)
{
    char_u	buf[64];
    পূর্ণসংখ্যা		bufno;
    nbbuf_T	*nbbuf;

    যদি (!NETBEANS_OPEN)
	ফেরত;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    যদি (nbbuf == ফাঁকা)
	ফেরত;

    nbbuf->modified = 0;

    sprintf((অক্ষর *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((অক্ষর *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    শূন্য
netbeans_deleted_all_সারিগুলি(buf_T *bufp)
{
    char_u	buf[64];
    পূর্ণসংখ্যা		bufno;
    nbbuf_T	*nbbuf;

    যদি (!NETBEANS_OPEN)
	ফেরত;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    যদি (nbbuf == ফাঁকা)
	ফেরত;

    // Don't mark as modified for initial read
    যদি (nbbuf->ঢোকাও_Done)
	nbbuf->modified = 1;

    sprintf((অক্ষর *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    পূর্ণসংখ্যা
netbeans_is_guarded(linenr_T শীর্ষ_2, linenr_T bot)
{
    sign_entry_T	*p;
    পূর্ণসংখ্যা			lnum;

    যদি (!NETBEANS_OPEN)
	ফেরত FALSE;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
	যদি (p->se_id >= GUARDEDOFFSET)
	    জন্য (lnum = শীর্ষ_2 + 1; lnum < bot; lnum++)
		যদি (lnum == p->se_lnum)
		    ফেরত TRUE;

    ফেরত FALSE;
}

#যদি defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    শূন্য
netbeans_draw_multisign_indicator(পূর্ণসংখ্যা row)
{
    পূর্ণসংখ্যা i;
    পূর্ণসংখ্যা y;
    পূর্ণসংখ্যা x;

    যদি (!NETBEANS_OPEN)
	ফেরত;

    x = 0;
    y = row * gui.char_height + 2;

    জন্য (i = 0; i < gui.char_height - 3; i++)
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
netbeans_draw_multisign_indicator(পূর্ণসংখ্যা row)
{
    পূর্ণসংখ্যা i;
    পূর্ণসংখ্যা y;
    পূর্ণসংখ্যা x;
# যদি GTK_CHECK_VERSION(3,0,0)
    cairo_t *cr = ফাঁকা;
# অন্যথা
    GdkDrawable *drawable = gui.drawarea->window;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    যদি (!NETBEANS_OPEN)
	ফেরত;

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

    জন্য (i = 0; i < gui.char_height - 3; i++)
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
	ফেরত;

    FOR_ALL_SIGNS_IN_BUF(curbuf, p)
    {
	যদি (p->se_lnum == lnum && p->se_পরবর্তী && p->se_পরবর্তী->se_lnum == lnum)
	{
	    sign_entry_T *লেজ;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    যদি (p->se_পূর্ব)
		p->se_পূর্ব->se_পরবর্তী = p->se_পরবর্তী;
	    অন্যথা
		curbuf->b_signlist = p->se_পরবর্তী;
	    p->se_পরবর্তী->se_পূর্ব = p->se_পূর্ব;
	    // now find end of sublist and insert p
	    জন্য (লেজ = p->se_পরবর্তী;
		  লেজ->se_পরবর্তী && লেজ->se_পরবর্তী->se_lnum == lnum
				       && লেজ->se_পরবর্তী->se_id < GUARDEDOFFSET;
		  লেজ = লেজ->se_পরবর্তী)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_পরবর্তী = লেজ->se_পরবর্তী;
	    যদি (লেজ->se_পরবর্তী)
		লেজ->se_পরবর্তী->se_পূর্ব = p;
	    p->se_পূর্ব = লেজ;
	    লেজ->se_পরবর্তী = p;
	    update_debug_sign(curbuf, lnum);
	    বিরতি;
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
    পূর্ণসংখ্যা		type_অঙ্ক,
    char_u	*typeName,
    char_u	*tooltip UNUSED,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    পূর্ণসংখ্যা i, j;
    পূর্ণসংখ্যা use_fg = (*fg && STRCMP(fg, "none") != 0);
    পূর্ণসংখ্যা use_bg = (*bg && STRCMP(bg, "none") != 0);

    জন্য (i = 0; i < globalsignmapused; i++)
	যদি (STRCMP(typeName, globalsignmap[i]) == 0)
	    বিরতি;

    যদি (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_অঙ্ক, typeName, tooltip, glyphFile, fg, bg));
	যদি (use_fg || use_bg)
	{
	    অক্ষর fgbuf[2 * (8 + MAX_COLOR_দৈর্ঘ্য) + 1];
	    অক্ষর bgbuf[2 * (8 + MAX_COLOR_দৈর্ঘ্য) + 1];
	    অক্ষর *ptr;
	    পূর্ণসংখ্যা মান;

	    মান = strtol((অক্ষর *)fg, &ptr, 10);
	    যদি (ptr != (অক্ষর *)fg)
		sprintf(fgbuf, "guifg=#%06x", মান & 0xFFFFFF);
	    অন্যথা
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    মান = strtol((অক্ষর *)bg, &ptr, 10);
	    যদি (ptr != (অক্ষর *)bg)
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
		globalsignmap = ALLOC_CLEAR_MULT(অক্ষর *, globalsignmaplen);
	    }
	    অন্যথা    // grow it
	    {
		পূর্ণসংখ্যা incr;
		পূর্ণসংখ্যা oldlen = globalsignmaplen;
		অক্ষর **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * আকার(অক্ষর *));
		যদি (globalsignmap == ফাঁকা)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    ফেরত;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * আকার(অক্ষর *));
	    }
	}

	globalsignmap[i] = (অক্ষর *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    জন্য (j = 0; j < buf->signmapused; j++)
	যদি (buf->signmap[j] == i + 1)
	    ফেরত;

    // add to local map
    যদি (buf->signmapused == buf->signmaplen)
    {
	যদি (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(পূর্ণসংখ্যা, buf->signmaplen);
	}
	অন্যথা    // grow it
	{
	    পূর্ণসংখ্যা incr;
	    পূর্ণসংখ্যা oldlen = buf->signmaplen;
	    পূর্ণসংখ্যা *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * আকার(পূর্ণসংখ্যা));
	    যদি (buf->signmap == ফাঁকা)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		ফেরত;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * আকার(পূর্ণসংখ্যা));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    স্থির পূর্ণসংখ্যা
mapsigntype(nbbuf_T *buf, পূর্ণসংখ্যা localsigntype)
{
    যদি (--localsigntype >= 0 && localsigntype < buf->signmapused)
	ফেরত buf->signmap[localsigntype];

    ফেরত 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    স্থির long
get_buf_আকার(buf_T *bufp)
{
    linenr_T	lnum;
    long	char_গুনতি = 0;
    পূর্ণসংখ্যা		eol_আকার;
    long	last_check = 100000L;

    যদি (bufp->b_ml.ml_flags & ML_EMPTY)
	ফেরত 0;

    যদি (get_fileformat(bufp) == EOL_DOS)
	eol_আকার = 2;
    অন্যথা
	eol_আকার = 1;
    জন্য (lnum = 1; lnum <= bufp->b_ml.ml_সারি_গুনতি; ++lnum)
    {
	char_গুনতি += ml_get_buf_len(bufp, lnum) + eol_আকার;
	// Check for a CTRL-C every 100000 characters
	যদি (char_গুনতি > last_check)
	{
	    ui_breakcheck();
	    যদি (got_int)
		ফেরত char_গুনতি;
	    last_check = char_গুনতি + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    যদি (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_গুনতি -= eol_আকার;

    ফেরত char_গুনতি;
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
	    ফেরত ফাঁকা;
	pos.lnum = lnum;
	pos.col = offset;
    }

    ফেরত &pos;
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

    off = strtol((অক্ষর *)*argp, (অক্ষর **)argp, 10);
    যদি (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.col = strtol((অক্ষর *)*argp, (অক্ষর **)argp, 10);
	mypos.coladd = 0;
	ফেরত &mypos;
    }
    ফেরত off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    স্থির long
pos2off(buf_T *buf, pos_T *pos)
{
    long	 offset = 0;

    যদি (buf->b_ml.ml_flags & ML_EMPTY)
	ফেরত 0;

    যদি ((offset = ml_find_সারি_or_offset(buf, pos->lnum, 0)) < 0)
	ফেরত 0;
    offset += pos->col;

    ফেরত offset;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    স্থির শূন্য
print_read_বার্তা(nbbuf_T *buf)
{
    পূর্ণসংখ্যা	    lnum = buf->bufp->b_ml.ml_সারি_গুনতি;
    off_T   nchars = buf->bufp->b_orig_আকার;
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
    বার্তা_trunc_attr((অক্ষর *)IObuff, FALSE, 0);
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
						buf->bufp->b_orig_আকার);

	VIM_CLEAR(keep_বার্তা);
	বার্তা_scrolled_ign = TRUE;
	p = (char_u *)বার্তা_trunc_attr((অক্ষর *)IObuff, FALSE, 0);
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
	অক্ষর msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_add_bang_to_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#endif // defined(FEAT_NETBEANS_INTG)
