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

#અન્તર્ભાવ "vim.h"
#undef EXTERN			// tcl.h defines it too

#ifdef DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#endif

#અન્તર્ભાવ <tcl.h>
#અન્તર્ભાવ <string.h>

પ્રકારવ્યાખ્યા સંરચના
{
    Tcl_Interp *interp;
    પૂર્ણાંક exitvalue;
    પૂર્ણાંક range_start, range_end;
    પૂર્ણાંક lbase;
    અક્ષર *curbuf, *curwin;
} tcl_info;

સ્થિર tcl_info tclinfo = { ખાલી, 0, 0, 0, 0, ખાલી, ખાલી };

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
સંરચના ref
{
    સંરચના ref	*next;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    શૂન્ય	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
સ્થિર અક્ષર * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
સ્થિર અક્ષર * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
સ્થિર પૂર્ણાંક tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, સંરચના ref *reflist, શૂન્ય *vimobj, Tcl_Obj *delcmd));
સ્થિર પૂર્ણાંક tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, પૂર્ણાંક *કિંમત_P, buf_T *buf));
સ્થિર win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
સ્થિર પૂર્ણાંક tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, પૂર્ણાંક objc, Tcl_Obj *CONST objv[], પૂર્ણાંક objn));
સ્થિર પૂર્ણાંક tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, પૂર્ણાંક objc, Tcl_Obj *CONST objv[], પૂર્ણાંક objn));
સ્થિર પૂર્ણાંક tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, પૂર્ણાંક objc, Tcl_Obj *CONST objv[], પૂર્ણાંક objn));
સ્થિર શૂન્ય tcldelthisinterp _ANSI_ARGS_ ((શૂન્ય));

સ્થિર પૂર્ણાંક vimerror _ANSI_ARGS_((Tcl_Interp *interp));
સ્થિર શૂન્ય tclmsg _ANSI_ARGS_((અક્ષર *text));
સ્થિર શૂન્ય tclerrmsg _ANSI_ARGS_((અક્ષર *text));
સ્થિર શૂન્ય tclupdatevars _ANSI_ARGS_((શૂન્ય));

સ્થિર સંરચના ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#જો defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
પ્રકારવ્યાખ્યા પૂર્ણાંક HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define પ્રતીક_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# નહીંતર
#  include <dlfcn.h>
#  define HANDLE શૂન્ય*
#  define TCL_PROC શૂન્ય*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define પ્રતીક_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
સ્થિર HANDLE hTclLib = ખાલી;
Tcl_Interp* (*dll_Tcl_CreateInterp)(શૂન્ય);
શૂન્ય (*dll_Tcl_FindExecutable)(અચલ શૂન્ય *);

/*
 * Table of name to function pointer of tcl.
 */
સ્થિર સંરચના {
    અક્ષર* name;
    TCL_PROC* ptr;
} tcl_funcname_table[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_FindExecutable},
    {ખાલી, ખાલી},
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
    સ્થિર પૂર્ણાંક
tcl_runtime_link_init(અક્ષર *libname, પૂર્ણાંક verbose)
{
    પૂર્ણાંક i;

    જો (hTclLib)
	પાછા OK;
    જો (!(hTclLib = load_dll(libname)))
    {
	જો (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	પાછા FAIL;
    }
    માટે (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	જો (!(*tcl_funcname_table[i].ptr = પ્રતીક_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = ખાલી;
	    જો (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    પાછા FAIL;
	}
    }
    પાછા OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
સ્થિર અક્ષર *find_executable_arg = ખાલી;
#endif

    શૂન્ય
vim_tcl_init(અક્ષર *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#નહીંતર
    Tcl_FindExecutable(arg);
#endif
}

#જો defined(DYNAMIC_TCL)

સ્થિર પૂર્ણાંક stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    પૂર્ણાંક
tcl_enabled(પૂર્ણાંક verbose)
{
    જો (!stubs_initialized && find_executable_arg != ખાલી
	    && tcl_runtime_link_init((અક્ષર *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	જો ((interp = dll_Tcl_CreateInterp()) != ખાલી)
	{
	    જો (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != ખાલી)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    પાછા stubs_initialized;
}
#endif

#જો defined(EXITFREE)
/*
 * Called once when exiting.
 */
    શૂન્ય
vim_tcl_finalize(શૂન્ય)
{
# ifdef DYNAMIC_TCL
    જો (stubs_initialized)
# endif
	Tcl_Finalize();
}
#endif

    શૂન્ય
tcl_end(શૂન્ય)
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
    સ્થિર પૂર્ણાંક
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    પૂર્ણાંક objc,
    Tcl_Obj *CONST objv[])
{
    પૂર્ણાંક કિંમત = 0;

    પસંદગી (objc)
    {
	સ્થિતિ 2:
	    જો (Tcl_GetIntFromObj(interp, objv[1], &કિંમત) != TCL_OK)
		તોડો;
	    // FALLTHROUGH
	સ્થિતિ 1:
	    tclinfo.exitvalue = કિંમત;

	    Tcl_DeleteInterp(interp);
	    તોડો;
	મૂળભૂત:
	    Tcl_Wrong_અંક_Args(interp, 1, objv, "?returnCode?");
    }
    પાછા TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    સ્થિર પૂર્ણાંક
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    પૂર્ણાંક objc,
    Tcl_Obj *CONST objv[])
{
    જો (objc != 1)
    {
	Tcl_Wrong_અંક_Args(interp, 1, objv, ખાલી);
	પાછા TCL_ERROR;
    }
    vim_beep(BO_LANG);
    પાછા TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    સ્થિર પૂર્ણાંક
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    પૂર્ણાંક objc,
    Tcl_Obj *CONST objv[])
{
    અક્ષર	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    પૂર્ણાંક		err, n, idx;
    ગણના {BCMD_EXISTS, BCMD_LIST};
    સ્થિર CONST84 અક્ષર *bcmdoptions[] =
    {
	"exists", "list", (અક્ષર *)0
    };

    જો (objc < 2)
    {
	Tcl_Wrong_અંક_Args(interp, 1, objv, "option");
	પાછા TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    જો (err == TCL_OK)
    {
	જો (objc != 2)
	{
	    Tcl_Wrong_અંક_Args(interp, 1, objv, "bufNumber");
	    પાછા TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    જો (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		જો (name == ખાલી)
		    પાછા TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		પાછા TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	પાછા TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    જો (err != TCL_OK)
	પાછા err;
    પસંદગી (idx)
    {
	સ્થિતિ BCMD_LIST:
	    જો (objc != 2)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, "");
		err = TCL_ERROR;
		તોડો;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		જો (name == ખાલી)
		{
		    err = TCL_ERROR;
		    તોડો;
		}
		Tcl_AppendElement(interp, name);
	    }
	    તોડો;

	સ્થિતિ BCMD_EXISTS:
	    જો (objc != 3)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		તોડો;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    જો (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != ખાલી);
		Tcl_SetObjResult(interp, resobj);
	    }
	    તોડો;

	મૂળભૂત:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    પાછા err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    સ્થિર પૂર્ણાંક
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    પૂર્ણાંક		objc,
    Tcl_Obj	*CONST objv[])
{
    અક્ષર	*what, *string;
    win_T	*win;

    જો (objc != 2)
    {
	Tcl_Wrong_અંક_Args(interp, 1, objv, "option");
	પાછા TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], ખાલી);
    જો (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    જો (string == ખાલી)
		પાછા TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	પાછા TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    પાછા TCL_ERROR;
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
    સ્થિર પૂર્ણાંક
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    પૂર્ણાંક		objc,
    Tcl_Obj	*CONST objv[])
{
    પૂર્ણાંક		opt, err, idx, flags;
    પૂર્ણાંક		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    અક્ષર	*લીટી;

    ગણના
    {
	BUF_APPEND, BUF_COMMAND, BUF_ગણતરી, BUF_DELCMD, BUF_DELETE, BUF_પદાવલિ,
	BUF_GET, BUF_ઉમેરો, BUF_LAST, BUF_MARK, BUF_NAME, BUF_સંખ્યા,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    સ્થિર CONST84 અક્ષર *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (અક્ષર *)0
    };

    જો (objc < 2)
    {
	Tcl_Wrong_અંક_Args(interp, 1, objv, "option ?arg ...?");
	પાછા TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    જો (err != TCL_OK)
	પાછા err;

    buf = (buf_T *)((સંરચના ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    પસંદગી (idx)
    {
	સ્થિતિ BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    તોડો;

	સ્થિતિ BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    તોડો;

	સ્થિતિ BUF_પદાવલિ:
	    err = tclvimexpr(interp, objc, objv, 2);
	    તોડો;

	સ્થિતિ BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    જો (objc != 2)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, ખાલી);
		err = TCL_ERROR;
		તોડો;
	    }
	    જો (buf->b_ffname)
		Tcl_SetResult(interp, (અક્ષર *)buf->b_ffname, TCL_VOLATILE);
	    નહીંતર
		Tcl_SetResult(interp, "", TCL_STATIC);
	    તોડો;

	સ્થિતિ BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	સ્થિતિ BUF_ગણતરી:
	    /*
	     * Get number of lines in buffer.
	     */
	    જો (objc != 2)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, ખાલી);
		err = TCL_ERROR;
		તોડો;
	    }
	    val1 = (પૂર્ણાંક)buf->b_ml.ml_લીટી_ગણતરી;
	    જો (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    તોડો;

	સ્થિતિ BUF_સંખ્યા:
	    /*
	     * Get buffer's number.
	     */
	    જો (objc != 2)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, ખાલી);
		err = TCL_ERROR;
		તોડો;
	    }
	    resobj = Tcl_NewIntObj((પૂર્ણાંક)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    તોડો;

	સ્થિતિ BUF_GET:
	    જો (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		તોડો;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    જો (err != TCL_OK)
		તોડો;
	    જો (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		જો (err != TCL_OK)
		    તોડો;
		જો (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		માટે (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    લીટી = (અક્ષર *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    જો (લીટી)
			Tcl_AppendElement(interp, લીટી);
		    નહીંતર
			err = TCL_ERROR;
		}
	    }
	    નહીંતર
	    {  // objc == 3
		લીટી = (અક્ષર *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, લીટી, TCL_VOLATILE);
	    }
	    તોડો;

	સ્થિતિ BUF_SET:
	    જો (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_અંક_Args(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		તોડો;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    જો (err != TCL_OK)
		પાછા TCL_ERROR;
	    જો (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		લીટી = Tcl_GetStringFromObj(objv[3], ખાલી);
		જો (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		નહીંતર
		જો (ml_replace((linenr_T)val1, (char_u *)લીટી, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		નહીંતર
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		તોડો;
	    }
	    નહીંતર
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		પૂર્ણાંક	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		જો (err != TCL_OK)
		    તોડો;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		જો (err != TCL_OK)
		    તોડો;
		જો (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		જો (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    તોડો;
		}
		flags |= FL_UPDATE_CURBUF;

		માટે (i = 0; i < lc && n <= val2; i++)
		{
		    લીટી = Tcl_GetStringFromObj(lv[i], ખાલી);
		    જો (ml_replace((linenr_T)n, (char_u *)લીટી, TRUE) != OK)
			જાઓ setListError;
		    ++n;
		}
		જો (i < lc)
		{
		    // append lines
		    કરો
		    {
			લીટી = Tcl_GetStringFromObj(lv[i], ખાલી);
			જો (ml_append((linenr_T)(n - 1),
					      (char_u *)લીટી, 0, FALSE) != OK)
			    જાઓ setListError;
			++n;
			++i;
		    } જ્યારે (i < lc);
		}
		નહીંતર જો (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    કરો
		    {
			જો (ml_delete((linenr_T)i) != OK)
			    જાઓ setListError;
			++n;
		    } જ્યારે (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (long)MAXLNUM,
								    (long)lc);
		changed_લીટીઓ((linenr_T)val1, 0, (linenr_T)val2 + 1, (long)lc);
		તોડો;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    તોડો;

	સ્થિતિ BUF_DELETE:
	    જો (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_અંક_Args(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		તોડો;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    જો (err != TCL_OK)
		તોડો;
	    val2 = val1;
	    જો (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		જો (err != TCL_OK)
		    પાછા err;
		જો (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    જો (u_savedel((linenr_T)val1, (long)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		તોડો;
	    }
	    માટે (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		જો (err != TCL_OK)
		    તોડો;
	    }
	    જો (i > 0)
		deleted_લીટીઓ_mark((linenr_T)val1, (long)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    તોડો;

	સ્થિતિ BUF_MARK:
	    જો (objc != 3)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, "markName");
		err = TCL_ERROR;
		તોડો;
	    }
	    લીટી = Tcl_GetStringFromObj(objv[2], ખાલી);

	    pos = ખાલી;
	    જો (લીટી[0] != '\0'  &&  લીટી[1] == '\0')
		pos = getmark(લીટી[0], FALSE);
	    જો (pos == ખાલી)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		તોડો;
	    }
	    err = vimerror(interp);
	    જો (err != TCL_OK)
		તોડો;
	    જો (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    નહીંતર
	    {
		અક્ષર rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (પૂર્ણાંક)row2tcl(pos->lnum), (પૂર્ણાંક)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    તોડો;

	સ્થિતિ BUF_ઉમેરો:
	    opt = 1;
	    // fallthrough
	સ્થિતિ BUF_APPEND:
	    જો (objc != 4)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		તોડો;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    જો (err != TCL_OK)
		તોડો;
	    જો (opt)
		--val1;
	    જો (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		તોડો;
	    }

	    લીટી = Tcl_GetStringFromObj(objv[3], ખાલી);
	    જો (ml_append((linenr_T)val1, (char_u *)લીટી, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		તોડો;
	    }
	    appended_લીટીઓ_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    તોડો;

	સ્થિતિ BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    જો (objc != 2)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, ખાલી);
		err = TCL_ERROR;
		તોડો;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		જો (win->w_buffer == buf)
		{
		    લીટી = tclgetwindow(interp, win);
		    જો (લીટી != ખાલી)
			Tcl_AppendElement(interp, લીટી);
		    નહીંતર
		    {
			err = TCL_ERROR;
			તોડો;
		    }
		}
	    }
	    તોડો;

	સ્થિતિ BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    જો (objc != 3)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		તોડો;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (શૂન્ય *)buf, objv[2]);
	    તોડો;

	મૂળભૂત:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    જો (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    જો (flags & FL_ADJUST_CURSOR)
	check_cursor();
    જો (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    પાછા err;
}

/*
 * This function implements the window commands.
 */
    સ્થિર પૂર્ણાંક
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    પૂર્ણાંક		objc,
    Tcl_Obj	*CONST objv[])
{
    પૂર્ણાંક		err, idx, flags;
    પૂર્ણાંક		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    અક્ષર	*str;

    ગણના
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_પદાવલિ,
	WIN_HEIGHT, WIN_OPTION
    };
    સ્થિર CONST84 અક્ષર *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (અક્ષર *)0
    };

    જો (objc < 2)
    {
	Tcl_Wrong_અંક_Args(interp, 1, objv, "option ?arg ...?");
	પાછા TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    જો (err != TCL_OK)
	પાછા TCL_ERROR;

    win = (win_T *)((સંરચના ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    પસંદગી (idx)
    {
	સ્થિતિ WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    તોડો;

	સ્થિતિ WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    તોડો;

	સ્થિતિ WIN_પદાવલિ:
	    err = tclvimexpr(interp, objc, objv, 2);
	    તોડો;

	સ્થિતિ WIN_HEIGHT:
	    જો (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		જો (err != TCL_OK)
		    તોડો;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		જો (err != TCL_OK)
		    તોડો;
	    }
	    નહીંતર
	    જો (objc != 2)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		તોડો;
	    }

	    resobj = Tcl_NewIntObj((પૂર્ણાંક)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    તોડો;

	સ્થિતિ WIN_BUFFER:
	    જો (objc != 2)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, ખાલી);
		err = TCL_ERROR;
		તોડો;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    જો (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    નહીંતર
		err = TCL_ERROR;
	    તોડો;

	સ્થિતિ WIN_DELCMD:
	    જો (objc != 3)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		તોડો;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (શૂન્ય *)win, objv[2]);
	    તોડો;

	સ્થિતિ WIN_CURSOR:
	    જો (objc > 4)
	    {
		Tcl_Wrong_અંક_Args(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		તોડો;
	    }
	    જો (objc == 2)
	    {
		અક્ષર buf[64];

		sprintf(buf, _("row %d column %d"), (પૂર્ણાંક)row2tcl(win->w_cursor.lnum), (પૂર્ણાંક)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		તોડો;
	    }
	    નહીંતર જો (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_સંદેશ);
		જો (var == ખાલી)
		{
		    err = TCL_ERROR;
		    તોડો;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		જો (err != TCL_OK)
		    તોડો;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_સંદેશ);
		જો (var == ખાલી)
		{
		    err = TCL_ERROR;
		    તોડો;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		જો (err != TCL_OK)
		    તોડો;
	    }
	    નહીંતર
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		જો (err != TCL_OK)
		    તોડો;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		જો (err != TCL_OK)
		    તોડો;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    તોડો;

	મૂળભૂત:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    તોડો;
    }

    curwin = savewin;
    curbuf = savebuf;
    જો (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    પાછા err;
}


    સ્થિર પૂર્ણાંક
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    પૂર્ણાંક		objc,
    Tcl_Obj	*CONST objv[])
{
    પૂર્ણાંક		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    પાછા err;
}

    સ્થિર પૂર્ણાંક
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    પૂર્ણાંક		objc,
    Tcl_Obj	*CONST objv[])
{
    પૂર્ણાંક		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    પાછા err;
}

    સ્થિર પૂર્ણાંક
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    પૂર્ણાંક		objc,
    Tcl_Obj	*CONST objv[])
{
    પાછા tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    સ્થિર પૂર્ણાંક
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    પૂર્ણાંક		*કિંમત_P,
    buf_T	*buf)
{
    પૂર્ણાંક err, i;

    ગણના { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_ટોચ };

    સ્થિર CONST84 અક્ષર *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (અક્ષર *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    જો (err == TCL_OK)
    {
	પસંદગી (i)
	{
	    સ્થિતિ LN_BEGIN:
	    સ્થિતિ LN_FIRST:
	    સ્થિતિ LN_START:
	    સ્થિતિ LN_ટોચ:
		*કિંમત_P = 1;
		તોડો;
	    સ્થિતિ LN_BOTTOM:
	    સ્થિતિ LN_END:
	    સ્થિતિ LN_LAST:
		*કિંમત_P = buf->b_ml.ml_લીટી_ગણતરી;
		તોડો;
	}
	પાછા TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    જો (err != TCL_OK)
	પાછા err;
    i = row2vim(i);
    જો (i < 1  ||  i > buf->b_ml.ml_લીટી_ગણતરી)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	પાછા TCL_ERROR;
    }
    *કિંમત_P = i;
    પાછા TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    સ્થિર win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	જો (win->w_buffer == buf)
	    પાછા win;
    }
    પાછા curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    સ્થિર પૂર્ણાંક
tcldoexcommand(
    Tcl_Interp	*interp,
    પૂર્ણાંક		objc,
    Tcl_Obj	*CONST objv[],
    પૂર્ણાંક		objn)
{
    tcl_info	saveinfo;
    પૂર્ણાંક		err, flag, nobjs;
    અક્ષર	*arg;

    nobjs = objc - objn;
    જો (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_અંક_Args(interp, objn, objv, "?-quiet? exCommand");
	પાછા TCL_ERROR;
    }

    flag = 0;
    જો (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], ખાલી);
	જો (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	નહીંતર
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (અક્ષર *)0);
	    પાછા TCL_ERROR;
	}
	++objn;
    }

    memcpy(&saveinfo, &tclinfo, કદ(tcl_info));
    tclinfo.interp = ખાલી;
    tclinfo.curwin = ખાલી;
    tclinfo.curbuf = ખાલી;

    arg = Tcl_GetStringFromObj(objv[objn], ખાલી);
    જો (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    જો (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    જો (tclinfo.interp)
	tcldelthisinterp();
    memcpy(&tclinfo, &saveinfo, કદ(tcl_info));
    tclupdatevars();

    પાછા err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    સ્થિર પૂર્ણાંક
tclsetoption(
    Tcl_Interp	*interp,
    પૂર્ણાંક		objc,
    Tcl_Obj	*CONST objv[],
    પૂર્ણાંક		objn)
{
    પૂર્ણાંક		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    long	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    ગણના { OPT_OFF, OPT_ON, OPT_TOGGLE };
    સ્થિર CONST84 અક્ષર *optkw[] = { "off", "on", "toggle", (અક્ષર *)0 };

    nobjs = objc - objn;
    જો (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_અંક_Args(interp, objn, objv, "vimOption ?value?");
	પાછા TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], ખાલી);
    ++objn;
    gov = get_option_કિંમત(option, &lval, &sval, ખાલી, 0);
    err = TCL_OK;
    પસંદગી (gov)
    {
	સ્થિતિ gov_string:
	    Tcl_SetResult(interp, (અક્ષર *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    તોડો;
	સ્થિતિ gov_bool:
	સ્થિતિ gov_સંખ્યા:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    તોડો;
	મૂળભૂત:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    પાછા TCL_ERROR;
    }
    જો (nobjs == 2)
    {
	જો (gov != gov_string)
	{
	    sval = ખાલી;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    જો (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    નહીંતર
	    {
		પસંદગી (idx)
		{
		    સ્થિતિ OPT_ON:
			lval = 1;
			તોડો;
		    સ્થિતિ OPT_OFF:
			lval = 0;
			તોડો;
		    સ્થિતિ OPT_TOGGLE:
			lval = !lval;
			તોડો;
		}
	    }
	}
	નહીંતર
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], ખાલી);
	જો (err == TCL_OK)
	{
	    set_option_કિંમત_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    પાછા err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    સ્થિર પૂર્ણાંક
tclvimexpr(
    Tcl_Interp	*interp,
    પૂર્ણાંક		objc,
    Tcl_Obj	*CONST objv[],
    પૂર્ણાંક		objn)
{
#ifdef FEAT_EVAL
    અક્ષર	*પદાવલિ, *str;
#endif
    પૂર્ણાંક		err;

    જો (objc - objn != 1)
    {
	Tcl_Wrong_અંક_Args(interp, objn, objv, "vimExpr");
	પાછા TCL_ERROR;
    }

#ifdef FEAT_EVAL
    પદાવલિ = Tcl_GetStringFromObj(objv[objn], ખાલી);
    str = (અક્ષર *)eval_to_string((char_u *)પદાવલિ, TRUE, FALSE);
    જો (str == ખાલી)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    નહીંતર
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#નહીંતર
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    પાછા err;
}

/*
 * Check for internal vim errors.
 */
    સ્થિર પૂર્ણાંક
vimerror(Tcl_Interp *interp)
{
    જો (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	પાછા TCL_ERROR;
    }
    નહીંતર જો (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	પાછા TCL_ERROR;
    }
    પાછા TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    સ્થિર શૂન્ય
delref(ClientData cref)
{
    સંરચના ref *ref = (સંરચના ref *)cref;

    જો (ref->delcmd)
    {
	Tcl_Decr_Ref_ગણતરી(ref->delcmd);
	ref->delcmd = ખાલી;
    }
    ref->interp = ખાલી;
}

    સ્થિર અક્ષર *
tclgetref(
    Tcl_Interp	*interp,
    શૂન્ય	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    અક્ષર	*prefix,	// "win" or "buf"
    શૂન્ય	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    સંરચના ref *ref, *unused = ખાલી;
    સ્થિર અક્ષર name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (સંરચના ref *)(*refstartP);
    જો (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	પાછા ખાલી;
    }

    જ્યારે (ref != ખાલી)
    {
	જો (ref->interp == interp)
	    તોડો;
	જો (ref->interp == ખાલી)
	    unused = ref;
	ref = ref->next;
    }

    જો (ref)
	vim_snprintf(name, કદ(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    નહીંતર
    {
	જો (unused)
	    ref = unused;
	નહીંતર
	{
	    ref = (સંરચના ref *)Tcl_Alloc(કદ(સંરચના ref));
	    ref->interp = ખાલી;
	    ref->next = (સંરચના ref *)(*refstartP);
	    (*refstartP) = (શૂન્ય *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, કદ(name), "::vim::%s_%lx",
					       prefix, (unsigned long)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	જો (!cmd)
	    પાછા ખાલી;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = ખાલી;
	ref->vimobj = vimobj;
    }
    પાછા name;
}

    સ્થિર અક્ષર *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    પાછા tclgetref(interp, &(win->w_tcl_ref), "win", (શૂન્ય *)win, winselfcmd);
}

    સ્થિર અક્ષર *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    પાછા tclgetref(interp, &(buf->b_tcl_ref), "buf", (શૂન્ય *)buf, bufselfcmd);
}

    સ્થિર પૂર્ણાંક
tclsetdelcmd(
    Tcl_Interp	*interp,
    સંરચના ref	*reflist,
    શૂન્ય	*vimobj,
    Tcl_Obj	*delcmd)
{
    જો (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	પાછા TCL_ERROR;
    }

    જ્યારે (reflist != ખાલી)
    {
	જો (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    જો (reflist->delcmd)
		Tcl_Decr_Ref_ગણતરી(reflist->delcmd);
	    Tcl_Incr_Ref_ગણતરી(delcmd);
	    reflist->delcmd = delcmd;
	    પાછા TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    પાછા TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    સ્થિર પૂર્ણાંક
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    પૂર્ણાંક		err = 0;

    // currently does nothing

    જો (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    પાછા err;
}

    સ્થિર પૂર્ણાંક
tcl_channel_આગત(
    ClientData	instance UNUSED,
    અક્ષર	*buf UNUSED,
    પૂર્ણાંક		bufsiz UNUSED,
    પૂર્ણાંક		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    જો (errptr)
	*errptr = EINVAL;
    પાછા -1;
}

    સ્થિર પૂર્ણાંક
tcl_channel_output(
    ClientData	instance,
    અચલ અક્ષર	*buf,
    પૂર્ણાંક		bufsiz,
    પૂર્ણાંક		*errptr)
{
    char_u	*str;
    પૂર્ણાંક		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    જો (!str)
    {
	Tcl_SetErrno(ENOMEM);
	જો (errptr)
	    *errptr = ENOMEM;
	પાછા -1;
    }

    result = bufsiz;
    જો (instance == VIMOUT)
	tclmsg((અક્ષર *)str);
    નહીંતર
    જો (instance == VIMERR)
	tclerrmsg((અક્ષર *)str);
    નહીંતર
    {
	Tcl_SetErrno(EBADF);
	જો (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    પાછા result;
}

    સ્થિર શૂન્ય
tcl_channel_watch(ClientData instance UNUSED, પૂર્ણાંક mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    સ્થિર પૂર્ણાંક
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    પૂર્ણાંક		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    પાછા EINVAL;
}


સ્થિર Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_VERSION_2, // version
    tcl_channel_close,	// closeProc
    tcl_channel_આગત,	// inputProc
    tcl_channel_output,	// outputProc
    ખાલી,		// seekProc
    ખાલી,		// setOptionProc
    ખાલી,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    ખાલી,		// close2Proc
    ખાલી,		// blockModeProc
#ifdef TCL_CHANNEL_VERSION_2
    ખાલી,		// flushProc
    ખાલી,		// handlerProc
#endif
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ifdef TCL_CHANNEL_VERSION_3
    ખાલી,		// wideSeekProc
#endif
#ifdef TCL_CHANNEL_VERSION_4
    ખાલી,		// threadActionProc
#endif
#ifdef TCL_CHANNEL_VERSION_5
    ખાલી		// truncateProc
#endif
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    સ્થિર શૂન્ય
tclupdatevars(શૂન્ય)
{
    અક્ષર varname[VARNAME_SIZE];	// must be writeable
    અક્ષર *name;

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


    સ્થિર પૂર્ણાંક
tclinit(exarg_T *eap)
{
    અક્ષર varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    અક્ષર *name;

#ifdef DYNAMIC_TCL
    જો (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	પાછા FAIL;
    }
#endif

    જો (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	સ્થિર Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_SetStdChannel(ch1, TCL_STDOUT);
	Tcl_SetStdChannel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	જો (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    પાછા FAIL;
	}
#જો 0
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
	    (ClientData)ખાલી, (Tcl_CmdDeleteProc *)ખાલી);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (ClientData)ખાલી, (Tcl_CmdDeleteProc *)ખાલી);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (ClientData)ખાલી, (Tcl_CmdDeleteProc *)ખાલી);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (ClientData)ખાલી, (Tcl_CmdDeleteProc *)ખાલી);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (ClientData)ખાલી, (Tcl_CmdDeleteProc *)ખાલી);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (ClientData)ખાલી, (Tcl_CmdDeleteProc *)ખાલી);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (ClientData)ખાલી, (Tcl_CmdDeleteProc *)ખાલી);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (અક્ષર *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (અક્ષર *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (અક્ષર *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (અક્ષર *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (અક્ષર *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (અક્ષર *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    નહીંતર
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    પાછા OK;
}

    સ્થિર શૂન્ય
tclerrmsg(અક્ષર *text)
{
    અક્ષર *next;

    જ્યારે ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    જો (*text)
	emsg(text);
}

    સ્થિર શૂન્ય
tclmsg(અક્ષર *text)
{
    અક્ષર *next;

    જ્યારે ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	સંદેશ(text);
	text = next;
    }
    જો (*text)
	સંદેશ(text);
}

    સ્થિર શૂન્ય
tcldelthisinterp(શૂન્ય)
{
    જો (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = ખાલી;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = ખાલી;
}

    સ્થિર પૂર્ણાંક
tclexit(પૂર્ણાંક error)
{
    પૂર્ણાંક newerr = OK;

    જો (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#જો (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	અક્ષર buf[50];

	sprintf(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	જો (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	નહીંતર
	    newerr = FAIL;

	tcldelthisinterp();
    }
    નહીંતર
    {
	અક્ષર *result;

	result = (અક્ષર *)Tcl_GetStringResult(tclinfo.interp);
	જો (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	નહીંતર
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    પાછા newerr;
}

/*
 * ":tcl"
 */
    શૂન્ય
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    પૂર્ણાંક		err;

    script = script_get(eap, eap->arg);
    જો (!eap->skip)
    {
	err = tclinit(eap);
	જો (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    જો (script == ખાલી)
		err = Tcl_Eval(tclinfo.interp, (અક્ષર *)eap->arg);
	    નહીંતર
		err = Tcl_Eval(tclinfo.interp, (અક્ષર *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    શૂન્ય
ex_tclfile(exarg_T *eap)
{
    અક્ષર *file = (અક્ષર *)eap->arg;
    પૂર્ણાંક err;

    err = tclinit(eap);
    જો (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    શૂન્ય
ex_tcldo(exarg_T *eap)
{
    અક્ષર	*script, *લીટી;
    પૂર્ણાંક		err, rs, re, lnum;
    અક્ષર	var_lnum[VARNAME_SIZE]; // must be writeable memory
    અક્ષર	var_લીટી[VARNAME_SIZE];
    linenr_T	first_લીટી = 0;
    linenr_T	last_લીટી = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (અક્ષર *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_લીટી, VAR_CURLINE);

    err = tclinit(eap);
    જો (err != OK)
	પાછા;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (અક્ષર *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    જો (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    જ્યારે (err == TCL_OK  &&  rs <= re)
    {
	જો ((linenr_T)rs > curbuf->b_ml.ml_લીટી_ગણતરી)
	    તોડો;
	લીટી = (અક્ષર *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	જો (!લીટી)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    તોડો;
	}
	Tcl_SetVar(tclinfo.interp, var_લીટી, લીટી, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	જો (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#જો (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_લીટી_ગણતરી)
	    તોડો;
	લીટી = (અક્ષર *)Tcl_GetVar(tclinfo.interp, var_લીટી, 0);
	જો (લીટી)
	{
	    જો (ml_replace((linenr_T)rs, (char_u *)લીટી, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		તોડો;
	    }
	    જો (first_લીટી == 0)
		first_લીટી = rs;
	    last_લીટી = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    જો (first_લીટી)
	changed_લીટીઓ(first_લીટી, 0, last_લીટી + 1, (long)0);

    Tcl_UnsetVar(tclinfo.interp, var_લીટી, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    જો (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (શૂન્ય)tclexit(err);
}

    સ્થિર શૂન્ય
tcldelallrefs(સંરચના ref *ref)
{
    સંરચના ref	*next;
    પૂર્ણાંક		err;
    અક્ષર	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    જો (exiting)
	પાછા;
#endif

    જ્યારે (ref != ખાલી)
    {
	next = ref->next;
	જો (ref->interp)
	{
	    જો (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		જો (err != TCL_OK)
		{
		    result = (અક્ષર *)Tcl_GetStringResult(ref->interp);
		    જો (result)
			tclerrmsg(result);
		}
		Tcl_Decr_Ref_ગણતરી(ref->delcmd);
		ref->delcmd = ખાલી;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((અક્ષર *)ref);
	ref = next;
    }
}

    શૂન્ય
tcl_buffer_free(buf_T *buf)
{
    સંરચના ref *reflist;

#ifdef DYNAMIC_TCL
    જો (!stubs_initialized)	// Not using Tcl, nothing to do.
	પાછા;
#endif

    reflist = (સંરચના ref *)(buf->b_tcl_ref);
    જો (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (શૂન્ય *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = ખાલી;
    }
}

    શૂન્ય
tcl_window_free(win_T *win)
{
    સંરચના ref *reflist;

#ifdef DYNAMIC_TCL
    જો (!stubs_initialized)	// Not using Tcl, nothing to do.
	પાછા;
#endif

    reflist = (સંરચના ref*)(win->w_tcl_ref);
    જો (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (શૂન્ય *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = ખાલી;
    }
}

// The End
