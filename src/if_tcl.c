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
#تعریف_منسوخ EXTERN			// tcl.h defines it too

#اگر_تعریف_شدہ DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#اگر_ختم

#شامل <tcl.h>
#شامل <string.h>

قسمتعریف ساخت
{
    Tcl_Interp *interp;
    صحیح exitvalue;
    صحیح range_آغاز, range_انتہا;
    صحیح lbase;
    حرف *curbuf, *curwin;
} tcl_info;

ساکن tcl_info tclinfo = { تہی, 0, 0, 0, 0, تہی, تہی };

#تعریف_کرو VAR_RANGE1	"::vim::range(start)"
#تعریف_کرو VAR_RANGE2	"::vim::range(begin)"
#تعریف_کرو VAR_RANGE3	"::vim::range(end)"
#تعریف_کرو VAR_CURBUF	"::vim::current(buffer)"
#تعریف_کرو VAR_CURWIN	"::vim::current(window)"
#تعریف_کرو VAR_LBASE	"::vim::lbase"
#تعریف_کرو VAR_CURLINE	"line"
#تعریف_کرو VAR_CURLNUM	"lnum"
#تعریف_کرو VARNAME_سائز	64

#تعریف_کرو row2tcl(x)  ((x) - (tclinfo.lbase==0))
#تعریف_کرو row2vim(x)  ((x) + (tclinfo.lbase==0))
#تعریف_کرو col2tcl(x)  ((x) + (tclinfo.lbase!=0))
#تعریف_کرو col2vim(x)  ((x) - (tclinfo.lbase!=0))


#تعریف_کرو VIMOUT	((Client_ڈیٹا)1)
#تعریف_کرو VIMERR	((Client_ڈیٹا)2)

// This appears to be new in Tcl 8.4.
#اگر_غیر_تعریف_شدہ CONST84
# define CONST84
#اگر_ختم

/*
 *  List of Tcl interpreters who reference a vim window or buffer.
 *  Each buffer and window has its own list in the w_tcl_ref or b_tcl_ref
 *  struct member.  We need this because Tcl can create sub-interpreters with
 *  the "interp" command, and each interpreter can reference all windows and
 *  buffers.
 */
ساخت ref
{
    ساخت ref	*اگلا;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    خالی	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
ساکن حرف * tclgetbuffer ANSI_دلائل((Tcl_Interp *interp, buf_T *buf));
ساکن حرف * tclgetwindow ANSI_دلائل((Tcl_Interp *interp, win_T *win));
ساکن صحیح tclsetdelcmd ANSI_دلائل((Tcl_Interp *interp, ساخت ref *reflist, خالی *vimobj, Tcl_Obj *delcmd));
ساکن صحیح tclgetlinenum ANSI_دلائل ((Tcl_Interp *interp, Tcl_Obj *obj, صحیح *قدر_P, buf_T *buf));
ساکن win_T *tclfindwin ANSI_دلائل ((buf_T *buf));
ساکن صحیح tcldoexcommand ANSI_دلائل ((Tcl_Interp *interp, صحیح objc, Tcl_Obj *CONST objv[], صحیح objn));
ساکن صحیح tclsetoption ANSI_دلائل ((Tcl_Interp *interp, صحیح objc, Tcl_Obj *CONST objv[], صحیح objn));
ساکن صحیح tclvimexpr ANSI_دلائل ((Tcl_Interp *interp, صحیح objc, Tcl_Obj *CONST objv[], صحیح objn));
ساکن خالی tcldelthisinterp ANSI_دلائل ((خالی));

ساکن صحیح vimerror ANSI_دلائل((Tcl_Interp *interp));
ساکن خالی tclmsg ANSI_دلائل((حرف *متن));
ساکن خالی tclerrmsg ANSI_دلائل((حرف *متن));
ساکن خالی tclupdatevars ANSI_دلائل((خالی));

ساکن ساخت ref refsdeleted;	// dummy object for deleted ref list

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
قسمتعریف صحیح HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define بھرو_dll vim_بھرو_Lib
#  define علامت_from_dll Get_Proc_پتہ
#  define close_dll FreeLibrary
#  define بھرو_dll_error GetWin32Error
# ورنہ
#  include <dlfcn.h>
#  define HANDLE خالی*
#  define TCL_PROC خالی*
#  define بھرو_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define علامت_from_dll dlsym
#  define close_dll dlclose
#  define بھرو_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
ساکن HANDLE hTclLib = تہی;
Tcl_Interp* (*dll_Tcl_CreateInterp)(خالی);
خالی (*dll_Tcl_ڈھونڈو_Executable)(ثابت خالی *);

/*
 * Table of name to function pointer of tcl.
 */
ساکن ساخت {
    حرف* name;
    TCL_PROC* ptr;
} tcl_funcname_جدول[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_ڈھونڈو_Executable},
    {تہی, تہی},
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
    ساکن صحیح
tcl_runtime_link_init(حرف *libname, صحیح verbose)
{
    صحیح i;

    اگر (hTclLib)
	واپس OK;
    اگر (!(hTclLib = بھرو_dll(libname)))
    {
	اگر (verbose)
	    semsg(_(e_could_not_بھرو_library_str_str), libname, بھرو_dll_error());
	واپس FAIL;
    }
    برائے (i = 0; tcl_funcname_جدول[i].ptr; ++i)
    {
	اگر (!(*tcl_funcname_جدول[i].ptr = علامت_from_dll(hTclLib,
						  tcl_funcname_جدول[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = تہی;
	    اگر (verbose)
		semsg(_(e_could_not_بھرو_library_function_str),
						   tcl_funcname_جدول[i].name);
	    واپس FAIL;
	}
    }
    واپس OK;
}
#اگر_ختم // defined(DYNAMIC_TCL)

#اگر_تعریف_شدہ DYNAMIC_TCL
ساکن حرف *ڈھونڈو_executable_arg = تہی;
#اگر_ختم

    خالی
vim_tcl_init(حرف *arg)
{
#اگر_تعریف_شدہ DYNAMIC_TCL
    ڈھونڈو_executable_arg = arg;
#ورنہ
    Tcl_ڈھونڈو_Executable(arg);
#اگر_ختم
}

#اگر defined(DYNAMIC_TCL)

ساکن صحیح stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    صحیح
tcl_enabled(صحیح verbose)
{
    اگر (!stubs_initialized && ڈھونڈو_executable_arg != تہی
	    && tcl_runtime_link_init((حرف *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_ڈھونڈو_Executable(ڈھونڈو_executable_arg);

	اگر ((interp = dll_Tcl_CreateInterp()) != تہی)
	{
	    اگر (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != تہی)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    واپس stubs_initialized;
}
#اگر_ختم

#اگر defined(EXITFREE)
/*
 * Called once when exiting.
 */
    خالی
vim_tcl_finalize(خالی)
{
# ifdef DYNAMIC_TCL
    اگر (stubs_initialized)
# endif
	Tcl_Finalize();
}
#اگر_ختم

    خالی
tcl_انتہا(خالی)
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
    ساکن صحیح
exitcmd(
    Client_ڈیٹا dummy غیر_مستعمل,
    Tcl_Interp *interp,
    صحیح objc,
    Tcl_Obj *CONST objv[])
{
    صحیح قدر = 0;

    انتخاب (objc)
    {
	حالت 2:
	    اگر (Tcl_GetIntFromObj(interp, objv[1], &قدر) != TCL_OK)
		توڑو;
	    // FALLTHROUGH
	حالت 1:
	    tclinfo.exitvalue = قدر;

	    Tcl_DeleteInterp(interp);
	    توڑو;
	طےشدہ:
	    Tcl_Wrong_عدد_دلائل(interp, 1, objv, "?returnCode?");
    }
    واپس TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    ساکن صحیح
beepcmd(
    Client_ڈیٹا dummy غیر_مستعمل,
    Tcl_Interp *interp,
    صحیح objc,
    Tcl_Obj *CONST objv[])
{
    اگر (objc != 1)
    {
	Tcl_Wrong_عدد_دلائل(interp, 1, objv, تہی);
	واپس TCL_ERROR;
    }
    vim_beep(BO_LANG);
    واپس TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    ساکن صحیح
buffercmd(
    Client_ڈیٹا dummy غیر_مستعمل,
    Tcl_Interp *interp,
    صحیح objc,
    Tcl_Obj *CONST objv[])
{
    حرف	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    صحیح		err, n, idx;
    شمار {BCMD_EXISTS, BCMD_LIST};
    ساکن CONST84 حرف *bcmdoptions[] =
    {
	"exists", "list", (حرف *)0
    };

    اگر (objc < 2)
    {
	Tcl_Wrong_عدد_دلائل(interp, 1, objv, "option");
	واپس TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    اگر (err == TCL_OK)
    {
	اگر (objc != 2)
	{
	    Tcl_Wrong_عدد_دلائل(interp, 1, objv, "bufNumber");
	    واپس TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    اگر (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		اگر (name == تہی)
		    واپس TCL_ERROR;
		Tcl_سیٹ_نتیجہ(interp, name, TCL_VOLATILE);
		واپس TCL_OK;
	    }
	}
	Tcl_سیٹ_نتیجہ(interp, _("invalid buffer number"), TCL_STATIC);
	واپس TCL_ERROR;
    }
    Tcl_Reset_نتیجہ(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_Get_اشاریہ_From_Obj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    اگر (err != TCL_OK)
	واپس err;
    انتخاب (idx)
    {
	حالت BCMD_LIST:
	    اگر (objc != 2)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, "");
		err = TCL_ERROR;
		توڑو;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		اگر (name == تہی)
		{
		    err = TCL_ERROR;
		    توڑو;
		}
		Tcl_AppendElement(interp, name);
	    }
	    توڑو;

	حالت BCMD_EXISTS:
	    اگر (objc != 3)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		توڑو;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    اگر (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != تہی);
		Tcl_سیٹ_Obj_نتیجہ(interp, resobj);
	    }
	    توڑو;

	طےشدہ:
	    Tcl_سیٹ_نتیجہ(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    واپس err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    ساکن صحیح
windowcmd(
    Client_ڈیٹا	dummy غیر_مستعمل,
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[])
{
    حرف	*what, *string;
    win_T	*win;

    اگر (objc != 2)
    {
	Tcl_Wrong_عدد_دلائل(interp, 1, objv, "option");
	واپس TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], تہی);
    اگر (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    اگر (string == تہی)
		واپس TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	واپس TCL_OK;
    }
    Tcl_سیٹ_نتیجہ(interp, _("unknown option"), TCL_STATIC);
    واپس TCL_ERROR;
}

/*
 * flags for bufselfcmd and winselfcmd to indicate outstanding actions.
 */
#تعریف_کرو FL_UPDATE_SCREEN	(1<<0)
#تعریف_کرو FL_UPDATE_CURBUF	(1<<1)
#تعریف_کرو FL_ADJUST_CURSOR	(1<<2)

/*
 * This function implements the buffer commands.
 */
    ساکن صحیح
bufselfcmd(
    Client_ڈیٹا	ref,
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
    حرف	*سطر;

    شمار
    {
	BUF_APPEND, BUF_COMMAND, BUF_گنتی, BUF_DELCMD, BUF_DELETE, BUF_اظہاریہ,
	BUF_GET, BUF_داخل_کرو, BUF_آخری, BUF_MARK, BUF_NAME, BUF_نمبر,
	BUF_OPTION, BUF_سیٹ, BUF_WINDOWS
    };
    ساکن CONST84 حرف *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (حرف *)0
    };

    اگر (objc < 2)
    {
	Tcl_Wrong_عدد_دلائل(interp, 1, objv, "option ?arg ...?");
	واپس TCL_ERROR;
    }

    err = Tcl_Get_اشاریہ_From_Obj(interp, objv[1], bufoptions, "option", 0, &idx);
    اگر (err != TCL_OK)
	واپس err;

    buf = (buf_T *)((ساخت ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    انتخاب (idx)
    {
	حالت BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    توڑو;

	حالت BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    توڑو;

	حالت BUF_اظہاریہ:
	    err = tclvimexpr(interp, objc, objv, 2);
	    توڑو;

	حالت BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    اگر (objc != 2)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, تہی);
		err = TCL_ERROR;
		توڑو;
	    }
	    اگر (buf->b_ffname)
		Tcl_سیٹ_نتیجہ(interp, (حرف *)buf->b_ffname, TCL_VOLATILE);
	    ورنہ
		Tcl_سیٹ_نتیجہ(interp, "", TCL_STATIC);
	    توڑو;

	حالت BUF_آخری:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	حالت BUF_گنتی:
	    /*
	     * Get number of lines in buffer.
	     */
	    اگر (objc != 2)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, تہی);
		err = TCL_ERROR;
		توڑو;
	    }
	    val1 = (صحیح)buf->b_ml.ml_سطر_گنتی;
	    اگر (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_سیٹ_Obj_نتیجہ(interp, resobj);
	    توڑو;

	حالت BUF_نمبر:
	    /*
	     * Get buffer's number.
	     */
	    اگر (objc != 2)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, تہی);
		err = TCL_ERROR;
		توڑو;
	    }
	    resobj = Tcl_NewIntObj((صحیح)buf->b_fnum);
	    Tcl_سیٹ_Obj_نتیجہ(interp, resobj);
	    توڑو;

	حالت BUF_GET:
	    اگر (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		توڑو;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    اگر (err != TCL_OK)
		توڑو;
	    اگر (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		اگر (err != TCL_OK)
		    توڑو;
		اگر (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_Reset_نتیجہ(interp);

		برائے (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    سطر = (حرف *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    اگر (سطر)
			Tcl_AppendElement(interp, سطر);
		    ورنہ
			err = TCL_ERROR;
		}
	    }
	    ورنہ
	    {  // objc == 3
		سطر = (حرف *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_سیٹ_نتیجہ(interp, سطر, TCL_VOLATILE);
	    }
	    توڑو;

	حالت BUF_سیٹ:
	    اگر (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		توڑو;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    اگر (err != TCL_OK)
		واپس TCL_ERROR;
	    اگر (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		سطر = Tcl_GetStringFromObj(objv[3], تہی);
		اگر (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_سیٹ_نتیجہ(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ورنہ
		اگر (ml_replace((linenr_T)val1, (char_u *)سطر, TRUE) != OK)
		{
		    Tcl_سیٹ_نتیجہ(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		ورنہ
		{
		    بدلا_بائٹیں((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		توڑو;
	    }
	    ورنہ
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
		    توڑو;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		اگر (err != TCL_OK)
		    توڑو;
		اگر (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		اگر (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_سیٹ_نتیجہ(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    توڑو;
		}
		flags |= FL_UPDATE_CURBUF;

		برائے (i = 0; i < lc && n <= val2; i++)
		{
		    سطر = Tcl_GetStringFromObj(lv[i], تہی);
		    اگر (ml_replace((linenr_T)n, (char_u *)سطر, TRUE) != OK)
			جاؤ سیٹ_List_Error;
		    ++n;
		}
		اگر (i < lc)
		{
		    // append lines
		    کرو
		    {
			سطر = Tcl_GetStringFromObj(lv[i], تہی);
			اگر (ml_append((linenr_T)(n - 1),
					      (char_u *)سطر, 0, FALSE) != OK)
			    جاؤ سیٹ_List_Error;
			++n;
			++i;
		    } جب (i < lc);
		}
		ورنہ اگر (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    کرو
		    {
			اگر (ml_delete((linenr_T)i) != OK)
			    جاؤ سیٹ_List_Error;
			++n;
		    } جب (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (طویل)MAXLNUM,
								    (طویل)lc);
		بدلا_سطریں((linenr_T)val1, 0, (linenr_T)val2 + 1, (طویل)lc);
		توڑو;
    سیٹ_List_Error:
		u_undo(1);  // ???
		Tcl_سیٹ_نتیجہ(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    توڑو;

	حالت BUF_DELETE:
	    اگر (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		توڑو;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    اگر (err != TCL_OK)
		توڑو;
	    val2 = val1;
	    اگر (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		اگر (err != TCL_OK)
		    واپس err;
		اگر (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    اگر (u_savedel((linenr_T)val1, (طویل)n) != OK)
	    {
		Tcl_سیٹ_نتیجہ(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		توڑو;
	    }
	    برائے (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		اگر (err != TCL_OK)
		    توڑو;
	    }
	    اگر (i > 0)
		deleted_سطریں_mark((linenr_T)val1, (طویل)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    توڑو;

	حالت BUF_MARK:
	    اگر (objc != 3)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, "markName");
		err = TCL_ERROR;
		توڑو;
	    }
	    سطر = Tcl_GetStringFromObj(objv[2], تہی);

	    pos = تہی;
	    اگر (سطر[0] != '\0'  &&  سطر[1] == '\0')
		pos = getmark(سطر[0], FALSE);
	    اگر (pos == تہی)
	    {
		Tcl_سیٹ_نتیجہ(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		توڑو;
	    }
	    err = vimerror(interp);
	    اگر (err != TCL_OK)
		توڑو;
	    اگر (pos->lnum <= 0)
	    {
		Tcl_سیٹ_نتیجہ(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    ورنہ
	    {
		حرف rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (صحیح)row2tcl(pos->lnum), (صحیح)col2tcl(pos->ستون));
		Tcl_سیٹ_نتیجہ(interp, rbuf, TCL_VOLATILE);
	    }
	    توڑو;

	حالت BUF_داخل_کرو:
	    opt = 1;
	    // fallthrough
	حالت BUF_APPEND:
	    اگر (objc != 4)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		توڑو;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    اگر (err != TCL_OK)
		توڑو;
	    اگر (opt)
		--val1;
	    اگر (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_سیٹ_نتیجہ(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		توڑو;
	    }

	    سطر = Tcl_GetStringFromObj(objv[3], تہی);
	    اگر (ml_append((linenr_T)val1, (char_u *)سطر, 0, FALSE) != OK)
	    {
		Tcl_سیٹ_نتیجہ(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		توڑو;
	    }
	    appended_سطریں_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    توڑو;

	حالت BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    اگر (objc != 2)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, تہی);
		err = TCL_ERROR;
		توڑو;
	    }
	    Tcl_Reset_نتیجہ(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		اگر (win->w_بفر == buf)
		{
		    سطر = tclgetwindow(interp, win);
		    اگر (سطر != تہی)
			Tcl_AppendElement(interp, سطر);
		    ورنہ
		    {
			err = TCL_ERROR;
			توڑو;
		    }
		}
	    }
	    توڑو;

	حالت BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    اگر (objc != 3)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, "command");
		err = TCL_ERROR;
		توڑو;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (خالی *)buf, objv[2]);
	    توڑو;

	طےشدہ:
	    Tcl_سیٹ_نتیجہ(interp, _("not implemented yet"), TCL_STATIC);
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

    واپس err;
}

/*
 * This function implements the window commands.
 */
    ساکن صحیح
winselfcmd(
    Client_ڈیٹا	ref,
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحیح		err, idx, flags;
    صحیح		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    حرف	*str;

    شمار
    {
	WIN_بفر, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_اظہاریہ,
	WIN_اونچائی, WIN_OPTION
    };
    ساکن CONST84 حرف *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (حرف *)0
    };

    اگر (objc < 2)
    {
	Tcl_Wrong_عدد_دلائل(interp, 1, objv, "option ?arg ...?");
	واپس TCL_ERROR;
    }

    err = Tcl_Get_اشاریہ_From_Obj(interp, objv[1], winoptions, "option", 0,  &idx);
    اگر (err != TCL_OK)
	واپس TCL_ERROR;

    win = (win_T *)((ساخت ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_بفر;
    flags = 0;

    انتخاب (idx)
    {
	حالت WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    توڑو;

	حالت WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    توڑو;

	حالت WIN_اظہاریہ:
	    err = tclvimexpr(interp, objc, objv, 2);
	    توڑو;

	حالت WIN_اونچائی:
	    اگر (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		اگر (err != TCL_OK)
		    توڑو;
#اگر_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#اگر_ختم
		win_setheight(val1);
		err = vimerror(interp);
		اگر (err != TCL_OK)
		    توڑو;
	    }
	    ورنہ
	    اگر (objc != 2)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		توڑو;
	    }

	    resobj = Tcl_NewIntObj((صحیح)(win->w_اونچائی));
	    Tcl_سیٹ_Obj_نتیجہ(interp, resobj);
	    توڑو;

	حالت WIN_بفر:
	    اگر (objc != 2)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, تہی);
		err = TCL_ERROR;
		توڑو;
	    }
	    str = tclgetbuffer(interp, win->w_بفر);
	    اگر (str)
		Tcl_سیٹ_نتیجہ(interp, str, TCL_VOLATILE);
	    ورنہ
		err = TCL_ERROR;
	    توڑو;

	حالت WIN_DELCMD:
	    اگر (objc != 3)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, "command");
		err = TCL_ERROR;
		توڑو;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (خالی *)win, objv[2]);
	    توڑو;

	حالت WIN_CURSOR:
	    اگر (objc > 4)
	    {
		Tcl_Wrong_عدد_دلائل(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		توڑو;
	    }
	    اگر (objc == 2)
	    {
		حرف buf[64];

		sprintf(buf, _("row %d column %d"), (صحیح)row2tcl(win->w_cursor.lnum), (صحیح)col2tcl(win->w_cursor.ستون));
		Tcl_سیٹ_نتیجہ(interp, buf, TCL_VOLATILE);
		توڑو;
	    }
	    ورنہ اگر (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_پیغام);
		اگر (var == تہی)
		{
		    err = TCL_ERROR;
		    توڑو;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_بفر);
		اگر (err != TCL_OK)
		    توڑو;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_پیغام);
		اگر (var == تہی)
		{
		    err = TCL_ERROR;
		    توڑو;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		اگر (err != TCL_OK)
		    توڑو;
	    }
	    ورنہ
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_بفر);
		اگر (err != TCL_OK)
		    توڑو;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		اگر (err != TCL_OK)
		    توڑو;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.ستون = col2vim(val2);
	    win->w_سیٹ_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    توڑو;

	طےشدہ:
	    Tcl_سیٹ_نتیجہ(interp, _("not implemented yet"), TCL_STATIC);
	    توڑو;
    }

    curwin = savewin;
    curbuf = savebuf;
    اگر (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    واپس err;
}


    ساکن صحیح
commandcmd(
    Client_ڈیٹا	dummy غیر_مستعمل,
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحیح		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    واپس err;
}

    ساکن صحیح
optioncmd(
    Client_ڈیٹا	dummy غیر_مستعمل,
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحیح		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    واپس err;
}

    ساکن صحیح
exprcmd(
    Client_ڈیٹا	dummy غیر_مستعمل,
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[])
{
    واپس tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    ساکن صحیح
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    صحیح		*قدر_P,
    buf_T	*buf)
{
    صحیح err, i;

    شمار { LN_BEGIN, LN_BOTTOM, LN_انتہا, LN_پہلا, LN_آخری, LN_آغاز, LN_چوٹی };

    ساکن CONST84 حرف *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (حرف *)0
    };

    err = Tcl_Get_اشاریہ_From_Obj(interp, obj, keyw, "", 0, &i);
    اگر (err == TCL_OK)
    {
	انتخاب (i)
	{
	    حالت LN_BEGIN:
	    حالت LN_پہلا:
	    حالت LN_آغاز:
	    حالت LN_چوٹی:
		*قدر_P = 1;
		توڑو;
	    حالت LN_BOTTOM:
	    حالت LN_انتہا:
	    حالت LN_آخری:
		*قدر_P = buf->b_ml.ml_سطر_گنتی;
		توڑو;
	}
	واپس TCL_OK;
    }
    Tcl_Reset_نتیجہ(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    اگر (err != TCL_OK)
	واپس err;
    i = row2vim(i);
    اگر (i < 1  ||  i > buf->b_ml.ml_سطر_گنتی)
    {
	Tcl_سیٹ_نتیجہ(interp, _("line number out of range"), TCL_STATIC);
	واپس TCL_ERROR;
    }
    *قدر_P = i;
    واپس TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    ساکن win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	اگر (win->w_بفر == buf)
	    واپس win;
    }
    واپس curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    ساکن صحیح
tcldoexcommand(
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[],
    صحیح		objn)
{
    tcl_info	saveinfo;
    صحیح		err, flag, nobjs;
    حرف	*arg;

    nobjs = objc - objn;
    اگر (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_عدد_دلائل(interp, objn, objv, "?-quiet? exCommand");
	واپس TCL_ERROR;
    }

    flag = 0;
    اگر (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], تہی);
	اگر (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	ورنہ
	{
	    Tcl_Reset_نتیجہ(interp);
	    Tcl_Append_نتیجہ(interp, _("unknown flag: "), arg, (حرف *)0);
	    واپس TCL_ERROR;
	}
	++objn;
    }

    حافظہ_نقل(&saveinfo, &tclinfo, حجم(tcl_info));
    tclinfo.interp = تہی;
    tclinfo.curwin = تہی;
    tclinfo.curbuf = تہی;

    arg = Tcl_GetStringFromObj(objv[objn], تہی);
    اگر (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    اگر (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    اگر (tclinfo.interp)
	tcldelthisinterp();
    حافظہ_نقل(&tclinfo, &saveinfo, حجم(tcl_info));
    tclupdatevars();

    واپس err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    ساکن صحیح
tclsetoption(
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[],
    صحیح		objn)
{
    صحیح		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    طویل	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    شمار { OPT_OFF, OPT_ON, OPT_TOGGLE };
    ساکن CONST84 حرف *optkw[] = { "off", "on", "toggle", (حرف *)0 };

    nobjs = objc - objn;
    اگر (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_عدد_دلائل(interp, objn, objv, "vimOption ?value?");
	واپس TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], تہی);
    ++objn;
    gov = get_option_قدر(option, &lval, &sval, تہی, 0);
    err = TCL_OK;
    انتخاب (gov)
    {
	حالت gov_string:
	    Tcl_سیٹ_نتیجہ(interp, (حرف *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    توڑو;
	حالت gov_bool:
	حالت gov_نمبر:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_سیٹ_Obj_نتیجہ(interp, resobj);
	    توڑو;
	طےشدہ:
	    Tcl_سیٹ_نتیجہ(interp, _("unknown vimOption"), TCL_STATIC);
	    واپس TCL_ERROR;
    }
    اگر (nobjs == 2)
    {
	اگر (gov != gov_string)
	{
	    sval = تہی;    // avoid compiler warning
	    err = Tcl_Get_اشاریہ_From_Obj(interp, objv[objn], optkw, "", 0, &idx);
	    اگر (err != TCL_OK)
	    {
		Tcl_Reset_نتیجہ(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    ورنہ
	    {
		انتخاب (idx)
		{
		    حالت OPT_ON:
			lval = 1;
			توڑو;
		    حالت OPT_OFF:
			lval = 0;
			توڑو;
		    حالت OPT_TOGGLE:
			lval = !lval;
			توڑو;
		}
	    }
	}
	ورنہ
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], تہی);
	اگر (err == TCL_OK)
	{
	    سیٹ_option_قدر_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    واپس err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    ساکن صحیح
tclvimexpr(
    Tcl_Interp	*interp,
    صحیح		objc,
    Tcl_Obj	*CONST objv[],
    صحیح		objn)
{
#اگر_تعریف_شدہ FEAT_EVAL
    حرف	*اظہاریہ, *str;
#اگر_ختم
    صحیح		err;

    اگر (objc - objn != 1)
    {
	Tcl_Wrong_عدد_دلائل(interp, objn, objv, "vimExpr");
	واپس TCL_ERROR;
    }

#اگر_تعریف_شدہ FEAT_EVAL
    اظہاریہ = Tcl_GetStringFromObj(objv[objn], تہی);
    str = (حرف *)eval_مقصد_string((char_u *)اظہاریہ, TRUE, FALSE);
    اگر (str == تہی)
	Tcl_سیٹ_نتیجہ(interp, _("invalid expression"), TCL_STATIC);
    ورنہ
    {
	Tcl_سیٹ_نتیجہ(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#ورنہ
    Tcl_سیٹ_نتیجہ(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#اگر_ختم

    واپس err;
}

/*
 * Check for internal vim errors.
 */
    ساکن صحیح
vimerror(Tcl_Interp *interp)
{
    اگر (got_int)
    {
	Tcl_سیٹ_نتیجہ(interp, _("keyboard interrupt"), TCL_STATIC);
	واپس TCL_ERROR;
    }
    ورنہ اگر (did_emsg)
    {
	Tcl_سیٹ_نتیجہ(interp, _("Vim error"), TCL_STATIC);
	واپس TCL_ERROR;
    }
    واپس TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    ساکن خالی
delref(Client_ڈیٹا cref)
{
    ساخت ref *ref = (ساخت ref *)cref;

    اگر (ref->delcmd)
    {
	Tcl_Decr_Ref_گنتی(ref->delcmd);
	ref->delcmd = تہی;
    }
    ref->interp = تہی;
}

    ساکن حرف *
tclgetref(
    Tcl_Interp	*interp,
    خالی	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    حرف	*سابقہ,	// "win" or "buf"
    خالی	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    ساخت ref *ref, *غیر_مستعمل_2 = تہی;
    ساکن حرف name[VARNAME_سائز];
    Tcl_Command cmd;

    ref = (ساخت ref *)(*refstartP);
    اگر (ref == &refsdeleted)
    {
	Tcl_سیٹ_نتیجہ(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	واپس تہی;
    }

    جب (ref != تہی)
    {
	اگر (ref->interp == interp)
	    توڑو;
	اگر (ref->interp == تہی)
	    غیر_مستعمل_2 = ref;
	ref = ref->اگلا;
    }

    اگر (ref)
	vim_snprintf(name, حجم(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    ورنہ
    {
	اگر (غیر_مستعمل_2)
	    ref = غیر_مستعمل_2;
	ورنہ
	{
	    ref = (ساخت ref *)Tcl_Alloc(حجم(ساخت ref));
	    ref->interp = تہی;
	    ref->اگلا = (ساخت ref *)(*refstartP);
	    (*refstartP) = (خالی *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, حجم(name), "::vim::%s_%lx",
					       سابقہ, (غیرعلامتی طویل)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (Client_ڈیٹا)ref, (Tcl_CmdDeleteProc *)delref);
	اگر (!cmd)
	    واپس تہی;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = تہی;
	ref->vimobj = vimobj;
    }
    واپس name;
}

    ساکن حرف *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    واپس tclgetref(interp, &(win->w_tcl_ref), "win", (خالی *)win, winselfcmd);
}

    ساکن حرف *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    واپس tclgetref(interp, &(buf->b_tcl_ref), "buf", (خالی *)buf, bufselfcmd);
}

    ساکن صحیح
tclsetdelcmd(
    Tcl_Interp	*interp,
    ساخت ref	*reflist,
    خالی	*vimobj,
    Tcl_Obj	*delcmd)
{
    اگر (reflist == &refsdeleted)
    {
	Tcl_سیٹ_نتیجہ(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	واپس TCL_ERROR;
    }

    جب (reflist != تہی)
    {
	اگر (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    اگر (reflist->delcmd)
		Tcl_Decr_Ref_گنتی(reflist->delcmd);
	    Tcl_Incr_Ref_گنتی(delcmd);
	    reflist->delcmd = delcmd;
	    واپس TCL_OK;
	}
	reflist = reflist->اگلا;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_سیٹ_نتیجہ(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    واپس TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    ساکن صحیح
tcl_channel_close(Client_ڈیٹا instance, Tcl_Interp *interp غیر_مستعمل)
{
    صحیح		err = 0;

    // currently does nothing

    اگر (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_سیٹ_Errno(EBADF);
	err = EBADF;
    }
    واپس err;
}

    ساکن صحیح
tcl_channel_داخلہ(
    Client_ڈیٹا	instance غیر_مستعمل,
    حرف	*buf غیر_مستعمل,
    صحیح		bufsiz غیر_مستعمل,
    صحیح		*errptr)
{

    // input is currently not supported

    Tcl_سیٹ_Errno(EINVAL);
    اگر (errptr)
	*errptr = EINVAL;
    واپس -1;
}

    ساکن صحیح
tcl_channel_output(
    Client_ڈیٹا	instance,
    ثابت حرف	*buf,
    صحیح		bufsiz,
    صحیح		*errptr)
{
    char_u	*str;
    صحیح		نتیجہ;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    اگر (!str)
    {
	Tcl_سیٹ_Errno(ENOMEM);
	اگر (errptr)
	    *errptr = ENOMEM;
	واپس -1;
    }

    نتیجہ = bufsiz;
    اگر (instance == VIMOUT)
	tclmsg((حرف *)str);
    ورنہ
    اگر (instance == VIMERR)
	tclerrmsg((حرف *)str);
    ورنہ
    {
	Tcl_سیٹ_Errno(EBADF);
	اگر (errptr)
	    *errptr = EBADF;
	نتیجہ = -1;
    }
    vim_free(str);
    واپس نتیجہ;
}

    ساکن خالی
tcl_channel_watch(Client_ڈیٹا instance غیر_مستعمل, صحیح نقاب غیر_مستعمل)
{
    Tcl_سیٹ_Errno(EINVAL);
}

    ساکن صحیح
tcl_channel_gethandle(
    Client_ڈیٹا	instance غیر_مستعمل,
    صحیح		direction غیر_مستعمل,
    Client_ڈیٹا	*handleptr غیر_مستعمل)
{
    Tcl_سیٹ_Errno(EINVAL);
    واپس EINVAL;
}


ساکن Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_نسخہ_2, //نسخہ
    tcl_channel_close,	// closeProc
    tcl_channel_داخلہ,	// inputProc
    tcl_channel_output,	// outputProc
    تہی,		// seekProc
    تہی,		// setOptionProc
    تہی,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    تہی,		// close2Proc
    تہی,		// blockModeProc
#اگر_تعریف_شدہ TCL_CHANNEL_نسخہ_2
    تہی,		// flushProc
    تہی,		// handlerProc
#اگر_ختم
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#اگر_تعریف_شدہ TCL_CHANNEL_نسخہ_3
    تہی,		// wideSeekProc
#اگر_ختم
#اگر_تعریف_شدہ TCL_CHANNEL_نسخہ_4
    تہی,		// threadActionProc
#اگر_ختم
#اگر_تعریف_شدہ TCL_CHANNEL_نسخہ_5
    تہی		// truncateProc
#اگر_ختم
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    ساکن خالی
tclupdatevars(خالی)
{
    حرف varname[VARNAME_سائز];	// must be writeable
    حرف *name;

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


    ساکن صحیح
tclinit(exarg_T *eap)
{
    حرف varname[VARNAME_سائز];	// Tcl_LinkVar requires writeable varname
    حرف *name;

#اگر_تعریف_شدہ DYNAMIC_TCL
    اگر (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	واپس FAIL;
    }
#اگر_ختم

    اگر (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	ساکن Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_سیٹ_Std_Channel(ch1, TCL_STDOUT);
	Tcl_سیٹ_Std_Channel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	اگر (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    واپس FAIL;
	}
#اگر 0
	// VIM sure is interactive
	Tcl_سیٹ_Var(interp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
#اگر_ختم

	Tcl_سیٹ_Channel_Option(interp, ch1, "-buffering", "line");
#اگر_تعریف_شدہ MSWIN
	Tcl_سیٹ_Channel_Option(interp, ch1, "-translation", "lf");
#اگر_ختم
	Tcl_سیٹ_Channel_Option(interp, ch2, "-buffering", "line");
#اگر_تعریف_شدہ MSWIN
	Tcl_سیٹ_Channel_Option(interp, ch2, "-translation", "lf");
#اگر_ختم

	// replace standard Tcl exit command
	Tcl_DeleteCommand(interp, "exit");
	Tcl_CreateObjCommand(interp, "exit", exitcmd,
	    (Client_ڈیٹا)تہی, (Tcl_CmdDeleteProc *)تہی);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (Client_ڈیٹا)تہی, (Tcl_CmdDeleteProc *)تہی);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (Client_ڈیٹا)تہی, (Tcl_CmdDeleteProc *)تہی);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (Client_ڈیٹا)تہی, (Tcl_CmdDeleteProc *)تہی);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (Client_ڈیٹا)تہی, (Tcl_CmdDeleteProc *)تہی);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (Client_ڈیٹا)تہی, (Tcl_CmdDeleteProc *)تہی);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (Client_ڈیٹا)تہی, (Tcl_CmdDeleteProc *)تہی);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_آغاز = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.range_آغاز, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.range_آغاز, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_انتہا   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.range_انتہا, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_سائز);
	tclinfo.curwin = Tcl_Alloc(VARNAME_سائز);
	name = tclgetbuffer(interp, curbuf);
	strcpy(tclinfo.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
	name = tclgetwindow(interp, curwin);
	strcpy(tclinfo.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

	tclinfo.interp = interp;
    }
    ورنہ
    {
	// Interpreter already exists, just update variables
	tclinfo.range_آغاز = row2tcl(eap->line1);
	tclinfo.range_انتہا = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    واپس OK;
}

    ساکن خالی
tclerrmsg(حرف *متن)
{
    حرف *اگلا;

    جب ((اگلا=strchr(متن, '\n')))
    {
	*اگلا++ = '\0';
	emsg(متن);
	متن = اگلا;
    }
    اگر (*متن)
	emsg(متن);
}

    ساکن خالی
tclmsg(حرف *متن)
{
    حرف *اگلا;

    جب ((اگلا=strchr(متن, '\n')))
    {
	*اگلا++ = '\0';
	پیغام(متن);
	متن = اگلا;
    }
    اگر (*متن)
	پیغام(متن);
}

    ساکن خالی
tcldelthisinterp(خالی)
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

    tclinfo.interp = تہی;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = تہی;
}

    ساکن صحیح
tclexit(صحیح error)
{
    صحیح newerr = OK;

    اگر (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#اگر (TCL_MAJOR_نسخہ == 8 && TCL_MINOR_نسخہ >= 5) || TCL_MAJOR_نسخہ > 8
	|| Tcl_حد_بندی_Exceeded(tclinfo.interp)  // True if the interpreter cannot continue
#اگر_ختم
	)
    {
	حرف buf[50];

	sprintf(buf, _(e_exit_رمز_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	اگر (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	ورنہ
	    newerr = FAIL;

	tcldelthisinterp();
    }
    ورنہ
    {
	حرف *نتیجہ;

	نتیجہ = (حرف *)Tcl_Get_String_نتیجہ(tclinfo.interp);
	اگر (error == TCL_OK)
	{
	    tclmsg(نتیجہ);
	    newerr = OK;
	}
	ورنہ
	{
	    tclerrmsg(نتیجہ);
	    newerr = FAIL;
	}
    }

    واپس newerr;
}

/*
 * ":tcl"
 */
    خالی
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
	    اگر (script == تہی)
		err = Tcl_Eval(tclinfo.interp, (حرف *)eap->arg);
	    ورنہ
		err = Tcl_Eval(tclinfo.interp, (حرف *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    خالی
ex_tclfile(exarg_T *eap)
{
    حرف *file = (حرف *)eap->arg;
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
    خالی
ex_tcldo(exarg_T *eap)
{
    حرف	*script, *سطر;
    صحیح		err, rs, re, lnum;
    حرف	var_lnum[VARNAME_سائز]; // must be writeable memory
    حرف	var_سطر[VARNAME_سائز];
    linenr_T	پہلا_سطر = 0;
    linenr_T	آخری_سطر = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (حرف *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_سطر, VAR_CURLINE);

    err = tclinit(eap);
    اگر (err != OK)
	واپس;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (حرف *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    اگر (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_سیٹ_نتیجہ(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    جب (err == TCL_OK  &&  rs <= re)
    {
	اگر ((linenr_T)rs > curbuf->b_ml.ml_سطر_گنتی)
	    توڑو;
	سطر = (حرف *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	اگر (!سطر)
	{
	    Tcl_سیٹ_نتیجہ(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    توڑو;
	}
	Tcl_سیٹ_Var(tclinfo.interp, var_سطر, سطر, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	اگر (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#اگر (TCL_MAJOR_نسخہ == 8 && TCL_MINOR_نسخہ >= 5) || TCL_MAJOR_نسخہ > 8
	    || Tcl_حد_بندی_Exceeded(tclinfo.interp)
#اگر_ختم
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_سطر_گنتی)
	    توڑو;
	سطر = (حرف *)Tcl_GetVar(tclinfo.interp, var_سطر, 0);
	اگر (سطر)
	{
	    اگر (ml_replace((linenr_T)rs, (char_u *)سطر, TRUE) != OK)
	    {
		Tcl_سیٹ_نتیجہ(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		توڑو;
	    }
	    اگر (پہلا_سطر == 0)
		پہلا_سطر = rs;
	    آخری_سطر = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    اگر (پہلا_سطر)
	بدلا_سطریں(پہلا_سطر, 0, آخری_سطر + 1, (طویل)0);

    Tcl_UnsetVar(tclinfo.interp, var_سطر, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    اگر (err == TCL_OK)
	Tcl_Reset_نتیجہ(tclinfo.interp);

    (خالی)tclexit(err);
}

    ساکن خالی
tcldelallrefs(ساخت ref *ref)
{
    ساخت ref	*اگلا;
    صحیح		err;
    حرف	*نتیجہ;

#اگر_تعریف_شدہ DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    اگر (exiting)
	واپس;
#اگر_ختم

    جب (ref != تہی)
    {
	اگلا = ref->اگلا;
	اگر (ref->interp)
	{
	    اگر (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		اگر (err != TCL_OK)
		{
		    نتیجہ = (حرف *)Tcl_Get_String_نتیجہ(ref->interp);
		    اگر (نتیجہ)
			tclerrmsg(نتیجہ);
		}
		Tcl_Decr_Ref_گنتی(ref->delcmd);
		ref->delcmd = تہی;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((حرف *)ref);
	ref = اگلا;
    }
}

    خالی
tcl_بفر_free(buf_T *buf)
{
    ساخت ref *reflist;

#اگر_تعریف_شدہ DYNAMIC_TCL
    اگر (!stubs_initialized)	// Not using Tcl, nothing to do.
	واپس;
#اگر_ختم

    reflist = (ساخت ref *)(buf->b_tcl_ref);
    اگر (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (خالی *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = تہی;
    }
}

    خالی
tcl_کھڑکی_free(win_T *win)
{
    ساخت ref *reflist;

#اگر_تعریف_شدہ DYNAMIC_TCL
    اگر (!stubs_initialized)	// Not using Tcl, nothing to do.
	واپس;
#اگر_ختم

    reflist = (ساخت ref*)(win->w_tcl_ref);
    اگر (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (خالی *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = تہی;
    }
}

// The End
