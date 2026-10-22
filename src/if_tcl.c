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

#include "vim.h"
#undef EXTERN			// tcl.h defines it too

#ifdef DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#endif

#include <tcl.h>
#include <string.h>

typedef sa_nracanaa
{
    Tcl_Interp *interp;
    පූර්ණ_සංඛ්‍යා exitvalue;
    පූර්ණ_සංඛ්‍යා range_start, range_end;
    පූර්ණ_සංඛ්‍යා lbase;
    var_na *curbuf, *curwin;
} tcl_info;

static tcl_info tclinfo = { NULL, 0, 0, 0, 0, NULL, NULL };

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
sa_nracanaa ref
{
    sa_nracanaa ref	*next;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    void	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
static var_na * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
static var_na * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
static පූර්ණ_සංඛ්‍යා tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, sa_nracanaa ref *reflist, void *vimobj, Tcl_Obj *delcmd));
static පූර්ණ_සංඛ්‍යා tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, පූර්ණ_සංඛ්‍යා *valueP, buf_T *buf));
static win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
static පූර්ණ_සංඛ්‍යා tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, පූර්ණ_සංඛ්‍යා objc, Tcl_Obj *CONST objv[], පූර්ණ_සංඛ්‍යා objn));
static පූර්ණ_සංඛ්‍යා tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, පූර්ණ_සංඛ්‍යා objc, Tcl_Obj *CONST objv[], පූර්ණ_සංඛ්‍යා objn));
static පූර්ණ_සංඛ්‍යා tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, පූර්ණ_සංඛ්‍යා objc, Tcl_Obj *CONST objv[], පූර්ණ_සංඛ්‍යා objn));
static void tcldelthisinterp _ANSI_ARGS_ ((void));

static පූර්ණ_සංඛ්‍යා vimerror _ANSI_ARGS_((Tcl_Interp *interp));
static void tclmsg _ANSI_ARGS_((var_na *text));
static void tclerrmsg _ANSI_ARGS_((var_na *text));
static void tclupdatevars _ANSI_ARGS_((void));

static sa_nracanaa ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#නම් defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
typedef පූර්ණ_සංඛ්‍යා HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define symbol_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# නැතහොත්
#  include <dlfcn.h>
#  define HANDLE void*
#  define TCL_PROC void*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define symbol_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
static HANDLE hTclLib = NULL;
Tcl_Interp* (*dll_Tcl_CreateInterp)(void);
void (*dll_Tcl_FindExecutable)(const void *);

/*
 * Table of name to function pointer of tcl.
 */
static sa_nracanaa {
    var_na* name;
    TCL_PROC* ptr;
} tcl_funcname_table[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_FindExecutable},
    {NULL, NULL},
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
    static පූර්ණ_සංඛ්‍යා
tcl_runtime_link_init(var_na *libname, පූර්ණ_සංඛ්‍යා verbose)
{
    පූර්ණ_සංඛ්‍යා i;

    නම් (hTclLib)
	ආපසු OK;
    නම් (!(hTclLib = load_dll(libname)))
    {
	නම් (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ආපසු FAIL;
    }
    සඳහා (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	නම් (!(*tcl_funcname_table[i].ptr = symbol_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = NULL;
	    නම් (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    ආපසු FAIL;
	}
    }
    ආපසු OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
static var_na *find_executable_arg = NULL;
#endif

    void
vim_tcl_init(var_na *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#නැතහොත්
    Tcl_FindExecutable(arg);
#endif
}

#නම් defined(DYNAMIC_TCL)

static පූර්ණ_සංඛ්‍යා stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    පූර්ණ_සංඛ්‍යා
tcl_enabled(පූර්ණ_සංඛ්‍යා verbose)
{
    නම් (!stubs_initialized && find_executable_arg != NULL
	    && tcl_runtime_link_init((var_na *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	නම් ((interp = dll_Tcl_CreateInterp()) != NULL)
	{
	    නම් (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != NULL)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    ආපසු stubs_initialized;
}
#endif

#නම් defined(EXITFREE)
/*
 * Called once when exiting.
 */
    void
vim_tcl_finalize(void)
{
# ifdef DYNAMIC_TCL
    නම් (stubs_initialized)
# endif
	Tcl_Finalize();
}
#endif

    void
tcl_end(void)
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
    static පූර්ණ_සංඛ්‍යා
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    පූර්ණ_සංඛ්‍යා objc,
    Tcl_Obj *CONST objv[])
{
    පූර්ණ_සංඛ්‍යා value = 0;

    switch (objc)
    {
	pak_sa 2:
	    නම් (Tcl_GetIntFromObj(interp, objv[1], &value) != TCL_OK)
		bha_nga;
	    // FALLTHROUGH
	pak_sa 1:
	    tclinfo.exitvalue = value;

	    Tcl_DeleteInterp(interp);
	    bha_nga;
	default:
	    Tcl_WrongNumArgs(interp, 1, objv, "?returnCode?");
    }
    ආපසු TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    static පූර්ණ_සංඛ්‍යා
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    පූර්ණ_සංඛ්‍යා objc,
    Tcl_Obj *CONST objv[])
{
    නම් (objc != 1)
    {
	Tcl_WrongNumArgs(interp, 1, objv, NULL);
	ආපසු TCL_ERROR;
    }
    vim_beep(BO_LANG);
    ආපසු TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    static පූර්ණ_සංඛ්‍යා
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    පූර්ණ_සංඛ්‍යා objc,
    Tcl_Obj *CONST objv[])
{
    var_na	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    පූර්ණ_සංඛ්‍යා		err, n, idx;
    ga_nanaa {BCMD_EXISTS, BCMD_LIST};
    static CONST84 var_na *bcmdoptions[] =
    {
	"exists", "list", (var_na *)0
    };

    නම් (objc < 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option");
	ආපසු TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    නම් (err == TCL_OK)
    {
	නම් (objc != 2)
	{
	    Tcl_WrongNumArgs(interp, 1, objv, "bufNumber");
	    ආපසු TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    නම් (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		නම් (name == NULL)
		    ආපසු TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		ආපසු TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	ආපසු TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    නම් (err != TCL_OK)
	ආපසු err;
    switch (idx)
    {
	pak_sa BCMD_LIST:
	    නම් (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "");
		err = TCL_ERROR;
		bha_nga;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		නම් (name == NULL)
		{
		    err = TCL_ERROR;
		    bha_nga;
		}
		Tcl_AppendElement(interp, name);
	    }
	    bha_nga;

	pak_sa BCMD_EXISTS:
	    නම් (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		bha_nga;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    නම් (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != NULL);
		Tcl_SetObjResult(interp, resobj);
	    }
	    bha_nga;

	default:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    ආපසු err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    static පූර්ණ_සංඛ්‍යා
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    පූර්ණ_සංඛ්‍යා		objc,
    Tcl_Obj	*CONST objv[])
{
    var_na	*what, *string;
    win_T	*win;

    නම් (objc != 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option");
	ආපසු TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], NULL);
    නම් (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    නම් (string == NULL)
		ආපසු TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	ආපසු TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    ආපසු TCL_ERROR;
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
    static පූර්ණ_සංඛ්‍යා
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    පූර්ණ_සංඛ්‍යා		objc,
    Tcl_Obj	*CONST objv[])
{
    පූර්ණ_සංඛ්‍යා		opt, err, idx, flags;
    පූර්ණ_සංඛ්‍යා		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    var_na	*line;

    ga_nanaa
    {
	BUF_APPEND, BUF_COMMAND, BUF_COUNT, BUF_DELCMD, BUF_DELETE, BUF_EXPR,
	BUF_GET, BUF_INSERT, BUF_LAST, BUF_MARK, BUF_NAME, BUF_NUMBER,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    static CONST84 var_na *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (var_na *)0
    };

    නම් (objc < 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
	ආපසු TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    නම් (err != TCL_OK)
	ආපසු err;

    buf = (buf_T *)((sa_nracanaa ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    switch (idx)
    {
	pak_sa BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    bha_nga;

	pak_sa BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    bha_nga;

	pak_sa BUF_EXPR:
	    err = tclvimexpr(interp, objc, objv, 2);
	    bha_nga;

	pak_sa BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    නම් (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		err = TCL_ERROR;
		bha_nga;
	    }
	    නම් (buf->b_ffname)
		Tcl_SetResult(interp, (var_na *)buf->b_ffname, TCL_VOLATILE);
	    නැතහොත්
		Tcl_SetResult(interp, "", TCL_STATIC);
	    bha_nga;

	pak_sa BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	pak_sa BUF_COUNT:
	    /*
	     * Get number of lines in buffer.
	     */
	    නම් (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		err = TCL_ERROR;
		bha_nga;
	    }
	    val1 = (පූර්ණ_සංඛ්‍යා)buf->b_ml.ml_line_count;
	    නම් (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    bha_nga;

	pak_sa BUF_NUMBER:
	    /*
	     * Get buffer's number.
	     */
	    නම් (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		err = TCL_ERROR;
		bha_nga;
	    }
	    resobj = Tcl_NewIntObj((පූර්ණ_සංඛ්‍යා)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    bha_nga;

	pak_sa BUF_GET:
	    නම් (objc != 3 && objc != 4)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		bha_nga;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    නම් (err != TCL_OK)
		bha_nga;
	    නම් (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		නම් (err != TCL_OK)
		    bha_nga;
		නම් (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		සඳහා (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    line = (var_na *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    නම් (line)
			Tcl_AppendElement(interp, line);
		    නැතහොත්
			err = TCL_ERROR;
		}
	    }
	    නැතහොත්
	    {  // objc == 3
		line = (var_na *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, line, TCL_VOLATILE);
	    }
	    bha_nga;

	pak_sa BUF_SET:
	    නම් (objc != 4 && objc != 5)
	    {
		Tcl_WrongNumArgs(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		bha_nga;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    නම් (err != TCL_OK)
		ආපසු TCL_ERROR;
	    නම් (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		line = Tcl_GetStringFromObj(objv[3], NULL);
		නම් (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		නැතහොත්
		නම් (ml_replace((linenr_T)val1, (char_u *)line, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		නැතහොත්
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		bha_nga;
	    }
	    නැතහොත්
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		පූර්ණ_සංඛ්‍යා	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		නම් (err != TCL_OK)
		    bha_nga;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		නම් (err != TCL_OK)
		    bha_nga;
		නම් (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		නම් (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    bha_nga;
		}
		flags |= FL_UPDATE_CURBUF;

		සඳහා (i = 0; i < lc && n <= val2; i++)
		{
		    line = Tcl_GetStringFromObj(lv[i], NULL);
		    නම් (ml_replace((linenr_T)n, (char_u *)line, TRUE) != OK)
			goto setListError;
		    ++n;
		}
		නම් (i < lc)
		{
		    // append lines
		    do
		    {
			line = Tcl_GetStringFromObj(lv[i], NULL);
			නම් (ml_append((linenr_T)(n - 1),
					      (char_u *)line, 0, FALSE) != OK)
			    goto setListError;
			++n;
			++i;
		    } අතරතුර (i < lc);
		}
		නැතහොත් නම් (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    do
		    {
			නම් (ml_delete((linenr_T)i) != OK)
			    goto setListError;
			++n;
		    } අතරතුර (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (long)MAXLNUM,
								    (long)lc);
		changed_lines((linenr_T)val1, 0, (linenr_T)val2 + 1, (long)lc);
		bha_nga;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    bha_nga;

	pak_sa BUF_DELETE:
	    නම් (objc != 3  &&  objc != 4)
	    {
		Tcl_WrongNumArgs(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		bha_nga;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    නම් (err != TCL_OK)
		bha_nga;
	    val2 = val1;
	    නම් (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		නම් (err != TCL_OK)
		    ආපසු err;
		නම් (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    නම් (u_savedel((linenr_T)val1, (long)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		bha_nga;
	    }
	    සඳහා (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		නම් (err != TCL_OK)
		    bha_nga;
	    }
	    නම් (i > 0)
		deleted_lines_mark((linenr_T)val1, (long)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    bha_nga;

	pak_sa BUF_MARK:
	    නම් (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "markName");
		err = TCL_ERROR;
		bha_nga;
	    }
	    line = Tcl_GetStringFromObj(objv[2], NULL);

	    pos = NULL;
	    නම් (line[0] != '\0'  &&  line[1] == '\0')
		pos = getmark(line[0], FALSE);
	    නම් (pos == NULL)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		bha_nga;
	    }
	    err = vimerror(interp);
	    නම් (err != TCL_OK)
		bha_nga;
	    නම් (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    නැතහොත්
	    {
		var_na rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (පූර්ණ_සංඛ්‍යා)row2tcl(pos->lnum), (පූර්ණ_සංඛ්‍යා)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    bha_nga;

	pak_sa BUF_INSERT:
	    opt = 1;
	    // fallthrough
	pak_sa BUF_APPEND:
	    නම් (objc != 4)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		bha_nga;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    නම් (err != TCL_OK)
		bha_nga;
	    නම් (opt)
		--val1;
	    නම් (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		bha_nga;
	    }

	    line = Tcl_GetStringFromObj(objv[3], NULL);
	    නම් (ml_append((linenr_T)val1, (char_u *)line, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		bha_nga;
	    }
	    appended_lines_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    bha_nga;

	pak_sa BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    නම් (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		err = TCL_ERROR;
		bha_nga;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		නම් (win->w_buffer == buf)
		{
		    line = tclgetwindow(interp, win);
		    නම් (line != NULL)
			Tcl_AppendElement(interp, line);
		    නැතහොත්
		    {
			err = TCL_ERROR;
			bha_nga;
		    }
		}
	    }
	    bha_nga;

	pak_sa BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    නම් (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "command");
		err = TCL_ERROR;
		bha_nga;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (void *)buf, objv[2]);
	    bha_nga;

	default:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    නම් (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    නම් (flags & FL_ADJUST_CURSOR)
	check_cursor();
    නම් (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    ආපසු err;
}

/*
 * This function implements the window commands.
 */
    static පූර්ණ_සංඛ්‍යා
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    පූර්ණ_සංඛ්‍යා		objc,
    Tcl_Obj	*CONST objv[])
{
    පූර්ණ_සංඛ්‍යා		err, idx, flags;
    පූර්ණ_සංඛ්‍යා		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    var_na	*str;

    ga_nanaa
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_EXPR,
	WIN_HEIGHT, WIN_OPTION
    };
    static CONST84 var_na *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (var_na *)0
    };

    නම් (objc < 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
	ආපසු TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    නම් (err != TCL_OK)
	ආපසු TCL_ERROR;

    win = (win_T *)((sa_nracanaa ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    switch (idx)
    {
	pak_sa WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    bha_nga;

	pak_sa WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    bha_nga;

	pak_sa WIN_EXPR:
	    err = tclvimexpr(interp, objc, objv, 2);
	    bha_nga;

	pak_sa WIN_HEIGHT:
	    නම් (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		නම් (err != TCL_OK)
		    bha_nga;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		නම් (err != TCL_OK)
		    bha_nga;
	    }
	    නැතහොත්
	    නම් (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		bha_nga;
	    }

	    resobj = Tcl_NewIntObj((පූර්ණ_සංඛ්‍යා)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    bha_nga;

	pak_sa WIN_BUFFER:
	    නම් (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		err = TCL_ERROR;
		bha_nga;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    නම් (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    නැතහොත්
		err = TCL_ERROR;
	    bha_nga;

	pak_sa WIN_DELCMD:
	    නම් (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "command");
		err = TCL_ERROR;
		bha_nga;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (void *)win, objv[2]);
	    bha_nga;

	pak_sa WIN_CURSOR:
	    නම් (objc > 4)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		bha_nga;
	    }
	    නම් (objc == 2)
	    {
		var_na buf[64];

		sprintf(buf, _("row %d column %d"), (පූර්ණ_සංඛ්‍යා)row2tcl(win->w_cursor.lnum), (පූර්ණ_සංඛ්‍යා)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		bha_nga;
	    }
	    නැතහොත් නම් (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_MSG);
		නම් (var == NULL)
		{
		    err = TCL_ERROR;
		    bha_nga;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		නම් (err != TCL_OK)
		    bha_nga;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_MSG);
		නම් (var == NULL)
		{
		    err = TCL_ERROR;
		    bha_nga;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		නම් (err != TCL_OK)
		    bha_nga;
	    }
	    නැතහොත්
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		නම් (err != TCL_OK)
		    bha_nga;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		නම් (err != TCL_OK)
		    bha_nga;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    bha_nga;

	default:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    bha_nga;
    }

    curwin = savewin;
    curbuf = savebuf;
    නම් (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    ආපසු err;
}


    static පූර්ණ_සංඛ්‍යා
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    පූර්ණ_සංඛ්‍යා		objc,
    Tcl_Obj	*CONST objv[])
{
    පූර්ණ_සංඛ්‍යා		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ආපසු err;
}

    static පූර්ණ_සංඛ්‍යා
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    පූර්ණ_සංඛ්‍යා		objc,
    Tcl_Obj	*CONST objv[])
{
    පූර්ණ_සංඛ්‍යා		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    ආපසු err;
}

    static පූර්ණ_සංඛ්‍යා
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    පූර්ණ_සංඛ්‍යා		objc,
    Tcl_Obj	*CONST objv[])
{
    ආපසු tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    static පූර්ණ_සංඛ්‍යා
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    පූර්ණ_සංඛ්‍යා		*valueP,
    buf_T	*buf)
{
    පූර්ණ_සංඛ්‍යා err, i;

    ga_nanaa { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_TOP };

    static CONST84 var_na *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (var_na *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    නම් (err == TCL_OK)
    {
	switch (i)
	{
	    pak_sa LN_BEGIN:
	    pak_sa LN_FIRST:
	    pak_sa LN_START:
	    pak_sa LN_TOP:
		*valueP = 1;
		bha_nga;
	    pak_sa LN_BOTTOM:
	    pak_sa LN_END:
	    pak_sa LN_LAST:
		*valueP = buf->b_ml.ml_line_count;
		bha_nga;
	}
	ආපසු TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    නම් (err != TCL_OK)
	ආපසු err;
    i = row2vim(i);
    නම් (i < 1  ||  i > buf->b_ml.ml_line_count)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	ආපසු TCL_ERROR;
    }
    *valueP = i;
    ආපසු TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    static win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	නම් (win->w_buffer == buf)
	    ආපසු win;
    }
    ආපසු curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    static පූර්ණ_සංඛ්‍යා
tcldoexcommand(
    Tcl_Interp	*interp,
    පූර්ණ_සංඛ්‍යා		objc,
    Tcl_Obj	*CONST objv[],
    පූර්ණ_සංඛ්‍යා		objn)
{
    tcl_info	saveinfo;
    පූර්ණ_සංඛ්‍යා		err, flag, nobjs;
    var_na	*arg;

    nobjs = objc - objn;
    නම් (nobjs < 1 || nobjs > 2)
    {
	Tcl_WrongNumArgs(interp, objn, objv, "?-quiet? exCommand");
	ආපසු TCL_ERROR;
    }

    flag = 0;
    නම් (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], NULL);
	නම් (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	නැතහොත්
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (var_na *)0);
	    ආපසු TCL_ERROR;
	}
	++objn;
    }

    memcpy(&saveinfo, &tclinfo, parimaa_na(tcl_info));
    tclinfo.interp = NULL;
    tclinfo.curwin = NULL;
    tclinfo.curbuf = NULL;

    arg = Tcl_GetStringFromObj(objv[objn], NULL);
    නම් (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    නම් (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    නම් (tclinfo.interp)
	tcldelthisinterp();
    memcpy(&tclinfo, &saveinfo, parimaa_na(tcl_info));
    tclupdatevars();

    ආපසු err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    static පූර්ණ_සංඛ්‍යා
tclsetoption(
    Tcl_Interp	*interp,
    පූර්ණ_සංඛ්‍යා		objc,
    Tcl_Obj	*CONST objv[],
    පූර්ණ_සංඛ්‍යා		objn)
{
    පූර්ණ_සංඛ්‍යා		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    long	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    ga_nanaa { OPT_OFF, OPT_ON, OPT_TOGGLE };
    static CONST84 var_na *optkw[] = { "off", "on", "toggle", (var_na *)0 };

    nobjs = objc - objn;
    නම් (nobjs != 1 && nobjs != 2)
    {
	Tcl_WrongNumArgs(interp, objn, objv, "vimOption ?value?");
	ආපසු TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], NULL);
    ++objn;
    gov = get_option_value(option, &lval, &sval, NULL, 0);
    err = TCL_OK;
    switch (gov)
    {
	pak_sa gov_string:
	    Tcl_SetResult(interp, (var_na *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    bha_nga;
	pak_sa gov_bool:
	pak_sa gov_number:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    bha_nga;
	default:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    ආපසු TCL_ERROR;
    }
    නම් (nobjs == 2)
    {
	නම් (gov != gov_string)
	{
	    sval = NULL;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    නම් (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    නැතහොත්
	    {
		switch (idx)
		{
		    pak_sa OPT_ON:
			lval = 1;
			bha_nga;
		    pak_sa OPT_OFF:
			lval = 0;
			bha_nga;
		    pak_sa OPT_TOGGLE:
			lval = !lval;
			bha_nga;
		}
	    }
	}
	නැතහොත්
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], NULL);
	නම් (err == TCL_OK)
	{
	    set_option_value_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    ආපසු err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    static පූර්ණ_සංඛ්‍යා
tclvimexpr(
    Tcl_Interp	*interp,
    පූර්ණ_සංඛ්‍යා		objc,
    Tcl_Obj	*CONST objv[],
    පූර්ණ_සංඛ්‍යා		objn)
{
#ifdef FEAT_EVAL
    var_na	*expr, *str;
#endif
    පූර්ණ_සංඛ්‍යා		err;

    නම් (objc - objn != 1)
    {
	Tcl_WrongNumArgs(interp, objn, objv, "vimExpr");
	ආපසු TCL_ERROR;
    }

#ifdef FEAT_EVAL
    expr = Tcl_GetStringFromObj(objv[objn], NULL);
    str = (var_na *)eval_to_string((char_u *)expr, TRUE, FALSE);
    නම් (str == NULL)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    නැතහොත්
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#නැතහොත්
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    ආපසු err;
}

/*
 * Check for internal vim errors.
 */
    static පූර්ණ_සංඛ්‍යා
vimerror(Tcl_Interp *interp)
{
    නම් (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	ආපසු TCL_ERROR;
    }
    නැතහොත් නම් (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	ආපසු TCL_ERROR;
    }
    ආපසු TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    static void
delref(ClientData cref)
{
    sa_nracanaa ref *ref = (sa_nracanaa ref *)cref;

    නම් (ref->delcmd)
    {
	Tcl_DecrRefCount(ref->delcmd);
	ref->delcmd = NULL;
    }
    ref->interp = NULL;
}

    static var_na *
tclgetref(
    Tcl_Interp	*interp,
    void	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    var_na	*prefix,	// "win" or "buf"
    void	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    sa_nracanaa ref *ref, *unused = NULL;
    static var_na name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (sa_nracanaa ref *)(*refstartP);
    නම් (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	ආපසු NULL;
    }

    අතරතුර (ref != NULL)
    {
	නම් (ref->interp == interp)
	    bha_nga;
	නම් (ref->interp == NULL)
	    unused = ref;
	ref = ref->next;
    }

    නම් (ref)
	vim_snprintf(name, parimaa_na(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    නැතහොත්
    {
	නම් (unused)
	    ref = unused;
	නැතහොත්
	{
	    ref = (sa_nracanaa ref *)Tcl_Alloc(parimaa_na(sa_nracanaa ref));
	    ref->interp = NULL;
	    ref->next = (sa_nracanaa ref *)(*refstartP);
	    (*refstartP) = (void *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, parimaa_na(name), "::vim::%s_%lx",
					       prefix, (unsigned long)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	නම් (!cmd)
	    ආපසු NULL;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = NULL;
	ref->vimobj = vimobj;
    }
    ආපසු name;
}

    static var_na *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    ආපසු tclgetref(interp, &(win->w_tcl_ref), "win", (void *)win, winselfcmd);
}

    static var_na *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    ආපසු tclgetref(interp, &(buf->b_tcl_ref), "buf", (void *)buf, bufselfcmd);
}

    static පූර්ණ_සංඛ්‍යා
tclsetdelcmd(
    Tcl_Interp	*interp,
    sa_nracanaa ref	*reflist,
    void	*vimobj,
    Tcl_Obj	*delcmd)
{
    නම් (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	ආපසු TCL_ERROR;
    }

    අතරතුර (reflist != NULL)
    {
	නම් (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    නම් (reflist->delcmd)
		Tcl_DecrRefCount(reflist->delcmd);
	    Tcl_IncrRefCount(delcmd);
	    reflist->delcmd = delcmd;
	    ආපසු TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    ආපසු TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    static පූර්ණ_සංඛ්‍යා
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    පූර්ණ_සංඛ්‍යා		err = 0;

    // currently does nothing

    නම් (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    ආපසු err;
}

    static පූර්ණ_සංඛ්‍යා
tcl_channel_input(
    ClientData	instance UNUSED,
    var_na	*buf UNUSED,
    පූර්ණ_සංඛ්‍යා		bufsiz UNUSED,
    පූර්ණ_සංඛ්‍යා		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    නම් (errptr)
	*errptr = EINVAL;
    ආපසු -1;
}

    static පූර්ණ_සංඛ්‍යා
tcl_channel_output(
    ClientData	instance,
    const var_na	*buf,
    පූර්ණ_සංඛ්‍යා		bufsiz,
    පූර්ණ_සංඛ්‍යා		*errptr)
{
    char_u	*str;
    පූර්ණ_සංඛ්‍යා		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    නම් (!str)
    {
	Tcl_SetErrno(ENOMEM);
	නම් (errptr)
	    *errptr = ENOMEM;
	ආපසු -1;
    }

    result = bufsiz;
    නම් (instance == VIMOUT)
	tclmsg((var_na *)str);
    නැතහොත්
    නම් (instance == VIMERR)
	tclerrmsg((var_na *)str);
    නැතහොත්
    {
	Tcl_SetErrno(EBADF);
	නම් (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    ආපසු result;
}

    static void
tcl_channel_watch(ClientData instance UNUSED, පූර්ණ_සංඛ්‍යා mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    static පූර්ණ_සංඛ්‍යා
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    පූර්ණ_සංඛ්‍යා		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    ආපසු EINVAL;
}


static Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_VERSION_2, // version
    tcl_channel_close,	// closeProc
    tcl_channel_input,	// inputProc
    tcl_channel_output,	// outputProc
    NULL,		// seekProc
    NULL,		// setOptionProc
    NULL,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    NULL,		// close2Proc
    NULL,		// blockModeProc
#ifdef TCL_CHANNEL_VERSION_2
    NULL,		// flushProc
    NULL,		// handlerProc
#endif
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#ifdef TCL_CHANNEL_VERSION_3
    NULL,		// wideSeekProc
#endif
#ifdef TCL_CHANNEL_VERSION_4
    NULL,		// threadActionProc
#endif
#ifdef TCL_CHANNEL_VERSION_5
    NULL		// truncateProc
#endif
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    static void
tclupdatevars(void)
{
    var_na varname[VARNAME_SIZE];	// must be writeable
    var_na *name;

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


    static පූර්ණ_සංඛ්‍යා
tclinit(exarg_T *eap)
{
    var_na varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    var_na *name;

#ifdef DYNAMIC_TCL
    නම් (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	ආපසු FAIL;
    }
#endif

    නම් (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	static Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_SetStdChannel(ch1, TCL_STDOUT);
	Tcl_SetStdChannel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	නම් (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    ආපසු FAIL;
	}
#නම් 0
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
	    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (var_na *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (var_na *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (var_na *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (var_na *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (var_na *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (var_na *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    නැතහොත්
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    ආපසු OK;
}

    static void
tclerrmsg(var_na *text)
{
    var_na *next;

    අතරතුර ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    නම් (*text)
	emsg(text);
}

    static void
tclmsg(var_na *text)
{
    var_na *next;

    අතරතුර ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	msg(text);
	text = next;
    }
    නම් (*text)
	msg(text);
}

    static void
tcldelthisinterp(void)
{
    නම් (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = NULL;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = NULL;
}

    static පූර්ණ_සංඛ්‍යා
tclexit(පූර්ණ_සංඛ්‍යා error)
{
    පූර්ණ_සංඛ්‍යා newerr = OK;

    නම් (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#නම් (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	var_na buf[50];

	sprintf(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	නම් (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	නැතහොත්
	    newerr = FAIL;

	tcldelthisinterp();
    }
    නැතහොත්
    {
	var_na *result;

	result = (var_na *)Tcl_GetStringResult(tclinfo.interp);
	නම් (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	නැතහොත්
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    ආපසු newerr;
}

/*
 * ":tcl"
 */
    void
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    පූර්ණ_සංඛ්‍යා		err;

    script = script_get(eap, eap->arg);
    නම් (!eap->skip)
    {
	err = tclinit(eap);
	නම් (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    නම් (script == NULL)
		err = Tcl_Eval(tclinfo.interp, (var_na *)eap->arg);
	    නැතහොත්
		err = Tcl_Eval(tclinfo.interp, (var_na *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    void
ex_tclfile(exarg_T *eap)
{
    var_na *file = (var_na *)eap->arg;
    පූර්ණ_සංඛ්‍යා err;

    err = tclinit(eap);
    නම් (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    void
ex_tcldo(exarg_T *eap)
{
    var_na	*script, *line;
    පූර්ණ_සංඛ්‍යා		err, rs, re, lnum;
    var_na	var_lnum[VARNAME_SIZE]; // must be writeable memory
    var_na	var_line[VARNAME_SIZE];
    linenr_T	first_line = 0;
    linenr_T	last_line = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (var_na *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_line, VAR_CURLINE);

    err = tclinit(eap);
    නම් (err != OK)
	ආපසු;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (var_na *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    නම් (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    අතරතුර (err == TCL_OK  &&  rs <= re)
    {
	නම් ((linenr_T)rs > curbuf->b_ml.ml_line_count)
	    bha_nga;
	line = (var_na *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	නම් (!line)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    bha_nga;
	}
	Tcl_SetVar(tclinfo.interp, var_line, line, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	නම් (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#නම් (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_line_count)
	    bha_nga;
	line = (var_na *)Tcl_GetVar(tclinfo.interp, var_line, 0);
	නම් (line)
	{
	    නම් (ml_replace((linenr_T)rs, (char_u *)line, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		bha_nga;
	    }
	    නම් (first_line == 0)
		first_line = rs;
	    last_line = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    නම් (first_line)
	changed_lines(first_line, 0, last_line + 1, (long)0);

    Tcl_UnsetVar(tclinfo.interp, var_line, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    නම් (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (void)tclexit(err);
}

    static void
tcldelallrefs(sa_nracanaa ref *ref)
{
    sa_nracanaa ref	*next;
    පූර්ණ_සංඛ්‍යා		err;
    var_na	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    නම් (exiting)
	ආපසු;
#endif

    අතරතුර (ref != NULL)
    {
	next = ref->next;
	නම් (ref->interp)
	{
	    නම් (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		නම් (err != TCL_OK)
		{
		    result = (var_na *)Tcl_GetStringResult(ref->interp);
		    නම් (result)
			tclerrmsg(result);
		}
		Tcl_DecrRefCount(ref->delcmd);
		ref->delcmd = NULL;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((var_na *)ref);
	ref = next;
    }
}

    void
tcl_buffer_free(buf_T *buf)
{
    sa_nracanaa ref *reflist;

#ifdef DYNAMIC_TCL
    නම් (!stubs_initialized)	// Not using Tcl, nothing to do.
	ආපසු;
#endif

    reflist = (sa_nracanaa ref *)(buf->b_tcl_ref);
    නම් (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (void *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = NULL;
    }
}

    void
tcl_window_free(win_T *win)
{
    sa_nracanaa ref *reflist;

#ifdef DYNAMIC_TCL
    නම් (!stubs_initialized)	// Not using Tcl, nothing to do.
	ආපසු;
#endif

    reflist = (sa_nracanaa ref*)(win->w_tcl_ref);
    නම් (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (void *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = NULL;
    }
}

// The End
