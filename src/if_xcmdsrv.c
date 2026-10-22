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

#如果 (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

typedef struct PendingCommand
{
    整数	    serial;	// Serial number expected in result.
    整数	    code;	// Result Code. 0 is OK
    char_u  *result;	// String result for command (malloc'ed).
			// NULL means command still pending.
    struct PendingCommand *nextPtr;
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

struct ServerReply
{
    Window  id;
    garray_T strings;
};
static garray_T serverReply = { 0, 0, 0, 0, 0 };
enum ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

typedef 整数 (*EndCond)(void *);

struct x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    struct x_cmdqueue	*next;
    struct x_cmdqueue	*prev;
};

typedef struct x_cmdqueue x_queue_T;

// dummy node, header for circular queue
static x_queue_T head = {NULL, 0, NULL, NULL};

/*
 * Forward declarations for procedures defined later in this file:
 */

static Window	LookupName(Display *dpy, char_u *name, 整数 delete, char_u **loose);
static 整数	SendInit(Display *dpy);
static 整数	DoRegisterName(Display *dpy, char_u *name);
static void	DeleteAnyLingerer(Display *dpy, Window w);
static 整数	GetRegProp(Display *dpy, char_u **regPropp, long_u *numItemsp, 整数 domsg);
static 整数	WaitForPend(void *p);
static 整数	WindowValid(Display *dpy, Window w);
static void	ServerWait(Display *dpy, Window w, EndCond endCond, void *endData, 整数 localLoop, 整数 seconds);
static 整数	AppendPropCarefully(Display *display, Window window, Atom property, char_u *value, 整数 length);
static 整数	x_error_check(Display *dpy, XErrorEvent *error_event);
static 整数	IsSerialName(char_u *name);
static void	save_in_queue(char_u *buf, long_u len);
static void	server_parse_message(Display *dpy, char_u *propInfo, long_u numItems);

// Private variables for the "server" functionality
static Atom	registryProperty = None;
static Atom	vimProperty = None;
static 整数	got_x_error = FALSE;

static char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    整数
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    整数		i;
    整数		res;
    char_u	*p = NULL;

    res = DoRegisterName(dpy, name);
    如果 (res >= 0)
	返回 OK;

    i = 1;
    do
    {
	如果 (res < -1 || i >= 1000)
	{
	    msg_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    返回 FAIL;
	}
	如果 (p == NULL)
	    p = alloc(STRLEN(name) + 10);
	如果 (p == NULL)
	{
	    res = -10;
	    continue;
	}
	sprintf((char *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    当 (res < 0)
	;
    vim_free(p);

    返回 OK;
}

    static 整数
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    如果 (commProperty == None)
    {
	如果 (SendInit(dpy) < 0)
	    返回 -2;
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
    如果 (w != (Window)0)
    {
	Status		status;
	整数		dummyInt;
	unsigned 整数	dummyUns;
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
	如果 (status != Success && w != commWindow)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    返回 -1;
	}
	(void)LookupName(dpy, name, /*delete=*/TRUE, NULL);
    }
    sprintf((char *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (void)XSetErrorHandler(old_handler);

    如果 (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, (整数)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	返回 0;
    }
    返回 -2;
}

# 如果 defined(FEAT_GUI)
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
    如果 (SendInit(dpy) < 0)
	返回;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    如果 (serverName != NULL)
    {
	// Reinsert name if it was already registered
	(void)LookupName(dpy, serverName, /*delete=*/TRUE, NULL);
	sprintf((char *)propInfo, "%x %.*s",
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
    整数
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**result,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	asExpr,			// Interpret as keystrokes or expr ?
    整数		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    整数		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    整数		    length;
    整数		    res;
    static 整数	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = NULL;

    如果 (result != NULL)
	*result = NULL;
    如果 (name == NULL || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    如果 (commProperty == None && dpy != NULL && SendInit(dpy) < 0)
	返回 -1;

# 如果 defined(FEAT_EVAL)
    ch_log(NULL, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    如果 (dpy == NULL || (serverName != NULL && STRICMP(name, serverName) == 0))
	返回 sendToLocalVim(cmd, asExpr, result);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    当 (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	如果 (w != None)
	{
	    如果 (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, NULL);
		VIM_CLEAR(loosename);
		continue;
	    }
	}
	break;
    }
    如果 (w == None)
    {
	如果 (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	返回 -1;
    }
    否则 如果 (loosename != NULL)
	name = loosename;
    如果 (server != NULL)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    length = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(length + 30);

    sprintf((char *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, asExpr ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    如果 (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((char *)property + length, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    length += STRLEN(property + length + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    vim_free(property);
    如果 (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	返回 -1;
    }

    如果 (!asExpr) // There is no answer for this - Keys are sent async
	返回 0;

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
    如果 (pendingCommands == &pending)
	pendingCommands = pending.nextPtr;
    否则
    {
	PendingCommand *pcPtr;

	为 (pcPtr = pendingCommands; pcPtr != NULL; pcPtr = pcPtr->nextPtr)
	    如果 (pcPtr->nextPtr == &pending)
	    {
		pcPtr->nextPtr = pending.nextPtr;
		break;
	    }
    }

# 如果 defined(FEAT_EVAL)
    ch_log(NULL, "serverSendToVim() result: %s",
	    pending.result == NULL ? "NULL" : (char *)pending.result);
# endif
    如果 (result != NULL)
	*result = pending.result;
    否则
	vim_free(pending.result);

    返回 pending.code == 0 ? 0 : -1;
}

    static 整数
WaitForPend(void *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    返回 pending->result != NULL;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    static 整数
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    整数		    numProp;
    整数		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &numProp);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    如果 (plist == NULL || got_x_error)
	返回 FALSE;

    为 (i = 0; i < numProp; i++)
	如果 (plist[i] == vimProperty)
	{
	    XFree(plist);
	    返回 TRUE;
	}
    XFree(plist);
    返回 FALSE;
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
    整数		localLoop,
    整数		seconds)
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
# 否则
    struct pollfd   fds;

    fds.fd = ConnectionNumber(dpy);
    fds.events = POLLIN;
# endif

    time(&start);
    当 (TRUE)
    {
	当 (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	如果 (endCond(endData) != 0)
	    break;
	如果 (!WindowValid(dpy, w))
	    break;
	time(&now);
	如果 (seconds >= 0 && (now - start) >= seconds)
	    break;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	如果 (localLoop)
	{
# ifdef HAVE_SELECT
	    struct timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    如果 (select(FD_SETSIZE, &fds, NULL, NULL, &tv) < 0)
		break;
# 否则
	    如果 (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		break;
# endif
	}
	否则
	{
	    如果 (got_int)
		break;
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

    如果 (registryProperty == None)
    {
	如果 (SendInit(dpy) < 0)
	    返回 NULL;
    }

    list = list_alloc();
    如果 (list == NULL)
	返回 NULL;

    /*
     * Read the registry property.
     */
    如果 (GetRegProp(dpy, &regProp, &numItems, TRUE) == FAIL)
	返回 NULL;

    /*
     * Scan all of the names out of the property.
     */
    为 (p = regProp; (long_u)(p - regProp) < numItems; p++)
    {
	entry = p;
	当 (*p != 0 && !SAFE_isspace(*p))
	    p++;
	如果 (*p != 0)
	{
	    w = None;
	    sscanf((char *)entry, "%x", &w);
	    如果 (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    当 (*p != 0)
		p++;
	}
    }
    如果 (regProp != empty_prop)
	XFree(regProp);
    返回 list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    static struct ServerReply *
ServerReplyFind(Window w, enum ServerReplyOp op)
{
    struct ServerReply *p;
    struct ServerReply e;
    整数		i;

    p = (struct ServerReply *) serverReply.ga_data;
    为 (i = 0; i < serverReply.ga_len; i++, p++)
	如果 (p->id == w)
	    break;
    如果 (i >= serverReply.ga_len)
	p = NULL;

    如果 (p == NULL && op == SROP_Add)
    {
	如果 (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, sizeof(struct ServerReply), 1);
	如果 (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((struct ServerReply *) serverReply.ga_data)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, sizeof(e));
	    serverReply.ga_len++;
	}
    }
    否则 如果 (p != NULL && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * sizeof(*p));
	serverReply.ga_len--;
    }

    返回 p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    unsigned  id = None;

    sscanf((char *)str, "0x%x", &id);
    如果 (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    返回 (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    整数
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    整数		length;
    整数		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    如果 (commProperty == None)
    {
	如果 (SendInit(dpy) < 0)
	    返回 -2;
    }
    如果 (!WindowValid(dpy, win))
	返回 -1;

    length = STRLEN(p_enc) + STRLEN(str) + 14;
    如果 ((property = alloc(length + 30)) == NULL)
	返回 -1;

    sprintf((char *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (unsigned 整数)commWindow);
    // Add length of what "%x" resulted in.
    length += STRLEN(property + length);
    res = AppendPropCarefully(dpy, win, commProperty, property, length + 1);
    vim_free(property);

    返回 res;
}

    static 整数
WaitForReply(void *p)
{
    Window  *w = (Window *) p;

    返回 ServerReplyFind(*w, SROP_Find) != NULL;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    整数
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    整数		localLoop,
    整数		timeout)
{
    整数		len;
    char_u	*s;
    struct	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    如果 ((p = ServerReplyFind(win, SROP_Find)) != NULL && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_data);
	len = STRLEN(*str) + 1;
	如果 (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_data;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	否则
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	返回 0;
    }
    返回 -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    整数
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    struct ServerReply *p;

    如果 ((p = ServerReplyFind(win, SROP_Find)) != NULL && p->strings.ga_len > 0)
    {
	如果 (str != NULL)
	    *str = p->strings.ga_data;
	返回 1;
    }
    如果 (!WindowValid(dpy, win))
	返回 -1;
    返回 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    static 整数
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    如果 (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    如果 (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    如果 (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    如果 (commWindow == None)
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
			(整数)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (void)XSetErrorHandler(old_handler);

    返回 got_x_error ? -1 : 0;
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
    整数		delete,	    // If non-zero, delete info about name.
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
    如果 (GetRegProp(dpy, &regProp, &numItems, FALSE) == FAIL)
	返回 0;

    /*
     * Scan the property for the desired name.
     */
    returnValue = (int_u)None;
    entry = NULL;	// Not needed, but eliminates compiler warning.
    为 (p = regProp; (long_u)(p - regProp) < numItems; )
    {
	entry = p;
	当 (*p != 0 && !SAFE_isspace(*p))
	    p++;
	如果 (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    sscanf((char *)entry, "%x", &returnValue);
	    break;
	}
	当 (*p != 0)
	    p++;
	p++;
    }

    如果 (loose != NULL && returnValue == (int_u)None && !IsSerialName(name))
    {
	为 (p = regProp; (long_u)(p - regProp) < numItems; )
	{
	    entry = p;
	    当 (*p != 0 && !SAFE_isspace(*p))
		p++;
	    如果 (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		如果 (sscanf((char *)entry, "%x", &returnValue) == 1
			&& returnValue != (int_u)None)
		    *loose = vim_strsave(p + 1);
		break;
	    }
	    当 (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    如果 (delete && returnValue != (int_u)None)
    {
	整数 count;

	当 (*p != 0)
	    p++;
	p++;
	count = numItems - (p - regProp);
	如果 (count > 0)
	    mch_memmove(entry, p, count);
	XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(整数)(numItems - (p - entry)));
	XSync(dpy, False);
    }

    如果 (regProp != empty_prop)
	XFree(regProp);
    返回 (Window)returnValue;
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
    如果 (GetRegProp(dpy, &regProp, &numItems, FALSE) == FAIL)
	返回;

    // Scan the property for the window id.
    为 (p = regProp; (long_u)(p - regProp) < numItems; )
    {
	如果 (*p != 0)
	{
	    sscanf((char *)p, "%x", &wwin);
	    如果 ((Window)wwin == win)
	    {
		整数 lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		当 (*p != 0)
		    p++;
		p++;
		lastHalf = numItems - (p - regProp);
		如果 (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		numItems = (entry - regProp) + lastHalf;
		p = entry;
		continue;
	    }
	}
	当 (*p != 0)
	    p++;
	p++;
    }

    如果 (entry != NULL)
    {
	XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(整数)(p - regProp));
	XSync(dpy, False);
    }

    如果 (regProp != empty_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    static 整数
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*numItemsp,
    整数		domsg)		// When TRUE give error message.
{
    整数		result, actualFormat;
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
    如果 (got_x_error)
	返回 FAIL;

    如果 (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*numItemsp = 0;
	*regPropp = empty_prop;
	返回 OK;
    }

    // If the property is improperly formed, then delete it.
    如果 (result != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	如果 (*regPropp != NULL)
	    XFree(*regPropp);
	XDeleteProperty(dpy, RootWindow(dpy, 0), registryProperty);
	如果 (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	返回 FAIL;
    }
    返回 OK;
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
    整数		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    整数		result, actualFormat;
    long_u	numItems, bytesAfter;
    Atom	actualType;

    如果 (eventPtr != NULL)
    {
	如果 (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != PropertyNewValue)
	    返回;
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
    如果 (result != Success || actualType != XA_STRING || actualFormat != 8)
    {
	如果 (propInfo != NULL)
	    XFree(propInfo);
	返回;
    }
    如果 (immediate)
	server_parse_message(dpy, propInfo, numItems);
    否则
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
    如果 (node == NULL)
	返回;	    // out of memory
    node->propInfo = propInfo;
    node->len = len;

    如果 (head.next == NULL)   // initialize circular queue
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

    如果 (!X_DISPLAY)
	返回; // cannot happen?
    当 (head.next != NULL && head.next != &head)
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
    整数
server_waiting(void)
{
    返回 head.next != NULL && head.next != &head;
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
    整数		code;
    char_u	*tofree;

# 如果 defined(FEAT_EVAL)
    ch_log(NULL, "server_parse_message() numItems: %ld", numItems);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    为 (p = propInfo; (long_u)(p - propInfo) < numItems; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	如果 (*p == 0)
	{
	    p++;
	    continue;
	}

	如果 ((*p == 'c' || *p == 'k') && p[1] == 0)
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
	    当 ((long_u)(p - propInfo) < numItems && *p == '-')
	    {
# 如果 defined(FEAT_EVAL)
		ch_log(NULL, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		switch (p[1])
		{
		    case 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			当 (vim_isxdigit(*end))
			{
			    resWindow = 16 * resWindow + (long_u)hex2nr(*end);
			    ++end;
			}
			如果 (end == p + 2 || *end != ' ')
			    resWindow = None;
			否则
			{
			    p = serial = end + 1;
			    clientWindow = resWindow; // Remember in global
			}
			break;
		    case 'n':
			如果 (p[2] == ' ')
			    name = p + 3;
			break;
		    case 's':
			如果 (p[2] == ' ')
			    script = p + 3;
			break;
		    case 'E':
			如果 (p[2] == ' ')
			    enc = p + 3;
			break;
		}
		当 (*p != 0)
		    p++;
		p++;
	    }

	    如果 (script == NULL || name == NULL)
		continue;

	    如果 (serverName != NULL && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		如果 (asKeys)
		    server_to_input_buf(script);
		否则
		{
		    char_u      *res;

		    res = eval_client_expr_to_string(script);
		    如果 (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(void)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(reply.ga_data, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			如果 (res != NULL)
			    ga_concat(&reply, res);
			否则
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
	否则 如果 (*p == 'r' && p[1] == 0)
	{
	    整数		    serial, gotSerial;
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
	    当 ((long_u)(p - propInfo) < numItems && *p == '-')
	    {
		switch (p[1])
		{
		    case 'r':
			如果 (p[2] == ' ')
			    res = p + 3;
			break;
		    case 'E':
			如果 (p[2] == ' ')
			    enc = p + 3;
			break;
		    case 's':
			如果 (sscanf((char *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			break;
		    case 'c':
			如果 (sscanf((char *)p + 2, " %d", &code) != 1)
			    code = 0;
			break;
		}
		当 (*p != 0)
		    p++;
		p++;
	    }

	    如果 (!gotSerial)
		continue;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    为 (pcPtr = pendingCommands; pcPtr != NULL; pcPtr = pcPtr->nextPtr)
	    {
		如果 (serial != pcPtr->serial || pcPtr->result != NULL)
		    continue;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		如果 (tofree == NULL)
		    res = vim_strsave(res);
		pcPtr->result = res;
		break;
	    }
	}
	否则 如果 (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    unsigned 整数 u;
	    整数		gotWindow;
	    char_u	*str;
	    struct	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = NULL;
	    当 ((long_u)(p - propInfo) < numItems && *p == '-')
	    {
		switch (p[1])
		{
		    case 'n':
			如果 (p[2] == ' ')
			    str = p + 3;
			break;
		    case 'E':
			如果 (p[2] == ' ')
			    enc = p + 3;
			break;
		    case 'w':
			如果 (sscanf((char *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			break;
		}
		当 (*p != 0)
		    p++;
		p++;
	    }

	    如果 (!gotWindow)
		continue;
	    str = serverConvert(enc, str, &tofree);
	    如果 ((r = ServerReplyFind(win, SROP_Add)) != NULL)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((char *)winstr, "0x%x", (unsigned 整数)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	否则
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    当 (*p != 0)
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
    static 整数
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*value,		// Characters  to append to property.
    整数		length)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, value, length);
    XSync(dpy, False);
    (void) XSetErrorHandler(old_handler);
    返回 got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    static 整数
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    返回 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    static 整数
IsSerialName(char_u *str)
{
    整数 len = STRLEN(str);

    返回 (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
