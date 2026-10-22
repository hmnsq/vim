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

#ಆದರೆ (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ PendingCommand
{
    ಪೂರ್ಣಾಂಕ	    serial;	// Serial number expected in result.
    ಪೂರ್ಣಾಂಕ	    ಸಂಕೇತ_2;	// Result Code. 0 is OK
    char_u  *ಫಲಿತಾಂಶ;	// String result for command (malloc'ed).
			// NULL means command still pending.
    ರಚನೆ PendingCommand *ಮುಂದಿನ_Ptr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

ಸ್ಥಿರ PendingCommand *pendingCommands = ಖಾಲಿ;
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

ರಚನೆ Server_ಉತ್ತರ
{
    ಕಿಟಕಿ  id;
    garray_T ಅಕ್ಷರಸರಣಿಗಳು;
};
ಸ್ಥಿರ garray_T server_ಉತ್ತರ = { 0, 0, 0, 0, 0 };
ಎಣಿಕೆ Server_ಉತ್ತರ_Op { SROP_ಕಂಡುಹಿಡಿ, SROP_ಕೂಡಿಸು, SROP_Delete };

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*ಕೊನೆ_Cond)(ಶೂನ್ಯ *);

ರಚನೆ x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    ರಚನೆ x_cmdqueue	*ಮುಂದಿನ;
    ರಚನೆ x_cmdqueue	*prev;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ x_cmdqueue x_ಸರತಿ_T;

// dummy node, header for circular queue
ಸ್ಥಿರ x_ಸರತಿ_T ತಲೆ = {ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ};

/*
 * Forward declarations for procedures defined later in this file:
 */

ಸ್ಥಿರ ಕಿಟಕಿ	LookupName(Display *dpy, char_u *name, ಪೂರ್ಣಾಂಕ delete, char_u **loose);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	SendInit(Display *dpy);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	Do_ರಿಜಿಸ್ಟರ್_Name(Display *dpy, char_u *name);
ಸ್ಥಿರ ಶೂನ್ಯ	DeleteAnyLingerer(Display *dpy, ಕಿಟಕಿ w);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	GetRegProp(Display *dpy, char_u **regPropp, long_u *ಅಂಕೆ_Itemsp, ಪೂರ್ಣಾಂಕ domsg);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	WaitForPend(ಶೂನ್ಯ *p);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಕಿಟಕಿ_Valid(Display *dpy, ಕಿಟಕಿ w);
ಸ್ಥಿರ ಶೂನ್ಯ	ServerWait(Display *dpy, ಕಿಟಕಿ w, ಕೊನೆ_Cond ಕೊನೆ_Cond_2, ಶೂನ್ಯ *ಕೊನೆ_ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ localLoop, ಪೂರ್ಣಾಂಕ seconds);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	AppendPropCarefully(Display *display, ಕಿಟಕಿ ಕಿಟಕಿ_2, Atom property, char_u *ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	x_error_check(Display *dpy, XErrorEvent *error_event);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	IsSerialName(char_u *name);
ಸ್ಥಿರ ಶೂನ್ಯ	save_ಒಳ_ಸರತಿ(char_u *buf, long_u len);
ಸ್ಥಿರ ಶೂನ್ಯ	server_parse_ಸಂದೇಶ(Display *dpy, char_u *propInfo, long_u ಅಂಕೆ_ವಸ್ತುಗಳು);

// Private variables for the "server" functionality
ಸ್ಥಿರ Atom	registryProperty = None;
ಸ್ಥಿರ Atom	vimProperty = None;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	got_x_error = FALSE;

ಸ್ಥಿರ char_u	*ಖಾಲಿಗಳು_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    ಪೂರ್ಣಾಂಕ
server_ರಿಜಿಸ್ಟರ್_Name(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		res;
    char_u	*p = ಖಾಲಿ;

    res = Do_ರಿಜಿಸ್ಟರ್_Name(dpy, name);
    ಆದರೆ (res >= 0)
	ಹಿಂತಿರುಗಿ OK;

    i = 1;
    ಮಾಡು
    {
	ಆದರೆ (res < -1 || i >= 1000)
	{
	    ಸಂದೇಶ_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (p == ಖಾಲಿ)
	    p = alloc(STRLEN(name) + 10);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    res = -10;
	    ಮುಂದುವರಿ;
	}
	sprintf((ಅಕ್ಷರ *)p, "%s%d", name, i++);
	res = Do_ರಿಜಿಸ್ಟರ್_Name(dpy, p);
    }
    ಆಗ (res < 0)
	;
    vim_free(p);

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Do_ರಿಜಿಸ್ಟರ್_Name(Display *dpy, char_u *name)
{
    ಕಿಟಕಿ	w;
    XErrorHandler old_handler;
# define MAX_NAME_ಉದ್ದಳತೆ 100
    char_u	propInfo[MAX_NAME_ಉದ್ದಳತೆ + 20];

    ಆದರೆ (commProperty == None)
    {
	ಆದರೆ (SendInit(dpy) < 0)
	    ಹಿಂತಿರುಗಿ -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, ಖಾಲಿ);
    ಆದರೆ (w != (ಕಿಟಕಿ)0)
    {
	ಸ್ಥಿತಿ_3		ಸ್ಥಿತಿ_2;
	ಪೂರ್ಣಾಂಕ		dummyInt;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	dummyUns;
	ಕಿಟಕಿ		dummyWin;

	/*
	 * The name is currently registered.  See if the commWindow
	 * associated with the name exists.  If not, or if the commWindow
	 * is *our* commWindow, then just unregister the old name (this
	 * could happen if an application dies without cleaning up the
	 * registry).
	 */
	old_handler = XSetErrorHandler(x_error_check);
	ಸ್ಥಿತಿ_2 = XGetGeometry(dpy, w, &dummyWin, &dummyInt, &dummyInt,
				  &dummyUns, &dummyUns, &dummyUns, &dummyUns);
	(ಶೂನ್ಯ)XSetErrorHandler(old_handler);
	ಆದರೆ (ಸ್ಥಿತಿ_2 != Success && w != comm_ಕಿಟಕಿ)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    ಹಿಂತಿರುಗಿ -1;
	}
	(ಶೂನ್ಯ)LookupName(dpy, name, /*delete=*/TRUE, ಖಾಲಿ);
    }
    sprintf((ಅಕ್ಷರ *)propInfo, "%x %.*s", (int_u)comm_ಕಿಟಕಿ,
						       MAX_NAME_ಉದ್ದಳತೆ, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, ಬೇರು_ಕಿಟಕಿ_2(dpy, 0), registryProperty, XA_STRING, 8,
		    Prop_ಬಹುಲಕ_Append, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (ಶೂನ್ಯ)XSetErrorHandler(old_handler);

    ಆದರೆ (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	ಗಣ_vim_var_string(VV_SEND_SERVER, name, (ಪೂರ್ಣಾಂಕ)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ -2;
}

# ಆದರೆ defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    ಶೂನ್ಯ
server_Change_Registered_ಕಿಟಕಿ(
    Display	*dpy,		// Display to register with
    ಕಿಟಕಿ	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_ಉದ್ದಳತೆ + 20];

    comm_ಕಿಟಕಿ = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    ಆದರೆ (SendInit(dpy) < 0)
	ಹಿಂತಿರುಗಿ;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    ಆದರೆ (serverName != ಖಾಲಿ)
    {
	// Reinsert name if it was already registered
	(ಶೂನ್ಯ)LookupName(dpy, serverName, /*delete=*/TRUE, ಖಾಲಿ);
	sprintf((ಅಕ್ಷರ *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_ಉದ್ದಳತೆ, serverName);
	XChangeProperty(dpy, ಬೇರು_ಕಿಟಕಿ_2(dpy, 0), registryProperty, XA_STRING, 8,
			Prop_ಬಹುಲಕ_Append, (char_u *)propInfo,
			STRLEN(propInfo) + 1);
    }
    XUngrabServer(dpy);
}
# endif

/*
 * Send to an instance of Vim via the X display.
 * Returns 0 for OK, negative for an error.
 */
    ಪೂರ್ಣಾಂಕ
server_Send_ಗಮ್ಯ_Vim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**ಫಲಿತಾಂಶ,		// Result of eval'ed expression
    ಕಿಟಕಿ	*server,		// Actual ID of receiving app
    Bool	as_ಅಭಿವ್ಯಕ್ತಿ,			// Interpret as keystrokes or expr ?
    ಪೂರ್ಣಾಂಕ		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    ಪೂರ್ಣಾಂಕ		silent)			// don't complain about no server
{
    ಕಿಟಕಿ	    w;
    char_u	    *property;
    ಪೂರ್ಣಾಂಕ		    ಉದ್ದಳತೆ;
    ಪೂರ್ಣಾಂಕ		    res;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = ಖಾಲಿ;

    ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ)
	*ಫಲಿತಾಂಶ = ಖಾಲಿ;
    ಆದರೆ (name == ಖಾಲಿ || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    ಆದರೆ (commProperty == None && dpy != ಖಾಲಿ && SendInit(dpy) < 0)
	ಹಿಂತಿರುಗಿ -1;

# ಆದರೆ defined(FEAT_EVAL)
    ch_log(ಖಾಲಿ, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    ಆದರೆ (dpy == ಖಾಲಿ || (serverName != ಖಾಲಿ && STRICMP(name, serverName) == 0))
	ಹಿಂತಿರುಗಿ send_ಗಮ್ಯ_Local_Vim(cmd, as_ಅಭಿವ್ಯಕ್ತಿ, ಫಲಿತಾಂಶ);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    ಆಗ (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	ಆದರೆ (w != None)
	{
	    ಆದರೆ (!ಕಿಟಕಿ_Valid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, ಖಾಲಿ);
		VIM_CLEAR(loosename);
		ಮುಂದುವರಿ;
	    }
	}
	ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (w == None)
    {
	ಆದರೆ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ಹಿಂತಿರುಗಿ -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (loosename != ಖಾಲಿ)
	name = loosename;
    ಆದರೆ (server != ಖಾಲಿ)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    ಉದ್ದಳತೆ = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(ಉದ್ದಳತೆ + 30);

    sprintf((ಅಕ್ಷರ *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_ಅಭಿವ್ಯಕ್ತಿ ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    ಆದರೆ (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((ಅಕ್ಷರ *)property + ಉದ್ದಳತೆ, "%c-r %x %d",
						0, (int_u)comm_ಕಿಟಕಿ, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    ಉದ್ದಳತೆ += STRLEN(property + ಉದ್ದಳತೆ + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, ಉದ್ದಳತೆ + 1);
    vim_free(property);
    ಆದರೆ (res < 0)
    {
	emsg(_(e_failed_ಗಮ್ಯ_send_command_ಗಮ್ಯ_destination_ಕಾರ್ಯಕ್ರಮ));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (!as_ಅಭಿವ್ಯಕ್ತಿ) // There is no answer for this - Keys are sent async
	ಹಿಂತಿರುಗಿ 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.ಸಂಕೇತ_2 = 0;
    pending.ಫಲಿತಾಂಶ = ಖಾಲಿ;
    pending.ಮುಂದಿನ_Ptr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    ಆದರೆ (pendingCommands == &pending)
	pendingCommands = pending.ಮುಂದಿನ_Ptr;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	PendingCommand *pcPtr;

	ಗಾಗಿ (pcPtr = pendingCommands; pcPtr != ಖಾಲಿ; pcPtr = pcPtr->ಮುಂದಿನ_Ptr)
	    ಆದರೆ (pcPtr->ಮುಂದಿನ_Ptr == &pending)
	    {
		pcPtr->ಮುಂದಿನ_Ptr = pending.ಮುಂದಿನ_Ptr;
		ನಿಲ್ಲಿಸು;
	    }
    }

# ಆದರೆ defined(FEAT_EVAL)
    ch_log(ಖಾಲಿ, "serverSendToVim() result: %s",
	    pending.ಫಲಿತಾಂಶ == ಖಾಲಿ ? "NULL" : (ಅಕ್ಷರ *)pending.ಫಲಿತಾಂಶ);
# endif
    ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ)
	*ಫಲಿತಾಂಶ = pending.ಫಲಿತಾಂಶ;
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(pending.ಫಲಿತಾಂಶ);

    ಹಿಂತಿರುಗಿ pending.ಸಂಕೇತ_2 == 0 ? 0 : -1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
WaitForPend(ಶೂನ್ಯ *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    ಹಿಂತಿರುಗಿ pending->ಫಲಿತಾಂಶ != ಖಾಲಿ;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಿಟಕಿ_Valid(Display *dpy, ಕಿಟಕಿ w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    ಪೂರ್ಣಾಂಕ		    ಅಂಕೆ_Prop;
    ಪೂರ್ಣಾಂಕ		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &ಅಂಕೆ_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    ಆದರೆ (plist == ಖಾಲಿ || got_x_error)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_Prop; i++)
	ಆದರೆ (plist[i] == vimProperty)
	{
	    XFree(plist);
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    XFree(plist);
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ServerWait(
    Display	*dpy,
    ಕಿಟಕಿ	w,
    ಕೊನೆ_Cond	ಕೊನೆ_Cond_2,
    ಶೂನ್ಯ	*ಕೊನೆ_ದತ್ತಾಂಶ,
    ಪೂರ್ಣಾಂಕ		localLoop,
    ಪೂರ್ಣಾಂಕ		seconds)
{
    time_t	    ಆರಂಭ;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_ಸಂಖ್ಯೆ(dpy), &fds);
# ಇಲ್ಲದಿದ್ದರೆ
    ರಚನೆ pollfd   fds;

    fds.fd = Connection_ಸಂಖ್ಯೆ(dpy);
    fds.events = POLLIN;
# endif

    ಸಮಯ(&ಆರಂಭ);
    ಆಗ (TRUE)
    {
	ಆಗ (XCheck_ಕಿಟಕಿ_Event(dpy, comm_ಕಿಟಕಿ, Property_Change_ಮುಖವಾಡ, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	ಆದರೆ (ಕೊನೆ_Cond_2(ಕೊನೆ_ದತ್ತಾಂಶ) != 0)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (!ಕಿಟಕಿ_Valid(dpy, w))
	    ನಿಲ್ಲಿಸು;
	ಸಮಯ(&now);
	ಆದರೆ (seconds >= 0 && (now - ಆರಂಭ) >= seconds)
	    ನಿಲ್ಲಿಸು;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	ಆದರೆ (localLoop)
	{
# ifdef HAVE_SELECT
	    ರಚನೆ timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    ಆದರೆ (select(FD_SETSIZE, &fds, ಖಾಲಿ, ಖಾಲಿ, &tv) < 0)
		ನಿಲ್ಲಿಸು;
# ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		ನಿಲ್ಲಿಸು;
# endif
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
	    ui_delay((ದೀರ್ಘ)UI_MSEC_DELAY, TRUE);
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
server_Get_Vim_ಹೆಸರುಗಳು(Display *dpy)
{
    char_u	*regProp;
    char_u	*ನಮೂದು;
    char_u	*p;
    long_u	ಅಂಕೆ_ವಸ್ತುಗಳು;
    int_u	w;
    list_T	*list;

    ಆದರೆ (registryProperty == None)
    {
	ಆದರೆ (SendInit(dpy) < 0)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    list = list_alloc();
    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    /*
     * Read the registry property.
     */
    ಆದರೆ (GetRegProp(dpy, &regProp, &ಅಂಕೆ_ವಸ್ತುಗಳು, TRUE) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    /*
     * Scan all of the names out of the property.
     */
    ಗಾಗಿ (p = regProp; (long_u)(p - regProp) < ಅಂಕೆ_ವಸ್ತುಗಳು; p++)
    {
	ನಮೂದು = p;
	ಆಗ (*p != 0 && !ಸುರಕ್ಷಿತ_isspace(*p))
	    p++;
	ಆದರೆ (*p != 0)
	{
	    w = None;
	    ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)ನಮೂದು, "%x", &w);
	    ಆದರೆ (ಕಿಟಕಿ_Valid(dpy, (ಕಿಟಕಿ)w))
		list_append_string(list, p + 1, -1);
	    ಆಗ (*p != 0)
		p++;
	}
    }
    ಆದರೆ (regProp != ಖಾಲಿಗಳು_prop)
	XFree(regProp);
    ಹಿಂತಿರುಗಿ list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    ಸ್ಥಿರ ರಚನೆ Server_ಉತ್ತರ *
Server_ಉತ್ತರ_ಕಂಡುಹಿಡಿ(ಕಿಟಕಿ w, ಎಣಿಕೆ Server_ಉತ್ತರ_Op op)
{
    ರಚನೆ Server_ಉತ್ತರ *p;
    ರಚನೆ Server_ಉತ್ತರ e;
    ಪೂರ್ಣಾಂಕ		i;

    p = (ರಚನೆ Server_ಉತ್ತರ *) server_ಉತ್ತರ.ga_ದತ್ತಾಂಶ;
    ಗಾಗಿ (i = 0; i < server_ಉತ್ತರ.ga_len; i++, p++)
	ಆದರೆ (p->id == w)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (i >= server_ಉತ್ತರ.ga_len)
	p = ಖಾಲಿ;

    ಆದರೆ (p == ಖಾಲಿ && op == SROP_ಕೂಡಿಸು)
    {
	ಆದರೆ (server_ಉತ್ತರ.ga_growsize == 0)
	    ga_init2(&server_ಉತ್ತರ, ಗಾತ್ರ(ರಚನೆ Server_ಉತ್ತರ), 1);
	ಆದರೆ (ga_grow(&server_ಉತ್ತರ, 1) == OK)
	{
	    p = ((ರಚನೆ Server_ಉತ್ತರ *) server_ಉತ್ತರ.ga_ದತ್ತಾಂಶ)
		+ server_ಉತ್ತರ.ga_len;
	    e.id = w;
	    ga_init2(&e.ಅಕ್ಷರಸರಣಿಗಳು, 1, 100);
	    mch_memmove(p, &e, ಗಾತ್ರ(e));
	    server_ಉತ್ತರ.ga_len++;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p != ಖಾಲಿ && op == SROP_Delete)
    {
	ga_clear(&p->ಅಕ್ಷರಸರಣಿಗಳು);
	mch_memmove(p, p + 1, (server_ಉತ್ತರ.ga_len - i - 1) * ಗಾತ್ರ(*p));
	server_ಉತ್ತರ.ga_len--;
    }

    ಹಿಂತಿರುಗಿ p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    ಕಿಟಕಿ
server_Str_ಗಮ್ಯ_Win(char_u *str)
{
    ಚಿಹ್ನೆರಹಿತ  id = None;

    ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)str, "0x%x", &id);
    ಆದರೆ (id == None)
	semsg(_(e_invalid_server_id_ಬಳಸಿದ_str), str);

    ಹಿಂತಿರುಗಿ (ಕಿಟಕಿ)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    ಪೂರ್ಣಾಂಕ
server_Send_ಉತ್ತರ(char_u *name, char_u *str)
{
    char_u	*property;
    ಪೂರ್ಣಾಂಕ		ಉದ್ದಳತೆ;
    ಪೂರ್ಣಾಂಕ		res;
    Display	*dpy = X_DISPLAY;
    ಕಿಟಕಿ	win = server_Str_ಗಮ್ಯ_Win(name);

    ಆದರೆ (commProperty == None)
    {
	ಆದರೆ (SendInit(dpy) < 0)
	    ಹಿಂತಿರುಗಿ -2;
    }
    ಆದರೆ (!ಕಿಟಕಿ_Valid(dpy, win))
	ಹಿಂತಿರುಗಿ -1;

    ಉದ್ದಳತೆ = STRLEN(p_enc) + STRLEN(str) + 14;
    ಆದರೆ ((property = alloc(ಉದ್ದಳತೆ + 30)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    sprintf((ಅಕ್ಷರ *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)comm_ಕಿಟಕಿ);
    // Add length of what "%x" resulted in.
    ಉದ್ದಳತೆ += STRLEN(property + ಉದ್ದಳತೆ);
    res = AppendPropCarefully(dpy, win, commProperty, property, ಉದ್ದಳತೆ + 1);
    vim_free(property);

    ಹಿಂತಿರುಗಿ res;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Wait_For_ಉತ್ತರ(ಶೂನ್ಯ *p)
{
    ಕಿಟಕಿ  *w = (ಕಿಟಕಿ *) p;

    ಹಿಂತಿರುಗಿ Server_ಉತ್ತರ_ಕಂಡುಹಿಡಿ(*w, SROP_ಕಂಡುಹಿಡಿ) != ಖಾಲಿ;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    ಪೂರ್ಣಾಂಕ
server_Read_ಉತ್ತರ(
    Display	*dpy,
    ಕಿಟಕಿ	win,
    char_u	**str,
    ಪೂರ್ಣಾಂಕ		localLoop,
    ಪೂರ್ಣಾಂಕ		timeout)
{
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*s;
    ರಚನೆ	Server_ಉತ್ತರ *p;

    ServerWait(dpy, win, Wait_For_ಉತ್ತರ, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    ಆದರೆ ((p = Server_ಉತ್ತರ_ಕಂಡುಹಿಡಿ(win, SROP_ಕಂಡುಹಿಡಿ)) != ಖಾಲಿ && p->ಅಕ್ಷರಸರಣಿಗಳು.ga_len > 0)
    {
	*str = vim_strsave(p->ಅಕ್ಷರಸರಣಿಗಳು.ga_ದತ್ತಾಂಶ);
	len = STRLEN(*str) + 1;
	ಆದರೆ (len < p->ಅಕ್ಷರಸರಣಿಗಳು.ga_len)
	{
	    s = (char_u *) p->ಅಕ್ಷರಸರಣಿಗಳು.ga_ದತ್ತಾಂಶ;
	    mch_memmove(s, s + len, p->ಅಕ್ಷರಸರಣಿಗಳು.ga_len - len);
	    p->ಅಕ್ಷರಸರಣಿಗಳು.ga_len -= len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->ಅಕ್ಷರಸರಣಿಗಳು);
	    Server_ಉತ್ತರ_ಕಂಡುಹಿಡಿ(win, SROP_Delete);
	}
	ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    ಪೂರ್ಣಾಂಕ
server_Peek_ಉತ್ತರ(Display *dpy, ಕಿಟಕಿ win, char_u **str)
{
    ರಚನೆ Server_ಉತ್ತರ *p;

    ಆದರೆ ((p = Server_ಉತ್ತರ_ಕಂಡುಹಿಡಿ(win, SROP_ಕಂಡುಹಿಡಿ)) != ಖಾಲಿ && p->ಅಕ್ಷರಸರಣಿಗಳು.ga_len > 0)
    {
	ಆದರೆ (str != ಖಾಲಿ)
	    *str = p->ಅಕ್ಷರಸರಣಿಗಳು.ga_ದತ್ತಾಂಶ;
	ಹಿಂತಿರುಗಿ 1;
    }
    ಆದರೆ (!ಕಿಟಕಿ_Valid(dpy, win))
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    ಆದರೆ (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    ಆದರೆ (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    ಆದರೆ (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    ಆದರೆ (comm_ಕಿಟಕಿ == None)
    {
	comm_ಕಿಟಕಿ = XCreate_Simple_ಕಿಟಕಿ(dpy, XDefault_ಬೇರು_ಕಿಟಕಿ(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_ಒಳಹರಿವು(dpy, comm_ಕಿಟಕಿ, Property_Change_ಮುಖವಾಡ);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, comm_ಕಿಟಕಿ);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, comm_ಕಿಟಕಿ, vimProperty, XA_STRING,
		    8, Prop_ಬಹುಲಕ_Replace, (char_u *)VIM_ಆವೃತ್ತಿ_SHORT,
			(ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(VIM_ಆವೃತ್ತಿ_SHORT) + 1);

    XSync(dpy, False);
    (ಶೂನ್ಯ)XSetErrorHandler(old_handler);

    ಹಿಂತಿರುಗಿ got_x_error ? -1 : 0;
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
    ಸ್ಥಿರ ಕಿಟಕಿ
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    ಪೂರ್ಣಾಂಕ		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *ನಮೂದು;
    char_u	*p;
    long_u	ಅಂಕೆ_ವಸ್ತುಗಳು;
    int_u	return_ಮೌಲ್ಯ;

    /*
     * Read the registry property.
     */
    ಆದರೆ (GetRegProp(dpy, &regProp, &ಅಂಕೆ_ವಸ್ತುಗಳು, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ 0;

    /*
     * Scan the property for the desired name.
     */
    return_ಮೌಲ್ಯ = (int_u)None;
    ನಮೂದು = ಖಾಲಿ;	// Not needed, but eliminates compiler warning.
    ಗಾಗಿ (p = regProp; (long_u)(p - regProp) < ಅಂಕೆ_ವಸ್ತುಗಳು; )
    {
	ನಮೂದು = p;
	ಆಗ (*p != 0 && !ಸುರಕ್ಷಿತ_isspace(*p))
	    p++;
	ಆದರೆ (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)ನಮೂದು, "%x", &return_ಮೌಲ್ಯ);
	    ನಿಲ್ಲಿಸು;
	}
	ಆಗ (*p != 0)
	    p++;
	p++;
    }

    ಆದರೆ (loose != ಖಾಲಿ && return_ಮೌಲ್ಯ == (int_u)None && !IsSerialName(name))
    {
	ಗಾಗಿ (p = regProp; (long_u)(p - regProp) < ಅಂಕೆ_ವಸ್ತುಗಳು; )
	{
	    ನಮೂದು = p;
	    ಆಗ (*p != 0 && !ಸುರಕ್ಷಿತ_isspace(*p))
		p++;
	    ಆದರೆ (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)ನಮೂದು, "%x", &return_ಮೌಲ್ಯ) == 1
			&& return_ಮೌಲ್ಯ != (int_u)None)
		    *loose = vim_strsave(p + 1);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆಗ (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    ಆದರೆ (delete && return_ಮೌಲ್ಯ != (int_u)None)
    {
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2;

	ಆಗ (*p != 0)
	    p++;
	p++;
	ಎಣಿಕೆ_2 = ಅಂಕೆ_ವಸ್ತುಗಳು - (p - regProp);
	ಆದರೆ (ಎಣಿಕೆ_2 > 0)
	    mch_memmove(ನಮೂದು, p, ಎಣಿಕೆ_2);
	XChangeProperty(dpy, ಬೇರು_ಕಿಟಕಿ_2(dpy, 0), registryProperty, XA_STRING,
			8, Prop_ಬಹುಲಕ_Replace, regProp,
			(ಪೂರ್ಣಾಂಕ)(ಅಂಕೆ_ವಸ್ತುಗಳು - (p - ನಮೂದು)));
	XSync(dpy, False);
    }

    ಆದರೆ (regProp != ಖಾಲಿಗಳು_prop)
	XFree(regProp);
    ಹಿಂತಿರುಗಿ (ಕಿಟಕಿ)return_ಮೌಲ್ಯ;
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
    ಕಿಟಕಿ	win)	// Window to remove
{
    char_u	*regProp, *ನಮೂದು = ಖಾಲಿ;
    char_u	*p;
    long_u	ಅಂಕೆ_ವಸ್ತುಗಳು;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    ಆದರೆ (GetRegProp(dpy, &regProp, &ಅಂಕೆ_ವಸ್ತುಗಳು, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // Scan the property for the window id.
    ಗಾಗಿ (p = regProp; (long_u)(p - regProp) < ಅಂಕೆ_ವಸ್ತುಗಳು; )
    {
	ಆದರೆ (*p != 0)
	{
	    ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)p, "%x", &wwin);
	    ಆದರೆ ((ಕಿಟಕಿ)wwin == win)
	    {
		ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_Half;

		// Copy down the remainder to delete entry
		ನಮೂದು = p;
		ಆಗ (*p != 0)
		    p++;
		p++;
		ಕೊನೆಯ_Half = ಅಂಕೆ_ವಸ್ತುಗಳು - (p - regProp);
		ಆದರೆ (ಕೊನೆಯ_Half > 0)
		    mch_memmove(ನಮೂದು, p, ಕೊನೆಯ_Half);
		ಅಂಕೆ_ವಸ್ತುಗಳು = (ನಮೂದು - regProp) + ಕೊನೆಯ_Half;
		p = ನಮೂದು;
		ಮುಂದುವರಿ;
	    }
	}
	ಆಗ (*p != 0)
	    p++;
	p++;
    }

    ಆದರೆ (ನಮೂದು != ಖಾಲಿ)
    {
	XChangeProperty(dpy, ಬೇರು_ಕಿಟಕಿ_2(dpy, 0), registryProperty,
			XA_STRING, 8, Prop_ಬಹುಲಕ_Replace, regProp,
			(ಪೂರ್ಣಾಂಕ)(p - regProp));
	XSync(dpy, False);
    }

    ಆದರೆ (regProp != ಖಾಲಿಗಳು_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*ಅಂಕೆ_Itemsp,
    ಪೂರ್ಣಾಂಕ		domsg)		// When TRUE give error message.
{
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ, actualFormat;
    long_u	ಬೈಟುಗಳು_After;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = ಖಾಲಿ;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    ಫಲಿತಾಂಶ = XGet_ಕಿಟಕಿ_Property(dpy, ಬೇರು_ಕಿಟಕಿ_2(dpy, 0), registryProperty, 0L,
				(ದೀರ್ಘ)MAX_PROP_ಪದಗಳು, False,
				XA_STRING, &actualType,
				&actualFormat, ಅಂಕೆ_Itemsp, &ಬೈಟುಗಳು_After,
				regPropp);

    XSync(dpy, FALSE);
    (ಶೂನ್ಯ)XSetErrorHandler(old_handler);
    ಆದರೆ (got_x_error)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*ಅಂಕೆ_Itemsp = 0;
	*regPropp = ಖಾಲಿಗಳು_prop;
	ಹಿಂತಿರುಗಿ OK;
    }

    // If the property is improperly formed, then delete it.
    ಆದರೆ (ಫಲಿತಾಂಶ != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	ಆದರೆ (*regPropp != ಖಾಲಿ)
	    XFree(*regPropp);
	XDeleteProperty(dpy, ಬೇರು_ಕಿಟಕಿ_2(dpy, 0), registryProperty);
	ಆದರೆ (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ		ತಕ್ಷಣ)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ, actualFormat;
    long_u	ಅಂಕೆ_ವಸ್ತುಗಳು, ಬೈಟುಗಳು_After;
    Atom	actualType;

    ಆದರೆ (eventPtr != ಖಾಲಿ)
    {
	ಆದರೆ (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_ಮೌಲ್ಯ)
	    ಹಿಂತಿರುಗಿ;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = ಖಾಲಿ;
    ಫಲಿತಾಂಶ = XGet_ಕಿಟಕಿ_Property(dpy, comm_ಕಿಟಕಿ, commProperty, 0L,
				(ದೀರ್ಘ)MAX_PROP_ಪದಗಳು, True,
				XA_STRING, &actualType,
				&actualFormat, &ಅಂಕೆ_ವಸ್ತುಗಳು, &ಬೈಟುಗಳು_After,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    ಆದರೆ (ಫಲಿತಾಂಶ != Success || actualType != XA_STRING || actualFormat != 8)
    {
	ಆದರೆ (propInfo != ಖಾಲಿ)
	    XFree(propInfo);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ತಕ್ಷಣ)
	server_parse_ಸಂದೇಶ(dpy, propInfo, ಅಂಕೆ_ವಸ್ತುಗಳು);
    ಇಲ್ಲದಿದ್ದರೆ
	save_ಒಳ_ಸರತಿ(propInfo, ಅಂಕೆ_ವಸ್ತುಗಳು);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
save_ಒಳ_ಸರತಿ(char_u *propInfo, long_u len)
{
    x_ಸರತಿ_T *ಗಂಟು;

    ಗಂಟು = ALLOC_ONE(x_ಸರತಿ_T);
    ಆದರೆ (ಗಂಟು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;	    // out of memory
    ಗಂಟು->propInfo = propInfo;
    ಗಂಟು->len = len;

    ಆದರೆ (ತಲೆ.ಮುಂದಿನ == ಖಾಲಿ)   // initialize circular queue
    {
	ತಲೆ.ಮುಂದಿನ = &ತಲೆ;
	ತಲೆ.prev = &ತಲೆ;
    }

    // insert node at tail of queue
    ಗಂಟು->ಮುಂದಿನ = &ತಲೆ;
    ಗಂಟು->prev = ತಲೆ.prev;
    ತಲೆ.prev->ಮುಂದಿನ = ಗಂಟು;
    ತಲೆ.prev = ಗಂಟು;
}

/*
 * Parses queued clientserver messages.
 */
    ಶೂನ್ಯ
server_parse_messages(ಶೂನ್ಯ)
{
    x_ಸರತಿ_T	*ಗಂಟು;

    ಆದರೆ (!X_DISPLAY)
	ಹಿಂತಿರುಗಿ; // cannot happen?
    ಆಗ (ತಲೆ.ಮುಂದಿನ != ಖಾಲಿ && ತಲೆ.ಮುಂದಿನ != &ತಲೆ)
    {
	ಗಂಟು = ತಲೆ.ಮುಂದಿನ;
	ತಲೆ.ಮುಂದಿನ = ಗಂಟು->ಮುಂದಿನ;
	ಗಂಟು->ಮುಂದಿನ->prev = ಗಂಟು->prev;
	server_parse_ಸಂದೇಶ(X_DISPLAY, ಗಂಟು->propInfo, ಗಂಟು->len);
	vim_free(ಗಂಟು);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    ಪೂರ್ಣಾಂಕ
server_ಕಾಯುವಿಕೆ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ತಲೆ.ಮುಂದಿನ != ಖಾಲಿ && ತಲೆ.ಮುಂದಿನ != &ತಲೆ;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
server_parse_ಸಂದೇಶ(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	ಅಂಕೆ_ವಸ್ತುಗಳು)  // The size of propInfo in bytes.
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		ಸಂಕೇತ_2;
    char_u	*tofree;

# ಆದರೆ defined(FEAT_EVAL)
    ch_log(ಖಾಲಿ, "server_parse_message() numItems: %ld", ಅಂಕೆ_ವಸ್ತುಗಳು);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    ಗಾಗಿ (p = propInfo; (long_u)(p - propInfo) < ಅಂಕೆ_ವಸ್ತುಗಳು; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	ಆದರೆ (*p == 0)
	{
	    p++;
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    ಕಿಟಕಿ	res_ಕಿಟಕಿ;
	    char_u	*name, *script, *serial, *ಕೊನೆ;
	    Bool	as_ಕೀಲಿಗಳು = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = ಖಾಲಿ;
	    res_ಕಿಟಕಿ = None;
	    serial = (char_u *)"";
	    script = ಖಾಲಿ;
	    enc = ಖಾಲಿ;
	    ಆಗ ((long_u)(p - propInfo) < ಅಂಕೆ_ವಸ್ತುಗಳು && *p == '-')
	    {
# ಆದರೆ defined(FEAT_EVAL)
		ch_log(ಖಾಲಿ, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		ಆಯ್ಕೆ (p[1])
		{
		    ಸ್ಥಿತಿ 'r':
			ಕೊನೆ = skipwhite(p + 2);
			res_ಕಿಟಕಿ = 0;
			ಆಗ (vim_isxdigit(*ಕೊನೆ))
			{
			    res_ಕಿಟಕಿ = 16 * res_ಕಿಟಕಿ + (long_u)hex2nr(*ಕೊನೆ);
			    ++ಕೊನೆ;
			}
			ಆದರೆ (ಕೊನೆ == p + 2 || *ಕೊನೆ != ' ')
			    res_ಕಿಟಕಿ = None;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    p = serial = ಕೊನೆ + 1;
			    client_ಕಿಟಕಿ = res_ಕಿಟಕಿ; // Remember in global
			}
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'n':
			ಆದರೆ (p[2] == ' ')
			    name = p + 3;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 's':
			ಆದರೆ (p[2] == ' ')
			    script = p + 3;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'E':
			ಆದರೆ (p[2] == ' ')
			    enc = p + 3;
			ನಿಲ್ಲಿಸು;
		}
		ಆಗ (*p != 0)
		    p++;
		p++;
	    }

	    ಆದರೆ (script == ಖಾಲಿ || name == ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    ಆದರೆ (serverName != ಖಾಲಿ && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		ಆದರೆ (as_ಕೀಲಿಗಳು)
		    server_ಗಮ್ಯ_ಒಳಹರಿವು_buf(script);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    char_u      *res;

		    res = eval_client_ಅಭಿವ್ಯಕ್ತಿ_ಗಮ್ಯ_string(script);
		    ಆದರೆ (res_ಕಿಟಕಿ != None)
		    {
			garray_T    ಉತ್ತರ;

			// Initialize the result property.
			ga_init2(&ಉತ್ತರ, 1, 100);
			(ಶೂನ್ಯ)ga_grow(&ಉತ್ತರ, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(ಉತ್ತರ.ga_ದತ್ತಾಂಶ, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			ಉತ್ತರ.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			ಆದರೆ (res != ಖಾಲಿ)
			    ga_concat(&ಉತ್ತರ, res);
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ga_concat(&ಉತ್ತರ,
				   (char_u *)_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_received));
			    ga_append(&ಉತ್ತರ, 0);
			    GA_CONCAT_LITERAL(&ಉತ್ತರ, "-c 1");
			}
			ga_append(&ಉತ್ತರ, NUL);
			(ಶೂನ್ಯ)AppendPropCarefully(dpy, res_ಕಿಟಕಿ, commProperty,
						 ಉತ್ತರ.ga_ದತ್ತಾಂಶ, ಉತ್ತರ.ga_len);
			ga_clear(&ಉತ್ತರ);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'r' && p[1] == 0)
	{
	    ಪೂರ್ಣಾಂಕ		    serial, gotSerial;
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
	    ಸಂಕೇತ_2 = 0;
	    enc = ಖಾಲಿ;
	    ಆಗ ((long_u)(p - propInfo) < ಅಂಕೆ_ವಸ್ತುಗಳು && *p == '-')
	    {
		ಆಯ್ಕೆ (p[1])
		{
		    ಸ್ಥಿತಿ 'r':
			ಆದರೆ (p[2] == ' ')
			    res = p + 3;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'E':
			ಆದರೆ (p[2] == ' ')
			    enc = p + 3;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 's':
			ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'c':
			ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)p + 2, " %d", &ಸಂಕೇತ_2) != 1)
			    ಸಂಕೇತ_2 = 0;
			ನಿಲ್ಲಿಸು;
		}
		ಆಗ (*p != 0)
		    p++;
		p++;
	    }

	    ಆದರೆ (!gotSerial)
		ಮುಂದುವರಿ;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    ಗಾಗಿ (pcPtr = pendingCommands; pcPtr != ಖಾಲಿ; pcPtr = pcPtr->ಮುಂದಿನ_Ptr)
	    {
		ಆದರೆ (serial != pcPtr->serial || pcPtr->ಫಲಿತಾಂಶ != ಖಾಲಿ)
		    ಮುಂದುವರಿ;

		pcPtr->ಸಂಕೇತ_2 = ಸಂಕೇತ_2;
		res = serverConvert(enc, res, &tofree);
		ಆದರೆ (tofree == ಖಾಲಿ)
		    res = vim_strsave(res);
		pcPtr->ಫಲಿತಾಂಶ = res;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'n' && p[1] == 0)
	{
	    ಕಿಟಕಿ	win = 0;
	    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ u;
	    ಪೂರ್ಣಾಂಕ		got_ಕಿಟಕಿ;
	    char_u	*str;
	    ರಚನೆ	Server_ಉತ್ತರ *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    got_ಕಿಟಕಿ = 0;
	    str = (char_u *)"";
	    enc = ಖಾಲಿ;
	    ಆಗ ((long_u)(p - propInfo) < ಅಂಕೆ_ವಸ್ತುಗಳು && *p == '-')
	    {
		ಆಯ್ಕೆ (p[1])
		{
		    ಸ್ಥಿತಿ 'n':
			ಆದರೆ (p[2] == ' ')
			    str = p + 3;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'E':
			ಆದರೆ (p[2] == ' ')
			    enc = p + 3;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'w':
			ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    got_ಕಿಟಕಿ = 1;
			}
			ನಿಲ್ಲಿಸು;
		}
		ಆಗ (*p != 0)
		    p++;
		p++;
	    }

	    ಆದರೆ (!got_ಕಿಟಕಿ)
		ಮುಂದುವರಿ;
	    str = serverConvert(enc, str, &tofree);
	    ಆದರೆ ((r = Server_ಉತ್ತರ_ಕಂಡುಹಿಡಿ(win, SROP_ಕೂಡಿಸು)) != ಖಾಲಿ)
	    {
		ga_concat(&(r->ಅಕ್ಷರಸರಣಿಗಳು), str);
		ga_append(&(r->ಅಕ್ಷರಸರಣಿಗಳು), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((ಅಕ್ಷರ *)winstr, "0x%x", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    ಆಗ (*p != 0)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    ಕಿಟಕಿ	ಕಿಟಕಿ_2,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*ಮೌಲ್ಯ,		// Characters  to append to property.
    ಪೂರ್ಣಾಂಕ		ಉದ್ದಳತೆ)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, ಕಿಟಕಿ_2, property, XA_STRING, 8,
					       Prop_ಬಹುಲಕ_Append, ಮೌಲ್ಯ, ಉದ್ದಳತೆ);
    XSync(dpy, False);
    (ಶೂನ್ಯ) XSetErrorHandler(old_handler);
    ಹಿಂತಿರುಗಿ got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
x_error_check(Display *dpy ಬಳಸದ, XErrorEvent *error_event ಬಳಸದ)
{
    got_x_error = TRUE;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
IsSerialName(char_u *str)
{
    ಪೂರ್ಣಾಂಕ len = STRLEN(str);

    ಹಿಂತಿರುಗಿ (len > 1 && vim_isdigit(str[len - 1]));
}
#ಆದರೆ_ಅಂತ್ಯ	// FEAT_CLIENTSERVER
