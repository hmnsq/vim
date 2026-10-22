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

#অন্তর্ভাৱ "vim.h"
#অন্তর্ভাৱ "version.h"

#যদি (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

প্রকারনির্দেশ সংরচনা PendingCommand
{
    পূর্ণাঙ্ক	    serial;	// Serial number expected in result.
    পূর্ণাঙ্ক	    code;	// Result Code. 0 is OK
    char_u  *result;	// String result for command (malloc'ed).
			// NULL means command still pending.
    সংরচনা PendingCommand *nextPtr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

স্থির PendingCommand *pendingCommands = রিক্ত;
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

# define MAX_PROP_শব্দগুলি 100000

সংরচনা ServerReply
{
    Window  id;
    garray_T strings;
};
স্থির garray_T serverReply = { 0, 0, 0, 0, 0 };
গণনা ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

প্রকারনির্দেশ পূর্ণাঙ্ক (*EndCond)(শূন্য *);

সংরচনা x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    সংরচনা x_cmdqueue	*next;
    সংরচনা x_cmdqueue	*prev;
};

প্রকারনির্দেশ সংরচনা x_cmdqueue x_queue_T;

// dummy node, header for circular queue
স্থির x_queue_T head = {রিক্ত, 0, রিক্ত, রিক্ত};

/*
 * Forward declarations for procedures defined later in this file:
 */

স্থির Window	LookupName(Display *dpy, char_u *name, পূর্ণাঙ্ক delete, char_u **loose);
স্থির পূর্ণাঙ্ক	SendInit(Display *dpy);
স্থির পূর্ণাঙ্ক	DoRegisterName(Display *dpy, char_u *name);
স্থির শূন্য	DeleteAnyLingerer(Display *dpy, Window w);
স্থির পূর্ণাঙ্ক	GetRegProp(Display *dpy, char_u **regPropp, long_u *অঙ্ক_Itemsp, পূর্ণাঙ্ক domsg);
স্থির পূর্ণাঙ্ক	WaitForPend(শূন্য *p);
স্থির পূর্ণাঙ্ক	WindowValid(Display *dpy, Window w);
স্থির শূন্য	ServerWait(Display *dpy, Window w, EndCond endCond, শূন্য *endData, পূর্ণাঙ্ক localLoop, পূর্ণাঙ্ক seconds);
স্থির পূর্ণাঙ্ক	AppendPropCarefully(Display *display, Window window, Atom property, char_u *মান, পূর্ণাঙ্ক length);
স্থির পূর্ণাঙ্ক	x_error_check(Display *dpy, XErrorEvent *error_event);
স্থির পূর্ণাঙ্ক	IsSerialName(char_u *name);
স্থির শূন্য	save_in_queue(char_u *buf, long_u len);
স্থির শূন্য	server_parse_message(Display *dpy, char_u *propInfo, long_u অঙ্ক_বস্তুসমূহ);

// Private variables for the "server" functionality
স্থির Atom	registryProperty = None;
স্থির Atom	vimProperty = None;
স্থির পূর্ণাঙ্ক	got_x_error = FALSE;

স্থির char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    পূর্ণাঙ্ক
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		res;
    char_u	*p = রিক্ত;

    res = DoRegisterName(dpy, name);
    যদি (res >= 0)
	ঘূৰাই OK;

    i = 1;
    কৰা
    {
	যদি (res < -1 || i >= 1000)
	{
	    বাৰ্তা_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    ঘূৰাই FAIL;
	}
	যদি (p == রিক্ত)
	    p = alloc(STRLEN(name) + 10);
	যদি (p == রিক্ত)
	{
	    res = -10;
	    অনুৱর্ত;
	}
	sprintf((ৱর্ণ *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    যেতিয়ালৈ (res < 0)
	;
    vim_free(p);

    ঘূৰাই OK;
}

    স্থির পূর্ণাঙ্ক
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    যদি (commProperty == None)
    {
	যদি (SendInit(dpy) < 0)
	    ঘূৰাই -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, রিক্ত);
    যদি (w != (Window)0)
    {
	Status		status;
	পূর্ণাঙ্ক		dummyInt;
	unsigned পূর্ণাঙ্ক	dummyUns;
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
	(শূন্য)XSetErrorHandler(old_handler);
	যদি (status != Success && w != commWindow)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    ঘূৰাই -1;
	}
	(শূন্য)LookupName(dpy, name, /*delete=*/TRUE, রিক্ত);
    }
    sprintf((ৱর্ণ *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, মূল_Window(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (শূন্য)XSetErrorHandler(old_handler);

    যদি (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, (পূর্ণাঙ্ক)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	ঘূৰাই 0;
    }
    ঘূৰাই -2;
}

# যদি defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    শূন্য
serverChangeRegisteredWindow(
    Display	*dpy,		// Display to register with
    Window	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    commWindow = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    যদি (SendInit(dpy) < 0)
	ঘূৰাই;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    যদি (serverName != রিক্ত)
    {
	// Reinsert name if it was already registered
	(শূন্য)LookupName(dpy, serverName, /*delete=*/TRUE, রিক্ত);
	sprintf((ৱর্ণ *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_LENGTH, serverName);
	XChangeProperty(dpy, মূল_Window(dpy, 0), registryProperty, XA_STRING, 8,
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
    পূর্ণাঙ্ক
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**result,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	as_ৰাশিমালা,			// Interpret as keystrokes or expr ?
    পূর্ণাঙ্ক		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    পূর্ণাঙ্ক		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    পূর্ণাঙ্ক		    length;
    পূর্ণাঙ্ক		    res;
    স্থির পূর্ণাঙ্ক	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = রিক্ত;

    যদি (result != রিক্ত)
	*result = রিক্ত;
    যদি (name == রিক্ত || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    যদি (commProperty == None && dpy != রিক্ত && SendInit(dpy) < 0)
	ঘূৰাই -1;

# যদি defined(FEAT_EVAL)
    ch_log(রিক্ত, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    যদি (dpy == রিক্ত || (serverName != রিক্ত && STRICMP(name, serverName) == 0))
	ঘূৰাই sendToLocalVim(cmd, as_ৰাশিমালা, result);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    যেতিয়ালৈ (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	যদি (w != None)
	{
	    যদি (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, রিক্ত);
		VIM_CLEAR(loosename);
		অনুৱর্ত;
	    }
	}
	ভঙ্গ;
    }
    যদি (w == None)
    {
	যদি (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ঘূৰাই -1;
    }
    অন্যথা যদি (loosename != রিক্ত)
	name = loosename;
    যদি (server != রিক্ত)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    length = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(length + 30);

    sprintf((ৱর্ণ *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_ৰাশিমালা ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    যদি (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((ৱর্ণ *)property + length, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    length += STRLEN(property + length + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    vim_free(property);
    যদি (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	ঘূৰাই -1;
    }

    যদি (!as_ৰাশিমালা) // There is no answer for this - Keys are sent async
	ঘূৰাই 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.code = 0;
    pending.result = রিক্ত;
    pending.nextPtr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    যদি (pendingCommands == &pending)
	pendingCommands = pending.nextPtr;
    অন্যথা
    {
	PendingCommand *pcPtr;

	বাবে (pcPtr = pendingCommands; pcPtr != রিক্ত; pcPtr = pcPtr->nextPtr)
	    যদি (pcPtr->nextPtr == &pending)
	    {
		pcPtr->nextPtr = pending.nextPtr;
		ভঙ্গ;
	    }
    }

# যদি defined(FEAT_EVAL)
    ch_log(রিক্ত, "serverSendToVim() result: %s",
	    pending.result == রিক্ত ? "NULL" : (ৱর্ণ *)pending.result);
# endif
    যদি (result != রিক্ত)
	*result = pending.result;
    অন্যথা
	vim_free(pending.result);

    ঘূৰাই pending.code == 0 ? 0 : -1;
}

    স্থির পূর্ণাঙ্ক
WaitForPend(শূন্য *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    ঘূৰাই pending->result != রিক্ত;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    স্থির পূর্ণাঙ্ক
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    পূর্ণাঙ্ক		    অঙ্ক_Prop;
    পূর্ণাঙ্ক		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &অঙ্ক_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    যদি (plist == রিক্ত || got_x_error)
	ঘূৰাই FALSE;

    বাবে (i = 0; i < অঙ্ক_Prop; i++)
	যদি (plist[i] == vimProperty)
	{
	    XFree(plist);
	    ঘূৰাই TRUE;
	}
    XFree(plist);
    ঘূৰাই FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    স্থির শূন্য
ServerWait(
    Display	*dpy,
    Window	w,
    EndCond	endCond,
    শূন্য	*endData,
    পূর্ণাঙ্ক		localLoop,
    পূর্ণাঙ্ক		seconds)
{
    time_t	    start;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_সংখ্যা(dpy), &fds);
# অন্যথা
    সংরচনা pollfd   fds;

    fds.fd = Connection_সংখ্যা(dpy);
    fds.events = POLLIN;
# endif

    time(&start);
    যেতিয়ালৈ (TRUE)
    {
	যেতিয়ালৈ (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	যদি (endCond(endData) != 0)
	    ভঙ্গ;
	যদি (!WindowValid(dpy, w))
	    ভঙ্গ;
	time(&now);
	যদি (seconds >= 0 && (now - start) >= seconds)
	    ভঙ্গ;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	যদি (localLoop)
	{
# ifdef HAVE_SELECT
	    সংরচনা timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    যদি (select(FD_SETSIZE, &fds, রিক্ত, রিক্ত, &tv) < 0)
		ভঙ্গ;
# অন্যথা
	    যদি (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		ভঙ্গ;
# endif
	}
	অন্যথা
	{
	    যদি (got_int)
		ভঙ্গ;
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
    long_u	অঙ্ক_বস্তুসমূহ;
    int_u	w;
    list_T	*list;

    যদি (registryProperty == None)
    {
	যদি (SendInit(dpy) < 0)
	    ঘূৰাই রিক্ত;
    }

    list = list_alloc();
    যদি (list == রিক্ত)
	ঘূৰাই রিক্ত;

    /*
     * Read the registry property.
     */
    যদি (GetRegProp(dpy, &regProp, &অঙ্ক_বস্তুসমূহ, TRUE) == FAIL)
	ঘূৰাই রিক্ত;

    /*
     * Scan all of the names out of the property.
     */
    বাবে (p = regProp; (long_u)(p - regProp) < অঙ্ক_বস্তুসমূহ; p++)
    {
	entry = p;
	যেতিয়ালৈ (*p != 0 && !SAFE_isspace(*p))
	    p++;
	যদি (*p != 0)
	{
	    w = None;
	    sscanf((ৱর্ণ *)entry, "%x", &w);
	    যদি (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    যেতিয়ালৈ (*p != 0)
		p++;
	}
    }
    যদি (regProp != empty_prop)
	XFree(regProp);
    ঘূৰাই list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    স্থির সংরচনা ServerReply *
ServerReplyFind(Window w, গণনা ServerReplyOp op)
{
    সংরচনা ServerReply *p;
    সংরচনা ServerReply e;
    পূর্ণাঙ্ক		i;

    p = (সংরচনা ServerReply *) serverReply.ga_data;
    বাবে (i = 0; i < serverReply.ga_len; i++, p++)
	যদি (p->id == w)
	    ভঙ্গ;
    যদি (i >= serverReply.ga_len)
	p = রিক্ত;

    যদি (p == রিক্ত && op == SROP_Add)
    {
	যদি (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, পরিমাণ(সংরচনা ServerReply), 1);
	যদি (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((সংরচনা ServerReply *) serverReply.ga_data)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, পরিমাণ(e));
	    serverReply.ga_len++;
	}
    }
    অন্যথা যদি (p != রিক্ত && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * পরিমাণ(*p));
	serverReply.ga_len--;
    }

    ঘূৰাই p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    unsigned  id = None;

    sscanf((ৱর্ণ *)str, "0x%x", &id);
    যদি (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    ঘূৰাই (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    পূর্ণাঙ্ক
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    পূর্ণাঙ্ক		length;
    পূর্ণাঙ্ক		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    যদি (commProperty == None)
    {
	যদি (SendInit(dpy) < 0)
	    ঘূৰাই -2;
    }
    যদি (!WindowValid(dpy, win))
	ঘূৰাই -1;

    length = STRLEN(p_enc) + STRLEN(str) + 14;
    যদি ((property = alloc(length + 30)) == রিক্ত)
	ঘূৰাই -1;

    sprintf((ৱর্ণ *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (unsigned পূর্ণাঙ্ক)commWindow);
    // Add length of what "%x" resulted in.
    length += STRLEN(property + length);
    res = AppendPropCarefully(dpy, win, commProperty, property, length + 1);
    vim_free(property);

    ঘূৰাই res;
}

    স্থির পূর্ণাঙ্ক
WaitForReply(শূন্য *p)
{
    Window  *w = (Window *) p;

    ঘূৰাই ServerReplyFind(*w, SROP_Find) != রিক্ত;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    পূর্ণাঙ্ক
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    পূর্ণাঙ্ক		localLoop,
    পূর্ণাঙ্ক		timeout)
{
    পূর্ণাঙ্ক		len;
    char_u	*s;
    সংরচনা	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    যদি ((p = ServerReplyFind(win, SROP_Find)) != রিক্ত && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_data);
	len = STRLEN(*str) + 1;
	যদি (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_data;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	অন্যথা
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	ঘূৰাই 0;
    }
    ঘূৰাই -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    পূর্ণাঙ্ক
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    সংরচনা ServerReply *p;

    যদি ((p = ServerReplyFind(win, SROP_Find)) != রিক্ত && p->strings.ga_len > 0)
    {
	যদি (str != রিক্ত)
	    *str = p->strings.ga_data;
	ঘূৰাই 1;
    }
    যদি (!WindowValid(dpy, win))
	ঘূৰাই -1;
    ঘূৰাই 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    স্থির পূর্ণাঙ্ক
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    যদি (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    যদি (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    যদি (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    যদি (commWindow == None)
    {
	commWindow = XCreateSimpleWindow(dpy, XDefault_মূল_Window(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_প্ৰবেশ(dpy, commWindow, PropertyChangeMask);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, commWindow);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, commWindow, vimProperty, XA_STRING,
		    8, PropModeReplace, (char_u *)VIM_VERSION_SHORT,
			(পূর্ণাঙ্ক)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (শূন্য)XSetErrorHandler(old_handler);

    ঘূৰাই got_x_error ? -1 : 0;
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
    স্থির Window
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    পূর্ণাঙ্ক		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *entry;
    char_u	*p;
    long_u	অঙ্ক_বস্তুসমূহ;
    int_u	return_মান;

    /*
     * Read the registry property.
     */
    যদি (GetRegProp(dpy, &regProp, &অঙ্ক_বস্তুসমূহ, FALSE) == FAIL)
	ঘূৰাই 0;

    /*
     * Scan the property for the desired name.
     */
    return_মান = (int_u)None;
    entry = রিক্ত;	// Not needed, but eliminates compiler warning.
    বাবে (p = regProp; (long_u)(p - regProp) < অঙ্ক_বস্তুসমূহ; )
    {
	entry = p;
	যেতিয়ালৈ (*p != 0 && !SAFE_isspace(*p))
	    p++;
	যদি (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    sscanf((ৱর্ণ *)entry, "%x", &return_মান);
	    ভঙ্গ;
	}
	যেতিয়ালৈ (*p != 0)
	    p++;
	p++;
    }

    যদি (loose != রিক্ত && return_মান == (int_u)None && !IsSerialName(name))
    {
	বাবে (p = regProp; (long_u)(p - regProp) < অঙ্ক_বস্তুসমূহ; )
	{
	    entry = p;
	    যেতিয়ালৈ (*p != 0 && !SAFE_isspace(*p))
		p++;
	    যদি (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		যদি (sscanf((ৱর্ণ *)entry, "%x", &return_মান) == 1
			&& return_মান != (int_u)None)
		    *loose = vim_strsave(p + 1);
		ভঙ্গ;
	    }
	    যেতিয়ালৈ (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    যদি (delete && return_মান != (int_u)None)
    {
	পূর্ণাঙ্ক গুনতি;

	যেতিয়ালৈ (*p != 0)
	    p++;
	p++;
	গুনতি = অঙ্ক_বস্তুসমূহ - (p - regProp);
	যদি (গুনতি > 0)
	    mch_memmove(entry, p, গুনতি);
	XChangeProperty(dpy, মূল_Window(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(পূর্ণাঙ্ক)(অঙ্ক_বস্তুসমূহ - (p - entry)));
	XSync(dpy, False);
    }

    যদি (regProp != empty_prop)
	XFree(regProp);
    ঘূৰাই (Window)return_মান;
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
    স্থির শূন্য
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    Window	win)	// Window to remove
{
    char_u	*regProp, *entry = রিক্ত;
    char_u	*p;
    long_u	অঙ্ক_বস্তুসমূহ;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    যদি (GetRegProp(dpy, &regProp, &অঙ্ক_বস্তুসমূহ, FALSE) == FAIL)
	ঘূৰাই;

    // Scan the property for the window id.
    বাবে (p = regProp; (long_u)(p - regProp) < অঙ্ক_বস্তুসমূহ; )
    {
	যদি (*p != 0)
	{
	    sscanf((ৱর্ণ *)p, "%x", &wwin);
	    যদি ((Window)wwin == win)
	    {
		পূর্ণাঙ্ক lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		যেতিয়ালৈ (*p != 0)
		    p++;
		p++;
		lastHalf = অঙ্ক_বস্তুসমূহ - (p - regProp);
		যদি (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		অঙ্ক_বস্তুসমূহ = (entry - regProp) + lastHalf;
		p = entry;
		অনুৱর্ত;
	    }
	}
	যেতিয়ালৈ (*p != 0)
	    p++;
	p++;
    }

    যদি (entry != রিক্ত)
    {
	XChangeProperty(dpy, মূল_Window(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(পূর্ণাঙ্ক)(p - regProp));
	XSync(dpy, False);
    }

    যদি (regProp != empty_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    স্থির পূর্ণাঙ্ক
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*অঙ্ক_Itemsp,
    পূর্ণাঙ্ক		domsg)		// When TRUE give error message.
{
    পূর্ণাঙ্ক		result, actualFormat;
    long_u	bytesAfter;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = রিক্ত;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    result = XGetWindowProperty(dpy, মূল_Window(dpy, 0), registryProperty, 0L,
				(long)MAX_PROP_শব্দগুলি, False,
				XA_STRING, &actualType,
				&actualFormat, অঙ্ক_Itemsp, &bytesAfter,
				regPropp);

    XSync(dpy, FALSE);
    (শূন্য)XSetErrorHandler(old_handler);
    যদি (got_x_error)
	ঘূৰাই FAIL;

    যদি (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*অঙ্ক_Itemsp = 0;
	*regPropp = empty_prop;
	ঘূৰাই OK;
    }

    // If the property is improperly formed, then delete it.
    যদি (result != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	যদি (*regPropp != রিক্ত)
	    XFree(*regPropp);
	XDeleteProperty(dpy, মূল_Window(dpy, 0), registryProperty);
	যদি (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    শূন্য
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    পূর্ণাঙ্ক		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    পূর্ণাঙ্ক		result, actualFormat;
    long_u	অঙ্ক_বস্তুসমূহ, bytesAfter;
    Atom	actualType;

    যদি (eventPtr != রিক্ত)
    {
	যদি (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_মান)
	    ঘূৰাই;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = রিক্ত;
    result = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(long)MAX_PROP_শব্দগুলি, True,
				XA_STRING, &actualType,
				&actualFormat, &অঙ্ক_বস্তুসমূহ, &bytesAfter,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    যদি (result != Success || actualType != XA_STRING || actualFormat != 8)
    {
	যদি (propInfo != রিক্ত)
	    XFree(propInfo);
	ঘূৰাই;
    }
    যদি (immediate)
	server_parse_message(dpy, propInfo, অঙ্ক_বস্তুসমূহ);
    অন্যথা
	save_in_queue(propInfo, অঙ্ক_বস্তুসমূহ);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    স্থির শূন্য
save_in_queue(char_u *propInfo, long_u len)
{
    x_queue_T *গ্ৰন্থি;

    গ্ৰন্থি = ALLOC_ONE(x_queue_T);
    যদি (গ্ৰন্থি == রিক্ত)
	ঘূৰাই;	    // out of memory
    গ্ৰন্থি->propInfo = propInfo;
    গ্ৰন্থি->len = len;

    যদি (head.next == রিক্ত)   // initialize circular queue
    {
	head.next = &head;
	head.prev = &head;
    }

    // insert node at tail of queue
    গ্ৰন্থি->next = &head;
    গ্ৰন্থি->prev = head.prev;
    head.prev->next = গ্ৰন্থি;
    head.prev = গ্ৰন্থি;
}

/*
 * Parses queued clientserver messages.
 */
    শূন্য
server_parse_messages(শূন্য)
{
    x_queue_T	*গ্ৰন্থি;

    যদি (!X_DISPLAY)
	ঘূৰাই; // cannot happen?
    যেতিয়ালৈ (head.next != রিক্ত && head.next != &head)
    {
	গ্ৰন্থি = head.next;
	head.next = গ্ৰন্থি->next;
	গ্ৰন্থি->next->prev = গ্ৰন্থি->prev;
	server_parse_message(X_DISPLAY, গ্ৰন্থি->propInfo, গ্ৰন্থি->len);
	vim_free(গ্ৰন্থি);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    পূর্ণাঙ্ক
server_waiting(শূন্য)
{
    ঘূৰাই head.next != রিক্ত && head.next != &head;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    স্থির শূন্য
server_parse_message(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	অঙ্ক_বস্তুসমূহ)  // The size of propInfo in bytes.
{
    char_u	*p;
    পূর্ণাঙ্ক		code;
    char_u	*tofree;

# যদি defined(FEAT_EVAL)
    ch_log(রিক্ত, "server_parse_message() numItems: %ld", অঙ্ক_বস্তুসমূহ);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    বাবে (p = propInfo; (long_u)(p - propInfo) < অঙ্ক_বস্তুসমূহ; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	যদি (*p == 0)
	{
	    p++;
	    অনুৱর্ত;
	}

	যদি ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    Window	resWindow;
	    char_u	*name, *script, *serial, *end;
	    Bool	as_চাবিগুলি = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = রিক্ত;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = রিক্ত;
	    enc = রিক্ত;
	    যেতিয়ালৈ ((long_u)(p - propInfo) < অঙ্ক_বস্তুসমূহ && *p == '-')
	    {
# যদি defined(FEAT_EVAL)
		ch_log(রিক্ত, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		ৱিকল্প (p[1])
		{
		    পক্ষ 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			যেতিয়ালৈ (vim_isxdigit(*end))
			{
			    resWindow = 16 * resWindow + (long_u)hex2nr(*end);
			    ++end;
			}
			যদি (end == p + 2 || *end != ' ')
			    resWindow = None;
			অন্যথা
			{
			    p = serial = end + 1;
			    clientWindow = resWindow; // Remember in global
			}
			ভঙ্গ;
		    পক্ষ 'n':
			যদি (p[2] == ' ')
			    name = p + 3;
			ভঙ্গ;
		    পক্ষ 's':
			যদি (p[2] == ' ')
			    script = p + 3;
			ভঙ্গ;
		    পক্ষ 'E':
			যদি (p[2] == ' ')
			    enc = p + 3;
			ভঙ্গ;
		}
		যেতিয়ালৈ (*p != 0)
		    p++;
		p++;
	    }

	    যদি (script == রিক্ত || name == রিক্ত)
		অনুৱর্ত;

	    যদি (serverName != রিক্ত && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		যদি (as_চাবিগুলি)
		    server_to_প্ৰবেশ_buf(script);
		অন্যথা
		{
		    char_u      *res;

		    res = eval_client_ৰাশিমালা_to_string(script);
		    যদি (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(শূন্য)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(reply.ga_data, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			যদি (res != রিক্ত)
			    ga_concat(&reply, res);
			অন্যথা
			{
			    ga_concat(&reply,
				   (char_u *)_(e_invalid_expression_received));
			    ga_append(&reply, 0);
			    GA_CONCAT_LITERAL(&reply, "-c 1");
			}
			ga_append(&reply, NUL);
			(শূন্য)AppendPropCarefully(dpy, resWindow, commProperty,
						 reply.ga_data, reply.ga_len);
			ga_clear(&reply);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	অন্যথা যদি (*p == 'r' && p[1] == 0)
	{
	    পূর্ণাঙ্ক		    serial, gotSerial;
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
	    enc = রিক্ত;
	    যেতিয়ালৈ ((long_u)(p - propInfo) < অঙ্ক_বস্তুসমূহ && *p == '-')
	    {
		ৱিকল্প (p[1])
		{
		    পক্ষ 'r':
			যদি (p[2] == ' ')
			    res = p + 3;
			ভঙ্গ;
		    পক্ষ 'E':
			যদি (p[2] == ' ')
			    enc = p + 3;
			ভঙ্গ;
		    পক্ষ 's':
			যদি (sscanf((ৱর্ণ *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			ভঙ্গ;
		    পক্ষ 'c':
			যদি (sscanf((ৱর্ণ *)p + 2, " %d", &code) != 1)
			    code = 0;
			ভঙ্গ;
		}
		যেতিয়ালৈ (*p != 0)
		    p++;
		p++;
	    }

	    যদি (!gotSerial)
		অনুৱর্ত;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    বাবে (pcPtr = pendingCommands; pcPtr != রিক্ত; pcPtr = pcPtr->nextPtr)
	    {
		যদি (serial != pcPtr->serial || pcPtr->result != রিক্ত)
		    অনুৱর্ত;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		যদি (tofree == রিক্ত)
		    res = vim_strsave(res);
		pcPtr->result = res;
		ভঙ্গ;
	    }
	}
	অন্যথা যদি (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    unsigned পূর্ণাঙ্ক u;
	    পূর্ণাঙ্ক		gotWindow;
	    char_u	*str;
	    সংরচনা	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = রিক্ত;
	    যেতিয়ালৈ ((long_u)(p - propInfo) < অঙ্ক_বস্তুসমূহ && *p == '-')
	    {
		ৱিকল্প (p[1])
		{
		    পক্ষ 'n':
			যদি (p[2] == ' ')
			    str = p + 3;
			ভঙ্গ;
		    পক্ষ 'E':
			যদি (p[2] == ' ')
			    enc = p + 3;
			ভঙ্গ;
		    পক্ষ 'w':
			যদি (sscanf((ৱর্ণ *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			ভঙ্গ;
		}
		যেতিয়ালৈ (*p != 0)
		    p++;
		p++;
	    }

	    যদি (!gotWindow)
		অনুৱর্ত;
	    str = serverConvert(enc, str, &tofree);
	    যদি ((r = ServerReplyFind(win, SROP_Add)) != রিক্ত)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((ৱর্ণ *)winstr, "0x%x", (unsigned পূর্ণাঙ্ক)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	অন্যথা
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    যেতিয়ালৈ (*p != 0)
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
    স্থির পূর্ণাঙ্ক
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*মান,		// Characters  to append to property.
    পূর্ণাঙ্ক		length)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, মান, length);
    XSync(dpy, False);
    (শূন্য) XSetErrorHandler(old_handler);
    ঘূৰাই got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    স্থির পূর্ণাঙ্ক
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    ঘূৰাই 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    স্থির পূর্ণাঙ্ক
IsSerialName(char_u *str)
{
    পূর্ণাঙ্ক len = STRLEN(str);

    ঘূৰাই (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
