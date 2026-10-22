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

#include "vim.h"
#include "version.h"

#ប្រសិនបើ (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

typedef sa_nracanaa PendingCommand
{
    ចំនួនគត់	    serial;	// Serial number expected in result.
    ចំនួនគត់	    code;	// Result Code. 0 is OK
    char_u  *result;	// String result for command (malloc'ed).
			// NULL means command still pending.
    sa_nracanaa PendingCommand *nextPtr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

static PendingCommand *pendingCommands = NULL;
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

sa_nracanaa ServerReply
{
    Window  id;
    garray_T strings;
};
static garray_T serverReply = { 0, 0, 0, 0, 0 };
ga_nanaa ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

typedef ចំនួនគត់ (*EndCond)(void *);

sa_nracanaa x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    sa_nracanaa x_cmdqueue	*next;
    sa_nracanaa x_cmdqueue	*prev;
};

typedef sa_nracanaa x_cmdqueue x_queue_T;

// dummy node, header for circular queue
static x_queue_T head = {NULL, 0, NULL, NULL};

/*
 * Forward declarations for procedures defined later in this file:
 */

static Window	LookupName(Display *dpy, char_u *name, ចំនួនគត់ delete, char_u **loose);
static ចំនួនគត់	SendInit(Display *dpy);
static ចំនួនគត់	DoRegisterName(Display *dpy, char_u *name);
static void	DeleteAnyLingerer(Display *dpy, Window w);
static ចំនួនគត់	GetRegProp(Display *dpy, char_u **regPropp, long_u *numItemsp, ចំនួនគត់ domsg);
static ចំនួនគត់	WaitForPend(void *p);
static ចំនួនគត់	WindowValid(Display *dpy, Window w);
static void	ServerWait(Display *dpy, Window w, EndCond endCond, void *endData, ចំនួនគត់ localLoop, ចំនួនគត់ seconds);
static ចំនួនគត់	AppendPropCarefully(Display *display, Window window, Atom property, char_u *value, ចំនួនគត់ length);
static ចំនួនគត់	x_error_check(Display *dpy, XErrorEvent *error_event);
static ចំនួនគត់	IsSerialName(char_u *name);
static void	save_in_queue(char_u *buf, long_u len);
static void	server_parse_message(Display *dpy, char_u *propInfo, long_u numItems);

// Private variables for the "server" functionality
static Atom	registryProperty = None;
static Atom	vimProperty = None;
static ចំនួនគត់	got_x_error = FALSE;

static char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    ចំនួនគត់
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    ចំនួនគត់		i;
    ចំនួនគត់		res;
    char_u	*p = NULL;

    res = DoRegisterName(dpy, name);
    ប្រសិនបើ (res >= 0)
	ត្រឡប់ OK;

    i = 1;
    do
    {
	ប្រសិនបើ (res < -1 || i >= 1000)
	{
	    msg_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    ត្រឡប់ FAIL;
	}
	ប្រសិនបើ (p == NULL)
	    p = alloc(STRLEN(name) + 10);
	ប្រសិនបើ (p == NULL)
	{
	    res = -10;
	    continue;
	}
	sprintf((var_na *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    ខណៈពេល (res < 0)
	;
    vim_free(p);

    ត្រឡប់ OK;
}

    static ចំនួនគត់
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    ប្រសិនបើ (commProperty == None)
    {
	ប្រសិនបើ (SendInit(dpy) < 0)
	    ត្រឡប់ -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, NULL);
    ប្រសិនបើ (w != (Window)0)
    {
	Status		status;
	ចំនួនគត់		dummyInt;
	unsigned ចំនួនគត់	dummyUns;
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
	(void)XSetErrorHandler(old_handler);
	ប្រសិនបើ (status != Success && w != commWindow)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    ត្រឡប់ -1;
	}
	(void)LookupName(dpy, name, /*delete=*/TRUE, NULL);
    }
    sprintf((var_na *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (void)XSetErrorHandler(old_handler);

    ប្រសិនបើ (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, (ចំនួនគត់)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	ត្រឡប់ 0;
    }
    ត្រឡប់ -2;
}

# ប្រសិនបើ defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    void
serverChangeRegisteredWindow(
    Display	*dpy,		// Display to register with
    Window	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    commWindow = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    ប្រសិនបើ (SendInit(dpy) < 0)
	ត្រឡប់;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    ប្រសិនបើ (serverName != NULL)
    {
	// Reinsert name if it was already registered
	(void)LookupName(dpy, serverName, /*delete=*/TRUE, NULL);
	sprintf((var_na *)propInfo, "%x %.*s",
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
    ចំនួនគត់
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**result,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	asExpr,			// Interpret as keystrokes or expr ?
    ចំនួនគត់		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    ចំនួនគត់		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    ចំនួនគត់		    length;
    ចំនួនគត់		    res;
    static ចំនួនគត់	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = NULL;

    ប្រសិនបើ (result != NULL)
	*result = NULL;
    ប្រសិនបើ (name == NULL || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    ប្រសិនបើ (commProperty == None && dpy != NULL && SendInit(dpy) < 0)
	ត្រឡប់ -1;

# ប្រសិនបើ defined(FEAT_EVAL)
    ch_log(NULL, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    ប្រសិនបើ (dpy == NULL || (serverName != NULL && STRICMP(name, serverName) == 0))
	ត្រឡប់ sendToLocalVim(cmd, asExpr, result);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    ខណៈពេល (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	ប្រសិនបើ (w != None)
	{
	    ប្រសិនបើ (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, NULL);
		VIM_CLEAR(loosename);
		continue;
	    }
	}
	bha_nga;
    }
    ប្រសិនបើ (w == None)
    {
	ប្រសិនបើ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ត្រឡប់ -1;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (loosename != NULL)
	name = loosename;
    ប្រសិនបើ (server != NULL)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    length = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(length + 30);

    sprintf((var_na *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, asExpr ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    ប្រសិនបើ (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((var_na *)property + length, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    length += STRLEN(property + length + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    vim_free(property);
    ប្រសិនបើ (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	ត្រឡប់ -1;
    }

    ប្រសិនបើ (!asExpr) // There is no answer for this - Keys are sent async
	ត្រឡប់ 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.code = 0;
    pending.result = NULL;
    pending.nextPtr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    ប្រសិនបើ (pendingCommands == &pending)
	pendingCommands = pending.nextPtr;
    បើមិនដូច្នេះ
    {
	PendingCommand *pcPtr;

	សម្រាប់ (pcPtr = pendingCommands; pcPtr != NULL; pcPtr = pcPtr->nextPtr)
	    ប្រសិនបើ (pcPtr->nextPtr == &pending)
	    {
		pcPtr->nextPtr = pending.nextPtr;
		bha_nga;
	    }
    }

# ប្រសិនបើ defined(FEAT_EVAL)
    ch_log(NULL, "serverSendToVim() result: %s",
	    pending.result == NULL ? "NULL" : (var_na *)pending.result);
# endif
    ប្រសិនបើ (result != NULL)
	*result = pending.result;
    បើមិនដូច្នេះ
	vim_free(pending.result);

    ត្រឡប់ pending.code == 0 ? 0 : -1;
}

    static ចំនួនគត់
WaitForPend(void *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    ត្រឡប់ pending->result != NULL;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    static ចំនួនគត់
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    ចំនួនគត់		    numProp;
    ចំនួនគត់		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &numProp);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    ប្រសិនបើ (plist == NULL || got_x_error)
	ត្រឡប់ FALSE;

    សម្រាប់ (i = 0; i < numProp; i++)
	ប្រសិនបើ (plist[i] == vimProperty)
	{
	    XFree(plist);
	    ត្រឡប់ TRUE;
	}
    XFree(plist);
    ត្រឡប់ FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    static void
ServerWait(
    Display	*dpy,
    Window	w,
    EndCond	endCond,
    void	*endData,
    ចំនួនគត់		localLoop,
    ចំនួនគត់		seconds)
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
# បើមិនដូច្នេះ
    sa_nracanaa pollfd   fds;

    fds.fd = ConnectionNumber(dpy);
    fds.events = POLLIN;
# endif

    time(&start);
    ខណៈពេល (TRUE)
    {
	ខណៈពេល (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	ប្រសិនបើ (endCond(endData) != 0)
	    bha_nga;
	ប្រសិនបើ (!WindowValid(dpy, w))
	    bha_nga;
	time(&now);
	ប្រសិនបើ (seconds >= 0 && (now - start) >= seconds)
	    bha_nga;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	ប្រសិនបើ (localLoop)
	{
# ifdef HAVE_SELECT
	    sa_nracanaa timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    ប្រសិនបើ (select(FD_SETSIZE, &fds, NULL, NULL, &tv) < 0)
		bha_nga;
# បើមិនដូច្នេះ
	    ប្រសិនបើ (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		bha_nga;
# endif
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (got_int)
		bha_nga;
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

    ប្រសិនបើ (registryProperty == None)
    {
	ប្រសិនបើ (SendInit(dpy) < 0)
	    ត្រឡប់ NULL;
    }

    list = list_alloc();
    ប្រសិនបើ (list == NULL)
	ត្រឡប់ NULL;

    /*
     * Read the registry property.
     */
    ប្រសិនបើ (GetRegProp(dpy, &regProp, &numItems, TRUE) == FAIL)
	ត្រឡប់ NULL;

    /*
     * Scan all of the names out of the property.
     */
    សម្រាប់ (p = regProp; (long_u)(p - regProp) < numItems; p++)
    {
	entry = p;
	ខណៈពេល (*p != 0 && !SAFE_isspace(*p))
	    p++;
	ប្រសិនបើ (*p != 0)
	{
	    w = None;
	    sscanf((var_na *)entry, "%x", &w);
	    ប្រសិនបើ (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    ខណៈពេល (*p != 0)
		p++;
	}
    }
    ប្រសិនបើ (regProp != empty_prop)
	XFree(regProp);
    ត្រឡប់ list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    static sa_nracanaa ServerReply *
ServerReplyFind(Window w, ga_nanaa ServerReplyOp op)
{
    sa_nracanaa ServerReply *p;
    sa_nracanaa ServerReply e;
    ចំនួនគត់		i;

    p = (sa_nracanaa ServerReply *) serverReply.ga_data;
    សម្រាប់ (i = 0; i < serverReply.ga_len; i++, p++)
	ប្រសិនបើ (p->id == w)
	    bha_nga;
    ប្រសិនបើ (i >= serverReply.ga_len)
	p = NULL;

    ប្រសិនបើ (p == NULL && op == SROP_Add)
    {
	ប្រសិនបើ (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, parimaa_na(sa_nracanaa ServerReply), 1);
	ប្រសិនបើ (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((sa_nracanaa ServerReply *) serverReply.ga_data)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, parimaa_na(e));
	    serverReply.ga_len++;
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (p != NULL && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * parimaa_na(*p));
	serverReply.ga_len--;
    }

    ត្រឡប់ p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    unsigned  id = None;

    sscanf((var_na *)str, "0x%x", &id);
    ប្រសិនបើ (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    ត្រឡប់ (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    ចំនួនគត់
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    ចំនួនគត់		length;
    ចំនួនគត់		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    ប្រសិនបើ (commProperty == None)
    {
	ប្រសិនបើ (SendInit(dpy) < 0)
	    ត្រឡប់ -2;
    }
    ប្រសិនបើ (!WindowValid(dpy, win))
	ត្រឡប់ -1;

    length = STRLEN(p_enc) + STRLEN(str) + 14;
    ប្រសិនបើ ((property = alloc(length + 30)) == NULL)
	ត្រឡប់ -1;

    sprintf((var_na *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (unsigned ចំនួនគត់)commWindow);
    // Add length of what "%x" resulted in.
    length += STRLEN(property + length);
    res = AppendPropCarefully(dpy, win, commProperty, property, length + 1);
    vim_free(property);

    ត្រឡប់ res;
}

    static ចំនួនគត់
WaitForReply(void *p)
{
    Window  *w = (Window *) p;

    ត្រឡប់ ServerReplyFind(*w, SROP_Find) != NULL;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    ចំនួនគត់
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    ចំនួនគត់		localLoop,
    ចំនួនគត់		timeout)
{
    ចំនួនគត់		len;
    char_u	*s;
    sa_nracanaa	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    ប្រសិនបើ ((p = ServerReplyFind(win, SROP_Find)) != NULL && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_data);
	len = STRLEN(*str) + 1;
	ប្រសិនបើ (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_data;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	បើមិនដូច្នេះ
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	ត្រឡប់ 0;
    }
    ត្រឡប់ -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    ចំនួនគត់
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    sa_nracanaa ServerReply *p;

    ប្រសិនបើ ((p = ServerReplyFind(win, SROP_Find)) != NULL && p->strings.ga_len > 0)
    {
	ប្រសិនបើ (str != NULL)
	    *str = p->strings.ga_data;
	ត្រឡប់ 1;
    }
    ប្រសិនបើ (!WindowValid(dpy, win))
	ត្រឡប់ -1;
    ត្រឡប់ 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    static ចំនួនគត់
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    ប្រសិនបើ (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    ប្រសិនបើ (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    ប្រសិនបើ (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    ប្រសិនបើ (commWindow == None)
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
			(ចំនួនគត់)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (void)XSetErrorHandler(old_handler);

    ត្រឡប់ got_x_error ? -1 : 0;
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
    static Window
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    ចំនួនគត់		delete,	    // If non-zero, delete info about name.
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
    ប្រសិនបើ (GetRegProp(dpy, &regProp, &numItems, FALSE) == FAIL)
	ត្រឡប់ 0;

    /*
     * Scan the property for the desired name.
     */
    returnValue = (int_u)None;
    entry = NULL;	// Not needed, but eliminates compiler warning.
    សម្រាប់ (p = regProp; (long_u)(p - regProp) < numItems; )
    {
	entry = p;
	ខណៈពេល (*p != 0 && !SAFE_isspace(*p))
	    p++;
	ប្រសិនបើ (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    sscanf((var_na *)entry, "%x", &returnValue);
	    bha_nga;
	}
	ខណៈពេល (*p != 0)
	    p++;
	p++;
    }

    ប្រសិនបើ (loose != NULL && returnValue == (int_u)None && !IsSerialName(name))
    {
	សម្រាប់ (p = regProp; (long_u)(p - regProp) < numItems; )
	{
	    entry = p;
	    ខណៈពេល (*p != 0 && !SAFE_isspace(*p))
		p++;
	    ប្រសិនបើ (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		ប្រសិនបើ (sscanf((var_na *)entry, "%x", &returnValue) == 1
			&& returnValue != (int_u)None)
		    *loose = vim_strsave(p + 1);
		bha_nga;
	    }
	    ខណៈពេល (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    ប្រសិនបើ (delete && returnValue != (int_u)None)
    {
	ចំនួនគត់ count;

	ខណៈពេល (*p != 0)
	    p++;
	p++;
	count = numItems - (p - regProp);
	ប្រសិនបើ (count > 0)
	    mch_memmove(entry, p, count);
	XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(ចំនួនគត់)(numItems - (p - entry)));
	XSync(dpy, False);
    }

    ប្រសិនបើ (regProp != empty_prop)
	XFree(regProp);
    ត្រឡប់ (Window)returnValue;
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
    static void
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    Window	win)	// Window to remove
{
    char_u	*regProp, *entry = NULL;
    char_u	*p;
    long_u	numItems;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    ប្រសិនបើ (GetRegProp(dpy, &regProp, &numItems, FALSE) == FAIL)
	ត្រឡប់;

    // Scan the property for the window id.
    សម្រាប់ (p = regProp; (long_u)(p - regProp) < numItems; )
    {
	ប្រសិនបើ (*p != 0)
	{
	    sscanf((var_na *)p, "%x", &wwin);
	    ប្រសិនបើ ((Window)wwin == win)
	    {
		ចំនួនគត់ lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		ខណៈពេល (*p != 0)
		    p++;
		p++;
		lastHalf = numItems - (p - regProp);
		ប្រសិនបើ (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		numItems = (entry - regProp) + lastHalf;
		p = entry;
		continue;
	    }
	}
	ខណៈពេល (*p != 0)
	    p++;
	p++;
    }

    ប្រសិនបើ (entry != NULL)
    {
	XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(ចំនួនគត់)(p - regProp));
	XSync(dpy, False);
    }

    ប្រសិនបើ (regProp != empty_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    static ចំនួនគត់
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*numItemsp,
    ចំនួនគត់		domsg)		// When TRUE give error message.
{
    ចំនួនគត់		result, actualFormat;
    long_u	bytesAfter;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = NULL;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    result = XGetWindowProperty(dpy, RootWindow(dpy, 0), registryProperty, 0L,
				(long)MAX_PROP_WORDS, False,
				XA_STRING, &actualType,
				&actualFormat, numItemsp, &bytesAfter,
				regPropp);

    XSync(dpy, FALSE);
    (void)XSetErrorHandler(old_handler);
    ប្រសិនបើ (got_x_error)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*numItemsp = 0;
	*regPropp = empty_prop;
	ត្រឡប់ OK;
    }

    // If the property is improperly formed, then delete it.
    ប្រសិនបើ (result != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	ប្រសិនបើ (*regPropp != NULL)
	    XFree(*regPropp);
	XDeleteProperty(dpy, RootWindow(dpy, 0), registryProperty);
	ប្រសិនបើ (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    void
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    ចំនួនគត់		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    ចំនួនគត់		result, actualFormat;
    long_u	numItems, bytesAfter;
    Atom	actualType;

    ប្រសិនបើ (eventPtr != NULL)
    {
	ប្រសិនបើ (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != PropertyNewValue)
	    ត្រឡប់;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = NULL;
    result = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(long)MAX_PROP_WORDS, True,
				XA_STRING, &actualType,
				&actualFormat, &numItems, &bytesAfter,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    ប្រសិនបើ (result != Success || actualType != XA_STRING || actualFormat != 8)
    {
	ប្រសិនបើ (propInfo != NULL)
	    XFree(propInfo);
	ត្រឡប់;
    }
    ប្រសិនបើ (immediate)
	server_parse_message(dpy, propInfo, numItems);
    បើមិនដូច្នេះ
	save_in_queue(propInfo, numItems);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    static void
save_in_queue(char_u *propInfo, long_u len)
{
    x_queue_T *node;

    node = ALLOC_ONE(x_queue_T);
    ប្រសិនបើ (node == NULL)
	ត្រឡប់;	    // out of memory
    node->propInfo = propInfo;
    node->len = len;

    ប្រសិនបើ (head.next == NULL)   // initialize circular queue
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
    void
server_parse_messages(void)
{
    x_queue_T	*node;

    ប្រសិនបើ (!X_DISPLAY)
	ត្រឡប់; // cannot happen?
    ខណៈពេល (head.next != NULL && head.next != &head)
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
    ចំនួនគត់
server_waiting(void)
{
    ត្រឡប់ head.next != NULL && head.next != &head;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    static void
server_parse_message(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	numItems)  // The size of propInfo in bytes.
{
    char_u	*p;
    ចំនួនគត់		code;
    char_u	*tofree;

# ប្រសិនបើ defined(FEAT_EVAL)
    ch_log(NULL, "server_parse_message() numItems: %ld", numItems);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    សម្រាប់ (p = propInfo; (long_u)(p - propInfo) < numItems; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	ប្រសិនបើ (*p == 0)
	{
	    p++;
	    continue;
	}

	ប្រសិនបើ ((*p == 'c' || *p == 'k') && p[1] == 0)
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
	    name = NULL;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = NULL;
	    enc = NULL;
	    ខណៈពេល ((long_u)(p - propInfo) < numItems && *p == '-')
	    {
# ប្រសិនបើ defined(FEAT_EVAL)
		ch_log(NULL, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		switch (p[1])
		{
		    pak_sa 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			ខណៈពេល (vim_isxdigit(*end))
			{
			    resWindow = 16 * resWindow + (long_u)hex2nr(*end);
			    ++end;
			}
			ប្រសិនបើ (end == p + 2 || *end != ' ')
			    resWindow = None;
			បើមិនដូច្នេះ
			{
			    p = serial = end + 1;
			    clientWindow = resWindow; // Remember in global
			}
			bha_nga;
		    pak_sa 'n':
			ប្រសិនបើ (p[2] == ' ')
			    name = p + 3;
			bha_nga;
		    pak_sa 's':
			ប្រសិនបើ (p[2] == ' ')
			    script = p + 3;
			bha_nga;
		    pak_sa 'E':
			ប្រសិនបើ (p[2] == ' ')
			    enc = p + 3;
			bha_nga;
		}
		ខណៈពេល (*p != 0)
		    p++;
		p++;
	    }

	    ប្រសិនបើ (script == NULL || name == NULL)
		continue;

	    ប្រសិនបើ (serverName != NULL && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		ប្រសិនបើ (asKeys)
		    server_to_input_buf(script);
		បើមិនដូច្នេះ
		{
		    char_u      *res;

		    res = eval_client_expr_to_string(script);
		    ប្រសិនបើ (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(void)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(reply.ga_data, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			ប្រសិនបើ (res != NULL)
			    ga_concat(&reply, res);
			បើមិនដូច្នេះ
			{
			    ga_concat(&reply,
				   (char_u *)_(e_invalid_expression_received));
			    ga_append(&reply, 0);
			    GA_CONCAT_LITERAL(&reply, "-c 1");
			}
			ga_append(&reply, NUL);
			(void)AppendPropCarefully(dpy, resWindow, commProperty,
						 reply.ga_data, reply.ga_len);
			ga_clear(&reply);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	បើមិនដូច្នេះ ប្រសិនបើ (*p == 'r' && p[1] == 0)
	{
	    ចំនួនគត់		    serial, gotSerial;
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
	    enc = NULL;
	    ខណៈពេល ((long_u)(p - propInfo) < numItems && *p == '-')
	    {
		switch (p[1])
		{
		    pak_sa 'r':
			ប្រសិនបើ (p[2] == ' ')
			    res = p + 3;
			bha_nga;
		    pak_sa 'E':
			ប្រសិនបើ (p[2] == ' ')
			    enc = p + 3;
			bha_nga;
		    pak_sa 's':
			ប្រសិនបើ (sscanf((var_na *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			bha_nga;
		    pak_sa 'c':
			ប្រសិនបើ (sscanf((var_na *)p + 2, " %d", &code) != 1)
			    code = 0;
			bha_nga;
		}
		ខណៈពេល (*p != 0)
		    p++;
		p++;
	    }

	    ប្រសិនបើ (!gotSerial)
		continue;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    សម្រាប់ (pcPtr = pendingCommands; pcPtr != NULL; pcPtr = pcPtr->nextPtr)
	    {
		ប្រសិនបើ (serial != pcPtr->serial || pcPtr->result != NULL)
		    continue;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		ប្រសិនបើ (tofree == NULL)
		    res = vim_strsave(res);
		pcPtr->result = res;
		bha_nga;
	    }
	}
	បើមិនដូច្នេះ ប្រសិនបើ (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    unsigned ចំនួនគត់ u;
	    ចំនួនគត់		gotWindow;
	    char_u	*str;
	    sa_nracanaa	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = NULL;
	    ខណៈពេល ((long_u)(p - propInfo) < numItems && *p == '-')
	    {
		switch (p[1])
		{
		    pak_sa 'n':
			ប្រសិនបើ (p[2] == ' ')
			    str = p + 3;
			bha_nga;
		    pak_sa 'E':
			ប្រសិនបើ (p[2] == ' ')
			    enc = p + 3;
			bha_nga;
		    pak_sa 'w':
			ប្រសិនបើ (sscanf((var_na *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			bha_nga;
		}
		ខណៈពេល (*p != 0)
		    p++;
		p++;
	    }

	    ប្រសិនបើ (!gotWindow)
		continue;
	    str = serverConvert(enc, str, &tofree);
	    ប្រសិនបើ ((r = ServerReplyFind(win, SROP_Add)) != NULL)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((var_na *)winstr, "0x%x", (unsigned ចំនួនគត់)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	បើមិនដូច្នេះ
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    ខណៈពេល (*p != 0)
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
    static ចំនួនគត់
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*value,		// Characters  to append to property.
    ចំនួនគត់		length)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, value, length);
    XSync(dpy, False);
    (void) XSetErrorHandler(old_handler);
    ត្រឡប់ got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    static ចំនួនគត់
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    ត្រឡប់ 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    static ចំនួនគត់
IsSerialName(char_u *str)
{
    ចំនួនគត់ len = STRLEN(str);

    ត្រឡប់ (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
