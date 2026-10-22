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

#ضمن "vim.h"
#ضمن "version.h"

#ܐܢ (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

تعريفنوع بنية PendingCommand
{
    صحيح	    serial;	// Serial number expected in result.
    صحيح	    code;	// Result Code. 0 is OK
    char_u  *result;	// String result for command (malloc'ed).
			// NULL means command still pending.
    بنية PendingCommand *nextPtr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

ساكن PendingCommand *pendingCommands = عدم;
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

# define MAX_PROP_WORDS 100000

بنية ServerReply
{
    Window  id;
    garray_T strings;
};
ساكن garray_T serverReply = { 0, 0, 0, 0, 0 };
تعداد ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

تعريفنوع صحيح (*EndCond)(فراغ *);

بنية x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    بنية x_cmdqueue	*next;
    بنية x_cmdqueue	*prev;
};

تعريفنوع بنية x_cmdqueue x_queue_T;

// dummy node, header for circular queue
ساكن x_queue_T head = {عدم, 0, عدم, عدم};

/*
 * Forward declarations for procedures defined later in this file:
 */

ساكن Window	LookupName(Display *dpy, char_u *name, صحيح delete, char_u **loose);
ساكن صحيح	SendInit(Display *dpy);
ساكن صحيح	DoRegisterName(Display *dpy, char_u *name);
ساكن فراغ	DeleteAnyLingerer(Display *dpy, Window w);
ساكن صحيح	GetRegProp(Display *dpy, char_u **regPropp, long_u *numItemsp, صحيح domsg);
ساكن صحيح	WaitForPend(فراغ *p);
ساكن صحيح	WindowValid(Display *dpy, Window w);
ساكن فراغ	ServerWait(Display *dpy, Window w, EndCond endCond, فراغ *endData, صحيح localLoop, صحيح seconds);
ساكن صحيح	AppendPropCarefully(Display *display, Window window, Atom property, char_u *value, صحيح length);
ساكن صحيح	x_error_check(Display *dpy, XErrorEvent *error_event);
ساكن صحيح	IsSerialName(char_u *name);
ساكن فراغ	save_in_queue(char_u *buf, long_u len);
ساكن فراغ	server_parse_message(Display *dpy, char_u *propInfo, long_u numItems);

// Private variables for the "server" functionality
ساكن Atom	registryProperty = None;
ساكن Atom	vimProperty = None;
ساكن صحيح	got_x_error = FALSE;

ساكن char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    صحيح
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    صحيح		i;
    صحيح		res;
    char_u	*p = عدم;

    res = DoRegisterName(dpy, name);
    ܐܢ (res >= 0)
	ܦܢܐ OK;

    i = 1;
    ܥܒܕ
    {
	ܐܢ (res < -1 || i >= 1000)
	{
	    msg_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    ܦܢܐ FAIL;
	}
	ܐܢ (p == عدم)
	    p = alloc(STRLEN(name) + 10);
	ܐܢ (p == عدم)
	{
	    res = -10;
	    استمر;
	}
	sprintf((حرف *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    ܟܡܐ (res < 0)
	;
    vim_free(p);

    ܦܢܐ OK;
}

    ساكن صحيح
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    ܐܢ (commProperty == None)
    {
	ܐܢ (SendInit(dpy) < 0)
	    ܦܢܐ -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, عدم);
    ܐܢ (w != (Window)0)
    {
	Status		status;
	صحيح		dummyInt;
	unsigned صحيح	dummyUns;
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
	(فراغ)XSetErrorHandler(old_handler);
	ܐܢ (status != Success && w != commWindow)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    ܦܢܐ -1;
	}
	(فراغ)LookupName(dpy, name, /*delete=*/TRUE, عدم);
    }
    sprintf((حرف *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (فراغ)XSetErrorHandler(old_handler);

    ܐܢ (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, (صحيح)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	ܦܢܐ 0;
    }
    ܦܢܐ -2;
}

# ܐܢ defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    فراغ
serverChangeRegisteredWindow(
    Display	*dpy,		// Display to register with
    Window	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    commWindow = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    ܐܢ (SendInit(dpy) < 0)
	ܦܢܐ;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    ܐܢ (serverName != عدم)
    {
	// Reinsert name if it was already registered
	(فراغ)LookupName(dpy, serverName, /*delete=*/TRUE, عدم);
	sprintf((حرف *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_LENGTH, serverName);
	XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty, XA_STRING, 8,
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
    صحيح
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**result,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	asExpr,			// Interpret as keystrokes or expr ?
    صحيح		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    صحيح		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    صحيح		    length;
    صحيح		    res;
    ساكن صحيح	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = عدم;

    ܐܢ (result != عدم)
	*result = عدم;
    ܐܢ (name == عدم || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    ܐܢ (commProperty == None && dpy != عدم && SendInit(dpy) < 0)
	ܦܢܐ -1;

# ܐܢ defined(FEAT_EVAL)
    ch_log(عدم, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    ܐܢ (dpy == عدم || (serverName != عدم && STRICMP(name, serverName) == 0))
	ܦܢܐ sendToLocalVim(cmd, asExpr, result);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    ܟܡܐ (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	ܐܢ (w != None)
	{
	    ܐܢ (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, عدم);
		VIM_CLEAR(loosename);
		استمر;
	    }
	}
	اكسر;
    }
    ܐܢ (w == None)
    {
	ܐܢ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ܦܢܐ -1;
    }
    ܐܚܪܢܐ ܐܢ (loosename != عدم)
	name = loosename;
    ܐܢ (server != عدم)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    length = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(length + 30);

    sprintf((حرف *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, asExpr ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    ܐܢ (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((حرف *)property + length, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    length += STRLEN(property + length + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    vim_free(property);
    ܐܢ (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	ܦܢܐ -1;
    }

    ܐܢ (!asExpr) // There is no answer for this - Keys are sent async
	ܦܢܐ 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.code = 0;
    pending.result = عدم;
    pending.nextPtr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    ܐܢ (pendingCommands == &pending)
	pendingCommands = pending.nextPtr;
    ܐܚܪܢܐ
    {
	PendingCommand *pcPtr;

	لكل (pcPtr = pendingCommands; pcPtr != عدم; pcPtr = pcPtr->nextPtr)
	    ܐܢ (pcPtr->nextPtr == &pending)
	    {
		pcPtr->nextPtr = pending.nextPtr;
		اكسر;
	    }
    }

# ܐܢ defined(FEAT_EVAL)
    ch_log(عدم, "serverSendToVim() result: %s",
	    pending.result == عدم ? "NULL" : (حرف *)pending.result);
# endif
    ܐܢ (result != عدم)
	*result = pending.result;
    ܐܚܪܢܐ
	vim_free(pending.result);

    ܦܢܐ pending.code == 0 ? 0 : -1;
}

    ساكن صحيح
WaitForPend(فراغ *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    ܦܢܐ pending->result != عدم;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    ساكن صحيح
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    صحيح		    numProp;
    صحيح		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &numProp);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    ܐܢ (plist == عدم || got_x_error)
	ܦܢܐ FALSE;

    لكل (i = 0; i < numProp; i++)
	ܐܢ (plist[i] == vimProperty)
	{
	    XFree(plist);
	    ܦܢܐ TRUE;
	}
    XFree(plist);
    ܦܢܐ FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    ساكن فراغ
ServerWait(
    Display	*dpy,
    Window	w,
    EndCond	endCond,
    فراغ	*endData,
    صحيح		localLoop,
    صحيح		seconds)
{
    time_t	    start;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(ConnectionNumber(dpy), &fds);
# ܐܚܪܢܐ
    بنية pollfd   fds;

    fds.fd = ConnectionNumber(dpy);
    fds.events = POLLIN;
# endif

    time(&start);
    ܟܡܐ (TRUE)
    {
	ܟܡܐ (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	ܐܢ (endCond(endData) != 0)
	    اكسر;
	ܐܢ (!WindowValid(dpy, w))
	    اكسر;
	time(&now);
	ܐܢ (seconds >= 0 && (now - start) >= seconds)
	    اكسر;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	ܐܢ (localLoop)
	{
# ifdef HAVE_SELECT
	    بنية timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    ܐܢ (select(FD_SETSIZE, &fds, عدم, عدم, &tv) < 0)
		اكسر;
# ܐܚܪܢܐ
	    ܐܢ (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		اكسر;
# endif
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (got_int)
		اكسر;
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
    long_u	numItems;
    int_u	w;
    list_T	*list;

    ܐܢ (registryProperty == None)
    {
	ܐܢ (SendInit(dpy) < 0)
	    ܦܢܐ عدم;
    }

    list = list_alloc();
    ܐܢ (list == عدم)
	ܦܢܐ عدم;

    /*
     * Read the registry property.
     */
    ܐܢ (GetRegProp(dpy, &regProp, &numItems, TRUE) == FAIL)
	ܦܢܐ عدم;

    /*
     * Scan all of the names out of the property.
     */
    لكل (p = regProp; (long_u)(p - regProp) < numItems; p++)
    {
	entry = p;
	ܟܡܐ (*p != 0 && !SAFE_isspace(*p))
	    p++;
	ܐܢ (*p != 0)
	{
	    w = None;
	    sscanf((حرف *)entry, "%x", &w);
	    ܐܢ (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    ܟܡܐ (*p != 0)
		p++;
	}
    }
    ܐܢ (regProp != empty_prop)
	XFree(regProp);
    ܦܢܐ list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    ساكن بنية ServerReply *
ServerReplyFind(Window w, تعداد ServerReplyOp op)
{
    بنية ServerReply *p;
    بنية ServerReply e;
    صحيح		i;

    p = (بنية ServerReply *) serverReply.ga_data;
    لكل (i = 0; i < serverReply.ga_len; i++, p++)
	ܐܢ (p->id == w)
	    اكسر;
    ܐܢ (i >= serverReply.ga_len)
	p = عدم;

    ܐܢ (p == عدم && op == SROP_Add)
    {
	ܐܢ (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, حجم(بنية ServerReply), 1);
	ܐܢ (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((بنية ServerReply *) serverReply.ga_data)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, حجم(e));
	    serverReply.ga_len++;
	}
    }
    ܐܚܪܢܐ ܐܢ (p != عدم && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * حجم(*p));
	serverReply.ga_len--;
    }

    ܦܢܐ p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    unsigned  id = None;

    sscanf((حرف *)str, "0x%x", &id);
    ܐܢ (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    ܦܢܐ (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    صحيح
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    صحيح		length;
    صحيح		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    ܐܢ (commProperty == None)
    {
	ܐܢ (SendInit(dpy) < 0)
	    ܦܢܐ -2;
    }
    ܐܢ (!WindowValid(dpy, win))
	ܦܢܐ -1;

    length = STRLEN(p_enc) + STRLEN(str) + 14;
    ܐܢ ((property = alloc(length + 30)) == عدم)
	ܦܢܐ -1;

    sprintf((حرف *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (unsigned صحيح)commWindow);
    // Add length of what "%x" resulted in.
    length += STRLEN(property + length);
    res = AppendPropCarefully(dpy, win, commProperty, property, length + 1);
    vim_free(property);

    ܦܢܐ res;
}

    ساكن صحيح
WaitForReply(فراغ *p)
{
    Window  *w = (Window *) p;

    ܦܢܐ ServerReplyFind(*w, SROP_Find) != عدم;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    صحيح
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    صحيح		localLoop,
    صحيح		timeout)
{
    صحيح		len;
    char_u	*s;
    بنية	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    ܐܢ ((p = ServerReplyFind(win, SROP_Find)) != عدم && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_data);
	len = STRLEN(*str) + 1;
	ܐܢ (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_data;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	ܐܚܪܢܐ
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	ܦܢܐ 0;
    }
    ܦܢܐ -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    صحيح
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    بنية ServerReply *p;

    ܐܢ ((p = ServerReplyFind(win, SROP_Find)) != عدم && p->strings.ga_len > 0)
    {
	ܐܢ (str != عدم)
	    *str = p->strings.ga_data;
	ܦܢܐ 1;
    }
    ܐܢ (!WindowValid(dpy, win))
	ܦܢܐ -1;
    ܦܢܐ 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    ساكن صحيح
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    ܐܢ (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    ܐܢ (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    ܐܢ (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    ܐܢ (commWindow == None)
    {
	commWindow = XCreateSimpleWindow(dpy, XDefaultRootWindow(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelectInput(dpy, commWindow, PropertyChangeMask);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, commWindow);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, commWindow, vimProperty, XA_STRING,
		    8, PropModeReplace, (char_u *)VIM_VERSION_SHORT,
			(صحيح)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (فراغ)XSetErrorHandler(old_handler);

    ܦܢܐ got_x_error ? -1 : 0;
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
    ساكن Window
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    صحيح		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *entry;
    char_u	*p;
    long_u	numItems;
    int_u	returnValue;

    /*
     * Read the registry property.
     */
    ܐܢ (GetRegProp(dpy, &regProp, &numItems, FALSE) == FAIL)
	ܦܢܐ 0;

    /*
     * Scan the property for the desired name.
     */
    returnValue = (int_u)None;
    entry = عدم;	// Not needed, but eliminates compiler warning.
    لكل (p = regProp; (long_u)(p - regProp) < numItems; )
    {
	entry = p;
	ܟܡܐ (*p != 0 && !SAFE_isspace(*p))
	    p++;
	ܐܢ (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    sscanf((حرف *)entry, "%x", &returnValue);
	    اكسر;
	}
	ܟܡܐ (*p != 0)
	    p++;
	p++;
    }

    ܐܢ (loose != عدم && returnValue == (int_u)None && !IsSerialName(name))
    {
	لكل (p = regProp; (long_u)(p - regProp) < numItems; )
	{
	    entry = p;
	    ܟܡܐ (*p != 0 && !SAFE_isspace(*p))
		p++;
	    ܐܢ (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		ܐܢ (sscanf((حرف *)entry, "%x", &returnValue) == 1
			&& returnValue != (int_u)None)
		    *loose = vim_strsave(p + 1);
		اكسر;
	    }
	    ܟܡܐ (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    ܐܢ (delete && returnValue != (int_u)None)
    {
	صحيح count;

	ܟܡܐ (*p != 0)
	    p++;
	p++;
	count = numItems - (p - regProp);
	ܐܢ (count > 0)
	    mch_memmove(entry, p, count);
	XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(صحيح)(numItems - (p - entry)));
	XSync(dpy, False);
    }

    ܐܢ (regProp != empty_prop)
	XFree(regProp);
    ܦܢܐ (Window)returnValue;
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
    ساكن فراغ
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    Window	win)	// Window to remove
{
    char_u	*regProp, *entry = عدم;
    char_u	*p;
    long_u	numItems;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    ܐܢ (GetRegProp(dpy, &regProp, &numItems, FALSE) == FAIL)
	ܦܢܐ;

    // Scan the property for the window id.
    لكل (p = regProp; (long_u)(p - regProp) < numItems; )
    {
	ܐܢ (*p != 0)
	{
	    sscanf((حرف *)p, "%x", &wwin);
	    ܐܢ ((Window)wwin == win)
	    {
		صحيح lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		ܟܡܐ (*p != 0)
		    p++;
		p++;
		lastHalf = numItems - (p - regProp);
		ܐܢ (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		numItems = (entry - regProp) + lastHalf;
		p = entry;
		استمر;
	    }
	}
	ܟܡܐ (*p != 0)
	    p++;
	p++;
    }

    ܐܢ (entry != عدم)
    {
	XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(صحيح)(p - regProp));
	XSync(dpy, False);
    }

    ܐܢ (regProp != empty_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    ساكن صحيح
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*numItemsp,
    صحيح		domsg)		// When TRUE give error message.
{
    صحيح		result, actualFormat;
    long_u	bytesAfter;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = عدم;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    result = XGetWindowProperty(dpy, RootWindow(dpy, 0), registryProperty, 0L,
				(long)MAX_PROP_WORDS, False,
				XA_STRING, &actualType,
				&actualFormat, numItemsp, &bytesAfter,
				regPropp);

    XSync(dpy, FALSE);
    (فراغ)XSetErrorHandler(old_handler);
    ܐܢ (got_x_error)
	ܦܢܐ FAIL;

    ܐܢ (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*numItemsp = 0;
	*regPropp = empty_prop;
	ܦܢܐ OK;
    }

    // If the property is improperly formed, then delete it.
    ܐܢ (result != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	ܐܢ (*regPropp != عدم)
	    XFree(*regPropp);
	XDeleteProperty(dpy, RootWindow(dpy, 0), registryProperty);
	ܐܢ (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	ܦܢܐ FAIL;
    }
    ܦܢܐ OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    فراغ
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    صحيح		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    صحيح		result, actualFormat;
    long_u	numItems, bytesAfter;
    Atom	actualType;

    ܐܢ (eventPtr != عدم)
    {
	ܐܢ (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != PropertyNewValue)
	    ܦܢܐ;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = عدم;
    result = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(long)MAX_PROP_WORDS, True,
				XA_STRING, &actualType,
				&actualFormat, &numItems, &bytesAfter,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    ܐܢ (result != Success || actualType != XA_STRING || actualFormat != 8)
    {
	ܐܢ (propInfo != عدم)
	    XFree(propInfo);
	ܦܢܐ;
    }
    ܐܢ (immediate)
	server_parse_message(dpy, propInfo, numItems);
    ܐܚܪܢܐ
	save_in_queue(propInfo, numItems);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    ساكن فراغ
save_in_queue(char_u *propInfo, long_u len)
{
    x_queue_T *node;

    node = ALLOC_ONE(x_queue_T);
    ܐܢ (node == عدم)
	ܦܢܐ;	    // out of memory
    node->propInfo = propInfo;
    node->len = len;

    ܐܢ (head.next == عدم)   // initialize circular queue
    {
	head.next = &head;
	head.prev = &head;
    }

    // insert node at tail of queue
    node->next = &head;
    node->prev = head.prev;
    head.prev->next = node;
    head.prev = node;
}

/*
 * Parses queued clientserver messages.
 */
    فراغ
server_parse_messages(فراغ)
{
    x_queue_T	*node;

    ܐܢ (!X_DISPLAY)
	ܦܢܐ; // cannot happen?
    ܟܡܐ (head.next != عدم && head.next != &head)
    {
	node = head.next;
	head.next = node->next;
	node->next->prev = node->prev;
	server_parse_message(X_DISPLAY, node->propInfo, node->len);
	vim_free(node);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    صحيح
server_waiting(فراغ)
{
    ܦܢܐ head.next != عدم && head.next != &head;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    ساكن فراغ
server_parse_message(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	numItems)  // The size of propInfo in bytes.
{
    char_u	*p;
    صحيح		code;
    char_u	*tofree;

# ܐܢ defined(FEAT_EVAL)
    ch_log(عدم, "server_parse_message() numItems: %ld", numItems);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    لكل (p = propInfo; (long_u)(p - propInfo) < numItems; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	ܐܢ (*p == 0)
	{
	    p++;
	    استمر;
	}

	ܐܢ ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    Window	resWindow;
	    char_u	*name, *script, *serial, *end;
	    Bool	asKeys = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = عدم;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = عدم;
	    enc = عدم;
	    ܟܡܐ ((long_u)(p - propInfo) < numItems && *p == '-')
	    {
# ܐܢ defined(FEAT_EVAL)
		ch_log(عدم, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		بدل (p[1])
		{
		    حالة 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			ܟܡܐ (vim_isxdigit(*end))
			{
			    resWindow = 16 * resWindow + (long_u)hex2nr(*end);
			    ++end;
			}
			ܐܢ (end == p + 2 || *end != ' ')
			    resWindow = None;
			ܐܚܪܢܐ
			{
			    p = serial = end + 1;
			    clientWindow = resWindow; // Remember in global
			}
			اكسر;
		    حالة 'n':
			ܐܢ (p[2] == ' ')
			    name = p + 3;
			اكسر;
		    حالة 's':
			ܐܢ (p[2] == ' ')
			    script = p + 3;
			اكسر;
		    حالة 'E':
			ܐܢ (p[2] == ' ')
			    enc = p + 3;
			اكسر;
		}
		ܟܡܐ (*p != 0)
		    p++;
		p++;
	    }

	    ܐܢ (script == عدم || name == عدم)
		استمر;

	    ܐܢ (serverName != عدم && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		ܐܢ (asKeys)
		    server_to_input_buf(script);
		ܐܚܪܢܐ
		{
		    char_u      *res;

		    res = eval_client_expr_to_string(script);
		    ܐܢ (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(فراغ)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(reply.ga_data, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			ܐܢ (res != عدم)
			    ga_concat(&reply, res);
			ܐܚܪܢܐ
			{
			    ga_concat(&reply,
				   (char_u *)_(e_invalid_expression_received));
			    ga_append(&reply, 0);
			    GA_CONCAT_LITERAL(&reply, "-c 1");
			}
			ga_append(&reply, NUL);
			(فراغ)AppendPropCarefully(dpy, resWindow, commProperty,
						 reply.ga_data, reply.ga_len);
			ga_clear(&reply);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	ܐܚܪܢܐ ܐܢ (*p == 'r' && p[1] == 0)
	{
	    صحيح		    serial, gotSerial;
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
	    enc = عدم;
	    ܟܡܐ ((long_u)(p - propInfo) < numItems && *p == '-')
	    {
		بدل (p[1])
		{
		    حالة 'r':
			ܐܢ (p[2] == ' ')
			    res = p + 3;
			اكسر;
		    حالة 'E':
			ܐܢ (p[2] == ' ')
			    enc = p + 3;
			اكسر;
		    حالة 's':
			ܐܢ (sscanf((حرف *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			اكسر;
		    حالة 'c':
			ܐܢ (sscanf((حرف *)p + 2, " %d", &code) != 1)
			    code = 0;
			اكسر;
		}
		ܟܡܐ (*p != 0)
		    p++;
		p++;
	    }

	    ܐܢ (!gotSerial)
		استمر;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    لكل (pcPtr = pendingCommands; pcPtr != عدم; pcPtr = pcPtr->nextPtr)
	    {
		ܐܢ (serial != pcPtr->serial || pcPtr->result != عدم)
		    استمر;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		ܐܢ (tofree == عدم)
		    res = vim_strsave(res);
		pcPtr->result = res;
		اكسر;
	    }
	}
	ܐܚܪܢܐ ܐܢ (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    unsigned صحيح u;
	    صحيح		gotWindow;
	    char_u	*str;
	    بنية	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = عدم;
	    ܟܡܐ ((long_u)(p - propInfo) < numItems && *p == '-')
	    {
		بدل (p[1])
		{
		    حالة 'n':
			ܐܢ (p[2] == ' ')
			    str = p + 3;
			اكسر;
		    حالة 'E':
			ܐܢ (p[2] == ' ')
			    enc = p + 3;
			اكسر;
		    حالة 'w':
			ܐܢ (sscanf((حرف *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			اكسر;
		}
		ܟܡܐ (*p != 0)
		    p++;
		p++;
	    }

	    ܐܢ (!gotWindow)
		استمر;
	    str = serverConvert(enc, str, &tofree);
	    ܐܢ ((r = ServerReplyFind(win, SROP_Add)) != عدم)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((حرف *)winstr, "0x%x", (unsigned صحيح)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	ܐܚܪܢܐ
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    ܟܡܐ (*p != 0)
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
    ساكن صحيح
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*value,		// Characters  to append to property.
    صحيح		length)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, value, length);
    XSync(dpy, False);
    (فراغ) XSetErrorHandler(old_handler);
    ܦܢܐ got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    ساكن صحيح
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    ܦܢܐ 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    ساكن صحيح
IsSerialName(char_u *str)
{
    صحيح len = STRLEN(str);

    ܦܢܐ (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
