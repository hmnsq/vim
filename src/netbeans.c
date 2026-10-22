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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_NETBEANS_INTG)

#నిర్వచితం_కాకపోతే MSWIN
# include <netdb.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
#అయితే_ముగింపు

#అన్తర్భావ "version.h"

#నిర్వచించు GUARDED		10000 // typenr for "guarded" annotation
#నిర్వచించు GUARDEDOFFSET 1000000 // base for "guarded" sign id's
#నిర్వచించు MAX_COLOR_నిడివి 32 // max length of color name in defineAnnoType

// Characters valid in a sign/highlight group name
#నిర్వచించు VALID_CHARS         (char_u *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#నిర్వచించు VALID_SIGNNAME_CHARS    VALID_CHARS "_"
#నిర్వచించు VALID_COLOR_CHARS       VALID_CHARS "#"

// The first implementation (working only with Netbeans) returned "1.1".  The
// protocol implemented here also supports A-A-P.
స్థిర అక్షరం *Ext_Ed_Protocol_సంస్కరణ = "2.5";

స్థిర దీర్ఘ pos2off(buf_T *, pos_T *);
స్థిర pos_T *off2pos(buf_T *, దీర్ఘ);
స్థిర pos_T *get_off_or_lnum(buf_T *buf, char_u **argp);
స్థిర దీర్ఘ get_buf_కొలత(buf_T *);
స్థిర పూర్ణాంకం netbeans_keystring(char_u *keystr);
స్థిర శూన్య special_కీలకాలు(char_u *వాదనలు);

స్థిర పూర్ణాంకం getConnInfo(అక్షరం *file, అక్షరం **host, అక్షరం **పోర్టు, అక్షరం **password);

స్థిర శూన్య nb_init_graphics(శూన్య);
స్థిర శూన్య coloncmd(అక్షరం *cmd, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
స్థిర శూన్య nb_సమితి_curbuf(buf_T *buf);
స్థిర శూన్య nb_parse_cmd(char_u *);
స్థిర పూర్ణాంకం  nb_do_cmd(పూర్ణాంకం, char_u *, పూర్ణాంకం, పూర్ణాంకం, char_u *);
స్థిర శూన్య nb_send(అక్షరం *buf, అక్షరం *fun);
స్థిర శూన్య nb_free(శూన్య);

#నిర్వచించు NETBEANS_OPEN (channel_can_write_గమ్యం(nb_channel))
స్థిర channel_T *nb_channel = ఖాళీ;

స్థిర పూర్ణాంకం r_cmdno;			// current command number for reply
స్థిర పూర్ణాంకం dosetvisible = FALSE;

/*
 * Include the debugging code if wanted.
 */
#నిర్వచితమైతే NBDEBUG
# include "nbdebug.c"
#అయితే_ముగింపు

స్థిర పూర్ణాంకం needupdate = 0;
స్థిర పూర్ణాంకం లోపల_Atomic = 0;

/*
 * Return TRUE if "str" contains only characters from "allowed".
 * Used to validate NetBeans-supplied strings before interpolating them
 * into Ex commands via coloncmd().
 */
    స్థిర పూర్ణాంకం
nb_is_సురక్షిత_string(char_u *str, char_u *allowed)
{
    ఒకవేళ (str == ఖాళీ)
	తిరిగి FALSE;
    కొరకు (char_u *p = str; *p != NUL; p++)
	ఒకవేళ (vim_strchr(allowed, *p) == ఖాళీ)
	    తిరిగి FALSE;
    తిరిగి TRUE;
}

/*
 * Callback invoked when the channel is closed.
 */
    స్థిర శూన్య
nb_channel_closed(శూన్య)
{
    nb_channel = ఖాళీ;
}

/*
 * Close the connection and cleanup.
 * May be called when the socket was closed earlier.
 */
    స్థిర శూన్య
netbeans_close(శూన్య)
{
    ఒకవేళ (NETBEANS_OPEN)
    {
	netbeans_send_disconnect();
	ఒకవేళ (nb_channel != ఖాళీ)
	{
	    // Close the socket and remove the input handlers.
	    channel_close(nb_channel, TRUE);
	    channel_clear(nb_channel);
	}
	nb_channel = ఖాళీ;
    }

#నిర్వచితమైతే FEAT_BEVAL_GUI
    bevalServers &= ~BEVAL_NETBEANS;
#అయితే_ముగింపు

    needupdate = 0;
    లోపల_Atomic = 0;
    nb_free();

    // remove all signs and update the screen after gutter removal
    coloncmd(":sign unplace *");
    మారింది_కిటికీ_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    బహిర్గతం_flush_cursor(TRUE, FALSE);
}

#నిర్వచించు NB_DEF_HOST "localhost"
#నిర్వచించు NB_DEF_ADDR "3219"
#నిర్వచించు NB_DEF_PASS "changeme"

    స్థిర పూర్ణాంకం
netbeans_connect(అక్షరం *params, పూర్ణాంకం doabort)
{
    పూర్ణాంకం		పోర్టు;
    అక్షరం	buf[32];
    అక్షరం	*hostname = ఖాళీ;
    అక్షరం	*చిరునామా = ఖాళీ;
    అక్షరం	*password = ఖాళీ;
    అక్షరం	*fname;
    అక్షరం	*arg = ఖాళీ;

    ఒకవేళ (*params == '=')
    {
	// "=fname": Read info from specified file.
	ఒకవేళ (getConnInfo(params + 1, &hostname, &చిరునామా, &password) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే
    {
	ఒకవేళ (*params == ':')
	    // ":<host>:<addr>:<password>": get info from argument
	    arg = params + 1;
	ఒకవేళ (arg == ఖాళీ && (fname = getenv("__NETBEANS_CONINFO")) != ఖాళీ)
	{
	    // "": get info from file specified in environment
	    ఒకవేళ (getConnInfo(fname, &hostname, &చిరునామా, &password) == FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే
	{
	    ఒకవేళ (arg != ఖాళీ)
	    {
		// ":<host>:<addr>:<password>": get info from argument
		hostname = arg;
		చిరునామా = strchr(hostname, ':');
		ఒకవేళ (చిరునామా != ఖాళీ)
		{
		    *చిరునామా++ = '\0';
		    password = strchr(చిరునామా, ':');
		    ఒకవేళ (password != ఖాళీ)
			*password++ = '\0';
		}
	    }

	    // Get the missing values from the environment.
	    ఒకవేళ (hostname == ఖాళీ || *hostname == '\0')
		hostname = getenv("__NETBEANS_HOST");
	    ఒకవేళ (చిరునామా == ఖాళీ)
		చిరునామా = getenv("__NETBEANS_SOCKET");
	    ఒకవేళ (password == ఖాళీ)
		password = getenv("__NETBEANS_VIM_PASSWORD");

	    // Move values to allocated memory.
	    ఒకవేళ (hostname != ఖాళీ)
		hostname = (అక్షరం *)vim_strsave((char_u *)hostname);
	    ఒకవేళ (చిరునామా != ఖాళీ)
		చిరునామా = (అక్షరం *)vim_strsave((char_u *)చిరునామా);
	    ఒకవేళ (password != ఖాళీ)
		password = (అక్షరం *)vim_strsave((char_u *)password);
	}
    }

    // Use the default when a value is missing.
    ఒకవేళ (hostname == ఖాళీ || *hostname == '\0')
    {
	vim_free(hostname);
	hostname = (అక్షరం *)vim_strsave((char_u *)NB_DEF_HOST);
    }
    ఒకవేళ (చిరునామా == ఖాళీ || *చిరునామా == '\0')
    {
	vim_free(చిరునామా);
	చిరునామా = (అక్షరం *)vim_strsave((char_u *)NB_DEF_ADDR);
    }
    ఒకవేళ (password == ఖాళీ || *password == '\0')
    {
	vim_free(password);
	password = (అక్షరం *)vim_strsave((char_u *)NB_DEF_PASS);
    }
    ఒకవేళ (hostname != ఖాళీ && చిరునామా != ఖాళీ && password != ఖాళీ)
    {
	పోర్టు = atoi(చిరునామా);
	nb_channel = channel_open(hostname, పోర్టు, 3000, nb_channel_closed);
	ఒకవేళ (nb_channel != ఖాళీ)
	{
	    // success
#నిర్వచితమైతే FEAT_BEVAL_GUI
	    bevalServers |= BEVAL_NETBEANS;
#అయితే_ముగింపు

	    // success, login
	    vim_snprintf(buf, పరిమాణం(buf), "AUTH %s\n", password);
	    nb_send(buf, "netbeans_connect");

	    sprintf(buf, "0:version=0 \"%s\"\n", Ext_Ed_Protocol_సంస్కరణ);
	    nb_send(buf, "externaleditor_version");
	}
    }

    ఒకవేళ (nb_channel == ఖాళీ && doabort)
	getout(1);

    vim_free(hostname);
    vim_free(చిరునామా);
    vim_free(password);
    తిరిగి NETBEANS_OPEN ? OK : FAIL;
}

/*
 * Obtain the NetBeans hostname, port address and password from a file.
 * Return the strings in allocated memory.
 * Return FAIL if the file could not be read, OK otherwise (no matter what it
 * contains).
 */
    స్థిర పూర్ణాంకం
getConnInfo(అక్షరం *file, అక్షరం **host, అక్షరం **పోర్టు, అక్షరం **auth)
{
    FILE *fp;
    char_u buf[BUFSIZ];
    char_u *lp;
    char_u *nlp;
#నిర్వచితమైతే UNIX
    stat_T	st;

    /*
     * For Unix only accept the file when it's not accessible by others.
     * The open will then fail if we don't own the file.
     */
    ఒకవేళ (mch_stat(file, &st) == 0 && (st.st_బహుళకం & 0077) != 0)
    {
	nbdebug(("Wrong access mode for NetBeans connection info file: \"%s\"\n",
								       file));
	semsg(_(e_wrong_access_బహుళకం_for_netbeans_connection_info_file_str),
									file);
	తిరిగి FAIL;
    }
#అయితే_ముగింపు

    fp = mch_fopen(file, "r");
    ఒకవేళ (fp == ఖాళీ)
    {
	nbdebug(("Cannot open NetBeans connection info file\n"));
	PERROR(e_cannot_open_netbeans_connection_info_file);
	తిరిగి FAIL;
    }

    // Read the file. There should be one of each parameter
    అయితే ((lp = (char_u *)fgets((అక్షరం *)buf, BUFSIZ, fp)) != ఖాళీ)
    {
	ఒకవేళ ((nlp = vim_strchr(lp, '\n')) != ఖాళీ)
	    *nlp = 0;	    // strip off the trailing newline

	ఒకవేళ (STRNCMP(lp, "host=", 5) == 0)
	{
	    vim_free(*host);
	    *host = (అక్షరం *)vim_strsave(&buf[5]);
	}
	లేకపోతే ఒకవేళ (STRNCMP(lp, "port=", 5) == 0)
	{
	    vim_free(*పోర్టు);
	    *పోర్టు = (అక్షరం *)vim_strsave(&buf[5]);
	}
	లేకపోతే ఒకవేళ (STRNCMP(lp, "auth=", 5) == 0)
	{
	    vim_free(*auth);
	    *auth = (అక్షరం *)vim_strsave(&buf[5]);
	}
    }
    fclose(fp);

    తిరిగి OK;
}


నిర్మాణం keyqueue
{
    char_u	    *keystr;
    నిర్మాణం keyqueue *తదుపరి;
    నిర్మాణం keyqueue *prev;
};

రకనిర్వచనం నిర్మాణం keyqueue కీలకం_Q_T;

స్థిర కీలకం_Q_T కీలకం_తల; // dummy node, header for circular queue


/*
 * Queue up key commands sent from netbeans.
 * We store the string, because it may depend on the global mod_mask and
 * :nbkey doesn't have a key number.
 */
    స్థిర శూన్య
postpone_keycommand(char_u *keystr)
{
    కీలకం_Q_T *కణుపు;

    కణుపు = ALLOC_ONE(కీలకం_Q_T);
    ఒకవేళ (కణుపు == ఖాళీ)
	తిరిగి;  // out of memory, drop the key

    ఒకవేళ (కీలకం_తల.తదుపరి == ఖాళీ) // initialize circular queue
    {
	కీలకం_తల.తదుపరి = &కీలకం_తల;
	కీలకం_తల.prev = &కీలకం_తల;
    }

    // insert node at tail of queue
    కణుపు->తదుపరి = &కీలకం_తల;
    కణుపు->prev = కీలకం_తల.prev;
    కీలకం_తల.prev->తదుపరి = కణుపు;
    కీలకం_తల.prev = కణుపు;

    కణుపు->keystr = vim_strsave(keystr);
}

/*
 * Handle any queued-up NetBeans keycommands to be send.
 */
    స్థిర శూన్య
handle_కీలకం_వరుస(శూన్య)
{
    పూర్ణాంకం postponed = FALSE;

    అయితే (!postponed && కీలకం_తల.తదుపరి && కీలకం_తల.తదుపరి != &కీలకం_తల)
    {
	// first, unlink the node
	కీలకం_Q_T *కణుపు = కీలకం_తల.తదుపరి;
	కీలకం_తల.తదుపరి = కణుపు->తదుపరి;
	కణుపు->తదుపరి->prev = కణుపు->prev;

	// Now, send the keycommand.  This may cause it to be postponed again
	// and change keyHead.
	ఒకవేళ (కణుపు->keystr != ఖాళీ)
	    postponed = !netbeans_keystring(కణుపు->keystr);
	vim_free(కణుపు->keystr);

	// Finally, dispose of the node
	vim_free(కణుపు);
    }
}


/*
 * While there's still a command in the work queue, parse and execute it.
 */
    శూన్య
netbeans_parse_messages(శూన్య)
{
    readq_T	*కణుపు;
    char_u	*బఫరు;
    char_u	*p;
    పూర్ణాంకం		own_కణుపు;

    అయితే (nb_channel != ఖాళీ)
    {
	కణుపు = channel_peek(nb_channel, PART_SOCK);
	ఒకవేళ (కణుపు == ఖాళీ)
	    ఆపు;	// nothing to read

	// Locate the end of the first line in the first buffer.
	p = channel_మొదటి_nl(కణుపు);
	ఒకవేళ (p == ఖాళీ)
	{
	    // Command isn't complete.  If there is no following buffer,
	    // return (wait for more). If there is another buffer following,
	    // prepend the text to that buffer and delete this one.
	    ఒకవేళ (channel_collapse(nb_channel, PART_SOCK, TRUE) == FAIL)
		తిరిగి;
	    కొనసాగు;
	}

	// There is a complete command at the start of the buffer.
	// Terminate it with a NUL.  When no more text is following unlink
	// the buffer.  Do this before executing, because new buffers can
	// be added while busy handling the command.
	*p++ = NUL;
	ఒకవేళ (*p == NUL)
	{
	    own_కణుపు = TRUE;
	    బఫరు = channel_get(nb_channel, PART_SOCK, ఖాళీ);
	    // "node" is now invalid!
	}
	లేకపోతే
	{
	    own_కణుపు = FALSE;
	    బఫరు = కణుపు->rq_బఫరు;
	}

	// Now, parse and execute the commands.  This may set nb_channel to
	// NULL if the channel is closed.
	nb_parse_cmd(బఫరు);

	ఒకవేళ (own_కణుపు)
	    // buffer finished, dispose of it
	    vim_free(బఫరు);
	లేకపోతే ఒకవేళ (nb_channel != ఖాళీ)
	    // more follows, move it to the start
	    channel_consume(nb_channel, PART_SOCK, (పూర్ణాంకం)(p - బఫరు));
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
    స్థిర శూన్య
nb_parse_cmd(char_u *cmd)
{
    అక్షరం	*verb;
    అక్షరం	*q;
    పూర్ణాంకం		bufno;
    పూర్ణాంకం		isfunc = -1;

    ఒకవేళ (STRCMP(cmd, "DISCONNECT") == 0)
    {
	// We assume the server knows that we can safely exit!
	// Disconnect before exiting, Motif hangs in a Select error
	// message otherwise.
	netbeans_close();
	getout(0);
	// NOTREACHED
    }

    ఒకవేళ (STRCMP(cmd, "DETACH") == 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    buf->b_has_sign_స్తంభం = false;

	// The IDE is breaking the connection.
	netbeans_close();
	తిరిగి;
    }

    bufno = strtol((అక్షరం *)cmd, &verb, 10);

    ఒకవేళ (*verb != ':')
    {
	nbdebug(("    missing colon: %s\n", cmd));
	semsg(_(e_missing_colon_str), cmd);
	తిరిగి;
    }
    ++verb; // skip colon

    కొరకు (q = verb; *q; q++)
    {
	ఒకవేళ (*q == '!')
	{
	    *q++ = NUL;
	    isfunc = 0;
	    ఆపు;
	}
	లేకపోతే ఒకవేళ (*q == '/')
	{
	    *q++ = NUL;
	    isfunc = 1;
	    ఆపు;
	}
    }

    ఒకవేళ (isfunc < 0)
    {
	nbdebug(("    missing ! or / in: %s\n", cmd));
	semsg(_(e_missing_bang_or_slash_లోపల_str), cmd);
	తిరిగి;
    }

    r_cmdno = strtol(q, &q, 10);

    q = (అక్షరం *)skipwhite((char_u *)q);

    ఒకవేళ (nb_do_cmd(bufno, (char_u *)verb, isfunc, r_cmdno, (char_u *)q) == FAIL)
    {
#నిర్వచితమైతే NBDEBUG
	/*
	 * This happens because the ExtEd can send a command or 2 after
	 * doing a stopDocumentListen command. It doesn't harm anything
	 * so I'm disabling it except for debugging.
	 */
	nbdebug(("nb_parse_cmd: Command error for \"%s\"\n", cmd));
	emsg(e_bad_return_from_nb_do_cmd);
#అయితే_ముగింపు
    }
}

నిర్మాణం nbbuf_struct
{
    buf_T		*bufp;
    గుర్తులేని పూర్ణాంకం	 fireChanges:1;
    గుర్తులేని పూర్ణాంకం	 init_పూర్తి:1;
    గుర్తులేని పూర్ణాంకం	 చొప్పించు_పూర్తి:1;
    గుర్తులేని పూర్ణాంకం	 modified:1;
    పూర్ణాంకం			 nbbuf_సంఖ్య;
    అక్షరం		*displayname;
    పూర్ణాంకం			*signmap;
    short_u		 signmaplen;
    short_u		 signmapused;
};

రకనిర్వచనం నిర్మాణం nbbuf_struct nbbuf_T;

స్థిర nbbuf_T *buf_list = ఖాళీ;
స్థిర పూర్ణాంకం buf_list_కొలత = 0;	// size of buf_list
స్థిర పూర్ణాంకం buf_list_వాడిన = 0;	// nr of entries in buf_list actually in use

స్థిర అక్షరం **globalsignmap = ఖాళీ;
స్థిర పూర్ణాంకం globalsignmaplen = 0;
స్థిర పూర్ణాంకం globalsignmapused = 0;

స్థిర పూర్ణాంకం  mapsigntype(nbbuf_T *, పూర్ణాంకం localsigntype);
స్థిర శూన్య addsigntype(nbbuf_T *, పూర్ణాంకం localsigntype, char_u *typeName,
			char_u *tooltip, char_u *glyphfile,
			char_u *fg, char_u *bg);
స్థిర శూన్య print_read_సందేశం(nbbuf_T *buf);
స్థిర శూన్య print_save_సందేశం(nbbuf_T *buf, off_T nchars);

స్థిర పూర్ణాంకం curPCtype = -1;

/*
 * Free netbeans resources.
 */
    స్థిర శూన్య
nb_free(శూన్య)
{
    కీలకం_Q_T *కీలకం_కణుపు = కీలకం_తల.తదుపరి;
    nbbuf_T buf;
    పూర్ణాంకం i;

    // free the netbeans buffer list
    కొరకు (i = 0; i < buf_list_వాడిన; i++)
    {
	buf = buf_list[i];
	vim_free(buf.displayname);
	vim_free(buf.signmap);
	ఒకవేళ (buf.bufp != ఖాళీ && buf_valid(buf.bufp))
	{
	    buf.bufp->b_netbeans_file = false;
	    buf.bufp->b_was_netbeans_file = false;
	}
    }
    VIM_CLEAR(buf_list);
    buf_list_కొలత = 0;
    buf_list_వాడిన = 0;

    // free the queued key commands
    అయితే (కీలకం_కణుపు != ఖాళీ && కీలకం_కణుపు != &కీలకం_తల)
    {
	కీలకం_Q_T *తదుపరి = కీలకం_కణుపు->తదుపరి;
	vim_free(కీలకం_కణుపు->keystr);
	vim_free(కీలకం_కణుపు);
	ఒకవేళ (తదుపరి == &కీలకం_తల)
	{
	    కీలకం_తల.తదుపరి = &కీలకం_తల;
	    కీలకం_తల.prev = &కీలకం_తల;
	    ఆపు;
	}
	కీలకం_కణుపు = తదుపరి;
    }

    // free the queued netbeans commands
    ఒకవేళ (nb_channel != ఖాళీ)
	channel_clear(nb_channel);
}

/*
 * Get the Netbeans buffer number for the specified buffer.
 */
    స్థిర పూర్ణాంకం
nb_getbufno(buf_T *bufp)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < buf_list_వాడిన; i++)
	ఒకవేళ (buf_list[i].bufp == bufp)
	    తిరిగి i;
    తిరిగి -1;
}

/*
 * Is this a NetBeans-owned buffer?
 */
    పూర్ణాంకం
is_Netbeans_బఫరు(buf_T *bufp)
{
    తిరిగి NETBEANS_OPEN && bufp->b_netbeans_file;
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
    పూర్ణాంకం
isNetbeansModified(buf_T *bufp)
{
    ఒకవేళ (is_Netbeans_బఫరు(bufp))
    {
	పూర్ణాంకం bufno = nb_getbufno(bufp);

	ఒకవేళ (bufno > 0)
	    తిరిగి buf_list[bufno].modified;
	లేకపోతే
	    తిరిగి FALSE;
    }
    లేకపోతే
	తిరిగి FALSE;
}

/*
 * Given a Netbeans buffer number, return the netbeans buffer.
 * Returns NULL for 0 or a negative number. A 0 bufno means a
 * non-buffer related command has been sent.
 */
    స్థిర nbbuf_T *
nb_get_buf(పూర్ణాంకం bufno)
{
    // find or create a buffer with the given number
    పూర్ణాంకం incr;

    ఒకవేళ (bufno <= 0)
	తిరిగి ఖాళీ;

    ఒకవేళ (!buf_list)
    {
	// initialize
	buf_list = alloc_clear(100 * పరిమాణం(nbbuf_T));
	buf_list_కొలత = 100;
    }
    ఒకవేళ (bufno >= buf_list_వాడిన) // new
    {
	ఒకవేళ (bufno >= buf_list_కొలత) // grow list
	{
	    nbbuf_T	*t_buf_list = buf_list;
	    size_t	bufsize;

	    incr = bufno - buf_list_కొలత + 90;
	    buf_list_కొలత += incr;
	    bufsize = buf_list_కొలత * పరిమాణం(nbbuf_T);
	    ఒకవేళ (bufsize == 0 || bufsize / పరిమాణం(nbbuf_T)
						      != (size_t)buf_list_కొలత)
	    {
		// list size overflow, bail out
		తిరిగి ఖాళీ;
	    }
	    buf_list = vim_realloc(buf_list, bufsize);
	    ఒకవేళ (buf_list == ఖాళీ)
	    {
		vim_free(t_buf_list);
		buf_list_కొలత = 0;
		తిరిగి ఖాళీ;
	    }
	    vim_memset(buf_list + buf_list_కొలత - incr, 0,
						      incr * పరిమాణం(nbbuf_T));
	}

	అయితే (buf_list_వాడిన <= bufno)
	{
	    // Default is to fire text changes.
	    buf_list[buf_list_వాడిన].fireChanges = 1;
	    ++buf_list_వాడిన;
	}
    }

    తిరిగి buf_list + bufno;
}

/*
 * Return the number of buffers that are modified.
 */
    స్థిర పూర్ణాంకం
లెక్క_మారింది_buffers(శూన్య)
{
    buf_T	*bufp;
    పూర్ణాంకం		n;

    n = 0;
    FOR_ALL_BUFFERS(bufp)
	ఒకవేళ (bufp->b_మారింది)
	    ++n;
    తిరిగి n;
}

/*
 * End the netbeans session.
 */
    శూన్య
netbeans_చివర(శూన్య)
{
    పూర్ణాంకం i;
    స్థిర అక్షరం buf[128];

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    కొరకు (i = 0; i < buf_list_వాడిన; i++)
    {
	ఒకవేళ (!buf_list[i].bufp)
	    కొనసాగు;
	ఒకవేళ (netbeansForcedQuit)
	{
	    // mark as unmodified so NetBeans won't put up dialog on "killed"
	    sprintf(buf, "%d:unmodified=%d\n", i, r_cmdno);
	    nbdebug(("EVT: %s", buf));
	    nb_send(buf, "netbeans_end");
	}
	sprintf(buf, "%d:killed=%d\n", i, r_cmdno);
	nbdebug(("EVT: %s", buf));
	// nb_send(buf, "netbeans_end");    avoid "write failed" messages
	nb_send(buf, ఖాళీ);
	buf_list[i].bufp = ఖాళీ;
    }
}

/*
 * Send a message to netbeans.
 * When "fun" is NULL no error is given.
 */
    స్థిర శూన్య
nb_send(అక్షరం *buf, అక్షరం *fun)
{
    ఒకవేళ (nb_channel != ఖాళీ)
	channel_send(nb_channel, PART_SOCK, (char_u *)buf,
						       (పూర్ణాంకం)STRLEN(buf), fun);
}

/*
 * Some input received from netbeans requires a response. This function
 * handles a response with no information (except the command number).
 */
    స్థిర శూన్య
nb_జవాబు_nil(పూర్ణాంకం cmdno)
{
    అక్షరం జవాబు[32];

    nbdebug(("REP %d: <none>\n", cmdno));

    // Avoid printing an annoying error message.
    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    sprintf(జవాబు, "%d\n", cmdno);
    nb_send(జవాబు, "nb_reply_nil");
}


/*
 * Send a response with text.
 * "result" must have been quoted already (using nb_quote()).
 */
    స్థిర శూన్య
nb_జవాబు_పాఠ్యం(పూర్ణాంకం cmdno, char_u *ఫలితం)
{
    char_u *జవాబు;

    nbdebug(("REP %d: %s\n", cmdno, (అక్షరం *)ఫలితం));

    జవాబు = alloc(STRLEN(ఫలితం) + 32);
    sprintf((అక్షరం *)జవాబు, "%d %s\n", cmdno, (అక్షరం *)ఫలితం);
    nb_send((అక్షరం *)జవాబు, "nb_reply_text");

    vim_free(జవాబు);
}


/*
 * Send a response with a number result code.
 */
    స్థిర శూన్య
nb_జవాబు_nr(పూర్ణాంకం cmdno, దీర్ఘ ఫలితం)
{
    అక్షరం జవాబు[32];

    nbdebug(("REP %d: %ld\n", cmdno, ఫలితం));

    sprintf(జవాబు, "%d %ld\n", cmdno, ఫలితం);
    nb_send(జవాబు, "nb_reply_nr");
}


/*
 * Encode newline, ret, backslash, double quote for transmission to NetBeans.
 */
    స్థిర char_u *
nb_quote(char_u *txt)
{
    char_u *buf = alloc(2 * STRLEN(txt) + 1);
    char_u *p = txt;
    char_u *q = buf;

    ఒకవేళ (buf == ఖాళీ)
	తిరిగి ఖాళీ;
    కొరకు (; *p; p++)
    {
	ఎంపిక (*p)
	{
	    స్థితి '\"':
	    స్థితి '\\':
		*q++ = '\\'; *q++ = *p; ఆపు;
	 // case '\t':
	 //     *q++ = '\\'; *q++ = 't'; break;
	    స్థితి '\n':
		*q++ = '\\'; *q++ = 'n'; ఆపు;
	    స్థితి '\r':
		*q++ = '\\'; *q++ = 'r'; ఆపు;
	    అప్రమేయ:
		*q++ = *p;
		ఆపు;
	}
    }
    *q = '\0';

    తిరిగి buf;
}


/*
 * Remove top level double quotes; convert backslashed chars.
 * Returns an allocated string (NULL for failure).
 * If "endp" is not NULL it is set to the character after the terminating
 * quote.
 */
    స్థిర అక్షరం *
nb_unquote(char_u *p, char_u **endp)
{
    అక్షరం *ఫలితం = 0;
    అక్షరం *q;
    పూర్ణాంకం పూర్తి = 0;

    // result is never longer than input
    ఫలితం = alloc_clear(STRLEN(p) + 1);
    ఒకవేళ (ఫలితం == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (*p++ != '"')
    {
	nbdebug(("nb_unquote called with string that doesn't start with a quote!: %s\n",
			p));
	ఫలితం[0] = NUL;
	తిరిగి ఫలితం;
    }

    కొరకు (q = ఫలితం; !పూర్తి && *p != NUL;)
    {
	ఎంపిక (*p)
	{
	    స్థితి '"':
		/*
		 * Unbackslashed dquote marks the end, if first char was dquote.
		 */
		పూర్తి = 1;
		ఆపు;

	    స్థితి '\\':
		++p;
		ఎంపిక (*p)
		{
		    స్థితి '\\':	*q++ = '\\';	ఆపు;
		    స్థితి 'n':	*q++ = '\n';	ఆపు;
		    స్థితి 't':	*q++ = '\t';	ఆపు;
		    స్థితి 'r':	*q++ = '\r';	ఆపు;
		    స్థితి '"':	*q++ = '"';	ఆపు;
		    స్థితి NUL:	--p;		ఆపు;
		    // default: skip over illegal chars
		}
		++p;
		ఆపు;

	    అప్రమేయ:
		*q++ = *p++;
	}
    }

    ఒకవేళ (endp != ఖాళీ)
	*endp = p;

    తిరిగి ఫలితం;
}

/*
 * Remove from "first" byte to "last" byte (inclusive), at line "lnum" of the
 * current buffer.  Remove to end of line when "last" is MAXCOL.
 */
    స్థిర శూన్య
nb_partialremove(linenr_T lnum, colnr_T మొదటి, colnr_T చివరి)
{
    char_u *oldtext, *newtext;
    పూర్ణాంకం oldlen;
    పూర్ణాంకం lastbyte = చివరి;

    oldtext = ml_get(lnum);
    oldlen = ml_get_len(lnum);
    ఒకవేళ (మొదటి >= (colnr_T)oldlen || oldlen == 0)  // just in case
	తిరిగి;
    ఒకవేళ (lastbyte >= oldlen)
	lastbyte = oldlen - 1;
    newtext = alloc(oldlen - (lastbyte - మొదటి));
    ఒకవేళ (newtext == ఖాళీ)
	తిరిగి;

    mch_memmove(newtext, oldtext, మొదటి);
    STRMOVE(newtext + మొదటి, oldtext + lastbyte + 1);
    nbdebug(("    NEW LINE %ld: %s\n", lnum, newtext));
    ml_replace(lnum, newtext, FALSE);
}

/*
 * Replace the "first" line with the concatenation of the "first" and
 * the "other" line. The "other" line is not removed.
 */
    స్థిర శూన్య
nb_joinlines(linenr_T మొదటి, linenr_T other)
{
    పూర్ణాంకం len_మొదటి, len_other;
    char_u *p;

    len_మొదటి = ml_get_len(మొదటి);
    len_other = ml_get_len(other);
    p = alloc(len_మొదటి + len_other + 1);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    mch_memmove(p, ml_get(మొదటి), len_మొదటి);
    mch_memmove(p + len_మొదటి, ml_get(other), len_other + 1);
    ml_replace(మొదటి, p, FALSE);
}

#నిర్వచించు SKIP_STOP 2
#నిర్వచించు streq(a,b) (strcmp(a,b) == 0)

/*
 * Do the actual processing of a single netbeans command or function.
 * The difference between a command and function is that a function
 * gets a response (it's required) but a command does not.
 * For arguments see comment for nb_parse_cmd().
 */
    స్థిర పూర్ణాంకం
nb_do_cmd(
    పూర్ణాంకం		bufno,
    char_u	*cmd,
    పూర్ణాంకం		func,
    పూర్ణాంకం		cmdno,
    char_u	*వాదనలు)	    // points to space before arguments or NUL
{
    పూర్ణాంకం		do_update = 0;
    దీర్ఘ	off = 0;
    nbbuf_T	*buf = nb_get_buf(bufno);
    స్థిర పూర్ణాంకం	skip = 0;
    పూర్ణాంకం		retval = OK;
    అక్షరం	*cp;	    // for when a char pointer is needed

    nbdebug(("%s %d: (%d) %s %s\n", (func) ? "FUN" : "CMD", cmdno, bufno, cmd,
	STRCMP(cmd, "insert") == 0 ? "<text>" : (అక్షరం *)వాదనలు));

    ఒకవేళ (func)
    {
// =====================================================================
	ఒకవేళ (streq((అక్షరం *)cmd, "getModified"))
	{
	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
		// Return the number of buffers that are modified.
		nb_జవాబు_nr(cmdno, (దీర్ఘ)లెక్క_మారింది_buffers());
	    లేకపోతే
		// Return whether the buffer is modified.
		nb_జవాబు_nr(cmdno, (దీర్ఘ)(buf->bufp->b_మారింది
					   || isNetbeansModified(buf->bufp)));
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "saveAndExit"))
	{
	    // Note: this will exit Vim if successful.
	    coloncmd(":confirm qall");

	    // We didn't exit: return the number of changed buffers.
	    nb_జవాబు_nr(cmdno, (దీర్ఘ)లెక్క_మారింది_buffers());
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "getCursor"))
	{
	    char_u పాఠ్యం[200];

	    // Note: nb_getbufno() may return -1.  This indicates the IDE
	    // didn't assign a number to the current buffer in response to a
	    // fileOpened event.
	    sprintf((అక్షరం *)పాఠ్యం, "%d %ld %d %ld",
		    nb_getbufno(curbuf),
		    (దీర్ఘ)curwin->w_cursor.lnum,
		    (పూర్ణాంకం)curwin->w_cursor.నిలువు,
		    pos2off(curbuf, &curwin->w_cursor));
	    nb_జవాబు_పాఠ్యం(cmdno, పాఠ్యం);
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "getAnno"))
	{
	    దీర్ఘ linenum = 0;
#నిర్వచితమైతే FEAT_SIGNS
	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    Invalid buffer identifier in getAnno\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_getanno));
		retval = FAIL;
	    }
	    లేకపోతే
	    {
		పూర్ణాంకం ser_అంకె;

		cp = (అక్షరం *)వాదనలు;
		ser_అంకె = strtol(cp, &cp, 10);
		// If the sign isn't found linenum will be zero.
		linenum = (దీర్ఘ)buf_findsign(buf->bufp, ser_అంకె, ఖాళీ);
	    }
#అయితే_ముగింపు
	    nb_జవాబు_nr(cmdno, linenum);
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "getLength"))
	{
	    దీర్ఘ len = 0;

	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in getLength\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_getlength));
		retval = FAIL;
	    }
	    లేకపోతే
	    {
		len = get_buf_కొలత(buf->bufp);
	    }
	    nb_జవాబు_nr(cmdno, len);
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "getText"))
	{
	    దీర్ఘ	len;
	    linenr_T	nlines;
	    char_u	*పాఠ్యం = ఖాళీ;
	    linenr_T	lno = 1;
	    char_u	*p;
	    char_u	*పంక్తి;

	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in getText\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_gettext));
		retval = FAIL;
	    }
	    లేకపోతే
	    {
		len = get_buf_కొలత(buf->bufp);
		nlines = buf->bufp->b_ml.ml_పంక్తి_లెక్క;
		పాఠ్యం = alloc((len > 0) ? ((len + nlines) * 2) : 4);
		ఒకవేళ (పాఠ్యం == ఖాళీ)
		{
		    nbdebug(("    nb_do_cmd: getText has null text field\n"));
		    retval = FAIL;
		}
		లేకపోతే
		{
		    p = పాఠ్యం;
		    *p++ = '\"';
		    కొరకు (; lno <= nlines ; lno++)
		    {
			పంక్తి = nb_quote(ml_get_buf(buf->bufp, lno, FALSE));
			ఒకవేళ (పంక్తి != ఖాళీ)
			{
			    STRCPY(p, పంక్తి);
			    p += STRLEN(పంక్తి);
			    *p++ = '\\';
			    *p++ = 'n';
			    vim_free(పంక్తి);
			}
		    }
		    *p++ = '\"';
		    *p = '\0';
		}
	    }
	    ఒకవేళ (పాఠ్యం == ఖాళీ)
		nb_జవాబు_పాఠ్యం(cmdno, (char_u *)"");
	    లేకపోతే
	    {
		nb_జవాబు_పాఠ్యం(cmdno, పాఠ్యం);
		vim_free(పాఠ్యం);
	    }
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "remove"))
	{
	    దీర్ఘ లెక్క;
	    pos_T మొదటి, చివరి;
	    pos_T *pos;
	    pos_T *తదుపరి;
	    linenr_T del_from_lnum, del_గమ్యం_lnum;  // lines to be deleted as a whole
	    పూర్ణాంకం oldFire = netbeansFireChanges;
	    పూర్ణాంకం oldSuppress = netbeans_Suppress_No_పంక్తులు;
	    పూర్ణాంకం was_మారింది_2;

	    ఒకవేళ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (అక్షరం *) cmd));
		nb_జవాబు_nil(cmdno);
		తిరిగి OK;
	    }

	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in remove\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_remove));
		retval = FAIL;
	    }
	    లేకపోతే
	    {
		netbeansFireChanges = FALSE;
		netbeans_Suppress_No_పంక్తులు = TRUE;

		nb_సమితి_curbuf(buf->bufp);
		was_మారింది_2 = buf->bufp->b_మారింది;
		cp = (అక్షరం *)వాదనలు;
		off = strtol(cp, &cp, 10);
		లెక్క = strtol(cp, &cp, 10);
		వాదనలు = (char_u *)cp;
		// delete "count" chars, starting at "off"
		pos = off2pos(buf->bufp, off);
		ఒకవేళ (!pos)
		{
		    nbdebug(("    !bad position\n"));
		    nb_జవాబు_పాఠ్యం(cmdno, (char_u *)"!bad position");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_పంక్తులు = oldSuppress;
		    తిరిగి FAIL;
		}
		మొదటి = *pos;
		nbdebug(("    FIRST POS: line %ld, col %d\n",
						      మొదటి.lnum, మొదటి.నిలువు));
		pos = off2pos(buf->bufp, off+లెక్క-1);
		ఒకవేళ (!pos)
		{
		    nbdebug(("    !bad count\n"));
		    nb_జవాబు_పాఠ్యం(cmdno, (char_u *)"!bad count");
		    netbeansFireChanges = oldFire;
		    netbeans_Suppress_No_పంక్తులు = oldSuppress;
		    తిరిగి FAIL;
		}
		చివరి = *pos;
		nbdebug(("    LAST POS: line %ld, col %d\n",
							చివరి.lnum, చివరి.నిలువు));
		del_from_lnum = మొదటి.lnum;
		del_గమ్యం_lnum = చివరి.lnum;
		do_update = 1;

		// Get the position of the first byte after the deleted
		// section.  "next" is NULL when deleting to the end of the
		// file.
		తదుపరి = off2pos(buf->bufp, off + లెక్క);

		// Remove part of the first line.
		ఒకవేళ (మొదటి.నిలువు != 0
				|| (తదుపరి != ఖాళీ && మొదటి.lnum == తదుపరి->lnum))
		{
		    ఒకవేళ (మొదటి.lnum != చివరి.lnum
			    || (తదుపరి != ఖాళీ && మొదటి.lnum != తదుపరి->lnum))
		    {
			// remove to the end of the first line
			nb_partialremove(మొదటి.lnum, మొదటి.నిలువు,
							     (colnr_T)MAXCOL);
			ఒకవేళ (మొదటి.lnum == చివరి.lnum)
			{
			    // Partial line to remove includes the end of
			    // line.  Join the line with the next one, have
			    // the next line deleted below.
			    nb_joinlines(మొదటి.lnum, తదుపరి->lnum);
			    del_గమ్యం_lnum = తదుపరి->lnum;
			}
		    }
		    లేకపోతే
		    {
			// remove within one line
			nb_partialremove(మొదటి.lnum, మొదటి.నిలువు, చివరి.నిలువు);
		    }
		    ++del_from_lnum;  // don't delete the first line
		}

		// Remove part of the last line.
		ఒకవేళ (మొదటి.lnum != చివరి.lnum && తదుపరి != ఖాళీ
			&& తదుపరి->నిలువు != 0 && చివరి.lnum == తదుపరి->lnum)
		{
		    nb_partialremove(చివరి.lnum, 0, చివరి.నిలువు);
		    ఒకవేళ (del_from_lnum > మొదటి.lnum)
		    {
			// Join end of last line to start of first line; last
			// line is deleted below.
			nb_joinlines(మొదటి.lnum, చివరి.lnum);
		    }
		    లేకపోతే
			// First line is deleted as a whole, keep the last
			// line.
			--del_గమ్యం_lnum;
		}

		// First is partial line; last line to remove includes
		// the end of line; join first line to line following last
		// line; line following last line is deleted below.
		ఒకవేళ (మొదటి.lnum != చివరి.lnum && del_from_lnum > మొదటి.lnum
			&& తదుపరి != ఖాళీ && చివరి.lnum != తదుపరి->lnum)
		{
		    nb_joinlines(మొదటి.lnum, తదుపరి->lnum);
		    del_గమ్యం_lnum = తదుపరి->lnum;
		}

		// Delete whole lines if there are any.
		ఒకవేళ (del_గమ్యం_lnum >= del_from_lnum)
		{
		    పూర్ణాంకం i;

		    // delete signs from the lines being deleted
		    కొరకు (i = del_from_lnum; i <= del_గమ్యం_lnum; i++)
		    {
			పూర్ణాంకం id = buf_findsign_id(buf->bufp, (linenr_T)i, ఖాళీ);
			ఒకవేళ (id > 0)
			{
			    nbdebug(("    Deleting sign %d on line %d\n",
								      id, i));
			    buf_delsign(buf->bufp, 0, id, ఖాళీ);
			}
			లేకపోతే
			{
			    nbdebug(("    No sign on line %d\n", i));
			}
		    }

		    nbdebug(("    Deleting lines %ld through %ld\n",
						 del_from_lnum, del_గమ్యం_lnum));
		    curwin->w_cursor.lnum = del_from_lnum;
		    curwin->w_cursor.నిలువు = 0;
		    del_పంక్తులు(del_గమ్యం_lnum - del_from_lnum + 1, FALSE);
		}

		// Leave cursor at first deleted byte.
		curwin->w_cursor = మొదటి;
		check_cursor_lnum();
		buf->bufp->b_మారింది = was_మారింది_2; // logically unchanged
		netbeansFireChanges = oldFire;
		netbeans_Suppress_No_పంక్తులు = oldSuppress;

		u_clearallandblockfree(buf->bufp);
	    }
	    nb_జవాబు_nil(cmdno);
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "insert"))
	{
	    char_u	*గమ్యం_free;

	    ఒకవేళ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (అక్షరం *) cmd));
		nb_జవాబు_nil(cmdno);
		తిరిగి OK;
	    }

	    // get offset
	    cp = (అక్షరం *)వాదనలు;
	    off = strtol(cp, &cp, 10);
	    వాదనలు = (char_u *)cp;

	    // get text to be inserted
	    వాదనలు = skipwhite(వాదనలు);
	    వాదనలు = గమ్యం_free = (char_u *)nb_unquote(వాదనలు, ఖాళీ);
	    /*
	    nbdebug(("    CHUNK[%d]: %d bytes at offset %d\n",
		    buf->bufp->b_ml.ml_line_count, STRLEN(args), off));
	    */

	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in insert\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_చొప్పించు));
		retval = FAIL;
	    }
	    లేకపోతే ఒకవేళ (వాదనలు != ఖాళీ)
	    {
		పూర్ణాంకం	ff_detected = EOL_UNKNOWN;
		పూర్ణాంకం	buf_was_ఖాళీలు = (buf->bufp->b_ml.ml_flags & ML_ఖాళీలు);
		size_t	len = 0;
		పూర్ణాంకం	added = 0;
		పూర్ణాంకం	oldFire = netbeansFireChanges;
		పూర్ణాంకం	old_b_మారింది;
		char_u	*nlp;
		linenr_T lnum;
		linenr_T lnum_ప్రారంభం;
		pos_T	*pos;

		netbeansFireChanges = 0;

		// Jump to the buffer where we insert.  After this "curbuf"
		// can be used.
		nb_సమితి_curbuf(buf->bufp);
		old_b_మారింది = curbuf->b_మారింది;

		// Convert the specified character offset into a lnum/col
		// position.
		pos = off2pos(curbuf, off);
		ఒకవేళ (pos != ఖాళీ)
		{
		    ఒకవేళ (pos->lnum <= 0)
			lnum_ప్రారంభం = 1;
		    లేకపోతే
			lnum_ప్రారంభం = pos->lnum;
		}
		లేకపోతే
		{
		    // If the given position is not found, assume we want
		    // the end of the file.  See setLocAndSize HACK.
		    ఒకవేళ (buf_was_ఖాళీలు)
			lnum_ప్రారంభం = 1;	    // above empty line
		    లేకపోతే
			lnum_ప్రారంభం = curbuf->b_ml.ml_పంక్తి_లెక్క + 1;
		}

		// "lnum" is the line where we insert: either append to it or
		// insert a new line above it.
		lnum = lnum_ప్రారంభం;

		// Loop over the "\n" separated lines of the argument.
		do_update = 1;
		అయితే (*వాదనలు != NUL)
		{
		    nlp = vim_strchr(వాదనలు, '\n');
		    ఒకవేళ (nlp == ఖాళీ)
		    {
			// Incomplete line, probably truncated.  Next "insert"
			// command should append to this one.
			len = STRLEN(వాదనలు);
		    }
		    లేకపోతే
		    {
			len = nlp - వాదనలు;

			/*
			 * We need to detect EOL style, because the commands
			 * use a character offset.
			 */
			ఒకవేళ (nlp > వాదనలు && nlp[-1] == '\r')
			{
			    ff_detected = EOL_DOS;
			    --len;
			}
			లేకపోతే
			    ff_detected = EOL_UNIX;
		    }
		    వాదనలు[len] = NUL;

		    ఒకవేళ (lnum == lnum_ప్రారంభం
			    && ((pos != ఖాళీ && pos->నిలువు > 0)
				|| (lnum == 1 && buf_was_ఖాళీలు)))
		    {
			char_u	*oldline = ml_get(lnum);
			char_u	*newline;
			పూర్ణాంకం	నిలువు = pos == ఖాళీ ? 0 : pos->నిలువు;

			// Insert halfway a line.
			newline = alloc(ml_get_len(lnum) + len + 1);
			ఒకవేళ (newline != ఖాళీ)
			{
			    mch_memmove(newline, oldline, (size_t)నిలువు);
			    newline[నిలువు] = NUL;
			    STRCAT(newline, వాదనలు);
			    STRCAT(newline, oldline + నిలువు);
			    ml_replace(lnum, newline, FALSE);
			}
		    }
		    లేకపోతే
		    {
			// Append a new line.  Not that we always do this,
			// also when the text doesn't end in a "\n".
			ml_append((linenr_T)(lnum - 1), వాదనలు,
						   (colnr_T)(len + 1), FALSE);
			++added;
		    }

		    ఒకవేళ (nlp == ఖాళీ)
			ఆపు;
		    ++lnum;
		    వాదనలు = nlp + 1;
		}

		// Adjust the marks below the inserted lines.
		appended_పంక్తులు_mark(lnum_ప్రారంభం - 1, (దీర్ఘ)added);

		/*
		 * When starting with an empty buffer set the fileformat.
		 * This is just guessing...
		 */
		ఒకవేళ (buf_was_ఖాళీలు)
		{
		    ఒకవేళ (ff_detected == EOL_UNKNOWN)
#ఒకవేళ defined(MSWIN)
			ff_detected = EOL_DOS;
#లేకపోతే
			ff_detected = EOL_UNIX;
#అయితే_ముగింపు
		    సమితి_fileformat(ff_detected, OPT_LOCAL);
		    curbuf->b_ప్రారంభం_ffc = *curbuf->b_p_ff;
		}

		/*
		 * XXX - GRP - Is the next line right? If I've inserted
		 * text the buffer has been updated but not written. Will
		 * netbeans guarantee to write it? Even if I do a :q! ?
		 */
		curbuf->b_మారింది = old_b_మారింది; // logically unchanged
		netbeansFireChanges = oldFire;

		// Undo info is invalid now...
		u_clearallandblockfree(curbuf);
	    }
	    vim_free(గమ్యం_free);
	    nb_జవాబు_nil(cmdno); // or !error
	}
	లేకపోతే
	{
	    nbdebug(("UNIMPLEMENTED FUNCTION: %s\n", cmd));
	    nb_జవాబు_nil(cmdno);
	    retval = FAIL;
	}
    }
    లేకపోతే // Not a function; no reply required.
    {
// =====================================================================
	ఒకవేళ (streq((అక్షరం *)cmd, "create"))
	{
	    // Create a buffer without a name.
	    ఒకవేళ (buf == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in create\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_create));
		తిరిగి FAIL;
	    }
	    VIM_CLEAR(buf->displayname);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, ఖాళీ, 0, 0, ECMD_ONE, ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
	    buf->చొప్పించు_పూర్తి = FALSE;
#ఒకవేళ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ఒకవేళ (gui.లోపల_use)
		gui_update_menus(0);
#అయితే_ముగింపు
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "insertDone"))
	{
	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in insertDone\n"));
	    }
	    లేకపోతే
	    {
		buf->bufp->b_ప్రారంభం_eol = *వాదనలు == 'T';
		buf->చొప్పించు_పూర్తి = TRUE;
		వాదనలు += 2;
		buf->bufp->b_p_ro = *వాదనలు == 'T';
		print_read_సందేశం(buf);
	    }
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "saveDone"))
	{
	    దీర్ఘ savedChars = atol((అక్షరం *)వాదనలు);

	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
		nbdebug(("    invalid buffer identifier in saveDone\n"));
	    లేకపోతే
		print_save_సందేశం(buf, savedChars);
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "startDocumentListen"))
	{
	    ఒకవేళ (buf == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in startDocumentListen\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_startdocumentlisten));
		తిరిగి FAIL;
	    }
	    buf->fireChanges = 1;
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "stopDocumentListen"))
	{
	    ఒకవేళ (buf == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in stopDocumentListen\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_stopdocumentlisten));
		తిరిగి FAIL;
	    }
	    buf->fireChanges = 0;
	    ఒకవేళ (buf->bufp != ఖాళీ && buf->bufp->b_was_netbeans_file)
	    {
		ఒకవేళ (!buf->bufp->b_netbeans_file)
		{
		    nbdebug((e_netbeans_connection_కోల్పోయిన_for_బఫరు_nr,
							   buf->bufp->b_fnum));
		    semsg(_(e_netbeans_connection_కోల్పోయిన_for_బఫరు_nr),
							   buf->bufp->b_fnum);
		}
		లేకపోతే
		{
		    // NetBeans uses stopDocumentListen when it stops editing
		    // a file.  It then expects the buffer in Vim to
		    // disappear.
		    do_bufdel(DOBUF_DEL, (char_u *)"", 1,
				  buf->bufp->b_fnum, buf->bufp->b_fnum, TRUE);
		    CLEAR_సూచిక(buf);
		}
	    }
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "setTitle"))
	{
	    ఒకవేళ (buf == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in setTitle\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_settitle));
		తిరిగి FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(వాదనలు, ఖాళీ);
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "initDone"))
	{
	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in initDone\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_initdone));
		తిరిగి FAIL;
	    }
	    do_update = 1;
	    buf->init_పూర్తి = TRUE;
	    nb_సమితి_curbuf(buf->bufp);
	    apply_autocmds(EVENT_BUFREADPOST, 0, 0, FALSE, buf->bufp);

	    // handle any postponed key commands
	    handle_కీలకం_వరుస();
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "setBufferNumber")
		|| streq((అక్షరం *)cmd, "putBufferNumber"))
	{
	    char_u	*మార్గం;
	    buf_T	*bufp;

	    ఒకవేళ (buf == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in setBufferNumber\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_setbuffernumber));
		తిరిగి FAIL;
	    }
	    మార్గం = (char_u *)nb_unquote(వాదనలు, ఖాళీ);
	    ఒకవేళ (మార్గం == ఖాళీ)
		తిరిగి FAIL;
	    bufp = buflist_findname(మార్గం);
	    vim_free(మార్గం);
	    ఒకవేళ (bufp == ఖాళీ)
	    {
		nbdebug(("    File %s not found in setBufferNumber\n", వాదనలు));
		semsg(_(e_file_str_not_దొరికింది_లోపల_setbuffernumber), వాదనలు);
		తిరిగి FAIL;
	    }
	    buf->bufp = bufp;
	    buf->nbbuf_సంఖ్య = bufp->b_fnum;

	    // "setBufferNumber" has the side effect of jumping to the buffer
	    // (don't know why!).  Don't do that for "putBufferNumber".
	    ఒకవేళ (*cmd != 'p')
		coloncmd(":buffer %d", bufp->b_fnum);
	    లేకపోతే
	    {
		buf->init_పూర్తి = TRUE;

		// handle any postponed key commands
		handle_కీలకం_వరుస();
	    }

// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "setFullName"))
	{
	    ఒకవేళ (buf == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in setFullName\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_setfullname));
		తిరిగి FAIL;
	    }
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(వాదనలు, ఖాళీ);

	    netbeansReadFile = 0; // don't try to open disk file
	    do_ecmd(0, (char_u *)buf->displayname, 0, 0, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    netbeansReadFile = 1;
	    buf->bufp = curbuf;
	    maketitle();
#ఒకవేళ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ఒకవేళ (gui.లోపల_use)
		gui_update_menus(0);
#అయితే_ముగింపు
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "editFile"))
	{
	    ఒకవేళ (buf == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in editFile\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_editfile));
		తిరిగి FAIL;
	    }
	    // Edit a file: like create + setFullName + read the file.
	    vim_free(buf->displayname);
	    buf->displayname = nb_unquote(వాదనలు, ఖాళీ);
	    do_ecmd(0, (char_u *)buf->displayname, ఖాళీ, ఖాళీ, ECMD_ONE,
					     ECMD_HIDE + ECMD_OLDBUF, curwin);
	    buf->bufp = curbuf;
	    buf->init_పూర్తి = TRUE;
	    do_update = 1;
	    maketitle();
#ఒకవేళ defined(FEAT_MENU) && defined(FEAT_GUI)
	    ఒకవేళ (gui.లోపల_use)
		gui_update_menus(0);
#అయితే_ముగింపు
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "setVisible"))
	{
	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in setVisible\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ఒకవేళ (p_verbose > 0)
		    emsg(_(e_invalid_బఫరు_identifier_లోపల_setvisible));
		తిరిగి FAIL;
	    }
	    ఒకవేళ (streq((అక్షరం *)వాదనలు, "T") && buf->bufp != curbuf)
	    {
		exarg_T exarg;
		CLEAR_క్షేత్రం(exarg);
		exarg.cmd = (char_u *)"goto";
		exarg.forceit = FALSE;
		exarg.cmdidx = CMD_USER;
		dosetvisible = TRUE;
		goto_బఫరు(&exarg, DOBUF_మొదటి, FORWARD, buf->bufp->b_fnum);
		do_update = 1;
		dosetvisible = FALSE;

#నిర్వచితమైతే FEAT_GUI
		// Side effect!!!.
		ఒకవేళ (gui.లోపల_use)
		    gui_mch_సమితి_foreground();
#అయితే_ముగింపు
	    }
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "raise"))
	{
#నిర్వచితమైతే FEAT_GUI
	    // Bring gvim to the foreground.
	    ఒకవేళ (gui.లోపల_use)
		gui_mch_సమితి_foreground();
#అయితే_ముగింపు
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "setModified"))
	{
	    పూర్ణాంకం prev_b_మారింది;

	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in setModified\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ఒకవేళ (p_verbose > 0)
		    emsg(_(e_invalid_బఫరు_identifier_లోపల_setmodified));
		తిరిగి FAIL;
	    }
	    prev_b_మారింది = buf->bufp->b_మారింది;
	    ఒకవేళ (streq((అక్షరం *)వాదనలు, "T"))
		buf->bufp->b_మారింది = TRUE;
	    లేకపోతే
	    {
		stat_T	st;

		// Assume NetBeans stored the file.  Reset the timestamp to
		// avoid "file changed" warnings.
		ఒకవేళ (buf->bufp->b_ffname != ఖాళీ
			&& mch_stat((అక్షరం *)buf->bufp->b_ffname, &st) >= 0)
		    buf_store_సమయం(buf->bufp, &st, buf->bufp->b_ffname);
		buf->bufp->b_మారింది = FALSE;
	    }
	    buf->modified = buf->bufp->b_మారింది;
	    ఒకవేళ (prev_b_మారింది != buf->bufp->b_మారింది)
	    {
		check_స్థితి(buf->bufp);
		redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
		redraw_tabpanel = TRUE;
#అయితే_ముగింపు
		maketitle();
		update_screen(0);
	    }
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "setModtime"))
	{
	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
		nbdebug(("    invalid buffer identifier in setModtime\n"));
	    లేకపోతే
	    {
		buf->bufp->b_mtime = atoi((అక్షరం *)వాదనలు);
		buf->bufp->b_mtime_ns = 0;
	    }
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "setReadOnly"))
	{
	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
		nbdebug(("    invalid buffer identifier in setReadOnly\n"));
	    లేకపోతే ఒకవేళ (streq((అక్షరం *)వాదనలు, "T"))
		buf->bufp->b_p_ro = TRUE;
	    లేకపోతే
		buf->bufp->b_p_ro = FALSE;
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "setMark"))
	{
	    // not yet
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "showBalloon"))
	{
#ఒకవేళ defined(FEAT_BEVAL_GUI)
	    స్థిర అక్షరం	*పాఠ్యం = ఖాళీ;

	    /*
	     * Set up the Balloon Expression Evaluation area.
	     * Ignore 'ballooneval' here.
	     * The text pointer must remain valid for a while.
	     */
	    ఒకవేళ (balloonEval != ఖాళీ)
	    {
		vim_free(పాఠ్యం);
		పాఠ్యం = nb_unquote(వాదనలు, ఖాళీ);
		ఒకవేళ (పాఠ్యం != ఖాళీ)
		    gui_mch_post_balloon(balloonEval, (char_u *)పాఠ్యం);
	    }
#అయితే_ముగింపు
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "setDot"))
	{
	    pos_T *pos;
#నిర్వచితమైతే NBDEBUG
	    char_u *s;
#అయితే_ముగింపు

	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in setDot\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_setdot));
		తిరిగి FAIL;
	    }

	    nb_సమితి_curbuf(buf->bufp);

	    // Don't want Visual mode now.
	    ఒకవేళ (VIsual_చురుకైన)
		చివర_visual_బహుళకం();
#నిర్వచితమైతే NBDEBUG
	    s = వాదనలు;
#అయితే_ముగింపు
	    pos = get_off_or_lnum(buf->bufp, &వాదనలు);
	    ఒకవేళ (pos)
	    {
		curwin->w_cursor = *pos;
		check_cursor();
#నిర్వచితమైతే FEAT_FOLDING
		foldOpenCursor();
#అయితే_ముగింపు
	    }
	    లేకపోతే
	    {
		nbdebug(("    BAD POSITION in setDot: %s\n", s));
	    }

	    // gui_update_cursor(TRUE, FALSE);
	    // update_curbuf(UPD_NOT_VALID);
	    update_topline();		// scroll to show the line
	    update_screen(UPD_VALID);
	    setcursor();
	    cursor_on();
	    బహిర్గతం_flush_cursor(TRUE, FALSE);

	    // Quit a hit-return or more prompt.
	    ఒకవేళ (State == బహుళకం_HITRETURN || State == బహుళకం_ASKMORE)
	    {
#నిర్వచితమైతే FEAT_GUI_GTK
		ఒకవేళ (gui.లోపల_use && gtk_main_level() > 0)
		    gtk_main_quit();
#అయితే_ముగింపు
	    }
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "close"))
	{
#నిర్వచితమైతే NBDEBUG
	    అక్షరం *name = "<NONE>";
#అయితే_ముగింపు

	    ఒకవేళ (buf == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_close));
		తిరిగి FAIL;
	    }

#నిర్వచితమైతే NBDEBUG
	    ఒకవేళ (buf->displayname != ఖాళీ)
		name = buf->displayname;
#అయితే_ముగింపు
	    ఒకవేళ (buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in close\n"));
		// This message was commented out, probably because it can
		// happen when shutting down.
		ఒకవేళ (p_verbose > 0)
		    emsg(_(e_invalid_బఫరు_identifier_లోపల_close));
	    }
	    nbdebug(("    CLOSE %d: %s\n", bufno, name));
#నిర్వచితమైతే FEAT_GUI
	    need_mouse_correct = TRUE;
#అయితే_ముగింపు
	    ఒకవేళ (buf->bufp != ఖాళీ)
		do_బఫరు(DOBUF_WIPE, DOBUF_మొదటి, FORWARD,
						     buf->bufp->b_fnum, TRUE);
	    buf->bufp = ఖాళీ;
	    buf->init_పూర్తి = FALSE;
	    do_update = 1;
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "setStyle")) // obsolete...
	{
	    nbdebug(("    setStyle is obsolete!\n"));
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "setExitDelay"))
	{
	    // Only used in version 2.1.
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "defineAnnoType"))
	{
#నిర్వచితమైతే FEAT_SIGNS
	    పూర్ణాంకం type_అంకె;
	    char_u *typeName;
	    char_u *tooltip;
	    char_u *p;
	    char_u *glyphFile;
	    పూర్ణాంకం parse_error = FALSE;
	    char_u *fg;
	    char_u *bg;

	    ఒకవేళ (buf == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in defineAnnoType\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_defineannotype));
		తిరిగి FAIL;
	    }

	    cp = (అక్షరం *)వాదనలు;
	    type_అంకె = strtol(cp, &cp, 10);
	    వాదనలు = (char_u *)cp;
	    వాదనలు = skipwhite(వాదనలు);
	    typeName = (char_u *)nb_unquote(వాదనలు, &వాదనలు);
	    వాదనలు = skipwhite(వాదనలు + 1);
	    tooltip = (char_u *)nb_unquote(వాదనలు, &వాదనలు);
	    వాదనలు = skipwhite(వాదనలు + 1);

	    p = (char_u *)nb_unquote(వాదనలు, &వాదనలు);
	    glyphFile = vim_strsave_escaped(p, escape_chars);
	    vim_free(p);

	    వాదనలు = skipwhite(వాదనలు + 1);
	    p = skiptowhite(వాదనలు);
	    ఒకవేళ (*p != NUL)
	    {
		*p = NUL;
		p = skipwhite(p + 1);
	    }
	    fg = vim_strsave(వాదనలు);
	    bg = vim_strsave(p);
	    ఒకవేళ (STRLEN(fg) > MAX_COLOR_నిడివి || STRLEN(bg) > MAX_COLOR_నిడివి)
	    {
		emsg(_(e_highlighting_color_name_too_long_లోపల_define_Anno_Type));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    లేకపోతే ఒకవేళ (!nb_is_సురక్షిత_string(typeName, VALID_SIGNNAME_CHARS) ||
		    (*fg != NUL && !nb_is_సురక్షిత_string(fg, VALID_COLOR_CHARS)) ||
		    (*bg != NUL && !nb_is_సురక్షిత_string(bg, VALID_COLOR_CHARS)))
	    {
		nbdebug(("    invalid chars in typeName/fg/bg in defineAnnoType\n"));
		emsg(_(e_invalid_identifier_లోపల_defineannotype));
		VIM_CLEAR(typeName);
		parse_error = TRUE;
	    }
	    లేకపోతే ఒకవేళ (typeName != ఖాళీ && tooltip != ఖాళీ && glyphFile != ఖాళీ)
		addsigntype(buf, type_అంకె, typeName, tooltip, glyphFile, fg, bg);

	    vim_free(typeName);
	    vim_free(fg);
	    vim_free(bg);
	    vim_free(tooltip);
	    vim_free(glyphFile);
	    ఒకవేళ (parse_error)
		తిరిగి FAIL;

#అయితే_ముగింపు
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "addAnno"))
	{
#నిర్వచితమైతే FEAT_SIGNS
	    పూర్ణాంకం ser_అంకె;
	    పూర్ణాంకం local_Type_అంకె;
	    పూర్ణాంకం type_అంకె;
	    pos_T *pos;

	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in addAnno\n"));
		emsg(_(e_invalid_బఫరు_identifier_లోపల_addanno));
		తిరిగి FAIL;
	    }

	    do_update = 1;

	    cp = (అక్షరం *)వాదనలు;
	    ser_అంకె = strtol(cp, &cp, 10);

	    // Get the typenr specific for this buffer and convert it to
	    // the global typenumber, as used for the sign name.
	    local_Type_అంకె = strtol(cp, &cp, 10);
	    వాదనలు = (char_u *)cp;
	    type_అంకె = mapsigntype(buf, local_Type_అంకె);

	    pos = get_off_or_lnum(buf->bufp, &వాదనలు);

	    cp = (అక్షరం *)వాదనలు;
	    vim_ignored = (పూర్ణాంకం)strtol(cp, &cp, 10);
	    వాదనలు = (char_u *)cp;
# ifdef NBDEBUG
	    ఒకవేళ (vim_ignored != -1)
		nbdebug(("    partial line annotation -- Not Yet Implemented!\n"));
# endif
	    ఒకవేళ (ser_అంకె >= GUARDEDOFFSET)
	    {
		nbdebug(("    too many annotations! ignoring...\n"));
		తిరిగి FAIL;
	    }
	    ఒకవేళ (pos)
	    {
		coloncmd(":sign place %d line=%ld name=%d buffer=%d",
			   ser_అంకె, pos->lnum, type_అంకె, buf->bufp->b_fnum);
		ఒకవేళ (type_అంకె == curPCtype)
		    coloncmd(":sign jump %d buffer=%d", ser_అంకె,
						       buf->bufp->b_fnum);
	    }
#అయితే_ముగింపు
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "removeAnno"))
	{
#నిర్వచితమైతే FEAT_SIGNS
	    పూర్ణాంకం ser_అంకె;

	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in removeAnno\n"));
		తిరిగి FAIL;
	    }
	    do_update = 1;
	    cp = (అక్షరం *)వాదనలు;
	    ser_అంకె = strtol(cp, &cp, 10);
	    వాదనలు = (char_u *)cp;
	    coloncmd(":sign unplace %d buffer=%d",
		     ser_అంకె, buf->bufp->b_fnum);
	    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
#అయితే_ముగింపు
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "moveAnnoToFront"))
	{
#నిర్వచితమైతే FEAT_SIGNS
	    nbdebug(("    moveAnnoToFront: Not Yet Implemented!\n"));
#అయితే_ముగింపు
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "guard") || streq((అక్షరం *)cmd, "unguard"))
	{
	    పూర్ణాంకం len;
	    pos_T మొదటి;
	    pos_T చివరి;
	    pos_T *pos;
	    పూర్ణాంకం un = (cmd[0] == 'u');
	    స్థిర పూర్ణాంకం guardId = GUARDEDOFFSET;

	    ఒకవేళ (skip >= SKIP_STOP)
	    {
		nbdebug(("    Skipping %s command\n", (అక్షరం *) cmd));
		తిరిగి OK;
	    }

	    nb_init_graphics();

	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		తిరిగి FAIL;
	    }
	    nb_సమితి_curbuf(buf->bufp);
	    cp = (అక్షరం *)వాదనలు;
	    off = strtol(cp, &cp, 10);
	    len = strtol(cp, ఖాళీ, 10);
	    వాదనలు = (char_u *)cp;
	    pos = off2pos(buf->bufp, off);
	    do_update = 1;
	    ఒకవేళ (!pos)
		nbdebug(("    no such start pos in %s, %ld\n", cmd, off));
	    లేకపోతే
	    {
		మొదటి = *pos;
		pos = off2pos(buf->bufp, off + len - 1);
		ఒకవేళ (pos != ఖాళీ && pos->నిలువు == 0)
		{
			/*
			 * In Java Swing the offset is a position between 2
			 * characters. If col == 0 then we really want the
			 * previous line as the end.
			 */
			pos = off2pos(buf->bufp, off + len - 2);
		}
		ఒకవేళ (!pos)
		    nbdebug(("    no such end pos in %s, %ld\n",
			    cmd, off + len - 1));
		లేకపోతే
		{
		    దీర్ఘ lnum;
		    చివరి = *pos;
		    // set highlight for region
		    nbdebug(("    %sGUARD %ld,%d to %ld,%d\n", (un) ? "UN" : "",
			     మొదటి.lnum, మొదటి.నిలువు,
			     చివరి.lnum, చివరి.నిలువు));
#నిర్వచితమైతే FEAT_SIGNS
		    కొరకు (lnum = మొదటి.lnum; lnum <= చివరి.lnum; lnum++)
		    {
			ఒకవేళ (un)
			{
			    // never used
			}
			లేకపోతే
			{
			    ఒకవేళ (buf_findsigntype_id(buf->bufp, lnum,
				GUARDED) == 0)
			    {
				coloncmd(
				    ":sign place %d line=%ld name=%d buffer=%d",
				     guardId++, lnum, GUARDED,
				     buf->bufp->b_fnum);
			    }
			}
		    }
#అయితే_ముగింపు
		    redraw_buf_later(buf->bufp, UPD_NOT_VALID);
		}
	    }
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "startAtomic"))
	{
	    లోపల_Atomic = 1;
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "endAtomic"))
	{
	    లోపల_Atomic = 0;
	    ఒకవేళ (needupdate)
	    {
		do_update = 1;
		needupdate = 0;
	    }
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "save"))
	{
	    /*
	     * NOTE - This command is obsolete wrt NetBeans. It's left in
	     * only for historical reasons.
	     */
	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		తిరిగి FAIL;
	    }

	    // the following is taken from ex_cmds.c (do_wqall function)
	    ఒకవేళ (buf_Is_మారింది(buf->bufp))
	    {
		// Only write if the buffer can be written.
		ఒకవేళ (p_write
			&& !buf->bufp->b_p_ro
			&& buf->bufp->b_ffname != ఖాళీ
			&& !bt_dontwrite(buf->bufp))
		{
		    bufref_T bufref;

		    సమితి_bufref(&bufref, buf->bufp);
		    buf_write_all(buf->bufp, FALSE);
		    // an autocommand may have deleted the buffer
		    ఒకవేళ (!bufref_valid(&bufref))
			buf->bufp = ఖాళీ;
		}
	    }
	    లేకపోతే
	    {
		nbdebug(("    Buffer has no changes!\n"));
	    }
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "netbeansBuffer"))
	{
	    ఒకవేళ (buf == ఖాళీ || buf->bufp == ఖాళీ)
	    {
		nbdebug(("    invalid buffer identifier in %s command\n", cmd));
		తిరిగి FAIL;
	    }
	    ఒకవేళ (*వాదనలు == 'T')
	    {
		buf->bufp->b_netbeans_file = true;
		buf->bufp->b_was_netbeans_file = true;
	    }
	    లేకపోతే
		buf->bufp->b_netbeans_file = false;
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "specialKeys"))
	{
	    special_కీలకాలు(వాదనలు);
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "actionMenuItem"))
	{
	    // not used yet
// =====================================================================
	}
	లేకపోతే ఒకవేళ (streq((అక్షరం *)cmd, "version"))
	{
	    // not used yet
	}
	లేకపోతే
	{
	    nbdebug(("Unrecognised command: %s\n", cmd));
	}
	/*
	 * Unrecognized command is ignored.
	 */
    }
    ఒకవేళ (లోపల_Atomic && do_update)
    {
	needupdate = 1;
	do_update = 0;
    }

    /*
     * Is this needed? I moved the netbeans_Xt_connect() later during startup
     * and it may no longer be necessary. If it's not needed then needupdate
     * and do_update can also be removed.
     */
    ఒకవేళ (buf != ఖాళీ && buf->init_పూర్తి && do_update)
    {
	update_screen(UPD_NOT_VALID);
	setcursor();
	cursor_on();
	బహిర్గతం_flush_cursor(TRUE, FALSE);

	// Quit a hit-return or more prompt.
	ఒకవేళ (State == బహుళకం_HITRETURN || State == బహుళకం_ASKMORE)
	{
#నిర్వచితమైతే FEAT_GUI_GTK
	    ఒకవేళ (gui.లోపల_use && gtk_main_level() > 0)
		gtk_main_quit();
#అయితే_ముగింపు
	}
    }

    తిరిగి retval;
}


/*
 * If "buf" is not the current buffer try changing to a window that edits this
 * buffer.  If there is no such window then close the current buffer and set
 * the current buffer as "buf".
 */
    స్థిర శూన్య
nb_సమితి_curbuf(buf_T *buf)
{
    ఒకవేళ (curbuf == buf)
	తిరిగి;

    ఒకవేళ (buf_jump_open_win(buf) != ఖాళీ)
	తిరిగి;
    ఒకవేళ ((swb_flags & SWB_USETAB) && buf_jump_open_tab(buf) != ఖాళీ)
	తిరిగి;
    సమితి_curbuf(buf, DOBUF_GOTO);
}

/*
 * Process a vim colon command.
 */
    స్థిర శూన్య
coloncmd(అక్షరం *cmd, ...)
{
    అక్షరం buf[1024];
    va_list ap;

    va_start(ap, cmd);
    vim_vsnprintf(buf, పరిమాణం(buf), cmd, ap);
    va_end(ap);

    nbdebug(("    COLONCMD %s\n", buf));

    do_cmdline((char_u *)buf, ఖాళీ, ఖాళీ, DOCMD_NOWAIT | DOCMD_KEYTYPED);

    setcursor();		// restore the cursor position
    బహిర్గతం_flush_cursor(TRUE, FALSE);
}


/*
 * Parse the specialKeys argument and issue the appropriate map commands.
 */
    స్థిర శూన్య
special_కీలకాలు(char_u *వాదనలు)
{
    అక్షరం *save_str = nb_unquote(వాదనలు, ఖాళీ);
    అక్షరం *tok = strtok(save_str, " ");
    అక్షరం *sep;
#నిర్వచించు KEYBUFLEN 64
    అక్షరం keybuf[KEYBUFLEN];
    అక్షరం cmdbuf[256];

    అయితే (tok != ఖాళీ)
    {
	పూర్ణాంకం i = 0;

	ఒకవేళ ((sep = strchr(tok, '-')) != ఖాళీ)
	{
	    *sep = NUL;
	    అయితే (*tok && i + 2 < KEYBUFLEN)
	    {
		ఎంపిక (*tok)
		{
		    స్థితి 'A':
		    స్థితి 'M':
		    స్థితి 'C':
		    స్థితి 'S':
			keybuf[i++] = *tok;
			keybuf[i++] = '-';
			ఆపు;
		}
		tok++;
	    }
	    tok++;
	}

	ఒకవేళ (అక్షరశ్రేణి_పొడవు(tok) + i < KEYBUFLEN)
	{
	    // Only allow alphanumeric and function-key name characters.
	    // Reject anything else to prevent map command injection.
	    పూర్ణాంకం సురక్షిత = TRUE;
	    కొరకు (char_u *tp = (char_u *)tok; *tp != NUL; tp++)
	    {
		ఒకవేళ (!ASCII_ISALNUM(*tp) && *tp != '-')
		{
		    సురక్షిత = FALSE;
		    ఆపు;
		}
	    }
	    ఒకవేళ (సురక్షిత)
	    {
		vim_strncpy((char_u *)&keybuf[i], (char_u *)tok, KEYBUFLEN - i - 1);
		vim_snprintf(cmdbuf, పరిమాణం(cmdbuf),
				"<silent><%s> :nbkey %s<CR>", keybuf, keybuf);
		do_map(MAPTYPE_MAP, (char_u *)cmdbuf, బహుళకం_సాధారణ, FALSE);
	    }
	}
	tok = strtok(ఖాళీ, " ");
    }
    vim_free(save_str);
}

    శూన్య
ex_nbclose(exarg_T *eap వాడని)
{
    netbeans_close();
}

    శూన్య
ex_nbkey(exarg_T *eap)
{
    (శూన్య)netbeans_keystring(eap->arg);
}

    శూన్య
ex_nbstart(
    exarg_T	*eap)
{
#నిర్వచితమైతే FEAT_GUI
# ఒకవేళ !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)  \
		&& !defined(FEAT_GUI_MSWIN)
    ఒకవేళ (gui.లోపల_use)
    {
	emsg(_(e_netbeans_is_not_supported_with_this_GUI));
	తిరిగి;
    }
# endif
#అయితే_ముగింపు
    netbeans_open((అక్షరం *)eap->arg, FALSE);
}

/*
 * Initialize highlights and signs for use by netbeans  (mostly obsolete)
 */
    స్థిర శూన్య
nb_init_graphics(శూన్య)
{
    స్థిర పూర్ణాంకం did_init = FALSE;

    ఒకవేళ (did_init)
	తిరిగి;

    coloncmd(":highlight NBGuarded guibg=Cyan guifg=Black"
	    " ctermbg=LightCyan ctermfg=Black");
    coloncmd(":sign define %d linehl=NBGuarded", GUARDED);

    did_init = TRUE;
}

/*
 * Convert key to netbeans name.  This uses the global "mod_mask".
 */
    స్థిర శూన్య
netbeans_keyname(పూర్ణాంకం కీలకం, అక్షరం *buf)
{
    అక్షరం *name = 0;
    అక్షరం namebuf[2];
    పూర్ణాంకం ctrl  = 0;
    పూర్ణాంకం జరుపు = 0;
    పూర్ణాంకం alt   = 0;

    ఒకవేళ (mod_ముసుగు & MOD_ముసుగు_CTRL)
	ctrl = 1;
    ఒకవేళ (mod_ముసుగు & MOD_ముసుగు_జరుపు)
	జరుపు = 1;
    ఒకవేళ (mod_ముసుగు & MOD_ముసుగు_ALT)
	alt = 1;


    ఎంపిక (కీలకం)
    {
	స్థితి K_F1:		name = "F1";		ఆపు;
	స్థితి K_S_F1:	name = "F1";	జరుపు = 1;	ఆపు;
	స్థితి K_F2:		name = "F2";		ఆపు;
	స్థితి K_S_F2:	name = "F2";	జరుపు = 1;	ఆపు;
	స్థితి K_F3:		name = "F3";		ఆపు;
	స్థితి K_S_F3:	name = "F3";	జరుపు = 1;	ఆపు;
	స్థితి K_F4:		name = "F4";		ఆపు;
	స్థితి K_S_F4:	name = "F4";	జరుపు = 1;	ఆపు;
	స్థితి K_F5:		name = "F5";		ఆపు;
	స్థితి K_S_F5:	name = "F5";	జరుపు = 1;	ఆపు;
	స్థితి K_F6:		name = "F6";		ఆపు;
	స్థితి K_S_F6:	name = "F6";	జరుపు = 1;	ఆపు;
	స్థితి K_F7:		name = "F7";		ఆపు;
	స్థితి K_S_F7:	name = "F7";	జరుపు = 1;	ఆపు;
	స్థితి K_F8:		name = "F8";		ఆపు;
	స్థితి K_S_F8:	name = "F8";	జరుపు = 1;	ఆపు;
	స్థితి K_F9:		name = "F9";		ఆపు;
	స్థితి K_S_F9:	name = "F9";	జరుపు = 1;	ఆపు;
	స్థితి K_F10:		name = "F10";		ఆపు;
	స్థితి K_S_F10:	name = "F10";	జరుపు = 1;	ఆపు;
	స్థితి K_F11:		name = "F11";		ఆపు;
	స్థితి K_S_F11:	name = "F11";	జరుపు = 1;	ఆపు;
	స్థితి K_F12:		name = "F12";		ఆపు;
	స్థితి K_S_F12:	name = "F12";	జరుపు = 1;	ఆపు;
	అప్రమేయ:
			ఒకవేళ (కీలకం >= ' ' && కీలకం <= '~')
			{
			    // Allow ASCII characters.
			    name = namebuf;
			    namebuf[0] = కీలకం;
			    namebuf[1] = NUL;
			}
			లేకపోతే
			    name = "X";
			ఆపు;
    }

    buf[0] = '\0';
    ఒకవేళ (ctrl)
	strcat(buf, "C");
    ఒకవేళ (జరుపు)
	strcat(buf, "S");
    ఒకవేళ (alt)
	strcat(buf, "M"); // META
    ఒకవేళ (ctrl || జరుపు || alt)
	strcat(buf, "-");
    strcat(buf, name);
}

#ఒకవేళ defined(FEAT_BEVAL)
/*
 * Function to be called for balloon evaluation.  Grabs the text under the
 * cursor and sends it to the debugger for evaluation.  The debugger should
 * respond with a showBalloon command when there is a useful result.
 */
    శూన్య
netbeans_beval_cb(
	BalloonEval	*beval,
	పూర్ణాంకం		 state వాడని)
{
    win_T	*wp;
    char_u	*పాఠ్యం;
    linenr_T	lnum;
    పూర్ణాంకం		నిలువు;
    అక్షరం	*buf;
    char_u	*p;

    // Don't do anything when 'ballooneval' is off, messages scrolled the
    // windows up or we have no connection.
    ఒకవేళ (!can_use_beval() || !NETBEANS_OPEN)
	తిరిగి;

    ఒకవేళ (get_beval_info(beval, TRUE, &wp, &lnum, &పాఠ్యం, &నిలువు) != OK)
	తిరిగి;

    // Send debugger request.  Only when the text is of reasonable
    // length.
    ఒకవేళ (పాఠ్యం != ఖాళీ && పాఠ్యం[0] != NUL && STRLEN(పాఠ్యం) < MAXPATHL)
    {
	buf = alloc(MAXPATHL * 2 + 25);
	ఒకవేళ (buf != ఖాళీ)
	{
	    p = nb_quote(పాఠ్యం);
	    ఒకవేళ (p != ఖాళీ)
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
    vim_free(పాఠ్యం);
}
#అయితే_ముగింపు

/*
 * Return TRUE when the netbeans connection is active.
 */
    పూర్ణాంకం
netbeans_చురుకైన(శూన్య)
{
    తిరిగి NETBEANS_OPEN;
}

/*
 * Tell netbeans that the window was opened, ready for commands.
 */
    శూన్య
netbeans_open(అక్షరం *params, పూర్ణాంకం doabort)
{
    అక్షరం *cmd = "0:startupDone=0\n";

    ఒకవేళ (NETBEANS_OPEN)
    {
	emsg(_(e_netbeans_already_connected));
	తిరిగి;
    }

    ఒకవేళ (netbeans_connect(params, doabort) != OK)
	తిరిగి;

    nbdebug(("EVT: %s", cmd));
    nb_send(cmd, "netbeans_startup_done");

    // update the screen after having added the gutter
    మారింది_కిటికీ_setting();
    update_screen(UPD_CLEAR);
    setcursor();
    cursor_on();
    బహిర్గతం_flush_cursor(TRUE, FALSE);
}

/*
 * Tell netbeans that we're exiting. This should be called right
 * before calling exit.
 */
    శూన్య
netbeans_send_disconnect(శూన్య)
{
    అక్షరం buf[128];

    ఒకవేళ (NETBEANS_OPEN)
    {
	sprintf(buf, "0:disconnect=%d\n", r_cmdno);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_disconnect");
    }
}

#ఒకవేళ defined(FEAT_EVAL)
    పూర్ణాంకం
సమితి_ref_లోపల_nb_channel(పూర్ణాంకం copyID)
{
    పూర్ణాంకం abort = FALSE;
    typval_T tv;

    ఒకవేళ (nb_channel == ఖాళీ)
	తిరిగి FALSE;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = nb_channel;
    abort = సమితి_ref_లోపల_అంశం(&tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
    తిరిగి abort;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN)
/*
 * Tell netbeans that the window was moved or resized.
 */
    శూన్య
netbeans_చట్రం_moved(పూర్ణాంకం new_x, పూర్ణాంకం new_y)
{
    అక్షరం buf[128];

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    sprintf(buf, "0:geometry=%d %d %d %d %d\n",
		    r_cmdno, (పూర్ణాంకం)Columns, (పూర్ణాంకం)అడ్డువరుసలు, new_x, new_y);
    // nbdebug(("EVT: %s", buf)); happens too many times during a move
    nb_send(buf, "netbeans_frame_moved");
}
#అయితే_ముగింపు

/*
 * Tell netbeans the user opened or activated a file.
 */
    శూన్య
netbeans_file_activated(buf_T *bufp)
{
    పూర్ణాంకం bufno = nb_getbufno(bufp);
    nbbuf_T *bp = nb_get_buf(bufno);
    అక్షరం    బఫరు[2*MAXPATHL];
    char_u  *q;

    ఒకవేళ (!NETBEANS_OPEN || !bufp->b_netbeans_file || dosetvisible)
	తిరిగి;

    q = nb_quote(bufp->b_ffname);
    ఒకవేళ (q == ఖాళీ)
	తిరిగి;
    ఒకవేళ (bp == ఖాళీ)
    {
	vim_free(q);
	తిరిగి;
    }

    vim_snprintf(బఫరు, పరిమాణం(బఫరు),  "%d:fileOpened=%d \"%s\" %s %s\n",
	    bufno,
	    bufno,
	    (అక్షరం *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", బఫరు));

    nb_send(బఫరు, "netbeans_file_opened");
}

/*
 * Tell netbeans the user opened a file.
 */
    శూన్య
netbeans_file_opened(buf_T *bufp)
{
    పూర్ణాంకం bufno = nb_getbufno(bufp);
    అక్షరం    బఫరు[2*MAXPATHL];
    char_u  *q;
    nbbuf_T *bp = nb_get_buf(nb_getbufno(bufp));
    పూర్ణాంకం	    bnum;

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    q = nb_quote(bufp->b_ffname);
    ఒకవేళ (q == ఖాళీ)
	తిరిగి;
    ఒకవేళ (bp != ఖాళీ)
	bnum = bufno;
    లేకపోతే
	bnum = 0;

    vim_snprintf(బఫరు, పరిమాణం(బఫరు), "%d:fileOpened=%d \"%s\" %s %s\n",
	    bnum,
	    0,
	    (అక్షరం *)q,
	    "T",  // open in NetBeans
	    "F"); // modified

    vim_free(q);
    nbdebug(("EVT: %s", బఫరు));

    nb_send(బఫరు, "netbeans_file_opened");
    ఒకవేళ (p_acd && vim_chdirfile(bufp->b_ffname, "auto") == OK)
    {
	చివరి_chdir_reason = "netbeans";
	shorten_fnames(TRUE);
    }
}

/*
 * Tell netbeans that a file was deleted or wiped out.
 */
    శూన్య
netbeans_file_killed(buf_T *bufp)
{
    పూర్ణాంకం		bufno = nb_getbufno(bufp);
    nbbuf_T	*nbbuf = nb_get_buf(bufno);
    అక్షరం	బఫరు[2*MAXPATHL];

    ఒకవేళ (!NETBEANS_OPEN || bufno == -1)
	తిరిగి;

    nbdebug(("netbeans_file_killed:\n"));
    nbdebug(("    Killing bufno: %d", bufno));

    sprintf(బఫరు, "%d:killed=%d\n", bufno, r_cmdno);

    nbdebug(("EVT: %s", బఫరు));

    nb_send(బఫరు, "netbeans_file_killed");

    ఒకవేళ (nbbuf != ఖాళీ)
	nbbuf->bufp = ఖాళీ;
}

/*
 * Get a pointer to the Netbeans buffer for Vim buffer "bufp".
 * Return NULL if there is no such buffer or changes are not to be reported.
 * Otherwise store the buffer number in "*bufnop".
 */
    స్థిర nbbuf_T *
nb_bufp2nbbuf_fire(buf_T *bufp, పూర్ణాంకం *bufnop)
{
    పూర్ణాంకం		bufno;
    nbbuf_T	*nbbuf;

    ఒకవేళ (!NETBEANS_OPEN || !netbeansFireChanges)
	తిరిగి ఖాళీ;		// changes are not reported at all

    bufno = nb_getbufno(bufp);
    ఒకవేళ (bufno <= 0)
	తిరిగి ఖాళీ;		// file is not known to NetBeans

    nbbuf = nb_get_buf(bufno);
    ఒకవేళ (nbbuf != ఖాళీ && !nbbuf->fireChanges)
	తిరిగి ఖాళీ;		// changes in this buffer are not reported

    *bufnop = bufno;
    తిరిగి nbbuf;
}

/*
 * Tell netbeans the user inserted some text.
 */
    శూన్య
netbeans_inserted(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	నిలువు,
    char_u	*txt,
    పూర్ణాంకం		newlen)
{
    char_u	*buf;
    పూర్ణాంకం		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    దీర్ఘ	off;
    char_u	*p;
    char_u	*newtxt;

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ఒకవేళ (nbbuf == ఖాళీ)
	తిరిగి;

    // Don't mark as modified for initial read
    ఒకవేళ (nbbuf->చొప్పించు_పూర్తి)
	nbbuf->modified = 1;

    // send the "insert" EVT
    newtxt = alloc(newlen + 1);
    vim_strncpy(newtxt, txt, newlen);

    // Note: this may make "txt" invalid
    pos.lnum = linenr;
    pos.నిలువు = నిలువు;
    off = pos2off(bufp, &pos);

    p = nb_quote(newtxt);
    ఒకవేళ (p != ఖాళీ)
    {
	buf = alloc(128 + 2*newlen);
	sprintf((అక్షరం *)buf, "%d:insert=%d %ld \"%s\"\n",
						      bufno, r_cmdno, off, p);
	nbdebug(("EVT: %s", buf));
	nb_send((అక్షరం *)buf, "netbeans_inserted");
	vim_free(p);
	vim_free(buf);
    }
    vim_free(newtxt);
}

/*
 * Tell netbeans some bytes have been removed.
 */
    శూన్య
netbeans_removed(
    buf_T	*bufp,
    linenr_T	linenr,
    colnr_T	నిలువు,
    దీర్ఘ	len)
{
    char_u	buf[128];
    పూర్ణాంకం		bufno;
    nbbuf_T	*nbbuf;
    pos_T	pos;
    దీర్ఘ	off;

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ఒకవేళ (nbbuf == ఖాళీ)
	తిరిగి;

    ఒకవేళ (len < 0)
    {
	nbdebug(("Negative len %ld in netbeans_removed()!\n", len));
	తిరిగి;
    }

    nbbuf->modified = 1;

    pos.lnum = linenr;
    pos.నిలువు = నిలువు;

    off = pos2off(bufp, &pos);

    sprintf((అక్షరం *)buf, "%d:remove=%d %ld %ld\n", bufno, r_cmdno, off, len);
    nbdebug(("EVT: %s", buf));
    nb_send((అక్షరం *)buf, "netbeans_removed");
}

/*
 * Send netbeans an unmodified command.
 */
    శూన్య
netbeans_unmodified(buf_T *bufp వాడని)
{
    // This is a no-op, because NetBeans considers a buffer modified
    // even when all changes have been undone.
}

/*
 * Send a button release event back to netbeans. It's up to netbeans
 * to decide what to do (if anything) with this event.
 */
    శూన్య
netbeans_button_release(పూర్ణాంకం button)
{
    అక్షరం	buf[128];
    పూర్ణాంకం		bufno;

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    bufno = nb_getbufno(curbuf);

    ఒకవేళ (bufno < 0 || curwin == ఖాళీ || curwin->w_బఫరు != curbuf)
	తిరిగి;

    పూర్ణాంకం నిలువు = mouse_నిలువు - curwin->w_wincol
	- ((curwin->w_p_nu || curwin->w_p_rnu) ? 9 : 1);
    దీర్ఘ off = pos2off(curbuf, &curwin->w_cursor);

    // sync the cursor position
    sprintf(buf, "%d:newDotAndMark=%d %ld %ld\n", bufno, r_cmdno, off, off);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release[newDotAndMark]");

    sprintf(buf, "%d:buttonRelease=%d %d %ld %d\n", bufno, r_cmdno,
	    button, (దీర్ఘ)curwin->w_cursor.lnum, నిలువు);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_button_release");
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key code.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    పూర్ణాంకం
netbeans_keycommand(పూర్ణాంకం కీలకం)
{
    అక్షరం	కీలకం_Name[60];

    netbeans_keyname(కీలకం, కీలకం_Name);
    తిరిగి netbeans_keystring((char_u *)కీలకం_Name);
}


/*
 * Send a keypress event back to netbeans. This usually simulates some
 * kind of function key press. This function operates on a key string.
 * Return TRUE when the key was sent, FALSE when the command has been
 * postponed.
 */
    స్థిర పూర్ణాంకం
netbeans_keystring(char_u *కీలకం_Name)
{
    అక్షరం	buf[2*MAXPATHL];
    పూర్ణాంకం		bufno = nb_getbufno(curbuf);
    దీర్ఘ	off;
    char_u	*q;

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి TRUE;

    ఒకవేళ (bufno == -1)
    {
	nbdebug(("got keycommand for non-NetBeans buffer, opening...\n"));
	q = curbuf->b_ffname == ఖాళీ ? (char_u *)""
						 : nb_quote(curbuf->b_ffname);
	ఒకవేళ (q == ఖాళీ)
	    తిరిగి TRUE;
	vim_snprintf(buf, పరిమాణం(buf), "0:fileOpened=%d \"%s\" %s %s\n", 0,
		q,
		"T",  // open in NetBeans
		"F"); // modified
	ఒకవేళ (curbuf->b_ffname != ఖాళీ)
	    vim_free(q);
	nbdebug(("EVT: %s", buf));
	nb_send(buf, "netbeans_keycommand");

	postpone_keycommand(కీలకం_Name);
	తిరిగి FALSE;
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
    vim_snprintf(buf, పరిమాణం(buf), "%d:keyCommand=%d \"%s\"\n",
						     bufno, r_cmdno, కీలకం_Name);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");

    // New: do both at once and include the lnum/col.
    vim_snprintf(buf, పరిమాణం(buf), "%d:keyAtPos=%d \"%s\" %ld %ld/%ld\n",
	    bufno, r_cmdno, కీలకం_Name,
		off, (దీర్ఘ)curwin->w_cursor.lnum, (దీర్ఘ)curwin->w_cursor.నిలువు);
    nbdebug(("EVT: %s", buf));
    nb_send(buf, "netbeans_keycommand");
    తిరిగి TRUE;
}


/*
 * Send a save event to netbeans.
 */
    శూన్య
netbeans_save_బఫరు(buf_T *bufp)
{
    char_u	buf[64];
    పూర్ణాంకం		bufno;
    nbbuf_T	*nbbuf;

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ఒకవేళ (nbbuf == ఖాళీ)
	తిరిగి;

    nbbuf->modified = 0;

    sprintf((అక్షరం *)buf, "%d:save=%d\n", bufno, r_cmdno);
    nbdebug(("EVT: %s", buf));
    nb_send((అక్షరం *)buf, "netbeans_save_buffer");
}


/*
 * Send remove command to netbeans (this command has been turned off).
 */
    శూన్య
netbeans_deleted_all_పంక్తులు(buf_T *bufp)
{
    char_u	buf[64];
    పూర్ణాంకం		bufno;
    nbbuf_T	*nbbuf;

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    nbbuf = nb_bufp2nbbuf_fire(bufp, &bufno);
    ఒకవేళ (nbbuf == ఖాళీ)
	తిరిగి;

    // Don't mark as modified for initial read
    ఒకవేళ (nbbuf->చొప్పించు_పూర్తి)
	nbbuf->modified = 1;

    sprintf((అక్షరం *)buf, "%d:remove=%d 0 -1\n", bufno, r_cmdno);
    nbdebug(("EVT(suppressed): %s", buf));
//     nb_send(buf, "netbeans_deleted_all_lines");
}


/*
 * See if the lines are guarded. The top and bot parameters are from
 * u_savecommon(), these are the line above the change and the line below the
 * change.
 */
    పూర్ణాంకం
netbeans_is_guarded(linenr_T శిఖరం, linenr_T bot)
{
    sign_నమోదు_T	*p;
    పూర్ణాంకం			lnum;

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి FALSE;

    FOR_ALL_SIGNS_లోపల_BUF(curbuf, p)
	ఒకవేళ (p->se_id >= GUARDEDOFFSET)
	    కొరకు (lnum = శిఖరం + 1; lnum < bot; lnum++)
		ఒకవేళ (lnum == p->se_lnum)
		    తిరిగి TRUE;

    తిరిగి FALSE;
}

#ఒకవేళ defined(FEAT_GUI_X11)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the Motif version.
 */
    శూన్య
netbeans_draw_multisign_indicator(పూర్ణాంకం అడ్డువరుస)
{
    పూర్ణాంకం i;
    పూర్ణాంకం y;
    పూర్ణాంకం x;

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    x = 0;
    y = అడ్డువరుస * gui.char_ఎత్తు + 2;

    కొరకు (i = 0; i < gui.char_ఎత్తు - 3; i++)
	XDraw_బిందువు(gui.dpy, gui.wid, gui.పాఠ్యం_gc, x+2, y++);

    XDraw_బిందువు(gui.dpy, gui.wid, gui.పాఠ్యం_gc, x+0, y);
    XDraw_బిందువు(gui.dpy, gui.wid, gui.పాఠ్యం_gc, x+2, y);
    XDraw_బిందువు(gui.dpy, gui.wid, gui.పాఠ్యం_gc, x+4, y++);
    XDraw_బిందువు(gui.dpy, gui.wid, gui.పాఠ్యం_gc, x+1, y);
    XDraw_బిందువు(gui.dpy, gui.wid, gui.పాఠ్యం_gc, x+2, y);
    XDraw_బిందువు(gui.dpy, gui.wid, gui.పాఠ్యం_gc, x+3, y++);
    XDraw_బిందువు(gui.dpy, gui.wid, gui.పాఠ్యం_gc, x+2, y);
}
#అయితే_ముగింపు // FEAT_GUI_X11

#ఒకవేళ defined(FEAT_GUI_GTK) && !defined(PROTO)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator instead. This is the GTK/Gnome version.
 */
    శూన్య
netbeans_draw_multisign_indicator(పూర్ణాంకం అడ్డువరుస)
{
    పూర్ణాంకం i;
    పూర్ణాంకం y;
    పూర్ణాంకం x;
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    cairo_t *cr = ఖాళీ;
# లేకపోతే
    GdkDrawable *drawable = gui.drawarea->కిటికీ_2;
# endif
# ifdef USE_GTK4
    cairo_surface_t *surf;
# endif

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    x = 0;
# ifdef USE_GTK4
    y = 2;
# లేకపోతే
    y = అడ్డువరుస * gui.char_ఎత్తు + 2;
# endif

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
#  ifdef USE_GTK4
    surf = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, 5, gui.char_ఎత్తు);
    cr = cairo_create(surf);
#  లేకపోతే
    cr = cairo_create(gui.surface);
#  endif
    cairo_సమితి_ఆధారం_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
# endif

    కొరకు (i = 0; i < gui.char_ఎత్తు - 3; i++)
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	cairo_rectangle(cr, x+2, y++, 1, 1);
# లేకపోతే
    gdk_draw_బిందువు(drawable, gui.పాఠ్యం_gc, x+2, y++);
# endif

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    cairo_rectangle(cr, x+0, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+4, y++, 1, 1);
    cairo_rectangle(cr, x+1, y, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
    cairo_rectangle(cr, x+3, y++, 1, 1);
    cairo_rectangle(cr, x+2, y, 1, 1);
# లేకపోతే
    gdk_draw_బిందువు(drawable, gui.పాఠ్యం_gc, x+0, y);
    gdk_draw_బిందువు(drawable, gui.పాఠ్యం_gc, x+2, y);
    gdk_draw_బిందువు(drawable, gui.పాఠ్యం_gc, x+4, y++);
    gdk_draw_బిందువు(drawable, gui.పాఠ్యం_gc, x+1, y);
    gdk_draw_బిందువు(drawable, gui.పాఠ్యం_gc, x+2, y);
    gdk_draw_బిందువు(drawable, gui.పాఠ్యం_gc, x+3, y++);
    gdk_draw_బిందువు(drawable, gui.పాఠ్యం_gc, x+2, y);
# endif

# ifdef USE_GTK4
    cairo_fill(cr);
    gui_gtk4_కూడు_multisign(surf, అడ్డువరుస, 0, 5, gui.char_ఎత్తు);
# endif

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    cairo_destroy(cr);
# endif
# ifdef USE_GTK4
    cairo_surface_destroy(surf);
# endif
}
#అయితే_ముగింపు // FEAT_GUI_GTK

/*
 * If the mouse is clicked in the gutter of a line with multiple
 * annotations, cycle through the set of signs.
 */
    శూన్య
netbeans_gutter_click(linenr_T lnum)
{
    sign_నమోదు_T	*p;

    ఒకవేళ (!NETBEANS_OPEN)
	తిరిగి;

    FOR_ALL_SIGNS_లోపల_BUF(curbuf, p)
    {
	ఒకవేళ (p->se_lnum == lnum && p->se_తదుపరి && p->se_తదుపరి->se_lnum == lnum)
	{
	    sign_నమోదు_T *తోక;

	    // remove "p" from list, reinsert it at the tail of the sublist
	    ఒకవేళ (p->se_prev)
		p->se_prev->se_తదుపరి = p->se_తదుపరి;
	    లేకపోతే
		curbuf->b_signlist = p->se_తదుపరి;
	    p->se_తదుపరి->se_prev = p->se_prev;
	    // now find end of sublist and insert p
	    కొరకు (తోక = p->se_తదుపరి;
		  తోక->se_తదుపరి && తోక->se_తదుపరి->se_lnum == lnum
				       && తోక->se_తదుపరి->se_id < GUARDEDOFFSET;
		  తోక = తోక->se_తదుపరి)
		;
	    // tail now points to last entry with same lnum (except
	    // that "guarded" annotations are always last)
	    p->se_తదుపరి = తోక->se_తదుపరి;
	    ఒకవేళ (తోక->se_తదుపరి)
		తోక->se_తదుపరి->se_prev = p;
	    p->se_prev = తోక;
	    తోక->se_తదుపరి = p;
	    update_debug_sign(curbuf, lnum);
	    ఆపు;
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
    స్థిర శూన్య
addsigntype(
    nbbuf_T	*buf,
    పూర్ణాంకం		type_అంకె,
    char_u	*typeName,
    char_u	*tooltip వాడని,
    char_u	*glyphFile,
    char_u	*fg,
    char_u	*bg)
{
    పూర్ణాంకం i, j;
    పూర్ణాంకం use_fg = (*fg && STRCMP(fg, "none") != 0);
    పూర్ణాంకం use_bg = (*bg && STRCMP(bg, "none") != 0);

    కొరకు (i = 0; i < globalsignmapused; i++)
	ఒకవేళ (STRCMP(typeName, globalsignmap[i]) == 0)
	    ఆపు;

    ఒకవేళ (i == globalsignmapused) // not found; add it to global map
    {
	nbdebug(("DEFINEANNOTYPE(%d,%s,%s,%s,%s,%s)\n",
			    type_అంకె, typeName, tooltip, glyphFile, fg, bg));
	ఒకవేళ (use_fg || use_bg)
	{
	    అక్షరం fgbuf[2 * (8 + MAX_COLOR_నిడివి) + 1];
	    అక్షరం bgbuf[2 * (8 + MAX_COLOR_నిడివి) + 1];
	    అక్షరం *ptr;
	    పూర్ణాంకం విలువ;

	    విలువ = strtol((అక్షరం *)fg, &ptr, 10);
	    ఒకవేళ (ptr != (అక్షరం *)fg)
		sprintf(fgbuf, "guifg=#%06x", విలువ & 0xFFFFFF);
	    లేకపోతే
		sprintf(fgbuf, "guifg=%s ctermfg=%s", fg, fg);

	    విలువ = strtol((అక్షరం *)bg, &ptr, 10);
	    ఒకవేళ (ptr != (అక్షరం *)bg)
		sprintf(bgbuf, "guibg=#%06x", విలువ & 0xFFFFFF);
	    లేకపోతే
		sprintf(bgbuf, "guibg=%s ctermbg=%s", bg, bg);

	    coloncmd(":highlight NB_%s %s %s", typeName, (use_fg) ? fgbuf : "",
		     (use_bg) ? bgbuf : "");
	    ఒకవేళ (*glyphFile == NUL)
		// no glyph, line highlighting only
		coloncmd(":sign define %d linehl=NB_%s", i + 1, typeName);
	    లేకపోతే ఒకవేళ (vim_strsize(glyphFile) <= 2)
		// one- or two-character glyph name, use as text glyph with
		// texthl
		coloncmd(":sign define %d text=%s texthl=NB_%s", i + 1,
							 glyphFile, typeName);
	    లేకపోతే
		// glyph, line highlighting
		coloncmd(":sign define %d icon=%s linehl=NB_%s", i + 1,
							 glyphFile, typeName);
	}
	లేకపోతే
	    // glyph, no line highlighting
	    coloncmd(":sign define %d icon=%s", i + 1, glyphFile);

	ఒకవేళ (STRCMP(typeName,"CurrentPC") == 0)
	    curPCtype = type_అంకె;

	ఒకవేళ (globalsignmapused == globalsignmaplen)
	{
	    ఒకవేళ (globalsignmaplen == 0) // first allocation
	    {
		globalsignmaplen = 20;
		globalsignmap = ALLOC_CLEAR_MULT(అక్షరం *, globalsignmaplen);
	    }
	    లేకపోతే    // grow it
	    {
		పూర్ణాంకం incr;
		పూర్ణాంకం oldlen = globalsignmaplen;
		అక్షరం **t_globalsignmap = globalsignmap;

		globalsignmaplen *= 2;
		incr = globalsignmaplen - oldlen;
		globalsignmap = vim_realloc(globalsignmap,
					   globalsignmaplen * పరిమాణం(అక్షరం *));
		ఒకవేళ (globalsignmap == ఖాళీ)
		{
		    vim_free(t_globalsignmap);
		    globalsignmaplen = 0;
		    తిరిగి;
		}
		vim_memset(globalsignmap + oldlen, 0, incr * పరిమాణం(అక్షరం *));
	    }
	}

	globalsignmap[i] = (అక్షరం *)vim_strsave(typeName);
	globalsignmapused = i + 1;
    }

    // check local map; should *not* be found!
    కొరకు (j = 0; j < buf->signmapused; j++)
	ఒకవేళ (buf->signmap[j] == i + 1)
	    తిరిగి;

    // add to local map
    ఒకవేళ (buf->signmapused == buf->signmaplen)
    {
	ఒకవేళ (buf->signmaplen == 0) // first allocation
	{
	    buf->signmaplen = 5;
	    buf->signmap = ALLOC_CLEAR_MULT(పూర్ణాంకం, buf->signmaplen);
	}
	లేకపోతే    // grow it
	{
	    పూర్ణాంకం incr;
	    పూర్ణాంకం oldlen = buf->signmaplen;
	    పూర్ణాంకం *t_signmap = buf->signmap;

	    buf->signmaplen *= 2;
	    incr = buf->signmaplen - oldlen;
	    buf->signmap = vim_realloc(buf->signmap,
					       buf->signmaplen * పరిమాణం(పూర్ణాంకం));
	    ఒకవేళ (buf->signmap == ఖాళీ)
	    {
		vim_free(t_signmap);
		buf->signmaplen = 0;
		తిరిగి;
	    }
	    vim_memset(buf->signmap + oldlen, 0, incr * పరిమాణం(పూర్ణాంకం));
	}
    }

    buf->signmap[buf->signmapused++] = i + 1;

}


/*
 * See if we have the requested sign type in the buffer.
 */
    స్థిర పూర్ణాంకం
mapsigntype(nbbuf_T *buf, పూర్ణాంకం localsigntype)
{
    ఒకవేళ (--localsigntype >= 0 && localsigntype < buf->signmapused)
	తిరిగి buf->signmap[localsigntype];

    తిరిగి 0;
}


/*
 * Compute length of buffer, don't print anything.
 */
    స్థిర దీర్ఘ
get_buf_కొలత(buf_T *bufp)
{
    linenr_T	lnum;
    దీర్ఘ	char_లెక్క = 0;
    పూర్ణాంకం		eol_కొలత;
    దీర్ఘ	చివరి_check = 100000L;

    ఒకవేళ (bufp->b_ml.ml_flags & ML_ఖాళీలు)
	తిరిగి 0;

    ఒకవేళ (get_fileformat(bufp) == EOL_DOS)
	eol_కొలత = 2;
    లేకపోతే
	eol_కొలత = 1;
    కొరకు (lnum = 1; lnum <= bufp->b_ml.ml_పంక్తి_లెక్క; ++lnum)
    {
	char_లెక్క += ml_get_buf_len(bufp, lnum) + eol_కొలత;
	// Check for a CTRL-C every 100000 characters
	ఒకవేళ (char_లెక్క > చివరి_check)
	{
	    ui_breakcheck();
	    ఒకవేళ (got_int)
		తిరిగి char_లెక్క;
	    చివరి_check = char_లెక్క + 100000L;
	}
    }
    // Correction for when last line doesn't have an EOL.
    ఒకవేళ (!bufp->b_p_eol && (bufp->b_p_bin || !bufp->b_p_fixeol))
	char_లెక్క -= eol_కొలత;

    తిరిగి char_లెక్క;
}

/*
 * Convert character offset to lnum,col
 */
    స్థిర pos_T *
off2pos(buf_T *buf, దీర్ఘ స్థానభ్రంశం)
{
    linenr_T	 lnum;
    స్థిర pos_T pos;

    pos.lnum = 0;
    pos.నిలువు = 0;
    pos.coladd = 0;

    ఒకవేళ (!(buf->b_ml.ml_flags & ML_ఖాళీలు))
    {
	ఒకవేళ ((lnum = ml_కనుగొను_పంక్తి_or_స్థానభ్రంశం(buf, (linenr_T)0, &స్థానభ్రంశం)) < 0)
	    తిరిగి ఖాళీ;
	pos.lnum = lnum;
	pos.నిలువు = స్థానభ్రంశం;
    }

    తిరిగి &pos;
}

/*
 * Convert an argument in the form "1234" to an offset and compute the
 * lnum/col from it.  Convert an argument in the form "123/12" directly to a
 * lnum/col.
 * "argp" is advanced to after the argument.
 * Return a pointer to the position, NULL if something is wrong.
 */
    స్థిర pos_T *
get_off_or_lnum(buf_T *buf, char_u **argp)
{
    స్థిర pos_T	mypos;
    దీర్ఘ		off;

    off = strtol((అక్షరం *)*argp, (అక్షరం **)argp, 10);
    ఒకవేళ (**argp == '/')
    {
	mypos.lnum = (linenr_T)off;
	++*argp;
	mypos.నిలువు = strtol((అక్షరం *)*argp, (అక్షరం **)argp, 10);
	mypos.coladd = 0;
	తిరిగి &mypos;
    }
    తిరిగి off2pos(buf, off);
}


/*
 * Convert (lnum,col) to byte offset in the file.
 */
    స్థిర దీర్ఘ
pos2off(buf_T *buf, pos_T *pos)
{
    దీర్ఘ	 స్థానభ్రంశం = 0;

    ఒకవేళ (buf->b_ml.ml_flags & ML_ఖాళీలు)
	తిరిగి 0;

    ఒకవేళ ((స్థానభ్రంశం = ml_కనుగొను_పంక్తి_or_స్థానభ్రంశం(buf, pos->lnum, 0)) < 0)
	తిరిగి 0;
    స్థానభ్రంశం += pos->నిలువు;

    తిరిగి స్థానభ్రంశం;
}


/*
 * This message is printed after NetBeans opens a new file. It's
 * similar to the message readfile() uses, but since NetBeans
 * doesn't normally call readfile, we do our own.
 */
    స్థిర శూన్య
print_read_సందేశం(nbbuf_T *buf)
{
    పూర్ణాంకం	    lnum = buf->bufp->b_ml.ml_పంక్తి_లెక్క;
    off_T   nchars = buf->bufp->b_orig_కొలత;
    char_u  c;

    సందేశం_కూడు_fname(buf->bufp, buf->bufp->b_ffname);
    c = FALSE;

    ఒకవేళ (buf->bufp->b_p_ro)
    {
	STRCAT(IObuff, shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
	c = TRUE;
    }
    ఒకవేళ (!buf->bufp->b_ప్రారంభం_eol)
    {
	STRCAT(IObuff, shortmess(SHM_చివరి) ? _("[noeol]")
					       : _("[Incomplete last line]"));
	c = TRUE;
    }
    సందేశం_కూడు_పంక్తులు(c, (దీర్ఘ)lnum, nchars);

    // Now display it
    VIM_CLEAR(keep_సందేశం);
    సందేశం_scrolled_ign = TRUE;
    సందేశం_trunc_attr((అక్షరం *)IObuff, FALSE, 0);
    సందేశం_scrolled_ign = FALSE;
}


/*
 * Print a message after NetBeans writes the file. This message should be
 * identical to the standard message a non-netbeans user would see when
 * writing a file.
 */
    స్థిర శూన్య
print_save_సందేశం(nbbuf_T *buf, off_T nchars)
{
    char_u	c;
    char_u	*p;

    ఒకవేళ (nchars >= 0)
    {
	// put fname in IObuff with quotes
	సందేశం_కూడు_fname(buf->bufp, buf->bufp->b_ffname);
	c = FALSE;

	సందేశం_కూడు_పంక్తులు(c, buf->bufp->b_ml.ml_పంక్తి_లెక్క,
						buf->bufp->b_orig_కొలత);

	VIM_CLEAR(keep_సందేశం);
	సందేశం_scrolled_ign = TRUE;
	p = (char_u *)సందేశం_trunc_attr((అక్షరం *)IObuff, FALSE, 0);
	ఒకవేళ ((సందేశం_scrolled && !need_wait_return) || !buf->init_పూర్తి)
	{
	    // Need to repeat the message after redrawing when:
	    // - When reading from stdin (the screen will be cleared next).
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    సమితి_keep_సందేశం(p, 0);
	}
	సందేశం_scrolled_ign = FALSE;
	// add_to_input_buf((char_u *)"\f", 1);
    }
    లేకపోతే
    {
	అక్షరం msgbuf[IOSIZE];

	vim_snprintf(msgbuf, IOSIZE,
		       _(e_is_read_only_కూడు_bang_గమ్యం_override), IObuff);
	nbdebug(("    %s\n", msgbuf));
	emsg(msgbuf);
    }
}

#అయితే_ముగింపు // defined(FEAT_NETBEANS_INTG)
