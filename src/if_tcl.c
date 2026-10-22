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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ EXTERN			// tcl.h defines it too

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <tcl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Tcl_Interp *interp;
    ਪੂਰਨ exitvalue;
    ਪੂਰਨ range_ਸ਼ੁਰੂ, range_ਸਿਰਾ;
    ਪੂਰਨ lbase;
    ਅੱਖਰ *curbuf, *curwin;
} tcl_info;

ਸਥਿਰ tcl_info tclinfo = { ਨਿਰਾ, 0, 0, 0, 0, ਨਿਰਾ, ਨਿਰਾ };

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_RANGE1	"::vim::range(start)"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_RANGE2	"::vim::range(begin)"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_RANGE3	"::vim::range(end)"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_CURBUF	"::vim::current(buffer)"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_CURWIN	"::vim::current(window)"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_LBASE	"::vim::lbase"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_CURLINE	"line"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_CURLNUM	"lnum"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VARNAME_ਮਾਪ	64

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ row2tcl(x)  ((x) - (tclinfo.lbase==0))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ row2vim(x)  ((x) + (tclinfo.lbase==0))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ col2tcl(x)  ((x) + (tclinfo.lbase!=0))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ col2vim(x)  ((x) - (tclinfo.lbase!=0))


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMOUT	((Client_ਅੰਕੜੇ)1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMERR	((Client_ਅੰਕੜੇ)2)

// This appears to be new in Tcl 8.4.
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONST84
# define CONST84
#ਜੇ_ਅੰਤ

/*
 *  List of Tcl interpreters who reference a vim window or buffer.
 *  Each buffer and window has its own list in the w_tcl_ref or b_tcl_ref
 *  struct member.  We need this because Tcl can create sub-interpreters with
 *  the "interp" command, and each interpreter can reference all windows and
 *  buffers.
 */
ਬਣਤਰ ref
{
    ਬਣਤਰ ref	*ਅਗਲਾ;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    ਖਾਲੀ	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
ਸਥਿਰ ਅੱਖਰ * tclgetbuffer ANSI_ਦਲੀਲਾਂ((Tcl_Interp *interp, buf_T *buf));
ਸਥਿਰ ਅੱਖਰ * tclgetwindow ANSI_ਦਲੀਲਾਂ((Tcl_Interp *interp, win_T *win));
ਸਥਿਰ ਪੂਰਨ tclsetdelcmd ANSI_ਦਲੀਲਾਂ((Tcl_Interp *interp, ਬਣਤਰ ref *reflist, ਖਾਲੀ *vimobj, Tcl_Obj *delcmd));
ਸਥਿਰ ਪੂਰਨ tclgetlinenum ANSI_ਦਲੀਲਾਂ ((Tcl_Interp *interp, Tcl_Obj *obj, ਪੂਰਨ *ਮੁੱਲ_P, buf_T *buf));
ਸਥਿਰ win_T *tclfindwin ANSI_ਦਲੀਲਾਂ ((buf_T *buf));
ਸਥਿਰ ਪੂਰਨ tcldoexcommand ANSI_ਦਲੀਲਾਂ ((Tcl_Interp *interp, ਪੂਰਨ objc, Tcl_Obj *CONST objv[], ਪੂਰਨ objn));
ਸਥਿਰ ਪੂਰਨ tclsetoption ANSI_ਦਲੀਲਾਂ ((Tcl_Interp *interp, ਪੂਰਨ objc, Tcl_Obj *CONST objv[], ਪੂਰਨ objn));
ਸਥਿਰ ਪੂਰਨ tclvimexpr ANSI_ਦਲੀਲਾਂ ((Tcl_Interp *interp, ਪੂਰਨ objc, Tcl_Obj *CONST objv[], ਪੂਰਨ objn));
ਸਥਿਰ ਖਾਲੀ tcldelthisinterp ANSI_ਦਲੀਲਾਂ ((ਖਾਲੀ));

ਸਥਿਰ ਪੂਰਨ vimerror ANSI_ਦਲੀਲਾਂ((Tcl_Interp *interp));
ਸਥਿਰ ਖਾਲੀ tclmsg ANSI_ਦਲੀਲਾਂ((ਅੱਖਰ *ਪਾਠ));
ਸਥਿਰ ਖਾਲੀ tclerrmsg ANSI_ਦਲੀਲਾਂ((ਅੱਖਰ *ਪਾਠ));
ਸਥਿਰ ਖਾਲੀ tclupdatevars ANSI_ਦਲੀਲਾਂ((ਖਾਲੀ));

ਸਥਿਰ ਬਣਤਰ ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#ਜੇ defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define ਲੱਦੋ_dll vim_ਲੱਦੋ_Lib
#  define ਚਿੰਨ੍ਹ_from_dll Get_Proc_ਪਤਾ
#  define close_dll FreeLibrary
#  define ਲੱਦੋ_dll_error GetWin32Error
# ਨਹੀਂਤਾਂ
#  include <dlfcn.h>
#  define HANDLE ਖਾਲੀ*
#  define TCL_PROC ਖਾਲੀ*
#  define ਲੱਦੋ_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define ਚਿੰਨ੍ਹ_from_dll dlsym
#  define close_dll dlclose
#  define ਲੱਦੋ_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
ਸਥਿਰ HANDLE hTclLib = ਨਿਰਾ;
Tcl_Interp* (*dll_Tcl_CreateInterp)(ਖਾਲੀ);
ਖਾਲੀ (*dll_Tcl_ਲੱਭੋ_Executable)(ਅਚਲ ਖਾਲੀ *);

/*
 * Table of name to function pointer of tcl.
 */
ਸਥਿਰ ਬਣਤਰ {
    ਅੱਖਰ* name;
    TCL_PROC* ptr;
} tcl_funcname_ਸਾਰਣੀ[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_ਲੱਭੋ_Executable},
    {ਨਿਰਾ, ਨਿਰਾ},
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
    ਸਥਿਰ ਪੂਰਨ
tcl_runtime_link_init(ਅੱਖਰ *libname, ਪੂਰਨ verbose)
{
    ਪੂਰਨ i;

    ਜੇ (hTclLib)
	ਵਾਪਸ OK;
    ਜੇ (!(hTclLib = ਲੱਦੋ_dll(libname)))
    {
	ਜੇ (verbose)
	    semsg(_(e_could_not_ਲੱਦੋ_library_str_str), libname, ਲੱਦੋ_dll_error());
	ਵਾਪਸ FAIL;
    }
    ਲਈ (i = 0; tcl_funcname_ਸਾਰਣੀ[i].ptr; ++i)
    {
	ਜੇ (!(*tcl_funcname_ਸਾਰਣੀ[i].ptr = ਚਿੰਨ੍ਹ_from_dll(hTclLib,
						  tcl_funcname_ਸਾਰਣੀ[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = ਨਿਰਾ;
	    ਜੇ (verbose)
		semsg(_(e_could_not_ਲੱਦੋ_library_function_str),
						   tcl_funcname_ਸਾਰਣੀ[i].name);
	    ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ // defined(DYNAMIC_TCL)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_TCL
ਸਥਿਰ ਅੱਖਰ *ਲੱਭੋ_executable_arg = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
vim_tcl_init(ਅੱਖਰ *arg)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_TCL
    ਲੱਭੋ_executable_arg = arg;
#ਨਹੀਂਤਾਂ
    Tcl_ਲੱਭੋ_Executable(arg);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(DYNAMIC_TCL)

ਸਥਿਰ ਪੂਰਨ stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    ਪੂਰਨ
tcl_enabled(ਪੂਰਨ verbose)
{
    ਜੇ (!stubs_initialized && ਲੱਭੋ_executable_arg != ਨਿਰਾ
	    && tcl_runtime_link_init((ਅੱਖਰ *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_ਲੱਭੋ_Executable(ਲੱਭੋ_executable_arg);

	ਜੇ ((interp = dll_Tcl_CreateInterp()) != ਨਿਰਾ)
	{
	    ਜੇ (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != ਨਿਰਾ)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    ਵਾਪਸ stubs_initialized;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(EXITFREE)
/*
 * Called once when exiting.
 */
    ਖਾਲੀ
vim_tcl_finalize(ਖਾਲੀ)
{
# ifdef DYNAMIC_TCL
    ਜੇ (stubs_initialized)
# endif
	Tcl_Finalize();
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
tcl_ਸਿਰਾ(ਖਾਲੀ)
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
    ਸਥਿਰ ਪੂਰਨ
exitcmd(
    Client_ਅੰਕੜੇ dummy ਅਣਵਰਤਿਆ,
    Tcl_Interp *interp,
    ਪੂਰਨ objc,
    Tcl_Obj *CONST objv[])
{
    ਪੂਰਨ ਮੁੱਲ = 0;

    ਚੋਣ (objc)
    {
	ਹਾਲਤ 2:
	    ਜੇ (Tcl_GetIntFromObj(interp, objv[1], &ਮੁੱਲ) != TCL_OK)
		ਰੋਕੋ;
	    // FALLTHROUGH
	ਹਾਲਤ 1:
	    tclinfo.exitvalue = ਮੁੱਲ;

	    Tcl_DeleteInterp(interp);
	    ਰੋਕੋ;
	ਮੂਲ:
	    Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 1, objv, "?returnCode?");
    }
    ਵਾਪਸ TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    ਸਥਿਰ ਪੂਰਨ
beepcmd(
    Client_ਅੰਕੜੇ dummy ਅਣਵਰਤਿਆ,
    Tcl_Interp *interp,
    ਪੂਰਨ objc,
    Tcl_Obj *CONST objv[])
{
    ਜੇ (objc != 1)
    {
	Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 1, objv, ਨਿਰਾ);
	ਵਾਪਸ TCL_ERROR;
    }
    vim_beep(BO_LANG);
    ਵਾਪਸ TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    ਸਥਿਰ ਪੂਰਨ
buffercmd(
    Client_ਅੰਕੜੇ dummy ਅਣਵਰਤਿਆ,
    Tcl_Interp *interp,
    ਪੂਰਨ objc,
    Tcl_Obj *CONST objv[])
{
    ਅੱਖਰ	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    ਪੂਰਨ		err, n, idx;
    ਗਿਣਤੀ {BCMD_EXISTS, BCMD_LIST};
    ਸਥਿਰ CONST84 ਅੱਖਰ *bcmdoptions[] =
    {
	"exists", "list", (ਅੱਖਰ *)0
    };

    ਜੇ (objc < 2)
    {
	Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 1, objv, "option");
	ਵਾਪਸ TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    ਜੇ (err == TCL_OK)
    {
	ਜੇ (objc != 2)
	{
	    Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 1, objv, "bufNumber");
	    ਵਾਪਸ TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    ਜੇ (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		ਜੇ (name == ਨਿਰਾ)
		    ਵਾਪਸ TCL_ERROR;
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, name, TCL_VOLATILE);
		ਵਾਪਸ TCL_OK;
	    }
	}
	Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("invalid buffer number"), TCL_STATIC);
	ਵਾਪਸ TCL_ERROR;
    }
    Tcl_Reset_ਨਤੀਜਾ(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_Get_ਸੂਚਕਾਂਕ_From_Obj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    ਜੇ (err != TCL_OK)
	ਵਾਪਸ err;
    ਚੋਣ (idx)
    {
	ਹਾਲਤ BCMD_LIST:
	    ਜੇ (objc != 2)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, "");
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		ਜੇ (name == ਨਿਰਾ)
		{
		    err = TCL_ERROR;
		    ਰੋਕੋ;
		}
		Tcl_AppendElement(interp, name);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ BCMD_EXISTS:
	    ਜੇ (objc != 3)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    ਜੇ (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != ਨਿਰਾ);
		Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, resobj);
	    }
	    ਰੋਕੋ;

	ਮੂਲ:
	    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    ਵਾਪਸ err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    ਸਥਿਰ ਪੂਰਨ
windowcmd(
    Client_ਅੰਕੜੇ	dummy ਅਣਵਰਤਿਆ,
    Tcl_Interp	*interp,
    ਪੂਰਨ		objc,
    Tcl_Obj	*CONST objv[])
{
    ਅੱਖਰ	*what, *string;
    win_T	*win;

    ਜੇ (objc != 2)
    {
	Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 1, objv, "option");
	ਵਾਪਸ TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], ਨਿਰਾ);
    ਜੇ (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    ਜੇ (string == ਨਿਰਾ)
		ਵਾਪਸ TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	ਵਾਪਸ TCL_OK;
    }
    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("unknown option"), TCL_STATIC);
    ਵਾਪਸ TCL_ERROR;
}

/*
 * flags for bufselfcmd and winselfcmd to indicate outstanding actions.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FL_UPDATE_SCREEN	(1<<0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FL_UPDATE_CURBUF	(1<<1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FL_ADJUST_CURSOR	(1<<2)

/*
 * This function implements the buffer commands.
 */
    ਸਥਿਰ ਪੂਰਨ
bufselfcmd(
    Client_ਅੰਕੜੇ	ref,
    Tcl_Interp	*interp,
    ਪੂਰਨ		objc,
    Tcl_Obj	*CONST objv[])
{
    ਪੂਰਨ		opt, err, idx, flags;
    ਪੂਰਨ		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    ਅੱਖਰ	*ਸਤਰ;

    ਗਿਣਤੀ
    {
	BUF_APPEND, BUF_COMMAND, BUF_ਗਿਣਤੀ, BUF_DELCMD, BUF_DELETE, BUF_ਵਿਅੰਜਕ,
	BUF_GET, BUF_ਪਾਓ, BUF_ਆਖਰੀ, BUF_MARK, BUF_NAME, BUF_ਸੰਖਿਆ,
	BUF_OPTION, BUF_ਸੈੱਟ, BUF_WINDOWS
    };
    ਸਥਿਰ CONST84 ਅੱਖਰ *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (ਅੱਖਰ *)0
    };

    ਜੇ (objc < 2)
    {
	Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 1, objv, "option ?arg ...?");
	ਵਾਪਸ TCL_ERROR;
    }

    err = Tcl_Get_ਸੂਚਕਾਂਕ_From_Obj(interp, objv[1], bufoptions, "option", 0, &idx);
    ਜੇ (err != TCL_OK)
	ਵਾਪਸ err;

    buf = (buf_T *)((ਬਣਤਰ ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    ਚੋਣ (idx)
    {
	ਹਾਲਤ BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_ਵਿਅੰਜਕ:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    ਜੇ (objc != 2)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, ਨਿਰਾ);
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    ਜੇ (buf->b_ffname)
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, (ਅੱਖਰ *)buf->b_ffname, TCL_VOLATILE);
	    ਨਹੀਂਤਾਂ
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, "", TCL_STATIC);
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_ਆਖਰੀ:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	ਹਾਲਤ BUF_ਗਿਣਤੀ:
	    /*
	     * Get number of lines in buffer.
	     */
	    ਜੇ (objc != 2)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, ਨਿਰਾ);
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    val1 = (ਪੂਰਨ)buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਜੇ (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, resobj);
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_ਸੰਖਿਆ:
	    /*
	     * Get buffer's number.
	     */
	    ਜੇ (objc != 2)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, ਨਿਰਾ);
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    resobj = Tcl_NewIntObj((ਪੂਰਨ)buf->b_fnum);
	    Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, resobj);
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_GET:
	    ਜੇ (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ਜੇ (err != TCL_OK)
		ਰੋਕੋ;
	    ਜੇ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ਜੇ (err != TCL_OK)
		    ਰੋਕੋ;
		ਜੇ (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_Reset_ਨਤੀਜਾ(interp);

		ਲਈ (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    ਸਤਰ = (ਅੱਖਰ *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    ਜੇ (ਸਤਰ)
			Tcl_AppendElement(interp, ਸਤਰ);
		    ਨਹੀਂਤਾਂ
			err = TCL_ERROR;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {  // objc == 3
		ਸਤਰ = (ਅੱਖਰ *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, ਸਤਰ, TCL_VOLATILE);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_ਸੈੱਟ:
	    ਜੇ (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ਜੇ (err != TCL_OK)
		ਵਾਪਸ TCL_ERROR;
	    ਜੇ (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		ਸਤਰ = Tcl_GetStringFromObj(objv[3], ਨਿਰਾ);
		ਜੇ (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ਨਹੀਂਤਾਂ
		ਜੇ (ml_replace((linenr_T)val1, (char_u *)ਸਤਰ, TRUE) != OK)
		{
		    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਬਦਲਿਆ_ਬਾਈਟਾਂ((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		ਪੂਰਨ	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ਜੇ (err != TCL_OK)
		    ਰੋਕੋ;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		ਜੇ (err != TCL_OK)
		    ਰੋਕੋ;
		ਜੇ (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		ਜੇ (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    ਰੋਕੋ;
		}
		flags |= FL_UPDATE_CURBUF;

		ਲਈ (i = 0; i < lc && n <= val2; i++)
		{
		    ਸਤਰ = Tcl_GetStringFromObj(lv[i], ਨਿਰਾ);
		    ਜੇ (ml_replace((linenr_T)n, (char_u *)ਸਤਰ, TRUE) != OK)
			ਜਾਓ ਸੈੱਟ_List_Error;
		    ++n;
		}
		ਜੇ (i < lc)
		{
		    // append lines
		    ਕਰੋ
		    {
			ਸਤਰ = Tcl_GetStringFromObj(lv[i], ਨਿਰਾ);
			ਜੇ (ml_append((linenr_T)(n - 1),
					      (char_u *)ਸਤਰ, 0, FALSE) != OK)
			    ਜਾਓ ਸੈੱਟ_List_Error;
			++n;
			++i;
		    } ਜਦੋਂ (i < lc);
		}
		ਨਹੀਂਤਾਂ ਜੇ (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    ਕਰੋ
		    {
			ਜੇ (ml_delete((linenr_T)i) != OK)
			    ਜਾਓ ਸੈੱਟ_List_Error;
			++n;
		    } ਜਦੋਂ (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (ਲੰਮਾ)MAXLNUM,
								    (ਲੰਮਾ)lc);
		ਬਦਲਿਆ_ਸਤਰਾਂ((linenr_T)val1, 0, (linenr_T)val2 + 1, (ਲੰਮਾ)lc);
		ਰੋਕੋ;
    ਸੈੱਟ_List_Error:
		u_undo(1);  // ???
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_DELETE:
	    ਜੇ (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ਜੇ (err != TCL_OK)
		ਰੋਕੋ;
	    val2 = val1;
	    ਜੇ (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ਜੇ (err != TCL_OK)
		    ਵਾਪਸ err;
		ਜੇ (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    ਜੇ (u_savedel((linenr_T)val1, (ਲੰਮਾ)n) != OK)
	    {
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    ਲਈ (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		ਜੇ (err != TCL_OK)
		    ਰੋਕੋ;
	    }
	    ਜੇ (i > 0)
		deleted_ਸਤਰਾਂ_mark((linenr_T)val1, (ਲੰਮਾ)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_MARK:
	    ਜੇ (objc != 3)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, "markName");
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    ਸਤਰ = Tcl_GetStringFromObj(objv[2], ਨਿਰਾ);

	    pos = ਨਿਰਾ;
	    ਜੇ (ਸਤਰ[0] != '\0'  &&  ਸਤਰ[1] == '\0')
		pos = getmark(ਸਤਰ[0], FALSE);
	    ਜੇ (pos == ਨਿਰਾ)
	    {
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    err = vimerror(interp);
	    ਜੇ (err != TCL_OK)
		ਰੋਕੋ;
	    ਜੇ (pos->lnum <= 0)
	    {
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਅੱਖਰ rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (ਪੂਰਨ)row2tcl(pos->lnum), (ਪੂਰਨ)col2tcl(pos->ਖਾਨਾ));
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, rbuf, TCL_VOLATILE);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_ਪਾਓ:
	    opt = 1;
	    // fallthrough
	ਹਾਲਤ BUF_APPEND:
	    ਜੇ (objc != 4)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ਜੇ (err != TCL_OK)
		ਰੋਕੋ;
	    ਜੇ (opt)
		--val1;
	    ਜੇ (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }

	    ਸਤਰ = Tcl_GetStringFromObj(objv[3], ਨਿਰਾ);
	    ਜੇ (ml_append((linenr_T)val1, (char_u *)ਸਤਰ, 0, FALSE) != OK)
	    {
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    appended_ਸਤਰਾਂ_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    ਜੇ (objc != 2)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, ਨਿਰਾ);
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    Tcl_Reset_ਨਤੀਜਾ(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		ਜੇ (win->w_ਬਫਰ == buf)
		{
		    ਸਤਰ = tclgetwindow(interp, win);
		    ਜੇ (ਸਤਰ != ਨਿਰਾ)
			Tcl_AppendElement(interp, ਸਤਰ);
		    ਨਹੀਂਤਾਂ
		    {
			err = TCL_ERROR;
			ਰੋਕੋ;
		    }
		}
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    ਜੇ (objc != 3)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, "command");
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (ਖਾਲੀ *)buf, objv[2]);
	    ਰੋਕੋ;

	ਮੂਲ:
	    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    ਜੇ (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    ਜੇ (flags & FL_ADJUST_CURSOR)
	check_cursor();
    ਜੇ (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    ਵਾਪਸ err;
}

/*
 * This function implements the window commands.
 */
    ਸਥਿਰ ਪੂਰਨ
winselfcmd(
    Client_ਅੰਕੜੇ	ref,
    Tcl_Interp	*interp,
    ਪੂਰਨ		objc,
    Tcl_Obj	*CONST objv[])
{
    ਪੂਰਨ		err, idx, flags;
    ਪੂਰਨ		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    ਅੱਖਰ	*str;

    ਗਿਣਤੀ
    {
	WIN_ਬਫਰ, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_ਵਿਅੰਜਕ,
	WIN_ਉਚਾਈ, WIN_OPTION
    };
    ਸਥਿਰ CONST84 ਅੱਖਰ *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (ਅੱਖਰ *)0
    };

    ਜੇ (objc < 2)
    {
	Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 1, objv, "option ?arg ...?");
	ਵਾਪਸ TCL_ERROR;
    }

    err = Tcl_Get_ਸੂਚਕਾਂਕ_From_Obj(interp, objv[1], winoptions, "option", 0,  &idx);
    ਜੇ (err != TCL_OK)
	ਵਾਪਸ TCL_ERROR;

    win = (win_T *)((ਬਣਤਰ ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_ਬਫਰ;
    flags = 0;

    ਚੋਣ (idx)
    {
	ਹਾਲਤ WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ਰੋਕੋ;

	ਹਾਲਤ WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ਰੋਕੋ;

	ਹਾਲਤ WIN_ਵਿਅੰਜਕ:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ਰੋਕੋ;

	ਹਾਲਤ WIN_ਉਚਾਈ:
	    ਜੇ (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		ਜੇ (err != TCL_OK)
		    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		win_setheight(val1);
		err = vimerror(interp);
		ਜੇ (err != TCL_OK)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ
	    ਜੇ (objc != 2)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }

	    resobj = Tcl_NewIntObj((ਪੂਰਨ)(win->w_ਉਚਾਈ));
	    Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, resobj);
	    ਰੋਕੋ;

	ਹਾਲਤ WIN_ਬਫਰ:
	    ਜੇ (objc != 2)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, ਨਿਰਾ);
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    str = tclgetbuffer(interp, win->w_ਬਫਰ);
	    ਜੇ (str)
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, str, TCL_VOLATILE);
	    ਨਹੀਂਤਾਂ
		err = TCL_ERROR;
	    ਰੋਕੋ;

	ਹਾਲਤ WIN_DELCMD:
	    ਜੇ (objc != 3)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, "command");
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (ਖਾਲੀ *)win, objv[2]);
	    ਰੋਕੋ;

	ਹਾਲਤ WIN_CURSOR:
	    ਜੇ (objc > 4)
	    {
		Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    ਜੇ (objc == 2)
	    {
		ਅੱਖਰ buf[64];

		sprintf(buf, _("row %d column %d"), (ਪੂਰਨ)row2tcl(win->w_cursor.lnum), (ਪੂਰਨ)col2tcl(win->w_cursor.ਖਾਨਾ));
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, buf, TCL_VOLATILE);
		ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_ਸੁਨੇਹਾ);
		ਜੇ (var == ਨਿਰਾ)
		{
		    err = TCL_ERROR;
		    ਰੋਕੋ;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_ਬਫਰ);
		ਜੇ (err != TCL_OK)
		    ਰੋਕੋ;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_ਸੁਨੇਹਾ);
		ਜੇ (var == ਨਿਰਾ)
		{
		    err = TCL_ERROR;
		    ਰੋਕੋ;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		ਜੇ (err != TCL_OK)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_ਬਫਰ);
		ਜੇ (err != TCL_OK)
		    ਰੋਕੋ;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		ਜੇ (err != TCL_OK)
		    ਰੋਕੋ;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.ਖਾਨਾ = col2vim(val2);
	    win->w_ਸੈੱਟ_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    ਰੋਕੋ;

	ਮੂਲ:
	    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("not implemented yet"), TCL_STATIC);
	    ਰੋਕੋ;
    }

    curwin = savewin;
    curbuf = savebuf;
    ਜੇ (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    ਵਾਪਸ err;
}


    ਸਥਿਰ ਪੂਰਨ
commandcmd(
    Client_ਅੰਕੜੇ	dummy ਅਣਵਰਤਿਆ,
    Tcl_Interp	*interp,
    ਪੂਰਨ		objc,
    Tcl_Obj	*CONST objv[])
{
    ਪੂਰਨ		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ਵਾਪਸ err;
}

    ਸਥਿਰ ਪੂਰਨ
optioncmd(
    Client_ਅੰਕੜੇ	dummy ਅਣਵਰਤਿਆ,
    Tcl_Interp	*interp,
    ਪੂਰਨ		objc,
    Tcl_Obj	*CONST objv[])
{
    ਪੂਰਨ		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ਵਾਪਸ err;
}

    ਸਥਿਰ ਪੂਰਨ
exprcmd(
    Client_ਅੰਕੜੇ	dummy ਅਣਵਰਤਿਆ,
    Tcl_Interp	*interp,
    ਪੂਰਨ		objc,
    Tcl_Obj	*CONST objv[])
{
    ਵਾਪਸ tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    ਸਥਿਰ ਪੂਰਨ
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    ਪੂਰਨ		*ਮੁੱਲ_P,
    buf_T	*buf)
{
    ਪੂਰਨ err, i;

    ਗਿਣਤੀ { LN_BEGIN, LN_BOTTOM, LN_ਸਿਰਾ, LN_ਪਹਿਲਾ, LN_ਆਖਰੀ, LN_ਸ਼ੁਰੂ, LN_ਸਿਖਰ };

    ਸਥਿਰ CONST84 ਅੱਖਰ *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (ਅੱਖਰ *)0
    };

    err = Tcl_Get_ਸੂਚਕਾਂਕ_From_Obj(interp, obj, keyw, "", 0, &i);
    ਜੇ (err == TCL_OK)
    {
	ਚੋਣ (i)
	{
	    ਹਾਲਤ LN_BEGIN:
	    ਹਾਲਤ LN_ਪਹਿਲਾ:
	    ਹਾਲਤ LN_ਸ਼ੁਰੂ:
	    ਹਾਲਤ LN_ਸਿਖਰ:
		*ਮੁੱਲ_P = 1;
		ਰੋਕੋ;
	    ਹਾਲਤ LN_BOTTOM:
	    ਹਾਲਤ LN_ਸਿਰਾ:
	    ਹਾਲਤ LN_ਆਖਰੀ:
		*ਮੁੱਲ_P = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		ਰੋਕੋ;
	}
	ਵਾਪਸ TCL_OK;
    }
    Tcl_Reset_ਨਤੀਜਾ(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    ਜੇ (err != TCL_OK)
	ਵਾਪਸ err;
    i = row2vim(i);
    ਜੇ (i < 1  ||  i > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("line number out of range"), TCL_STATIC);
	ਵਾਪਸ TCL_ERROR;
    }
    *ਮੁੱਲ_P = i;
    ਵਾਪਸ TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    ਸਥਿਰ win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	ਜੇ (win->w_ਬਫਰ == buf)
	    ਵਾਪਸ win;
    }
    ਵਾਪਸ curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    ਸਥਿਰ ਪੂਰਨ
tcldoexcommand(
    Tcl_Interp	*interp,
    ਪੂਰਨ		objc,
    Tcl_Obj	*CONST objv[],
    ਪੂਰਨ		objn)
{
    tcl_info	saveinfo;
    ਪੂਰਨ		err, flag, nobjs;
    ਅੱਖਰ	*arg;

    nobjs = objc - objn;
    ਜੇ (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, objn, objv, "?-quiet? exCommand");
	ਵਾਪਸ TCL_ERROR;
    }

    flag = 0;
    ਜੇ (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], ਨਿਰਾ);
	ਜੇ (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	ਨਹੀਂਤਾਂ
	{
	    Tcl_Reset_ਨਤੀਜਾ(interp);
	    Tcl_Append_ਨਤੀਜਾ(interp, _("unknown flag: "), arg, (ਅੱਖਰ *)0);
	    ਵਾਪਸ TCL_ERROR;
	}
	++objn;
    }

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&saveinfo, &tclinfo, ਆਕਾਰ(tcl_info));
    tclinfo.interp = ਨਿਰਾ;
    tclinfo.curwin = ਨਿਰਾ;
    tclinfo.curbuf = ਨਿਰਾ;

    arg = Tcl_GetStringFromObj(objv[objn], ਨਿਰਾ);
    ਜੇ (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    ਜੇ (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    ਜੇ (tclinfo.interp)
	tcldelthisinterp();
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&tclinfo, &saveinfo, ਆਕਾਰ(tcl_info));
    tclupdatevars();

    ਵਾਪਸ err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    ਸਥਿਰ ਪੂਰਨ
tclsetoption(
    Tcl_Interp	*interp,
    ਪੂਰਨ		objc,
    Tcl_Obj	*CONST objv[],
    ਪੂਰਨ		objn)
{
    ਪੂਰਨ		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    ਲੰਮਾ	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    ਗਿਣਤੀ { OPT_OFF, OPT_ON, OPT_TOGGLE };
    ਸਥਿਰ CONST84 ਅੱਖਰ *optkw[] = { "off", "on", "toggle", (ਅੱਖਰ *)0 };

    nobjs = objc - objn;
    ਜੇ (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, objn, objv, "vimOption ?value?");
	ਵਾਪਸ TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], ਨਿਰਾ);
    ++objn;
    gov = get_option_ਮੁੱਲ(option, &lval, &sval, ਨਿਰਾ, 0);
    err = TCL_OK;
    ਚੋਣ (gov)
    {
	ਹਾਲਤ gov_string:
	    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, (ਅੱਖਰ *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    ਰੋਕੋ;
	ਹਾਲਤ gov_bool:
	ਹਾਲਤ gov_ਸੰਖਿਆ:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, resobj);
	    ਰੋਕੋ;
	ਮੂਲ:
	    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("unknown vimOption"), TCL_STATIC);
	    ਵਾਪਸ TCL_ERROR;
    }
    ਜੇ (nobjs == 2)
    {
	ਜੇ (gov != gov_string)
	{
	    sval = ਨਿਰਾ;    // avoid compiler warning
	    err = Tcl_Get_ਸੂਚਕਾਂਕ_From_Obj(interp, objv[objn], optkw, "", 0, &idx);
	    ਜੇ (err != TCL_OK)
	    {
		Tcl_Reset_ਨਤੀਜਾ(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਚੋਣ (idx)
		{
		    ਹਾਲਤ OPT_ON:
			lval = 1;
			ਰੋਕੋ;
		    ਹਾਲਤ OPT_OFF:
			lval = 0;
			ਰੋਕੋ;
		    ਹਾਲਤ OPT_TOGGLE:
			lval = !lval;
			ਰੋਕੋ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], ਨਿਰਾ);
	ਜੇ (err == TCL_OK)
	{
	    ਸੈੱਟ_option_ਮੁੱਲ_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    ਵਾਪਸ err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    ਸਥਿਰ ਪੂਰਨ
tclvimexpr(
    Tcl_Interp	*interp,
    ਪੂਰਨ		objc,
    Tcl_Obj	*CONST objv[],
    ਪੂਰਨ		objn)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਅੱਖਰ	*ਵਿਅੰਜਕ, *str;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		err;

    ਜੇ (objc - objn != 1)
    {
	Tcl_Wrong_ਅੰਕ_ਦਲੀਲਾਂ(interp, objn, objv, "vimExpr");
	ਵਾਪਸ TCL_ERROR;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਵਿਅੰਜਕ = Tcl_GetStringFromObj(objv[objn], ਨਿਰਾ);
    str = (ਅੱਖਰ *)eval_ਮੰਜ਼ਿਲ_string((char_u *)ਵਿਅੰਜਕ, TRUE, FALSE);
    ਜੇ (str == ਨਿਰਾ)
	Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("invalid expression"), TCL_STATIC);
    ਨਹੀਂਤਾਂ
    {
	Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#ਨਹੀਂਤਾਂ
    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ err;
}

/*
 * Check for internal vim errors.
 */
    ਸਥਿਰ ਪੂਰਨ
vimerror(Tcl_Interp *interp)
{
    ਜੇ (got_int)
    {
	Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("keyboard interrupt"), TCL_STATIC);
	ਵਾਪਸ TCL_ERROR;
    }
    ਨਹੀਂਤਾਂ ਜੇ (did_emsg)
    {
	Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("Vim error"), TCL_STATIC);
	ਵਾਪਸ TCL_ERROR;
    }
    ਵਾਪਸ TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    ਸਥਿਰ ਖਾਲੀ
delref(Client_ਅੰਕੜੇ cref)
{
    ਬਣਤਰ ref *ref = (ਬਣਤਰ ref *)cref;

    ਜੇ (ref->delcmd)
    {
	Tcl_Decr_Ref_ਗਿਣਤੀ(ref->delcmd);
	ref->delcmd = ਨਿਰਾ;
    }
    ref->interp = ਨਿਰਾ;
}

    ਸਥਿਰ ਅੱਖਰ *
tclgetref(
    Tcl_Interp	*interp,
    ਖਾਲੀ	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    ਅੱਖਰ	*ਅਗੇਤਰ,	// "win" or "buf"
    ਖਾਲੀ	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    ਬਣਤਰ ref *ref, *ਅਣਵਰਤਿਆ_2 = ਨਿਰਾ;
    ਸਥਿਰ ਅੱਖਰ name[VARNAME_ਮਾਪ];
    Tcl_Command cmd;

    ref = (ਬਣਤਰ ref *)(*refstartP);
    ਜੇ (ref == &refsdeleted)
    {
	Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜਦੋਂ (ref != ਨਿਰਾ)
    {
	ਜੇ (ref->interp == interp)
	    ਰੋਕੋ;
	ਜੇ (ref->interp == ਨਿਰਾ)
	    ਅਣਵਰਤਿਆ_2 = ref;
	ref = ref->ਅਗਲਾ;
    }

    ਜੇ (ref)
	vim_snprintf(name, ਆਕਾਰ(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਅਣਵਰਤਿਆ_2)
	    ref = ਅਣਵਰਤਿਆ_2;
	ਨਹੀਂਤਾਂ
	{
	    ref = (ਬਣਤਰ ref *)Tcl_Alloc(ਆਕਾਰ(ਬਣਤਰ ref));
	    ref->interp = ਨਿਰਾ;
	    ref->ਅਗਲਾ = (ਬਣਤਰ ref *)(*refstartP);
	    (*refstartP) = (ਖਾਲੀ *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, ਆਕਾਰ(name), "::vim::%s_%lx",
					       ਅਗੇਤਰ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (Client_ਅੰਕੜੇ)ref, (Tcl_CmdDeleteProc *)delref);
	ਜੇ (!cmd)
	    ਵਾਪਸ ਨਿਰਾ;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = ਨਿਰਾ;
	ref->vimobj = vimobj;
    }
    ਵਾਪਸ name;
}

    ਸਥਿਰ ਅੱਖਰ *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    ਵਾਪਸ tclgetref(interp, &(win->w_tcl_ref), "win", (ਖਾਲੀ *)win, winselfcmd);
}

    ਸਥਿਰ ਅੱਖਰ *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    ਵਾਪਸ tclgetref(interp, &(buf->b_tcl_ref), "buf", (ਖਾਲੀ *)buf, bufselfcmd);
}

    ਸਥਿਰ ਪੂਰਨ
tclsetdelcmd(
    Tcl_Interp	*interp,
    ਬਣਤਰ ref	*reflist,
    ਖਾਲੀ	*vimobj,
    Tcl_Obj	*delcmd)
{
    ਜੇ (reflist == &refsdeleted)
    {
	Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	ਵਾਪਸ TCL_ERROR;
    }

    ਜਦੋਂ (reflist != ਨਿਰਾ)
    {
	ਜੇ (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    ਜੇ (reflist->delcmd)
		Tcl_Decr_Ref_ਗਿਣਤੀ(reflist->delcmd);
	    Tcl_Incr_Ref_ਗਿਣਤੀ(delcmd);
	    reflist->delcmd = delcmd;
	    ਵਾਪਸ TCL_OK;
	}
	reflist = reflist->ਅਗਲਾ;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    ਵਾਪਸ TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    ਸਥਿਰ ਪੂਰਨ
tcl_channel_close(Client_ਅੰਕੜੇ instance, Tcl_Interp *interp ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		err = 0;

    // currently does nothing

    ਜੇ (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_ਸੈੱਟ_Errno(EBADF);
	err = EBADF;
    }
    ਵਾਪਸ err;
}

    ਸਥਿਰ ਪੂਰਨ
tcl_channel_ਨਿਵੇਸ਼(
    Client_ਅੰਕੜੇ	instance ਅਣਵਰਤਿਆ,
    ਅੱਖਰ	*buf ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		bufsiz ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		*errptr)
{

    // input is currently not supported

    Tcl_ਸੈੱਟ_Errno(EINVAL);
    ਜੇ (errptr)
	*errptr = EINVAL;
    ਵਾਪਸ -1;
}

    ਸਥਿਰ ਪੂਰਨ
tcl_channel_output(
    Client_ਅੰਕੜੇ	instance,
    ਅਚਲ ਅੱਖਰ	*buf,
    ਪੂਰਨ		bufsiz,
    ਪੂਰਨ		*errptr)
{
    char_u	*str;
    ਪੂਰਨ		ਨਤੀਜਾ;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    ਜੇ (!str)
    {
	Tcl_ਸੈੱਟ_Errno(ENOMEM);
	ਜੇ (errptr)
	    *errptr = ENOMEM;
	ਵਾਪਸ -1;
    }

    ਨਤੀਜਾ = bufsiz;
    ਜੇ (instance == VIMOUT)
	tclmsg((ਅੱਖਰ *)str);
    ਨਹੀਂਤਾਂ
    ਜੇ (instance == VIMERR)
	tclerrmsg((ਅੱਖਰ *)str);
    ਨਹੀਂਤਾਂ
    {
	Tcl_ਸੈੱਟ_Errno(EBADF);
	ਜੇ (errptr)
	    *errptr = EBADF;
	ਨਤੀਜਾ = -1;
    }
    vim_free(str);
    ਵਾਪਸ ਨਤੀਜਾ;
}

    ਸਥਿਰ ਖਾਲੀ
tcl_channel_watch(Client_ਅੰਕੜੇ instance ਅਣਵਰਤਿਆ, ਪੂਰਨ ਨਕਾਬ ਅਣਵਰਤਿਆ)
{
    Tcl_ਸੈੱਟ_Errno(EINVAL);
}

    ਸਥਿਰ ਪੂਰਨ
tcl_channel_gethandle(
    Client_ਅੰਕੜੇ	instance ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		direction ਅਣਵਰਤਿਆ,
    Client_ਅੰਕੜੇ	*handleptr ਅਣਵਰਤਿਆ)
{
    Tcl_ਸੈੱਟ_Errno(EINVAL);
    ਵਾਪਸ EINVAL;
}


ਸਥਿਰ Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_ਸੰਸਕਰਣ_2, //ਸੰਸਕਰਣ
    tcl_channel_close,	// closeProc
    tcl_channel_ਨਿਵੇਸ਼,	// inputProc
    tcl_channel_output,	// outputProc
    ਨਿਰਾ,		// seekProc
    ਨਿਰਾ,		// setOptionProc
    ਨਿਰਾ,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    ਨਿਰਾ,		// close2Proc
    ਨਿਰਾ,		// blockModeProc
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TCL_CHANNEL_ਸੰਸਕਰਣ_2
    ਨਿਰਾ,		// flushProc
    ਨਿਰਾ,		// handlerProc
#ਜੇ_ਅੰਤ
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TCL_CHANNEL_ਸੰਸਕਰਣ_3
    ਨਿਰਾ,		// wideSeekProc
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TCL_CHANNEL_ਸੰਸਕਰਣ_4
    ਨਿਰਾ,		// threadActionProc
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TCL_CHANNEL_ਸੰਸਕਰਣ_5
    ਨਿਰਾ		// truncateProc
#ਜੇ_ਅੰਤ
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    ਸਥਿਰ ਖਾਲੀ
tclupdatevars(ਖਾਲੀ)
{
    ਅੱਖਰ varname[VARNAME_ਮਾਪ];	// must be writeable
    ਅੱਖਰ *name;

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


    ਸਥਿਰ ਪੂਰਨ
tclinit(exarg_T *eap)
{
    ਅੱਖਰ varname[VARNAME_ਮਾਪ];	// Tcl_LinkVar requires writeable varname
    ਅੱਖਰ *name;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_TCL
    ਜੇ (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	ਵਾਪਸ FAIL;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	ਸਥਿਰ Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_ਸੈੱਟ_Std_Channel(ch1, TCL_STDOUT);
	Tcl_ਸੈੱਟ_Std_Channel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	ਜੇ (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    ਵਾਪਸ FAIL;
	}
#ਜੇ 0
	// VIM sure is interactive
	Tcl_ਸੈੱਟ_Var(interp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
#ਜੇ_ਅੰਤ

	Tcl_ਸੈੱਟ_Channel_Option(interp, ch1, "-buffering", "line");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	Tcl_ਸੈੱਟ_Channel_Option(interp, ch1, "-translation", "lf");
#ਜੇ_ਅੰਤ
	Tcl_ਸੈੱਟ_Channel_Option(interp, ch2, "-buffering", "line");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	Tcl_ਸੈੱਟ_Channel_Option(interp, ch2, "-translation", "lf");
#ਜੇ_ਅੰਤ

	// replace standard Tcl exit command
	Tcl_DeleteCommand(interp, "exit");
	Tcl_CreateObjCommand(interp, "exit", exitcmd,
	    (Client_ਅੰਕੜੇ)ਨਿਰਾ, (Tcl_CmdDeleteProc *)ਨਿਰਾ);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (Client_ਅੰਕੜੇ)ਨਿਰਾ, (Tcl_CmdDeleteProc *)ਨਿਰਾ);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (Client_ਅੰਕੜੇ)ਨਿਰਾ, (Tcl_CmdDeleteProc *)ਨਿਰਾ);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (Client_ਅੰਕੜੇ)ਨਿਰਾ, (Tcl_CmdDeleteProc *)ਨਿਰਾ);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (Client_ਅੰਕੜੇ)ਨਿਰਾ, (Tcl_CmdDeleteProc *)ਨਿਰਾ);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (Client_ਅੰਕੜੇ)ਨਿਰਾ, (Tcl_CmdDeleteProc *)ਨਿਰਾ);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (Client_ਅੰਕੜੇ)ਨਿਰਾ, (Tcl_CmdDeleteProc *)ਨਿਰਾ);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (ਅੱਖਰ *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_ਸ਼ੁਰੂ = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (ਅੱਖਰ *)&tclinfo.range_ਸ਼ੁਰੂ, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (ਅੱਖਰ *)&tclinfo.range_ਸ਼ੁਰੂ, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_ਸਿਰਾ   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (ਅੱਖਰ *)&tclinfo.range_ਸਿਰਾ, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_ਮਾਪ);
	tclinfo.curwin = Tcl_Alloc(VARNAME_ਮਾਪ);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (ਅੱਖਰ *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (ਅੱਖਰ *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    ਨਹੀਂਤਾਂ
    {
	// Interpreter already exists, just update variables
	tclinfo.range_ਸ਼ੁਰੂ = row2tcl(eap->line1);
	tclinfo.range_ਸਿਰਾ = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਖਾਲੀ
tclerrmsg(ਅੱਖਰ *ਪਾਠ)
{
    ਅੱਖਰ *ਅਗਲਾ;

    ਜਦੋਂ ((ਅਗਲਾ=strchr(ਪਾਠ, '\n')))
    {
	*ਅਗਲਾ++ = '\0';
	emsg(ਪਾਠ);
	ਪਾਠ = ਅਗਲਾ;
    }
    ਜੇ (*ਪਾਠ)
	emsg(ਪਾਠ);
}

    ਸਥਿਰ ਖਾਲੀ
tclmsg(ਅੱਖਰ *ਪਾਠ)
{
    ਅੱਖਰ *ਅਗਲਾ;

    ਜਦੋਂ ((ਅਗਲਾ=strchr(ਪਾਠ, '\n')))
    {
	*ਅਗਲਾ++ = '\0';
	ਸੁਨੇਹਾ(ਪਾਠ);
	ਪਾਠ = ਅਗਲਾ;
    }
    ਜੇ (*ਪਾਠ)
	ਸੁਨੇਹਾ(ਪਾਠ);
}

    ਸਥਿਰ ਖਾਲੀ
tcldelthisinterp(ਖਾਲੀ)
{
    ਜੇ (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = ਨਿਰਾ;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = ਨਿਰਾ;
}

    ਸਥਿਰ ਪੂਰਨ
tclexit(ਪੂਰਨ error)
{
    ਪੂਰਨ newerr = OK;

    ਜੇ (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#ਜੇ (TCL_MAJOR_ਸੰਸਕਰਣ == 8 && TCL_MINOR_ਸੰਸਕਰਣ >= 5) || TCL_MAJOR_ਸੰਸਕਰਣ > 8
	|| Tcl_ਹੱਦਬੰਦੀ_Exceeded(tclinfo.interp)  // True if the interpreter cannot continue
#ਜੇ_ਅੰਤ
	)
    {
	ਅੱਖਰ buf[50];

	sprintf(buf, _(e_exit_ਕੋਡ_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	ਜੇ (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	ਨਹੀਂਤਾਂ
	    newerr = FAIL;

	tcldelthisinterp();
    }
    ਨਹੀਂਤਾਂ
    {
	ਅੱਖਰ *ਨਤੀਜਾ;

	ਨਤੀਜਾ = (ਅੱਖਰ *)Tcl_Get_String_ਨਤੀਜਾ(tclinfo.interp);
	ਜੇ (error == TCL_OK)
	{
	    tclmsg(ਨਤੀਜਾ);
	    newerr = OK;
	}
	ਨਹੀਂਤਾਂ
	{
	    tclerrmsg(ਨਤੀਜਾ);
	    newerr = FAIL;
	}
    }

    ਵਾਪਸ newerr;
}

/*
 * ":tcl"
 */
    ਖਾਲੀ
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    ਪੂਰਨ		err;

    script = script_get(eap, eap->arg);
    ਜੇ (!eap->skip)
    {
	err = tclinit(eap);
	ਜੇ (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    ਜੇ (script == ਨਿਰਾ)
		err = Tcl_Eval(tclinfo.interp, (ਅੱਖਰ *)eap->arg);
	    ਨਹੀਂਤਾਂ
		err = Tcl_Eval(tclinfo.interp, (ਅੱਖਰ *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    ਖਾਲੀ
ex_tclfile(exarg_T *eap)
{
    ਅੱਖਰ *file = (ਅੱਖਰ *)eap->arg;
    ਪੂਰਨ err;

    err = tclinit(eap);
    ਜੇ (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    ਖਾਲੀ
ex_tcldo(exarg_T *eap)
{
    ਅੱਖਰ	*script, *ਸਤਰ;
    ਪੂਰਨ		err, rs, re, lnum;
    ਅੱਖਰ	var_lnum[VARNAME_ਮਾਪ]; // must be writeable memory
    ਅੱਖਰ	var_ਸਤਰ[VARNAME_ਮਾਪ];
    linenr_T	ਪਹਿਲਾ_ਸਤਰ = 0;
    linenr_T	ਆਖਰੀ_ਸਤਰ = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (ਅੱਖਰ *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_ਸਤਰ, VAR_CURLINE);

    err = tclinit(eap);
    ਜੇ (err != OK)
	ਵਾਪਸ;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (ਅੱਖਰ *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    ਜੇ (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_ਸੈੱਟ_ਨਤੀਜਾ(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    ਜਦੋਂ (err == TCL_OK  &&  rs <= re)
    {
	ਜੇ ((linenr_T)rs > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਰੋਕੋ;
	ਸਤਰ = (ਅੱਖਰ *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	ਜੇ (!ਸਤਰ)
	{
	    Tcl_ਸੈੱਟ_ਨਤੀਜਾ(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    ਰੋਕੋ;
	}
	Tcl_ਸੈੱਟ_Var(tclinfo.interp, var_ਸਤਰ, ਸਤਰ, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	ਜੇ (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#ਜੇ (TCL_MAJOR_ਸੰਸਕਰਣ == 8 && TCL_MINOR_ਸੰਸਕਰਣ >= 5) || TCL_MAJOR_ਸੰਸਕਰਣ > 8
	    || Tcl_ਹੱਦਬੰਦੀ_Exceeded(tclinfo.interp)
#ਜੇ_ਅੰਤ
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਰੋਕੋ;
	ਸਤਰ = (ਅੱਖਰ *)Tcl_GetVar(tclinfo.interp, var_ਸਤਰ, 0);
	ਜੇ (ਸਤਰ)
	{
	    ਜੇ (ml_replace((linenr_T)rs, (char_u *)ਸਤਰ, TRUE) != OK)
	    {
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		ਰੋਕੋ;
	    }
	    ਜੇ (ਪਹਿਲਾ_ਸਤਰ == 0)
		ਪਹਿਲਾ_ਸਤਰ = rs;
	    ਆਖਰੀ_ਸਤਰ = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    ਜੇ (ਪਹਿਲਾ_ਸਤਰ)
	ਬਦਲਿਆ_ਸਤਰਾਂ(ਪਹਿਲਾ_ਸਤਰ, 0, ਆਖਰੀ_ਸਤਰ + 1, (ਲੰਮਾ)0);

    Tcl_UnsetVar(tclinfo.interp, var_ਸਤਰ, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    ਜੇ (err == TCL_OK)
	Tcl_Reset_ਨਤੀਜਾ(tclinfo.interp);

    (ਖਾਲੀ)tclexit(err);
}

    ਸਥਿਰ ਖਾਲੀ
tcldelallrefs(ਬਣਤਰ ref *ref)
{
    ਬਣਤਰ ref	*ਅਗਲਾ;
    ਪੂਰਨ		err;
    ਅੱਖਰ	*ਨਤੀਜਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    ਜੇ (exiting)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    ਜਦੋਂ (ref != ਨਿਰਾ)
    {
	ਅਗਲਾ = ref->ਅਗਲਾ;
	ਜੇ (ref->interp)
	{
	    ਜੇ (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		ਜੇ (err != TCL_OK)
		{
		    ਨਤੀਜਾ = (ਅੱਖਰ *)Tcl_Get_String_ਨਤੀਜਾ(ref->interp);
		    ਜੇ (ਨਤੀਜਾ)
			tclerrmsg(ਨਤੀਜਾ);
		}
		Tcl_Decr_Ref_ਗਿਣਤੀ(ref->delcmd);
		ref->delcmd = ਨਿਰਾ;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((ਅੱਖਰ *)ref);
	ref = ਅਗਲਾ;
    }
}

    ਖਾਲੀ
tcl_ਬਫਰ_free(buf_T *buf)
{
    ਬਣਤਰ ref *reflist;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_TCL
    ਜੇ (!stubs_initialized)	// Not using Tcl, nothing to do.
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    reflist = (ਬਣਤਰ ref *)(buf->b_tcl_ref);
    ਜੇ (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (ਖਾਲੀ *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = ਨਿਰਾ;
    }
}

    ਖਾਲੀ
tcl_ਖਿੜਕੀ_free(win_T *win)
{
    ਬਣਤਰ ref *reflist;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_TCL
    ਜੇ (!stubs_initialized)	// Not using Tcl, nothing to do.
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    reflist = (ਬਣਤਰ ref*)(win->w_tcl_ref);
    ਜੇ (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (ਖਾਲੀ *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = ਨਿਰਾ;
    }
}

// The End
