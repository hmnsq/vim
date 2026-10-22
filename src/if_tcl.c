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

#ಅನ್ತರ್ಭಾವ "vim.h"
#undef EXTERN			// tcl.h defines it too

#ifdef DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#endif

#ಅನ್ತರ್ಭಾವ <tcl.h>
#ಅನ್ತರ್ಭಾವ <string.h>

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    Tcl_Interp *interp;
    ಪೂರ್ಣಾಙ್ಕ exitvalue;
    ಪೂರ್ಣಾಙ್ಕ range_start, range_end;
    ಪೂರ್ಣಾಙ್ಕ lbase;
    ವರ್ಣ *curbuf, *curwin;
} tcl_info;

ಸ್ಥಿರ tcl_info tclinfo = { ರಿಕ್ತ, 0, 0, 0, 0, ರಿಕ್ತ, ರಿಕ್ತ };

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
ಸಂರಚನಾ ref
{
    ಸಂರಚನಾ ref	*next;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    ಶೂನ್ಯ	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
ಸ್ಥಿರ ವರ್ಣ * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
ಸ್ಥಿರ ವರ್ಣ * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, ಸಂರಚನಾ ref *reflist, ಶೂನ್ಯ *vimobj, Tcl_Obj *delcmd));
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, ಪೂರ್ಣಾಙ್ಕ *ಮೌಲ್ಯ_P, buf_T *buf));
ಸ್ಥಿರ win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, ಪೂರ್ಣಾಙ್ಕ objc, Tcl_Obj *CONST objv[], ಪೂರ್ಣಾಙ್ಕ objn));
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, ಪೂರ್ಣಾಙ್ಕ objc, Tcl_Obj *CONST objv[], ಪೂರ್ಣಾಙ್ಕ objn));
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, ಪೂರ್ಣಾಙ್ಕ objc, Tcl_Obj *CONST objv[], ಪೂರ್ಣಾಙ್ಕ objn));
ಸ್ಥಿರ ಶೂನ್ಯ tcldelthisinterp _ANSI_ARGS_ ((ಶೂನ್ಯ));

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vimerror _ANSI_ARGS_((Tcl_Interp *interp));
ಸ್ಥಿರ ಶೂನ್ಯ tclmsg _ANSI_ARGS_((ವರ್ಣ *text));
ಸ್ಥಿರ ಶೂನ್ಯ tclerrmsg _ANSI_ARGS_((ವರ್ಣ *text));
ಸ್ಥಿರ ಶೂನ್ಯ tclupdatevars _ANSI_ARGS_((ಶೂನ್ಯ));

ಸ್ಥಿರ ಸಂರಚನಾ ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#ಯದಿ defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define ಚಿಹ್ನೆ_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# ಅನ್ಯಥಾ
#  include <dlfcn.h>
#  define HANDLE ಶೂನ್ಯ*
#  define TCL_PROC ಶೂನ್ಯ*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define ಚಿಹ್ನೆ_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
ಸ್ಥಿರ HANDLE hTclLib = ರಿಕ್ತ;
Tcl_Interp* (*dll_Tcl_CreateInterp)(ಶೂನ್ಯ);
ಶೂನ್ಯ (*dll_Tcl_FindExecutable)(ಧ್ರುವ ಶೂನ್ಯ *);

/*
 * Table of name to function pointer of tcl.
 */
ಸ್ಥಿರ ಸಂರಚನಾ {
    ವರ್ಣ* name;
    TCL_PROC* ptr;
} tcl_funcname_table[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_FindExecutable},
    {ರಿಕ್ತ, ರಿಕ್ತ},
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tcl_runtime_link_init(ವರ್ಣ *libname, ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (hTclLib)
	ಪ್ರತ್ಯಾಗಮ OK;
    ಯದಿ (!(hTclLib = load_dll(libname)))
    {
	ಯದಿ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತಿ (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	ಯದಿ (!(*tcl_funcname_table[i].ptr = ಚಿಹ್ನೆ_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = ರಿಕ್ತ;
	    ಯದಿ (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
ಸ್ಥಿರ ವರ್ಣ *find_executable_arg = ರಿಕ್ತ;
#endif

    ಶೂನ್ಯ
vim_tcl_init(ವರ್ಣ *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#ಅನ್ಯಥಾ
    Tcl_FindExecutable(arg);
#endif
}

#ಯದಿ defined(DYNAMIC_TCL)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    ಪೂರ್ಣಾಙ್ಕ
tcl_enabled(ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಯದಿ (!stubs_initialized && find_executable_arg != ರಿಕ್ತ
	    && tcl_runtime_link_init((ವರ್ಣ *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	ಯದಿ ((interp = dll_Tcl_CreateInterp()) != ರಿಕ್ತ)
	{
	    ಯದಿ (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != ರಿಕ್ತ)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    ಪ್ರತ್ಯಾಗಮ stubs_initialized;
}
#endif

#ಯದಿ defined(EXITFREE)
/*
 * Called once when exiting.
 */
    ಶೂನ್ಯ
vim_tcl_finalize(ಶೂನ್ಯ)
{
# ifdef DYNAMIC_TCL
    ಯದಿ (stubs_initialized)
# endif
	Tcl_Finalize();
}
#endif

    ಶೂನ್ಯ
tcl_end(ಶೂನ್ಯ)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    ಪೂರ್ಣಾಙ್ಕ objc,
    Tcl_Obj *CONST objv[])
{
    ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ = 0;

    ವಿಕಲ್ಪ (objc)
    {
	ಪಕ್ಷ 2:
	    ಯದಿ (Tcl_GetIntFromObj(interp, objv[1], &ಮೌಲ್ಯ) != TCL_OK)
		ಭಙ್ಗ;
	    // FALLTHROUGH
	ಪಕ್ಷ 1:
	    tclinfo.exitvalue = ಮೌಲ್ಯ;

	    Tcl_DeleteInterp(interp);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    Tcl_Wrong_ಅಂಕೆ_Args(interp, 1, objv, "?returnCode?");
    }
    ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    ಪೂರ್ಣಾಙ್ಕ objc,
    Tcl_Obj *CONST objv[])
{
    ಯದಿ (objc != 1)
    {
	Tcl_Wrong_ಅಂಕೆ_Args(interp, 1, objv, ರಿಕ್ತ);
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }
    vim_beep(BO_LANG);
    ಪ್ರತ್ಯಾಗಮ TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    ಪೂರ್ಣಾಙ್ಕ objc,
    Tcl_Obj *CONST objv[])
{
    ವರ್ಣ	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    ಪೂರ್ಣಾಙ್ಕ		err, n, idx;
    ಗಣನಾ {BCMD_EXISTS, BCMD_LIST};
    ಸ್ಥಿರ CONST84 ವರ್ಣ *bcmdoptions[] =
    {
	"exists", "list", (ವರ್ಣ *)0
    };

    ಯದಿ (objc < 2)
    {
	Tcl_Wrong_ಅಂಕೆ_Args(interp, 1, objv, "option");
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    ಯದಿ (err == TCL_OK)
    {
	ಯದಿ (objc != 2)
	{
	    Tcl_Wrong_ಅಂಕೆ_Args(interp, 1, objv, "bufNumber");
	    ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    ಯದಿ (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		ಯದಿ (name == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		ಪ್ರತ್ಯಾಗಮ TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    ಯದಿ (err != TCL_OK)
	ಪ್ರತ್ಯಾಗಮ err;
    ವಿಕಲ್ಪ (idx)
    {
	ಪಕ್ಷ BCMD_LIST:
	    ಯದಿ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, "");
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		ಯದಿ (name == ರಿಕ್ತ)
		{
		    err = TCL_ERROR;
		    ಭಙ್ಗ;
		}
		Tcl_AppendElement(interp, name);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ BCMD_EXISTS:
	    ಯದಿ (objc != 3)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    ಯದಿ (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != ರಿಕ್ತ);
		Tcl_SetObjResult(interp, resobj);
	    }
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಙ್ಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ವರ್ಣ	*what, *string;
    win_T	*win;

    ಯದಿ (objc != 2)
    {
	Tcl_Wrong_ಅಂಕೆ_Args(interp, 1, objv, "option");
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], ರಿಕ್ತ);
    ಯದಿ (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    ಯದಿ (string == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	ಪ್ರತ್ಯಾಗಮ TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಙ್ಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ಪೂರ್ಣಾಙ್ಕ		opt, err, idx, flags;
    ಪೂರ್ಣಾಙ್ಕ		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    ವರ್ಣ	*ಸಾಲು;

    ಗಣನಾ
    {
	BUF_APPEND, BUF_COMMAND, BUF_ಎಣಿಕೆ, BUF_DELCMD, BUF_DELETE, BUF_ಅಭಿವ್ಯಕ್ತಿ,
	BUF_GET, BUF_ಸೇರಿಸು, BUF_LAST, BUF_MARK, BUF_NAME, BUF_ಸಂಖ್ಯೆ,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    ಸ್ಥಿರ CONST84 ವರ್ಣ *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (ವರ್ಣ *)0
    };

    ಯದಿ (objc < 2)
    {
	Tcl_Wrong_ಅಂಕೆ_Args(interp, 1, objv, "option ?arg ...?");
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    ಯದಿ (err != TCL_OK)
	ಪ್ರತ್ಯಾಗಮ err;

    buf = (buf_T *)((ಸಂರಚನಾ ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    ವಿಕಲ್ಪ (idx)
    {
	ಪಕ್ಷ BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_ಅಭಿವ್ಯಕ್ತಿ:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    ಯದಿ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, ರಿಕ್ತ);
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (buf->b_ffname)
		Tcl_SetResult(interp, (ವರ್ಣ *)buf->b_ffname, TCL_VOLATILE);
	    ಅನ್ಯಥಾ
		Tcl_SetResult(interp, "", TCL_STATIC);
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	ಪಕ್ಷ BUF_ಎಣಿಕೆ:
	    /*
	     * Get number of lines in buffer.
	     */
	    ಯದಿ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, ರಿಕ್ತ);
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    val1 = (ಪೂರ್ಣಾಙ್ಕ)buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಯದಿ (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_ಸಂಖ್ಯೆ:
	    /*
	     * Get buffer's number.
	     */
	    ಯದಿ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, ರಿಕ್ತ);
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    resobj = Tcl_NewIntObj((ಪೂರ್ಣಾಙ್ಕ)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_GET:
	    ಯದಿ (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ಯದಿ (err != TCL_OK)
		ಭಙ್ಗ;
	    ಯದಿ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ಯದಿ (err != TCL_OK)
		    ಭಙ್ಗ;
		ಯದಿ (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		ಪ್ರತಿ (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    ಸಾಲು = (ವರ್ಣ *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    ಯದಿ (ಸಾಲು)
			Tcl_AppendElement(interp, ಸಾಲು);
		    ಅನ್ಯಥಾ
			err = TCL_ERROR;
		}
	    }
	    ಅನ್ಯಥಾ
	    {  // objc == 3
		ಸಾಲು = (ವರ್ಣ *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, ಸಾಲು, TCL_VOLATILE);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_SET:
	    ಯದಿ (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ಯದಿ (err != TCL_OK)
		ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
	    ಯದಿ (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		ಸಾಲು = Tcl_GetStringFromObj(objv[3], ರಿಕ್ತ);
		ಯದಿ (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ಅನ್ಯಥಾ
		ಯದಿ (ml_replace((linenr_T)val1, (char_u *)ಸಾಲು, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ಅನ್ಯಥಾ
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		ಪೂರ್ಣಾಙ್ಕ	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ಯದಿ (err != TCL_OK)
		    ಭಙ್ಗ;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		ಯದಿ (err != TCL_OK)
		    ಭಙ್ಗ;
		ಯದಿ (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		ಯದಿ (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    ಭಙ್ಗ;
		}
		flags |= FL_UPDATE_CURBUF;

		ಪ್ರತಿ (i = 0; i < lc && n <= val2; i++)
		{
		    ಸಾಲು = Tcl_GetStringFromObj(lv[i], ರಿಕ್ತ);
		    ಯದಿ (ml_replace((linenr_T)n, (char_u *)ಸಾಲು, TRUE) != OK)
			ಗಚ್ಛ setListError;
		    ++n;
		}
		ಯದಿ (i < lc)
		{
		    // append lines
		    ಕುರು
		    {
			ಸಾಲು = Tcl_GetStringFromObj(lv[i], ರಿಕ್ತ);
			ಯದಿ (ml_append((linenr_T)(n - 1),
					      (char_u *)ಸಾಲು, 0, FALSE) != OK)
			    ಗಚ್ಛ setListError;
			++n;
			++i;
		    } ಯಾವತ್ (i < lc);
		}
		ಅನ್ಯಥಾ ಯದಿ (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    ಕುರು
		    {
			ಯದಿ (ml_delete((linenr_T)i) != OK)
			    ಗಚ್ಛ setListError;
			++n;
		    } ಯಾವತ್ (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (long)MAXLNUM,
								    (long)lc);
		changed_ಸಾಲುಗಳು((linenr_T)val1, 0, (linenr_T)val2 + 1, (long)lc);
		ಭಙ್ಗ;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_DELETE:
	    ಯದಿ (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ಯದಿ (err != TCL_OK)
		ಭಙ್ಗ;
	    val2 = val1;
	    ಯದಿ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ಯದಿ (err != TCL_OK)
		    ಪ್ರತ್ಯಾಗಮ err;
		ಯದಿ (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    ಯದಿ (u_savedel((linenr_T)val1, (long)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    ಪ್ರತಿ (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		ಯದಿ (err != TCL_OK)
		    ಭಙ್ಗ;
	    }
	    ಯದಿ (i > 0)
		deleted_ಸಾಲುಗಳು_mark((linenr_T)val1, (long)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_MARK:
	    ಯದಿ (objc != 3)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, "markName");
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    ಸಾಲು = Tcl_GetStringFromObj(objv[2], ರಿಕ್ತ);

	    pos = ರಿಕ್ತ;
	    ಯದಿ (ಸಾಲು[0] != '\0'  &&  ಸಾಲು[1] == '\0')
		pos = getmark(ಸಾಲು[0], FALSE);
	    ಯದಿ (pos == ರಿಕ್ತ)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    err = vimerror(interp);
	    ಯದಿ (err != TCL_OK)
		ಭಙ್ಗ;
	    ಯದಿ (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ಅನ್ಯಥಾ
	    {
		ವರ್ಣ rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (ಪೂರ್ಣಾಙ್ಕ)row2tcl(pos->lnum), (ಪೂರ್ಣಾಙ್ಕ)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_ಸೇರಿಸು:
	    opt = 1;
	    // fallthrough
	ಪಕ್ಷ BUF_APPEND:
	    ಯದಿ (objc != 4)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ಯದಿ (err != TCL_OK)
		ಭಙ್ಗ;
	    ಯದಿ (opt)
		--val1;
	    ಯದಿ (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }

	    ಸಾಲು = Tcl_GetStringFromObj(objv[3], ರಿಕ್ತ);
	    ಯದಿ (ml_append((linenr_T)val1, (char_u *)ಸಾಲು, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    appended_ಸಾಲುಗಳು_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    ಯದಿ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, ರಿಕ್ತ);
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		ಯದಿ (win->w_buffer == buf)
		{
		    ಸಾಲು = tclgetwindow(interp, win);
		    ಯದಿ (ಸಾಲು != ರಿಕ್ತ)
			Tcl_AppendElement(interp, ಸಾಲು);
		    ಅನ್ಯಥಾ
		    {
			err = TCL_ERROR;
			ಭಙ್ಗ;
		    }
		}
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    ಯದಿ (objc != 3)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (ಶೂನ್ಯ *)buf, objv[2]);
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    ಯದಿ (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    ಯದಿ (flags & FL_ADJUST_CURSOR)
	check_cursor();
    ಯದಿ (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    ಪ್ರತ್ಯಾಗಮ err;
}

/*
 * This function implements the window commands.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಙ್ಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ಪೂರ್ಣಾಙ್ಕ		err, idx, flags;
    ಪೂರ್ಣಾಙ್ಕ		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    ವರ್ಣ	*str;

    ಗಣನಾ
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_ಅಭಿವ್ಯಕ್ತಿ,
	WIN_HEIGHT, WIN_OPTION
    };
    ಸ್ಥಿರ CONST84 ವರ್ಣ *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (ವರ್ಣ *)0
    };

    ಯದಿ (objc < 2)
    {
	Tcl_Wrong_ಅಂಕೆ_Args(interp, 1, objv, "option ?arg ...?");
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    ಯದಿ (err != TCL_OK)
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;

    win = (win_T *)((ಸಂರಚನಾ ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    ವಿಕಲ್ಪ (idx)
    {
	ಪಕ್ಷ WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ಭಙ್ಗ;

	ಪಕ್ಷ WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ಭಙ್ಗ;

	ಪಕ್ಷ WIN_ಅಭಿವ್ಯಕ್ತಿ:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ಭಙ್ಗ;

	ಪಕ್ಷ WIN_HEIGHT:
	    ಯದಿ (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		ಯದಿ (err != TCL_OK)
		    ಭಙ್ಗ;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		ಯದಿ (err != TCL_OK)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ
	    ಯದಿ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }

	    resobj = Tcl_NewIntObj((ಪೂರ್ಣಾಙ್ಕ)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    ಭಙ್ಗ;

	ಪಕ್ಷ WIN_BUFFER:
	    ಯದಿ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, ರಿಕ್ತ);
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    ಯದಿ (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    ಅನ್ಯಥಾ
		err = TCL_ERROR;
	    ಭಙ್ಗ;

	ಪಕ್ಷ WIN_DELCMD:
	    ಯದಿ (objc != 3)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (ಶೂನ್ಯ *)win, objv[2]);
	    ಭಙ್ಗ;

	ಪಕ್ಷ WIN_CURSOR:
	    ಯದಿ (objc > 4)
	    {
		Tcl_Wrong_ಅಂಕೆ_Args(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (objc == 2)
	    {
		ವರ್ಣ buf[64];

		sprintf(buf, _("row %d column %d"), (ಪೂರ್ಣಾಙ್ಕ)row2tcl(win->w_cursor.lnum), (ಪೂರ್ಣಾಙ್ಕ)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_ಸಂದೇಶ);
		ಯದಿ (var == ರಿಕ್ತ)
		{
		    err = TCL_ERROR;
		    ಭಙ್ಗ;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		ಯದಿ (err != TCL_OK)
		    ಭಙ್ಗ;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_ಸಂದೇಶ);
		ಯದಿ (var == ರಿಕ್ತ)
		{
		    err = TCL_ERROR;
		    ಭಙ್ಗ;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		ಯದಿ (err != TCL_OK)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		ಯದಿ (err != TCL_OK)
		    ಭಙ್ಗ;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		ಯದಿ (err != TCL_OK)
		    ಭಙ್ಗ;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    ಭಙ್ಗ;
    }

    curwin = savewin;
    curbuf = savebuf;
    ಯದಿ (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    ಪ್ರತ್ಯಾಗಮ err;
}


    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಙ್ಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ಪೂರ್ಣಾಙ್ಕ		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ಪ್ರತ್ಯಾಗಮ err;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಙ್ಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ಪೂರ್ಣಾಙ್ಕ		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ಪ್ರತ್ಯಾಗಮ err;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಙ್ಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ಪ್ರತ್ಯಾಗಮ tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    ಪೂರ್ಣಾಙ್ಕ		*ಮೌಲ್ಯ_P,
    buf_T	*buf)
{
    ಪೂರ್ಣಾಙ್ಕ err, i;

    ಗಣನಾ { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_ತುದಿ };

    ಸ್ಥಿರ CONST84 ವರ್ಣ *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (ವರ್ಣ *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    ಯದಿ (err == TCL_OK)
    {
	ವಿಕಲ್ಪ (i)
	{
	    ಪಕ್ಷ LN_BEGIN:
	    ಪಕ್ಷ LN_FIRST:
	    ಪಕ್ಷ LN_START:
	    ಪಕ್ಷ LN_ತುದಿ:
		*ಮೌಲ್ಯ_P = 1;
		ಭಙ್ಗ;
	    ಪಕ್ಷ LN_BOTTOM:
	    ಪಕ್ಷ LN_END:
	    ಪಕ್ಷ LN_LAST:
		*ಮೌಲ್ಯ_P = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		ಭಙ್ಗ;
	}
	ಪ್ರತ್ಯಾಗಮ TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    ಯದಿ (err != TCL_OK)
	ಪ್ರತ್ಯಾಗಮ err;
    i = row2vim(i);
    ಯದಿ (i < 1  ||  i > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }
    *ಮೌಲ್ಯ_P = i;
    ಪ್ರತ್ಯಾಗಮ TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    ಸ್ಥಿರ win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	ಯದಿ (win->w_buffer == buf)
	    ಪ್ರತ್ಯಾಗಮ win;
    }
    ಪ್ರತ್ಯಾಗಮ curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tcldoexcommand(
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಙ್ಕ		objc,
    Tcl_Obj	*CONST objv[],
    ಪೂರ್ಣಾಙ್ಕ		objn)
{
    tcl_info	saveinfo;
    ಪೂರ್ಣಾಙ್ಕ		err, flag, nobjs;
    ವರ್ಣ	*arg;

    nobjs = objc - objn;
    ಯದಿ (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_ಅಂಕೆ_Args(interp, objn, objv, "?-quiet? exCommand");
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }

    flag = 0;
    ಯದಿ (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], ರಿಕ್ತ);
	ಯದಿ (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	ಅನ್ಯಥಾ
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (ವರ್ಣ *)0);
	    ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
	}
	++objn;
    }

    memcpy(&saveinfo, &tclinfo, ಪರಿಮಾಣ(tcl_info));
    tclinfo.interp = ರಿಕ್ತ;
    tclinfo.curwin = ರಿಕ್ತ;
    tclinfo.curbuf = ರಿಕ್ತ;

    arg = Tcl_GetStringFromObj(objv[objn], ರಿಕ್ತ);
    ಯದಿ (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    ಯದಿ (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    ಯದಿ (tclinfo.interp)
	tcldelthisinterp();
    memcpy(&tclinfo, &saveinfo, ಪರಿಮಾಣ(tcl_info));
    tclupdatevars();

    ಪ್ರತ್ಯಾಗಮ err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tclsetoption(
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಙ್ಕ		objc,
    Tcl_Obj	*CONST objv[],
    ಪೂರ್ಣಾಙ್ಕ		objn)
{
    ಪೂರ್ಣಾಙ್ಕ		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    long	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    ಗಣನಾ { OPT_OFF, OPT_ON, OPT_TOGGLE };
    ಸ್ಥಿರ CONST84 ವರ್ಣ *optkw[] = { "off", "on", "toggle", (ವರ್ಣ *)0 };

    nobjs = objc - objn;
    ಯದಿ (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_ಅಂಕೆ_Args(interp, objn, objv, "vimOption ?value?");
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], ರಿಕ್ತ);
    ++objn;
    gov = get_option_ಮೌಲ್ಯ(option, &lval, &sval, ರಿಕ್ತ, 0);
    err = TCL_OK;
    ವಿಕಲ್ಪ (gov)
    {
	ಪಕ್ಷ gov_string:
	    Tcl_SetResult(interp, (ವರ್ಣ *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    ಭಙ್ಗ;
	ಪಕ್ಷ gov_bool:
	ಪಕ್ಷ gov_ಸಂಖ್ಯೆ:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }
    ಯದಿ (nobjs == 2)
    {
	ಯದಿ (gov != gov_string)
	{
	    sval = ರಿಕ್ತ;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    ಯದಿ (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    ಅನ್ಯಥಾ
	    {
		ವಿಕಲ್ಪ (idx)
		{
		    ಪಕ್ಷ OPT_ON:
			lval = 1;
			ಭಙ್ಗ;
		    ಪಕ್ಷ OPT_OFF:
			lval = 0;
			ಭಙ್ಗ;
		    ಪಕ್ಷ OPT_TOGGLE:
			lval = !lval;
			ಭಙ್ಗ;
		}
	    }
	}
	ಅನ್ಯಥಾ
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], ರಿಕ್ತ);
	ಯದಿ (err == TCL_OK)
	{
	    set_option_ಮೌಲ್ಯ_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    ಪ್ರತ್ಯಾಗಮ err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tclvimexpr(
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಙ್ಕ		objc,
    Tcl_Obj	*CONST objv[],
    ಪೂರ್ಣಾಙ್ಕ		objn)
{
#ifdef FEAT_EVAL
    ವರ್ಣ	*ಅಭಿವ್ಯಕ್ತಿ, *str;
#endif
    ಪೂರ್ಣಾಙ್ಕ		err;

    ಯದಿ (objc - objn != 1)
    {
	Tcl_Wrong_ಅಂಕೆ_Args(interp, objn, objv, "vimExpr");
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }

#ifdef FEAT_EVAL
    ಅಭಿವ್ಯಕ್ತಿ = Tcl_GetStringFromObj(objv[objn], ರಿಕ್ತ);
    str = (ವರ್ಣ *)eval_to_string((char_u *)ಅಭಿವ್ಯಕ್ತಿ, TRUE, FALSE);
    ಯದಿ (str == ರಿಕ್ತ)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    ಅನ್ಯಥಾ
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#ಅನ್ಯಥಾ
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    ಪ್ರತ್ಯಾಗಮ err;
}

/*
 * Check for internal vim errors.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vimerror(Tcl_Interp *interp)
{
    ಯದಿ (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }
    ಅನ್ಯಥಾ ಯದಿ (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
delref(ClientData cref)
{
    ಸಂರಚನಾ ref *ref = (ಸಂರಚನಾ ref *)cref;

    ಯದಿ (ref->delcmd)
    {
	Tcl_Decr_Ref_ಎಣಿಕೆ(ref->delcmd);
	ref->delcmd = ರಿಕ್ತ;
    }
    ref->interp = ರಿಕ್ತ;
}

    ಸ್ಥಿರ ವರ್ಣ *
tclgetref(
    Tcl_Interp	*interp,
    ಶೂನ್ಯ	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    ವರ್ಣ	*prefix,	// "win" or "buf"
    ಶೂನ್ಯ	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    ಸಂರಚನಾ ref *ref, *unused = ರಿಕ್ತ;
    ಸ್ಥಿರ ವರ್ಣ name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (ಸಂರಚನಾ ref *)(*refstartP);
    ಯದಿ (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯಾವತ್ (ref != ರಿಕ್ತ)
    {
	ಯದಿ (ref->interp == interp)
	    ಭಙ್ಗ;
	ಯದಿ (ref->interp == ರಿಕ್ತ)
	    unused = ref;
	ref = ref->next;
    }

    ಯದಿ (ref)
	vim_snprintf(name, ಪರಿಮಾಣ(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    ಅನ್ಯಥಾ
    {
	ಯದಿ (unused)
	    ref = unused;
	ಅನ್ಯಥಾ
	{
	    ref = (ಸಂರಚನಾ ref *)Tcl_Alloc(ಪರಿಮಾಣ(ಸಂರಚನಾ ref));
	    ref->interp = ರಿಕ್ತ;
	    ref->next = (ಸಂರಚನಾ ref *)(*refstartP);
	    (*refstartP) = (ಶೂನ್ಯ *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, ಪರಿಮಾಣ(name), "::vim::%s_%lx",
					       prefix, (unsigned long)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	ಯದಿ (!cmd)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = ರಿಕ್ತ;
	ref->vimobj = vimobj;
    }
    ಪ್ರತ್ಯಾಗಮ name;
}

    ಸ್ಥಿರ ವರ್ಣ *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    ಪ್ರತ್ಯಾಗಮ tclgetref(interp, &(win->w_tcl_ref), "win", (ಶೂನ್ಯ *)win, winselfcmd);
}

    ಸ್ಥಿರ ವರ್ಣ *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ tclgetref(interp, &(buf->b_tcl_ref), "buf", (ಶೂನ್ಯ *)buf, bufselfcmd);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tclsetdelcmd(
    Tcl_Interp	*interp,
    ಸಂರಚನಾ ref	*reflist,
    ಶೂನ್ಯ	*vimobj,
    Tcl_Obj	*delcmd)
{
    ಯದಿ (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
    }

    ಯಾವತ್ (reflist != ರಿಕ್ತ)
    {
	ಯದಿ (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    ಯದಿ (reflist->delcmd)
		Tcl_Decr_Ref_ಎಣಿಕೆ(reflist->delcmd);
	    Tcl_Incr_Ref_ಎಣಿಕೆ(delcmd);
	    reflist->delcmd = delcmd;
	    ಪ್ರತ್ಯಾಗಮ TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		err = 0;

    // currently does nothing

    ಯದಿ (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    ಪ್ರತ್ಯಾಗಮ err;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tcl_channel_ಒಳಹರಿವು(
    ClientData	instance UNUSED,
    ವರ್ಣ	*buf UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		bufsiz UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    ಯದಿ (errptr)
	*errptr = EINVAL;
    ಪ್ರತ್ಯಾಗಮ -1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tcl_channel_output(
    ClientData	instance,
    ಧ್ರುವ ವರ್ಣ	*buf,
    ಪೂರ್ಣಾಙ್ಕ		bufsiz,
    ಪೂರ್ಣಾಙ್ಕ		*errptr)
{
    char_u	*str;
    ಪೂರ್ಣಾಙ್ಕ		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    ಯದಿ (!str)
    {
	Tcl_SetErrno(ENOMEM);
	ಯದಿ (errptr)
	    *errptr = ENOMEM;
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    result = bufsiz;
    ಯದಿ (instance == VIMOUT)
	tclmsg((ವರ್ಣ *)str);
    ಅನ್ಯಥಾ
    ಯದಿ (instance == VIMERR)
	tclerrmsg((ವರ್ಣ *)str);
    ಅನ್ಯಥಾ
    {
	Tcl_SetErrno(EBADF);
	ಯದಿ (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    ಪ್ರತ್ಯಾಗಮ result;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tcl_channel_watch(ClientData instance UNUSED, ಪೂರ್ಣಾಙ್ಕ mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    ಪ್ರತ್ಯಾಗಮ EINVAL;
}


ಸ್ಥಿರ Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_VERSION_2, // version
    tcl_channel_close,	// closeProc
    tcl_channel_ಒಳಹರಿವು,	// inputProc
    tcl_channel_output,	// outputProc
    ರಿಕ್ತ,		// seekProc
    ರಿಕ್ತ,		// setOptionProc
    ರಿಕ್ತ,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    ರಿಕ್ತ,		// close2Proc
    ರಿಕ್ತ,		// blockModeProc
#ifdef TCL_CHANNEL_VERSION_2
    ರಿಕ್ತ,		// flushProc
    ರಿಕ್ತ,		// handlerProc
#endif
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ifdef TCL_CHANNEL_VERSION_3
    ರಿಕ್ತ,		// wideSeekProc
#endif
#ifdef TCL_CHANNEL_VERSION_4
    ರಿಕ್ತ,		// threadActionProc
#endif
#ifdef TCL_CHANNEL_VERSION_5
    ರಿಕ್ತ		// truncateProc
#endif
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    ಸ್ಥಿರ ಶೂನ್ಯ
tclupdatevars(ಶೂನ್ಯ)
{
    ವರ್ಣ varname[VARNAME_SIZE];	// must be writeable
    ವರ್ಣ *name;

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


    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tclinit(exarg_T *eap)
{
    ವರ್ಣ varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    ವರ್ಣ *name;

#ifdef DYNAMIC_TCL
    ಯದಿ (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
#endif

    ಯದಿ (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	ಸ್ಥಿರ Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_SetStdChannel(ch1, TCL_STDOUT);
	Tcl_SetStdChannel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	ಯದಿ (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
#ಯದಿ 0
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
	    (ClientData)ರಿಕ್ತ, (Tcl_CmdDeleteProc *)ರಿಕ್ತ);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (ClientData)ರಿಕ್ತ, (Tcl_CmdDeleteProc *)ರಿಕ್ತ);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (ClientData)ರಿಕ್ತ, (Tcl_CmdDeleteProc *)ರಿಕ್ತ);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (ClientData)ರಿಕ್ತ, (Tcl_CmdDeleteProc *)ರಿಕ್ತ);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (ClientData)ರಿಕ್ತ, (Tcl_CmdDeleteProc *)ರಿಕ್ತ);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (ClientData)ರಿಕ್ತ, (Tcl_CmdDeleteProc *)ರಿಕ್ತ);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (ClientData)ರಿಕ್ತ, (Tcl_CmdDeleteProc *)ರಿಕ್ತ);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (ವರ್ಣ *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (ವರ್ಣ *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (ವರ್ಣ *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (ವರ್ಣ *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (ವರ್ಣ *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (ವರ್ಣ *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    ಅನ್ಯಥಾ
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tclerrmsg(ವರ್ಣ *text)
{
    ವರ್ಣ *next;

    ಯಾವತ್ ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    ಯದಿ (*text)
	emsg(text);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tclmsg(ವರ್ಣ *text)
{
    ವರ್ಣ *next;

    ಯಾವತ್ ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	ಸಂದೇಶ(text);
	text = next;
    }
    ಯದಿ (*text)
	ಸಂದೇಶ(text);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tcldelthisinterp(ಶೂನ್ಯ)
{
    ಯದಿ (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = ರಿಕ್ತ;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tclexit(ಪೂರ್ಣಾಙ್ಕ error)
{
    ಪೂರ್ಣಾಙ್ಕ newerr = OK;

    ಯದಿ (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#ಯದಿ (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	ವರ್ಣ buf[50];

	sprintf(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	ಯದಿ (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	ಅನ್ಯಥಾ
	    newerr = FAIL;

	tcldelthisinterp();
    }
    ಅನ್ಯಥಾ
    {
	ವರ್ಣ *result;

	result = (ವರ್ಣ *)Tcl_GetStringResult(tclinfo.interp);
	ಯದಿ (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	ಅನ್ಯಥಾ
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    ಪ್ರತ್ಯಾಗಮ newerr;
}

/*
 * ":tcl"
 */
    ಶೂನ್ಯ
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    ಪೂರ್ಣಾಙ್ಕ		err;

    script = script_get(eap, eap->arg);
    ಯದಿ (!eap->skip)
    {
	err = tclinit(eap);
	ಯದಿ (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    ಯದಿ (script == ರಿಕ್ತ)
		err = Tcl_Eval(tclinfo.interp, (ವರ್ಣ *)eap->arg);
	    ಅನ್ಯಥಾ
		err = Tcl_Eval(tclinfo.interp, (ವರ್ಣ *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    ಶೂನ್ಯ
ex_tclfile(exarg_T *eap)
{
    ವರ್ಣ *file = (ವರ್ಣ *)eap->arg;
    ಪೂರ್ಣಾಙ್ಕ err;

    err = tclinit(eap);
    ಯದಿ (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    ಶೂನ್ಯ
ex_tcldo(exarg_T *eap)
{
    ವರ್ಣ	*script, *ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		err, rs, re, lnum;
    ವರ್ಣ	var_lnum[VARNAME_SIZE]; // must be writeable memory
    ವರ್ಣ	var_ಸಾಲು[VARNAME_SIZE];
    linenr_T	first_ಸಾಲು = 0;
    linenr_T	last_ಸಾಲು = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (ವರ್ಣ *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_ಸಾಲು, VAR_CURLINE);

    err = tclinit(eap);
    ಯದಿ (err != OK)
	ಪ್ರತ್ಯಾಗಮ;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (ವರ್ಣ *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    ಯದಿ (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    ಯಾವತ್ (err == TCL_OK  &&  rs <= re)
    {
	ಯದಿ ((linenr_T)rs > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಭಙ್ಗ;
	ಸಾಲು = (ವರ್ಣ *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	ಯದಿ (!ಸಾಲು)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    ಭಙ್ಗ;
	}
	Tcl_SetVar(tclinfo.interp, var_ಸಾಲು, ಸಾಲು, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	ಯದಿ (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#ಯದಿ (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಭಙ್ಗ;
	ಸಾಲು = (ವರ್ಣ *)Tcl_GetVar(tclinfo.interp, var_ಸಾಲು, 0);
	ಯದಿ (ಸಾಲು)
	{
	    ಯದಿ (ml_replace((linenr_T)rs, (char_u *)ಸಾಲು, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (first_ಸಾಲು == 0)
		first_ಸಾಲು = rs;
	    last_ಸಾಲು = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    ಯದಿ (first_ಸಾಲು)
	changed_ಸಾಲುಗಳು(first_ಸಾಲು, 0, last_ಸಾಲು + 1, (long)0);

    Tcl_UnsetVar(tclinfo.interp, var_ಸಾಲು, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    ಯದಿ (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (ಶೂನ್ಯ)tclexit(err);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tcldelallrefs(ಸಂರಚನಾ ref *ref)
{
    ಸಂರಚನಾ ref	*next;
    ಪೂರ್ಣಾಙ್ಕ		err;
    ವರ್ಣ	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    ಯದಿ (exiting)
	ಪ್ರತ್ಯಾಗಮ;
#endif

    ಯಾವತ್ (ref != ರಿಕ್ತ)
    {
	next = ref->next;
	ಯದಿ (ref->interp)
	{
	    ಯದಿ (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		ಯದಿ (err != TCL_OK)
		{
		    result = (ವರ್ಣ *)Tcl_GetStringResult(ref->interp);
		    ಯದಿ (result)
			tclerrmsg(result);
		}
		Tcl_Decr_Ref_ಎಣಿಕೆ(ref->delcmd);
		ref->delcmd = ರಿಕ್ತ;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((ವರ್ಣ *)ref);
	ref = next;
    }
}

    ಶೂನ್ಯ
tcl_buffer_free(buf_T *buf)
{
    ಸಂರಚನಾ ref *reflist;

#ifdef DYNAMIC_TCL
    ಯದಿ (!stubs_initialized)	// Not using Tcl, nothing to do.
	ಪ್ರತ್ಯಾಗಮ;
#endif

    reflist = (ಸಂರಚನಾ ref *)(buf->b_tcl_ref);
    ಯದಿ (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (ಶೂನ್ಯ *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = ರಿಕ್ತ;
    }
}

    ಶೂನ್ಯ
tcl_window_free(win_T *win)
{
    ಸಂರಚನಾ ref *reflist;

#ifdef DYNAMIC_TCL
    ಯದಿ (!stubs_initialized)	// Not using Tcl, nothing to do.
	ಪ್ರತ್ಯಾಗಮ;
#endif

    reflist = (ಸಂರಚನಾ ref*)(win->w_tcl_ref);
    ಯದಿ (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (ಶೂನ್ಯ *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = ರಿಕ್ತ;
    }
}

// The End
