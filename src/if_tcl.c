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

#شامل "vim.h"
#undef EXTERN			// tcl.h defines it too

#ifdef DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#endif

#شامل <tcl.h>
#شامل <string.h>

تعریفنوع ساختار
{
    Tcl_Interp *interp;
    صحیح exitvalue;
    صحیح range_start, range_end;
    صحیح lbase;
    نویسه *curbuf, *curwin;
} tcl_info;

ایستا tcl_info tclinfo = { پوچ, 0, 0, 0, 0, پوچ, پوچ };

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
ساختار ref
{
    ساختار ref	*next;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    تهی	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
ایستا نویسه * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
ایستا نویسه * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
ایستا صحیح tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, ساختار ref *reflist, تهی *vimobj, Tcl_Obj *delcmd));
ایستا صحیح tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, صحیح *مقدار_P, buf_T *buf));
ایستا win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
ایستا صحیح tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, صحیح objc, Tcl_Obj *CONST objv[], صحیح objn));
ایستا صحیح tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, صحیح objc, Tcl_Obj *CONST objv[], صحیح objn));
ایستا صحیح tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, صحیح objc, Tcl_Obj *CONST objv[], صحیح objn));
ایستا تهی tcldelthisinterp _ANSI_ARGS_ ((تهی));

ایستا صحیح vimerror _ANSI_ARGS_((Tcl_Interp *interp));
ایستا تهی tclmsg _ANSI_ARGS_((نویسه *text));
ایستا تهی tclerrmsg _ANSI_ARGS_((نویسه *text));
ایستا تهی tclupdatevars _ANSI_ARGS_((تهی));

ایستا ساختار ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#اگر defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
تعریفنوع صحیح HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define نماد_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# وگرنه
#  include <dlfcn.h>
#  define HANDLE تهی*
#  define TCL_PROC تهی*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define نماد_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
ایستا HANDLE hTclLib = پوچ;
Tcl_Interp* (*dll_Tcl_CreateInterp)(تهی);
تهی (*dll_Tcl_FindExecutable)(ثابت تهی *);

/*
 * Table of name to function pointer of tcl.
 */
ایستا ساختار {
    نویسه* name;
    TCL_PROC* ptr;
} tcl_funcname_table[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_FindExecutable},
    {پوچ, پوچ},
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
    ایستا صحیح
tcl_runtime_link_init(نویسه *libname, صحیح verbose)
{
    صحیح i;

    اگر (hTclLib)
	بازگشت OK;
    اگر (!(hTclLib = load_dll(libname)))
    {
	اگر (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	بازگشت FAIL;
    }
    برای (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	اگر (!(*tcl_funcname_table[i].ptr = نماد_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = پوچ;
	    اگر (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    بازگشت FAIL;
	}
    }
    بازگشت OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
ایستا نویسه *find_executable_arg = پوچ;
#endif

    تهی
vim_tcl_init(نویسه *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#وگرنه
    Tcl_FindExecutable(arg);
#endif
}

#اگر defined(DYNAMIC_TCL)

ایستا صحیح stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    صحیح
tcl_enabled(صحیح verbose)
{
    اگر (!stubs_initialized && find_executable_arg != پوچ
	    && tcl_runtime_link_init((نویسه *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	اگر ((interp = dll_Tcl_CreateInterp()) != پوچ)
	{
	    اگر (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != پوچ)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    بازگشت stubs_initialized;
}
#endif

#اگر defined(EXITFREE)
/*
 * Called once when exiting.
 */
    تهی
vim_tcl_finalize(تهی)
{
# ifdef DYNAMIC_TCL
    اگر (stubs_initialized)
# endif
	Tcl_Finalize();
}
#endif

    تهی
tcl_end(تهی)
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
    ایستا صحیح
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    صحیح objc,
    Tcl_Obj *CONST objv[])
{
    صحیح مقدار = 0;

    انتخاب (objc)
    {
	حالت 2:
	    اگر (Tcl_GetIntFromObj(interp, objv[1], &مقدار) != TCL_OK)
		بشکن;
	    // FALLTHROUGH
	حالت 1:
	    tclinfo.exitvalue = مقدار;

	    Tcl_DeleteInterp(interp);
	    بشکن;
	پیشفرض:
	    Tcl_Wrong_رقم_Args(interp, 1, objv, "?returnCode?");
    }
    بازگشت TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    ایستا صحیح
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    صحیح objc,
    Tcl_Obj *CONST objv[])
{
    اگر (objc != 1)
    {
	Tcl_Wrong_رقم_Args(interp, 1, objv, پوچ);
	بازگشت TCL_ERROR;
    }
    vim_beep(BO_LANG);
    بازگشت TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    ایستا صحیح
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    صحیح objc,
    Tcl_Obj *CONST objv[])
{
    نویسه	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    صحیح		err, n, idx;
    شمارش {BCMD_EXISTS, BCMD_LIST};
    ایستا CONST84 نویسه *bcmdoptions[] =
    {
	"exists", "list", (نویسه *)0
    };

    اگر (objc < 2)
    {
	Tcl_Wrong_رقم_Args(interp, 1, objv, "option");
	بازگشت TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    اگر (err == TCL_OK)
    {
	اگر (objc != 2)
	{
	    Tcl_Wrong_رقم_Args(interp, 1, objv, "bufNumber");
	    بازگشت TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    اگر (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		اگر (name == پوچ)
		    بازگشت TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		بازگشت TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	بازگشت TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    اگر (err != TCL_OK)
	بازگشت err;
    انتخاب (idx)
    {
	حالت BCMD_LIST:
	    اگر (objc != 2)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, "");
		err = TCL_ERROR;
		بشکن;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		اگر (name == پوچ)
		{
		    err = TCL_ERROR;
		    بشکن;
		}
		Tcl_AppendElement(interp, name);
	    }
	    بشکن;

	حالت BCMD_EXISTS:
	    اگر (objc != 3)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		بشکن;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    اگر (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != پوچ);
		Tcl_SetObjResult(interp, resobj);
	    }
	    بشکن;

	پیشفرض:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    بازگشت err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    ایستا صحیح
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[])
{
    نویسه	*what, *string;
    win_T	*win;

    اگر (objc != 2)
    {
	Tcl_Wrong_رقم_Args(interp, 1, objv, "option");
	بازگشت TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], پوچ);
    اگر (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    اگر (string == پوچ)
		بازگشت TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	بازگشت TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    بازگشت TCL_ERROR;
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
    ایستا صحیح
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحیح		opt, err, idx, flags;
    صحیح		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    نویسه	*سطر;

    شمارش
    {
	BUF_APPEND, BUF_COMMAND, BUF_شمار, BUF_DELCMD, BUF_DELETE, BUF_عبارت,
	BUF_GET, BUF_درج, BUF_LAST, BUF_MARK, BUF_NAME, BUF_عدد,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    ایستا CONST84 نویسه *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (نویسه *)0
    };

    اگر (objc < 2)
    {
	Tcl_Wrong_رقم_Args(interp, 1, objv, "option ?arg ...?");
	بازگشت TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    اگر (err != TCL_OK)
	بازگشت err;

    buf = (buf_T *)((ساختار ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    انتخاب (idx)
    {
	حالت BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    بشکن;

	حالت BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    بشکن;

	حالت BUF_عبارت:
	    err = tclvimexpr(interp, objc, objv, 2);
	    بشکن;

	حالت BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    اگر (objc != 2)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, پوچ);
		err = TCL_ERROR;
		بشکن;
	    }
	    اگر (buf->b_ffname)
		Tcl_SetResult(interp, (نویسه *)buf->b_ffname, TCL_VOLATILE);
	    وگرنه
		Tcl_SetResult(interp, "", TCL_STATIC);
	    بشکن;

	حالت BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	حالت BUF_شمار:
	    /*
	     * Get number of lines in buffer.
	     */
	    اگر (objc != 2)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, پوچ);
		err = TCL_ERROR;
		بشکن;
	    }
	    val1 = (صحیح)buf->b_ml.ml_سطر_شمار;
	    اگر (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    بشکن;

	حالت BUF_عدد:
	    /*
	     * Get buffer's number.
	     */
	    اگر (objc != 2)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, پوچ);
		err = TCL_ERROR;
		بشکن;
	    }
	    resobj = Tcl_NewIntObj((صحیح)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    بشکن;

	حالت BUF_GET:
	    اگر (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		بشکن;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    اگر (err != TCL_OK)
		بشکن;
	    اگر (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		اگر (err != TCL_OK)
		    بشکن;
		اگر (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		برای (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    سطر = (نویسه *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    اگر (سطر)
			Tcl_AppendElement(interp, سطر);
		    وگرنه
			err = TCL_ERROR;
		}
	    }
	    وگرنه
	    {  // objc == 3
		سطر = (نویسه *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, سطر, TCL_VOLATILE);
	    }
	    بشکن;

	حالت BUF_SET:
	    اگر (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_رقم_Args(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		بشکن;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    اگر (err != TCL_OK)
		بازگشت TCL_ERROR;
	    اگر (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		سطر = Tcl_GetStringFromObj(objv[3], پوچ);
		اگر (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		وگرنه
		اگر (ml_replace((linenr_T)val1, (char_u *)سطر, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		وگرنه
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		بشکن;
	    }
	    وگرنه
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		صحیح	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		اگر (err != TCL_OK)
		    بشکن;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		اگر (err != TCL_OK)
		    بشکن;
		اگر (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		اگر (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    بشکن;
		}
		flags |= FL_UPDATE_CURBUF;

		برای (i = 0; i < lc && n <= val2; i++)
		{
		    سطر = Tcl_GetStringFromObj(lv[i], پوچ);
		    اگر (ml_replace((linenr_T)n, (char_u *)سطر, TRUE) != OK)
			برو setListError;
		    ++n;
		}
		اگر (i < lc)
		{
		    // append lines
		    انجامبده
		    {
			سطر = Tcl_GetStringFromObj(lv[i], پوچ);
			اگر (ml_append((linenr_T)(n - 1),
					      (char_u *)سطر, 0, FALSE) != OK)
			    برو setListError;
			++n;
			++i;
		    } وقتی (i < lc);
		}
		وگرنه اگر (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    انجامبده
		    {
			اگر (ml_delete((linenr_T)i) != OK)
			    برو setListError;
			++n;
		    } وقتی (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (long)MAXLNUM,
								    (long)lc);
		changed_سطرها((linenr_T)val1, 0, (linenr_T)val2 + 1, (long)lc);
		بشکن;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    بشکن;

	حالت BUF_DELETE:
	    اگر (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_رقم_Args(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		بشکن;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    اگر (err != TCL_OK)
		بشکن;
	    val2 = val1;
	    اگر (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		اگر (err != TCL_OK)
		    بازگشت err;
		اگر (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    اگر (u_savedel((linenr_T)val1, (long)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		بشکن;
	    }
	    برای (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		اگر (err != TCL_OK)
		    بشکن;
	    }
	    اگر (i > 0)
		deleted_سطرها_mark((linenr_T)val1, (long)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    بشکن;

	حالت BUF_MARK:
	    اگر (objc != 3)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, "markName");
		err = TCL_ERROR;
		بشکن;
	    }
	    سطر = Tcl_GetStringFromObj(objv[2], پوچ);

	    pos = پوچ;
	    اگر (سطر[0] != '\0'  &&  سطر[1] == '\0')
		pos = getmark(سطر[0], FALSE);
	    اگر (pos == پوچ)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		بشکن;
	    }
	    err = vimerror(interp);
	    اگر (err != TCL_OK)
		بشکن;
	    اگر (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    وگرنه
	    {
		نویسه rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (صحیح)row2tcl(pos->lnum), (صحیح)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    بشکن;

	حالت BUF_درج:
	    opt = 1;
	    // fallthrough
	حالت BUF_APPEND:
	    اگر (objc != 4)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		بشکن;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    اگر (err != TCL_OK)
		بشکن;
	    اگر (opt)
		--val1;
	    اگر (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		بشکن;
	    }

	    سطر = Tcl_GetStringFromObj(objv[3], پوچ);
	    اگر (ml_append((linenr_T)val1, (char_u *)سطر, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		بشکن;
	    }
	    appended_سطرها_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    بشکن;

	حالت BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    اگر (objc != 2)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, پوچ);
		err = TCL_ERROR;
		بشکن;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		اگر (win->w_buffer == buf)
		{
		    سطر = tclgetwindow(interp, win);
		    اگر (سطر != پوچ)
			Tcl_AppendElement(interp, سطر);
		    وگرنه
		    {
			err = TCL_ERROR;
			بشکن;
		    }
		}
	    }
	    بشکن;

	حالت BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    اگر (objc != 3)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		بشکن;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (تهی *)buf, objv[2]);
	    بشکن;

	پیشفرض:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    اگر (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    اگر (flags & FL_ADJUST_CURSOR)
	check_cursor();
    اگر (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    بازگشت err;
}

/*
 * This function implements the window commands.
 */
    ایستا صحیح
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحیح		err, idx, flags;
    صحیح		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    نویسه	*str;

    شمارش
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_عبارت,
	WIN_HEIGHT, WIN_OPTION
    };
    ایستا CONST84 نویسه *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (نویسه *)0
    };

    اگر (objc < 2)
    {
	Tcl_Wrong_رقم_Args(interp, 1, objv, "option ?arg ...?");
	بازگشت TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    اگر (err != TCL_OK)
	بازگشت TCL_ERROR;

    win = (win_T *)((ساختار ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    انتخاب (idx)
    {
	حالت WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    بشکن;

	حالت WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    بشکن;

	حالت WIN_عبارت:
	    err = tclvimexpr(interp, objc, objv, 2);
	    بشکن;

	حالت WIN_HEIGHT:
	    اگر (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		اگر (err != TCL_OK)
		    بشکن;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		اگر (err != TCL_OK)
		    بشکن;
	    }
	    وگرنه
	    اگر (objc != 2)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		بشکن;
	    }

	    resobj = Tcl_NewIntObj((صحیح)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    بشکن;

	حالت WIN_BUFFER:
	    اگر (objc != 2)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, پوچ);
		err = TCL_ERROR;
		بشکن;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    اگر (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    وگرنه
		err = TCL_ERROR;
	    بشکن;

	حالت WIN_DELCMD:
	    اگر (objc != 3)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		بشکن;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (تهی *)win, objv[2]);
	    بشکن;

	حالت WIN_CURSOR:
	    اگر (objc > 4)
	    {
		Tcl_Wrong_رقم_Args(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		بشکن;
	    }
	    اگر (objc == 2)
	    {
		نویسه buf[64];

		sprintf(buf, _("row %d column %d"), (صحیح)row2tcl(win->w_cursor.lnum), (صحیح)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		بشکن;
	    }
	    وگرنه اگر (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_پیام);
		اگر (var == پوچ)
		{
		    err = TCL_ERROR;
		    بشکن;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		اگر (err != TCL_OK)
		    بشکن;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_پیام);
		اگر (var == پوچ)
		{
		    err = TCL_ERROR;
		    بشکن;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		اگر (err != TCL_OK)
		    بشکن;
	    }
	    وگرنه
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		اگر (err != TCL_OK)
		    بشکن;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		اگر (err != TCL_OK)
		    بشکن;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    بشکن;

	پیشفرض:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    بشکن;
    }

    curwin = savewin;
    curbuf = savebuf;
    اگر (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    بازگشت err;
}


    ایستا صحیح
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحیح		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    بازگشت err;
}

    ایستا صحیح
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحیح		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    بازگشت err;
}

    ایستا صحیح
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[])
{
    بازگشت tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    ایستا صحیح
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    صحیح		*مقدار_P,
    buf_T	*buf)
{
    صحیح err, i;

    شمارش { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_بالاترین };

    ایستا CONST84 نویسه *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (نویسه *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    اگر (err == TCL_OK)
    {
	انتخاب (i)
	{
	    حالت LN_BEGIN:
	    حالت LN_FIRST:
	    حالت LN_START:
	    حالت LN_بالاترین:
		*مقدار_P = 1;
		بشکن;
	    حالت LN_BOTTOM:
	    حالت LN_END:
	    حالت LN_LAST:
		*مقدار_P = buf->b_ml.ml_سطر_شمار;
		بشکن;
	}
	بازگشت TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    اگر (err != TCL_OK)
	بازگشت err;
    i = row2vim(i);
    اگر (i < 1  ||  i > buf->b_ml.ml_سطر_شمار)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	بازگشت TCL_ERROR;
    }
    *مقدار_P = i;
    بازگشت TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    ایستا win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	اگر (win->w_buffer == buf)
	    بازگشت win;
    }
    بازگشت curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    ایستا صحیح
tcldoexcommand(
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[],
    صحیح		objn)
{
    tcl_info	saveinfo;
    صحیح		err, flag, nobjs;
    نویسه	*arg;

    nobjs = objc - objn;
    اگر (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_رقم_Args(interp, objn, objv, "?-quiet? exCommand");
	بازگشت TCL_ERROR;
    }

    flag = 0;
    اگر (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], پوچ);
	اگر (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	وگرنه
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (نویسه *)0);
	    بازگشت TCL_ERROR;
	}
	++objn;
    }

    memcpy(&saveinfo, &tclinfo, اندازه(tcl_info));
    tclinfo.interp = پوچ;
    tclinfo.curwin = پوچ;
    tclinfo.curbuf = پوچ;

    arg = Tcl_GetStringFromObj(objv[objn], پوچ);
    اگر (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    اگر (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    اگر (tclinfo.interp)
	tcldelthisinterp();
    memcpy(&tclinfo, &saveinfo, اندازه(tcl_info));
    tclupdatevars();

    بازگشت err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    ایستا صحیح
tclsetoption(
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[],
    صحیح		objn)
{
    صحیح		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    long	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    شمارش { OPT_OFF, OPT_ON, OPT_TOGGLE };
    ایستا CONST84 نویسه *optkw[] = { "off", "on", "toggle", (نویسه *)0 };

    nobjs = objc - objn;
    اگر (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_رقم_Args(interp, objn, objv, "vimOption ?value?");
	بازگشت TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], پوچ);
    ++objn;
    gov = get_option_مقدار(option, &lval, &sval, پوچ, 0);
    err = TCL_OK;
    انتخاب (gov)
    {
	حالت gov_string:
	    Tcl_SetResult(interp, (نویسه *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    بشکن;
	حالت gov_bool:
	حالت gov_عدد:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    بشکن;
	پیشفرض:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    بازگشت TCL_ERROR;
    }
    اگر (nobjs == 2)
    {
	اگر (gov != gov_string)
	{
	    sval = پوچ;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    اگر (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    وگرنه
	    {
		انتخاب (idx)
		{
		    حالت OPT_ON:
			lval = 1;
			بشکن;
		    حالت OPT_OFF:
			lval = 0;
			بشکن;
		    حالت OPT_TOGGLE:
			lval = !lval;
			بشکن;
		}
	    }
	}
	وگرنه
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], پوچ);
	اگر (err == TCL_OK)
	{
	    set_option_مقدار_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    بازگشت err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    ایستا صحیح
tclvimexpr(
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[],
    صحیح		objn)
{
#ifdef FEAT_EVAL
    نویسه	*عبارت, *str;
#endif
    صحیح		err;

    اگر (objc - objn != 1)
    {
	Tcl_Wrong_رقم_Args(interp, objn, objv, "vimExpr");
	بازگشت TCL_ERROR;
    }

#ifdef FEAT_EVAL
    عبارت = Tcl_GetStringFromObj(objv[objn], پوچ);
    str = (نویسه *)eval_to_string((char_u *)عبارت, TRUE, FALSE);
    اگر (str == پوچ)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    وگرنه
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#وگرنه
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    بازگشت err;
}

/*
 * Check for internal vim errors.
 */
    ایستا صحیح
vimerror(Tcl_Interp *interp)
{
    اگر (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	بازگشت TCL_ERROR;
    }
    وگرنه اگر (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	بازگشت TCL_ERROR;
    }
    بازگشت TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    ایستا تهی
delref(ClientData cref)
{
    ساختار ref *ref = (ساختار ref *)cref;

    اگر (ref->delcmd)
    {
	Tcl_Decr_Ref_شمار(ref->delcmd);
	ref->delcmd = پوچ;
    }
    ref->interp = پوچ;
}

    ایستا نویسه *
tclgetref(
    Tcl_Interp	*interp,
    تهی	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    نویسه	*prefix,	// "win" or "buf"
    تهی	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    ساختار ref *ref, *unused = پوچ;
    ایستا نویسه name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (ساختار ref *)(*refstartP);
    اگر (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	بازگشت پوچ;
    }

    وقتی (ref != پوچ)
    {
	اگر (ref->interp == interp)
	    بشکن;
	اگر (ref->interp == پوچ)
	    unused = ref;
	ref = ref->next;
    }

    اگر (ref)
	vim_snprintf(name, اندازه(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    وگرنه
    {
	اگر (unused)
	    ref = unused;
	وگرنه
	{
	    ref = (ساختار ref *)Tcl_Alloc(اندازه(ساختار ref));
	    ref->interp = پوچ;
	    ref->next = (ساختار ref *)(*refstartP);
	    (*refstartP) = (تهی *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, اندازه(name), "::vim::%s_%lx",
					       prefix, (unsigned long)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	اگر (!cmd)
	    بازگشت پوچ;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = پوچ;
	ref->vimobj = vimobj;
    }
    بازگشت name;
}

    ایستا نویسه *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    بازگشت tclgetref(interp, &(win->w_tcl_ref), "win", (تهی *)win, winselfcmd);
}

    ایستا نویسه *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    بازگشت tclgetref(interp, &(buf->b_tcl_ref), "buf", (تهی *)buf, bufselfcmd);
}

    ایستا صحیح
tclsetdelcmd(
    Tcl_Interp	*interp,
    ساختار ref	*reflist,
    تهی	*vimobj,
    Tcl_Obj	*delcmd)
{
    اگر (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	بازگشت TCL_ERROR;
    }

    وقتی (reflist != پوچ)
    {
	اگر (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    اگر (reflist->delcmd)
		Tcl_Decr_Ref_شمار(reflist->delcmd);
	    Tcl_Incr_Ref_شمار(delcmd);
	    reflist->delcmd = delcmd;
	    بازگشت TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    بازگشت TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    ایستا صحیح
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    صحیح		err = 0;

    // currently does nothing

    اگر (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    بازگشت err;
}

    ایستا صحیح
tcl_channel_درون_داد(
    ClientData	instance UNUSED,
    نویسه	*buf UNUSED,
    صحیح		bufsiz UNUSED,
    صحیح		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    اگر (errptr)
	*errptr = EINVAL;
    بازگشت -1;
}

    ایستا صحیح
tcl_channel_output(
    ClientData	instance,
    ثابت نویسه	*buf,
    صحیح		bufsiz,
    صحیح		*errptr)
{
    char_u	*str;
    صحیح		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    اگر (!str)
    {
	Tcl_SetErrno(ENOMEM);
	اگر (errptr)
	    *errptr = ENOMEM;
	بازگشت -1;
    }

    result = bufsiz;
    اگر (instance == VIMOUT)
	tclmsg((نویسه *)str);
    وگرنه
    اگر (instance == VIMERR)
	tclerrmsg((نویسه *)str);
    وگرنه
    {
	Tcl_SetErrno(EBADF);
	اگر (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    بازگشت result;
}

    ایستا تهی
tcl_channel_watch(ClientData instance UNUSED, صحیح mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    ایستا صحیح
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    صحیح		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    بازگشت EINVAL;
}


ایستا Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_VERSION_2, // version
    tcl_channel_close,	// closeProc
    tcl_channel_درون_داد,	// inputProc
    tcl_channel_output,	// outputProc
    پوچ,		// seekProc
    پوچ,		// setOptionProc
    پوچ,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    پوچ,		// close2Proc
    پوچ,		// blockModeProc
#ifdef TCL_CHANNEL_VERSION_2
    پوچ,		// flushProc
    پوچ,		// handlerProc
#endif
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ifdef TCL_CHANNEL_VERSION_3
    پوچ,		// wideSeekProc
#endif
#ifdef TCL_CHANNEL_VERSION_4
    پوچ,		// threadActionProc
#endif
#ifdef TCL_CHANNEL_VERSION_5
    پوچ		// truncateProc
#endif
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    ایستا تهی
tclupdatevars(تهی)
{
    نویسه varname[VARNAME_SIZE];	// must be writeable
    نویسه *name;

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


    ایستا صحیح
tclinit(exarg_T *eap)
{
    نویسه varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    نویسه *name;

#ifdef DYNAMIC_TCL
    اگر (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	بازگشت FAIL;
    }
#endif

    اگر (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	ایستا Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_SetStdChannel(ch1, TCL_STDOUT);
	Tcl_SetStdChannel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	اگر (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    بازگشت FAIL;
	}
#اگر 0
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
	    (ClientData)پوچ, (Tcl_CmdDeleteProc *)پوچ);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (ClientData)پوچ, (Tcl_CmdDeleteProc *)پوچ);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (ClientData)پوچ, (Tcl_CmdDeleteProc *)پوچ);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (ClientData)پوچ, (Tcl_CmdDeleteProc *)پوچ);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (ClientData)پوچ, (Tcl_CmdDeleteProc *)پوچ);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (ClientData)پوچ, (Tcl_CmdDeleteProc *)پوچ);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (ClientData)پوچ, (Tcl_CmdDeleteProc *)پوچ);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (نویسه *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (نویسه *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (نویسه *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (نویسه *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (نویسه *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (نویسه *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    وگرنه
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    بازگشت OK;
}

    ایستا تهی
tclerrmsg(نویسه *text)
{
    نویسه *next;

    وقتی ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    اگر (*text)
	emsg(text);
}

    ایستا تهی
tclmsg(نویسه *text)
{
    نویسه *next;

    وقتی ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	پیام(text);
	text = next;
    }
    اگر (*text)
	پیام(text);
}

    ایستا تهی
tcldelthisinterp(تهی)
{
    اگر (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = پوچ;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = پوچ;
}

    ایستا صحیح
tclexit(صحیح error)
{
    صحیح newerr = OK;

    اگر (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#اگر (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	نویسه buf[50];

	sprintf(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	اگر (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	وگرنه
	    newerr = FAIL;

	tcldelthisinterp();
    }
    وگرنه
    {
	نویسه *result;

	result = (نویسه *)Tcl_GetStringResult(tclinfo.interp);
	اگر (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	وگرنه
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    بازگشت newerr;
}

/*
 * ":tcl"
 */
    تهی
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    صحیح		err;

    script = script_get(eap, eap->arg);
    اگر (!eap->skip)
    {
	err = tclinit(eap);
	اگر (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    اگر (script == پوچ)
		err = Tcl_Eval(tclinfo.interp, (نویسه *)eap->arg);
	    وگرنه
		err = Tcl_Eval(tclinfo.interp, (نویسه *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    تهی
ex_tclfile(exarg_T *eap)
{
    نویسه *file = (نویسه *)eap->arg;
    صحیح err;

    err = tclinit(eap);
    اگر (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    تهی
ex_tcldo(exarg_T *eap)
{
    نویسه	*script, *سطر;
    صحیح		err, rs, re, lnum;
    نویسه	var_lnum[VARNAME_SIZE]; // must be writeable memory
    نویسه	var_سطر[VARNAME_SIZE];
    linenr_T	first_سطر = 0;
    linenr_T	last_سطر = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (نویسه *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_سطر, VAR_CURLINE);

    err = tclinit(eap);
    اگر (err != OK)
	بازگشت;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (نویسه *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    اگر (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    وقتی (err == TCL_OK  &&  rs <= re)
    {
	اگر ((linenr_T)rs > curbuf->b_ml.ml_سطر_شمار)
	    بشکن;
	سطر = (نویسه *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	اگر (!سطر)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    بشکن;
	}
	Tcl_SetVar(tclinfo.interp, var_سطر, سطر, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	اگر (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#اگر (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_سطر_شمار)
	    بشکن;
	سطر = (نویسه *)Tcl_GetVar(tclinfo.interp, var_سطر, 0);
	اگر (سطر)
	{
	    اگر (ml_replace((linenr_T)rs, (char_u *)سطر, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		بشکن;
	    }
	    اگر (first_سطر == 0)
		first_سطر = rs;
	    last_سطر = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    اگر (first_سطر)
	changed_سطرها(first_سطر, 0, last_سطر + 1, (long)0);

    Tcl_UnsetVar(tclinfo.interp, var_سطر, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    اگر (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (تهی)tclexit(err);
}

    ایستا تهی
tcldelallrefs(ساختار ref *ref)
{
    ساختار ref	*next;
    صحیح		err;
    نویسه	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    اگر (exiting)
	بازگشت;
#endif

    وقتی (ref != پوچ)
    {
	next = ref->next;
	اگر (ref->interp)
	{
	    اگر (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		اگر (err != TCL_OK)
		{
		    result = (نویسه *)Tcl_GetStringResult(ref->interp);
		    اگر (result)
			tclerrmsg(result);
		}
		Tcl_Decr_Ref_شمار(ref->delcmd);
		ref->delcmd = پوچ;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((نویسه *)ref);
	ref = next;
    }
}

    تهی
tcl_buffer_free(buf_T *buf)
{
    ساختار ref *reflist;

#ifdef DYNAMIC_TCL
    اگر (!stubs_initialized)	// Not using Tcl, nothing to do.
	بازگشت;
#endif

    reflist = (ساختار ref *)(buf->b_tcl_ref);
    اگر (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (تهی *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = پوچ;
    }
}

    تهی
tcl_window_free(win_T *win)
{
    ساختار ref *reflist;

#ifdef DYNAMIC_TCL
    اگر (!stubs_initialized)	// Not using Tcl, nothing to do.
	بازگشت;
#endif

    reflist = (ساختار ref*)(win->w_tcl_ref);
    اگر (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (تهی *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = پوچ;
    }
}

// The End
