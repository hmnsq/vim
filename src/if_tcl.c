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

#अन्तर्भाव "vim.h"
#undef EXTERN			// tcl.h defines it too

#ifdef DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#endif

#अन्तर्भाव <tcl.h>
#अन्तर्भाव <string.h>

प्रकारव्याख्या संरचना
{
    Tcl_Interp *interp;
    पूर्णांक exitvalue;
    पूर्णांक range_start, range_end;
    पूर्णांक lbase;
    अक्षर *curbuf, *curwin;
} tcl_info;

स्थिर tcl_info tclinfo = { रिक्त, 0, 0, 0, 0, रिक्त, रिक्त };

#define VAR_RANGE1	"::vim::range(start)"
#define VAR_RANGE2	"::vim::range(begin)"
#define VAR_RANGE3	"::vim::range(end)"
#define VAR_CURBUF	"::vim::current(buffer)"
#define VAR_CURWIN	"::vim::current(window)"
#define VAR_LBASE	"::vim::lbase"
#define VAR_CURLINE	"line"
#define VAR_CURLNUM	"lnum"
#define VARNAME_SIZE	64

#define row2tcl(x)  ((x) - (tclinfo.lbase==0))
#define row2vim(x)  ((x) + (tclinfo.lbase==0))
#define col2tcl(x)  ((x) + (tclinfo.lbase!=0))
#define col2vim(x)  ((x) - (tclinfo.lbase!=0))


#define VIMOUT	((ClientData)1)
#define VIMERR	((ClientData)2)

// This appears to be new in Tcl 8.4.
#ifndef CONST84
# define CONST84
#endif

/*
 *  List of Tcl interpreters who reference a vim window or buffer.
 *  Each buffer and window has its own list in the w_tcl_ref or b_tcl_ref
 *  struct member.  We need this because Tcl can create sub-interpreters with
 *  the "interp" command, and each interpreter can reference all windows and
 *  buffers.
 */
संरचना ref
{
    संरचना ref	*next;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    शून्य	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
स्थिर अक्षर * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
स्थिर अक्षर * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
स्थिर पूर्णांक tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, संरचना ref *reflist, शून्य *vimobj, Tcl_Obj *delcmd));
स्थिर पूर्णांक tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, पूर्णांक *मूल्य_P, buf_T *buf));
स्थिर win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
स्थिर पूर्णांक tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, पूर्णांक objc, Tcl_Obj *CONST objv[], पूर्णांक objn));
स्थिर पूर्णांक tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, पूर्णांक objc, Tcl_Obj *CONST objv[], पूर्णांक objn));
स्थिर पूर्णांक tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, पूर्णांक objc, Tcl_Obj *CONST objv[], पूर्णांक objn));
स्थिर शून्य tcldelthisinterp _ANSI_ARGS_ ((शून्य));

स्थिर पूर्णांक vimerror _ANSI_ARGS_((Tcl_Interp *interp));
स्थिर शून्य tclmsg _ANSI_ARGS_((अक्षर *text));
स्थिर शून्य tclerrmsg _ANSI_ARGS_((अक्षर *text));
स्थिर शून्य tclupdatevars _ANSI_ARGS_((शून्य));

स्थिर संरचना ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#जर defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
प्रकारव्याख्या पूर्णांक HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define चिन्ह_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# अन्यथा
#  include <dlfcn.h>
#  define HANDLE शून्य*
#  define TCL_PROC शून्य*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define चिन्ह_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
स्थिर HANDLE hTclLib = रिक्त;
Tcl_Interp* (*dll_Tcl_CreateInterp)(शून्य);
शून्य (*dll_Tcl_FindExecutable)(स्थिरांक शून्य *);

/*
 * Table of name to function pointer of tcl.
 */
स्थिर संरचना {
    अक्षर* name;
    TCL_PROC* ptr;
} tcl_funcname_table[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_FindExecutable},
    {रिक्त, रिक्त},
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
    स्थिर पूर्णांक
tcl_runtime_link_init(अक्षर *libname, पूर्णांक verbose)
{
    पूर्णांक i;

    जर (hTclLib)
	परत OK;
    जर (!(hTclLib = load_dll(libname)))
    {
	जर (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	परत FAIL;
    }
    साठी (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	जर (!(*tcl_funcname_table[i].ptr = चिन्ह_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = रिक्त;
	    जर (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    परत FAIL;
	}
    }
    परत OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
स्थिर अक्षर *find_executable_arg = रिक्त;
#endif

    शून्य
vim_tcl_init(अक्षर *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#अन्यथा
    Tcl_FindExecutable(arg);
#endif
}

#जर defined(DYNAMIC_TCL)

स्थिर पूर्णांक stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    पूर्णांक
tcl_enabled(पूर्णांक verbose)
{
    जर (!stubs_initialized && find_executable_arg != रिक्त
	    && tcl_runtime_link_init((अक्षर *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	जर ((interp = dll_Tcl_CreateInterp()) != रिक्त)
	{
	    जर (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != रिक्त)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    परत stubs_initialized;
}
#endif

#जर defined(EXITFREE)
/*
 * Called once when exiting.
 */
    शून्य
vim_tcl_finalize(शून्य)
{
# ifdef DYNAMIC_TCL
    जर (stubs_initialized)
# endif
	Tcl_Finalize();
}
#endif

    शून्य
tcl_end(शून्य)
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
    स्थिर पूर्णांक
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    पूर्णांक objc,
    Tcl_Obj *CONST objv[])
{
    पूर्णांक मूल्य = 0;

    निवड (objc)
    {
	स्थिती 2:
	    जर (Tcl_GetIntFromObj(interp, objv[1], &मूल्य) != TCL_OK)
		थांबा;
	    // FALLTHROUGH
	स्थिती 1:
	    tclinfo.exitvalue = मूल्य;

	    Tcl_DeleteInterp(interp);
	    थांबा;
	पूर्वनिर्धारित:
	    Tcl_Wrong_अंक_Args(interp, 1, objv, "?returnCode?");
    }
    परत TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    स्थिर पूर्णांक
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    पूर्णांक objc,
    Tcl_Obj *CONST objv[])
{
    जर (objc != 1)
    {
	Tcl_Wrong_अंक_Args(interp, 1, objv, रिक्त);
	परत TCL_ERROR;
    }
    vim_beep(BO_LANG);
    परत TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    स्थिर पूर्णांक
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    पूर्णांक objc,
    Tcl_Obj *CONST objv[])
{
    अक्षर	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    पूर्णांक		err, n, idx;
    गणना {BCMD_EXISTS, BCMD_LIST};
    स्थिर CONST84 अक्षर *bcmdoptions[] =
    {
	"exists", "list", (अक्षर *)0
    };

    जर (objc < 2)
    {
	Tcl_Wrong_अंक_Args(interp, 1, objv, "option");
	परत TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    जर (err == TCL_OK)
    {
	जर (objc != 2)
	{
	    Tcl_Wrong_अंक_Args(interp, 1, objv, "bufNumber");
	    परत TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    जर (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		जर (name == रिक्त)
		    परत TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		परत TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	परत TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    जर (err != TCL_OK)
	परत err;
    निवड (idx)
    {
	स्थिती BCMD_LIST:
	    जर (objc != 2)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, "");
		err = TCL_ERROR;
		थांबा;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		जर (name == रिक्त)
		{
		    err = TCL_ERROR;
		    थांबा;
		}
		Tcl_AppendElement(interp, name);
	    }
	    थांबा;

	स्थिती BCMD_EXISTS:
	    जर (objc != 3)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		थांबा;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    जर (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != रिक्त);
		Tcl_SetObjResult(interp, resobj);
	    }
	    थांबा;

	पूर्वनिर्धारित:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    परत err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    स्थिर पूर्णांक
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    पूर्णांक		objc,
    Tcl_Obj	*CONST objv[])
{
    अक्षर	*what, *string;
    win_T	*win;

    जर (objc != 2)
    {
	Tcl_Wrong_अंक_Args(interp, 1, objv, "option");
	परत TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], रिक्त);
    जर (म_भेद(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    जर (string == रिक्त)
		परत TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	परत TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    परत TCL_ERROR;
}

/*
 * flags for bufselfcmd and winselfcmd to indicate outstanding actions.
 */
#define FL_UPDATE_SCREEN	(1<<0)
#define FL_UPDATE_CURBUF	(1<<1)
#define FL_ADJUST_CURSOR	(1<<2)

/*
 * This function implements the buffer commands.
 */
    स्थिर पूर्णांक
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    पूर्णांक		objc,
    Tcl_Obj	*CONST objv[])
{
    पूर्णांक		opt, err, idx, flags;
    पूर्णांक		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    अक्षर	*ओळ;

    गणना
    {
	BUF_APPEND, BUF_COMMAND, BUF_मोजणी, BUF_DELCMD, BUF_DELETE, BUF_अभिव्यक्ती,
	BUF_GET, BUF_घाला, BUF_LAST, BUF_MARK, BUF_NAME, BUF_संख्या,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    स्थिर CONST84 अक्षर *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (अक्षर *)0
    };

    जर (objc < 2)
    {
	Tcl_Wrong_अंक_Args(interp, 1, objv, "option ?arg ...?");
	परत TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    जर (err != TCL_OK)
	परत err;

    buf = (buf_T *)((संरचना ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    निवड (idx)
    {
	स्थिती BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    थांबा;

	स्थिती BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    थांबा;

	स्थिती BUF_अभिव्यक्ती:
	    err = tclvimexpr(interp, objc, objv, 2);
	    थांबा;

	स्थिती BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    जर (objc != 2)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, रिक्त);
		err = TCL_ERROR;
		थांबा;
	    }
	    जर (buf->b_ffname)
		Tcl_SetResult(interp, (अक्षर *)buf->b_ffname, TCL_VOLATILE);
	    अन्यथा
		Tcl_SetResult(interp, "", TCL_STATIC);
	    थांबा;

	स्थिती BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	स्थिती BUF_मोजणी:
	    /*
	     * Get number of lines in buffer.
	     */
	    जर (objc != 2)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, रिक्त);
		err = TCL_ERROR;
		थांबा;
	    }
	    val1 = (पूर्णांक)buf->b_ml.ml_ओळ_मोजणी;
	    जर (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    थांबा;

	स्थिती BUF_संख्या:
	    /*
	     * Get buffer's number.
	     */
	    जर (objc != 2)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, रिक्त);
		err = TCL_ERROR;
		थांबा;
	    }
	    resobj = Tcl_NewIntObj((पूर्णांक)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    थांबा;

	स्थिती BUF_GET:
	    जर (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		थांबा;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    जर (err != TCL_OK)
		थांबा;
	    जर (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		जर (err != TCL_OK)
		    थांबा;
		जर (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		साठी (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    ओळ = (अक्षर *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    जर (ओळ)
			Tcl_AppendElement(interp, ओळ);
		    अन्यथा
			err = TCL_ERROR;
		}
	    }
	    अन्यथा
	    {  // objc == 3
		ओळ = (अक्षर *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, ओळ, TCL_VOLATILE);
	    }
	    थांबा;

	स्थिती BUF_SET:
	    जर (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_अंक_Args(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		थांबा;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    जर (err != TCL_OK)
		परत TCL_ERROR;
	    जर (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		ओळ = Tcl_GetStringFromObj(objv[3], रिक्त);
		जर (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		अन्यथा
		जर (ml_replace((linenr_T)val1, (char_u *)ओळ, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		अन्यथा
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		थांबा;
	    }
	    अन्यथा
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		पूर्णांक	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		जर (err != TCL_OK)
		    थांबा;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		जर (err != TCL_OK)
		    थांबा;
		जर (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		जर (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    थांबा;
		}
		flags |= FL_UPDATE_CURBUF;

		साठी (i = 0; i < lc && n <= val2; i++)
		{
		    ओळ = Tcl_GetStringFromObj(lv[i], रिक्त);
		    जर (ml_replace((linenr_T)n, (char_u *)ओळ, TRUE) != OK)
			जा setListError;
		    ++n;
		}
		जर (i < lc)
		{
		    // append lines
		    करा
		    {
			ओळ = Tcl_GetStringFromObj(lv[i], रिक्त);
			जर (ml_append((linenr_T)(n - 1),
					      (char_u *)ओळ, 0, FALSE) != OK)
			    जा setListError;
			++n;
			++i;
		    } पर्यंत (i < lc);
		}
		अन्यथा जर (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    करा
		    {
			जर (ml_delete((linenr_T)i) != OK)
			    जा setListError;
			++n;
		    } पर्यंत (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (दीर्घ)MAXLNUM,
								    (दीर्घ)lc);
		changed_ओळी((linenr_T)val1, 0, (linenr_T)val2 + 1, (दीर्घ)lc);
		थांबा;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    थांबा;

	स्थिती BUF_DELETE:
	    जर (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_अंक_Args(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		थांबा;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    जर (err != TCL_OK)
		थांबा;
	    val2 = val1;
	    जर (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		जर (err != TCL_OK)
		    परत err;
		जर (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    जर (u_savedel((linenr_T)val1, (दीर्घ)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		थांबा;
	    }
	    साठी (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		जर (err != TCL_OK)
		    थांबा;
	    }
	    जर (i > 0)
		deleted_ओळी_mark((linenr_T)val1, (दीर्घ)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    थांबा;

	स्थिती BUF_MARK:
	    जर (objc != 3)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, "markName");
		err = TCL_ERROR;
		थांबा;
	    }
	    ओळ = Tcl_GetStringFromObj(objv[2], रिक्त);

	    pos = रिक्त;
	    जर (ओळ[0] != '\0'  &&  ओळ[1] == '\0')
		pos = getmark(ओळ[0], FALSE);
	    जर (pos == रिक्त)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		थांबा;
	    }
	    err = vimerror(interp);
	    जर (err != TCL_OK)
		थांबा;
	    जर (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    अन्यथा
	    {
		अक्षर rbuf[64];

		प्र_लिखो(rbuf, _("row %d column %d"),
			     (पूर्णांक)row2tcl(pos->lnum), (पूर्णांक)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    थांबा;

	स्थिती BUF_घाला:
	    opt = 1;
	    // fallthrough
	स्थिती BUF_APPEND:
	    जर (objc != 4)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		थांबा;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    जर (err != TCL_OK)
		थांबा;
	    जर (opt)
		--val1;
	    जर (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		थांबा;
	    }

	    ओळ = Tcl_GetStringFromObj(objv[3], रिक्त);
	    जर (ml_append((linenr_T)val1, (char_u *)ओळ, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		थांबा;
	    }
	    appended_ओळी_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    थांबा;

	स्थिती BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    जर (objc != 2)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, रिक्त);
		err = TCL_ERROR;
		थांबा;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		जर (win->w_buffer == buf)
		{
		    ओळ = tclgetwindow(interp, win);
		    जर (ओळ != रिक्त)
			Tcl_AppendElement(interp, ओळ);
		    अन्यथा
		    {
			err = TCL_ERROR;
			थांबा;
		    }
		}
	    }
	    थांबा;

	स्थिती BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    जर (objc != 3)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		थांबा;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (शून्य *)buf, objv[2]);
	    थांबा;

	पूर्वनिर्धारित:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    जर (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    जर (flags & FL_ADJUST_CURSOR)
	check_cursor();
    जर (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    परत err;
}

/*
 * This function implements the window commands.
 */
    स्थिर पूर्णांक
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    पूर्णांक		objc,
    Tcl_Obj	*CONST objv[])
{
    पूर्णांक		err, idx, flags;
    पूर्णांक		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    अक्षर	*str;

    गणना
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_अभिव्यक्ती,
	WIN_HEIGHT, WIN_OPTION
    };
    स्थिर CONST84 अक्षर *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (अक्षर *)0
    };

    जर (objc < 2)
    {
	Tcl_Wrong_अंक_Args(interp, 1, objv, "option ?arg ...?");
	परत TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    जर (err != TCL_OK)
	परत TCL_ERROR;

    win = (win_T *)((संरचना ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    निवड (idx)
    {
	स्थिती WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    थांबा;

	स्थिती WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    थांबा;

	स्थिती WIN_अभिव्यक्ती:
	    err = tclvimexpr(interp, objc, objv, 2);
	    थांबा;

	स्थिती WIN_HEIGHT:
	    जर (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		जर (err != TCL_OK)
		    थांबा;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		जर (err != TCL_OK)
		    थांबा;
	    }
	    अन्यथा
	    जर (objc != 2)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		थांबा;
	    }

	    resobj = Tcl_NewIntObj((पूर्णांक)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    थांबा;

	स्थिती WIN_BUFFER:
	    जर (objc != 2)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, रिक्त);
		err = TCL_ERROR;
		थांबा;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    जर (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    अन्यथा
		err = TCL_ERROR;
	    थांबा;

	स्थिती WIN_DELCMD:
	    जर (objc != 3)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		थांबा;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (शून्य *)win, objv[2]);
	    थांबा;

	स्थिती WIN_CURSOR:
	    जर (objc > 4)
	    {
		Tcl_Wrong_अंक_Args(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		थांबा;
	    }
	    जर (objc == 2)
	    {
		अक्षर buf[64];

		प्र_लिखो(buf, _("row %d column %d"), (पूर्णांक)row2tcl(win->w_cursor.lnum), (पूर्णांक)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		थांबा;
	    }
	    अन्यथा जर (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_संदेश);
		जर (var == रिक्त)
		{
		    err = TCL_ERROR;
		    थांबा;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		जर (err != TCL_OK)
		    थांबा;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_संदेश);
		जर (var == रिक्त)
		{
		    err = TCL_ERROR;
		    थांबा;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		जर (err != TCL_OK)
		    थांबा;
	    }
	    अन्यथा
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		जर (err != TCL_OK)
		    थांबा;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		जर (err != TCL_OK)
		    थांबा;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    थांबा;

	पूर्वनिर्धारित:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    थांबा;
    }

    curwin = savewin;
    curbuf = savebuf;
    जर (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    परत err;
}


    स्थिर पूर्णांक
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    पूर्णांक		objc,
    Tcl_Obj	*CONST objv[])
{
    पूर्णांक		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    परत err;
}

    स्थिर पूर्णांक
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    पूर्णांक		objc,
    Tcl_Obj	*CONST objv[])
{
    पूर्णांक		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    परत err;
}

    स्थिर पूर्णांक
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    पूर्णांक		objc,
    Tcl_Obj	*CONST objv[])
{
    परत tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    स्थिर पूर्णांक
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    पूर्णांक		*मूल्य_P,
    buf_T	*buf)
{
    पूर्णांक err, i;

    गणना { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_शीर्ष };

    स्थिर CONST84 अक्षर *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (अक्षर *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    जर (err == TCL_OK)
    {
	निवड (i)
	{
	    स्थिती LN_BEGIN:
	    स्थिती LN_FIRST:
	    स्थिती LN_START:
	    स्थिती LN_शीर्ष:
		*मूल्य_P = 1;
		थांबा;
	    स्थिती LN_BOTTOM:
	    स्थिती LN_END:
	    स्थिती LN_LAST:
		*मूल्य_P = buf->b_ml.ml_ओळ_मोजणी;
		थांबा;
	}
	परत TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    जर (err != TCL_OK)
	परत err;
    i = row2vim(i);
    जर (i < 1  ||  i > buf->b_ml.ml_ओळ_मोजणी)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	परत TCL_ERROR;
    }
    *मूल्य_P = i;
    परत TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    स्थिर win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	जर (win->w_buffer == buf)
	    परत win;
    }
    परत curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    स्थिर पूर्णांक
tcldoexcommand(
    Tcl_Interp	*interp,
    पूर्णांक		objc,
    Tcl_Obj	*CONST objv[],
    पूर्णांक		objn)
{
    tcl_info	saveinfo;
    पूर्णांक		err, flag, nobjs;
    अक्षर	*arg;

    nobjs = objc - objn;
    जर (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_अंक_Args(interp, objn, objv, "?-quiet? exCommand");
	परत TCL_ERROR;
    }

    flag = 0;
    जर (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], रिक्त);
	जर (म_भेद(arg, "-quiet") == 0)
	    flag = 1;
	अन्यथा
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (अक्षर *)0);
	    परत TCL_ERROR;
	}
	++objn;
    }

    स_नकल(&saveinfo, &tclinfo, आकार(tcl_info));
    tclinfo.interp = रिक्त;
    tclinfo.curwin = रिक्त;
    tclinfo.curbuf = रिक्त;

    arg = Tcl_GetStringFromObj(objv[objn], रिक्त);
    जर (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    जर (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    जर (tclinfo.interp)
	tcldelthisinterp();
    स_नकल(&tclinfo, &saveinfo, आकार(tcl_info));
    tclupdatevars();

    परत err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    स्थिर पूर्णांक
tclsetoption(
    Tcl_Interp	*interp,
    पूर्णांक		objc,
    Tcl_Obj	*CONST objv[],
    पूर्णांक		objn)
{
    पूर्णांक		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    दीर्घ	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    गणना { OPT_OFF, OPT_ON, OPT_TOGGLE };
    स्थिर CONST84 अक्षर *optkw[] = { "off", "on", "toggle", (अक्षर *)0 };

    nobjs = objc - objn;
    जर (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_अंक_Args(interp, objn, objv, "vimOption ?value?");
	परत TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], रिक्त);
    ++objn;
    gov = get_option_मूल्य(option, &lval, &sval, रिक्त, 0);
    err = TCL_OK;
    निवड (gov)
    {
	स्थिती gov_string:
	    Tcl_SetResult(interp, (अक्षर *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    थांबा;
	स्थिती gov_bool:
	स्थिती gov_संख्या:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    थांबा;
	पूर्वनिर्धारित:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    परत TCL_ERROR;
    }
    जर (nobjs == 2)
    {
	जर (gov != gov_string)
	{
	    sval = रिक्त;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    जर (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    अन्यथा
	    {
		निवड (idx)
		{
		    स्थिती OPT_ON:
			lval = 1;
			थांबा;
		    स्थिती OPT_OFF:
			lval = 0;
			थांबा;
		    स्थिती OPT_TOGGLE:
			lval = !lval;
			थांबा;
		}
	    }
	}
	अन्यथा
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], रिक्त);
	जर (err == TCL_OK)
	{
	    set_option_मूल्य_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    परत err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    स्थिर पूर्णांक
tclvimexpr(
    Tcl_Interp	*interp,
    पूर्णांक		objc,
    Tcl_Obj	*CONST objv[],
    पूर्णांक		objn)
{
#ifdef FEAT_EVAL
    अक्षर	*अभिव्यक्ती, *str;
#endif
    पूर्णांक		err;

    जर (objc - objn != 1)
    {
	Tcl_Wrong_अंक_Args(interp, objn, objv, "vimExpr");
	परत TCL_ERROR;
    }

#ifdef FEAT_EVAL
    अभिव्यक्ती = Tcl_GetStringFromObj(objv[objn], रिक्त);
    str = (अक्षर *)eval_to_string((char_u *)अभिव्यक्ती, TRUE, FALSE);
    जर (str == रिक्त)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    अन्यथा
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#अन्यथा
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    परत err;
}

/*
 * Check for internal vim errors.
 */
    स्थिर पूर्णांक
vimerror(Tcl_Interp *interp)
{
    जर (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	परत TCL_ERROR;
    }
    अन्यथा जर (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	परत TCL_ERROR;
    }
    परत TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    स्थिर शून्य
delref(ClientData cref)
{
    संरचना ref *ref = (संरचना ref *)cref;

    जर (ref->delcmd)
    {
	Tcl_Decr_Ref_मोजणी(ref->delcmd);
	ref->delcmd = रिक्त;
    }
    ref->interp = रिक्त;
}

    स्थिर अक्षर *
tclgetref(
    Tcl_Interp	*interp,
    शून्य	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    अक्षर	*prefix,	// "win" or "buf"
    शून्य	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    संरचना ref *ref, *unused = रिक्त;
    स्थिर अक्षर name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (संरचना ref *)(*refstartP);
    जर (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	परत रिक्त;
    }

    पर्यंत (ref != रिक्त)
    {
	जर (ref->interp == interp)
	    थांबा;
	जर (ref->interp == रिक्त)
	    unused = ref;
	ref = ref->next;
    }

    जर (ref)
	vim_snprintf(name, आकार(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    अन्यथा
    {
	जर (unused)
	    ref = unused;
	अन्यथा
	{
	    ref = (संरचना ref *)Tcl_Alloc(आकार(संरचना ref));
	    ref->interp = रिक्त;
	    ref->next = (संरचना ref *)(*refstartP);
	    (*refstartP) = (शून्य *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, आकार(name), "::vim::%s_%lx",
					       prefix, (अचिन्हित दीर्घ)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	जर (!cmd)
	    परत रिक्त;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = रिक्त;
	ref->vimobj = vimobj;
    }
    परत name;
}

    स्थिर अक्षर *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    परत tclgetref(interp, &(win->w_tcl_ref), "win", (शून्य *)win, winselfcmd);
}

    स्थिर अक्षर *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    परत tclgetref(interp, &(buf->b_tcl_ref), "buf", (शून्य *)buf, bufselfcmd);
}

    स्थिर पूर्णांक
tclsetdelcmd(
    Tcl_Interp	*interp,
    संरचना ref	*reflist,
    शून्य	*vimobj,
    Tcl_Obj	*delcmd)
{
    जर (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	परत TCL_ERROR;
    }

    पर्यंत (reflist != रिक्त)
    {
	जर (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    जर (reflist->delcmd)
		Tcl_Decr_Ref_मोजणी(reflist->delcmd);
	    Tcl_Incr_Ref_मोजणी(delcmd);
	    reflist->delcmd = delcmd;
	    परत TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    परत TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    स्थिर पूर्णांक
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    पूर्णांक		err = 0;

    // currently does nothing

    जर (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    परत err;
}

    स्थिर पूर्णांक
tcl_channel_आदान(
    ClientData	instance UNUSED,
    अक्षर	*buf UNUSED,
    पूर्णांक		bufsiz UNUSED,
    पूर्णांक		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    जर (errptr)
	*errptr = EINVAL;
    परत -1;
}

    स्थिर पूर्णांक
tcl_channel_output(
    ClientData	instance,
    स्थिरांक अक्षर	*buf,
    पूर्णांक		bufsiz,
    पूर्णांक		*errptr)
{
    char_u	*str;
    पूर्णांक		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    जर (!str)
    {
	Tcl_SetErrno(ENOMEM);
	जर (errptr)
	    *errptr = ENOMEM;
	परत -1;
    }

    result = bufsiz;
    जर (instance == VIMOUT)
	tclmsg((अक्षर *)str);
    अन्यथा
    जर (instance == VIMERR)
	tclerrmsg((अक्षर *)str);
    अन्यथा
    {
	Tcl_SetErrno(EBADF);
	जर (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    परत result;
}

    स्थिर शून्य
tcl_channel_watch(ClientData instance UNUSED, पूर्णांक mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    स्थिर पूर्णांक
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    पूर्णांक		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    परत EINVAL;
}


स्थिर Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_VERSION_2, // version
    tcl_channel_close,	// closeProc
    tcl_channel_आदान,	// inputProc
    tcl_channel_output,	// outputProc
    रिक्त,		// seekProc
    रिक्त,		// setOptionProc
    रिक्त,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    रिक्त,		// close2Proc
    रिक्त,		// blockModeProc
#ifdef TCL_CHANNEL_VERSION_2
    रिक्त,		// flushProc
    रिक्त,		// handlerProc
#endif
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ifdef TCL_CHANNEL_VERSION_3
    रिक्त,		// wideSeekProc
#endif
#ifdef TCL_CHANNEL_VERSION_4
    रिक्त,		// threadActionProc
#endif
#ifdef TCL_CHANNEL_VERSION_5
    रिक्त		// truncateProc
#endif
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    स्थिर शून्य
tclupdatevars(शून्य)
{
    अक्षर varname[VARNAME_SIZE];	// must be writeable
    अक्षर *name;

    म_नकल(varname, VAR_RANGE1);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);
    म_नकल(varname, VAR_RANGE2);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);
    म_नकल(varname, VAR_RANGE3);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);

    म_नकल(varname, VAR_LBASE);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);

    name = tclgetbuffer(tclinfo.interp, curbuf);
    म_नकल(tclinfo.curbuf, name);
    म_नकल(varname, VAR_CURBUF);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);

    name = tclgetwindow(tclinfo.interp, curwin);
    म_नकल(tclinfo.curwin, name);
    म_नकल(varname, VAR_CURWIN);
    Tcl_UpdateLinkedVar(tclinfo.interp, varname);
}


    स्थिर पूर्णांक
tclinit(exarg_T *eap)
{
    अक्षर varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    अक्षर *name;

#ifdef DYNAMIC_TCL
    जर (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	परत FAIL;
    }
#endif

    जर (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	स्थिर Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_SetStdChannel(ch1, TCL_STDOUT);
	Tcl_SetStdChannel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	जर (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    परत FAIL;
	}
#जर 0
	// VIM sure is interactive
	Tcl_SetVar(interp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
#endif

	Tcl_SetChannelOption(interp, ch1, "-buffering", "line");
#ifdef MSWIN
	Tcl_SetChannelOption(interp, ch1, "-translation", "lf");
#endif
	Tcl_SetChannelOption(interp, ch2, "-buffering", "line");
#ifdef MSWIN
	Tcl_SetChannelOption(interp, ch2, "-translation", "lf");
#endif

	// replace standard Tcl exit command
	Tcl_DeleteCommand(interp, "exit");
	Tcl_CreateObjCommand(interp, "exit", exitcmd,
	    (ClientData)रिक्त, (Tcl_CmdDeleteProc *)रिक्त);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (ClientData)रिक्त, (Tcl_CmdDeleteProc *)रिक्त);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (ClientData)रिक्त, (Tcl_CmdDeleteProc *)रिक्त);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (ClientData)रिक्त, (Tcl_CmdDeleteProc *)रिक्त);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (ClientData)रिक्त, (Tcl_CmdDeleteProc *)रिक्त);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (ClientData)रिक्त, (Tcl_CmdDeleteProc *)रिक्त);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (ClientData)रिक्त, (Tcl_CmdDeleteProc *)रिक्त);

	// "lbase" variable
	tclinfo.lbase = 1;
	म_नकल(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (अक्षर *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	म_नकल(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (अक्षर *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	म_नकल(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (अक्षर *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	म_नकल(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (अक्षर *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	म_नकल(tclinfo.curbuf, name);
	म_नकल(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (अक्षर *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	म_नकल(tclinfo.curwin, name);
	म_नकल(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (अक्षर *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    अन्यथा
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    परत OK;
}

    स्थिर शून्य
tclerrmsg(अक्षर *text)
{
    अक्षर *next;

    पर्यंत ((next=म_अक्षर(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    जर (*text)
	emsg(text);
}

    स्थिर शून्य
tclmsg(अक्षर *text)
{
    अक्षर *next;

    पर्यंत ((next=म_अक्षर(text, '\n')))
    {
	*next++ = '\0';
	संदेश(text);
	text = next;
    }
    जर (*text)
	संदेश(text);
}

    स्थिर शून्य
tcldelthisinterp(शून्य)
{
    जर (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = रिक्त;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = रिक्त;
}

    स्थिर पूर्णांक
tclexit(पूर्णांक error)
{
    पूर्णांक newerr = OK;

    जर (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#जर (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	अक्षर buf[50];

	प्र_लिखो(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	जर (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	अन्यथा
	    newerr = FAIL;

	tcldelthisinterp();
    }
    अन्यथा
    {
	अक्षर *result;

	result = (अक्षर *)Tcl_GetStringResult(tclinfo.interp);
	जर (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	अन्यथा
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    परत newerr;
}

/*
 * ":tcl"
 */
    शून्य
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    पूर्णांक		err;

    script = script_get(eap, eap->arg);
    जर (!eap->skip)
    {
	err = tclinit(eap);
	जर (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    जर (script == रिक्त)
		err = Tcl_Eval(tclinfo.interp, (अक्षर *)eap->arg);
	    अन्यथा
		err = Tcl_Eval(tclinfo.interp, (अक्षर *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    शून्य
ex_tclfile(exarg_T *eap)
{
    अक्षर *file = (अक्षर *)eap->arg;
    पूर्णांक err;

    err = tclinit(eap);
    जर (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    शून्य
ex_tcldo(exarg_T *eap)
{
    अक्षर	*script, *ओळ;
    पूर्णांक		err, rs, re, lnum;
    अक्षर	var_lnum[VARNAME_SIZE]; // must be writeable memory
    अक्षर	var_ओळ[VARNAME_SIZE];
    linenr_T	first_ओळ = 0;
    linenr_T	last_ओळ = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (अक्षर *)eap->arg;
    म_नकल(var_lnum, VAR_CURLNUM);
    म_नकल(var_ओळ, VAR_CURLINE);

    err = tclinit(eap);
    जर (err != OK)
	परत;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (अक्षर *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    जर (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    पर्यंत (err == TCL_OK  &&  rs <= re)
    {
	जर ((linenr_T)rs > curbuf->b_ml.ml_ओळ_मोजणी)
	    थांबा;
	ओळ = (अक्षर *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	जर (!ओळ)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    थांबा;
	}
	Tcl_SetVar(tclinfo.interp, var_ओळ, ओळ, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	जर (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#जर (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_ओळ_मोजणी)
	    थांबा;
	ओळ = (अक्षर *)Tcl_GetVar(tclinfo.interp, var_ओळ, 0);
	जर (ओळ)
	{
	    जर (ml_replace((linenr_T)rs, (char_u *)ओळ, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		थांबा;
	    }
	    जर (first_ओळ == 0)
		first_ओळ = rs;
	    last_ओळ = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    जर (first_ओळ)
	changed_ओळी(first_ओळ, 0, last_ओळ + 1, (दीर्घ)0);

    Tcl_UnsetVar(tclinfo.interp, var_ओळ, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    जर (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (शून्य)tclexit(err);
}

    स्थिर शून्य
tcldelallrefs(संरचना ref *ref)
{
    संरचना ref	*next;
    पूर्णांक		err;
    अक्षर	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    जर (exiting)
	परत;
#endif

    पर्यंत (ref != रिक्त)
    {
	next = ref->next;
	जर (ref->interp)
	{
	    जर (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		जर (err != TCL_OK)
		{
		    result = (अक्षर *)Tcl_GetStringResult(ref->interp);
		    जर (result)
			tclerrmsg(result);
		}
		Tcl_Decr_Ref_मोजणी(ref->delcmd);
		ref->delcmd = रिक्त;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((अक्षर *)ref);
	ref = next;
    }
}

    शून्य
tcl_buffer_free(buf_T *buf)
{
    संरचना ref *reflist;

#ifdef DYNAMIC_TCL
    जर (!stubs_initialized)	// Not using Tcl, nothing to do.
	परत;
#endif

    reflist = (संरचना ref *)(buf->b_tcl_ref);
    जर (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (शून्य *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = रिक्त;
    }
}

    शून्य
tcl_window_free(win_T *win)
{
    संरचना ref *reflist;

#ifdef DYNAMIC_TCL
    जर (!stubs_initialized)	// Not using Tcl, nothing to do.
	परत;
#endif

    reflist = (संरचना ref*)(win->w_tcl_ref);
    जर (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (शून्य *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = रिक्त;
    }
}

// The End
