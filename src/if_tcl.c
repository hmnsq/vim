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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#undef EXTERN			// tcl.h defines it too

#ifdef DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#endif

#ଅନ୍ତର୍ଭାଵ <tcl.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    Tcl_Interp *interp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା exitvalue;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା range_start, range_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lbase;
    ଅକ୍ଷର *curbuf, *curwin;
} tcl_info;

ସ୍ଥିର tcl_info tclinfo = { ଖାଲି, 0, 0, 0, 0, ଖାଲି, ଖାଲି };

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
ସଂରଚନା ref
{
    ସଂରଚନା ref	*next;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    ଶୂନ୍ୟ	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
ସ୍ଥିର ଅକ୍ଷର * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
ସ୍ଥିର ଅକ୍ଷର * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, ସଂରଚନା ref *reflist, ଶୂନ୍ୟ *vimobj, Tcl_Obj *delcmd));
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ମୂଲ୍ୟ_P, buf_T *buf));
ସ୍ଥିର win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା objc, Tcl_Obj *CONST objv[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା objn));
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା objc, Tcl_Obj *CONST objv[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା objn));
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା objc, Tcl_Obj *CONST objv[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା objn));
ସ୍ଥିର ଶୂନ୍ୟ tcldelthisinterp _ANSI_ARGS_ ((ଶୂନ୍ୟ));

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vimerror _ANSI_ARGS_((Tcl_Interp *interp));
ସ୍ଥିର ଶୂନ୍ୟ tclmsg _ANSI_ARGS_((ଅକ୍ଷର *text));
ସ୍ଥିର ଶୂନ୍ୟ tclerrmsg _ANSI_ARGS_((ଅକ୍ଷର *text));
ସ୍ଥିର ଶୂନ୍ୟ tclupdatevars _ANSI_ARGS_((ଶୂନ୍ୟ));

ସ୍ଥିର ସଂରଚନା ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#ଯଦି defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define ପ୍ରତୀକ_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# ଅନ୍ୟଥା
#  include <dlfcn.h>
#  define HANDLE ଶୂନ୍ୟ*
#  define TCL_PROC ଶୂନ୍ୟ*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define ପ୍ରତୀକ_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
ସ୍ଥିର HANDLE hTclLib = ଖାଲି;
Tcl_Interp* (*dll_Tcl_CreateInterp)(ଶୂନ୍ୟ);
ଶୂନ୍ୟ (*dll_Tcl_FindExecutable)(ଅଚଳ ଶୂନ୍ୟ *);

/*
 * Table of name to function pointer of tcl.
 */
ସ୍ଥିର ସଂରଚନା {
    ଅକ୍ଷର* name;
    TCL_PROC* ptr;
} tcl_funcname_table[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_FindExecutable},
    {ଖାଲି, ଖାଲି},
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tcl_runtime_link_init(ଅକ୍ଷର *libname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (hTclLib)
	ଫେରନ୍ତୁ OK;
    ଯଦି (!(hTclLib = load_dll(libname)))
    {
	ଯଦି (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ଫେରନ୍ତୁ FAIL;
    }
    ପାଇଁ (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	ଯଦି (!(*tcl_funcname_table[i].ptr = ପ୍ରତୀକ_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = ଖାଲି;
	    ଯଦି (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଫେରନ୍ତୁ OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
ସ୍ଥିର ଅକ୍ଷର *find_executable_arg = ଖାଲି;
#endif

    ଶୂନ୍ୟ
vim_tcl_init(ଅକ୍ଷର *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#ଅନ୍ୟଥା
    Tcl_FindExecutable(arg);
#endif
}

#ଯଦି defined(DYNAMIC_TCL)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tcl_enabled(ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ଯଦି (!stubs_initialized && find_executable_arg != ଖାଲି
	    && tcl_runtime_link_init((ଅକ୍ଷର *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	ଯଦି ((interp = dll_Tcl_CreateInterp()) != ଖାଲି)
	{
	    ଯଦି (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != ଖାଲି)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    ଫେରନ୍ତୁ stubs_initialized;
}
#endif

#ଯଦି defined(EXITFREE)
/*
 * Called once when exiting.
 */
    ଶୂନ୍ୟ
vim_tcl_finalize(ଶୂନ୍ୟ)
{
# ifdef DYNAMIC_TCL
    ଯଦି (stubs_initialized)
# endif
	Tcl_Finalize();
}
#endif

    ଶୂନ୍ୟ
tcl_end(ଶୂନ୍ୟ)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା objc,
    Tcl_Obj *CONST objv[])
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ = 0;

    ବାଛ (objc)
    {
	ସ୍ଥିତି 2:
	    ଯଦି (Tcl_GetIntFromObj(interp, objv[1], &ମୂଲ୍ୟ) != TCL_OK)
		ଭାଙ୍ଗ;
	    // FALLTHROUGH
	ସ୍ଥିତି 1:
	    tclinfo.exitvalue = ମୂଲ୍ୟ;

	    Tcl_DeleteInterp(interp);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    Tcl_Wrong_ଅଙ୍କ_Args(interp, 1, objv, "?returnCode?");
    }
    ଫେରନ୍ତୁ TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା objc,
    Tcl_Obj *CONST objv[])
{
    ଯଦି (objc != 1)
    {
	Tcl_Wrong_ଅଙ୍କ_Args(interp, 1, objv, ଖାଲି);
	ଫେରନ୍ତୁ TCL_ERROR;
    }
    vim_beep(BO_LANG);
    ଫେରନ୍ତୁ TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା objc,
    Tcl_Obj *CONST objv[])
{
    ଅକ୍ଷର	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err, n, idx;
    ଗଣନା {BCMD_EXISTS, BCMD_LIST};
    ସ୍ଥିର CONST84 ଅକ୍ଷର *bcmdoptions[] =
    {
	"exists", "list", (ଅକ୍ଷର *)0
    };

    ଯଦି (objc < 2)
    {
	Tcl_Wrong_ଅଙ୍କ_Args(interp, 1, objv, "option");
	ଫେରନ୍ତୁ TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    ଯଦି (err == TCL_OK)
    {
	ଯଦି (objc != 2)
	{
	    Tcl_Wrong_ଅଙ୍କ_Args(interp, 1, objv, "bufNumber");
	    ଫେରନ୍ତୁ TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    ଯଦି (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		ଯଦି (name == ଖାଲି)
		    ଫେରନ୍ତୁ TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		ଫେରନ୍ତୁ TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	ଫେରନ୍ତୁ TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    ଯଦି (err != TCL_OK)
	ଫେରନ୍ତୁ err;
    ବାଛ (idx)
    {
	ସ୍ଥିତି BCMD_LIST:
	    ଯଦି (objc != 2)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, "");
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		ଯଦି (name == ଖାଲି)
		{
		    err = TCL_ERROR;
		    ଭାଙ୍ଗ;
		}
		Tcl_AppendElement(interp, name);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BCMD_EXISTS:
	    ଯଦି (objc != 3)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    ଯଦି (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != ଖାଲି);
		Tcl_SetObjResult(interp, resobj);
	    }
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    ଫେରନ୍ତୁ err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objc,
    Tcl_Obj	*CONST objv[])
{
    ଅକ୍ଷର	*what, *string;
    win_T	*win;

    ଯଦି (objc != 2)
    {
	Tcl_Wrong_ଅଙ୍କ_Args(interp, 1, objv, "option");
	ଫେରନ୍ତୁ TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], ଖାଲି);
    ଯଦି (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    ଯଦି (string == ଖାଲି)
		ଫେରନ୍ତୁ TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	ଫେରନ୍ତୁ TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    ଫେରନ୍ତୁ TCL_ERROR;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objc,
    Tcl_Obj	*CONST objv[])
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt, err, idx, flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    ଅକ୍ଷର	*ଧାଡ଼ି;

    ଗଣନା
    {
	BUF_APPEND, BUF_COMMAND, BUF_ଗଣନା, BUF_DELCMD, BUF_DELETE, BUF_ପଦସମଷ୍ଟି,
	BUF_GET, BUF_ଭର୍ତ୍ତି, BUF_LAST, BUF_MARK, BUF_NAME, BUF_ସଂଖ୍ୟା,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    ସ୍ଥିର CONST84 ଅକ୍ଷର *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (ଅକ୍ଷର *)0
    };

    ଯଦି (objc < 2)
    {
	Tcl_Wrong_ଅଙ୍କ_Args(interp, 1, objv, "option ?arg ...?");
	ଫେରନ୍ତୁ TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    ଯଦି (err != TCL_OK)
	ଫେରନ୍ତୁ err;

    buf = (buf_T *)((ସଂରଚନା ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    ବାଛ (idx)
    {
	ସ୍ଥିତି BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_ପଦସମଷ୍ଟି:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    ଯଦି (objc != 2)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, ଖାଲି);
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (buf->b_ffname)
		Tcl_SetResult(interp, (ଅକ୍ଷର *)buf->b_ffname, TCL_VOLATILE);
	    ଅନ୍ୟଥା
		Tcl_SetResult(interp, "", TCL_STATIC);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	ସ୍ଥିତି BUF_ଗଣନା:
	    /*
	     * Get number of lines in buffer.
	     */
	    ଯଦି (objc != 2)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, ଖାଲି);
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    val1 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    ଯଦି (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_ସଂଖ୍ୟା:
	    /*
	     * Get buffer's number.
	     */
	    ଯଦି (objc != 2)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, ଖାଲି);
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    resobj = Tcl_NewIntObj((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_GET:
	    ଯଦି (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ଯଦି (err != TCL_OK)
		ଭାଙ୍ଗ;
	    ଯଦି (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ଯଦି (err != TCL_OK)
		    ଭାଙ୍ଗ;
		ଯଦି (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		ପାଇଁ (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    ଧାଡ଼ି = (ଅକ୍ଷର *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    ଯଦି (ଧାଡ଼ି)
			Tcl_AppendElement(interp, ଧାଡ଼ି);
		    ଅନ୍ୟଥା
			err = TCL_ERROR;
		}
	    }
	    ଅନ୍ୟଥା
	    {  // objc == 3
		ଧାଡ଼ି = (ଅକ୍ଷର *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, ଧାଡ଼ି, TCL_VOLATILE);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_SET:
	    ଯଦି (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ଯଦି (err != TCL_OK)
		ଫେରନ୍ତୁ TCL_ERROR;
	    ଯଦି (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		ଧାଡ଼ି = Tcl_GetStringFromObj(objv[3], ଖାଲି);
		ଯଦି (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ଅନ୍ୟଥା
		ଯଦି (ml_replace((linenr_T)val1, (char_u *)ଧାଡ଼ି, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ଅନ୍ୟଥା
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ଯଦି (err != TCL_OK)
		    ଭାଙ୍ଗ;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		ଯଦି (err != TCL_OK)
		    ଭାଙ୍ଗ;
		ଯଦି (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		ଯଦି (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    ଭାଙ୍ଗ;
		}
		flags |= FL_UPDATE_CURBUF;

		ପାଇଁ (i = 0; i < lc && n <= val2; i++)
		{
		    ଧାଡ଼ି = Tcl_GetStringFromObj(lv[i], ଖାଲି);
		    ଯଦି (ml_replace((linenr_T)n, (char_u *)ଧାଡ଼ି, TRUE) != OK)
			ଯାଅ setListError;
		    ++n;
		}
		ଯଦି (i < lc)
		{
		    // append lines
		    କର
		    {
			ଧାଡ଼ି = Tcl_GetStringFromObj(lv[i], ଖାଲି);
			ଯଦି (ml_append((linenr_T)(n - 1),
					      (char_u *)ଧାଡ଼ି, 0, FALSE) != OK)
			    ଯାଅ setListError;
			++n;
			++i;
		    } ଯେତେବେଳେ (i < lc);
		}
		ଅନ୍ୟଥା ଯଦି (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    କର
		    {
			ଯଦି (ml_delete((linenr_T)i) != OK)
			    ଯାଅ setListError;
			++n;
		    } ଯେତେବେଳେ (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (long)MAXLNUM,
								    (long)lc);
		changed_ଧାଡ଼ିଗୁଡ଼ିକ((linenr_T)val1, 0, (linenr_T)val2 + 1, (long)lc);
		ଭାଙ୍ଗ;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_DELETE:
	    ଯଦି (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ଯଦି (err != TCL_OK)
		ଭାଙ୍ଗ;
	    val2 = val1;
	    ଯଦି (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		ଯଦି (err != TCL_OK)
		    ଫେରନ୍ତୁ err;
		ଯଦି (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    ଯଦି (u_savedel((linenr_T)val1, (long)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    ପାଇଁ (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		ଯଦି (err != TCL_OK)
		    ଭାଙ୍ଗ;
	    }
	    ଯଦି (i > 0)
		deleted_ଧାଡ଼ିଗୁଡ଼ିକ_mark((linenr_T)val1, (long)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_MARK:
	    ଯଦି (objc != 3)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, "markName");
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    ଧାଡ଼ି = Tcl_GetStringFromObj(objv[2], ଖାଲି);

	    pos = ଖାଲି;
	    ଯଦି (ଧାଡ଼ି[0] != '\0'  &&  ଧାଡ଼ି[1] == '\0')
		pos = getmark(ଧାଡ଼ି[0], FALSE);
	    ଯଦି (pos == ଖାଲି)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    err = vimerror(interp);
	    ଯଦି (err != TCL_OK)
		ଭାଙ୍ଗ;
	    ଯଦି (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ଅନ୍ୟଥା
	    {
		ଅକ୍ଷର rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)row2tcl(pos->lnum), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_ଭର୍ତ୍ତି:
	    opt = 1;
	    // fallthrough
	ସ୍ଥିତି BUF_APPEND:
	    ଯଦି (objc != 4)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    ଯଦି (err != TCL_OK)
		ଭାଙ୍ଗ;
	    ଯଦି (opt)
		--val1;
	    ଯଦି (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }

	    ଧାଡ଼ି = Tcl_GetStringFromObj(objv[3], ଖାଲି);
	    ଯଦି (ml_append((linenr_T)val1, (char_u *)ଧାଡ଼ି, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    ଯଦି (objc != 2)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, ଖାଲି);
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		ଯଦି (win->w_buffer == buf)
		{
		    ଧାଡ଼ି = tclgetwindow(interp, win);
		    ଯଦି (ଧାଡ଼ି != ଖାଲି)
			Tcl_AppendElement(interp, ଧାଡ଼ି);
		    ଅନ୍ୟଥା
		    {
			err = TCL_ERROR;
			ଭାଙ୍ଗ;
		    }
		}
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    ଯଦି (objc != 3)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (ଶୂନ୍ୟ *)buf, objv[2]);
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    ଯଦି (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    ଯଦି (flags & FL_ADJUST_CURSOR)
	check_cursor();
    ଯଦି (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    ଫେରନ୍ତୁ err;
}

/*
 * This function implements the window commands.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objc,
    Tcl_Obj	*CONST objv[])
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err, idx, flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    ଅକ୍ଷର	*str;

    ଗଣନା
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_ପଦସମଷ୍ଟି,
	WIN_HEIGHT, WIN_OPTION
    };
    ସ୍ଥିର CONST84 ଅକ୍ଷର *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (ଅକ୍ଷର *)0
    };

    ଯଦି (objc < 2)
    {
	Tcl_Wrong_ଅଙ୍କ_Args(interp, 1, objv, "option ?arg ...?");
	ଫେରନ୍ତୁ TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    ଯଦି (err != TCL_OK)
	ଫେରନ୍ତୁ TCL_ERROR;

    win = (win_T *)((ସଂରଚନା ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    ବାଛ (idx)
    {
	ସ୍ଥିତି WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି WIN_ପଦସମଷ୍ଟି:
	    err = tclvimexpr(interp, objc, objv, 2);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି WIN_HEIGHT:
	    ଯଦି (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		ଯଦି (err != TCL_OK)
		    ଭାଙ୍ଗ;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		ଯଦି (err != TCL_OK)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା
	    ଯଦି (objc != 2)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }

	    resobj = Tcl_NewIntObj((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି WIN_BUFFER:
	    ଯଦି (objc != 2)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, ଖାଲି);
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    ଯଦି (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    ଅନ୍ୟଥା
		err = TCL_ERROR;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି WIN_DELCMD:
	    ଯଦି (objc != 3)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, "command");
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (ଶୂନ୍ୟ *)win, objv[2]);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି WIN_CURSOR:
	    ଯଦି (objc > 4)
	    {
		Tcl_Wrong_ଅଙ୍କ_Args(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (objc == 2)
	    {
		ଅକ୍ଷର buf[64];

		sprintf(buf, _("row %d column %d"), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)row2tcl(win->w_cursor.lnum), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_ସନ୍ଦେଶ);
		ଯଦି (var == ଖାଲି)
		{
		    err = TCL_ERROR;
		    ଭାଙ୍ଗ;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		ଯଦି (err != TCL_OK)
		    ଭାଙ୍ଗ;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_ସନ୍ଦେଶ);
		ଯଦି (var == ଖାଲି)
		{
		    err = TCL_ERROR;
		    ଭାଙ୍ଗ;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		ଯଦି (err != TCL_OK)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		ଯଦି (err != TCL_OK)
		    ଭାଙ୍ଗ;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		ଯଦି (err != TCL_OK)
		    ଭାଙ୍ଗ;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    ଭାଙ୍ଗ;
    }

    curwin = savewin;
    curbuf = savebuf;
    ଯଦି (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    ଫେରନ୍ତୁ err;
}


    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objc,
    Tcl_Obj	*CONST objv[])
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ଫେରନ୍ତୁ err;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objc,
    Tcl_Obj	*CONST objv[])
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ଫେରନ୍ତୁ err;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objc,
    Tcl_Obj	*CONST objv[])
{
    ଫେରନ୍ତୁ tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ମୂଲ୍ୟ_P,
    buf_T	*buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା err, i;

    ଗଣନା { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_ଶୀର୍ଷ };

    ସ୍ଥିର CONST84 ଅକ୍ଷର *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (ଅକ୍ଷର *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    ଯଦି (err == TCL_OK)
    {
	ବାଛ (i)
	{
	    ସ୍ଥିତି LN_BEGIN:
	    ସ୍ଥିତି LN_FIRST:
	    ସ୍ଥିତି LN_START:
	    ସ୍ଥିତି LN_ଶୀର୍ଷ:
		*ମୂଲ୍ୟ_P = 1;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି LN_BOTTOM:
	    ସ୍ଥିତି LN_END:
	    ସ୍ଥିତି LN_LAST:
		*ମୂଲ୍ୟ_P = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
		ଭାଙ୍ଗ;
	}
	ଫେରନ୍ତୁ TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    ଯଦି (err != TCL_OK)
	ଫେରନ୍ତୁ err;
    i = row2vim(i);
    ଯଦି (i < 1  ||  i > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	ଫେରନ୍ତୁ TCL_ERROR;
    }
    *ମୂଲ୍ୟ_P = i;
    ଫେରନ୍ତୁ TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    ସ୍ଥିର win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	ଯଦି (win->w_buffer == buf)
	    ଫେରନ୍ତୁ win;
    }
    ଫେରନ୍ତୁ curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tcldoexcommand(
    Tcl_Interp	*interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objc,
    Tcl_Obj	*CONST objv[],
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objn)
{
    tcl_info	saveinfo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err, flag, nobjs;
    ଅକ୍ଷର	*arg;

    nobjs = objc - objn;
    ଯଦି (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_ଅଙ୍କ_Args(interp, objn, objv, "?-quiet? exCommand");
	ଫେରନ୍ତୁ TCL_ERROR;
    }

    flag = 0;
    ଯଦି (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], ଖାଲି);
	ଯଦି (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	ଅନ୍ୟଥା
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (ଅକ୍ଷର *)0);
	    ଫେରନ୍ତୁ TCL_ERROR;
	}
	++objn;
    }

    memcpy(&saveinfo, &tclinfo, ଆକାର(tcl_info));
    tclinfo.interp = ଖାଲି;
    tclinfo.curwin = ଖାଲି;
    tclinfo.curbuf = ଖାଲି;

    arg = Tcl_GetStringFromObj(objv[objn], ଖାଲି);
    ଯଦି (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    ଯଦି (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    ଯଦି (tclinfo.interp)
	tcldelthisinterp();
    memcpy(&tclinfo, &saveinfo, ଆକାର(tcl_info));
    tclupdatevars();

    ଫେରନ୍ତୁ err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tclsetoption(
    Tcl_Interp	*interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objc,
    Tcl_Obj	*CONST objv[],
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objn)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    long	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    ଗଣନା { OPT_OFF, OPT_ON, OPT_TOGGLE };
    ସ୍ଥିର CONST84 ଅକ୍ଷର *optkw[] = { "off", "on", "toggle", (ଅକ୍ଷର *)0 };

    nobjs = objc - objn;
    ଯଦି (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_ଅଙ୍କ_Args(interp, objn, objv, "vimOption ?value?");
	ଫେରନ୍ତୁ TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], ଖାଲି);
    ++objn;
    gov = get_option_ମୂଲ୍ୟ(option, &lval, &sval, ଖାଲି, 0);
    err = TCL_OK;
    ବାଛ (gov)
    {
	ସ୍ଥିତି gov_string:
	    Tcl_SetResult(interp, (ଅକ୍ଷର *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି gov_bool:
	ସ୍ଥିତି gov_ସଂଖ୍ୟା:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    ଫେରନ୍ତୁ TCL_ERROR;
    }
    ଯଦି (nobjs == 2)
    {
	ଯଦି (gov != gov_string)
	{
	    sval = ଖାଲି;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    ଯଦି (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    ଅନ୍ୟଥା
	    {
		ବାଛ (idx)
		{
		    ସ୍ଥିତି OPT_ON:
			lval = 1;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି OPT_OFF:
			lval = 0;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି OPT_TOGGLE:
			lval = !lval;
			ଭାଙ୍ଗ;
		}
	    }
	}
	ଅନ୍ୟଥା
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], ଖାଲି);
	ଯଦି (err == TCL_OK)
	{
	    set_option_ମୂଲ୍ୟ_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    ଫେରନ୍ତୁ err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tclvimexpr(
    Tcl_Interp	*interp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objc,
    Tcl_Obj	*CONST objv[],
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		objn)
{
#ifdef FEAT_EVAL
    ଅକ୍ଷର	*ପଦସମଷ୍ଟି, *str;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err;

    ଯଦି (objc - objn != 1)
    {
	Tcl_Wrong_ଅଙ୍କ_Args(interp, objn, objv, "vimExpr");
	ଫେରନ୍ତୁ TCL_ERROR;
    }

#ifdef FEAT_EVAL
    ପଦସମଷ୍ଟି = Tcl_GetStringFromObj(objv[objn], ଖାଲି);
    str = (ଅକ୍ଷର *)eval_to_string((char_u *)ପଦସମଷ୍ଟି, TRUE, FALSE);
    ଯଦି (str == ଖାଲି)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    ଅନ୍ୟଥା
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#ଅନ୍ୟଥା
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    ଫେରନ୍ତୁ err;
}

/*
 * Check for internal vim errors.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vimerror(Tcl_Interp *interp)
{
    ଯଦି (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	ଫେରନ୍ତୁ TCL_ERROR;
    }
    ଅନ୍ୟଥା ଯଦି (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	ଫେରନ୍ତୁ TCL_ERROR;
    }
    ଫେରନ୍ତୁ TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    ସ୍ଥିର ଶୂନ୍ୟ
delref(ClientData cref)
{
    ସଂରଚନା ref *ref = (ସଂରଚନା ref *)cref;

    ଯଦି (ref->delcmd)
    {
	Tcl_Decr_Ref_ଗଣନା(ref->delcmd);
	ref->delcmd = ଖାଲି;
    }
    ref->interp = ଖାଲି;
}

    ସ୍ଥିର ଅକ୍ଷର *
tclgetref(
    Tcl_Interp	*interp,
    ଶୂନ୍ୟ	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    ଅକ୍ଷର	*prefix,	// "win" or "buf"
    ଶୂନ୍ୟ	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    ସଂରଚନା ref *ref, *unused = ଖାଲି;
    ସ୍ଥିର ଅକ୍ଷର name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (ସଂରଚନା ref *)(*refstartP);
    ଯଦି (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯେତେବେଳେ (ref != ଖାଲି)
    {
	ଯଦି (ref->interp == interp)
	    ଭାଙ୍ଗ;
	ଯଦି (ref->interp == ଖାଲି)
	    unused = ref;
	ref = ref->next;
    }

    ଯଦି (ref)
	vim_snprintf(name, ଆକାର(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    ଅନ୍ୟଥା
    {
	ଯଦି (unused)
	    ref = unused;
	ଅନ୍ୟଥା
	{
	    ref = (ସଂରଚନା ref *)Tcl_Alloc(ଆକାର(ସଂରଚନା ref));
	    ref->interp = ଖାଲି;
	    ref->next = (ସଂରଚନା ref *)(*refstartP);
	    (*refstartP) = (ଶୂନ୍ୟ *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, ଆକାର(name), "::vim::%s_%lx",
					       prefix, (unsigned long)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	ଯଦି (!cmd)
	    ଫେରନ୍ତୁ ଖାଲି;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = ଖାଲି;
	ref->vimobj = vimobj;
    }
    ଫେରନ୍ତୁ name;
}

    ସ୍ଥିର ଅକ୍ଷର *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    ଫେରନ୍ତୁ tclgetref(interp, &(win->w_tcl_ref), "win", (ଶୂନ୍ୟ *)win, winselfcmd);
}

    ସ୍ଥିର ଅକ୍ଷର *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    ଫେରନ୍ତୁ tclgetref(interp, &(buf->b_tcl_ref), "buf", (ଶୂନ୍ୟ *)buf, bufselfcmd);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tclsetdelcmd(
    Tcl_Interp	*interp,
    ସଂରଚନା ref	*reflist,
    ଶୂନ୍ୟ	*vimobj,
    Tcl_Obj	*delcmd)
{
    ଯଦି (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	ଫେରନ୍ତୁ TCL_ERROR;
    }

    ଯେତେବେଳେ (reflist != ଖାଲି)
    {
	ଯଦି (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    ଯଦି (reflist->delcmd)
		Tcl_Decr_Ref_ଗଣନା(reflist->delcmd);
	    Tcl_Incr_Ref_ଗଣନା(delcmd);
	    reflist->delcmd = delcmd;
	    ଫେରନ୍ତୁ TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    ଫେରନ୍ତୁ TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err = 0;

    // currently does nothing

    ଯଦି (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    ଫେରନ୍ତୁ err;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tcl_channel_ନିବେଶ(
    ClientData	instance UNUSED,
    ଅକ୍ଷର	*buf UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufsiz UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    ଯଦି (errptr)
	*errptr = EINVAL;
    ଫେରନ୍ତୁ -1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tcl_channel_output(
    ClientData	instance,
    ଅଚଳ ଅକ୍ଷର	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufsiz,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*errptr)
{
    char_u	*str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    ଯଦି (!str)
    {
	Tcl_SetErrno(ENOMEM);
	ଯଦି (errptr)
	    *errptr = ENOMEM;
	ଫେରନ୍ତୁ -1;
    }

    result = bufsiz;
    ଯଦି (instance == VIMOUT)
	tclmsg((ଅକ୍ଷର *)str);
    ଅନ୍ୟଥା
    ଯଦି (instance == VIMERR)
	tclerrmsg((ଅକ୍ଷର *)str);
    ଅନ୍ୟଥା
    {
	Tcl_SetErrno(EBADF);
	ଯଦି (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    ଫେରନ୍ତୁ result;
}

    ସ୍ଥିର ଶୂନ୍ୟ
tcl_channel_watch(ClientData instance UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    ଫେରନ୍ତୁ EINVAL;
}


ସ୍ଥିର Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_VERSION_2, // version
    tcl_channel_close,	// closeProc
    tcl_channel_ନିବେଶ,	// inputProc
    tcl_channel_output,	// outputProc
    ଖାଲି,		// seekProc
    ଖାଲି,		// setOptionProc
    ଖାଲି,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    ଖାଲି,		// close2Proc
    ଖାଲି,		// blockModeProc
#ifdef TCL_CHANNEL_VERSION_2
    ଖାଲି,		// flushProc
    ଖାଲି,		// handlerProc
#endif
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ifdef TCL_CHANNEL_VERSION_3
    ଖାଲି,		// wideSeekProc
#endif
#ifdef TCL_CHANNEL_VERSION_4
    ଖାଲି,		// threadActionProc
#endif
#ifdef TCL_CHANNEL_VERSION_5
    ଖାଲି		// truncateProc
#endif
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    ସ୍ଥିର ଶୂନ୍ୟ
tclupdatevars(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର varname[VARNAME_SIZE];	// must be writeable
    ଅକ୍ଷର *name;

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


    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tclinit(exarg_T *eap)
{
    ଅକ୍ଷର varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    ଅକ୍ଷର *name;

#ifdef DYNAMIC_TCL
    ଯଦି (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	ଫେରନ୍ତୁ FAIL;
    }
#endif

    ଯଦି (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	ସ୍ଥିର Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_SetStdChannel(ch1, TCL_STDOUT);
	Tcl_SetStdChannel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	ଯଦି (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    ଫେରନ୍ତୁ FAIL;
	}
#ଯଦି 0
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
	    (ClientData)ଖାଲି, (Tcl_CmdDeleteProc *)ଖାଲି);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (ClientData)ଖାଲି, (Tcl_CmdDeleteProc *)ଖାଲି);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (ClientData)ଖାଲି, (Tcl_CmdDeleteProc *)ଖାଲି);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (ClientData)ଖାଲି, (Tcl_CmdDeleteProc *)ଖାଲି);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (ClientData)ଖାଲି, (Tcl_CmdDeleteProc *)ଖାଲି);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (ClientData)ଖାଲି, (Tcl_CmdDeleteProc *)ଖାଲି);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (ClientData)ଖାଲି, (Tcl_CmdDeleteProc *)ଖାଲି);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (ଅକ୍ଷର *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (ଅକ୍ଷର *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (ଅକ୍ଷର *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (ଅକ୍ଷର *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (ଅକ୍ଷର *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (ଅକ୍ଷର *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    ଅନ୍ୟଥା
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ଶୂନ୍ୟ
tclerrmsg(ଅକ୍ଷର *text)
{
    ଅକ୍ଷର *next;

    ଯେତେବେଳେ ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    ଯଦି (*text)
	emsg(text);
}

    ସ୍ଥିର ଶୂନ୍ୟ
tclmsg(ଅକ୍ଷର *text)
{
    ଅକ୍ଷର *next;

    ଯେତେବେଳେ ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	ସନ୍ଦେଶ(text);
	text = next;
    }
    ଯଦି (*text)
	ସନ୍ଦେଶ(text);
}

    ସ୍ଥିର ଶୂନ୍ୟ
tcldelthisinterp(ଶୂନ୍ୟ)
{
    ଯଦି (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = ଖାଲି;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = ଖାଲି;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tclexit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା error)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା newerr = OK;

    ଯଦି (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#ଯଦି (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	ଅକ୍ଷର buf[50];

	sprintf(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	ଯଦି (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	ଅନ୍ୟଥା
	    newerr = FAIL;

	tcldelthisinterp();
    }
    ଅନ୍ୟଥା
    {
	ଅକ୍ଷର *result;

	result = (ଅକ୍ଷର *)Tcl_GetStringResult(tclinfo.interp);
	ଯଦି (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	ଅନ୍ୟଥା
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    ଫେରନ୍ତୁ newerr;
}

/*
 * ":tcl"
 */
    ଶୂନ୍ୟ
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err;

    script = script_get(eap, eap->arg);
    ଯଦି (!eap->skip)
    {
	err = tclinit(eap);
	ଯଦି (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    ଯଦି (script == ଖାଲି)
		err = Tcl_Eval(tclinfo.interp, (ଅକ୍ଷର *)eap->arg);
	    ଅନ୍ୟଥା
		err = Tcl_Eval(tclinfo.interp, (ଅକ୍ଷର *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    ଶୂନ୍ୟ
ex_tclfile(exarg_T *eap)
{
    ଅକ୍ଷର *file = (ଅକ୍ଷର *)eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

    err = tclinit(eap);
    ଯଦି (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    ଶୂନ୍ୟ
ex_tcldo(exarg_T *eap)
{
    ଅକ୍ଷର	*script, *ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err, rs, re, lnum;
    ଅକ୍ଷର	var_lnum[VARNAME_SIZE]; // must be writeable memory
    ଅକ୍ଷର	var_ଧାଡ଼ି[VARNAME_SIZE];
    linenr_T	first_ଧାଡ଼ି = 0;
    linenr_T	last_ଧାଡ଼ି = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (ଅକ୍ଷର *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_ଧାଡ଼ି, VAR_CURLINE);

    err = tclinit(eap);
    ଯଦି (err != OK)
	ଫେରନ୍ତୁ;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (ଅକ୍ଷର *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    ଯଦି (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    ଯେତେବେଳେ (err == TCL_OK  &&  rs <= re)
    {
	ଯଦି ((linenr_T)rs > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ଭାଙ୍ଗ;
	ଧାଡ଼ି = (ଅକ୍ଷର *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	ଯଦି (!ଧାଡ଼ି)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    ଭାଙ୍ଗ;
	}
	Tcl_SetVar(tclinfo.interp, var_ଧାଡ଼ି, ଧାଡ଼ି, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	ଯଦି (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#ଯଦି (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ଭାଙ୍ଗ;
	ଧାଡ଼ି = (ଅକ୍ଷର *)Tcl_GetVar(tclinfo.interp, var_ଧାଡ଼ି, 0);
	ଯଦି (ଧାଡ଼ି)
	{
	    ଯଦି (ml_replace((linenr_T)rs, (char_u *)ଧାଡ଼ି, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (first_ଧାଡ଼ି == 0)
		first_ଧାଡ଼ି = rs;
	    last_ଧାଡ଼ି = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    ଯଦି (first_ଧାଡ଼ି)
	changed_ଧାଡ଼ିଗୁଡ଼ିକ(first_ଧାଡ଼ି, 0, last_ଧାଡ଼ି + 1, (long)0);

    Tcl_UnsetVar(tclinfo.interp, var_ଧାଡ଼ି, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    ଯଦି (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (ଶୂନ୍ୟ)tclexit(err);
}

    ସ୍ଥିର ଶୂନ୍ୟ
tcldelallrefs(ସଂରଚନା ref *ref)
{
    ସଂରଚନା ref	*next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err;
    ଅକ୍ଷର	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    ଯଦି (exiting)
	ଫେରନ୍ତୁ;
#endif

    ଯେତେବେଳେ (ref != ଖାଲି)
    {
	next = ref->next;
	ଯଦି (ref->interp)
	{
	    ଯଦି (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		ଯଦି (err != TCL_OK)
		{
		    result = (ଅକ୍ଷର *)Tcl_GetStringResult(ref->interp);
		    ଯଦି (result)
			tclerrmsg(result);
		}
		Tcl_Decr_Ref_ଗଣନା(ref->delcmd);
		ref->delcmd = ଖାଲି;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((ଅକ୍ଷର *)ref);
	ref = next;
    }
}

    ଶୂନ୍ୟ
tcl_buffer_free(buf_T *buf)
{
    ସଂରଚନା ref *reflist;

#ifdef DYNAMIC_TCL
    ଯଦି (!stubs_initialized)	// Not using Tcl, nothing to do.
	ଫେରନ୍ତୁ;
#endif

    reflist = (ସଂରଚନା ref *)(buf->b_tcl_ref);
    ଯଦି (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (ଶୂନ୍ୟ *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = ଖାଲି;
    }
}

    ଶୂନ୍ୟ
tcl_window_free(win_T *win)
{
    ସଂରଚନା ref *reflist;

#ifdef DYNAMIC_TCL
    ଯଦି (!stubs_initialized)	// Not using Tcl, nothing to do.
	ଫେରନ୍ତୁ;
#endif

    reflist = (ସଂରଚନା ref*)(win->w_tcl_ref);
    ଯଦି (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (ଶୂନ୍ୟ *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = ଖାଲି;
    }
}

// The End
