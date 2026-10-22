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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_NETBEANS_INTG)

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUARDED		10000 // typenr for "guarded" annotation
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_COLOR_ਲੰਮਾਈ 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
ਸਥਿਰ ਅੱਖਰ *Ext_Ed_Protocol_ਸੰਸਕਰਣ = "2.5";

ਸਥਿਰ ਲੰਮਾ pos2off(buf_T *, pos_T *);
ਸਥਿਰ pos_T *off2pos(buf_T *, ਲੰਮਾ);
ਸਥਿਰ pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
ਸਥਿਰ ਲੰਮਾ get_buf_ਮਾਪ(buf_T *);
ਸਥਿਰ ਪੂਰਨ netbeans_keystring(char_u *keystr);
ਸਥਿਰ ਖਾਲੀ special_ਕੁੰਜੀਆਂ(char_u *ਦਲੀਲਾਂ);

ਸਥਿਰ ਪੂਰਨ getConnInfo(ਅੱਖਰ *file, ਅੱਖਰ **host, ਅੱਖਰ **ਪੋਰਟ, ਅੱਖਰ **password);

ਸਥਿਰ ਖਾਲੀ nb_init_graphics(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ coloncmd(ਅੱਖਰ *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ਸਥਿਰ ਖਾਲੀ nb_ਸੈੱਟ_curbuf(buf_T *buf);
ਸਥਿਰ ਖਾਲੀ nb_parse_cmd(char_u *);
ਸਥਿਰ ਪੂਰਨ  nb_do_cmd(ਪੂਰਨ, char_u *, ਪੂਰਨ, ਪੂਰਨ, char_u *);
ਸਥਿਰ ਖਾਲੀ nb_send(ਅੱਖਰ *buf, ਅੱਖਰ *fun);
ਸਥਿਰ ਖਾਲੀ nb_free(ਖਾਲੀ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NETBEANS_OPEN (channel_can_write_ਮੰਜ਼ਿਲ(nb_channel))
ਸਥਿਰ channel_T *nb_channel = ਨਿਰਾ;

ਸਥਿਰ ਪੂਰਨ r_cmdno;			// current command number for reply
ਸਥਿਰ ਪੂਰਨ dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NBDEBUG
# include "nbdebug.c"
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ needupdate = 0;
ਸਥਿਰ ਪੂਰਨ ਵਿੱਚ_Atomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    ਸਥਿਰ ਪੂਰਨ
nb_is_ਸੁਰੱਖਿਅਤ_string(char_u *str, char_u *allowed)
{
    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਲਈ (char_u *p = str; *p != NUL; p++)
	ਜੇ (vim_strchr(allowed, *p) == ਨਿਰਾ)
	    ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    ਸਥਿਰ ਖਾਲੀ
nb_channel_closed(ਖਾਲੀ)
{
    nb_channel = ਨਿਰਾ;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    ਸਥਿਰ ਖਾਲੀ
netbeans_close(ਖਾਲੀ)
{
    ਜੇ (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	ਜੇ (nb_channel != ਨਿਰਾ)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = ਨਿਰਾ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
    bevalServers &= ~BEVAL_NETBEANS;
#ਜੇ_ਅੰਤ

    needupdate = 0;
    ਵਿੱਚ_Atomic = 0;
    nb_free();

    // remove all signs and update the screen after gutter removal
    coloncmd(":sign unplace *");
    ਬਦਲਿਆ_ਖਿੜਕੀ_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NB_DEF_HOST "localhost"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NB_DEF_ADDR "3219"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NB_DEF_PASS "changeme"

    ਸਥਿਰ ਪੂਰਨ
netbeans_connect(ਅੱਖਰ *params, ਪੂਰਨ doabort)
{
    ਪੂਰਨ		ਪੋਰਟ;
    ਅੱਖਰ	buf[32];
    ਅੱਖਰ	*hostname = ਨਿਰਾ;
    ਅੱਖਰ	*ਪਤਾ = ਨਿਰਾ;
    ਅੱਖਰ	*password = ਨਿਰਾ;
    ਅੱਖਰ	*fname;
    ਅੱਖਰ	*arg = ਨਿਰਾ;

    ਜੇ (*params == '=')
    {
	// "=fname": Read info from specified file.
	ਜੇ (getConnInfo(params + 1, &hostname, &ਪਤਾ, &password) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	ਜੇ (arg == ਨਿਰਾ && (fname = getenv("__NETBEANS_CONINFO")) != ਨਿਰਾ)
	{
	    // "": get info from file specified in environment
	    ਜੇ (getConnInfo(fname, &hostname, &ਪਤਾ, &password) == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (arg != ਨਿਰਾ)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		ਪਤਾ = strchr(hostname, ':');
		ਜੇ (ਪਤਾ != ਨਿਰਾ)
		{
		    *ਪਤਾ++ = '\0';
		    password = strchr(ਪਤਾ, ':');
		    ਜੇ (password != ਨਿਰਾ)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    ਜੇ (hostname == ਨਿਰਾ || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    ਜੇ (ਪਤਾ == ਨਿਰਾ)
		ਪਤਾ = getenv("__NETBEANS_SOCKET");
	    ਜੇ (password == ਨਿਰਾ)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    ਜੇ (hostname != ਨਿਰਾ)
		hostname = (ਅੱਖਰ *)vim_strsave((char_u *)hostname);
	    ਜੇ (ਪਤਾ != ਨਿਰਾ)
		ਪਤਾ = (ਅੱਖਰ *)vim_strsave((char_u *)ਪਤਾ);
	    ਜੇ (password != ਨਿਰਾ)
		password = (ਅੱਖਰ *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    ਜੇ (hostname == ਨਿਰਾ || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (ਅੱਖਰ *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    ਜੇ (ਪਤਾ == ਨਿਰਾ || *ਪਤਾ == '\0')
    {
	vim_free(ਪਤਾ);
	ਪਤਾ = (ਅੱਖਰ *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    ਜੇ (password == ਨਿਰਾ || *password == '\0')
    {
	vim_free(password);
	password = (ਅੱਖਰ *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    ਜੇ (hostname != ਨਿਰਾ && ਪਤਾ != ਨਿਰਾ && password != ਨਿਰਾ)
    {
	ਪੋਰਟ = atoi(ਪਤਾ);
	nb_channel = channel_open(hostname, ਪੋਰਟ, 3000, nb_channel_closed);
	ਜੇ (nb_channel != ਨਿਰਾ)
	{
	    // success
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#ਜੇ_ਅੰਤ

	    // success, login
	    vim_snprintf(buf, ਆਕਾਰ(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", Ext_Ed_Protocol_ਸੰਸਕਰਣ);
	    nb_send(buf, "externaleditor_version");
	}
    }

    ਜੇ (nb_channel == ਨਿਰਾ && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(ਪਤਾ);
    vim_free(password);
    ਵਾਪਸ NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    ਸਥਿਰ ਪੂਰਨ
getConnInfo(ਅੱਖਰ *file, ਅੱਖਰ **host, ਅੱਖਰ **ਪੋਰਟ, ਅੱਖਰ **auth)
{
    FILE *fp;
    char_u buf[BUFSIZ];
    char_u *lp;
    char_u *nlp;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    stat_T	st;

    /*
     * For Unix only accept the file when it's not accessible by others.
     * The open will then fail if we don't own the file.
     */
    ਜੇ (mch_stat(file, &st) == 0 && (st.st_ਬਹੁਲਕ & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_ਬਹੁਲਕ_for_netbeans_connection_info_file_str),
									file);
	ਵਾਪਸ FAIL;
    }
#ਜੇ_ਅੰਤ

    fp = mch_fopen(file, "r");
    ਜੇ (fp == ਨਿਰਾ)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	ਵਾਪਸ FAIL;
    }

    // Read the file. There should be one of each parameter
    ਜਦੋਂ ((lp = (char_u *)fgets((ਅੱਖਰ *)buf, BUFSIZ, fp)) != ਨਿਰਾ)
    {
	ਜੇ ((nlp = vim_strchr(lp, '\n')) != ਨਿਰਾ)
	    *nlp = 0;	    // strip off the trailing newline

	ਜੇ (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (ਅੱਖਰ *)vim_strsave(&buf[5]);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*ਪੋਰਟ);
	    *ਪੋਰਟ = (ਅੱਖਰ *)vim_strsave(&buf[5]);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (ਅੱਖਰ *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    ਵਾਪਸ OK;
}


ਬਣਤਰ keyqueue
{
    char_u	    *keystr;
    ਬਣਤਰ keyqueue *ਅਗਲਾ;
    ਬਣਤਰ keyqueue *prev;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ keyqueue ਕੁੰਜੀ_Q_T;

ਸਥਿਰ ਕੁੰਜੀ_Q_T ਕੁੰਜੀ_ਸਿਰ; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    ਸਥਿਰ ਖਾਲੀ
postpone_keycommand(char_u *keystr)
{
    ਕੁੰਜੀ_Q_T *ਗੰਢ;

    ਗੰਢ = ALLOC_ONE(ਕੁੰਜੀ_Q_T);
    ਜੇ (ਗੰਢ == ਨਿਰਾ)
	ਵਾਪਸ;  // out of memory, drop the key

    ਜੇ (ਕੁੰਜੀ_ਸਿਰ.ਅਗਲਾ == ਨਿਰਾ) // initialize circular queue
    {
	ਕੁੰਜੀ_ਸਿਰ.ਅਗਲਾ = &ਕੁੰਜੀ_ਸਿਰ;
	ਕੁੰਜੀ_ਸਿਰ.prev = &ਕੁੰਜੀ_ਸਿਰ;
    }

    // insert node at tail of queue
    ਗੰਢ->ਅਗਲਾ = &ਕੁੰਜੀ_ਸਿਰ;
    ਗੰਢ->prev = ਕੁੰਜੀ_ਸਿਰ.prev;
    ਕੁੰਜੀ_ਸਿਰ.prev->ਅਗਲਾ = ਗੰਢ;
    ਕੁੰਜੀ_ਸਿਰ.prev = ਗੰਢ;

    ਗੰਢ->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_ਕੁੰਜੀ_ਕਤਾਰ(ਖਾਲੀ)
{
    ਪੂਰਨ postponed = FALSE;

    ਜਦੋਂ (!postponed && ਕੁੰਜੀ_ਸਿਰ.ਅਗਲਾ && ਕੁੰਜੀ_ਸਿਰ.ਅਗਲਾ != &ਕੁੰਜੀ_ਸਿਰ)
    {
	// first, unlink the node
	ਕੁੰਜੀ_Q_T *ਗੰਢ = ਕੁੰਜੀ_ਸਿਰ.ਅਗਲਾ;
	ਕੁੰਜੀ_ਸਿਰ.ਅਗਲਾ = ਗੰਢ->ਅਗਲਾ;
	ਗੰਢ->ਅਗਲਾ->prev = ਗੰਢ->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	ਜੇ (ਗੰਢ->keystr != ਨਿਰਾ)
	    postponed = !netbeans_keystring(ਗੰਢ->keystr);
	vim_free(ਗੰਢ->keystr);

	// Finally, dispose of the node
	vim_free(ਗੰਢ);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    ਖਾਲੀ
netbeans_parse_messages(ਖਾਲੀ)
{
    readq_T	*ਗੰਢ;
    char_u	*ਬਫਰ;
    char_u	*p;
    ਪੂਰਨ		own_ਗੰਢ;

    ਜਦੋਂ (nb_channel != ਨਿਰਾ)
    {
	ਗੰਢ = channel_peek(nb_channel, PART_SOCK);
	ਜੇ (ਗੰਢ == ਨਿਰਾ)
	    ਰੋਕੋ;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_ਪਹਿਲਾ_nl(ਗੰਢ);
	ਜੇ (p == ਨਿਰਾ)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    ਜੇ (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		ਵਾਪਸ;
	    ਜਾਰੀ;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	ਜੇ (*p == NUL)
	{
	    own_ਗੰਢ = TRUE;
	    ਬਫਰ = channel_get(nb_channel, PART_SOCK, ਨਿਰਾ);
	    // "node" is now invalid!
	}
	ਨਹੀਂਤਾਂ
	{
	    own_ਗੰਢ = FALSE;
	    ਬਫਰ = ਗੰਢ->rq_ਬਫਰ;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(ਬਫਰ);

	ਜੇ (own_ਗੰਢ)
	    // buffer finished, dispose of it
	    vim_free(ਬਫਰ);
	ਨਹੀਂਤਾਂ ਜੇ (nb_channel != ਨਿਰਾ)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (ਪੂਰਨ)(p - ਬਫਰ));
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
    ਸਥਿਰ ਖਾਲੀ
nb_parse_cmd(char_u *cmd)
{
    ਅੱਖਰ	*verb;
    ਅੱਖਰ	*q;
    ਪੂਰਨ		bufno;
    ਪੂਰਨ		isfunc = -1;

    ਜੇ (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    ਜੇ (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_ਕਾਲਮ = false;

	// The IDE is breaking the connection.
	netbeans_close();
	ਵਾਪਸ;
    }

    bufno = strtol((ਅੱਖਰ *)cmd, &verb, 10);

    ਜੇ (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	ਵਾਪਸ;
    }
    ++verb; // skip colon

    ਲਈ (q = verb; *q; q++)
    {
	ਜੇ (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    ਰੋਕੋ;
	}
    }

    ਜੇ (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_ਵਿੱਚ_str), cmd);
	ਵਾਪਸ;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (ਅੱਖਰ *)skipwhite((char_u *)q);

    ਜੇ (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NBDEBUG
	/*
	 * This happens because the ExtEd can send a command or 2 after
	 * doing a stopDocumentListen command. It doesn't harm anything
	 * so I'm disabling it except for debugging.
	 */
	nbdebug(("nb_parse_cmd: Command error for \"%s\"\n", cmd));
	emsg(e_bad_return_from_nb_do_cmd);
#ਜੇ_ਅੰਤ
    }
}

ਬਣਤਰ nbbuf_struct
{
    buf_T		*bufp;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	 fireChanges:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	 init_ਪੂਰਾ:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	 ਪਾਓ_ਪੂਰਾ:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	 modified:1;
    ਪੂਰਨ			 nbbuf_ਸੰਖਿਆ;
    ਅੱਖਰ		*displayname;
    ਪੂਰਨ			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ nbbuf_struct nbbuf_T;

ਸਥਿਰ nbbuf_T *buf_list = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ buf_list_ਮਾਪ = 0;	// size of buf_list
ਸਥਿਰ ਪੂਰਨ buf_list_ਵਰਤਿਆ = 0;	// nr of entries in buf_list actually in use

ਸਥਿਰ ਅੱਖਰ **globalsignmap = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ globalsignmaplen = 0;
ਸਥਿਰ ਪੂਰਨ globalsignmapused = 0;

ਸਥਿਰ ਪੂਰਨ  mapsigntype(nbbuf_T *, ਪੂਰਨ localsigntype);
ਸਥਿਰ ਖਾਲੀ addsigntype(nbbuf_T *, ਪੂਰਨ localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
ਸਥਿਰ ਖਾਲੀ print_read_ਸੁਨੇਹਾ(nbbuf_T *buf);
ਸਥਿਰ ਖਾਲੀ print_save_ਸੁਨੇਹਾ(nbbuf_T *buf, off_T nchars);

ਸਥਿਰ ਪੂਰਨ curPCtype = -1;

/*
 * Free netbeans resources.
 */
    ਸਥਿਰ ਖਾਲੀ
nb_free(ਖਾਲੀ)
{
    ਕੁੰਜੀ_Q_T *ਕੁੰਜੀ_ਗੰਢ = ਕੁੰਜੀ_ਸਿਰ.ਅਗਲਾ;
    nbbuf_T buf;
    ਪੂਰਨ i;

    // free the netbeans buffer list
    ਲਈ (i = 0; i < buf_list_ਵਰਤਿਆ; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	ਜੇ (buf.bufp != ਨਿਰਾ && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_ਮਾਪ = 0;
    buf_list_ਵਰਤਿਆ = 0;

    // free the queued key commands
    ਜਦੋਂ (ਕੁੰਜੀ_ਗੰਢ != ਨਿਰਾ && ਕੁੰਜੀ_ਗੰਢ != &ਕੁੰਜੀ_ਸਿਰ)
    {
	ਕੁੰਜੀ_Q_T *ਅਗਲਾ = ਕੁੰਜੀ_ਗੰਢ->ਅਗਲਾ;
	vim_free(ਕੁੰਜੀ_ਗੰਢ->keystr);
	vim_free(ਕੁੰਜੀ_ਗੰਢ);
	ਜੇ (ਅਗਲਾ == &ਕੁੰਜੀ_ਸਿਰ)
	{
	    ਕੁੰਜੀ_ਸਿਰ.ਅਗਲਾ = &ਕੁੰਜੀ_ਸਿਰ;
	    ਕੁੰਜੀ_ਸਿਰ.prev = &ਕੁੰਜੀ_ਸਿਰ;
	    ਰੋਕੋ;
	}
	ਕੁੰਜੀ_ਗੰਢ = ਅਗਲਾ;
    }

    // free the queued netbeans commands
    ਜੇ (nb_channel != ਨਿਰਾ)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    ਸਥਿਰ ਪੂਰਨ
nb_getbufno(buf_T *bufp)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < buf_list_ਵਰਤਿਆ; i++)
	ਜੇ (buf_list[i].bufp == bufp)
	    ਵਾਪਸ i;
    ਵਾਪਸ -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    ਪੂਰਨ
is_Netbeans_ਬਫਰ(buf_T *bufp)
{
    ਵਾਪਸ NETBEANS_OPEN && bufp->b_netbeans_file;
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
    ਪੂਰਨ
isNetbeansModified(buf_T *bufp)
{
    ਜੇ (is_Netbeans_ਬਫਰ(bufp))
    {
	ਪੂਰਨ bufno = nb_getbufno(bufp);

	ਜੇ (bufno > 0)
	    ਵਾਪਸ buf_list[bufno].modified;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    ਸਥਿਰ nbbuf_T *
nb_get_buf(ਪੂਰਨ bufno)
{
    // find or create a buffer with the given number
    ਪੂਰਨ incr;

    ਜੇ (bufno <= 0)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * ਆਕਾਰ(nbbuf_T));
	buf_list_ਮਾਪ = 100;
    }
    ਜੇ (bufno >= buf_list_ਵਰਤਿਆ) // new
    {
	ਜੇ (bufno >= buf_list_ਮਾਪ) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_ਮਾਪ + 90;
	    buf_list_ਮਾਪ += incr;
	    bufsize = buf_list_ਮਾਪ * ਆਕਾਰ(nbbuf_T);
	    ਜੇ (bufsize == 0 || bufsize / ਆਕਾਰ(nbbuf_T)
						      != (size_t)buf_list_ਮਾਪ)
	    {
		// list size overflow, bail out
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    ਜੇ (buf_list == ਨਿਰਾ)
	    {
		vim_free(t_buf_list);
		buf_list_ਮਾਪ = 0;
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    vim_memset(buf_list + buf_list_ਮਾਪ - incr, 0,
						      incr * ਆਕਾਰ(nbbuf_T));
	}

	ਜਦੋਂ (buf_list_ਵਰਤਿਆ <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_ਵਰਤਿਆ].fireChanges = 1;
	    ++buf_list_ਵਰਤਿਆ;
	}
    }

    ਵਾਪਸ buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    ਸਥਿਰ ਪੂਰਨ
ਗਿਣਤੀ_ਬਦਲਿਆ_buffers(ਖਾਲੀ)
{
    buf_T	*bufp;
    ਪੂਰਨ		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	ਜੇ (bufp->b_ਬਦਲਿਆ)
	    ++n;
    ਵਾਪਸ n;
}

/*
 * End the netbeans session.
 */
    ਖਾਲੀ
netbeans_ਸਿਰਾ(ਖਾਲੀ)
{
    ਪੂਰਨ i;
    ਸਥਿਰ ਅੱਖਰ buf[128];

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    ਲਈ (i = 0; i < buf_list_ਵਰਤਿਆ; i++)
    {
	ਜੇ (!buf_list[i].bufp)
	    ਜਾਰੀ;
	ਜੇ (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, ਨਿਰਾ);
	buf_list[i].bufp = ਨਿਰਾ;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    ਸਥਿਰ ਖਾਲੀ
nb_send(ਅੱਖਰ *buf, ਅੱਖਰ *fun)
{
    ਜੇ (nb_channel != ਨਿਰਾ)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (ਪੂਰਨ)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    ਸਥਿਰ ਖਾਲੀ
nb_ਜਵਾਬ_nil(ਪੂਰਨ cmdno)
{
    ਅੱਖਰ ਜਵਾਬ[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    sprintf(ਜਵਾਬ, "%d\n", cmdno);
    nb_send(ਜਵਾਬ, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    ਸਥਿਰ ਖਾਲੀ
nb_ਜਵਾਬ_ਪਾਠ(ਪੂਰਨ cmdno, char_u *ਨਤੀਜਾ)
{
    char_u *ਜਵਾਬ;

    nbdebug(("REP %d: %s\n", cmdno, (ਅੱਖਰ *)ਨਤੀਜਾ));

    ਜਵਾਬ = alloc(STRLEN(ਨਤੀਜਾ) + 32);
    sprintf((ਅੱਖਰ *)ਜਵਾਬ, "%d %s\n", cmdno, (ਅੱਖਰ *)ਨਤੀਜਾ);
    nb_send((ਅੱਖਰ *)ਜਵਾਬ, "nb_reply_text");

    vim_free(ਜਵਾਬ);
}


/*
 * Send a response with a number result code.
 */
    ਸਥਿਰ ਖਾਲੀ
nb_ਜਵਾਬ_nr(ਪੂਰਨ cmdno, ਲੰਮਾ ਨਤੀਜਾ)
{
    ਅੱਖਰ ਜਵਾਬ[32];

    nbdebug(("REP %d: %ld\n", cmdno, ਨਤੀਜਾ));

    sprintf(ਜਵਾਬ, "%d %ld\n", cmdno, ਨਤੀਜਾ);
    nb_send(ਜਵਾਬ, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    ਸਥਿਰ char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਲਈ (; *p; p++)
    {
	ਚੋਣ (*p)
	{
	    ਹਾਲਤ '\"':
	    ਹਾਲਤ '\\':
		*q++ = '\\'; *q++ = *p; ਰੋਕੋ;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    ਹਾਲਤ '\n':
		*q++ = '\\'; *q++ = 'n'; ਰੋਕੋ;
	    ਹਾਲਤ '\r':
		*q++ = '\\'; *q++ = 'r'; ਰੋਕੋ;
	    ਮੂਲ:
		*q++ = *p;
		ਰੋਕੋ;
	}
    }
    *q = '\0';

    ਵਾਪਸ buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    ਸਥਿਰ ਅੱਖਰ *
nb_unquote(char_u *p, char_u **endp)
{
    ਅੱਖਰ *ਨਤੀਜਾ = 0;
    ਅੱਖਰ *q;
    ਪੂਰਨ ਪੂਰਾ = 0;

    // result is never longer than input
    ਨਤੀਜਾ = alloc_clear(STRLEN(p) + 1);
    ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	ਨਤੀਜਾ[0] = NUL;
	ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਲਈ (q = ਨਤੀਜਾ; !ਪੂਰਾ && *p != NUL;)
    {
	ਚੋਣ (*p)
	{
	    ਹਾਲਤ '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		ਪੂਰਾ = 1;
		ਰੋਕੋ;

	    ਹਾਲਤ '\\':
		++p;
		ਚੋਣ (*p)
		{
		    ਹਾਲਤ '\\':	*q++ = '\\';	ਰੋਕੋ;
		    ਹਾਲਤ 'n':	*q++ = '\n';	ਰੋਕੋ;
		    ਹਾਲਤ 't':	*q++ = '\t';	ਰੋਕੋ;
		    ਹਾਲਤ 'r':	*q++ = '\r';	ਰੋਕੋ;
		    ਹਾਲਤ '"':	*q++ = '"';	ਰੋਕੋ;
		    ਹਾਲਤ NUL:	--p;		ਰੋਕੋ;
		    // default: skip over illegal chars
		}
		++p;
		ਰੋਕੋ;

	    ਮੂਲ:
		*q++ = *p++;
	}
    }

    ਜੇ (endp != ਨਿਰਾ)
	*endp = p;

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    ਸਥਿਰ ਖਾਲੀ
nb_partialremove(linenr_T lnum, colnr_T ਪਹਿਲਾ, colnr_T ਆਖਰੀ)
{
    char_u *oldtext, *newtext;
    ਪੂਰਨ oldlen;
    ਪੂਰਨ lastbyte = ਆਖਰੀ;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    ਜੇ (ਪਹਿਲਾ >= (colnr_T)oldlen || oldlen == 0)  // just in case
	ਵਾਪਸ;
    ਜੇ (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - ਪਹਿਲਾ));
    ਜੇ (newtext == ਨਿਰਾ)
	ਵਾਪਸ;

    mch_memmove(newtext, oldtext, ਪਹਿਲਾ);
    STRMOVE(newtext + ਪਹਿਲਾ, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    ਸਥਿਰ ਖਾਲੀ
nb_joinlines(linenr_T ਪਹਿਲਾ, linenr_T other)
{
    ਪੂਰਨ len_ਪਹਿਲਾ, len_other;
    char_u *p;

    len_ਪਹਿਲਾ = ml_get_len(ਪਹਿਲਾ);
    len_other = ml_get_len(other);
    p = alloc(len_ਪਹਿਲਾ + len_other + 1);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    mch_memmove(p, ml_get(ਪਹਿਲਾ), len_ਪਹਿਲਾ);
    mch_memmove(p + len_ਪਹਿਲਾ, ml_get(other), len_other + 1);
    ml_replace(ਪਹਿਲਾ, p, FALSE);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SKIP_STOP 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ streq(a,b) (strcmp(a,b) == 0)

/*
 * Do the actual processing of a single netbeans command or function.
 * The difference between a command and function is that a function
 * gets a response (it's required) but a command does not.
 * For arguments see comment for nb_parse_cmd().
 */
    ਸਥਿਰ ਪੂਰਨ
nb_do_cmd(
    ਪੂਰਨ		bufno,
    char_u	*cmd,
    ਪੂਰਨ		func,
    ਪੂਰਨ		cmdno,
    char_u	*ਦਲੀਲਾਂ)	    // points to space before arguments or NUL
{
    ਪੂਰਨ		do_update = 0;
    ਲੰਮਾ	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    ਸਥਿਰ ਪੂਰਨ	skip = 0;
    ਪੂਰਨ		retval = OK;
    ਅੱਖਰ	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (ਅੱਖਰ *)ਦਲੀਲਾਂ));

    ਜੇ (func)
    {
// =====================================================================
	ਜੇ (streq((ਅੱਖਰ *)cmd, "getModified"))
	{
	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
		// Return the number of buffers that are modified.
		nb_ਜਵਾਬ_nr(cmdno, (ਲੰਮਾ)ਗਿਣਤੀ_ਬਦਲਿਆ_buffers());
	    ਨਹੀਂਤਾਂ
		// Return whether the buffer is modified.
		nb_ਜਵਾਬ_nr(cmdno, (ਲੰਮਾ)(buf->bufp->b_ਬਦਲਿਆ
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_ਜਵਾਬ_nr(cmdno, (ਲੰਮਾ)ਗਿਣਤੀ_ਬਦਲਿਆ_buffers());
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "getCursor"))
	{
	    char_u ਪਾਠ[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((ਅੱਖਰ *)ਪਾਠ, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (ਲੰਮਾ)curwin->w_cursor.lnum,
		    (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_ਜਵਾਬ_ਪਾਠ(cmdno, ਪਾਠ);
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "getAnno"))
	{
	    ਲੰਮਾ linenum = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_getanno));
		retval = FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ ser_ਅੰਕ;

		cp = (ਅੱਖਰ *)ਦਲੀਲਾਂ;
		ser_ਅੰਕ = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (ਲੰਮਾ)buf_findsign(buf->bufp, ser_ਅੰਕ, ਨਿਰਾ);
	    }
#ਜੇ_ਅੰਤ
	    nb_ਜਵਾਬ_nr(cmdno, linenum);
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "getLength"))
	{
	    ਲੰਮਾ len = 0;

	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_getlength));
		retval = FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		len = get_buf_ਮਾਪ(buf->bufp);
	    }
	    nb_ਜਵਾਬ_nr(cmdno, len);
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "getText"))
	{
	    ਲੰਮਾ	len;
	    linenr_T	nlines;
	    char_u	*ਪਾਠ = ਨਿਰਾ;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*ਸਤਰ;

	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_gettext));
		retval = FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		len = get_buf_ਮਾਪ(buf->bufp);
		nlines = buf->bufp->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		ਪਾਠ = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		ਜੇ (ਪਾਠ == ਨਿਰਾ)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		ਨਹੀਂਤਾਂ
		{
		    p = ਪਾਠ;
		    *p++ = '\"';
		    ਲਈ (; lno <= nlines ; lno++)
		    {
			ਸਤਰ = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			ਜੇ (ਸਤਰ != ਨਿਰਾ)
			{
			    STRCPY(p, ਸਤਰ);
			    p += STRLEN(ਸਤਰ);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(ਸਤਰ);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    ਜੇ (ਪਾਠ == ਨਿਰਾ)
		nb_ਜਵਾਬ_ਪਾਠ(cmdno, (char_u *)"");
	    ਨਹੀਂਤਾਂ
	    {
		nb_ਜਵਾਬ_ਪਾਠ(cmdno, ਪਾਠ);
		vim_free(ਪਾਠ);
	    }
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "remove"))
	{
	    ਲੰਮਾ ਗਿਣਤੀ_2;
	    pos_T ਪਹਿਲਾ, ਆਖਰੀ;
	    pos_T *pos;
	    pos_T *ਅਗਲਾ;
	    linenr_T del_from_lnum, del_ਮੰਜ਼ਿਲ_lnum;  // lines to be deleted as a whole
	    ਪੂਰਨ oldFire = netbeansFireChanges;
	    ਪੂਰਨ oldSuppress = netbeans_Suppress_No_ਸਤਰਾਂ;
	    ਪੂਰਨ was_ਬਦਲਿਆ_2;

	    ਜੇ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (ਅੱਖਰ *) cmd));
		nb_ਜਵਾਬ_nil(cmdno);
		ਵਾਪਸ OK;
	    }

	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_remove));
		retval = FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_ਸਤਰਾਂ = TRUE;

		nb_ਸੈੱਟ_curbuf(buf->bufp);
		was_ਬਦਲਿਆ_2 = buf->bufp->b_ਬਦਲਿਆ;
		cp = (ਅੱਖਰ *)ਦਲੀਲਾਂ;
		off = strtol(cp, &cp, 10);
		ਗਿਣਤੀ_2 = strtol(cp, &cp, 10);
		ਦਲੀਲਾਂ = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		ਜੇ (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_ਜਵਾਬ_ਪਾਠ(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_ਸਤਰਾਂ = oldSuppress;
		    ਵਾਪਸ FAIL;
		}
		ਪਹਿਲਾ = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      ਪਹਿਲਾ.lnum, ਪਹਿਲਾ.ਖਾਨਾ));
		pos = off2pos(buf->bufp, off+ਗਿਣਤੀ_2-1);
		ਜੇ (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_ਜਵਾਬ_ਪਾਠ(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_ਸਤਰਾਂ = oldSuppress;
		    ਵਾਪਸ FAIL;
		}
		ਆਖਰੀ = *pos;
		nbdebug(("    LAST POS: line %ld, col %d\n",
							ਆਖਰੀ.lnum, ਆਖਰੀ.ਖਾਨਾ));
		del_from_lnum = ਪਹਿਲਾ.lnum;
		del_ਮੰਜ਼ਿਲ_lnum = ਆਖਰੀ.lnum;
		do_update = 1;

		// Get the position of the first byte after the deleted
		// section.  "next" is NULL when deleting to the end of the
		// file.
		ਅਗਲਾ = off2pos(buf->bufp, off + ਗਿਣਤੀ_2);

		// Remove part of the first line.
		ਜੇ (ਪਹਿਲਾ.ਖਾਨਾ != 0
				|| (ਅਗਲਾ != ਨਿਰਾ && ਪਹਿਲਾ.lnum == ਅਗਲਾ->lnum))
		{
		    ਜੇ (ਪਹਿਲਾ.lnum != ਆਖਰੀ.lnum
			    || (ਅਗਲਾ != ਨਿਰਾ && ਪਹਿਲਾ.lnum != ਅਗਲਾ->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(ਪਹਿਲਾ.lnum, ਪਹਿਲਾ.ਖਾਨਾ,
							     (colnr_T)MAXCOL);
			ਜੇ (ਪਹਿਲਾ.lnum == ਆਖਰੀ.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(ਪਹਿਲਾ.lnum, ਅਗਲਾ->lnum);
			    del_ਮੰਜ਼ਿਲ_lnum = ਅਗਲਾ->lnum;
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// remove within one line
			nb_partialremove(ਪਹਿਲਾ.lnum, ਪਹਿਲਾ.ਖਾਨਾ, ਆਖਰੀ.ਖਾਨਾ);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		ਜੇ (ਪਹਿਲਾ.lnum != ਆਖਰੀ.lnum && ਅਗਲਾ != ਨਿਰਾ
			&& ਅਗਲਾ->ਖਾਨਾ != 0 && ਆਖਰੀ.lnum == ਅਗਲਾ->lnum)
		{
		    nb_partialremove(ਆਖਰੀ.lnum, 0, ਆਖਰੀ.ਖਾਨਾ);
		    ਜੇ (del_from_lnum > ਪਹਿਲਾ.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(ਪਹਿਲਾ.lnum, ਆਖਰੀ.lnum);
		    }
		    ਨਹੀਂਤਾਂ
			// First line is deleted as a whole, keep the last
			// line.
			--del_ਮੰਜ਼ਿਲ_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		ਜੇ (ਪਹਿਲਾ.lnum != ਆਖਰੀ.lnum && del_from_lnum > ਪਹਿਲਾ.lnum
			&& ਅਗਲਾ != ਨਿਰਾ && ਆਖਰੀ.lnum != ਅਗਲਾ->lnum)
		{
		    nb_joinlines(ਪਹਿਲਾ.lnum, ਅਗਲਾ->lnum);
		    del_ਮੰਜ਼ਿਲ_lnum = ਅਗਲਾ->lnum;
		}

		// Delete whole lines if there are any.
		ਜੇ (del_ਮੰਜ਼ਿਲ_lnum >= del_from_lnum)
		{
		    ਪੂਰਨ i;

		    // delete signs from the lines being deleted
		    ਲਈ (i = del_from_lnum; i <= del_ਮੰਜ਼ਿਲ_lnum; i++)
		    {
			ਪੂਰਨ id = buf_findsign_id(buf->bufp, (linenr_T)i, ਨਿਰਾ);
			ਜੇ (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, ਨਿਰਾ);
			}
			ਨਹੀਂਤਾਂ
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_ਮੰਜ਼ਿਲ_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.ਖਾਨਾ = 0;
		    del_ਸਤਰਾਂ(del_ਮੰਜ਼ਿਲ_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = ਪਹਿਲਾ;
		check_cursor_lnum();
		buf->bufp->b_ਬਦਲਿਆ = was_ਬਦਲਿਆ_2; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_ਸਤਰਾਂ = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_ਜਵਾਬ_nil(cmdno);
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "insert"))
	{
	    char_u	*ਮੰਜ਼ਿਲ_free;

	    ਜੇ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (ਅੱਖਰ *) cmd));
		nb_ਜਵਾਬ_nil(cmdno);
		ਵਾਪਸ OK;
	    }

	    // get offset
	    cp = (ਅੱਖਰ *)ਦਲੀਲਾਂ;
	    off = strtol(cp, &cp, 10);
	    ਦਲੀਲਾਂ = (char_u *)cp;

	    // get text to be inserted
	    ਦਲੀਲਾਂ = skipwhite(ਦਲੀਲਾਂ);
	    ਦਲੀਲਾਂ = ਮੰਜ਼ਿਲ_free = (char_u *)nb_unquote(ਦਲੀਲਾਂ, ਨਿਰਾ);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_ਪਾਓ));
		retval = FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਦਲੀਲਾਂ != ਨਿਰਾ)
	    {
		ਪੂਰਨ	ff_detected = EOL_UNKNOWN;
		ਪੂਰਨ	buf_was_ਖਾਲੀ_ਥਾਵਾਂ = (buf->bufp->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ);
		size_t	len = 0;
		ਪੂਰਨ	added = 0;
		ਪੂਰਨ	oldFire = netbeansFireChanges;
		ਪੂਰਨ	old_b_ਬਦਲਿਆ;
		char_u	*nlp;
		linenr_T lnum;
		linenr_T lnum_ਸ਼ੁਰੂ;
		pos_T	*pos;

		netbeansFireChanges = 0;

		// Jump to the buffer where we insert.  After this "curbuf"
		// can be used.
		nb_ਸੈੱਟ_curbuf(buf->bufp);
		old_b_ਬਦਲਿਆ = curbuf->b_ਬਦਲਿਆ;

		// Convert the specified character offset into a lnum/col
		// position.
		pos = off2pos(curbuf, off);
		ਜੇ (pos != ਨਿਰਾ)
		{
		    ਜੇ (pos->lnum <= 0)
			lnum_ਸ਼ੁਰੂ = 1;
		    ਨਹੀਂਤਾਂ
			lnum_ਸ਼ੁਰੂ = pos->lnum;
		}
		ਨਹੀਂਤਾਂ
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    ਜੇ (buf_was_ਖਾਲੀ_ਥਾਵਾਂ)
			lnum_ਸ਼ੁਰੂ = 1;	    // above empty line
		    ਨਹੀਂਤਾਂ
			lnum_ਸ਼ੁਰੂ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_ਸ਼ੁਰੂ;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		ਜਦੋਂ (*ਦਲੀਲਾਂ != NUL)
		{
		    nlp = vim_strchr(ਦਲੀਲਾਂ, '\n');
		    ਜੇ (nlp == ਨਿਰਾ)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(ਦਲੀਲਾਂ);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			len = nlp - ਦਲੀਲਾਂ;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			ਜੇ (nlp > ਦਲੀਲਾਂ && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			ਨਹੀਂਤਾਂ
			    ff_detected = EOL_UNIX;
		    }
		    ਦਲੀਲਾਂ[len] = NUL;

		    ਜੇ (lnum == lnum_ਸ਼ੁਰੂ
			    && ((pos != ਨਿਰਾ && pos->ਖਾਨਾ > 0)
				|| (lnum == 1 && buf_was_ਖਾਲੀ_ਥਾਵਾਂ)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			ਪੂਰਨ	ਖਾਨਾ = pos == ਨਿਰਾ ? 0 : pos->ਖਾਨਾ;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			ਜੇ (newline != ਨਿਰਾ)
			{
			    mch_memmove(newline, oldline, (size_t)ਖਾਨਾ);
			    newline[ਖਾਨਾ] = NUL;
			    STRCAT(newline, ਦਲੀਲਾਂ);
			    STRCAT(newline, oldline + ਖਾਨਾ);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), ਦਲੀਲਾਂ,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    ਜੇ (nlp == ਨਿਰਾ)
			ਰੋਕੋ;
		    ++lnum;
		    ਦਲੀਲਾਂ = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_ਸਤਰਾਂ_mark(lnum_ਸ਼ੁਰੂ - 1, (ਲੰਮਾ)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		ਜੇ (buf_was_ਖਾਲੀ_ਥਾਵਾਂ)
		{
		    ਜੇ (ff_detected == EOL_UNKNOWN)
#ਜੇ defined(MSWIN)
			ff_detected = EOL_DOS;
#ਨਹੀਂਤਾਂ
			ff_detected = EOL_UNIX;
#ਜੇ_ਅੰਤ
		    ਸੈੱਟ_fileformat(ff_detected, OPT_LOCAL);
		    curbuf->b_ਸ਼ੁਰੂ_ffc = *curbuf->b_p_ff;
		}

		/*
		 * XXX - GRP - Is the next line right? If I've inserted
		 * text the buffer has been updated but not written. Will
		 * netbeans guarantee to write it? Even if I do a :q! ?
		 */
		curbuf->b_ਬਦਲਿਆ = old_b_ਬਦਲਿਆ; // logically unchanged
		netbeansFireChanges = oldFire;

		// Undo info is invalid now...
		u_clearallandblockfree(curbuf);
	    }
	    vim_free(ਮੰਜ਼ਿਲ_free);
	    nb_ਜਵਾਬ_nil(cmdno); // or !error
	}
	ਨਹੀਂਤਾਂ
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_ਜਵਾਬ_nil(cmdno);
	    retval = FAIL;
	}
    }
    ਨਹੀਂਤਾਂ // Not a function; no reply required.
    {
// =====================================================================
	ਜੇ (streq((ਅੱਖਰ *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_create));
		ਵਾਪਸ FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, ਨਿਰਾ, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->ਪਾਓ_ਪੂਰਾ = FALSE;
#ਜੇ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ਜੇ (gui.ਵਿੱਚ_use)
		gui_update_menus(0);
#ਜੇ_ਅੰਤ
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "insertDone"))
	{
	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    ਨਹੀਂਤਾਂ
	    {
		buf->bufp->b_ਸ਼ੁਰੂ_eol = *ਦਲੀਲਾਂ == 'T';
		buf->ਪਾਓ_ਪੂਰਾ = TRUE;
		ਦਲੀਲਾਂ += 2;
		buf->bufp->b_p_ro = *ਦਲੀਲਾਂ == 'T';
		print_read_ਸੁਨੇਹਾ(buf);
	    }
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "saveDone"))
	{
	    ਲੰਮਾ savedChars = atol((ਅੱਖਰ *)ਦਲੀਲਾਂ);

	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    ਨਹੀਂਤਾਂ
		print_save_ਸੁਨੇਹਾ(buf, savedChars);
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "startDocumentListen"))
	{
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_startdocumentlisten));
		ਵਾਪਸ FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "stopDocumentListen"))
	{
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_stopdocumentlisten));
		ਵਾਪਸ FAIL;
	    }
	    buf->fireChanges = 0;
	    ਜੇ (buf->bufp != ਨਿਰਾ && buf->bufp->b_was_netbeans_file)
	    {
		ਜੇ (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_ਗੁਆਚਾ_for_ਬਫਰ_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_ਗੁਆਚਾ_for_ਬਫਰ_nr),
							   buf->bufp->b_fnum);
		}
		ਨਹੀਂਤਾਂ
		{
		    // NetBeans uses stopDocumentListen when it stops editing
		    // a file.  It then expects the buffer in Vim to
		    // disappear.
		    do_bufdel(DOBUF_DEL, (char_u *)"", 1,
				  buf->bufp->b_fnum, buf->bufp->b_fnum, TRUE);
		    CLEAR_ਸੂਚਕ(buf);
		}
	    }
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "setTitle"))
	{
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_settitle));
		ਵਾਪਸ FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(ਦਲੀਲਾਂ, ਨਿਰਾ);
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "initDone"))
	{
	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_initdone));
		ਵਾਪਸ FAIL;
	    }
	    do_update = 1;
	    buf->init_ਪੂਰਾ = TRUE;
	    nb_ਸੈੱਟ_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_ਕੁੰਜੀ_ਕਤਾਰ();
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "setBufferNumber")
		|| streq((ਅੱਖਰ *)cmd, "putBufferNumber"))
	{
	    char_u	*ਰਸਤਾ;
	    buf_T	*bufp;

	    ਜੇ (buf == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_setbuffernumber));
		ਵਾਪਸ FAIL;
	    }
	    ਰਸਤਾ = (char_u *)nb_unquote(ਦਲੀਲਾਂ, ਨਿਰਾ);
	    ਜੇ (ਰਸਤਾ == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    bufp = buflist_findname(ਰਸਤਾ);
	    vim_free(ਰਸਤਾ);
	    ਜੇ (bufp == ਨਿਰਾ)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", ਦਲੀਲਾਂ));
		semsg(_(e_file_str_not_ਮਿਲਿਆ_ਵਿੱਚ_setbuffernumber), ਦਲੀਲਾਂ);
		ਵਾਪਸ FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_ਸੰਖਿਆ = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    ਜੇ (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    ਨਹੀਂਤਾਂ
	    {
		buf->init_ਪੂਰਾ = TRUE;

		// handle any postponed key commands
		handle_ਕੁੰਜੀ_ਕਤਾਰ();
	    }

// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "setFullName"))
	{
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_setfullname));
		ਵਾਪਸ FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(ਦਲੀਲਾਂ, ਨਿਰਾ);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#ਜੇ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ਜੇ (gui.ਵਿੱਚ_use)
		gui_update_menus(0);
#ਜੇ_ਅੰਤ
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "editFile"))
	{
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_editfile));
		ਵਾਪਸ FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(ਦਲੀਲਾਂ, ਨਿਰਾ);
	    do_ecmd(0, (char_u *)buf->displayname, ਨਿਰਾ, ਨਿਰਾ, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->init_ਪੂਰਾ = TRUE;
	    do_update = 1;
	    maketitle();
#ਜੇ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ਜੇ (gui.ਵਿੱਚ_use)
		gui_update_menus(0);
#ਜੇ_ਅੰਤ
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "setVisible"))
	{
	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ਜੇ (p_verbose > 0)
		    emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_setvisible));
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (streq((ਅੱਖਰ *)ਦਲੀਲਾਂ, "T") && buf->bufp != curbuf)
	    {
		exarg_T exarg;
		CLEAR_ਖੇਤਰ(exarg);
		exarg.cmd = (char_u *)"goto";
		exarg.forceit = FALSE;
		exarg.cmdidx = CMD_USER;
		dosetvisible = TRUE;
		goto_ਬਫਰ(&exarg, DOBUF_ਪਹਿਲਾ, FORWARD, buf->bufp->b_fnum);
		do_update = 1;
		dosetvisible = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		// Side effect!!!.
		ਜੇ (gui.ਵਿੱਚ_use)
		    gui_mch_ਸੈੱਟ_foreground();
#ਜੇ_ਅੰਤ
	    }
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "raise"))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    // Bring gvim to the foreground.
	    ਜੇ (gui.ਵਿੱਚ_use)
		gui_mch_ਸੈੱਟ_foreground();
#ਜੇ_ਅੰਤ
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "setModified"))
	{
	    ਪੂਰਨ prev_b_ਬਦਲਿਆ;

	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ਜੇ (p_verbose > 0)
		    emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_setmodified));
		ਵਾਪਸ FAIL;
	    }
	    prev_b_ਬਦਲਿਆ = buf->bufp->b_ਬਦਲਿਆ;
	    ਜੇ (streq((ਅੱਖਰ *)ਦਲੀਲਾਂ, "T"))
		buf->bufp->b_ਬਦਲਿਆ = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		ਜੇ (buf->bufp->b_ffname != ਨਿਰਾ
			&& mch_stat((ਅੱਖਰ *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_ਸਮਾਂ(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_ਬਦਲਿਆ = FALSE;
	    }
	    buf->modified = buf->bufp->b_ਬਦਲਿਆ;
	    ਜੇ (prev_b_ਬਦਲਿਆ != buf->bufp->b_ਬਦਲਿਆ)
	    {
		check_ਸਥਿਤੀ(buf->bufp);
		redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "setModtime"))
	{
	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    ਨਹੀਂਤਾਂ
	    {
		buf->bufp->b_mtime = atoi((ਅੱਖਰ *)ਦਲੀਲਾਂ);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "setReadOnly"))
	{
	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)ਦਲੀਲਾਂ, "T"))
		buf->bufp->b_p_ro = TRUE;
	    ਨਹੀਂਤਾਂ
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "showBalloon"))
	{
#ਜੇ defined(FEAT_BEVAL_GUI)
	    ਸਥਿਰ ਅੱਖਰ	*ਪਾਠ = ਨਿਰਾ;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    ਜੇ (balloonEval != ਨਿਰਾ)
	    {
		vim_free(ਪਾਠ);
		ਪਾਠ = nb_unquote(ਦਲੀਲਾਂ, ਨਿਰਾ);
		ਜੇ (ਪਾਠ != ਨਿਰਾ)
		    gui_mch_post_balloon(balloonEval, (char_u *)ਪਾਠ);
	    }
#ਜੇ_ਅੰਤ
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "setDot"))
	{
	    pos_T *pos;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NBDEBUG
	    char_u *s;
#ਜੇ_ਅੰਤ

	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_setdot));
		ਵਾਪਸ FAIL;
	    }

	    nb_ਸੈੱਟ_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    ਜੇ (VIsual_ਸਰਗਰਮ)
		ਸਿਰਾ_visual_ਬਹੁਲਕ();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NBDEBUG
	    s = ਦਲੀਲਾਂ;
#ਜੇ_ਅੰਤ
	    pos = get_off_or_lnum(buf->bufp, &ਦਲੀਲਾਂ);
	    ਜੇ (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		foldOpenCursor();
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
	    {
		nbdebug(("    BAD POSITION in setDot: %s\n", s));
	    }

	    // gui_update_cursor(TRUE, FALSE);
	    // update_curbuf(UPD_NOT_VALID);
	    update_topline();		// scroll to show the line
	    update_screen(UPD_VALID);
	    setcursor();
	    cursor_on();
	    ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);

	    // Quit a hit-return or more prompt.
	    ਜੇ (State == ਬਹੁਲਕ_HITRETURN || State == ਬਹੁਲਕ_ASKMORE)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
		ਜੇ (gui.ਵਿੱਚ_use && gtk_main_level() > 0)
		    gtk_main_quit();
#ਜੇ_ਅੰਤ
	    }
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "close"))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NBDEBUG
	    ਅੱਖਰ *name = "<NONE>";
#ਜੇ_ਅੰਤ

	    ਜੇ (buf == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_close));
		ਵਾਪਸ FAIL;
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NBDEBUG
	    ਜੇ (buf->displayname != ਨਿਰਾ)
		name = buf->displayname;
#ਜੇ_ਅੰਤ
	    ਜੇ (buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ਜੇ (p_verbose > 0)
		    emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
	    ਜੇ (buf->bufp != ਨਿਰਾ)
		do_ਬਫਰ(DOBUF_WIPE, DOBUF_ਪਹਿਲਾ, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = ਨਿਰਾ;
	    buf->init_ਪੂਰਾ = FALSE;
	    do_update = 1;
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "defineAnnoType"))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	    ਪੂਰਨ type_ਅੰਕ;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    ਪੂਰਨ parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    ਜੇ (buf == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_defineannotype));
		ਵਾਪਸ FAIL;
	    }

	    cp = (ਅੱਖਰ *)ਦਲੀਲਾਂ;
	    type_ਅੰਕ = strtol(cp, &cp, 10);
	    ਦਲੀਲਾਂ = (char_u *)cp;
	    ਦਲੀਲਾਂ = skipwhite(ਦਲੀਲਾਂ);
	    typeName = (char_u *)nb_unquote(ਦਲੀਲਾਂ, &ਦਲੀਲਾਂ);
	    ਦਲੀਲਾਂ = skipwhite(ਦਲੀਲਾਂ + 1);
	    tooltip = (char_u *)nb_unquote(ਦਲੀਲਾਂ, &ਦਲੀਲਾਂ);
	    ਦਲੀਲਾਂ = skipwhite(ਦਲੀਲਾਂ + 1);

	    p = (char_u *)nb_unquote(ਦਲੀਲਾਂ, &ਦਲੀਲਾਂ);
	    glyphFile = vim_strsave_escaped(p, escape_chars);
	    vim_free(p);

	    ਦਲੀਲਾਂ = skipwhite(ਦਲੀਲਾਂ + 1);
	    p = skiptowhite(ਦਲੀਲਾਂ);
	    ਜੇ (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(ਦਲੀਲਾਂ);
	    bg = vim_strsave(p);
	    ਜੇ (STRLEN(fg) > MAX_COLOR_ਲੰਮਾਈ || STRLEN(bg) > MAX_COLOR_ਲੰਮਾਈ)
	    {
		emsg(_(e_highlighting_color_name_too_long_ਵਿੱਚ_define_Anno_Type));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!nb_is_ਸੁਰੱਖਿਅਤ_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_ਸੁਰੱਖਿਅਤ_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_ਸੁਰੱਖਿਅਤ_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_ਵਿੱਚ_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (typeName != ਨਿਰਾ && tooltip != ਨਿਰਾ && glyphFile != ਨਿਰਾ)
		addsigntype(buf, type_ਅੰਕ, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    ਜੇ (parse_error)
		ਵਾਪਸ FAIL;

#ਜੇ_ਅੰਤ
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "addAnno"))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	    ਪੂਰਨ ser_ਅੰਕ;
	    ਪੂਰਨ local_Type_ਅੰਕ;
	    ਪੂਰਨ type_ਅੰਕ;
	    pos_T *pos;

	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_ਬਫਰ_identifier_ਵਿੱਚ_addanno));
		ਵਾਪਸ FAIL;
	    }

	    do_update = 1;

	    cp = (ਅੱਖਰ *)ਦਲੀਲਾਂ;
	    ser_ਅੰਕ = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_ਅੰਕ = strtol(cp, &cp, 10);
	    ਦਲੀਲਾਂ = (char_u *)cp;
	    type_ਅੰਕ = mapsigntype(buf, local_Type_ਅੰਕ);

	    pos = get_off_or_lnum(buf->bufp, &ਦਲੀਲਾਂ);

	    cp = (ਅੱਖਰ *)ਦਲੀਲਾਂ;
	    vim_ignored = (ਪੂਰਨ)strtol(cp, &cp, 10);
	    ਦਲੀਲਾਂ = (char_u *)cp;
# ifdef NBDEBUG
	    ਜੇ (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    ਜੇ (ser_ਅੰਕ >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_ਅੰਕ, pos->lnum, type_ਅੰਕ, buf->bufp->b_fnum);
		ਜੇ (type_ਅੰਕ == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_ਅੰਕ,
						       buf->bufp->b_fnum);
	    }
#ਜੇ_ਅੰਤ
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "removeAnno"))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	    ਪੂਰਨ ser_ਅੰਕ;

	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		ਵਾਪਸ FAIL;
	    }
	    do_update = 1;
	    cp = (ਅੱਖਰ *)ਦਲੀਲਾਂ;
	    ser_ਅੰਕ = strtol(cp, &cp, 10);
	    ਦਲੀਲਾਂ = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_ਅੰਕ, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#ਜੇ_ਅੰਤ
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "moveAnnoToFront"))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#ਜੇ_ਅੰਤ
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "guard") || streq((ਅੱਖਰ *)cmd, "unguard"))
	{
	    ਪੂਰਨ len;
	    pos_T ਪਹਿਲਾ;
	    pos_T ਆਖਰੀ;
	    pos_T *pos;
	    ਪੂਰਨ un = (cmd[0] == 'u');
	    ਸਥਿਰ ਪੂਰਨ guardId = GUARDEDOFFSET;

	    ਜੇ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (ਅੱਖਰ *) cmd));
		ਵਾਪਸ OK;
	    }

	    nb_init_graphics();

	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ਵਾਪਸ FAIL;
	    }
	    nb_ਸੈੱਟ_curbuf(buf->bufp);
	    cp = (ਅੱਖਰ *)ਦਲੀਲਾਂ;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, ਨਿਰਾ, 10);
	    ਦਲੀਲਾਂ = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    ਜੇ (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    ਨਹੀਂਤਾਂ
	    {
		ਪਹਿਲਾ = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		ਜੇ (pos != ਨਿਰਾ && pos->ਖਾਨਾ == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		ਜੇ (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		ਨਹੀਂਤਾਂ
		{
		    ਲੰਮਾ lnum;
		    ਆਖਰੀ = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     ਪਹਿਲਾ.lnum, ਪਹਿਲਾ.ਖਾਨਾ,
			     ਆਖਰੀ.lnum, ਆਖਰੀ.ਖਾਨਾ));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
		    ਲਈ (lnum = ਪਹਿਲਾ.lnum; lnum <= ਆਖਰੀ.lnum; lnum++)
		    {
			ਜੇ (un)
			{
			    // never used
			}
			ਨਹੀਂਤਾਂ
			{
			    ਜੇ (buf_findsigntype_id(buf->bufp, lnum,
				GUARDED) == 0)
			    {
				coloncmd(
				    ":sign place %d line=%ld name=%d buffer=%d",
				     guardId++, lnum, GUARDED,
				     buf->bufp->b_fnum);
			    }
			}
		    }
#ਜੇ_ਅੰਤ
		    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
		}
	    }
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "startAtomic"))
	{
	    ਵਿੱਚ_Atomic = 1;
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "endAtomic"))
	{
	    ਵਿੱਚ_Atomic = 0;
	    ਜੇ (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ਵਾਪਸ FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    ਜੇ (buf_Is_ਬਦਲਿਆ(buf->bufp))
	    {
		// Only write if the buffer can be written.
		ਜੇ (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != ਨਿਰਾ
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    ਸੈੱਟ_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    ਜੇ (!bufref_valid(&bufref))
			buf->bufp = ਨਿਰਾ;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "netbeansBuffer"))
	{
	    ਜੇ (buf == ਨਿਰਾ || buf->bufp == ਨਿਰਾ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (*ਦਲੀਲਾਂ == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    ਨਹੀਂਤਾਂ
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "specialKeys"))
	{
	    special_ਕੁੰਜੀਆਂ(ਦਲੀਲਾਂ);
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	ਨਹੀਂਤਾਂ ਜੇ (streq((ਅੱਖਰ *)cmd, "version"))
	{
	    // not used yet
	}
	ਨਹੀਂਤਾਂ
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    ਜੇ (ਵਿੱਚ_Atomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    ਜੇ (buf != ਨਿਰਾ && buf->init_ਪੂਰਾ && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	ਜੇ (State == ਬਹੁਲਕ_HITRETURN || State == ਬਹੁਲਕ_ASKMORE)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	    ਜੇ (gui.ਵਿੱਚ_use && gtk_main_level() > 0)
		gtk_main_quit();
#ਜੇ_ਅੰਤ
	}
    }

    ਵਾਪਸ retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    ਸਥਿਰ ਖਾਲੀ
nb_ਸੈੱਟ_curbuf(buf_T *buf)
{
    ਜੇ (curbuf == buf)
	ਵਾਪਸ;

    ਜੇ (buf_jump_open_win(buf) != ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != ਨਿਰਾ)
	ਵਾਪਸ;
    ਸੈੱਟ_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    ਸਥਿਰ ਖਾਲੀ
coloncmd(ਅੱਖਰ *cmd, ...)
{
    ਅੱਖਰ buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, ਆਕਾਰ(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, ਨਿਰਾ, ਨਿਰਾ, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    ਸਥਿਰ ਖਾਲੀ
special_ਕੁੰਜੀਆਂ(char_u *ਦਲੀਲਾਂ)
{
    ਅੱਖਰ *save_str = nb_unquote(ਦਲੀਲਾਂ, ਨਿਰਾ);
    ਅੱਖਰ *tok = strtok(save_str, " ");
    ਅੱਖਰ *sep;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEYBUFLEN 64
    ਅੱਖਰ keybuf[KEYBUFLEN];
    ਅੱਖਰ cmdbuf[256];

    ਜਦੋਂ (tok != ਨਿਰਾ)
    {
	ਪੂਰਨ i = 0;

	ਜੇ ((sep = strchr(tok, '-')) != ਨਿਰਾ)
	{
	    *sep = NUL;
	    ਜਦੋਂ (*tok && i + 2 < KEYBUFLEN)
	    {
		ਚੋਣ (*tok)
		{
		    ਹਾਲਤ 'A':
		    ਹਾਲਤ 'M':
		    ਹਾਲਤ 'C':
		    ਹਾਲਤ 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			ਰੋਕੋ;
		}
		tok++;
	    }
	    tok++;
	}

	ਜੇ (ਲੜੀ_ਲੰਬਾਈ(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    ਪੂਰਨ ਸੁਰੱਖਿਅਤ_2 = TRUE;
	    ਲਈ (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		ਜੇ (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    ਸੁਰੱਖਿਅਤ_2 = FALSE;
		    ਰੋਕੋ;
		}
	    }
	    ਜੇ (ਸੁਰੱਖਿਅਤ_2)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, ਆਕਾਰ(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, ਬਹੁਲਕ_ਸਾਧਾਰਨ, FALSE);
	    }
	}
	tok = strtok(ਨਿਰਾ, " ");
    }
    vim_free(save_str);
}

    ਖਾਲੀ
ex_nbclose(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    netbeans_close();
}

    ਖਾਲੀ
ex_nbkey(exarg_T *eap)
{
    (ਖਾਲੀ)netbeans_keystring(eap->arg);
}

    ਖਾਲੀ
ex_nbstart(
    exarg_T	*eap)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# ਜੇ !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	ਵਾਪਸ;
    }
# endif
#ਜੇ_ਅੰਤ
    netbeans_open((ਅੱਖਰ *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    ਸਥਿਰ ਖਾਲੀ
nb_init_graphics(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ did_init = FALSE;

    ਜੇ (did_init)
	ਵਾਪਸ;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    ਸਥਿਰ ਖਾਲੀ
netbeans_keyname(ਪੂਰਨ ਕੁੰਜੀ, ਅੱਖਰ *buf)
{
    ਅੱਖਰ *name = 0;
    ਅੱਖਰ namebuf[2];
    ਪੂਰਨ ctrl  = 0;
    ਪੂਰਨ ਖਿਸਕਾਓ = 0;
    ਪੂਰਨ alt   = 0;

    ਜੇ (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL)
	ctrl = 1;
    ਜੇ (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)
	ਖਿਸਕਾਓ = 1;
    ਜੇ (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ALT)
	alt = 1;


    ਚੋਣ (ਕੁੰਜੀ)
    {
	ਹਾਲਤ K_F1:		name = "F1";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F1:	name = "F1";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਹਾਲਤ K_F2:		name = "F2";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F2:	name = "F2";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਹਾਲਤ K_F3:		name = "F3";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F3:	name = "F3";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਹਾਲਤ K_F4:		name = "F4";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F4:	name = "F4";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਹਾਲਤ K_F5:		name = "F5";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F5:	name = "F5";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਹਾਲਤ K_F6:		name = "F6";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F6:	name = "F6";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਹਾਲਤ K_F7:		name = "F7";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F7:	name = "F7";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਹਾਲਤ K_F8:		name = "F8";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F8:	name = "F8";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਹਾਲਤ K_F9:		name = "F9";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F9:	name = "F9";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਹਾਲਤ K_F10:		name = "F10";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F10:	name = "F10";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਹਾਲਤ K_F11:		name = "F11";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F11:	name = "F11";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਹਾਲਤ K_F12:		name = "F12";		ਰੋਕੋ;
	ਹਾਲਤ K_S_F12:	name = "F12";	ਖਿਸਕਾਓ = 1;	ਰੋਕੋ;
	ਮੂਲ:
			ਜੇ (ਕੁੰਜੀ >= ' ' && ਕੁੰਜੀ <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = ਕੁੰਜੀ;
			    namebuf[1] = NUL;
			}
			ਨਹੀਂਤਾਂ
			    name = "X";
			ਰੋਕੋ;
    }

    buf[0] = '\0';
    ਜੇ (ctrl)
	strcat(buf, "C");
    ਜੇ (ਖਿਸਕਾਓ)
	strcat(buf, "S");
    ਜੇ (alt)
	strcat(buf, "M"); // META
    ਜੇ (ctrl || ਖਿਸਕਾਓ || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#ਜੇ defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    ਖਾਲੀ
netbeans_beval_cb(
	BalloonEval	*beval,
	ਪੂਰਨ		 state ਅਣਵਰਤਿਆ)
{
    win_T	*wp;
    char_u	*ਪਾਠ;
    linenr_T	lnum;
    ਪੂਰਨ		ਖਾਨਾ;
    ਅੱਖਰ	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    ਜੇ (!can_use_beval() || !NETBEANS_OPEN)
	ਵਾਪਸ;

    ਜੇ (get_beval_info(beval, TRUE, &wp, &lnum, &ਪਾਠ, &ਖਾਨਾ) != OK)
	ਵਾਪਸ;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    ਜੇ (ਪਾਠ != ਨਿਰਾ && ਪਾਠ[0] != NUL && STRLEN(ਪਾਠ) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	ਜੇ (buf != ਨਿਰਾ)
	{
	    p = nb_quote(ਪਾਠ);
	    ਜੇ (p != ਨਿਰਾ)
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
    vim_free(ਪਾਠ);
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE when the netbeans connection is active.
 */
    ਪੂਰਨ
netbeans_ਸਰਗਰਮ(ਖਾਲੀ)
{
    ਵਾਪਸ NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    ਖਾਲੀ
netbeans_open(ਅੱਖਰ *params, ਪੂਰਨ doabort)
{
    ਅੱਖਰ *cmd = "0:startupDone=0\n";

    ਜੇ (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	ਵਾਪਸ;
    }

    ਜੇ (netbeans_connect(params, doabort) != OK)
	ਵਾਪਸ;

    nbdebug(("EVT: %s", cmd));
    nb_send(cmd, "netbeans_startup_done");

    // update the screen after having added the gutter
    ਬਦਲਿਆ_ਖਿੜਕੀ_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);
}

/*
 * Tell netbeans that we're exiting. This should be called right
 * before calling exit.
 */
    ਖਾਲੀ
netbeans_send_disconnect(ਖਾਲੀ)
{
    ਅੱਖਰ buf[128];

    ਜੇ (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#ਜੇ defined(FEAT_EVAL)
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_nb_channel(ਪੂਰਨ copyID)
{
    ਪੂਰਨ abort = FALSE;
    typval_T tv;

    ਜੇ (nb_channel == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    ਵਾਪਸ abort;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    ਖਾਲੀ
netbeans_ਚੌਖਟਾ_moved(ਪੂਰਨ new_x, ਪੂਰਨ new_y)
{
    ਅੱਖਰ buf[128];

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (ਪੂਰਨ)Columns, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#ਜੇ_ਅੰਤ

/*
 * Tell netbeans the user opened or activated a file.
 */
    ਖਾਲੀ
netbeans_file_activated(buf_T *bufp)
{
    ਪੂਰਨ bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    ਅੱਖਰ    ਬਫਰ[2*MAXPATHL];
    char_u  *q;

    ਜੇ (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	ਵਾਪਸ;

    q = nb_quote(bufp->b_ffname);
    ਜੇ (q == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (bp == ਨਿਰਾ)
    {
	vim_free(q);
	ਵਾਪਸ;
    }

    vim_snprintf(ਬਫਰ, ਆਕਾਰ(ਬਫਰ),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (ਅੱਖਰ *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", ਬਫਰ));

    nb_send(ਬਫਰ, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    ਖਾਲੀ
netbeans_file_opened(buf_T *bufp)
{
    ਪੂਰਨ bufno = nb_getbufno(bufp);
    ਅੱਖਰ    ਬਫਰ[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    ਪੂਰਨ	    bnum;

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    q = nb_quote(bufp->b_ffname);
    ਜੇ (q == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (bp != ਨਿਰਾ)
	bnum = bufno;
    ਨਹੀਂਤਾਂ
	bnum = 0;

    vim_snprintf(ਬਫਰ, ਆਕਾਰ(ਬਫਰ), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (ਅੱਖਰ *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", ਬਫਰ));

    nb_send(ਬਫਰ, "netbeans_file_opened");
    ਜੇ (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	ਆਖਰੀ_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    ਖਾਲੀ
netbeans_file_killed(buf_T *bufp)
{
    ਪੂਰਨ		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    ਅੱਖਰ	ਬਫਰ[2*MAXPATHL];

    ਜੇ (!NETBEANS_OPEN || bufno == -1)
	ਵਾਪਸ;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(ਬਫਰ, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", ਬਫਰ));

    nb_send(ਬਫਰ, "netbeans_file_killed");

    ਜੇ (nbbuf != ਨਿਰਾ)
	nbbuf->bufp = ਨਿਰਾ;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    ਸਥਿਰ nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, ਪੂਰਨ *bufnop)
{
    ਪੂਰਨ		bufno;
    nbbuf_T	*nbbuf;

    ਜੇ (!NETBEANS_OPEN || !netbeansFireChanges)
	ਵਾਪਸ ਨਿਰਾ;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    ਜੇ (bufno <= 0)
	ਵਾਪਸ ਨਿਰਾ;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    ਜੇ (nbbuf != ਨਿਰਾ && !nbbuf->fireChanges)
	ਵਾਪਸ ਨਿਰਾ;		// changes in this buffer are not reported

    *bufnop = bufno;
    ਵਾਪਸ nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    ਖਾਲੀ
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	ਖਾਨਾ,
    char_u	*txt,
    ਪੂਰਨ		newlen)
{
    char_u	*buf;
    ਪੂਰਨ		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    ਲੰਮਾ	off;
    char_u	*p;
    char_u	*newtxt;

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ਜੇ (nbbuf == ਨਿਰਾ)
	ਵਾਪਸ;

    // Don't mark as modified for initial read
    ਜੇ (nbbuf->ਪਾਓ_ਪੂਰਾ)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.ਖਾਨਾ = ਖਾਨਾ;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    ਜੇ (p != ਨਿਰਾ)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((ਅੱਖਰ *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((ਅੱਖਰ *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    ਖਾਲੀ
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	ਖਾਨਾ,
    ਲੰਮਾ	len)
{
    char_u	buf[128];
    ਪੂਰਨ		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    ਲੰਮਾ	off;

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ਜੇ (nbbuf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	ਵਾਪਸ;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.ਖਾਨਾ = ਖਾਨਾ;

    off = pos2off(bufp, &pos);

    sprintf((ਅੱਖਰ *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((ਅੱਖਰ *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    ਖਾਲੀ
netbeans_unmodified(buf_T *bufp ਅਣਵਰਤਿਆ)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    ਖਾਲੀ
netbeans_button_release(ਪੂਰਨ button)
{
    ਅੱਖਰ	buf[128];
    ਪੂਰਨ		bufno;

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    bufno = nb_getbufno(curbuf);

    ਜੇ (bufno < 0 || curwin == ਨਿਰਾ || curwin->w_ਬਫਰ != curbuf)
	ਵਾਪਸ;

    ਪੂਰਨ ਖਾਨਾ = mouse_ਖਾਨਾ - curwin->w_wincol
	- ((curwin->w_p_nu || curwin->w_p_rnu) ? 9 : 1);
    ਲੰਮਾ off = pos2off(curbuf, &curwin->w_cursor);

    // sync the cursor position
    sprintf(buf, "%d:newDotAndMark=%d %ld %ld\n", bufno, r_cmdno, off, off);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release[newDotAndMark]");

    sprintf(buf, "%d:buttonRelease=%d %d %ld %d\n", bufno, r_cmdno,
	    button, (ਲੰਮਾ)curwin->w_cursor.lnum, ਖਾਨਾ);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release");
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key code.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    ਪੂਰਨ
netbeans_keycommand(ਪੂਰਨ ਕੁੰਜੀ)
{
    ਅੱਖਰ	ਕੁੰਜੀ_Name[60];

    netbeans_keyname(ਕੁੰਜੀ, ਕੁੰਜੀ_Name);
    ਵਾਪਸ netbeans_keystring((char_u *)ਕੁੰਜੀ_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    ਸਥਿਰ ਪੂਰਨ
netbeans_keystring(char_u *ਕੁੰਜੀ_Name)
{
    ਅੱਖਰ	buf[2*MAXPATHL];
    ਪੂਰਨ		bufno = nb_getbufno(curbuf);
    ਲੰਮਾ	off;
    char_u	*q;

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ TRUE;

    ਜੇ (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == ਨਿਰਾ ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	ਜੇ (q == ਨਿਰਾ)
	    ਵਾਪਸ TRUE;
	vim_snprintf(buf, ਆਕਾਰ(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	ਜੇ (curbuf->b_ffname != ਨਿਰਾ)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(ਕੁੰਜੀ_Name);
	ਵਾਪਸ FALSE;
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
    vim_snprintf(buf, ਆਕਾਰ(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, ਕੁੰਜੀ_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, ਆਕਾਰ(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, ਕੁੰਜੀ_Name,
		off, (ਲੰਮਾ)curwin->w_cursor.lnum, (ਲੰਮਾ)curwin->w_cursor.ਖਾਨਾ);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    ਵਾਪਸ TRUE;
}


/*
 * Send a save event to netbeans.
 */
    ਖਾਲੀ
netbeans_save_ਬਫਰ(buf_T *bufp)
{
    char_u	buf[64];
    ਪੂਰਨ		bufno;
    nbbuf_T	*nbbuf;

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ਜੇ (nbbuf == ਨਿਰਾ)
	ਵਾਪਸ;

    nbbuf->modified = 0;

    sprintf((ਅੱਖਰ *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((ਅੱਖਰ *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    ਖਾਲੀ
netbeans_deleted_all_ਸਤਰਾਂ(buf_T *bufp)
{
    char_u	buf[64];
    ਪੂਰਨ		bufno;
    nbbuf_T	*nbbuf;

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ਜੇ (nbbuf == ਨਿਰਾ)
	ਵਾਪਸ;

    // Don't mark as modified for initial read
    ਜੇ (nbbuf->ਪਾਓ_ਪੂਰਾ)
	nbbuf->modified = 1;

    sprintf((ਅੱਖਰ *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    ਪੂਰਨ
netbeans_is_guarded(linenr_T ਸਿਖਰ, linenr_T bot)
{
    sign_ਇੰਦਰਾਜ_T	*p;
    ਪੂਰਨ			lnum;

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ FALSE;

    FOR_ALL_SIGNS_ਵਿੱਚ_BUF(curbuf, p)
	ਜੇ (p->se_id >= GUARDEDOFFSET)
	    ਲਈ (lnum = ਸਿਖਰ + 1; lnum < bot; lnum++)
		ਜੇ (lnum == p->se_lnum)
		    ਵਾਪਸ TRUE;

    ਵਾਪਸ FALSE;
}

#ਜੇ defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    ਖਾਲੀ
netbeans_draw_multisign_indicator(ਪੂਰਨ ਪੰਕਤੀ)
{
    ਪੂਰਨ i;
    ਪੂਰਨ y;
    ਪੂਰਨ x;

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    x = 0;
    y = ਪੰਕਤੀ * gui.char_ਉਚਾਈ + 2;

    ਲਈ (i = 0; i < gui.char_ਉਚਾਈ - 3; i++)
	XDraw_ਬਿੰਦੂ(gui.dpy, gui.wid, gui.ਪਾਠ_gc, x+2, y++);

    XDraw_ਬਿੰਦੂ(gui.dpy, gui.wid, gui.ਪਾਠ_gc, x+0, y);
    XDraw_ਬਿੰਦੂ(gui.dpy, gui.wid, gui.ਪਾਠ_gc, x+2, y);
    XDraw_ਬਿੰਦੂ(gui.dpy, gui.wid, gui.ਪਾਠ_gc, x+4, y++);
    XDraw_ਬਿੰਦੂ(gui.dpy, gui.wid, gui.ਪਾਠ_gc, x+1, y);
    XDraw_ਬਿੰਦੂ(gui.dpy, gui.wid, gui.ਪਾਠ_gc, x+2, y);
    XDraw_ਬਿੰਦੂ(gui.dpy, gui.wid, gui.ਪਾਠ_gc, x+3, y++);
    XDraw_ਬਿੰਦੂ(gui.dpy, gui.wid, gui.ਪਾਠ_gc, x+2, y);
}
#ਜੇ_ਅੰਤ // FEAT_GUI_X11

#ਜੇ defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    ਖਾਲੀ
netbeans_draw_multisign_indicator(ਪੂਰਨ ਪੰਕਤੀ)
{
    ਪੂਰਨ i;
    ਪੂਰਨ y;
    ਪੂਰਨ x;
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    cairo_t *cr = ਨਿਰਾ;
# ਨਹੀਂਤਾਂ
    GdkDrawable *drawable = gui.drawarea->ਖਿੜਕੀ_2;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# ਨਹੀਂਤਾਂ
    y = ਪੰਕਤੀ * gui.char_ਉਚਾਈ + 2;
# endif

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_ਉਚਾਈ);
    cr = cairo_create(surf);
#  ਨਹੀਂਤਾਂ
    cr = cairo_create(gui.surface);
#  endif
    cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    ਲਈ (i = 0; i < gui.char_ਉਚਾਈ - 3; i++)
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# ਨਹੀਂਤਾਂ
    gdk_draw_ਬਿੰਦੂ(drawable, gui.ਪਾਠ_gc, x+2, y++);
# endif

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# ਨਹੀਂਤਾਂ
    gdk_draw_ਬਿੰਦੂ(drawable, gui.ਪਾਠ_gc, x+0, y);
    gdk_draw_ਬਿੰਦੂ(drawable, gui.ਪਾਠ_gc, x+2, y);
    gdk_draw_ਬਿੰਦੂ(drawable, gui.ਪਾਠ_gc, x+4, y++);
    gdk_draw_ਬਿੰਦੂ(drawable, gui.ਪਾਠ_gc, x+1, y);
    gdk_draw_ਬਿੰਦੂ(drawable, gui.ਪਾਠ_gc, x+2, y);
    gdk_draw_ਬਿੰਦੂ(drawable, gui.ਪਾਠ_gc, x+3, y++);
    gdk_draw_ਬਿੰਦੂ(drawable, gui.ਪਾਠ_gc, x+2, y);
# endif

# ifdef USE_GTK4
    cairo_fill(cr);
    gui_gtk4_ਜੋੜੋ_multisign(surf, ਪੰਕਤੀ, 0, 5, gui.char_ਉਚਾਈ);
# endif

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    cairo_destroy(cr);
# endif
# ifdef USE_GTK4
    cairo_surface_destroy(surf);
# endif
}
#ਜੇ_ਅੰਤ // FEAT_GUI_GTK

/*
 * If the mouse is clicked in the gutter of a line with multiple
 * annotations, cycle through the set of signs.
 */
    ਖਾਲੀ
netbeans_gutter_click(linenr_T lnum)
{
    sign_ਇੰਦਰਾਜ_T	*p;

    ਜੇ (!NETBEANS_OPEN)
	ਵਾਪਸ;

    FOR_ALL_SIGNS_ਵਿੱਚ_BUF(curbuf, p)
    {
	ਜੇ (p->se_lnum == lnum && p->se_ਅਗਲਾ && p->se_ਅਗਲਾ->se_lnum == lnum)
	{
	    sign_ਇੰਦਰਾਜ_T *ਪੂਛ;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    ਜੇ (p->se_prev)
		p->se_prev->se_ਅਗਲਾ = p->se_ਅਗਲਾ;
	    ਨਹੀਂਤਾਂ
		curbuf->b_signlist = p->se_ਅਗਲਾ;
	    p->se_ਅਗਲਾ->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    ਲਈ (ਪੂਛ = p->se_ਅਗਲਾ;
		  ਪੂਛ->se_ਅਗਲਾ && ਪੂਛ->se_ਅਗਲਾ->se_lnum == lnum
				       && ਪੂਛ->se_ਅਗਲਾ->se_id < GUARDEDOFFSET;
		  ਪੂਛ = ਪੂਛ->se_ਅਗਲਾ)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_ਅਗਲਾ = ਪੂਛ->se_ਅਗਲਾ;
	    ਜੇ (ਪੂਛ->se_ਅਗਲਾ)
		ਪੂਛ->se_ਅਗਲਾ->se_prev = p;
	    p->se_prev = ਪੂਛ;
	    ਪੂਛ->se_ਅਗਲਾ = p;
	    update_debug_sign(curbuf, lnum);
	    ਰੋਕੋ;
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
    ਸਥਿਰ ਖਾਲੀ
addsigntype(
    nbbuf_T	*buf,
    ਪੂਰਨ		type_ਅੰਕ,
    char_u	*typeName,
    char_u	*tooltip ਅਣਵਰਤਿਆ,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    ਪੂਰਨ i, j;
    ਪੂਰਨ use_fg = (*fg && STRCMP(fg, "none") != 0);
    ਪੂਰਨ use_bg = (*bg && STRCMP(bg, "none") != 0);

    ਲਈ (i = 0; i < globalsignmapused; i++)
	ਜੇ (STRCMP(typeName, globalsignmap[i]) == 0)
	    ਰੋਕੋ;

    ਜੇ (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_ਅੰਕ, typeName, tooltip, glyphFile, fg, bg));
	ਜੇ (use_fg || use_bg)
	{
	    ਅੱਖਰ fgbuf[2 * (8 + MAX_COLOR_ਲੰਮਾਈ) + 1];
	    ਅੱਖਰ bgbuf[2 * (8 + MAX_COLOR_ਲੰਮਾਈ) + 1];
	    ਅੱਖਰ *ptr;
	    ਪੂਰਨ ਮੁੱਲ;

	    ਮੁੱਲ = strtol((ਅੱਖਰ *)fg, &ptr, 10);
	    ਜੇ (ptr != (ਅੱਖਰ *)fg)
		sprintf(fgbuf, "guifg=#%06x", ਮੁੱਲ & 0xFFFFFF);
	    ਨਹੀਂਤਾਂ
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    ਮੁੱਲ = strtol((ਅੱਖਰ *)bg, &ptr, 10);
	    ਜੇ (ptr != (ਅੱਖਰ *)bg)
		sprintf(bgbuf, "guibg=#%06x", ਮੁੱਲ & 0xFFFFFF);
	    ਨਹੀਂਤਾਂ
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    ਜੇ (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    ਨਹੀਂਤਾਂ ਜੇ (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    ਨਹੀਂਤਾਂ
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	ਨਹੀਂਤਾਂ
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	ਜੇ (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_ਅੰਕ;

	ਜੇ (globalsignmapused == globalsignmaplen)
	{
	    ਜੇ (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(ਅੱਖਰ *, globalsignmaplen);
	    }
	    ਨਹੀਂਤਾਂ    // grow it
	    {
		ਪੂਰਨ incr;
		ਪੂਰਨ oldlen = globalsignmaplen;
		ਅੱਖਰ **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * ਆਕਾਰ(ਅੱਖਰ *));
		ਜੇ (globalsignmap == ਨਿਰਾ)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    ਵਾਪਸ;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * ਆਕਾਰ(ਅੱਖਰ *));
	    }
	}

	globalsignmap[i] = (ਅੱਖਰ *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    ਲਈ (j = 0; j < buf->signmapused; j++)
	ਜੇ (buf->signmap[j] == i + 1)
	    ਵਾਪਸ;

    // add to local map
    ਜੇ (buf->signmapused == buf->signmaplen)
    {
	ਜੇ (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(ਪੂਰਨ, buf->signmaplen);
	}
	ਨਹੀਂਤਾਂ    // grow it
	{
	    ਪੂਰਨ incr;
	    ਪੂਰਨ oldlen = buf->signmaplen;
	    ਪੂਰਨ *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * ਆਕਾਰ(ਪੂਰਨ));
	    ਜੇ (buf->signmap == ਨਿਰਾ)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		ਵਾਪਸ;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * ਆਕਾਰ(ਪੂਰਨ));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    ਸਥਿਰ ਪੂਰਨ
mapsigntype(nbbuf_T *buf, ਪੂਰਨ localsigntype)
{
    ਜੇ (--localsigntype >= 0 && localsigntype < buf->signmapused)
	ਵਾਪਸ buf->signmap[localsigntype];

    ਵਾਪਸ 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    ਸਥਿਰ ਲੰਮਾ
get_buf_ਮਾਪ(buf_T *bufp)
{
    linenr_T	lnum;
    ਲੰਮਾ	char_ਗਿਣਤੀ = 0;
    ਪੂਰਨ		eol_ਮਾਪ;
    ਲੰਮਾ	ਆਖਰੀ_check = 100000L;

    ਜੇ (bufp->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
	ਵਾਪਸ 0;

    ਜੇ (get_fileformat(bufp) == EOL_DOS)
	eol_ਮਾਪ = 2;
    ਨਹੀਂਤਾਂ
	eol_ਮਾਪ = 1;
    ਲਈ (lnum = 1; lnum <= bufp->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; ++lnum)
    {
	char_ਗਿਣਤੀ += ml_get_buf_len(bufp, lnum) + eol_ਮਾਪ;
	// Check for a CTRL-C every 100000 characters
	ਜੇ (char_ਗਿਣਤੀ > ਆਖਰੀ_check)
	{
	    ui_breakcheck();
	    ਜੇ (got_int)
		ਵਾਪਸ char_ਗਿਣਤੀ;
	    ਆਖਰੀ_check = char_ਗਿਣਤੀ + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    ਜੇ (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_ਗਿਣਤੀ -= eol_ਮਾਪ;

    ਵਾਪਸ char_ਗਿਣਤੀ;
}

/*
 * Convert character offset to lnum,col
 */
    ਸਥਿਰ pos_T *
off2pos(buf_T *buf, ਲੰਮਾ ਵਿੱਥ)
{
    linenr_T	 lnum;
    ਸਥਿਰ pos_T pos;

    pos.lnum = 0;
    pos.ਖਾਨਾ = 0;
    pos.coladd = 0;

    ਜੇ (!(buf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ))
    {
	ਜੇ ((lnum = ml_ਲੱਭੋ_ਸਤਰ_or_ਵਿੱਥ(buf, (linenr_T)0, &ਵਿੱਥ)) < 0)
	    ਵਾਪਸ ਨਿਰਾ;
	pos.lnum = lnum;
	pos.ਖਾਨਾ = ਵਿੱਥ;
    }

    ਵਾਪਸ &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    ਸਥਿਰ pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    ਸਥਿਰ pos_T	mypos;
    ਲੰਮਾ		off;

    off = strtol((ਅੱਖਰ *)*argp, (ਅੱਖਰ **)argp, 10);
    ਜੇ (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.ਖਾਨਾ = strtol((ਅੱਖਰ *)*argp, (ਅੱਖਰ **)argp, 10);
	mypos.coladd = 0;
	ਵਾਪਸ &mypos;
    }
    ਵਾਪਸ off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    ਸਥਿਰ ਲੰਮਾ
pos2off(buf_T *buf, pos_T *pos)
{
    ਲੰਮਾ	 ਵਿੱਥ = 0;

    ਜੇ (buf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
	ਵਾਪਸ 0;

    ਜੇ ((ਵਿੱਥ = ml_ਲੱਭੋ_ਸਤਰ_or_ਵਿੱਥ(buf, pos->lnum, 0)) < 0)
	ਵਾਪਸ 0;
    ਵਿੱਥ += pos->ਖਾਨਾ;

    ਵਾਪਸ ਵਿੱਥ;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    ਸਥਿਰ ਖਾਲੀ
print_read_ਸੁਨੇਹਾ(nbbuf_T *buf)
{
    ਪੂਰਨ	    lnum = buf->bufp->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    off_T   nchars = buf->bufp->b_orig_ਮਾਪ;
    char_u  c;

    ਸੁਨੇਹਾ_ਜੋੜੋ_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    ਜੇ (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    ਜੇ (!buf->bufp->b_ਸ਼ੁਰੂ_eol)
    {
	STRCAT(IObuff, shortmess(SHM_ਆਖਰੀ) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    ਸੁਨੇਹਾ_ਜੋੜੋ_ਸਤਰਾਂ(c, (ਲੰਮਾ)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_ਸੁਨੇਹਾ);
    ਸੁਨੇਹਾ_scrolled_ign = TRUE;
    ਸੁਨੇਹਾ_trunc_attr((ਅੱਖਰ *)IObuff, FALSE, 0);
    ਸੁਨੇਹਾ_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    ਸਥਿਰ ਖਾਲੀ
print_save_ਸੁਨੇਹਾ(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    ਜੇ (nchars >= 0)
    {
	// put fname in IObuff with quotes
	ਸੁਨੇਹਾ_ਜੋੜੋ_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	ਸੁਨੇਹਾ_ਜੋੜੋ_ਸਤਰਾਂ(c, buf->bufp->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
						buf->bufp->b_orig_ਮਾਪ);

	VIM_CLEAR(keep_ਸੁਨੇਹਾ);
	ਸੁਨੇਹਾ_scrolled_ign = TRUE;
	p = (char_u *)ਸੁਨੇਹਾ_trunc_attr((ਅੱਖਰ *)IObuff, FALSE, 0);
	ਜੇ ((ਸੁਨੇਹਾ_scrolled && !need_wait_return) || !buf->init_ਪੂਰਾ)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    ਸੈੱਟ_keep_ਸੁਨੇਹਾ(p, 0);
	}
	ਸੁਨੇਹਾ_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    ਨਹੀਂਤਾਂ
    {
	ਅੱਖਰ msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#ਜੇ_ਅੰਤ // defined(FEAT_NETBEANS_INTG)
