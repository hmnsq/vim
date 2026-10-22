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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

#ଯଦି (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା PendingCommand
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    serial;	// Serial number expected in result.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    code;	// Result Code. 0 is OK
    char_u  *result;	// String result for command (malloc'ed).
			// NULL means command still pending.
    ସଂରଚନା PendingCommand *nextPtr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

ସ୍ଥିର PendingCommand *pendingCommands = ଖାଲି;
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

# define MAX_PROP_ଶବ୍ଦଗୁଡ଼ିକ 100000

ସଂରଚନା ServerReply
{
    Window  id;
    garray_T strings;
};
ସ୍ଥିର garray_T serverReply = { 0, 0, 0, 0, 0 };
ଗଣନା ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*EndCond)(ଶୂନ୍ୟ *);

ସଂରଚନା x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    ସଂରଚନା x_cmdqueue	*next;
    ସଂରଚନା x_cmdqueue	*prev;
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା x_cmdqueue x_queue_T;

// dummy node, header for circular queue
ସ୍ଥିର x_queue_T head = {ଖାଲି, 0, ଖାଲି, ଖାଲି};

/*
 * Forward declarations for procedures defined later in this file:
 */

ସ୍ଥିର Window	LookupName(Display *dpy, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା delete, char_u **loose);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	SendInit(Display *dpy);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	DoRegisterName(Display *dpy, char_u *name);
ସ୍ଥିର ଶୂନ୍ୟ	DeleteAnyLingerer(Display *dpy, Window w);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	GetRegProp(Display *dpy, char_u **regPropp, long_u *ଅଙ୍କ_Itemsp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା domsg);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	WaitForPend(ଶୂନ୍ୟ *p);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	WindowValid(Display *dpy, Window w);
ସ୍ଥିର ଶୂନ୍ୟ	ServerWait(Display *dpy, Window w, EndCond endCond, ଶୂନ୍ୟ *endData, ପୂର୍ଣ୍ଣସଂଖ୍ୟା localLoop, ପୂର୍ଣ୍ଣସଂଖ୍ୟା seconds);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	AppendPropCarefully(Display *display, Window window, Atom property, char_u *ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା length);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	x_error_check(Display *dpy, XErrorEvent *error_event);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	IsSerialName(char_u *name);
ସ୍ଥିର ଶୂନ୍ୟ	save_in_queue(char_u *buf, long_u len);
ସ୍ଥିର ଶୂନ୍ୟ	server_parse_message(Display *dpy, char_u *propInfo, long_u ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ);

// Private variables for the "server" functionality
ସ୍ଥିର Atom	registryProperty = None;
ସ୍ଥିର Atom	vimProperty = None;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	got_x_error = FALSE;

ସ୍ଥିର char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    char_u	*p = ଖାଲି;

    res = DoRegisterName(dpy, name);
    ଯଦି (res >= 0)
	ଫେରନ୍ତୁ OK;

    i = 1;
    କର
    {
	ଯଦି (res < -1 || i >= 1000)
	{
	    ସନ୍ଦେଶ_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (p == ଖାଲି)
	    p = alloc(STRLEN(name) + 10);
	ଯଦି (p == ଖାଲି)
	{
	    res = -10;
	    ଜାରି;
	}
	sprintf((ଅକ୍ଷର *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    ଯେତେବେଳେ (res < 0)
	;
    vim_free(p);

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    ଯଦି (commProperty == None)
    {
	ଯଦି (SendInit(dpy) < 0)
	    ଫେରନ୍ତୁ -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, ଖାଲି);
    ଯଦି (w != (Window)0)
    {
	Status		status;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dummyInt;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା	dummyUns;
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
	(ଶୂନ୍ୟ)XSetErrorHandler(old_handler);
	ଯଦି (status != Success && w != commWindow)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    ଫେରନ୍ତୁ -1;
	}
	(ଶୂନ୍ୟ)LookupName(dpy, name, /*delete=*/TRUE, ଖାଲି);
    }
    sprintf((ଅକ୍ଷର *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, ମୂଳ_Window(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (ଶୂନ୍ୟ)XSetErrorHandler(old_handler);

    ଯଦି (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ -2;
}

# ଯଦି defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    ଶୂନ୍ୟ
serverChangeRegisteredWindow(
    Display	*dpy,		// Display to register with
    Window	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    commWindow = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    ଯଦି (SendInit(dpy) < 0)
	ଫେରନ୍ତୁ;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    ଯଦି (serverName != ଖାଲି)
    {
	// Reinsert name if it was already registered
	(ଶୂନ୍ୟ)LookupName(dpy, serverName, /*delete=*/TRUE, ଖାଲି);
	sprintf((ଅକ୍ଷର *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_LENGTH, serverName);
	XChangeProperty(dpy, ମୂଳ_Window(dpy, 0), registryProperty, XA_STRING, 8,
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**result,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	as_ପଦସମଷ୍ଟି,			// Interpret as keystrokes or expr ?
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    length;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    res;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = ଖାଲି;

    ଯଦି (result != ଖାଲି)
	*result = ଖାଲି;
    ଯଦି (name == ଖାଲି || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    ଯଦି (commProperty == None && dpy != ଖାଲି && SendInit(dpy) < 0)
	ଫେରନ୍ତୁ -1;

# ଯଦି defined(FEAT_EVAL)
    ch_log(ଖାଲି, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    ଯଦି (dpy == ଖାଲି || (serverName != ଖାଲି && STRICMP(name, serverName) == 0))
	ଫେରନ୍ତୁ sendToLocalVim(cmd, as_ପଦସମଷ୍ଟି, result);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    ଯେତେବେଳେ (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	ଯଦି (w != None)
	{
	    ଯଦି (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, ଖାଲି);
		VIM_CLEAR(loosename);
		ଜାରି;
	    }
	}
	ଭାଙ୍ଗ;
    }
    ଯଦି (w == None)
    {
	ଯଦି (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ଫେରନ୍ତୁ -1;
    }
    ଅନ୍ୟଥା ଯଦି (loosename != ଖାଲି)
	name = loosename;
    ଯଦି (server != ଖାଲି)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    length = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(length + 30);

    sprintf((ଅକ୍ଷର *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_ପଦସମଷ୍ଟି ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    ଯଦି (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((ଅକ୍ଷର *)property + length, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    length += STRLEN(property + length + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    vim_free(property);
    ଯଦି (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (!as_ପଦସମଷ୍ଟି) // There is no answer for this - Keys are sent async
	ଫେରନ୍ତୁ 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.code = 0;
    pending.result = ଖାଲି;
    pending.nextPtr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    ଯଦି (pendingCommands == &pending)
	pendingCommands = pending.nextPtr;
    ଅନ୍ୟଥା
    {
	PendingCommand *pcPtr;

	ପାଇଁ (pcPtr = pendingCommands; pcPtr != ଖାଲି; pcPtr = pcPtr->nextPtr)
	    ଯଦି (pcPtr->nextPtr == &pending)
	    {
		pcPtr->nextPtr = pending.nextPtr;
		ଭାଙ୍ଗ;
	    }
    }

# ଯଦି defined(FEAT_EVAL)
    ch_log(ଖାଲି, "serverSendToVim() result: %s",
	    pending.result == ଖାଲି ? "NULL" : (ଅକ୍ଷର *)pending.result);
# endif
    ଯଦି (result != ଖାଲି)
	*result = pending.result;
    ଅନ୍ୟଥା
	vim_free(pending.result);

    ଫେରନ୍ତୁ pending.code == 0 ? 0 : -1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
WaitForPend(ଶୂନ୍ୟ *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    ଫେରନ୍ତୁ pending->result != ଖାଲି;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ଅଙ୍କ_Prop;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &ଅଙ୍କ_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    ଯଦି (plist == ଖାଲି || got_x_error)
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (i = 0; i < ଅଙ୍କ_Prop; i++)
	ଯଦି (plist[i] == vimProperty)
	{
	    XFree(plist);
	    ଫେରନ୍ତୁ TRUE;
	}
    XFree(plist);
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ServerWait(
    Display	*dpy,
    Window	w,
    EndCond	endCond,
    ଶୂନ୍ୟ	*endData,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		localLoop,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		seconds)
{
    time_t	    start;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_ସଂଖ୍ୟା(dpy), &fds);
# ଅନ୍ୟଥା
    ସଂରଚନା pollfd   fds;

    fds.fd = Connection_ସଂଖ୍ୟା(dpy);
    fds.events = POLLIN;
# endif

    time(&start);
    ଯେତେବେଳେ (TRUE)
    {
	ଯେତେବେଳେ (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	ଯଦି (endCond(endData) != 0)
	    ଭାଙ୍ଗ;
	ଯଦି (!WindowValid(dpy, w))
	    ଭାଙ୍ଗ;
	time(&now);
	ଯଦି (seconds >= 0 && (now - start) >= seconds)
	    ଭାଙ୍ଗ;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	ଯଦି (localLoop)
	{
# ifdef HAVE_SELECT
	    ସଂରଚନା timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    ଯଦି (select(FD_SETSIZE, &fds, ଖାଲି, ଖାଲି, &tv) < 0)
		ଭାଙ୍ଗ;
# ଅନ୍ୟଥା
	    ଯଦି (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		ଭାଙ୍ଗ;
# endif
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
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
    long_u	ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ;
    int_u	w;
    list_T	*list;

    ଯଦି (registryProperty == None)
    {
	ଯଦି (SendInit(dpy) < 0)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    list = list_alloc();
    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    /*
     * Read the registry property.
     */
    ଯଦି (GetRegProp(dpy, &regProp, &ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ, TRUE) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    /*
     * Scan all of the names out of the property.
     */
    ପାଇଁ (p = regProp; (long_u)(p - regProp) < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ; p++)
    {
	entry = p;
	ଯେତେବେଳେ (*p != 0 && !SAFE_isspace(*p))
	    p++;
	ଯଦି (*p != 0)
	{
	    w = None;
	    sscanf((ଅକ୍ଷର *)entry, "%x", &w);
	    ଯଦି (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    ଯେତେବେଳେ (*p != 0)
		p++;
	}
    }
    ଯଦି (regProp != empty_prop)
	XFree(regProp);
    ଫେରନ୍ତୁ list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    ସ୍ଥିର ସଂରଚନା ServerReply *
ServerReplyFind(Window w, ଗଣନା ServerReplyOp op)
{
    ସଂରଚନା ServerReply *p;
    ସଂରଚନା ServerReply e;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    p = (ସଂରଚନା ServerReply *) serverReply.ga_data;
    ପାଇଁ (i = 0; i < serverReply.ga_len; i++, p++)
	ଯଦି (p->id == w)
	    ଭାଙ୍ଗ;
    ଯଦି (i >= serverReply.ga_len)
	p = ଖାଲି;

    ଯଦି (p == ଖାଲି && op == SROP_Add)
    {
	ଯଦି (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, ଆକାର(ସଂରଚନା ServerReply), 1);
	ଯଦି (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((ସଂରଚନା ServerReply *) serverReply.ga_data)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, ଆକାର(e));
	    serverReply.ga_len++;
	}
    }
    ଅନ୍ୟଥା ଯଦି (p != ଖାଲି && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * ଆକାର(*p));
	serverReply.ga_len--;
    }

    ଫେରନ୍ତୁ p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    unsigned  id = None;

    sscanf((ଅକ୍ଷର *)str, "0x%x", &id);
    ଯଦି (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    ଫେରନ୍ତୁ (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		length;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    ଯଦି (commProperty == None)
    {
	ଯଦି (SendInit(dpy) < 0)
	    ଫେରନ୍ତୁ -2;
    }
    ଯଦି (!WindowValid(dpy, win))
	ଫେରନ୍ତୁ -1;

    length = STRLEN(p_enc) + STRLEN(str) + 14;
    ଯଦି ((property = alloc(length + 30)) == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    sprintf((ଅକ୍ଷର *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)commWindow);
    // Add length of what "%x" resulted in.
    length += STRLEN(property + length);
    res = AppendPropCarefully(dpy, win, commProperty, property, length + 1);
    vim_free(property);

    ଫେରନ୍ତୁ res;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
WaitForReply(ଶୂନ୍ୟ *p)
{
    Window  *w = (Window *) p;

    ଫେରନ୍ତୁ ServerReplyFind(*w, SROP_Find) != ଖାଲି;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		localLoop,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		timeout)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*s;
    ସଂରଚନା	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    ଯଦି ((p = ServerReplyFind(win, SROP_Find)) != ଖାଲି && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_data);
	len = STRLEN(*str) + 1;
	ଯଦି (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_data;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	ଅନ୍ୟଥା
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    ସଂରଚନା ServerReply *p;

    ଯଦି ((p = ServerReplyFind(win, SROP_Find)) != ଖାଲି && p->strings.ga_len > 0)
    {
	ଯଦି (str != ଖାଲି)
	    *str = p->strings.ga_data;
	ଫେରନ୍ତୁ 1;
    }
    ଯଦି (!WindowValid(dpy, win))
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    ଯଦି (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    ଯଦି (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    ଯଦି (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    ଯଦି (commWindow == None)
    {
	commWindow = XCreateSimpleWindow(dpy, XDefault_ମୂଳ_Window(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_ନିବେଶ(dpy, commWindow, PropertyChangeMask);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, commWindow);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, commWindow, vimProperty, XA_STRING,
		    8, PropModeReplace, (char_u *)VIM_VERSION_SHORT,
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (ଶୂନ୍ୟ)XSetErrorHandler(old_handler);

    ଫେରନ୍ତୁ got_x_error ? -1 : 0;
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
    ସ୍ଥିର Window
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *entry;
    char_u	*p;
    long_u	ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ;
    int_u	return_ମୂଲ୍ୟ;

    /*
     * Read the registry property.
     */
    ଯଦି (GetRegProp(dpy, &regProp, &ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ, FALSE) == FAIL)
	ଫେରନ୍ତୁ 0;

    /*
     * Scan the property for the desired name.
     */
    return_ମୂଲ୍ୟ = (int_u)None;
    entry = ଖାଲି;	// Not needed, but eliminates compiler warning.
    ପାଇଁ (p = regProp; (long_u)(p - regProp) < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ; )
    {
	entry = p;
	ଯେତେବେଳେ (*p != 0 && !SAFE_isspace(*p))
	    p++;
	ଯଦି (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    sscanf((ଅକ୍ଷର *)entry, "%x", &return_ମୂଲ୍ୟ);
	    ଭାଙ୍ଗ;
	}
	ଯେତେବେଳେ (*p != 0)
	    p++;
	p++;
    }

    ଯଦି (loose != ଖାଲି && return_ମୂଲ୍ୟ == (int_u)None && !IsSerialName(name))
    {
	ପାଇଁ (p = regProp; (long_u)(p - regProp) < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ; )
	{
	    entry = p;
	    ଯେତେବେଳେ (*p != 0 && !SAFE_isspace(*p))
		p++;
	    ଯଦି (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		ଯଦି (sscanf((ଅକ୍ଷର *)entry, "%x", &return_ମୂଲ୍ୟ) == 1
			&& return_ମୂଲ୍ୟ != (int_u)None)
		    *loose = vim_strsave(p + 1);
		ଭାଙ୍ଗ;
	    }
	    ଯେତେବେଳେ (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    ଯଦି (delete && return_ମୂଲ୍ୟ != (int_u)None)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2;

	ଯେତେବେଳେ (*p != 0)
	    p++;
	p++;
	ଗଣନା_2 = ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ - (p - regProp);
	ଯଦି (ଗଣନା_2 > 0)
	    mch_memmove(entry, p, ଗଣନା_2);
	XChangeProperty(dpy, ମୂଳ_Window(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ - (p - entry)));
	XSync(dpy, False);
    }

    ଯଦି (regProp != empty_prop)
	XFree(regProp);
    ଫେରନ୍ତୁ (Window)return_ମୂଲ୍ୟ;
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
    ସ୍ଥିର ଶୂନ୍ୟ
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    Window	win)	// Window to remove
{
    char_u	*regProp, *entry = ଖାଲି;
    char_u	*p;
    long_u	ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    ଯଦି (GetRegProp(dpy, &regProp, &ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ, FALSE) == FAIL)
	ଫେରନ୍ତୁ;

    // Scan the property for the window id.
    ପାଇଁ (p = regProp; (long_u)(p - regProp) < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ; )
    {
	ଯଦି (*p != 0)
	{
	    sscanf((ଅକ୍ଷର *)p, "%x", &wwin);
	    ଯଦି ((Window)wwin == win)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		ଯେତେବେଳେ (*p != 0)
		    p++;
		p++;
		lastHalf = ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ - (p - regProp);
		ଯଦି (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ = (entry - regProp) + lastHalf;
		p = entry;
		ଜାରି;
	    }
	}
	ଯେତେବେଳେ (*p != 0)
	    p++;
	p++;
    }

    ଯଦି (entry != ଖାଲି)
    {
	XChangeProperty(dpy, ମୂଳ_Window(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - regProp));
	XSync(dpy, False);
    }

    ଯଦି (regProp != empty_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*ଅଙ୍କ_Itemsp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		domsg)		// When TRUE give error message.
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result, actualFormat;
    long_u	bytesAfter;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = ଖାଲି;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    result = XGetWindowProperty(dpy, ମୂଳ_Window(dpy, 0), registryProperty, 0L,
				(long)MAX_PROP_ଶବ୍ଦଗୁଡ଼ିକ, False,
				XA_STRING, &actualType,
				&actualFormat, ଅଙ୍କ_Itemsp, &bytesAfter,
				regPropp);

    XSync(dpy, FALSE);
    (ଶୂନ୍ୟ)XSetErrorHandler(old_handler);
    ଯଦି (got_x_error)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*ଅଙ୍କ_Itemsp = 0;
	*regPropp = empty_prop;
	ଫେରନ୍ତୁ OK;
    }

    // If the property is improperly formed, then delete it.
    ଯଦି (result != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	ଯଦି (*regPropp != ଖାଲି)
	    XFree(*regPropp);
	XDeleteProperty(dpy, ମୂଳ_Window(dpy, 0), registryProperty);
	ଯଦି (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    ଶୂନ୍ୟ
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result, actualFormat;
    long_u	ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ, bytesAfter;
    Atom	actualType;

    ଯଦି (eventPtr != ଖାଲି)
    {
	ଯଦି (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_ମୂଲ୍ୟ)
	    ଫେରନ୍ତୁ;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = ଖାଲି;
    result = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(long)MAX_PROP_ଶବ୍ଦଗୁଡ଼ିକ, True,
				XA_STRING, &actualType,
				&actualFormat, &ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ, &bytesAfter,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    ଯଦି (result != Success || actualType != XA_STRING || actualFormat != 8)
    {
	ଯଦି (propInfo != ଖାଲି)
	    XFree(propInfo);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (immediate)
	server_parse_message(dpy, propInfo, ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ);
    ଅନ୍ୟଥା
	save_in_queue(propInfo, ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
save_in_queue(char_u *propInfo, long_u len)
{
    x_queue_T *ଗଣ୍ଠି;

    ଗଣ୍ଠି = ALLOC_ONE(x_queue_T);
    ଯଦି (ଗଣ୍ଠି == ଖାଲି)
	ଫେରନ୍ତୁ;	    // out of memory
    ଗଣ୍ଠି->propInfo = propInfo;
    ଗଣ୍ଠି->len = len;

    ଯଦି (head.next == ଖାଲି)   // initialize circular queue
    {
	head.next = &head;
	head.prev = &head;
    }

    // insert node at tail of queue
    ଗଣ୍ଠି->next = &head;
    ଗଣ୍ଠି->prev = head.prev;
    head.prev->next = ଗଣ୍ଠି;
    head.prev = ଗଣ୍ଠି;
}

/*
 * Parses queued clientserver messages.
 */
    ଶୂନ୍ୟ
server_parse_messages(ଶୂନ୍ୟ)
{
    x_queue_T	*ଗଣ୍ଠି;

    ଯଦି (!X_DISPLAY)
	ଫେରନ୍ତୁ; // cannot happen?
    ଯେତେବେଳେ (head.next != ଖାଲି && head.next != &head)
    {
	ଗଣ୍ଠି = head.next;
	head.next = ଗଣ୍ଠି->next;
	ଗଣ୍ଠି->next->prev = ଗଣ୍ଠି->prev;
	server_parse_message(X_DISPLAY, ଗଣ୍ଠି->propInfo, ଗଣ୍ଠି->len);
	vim_free(ଗଣ୍ଠି);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
server_waiting(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ head.next != ଖାଲି && head.next != &head;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
server_parse_message(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ)  // The size of propInfo in bytes.
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		code;
    char_u	*tofree;

# ଯଦି defined(FEAT_EVAL)
    ch_log(ଖାଲି, "server_parse_message() numItems: %ld", ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    ପାଇଁ (p = propInfo; (long_u)(p - propInfo) < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	ଯଦି (*p == 0)
	{
	    p++;
	    ଜାରି;
	}

	ଯଦି ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    Window	resWindow;
	    char_u	*name, *script, *serial, *end;
	    Bool	as_ଚାବିଗୁଡ଼ିକ = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = ଖାଲି;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = ଖାଲି;
	    enc = ଖାଲି;
	    ଯେତେବେଳେ ((long_u)(p - propInfo) < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ && *p == '-')
	    {
# ଯଦି defined(FEAT_EVAL)
		ch_log(ଖାଲି, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		ବାଛ (p[1])
		{
		    ସ୍ଥିତି 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			ଯେତେବେଳେ (vim_isxdigit(*end))
			{
			    resWindow = 16 * resWindow + (long_u)hex2nr(*end);
			    ++end;
			}
			ଯଦି (end == p + 2 || *end != ' ')
			    resWindow = None;
			ଅନ୍ୟଥା
			{
			    p = serial = end + 1;
			    clientWindow = resWindow; // Remember in global
			}
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'n':
			ଯଦି (p[2] == ' ')
			    name = p + 3;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 's':
			ଯଦି (p[2] == ' ')
			    script = p + 3;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'E':
			ଯଦି (p[2] == ' ')
			    enc = p + 3;
			ଭାଙ୍ଗ;
		}
		ଯେତେବେଳେ (*p != 0)
		    p++;
		p++;
	    }

	    ଯଦି (script == ଖାଲି || name == ଖାଲି)
		ଜାରି;

	    ଯଦି (serverName != ଖାଲି && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		ଯଦି (as_ଚାବିଗୁଡ଼ିକ)
		    server_to_ନିବେଶ_buf(script);
		ଅନ୍ୟଥା
		{
		    char_u      *res;

		    res = eval_client_ପଦସମଷ୍ଟି_to_string(script);
		    ଯଦି (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(ଶୂନ୍ୟ)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(reply.ga_data, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			ଯଦି (res != ଖାଲି)
			    ga_concat(&reply, res);
			ଅନ୍ୟଥା
			{
			    ga_concat(&reply,
				   (char_u *)_(e_invalid_expression_received));
			    ga_append(&reply, 0);
			    GA_CONCAT_LITERAL(&reply, "-c 1");
			}
			ga_append(&reply, NUL);
			(ଶୂନ୍ୟ)AppendPropCarefully(dpy, resWindow, commProperty,
						 reply.ga_data, reply.ga_len);
			ga_clear(&reply);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (*p == 'r' && p[1] == 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    serial, gotSerial;
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
	    enc = ଖାଲି;
	    ଯେତେବେଳେ ((long_u)(p - propInfo) < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ && *p == '-')
	    {
		ବାଛ (p[1])
		{
		    ସ୍ଥିତି 'r':
			ଯଦି (p[2] == ' ')
			    res = p + 3;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'E':
			ଯଦି (p[2] == ' ')
			    enc = p + 3;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 's':
			ଯଦି (sscanf((ଅକ୍ଷର *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'c':
			ଯଦି (sscanf((ଅକ୍ଷର *)p + 2, " %d", &code) != 1)
			    code = 0;
			ଭାଙ୍ଗ;
		}
		ଯେତେବେଳେ (*p != 0)
		    p++;
		p++;
	    }

	    ଯଦି (!gotSerial)
		ଜାରି;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    ପାଇଁ (pcPtr = pendingCommands; pcPtr != ଖାଲି; pcPtr = pcPtr->nextPtr)
	    {
		ଯଦି (serial != pcPtr->serial || pcPtr->result != ଖାଲି)
		    ଜାରି;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		ଯଦି (tofree == ଖାଲି)
		    res = vim_strsave(res);
		pcPtr->result = res;
		ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା u;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		gotWindow;
	    char_u	*str;
	    ସଂରଚନା	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = ଖାଲି;
	    ଯେତେବେଳେ ((long_u)(p - propInfo) < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ && *p == '-')
	    {
		ବାଛ (p[1])
		{
		    ସ୍ଥିତି 'n':
			ଯଦି (p[2] == ' ')
			    str = p + 3;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'E':
			ଯଦି (p[2] == ' ')
			    enc = p + 3;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'w':
			ଯଦି (sscanf((ଅକ୍ଷର *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			ଭାଙ୍ଗ;
		}
		ଯେତେବେଳେ (*p != 0)
		    p++;
		p++;
	    }

	    ଯଦି (!gotWindow)
		ଜାରି;
	    str = serverConvert(enc, str, &tofree);
	    ଯଦି ((r = ServerReplyFind(win, SROP_Add)) != ଖାଲି)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((ଅକ୍ଷର *)winstr, "0x%x", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	ଅନ୍ୟଥା
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    ଯେତେବେଳେ (*p != 0)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*ମୂଲ୍ୟ,		// Characters  to append to property.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		length)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, ମୂଲ୍ୟ, length);
    XSync(dpy, False);
    (ଶୂନ୍ୟ) XSetErrorHandler(old_handler);
    ଫେରନ୍ତୁ got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    ଫେରନ୍ତୁ 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
IsSerialName(char_u *str)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = STRLEN(str);

    ଫେରନ୍ତୁ (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
