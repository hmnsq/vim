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

#ضمن "vim.h"
#تعریف_لغوه EXTERN			// tcl.h defines it too

#که_تعریف_شوی DYNAMIC_TCL
# define USE_TCL_STUBS // use tcl's stubs mechanism
#د_که_پای

#ضمن <tcl.h>
#ضمن <string.h>

تعريفنوع بنية
{
    Tcl_Interp *interp;
    صحيح exitvalue;
    صحيح range_پیل, range_پای_ټکی;
    صحيح lbase;
    حرف *curbuf, *curwin;
} tcl_info;

ساكن tcl_info tclinfo = { عدم, 0, 0, 0, 0, عدم, عدم };

#تعریف_کړه VAR_RANGE1	"::vim::range(start)"
#تعریف_کړه VAR_RANGE2	"::vim::range(begin)"
#تعریف_کړه VAR_RANGE3	"::vim::range(end)"
#تعریف_کړه VAR_CURBUF	"::vim::current(buffer)"
#تعریف_کړه VAR_CURWIN	"::vim::current(window)"
#تعریف_کړه VAR_LBASE	"::vim::lbase"
#تعریف_کړه VAR_CURLINE	"line"
#تعریف_کړه VAR_CURLNUM	"lnum"
#تعریف_کړه VARNAME_اندازه	64

#تعریف_کړه row2tcl(x)  ((x) - (tclinfo.lbase==0))
#تعریف_کړه row2vim(x)  ((x) + (tclinfo.lbase==0))
#تعریف_کړه col2tcl(x)  ((x) + (tclinfo.lbase!=0))
#تعریف_کړه col2vim(x)  ((x) - (tclinfo.lbase!=0))


#تعریف_کړه VIMOUT	((Client_معلومات)1)
#تعریف_کړه VIMERR	((Client_معلومات)2)

// This appears to be new in Tcl 8.4.
#که_نه_تعریف_شوی CONST84
# define CONST84
#د_که_پای

/*
 *  List of Tcl interpreters who reference a vim window or buffer.
 *  Each buffer and window has its own list in the w_tcl_ref or b_tcl_ref
 *  struct member.  We need this because Tcl can create sub-interpreters with
 *  the "interp" command, and each interpreter can reference all windows and
 *  buffers.
 */
بنية ref
{
    بنية ref	*بل;

    Tcl_Interp	*interp;
    Tcl_Command cmd;	    // Tcl command that represents this object
    Tcl_Obj	*delcmd;    // Tcl command to call when object is being del.
    فراغ	*vimobj;    // Vim window or buffer (win_T* or buf_T*)
};
ساكن حرف * tclgetbuffer ANSI_دلیلونه((Tcl_Interp *interp, buf_T *buf));
ساكن حرف * tclgetwindow ANSI_دلیلونه((Tcl_Interp *interp, win_T *win));
ساكن صحيح tclsetdelcmd ANSI_دلیلونه((Tcl_Interp *interp, بنية ref *reflist, فراغ *vimobj, Tcl_Obj *delcmd));
ساكن صحيح tclgetlinenum ANSI_دلیلونه ((Tcl_Interp *interp, Tcl_Obj *obj, صحيح *ارزښت_P, buf_T *buf));
ساكن win_T *tclfindwin ANSI_دلیلونه ((buf_T *buf));
ساكن صحيح tcldoexcommand ANSI_دلیلونه ((Tcl_Interp *interp, صحيح objc, Tcl_Obj *CONST objv[], صحيح objn));
ساكن صحيح tclsetoption ANSI_دلیلونه ((Tcl_Interp *interp, صحيح objc, Tcl_Obj *CONST objv[], صحيح objn));
ساكن صحيح tclvimexpr ANSI_دلیلونه ((Tcl_Interp *interp, صحيح objc, Tcl_Obj *CONST objv[], صحيح objn));
ساكن فراغ tcldelthisinterp ANSI_دلیلونه ((فراغ));

ساكن صحيح vimerror ANSI_دلیلونه((Tcl_Interp *interp));
ساكن فراغ tclmsg ANSI_دلیلونه((حرف *متن));
ساكن فراغ tclerrmsg ANSI_دلیلونه((حرف *متن));
ساكن فراغ tclupdatevars ANSI_دلیلونه((فراغ));

ساكن بنية ref refsdeleted;	// dummy object for deleted ref list

//////////////////////////////////////////////////////////////////////////////
// TCL interface manager
////////////////////////////////////////////////////////////////////////////

#إذا defined(DYNAMIC_TCL)
# ifndef DYNAMIC_TCL_DLL
#  define DYNAMIC_TCL_DLL "tcl83.dll"
# endif
# ifndef DYNAMIC_TCL_VER
#  define DYNAMIC_TCL_VER "8.3"
# endif

# ifndef  DYNAMIC_TCL // Just generating prototypes
تعريفنوع صحيح HANDLE;
# endif

# ifdef MSWIN
#  define TCL_PROC FARPROC
#  define بارول_dll vim_بارول_Lib
#  define نښه_from_dll Get_Proc_پته
#  define close_dll FreeLibrary
#  define بارول_dll_error GetWin32Error
# وإلا
#  include <dlfcn.h>
#  define HANDLE فراغ*
#  define TCL_PROC فراغ*
#  define بارول_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define نښه_from_dll dlsym
#  define close_dll dlclose
#  define بارول_dll_error dlerror
# endif

/*
 * Declare HANDLE for tcl.dll and function pointers.
 */
ساكن HANDLE hTclLib = عدم;
Tcl_Interp* (*dll_Tcl_CreateInterp)(فراغ);
فراغ (*dll_Tcl_پیدا_کړه_Executable)(ثابت فراغ *);

/*
 * Table of name to function pointer of tcl.
 */
ساكن بنية {
    حرف* name;
    TCL_PROC* ptr;
} tcl_funcname_جدول[] = {
    {"Tcl_CreateInterp", (TCL_PROC*)&dll_Tcl_CreateInterp},
    {"Tcl_FindExecutable", (TCL_PROC*)&dll_Tcl_پیدا_کړه_Executable},
    {عدم, عدم},
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
    ساكن صحيح
tcl_runtime_link_init(حرف *libname, صحيح verbose)
{
    صحيح i;

    إذا (hTclLib)
	أرجع OK;
    إذا (!(hTclLib = بارول_dll(libname)))
    {
	إذا (verbose)
	    semsg(_(e_could_not_بارول_library_str_str), libname, بارول_dll_error());
	أرجع FAIL;
    }
    لكل (i = 0; tcl_funcname_جدول[i].ptr; ++i)
    {
	إذا (!(*tcl_funcname_جدول[i].ptr = نښه_from_dll(hTclLib,
						  tcl_funcname_جدول[i].name)))
	{
	    close_dll(hTclLib);
	    hTclLib = عدم;
	    إذا (verbose)
		semsg(_(e_could_not_بارول_library_function_str),
						   tcl_funcname_جدول[i].name);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}
#د_که_پای // defined(DYNAMIC_TCL)

#که_تعریف_شوی DYNAMIC_TCL
ساكن حرف *پیدا_کړه_executable_arg = عدم;
#د_که_پای

    فراغ
vim_tcl_init(حرف *arg)
{
#که_تعریف_شوی DYNAMIC_TCL
    پیدا_کړه_executable_arg = arg;
#وإلا
    Tcl_پیدا_کړه_Executable(arg);
#د_که_پای
}

#إذا defined(DYNAMIC_TCL)

ساكن صحيح stubs_initialized = FALSE;

/*
 * Return TRUE if the TCL interface can be used.
 */
    صحيح
tcl_enabled(صحيح verbose)
{
    إذا (!stubs_initialized && پیدا_کړه_executable_arg != عدم
	    && tcl_runtime_link_init((حرف *)p_tcldll, verbose) == OK)
    {
	Tcl_Interp *interp;

	// Note: the library will allocate memory to store the executable name,
	// which will be reported as possibly leaked by valgrind.
	dll_Tcl_پیدا_کړه_Executable(پیدا_کړه_executable_arg);

	إذا ((interp = dll_Tcl_CreateInterp()) != عدم)
	{
	    إذا (Tcl_InitStubs(interp, DYNAMIC_TCL_VER, 0) != عدم)
	    {
		Tcl_DeleteInterp(interp);
		stubs_initialized = TRUE;
	    }
	    // FIXME: When Tcl_InitStubs() was failed, how delete interp?
	}
    }
    أرجع stubs_initialized;
}
#د_که_پای

#إذا defined(EXITFREE)
/*
 * Called once when exiting.
 */
    فراغ
vim_tcl_finalize(فراغ)
{
# ifdef DYNAMIC_TCL
    إذا (stubs_initialized)
# endif
	Tcl_Finalize();
}
#د_که_پای

    فراغ
tcl_پای_ټکی(فراغ)
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
    ساكن صحيح
exitcmd(
    Client_معلومات dummy ناکارول_شوی,
    Tcl_Interp *interp,
    صحيح objc,
    Tcl_Obj *CONST objv[])
{
    صحيح ارزښت = 0;

    بدل (objc)
    {
	حالة 2:
	    إذا (Tcl_GetIntFromObj(interp, objv[1], &ارزښت) != TCL_OK)
		اكسر;
	    // FALLTHROUGH
	حالة 1:
	    tclinfo.exitvalue = ارزښت;

	    Tcl_DeleteInterp(interp);
	    اكسر;
	افتراضي:
	    Tcl_Wrong_عدد_دلیلونه(interp, 1, objv, "?returnCode?");
    }
    أرجع TCL_ERROR;
}

/*
 *  "::vim::beep" - what Vi[m] does best :-)
 */
    ساكن صحيح
beepcmd(
    Client_معلومات dummy ناکارول_شوی,
    Tcl_Interp *interp,
    صحيح objc,
    Tcl_Obj *CONST objv[])
{
    إذا (objc != 1)
    {
	Tcl_Wrong_عدد_دلیلونه(interp, 1, objv, عدم);
	أرجع TCL_ERROR;
    }
    vim_beep(BO_LANG);
    أرجع TCL_OK;
}

/*
 *  "::vim::buffer list" - create a list of buffer commands.
 *  "::vim::buffer {N}" - create buffer command for buffer N.
 *  "::vim::buffer exists {N}" - test if buffer N exists.
 *  "::vim::buffer new" - create a new buffer (not implemented)
 */
    ساكن صحيح
buffercmd(
    Client_معلومات dummy ناکارول_شوی,
    Tcl_Interp *interp,
    صحيح objc,
    Tcl_Obj *CONST objv[])
{
    حرف	*name;
    buf_T	*buf;
    Tcl_Obj	*resobj;
    صحيح		err, n, idx;
    تعداد {BCMD_EXISTS, BCMD_LIST};
    ساكن CONST84 حرف *bcmdoptions[] =
    {
	"exists", "list", (حرف *)0
    };

    إذا (objc < 2)
    {
	Tcl_Wrong_عدد_دلیلونه(interp, 1, objv, "option");
	أرجع TCL_ERROR;
    }
    err = Tcl_GetIntFromObj(interp, objv[1], &n);
    إذا (err == TCL_OK)
    {
	إذا (objc != 2)
	{
	    Tcl_Wrong_عدد_دلیلونه(interp, 1, objv, "bufNumber");
	    أرجع TCL_ERROR;
	}
	FOR_ALL_BUFFERS(buf)
	{
	    إذا (buf->b_fnum == n)
	    {
		name = tclgetbuffer(interp, buf);
		إذا (name == عدم)
		    أرجع TCL_ERROR;
		Tcl_سېټ_پایله(interp, name, TCL_VOLATILE);
		أرجع TCL_OK;
	    }
	}
	Tcl_سېټ_پایله(interp, _("invalid buffer number"), TCL_STATIC);
	أرجع TCL_ERROR;
    }
    Tcl_Reset_پایله(interp); // clear error from Tcl_GetIntFromObj

    err = Tcl_Get_شاخص_From_Obj(interp, objv[1], bcmdoptions, "option", 0, &idx);
    إذا (err != TCL_OK)
	أرجع err;
    بدل (idx)
    {
	حالة BCMD_LIST:
	    إذا (objc != 2)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, "");
		err = TCL_ERROR;
		اكسر;
	    }
	    FOR_ALL_BUFFERS(buf)
	    {
		name = tclgetbuffer(interp, buf);
		إذا (name == عدم)
		{
		    err = TCL_ERROR;
		    اكسر;
		}
		Tcl_AppendElement(interp, name);
	    }
	    اكسر;

	حالة BCMD_EXISTS:
	    إذا (objc != 3)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, "bufNumber");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = Tcl_GetIntFromObj(interp, objv[2], &n);
	    إذا (err == TCL_OK)
	    {
		buf = buflist_findnr(n);
		resobj = Tcl_NewIntObj(buf != عدم);
		Tcl_سېټ_Obj_پایله(interp, resobj);
	    }
	    اكسر;

	افتراضي:
	    Tcl_سېټ_پایله(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }
    أرجع err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
    ساكن صحيح
windowcmd(
    Client_معلومات	dummy ناکارول_شوی,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    حرف	*what, *string;
    win_T	*win;

    إذا (objc != 2)
    {
	Tcl_Wrong_عدد_دلیلونه(interp, 1, objv, "option");
	أرجع TCL_ERROR;
    }
    what = Tcl_GetStringFromObj(objv[1], عدم);
    إذا (strcmp(what, "list") == 0)
    {
	FOR_ALL_WINDOWS(win)
	{
	    string = tclgetwindow(interp, win);
	    إذا (string == عدم)
		أرجع TCL_ERROR;
	    Tcl_AppendElement(interp, string);
	}
	أرجع TCL_OK;
    }
    Tcl_سېټ_پایله(interp, _("unknown option"), TCL_STATIC);
    أرجع TCL_ERROR;
}

/*
 * flags for bufselfcmd and winselfcmd to indicate outstanding actions.
 */
#تعریف_کړه FL_UPDATE_SCREEN	(1<<0)
#تعریف_کړه FL_UPDATE_CURBUF	(1<<1)
#تعریف_کړه FL_ADJUST_CURSOR	(1<<2)

/*
 * This function implements the buffer commands.
 */
    ساكن صحيح
bufselfcmd(
    Client_معلومات	ref,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحيح		opt, err, idx, flags;
    صحيح		val1, val2, n, i;
    buf_T	*buf, *savebuf;
    win_T	*win, *savewin;
    Tcl_Obj	*resobj;
    pos_T	*pos;
    حرف	*کرښه;

    تعداد
    {
	BUF_APPEND, BUF_COMMAND, BUF_شمېر, BUF_DELCMD, BUF_DELETE, BUF_بیان,
	BUF_GET, BUF_ورننباسه, BUF_وروستی, BUF_MARK, BUF_NAME, BUF_شمېره,
	BUF_OPTION, BUF_سېټ, BUF_WINDOWS
    };
    ساكن CONST84 حرف *bufoptions[] =
    {
	"append", "command", "count", "delcmd", "delete", "expr",
	"get", "insert", "last", "mark", "name", "number",
	"option", "set", "windows", (حرف *)0
    };

    إذا (objc < 2)
    {
	Tcl_Wrong_عدد_دلیلونه(interp, 1, objv, "option ?arg ...?");
	أرجع TCL_ERROR;
    }

    err = Tcl_Get_شاخص_From_Obj(interp, objv[1], bufoptions, "option", 0, &idx);
    إذا (err != TCL_OK)
	أرجع err;

    buf = (buf_T *)((بنية ref *)ref)->vimobj;
    savebuf = curbuf;  curbuf = buf;
    savewin = curwin;  curwin = tclfindwin(buf);
    flags = 0;
    opt = 0;

    بدل (idx)
    {
	حالة BUF_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	حالة BUF_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	حالة BUF_بیان:
	    err = tclvimexpr(interp, objc, objv, 2);
	    اكسر;

	حالة BUF_NAME:
	    /*
	     *	Get filename of buffer.
	     */
	    إذا (objc != 2)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, عدم);
		err = TCL_ERROR;
		اكسر;
	    }
	    إذا (buf->b_ffname)
		Tcl_سېټ_پایله(interp, (حرف *)buf->b_ffname, TCL_VOLATILE);
	    وإلا
		Tcl_سېټ_پایله(interp, "", TCL_STATIC);
	    اكسر;

	حالة BUF_وروستی:
	    /*
	     * Get line number of last line.
	     */
	    opt = 1;
	    // fallthrough
	حالة BUF_شمېر:
	    /*
	     * Get number of lines in buffer.
	     */
	    إذا (objc != 2)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, عدم);
		err = TCL_ERROR;
		اكسر;
	    }
	    val1 = (صحيح)buf->b_ml.ml_کرښه_شمېر;
	    إذا (opt)
		val1 = row2tcl(val1);

	    resobj = Tcl_NewIntObj(val1);
	    Tcl_سېټ_Obj_پایله(interp, resobj);
	    اكسر;

	حالة BUF_شمېره:
	    /*
	     * Get buffer's number.
	     */
	    إذا (objc != 2)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, عدم);
		err = TCL_ERROR;
		اكسر;
	    }
	    resobj = Tcl_NewIntObj((صحيح)buf->b_fnum);
	    Tcl_سېټ_Obj_پایله(interp, resobj);
	    اكسر;

	حالة BUF_GET:
	    إذا (objc != 3 && objc != 4)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    إذا (err != TCL_OK)
		اكسر;
	    إذا (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		إذا (err != TCL_OK)
		    اكسر;
		إذا (val1 > val2)
		{
		    n = val1; val1 = val2; val2 = n;
		}
		Tcl_Reset_پایله(interp);

		لكل (n = val1; n <= val2 && err == TCL_OK; n++)
		{
		    کرښه = (حرف *)ml_get_buf(buf, (linenr_T)n, FALSE);
		    إذا (کرښه)
			Tcl_AppendElement(interp, کرښه);
		    وإلا
			err = TCL_ERROR;
		}
	    }
	    وإلا
	    {  // objc == 3
		کرښه = (حرف *)ml_get_buf(buf, (linenr_T)val1, FALSE);
		Tcl_سېټ_پایله(interp, کرښه, TCL_VOLATILE);
	    }
	    اكسر;

	حالة BUF_سېټ:
	    إذا (objc != 4 && objc != 5)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    إذا (err != TCL_OK)
		أرجع TCL_ERROR;
	    إذا (objc == 4)
	    {
		/*
		 *  Replace one line with a string.
		 *	$buf set {n} {string}
		 */
		کرښه = Tcl_GetStringFromObj(objv[3], عدم);
		إذا (u_savesub((linenr_T)val1) != OK)
		{
		    Tcl_سېټ_پایله(interp, _("cannot save undo information"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		وإلا
		إذا (ml_replace((linenr_T)val1, (char_u *)کرښه, TRUE) != OK)
		{
		    Tcl_سېټ_پایله(interp, _("cannot replace line"), TCL_STATIC);
		    err = TCL_ERROR;
		}
		وإلا
		{
		    بدل_شوی_بایټونه((linenr_T)val1, 0);
		    flags |= FL_UPDATE_CURBUF;
		}
		اكسر;
	    }
	    وإلا
	    {
		/*
		 * Replace several lines with the elements of a Tcl list.
		 *	$buf set {n} {m} {list}
		 * If the list contains more than {m}-{n}+1 elements, they
		 * are * inserted after line {m}.  If the list contains fewer
		 * elements, * the lines from {n}+length({list}) through {m}
		 * are deleted.
		 */
		صحيح	    lc;
		Tcl_Obj	    **lv;

		err = tclgetlinenum(interp, objv[3], &val2, buf);
		إذا (err != TCL_OK)
		    اكسر;
		err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
		إذا (err != TCL_OK)
		    اكسر;
		إذا (val1 > val2)
		{
		    n = val1;
		    val1 = val2;
		    val2 = n;
		}

		n = val1;
		إذا (u_save((linenr_T)(val1 - 1), (linenr_T)(val2 + 1)) != OK)
		{
		    Tcl_سېټ_پایله(interp, _("cannot save undo information"),
								  TCL_STATIC);
		    err = TCL_ERROR;
		    اكسر;
		}
		flags |= FL_UPDATE_CURBUF;

		لكل (i = 0; i < lc && n <= val2; i++)
		{
		    کرښه = Tcl_GetStringFromObj(lv[i], عدم);
		    إذا (ml_replace((linenr_T)n, (char_u *)کرښه, TRUE) != OK)
			اذهب سېټ_List_Error;
		    ++n;
		}
		إذا (i < lc)
		{
		    // append lines
		    افعل
		    {
			کرښه = Tcl_GetStringFromObj(lv[i], عدم);
			إذا (ml_append((linenr_T)(n - 1),
					      (char_u *)کرښه, 0, FALSE) != OK)
			    اذهب سېټ_List_Error;
			++n;
			++i;
		    } طالما (i < lc);
		}
		وإلا إذا (n <= val2)
		{
		    // did not replace all lines, delete
		    i = n;
		    افعل
		    {
			إذا (ml_delete((linenr_T)i) != OK)
			    اذهب سېټ_List_Error;
			++n;
		    } طالما (n <= val2);
		}
		lc -= val2 - val1 + 1;	// number of lines to be replaced
		mark_adjust((linenr_T)val1, (linenr_T)val2, (اوږد)MAXLNUM,
								    (اوږد)lc);
		بدل_شوی_کرښې((linenr_T)val1, 0, (linenr_T)val2 + 1, (اوږد)lc);
		اكسر;
    سېټ_List_Error:
		u_undo(1);  // ???
		Tcl_سېټ_پایله(interp, _("cannot set line(s)"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    اكسر;

	حالة BUF_DELETE:
	    إذا (objc != 3  &&  objc != 4)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 3, objv, "lineNumber ?lineNumber?");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    إذا (err != TCL_OK)
		اكسر;
	    val2 = val1;
	    إذا (objc == 4)
	    {
		err = tclgetlinenum(interp, objv[3], &val2, buf);
		إذا (err != TCL_OK)
		    أرجع err;
		إذا (val1 > val2)
		{
		    i = val1; val1 = val2; val2 = i;
		}
	    }
	    n = val2 - val1 + 1;
	    إذا (u_savedel((linenr_T)val1, (اوږد)n) != OK)
	    {
		Tcl_سېټ_پایله(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		اكسر;
	    }
	    لكل (i = 0; i < n; i++)
	    {
		ml_delete((linenr_T)val1);
		err = vimerror(interp);
		إذا (err != TCL_OK)
		    اكسر;
	    }
	    إذا (i > 0)
		deleted_کرښې_mark((linenr_T)val1, (اوږد)i);
	    flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
	    اكسر;

	حالة BUF_MARK:
	    إذا (objc != 3)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, "markName");
		err = TCL_ERROR;
		اكسر;
	    }
	    کرښه = Tcl_GetStringFromObj(objv[2], عدم);

	    pos = عدم;
	    إذا (کرښه[0] != '\0'  &&  کرښه[1] == '\0')
		pos = getmark(کرښه[0], FALSE);
	    إذا (pos == عدم)
	    {
		Tcl_سېټ_پایله(interp, _("invalid mark name"), TCL_STATIC);
		err = TCL_ERROR;
		اكسر;
	    }
	    err = vimerror(interp);
	    إذا (err != TCL_OK)
		اكسر;
	    إذا (pos->lnum <= 0)
	    {
		Tcl_سېټ_پایله(interp, _("mark not set"), TCL_STATIC);
		err = TCL_ERROR;
	    }
	    وإلا
	    {
		حرف rbuf[64];

		sprintf(rbuf, _("row %d column %d"),
			     (صحيح)row2tcl(pos->lnum), (صحيح)col2tcl(pos->ستون));
		Tcl_سېټ_پایله(interp, rbuf, TCL_VOLATILE);
	    }
	    اكسر;

	حالة BUF_ورننباسه:
	    opt = 1;
	    // fallthrough
	حالة BUF_APPEND:
	    إذا (objc != 4)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, "lineNum text");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclgetlinenum(interp, objv[2], &val1, buf);
	    إذا (err != TCL_OK)
		اكسر;
	    إذا (opt)
		--val1;
	    إذا (u_save((linenr_T)val1, (linenr_T)(val1+1)) != OK)
	    {
		Tcl_سېټ_پایله(interp, _("cannot save undo information"),
								  TCL_STATIC);
		err = TCL_ERROR;
		اكسر;
	    }

	    کرښه = Tcl_GetStringFromObj(objv[3], عدم);
	    إذا (ml_append((linenr_T)val1, (char_u *)کرښه, 0, FALSE) != OK)
	    {
		Tcl_سېټ_پایله(interp, _("cannot insert/append line"),
								  TCL_STATIC);
		err = TCL_ERROR;
		اكسر;
	    }
	    appended_کرښې_mark((linenr_T)val1, 1L);
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	حالة BUF_WINDOWS:
	    /*
	     * Return list of window commands.
	     */
	    إذا (objc != 2)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, عدم);
		err = TCL_ERROR;
		اكسر;
	    }
	    Tcl_Reset_پایله(interp);
	    FOR_ALL_WINDOWS(win)
	    {
		إذا (win->w_بفر == buf)
		{
		    کرښه = tclgetwindow(interp, win);
		    إذا (کرښه != عدم)
			Tcl_AppendElement(interp, کرښه);
		    وإلا
		    {
			err = TCL_ERROR;
			اكسر;
		    }
		}
	    }
	    اكسر;

	حالة BUF_DELCMD:
	    /*
	     * Register deletion callback.
	     * TODO: Should be able to register multiple callbacks
	     */
	    إذا (objc != 3)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, "command");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclsetdelcmd(interp, buf->b_tcl_ref, (فراغ *)buf, objv[2]);
	    اكسر;

	افتراضي:
	    Tcl_سېټ_پایله(interp, _("not implemented yet"), TCL_STATIC);
	    err = TCL_ERROR;
    }

    إذا (flags & FL_UPDATE_CURBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    curbuf = savebuf;
    curwin = savewin;
    إذا (flags & FL_ADJUST_CURSOR)
	check_cursor();
    إذا (flags & (FL_UPDATE_SCREEN | FL_UPDATE_CURBUF))
	update_screen(UPD_NOT_VALID);

    أرجع err;
}

/*
 * This function implements the window commands.
 */
    ساكن صحيح
winselfcmd(
    Client_معلومات	ref,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحيح		err, idx, flags;
    صحيح		val1, val2;
    Tcl_Obj	*resobj;
    win_T	*savewin, *win;
    buf_T	*savebuf;
    حرف	*str;

    تعداد
    {
	WIN_بفر, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_بیان,
	WIN_لوړوالی, WIN_OPTION
    };
    ساكن CONST84 حرف *winoptions[] =
    {
	"buffer", "command", "cursor", "delcmd", "expr",
	"height", "option", (حرف *)0
    };

    إذا (objc < 2)
    {
	Tcl_Wrong_عدد_دلیلونه(interp, 1, objv, "option ?arg ...?");
	أرجع TCL_ERROR;
    }

    err = Tcl_Get_شاخص_From_Obj(interp, objv[1], winoptions, "option", 0,  &idx);
    إذا (err != TCL_OK)
	أرجع TCL_ERROR;

    win = (win_T *)((بنية ref *)ref)->vimobj;
    savewin = curwin;  curwin = win;
    savebuf = curbuf;  curbuf = win->w_بفر;
    flags = 0;

    بدل (idx)
    {
	حالة WIN_OPTION:
	    err = tclsetoption(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	حالة WIN_COMMAND:
	    err = tcldoexcommand(interp, objc, objv, 2);
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	حالة WIN_بیان:
	    err = tclvimexpr(interp, objc, objv, 2);
	    اكسر;

	حالة WIN_لوړوالی:
	    إذا (objc == 3)
	    {
		err = Tcl_GetIntFromObj(interp, objv[2], &val1);
		إذا (err != TCL_OK)
		    اكسر;
#که_تعریف_شوی FEAT_GUI
		need_mouse_correct = TRUE;
#د_که_پای
		win_setheight(val1);
		err = vimerror(interp);
		إذا (err != TCL_OK)
		    اكسر;
	    }
	    وإلا
	    إذا (objc != 2)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, "?value?");
		err = TCL_ERROR;
		اكسر;
	    }

	    resobj = Tcl_NewIntObj((صحيح)(win->w_لوړوالی));
	    Tcl_سېټ_Obj_پایله(interp, resobj);
	    اكسر;

	حالة WIN_بفر:
	    إذا (objc != 2)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, عدم);
		err = TCL_ERROR;
		اكسر;
	    }
	    str = tclgetbuffer(interp, win->w_بفر);
	    إذا (str)
		Tcl_سېټ_پایله(interp, str, TCL_VOLATILE);
	    وإلا
		err = TCL_ERROR;
	    اكسر;

	حالة WIN_DELCMD:
	    إذا (objc != 3)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, "command");
		err = TCL_ERROR;
		اكسر;
	    }
	    err = tclsetdelcmd(interp, win->w_tcl_ref, (فراغ *)win, objv[2]);
	    اكسر;

	حالة WIN_CURSOR:
	    إذا (objc > 4)
	    {
		Tcl_Wrong_عدد_دلیلونه(interp, 2, objv, "?arg1 ?arg2??");
		err = TCL_ERROR;
		اكسر;
	    }
	    إذا (objc == 2)
	    {
		حرف buf[64];

		sprintf(buf, _("row %d column %d"), (صحيح)row2tcl(win->w_cursor.lnum), (صحيح)col2tcl(win->w_cursor.ستون));
		Tcl_سېټ_پایله(interp, buf, TCL_VOLATILE);
		اكسر;
	    }
	    وإلا إذا (objc == 3)
	    {
		Tcl_Obj *part, *var;

		part = Tcl_NewStringObj("row", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_پیغام);
		إذا (var == عدم)
		{
		    err = TCL_ERROR;
		    اكسر;
		}
		err = tclgetlinenum(interp, var, &val1, win->w_بفر);
		إذا (err != TCL_OK)
		    اكسر;
		part = Tcl_NewStringObj("column", -1);
		var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_پیغام);
		إذا (var == عدم)
		{
		    err = TCL_ERROR;
		    اكسر;
		}
		err = Tcl_GetIntFromObj(interp, var, &val2);
		إذا (err != TCL_OK)
		    اكسر;
	    }
	    وإلا
	    {  // objc == 4
		err = tclgetlinenum(interp, objv[2], &val1, win->w_بفر);
		إذا (err != TCL_OK)
		    اكسر;
		err = Tcl_GetIntFromObj(interp, objv[3], &val2);
		إذا (err != TCL_OK)
		    اكسر;
	    }
	    // TODO: should check column
	    win->w_cursor.lnum = val1;
	    win->w_cursor.ستون = col2vim(val2);
	    win->w_سېټ_curswant = true;
	    flags |= FL_UPDATE_SCREEN;
	    اكسر;

	افتراضي:
	    Tcl_سېټ_پایله(interp, _("not implemented yet"), TCL_STATIC);
	    اكسر;
    }

    curwin = savewin;
    curbuf = savebuf;
    إذا (flags & FL_UPDATE_SCREEN)
	update_screen(UPD_NOT_VALID);

    أرجع err;
}


    ساكن صحيح
commandcmd(
    Client_معلومات	dummy ناکارول_شوی,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحيح		err;

    err = tcldoexcommand(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    أرجع err;
}

    ساكن صحيح
optioncmd(
    Client_معلومات	dummy ناکارول_شوی,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    صحيح		err;

    err = tclsetoption(interp, objc, objv, 1);
    update_screen(UPD_VALID);
    أرجع err;
}

    ساكن صحيح
exprcmd(
    Client_معلومات	dummy ناکارول_شوی,
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[])
{
    أرجع tclvimexpr(interp, objc, objv, 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support functions for Tcl commands
////////////////////////////////////////////////////////////////////////////

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
    ساكن صحيح
tclgetlinenum(
    Tcl_Interp	*interp,
    Tcl_Obj	*obj,
    صحيح		*ارزښت_P,
    buf_T	*buf)
{
    صحيح err, i;

    تعداد { LN_BEGIN, LN_BOTTOM, LN_پای_ټکی, LN_لومړی, LN_وروستی, LN_پیل, LN_سر };

    ساكن CONST84 حرف *keyw[] =
    {
	"begin", "bottom", "end", "first", "last", "start", "top", (حرف *)0
    };

    err = Tcl_Get_شاخص_From_Obj(interp, obj, keyw, "", 0, &i);
    إذا (err == TCL_OK)
    {
	بدل (i)
	{
	    حالة LN_BEGIN:
	    حالة LN_لومړی:
	    حالة LN_پیل:
	    حالة LN_سر:
		*ارزښت_P = 1;
		اكسر;
	    حالة LN_BOTTOM:
	    حالة LN_پای_ټکی:
	    حالة LN_وروستی:
		*ارزښت_P = buf->b_ml.ml_کرښه_شمېر;
		اكسر;
	}
	أرجع TCL_OK;
    }
    Tcl_Reset_پایله(interp);

    err = Tcl_GetIntFromObj(interp, obj, &i);
    إذا (err != TCL_OK)
	أرجع err;
    i = row2vim(i);
    إذا (i < 1  ||  i > buf->b_ml.ml_کرښه_شمېر)
    {
	Tcl_سېټ_پایله(interp, _("line number out of range"), TCL_STATIC);
	أرجع TCL_ERROR;
    }
    *ارزښت_P = i;
    أرجع TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
    ساكن win_T *
tclfindwin(buf_T *buf)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	إذا (win->w_بفر == buf)
	    أرجع win;
    }
    أرجع curwin;  // keep current window context
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
    ساكن صحيح
tcldoexcommand(
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[],
    صحيح		objn)
{
    tcl_info	saveinfo;
    صحيح		err, flag, nobjs;
    حرف	*arg;

    nobjs = objc - objn;
    إذا (nobjs < 1 || nobjs > 2)
    {
	Tcl_Wrong_عدد_دلیلونه(interp, objn, objv, "?-quiet? exCommand");
	أرجع TCL_ERROR;
    }

    flag = 0;
    إذا (nobjs == 2)
    {
	arg = Tcl_GetStringFromObj(objv[objn], عدم);
	إذا (strcmp(arg, "-quiet") == 0)
	    flag = 1;
	وإلا
	{
	    Tcl_Reset_پایله(interp);
	    Tcl_Append_پایله(interp, _("unknown flag: "), arg, (حرف *)0);
	    أرجع TCL_ERROR;
	}
	++objn;
    }

    حافظه_نقل(&saveinfo, &tclinfo, حجم(tcl_info));
    tclinfo.interp = عدم;
    tclinfo.curwin = عدم;
    tclinfo.curbuf = عدم;

    arg = Tcl_GetStringFromObj(objv[objn], عدم);
    إذا (flag)
	++emsg_off;
    do_cmdline_cmd((char_u *)arg);
    إذا (flag)
	--emsg_off;
    err = vimerror(interp);

    // If the ex command created a new Tcl interpreter, remove it
    إذا (tclinfo.interp)
	tcldelthisinterp();
    حافظه_نقل(&tclinfo, &saveinfo, حجم(tcl_info));
    tclupdatevars();

    أرجع err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
    ساكن صحيح
tclsetoption(
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[],
    صحيح		objn)
{
    صحيح		err, nobjs, idx;
    char_u	*option;
    getoption_T	gov;
    اوږد	lval;
    char_u	*sval;
    Tcl_Obj	*resobj;

    تعداد { OPT_OFF, OPT_ON, OPT_TOGGLE };
    ساكن CONST84 حرف *optkw[] = { "off", "on", "toggle", (حرف *)0 };

    nobjs = objc - objn;
    إذا (nobjs != 1 && nobjs != 2)
    {
	Tcl_Wrong_عدد_دلیلونه(interp, objn, objv, "vimOption ?value?");
	أرجع TCL_ERROR;
    }

    option = (char_u *)Tcl_GetStringFromObj(objv[objn], عدم);
    ++objn;
    gov = get_option_ارزښت(option, &lval, &sval, عدم, 0);
    err = TCL_OK;
    بدل (gov)
    {
	حالة gov_string:
	    Tcl_سېټ_پایله(interp, (حرف *)sval, TCL_VOLATILE);
	    vim_free(sval);
	    اكسر;
	حالة gov_bool:
	حالة gov_شمېره:
	    resobj = Tcl_NewLongObj(lval);
	    Tcl_سېټ_Obj_پایله(interp, resobj);
	    اكسر;
	افتراضي:
	    Tcl_سېټ_پایله(interp, _("unknown vimOption"), TCL_STATIC);
	    أرجع TCL_ERROR;
    }
    إذا (nobjs == 2)
    {
	إذا (gov != gov_string)
	{
	    sval = عدم;    // avoid compiler warning
	    err = Tcl_Get_شاخص_From_Obj(interp, objv[objn], optkw, "", 0, &idx);
	    إذا (err != TCL_OK)
	    {
		Tcl_Reset_پایله(interp);
		err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
	    }
	    وإلا
	    {
		بدل (idx)
		{
		    حالة OPT_ON:
			lval = 1;
			اكسر;
		    حالة OPT_OFF:
			lval = 0;
			اكسر;
		    حالة OPT_TOGGLE:
			lval = !lval;
			اكسر;
		}
	    }
	}
	وإلا
	    sval = (char_u *)Tcl_GetStringFromObj(objv[objn], عدم);
	إذا (err == TCL_OK)
	{
	    سېټ_option_ارزښت_give_err(option, lval, sval, OPT_LOCAL);
	    err = vimerror(interp);
	}
    }
    أرجع err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
    ساكن صحيح
tclvimexpr(
    Tcl_Interp	*interp,
    صحيح		objc,
    Tcl_Obj	*CONST objv[],
    صحيح		objn)
{
#که_تعریف_شوی FEAT_EVAL
    حرف	*بیان, *str;
#د_که_پای
    صحيح		err;

    إذا (objc - objn != 1)
    {
	Tcl_Wrong_عدد_دلیلونه(interp, objn, objv, "vimExpr");
	أرجع TCL_ERROR;
    }

#که_تعریف_شوی FEAT_EVAL
    بیان = Tcl_GetStringFromObj(objv[objn], عدم);
    str = (حرف *)eval_منزل_string((char_u *)بیان, TRUE, FALSE);
    إذا (str == عدم)
	Tcl_سېټ_پایله(interp, _("invalid expression"), TCL_STATIC);
    وإلا
    {
	Tcl_سېټ_پایله(interp, str, TCL_VOLATILE);
	vim_free(str);
    }
    err = vimerror(interp);
#وإلا
    Tcl_سېټ_پایله(interp, _("expressions disabled at compile time"), TCL_STATIC);
    err = TCL_ERROR;
#د_که_پای

    أرجع err;
}

/*
 * Check for internal vim errors.
 */
    ساكن صحيح
vimerror(Tcl_Interp *interp)
{
    إذا (got_int)
    {
	Tcl_سېټ_پایله(interp, _("keyboard interrupt"), TCL_STATIC);
	أرجع TCL_ERROR;
    }
    وإلا إذا (did_emsg)
    {
	Tcl_سېټ_پایله(interp, _("Vim error"), TCL_STATIC);
	أرجع TCL_ERROR;
    }
    أرجع TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a win_T* or buf_T* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
    ساكن فراغ
delref(Client_معلومات cref)
{
    بنية ref *ref = (بنية ref *)cref;

    إذا (ref->delcmd)
    {
	Tcl_Decr_Ref_شمېر(ref->delcmd);
	ref->delcmd = عدم;
    }
    ref->interp = عدم;
}

    ساكن حرف *
tclgetref(
    Tcl_Interp	*interp,
    فراغ	**refstartP,	// ptr to w_tcl_ref/b_tcl-ref member of
				// win_T/buf_T struct
    حرف	*مختاړی,	// "win" or "buf"
    فراغ	*vimobj,	// win_T* or buf_T*
    Tcl_ObjCmdProc *proc)	// winselfcmd or bufselfcmd
{
    بنية ref *ref, *ناکارول_شوی_2 = عدم;
    ساكن حرف name[VARNAME_اندازه];
    Tcl_Command cmd;

    ref = (بنية ref *)(*refstartP);
    إذا (ref == &refsdeleted)
    {
	Tcl_سېټ_پایله(interp, _("cannot create buffer/window command: object is being deleted"), TCL_STATIC);
	أرجع عدم;
    }

    طالما (ref != عدم)
    {
	إذا (ref->interp == interp)
	    اكسر;
	إذا (ref->interp == عدم)
	    ناکارول_شوی_2 = ref;
	ref = ref->بل;
    }

    إذا (ref)
	vim_snprintf(name, حجم(name), "::vim::%s",
					Tcl_GetCommandName(interp, ref->cmd));
    وإلا
    {
	إذا (ناکارول_شوی_2)
	    ref = ناکارول_شوی_2;
	وإلا
	{
	    ref = (بنية ref *)Tcl_Alloc(حجم(بنية ref));
	    ref->interp = عدم;
	    ref->بل = (بنية ref *)(*refstartP);
	    (*refstartP) = (فراغ *)ref;
	}

	// This might break on some exotic systems...
	vim_snprintf(name, حجم(name), "::vim::%s_%lx",
					       مختاړی, (بینښه اوږد)vimobj);
	cmd = Tcl_CreateObjCommand(interp, name, proc,
	    (Client_معلومات)ref, (Tcl_CmdDeleteProc *)delref);
	إذا (!cmd)
	    أرجع عدم;

	ref->interp = interp;
	ref->cmd = cmd;
	ref->delcmd = عدم;
	ref->vimobj = vimobj;
    }
    أرجع name;
}

    ساكن حرف *
tclgetwindow(Tcl_Interp *interp, win_T *win)
{
    أرجع tclgetref(interp, &(win->w_tcl_ref), "win", (فراغ *)win, winselfcmd);
}

    ساكن حرف *
tclgetbuffer(Tcl_Interp *interp, buf_T *buf)
{
    أرجع tclgetref(interp, &(buf->b_tcl_ref), "buf", (فراغ *)buf, bufselfcmd);
}

    ساكن صحيح
tclsetdelcmd(
    Tcl_Interp	*interp,
    بنية ref	*reflist,
    فراغ	*vimobj,
    Tcl_Obj	*delcmd)
{
    إذا (reflist == &refsdeleted)
    {
	Tcl_سېټ_پایله(interp, _("cannot register callback command: buffer/window is already being deleted"), TCL_STATIC);
	أرجع TCL_ERROR;
    }

    طالما (reflist != عدم)
    {
	إذا (reflist->interp == interp && reflist->vimobj == vimobj)
	{
	    إذا (reflist->delcmd)
		Tcl_Decr_Ref_شمېر(reflist->delcmd);
	    Tcl_Incr_Ref_شمېر(delcmd);
	    reflist->delcmd = delcmd;
	    أرجع TCL_OK;
	}
	reflist = reflist->بل;
    }
    // This should never happen.  Famous last word?
    iemsg(e_tcl_fatal_error_reflist_corrupt_please_report_this);
    Tcl_سېټ_پایله(interp, _("cannot register callback command: buffer/window reference not found"), TCL_STATIC);
    أرجع TCL_ERROR;
}


////////////////////////////////////////////
//    I/O Channel
////////////////////////////////////////////

    ساكن صحيح
tcl_channel_close(Client_معلومات instance, Tcl_Interp *interp ناکارول_شوی)
{
    صحيح		err = 0;

    // currently does nothing

    إذا (instance != VIMOUT && instance != VIMERR)
    {
	Tcl_سېټ_Errno(EBADF);
	err = EBADF;
    }
    أرجع err;
}

    ساكن صحيح
tcl_channel_ننوت(
    Client_معلومات	instance ناکارول_شوی,
    حرف	*buf ناکارول_شوی,
    صحيح		bufsiz ناکارول_شوی,
    صحيح		*errptr)
{

    // input is currently not supported

    Tcl_سېټ_Errno(EINVAL);
    إذا (errptr)
	*errptr = EINVAL;
    أرجع -1;
}

    ساكن صحيح
tcl_channel_output(
    Client_معلومات	instance,
    ثابت حرف	*buf,
    صحيح		bufsiz,
    صحيح		*errptr)
{
    char_u	*str;
    صحيح		پایله;

    // The buffer is not guaranteed to be 0-terminated, and we don't if
    // there is enough room to add a '\0'.  So we have to create a copy
    // of the buffer...
    str = vim_strnsave((char_u *)buf, bufsiz);
    إذا (!str)
    {
	Tcl_سېټ_Errno(ENOMEM);
	إذا (errptr)
	    *errptr = ENOMEM;
	أرجع -1;
    }

    پایله = bufsiz;
    إذا (instance == VIMOUT)
	tclmsg((حرف *)str);
    وإلا
    إذا (instance == VIMERR)
	tclerrmsg((حرف *)str);
    وإلا
    {
	Tcl_سېټ_Errno(EBADF);
	إذا (errptr)
	    *errptr = EBADF;
	پایله = -1;
    }
    vim_free(str);
    أرجع پایله;
}

    ساكن فراغ
tcl_channel_watch(Client_معلومات instance ناکارول_شوی, صحيح ماسک ناکارول_شوی)
{
    Tcl_سېټ_Errno(EINVAL);
}

    ساكن صحيح
tcl_channel_gethandle(
    Client_معلومات	instance ناکارول_شوی,
    صحيح		direction ناکارول_شوی,
    Client_معلومات	*handleptr ناکارول_شوی)
{
    Tcl_سېټ_Errno(EINVAL);
    أرجع EINVAL;
}


ساكن Tcl_ChannelType tcl_channel_type =
{
    "vimmessage",	// typeName
    TCL_CHANNEL_نسخه_2, //نسخه
    tcl_channel_close,	// closeProc
    tcl_channel_ننوت,	// inputProc
    tcl_channel_output,	// outputProc
    عدم,		// seekProc
    عدم,		// setOptionProc
    عدم,		// getOptionProc
    tcl_channel_watch,	// watchProc
    tcl_channel_gethandle, // getHandleProc
    عدم,		// close2Proc
    عدم,		// blockModeProc
#که_تعریف_شوی TCL_CHANNEL_نسخه_2
    عدم,		// flushProc
    عدم,		// handlerProc
#د_که_پای
// The following should not be necessary since TCL_CHANNEL_VERSION_2 was
// set above
#که_تعریف_شوی TCL_CHANNEL_نسخه_3
    عدم,		// wideSeekProc
#د_که_پای
#که_تعریف_شوی TCL_CHANNEL_نسخه_4
    عدم,		// threadActionProc
#د_که_پای
#که_تعریف_شوی TCL_CHANNEL_نسخه_5
    عدم		// truncateProc
#د_که_پای
};

///////////////////////////////////
// Interface to vim
//////////////////////////////////

    ساكن فراغ
tclupdatevars(فراغ)
{
    حرف varname[VARNAME_اندازه];	// must be writeable
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


    ساكن صحيح
tclinit(exarg_T *eap)
{
    حرف varname[VARNAME_اندازه];	// Tcl_LinkVar requires writeable varname
    حرف *name;

#که_تعریف_شوی DYNAMIC_TCL
    إذا (!tcl_enabled(TRUE))
    {
	emsg(_(e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded));
	أرجع FAIL;
    }
#د_که_پای

    إذا (!tclinfo.interp)
    {
	Tcl_Interp *interp;
	ساكن Tcl_Channel ch1, ch2;

	// Create replacement channels for stdout and stderr; this has to be
	// done each time an interpreter is created since the channels are closed
	// when the interpreter is deleted
	ch1 = Tcl_CreateChannel(&tcl_channel_type, "vimout", VIMOUT, TCL_WRITABLE);
	ch2 = Tcl_CreateChannel(&tcl_channel_type, "vimerr", VIMERR, TCL_WRITABLE);
	Tcl_سېټ_Std_Channel(ch1, TCL_STDOUT);
	Tcl_سېټ_Std_Channel(ch2, TCL_STDERR);

	interp = Tcl_CreateInterp();
	Tcl_Preserve(interp);
	إذا (Tcl_Init(interp) == TCL_ERROR)
	{
	    Tcl_Release(interp);
	    Tcl_DeleteInterp(interp);
	    أرجع FAIL;
	}
#إذا 0
	// VIM sure is interactive
	Tcl_سېټ_Var(interp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
#د_که_پای

	Tcl_سېټ_Channel_Option(interp, ch1, "-buffering", "line");
#که_تعریف_شوی MSWIN
	Tcl_سېټ_Channel_Option(interp, ch1, "-translation", "lf");
#د_که_پای
	Tcl_سېټ_Channel_Option(interp, ch2, "-buffering", "line");
#که_تعریف_شوی MSWIN
	Tcl_سېټ_Channel_Option(interp, ch2, "-translation", "lf");
#د_که_پای

	// replace standard Tcl exit command
	Tcl_DeleteCommand(interp, "exit");
	Tcl_CreateObjCommand(interp, "exit", exitcmd,
	    (Client_معلومات)عدم, (Tcl_CmdDeleteProc *)عدم);

	// new commands, in ::vim namespace
	Tcl_CreateObjCommand(interp, "::vim::buffer", buffercmd,
	    (Client_معلومات)عدم, (Tcl_CmdDeleteProc *)عدم);
	Tcl_CreateObjCommand(interp, "::vim::window", windowcmd,
	   (Client_معلومات)عدم, (Tcl_CmdDeleteProc *)عدم);
	Tcl_CreateObjCommand(interp, "::vim::command", commandcmd,
	   (Client_معلومات)عدم, (Tcl_CmdDeleteProc *)عدم);
	Tcl_CreateObjCommand(interp, "::vim::beep", beepcmd,
	   (Client_معلومات)عدم, (Tcl_CmdDeleteProc *)عدم);
	Tcl_CreateObjCommand(interp, "::vim::option", optioncmd,
	   (Client_معلومات)عدم, (Tcl_CmdDeleteProc *)عدم);
	Tcl_CreateObjCommand(interp, "::vim::expr", exprcmd,
	   (Client_معلومات)عدم, (Tcl_CmdDeleteProc *)عدم);

	// "lbase" variable
	tclinfo.lbase = 1;
	strcpy(varname, VAR_LBASE);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.lbase, TCL_LINK_INT);

	// "range" variable
	tclinfo.range_پیل = eap->line1;
	strcpy(varname, VAR_RANGE1);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.range_پیل, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	strcpy(varname, VAR_RANGE2);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.range_پیل, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	tclinfo.range_پای_ټکی   = eap->line2;
	strcpy(varname, VAR_RANGE3);
	Tcl_LinkVar(interp, varname, (حرف *)&tclinfo.range_پای_ټکی, TCL_LINK_INT|TCL_LINK_READ_ONLY);

	// "current" variable
	tclinfo.curbuf = Tcl_Alloc(VARNAME_اندازه);
	tclinfo.curwin = Tcl_Alloc(VARNAME_اندازه);
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
    وإلا
    {
	// Interpreter already exists, just update variables
	tclinfo.range_پیل = row2tcl(eap->line1);
	tclinfo.range_پای_ټکی = row2tcl(eap->line2);
	tclupdatevars();
    }

    tclinfo.exitvalue = 0;
    أرجع OK;
}

    ساكن فراغ
tclerrmsg(حرف *متن)
{
    حرف *بل;

    طالما ((بل=strchr(متن, '\n')))
    {
	*بل++ = '\0';
	emsg(متن);
	متن = بل;
    }
    إذا (*متن)
	emsg(متن);
}

    ساكن فراغ
tclmsg(حرف *متن)
{
    حرف *بل;

    طالما ((بل=strchr(متن, '\n')))
    {
	*بل++ = '\0';
	پیغام(متن);
	متن = بل;
    }
    إذا (*متن)
	پیغام(متن);
}

    ساكن فراغ
tcldelthisinterp(فراغ)
{
    إذا (!Tcl_InterpDeleted(tclinfo.interp))
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

    tclinfo.interp = عدم;
    Tcl_Free(tclinfo.curbuf);
    Tcl_Free(tclinfo.curwin);
    tclinfo.curbuf = tclinfo.curwin = عدم;
}

    ساكن صحيح
tclexit(صحيح error)
{
    صحيح newerr = OK;

    إذا (Tcl_InterpDeleted(tclinfo.interp)     // True if we intercepted Tcl's exit command
#إذا (TCL_MAJOR_نسخه == 8 && TCL_MINOR_نسخه >= 5) || TCL_MAJOR_نسخه > 8
	|| Tcl_حد_Exceeded(tclinfo.interp)  // True if the interpreter cannot continue
#د_که_پای
	)
    {
	حرف buf[50];

	sprintf(buf, _(e_exit_کوډ_nr), tclinfo.exitvalue);
	tclerrmsg(buf);
	إذا (tclinfo.exitvalue == 0)
	{
	    did_emsg = 0;
	    newerr = OK;
	}
	وإلا
	    newerr = FAIL;

	tcldelthisinterp();
    }
    وإلا
    {
	حرف *پایله;

	پایله = (حرف *)Tcl_Get_String_پایله(tclinfo.interp);
	إذا (error == TCL_OK)
	{
	    tclmsg(پایله);
	    newerr = OK;
	}
	وإلا
	{
	    tclerrmsg(پایله);
	    newerr = FAIL;
	}
    }

    أرجع newerr;
}

/*
 * ":tcl"
 */
    فراغ
ex_tcl(exarg_T *eap)
{
    char_u	*script;
    صحيح		err;

    script = script_get(eap, eap->arg);
    إذا (!eap->skip)
    {
	err = tclinit(eap);
	إذا (err == OK)
	{
	    Tcl_AllowExceptions(tclinfo.interp);
	    إذا (script == عدم)
		err = Tcl_Eval(tclinfo.interp, (حرف *)eap->arg);
	    وإلا
		err = Tcl_Eval(tclinfo.interp, (حرف *)script);
	    err = tclexit(err);
	}
    }
    vim_free(script);
}

/*
 * ":tclfile"
 */
    فراغ
ex_tclfile(exarg_T *eap)
{
    حرف *file = (حرف *)eap->arg;
    صحيح err;

    err = tclinit(eap);
    إذا (err == OK)
    {
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_EvalFile(tclinfo.interp, file);
	err = tclexit(err);
    }
}

/*
 * ":tcldo"
 */
    فراغ
ex_tcldo(exarg_T *eap)
{
    حرف	*script, *کرښه;
    صحيح		err, rs, re, lnum;
    حرف	var_lnum[VARNAME_اندازه]; // must be writeable memory
    حرف	var_کرښه[VARNAME_اندازه];
    linenr_T	لومړی_کرښه = 0;
    linenr_T	وروستی_کرښه = 0;
    buf_T	*was_curbuf = curbuf;

    rs = eap->line1;
    re = eap->line2;
    script = (حرف *)eap->arg;
    strcpy(var_lnum, VAR_CURLNUM);
    strcpy(var_کرښه, VAR_CURLINE);

    err = tclinit(eap);
    إذا (err != OK)
	أرجع;

    lnum = row2tcl(rs);
    Tcl_LinkVar(tclinfo.interp, var_lnum, (حرف *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
    err = TCL_OK;
    إذا (u_save((linenr_T)(rs-1), (linenr_T)(re+1)) != OK)
    {
	Tcl_سېټ_پایله(tclinfo.interp, _("cannot save undo information"), TCL_STATIC);
	err = TCL_ERROR;
    }
    طالما (err == TCL_OK  &&  rs <= re)
    {
	إذا ((linenr_T)rs > curbuf->b_ml.ml_کرښه_شمېر)
	    اكسر;
	کرښه = (حرف *)ml_get_buf(curbuf, (linenr_T)rs, FALSE);
	إذا (!کرښه)
	{
	    Tcl_سېټ_پایله(tclinfo.interp, _("cannot get line"), TCL_STATIC);
	    err = TCL_ERROR;
	    اكسر;
	}
	Tcl_سېټ_Var(tclinfo.interp, var_کرښه, کرښه, 0);
	Tcl_AllowExceptions(tclinfo.interp);
	err = Tcl_Eval(tclinfo.interp, script);
	إذا (err != TCL_OK
	    || Tcl_InterpDeleted(tclinfo.interp)
#إذا (TCL_MAJOR_نسخه == 8 && TCL_MINOR_نسخه >= 5) || TCL_MAJOR_نسخه > 8
	    || Tcl_حد_Exceeded(tclinfo.interp)
#د_که_پای
	    || curbuf != was_curbuf
	    || (linenr_T)rs > curbuf->b_ml.ml_کرښه_شمېر)
	    اكسر;
	کرښه = (حرف *)Tcl_GetVar(tclinfo.interp, var_کرښه, 0);
	إذا (کرښه)
	{
	    إذا (ml_replace((linenr_T)rs, (char_u *)کرښه, TRUE) != OK)
	    {
		Tcl_سېټ_پایله(tclinfo.interp, _("cannot replace line"), TCL_STATIC);
		err = TCL_ERROR;
		اكسر;
	    }
	    إذا (لومړی_کرښه == 0)
		لومړی_کرښه = rs;
	    وروستی_کرښه = rs;
	}
	++rs;
	++lnum;
	Tcl_UpdateLinkedVar(tclinfo.interp, var_lnum);
    }
    إذا (لومړی_کرښه)
	بدل_شوی_کرښې(لومړی_کرښه, 0, وروستی_کرښه + 1, (اوږد)0);

    Tcl_UnsetVar(tclinfo.interp, var_کرښه, 0);
    Tcl_UnlinkVar(tclinfo.interp, var_lnum);
    إذا (err == TCL_OK)
	Tcl_Reset_پایله(tclinfo.interp);

    (فراغ)tclexit(err);
}

    ساكن فراغ
tcldelallrefs(بنية ref *ref)
{
    بنية ref	*بل;
    صحيح		err;
    حرف	*پایله;

#که_تعریف_شوی DYNAMIC_TCL
    // TODO: this code currently crashes Vim on exit
    إذا (exiting)
	أرجع;
#د_که_پای

    طالما (ref != عدم)
    {
	بل = ref->بل;
	إذا (ref->interp)
	{
	    إذا (ref->delcmd)
	    {
		err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
		إذا (err != TCL_OK)
		{
		    پایله = (حرف *)Tcl_Get_String_پایله(ref->interp);
		    إذا (پایله)
			tclerrmsg(پایله);
		}
		Tcl_Decr_Ref_شمېر(ref->delcmd);
		ref->delcmd = عدم;
	    }
	    Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
	}
	Tcl_Free((حرف *)ref);
	ref = بل;
    }
}

    فراغ
tcl_بفر_free(buf_T *buf)
{
    بنية ref *reflist;

#که_تعریف_شوی DYNAMIC_TCL
    إذا (!stubs_initialized)	// Not using Tcl, nothing to do.
	أرجع;
#د_که_پای

    reflist = (بنية ref *)(buf->b_tcl_ref);
    إذا (reflist != &refsdeleted)
    {
	buf->b_tcl_ref = (فراغ *)&refsdeleted;
	tcldelallrefs(reflist);
	buf->b_tcl_ref = عدم;
    }
}

    فراغ
tcl_کړکۍ_free(win_T *win)
{
    بنية ref *reflist;

#که_تعریف_شوی DYNAMIC_TCL
    إذا (!stubs_initialized)	// Not using Tcl, nothing to do.
	أرجع;
#د_که_پای

    reflist = (بنية ref*)(win->w_tcl_ref);
    إذا (reflist != &refsdeleted)
    {
	win->w_tcl_ref = (فراغ *)&refsdeleted;
	tcldelallrefs(reflist);
	win->w_tcl_ref = عدم;
    }
}

// The End
