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

#شامل "vim.h"

#اگر defined(FEAT_NETBEANS_INTG)

#اگر_غیر_تعریف_شدہ MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#اگر_ختم

#شامل "version.h"

#تعریف_کرو GUARDED		10000 // typenr for "guarded" annotation
#تعریف_کرو GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#تعریف_کرو MAX_COLOR_لمبائی 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#تعریف_کرو VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#تعریف_کرو VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#تعریف_کرو VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
ساکن حرف *Ext_Ed_Protocol_نسخہ = "2.5";

ساکن طویل pos2off(buf_T *, pos_T *);
ساکن pos_T *off2pos(buf_T *, طویل);
ساکن pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
ساکن طویل get_buf_سائز(buf_T *);
ساکن صحیح netbeans_keystring(char_u *keystr);
ساکن خالی special_کلیدیں(char_u *دلائل);

ساکن صحیح getConnInfo(حرف *file, حرف **host, حرف **پورٹ, حرف **password);

ساکن خالی nb_init_graphics(خالی);
ساکن خالی coloncmd(حرف *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ساکن خالی nb_سیٹ_curbuf(buf_T *buf);
ساکن خالی nb_parse_cmd(char_u *);
ساکن صحیح  nb_do_cmd(صحیح, char_u *, صحیح, صحیح, char_u *);
ساکن خالی nb_send(حرف *buf, حرف *fun);
ساکن خالی nb_free(خالی);

#تعریف_کرو NETBEANS_OPEN (channel_can_write_مقصد(nb_channel))
ساکن channel_T *nb_channel = تہی;

ساکن صحیح r_cmdno;			// current command number for reply
ساکن صحیح dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#اگر_تعریف_شدہ NBDEBUG
# include "nbdebug.c"
#اگر_ختم

ساکن صحیح needupdate = 0;
ساکن صحیح اندر_Atomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    ساکن صحیح
nb_is_محفوظ_string(char_u *str, char_u *allowed)
{
    اگر (str == تہی)
	واپس FALSE;
    برائے (char_u *p = str; *p != NUL; p++)
	اگر (vim_strchr(allowed, *p) == تہی)
	    واپس FALSE;
    واپس TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    ساکن خالی
nb_channel_closed(خالی)
{
    nb_channel = تہی;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    ساکن خالی
netbeans_close(خالی)
{
    اگر (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	اگر (nb_channel != تہی)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = تہی;
    }

#اگر_تعریف_شدہ FEAT_BEVAL_GUI
    bevalServers &= ~BEVAL_NETBEANS;
#اگر_ختم

    needupdate = 0;
    اندر_Atomic = 0;
    nb_free();

    // remove all signs and update the screen after gutter removal
    coloncmd(":sign unplace *");
    بدلا_کھڑکی_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    برآمد_flush_cursor(TRUE, FALSE);
}

#تعریف_کرو NB_DEF_HOST "localhost"
#تعریف_کرو NB_DEF_ADDR "3219"
#تعریف_کرو NB_DEF_PASS "changeme"

    ساکن صحیح
netbeans_connect(حرف *params, صحیح doabort)
{
    صحیح		پورٹ;
    حرف	buf[32];
    حرف	*hostname = تہی;
    حرف	*پتہ = تہی;
    حرف	*password = تہی;
    حرف	*fname;
    حرف	*arg = تہی;

    اگر (*params == '=')
    {
	// "=fname": Read info from specified file.
	اگر (getConnInfo(params + 1, &hostname, &پتہ, &password) == FAIL)
	    واپس FAIL;
    }
    ورنہ
    {
	اگر (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	اگر (arg == تہی && (fname = getenv("__NETBEANS_CONINFO")) != تہی)
	{
	    // "": get info from file specified in environment
	    اگر (getConnInfo(fname, &hostname, &پتہ, &password) == FAIL)
		واپس FAIL;
	}
	ورنہ
	{
	    اگر (arg != تہی)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		پتہ = strchr(hostname, ':');
		اگر (پتہ != تہی)
		{
		    *پتہ++ = '\0';
		    password = strchr(پتہ, ':');
		    اگر (password != تہی)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    اگر (hostname == تہی || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    اگر (پتہ == تہی)
		پتہ = getenv("__NETBEANS_SOCKET");
	    اگر (password == تہی)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    اگر (hostname != تہی)
		hostname = (حرف *)vim_strsave((char_u *)hostname);
	    اگر (پتہ != تہی)
		پتہ = (حرف *)vim_strsave((char_u *)پتہ);
	    اگر (password != تہی)
		password = (حرف *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    اگر (hostname == تہی || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (حرف *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    اگر (پتہ == تہی || *پتہ == '\0')
    {
	vim_free(پتہ);
	پتہ = (حرف *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    اگر (password == تہی || *password == '\0')
    {
	vim_free(password);
	password = (حرف *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    اگر (hostname != تہی && پتہ != تہی && password != تہی)
    {
	پورٹ = atoi(پتہ);
	nb_channel = channel_open(hostname, پورٹ, 3000, nb_channel_closed);
	اگر (nb_channel != تہی)
	{
	    // success
#اگر_تعریف_شدہ FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#اگر_ختم

	    // success, login
	    vim_snprintf(buf, حجم(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", Ext_Ed_Protocol_نسخہ);
	    nb_send(buf, "externaleditor_version");
	}
    }

    اگر (nb_channel == تہی && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(پتہ);
    vim_free(password);
    واپس NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    ساکن صحیح
getConnInfo(حرف *file, حرف **host, حرف **پورٹ, حرف **auth)
{
    FILE *fp;
    char_u buf[BUFSIZ];
    char_u *lp;
    char_u *nlp;
#اگر_تعریف_شدہ UNIX
    stat_T	st;

    /*
     * For Unix only accept the file when it's not accessible by others.
     * The open will then fail if we don't own the file.
     */
    اگر (mch_stat(file, &st) == 0 && (st.st_منوال & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_منوال_for_netbeans_connection_info_file_str),
									file);
	واپس FAIL;
    }
#اگر_ختم

    fp = mch_fopen(file, "r");
    اگر (fp == تہی)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	واپس FAIL;
    }

    // Read the file. There should be one of each parameter
    جب ((lp = (char_u *)fgets((حرف *)buf, BUFSIZ, fp)) != تہی)
    {
	اگر ((nlp = vim_strchr(lp, '\n')) != تہی)
	    *nlp = 0;	    // strip off the trailing newline

	اگر (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (حرف *)vim_strsave(&buf[5]);
	}
	ورنہ اگر (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*پورٹ);
	    *پورٹ = (حرف *)vim_strsave(&buf[5]);
	}
	ورنہ اگر (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (حرف *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    واپس OK;
}


ساخت keyqueue
{
    char_u	    *keystr;
    ساخت keyqueue *اگلا;
    ساخت keyqueue *پچھلا;
};

قسمتعریف ساخت keyqueue کلید_Q_T;

ساکن کلید_Q_T کلید_سر; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    ساکن خالی
postpone_keycommand(char_u *keystr)
{
    کلید_Q_T *گرہ;

    گرہ = ALLOC_ONE(کلید_Q_T);
    اگر (گرہ == تہی)
	واپس;  // out of memory, drop the key

    اگر (کلید_سر.اگلا == تہی) // initialize circular queue
    {
	کلید_سر.اگلا = &کلید_سر;
	کلید_سر.پچھلا = &کلید_سر;
    }

    // insert node at tail of queue
    گرہ->اگلا = &کلید_سر;
    گرہ->پچھلا = کلید_سر.پچھلا;
    کلید_سر.پچھلا->اگلا = گرہ;
    کلید_سر.پچھلا = گرہ;

    گرہ->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    ساکن خالی
handle_کلید_قطار(خالی)
{
    صحیح postponed = FALSE;

    جب (!postponed && کلید_سر.اگلا && کلید_سر.اگلا != &کلید_سر)
    {
	// first, unlink the node
	کلید_Q_T *گرہ = کلید_سر.اگلا;
	کلید_سر.اگلا = گرہ->اگلا;
	گرہ->اگلا->پچھلا = گرہ->پچھلا;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	اگر (گرہ->keystr != تہی)
	    postponed = !netbeans_keystring(گرہ->keystr);
	vim_free(گرہ->keystr);

	// Finally, dispose of the node
	vim_free(گرہ);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    خالی
netbeans_parse_messages(خالی)
{
    readq_T	*گرہ;
    char_u	*بفر;
    char_u	*p;
    صحیح		own_گرہ;

    جب (nb_channel != تہی)
    {
	گرہ = channel_peek(nb_channel, PART_SOCK);
	اگر (گرہ == تہی)
	    توڑو;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_پہلا_nl(گرہ);
	اگر (p == تہی)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    اگر (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		واپس;
	    جاری;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	اگر (*p == NUL)
	{
	    own_گرہ = TRUE;
	    بفر = channel_get(nb_channel, PART_SOCK, تہی);
	    // "node" is now invalid!
	}
	ورنہ
	{
	    own_گرہ = FALSE;
	    بفر = گرہ->rq_بفر;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(بفر);

	اگر (own_گرہ)
	    // buffer finished, dispose of it
	    vim_free(بفر);
	ورنہ اگر (nb_channel != تہی)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (صحیح)(p - بفر));
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
    ساکن خالی
nb_parse_cmd(char_u *cmd)
{
    حرف	*verb;
    حرف	*q;
    صحیح		bufno;
    صحیح		isfunc = -1;

    اگر (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    اگر (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_عمود = false;

	// The IDE is breaking the connection.
	netbeans_close();
	واپس;
    }

    bufno = strtol((حرف *)cmd, &verb, 10);

    اگر (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	واپس;
    }
    ++verb; // skip colon

    برائے (q = verb; *q; q++)
    {
	اگر (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    توڑو;
	}
	ورنہ اگر (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    توڑو;
	}
    }

    اگر (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_اندر_str), cmd);
	واپس;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (حرف *)skipwhite((char_u *)q);

    اگر (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
    {
#اگر_تعریف_شدہ NBDEBUG
	/*
	 * This happens because the ExtEd can send a command or 2 after
	 * doing a stopDocumentListen command. It doesn't harm anything
	 * so I'm disabling it except for debugging.
	 */
	nbdebug(("nb_parse_cmd: Command error for \"%s\"\n", cmd));
	emsg(e_bad_return_from_nb_do_cmd);
#اگر_ختم
    }
}

ساخت nbbuf_struct
{
    buf_T		*bufp;
    غیرعلامتی صحیح	 fireChanges:1;
    غیرعلامتی صحیح	 init_مکمل:1;
    غیرعلامتی صحیح	 داخل_کرو_مکمل:1;
    غیرعلامتی صحیح	 modified:1;
    صحیح			 nbbuf_نمبر;
    حرف		*displayname;
    صحیح			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

قسمتعریف ساخت nbbuf_struct nbbuf_T;

ساکن nbbuf_T *buf_list = تہی;
ساکن صحیح buf_list_سائز = 0;	// size of buf_list
ساکن صحیح buf_list_مستعمل = 0;	// nr of entries in buf_list actually in use

ساکن حرف **globalsignmap = تہی;
ساکن صحیح globalsignmaplen = 0;
ساکن صحیح globalsignmapused = 0;

ساکن صحیح  mapsigntype(nbbuf_T *, صحیح localsigntype);
ساکن خالی addsigntype(nbbuf_T *, صحیح localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
ساکن خالی print_read_پیغام(nbbuf_T *buf);
ساکن خالی print_save_پیغام(nbbuf_T *buf, off_T nchars);

ساکن صحیح curPCtype = -1;

/*
 * Free netbeans resources.
 */
    ساکن خالی
nb_free(خالی)
{
    کلید_Q_T *کلید_گرہ = کلید_سر.اگلا;
    nbbuf_T buf;
    صحیح i;

    // free the netbeans buffer list
    برائے (i = 0; i < buf_list_مستعمل; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	اگر (buf.bufp != تہی && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_سائز = 0;
    buf_list_مستعمل = 0;

    // free the queued key commands
    جب (کلید_گرہ != تہی && کلید_گرہ != &کلید_سر)
    {
	کلید_Q_T *اگلا = کلید_گرہ->اگلا;
	vim_free(کلید_گرہ->keystr);
	vim_free(کلید_گرہ);
	اگر (اگلا == &کلید_سر)
	{
	    کلید_سر.اگلا = &کلید_سر;
	    کلید_سر.پچھلا = &کلید_سر;
	    توڑو;
	}
	کلید_گرہ = اگلا;
    }

    // free the queued netbeans commands
    اگر (nb_channel != تہی)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    ساکن صحیح
nb_getbufno(buf_T *bufp)
{
    صحیح i;

    برائے (i = 0; i < buf_list_مستعمل; i++)
	اگر (buf_list[i].bufp == bufp)
	    واپس i;
    واپس -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    صحیح
is_Netbeans_بفر(buf_T *bufp)
{
    واپس NETBEANS_OPEN && bufp->b_netbeans_file;
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
    صحیح
isNetbeansModified(buf_T *bufp)
{
    اگر (is_Netbeans_بفر(bufp))
    {
	صحیح bufno = nb_getbufno(bufp);

	اگر (bufno > 0)
	    واپس buf_list[bufno].modified;
	ورنہ
	    واپس FALSE;
    }
    ورنہ
	واپس FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    ساکن nbbuf_T *
nb_get_buf(صحیح bufno)
{
    // find or create a buffer with the given number
    صحیح incr;

    اگر (bufno <= 0)
	واپس تہی;

    اگر (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * حجم(nbbuf_T));
	buf_list_سائز = 100;
    }
    اگر (bufno >= buf_list_مستعمل) // new
    {
	اگر (bufno >= buf_list_سائز) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_سائز + 90;
	    buf_list_سائز += incr;
	    bufsize = buf_list_سائز * حجم(nbbuf_T);
	    اگر (bufsize == 0 || bufsize / حجم(nbbuf_T)
						      != (size_t)buf_list_سائز)
	    {
		// list size overflow, bail out
		واپس تہی;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    اگر (buf_list == تہی)
	    {
		vim_free(t_buf_list);
		buf_list_سائز = 0;
		واپس تہی;
	    }
	    vim_memset(buf_list + buf_list_سائز - incr, 0,
						      incr * حجم(nbbuf_T));
	}

	جب (buf_list_مستعمل <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_مستعمل].fireChanges = 1;
	    ++buf_list_مستعمل;
	}
    }

    واپس buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    ساکن صحیح
گنتی_بدلا_buffers(خالی)
{
    buf_T	*bufp;
    صحیح		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	اگر (bufp->b_بدلا)
	    ++n;
    واپس n;
}

/*
 * End the netbeans session.
 */
    خالی
netbeans_انتہا(خالی)
{
    صحیح i;
    ساکن حرف buf[128];

    اگر (!NETBEANS_OPEN)
	واپس;

    برائے (i = 0; i < buf_list_مستعمل; i++)
    {
	اگر (!buf_list[i].bufp)
	    جاری;
	اگر (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, تہی);
	buf_list[i].bufp = تہی;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    ساکن خالی
nb_send(حرف *buf, حرف *fun)
{
    اگر (nb_channel != تہی)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (صحیح)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    ساکن خالی
nb_جواب_nil(صحیح cmdno)
{
    حرف جواب[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    اگر (!NETBEANS_OPEN)
	واپس;

    sprintf(جواب, "%d\n", cmdno);
    nb_send(جواب, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    ساکن خالی
nb_جواب_متن(صحیح cmdno, char_u *نتیجہ)
{
    char_u *جواب;

    nbdebug(("REP %d: %s\n", cmdno, (حرف *)نتیجہ));

    جواب = alloc(STRLEN(نتیجہ) + 32);
    sprintf((حرف *)جواب, "%d %s\n", cmdno, (حرف *)نتیجہ);
    nb_send((حرف *)جواب, "nb_reply_text");

    vim_free(جواب);
}


/*
 * Send a response with a number result code.
 */
    ساکن خالی
nb_جواب_nr(صحیح cmdno, طویل نتیجہ)
{
    حرف جواب[32];

    nbdebug(("REP %d: %ld\n", cmdno, نتیجہ));

    sprintf(جواب, "%d %ld\n", cmdno, نتیجہ);
    nb_send(جواب, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    ساکن char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    اگر (buf == تہی)
	واپس تہی;
    برائے (; *p; p++)
    {
	انتخاب (*p)
	{
	    حالت '\"':
	    حالت '\\':
		*q++ = '\\'; *q++ = *p; توڑو;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    حالت '\n':
		*q++ = '\\'; *q++ = 'n'; توڑو;
	    حالت '\r':
		*q++ = '\\'; *q++ = 'r'; توڑو;
	    طےشدہ:
		*q++ = *p;
		توڑو;
	}
    }
    *q = '\0';

    واپس buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    ساکن حرف *
nb_unquote(char_u *p, char_u **endp)
{
    حرف *نتیجہ = 0;
    حرف *q;
    صحیح مکمل = 0;

    // result is never longer than input
    نتیجہ = alloc_clear(STRLEN(p) + 1);
    اگر (نتیجہ == تہی)
	واپس تہی;

    اگر (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	نتیجہ[0] = NUL;
	واپس نتیجہ;
    }

    برائے (q = نتیجہ; !مکمل && *p != NUL;)
    {
	انتخاب (*p)
	{
	    حالت '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		مکمل = 1;
		توڑو;

	    حالت '\\':
		++p;
		انتخاب (*p)
		{
		    حالت '\\':	*q++ = '\\';	توڑو;
		    حالت 'n':	*q++ = '\n';	توڑو;
		    حالت 't':	*q++ = '\t';	توڑو;
		    حالت 'r':	*q++ = '\r';	توڑو;
		    حالت '"':	*q++ = '"';	توڑو;
		    حالت NUL:	--p;		توڑو;
		    // default: skip over illegal chars
		}
		++p;
		توڑو;

	    طےشدہ:
		*q++ = *p++;
	}
    }

    اگر (endp != تہی)
	*endp = p;

    واپس نتیجہ;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    ساکن خالی
nb_partialremove(linenr_T lnum, colnr_T پہلا, colnr_T آخری)
{
    char_u *oldtext, *newtext;
    صحیح oldlen;
    صحیح lastbyte = آخری;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    اگر (پہلا >= (colnr_T)oldlen || oldlen == 0)  // just in case
	واپس;
    اگر (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - پہلا));
    اگر (newtext == تہی)
	واپس;

    mch_memmove(newtext, oldtext, پہلا);
    STRMOVE(newtext + پہلا, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    ساکن خالی
nb_joinlines(linenr_T پہلا, linenr_T other)
{
    صحیح len_پہلا, len_other;
    char_u *p;

    len_پہلا = ml_get_len(پہلا);
    len_other = ml_get_len(other);
    p = alloc(len_پہلا + len_other + 1);
    اگر (p == تہی)
	واپس;

    mch_memmove(p, ml_get(پہلا), len_پہلا);
    mch_memmove(p + len_پہلا, ml_get(other), len_other + 1);
    ml_replace(پہلا, p, FALSE);
}

#تعریف_کرو SKIP_STOP 2
#تعریف_کرو streq(a,b) (strcmp(a,b) == 0)

/*
 * Do the actual processing of a single netbeans command or function.
 * The difference between a command and function is that a function
 * gets a response (it's required) but a command does not.
 * For arguments see comment for nb_parse_cmd().
 */
    ساکن صحیح
nb_do_cmd(
    صحیح		bufno,
    char_u	*cmd,
    صحیح		func,
    صحیح		cmdno,
    char_u	*دلائل)	    // points to space before arguments or NUL
{
    صحیح		do_update = 0;
    طویل	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    ساکن صحیح	skip = 0;
    صحیح		retval = OK;
    حرف	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (حرف *)دلائل));

    اگر (func)
    {
// =====================================================================
	اگر (streq((حرف *)cmd, "getModified"))
	{
	    اگر (buf == تہی || buf->bufp == تہی)
		// Return the number of buffers that are modified.
		nb_جواب_nr(cmdno, (طویل)گنتی_بدلا_buffers());
	    ورنہ
		// Return whether the buffer is modified.
		nb_جواب_nr(cmdno, (طویل)(buf->bufp->b_بدلا
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_جواب_nr(cmdno, (طویل)گنتی_بدلا_buffers());
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "getCursor"))
	{
	    char_u متن[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((حرف *)متن, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (طویل)curwin->w_cursor.lnum,
		    (صحیح)curwin->w_cursor.ستون,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_جواب_متن(cmdno, متن);
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "getAnno"))
	{
	    طویل linenum = 0;
#اگر_تعریف_شدہ FEAT_SIGNS
	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_getanno));
		retval = FAIL;
	    }
	    ورنہ
	    {
		صحیح ser_عدد;

		cp = (حرف *)دلائل;
		ser_عدد = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (طویل)buf_findsign(buf->bufp, ser_عدد, تہی);
	    }
#اگر_ختم
	    nb_جواب_nr(cmdno, linenum);
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "getLength"))
	{
	    طویل len = 0;

	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_getlength));
		retval = FAIL;
	    }
	    ورنہ
	    {
		len = get_buf_سائز(buf->bufp);
	    }
	    nb_جواب_nr(cmdno, len);
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "getText"))
	{
	    طویل	len;
	    linenr_T	nlines;
	    char_u	*متن = تہی;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*سطر;

	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_gettext));
		retval = FAIL;
	    }
	    ورنہ
	    {
		len = get_buf_سائز(buf->bufp);
		nlines = buf->bufp->b_ml.ml_سطر_گنتی;
		متن = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		اگر (متن == تہی)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		ورنہ
		{
		    p = متن;
		    *p++ = '\"';
		    برائے (; lno <= nlines ; lno++)
		    {
			سطر = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			اگر (سطر != تہی)
			{
			    STRCPY(p, سطر);
			    p += STRLEN(سطر);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(سطر);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    اگر (متن == تہی)
		nb_جواب_متن(cmdno, (char_u *)"");
	    ورنہ
	    {
		nb_جواب_متن(cmdno, متن);
		vim_free(متن);
	    }
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "remove"))
	{
	    طویل گنتی;
	    pos_T پہلا, آخری;
	    pos_T *pos;
	    pos_T *اگلا;
	    linenr_T del_from_lnum, del_مقصد_lnum;  // lines to be deleted as a whole
	    صحیح oldFire = netbeansFireChanges;
	    صحیح oldSuppress = netbeans_Suppress_No_سطریں;
	    صحیح was_بدلا_2;

	    اگر (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (حرف *) cmd));
		nb_جواب_nil(cmdno);
		واپس OK;
	    }

	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_remove));
		retval = FAIL;
	    }
	    ورنہ
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_سطریں = TRUE;

		nb_سیٹ_curbuf(buf->bufp);
		was_بدلا_2 = buf->bufp->b_بدلا;
		cp = (حرف *)دلائل;
		off = strtol(cp, &cp, 10);
		گنتی = strtol(cp, &cp, 10);
		دلائل = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		اگر (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_جواب_متن(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_سطریں = oldSuppress;
		    واپس FAIL;
		}
		پہلا = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      پہلا.lnum, پہلا.ستون));
		pos = off2pos(buf->bufp, off+گنتی-1);
		اگر (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_جواب_متن(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_سطریں = oldSuppress;
		    واپس FAIL;
		}
		آخری = *pos;
		nbdebug(("    LAST POS: line %ld, col %d\n",
							آخری.lnum, آخری.ستون));
		del_from_lnum = پہلا.lnum;
		del_مقصد_lnum = آخری.lnum;
		do_update = 1;

		// Get the position of the first byte after the deleted
		// section.  "next" is NULL when deleting to the end of the
		// file.
		اگلا = off2pos(buf->bufp, off + گنتی);

		// Remove part of the first line.
		اگر (پہلا.ستون != 0
				|| (اگلا != تہی && پہلا.lnum == اگلا->lnum))
		{
		    اگر (پہلا.lnum != آخری.lnum
			    || (اگلا != تہی && پہلا.lnum != اگلا->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(پہلا.lnum, پہلا.ستون,
							     (colnr_T)MAXCOL);
			اگر (پہلا.lnum == آخری.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(پہلا.lnum, اگلا->lnum);
			    del_مقصد_lnum = اگلا->lnum;
			}
		    }
		    ورنہ
		    {
			// remove within one line
			nb_partialremove(پہلا.lnum, پہلا.ستون, آخری.ستون);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		اگر (پہلا.lnum != آخری.lnum && اگلا != تہی
			&& اگلا->ستون != 0 && آخری.lnum == اگلا->lnum)
		{
		    nb_partialremove(آخری.lnum, 0, آخری.ستون);
		    اگر (del_from_lnum > پہلا.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(پہلا.lnum, آخری.lnum);
		    }
		    ورنہ
			// First line is deleted as a whole, keep the last
			// line.
			--del_مقصد_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		اگر (پہلا.lnum != آخری.lnum && del_from_lnum > پہلا.lnum
			&& اگلا != تہی && آخری.lnum != اگلا->lnum)
		{
		    nb_joinlines(پہلا.lnum, اگلا->lnum);
		    del_مقصد_lnum = اگلا->lnum;
		}

		// Delete whole lines if there are any.
		اگر (del_مقصد_lnum >= del_from_lnum)
		{
		    صحیح i;

		    // delete signs from the lines being deleted
		    برائے (i = del_from_lnum; i <= del_مقصد_lnum; i++)
		    {
			صحیح id = buf_findsign_id(buf->bufp, (linenr_T)i, تہی);
			اگر (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, تہی);
			}
			ورنہ
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_مقصد_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.ستون = 0;
		    del_سطریں(del_مقصد_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = پہلا;
		check_cursor_lnum();
		buf->bufp->b_بدلا = was_بدلا_2; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_سطریں = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_جواب_nil(cmdno);
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "insert"))
	{
	    char_u	*مقصد_free;

	    اگر (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (حرف *) cmd));
		nb_جواب_nil(cmdno);
		واپس OK;
	    }

	    // get offset
	    cp = (حرف *)دلائل;
	    off = strtol(cp, &cp, 10);
	    دلائل = (char_u *)cp;

	    // get text to be inserted
	    دلائل = skipwhite(دلائل);
	    دلائل = مقصد_free = (char_u *)nb_unquote(دلائل, تہی);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_داخل_کرو));
		retval = FAIL;
	    }
	    ورنہ اگر (دلائل != تہی)
	    {
		صحیح	ff_detected = EOL_UNKNOWN;
		صحیح	buf_was_خالی_خانے = (buf->bufp->b_ml.ml_flags & ML_خالی_خانے);
		size_t	len = 0;
		صحیح	added = 0;
		صحیح	oldFire = netbeansFireChanges;
		صحیح	old_b_بدلا;
		char_u	*nlp;
		linenr_T lnum;
		linenr_T lnum_آغاز;
		pos_T	*pos;

		netbeansFireChanges = 0;

		// Jump to the buffer where we insert.  After this "curbuf"
		// can be used.
		nb_سیٹ_curbuf(buf->bufp);
		old_b_بدلا = curbuf->b_بدلا;

		// Convert the specified character offset into a lnum/col
		// position.
		pos = off2pos(curbuf, off);
		اگر (pos != تہی)
		{
		    اگر (pos->lnum <= 0)
			lnum_آغاز = 1;
		    ورنہ
			lnum_آغاز = pos->lnum;
		}
		ورنہ
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    اگر (buf_was_خالی_خانے)
			lnum_آغاز = 1;	    // above empty line
		    ورنہ
			lnum_آغاز = curbuf->b_ml.ml_سطر_گنتی + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_آغاز;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		جب (*دلائل != NUL)
		{
		    nlp = vim_strchr(دلائل, '\n');
		    اگر (nlp == تہی)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(دلائل);
		    }
		    ورنہ
		    {
			len = nlp - دلائل;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			اگر (nlp > دلائل && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			ورنہ
			    ff_detected = EOL_UNIX;
		    }
		    دلائل[len] = NUL;

		    اگر (lnum == lnum_آغاز
			    && ((pos != تہی && pos->ستون > 0)
				|| (lnum == 1 && buf_was_خالی_خانے)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			صحیح	ستون = pos == تہی ? 0 : pos->ستون;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			اگر (newline != تہی)
			{
			    mch_memmove(newline, oldline, (size_t)ستون);
			    newline[ستون] = NUL;
			    STRCAT(newline, دلائل);
			    STRCAT(newline, oldline + ستون);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    ورنہ
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), دلائل,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    اگر (nlp == تہی)
			توڑو;
		    ++lnum;
		    دلائل = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_سطریں_mark(lnum_آغاز - 1, (طویل)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		اگر (buf_was_خالی_خانے)
		{
		    اگر (ff_detected == EOL_UNKNOWN)
#اگر defined(MSWIN)
			ff_detected = EOL_DOS;
#ورنہ
			ff_detected = EOL_UNIX;
#اگر_ختم
		    سیٹ_fileformat(ff_detected, OPT_LOCAL);
		    curbuf->b_آغاز_ffc = *curbuf->b_p_ff;
		}

		/*
		 * XXX - GRP - Is the next line right? If I've inserted
		 * text the buffer has been updated but not written. Will
		 * netbeans guarantee to write it? Even if I do a :q! ?
		 */
		curbuf->b_بدلا = old_b_بدلا; // logically unchanged
		netbeansFireChanges = oldFire;

		// Undo info is invalid now...
		u_clearallandblockfree(curbuf);
	    }
	    vim_free(مقصد_free);
	    nb_جواب_nil(cmdno); // or !error
	}
	ورنہ
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_جواب_nil(cmdno);
	    retval = FAIL;
	}
    }
    ورنہ // Not a function; no reply required.
    {
// =====================================================================
	اگر (streq((حرف *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    اگر (buf == تہی)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_create));
		واپس FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, تہی, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->داخل_کرو_مکمل = FALSE;
#اگر defined(FEAT_MENU) && defined(FEAT_GUI)
	    اگر (gui.اندر_use)
		gui_update_menus(0);
#اگر_ختم
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "insertDone"))
	{
	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    ورنہ
	    {
		buf->bufp->b_آغاز_eol = *دلائل == 'T';
		buf->داخل_کرو_مکمل = TRUE;
		دلائل += 2;
		buf->bufp->b_p_ro = *دلائل == 'T';
		print_read_پیغام(buf);
	    }
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "saveDone"))
	{
	    طویل savedChars = atol((حرف *)دلائل);

	    اگر (buf == تہی || buf->bufp == تہی)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    ورنہ
		print_save_پیغام(buf, savedChars);
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "startDocumentListen"))
	{
	    اگر (buf == تہی)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_startdocumentlisten));
		واپس FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "stopDocumentListen"))
	{
	    اگر (buf == تہی)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_stopdocumentlisten));
		واپس FAIL;
	    }
	    buf->fireChanges = 0;
	    اگر (buf->bufp != تہی && buf->bufp->b_was_netbeans_file)
	    {
		اگر (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_گمشدہ_for_بفر_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_گمشدہ_for_بفر_nr),
							   buf->bufp->b_fnum);
		}
		ورنہ
		{
		    // NetBeans uses stopDocumentListen when it stops editing
		    // a file.  It then expects the buffer in Vim to
		    // disappear.
		    do_bufdel(DOBUF_DEL, (char_u *)"", 1,
				  buf->bufp->b_fnum, buf->bufp->b_fnum, TRUE);
		    CLEAR_اشارہ_گر(buf);
		}
	    }
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "setTitle"))
	{
	    اگر (buf == تہی)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_settitle));
		واپس FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(دلائل, تہی);
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "initDone"))
	{
	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_initdone));
		واپس FAIL;
	    }
	    do_update = 1;
	    buf->init_مکمل = TRUE;
	    nb_سیٹ_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_کلید_قطار();
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "setBufferNumber")
		|| streq((حرف *)cmd, "putBufferNumber"))
	{
	    char_u	*راستہ;
	    buf_T	*bufp;

	    اگر (buf == تہی)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_setbuffernumber));
		واپس FAIL;
	    }
	    راستہ = (char_u *)nb_unquote(دلائل, تہی);
	    اگر (راستہ == تہی)
		واپس FAIL;
	    bufp = buflist_findname(راستہ);
	    vim_free(راستہ);
	    اگر (bufp == تہی)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", دلائل));
		semsg(_(e_file_str_not_ملا_اندر_setbuffernumber), دلائل);
		واپس FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_نمبر = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    اگر (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    ورنہ
	    {
		buf->init_مکمل = TRUE;

		// handle any postponed key commands
		handle_کلید_قطار();
	    }

// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "setFullName"))
	{
	    اگر (buf == تہی)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_setfullname));
		واپس FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(دلائل, تہی);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#اگر defined(FEAT_MENU) && defined(FEAT_GUI)
	    اگر (gui.اندر_use)
		gui_update_menus(0);
#اگر_ختم
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "editFile"))
	{
	    اگر (buf == تہی)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_editfile));
		واپس FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(دلائل, تہی);
	    do_ecmd(0, (char_u *)buf->displayname, تہی, تہی, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->init_مکمل = TRUE;
	    do_update = 1;
	    maketitle();
#اگر defined(FEAT_MENU) && defined(FEAT_GUI)
	    اگر (gui.اندر_use)
		gui_update_menus(0);
#اگر_ختم
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "setVisible"))
	{
	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		اگر (p_verbose > 0)
		    emsg(_(e_invalid_بفر_identifier_اندر_setvisible));
		واپس FAIL;
	    }
	    اگر (streq((حرف *)دلائل, "T") && buf->bufp != curbuf)
	    {
		exarg_T exarg;
		CLEAR_میدان(exarg);
		exarg.cmd = (char_u *)"goto";
		exarg.forceit = FALSE;
		exarg.cmdidx = CMD_USER;
		dosetvisible = TRUE;
		goto_بفر(&exarg, DOBUF_پہلا, FORWARD, buf->bufp->b_fnum);
		do_update = 1;
		dosetvisible = FALSE;

#اگر_تعریف_شدہ FEAT_GUI
		// Side effect!!!.
		اگر (gui.اندر_use)
		    gui_mch_سیٹ_foreground();
#اگر_ختم
	    }
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "raise"))
	{
#اگر_تعریف_شدہ FEAT_GUI
	    // Bring gvim to the foreground.
	    اگر (gui.اندر_use)
		gui_mch_سیٹ_foreground();
#اگر_ختم
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "setModified"))
	{
	    صحیح پچھلا_b_بدلا;

	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		اگر (p_verbose > 0)
		    emsg(_(e_invalid_بفر_identifier_اندر_setmodified));
		واپس FAIL;
	    }
	    پچھلا_b_بدلا = buf->bufp->b_بدلا;
	    اگر (streq((حرف *)دلائل, "T"))
		buf->bufp->b_بدلا = TRUE;
	    ورنہ
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		اگر (buf->bufp->b_ffname != تہی
			&& mch_stat((حرف *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_وقت(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_بدلا = FALSE;
	    }
	    buf->modified = buf->bufp->b_بدلا;
	    اگر (پچھلا_b_بدلا != buf->bufp->b_بدلا)
	    {
		check_حالت(buf->bufp);
		redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#اگر_ختم
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "setModtime"))
	{
	    اگر (buf == تہی || buf->bufp == تہی)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    ورنہ
	    {
		buf->bufp->b_mtime = atoi((حرف *)دلائل);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "setReadOnly"))
	{
	    اگر (buf == تہی || buf->bufp == تہی)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    ورنہ اگر (streq((حرف *)دلائل, "T"))
		buf->bufp->b_p_ro = TRUE;
	    ورنہ
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "showBalloon"))
	{
#اگر defined(FEAT_BEVAL_GUI)
	    ساکن حرف	*متن = تہی;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    اگر (balloonEval != تہی)
	    {
		vim_free(متن);
		متن = nb_unquote(دلائل, تہی);
		اگر (متن != تہی)
		    gui_mch_post_balloon(balloonEval, (char_u *)متن);
	    }
#اگر_ختم
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "setDot"))
	{
	    pos_T *pos;
#اگر_تعریف_شدہ NBDEBUG
	    char_u *s;
#اگر_ختم

	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_setdot));
		واپس FAIL;
	    }

	    nb_سیٹ_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    اگر (VIsual_فعال)
		انتہا_visual_منوال();
#اگر_تعریف_شدہ NBDEBUG
	    s = دلائل;
#اگر_ختم
	    pos = get_off_or_lnum(buf->bufp, &دلائل);
	    اگر (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#اگر_تعریف_شدہ FEAT_FOLDING
		foldOpenCursor();
#اگر_ختم
	    }
	    ورنہ
	    {
		nbdebug(("    BAD POSITION in setDot: %s\n", s));
	    }

	    // gui_update_cursor(TRUE, FALSE);
	    // update_curbuf(UPD_NOT_VALID);
	    update_topline();		// scroll to show the line
	    update_screen(UPD_VALID);
	    setcursor();
	    cursor_on();
	    برآمد_flush_cursor(TRUE, FALSE);

	    // Quit a hit-return or more prompt.
	    اگر (State == منوال_HITRETURN || State == منوال_ASKMORE)
	    {
#اگر_تعریف_شدہ FEAT_GUI_GTK
		اگر (gui.اندر_use && gtk_main_level() > 0)
		    gtk_main_quit();
#اگر_ختم
	    }
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "close"))
	{
#اگر_تعریف_شدہ NBDEBUG
	    حرف *name = "<NONE>";
#اگر_ختم

	    اگر (buf == تہی)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_close));
		واپس FAIL;
	    }

#اگر_تعریف_شدہ NBDEBUG
	    اگر (buf->displayname != تہی)
		name = buf->displayname;
#اگر_ختم
	    اگر (buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		اگر (p_verbose > 0)
		    emsg(_(e_invalid_بفر_identifier_اندر_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#اگر_تعریف_شدہ FEAT_GUI
	    need_mouse_correct = TRUE;
#اگر_ختم
	    اگر (buf->bufp != تہی)
		do_بفر(DOBUF_WIPE, DOBUF_پہلا, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = تہی;
	    buf->init_مکمل = FALSE;
	    do_update = 1;
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "defineAnnoType"))
	{
#اگر_تعریف_شدہ FEAT_SIGNS
	    صحیح type_عدد;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    صحیح parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    اگر (buf == تہی)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_defineannotype));
		واپس FAIL;
	    }

	    cp = (حرف *)دلائل;
	    type_عدد = strtol(cp, &cp, 10);
	    دلائل = (char_u *)cp;
	    دلائل = skipwhite(دلائل);
	    typeName = (char_u *)nb_unquote(دلائل, &دلائل);
	    دلائل = skipwhite(دلائل + 1);
	    tooltip = (char_u *)nb_unquote(دلائل, &دلائل);
	    دلائل = skipwhite(دلائل + 1);

	    p = (char_u *)nb_unquote(دلائل, &دلائل);
	    glyphFile = vim_strsave_escaped(p, escape_chars);
	    vim_free(p);

	    دلائل = skipwhite(دلائل + 1);
	    p = skiptowhite(دلائل);
	    اگر (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(دلائل);
	    bg = vim_strsave(p);
	    اگر (STRLEN(fg) > MAX_COLOR_لمبائی || STRLEN(bg) > MAX_COLOR_لمبائی)
	    {
		emsg(_(e_highlighting_color_name_too_long_اندر_define_Anno_Type));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    ورنہ اگر (!nb_is_محفوظ_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_محفوظ_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_محفوظ_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_اندر_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    ورنہ اگر (typeName != تہی && tooltip != تہی && glyphFile != تہی)
		addsigntype(buf, type_عدد, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    اگر (parse_error)
		واپس FAIL;

#اگر_ختم
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "addAnno"))
	{
#اگر_تعریف_شدہ FEAT_SIGNS
	    صحیح ser_عدد;
	    صحیح local_Type_عدد;
	    صحیح type_عدد;
	    pos_T *pos;

	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_بفر_identifier_اندر_addanno));
		واپس FAIL;
	    }

	    do_update = 1;

	    cp = (حرف *)دلائل;
	    ser_عدد = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_عدد = strtol(cp, &cp, 10);
	    دلائل = (char_u *)cp;
	    type_عدد = mapsigntype(buf, local_Type_عدد);

	    pos = get_off_or_lnum(buf->bufp, &دلائل);

	    cp = (حرف *)دلائل;
	    vim_ignored = (صحیح)strtol(cp, &cp, 10);
	    دلائل = (char_u *)cp;
# ifdef NBDEBUG
	    اگر (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    اگر (ser_عدد >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		واپس FAIL;
	    }
	    اگر (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_عدد, pos->lnum, type_عدد, buf->bufp->b_fnum);
		اگر (type_عدد == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_عدد,
						       buf->bufp->b_fnum);
	    }
#اگر_ختم
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "removeAnno"))
	{
#اگر_تعریف_شدہ FEAT_SIGNS
	    صحیح ser_عدد;

	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		واپس FAIL;
	    }
	    do_update = 1;
	    cp = (حرف *)دلائل;
	    ser_عدد = strtol(cp, &cp, 10);
	    دلائل = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_عدد, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#اگر_ختم
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "moveAnnoToFront"))
	{
#اگر_تعریف_شدہ FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#اگر_ختم
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "guard") || streq((حرف *)cmd, "unguard"))
	{
	    صحیح len;
	    pos_T پہلا;
	    pos_T آخری;
	    pos_T *pos;
	    صحیح un = (cmd[0] == 'u');
	    ساکن صحیح guardId = GUARDEDOFFSET;

	    اگر (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (حرف *) cmd));
		واپس OK;
	    }

	    nb_init_graphics();

	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		واپس FAIL;
	    }
	    nb_سیٹ_curbuf(buf->bufp);
	    cp = (حرف *)دلائل;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, تہی, 10);
	    دلائل = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    اگر (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    ورنہ
	    {
		پہلا = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		اگر (pos != تہی && pos->ستون == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		اگر (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		ورنہ
		{
		    طویل lnum;
		    آخری = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     پہلا.lnum, پہلا.ستون,
			     آخری.lnum, آخری.ستون));
#اگر_تعریف_شدہ FEAT_SIGNS
		    برائے (lnum = پہلا.lnum; lnum <= آخری.lnum; lnum++)
		    {
			اگر (un)
			{
			    // never used
			}
			ورنہ
			{
			    اگر (buf_findsigntype_id(buf->bufp, lnum,
				GUARDED) == 0)
			    {
				coloncmd(
				    ":sign place %d line=%ld name=%d buffer=%d",
				     guardId++, lnum, GUARDED,
				     buf->bufp->b_fnum);
			    }
			}
		    }
#اگر_ختم
		    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
		}
	    }
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "startAtomic"))
	{
	    اندر_Atomic = 1;
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "endAtomic"))
	{
	    اندر_Atomic = 0;
	    اگر (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		واپس FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    اگر (buf_Is_بدلا(buf->bufp))
	    {
		// Only write if the buffer can be written.
		اگر (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != تہی
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    سیٹ_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    اگر (!bufref_valid(&bufref))
			buf->bufp = تہی;
		}
	    }
	    ورنہ
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "netbeansBuffer"))
	{
	    اگر (buf == تہی || buf->bufp == تہی)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		واپس FAIL;
	    }
	    اگر (*دلائل == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    ورنہ
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "specialKeys"))
	{
	    special_کلیدیں(دلائل);
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	ورنہ اگر (streq((حرف *)cmd, "version"))
	{
	    // not used yet
	}
	ورنہ
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    اگر (اندر_Atomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    اگر (buf != تہی && buf->init_مکمل && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	برآمد_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	اگر (State == منوال_HITRETURN || State == منوال_ASKMORE)
	{
#اگر_تعریف_شدہ FEAT_GUI_GTK
	    اگر (gui.اندر_use && gtk_main_level() > 0)
		gtk_main_quit();
#اگر_ختم
	}
    }

    واپس retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    ساکن خالی
nb_سیٹ_curbuf(buf_T *buf)
{
    اگر (curbuf == buf)
	واپس;

    اگر (buf_jump_open_win(buf) != تہی)
	واپس;
    اگر ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != تہی)
	واپس;
    سیٹ_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    ساکن خالی
coloncmd(حرف *cmd, ...)
{
    حرف buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, حجم(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, تہی, تہی, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    برآمد_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    ساکن خالی
special_کلیدیں(char_u *دلائل)
{
    حرف *save_str = nb_unquote(دلائل, تہی);
    حرف *tok = strtok(save_str, " ");
    حرف *sep;
#تعریف_کرو KEYBUFLEN 64
    حرف keybuf[KEYBUFLEN];
    حرف cmdbuf[256];

    جب (tok != تہی)
    {
	صحیح i = 0;

	اگر ((sep = strchr(tok, '-')) != تہی)
	{
	    *sep = NUL;
	    جب (*tok && i + 2 < KEYBUFLEN)
	    {
		انتخاب (*tok)
		{
		    حالت 'A':
		    حالت 'M':
		    حالت 'C':
		    حالت 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			توڑو;
		}
		tok++;
	    }
	    tok++;
	}

	اگر (لڑی_لمبائی(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    صحیح محفوظ_2 = TRUE;
	    برائے (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		اگر (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    محفوظ_2 = FALSE;
		    توڑو;
		}
	    }
	    اگر (محفوظ_2)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, حجم(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, منوال_معمول, FALSE);
	    }
	}
	tok = strtok(تہی, " ");
    }
    vim_free(save_str);
}

    خالی
ex_nbclose(exarg_T *eap غیر_مستعمل)
{
    netbeans_close();
}

    خالی
ex_nbkey(exarg_T *eap)
{
    (خالی)netbeans_keystring(eap->arg);
}

    خالی
ex_nbstart(
    exarg_T	*eap)
{
#اگر_تعریف_شدہ FEAT_GUI
# اگر !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    اگر (gui.اندر_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	واپس;
    }
# endif
#اگر_ختم
    netbeans_open((حرف *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    ساکن خالی
nb_init_graphics(خالی)
{
    ساکن صحیح did_init = FALSE;

    اگر (did_init)
	واپس;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    ساکن خالی
netbeans_keyname(صحیح کلید, حرف *buf)
{
    حرف *name = 0;
    حرف namebuf[2];
    صحیح ctrl  = 0;
    صحیح سرکاؤ = 0;
    صحیح alt   = 0;

    اگر (mod_نقاب & MOD_نقاب_CTRL)
	ctrl = 1;
    اگر (mod_نقاب & MOD_نقاب_سرکاؤ)
	سرکاؤ = 1;
    اگر (mod_نقاب & MOD_نقاب_ALT)
	alt = 1;


    انتخاب (کلید)
    {
	حالت K_F1:		name = "F1";		توڑو;
	حالت K_S_F1:	name = "F1";	سرکاؤ = 1;	توڑو;
	حالت K_F2:		name = "F2";		توڑو;
	حالت K_S_F2:	name = "F2";	سرکاؤ = 1;	توڑو;
	حالت K_F3:		name = "F3";		توڑو;
	حالت K_S_F3:	name = "F3";	سرکاؤ = 1;	توڑو;
	حالت K_F4:		name = "F4";		توڑو;
	حالت K_S_F4:	name = "F4";	سرکاؤ = 1;	توڑو;
	حالت K_F5:		name = "F5";		توڑو;
	حالت K_S_F5:	name = "F5";	سرکاؤ = 1;	توڑو;
	حالت K_F6:		name = "F6";		توڑو;
	حالت K_S_F6:	name = "F6";	سرکاؤ = 1;	توڑو;
	حالت K_F7:		name = "F7";		توڑو;
	حالت K_S_F7:	name = "F7";	سرکاؤ = 1;	توڑو;
	حالت K_F8:		name = "F8";		توڑو;
	حالت K_S_F8:	name = "F8";	سرکاؤ = 1;	توڑو;
	حالت K_F9:		name = "F9";		توڑو;
	حالت K_S_F9:	name = "F9";	سرکاؤ = 1;	توڑو;
	حالت K_F10:		name = "F10";		توڑو;
	حالت K_S_F10:	name = "F10";	سرکاؤ = 1;	توڑو;
	حالت K_F11:		name = "F11";		توڑو;
	حالت K_S_F11:	name = "F11";	سرکاؤ = 1;	توڑو;
	حالت K_F12:		name = "F12";		توڑو;
	حالت K_S_F12:	name = "F12";	سرکاؤ = 1;	توڑو;
	طےشدہ:
			اگر (کلید >= ' ' && کلید <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = کلید;
			    namebuf[1] = NUL;
			}
			ورنہ
			    name = "X";
			توڑو;
    }

    buf[0] = '\0';
    اگر (ctrl)
	strcat(buf, "C");
    اگر (سرکاؤ)
	strcat(buf, "S");
    اگر (alt)
	strcat(buf, "M"); // META
    اگر (ctrl || سرکاؤ || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#اگر defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    خالی
netbeans_beval_cb(
	BalloonEval	*beval,
	صحیح		 state غیر_مستعمل)
{
    win_T	*wp;
    char_u	*متن;
    linenr_T	lnum;
    صحیح		ستون;
    حرف	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    اگر (!can_use_beval() || !NETBEANS_OPEN)
	واپس;

    اگر (get_beval_info(beval, TRUE, &wp, &lnum, &متن, &ستون) != OK)
	واپس;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    اگر (متن != تہی && متن[0] != NUL && STRLEN(متن) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	اگر (buf != تہی)
	{
	    p = nb_quote(متن);
	    اگر (p != تہی)
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
    vim_free(متن);
}
#اگر_ختم

/*
 * Return TRUE when the netbeans connection is active.
 */
    صحیح
netbeans_فعال(خالی)
{
    واپس NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    خالی
netbeans_open(حرف *params, صحیح doabort)
{
    حرف *cmd = "0:startupDone=0\n";

    اگر (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	واپس;
    }

    اگر (netbeans_connect(params, doabort) != OK)
	واپس;

    nbdebug(("EVT: %s", cmd));
    nb_send(cmd, "netbeans_startup_done");

    // update the screen after having added the gutter
    بدلا_کھڑکی_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    برآمد_flush_cursor(TRUE, FALSE);
}

/*
 * Tell netbeans that we're exiting. This should be called right
 * before calling exit.
 */
    خالی
netbeans_send_disconnect(خالی)
{
    حرف buf[128];

    اگر (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#اگر defined(FEAT_EVAL)
    صحیح
سیٹ_ref_اندر_nb_channel(صحیح copyID)
{
    صحیح abort = FALSE;
    typval_T tv;

    اگر (nb_channel == تہی)
	واپس FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = سیٹ_ref_اندر_شے(&tv, copyID, تہی, تہی, تہی);
    واپس abort;
}
#اگر_ختم

#اگر defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    خالی
netbeans_چوکھٹا_moved(صحیح new_x, صحیح new_y)
{
    حرف buf[128];

    اگر (!NETBEANS_OPEN)
	واپس;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (صحیح)Columns, (صحیح)صفیں, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#اگر_ختم

/*
 * Tell netbeans the user opened or activated a file.
 */
    خالی
netbeans_file_activated(buf_T *bufp)
{
    صحیح bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    حرف    بفر[2*MAXPATHL];
    char_u  *q;

    اگر (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	واپس;

    q = nb_quote(bufp->b_ffname);
    اگر (q == تہی)
	واپس;
    اگر (bp == تہی)
    {
	vim_free(q);
	واپس;
    }

    vim_snprintf(بفر, حجم(بفر),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (حرف *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", بفر));

    nb_send(بفر, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    خالی
netbeans_file_opened(buf_T *bufp)
{
    صحیح bufno = nb_getbufno(bufp);
    حرف    بفر[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    صحیح	    bnum;

    اگر (!NETBEANS_OPEN)
	واپس;

    q = nb_quote(bufp->b_ffname);
    اگر (q == تہی)
	واپس;
    اگر (bp != تہی)
	bnum = bufno;
    ورنہ
	bnum = 0;

    vim_snprintf(بفر, حجم(بفر), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (حرف *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", بفر));

    nb_send(بفر, "netbeans_file_opened");
    اگر (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	آخری_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    خالی
netbeans_file_killed(buf_T *bufp)
{
    صحیح		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    حرف	بفر[2*MAXPATHL];

    اگر (!NETBEANS_OPEN || bufno == -1)
	واپس;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(بفر, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", بفر));

    nb_send(بفر, "netbeans_file_killed");

    اگر (nbbuf != تہی)
	nbbuf->bufp = تہی;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    ساکن nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, صحیح *bufnop)
{
    صحیح		bufno;
    nbbuf_T	*nbbuf;

    اگر (!NETBEANS_OPEN || !netbeansFireChanges)
	واپس تہی;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    اگر (bufno <= 0)
	واپس تہی;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    اگر (nbbuf != تہی && !nbbuf->fireChanges)
	واپس تہی;		// changes in this buffer are not reported

    *bufnop = bufno;
    واپس nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    خالی
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	ستون,
    char_u	*txt,
    صحیح		newlen)
{
    char_u	*buf;
    صحیح		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    طویل	off;
    char_u	*p;
    char_u	*newtxt;

    اگر (!NETBEANS_OPEN)
	واپس;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    اگر (nbbuf == تہی)
	واپس;

    // Don't mark as modified for initial read
    اگر (nbbuf->داخل_کرو_مکمل)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.ستون = ستون;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    اگر (p != تہی)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((حرف *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((حرف *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    خالی
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	ستون,
    طویل	len)
{
    char_u	buf[128];
    صحیح		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    طویل	off;

    اگر (!NETBEANS_OPEN)
	واپس;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    اگر (nbbuf == تہی)
	واپس;

    اگر (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	واپس;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.ستون = ستون;

    off = pos2off(bufp, &pos);

    sprintf((حرف *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((حرف *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    خالی
netbeans_unmodified(buf_T *bufp غیر_مستعمل)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    خالی
netbeans_button_release(صحیح button)
{
    حرف	buf[128];
    صحیح		bufno;

    اگر (!NETBEANS_OPEN)
	واپس;

    bufno = nb_getbufno(curbuf);

    اگر (bufno < 0 || curwin == تہی || curwin->w_بفر != curbuf)
	واپس;

    صحیح ستون = mouse_ستون - curwin->w_wincol
	- ((curwin->w_p_nu || curwin->w_p_rnu) ? 9 : 1);
    طویل off = pos2off(curbuf, &curwin->w_cursor);

    // sync the cursor position
    sprintf(buf, "%d:newDotAndMark=%d %ld %ld\n", bufno, r_cmdno, off, off);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release[newDotAndMark]");

    sprintf(buf, "%d:buttonRelease=%d %d %ld %d\n", bufno, r_cmdno,
	    button, (طویل)curwin->w_cursor.lnum, ستون);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release");
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key code.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    صحیح
netbeans_keycommand(صحیح کلید)
{
    حرف	کلید_Name[60];

    netbeans_keyname(کلید, کلید_Name);
    واپس netbeans_keystring((char_u *)کلید_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    ساکن صحیح
netbeans_keystring(char_u *کلید_Name)
{
    حرف	buf[2*MAXPATHL];
    صحیح		bufno = nb_getbufno(curbuf);
    طویل	off;
    char_u	*q;

    اگر (!NETBEANS_OPEN)
	واپس TRUE;

    اگر (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == تہی ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	اگر (q == تہی)
	    واپس TRUE;
	vim_snprintf(buf, حجم(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	اگر (curbuf->b_ffname != تہی)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(کلید_Name);
	واپس FALSE;
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
    vim_snprintf(buf, حجم(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, کلید_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, حجم(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, کلید_Name,
		off, (طویل)curwin->w_cursor.lnum, (طویل)curwin->w_cursor.ستون);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    واپس TRUE;
}


/*
 * Send a save event to netbeans.
 */
    خالی
netbeans_save_بفر(buf_T *bufp)
{
    char_u	buf[64];
    صحیح		bufno;
    nbbuf_T	*nbbuf;

    اگر (!NETBEANS_OPEN)
	واپس;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    اگر (nbbuf == تہی)
	واپس;

    nbbuf->modified = 0;

    sprintf((حرف *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((حرف *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    خالی
netbeans_deleted_all_سطریں(buf_T *bufp)
{
    char_u	buf[64];
    صحیح		bufno;
    nbbuf_T	*nbbuf;

    اگر (!NETBEANS_OPEN)
	واپس;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    اگر (nbbuf == تہی)
	واپس;

    // Don't mark as modified for initial read
    اگر (nbbuf->داخل_کرو_مکمل)
	nbbuf->modified = 1;

    sprintf((حرف *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    صحیح
netbeans_is_guarded(linenr_T چوٹی, linenr_T bot)
{
    sign_اندراج_T	*p;
    صحیح			lnum;

    اگر (!NETBEANS_OPEN)
	واپس FALSE;

    FOR_ALL_SIGNS_اندر_BUF(curbuf, p)
	اگر (p->se_id >= GUARDEDOFFSET)
	    برائے (lnum = چوٹی + 1; lnum < bot; lnum++)
		اگر (lnum == p->se_lnum)
		    واپس TRUE;

    واپس FALSE;
}

#اگر defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    خالی
netbeans_draw_multisign_indicator(صحیح صف)
{
    صحیح i;
    صحیح y;
    صحیح x;

    اگر (!NETBEANS_OPEN)
	واپس;

    x = 0;
    y = صف * gui.char_اونچائی + 2;

    برائے (i = 0; i < gui.char_اونچائی - 3; i++)
	XDraw_نقطہ(gui.dpy, gui.wid, gui.متن_gc, x+2, y++);

    XDraw_نقطہ(gui.dpy, gui.wid, gui.متن_gc, x+0, y);
    XDraw_نقطہ(gui.dpy, gui.wid, gui.متن_gc, x+2, y);
    XDraw_نقطہ(gui.dpy, gui.wid, gui.متن_gc, x+4, y++);
    XDraw_نقطہ(gui.dpy, gui.wid, gui.متن_gc, x+1, y);
    XDraw_نقطہ(gui.dpy, gui.wid, gui.متن_gc, x+2, y);
    XDraw_نقطہ(gui.dpy, gui.wid, gui.متن_gc, x+3, y++);
    XDraw_نقطہ(gui.dpy, gui.wid, gui.متن_gc, x+2, y);
}
#اگر_ختم // FEAT_GUI_X11

#اگر defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    خالی
netbeans_draw_multisign_indicator(صحیح صف)
{
    صحیح i;
    صحیح y;
    صحیح x;
# اگر GTK_CHECK_نسخہ(3,0,0)
    cairo_t *cr = تہی;
# ورنہ
    GdkDrawable *drawable = gui.drawarea->کھڑکی_2;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    اگر (!NETBEANS_OPEN)
	واپس;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# ورنہ
    y = صف * gui.char_اونچائی + 2;
# endif

# اگر GTK_CHECK_نسخہ(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_اونچائی);
    cr = cairo_create(surf);
#  ورنہ
    cr = cairo_create(gui.surface);
#  endif
    cairo_سیٹ_ماخذ_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    برائے (i = 0; i < gui.char_اونچائی - 3; i++)
# اگر GTK_CHECK_نسخہ(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# ورنہ
    gdk_draw_نقطہ(drawable, gui.متن_gc, x+2, y++);
# endif

# اگر GTK_CHECK_نسخہ(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# ورنہ
    gdk_draw_نقطہ(drawable, gui.متن_gc, x+0, y);
    gdk_draw_نقطہ(drawable, gui.متن_gc, x+2, y);
    gdk_draw_نقطہ(drawable, gui.متن_gc, x+4, y++);
    gdk_draw_نقطہ(drawable, gui.متن_gc, x+1, y);
    gdk_draw_نقطہ(drawable, gui.متن_gc, x+2, y);
    gdk_draw_نقطہ(drawable, gui.متن_gc, x+3, y++);
    gdk_draw_نقطہ(drawable, gui.متن_gc, x+2, y);
# endif

# ifdef USE_GTK4
    cairo_fill(cr);
    gui_gtk4_جوڑو_multisign(surf, صف, 0, 5, gui.char_اونچائی);
# endif

# اگر GTK_CHECK_نسخہ(3,0,0)
    cairo_destroy(cr);
# endif
# ifdef USE_GTK4
    cairo_surface_destroy(surf);
# endif
}
#اگر_ختم // FEAT_GUI_GTK

/*
 * If the mouse is clicked in the gutter of a line with multiple
 * annotations, cycle through the set of signs.
 */
    خالی
netbeans_gutter_click(linenr_T lnum)
{
    sign_اندراج_T	*p;

    اگر (!NETBEANS_OPEN)
	واپس;

    FOR_ALL_SIGNS_اندر_BUF(curbuf, p)
    {
	اگر (p->se_lnum == lnum && p->se_اگلا && p->se_اگلا->se_lnum == lnum)
	{
	    sign_اندراج_T *دم;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    اگر (p->se_پچھلا)
		p->se_پچھلا->se_اگلا = p->se_اگلا;
	    ورنہ
		curbuf->b_signlist = p->se_اگلا;
	    p->se_اگلا->se_پچھلا = p->se_پچھلا;
	    // now find end of sublist and insert p
	    برائے (دم = p->se_اگلا;
		  دم->se_اگلا && دم->se_اگلا->se_lnum == lnum
				       && دم->se_اگلا->se_id < GUARDEDOFFSET;
		  دم = دم->se_اگلا)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_اگلا = دم->se_اگلا;
	    اگر (دم->se_اگلا)
		دم->se_اگلا->se_پچھلا = p;
	    p->se_پچھلا = دم;
	    دم->se_اگلا = p;
	    update_debug_sign(curbuf, lnum);
	    توڑو;
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
    ساکن خالی
addsigntype(
    nbbuf_T	*buf,
    صحیح		type_عدد,
    char_u	*typeName,
    char_u	*tooltip غیر_مستعمل,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    صحیح i, j;
    صحیح use_fg = (*fg && STRCMP(fg, "none") != 0);
    صحیح use_bg = (*bg && STRCMP(bg, "none") != 0);

    برائے (i = 0; i < globalsignmapused; i++)
	اگر (STRCMP(typeName, globalsignmap[i]) == 0)
	    توڑو;

    اگر (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_عدد, typeName, tooltip, glyphFile, fg, bg));
	اگر (use_fg || use_bg)
	{
	    حرف fgbuf[2 * (8 + MAX_COLOR_لمبائی) + 1];
	    حرف bgbuf[2 * (8 + MAX_COLOR_لمبائی) + 1];
	    حرف *ptr;
	    صحیح قدر;

	    قدر = strtol((حرف *)fg, &ptr, 10);
	    اگر (ptr != (حرف *)fg)
		sprintf(fgbuf, "guifg=#%06x", قدر & 0xFFFFFF);
	    ورنہ
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    قدر = strtol((حرف *)bg, &ptr, 10);
	    اگر (ptr != (حرف *)bg)
		sprintf(bgbuf, "guibg=#%06x", قدر & 0xFFFFFF);
	    ورنہ
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    اگر (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    ورنہ اگر (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    ورنہ
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	ورنہ
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	اگر (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_عدد;

	اگر (globalsignmapused == globalsignmaplen)
	{
	    اگر (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(حرف *, globalsignmaplen);
	    }
	    ورنہ    // grow it
	    {
		صحیح incr;
		صحیح oldlen = globalsignmaplen;
		حرف **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * حجم(حرف *));
		اگر (globalsignmap == تہی)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    واپس;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * حجم(حرف *));
	    }
	}

	globalsignmap[i] = (حرف *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    برائے (j = 0; j < buf->signmapused; j++)
	اگر (buf->signmap[j] == i + 1)
	    واپس;

    // add to local map
    اگر (buf->signmapused == buf->signmaplen)
    {
	اگر (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(صحیح, buf->signmaplen);
	}
	ورنہ    // grow it
	{
	    صحیح incr;
	    صحیح oldlen = buf->signmaplen;
	    صحیح *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * حجم(صحیح));
	    اگر (buf->signmap == تہی)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		واپس;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * حجم(صحیح));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    ساکن صحیح
mapsigntype(nbbuf_T *buf, صحیح localsigntype)
{
    اگر (--localsigntype >= 0 && localsigntype < buf->signmapused)
	واپس buf->signmap[localsigntype];

    واپس 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    ساکن طویل
get_buf_سائز(buf_T *bufp)
{
    linenr_T	lnum;
    طویل	char_گنتی = 0;
    صحیح		eol_سائز;
    طویل	آخری_check = 100000L;

    اگر (bufp->b_ml.ml_flags & ML_خالی_خانے)
	واپس 0;

    اگر (get_fileformat(bufp) == EOL_DOS)
	eol_سائز = 2;
    ورنہ
	eol_سائز = 1;
    برائے (lnum = 1; lnum <= bufp->b_ml.ml_سطر_گنتی; ++lnum)
    {
	char_گنتی += ml_get_buf_len(bufp, lnum) + eol_سائز;
	// Check for a CTRL-C every 100000 characters
	اگر (char_گنتی > آخری_check)
	{
	    ui_breakcheck();
	    اگر (got_int)
		واپس char_گنتی;
	    آخری_check = char_گنتی + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    اگر (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_گنتی -= eol_سائز;

    واپس char_گنتی;
}

/*
 * Convert character offset to lnum,col
 */
    ساکن pos_T *
off2pos(buf_T *buf, طویل انحراف)
{
    linenr_T	 lnum;
    ساکن pos_T pos;

    pos.lnum = 0;
    pos.ستون = 0;
    pos.coladd = 0;

    اگر (!(buf->b_ml.ml_flags & ML_خالی_خانے))
    {
	اگر ((lnum = ml_ڈھونڈو_سطر_or_انحراف(buf, (linenr_T)0, &انحراف)) < 0)
	    واپس تہی;
	pos.lnum = lnum;
	pos.ستون = انحراف;
    }

    واپس &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    ساکن pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    ساکن pos_T	mypos;
    طویل		off;

    off = strtol((حرف *)*argp, (حرف **)argp, 10);
    اگر (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.ستون = strtol((حرف *)*argp, (حرف **)argp, 10);
	mypos.coladd = 0;
	واپس &mypos;
    }
    واپس off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    ساکن طویل
pos2off(buf_T *buf, pos_T *pos)
{
    طویل	 انحراف = 0;

    اگر (buf->b_ml.ml_flags & ML_خالی_خانے)
	واپس 0;

    اگر ((انحراف = ml_ڈھونڈو_سطر_or_انحراف(buf, pos->lnum, 0)) < 0)
	واپس 0;
    انحراف += pos->ستون;

    واپس انحراف;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    ساکن خالی
print_read_پیغام(nbbuf_T *buf)
{
    صحیح	    lnum = buf->bufp->b_ml.ml_سطر_گنتی;
    off_T   nchars = buf->bufp->b_orig_سائز;
    char_u  c;

    پیغام_جوڑو_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    اگر (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    اگر (!buf->bufp->b_آغاز_eol)
    {
	STRCAT(IObuff, shortmess(SHM_آخری) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    پیغام_جوڑو_سطریں(c, (طویل)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_پیغام);
    پیغام_scrolled_ign = TRUE;
    پیغام_trunc_attr((حرف *)IObuff, FALSE, 0);
    پیغام_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    ساکن خالی
print_save_پیغام(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    اگر (nchars >= 0)
    {
	// put fname in IObuff with quotes
	پیغام_جوڑو_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	پیغام_جوڑو_سطریں(c, buf->bufp->b_ml.ml_سطر_گنتی,
						buf->bufp->b_orig_سائز);

	VIM_CLEAR(keep_پیغام);
	پیغام_scrolled_ign = TRUE;
	p = (char_u *)پیغام_trunc_attr((حرف *)IObuff, FALSE, 0);
	اگر ((پیغام_scrolled && !need_wait_return) || !buf->init_مکمل)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    سیٹ_keep_پیغام(p, 0);
	}
	پیغام_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    ورنہ
    {
	حرف msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_جوڑو_bang_مقصد_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#اگر_ختم // defined(FEAT_NETBEANS_INTG)
