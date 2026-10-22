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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_NETBEANS_INTG)

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "version.h"

#ವ್ಯಾಖ್ಯಾನಿಸು GUARDED		10000 // typenr for "guarded" annotation
#ವ್ಯಾಖ್ಯಾನಿಸು GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_COLOR_ಉದ್ದಳತೆ 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
ಸ್ಥಿರ ಅಕ್ಷರ *Ext_Ed_Protocol_ಆವೃತ್ತಿ = "2.5";

ಸ್ಥಿರ ದೀರ್ಘ pos2off(buf_T *, pos_T *);
ಸ್ಥಿರ pos_T *off2pos(buf_T *, ದೀರ್ಘ);
ಸ್ಥಿರ pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
ಸ್ಥಿರ ದೀರ್ಘ get_buf_ಗಾತ್ರ(buf_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ netbeans_keystring(char_u *keystr);
ಸ್ಥಿರ ಶೂನ್ಯ special_ಕೀಲಿಗಳು(char_u *ವಾದಗಳು);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ getConnInfo(ಅಕ್ಷರ *file, ಅಕ್ಷರ **host, ಅಕ್ಷರ **ಪೋರ್ಟ್, ಅಕ್ಷರ **password);

ಸ್ಥಿರ ಶೂನ್ಯ nb_init_graphics(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ coloncmd(ಅಕ್ಷರ *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ಸ್ಥಿರ ಶೂನ್ಯ nb_ಗಣ_curbuf(buf_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ nb_parse_cmd(char_u *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  nb_do_cmd(ಪೂರ್ಣಾಂಕ, char_u *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, char_u *);
ಸ್ಥಿರ ಶೂನ್ಯ nb_send(ಅಕ್ಷರ *buf, ಅಕ್ಷರ *fun);
ಸ್ಥಿರ ಶೂನ್ಯ nb_free(ಶೂನ್ಯ);

#ವ್ಯಾಖ್ಯಾನಿಸು NETBEANS_OPEN (channel_can_write_ಗಮ್ಯ(nb_channel))
ಸ್ಥಿರ channel_T *nb_channel = ಖಾಲಿ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ r_cmdno;			// current command number for reply
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NBDEBUG
# include "nbdebug.c"
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ needupdate = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಒಳ_Atomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nb_is_ಸುರಕ್ಷಿತ_string(char_u *str, char_u *allowed)
{
    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಗಾಗಿ (char_u *p = str; *p != NUL; p++)
	ಆದರೆ (vim_strchr(allowed, *p) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nb_channel_closed(ಶೂನ್ಯ)
{
    nb_channel = ಖಾಲಿ;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
netbeans_close(ಶೂನ್ಯ)
{
    ಆದರೆ (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	ಆದರೆ (nb_channel != ಖಾಲಿ)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = ಖಾಲಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
    bevalServers &= ~BEVAL_NETBEANS;
#ಆದರೆ_ಅಂತ್ಯ

    needupdate = 0;
    ಒಳ_Atomic = 0;
    nb_free();

    // remove all signs and update the screen after gutter removal
    coloncmd(":sign unplace *");
    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);
}

#ವ್ಯಾಖ್ಯಾನಿಸು NB_DEF_HOST "localhost"
#ವ್ಯಾಖ್ಯಾನಿಸು NB_DEF_ADDR "3219"
#ವ್ಯಾಖ್ಯಾನಿಸು NB_DEF_PASS "changeme"

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
netbeans_connect(ಅಕ್ಷರ *params, ಪೂರ್ಣಾಂಕ doabort)
{
    ಪೂರ್ಣಾಂಕ		ಪೋರ್ಟ್;
    ಅಕ್ಷರ	buf[32];
    ಅಕ್ಷರ	*hostname = ಖಾಲಿ;
    ಅಕ್ಷರ	*ವಿಳಾಸ = ಖಾಲಿ;
    ಅಕ್ಷರ	*password = ಖಾಲಿ;
    ಅಕ್ಷರ	*fname;
    ಅಕ್ಷರ	*arg = ಖಾಲಿ;

    ಆದರೆ (*params == '=')
    {
	// "=fname": Read info from specified file.
	ಆದರೆ (getConnInfo(params + 1, &hostname, &ವಿಳಾಸ, &password) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	ಆದರೆ (arg == ಖಾಲಿ && (fname = getenv("__NETBEANS_CONINFO")) != ಖಾಲಿ)
	{
	    // "": get info from file specified in environment
	    ಆದರೆ (getConnInfo(fname, &hostname, &ವಿಳಾಸ, &password) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (arg != ಖಾಲಿ)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		ವಿಳಾಸ = strchr(hostname, ':');
		ಆದರೆ (ವಿಳಾಸ != ಖಾಲಿ)
		{
		    *ವಿಳಾಸ++ = '\0';
		    password = strchr(ವಿಳಾಸ, ':');
		    ಆದರೆ (password != ಖಾಲಿ)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    ಆದರೆ (hostname == ಖಾಲಿ || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    ಆದರೆ (ವಿಳಾಸ == ಖಾಲಿ)
		ವಿಳಾಸ = getenv("__NETBEANS_SOCKET");
	    ಆದರೆ (password == ಖಾಲಿ)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    ಆದರೆ (hostname != ಖಾಲಿ)
		hostname = (ಅಕ್ಷರ *)vim_strsave((char_u *)hostname);
	    ಆದರೆ (ವಿಳಾಸ != ಖಾಲಿ)
		ವಿಳಾಸ = (ಅಕ್ಷರ *)vim_strsave((char_u *)ವಿಳಾಸ);
	    ಆದರೆ (password != ಖಾಲಿ)
		password = (ಅಕ್ಷರ *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    ಆದರೆ (hostname == ಖಾಲಿ || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (ಅಕ್ಷರ *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    ಆದರೆ (ವಿಳಾಸ == ಖಾಲಿ || *ವಿಳಾಸ == '\0')
    {
	vim_free(ವಿಳಾಸ);
	ವಿಳಾಸ = (ಅಕ್ಷರ *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    ಆದರೆ (password == ಖಾಲಿ || *password == '\0')
    {
	vim_free(password);
	password = (ಅಕ್ಷರ *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    ಆದರೆ (hostname != ಖಾಲಿ && ವಿಳಾಸ != ಖಾಲಿ && password != ಖಾಲಿ)
    {
	ಪೋರ್ಟ್ = atoi(ವಿಳಾಸ);
	nb_channel = channel_open(hostname, ಪೋರ್ಟ್, 3000, nb_channel_closed);
	ಆದರೆ (nb_channel != ಖಾಲಿ)
	{
	    // success
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#ಆದರೆ_ಅಂತ್ಯ

	    // success, login
	    vim_snprintf(buf, ಗಾತ್ರ(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", Ext_Ed_Protocol_ಆವೃತ್ತಿ);
	    nb_send(buf, "externaleditor_version");
	}
    }

    ಆದರೆ (nb_channel == ಖಾಲಿ && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(ವಿಳಾಸ);
    vim_free(password);
    ಹಿಂತಿರುಗಿ NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
getConnInfo(ಅಕ್ಷರ *file, ಅಕ್ಷರ **host, ಅಕ್ಷರ **ಪೋರ್ಟ್, ಅಕ್ಷರ **auth)
{
    FILE *fp;
    char_u buf[BUFSIZ];
    char_u *lp;
    char_u *nlp;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    stat_T	st;

    /*
     * For Unix only accept the file when it's not accessible by others.
     * The open will then fail if we don't own the file.
     */
    ಆದರೆ (mch_stat(file, &st) == 0 && (st.st_ಬಹುಲಕ & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_ಬಹುಲಕ_for_netbeans_connection_info_file_str),
									file);
	ಹಿಂತಿರುಗಿ FAIL;
    }
#ಆದರೆ_ಅಂತ್ಯ

    fp = mch_fopen(file, "r");
    ಆದರೆ (fp == ಖಾಲಿ)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Read the file. There should be one of each parameter
    ಆಗ ((lp = (char_u *)fgets((ಅಕ್ಷರ *)buf, BUFSIZ, fp)) != ಖಾಲಿ)
    {
	ಆದರೆ ((nlp = vim_strchr(lp, '\n')) != ಖಾಲಿ)
	    *nlp = 0;	    // strip off the trailing newline

	ಆದರೆ (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (ಅಕ್ಷರ *)vim_strsave(&buf[5]);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*ಪೋರ್ಟ್);
	    *ಪೋರ್ಟ್ = (ಅಕ್ಷರ *)vim_strsave(&buf[5]);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (ಅಕ್ಷರ *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    ಹಿಂತಿರುಗಿ OK;
}


ರಚನೆ keyqueue
{
    char_u	    *keystr;
    ರಚನೆ keyqueue *ಮುಂದಿನ;
    ರಚನೆ keyqueue *prev;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ keyqueue ಕೀಲಿ_Q_T;

ಸ್ಥಿರ ಕೀಲಿ_Q_T ಕೀಲಿ_ತಲೆ; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
postpone_keycommand(char_u *keystr)
{
    ಕೀಲಿ_Q_T *ಗಂಟು;

    ಗಂಟು = ALLOC_ONE(ಕೀಲಿ_Q_T);
    ಆದರೆ (ಗಂಟು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // out of memory, drop the key

    ಆದರೆ (ಕೀಲಿ_ತಲೆ.ಮುಂದಿನ == ಖಾಲಿ) // initialize circular queue
    {
	ಕೀಲಿ_ತಲೆ.ಮುಂದಿನ = &ಕೀಲಿ_ತಲೆ;
	ಕೀಲಿ_ತಲೆ.prev = &ಕೀಲಿ_ತಲೆ;
    }

    // insert node at tail of queue
    ಗಂಟು->ಮುಂದಿನ = &ಕೀಲಿ_ತಲೆ;
    ಗಂಟು->prev = ಕೀಲಿ_ತಲೆ.prev;
    ಕೀಲಿ_ತಲೆ.prev->ಮುಂದಿನ = ಗಂಟು;
    ಕೀಲಿ_ತಲೆ.prev = ಗಂಟು;

    ಗಂಟು->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_ಕೀಲಿ_ಸರತಿ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ postponed = FALSE;

    ಆಗ (!postponed && ಕೀಲಿ_ತಲೆ.ಮುಂದಿನ && ಕೀಲಿ_ತಲೆ.ಮುಂದಿನ != &ಕೀಲಿ_ತಲೆ)
    {
	// first, unlink the node
	ಕೀಲಿ_Q_T *ಗಂಟು = ಕೀಲಿ_ತಲೆ.ಮುಂದಿನ;
	ಕೀಲಿ_ತಲೆ.ಮುಂದಿನ = ಗಂಟು->ಮುಂದಿನ;
	ಗಂಟು->ಮುಂದಿನ->prev = ಗಂಟು->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	ಆದರೆ (ಗಂಟು->keystr != ಖಾಲಿ)
	    postponed = !netbeans_keystring(ಗಂಟು->keystr);
	vim_free(ಗಂಟು->keystr);

	// Finally, dispose of the node
	vim_free(ಗಂಟು);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    ಶೂನ್ಯ
netbeans_parse_messages(ಶೂನ್ಯ)
{
    readq_T	*ಗಂಟು;
    char_u	*ಬಫರ್;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		own_ಗಂಟು;

    ಆಗ (nb_channel != ಖಾಲಿ)
    {
	ಗಂಟು = channel_peek(nb_channel, PART_SOCK);
	ಆದರೆ (ಗಂಟು == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_ಮೊದಲ_nl(ಗಂಟು);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    ಆದರೆ (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		ಹಿಂತಿರುಗಿ;
	    ಮುಂದುವರಿ;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	ಆದರೆ (*p == NUL)
	{
	    own_ಗಂಟು = TRUE;
	    ಬಫರ್ = channel_get(nb_channel, PART_SOCK, ಖಾಲಿ);
	    // "node" is now invalid!
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    own_ಗಂಟು = FALSE;
	    ಬಫರ್ = ಗಂಟು->rq_ಬಫರ್;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(ಬಫರ್);

	ಆದರೆ (own_ಗಂಟು)
	    // buffer finished, dispose of it
	    vim_free(ಬಫರ್);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nb_channel != ಖಾಲಿ)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (ಪೂರ್ಣಾಂಕ)(p - ಬಫರ್));
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
    ಸ್ಥಿರ ಶೂನ್ಯ
nb_parse_cmd(char_u *cmd)
{
    ಅಕ್ಷರ	*verb;
    ಅಕ್ಷರ	*q;
    ಪೂರ್ಣಾಂಕ		bufno;
    ಪೂರ್ಣಾಂಕ		isfunc = -1;

    ಆದರೆ (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    ಆದರೆ (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_ಸ್ತಂಭ = false;

	// The IDE is breaking the connection.
	netbeans_close();
	ಹಿಂತಿರುಗಿ;
    }

    bufno = strtol((ಅಕ್ಷರ *)cmd, &verb, 10);

    ಆದರೆ (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	ಹಿಂತಿರುಗಿ;
    }
    ++verb; // skip colon

    ಗಾಗಿ (q = verb; *q; q++)
    {
	ಆದರೆ (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_ಒಳ_str), cmd);
	ಹಿಂತಿರುಗಿ;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (ಅಕ್ಷರ *)skipwhite((char_u *)q);

    ಆದರೆ (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NBDEBUG
	/*
	 * This happens because the ExtEd can send a command or 2 after
	 * doing a stopDocumentListen command. It doesn't harm anything
	 * so I'm disabling it except for debugging.
	 */
	nbdebug(("nb_parse_cmd: Command error for \"%s\"\n", cmd));
	emsg(e_bad_return_from_nb_do_cmd);
#ಆದರೆ_ಅಂತ್ಯ
    }
}

ರಚನೆ nbbuf_struct
{
    buf_T		*bufp;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	 fireChanges:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	 init_ಮುಗಿದ:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	 ಸೇರಿಸು_ಮುಗಿದ:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	 modified:1;
    ಪೂರ್ಣಾಂಕ			 nbbuf_ಸಂಖ್ಯೆ;
    ಅಕ್ಷರ		*displayname;
    ಪೂರ್ಣಾಂಕ			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ nbbuf_struct nbbuf_T;

ಸ್ಥಿರ nbbuf_T *buf_list = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ buf_list_ಗಾತ್ರ = 0;	// size of buf_list
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ buf_list_ಬಳಸಿದ = 0;	// nr of entries in buf_list actually in use

ಸ್ಥಿರ ಅಕ್ಷರ **globalsignmap = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ globalsignmaplen = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ globalsignmapused = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  mapsigntype(nbbuf_T *, ಪೂರ್ಣಾಂಕ localsigntype);
ಸ್ಥಿರ ಶೂನ್ಯ addsigntype(nbbuf_T *, ಪೂರ್ಣಾಂಕ localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
ಸ್ಥಿರ ಶೂನ್ಯ print_read_ಸಂದೇಶ(nbbuf_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ print_save_ಸಂದೇಶ(nbbuf_T *buf, off_T nchars);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ curPCtype = -1;

/*
 * Free netbeans resources.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nb_free(ಶೂನ್ಯ)
{
    ಕೀಲಿ_Q_T *ಕೀಲಿ_ಗಂಟು = ಕೀಲಿ_ತಲೆ.ಮುಂದಿನ;
    nbbuf_T buf;
    ಪೂರ್ಣಾಂಕ i;

    // free the netbeans buffer list
    ಗಾಗಿ (i = 0; i < buf_list_ಬಳಸಿದ; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	ಆದರೆ (buf.bufp != ಖಾಲಿ && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_ಗಾತ್ರ = 0;
    buf_list_ಬಳಸಿದ = 0;

    // free the queued key commands
    ಆಗ (ಕೀಲಿ_ಗಂಟು != ಖಾಲಿ && ಕೀಲಿ_ಗಂಟು != &ಕೀಲಿ_ತಲೆ)
    {
	ಕೀಲಿ_Q_T *ಮುಂದಿನ = ಕೀಲಿ_ಗಂಟು->ಮುಂದಿನ;
	vim_free(ಕೀಲಿ_ಗಂಟು->keystr);
	vim_free(ಕೀಲಿ_ಗಂಟು);
	ಆದರೆ (ಮುಂದಿನ == &ಕೀಲಿ_ತಲೆ)
	{
	    ಕೀಲಿ_ತಲೆ.ಮುಂದಿನ = &ಕೀಲಿ_ತಲೆ;
	    ಕೀಲಿ_ತಲೆ.prev = &ಕೀಲಿ_ತಲೆ;
	    ನಿಲ್ಲಿಸು;
	}
	ಕೀಲಿ_ಗಂಟು = ಮುಂದಿನ;
    }

    // free the queued netbeans commands
    ಆದರೆ (nb_channel != ಖಾಲಿ)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nb_getbufno(buf_T *bufp)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < buf_list_ಬಳಸಿದ; i++)
	ಆದರೆ (buf_list[i].bufp == bufp)
	    ಹಿಂತಿರುಗಿ i;
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    ಪೂರ್ಣಾಂಕ
is_Netbeans_ಬಫರ್(buf_T *bufp)
{
    ಹಿಂತಿರುಗಿ NETBEANS_OPEN && bufp->b_netbeans_file;
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
    ಪೂರ್ಣಾಂಕ
isNetbeansModified(buf_T *bufp)
{
    ಆದರೆ (is_Netbeans_ಬಫರ್(bufp))
    {
	ಪೂರ್ಣಾಂಕ bufno = nb_getbufno(bufp);

	ಆದರೆ (bufno > 0)
	    ಹಿಂತಿರುಗಿ buf_list[bufno].modified;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    ಸ್ಥಿರ nbbuf_T *
nb_get_buf(ಪೂರ್ಣಾಂಕ bufno)
{
    // find or create a buffer with the given number
    ಪೂರ್ಣಾಂಕ incr;

    ಆದರೆ (bufno <= 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * ಗಾತ್ರ(nbbuf_T));
	buf_list_ಗಾತ್ರ = 100;
    }
    ಆದರೆ (bufno >= buf_list_ಬಳಸಿದ) // new
    {
	ಆದರೆ (bufno >= buf_list_ಗಾತ್ರ) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_ಗಾತ್ರ + 90;
	    buf_list_ಗಾತ್ರ += incr;
	    bufsize = buf_list_ಗಾತ್ರ * ಗಾತ್ರ(nbbuf_T);
	    ಆದರೆ (bufsize == 0 || bufsize / ಗಾತ್ರ(nbbuf_T)
						      != (size_t)buf_list_ಗಾತ್ರ)
	    {
		// list size overflow, bail out
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    ಆದರೆ (buf_list == ಖಾಲಿ)
	    {
		vim_free(t_buf_list);
		buf_list_ಗಾತ್ರ = 0;
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    vim_memset(buf_list + buf_list_ಗಾತ್ರ - incr, 0,
						      incr * ಗಾತ್ರ(nbbuf_T));
	}

	ಆಗ (buf_list_ಬಳಸಿದ <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_ಬಳಸಿದ].fireChanges = 1;
	    ++buf_list_ಬಳಸಿದ;
	}
    }

    ಹಿಂತಿರುಗಿ buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಎಣಿಕೆ_ಬದಲಾಯಿತು_buffers(ಶೂನ್ಯ)
{
    buf_T	*bufp;
    ಪೂರ್ಣಾಂಕ		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	ಆದರೆ (bufp->b_ಬದಲಾಯಿತು)
	    ++n;
    ಹಿಂತಿರುಗಿ n;
}

/*
 * End the netbeans session.
 */
    ಶೂನ್ಯ
netbeans_ಕೊನೆ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;
    ಸ್ಥಿರ ಅಕ್ಷರ buf[128];

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < buf_list_ಬಳಸಿದ; i++)
    {
	ಆದರೆ (!buf_list[i].bufp)
	    ಮುಂದುವರಿ;
	ಆದರೆ (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, ಖಾಲಿ);
	buf_list[i].bufp = ಖಾಲಿ;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nb_send(ಅಕ್ಷರ *buf, ಅಕ್ಷರ *fun)
{
    ಆದರೆ (nb_channel != ಖಾಲಿ)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (ಪೂರ್ಣಾಂಕ)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nb_ಉತ್ತರ_nil(ಪೂರ್ಣಾಂಕ cmdno)
{
    ಅಕ್ಷರ ಉತ್ತರ[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    sprintf(ಉತ್ತರ, "%d\n", cmdno);
    nb_send(ಉತ್ತರ, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nb_ಉತ್ತರ_ಪಠ್ಯ(ಪೂರ್ಣಾಂಕ cmdno, char_u *ಫಲಿತಾಂಶ)
{
    char_u *ಉತ್ತರ;

    nbdebug(("REP %d: %s\n", cmdno, (ಅಕ್ಷರ *)ಫಲಿತಾಂಶ));

    ಉತ್ತರ = alloc(STRLEN(ಫಲಿತಾಂಶ) + 32);
    sprintf((ಅಕ್ಷರ *)ಉತ್ತರ, "%d %s\n", cmdno, (ಅಕ್ಷರ *)ಫಲಿತಾಂಶ);
    nb_send((ಅಕ್ಷರ *)ಉತ್ತರ, "nb_reply_text");

    vim_free(ಉತ್ತರ);
}


/*
 * Send a response with a number result code.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nb_ಉತ್ತರ_nr(ಪೂರ್ಣಾಂಕ cmdno, ದೀರ್ಘ ಫಲಿತಾಂಶ)
{
    ಅಕ್ಷರ ಉತ್ತರ[32];

    nbdebug(("REP %d: %ld\n", cmdno, ಫಲಿತಾಂಶ));

    sprintf(ಉತ್ತರ, "%d %ld\n", cmdno, ಫಲಿತಾಂಶ);
    nb_send(ಉತ್ತರ, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    ಸ್ಥಿರ char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಗಾಗಿ (; *p; p++)
    {
	ಆಯ್ಕೆ (*p)
	{
	    ಸ್ಥಿತಿ '\"':
	    ಸ್ಥಿತಿ '\\':
		*q++ = '\\'; *q++ = *p; ನಿಲ್ಲಿಸು;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    ಸ್ಥಿತಿ '\n':
		*q++ = '\\'; *q++ = 'n'; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '\r':
		*q++ = '\\'; *q++ = 'r'; ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		*q++ = *p;
		ನಿಲ್ಲಿಸು;
	}
    }
    *q = '\0';

    ಹಿಂತಿರುಗಿ buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
nb_unquote(char_u *p, char_u **endp)
{
    ಅಕ್ಷರ *ಫಲಿತಾಂಶ = 0;
    ಅಕ್ಷರ *q;
    ಪೂರ್ಣಾಂಕ ಮುಗಿದ = 0;

    // result is never longer than input
    ಫಲಿತಾಂಶ = alloc_clear(STRLEN(p) + 1);
    ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	ಫಲಿತಾಂಶ[0] = NUL;
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಗಾಗಿ (q = ಫಲಿತಾಂಶ; !ಮುಗಿದ && *p != NUL;)
    {
	ಆಯ್ಕೆ (*p)
	{
	    ಸ್ಥಿತಿ '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		ಮುಗಿದ = 1;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ '\\':
		++p;
		ಆಯ್ಕೆ (*p)
		{
		    ಸ್ಥಿತಿ '\\':	*q++ = '\\';	ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'n':	*q++ = '\n';	ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 't':	*q++ = '\t';	ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'r':	*q++ = '\r';	ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '"':	*q++ = '"';	ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ NUL:	--p;		ನಿಲ್ಲಿಸು;
		    // default: skip over illegal chars
		}
		++p;
		ನಿಲ್ಲಿಸು;

	    ಪೂರ್ವನಿಯೋಜಿತ:
		*q++ = *p++;
	}
    }

    ಆದರೆ (endp != ಖಾಲಿ)
	*endp = p;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nb_partialremove(linenr_T lnum, colnr_T ಮೊದಲ, colnr_T ಕೊನೆಯ)
{
    char_u *oldtext, *newtext;
    ಪೂರ್ಣಾಂಕ oldlen;
    ಪೂರ್ಣಾಂಕ lastbyte = ಕೊನೆಯ;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    ಆದರೆ (ಮೊದಲ >= (colnr_T)oldlen || oldlen == 0)  // just in case
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - ಮೊದಲ));
    ಆದರೆ (newtext == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    mch_memmove(newtext, oldtext, ಮೊದಲ);
    STRMOVE(newtext + ಮೊದಲ, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nb_joinlines(linenr_T ಮೊದಲ, linenr_T other)
{
    ಪೂರ್ಣಾಂಕ len_ಮೊದಲ, len_other;
    char_u *p;

    len_ಮೊದಲ = ml_get_len(ಮೊದಲ);
    len_other = ml_get_len(other);
    p = alloc(len_ಮೊದಲ + len_other + 1);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    mch_memmove(p, ml_get(ಮೊದಲ), len_ಮೊದಲ);
    mch_memmove(p + len_ಮೊದಲ, ml_get(other), len_other + 1);
    ml_replace(ಮೊದಲ, p, FALSE);
}

#ವ್ಯಾಖ್ಯಾನಿಸು SKIP_STOP 2
#ವ್ಯಾಖ್ಯಾನಿಸು streq(a,b) (strcmp(a,b) == 0)

/*
 * Do the actual processing of a single netbeans command or function.
 * The difference between a command and function is that a function
 * gets a response (it's required) but a command does not.
 * For arguments see comment for nb_parse_cmd().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nb_do_cmd(
    ಪೂರ್ಣಾಂಕ		bufno,
    char_u	*cmd,
    ಪೂರ್ಣಾಂಕ		func,
    ಪೂರ್ಣಾಂಕ		cmdno,
    char_u	*ವಾದಗಳು)	    // points to space before arguments or NUL
{
    ಪೂರ್ಣಾಂಕ		do_update = 0;
    ದೀರ್ಘ	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	skip = 0;
    ಪೂರ್ಣಾಂಕ		retval = OK;
    ಅಕ್ಷರ	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (ಅಕ್ಷರ *)ವಾದಗಳು));

    ಆದರೆ (func)
    {
// =====================================================================
	ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "getModified"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
		// Return the number of buffers that are modified.
		nb_ಉತ್ತರ_nr(cmdno, (ದೀರ್ಘ)ಎಣಿಕೆ_ಬದಲಾಯಿತು_buffers());
	    ಇಲ್ಲದಿದ್ದರೆ
		// Return whether the buffer is modified.
		nb_ಉತ್ತರ_nr(cmdno, (ದೀರ್ಘ)(buf->bufp->b_ಬದಲಾಯಿತು
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_ಉತ್ತರ_nr(cmdno, (ದೀರ್ಘ)ಎಣಿಕೆ_ಬದಲಾಯಿತು_buffers());
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "getCursor"))
	{
	    char_u ಪಠ್ಯ[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((ಅಕ್ಷರ *)ಪಠ್ಯ, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (ದೀರ್ಘ)curwin->w_cursor.lnum,
		    (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_ಉತ್ತರ_ಪಠ್ಯ(cmdno, ಪಠ್ಯ);
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "getAnno"))
	{
	    ದೀರ್ಘ linenum = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_getanno));
		retval = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ ser_ಅಂಕೆ;

		cp = (ಅಕ್ಷರ *)ವಾದಗಳು;
		ser_ಅಂಕೆ = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (ದೀರ್ಘ)buf_findsign(buf->bufp, ser_ಅಂಕೆ, ಖಾಲಿ);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    nb_ಉತ್ತರ_nr(cmdno, linenum);
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "getLength"))
	{
	    ದೀರ್ಘ len = 0;

	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_getlength));
		retval = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		len = get_buf_ಗಾತ್ರ(buf->bufp);
	    }
	    nb_ಉತ್ತರ_nr(cmdno, len);
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "getText"))
	{
	    ದೀರ್ಘ	len;
	    linenr_T	nlines;
	    char_u	*ಪಠ್ಯ = ಖಾಲಿ;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*ಸಾಲು;

	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_gettext));
		retval = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		len = get_buf_ಗಾತ್ರ(buf->bufp);
		nlines = buf->bufp->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		ಪಠ್ಯ = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    p = ಪಠ್ಯ;
		    *p++ = '\"';
		    ಗಾಗಿ (; lno <= nlines ; lno++)
		    {
			ಸಾಲು = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			ಆದರೆ (ಸಾಲು != ಖಾಲಿ)
			{
			    STRCPY(p, ಸಾಲು);
			    p += STRLEN(ಸಾಲು);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(ಸಾಲು);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ)
		nb_ಉತ್ತರ_ಪಠ್ಯ(cmdno, (char_u *)"");
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		nb_ಉತ್ತರ_ಪಠ್ಯ(cmdno, ಪಠ್ಯ);
		vim_free(ಪಠ್ಯ);
	    }
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "remove"))
	{
	    ದೀರ್ಘ ಎಣಿಕೆ_2;
	    pos_T ಮೊದಲ, ಕೊನೆಯ;
	    pos_T *pos;
	    pos_T *ಮುಂದಿನ;
	    linenr_T del_from_lnum, del_ಗಮ್ಯ_lnum;  // lines to be deleted as a whole
	    ಪೂರ್ಣಾಂಕ oldFire = netbeansFireChanges;
	    ಪೂರ್ಣಾಂಕ oldSuppress = netbeans_Suppress_No_ಸಾಲುಗಳು;
	    ಪೂರ್ಣಾಂಕ was_ಬದಲಾಯಿತು_2;

	    ಆದರೆ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (ಅಕ್ಷರ *) cmd));
		nb_ಉತ್ತರ_nil(cmdno);
		ಹಿಂತಿರುಗಿ OK;
	    }

	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_remove));
		retval = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_ಸಾಲುಗಳು = TRUE;

		nb_ಗಣ_curbuf(buf->bufp);
		was_ಬದಲಾಯಿತು_2 = buf->bufp->b_ಬದಲಾಯಿತು;
		cp = (ಅಕ್ಷರ *)ವಾದಗಳು;
		off = strtol(cp, &cp, 10);
		ಎಣಿಕೆ_2 = strtol(cp, &cp, 10);
		ವಾದಗಳು = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		ಆದರೆ (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_ಉತ್ತರ_ಪಠ್ಯ(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_ಸಾಲುಗಳು = oldSuppress;
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಮೊದಲ = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      ಮೊದಲ.lnum, ಮೊದಲ.ಲಂಬಸಾಲು));
		pos = off2pos(buf->bufp, off+ಎಣಿಕೆ_2-1);
		ಆದರೆ (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_ಉತ್ತರ_ಪಠ್ಯ(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_ಸಾಲುಗಳು = oldSuppress;
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಕೊನೆಯ = *pos;
		nbdebug(("    LAST POS: line %ld, col %d\n",
							ಕೊನೆಯ.lnum, ಕೊನೆಯ.ಲಂಬಸಾಲು));
		del_from_lnum = ಮೊದಲ.lnum;
		del_ಗಮ್ಯ_lnum = ಕೊನೆಯ.lnum;
		do_update = 1;

		// Get the position of the first byte after the deleted
		// section.  "next" is NULL when deleting to the end of the
		// file.
		ಮುಂದಿನ = off2pos(buf->bufp, off + ಎಣಿಕೆ_2);

		// Remove part of the first line.
		ಆದರೆ (ಮೊದಲ.ಲಂಬಸಾಲು != 0
				|| (ಮುಂದಿನ != ಖಾಲಿ && ಮೊದಲ.lnum == ಮುಂದಿನ->lnum))
		{
		    ಆದರೆ (ಮೊದಲ.lnum != ಕೊನೆಯ.lnum
			    || (ಮುಂದಿನ != ಖಾಲಿ && ಮೊದಲ.lnum != ಮುಂದಿನ->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(ಮೊದಲ.lnum, ಮೊದಲ.ಲಂಬಸಾಲು,
							     (colnr_T)MAXCOL);
			ಆದರೆ (ಮೊದಲ.lnum == ಕೊನೆಯ.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(ಮೊದಲ.lnum, ಮುಂದಿನ->lnum);
			    del_ಗಮ್ಯ_lnum = ಮುಂದಿನ->lnum;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// remove within one line
			nb_partialremove(ಮೊದಲ.lnum, ಮೊದಲ.ಲಂಬಸಾಲು, ಕೊನೆಯ.ಲಂಬಸಾಲು);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		ಆದರೆ (ಮೊದಲ.lnum != ಕೊನೆಯ.lnum && ಮುಂದಿನ != ಖಾಲಿ
			&& ಮುಂದಿನ->ಲಂಬಸಾಲು != 0 && ಕೊನೆಯ.lnum == ಮುಂದಿನ->lnum)
		{
		    nb_partialremove(ಕೊನೆಯ.lnum, 0, ಕೊನೆಯ.ಲಂಬಸಾಲು);
		    ಆದರೆ (del_from_lnum > ಮೊದಲ.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(ಮೊದಲ.lnum, ಕೊನೆಯ.lnum);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			// First line is deleted as a whole, keep the last
			// line.
			--del_ಗಮ್ಯ_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		ಆದರೆ (ಮೊದಲ.lnum != ಕೊನೆಯ.lnum && del_from_lnum > ಮೊದಲ.lnum
			&& ಮುಂದಿನ != ಖಾಲಿ && ಕೊನೆಯ.lnum != ಮುಂದಿನ->lnum)
		{
		    nb_joinlines(ಮೊದಲ.lnum, ಮುಂದಿನ->lnum);
		    del_ಗಮ್ಯ_lnum = ಮುಂದಿನ->lnum;
		}

		// Delete whole lines if there are any.
		ಆದರೆ (del_ಗಮ್ಯ_lnum >= del_from_lnum)
		{
		    ಪೂರ್ಣಾಂಕ i;

		    // delete signs from the lines being deleted
		    ಗಾಗಿ (i = del_from_lnum; i <= del_ಗಮ್ಯ_lnum; i++)
		    {
			ಪೂರ್ಣಾಂಕ id = buf_findsign_id(buf->bufp, (linenr_T)i, ಖಾಲಿ);
			ಆದರೆ (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, ಖಾಲಿ);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_ಗಮ್ಯ_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
		    del_ಸಾಲುಗಳು(del_ಗಮ್ಯ_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = ಮೊದಲ;
		check_cursor_lnum();
		buf->bufp->b_ಬದಲಾಯಿತು = was_ಬದಲಾಯಿತು_2; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_ಸಾಲುಗಳು = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_ಉತ್ತರ_nil(cmdno);
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "insert"))
	{
	    char_u	*ಗಮ್ಯ_free;

	    ಆದರೆ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (ಅಕ್ಷರ *) cmd));
		nb_ಉತ್ತರ_nil(cmdno);
		ಹಿಂತಿರುಗಿ OK;
	    }

	    // get offset
	    cp = (ಅಕ್ಷರ *)ವಾದಗಳು;
	    off = strtol(cp, &cp, 10);
	    ವಾದಗಳು = (char_u *)cp;

	    // get text to be inserted
	    ವಾದಗಳು = skipwhite(ವಾದಗಳು);
	    ವಾದಗಳು = ಗಮ್ಯ_free = (char_u *)nb_unquote(ವಾದಗಳು, ಖಾಲಿ);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_ಸೇರಿಸು));
		retval = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಾದಗಳು != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ	ff_detected = EOL_UNKNOWN;
		ಪೂರ್ಣಾಂಕ	buf_was_ಖಾಲಿಗಳು = (buf->bufp->b_ml.ml_flags & ML_ಖಾಲಿಗಳು);
		size_t	len = 0;
		ಪೂರ್ಣಾಂಕ	added = 0;
		ಪೂರ್ಣಾಂಕ	oldFire = netbeansFireChanges;
		ಪೂರ್ಣಾಂಕ	old_b_ಬದಲಾಯಿತು;
		char_u	*nlp;
		linenr_T lnum;
		linenr_T lnum_ಆರಂಭ;
		pos_T	*pos;

		netbeansFireChanges = 0;

		// Jump to the buffer where we insert.  After this "curbuf"
		// can be used.
		nb_ಗಣ_curbuf(buf->bufp);
		old_b_ಬದಲಾಯಿತು = curbuf->b_ಬದಲಾಯಿತು;

		// Convert the specified character offset into a lnum/col
		// position.
		pos = off2pos(curbuf, off);
		ಆದರೆ (pos != ಖಾಲಿ)
		{
		    ಆದರೆ (pos->lnum <= 0)
			lnum_ಆರಂಭ = 1;
		    ಇಲ್ಲದಿದ್ದರೆ
			lnum_ಆರಂಭ = pos->lnum;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    ಆದರೆ (buf_was_ಖಾಲಿಗಳು)
			lnum_ಆರಂಭ = 1;	    // above empty line
		    ಇಲ್ಲದಿದ್ದರೆ
			lnum_ಆರಂಭ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_ಆರಂಭ;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		ಆಗ (*ವಾದಗಳು != NUL)
		{
		    nlp = vim_strchr(ವಾದಗಳು, '\n');
		    ಆದರೆ (nlp == ಖಾಲಿ)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(ವಾದಗಳು);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			len = nlp - ವಾದಗಳು;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			ಆದರೆ (nlp > ವಾದಗಳು && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    ff_detected = EOL_UNIX;
		    }
		    ವಾದಗಳು[len] = NUL;

		    ಆದರೆ (lnum == lnum_ಆರಂಭ
			    && ((pos != ಖಾಲಿ && pos->ಲಂಬಸಾಲು > 0)
				|| (lnum == 1 && buf_was_ಖಾಲಿಗಳು)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು = pos == ಖಾಲಿ ? 0 : pos->ಲಂಬಸಾಲು;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			ಆದರೆ (newline != ಖಾಲಿ)
			{
			    mch_memmove(newline, oldline, (size_t)ಲಂಬಸಾಲು);
			    newline[ಲಂಬಸಾಲು] = NUL;
			    STRCAT(newline, ವಾದಗಳು);
			    STRCAT(newline, oldline + ಲಂಬಸಾಲು);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), ವಾದಗಳು,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    ಆದರೆ (nlp == ಖಾಲಿ)
			ನಿಲ್ಲಿಸು;
		    ++lnum;
		    ವಾದಗಳು = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_ಸಾಲುಗಳು_mark(lnum_ಆರಂಭ - 1, (ದೀರ್ಘ)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		ಆದರೆ (buf_was_ಖಾಲಿಗಳು)
		{
		    ಆದರೆ (ff_detected == EOL_UNKNOWN)
#ಆದರೆ defined(MSWIN)
			ff_detected = EOL_DOS;
#ಇಲ್ಲದಿದ್ದರೆ
			ff_detected = EOL_UNIX;
#ಆದರೆ_ಅಂತ್ಯ
		    ಗಣ_fileformat(ff_detected, OPT_LOCAL);
		    curbuf->b_ಆರಂಭ_ffc = *curbuf->b_p_ff;
		}

		/*
		 * XXX - GRP - Is the next line right? If I've inserted
		 * text the buffer has been updated but not written. Will
		 * netbeans guarantee to write it? Even if I do a :q! ?
		 */
		curbuf->b_ಬದಲಾಯಿತು = old_b_ಬದಲಾಯಿತು; // logically unchanged
		netbeansFireChanges = oldFire;

		// Undo info is invalid now...
		u_clearallandblockfree(curbuf);
	    }
	    vim_free(ಗಮ್ಯ_free);
	    nb_ಉತ್ತರ_nil(cmdno); // or !error
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_ಉತ್ತರ_nil(cmdno);
	    retval = FAIL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ // Not a function; no reply required.
    {
// =====================================================================
	ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_create));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, ಖಾಲಿ, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->ಸೇರಿಸು_ಮುಗಿದ = FALSE;
#ಆದರೆ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ಆದರೆ (gui.ಒಳ_use)
		gui_update_menus(0);
#ಆದರೆ_ಅಂತ್ಯ
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "insertDone"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		buf->bufp->b_ಆರಂಭ_eol = *ವಾದಗಳು == 'T';
		buf->ಸೇರಿಸು_ಮುಗಿದ = TRUE;
		ವಾದಗಳು += 2;
		buf->bufp->b_p_ro = *ವಾದಗಳು == 'T';
		print_read_ಸಂದೇಶ(buf);
	    }
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "saveDone"))
	{
	    ದೀರ್ಘ savedChars = atol((ಅಕ್ಷರ *)ವಾದಗಳು);

	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    ಇಲ್ಲದಿದ್ದರೆ
		print_save_ಸಂದೇಶ(buf, savedChars);
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "startDocumentListen"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_startdocumentlisten));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "stopDocumentListen"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_stopdocumentlisten));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    buf->fireChanges = 0;
	    ಆದರೆ (buf->bufp != ಖಾಲಿ && buf->bufp->b_was_netbeans_file)
	    {
		ಆದರೆ (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_ಕಳೆದುಹೋದ_for_ಬಫರ್_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_ಕಳೆದುಹೋದ_for_ಬಫರ್_nr),
							   buf->bufp->b_fnum);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // NetBeans uses stopDocumentListen when it stops editing
		    // a file.  It then expects the buffer in Vim to
		    // disappear.
		    do_bufdel(DOBUF_DEL, (char_u *)"", 1,
				  buf->bufp->b_fnum, buf->bufp->b_fnum, TRUE);
		    CLEAR_ಸೂಚಕ(buf);
		}
	    }
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "setTitle"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_settitle));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(ವಾದಗಳು, ಖಾಲಿ);
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "initDone"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_initdone));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    do_update = 1;
	    buf->init_ಮುಗಿದ = TRUE;
	    nb_ಗಣ_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_ಕೀಲಿ_ಸರತಿ();
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "setBufferNumber")
		|| streq((ಅಕ್ಷರ *)cmd, "putBufferNumber"))
	{
	    char_u	*ಮಾರ್ಗ;
	    buf_T	*bufp;

	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_setbuffernumber));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಮಾರ್ಗ = (char_u *)nb_unquote(ವಾದಗಳು, ಖಾಲಿ);
	    ಆದರೆ (ಮಾರ್ಗ == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	    bufp = buflist_findname(ಮಾರ್ಗ);
	    vim_free(ಮಾರ್ಗ);
	    ಆದರೆ (bufp == ಖಾಲಿ)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", ವಾದಗಳು));
		semsg(_(e_file_str_not_ಸಿಕ್ಕಿತು_ಒಳ_setbuffernumber), ವಾದಗಳು);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_ಸಂಖ್ಯೆ = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    ಆದರೆ (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		buf->init_ಮುಗಿದ = TRUE;

		// handle any postponed key commands
		handle_ಕೀಲಿ_ಸರತಿ();
	    }

// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "setFullName"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_setfullname));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(ವಾದಗಳು, ಖಾಲಿ);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#ಆದರೆ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ಆದರೆ (gui.ಒಳ_use)
		gui_update_menus(0);
#ಆದರೆ_ಅಂತ್ಯ
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "editFile"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_editfile));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(ವಾದಗಳು, ಖಾಲಿ);
	    do_ecmd(0, (char_u *)buf->displayname, ಖಾಲಿ, ಖಾಲಿ, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->init_ಮುಗಿದ = TRUE;
	    do_update = 1;
	    maketitle();
#ಆದರೆ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ಆದರೆ (gui.ಒಳ_use)
		gui_update_menus(0);
#ಆದರೆ_ಅಂತ್ಯ
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "setVisible"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ಆದರೆ (p_verbose > 0)
		    emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_setvisible));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (streq((ಅಕ್ಷರ *)ವಾದಗಳು, "T") && buf->bufp != curbuf)
	    {
		exarg_T exarg;
		CLEAR_ಕ್ಷೇತ್ರ(exarg);
		exarg.cmd = (char_u *)"goto";
		exarg.forceit = FALSE;
		exarg.cmdidx = CMD_USER;
		dosetvisible = TRUE;
		goto_ಬಫರ್(&exarg, DOBUF_ಮೊದಲ, FORWARD, buf->bufp->b_fnum);
		do_update = 1;
		dosetvisible = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		// Side effect!!!.
		ಆದರೆ (gui.ಒಳ_use)
		    gui_mch_ಗಣ_foreground();
#ಆದರೆ_ಅಂತ್ಯ
	    }
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "raise"))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    // Bring gvim to the foreground.
	    ಆದರೆ (gui.ಒಳ_use)
		gui_mch_ಗಣ_foreground();
#ಆದರೆ_ಅಂತ್ಯ
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "setModified"))
	{
	    ಪೂರ್ಣಾಂಕ prev_b_ಬದಲಾಯಿತು;

	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ಆದರೆ (p_verbose > 0)
		    emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_setmodified));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    prev_b_ಬದಲಾಯಿತು = buf->bufp->b_ಬದಲಾಯಿತು;
	    ಆದರೆ (streq((ಅಕ್ಷರ *)ವಾದಗಳು, "T"))
		buf->bufp->b_ಬದಲಾಯಿತು = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		ಆದರೆ (buf->bufp->b_ffname != ಖಾಲಿ
			&& mch_stat((ಅಕ್ಷರ *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_ಸಮಯ(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_ಬದಲಾಯಿತು = FALSE;
	    }
	    buf->modified = buf->bufp->b_ಬದಲಾಯಿತು;
	    ಆದರೆ (prev_b_ಬದಲಾಯಿತು != buf->bufp->b_ಬದಲಾಯಿತು)
	    {
		check_ಸ್ಥಿತಿ(buf->bufp);
		redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "setModtime"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		buf->bufp->b_mtime = atoi((ಅಕ್ಷರ *)ವಾದಗಳು);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "setReadOnly"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)ವಾದಗಳು, "T"))
		buf->bufp->b_p_ro = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "showBalloon"))
	{
#ಆದರೆ defined(FEAT_BEVAL_GUI)
	    ಸ್ಥಿರ ಅಕ್ಷರ	*ಪಠ್ಯ = ಖಾಲಿ;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    ಆದರೆ (balloonEval != ಖಾಲಿ)
	    {
		vim_free(ಪಠ್ಯ);
		ಪಠ್ಯ = nb_unquote(ವಾದಗಳು, ಖಾಲಿ);
		ಆದರೆ (ಪಠ್ಯ != ಖಾಲಿ)
		    gui_mch_post_balloon(balloonEval, (char_u *)ಪಠ್ಯ);
	    }
#ಆದರೆ_ಅಂತ್ಯ
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "setDot"))
	{
	    pos_T *pos;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NBDEBUG
	    char_u *s;
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_setdot));
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    nb_ಗಣ_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
		ಕೊನೆ_visual_ಬಹುಲಕ();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NBDEBUG
	    s = ವಾದಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
	    pos = get_off_or_lnum(buf->bufp, &ವಾದಗಳು);
	    ಆದರೆ (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		nbdebug(("    BAD POSITION in setDot: %s\n", s));
	    }

	    // gui_update_cursor(TRUE, FALSE);
	    // update_curbuf(UPD_NOT_VALID);
	    update_topline();		// scroll to show the line
	    update_screen(UPD_VALID);
	    setcursor();
	    cursor_on();
	    ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);

	    // Quit a hit-return or more prompt.
	    ಆದರೆ (State == ಬಹುಲಕ_HITRETURN || State == ಬಹುಲಕ_ASKMORE)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
		ಆದರೆ (gui.ಒಳ_use && gtk_main_level() > 0)
		    gtk_main_quit();
#ಆದರೆ_ಅಂತ್ಯ
	    }
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "close"))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NBDEBUG
	    ಅಕ್ಷರ *name = "<NONE>";
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_close));
		ಹಿಂತಿರುಗಿ FAIL;
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NBDEBUG
	    ಆದರೆ (buf->displayname != ಖಾಲಿ)
		name = buf->displayname;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ಆದರೆ (p_verbose > 0)
		    emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (buf->bufp != ಖಾಲಿ)
		do_ಬಫರ್(DOBUF_WIPE, DOBUF_ಮೊದಲ, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = ಖಾಲಿ;
	    buf->init_ಮುಗಿದ = FALSE;
	    do_update = 1;
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "defineAnnoType"))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	    ಪೂರ್ಣಾಂಕ type_ಅಂಕೆ;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    ಪೂರ್ಣಾಂಕ parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_defineannotype));
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    cp = (ಅಕ್ಷರ *)ವಾದಗಳು;
	    type_ಅಂಕೆ = strtol(cp, &cp, 10);
	    ವಾದಗಳು = (char_u *)cp;
	    ವಾದಗಳು = skipwhite(ವಾದಗಳು);
	    typeName = (char_u *)nb_unquote(ವಾದಗಳು, &ವಾದಗಳು);
	    ವಾದಗಳು = skipwhite(ವಾದಗಳು + 1);
	    tooltip = (char_u *)nb_unquote(ವಾದಗಳು, &ವಾದಗಳು);
	    ವಾದಗಳು = skipwhite(ವಾದಗಳು + 1);

	    p = (char_u *)nb_unquote(ವಾದಗಳು, &ವಾದಗಳು);
	    glyphFile = vim_strsave_escaped(p, escape_chars);
	    vim_free(p);

	    ವಾದಗಳು = skipwhite(ವಾದಗಳು + 1);
	    p = skiptowhite(ವಾದಗಳು);
	    ಆದರೆ (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(ವಾದಗಳು);
	    bg = vim_strsave(p);
	    ಆದರೆ (STRLEN(fg) > MAX_COLOR_ಉದ್ದಳತೆ || STRLEN(bg) > MAX_COLOR_ಉದ್ದಳತೆ)
	    {
		emsg(_(e_highlighting_color_name_too_long_ಒಳ_define_Anno_Type));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!nb_is_ಸುರಕ್ಷಿತ_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_ಸುರಕ್ಷಿತ_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_ಸುರಕ್ಷಿತ_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_ಒಳ_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (typeName != ಖಾಲಿ && tooltip != ಖಾಲಿ && glyphFile != ಖಾಲಿ)
		addsigntype(buf, type_ಅಂಕೆ, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    ಆದರೆ (parse_error)
		ಹಿಂತಿರುಗಿ FAIL;

#ಆದರೆ_ಅಂತ್ಯ
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "addAnno"))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	    ಪೂರ್ಣಾಂಕ ser_ಅಂಕೆ;
	    ಪೂರ್ಣಾಂಕ local_Type_ಅಂಕೆ;
	    ಪೂರ್ಣಾಂಕ type_ಅಂಕೆ;
	    pos_T *pos;

	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_ಬಫರ್_identifier_ಒಳ_addanno));
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    do_update = 1;

	    cp = (ಅಕ್ಷರ *)ವಾದಗಳು;
	    ser_ಅಂಕೆ = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_ಅಂಕೆ = strtol(cp, &cp, 10);
	    ವಾದಗಳು = (char_u *)cp;
	    type_ಅಂಕೆ = mapsigntype(buf, local_Type_ಅಂಕೆ);

	    pos = get_off_or_lnum(buf->bufp, &ವಾದಗಳು);

	    cp = (ಅಕ್ಷರ *)ವಾದಗಳು;
	    vim_ignored = (ಪೂರ್ಣಾಂಕ)strtol(cp, &cp, 10);
	    ವಾದಗಳು = (char_u *)cp;
# ifdef NBDEBUG
	    ಆದರೆ (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    ಆದರೆ (ser_ಅಂಕೆ >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_ಅಂಕೆ, pos->lnum, type_ಅಂಕೆ, buf->bufp->b_fnum);
		ಆದರೆ (type_ಅಂಕೆ == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_ಅಂಕೆ,
						       buf->bufp->b_fnum);
	    }
#ಆದರೆ_ಅಂತ್ಯ
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "removeAnno"))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	    ಪೂರ್ಣಾಂಕ ser_ಅಂಕೆ;

	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    do_update = 1;
	    cp = (ಅಕ್ಷರ *)ವಾದಗಳು;
	    ser_ಅಂಕೆ = strtol(cp, &cp, 10);
	    ವಾದಗಳು = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_ಅಂಕೆ, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#ಆದರೆ_ಅಂತ್ಯ
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "moveAnnoToFront"))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#ಆದರೆ_ಅಂತ್ಯ
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "guard") || streq((ಅಕ್ಷರ *)cmd, "unguard"))
	{
	    ಪೂರ್ಣಾಂಕ len;
	    pos_T ಮೊದಲ;
	    pos_T ಕೊನೆಯ;
	    pos_T *pos;
	    ಪೂರ್ಣಾಂಕ un = (cmd[0] == 'u');
	    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ guardId = GUARDEDOFFSET;

	    ಆದರೆ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (ಅಕ್ಷರ *) cmd));
		ಹಿಂತಿರುಗಿ OK;
	    }

	    nb_init_graphics();

	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    nb_ಗಣ_curbuf(buf->bufp);
	    cp = (ಅಕ್ಷರ *)ವಾದಗಳು;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, ಖಾಲಿ, 10);
	    ವಾದಗಳು = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    ಆದರೆ (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಮೊದಲ = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		ಆದರೆ (pos != ಖಾಲಿ && pos->ಲಂಬಸಾಲು == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		ಆದರೆ (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ದೀರ್ಘ lnum;
		    ಕೊನೆಯ = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     ಮೊದಲ.lnum, ಮೊದಲ.ಲಂಬಸಾಲು,
			     ಕೊನೆಯ.lnum, ಕೊನೆಯ.ಲಂಬಸಾಲು));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
		    ಗಾಗಿ (lnum = ಮೊದಲ.lnum; lnum <= ಕೊನೆಯ.lnum; lnum++)
		    {
			ಆದರೆ (un)
			{
			    // never used
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಆದರೆ (buf_findsigntype_id(buf->bufp, lnum,
				GUARDED) == 0)
			    {
				coloncmd(
				    ":sign place %d line=%ld name=%d buffer=%d",
				     guardId++, lnum, GUARDED,
				     buf->bufp->b_fnum);
			    }
			}
		    }
#ಆದರೆ_ಅಂತ್ಯ
		    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
		}
	    }
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "startAtomic"))
	{
	    ಒಳ_Atomic = 1;
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "endAtomic"))
	{
	    ಒಳ_Atomic = 0;
	    ಆದರೆ (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(buf->bufp))
	    {
		// Only write if the buffer can be written.
		ಆದರೆ (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != ಖಾಲಿ
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    ಗಣ_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    ಆದರೆ (!bufref_valid(&bufref))
			buf->bufp = ಖಾಲಿ;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "netbeansBuffer"))
	{
	    ಆದರೆ (buf == ಖಾಲಿ || buf->bufp == ಖಾಲಿ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (*ವಾದಗಳು == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "specialKeys"))
	{
	    special_ಕೀಲಿಗಳು(ವಾದಗಳು);
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq((ಅಕ್ಷರ *)cmd, "version"))
	{
	    // not used yet
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    ಆದರೆ (ಒಳ_Atomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    ಆದರೆ (buf != ಖಾಲಿ && buf->init_ಮುಗಿದ && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	ಆದರೆ (State == ಬಹುಲಕ_HITRETURN || State == ಬಹುಲಕ_ASKMORE)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	    ಆದರೆ (gui.ಒಳ_use && gtk_main_level() > 0)
		gtk_main_quit();
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

    ಹಿಂತಿರುಗಿ retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nb_ಗಣ_curbuf(buf_T *buf)
{
    ಆದರೆ (curbuf == buf)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (buf_jump_open_win(buf) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಗಣ_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
coloncmd(ಅಕ್ಷರ *cmd, ...)
{
    ಅಕ್ಷರ buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, ಗಾತ್ರ(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, ಖಾಲಿ, ಖಾಲಿ, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
special_ಕೀಲಿಗಳು(char_u *ವಾದಗಳು)
{
    ಅಕ್ಷರ *save_str = nb_unquote(ವಾದಗಳು, ಖಾಲಿ);
    ಅಕ್ಷರ *tok = strtok(save_str, " ");
    ಅಕ್ಷರ *sep;
#ವ್ಯಾಖ್ಯಾನಿಸು KEYBUFLEN 64
    ಅಕ್ಷರ keybuf[KEYBUFLEN];
    ಅಕ್ಷರ cmdbuf[256];

    ಆಗ (tok != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ i = 0;

	ಆದರೆ ((sep = strchr(tok, '-')) != ಖಾಲಿ)
	{
	    *sep = NUL;
	    ಆಗ (*tok && i + 2 < KEYBUFLEN)
	    {
		ಆಯ್ಕೆ (*tok)
		{
		    ಸ್ಥಿತಿ 'A':
		    ಸ್ಥಿತಿ 'M':
		    ಸ್ಥಿತಿ 'C':
		    ಸ್ಥಿತಿ 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			ನಿಲ್ಲಿಸು;
		}
		tok++;
	    }
	    tok++;
	}

	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ = TRUE;
	    ಗಾಗಿ (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		ಆದರೆ (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    ಸುರಕ್ಷಿತ = FALSE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಆದರೆ (ಸುರಕ್ಷಿತ)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, ಗಾತ್ರ(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, ಬಹುಲಕ_ಸಾಮಾನ್ಯ, FALSE);
	    }
	}
	tok = strtok(ಖಾಲಿ, " ");
    }
    vim_free(save_str);
}

    ಶೂನ್ಯ
ex_nbclose(exarg_T *eap ಬಳಸದ)
{
    netbeans_close();
}

    ಶೂನ್ಯ
ex_nbkey(exarg_T *eap)
{
    (ಶೂನ್ಯ)netbeans_keystring(eap->arg);
}

    ಶೂನ್ಯ
ex_nbstart(
    exarg_T	*eap)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# ಆದರೆ !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    ಆದರೆ (gui.ಒಳ_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	ಹಿಂತಿರುಗಿ;
    }
# endif
#ಆದರೆ_ಅಂತ್ಯ
    netbeans_open((ಅಕ್ಷರ *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nb_init_graphics(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ did_init = FALSE;

    ಆದರೆ (did_init)
	ಹಿಂತಿರುಗಿ;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
netbeans_keyname(ಪೂರ್ಣಾಂಕ ಕೀಲಿ, ಅಕ್ಷರ *buf)
{
    ಅಕ್ಷರ *name = 0;
    ಅಕ್ಷರ namebuf[2];
    ಪೂರ್ಣಾಂಕ ctrl  = 0;
    ಪೂರ್ಣಾಂಕ ಸರಿಸು = 0;
    ಪೂರ್ಣಾಂಕ alt   = 0;

    ಆದರೆ (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL)
	ctrl = 1;
    ಆದರೆ (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು)
	ಸರಿಸು = 1;
    ಆದರೆ (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ALT)
	alt = 1;


    ಆಯ್ಕೆ (ಕೀಲಿ)
    {
	ಸ್ಥಿತಿ K_F1:		name = "F1";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F1:	name = "F1";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F2:		name = "F2";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F2:	name = "F2";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F3:		name = "F3";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F3:	name = "F3";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F4:		name = "F4";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F4:	name = "F4";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F5:		name = "F5";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F5:	name = "F5";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F6:		name = "F6";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F6:	name = "F6";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F7:		name = "F7";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F7:	name = "F7";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F8:		name = "F8";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F8:	name = "F8";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F9:		name = "F9";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F9:	name = "F9";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F10:		name = "F10";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F10:	name = "F10";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F11:		name = "F11";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F11:	name = "F11";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F12:		name = "F12";		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_F12:	name = "F12";	ಸರಿಸು = 1;	ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
			ಆದರೆ (ಕೀಲಿ >= ' ' && ಕೀಲಿ <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = ಕೀಲಿ;
			    namebuf[1] = NUL;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    name = "X";
			ನಿಲ್ಲಿಸು;
    }

    buf[0] = '\0';
    ಆದರೆ (ctrl)
	strcat(buf, "C");
    ಆದರೆ (ಸರಿಸು)
	strcat(buf, "S");
    ಆದರೆ (alt)
	strcat(buf, "M"); // META
    ಆದರೆ (ctrl || ಸರಿಸು || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#ಆದರೆ defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    ಶೂನ್ಯ
netbeans_beval_cb(
	BalloonEval	*beval,
	ಪೂರ್ಣಾಂಕ		 state ಬಳಸದ)
{
    win_T	*wp;
    char_u	*ಪಠ್ಯ;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಅಕ್ಷರ	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    ಆದರೆ (!can_use_beval() || !NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (get_beval_info(beval, TRUE, &wp, &lnum, &ಪಠ್ಯ, &ಲಂಬಸಾಲು) != OK)
	ಹಿಂತಿರುಗಿ;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    ಆದರೆ (ಪಠ್ಯ != ಖಾಲಿ && ಪಠ್ಯ[0] != NUL && STRLEN(ಪಠ್ಯ) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    p = nb_quote(ಪಠ್ಯ);
	    ಆದರೆ (p != ಖಾಲಿ)
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
    vim_free(ಪಠ್ಯ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE when the netbeans connection is active.
 */
    ಪೂರ್ಣಾಂಕ
netbeans_ಸಕ್ರಿಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    ಶೂನ್ಯ
netbeans_open(ಅಕ್ಷರ *params, ಪೂರ್ಣಾಂಕ doabort)
{
    ಅಕ್ಷರ *cmd = "0:startupDone=0\n";

    ಆದರೆ (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (netbeans_connect(params, doabort) != OK)
	ಹಿಂತಿರುಗಿ;

    nbdebug(("EVT: %s", cmd));
    nb_send(cmd, "netbeans_startup_done");

    // update the screen after having added the gutter
    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);
}

/*
 * Tell netbeans that we're exiting. This should be called right
 * before calling exit.
 */
    ಶೂನ್ಯ
netbeans_send_disconnect(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ buf[128];

    ಆದರೆ (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#ಆದರೆ defined(FEAT_EVAL)
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_nb_channel(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ abort = FALSE;
    typval_T tv;

    ಆದರೆ (nb_channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = ಗಣ_ref_ಒಳ_ಅಂಶ(&tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ abort;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    ಶೂನ್ಯ
netbeans_ಚೌಕಟ್ಟು_moved(ಪೂರ್ಣಾಂಕ new_x, ಪೂರ್ಣಾಂಕ new_y)
{
    ಅಕ್ಷರ buf[128];

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (ಪೂರ್ಣಾಂಕ)Columns, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Tell netbeans the user opened or activated a file.
 */
    ಶೂನ್ಯ
netbeans_file_activated(buf_T *bufp)
{
    ಪೂರ್ಣಾಂಕ bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    ಅಕ್ಷರ    ಬಫರ್[2*MAXPATHL];
    char_u  *q;

    ಆದರೆ (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	ಹಿಂತಿರುಗಿ;

    q = nb_quote(bufp->b_ffname);
    ಆದರೆ (q == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (bp == ಖಾಲಿ)
    {
	vim_free(q);
	ಹಿಂತಿರುಗಿ;
    }

    vim_snprintf(ಬಫರ್, ಗಾತ್ರ(ಬಫರ್),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (ಅಕ್ಷರ *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", ಬಫರ್));

    nb_send(ಬಫರ್, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    ಶೂನ್ಯ
netbeans_file_opened(buf_T *bufp)
{
    ಪೂರ್ಣಾಂಕ bufno = nb_getbufno(bufp);
    ಅಕ್ಷರ    ಬಫರ್[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    ಪೂರ್ಣಾಂಕ	    bnum;

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    q = nb_quote(bufp->b_ffname);
    ಆದರೆ (q == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (bp != ಖಾಲಿ)
	bnum = bufno;
    ಇಲ್ಲದಿದ್ದರೆ
	bnum = 0;

    vim_snprintf(ಬಫರ್, ಗಾತ್ರ(ಬಫರ್), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (ಅಕ್ಷರ *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", ಬಫರ್));

    nb_send(ಬಫರ್, "netbeans_file_opened");
    ಆದರೆ (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	ಕೊನೆಯ_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    ಶೂನ್ಯ
netbeans_file_killed(buf_T *bufp)
{
    ಪೂರ್ಣಾಂಕ		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    ಅಕ್ಷರ	ಬಫರ್[2*MAXPATHL];

    ಆದರೆ (!NETBEANS_OPEN || bufno == -1)
	ಹಿಂತಿರುಗಿ;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(ಬಫರ್, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", ಬಫರ್));

    nb_send(ಬಫರ್, "netbeans_file_killed");

    ಆದರೆ (nbbuf != ಖಾಲಿ)
	nbbuf->bufp = ಖಾಲಿ;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    ಸ್ಥಿರ nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, ಪೂರ್ಣಾಂಕ *bufnop)
{
    ಪೂರ್ಣಾಂಕ		bufno;
    nbbuf_T	*nbbuf;

    ಆದರೆ (!NETBEANS_OPEN || !netbeansFireChanges)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    ಆದರೆ (bufno <= 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    ಆದರೆ (nbbuf != ಖಾಲಿ && !nbbuf->fireChanges)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;		// changes in this buffer are not reported

    *bufnop = bufno;
    ಹಿಂತಿರುಗಿ nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    ಶೂನ್ಯ
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	ಲಂಬಸಾಲು,
    char_u	*txt,
    ಪೂರ್ಣಾಂಕ		newlen)
{
    char_u	*buf;
    ಪೂರ್ಣಾಂಕ		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    ದೀರ್ಘ	off;
    char_u	*p;
    char_u	*newtxt;

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ಆದರೆ (nbbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Don't mark as modified for initial read
    ಆದರೆ (nbbuf->ಸೇರಿಸು_ಮುಗಿದ)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    ಆದರೆ (p != ಖಾಲಿ)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((ಅಕ್ಷರ *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((ಅಕ್ಷರ *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    ಶೂನ್ಯ
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	ಲಂಬಸಾಲು,
    ದೀರ್ಘ	len)
{
    char_u	buf[128];
    ಪೂರ್ಣಾಂಕ		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    ದೀರ್ಘ	off;

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ಆದರೆ (nbbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	ಹಿಂತಿರುಗಿ;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;

    off = pos2off(bufp, &pos);

    sprintf((ಅಕ್ಷರ *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((ಅಕ್ಷರ *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    ಶೂನ್ಯ
netbeans_unmodified(buf_T *bufp ಬಳಸದ)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    ಶೂನ್ಯ
netbeans_button_release(ಪೂರ್ಣಾಂಕ button)
{
    ಅಕ್ಷರ	buf[128];
    ಪೂರ್ಣಾಂಕ		bufno;

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    bufno = nb_getbufno(curbuf);

    ಆದರೆ (bufno < 0 || curwin == ಖಾಲಿ || curwin->w_ಬಫರ್ != curbuf)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು - curwin->w_wincol
	- ((curwin->w_p_nu || curwin->w_p_rnu) ? 9 : 1);
    ದೀರ್ಘ off = pos2off(curbuf, &curwin->w_cursor);

    // sync the cursor position
    sprintf(buf, "%d:newDotAndMark=%d %ld %ld\n", bufno, r_cmdno, off, off);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release[newDotAndMark]");

    sprintf(buf, "%d:buttonRelease=%d %d %ld %d\n", bufno, r_cmdno,
	    button, (ದೀರ್ಘ)curwin->w_cursor.lnum, ಲಂಬಸಾಲು);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release");
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key code.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    ಪೂರ್ಣಾಂಕ
netbeans_keycommand(ಪೂರ್ಣಾಂಕ ಕೀಲಿ)
{
    ಅಕ್ಷರ	ಕೀಲಿ_Name[60];

    netbeans_keyname(ಕೀಲಿ, ಕೀಲಿ_Name);
    ಹಿಂತಿರುಗಿ netbeans_keystring((char_u *)ಕೀಲಿ_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
netbeans_keystring(char_u *ಕೀಲಿ_Name)
{
    ಅಕ್ಷರ	buf[2*MAXPATHL];
    ಪೂರ್ಣಾಂಕ		bufno = nb_getbufno(curbuf);
    ದೀರ್ಘ	off;
    char_u	*q;

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == ಖಾಲಿ ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	ಆದರೆ (q == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ TRUE;
	vim_snprintf(buf, ಗಾತ್ರ(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	ಆದರೆ (curbuf->b_ffname != ಖಾಲಿ)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(ಕೀಲಿ_Name);
	ಹಿಂತಿರುಗಿ FALSE;
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
    vim_snprintf(buf, ಗಾತ್ರ(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, ಕೀಲಿ_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, ಗಾತ್ರ(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, ಕೀಲಿ_Name,
		off, (ದೀರ್ಘ)curwin->w_cursor.lnum, (ದೀರ್ಘ)curwin->w_cursor.ಲಂಬಸಾಲು);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    ಹಿಂತಿರುಗಿ TRUE;
}


/*
 * Send a save event to netbeans.
 */
    ಶೂನ್ಯ
netbeans_save_ಬಫರ್(buf_T *bufp)
{
    char_u	buf[64];
    ಪೂರ್ಣಾಂಕ		bufno;
    nbbuf_T	*nbbuf;

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ಆದರೆ (nbbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    nbbuf->modified = 0;

    sprintf((ಅಕ್ಷರ *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((ಅಕ್ಷರ *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    ಶೂನ್ಯ
netbeans_deleted_all_ಸಾಲುಗಳು(buf_T *bufp)
{
    char_u	buf[64];
    ಪೂರ್ಣಾಂಕ		bufno;
    nbbuf_T	*nbbuf;

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ಆದರೆ (nbbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Don't mark as modified for initial read
    ಆದರೆ (nbbuf->ಸೇರಿಸು_ಮುಗಿದ)
	nbbuf->modified = 1;

    sprintf((ಅಕ್ಷರ *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    ಪೂರ್ಣಾಂಕ
netbeans_is_guarded(linenr_T ತುದಿ, linenr_T bot)
{
    sign_ನಮೂದು_T	*p;
    ಪೂರ್ಣಾಂಕ			lnum;

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ FALSE;

    FOR_ALL_SIGNS_ಒಳ_BUF(curbuf, p)
	ಆದರೆ (p->se_id >= GUARDEDOFFSET)
	    ಗಾಗಿ (lnum = ತುದಿ + 1; lnum < bot; lnum++)
		ಆದರೆ (lnum == p->se_lnum)
		    ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    ಶೂನ್ಯ
netbeans_draw_multisign_indicator(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ y;
    ಪೂರ್ಣಾಂಕ x;

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    x = 0;
    y = ಅಡ್ಡಸಾಲು * gui.char_ಎತ್ತರ + 2;

    ಗಾಗಿ (i = 0; i < gui.char_ಎತ್ತರ - 3; i++)
	XDraw_ಬಿಂದು(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, x+2, y++);

    XDraw_ಬಿಂದು(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, x+0, y);
    XDraw_ಬಿಂದು(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, x+2, y);
    XDraw_ಬಿಂದು(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, x+4, y++);
    XDraw_ಬಿಂದು(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, x+1, y);
    XDraw_ಬಿಂದು(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, x+2, y);
    XDraw_ಬಿಂದು(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, x+3, y++);
    XDraw_ಬಿಂದು(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, x+2, y);
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_X11

#ಆದರೆ defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    ಶೂನ್ಯ
netbeans_draw_multisign_indicator(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ y;
    ಪೂರ್ಣಾಂಕ x;
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    cairo_t *cr = ಖಾಲಿ;
# ಇಲ್ಲದಿದ್ದರೆ
    GdkDrawable *drawable = gui.drawarea->ಕಿಟಕಿ_2;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# ಇಲ್ಲದಿದ್ದರೆ
    y = ಅಡ್ಡಸಾಲು * gui.char_ಎತ್ತರ + 2;
# endif

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_ಎತ್ತರ);
    cr = cairo_create(surf);
#  ಇಲ್ಲದಿದ್ದರೆ
    cr = cairo_create(gui.surface);
#  endif
    cairo_ಗಣ_ಮೂಲ_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    ಗಾಗಿ (i = 0; i < gui.char_ಎತ್ತರ - 3; i++)
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# ಇಲ್ಲದಿದ್ದರೆ
    gdk_draw_ಬಿಂದು(drawable, gui.ಪಠ್ಯ_gc, x+2, y++);
# endif

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# ಇಲ್ಲದಿದ್ದರೆ
    gdk_draw_ಬಿಂದು(drawable, gui.ಪಠ್ಯ_gc, x+0, y);
    gdk_draw_ಬಿಂದು(drawable, gui.ಪಠ್ಯ_gc, x+2, y);
    gdk_draw_ಬಿಂದು(drawable, gui.ಪಠ್ಯ_gc, x+4, y++);
    gdk_draw_ಬಿಂದು(drawable, gui.ಪಠ್ಯ_gc, x+1, y);
    gdk_draw_ಬಿಂದು(drawable, gui.ಪಠ್ಯ_gc, x+2, y);
    gdk_draw_ಬಿಂದು(drawable, gui.ಪಠ್ಯ_gc, x+3, y++);
    gdk_draw_ಬಿಂದು(drawable, gui.ಪಠ್ಯ_gc, x+2, y);
# endif

# ifdef USE_GTK4
    cairo_fill(cr);
    gui_gtk4_ಕೂಡಿಸು_multisign(surf, ಅಡ್ಡಸಾಲು, 0, 5, gui.char_ಎತ್ತರ);
# endif

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    cairo_destroy(cr);
# endif
# ifdef USE_GTK4
    cairo_surface_destroy(surf);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_GTK

/*
 * If the mouse is clicked in the gutter of a line with multiple
 * annotations, cycle through the set of signs.
 */
    ಶೂನ್ಯ
netbeans_gutter_click(linenr_T lnum)
{
    sign_ನಮೂದು_T	*p;

    ಆದರೆ (!NETBEANS_OPEN)
	ಹಿಂತಿರುಗಿ;

    FOR_ALL_SIGNS_ಒಳ_BUF(curbuf, p)
    {
	ಆದರೆ (p->se_lnum == lnum && p->se_ಮುಂದಿನ && p->se_ಮುಂದಿನ->se_lnum == lnum)
	{
	    sign_ನಮೂದು_T *ಬಾಲ;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    ಆದರೆ (p->se_prev)
		p->se_prev->se_ಮುಂದಿನ = p->se_ಮುಂದಿನ;
	    ಇಲ್ಲದಿದ್ದರೆ
		curbuf->b_signlist = p->se_ಮುಂದಿನ;
	    p->se_ಮುಂದಿನ->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    ಗಾಗಿ (ಬಾಲ = p->se_ಮುಂದಿನ;
		  ಬಾಲ->se_ಮುಂದಿನ && ಬಾಲ->se_ಮುಂದಿನ->se_lnum == lnum
				       && ಬಾಲ->se_ಮುಂದಿನ->se_id < GUARDEDOFFSET;
		  ಬಾಲ = ಬಾಲ->se_ಮುಂದಿನ)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_ಮುಂದಿನ = ಬಾಲ->se_ಮುಂದಿನ;
	    ಆದರೆ (ಬಾಲ->se_ಮುಂದಿನ)
		ಬಾಲ->se_ಮುಂದಿನ->se_prev = p;
	    p->se_prev = ಬಾಲ;
	    ಬಾಲ->se_ಮುಂದಿನ = p;
	    update_debug_sign(curbuf, lnum);
	    ನಿಲ್ಲಿಸು;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
addsigntype(
    nbbuf_T	*buf,
    ಪೂರ್ಣಾಂಕ		type_ಅಂಕೆ,
    char_u	*typeName,
    char_u	*tooltip ಬಳಸದ,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    ಪೂರ್ಣಾಂಕ i, j;
    ಪೂರ್ಣಾಂಕ use_fg = (*fg && STRCMP(fg, "none") != 0);
    ಪೂರ್ಣಾಂಕ use_bg = (*bg && STRCMP(bg, "none") != 0);

    ಗಾಗಿ (i = 0; i < globalsignmapused; i++)
	ಆದರೆ (STRCMP(typeName, globalsignmap[i]) == 0)
	    ನಿಲ್ಲಿಸು;

    ಆದರೆ (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_ಅಂಕೆ, typeName, tooltip, glyphFile, fg, bg));
	ಆದರೆ (use_fg || use_bg)
	{
	    ಅಕ್ಷರ fgbuf[2 * (8 + MAX_COLOR_ಉದ್ದಳತೆ) + 1];
	    ಅಕ್ಷರ bgbuf[2 * (8 + MAX_COLOR_ಉದ್ದಳತೆ) + 1];
	    ಅಕ್ಷರ *ptr;
	    ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ;

	    ಮೌಲ್ಯ = strtol((ಅಕ್ಷರ *)fg, &ptr, 10);
	    ಆದರೆ (ptr != (ಅಕ್ಷರ *)fg)
		sprintf(fgbuf, "guifg=#%06x", ಮೌಲ್ಯ & 0xFFFFFF);
	    ಇಲ್ಲದಿದ್ದರೆ
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    ಮೌಲ್ಯ = strtol((ಅಕ್ಷರ *)bg, &ptr, 10);
	    ಆದರೆ (ptr != (ಅಕ್ಷರ *)bg)
		sprintf(bgbuf, "guibg=#%06x", ಮೌಲ್ಯ & 0xFFFFFF);
	    ಇಲ್ಲದಿದ್ದರೆ
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    ಆದರೆ (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    ಇಲ್ಲದಿದ್ದರೆ
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	ಆದರೆ (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_ಅಂಕೆ;

	ಆದರೆ (globalsignmapused == globalsignmaplen)
	{
	    ಆದರೆ (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(ಅಕ್ಷರ *, globalsignmaplen);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ    // grow it
	    {
		ಪೂರ್ಣಾಂಕ incr;
		ಪೂರ್ಣಾಂಕ oldlen = globalsignmaplen;
		ಅಕ್ಷರ **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * ಗಾತ್ರ(ಅಕ್ಷರ *));
		ಆದರೆ (globalsignmap == ಖಾಲಿ)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    ಹಿಂತಿರುಗಿ;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * ಗಾತ್ರ(ಅಕ್ಷರ *));
	    }
	}

	globalsignmap[i] = (ಅಕ್ಷರ *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    ಗಾಗಿ (j = 0; j < buf->signmapused; j++)
	ಆದರೆ (buf->signmap[j] == i + 1)
	    ಹಿಂತಿರುಗಿ;

    // add to local map
    ಆದರೆ (buf->signmapused == buf->signmaplen)
    {
	ಆದರೆ (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(ಪೂರ್ಣಾಂಕ, buf->signmaplen);
	}
	ಇಲ್ಲದಿದ್ದರೆ    // grow it
	{
	    ಪೂರ್ಣಾಂಕ incr;
	    ಪೂರ್ಣಾಂಕ oldlen = buf->signmaplen;
	    ಪೂರ್ಣಾಂಕ *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
	    ಆದರೆ (buf->signmap == ಖಾಲಿ)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		ಹಿಂತಿರುಗಿ;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mapsigntype(nbbuf_T *buf, ಪೂರ್ಣಾಂಕ localsigntype)
{
    ಆದರೆ (--localsigntype >= 0 && localsigntype < buf->signmapused)
	ಹಿಂತಿರುಗಿ buf->signmap[localsigntype];

    ಹಿಂತಿರುಗಿ 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
get_buf_ಗಾತ್ರ(buf_T *bufp)
{
    linenr_T	lnum;
    ದೀರ್ಘ	char_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		eol_ಗಾತ್ರ;
    ದೀರ್ಘ	ಕೊನೆಯ_check = 100000L;

    ಆದರೆ (bufp->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (get_fileformat(bufp) == EOL_DOS)
	eol_ಗಾತ್ರ = 2;
    ಇಲ್ಲದಿದ್ದರೆ
	eol_ಗಾತ್ರ = 1;
    ಗಾಗಿ (lnum = 1; lnum <= bufp->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
    {
	char_ಎಣಿಕೆ += ml_get_buf_len(bufp, lnum) + eol_ಗಾತ್ರ;
	// Check for a CTRL-C every 100000 characters
	ಆದರೆ (char_ಎಣಿಕೆ > ಕೊನೆಯ_check)
	{
	    ui_breakcheck();
	    ಆದರೆ (got_int)
		ಹಿಂತಿರುಗಿ char_ಎಣಿಕೆ;
	    ಕೊನೆಯ_check = char_ಎಣಿಕೆ + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    ಆದರೆ (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_ಎಣಿಕೆ -= eol_ಗಾತ್ರ;

    ಹಿಂತಿರುಗಿ char_ಎಣಿಕೆ;
}

/*
 * Convert character offset to lnum,col
 */
    ಸ್ಥಿರ pos_T *
off2pos(buf_T *buf, ದೀರ್ಘ ಸ್ಥಾನಾಂತರ)
{
    linenr_T	 lnum;
    ಸ್ಥಿರ pos_T pos;

    pos.lnum = 0;
    pos.ಲಂಬಸಾಲು = 0;
    pos.coladd = 0;

    ಆದರೆ (!(buf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು))
    {
	ಆದರೆ ((lnum = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು_or_ಸ್ಥಾನಾಂತರ(buf, (linenr_T)0, &ಸ್ಥಾನಾಂತರ)) < 0)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	pos.lnum = lnum;
	pos.ಲಂಬಸಾಲು = ಸ್ಥಾನಾಂತರ;
    }

    ಹಿಂತಿರುಗಿ &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    ಸ್ಥಿರ pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    ಸ್ಥಿರ pos_T	mypos;
    ದೀರ್ಘ		off;

    off = strtol((ಅಕ್ಷರ *)*argp, (ಅಕ್ಷರ **)argp, 10);
    ಆದರೆ (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.ಲಂಬಸಾಲು = strtol((ಅಕ್ಷರ *)*argp, (ಅಕ್ಷರ **)argp, 10);
	mypos.coladd = 0;
	ಹಿಂತಿರುಗಿ &mypos;
    }
    ಹಿಂತಿರುಗಿ off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
pos2off(buf_T *buf, pos_T *pos)
{
    ದೀರ್ಘ	 ಸ್ಥಾನಾಂತರ = 0;

    ಆದರೆ (buf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ ((ಸ್ಥಾನಾಂತರ = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು_or_ಸ್ಥಾನಾಂತರ(buf, pos->lnum, 0)) < 0)
	ಹಿಂತಿರುಗಿ 0;
    ಸ್ಥಾನಾಂತರ += pos->ಲಂಬಸಾಲು;

    ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
print_read_ಸಂದೇಶ(nbbuf_T *buf)
{
    ಪೂರ್ಣಾಂಕ	    lnum = buf->bufp->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    off_T   nchars = buf->bufp->b_orig_ಗಾತ್ರ;
    char_u  c;

    ಸಂದೇಶ_ಕೂಡಿಸು_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    ಆದರೆ (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    ಆದರೆ (!buf->bufp->b_ಆರಂಭ_eol)
    {
	STRCAT(IObuff, shortmess(SHM_ಕೊನೆಯ) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    ಸಂದೇಶ_ಕೂಡಿಸು_ಸಾಲುಗಳು(c, (ದೀರ್ಘ)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_ಸಂದೇಶ);
    ಸಂದೇಶ_scrolled_ign = TRUE;
    ಸಂದೇಶ_trunc_attr((ಅಕ್ಷರ *)IObuff, FALSE, 0);
    ಸಂದೇಶ_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
print_save_ಸಂದೇಶ(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    ಆದರೆ (nchars >= 0)
    {
	// put fname in IObuff with quotes
	ಸಂದೇಶ_ಕೂಡಿಸು_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	ಸಂದೇಶ_ಕೂಡಿಸು_ಸಾಲುಗಳು(c, buf->bufp->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
						buf->bufp->b_orig_ಗಾತ್ರ);

	VIM_CLEAR(keep_ಸಂದೇಶ);
	ಸಂದೇಶ_scrolled_ign = TRUE;
	p = (char_u *)ಸಂದೇಶ_trunc_attr((ಅಕ್ಷರ *)IObuff, FALSE, 0);
	ಆದರೆ ((ಸಂದೇಶ_scrolled && !need_wait_return) || !buf->init_ಮುಗಿದ)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    ಗಣ_keep_ಸಂದೇಶ(p, 0);
	}
	ಸಂದೇಶ_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಕ್ಷರ msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#ಆದರೆ_ಅಂತ್ಯ // defined(FEAT_NETBEANS_INTG)
