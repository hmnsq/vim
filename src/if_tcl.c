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

#അന്തര്ഭാവ "vim.h"
#undef EXTERN			// tcl.h defines it too

#ifdef DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#endif

#അന്തര്ഭാവ <tcl.h>
#അന്തര്ഭാവ <string.h>

തരനിർവചനം ഘടന
{
    Tcl_Interp *interp;
    പൂർണ്ണസംഖ്യ exitvalue;
    പൂർണ്ണസംഖ്യ range_start, range_end;
    പൂർണ്ണസംഖ്യ lbase;
    അക്ഷരം *curbuf, *curwin;
} tcl_info;

സ്ഥിതം tcl_info tclinfo = { ശൂന്യ, 0, 0, 0, 0, ശൂന്യ, ശൂന്യ };

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
ഘടന ref
{
    ഘടന ref	*next;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    ശൂന്യം	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
സ്ഥിതം അക്ഷരം * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
സ്ഥിതം അക്ഷരം * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
സ്ഥിതം പൂർണ്ണസംഖ്യ tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, ഘടന ref *reflist, ശൂന്യം *vimobj, Tcl_Obj *delcmd));
സ്ഥിതം പൂർണ്ണസംഖ്യ tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, പൂർണ്ണസംഖ്യ *മൂല്യം_P, buf_T *buf));
സ്ഥിതം win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
സ്ഥിതം പൂർണ്ണസംഖ്യ tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, പൂർണ്ണസംഖ്യ objc, Tcl_Obj *CONST objv[], പൂർണ്ണസംഖ്യ objn));
സ്ഥിതം പൂർണ്ണസംഖ്യ tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, പൂർണ്ണസംഖ്യ objc, Tcl_Obj *CONST objv[], പൂർണ്ണസംഖ്യ objn));
സ്ഥിതം പൂർണ്ണസംഖ്യ tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, പൂർണ്ണസംഖ്യ objc, Tcl_Obj *CONST objv[], പൂർണ്ണസംഖ്യ objn));
സ്ഥിതം ശൂന്യം tcldelthisinterp _ANSI_ARGS_ ((ശൂന്യം));

സ്ഥിതം പൂർണ്ണസംഖ്യ vimerror _ANSI_ARGS_((Tcl_Interp *interp));
സ്ഥിതം ശൂന്യം tclmsg _ANSI_ARGS_((അക്ഷരം *text));
സ്ഥിതം ശൂന്യം tclerrmsg _ANSI_ARGS_((അക്ഷരം *text));
സ്ഥിതം ശൂന്യം tclupdatevars _ANSI_ARGS_((ശൂന്യം));

സ്ഥിതം ഘടന ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#എങ്കിൽ defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
തരനിർവചനം പൂർണ്ണസംഖ്യ HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define ചിഹ്നം_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# അല്ലെങ്കിൽ
#  include <dlfcn.h>
#  define HANDLE ശൂന്യം*
#  define TCL_PROC ശൂന്യം*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define ചിഹ്നം_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
സ്ഥിതം HANDLE hTclLib = ശൂന്യ;
Tcl_Interp* (*dll_Tcl_CreateInterp)(ശൂന്യം);
ശൂന്യം (*dll_Tcl_FindExecutable)(സ്ഥിരം ശൂന്യം *);

/*
 * Table of name to function pointer of tcl.
 */
സ്ഥിതം ഘടന {
    അക്ഷരം* name;
    TCL_PROC* ptr;
} tcl_funcname_table[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_FindExecutable},
    {ശൂന്യ, ശൂന്യ},
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tcl_runtime_link_init(അക്ഷരം *libname, പൂർണ്ണസംഖ്യ verbose)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (hTclLib)
	തിരികെ OK;
    എങ്കിൽ (!(hTclLib = load_dll(libname)))
    {
	എങ്കിൽ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	തിരികെ FAIL;
    }
    വേണ്ടി (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	എങ്കിൽ (!(*tcl_funcname_table[i].ptr = ചിഹ്നം_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = ശൂന്യ;
	    എങ്കിൽ (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    തിരികെ FAIL;
	}
    }
    തിരികെ OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
സ്ഥിതം അക്ഷരം *find_executable_arg = ശൂന്യ;
#endif

    ശൂന്യം
vim_tcl_init(അക്ഷരം *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#അല്ലെങ്കിൽ
    Tcl_FindExecutable(arg);
#endif
}

#എങ്കിൽ defined(DYNAMIC_TCL)

സ്ഥിതം പൂർണ്ണസംഖ്യ stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    പൂർണ്ണസംഖ്യ
tcl_enabled(പൂർണ്ണസംഖ്യ verbose)
{
    എങ്കിൽ (!stubs_initialized && find_executable_arg != ശൂന്യ
	    && tcl_runtime_link_init((അക്ഷരം *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	എങ്കിൽ ((interp = dll_Tcl_CreateInterp()) != ശൂന്യ)
	{
	    എങ്കിൽ (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != ശൂന്യ)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    തിരികെ stubs_initialized;
}
#endif

#എങ്കിൽ defined(EXITFREE)
/*
 * Called once when exiting.
 */
    ശൂന്യം
vim_tcl_finalize(ശൂന്യം)
{
# ifdef DYNAMIC_TCL
    എങ്കിൽ (stubs_initialized)
# endif
	Tcl_Finalize();
}
#endif

    ശൂന്യം
tcl_end(ശൂന്യം)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    പൂർണ്ണസംഖ്യ objc,
    Tcl_Obj *CONST objv[])
{
    പൂർണ്ണസംഖ്യ മൂല്യം = 0;

    തിരഞ്ഞെടുപ്പ് (objc)
    {
	അവസ്ഥ 2:
	    എങ്കിൽ (Tcl_GetIntFromObj(interp, objv[1], &മൂല്യം) != TCL_OK)
		നിർത്തുക;
	    // FALLTHROUGH
	അവസ്ഥ 1:
	    tclinfo.exitvalue = മൂല്യം;

	    Tcl_DeleteInterp(interp);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    Tcl_Wrong_അക്കം_Args(interp, 1, objv, "?returnCode?");
    }
    തിരികെ TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    പൂർണ്ണസംഖ്യ objc,
    Tcl_Obj *CONST objv[])
{
    എങ്കിൽ (objc != 1)
    {
	Tcl_Wrong_അക്കം_Args(interp, 1, objv, ശൂന്യ);
	തിരികെ TCL_ERROR;
    }
    vim_beep(BO_LANG);
    തിരികെ TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    പൂർണ്ണസംഖ്യ objc,
    Tcl_Obj *CONST objv[])
{
    അക്ഷരം	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    പൂർണ്ണസംഖ്യ		err, n, idx;
    എണ്ണൽ {BCMD_EXISTS, BCMD_LIST};
    സ്ഥിതം CONST84 അക്ഷരം *bcmdoptions[] =
    {
	"exists", "list", (അക്ഷരം *)0
    };

    എങ്കിൽ (objc < 2)
    {
	Tcl_Wrong_അക്കം_Args(interp, 1, objv, "option");
	തിരികെ TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    എങ്കിൽ (err == TCL_OK)
    {
	എങ്കിൽ (objc != 2)
	{
	    Tcl_Wrong_അക്കം_Args(interp, 1, objv, "bufNumber");
	    തിരികെ TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    എങ്കിൽ (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		എങ്കിൽ (name == ശൂന്യ)
		    തിരികെ TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		തിരികെ TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	തിരികെ TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    എങ്കിൽ (err != TCL_OK)
	തിരികെ err;
    തിരഞ്ഞെടുപ്പ് (idx)
    {
	അവസ്ഥ BCMD_LIST:
	    എങ്കിൽ (objc != 2)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, "");
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		എങ്കിൽ (name == ശൂന്യ)
		{
		    err = TCL_ERROR;
		    നിർത്തുക;
		}
		Tcl_AppendElement(interp, name);
	    }
	    നിർത്തുക;

	അവസ്ഥ BCMD_EXISTS:
	    എങ്കിൽ (objc != 3)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    എങ്കിൽ (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != ശൂന്യ);
		Tcl_SetObjResult(interp, resobj);
	    }
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    തിരികെ err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    പൂർണ്ണസംഖ്യ		objc,
    Tcl_Obj	*CONST objv[])
{
    അക്ഷരം	*what, *string;
    win_T	*win;

    എങ്കിൽ (objc != 2)
    {
	Tcl_Wrong_അക്കം_Args(interp, 1, objv, "option");
	തിരികെ TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], ശൂന്യ);
    എങ്കിൽ (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    എങ്കിൽ (string == ശൂന്യ)
		തിരികെ TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	തിരികെ TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    തിരികെ TCL_ERROR;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    പൂർണ്ണസംഖ്യ		objc,
    Tcl_Obj	*CONST objv[])
{
    പൂർണ്ണസംഖ്യ		opt, err, idx, flags;
    പൂർണ്ണസംഖ്യ		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    അക്ഷരം	*വരി;

    എണ്ണൽ
    {
	BUF_APPEND, BUF_COMMAND, BUF_എണ്ണം, BUF_DELCMD, BUF_DELETE, BUF_വ്യഞ്ജകം,
	BUF_GET, BUF_തിരുകുക, BUF_LAST, BUF_MARK, BUF_NAME, BUF_സംഖ്യ,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    സ്ഥിതം CONST84 അക്ഷരം *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (അക്ഷരം *)0
    };

    എങ്കിൽ (objc < 2)
    {
	Tcl_Wrong_അക്കം_Args(interp, 1, objv, "option ?arg ...?");
	തിരികെ TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    എങ്കിൽ (err != TCL_OK)
	തിരികെ err;

    buf = (buf_T *)((ഘടന ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    തിരഞ്ഞെടുപ്പ് (idx)
    {
	അവസ്ഥ BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    നിർത്തുക;

	അവസ്ഥ BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    നിർത്തുക;

	അവസ്ഥ BUF_വ്യഞ്ജകം:
	    err = tclvimexpr(interp, objc, objv, 2);
	    നിർത്തുക;

	അവസ്ഥ BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    എങ്കിൽ (objc != 2)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, ശൂന്യ);
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    എങ്കിൽ (buf->b_ffname)
		Tcl_SetResult(interp, (അക്ഷരം *)buf->b_ffname, TCL_VOLATILE);
	    അല്ലെങ്കിൽ
		Tcl_SetResult(interp, "", TCL_STATIC);
	    നിർത്തുക;

	അവസ്ഥ BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	അവസ്ഥ BUF_എണ്ണം:
	    /*
	     * Get number of lines in buffer.
	     */
	    എങ്കിൽ (objc != 2)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, ശൂന്യ);
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    val1 = (പൂർണ്ണസംഖ്യ)buf->b_ml.ml_വരി_എണ്ണം;
	    എങ്കിൽ (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    നിർത്തുക;

	അവസ്ഥ BUF_സംഖ്യ:
	    /*
	     * Get buffer's number.
	     */
	    എങ്കിൽ (objc != 2)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, ശൂന്യ);
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    resobj = Tcl_NewIntObj((പൂർണ്ണസംഖ്യ)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    നിർത്തുക;

	അവസ്ഥ BUF_GET:
	    എങ്കിൽ (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    എങ്കിൽ (err != TCL_OK)
		നിർത്തുക;
	    എങ്കിൽ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		എങ്കിൽ (err != TCL_OK)
		    നിർത്തുക;
		എങ്കിൽ (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		വേണ്ടി (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    വരി = (അക്ഷരം *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    എങ്കിൽ (വരി)
			Tcl_AppendElement(interp, വരി);
		    അല്ലെങ്കിൽ
			err = TCL_ERROR;
		}
	    }
	    അല്ലെങ്കിൽ
	    {  // objc == 3
		വരി = (അക്ഷരം *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, വരി, TCL_VOLATILE);
	    }
	    നിർത്തുക;

	അവസ്ഥ BUF_SET:
	    എങ്കിൽ (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    എങ്കിൽ (err != TCL_OK)
		തിരികെ TCL_ERROR;
	    എങ്കിൽ (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		വരി = Tcl_GetStringFromObj(objv[3], ശൂന്യ);
		എങ്കിൽ (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		അല്ലെങ്കിൽ
		എങ്കിൽ (ml_replace((linenr_T)val1, (char_u *)വരി, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		അല്ലെങ്കിൽ
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		പൂർണ്ണസംഖ്യ	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		എങ്കിൽ (err != TCL_OK)
		    നിർത്തുക;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		എങ്കിൽ (err != TCL_OK)
		    നിർത്തുക;
		എങ്കിൽ (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		എങ്കിൽ (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    നിർത്തുക;
		}
		flags |= FL_UPDATE_CURBUF;

		വേണ്ടി (i = 0; i < lc && n <= val2; i++)
		{
		    വരി = Tcl_GetStringFromObj(lv[i], ശൂന്യ);
		    എങ്കിൽ (ml_replace((linenr_T)n, (char_u *)വരി, TRUE) != OK)
			പോകുക setListError;
		    ++n;
		}
		എങ്കിൽ (i < lc)
		{
		    // append lines
		    ചെയ്യുക
		    {
			വരി = Tcl_GetStringFromObj(lv[i], ശൂന്യ);
			എങ്കിൽ (ml_append((linenr_T)(n - 1),
					      (char_u *)വരി, 0, FALSE) != OK)
			    പോകുക setListError;
			++n;
			++i;
		    } ആകുമ്പോൾ (i < lc);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    ചെയ്യുക
		    {
			എങ്കിൽ (ml_delete((linenr_T)i) != OK)
			    പോകുക setListError;
			++n;
		    } ആകുമ്പോൾ (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (long)MAXLNUM,
								    (long)lc);
		changed_വരികൾ((linenr_T)val1, 0, (linenr_T)val2 + 1, (long)lc);
		നിർത്തുക;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    നിർത്തുക;

	അവസ്ഥ BUF_DELETE:
	    എങ്കിൽ (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    എങ്കിൽ (err != TCL_OK)
		നിർത്തുക;
	    val2 = val1;
	    എങ്കിൽ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		എങ്കിൽ (err != TCL_OK)
		    തിരികെ err;
		എങ്കിൽ (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    എങ്കിൽ (u_savedel((linenr_T)val1, (long)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    വേണ്ടി (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		എങ്കിൽ (err != TCL_OK)
		    നിർത്തുക;
	    }
	    എങ്കിൽ (i > 0)
		deleted_വരികൾ_mark((linenr_T)val1, (long)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    നിർത്തുക;

	അവസ്ഥ BUF_MARK:
	    എങ്കിൽ (objc != 3)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, "markName");
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    വരി = Tcl_GetStringFromObj(objv[2], ശൂന്യ);

	    pos = ശൂന്യ;
	    എങ്കിൽ (വരി[0] != '\0'  &&  വരി[1] == '\0')
		pos = getmark(വരി[0], FALSE);
	    എങ്കിൽ (pos == ശൂന്യ)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    err = vimerror(interp);
	    എങ്കിൽ (err != TCL_OK)
		നിർത്തുക;
	    എങ്കിൽ (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    അല്ലെങ്കിൽ
	    {
		അക്ഷരം rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (പൂർണ്ണസംഖ്യ)row2tcl(pos->lnum), (പൂർണ്ണസംഖ്യ)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    നിർത്തുക;

	അവസ്ഥ BUF_തിരുകുക:
	    opt = 1;
	    // fallthrough
	അവസ്ഥ BUF_APPEND:
	    എങ്കിൽ (objc != 4)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    എങ്കിൽ (err != TCL_OK)
		നിർത്തുക;
	    എങ്കിൽ (opt)
		--val1;
	    എങ്കിൽ (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		നിർത്തുക;
	    }

	    വരി = Tcl_GetStringFromObj(objv[3], ശൂന്യ);
	    എങ്കിൽ (ml_append((linenr_T)val1, (char_u *)വരി, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    appended_വരികൾ_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    നിർത്തുക;

	അവസ്ഥ BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    എങ്കിൽ (objc != 2)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, ശൂന്യ);
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		എങ്കിൽ (win->w_buffer == buf)
		{
		    വരി = tclgetwindow(interp, win);
		    എങ്കിൽ (വരി != ശൂന്യ)
			Tcl_AppendElement(interp, വരി);
		    അല്ലെങ്കിൽ
		    {
			err = TCL_ERROR;
			നിർത്തുക;
		    }
		}
	    }
	    നിർത്തുക;

	അവസ്ഥ BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    എങ്കിൽ (objc != 3)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (ശൂന്യം *)buf, objv[2]);
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    എങ്കിൽ (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    എങ്കിൽ (flags & FL_ADJUST_CURSOR)
	check_cursor();
    എങ്കിൽ (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    തിരികെ err;
}

/*
 * This function implements the window commands.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    പൂർണ്ണസംഖ്യ		objc,
    Tcl_Obj	*CONST objv[])
{
    പൂർണ്ണസംഖ്യ		err, idx, flags;
    പൂർണ്ണസംഖ്യ		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    അക്ഷരം	*str;

    എണ്ണൽ
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_വ്യഞ്ജകം,
	WIN_HEIGHT, WIN_OPTION
    };
    സ്ഥിതം CONST84 അക്ഷരം *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (അക്ഷരം *)0
    };

    എങ്കിൽ (objc < 2)
    {
	Tcl_Wrong_അക്കം_Args(interp, 1, objv, "option ?arg ...?");
	തിരികെ TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    എങ്കിൽ (err != TCL_OK)
	തിരികെ TCL_ERROR;

    win = (win_T *)((ഘടന ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    തിരഞ്ഞെടുപ്പ് (idx)
    {
	അവസ്ഥ WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    നിർത്തുക;

	അവസ്ഥ WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    നിർത്തുക;

	അവസ്ഥ WIN_വ്യഞ്ജകം:
	    err = tclvimexpr(interp, objc, objv, 2);
	    നിർത്തുക;

	അവസ്ഥ WIN_HEIGHT:
	    എങ്കിൽ (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		എങ്കിൽ (err != TCL_OK)
		    നിർത്തുക;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		എങ്കിൽ (err != TCL_OK)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ
	    എങ്കിൽ (objc != 2)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		നിർത്തുക;
	    }

	    resobj = Tcl_NewIntObj((പൂർണ്ണസംഖ്യ)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    നിർത്തുക;

	അവസ്ഥ WIN_BUFFER:
	    എങ്കിൽ (objc != 2)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, ശൂന്യ);
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    എങ്കിൽ (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    അല്ലെങ്കിൽ
		err = TCL_ERROR;
	    നിർത്തുക;

	അവസ്ഥ WIN_DELCMD:
	    എങ്കിൽ (objc != 3)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (ശൂന്യം *)win, objv[2]);
	    നിർത്തുക;

	അവസ്ഥ WIN_CURSOR:
	    എങ്കിൽ (objc > 4)
	    {
		Tcl_Wrong_അക്കം_Args(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    എങ്കിൽ (objc == 2)
	    {
		അക്ഷരം buf[64];

		sprintf(buf, _("row %d column %d"), (പൂർണ്ണസംഖ്യ)row2tcl(win->w_cursor.lnum), (പൂർണ്ണസംഖ്യ)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_സന്ദേശം);
		എങ്കിൽ (var == ശൂന്യ)
		{
		    err = TCL_ERROR;
		    നിർത്തുക;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		എങ്കിൽ (err != TCL_OK)
		    നിർത്തുക;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_സന്ദേശം);
		എങ്കിൽ (var == ശൂന്യ)
		{
		    err = TCL_ERROR;
		    നിർത്തുക;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		എങ്കിൽ (err != TCL_OK)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		എങ്കിൽ (err != TCL_OK)
		    നിർത്തുക;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		എങ്കിൽ (err != TCL_OK)
		    നിർത്തുക;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    നിർത്തുക;
    }

    curwin = savewin;
    curbuf = savebuf;
    എങ്കിൽ (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    തിരികെ err;
}


    സ്ഥിതം പൂർണ്ണസംഖ്യ
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    പൂർണ്ണസംഖ്യ		objc,
    Tcl_Obj	*CONST objv[])
{
    പൂർണ്ണസംഖ്യ		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    തിരികെ err;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    പൂർണ്ണസംഖ്യ		objc,
    Tcl_Obj	*CONST objv[])
{
    പൂർണ്ണസംഖ്യ		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    തിരികെ err;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    പൂർണ്ണസംഖ്യ		objc,
    Tcl_Obj	*CONST objv[])
{
    തിരികെ tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    പൂർണ്ണസംഖ്യ		*മൂല്യം_P,
    buf_T	*buf)
{
    പൂർണ്ണസംഖ്യ err, i;

    എണ്ണൽ { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_മുകൾ };

    സ്ഥിതം CONST84 അക്ഷരം *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (അക്ഷരം *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    എങ്കിൽ (err == TCL_OK)
    {
	തിരഞ്ഞെടുപ്പ് (i)
	{
	    അവസ്ഥ LN_BEGIN:
	    അവസ്ഥ LN_FIRST:
	    അവസ്ഥ LN_START:
	    അവസ്ഥ LN_മുകൾ:
		*മൂല്യം_P = 1;
		നിർത്തുക;
	    അവസ്ഥ LN_BOTTOM:
	    അവസ്ഥ LN_END:
	    അവസ്ഥ LN_LAST:
		*മൂല്യം_P = buf->b_ml.ml_വരി_എണ്ണം;
		നിർത്തുക;
	}
	തിരികെ TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    എങ്കിൽ (err != TCL_OK)
	തിരികെ err;
    i = row2vim(i);
    എങ്കിൽ (i < 1  ||  i > buf->b_ml.ml_വരി_എണ്ണം)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	തിരികെ TCL_ERROR;
    }
    *മൂല്യം_P = i;
    തിരികെ TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    സ്ഥിതം win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	എങ്കിൽ (win->w_buffer == buf)
	    തിരികെ win;
    }
    തിരികെ curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tcldoexcommand(
    Tcl_Interp	*interp,
    പൂർണ്ണസംഖ്യ		objc,
    Tcl_Obj	*CONST objv[],
    പൂർണ്ണസംഖ്യ		objn)
{
    tcl_info	saveinfo;
    പൂർണ്ണസംഖ്യ		err, flag, nobjs;
    അക്ഷരം	*arg;

    nobjs = objc - objn;
    എങ്കിൽ (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_അക്കം_Args(interp, objn, objv, "?-quiet? exCommand");
	തിരികെ TCL_ERROR;
    }

    flag = 0;
    എങ്കിൽ (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], ശൂന്യ);
	എങ്കിൽ (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	അല്ലെങ്കിൽ
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (അക്ഷരം *)0);
	    തിരികെ TCL_ERROR;
	}
	++objn;
    }

    memcpy(&saveinfo, &tclinfo, വലുപ്പം(tcl_info));
    tclinfo.interp = ശൂന്യ;
    tclinfo.curwin = ശൂന്യ;
    tclinfo.curbuf = ശൂന്യ;

    arg = Tcl_GetStringFromObj(objv[objn], ശൂന്യ);
    എങ്കിൽ (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    എങ്കിൽ (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    എങ്കിൽ (tclinfo.interp)
	tcldelthisinterp();
    memcpy(&tclinfo, &saveinfo, വലുപ്പം(tcl_info));
    tclupdatevars();

    തിരികെ err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tclsetoption(
    Tcl_Interp	*interp,
    പൂർണ്ണസംഖ്യ		objc,
    Tcl_Obj	*CONST objv[],
    പൂർണ്ണസംഖ്യ		objn)
{
    പൂർണ്ണസംഖ്യ		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    long	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    എണ്ണൽ { OPT_OFF, OPT_ON, OPT_TOGGLE };
    സ്ഥിതം CONST84 അക്ഷരം *optkw[] = { "off", "on", "toggle", (അക്ഷരം *)0 };

    nobjs = objc - objn;
    എങ്കിൽ (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_അക്കം_Args(interp, objn, objv, "vimOption ?value?");
	തിരികെ TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], ശൂന്യ);
    ++objn;
    gov = get_option_മൂല്യം(option, &lval, &sval, ശൂന്യ, 0);
    err = TCL_OK;
    തിരഞ്ഞെടുപ്പ് (gov)
    {
	അവസ്ഥ gov_string:
	    Tcl_SetResult(interp, (അക്ഷരം *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    നിർത്തുക;
	അവസ്ഥ gov_bool:
	അവസ്ഥ gov_സംഖ്യ:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    തിരികെ TCL_ERROR;
    }
    എങ്കിൽ (nobjs == 2)
    {
	എങ്കിൽ (gov != gov_string)
	{
	    sval = ശൂന്യ;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    എങ്കിൽ (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    അല്ലെങ്കിൽ
	    {
		തിരഞ്ഞെടുപ്പ് (idx)
		{
		    അവസ്ഥ OPT_ON:
			lval = 1;
			നിർത്തുക;
		    അവസ്ഥ OPT_OFF:
			lval = 0;
			നിർത്തുക;
		    അവസ്ഥ OPT_TOGGLE:
			lval = !lval;
			നിർത്തുക;
		}
	    }
	}
	അല്ലെങ്കിൽ
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], ശൂന്യ);
	എങ്കിൽ (err == TCL_OK)
	{
	    set_option_മൂല്യം_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    തിരികെ err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tclvimexpr(
    Tcl_Interp	*interp,
    പൂർണ്ണസംഖ്യ		objc,
    Tcl_Obj	*CONST objv[],
    പൂർണ്ണസംഖ്യ		objn)
{
#ifdef FEAT_EVAL
    അക്ഷരം	*വ്യഞ്ജകം, *str;
#endif
    പൂർണ്ണസംഖ്യ		err;

    എങ്കിൽ (objc - objn != 1)
    {
	Tcl_Wrong_അക്കം_Args(interp, objn, objv, "vimExpr");
	തിരികെ TCL_ERROR;
    }

#ifdef FEAT_EVAL
    വ്യഞ്ജകം = Tcl_GetStringFromObj(objv[objn], ശൂന്യ);
    str = (അക്ഷരം *)eval_to_string((char_u *)വ്യഞ്ജകം, TRUE, FALSE);
    എങ്കിൽ (str == ശൂന്യ)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    അല്ലെങ്കിൽ
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#അല്ലെങ്കിൽ
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    തിരികെ err;
}

/*
 * Check for internal vim errors.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vimerror(Tcl_Interp *interp)
{
    എങ്കിൽ (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	തിരികെ TCL_ERROR;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	തിരികെ TCL_ERROR;
    }
    തിരികെ TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    സ്ഥിതം ശൂന്യം
delref(ClientData cref)
{
    ഘടന ref *ref = (ഘടന ref *)cref;

    എങ്കിൽ (ref->delcmd)
    {
	Tcl_Decr_Ref_എണ്ണം(ref->delcmd);
	ref->delcmd = ശൂന്യ;
    }
    ref->interp = ശൂന്യ;
}

    സ്ഥിതം അക്ഷരം *
tclgetref(
    Tcl_Interp	*interp,
    ശൂന്യം	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    അക്ഷരം	*prefix,	// "win" or "buf"
    ശൂന്യം	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    ഘടന ref *ref, *unused = ശൂന്യ;
    സ്ഥിതം അക്ഷരം name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (ഘടന ref *)(*refstartP);
    എങ്കിൽ (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	തിരികെ ശൂന്യ;
    }

    ആകുമ്പോൾ (ref != ശൂന്യ)
    {
	എങ്കിൽ (ref->interp == interp)
	    നിർത്തുക;
	എങ്കിൽ (ref->interp == ശൂന്യ)
	    unused = ref;
	ref = ref->next;
    }

    എങ്കിൽ (ref)
	vim_snprintf(name, വലുപ്പം(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (unused)
	    ref = unused;
	അല്ലെങ്കിൽ
	{
	    ref = (ഘടന ref *)Tcl_Alloc(വലുപ്പം(ഘടന ref));
	    ref->interp = ശൂന്യ;
	    ref->next = (ഘടന ref *)(*refstartP);
	    (*refstartP) = (ശൂന്യം *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, വലുപ്പം(name), "::vim::%s_%lx",
					       prefix, (unsigned long)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	എങ്കിൽ (!cmd)
	    തിരികെ ശൂന്യ;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = ശൂന്യ;
	ref->vimobj = vimobj;
    }
    തിരികെ name;
}

    സ്ഥിതം അക്ഷരം *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    തിരികെ tclgetref(interp, &(win->w_tcl_ref), "win", (ശൂന്യം *)win, winselfcmd);
}

    സ്ഥിതം അക്ഷരം *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    തിരികെ tclgetref(interp, &(buf->b_tcl_ref), "buf", (ശൂന്യം *)buf, bufselfcmd);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
tclsetdelcmd(
    Tcl_Interp	*interp,
    ഘടന ref	*reflist,
    ശൂന്യം	*vimobj,
    Tcl_Obj	*delcmd)
{
    എങ്കിൽ (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	തിരികെ TCL_ERROR;
    }

    ആകുമ്പോൾ (reflist != ശൂന്യ)
    {
	എങ്കിൽ (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    എങ്കിൽ (reflist->delcmd)
		Tcl_Decr_Ref_എണ്ണം(reflist->delcmd);
	    Tcl_Incr_Ref_എണ്ണം(delcmd);
	    reflist->delcmd = delcmd;
	    തിരികെ TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    തിരികെ TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    സ്ഥിതം പൂർണ്ണസംഖ്യ
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    പൂർണ്ണസംഖ്യ		err = 0;

    // currently does nothing

    എങ്കിൽ (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    തിരികെ err;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
tcl_channel_നിവേശം(
    ClientData	instance UNUSED,
    അക്ഷരം	*buf UNUSED,
    പൂർണ്ണസംഖ്യ		bufsiz UNUSED,
    പൂർണ്ണസംഖ്യ		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    എങ്കിൽ (errptr)
	*errptr = EINVAL;
    തിരികെ -1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
tcl_channel_output(
    ClientData	instance,
    സ്ഥിരം അക്ഷരം	*buf,
    പൂർണ്ണസംഖ്യ		bufsiz,
    പൂർണ്ണസംഖ്യ		*errptr)
{
    char_u	*str;
    പൂർണ്ണസംഖ്യ		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    എങ്കിൽ (!str)
    {
	Tcl_SetErrno(ENOMEM);
	എങ്കിൽ (errptr)
	    *errptr = ENOMEM;
	തിരികെ -1;
    }

    result = bufsiz;
    എങ്കിൽ (instance == VIMOUT)
	tclmsg((അക്ഷരം *)str);
    അല്ലെങ്കിൽ
    എങ്കിൽ (instance == VIMERR)
	tclerrmsg((അക്ഷരം *)str);
    അല്ലെങ്കിൽ
    {
	Tcl_SetErrno(EBADF);
	എങ്കിൽ (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    തിരികെ result;
}

    സ്ഥിതം ശൂന്യം
tcl_channel_watch(ClientData instance UNUSED, പൂർണ്ണസംഖ്യ mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    പൂർണ്ണസംഖ്യ		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    തിരികെ EINVAL;
}


സ്ഥിതം Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_VERSION_2, // version
    tcl_channel_close,	// closeProc
    tcl_channel_നിവേശം,	// inputProc
    tcl_channel_output,	// outputProc
    ശൂന്യ,		// seekProc
    ശൂന്യ,		// setOptionProc
    ശൂന്യ,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    ശൂന്യ,		// close2Proc
    ശൂന്യ,		// blockModeProc
#ifdef TCL_CHANNEL_VERSION_2
    ശൂന്യ,		// flushProc
    ശൂന്യ,		// handlerProc
#endif
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ifdef TCL_CHANNEL_VERSION_3
    ശൂന്യ,		// wideSeekProc
#endif
#ifdef TCL_CHANNEL_VERSION_4
    ശൂന്യ,		// threadActionProc
#endif
#ifdef TCL_CHANNEL_VERSION_5
    ശൂന്യ		// truncateProc
#endif
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    സ്ഥിതം ശൂന്യം
tclupdatevars(ശൂന്യം)
{
    അക്ഷരം varname[VARNAME_SIZE];	// must be writeable
    അക്ഷരം *name;

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


    സ്ഥിതം പൂർണ്ണസംഖ്യ
tclinit(exarg_T *eap)
{
    അക്ഷരം varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    അക്ഷരം *name;

#ifdef DYNAMIC_TCL
    എങ്കിൽ (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	തിരികെ FAIL;
    }
#endif

    എങ്കിൽ (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	സ്ഥിതം Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_SetStdChannel(ch1, TCL_STDOUT);
	Tcl_SetStdChannel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	എങ്കിൽ (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    തിരികെ FAIL;
	}
#എങ്കിൽ 0
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
	    (ClientData)ശൂന്യ, (Tcl_CmdDeleteProc *)ശൂന്യ);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (ClientData)ശൂന്യ, (Tcl_CmdDeleteProc *)ശൂന്യ);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (ClientData)ശൂന്യ, (Tcl_CmdDeleteProc *)ശൂന്യ);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (ClientData)ശൂന്യ, (Tcl_CmdDeleteProc *)ശൂന്യ);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (ClientData)ശൂന്യ, (Tcl_CmdDeleteProc *)ശൂന്യ);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (ClientData)ശൂന്യ, (Tcl_CmdDeleteProc *)ശൂന്യ);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (ClientData)ശൂന്യ, (Tcl_CmdDeleteProc *)ശൂന്യ);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (അക്ഷരം *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (അക്ഷരം *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (അക്ഷരം *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (അക്ഷരം *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (അക്ഷരം *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (അക്ഷരം *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    അല്ലെങ്കിൽ
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    തിരികെ OK;
}

    സ്ഥിതം ശൂന്യം
tclerrmsg(അക്ഷരം *text)
{
    അക്ഷരം *next;

    ആകുമ്പോൾ ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    എങ്കിൽ (*text)
	emsg(text);
}

    സ്ഥിതം ശൂന്യം
tclmsg(അക്ഷരം *text)
{
    അക്ഷരം *next;

    ആകുമ്പോൾ ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	സന്ദേശം(text);
	text = next;
    }
    എങ്കിൽ (*text)
	സന്ദേശം(text);
}

    സ്ഥിതം ശൂന്യം
tcldelthisinterp(ശൂന്യം)
{
    എങ്കിൽ (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = ശൂന്യ;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = ശൂന്യ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
tclexit(പൂർണ്ണസംഖ്യ error)
{
    പൂർണ്ണസംഖ്യ newerr = OK;

    എങ്കിൽ (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#എങ്കിൽ (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	അക്ഷരം buf[50];

	sprintf(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	എങ്കിൽ (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	അല്ലെങ്കിൽ
	    newerr = FAIL;

	tcldelthisinterp();
    }
    അല്ലെങ്കിൽ
    {
	അക്ഷരം *result;

	result = (അക്ഷരം *)Tcl_GetStringResult(tclinfo.interp);
	എങ്കിൽ (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	അല്ലെങ്കിൽ
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    തിരികെ newerr;
}

/*
 * ":tcl"
 */
    ശൂന്യം
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    പൂർണ്ണസംഖ്യ		err;

    script = script_get(eap, eap->arg);
    എങ്കിൽ (!eap->skip)
    {
	err = tclinit(eap);
	എങ്കിൽ (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    എങ്കിൽ (script == ശൂന്യ)
		err = Tcl_Eval(tclinfo.interp, (അക്ഷരം *)eap->arg);
	    അല്ലെങ്കിൽ
		err = Tcl_Eval(tclinfo.interp, (അക്ഷരം *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    ശൂന്യം
ex_tclfile(exarg_T *eap)
{
    അക്ഷരം *file = (അക്ഷരം *)eap->arg;
    പൂർണ്ണസംഖ്യ err;

    err = tclinit(eap);
    എങ്കിൽ (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    ശൂന്യം
ex_tcldo(exarg_T *eap)
{
    അക്ഷരം	*script, *വരി;
    പൂർണ്ണസംഖ്യ		err, rs, re, lnum;
    അക്ഷരം	var_lnum[VARNAME_SIZE]; // must be writeable memory
    അക്ഷരം	var_വരി[VARNAME_SIZE];
    linenr_T	first_വരി = 0;
    linenr_T	last_വരി = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (അക്ഷരം *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_വരി, VAR_CURLINE);

    err = tclinit(eap);
    എങ്കിൽ (err != OK)
	തിരികെ;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (അക്ഷരം *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    എങ്കിൽ (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    ആകുമ്പോൾ (err == TCL_OK  &&  rs <= re)
    {
	എങ്കിൽ ((linenr_T)rs > curbuf->b_ml.ml_വരി_എണ്ണം)
	    നിർത്തുക;
	വരി = (അക്ഷരം *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	എങ്കിൽ (!വരി)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    നിർത്തുക;
	}
	Tcl_SetVar(tclinfo.interp, var_വരി, വരി, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	എങ്കിൽ (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#എങ്കിൽ (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_വരി_എണ്ണം)
	    നിർത്തുക;
	വരി = (അക്ഷരം *)Tcl_GetVar(tclinfo.interp, var_വരി, 0);
	എങ്കിൽ (വരി)
	{
	    എങ്കിൽ (ml_replace((linenr_T)rs, (char_u *)വരി, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		നിർത്തുക;
	    }
	    എങ്കിൽ (first_വരി == 0)
		first_വരി = rs;
	    last_വരി = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    എങ്കിൽ (first_വരി)
	changed_വരികൾ(first_വരി, 0, last_വരി + 1, (long)0);

    Tcl_UnsetVar(tclinfo.interp, var_വരി, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    എങ്കിൽ (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (ശൂന്യം)tclexit(err);
}

    സ്ഥിതം ശൂന്യം
tcldelallrefs(ഘടന ref *ref)
{
    ഘടന ref	*next;
    പൂർണ്ണസംഖ്യ		err;
    അക്ഷരം	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    എങ്കിൽ (exiting)
	തിരികെ;
#endif

    ആകുമ്പോൾ (ref != ശൂന്യ)
    {
	next = ref->next;
	എങ്കിൽ (ref->interp)
	{
	    എങ്കിൽ (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		എങ്കിൽ (err != TCL_OK)
		{
		    result = (അക്ഷരം *)Tcl_GetStringResult(ref->interp);
		    എങ്കിൽ (result)
			tclerrmsg(result);
		}
		Tcl_Decr_Ref_എണ്ണം(ref->delcmd);
		ref->delcmd = ശൂന്യ;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((അക്ഷരം *)ref);
	ref = next;
    }
}

    ശൂന്യം
tcl_buffer_free(buf_T *buf)
{
    ഘടന ref *reflist;

#ifdef DYNAMIC_TCL
    എങ്കിൽ (!stubs_initialized)	// Not using Tcl, nothing to do.
	തിരികെ;
#endif

    reflist = (ഘടന ref *)(buf->b_tcl_ref);
    എങ്കിൽ (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (ശൂന്യം *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = ശൂന്യ;
    }
}

    ശൂന്യം
tcl_window_free(win_T *win)
{
    ഘടന ref *reflist;

#ifdef DYNAMIC_TCL
    എങ്കിൽ (!stubs_initialized)	// Not using Tcl, nothing to do.
	തിരികെ;
#endif

    reflist = (ഘടന ref*)(win->w_tcl_ref);
    എങ്കിൽ (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (ശൂന്യം *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = ശൂന്യ;
    }
}

// The End
