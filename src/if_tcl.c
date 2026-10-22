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
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು EXTERN			// tcl.h defines it too

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <tcl.h>
#ಅನ್ತರ್ಭಾವ <string.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Tcl_Interp *interp;
    ಪೂರ್ಣಾಂಕ exitvalue;
    ಪೂರ್ಣಾಂಕ range_ಆರಂಭ, range_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ lbase;
    ಅಕ್ಷರ *curbuf, *curwin;
} tcl_info;

ಸ್ಥಿರ tcl_info tclinfo = { ಖಾಲಿ, 0, 0, 0, 0, ಖಾಲಿ, ಖಾಲಿ };

#ವ್ಯಾಖ್ಯಾನಿಸು VAR_RANGE1	"::vim::range(start)"
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_RANGE2	"::vim::range(begin)"
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_RANGE3	"::vim::range(end)"
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_CURBUF	"::vim::current(buffer)"
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_CURWIN	"::vim::current(window)"
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_LBASE	"::vim::lbase"
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_CURLINE	"line"
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_CURLNUM	"lnum"
#ವ್ಯಾಖ್ಯಾನಿಸು VARNAME_ಗಾತ್ರ	64

#ವ್ಯಾಖ್ಯಾನಿಸು row2tcl(x)  ((x) - (tclinfo.lbase==0))
#ವ್ಯಾಖ್ಯಾನಿಸು row2vim(x)  ((x) + (tclinfo.lbase==0))
#ವ್ಯಾಖ್ಯಾನಿಸು col2tcl(x)  ((x) + (tclinfo.lbase!=0))
#ವ್ಯಾಖ್ಯಾನಿಸು col2vim(x)  ((x) - (tclinfo.lbase!=0))


#ವ್ಯಾಖ್ಯಾನಿಸು VIMOUT	((Client_ದತ್ತಾಂಶ)1)
#ವ್ಯಾಖ್ಯಾನಿಸು VIMERR	((Client_ದತ್ತಾಂಶ)2)

// This appears to be new in Tcl 8.4.
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONST84
# define CONST84
#ಆದರೆ_ಅಂತ್ಯ

/*
 *  List of Tcl interpreters who reference a vim window or buffer.
 *  Each buffer and window has its own list in the w_tcl_ref or b_tcl_ref
 *  struct member.  We need this because Tcl can create sub-interpreters with
 *  the "interp" command, and each interpreter can reference all windows and
 *  buffers.
 */
ರಚನೆ ref
{
    ರಚನೆ ref	*ಮುಂದಿನ;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    ಶೂನ್ಯ	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
ಸ್ಥಿರ ಅಕ್ಷರ * tclgetbuffer ANSI_ವಾದಗಳು((Tcl_Interp *interp, buf_T *buf));
ಸ್ಥಿರ ಅಕ್ಷರ * tclgetwindow ANSI_ವಾದಗಳು((Tcl_Interp *interp, win_T *win));
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tclsetdelcmd ANSI_ವಾದಗಳು((Tcl_Interp *interp, ರಚನೆ ref *reflist, ಶೂನ್ಯ *vimobj, Tcl_Obj *delcmd));
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tclgetlinenum ANSI_ವಾದಗಳು ((Tcl_Interp *interp, Tcl_Obj *obj, ಪೂರ್ಣಾಂಕ *ಮೌಲ್ಯ_P, buf_T *buf));
ಸ್ಥಿರ win_T *tclfindwin ANSI_ವಾದಗಳು ((buf_T *buf));
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tcldoexcommand ANSI_ವಾದಗಳು ((Tcl_Interp *interp, ಪೂರ್ಣಾಂಕ objc, Tcl_Obj *CONST objv[], ಪೂರ್ಣಾಂಕ objn));
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tclsetoption ANSI_ವಾದಗಳು ((Tcl_Interp *interp, ಪೂರ್ಣಾಂಕ objc, Tcl_Obj *CONST objv[], ಪೂರ್ಣಾಂಕ objn));
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tclvimexpr ANSI_ವಾದಗಳು ((Tcl_Interp *interp, ಪೂರ್ಣಾಂಕ objc, Tcl_Obj *CONST objv[], ಪೂರ್ಣಾಂಕ objn));
ಸ್ಥಿರ ಶೂನ್ಯ tcldelthisinterp ANSI_ವಾದಗಳು ((ಶೂನ್ಯ));

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vimerror ANSI_ವಾದಗಳು((Tcl_Interp *interp));
ಸ್ಥಿರ ಶೂನ್ಯ tclmsg ANSI_ವಾದಗಳು((ಅಕ್ಷರ *ಪಠ್ಯ));
ಸ್ಥಿರ ಶೂನ್ಯ tclerrmsg ANSI_ವಾದಗಳು((ಅಕ್ಷರ *ಪಠ್ಯ));
ಸ್ಥಿರ ಶೂನ್ಯ tclupdatevars ANSI_ವಾದಗಳು((ಶೂನ್ಯ));

ಸ್ಥಿರ ರಚನೆ ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#ಆದರೆ defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define ತುಂಬು_dll vim_ತುಂಬು_Lib
#  define ಚಿಹ್ನೆ_from_dll Get_Proc_ವಿಳಾಸ
#  define close_dll FreeLibrary
#  define ತುಂಬು_dll_error GetWin32Error
# ಇಲ್ಲದಿದ್ದರೆ
#  include <dlfcn.h>
#  define HANDLE ಶೂನ್ಯ*
#  define TCL_PROC ಶೂನ್ಯ*
#  define ತುಂಬು_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define ಚಿಹ್ನೆ_from_dll dlsym
#  define close_dll dlclose
#  define ತುಂಬು_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
ಸ್ಥಿರ HANDLE hTclLib = ಖಾಲಿ;
Tcl_Interp* (*dll_Tcl_CreateInterp)(ಶೂನ್ಯ);
ಶೂನ್ಯ (*dll_Tcl_ಕಂಡುಹಿಡಿ_Executable)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *);

/*
 * Table of name to function pointer of tcl.
 */
ಸ್ಥಿರ ರಚನೆ {
    ಅಕ್ಷರ* name;
    TCL_PROC* ptr;
} tcl_funcname_ಕೋಷ್ಟಕ[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_ಕಂಡುಹಿಡಿ_Executable},
    {ಖಾಲಿ, ಖಾಲಿ},
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tcl_runtime_link_init(ಅಕ್ಷರ *libname, ಪೂರ್ಣಾಂಕ verbose)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (hTclLib)
	ಹಿಂತಿರುಗಿ OK;
    ಆದರೆ (!(hTclLib = ತುಂಬು_dll(libname)))
    {
	ಆದರೆ (verbose)
	    semsg(_(e_could_not_ತುಂಬು_library_str_str), libname, ತುಂಬು_dll_error());
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಗಾಗಿ (i = 0; tcl_funcname_ಕೋಷ್ಟಕ[i].ptr; ++i)
    {
	ಆದರೆ (!(*tcl_funcname_ಕೋಷ್ಟಕ[i].ptr = ಚಿಹ್ನೆ_from_dll(hTclLib,
						  tcl_funcname_ಕೋಷ್ಟಕ[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = ಖಾಲಿ;
	    ಆದರೆ (verbose)
		semsg(_(e_could_not_ತುಂಬು_library_function_str),
						   tcl_funcname_ಕೋಷ್ಟಕ[i].name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ // defined(DYNAMIC_TCL)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_TCL
ಸ್ಥಿರ ಅಕ್ಷರ *ಕಂಡುಹಿಡಿ_executable_arg = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
vim_tcl_init(ಅಕ್ಷರ *arg)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_TCL
    ಕಂಡುಹಿಡಿ_executable_arg = arg;
#ಇಲ್ಲದಿದ್ದರೆ
    Tcl_ಕಂಡುಹಿಡಿ_Executable(arg);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(DYNAMIC_TCL)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    ಪೂರ್ಣಾಂಕ
tcl_enabled(ಪೂರ್ಣಾಂಕ verbose)
{
    ಆದರೆ (!stubs_initialized && ಕಂಡುಹಿಡಿ_executable_arg != ಖಾಲಿ
	    && tcl_runtime_link_init((ಅಕ್ಷರ *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_ಕಂಡುಹಿಡಿ_Executable(ಕಂಡುಹಿಡಿ_executable_arg);

	ಆದರೆ ((interp = dll_Tcl_CreateInterp()) != ಖಾಲಿ)
	{
	    ಆದರೆ (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != ಖಾಲಿ)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    ಹಿಂತಿರುಗಿ stubs_initialized;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(EXITFREE)
/*
 * Called once when exiting.
 */
    ಶೂನ್ಯ
vim_tcl_finalize(ಶೂನ್ಯ)
{
# ifdef DYNAMIC_TCL
    ಆದರೆ (stubs_initialized)
# endif
	Tcl_Finalize();
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
tcl_ಕೊನೆ(ಶೂನ್ಯ)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
exitcmd(
    Client_ದತ್ತಾಂಶ dummy ಬಳಸದ,
    Tcl_Interp *interp,
    ಪೂರ್ಣಾಂಕ objc,
    Tcl_Obj *CONST objv[])
{
    ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ = 0;

    ಆಯ್ಕೆ (objc)
    {
	ಸ್ಥಿತಿ 2:
	    ಆದರೆ (Tcl_GetIntFromObj(interp, objv[1], &ಮೌಲ್ಯ) != TCL_OK)
		ನಿಲ್ಲಿಸು;
	    // FALLTHROUGH
	ಸ್ಥಿತಿ 1:
	    tclinfo.exitvalue = ಮೌಲ್ಯ;

	    Tcl_DeleteInterp(interp);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 1, objv, "?returnCode?");
    }
    ಹಿಂತಿರುಗಿ TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
beepcmd(
    Client_ದತ್ತಾಂಶ dummy ಬಳಸದ,
    Tcl_Interp *interp,
    ಪೂರ್ಣಾಂಕ objc,
    Tcl_Obj *CONST objv[])
{
    ಆದರೆ (objc != 1)
    {
	Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 1, objv, ಖಾಲಿ);
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }
    vim_beep(BO_LANG);
    ಹಿಂತಿರುಗಿ TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
buffercmd(
    Client_ದತ್ತಾಂಶ dummy ಬಳಸದ,
    Tcl_Interp *interp,
    ಪೂರ್ಣಾಂಕ objc,
    Tcl_Obj *CONST objv[])
{
    ಅಕ್ಷರ	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    ಪೂರ್ಣಾಂಕ		err, n, idx;
    ಎಣಿಕೆ {BCMD_EXISTS, BCMD_LIST};
    ಸ್ಥಿರ CONST84 ಅಕ್ಷರ *bcmdoptions[] =
    {
	"exists", "list", (ಅಕ್ಷರ *)0
    };

    ಆದರೆ (objc < 2)
    {
	Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 1, objv, "option");
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    ಆದರೆ (err == TCL_OK)
    {
	ಆದರೆ (objc != 2)
	{
	    Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 1, objv, "bufNumber");
	    ಹಿಂತಿರುಗಿ TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    ಆದರೆ (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		ಆದರೆ (name == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ TCL_ERROR;
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, name, TCL_VOLATILE);
		ಹಿಂತಿರುಗಿ TCL_OK;
	    }
	}
	Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("invalid buffer number"), TCL_STATIC);
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }
    Tcl_Reset_ಫಲಿತಾಂಶ(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_Get_ಸೂಚ್ಯಂಕ_From_Obj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    ಆದರೆ (err != TCL_OK)
	ಹಿಂತಿರುಗಿ err;
    ಆಯ್ಕೆ (idx)
    {
	ಸ್ಥಿತಿ BCMD_LIST:
	    ಆದರೆ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, "");
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		ಆದರೆ (name == ಖಾಲಿ)
		{
		    err = TCL_ERROR;
		    ನಿಲ್ಲಿಸು;
		}
		Tcl_AppendElement(interp, name);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BCMD_EXISTS:
	    ಆದರೆ (objc != 3)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    ಆದರೆ (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != ಖಾಲಿ);
		Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, resobj);
	    }
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    ಹಿಂತಿರುಗಿ err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
windowcmd(
    Client_ದತ್ತಾಂಶ	dummy ಬಳಸದ,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಂಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ಅಕ್ಷರ	*what, *string;
    win_T	*win;

    ಆದರೆ (objc != 2)
    {
	Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 1, objv, "option");
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], ಖಾಲಿ);
    ಆದರೆ (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    ಆದರೆ (string == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	ಹಿಂತಿರುಗಿ TCL_OK;
    }
    Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("unknown option"), TCL_STATIC);
    ಹಿಂತಿರುಗಿ TCL_ERROR;
}

/*
 * flags for bufselfcmd and winselfcmd to indicate outstanding actions.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು FL_UPDATE_SCREEN	(1<<0)
#ವ್ಯಾಖ್ಯಾನಿಸು FL_UPDATE_CURBUF	(1<<1)
#ವ್ಯಾಖ್ಯಾನಿಸು FL_ADJUST_CURSOR	(1<<2)

/*
 * This function implements the buffer commands.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
bufselfcmd(
    Client_ದತ್ತಾಂಶ	ref,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಂಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ಪೂರ್ಣಾಂಕ		opt, err, idx, flags;
    ಪೂರ್ಣಾಂಕ		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    ಅಕ್ಷರ	*ಸಾಲು;

    ಎಣಿಕೆ
    {
	BUF_APPEND, BUF_COMMAND, BUF_ಎಣಿಕೆ, BUF_DELCMD, BUF_DELETE, BUF_ಅಭಿವ್ಯಕ್ತಿ,
	BUF_GET, BUF_ಸೇರಿಸು, BUF_ಕೊನೆಯ, BUF_MARK, BUF_NAME, BUF_ಸಂಖ್ಯೆ,
	BUF_OPTION, BUF_ಗಣ, BUF_WINDOWS
    };
    ಸ್ಥಿರ CONST84 ಅಕ್ಷರ *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (ಅಕ್ಷರ *)0
    };

    ಆದರೆ (objc < 2)
    {
	Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 1, objv, "option ?arg ...?");
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }

    err = Tcl_Get_ಸೂಚ್ಯಂಕ_From_Obj(interp, objv[1], bufoptions, "option", 0, &idx);
    ಆದರೆ (err != TCL_OK)
	ಹಿಂತಿರುಗಿ err;

    buf = (buf_T *)((ರಚನೆ ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    ಆಯ್ಕೆ (idx)
    {
	ಸ್ಥಿತಿ BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_ಅಭಿವ್ಯಕ್ತಿ:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    ಆದರೆ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, ಖಾಲಿ);
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (buf->b_ffname)
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, (ಅಕ್ಷರ *)buf->b_ffname, TCL_VOLATILE);
	    ಇಲ್ಲದಿದ್ದರೆ
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, "", TCL_STATIC);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_ಕೊನೆಯ:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	ಸ್ಥಿತಿ BUF_ಎಣಿಕೆ:
	    /*
	     * Get number of lines in buffer.
	     */
	    ಆದರೆ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, ಖಾಲಿ);
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    val1 = (ಪೂರ್ಣಾಂಕ)buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಆದರೆ (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, resobj);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_ಸಂಖ್ಯೆ:
	    /*
	     * Get buffer's number.
	     */
	    ಆದರೆ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, ಖಾಲಿ);
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    resobj = Tcl_NewIntObj((ಪೂರ್ಣಾಂಕ)buf->b_fnum);
	    Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, resobj);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_GET:
	    ಆದರೆ (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ಆದರೆ (err != TCL_OK)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ಆದರೆ (err != TCL_OK)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_Reset_ಫಲಿತಾಂಶ(interp);

		ಗಾಗಿ (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    ಸಾಲು = (ಅಕ್ಷರ *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    ಆದರೆ (ಸಾಲು)
			Tcl_AppendElement(interp, ಸಾಲು);
		    ಇಲ್ಲದಿದ್ದರೆ
			err = TCL_ERROR;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {  // objc == 3
		ಸಾಲು = (ಅಕ್ಷರ *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, ಸಾಲು, TCL_VOLATILE);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_ಗಣ:
	    ಆದರೆ (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ಆದರೆ (err != TCL_OK)
		ಹಿಂತಿರುಗಿ TCL_ERROR;
	    ಆದರೆ (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		ಸಾಲು = Tcl_GetStringFromObj(objv[3], ಖಾಲಿ);
		ಆದರೆ (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ (ml_replace((linenr_T)val1, (char_u *)ಸಾಲು, TRUE) != OK)
		{
		    Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಬದಲಾಯಿತು_ಬೈಟುಗಳು((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		ಪೂರ್ಣಾಂಕ	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ಆದರೆ (err != TCL_OK)
		    ನಿಲ್ಲಿಸು;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		ಆದರೆ (err != TCL_OK)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		ಆದರೆ (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    ನಿಲ್ಲಿಸು;
		}
		flags |= FL_UPDATE_CURBUF;

		ಗಾಗಿ (i = 0; i < lc && n <= val2; i++)
		{
		    ಸಾಲು = Tcl_GetStringFromObj(lv[i], ಖಾಲಿ);
		    ಆದರೆ (ml_replace((linenr_T)n, (char_u *)ಸಾಲು, TRUE) != OK)
			ಹೋಗು ಗಣ_List_Error;
		    ++n;
		}
		ಆದರೆ (i < lc)
		{
		    // append lines
		    ಮಾಡು
		    {
			ಸಾಲು = Tcl_GetStringFromObj(lv[i], ಖಾಲಿ);
			ಆದರೆ (ml_append((linenr_T)(n - 1),
					      (char_u *)ಸಾಲು, 0, FALSE) != OK)
			    ಹೋಗು ಗಣ_List_Error;
			++n;
			++i;
		    } ಆಗ (i < lc);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    ಮಾಡು
		    {
			ಆದರೆ (ml_delete((linenr_T)i) != OK)
			    ಹೋಗು ಗಣ_List_Error;
			++n;
		    } ಆಗ (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (ದೀರ್ಘ)MAXLNUM,
								    (ದೀರ್ಘ)lc);
		ಬದಲಾಯಿತು_ಸಾಲುಗಳು((linenr_T)val1, 0, (linenr_T)val2 + 1, (ದೀರ್ಘ)lc);
		ನಿಲ್ಲಿಸು;
    ಗಣ_List_Error:
		u_undo(1);  // ???
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_DELETE:
	    ಆದರೆ (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ಆದರೆ (err != TCL_OK)
		ನಿಲ್ಲಿಸು;
	    val2 = val1;
	    ಆದರೆ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ಆದರೆ (err != TCL_OK)
		    ಹಿಂತಿರುಗಿ err;
		ಆದರೆ (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    ಆದರೆ (u_savedel((linenr_T)val1, (ದೀರ್ಘ)n) != OK)
	    {
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    ಗಾಗಿ (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		ಆದರೆ (err != TCL_OK)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (i > 0)
		deleted_ಸಾಲುಗಳು_mark((linenr_T)val1, (ದೀರ್ಘ)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_MARK:
	    ಆದರೆ (objc != 3)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, "markName");
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    ಸಾಲು = Tcl_GetStringFromObj(objv[2], ಖಾಲಿ);

	    pos = ಖಾಲಿ;
	    ಆದರೆ (ಸಾಲು[0] != '\0'  &&  ಸಾಲು[1] == '\0')
		pos = getmark(ಸಾಲು[0], FALSE);
	    ಆದರೆ (pos == ಖಾಲಿ)
	    {
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    err = vimerror(interp);
	    ಆದರೆ (err != TCL_OK)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (pos->lnum <= 0)
	    {
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಅಕ್ಷರ rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (ಪೂರ್ಣಾಂಕ)row2tcl(pos->lnum), (ಪೂರ್ಣಾಂಕ)col2tcl(pos->ಲಂಬಸಾಲು));
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, rbuf, TCL_VOLATILE);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_ಸೇರಿಸು:
	    opt = 1;
	    // fallthrough
	ಸ್ಥಿತಿ BUF_APPEND:
	    ಆದರೆ (objc != 4)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ಆದರೆ (err != TCL_OK)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (opt)
		--val1;
	    ಆದರೆ (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }

	    ಸಾಲು = Tcl_GetStringFromObj(objv[3], ಖಾಲಿ);
	    ಆದರೆ (ml_append((linenr_T)val1, (char_u *)ಸಾಲು, 0, FALSE) != OK)
	    {
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    appended_ಸಾಲುಗಳು_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    ಆದರೆ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, ಖಾಲಿ);
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    Tcl_Reset_ಫಲಿತಾಂಶ(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		ಆದರೆ (win->w_ಬಫರ್ == buf)
		{
		    ಸಾಲು = tclgetwindow(interp, win);
		    ಆದರೆ (ಸಾಲು != ಖಾಲಿ)
			Tcl_AppendElement(interp, ಸಾಲು);
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			err = TCL_ERROR;
			ನಿಲ್ಲಿಸು;
		    }
		}
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    ಆದರೆ (objc != 3)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, "command");
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (ಶೂನ್ಯ *)buf, objv[2]);
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    ಆದರೆ (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    ಆದರೆ (flags & FL_ADJUST_CURSOR)
	check_cursor();
    ಆದರೆ (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    ಹಿಂತಿರುಗಿ err;
}

/*
 * This function implements the window commands.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
winselfcmd(
    Client_ದತ್ತಾಂಶ	ref,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಂಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ಪೂರ್ಣಾಂಕ		err, idx, flags;
    ಪೂರ್ಣಾಂಕ		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    ಅಕ್ಷರ	*str;

    ಎಣಿಕೆ
    {
	WIN_ಬಫರ್, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_ಅಭಿವ್ಯಕ್ತಿ,
	WIN_ಎತ್ತರ, WIN_OPTION
    };
    ಸ್ಥಿರ CONST84 ಅಕ್ಷರ *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (ಅಕ್ಷರ *)0
    };

    ಆದರೆ (objc < 2)
    {
	Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 1, objv, "option ?arg ...?");
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }

    err = Tcl_Get_ಸೂಚ್ಯಂಕ_From_Obj(interp, objv[1], winoptions, "option", 0,  &idx);
    ಆದರೆ (err != TCL_OK)
	ಹಿಂತಿರುಗಿ TCL_ERROR;

    win = (win_T *)((ರಚನೆ ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_ಬಫರ್;
    flags = 0;

    ಆಯ್ಕೆ (idx)
    {
	ಸ್ಥಿತಿ WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ WIN_ಅಭಿವ್ಯಕ್ತಿ:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ WIN_ಎತ್ತರ:
	    ಆದರೆ (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		ಆದರೆ (err != TCL_OK)
		    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		win_setheight(val1);
		err = vimerror(interp);
		ಆದರೆ (err != TCL_OK)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }

	    resobj = Tcl_NewIntObj((ಪೂರ್ಣಾಂಕ)(win->w_ಎತ್ತರ));
	    Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, resobj);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ WIN_ಬಫರ್:
	    ಆದರೆ (objc != 2)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, ಖಾಲಿ);
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    str = tclgetbuffer(interp, win->w_ಬಫರ್);
	    ಆದರೆ (str)
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, str, TCL_VOLATILE);
	    ಇಲ್ಲದಿದ್ದರೆ
		err = TCL_ERROR;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ WIN_DELCMD:
	    ಆದರೆ (objc != 3)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, "command");
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (ಶೂನ್ಯ *)win, objv[2]);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ WIN_CURSOR:
	    ಆದರೆ (objc > 4)
	    {
		Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (objc == 2)
	    {
		ಅಕ್ಷರ buf[64];

		sprintf(buf, _("row %d column %d"), (ಪೂರ್ಣಾಂಕ)row2tcl(win->w_cursor.lnum), (ಪೂರ್ಣಾಂಕ)col2tcl(win->w_cursor.ಲಂಬಸಾಲು));
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, buf, TCL_VOLATILE);
		ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_ಸಂದೇಶ);
		ಆದರೆ (var == ಖಾಲಿ)
		{
		    err = TCL_ERROR;
		    ನಿಲ್ಲಿಸು;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_ಬಫರ್);
		ಆದರೆ (err != TCL_OK)
		    ನಿಲ್ಲಿಸು;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_ಸಂದೇಶ);
		ಆದರೆ (var == ಖಾಲಿ)
		{
		    err = TCL_ERROR;
		    ನಿಲ್ಲಿಸು;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		ಆದರೆ (err != TCL_OK)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_ಬಫರ್);
		ಆದರೆ (err != TCL_OK)
		    ನಿಲ್ಲಿಸು;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		ಆದರೆ (err != TCL_OK)
		    ನಿಲ್ಲಿಸು;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.ಲಂಬಸಾಲು = col2vim(val2);
	    win->w_ಗಣ_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("not implemented yet"), TCL_STATIC);
	    ನಿಲ್ಲಿಸು;
    }

    curwin = savewin;
    curbuf = savebuf;
    ಆದರೆ (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    ಹಿಂತಿರುಗಿ err;
}


    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
commandcmd(
    Client_ದತ್ತಾಂಶ	dummy ಬಳಸದ,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಂಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ಪೂರ್ಣಾಂಕ		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ಹಿಂತಿರುಗಿ err;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
optioncmd(
    Client_ದತ್ತಾಂಶ	dummy ಬಳಸದ,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಂಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ಪೂರ್ಣಾಂಕ		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ಹಿಂತಿರುಗಿ err;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
exprcmd(
    Client_ದತ್ತಾಂಶ	dummy ಬಳಸದ,
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಂಕ		objc,
    Tcl_Obj	*CONST objv[])
{
    ಹಿಂತಿರುಗಿ tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    ಪೂರ್ಣಾಂಕ		*ಮೌಲ್ಯ_P,
    buf_T	*buf)
{
    ಪೂರ್ಣಾಂಕ err, i;

    ಎಣಿಕೆ { LN_BEGIN, LN_BOTTOM, LN_ಕೊನೆ, LN_ಮೊದಲ, LN_ಕೊನೆಯ, LN_ಆರಂಭ, LN_ತುದಿ };

    ಸ್ಥಿರ CONST84 ಅಕ್ಷರ *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (ಅಕ್ಷರ *)0
    };

    err = Tcl_Get_ಸೂಚ್ಯಂಕ_From_Obj(interp, obj, keyw, "", 0, &i);
    ಆದರೆ (err == TCL_OK)
    {
	ಆಯ್ಕೆ (i)
	{
	    ಸ್ಥಿತಿ LN_BEGIN:
	    ಸ್ಥಿತಿ LN_ಮೊದಲ:
	    ಸ್ಥಿತಿ LN_ಆರಂಭ:
	    ಸ್ಥಿತಿ LN_ತುದಿ:
		*ಮೌಲ್ಯ_P = 1;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ LN_BOTTOM:
	    ಸ್ಥಿತಿ LN_ಕೊನೆ:
	    ಸ್ಥಿತಿ LN_ಕೊನೆಯ:
		*ಮೌಲ್ಯ_P = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		ನಿಲ್ಲಿಸು;
	}
	ಹಿಂತಿರುಗಿ TCL_OK;
    }
    Tcl_Reset_ಫಲಿತಾಂಶ(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    ಆದರೆ (err != TCL_OK)
	ಹಿಂತಿರುಗಿ err;
    i = row2vim(i);
    ಆದರೆ (i < 1  ||  i > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("line number out of range"), TCL_STATIC);
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }
    *ಮೌಲ್ಯ_P = i;
    ಹಿಂತಿರುಗಿ TCL_OK;
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
	ಆದರೆ (win->w_ಬಫರ್ == buf)
	    ಹಿಂತಿರುಗಿ win;
    }
    ಹಿಂತಿರುಗಿ curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tcldoexcommand(
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಂಕ		objc,
    Tcl_Obj	*CONST objv[],
    ಪೂರ್ಣಾಂಕ		objn)
{
    tcl_info	saveinfo;
    ಪೂರ್ಣಾಂಕ		err, flag, nobjs;
    ಅಕ್ಷರ	*arg;

    nobjs = objc - objn;
    ಆದರೆ (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, objn, objv, "?-quiet? exCommand");
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }

    flag = 0;
    ಆದರೆ (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], ಖಾಲಿ);
	ಆದರೆ (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    Tcl_Reset_ಫಲಿತಾಂಶ(interp);
	    Tcl_Append_ಫಲಿತಾಂಶ(interp, _("unknown flag: "), arg, (ಅಕ್ಷರ *)0);
	    ಹಿಂತಿರುಗಿ TCL_ERROR;
	}
	++objn;
    }

    ಸ್ಮರಣೆ_ನಕಲು(&saveinfo, &tclinfo, ಗಾತ್ರ(tcl_info));
    tclinfo.interp = ಖಾಲಿ;
    tclinfo.curwin = ಖಾಲಿ;
    tclinfo.curbuf = ಖಾಲಿ;

    arg = Tcl_GetStringFromObj(objv[objn], ಖಾಲಿ);
    ಆದರೆ (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    ಆದರೆ (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    ಆದರೆ (tclinfo.interp)
	tcldelthisinterp();
    ಸ್ಮರಣೆ_ನಕಲು(&tclinfo, &saveinfo, ಗಾತ್ರ(tcl_info));
    tclupdatevars();

    ಹಿಂತಿರುಗಿ err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tclsetoption(
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಂಕ		objc,
    Tcl_Obj	*CONST objv[],
    ಪೂರ್ಣಾಂಕ		objn)
{
    ಪೂರ್ಣಾಂಕ		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    ದೀರ್ಘ	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    ಎಣಿಕೆ { OPT_OFF, OPT_ON, OPT_TOGGLE };
    ಸ್ಥಿರ CONST84 ಅಕ್ಷರ *optkw[] = { "off", "on", "toggle", (ಅಕ್ಷರ *)0 };

    nobjs = objc - objn;
    ಆದರೆ (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, objn, objv, "vimOption ?value?");
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], ಖಾಲಿ);
    ++objn;
    gov = get_option_ಮೌಲ್ಯ(option, &lval, &sval, ಖಾಲಿ, 0);
    err = TCL_OK;
    ಆಯ್ಕೆ (gov)
    {
	ಸ್ಥಿತಿ gov_string:
	    Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, (ಅಕ್ಷರ *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ gov_bool:
	ಸ್ಥಿತಿ gov_ಸಂಖ್ಯೆ:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, resobj);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("unknown vimOption"), TCL_STATIC);
	    ಹಿಂತಿರುಗಿ TCL_ERROR;
    }
    ಆದರೆ (nobjs == 2)
    {
	ಆದರೆ (gov != gov_string)
	{
	    sval = ಖಾಲಿ;    // avoid compiler warning
	    err = Tcl_Get_ಸೂಚ್ಯಂಕ_From_Obj(interp, objv[objn], optkw, "", 0, &idx);
	    ಆದರೆ (err != TCL_OK)
	    {
		Tcl_Reset_ಫಲಿತಾಂಶ(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆಯ್ಕೆ (idx)
		{
		    ಸ್ಥಿತಿ OPT_ON:
			lval = 1;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ OPT_OFF:
			lval = 0;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ OPT_TOGGLE:
			lval = !lval;
			ನಿಲ್ಲಿಸು;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], ಖಾಲಿ);
	ಆದರೆ (err == TCL_OK)
	{
	    ಗಣ_option_ಮೌಲ್ಯ_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    ಹಿಂತಿರುಗಿ err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tclvimexpr(
    Tcl_Interp	*interp,
    ಪೂರ್ಣಾಂಕ		objc,
    Tcl_Obj	*CONST objv[],
    ಪೂರ್ಣಾಂಕ		objn)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಅಕ್ಷರ	*ಅಭಿವ್ಯಕ್ತಿ, *str;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		err;

    ಆದರೆ (objc - objn != 1)
    {
	Tcl_Wrong_ಅಂಕೆ_ವಾದಗಳು(interp, objn, objv, "vimExpr");
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಅಭಿವ್ಯಕ್ತಿ = Tcl_GetStringFromObj(objv[objn], ಖಾಲಿ);
    str = (ಅಕ್ಷರ *)eval_ಗಮ್ಯ_string((char_u *)ಅಭಿವ್ಯಕ್ತಿ, TRUE, FALSE);
    ಆದರೆ (str == ಖಾಲಿ)
	Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("invalid expression"), TCL_STATIC);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#ಇಲ್ಲದಿದ್ದರೆ
    Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ err;
}

/*
 * Check for internal vim errors.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vimerror(Tcl_Interp *interp)
{
    ಆದರೆ (got_int)
    {
	Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("keyboard interrupt"), TCL_STATIC);
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (did_emsg)
    {
	Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("Vim error"), TCL_STATIC);
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }
    ಹಿಂತಿರುಗಿ TCL_OK;
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
delref(Client_ದತ್ತಾಂಶ cref)
{
    ರಚನೆ ref *ref = (ರಚನೆ ref *)cref;

    ಆದರೆ (ref->delcmd)
    {
	Tcl_Decr_Ref_ಎಣಿಕೆ(ref->delcmd);
	ref->delcmd = ಖಾಲಿ;
    }
    ref->interp = ಖಾಲಿ;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
tclgetref(
    Tcl_Interp	*interp,
    ಶೂನ್ಯ	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    ಅಕ್ಷರ	*ಪೂರ್ವಪ್ರತ್ಯಯ,	// "win" or "buf"
    ಶೂನ್ಯ	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    ರಚನೆ ref *ref, *ಬಳಸದ_2 = ಖಾಲಿ;
    ಸ್ಥಿರ ಅಕ್ಷರ name[VARNAME_ಗಾತ್ರ];
    Tcl_Command cmd;

    ref = (ರಚನೆ ref *)(*refstartP);
    ಆದರೆ (ref == &refsdeleted)
    {
	Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆಗ (ref != ಖಾಲಿ)
    {
	ಆದರೆ (ref->interp == interp)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (ref->interp == ಖಾಲಿ)
	    ಬಳಸದ_2 = ref;
	ref = ref->ಮುಂದಿನ;
    }

    ಆದರೆ (ref)
	vim_snprintf(name, ಗಾತ್ರ(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಬಳಸದ_2)
	    ref = ಬಳಸದ_2;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ref = (ರಚನೆ ref *)Tcl_Alloc(ಗಾತ್ರ(ರಚನೆ ref));
	    ref->interp = ಖಾಲಿ;
	    ref->ಮುಂದಿನ = (ರಚನೆ ref *)(*refstartP);
	    (*refstartP) = (ಶೂನ್ಯ *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, ಗಾತ್ರ(name), "::vim::%s_%lx",
					       ಪೂರ್ವಪ್ರತ್ಯಯ, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (Client_ದತ್ತಾಂಶ)ref, (Tcl_CmdDeleteProc *)delref);
	ಆದರೆ (!cmd)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = ಖಾಲಿ;
	ref->vimobj = vimobj;
    }
    ಹಿಂತಿರುಗಿ name;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    ಹಿಂತಿರುಗಿ tclgetref(interp, &(win->w_tcl_ref), "win", (ಶೂನ್ಯ *)win, winselfcmd);
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    ಹಿಂತಿರುಗಿ tclgetref(interp, &(buf->b_tcl_ref), "buf", (ಶೂನ್ಯ *)buf, bufselfcmd);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tclsetdelcmd(
    Tcl_Interp	*interp,
    ರಚನೆ ref	*reflist,
    ಶೂನ್ಯ	*vimobj,
    Tcl_Obj	*delcmd)
{
    ಆದರೆ (reflist == &refsdeleted)
    {
	Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	ಹಿಂತಿರುಗಿ TCL_ERROR;
    }

    ಆಗ (reflist != ಖಾಲಿ)
    {
	ಆದರೆ (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    ಆದರೆ (reflist->delcmd)
		Tcl_Decr_Ref_ಎಣಿಕೆ(reflist->delcmd);
	    Tcl_Incr_Ref_ಎಣಿಕೆ(delcmd);
	    reflist->delcmd = delcmd;
	    ಹಿಂತಿರುಗಿ TCL_OK;
	}
	reflist = reflist->ಮುಂದಿನ;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    ಹಿಂತಿರುಗಿ TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tcl_channel_close(Client_ದತ್ತಾಂಶ instance, Tcl_Interp *interp ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		err = 0;

    // currently does nothing

    ಆದರೆ (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_ಗಣ_Errno(EBADF);
	err = EBADF;
    }
    ಹಿಂತಿರುಗಿ err;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tcl_channel_ಒಳಹರಿವು(
    Client_ದತ್ತಾಂಶ	instance ಬಳಸದ,
    ಅಕ್ಷರ	*buf ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		bufsiz ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		*errptr)
{

    // input is currently not supported

    Tcl_ಗಣ_Errno(EINVAL);
    ಆದರೆ (errptr)
	*errptr = EINVAL;
    ಹಿಂತಿರುಗಿ -1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tcl_channel_output(
    Client_ದತ್ತಾಂಶ	instance,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*buf,
    ಪೂರ್ಣಾಂಕ		bufsiz,
    ಪೂರ್ಣಾಂಕ		*errptr)
{
    char_u	*str;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    ಆದರೆ (!str)
    {
	Tcl_ಗಣ_Errno(ENOMEM);
	ಆದರೆ (errptr)
	    *errptr = ENOMEM;
	ಹಿಂತಿರುಗಿ -1;
    }

    ಫಲಿತಾಂಶ = bufsiz;
    ಆದರೆ (instance == VIMOUT)
	tclmsg((ಅಕ್ಷರ *)str);
    ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (instance == VIMERR)
	tclerrmsg((ಅಕ್ಷರ *)str);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Tcl_ಗಣ_Errno(EBADF);
	ಆದರೆ (errptr)
	    *errptr = EBADF;
	ಫಲಿತಾಂಶ = -1;
    }
    vim_free(str);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tcl_channel_watch(Client_ದತ್ತಾಂಶ instance ಬಳಸದ, ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ ಬಳಸದ)
{
    Tcl_ಗಣ_Errno(EINVAL);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tcl_channel_gethandle(
    Client_ದತ್ತಾಂಶ	instance ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		direction ಬಳಸದ,
    Client_ದತ್ತಾಂಶ	*handleptr ಬಳಸದ)
{
    Tcl_ಗಣ_Errno(EINVAL);
    ಹಿಂತಿರುಗಿ EINVAL;
}


ಸ್ಥಿರ Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_ಆವೃತ್ತಿ_2, //ಆವೃತ್ತಿ
    tcl_channel_close,	// closeProc
    tcl_channel_ಒಳಹರಿವು,	// inputProc
    tcl_channel_output,	// outputProc
    ಖಾಲಿ,		// seekProc
    ಖಾಲಿ,		// setOptionProc
    ಖಾಲಿ,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    ಖಾಲಿ,		// close2Proc
    ಖಾಲಿ,		// blockModeProc
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TCL_CHANNEL_ಆವೃತ್ತಿ_2
    ಖಾಲಿ,		// flushProc
    ಖಾಲಿ,		// handlerProc
#ಆದರೆ_ಅಂತ್ಯ
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TCL_CHANNEL_ಆವೃತ್ತಿ_3
    ಖಾಲಿ,		// wideSeekProc
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TCL_CHANNEL_ಆವೃತ್ತಿ_4
    ಖಾಲಿ,		// threadActionProc
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TCL_CHANNEL_ಆವೃತ್ತಿ_5
    ಖಾಲಿ		// truncateProc
#ಆದರೆ_ಅಂತ್ಯ
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    ಸ್ಥಿರ ಶೂನ್ಯ
tclupdatevars(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ varname[VARNAME_ಗಾತ್ರ];	// must be writeable
    ಅಕ್ಷರ *name;

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


    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tclinit(exarg_T *eap)
{
    ಅಕ್ಷರ varname[VARNAME_ಗಾತ್ರ];	// Tcl_LinkVar requires writeable varname
    ಅಕ್ಷರ *name;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_TCL
    ಆದರೆ (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	ಹಿಂತಿರುಗಿ FAIL;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	ಸ್ಥಿರ Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_ಗಣ_Std_Channel(ch1, TCL_STDOUT);
	Tcl_ಗಣ_Std_Channel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	ಆದರೆ (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
#ಆದರೆ 0
	// VIM sure is interactive
	Tcl_ಗಣ_Var(interp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
#ಆದರೆ_ಅಂತ್ಯ

	Tcl_ಗಣ_Channel_Option(interp, ch1, "-buffering", "line");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	Tcl_ಗಣ_Channel_Option(interp, ch1, "-translation", "lf");
#ಆದರೆ_ಅಂತ್ಯ
	Tcl_ಗಣ_Channel_Option(interp, ch2, "-buffering", "line");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	Tcl_ಗಣ_Channel_Option(interp, ch2, "-translation", "lf");
#ಆದರೆ_ಅಂತ್ಯ

	// replace standard Tcl exit command
	Tcl_DeleteCommand(interp, "exit");
	Tcl_CreateObjCommand(interp, "exit", exitcmd,
	    (Client_ದತ್ತಾಂಶ)ಖಾಲಿ, (Tcl_CmdDeleteProc *)ಖಾಲಿ);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (Client_ದತ್ತಾಂಶ)ಖಾಲಿ, (Tcl_CmdDeleteProc *)ಖಾಲಿ);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (Client_ದತ್ತಾಂಶ)ಖಾಲಿ, (Tcl_CmdDeleteProc *)ಖಾಲಿ);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (Client_ದತ್ತಾಂಶ)ಖಾಲಿ, (Tcl_CmdDeleteProc *)ಖಾಲಿ);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (Client_ದತ್ತಾಂಶ)ಖಾಲಿ, (Tcl_CmdDeleteProc *)ಖಾಲಿ);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (Client_ದತ್ತಾಂಶ)ಖಾಲಿ, (Tcl_CmdDeleteProc *)ಖಾಲಿ);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (Client_ದತ್ತಾಂಶ)ಖಾಲಿ, (Tcl_CmdDeleteProc *)ಖಾಲಿ);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (ಅಕ್ಷರ *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_ಆರಂಭ = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (ಅಕ್ಷರ *)&tclinfo.range_ಆರಂಭ, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (ಅಕ್ಷರ *)&tclinfo.range_ಆರಂಭ, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_ಕೊನೆ   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (ಅಕ್ಷರ *)&tclinfo.range_ಕೊನೆ, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_ಗಾತ್ರ);
	tclinfo.curwin = Tcl_Alloc(VARNAME_ಗಾತ್ರ);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (ಅಕ್ಷರ *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (ಅಕ್ಷರ *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Interpreter already exists, just update variables
	tclinfo.range_ಆರಂಭ = row2tcl(eap->line1);
	tclinfo.range_ಕೊನೆ = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tclerrmsg(ಅಕ್ಷರ *ಪಠ್ಯ)
{
    ಅಕ್ಷರ *ಮುಂದಿನ;

    ಆಗ ((ಮುಂದಿನ=strchr(ಪಠ್ಯ, '\n')))
    {
	*ಮುಂದಿನ++ = '\0';
	emsg(ಪಠ್ಯ);
	ಪಠ್ಯ = ಮುಂದಿನ;
    }
    ಆದರೆ (*ಪಠ್ಯ)
	emsg(ಪಠ್ಯ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tclmsg(ಅಕ್ಷರ *ಪಠ್ಯ)
{
    ಅಕ್ಷರ *ಮುಂದಿನ;

    ಆಗ ((ಮುಂದಿನ=strchr(ಪಠ್ಯ, '\n')))
    {
	*ಮುಂದಿನ++ = '\0';
	ಸಂದೇಶ(ಪಠ್ಯ);
	ಪಠ್ಯ = ಮುಂದಿನ;
    }
    ಆದರೆ (*ಪಠ್ಯ)
	ಸಂದೇಶ(ಪಠ್ಯ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tcldelthisinterp(ಶೂನ್ಯ)
{
    ಆದರೆ (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = ಖಾಲಿ;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = ಖಾಲಿ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tclexit(ಪೂರ್ಣಾಂಕ error)
{
    ಪೂರ್ಣಾಂಕ newerr = OK;

    ಆದರೆ (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#ಆದರೆ (TCL_MAJOR_ಆವೃತ್ತಿ == 8 && TCL_MINOR_ಆವೃತ್ತಿ >= 5) || TCL_MAJOR_ಆವೃತ್ತಿ > 8
	|| Tcl_ಎಲ್ಲೆ_Exceeded(tclinfo.interp)  // True if the interpreter cannot continue
#ಆದರೆ_ಅಂತ್ಯ
	)
    {
	ಅಕ್ಷರ buf[50];

	sprintf(buf, _(e_exit_ಸಂಕೇತ_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	ಆದರೆ (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    newerr = FAIL;

	tcldelthisinterp();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಕ್ಷರ *ಫಲಿತಾಂಶ;

	ಫಲಿತಾಂಶ = (ಅಕ್ಷರ *)Tcl_Get_String_ಫಲಿತಾಂಶ(tclinfo.interp);
	ಆದರೆ (error == TCL_OK)
	{
	    tclmsg(ಫಲಿತಾಂಶ);
	    newerr = OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    tclerrmsg(ಫಲಿತಾಂಶ);
	    newerr = FAIL;
	}
    }

    ಹಿಂತಿರುಗಿ newerr;
}

/*
 * ":tcl"
 */
    ಶೂನ್ಯ
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    ಪೂರ್ಣಾಂಕ		err;

    script = script_get(eap, eap->arg);
    ಆದರೆ (!eap->skip)
    {
	err = tclinit(eap);
	ಆದರೆ (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    ಆದರೆ (script == ಖಾಲಿ)
		err = Tcl_Eval(tclinfo.interp, (ಅಕ್ಷರ *)eap->arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		err = Tcl_Eval(tclinfo.interp, (ಅಕ್ಷರ *)script);
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
    ಅಕ್ಷರ *file = (ಅಕ್ಷರ *)eap->arg;
    ಪೂರ್ಣಾಂಕ err;

    err = tclinit(eap);
    ಆದರೆ (err == OK)
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
    ಅಕ್ಷರ	*script, *ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		err, rs, re, lnum;
    ಅಕ್ಷರ	var_lnum[VARNAME_ಗಾತ್ರ]; // must be writeable memory
    ಅಕ್ಷರ	var_ಸಾಲು[VARNAME_ಗಾತ್ರ];
    linenr_T	ಮೊದಲ_ಸಾಲು = 0;
    linenr_T	ಕೊನೆಯ_ಸಾಲು = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (ಅಕ್ಷರ *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_ಸಾಲು, VAR_CURLINE);

    err = tclinit(eap);
    ಆದರೆ (err != OK)
	ಹಿಂತಿರುಗಿ;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (ಅಕ್ಷರ *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    ಆದರೆ (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_ಗಣ_ಫಲಿತಾಂಶ(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    ಆಗ (err == TCL_OK  &&  rs <= re)
    {
	ಆದರೆ ((linenr_T)rs > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ನಿಲ್ಲಿಸು;
	ಸಾಲು = (ಅಕ್ಷರ *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	ಆದರೆ (!ಸಾಲು)
	{
	    Tcl_ಗಣ_ಫಲಿತಾಂಶ(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    ನಿಲ್ಲಿಸು;
	}
	Tcl_ಗಣ_Var(tclinfo.interp, var_ಸಾಲು, ಸಾಲು, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	ಆದರೆ (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#ಆದರೆ (TCL_MAJOR_ಆವೃತ್ತಿ == 8 && TCL_MINOR_ಆವೃತ್ತಿ >= 5) || TCL_MAJOR_ಆವೃತ್ತಿ > 8
	    || Tcl_ಎಲ್ಲೆ_Exceeded(tclinfo.interp)
#ಆದರೆ_ಅಂತ್ಯ
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ನಿಲ್ಲಿಸು;
	ಸಾಲು = (ಅಕ್ಷರ *)Tcl_GetVar(tclinfo.interp, var_ಸಾಲು, 0);
	ಆದರೆ (ಸಾಲು)
	{
	    ಆದರೆ (ml_replace((linenr_T)rs, (char_u *)ಸಾಲು, TRUE) != OK)
	    {
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (ಮೊದಲ_ಸಾಲು == 0)
		ಮೊದಲ_ಸಾಲು = rs;
	    ಕೊನೆಯ_ಸಾಲು = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    ಆದರೆ (ಮೊದಲ_ಸಾಲು)
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(ಮೊದಲ_ಸಾಲು, 0, ಕೊನೆಯ_ಸಾಲು + 1, (ದೀರ್ಘ)0);

    Tcl_UnsetVar(tclinfo.interp, var_ಸಾಲು, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    ಆದರೆ (err == TCL_OK)
	Tcl_Reset_ಫಲಿತಾಂಶ(tclinfo.interp);

    (ಶೂನ್ಯ)tclexit(err);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tcldelallrefs(ರಚನೆ ref *ref)
{
    ರಚನೆ ref	*ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ		err;
    ಅಕ್ಷರ	*ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    ಆದರೆ (exiting)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆಗ (ref != ಖಾಲಿ)
    {
	ಮುಂದಿನ = ref->ಮುಂದಿನ;
	ಆದರೆ (ref->interp)
	{
	    ಆದರೆ (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		ಆದರೆ (err != TCL_OK)
		{
		    ಫಲಿತಾಂಶ = (ಅಕ್ಷರ *)Tcl_Get_String_ಫಲಿತಾಂಶ(ref->interp);
		    ಆದರೆ (ಫಲಿತಾಂಶ)
			tclerrmsg(ಫಲಿತಾಂಶ);
		}
		Tcl_Decr_Ref_ಎಣಿಕೆ(ref->delcmd);
		ref->delcmd = ಖಾಲಿ;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((ಅಕ್ಷರ *)ref);
	ref = ಮುಂದಿನ;
    }
}

    ಶೂನ್ಯ
tcl_ಬಫರ್_free(buf_T *buf)
{
    ರಚನೆ ref *reflist;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_TCL
    ಆದರೆ (!stubs_initialized)	// Not using Tcl, nothing to do.
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    reflist = (ರಚನೆ ref *)(buf->b_tcl_ref);
    ಆದರೆ (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (ಶೂನ್ಯ *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = ಖಾಲಿ;
    }
}

    ಶೂನ್ಯ
tcl_ಕಿಟಕಿ_free(win_T *win)
{
    ರಚನೆ ref *reflist;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_TCL
    ಆದರೆ (!stubs_initialized)	// Not using Tcl, nothing to do.
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    reflist = (ರಚನೆ ref*)(win->w_tcl_ref);
    ಆದರೆ (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (ಶೂನ್ಯ *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = ಖಾಲಿ;
    }
}

// The End
