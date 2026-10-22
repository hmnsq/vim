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

प्रकारनिर्देश संरचना
{
    Tcl_Interp *interp;
    पूर्णाङ्क exitvalue;
    पूर्णाङ्क range_start, range_end;
    पूर्णाङ्क lbase;
    वर्ण *curbuf, *curwin;
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
स्थिर वर्ण * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
स्थिर वर्ण * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
स्थिर पूर्णाङ्क tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, संरचना ref *reflist, शून्य *vimobj, Tcl_Obj *delcmd));
स्थिर पूर्णाङ्क tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, पूर्णाङ्क *मानम्_P, buf_T *buf));
स्थिर win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
स्थिर पूर्णाङ्क tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, पूर्णाङ्क objc, Tcl_Obj *CONST objv[], पूर्णाङ्क objn));
स्थिर पूर्णाङ्क tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, पूर्णाङ्क objc, Tcl_Obj *CONST objv[], पूर्णाङ्क objn));
स्थिर पूर्णाङ्क tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, पूर्णाङ्क objc, Tcl_Obj *CONST objv[], पूर्णाङ्क objn));
स्थिर शून्य tcldelthisinterp _ANSI_ARGS_ ((शून्य));

स्थिर पूर्णाङ्क vimerror _ANSI_ARGS_((Tcl_Interp *interp));
स्थिर शून्य tclmsg _ANSI_ARGS_((वर्ण *text));
स्थिर शून्य tclerrmsg _ANSI_ARGS_((वर्ण *text));
स्थिर शून्य tclupdatevars _ANSI_ARGS_((शून्य));

स्थिर संरचना ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#सचे defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
प्रकारनिर्देश पूर्णाङ्क HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define चिह्नम्_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# अञ्ञथा
#  include <dlfcn.h>
#  define HANDLE शून्य*
#  define TCL_PROC शून्य*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define चिह्नम्_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
स्थिर HANDLE hTclLib = रिक्त;
Tcl_Interp* (*dll_Tcl_CreateInterp)(शून्य);
शून्य (*dll_Tcl_FindExecutable)(ध्रुव शून्य *);

/*
 * Table of name to function pointer of tcl.
 */
स्थिर संरचना {
    वर्ण* name;
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
    स्थिर पूर्णाङ्क
tcl_runtime_link_init(वर्ण *libname, पूर्णाङ्क verbose)
{
    पूर्णाङ्क i;

    सचे (hTclLib)
	पटिनिवत्त OK;
    सचे (!(hTclLib = load_dll(libname)))
    {
	सचे (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	पटिनिवत्त FAIL;
    }
    अत्थाय (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	सचे (!(*tcl_funcname_table[i].ptr = चिह्नम्_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = रिक्त;
	    सचे (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    पटिनिवत्त FAIL;
	}
    }
    पटिनिवत्त OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
स्थिर वर्ण *find_executable_arg = रिक्त;
#endif

    शून्य
vim_tcl_init(वर्ण *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#अञ्ञथा
    Tcl_FindExecutable(arg);
#endif
}

#सचे defined(DYNAMIC_TCL)

स्थिर पूर्णाङ्क stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    पूर्णाङ्क
tcl_enabled(पूर्णाङ्क verbose)
{
    सचे (!stubs_initialized && find_executable_arg != रिक्त
	    && tcl_runtime_link_init((वर्ण *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	सचे ((interp = dll_Tcl_CreateInterp()) != रिक्त)
	{
	    सचे (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != रिक्त)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    पटिनिवत्त stubs_initialized;
}
#endif

#सचे defined(EXITFREE)
/*
 * Called once when exiting.
 */
    शून्य
vim_tcl_finalize(शून्य)
{
# ifdef DYNAMIC_TCL
    सचे (stubs_initialized)
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
    स्थिर पूर्णाङ्क
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    पूर्णाङ्क objc,
    Tcl_Obj *CONST objv[])
{
    पूर्णाङ्क मानम् = 0;

    विकल्प (objc)
    {
	पक्ष 2:
	    सचे (Tcl_GetIntFromObj(interp, objv[1], &मानम्) != TCL_OK)
		भङ्ग;
	    // FALLTHROUGH
	पक्ष 1:
	    tclinfo.exitvalue = मानम्;

	    Tcl_DeleteInterp(interp);
	    भङ्ग;
	स्वभाव:
	    Tcl_Wrong_अङ्कः_Args(interp, 1, objv, "?returnCode?");
    }
    पटिनिवत्त TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    स्थिर पूर्णाङ्क
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    पूर्णाङ्क objc,
    Tcl_Obj *CONST objv[])
{
    सचे (objc != 1)
    {
	Tcl_Wrong_अङ्कः_Args(interp, 1, objv, रिक्त);
	पटिनिवत्त TCL_ERROR;
    }
    vim_beep(BO_LANG);
    पटिनिवत्त TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    स्थिर पूर्णाङ्क
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    पूर्णाङ्क objc,
    Tcl_Obj *CONST objv[])
{
    वर्ण	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    पूर्णाङ्क		err, n, idx;
    गणना {BCMD_EXISTS, BCMD_LIST};
    स्थिर CONST84 वर्ण *bcmdoptions[] =
    {
	"exists", "list", (वर्ण *)0
    };

    सचे (objc < 2)
    {
	Tcl_Wrong_अङ्कः_Args(interp, 1, objv, "option");
	पटिनिवत्त TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    सचे (err == TCL_OK)
    {
	सचे (objc != 2)
	{
	    Tcl_Wrong_अङ्कः_Args(interp, 1, objv, "bufNumber");
	    पटिनिवत्त TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    सचे (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		सचे (name == रिक्त)
		    पटिनिवत्त TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		पटिनिवत्त TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	पटिनिवत्त TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    सचे (err != TCL_OK)
	पटिनिवत्त err;
    विकल्प (idx)
    {
	पक्ष BCMD_LIST:
	    सचे (objc != 2)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, "");
		err = TCL_ERROR;
		भङ्ग;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		सचे (name == रिक्त)
		{
		    err = TCL_ERROR;
		    भङ्ग;
		}
		Tcl_AppendElement(interp, name);
	    }
	    भङ्ग;

	पक्ष BCMD_EXISTS:
	    सचे (objc != 3)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		भङ्ग;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    सचे (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != रिक्त);
		Tcl_SetObjResult(interp, resobj);
	    }
	    भङ्ग;

	स्वभाव:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    पटिनिवत्त err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    स्थिर पूर्णाङ्क
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    पूर्णाङ्क		objc,
    Tcl_Obj	*CONST objv[])
{
    वर्ण	*what, *string;
    win_T	*win;

    सचे (objc != 2)
    {
	Tcl_Wrong_अङ्कः_Args(interp, 1, objv, "option");
	पटिनिवत्त TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], रिक्त);
    सचे (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    सचे (string == रिक्त)
		पटिनिवत्त TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	पटिनिवत्त TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    पटिनिवत्त TCL_ERROR;
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
    स्थिर पूर्णाङ्क
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    पूर्णाङ्क		objc,
    Tcl_Obj	*CONST objv[])
{
    पूर्णाङ्क		opt, err, idx, flags;
    पूर्णाङ्क		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    वर्ण	*पङ्क्तिः;

    गणना
    {
	BUF_APPEND, BUF_COMMAND, BUF_गणनम्, BUF_DELCMD, BUF_DELETE, BUF_व्यञ्जकम्,
	BUF_GET, BUF_निवेशय, BUF_LAST, BUF_MARK, BUF_NAME, BUF_सङ्ख्या,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    स्थिर CONST84 वर्ण *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (वर्ण *)0
    };

    सचे (objc < 2)
    {
	Tcl_Wrong_अङ्कः_Args(interp, 1, objv, "option ?arg ...?");
	पटिनिवत्त TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    सचे (err != TCL_OK)
	पटिनिवत्त err;

    buf = (buf_T *)((संरचना ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    विकल्प (idx)
    {
	पक्ष BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    भङ्ग;

	पक्ष BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    भङ्ग;

	पक्ष BUF_व्यञ्जकम्:
	    err = tclvimexpr(interp, objc, objv, 2);
	    भङ्ग;

	पक्ष BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    सचे (objc != 2)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, रिक्त);
		err = TCL_ERROR;
		भङ्ग;
	    }
	    सचे (buf->b_ffname)
		Tcl_SetResult(interp, (वर्ण *)buf->b_ffname, TCL_VOLATILE);
	    अञ्ञथा
		Tcl_SetResult(interp, "", TCL_STATIC);
	    भङ्ग;

	पक्ष BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	पक्ष BUF_गणनम्:
	    /*
	     * Get number of lines in buffer.
	     */
	    सचे (objc != 2)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, रिक्त);
		err = TCL_ERROR;
		भङ्ग;
	    }
	    val1 = (पूर्णाङ्क)buf->b_ml.ml_पङ्क्तिः_गणनम्;
	    सचे (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    भङ्ग;

	पक्ष BUF_सङ्ख्या:
	    /*
	     * Get buffer's number.
	     */
	    सचे (objc != 2)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, रिक्त);
		err = TCL_ERROR;
		भङ्ग;
	    }
	    resobj = Tcl_NewIntObj((पूर्णाङ्क)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    भङ्ग;

	पक्ष BUF_GET:
	    सचे (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		भङ्ग;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    सचे (err != TCL_OK)
		भङ्ग;
	    सचे (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		सचे (err != TCL_OK)
		    भङ्ग;
		सचे (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		अत्थाय (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    पङ्क्तिः = (वर्ण *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    सचे (पङ्क्तिः)
			Tcl_AppendElement(interp, पङ्क्तिः);
		    अञ्ञथा
			err = TCL_ERROR;
		}
	    }
	    अञ्ञथा
	    {  // objc == 3
		पङ्क्तिः = (वर्ण *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, पङ्क्तिः, TCL_VOLATILE);
	    }
	    भङ्ग;

	पक्ष BUF_SET:
	    सचे (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		भङ्ग;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    सचे (err != TCL_OK)
		पटिनिवत्त TCL_ERROR;
	    सचे (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		पङ्क्तिः = Tcl_GetStringFromObj(objv[3], रिक्त);
		सचे (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		अञ्ञथा
		सचे (ml_replace((linenr_T)val1, (char_u *)पङ्क्तिः, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		अञ्ञथा
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		भङ्ग;
	    }
	    अञ्ञथा
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		पूर्णाङ्क	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		सचे (err != TCL_OK)
		    भङ्ग;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		सचे (err != TCL_OK)
		    भङ्ग;
		सचे (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		सचे (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    भङ्ग;
		}
		flags |= FL_UPDATE_CURBUF;

		अत्थाय (i = 0; i < lc && n <= val2; i++)
		{
		    पङ्क्तिः = Tcl_GetStringFromObj(lv[i], रिक्त);
		    सचे (ml_replace((linenr_T)n, (char_u *)पङ्क्तिः, TRUE) != OK)
			गच्छ setListError;
		    ++n;
		}
		सचे (i < lc)
		{
		    // append lines
		    करोहि
		    {
			पङ्क्तिः = Tcl_GetStringFromObj(lv[i], रिक्त);
			सचे (ml_append((linenr_T)(n - 1),
					      (char_u *)पङ्क्तिः, 0, FALSE) != OK)
			    गच्छ setListError;
			++n;
			++i;
		    } याव (i < lc);
		}
		अञ्ञथा सचे (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    करोहि
		    {
			सचे (ml_delete((linenr_T)i) != OK)
			    गच्छ setListError;
			++n;
		    } याव (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (long)MAXLNUM,
								    (long)lc);
		changed_पङ्क्तयः((linenr_T)val1, 0, (linenr_T)val2 + 1, (long)lc);
		भङ्ग;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    भङ्ग;

	पक्ष BUF_DELETE:
	    सचे (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		भङ्ग;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    सचे (err != TCL_OK)
		भङ्ग;
	    val2 = val1;
	    सचे (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		सचे (err != TCL_OK)
		    पटिनिवत्त err;
		सचे (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    सचे (u_savedel((linenr_T)val1, (long)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		भङ्ग;
	    }
	    अत्थाय (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		सचे (err != TCL_OK)
		    भङ्ग;
	    }
	    सचे (i > 0)
		deleted_पङ्क्तयः_mark((linenr_T)val1, (long)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    भङ्ग;

	पक्ष BUF_MARK:
	    सचे (objc != 3)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, "markName");
		err = TCL_ERROR;
		भङ्ग;
	    }
	    पङ्क्तिः = Tcl_GetStringFromObj(objv[2], रिक्त);

	    pos = रिक्त;
	    सचे (पङ्क्तिः[0] != '\0'  &&  पङ्क्तिः[1] == '\0')
		pos = getmark(पङ्क्तिः[0], FALSE);
	    सचे (pos == रिक्त)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		भङ्ग;
	    }
	    err = vimerror(interp);
	    सचे (err != TCL_OK)
		भङ्ग;
	    सचे (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    अञ्ञथा
	    {
		वर्ण rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (पूर्णाङ्क)row2tcl(pos->lnum), (पूर्णाङ्क)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    भङ्ग;

	पक्ष BUF_निवेशय:
	    opt = 1;
	    // fallthrough
	पक्ष BUF_APPEND:
	    सचे (objc != 4)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		भङ्ग;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    सचे (err != TCL_OK)
		भङ्ग;
	    सचे (opt)
		--val1;
	    सचे (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		भङ्ग;
	    }

	    पङ्क्तिः = Tcl_GetStringFromObj(objv[3], रिक्त);
	    सचे (ml_append((linenr_T)val1, (char_u *)पङ्क्तिः, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		भङ्ग;
	    }
	    appended_पङ्क्तयः_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    भङ्ग;

	पक्ष BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    सचे (objc != 2)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, रिक्त);
		err = TCL_ERROR;
		भङ्ग;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		सचे (win->w_buffer == buf)
		{
		    पङ्क्तिः = tclgetwindow(interp, win);
		    सचे (पङ्क्तिः != रिक्त)
			Tcl_AppendElement(interp, पङ्क्तिः);
		    अञ्ञथा
		    {
			err = TCL_ERROR;
			भङ्ग;
		    }
		}
	    }
	    भङ्ग;

	पक्ष BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    सचे (objc != 3)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		भङ्ग;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (शून्य *)buf, objv[2]);
	    भङ्ग;

	स्वभाव:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    सचे (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    सचे (flags & FL_ADJUST_CURSOR)
	check_cursor();
    सचे (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    पटिनिवत्त err;
}

/*
 * This function implements the window commands.
 */
    स्थिर पूर्णाङ्क
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    पूर्णाङ्क		objc,
    Tcl_Obj	*CONST objv[])
{
    पूर्णाङ्क		err, idx, flags;
    पूर्णाङ्क		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    वर्ण	*str;

    गणना
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_व्यञ्जकम्,
	WIN_HEIGHT, WIN_OPTION
    };
    स्थिर CONST84 वर्ण *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (वर्ण *)0
    };

    सचे (objc < 2)
    {
	Tcl_Wrong_अङ्कः_Args(interp, 1, objv, "option ?arg ...?");
	पटिनिवत्त TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    सचे (err != TCL_OK)
	पटिनिवत्त TCL_ERROR;

    win = (win_T *)((संरचना ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    विकल्प (idx)
    {
	पक्ष WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    भङ्ग;

	पक्ष WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    भङ्ग;

	पक्ष WIN_व्यञ्जकम्:
	    err = tclvimexpr(interp, objc, objv, 2);
	    भङ्ग;

	पक्ष WIN_HEIGHT:
	    सचे (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		सचे (err != TCL_OK)
		    भङ्ग;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		सचे (err != TCL_OK)
		    भङ्ग;
	    }
	    अञ्ञथा
	    सचे (objc != 2)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		भङ्ग;
	    }

	    resobj = Tcl_NewIntObj((पूर्णाङ्क)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    भङ्ग;

	पक्ष WIN_BUFFER:
	    सचे (objc != 2)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, रिक्त);
		err = TCL_ERROR;
		भङ्ग;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    सचे (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    अञ्ञथा
		err = TCL_ERROR;
	    भङ्ग;

	पक्ष WIN_DELCMD:
	    सचे (objc != 3)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		भङ्ग;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (शून्य *)win, objv[2]);
	    भङ्ग;

	पक्ष WIN_CURSOR:
	    सचे (objc > 4)
	    {
		Tcl_Wrong_अङ्कः_Args(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		भङ्ग;
	    }
	    सचे (objc == 2)
	    {
		वर्ण buf[64];

		sprintf(buf, _("row %d column %d"), (पूर्णाङ्क)row2tcl(win->w_cursor.lnum), (पूर्णाङ्क)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		भङ्ग;
	    }
	    अञ्ञथा सचे (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_सन्देशः);
		सचे (var == रिक्त)
		{
		    err = TCL_ERROR;
		    भङ्ग;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		सचे (err != TCL_OK)
		    भङ्ग;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_सन्देशः);
		सचे (var == रिक्त)
		{
		    err = TCL_ERROR;
		    भङ्ग;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		सचे (err != TCL_OK)
		    भङ्ग;
	    }
	    अञ्ञथा
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		सचे (err != TCL_OK)
		    भङ्ग;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		सचे (err != TCL_OK)
		    भङ्ग;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    भङ्ग;

	स्वभाव:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    भङ्ग;
    }

    curwin = savewin;
    curbuf = savebuf;
    सचे (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    पटिनिवत्त err;
}


    स्थिर पूर्णाङ्क
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    पूर्णाङ्क		objc,
    Tcl_Obj	*CONST objv[])
{
    पूर्णाङ्क		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    पटिनिवत्त err;
}

    स्थिर पूर्णाङ्क
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    पूर्णाङ्क		objc,
    Tcl_Obj	*CONST objv[])
{
    पूर्णाङ्क		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    पटिनिवत्त err;
}

    स्थिर पूर्णाङ्क
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    पूर्णाङ्क		objc,
    Tcl_Obj	*CONST objv[])
{
    पटिनिवत्त tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    स्थिर पूर्णाङ्क
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    पूर्णाङ्क		*मानम्_P,
    buf_T	*buf)
{
    पूर्णाङ्क err, i;

    गणना { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_शिखरम् };

    स्थिर CONST84 वर्ण *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (वर्ण *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    सचे (err == TCL_OK)
    {
	विकल्प (i)
	{
	    पक्ष LN_BEGIN:
	    पक्ष LN_FIRST:
	    पक्ष LN_START:
	    पक्ष LN_शिखरम्:
		*मानम्_P = 1;
		भङ्ग;
	    पक्ष LN_BOTTOM:
	    पक्ष LN_END:
	    पक्ष LN_LAST:
		*मानम्_P = buf->b_ml.ml_पङ्क्तिः_गणनम्;
		भङ्ग;
	}
	पटिनिवत्त TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    सचे (err != TCL_OK)
	पटिनिवत्त err;
    i = row2vim(i);
    सचे (i < 1  ||  i > buf->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	पटिनिवत्त TCL_ERROR;
    }
    *मानम्_P = i;
    पटिनिवत्त TCL_OK;
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
	सचे (win->w_buffer == buf)
	    पटिनिवत्त win;
    }
    पटिनिवत्त curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    स्थिर पूर्णाङ्क
tcldoexcommand(
    Tcl_Interp	*interp,
    पूर्णाङ्क		objc,
    Tcl_Obj	*CONST objv[],
    पूर्णाङ्क		objn)
{
    tcl_info	saveinfo;
    पूर्णाङ्क		err, flag, nobjs;
    वर्ण	*arg;

    nobjs = objc - objn;
    सचे (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_अङ्कः_Args(interp, objn, objv, "?-quiet? exCommand");
	पटिनिवत्त TCL_ERROR;
    }

    flag = 0;
    सचे (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], रिक्त);
	सचे (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	अञ्ञथा
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (वर्ण *)0);
	    पटिनिवत्त TCL_ERROR;
	}
	++objn;
    }

    memcpy(&saveinfo, &tclinfo, परिमाण(tcl_info));
    tclinfo.interp = रिक्त;
    tclinfo.curwin = रिक्त;
    tclinfo.curbuf = रिक्त;

    arg = Tcl_GetStringFromObj(objv[objn], रिक्त);
    सचे (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    सचे (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    सचे (tclinfo.interp)
	tcldelthisinterp();
    memcpy(&tclinfo, &saveinfo, परिमाण(tcl_info));
    tclupdatevars();

    पटिनिवत्त err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    स्थिर पूर्णाङ्क
tclsetoption(
    Tcl_Interp	*interp,
    पूर्णाङ्क		objc,
    Tcl_Obj	*CONST objv[],
    पूर्णाङ्क		objn)
{
    पूर्णाङ्क		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    long	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    गणना { OPT_OFF, OPT_ON, OPT_TOGGLE };
    स्थिर CONST84 वर्ण *optkw[] = { "off", "on", "toggle", (वर्ण *)0 };

    nobjs = objc - objn;
    सचे (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_अङ्कः_Args(interp, objn, objv, "vimOption ?value?");
	पटिनिवत्त TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], रिक्त);
    ++objn;
    gov = get_option_मानम्(option, &lval, &sval, रिक्त, 0);
    err = TCL_OK;
    विकल्प (gov)
    {
	पक्ष gov_string:
	    Tcl_SetResult(interp, (वर्ण *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    भङ्ग;
	पक्ष gov_bool:
	पक्ष gov_सङ्ख्या:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    भङ्ग;
	स्वभाव:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    पटिनिवत्त TCL_ERROR;
    }
    सचे (nobjs == 2)
    {
	सचे (gov != gov_string)
	{
	    sval = रिक्त;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    सचे (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    अञ्ञथा
	    {
		विकल्प (idx)
		{
		    पक्ष OPT_ON:
			lval = 1;
			भङ्ग;
		    पक्ष OPT_OFF:
			lval = 0;
			भङ्ग;
		    पक्ष OPT_TOGGLE:
			lval = !lval;
			भङ्ग;
		}
	    }
	}
	अञ्ञथा
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], रिक्त);
	सचे (err == TCL_OK)
	{
	    set_option_मानम्_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    पटिनिवत्त err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    स्थिर पूर्णाङ्क
tclvimexpr(
    Tcl_Interp	*interp,
    पूर्णाङ्क		objc,
    Tcl_Obj	*CONST objv[],
    पूर्णाङ्क		objn)
{
#ifdef FEAT_EVAL
    वर्ण	*व्यञ्जकम्, *str;
#endif
    पूर्णाङ्क		err;

    सचे (objc - objn != 1)
    {
	Tcl_Wrong_अङ्कः_Args(interp, objn, objv, "vimExpr");
	पटिनिवत्त TCL_ERROR;
    }

#ifdef FEAT_EVAL
    व्यञ्जकम् = Tcl_GetStringFromObj(objv[objn], रिक्त);
    str = (वर्ण *)eval_to_string((char_u *)व्यञ्जकम्, TRUE, FALSE);
    सचे (str == रिक्त)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    अञ्ञथा
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#अञ्ञथा
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    पटिनिवत्त err;
}

/*
 * Check for internal vim errors.
 */
    स्थिर पूर्णाङ्क
vimerror(Tcl_Interp *interp)
{
    सचे (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	पटिनिवत्त TCL_ERROR;
    }
    अञ्ञथा सचे (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	पटिनिवत्त TCL_ERROR;
    }
    पटिनिवत्त TCL_OK;
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

    सचे (ref->delcmd)
    {
	Tcl_Decr_Ref_गणनम्(ref->delcmd);
	ref->delcmd = रिक्त;
    }
    ref->interp = रिक्त;
}

    स्थिर वर्ण *
tclgetref(
    Tcl_Interp	*interp,
    शून्य	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    वर्ण	*prefix,	// "win" or "buf"
    शून्य	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    संरचना ref *ref, *unused = रिक्त;
    स्थिर वर्ण name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (संरचना ref *)(*refstartP);
    सचे (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	पटिनिवत्त रिक्त;
    }

    याव (ref != रिक्त)
    {
	सचे (ref->interp == interp)
	    भङ्ग;
	सचे (ref->interp == रिक्त)
	    unused = ref;
	ref = ref->next;
    }

    सचे (ref)
	vim_snprintf(name, परिमाण(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    अञ्ञथा
    {
	सचे (unused)
	    ref = unused;
	अञ्ञथा
	{
	    ref = (संरचना ref *)Tcl_Alloc(परिमाण(संरचना ref));
	    ref->interp = रिक्त;
	    ref->next = (संरचना ref *)(*refstartP);
	    (*refstartP) = (शून्य *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, परिमाण(name), "::vim::%s_%lx",
					       prefix, (unsigned long)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	सचे (!cmd)
	    पटिनिवत्त रिक्त;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = रिक्त;
	ref->vimobj = vimobj;
    }
    पटिनिवत्त name;
}

    स्थिर वर्ण *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    पटिनिवत्त tclgetref(interp, &(win->w_tcl_ref), "win", (शून्य *)win, winselfcmd);
}

    स्थिर वर्ण *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    पटिनिवत्त tclgetref(interp, &(buf->b_tcl_ref), "buf", (शून्य *)buf, bufselfcmd);
}

    स्थिर पूर्णाङ्क
tclsetdelcmd(
    Tcl_Interp	*interp,
    संरचना ref	*reflist,
    शून्य	*vimobj,
    Tcl_Obj	*delcmd)
{
    सचे (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	पटिनिवत्त TCL_ERROR;
    }

    याव (reflist != रिक्त)
    {
	सचे (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    सचे (reflist->delcmd)
		Tcl_Decr_Ref_गणनम्(reflist->delcmd);
	    Tcl_Incr_Ref_गणनम्(delcmd);
	    reflist->delcmd = delcmd;
	    पटिनिवत्त TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    पटिनिवत्त TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    स्थिर पूर्णाङ्क
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    पूर्णाङ्क		err = 0;

    // currently does nothing

    सचे (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    पटिनिवत्त err;
}

    स्थिर पूर्णाङ्क
tcl_channel_निवेशः(
    ClientData	instance UNUSED,
    वर्ण	*buf UNUSED,
    पूर्णाङ्क		bufsiz UNUSED,
    पूर्णाङ्क		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    सचे (errptr)
	*errptr = EINVAL;
    पटिनिवत्त -1;
}

    स्थिर पूर्णाङ्क
tcl_channel_output(
    ClientData	instance,
    ध्रुव वर्ण	*buf,
    पूर्णाङ्क		bufsiz,
    पूर्णाङ्क		*errptr)
{
    char_u	*str;
    पूर्णाङ्क		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    सचे (!str)
    {
	Tcl_SetErrno(ENOMEM);
	सचे (errptr)
	    *errptr = ENOMEM;
	पटिनिवत्त -1;
    }

    result = bufsiz;
    सचे (instance == VIMOUT)
	tclmsg((वर्ण *)str);
    अञ्ञथा
    सचे (instance == VIMERR)
	tclerrmsg((वर्ण *)str);
    अञ्ञथा
    {
	Tcl_SetErrno(EBADF);
	सचे (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    पटिनिवत्त result;
}

    स्थिर शून्य
tcl_channel_watch(ClientData instance UNUSED, पूर्णाङ्क mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    स्थिर पूर्णाङ्क
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    पूर्णाङ्क		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    पटिनिवत्त EINVAL;
}


स्थिर Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_VERSION_2, // version
    tcl_channel_close,	// closeProc
    tcl_channel_निवेशः,	// inputProc
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
    वर्ण varname[VARNAME_SIZE];	// must be writeable
    वर्ण *name;

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


    स्थिर पूर्णाङ्क
tclinit(exarg_T *eap)
{
    वर्ण varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    वर्ण *name;

#ifdef DYNAMIC_TCL
    सचे (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	पटिनिवत्त FAIL;
    }
#endif

    सचे (!tclinfo.interp)
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
	सचे (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    पटिनिवत्त FAIL;
	}
#सचे 0
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
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (वर्ण *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (वर्ण *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (वर्ण *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (वर्ण *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (वर्ण *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (वर्ण *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    अञ्ञथा
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    पटिनिवत्त OK;
}

    स्थिर शून्य
tclerrmsg(वर्ण *text)
{
    वर्ण *next;

    याव ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    सचे (*text)
	emsg(text);
}

    स्थिर शून्य
tclmsg(वर्ण *text)
{
    वर्ण *next;

    याव ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	सन्देशः(text);
	text = next;
    }
    सचे (*text)
	सन्देशः(text);
}

    स्थिर शून्य
tcldelthisinterp(शून्य)
{
    सचे (!Tcl_InterpDeleted(tclinfo.interp))
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

    स्थिर पूर्णाङ्क
tclexit(पूर्णाङ्क error)
{
    पूर्णाङ्क newerr = OK;

    सचे (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#सचे (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	वर्ण buf[50];

	sprintf(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	सचे (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	अञ्ञथा
	    newerr = FAIL;

	tcldelthisinterp();
    }
    अञ्ञथा
    {
	वर्ण *result;

	result = (वर्ण *)Tcl_GetStringResult(tclinfo.interp);
	सचे (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	अञ्ञथा
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    पटिनिवत्त newerr;
}

/*
 * ":tcl"
 */
    शून्य
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    पूर्णाङ्क		err;

    script = script_get(eap, eap->arg);
    सचे (!eap->skip)
    {
	err = tclinit(eap);
	सचे (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    सचे (script == रिक्त)
		err = Tcl_Eval(tclinfo.interp, (वर्ण *)eap->arg);
	    अञ्ञथा
		err = Tcl_Eval(tclinfo.interp, (वर्ण *)script);
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
    वर्ण *file = (वर्ण *)eap->arg;
    पूर्णाङ्क err;

    err = tclinit(eap);
    सचे (err == OK)
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
    वर्ण	*script, *पङ्क्तिः;
    पूर्णाङ्क		err, rs, re, lnum;
    वर्ण	var_lnum[VARNAME_SIZE]; // must be writeable memory
    वर्ण	var_पङ्क्तिः[VARNAME_SIZE];
    linenr_T	first_पङ्क्तिः = 0;
    linenr_T	last_पङ्क्तिः = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (वर्ण *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_पङ्क्तिः, VAR_CURLINE);

    err = tclinit(eap);
    सचे (err != OK)
	पटिनिवत्त;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (वर्ण *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    सचे (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    याव (err == TCL_OK  &&  rs <= re)
    {
	सचे ((linenr_T)rs > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    भङ्ग;
	पङ्क्तिः = (वर्ण *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	सचे (!पङ्क्तिः)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    भङ्ग;
	}
	Tcl_SetVar(tclinfo.interp, var_पङ्क्तिः, पङ्क्तिः, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	सचे (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#सचे (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    भङ्ग;
	पङ्क्तिः = (वर्ण *)Tcl_GetVar(tclinfo.interp, var_पङ्क्तिः, 0);
	सचे (पङ्क्तिः)
	{
	    सचे (ml_replace((linenr_T)rs, (char_u *)पङ्क्तिः, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		भङ्ग;
	    }
	    सचे (first_पङ्क्तिः == 0)
		first_पङ्क्तिः = rs;
	    last_पङ्क्तिः = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    सचे (first_पङ्क्तिः)
	changed_पङ्क्तयः(first_पङ्क्तिः, 0, last_पङ्क्तिः + 1, (long)0);

    Tcl_UnsetVar(tclinfo.interp, var_पङ्क्तिः, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    सचे (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (शून्य)tclexit(err);
}

    स्थिर शून्य
tcldelallrefs(संरचना ref *ref)
{
    संरचना ref	*next;
    पूर्णाङ्क		err;
    वर्ण	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    सचे (exiting)
	पटिनिवत्त;
#endif

    याव (ref != रिक्त)
    {
	next = ref->next;
	सचे (ref->interp)
	{
	    सचे (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		सचे (err != TCL_OK)
		{
		    result = (वर्ण *)Tcl_GetStringResult(ref->interp);
		    सचे (result)
			tclerrmsg(result);
		}
		Tcl_Decr_Ref_गणनम्(ref->delcmd);
		ref->delcmd = रिक्त;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((वर्ण *)ref);
	ref = next;
    }
}

    शून्य
tcl_buffer_free(buf_T *buf)
{
    संरचना ref *reflist;

#ifdef DYNAMIC_TCL
    सचे (!stubs_initialized)	// Not using Tcl, nothing to do.
	पटिनिवत्त;
#endif

    reflist = (संरचना ref *)(buf->b_tcl_ref);
    सचे (reflist != &refsdeleted)
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
    सचे (!stubs_initialized)	// Not using Tcl, nothing to do.
	पटिनिवत्त;
#endif

    reflist = (संरचना ref*)(win->w_tcl_ref);
    सचे (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (शून्य *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = रिक्त;
    }
}

// The End
