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

typedef struct
{
    Tcl_Interp *interp;
    정수 exitvalue;
    정수 range_start, range_end;
    정수 lbase;
    char *curbuf, *curwin;
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
struct ref
{
    struct ref	*next;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    void	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
static char * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, buf_T *buf));
static char * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, win_T *win));
static 정수 tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, struct ref *reflist, void *vimobj, Tcl_Obj *delcmd));
static 정수 tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, 정수 *valueP, buf_T *buf));
static win_T *tclfindwin _ANSI_ARGS_ ((buf_T *buf));
static 정수 tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, 정수 objc, Tcl_Obj *CONST objv[], 정수 objn));
static 정수 tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, 정수 objc, Tcl_Obj *CONST objv[], 정수 objn));
static 정수 tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, 정수 objc, Tcl_Obj *CONST objv[], 정수 objn));
static void tcldelthisinterp _ANSI_ARGS_ ((void));

static 정수 vimerror _ANSI_ARGS_((Tcl_Interp *interp));
static void tclmsg _ANSI_ARGS_((char *text));
static void tclerrmsg _ANSI_ARGS_((char *text));
static void tclupdatevars _ANSI_ARGS_((void));

static struct ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#만약 defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
typedef 정수 HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define load_dll vimLoadLib
#  define symbol_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# 아니면
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
static struct {
    char* name;
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
    static 정수
tcl_runtime_link_init(char *libname, 정수 verbose)
{
    정수 i;

    만약 (hTclLib)
	반환 OK;
    만약 (!(hTclLib = load_dll(libname)))
    {
	만약 (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	반환 FAIL;
    }
    위해 (i = 0; tcl_funcname_table[i].ptr; ++i)
    {
	만약 (!(*tcl_funcname_table[i].ptr = symbol_from_dll(hTclLib,
						  tcl_funcname_table[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = NULL;
	    만약 (verbose)
		semsg(_(e_could_not_load_library_function_str),
						   tcl_funcname_table[i].name);
	    반환 FAIL;
	}
    }
    반환 OK;
}
#endif // defined(DYNAMIC_TCL)

#ifdef DYNAMIC_TCL
static char *find_executable_arg = NULL;
#endif

    void
vim_tcl_init(char *arg)
{
#ifdef DYNAMIC_TCL
    find_executable_arg = arg;
#아니면
    Tcl_FindExecutable(arg);
#endif
}

#만약 defined(DYNAMIC_TCL)

static 정수 stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    정수
tcl_enabled(정수 verbose)
{
    만약 (!stubs_initialized && find_executable_arg != NULL
	    && tcl_runtime_link_init((char *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_FindExecutable(find_executable_arg);

	만약 ((interp = dll_Tcl_CreateInterp()) != NULL)
	{
	    만약 (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != NULL)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    반환 stubs_initialized;
}
#endif

#만약 defined(EXITFREE)
/*
 * Called once when exiting.
 */
    void
vim_tcl_finalize(void)
{
# ifdef DYNAMIC_TCL
    만약 (stubs_initialized)
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
    static 정수
exitcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    정수 objc,
    Tcl_Obj *CONST objv[])
{
    정수 value = 0;

    switch (objc)
    {
	case 2:
	    만약 (Tcl_GetIntFromObj(interp, objv[1], &value) != TCL_OK)
		break;
	    // FALLTHROUGH
	case 1:
	    tclinfo.exitvalue = value;

	    Tcl_DeleteInterp(interp);
	    break;
	default:
	    Tcl_WrongNumArgs(interp, 1, objv, "?returnCode?");
    }
    반환 TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    static 정수
beepcmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    정수 objc,
    Tcl_Obj *CONST objv[])
{
    만약 (objc != 1)
    {
	Tcl_WrongNumArgs(interp, 1, objv, NULL);
	반환 TCL_ERROR;
    }
    vim_beep(BO_LANG);
    반환 TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    static 정수
buffercmd(
    ClientData dummy UNUSED,
    Tcl_Interp *interp,
    정수 objc,
    Tcl_Obj *CONST objv[])
{
    char	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    정수		err, n, idx;
    enum {BCMD_EXISTS, BCMD_LIST};
    static CONST84 char *bcmdoptions[] =
    {
	"exists", "list", (char *)0
    };

    만약 (objc < 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option");
	반환 TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    만약 (err == TCL_OK)
    {
	만약 (objc != 2)
	{
	    Tcl_WrongNumArgs(interp, 1, objv, "bufNumber");
	    반환 TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    만약 (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		만약 (name == NULL)
		    반환 TCL_ERROR;
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		반환 TCL_OK;
	    }
	}
	Tcl_SetResult(interp, _("invalid buffer number"), TCL_STATIC);
	반환 TCL_ERROR;
    }
    Tcl_ResetResult(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    만약 (err != TCL_OK)
	반환 err;
    switch (idx)
    {
	case BCMD_LIST:
	    만약 (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "");
		err = TCL_ERROR;
		break;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		만약 (name == NULL)
		{
		    err = TCL_ERROR;
		    break;
		}
		Tcl_AppendElement(interp, name);
	    }
	    break;

	case BCMD_EXISTS:
	    만약 (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		break;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    만약 (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != NULL);
		Tcl_SetObjResult(interp, resobj);
	    }
	    break;

	default:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    반환 err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    static 정수
windowcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    정수		objc,
    Tcl_Obj	*CONST objv[])
{
    char	*what, *string;
    win_T	*win;

    만약 (objc != 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option");
	반환 TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], NULL);
    만약 (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    만약 (string == NULL)
		반환 TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	반환 TCL_OK;
    }
    Tcl_SetResult(interp, _("unknown option"), TCL_STATIC);
    반환 TCL_ERROR;
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
    static 정수
bufselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    정수		objc,
    Tcl_Obj	*CONST objv[])
{
    정수		opt, err, idx, flags;
    정수		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    char	*line;

    enum
    {
	BUF_APPEND, BUF_COMMAND, BUF_COUNT, BUF_DELCMD, BUF_DELETE, BUF_EXPR,
	BUF_GET, BUF_INSERT, BUF_LAST, BUF_MARK, BUF_NAME, BUF_NUMBER,
	BUF_OPTION, BUF_SET, BUF_WINDOWS
    };
    static CONST84 char *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (char *)0
    };

    만약 (objc < 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
	반환 TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
    만약 (err != TCL_OK)
	반환 err;

    buf = (buf_T *)((struct ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    switch (idx)
    {
	case BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    break;

	case BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    break;

	case BUF_EXPR:
	    err = tclvimexpr(interp, objc, objv, 2);
	    break;

	case BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    만약 (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		err = TCL_ERROR;
		break;
	    }
	    만약 (buf->b_ffname)
		Tcl_SetResult(interp, (char *)buf->b_ffname, TCL_VOLATILE);
	    아니면
		Tcl_SetResult(interp, "", TCL_STATIC);
	    break;

	case BUF_LAST:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	case BUF_COUNT:
	    /*
	     * Get number of lines in buffer.
	     */
	    만약 (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		err = TCL_ERROR;
		break;
	    }
	    val1 = (정수)buf->b_ml.ml_line_count;
	    만약 (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_SetObjResult(interp, resobj);
	    break;

	case BUF_NUMBER:
	    /*
	     * Get buffer's number.
	     */
	    만약 (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		err = TCL_ERROR;
		break;
	    }
	    resobj = Tcl_NewIntObj((정수)buf->b_fnum);
	    Tcl_SetObjResult(interp, resobj);
	    break;

	case BUF_GET:
	    만약 (objc != 3 && objc != 4)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		break;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    만약 (err != TCL_OK)
		break;
	    만약 (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		만약 (err != TCL_OK)
		    break;
		만약 (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_ResetResult(interp);

		위해 (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    line = (char *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    만약 (line)
			Tcl_AppendElement(interp, line);
		    아니면
			err = TCL_ERROR;
		}
	    }
	    아니면
	    {  // objc == 3
		line = (char *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_SetResult(interp, line, TCL_VOLATILE);
	    }
	    break;

	case BUF_SET:
	    만약 (objc != 4 && objc != 5)
	    {
		Tcl_WrongNumArgs(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		break;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    만약 (err != TCL_OK)
		반환 TCL_ERROR;
	    만약 (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		line = Tcl_GetStringFromObj(objv[3], NULL);
		만약 (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		아니면
		만약 (ml_replace((linenr_T)val1, (char_u *)line, TRUE) != OK)
		{
		    Tcl_SetResult(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		아니면
		{
		    changed_bytes((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		break;
	    }
	    아니면
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		정수	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		만약 (err != TCL_OK)
		    break;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		만약 (err != TCL_OK)
		    break;
		만약 (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		만약 (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    break;
		}
		flags |= FL_UPDATE_CURBUF;

		위해 (i = 0; i < lc && n <= val2; i++)
		{
		    line = Tcl_GetStringFromObj(lv[i], NULL);
		    만약 (ml_replace((linenr_T)n, (char_u *)line, TRUE) != OK)
			goto setListError;
		    ++n;
		}
		만약 (i < lc)
		{
		    // append lines
		    do
		    {
			line = Tcl_GetStringFromObj(lv[i], NULL);
			만약 (ml_append((linenr_T)(n - 1),
					      (char_u *)line, 0, FALSE) != OK)
			    goto setListError;
			++n;
			++i;
		    } 동안 (i < lc);
		}
		아니면 만약 (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    do
		    {
			만약 (ml_delete((linenr_T)i) != OK)
			    goto setListError;
			++n;
		    } 동안 (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (long)MAXLNUM,
								    (long)lc);
		changed_lines((linenr_T)val1, 0, (linenr_T)val2 + 1, (long)lc);
		break;
    setListError:
		u_undo(1);  // ???
		Tcl_SetResult(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    break;

	case BUF_DELETE:
	    만약 (objc != 3  &&  objc != 4)
	    {
		Tcl_WrongNumArgs(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		break;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    만약 (err != TCL_OK)
		break;
	    val2 = val1;
	    만약 (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		만약 (err != TCL_OK)
		    반환 err;
		만약 (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    만약 (u_savedel((linenr_T)val1, (long)n) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		break;
	    }
	    위해 (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		만약 (err != TCL_OK)
		    break;
	    }
	    만약 (i > 0)
		deleted_lines_mark((linenr_T)val1, (long)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    break;

	case BUF_MARK:
	    만약 (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "markName");
		err = TCL_ERROR;
		break;
	    }
	    line = Tcl_GetStringFromObj(objv[2], NULL);

	    pos = NULL;
	    만약 (line[0] != '\0'  &&  line[1] == '\0')
		pos = getmark(line[0], FALSE);
	    만약 (pos == NULL)
	    {
		Tcl_SetResult(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		break;
	    }
	    err = vimerror(interp);
	    만약 (err != TCL_OK)
		break;
	    만약 (pos->lnum <= 0)
	    {
		Tcl_SetResult(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    아니면
	    {
		char rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (정수)row2tcl(pos->lnum), (정수)col2tcl(pos->col));
		Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
	    }
	    break;

	case BUF_INSERT:
	    opt = 1;
	    // fallthrough
	case BUF_APPEND:
	    만약 (objc != 4)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		break;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    만약 (err != TCL_OK)
		break;
	    만약 (opt)
		--val1;
	    만약 (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_SetResult(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		break;
	    }

	    line = Tcl_GetStringFromObj(objv[3], NULL);
	    만약 (ml_append((linenr_T)val1, (char_u *)line, 0, FALSE) != OK)
	    {
		Tcl_SetResult(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		break;
	    }
	    appended_lines_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    break;

	case BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    만약 (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		err = TCL_ERROR;
		break;
	    }
	    Tcl_ResetResult(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		만약 (win->w_buffer == buf)
		{
		    line = tclgetwindow(interp, win);
		    만약 (line != NULL)
			Tcl_AppendElement(interp, line);
		    아니면
		    {
			err = TCL_ERROR;
			break;
		    }
		}
	    }
	    break;

	case BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    만약 (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "command");
		err = TCL_ERROR;
		break;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (void *)buf, objv[2]);
	    break;

	default:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    만약 (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    만약 (flags & FL_ADJUST_CURSOR)
	check_cursor();
    만약 (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    반환 err;
}

/*
 * This function implements the window commands.
 */
    static 정수
winselfcmd(
    ClientData	ref,
    Tcl_Interp	*interp,
    정수		objc,
    Tcl_Obj	*CONST objv[])
{
    정수		err, idx, flags;
    정수		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    char	*str;

    enum
    {
	WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_EXPR,
	WIN_HEIGHT, WIN_OPTION
    };
    static CONST84 char *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (char *)0
    };

    만약 (objc < 2)
    {
	Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
	반환 TCL_ERROR;
    }

    err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
    만약 (err != TCL_OK)
	반환 TCL_ERROR;

    win = (win_T *)((struct ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_buffer;
    flags = 0;

    switch (idx)
    {
	case WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    break;

	case WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    break;

	case WIN_EXPR:
	    err = tclvimexpr(interp, objc, objv, 2);
	    break;

	case WIN_HEIGHT:
	    만약 (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		만약 (err != TCL_OK)
		    break;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(val1);
		err = vimerror(interp);
		만약 (err != TCL_OK)
		    break;
	    }
	    아니면
	    만약 (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		break;
	    }

	    resobj = Tcl_NewIntObj((정수)(win->w_height));
	    Tcl_SetObjResult(interp, resobj);
	    break;

	case WIN_BUFFER:
	    만약 (objc != 2)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		err = TCL_ERROR;
		break;
	    }
	    str = tclgetbuffer(interp, win->w_buffer);
	    만약 (str)
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	    아니면
		err = TCL_ERROR;
	    break;

	case WIN_DELCMD:
	    만약 (objc != 3)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "command");
		err = TCL_ERROR;
		break;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (void *)win, objv[2]);
	    break;

	case WIN_CURSOR:
	    만약 (objc > 4)
	    {
		Tcl_WrongNumArgs(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		break;
	    }
	    만약 (objc == 2)
	    {
		char buf[64];

		sprintf(buf, _("row %d column %d"), (정수)row2tcl(win->w_cursor.lnum), (정수)col2tcl(win->w_cursor.col));
		Tcl_SetResult(interp, buf, TCL_VOLATILE);
		break;
	    }
	    아니면 만약 (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_MSG);
		만약 (var == NULL)
		{
		    err = TCL_ERROR;
		    break;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_buffer);
		만약 (err != TCL_OK)
		    break;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_MSG);
		만약 (var == NULL)
		{
		    err = TCL_ERROR;
		    break;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		만약 (err != TCL_OK)
		    break;
	    }
	    아니면
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
		만약 (err != TCL_OK)
		    break;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		만약 (err != TCL_OK)
		    break;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.col = col2vim(val2);
	    win->w_set_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    break;

	default:
	    Tcl_SetResult(interp, _("not implemented yet"), TCL_STATIC);
	    break;
    }

    curwin = savewin;
    curbuf = savebuf;
    만약 (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    반환 err;
}


    static 정수
commandcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    정수		objc,
    Tcl_Obj	*CONST objv[])
{
    정수		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    반환 err;
}

    static 정수
optioncmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    정수		objc,
    Tcl_Obj	*CONST objv[])
{
    정수		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    반환 err;
}

    static 정수
exprcmd(
    ClientData	dummy UNUSED,
    Tcl_Interp	*interp,
    정수		objc,
    Tcl_Obj	*CONST objv[])
{
    반환 tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    static 정수
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    정수		*valueP,
    buf_T	*buf)
{
    정수 err, i;

    enum { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_TOP };

    static CONST84 char *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (char *)0
    };

    err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
    만약 (err == TCL_OK)
    {
	switch (i)
	{
	    case LN_BEGIN:
	    case LN_FIRST:
	    case LN_START:
	    case LN_TOP:
		*valueP = 1;
		break;
	    case LN_BOTTOM:
	    case LN_END:
	    case LN_LAST:
		*valueP = buf->b_ml.ml_line_count;
		break;
	}
	반환 TCL_OK;
    }
    Tcl_ResetResult(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    만약 (err != TCL_OK)
	반환 err;
    i = row2vim(i);
    만약 (i < 1  ||  i > buf->b_ml.ml_line_count)
    {
	Tcl_SetResult(interp, _("line number out of range"), TCL_STATIC);
	반환 TCL_ERROR;
    }
    *valueP = i;
    반환 TCL_OK;
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
	만약 (win->w_buffer == buf)
	    반환 win;
    }
    반환 curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    static 정수
tcldoexcommand(
    Tcl_Interp	*interp,
    정수		objc,
    Tcl_Obj	*CONST objv[],
    정수		objn)
{
    tcl_info	saveinfo;
    정수		err, flag, nobjs;
    char	*arg;

    nobjs = objc - objn;
    만약 (nobjs < 1 || nobjs > 2)
    {
	Tcl_WrongNumArgs(interp, objn, objv, "?-quiet? exCommand");
	반환 TCL_ERROR;
    }

    flag = 0;
    만약 (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], NULL);
	만약 (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	아니면
	{
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, _("unknown flag: "), arg, (char *)0);
	    반환 TCL_ERROR;
	}
	++objn;
    }

    memcpy(&saveinfo, &tclinfo, sizeof(tcl_info));
    tclinfo.interp = NULL;
    tclinfo.curwin = NULL;
    tclinfo.curbuf = NULL;

    arg = Tcl_GetStringFromObj(objv[objn], NULL);
    만약 (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    만약 (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    만약 (tclinfo.interp)
	tcldelthisinterp();
    memcpy(&tclinfo, &saveinfo, sizeof(tcl_info));
    tclupdatevars();

    반환 err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    static 정수
tclsetoption(
    Tcl_Interp	*interp,
    정수		objc,
    Tcl_Obj	*CONST objv[],
    정수		objn)
{
    정수		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    long	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    enum { OPT_OFF, OPT_ON, OPT_TOGGLE };
    static CONST84 char *optkw[] = { "off", "on", "toggle", (char *)0 };

    nobjs = objc - objn;
    만약 (nobjs != 1 && nobjs != 2)
    {
	Tcl_WrongNumArgs(interp, objn, objv, "vimOption ?value?");
	반환 TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], NULL);
    ++objn;
    gov = get_option_value(option, &lval, &sval, NULL, 0);
    err = TCL_OK;
    switch (gov)
    {
	case gov_string:
	    Tcl_SetResult(interp, (char *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    break;
	case gov_bool:
	case gov_number:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_SetObjResult(interp, resobj);
	    break;
	default:
	    Tcl_SetResult(interp, _("unknown vimOption"), TCL_STATIC);
	    반환 TCL_ERROR;
    }
    만약 (nobjs == 2)
    {
	만약 (gov != gov_string)
	{
	    sval = NULL;    // avoid compiler warning
	    err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
	    만약 (err != TCL_OK)
	    {
		Tcl_ResetResult(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    아니면
	    {
		switch (idx)
		{
		    case OPT_ON:
			lval = 1;
			break;
		    case OPT_OFF:
			lval = 0;
			break;
		    case OPT_TOGGLE:
			lval = !lval;
			break;
		}
	    }
	}
	아니면
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], NULL);
	만약 (err == TCL_OK)
	{
	    set_option_value_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    반환 err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    static 정수
tclvimexpr(
    Tcl_Interp	*interp,
    정수		objc,
    Tcl_Obj	*CONST objv[],
    정수		objn)
{
#ifdef FEAT_EVAL
    char	*expr, *str;
#endif
    정수		err;

    만약 (objc - objn != 1)
    {
	Tcl_WrongNumArgs(interp, objn, objv, "vimExpr");
	반환 TCL_ERROR;
    }

#ifdef FEAT_EVAL
    expr = Tcl_GetStringFromObj(objv[objn], NULL);
    str = (char *)eval_to_string((char_u *)expr, TRUE, FALSE);
    만약 (str == NULL)
	Tcl_SetResult(interp, _("invalid expression"), TCL_STATIC);
    아니면
    {
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#아니면
    Tcl_SetResult(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#endif

    반환 err;
}

/*
 * Check for internal vim errors.
 */
    static 정수
vimerror(Tcl_Interp *interp)
{
    만약 (got_int)
    {
	Tcl_SetResult(interp, _("keyboard interrupt"), TCL_STATIC);
	반환 TCL_ERROR;
    }
    아니면 만약 (did_emsg)
    {
	Tcl_SetResult(interp, _("Vim error"), TCL_STATIC);
	반환 TCL_ERROR;
    }
    반환 TCL_OK;
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
    struct ref *ref = (struct ref *)cref;

    만약 (ref->delcmd)
    {
	Tcl_DecrRefCount(ref->delcmd);
	ref->delcmd = NULL;
    }
    ref->interp = NULL;
}

    static char *
tclgetref(
    Tcl_Interp	*interp,
    void	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    char	*prefix,	// "win" or "buf"
    void	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    struct ref *ref, *unused = NULL;
    static char name[VARNAME_SIZE];
    Tcl_Command cmd;

    ref = (struct ref *)(*refstartP);
    만약 (ref == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	반환 NULL;
    }

    동안 (ref != NULL)
    {
	만약 (ref->interp == interp)
	    break;
	만약 (ref->interp == NULL)
	    unused = ref;
	ref = ref->next;
    }

    만약 (ref)
	vim_snprintf(name, sizeof(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    아니면
    {
	만약 (unused)
	    ref = unused;
	아니면
	{
	    ref = (struct ref *)Tcl_Alloc(sizeof(struct ref));
	    ref->interp = NULL;
	    ref->next = (struct ref *)(*refstartP);
	    (*refstartP) = (void *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, sizeof(name), "::vim::%s_%lx",
					       prefix, (unsigned long)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (ClientData)ref, (Tcl_CmdDeleteProc *)delref);
	만약 (!cmd)
	    반환 NULL;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = NULL;
	ref->vimobj = vimobj;
    }
    반환 name;
}

    static char *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    반환 tclgetref(interp, &(win->w_tcl_ref), "win", (void *)win, winselfcmd);
}

    static char *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    반환 tclgetref(interp, &(buf->b_tcl_ref), "buf", (void *)buf, bufselfcmd);
}

    static 정수
tclsetdelcmd(
    Tcl_Interp	*interp,
    struct ref	*reflist,
    void	*vimobj,
    Tcl_Obj	*delcmd)
{
    만약 (reflist == &refsdeleted)
    {
	Tcl_SetResult(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	반환 TCL_ERROR;
    }

    동안 (reflist != NULL)
    {
	만약 (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    만약 (reflist->delcmd)
		Tcl_DecrRefCount(reflist->delcmd);
	    Tcl_IncrRefCount(delcmd);
	    reflist->delcmd = delcmd;
	    반환 TCL_OK;
	}
	reflist = reflist->next;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_SetResult(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    반환 TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    static 정수
tcl_channel_close(ClientData instance, Tcl_Interp *interp UNUSED)
{
    정수		err = 0;

    // currently does nothing

    만약 (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_SetErrno(EBADF);
	err = EBADF;
    }
    반환 err;
}

    static 정수
tcl_channel_input(
    ClientData	instance UNUSED,
    char	*buf UNUSED,
    정수		bufsiz UNUSED,
    정수		*errptr)
{

    // input is currently not supported

    Tcl_SetErrno(EINVAL);
    만약 (errptr)
	*errptr = EINVAL;
    반환 -1;
}

    static 정수
tcl_channel_output(
    ClientData	instance,
    const char	*buf,
    정수		bufsiz,
    정수		*errptr)
{
    char_u	*str;
    정수		result;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    만약 (!str)
    {
	Tcl_SetErrno(ENOMEM);
	만약 (errptr)
	    *errptr = ENOMEM;
	반환 -1;
    }

    result = bufsiz;
    만약 (instance == VIMOUT)
	tclmsg((char *)str);
    아니면
    만약 (instance == VIMERR)
	tclerrmsg((char *)str);
    아니면
    {
	Tcl_SetErrno(EBADF);
	만약 (errptr)
	    *errptr = EBADF;
	result = -1;
    }
    vim_free(str);
    반환 result;
}

    static void
tcl_channel_watch(ClientData instance UNUSED, 정수 mask UNUSED)
{
    Tcl_SetErrno(EINVAL);
}

    static 정수
tcl_channel_gethandle(
    ClientData	instance UNUSED,
    정수		direction UNUSED,
    ClientData	*handleptr UNUSED)
{
    Tcl_SetErrno(EINVAL);
    반환 EINVAL;
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
    char varname[VARNAME_SIZE];	// must be writeable
    char *name;

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


    static 정수
tclinit(exarg_T *eap)
{
    char varname[VARNAME_SIZE];	// Tcl_LinkVar requires writeable varname
    char *name;

#ifdef DYNAMIC_TCL
    만약 (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	반환 FAIL;
    }
#endif

    만약 (!tclinfo.interp)
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
	만약 (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    반환 FAIL;
	}
#만약 0
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
	Tcl_LinkVar(interp, varname, (char *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_start = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (char *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (char *)&tclinfo.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_end   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (char *)&tclinfo.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_SIZE);
	tclinfo.curwin = Tcl_Alloc(VARNAME_SIZE);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (char *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (char *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    아니면
    {
	// Interpreter already exists, just update variables
	tclinfo.range_start = row2tcl(eap->line1);
	tclinfo.range_end = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    반환 OK;
}

    static void
tclerrmsg(char *text)
{
    char *next;

    동안 ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	emsg(text);
	text = next;
    }
    만약 (*text)
	emsg(text);
}

    static void
tclmsg(char *text)
{
    char *next;

    동안 ((next=strchr(text, '\n')))
    {
	*next++ = '\0';
	msg(text);
	text = next;
    }
    만약 (*text)
	msg(text);
}

    static void
tcldelthisinterp(void)
{
    만약 (!Tcl_InterpDeleted(tclinfo.interp))
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

    static 정수
tclexit(정수 error)
{
    정수 newerr = OK;

    만약 (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#만약 (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	|| Tcl_LimitExceeded(tclinfo.interp)  // True if the interpreter cannot continue
#endif
	)
    {
	char buf[50];

	sprintf(buf, _(e_exit_code_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	만약 (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	아니면
	    newerr = FAIL;

	tcldelthisinterp();
    }
    아니면
    {
	char *result;

	result = (char *)Tcl_GetStringResult(tclinfo.interp);
	만약 (error == TCL_OK)
	{
	    tclmsg(result);
	    newerr = OK;
	}
	아니면
	{
	    tclerrmsg(result);
	    newerr = FAIL;
	}
    }

    반환 newerr;
}

/*
 * ":tcl"
 */
    void
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    정수		err;

    script = script_get(eap, eap->arg);
    만약 (!eap->skip)
    {
	err = tclinit(eap);
	만약 (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    만약 (script == NULL)
		err = Tcl_Eval(tclinfo.interp, (char *)eap->arg);
	    아니면
		err = Tcl_Eval(tclinfo.interp, (char *)script);
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
    char *file = (char *)eap->arg;
    정수 err;

    err = tclinit(eap);
    만약 (err == OK)
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
    char	*script, *line;
    정수		err, rs, re, lnum;
    char	var_lnum[VARNAME_SIZE]; // must be writeable memory
    char	var_line[VARNAME_SIZE];
    linenr_T	first_line = 0;
    linenr_T	last_line = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (char *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_line, VAR_CURLINE);

    err = tclinit(eap);
    만약 (err != OK)
	반환;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (char *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    만약 (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_SetResult(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    동안 (err == TCL_OK  &&  rs <= re)
    {
	만약 ((linenr_T)rs > curbuf->b_ml.ml_line_count)
	    break;
	line = (char *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	만약 (!line)
	{
	    Tcl_SetResult(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    break;
	}
	Tcl_SetVar(tclinfo.interp, var_line, line, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	만약 (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#만약 (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5) || TCL_MAJOR_VERSION > 8
	    || Tcl_LimitExceeded(tclinfo.interp)
#endif
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_line_count)
	    break;
	line = (char *)Tcl_GetVar(tclinfo.interp, var_line, 0);
	만약 (line)
	{
	    만약 (ml_replace((linenr_T)rs, (char_u *)line, TRUE) != OK)
	    {
		Tcl_SetResult(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		break;
	    }
	    만약 (first_line == 0)
		first_line = rs;
	    last_line = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    만약 (first_line)
	changed_lines(first_line, 0, last_line + 1, (long)0);

    Tcl_UnsetVar(tclinfo.interp, var_line, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    만약 (err == TCL_OK)
	Tcl_ResetResult(tclinfo.interp);

    (void)tclexit(err);
}

    static void
tcldelallrefs(struct ref *ref)
{
    struct ref	*next;
    정수		err;
    char	*result;

#ifdef DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    만약 (exiting)
	반환;
#endif

    동안 (ref != NULL)
    {
	next = ref->next;
	만약 (ref->interp)
	{
	    만약 (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		만약 (err != TCL_OK)
		{
		    result = (char *)Tcl_GetStringResult(ref->interp);
		    만약 (result)
			tclerrmsg(result);
		}
		Tcl_DecrRefCount(ref->delcmd);
		ref->delcmd = NULL;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((char *)ref);
	ref = next;
    }
}

    void
tcl_buffer_free(buf_T *buf)
{
    struct ref *reflist;

#ifdef DYNAMIC_TCL
    만약 (!stubs_initialized)	// Not using Tcl, nothing to do.
	반환;
#endif

    reflist = (struct ref *)(buf->b_tcl_ref);
    만약 (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (void *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = NULL;
    }
}

    void
tcl_window_free(win_T *win)
{
    struct ref *reflist;

#ifdef DYNAMIC_TCL
    만약 (!stubs_initialized)	// Not using Tcl, nothing to do.
	반환;
#endif

    reflist = (struct ref*)(win->w_tcl_ref);
    만약 (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (void *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = NULL;
    }
}

// The End
