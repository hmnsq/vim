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

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "version.h"

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

প্রকারলেখ কাঠামো PendingCommand
{
    পূর্ণসংখ্যা	    serial;	// Serial number expected in result.
    পূর্ণসংখ্যা	    code;	// Result Code. 0 is OK
    char_u  *ফলাফল;	// String result for command (malloc'ed).
			// NULL means command still pending.
    কাঠামো PendingCommand *পরবর্তী_Ptr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

স্থির PendingCommand *pendingCommands = ফাঁকা;
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

কাঠামো ServerReply
{
    Window  id;
    garray_T strings;
};
স্থির garray_T serverReply = { 0, 0, 0, 0, 0 };
গণনা ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

প্রকারলেখ পূর্ণসংখ্যা (*EndCond)(শূন্য *);

কাঠামো x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    কাঠামো x_cmdqueue	*পরবর্তী;
    কাঠামো x_cmdqueue	*পূর্ব;
};

প্রকারলেখ কাঠামো x_cmdqueue x_queue_T;

// dummy node, header for circular queue
স্থির x_queue_T শীর্ষ = {ফাঁকা, 0, ফাঁকা, ফাঁকা};

/*
 * Forward declarations for procedures defined later in this file:
 */

স্থির Window	LookupName(Display *dpy, char_u *name, পূর্ণসংখ্যা delete, char_u **loose);
স্থির পূর্ণসংখ্যা	SendInit(Display *dpy);
স্থির পূর্ণসংখ্যা	DoRegisterName(Display *dpy, char_u *name);
স্থির শূন্য	DeleteAnyLingerer(Display *dpy, Window w);
স্থির পূর্ণসংখ্যা	GetRegProp(Display *dpy, char_u **regPropp, long_u *অঙ্ক_Itemsp, পূর্ণসংখ্যা domsg);
স্থির পূর্ণসংখ্যা	WaitForPend(শূন্য *p);
স্থির পূর্ণসংখ্যা	WindowValid(Display *dpy, Window w);
স্থির শূন্য	ServerWait(Display *dpy, Window w, EndCond endCond, শূন্য *end_তথ্য, পূর্ণসংখ্যা localLoop, পূর্ণসংখ্যা seconds);
স্থির পূর্ণসংখ্যা	AppendPropCarefully(Display *display, Window window, Atom property, char_u *মান, পূর্ণসংখ্যা দৈর্ঘ্য);
স্থির পূর্ণসংখ্যা	x_error_check(Display *dpy, XErrorEvent *error_event);
স্থির পূর্ণসংখ্যা	IsSerialName(char_u *name);
স্থির শূন্য	save_in_queue(char_u *buf, long_u len);
স্থির শূন্য	server_parse_message(Display *dpy, char_u *propInfo, long_u অঙ্ক_বস্তুসমূহ);

// Private variables for the "server" functionality
স্থির Atom	registryProperty = None;
স্থির Atom	vimProperty = None;
স্থির পূর্ণসংখ্যা	got_x_error = FALSE;

স্থির char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    পূর্ণসংখ্যা
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		res;
    char_u	*p = ফাঁকা;

    res = DoRegisterName(dpy, name);
    যদি (res >= 0)
	ফেরত OK;

    i = 1;
    করো
    {
	যদি (res < -1 || i >= 1000)
	{
	    বার্তা_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    ফেরত FAIL;
	}
	যদি (p == ফাঁকা)
	    p = alloc(STRLEN(name) + 10);
	যদি (p == ফাঁকা)
	{
	    res = -10;
	    চালু;
	}
	sprintf((অক্ষর *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    যখন (res < 0)
	;
    vim_free(p);

    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_দৈর্ঘ্য 100
    char_u	propInfo[MAX_NAME_দৈর্ঘ্য + 20];

    যদি (commProperty == None)
    {
	যদি (SendInit(dpy) < 0)
	    ফেরত -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, ফাঁকা);
    যদি (w != (Window)0)
    {
	Status		status;
	পূর্ণসংখ্যা		dummyInt;
	unsigned পূর্ণসংখ্যা	dummyUns;
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
	    ফেরত -1;
	}
	(শূন্য)LookupName(dpy, name, /*delete=*/TRUE, ফাঁকা);
    }
    sprintf((অক্ষর *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_দৈর্ঘ্য, name);
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
	set_vim_var_string(VV_SEND_SERVER, name, (পূর্ণসংখ্যা)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	ফেরত 0;
    }
    ফেরত -2;
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
    char_u	propInfo[MAX_NAME_দৈর্ঘ্য + 20];

    commWindow = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    যদি (SendInit(dpy) < 0)
	ফেরত;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    যদি (serverName != ফাঁকা)
    {
	// Reinsert name if it was already registered
	(শূন্য)LookupName(dpy, serverName, /*delete=*/TRUE, ফাঁকা);
	sprintf((অক্ষর *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_দৈর্ঘ্য, serverName);
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
    পূর্ণসংখ্যা
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**ফলাফল,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	as_রাশিমালা,			// Interpret as keystrokes or expr ?
    পূর্ণসংখ্যা		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    পূর্ণসংখ্যা		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    পূর্ণসংখ্যা		    দৈর্ঘ্য;
    পূর্ণসংখ্যা		    res;
    স্থির পূর্ণসংখ্যা	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = ফাঁকা;

    যদি (ফলাফল != ফাঁকা)
	*ফলাফল = ফাঁকা;
    যদি (name == ফাঁকা || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    যদি (commProperty == None && dpy != ফাঁকা && SendInit(dpy) < 0)
	ফেরত -1;

# যদি defined(FEAT_EVAL)
    ch_log(ফাঁকা, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    যদি (dpy == ফাঁকা || (serverName != ফাঁকা && STRICMP(name, serverName) == 0))
	ফেরত sendToLocalVim(cmd, as_রাশিমালা, ফলাফল);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    যখন (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	যদি (w != None)
	{
	    যদি (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, ফাঁকা);
		VIM_CLEAR(loosename);
		চালু;
	    }
	}
	বিরতি;
    }
    যদি (w == None)
    {
	যদি (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ফেরত -1;
    }
    অন্যথা যদি (loosename != ফাঁকা)
	name = loosename;
    যদি (server != ফাঁকা)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    দৈর্ঘ্য = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(দৈর্ঘ্য + 30);

    sprintf((অক্ষর *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_রাশিমালা ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    যদি (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((অক্ষর *)property + দৈর্ঘ্য, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    দৈর্ঘ্য += STRLEN(property + দৈর্ঘ্য + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, দৈর্ঘ্য + 1);
    vim_free(property);
    যদি (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	ফেরত -1;
    }

    যদি (!as_রাশিমালা) // There is no answer for this - Keys are sent async
	ফেরত 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.code = 0;
    pending.ফলাফল = ফাঁকা;
    pending.পরবর্তী_Ptr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    যদি (pendingCommands == &pending)
	pendingCommands = pending.পরবর্তী_Ptr;
    অন্যথা
    {
	PendingCommand *pcPtr;

	জন্য (pcPtr = pendingCommands; pcPtr != ফাঁকা; pcPtr = pcPtr->পরবর্তী_Ptr)
	    যদি (pcPtr->পরবর্তী_Ptr == &pending)
	    {
		pcPtr->পরবর্তী_Ptr = pending.পরবর্তী_Ptr;
		বিরতি;
	    }
    }

# যদি defined(FEAT_EVAL)
    ch_log(ফাঁকা, "serverSendToVim() result: %s",
	    pending.ফলাফল == ফাঁকা ? "NULL" : (অক্ষর *)pending.ফলাফল);
# endif
    যদি (ফলাফল != ফাঁকা)
	*ফলাফল = pending.ফলাফল;
    অন্যথা
	vim_free(pending.ফলাফল);

    ফেরত pending.code == 0 ? 0 : -1;
}

    স্থির পূর্ণসংখ্যা
WaitForPend(শূন্য *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    ফেরত pending->ফলাফল != ফাঁকা;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    স্থির পূর্ণসংখ্যা
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    পূর্ণসংখ্যা		    অঙ্ক_Prop;
    পূর্ণসংখ্যা		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &অঙ্ক_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    যদি (plist == ফাঁকা || got_x_error)
	ফেরত FALSE;

    জন্য (i = 0; i < অঙ্ক_Prop; i++)
	যদি (plist[i] == vimProperty)
	{
	    XFree(plist);
	    ফেরত TRUE;
	}
    XFree(plist);
    ফেরত FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    স্থির শূন্য
ServerWait(
    Display	*dpy,
    Window	w,
    EndCond	endCond,
    শূন্য	*end_তথ্য,
    পূর্ণসংখ্যা		localLoop,
    পূর্ণসংখ্যা		seconds)
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
    কাঠামো pollfd   fds;

    fds.fd = Connection_সংখ্যা(dpy);
    fds.events = POLLIN;
# endif

    time(&start);
    যখন (TRUE)
    {
	যখন (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	যদি (endCond(end_তথ্য) != 0)
	    বিরতি;
	যদি (!WindowValid(dpy, w))
	    বিরতি;
	time(&now);
	যদি (seconds >= 0 && (now - start) >= seconds)
	    বিরতি;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	যদি (localLoop)
	{
# ifdef HAVE_SELECT
	    কাঠামো timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    যদি (select(FD_SETSIZE, &fds, ফাঁকা, ফাঁকা, &tv) < 0)
		বিরতি;
# অন্যথা
	    যদি (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		বিরতি;
# endif
	}
	অন্যথা
	{
	    যদি (got_int)
		বিরতি;
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
	    ফেরত ফাঁকা;
    }

    list = list_alloc();
    যদি (list == ফাঁকা)
	ফেরত ফাঁকা;

    /*
     * Read the registry property.
     */
    যদি (GetRegProp(dpy, &regProp, &অঙ্ক_বস্তুসমূহ, TRUE) == FAIL)
	ফেরত ফাঁকা;

    /*
     * Scan all of the names out of the property.
     */
    জন্য (p = regProp; (long_u)(p - regProp) < অঙ্ক_বস্তুসমূহ; p++)
    {
	entry = p;
	যখন (*p != 0 && !SAFE_isspace(*p))
	    p++;
	যদি (*p != 0)
	{
	    w = None;
	    sscanf((অক্ষর *)entry, "%x", &w);
	    যদি (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    যখন (*p != 0)
		p++;
	}
    }
    যদি (regProp != empty_prop)
	XFree(regProp);
    ফেরত list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    স্থির কাঠামো ServerReply *
ServerReplyFind(Window w, গণনা ServerReplyOp op)
{
    কাঠামো ServerReply *p;
    কাঠামো ServerReply e;
    পূর্ণসংখ্যা		i;

    p = (কাঠামো ServerReply *) serverReply.ga_তথ্য;
    জন্য (i = 0; i < serverReply.ga_len; i++, p++)
	যদি (p->id == w)
	    বিরতি;
    যদি (i >= serverReply.ga_len)
	p = ফাঁকা;

    যদি (p == ফাঁকা && op == SROP_Add)
    {
	যদি (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, আকার(কাঠামো ServerReply), 1);
	যদি (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((কাঠামো ServerReply *) serverReply.ga_তথ্য)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, আকার(e));
	    serverReply.ga_len++;
	}
    }
    অন্যথা যদি (p != ফাঁকা && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * আকার(*p));
	serverReply.ga_len--;
    }

    ফেরত p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    unsigned  id = None;

    sscanf((অক্ষর *)str, "0x%x", &id);
    যদি (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    ফেরত (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    পূর্ণসংখ্যা
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    পূর্ণসংখ্যা		দৈর্ঘ্য;
    পূর্ণসংখ্যা		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    যদি (commProperty == None)
    {
	যদি (SendInit(dpy) < 0)
	    ফেরত -2;
    }
    যদি (!WindowValid(dpy, win))
	ফেরত -1;

    দৈর্ঘ্য = STRLEN(p_enc) + STRLEN(str) + 14;
    যদি ((property = alloc(দৈর্ঘ্য + 30)) == ফাঁকা)
	ফেরত -1;

    sprintf((অক্ষর *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (unsigned পূর্ণসংখ্যা)commWindow);
    // Add length of what "%x" resulted in.
    দৈর্ঘ্য += STRLEN(property + দৈর্ঘ্য);
    res = AppendPropCarefully(dpy, win, commProperty, property, দৈর্ঘ্য + 1);
    vim_free(property);

    ফেরত res;
}

    স্থির পূর্ণসংখ্যা
WaitForReply(শূন্য *p)
{
    Window  *w = (Window *) p;

    ফেরত ServerReplyFind(*w, SROP_Find) != ফাঁকা;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    পূর্ণসংখ্যা
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    পূর্ণসংখ্যা		localLoop,
    পূর্ণসংখ্যা		timeout)
{
    পূর্ণসংখ্যা		len;
    char_u	*s;
    কাঠামো	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    যদি ((p = ServerReplyFind(win, SROP_Find)) != ফাঁকা && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_তথ্য);
	len = STRLEN(*str) + 1;
	যদি (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_তথ্য;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	অন্যথা
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	ফেরত 0;
    }
    ফেরত -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    পূর্ণসংখ্যা
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    কাঠামো ServerReply *p;

    যদি ((p = ServerReplyFind(win, SROP_Find)) != ফাঁকা && p->strings.ga_len > 0)
    {
	যদি (str != ফাঁকা)
	    *str = p->strings.ga_তথ্য;
	ফেরত 1;
    }
    যদি (!WindowValid(dpy, win))
	ফেরত -1;
    ফেরত 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    স্থির পূর্ণসংখ্যা
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
	XSelect_প্রবেশ(dpy, commWindow, PropertyChangeMask);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, commWindow);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, commWindow, vimProperty, XA_STRING,
		    8, PropModeReplace, (char_u *)VIM_VERSION_SHORT,
			(পূর্ণসংখ্যা)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (শূন্য)XSetErrorHandler(old_handler);

    ফেরত got_x_error ? -1 : 0;
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
    পূর্ণসংখ্যা		delete,	    // If non-zero, delete info about name.
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
	ফেরত 0;

    /*
     * Scan the property for the desired name.
     */
    return_মান = (int_u)None;
    entry = ফাঁকা;	// Not needed, but eliminates compiler warning.
    জন্য (p = regProp; (long_u)(p - regProp) < অঙ্ক_বস্তুসমূহ; )
    {
	entry = p;
	যখন (*p != 0 && !SAFE_isspace(*p))
	    p++;
	যদি (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    sscanf((অক্ষর *)entry, "%x", &return_মান);
	    বিরতি;
	}
	যখন (*p != 0)
	    p++;
	p++;
    }

    যদি (loose != ফাঁকা && return_মান == (int_u)None && !IsSerialName(name))
    {
	জন্য (p = regProp; (long_u)(p - regProp) < অঙ্ক_বস্তুসমূহ; )
	{
	    entry = p;
	    যখন (*p != 0 && !SAFE_isspace(*p))
		p++;
	    যদি (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		যদি (sscanf((অক্ষর *)entry, "%x", &return_মান) == 1
			&& return_মান != (int_u)None)
		    *loose = vim_strsave(p + 1);
		বিরতি;
	    }
	    যখন (*p != 0)
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
	পূর্ণসংখ্যা গুনতি;

	যখন (*p != 0)
	    p++;
	p++;
	গুনতি = অঙ্ক_বস্তুসমূহ - (p - regProp);
	যদি (গুনতি > 0)
	    mch_memmove(entry, p, গুনতি);
	XChangeProperty(dpy, মূল_Window(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(পূর্ণসংখ্যা)(অঙ্ক_বস্তুসমূহ - (p - entry)));
	XSync(dpy, False);
    }

    যদি (regProp != empty_prop)
	XFree(regProp);
    ফেরত (Window)return_মান;
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
    char_u	*regProp, *entry = ফাঁকা;
    char_u	*p;
    long_u	অঙ্ক_বস্তুসমূহ;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    যদি (GetRegProp(dpy, &regProp, &অঙ্ক_বস্তুসমূহ, FALSE) == FAIL)
	ফেরত;

    // Scan the property for the window id.
    জন্য (p = regProp; (long_u)(p - regProp) < অঙ্ক_বস্তুসমূহ; )
    {
	যদি (*p != 0)
	{
	    sscanf((অক্ষর *)p, "%x", &wwin);
	    যদি ((Window)wwin == win)
	    {
		পূর্ণসংখ্যা lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		যখন (*p != 0)
		    p++;
		p++;
		lastHalf = অঙ্ক_বস্তুসমূহ - (p - regProp);
		যদি (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		অঙ্ক_বস্তুসমূহ = (entry - regProp) + lastHalf;
		p = entry;
		চালু;
	    }
	}
	যখন (*p != 0)
	    p++;
	p++;
    }

    যদি (entry != ফাঁকা)
    {
	XChangeProperty(dpy, মূল_Window(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(পূর্ণসংখ্যা)(p - regProp));
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
    স্থির পূর্ণসংখ্যা
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*অঙ্ক_Itemsp,
    পূর্ণসংখ্যা		domsg)		// When TRUE give error message.
{
    পূর্ণসংখ্যা		ফলাফল, actualFormat;
    long_u	bytesAfter;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = ফাঁকা;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    ফলাফল = XGetWindowProperty(dpy, মূল_Window(dpy, 0), registryProperty, 0L,
				(long)MAX_PROP_শব্দগুলি, False,
				XA_STRING, &actualType,
				&actualFormat, অঙ্ক_Itemsp, &bytesAfter,
				regPropp);

    XSync(dpy, FALSE);
    (শূন্য)XSetErrorHandler(old_handler);
    যদি (got_x_error)
	ফেরত FAIL;

    যদি (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*অঙ্ক_Itemsp = 0;
	*regPropp = empty_prop;
	ফেরত OK;
    }

    // If the property is improperly formed, then delete it.
    যদি (ফলাফল != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	যদি (*regPropp != ফাঁকা)
	    XFree(*regPropp);
	XDeleteProperty(dpy, মূল_Window(dpy, 0), registryProperty);
	যদি (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	ফেরত FAIL;
    }
    ফেরত OK;
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
    পূর্ণসংখ্যা		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    পূর্ণসংখ্যা		ফলাফল, actualFormat;
    long_u	অঙ্ক_বস্তুসমূহ, bytesAfter;
    Atom	actualType;

    যদি (eventPtr != ফাঁকা)
    {
	যদি (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_মান)
	    ফেরত;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = ফাঁকা;
    ফলাফল = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(long)MAX_PROP_শব্দগুলি, True,
				XA_STRING, &actualType,
				&actualFormat, &অঙ্ক_বস্তুসমূহ, &bytesAfter,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    যদি (ফলাফল != Success || actualType != XA_STRING || actualFormat != 8)
    {
	যদি (propInfo != ফাঁকা)
	    XFree(propInfo);
	ফেরত;
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
    x_queue_T *গ্রন্থি;

    গ্রন্থি = ALLOC_ONE(x_queue_T);
    যদি (গ্রন্থি == ফাঁকা)
	ফেরত;	    // out of memory
    গ্রন্থি->propInfo = propInfo;
    গ্রন্থি->len = len;

    যদি (শীর্ষ.পরবর্তী == ফাঁকা)   // initialize circular queue
    {
	শীর্ষ.পরবর্তী = &শীর্ষ;
	শীর্ষ.পূর্ব = &শীর্ষ;
    }

    // insert node at tail of queue
    গ্রন্থি->পরবর্তী = &শীর্ষ;
    গ্রন্থি->পূর্ব = শীর্ষ.পূর্ব;
    শীর্ষ.পূর্ব->পরবর্তী = গ্রন্থি;
    শীর্ষ.পূর্ব = গ্রন্থি;
}

/*
 * Parses queued clientserver messages.
 */
    শূন্য
server_parse_messages(শূন্য)
{
    x_queue_T	*গ্রন্থি;

    যদি (!X_DISPLAY)
	ফেরত; // cannot happen?
    যখন (শীর্ষ.পরবর্তী != ফাঁকা && শীর্ষ.পরবর্তী != &শীর্ষ)
    {
	গ্রন্থি = শীর্ষ.পরবর্তী;
	শীর্ষ.পরবর্তী = গ্রন্থি->পরবর্তী;
	গ্রন্থি->পরবর্তী->পূর্ব = গ্রন্থি->পূর্ব;
	server_parse_message(X_DISPLAY, গ্রন্থি->propInfo, গ্রন্থি->len);
	vim_free(গ্রন্থি);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    পূর্ণসংখ্যা
server_waiting(শূন্য)
{
    ফেরত শীর্ষ.পরবর্তী != ফাঁকা && শীর্ষ.পরবর্তী != &শীর্ষ;
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
    পূর্ণসংখ্যা		code;
    char_u	*tofree;

# যদি defined(FEAT_EVAL)
    ch_log(ফাঁকা, "server_parse_message() numItems: %ld", অঙ্ক_বস্তুসমূহ);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    জন্য (p = propInfo; (long_u)(p - propInfo) < অঙ্ক_বস্তুসমূহ; )
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
	    চালু;
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
	    name = ফাঁকা;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = ফাঁকা;
	    enc = ফাঁকা;
	    যখন ((long_u)(p - propInfo) < অঙ্ক_বস্তুসমূহ && *p == '-')
	    {
# যদি defined(FEAT_EVAL)
		ch_log(ফাঁকা, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		নির্বাচন (p[1])
		{
		    ক্ষেত্র 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			যখন (vim_isxdigit(*end))
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
			বিরতি;
		    ক্ষেত্র 'n':
			যদি (p[2] == ' ')
			    name = p + 3;
			বিরতি;
		    ক্ষেত্র 's':
			যদি (p[2] == ' ')
			    script = p + 3;
			বিরতি;
		    ক্ষেত্র 'E':
			যদি (p[2] == ' ')
			    enc = p + 3;
			বিরতি;
		}
		যখন (*p != 0)
		    p++;
		p++;
	    }

	    যদি (script == ফাঁকা || name == ফাঁকা)
		চালু;

	    যদি (serverName != ফাঁকা && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		যদি (as_চাবিগুলি)
		    server_to_প্রবেশ_buf(script);
		অন্যথা
		{
		    char_u      *res;

		    res = eval_client_রাশিমালা_to_string(script);
		    যদি (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(শূন্য)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(reply.ga_তথ্য, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			যদি (res != ফাঁকা)
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
						 reply.ga_তথ্য, reply.ga_len);
			ga_clear(&reply);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	অন্যথা যদি (*p == 'r' && p[1] == 0)
	{
	    পূর্ণসংখ্যা		    serial, gotSerial;
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
	    enc = ফাঁকা;
	    যখন ((long_u)(p - propInfo) < অঙ্ক_বস্তুসমূহ && *p == '-')
	    {
		নির্বাচন (p[1])
		{
		    ক্ষেত্র 'r':
			যদি (p[2] == ' ')
			    res = p + 3;
			বিরতি;
		    ক্ষেত্র 'E':
			যদি (p[2] == ' ')
			    enc = p + 3;
			বিরতি;
		    ক্ষেত্র 's':
			যদি (sscanf((অক্ষর *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			বিরতি;
		    ক্ষেত্র 'c':
			যদি (sscanf((অক্ষর *)p + 2, " %d", &code) != 1)
			    code = 0;
			বিরতি;
		}
		যখন (*p != 0)
		    p++;
		p++;
	    }

	    যদি (!gotSerial)
		চালু;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    জন্য (pcPtr = pendingCommands; pcPtr != ফাঁকা; pcPtr = pcPtr->পরবর্তী_Ptr)
	    {
		যদি (serial != pcPtr->serial || pcPtr->ফলাফল != ফাঁকা)
		    চালু;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		যদি (tofree == ফাঁকা)
		    res = vim_strsave(res);
		pcPtr->ফলাফল = res;
		বিরতি;
	    }
	}
	অন্যথা যদি (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    unsigned পূর্ণসংখ্যা u;
	    পূর্ণসংখ্যা		gotWindow;
	    char_u	*str;
	    কাঠামো	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = ফাঁকা;
	    যখন ((long_u)(p - propInfo) < অঙ্ক_বস্তুসমূহ && *p == '-')
	    {
		নির্বাচন (p[1])
		{
		    ক্ষেত্র 'n':
			যদি (p[2] == ' ')
			    str = p + 3;
			বিরতি;
		    ক্ষেত্র 'E':
			যদি (p[2] == ' ')
			    enc = p + 3;
			বিরতি;
		    ক্ষেত্র 'w':
			যদি (sscanf((অক্ষর *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			বিরতি;
		}
		যখন (*p != 0)
		    p++;
		p++;
	    }

	    যদি (!gotWindow)
		চালু;
	    str = serverConvert(enc, str, &tofree);
	    যদি ((r = ServerReplyFind(win, SROP_Add)) != ফাঁকা)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((অক্ষর *)winstr, "0x%x", (unsigned পূর্ণসংখ্যা)win);
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
	    যখন (*p != 0)
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
    স্থির পূর্ণসংখ্যা
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*মান,		// Characters  to append to property.
    পূর্ণসংখ্যা		দৈর্ঘ্য)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, মান, দৈর্ঘ্য);
    XSync(dpy, False);
    (শূন্য) XSetErrorHandler(old_handler);
    ফেরত got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    স্থির পূর্ণসংখ্যা
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    ফেরত 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    স্থির পূর্ণসংখ্যা
IsSerialName(char_u *str)
{
    পূর্ণসংখ্যা len = STRLEN(str);

    ফেরত (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
