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

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

#जर (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

प्रकारव्याख्या संरचना PendingCommand
{
    पूर्णांक	    serial;	// Serial number expected in result.
    पूर्णांक	    code;	// Result Code. 0 is OK
    char_u  *result;	// String result for command (malloc'ed).
			// NULL means command still pending.
    संरचना PendingCommand *nextPtr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

स्थिर PendingCommand *pendingCommands = रिक्त;
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

# define MAX_PROP_शब्द 100000

संरचना ServerReply
{
    Window  id;
    garray_T strings;
};
स्थिर garray_T serverReply = { 0, 0, 0, 0, 0 };
गणना ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

प्रकारव्याख्या पूर्णांक (*EndCond)(शून्य *);

संरचना x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    संरचना x_cmdqueue	*next;
    संरचना x_cmdqueue	*prev;
};

प्रकारव्याख्या संरचना x_cmdqueue x_queue_T;

// dummy node, header for circular queue
स्थिर x_queue_T head = {रिक्त, 0, रिक्त, रिक्त};

/*
 * Forward declarations for procedures defined later in this file:
 */

स्थिर Window	LookupName(Display *dpy, char_u *name, पूर्णांक delete, char_u **loose);
स्थिर पूर्णांक	SendInit(Display *dpy);
स्थिर पूर्णांक	DoRegisterName(Display *dpy, char_u *name);
स्थिर शून्य	DeleteAnyLingerer(Display *dpy, Window w);
स्थिर पूर्णांक	GetRegProp(Display *dpy, char_u **regPropp, long_u *अंक_Itemsp, पूर्णांक domsg);
स्थिर पूर्णांक	WaitForPend(शून्य *p);
स्थिर पूर्णांक	WindowValid(Display *dpy, Window w);
स्थिर शून्य	ServerWait(Display *dpy, Window w, EndCond endCond, शून्य *endData, पूर्णांक localLoop, पूर्णांक seconds);
स्थिर पूर्णांक	AppendPropCarefully(Display *display, Window window, Atom property, char_u *मूल्य, पूर्णांक length);
स्थिर पूर्णांक	x_error_check(Display *dpy, XErrorEvent *error_event);
स्थिर पूर्णांक	IsSerialName(char_u *name);
स्थिर शून्य	save_in_queue(char_u *buf, long_u len);
स्थिर शून्य	server_parse_message(Display *dpy, char_u *propInfo, long_u अंक_वस्तुसंच);

// Private variables for the "server" functionality
स्थिर Atom	registryProperty = None;
स्थिर Atom	vimProperty = None;
स्थिर पूर्णांक	got_x_error = FALSE;

स्थिर char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    पूर्णांक
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    पूर्णांक		i;
    पूर्णांक		res;
    char_u	*p = रिक्त;

    res = DoRegisterName(dpy, name);
    जर (res >= 0)
	परत OK;

    i = 1;
    करा
    {
	जर (res < -1 || i >= 1000)
	{
	    संदेश_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    परत FAIL;
	}
	जर (p == रिक्त)
	    p = alloc(STRLEN(name) + 10);
	जर (p == रिक्त)
	{
	    res = -10;
	    सुरूठेवा;
	}
	प्र_लिखो((अक्षर *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    पर्यंत (res < 0)
	;
    vim_free(p);

    परत OK;
}

    स्थिर पूर्णांक
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    जर (commProperty == None)
    {
	जर (SendInit(dpy) < 0)
	    परत -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, रिक्त);
    जर (w != (Window)0)
    {
	Status		status;
	पूर्णांक		dummyInt;
	अचिन्हित पूर्णांक	dummyUns;
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
	(शून्य)XSetErrorHandler(old_handler);
	जर (status != Success && w != commWindow)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    परत -1;
	}
	(शून्य)LookupName(dpy, name, /*delete=*/TRUE, रिक्त);
    }
    प्र_लिखो((अक्षर *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, मूळ_Window(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (शून्य)XSetErrorHandler(old_handler);

    जर (!got_x_error)
    {
	माप_प्रकार	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, (पूर्णांक)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	परत 0;
    }
    परत -2;
}

# जर defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    शून्य
serverChangeRegisteredWindow(
    Display	*dpy,		// Display to register with
    Window	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    commWindow = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    जर (SendInit(dpy) < 0)
	परत;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    जर (serverName != रिक्त)
    {
	// Reinsert name if it was already registered
	(शून्य)LookupName(dpy, serverName, /*delete=*/TRUE, रिक्त);
	प्र_लिखो((अक्षर *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_LENGTH, serverName);
	XChangeProperty(dpy, मूळ_Window(dpy, 0), registryProperty, XA_STRING, 8,
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
    पूर्णांक
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**result,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	as_अभिव्यक्ती,			// Interpret as keystrokes or expr ?
    पूर्णांक		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    पूर्णांक		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    पूर्णांक		    length;
    पूर्णांक		    res;
    स्थिर पूर्णांक	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = रिक्त;

    जर (result != रिक्त)
	*result = रिक्त;
    जर (name == रिक्त || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    जर (commProperty == None && dpy != रिक्त && SendInit(dpy) < 0)
	परत -1;

# जर defined(FEAT_EVAL)
    ch_log(रिक्त, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    जर (dpy == रिक्त || (serverName != रिक्त && STRICMP(name, serverName) == 0))
	परत sendToLocalVim(cmd, as_अभिव्यक्ती, result);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    पर्यंत (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	जर (w != None)
	{
	    जर (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, रिक्त);
		VIM_CLEAR(loosename);
		सुरूठेवा;
	    }
	}
	थांबा;
    }
    जर (w == None)
    {
	जर (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	परत -1;
    }
    अन्यथा जर (loosename != रिक्त)
	name = loosename;
    जर (server != रिक्त)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    length = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(length + 30);

    प्र_लिखो((अक्षर *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_अभिव्यक्ती ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    जर (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    प्र_लिखो((अक्षर *)property + length, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    length += STRLEN(property + length + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    vim_free(property);
    जर (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	परत -1;
    }

    जर (!as_अभिव्यक्ती) // There is no answer for this - Keys are sent async
	परत 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.code = 0;
    pending.result = रिक्त;
    pending.nextPtr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    जर (pendingCommands == &pending)
	pendingCommands = pending.nextPtr;
    अन्यथा
    {
	PendingCommand *pcPtr;

	साठी (pcPtr = pendingCommands; pcPtr != रिक्त; pcPtr = pcPtr->nextPtr)
	    जर (pcPtr->nextPtr == &pending)
	    {
		pcPtr->nextPtr = pending.nextPtr;
		थांबा;
	    }
    }

# जर defined(FEAT_EVAL)
    ch_log(रिक्त, "serverSendToVim() result: %s",
	    pending.result == रिक्त ? "NULL" : (अक्षर *)pending.result);
# endif
    जर (result != रिक्त)
	*result = pending.result;
    अन्यथा
	vim_free(pending.result);

    परत pending.code == 0 ? 0 : -1;
}

    स्थिर पूर्णांक
WaitForPend(शून्य *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    परत pending->result != रिक्त;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    स्थिर पूर्णांक
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    पूर्णांक		    अंक_Prop;
    पूर्णांक		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &अंक_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    जर (plist == रिक्त || got_x_error)
	परत FALSE;

    साठी (i = 0; i < अंक_Prop; i++)
	जर (plist[i] == vimProperty)
	{
	    XFree(plist);
	    परत TRUE;
	}
    XFree(plist);
    परत FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    स्थिर शून्य
ServerWait(
    Display	*dpy,
    Window	w,
    EndCond	endCond,
    शून्य	*endData,
    पूर्णांक		localLoop,
    पूर्णांक		seconds)
{
    समय_प्रकार	    start;
    समय_प्रकार	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_संख्या(dpy), &fds);
# अन्यथा
    संरचना pollfd   fds;

    fds.fd = Connection_संख्या(dpy);
    fds.events = POLLIN;
# endif

    समय(&start);
    पर्यंत (TRUE)
    {
	पर्यंत (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	जर (endCond(endData) != 0)
	    थांबा;
	जर (!WindowValid(dpy, w))
	    थांबा;
	समय(&now);
	जर (seconds >= 0 && (now - start) >= seconds)
	    थांबा;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	जर (localLoop)
	{
# ifdef HAVE_SELECT
	    संरचना timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    जर (select(FD_SETSIZE, &fds, रिक्त, रिक्त, &tv) < 0)
		थांबा;
# अन्यथा
	    जर (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		थांबा;
# endif
	}
	अन्यथा
	{
	    जर (got_int)
		थांबा;
	    ui_delay((दीर्घ)UI_MSEC_DELAY, TRUE);
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
    long_u	अंक_वस्तुसंच;
    int_u	w;
    list_T	*list;

    जर (registryProperty == None)
    {
	जर (SendInit(dpy) < 0)
	    परत रिक्त;
    }

    list = list_alloc();
    जर (list == रिक्त)
	परत रिक्त;

    /*
     * Read the registry property.
     */
    जर (GetRegProp(dpy, &regProp, &अंक_वस्तुसंच, TRUE) == FAIL)
	परत रिक्त;

    /*
     * Scan all of the names out of the property.
     */
    साठी (p = regProp; (long_u)(p - regProp) < अंक_वस्तुसंच; p++)
    {
	entry = p;
	पर्यंत (*p != 0 && !SAFE_isspace(*p))
	    p++;
	जर (*p != 0)
	{
	    w = None;
	    माला_पूछो((अक्षर *)entry, "%x", &w);
	    जर (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    पर्यंत (*p != 0)
		p++;
	}
    }
    जर (regProp != empty_prop)
	XFree(regProp);
    परत list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    स्थिर संरचना ServerReply *
ServerReplyFind(Window w, गणना ServerReplyOp op)
{
    संरचना ServerReply *p;
    संरचना ServerReply e;
    पूर्णांक		i;

    p = (संरचना ServerReply *) serverReply.ga_data;
    साठी (i = 0; i < serverReply.ga_len; i++, p++)
	जर (p->id == w)
	    थांबा;
    जर (i >= serverReply.ga_len)
	p = रिक्त;

    जर (p == रिक्त && op == SROP_Add)
    {
	जर (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, आकार(संरचना ServerReply), 1);
	जर (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((संरचना ServerReply *) serverReply.ga_data)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, आकार(e));
	    serverReply.ga_len++;
	}
    }
    अन्यथा जर (p != रिक्त && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * आकार(*p));
	serverReply.ga_len--;
    }

    परत p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    अचिन्हित  id = None;

    माला_पूछो((अक्षर *)str, "0x%x", &id);
    जर (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    परत (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    पूर्णांक
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    पूर्णांक		length;
    पूर्णांक		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    जर (commProperty == None)
    {
	जर (SendInit(dpy) < 0)
	    परत -2;
    }
    जर (!WindowValid(dpy, win))
	परत -1;

    length = STRLEN(p_enc) + STRLEN(str) + 14;
    जर ((property = alloc(length + 30)) == रिक्त)
	परत -1;

    प्र_लिखो((अक्षर *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (अचिन्हित पूर्णांक)commWindow);
    // Add length of what "%x" resulted in.
    length += STRLEN(property + length);
    res = AppendPropCarefully(dpy, win, commProperty, property, length + 1);
    vim_free(property);

    परत res;
}

    स्थिर पूर्णांक
WaitForReply(शून्य *p)
{
    Window  *w = (Window *) p;

    परत ServerReplyFind(*w, SROP_Find) != रिक्त;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    पूर्णांक
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    पूर्णांक		localLoop,
    पूर्णांक		timeout)
{
    पूर्णांक		len;
    char_u	*s;
    संरचना	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    जर ((p = ServerReplyFind(win, SROP_Find)) != रिक्त && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_data);
	len = STRLEN(*str) + 1;
	जर (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_data;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	अन्यथा
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	परत 0;
    }
    परत -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    पूर्णांक
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    संरचना ServerReply *p;

    जर ((p = ServerReplyFind(win, SROP_Find)) != रिक्त && p->strings.ga_len > 0)
    {
	जर (str != रिक्त)
	    *str = p->strings.ga_data;
	परत 1;
    }
    जर (!WindowValid(dpy, win))
	परत -1;
    परत 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    स्थिर पूर्णांक
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    जर (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    जर (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    जर (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    जर (commWindow == None)
    {
	commWindow = XCreateSimpleWindow(dpy, XDefault_मूळ_Window(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_आदान(dpy, commWindow, PropertyChangeMask);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, commWindow);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, commWindow, vimProperty, XA_STRING,
		    8, PropModeReplace, (char_u *)VIM_VERSION_SHORT,
			(पूर्णांक)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (शून्य)XSetErrorHandler(old_handler);

    परत got_x_error ? -1 : 0;
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
    स्थिर Window
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    पूर्णांक		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *entry;
    char_u	*p;
    long_u	अंक_वस्तुसंच;
    int_u	return_मूल्य;

    /*
     * Read the registry property.
     */
    जर (GetRegProp(dpy, &regProp, &अंक_वस्तुसंच, FALSE) == FAIL)
	परत 0;

    /*
     * Scan the property for the desired name.
     */
    return_मूल्य = (int_u)None;
    entry = रिक्त;	// Not needed, but eliminates compiler warning.
    साठी (p = regProp; (long_u)(p - regProp) < अंक_वस्तुसंच; )
    {
	entry = p;
	पर्यंत (*p != 0 && !SAFE_isspace(*p))
	    p++;
	जर (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    माला_पूछो((अक्षर *)entry, "%x", &return_मूल्य);
	    थांबा;
	}
	पर्यंत (*p != 0)
	    p++;
	p++;
    }

    जर (loose != रिक्त && return_मूल्य == (int_u)None && !IsSerialName(name))
    {
	साठी (p = regProp; (long_u)(p - regProp) < अंक_वस्तुसंच; )
	{
	    entry = p;
	    पर्यंत (*p != 0 && !SAFE_isspace(*p))
		p++;
	    जर (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		जर (माला_पूछो((अक्षर *)entry, "%x", &return_मूल्य) == 1
			&& return_मूल्य != (int_u)None)
		    *loose = vim_strsave(p + 1);
		थांबा;
	    }
	    पर्यंत (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    जर (delete && return_मूल्य != (int_u)None)
    {
	पूर्णांक मोजणी;

	पर्यंत (*p != 0)
	    p++;
	p++;
	मोजणी = अंक_वस्तुसंच - (p - regProp);
	जर (मोजणी > 0)
	    mch_memmove(entry, p, मोजणी);
	XChangeProperty(dpy, मूळ_Window(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(पूर्णांक)(अंक_वस्तुसंच - (p - entry)));
	XSync(dpy, False);
    }

    जर (regProp != empty_prop)
	XFree(regProp);
    परत (Window)return_मूल्य;
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
    स्थिर शून्य
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    Window	win)	// Window to remove
{
    char_u	*regProp, *entry = रिक्त;
    char_u	*p;
    long_u	अंक_वस्तुसंच;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    जर (GetRegProp(dpy, &regProp, &अंक_वस्तुसंच, FALSE) == FAIL)
	परत;

    // Scan the property for the window id.
    साठी (p = regProp; (long_u)(p - regProp) < अंक_वस्तुसंच; )
    {
	जर (*p != 0)
	{
	    माला_पूछो((अक्षर *)p, "%x", &wwin);
	    जर ((Window)wwin == win)
	    {
		पूर्णांक lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		पर्यंत (*p != 0)
		    p++;
		p++;
		lastHalf = अंक_वस्तुसंच - (p - regProp);
		जर (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		अंक_वस्तुसंच = (entry - regProp) + lastHalf;
		p = entry;
		सुरूठेवा;
	    }
	}
	पर्यंत (*p != 0)
	    p++;
	p++;
    }

    जर (entry != रिक्त)
    {
	XChangeProperty(dpy, मूळ_Window(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(पूर्णांक)(p - regProp));
	XSync(dpy, False);
    }

    जर (regProp != empty_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    स्थिर पूर्णांक
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*अंक_Itemsp,
    पूर्णांक		domsg)		// When TRUE give error message.
{
    पूर्णांक		result, actualFormat;
    long_u	bytesAfter;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = रिक्त;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    result = XGetWindowProperty(dpy, मूळ_Window(dpy, 0), registryProperty, 0L,
				(दीर्घ)MAX_PROP_शब्द, False,
				XA_STRING, &actualType,
				&actualFormat, अंक_Itemsp, &bytesAfter,
				regPropp);

    XSync(dpy, FALSE);
    (शून्य)XSetErrorHandler(old_handler);
    जर (got_x_error)
	परत FAIL;

    जर (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*अंक_Itemsp = 0;
	*regPropp = empty_prop;
	परत OK;
    }

    // If the property is improperly formed, then delete it.
    जर (result != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	जर (*regPropp != रिक्त)
	    XFree(*regPropp);
	XDeleteProperty(dpy, मूळ_Window(dpy, 0), registryProperty);
	जर (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	परत FAIL;
    }
    परत OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    शून्य
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    पूर्णांक		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    पूर्णांक		result, actualFormat;
    long_u	अंक_वस्तुसंच, bytesAfter;
    Atom	actualType;

    जर (eventPtr != रिक्त)
    {
	जर (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_मूल्य)
	    परत;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = रिक्त;
    result = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(दीर्घ)MAX_PROP_शब्द, True,
				XA_STRING, &actualType,
				&actualFormat, &अंक_वस्तुसंच, &bytesAfter,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    जर (result != Success || actualType != XA_STRING || actualFormat != 8)
    {
	जर (propInfo != रिक्त)
	    XFree(propInfo);
	परत;
    }
    जर (immediate)
	server_parse_message(dpy, propInfo, अंक_वस्तुसंच);
    अन्यथा
	save_in_queue(propInfo, अंक_वस्तुसंच);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    स्थिर शून्य
save_in_queue(char_u *propInfo, long_u len)
{
    x_queue_T *गाठ;

    गाठ = ALLOC_ONE(x_queue_T);
    जर (गाठ == रिक्त)
	परत;	    // out of memory
    गाठ->propInfo = propInfo;
    गाठ->len = len;

    जर (head.next == रिक्त)   // initialize circular queue
    {
	head.next = &head;
	head.prev = &head;
    }

    // insert node at tail of queue
    गाठ->next = &head;
    गाठ->prev = head.prev;
    head.prev->next = गाठ;
    head.prev = गाठ;
}

/*
 * Parses queued clientserver messages.
 */
    शून्य
server_parse_messages(शून्य)
{
    x_queue_T	*गाठ;

    जर (!X_DISPLAY)
	परत; // cannot happen?
    पर्यंत (head.next != रिक्त && head.next != &head)
    {
	गाठ = head.next;
	head.next = गाठ->next;
	गाठ->next->prev = गाठ->prev;
	server_parse_message(X_DISPLAY, गाठ->propInfo, गाठ->len);
	vim_free(गाठ);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    पूर्णांक
server_waiting(शून्य)
{
    परत head.next != रिक्त && head.next != &head;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    स्थिर शून्य
server_parse_message(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	अंक_वस्तुसंच)  // The size of propInfo in bytes.
{
    char_u	*p;
    पूर्णांक		code;
    char_u	*tofree;

# जर defined(FEAT_EVAL)
    ch_log(रिक्त, "server_parse_message() numItems: %ld", अंक_वस्तुसंच);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    साठी (p = propInfo; (long_u)(p - propInfo) < अंक_वस्तुसंच; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	जर (*p == 0)
	{
	    p++;
	    सुरूठेवा;
	}

	जर ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    Window	resWindow;
	    char_u	*name, *script, *serial, *end;
	    Bool	as_किल्ल्या = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = रिक्त;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = रिक्त;
	    enc = रिक्त;
	    पर्यंत ((long_u)(p - propInfo) < अंक_वस्तुसंच && *p == '-')
	    {
# जर defined(FEAT_EVAL)
		ch_log(रिक्त, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		निवड (p[1])
		{
		    स्थिती 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			पर्यंत (vim_isxdigit(*end))
			{
			    resWindow = 16 * resWindow + (long_u)hex2nr(*end);
			    ++end;
			}
			जर (end == p + 2 || *end != ' ')
			    resWindow = None;
			अन्यथा
			{
			    p = serial = end + 1;
			    clientWindow = resWindow; // Remember in global
			}
			थांबा;
		    स्थिती 'n':
			जर (p[2] == ' ')
			    name = p + 3;
			थांबा;
		    स्थिती 's':
			जर (p[2] == ' ')
			    script = p + 3;
			थांबा;
		    स्थिती 'E':
			जर (p[2] == ' ')
			    enc = p + 3;
			थांबा;
		}
		पर्यंत (*p != 0)
		    p++;
		p++;
	    }

	    जर (script == रिक्त || name == रिक्त)
		सुरूठेवा;

	    जर (serverName != रिक्त && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		जर (as_किल्ल्या)
		    server_to_आदान_buf(script);
		अन्यथा
		{
		    char_u      *res;

		    res = eval_client_अभिव्यक्ती_to_string(script);
		    जर (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(शून्य)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			प्र_लिखो(reply.ga_data, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			जर (res != रिक्त)
			    ga_concat(&reply, res);
			अन्यथा
			{
			    ga_concat(&reply,
				   (char_u *)_(e_invalid_expression_received));
			    ga_append(&reply, 0);
			    GA_CONCAT_LITERAL(&reply, "-c 1");
			}
			ga_append(&reply, NUL);
			(शून्य)AppendPropCarefully(dpy, resWindow, commProperty,
						 reply.ga_data, reply.ga_len);
			ga_clear(&reply);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	अन्यथा जर (*p == 'r' && p[1] == 0)
	{
	    पूर्णांक		    serial, gotSerial;
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
	    enc = रिक्त;
	    पर्यंत ((long_u)(p - propInfo) < अंक_वस्तुसंच && *p == '-')
	    {
		निवड (p[1])
		{
		    स्थिती 'r':
			जर (p[2] == ' ')
			    res = p + 3;
			थांबा;
		    स्थिती 'E':
			जर (p[2] == ' ')
			    enc = p + 3;
			थांबा;
		    स्थिती 's':
			जर (माला_पूछो((अक्षर *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			थांबा;
		    स्थिती 'c':
			जर (माला_पूछो((अक्षर *)p + 2, " %d", &code) != 1)
			    code = 0;
			थांबा;
		}
		पर्यंत (*p != 0)
		    p++;
		p++;
	    }

	    जर (!gotSerial)
		सुरूठेवा;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    साठी (pcPtr = pendingCommands; pcPtr != रिक्त; pcPtr = pcPtr->nextPtr)
	    {
		जर (serial != pcPtr->serial || pcPtr->result != रिक्त)
		    सुरूठेवा;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		जर (tofree == रिक्त)
		    res = vim_strsave(res);
		pcPtr->result = res;
		थांबा;
	    }
	}
	अन्यथा जर (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    अचिन्हित पूर्णांक u;
	    पूर्णांक		gotWindow;
	    char_u	*str;
	    संरचना	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = रिक्त;
	    पर्यंत ((long_u)(p - propInfo) < अंक_वस्तुसंच && *p == '-')
	    {
		निवड (p[1])
		{
		    स्थिती 'n':
			जर (p[2] == ' ')
			    str = p + 3;
			थांबा;
		    स्थिती 'E':
			जर (p[2] == ' ')
			    enc = p + 3;
			थांबा;
		    स्थिती 'w':
			जर (माला_पूछो((अक्षर *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			थांबा;
		}
		पर्यंत (*p != 0)
		    p++;
		p++;
	    }

	    जर (!gotWindow)
		सुरूठेवा;
	    str = serverConvert(enc, str, &tofree);
	    जर ((r = ServerReplyFind(win, SROP_Add)) != रिक्त)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		प्र_लिखो((अक्षर *)winstr, "0x%x", (अचिन्हित पूर्णांक)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	अन्यथा
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    पर्यंत (*p != 0)
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
    स्थिर पूर्णांक
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*मूल्य,		// Characters  to append to property.
    पूर्णांक		length)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, मूल्य, length);
    XSync(dpy, False);
    (शून्य) XSetErrorHandler(old_handler);
    परत got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    स्थिर पूर्णांक
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    परत 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    स्थिर पूर्णांक
IsSerialName(char_u *str)
{
    पूर्णांक len = STRLEN(str);

    परत (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
