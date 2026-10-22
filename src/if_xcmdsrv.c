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

#ضمن "vim.h"
#ضمن "version.h"

#إذا (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

تعريفنوع بنية PendingCommand
{
    صحيح	    serial;	// Serial number expected in result.
    صحيح	    کوډ;	// Result Code. 0 is OK
    char_u  *پایله;	// String result for command (malloc'ed).
			// NULL means command still pending.
    بنية PendingCommand *بل_Ptr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

ساكن PendingCommand *pendingCommands = عدم;
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

# define MAX_PROP_ټکي 100000

بنية Server_ځواب
{
    کړکۍ  id;
    garray_T مزي_ګانې;
};
ساكن garray_T server_ځواب = { 0, 0, 0, 0, 0 };
تعداد Server_ځواب_Op { SROP_پیدا_کړه, SROP_جمع_کړه, SROP_Delete };

تعريفنوع صحيح (*پای_ټکی_Cond)(فراغ *);

بنية x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    بنية x_cmdqueue	*بل;
    بنية x_cmdqueue	*prev;
};

تعريفنوع بنية x_cmdqueue x_کتار_T;

// dummy node, header for circular queue
ساكن x_کتار_T سر = {عدم, 0, عدم, عدم};

/*
 * Forward declarations for procedures defined later in this file:
 */

ساكن کړکۍ	LookupName(Display *dpy, char_u *name, صحيح delete, char_u **loose);
ساكن صحيح	SendInit(Display *dpy);
ساكن صحيح	Do_ثبتوونکی_Name(Display *dpy, char_u *name);
ساكن فراغ	DeleteAnyLingerer(Display *dpy, کړکۍ w);
ساكن صحيح	GetRegProp(Display *dpy, char_u **regPropp, long_u *عدد_Itemsp, صحيح domsg);
ساكن صحيح	WaitForPend(فراغ *p);
ساكن صحيح	کړکۍ_Valid(Display *dpy, کړکۍ w);
ساكن فراغ	ServerWait(Display *dpy, کړکۍ w, پای_ټکی_Cond پای_ټکی_Cond_2, فراغ *پای_ټکی_معلومات, صحيح localLoop, صحيح seconds);
ساكن صحيح	AppendPropCarefully(Display *display, کړکۍ کړکۍ_2, Atom property, char_u *ارزښت, صحيح اوږدتیا);
ساكن صحيح	x_error_check(Display *dpy, XErrorEvent *error_event);
ساكن صحيح	IsSerialName(char_u *name);
ساكن فراغ	save_دننه_کتار(char_u *buf, long_u len);
ساكن فراغ	server_parse_پیغام(Display *dpy, char_u *propInfo, long_u عدد_توکي);

// Private variables for the "server" functionality
ساكن Atom	registryProperty = None;
ساكن Atom	vimProperty = None;
ساكن صحيح	got_x_error = FALSE;

ساكن char_u	*تشه_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    صحيح
server_ثبتوونکی_Name(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    صحيح		i;
    صحيح		res;
    char_u	*p = عدم;

    res = Do_ثبتوونکی_Name(dpy, name);
    إذا (res >= 0)
	أرجع OK;

    i = 1;
    افعل
    {
	إذا (res < -1 || i >= 1000)
	{
	    پیغام_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    أرجع FAIL;
	}
	إذا (p == عدم)
	    p = alloc(STRLEN(name) + 10);
	إذا (p == عدم)
	{
	    res = -10;
	    استمر;
	}
	sprintf((حرف *)p, "%s%d", name, i++);
	res = Do_ثبتوونکی_Name(dpy, p);
    }
    طالما (res < 0)
	;
    vim_free(p);

    أرجع OK;
}

    ساكن صحيح
Do_ثبتوونکی_Name(Display *dpy, char_u *name)
{
    کړکۍ	w;
    XErrorHandler old_handler;
# define MAX_NAME_اوږدتیا 100
    char_u	propInfo[MAX_NAME_اوږدتیا + 20];

    إذا (commProperty == None)
    {
	إذا (SendInit(dpy) < 0)
	    أرجع -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, عدم);
    إذا (w != (کړکۍ)0)
    {
	حالت_2		حالت;
	صحيح		dummyInt;
	بینښه صحيح	dummyUns;
	کړکۍ		dummyWin;

	/*
	 * The name is currently registered.  See if the commWindow
	 * associated with the name exists.  If not, or if the commWindow
	 * is *our* commWindow, then just unregister the old name (this
	 * could happen if an application dies without cleaning up the
	 * registry).
	 */
	old_handler = XSetErrorHandler(x_error_check);
	حالت = XGetGeometry(dpy, w, &dummyWin, &dummyInt, &dummyInt,
				  &dummyUns, &dummyUns, &dummyUns, &dummyUns);
	(فراغ)XSetErrorHandler(old_handler);
	إذا (حالت != Success && w != comm_کړکۍ)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    أرجع -1;
	}
	(فراغ)LookupName(dpy, name, /*delete=*/TRUE, عدم);
    }
    sprintf((حرف *)propInfo, "%x %.*s", (int_u)comm_کړکۍ,
						       MAX_NAME_اوږدتیا, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, ریښه_کړکۍ_2(dpy, 0), registryProperty, XA_STRING, 8,
		    Prop_ډېرپېښ_Append, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (فراغ)XSetErrorHandler(old_handler);

    إذا (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	سېټ_vim_var_string(VV_SEND_SERVER, name, (صحيح)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	أرجع 0;
    }
    أرجع -2;
}

# إذا defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    فراغ
server_Change_Registered_کړکۍ(
    Display	*dpy,		// Display to register with
    کړکۍ	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_اوږدتیا + 20];

    comm_کړکۍ = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    إذا (SendInit(dpy) < 0)
	أرجع;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    إذا (serverName != عدم)
    {
	// Reinsert name if it was already registered
	(فراغ)LookupName(dpy, serverName, /*delete=*/TRUE, عدم);
	sprintf((حرف *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_اوږدتیا, serverName);
	XChangeProperty(dpy, ریښه_کړکۍ_2(dpy, 0), registryProperty, XA_STRING, 8,
			Prop_ډېرپېښ_Append, (char_u *)propInfo,
			STRLEN(propInfo) + 1);
    }
    XUngrabServer(dpy);
}
# endif

/*
 * Send to an instance of Vim via the X display.
 * Returns 0 for OK, negative for an error.
 */
    صحيح
server_Send_منزل_Vim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**پایله,		// Result of eval'ed expression
    کړکۍ	*server,		// Actual ID of receiving app
    Bool	as_بیان,			// Interpret as keystrokes or expr ?
    صحيح		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    صحيح		silent)			// don't complain about no server
{
    کړکۍ	    w;
    char_u	    *property;
    صحيح		    اوږدتیا;
    صحيح		    res;
    ساكن صحيح	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = عدم;

    إذا (پایله != عدم)
	*پایله = عدم;
    إذا (name == عدم || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    إذا (commProperty == None && dpy != عدم && SendInit(dpy) < 0)
	أرجع -1;

# إذا defined(FEAT_EVAL)
    ch_log(عدم, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    إذا (dpy == عدم || (serverName != عدم && STRICMP(name, serverName) == 0))
	أرجع send_منزل_Local_Vim(cmd, as_بیان, پایله);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    طالما (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	إذا (w != None)
	{
	    إذا (!کړکۍ_Valid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, عدم);
		VIM_CLEAR(loosename);
		استمر;
	    }
	}
	اكسر;
    }
    إذا (w == None)
    {
	إذا (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	أرجع -1;
    }
    وإلا إذا (loosename != عدم)
	name = loosename;
    إذا (server != عدم)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    اوږدتیا = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(اوږدتیا + 30);

    sprintf((حرف *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_بیان ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    إذا (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((حرف *)property + اوږدتیا, "%c-r %x %d",
						0, (int_u)comm_کړکۍ, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    اوږدتیا += STRLEN(property + اوږدتیا + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, اوږدتیا + 1);
    vim_free(property);
    إذا (res < 0)
    {
	emsg(_(e_failed_منزل_send_command_منزل_destination_پروګرام));
	أرجع -1;
    }

    إذا (!as_بیان) // There is no answer for this - Keys are sent async
	أرجع 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.کوډ = 0;
    pending.پایله = عدم;
    pending.بل_Ptr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    إذا (pendingCommands == &pending)
	pendingCommands = pending.بل_Ptr;
    وإلا
    {
	PendingCommand *pcPtr;

	لكل (pcPtr = pendingCommands; pcPtr != عدم; pcPtr = pcPtr->بل_Ptr)
	    إذا (pcPtr->بل_Ptr == &pending)
	    {
		pcPtr->بل_Ptr = pending.بل_Ptr;
		اكسر;
	    }
    }

# إذا defined(FEAT_EVAL)
    ch_log(عدم, "serverSendToVim() result: %s",
	    pending.پایله == عدم ? "NULL" : (حرف *)pending.پایله);
# endif
    إذا (پایله != عدم)
	*پایله = pending.پایله;
    وإلا
	vim_free(pending.پایله);

    أرجع pending.کوډ == 0 ? 0 : -1;
}

    ساكن صحيح
WaitForPend(فراغ *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    أرجع pending->پایله != عدم;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    ساكن صحيح
کړکۍ_Valid(Display *dpy, کړکۍ w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    صحيح		    عدد_Prop;
    صحيح		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &عدد_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    إذا (plist == عدم || got_x_error)
	أرجع FALSE;

    لكل (i = 0; i < عدد_Prop; i++)
	إذا (plist[i] == vimProperty)
	{
	    XFree(plist);
	    أرجع TRUE;
	}
    XFree(plist);
    أرجع FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    ساكن فراغ
ServerWait(
    Display	*dpy,
    کړکۍ	w,
    پای_ټکی_Cond	پای_ټکی_Cond_2,
    فراغ	*پای_ټکی_معلومات,
    صحيح		localLoop,
    صحيح		seconds)
{
    time_t	    پیل;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_شمېره(dpy), &fds);
# وإلا
    بنية pollfd   fds;

    fds.fd = Connection_شمېره(dpy);
    fds.events = POLLIN;
# endif

    وخت(&پیل);
    طالما (TRUE)
    {
	طالما (XCheck_کړکۍ_Event(dpy, comm_کړکۍ, Property_Change_ماسک, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	إذا (پای_ټکی_Cond_2(پای_ټکی_معلومات) != 0)
	    اكسر;
	إذا (!کړکۍ_Valid(dpy, w))
	    اكسر;
	وخت(&now);
	إذا (seconds >= 0 && (now - پیل) >= seconds)
	    اكسر;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	إذا (localLoop)
	{
# ifdef HAVE_SELECT
	    بنية timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    إذا (select(FD_SETSIZE, &fds, عدم, عدم, &tv) < 0)
		اكسر;
# وإلا
	    إذا (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		اكسر;
# endif
	}
	وإلا
	{
	    إذا (got_int)
		اكسر;
	    ui_delay((اوږد)UI_MSEC_DELAY, TRUE);
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
server_Get_Vim_نومونه(Display *dpy)
{
    char_u	*regProp;
    char_u	*ثبت;
    char_u	*p;
    long_u	عدد_توکي;
    int_u	w;
    list_T	*list;

    إذا (registryProperty == None)
    {
	إذا (SendInit(dpy) < 0)
	    أرجع عدم;
    }

    list = list_alloc();
    إذا (list == عدم)
	أرجع عدم;

    /*
     * Read the registry property.
     */
    إذا (GetRegProp(dpy, &regProp, &عدد_توکي, TRUE) == FAIL)
	أرجع عدم;

    /*
     * Scan all of the names out of the property.
     */
    لكل (p = regProp; (long_u)(p - regProp) < عدد_توکي; p++)
    {
	ثبت = p;
	طالما (*p != 0 && !خوندي_isspace(*p))
	    p++;
	إذا (*p != 0)
	{
	    w = None;
	    له_مزي_ولوله((حرف *)ثبت, "%x", &w);
	    إذا (کړکۍ_Valid(dpy, (کړکۍ)w))
		list_append_string(list, p + 1, -1);
	    طالما (*p != 0)
		p++;
	}
    }
    إذا (regProp != تشه_prop)
	XFree(regProp);
    أرجع list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    ساكن بنية Server_ځواب *
Server_ځواب_پیدا_کړه(کړکۍ w, تعداد Server_ځواب_Op op)
{
    بنية Server_ځواب *p;
    بنية Server_ځواب e;
    صحيح		i;

    p = (بنية Server_ځواب *) server_ځواب.ga_معلومات;
    لكل (i = 0; i < server_ځواب.ga_len; i++, p++)
	إذا (p->id == w)
	    اكسر;
    إذا (i >= server_ځواب.ga_len)
	p = عدم;

    إذا (p == عدم && op == SROP_جمع_کړه)
    {
	إذا (server_ځواب.ga_growsize == 0)
	    ga_init2(&server_ځواب, حجم(بنية Server_ځواب), 1);
	إذا (ga_grow(&server_ځواب, 1) == OK)
	{
	    p = ((بنية Server_ځواب *) server_ځواب.ga_معلومات)
		+ server_ځواب.ga_len;
	    e.id = w;
	    ga_init2(&e.مزي_ګانې, 1, 100);
	    mch_memmove(p, &e, حجم(e));
	    server_ځواب.ga_len++;
	}
    }
    وإلا إذا (p != عدم && op == SROP_Delete)
    {
	ga_clear(&p->مزي_ګانې);
	mch_memmove(p, p + 1, (server_ځواب.ga_len - i - 1) * حجم(*p));
	server_ځواب.ga_len--;
    }

    أرجع p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    کړکۍ
server_Str_منزل_Win(char_u *str)
{
    بینښه  id = None;

    له_مزي_ولوله((حرف *)str, "0x%x", &id);
    إذا (id == None)
	semsg(_(e_invalid_server_id_کارول_شوی_str), str);

    أرجع (کړکۍ)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    صحيح
server_Send_ځواب(char_u *name, char_u *str)
{
    char_u	*property;
    صحيح		اوږدتیا;
    صحيح		res;
    Display	*dpy = X_DISPLAY;
    کړکۍ	win = server_Str_منزل_Win(name);

    إذا (commProperty == None)
    {
	إذا (SendInit(dpy) < 0)
	    أرجع -2;
    }
    إذا (!کړکۍ_Valid(dpy, win))
	أرجع -1;

    اوږدتیا = STRLEN(p_enc) + STRLEN(str) + 14;
    إذا ((property = alloc(اوږدتیا + 30)) == عدم)
	أرجع -1;

    sprintf((حرف *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (بینښه صحيح)comm_کړکۍ);
    // Add length of what "%x" resulted in.
    اوږدتیا += STRLEN(property + اوږدتیا);
    res = AppendPropCarefully(dpy, win, commProperty, property, اوږدتیا + 1);
    vim_free(property);

    أرجع res;
}

    ساكن صحيح
Wait_For_ځواب(فراغ *p)
{
    کړکۍ  *w = (کړکۍ *) p;

    أرجع Server_ځواب_پیدا_کړه(*w, SROP_پیدا_کړه) != عدم;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    صحيح
server_Read_ځواب(
    Display	*dpy,
    کړکۍ	win,
    char_u	**str,
    صحيح		localLoop,
    صحيح		timeout)
{
    صحيح		len;
    char_u	*s;
    بنية	Server_ځواب *p;

    ServerWait(dpy, win, Wait_For_ځواب, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    إذا ((p = Server_ځواب_پیدا_کړه(win, SROP_پیدا_کړه)) != عدم && p->مزي_ګانې.ga_len > 0)
    {
	*str = vim_strsave(p->مزي_ګانې.ga_معلومات);
	len = STRLEN(*str) + 1;
	إذا (len < p->مزي_ګانې.ga_len)
	{
	    s = (char_u *) p->مزي_ګانې.ga_معلومات;
	    mch_memmove(s, s + len, p->مزي_ګانې.ga_len - len);
	    p->مزي_ګانې.ga_len -= len;
	}
	وإلا
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->مزي_ګانې);
	    Server_ځواب_پیدا_کړه(win, SROP_Delete);
	}
	أرجع 0;
    }
    أرجع -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    صحيح
server_Peek_ځواب(Display *dpy, کړکۍ win, char_u **str)
{
    بنية Server_ځواب *p;

    إذا ((p = Server_ځواب_پیدا_کړه(win, SROP_پیدا_کړه)) != عدم && p->مزي_ګانې.ga_len > 0)
    {
	إذا (str != عدم)
	    *str = p->مزي_ګانې.ga_معلومات;
	أرجع 1;
    }
    إذا (!کړکۍ_Valid(dpy, win))
	أرجع -1;
    أرجع 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    ساكن صحيح
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    إذا (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    إذا (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    إذا (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    إذا (comm_کړکۍ == None)
    {
	comm_کړکۍ = XCreate_Simple_کړکۍ(dpy, XDefault_ریښه_کړکۍ(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_ننوت(dpy, comm_کړکۍ, Property_Change_ماسک);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, comm_کړکۍ);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, comm_کړکۍ, vimProperty, XA_STRING,
		    8, Prop_ډېرپېښ_Replace, (char_u *)VIM_نسخه_SHORT,
			(صحيح)STRLEN_LITERAL(VIM_نسخه_SHORT) + 1);

    XSync(dpy, False);
    (فراغ)XSetErrorHandler(old_handler);

    أرجع got_x_error ? -1 : 0;
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
    ساكن کړکۍ
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    صحيح		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *ثبت;
    char_u	*p;
    long_u	عدد_توکي;
    int_u	return_ارزښت;

    /*
     * Read the registry property.
     */
    إذا (GetRegProp(dpy, &regProp, &عدد_توکي, FALSE) == FAIL)
	أرجع 0;

    /*
     * Scan the property for the desired name.
     */
    return_ارزښت = (int_u)None;
    ثبت = عدم;	// Not needed, but eliminates compiler warning.
    لكل (p = regProp; (long_u)(p - regProp) < عدد_توکي; )
    {
	ثبت = p;
	طالما (*p != 0 && !خوندي_isspace(*p))
	    p++;
	إذا (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    له_مزي_ولوله((حرف *)ثبت, "%x", &return_ارزښت);
	    اكسر;
	}
	طالما (*p != 0)
	    p++;
	p++;
    }

    إذا (loose != عدم && return_ارزښت == (int_u)None && !IsSerialName(name))
    {
	لكل (p = regProp; (long_u)(p - regProp) < عدد_توکي; )
	{
	    ثبت = p;
	    طالما (*p != 0 && !خوندي_isspace(*p))
		p++;
	    إذا (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		إذا (له_مزي_ولوله((حرف *)ثبت, "%x", &return_ارزښت) == 1
			&& return_ارزښت != (int_u)None)
		    *loose = vim_strsave(p + 1);
		اكسر;
	    }
	    طالما (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    إذا (delete && return_ارزښت != (int_u)None)
    {
	صحيح شمېر;

	طالما (*p != 0)
	    p++;
	p++;
	شمېر = عدد_توکي - (p - regProp);
	إذا (شمېر > 0)
	    mch_memmove(ثبت, p, شمېر);
	XChangeProperty(dpy, ریښه_کړکۍ_2(dpy, 0), registryProperty, XA_STRING,
			8, Prop_ډېرپېښ_Replace, regProp,
			(صحيح)(عدد_توکي - (p - ثبت)));
	XSync(dpy, False);
    }

    إذا (regProp != تشه_prop)
	XFree(regProp);
    أرجع (کړکۍ)return_ارزښت;
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
    ساكن فراغ
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    کړکۍ	win)	// Window to remove
{
    char_u	*regProp, *ثبت = عدم;
    char_u	*p;
    long_u	عدد_توکي;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    إذا (GetRegProp(dpy, &regProp, &عدد_توکي, FALSE) == FAIL)
	أرجع;

    // Scan the property for the window id.
    لكل (p = regProp; (long_u)(p - regProp) < عدد_توکي; )
    {
	إذا (*p != 0)
	{
	    له_مزي_ولوله((حرف *)p, "%x", &wwin);
	    إذا ((کړکۍ)wwin == win)
	    {
		صحيح وروستی_Half;

		// Copy down the remainder to delete entry
		ثبت = p;
		طالما (*p != 0)
		    p++;
		p++;
		وروستی_Half = عدد_توکي - (p - regProp);
		إذا (وروستی_Half > 0)
		    mch_memmove(ثبت, p, وروستی_Half);
		عدد_توکي = (ثبت - regProp) + وروستی_Half;
		p = ثبت;
		استمر;
	    }
	}
	طالما (*p != 0)
	    p++;
	p++;
    }

    إذا (ثبت != عدم)
    {
	XChangeProperty(dpy, ریښه_کړکۍ_2(dpy, 0), registryProperty,
			XA_STRING, 8, Prop_ډېرپېښ_Replace, regProp,
			(صحيح)(p - regProp));
	XSync(dpy, False);
    }

    إذا (regProp != تشه_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    ساكن صحيح
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*عدد_Itemsp,
    صحيح		domsg)		// When TRUE give error message.
{
    صحيح		پایله, actualFormat;
    long_u	بایټونه_After;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = عدم;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    پایله = XGet_کړکۍ_Property(dpy, ریښه_کړکۍ_2(dpy, 0), registryProperty, 0L,
				(اوږد)MAX_PROP_ټکي, False,
				XA_STRING, &actualType,
				&actualFormat, عدد_Itemsp, &بایټونه_After,
				regPropp);

    XSync(dpy, FALSE);
    (فراغ)XSetErrorHandler(old_handler);
    إذا (got_x_error)
	أرجع FAIL;

    إذا (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*عدد_Itemsp = 0;
	*regPropp = تشه_prop;
	أرجع OK;
    }

    // If the property is improperly formed, then delete it.
    إذا (پایله != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	إذا (*regPropp != عدم)
	    XFree(*regPropp);
	XDeleteProperty(dpy, ریښه_کړکۍ_2(dpy, 0), registryProperty);
	إذا (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	أرجع FAIL;
    }
    أرجع OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    فراغ
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    صحيح		سمدستي)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    صحيح		پایله, actualFormat;
    long_u	عدد_توکي, بایټونه_After;
    Atom	actualType;

    إذا (eventPtr != عدم)
    {
	إذا (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_ارزښت)
	    أرجع;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = عدم;
    پایله = XGet_کړکۍ_Property(dpy, comm_کړکۍ, commProperty, 0L,
				(اوږد)MAX_PROP_ټکي, True,
				XA_STRING, &actualType,
				&actualFormat, &عدد_توکي, &بایټونه_After,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    إذا (پایله != Success || actualType != XA_STRING || actualFormat != 8)
    {
	إذا (propInfo != عدم)
	    XFree(propInfo);
	أرجع;
    }
    إذا (سمدستي)
	server_parse_پیغام(dpy, propInfo, عدد_توکي);
    وإلا
	save_دننه_کتار(propInfo, عدد_توکي);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    ساكن فراغ
save_دننه_کتار(char_u *propInfo, long_u len)
{
    x_کتار_T *غوټه;

    غوټه = ALLOC_ONE(x_کتار_T);
    إذا (غوټه == عدم)
	أرجع;	    // out of memory
    غوټه->propInfo = propInfo;
    غوټه->len = len;

    إذا (سر.بل == عدم)   // initialize circular queue
    {
	سر.بل = &سر;
	سر.prev = &سر;
    }

    // insert node at tail of queue
    غوټه->بل = &سر;
    غوټه->prev = سر.prev;
    سر.prev->بل = غوټه;
    سر.prev = غوټه;
}

/*
 * Parses queued clientserver messages.
 */
    فراغ
server_parse_messages(فراغ)
{
    x_کتار_T	*غوټه;

    إذا (!X_DISPLAY)
	أرجع; // cannot happen?
    طالما (سر.بل != عدم && سر.بل != &سر)
    {
	غوټه = سر.بل;
	سر.بل = غوټه->بل;
	غوټه->بل->prev = غوټه->prev;
	server_parse_پیغام(X_DISPLAY, غوټه->propInfo, غوټه->len);
	vim_free(غوټه);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    صحيح
server_انتظار(فراغ)
{
    أرجع سر.بل != عدم && سر.بل != &سر;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    ساكن فراغ
server_parse_پیغام(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	عدد_توکي)  // The size of propInfo in bytes.
{
    char_u	*p;
    صحيح		کوډ;
    char_u	*tofree;

# إذا defined(FEAT_EVAL)
    ch_log(عدم, "server_parse_message() numItems: %ld", عدد_توکي);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    لكل (p = propInfo; (long_u)(p - propInfo) < عدد_توکي; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	إذا (*p == 0)
	{
	    p++;
	    استمر;
	}

	إذا ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    کړکۍ	res_کړکۍ;
	    char_u	*name, *script, *serial, *پای_ټکی;
	    Bool	as_کلیګانې = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = عدم;
	    res_کړکۍ = None;
	    serial = (char_u *)"";
	    script = عدم;
	    enc = عدم;
	    طالما ((long_u)(p - propInfo) < عدد_توکي && *p == '-')
	    {
# إذا defined(FEAT_EVAL)
		ch_log(عدم, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		بدل (p[1])
		{
		    حالة 'r':
			پای_ټکی = skipwhite(p + 2);
			res_کړکۍ = 0;
			طالما (vim_isxdigit(*پای_ټکی))
			{
			    res_کړکۍ = 16 * res_کړکۍ + (long_u)hex2nr(*پای_ټکی);
			    ++پای_ټکی;
			}
			إذا (پای_ټکی == p + 2 || *پای_ټکی != ' ')
			    res_کړکۍ = None;
			وإلا
			{
			    p = serial = پای_ټکی + 1;
			    client_کړکۍ = res_کړکۍ; // Remember in global
			}
			اكسر;
		    حالة 'n':
			إذا (p[2] == ' ')
			    name = p + 3;
			اكسر;
		    حالة 's':
			إذا (p[2] == ' ')
			    script = p + 3;
			اكسر;
		    حالة 'E':
			إذا (p[2] == ' ')
			    enc = p + 3;
			اكسر;
		}
		طالما (*p != 0)
		    p++;
		p++;
	    }

	    إذا (script == عدم || name == عدم)
		استمر;

	    إذا (serverName != عدم && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		إذا (as_کلیګانې)
		    server_منزل_ننوت_buf(script);
		وإلا
		{
		    char_u      *res;

		    res = eval_client_بیان_منزل_string(script);
		    إذا (res_کړکۍ != None)
		    {
			garray_T    ځواب;

			// Initialize the result property.
			ga_init2(&ځواب, 1, 100);
			(فراغ)ga_grow(&ځواب, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(ځواب.ga_معلومات, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			ځواب.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			إذا (res != عدم)
			    ga_concat(&ځواب, res);
			وإلا
			{
			    ga_concat(&ځواب,
				   (char_u *)_(e_invalid_افاده_received));
			    ga_append(&ځواب, 0);
			    GA_CONCAT_LITERAL(&ځواب, "-c 1");
			}
			ga_append(&ځواب, NUL);
			(فراغ)AppendPropCarefully(dpy, res_کړکۍ, commProperty,
						 ځواب.ga_معلومات, ځواب.ga_len);
			ga_clear(&ځواب);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	وإلا إذا (*p == 'r' && p[1] == 0)
	{
	    صحيح		    serial, gotSerial;
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
	    کوډ = 0;
	    enc = عدم;
	    طالما ((long_u)(p - propInfo) < عدد_توکي && *p == '-')
	    {
		بدل (p[1])
		{
		    حالة 'r':
			إذا (p[2] == ' ')
			    res = p + 3;
			اكسر;
		    حالة 'E':
			إذا (p[2] == ' ')
			    enc = p + 3;
			اكسر;
		    حالة 's':
			إذا (له_مزي_ولوله((حرف *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			اكسر;
		    حالة 'c':
			إذا (له_مزي_ولوله((حرف *)p + 2, " %d", &کوډ) != 1)
			    کوډ = 0;
			اكسر;
		}
		طالما (*p != 0)
		    p++;
		p++;
	    }

	    إذا (!gotSerial)
		استمر;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    لكل (pcPtr = pendingCommands; pcPtr != عدم; pcPtr = pcPtr->بل_Ptr)
	    {
		إذا (serial != pcPtr->serial || pcPtr->پایله != عدم)
		    استمر;

		pcPtr->کوډ = کوډ;
		res = serverConvert(enc, res, &tofree);
		إذا (tofree == عدم)
		    res = vim_strsave(res);
		pcPtr->پایله = res;
		اكسر;
	    }
	}
	وإلا إذا (*p == 'n' && p[1] == 0)
	{
	    کړکۍ	win = 0;
	    بینښه صحيح u;
	    صحيح		got_کړکۍ;
	    char_u	*str;
	    بنية	Server_ځواب *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    got_کړکۍ = 0;
	    str = (char_u *)"";
	    enc = عدم;
	    طالما ((long_u)(p - propInfo) < عدد_توکي && *p == '-')
	    {
		بدل (p[1])
		{
		    حالة 'n':
			إذا (p[2] == ' ')
			    str = p + 3;
			اكسر;
		    حالة 'E':
			إذا (p[2] == ' ')
			    enc = p + 3;
			اكسر;
		    حالة 'w':
			إذا (له_مزي_ولوله((حرف *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    got_کړکۍ = 1;
			}
			اكسر;
		}
		طالما (*p != 0)
		    p++;
		p++;
	    }

	    إذا (!got_کړکۍ)
		استمر;
	    str = serverConvert(enc, str, &tofree);
	    إذا ((r = Server_ځواب_پیدا_کړه(win, SROP_جمع_کړه)) != عدم)
	    {
		ga_concat(&(r->مزي_ګانې), str);
		ga_append(&(r->مزي_ګانې), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((حرف *)winstr, "0x%x", (بینښه صحيح)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	وإلا
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    طالما (*p != 0)
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
    ساكن صحيح
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    کړکۍ	کړکۍ_2,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*ارزښت,		// Characters  to append to property.
    صحيح		اوږدتیا)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, کړکۍ_2, property, XA_STRING, 8,
					       Prop_ډېرپېښ_Append, ارزښت, اوږدتیا);
    XSync(dpy, False);
    (فراغ) XSetErrorHandler(old_handler);
    أرجع got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    ساكن صحيح
x_error_check(Display *dpy ناکارول_شوی, XErrorEvent *error_event ناکارول_شوی)
{
    got_x_error = TRUE;
    أرجع 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    ساكن صحيح
IsSerialName(char_u *str)
{
    صحيح len = STRLEN(str);

    أرجع (len > 1 && vim_isdigit(str[len - 1]));
}
#د_که_پای	// FEAT_CLIENTSERVER
