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

#شامل "vim.h"
#شامل "version.h"

#اگر (defined(FEAT_CLIENTSERVER) && defined(FEAT_X11))

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

قسمتعریف ساخت PendingCommand
{
    صحیح	    serial;	// Serial number expected in result.
    صحیح	    رمز;	// Result Code. 0 is OK
    char_u  *نتیجہ;	// String result for command (malloc'ed).
			// NULL means command still pending.
    ساخت PendingCommand *اگلا_Ptr;
			// Next in list of all outstanding commands.
			// NULL means end of list.
} PendingCommand;

ساکن PendingCommand *pendingCommands = تہی;
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

# define MAX_PROP_الفاظ 100000

ساخت Server_جواب
{
    کھڑکی  id;
    garray_T لڑیاں;
};
ساکن garray_T server_جواب = { 0, 0, 0, 0, 0 };
شمار Server_جواب_Op { SROP_ڈھونڈو, SROP_جوڑو, SROP_Delete };

قسمتعریف صحیح (*انتہا_Cond)(خالی *);

ساخت x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    ساخت x_cmdqueue	*اگلا;
    ساخت x_cmdqueue	*پچھلا;
};

قسمتعریف ساخت x_cmdqueue x_قطار_T;

// dummy node, header for circular queue
ساکن x_قطار_T سر = {تہی, 0, تہی, تہی};

/*
 * Forward declarations for procedures defined later in this file:
 */

ساکن کھڑکی	LookupName(Display *dpy, char_u *name, صحیح delete, char_u **loose);
ساکن صحیح	SendInit(Display *dpy);
ساکن صحیح	Do_رجسٹر_Name(Display *dpy, char_u *name);
ساکن خالی	DeleteAnyLingerer(Display *dpy, کھڑکی w);
ساکن صحیح	GetRegProp(Display *dpy, char_u **regPropp, long_u *عدد_Itemsp, صحیح domsg);
ساکن صحیح	WaitForPend(خالی *p);
ساکن صحیح	کھڑکی_Valid(Display *dpy, کھڑکی w);
ساکن خالی	ServerWait(Display *dpy, کھڑکی w, انتہا_Cond انتہا_Cond_2, خالی *انتہا_ڈیٹا, صحیح localLoop, صحیح seconds);
ساکن صحیح	AppendPropCarefully(Display *display, کھڑکی کھڑکی_2, Atom property, char_u *قدر, صحیح لمبائی);
ساکن صحیح	x_error_check(Display *dpy, XErrorEvent *error_event);
ساکن صحیح	IsSerialName(char_u *name);
ساکن خالی	save_اندر_قطار(char_u *buf, long_u len);
ساکن خالی	server_parse_پیغام(Display *dpy, char_u *propInfo, long_u عدد_اشیاء);

// Private variables for the "server" functionality
ساکن Atom	registryProperty = None;
ساکن Atom	vimProperty = None;
ساکن صحیح	got_x_error = FALSE;

ساکن char_u	*خالی_خانے_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    صحیح
server_رجسٹر_Name(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    صحیح		i;
    صحیح		res;
    char_u	*p = تہی;

    res = Do_رجسٹر_Name(dpy, name);
    اگر (res >= 0)
	واپس OK;

    i = 1;
    کرو
    {
	اگر (res < -1 || i >= 1000)
	{
	    پیغام_attr(_("Unable to register a command server name"),
		    HL_ATTR(HLF_W));
	    واپس FAIL;
	}
	اگر (p == تہی)
	    p = alloc(STRLEN(name) + 10);
	اگر (p == تہی)
	{
	    res = -10;
	    جاری;
	}
	sprintf((حرف *)p, "%s%d", name, i++);
	res = Do_رجسٹر_Name(dpy, p);
    }
    جب (res < 0)
	;
    vim_free(p);

    واپس OK;
}

    ساکن صحیح
Do_رجسٹر_Name(Display *dpy, char_u *name)
{
    کھڑکی	w;
    XErrorHandler old_handler;
# define MAX_NAME_لمبائی 100
    char_u	propInfo[MAX_NAME_لمبائی + 20];

    اگر (commProperty == None)
    {
	اگر (SendInit(dpy) < 0)
	    واپس -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     * WARNING: Do not step through this while debugging, it will hangup the X
     * server!
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, FALSE, تہی);
    اگر (w != (کھڑکی)0)
    {
	حالت_3		حالت_2;
	صحیح		dummyInt;
	غیرعلامتی صحیح	dummyUns;
	کھڑکی		dummyWin;

	/*
	 * The name is currently registered.  See if the commWindow
	 * associated with the name exists.  If not, or if the commWindow
	 * is *our* commWindow, then just unregister the old name (this
	 * could happen if an application dies without cleaning up the
	 * registry).
	 */
	old_handler = XSetErrorHandler(x_error_check);
	حالت_2 = XGetGeometry(dpy, w, &dummyWin, &dummyInt, &dummyInt,
				  &dummyUns, &dummyUns, &dummyUns, &dummyUns);
	(خالی)XSetErrorHandler(old_handler);
	اگر (حالت_2 != Success && w != comm_کھڑکی)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    واپس -1;
	}
	(خالی)LookupName(dpy, name, /*delete=*/TRUE, تہی);
    }
    sprintf((حرف *)propInfo, "%x %.*s", (int_u)comm_کھڑکی,
						       MAX_NAME_لمبائی, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, جڑ_کھڑکی_2(dpy, 0), registryProperty, XA_STRING, 8,
		    Prop_منوال_Append, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (خالی)XSetErrorHandler(old_handler);

    اگر (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	سیٹ_vim_var_string(VV_SEND_SERVER, name, (صحیح)namelen);
# endif
	serverName = vim_strnsave(name, namelen);
	need_maketitle = TRUE;
	واپس 0;
    }
    واپس -2;
}

# اگر defined(FEAT_GUI)
/*
 * Clean out new ID from registry and set it as comm win.
 * Change any registered window ID.
 */
    خالی
server_Change_Registered_کھڑکی(
    Display	*dpy,		// Display to register with
    کھڑکی	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_لمبائی + 20];

    comm_کھڑکی = newwin;

    // Always call SendInit() here, to make sure commWindow is marked as a Vim
    // window.
    اگر (SendInit(dpy) < 0)
	واپس;

    // WARNING: Do not step through this while debugging, it will hangup the X
    // server!
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    اگر (serverName != تہی)
    {
	// Reinsert name if it was already registered
	(خالی)LookupName(dpy, serverName, /*delete=*/TRUE, تہی);
	sprintf((حرف *)propInfo, "%x %.*s",
		(int_u)newwin, MAX_NAME_لمبائی, serverName);
	XChangeProperty(dpy, جڑ_کھڑکی_2(dpy, 0), registryProperty, XA_STRING, 8,
			Prop_منوال_Append, (char_u *)propInfo,
			STRLEN(propInfo) + 1);
    }
    XUngrabServer(dpy);
}
# endif

/*
 * Send to an instance of Vim via the X display.
 * Returns 0 for OK, negative for an error.
 */
    صحیح
server_Send_مقصد_Vim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**نتیجہ,		// Result of eval'ed expression
    کھڑکی	*server,		// Actual ID of receiving app
    Bool	as_اظہاریہ,			// Interpret as keystrokes or expr ?
    صحیح		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    صحیح		silent)			// don't complain about no server
{
    کھڑکی	    w;
    char_u	    *property;
    صحیح		    لمبائی;
    صحیح		    res;
    ساکن صحیح	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = تہی;

    اگر (نتیجہ != تہی)
	*نتیجہ = تہی;
    اگر (name == تہی || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    اگر (commProperty == None && dpy != تہی && SendInit(dpy) < 0)
	واپس -1;

# اگر defined(FEAT_EVAL)
    ch_log(تہی, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    اگر (dpy == تہی || (serverName != تہی && STRICMP(name, serverName) == 0))
	واپس send_مقصد_Local_Vim(cmd, as_اظہاریہ, نتیجہ);

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    جب (TRUE)
    {
	w = LookupName(dpy, name, FALSE, &loosename);
	// Check that the window is hot
	اگر (w != None)
	{
	    اگر (!کھڑکی_Valid(dpy, w))
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, تہی);
		VIM_CLEAR(loosename);
		جاری;
	    }
	}
	توڑو;
    }
    اگر (w == None)
    {
	اگر (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	واپس -1;
    }
    ورنہ اگر (loosename != تہی)
	name = loosename;
    اگر (server != تہی)
	*server = w;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     * Length must be computed exactly!
     */
    لمبائی = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(لمبائی + 30);

    sprintf((حرف *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_اظہاریہ ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    اگر (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((حرف *)property + لمبائی, "%c-r %x %d",
						0, (int_u)comm_کھڑکی, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    لمبائی += STRLEN(property + لمبائی + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, لمبائی + 1);
    vim_free(property);
    اگر (res < 0)
    {
	emsg(_(e_failed_مقصد_send_command_مقصد_destination_پروگرام));
	واپس -1;
    }

    اگر (!as_اظہاریہ) // There is no answer for this - Keys are sent async
	واپس 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.رمز = 0;
    pending.نتیجہ = تہی;
    pending.اگلا_Ptr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    اگر (pendingCommands == &pending)
	pendingCommands = pending.اگلا_Ptr;
    ورنہ
    {
	PendingCommand *pcPtr;

	برائے (pcPtr = pendingCommands; pcPtr != تہی; pcPtr = pcPtr->اگلا_Ptr)
	    اگر (pcPtr->اگلا_Ptr == &pending)
	    {
		pcPtr->اگلا_Ptr = pending.اگلا_Ptr;
		توڑو;
	    }
    }

# اگر defined(FEAT_EVAL)
    ch_log(تہی, "serverSendToVim() result: %s",
	    pending.نتیجہ == تہی ? "NULL" : (حرف *)pending.نتیجہ);
# endif
    اگر (نتیجہ != تہی)
	*نتیجہ = pending.نتیجہ;
    ورنہ
	vim_free(pending.نتیجہ);

    واپس pending.رمز == 0 ? 0 : -1;
}

    ساکن صحیح
WaitForPend(خالی *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    واپس pending->نتیجہ != تہی;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    ساکن صحیح
کھڑکی_Valid(Display *dpy, کھڑکی w)
{
    XErrorHandler   old_handler;
    Atom	    *plist;
    صحیح		    عدد_Prop;
    صحیح		    i;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = 0;
    plist = XListProperties(dpy, w, &عدد_Prop);
    XSync(dpy, False);
    XSetErrorHandler(old_handler);
    اگر (plist == تہی || got_x_error)
	واپس FALSE;

    برائے (i = 0; i < عدد_Prop; i++)
	اگر (plist[i] == vimProperty)
	{
	    XFree(plist);
	    واپس TRUE;
	}
    XFree(plist);
    واپس FALSE;
}

/*
 * Enter a loop processing X events & polling chars until we see a result
 */
    ساکن خالی
ServerWait(
    Display	*dpy,
    کھڑکی	w,
    انتہا_Cond	انتہا_Cond_2,
    خالی	*انتہا_ڈیٹا,
    صحیح		localLoop,
    صحیح		seconds)
{
    time_t	    آغاز;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_نمبر(dpy), &fds);
# ورنہ
    ساخت pollfd   fds;

    fds.fd = Connection_نمبر(dpy);
    fds.events = POLLIN;
# endif

    وقت(&آغاز);
    جب (TRUE)
    {
	جب (XCheck_کھڑکی_Event(dpy, comm_کھڑکی, Property_Change_نقاب, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	اگر (انتہا_Cond_2(انتہا_ڈیٹا) != 0)
	    توڑو;
	اگر (!کھڑکی_Valid(dpy, w))
	    توڑو;
	وقت(&now);
	اگر (seconds >= 0 && (now - آغاز) >= seconds)
	    توڑو;

# ifdef FEAT_TIMERS
	check_due_timer();
# endif

	// Just look out for the answer without calling back into Vim
	اگر (localLoop)
	{
# ifdef HAVE_SELECT
	    ساخت timeval  tv;

	    // Set the time every call, select() may change it to the remaining
	    // time.
	    tv.tv_sec = 0;
	    tv.tv_usec =  SEND_MSEC_POLL * 1000;
	    اگر (select(FD_SETSIZE, &fds, تہی, تہی, &tv) < 0)
		توڑو;
# ورنہ
	    اگر (poll(&fds, 1, SEND_MSEC_POLL) < 0)
		توڑو;
# endif
	}
	ورنہ
	{
	    اگر (got_int)
		توڑو;
	    ui_delay((طویل)UI_MSEC_DELAY, TRUE);
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
server_Get_Vim_نام(Display *dpy)
{
    char_u	*regProp;
    char_u	*اندراج_2;
    char_u	*p;
    long_u	عدد_اشیاء;
    int_u	w;
    list_T	*list;

    اگر (registryProperty == None)
    {
	اگر (SendInit(dpy) < 0)
	    واپس تہی;
    }

    list = list_alloc();
    اگر (list == تہی)
	واپس تہی;

    /*
     * Read the registry property.
     */
    اگر (GetRegProp(dpy, &regProp, &عدد_اشیاء, TRUE) == FAIL)
	واپس تہی;

    /*
     * Scan all of the names out of the property.
     */
    برائے (p = regProp; (long_u)(p - regProp) < عدد_اشیاء; p++)
    {
	اندراج_2 = p;
	جب (*p != 0 && !محفوظ_isspace(*p))
	    p++;
	اگر (*p != 0)
	{
	    w = None;
	    لڑی_سے_پڑھو((حرف *)اندراج_2, "%x", &w);
	    اگر (کھڑکی_Valid(dpy, (کھڑکی)w))
		list_append_string(list, p + 1, -1);
	    جب (*p != 0)
		p++;
	}
    }
    اگر (regProp != خالی_خانے_prop)
	XFree(regProp);
    واپس list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    ساکن ساخت Server_جواب *
Server_جواب_ڈھونڈو(کھڑکی w, شمار Server_جواب_Op op)
{
    ساخت Server_جواب *p;
    ساخت Server_جواب e;
    صحیح		i;

    p = (ساخت Server_جواب *) server_جواب.ga_ڈیٹا;
    برائے (i = 0; i < server_جواب.ga_len; i++, p++)
	اگر (p->id == w)
	    توڑو;
    اگر (i >= server_جواب.ga_len)
	p = تہی;

    اگر (p == تہی && op == SROP_جوڑو)
    {
	اگر (server_جواب.ga_growsize == 0)
	    ga_init2(&server_جواب, حجم(ساخت Server_جواب), 1);
	اگر (ga_grow(&server_جواب, 1) == OK)
	{
	    p = ((ساخت Server_جواب *) server_جواب.ga_ڈیٹا)
		+ server_جواب.ga_len;
	    e.id = w;
	    ga_init2(&e.لڑیاں, 1, 100);
	    mch_memmove(p, &e, حجم(e));
	    server_جواب.ga_len++;
	}
    }
    ورنہ اگر (p != تہی && op == SROP_Delete)
    {
	ga_clear(&p->لڑیاں);
	mch_memmove(p, p + 1, (server_جواب.ga_len - i - 1) * حجم(*p));
	server_جواب.ga_len--;
    }

    واپس p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    کھڑکی
server_Str_مقصد_Win(char_u *str)
{
    غیرعلامتی  id = None;

    لڑی_سے_پڑھو((حرف *)str, "0x%x", &id);
    اگر (id == None)
	semsg(_(e_invalid_server_id_مستعمل_str), str);

    واپس (کھڑکی)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    صحیح
server_Send_جواب(char_u *name, char_u *str)
{
    char_u	*property;
    صحیح		لمبائی;
    صحیح		res;
    Display	*dpy = X_DISPLAY;
    کھڑکی	win = server_Str_مقصد_Win(name);

    اگر (commProperty == None)
    {
	اگر (SendInit(dpy) < 0)
	    واپس -2;
    }
    اگر (!کھڑکی_Valid(dpy, win))
	واپس -1;

    لمبائی = STRLEN(p_enc) + STRLEN(str) + 14;
    اگر ((property = alloc(لمبائی + 30)) == تہی)
	واپس -1;

    sprintf((حرف *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (غیرعلامتی صحیح)comm_کھڑکی);
    // Add length of what "%x" resulted in.
    لمبائی += STRLEN(property + لمبائی);
    res = AppendPropCarefully(dpy, win, commProperty, property, لمبائی + 1);
    vim_free(property);

    واپس res;
}

    ساکن صحیح
Wait_For_جواب(خالی *p)
{
    کھڑکی  *w = (کھڑکی *) p;

    واپس Server_جواب_ڈھونڈو(*w, SROP_ڈھونڈو) != تہی;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    صحیح
server_Read_جواب(
    Display	*dpy,
    کھڑکی	win,
    char_u	**str,
    صحیح		localLoop,
    صحیح		timeout)
{
    صحیح		len;
    char_u	*s;
    ساخت	Server_جواب *p;

    ServerWait(dpy, win, Wait_For_جواب, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    اگر ((p = Server_جواب_ڈھونڈو(win, SROP_ڈھونڈو)) != تہی && p->لڑیاں.ga_len > 0)
    {
	*str = vim_strsave(p->لڑیاں.ga_ڈیٹا);
	len = STRLEN(*str) + 1;
	اگر (len < p->لڑیاں.ga_len)
	{
	    s = (char_u *) p->لڑیاں.ga_ڈیٹا;
	    mch_memmove(s, s + len, p->لڑیاں.ga_len - len);
	    p->لڑیاں.ga_len -= len;
	}
	ورنہ
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->لڑیاں);
	    Server_جواب_ڈھونڈو(win, SROP_Delete);
	}
	واپس 0;
    }
    واپس -1;
}

/*
 * Check for replies from id (win).
 * Return TRUE and a non-malloc'ed string if there is.  Else return FALSE.
 */
    صحیح
server_Peek_جواب(Display *dpy, کھڑکی win, char_u **str)
{
    ساخت Server_جواب *p;

    اگر ((p = Server_جواب_ڈھونڈو(win, SROP_ڈھونڈو)) != تہی && p->لڑیاں.ga_len > 0)
    {
	اگر (str != تہی)
	    *str = p->لڑیاں.ga_ڈیٹا;
	واپس 1;
    }
    اگر (!کھڑکی_Valid(dpy, win))
	واپس -1;
    واپس 0;
}


/*
 * Initialize the communication channels for sending commands and receiving
 * results.
 */
    ساکن صحیح
SendInit(Display *dpy)
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    اگر (commProperty == None)
	commProperty = XInternAtom(dpy, "Comm", False);
    اگر (vimProperty == None)
	vimProperty = XInternAtom(dpy, "Vim", False);
    اگر (registryProperty == None)
	registryProperty = XInternAtom(dpy, "VimRegistry", False);

    اگر (comm_کھڑکی == None)
    {
	comm_کھڑکی = XCreate_Simple_کھڑکی(dpy, XDefault_جڑ_کھڑکی(dpy),
				getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelect_داخلہ(dpy, comm_کھڑکی, Property_Change_نقاب);
	// WARNING: Do not step through this while debugging, it will hangup
	// the X server!
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, comm_کھڑکی);
	XUngrabServer(dpy);
    }

    // Make window recognizable as a vim window
    XChangeProperty(dpy, comm_کھڑکی, vimProperty, XA_STRING,
		    8, Prop_منوال_Replace, (char_u *)VIM_نسخہ_SHORT,
			(صحیح)STRLEN_LITERAL(VIM_نسخہ_SHORT) + 1);

    XSync(dpy, False);
    (خالی)XSetErrorHandler(old_handler);

    واپس got_x_error ? -1 : 0;
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
    ساکن کھڑکی
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    صحیح		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *اندراج_2;
    char_u	*p;
    long_u	عدد_اشیاء;
    int_u	return_قدر;

    /*
     * Read the registry property.
     */
    اگر (GetRegProp(dpy, &regProp, &عدد_اشیاء, FALSE) == FAIL)
	واپس 0;

    /*
     * Scan the property for the desired name.
     */
    return_قدر = (int_u)None;
    اندراج_2 = تہی;	// Not needed, but eliminates compiler warning.
    برائے (p = regProp; (long_u)(p - regProp) < عدد_اشیاء; )
    {
	اندراج_2 = p;
	جب (*p != 0 && !محفوظ_isspace(*p))
	    p++;
	اگر (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    لڑی_سے_پڑھو((حرف *)اندراج_2, "%x", &return_قدر);
	    توڑو;
	}
	جب (*p != 0)
	    p++;
	p++;
    }

    اگر (loose != تہی && return_قدر == (int_u)None && !IsSerialName(name))
    {
	برائے (p = regProp; (long_u)(p - regProp) < عدد_اشیاء; )
	{
	    اندراج_2 = p;
	    جب (*p != 0 && !محفوظ_isspace(*p))
		p++;
	    اگر (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		اگر (لڑی_سے_پڑھو((حرف *)اندراج_2, "%x", &return_قدر) == 1
			&& return_قدر != (int_u)None)
		    *loose = vim_strsave(p + 1);
		توڑو;
	    }
	    جب (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    اگر (delete && return_قدر != (int_u)None)
    {
	صحیح گنتی;

	جب (*p != 0)
	    p++;
	p++;
	گنتی = عدد_اشیاء - (p - regProp);
	اگر (گنتی > 0)
	    mch_memmove(اندراج_2, p, گنتی);
	XChangeProperty(dpy, جڑ_کھڑکی_2(dpy, 0), registryProperty, XA_STRING,
			8, Prop_منوال_Replace, regProp,
			(صحیح)(عدد_اشیاء - (p - اندراج_2)));
	XSync(dpy, False);
    }

    اگر (regProp != خالی_خانے_prop)
	XFree(regProp);
    واپس (کھڑکی)return_قدر;
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
    ساکن خالی
DeleteAnyLingerer(
    Display	*dpy,	// Display whose registry to check.
    کھڑکی	win)	// Window to remove
{
    char_u	*regProp, *اندراج_2 = تہی;
    char_u	*p;
    long_u	عدد_اشیاء;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    اگر (GetRegProp(dpy, &regProp, &عدد_اشیاء, FALSE) == FAIL)
	واپس;

    // Scan the property for the window id.
    برائے (p = regProp; (long_u)(p - regProp) < عدد_اشیاء; )
    {
	اگر (*p != 0)
	{
	    لڑی_سے_پڑھو((حرف *)p, "%x", &wwin);
	    اگر ((کھڑکی)wwin == win)
	    {
		صحیح آخری_Half;

		// Copy down the remainder to delete entry
		اندراج_2 = p;
		جب (*p != 0)
		    p++;
		p++;
		آخری_Half = عدد_اشیاء - (p - regProp);
		اگر (آخری_Half > 0)
		    mch_memmove(اندراج_2, p, آخری_Half);
		عدد_اشیاء = (اندراج_2 - regProp) + آخری_Half;
		p = اندراج_2;
		جاری;
	    }
	}
	جب (*p != 0)
	    p++;
	p++;
    }

    اگر (اندراج_2 != تہی)
    {
	XChangeProperty(dpy, جڑ_کھڑکی_2(dpy, 0), registryProperty,
			XA_STRING, 8, Prop_منوال_Replace, regProp,
			(صحیح)(p - regProp));
	XSync(dpy, False);
    }

    اگر (regProp != خالی_خانے_prop)
	XFree(regProp);
}

/*
 * Read the registry property.  Delete it when it's formatted wrong.
 * Return the property in "regPropp".  "empty_prop" is used when it doesn't
 * exist yet.
 * Return OK when successful.
 */
    ساکن صحیح
GetRegProp(
    Display	*dpy,
    char_u	**regPropp,
    long_u	*عدد_Itemsp,
    صحیح		domsg)		// When TRUE give error message.
{
    صحیح		نتیجہ, actualFormat;
    long_u	بائٹیں_After;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = تہی;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    نتیجہ = XGet_کھڑکی_Property(dpy, جڑ_کھڑکی_2(dpy, 0), registryProperty, 0L,
				(طویل)MAX_PROP_الفاظ, False,
				XA_STRING, &actualType,
				&actualFormat, عدد_Itemsp, &بائٹیں_After,
				regPropp);

    XSync(dpy, FALSE);
    (خالی)XSetErrorHandler(old_handler);
    اگر (got_x_error)
	واپس FAIL;

    اگر (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*عدد_Itemsp = 0;
	*regPropp = خالی_خانے_prop;
	واپس OK;
    }

    // If the property is improperly formed, then delete it.
    اگر (نتیجہ != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	اگر (*regPropp != تہی)
	    XFree(*regPropp);
	XDeleteProperty(dpy, جڑ_کھڑکی_2(dpy, 0), registryProperty);
	اگر (domsg)
	    emsg(_(e_vim_instance_registry_property_is_badly_formed_deleted));
	واپس FAIL;
    }
    واپس OK;
}


/*
 * This procedure is invoked by the various X event loops throughout Vims when
 * a property changes on the communication window.  This procedure reads the
 * property and enqueues command requests and responses. If immediate is true,
 * it runs the event immediately instead of enqueuing it. Immediate can cause
 * unintended behavior and should only be used for code that blocks for a
 * response.
 */
    خالی
serverEventProc(
    Display	*dpy,
    XEvent	*eventPtr,	// Information about event.
    صحیح		فوری)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    صحیح		نتیجہ, actualFormat;
    long_u	عدد_اشیاء, بائٹیں_After;
    Atom	actualType;

    اگر (eventPtr != تہی)
    {
	اگر (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_قدر)
	    واپس;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = تہی;
    نتیجہ = XGet_کھڑکی_Property(dpy, comm_کھڑکی, commProperty, 0L,
				(طویل)MAX_PROP_الفاظ, True,
				XA_STRING, &actualType,
				&actualFormat, &عدد_اشیاء, &بائٹیں_After,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    اگر (نتیجہ != Success || actualType != XA_STRING || actualFormat != 8)
    {
	اگر (propInfo != تہی)
	    XFree(propInfo);
	واپس;
    }
    اگر (فوری)
	server_parse_پیغام(dpy, propInfo, عدد_اشیاء);
    ورنہ
	save_اندر_قطار(propInfo, عدد_اشیاء);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    ساکن خالی
save_اندر_قطار(char_u *propInfo, long_u len)
{
    x_قطار_T *گرہ;

    گرہ = ALLOC_ONE(x_قطار_T);
    اگر (گرہ == تہی)
	واپس;	    // out of memory
    گرہ->propInfo = propInfo;
    گرہ->len = len;

    اگر (سر.اگلا == تہی)   // initialize circular queue
    {
	سر.اگلا = &سر;
	سر.پچھلا = &سر;
    }

    // insert node at tail of queue
    گرہ->اگلا = &سر;
    گرہ->پچھلا = سر.پچھلا;
    سر.پچھلا->اگلا = گرہ;
    سر.پچھلا = گرہ;
}

/*
 * Parses queued clientserver messages.
 */
    خالی
server_parse_messages(خالی)
{
    x_قطار_T	*گرہ;

    اگر (!X_DISPLAY)
	واپس; // cannot happen?
    جب (سر.اگلا != تہی && سر.اگلا != &سر)
    {
	گرہ = سر.اگلا;
	سر.اگلا = گرہ->اگلا;
	گرہ->اگلا->پچھلا = گرہ->پچھلا;
	server_parse_پیغام(X_DISPLAY, گرہ->propInfo, گرہ->len);
	vim_free(گرہ);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    صحیح
server_انتظار(خالی)
{
    واپس سر.اگلا != تہی && سر.اگلا != &سر;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    ساکن خالی
server_parse_پیغام(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	عدد_اشیاء)  // The size of propInfo in bytes.
{
    char_u	*p;
    صحیح		رمز;
    char_u	*tofree;

# اگر defined(FEAT_EVAL)
    ch_log(تہی, "server_parse_message() numItems: %ld", عدد_اشیاء);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    برائے (p = propInfo; (long_u)(p - propInfo) < عدد_اشیاء; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	اگر (*p == 0)
	{
	    p++;
	    جاری;
	}

	اگر ((*p == 'c' || *p == 'k') && p[1] == 0)
	{
	    کھڑکی	res_کھڑکی;
	    char_u	*name, *script, *serial, *انتہا;
	    Bool	as_کلیدیں = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = تہی;
	    res_کھڑکی = None;
	    serial = (char_u *)"";
	    script = تہی;
	    enc = تہی;
	    جب ((long_u)(p - propInfo) < عدد_اشیاء && *p == '-')
	    {
# اگر defined(FEAT_EVAL)
		ch_log(تہی, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		انتخاب (p[1])
		{
		    حالت 'r':
			انتہا = skipwhite(p + 2);
			res_کھڑکی = 0;
			جب (vim_isxdigit(*انتہا))
			{
			    res_کھڑکی = 16 * res_کھڑکی + (long_u)hex2nr(*انتہا);
			    ++انتہا;
			}
			اگر (انتہا == p + 2 || *انتہا != ' ')
			    res_کھڑکی = None;
			ورنہ
			{
			    p = serial = انتہا + 1;
			    client_کھڑکی = res_کھڑکی; // Remember in global
			}
			توڑو;
		    حالت 'n':
			اگر (p[2] == ' ')
			    name = p + 3;
			توڑو;
		    حالت 's':
			اگر (p[2] == ' ')
			    script = p + 3;
			توڑو;
		    حالت 'E':
			اگر (p[2] == ' ')
			    enc = p + 3;
			توڑو;
		}
		جب (*p != 0)
		    p++;
		p++;
	    }

	    اگر (script == تہی || name == تہی)
		جاری;

	    اگر (serverName != تہی && STRICMP(name, serverName) == 0)
	    {
		script = serverConvert(enc, script, &tofree);
		اگر (as_کلیدیں)
		    server_مقصد_داخلہ_buf(script);
		ورنہ
		{
		    char_u      *res;

		    res = eval_client_اظہاریہ_مقصد_string(script);
		    اگر (res_کھڑکی != None)
		    {
			garray_T    جواب;

			// Initialize the result property.
			ga_init2(&جواب, 1, 100);
			(خالی)ga_grow(&جواب, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(جواب.ga_ڈیٹا, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			جواب.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			اگر (res != تہی)
			    ga_concat(&جواب, res);
			ورنہ
			{
			    ga_concat(&جواب,
				   (char_u *)_(e_invalid_عبارت_received));
			    ga_append(&جواب, 0);
			    GA_CONCAT_LITERAL(&جواب, "-c 1");
			}
			ga_append(&جواب, NUL);
			(خالی)AppendPropCarefully(dpy, res_کھڑکی, commProperty,
						 جواب.ga_ڈیٹا, جواب.ga_len);
			ga_clear(&جواب);
		    }
		    vim_free(res);
		}
		vim_free(tofree);
	    }
	}
	ورنہ اگر (*p == 'r' && p[1] == 0)
	{
	    صحیح		    serial, gotSerial;
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
	    رمز = 0;
	    enc = تہی;
	    جب ((long_u)(p - propInfo) < عدد_اشیاء && *p == '-')
	    {
		انتخاب (p[1])
		{
		    حالت 'r':
			اگر (p[2] == ' ')
			    res = p + 3;
			توڑو;
		    حالت 'E':
			اگر (p[2] == ' ')
			    enc = p + 3;
			توڑو;
		    حالت 's':
			اگر (لڑی_سے_پڑھو((حرف *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			توڑو;
		    حالت 'c':
			اگر (لڑی_سے_پڑھو((حرف *)p + 2, " %d", &رمز) != 1)
			    رمز = 0;
			توڑو;
		}
		جب (*p != 0)
		    p++;
		p++;
	    }

	    اگر (!gotSerial)
		جاری;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */
	    برائے (pcPtr = pendingCommands; pcPtr != تہی; pcPtr = pcPtr->اگلا_Ptr)
	    {
		اگر (serial != pcPtr->serial || pcPtr->نتیجہ != تہی)
		    جاری;

		pcPtr->رمز = رمز;
		res = serverConvert(enc, res, &tofree);
		اگر (tofree == تہی)
		    res = vim_strsave(res);
		pcPtr->نتیجہ = res;
		توڑو;
	    }
	}
	ورنہ اگر (*p == 'n' && p[1] == 0)
	{
	    کھڑکی	win = 0;
	    غیرعلامتی صحیح u;
	    صحیح		got_کھڑکی;
	    char_u	*str;
	    ساخت	Server_جواب *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    got_کھڑکی = 0;
	    str = (char_u *)"";
	    enc = تہی;
	    جب ((long_u)(p - propInfo) < عدد_اشیاء && *p == '-')
	    {
		انتخاب (p[1])
		{
		    حالت 'n':
			اگر (p[2] == ' ')
			    str = p + 3;
			توڑو;
		    حالت 'E':
			اگر (p[2] == ' ')
			    enc = p + 3;
			توڑو;
		    حالت 'w':
			اگر (لڑی_سے_پڑھو((حرف *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    got_کھڑکی = 1;
			}
			توڑو;
		}
		جب (*p != 0)
		    p++;
		p++;
	    }

	    اگر (!got_کھڑکی)
		جاری;
	    str = serverConvert(enc, str, &tofree);
	    اگر ((r = Server_جواب_ڈھونڈو(win, SROP_جوڑو)) != تہی)
	    {
		ga_concat(&(r->لڑیاں), str);
		ga_append(&(r->لڑیاں), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((حرف *)winstr, "0x%x", (غیرعلامتی صحیح)win);
		apply_autocmds(EVENT_REMOTEREPLY, winstr, str, TRUE, curbuf);
	    }
	    vim_free(tofree);
	}
	ورنہ
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    جب (*p != 0)
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
    ساکن صحیح
AppendPropCarefully(
    Display	*dpy,		// Display on which to operate.
    کھڑکی	کھڑکی_2,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*قدر,		// Characters  to append to property.
    صحیح		لمبائی)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, کھڑکی_2, property, XA_STRING, 8,
					       Prop_منوال_Append, قدر, لمبائی);
    XSync(dpy, False);
    (خالی) XSetErrorHandler(old_handler);
    واپس got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    ساکن صحیح
x_error_check(Display *dpy غیر_مستعمل, XErrorEvent *error_event غیر_مستعمل)
{
    got_x_error = TRUE;
    واپس 0;
}

/*
 * Check if "str" looks like it had a serial number appended.
 * Actually just checks if the name ends in a digit.
 */
    ساکن صحیح
IsSerialName(char_u *str)
{
    صحیح len = STRLEN(str);

    واپس (len > 1 && vim_isdigit(str[len - 1]));
}
#اگر_ختم	// FEAT_CLIENTSERVER
