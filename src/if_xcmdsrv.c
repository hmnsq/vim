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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਜੇ (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ PendingCommand
{
    ਪੂਰਨ	    serial;	// Serial number expected in result.
    ਪੂਰਨ	    ਕੋਡ;	// Result Code. 0 is OK
    char_u  *ਨਤੀਜਾ;	// String result for command (malloc'ed).
			// NULL means command still pending.
    ਬਣਤਰ PendingCommand *ਅਗਲਾ_Ptr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

ਸਥਿਰ PendingCommand *pendingCommands = ਨਿਰਾ;
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

# define MAX_PROP_ਸ਼ਬਦ 100000

ਬਣਤਰ Server_ਜਵਾਬ
{
    ਖਿੜਕੀ  id;
    garray_T ਲੜੀਆਂ;
};
ਸਥਿਰ garray_T server_ਜਵਾਬ = { 0, 0, 0, 0, 0 };
ਗਿਣਤੀ Server_ਜਵਾਬ_Op { SROP_ਲੱਭੋ, SROP_ਜੋੜੋ, SROP_Delete };

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*ਸਿਰਾ_Cond)(ਖਾਲੀ *);

ਬਣਤਰ x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    ਬਣਤਰ x_cmdqueue	*ਅਗਲਾ;
    ਬਣਤਰ x_cmdqueue	*prev;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ x_cmdqueue x_ਕਤਾਰ_T;

// dummy node, header for circular queue
ਸਥਿਰ x_ਕਤਾਰ_T ਸਿਰ = {ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ};

/*
 * Forward declarations for procedures defined later in this file:
 */

ਸਥਿਰ ਖਿੜਕੀ	LookupName(Display *dpy, char_u *name, ਪੂਰਨ delete, char_u **loose);
ਸਥਿਰ ਪੂਰਨ	SendInit(Display *dpy);
ਸਥਿਰ ਪੂਰਨ	Do_ਰਜਿਸਟਰ_Name(Display *dpy, char_u *name);
ਸਥਿਰ ਖਾਲੀ	DeleteAnyLingerer(Display *dpy, ਖਿੜਕੀ w);
ਸਥਿਰ ਪੂਰਨ	GetRegProp(Display *dpy, char_u **regPropp, long_u *ਅੰਕ_Itemsp, ਪੂਰਨ domsg);
ਸਥਿਰ ਪੂਰਨ	WaitForPend(ਖਾਲੀ *p);
ਸਥਿਰ ਪੂਰਨ	ਖਿੜਕੀ_Valid(Display *dpy, ਖਿੜਕੀ w);
ਸਥਿਰ ਖਾਲੀ	ServerWait(Display *dpy, ਖਿੜਕੀ w, ਸਿਰਾ_Cond ਸਿਰਾ_Cond_2, ਖਾਲੀ *ਸਿਰਾ_ਅੰਕੜੇ, ਪੂਰਨ localLoop, ਪੂਰਨ seconds);
ਸਥਿਰ ਪੂਰਨ	AppendPropCarefully(Display *display, ਖਿੜਕੀ ਖਿੜਕੀ_2, Atom property, char_u *ਮੁੱਲ, ਪੂਰਨ ਲੰਮਾਈ);
ਸਥਿਰ ਪੂਰਨ	x_error_check(Display *dpy, XErrorEvent *error_event);
ਸਥਿਰ ਪੂਰਨ	IsSerialName(char_u *name);
ਸਥਿਰ ਖਾਲੀ	save_ਵਿੱਚ_ਕਤਾਰ(char_u *buf, long_u len);
ਸਥਿਰ ਖਾਲੀ	server_parse_ਸੁਨੇਹਾ(Display *dpy, char_u *propInfo, long_u ਅੰਕ_ਚੀਜ਼ਾਂ);

// Private variables for the "server" functionality
ਸਥਿਰ Atom	registryProperty = None;
ਸਥਿਰ Atom	vimProperty = None;
ਸਥਿਰ ਪੂਰਨ	got_x_error = FALSE;

ਸਥਿਰ char_u	*ਖਾਲੀ_ਥਾਵਾਂ_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    ਪੂਰਨ
server_ਰਜਿਸਟਰ_Name(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		res;
    char_u	*p = ਨਿਰਾ;

    res = Do_ਰਜਿਸਟਰ_Name(dpy, name);
    ਜੇ (res >= 0)
	ਵਾਪਸ OK;

    i = 1;
    ਕਰੋ
    {
	ਜੇ (res < -1 || i >= 1000)
	{
	    ਸੁਨੇਹਾ_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (p == ਨਿਰਾ)
	    p = alloc(STRLEN(name) + 10);
	ਜੇ (p == ਨਿਰਾ)
	{
	    res = -10;
	    ਜਾਰੀ;
	}
	sprintf((ਅੱਖਰ *)p, "%s%d", name, i++);
	res = Do_ਰਜਿਸਟਰ_Name(dpy, p);
    }
    ਜਦੋਂ (res < 0)
	;
    vim_free(p);

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
Do_ਰਜਿਸਟਰ_Name(Display *dpy, char_u *name)
{
    ਖਿੜਕੀ	w;
    XErrorHandler old_handler;
# define MAX_NAME_ਲੰਮਾਈ 100
    char_u	propInfo[MAX_NAME_ਲੰਮਾਈ + 20];

    ਜੇ (commProperty == None)
    {
	ਜੇ (SendInit(dpy) < 0)
	    ਵਾਪਸ -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, ਨਿਰਾ);
    ਜੇ (w != (ਖਿੜਕੀ)0)
    {
	ਸਥਿਤੀ_2		ਸਥਿਤੀ;
	ਪੂਰਨ		dummyInt;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	dummyUns;
	ਖਿੜਕੀ		dummyWin;

	/*
	 * The name is currently registered.  See if the commWindow
	 * associated with the name exists.  If not, or if the commWindow
	 * is *our* commWindow, then just unregister the old name (this
	 * could happen if an application dies without cleaning up the
	 * registry).
	 */
	old_handler = XSetErrorHandler(x_error_check);
	ਸਥਿਤੀ = XGetGeometry(dpy, w, &dummyWin, &dummyInt, &dummyInt,
				  &dummyUns, &dummyUns, &dummyUns, &dummyUns);
	(ਖਾਲੀ)XSetErrorHandler(old_handler);
	ਜੇ (ਸਥਿਤੀ != Success && w != comm_ਖਿੜਕੀ)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    ਵਾਪਸ -1;
	}
	(ਖਾਲੀ)LookupName(dpy, name, /*delete=*/TRUE, ਨਿਰਾ);
    }
    sprintf((ਅੱਖਰ *)propInfo, "%x %.*s", (int_u)comm_ਖਿੜਕੀ,
						       MAX_NAME_ਲੰਮਾਈ, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, ਜੜ੍ਹ_ਖਿੜਕੀ_2(dpy, 0), registryProperty, XA_STRING, 8,
		    Prop_ਬਹੁਲਕ_Append, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (ਖਾਲੀ)XSetErrorHandler(old_handler);

    ਜੇ (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	ਸੈੱਟ_vim_var_string(VV_SEND_SERVER, name, (ਪੂਰਨ)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	ਵਾਪਸ 0;
    }
    ਵਾਪਸ -2;
}

# ਜੇ defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    ਖਾਲੀ
server_Change_Registered_ਖਿੜਕੀ(
    Display	*dpy,		// Display to register with
    ਖਿੜਕੀ	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_ਲੰਮਾਈ + 20];

    comm_ਖਿੜਕੀ = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    ਜੇ (SendInit(dpy) < 0)
	ਵਾਪਸ;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    ਜੇ (serverName != ਨਿਰਾ)
    {
	// Reinsert name if it was already registered
	(ਖਾਲੀ)LookupName(dpy, serverName, /*delete=*/TRUE, ਨਿਰਾ);
	sprintf((ਅੱਖਰ *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_ਲੰਮਾਈ, serverName);
	XChangeProperty(dpy, ਜੜ੍ਹ_ਖਿੜਕੀ_2(dpy, 0), registryProperty, XA_STRING, 8,
			Prop_ਬਹੁਲਕ_Append, (char_u *)propInfo,
			STRLEN(propInfo) + 1);
    }
    XUngrabServer(dpy);
}
# endif

/*
 * Send to an instance of Vim via the X display.
 * Returns 0 for OK, negative for an error.
 */
    ਪੂਰਨ
server_Send_ਮੰਜ਼ਿਲ_Vim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**ਨਤੀਜਾ,		// Result of eval'ed expression
    ਖਿੜਕੀ	*server,		// Actual ID of receiving app
    Bool	as_ਵਿਅੰਜਕ,			// Interpret as keystrokes or expr ?
    ਪੂਰਨ		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    ਪੂਰਨ		silent)			// don't complain about no server
{
    ਖਿੜਕੀ	    w;
    char_u	    *property;
    ਪੂਰਨ		    ਲੰਮਾਈ;
    ਪੂਰਨ		    res;
    ਸਥਿਰ ਪੂਰਨ	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = ਨਿਰਾ;

    ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ)
	*ਨਤੀਜਾ = ਨਿਰਾ;
    ਜੇ (name == ਨਿਰਾ || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    ਜੇ (commProperty == None && dpy != ਨਿਰਾ && SendInit(dpy) < 0)
	ਵਾਪਸ -1;

# ਜੇ defined(FEAT_EVAL)
    ch_log(ਨਿਰਾ, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    ਜੇ (dpy == ਨਿਰਾ || (serverName != ਨਿਰਾ && STRICMP(name, serverName) == 0))
	ਵਾਪਸ send_ਮੰਜ਼ਿਲ_Local_Vim(cmd, as_ਵਿਅੰਜਕ, ਨਤੀਜਾ);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    ਜਦੋਂ (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	ਜੇ (w != None)
	{
	    ਜੇ (!ਖਿੜਕੀ_Valid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, ਨਿਰਾ);
		VIM_CLEAR(loosename);
		ਜਾਰੀ;
	    }
	}
	ਰੋਕੋ;
    }
    ਜੇ (w == None)
    {
	ਜੇ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ਵਾਪਸ -1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (loosename != ਨਿਰਾ)
	name = loosename;
    ਜੇ (server != ਨਿਰਾ)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    ਲੰਮਾਈ = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(ਲੰਮਾਈ + 30);

    sprintf((ਅੱਖਰ *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_ਵਿਅੰਜਕ ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    ਜੇ (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((ਅੱਖਰ *)property + ਲੰਮਾਈ, "%c-r %x %d",
						0, (int_u)comm_ਖਿੜਕੀ, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    ਲੰਮਾਈ += STRLEN(property + ਲੰਮਾਈ + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, ਲੰਮਾਈ + 1);
    vim_free(property);
    ਜੇ (res < 0)
    {
	emsg(_(e_failed_ਮੰਜ਼ਿਲ_send_command_ਮੰਜ਼ਿਲ_destination_ਪ੍ਰੋਗਰਾਮ));
	ਵਾਪਸ -1;
    }

    ਜੇ (!as_ਵਿਅੰਜਕ) // There is no answer for this - Keys are sent async
	ਵਾਪਸ 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.ਕੋਡ = 0;
    pending.ਨਤੀਜਾ = ਨਿਰਾ;
    pending.ਅਗਲਾ_Ptr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    ਜੇ (pendingCommands == &pending)
	pendingCommands = pending.ਅਗਲਾ_Ptr;
    ਨਹੀਂਤਾਂ
    {
	PendingCommand *pcPtr;

	ਲਈ (pcPtr = pendingCommands; pcPtr != ਨਿਰਾ; pcPtr = pcPtr->ਅਗਲਾ_Ptr)
	    ਜੇ (pcPtr->ਅਗਲਾ_Ptr == &pending)
	    {
		pcPtr->ਅਗਲਾ_Ptr = pending.ਅਗਲਾ_Ptr;
		ਰੋਕੋ;
	    }
    }

# ਜੇ defined(FEAT_EVAL)
    ch_log(ਨਿਰਾ, "serverSendToVim() result: %s",
	    pending.ਨਤੀਜਾ == ਨਿਰਾ ? "NULL" : (ਅੱਖਰ *)pending.ਨਤੀਜਾ);
# endif
    ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ)
	*ਨਤੀਜਾ = pending.ਨਤੀਜਾ;
    ਨਹੀਂਤਾਂ
	vim_free(pending.ਨਤੀਜਾ);

    ਵਾਪਸ pending.ਕੋਡ == 0 ? 0 : -1;
}

    ਸਥਿਰ ਪੂਰਨ
WaitForPend(ਖਾਲੀ *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    ਵਾਪਸ pending->ਨਤੀਜਾ != ਨਿਰਾ;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    ਸਥਿਰ ਪੂਰਨ
ਖਿੜਕੀ_Valid(Display *dpy, ਖਿੜਕੀ w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    ਪੂਰਨ		    ਅੰਕ_Prop;
    ਪੂਰਨ		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &ਅੰਕ_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    ਜੇ (plist == ਨਿਰਾ || got_x_error)
	ਵਾਪਸ FALSE;

    ਲਈ (i = 0; i < ਅੰਕ_Prop; i++)
	ਜੇ (plist[i] == vimProperty)
	{
	    XFree(plist);
	    ਵਾਪਸ TRUE;
	}
    XFree(plist);
    ਵਾਪਸ FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    ਸਥਿਰ ਖਾਲੀ
ServerWait(
    Display	*dpy,
    ਖਿੜਕੀ	w,
    ਸਿਰਾ_Cond	ਸਿਰਾ_Cond_2,
    ਖਾਲੀ	*ਸਿਰਾ_ਅੰਕੜੇ,
    ਪੂਰਨ		localLoop,
    ਪੂਰਨ		seconds)
{
    time_t	    ਸ਼ੁਰੂ;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_ਸੰਖਿਆ(dpy), &fds);
# ਨਹੀਂਤਾਂ
    ਬਣਤਰ pollfd   fds;

    fds.fd = Connection_ਸੰਖਿਆ(dpy);
    fds.events = POLLIN;
# endif

    ਸਮਾਂ(&ਸ਼ੁਰੂ);
    ਜਦੋਂ (TRUE)
    {
	ਜਦੋਂ (XCheck_ਖਿੜਕੀ_Event(dpy, comm_ਖਿੜਕੀ, Property_Change_ਨਕਾਬ, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	ਜੇ (ਸਿਰਾ_Cond_2(ਸਿਰਾ_ਅੰਕੜੇ) != 0)
	    ਰੋਕੋ;
	ਜੇ (!ਖਿੜਕੀ_Valid(dpy, w))
	    ਰੋਕੋ;
	ਸਮਾਂ(&now);
	ਜੇ (seconds >= 0 && (now - ਸ਼ੁਰੂ) >= seconds)
	    ਰੋਕੋ;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	ਜੇ (localLoop)
	{
# ifdef HAVE_SELECT
	    ਬਣਤਰ timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    ਜੇ (select(FD_SETSIZE, &fds, ਨਿਰਾ, ਨਿਰਾ, &tv) < 0)
		ਰੋਕੋ;
# ਨਹੀਂਤਾਂ
	    ਜੇ (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		ਰੋਕੋ;
# endif
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (got_int)
		ਰੋਕੋ;
	    ui_delay((ਲੰਮਾ)UI_MSEC_DELAY, TRUE);
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
server_Get_Vim_ਨਾਂ(Display *dpy)
{
    char_u	*regProp;
    char_u	*ਇੰਦਰਾਜ;
    char_u	*p;
    long_u	ਅੰਕ_ਚੀਜ਼ਾਂ;
    int_u	w;
    list_T	*list;

    ਜੇ (registryProperty == None)
    {
	ਜੇ (SendInit(dpy) < 0)
	    ਵਾਪਸ ਨਿਰਾ;
    }

    list = list_alloc();
    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    /*
     * Read the registry property.
     */
    ਜੇ (GetRegProp(dpy, &regProp, &ਅੰਕ_ਚੀਜ਼ਾਂ, TRUE) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    /*
     * Scan all of the names out of the property.
     */
    ਲਈ (p = regProp; (long_u)(p - regProp) < ਅੰਕ_ਚੀਜ਼ਾਂ; p++)
    {
	ਇੰਦਰਾਜ = p;
	ਜਦੋਂ (*p != 0 && !ਸੁਰੱਖਿਅਤ_isspace(*p))
	    p++;
	ਜੇ (*p != 0)
	{
	    w = None;
	    ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)ਇੰਦਰਾਜ, "%x", &w);
	    ਜੇ (ਖਿੜਕੀ_Valid(dpy, (ਖਿੜਕੀ)w))
		list_append_string(list, p + 1, -1);
	    ਜਦੋਂ (*p != 0)
		p++;
	}
    }
    ਜੇ (regProp != ਖਾਲੀ_ਥਾਵਾਂ_prop)
	XFree(regProp);
    ਵਾਪਸ list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    ਸਥਿਰ ਬਣਤਰ Server_ਜਵਾਬ *
Server_ਜਵਾਬ_ਲੱਭੋ(ਖਿੜਕੀ w, ਗਿਣਤੀ Server_ਜਵਾਬ_Op op)
{
    ਬਣਤਰ Server_ਜਵਾਬ *p;
    ਬਣਤਰ Server_ਜਵਾਬ e;
    ਪੂਰਨ		i;

    p = (ਬਣਤਰ Server_ਜਵਾਬ *) server_ਜਵਾਬ.ga_ਅੰਕੜੇ;
    ਲਈ (i = 0; i < server_ਜਵਾਬ.ga_len; i++, p++)
	ਜੇ (p->id == w)
	    ਰੋਕੋ;
    ਜੇ (i >= server_ਜਵਾਬ.ga_len)
	p = ਨਿਰਾ;

    ਜੇ (p == ਨਿਰਾ && op == SROP_ਜੋੜੋ)
    {
	ਜੇ (server_ਜਵਾਬ.ga_growsize == 0)
	    ga_init2(&server_ਜਵਾਬ, ਆਕਾਰ(ਬਣਤਰ Server_ਜਵਾਬ), 1);
	ਜੇ (ga_grow(&server_ਜਵਾਬ, 1) == OK)
	{
	    p = ((ਬਣਤਰ Server_ਜਵਾਬ *) server_ਜਵਾਬ.ga_ਅੰਕੜੇ)
		+ server_ਜਵਾਬ.ga_len;
	    e.id = w;
	    ga_init2(&e.ਲੜੀਆਂ, 1, 100);
	    mch_memmove(p, &e, ਆਕਾਰ(e));
	    server_ਜਵਾਬ.ga_len++;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (p != ਨਿਰਾ && op == SROP_Delete)
    {
	ga_clear(&p->ਲੜੀਆਂ);
	mch_memmove(p, p + 1, (server_ਜਵਾਬ.ga_len - i - 1) * ਆਕਾਰ(*p));
	server_ਜਵਾਬ.ga_len--;
    }

    ਵਾਪਸ p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    ਖਿੜਕੀ
server_Str_ਮੰਜ਼ਿਲ_Win(char_u *str)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ  id = None;

    ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)str, "0x%x", &id);
    ਜੇ (id == None)
	semsg(_(e_invalid_server_id_ਵਰਤਿਆ_str), str);

    ਵਾਪਸ (ਖਿੜਕੀ)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    ਪੂਰਨ
server_Send_ਜਵਾਬ(char_u *name, char_u *str)
{
    char_u	*property;
    ਪੂਰਨ		ਲੰਮਾਈ;
    ਪੂਰਨ		res;
    Display	*dpy = X_DISPLAY;
    ਖਿੜਕੀ	win = server_Str_ਮੰਜ਼ਿਲ_Win(name);

    ਜੇ (commProperty == None)
    {
	ਜੇ (SendInit(dpy) < 0)
	    ਵਾਪਸ -2;
    }
    ਜੇ (!ਖਿੜਕੀ_Valid(dpy, win))
	ਵਾਪਸ -1;

    ਲੰਮਾਈ = STRLEN(p_enc) + STRLEN(str) + 14;
    ਜੇ ((property = alloc(ਲੰਮਾਈ + 30)) == ਨਿਰਾ)
	ਵਾਪਸ -1;

    sprintf((ਅੱਖਰ *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)comm_ਖਿੜਕੀ);
    // Add length of what "%x" resulted in.
    ਲੰਮਾਈ += STRLEN(property + ਲੰਮਾਈ);
    res = AppendPropCarefully(dpy, win, commProperty, property, ਲੰਮਾਈ + 1);
    vim_free(property);

    ਵਾਪਸ res;
}

    ਸਥਿਰ ਪੂਰਨ
Wait_For_ਜਵਾਬ(ਖਾਲੀ *p)
{
    ਖਿੜਕੀ  *w = (ਖਿੜਕੀ *) p;

    ਵਾਪਸ Server_ਜਵਾਬ_ਲੱਭੋ(*w, SROP_ਲੱਭੋ) != ਨਿਰਾ;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    ਪੂਰਨ
server_Read_ਜਵਾਬ(
    Display	*dpy,
    ਖਿੜਕੀ	win,
    char_u	**str,
    ਪੂਰਨ		localLoop,
    ਪੂਰਨ		timeout)
{
    ਪੂਰਨ		len;
    char_u	*s;
    ਬਣਤਰ	Server_ਜਵਾਬ *p;

    ServerWait(dpy, win, Wait_For_ਜਵਾਬ, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    ਜੇ ((p = Server_ਜਵਾਬ_ਲੱਭੋ(win, SROP_ਲੱਭੋ)) != ਨਿਰਾ && p->ਲੜੀਆਂ.ga_len > 0)
    {
	*str = vim_strsave(p->ਲੜੀਆਂ.ga_ਅੰਕੜੇ);
	len = STRLEN(*str) + 1;
	ਜੇ (len < p->ਲੜੀਆਂ.ga_len)
	{
	    s = (char_u *) p->ਲੜੀਆਂ.ga_ਅੰਕੜੇ;
	    mch_memmove(s, s + len, p->ਲੜੀਆਂ.ga_len - len);
	    p->ਲੜੀਆਂ.ga_len -= len;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->ਲੜੀਆਂ);
	    Server_ਜਵਾਬ_ਲੱਭੋ(win, SROP_Delete);
	}
	ਵਾਪਸ 0;
    }
    ਵਾਪਸ -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    ਪੂਰਨ
server_Peek_ਜਵਾਬ(Display *dpy, ਖਿੜਕੀ win, char_u **str)
{
    ਬਣਤਰ Server_ਜਵਾਬ *p;

    ਜੇ ((p = Server_ਜਵਾਬ_ਲੱਭੋ(win, SROP_ਲੱਭੋ)) != ਨਿਰਾ && p->ਲੜੀਆਂ.ga_len > 0)
    {
	ਜੇ (str != ਨਿਰਾ)
	    *str = p->ਲੜੀਆਂ.ga_ਅੰਕੜੇ;
	ਵਾਪਸ 1;
    }
    ਜੇ (!ਖਿੜਕੀ_Valid(dpy, win))
	ਵਾਪਸ -1;
    ਵਾਪਸ 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    ਸਥਿਰ ਪੂਰਨ
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    ਜੇ (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    ਜੇ (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    ਜੇ (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    ਜੇ (comm_ਖਿੜਕੀ == None)
    {
	comm_ਖਿੜਕੀ = XCreate_Simple_ਖਿੜਕੀ(dpy, XDefault_ਜੜ੍ਹ_ਖਿੜਕੀ(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_ਨਿਵੇਸ਼(dpy, comm_ਖਿੜਕੀ, Property_Change_ਨਕਾਬ);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, comm_ਖਿੜਕੀ);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, comm_ਖਿੜਕੀ, vimProperty, XA_STRING,
		    8, Prop_ਬਹੁਲਕ_Replace, (char_u *)VIM_ਸੰਸਕਰਣ_SHORT,
			(ਪੂਰਨ)STRLEN_LITERAL(VIM_ਸੰਸਕਰਣ_SHORT) + 1);

    XSync(dpy, False);
    (ਖਾਲੀ)XSetErrorHandler(old_handler);

    ਵਾਪਸ got_x_error ? -1 : 0;
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
    ਸਥਿਰ ਖਿੜਕੀ
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    ਪੂਰਨ		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *ਇੰਦਰਾਜ;
    char_u	*p;
    long_u	ਅੰਕ_ਚੀਜ਼ਾਂ;
    int_u	return_ਮੁੱਲ;

    /*
     * Read the registry property.
     */
    ਜੇ (GetRegProp(dpy, &regProp, &ਅੰਕ_ਚੀਜ਼ਾਂ, FALSE) == FAIL)
	ਵਾਪਸ 0;

    /*
     * Scan the property for the desired name.
     */
    return_ਮੁੱਲ = (int_u)None;
    ਇੰਦਰਾਜ = ਨਿਰਾ;	// Not needed, but eliminates compiler warning.
    ਲਈ (p = regProp; (long_u)(p - regProp) < ਅੰਕ_ਚੀਜ਼ਾਂ; )
    {
	ਇੰਦਰਾਜ = p;
	ਜਦੋਂ (*p != 0 && !ਸੁਰੱਖਿਅਤ_isspace(*p))
	    p++;
	ਜੇ (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)ਇੰਦਰਾਜ, "%x", &return_ਮੁੱਲ);
	    ਰੋਕੋ;
	}
	ਜਦੋਂ (*p != 0)
	    p++;
	p++;
    }

    ਜੇ (loose != ਨਿਰਾ && return_ਮੁੱਲ == (int_u)None && !IsSerialName(name))
    {
	ਲਈ (p = regProp; (long_u)(p - regProp) < ਅੰਕ_ਚੀਜ਼ਾਂ; )
	{
	    ਇੰਦਰਾਜ = p;
	    ਜਦੋਂ (*p != 0 && !ਸੁਰੱਖਿਅਤ_isspace(*p))
		p++;
	    ਜੇ (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		ਜੇ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)ਇੰਦਰਾਜ, "%x", &return_ਮੁੱਲ) == 1
			&& return_ਮੁੱਲ != (int_u)None)
		    *loose = vim_strsave(p + 1);
		ਰੋਕੋ;
	    }
	    ਜਦੋਂ (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    ਜੇ (delete && return_ਮੁੱਲ != (int_u)None)
    {
	ਪੂਰਨ ਗਿਣਤੀ_2;

	ਜਦੋਂ (*p != 0)
	    p++;
	p++;
	ਗਿਣਤੀ_2 = ਅੰਕ_ਚੀਜ਼ਾਂ - (p - regProp);
	ਜੇ (ਗਿਣਤੀ_2 > 0)
	    mch_memmove(ਇੰਦਰਾਜ, p, ਗਿਣਤੀ_2);
	XChangeProperty(dpy, ਜੜ੍ਹ_ਖਿੜਕੀ_2(dpy, 0), registryProperty, XA_STRING,
			8, Prop_ਬਹੁਲਕ_Replace, regProp,
			(ਪੂਰਨ)(ਅੰਕ_ਚੀਜ਼ਾਂ - (p - ਇੰਦਰਾਜ)));
	XSync(dpy, False);
    }

    ਜੇ (regProp != ਖਾਲੀ_ਥਾਵਾਂ_prop)
	XFree(regProp);
    ਵਾਪਸ (ਖਿੜਕੀ)return_ਮੁੱਲ;
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
    ਸਥਿਰ ਖਾਲੀ
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    ਖਿੜਕੀ	win)	// Window to remove
{
    char_u	*regProp, *ਇੰਦਰਾਜ = ਨਿਰਾ;
    char_u	*p;
    long_u	ਅੰਕ_ਚੀਜ਼ਾਂ;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    ਜੇ (GetRegProp(dpy, &regProp, &ਅੰਕ_ਚੀਜ਼ਾਂ, FALSE) == FAIL)
	ਵਾਪਸ;

    // Scan the property for the window id.
    ਲਈ (p = regProp; (long_u)(p - regProp) < ਅੰਕ_ਚੀਜ਼ਾਂ; )
    {
	ਜੇ (*p != 0)
	{
	    ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)p, "%x", &wwin);
	    ਜੇ ((ਖਿੜਕੀ)wwin == win)
	    {
		ਪੂਰਨ ਆਖਰੀ_Half;

		// Copy down the remainder to delete entry
		ਇੰਦਰਾਜ = p;
		ਜਦੋਂ (*p != 0)
		    p++;
		p++;
		ਆਖਰੀ_Half = ਅੰਕ_ਚੀਜ਼ਾਂ - (p - regProp);
		ਜੇ (ਆਖਰੀ_Half > 0)
		    mch_memmove(ਇੰਦਰਾਜ, p, ਆਖਰੀ_Half);
		ਅੰਕ_ਚੀਜ਼ਾਂ = (ਇੰਦਰਾਜ - regProp) + ਆਖਰੀ_Half;
		p = ਇੰਦਰਾਜ;
		ਜਾਰੀ;
	    }
	}
	ਜਦੋਂ (*p != 0)
	    p++;
	p++;
    }

    ਜੇ (ਇੰਦਰਾਜ != ਨਿਰਾ)
    {
	XChangeProperty(dpy, ਜੜ੍ਹ_ਖਿੜਕੀ_2(dpy, 0), registryProperty,
			XA_STRING, 8, Prop_ਬਹੁਲਕ_Replace, regProp,
			(ਪੂਰਨ)(p - regProp));
	XSync(dpy, False);
    }

    ਜੇ (regProp != ਖਾਲੀ_ਥਾਵਾਂ_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    ਸਥਿਰ ਪੂਰਨ
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*ਅੰਕ_Itemsp,
    ਪੂਰਨ		domsg)		// When TRUE give error message.
{
    ਪੂਰਨ		ਨਤੀਜਾ, actualFormat;
    long_u	ਬਾਈਟਾਂ_After;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = ਨਿਰਾ;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    ਨਤੀਜਾ = XGet_ਖਿੜਕੀ_Property(dpy, ਜੜ੍ਹ_ਖਿੜਕੀ_2(dpy, 0), registryProperty, 0L,
				(ਲੰਮਾ)MAX_PROP_ਸ਼ਬਦ, False,
				XA_STRING, &actualType,
				&actualFormat, ਅੰਕ_Itemsp, &ਬਾਈਟਾਂ_After,
				regPropp);

    XSync(dpy, FALSE);
    (ਖਾਲੀ)XSetErrorHandler(old_handler);
    ਜੇ (got_x_error)
	ਵਾਪਸ FAIL;

    ਜੇ (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*ਅੰਕ_Itemsp = 0;
	*regPropp = ਖਾਲੀ_ਥਾਵਾਂ_prop;
	ਵਾਪਸ OK;
    }

    // If the property is improperly formed, then delete it.
    ਜੇ (ਨਤੀਜਾ != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	ਜੇ (*regPropp != ਨਿਰਾ)
	    XFree(*regPropp);
	XDeleteProperty(dpy, ਜੜ੍ਹ_ਖਿੜਕੀ_2(dpy, 0), registryProperty);
	ਜੇ (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    ਖਾਲੀ
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    ਪੂਰਨ		ਤੁਰੰਤ)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    ਪੂਰਨ		ਨਤੀਜਾ, actualFormat;
    long_u	ਅੰਕ_ਚੀਜ਼ਾਂ, ਬਾਈਟਾਂ_After;
    Atom	actualType;

    ਜੇ (eventPtr != ਨਿਰਾ)
    {
	ਜੇ (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_ਮੁੱਲ)
	    ਵਾਪਸ;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = ਨਿਰਾ;
    ਨਤੀਜਾ = XGet_ਖਿੜਕੀ_Property(dpy, comm_ਖਿੜਕੀ, commProperty, 0L,
				(ਲੰਮਾ)MAX_PROP_ਸ਼ਬਦ, True,
				XA_STRING, &actualType,
				&actualFormat, &ਅੰਕ_ਚੀਜ਼ਾਂ, &ਬਾਈਟਾਂ_After,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    ਜੇ (ਨਤੀਜਾ != Success || actualType != XA_STRING || actualFormat != 8)
    {
	ਜੇ (propInfo != ਨਿਰਾ)
	    XFree(propInfo);
	ਵਾਪਸ;
    }
    ਜੇ (ਤੁਰੰਤ)
	server_parse_ਸੁਨੇਹਾ(dpy, propInfo, ਅੰਕ_ਚੀਜ਼ਾਂ);
    ਨਹੀਂਤਾਂ
	save_ਵਿੱਚ_ਕਤਾਰ(propInfo, ਅੰਕ_ਚੀਜ਼ਾਂ);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    ਸਥਿਰ ਖਾਲੀ
save_ਵਿੱਚ_ਕਤਾਰ(char_u *propInfo, long_u len)
{
    x_ਕਤਾਰ_T *ਗੰਢ;

    ਗੰਢ = ALLOC_ONE(x_ਕਤਾਰ_T);
    ਜੇ (ਗੰਢ == ਨਿਰਾ)
	ਵਾਪਸ;	    // out of memory
    ਗੰਢ->propInfo = propInfo;
    ਗੰਢ->len = len;

    ਜੇ (ਸਿਰ.ਅਗਲਾ == ਨਿਰਾ)   // initialize circular queue
    {
	ਸਿਰ.ਅਗਲਾ = &ਸਿਰ;
	ਸਿਰ.prev = &ਸਿਰ;
    }

    // insert node at tail of queue
    ਗੰਢ->ਅਗਲਾ = &ਸਿਰ;
    ਗੰਢ->prev = ਸਿਰ.prev;
    ਸਿਰ.prev->ਅਗਲਾ = ਗੰਢ;
    ਸਿਰ.prev = ਗੰਢ;
}

/*
 * Parses queued clientserver messages.
 */
    ਖਾਲੀ
server_parse_messages(ਖਾਲੀ)
{
    x_ਕਤਾਰ_T	*ਗੰਢ;

    ਜੇ (!X_DISPLAY)
	ਵਾਪਸ; // cannot happen?
    ਜਦੋਂ (ਸਿਰ.ਅਗਲਾ != ਨਿਰਾ && ਸਿਰ.ਅਗਲਾ != &ਸਿਰ)
    {
	ਗੰਢ = ਸਿਰ.ਅਗਲਾ;
	ਸਿਰ.ਅਗਲਾ = ਗੰਢ->ਅਗਲਾ;
	ਗੰਢ->ਅਗਲਾ->prev = ਗੰਢ->prev;
	server_parse_ਸੁਨੇਹਾ(X_DISPLAY, ਗੰਢ->propInfo, ਗੰਢ->len);
	vim_free(ਗੰਢ);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    ਪੂਰਨ
server_ਉਡੀਕ(ਖਾਲੀ)
{
    ਵਾਪਸ ਸਿਰ.ਅਗਲਾ != ਨਿਰਾ && ਸਿਰ.ਅਗਲਾ != &ਸਿਰ;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    ਸਥਿਰ ਖਾਲੀ
server_parse_ਸੁਨੇਹਾ(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	ਅੰਕ_ਚੀਜ਼ਾਂ)  // The size of propInfo in bytes.
{
    char_u	*p;
    ਪੂਰਨ		ਕੋਡ;
    char_u	*tofree;

# ਜੇ defined(FEAT_EVAL)
    ch_log(ਨਿਰਾ, "server_parse_message() numItems: %ld", ਅੰਕ_ਚੀਜ਼ਾਂ);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    ਲਈ (p = propInfo; (long_u)(p - propInfo) < ਅੰਕ_ਚੀਜ਼ਾਂ; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	ਜੇ (*p == 0)
	{
	    p++;
	    ਜਾਰੀ;
	}

	ਜੇ ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    ਖਿੜਕੀ	res_ਖਿੜਕੀ;
	    char_u	*name, *script, *serial, *ਸਿਰਾ;
	    Bool	as_ਕੁੰਜੀਆਂ = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = ਨਿਰਾ;
	    res_ਖਿੜਕੀ = None;
	    serial = (char_u *)"";
	    script = ਨਿਰਾ;
	    enc = ਨਿਰਾ;
	    ਜਦੋਂ ((long_u)(p - propInfo) < ਅੰਕ_ਚੀਜ਼ਾਂ && *p == '-')
	    {
# ਜੇ defined(FEAT_EVAL)
		ch_log(ਨਿਰਾ, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		ਚੋਣ (p[1])
		{
		    ਹਾਲਤ 'r':
			ਸਿਰਾ = skipwhite(p + 2);
			res_ਖਿੜਕੀ = 0;
			ਜਦੋਂ (vim_isxdigit(*ਸਿਰਾ))
			{
			    res_ਖਿੜਕੀ = 16 * res_ਖਿੜਕੀ + (long_u)hex2nr(*ਸਿਰਾ);
			    ++ਸਿਰਾ;
			}
			ਜੇ (ਸਿਰਾ == p + 2 || *ਸਿਰਾ != ' ')
			    res_ਖਿੜਕੀ = None;
			ਨਹੀਂਤਾਂ
			{
			    p = serial = ਸਿਰਾ + 1;
			    client_ਖਿੜਕੀ = res_ਖਿੜਕੀ; // Remember in global
			}
			ਰੋਕੋ;
		    ਹਾਲਤ 'n':
			ਜੇ (p[2] == ' ')
			    name = p + 3;
			ਰੋਕੋ;
		    ਹਾਲਤ 's':
			ਜੇ (p[2] == ' ')
			    script = p + 3;
			ਰੋਕੋ;
		    ਹਾਲਤ 'E':
			ਜੇ (p[2] == ' ')
			    enc = p + 3;
			ਰੋਕੋ;
		}
		ਜਦੋਂ (*p != 0)
		    p++;
		p++;
	    }

	    ਜੇ (script == ਨਿਰਾ || name == ਨਿਰਾ)
		ਜਾਰੀ;

	    ਜੇ (serverName != ਨਿਰਾ && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		ਜੇ (as_ਕੁੰਜੀਆਂ)
		    server_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(script);
		ਨਹੀਂਤਾਂ
		{
		    char_u      *res;

		    res = eval_client_ਵਿਅੰਜਕ_ਮੰਜ਼ਿਲ_string(script);
		    ਜੇ (res_ਖਿੜਕੀ != None)
		    {
			garray_T    ਜਵਾਬ;

			// Initialize the result property.
			ga_init2(&ਜਵਾਬ, 1, 100);
			(ਖਾਲੀ)ga_grow(&ਜਵਾਬ, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(ਜਵਾਬ.ga_ਅੰਕੜੇ, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			ਜਵਾਬ.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			ਜੇ (res != ਨਿਰਾ)
			    ga_concat(&ਜਵਾਬ, res);
			ਨਹੀਂਤਾਂ
			{
			    ga_concat(&ਜਵਾਬ,
				   (char_u *)_(e_invalid_ਵਿਅੰਜਕ_received));
			    ga_append(&ਜਵਾਬ, 0);
			    GA_CONCAT_LITERAL(&ਜਵਾਬ, "-c 1");
			}
			ga_append(&ਜਵਾਬ, NUL);
			(ਖਾਲੀ)AppendPropCarefully(dpy, res_ਖਿੜਕੀ, commProperty,
						 ਜਵਾਬ.ga_ਅੰਕੜੇ, ਜਵਾਬ.ga_len);
			ga_clear(&ਜਵਾਬ);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'r' && p[1] == 0)
	{
	    ਪੂਰਨ		    serial, gotSerial;
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
	    ਕੋਡ = 0;
	    enc = ਨਿਰਾ;
	    ਜਦੋਂ ((long_u)(p - propInfo) < ਅੰਕ_ਚੀਜ਼ਾਂ && *p == '-')
	    {
		ਚੋਣ (p[1])
		{
		    ਹਾਲਤ 'r':
			ਜੇ (p[2] == ' ')
			    res = p + 3;
			ਰੋਕੋ;
		    ਹਾਲਤ 'E':
			ਜੇ (p[2] == ' ')
			    enc = p + 3;
			ਰੋਕੋ;
		    ਹਾਲਤ 's':
			ਜੇ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			ਰੋਕੋ;
		    ਹਾਲਤ 'c':
			ਜੇ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)p + 2, " %d", &ਕੋਡ) != 1)
			    ਕੋਡ = 0;
			ਰੋਕੋ;
		}
		ਜਦੋਂ (*p != 0)
		    p++;
		p++;
	    }

	    ਜੇ (!gotSerial)
		ਜਾਰੀ;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    ਲਈ (pcPtr = pendingCommands; pcPtr != ਨਿਰਾ; pcPtr = pcPtr->ਅਗਲਾ_Ptr)
	    {
		ਜੇ (serial != pcPtr->serial || pcPtr->ਨਤੀਜਾ != ਨਿਰਾ)
		    ਜਾਰੀ;

		pcPtr->ਕੋਡ = ਕੋਡ;
		res = serverConvert(enc, res, &tofree);
		ਜੇ (tofree == ਨਿਰਾ)
		    res = vim_strsave(res);
		pcPtr->ਨਤੀਜਾ = res;
		ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == 'n' && p[1] == 0)
	{
	    ਖਿੜਕੀ	win = 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ u;
	    ਪੂਰਨ		got_ਖਿੜਕੀ;
	    char_u	*str;
	    ਬਣਤਰ	Server_ਜਵਾਬ *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    got_ਖਿੜਕੀ = 0;
	    str = (char_u *)"";
	    enc = ਨਿਰਾ;
	    ਜਦੋਂ ((long_u)(p - propInfo) < ਅੰਕ_ਚੀਜ਼ਾਂ && *p == '-')
	    {
		ਚੋਣ (p[1])
		{
		    ਹਾਲਤ 'n':
			ਜੇ (p[2] == ' ')
			    str = p + 3;
			ਰੋਕੋ;
		    ਹਾਲਤ 'E':
			ਜੇ (p[2] == ' ')
			    enc = p + 3;
			ਰੋਕੋ;
		    ਹਾਲਤ 'w':
			ਜੇ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    got_ਖਿੜਕੀ = 1;
			}
			ਰੋਕੋ;
		}
		ਜਦੋਂ (*p != 0)
		    p++;
		p++;
	    }

	    ਜੇ (!got_ਖਿੜਕੀ)
		ਜਾਰੀ;
	    str = serverConvert(enc, str, &tofree);
	    ਜੇ ((r = Server_ਜਵਾਬ_ਲੱਭੋ(win, SROP_ਜੋੜੋ)) != ਨਿਰਾ)
	    {
		ga_concat(&(r->ਲੜੀਆਂ), str);
		ga_append(&(r->ਲੜੀਆਂ), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((ਅੱਖਰ *)winstr, "0x%x", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    ਜਦੋਂ (*p != 0)
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
    ਸਥਿਰ ਪੂਰਨ
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    ਖਿੜਕੀ	ਖਿੜਕੀ_2,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*ਮੁੱਲ,		// Characters  to append to property.
    ਪੂਰਨ		ਲੰਮਾਈ)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, ਖਿੜਕੀ_2, property, XA_STRING, 8,
					       Prop_ਬਹੁਲਕ_Append, ਮੁੱਲ, ਲੰਮਾਈ);
    XSync(dpy, False);
    (ਖਾਲੀ) XSetErrorHandler(old_handler);
    ਵਾਪਸ got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    ਸਥਿਰ ਪੂਰਨ
x_error_check(Display *dpy ਅਣਵਰਤਿਆ, XErrorEvent *error_event ਅਣਵਰਤਿਆ)
{
    got_x_error = TRUE;
    ਵਾਪਸ 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    ਸਥਿਰ ਪੂਰਨ
IsSerialName(char_u *str)
{
    ਪੂਰਨ len = STRLEN(str);

    ਵਾਪਸ (len > 1 && vim_isdigit(str[len - 1]));
}
#ਜੇ_ਅੰਤ	// FEAT_CLIENTSERVER
