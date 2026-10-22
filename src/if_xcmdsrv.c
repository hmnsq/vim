/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 * X command server by Flemming Madsen
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 *
 * if_xcmdsrv.c: Functions for passing commands through an X11 display.
 *
 */

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "version.h"

#എങ്കിൽ (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

# ifdef FEAT_X11
#  include <X11/Intrinsic.h>
#  include <X11/Xatom.h>
# endif

/*
 * This file provides procedures that implement the command server
 * functionality of Vim when in contact with an X11 server.
 *
 * Adapted from TCL/TK's send command  in tkSend.c of the tk 3.6 distribution.
 * Adapted for use in Vim by Flemming Madsen. Protocol changed to that of tk 4
 */

/*
 * Copyright (c) 1989-1993 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */


/*
 * When a result is being awaited from a sent command, one of
 * the following structures is present on a list of all outstanding
 * sent commands.  The information in the structure is used to
 * process the result when it arrives.  You're probably wondering
 * how there could ever be multiple outstanding sent commands.
 * This could happen if Vim instances invoke each other recursively.
 * It's unlikely, but possible.
 */

തരനിർവചനം ഘടന PendingCommand
{
    പൂർണ്ണസംഖ്യ	    serial;	// Serial number expected in result.
    പൂർണ്ണസംഖ്യ	    code;	// Result Code. 0 is OK
    char_u  *result;	// String result for command (malloc'ed).
			// NULL means command still pending.
    ഘടന PendingCommand *nextPtr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

സ്ഥിതം PendingCommand *pendingCommands = ശൂന്യ;
				// List of all commands currently
				// being waited for.

/*
 * The information below is used for communication between processes
 * during "send" commands.  Each process keeps a private window, never
 * even mapped, with one property, "Comm".  When a command is sent to
 * an interpreter, the command is appended to the comm property of the
 * communication window associated with the interp's process.  Similarly,
 * when a result is returned from a sent command, it is also appended
 * to the comm property.
 *
 * Each command and each result takes the form of ASCII text.  For a
 * command, the text consists of a nul character followed by several
 * nul-terminated ASCII strings.  The first string consists of a
 * single letter:
 * "c" for an expression
 * "k" for keystrokes
 * "r" for reply
 * "n" for notification.
 * Subsequent strings have the form "option value" where the following options
 * are supported:
 *
 * -r commWindow serial
 *
 *	This option means that a response should be sent to the window
 *	whose X identifier is "commWindow" (in hex), and the response should
 *	be identified with the serial number given by "serial" (in decimal).
 *	If this option isn't specified then the send is asynchronous and
 *	no response is sent.
 *
 * -n name
 *	"Name" gives the name of the application for which the command is
 *	intended.  This option must be present.
 *
 * -E encoding
 *	Encoding name used for the text.  This is the 'encoding' of the
 *	sender.  The receiver may want to do conversion to his 'encoding'.
 *
 * -s script
 *	"Script" is the script to be executed.  This option must be
 *	present.  Taken as a series of keystrokes in a "k" command where
 *	<Key>'s are expanded
 *
 * The options may appear in any order.  The -n and -s options must be
 * present, but -r may be omitted for asynchronous RPCs.  For compatibility
 * with future releases that may add new features, there may be additional
 * options present;  as long as they start with a "-" character, they will
 * be ignored.
 *
 * A result also consists of a zero character followed by several null-
 * terminated ASCII strings.  The first string consists of the single
 * letter "r".  Subsequent strings have the form "option value" where
 * the following options are supported:
 *
 * -s serial
 *	Identifies the command for which this is the result.  It is the
 *	same as the "serial" field from the -s option in the command.  This
 *	option must be present.
 *
 * -r result
 *	"Result" is the result string for the script, which may be either
 *	a result or an error message.  If this field is omitted then it
 *	defaults to an empty string.
 *
 * -c code
 *	0: for OK. This is the default.
 *	1: for error: Result is the last error
 *
 * -i errorInfo
 * -e errorCode
 *	Not applicable for Vim
 *
 * Options may appear in any order, and only the -s option must be
 * present.  As with commands, there may be additional options besides
 * these;  unknown options are ignored.
 */

/*
 * Maximum size property that can be read at one time by
 * this module:
 */

# define MAX_PROP_വാക്കുകൾ 100000

ഘടന ServerReply
{
    Window  id;
    garray_T strings;
};
സ്ഥിതം garray_T serverReply = { 0, 0, 0, 0, 0 };
എണ്ണൽ ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

തരനിർവചനം പൂർണ്ണസംഖ്യ (*EndCond)(ശൂന്യം *);

ഘടന x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    ഘടന x_cmdqueue	*next;
    ഘടന x_cmdqueue	*prev;
};

തരനിർവചനം ഘടന x_cmdqueue x_queue_T;

// dummy node, header for circular queue
സ്ഥിതം x_queue_T head = {ശൂന്യ, 0, ശൂന്യ, ശൂന്യ};

/*
 * Forward declarations for procedures defined later in this file:
 */

സ്ഥിതം Window	LookupName(Display *dpy, char_u *name, പൂർണ്ണസംഖ്യ delete, char_u **loose);
സ്ഥിതം പൂർണ്ണസംഖ്യ	SendInit(Display *dpy);
സ്ഥിതം പൂർണ്ണസംഖ്യ	DoRegisterName(Display *dpy, char_u *name);
സ്ഥിതം ശൂന്യം	DeleteAnyLingerer(Display *dpy, Window w);
സ്ഥിതം പൂർണ്ണസംഖ്യ	GetRegProp(Display *dpy, char_u **regPropp, long_u *അക്കം_Itemsp, പൂർണ്ണസംഖ്യ domsg);
സ്ഥിതം പൂർണ്ണസംഖ്യ	WaitForPend(ശൂന്യം *p);
സ്ഥിതം പൂർണ്ണസംഖ്യ	WindowValid(Display *dpy, Window w);
സ്ഥിതം ശൂന്യം	ServerWait(Display *dpy, Window w, EndCond endCond, ശൂന്യം *endData, പൂർണ്ണസംഖ്യ localLoop, പൂർണ്ണസംഖ്യ seconds);
സ്ഥിതം പൂർണ്ണസംഖ്യ	AppendPropCarefully(Display *display, Window window, Atom property, char_u *മൂല്യം, പൂർണ്ണസംഖ്യ length);
സ്ഥിതം പൂർണ്ണസംഖ്യ	x_error_check(Display *dpy, XErrorEvent *error_event);
സ്ഥിതം പൂർണ്ണസംഖ്യ	IsSerialName(char_u *name);
സ്ഥിതം ശൂന്യം	save_in_queue(char_u *buf, long_u len);
സ്ഥിതം ശൂന്യം	server_parse_message(Display *dpy, char_u *propInfo, long_u അക്കം_ഇനങ്ങൾ);

// Private variables for the "server" functionality
സ്ഥിതം Atom	registryProperty = None;
സ്ഥിതം Atom	vimProperty = None;
സ്ഥിതം പൂർണ്ണസംഖ്യ	got_x_error = FALSE;

സ്ഥിതം char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    പൂർണ്ണസംഖ്യ
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		res;
    char_u	*p = ശൂന്യ;

    res = DoRegisterName(dpy, name);
    എങ്കിൽ (res >= 0)
	തിരികെ OK;

    i = 1;
    ചെയ്യുക
    {
	എങ്കിൽ (res < -1 || i >= 1000)
	{
	    സന്ദേശം_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    തിരികെ FAIL;
	}
	എങ്കിൽ (p == ശൂന്യ)
	    p = alloc(STRLEN(name) + 10);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    res = -10;
	    തുടരുക;
	}
	sprintf((അക്ഷരം *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    ആകുമ്പോൾ (res < 0)
	;
    vim_free(p);

    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    എങ്കിൽ (commProperty == None)
    {
	എങ്കിൽ (SendInit(dpy) < 0)
	    തിരികെ -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, ശൂന്യ);
    എങ്കിൽ (w != (Window)0)
    {
	Status		status;
	പൂർണ്ണസംഖ്യ		dummyInt;
	unsigned പൂർണ്ണസംഖ്യ	dummyUns;
	Window		dummyWin;

	/*
	 * The name is currently registered.  See if the commWindow
	 * associated with the name exists.  If not, or if the commWindow
	 * is *our* commWindow, then just unregister the old name (this
	 * could happen if an application dies without cleaning up the
	 * registry).
	 */
	old_handler = XSetErrorHandler(x_error_check);
	status = XGetGeometry(dpy, w, &dummyWin, &dummyInt, &dummyInt,
				  &dummyUns, &dummyUns, &dummyUns, &dummyUns);
	(ശൂന്യം)XSetErrorHandler(old_handler);
	എങ്കിൽ (status != Success && w != commWindow)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    തിരികെ -1;
	}
	(ശൂന്യം)LookupName(dpy, name, /*delete=*/TRUE, ശൂന്യ);
    }
    sprintf((അക്ഷരം *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, വേര്_Window(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (ശൂന്യം)XSetErrorHandler(old_handler);

    എങ്കിൽ (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, (പൂർണ്ണസംഖ്യ)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	തിരികെ 0;
    }
    തിരികെ -2;
}

# എങ്കിൽ defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    ശൂന്യം
serverChangeRegisteredWindow(
    Display	*dpy,		// Display to register with
    Window	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    commWindow = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    എങ്കിൽ (SendInit(dpy) < 0)
	തിരികെ;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    എങ്കിൽ (serverName != ശൂന്യ)
    {
	// Reinsert name if it was already registered
	(ശൂന്യം)LookupName(dpy, serverName, /*delete=*/TRUE, ശൂന്യ);
	sprintf((അക്ഷരം *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_LENGTH, serverName);
	XChangeProperty(dpy, വേര്_Window(dpy, 0), registryProperty, XA_STRING, 8,
			PropModeAppend, (char_u *)propInfo,
			STRLEN(propInfo) + 1);
    }
    XUngrabServer(dpy);
}
# endif

/*
 * Send to an instance of Vim via the X display.
 * Returns 0 for OK, negative for an error.
 */
    പൂർണ്ണസംഖ്യ
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**result,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	as_വ്യഞ്ജകം,			// Interpret as keystrokes or expr ?
    പൂർണ്ണസംഖ്യ		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    പൂർണ്ണസംഖ്യ		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    പൂർണ്ണസംഖ്യ		    length;
    പൂർണ്ണസംഖ്യ		    res;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = ശൂന്യ;

    എങ്കിൽ (result != ശൂന്യ)
	*result = ശൂന്യ;
    എങ്കിൽ (name == ശൂന്യ || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    എങ്കിൽ (commProperty == None && dpy != ശൂന്യ && SendInit(dpy) < 0)
	തിരികെ -1;

# എങ്കിൽ defined(FEAT_EVAL)
    ch_log(ശൂന്യ, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    എങ്കിൽ (dpy == ശൂന്യ || (serverName != ശൂന്യ && STRICMP(name, serverName) == 0))
	തിരികെ sendToLocalVim(cmd, as_വ്യഞ്ജകം, result);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    ആകുമ്പോൾ (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	എങ്കിൽ (w != None)
	{
	    എങ്കിൽ (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, ശൂന്യ);
		VIM_CLEAR(loosename);
		തുടരുക;
	    }
	}
	നിർത്തുക;
    }
    എങ്കിൽ (w == None)
    {
	എങ്കിൽ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	തിരികെ -1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (loosename != ശൂന്യ)
	name = loosename;
    എങ്കിൽ (server != ശൂന്യ)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    length = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(length + 30);

    sprintf((അക്ഷരം *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_വ്യഞ്ജകം ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    എങ്കിൽ (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((അക്ഷരം *)property + length, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    length += STRLEN(property + length + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    vim_free(property);
    എങ്കിൽ (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	തിരികെ -1;
    }

    എങ്കിൽ (!as_വ്യഞ്ജകം) // There is no answer for this - Keys are sent async
	തിരികെ 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.code = 0;
    pending.result = ശൂന്യ;
    pending.nextPtr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    എങ്കിൽ (pendingCommands == &pending)
	pendingCommands = pending.nextPtr;
    അല്ലെങ്കിൽ
    {
	PendingCommand *pcPtr;

	വേണ്ടി (pcPtr = pendingCommands; pcPtr != ശൂന്യ; pcPtr = pcPtr->nextPtr)
	    എങ്കിൽ (pcPtr->nextPtr == &pending)
	    {
		pcPtr->nextPtr = pending.nextPtr;
		നിർത്തുക;
	    }
    }

# എങ്കിൽ defined(FEAT_EVAL)
    ch_log(ശൂന്യ, "serverSendToVim() result: %s",
	    pending.result == ശൂന്യ ? "NULL" : (അക്ഷരം *)pending.result);
# endif
    എങ്കിൽ (result != ശൂന്യ)
	*result = pending.result;
    അല്ലെങ്കിൽ
	vim_free(pending.result);

    തിരികെ pending.code == 0 ? 0 : -1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
WaitForPend(ശൂന്യം *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    തിരികെ pending->result != ശൂന്യ;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    പൂർണ്ണസംഖ്യ		    അക്കം_Prop;
    പൂർണ്ണസംഖ്യ		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &അക്കം_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    എങ്കിൽ (plist == ശൂന്യ || got_x_error)
	തിരികെ FALSE;

    വേണ്ടി (i = 0; i < അക്കം_Prop; i++)
	എങ്കിൽ (plist[i] == vimProperty)
	{
	    XFree(plist);
	    തിരികെ TRUE;
	}
    XFree(plist);
    തിരികെ FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    സ്ഥിതം ശൂന്യം
ServerWait(
    Display	*dpy,
    Window	w,
    EndCond	endCond,
    ശൂന്യം	*endData,
    പൂർണ്ണസംഖ്യ		localLoop,
    പൂർണ്ണസംഖ്യ		seconds)
{
    time_t	    start;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_സംഖ്യ(dpy), &fds);
# അല്ലെങ്കിൽ
    ഘടന pollfd   fds;

    fds.fd = Connection_സംഖ്യ(dpy);
    fds.events = POLLIN;
# endif

    time(&start);
    ആകുമ്പോൾ (TRUE)
    {
	ആകുമ്പോൾ (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	എങ്കിൽ (endCond(endData) != 0)
	    നിർത്തുക;
	എങ്കിൽ (!WindowValid(dpy, w))
	    നിർത്തുക;
	time(&now);
	എങ്കിൽ (seconds >= 0 && (now - start) >= seconds)
	    നിർത്തുക;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	എങ്കിൽ (localLoop)
	{
# ifdef HAVE_SELECT
	    ഘടന timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    എങ്കിൽ (select(FD_SETSIZE, &fds, ശൂന്യ, ശൂന്യ, &tv) < 0)
		നിർത്തുക;
# അല്ലെങ്കിൽ
	    എങ്കിൽ (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		നിർത്തുക;
# endif
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (got_int)
		നിർത്തുക;
	    ui_delay((long)UI_MSEC_DELAY, TRUE);
	    ui_breakcheck();
	}
    }
}


/*
 * Fetch a list of all the Vim instance names currently registered for the
 * display.
 *
 * Returns a list of strings or NULL on failure.
 */
    list_T *
serverGetVimNames(Display *dpy)
{
    char_u	*regProp;
    char_u	*entry;
    char_u	*p;
    long_u	അക്കം_ഇനങ്ങൾ;
    int_u	w;
    list_T	*list;

    എങ്കിൽ (registryProperty == None)
    {
	എങ്കിൽ (SendInit(dpy) < 0)
	    തിരികെ ശൂന്യ;
    }

    list = list_alloc();
    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ ശൂന്യ;

    /*
     * Read the registry property.
     */
    എങ്കിൽ (GetRegProp(dpy, &regProp, &അക്കം_ഇനങ്ങൾ, TRUE) == FAIL)
	തിരികെ ശൂന്യ;

    /*
     * Scan all of the names out of the property.
     */
    വേണ്ടി (p = regProp; (long_u)(p - regProp) < അക്കം_ഇനങ്ങൾ; p++)
    {
	entry = p;
	ആകുമ്പോൾ (*p != 0 && !SAFE_isspace(*p))
	    p++;
	എങ്കിൽ (*p != 0)
	{
	    w = None;
	    sscanf((അക്ഷരം *)entry, "%x", &w);
	    എങ്കിൽ (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    ആകുമ്പോൾ (*p != 0)
		p++;
	}
    }
    എങ്കിൽ (regProp != empty_prop)
	XFree(regProp);
    തിരികെ list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    സ്ഥിതം ഘടന ServerReply *
ServerReplyFind(Window w, എണ്ണൽ ServerReplyOp op)
{
    ഘടന ServerReply *p;
    ഘടന ServerReply e;
    പൂർണ്ണസംഖ്യ		i;

    p = (ഘടന ServerReply *) serverReply.ga_data;
    വേണ്ടി (i = 0; i < serverReply.ga_len; i++, p++)
	എങ്കിൽ (p->id == w)
	    നിർത്തുക;
    എങ്കിൽ (i >= serverReply.ga_len)
	p = ശൂന്യ;

    എങ്കിൽ (p == ശൂന്യ && op == SROP_Add)
    {
	എങ്കിൽ (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, വലുപ്പം(ഘടന ServerReply), 1);
	എങ്കിൽ (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((ഘടന ServerReply *) serverReply.ga_data)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, വലുപ്പം(e));
	    serverReply.ga_len++;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p != ശൂന്യ && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * വലുപ്പം(*p));
	serverReply.ga_len--;
    }

    തിരികെ p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    unsigned  id = None;

    sscanf((അക്ഷരം *)str, "0x%x", &id);
    എങ്കിൽ (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    തിരികെ (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    പൂർണ്ണസംഖ്യ
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    പൂർണ്ണസംഖ്യ		length;
    പൂർണ്ണസംഖ്യ		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    എങ്കിൽ (commProperty == None)
    {
	എങ്കിൽ (SendInit(dpy) < 0)
	    തിരികെ -2;
    }
    എങ്കിൽ (!WindowValid(dpy, win))
	തിരികെ -1;

    length = STRLEN(p_enc) + STRLEN(str) + 14;
    എങ്കിൽ ((property = alloc(length + 30)) == ശൂന്യ)
	തിരികെ -1;

    sprintf((അക്ഷരം *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (unsigned പൂർണ്ണസംഖ്യ)commWindow);
    // Add length of what "%x" resulted in.
    length += STRLEN(property + length);
    res = AppendPropCarefully(dpy, win, commProperty, property, length + 1);
    vim_free(property);

    തിരികെ res;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
WaitForReply(ശൂന്യം *p)
{
    Window  *w = (Window *) p;

    തിരികെ ServerReplyFind(*w, SROP_Find) != ശൂന്യ;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    പൂർണ്ണസംഖ്യ
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    പൂർണ്ണസംഖ്യ		localLoop,
    പൂർണ്ണസംഖ്യ		timeout)
{
    പൂർണ്ണസംഖ്യ		len;
    char_u	*s;
    ഘടന	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    എങ്കിൽ ((p = ServerReplyFind(win, SROP_Find)) != ശൂന്യ && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_data);
	len = STRLEN(*str) + 1;
	എങ്കിൽ (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_data;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	അല്ലെങ്കിൽ
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	തിരികെ 0;
    }
    തിരികെ -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    പൂർണ്ണസംഖ്യ
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    ഘടന ServerReply *p;

    എങ്കിൽ ((p = ServerReplyFind(win, SROP_Find)) != ശൂന്യ && p->strings.ga_len > 0)
    {
	എങ്കിൽ (str != ശൂന്യ)
	    *str = p->strings.ga_data;
	തിരികെ 1;
    }
    എങ്കിൽ (!WindowValid(dpy, win))
	തിരികെ -1;
    തിരികെ 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    എങ്കിൽ (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    എങ്കിൽ (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    എങ്കിൽ (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    എങ്കിൽ (commWindow == None)
    {
	commWindow = XCreateSimpleWindow(dpy, XDefault_വേര്_Window(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_നിവേശം(dpy, commWindow, PropertyChangeMask);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, commWindow);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, commWindow, vimProperty, XA_STRING,
		    8, PropModeReplace, (char_u *)VIM_VERSION_SHORT,
			(പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (ശൂന്യം)XSetErrorHandler(old_handler);

    തിരികെ got_x_error ? -1 : 0;
}

/*
 * Given a server name, see if the name exists in the registry for a
 * particular display.
 *
 * If the given name is registered, return the ID of the window associated
 * with the name.  If the name isn't registered, then return 0.
 *
 * Side effects:
 *	If the registry property is improperly formed, then it is deleted.
 *	If "delete" is non-zero, then if the named server is found it is
 *	removed from the registry property.
 */
    സ്ഥിതം Window
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    പൂർണ്ണസംഖ്യ		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *entry;
    char_u	*p;
    long_u	അക്കം_ഇനങ്ങൾ;
    int_u	return_മൂല്യം;

    /*
     * Read the registry property.
     */
    എങ്കിൽ (GetRegProp(dpy, &regProp, &അക്കം_ഇനങ്ങൾ, FALSE) == FAIL)
	തിരികെ 0;

    /*
     * Scan the property for the desired name.
     */
    return_മൂല്യം = (int_u)None;
    entry = ശൂന്യ;	// Not needed, but eliminates compiler warning.
    വേണ്ടി (p = regProp; (long_u)(p - regProp) < അക്കം_ഇനങ്ങൾ; )
    {
	entry = p;
	ആകുമ്പോൾ (*p != 0 && !SAFE_isspace(*p))
	    p++;
	എങ്കിൽ (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    sscanf((അക്ഷരം *)entry, "%x", &return_മൂല്യം);
	    നിർത്തുക;
	}
	ആകുമ്പോൾ (*p != 0)
	    p++;
	p++;
    }

    എങ്കിൽ (loose != ശൂന്യ && return_മൂല്യം == (int_u)None && !IsSerialName(name))
    {
	വേണ്ടി (p = regProp; (long_u)(p - regProp) < അക്കം_ഇനങ്ങൾ; )
	{
	    entry = p;
	    ആകുമ്പോൾ (*p != 0 && !SAFE_isspace(*p))
		p++;
	    എങ്കിൽ (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		എങ്കിൽ (sscanf((അക്ഷരം *)entry, "%x", &return_മൂല്യം) == 1
			&& return_മൂല്യം != (int_u)None)
		    *loose = vim_strsave(p + 1);
		നിർത്തുക;
	    }
	    ആകുമ്പോൾ (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    എങ്കിൽ (delete && return_മൂല്യം != (int_u)None)
    {
	പൂർണ്ണസംഖ്യ എണ്ണം;

	ആകുമ്പോൾ (*p != 0)
	    p++;
	p++;
	എണ്ണം = അക്കം_ഇനങ്ങൾ - (p - regProp);
	എങ്കിൽ (എണ്ണം > 0)
	    mch_memmove(entry, p, എണ്ണം);
	XChangeProperty(dpy, വേര്_Window(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(പൂർണ്ണസംഖ്യ)(അക്കം_ഇനങ്ങൾ - (p - entry)));
	XSync(dpy, False);
    }

    എങ്കിൽ (regProp != empty_prop)
	XFree(regProp);
    തിരികെ (Window)return_മൂല്യം;
}

/*
 * Delete any lingering occurrence of window id.  We promise that any
 * occurrence is not ours since it is not yet put into the registry (by us)
 *
 * This is necessary in the following scenario:
 * 1. There is an old windowid for an exit'ed vim in the registry
 * 2. We get that id for our commWindow but only want to send, not register.
 * 3. The window will mistakenly be regarded valid because of own commWindow
 */
    സ്ഥിതം ശൂന്യം
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    Window	win)	// Window to remove
{
    char_u	*regProp, *entry = ശൂന്യ;
    char_u	*p;
    long_u	അക്കം_ഇനങ്ങൾ;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    എങ്കിൽ (GetRegProp(dpy, &regProp, &അക്കം_ഇനങ്ങൾ, FALSE) == FAIL)
	തിരികെ;

    // Scan the property for the window id.
    വേണ്ടി (p = regProp; (long_u)(p - regProp) < അക്കം_ഇനങ്ങൾ; )
    {
	എങ്കിൽ (*p != 0)
	{
	    sscanf((അക്ഷരം *)p, "%x", &wwin);
	    എങ്കിൽ ((Window)wwin == win)
	    {
		പൂർണ്ണസംഖ്യ lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		ആകുമ്പോൾ (*p != 0)
		    p++;
		p++;
		lastHalf = അക്കം_ഇനങ്ങൾ - (p - regProp);
		എങ്കിൽ (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		അക്കം_ഇനങ്ങൾ = (entry - regProp) + lastHalf;
		p = entry;
		തുടരുക;
	    }
	}
	ആകുമ്പോൾ (*p != 0)
	    p++;
	p++;
    }

    എങ്കിൽ (entry != ശൂന്യ)
    {
	XChangeProperty(dpy, വേര്_Window(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(പൂർണ്ണസംഖ്യ)(p - regProp));
	XSync(dpy, False);
    }

    എങ്കിൽ (regProp != empty_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*അക്കം_Itemsp,
    പൂർണ്ണസംഖ്യ		domsg)		// When TRUE give error message.
{
    പൂർണ്ണസംഖ്യ		result, actualFormat;
    long_u	bytesAfter;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = ശൂന്യ;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    result = XGetWindowProperty(dpy, വേര്_Window(dpy, 0), registryProperty, 0L,
				(long)MAX_PROP_വാക്കുകൾ, False,
				XA_STRING, &actualType,
				&actualFormat, അക്കം_Itemsp, &bytesAfter,
				regPropp);

    XSync(dpy, FALSE);
    (ശൂന്യം)XSetErrorHandler(old_handler);
    എങ്കിൽ (got_x_error)
	തിരികെ FAIL;

    എങ്കിൽ (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*അക്കം_Itemsp = 0;
	*regPropp = empty_prop;
	തിരികെ OK;
    }

    // If the property is improperly formed, then delete it.
    എങ്കിൽ (result != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	എങ്കിൽ (*regPropp != ശൂന്യ)
	    XFree(*regPropp);
	XDeleteProperty(dpy, വേര്_Window(dpy, 0), registryProperty);
	എങ്കിൽ (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	തിരികെ FAIL;
    }
    തിരികെ OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    ശൂന്യം
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    പൂർണ്ണസംഖ്യ		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    പൂർണ്ണസംഖ്യ		result, actualFormat;
    long_u	അക്കം_ഇനങ്ങൾ, bytesAfter;
    Atom	actualType;

    എങ്കിൽ (eventPtr != ശൂന്യ)
    {
	എങ്കിൽ (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_മൂല്യം)
	    തിരികെ;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = ശൂന്യ;
    result = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(long)MAX_PROP_വാക്കുകൾ, True,
				XA_STRING, &actualType,
				&actualFormat, &അക്കം_ഇനങ്ങൾ, &bytesAfter,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    എങ്കിൽ (result != Success || actualType != XA_STRING || actualFormat != 8)
    {
	എങ്കിൽ (propInfo != ശൂന്യ)
	    XFree(propInfo);
	തിരികെ;
    }
    എങ്കിൽ (immediate)
	server_parse_message(dpy, propInfo, അക്കം_ഇനങ്ങൾ);
    അല്ലെങ്കിൽ
	save_in_queue(propInfo, അക്കം_ഇനങ്ങൾ);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    സ്ഥിതം ശൂന്യം
save_in_queue(char_u *propInfo, long_u len)
{
    x_queue_T *കണ്ണി;

    കണ്ണി = ALLOC_ONE(x_queue_T);
    എങ്കിൽ (കണ്ണി == ശൂന്യ)
	തിരികെ;	    // out of memory
    കണ്ണി->propInfo = propInfo;
    കണ്ണി->len = len;

    എങ്കിൽ (head.next == ശൂന്യ)   // initialize circular queue
    {
	head.next = &head;
	head.prev = &head;
    }

    // insert node at tail of queue
    കണ്ണി->next = &head;
    കണ്ണി->prev = head.prev;
    head.prev->next = കണ്ണി;
    head.prev = കണ്ണി;
}

/*
 * Parses queued clientserver messages.
 */
    ശൂന്യം
server_parse_messages(ശൂന്യം)
{
    x_queue_T	*കണ്ണി;

    എങ്കിൽ (!X_DISPLAY)
	തിരികെ; // cannot happen?
    ആകുമ്പോൾ (head.next != ശൂന്യ && head.next != &head)
    {
	കണ്ണി = head.next;
	head.next = കണ്ണി->next;
	കണ്ണി->next->prev = കണ്ണി->prev;
	server_parse_message(X_DISPLAY, കണ്ണി->propInfo, കണ്ണി->len);
	vim_free(കണ്ണി);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    പൂർണ്ണസംഖ്യ
server_waiting(ശൂന്യം)
{
    തിരികെ head.next != ശൂന്യ && head.next != &head;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    സ്ഥിതം ശൂന്യം
server_parse_message(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	അക്കം_ഇനങ്ങൾ)  // The size of propInfo in bytes.
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		code;
    char_u	*tofree;

# എങ്കിൽ defined(FEAT_EVAL)
    ch_log(ശൂന്യ, "server_parse_message() numItems: %ld", അക്കം_ഇനങ്ങൾ);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    വേണ്ടി (p = propInfo; (long_u)(p - propInfo) < അക്കം_ഇനങ്ങൾ; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	എങ്കിൽ (*p == 0)
	{
	    p++;
	    തുടരുക;
	}

	എങ്കിൽ ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    Window	resWindow;
	    char_u	*name, *script, *serial, *end;
	    Bool	as_താക്കോലുകൾ = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = ശൂന്യ;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = ശൂന്യ;
	    enc = ശൂന്യ;
	    ആകുമ്പോൾ ((long_u)(p - propInfo) < അക്കം_ഇനങ്ങൾ && *p == '-')
	    {
# എങ്കിൽ defined(FEAT_EVAL)
		ch_log(ശൂന്യ, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		തിരഞ്ഞെടുപ്പ് (p[1])
		{
		    അവസ്ഥ 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			ആകുമ്പോൾ (vim_isxdigit(*end))
			{
			    resWindow = 16 * resWindow + (long_u)hex2nr(*end);
			    ++end;
			}
			എങ്കിൽ (end == p + 2 || *end != ' ')
			    resWindow = None;
			അല്ലെങ്കിൽ
			{
			    p = serial = end + 1;
			    clientWindow = resWindow; // Remember in global
			}
			നിർത്തുക;
		    അവസ്ഥ 'n':
			എങ്കിൽ (p[2] == ' ')
			    name = p + 3;
			നിർത്തുക;
		    അവസ്ഥ 's':
			എങ്കിൽ (p[2] == ' ')
			    script = p + 3;
			നിർത്തുക;
		    അവസ്ഥ 'E':
			എങ്കിൽ (p[2] == ' ')
			    enc = p + 3;
			നിർത്തുക;
		}
		ആകുമ്പോൾ (*p != 0)
		    p++;
		p++;
	    }

	    എങ്കിൽ (script == ശൂന്യ || name == ശൂന്യ)
		തുടരുക;

	    എങ്കിൽ (serverName != ശൂന്യ && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		എങ്കിൽ (as_താക്കോലുകൾ)
		    server_to_നിവേശം_buf(script);
		അല്ലെങ്കിൽ
		{
		    char_u      *res;

		    res = eval_client_വ്യഞ്ജകം_to_string(script);
		    എങ്കിൽ (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(ശൂന്യം)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(reply.ga_data, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			എങ്കിൽ (res != ശൂന്യ)
			    ga_concat(&reply, res);
			അല്ലെങ്കിൽ
			{
			    ga_concat(&reply,
				   (char_u *)_(e_invalid_expression_received));
			    ga_append(&reply, 0);
			    GA_CONCAT_LITERAL(&reply, "-c 1");
			}
			ga_append(&reply, NUL);
			(ശൂന്യം)AppendPropCarefully(dpy, resWindow, commProperty,
						 reply.ga_data, reply.ga_len);
			ga_clear(&reply);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'r' && p[1] == 0)
	{
	    പൂർണ്ണസംഖ്യ		    serial, gotSerial;
	    char_u	    *res;
	    PendingCommand  *pcPtr;
	    char_u	    *enc;

	    /*
	     * This is a reply to some command that we sent out.  Iterate
	     * over all of its options.  Stop when we reach the end of the
	     * property or something that doesn't look like an option.
	     */
	    p += 2;
	    gotSerial = 0;
	    res = (char_u *)"";
	    code = 0;
	    enc = ശൂന്യ;
	    ആകുമ്പോൾ ((long_u)(p - propInfo) < അക്കം_ഇനങ്ങൾ && *p == '-')
	    {
		തിരഞ്ഞെടുപ്പ് (p[1])
		{
		    അവസ്ഥ 'r':
			എങ്കിൽ (p[2] == ' ')
			    res = p + 3;
			നിർത്തുക;
		    അവസ്ഥ 'E':
			എങ്കിൽ (p[2] == ' ')
			    enc = p + 3;
			നിർത്തുക;
		    അവസ്ഥ 's':
			എങ്കിൽ (sscanf((അക്ഷരം *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			നിർത്തുക;
		    അവസ്ഥ 'c':
			എങ്കിൽ (sscanf((അക്ഷരം *)p + 2, " %d", &code) != 1)
			    code = 0;
			നിർത്തുക;
		}
		ആകുമ്പോൾ (*p != 0)
		    p++;
		p++;
	    }

	    എങ്കിൽ (!gotSerial)
		തുടരുക;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    വേണ്ടി (pcPtr = pendingCommands; pcPtr != ശൂന്യ; pcPtr = pcPtr->nextPtr)
	    {
		എങ്കിൽ (serial != pcPtr->serial || pcPtr->result != ശൂന്യ)
		    തുടരുക;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		എങ്കിൽ (tofree == ശൂന്യ)
		    res = vim_strsave(res);
		pcPtr->result = res;
		നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    unsigned പൂർണ്ണസംഖ്യ u;
	    പൂർണ്ണസംഖ്യ		gotWindow;
	    char_u	*str;
	    ഘടന	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = ശൂന്യ;
	    ആകുമ്പോൾ ((long_u)(p - propInfo) < അക്കം_ഇനങ്ങൾ && *p == '-')
	    {
		തിരഞ്ഞെടുപ്പ് (p[1])
		{
		    അവസ്ഥ 'n':
			എങ്കിൽ (p[2] == ' ')
			    str = p + 3;
			നിർത്തുക;
		    അവസ്ഥ 'E':
			എങ്കിൽ (p[2] == ' ')
			    enc = p + 3;
			നിർത്തുക;
		    അവസ്ഥ 'w':
			എങ്കിൽ (sscanf((അക്ഷരം *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			നിർത്തുക;
		}
		ആകുമ്പോൾ (*p != 0)
		    p++;
		p++;
	    }

	    എങ്കിൽ (!gotWindow)
		തുടരുക;
	    str = serverConvert(enc, str, &tofree);
	    എങ്കിൽ ((r = ServerReplyFind(win, SROP_Add)) != ശൂന്യ)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((അക്ഷരം *)winstr, "0x%x", (unsigned പൂർണ്ണസംഖ്യ)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    ആകുമ്പോൾ (*p != 0)
		p++;
	    p++;
	}
    }
    XFree(propInfo);
}

/*
 * Append a given property to a given window, but set up an X error handler so
 * that if the append fails this procedure can return an error code rather
 * than having Xlib panic.
 * Return: 0 for OK, -1 for error
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*മൂല്യം,		// Characters  to append to property.
    പൂർണ്ണസംഖ്യ		length)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, മൂല്യം, length);
    XSync(dpy, False);
    (ശൂന്യം) XSetErrorHandler(old_handler);
    തിരികെ got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    തിരികെ 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
IsSerialName(char_u *str)
{
    പൂർണ്ണസംഖ്യ len = STRLEN(str);

    തിരികെ (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
