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

#ضمن "vim.h"
#undef EXTERN			// tcl.h defines it too

#ifdef DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#endif

#ضمن <tcl.h>
#ضمن <string.h>

تعريفنوع بنية
{
    Tcl_Interp *interp;
    صحيح exitvalue;
    صحيح range_start, range_end;
    صحيح lbase;
    حرف *curbuf, *curwin;
} tcl_info;

ساكن tcl_info tclinfo = { عدم, 0, 0, 0, 0, عدم, عدم };

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
بنية ref
{
    بنية ref	*next;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    فراغ	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
ساكن حرف * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
ساكن حرف * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
ساكن صحيح tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, بنية ref *reflist, فراغ *vimobj, Tcl_Obj *delcmd));
ساكن صحيح tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, صحيح *valueP, buf_T *buf));
ساكن win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
ساكن صحيح tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, صحيح objc, Tcl_Obj *CONST objv[], صحيح objn));
ساكن صحيح tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, صحيح objc, Tcl_Obj *CONST objv[], صحيح objn));
ساكن صحيح tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, صحيح objc, Tcl_Obj *CONST objv[], صحيح objn));
ساكن فراغ tcldelthisinterp _ANSI_ARGS_ ((فراغ));

ساكن صحيح vimerror _ANSI_ARGS_((Tcl_Interp *interp));
ساكن فراغ tclmsg _ANSI_ARGS_((حرف *text));
ساكن فراغ tclerrmsg _ANSI_ARGS_((حرف *text));
ساكن فراغ tclupdatevars _ANSI_ARGS_((فراغ));

ساكن بنية ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#ܐܢ defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
تعريفنوع صحيح HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define symbol_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# ܐܚܪܢܐ
#  include <dlfcn.h>
#  define HANDLE فراغ*
#  define TCL_PROC فراغ*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define symbol_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
ساكن HANDLE hTclLib = عدم;
Tcl_Interp* (*dll_Tcl_CreateInterp)(فراغ);
فراغ (*dll_Tcl_FindExecutable)(ثابت فراغ *);

/*
 * Table of name to function pointer of tcl.
 */
ساكن بنية {
    حرف* name;
    TCL_PROC* ptr;
} tcl_funcname_table[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_FindExecutable},
    {عدم, عدم},
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
    ساكن صحيح
tcl_runtime_link_init(حرف *libname, صحيح verbose)
{
    صحيح i;

    ܐܢ (hTclLib)
	ܦܢܐ OK;
    ܐܢ (!(hTclLib = load_dll(libname)))
    {
	ܐܢ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ܦܢܐ FAIL;
    }
    لكل (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	ܐܢ (!(*tcl_funcname_table[i].ptr = symbol_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = عدم;
	    ܐܢ (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    ܦܢܐ FAIL;
	}
    }
    ܦܢܐ OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
ساكن حرف *find_executable_arg = عدم;
#endif

    فراغ
vim_tcl_init(حرف *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#ܐܚܪܢܐ
    Tcl_FindExecutable(arg);
#endif
}

#ܐܢ defined(DYNAMIC_TCL)

ساكن صحيح stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    صحيح
tcl_enabled(صحيح verbose)
{
    ܐܢ (!stubs_initialized && find_executable_arg != عدم
	    && tcl_runtime_link_init((حرف *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	ܐܢ ((interp = dll_Tcl_CreateInterp()) != عدم)
	{
	    ܐܢ (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != عدم)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    ܦܢܐ stubs_initialized;
}
#endif

#ܐܢ defined(EXITFREE)
/*
 * Called once when exiting.
 */
    فراغ
vim_tcl_finalize(فراغ)
{
# ifdef DYNAMIC_TCL
    ܐܢ (stubs_initialized)
# endif
	Tcl_Finalize();
}
#endif

    فراغ
tcl_end(فراغ)
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
    ساكن صحيح
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    صحيح objc,
    Tcl_Obj *CONST objv[])
{
    صحيح value = 0;

    بدل (objc)
    {
	حالة 2:
	    ܐܢ (Tcl_GetIntFromObj(interp, objv[1], &value) != TCL_OK)
		اكسر;
	    // FALLTHROUGH
	حالة 1:
	    tclinfo.exitvalue = value;

	    Tcl_DeleteInterp(interp);
	    اكسر;
	افتراضي:
	    Tcl_WrongNumArgs(interp, 1, objv, "?returnCode?");
    }
    ܦܢܐ TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    ساكن صحيح
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    صحيح objc,
    Tcl_Obj *CONST objv[])
{
    ܐܢ (objc != 1)
    {
	Tcl_WrongNumArgs(interp, 1, objv, عدم);
	ܦܢܐ TCL_ERROR;
    }
    vim_beep(BO_LANG);
    ܦܢܐ TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    ساكن صحيح
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    صحيح objc,
    Tcl_Obj *CONST objv[])
{
    حرف	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    صحيح		err, n, idx;
    تعداد {BCMD_EXISTS, BCMD_LIST};
    ساكن CONST84 حرف *bcmdoptions[] =
    {
	"exists", "list", (حرف *)0
    };

    ܐܢ (objc < 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option");
	ܦܢܐ TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    ܐܢ (err == TCL_OK)
    {
	ܐܢ (objc != 2)
	{
	    Tcl_WrongNumArgs(interp, 1, objv, "bufNumber");
	    ܦܢܐ TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    ܐܢ (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		ܐܢ (name == عدم)
		    ܦܢܐ TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		ܦܢܐ TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	ܦܢܐ TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    ܐܢ (err != TCL_OK)
	ܦܢܐ err;
    بدل (idx)
    {
	حالة BCMD_LIST:
	    ܐܢ (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "");
		err = TCL_ERROR;
		اكسر;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		ܐܢ (name == عدم)
		{
		    err = TCL_ERROR;
		    اكسر;
		}
		Tcl_AppendElement(interp, name);
	    }
	    اكسر;

	حالة BCMD_EXISTS:
	    ܐܢ (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    ܐܢ (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != عدم);
		Tcl_SetObjResult(interp, resobj);
	    }
	    اكسر;

	افتراضي:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    ܦܢܐ err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    ساكن صحيح
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    حرف	*what, *string;
    win_T	*win;

    ܐܢ (objc != 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option");
	ܦܢܐ TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], عدم);
    ܐܢ (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    ܐܢ (string == عدم)
		ܦܢܐ TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	ܦܢܐ TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    ܦܢܐ TCL_ERROR;
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
    ساكن صحيح
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحيح		opt, err, idx, flags;
    صحيح		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    حرف	*line;

    تعداد
    {
	BUF_APPEND, BUF_COMMAND, BUF_COUNT, BUF_DELCMD, BUF_DELETE, BUF_EXPR,
	BUF_GET, BUF_INSERT, BUF_LAST, BUF_MARK, BUF_NAME, BUF_NUMBER,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    ساكن CONST84 حرف *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (حرف *)0
    };

    ܐܢ (objc < 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
	ܦܢܐ TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    ܐܢ (err != TCL_OK)
	ܦܢܐ err;

    buf = (buf_T *)((بنية ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    بدل (idx)
    {
	حالة BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	حالة BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	حالة BUF_EXPR:
	    err = tclvimexpr(interp, objc, objv, 2);
	    اكسر;

	حالة BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    ܐܢ (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, عدم);
		err = TCL_ERROR;
		اكسر;
	    }
	    ܐܢ (buf->b_ffname)
		Tcl_SetResult(interp, (حرف *)buf->b_ffname, TCL_VOLATILE);
	    ܐܚܪܢܐ
		Tcl_SetResult(interp, "", TCL_STATIC);
	    اكسر;

	حالة BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	حالة BUF_COUNT:
	    /*
	     * Get number of lines in buffer.
	     */
	    ܐܢ (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, عدم);
		err = TCL_ERROR;
		اكسر;
	    }
	    val1 = (صحيح)buf->b_ml.ml_line_count;
	    ܐܢ (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    اكسر;

	حالة BUF_NUMBER:
	    /*
	     * Get buffer's number.
	     */
	    ܐܢ (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, عدم);
		err = TCL_ERROR;
		اكسر;
	    }
	    resobj = Tcl_NewIntObj((صحيح)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    اكسر;

	حالة BUF_GET:
	    ܐܢ (objc != 3 && objc != 4)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ܐܢ (err != TCL_OK)
		اكسر;
	    ܐܢ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ܐܢ (err != TCL_OK)
		    اكسر;
		ܐܢ (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		لكل (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    line = (حرف *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    ܐܢ (line)
			Tcl_AppendElement(interp, line);
		    ܐܚܪܢܐ
			err = TCL_ERROR;
		}
	    }
	    ܐܚܪܢܐ
	    {  // objc == 3
		line = (حرف *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, line, TCL_VOLATILE);
	    }
	    اكسر;

	حالة BUF_SET:
	    ܐܢ (objc != 4 && objc != 5)
	    {
		Tcl_WrongNumArgs(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ܐܢ (err != TCL_OK)
		ܦܢܐ TCL_ERROR;
	    ܐܢ (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		line = Tcl_GetStringFromObj(objv[3], عدم);
		ܐܢ (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ܐܚܪܢܐ
		ܐܢ (ml_replace((linenr_T)val1, (char_u *)line, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ܐܚܪܢܐ
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		اكسر;
	    }
	    ܐܚܪܢܐ
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		صحيح	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ܐܢ (err != TCL_OK)
		    اكسر;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		ܐܢ (err != TCL_OK)
		    اكسر;
		ܐܢ (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		ܐܢ (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    اكسر;
		}
		flags |= FL_UPDATE_CURBUF;

		لكل (i = 0; i < lc && n <= val2; i++)
		{
		    line = Tcl_GetStringFromObj(lv[i], عدم);
		    ܐܢ (ml_replace((linenr_T)n, (char_u *)line, TRUE) != OK)
			اذهب setListError;
		    ++n;
		}
		ܐܢ (i < lc)
		{
		    // append lines
		    ܥܒܕ
		    {
			line = Tcl_GetStringFromObj(lv[i], عدم);
			ܐܢ (ml_append((linenr_T)(n - 1),
					      (char_u *)line, 0, FALSE) != OK)
			    اذهب setListError;
			++n;
			++i;
		    } ܟܡܐ (i < lc);
		}
		ܐܚܪܢܐ ܐܢ (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    ܥܒܕ
		    {
			ܐܢ (ml_delete((linenr_T)i) != OK)
			    اذهب setListError;
			++n;
		    } ܟܡܐ (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (long)MAXLNUM,
								    (long)lc);
		changed_lines((linenr_T)val1, 0, (linenr_T)val2 + 1, (long)lc);
		اكسر;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    اكسر;

	حالة BUF_DELETE:
	    ܐܢ (objc != 3  &&  objc != 4)
	    {
		Tcl_WrongNumArgs(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ܐܢ (err != TCL_OK)
		اكسر;
	    val2 = val1;
	    ܐܢ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ܐܢ (err != TCL_OK)
		    ܦܢܐ err;
		ܐܢ (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    ܐܢ (u_savedel((linenr_T)val1, (long)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		اكسر;
	    }
	    لكل (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		ܐܢ (err != TCL_OK)
		    اكسر;
	    }
	    ܐܢ (i > 0)
		deleted_lines_mark((linenr_T)val1, (long)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    اكسر;

	حالة BUF_MARK:
	    ܐܢ (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "markName");
		err = TCL_ERROR;
		اكسر;
	    }
	    line = Tcl_GetStringFromObj(objv[2], عدم);

	    pos = عدم;
	    ܐܢ (line[0] != '\0'  &&  line[1] == '\0')
		pos = getmark(line[0], FALSE);
	    ܐܢ (pos == عدم)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		اكسر;
	    }
	    err = vimerror(interp);
	    ܐܢ (err != TCL_OK)
		اكسر;
	    ܐܢ (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ܐܚܪܢܐ
	    {
		حرف rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (صحيح)row2tcl(pos->lnum), (صحيح)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    اكسر;

	حالة BUF_INSERT:
	    opt = 1;
	    // fallthrough
	حالة BUF_APPEND:
	    ܐܢ (objc != 4)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ܐܢ (err != TCL_OK)
		اكسر;
	    ܐܢ (opt)
		--val1;
	    ܐܢ (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		اكسر;
	    }

	    line = Tcl_GetStringFromObj(objv[3], عدم);
	    ܐܢ (ml_append((linenr_T)val1, (char_u *)line, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		اكسر;
	    }
	    appended_lines_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	حالة BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    ܐܢ (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, عدم);
		err = TCL_ERROR;
		اكسر;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		ܐܢ (win->w_buffer == buf)
		{
		    line = tclgetwindow(interp, win);
		    ܐܢ (line != عدم)
			Tcl_AppendElement(interp, line);
		    ܐܚܪܢܐ
		    {
			err = TCL_ERROR;
			اكسر;
		    }
		}
	    }
	    اكسر;

	حالة BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    ܐܢ (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "command");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (فراغ *)buf, objv[2]);
	    اكسر;

	افتراضي:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    ܐܢ (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    ܐܢ (flags & FL_ADJUST_CURSOR)
	check_cursor();
    ܐܢ (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    ܦܢܐ err;
}

/*
 * This function implements the window commands.
 */
    ساكن صحيح
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحيح		err, idx, flags;
    صحيح		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    حرف	*str;

    تعداد
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_EXPR,
	WIN_HEIGHT, WIN_OPTION
    };
    ساكن CONST84 حرف *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (حرف *)0
    };

    ܐܢ (objc < 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
	ܦܢܐ TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    ܐܢ (err != TCL_OK)
	ܦܢܐ TCL_ERROR;

    win = (win_T *)((بنية ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    بدل (idx)
    {
	حالة WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	حالة WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	حالة WIN_EXPR:
	    err = tclvimexpr(interp, objc, objv, 2);
	    اكسر;

	حالة WIN_HEIGHT:
	    ܐܢ (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		ܐܢ (err != TCL_OK)
		    اكسر;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		ܐܢ (err != TCL_OK)
		    اكسر;
	    }
	    ܐܚܪܢܐ
	    ܐܢ (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		اكسر;
	    }

	    resobj = Tcl_NewIntObj((صحيح)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    اكسر;

	حالة WIN_BUFFER:
	    ܐܢ (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, عدم);
		err = TCL_ERROR;
		اكسر;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    ܐܢ (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    ܐܚܪܢܐ
		err = TCL_ERROR;
	    اكسر;

	حالة WIN_DELCMD:
	    ܐܢ (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "command");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (فراغ *)win, objv[2]);
	    اكسر;

	حالة WIN_CURSOR:
	    ܐܢ (objc > 4)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		اكسر;
	    }
	    ܐܢ (objc == 2)
	    {
		حرف buf[64];

		sprintf(buf, _("row %d column %d"), (صحيح)row2tcl(win->w_cursor.lnum), (صحيح)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		اكسر;
	    }
	    ܐܚܪܢܐ ܐܢ (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_MSG);
		ܐܢ (var == عدم)
		{
		    err = TCL_ERROR;
		    اكسر;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		ܐܢ (err != TCL_OK)
		    اكسر;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_MSG);
		ܐܢ (var == عدم)
		{
		    err = TCL_ERROR;
		    اكسر;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		ܐܢ (err != TCL_OK)
		    اكسر;
	    }
	    ܐܚܪܢܐ
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		ܐܢ (err != TCL_OK)
		    اكسر;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		ܐܢ (err != TCL_OK)
		    اكسر;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	افتراضي:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    اكسر;
    }

    curwin = savewin;
    curbuf = savebuf;
    ܐܢ (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    ܦܢܐ err;
}


    ساكن صحيح
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحيح		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ܦܢܐ err;
}

    ساكن صحيح
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحيح		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ܦܢܐ err;
}

    ساكن صحيح
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    ܦܢܐ tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    ساكن صحيح
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    صحيح		*valueP,
    buf_T	*buf)
{
    صحيح err, i;

    تعداد { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_TOP };

    ساكن CONST84 حرف *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (حرف *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    ܐܢ (err == TCL_OK)
    {
	بدل (i)
	{
	    حالة LN_BEGIN:
	    حالة LN_FIRST:
	    حالة LN_START:
	    حالة LN_TOP:
		*valueP = 1;
		اكسر;
	    حالة LN_BOTTOM:
	    حالة LN_END:
	    حالة LN_LAST:
		*valueP = buf->b_ml.ml_line_count;
		اكسر;
	}
	ܦܢܐ TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    ܐܢ (err != TCL_OK)
	ܦܢܐ err;
    i = row2vim(i);
    ܐܢ (i < 1  ||  i > buf->b_ml.ml_line_count)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	ܦܢܐ TCL_ERROR;
    }
    *valueP = i;
    ܦܢܐ TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    ساكن win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	ܐܢ (win->w_buffer == buf)
	    ܦܢܐ win;
    }
    ܦܢܐ curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    ساكن صحيح
tcldoexcommand(
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[],
    صحيح		objn)
{
    tcl_info	saveinfo;
    صحيح		err, flag, nobjs;
    حرف	*arg;

    nobjs = objc - objn;
    ܐܢ (nobjs < 1 || nobjs > 2)
    {
	Tcl_WrongNumArgs(interp, objn, objv, "?-quiet? exCommand");
	ܦܢܐ TCL_ERROR;
    }

    flag = 0;
    ܐܢ (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], عدم);
	ܐܢ (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	ܐܚܪܢܐ
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (حرف *)0);
	    ܦܢܐ TCL_ERROR;
	}
	++objn;
    }

    memcpy(&saveinfo, &tclinfo, حجم(tcl_info));
    tclinfo.interp = عدم;
    tclinfo.curwin = عدم;
    tclinfo.curbuf = عدم;

    arg = Tcl_GetStringFromObj(objv[objn], عدم);
    ܐܢ (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    ܐܢ (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    ܐܢ (tclinfo.interp)
	tcldelthisinterp();
    memcpy(&tclinfo, &saveinfo, حجم(tcl_info));
    tclupdatevars();

    ܦܢܐ err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    ساكن صحيح
tclsetoption(
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[],
    صحيح		objn)
{
    صحيح		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    long	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    تعداد { OPT_OFF, OPT_ON, OPT_TOGGLE };
    ساكن CONST84 حرف *optkw[] = { "off", "on", "toggle", (حرف *)0 };

    nobjs = objc - objn;
    ܐܢ (nobjs != 1 && nobjs != 2)
    {
	Tcl_WrongNumArgs(interp, objn, objv, "vimOption ?value?");
	ܦܢܐ TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], عدم);
    ++objn;
    gov = get_option_value(option, &lval, &sval, عدم, 0);
    err = TCL_OK;
    بدل (gov)
    {
	حالة gov_string:
	    Tcl_SetResult(interp, (حرف *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    اكسر;
	حالة gov_bool:
	حالة gov_number:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    اكسر;
	افتراضي:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    ܦܢܐ TCL_ERROR;
    }
    ܐܢ (nobjs == 2)
    {
	ܐܢ (gov != gov_string)
	{
	    sval = عدم;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    ܐܢ (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    ܐܚܪܢܐ
	    {
		بدل (idx)
		{
		    حالة OPT_ON:
			lval = 1;
			اكسر;
		    حالة OPT_OFF:
			lval = 0;
			اكسر;
		    حالة OPT_TOGGLE:
			lval = !lval;
			اكسر;
		}
	    }
	}
	ܐܚܪܢܐ
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], عدم);
	ܐܢ (err == TCL_OK)
	{
	    set_option_value_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    ܦܢܐ err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    ساكن صحيح
tclvimexpr(
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[],
    صحيح		objn)
{
#ifdef FEAT_EVAL
    حرف	*expr, *str;
#endif
    صحيح		err;

    ܐܢ (objc - objn != 1)
    {
	Tcl_WrongNumArgs(interp, objn, objv, "vimExpr");
	ܦܢܐ TCL_ERROR;
    }

#ifdef FEAT_EVAL
    expr = Tcl_GetStringFromObj(objv[objn], عدم);
    str = (حرف *)eval_to_string((char_u *)expr, TRUE, FALSE);
    ܐܢ (str == عدم)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    ܐܚܪܢܐ
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#ܐܚܪܢܐ
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    ܦܢܐ err;
}

/*
 * Check for internal vim errors.
 */
    ساكن صحيح
vimerror(Tcl_Interp *interp)
{
    ܐܢ (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	ܦܢܐ TCL_ERROR;
    }
    ܐܚܪܢܐ ܐܢ (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	ܦܢܐ TCL_ERROR;
    }
    ܦܢܐ TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    ساكن فراغ
delref(ClientData cref)
{
    بنية ref *ref = (بنية ref *)cref;

    ܐܢ (ref->delcmd)
    {
	Tcl_DecrRefCount(ref->delcmd);
	ref->delcmd = عدم;
    }
    ref->interp = عدم;
}

    ساكن حرف *
tclgetref(
    Tcl_Interp	*interp,
    فراغ	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    حرف	*prefix,	// "win" or "buf"
    فراغ	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    بنية ref *ref, *unused = عدم;
    ساكن حرف name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (بنية ref *)(*refstartP);
    ܐܢ (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	ܦܢܐ عدم;
    }

    ܟܡܐ (ref != عدم)
    {
	ܐܢ (ref->interp == interp)
	    اكسر;
	ܐܢ (ref->interp == عدم)
	    unused = ref;
	ref = ref->next;
    }

    ܐܢ (ref)
	vim_snprintf(name, حجم(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    ܐܚܪܢܐ
    {
	ܐܢ (unused)
	    ref = unused;
	ܐܚܪܢܐ
	{
	    ref = (بنية ref *)Tcl_Alloc(حجم(بنية ref));
	    ref->interp = عدم;
	    ref->next = (بنية ref *)(*refstartP);
	    (*refstartP) = (فراغ *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, حجم(name), "::vim::%s_%lx",
					       prefix, (unsigned long)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	ܐܢ (!cmd)
	    ܦܢܐ عدم;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = عدم;
	ref->vimobj = vimobj;
    }
    ܦܢܐ name;
}

    ساكن حرف *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    ܦܢܐ tclgetref(interp, &(win->w_tcl_ref), "win", (فراغ *)win, winselfcmd);
}

    ساكن حرف *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    ܦܢܐ tclgetref(interp, &(buf->b_tcl_ref), "buf", (فراغ *)buf, bufselfcmd);
}

    ساكن صحيح
tclsetdelcmd(
    Tcl_Interp	*interp,
    بنية ref	*reflist,
    فراغ	*vimobj,
    Tcl_Obj	*delcmd)
{
    ܐܢ (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	ܦܢܐ TCL_ERROR;
    }

    ܟܡܐ (reflist != عدم)
    {
	ܐܢ (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    ܐܢ (reflist->delcmd)
		Tcl_DecrRefCount(reflist->delcmd);
	    Tcl_IncrRefCount(delcmd);
	    reflist->delcmd = delcmd;
	    ܦܢܐ TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    ܦܢܐ TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    ساكن صحيح
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    صحيح		err = 0;

    // currently does nothing

    ܐܢ (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    ܦܢܐ err;
}

    ساكن صحيح
tcl_channel_input(
    ClientData	instance UNUSED,
    حرف	*buf UNUSED,
    صحيح		bufsiz UNUSED,
    صحيح		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    ܐܢ (errptr)
	*errptr = EINVAL;
    ܦܢܐ -1;
}

    ساكن صحيح
tcl_channel_output(
    ClientData	instance,
    ثابت حرف	*buf,
    صحيح		bufsiz,
    صحيح		*errptr)
{
    char_u	*str;
    صحيح		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    ܐܢ (!str)
    {
	Tcl_SetErrno(ENOMEM);
	ܐܢ (errptr)
	    *errptr = ENOMEM;
	ܦܢܐ -1;
    }

    result = bufsiz;
    ܐܢ (instance == VIMOUT)
	tclmsg((حرف *)str);
    ܐܚܪܢܐ
    ܐܢ (instance == VIMERR)
	tclerrmsg((حرف *)str);
    ܐܚܪܢܐ
    {
	Tcl_SetErrno(EBADF);
	ܐܢ (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    ܦܢܐ result;
}

    ساكن فراغ
tcl_channel_watch(ClientData instance UNUSED, صحيح mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    ساكن صحيح
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    صحيح		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    ܦܢܐ EINVAL;
}


ساكن Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_VERSION_2, // version
    tcl_channel_close,	// closeProc
    tcl_channel_input,	// inputProc
    tcl_channel_output,	// outputProc
    عدم,		// seekProc
    عدم,		// setOptionProc
    عدم,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    عدم,		// close2Proc
    عدم,		// blockModeProc
#ifdef TCL_CHANNEL_VERSION_2
    عدم,		// flushProc
    عدم,		// handlerProc
#endif
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ifdef TCL_CHANNEL_VERSION_3
    عدم,		// wideSeekProc
#endif
#ifdef TCL_CHANNEL_VERSION_4
    عدم,		// threadActionProc
#endif
#ifdef TCL_CHANNEL_VERSION_5
    عدم		// truncateProc
#endif
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    ساكن فراغ
tclupdatevars(فراغ)
{
    حرف varname[VARNAME_SIZE];	// must be writeable
    حرف *name;

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


    ساكن صحيح
tclinit(exarg_T *eap)
{
    حرف varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    حرف *name;

#ifdef DYNAMIC_TCL
    ܐܢ (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	ܦܢܐ FAIL;
    }
#endif

    ܐܢ (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	ساكن Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_SetStdChannel(ch1, TCL_STDOUT);
	Tcl_SetStdChannel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	ܐܢ (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    ܦܢܐ FAIL;
	}
#ܐܢ 0
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
	    (ClientData)عدم, (Tcl_CmdDeleteProc *)عدم);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (ClientData)عدم, (Tcl_CmdDeleteProc *)عدم);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (ClientData)عدم, (Tcl_CmdDeleteProc *)عدم);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (ClientData)عدم, (Tcl_CmdDeleteProc *)عدم);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (ClientData)عدم, (Tcl_CmdDeleteProc *)عدم);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (ClientData)عدم, (Tcl_CmdDeleteProc *)عدم);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (ClientData)عدم, (Tcl_CmdDeleteProc *)عدم);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    ܐܚܪܢܐ
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    ܦܢܐ OK;
}

    ساكن فراغ
tclerrmsg(حرف *text)
{
    حرف *next;

    ܟܡܐ ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    ܐܢ (*text)
	emsg(text);
}

    ساكن فراغ
tclmsg(حرف *text)
{
    حرف *next;

    ܟܡܐ ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	msg(text);
	text = next;
    }
    ܐܢ (*text)
	msg(text);
}

    ساكن فراغ
tcldelthisinterp(فراغ)
{
    ܐܢ (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = عدم;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = عدم;
}

    ساكن صحيح
tclexit(صحيح error)
{
    صحيح newerr = OK;

    ܐܢ (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#ܐܢ (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	حرف buf[50];

	sprintf(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	ܐܢ (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	ܐܚܪܢܐ
	    newerr = FAIL;

	tcldelthisinterp();
    }
    ܐܚܪܢܐ
    {
	حرف *result;

	result = (حرف *)Tcl_GetStringResult(tclinfo.interp);
	ܐܢ (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	ܐܚܪܢܐ
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    ܦܢܐ newerr;
}

/*
 * ":tcl"
 */
    فراغ
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    صحيح		err;

    script = script_get(eap, eap->arg);
    ܐܢ (!eap->skip)
    {
	err = tclinit(eap);
	ܐܢ (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    ܐܢ (script == عدم)
		err = Tcl_Eval(tclinfo.interp, (حرف *)eap->arg);
	    ܐܚܪܢܐ
		err = Tcl_Eval(tclinfo.interp, (حرف *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    فراغ
ex_tclfile(exarg_T *eap)
{
    حرف *file = (حرف *)eap->arg;
    صحيح err;

    err = tclinit(eap);
    ܐܢ (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    فراغ
ex_tcldo(exarg_T *eap)
{
    حرف	*script, *line;
    صحيح		err, rs, re, lnum;
    حرف	var_lnum[VARNAME_SIZE]; // must be writeable memory
    حرف	var_line[VARNAME_SIZE];
    linenr_T	first_line = 0;
    linenr_T	last_line = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (حرف *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_line, VAR_CURLINE);

    err = tclinit(eap);
    ܐܢ (err != OK)
	ܦܢܐ;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (حرف *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    ܐܢ (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    ܟܡܐ (err == TCL_OK  &&  rs <= re)
    {
	ܐܢ ((linenr_T)rs > curbuf->b_ml.ml_line_count)
	    اكسر;
	line = (حرف *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	ܐܢ (!line)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    اكسر;
	}
	Tcl_SetVar(tclinfo.interp, var_line, line, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	ܐܢ (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#ܐܢ (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_line_count)
	    اكسر;
	line = (حرف *)Tcl_GetVar(tclinfo.interp, var_line, 0);
	ܐܢ (line)
	{
	    ܐܢ (ml_replace((linenr_T)rs, (char_u *)line, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		اكسر;
	    }
	    ܐܢ (first_line == 0)
		first_line = rs;
	    last_line = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    ܐܢ (first_line)
	changed_lines(first_line, 0, last_line + 1, (long)0);

    Tcl_UnsetVar(tclinfo.interp, var_line, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    ܐܢ (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (فراغ)tclexit(err);
}

    ساكن فراغ
tcldelallrefs(بنية ref *ref)
{
    بنية ref	*next;
    صحيح		err;
    حرف	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    ܐܢ (exiting)
	ܦܢܐ;
#endif

    ܟܡܐ (ref != عدم)
    {
	next = ref->next;
	ܐܢ (ref->interp)
	{
	    ܐܢ (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		ܐܢ (err != TCL_OK)
		{
		    result = (حرف *)Tcl_GetStringResult(ref->interp);
		    ܐܢ (result)
			tclerrmsg(result);
		}
		Tcl_DecrRefCount(ref->delcmd);
		ref->delcmd = عدم;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((حرف *)ref);
	ref = next;
    }
}

    فراغ
tcl_buffer_free(buf_T *buf)
{
    بنية ref *reflist;

#ifdef DYNAMIC_TCL
    ܐܢ (!stubs_initialized)	// Not using Tcl, nothing to do.
	ܦܢܐ;
#endif

    reflist = (بنية ref *)(buf->b_tcl_ref);
    ܐܢ (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (فراغ *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = عدم;
    }
}

    فراغ
tcl_window_free(win_T *win)
{
    بنية ref *reflist;

#ifdef DYNAMIC_TCL
    ܐܢ (!stubs_initialized)	// Not using Tcl, nothing to do.
	ܦܢܐ;
#endif

    reflist = (بنية ref*)(win->w_tcl_ref);
    ܐܢ (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (فراغ *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = عدم;
    }
}

// The End
