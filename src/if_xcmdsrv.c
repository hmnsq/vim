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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

#ಯದಿ (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ PendingCommand
{
    ಪೂರ್ಣಾಙ್ಕ	    serial;	// Serial number expected in result.
    ಪೂರ್ಣಾಙ್ಕ	    code;	// Result Code. 0 is OK
    char_u  *result;	// String result for command (malloc'ed).
			// NULL means command still pending.
    ಸಂರಚನಾ PendingCommand *nextPtr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

ಸ್ಥಿರ PendingCommand *pendingCommands = ರಿಕ್ತ;
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

# define MAX_PROP_ಪದಗಳು 100000

ಸಂರಚನಾ ServerReply
{
    Window  id;
    garray_T strings;
};
ಸ್ಥಿರ garray_T serverReply = { 0, 0, 0, 0, 0 };
ಗಣನಾ ServerReplyOp { SROP_Find, SROP_Add, SROP_Delete };

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*EndCond)(ಶೂನ್ಯ *);

ಸಂರಚನಾ x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    ಸಂರಚನಾ x_cmdqueue	*next;
    ಸಂರಚನಾ x_cmdqueue	*prev;
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ x_cmdqueue x_queue_T;

// dummy node, header for circular queue
ಸ್ಥಿರ x_queue_T head = {ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ};

/*
 * Forward declarations for procedures defined later in this file:
 */

ಸ್ಥಿರ Window	LookupName(Display *dpy, char_u *name, ಪೂರ್ಣಾಙ್ಕ delete, char_u **loose);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	SendInit(Display *dpy);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	DoRegisterName(Display *dpy, char_u *name);
ಸ್ಥಿರ ಶೂನ್ಯ	DeleteAnyLingerer(Display *dpy, Window w);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	GetRegProp(Display *dpy, char_u **regPropp, long_u *ಅಂಕೆ_Itemsp, ಪೂರ್ಣಾಙ್ಕ domsg);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	WaitForPend(ಶೂನ್ಯ *p);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	WindowValid(Display *dpy, Window w);
ಸ್ಥಿರ ಶೂನ್ಯ	ServerWait(Display *dpy, Window w, EndCond endCond, ಶೂನ್ಯ *endData, ಪೂರ್ಣಾಙ್ಕ localLoop, ಪೂರ್ಣಾಙ್ಕ seconds);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	AppendPropCarefully(Display *display, Window window, Atom property, char_u *ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ length);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	x_error_check(Display *dpy, XErrorEvent *error_event);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	IsSerialName(char_u *name);
ಸ್ಥಿರ ಶೂನ್ಯ	save_in_queue(char_u *buf, long_u len);
ಸ್ಥಿರ ಶೂನ್ಯ	server_parse_message(Display *dpy, char_u *propInfo, long_u ಅಂಕೆ_ವಸ್ತುಗಳು);

// Private variables for the "server" functionality
ಸ್ಥಿರ Atom	registryProperty = None;
ಸ್ಥಿರ Atom	vimProperty = None;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	got_x_error = FALSE;

ಸ್ಥಿರ char_u	*empty_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    ಪೂರ್ಣಾಙ್ಕ
serverRegisterName(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		res;
    char_u	*p = ರಿಕ್ತ;

    res = DoRegisterName(dpy, name);
    ಯದಿ (res >= 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    i = 1;
    ಕುರು
    {
	ಯದಿ (res < -1 || i >= 1000)
	{
	    ಸಂದೇಶ_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (p == ರಿಕ್ತ)
	    p = alloc(STRLEN(name) + 10);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    res = -10;
	    ಅನುವರ್ತ;
	}
	sprintf((ವರ್ಣ *)p, "%s%d", name, i++);
	res = DoRegisterName(dpy, p);
    }
    ಯಾವತ್ (res < 0)
	;
    vim_free(p);

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
DoRegisterName(Display *dpy, char_u *name)
{
    Window	w;
    XErrorHandler old_handler;
# define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    ಯದಿ (commProperty == None)
    {
	ಯದಿ (SendInit(dpy) < 0)
	    ಪ್ರತ್ಯಾಗಮ -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, ರಿಕ್ತ);
    ಯದಿ (w != (Window)0)
    {
	Status		status;
	ಪೂರ್ಣಾಙ್ಕ		dummyInt;
	unsigned ಪೂರ್ಣಾಙ್ಕ	dummyUns;
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
	(ಶೂನ್ಯ)XSetErrorHandler(old_handler);
	ಯದಿ (status != Success && w != commWindow)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	(ಶೂನ್ಯ)LookupName(dpy, name, /*delete=*/TRUE, ರಿಕ್ತ);
    }
    sprintf((ವರ್ಣ *)propInfo, "%x %.*s", (int_u)commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, ಬೇರು_Window(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (ಶೂನ್ಯ)XSetErrorHandler(old_handler);

    ಯದಿ (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, (ಪೂರ್ಣಾಙ್ಕ)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ -2;
}

# ಯದಿ defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    ಶೂನ್ಯ
serverChangeRegisteredWindow(
    Display	*dpy,		// Display to register with
    Window	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    commWindow = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    ಯದಿ (SendInit(dpy) < 0)
	ಪ್ರತ್ಯಾಗಮ;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    ಯದಿ (serverName != ರಿಕ್ತ)
    {
	// Reinsert name if it was already registered
	(ಶೂನ್ಯ)LookupName(dpy, serverName, /*delete=*/TRUE, ರಿಕ್ತ);
	sprintf((ವರ್ಣ *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_LENGTH, serverName);
	XChangeProperty(dpy, ಬೇರು_Window(dpy, 0), registryProperty, XA_STRING, 8,
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
    ಪೂರ್ಣಾಙ್ಕ
serverSendToVim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**result,		// Result of eval'ed expression
    Window	*server,		// Actual ID of receiving app
    Bool	as_ಅಭಿವ್ಯಕ್ತಿ,			// Interpret as keystrokes or expr ?
    ಪೂರ್ಣಾಙ್ಕ		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    ಪೂರ್ಣಾಙ್ಕ		silent)			// don't complain about no server
{
    Window	    w;
    char_u	    *property;
    ಪೂರ್ಣಾಙ್ಕ		    length;
    ಪೂರ್ಣಾಙ್ಕ		    res;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = ರಿಕ್ತ;

    ಯದಿ (result != ರಿಕ್ತ)
	*result = ರಿಕ್ತ;
    ಯದಿ (name == ರಿಕ್ತ || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    ಯದಿ (commProperty == None && dpy != ರಿಕ್ತ && SendInit(dpy) < 0)
	ಪ್ರತ್ಯಾಗಮ -1;

# ಯದಿ defined(FEAT_EVAL)
    ch_log(ರಿಕ್ತ, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    ಯದಿ (dpy == ರಿಕ್ತ || (serverName != ರಿಕ್ತ && STRICMP(name, serverName) == 0))
	ಪ್ರತ್ಯಾಗಮ sendToLocalVim(cmd, as_ಅಭಿವ್ಯಕ್ತಿ, result);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    ಯಾವತ್ (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	ಯದಿ (w != None)
	{
	    ಯದಿ (!WindowValid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, ರಿಕ್ತ);
		VIM_CLEAR(loosename);
		ಅನುವರ್ತ;
	    }
	}
	ಭಙ್ಗ;
    }
    ಯದಿ (w == None)
    {
	ಯದಿ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಅನ್ಯಥಾ ಯದಿ (loosename != ರಿಕ್ತ)
	name = loosename;
    ಯದಿ (server != ರಿಕ್ತ)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    length = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(length + 30);

    sprintf((ವರ್ಣ *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_ಅಭಿವ್ಯಕ್ತಿ ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    ಯದಿ (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((ವರ್ಣ *)property + length, "%c-r %x %d",
						0, (int_u)commWindow, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    length += STRLEN(property + length + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    vim_free(property);
    ಯದಿ (res < 0)
    {
	emsg(_(e_failed_to_send_command_to_destination_program));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (!as_ಅಭಿವ್ಯಕ್ತಿ) // There is no answer for this - Keys are sent async
	ಪ್ರತ್ಯಾಗಮ 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.code = 0;
    pending.result = ರಿಕ್ತ;
    pending.nextPtr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    ಯದಿ (pendingCommands == &pending)
	pendingCommands = pending.nextPtr;
    ಅನ್ಯಥಾ
    {
	PendingCommand *pcPtr;

	ಪ್ರತಿ (pcPtr = pendingCommands; pcPtr != ರಿಕ್ತ; pcPtr = pcPtr->nextPtr)
	    ಯದಿ (pcPtr->nextPtr == &pending)
	    {
		pcPtr->nextPtr = pending.nextPtr;
		ಭಙ್ಗ;
	    }
    }

# ಯದಿ defined(FEAT_EVAL)
    ch_log(ರಿಕ್ತ, "serverSendToVim() result: %s",
	    pending.result == ರಿಕ್ತ ? "NULL" : (ವರ್ಣ *)pending.result);
# endif
    ಯದಿ (result != ರಿಕ್ತ)
	*result = pending.result;
    ಅನ್ಯಥಾ
	vim_free(pending.result);

    ಪ್ರತ್ಯಾಗಮ pending.code == 0 ? 0 : -1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
WaitForPend(ಶೂನ್ಯ *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    ಪ್ರತ್ಯಾಗಮ pending->result != ರಿಕ್ತ;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
WindowValid(Display *dpy, Window w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    ಪೂರ್ಣಾಙ್ಕ		    ಅಂಕೆ_Prop;
    ಪೂರ್ಣಾಙ್ಕ		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &ಅಂಕೆ_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    ಯದಿ (plist == ರಿಕ್ತ || got_x_error)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_Prop; i++)
	ಯದಿ (plist[i] == vimProperty)
	{
	    XFree(plist);
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    XFree(plist);
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ServerWait(
    Display	*dpy,
    Window	w,
    EndCond	endCond,
    ಶೂನ್ಯ	*endData,
    ಪೂರ್ಣಾಙ್ಕ		localLoop,
    ಪೂರ್ಣಾಙ್ಕ		seconds)
{
    time_t	    start;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_ಸಂಖ್ಯೆ(dpy), &fds);
# ಅನ್ಯಥಾ
    ಸಂರಚನಾ pollfd   fds;

    fds.fd = Connection_ಸಂಖ್ಯೆ(dpy);
    fds.events = POLLIN;
# endif

    time(&start);
    ಯಾವತ್ (TRUE)
    {
	ಯಾವತ್ (XCheckWindowEvent(dpy, commWindow, PropertyChangeMask, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	ಯದಿ (endCond(endData) != 0)
	    ಭಙ್ಗ;
	ಯದಿ (!WindowValid(dpy, w))
	    ಭಙ್ಗ;
	time(&now);
	ಯದಿ (seconds >= 0 && (now - start) >= seconds)
	    ಭಙ್ಗ;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	ಯದಿ (localLoop)
	{
# ifdef HAVE_SELECT
	    ಸಂರಚನಾ timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    ಯದಿ (select(FD_SETSIZE, &fds, ರಿಕ್ತ, ರಿಕ್ತ, &tv) < 0)
		ಭಙ್ಗ;
# ಅನ್ಯಥಾ
	    ಯದಿ (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		ಭಙ್ಗ;
# endif
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (got_int)
		ಭಙ್ಗ;
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
    long_u	ಅಂಕೆ_ವಸ್ತುಗಳು;
    int_u	w;
    list_T	*list;

    ಯದಿ (registryProperty == None)
    {
	ಯದಿ (SendInit(dpy) < 0)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    list = list_alloc();
    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    /*
     * Read the registry property.
     */
    ಯದಿ (GetRegProp(dpy, &regProp, &ಅಂಕೆ_ವಸ್ತುಗಳು, TRUE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    /*
     * Scan all of the names out of the property.
     */
    ಪ್ರತಿ (p = regProp; (long_u)(p - regProp) < ಅಂಕೆ_ವಸ್ತುಗಳು; p++)
    {
	entry = p;
	ಯಾವತ್ (*p != 0 && !SAFE_isspace(*p))
	    p++;
	ಯದಿ (*p != 0)
	{
	    w = None;
	    sscanf((ವರ್ಣ *)entry, "%x", &w);
	    ಯದಿ (WindowValid(dpy, (Window)w))
		list_append_string(list, p + 1, -1);
	    ಯಾವತ್ (*p != 0)
		p++;
	}
    }
    ಯದಿ (regProp != empty_prop)
	XFree(regProp);
    ಪ್ರತ್ಯಾಗಮ list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    ಸ್ಥಿರ ಸಂರಚನಾ ServerReply *
ServerReplyFind(Window w, ಗಣನಾ ServerReplyOp op)
{
    ಸಂರಚನಾ ServerReply *p;
    ಸಂರಚನಾ ServerReply e;
    ಪೂರ್ಣಾಙ್ಕ		i;

    p = (ಸಂರಚನಾ ServerReply *) serverReply.ga_data;
    ಪ್ರತಿ (i = 0; i < serverReply.ga_len; i++, p++)
	ಯದಿ (p->id == w)
	    ಭಙ್ಗ;
    ಯದಿ (i >= serverReply.ga_len)
	p = ರಿಕ್ತ;

    ಯದಿ (p == ರಿಕ್ತ && op == SROP_Add)
    {
	ಯದಿ (serverReply.ga_growsize == 0)
	    ga_init2(&serverReply, ಪರಿಮಾಣ(ಸಂರಚನಾ ServerReply), 1);
	ಯದಿ (ga_grow(&serverReply, 1) == OK)
	{
	    p = ((ಸಂರಚನಾ ServerReply *) serverReply.ga_data)
		+ serverReply.ga_len;
	    e.id = w;
	    ga_init2(&e.strings, 1, 100);
	    mch_memmove(p, &e, ಪರಿಮಾಣ(e));
	    serverReply.ga_len++;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (p != ರಿಕ್ತ && op == SROP_Delete)
    {
	ga_clear(&p->strings);
	mch_memmove(p, p + 1, (serverReply.ga_len - i - 1) * ಪರಿಮಾಣ(*p));
	serverReply.ga_len--;
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    Window
serverStrToWin(char_u *str)
{
    unsigned  id = None;

    sscanf((ವರ್ಣ *)str, "0x%x", &id);
    ಯದಿ (id == None)
	semsg(_(e_invalid_server_id_used_str), str);

    ಪ್ರತ್ಯಾಗಮ (Window)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    ಪೂರ್ಣಾಙ್ಕ
serverSendReply(char_u *name, char_u *str)
{
    char_u	*property;
    ಪೂರ್ಣಾಙ್ಕ		length;
    ಪೂರ್ಣಾಙ್ಕ		res;
    Display	*dpy = X_DISPLAY;
    Window	win = serverStrToWin(name);

    ಯದಿ (commProperty == None)
    {
	ಯದಿ (SendInit(dpy) < 0)
	    ಪ್ರತ್ಯಾಗಮ -2;
    }
    ಯದಿ (!WindowValid(dpy, win))
	ಪ್ರತ್ಯಾಗಮ -1;

    length = STRLEN(p_enc) + STRLEN(str) + 14;
    ಯದಿ ((property = alloc(length + 30)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    sprintf((ವರ್ಣ *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (unsigned ಪೂರ್ಣಾಙ್ಕ)commWindow);
    // Add length of what "%x" resulted in.
    length += STRLEN(property + length);
    res = AppendPropCarefully(dpy, win, commProperty, property, length + 1);
    vim_free(property);

    ಪ್ರತ್ಯಾಗಮ res;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
WaitForReply(ಶೂನ್ಯ *p)
{
    Window  *w = (Window *) p;

    ಪ್ರತ್ಯಾಗಮ ServerReplyFind(*w, SROP_Find) != ರಿಕ್ತ;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    ಪೂರ್ಣಾಙ್ಕ
serverReadReply(
    Display	*dpy,
    Window	win,
    char_u	**str,
    ಪೂರ್ಣಾಙ್ಕ		localLoop,
    ಪೂರ್ಣಾಙ್ಕ		timeout)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*s;
    ಸಂರಚನಾ	ServerReply *p;

    ServerWait(dpy, win, WaitForReply, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    ಯದಿ ((p = ServerReplyFind(win, SROP_Find)) != ರಿಕ್ತ && p->strings.ga_len > 0)
    {
	*str = vim_strsave(p->strings.ga_data);
	len = STRLEN(*str) + 1;
	ಯದಿ (len < p->strings.ga_len)
	{
	    s = (char_u *) p->strings.ga_data;
	    mch_memmove(s, s + len, p->strings.ga_len - len);
	    p->strings.ga_len -= len;
	}
	ಅನ್ಯಥಾ
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->strings);
	    ServerReplyFind(win, SROP_Delete);
	}
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    ಪೂರ್ಣಾಙ್ಕ
serverPeekReply(Display *dpy, Window win, char_u **str)
{
    ಸಂರಚನಾ ServerReply *p;

    ಯದಿ ((p = ServerReplyFind(win, SROP_Find)) != ರಿಕ್ತ && p->strings.ga_len > 0)
    {
	ಯದಿ (str != ರಿಕ್ತ)
	    *str = p->strings.ga_data;
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಯದಿ (!WindowValid(dpy, win))
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    ಯದಿ (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    ಯದಿ (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    ಯದಿ (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    ಯದಿ (commWindow == None)
    {
	commWindow = XCreateSimpleWindow(dpy, XDefault_ಬೇರು_Window(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_ಒಳಹರಿವು(dpy, commWindow, PropertyChangeMask);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, commWindow);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, commWindow, vimProperty, XA_STRING,
		    8, PropModeReplace, (char_u *)VIM_VERSION_SHORT,
			(ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(VIM_VERSION_SHORT) + 1);

    XSync(dpy, False);
    (ಶೂನ್ಯ)XSetErrorHandler(old_handler);

    ಪ್ರತ್ಯಾಗಮ got_x_error ? -1 : 0;
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
    ಸ್ಥಿರ Window
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    ಪೂರ್ಣಾಙ್ಕ		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *entry;
    char_u	*p;
    long_u	ಅಂಕೆ_ವಸ್ತುಗಳು;
    int_u	return_ಮೌಲ್ಯ;

    /*
     * Read the registry property.
     */
    ಯದಿ (GetRegProp(dpy, &regProp, &ಅಂಕೆ_ವಸ್ತುಗಳು, FALSE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ 0;

    /*
     * Scan the property for the desired name.
     */
    return_ಮೌಲ್ಯ = (int_u)None;
    entry = ರಿಕ್ತ;	// Not needed, but eliminates compiler warning.
    ಪ್ರತಿ (p = regProp; (long_u)(p - regProp) < ಅಂಕೆ_ವಸ್ತುಗಳು; )
    {
	entry = p;
	ಯಾವತ್ (*p != 0 && !SAFE_isspace(*p))
	    p++;
	ಯದಿ (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    sscanf((ವರ್ಣ *)entry, "%x", &return_ಮೌಲ್ಯ);
	    ಭಙ್ಗ;
	}
	ಯಾವತ್ (*p != 0)
	    p++;
	p++;
    }

    ಯದಿ (loose != ರಿಕ್ತ && return_ಮೌಲ್ಯ == (int_u)None && !IsSerialName(name))
    {
	ಪ್ರತಿ (p = regProp; (long_u)(p - regProp) < ಅಂಕೆ_ವಸ್ತುಗಳು; )
	{
	    entry = p;
	    ಯಾವತ್ (*p != 0 && !SAFE_isspace(*p))
		p++;
	    ಯದಿ (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		ಯದಿ (sscanf((ವರ್ಣ *)entry, "%x", &return_ಮೌಲ್ಯ) == 1
			&& return_ಮೌಲ್ಯ != (int_u)None)
		    *loose = vim_strsave(p + 1);
		ಭಙ್ಗ;
	    }
	    ಯಾವತ್ (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    ಯದಿ (delete && return_ಮೌಲ್ಯ != (int_u)None)
    {
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ;

	ಯಾವತ್ (*p != 0)
	    p++;
	p++;
	ಎಣಿಕೆ = ಅಂಕೆ_ವಸ್ತುಗಳು - (p - regProp);
	ಯದಿ (ಎಣಿಕೆ > 0)
	    mch_memmove(entry, p, ಎಣಿಕೆ);
	XChangeProperty(dpy, ಬೇರು_Window(dpy, 0), registryProperty, XA_STRING,
			8, PropModeReplace, regProp,
			(ಪೂರ್ಣಾಙ್ಕ)(ಅಂಕೆ_ವಸ್ತುಗಳು - (p - entry)));
	XSync(dpy, False);
    }

    ಯದಿ (regProp != empty_prop)
	XFree(regProp);
    ಪ್ರತ್ಯಾಗಮ (Window)return_ಮೌಲ್ಯ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    Window	win)	// Window to remove
{
    char_u	*regProp, *entry = ರಿಕ್ತ;
    char_u	*p;
    long_u	ಅಂಕೆ_ವಸ್ತುಗಳು;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    ಯದಿ (GetRegProp(dpy, &regProp, &ಅಂಕೆ_ವಸ್ತುಗಳು, FALSE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // Scan the property for the window id.
    ಪ್ರತಿ (p = regProp; (long_u)(p - regProp) < ಅಂಕೆ_ವಸ್ತುಗಳು; )
    {
	ಯದಿ (*p != 0)
	{
	    sscanf((ವರ್ಣ *)p, "%x", &wwin);
	    ಯದಿ ((Window)wwin == win)
	    {
		ಪೂರ್ಣಾಙ್ಕ lastHalf;

		// Copy down the remainder to delete entry
		entry = p;
		ಯಾವತ್ (*p != 0)
		    p++;
		p++;
		lastHalf = ಅಂಕೆ_ವಸ್ತುಗಳು - (p - regProp);
		ಯದಿ (lastHalf > 0)
		    mch_memmove(entry, p, lastHalf);
		ಅಂಕೆ_ವಸ್ತುಗಳು = (entry - regProp) + lastHalf;
		p = entry;
		ಅನುವರ್ತ;
	    }
	}
	ಯಾವತ್ (*p != 0)
	    p++;
	p++;
    }

    ಯದಿ (entry != ರಿಕ್ತ)
    {
	XChangeProperty(dpy, ಬೇರು_Window(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(ಪೂರ್ಣಾಙ್ಕ)(p - regProp));
	XSync(dpy, False);
    }

    ಯದಿ (regProp != empty_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*ಅಂಕೆ_Itemsp,
    ಪೂರ್ಣಾಙ್ಕ		domsg)		// When TRUE give error message.
{
    ಪೂರ್ಣಾಙ್ಕ		result, actualFormat;
    long_u	bytesAfter;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = ರಿಕ್ತ;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    result = XGetWindowProperty(dpy, ಬೇರು_Window(dpy, 0), registryProperty, 0L,
				(long)MAX_PROP_ಪದಗಳು, False,
				XA_STRING, &actualType,
				&actualFormat, ಅಂಕೆ_Itemsp, &bytesAfter,
				regPropp);

    XSync(dpy, FALSE);
    (ಶೂನ್ಯ)XSetErrorHandler(old_handler);
    ಯದಿ (got_x_error)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*ಅಂಕೆ_Itemsp = 0;
	*regPropp = empty_prop;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    // If the property is improperly formed, then delete it.
    ಯದಿ (result != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	ಯದಿ (*regPropp != ರಿಕ್ತ)
	    XFree(*regPropp);
	XDeleteProperty(dpy, ಬೇರು_Window(dpy, 0), registryProperty);
	ಯದಿ (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    ಶೂನ್ಯ
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    ಪೂರ್ಣಾಙ್ಕ		immediate)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    ಪೂರ್ಣಾಙ್ಕ		result, actualFormat;
    long_u	ಅಂಕೆ_ವಸ್ತುಗಳು, bytesAfter;
    Atom	actualType;

    ಯದಿ (eventPtr != ರಿಕ್ತ)
    {
	ಯದಿ (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_ಮೌಲ್ಯ)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = ರಿಕ್ತ;
    result = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(long)MAX_PROP_ಪದಗಳು, True,
				XA_STRING, &actualType,
				&actualFormat, &ಅಂಕೆ_ವಸ್ತುಗಳು, &bytesAfter,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    ಯದಿ (result != Success || actualType != XA_STRING || actualFormat != 8)
    {
	ಯದಿ (propInfo != ರಿಕ್ತ)
	    XFree(propInfo);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (immediate)
	server_parse_message(dpy, propInfo, ಅಂಕೆ_ವಸ್ತುಗಳು);
    ಅನ್ಯಥಾ
	save_in_queue(propInfo, ಅಂಕೆ_ವಸ್ತುಗಳು);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
save_in_queue(char_u *propInfo, long_u len)
{
    x_queue_T *ಗಂಟು;

    ಗಂಟು = ALLOC_ONE(x_queue_T);
    ಯದಿ (ಗಂಟು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;	    // out of memory
    ಗಂಟು->propInfo = propInfo;
    ಗಂಟು->len = len;

    ಯದಿ (head.next == ರಿಕ್ತ)   // initialize circular queue
    {
	head.next = &head;
	head.prev = &head;
    }

    // insert node at tail of queue
    ಗಂಟು->next = &head;
    ಗಂಟು->prev = head.prev;
    head.prev->next = ಗಂಟು;
    head.prev = ಗಂಟು;
}

/*
 * Parses queued clientserver messages.
 */
    ಶೂನ್ಯ
server_parse_messages(ಶೂನ್ಯ)
{
    x_queue_T	*ಗಂಟು;

    ಯದಿ (!X_DISPLAY)
	ಪ್ರತ್ಯಾಗಮ; // cannot happen?
    ಯಾವತ್ (head.next != ರಿಕ್ತ && head.next != &head)
    {
	ಗಂಟು = head.next;
	head.next = ಗಂಟು->next;
	ಗಂಟು->next->prev = ಗಂಟು->prev;
	server_parse_message(X_DISPLAY, ಗಂಟು->propInfo, ಗಂಟು->len);
	vim_free(ಗಂಟು);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    ಪೂರ್ಣಾಙ್ಕ
server_waiting(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ head.next != ರಿಕ್ತ && head.next != &head;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
server_parse_message(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	ಅಂಕೆ_ವಸ್ತುಗಳು)  // The size of propInfo in bytes.
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		code;
    char_u	*tofree;

# ಯದಿ defined(FEAT_EVAL)
    ch_log(ರಿಕ್ತ, "server_parse_message() numItems: %ld", ಅಂಕೆ_ವಸ್ತುಗಳು);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    ಪ್ರತಿ (p = propInfo; (long_u)(p - propInfo) < ಅಂಕೆ_ವಸ್ತುಗಳು; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	ಯದಿ (*p == 0)
	{
	    p++;
	    ಅನುವರ್ತ;
	}

	ಯದಿ ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    Window	resWindow;
	    char_u	*name, *script, *serial, *end;
	    Bool	as_ಕೀಲಿಗಳು = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = ರಿಕ್ತ;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = ರಿಕ್ತ;
	    enc = ರಿಕ್ತ;
	    ಯಾವತ್ ((long_u)(p - propInfo) < ಅಂಕೆ_ವಸ್ತುಗಳು && *p == '-')
	    {
# ಯದಿ defined(FEAT_EVAL)
		ch_log(ರಿಕ್ತ, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		ವಿಕಲ್ಪ (p[1])
		{
		    ಪಕ್ಷ 'r':
			end = skipwhite(p + 2);
			resWindow = 0;
			ಯಾವತ್ (vim_isxdigit(*end))
			{
			    resWindow = 16 * resWindow + (long_u)hex2nr(*end);
			    ++end;
			}
			ಯದಿ (end == p + 2 || *end != ' ')
			    resWindow = None;
			ಅನ್ಯಥಾ
			{
			    p = serial = end + 1;
			    clientWindow = resWindow; // Remember in global
			}
			ಭಙ್ಗ;
		    ಪಕ್ಷ 'n':
			ಯದಿ (p[2] == ' ')
			    name = p + 3;
			ಭಙ್ಗ;
		    ಪಕ್ಷ 's':
			ಯದಿ (p[2] == ' ')
			    script = p + 3;
			ಭಙ್ಗ;
		    ಪಕ್ಷ 'E':
			ಯದಿ (p[2] == ' ')
			    enc = p + 3;
			ಭಙ್ಗ;
		}
		ಯಾವತ್ (*p != 0)
		    p++;
		p++;
	    }

	    ಯದಿ (script == ರಿಕ್ತ || name == ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    ಯದಿ (serverName != ರಿಕ್ತ && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		ಯದಿ (as_ಕೀಲಿಗಳು)
		    server_to_ಒಳಹರಿವು_buf(script);
		ಅನ್ಯಥಾ
		{
		    char_u      *res;

		    res = eval_client_ಅಭಿವ್ಯಕ್ತಿ_to_string(script);
		    ಯದಿ (resWindow != None)
		    {
			garray_T    reply;

			// Initialize the result property.
			ga_init2(&reply, 1, 100);
			(ಶೂನ್ಯ)ga_grow(&reply, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(reply.ga_data, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			reply.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			ಯದಿ (res != ರಿಕ್ತ)
			    ga_concat(&reply, res);
			ಅನ್ಯಥಾ
			{
			    ga_concat(&reply,
				   (char_u *)_(e_invalid_expression_received));
			    ga_append(&reply, 0);
			    GA_CONCAT_LITERAL(&reply, "-c 1");
			}
			ga_append(&reply, NUL);
			(ಶೂನ್ಯ)AppendPropCarefully(dpy, resWindow, commProperty,
						 reply.ga_data, reply.ga_len);
			ga_clear(&reply);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == 'r' && p[1] == 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ		    serial, gotSerial;
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
	    enc = ರಿಕ್ತ;
	    ಯಾವತ್ ((long_u)(p - propInfo) < ಅಂಕೆ_ವಸ್ತುಗಳು && *p == '-')
	    {
		ವಿಕಲ್ಪ (p[1])
		{
		    ಪಕ್ಷ 'r':
			ಯದಿ (p[2] == ' ')
			    res = p + 3;
			ಭಙ್ಗ;
		    ಪಕ್ಷ 'E':
			ಯದಿ (p[2] == ' ')
			    enc = p + 3;
			ಭಙ್ಗ;
		    ಪಕ್ಷ 's':
			ಯದಿ (sscanf((ವರ್ಣ *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			ಭಙ್ಗ;
		    ಪಕ್ಷ 'c':
			ಯದಿ (sscanf((ವರ್ಣ *)p + 2, " %d", &code) != 1)
			    code = 0;
			ಭಙ್ಗ;
		}
		ಯಾವತ್ (*p != 0)
		    p++;
		p++;
	    }

	    ಯದಿ (!gotSerial)
		ಅನುವರ್ತ;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    ಪ್ರತಿ (pcPtr = pendingCommands; pcPtr != ರಿಕ್ತ; pcPtr = pcPtr->nextPtr)
	    {
		ಯದಿ (serial != pcPtr->serial || pcPtr->result != ರಿಕ್ತ)
		    ಅನುವರ್ತ;

		pcPtr->code = code;
		res = serverConvert(enc, res, &tofree);
		ಯದಿ (tofree == ರಿಕ್ತ)
		    res = vim_strsave(res);
		pcPtr->result = res;
		ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == 'n' && p[1] == 0)
	{
	    Window	win = 0;
	    unsigned ಪೂರ್ಣಾಙ್ಕ u;
	    ಪೂರ್ಣಾಙ್ಕ		gotWindow;
	    char_u	*str;
	    ಸಂರಚನಾ	ServerReply *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    gotWindow = 0;
	    str = (char_u *)"";
	    enc = ರಿಕ್ತ;
	    ಯಾವತ್ ((long_u)(p - propInfo) < ಅಂಕೆ_ವಸ್ತುಗಳು && *p == '-')
	    {
		ವಿಕಲ್ಪ (p[1])
		{
		    ಪಕ್ಷ 'n':
			ಯದಿ (p[2] == ' ')
			    str = p + 3;
			ಭಙ್ಗ;
		    ಪಕ್ಷ 'E':
			ಯದಿ (p[2] == ' ')
			    enc = p + 3;
			ಭಙ್ಗ;
		    ಪಕ್ಷ 'w':
			ಯದಿ (sscanf((ವರ್ಣ *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    gotWindow = 1;
			}
			ಭಙ್ಗ;
		}
		ಯಾವತ್ (*p != 0)
		    p++;
		p++;
	    }

	    ಯದಿ (!gotWindow)
		ಅನುವರ್ತ;
	    str = serverConvert(enc, str, &tofree);
	    ಯದಿ ((r = ServerReplyFind(win, SROP_Add)) != ರಿಕ್ತ)
	    {
		ga_concat(&(r->strings), str);
		ga_append(&(r->strings), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((ವರ್ಣ *)winstr, "0x%x", (unsigned ಪೂರ್ಣಾಙ್ಕ)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	ಅನ್ಯಥಾ
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    ಯಾವತ್ (*p != 0)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    Window	window,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*ಮೌಲ್ಯ,		// Characters  to append to property.
    ಪೂರ್ಣಾಙ್ಕ		length)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
					       PropModeAppend, ಮೌಲ್ಯ, length);
    XSync(dpy, False);
    (ಶೂನ್ಯ) XSetErrorHandler(old_handler);
    ಪ್ರತ್ಯಾಗಮ got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
IsSerialName(char_u *str)
{
    ಪೂರ್ಣಾಙ್ಕ len = STRLEN(str);

    ಪ್ರತ್ಯಾಗಮ (len > 1 && vim_isdigit(str[len - 1]));
}
#endif	// FEAT_CLIENTSERVER
