/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Tcl extensions by Ingo Wilken <Ingo.Wilken@informatik.uni-oldenburg.de>
 * Last modification: Wed May 10 21:28:44 CEST 2000
 * Requires Tcl 8.0 or higher.
 *
 *  Variables:
 *  ::vim::current(buffer)	# Name of buffer command for current buffer.
 *  ::vim::current(window)	# Name of window command for current window.
 *  ::vim::range(start)		# Start of current range (line number).
 *  ::vim::range(end)		# End of current range (line number).
 *  ::vim::lbase		# Start of line/column numbers (1 or 0).
 *
 *  Commands:
 *  ::vim::command {cmd}	# Execute ex command {cmd}.
 *  ::vim::option {opt} [val]	# Get/Set option {opt}.
 *  ::vim::expr {expr}		# Evaluate {expr} using vim's evaluator.
 *  ::vim::beep			# Guess.
 *
 *  set buf [::vim::buffer {n}]	# Create Tcl command for buffer N.
 *  set bl [::vim::buffer list] # Get list of Tcl commands of all buffers.
 *  ::vim::buffer exists {n}	# True if buffer {n} exists.
 *
 *  set wl [::vim::window list] # Get list of Tcl commands of all windows.
 *
 *  set n [$win height]		# Report window height.
 *  $win height {n}		# Set window height to {n}.
 *  array set pos [$win cursor] # Get cursor position.
 *  $win cursor {row} {col}	# Set cursor position.
 *  $win cursor pos		# Set cursor position from array var "pos"
 *  $win delcmd {cmd}		# Register callback command for closed window.
 *  $win option {opt} [val]	# Get/Set vim option in context of $win.
 *  $win command {cmd}		# Execute ex command in context of $win.
 *  $win expr {expr}		# Evaluate vim expression in context of $win.
 *  set buf [$win buffer]	# Create Tcl command for window's buffer.
 *
 *  $buf name			# Reports file name in buffer.
 *  $buf number			# Reports buffer number.
 *  set l [$buf get {n}]	# Get buffer line {n} as a string.
 *  set L [$buf get {n} {m}]	# Get lines {n} through {m} as a list.
 *  $buf count			# Reports number of lines in buffer.
 *  $buf last			# Reports number of last line in buffer.
 *  $buf delete {n}		# Delete line {n}.
 *  $buf delete {n} {m}		# Delete lines {n} through {m}.
 *  $buf set {n} {l}		# Set line {n} to string {l}.
 *  $buf set {n} {m} {L}	# Set lines {n} through {m} from list {L}.
 *				# Delete/inserts lines as appropriate.
 *  $buf option {opt} [val]	# Get/Set vim option in context of $buf.
 *  $buf command {cmd}		# Execute ex command in context of $buf
 *  $buf expr {cmd}		# Evaluate vim expression in context of $buf.
 *  array set pos [$buf mark {m}]   # Get position of mark.
 *  $buf append {n} {str}	# Append string {str} to buffer,after line {n}.
 *  $buf insert {n} {str}	# Insert string {str} in buffer as line {n}.
 *  $buf delcmd {cmd}		# Register callback command for deleted buffer.
 *  set wl [$buf windows]	# Get list of Tcl commands for all windows of
 *				# this buffer.
TODO:
 *  ::vim::buffer new		#   create new buffer + Tcl command
 */

#అన్తర్భావ "vim.h"
#నిర్వచనం_రద్దు EXTERN			// tcl.h defines it too

#నిర్వచితమైతే DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#అయితే_ముగింపు

#అన్తర్భావ <tcl.h>
#అన్తర్భావ <string.h>

రకనిర్వచనం నిర్మాణం
{
    Tcl_Interp *interp;
    పూర్ణాంకం exitvalue;
    పూర్ణాంకం range_ప్రారంభం, range_చివర;
    పూర్ణాంకం lbase;
    అక్షరం *curbuf, *curwin;
} tcl_info;

స్థిర tcl_info tclinfo = { ఖాళీ, 0, 0, 0, 0, ఖాళీ, ఖాళీ };

#నిర్వచించు VAR_RANGE1	"::vim::range(start)"
#నిర్వచించు VAR_RANGE2	"::vim::range(begin)"
#నిర్వచించు VAR_RANGE3	"::vim::range(end)"
#నిర్వచించు VAR_CURBUF	"::vim::current(buffer)"
#నిర్వచించు VAR_CURWIN	"::vim::current(window)"
#నిర్వచించు VAR_LBASE	"::vim::lbase"
#నిర్వచించు VAR_CURLINE	"line"
#నిర్వచించు VAR_CURLNUM	"lnum"
#నిర్వచించు VARNAME_కొలత	64

#నిర్వచించు row2tcl(x)  ((x) - (tclinfo.lbase==0))
#నిర్వచించు row2vim(x)  ((x) + (tclinfo.lbase==0))
#నిర్వచించు col2tcl(x)  ((x) + (tclinfo.lbase!=0))
#నిర్వచించు col2vim(x)  ((x) - (tclinfo.lbase!=0))


#నిర్వచించు VIMOUT	((Client_దత్తాంశం)1)
#నిర్వచించు VIMERR	((Client_దత్తాంశం)2)

// This appears to be new in Tcl 8.4.
#నిర్వచితం_కాకపోతే CONST84
# define CONST84
#అయితే_ముగింపు

/*
 *  List of Tcl interpreters who reference a vim window or buffer.
 *  Each buffer and window has its own list in the w_tcl_ref or b_tcl_ref
 *  struct member.  We need this because Tcl can create sub-interpreters with
 *  the "interp" command, and each interpreter can reference all windows and
 *  buffers.
 */
నిర్మాణం ref
{
    నిర్మాణం ref	*తదుపరి;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    శూన్య	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
స్థిర అక్షరం * tclgetbuffer ANSI_వాదనలు((Tcl_Interp *interp, buf_T *buf));
స్థిర అక్షరం * tclgetwindow ANSI_వాదనలు((Tcl_Interp *interp, win_T *win));
స్థిర పూర్ణాంకం tclsetdelcmd ANSI_వాదనలు((Tcl_Interp *interp, నిర్మాణం ref *reflist, శూన్య *vimobj, Tcl_Obj *delcmd));
స్థిర పూర్ణాంకం tclgetlinenum ANSI_వాదనలు ((Tcl_Interp *interp, Tcl_Obj *obj, పూర్ణాంకం *విలువ_P, buf_T *buf));
స్థిర win_T *tclfindwin ANSI_వాదనలు ((buf_T *buf));
స్థిర పూర్ణాంకం tcldoexcommand ANSI_వాదనలు ((Tcl_Interp *interp, పూర్ణాంకం objc, Tcl_Obj *CONST objv[], పూర్ణాంకం objn));
స్థిర పూర్ణాంకం tclsetoption ANSI_వాదనలు ((Tcl_Interp *interp, పూర్ణాంకం objc, Tcl_Obj *CONST objv[], పూర్ణాంకం objn));
స్థిర పూర్ణాంకం tclvimexpr ANSI_వాదనలు ((Tcl_Interp *interp, పూర్ణాంకం objc, Tcl_Obj *CONST objv[], పూర్ణాంకం objn));
స్థిర శూన్య tcldelthisinterp ANSI_వాదనలు ((శూన్య));

స్థిర పూర్ణాంకం vimerror ANSI_వాదనలు((Tcl_Interp *interp));
స్థిర శూన్య tclmsg ANSI_వాదనలు((అక్షరం *పాఠ్యం));
స్థిర శూన్య tclerrmsg ANSI_వాదనలు((అక్షరం *పాఠ్యం));
స్థిర శూన్య tclupdatevars ANSI_వాదనలు((శూన్య));

స్థిర నిర్మాణం ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#ఒకవేళ defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
రకనిర్వచనం పూర్ణాంకం HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define ఎక్కించు_dll vim_ఎక్కించు_Lib
#  define చిహ్నం_from_dll Get_Proc_చిరునామా
#  define close_dll FreeLibrary
#  define ఎక్కించు_dll_error GetWin32Error
# లేకపోతే
#  include <dlfcn.h>
#  define HANDLE శూన్య*
#  define TCL_PROC శూన్య*
#  define ఎక్కించు_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define చిహ్నం_from_dll dlsym
#  define close_dll dlclose
#  define ఎక్కించు_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
స్థిర HANDLE hTclLib = ఖాళీ;
Tcl_Interp* (*dll_Tcl_CreateInterp)(శూన్య);
శూన్య (*dll_Tcl_కనుగొను_Executable)(స్థిరరాశి శూన్య *);

/*
 * Table of name to function pointer of tcl.
 */
స్థిర నిర్మాణం {
    అక్షరం* name;
    TCL_PROC* ptr;
} tcl_funcname_పట్టిక[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_కనుగొను_Executable},
    {ఖాళీ, ఖాళీ},
};

/*
 * Make all runtime-links of tcl.
 *
 * 1. Get module handle using LoadLibraryEx.
 * 2. Get pointer to tcl function by GetProcAddress.
 * 3. Repeat 2, until get all functions will be used.
 *
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
tcl_runtime_link_init(అక్షరం *libname, పూర్ణాంకం verbose)
{
    పూర్ణాంకం i;

    ఒకవేళ (hTclLib)
	తిరిగి OK;
    ఒకవేళ (!(hTclLib = ఎక్కించు_dll(libname)))
    {
	ఒకవేళ (verbose)
	    semsg(_(e_could_not_ఎక్కించు_library_str_str), libname, ఎక్కించు_dll_error());
	తిరిగి FAIL;
    }
    కొరకు (i = 0; tcl_funcname_పట్టిక[i].ptr; ++i)
    {
	ఒకవేళ (!(*tcl_funcname_పట్టిక[i].ptr = చిహ్నం_from_dll(hTclLib,
						  tcl_funcname_పట్టిక[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = ఖాళీ;
	    ఒకవేళ (verbose)
		semsg(_(e_could_not_ఎక్కించు_library_function_str),
						   tcl_funcname_పట్టిక[i].name);
	    తిరిగి FAIL;
	}
    }
    తిరిగి OK;
}
#అయితే_ముగింపు // defined(DYNAMIC_TCL)

#నిర్వచితమైతే DYNAMIC_TCL
స్థిర అక్షరం *కనుగొను_executable_arg = ఖాళీ;
#అయితే_ముగింపు

    శూన్య
vim_tcl_init(అక్షరం *arg)
{
#నిర్వచితమైతే DYNAMIC_TCL
    కనుగొను_executable_arg = arg;
#లేకపోతే
    Tcl_కనుగొను_Executable(arg);
#అయితే_ముగింపు
}

#ఒకవేళ defined(DYNAMIC_TCL)

స్థిర పూర్ణాంకం stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    పూర్ణాంకం
tcl_enabled(పూర్ణాంకం verbose)
{
    ఒకవేళ (!stubs_initialized && కనుగొను_executable_arg != ఖాళీ
	    && tcl_runtime_link_init((అక్షరం *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_కనుగొను_Executable(కనుగొను_executable_arg);

	ఒకవేళ ((interp = dll_Tcl_CreateInterp()) != ఖాళీ)
	{
	    ఒకవేళ (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != ఖాళీ)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    తిరిగి stubs_initialized;
}
#అయితే_ముగింపు

#ఒకవేళ defined(EXITFREE)
/*
 * Called once when exiting.
 */
    శూన్య
vim_tcl_finalize(శూన్య)
{
# ifdef DYNAMIC_TCL
    ఒకవేళ (stubs_initialized)
# endif
	Tcl_Finalize();
}
#అయితే_ముగింపు

    శూన్య
tcl_చివర(శూన్య)
{
}

/////////////////////////////////////////////////////////////////////////////
// Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Replace standard "exit" command.
 *
 * Delete the Tcl interpreter; a new one will be created with the next
 * :tcl command). The exit code is saved (and retrieved in tclexit()).
 * Since Tcl's exit is never expected to return and this replacement
 * does, then (except for a trivial case) additional Tcl commands will
 * be run. Since the interpreter is now marked as deleted, an error
 * will be returned -- typically "attempt to call eval in deleted
 * interpreter". Hopefully, at this point, checks for TCL_ERROR take
 * place and control percolates back up to Vim -- but with this new error
 * string in the interpreter's result value. Therefore it would be
 * useless for this routine to return the exit code via Tcl_SetResult().
 */
    స్థిర పూర్ణాంకం
exitcmd(
    Client_దత్తాంశం dummy వాడని,
    Tcl_Interp *interp,
    పూర్ణాంకం objc,
    Tcl_Obj *CONST objv[])
{
    పూర్ణాంకం విలువ = 0;

    ఎంపిక (objc)
    {
	స్థితి 2:
	    ఒకవేళ (Tcl_GetIntFromObj(interp, objv[1], &విలువ) != TCL_OK)
		ఆపు;
	    // FALLTHROUGH
	స్థితి 1:
	    tclinfo.exitvalue = విలువ;

	    Tcl_DeleteInterp(interp);
	    ఆపు;
	అప్రమేయ:
	    Tcl_Wrong_అంకె_వాదనలు(interp, 1, objv, "?returnCode?");
    }
    తిరిగి TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    స్థిర పూర్ణాంకం
beepcmd(
    Client_దత్తాంశం dummy వాడని,
    Tcl_Interp *interp,
    పూర్ణాంకం objc,
    Tcl_Obj *CONST objv[])
{
    ఒకవేళ (objc != 1)
    {
	Tcl_Wrong_అంకె_వాదనలు(interp, 1, objv, ఖాళీ);
	తిరిగి TCL_ERROR;
    }
    vim_beep(BO_LANG);
    తిరిగి TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    స్థిర పూర్ణాంకం
buffercmd(
    Client_దత్తాంశం dummy వాడని,
    Tcl_Interp *interp,
    పూర్ణాంకం objc,
    Tcl_Obj *CONST objv[])
{
    అక్షరం	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    పూర్ణాంకం		err, n, idx;
    గణన {BCMD_EXISTS, BCMD_LIST};
    స్థిర CONST84 అక్షరం *bcmdoptions[] =
    {
	"exists", "list", (అక్షరం *)0
    };

    ఒకవేళ (objc < 2)
    {
	Tcl_Wrong_అంకె_వాదనలు(interp, 1, objv, "option");
	తిరిగి TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    ఒకవేళ (err == TCL_OK)
    {
	ఒకవేళ (objc != 2)
	{
	    Tcl_Wrong_అంకె_వాదనలు(interp, 1, objv, "bufNumber");
	    తిరిగి TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    ఒకవేళ (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		ఒకవేళ (name == ఖాళీ)
		    తిరిగి TCL_ERROR;
		Tcl_సమితి_ఫలితం(interp, name, TCL_VOLATILE);
		తిరిగి TCL_OK;
	    }
	}
	Tcl_సమితి_ఫలితం(interp, _("invalid buffer number"), TCL_STATIC);
	తిరిగి TCL_ERROR;
    }
    Tcl_Reset_ఫలితం(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_Get_సూచీ_From_Obj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    ఒకవేళ (err != TCL_OK)
	తిరిగి err;
    ఎంపిక (idx)
    {
	స్థితి BCMD_LIST:
	    ఒకవేళ (objc != 2)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, "");
		err = TCL_ERROR;
		ఆపు;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		ఒకవేళ (name == ఖాళీ)
		{
		    err = TCL_ERROR;
		    ఆపు;
		}
		Tcl_AppendElement(interp, name);
	    }
	    ఆపు;

	స్థితి BCMD_EXISTS:
	    ఒకవేళ (objc != 3)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		ఆపు;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    ఒకవేళ (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != ఖాళీ);
		Tcl_సమితి_Obj_ఫలితం(interp, resobj);
	    }
	    ఆపు;

	అప్రమేయ:
	    Tcl_సమితి_ఫలితం(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    తిరిగి err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    స్థిర పూర్ణాంకం
windowcmd(
    Client_దత్తాంశం	dummy వాడని,
    Tcl_Interp	*interp,
    పూర్ణాంకం		objc,
    Tcl_Obj	*CONST objv[])
{
    అక్షరం	*what, *string;
    win_T	*win;

    ఒకవేళ (objc != 2)
    {
	Tcl_Wrong_అంకె_వాదనలు(interp, 1, objv, "option");
	తిరిగి TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], ఖాళీ);
    ఒకవేళ (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    ఒకవేళ (string == ఖాళీ)
		తిరిగి TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	తిరిగి TCL_OK;
    }
    Tcl_సమితి_ఫలితం(interp, _("unknown option"), TCL_STATIC);
    తిరిగి TCL_ERROR;
}

/*
 * flags for bufselfcmd and winselfcmd to indicate outstanding actions.
 */
#నిర్వచించు FL_UPDATE_SCREEN	(1<<0)
#నిర్వచించు FL_UPDATE_CURBUF	(1<<1)
#నిర్వచించు FL_ADJUST_CURSOR	(1<<2)

/*
 * This function implements the buffer commands.
 */
    స్థిర పూర్ణాంకం
bufselfcmd(
    Client_దత్తాంశం	ref,
    Tcl_Interp	*interp,
    పూర్ణాంకం		objc,
    Tcl_Obj	*CONST objv[])
{
    పూర్ణాంకం		opt, err, idx, flags;
    పూర్ణాంకం		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    అక్షరం	*పంక్తి;

    గణన
    {
	BUF_APPEND, BUF_COMMAND, BUF_లెక్క, BUF_DELCMD, BUF_DELETE, BUF_సమాసం,
	BUF_GET, BUF_చొప్పించు, BUF_చివరి, BUF_MARK, BUF_NAME, BUF_సంఖ్య,
	BUF_OPTION, BUF_సమితి, BUF_WINDOWS
    };
    స్థిర CONST84 అక్షరం *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (అక్షరం *)0
    };

    ఒకవేళ (objc < 2)
    {
	Tcl_Wrong_అంకె_వాదనలు(interp, 1, objv, "option ?arg ...?");
	తిరిగి TCL_ERROR;
    }

    err = Tcl_Get_సూచీ_From_Obj(interp, objv[1], bufoptions, "option", 0, &idx);
    ఒకవేళ (err != TCL_OK)
	తిరిగి err;

    buf = (buf_T *)((నిర్మాణం ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    ఎంపిక (idx)
    {
	స్థితి BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ఆపు;

	స్థితి BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ఆపు;

	స్థితి BUF_సమాసం:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ఆపు;

	స్థితి BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    ఒకవేళ (objc != 2)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, ఖాళీ);
		err = TCL_ERROR;
		ఆపు;
	    }
	    ఒకవేళ (buf->b_ffname)
		Tcl_సమితి_ఫలితం(interp, (అక్షరం *)buf->b_ffname, TCL_VOLATILE);
	    లేకపోతే
		Tcl_సమితి_ఫలితం(interp, "", TCL_STATIC);
	    ఆపు;

	స్థితి BUF_చివరి:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	స్థితి BUF_లెక్క:
	    /*
	     * Get number of lines in buffer.
	     */
	    ఒకవేళ (objc != 2)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, ఖాళీ);
		err = TCL_ERROR;
		ఆపు;
	    }
	    val1 = (పూర్ణాంకం)buf->b_ml.ml_పంక్తి_లెక్క;
	    ఒకవేళ (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_సమితి_Obj_ఫలితం(interp, resobj);
	    ఆపు;

	స్థితి BUF_సంఖ్య:
	    /*
	     * Get buffer's number.
	     */
	    ఒకవేళ (objc != 2)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, ఖాళీ);
		err = TCL_ERROR;
		ఆపు;
	    }
	    resobj = Tcl_NewIntObj((పూర్ణాంకం)buf->b_fnum);
	    Tcl_సమితి_Obj_ఫలితం(interp, resobj);
	    ఆపు;

	స్థితి BUF_GET:
	    ఒకవేళ (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ఆపు;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ఒకవేళ (err != TCL_OK)
		ఆపు;
	    ఒకవేళ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ఒకవేళ (err != TCL_OK)
		    ఆపు;
		ఒకవేళ (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_Reset_ఫలితం(interp);

		కొరకు (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    పంక్తి = (అక్షరం *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    ఒకవేళ (పంక్తి)
			Tcl_AppendElement(interp, పంక్తి);
		    లేకపోతే
			err = TCL_ERROR;
		}
	    }
	    లేకపోతే
	    {  // objc == 3
		పంక్తి = (అక్షరం *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_సమితి_ఫలితం(interp, పంక్తి, TCL_VOLATILE);
	    }
	    ఆపు;

	స్థితి BUF_సమితి:
	    ఒకవేళ (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		ఆపు;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ఒకవేళ (err != TCL_OK)
		తిరిగి TCL_ERROR;
	    ఒకవేళ (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		పంక్తి = Tcl_GetStringFromObj(objv[3], ఖాళీ);
		ఒకవేళ (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_సమితి_ఫలితం(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		లేకపోతే
		ఒకవేళ (ml_replace((linenr_T)val1, (char_u *)పంక్తి, TRUE) != OK)
		{
		    Tcl_సమితి_ఫలితం(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		లేకపోతే
		{
		    మారింది_బైట్లు((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		ఆపు;
	    }
	    లేకపోతే
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		పూర్ణాంకం	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ఒకవేళ (err != TCL_OK)
		    ఆపు;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		ఒకవేళ (err != TCL_OK)
		    ఆపు;
		ఒకవేళ (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		ఒకవేళ (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_సమితి_ఫలితం(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    ఆపు;
		}
		flags |= FL_UPDATE_CURBUF;

		కొరకు (i = 0; i < lc && n <= val2; i++)
		{
		    పంక్తి = Tcl_GetStringFromObj(lv[i], ఖాళీ);
		    ఒకవేళ (ml_replace((linenr_T)n, (char_u *)పంక్తి, TRUE) != OK)
			వెళ్ళు సమితి_List_Error;
		    ++n;
		}
		ఒకవేళ (i < lc)
		{
		    // append lines
		    చేయి
		    {
			పంక్తి = Tcl_GetStringFromObj(lv[i], ఖాళీ);
			ఒకవేళ (ml_append((linenr_T)(n - 1),
					      (char_u *)పంక్తి, 0, FALSE) != OK)
			    వెళ్ళు సమితి_List_Error;
			++n;
			++i;
		    } అయితే (i < lc);
		}
		లేకపోతే ఒకవేళ (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    చేయి
		    {
			ఒకవేళ (ml_delete((linenr_T)i) != OK)
			    వెళ్ళు సమితి_List_Error;
			++n;
		    } అయితే (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (దీర్ఘ)MAXLNUM,
								    (దీర్ఘ)lc);
		మారింది_పంక్తులు((linenr_T)val1, 0, (linenr_T)val2 + 1, (దీర్ఘ)lc);
		ఆపు;
    సమితి_List_Error:
		u_undo(1);  // ???
		Tcl_సమితి_ఫలితం(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ఆపు;

	స్థితి BUF_DELETE:
	    ఒకవేళ (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ఆపు;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ఒకవేళ (err != TCL_OK)
		ఆపు;
	    val2 = val1;
	    ఒకవేళ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ఒకవేళ (err != TCL_OK)
		    తిరిగి err;
		ఒకవేళ (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    ఒకవేళ (u_savedel((linenr_T)val1, (దీర్ఘ)n) != OK)
	    {
		Tcl_సమితి_ఫలితం(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ఆపు;
	    }
	    కొరకు (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		ఒకవేళ (err != TCL_OK)
		    ఆపు;
	    }
	    ఒకవేళ (i > 0)
		deleted_పంక్తులు_mark((linenr_T)val1, (దీర్ఘ)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    ఆపు;

	స్థితి BUF_MARK:
	    ఒకవేళ (objc != 3)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, "markName");
		err = TCL_ERROR;
		ఆపు;
	    }
	    పంక్తి = Tcl_GetStringFromObj(objv[2], ఖాళీ);

	    pos = ఖాళీ;
	    ఒకవేళ (పంక్తి[0] != '\0'  &&  పంక్తి[1] == '\0')
		pos = getmark(పంక్తి[0], FALSE);
	    ఒకవేళ (pos == ఖాళీ)
	    {
		Tcl_సమితి_ఫలితం(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		ఆపు;
	    }
	    err = vimerror(interp);
	    ఒకవేళ (err != TCL_OK)
		ఆపు;
	    ఒకవేళ (pos->lnum <= 0)
	    {
		Tcl_సమితి_ఫలితం(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    లేకపోతే
	    {
		అక్షరం rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (పూర్ణాంకం)row2tcl(pos->lnum), (పూర్ణాంకం)col2tcl(pos->నిలువు));
		Tcl_సమితి_ఫలితం(interp, rbuf, TCL_VOLATILE);
	    }
	    ఆపు;

	స్థితి BUF_చొప్పించు:
	    opt = 1;
	    // fallthrough
	స్థితి BUF_APPEND:
	    ఒకవేళ (objc != 4)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		ఆపు;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ఒకవేళ (err != TCL_OK)
		ఆపు;
	    ఒకవేళ (opt)
		--val1;
	    ఒకవేళ (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_సమితి_ఫలితం(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ఆపు;
	    }

	    పంక్తి = Tcl_GetStringFromObj(objv[3], ఖాళీ);
	    ఒకవేళ (ml_append((linenr_T)val1, (char_u *)పంక్తి, 0, FALSE) != OK)
	    {
		Tcl_సమితి_ఫలితం(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ఆపు;
	    }
	    appended_పంక్తులు_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    ఆపు;

	స్థితి BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    ఒకవేళ (objc != 2)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, ఖాళీ);
		err = TCL_ERROR;
		ఆపు;
	    }
	    Tcl_Reset_ఫలితం(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		ఒకవేళ (win->w_బఫరు == buf)
		{
		    పంక్తి = tclgetwindow(interp, win);
		    ఒకవేళ (పంక్తి != ఖాళీ)
			Tcl_AppendElement(interp, పంక్తి);
		    లేకపోతే
		    {
			err = TCL_ERROR;
			ఆపు;
		    }
		}
	    }
	    ఆపు;

	స్థితి BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    ఒకవేళ (objc != 3)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, "command");
		err = TCL_ERROR;
		ఆపు;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (శూన్య *)buf, objv[2]);
	    ఆపు;

	అప్రమేయ:
	    Tcl_సమితి_ఫలితం(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    ఒకవేళ (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    ఒకవేళ (flags & FL_ADJUST_CURSOR)
	check_cursor();
    ఒకవేళ (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    తిరిగి err;
}

/*
 * This function implements the window commands.
 */
    స్థిర పూర్ణాంకం
winselfcmd(
    Client_దత్తాంశం	ref,
    Tcl_Interp	*interp,
    పూర్ణాంకం		objc,
    Tcl_Obj	*CONST objv[])
{
    పూర్ణాంకం		err, idx, flags;
    పూర్ణాంకం		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    అక్షరం	*str;

    గణన
    {
	WIN_బఫరు, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_సమాసం,
	WIN_ఎత్తు, WIN_OPTION
    };
    స్థిర CONST84 అక్షరం *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (అక్షరం *)0
    };

    ఒకవేళ (objc < 2)
    {
	Tcl_Wrong_అంకె_వాదనలు(interp, 1, objv, "option ?arg ...?");
	తిరిగి TCL_ERROR;
    }

    err = Tcl_Get_సూచీ_From_Obj(interp, objv[1], winoptions, "option", 0,  &idx);
    ఒకవేళ (err != TCL_OK)
	తిరిగి TCL_ERROR;

    win = (win_T *)((నిర్మాణం ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_బఫరు;
    flags = 0;

    ఎంపిక (idx)
    {
	స్థితి WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ఆపు;

	స్థితి WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ఆపు;

	స్థితి WIN_సమాసం:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ఆపు;

	స్థితి WIN_ఎత్తు:
	    ఒకవేళ (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		ఒకవేళ (err != TCL_OK)
		    ఆపు;
#నిర్వచితమైతే FEAT_GUI
		need_mouse_correct = TRUE;
#అయితే_ముగింపు
		win_setheight(val1);
		err = vimerror(interp);
		ఒకవేళ (err != TCL_OK)
		    ఆపు;
	    }
	    లేకపోతే
	    ఒకవేళ (objc != 2)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		ఆపు;
	    }

	    resobj = Tcl_NewIntObj((పూర్ణాంకం)(win->w_ఎత్తు));
	    Tcl_సమితి_Obj_ఫలితం(interp, resobj);
	    ఆపు;

	స్థితి WIN_బఫరు:
	    ఒకవేళ (objc != 2)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, ఖాళీ);
		err = TCL_ERROR;
		ఆపు;
	    }
	    str = tclgetbuffer(interp, win->w_బఫరు);
	    ఒకవేళ (str)
		Tcl_సమితి_ఫలితం(interp, str, TCL_VOLATILE);
	    లేకపోతే
		err = TCL_ERROR;
	    ఆపు;

	స్థితి WIN_DELCMD:
	    ఒకవేళ (objc != 3)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, "command");
		err = TCL_ERROR;
		ఆపు;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (శూన్య *)win, objv[2]);
	    ఆపు;

	స్థితి WIN_CURSOR:
	    ఒకవేళ (objc > 4)
	    {
		Tcl_Wrong_అంకె_వాదనలు(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		ఆపు;
	    }
	    ఒకవేళ (objc == 2)
	    {
		అక్షరం buf[64];

		sprintf(buf, _("row %d column %d"), (పూర్ణాంకం)row2tcl(win->w_cursor.lnum), (పూర్ణాంకం)col2tcl(win->w_cursor.నిలువు));
		Tcl_సమితి_ఫలితం(interp, buf, TCL_VOLATILE);
		ఆపు;
	    }
	    లేకపోతే ఒకవేళ (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_సందేశం);
		ఒకవేళ (var == ఖాళీ)
		{
		    err = TCL_ERROR;
		    ఆపు;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_బఫరు);
		ఒకవేళ (err != TCL_OK)
		    ఆపు;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_సందేశం);
		ఒకవేళ (var == ఖాళీ)
		{
		    err = TCL_ERROR;
		    ఆపు;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		ఒకవేళ (err != TCL_OK)
		    ఆపు;
	    }
	    లేకపోతే
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_బఫరు);
		ఒకవేళ (err != TCL_OK)
		    ఆపు;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		ఒకవేళ (err != TCL_OK)
		    ఆపు;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.నిలువు = col2vim(val2);
	    win->w_సమితి_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    ఆపు;

	అప్రమేయ:
	    Tcl_సమితి_ఫలితం(interp, _("not implemented yet"), TCL_STATIC);
	    ఆపు;
    }

    curwin = savewin;
    curbuf = savebuf;
    ఒకవేళ (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    తిరిగి err;
}


    స్థిర పూర్ణాంకం
commandcmd(
    Client_దత్తాంశం	dummy వాడని,
    Tcl_Interp	*interp,
    పూర్ణాంకం		objc,
    Tcl_Obj	*CONST objv[])
{
    పూర్ణాంకం		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    తిరిగి err;
}

    స్థిర పూర్ణాంకం
optioncmd(
    Client_దత్తాంశం	dummy వాడని,
    Tcl_Interp	*interp,
    పూర్ణాంకం		objc,
    Tcl_Obj	*CONST objv[])
{
    పూర్ణాంకం		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    తిరిగి err;
}

    స్థిర పూర్ణాంకం
exprcmd(
    Client_దత్తాంశం	dummy వాడని,
    Tcl_Interp	*interp,
    పూర్ణాంకం		objc,
    Tcl_Obj	*CONST objv[])
{
    తిరిగి tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    స్థిర పూర్ణాంకం
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    పూర్ణాంకం		*విలువ_P,
    buf_T	*buf)
{
    పూర్ణాంకం err, i;

    గణన { LN_BEGIN, LN_BOTTOM, LN_చివర, LN_మొదటి, LN_చివరి, LN_ప్రారంభం, LN_శిఖరం };

    స్థిర CONST84 అక్షరం *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (అక్షరం *)0
    };

    err = Tcl_Get_సూచీ_From_Obj(interp, obj, keyw, "", 0, &i);
    ఒకవేళ (err == TCL_OK)
    {
	ఎంపిక (i)
	{
	    స్థితి LN_BEGIN:
	    స్థితి LN_మొదటి:
	    స్థితి LN_ప్రారంభం:
	    స్థితి LN_శిఖరం:
		*విలువ_P = 1;
		ఆపు;
	    స్థితి LN_BOTTOM:
	    స్థితి LN_చివర:
	    స్థితి LN_చివరి:
		*విలువ_P = buf->b_ml.ml_పంక్తి_లెక్క;
		ఆపు;
	}
	తిరిగి TCL_OK;
    }
    Tcl_Reset_ఫలితం(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    ఒకవేళ (err != TCL_OK)
	తిరిగి err;
    i = row2vim(i);
    ఒకవేళ (i < 1  ||  i > buf->b_ml.ml_పంక్తి_లెక్క)
    {
	Tcl_సమితి_ఫలితం(interp, _("line number out of range"), TCL_STATIC);
	తిరిగి TCL_ERROR;
    }
    *విలువ_P = i;
    తిరిగి TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    స్థిర win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	ఒకవేళ (win->w_బఫరు == buf)
	    తిరిగి win;
    }
    తిరిగి curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    స్థిర పూర్ణాంకం
tcldoexcommand(
    Tcl_Interp	*interp,
    పూర్ణాంకం		objc,
    Tcl_Obj	*CONST objv[],
    పూర్ణాంకం		objn)
{
    tcl_info	saveinfo;
    పూర్ణాంకం		err, flag, nobjs;
    అక్షరం	*arg;

    nobjs = objc - objn;
    ఒకవేళ (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_అంకె_వాదనలు(interp, objn, objv, "?-quiet? exCommand");
	తిరిగి TCL_ERROR;
    }

    flag = 0;
    ఒకవేళ (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], ఖాళీ);
	ఒకవేళ (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	లేకపోతే
	{
	    Tcl_Reset_ఫలితం(interp);
	    Tcl_Append_ఫలితం(interp, _("unknown flag: "), arg, (అక్షరం *)0);
	    తిరిగి TCL_ERROR;
	}
	++objn;
    }

    జ్ఞాపకం_నకలు(&saveinfo, &tclinfo, పరిమాణం(tcl_info));
    tclinfo.interp = ఖాళీ;
    tclinfo.curwin = ఖాళీ;
    tclinfo.curbuf = ఖాళీ;

    arg = Tcl_GetStringFromObj(objv[objn], ఖాళీ);
    ఒకవేళ (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    ఒకవేళ (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    ఒకవేళ (tclinfo.interp)
	tcldelthisinterp();
    జ్ఞాపకం_నకలు(&tclinfo, &saveinfo, పరిమాణం(tcl_info));
    tclupdatevars();

    తిరిగి err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    స్థిర పూర్ణాంకం
tclsetoption(
    Tcl_Interp	*interp,
    పూర్ణాంకం		objc,
    Tcl_Obj	*CONST objv[],
    పూర్ణాంకం		objn)
{
    పూర్ణాంకం		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    దీర్ఘ	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    గణన { OPT_OFF, OPT_ON, OPT_TOGGLE };
    స్థిర CONST84 అక్షరం *optkw[] = { "off", "on", "toggle", (అక్షరం *)0 };

    nobjs = objc - objn;
    ఒకవేళ (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_అంకె_వాదనలు(interp, objn, objv, "vimOption ?value?");
	తిరిగి TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], ఖాళీ);
    ++objn;
    gov = get_option_విలువ(option, &lval, &sval, ఖాళీ, 0);
    err = TCL_OK;
    ఎంపిక (gov)
    {
	స్థితి gov_string:
	    Tcl_సమితి_ఫలితం(interp, (అక్షరం *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    ఆపు;
	స్థితి gov_bool:
	స్థితి gov_సంఖ్య:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_సమితి_Obj_ఫలితం(interp, resobj);
	    ఆపు;
	అప్రమేయ:
	    Tcl_సమితి_ఫలితం(interp, _("unknown vimOption"), TCL_STATIC);
	    తిరిగి TCL_ERROR;
    }
    ఒకవేళ (nobjs == 2)
    {
	ఒకవేళ (gov != gov_string)
	{
	    sval = ఖాళీ;    // avoid compiler warning
	    err = Tcl_Get_సూచీ_From_Obj(interp, objv[objn], optkw, "", 0, &idx);
	    ఒకవేళ (err != TCL_OK)
	    {
		Tcl_Reset_ఫలితం(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    లేకపోతే
	    {
		ఎంపిక (idx)
		{
		    స్థితి OPT_ON:
			lval = 1;
			ఆపు;
		    స్థితి OPT_OFF:
			lval = 0;
			ఆపు;
		    స్థితి OPT_TOGGLE:
			lval = !lval;
			ఆపు;
		}
	    }
	}
	లేకపోతే
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], ఖాళీ);
	ఒకవేళ (err == TCL_OK)
	{
	    సమితి_option_విలువ_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    తిరిగి err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    స్థిర పూర్ణాంకం
tclvimexpr(
    Tcl_Interp	*interp,
    పూర్ణాంకం		objc,
    Tcl_Obj	*CONST objv[],
    పూర్ణాంకం		objn)
{
#నిర్వచితమైతే FEAT_EVAL
    అక్షరం	*సమాసం, *str;
#అయితే_ముగింపు
    పూర్ణాంకం		err;

    ఒకవేళ (objc - objn != 1)
    {
	Tcl_Wrong_అంకె_వాదనలు(interp, objn, objv, "vimExpr");
	తిరిగి TCL_ERROR;
    }

#నిర్వచితమైతే FEAT_EVAL
    సమాసం = Tcl_GetStringFromObj(objv[objn], ఖాళీ);
    str = (అక్షరం *)eval_గమ్యం_string((char_u *)సమాసం, TRUE, FALSE);
    ఒకవేళ (str == ఖాళీ)
	Tcl_సమితి_ఫలితం(interp, _("invalid expression"), TCL_STATIC);
    లేకపోతే
    {
	Tcl_సమితి_ఫలితం(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#లేకపోతే
    Tcl_సమితి_ఫలితం(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#అయితే_ముగింపు

    తిరిగి err;
}

/*
 * Check for internal vim errors.
 */
    స్థిర పూర్ణాంకం
vimerror(Tcl_Interp *interp)
{
    ఒకవేళ (got_int)
    {
	Tcl_సమితి_ఫలితం(interp, _("keyboard interrupt"), TCL_STATIC);
	తిరిగి TCL_ERROR;
    }
    లేకపోతే ఒకవేళ (did_emsg)
    {
	Tcl_సమితి_ఫలితం(interp, _("Vim error"), TCL_STATIC);
	తిరిగి TCL_ERROR;
    }
    తిరిగి TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    స్థిర శూన్య
delref(Client_దత్తాంశం cref)
{
    నిర్మాణం ref *ref = (నిర్మాణం ref *)cref;

    ఒకవేళ (ref->delcmd)
    {
	Tcl_Decr_Ref_లెక్క(ref->delcmd);
	ref->delcmd = ఖాళీ;
    }
    ref->interp = ఖాళీ;
}

    స్థిర అక్షరం *
tclgetref(
    Tcl_Interp	*interp,
    శూన్య	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    అక్షరం	*ఉపసర్గ,	// "win" or "buf"
    శూన్య	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    నిర్మాణం ref *ref, *వాడని_2 = ఖాళీ;
    స్థిర అక్షరం name[VARNAME_కొలత];
    Tcl_Command cmd;

    ref = (నిర్మాణం ref *)(*refstartP);
    ఒకవేళ (ref == &refsdeleted)
    {
	Tcl_సమితి_ఫలితం(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	తిరిగి ఖాళీ;
    }

    అయితే (ref != ఖాళీ)
    {
	ఒకవేళ (ref->interp == interp)
	    ఆపు;
	ఒకవేళ (ref->interp == ఖాళీ)
	    వాడని_2 = ref;
	ref = ref->తదుపరి;
    }

    ఒకవేళ (ref)
	vim_snprintf(name, పరిమాణం(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    లేకపోతే
    {
	ఒకవేళ (వాడని_2)
	    ref = వాడని_2;
	లేకపోతే
	{
	    ref = (నిర్మాణం ref *)Tcl_Alloc(పరిమాణం(నిర్మాణం ref));
	    ref->interp = ఖాళీ;
	    ref->తదుపరి = (నిర్మాణం ref *)(*refstartP);
	    (*refstartP) = (శూన్య *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, పరిమాణం(name), "::vim::%s_%lx",
					       ఉపసర్గ, (గుర్తులేని దీర్ఘ)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (Client_దత్తాంశం)ref, (Tcl_CmdDeleteProc *)delref);
	ఒకవేళ (!cmd)
	    తిరిగి ఖాళీ;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = ఖాళీ;
	ref->vimobj = vimobj;
    }
    తిరిగి name;
}

    స్థిర అక్షరం *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    తిరిగి tclgetref(interp, &(win->w_tcl_ref), "win", (శూన్య *)win, winselfcmd);
}

    స్థిర అక్షరం *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    తిరిగి tclgetref(interp, &(buf->b_tcl_ref), "buf", (శూన్య *)buf, bufselfcmd);
}

    స్థిర పూర్ణాంకం
tclsetdelcmd(
    Tcl_Interp	*interp,
    నిర్మాణం ref	*reflist,
    శూన్య	*vimobj,
    Tcl_Obj	*delcmd)
{
    ఒకవేళ (reflist == &refsdeleted)
    {
	Tcl_సమితి_ఫలితం(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	తిరిగి TCL_ERROR;
    }

    అయితే (reflist != ఖాళీ)
    {
	ఒకవేళ (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    ఒకవేళ (reflist->delcmd)
		Tcl_Decr_Ref_లెక్క(reflist->delcmd);
	    Tcl_Incr_Ref_లెక్క(delcmd);
	    reflist->delcmd = delcmd;
	    తిరిగి TCL_OK;
	}
	reflist = reflist->తదుపరి;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_సమితి_ఫలితం(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    తిరిగి TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    స్థిర పూర్ణాంకం
tcl_channel_close(Client_దత్తాంశం instance, Tcl_Interp *interp వాడని)
{
    పూర్ణాంకం		err = 0;

    // currently does nothing

    ఒకవేళ (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_సమితి_Errno(EBADF);
	err = EBADF;
    }
    తిరిగి err;
}

    స్థిర పూర్ణాంకం
tcl_channel_ఆదానం(
    Client_దత్తాంశం	instance వాడని,
    అక్షరం	*buf వాడని,
    పూర్ణాంకం		bufsiz వాడని,
    పూర్ణాంకం		*errptr)
{

    // input is currently not supported

    Tcl_సమితి_Errno(EINVAL);
    ఒకవేళ (errptr)
	*errptr = EINVAL;
    తిరిగి -1;
}

    స్థిర పూర్ణాంకం
tcl_channel_output(
    Client_దత్తాంశం	instance,
    స్థిరరాశి అక్షరం	*buf,
    పూర్ణాంకం		bufsiz,
    పూర్ణాంకం		*errptr)
{
    char_u	*str;
    పూర్ణాంకం		ఫలితం;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    ఒకవేళ (!str)
    {
	Tcl_సమితి_Errno(ENOMEM);
	ఒకవేళ (errptr)
	    *errptr = ENOMEM;
	తిరిగి -1;
    }

    ఫలితం = bufsiz;
    ఒకవేళ (instance == VIMOUT)
	tclmsg((అక్షరం *)str);
    లేకపోతే
    ఒకవేళ (instance == VIMERR)
	tclerrmsg((అక్షరం *)str);
    లేకపోతే
    {
	Tcl_సమితి_Errno(EBADF);
	ఒకవేళ (errptr)
	    *errptr = EBADF;
	ఫలితం = -1;
    }
    vim_free(str);
    తిరిగి ఫలితం;
}

    స్థిర శూన్య
tcl_channel_watch(Client_దత్తాంశం instance వాడని, పూర్ణాంకం ముసుగు వాడని)
{
    Tcl_సమితి_Errno(EINVAL);
}

    స్థిర పూర్ణాంకం
tcl_channel_gethandle(
    Client_దత్తాంశం	instance వాడని,
    పూర్ణాంకం		direction వాడని,
    Client_దత్తాంశం	*handleptr వాడని)
{
    Tcl_సమితి_Errno(EINVAL);
    తిరిగి EINVAL;
}


స్థిర Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_సంస్కరణ_2, //సంస్కరణ
    tcl_channel_close,	// closeProc
    tcl_channel_ఆదానం,	// inputProc
    tcl_channel_output,	// outputProc
    ఖాళీ,		// seekProc
    ఖాళీ,		// setOptionProc
    ఖాళీ,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    ఖాళీ,		// close2Proc
    ఖాళీ,		// blockModeProc
#నిర్వచితమైతే TCL_CHANNEL_సంస్కరణ_2
    ఖాళీ,		// flushProc
    ఖాళీ,		// handlerProc
#అయితే_ముగింపు
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#నిర్వచితమైతే TCL_CHANNEL_సంస్కరణ_3
    ఖాళీ,		// wideSeekProc
#అయితే_ముగింపు
#నిర్వచితమైతే TCL_CHANNEL_సంస్కరణ_4
    ఖాళీ,		// threadActionProc
#అయితే_ముగింపు
#నిర్వచితమైతే TCL_CHANNEL_సంస్కరణ_5
    ఖాళీ		// truncateProc
#అయితే_ముగింపు
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    స్థిర శూన్య
tclupdatevars(శూన్య)
{
    అక్షరం varname[VARNAME_కొలత];	// must be writeable
    అక్షరం *name;

    strcpy(varname, VAR_RANGE1);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);
    strcpy(varname, VAR_RANGE2);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);
    strcpy(varname, VAR_RANGE3);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);

    strcpy(varname, VAR_LBASE);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);

    name = tclgetbuffer(tclinfo.interp, curbuf);
    strcpy(tclinfo.curbuf, name);
    strcpy(varname, VAR_CURBUF);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);

    name = tclgetwindow(tclinfo.interp, curwin);
    strcpy(tclinfo.curwin, name);
    strcpy(varname, VAR_CURWIN);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);
}


    స్థిర పూర్ణాంకం
tclinit(exarg_T *eap)
{
    అక్షరం varname[VARNAME_కొలత];	// Tcl_LinkVar requires writeable varname
    అక్షరం *name;

#నిర్వచితమైతే DYNAMIC_TCL
    ఒకవేళ (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	తిరిగి FAIL;
    }
#అయితే_ముగింపు

    ఒకవేళ (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	స్థిర Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_సమితి_Std_Channel(ch1, TCL_STDOUT);
	Tcl_సమితి_Std_Channel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	ఒకవేళ (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    తిరిగి FAIL;
	}
#ఒకవేళ 0
	// VIM sure is interactive
	Tcl_సమితి_Var(interp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
#అయితే_ముగింపు

	Tcl_సమితి_Channel_Option(interp, ch1, "-buffering", "line");
#నిర్వచితమైతే MSWIN
	Tcl_సమితి_Channel_Option(interp, ch1, "-translation", "lf");
#అయితే_ముగింపు
	Tcl_సమితి_Channel_Option(interp, ch2, "-buffering", "line");
#నిర్వచితమైతే MSWIN
	Tcl_సమితి_Channel_Option(interp, ch2, "-translation", "lf");
#అయితే_ముగింపు

	// replace standard Tcl exit command
	Tcl_DeleteCommand(interp, "exit");
	Tcl_CreateObjCommand(interp, "exit", exitcmd,
	    (Client_దత్తాంశం)ఖాళీ, (Tcl_CmdDeleteProc *)ఖాళీ);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (Client_దత్తాంశం)ఖాళీ, (Tcl_CmdDeleteProc *)ఖాళీ);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (Client_దత్తాంశం)ఖాళీ, (Tcl_CmdDeleteProc *)ఖాళీ);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (Client_దత్తాంశం)ఖాళీ, (Tcl_CmdDeleteProc *)ఖాళీ);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (Client_దత్తాంశం)ఖాళీ, (Tcl_CmdDeleteProc *)ఖాళీ);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (Client_దత్తాంశం)ఖాళీ, (Tcl_CmdDeleteProc *)ఖాళీ);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (Client_దత్తాంశం)ఖాళీ, (Tcl_CmdDeleteProc *)ఖాళీ);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (అక్షరం *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_ప్రారంభం = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (అక్షరం *)&tclinfo.range_ప్రారంభం, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (అక్షరం *)&tclinfo.range_ప్రారంభం, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_చివర   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (అక్షరం *)&tclinfo.range_చివర, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_కొలత);
	tclinfo.curwin = Tcl_Alloc(VARNAME_కొలత);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (అక్షరం *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (అక్షరం *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    లేకపోతే
    {
	// Interpreter already exists, just update variables
	tclinfo.range_ప్రారంభం = row2tcl(eap->line1);
	tclinfo.range_చివర = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    తిరిగి OK;
}

    స్థిర శూన్య
tclerrmsg(అక్షరం *పాఠ్యం)
{
    అక్షరం *తదుపరి;

    అయితే ((తదుపరి=strchr(పాఠ్యం, '\n')))
    {
	*తదుపరి++ = '\0';
	emsg(పాఠ్యం);
	పాఠ్యం = తదుపరి;
    }
    ఒకవేళ (*పాఠ్యం)
	emsg(పాఠ్యం);
}

    స్థిర శూన్య
tclmsg(అక్షరం *పాఠ్యం)
{
    అక్షరం *తదుపరి;

    అయితే ((తదుపరి=strchr(పాఠ్యం, '\n')))
    {
	*తదుపరి++ = '\0';
	సందేశం(పాఠ్యం);
	పాఠ్యం = తదుపరి;
    }
    ఒకవేళ (*పాఠ్యం)
	సందేశం(పాఠ్యం);
}

    స్థిర శూన్య
tcldelthisinterp(శూన్య)
{
    ఒకవేళ (!Tcl_InterpDeleted(tclinfo.interp))
	Tcl_DeleteInterp(tclinfo.interp);
    Tcl_Release(tclinfo.interp);
    // The interpreter is now gets deleted.  All registered commands (esp.
    // window and buffer commands) are deleted, triggering their deletion
    // callback, which deletes all refs pointing to this interpreter.
    // We could garbage-collect the unused ref structs in all windows and
    // buffers, but unless the user creates hundreds of sub-interpreters
    // all referring to lots of windows and buffers, this is hardly worth
    // the effort.  Unused refs are recycled by other interpreters, and
    // all refs are free'd when the window/buffer gets closed by vim.

    tclinfo.interp = ఖాళీ;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = ఖాళీ;
}

    స్థిర పూర్ణాంకం
tclexit(పూర్ణాంకం error)
{
    పూర్ణాంకం newerr = OK;

    ఒకవేళ (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#ఒకవేళ (TCL_MAJOR_సంస్కరణ == 8 && TCL_MINOR_సంస్కరణ >= 5) || TCL_MAJOR_సంస్కరణ > 8
	|| Tcl_హద్దు_Exceeded(tclinfo.interp)  // True if the interpreter cannot continue
#అయితే_ముగింపు
	)
    {
	అక్షరం buf[50];

	sprintf(buf, _(e_exit_సంకేతం_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	ఒకవేళ (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	లేకపోతే
	    newerr = FAIL;

	tcldelthisinterp();
    }
    లేకపోతే
    {
	అక్షరం *ఫలితం;

	ఫలితం = (అక్షరం *)Tcl_Get_String_ఫలితం(tclinfo.interp);
	ఒకవేళ (error == TCL_OK)
	{
	    tclmsg(ఫలితం);
	    newerr = OK;
	}
	లేకపోతే
	{
	    tclerrmsg(ఫలితం);
	    newerr = FAIL;
	}
    }

    తిరిగి newerr;
}

/*
 * ":tcl"
 */
    శూన్య
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    పూర్ణాంకం		err;

    script = script_get(eap, eap->arg);
    ఒకవేళ (!eap->skip)
    {
	err = tclinit(eap);
	ఒకవేళ (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    ఒకవేళ (script == ఖాళీ)
		err = Tcl_Eval(tclinfo.interp, (అక్షరం *)eap->arg);
	    లేకపోతే
		err = Tcl_Eval(tclinfo.interp, (అక్షరం *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    శూన్య
ex_tclfile(exarg_T *eap)
{
    అక్షరం *file = (అక్షరం *)eap->arg;
    పూర్ణాంకం err;

    err = tclinit(eap);
    ఒకవేళ (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    శూన్య
ex_tcldo(exarg_T *eap)
{
    అక్షరం	*script, *పంక్తి;
    పూర్ణాంకం		err, rs, re, lnum;
    అక్షరం	var_lnum[VARNAME_కొలత]; // must be writeable memory
    అక్షరం	var_పంక్తి[VARNAME_కొలత];
    linenr_T	మొదటి_పంక్తి = 0;
    linenr_T	చివరి_పంక్తి = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (అక్షరం *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_పంక్తి, VAR_CURLINE);

    err = tclinit(eap);
    ఒకవేళ (err != OK)
	తిరిగి;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (అక్షరం *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    ఒకవేళ (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_సమితి_ఫలితం(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    అయితే (err == TCL_OK  &&  rs <= re)
    {
	ఒకవేళ ((linenr_T)rs > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    ఆపు;
	పంక్తి = (అక్షరం *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	ఒకవేళ (!పంక్తి)
	{
	    Tcl_సమితి_ఫలితం(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    ఆపు;
	}
	Tcl_సమితి_Var(tclinfo.interp, var_పంక్తి, పంక్తి, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	ఒకవేళ (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#ఒకవేళ (TCL_MAJOR_సంస్కరణ == 8 && TCL_MINOR_సంస్కరణ >= 5) || TCL_MAJOR_సంస్కరణ > 8
	    || Tcl_హద్దు_Exceeded(tclinfo.interp)
#అయితే_ముగింపు
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    ఆపు;
	పంక్తి = (అక్షరం *)Tcl_GetVar(tclinfo.interp, var_పంక్తి, 0);
	ఒకవేళ (పంక్తి)
	{
	    ఒకవేళ (ml_replace((linenr_T)rs, (char_u *)పంక్తి, TRUE) != OK)
	    {
		Tcl_సమితి_ఫలితం(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		ఆపు;
	    }
	    ఒకవేళ (మొదటి_పంక్తి == 0)
		మొదటి_పంక్తి = rs;
	    చివరి_పంక్తి = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    ఒకవేళ (మొదటి_పంక్తి)
	మారింది_పంక్తులు(మొదటి_పంక్తి, 0, చివరి_పంక్తి + 1, (దీర్ఘ)0);

    Tcl_UnsetVar(tclinfo.interp, var_పంక్తి, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    ఒకవేళ (err == TCL_OK)
	Tcl_Reset_ఫలితం(tclinfo.interp);

    (శూన్య)tclexit(err);
}

    స్థిర శూన్య
tcldelallrefs(నిర్మాణం ref *ref)
{
    నిర్మాణం ref	*తదుపరి;
    పూర్ణాంకం		err;
    అక్షరం	*ఫలితం;

#నిర్వచితమైతే DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    ఒకవేళ (exiting)
	తిరిగి;
#అయితే_ముగింపు

    అయితే (ref != ఖాళీ)
    {
	తదుపరి = ref->తదుపరి;
	ఒకవేళ (ref->interp)
	{
	    ఒకవేళ (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		ఒకవేళ (err != TCL_OK)
		{
		    ఫలితం = (అక్షరం *)Tcl_Get_String_ఫలితం(ref->interp);
		    ఒకవేళ (ఫలితం)
			tclerrmsg(ఫలితం);
		}
		Tcl_Decr_Ref_లెక్క(ref->delcmd);
		ref->delcmd = ఖాళీ;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((అక్షరం *)ref);
	ref = తదుపరి;
    }
}

    శూన్య
tcl_బఫరు_free(buf_T *buf)
{
    నిర్మాణం ref *reflist;

#నిర్వచితమైతే DYNAMIC_TCL
    ఒకవేళ (!stubs_initialized)	// Not using Tcl, nothing to do.
	తిరిగి;
#అయితే_ముగింపు

    reflist = (నిర్మాణం ref *)(buf->b_tcl_ref);
    ఒకవేళ (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (శూన్య *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = ఖాళీ;
    }
}

    శూన్య
tcl_కిటికీ_free(win_T *win)
{
    నిర్మాణం ref *reflist;

#నిర్వచితమైతే DYNAMIC_TCL
    ఒకవేళ (!stubs_initialized)	// Not using Tcl, nothing to do.
	తిరిగి;
#అయితే_ముగింపు

    reflist = (నిర్మాణం ref*)(win->w_tcl_ref);
    ఒకవేళ (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (శూన్య *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = ఖాళీ;
    }
}

// The End
