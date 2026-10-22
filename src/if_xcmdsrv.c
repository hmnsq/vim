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
    صحيح	    کوڈ;	// Result Code. 0 is OK
    char_u  *نتیجہ;	// String result for command (malloc'ed).
			// NULL means command still pending.
    بنية PendingCommand *اگلا_Ptr;
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

# define MAX_PROP_الفاظ 100000

بنية Server_جواب
{
    کھڑکی  id;
    garray_T لڑیاں;
};
ساكن garray_T server_جواب = { 0, 0, 0, 0, 0 };
تعداد Server_جواب_Op { SROP_لبھو, SROP_جوڑو, SROP_Delete };

تعريفنوع صحيح (*سرا_Cond)(فراغ *);

بنية x_cmdqueue
{
    char_u		*propInfo;
    long_u		len;
    بنية x_cmdqueue	*اگلا;
    بنية x_cmdqueue	*prev;
};

تعريفنوع بنية x_cmdqueue x_قطار_T;

// dummy node, header for circular queue
ساكن x_قطار_T سر = {عدم, 0, عدم, عدم};

/*
 * Forward declarations for procedures defined later in this file:
 */

ساكن کھڑکی	LookupName(Display *dpy, char_u *name, صحيح delete, char_u **loose);
ساكن صحيح	SendInit(Display *dpy);
ساكن صحيح	Do_رجسٹر_Name(Display *dpy, char_u *name);
ساكن فراغ	DeleteAnyLingerer(Display *dpy, کھڑکی w);
ساكن صحيح	GetRegProp(Display *dpy, char_u **regPropp, long_u *عدد_Itemsp, صحيح domsg);
ساكن صحيح	WaitForPend(فراغ *p);
ساكن صحيح	کھڑکی_Valid(Display *dpy, کھڑکی w);
ساكن فراغ	ServerWait(Display *dpy, کھڑکی w, سرا_Cond سرا_Cond_2, فراغ *سرا_انکڑے, صحيح localLoop, صحيح seconds);
ساكن صحيح	AppendPropCarefully(Display *display, کھڑکی کھڑکی_2, Atom property, char_u *قدر, صحيح لمائی);
ساكن صحيح	x_error_check(Display *dpy, XErrorEvent *error_event);
ساكن صحيح	IsSerialName(char_u *name);
ساكن فراغ	save_وچ_قطار(char_u *buf, long_u len);
ساكن فراغ	server_parse_سنیہا(Display *dpy, char_u *propInfo, long_u عدد_اشیاء);

// Private variables for the "server" functionality
ساكن Atom	registryProperty = None;
ساكن Atom	vimProperty = None;
ساكن صحيح	got_x_error = FALSE;

ساكن char_u	*خالی_تھاواں_prop = (char_u *)"";	// empty GetRegProp() result

/*
 * Associate an ASCII name with Vim.  Try real hard to get a unique one.
 * Returns FAIL or OK.
 */
    صحيح
server_رجسٹر_Name(
    Display	*dpy,		// display to register with
    char_u	*name)		// the name that will be used as a base
{
    صحيح		i;
    صحيح		res;
    char_u	*p = عدم;

    res = Do_رجسٹر_Name(dpy, name);
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
	res = Do_رجسٹر_Name(dpy, p);
    }
    طالما (res < 0)
	;
    vim_free(p);

    أرجع OK;
}

    ساكن صحيح
Do_رجسٹر_Name(Display *dpy, char_u *name)
{
    کھڑکی	w;
    XErrorHandler old_handler;
# define MAX_NAME_لمائی 100
    char_u	propInfo[MAX_NAME_لمائی + 20];

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
    إذا (w != (کھڑکی)0)
    {
	حالت_2		حالت;
	صحيح		dummyInt;
	بنانشانی صحيح	dummyUns;
	کھڑکی		dummyWin;

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
	إذا (حالت != Success && w != comm_کھڑکی)
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    أرجع -1;
	}
	(فراغ)LookupName(dpy, name, /*delete=*/TRUE, عدم);
    }
    sprintf((حرف *)propInfo, "%x %.*s", (int_u)comm_کھڑکی,
						       MAX_NAME_لمائی, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, جڑ_کھڑکی_2(dpy, 0), registryProperty, XA_STRING, 8,
		    Prop_منوال_Append, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (فراغ)XSetErrorHandler(old_handler);

    إذا (!got_x_error)
    {
	size_t	namelen;

	namelen = STRLEN(name);
# ifdef FEAT_EVAL
	سیٹ_vim_var_string(VV_SEND_SERVER, name, (صحيح)namelen);
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
server_Change_Registered_کھڑکی(
    Display	*dpy,		// Display to register with
    کھڑکی	newwin)		// Re-register to this ID
{
    char_u	propInfo[MAX_NAME_لمائی + 20];

    comm_کھڑکی = newwin;

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
		(int_u)newwin, MAX_NAME_لمائی, serverName);
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
    صحيح
server_Send_منزل_Vim(
    Display	*dpy,			// Where to send.
    char_u	*name,			// Where to send.
    char_u	*cmd,			// What to send.
    char_u	**نتیجہ,		// Result of eval'ed expression
    کھڑکی	*server,		// Actual ID of receiving app
    Bool	as_اظہاریہ,			// Interpret as keystrokes or expr ?
    صحيح		timeout,		// seconds to wait or zero
    Bool	localLoop,		// Throw away everything but result
    صحيح		silent)			// don't complain about no server
{
    کھڑکی	    w;
    char_u	    *property;
    صحيح		    لمائی;
    صحيح		    res;
    ساكن صحيح	    serial = 0;	// Running count of sent commands.
				// Used to give each command a
				// different serial number.
    PendingCommand  pending;
    char_u	    *loosename = عدم;

    إذا (نتیجہ != عدم)
	*نتیجہ = عدم;
    إذا (name == عدم || *name == NUL)
	name = (char_u *)"GVIM";    // use a default name

    إذا (commProperty == None && dpy != عدم && SendInit(dpy) < 0)
	أرجع -1;

# إذا defined(FEAT_EVAL)
    ch_log(عدم, "serverSendToVim(%s, %s)", name, cmd);
# endif

    // Execute locally if no display or target is ourselves
    إذا (dpy == عدم || (serverName != عدم && STRICMP(name, serverName) == 0))
	أرجع send_منزل_Local_Vim(cmd, as_اظہاریہ, نتیجہ);

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
	    إذا (!کھڑکی_Valid(dpy, w))
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
    لمائی = STRLEN(name) + STRLEN(p_enc) + STRLEN(cmd) + 14;
    property = alloc(لمائی + 30);

    sprintf((حرف *)property, "%c%c%c-n %s%c-E %s%c-s %s",
		      0, as_اظہاریہ ? 'c' : 'k', 0, name, 0, p_enc, 0, cmd);
    إذا (name == loosename)
	vim_free(loosename);
    // Add a back reference to our comm window
    serial++;
    sprintf((حرف *)property + لمائی, "%c-r %x %d",
						0, (int_u)comm_کھڑکی, serial);
    // Add length of what "-r %x %d" resulted in, skipping the NUL.
    لمائی += STRLEN(property + لمائی + 1) + 1;

    res = AppendPropCarefully(dpy, w, commProperty, property, لمائی + 1);
    vim_free(property);
    إذا (res < 0)
    {
	emsg(_(e_failed_منزل_send_command_منزل_destination_پروگرام));
	أرجع -1;
    }

    إذا (!as_اظہاریہ) // There is no answer for this - Keys are sent async
	أرجع 0;

    /*
     * Register the fact that we're waiting for a command to
     * complete (this is needed by SendEventProc and by
     * AppendErrorProc to pass back the command's results).
     */
    pending.serial = serial;
    pending.کوڈ = 0;
    pending.نتیجہ = عدم;
    pending.اگلا_Ptr = pendingCommands;
    pendingCommands = &pending;

    ServerWait(dpy, w, WaitForPend, &pending, localLoop,
						  timeout > 0 ? timeout : 600);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */
    إذا (pendingCommands == &pending)
	pendingCommands = pending.اگلا_Ptr;
    وإلا
    {
	PendingCommand *pcPtr;

	لكل (pcPtr = pendingCommands; pcPtr != عدم; pcPtr = pcPtr->اگلا_Ptr)
	    إذا (pcPtr->اگلا_Ptr == &pending)
	    {
		pcPtr->اگلا_Ptr = pending.اگلا_Ptr;
		اكسر;
	    }
    }

# إذا defined(FEAT_EVAL)
    ch_log(عدم, "serverSendToVim() result: %s",
	    pending.نتیجہ == عدم ? "NULL" : (حرف *)pending.نتیجہ);
# endif
    إذا (نتیجہ != عدم)
	*نتیجہ = pending.نتیجہ;
    وإلا
	vim_free(pending.نتیجہ);

    أرجع pending.کوڈ == 0 ? 0 : -1;
}

    ساكن صحيح
WaitForPend(فراغ *p)
{
    PendingCommand *pending = (PendingCommand *) p;
    أرجع pending->نتیجہ != عدم;
}

/*
 * Return TRUE if window "w" exists and has a "Vim" property on it.
 */
    ساكن صحيح
کھڑکی_Valid(Display *dpy, کھڑکی w)
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
    کھڑکی	w,
    سرا_Cond	سرا_Cond_2,
    فراغ	*سرا_انکڑے,
    صحيح		localLoop,
    صحيح		seconds)
{
    time_t	    شروع;
    time_t	    now;
    XEvent	    event;

# define UI_MSEC_DELAY 53
# define SEND_MSEC_POLL 500
# ifdef HAVE_SELECT
    fd_set	    fds;

    FD_ZERO(&fds);
    FD_SET(Connection_نمبر(dpy), &fds);
# وإلا
    بنية pollfd   fds;

    fds.fd = Connection_نمبر(dpy);
    fds.events = POLLIN;
# endif

    ویلا(&شروع);
    طالما (TRUE)
    {
	طالما (XCheck_کھڑکی_Event(dpy, comm_کھڑکی, Property_Change_نقاب, &event))
	    serverEventProc(dpy, &event, 1);
	server_parse_messages();

	إذا (سرا_Cond_2(سرا_انکڑے) != 0)
	    اكسر;
	إذا (!کھڑکی_Valid(dpy, w))
	    اكسر;
	ویلا(&now);
	إذا (seconds >= 0 && (now - شروع) >= seconds)
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
	    ui_delay((لما)UI_MSEC_DELAY, TRUE);
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
server_Get_Vim_ناں(Display *dpy)
{
    char_u	*regProp;
    char_u	*اندراج;
    char_u	*p;
    long_u	عدد_اشیاء;
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
    إذا (GetRegProp(dpy, &regProp, &عدد_اشیاء, TRUE) == FAIL)
	أرجع عدم;

    /*
     * Scan all of the names out of the property.
     */
    لكل (p = regProp; (long_u)(p - regProp) < عدد_اشیاء; p++)
    {
	اندراج = p;
	طالما (*p != 0 && !محفوظ_isspace(*p))
	    p++;
	إذا (*p != 0)
	{
	    w = None;
	    لڑی_توں_پڑھو((حرف *)اندراج, "%x", &w);
	    إذا (کھڑکی_Valid(dpy, (کھڑکی)w))
		list_append_string(list, p + 1, -1);
	    طالما (*p != 0)
		p++;
	}
    }
    إذا (regProp != خالی_تھاواں_prop)
	XFree(regProp);
    أرجع list;
}

/////////////////////////////////////////////////////////////
// Reply stuff

    ساكن بنية Server_جواب *
Server_جواب_لبھو(کھڑکی w, تعداد Server_جواب_Op op)
{
    بنية Server_جواب *p;
    بنية Server_جواب e;
    صحيح		i;

    p = (بنية Server_جواب *) server_جواب.ga_انکڑے;
    لكل (i = 0; i < server_جواب.ga_len; i++, p++)
	إذا (p->id == w)
	    اكسر;
    إذا (i >= server_جواب.ga_len)
	p = عدم;

    إذا (p == عدم && op == SROP_جوڑو)
    {
	إذا (server_جواب.ga_growsize == 0)
	    ga_init2(&server_جواب, حجم(بنية Server_جواب), 1);
	إذا (ga_grow(&server_جواب, 1) == OK)
	{
	    p = ((بنية Server_جواب *) server_جواب.ga_انکڑے)
		+ server_جواب.ga_len;
	    e.id = w;
	    ga_init2(&e.لڑیاں, 1, 100);
	    mch_memmove(p, &e, حجم(e));
	    server_جواب.ga_len++;
	}
    }
    وإلا إذا (p != عدم && op == SROP_Delete)
    {
	ga_clear(&p->لڑیاں);
	mch_memmove(p, p + 1, (server_جواب.ga_len - i - 1) * حجم(*p));
	server_جواب.ga_len--;
    }

    أرجع p;
}

/*
 * Convert string to windowid.
 * Issue an error if the id is invalid.
 */
    کھڑکی
server_Str_منزل_Win(char_u *str)
{
    بنانشانی  id = None;

    لڑی_توں_پڑھو((حرف *)str, "0x%x", &id);
    إذا (id == None)
	semsg(_(e_invalid_server_id_ورتیا_str), str);

    أرجع (کھڑکی)id;
}

/*
 * Send a reply string (notification) to client with id "name".
 * Return -1 if the window is invalid.
 */
    صحيح
server_Send_جواب(char_u *name, char_u *str)
{
    char_u	*property;
    صحيح		لمائی;
    صحيح		res;
    Display	*dpy = X_DISPLAY;
    کھڑکی	win = server_Str_منزل_Win(name);

    إذا (commProperty == None)
    {
	إذا (SendInit(dpy) < 0)
	    أرجع -2;
    }
    إذا (!کھڑکی_Valid(dpy, win))
	أرجع -1;

    لمائی = STRLEN(p_enc) + STRLEN(str) + 14;
    إذا ((property = alloc(لمائی + 30)) == عدم)
	أرجع -1;

    sprintf((حرف *)property, "%cn%c-E %s%c-n %s%c-w %x",
	    0, 0, p_enc, 0, str, 0, (بنانشانی صحيح)comm_کھڑکی);
    // Add length of what "%x" resulted in.
    لمائی += STRLEN(property + لمائی);
    res = AppendPropCarefully(dpy, win, commProperty, property, لمائی + 1);
    vim_free(property);

    أرجع res;
}

    ساكن صحيح
Wait_For_جواب(فراغ *p)
{
    کھڑکی  *w = (کھڑکی *) p;

    أرجع Server_جواب_لبھو(*w, SROP_لبھو) != عدم;
}

/*
 * Wait for replies from id (win)
 * When "timeout" is non-zero wait up to this many seconds.
 * Return 0 and the allocated string in "*str" when a reply is available.
 * Return -1 if the window becomes invalid while waiting.
 */
    صحيح
server_Read_جواب(
    Display	*dpy,
    کھڑکی	win,
    char_u	**str,
    صحيح		localLoop,
    صحيح		timeout)
{
    صحيح		len;
    char_u	*s;
    بنية	Server_جواب *p;

    ServerWait(dpy, win, Wait_For_جواب, &win, localLoop,
						   timeout > 0 ? timeout : -1);

    إذا ((p = Server_جواب_لبھو(win, SROP_لبھو)) != عدم && p->لڑیاں.ga_len > 0)
    {
	*str = vim_strsave(p->لڑیاں.ga_انکڑے);
	len = STRLEN(*str) + 1;
	إذا (len < p->لڑیاں.ga_len)
	{
	    s = (char_u *) p->لڑیاں.ga_انکڑے;
	    mch_memmove(s, s + len, p->لڑیاں.ga_len - len);
	    p->لڑیاں.ga_len -= len;
	}
	وإلا
	{
	    // Last string read.  Remove from list
	    ga_clear(&p->لڑیاں);
	    Server_جواب_لبھو(win, SROP_Delete);
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
server_Peek_جواب(Display *dpy, کھڑکی win, char_u **str)
{
    بنية Server_جواب *p;

    إذا ((p = Server_جواب_لبھو(win, SROP_لبھو)) != عدم && p->لڑیاں.ga_len > 0)
    {
	إذا (str != عدم)
	    *str = p->لڑیاں.ga_انکڑے;
	أرجع 1;
    }
    إذا (!کھڑکی_Valid(dpy, win))
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

    إذا (comm_کھڑکی == None)
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
			(صحيح)STRLEN_LITERAL(VIM_نسخہ_SHORT) + 1);

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
    ساكن کھڑکی
LookupName(
    Display	*dpy,	    // Display whose registry to check.
    char_u	*name,	    // Name of a server.
    صحيح		delete,	    // If non-zero, delete info about name.
    char_u	**loose)    // Do another search matching -999 if not found
			    // Return result here if a match is found
{
    char_u	*regProp, *اندراج;
    char_u	*p;
    long_u	عدد_اشیاء;
    int_u	return_قدر;

    /*
     * Read the registry property.
     */
    إذا (GetRegProp(dpy, &regProp, &عدد_اشیاء, FALSE) == FAIL)
	أرجع 0;

    /*
     * Scan the property for the desired name.
     */
    return_قدر = (int_u)None;
    اندراج = عدم;	// Not needed, but eliminates compiler warning.
    لكل (p = regProp; (long_u)(p - regProp) < عدد_اشیاء; )
    {
	اندراج = p;
	طالما (*p != 0 && !محفوظ_isspace(*p))
	    p++;
	إذا (*p != 0 && STRICMP(name, p + 1) == 0)
	{
	    لڑی_توں_پڑھو((حرف *)اندراج, "%x", &return_قدر);
	    اكسر;
	}
	طالما (*p != 0)
	    p++;
	p++;
    }

    إذا (loose != عدم && return_قدر == (int_u)None && !IsSerialName(name))
    {
	لكل (p = regProp; (long_u)(p - regProp) < عدد_اشیاء; )
	{
	    اندراج = p;
	    طالما (*p != 0 && !محفوظ_isspace(*p))
		p++;
	    إذا (*p != 0 && IsSerialName(p + 1)
		    && STRNICMP(name, p + 1, STRLEN(name)) == 0)
	    {
		إذا (لڑی_توں_پڑھو((حرف *)اندراج, "%x", &return_قدر) == 1
			&& return_قدر != (int_u)None)
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
    إذا (delete && return_قدر != (int_u)None)
    {
	صحيح گنتی;

	طالما (*p != 0)
	    p++;
	p++;
	گنتی = عدد_اشیاء - (p - regProp);
	إذا (گنتی > 0)
	    mch_memmove(اندراج, p, گنتی);
	XChangeProperty(dpy, جڑ_کھڑکی_2(dpy, 0), registryProperty, XA_STRING,
			8, Prop_منوال_Replace, regProp,
			(صحيح)(عدد_اشیاء - (p - اندراج)));
	XSync(dpy, False);
    }

    إذا (regProp != خالی_تھاواں_prop)
	XFree(regProp);
    أرجع (کھڑکی)return_قدر;
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
    کھڑکی	win)	// Window to remove
{
    char_u	*regProp, *اندراج = عدم;
    char_u	*p;
    long_u	عدد_اشیاء;
    int_u	wwin;

    /*
     * Read the registry property.
     */
    إذا (GetRegProp(dpy, &regProp, &عدد_اشیاء, FALSE) == FAIL)
	أرجع;

    // Scan the property for the window id.
    لكل (p = regProp; (long_u)(p - regProp) < عدد_اشیاء; )
    {
	إذا (*p != 0)
	{
	    لڑی_توں_پڑھو((حرف *)p, "%x", &wwin);
	    إذا ((کھڑکی)wwin == win)
	    {
		صحيح آخری_Half;

		// Copy down the remainder to delete entry
		اندراج = p;
		طالما (*p != 0)
		    p++;
		p++;
		آخری_Half = عدد_اشیاء - (p - regProp);
		إذا (آخری_Half > 0)
		    mch_memmove(اندراج, p, آخری_Half);
		عدد_اشیاء = (اندراج - regProp) + آخری_Half;
		p = اندراج;
		استمر;
	    }
	}
	طالما (*p != 0)
	    p++;
	p++;
    }

    إذا (اندراج != عدم)
    {
	XChangeProperty(dpy, جڑ_کھڑکی_2(dpy, 0), registryProperty,
			XA_STRING, 8, Prop_منوال_Replace, regProp,
			(صحيح)(p - regProp));
	XSync(dpy, False);
    }

    إذا (regProp != خالی_تھاواں_prop)
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
    صحيح		نتیجہ, actualFormat;
    long_u	بائٹاں_After;
    Atom	actualType;
    XErrorHandler old_handler;

    *regPropp = عدم;
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    نتیجہ = XGet_کھڑکی_Property(dpy, جڑ_کھڑکی_2(dpy, 0), registryProperty, 0L,
				(لما)MAX_PROP_الفاظ, False,
				XA_STRING, &actualType,
				&actualFormat, عدد_Itemsp, &بائٹاں_After,
				regPropp);

    XSync(dpy, FALSE);
    (فراغ)XSetErrorHandler(old_handler);
    إذا (got_x_error)
	أرجع FAIL;

    إذا (actualType == None)
    {
	// No prop yet. Logically equal to the empty list
	*عدد_Itemsp = 0;
	*regPropp = خالی_تھاواں_prop;
	أرجع OK;
    }

    // If the property is improperly formed, then delete it.
    إذا (نتیجہ != Success || actualFormat != 8 || actualType != XA_STRING)
    {
	إذا (*regPropp != عدم)
	    XFree(*regPropp);
	XDeleteProperty(dpy, جڑ_کھڑکی_2(dpy, 0), registryProperty);
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
    صحيح		فوری)	// Run event immediately. Should mostly be 0.
{
    char_u	*propInfo;
    صحيح		نتیجہ, actualFormat;
    long_u	عدد_اشیاء, بائٹاں_After;
    Atom	actualType;

    إذا (eventPtr != عدم)
    {
	إذا (eventPtr->xproperty.atom != commProperty
		|| eventPtr->xproperty.state != Property_New_قدر)
	    أرجع;
    }

    /*
     * Read the comm property and delete it.
     */
    propInfo = عدم;
    نتیجہ = XGet_کھڑکی_Property(dpy, comm_کھڑکی, commProperty, 0L,
				(لما)MAX_PROP_الفاظ, True,
				XA_STRING, &actualType,
				&actualFormat, &عدد_اشیاء, &بائٹاں_After,
				&propInfo);

    // If the property doesn't exist or is improperly formed then ignore it.
    إذا (نتیجہ != Success || actualType != XA_STRING || actualFormat != 8)
    {
	إذا (propInfo != عدم)
	    XFree(propInfo);
	أرجع;
    }
    إذا (فوری)
	server_parse_سنیہا(dpy, propInfo, عدد_اشیاء);
    وإلا
	save_وچ_قطار(propInfo, عدد_اشیاء);
}

/*
 * Saves x clientserver commands in a queue so that they can be called when
 * vim is idle.
 */
    ساكن فراغ
save_وچ_قطار(char_u *propInfo, long_u len)
{
    x_قطار_T *گرہ;

    گرہ = ALLOC_ONE(x_قطار_T);
    إذا (گرہ == عدم)
	أرجع;	    // out of memory
    گرہ->propInfo = propInfo;
    گرہ->len = len;

    إذا (سر.اگلا == عدم)   // initialize circular queue
    {
	سر.اگلا = &سر;
	سر.prev = &سر;
    }

    // insert node at tail of queue
    گرہ->اگلا = &سر;
    گرہ->prev = سر.prev;
    سر.prev->اگلا = گرہ;
    سر.prev = گرہ;
}

/*
 * Parses queued clientserver messages.
 */
    فراغ
server_parse_messages(فراغ)
{
    x_قطار_T	*گرہ;

    إذا (!X_DISPLAY)
	أرجع; // cannot happen?
    طالما (سر.اگلا != عدم && سر.اگلا != &سر)
    {
	گرہ = سر.اگلا;
	سر.اگلا = گرہ->اگلا;
	گرہ->اگلا->prev = گرہ->prev;
	server_parse_سنیہا(X_DISPLAY, گرہ->propInfo, گرہ->len);
	vim_free(گرہ);
    }
}

/*
 * Returns a non-zero value if there are clientserver messages waiting
 * int the queue.
 */
    صحيح
server_اڈیک(فراغ)
{
    أرجع سر.اگلا != عدم && سر.اگلا != &سر;
}

/*
 * Prases a single clientserver message. A single message may contain multiple
 * commands.
 * "propInfo" will be freed.
 */
    ساكن فراغ
server_parse_سنیہا(
    Display	*dpy,
    char_u	*propInfo, // A string containing 0 or more X commands
    long_u	عدد_اشیاء)  // The size of propInfo in bytes.
{
    char_u	*p;
    صحيح		کوڈ;
    char_u	*tofree;

# إذا defined(FEAT_EVAL)
    ch_log(عدم, "server_parse_message() numItems: %ld", عدد_اشیاء);
# endif

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    لكل (p = propInfo; (long_u)(p - propInfo) < عدد_اشیاء; )
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
	    کھڑکی	res_کھڑکی;
	    char_u	*name, *script, *serial, *سرا;
	    Bool	as_کلیدیں = *p == 'k';
	    char_u	*enc;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = عدم;
	    res_کھڑکی = None;
	    serial = (char_u *)"";
	    script = عدم;
	    enc = عدم;
	    طالما ((long_u)(p - propInfo) < عدد_اشیاء && *p == '-')
	    {
# إذا defined(FEAT_EVAL)
		ch_log(عدم, "server_parse_message() item: %c, %s", p[-2], p);
# endif
		بدل (p[1])
		{
		    حالة 'r':
			سرا = skipwhite(p + 2);
			res_کھڑکی = 0;
			طالما (vim_isxdigit(*سرا))
			{
			    res_کھڑکی = 16 * res_کھڑکی + (long_u)hex2nr(*سرا);
			    ++سرا;
			}
			إذا (سرا == p + 2 || *سرا != ' ')
			    res_کھڑکی = None;
			وإلا
			{
			    p = serial = سرا + 1;
			    client_کھڑکی = res_کھڑکی; // Remember in global
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
		إذا (as_کلیدیں)
		    server_منزل_داخلہ_buf(script);
		وإلا
		{
		    char_u      *res;

		    res = eval_client_اظہاریہ_منزل_string(script);
		    إذا (res_کھڑکی != None)
		    {
			garray_T    جواب;

			// Initialize the result property.
			ga_init2(&جواب, 1, 100);
			(فراغ)ga_grow(&جواب, 50 + STRLEN(p_enc) + STRLEN(serial));
			sprintf(جواب.ga_انکڑے, "%cr%c-E %s%c-s %s%c-r ",
						   0, 0, p_enc, 0, serial, 0);
			جواب.ga_len = 14 + STRLEN(p_enc) + STRLEN(serial);

			// Evaluate the expression and return the result.
			إذا (res != عدم)
			    ga_concat(&جواب, res);
			وإلا
			{
			    ga_concat(&جواب,
				   (char_u *)_(e_invalid_عبارت_received));
			    ga_append(&جواب, 0);
			    GA_CONCAT_LITERAL(&جواب, "-c 1");
			}
			ga_append(&جواب, NUL);
			(فراغ)AppendPropCarefully(dpy, res_کھڑکی, commProperty,
						 جواب.ga_انکڑے, جواب.ga_len);
			ga_clear(&جواب);
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
	    کوڈ = 0;
	    enc = عدم;
	    طالما ((long_u)(p - propInfo) < عدد_اشیاء && *p == '-')
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
			إذا (لڑی_توں_پڑھو((حرف *)p + 2, " %d", &serial) == 1)
			    gotSerial = 1;
			اكسر;
		    حالة 'c':
			إذا (لڑی_توں_پڑھو((حرف *)p + 2, " %d", &کوڈ) != 1)
			    کوڈ = 0;
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
	    لكل (pcPtr = pendingCommands; pcPtr != عدم; pcPtr = pcPtr->اگلا_Ptr)
	    {
		إذا (serial != pcPtr->serial || pcPtr->نتیجہ != عدم)
		    استمر;

		pcPtr->کوڈ = کوڈ;
		res = serverConvert(enc, res, &tofree);
		إذا (tofree == عدم)
		    res = vim_strsave(res);
		pcPtr->نتیجہ = res;
		اكسر;
	    }
	}
	وإلا إذا (*p == 'n' && p[1] == 0)
	{
	    کھڑکی	win = 0;
	    بنانشانی صحيح u;
	    صحيح		got_کھڑکی;
	    char_u	*str;
	    بنية	Server_جواب *r;
	    char_u	*enc;

	    /*
	     * This is a (n)otification.  Sent with serverreply_send in Vim
	     * script.  Execute any autocommand and save it for later retrieval
	     */
	    p += 2;
	    got_کھڑکی = 0;
	    str = (char_u *)"";
	    enc = عدم;
	    طالما ((long_u)(p - propInfo) < عدد_اشیاء && *p == '-')
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
			إذا (لڑی_توں_پڑھو((حرف *)p + 2, " %x", &u) == 1)
			{
			    win = u;
			    got_کھڑکی = 1;
			}
			اكسر;
		}
		طالما (*p != 0)
		    p++;
		p++;
	    }

	    إذا (!got_کھڑکی)
		استمر;
	    str = serverConvert(enc, str, &tofree);
	    إذا ((r = Server_جواب_لبھو(win, SROP_جوڑو)) != عدم)
	    {
		ga_concat(&(r->لڑیاں), str);
		ga_append(&(r->لڑیاں), NUL);
	    }
	    {
		char_u	winstr[30];

		sprintf((حرف *)winstr, "0x%x", (بنانشانی صحيح)win);
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
    کھڑکی	کھڑکی_2,		// Window whose property is to be modified.
    Atom	property,	// Name of property.
    char_u	*قدر,		// Characters  to append to property.
    صحيح		لمائی)		// How much to append
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, کھڑکی_2, property, XA_STRING, 8,
					       Prop_منوال_Append, قدر, لمائی);
    XSync(dpy, False);
    (فراغ) XSetErrorHandler(old_handler);
    أرجع got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
    ساكن صحيح
x_error_check(Display *dpy بن_ورتیا, XErrorEvent *error_event بن_ورتیا)
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
#جے_انت	// FEAT_CLIENTSERVER
