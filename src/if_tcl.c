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

#அந்தர்பாவ "vim.h"
#undef EXTERN			// tcl.h defines it too

#ifdef DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#endif

#அந்தர்பாவ <tcl.h>
#அந்தர்பாவ <string.h>

வகைவரை கட்டமைப்பு
{
    Tcl_Interp *interp;
    முழு exitvalue;
    முழு range_start, range_end;
    முழு lbase;
    எழுத்து *curbuf, *curwin;
} tcl_info;

நிலையான tcl_info tclinfo = { வெற்று, 0, 0, 0, 0, வெற்று, வெற்று };

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
கட்டமைப்பு ref
{
    கட்டமைப்பு ref	*next;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    வெறுமை	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
நிலையான எழுத்து * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
நிலையான எழுத்து * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
நிலையான முழு tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, கட்டமைப்பு ref *reflist, வெறுமை *vimobj, Tcl_Obj *delcmd));
நிலையான முழு tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, முழு *மதிப்பு_P, buf_T *buf));
நிலையான win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
நிலையான முழு tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, முழு objc, Tcl_Obj *CONST objv[], முழு objn));
நிலையான முழு tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, முழு objc, Tcl_Obj *CONST objv[], முழு objn));
நிலையான முழு tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, முழு objc, Tcl_Obj *CONST objv[], முழு objn));
நிலையான வெறுமை tcldelthisinterp _ANSI_ARGS_ ((வெறுமை));

நிலையான முழு vimerror _ANSI_ARGS_((Tcl_Interp *interp));
நிலையான வெறுமை tclmsg _ANSI_ARGS_((எழுத்து *text));
நிலையான வெறுமை tclerrmsg _ANSI_ARGS_((எழுத்து *text));
நிலையான வெறுமை tclupdatevars _ANSI_ARGS_((வெறுமை));

நிலையான கட்டமைப்பு ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#எனில் defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
வகைவரை முழு HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define குறியீடு_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# இல்லையெனில்
#  include <dlfcn.h>
#  define HANDLE வெறுமை*
#  define TCL_PROC வெறுமை*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define குறியீடு_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
நிலையான HANDLE hTclLib = வெற்று;
Tcl_Interp* (*dll_Tcl_CreateInterp)(வெறுமை);
வெறுமை (*dll_Tcl_FindExecutable)(மாறிலி வெறுமை *);

/*
 * Table of name to function pointer of tcl.
 */
நிலையான கட்டமைப்பு {
    எழுத்து* name;
    TCL_PROC* ptr;
} tcl_funcname_table[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_FindExecutable},
    {வெற்று, வெற்று},
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
    நிலையான முழு
tcl_runtime_link_init(எழுத்து *libname, முழு verbose)
{
    முழு i;

    எனில் (hTclLib)
	திரும்பு OK;
    எனில் (!(hTclLib = load_dll(libname)))
    {
	எனில் (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	திரும்பு FAIL;
    }
    க்கு (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	எனில் (!(*tcl_funcname_table[i].ptr = குறியீடு_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = வெற்று;
	    எனில் (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    திரும்பு FAIL;
	}
    }
    திரும்பு OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
நிலையான எழுத்து *find_executable_arg = வெற்று;
#endif

    வெறுமை
vim_tcl_init(எழுத்து *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#இல்லையெனில்
    Tcl_FindExecutable(arg);
#endif
}

#எனில் defined(DYNAMIC_TCL)

நிலையான முழு stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    முழு
tcl_enabled(முழு verbose)
{
    எனில் (!stubs_initialized && find_executable_arg != வெற்று
	    && tcl_runtime_link_init((எழுத்து *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	எனில் ((interp = dll_Tcl_CreateInterp()) != வெற்று)
	{
	    எனில் (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != வெற்று)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    திரும்பு stubs_initialized;
}
#endif

#எனில் defined(EXITFREE)
/*
 * Called once when exiting.
 */
    வெறுமை
vim_tcl_finalize(வெறுமை)
{
# ifdef DYNAMIC_TCL
    எனில் (stubs_initialized)
# endif
	Tcl_Finalize();
}
#endif

    வெறுமை
tcl_end(வெறுமை)
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
    நிலையான முழு
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    முழு objc,
    Tcl_Obj *CONST objv[])
{
    முழு மதிப்பு = 0;

    தேர்வு (objc)
    {
	நிலை 2:
	    எனில் (Tcl_GetIntFromObj(interp, objv[1], &மதிப்பு) != TCL_OK)
		நிறுத்து;
	    // FALLTHROUGH
	நிலை 1:
	    tclinfo.exitvalue = மதிப்பு;

	    Tcl_DeleteInterp(interp);
	    நிறுத்து;
	இயல்பு:
	    Tcl_Wrong_இலக்கம்_Args(interp, 1, objv, "?returnCode?");
    }
    திரும்பு TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    நிலையான முழு
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    முழு objc,
    Tcl_Obj *CONST objv[])
{
    எனில் (objc != 1)
    {
	Tcl_Wrong_இலக்கம்_Args(interp, 1, objv, வெற்று);
	திரும்பு TCL_ERROR;
    }
    vim_beep(BO_LANG);
    திரும்பு TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    நிலையான முழு
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    முழு objc,
    Tcl_Obj *CONST objv[])
{
    எழுத்து	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    முழு		err, n, idx;
    எண்ணிக்கை {BCMD_EXISTS, BCMD_LIST};
    நிலையான CONST84 எழுத்து *bcmdoptions[] =
    {
	"exists", "list", (எழுத்து *)0
    };

    எனில் (objc < 2)
    {
	Tcl_Wrong_இலக்கம்_Args(interp, 1, objv, "option");
	திரும்பு TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    எனில் (err == TCL_OK)
    {
	எனில் (objc != 2)
	{
	    Tcl_Wrong_இலக்கம்_Args(interp, 1, objv, "bufNumber");
	    திரும்பு TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    எனில் (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		எனில் (name == வெற்று)
		    திரும்பு TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		திரும்பு TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	திரும்பு TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    எனில் (err != TCL_OK)
	திரும்பு err;
    தேர்வு (idx)
    {
	நிலை BCMD_LIST:
	    எனில் (objc != 2)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, "");
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		எனில் (name == வெற்று)
		{
		    err = TCL_ERROR;
		    நிறுத்து;
		}
		Tcl_AppendElement(interp, name);
	    }
	    நிறுத்து;

	நிலை BCMD_EXISTS:
	    எனில் (objc != 3)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    எனில் (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != வெற்று);
		Tcl_SetObjResult(interp, resobj);
	    }
	    நிறுத்து;

	இயல்பு:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    திரும்பு err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    நிலையான முழு
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    முழு		objc,
    Tcl_Obj	*CONST objv[])
{
    எழுத்து	*what, *string;
    win_T	*win;

    எனில் (objc != 2)
    {
	Tcl_Wrong_இலக்கம்_Args(interp, 1, objv, "option");
	திரும்பு TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], வெற்று);
    எனில் (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    எனில் (string == வெற்று)
		திரும்பு TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	திரும்பு TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    திரும்பு TCL_ERROR;
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
    நிலையான முழு
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    முழு		objc,
    Tcl_Obj	*CONST objv[])
{
    முழு		opt, err, idx, flags;
    முழு		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    எழுத்து	*வரி;

    எண்ணிக்கை
    {
	BUF_APPEND, BUF_COMMAND, BUF_மொத்தம், BUF_DELCMD, BUF_DELETE, BUF_கோவை,
	BUF_GET, BUF_செருகு, BUF_LAST, BUF_MARK, BUF_NAME, BUF_எண்,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    நிலையான CONST84 எழுத்து *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (எழுத்து *)0
    };

    எனில் (objc < 2)
    {
	Tcl_Wrong_இலக்கம்_Args(interp, 1, objv, "option ?arg ...?");
	திரும்பு TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    எனில் (err != TCL_OK)
	திரும்பு err;

    buf = (buf_T *)((கட்டமைப்பு ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    தேர்வு (idx)
    {
	நிலை BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    நிறுத்து;

	நிலை BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    நிறுத்து;

	நிலை BUF_கோவை:
	    err = tclvimexpr(interp, objc, objv, 2);
	    நிறுத்து;

	நிலை BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    எனில் (objc != 2)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, வெற்று);
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    எனில் (buf->b_ffname)
		Tcl_SetResult(interp, (எழுத்து *)buf->b_ffname, TCL_VOLATILE);
	    இல்லையெனில்
		Tcl_SetResult(interp, "", TCL_STATIC);
	    நிறுத்து;

	நிலை BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	நிலை BUF_மொத்தம்:
	    /*
	     * Get number of lines in buffer.
	     */
	    எனில் (objc != 2)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, வெற்று);
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    val1 = (முழு)buf->b_ml.ml_வரி_மொத்தம்;
	    எனில் (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    நிறுத்து;

	நிலை BUF_எண்:
	    /*
	     * Get buffer's number.
	     */
	    எனில் (objc != 2)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, வெற்று);
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    resobj = Tcl_NewIntObj((முழு)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    நிறுத்து;

	நிலை BUF_GET:
	    எனில் (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    எனில் (err != TCL_OK)
		நிறுத்து;
	    எனில் (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		எனில் (err != TCL_OK)
		    நிறுத்து;
		எனில் (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		க்கு (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    வரி = (எழுத்து *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    எனில் (வரி)
			Tcl_AppendElement(interp, வரி);
		    இல்லையெனில்
			err = TCL_ERROR;
		}
	    }
	    இல்லையெனில்
	    {  // objc == 3
		வரி = (எழுத்து *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, வரி, TCL_VOLATILE);
	    }
	    நிறுத்து;

	நிலை BUF_SET:
	    எனில் (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    எனில் (err != TCL_OK)
		திரும்பு TCL_ERROR;
	    எனில் (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		வரி = Tcl_GetStringFromObj(objv[3], வெற்று);
		எனில் (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		இல்லையெனில்
		எனில் (ml_replace((linenr_T)val1, (char_u *)வரி, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		இல்லையெனில்
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		நிறுத்து;
	    }
	    இல்லையெனில்
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		முழு	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		எனில் (err != TCL_OK)
		    நிறுத்து;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		எனில் (err != TCL_OK)
		    நிறுத்து;
		எனில் (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		எனில் (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    நிறுத்து;
		}
		flags |= FL_UPDATE_CURBUF;

		க்கு (i = 0; i < lc && n <= val2; i++)
		{
		    வரி = Tcl_GetStringFromObj(lv[i], வெற்று);
		    எனில் (ml_replace((linenr_T)n, (char_u *)வரி, TRUE) != OK)
			செல் setListError;
		    ++n;
		}
		எனில் (i < lc)
		{
		    // append lines
		    செய்
		    {
			வரி = Tcl_GetStringFromObj(lv[i], வெற்று);
			எனில் (ml_append((linenr_T)(n - 1),
					      (char_u *)வரி, 0, FALSE) != OK)
			    செல் setListError;
			++n;
			++i;
		    } போது (i < lc);
		}
		இல்லையெனில் எனில் (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    செய்
		    {
			எனில் (ml_delete((linenr_T)i) != OK)
			    செல் setListError;
			++n;
		    } போது (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (long)MAXLNUM,
								    (long)lc);
		changed_வரிகள்((linenr_T)val1, 0, (linenr_T)val2 + 1, (long)lc);
		நிறுத்து;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    நிறுத்து;

	நிலை BUF_DELETE:
	    எனில் (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    எனில் (err != TCL_OK)
		நிறுத்து;
	    val2 = val1;
	    எனில் (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		எனில் (err != TCL_OK)
		    திரும்பு err;
		எனில் (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    எனில் (u_savedel((linenr_T)val1, (long)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    க்கு (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		எனில் (err != TCL_OK)
		    நிறுத்து;
	    }
	    எனில் (i > 0)
		deleted_வரிகள்_mark((linenr_T)val1, (long)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    நிறுத்து;

	நிலை BUF_MARK:
	    எனில் (objc != 3)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, "markName");
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    வரி = Tcl_GetStringFromObj(objv[2], வெற்று);

	    pos = வெற்று;
	    எனில் (வரி[0] != '\0'  &&  வரி[1] == '\0')
		pos = getmark(வரி[0], FALSE);
	    எனில் (pos == வெற்று)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    err = vimerror(interp);
	    எனில் (err != TCL_OK)
		நிறுத்து;
	    எனில் (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    இல்லையெனில்
	    {
		எழுத்து rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (முழு)row2tcl(pos->lnum), (முழு)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    நிறுத்து;

	நிலை BUF_செருகு:
	    opt = 1;
	    // fallthrough
	நிலை BUF_APPEND:
	    எனில் (objc != 4)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    எனில் (err != TCL_OK)
		நிறுத்து;
	    எனில் (opt)
		--val1;
	    எனில் (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		நிறுத்து;
	    }

	    வரி = Tcl_GetStringFromObj(objv[3], வெற்று);
	    எனில் (ml_append((linenr_T)val1, (char_u *)வரி, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    appended_வரிகள்_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    நிறுத்து;

	நிலை BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    எனில் (objc != 2)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, வெற்று);
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		எனில் (win->w_buffer == buf)
		{
		    வரி = tclgetwindow(interp, win);
		    எனில் (வரி != வெற்று)
			Tcl_AppendElement(interp, வரி);
		    இல்லையெனில்
		    {
			err = TCL_ERROR;
			நிறுத்து;
		    }
		}
	    }
	    நிறுத்து;

	நிலை BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    எனில் (objc != 3)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (வெறுமை *)buf, objv[2]);
	    நிறுத்து;

	இயல்பு:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    எனில் (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    எனில் (flags & FL_ADJUST_CURSOR)
	check_cursor();
    எனில் (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    திரும்பு err;
}

/*
 * This function implements the window commands.
 */
    நிலையான முழு
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    முழு		objc,
    Tcl_Obj	*CONST objv[])
{
    முழு		err, idx, flags;
    முழு		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    எழுத்து	*str;

    எண்ணிக்கை
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_கோவை,
	WIN_HEIGHT, WIN_OPTION
    };
    நிலையான CONST84 எழுத்து *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (எழுத்து *)0
    };

    எனில் (objc < 2)
    {
	Tcl_Wrong_இலக்கம்_Args(interp, 1, objv, "option ?arg ...?");
	திரும்பு TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    எனில் (err != TCL_OK)
	திரும்பு TCL_ERROR;

    win = (win_T *)((கட்டமைப்பு ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    தேர்வு (idx)
    {
	நிலை WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    நிறுத்து;

	நிலை WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    நிறுத்து;

	நிலை WIN_கோவை:
	    err = tclvimexpr(interp, objc, objv, 2);
	    நிறுத்து;

	நிலை WIN_HEIGHT:
	    எனில் (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		எனில் (err != TCL_OK)
		    நிறுத்து;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		எனில் (err != TCL_OK)
		    நிறுத்து;
	    }
	    இல்லையெனில்
	    எனில் (objc != 2)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		நிறுத்து;
	    }

	    resobj = Tcl_NewIntObj((முழு)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    நிறுத்து;

	நிலை WIN_BUFFER:
	    எனில் (objc != 2)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, வெற்று);
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    எனில் (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    இல்லையெனில்
		err = TCL_ERROR;
	    நிறுத்து;

	நிலை WIN_DELCMD:
	    எனில் (objc != 3)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (வெறுமை *)win, objv[2]);
	    நிறுத்து;

	நிலை WIN_CURSOR:
	    எனில் (objc > 4)
	    {
		Tcl_Wrong_இலக்கம்_Args(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    எனில் (objc == 2)
	    {
		எழுத்து buf[64];

		sprintf(buf, _("row %d column %d"), (முழு)row2tcl(win->w_cursor.lnum), (முழு)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		நிறுத்து;
	    }
	    இல்லையெனில் எனில் (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_செய்தி);
		எனில் (var == வெற்று)
		{
		    err = TCL_ERROR;
		    நிறுத்து;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		எனில் (err != TCL_OK)
		    நிறுத்து;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_செய்தி);
		எனில் (var == வெற்று)
		{
		    err = TCL_ERROR;
		    நிறுத்து;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		எனில் (err != TCL_OK)
		    நிறுத்து;
	    }
	    இல்லையெனில்
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		எனில் (err != TCL_OK)
		    நிறுத்து;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		எனில் (err != TCL_OK)
		    நிறுத்து;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    நிறுத்து;

	இயல்பு:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    நிறுத்து;
    }

    curwin = savewin;
    curbuf = savebuf;
    எனில் (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    திரும்பு err;
}


    நிலையான முழு
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    முழு		objc,
    Tcl_Obj	*CONST objv[])
{
    முழு		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    திரும்பு err;
}

    நிலையான முழு
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    முழு		objc,
    Tcl_Obj	*CONST objv[])
{
    முழு		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    திரும்பு err;
}

    நிலையான முழு
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    முழு		objc,
    Tcl_Obj	*CONST objv[])
{
    திரும்பு tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    நிலையான முழு
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    முழு		*மதிப்பு_P,
    buf_T	*buf)
{
    முழு err, i;

    எண்ணிக்கை { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_உச்சி };

    நிலையான CONST84 எழுத்து *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (எழுத்து *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    எனில் (err == TCL_OK)
    {
	தேர்வு (i)
	{
	    நிலை LN_BEGIN:
	    நிலை LN_FIRST:
	    நிலை LN_START:
	    நிலை LN_உச்சி:
		*மதிப்பு_P = 1;
		நிறுத்து;
	    நிலை LN_BOTTOM:
	    நிலை LN_END:
	    நிலை LN_LAST:
		*மதிப்பு_P = buf->b_ml.ml_வரி_மொத்தம்;
		நிறுத்து;
	}
	திரும்பு TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    எனில் (err != TCL_OK)
	திரும்பு err;
    i = row2vim(i);
    எனில் (i < 1  ||  i > buf->b_ml.ml_வரி_மொத்தம்)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	திரும்பு TCL_ERROR;
    }
    *மதிப்பு_P = i;
    திரும்பு TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    நிலையான win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	எனில் (win->w_buffer == buf)
	    திரும்பு win;
    }
    திரும்பு curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    நிலையான முழு
tcldoexcommand(
    Tcl_Interp	*interp,
    முழு		objc,
    Tcl_Obj	*CONST objv[],
    முழு		objn)
{
    tcl_info	saveinfo;
    முழு		err, flag, nobjs;
    எழுத்து	*arg;

    nobjs = objc - objn;
    எனில் (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_இலக்கம்_Args(interp, objn, objv, "?-quiet? exCommand");
	திரும்பு TCL_ERROR;
    }

    flag = 0;
    எனில் (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], வெற்று);
	எனில் (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	இல்லையெனில்
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (எழுத்து *)0);
	    திரும்பு TCL_ERROR;
	}
	++objn;
    }

    memcpy(&saveinfo, &tclinfo, அளவு(tcl_info));
    tclinfo.interp = வெற்று;
    tclinfo.curwin = வெற்று;
    tclinfo.curbuf = வெற்று;

    arg = Tcl_GetStringFromObj(objv[objn], வெற்று);
    எனில் (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    எனில் (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    எனில் (tclinfo.interp)
	tcldelthisinterp();
    memcpy(&tclinfo, &saveinfo, அளவு(tcl_info));
    tclupdatevars();

    திரும்பு err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    நிலையான முழு
tclsetoption(
    Tcl_Interp	*interp,
    முழு		objc,
    Tcl_Obj	*CONST objv[],
    முழு		objn)
{
    முழு		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    long	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    எண்ணிக்கை { OPT_OFF, OPT_ON, OPT_TOGGLE };
    நிலையான CONST84 எழுத்து *optkw[] = { "off", "on", "toggle", (எழுத்து *)0 };

    nobjs = objc - objn;
    எனில் (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_இலக்கம்_Args(interp, objn, objv, "vimOption ?value?");
	திரும்பு TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], வெற்று);
    ++objn;
    gov = get_option_மதிப்பு(option, &lval, &sval, வெற்று, 0);
    err = TCL_OK;
    தேர்வு (gov)
    {
	நிலை gov_string:
	    Tcl_SetResult(interp, (எழுத்து *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    நிறுத்து;
	நிலை gov_bool:
	நிலை gov_எண்:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    நிறுத்து;
	இயல்பு:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    திரும்பு TCL_ERROR;
    }
    எனில் (nobjs == 2)
    {
	எனில் (gov != gov_string)
	{
	    sval = வெற்று;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    எனில் (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    இல்லையெனில்
	    {
		தேர்வு (idx)
		{
		    நிலை OPT_ON:
			lval = 1;
			நிறுத்து;
		    நிலை OPT_OFF:
			lval = 0;
			நிறுத்து;
		    நிலை OPT_TOGGLE:
			lval = !lval;
			நிறுத்து;
		}
	    }
	}
	இல்லையெனில்
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], வெற்று);
	எனில் (err == TCL_OK)
	{
	    set_option_மதிப்பு_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    திரும்பு err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    நிலையான முழு
tclvimexpr(
    Tcl_Interp	*interp,
    முழு		objc,
    Tcl_Obj	*CONST objv[],
    முழு		objn)
{
#ifdef FEAT_EVAL
    எழுத்து	*கோவை, *str;
#endif
    முழு		err;

    எனில் (objc - objn != 1)
    {
	Tcl_Wrong_இலக்கம்_Args(interp, objn, objv, "vimExpr");
	திரும்பு TCL_ERROR;
    }

#ifdef FEAT_EVAL
    கோவை = Tcl_GetStringFromObj(objv[objn], வெற்று);
    str = (எழுத்து *)eval_to_string((char_u *)கோவை, TRUE, FALSE);
    எனில் (str == வெற்று)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    இல்லையெனில்
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#இல்லையெனில்
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    திரும்பு err;
}

/*
 * Check for internal vim errors.
 */
    நிலையான முழு
vimerror(Tcl_Interp *interp)
{
    எனில் (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	திரும்பு TCL_ERROR;
    }
    இல்லையெனில் எனில் (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	திரும்பு TCL_ERROR;
    }
    திரும்பு TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    நிலையான வெறுமை
delref(ClientData cref)
{
    கட்டமைப்பு ref *ref = (கட்டமைப்பு ref *)cref;

    எனில் (ref->delcmd)
    {
	Tcl_Decr_Ref_மொத்தம்(ref->delcmd);
	ref->delcmd = வெற்று;
    }
    ref->interp = வெற்று;
}

    நிலையான எழுத்து *
tclgetref(
    Tcl_Interp	*interp,
    வெறுமை	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    எழுத்து	*prefix,	// "win" or "buf"
    வெறுமை	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    கட்டமைப்பு ref *ref, *unused = வெற்று;
    நிலையான எழுத்து name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (கட்டமைப்பு ref *)(*refstartP);
    எனில் (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	திரும்பு வெற்று;
    }

    போது (ref != வெற்று)
    {
	எனில் (ref->interp == interp)
	    நிறுத்து;
	எனில் (ref->interp == வெற்று)
	    unused = ref;
	ref = ref->next;
    }

    எனில் (ref)
	vim_snprintf(name, அளவு(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    இல்லையெனில்
    {
	எனில் (unused)
	    ref = unused;
	இல்லையெனில்
	{
	    ref = (கட்டமைப்பு ref *)Tcl_Alloc(அளவு(கட்டமைப்பு ref));
	    ref->interp = வெற்று;
	    ref->next = (கட்டமைப்பு ref *)(*refstartP);
	    (*refstartP) = (வெறுமை *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, அளவு(name), "::vim::%s_%lx",
					       prefix, (unsigned long)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	எனில் (!cmd)
	    திரும்பு வெற்று;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = வெற்று;
	ref->vimobj = vimobj;
    }
    திரும்பு name;
}

    நிலையான எழுத்து *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    திரும்பு tclgetref(interp, &(win->w_tcl_ref), "win", (வெறுமை *)win, winselfcmd);
}

    நிலையான எழுத்து *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    திரும்பு tclgetref(interp, &(buf->b_tcl_ref), "buf", (வெறுமை *)buf, bufselfcmd);
}

    நிலையான முழு
tclsetdelcmd(
    Tcl_Interp	*interp,
    கட்டமைப்பு ref	*reflist,
    வெறுமை	*vimobj,
    Tcl_Obj	*delcmd)
{
    எனில் (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	திரும்பு TCL_ERROR;
    }

    போது (reflist != வெற்று)
    {
	எனில் (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    எனில் (reflist->delcmd)
		Tcl_Decr_Ref_மொத்தம்(reflist->delcmd);
	    Tcl_Incr_Ref_மொத்தம்(delcmd);
	    reflist->delcmd = delcmd;
	    திரும்பு TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    திரும்பு TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    நிலையான முழு
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    முழு		err = 0;

    // currently does nothing

    எனில் (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    திரும்பு err;
}

    நிலையான முழு
tcl_channel_உள்ளீடு(
    ClientData	instance UNUSED,
    எழுத்து	*buf UNUSED,
    முழு		bufsiz UNUSED,
    முழு		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    எனில் (errptr)
	*errptr = EINVAL;
    திரும்பு -1;
}

    நிலையான முழு
tcl_channel_output(
    ClientData	instance,
    மாறிலி எழுத்து	*buf,
    முழு		bufsiz,
    முழு		*errptr)
{
    char_u	*str;
    முழு		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    எனில் (!str)
    {
	Tcl_SetErrno(ENOMEM);
	எனில் (errptr)
	    *errptr = ENOMEM;
	திரும்பு -1;
    }

    result = bufsiz;
    எனில் (instance == VIMOUT)
	tclmsg((எழுத்து *)str);
    இல்லையெனில்
    எனில் (instance == VIMERR)
	tclerrmsg((எழுத்து *)str);
    இல்லையெனில்
    {
	Tcl_SetErrno(EBADF);
	எனில் (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    திரும்பு result;
}

    நிலையான வெறுமை
tcl_channel_watch(ClientData instance UNUSED, முழு mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    நிலையான முழு
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    முழு		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    திரும்பு EINVAL;
}


நிலையான Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_VERSION_2, // version
    tcl_channel_close,	// closeProc
    tcl_channel_உள்ளீடு,	// inputProc
    tcl_channel_output,	// outputProc
    வெற்று,		// seekProc
    வெற்று,		// setOptionProc
    வெற்று,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    வெற்று,		// close2Proc
    வெற்று,		// blockModeProc
#ifdef TCL_CHANNEL_VERSION_2
    வெற்று,		// flushProc
    வெற்று,		// handlerProc
#endif
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ifdef TCL_CHANNEL_VERSION_3
    வெற்று,		// wideSeekProc
#endif
#ifdef TCL_CHANNEL_VERSION_4
    வெற்று,		// threadActionProc
#endif
#ifdef TCL_CHANNEL_VERSION_5
    வெற்று		// truncateProc
#endif
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    நிலையான வெறுமை
tclupdatevars(வெறுமை)
{
    எழுத்து varname[VARNAME_SIZE];	// must be writeable
    எழுத்து *name;

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


    நிலையான முழு
tclinit(exarg_T *eap)
{
    எழுத்து varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    எழுத்து *name;

#ifdef DYNAMIC_TCL
    எனில் (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	திரும்பு FAIL;
    }
#endif

    எனில் (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	நிலையான Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_SetStdChannel(ch1, TCL_STDOUT);
	Tcl_SetStdChannel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	எனில் (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    திரும்பு FAIL;
	}
#எனில் 0
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
	    (ClientData)வெற்று, (Tcl_CmdDeleteProc *)வெற்று);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (ClientData)வெற்று, (Tcl_CmdDeleteProc *)வெற்று);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (ClientData)வெற்று, (Tcl_CmdDeleteProc *)வெற்று);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (ClientData)வெற்று, (Tcl_CmdDeleteProc *)வெற்று);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (ClientData)வெற்று, (Tcl_CmdDeleteProc *)வெற்று);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (ClientData)வெற்று, (Tcl_CmdDeleteProc *)வெற்று);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (ClientData)வெற்று, (Tcl_CmdDeleteProc *)வெற்று);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (எழுத்து *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (எழுத்து *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (எழுத்து *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (எழுத்து *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (எழுத்து *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (எழுத்து *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    இல்லையெனில்
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    திரும்பு OK;
}

    நிலையான வெறுமை
tclerrmsg(எழுத்து *text)
{
    எழுத்து *next;

    போது ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    எனில் (*text)
	emsg(text);
}

    நிலையான வெறுமை
tclmsg(எழுத்து *text)
{
    எழுத்து *next;

    போது ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	செய்தி(text);
	text = next;
    }
    எனில் (*text)
	செய்தி(text);
}

    நிலையான வெறுமை
tcldelthisinterp(வெறுமை)
{
    எனில் (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = வெற்று;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = வெற்று;
}

    நிலையான முழு
tclexit(முழு error)
{
    முழு newerr = OK;

    எனில் (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#எனில் (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	எழுத்து buf[50];

	sprintf(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	எனில் (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	இல்லையெனில்
	    newerr = FAIL;

	tcldelthisinterp();
    }
    இல்லையெனில்
    {
	எழுத்து *result;

	result = (எழுத்து *)Tcl_GetStringResult(tclinfo.interp);
	எனில் (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	இல்லையெனில்
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    திரும்பு newerr;
}

/*
 * ":tcl"
 */
    வெறுமை
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    முழு		err;

    script = script_get(eap, eap->arg);
    எனில் (!eap->skip)
    {
	err = tclinit(eap);
	எனில் (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    எனில் (script == வெற்று)
		err = Tcl_Eval(tclinfo.interp, (எழுத்து *)eap->arg);
	    இல்லையெனில்
		err = Tcl_Eval(tclinfo.interp, (எழுத்து *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    வெறுமை
ex_tclfile(exarg_T *eap)
{
    எழுத்து *file = (எழுத்து *)eap->arg;
    முழு err;

    err = tclinit(eap);
    எனில் (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    வெறுமை
ex_tcldo(exarg_T *eap)
{
    எழுத்து	*script, *வரி;
    முழு		err, rs, re, lnum;
    எழுத்து	var_lnum[VARNAME_SIZE]; // must be writeable memory
    எழுத்து	var_வரி[VARNAME_SIZE];
    linenr_T	first_வரி = 0;
    linenr_T	last_வரி = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (எழுத்து *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_வரி, VAR_CURLINE);

    err = tclinit(eap);
    எனில் (err != OK)
	திரும்பு;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (எழுத்து *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    எனில் (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    போது (err == TCL_OK  &&  rs <= re)
    {
	எனில் ((linenr_T)rs > curbuf->b_ml.ml_வரி_மொத்தம்)
	    நிறுத்து;
	வரி = (எழுத்து *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	எனில் (!வரி)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    நிறுத்து;
	}
	Tcl_SetVar(tclinfo.interp, var_வரி, வரி, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	எனில் (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#எனில் (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_வரி_மொத்தம்)
	    நிறுத்து;
	வரி = (எழுத்து *)Tcl_GetVar(tclinfo.interp, var_வரி, 0);
	எனில் (வரி)
	{
	    எனில் (ml_replace((linenr_T)rs, (char_u *)வரி, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		நிறுத்து;
	    }
	    எனில் (first_வரி == 0)
		first_வரி = rs;
	    last_வரி = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    எனில் (first_வரி)
	changed_வரிகள்(first_வரி, 0, last_வரி + 1, (long)0);

    Tcl_UnsetVar(tclinfo.interp, var_வரி, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    எனில் (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (வெறுமை)tclexit(err);
}

    நிலையான வெறுமை
tcldelallrefs(கட்டமைப்பு ref *ref)
{
    கட்டமைப்பு ref	*next;
    முழு		err;
    எழுத்து	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    எனில் (exiting)
	திரும்பு;
#endif

    போது (ref != வெற்று)
    {
	next = ref->next;
	எனில் (ref->interp)
	{
	    எனில் (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		எனில் (err != TCL_OK)
		{
		    result = (எழுத்து *)Tcl_GetStringResult(ref->interp);
		    எனில் (result)
			tclerrmsg(result);
		}
		Tcl_Decr_Ref_மொத்தம்(ref->delcmd);
		ref->delcmd = வெற்று;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((எழுத்து *)ref);
	ref = next;
    }
}

    வெறுமை
tcl_buffer_free(buf_T *buf)
{
    கட்டமைப்பு ref *reflist;

#ifdef DYNAMIC_TCL
    எனில் (!stubs_initialized)	// Not using Tcl, nothing to do.
	திரும்பு;
#endif

    reflist = (கட்டமைப்பு ref *)(buf->b_tcl_ref);
    எனில் (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (வெறுமை *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = வெற்று;
    }
}

    வெறுமை
tcl_window_free(win_T *win)
{
    கட்டமைப்பு ref *reflist;

#ifdef DYNAMIC_TCL
    எனில் (!stubs_initialized)	// Not using Tcl, nothing to do.
	திரும்பு;
#endif

    reflist = (கட்டமைப்பு ref*)(win->w_tcl_ref);
    எனில் (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (வெறுமை *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = வெற்று;
    }
}

// The End
