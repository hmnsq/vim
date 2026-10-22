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

#অন্তর্ভাব "vim.h"
#undef EXTERN			// tcl.h defines it too

#ifdef DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#endif

#অন্তর্ভাব <tcl.h>
#অন্তর্ভাব <string.h>

প্রকারনির্দেশ সংরচনা
{
    Tcl_Interp *interp;
    পূর্ণাঙ্ক exitvalue;
    পূর্ণাঙ্ক range_start, range_end;
    পূর্ণাঙ্ক lbase;
    বর্ণ *curbuf, *curwin;
} tcl_info;

স্থির tcl_info tclinfo = { রিক্ত, 0, 0, 0, 0, রিক্ত, রিক্ত };

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
সংরচনা ref
{
    সংরচনা ref	*next;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    শূন্য	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
স্থির বর্ণ * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
স্থির বর্ণ * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
স্থির পূর্ণাঙ্ক tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, সংরচনা ref *reflist, শূন্য *vimobj, Tcl_Obj *delcmd));
স্থির পূর্ণাঙ্ক tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, পূর্ণাঙ্ক *মান_P, buf_T *buf));
স্থির win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
স্থির পূর্ণাঙ্ক tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, পূর্ণাঙ্ক objc, Tcl_Obj *CONST objv[], পূর্ণাঙ্ক objn));
স্থির পূর্ণাঙ্ক tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, পূর্ণাঙ্ক objc, Tcl_Obj *CONST objv[], পূর্ণাঙ্ক objn));
স্থির পূর্ণাঙ্ক tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, পূর্ণাঙ্ক objc, Tcl_Obj *CONST objv[], পূর্ণাঙ্ক objn));
স্থির শূন্য tcldelthisinterp _ANSI_ARGS_ ((শূন্য));

স্থির পূর্ণাঙ্ক vimerror _ANSI_ARGS_((Tcl_Interp *interp));
স্থির শূন্য tclmsg _ANSI_ARGS_((বর্ণ *text));
স্থির শূন্য tclerrmsg _ANSI_ARGS_((বর্ণ *text));
স্থির শূন্য tclupdatevars _ANSI_ARGS_((শূন্য));

স্থির সংরচনা ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#যদি defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
প্রকারনির্দেশ পূর্ণাঙ্ক HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define প্রতীক_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# অন্যথা
#  include <dlfcn.h>
#  define HANDLE শূন্য*
#  define TCL_PROC শূন্য*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define প্রতীক_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
স্থির HANDLE hTclLib = রিক্ত;
Tcl_Interp* (*dll_Tcl_CreateInterp)(শূন্য);
শূন্য (*dll_Tcl_FindExecutable)(ধ্রুব শূন্য *);

/*
 * Table of name to function pointer of tcl.
 */
স্থির সংরচনা {
    বর্ণ* name;
    TCL_PROC* ptr;
} tcl_funcname_table[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_FindExecutable},
    {রিক্ত, রিক্ত},
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
    স্থির পূর্ণাঙ্ক
tcl_runtime_link_init(বর্ণ *libname, পূর্ণাঙ্ক verbose)
{
    পূর্ণাঙ্ক i;

    যদি (hTclLib)
	প্রত্যাগম OK;
    যদি (!(hTclLib = load_dll(libname)))
    {
	যদি (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	প্রত্যাগম FAIL;
    }
    প্রতি (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	যদি (!(*tcl_funcname_table[i].ptr = প্রতীক_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = রিক্ত;
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    প্রত্যাগম FAIL;
	}
    }
    প্রত্যাগম OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
স্থির বর্ণ *find_executable_arg = রিক্ত;
#endif

    শূন্য
vim_tcl_init(বর্ণ *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#অন্যথা
    Tcl_FindExecutable(arg);
#endif
}

#যদি defined(DYNAMIC_TCL)

স্থির পূর্ণাঙ্ক stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    পূর্ণাঙ্ক
tcl_enabled(পূর্ণাঙ্ক verbose)
{
    যদি (!stubs_initialized && find_executable_arg != রিক্ত
	    && tcl_runtime_link_init((বর্ণ *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	যদি ((interp = dll_Tcl_CreateInterp()) != রিক্ত)
	{
	    যদি (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != রিক্ত)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    প্রত্যাগম stubs_initialized;
}
#endif

#যদি defined(EXITFREE)
/*
 * Called once when exiting.
 */
    শূন্য
vim_tcl_finalize(শূন্য)
{
# ifdef DYNAMIC_TCL
    যদি (stubs_initialized)
# endif
	Tcl_Finalize();
}
#endif

    শূন্য
tcl_end(শূন্য)
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
    স্থির পূর্ণাঙ্ক
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    পূর্ণাঙ্ক objc,
    Tcl_Obj *CONST objv[])
{
    পূর্ণাঙ্ক মান = 0;

    বিকল্প (objc)
    {
	পক্ষ 2:
	    যদি (Tcl_GetIntFromObj(interp, objv[1], &মান) != TCL_OK)
		ভঙ্গ;
	    // FALLTHROUGH
	পক্ষ 1:
	    tclinfo.exitvalue = মান;

	    Tcl_DeleteInterp(interp);
	    ভঙ্গ;
	স্বভাব:
	    Tcl_Wrong_অঙ্ক_Args(interp, 1, objv, "?returnCode?");
    }
    প্রত্যাগম TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    স্থির পূর্ণাঙ্ক
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    পূর্ণাঙ্ক objc,
    Tcl_Obj *CONST objv[])
{
    যদি (objc != 1)
    {
	Tcl_Wrong_অঙ্ক_Args(interp, 1, objv, রিক্ত);
	প্রত্যাগম TCL_ERROR;
    }
    vim_beep(BO_LANG);
    প্রত্যাগম TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    স্থির পূর্ণাঙ্ক
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    পূর্ণাঙ্ক objc,
    Tcl_Obj *CONST objv[])
{
    বর্ণ	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    পূর্ণাঙ্ক		err, n, idx;
    গণনা {BCMD_EXISTS, BCMD_LIST};
    স্থির CONST84 বর্ণ *bcmdoptions[] =
    {
	"exists", "list", (বর্ণ *)0
    };

    যদি (objc < 2)
    {
	Tcl_Wrong_অঙ্ক_Args(interp, 1, objv, "option");
	প্রত্যাগম TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    যদি (err == TCL_OK)
    {
	যদি (objc != 2)
	{
	    Tcl_Wrong_অঙ্ক_Args(interp, 1, objv, "bufNumber");
	    প্রত্যাগম TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    যদি (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		যদি (name == রিক্ত)
		    প্রত্যাগম TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		প্রত্যাগম TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	প্রত্যাগম TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    যদি (err != TCL_OK)
	প্রত্যাগম err;
    বিকল্প (idx)
    {
	পক্ষ BCMD_LIST:
	    যদি (objc != 2)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, "");
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		যদি (name == রিক্ত)
		{
		    err = TCL_ERROR;
		    ভঙ্গ;
		}
		Tcl_AppendElement(interp, name);
	    }
	    ভঙ্গ;

	পক্ষ BCMD_EXISTS:
	    যদি (objc != 3)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    যদি (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != রিক্ত);
		Tcl_SetObjResult(interp, resobj);
	    }
	    ভঙ্গ;

	স্বভাব:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    প্রত্যাগম err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    স্থির পূর্ণাঙ্ক
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    পূর্ণাঙ্ক		objc,
    Tcl_Obj	*CONST objv[])
{
    বর্ণ	*what, *string;
    win_T	*win;

    যদি (objc != 2)
    {
	Tcl_Wrong_অঙ্ক_Args(interp, 1, objv, "option");
	প্রত্যাগম TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], রিক্ত);
    যদি (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    যদি (string == রিক্ত)
		প্রত্যাগম TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	প্রত্যাগম TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    প্রত্যাগম TCL_ERROR;
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
    স্থির পূর্ণাঙ্ক
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    পূর্ণাঙ্ক		objc,
    Tcl_Obj	*CONST objv[])
{
    পূর্ণাঙ্ক		opt, err, idx, flags;
    পূর্ণাঙ্ক		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    বর্ণ	*সারি;

    গণনা
    {
	BUF_APPEND, BUF_COMMAND, BUF_গুনতি, BUF_DELCMD, BUF_DELETE, BUF_রাশিমালা,
	BUF_GET, BUF_ঢোকাও, BUF_LAST, BUF_MARK, BUF_NAME, BUF_সংখ্যা,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    স্থির CONST84 বর্ণ *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (বর্ণ *)0
    };

    যদি (objc < 2)
    {
	Tcl_Wrong_অঙ্ক_Args(interp, 1, objv, "option ?arg ...?");
	প্রত্যাগম TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    যদি (err != TCL_OK)
	প্রত্যাগম err;

    buf = (buf_T *)((সংরচনা ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    বিকল্প (idx)
    {
	পক্ষ BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ভঙ্গ;

	পক্ষ BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ভঙ্গ;

	পক্ষ BUF_রাশিমালা:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ভঙ্গ;

	পক্ষ BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    যদি (objc != 2)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, রিক্ত);
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    যদি (buf->b_ffname)
		Tcl_SetResult(interp, (বর্ণ *)buf->b_ffname, TCL_VOLATILE);
	    অন্যথা
		Tcl_SetResult(interp, "", TCL_STATIC);
	    ভঙ্গ;

	পক্ষ BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	পক্ষ BUF_গুনতি:
	    /*
	     * Get number of lines in buffer.
	     */
	    যদি (objc != 2)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, রিক্ত);
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    val1 = (পূর্ণাঙ্ক)buf->b_ml.ml_সারি_গুনতি;
	    যদি (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    ভঙ্গ;

	পক্ষ BUF_সংখ্যা:
	    /*
	     * Get buffer's number.
	     */
	    যদি (objc != 2)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, রিক্ত);
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    resobj = Tcl_NewIntObj((পূর্ণাঙ্ক)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    ভঙ্গ;

	পক্ষ BUF_GET:
	    যদি (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    যদি (err != TCL_OK)
		ভঙ্গ;
	    যদি (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		যদি (err != TCL_OK)
		    ভঙ্গ;
		যদি (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		প্রতি (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    সারি = (বর্ণ *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    যদি (সারি)
			Tcl_AppendElement(interp, সারি);
		    অন্যথা
			err = TCL_ERROR;
		}
	    }
	    অন্যথা
	    {  // objc == 3
		সারি = (বর্ণ *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, সারি, TCL_VOLATILE);
	    }
	    ভঙ্গ;

	পক্ষ BUF_SET:
	    যদি (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    যদি (err != TCL_OK)
		প্রত্যাগম TCL_ERROR;
	    যদি (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		সারি = Tcl_GetStringFromObj(objv[3], রিক্ত);
		যদি (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		অন্যথা
		যদি (ml_replace((linenr_T)val1, (char_u *)সারি, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		অন্যথা
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		ভঙ্গ;
	    }
	    অন্যথা
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		পূর্ণাঙ্ক	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		যদি (err != TCL_OK)
		    ভঙ্গ;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		যদি (err != TCL_OK)
		    ভঙ্গ;
		যদি (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		যদি (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    ভঙ্গ;
		}
		flags |= FL_UPDATE_CURBUF;

		প্রতি (i = 0; i < lc && n <= val2; i++)
		{
		    সারি = Tcl_GetStringFromObj(lv[i], রিক্ত);
		    যদি (ml_replace((linenr_T)n, (char_u *)সারি, TRUE) != OK)
			গচ্ছ setListError;
		    ++n;
		}
		যদি (i < lc)
		{
		    // append lines
		    কুরু
		    {
			সারি = Tcl_GetStringFromObj(lv[i], রিক্ত);
			যদি (ml_append((linenr_T)(n - 1),
					      (char_u *)সারি, 0, FALSE) != OK)
			    গচ্ছ setListError;
			++n;
			++i;
		    } যাবত্ (i < lc);
		}
		অন্যথা যদি (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    কুরু
		    {
			যদি (ml_delete((linenr_T)i) != OK)
			    গচ্ছ setListError;
			++n;
		    } যাবত্ (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (long)MAXLNUM,
								    (long)lc);
		changed_সারিগুলি((linenr_T)val1, 0, (linenr_T)val2 + 1, (long)lc);
		ভঙ্গ;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ভঙ্গ;

	পক্ষ BUF_DELETE:
	    যদি (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    যদি (err != TCL_OK)
		ভঙ্গ;
	    val2 = val1;
	    যদি (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		যদি (err != TCL_OK)
		    প্রত্যাগম err;
		যদি (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    যদি (u_savedel((linenr_T)val1, (long)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    প্রতি (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		যদি (err != TCL_OK)
		    ভঙ্গ;
	    }
	    যদি (i > 0)
		deleted_সারিগুলি_mark((linenr_T)val1, (long)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    ভঙ্গ;

	পক্ষ BUF_MARK:
	    যদি (objc != 3)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, "markName");
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    সারি = Tcl_GetStringFromObj(objv[2], রিক্ত);

	    pos = রিক্ত;
	    যদি (সারি[0] != '\0'  &&  সারি[1] == '\0')
		pos = getmark(সারি[0], FALSE);
	    যদি (pos == রিক্ত)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    err = vimerror(interp);
	    যদি (err != TCL_OK)
		ভঙ্গ;
	    যদি (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    অন্যথা
	    {
		বর্ণ rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (পূর্ণাঙ্ক)row2tcl(pos->lnum), (পূর্ণাঙ্ক)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    ভঙ্গ;

	পক্ষ BUF_ঢোকাও:
	    opt = 1;
	    // fallthrough
	পক্ষ BUF_APPEND:
	    যদি (objc != 4)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    যদি (err != TCL_OK)
		ভঙ্গ;
	    যদি (opt)
		--val1;
	    যদি (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ভঙ্গ;
	    }

	    সারি = Tcl_GetStringFromObj(objv[3], রিক্ত);
	    যদি (ml_append((linenr_T)val1, (char_u *)সারি, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    appended_সারিগুলি_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    ভঙ্গ;

	পক্ষ BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    যদি (objc != 2)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, রিক্ত);
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		যদি (win->w_buffer == buf)
		{
		    সারি = tclgetwindow(interp, win);
		    যদি (সারি != রিক্ত)
			Tcl_AppendElement(interp, সারি);
		    অন্যথা
		    {
			err = TCL_ERROR;
			ভঙ্গ;
		    }
		}
	    }
	    ভঙ্গ;

	পক্ষ BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    যদি (objc != 3)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (শূন্য *)buf, objv[2]);
	    ভঙ্গ;

	স্বভাব:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    যদি (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    যদি (flags & FL_ADJUST_CURSOR)
	check_cursor();
    যদি (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    প্রত্যাগম err;
}

/*
 * This function implements the window commands.
 */
    স্থির পূর্ণাঙ্ক
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    পূর্ণাঙ্ক		objc,
    Tcl_Obj	*CONST objv[])
{
    পূর্ণাঙ্ক		err, idx, flags;
    পূর্ণাঙ্ক		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    বর্ণ	*str;

    গণনা
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_রাশিমালা,
	WIN_HEIGHT, WIN_OPTION
    };
    স্থির CONST84 বর্ণ *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (বর্ণ *)0
    };

    যদি (objc < 2)
    {
	Tcl_Wrong_অঙ্ক_Args(interp, 1, objv, "option ?arg ...?");
	প্রত্যাগম TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    যদি (err != TCL_OK)
	প্রত্যাগম TCL_ERROR;

    win = (win_T *)((সংরচনা ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    বিকল্প (idx)
    {
	পক্ষ WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ভঙ্গ;

	পক্ষ WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ভঙ্গ;

	পক্ষ WIN_রাশিমালা:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ভঙ্গ;

	পক্ষ WIN_HEIGHT:
	    যদি (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		যদি (err != TCL_OK)
		    ভঙ্গ;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		যদি (err != TCL_OK)
		    ভঙ্গ;
	    }
	    অন্যথা
	    যদি (objc != 2)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		ভঙ্গ;
	    }

	    resobj = Tcl_NewIntObj((পূর্ণাঙ্ক)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    ভঙ্গ;

	পক্ষ WIN_BUFFER:
	    যদি (objc != 2)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, রিক্ত);
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    যদি (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    অন্যথা
		err = TCL_ERROR;
	    ভঙ্গ;

	পক্ষ WIN_DELCMD:
	    যদি (objc != 3)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (শূন্য *)win, objv[2]);
	    ভঙ্গ;

	পক্ষ WIN_CURSOR:
	    যদি (objc > 4)
	    {
		Tcl_Wrong_অঙ্ক_Args(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    যদি (objc == 2)
	    {
		বর্ণ buf[64];

		sprintf(buf, _("row %d column %d"), (পূর্ণাঙ্ক)row2tcl(win->w_cursor.lnum), (পূর্ণাঙ্ক)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		ভঙ্গ;
	    }
	    অন্যথা যদি (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_বার্তা);
		যদি (var == রিক্ত)
		{
		    err = TCL_ERROR;
		    ভঙ্গ;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		যদি (err != TCL_OK)
		    ভঙ্গ;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_বার্তা);
		যদি (var == রিক্ত)
		{
		    err = TCL_ERROR;
		    ভঙ্গ;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		যদি (err != TCL_OK)
		    ভঙ্গ;
	    }
	    অন্যথা
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		যদি (err != TCL_OK)
		    ভঙ্গ;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		যদি (err != TCL_OK)
		    ভঙ্গ;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    ভঙ্গ;

	স্বভাব:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    ভঙ্গ;
    }

    curwin = savewin;
    curbuf = savebuf;
    যদি (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    প্রত্যাগম err;
}


    স্থির পূর্ণাঙ্ক
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    পূর্ণাঙ্ক		objc,
    Tcl_Obj	*CONST objv[])
{
    পূর্ণাঙ্ক		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    প্রত্যাগম err;
}

    স্থির পূর্ণাঙ্ক
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    পূর্ণাঙ্ক		objc,
    Tcl_Obj	*CONST objv[])
{
    পূর্ণাঙ্ক		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    প্রত্যাগম err;
}

    স্থির পূর্ণাঙ্ক
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    পূর্ণাঙ্ক		objc,
    Tcl_Obj	*CONST objv[])
{
    প্রত্যাগম tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    স্থির পূর্ণাঙ্ক
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    পূর্ণাঙ্ক		*মান_P,
    buf_T	*buf)
{
    পূর্ণাঙ্ক err, i;

    গণনা { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_শীর্ষ };

    স্থির CONST84 বর্ণ *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (বর্ণ *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    যদি (err == TCL_OK)
    {
	বিকল্প (i)
	{
	    পক্ষ LN_BEGIN:
	    পক্ষ LN_FIRST:
	    পক্ষ LN_START:
	    পক্ষ LN_শীর্ষ:
		*মান_P = 1;
		ভঙ্গ;
	    পক্ষ LN_BOTTOM:
	    পক্ষ LN_END:
	    পক্ষ LN_LAST:
		*মান_P = buf->b_ml.ml_সারি_গুনতি;
		ভঙ্গ;
	}
	প্রত্যাগম TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    যদি (err != TCL_OK)
	প্রত্যাগম err;
    i = row2vim(i);
    যদি (i < 1  ||  i > buf->b_ml.ml_সারি_গুনতি)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	প্রত্যাগম TCL_ERROR;
    }
    *মান_P = i;
    প্রত্যাগম TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    স্থির win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	যদি (win->w_buffer == buf)
	    প্রত্যাগম win;
    }
    প্রত্যাগম curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    স্থির পূর্ণাঙ্ক
tcldoexcommand(
    Tcl_Interp	*interp,
    পূর্ণাঙ্ক		objc,
    Tcl_Obj	*CONST objv[],
    পূর্ণাঙ্ক		objn)
{
    tcl_info	saveinfo;
    পূর্ণাঙ্ক		err, flag, nobjs;
    বর্ণ	*arg;

    nobjs = objc - objn;
    যদি (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_অঙ্ক_Args(interp, objn, objv, "?-quiet? exCommand");
	প্রত্যাগম TCL_ERROR;
    }

    flag = 0;
    যদি (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], রিক্ত);
	যদি (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	অন্যথা
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (বর্ণ *)0);
	    প্রত্যাগম TCL_ERROR;
	}
	++objn;
    }

    memcpy(&saveinfo, &tclinfo, পরিমাণ(tcl_info));
    tclinfo.interp = রিক্ত;
    tclinfo.curwin = রিক্ত;
    tclinfo.curbuf = রিক্ত;

    arg = Tcl_GetStringFromObj(objv[objn], রিক্ত);
    যদি (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    যদি (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    যদি (tclinfo.interp)
	tcldelthisinterp();
    memcpy(&tclinfo, &saveinfo, পরিমাণ(tcl_info));
    tclupdatevars();

    প্রত্যাগম err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    স্থির পূর্ণাঙ্ক
tclsetoption(
    Tcl_Interp	*interp,
    পূর্ণাঙ্ক		objc,
    Tcl_Obj	*CONST objv[],
    পূর্ণাঙ্ক		objn)
{
    পূর্ণাঙ্ক		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    long	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    গণনা { OPT_OFF, OPT_ON, OPT_TOGGLE };
    স্থির CONST84 বর্ণ *optkw[] = { "off", "on", "toggle", (বর্ণ *)0 };

    nobjs = objc - objn;
    যদি (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_অঙ্ক_Args(interp, objn, objv, "vimOption ?value?");
	প্রত্যাগম TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], রিক্ত);
    ++objn;
    gov = get_option_মান(option, &lval, &sval, রিক্ত, 0);
    err = TCL_OK;
    বিকল্প (gov)
    {
	পক্ষ gov_string:
	    Tcl_SetResult(interp, (বর্ণ *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    ভঙ্গ;
	পক্ষ gov_bool:
	পক্ষ gov_সংখ্যা:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    ভঙ্গ;
	স্বভাব:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    প্রত্যাগম TCL_ERROR;
    }
    যদি (nobjs == 2)
    {
	যদি (gov != gov_string)
	{
	    sval = রিক্ত;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    যদি (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    অন্যথা
	    {
		বিকল্প (idx)
		{
		    পক্ষ OPT_ON:
			lval = 1;
			ভঙ্গ;
		    পক্ষ OPT_OFF:
			lval = 0;
			ভঙ্গ;
		    পক্ষ OPT_TOGGLE:
			lval = !lval;
			ভঙ্গ;
		}
	    }
	}
	অন্যথা
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], রিক্ত);
	যদি (err == TCL_OK)
	{
	    set_option_মান_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    প্রত্যাগম err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    স্থির পূর্ণাঙ্ক
tclvimexpr(
    Tcl_Interp	*interp,
    পূর্ণাঙ্ক		objc,
    Tcl_Obj	*CONST objv[],
    পূর্ণাঙ্ক		objn)
{
#ifdef FEAT_EVAL
    বর্ণ	*রাশিমালা, *str;
#endif
    পূর্ণাঙ্ক		err;

    যদি (objc - objn != 1)
    {
	Tcl_Wrong_অঙ্ক_Args(interp, objn, objv, "vimExpr");
	প্রত্যাগম TCL_ERROR;
    }

#ifdef FEAT_EVAL
    রাশিমালা = Tcl_GetStringFromObj(objv[objn], রিক্ত);
    str = (বর্ণ *)eval_to_string((char_u *)রাশিমালা, TRUE, FALSE);
    যদি (str == রিক্ত)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    অন্যথা
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#অন্যথা
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    প্রত্যাগম err;
}

/*
 * Check for internal vim errors.
 */
    স্থির পূর্ণাঙ্ক
vimerror(Tcl_Interp *interp)
{
    যদি (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	প্রত্যাগম TCL_ERROR;
    }
    অন্যথা যদি (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	প্রত্যাগম TCL_ERROR;
    }
    প্রত্যাগম TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    স্থির শূন্য
delref(ClientData cref)
{
    সংরচনা ref *ref = (সংরচনা ref *)cref;

    যদি (ref->delcmd)
    {
	Tcl_Decr_Ref_গুনতি(ref->delcmd);
	ref->delcmd = রিক্ত;
    }
    ref->interp = রিক্ত;
}

    স্থির বর্ণ *
tclgetref(
    Tcl_Interp	*interp,
    শূন্য	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    বর্ণ	*prefix,	// "win" or "buf"
    শূন্য	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    সংরচনা ref *ref, *unused = রিক্ত;
    স্থির বর্ণ name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (সংরচনা ref *)(*refstartP);
    যদি (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	প্রত্যাগম রিক্ত;
    }

    যাবত্ (ref != রিক্ত)
    {
	যদি (ref->interp == interp)
	    ভঙ্গ;
	যদি (ref->interp == রিক্ত)
	    unused = ref;
	ref = ref->next;
    }

    যদি (ref)
	vim_snprintf(name, পরিমাণ(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    অন্যথা
    {
	যদি (unused)
	    ref = unused;
	অন্যথা
	{
	    ref = (সংরচনা ref *)Tcl_Alloc(পরিমাণ(সংরচনা ref));
	    ref->interp = রিক্ত;
	    ref->next = (সংরচনা ref *)(*refstartP);
	    (*refstartP) = (শূন্য *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, পরিমাণ(name), "::vim::%s_%lx",
					       prefix, (unsigned long)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	যদি (!cmd)
	    প্রত্যাগম রিক্ত;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = রিক্ত;
	ref->vimobj = vimobj;
    }
    প্রত্যাগম name;
}

    স্থির বর্ণ *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    প্রত্যাগম tclgetref(interp, &(win->w_tcl_ref), "win", (শূন্য *)win, winselfcmd);
}

    স্থির বর্ণ *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    প্রত্যাগম tclgetref(interp, &(buf->b_tcl_ref), "buf", (শূন্য *)buf, bufselfcmd);
}

    স্থির পূর্ণাঙ্ক
tclsetdelcmd(
    Tcl_Interp	*interp,
    সংরচনা ref	*reflist,
    শূন্য	*vimobj,
    Tcl_Obj	*delcmd)
{
    যদি (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	প্রত্যাগম TCL_ERROR;
    }

    যাবত্ (reflist != রিক্ত)
    {
	যদি (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    যদি (reflist->delcmd)
		Tcl_Decr_Ref_গুনতি(reflist->delcmd);
	    Tcl_Incr_Ref_গুনতি(delcmd);
	    reflist->delcmd = delcmd;
	    প্রত্যাগম TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    প্রত্যাগম TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    স্থির পূর্ণাঙ্ক
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    পূর্ণাঙ্ক		err = 0;

    // currently does nothing

    যদি (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    প্রত্যাগম err;
}

    স্থির পূর্ণাঙ্ক
tcl_channel_প্রবেশ(
    ClientData	instance UNUSED,
    বর্ণ	*buf UNUSED,
    পূর্ণাঙ্ক		bufsiz UNUSED,
    পূর্ণাঙ্ক		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    যদি (errptr)
	*errptr = EINVAL;
    প্রত্যাগম -1;
}

    স্থির পূর্ণাঙ্ক
tcl_channel_output(
    ClientData	instance,
    ধ্রুব বর্ণ	*buf,
    পূর্ণাঙ্ক		bufsiz,
    পূর্ণাঙ্ক		*errptr)
{
    char_u	*str;
    পূর্ণাঙ্ক		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    যদি (!str)
    {
	Tcl_SetErrno(ENOMEM);
	যদি (errptr)
	    *errptr = ENOMEM;
	প্রত্যাগম -1;
    }

    result = bufsiz;
    যদি (instance == VIMOUT)
	tclmsg((বর্ণ *)str);
    অন্যথা
    যদি (instance == VIMERR)
	tclerrmsg((বর্ণ *)str);
    অন্যথা
    {
	Tcl_SetErrno(EBADF);
	যদি (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    প্রত্যাগম result;
}

    স্থির শূন্য
tcl_channel_watch(ClientData instance UNUSED, পূর্ণাঙ্ক mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    স্থির পূর্ণাঙ্ক
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    পূর্ণাঙ্ক		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    প্রত্যাগম EINVAL;
}


স্থির Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_VERSION_2, // version
    tcl_channel_close,	// closeProc
    tcl_channel_প্রবেশ,	// inputProc
    tcl_channel_output,	// outputProc
    রিক্ত,		// seekProc
    রিক্ত,		// setOptionProc
    রিক্ত,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    রিক্ত,		// close2Proc
    রিক্ত,		// blockModeProc
#ifdef TCL_CHANNEL_VERSION_2
    রিক্ত,		// flushProc
    রিক্ত,		// handlerProc
#endif
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ifdef TCL_CHANNEL_VERSION_3
    রিক্ত,		// wideSeekProc
#endif
#ifdef TCL_CHANNEL_VERSION_4
    রিক্ত,		// threadActionProc
#endif
#ifdef TCL_CHANNEL_VERSION_5
    রিক্ত		// truncateProc
#endif
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    স্থির শূন্য
tclupdatevars(শূন্য)
{
    বর্ণ varname[VARNAME_SIZE];	// must be writeable
    বর্ণ *name;

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


    স্থির পূর্ণাঙ্ক
tclinit(exarg_T *eap)
{
    বর্ণ varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    বর্ণ *name;

#ifdef DYNAMIC_TCL
    যদি (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	প্রত্যাগম FAIL;
    }
#endif

    যদি (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	স্থির Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_SetStdChannel(ch1, TCL_STDOUT);
	Tcl_SetStdChannel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	যদি (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    প্রত্যাগম FAIL;
	}
#যদি 0
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
	    (ClientData)রিক্ত, (Tcl_CmdDeleteProc *)রিক্ত);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (ClientData)রিক্ত, (Tcl_CmdDeleteProc *)রিক্ত);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (ClientData)রিক্ত, (Tcl_CmdDeleteProc *)রিক্ত);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (ClientData)রিক্ত, (Tcl_CmdDeleteProc *)রিক্ত);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (ClientData)রিক্ত, (Tcl_CmdDeleteProc *)রিক্ত);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (ClientData)রিক্ত, (Tcl_CmdDeleteProc *)রিক্ত);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (ClientData)রিক্ত, (Tcl_CmdDeleteProc *)রিক্ত);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (বর্ণ *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (বর্ণ *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (বর্ণ *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (বর্ণ *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (বর্ণ *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (বর্ণ *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    অন্যথা
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    প্রত্যাগম OK;
}

    স্থির শূন্য
tclerrmsg(বর্ণ *text)
{
    বর্ণ *next;

    যাবত্ ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    যদি (*text)
	emsg(text);
}

    স্থির শূন্য
tclmsg(বর্ণ *text)
{
    বর্ণ *next;

    যাবত্ ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	বার্তা(text);
	text = next;
    }
    যদি (*text)
	বার্তা(text);
}

    স্থির শূন্য
tcldelthisinterp(শূন্য)
{
    যদি (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = রিক্ত;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = রিক্ত;
}

    স্থির পূর্ণাঙ্ক
tclexit(পূর্ণাঙ্ক error)
{
    পূর্ণাঙ্ক newerr = OK;

    যদি (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#যদি (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	বর্ণ buf[50];

	sprintf(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	যদি (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	অন্যথা
	    newerr = FAIL;

	tcldelthisinterp();
    }
    অন্যথা
    {
	বর্ণ *result;

	result = (বর্ণ *)Tcl_GetStringResult(tclinfo.interp);
	যদি (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	অন্যথা
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    প্রত্যাগম newerr;
}

/*
 * ":tcl"
 */
    শূন্য
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    পূর্ণাঙ্ক		err;

    script = script_get(eap, eap->arg);
    যদি (!eap->skip)
    {
	err = tclinit(eap);
	যদি (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    যদি (script == রিক্ত)
		err = Tcl_Eval(tclinfo.interp, (বর্ণ *)eap->arg);
	    অন্যথা
		err = Tcl_Eval(tclinfo.interp, (বর্ণ *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    শূন্য
ex_tclfile(exarg_T *eap)
{
    বর্ণ *file = (বর্ণ *)eap->arg;
    পূর্ণাঙ্ক err;

    err = tclinit(eap);
    যদি (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    শূন্য
ex_tcldo(exarg_T *eap)
{
    বর্ণ	*script, *সারি;
    পূর্ণাঙ্ক		err, rs, re, lnum;
    বর্ণ	var_lnum[VARNAME_SIZE]; // must be writeable memory
    বর্ণ	var_সারি[VARNAME_SIZE];
    linenr_T	first_সারি = 0;
    linenr_T	last_সারি = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (বর্ণ *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_সারি, VAR_CURLINE);

    err = tclinit(eap);
    যদি (err != OK)
	প্রত্যাগম;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (বর্ণ *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    যদি (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    যাবত্ (err == TCL_OK  &&  rs <= re)
    {
	যদি ((linenr_T)rs > curbuf->b_ml.ml_সারি_গুনতি)
	    ভঙ্গ;
	সারি = (বর্ণ *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	যদি (!সারি)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    ভঙ্গ;
	}
	Tcl_SetVar(tclinfo.interp, var_সারি, সারি, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	যদি (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#যদি (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_সারি_গুনতি)
	    ভঙ্গ;
	সারি = (বর্ণ *)Tcl_GetVar(tclinfo.interp, var_সারি, 0);
	যদি (সারি)
	{
	    যদি (ml_replace((linenr_T)rs, (char_u *)সারি, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		ভঙ্গ;
	    }
	    যদি (first_সারি == 0)
		first_সারি = rs;
	    last_সারি = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    যদি (first_সারি)
	changed_সারিগুলি(first_সারি, 0, last_সারি + 1, (long)0);

    Tcl_UnsetVar(tclinfo.interp, var_সারি, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    যদি (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (শূন্য)tclexit(err);
}

    স্থির শূন্য
tcldelallrefs(সংরচনা ref *ref)
{
    সংরচনা ref	*next;
    পূর্ণাঙ্ক		err;
    বর্ণ	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    যদি (exiting)
	প্রত্যাগম;
#endif

    যাবত্ (ref != রিক্ত)
    {
	next = ref->next;
	যদি (ref->interp)
	{
	    যদি (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		যদি (err != TCL_OK)
		{
		    result = (বর্ণ *)Tcl_GetStringResult(ref->interp);
		    যদি (result)
			tclerrmsg(result);
		}
		Tcl_Decr_Ref_গুনতি(ref->delcmd);
		ref->delcmd = রিক্ত;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((বর্ণ *)ref);
	ref = next;
    }
}

    শূন্য
tcl_buffer_free(buf_T *buf)
{
    সংরচনা ref *reflist;

#ifdef DYNAMIC_TCL
    যদি (!stubs_initialized)	// Not using Tcl, nothing to do.
	প্রত্যাগম;
#endif

    reflist = (সংরচনা ref *)(buf->b_tcl_ref);
    যদি (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (শূন্য *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = রিক্ত;
    }
}

    শূন্য
tcl_window_free(win_T *win)
{
    সংরচনা ref *reflist;

#ifdef DYNAMIC_TCL
    যদি (!stubs_initialized)	// Not using Tcl, nothing to do.
	প্রত্যাগম;
#endif

    reflist = (সংরচনা ref*)(win->w_tcl_ref);
    যদি (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (শূন্য *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = রিক্ত;
    }
}

// The End
