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

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "version.h"

#எனில் (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

வகைவரை கட்டமைப்பு PendingCommand
{
    முழு	    serial;	// Serial number expected in result.
    முழு	    code;	// Result Code. 0 is OK
    char_u  *result;	// String result for command (malloc'ed).
			// NULL means command still pending.
    கட்டமைப்பு PendingCommand *nextPtr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

நிலையான PendingCommand *pendingCommands = வெற்று;
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

# define MAX_PROP_சொற்கள் 100000

கட்டமைப்பு ServerReply
{
    Window  id;
    garray_T strings;
};
நிலையான garray_T serverReply = { 0, 0, 0, 0, 0 };
எண்ணிக்கை ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

வகைவரை முழு (*EndCond)(வெறுமை *);

கட்டமைப்பு x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    கட்டமைப்பு x_cmdqueue	*next;
    கட்டமைப்பு x_cmdqueue	*prev;
};

வகைவரை கட்டமைப்பு x_cmdqueue x_queue_T;

// dummy node, header for circular queue
நிலையான x_queue_T head = {வெற்று, 0, வெற்று, வெற்று};

/*
 * Forward declarations for procedures defined later in this file:
 */

நிலையான Window	LookupName(Display *dpy, char_u *name, முழு delete, char_u **loose);
நிலையான முழு	SendInit(Display *dpy);
நிலையான முழு	DoRegisterName(Display *dpy, char_u *name);
நிலையான வெறுமை	DeleteAnyLingerer(Display *dpy, Window w);
நிலையான முழு	GetRegProp(Display *dpy, char_u **regPropp, long_u *இலக்கம்_Itemsp, முழு domsg);
நிலையான முழு	WaitForPend(வெறுமை *p);
நிலையான முழு	WindowValid(Display *dpy, Window w);
நிலையான வெறுமை	ServerWait(Display *dpy, Window w, EndCond endCond, வெறுமை *endData, முழு localLoop, முழு seconds);
நிலையான முழு	AppendPropCarefully(Display *display, Window window, Atom property, char_u *மதிப்பு, முழு length);
நிலையான முழு	x_error_check(Display *dpy, XErrorEvent *error_event);
நிலையான முழு	IsSerialName(char_u *name);
நிலையான வெறுமை	save_in_queue(char_u *buf, long_u len);
நிலையான வெறுமை	server_parse_message(Display *dpy, char_u *propInfo, long_u இலக்கம்_பொருட்கள்);

// Private variables for the "server" functionality
நிலையான Atom	registryProperty = None;
நிலையான Atom	vimProperty = None;
நிலையான முழு	got_x_error = FALSE;

நிலையான char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    முழு
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    முழு		i;
    முழு		res;
    char_u	*p = வெற்று;

    res = DoRegisterName(dpy, name);
    எனில் (res >= 0)
	திரும்பு OK;

    i = 1;
    செய்
    {
	எனில் (res < -1 || i >= 1000)
	{
	    செய்தி_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    திரும்பு FAIL;
	}
	எனில் (p == வெற்று)
	    p = alloc(STRLEN(name) + 10);
	எனில் (p == வெற்று)
	{
	    res = -10;
	    தொடர்;
	}
	sprintf((எழுத்து *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    போது (res < 0)
	;
    vim_free(p);

    திரும்பு OK;
}

    நிலையான முழு
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    எனில் (commProperty == None)
    {
	எனில் (SendInit(dpy) < 0)
	    திரும்பு -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, வெற்று);
    எனில் (w != (Window)0)
    {
	Status		status;
	முழு		dummyInt;
	unsigned முழு	dummyUns;
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
	(வெறுமை)XSetErrorHandler(old_handler);
	எனில் (status != Success && w != commWindow)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    திரும்பு -1;
	}
	(வெறுமை)LookupName(dpy, name, /*delete=*/TRUE, வெற்று);
    }
    sprintf((எழுத்து *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, வேர்_Window(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (வெறுமை)XSetErrorHandler(old_handler);

    எனில் (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, (முழு)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	திரும்பு 0;
    }
    திரும்பு -2;
}

# எனில் defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    வெறுமை
serverChangeRegisteredWindow(
    Display	*dpy,		// Display to register with
    Window	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    commWindow = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    எனில் (SendInit(dpy) < 0)
	திரும்பு;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    எனில் (serverName != வெற்று)
    {
	// Reinsert name if it was already registered
	(வெறுமை)LookupName(dpy, serverName, /*delete=*/TRUE, வெற்று);
	sprintf((எழுத்து *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_LENGTH, serverName);
	XChangeProperty(dpy, வேர்_Window(dpy, 0), registryProperty, XA_STRING, 8,
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
    முழு
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**result,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	as_கோவை,			// Interpret as keystrokes or expr ?
    முழு		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    முழு		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    முழு		    length;
    முழு		    res;
    நிலையான முழு	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = வெற்று;

    எனில் (result != வெற்று)
	*result = வெற்று;
    எனில் (name == வெற்று || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    எனில் (commProperty == None && dpy != வெற்று && SendInit(dpy) < 0)
	திரும்பு -1;

# எனில் defined(FEAT_EVAL)
    ch_log(வெற்று, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    எனில் (dpy == வெற்று || (serverName != வெற்று && STRICMP(name, serverName) == 0))
	திரும்பு sendToLocalVim(cmd, as_கோவை, result);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    போது (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	எனில் (w != None)
	{
	    எனில் (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, வெற்று);
		VIM_CLEAR(loosename);
		தொடர்;
	    }
	}
	நிறுத்து;
    }
    எனில் (w == None)
    {
	எனில் (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	திரும்பு -1;
    }
    இல்லையெனில் எனில் (loosename != வெற்று)
	name = loosename;
    எனில் (server != வெற்று)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    length = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(length + 30);

    sprintf((எழுத்து *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_கோவை ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    எனில் (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((எழுத்து *)property + length, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    length += STRLEN(property + length + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    vim_free(property);
    எனில் (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	திரும்பு -1;
    }

    எனில் (!as_கோவை) // There is no answer for this - Keys are sent async
	திரும்பு 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.code = 0;
    pending.result = வெற்று;
    pending.nextPtr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    எனில் (pendingCommands == &pending)
	pendingCommands = pending.nextPtr;
    இல்லையெனில்
    {
	PendingCommand *pcPtr;

	க்கு (pcPtr = pendingCommands; pcPtr != வெற்று; pcPtr = pcPtr->nextPtr)
	    எனில் (pcPtr->nextPtr == &pending)
	    {
		pcPtr->nextPtr = pending.nextPtr;
		நிறுத்து;
	    }
    }

# எனில் defined(FEAT_EVAL)
    ch_log(வெற்று, "serverSendToVim() result: %s",
	    pending.result == வெற்று ? "NULL" : (எழுத்து *)pending.result);
# endif
    எனில் (result != வெற்று)
	*result = pending.result;
    இல்லையெனில்
	vim_free(pending.result);

    திரும்பு pending.code == 0 ? 0 : -1;
}

    நிலையான முழு
WaitForPend(வெறுமை *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    திரும்பு pending->result != வெற்று;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    நிலையான முழு
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    முழு		    இலக்கம்_Prop;
    முழு		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &இலக்கம்_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    எனில் (plist == வெற்று || got_x_error)
	திரும்பு FALSE;

    க்கு (i = 0; i < இலக்கம்_Prop; i++)
	எனில் (plist[i] == vimProperty)
	{
	    XFree(plist);
	    திரும்பு TRUE;
	}
    XFree(plist);
    திரும்பு FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    நிலையான வெறுமை
ServerWait(
    Display	*dpy,
    Window	w,
    EndCond	endCond,
    வெறுமை	*endData,
    முழு		localLoop,
    முழு		seconds)
{
    time_t	    start;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_எண்(dpy), &fds);
# இல்லையெனில்
    கட்டமைப்பு pollfd   fds;

    fds.fd = Connection_எண்(dpy);
    fds.events = POLLIN;
# endif

    time(&start);
    போது (TRUE)
    {
	போது (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	எனில் (endCond(endData) != 0)
	    நிறுத்து;
	எனில் (!WindowValid(dpy, w))
	    நிறுத்து;
	time(&now);
	எனில் (seconds >= 0 && (now - start) >= seconds)
	    நிறுத்து;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	எனில் (localLoop)
	{
# ifdef HAVE_SELECT
	    கட்டமைப்பு timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    எனில் (select(FD_SETSIZE, &fds, வெற்று, வெற்று, &tv) < 0)
		நிறுத்து;
# இல்லையெனில்
	    எனில் (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		நிறுத்து;
# endif
	}
	இல்லையெனில்
	{
	    எனில் (got_int)
		நிறுத்து;
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
    long_u	இலக்கம்_பொருட்கள்;
    int_u	w;
    list_T	*list;

    எனில் (registryProperty == None)
    {
	எனில் (SendInit(dpy) < 0)
	    திரும்பு வெற்று;
    }

    list = list_alloc();
    எனில் (list == வெற்று)
	திரும்பு வெற்று;

    /*
     * Read the registry property.
     */
    எனில் (GetRegProp(dpy, &regProp, &இலக்கம்_பொருட்கள், TRUE) == FAIL)
	திரும்பு வெற்று;

    /*
     * Scan all of the names out of the property.
     */
    க்கு (p = regProp; (long_u)(p - regProp) < இலக்கம்_பொருட்கள்; p++)
    {
	entry = p;
	போது (*p != 0 && !SAFE_isspace(*p))
	    p++;
	எனில் (*p != 0)
	{
	    w = None;
	    sscanf((எழுத்து *)entry, "%x", &w);
	    எனில் (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    போது (*p != 0)
		p++;
	}
    }
    எனில் (regProp != empty_prop)
	XFree(regProp);
    திரும்பு list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    நிலையான கட்டமைப்பு ServerReply *
ServerReplyFind(Window w, எண்ணிக்கை ServerReplyOp op)
{
    கட்டமைப்பு ServerReply *p;
    கட்டமைப்பு ServerReply e;
    முழு		i;

    p = (கட்டமைப்பு ServerReply *) serverReply.ga_data;
    க்கு (i = 0; i < serverReply.ga_len; i++, p++)
	எனில் (p->id == w)
	    நிறுத்து;
    எனில் (i >= serverReply.ga_len)
	p = வெற்று;

    எனில் (p == வெற்று && op == SROP_Add)
    {
	எனில் (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, அளவு(கட்டமைப்பு ServerReply), 1);
	எனில் (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((கட்டமைப்பு ServerReply *) serverReply.ga_data)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, அளவு(e));
	    serverReply.ga_len++;
	}
    }
    இல்லையெனில் எனில் (p != வெற்று && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * அளவு(*p));
	serverReply.ga_len--;
    }

    திரும்பு p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    unsigned  id = None;

    sscanf((எழுத்து *)str, "0x%x", &id);
    எனில் (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    திரும்பு (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    முழு
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    முழு		length;
    முழு		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    எனில் (commProperty == None)
    {
	எனில் (SendInit(dpy) < 0)
	    திரும்பு -2;
    }
    எனில் (!WindowValid(dpy, win))
	திரும்பு -1;

    length = STRLEN(p_enc) + STRLEN(str) + 14;
    எனில் ((property = alloc(length + 30)) == வெற்று)
	திரும்பு -1;

    sprintf((எழுத்து *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (unsigned முழு)commWindow);
    // Add length of what "%x" resulted in.
    length += STRLEN(property + length);
    res = AppendPropCarefully(dpy, win, commProperty, property, length + 1);
    vim_free(property);

    திரும்பு res;
}

    நிலையான முழு
WaitForReply(வெறுமை *p)
{
    Window  *w = (Window *) p;

    திரும்பு ServerReplyFind(*w, SROP_Find) != வெற்று;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    முழு
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    முழு		localLoop,
    முழு		timeout)
{
    முழு		len;
    char_u	*s;
    கட்டமைப்பு	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    எனில் ((p = ServerReplyFind(win, SROP_Find)) != வெற்று && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_data);
	len = STRLEN(*str) + 1;
	எனில் (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_data;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	இல்லையெனில்
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	திரும்பு 0;
    }
    திரும்பு -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    முழு
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    கட்டமைப்பு ServerReply *p;

    எனில் ((p = ServerReplyFind(win, SROP_Find)) != வெற்று && p->strings.ga_len > 0)
    {
	எனில் (str != வெற்று)
	    *str = p->strings.ga_data;
	திரும்பு 1;
    }
    எனில் (!WindowValid(dpy, win))
	திரும்பு -1;
    திரும்பு 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    நிலையான முழு
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    எனில் (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    எனில் (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    எனில் (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    எனில் (commWindow == None)
    {
	commWindow = XCreateSimpleWindow(dpy, XDefault_வேர்_Window(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_உள்ளீடு(dpy, commWindow, PropertyChangeMask);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, commWindow);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, commWindow, vimProperty, XA_STRING,
		    8, PropModeReplace, (char_u *)VIM_VERSION_SHORT,
			(முழு)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (வெறுமை)XSetErrorHandler(old_handler);

    திரும்பு got_x_error ? -1 : 0;
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
    நிலையான Window
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    முழு		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *entry;
    char_u	*p;
    long_u	இலக்கம்_பொருட்கள்;
    int_u	return_மதிப்பு;

    /*
     * Read the registry property.
     */
    எனில் (GetRegProp(dpy, &regProp, &இலக்கம்_பொருட்கள், FALSE) == FAIL)
	திரும்பு 0;

    /*
     * Scan the property for the desired name.
     */
    return_மதிப்பு = (int_u)None;
    entry = வெற்று;	// Not needed, but eliminates compiler warning.
    க்கு (p = regProp; (long_u)(p - regProp) < இலக்கம்_பொருட்கள்; )
    {
	entry = p;
	போது (*p != 0 && !SAFE_isspace(*p))
	    p++;
	எனில் (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    sscanf((எழுத்து *)entry, "%x", &return_மதிப்பு);
	    நிறுத்து;
	}
	போது (*p != 0)
	    p++;
	p++;
    }

    எனில் (loose != வெற்று && return_மதிப்பு == (int_u)None && !IsSerialName(name))
    {
	க்கு (p = regProp; (long_u)(p - regProp) < இலக்கம்_பொருட்கள்; )
	{
	    entry = p;
	    போது (*p != 0 && !SAFE_isspace(*p))
		p++;
	    எனில் (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		எனில் (sscanf((எழுத்து *)entry, "%x", &return_மதிப்பு) == 1
			&& return_மதிப்பு != (int_u)None)
		    *loose = vim_strsave(p + 1);
		நிறுத்து;
	    }
	    போது (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    எனில் (delete && return_மதிப்பு != (int_u)None)
    {
	முழு மொத்தம்;

	போது (*p != 0)
	    p++;
	p++;
	மொத்தம் = இலக்கம்_பொருட்கள் - (p - regProp);
	எனில் (மொத்தம் > 0)
	    mch_memmove(entry, p, மொத்தம்);
	XChangeProperty(dpy, வேர்_Window(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(முழு)(இலக்கம்_பொருட்கள் - (p - entry)));
	XSync(dpy, False);
    }

    எனில் (regProp != empty_prop)
	XFree(regProp);
    திரும்பு (Window)return_மதிப்பு;
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
    நிலையான வெறுமை
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    Window	win)	// Window to remove
{
    char_u	*regProp, *entry = வெற்று;
    char_u	*p;
    long_u	இலக்கம்_பொருட்கள்;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    எனில் (GetRegProp(dpy, &regProp, &இலக்கம்_பொருட்கள், FALSE) == FAIL)
	திரும்பு;

    // Scan the property for the window id.
    க்கு (p = regProp; (long_u)(p - regProp) < இலக்கம்_பொருட்கள்; )
    {
	எனில் (*p != 0)
	{
	    sscanf((எழுத்து *)p, "%x", &wwin);
	    எனில் ((Window)wwin == win)
	    {
		முழு lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		போது (*p != 0)
		    p++;
		p++;
		lastHalf = இலக்கம்_பொருட்கள் - (p - regProp);
		எனில் (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		இலக்கம்_பொருட்கள் = (entry - regProp) + lastHalf;
		p = entry;
		தொடர்;
	    }
	}
	போது (*p != 0)
	    p++;
	p++;
    }

    எனில் (entry != வெற்று)
    {
	XChangeProperty(dpy, வேர்_Window(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(முழு)(p - regProp));
	XSync(dpy, False);
    }

    எனில் (regProp != empty_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    நிலையான முழு
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*இலக்கம்_Itemsp,
    முழு		domsg)		// When TRUE give error message.
{
    முழு		result, actualFormat;
    long_u	bytesAfter;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = வெற்று;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    result = XGetWindowProperty(dpy, வேர்_Window(dpy, 0), registryProperty, 0L,
				(long)MAX_PROP_சொற்கள், False,
				XA_STRING, &actualType,
				&actualFormat, இலக்கம்_Itemsp, &bytesAfter,
				regPropp);

    XSync(dpy, FALSE);
    (வெறுமை)XSetErrorHandler(old_handler);
    எனில் (got_x_error)
	திரும்பு FAIL;

    எனில் (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*இலக்கம்_Itemsp = 0;
	*regPropp = empty_prop;
	திரும்பு OK;
    }

    // If the property is improperly formed, then delete it.
    எனில் (result != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	எனில் (*regPropp != வெற்று)
	    XFree(*regPropp);
	XDeleteProperty(dpy, வேர்_Window(dpy, 0), registryProperty);
	எனில் (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	திரும்பு FAIL;
    }
    திரும்பு OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    வெறுமை
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    முழு		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    முழு		result, actualFormat;
    long_u	இலக்கம்_பொருட்கள், bytesAfter;
    Atom	actualType;

    எனில் (eventPtr != வெற்று)
    {
	எனில் (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_மதிப்பு)
	    திரும்பு;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = வெற்று;
    result = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(long)MAX_PROP_சொற்கள், True,
				XA_STRING, &actualType,
				&actualFormat, &இலக்கம்_பொருட்கள், &bytesAfter,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    எனில் (result != Success || actualType != XA_STRING || actualFormat != 8)
    {
	எனில் (propInfo != வெற்று)
	    XFree(propInfo);
	திரும்பு;
    }
    எனில் (immediate)
	server_parse_message(dpy, propInfo, இலக்கம்_பொருட்கள்);
    இல்லையெனில்
	save_in_queue(propInfo, இலக்கம்_பொருட்கள்);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    நிலையான வெறுமை
save_in_queue(char_u *propInfo, long_u len)
{
    x_queue_T *முனை;

    முனை = ALLOC_ONE(x_queue_T);
    எனில் (முனை == வெற்று)
	திரும்பு;	    // out of memory
    முனை->propInfo = propInfo;
    முனை->len = len;

    எனில் (head.next == வெற்று)   // initialize circular queue
    {
	head.next = &head;
	head.prev = &head;
    }

    // insert node at tail of queue
    முனை->next = &head;
    முனை->prev = head.prev;
    head.prev->next = முனை;
    head.prev = முனை;
}

/*
 * Parses queued clientserver messages.
 */
    வெறுமை
server_parse_messages(வெறுமை)
{
    x_queue_T	*முனை;

    எனில் (!X_DISPLAY)
	திரும்பு; // cannot happen?
    போது (head.next != வெற்று && head.next != &head)
    {
	முனை = head.next;
	head.next = முனை->next;
	முனை->next->prev = முனை->prev;
	server_parse_message(X_DISPLAY, முனை->propInfo, முனை->len);
	vim_free(முனை);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    முழு
server_waiting(வெறுமை)
{
    திரும்பு head.next != வெற்று && head.next != &head;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    நிலையான வெறுமை
server_parse_message(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	இலக்கம்_பொருட்கள்)  // The size of propInfo in bytes.
{
    char_u	*p;
    முழு		code;
    char_u	*tofree;

# எனில் defined(FEAT_EVAL)
    ch_log(வெற்று, "server_parse_message() numItems: %ld", இலக்கம்_பொருட்கள்);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    க்கு (p = propInfo; (long_u)(p - propInfo) < இலக்கம்_பொருட்கள்; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	எனில் (*p == 0)
	{
	    p++;
	    தொடர்;
	}

	எனில் ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    Window	resWindow;
	    char_u	*name, *script, *serial, *end;
	    Bool	as_சாவிகள் = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = வெற்று;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = வெற்று;
	    enc = வெற்று;
	    போது ((long_u)(p - propInfo) < இலக்கம்_பொருட்கள் && *p == '-')
	    {
# எனில் defined(FEAT_EVAL)
		ch_log(வெற்று, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		தேர்வு (p[1])
		{
		    நிலை 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			போது (vim_isxdigit(*end))
			{
			    resWindow = 16 * resWindow + (long_u)hex2nr(*end);
			    ++end;
			}
			எனில் (end == p + 2 || *end != ' ')
			    resWindow = None;
			இல்லையெனில்
			{
			    p = serial = end + 1;
			    clientWindow = resWindow; // Remember in global
			}
			நிறுத்து;
		    நிலை 'n':
			எனில் (p[2] == ' ')
			    name = p + 3;
			நிறுத்து;
		    நிலை 's':
			எனில் (p[2] == ' ')
			    script = p + 3;
			நிறுத்து;
		    நிலை 'E':
			எனில் (p[2] == ' ')
			    enc = p + 3;
			நிறுத்து;
		}
		போது (*p != 0)
		    p++;
		p++;
	    }

	    எனில் (script == வெற்று || name == வெற்று)
		தொடர்;

	    எனில் (serverName != வெற்று && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		எனில் (as_சாவிகள்)
		    server_to_உள்ளீடு_buf(script);
		இல்லையெனில்
		{
		    char_u      *res;

		    res = eval_client_கோவை_to_string(script);
		    எனில் (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(வெறுமை)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(reply.ga_data, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			எனில் (res != வெற்று)
			    ga_concat(&reply, res);
			இல்லையெனில்
			{
			    ga_concat(&reply,
				   (char_u *)_(e_invalid_expression_received));
			    ga_append(&reply, 0);
			    GA_CONCAT_LITERAL(&reply, "-c 1");
			}
			ga_append(&reply, NUL);
			(வெறுமை)AppendPropCarefully(dpy, resWindow, commProperty,
						 reply.ga_data, reply.ga_len);
			ga_clear(&reply);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	இல்லையெனில் எனில் (*p == 'r' && p[1] == 0)
	{
	    முழு		    serial, gotSerial;
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
	    enc = வெற்று;
	    போது ((long_u)(p - propInfo) < இலக்கம்_பொருட்கள் && *p == '-')
	    {
		தேர்வு (p[1])
		{
		    நிலை 'r':
			எனில் (p[2] == ' ')
			    res = p + 3;
			நிறுத்து;
		    நிலை 'E':
			எனில் (p[2] == ' ')
			    enc = p + 3;
			நிறுத்து;
		    நிலை 's':
			எனில் (sscanf((எழுத்து *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			நிறுத்து;
		    நிலை 'c':
			எனில் (sscanf((எழுத்து *)p + 2, " %d", &code) != 1)
			    code = 0;
			நிறுத்து;
		}
		போது (*p != 0)
		    p++;
		p++;
	    }

	    எனில் (!gotSerial)
		தொடர்;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    க்கு (pcPtr = pendingCommands; pcPtr != வெற்று; pcPtr = pcPtr->nextPtr)
	    {
		எனில் (serial != pcPtr->serial || pcPtr->result != வெற்று)
		    தொடர்;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		எனில் (tofree == வெற்று)
		    res = vim_strsave(res);
		pcPtr->result = res;
		நிறுத்து;
	    }
	}
	இல்லையெனில் எனில் (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    unsigned முழு u;
	    முழு		gotWindow;
	    char_u	*str;
	    கட்டமைப்பு	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = வெற்று;
	    போது ((long_u)(p - propInfo) < இலக்கம்_பொருட்கள் && *p == '-')
	    {
		தேர்வு (p[1])
		{
		    நிலை 'n':
			எனில் (p[2] == ' ')
			    str = p + 3;
			நிறுத்து;
		    நிலை 'E':
			எனில் (p[2] == ' ')
			    enc = p + 3;
			நிறுத்து;
		    நிலை 'w':
			எனில் (sscanf((எழுத்து *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			நிறுத்து;
		}
		போது (*p != 0)
		    p++;
		p++;
	    }

	    எனில் (!gotWindow)
		தொடர்;
	    str = serverConvert(enc, str, &tofree);
	    எனில் ((r = ServerReplyFind(win, SROP_Add)) != வெற்று)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((எழுத்து *)winstr, "0x%x", (unsigned முழு)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	இல்லையெனில்
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    போது (*p != 0)
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
    நிலையான முழு
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*மதிப்பு,		// Characters  to append to property.
    முழு		length)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, மதிப்பு, length);
    XSync(dpy, False);
    (வெறுமை) XSetErrorHandler(old_handler);
    திரும்பு got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    நிலையான முழு
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    திரும்பு 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    நிலையான முழு
IsSerialName(char_u *str)
{
    முழு len = STRLEN(str);

    திரும்பு (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
