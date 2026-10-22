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

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

#જો (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

પ્રકારવ્યાખ્યા સંરચના PendingCommand
{
    પૂર્ણાંક	    serial;	// Serial number expected in result.
    પૂર્ણાંક	    code;	// Result Code. 0 is OK
    char_u  *result;	// String result for command (malloc'ed).
			// NULL means command still pending.
    સંરચના PendingCommand *nextPtr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

સ્થિર PendingCommand *pendingCommands = ખાલી;
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

# define MAX_PROP_શબ્દો 100000

સંરચના ServerReply
{
    Window  id;
    garray_T strings;
};
સ્થિર garray_T serverReply = { 0, 0, 0, 0, 0 };
ગણના ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

પ્રકારવ્યાખ્યા પૂર્ણાંક (*EndCond)(શૂન્ય *);

સંરચના x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    સંરચના x_cmdqueue	*next;
    સંરચના x_cmdqueue	*prev;
};

પ્રકારવ્યાખ્યા સંરચના x_cmdqueue x_queue_T;

// dummy node, header for circular queue
સ્થિર x_queue_T head = {ખાલી, 0, ખાલી, ખાલી};

/*
 * Forward declarations for procedures defined later in this file:
 */

સ્થિર Window	LookupName(Display *dpy, char_u *name, પૂર્ણાંક delete, char_u **loose);
સ્થિર પૂર્ણાંક	SendInit(Display *dpy);
સ્થિર પૂર્ણાંક	DoRegisterName(Display *dpy, char_u *name);
સ્થિર શૂન્ય	DeleteAnyLingerer(Display *dpy, Window w);
સ્થિર પૂર્ણાંક	GetRegProp(Display *dpy, char_u **regPropp, long_u *અંક_Itemsp, પૂર્ણાંક domsg);
સ્થિર પૂર્ણાંક	WaitForPend(શૂન્ય *p);
સ્થિર પૂર્ણાંક	WindowValid(Display *dpy, Window w);
સ્થિર શૂન્ય	ServerWait(Display *dpy, Window w, EndCond endCond, શૂન્ય *endData, પૂર્ણાંક localLoop, પૂર્ણાંક seconds);
સ્થિર પૂર્ણાંક	AppendPropCarefully(Display *display, Window window, Atom property, char_u *કિંમત, પૂર્ણાંક length);
સ્થિર પૂર્ણાંક	x_error_check(Display *dpy, XErrorEvent *error_event);
સ્થિર પૂર્ણાંક	IsSerialName(char_u *name);
સ્થિર શૂન્ય	save_in_queue(char_u *buf, long_u len);
સ્થિર શૂન્ય	server_parse_message(Display *dpy, char_u *propInfo, long_u અંક_વસ્તુઓ);

// Private variables for the "server" functionality
સ્થિર Atom	registryProperty = None;
સ્થિર Atom	vimProperty = None;
સ્થિર પૂર્ણાંક	got_x_error = FALSE;

સ્થિર char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    પૂર્ણાંક
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		res;
    char_u	*p = ખાલી;

    res = DoRegisterName(dpy, name);
    જો (res >= 0)
	પાછા OK;

    i = 1;
    કરો
    {
	જો (res < -1 || i >= 1000)
	{
	    સંદેશ_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    પાછા FAIL;
	}
	જો (p == ખાલી)
	    p = alloc(STRLEN(name) + 10);
	જો (p == ખાલી)
	{
	    res = -10;
	    ચાલુ;
	}
	sprintf((અક્ષર *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    જ્યારે (res < 0)
	;
    vim_free(p);

    પાછા OK;
}

    સ્થિર પૂર્ણાંક
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    જો (commProperty == None)
    {
	જો (SendInit(dpy) < 0)
	    પાછા -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, ખાલી);
    જો (w != (Window)0)
    {
	Status		status;
	પૂર્ણાંક		dummyInt;
	unsigned પૂર્ણાંક	dummyUns;
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
	(શૂન્ય)XSetErrorHandler(old_handler);
	જો (status != Success && w != commWindow)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    પાછા -1;
	}
	(શૂન્ય)LookupName(dpy, name, /*delete=*/TRUE, ખાલી);
    }
    sprintf((અક્ષર *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, મૂળ_Window(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (શૂન્ય)XSetErrorHandler(old_handler);

    જો (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, (પૂર્ણાંક)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	પાછા 0;
    }
    પાછા -2;
}

# જો defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    શૂન્ય
serverChangeRegisteredWindow(
    Display	*dpy,		// Display to register with
    Window	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    commWindow = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    જો (SendInit(dpy) < 0)
	પાછા;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    જો (serverName != ખાલી)
    {
	// Reinsert name if it was already registered
	(શૂન્ય)LookupName(dpy, serverName, /*delete=*/TRUE, ખાલી);
	sprintf((અક્ષર *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_LENGTH, serverName);
	XChangeProperty(dpy, મૂળ_Window(dpy, 0), registryProperty, XA_STRING, 8,
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
    પૂર્ણાંક
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**result,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	as_પદાવલિ,			// Interpret as keystrokes or expr ?
    પૂર્ણાંક		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    પૂર્ણાંક		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    પૂર્ણાંક		    length;
    પૂર્ણાંક		    res;
    સ્થિર પૂર્ણાંક	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = ખાલી;

    જો (result != ખાલી)
	*result = ખાલી;
    જો (name == ખાલી || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    જો (commProperty == None && dpy != ખાલી && SendInit(dpy) < 0)
	પાછા -1;

# જો defined(FEAT_EVAL)
    ch_log(ખાલી, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    જો (dpy == ખાલી || (serverName != ખાલી && STRICMP(name, serverName) == 0))
	પાછા sendToLocalVim(cmd, as_પદાવલિ, result);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    જ્યારે (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	જો (w != None)
	{
	    જો (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, ખાલી);
		VIM_CLEAR(loosename);
		ચાલુ;
	    }
	}
	તોડો;
    }
    જો (w == None)
    {
	જો (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	પાછા -1;
    }
    નહીંતર જો (loosename != ખાલી)
	name = loosename;
    જો (server != ખાલી)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    length = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(length + 30);

    sprintf((અક્ષર *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_પદાવલિ ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    જો (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((અક્ષર *)property + length, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    length += STRLEN(property + length + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    vim_free(property);
    જો (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	પાછા -1;
    }

    જો (!as_પદાવલિ) // There is no answer for this - Keys are sent async
	પાછા 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.code = 0;
    pending.result = ખાલી;
    pending.nextPtr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    જો (pendingCommands == &pending)
	pendingCommands = pending.nextPtr;
    નહીંતર
    {
	PendingCommand *pcPtr;

	માટે (pcPtr = pendingCommands; pcPtr != ખાલી; pcPtr = pcPtr->nextPtr)
	    જો (pcPtr->nextPtr == &pending)
	    {
		pcPtr->nextPtr = pending.nextPtr;
		તોડો;
	    }
    }

# જો defined(FEAT_EVAL)
    ch_log(ખાલી, "serverSendToVim() result: %s",
	    pending.result == ખાલી ? "NULL" : (અક્ષર *)pending.result);
# endif
    જો (result != ખાલી)
	*result = pending.result;
    નહીંતર
	vim_free(pending.result);

    પાછા pending.code == 0 ? 0 : -1;
}

    સ્થિર પૂર્ણાંક
WaitForPend(શૂન્ય *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    પાછા pending->result != ખાલી;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    સ્થિર પૂર્ણાંક
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    પૂર્ણાંક		    અંક_Prop;
    પૂર્ણાંક		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &અંક_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    જો (plist == ખાલી || got_x_error)
	પાછા FALSE;

    માટે (i = 0; i < અંક_Prop; i++)
	જો (plist[i] == vimProperty)
	{
	    XFree(plist);
	    પાછા TRUE;
	}
    XFree(plist);
    પાછા FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    સ્થિર શૂન્ય
ServerWait(
    Display	*dpy,
    Window	w,
    EndCond	endCond,
    શૂન્ય	*endData,
    પૂર્ણાંક		localLoop,
    પૂર્ણાંક		seconds)
{
    time_t	    start;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_સંખ્યા(dpy), &fds);
# નહીંતર
    સંરચના pollfd   fds;

    fds.fd = Connection_સંખ્યા(dpy);
    fds.events = POLLIN;
# endif

    time(&start);
    જ્યારે (TRUE)
    {
	જ્યારે (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	જો (endCond(endData) != 0)
	    તોડો;
	જો (!WindowValid(dpy, w))
	    તોડો;
	time(&now);
	જો (seconds >= 0 && (now - start) >= seconds)
	    તોડો;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	જો (localLoop)
	{
# ifdef HAVE_SELECT
	    સંરચના timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    જો (select(FD_SETSIZE, &fds, ખાલી, ખાલી, &tv) < 0)
		તોડો;
# નહીંતર
	    જો (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		તોડો;
# endif
	}
	નહીંતર
	{
	    જો (got_int)
		તોડો;
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
    long_u	અંક_વસ્તુઓ;
    int_u	w;
    list_T	*list;

    જો (registryProperty == None)
    {
	જો (SendInit(dpy) < 0)
	    પાછા ખાલી;
    }

    list = list_alloc();
    જો (list == ખાલી)
	પાછા ખાલી;

    /*
     * Read the registry property.
     */
    જો (GetRegProp(dpy, &regProp, &અંક_વસ્તુઓ, TRUE) == FAIL)
	પાછા ખાલી;

    /*
     * Scan all of the names out of the property.
     */
    માટે (p = regProp; (long_u)(p - regProp) < અંક_વસ્તુઓ; p++)
    {
	entry = p;
	જ્યારે (*p != 0 && !SAFE_isspace(*p))
	    p++;
	જો (*p != 0)
	{
	    w = None;
	    sscanf((અક્ષર *)entry, "%x", &w);
	    જો (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    જ્યારે (*p != 0)
		p++;
	}
    }
    જો (regProp != empty_prop)
	XFree(regProp);
    પાછા list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    સ્થિર સંરચના ServerReply *
ServerReplyFind(Window w, ગણના ServerReplyOp op)
{
    સંરચના ServerReply *p;
    સંરચના ServerReply e;
    પૂર્ણાંક		i;

    p = (સંરચના ServerReply *) serverReply.ga_data;
    માટે (i = 0; i < serverReply.ga_len; i++, p++)
	જો (p->id == w)
	    તોડો;
    જો (i >= serverReply.ga_len)
	p = ખાલી;

    જો (p == ખાલી && op == SROP_Add)
    {
	જો (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, કદ(સંરચના ServerReply), 1);
	જો (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((સંરચના ServerReply *) serverReply.ga_data)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, કદ(e));
	    serverReply.ga_len++;
	}
    }
    નહીંતર જો (p != ખાલી && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * કદ(*p));
	serverReply.ga_len--;
    }

    પાછા p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    unsigned  id = None;

    sscanf((અક્ષર *)str, "0x%x", &id);
    જો (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    પાછા (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    પૂર્ણાંક
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    પૂર્ણાંક		length;
    પૂર્ણાંક		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    જો (commProperty == None)
    {
	જો (SendInit(dpy) < 0)
	    પાછા -2;
    }
    જો (!WindowValid(dpy, win))
	પાછા -1;

    length = STRLEN(p_enc) + STRLEN(str) + 14;
    જો ((property = alloc(length + 30)) == ખાલી)
	પાછા -1;

    sprintf((અક્ષર *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (unsigned પૂર્ણાંક)commWindow);
    // Add length of what "%x" resulted in.
    length += STRLEN(property + length);
    res = AppendPropCarefully(dpy, win, commProperty, property, length + 1);
    vim_free(property);

    પાછા res;
}

    સ્થિર પૂર્ણાંક
WaitForReply(શૂન્ય *p)
{
    Window  *w = (Window *) p;

    પાછા ServerReplyFind(*w, SROP_Find) != ખાલી;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    પૂર્ણાંક
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    પૂર્ણાંક		localLoop,
    પૂર્ણાંક		timeout)
{
    પૂર્ણાંક		len;
    char_u	*s;
    સંરચના	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    જો ((p = ServerReplyFind(win, SROP_Find)) != ખાલી && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_data);
	len = STRLEN(*str) + 1;
	જો (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_data;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	નહીંતર
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	પાછા 0;
    }
    પાછા -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    પૂર્ણાંક
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    સંરચના ServerReply *p;

    જો ((p = ServerReplyFind(win, SROP_Find)) != ખાલી && p->strings.ga_len > 0)
    {
	જો (str != ખાલી)
	    *str = p->strings.ga_data;
	પાછા 1;
    }
    જો (!WindowValid(dpy, win))
	પાછા -1;
    પાછા 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    સ્થિર પૂર્ણાંક
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    જો (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    જો (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    જો (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    જો (commWindow == None)
    {
	commWindow = XCreateSimpleWindow(dpy, XDefault_મૂળ_Window(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_આગત(dpy, commWindow, PropertyChangeMask);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, commWindow);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, commWindow, vimProperty, XA_STRING,
		    8, PropModeReplace, (char_u *)VIM_VERSION_SHORT,
			(પૂર્ણાંક)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (શૂન્ય)XSetErrorHandler(old_handler);

    પાછા got_x_error ? -1 : 0;
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
    સ્થિર Window
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    પૂર્ણાંક		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *entry;
    char_u	*p;
    long_u	અંક_વસ્તુઓ;
    int_u	return_કિંમત;

    /*
     * Read the registry property.
     */
    જો (GetRegProp(dpy, &regProp, &અંક_વસ્તુઓ, FALSE) == FAIL)
	પાછા 0;

    /*
     * Scan the property for the desired name.
     */
    return_કિંમત = (int_u)None;
    entry = ખાલી;	// Not needed, but eliminates compiler warning.
    માટે (p = regProp; (long_u)(p - regProp) < અંક_વસ્તુઓ; )
    {
	entry = p;
	જ્યારે (*p != 0 && !SAFE_isspace(*p))
	    p++;
	જો (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    sscanf((અક્ષર *)entry, "%x", &return_કિંમત);
	    તોડો;
	}
	જ્યારે (*p != 0)
	    p++;
	p++;
    }

    જો (loose != ખાલી && return_કિંમત == (int_u)None && !IsSerialName(name))
    {
	માટે (p = regProp; (long_u)(p - regProp) < અંક_વસ્તુઓ; )
	{
	    entry = p;
	    જ્યારે (*p != 0 && !SAFE_isspace(*p))
		p++;
	    જો (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		જો (sscanf((અક્ષર *)entry, "%x", &return_કિંમત) == 1
			&& return_કિંમત != (int_u)None)
		    *loose = vim_strsave(p + 1);
		તોડો;
	    }
	    જ્યારે (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    જો (delete && return_કિંમત != (int_u)None)
    {
	પૂર્ણાંક ગણતરી;

	જ્યારે (*p != 0)
	    p++;
	p++;
	ગણતરી = અંક_વસ્તુઓ - (p - regProp);
	જો (ગણતરી > 0)
	    mch_memmove(entry, p, ગણતરી);
	XChangeProperty(dpy, મૂળ_Window(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(પૂર્ણાંક)(અંક_વસ્તુઓ - (p - entry)));
	XSync(dpy, False);
    }

    જો (regProp != empty_prop)
	XFree(regProp);
    પાછા (Window)return_કિંમત;
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
    સ્થિર શૂન્ય
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    Window	win)	// Window to remove
{
    char_u	*regProp, *entry = ખાલી;
    char_u	*p;
    long_u	અંક_વસ્તુઓ;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    જો (GetRegProp(dpy, &regProp, &અંક_વસ્તુઓ, FALSE) == FAIL)
	પાછા;

    // Scan the property for the window id.
    માટે (p = regProp; (long_u)(p - regProp) < અંક_વસ્તુઓ; )
    {
	જો (*p != 0)
	{
	    sscanf((અક્ષર *)p, "%x", &wwin);
	    જો ((Window)wwin == win)
	    {
		પૂર્ણાંક lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		જ્યારે (*p != 0)
		    p++;
		p++;
		lastHalf = અંક_વસ્તુઓ - (p - regProp);
		જો (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		અંક_વસ્તુઓ = (entry - regProp) + lastHalf;
		p = entry;
		ચાલુ;
	    }
	}
	જ્યારે (*p != 0)
	    p++;
	p++;
    }

    જો (entry != ખાલી)
    {
	XChangeProperty(dpy, મૂળ_Window(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(પૂર્ણાંક)(p - regProp));
	XSync(dpy, False);
    }

    જો (regProp != empty_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    સ્થિર પૂર્ણાંક
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*અંક_Itemsp,
    પૂર્ણાંક		domsg)		// When TRUE give error message.
{
    પૂર્ણાંક		result, actualFormat;
    long_u	bytesAfter;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = ખાલી;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    result = XGetWindowProperty(dpy, મૂળ_Window(dpy, 0), registryProperty, 0L,
				(long)MAX_PROP_શબ્દો, False,
				XA_STRING, &actualType,
				&actualFormat, અંક_Itemsp, &bytesAfter,
				regPropp);

    XSync(dpy, FALSE);
    (શૂન્ય)XSetErrorHandler(old_handler);
    જો (got_x_error)
	પાછા FAIL;

    જો (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*અંક_Itemsp = 0;
	*regPropp = empty_prop;
	પાછા OK;
    }

    // If the property is improperly formed, then delete it.
    જો (result != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	જો (*regPropp != ખાલી)
	    XFree(*regPropp);
	XDeleteProperty(dpy, મૂળ_Window(dpy, 0), registryProperty);
	જો (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	પાછા FAIL;
    }
    પાછા OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    શૂન્ય
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    પૂર્ણાંક		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    પૂર્ણાંક		result, actualFormat;
    long_u	અંક_વસ્તુઓ, bytesAfter;
    Atom	actualType;

    જો (eventPtr != ખાલી)
    {
	જો (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_કિંમત)
	    પાછા;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = ખાલી;
    result = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(long)MAX_PROP_શબ્દો, True,
				XA_STRING, &actualType,
				&actualFormat, &અંક_વસ્તુઓ, &bytesAfter,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    જો (result != Success || actualType != XA_STRING || actualFormat != 8)
    {
	જો (propInfo != ખાલી)
	    XFree(propInfo);
	પાછા;
    }
    જો (immediate)
	server_parse_message(dpy, propInfo, અંક_વસ્તુઓ);
    નહીંતર
	save_in_queue(propInfo, અંક_વસ્તુઓ);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    સ્થિર શૂન્ય
save_in_queue(char_u *propInfo, long_u len)
{
    x_queue_T *ગાંઠ;

    ગાંઠ = ALLOC_ONE(x_queue_T);
    જો (ગાંઠ == ખાલી)
	પાછા;	    // out of memory
    ગાંઠ->propInfo = propInfo;
    ગાંઠ->len = len;

    જો (head.next == ખાલી)   // initialize circular queue
    {
	head.next = &head;
	head.prev = &head;
    }

    // insert node at tail of queue
    ગાંઠ->next = &head;
    ગાંઠ->prev = head.prev;
    head.prev->next = ગાંઠ;
    head.prev = ગાંઠ;
}

/*
 * Parses queued clientserver messages.
 */
    શૂન્ય
server_parse_messages(શૂન્ય)
{
    x_queue_T	*ગાંઠ;

    જો (!X_DISPLAY)
	પાછા; // cannot happen?
    જ્યારે (head.next != ખાલી && head.next != &head)
    {
	ગાંઠ = head.next;
	head.next = ગાંઠ->next;
	ગાંઠ->next->prev = ગાંઠ->prev;
	server_parse_message(X_DISPLAY, ગાંઠ->propInfo, ગાંઠ->len);
	vim_free(ગાંઠ);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    પૂર્ણાંક
server_waiting(શૂન્ય)
{
    પાછા head.next != ખાલી && head.next != &head;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    સ્થિર શૂન્ય
server_parse_message(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	અંક_વસ્તુઓ)  // The size of propInfo in bytes.
{
    char_u	*p;
    પૂર્ણાંક		code;
    char_u	*tofree;

# જો defined(FEAT_EVAL)
    ch_log(ખાલી, "server_parse_message() numItems: %ld", અંક_વસ્તુઓ);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    માટે (p = propInfo; (long_u)(p - propInfo) < અંક_વસ્તુઓ; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	જો (*p == 0)
	{
	    p++;
	    ચાલુ;
	}

	જો ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    Window	resWindow;
	    char_u	*name, *script, *serial, *end;
	    Bool	as_ચાવીઓ = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = ખાલી;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = ખાલી;
	    enc = ખાલી;
	    જ્યારે ((long_u)(p - propInfo) < અંક_વસ્તુઓ && *p == '-')
	    {
# જો defined(FEAT_EVAL)
		ch_log(ખાલી, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		પસંદગી (p[1])
		{
		    સ્થિતિ 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			જ્યારે (vim_isxdigit(*end))
			{
			    resWindow = 16 * resWindow + (long_u)hex2nr(*end);
			    ++end;
			}
			જો (end == p + 2 || *end != ' ')
			    resWindow = None;
			નહીંતર
			{
			    p = serial = end + 1;
			    clientWindow = resWindow; // Remember in global
			}
			તોડો;
		    સ્થિતિ 'n':
			જો (p[2] == ' ')
			    name = p + 3;
			તોડો;
		    સ્થિતિ 's':
			જો (p[2] == ' ')
			    script = p + 3;
			તોડો;
		    સ્થિતિ 'E':
			જો (p[2] == ' ')
			    enc = p + 3;
			તોડો;
		}
		જ્યારે (*p != 0)
		    p++;
		p++;
	    }

	    જો (script == ખાલી || name == ખાલી)
		ચાલુ;

	    જો (serverName != ખાલી && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		જો (as_ચાવીઓ)
		    server_to_આગત_buf(script);
		નહીંતર
		{
		    char_u      *res;

		    res = eval_client_પદાવલિ_to_string(script);
		    જો (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(શૂન્ય)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(reply.ga_data, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			જો (res != ખાલી)
			    ga_concat(&reply, res);
			નહીંતર
			{
			    ga_concat(&reply,
				   (char_u *)_(e_invalid_expression_received));
			    ga_append(&reply, 0);
			    GA_CONCAT_LITERAL(&reply, "-c 1");
			}
			ga_append(&reply, NUL);
			(શૂન્ય)AppendPropCarefully(dpy, resWindow, commProperty,
						 reply.ga_data, reply.ga_len);
			ga_clear(&reply);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	નહીંતર જો (*p == 'r' && p[1] == 0)
	{
	    પૂર્ણાંક		    serial, gotSerial;
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
	    enc = ખાલી;
	    જ્યારે ((long_u)(p - propInfo) < અંક_વસ્તુઓ && *p == '-')
	    {
		પસંદગી (p[1])
		{
		    સ્થિતિ 'r':
			જો (p[2] == ' ')
			    res = p + 3;
			તોડો;
		    સ્થિતિ 'E':
			જો (p[2] == ' ')
			    enc = p + 3;
			તોડો;
		    સ્થિતિ 's':
			જો (sscanf((અક્ષર *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			તોડો;
		    સ્થિતિ 'c':
			જો (sscanf((અક્ષર *)p + 2, " %d", &code) != 1)
			    code = 0;
			તોડો;
		}
		જ્યારે (*p != 0)
		    p++;
		p++;
	    }

	    જો (!gotSerial)
		ચાલુ;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    માટે (pcPtr = pendingCommands; pcPtr != ખાલી; pcPtr = pcPtr->nextPtr)
	    {
		જો (serial != pcPtr->serial || pcPtr->result != ખાલી)
		    ચાલુ;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		જો (tofree == ખાલી)
		    res = vim_strsave(res);
		pcPtr->result = res;
		તોડો;
	    }
	}
	નહીંતર જો (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    unsigned પૂર્ણાંક u;
	    પૂર્ણાંક		gotWindow;
	    char_u	*str;
	    સંરચના	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = ખાલી;
	    જ્યારે ((long_u)(p - propInfo) < અંક_વસ્તુઓ && *p == '-')
	    {
		પસંદગી (p[1])
		{
		    સ્થિતિ 'n':
			જો (p[2] == ' ')
			    str = p + 3;
			તોડો;
		    સ્થિતિ 'E':
			જો (p[2] == ' ')
			    enc = p + 3;
			તોડો;
		    સ્થિતિ 'w':
			જો (sscanf((અક્ષર *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			તોડો;
		}
		જ્યારે (*p != 0)
		    p++;
		p++;
	    }

	    જો (!gotWindow)
		ચાલુ;
	    str = serverConvert(enc, str, &tofree);
	    જો ((r = ServerReplyFind(win, SROP_Add)) != ખાલી)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((અક્ષર *)winstr, "0x%x", (unsigned પૂર્ણાંક)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	નહીંતર
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    જ્યારે (*p != 0)
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
    સ્થિર પૂર્ણાંક
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*કિંમત,		// Characters  to append to property.
    પૂર્ણાંક		length)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, કિંમત, length);
    XSync(dpy, False);
    (શૂન્ય) XSetErrorHandler(old_handler);
    પાછા got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    સ્થિર પૂર્ણાંક
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    પાછા 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    સ્થિર પૂર્ણાંક
IsSerialName(char_u *str)
{
    પૂર્ણાંક len = STRLEN(str);

    પાછા (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
