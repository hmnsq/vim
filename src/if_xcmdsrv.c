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

#شامل "vim.h"
#شامل "version.h"

#اگر (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

تعریفنوع ساختار PendingCommand
{
    صحیح	    serial;	// Serial number expected in result.
    صحیح	    code;	// Result Code. 0 is OK
    char_u  *result;	// String result for command (malloc'ed).
			// NULL means command still pending.
    ساختار PendingCommand *nextPtr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

ایستا PendingCommand *pendingCommands = پوچ;
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

# define MAX_PROP_واژگان 100000

ساختار ServerReply
{
    Window  id;
    garray_T strings;
};
ایستا garray_T serverReply = { 0, 0, 0, 0, 0 };
شمارش ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

تعریفنوع صحیح (*EndCond)(تهی *);

ساختار x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    ساختار x_cmdqueue	*next;
    ساختار x_cmdqueue	*prev;
};

تعریفنوع ساختار x_cmdqueue x_queue_T;

// dummy node, header for circular queue
ایستا x_queue_T head = {پوچ, 0, پوچ, پوچ};

/*
 * Forward declarations for procedures defined later in this file:
 */

ایستا Window	LookupName(Display *dpy, char_u *name, صحیح delete, char_u **loose);
ایستا صحیح	SendInit(Display *dpy);
ایستا صحیح	DoRegisterName(Display *dpy, char_u *name);
ایستا تهی	DeleteAnyLingerer(Display *dpy, Window w);
ایستا صحیح	GetRegProp(Display *dpy, char_u **regPropp, long_u *رقم_Itemsp, صحیح domsg);
ایستا صحیح	WaitForPend(تهی *p);
ایستا صحیح	WindowValid(Display *dpy, Window w);
ایستا تهی	ServerWait(Display *dpy, Window w, EndCond endCond, تهی *endData, صحیح localLoop, صحیح seconds);
ایستا صحیح	AppendPropCarefully(Display *display, Window window, Atom property, char_u *مقدار, صحیح length);
ایستا صحیح	x_error_check(Display *dpy, XErrorEvent *error_event);
ایستا صحیح	IsSerialName(char_u *name);
ایستا تهی	save_in_queue(char_u *buf, long_u len);
ایستا تهی	server_parse_message(Display *dpy, char_u *propInfo, long_u رقم_اقلام);

// Private variables for the "server" functionality
ایستا Atom	registryProperty = None;
ایستا Atom	vimProperty = None;
ایستا صحیح	got_x_error = FALSE;

ایستا char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    صحیح
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    صحیح		i;
    صحیح		res;
    char_u	*p = پوچ;

    res = DoRegisterName(dpy, name);
    اگر (res >= 0)
	بازگشت OK;

    i = 1;
    انجامبده
    {
	اگر (res < -1 || i >= 1000)
	{
	    پیام_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    بازگشت FAIL;
	}
	اگر (p == پوچ)
	    p = alloc(STRLEN(name) + 10);
	اگر (p == پوچ)
	{
	    res = -10;
	    ادامه;
	}
	sprintf((نویسه *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    وقتی (res < 0)
	;
    vim_free(p);

    بازگشت OK;
}

    ایستا صحیح
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    اگر (commProperty == None)
    {
	اگر (SendInit(dpy) < 0)
	    بازگشت -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, پوچ);
    اگر (w != (Window)0)
    {
	Status		status;
	صحیح		dummyInt;
	unsigned صحیح	dummyUns;
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
	(تهی)XSetErrorHandler(old_handler);
	اگر (status != Success && w != commWindow)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    بازگشت -1;
	}
	(تهی)LookupName(dpy, name, /*delete=*/TRUE, پوچ);
    }
    sprintf((نویسه *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, ریشه_Window(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (تهی)XSetErrorHandler(old_handler);

    اگر (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, (صحیح)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	بازگشت 0;
    }
    بازگشت -2;
}

# اگر defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    تهی
serverChangeRegisteredWindow(
    Display	*dpy,		// Display to register with
    Window	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    commWindow = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    اگر (SendInit(dpy) < 0)
	بازگشت;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    اگر (serverName != پوچ)
    {
	// Reinsert name if it was already registered
	(تهی)LookupName(dpy, serverName, /*delete=*/TRUE, پوچ);
	sprintf((نویسه *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_LENGTH, serverName);
	XChangeProperty(dpy, ریشه_Window(dpy, 0), registryProperty, XA_STRING, 8,
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
    صحیح
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**result,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	as_عبارت,			// Interpret as keystrokes or expr ?
    صحیح		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    صحیح		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    صحیح		    length;
    صحیح		    res;
    ایستا صحیح	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = پوچ;

    اگر (result != پوچ)
	*result = پوچ;
    اگر (name == پوچ || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    اگر (commProperty == None && dpy != پوچ && SendInit(dpy) < 0)
	بازگشت -1;

# اگر defined(FEAT_EVAL)
    ch_log(پوچ, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    اگر (dpy == پوچ || (serverName != پوچ && STRICMP(name, serverName) == 0))
	بازگشت sendToLocalVim(cmd, as_عبارت, result);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    وقتی (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	اگر (w != None)
	{
	    اگر (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, پوچ);
		VIM_CLEAR(loosename);
		ادامه;
	    }
	}
	بشکن;
    }
    اگر (w == None)
    {
	اگر (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	بازگشت -1;
    }
    وگرنه اگر (loosename != پوچ)
	name = loosename;
    اگر (server != پوچ)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    length = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(length + 30);

    sprintf((نویسه *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_عبارت ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    اگر (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((نویسه *)property + length, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    length += STRLEN(property + length + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    vim_free(property);
    اگر (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	بازگشت -1;
    }

    اگر (!as_عبارت) // There is no answer for this - Keys are sent async
	بازگشت 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.code = 0;
    pending.result = پوچ;
    pending.nextPtr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    اگر (pendingCommands == &pending)
	pendingCommands = pending.nextPtr;
    وگرنه
    {
	PendingCommand *pcPtr;

	برای (pcPtr = pendingCommands; pcPtr != پوچ; pcPtr = pcPtr->nextPtr)
	    اگر (pcPtr->nextPtr == &pending)
	    {
		pcPtr->nextPtr = pending.nextPtr;
		بشکن;
	    }
    }

# اگر defined(FEAT_EVAL)
    ch_log(پوچ, "serverSendToVim() result: %s",
	    pending.result == پوچ ? "NULL" : (نویسه *)pending.result);
# endif
    اگر (result != پوچ)
	*result = pending.result;
    وگرنه
	vim_free(pending.result);

    بازگشت pending.code == 0 ? 0 : -1;
}

    ایستا صحیح
WaitForPend(تهی *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    بازگشت pending->result != پوچ;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    ایستا صحیح
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    صحیح		    رقم_Prop;
    صحیح		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &رقم_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    اگر (plist == پوچ || got_x_error)
	بازگشت FALSE;

    برای (i = 0; i < رقم_Prop; i++)
	اگر (plist[i] == vimProperty)
	{
	    XFree(plist);
	    بازگشت TRUE;
	}
    XFree(plist);
    بازگشت FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    ایستا تهی
ServerWait(
    Display	*dpy,
    Window	w,
    EndCond	endCond,
    تهی	*endData,
    صحیح		localLoop,
    صحیح		seconds)
{
    time_t	    start;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_عدد(dpy), &fds);
# وگرنه
    ساختار pollfd   fds;

    fds.fd = Connection_عدد(dpy);
    fds.events = POLLIN;
# endif

    time(&start);
    وقتی (TRUE)
    {
	وقتی (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	اگر (endCond(endData) != 0)
	    بشکن;
	اگر (!WindowValid(dpy, w))
	    بشکن;
	time(&now);
	اگر (seconds >= 0 && (now - start) >= seconds)
	    بشکن;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	اگر (localLoop)
	{
# ifdef HAVE_SELECT
	    ساختار timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    اگر (select(FD_SETSIZE, &fds, پوچ, پوچ, &tv) < 0)
		بشکن;
# وگرنه
	    اگر (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		بشکن;
# endif
	}
	وگرنه
	{
	    اگر (got_int)
		بشکن;
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
    long_u	رقم_اقلام;
    int_u	w;
    list_T	*list;

    اگر (registryProperty == None)
    {
	اگر (SendInit(dpy) < 0)
	    بازگشت پوچ;
    }

    list = list_alloc();
    اگر (list == پوچ)
	بازگشت پوچ;

    /*
     * Read the registry property.
     */
    اگر (GetRegProp(dpy, &regProp, &رقم_اقلام, TRUE) == FAIL)
	بازگشت پوچ;

    /*
     * Scan all of the names out of the property.
     */
    برای (p = regProp; (long_u)(p - regProp) < رقم_اقلام; p++)
    {
	entry = p;
	وقتی (*p != 0 && !SAFE_isspace(*p))
	    p++;
	اگر (*p != 0)
	{
	    w = None;
	    sscanf((نویسه *)entry, "%x", &w);
	    اگر (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    وقتی (*p != 0)
		p++;
	}
    }
    اگر (regProp != empty_prop)
	XFree(regProp);
    بازگشت list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    ایستا ساختار ServerReply *
ServerReplyFind(Window w, شمارش ServerReplyOp op)
{
    ساختار ServerReply *p;
    ساختار ServerReply e;
    صحیح		i;

    p = (ساختار ServerReply *) serverReply.ga_data;
    برای (i = 0; i < serverReply.ga_len; i++, p++)
	اگر (p->id == w)
	    بشکن;
    اگر (i >= serverReply.ga_len)
	p = پوچ;

    اگر (p == پوچ && op == SROP_Add)
    {
	اگر (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, اندازه(ساختار ServerReply), 1);
	اگر (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((ساختار ServerReply *) serverReply.ga_data)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, اندازه(e));
	    serverReply.ga_len++;
	}
    }
    وگرنه اگر (p != پوچ && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * اندازه(*p));
	serverReply.ga_len--;
    }

    بازگشت p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    unsigned  id = None;

    sscanf((نویسه *)str, "0x%x", &id);
    اگر (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    بازگشت (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    صحیح
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    صحیح		length;
    صحیح		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    اگر (commProperty == None)
    {
	اگر (SendInit(dpy) < 0)
	    بازگشت -2;
    }
    اگر (!WindowValid(dpy, win))
	بازگشت -1;

    length = STRLEN(p_enc) + STRLEN(str) + 14;
    اگر ((property = alloc(length + 30)) == پوچ)
	بازگشت -1;

    sprintf((نویسه *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (unsigned صحیح)commWindow);
    // Add length of what "%x" resulted in.
    length += STRLEN(property + length);
    res = AppendPropCarefully(dpy, win, commProperty, property, length + 1);
    vim_free(property);

    بازگشت res;
}

    ایستا صحیح
WaitForReply(تهی *p)
{
    Window  *w = (Window *) p;

    بازگشت ServerReplyFind(*w, SROP_Find) != پوچ;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    صحیح
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    صحیح		localLoop,
    صحیح		timeout)
{
    صحیح		len;
    char_u	*s;
    ساختار	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    اگر ((p = ServerReplyFind(win, SROP_Find)) != پوچ && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_data);
	len = STRLEN(*str) + 1;
	اگر (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_data;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	وگرنه
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	بازگشت 0;
    }
    بازگشت -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    صحیح
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    ساختار ServerReply *p;

    اگر ((p = ServerReplyFind(win, SROP_Find)) != پوچ && p->strings.ga_len > 0)
    {
	اگر (str != پوچ)
	    *str = p->strings.ga_data;
	بازگشت 1;
    }
    اگر (!WindowValid(dpy, win))
	بازگشت -1;
    بازگشت 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    ایستا صحیح
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    اگر (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    اگر (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    اگر (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    اگر (commWindow == None)
    {
	commWindow = XCreateSimpleWindow(dpy, XDefault_ریشه_Window(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_درون_داد(dpy, commWindow, PropertyChangeMask);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, commWindow);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, commWindow, vimProperty, XA_STRING,
		    8, PropModeReplace, (char_u *)VIM_VERSION_SHORT,
			(صحیح)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (تهی)XSetErrorHandler(old_handler);

    بازگشت got_x_error ? -1 : 0;
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
    ایستا Window
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    صحیح		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *entry;
    char_u	*p;
    long_u	رقم_اقلام;
    int_u	return_مقدار;

    /*
     * Read the registry property.
     */
    اگر (GetRegProp(dpy, &regProp, &رقم_اقلام, FALSE) == FAIL)
	بازگشت 0;

    /*
     * Scan the property for the desired name.
     */
    return_مقدار = (int_u)None;
    entry = پوچ;	// Not needed, but eliminates compiler warning.
    برای (p = regProp; (long_u)(p - regProp) < رقم_اقلام; )
    {
	entry = p;
	وقتی (*p != 0 && !SAFE_isspace(*p))
	    p++;
	اگر (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    sscanf((نویسه *)entry, "%x", &return_مقدار);
	    بشکن;
	}
	وقتی (*p != 0)
	    p++;
	p++;
    }

    اگر (loose != پوچ && return_مقدار == (int_u)None && !IsSerialName(name))
    {
	برای (p = regProp; (long_u)(p - regProp) < رقم_اقلام; )
	{
	    entry = p;
	    وقتی (*p != 0 && !SAFE_isspace(*p))
		p++;
	    اگر (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		اگر (sscanf((نویسه *)entry, "%x", &return_مقدار) == 1
			&& return_مقدار != (int_u)None)
		    *loose = vim_strsave(p + 1);
		بشکن;
	    }
	    وقتی (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    اگر (delete && return_مقدار != (int_u)None)
    {
	صحیح شمار;

	وقتی (*p != 0)
	    p++;
	p++;
	شمار = رقم_اقلام - (p - regProp);
	اگر (شمار > 0)
	    mch_memmove(entry, p, شمار);
	XChangeProperty(dpy, ریشه_Window(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(صحیح)(رقم_اقلام - (p - entry)));
	XSync(dpy, False);
    }

    اگر (regProp != empty_prop)
	XFree(regProp);
    بازگشت (Window)return_مقدار;
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
    ایستا تهی
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    Window	win)	// Window to remove
{
    char_u	*regProp, *entry = پوچ;
    char_u	*p;
    long_u	رقم_اقلام;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    اگر (GetRegProp(dpy, &regProp, &رقم_اقلام, FALSE) == FAIL)
	بازگشت;

    // Scan the property for the window id.
    برای (p = regProp; (long_u)(p - regProp) < رقم_اقلام; )
    {
	اگر (*p != 0)
	{
	    sscanf((نویسه *)p, "%x", &wwin);
	    اگر ((Window)wwin == win)
	    {
		صحیح lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		وقتی (*p != 0)
		    p++;
		p++;
		lastHalf = رقم_اقلام - (p - regProp);
		اگر (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		رقم_اقلام = (entry - regProp) + lastHalf;
		p = entry;
		ادامه;
	    }
	}
	وقتی (*p != 0)
	    p++;
	p++;
    }

    اگر (entry != پوچ)
    {
	XChangeProperty(dpy, ریشه_Window(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(صحیح)(p - regProp));
	XSync(dpy, False);
    }

    اگر (regProp != empty_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    ایستا صحیح
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*رقم_Itemsp,
    صحیح		domsg)		// When TRUE give error message.
{
    صحیح		result, actualFormat;
    long_u	bytesAfter;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = پوچ;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    result = XGetWindowProperty(dpy, ریشه_Window(dpy, 0), registryProperty, 0L,
				(long)MAX_PROP_واژگان, False,
				XA_STRING, &actualType,
				&actualFormat, رقم_Itemsp, &bytesAfter,
				regPropp);

    XSync(dpy, FALSE);
    (تهی)XSetErrorHandler(old_handler);
    اگر (got_x_error)
	بازگشت FAIL;

    اگر (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*رقم_Itemsp = 0;
	*regPropp = empty_prop;
	بازگشت OK;
    }

    // If the property is improperly formed, then delete it.
    اگر (result != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	اگر (*regPropp != پوچ)
	    XFree(*regPropp);
	XDeleteProperty(dpy, ریشه_Window(dpy, 0), registryProperty);
	اگر (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	بازگشت FAIL;
    }
    بازگشت OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    تهی
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    صحیح		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    صحیح		result, actualFormat;
    long_u	رقم_اقلام, bytesAfter;
    Atom	actualType;

    اگر (eventPtr != پوچ)
    {
	اگر (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_مقدار)
	    بازگشت;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = پوچ;
    result = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(long)MAX_PROP_واژگان, True,
				XA_STRING, &actualType,
				&actualFormat, &رقم_اقلام, &bytesAfter,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    اگر (result != Success || actualType != XA_STRING || actualFormat != 8)
    {
	اگر (propInfo != پوچ)
	    XFree(propInfo);
	بازگشت;
    }
    اگر (immediate)
	server_parse_message(dpy, propInfo, رقم_اقلام);
    وگرنه
	save_in_queue(propInfo, رقم_اقلام);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    ایستا تهی
save_in_queue(char_u *propInfo, long_u len)
{
    x_queue_T *گره;

    گره = ALLOC_ONE(x_queue_T);
    اگر (گره == پوچ)
	بازگشت;	    // out of memory
    گره->propInfo = propInfo;
    گره->len = len;

    اگر (head.next == پوچ)   // initialize circular queue
    {
	head.next = &head;
	head.prev = &head;
    }

    // insert node at tail of queue
    گره->next = &head;
    گره->prev = head.prev;
    head.prev->next = گره;
    head.prev = گره;
}

/*
 * Parses queued clientserver messages.
 */
    تهی
server_parse_messages(تهی)
{
    x_queue_T	*گره;

    اگر (!X_DISPLAY)
	بازگشت; // cannot happen?
    وقتی (head.next != پوچ && head.next != &head)
    {
	گره = head.next;
	head.next = گره->next;
	گره->next->prev = گره->prev;
	server_parse_message(X_DISPLAY, گره->propInfo, گره->len);
	vim_free(گره);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    صحیح
server_waiting(تهی)
{
    بازگشت head.next != پوچ && head.next != &head;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    ایستا تهی
server_parse_message(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	رقم_اقلام)  // The size of propInfo in bytes.
{
    char_u	*p;
    صحیح		code;
    char_u	*tofree;

# اگر defined(FEAT_EVAL)
    ch_log(پوچ, "server_parse_message() numItems: %ld", رقم_اقلام);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    برای (p = propInfo; (long_u)(p - propInfo) < رقم_اقلام; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	اگر (*p == 0)
	{
	    p++;
	    ادامه;
	}

	اگر ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    Window	resWindow;
	    char_u	*name, *script, *serial, *end;
	    Bool	as_کلیدها = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = پوچ;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = پوچ;
	    enc = پوچ;
	    وقتی ((long_u)(p - propInfo) < رقم_اقلام && *p == '-')
	    {
# اگر defined(FEAT_EVAL)
		ch_log(پوچ, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		انتخاب (p[1])
		{
		    حالت 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			وقتی (vim_isxdigit(*end))
			{
			    resWindow = 16 * resWindow + (long_u)hex2nr(*end);
			    ++end;
			}
			اگر (end == p + 2 || *end != ' ')
			    resWindow = None;
			وگرنه
			{
			    p = serial = end + 1;
			    clientWindow = resWindow; // Remember in global
			}
			بشکن;
		    حالت 'n':
			اگر (p[2] == ' ')
			    name = p + 3;
			بشکن;
		    حالت 's':
			اگر (p[2] == ' ')
			    script = p + 3;
			بشکن;
		    حالت 'E':
			اگر (p[2] == ' ')
			    enc = p + 3;
			بشکن;
		}
		وقتی (*p != 0)
		    p++;
		p++;
	    }

	    اگر (script == پوچ || name == پوچ)
		ادامه;

	    اگر (serverName != پوچ && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		اگر (as_کلیدها)
		    server_to_درون_داد_buf(script);
		وگرنه
		{
		    char_u      *res;

		    res = eval_client_عبارت_to_string(script);
		    اگر (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(تهی)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(reply.ga_data, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			اگر (res != پوچ)
			    ga_concat(&reply, res);
			وگرنه
			{
			    ga_concat(&reply,
				   (char_u *)_(e_invalid_expression_received));
			    ga_append(&reply, 0);
			    GA_CONCAT_LITERAL(&reply, "-c 1");
			}
			ga_append(&reply, NUL);
			(تهی)AppendPropCarefully(dpy, resWindow, commProperty,
						 reply.ga_data, reply.ga_len);
			ga_clear(&reply);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	وگرنه اگر (*p == 'r' && p[1] == 0)
	{
	    صحیح		    serial, gotSerial;
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
	    enc = پوچ;
	    وقتی ((long_u)(p - propInfo) < رقم_اقلام && *p == '-')
	    {
		انتخاب (p[1])
		{
		    حالت 'r':
			اگر (p[2] == ' ')
			    res = p + 3;
			بشکن;
		    حالت 'E':
			اگر (p[2] == ' ')
			    enc = p + 3;
			بشکن;
		    حالت 's':
			اگر (sscanf((نویسه *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			بشکن;
		    حالت 'c':
			اگر (sscanf((نویسه *)p + 2, " %d", &code) != 1)
			    code = 0;
			بشکن;
		}
		وقتی (*p != 0)
		    p++;
		p++;
	    }

	    اگر (!gotSerial)
		ادامه;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    برای (pcPtr = pendingCommands; pcPtr != پوچ; pcPtr = pcPtr->nextPtr)
	    {
		اگر (serial != pcPtr->serial || pcPtr->result != پوچ)
		    ادامه;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		اگر (tofree == پوچ)
		    res = vim_strsave(res);
		pcPtr->result = res;
		بشکن;
	    }
	}
	وگرنه اگر (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    unsigned صحیح u;
	    صحیح		gotWindow;
	    char_u	*str;
	    ساختار	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = پوچ;
	    وقتی ((long_u)(p - propInfo) < رقم_اقلام && *p == '-')
	    {
		انتخاب (p[1])
		{
		    حالت 'n':
			اگر (p[2] == ' ')
			    str = p + 3;
			بشکن;
		    حالت 'E':
			اگر (p[2] == ' ')
			    enc = p + 3;
			بشکن;
		    حالت 'w':
			اگر (sscanf((نویسه *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			بشکن;
		}
		وقتی (*p != 0)
		    p++;
		p++;
	    }

	    اگر (!gotWindow)
		ادامه;
	    str = serverConvert(enc, str, &tofree);
	    اگر ((r = ServerReplyFind(win, SROP_Add)) != پوچ)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((نویسه *)winstr, "0x%x", (unsigned صحیح)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	وگرنه
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    وقتی (*p != 0)
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
    ایستا صحیح
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*مقدار,		// Characters  to append to property.
    صحیح		length)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, مقدار, length);
    XSync(dpy, False);
    (تهی) XSetErrorHandler(old_handler);
    بازگشت got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    ایستا صحیح
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    بازگشت 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    ایستا صحیح
IsSerialName(char_u *str)
{
    صحیح len = STRLEN(str);

    بازگشت (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
