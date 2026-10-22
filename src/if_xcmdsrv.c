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

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

#ఒకవేళ (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

రకనిర్వచనం నిర్మాణం PendingCommand
{
    పూర్ణాంకం	    serial;	// Serial number expected in result.
    పూర్ణాంకం	    సంకేతం_2;	// Result Code. 0 is OK
    char_u  *ఫలితం;	// String result for command (malloc'ed).
			// NULL means command still pending.
    నిర్మాణం PendingCommand *తదుపరి_Ptr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

స్థిర PendingCommand *pendingCommands = ఖాళీ;
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

# define MAX_PROP_పదాలు 100000

నిర్మాణం Server_జవాబు
{
    కిటికీ  id;
    garray_T అక్షరశ్రేణులు;
};
స్థిర garray_T server_జవాబు = { 0, 0, 0, 0, 0 };
గణన Server_జవాబు_Op { SROP_కనుగొను, SROP_కూడు, SROP_Delete };

రకనిర్వచనం పూర్ణాంకం (*చివర_Cond)(శూన్య *);

నిర్మాణం x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    నిర్మాణం x_cmdqueue	*తదుపరి;
    నిర్మాణం x_cmdqueue	*prev;
};

రకనిర్వచనం నిర్మాణం x_cmdqueue x_వరుస_T;

// dummy node, header for circular queue
స్థిర x_వరుస_T తల = {ఖాళీ, 0, ఖాళీ, ఖాళీ};

/*
 * Forward declarations for procedures defined later in this file:
 */

స్థిర కిటికీ	LookupName(Display *dpy, char_u *name, పూర్ణాంకం delete, char_u **loose);
స్థిర పూర్ణాంకం	SendInit(Display *dpy);
స్థిర పూర్ణాంకం	Do_రిజిస్టరు_Name(Display *dpy, char_u *name);
స్థిర శూన్య	DeleteAnyLingerer(Display *dpy, కిటికీ w);
స్థిర పూర్ణాంకం	GetRegProp(Display *dpy, char_u **regPropp, long_u *అంకె_Itemsp, పూర్ణాంకం domsg);
స్థిర పూర్ణాంకం	WaitForPend(శూన్య *p);
స్థిర పూర్ణాంకం	కిటికీ_Valid(Display *dpy, కిటికీ w);
స్థిర శూన్య	ServerWait(Display *dpy, కిటికీ w, చివర_Cond చివర_Cond_2, శూన్య *చివర_దత్తాంశం, పూర్ణాంకం localLoop, పూర్ణాంకం seconds);
స్థిర పూర్ణాంకం	AppendPropCarefully(Display *display, కిటికీ కిటికీ_2, Atom property, char_u *విలువ, పూర్ణాంకం నిడివి);
స్థిర పూర్ణాంకం	x_error_check(Display *dpy, XErrorEvent *error_event);
స్థిర పూర్ణాంకం	IsSerialName(char_u *name);
స్థిర శూన్య	save_లోపల_వరుస(char_u *buf, long_u len);
స్థిర శూన్య	server_parse_సందేశం(Display *dpy, char_u *propInfo, long_u అంకె_వస్తువులు);

// Private variables for the "server" functionality
స్థిర Atom	registryProperty = None;
స్థిర Atom	vimProperty = None;
స్థిర పూర్ణాంకం	got_x_error = FALSE;

స్థిర char_u	*ఖాళీలు_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    పూర్ణాంకం
server_రిజిస్టరు_Name(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		res;
    char_u	*p = ఖాళీ;

    res = Do_రిజిస్టరు_Name(dpy, name);
    ఒకవేళ (res >= 0)
	తిరిగి OK;

    i = 1;
    చేయి
    {
	ఒకవేళ (res < -1 || i >= 1000)
	{
	    సందేశం_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    తిరిగి FAIL;
	}
	ఒకవేళ (p == ఖాళీ)
	    p = alloc(STRLEN(name) + 10);
	ఒకవేళ (p == ఖాళీ)
	{
	    res = -10;
	    కొనసాగు;
	}
	sprintf((అక్షరం *)p, "%s%d", name, i++);
	res = Do_రిజిస్టరు_Name(dpy, p);
    }
    అయితే (res < 0)
	;
    vim_free(p);

    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
Do_రిజిస్టరు_Name(Display *dpy, char_u *name)
{
    కిటికీ	w;
    XErrorHandler old_handler;
# define MAX_NAME_నిడివి 100
    char_u	propInfo[MAX_NAME_నిడివి + 20];

    ఒకవేళ (commProperty == None)
    {
	ఒకవేళ (SendInit(dpy) < 0)
	    తిరిగి -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, ఖాళీ);
    ఒకవేళ (w != (కిటికీ)0)
    {
	స్థితి_3		స్థితి_2;
	పూర్ణాంకం		dummyInt;
	గుర్తులేని పూర్ణాంకం	dummyUns;
	కిటికీ		dummyWin;

	/*
	 * The name is currently registered.  See if the commWindow
	 * associated with the name exists.  If not, or if the commWindow
	 * is *our* commWindow, then just unregister the old name (this
	 * could happen if an application dies without cleaning up the
	 * registry).
	 */
	old_handler = XSetErrorHandler(x_error_check);
	స్థితి_2 = XGetGeometry(dpy, w, &dummyWin, &dummyInt, &dummyInt,
				  &dummyUns, &dummyUns, &dummyUns, &dummyUns);
	(శూన్య)XSetErrorHandler(old_handler);
	ఒకవేళ (స్థితి_2 != Success && w != comm_కిటికీ)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    తిరిగి -1;
	}
	(శూన్య)LookupName(dpy, name, /*delete=*/TRUE, ఖాళీ);
    }
    sprintf((అక్షరం *)propInfo, "%x %.*s", (int_u)comm_కిటికీ,
						       MAX_NAME_నిడివి, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, మూలం_కిటికీ_2(dpy, 0), registryProperty, XA_STRING, 8,
		    Prop_బహుళకం_Append, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (శూన్య)XSetErrorHandler(old_handler);

    ఒకవేళ (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	సమితి_vim_var_string(VV_SEND_SERVER, name, (పూర్ణాంకం)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	తిరిగి 0;
    }
    తిరిగి -2;
}

# ఒకవేళ defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    శూన్య
server_Change_Registered_కిటికీ(
    Display	*dpy,		// Display to register with
    కిటికీ	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_నిడివి + 20];

    comm_కిటికీ = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    ఒకవేళ (SendInit(dpy) < 0)
	తిరిగి;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    ఒకవేళ (serverName != ఖాళీ)
    {
	// Reinsert name if it was already registered
	(శూన్య)LookupName(dpy, serverName, /*delete=*/TRUE, ఖాళీ);
	sprintf((అక్షరం *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_నిడివి, serverName);
	XChangeProperty(dpy, మూలం_కిటికీ_2(dpy, 0), registryProperty, XA_STRING, 8,
			Prop_బహుళకం_Append, (char_u *)propInfo,
			STRLEN(propInfo) + 1);
    }
    XUngrabServer(dpy);
}
# endif

/*
 * Send to an instance of Vim via the X display.
 * Returns 0 for OK, negative for an error.
 */
    పూర్ణాంకం
server_Send_గమ్యం_Vim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**ఫలితం,		// Result of eval'ed expression
    కిటికీ	*server,		// Actual ID of receiving app
    Bool	as_సమాసం,			// Interpret as keystrokes or expr ?
    పూర్ణాంకం		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    పూర్ణాంకం		silent)			// don't complain about no server
{
    కిటికీ	    w;
    char_u	    *property;
    పూర్ణాంకం		    నిడివి;
    పూర్ణాంకం		    res;
    స్థిర పూర్ణాంకం	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = ఖాళీ;

    ఒకవేళ (ఫలితం != ఖాళీ)
	*ఫలితం = ఖాళీ;
    ఒకవేళ (name == ఖాళీ || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    ఒకవేళ (commProperty == None && dpy != ఖాళీ && SendInit(dpy) < 0)
	తిరిగి -1;

# ఒకవేళ defined(FEAT_EVAL)
    ch_log(ఖాళీ, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    ఒకవేళ (dpy == ఖాళీ || (serverName != ఖాళీ && STRICMP(name, serverName) == 0))
	తిరిగి send_గమ్యం_Local_Vim(cmd, as_సమాసం, ఫలితం);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    అయితే (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	ఒకవేళ (w != None)
	{
	    ఒకవేళ (!కిటికీ_Valid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, ఖాళీ);
		VIM_CLEAR(loosename);
		కొనసాగు;
	    }
	}
	ఆపు;
    }
    ఒకవేళ (w == None)
    {
	ఒకవేళ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	తిరిగి -1;
    }
    లేకపోతే ఒకవేళ (loosename != ఖాళీ)
	name = loosename;
    ఒకవేళ (server != ఖాళీ)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    నిడివి = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(నిడివి + 30);

    sprintf((అక్షరం *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_సమాసం ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    ఒకవేళ (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((అక్షరం *)property + నిడివి, "%c-r %x %d",
						0, (int_u)comm_కిటికీ, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    నిడివి += STRLEN(property + నిడివి + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, నిడివి + 1);
    vim_free(property);
    ఒకవేళ (res < 0)
    {
	emsg(_(e_failed_గమ్యం_send_command_గమ్యం_destination_కార్యక్రమం));
	తిరిగి -1;
    }

    ఒకవేళ (!as_సమాసం) // There is no answer for this - Keys are sent async
	తిరిగి 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.సంకేతం_2 = 0;
    pending.ఫలితం = ఖాళీ;
    pending.తదుపరి_Ptr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    ఒకవేళ (pendingCommands == &pending)
	pendingCommands = pending.తదుపరి_Ptr;
    లేకపోతే
    {
	PendingCommand *pcPtr;

	కొరకు (pcPtr = pendingCommands; pcPtr != ఖాళీ; pcPtr = pcPtr->తదుపరి_Ptr)
	    ఒకవేళ (pcPtr->తదుపరి_Ptr == &pending)
	    {
		pcPtr->తదుపరి_Ptr = pending.తదుపరి_Ptr;
		ఆపు;
	    }
    }

# ఒకవేళ defined(FEAT_EVAL)
    ch_log(ఖాళీ, "serverSendToVim() result: %s",
	    pending.ఫలితం == ఖాళీ ? "NULL" : (అక్షరం *)pending.ఫలితం);
# endif
    ఒకవేళ (ఫలితం != ఖాళీ)
	*ఫలితం = pending.ఫలితం;
    లేకపోతే
	vim_free(pending.ఫలితం);

    తిరిగి pending.సంకేతం_2 == 0 ? 0 : -1;
}

    స్థిర పూర్ణాంకం
WaitForPend(శూన్య *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    తిరిగి pending->ఫలితం != ఖాళీ;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    స్థిర పూర్ణాంకం
కిటికీ_Valid(Display *dpy, కిటికీ w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    పూర్ణాంకం		    అంకె_Prop;
    పూర్ణాంకం		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &అంకె_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    ఒకవేళ (plist == ఖాళీ || got_x_error)
	తిరిగి FALSE;

    కొరకు (i = 0; i < అంకె_Prop; i++)
	ఒకవేళ (plist[i] == vimProperty)
	{
	    XFree(plist);
	    తిరిగి TRUE;
	}
    XFree(plist);
    తిరిగి FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    స్థిర శూన్య
ServerWait(
    Display	*dpy,
    కిటికీ	w,
    చివర_Cond	చివర_Cond_2,
    శూన్య	*చివర_దత్తాంశం,
    పూర్ణాంకం		localLoop,
    పూర్ణాంకం		seconds)
{
    time_t	    ప్రారంభం;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_సంఖ్య(dpy), &fds);
# లేకపోతే
    నిర్మాణం pollfd   fds;

    fds.fd = Connection_సంఖ్య(dpy);
    fds.events = POLLIN;
# endif

    సమయం(&ప్రారంభం);
    అయితే (TRUE)
    {
	అయితే (XCheck_కిటికీ_Event(dpy, comm_కిటికీ, Property_Change_ముసుగు, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	ఒకవేళ (చివర_Cond_2(చివర_దత్తాంశం) != 0)
	    ఆపు;
	ఒకవేళ (!కిటికీ_Valid(dpy, w))
	    ఆపు;
	సమయం(&now);
	ఒకవేళ (seconds >= 0 && (now - ప్రారంభం) >= seconds)
	    ఆపు;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	ఒకవేళ (localLoop)
	{
# ifdef HAVE_SELECT
	    నిర్మాణం timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    ఒకవేళ (select(FD_SETSIZE, &fds, ఖాళీ, ఖాళీ, &tv) < 0)
		ఆపు;
# లేకపోతే
	    ఒకవేళ (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		ఆపు;
# endif
	}
	లేకపోతే
	{
	    ఒకవేళ (got_int)
		ఆపు;
	    ui_delay((దీర్ఘ)UI_MSEC_DELAY, TRUE);
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
server_Get_Vim_పేర్లు(Display *dpy)
{
    char_u	*regProp;
    char_u	*నమోదు_2;
    char_u	*p;
    long_u	అంకె_వస్తువులు;
    int_u	w;
    list_T	*list;

    ఒకవేళ (registryProperty == None)
    {
	ఒకవేళ (SendInit(dpy) < 0)
	    తిరిగి ఖాళీ;
    }

    list = list_alloc();
    ఒకవేళ (list == ఖాళీ)
	తిరిగి ఖాళీ;

    /*
     * Read the registry property.
     */
    ఒకవేళ (GetRegProp(dpy, &regProp, &అంకె_వస్తువులు, TRUE) == FAIL)
	తిరిగి ఖాళీ;

    /*
     * Scan all of the names out of the property.
     */
    కొరకు (p = regProp; (long_u)(p - regProp) < అంకె_వస్తువులు; p++)
    {
	నమోదు_2 = p;
	అయితే (*p != 0 && !సురక్షిత_isspace(*p))
	    p++;
	ఒకవేళ (*p != 0)
	{
	    w = None;
	    అక్షరశ్రేణి_చదువు((అక్షరం *)నమోదు_2, "%x", &w);
	    ఒకవేళ (కిటికీ_Valid(dpy, (కిటికీ)w))
		list_append_string(list, p + 1, -1);
	    అయితే (*p != 0)
		p++;
	}
    }
    ఒకవేళ (regProp != ఖాళీలు_prop)
	XFree(regProp);
    తిరిగి list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    స్థిర నిర్మాణం Server_జవాబు *
Server_జవాబు_కనుగొను(కిటికీ w, గణన Server_జవాబు_Op op)
{
    నిర్మాణం Server_జవాబు *p;
    నిర్మాణం Server_జవాబు e;
    పూర్ణాంకం		i;

    p = (నిర్మాణం Server_జవాబు *) server_జవాబు.ga_దత్తాంశం;
    కొరకు (i = 0; i < server_జవాబు.ga_len; i++, p++)
	ఒకవేళ (p->id == w)
	    ఆపు;
    ఒకవేళ (i >= server_జవాబు.ga_len)
	p = ఖాళీ;

    ఒకవేళ (p == ఖాళీ && op == SROP_కూడు)
    {
	ఒకవేళ (server_జవాబు.ga_growsize == 0)
	    ga_init2(&server_జవాబు, పరిమాణం(నిర్మాణం Server_జవాబు), 1);
	ఒకవేళ (ga_grow(&server_జవాబు, 1) == OK)
	{
	    p = ((నిర్మాణం Server_జవాబు *) server_జవాబు.ga_దత్తాంశం)
		+ server_జవాబు.ga_len;
	    e.id = w;
	    ga_init2(&e.అక్షరశ్రేణులు, 1, 100);
	    mch_memmove(p, &e, పరిమాణం(e));
	    server_జవాబు.ga_len++;
	}
    }
    లేకపోతే ఒకవేళ (p != ఖాళీ && op == SROP_Delete)
    {
	ga_clear(&p->అక్షరశ్రేణులు);
	mch_memmove(p, p + 1, (server_జవాబు.ga_len - i - 1) * పరిమాణం(*p));
	server_జవాబు.ga_len--;
    }

    తిరిగి p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    కిటికీ
server_Str_గమ్యం_Win(char_u *str)
{
    గుర్తులేని  id = None;

    అక్షరశ్రేణి_చదువు((అక్షరం *)str, "0x%x", &id);
    ఒకవేళ (id == None)
	semsg(_(e_invalid_server_id_వాడిన_str), str);

    తిరిగి (కిటికీ)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    పూర్ణాంకం
server_Send_జవాబు(char_u *name, char_u *str)
{
    char_u	*property;
    పూర్ణాంకం		నిడివి;
    పూర్ణాంకం		res;
    Display	*dpy = X_DISPLAY;
    కిటికీ	win = server_Str_గమ్యం_Win(name);

    ఒకవేళ (commProperty == None)
    {
	ఒకవేళ (SendInit(dpy) < 0)
	    తిరిగి -2;
    }
    ఒకవేళ (!కిటికీ_Valid(dpy, win))
	తిరిగి -1;

    నిడివి = STRLEN(p_enc) + STRLEN(str) + 14;
    ఒకవేళ ((property = alloc(నిడివి + 30)) == ఖాళీ)
	తిరిగి -1;

    sprintf((అక్షరం *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (గుర్తులేని పూర్ణాంకం)comm_కిటికీ);
    // Add length of what "%x" resulted in.
    నిడివి += STRLEN(property + నిడివి);
    res = AppendPropCarefully(dpy, win, commProperty, property, నిడివి + 1);
    vim_free(property);

    తిరిగి res;
}

    స్థిర పూర్ణాంకం
Wait_For_జవాబు(శూన్య *p)
{
    కిటికీ  *w = (కిటికీ *) p;

    తిరిగి Server_జవాబు_కనుగొను(*w, SROP_కనుగొను) != ఖాళీ;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    పూర్ణాంకం
server_Read_జవాబు(
    Display	*dpy,
    కిటికీ	win,
    char_u	**str,
    పూర్ణాంకం		localLoop,
    పూర్ణాంకం		timeout)
{
    పూర్ణాంకం		len;
    char_u	*s;
    నిర్మాణం	Server_జవాబు *p;

    ServerWait(dpy, win, Wait_For_జవాబు, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    ఒకవేళ ((p = Server_జవాబు_కనుగొను(win, SROP_కనుగొను)) != ఖాళీ && p->అక్షరశ్రేణులు.ga_len > 0)
    {
	*str = vim_strsave(p->అక్షరశ్రేణులు.ga_దత్తాంశం);
	len = STRLEN(*str) + 1;
	ఒకవేళ (len < p->అక్షరశ్రేణులు.ga_len)
	{
	    s = (char_u *) p->అక్షరశ్రేణులు.ga_దత్తాంశం;
	    mch_memmove(s, s + len, p->అక్షరశ్రేణులు.ga_len - len);
	    p->అక్షరశ్రేణులు.ga_len -= len;
	}
	లేకపోతే
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->అక్షరశ్రేణులు);
	    Server_జవాబు_కనుగొను(win, SROP_Delete);
	}
	తిరిగి 0;
    }
    తిరిగి -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    పూర్ణాంకం
server_Peek_జవాబు(Display *dpy, కిటికీ win, char_u **str)
{
    నిర్మాణం Server_జవాబు *p;

    ఒకవేళ ((p = Server_జవాబు_కనుగొను(win, SROP_కనుగొను)) != ఖాళీ && p->అక్షరశ్రేణులు.ga_len > 0)
    {
	ఒకవేళ (str != ఖాళీ)
	    *str = p->అక్షరశ్రేణులు.ga_దత్తాంశం;
	తిరిగి 1;
    }
    ఒకవేళ (!కిటికీ_Valid(dpy, win))
	తిరిగి -1;
    తిరిగి 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    స్థిర పూర్ణాంకం
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    ఒకవేళ (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    ఒకవేళ (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    ఒకవేళ (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    ఒకవేళ (comm_కిటికీ == None)
    {
	comm_కిటికీ = XCreate_Simple_కిటికీ(dpy, XDefault_మూలం_కిటికీ(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_ఆదానం(dpy, comm_కిటికీ, Property_Change_ముసుగు);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, comm_కిటికీ);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, comm_కిటికీ, vimProperty, XA_STRING,
		    8, Prop_బహుళకం_Replace, (char_u *)VIM_సంస్కరణ_SHORT,
			(పూర్ణాంకం)STRLEN_LITERAL(VIM_సంస్కరణ_SHORT) + 1);

    XSync(dpy, False);
    (శూన్య)XSetErrorHandler(old_handler);

    తిరిగి got_x_error ? -1 : 0;
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
    స్థిర కిటికీ
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    పూర్ణాంకం		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *నమోదు_2;
    char_u	*p;
    long_u	అంకె_వస్తువులు;
    int_u	return_విలువ;

    /*
     * Read the registry property.
     */
    ఒకవేళ (GetRegProp(dpy, &regProp, &అంకె_వస్తువులు, FALSE) == FAIL)
	తిరిగి 0;

    /*
     * Scan the property for the desired name.
     */
    return_విలువ = (int_u)None;
    నమోదు_2 = ఖాళీ;	// Not needed, but eliminates compiler warning.
    కొరకు (p = regProp; (long_u)(p - regProp) < అంకె_వస్తువులు; )
    {
	నమోదు_2 = p;
	అయితే (*p != 0 && !సురక్షిత_isspace(*p))
	    p++;
	ఒకవేళ (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    అక్షరశ్రేణి_చదువు((అక్షరం *)నమోదు_2, "%x", &return_విలువ);
	    ఆపు;
	}
	అయితే (*p != 0)
	    p++;
	p++;
    }

    ఒకవేళ (loose != ఖాళీ && return_విలువ == (int_u)None && !IsSerialName(name))
    {
	కొరకు (p = regProp; (long_u)(p - regProp) < అంకె_వస్తువులు; )
	{
	    నమోదు_2 = p;
	    అయితే (*p != 0 && !సురక్షిత_isspace(*p))
		p++;
	    ఒకవేళ (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		ఒకవేళ (అక్షరశ్రేణి_చదువు((అక్షరం *)నమోదు_2, "%x", &return_విలువ) == 1
			&& return_విలువ != (int_u)None)
		    *loose = vim_strsave(p + 1);
		ఆపు;
	    }
	    అయితే (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    ఒకవేళ (delete && return_విలువ != (int_u)None)
    {
	పూర్ణాంకం లెక్క;

	అయితే (*p != 0)
	    p++;
	p++;
	లెక్క = అంకె_వస్తువులు - (p - regProp);
	ఒకవేళ (లెక్క > 0)
	    mch_memmove(నమోదు_2, p, లెక్క);
	XChangeProperty(dpy, మూలం_కిటికీ_2(dpy, 0), registryProperty, XA_STRING,
			8, Prop_బహుళకం_Replace, regProp,
			(పూర్ణాంకం)(అంకె_వస్తువులు - (p - నమోదు_2)));
	XSync(dpy, False);
    }

    ఒకవేళ (regProp != ఖాళీలు_prop)
	XFree(regProp);
    తిరిగి (కిటికీ)return_విలువ;
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
    స్థిర శూన్య
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    కిటికీ	win)	// Window to remove
{
    char_u	*regProp, *నమోదు_2 = ఖాళీ;
    char_u	*p;
    long_u	అంకె_వస్తువులు;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    ఒకవేళ (GetRegProp(dpy, &regProp, &అంకె_వస్తువులు, FALSE) == FAIL)
	తిరిగి;

    // Scan the property for the window id.
    కొరకు (p = regProp; (long_u)(p - regProp) < అంకె_వస్తువులు; )
    {
	ఒకవేళ (*p != 0)
	{
	    అక్షరశ్రేణి_చదువు((అక్షరం *)p, "%x", &wwin);
	    ఒకవేళ ((కిటికీ)wwin == win)
	    {
		పూర్ణాంకం చివరి_Half;

		// Copy down the remainder to delete entry
		నమోదు_2 = p;
		అయితే (*p != 0)
		    p++;
		p++;
		చివరి_Half = అంకె_వస్తువులు - (p - regProp);
		ఒకవేళ (చివరి_Half > 0)
		    mch_memmove(నమోదు_2, p, చివరి_Half);
		అంకె_వస్తువులు = (నమోదు_2 - regProp) + చివరి_Half;
		p = నమోదు_2;
		కొనసాగు;
	    }
	}
	అయితే (*p != 0)
	    p++;
	p++;
    }

    ఒకవేళ (నమోదు_2 != ఖాళీ)
    {
	XChangeProperty(dpy, మూలం_కిటికీ_2(dpy, 0), registryProperty,
			XA_STRING, 8, Prop_బహుళకం_Replace, regProp,
			(పూర్ణాంకం)(p - regProp));
	XSync(dpy, False);
    }

    ఒకవేళ (regProp != ఖాళీలు_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    స్థిర పూర్ణాంకం
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*అంకె_Itemsp,
    పూర్ణాంకం		domsg)		// When TRUE give error message.
{
    పూర్ణాంకం		ఫలితం, actualFormat;
    long_u	బైట్లు_After;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = ఖాళీ;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    ఫలితం = XGet_కిటికీ_Property(dpy, మూలం_కిటికీ_2(dpy, 0), registryProperty, 0L,
				(దీర్ఘ)MAX_PROP_పదాలు, False,
				XA_STRING, &actualType,
				&actualFormat, అంకె_Itemsp, &బైట్లు_After,
				regPropp);

    XSync(dpy, FALSE);
    (శూన్య)XSetErrorHandler(old_handler);
    ఒకవేళ (got_x_error)
	తిరిగి FAIL;

    ఒకవేళ (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*అంకె_Itemsp = 0;
	*regPropp = ఖాళీలు_prop;
	తిరిగి OK;
    }

    // If the property is improperly formed, then delete it.
    ఒకవేళ (ఫలితం != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	ఒకవేళ (*regPropp != ఖాళీ)
	    XFree(*regPropp);
	XDeleteProperty(dpy, మూలం_కిటికీ_2(dpy, 0), registryProperty);
	ఒకవేళ (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    శూన్య
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    పూర్ణాంకం		తక్షణ)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    పూర్ణాంకం		ఫలితం, actualFormat;
    long_u	అంకె_వస్తువులు, బైట్లు_After;
    Atom	actualType;

    ఒకవేళ (eventPtr != ఖాళీ)
    {
	ఒకవేళ (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_విలువ)
	    తిరిగి;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = ఖాళీ;
    ఫలితం = XGet_కిటికీ_Property(dpy, comm_కిటికీ, commProperty, 0L,
				(దీర్ఘ)MAX_PROP_పదాలు, True,
				XA_STRING, &actualType,
				&actualFormat, &అంకె_వస్తువులు, &బైట్లు_After,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    ఒకవేళ (ఫలితం != Success || actualType != XA_STRING || actualFormat != 8)
    {
	ఒకవేళ (propInfo != ఖాళీ)
	    XFree(propInfo);
	తిరిగి;
    }
    ఒకవేళ (తక్షణ)
	server_parse_సందేశం(dpy, propInfo, అంకె_వస్తువులు);
    లేకపోతే
	save_లోపల_వరుస(propInfo, అంకె_వస్తువులు);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    స్థిర శూన్య
save_లోపల_వరుస(char_u *propInfo, long_u len)
{
    x_వరుస_T *కణుపు;

    కణుపు = ALLOC_ONE(x_వరుస_T);
    ఒకవేళ (కణుపు == ఖాళీ)
	తిరిగి;	    // out of memory
    కణుపు->propInfo = propInfo;
    కణుపు->len = len;

    ఒకవేళ (తల.తదుపరి == ఖాళీ)   // initialize circular queue
    {
	తల.తదుపరి = &తల;
	తల.prev = &తల;
    }

    // insert node at tail of queue
    కణుపు->తదుపరి = &తల;
    కణుపు->prev = తల.prev;
    తల.prev->తదుపరి = కణుపు;
    తల.prev = కణుపు;
}

/*
 * Parses queued clientserver messages.
 */
    శూన్య
server_parse_messages(శూన్య)
{
    x_వరుస_T	*కణుపు;

    ఒకవేళ (!X_DISPLAY)
	తిరిగి; // cannot happen?
    అయితే (తల.తదుపరి != ఖాళీ && తల.తదుపరి != &తల)
    {
	కణుపు = తల.తదుపరి;
	తల.తదుపరి = కణుపు->తదుపరి;
	కణుపు->తదుపరి->prev = కణుపు->prev;
	server_parse_సందేశం(X_DISPLAY, కణుపు->propInfo, కణుపు->len);
	vim_free(కణుపు);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    పూర్ణాంకం
server_నిరీక్షణ(శూన్య)
{
    తిరిగి తల.తదుపరి != ఖాళీ && తల.తదుపరి != &తల;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    స్థిర శూన్య
server_parse_సందేశం(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	అంకె_వస్తువులు)  // The size of propInfo in bytes.
{
    char_u	*p;
    పూర్ణాంకం		సంకేతం_2;
    char_u	*tofree;

# ఒకవేళ defined(FEAT_EVAL)
    ch_log(ఖాళీ, "server_parse_message() numItems: %ld", అంకె_వస్తువులు);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    కొరకు (p = propInfo; (long_u)(p - propInfo) < అంకె_వస్తువులు; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	ఒకవేళ (*p == 0)
	{
	    p++;
	    కొనసాగు;
	}

	ఒకవేళ ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    కిటికీ	res_కిటికీ;
	    char_u	*name, *script, *serial, *చివర;
	    Bool	as_కీలకాలు = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = ఖాళీ;
	    res_కిటికీ = None;
	    serial = (char_u *)"";
	    script = ఖాళీ;
	    enc = ఖాళీ;
	    అయితే ((long_u)(p - propInfo) < అంకె_వస్తువులు && *p == '-')
	    {
# ఒకవేళ defined(FEAT_EVAL)
		ch_log(ఖాళీ, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		ఎంపిక (p[1])
		{
		    స్థితి 'r':
			చివర = skipwhite(p + 2);
			res_కిటికీ = 0;
			అయితే (vim_isxdigit(*చివర))
			{
			    res_కిటికీ = 16 * res_కిటికీ + (long_u)hex2nr(*చివర);
			    ++చివర;
			}
			ఒకవేళ (చివర == p + 2 || *చివర != ' ')
			    res_కిటికీ = None;
			లేకపోతే
			{
			    p = serial = చివర + 1;
			    client_కిటికీ = res_కిటికీ; // Remember in global
			}
			ఆపు;
		    స్థితి 'n':
			ఒకవేళ (p[2] == ' ')
			    name = p + 3;
			ఆపు;
		    స్థితి 's':
			ఒకవేళ (p[2] == ' ')
			    script = p + 3;
			ఆపు;
		    స్థితి 'E':
			ఒకవేళ (p[2] == ' ')
			    enc = p + 3;
			ఆపు;
		}
		అయితే (*p != 0)
		    p++;
		p++;
	    }

	    ఒకవేళ (script == ఖాళీ || name == ఖాళీ)
		కొనసాగు;

	    ఒకవేళ (serverName != ఖాళీ && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		ఒకవేళ (as_కీలకాలు)
		    server_గమ్యం_ఆదానం_buf(script);
		లేకపోతే
		{
		    char_u      *res;

		    res = eval_client_సమాసం_గమ్యం_string(script);
		    ఒకవేళ (res_కిటికీ != None)
		    {
			garray_T    జవాబు;

			// Initialize the result property.
			ga_init2(&జవాబు, 1, 100);
			(శూన్య)ga_grow(&జవాబు, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(జవాబు.ga_దత్తాంశం, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			జవాబు.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			ఒకవేళ (res != ఖాళీ)
			    ga_concat(&జవాబు, res);
			లేకపోతే
			{
			    ga_concat(&జవాబు,
				   (char_u *)_(e_invalid_వ్యక్తీకరణ_received));
			    ga_append(&జవాబు, 0);
			    GA_CONCAT_LITERAL(&జవాబు, "-c 1");
			}
			ga_append(&జవాబు, NUL);
			(శూన్య)AppendPropCarefully(dpy, res_కిటికీ, commProperty,
						 జవాబు.ga_దత్తాంశం, జవాబు.ga_len);
			ga_clear(&జవాబు);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	లేకపోతే ఒకవేళ (*p == 'r' && p[1] == 0)
	{
	    పూర్ణాంకం		    serial, gotSerial;
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
	    సంకేతం_2 = 0;
	    enc = ఖాళీ;
	    అయితే ((long_u)(p - propInfo) < అంకె_వస్తువులు && *p == '-')
	    {
		ఎంపిక (p[1])
		{
		    స్థితి 'r':
			ఒకవేళ (p[2] == ' ')
			    res = p + 3;
			ఆపు;
		    స్థితి 'E':
			ఒకవేళ (p[2] == ' ')
			    enc = p + 3;
			ఆపు;
		    స్థితి 's':
			ఒకవేళ (అక్షరశ్రేణి_చదువు((అక్షరం *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			ఆపు;
		    స్థితి 'c':
			ఒకవేళ (అక్షరశ్రేణి_చదువు((అక్షరం *)p + 2, " %d", &సంకేతం_2) != 1)
			    సంకేతం_2 = 0;
			ఆపు;
		}
		అయితే (*p != 0)
		    p++;
		p++;
	    }

	    ఒకవేళ (!gotSerial)
		కొనసాగు;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    కొరకు (pcPtr = pendingCommands; pcPtr != ఖాళీ; pcPtr = pcPtr->తదుపరి_Ptr)
	    {
		ఒకవేళ (serial != pcPtr->serial || pcPtr->ఫలితం != ఖాళీ)
		    కొనసాగు;

		pcPtr->సంకేతం_2 = సంకేతం_2;
		res = serverConvert(enc, res, &tofree);
		ఒకవేళ (tofree == ఖాళీ)
		    res = vim_strsave(res);
		pcPtr->ఫలితం = res;
		ఆపు;
	    }
	}
	లేకపోతే ఒకవేళ (*p == 'n' && p[1] == 0)
	{
	    కిటికీ	win = 0;
	    గుర్తులేని పూర్ణాంకం u;
	    పూర్ణాంకం		got_కిటికీ;
	    char_u	*str;
	    నిర్మాణం	Server_జవాబు *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    got_కిటికీ = 0;
	    str = (char_u *)"";
	    enc = ఖాళీ;
	    అయితే ((long_u)(p - propInfo) < అంకె_వస్తువులు && *p == '-')
	    {
		ఎంపిక (p[1])
		{
		    స్థితి 'n':
			ఒకవేళ (p[2] == ' ')
			    str = p + 3;
			ఆపు;
		    స్థితి 'E':
			ఒకవేళ (p[2] == ' ')
			    enc = p + 3;
			ఆపు;
		    స్థితి 'w':
			ఒకవేళ (అక్షరశ్రేణి_చదువు((అక్షరం *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    got_కిటికీ = 1;
			}
			ఆపు;
		}
		అయితే (*p != 0)
		    p++;
		p++;
	    }

	    ఒకవేళ (!got_కిటికీ)
		కొనసాగు;
	    str = serverConvert(enc, str, &tofree);
	    ఒకవేళ ((r = Server_జవాబు_కనుగొను(win, SROP_కూడు)) != ఖాళీ)
	    {
		ga_concat(&(r->అక్షరశ్రేణులు), str);
		ga_append(&(r->అక్షరశ్రేణులు), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((అక్షరం *)winstr, "0x%x", (గుర్తులేని పూర్ణాంకం)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	లేకపోతే
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    అయితే (*p != 0)
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
    స్థిర పూర్ణాంకం
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    కిటికీ	కిటికీ_2,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*విలువ,		// Characters  to append to property.
    పూర్ణాంకం		నిడివి)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, కిటికీ_2, property, XA_STRING, 8,
					       Prop_బహుళకం_Append, విలువ, నిడివి);
    XSync(dpy, False);
    (శూన్య) XSetErrorHandler(old_handler);
    తిరిగి got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    స్థిర పూర్ణాంకం
x_error_check(Display *dpy వాడని, XErrorEvent *error_event వాడని)
{
    got_x_error = TRUE;
    తిరిగి 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    స్థిర పూర్ణాంకం
IsSerialName(char_u *str)
{
    పూర్ణాంకం len = STRLEN(str);

    తిరిగి (len > 1 && vim_isdigit(str[len - 1]));
}
#అయితే_ముగింపు	// FEAT_CLIENTSERVER
