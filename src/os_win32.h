/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Win32 machine-dependent things.
 */

#ضمن "os_dos.h"		// common MS-DOS and Win32 stuff
#ضمن <direct.h>		// for _mkdir()

#تعریف_کن دودویی_FILE_IO
#تعریف_کن USE_EXE_NAME		// use argv[0] for $VIM
#تعریف_کن USE_جمله_CONSOLE
#اگر_تعریف_نشده HAVE_STRING_H
# define HAVE_STRING_H
#پایان_اگر
#اگر_تعریف_نشده HAVE_MATH_H
# define HAVE_MATH_H
#پایان_اگر
#اگر_تعریف_نشده __GNUC__
# define HAVE_STRICMP
# define HAVE_STRNICMP
#پایان_اگر
#اگر_تعریف_نشده HAVE_STRFTIME
# define HAVE_STRFTIME		// guessed
#پایان_اگر
#اگر_تعریف_نشده HAVE_STRPTIME
# define HAVE_STRPTIME		// provided by src/strptime.c
#پایان_اگر
#اگر_تعریف_نشده HAVE_TZSET
# define HAVE_TZSET		// CRT has tzset() / _tzset()
#پایان_اگر
#تعریف_کن HAVE_MEMSET
#اگر_تعریف_نشده HAVE_LOCALE_H
# define HAVE_LOCALE_H 1
#پایان_اگر
#اگر_تعریف_نشده HAVE_FCNTL_H
# define HAVE_FCNTL_H
#پایان_اگر
#تعریف_کن HAVE_QSORT
#تعریف_کن HAVE_ST_نما		// have stat.st_mode

#تعریف_کن FEAT_SHORTCUT		// resolve shortcuts

// Access Control List (actually security info).
#تعریف_کن HAVE_ACL

#تعریف_کن USE_FNAME_CASE		// adjust case of file names
#إذا !defined(FEAT_CLIPBOARD)
# define FEAT_CLIPBOARD		// include clipboard support
#پایان_اگر
#إذا defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_زمان
#پایان_اگر
#اگر_تعریف_نشده FEAT_GUI_MSWIN		// GUI works different
# define BREAKCHECK_SKIP    1	// call mch_breakcheck() each time, it's fast
#پایان_اگر

#تعریف_کن HAVE_مجموع_MEM

#تعریف_کن HAVE_PUTENV		// at least Bcc 5.2 and MSC have it

#إذا defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
# define NO_CONSOLE		// don't include console-only code
#پایان_اگر

// toupper() is not really broken, but it's very slow.	Probably because of
// using Unicode characters on Windows NT
#تعریف_کن BROKEN_TOUPPER

#تعریف_کن FNAME_ILLEGAL "\"*?><|" // illegal characters in a file name

#ضمن <signal.h>
#ضمن <stdlib.h>
#ضمن <time.h>
#ضمن <sys/types.h>

#اگر_تعریف_نشده STRICT
# define STRICT
#پایان_اگر
#اگر_تعریف_نشده COBJMACROS
# define COBJMACROS	// For OLE: Enable "friendlier" access to objects
#پایان_اگر

// Must include winsock2.h before windows.h since it conflicts with winsock.h
// (included in windows.h).
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#ضمن <windows.h>

// Weird: rpcndr.h defines "small" to "char", which causes trouble
#لغو_تعریف small

#اگر_تعریف_نشده SM_CXPADDEDBORDER
# define SM_CXPADDEDBORDER     92
#پایان_اگر

تعريفنوع فراغ (*sighandler_T)(صحيح, صحيح);

/*
 * Win32 has plenty of memory, use large buffers
 */
#تعریف_کن CMDBUFFSIZE 1024	// size of the command processing buffer

// _MAX_PATH is only 260 (stdlib.h), but we want more for the 'path' option,
// thus use a larger number.
#تعریف_کن MAXPATHL	1024

#اگر_تعریف_نشده BASENAMELEN
# define BASENAMELEN	(MAX_مسیر_2 - 5)	// length of base of file name
#پایان_اگر

#تعریف_کن TEMPNAMELEN	MAX_مسیر_2	// length of temp file name path

#اگر_تعریف_نشده DFLT_MAXMEM
# define DFLT_MAXMEM	(2*1024)    // use up to 2 Mbyte for a buffer
#پایان_اگر

#اگر_تعریف_نشده DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	(5*1024)    // use up to 5 Mbyte for Vim
#پایان_اگر

/*
 * Reparse Point
 */
#اگر_تعریف_نشده FILE_ATTRIBUTE_REPARSE_نقطه
# define FILE_ATTRIBUTE_REPARSE_نقطه	0x00000400
#پایان_اگر
#اگر_تعریف_نشده IO_REPARSE_برچسب_MOUNT_نقطه
# define IO_REPARSE_برچسب_MOUNT_نقطه	0xA0000003
#پایان_اگر
#اگر_تعریف_نشده IO_REPARSE_برچسب_SYMLINK
# define IO_REPARSE_برچسب_SYMLINK		0xA000000C
#پایان_اگر
#اگر_تعریف_نشده IO_REPARSE_برچسب_APPEXECLINK
# define IO_REPARSE_برچسب_APPEXECLINK	0x8000001B
#پایان_اگر

/*
 * Definition of the reparse point buffer.
 * This is usually defined in the DDK, copy the definition here to avoid
 * adding it as a dependence only for a single structure.
 */
تعريفنوع بنية REPARSE_داده_میانگیر_2 {
    ULONG  Reparse_برچسب;
    USHORT Reparse_داده_درازی;
    USHORT Reserved;
    اتحاد {
	بنية {
	    USHORT Substitute_Name_جابه_جایی;
	    USHORT Substitute_Name_درازی;
	    USHORT Print_Name_جابه_جایی;
	    USHORT Print_Name_درازی;
	    ULONG  Flags;
	    WCHAR  مسیر_میانگیر[1];
	} Symbolic_Link_Reparse_میانگیر;
	بنية {
	    USHORT Substitute_Name_جابه_جایی;
	    USHORT Substitute_Name_درازی;
	    USHORT Print_Name_جابه_جایی;
	    USHORT Print_Name_درازی;
	    WCHAR  مسیر_میانگیر[1];
	} Mount_نقطه_Reparse_میانگیر;
	بنية {
	    UCHAR داده_میانگیر[1];
	} Generic_Reparse_میانگیر;
	بنية
	{
	    ULONG String_شمار;
	    WCHAR StringList[1];
	} App_Exec_Link_Reparse_میانگیر;
    } DUMMYUNIONNAME;
} REPARSE_داده_میانگیر, *PREPARSE_داده_میانگیر;

#اگر_تعریف_شده _MSC_VER
    // Support for __try / __except.  All versions of MSVC are
    // expected to have this.  Any other compilers that support it?
# define HAVE_TRY_EXCEPT 1
# include <malloc.h>		// for _resetstkoflw()
#پایان_اگر

/*
 * Some simple debugging macros that look and behave a lot like their
 * namesakes in MFC.
 */

#اگر_تعریف_شده _DEBUG

# include <crtdbg.h>
# define ASSERT(f) _ASSERT(f)

# define TRACE			Trace
# define TRACE0(sz)		Trace(_T("%s"), _T(sz))
# define TRACE1(sz, p1)		Trace(_T(sz), p1)
# define TRACE2(sz, p1, p2)	Trace(_T(sz), p1, p2)
# define TRACE3(sz, p1, p2, p3) Trace(_T(sz), p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4) Trace(_T(sz), p1, p2, p3, p4)

// In debug version, writes trace messages to debug stream
فراغ __cdecl
Trace(حرف *pszFormat, ...);

#وإلا // !_DEBUG

  // These macros should all compile away to nothing
# define ASSERT(f)		((فراغ)0)
# define TRACE			1 ? (فراغ)0 : (فراغ)چاپ_کن
# define TRACE0(sz)
# define TRACE1(sz, p1)
# define TRACE2(sz, p1, p2)
# define TRACE3(sz, p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4)

#پایان_اگر // !_DEBUG


#تعریف_کن ASSERT_اشاره_گر(p, type) \
    ASSERT(((p) != عدم)  &&  Is_Valid_آدرس((p), حجم(type), FALSE))

#تعریف_کن ASSERT_NULL_OR_اشاره_گر(p, type) \
    ASSERT(((p) == عدم)  ||  Is_Valid_آدرس((p), حجم(type), FALSE))

#اگر_تعریف_نشده HAVE_SETENV
# define HAVE_SETENV
#پایان_اگر
#تعریف_کن mch_getenv(x) (char_u *)getenv((حرف *)(x))
#تعریف_کن vim_mkdir(x, y) mch_mkdir(x)

// Windows Version
#تعریف_کن MAKE_VER(major, minor, build) \
    (((major) << 24) | ((minor) << 16) | (build))

// The Windows CRT does not declare strptime(); Vim provides it in strptime.c.
حرف *strptime(ثابت حرف *buf, ثابت حرف *fmt, بنية tm *tm);
