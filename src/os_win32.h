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

#تعریف_کرو ثنائی_FILE_IO
#تعریف_کرو USE_EXE_NAME		// use argv[0] for $VIM
#تعریف_کرو USE_رکن_CONSOLE
#جے_بنا_تعریف HAVE_STRING_H
# define HAVE_STRING_H
#جے_انت
#جے_بنا_تعریف HAVE_MATH_H
# define HAVE_MATH_H
#جے_انت
#جے_بنا_تعریف __GNUC__
# define HAVE_STRICMP
# define HAVE_STRNICMP
#جے_انت
#جے_بنا_تعریف HAVE_STRFTIME
# define HAVE_STRFTIME		// guessed
#جے_انت
#جے_بنا_تعریف HAVE_STRPTIME
# define HAVE_STRPTIME		// provided by src/strptime.c
#جے_انت
#جے_بنا_تعریف HAVE_TZSET
# define HAVE_TZSET		// CRT has tzset() / _tzset()
#جے_انت
#تعریف_کرو HAVE_MEMSET
#جے_بنا_تعریف HAVE_LOCALE_H
# define HAVE_LOCALE_H 1
#جے_انت
#جے_بنا_تعریف HAVE_FCNTL_H
# define HAVE_FCNTL_H
#جے_انت
#تعریف_کرو HAVE_QSORT
#تعریف_کرو HAVE_ST_منوال		// have stat.st_mode

#تعریف_کرو FEAT_SHORTCUT		// resolve shortcuts

// Access Control List (actually security info).
#تعریف_کرو HAVE_ACL

#تعریف_کرو USE_FNAME_CASE		// adjust case of file names
#إذا !defined(FEAT_CLIPBOARD)
# define FEAT_CLIPBOARD		// include clipboard support
#جے_انت
#إذا defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_ویلا
#جے_انت
#جے_بنا_تعریف FEAT_GUI_MSWIN		// GUI works different
# define BREAKCHECK_SKIP    1	// call mch_breakcheck() each time, it's fast
#جے_انت

#تعریف_کرو HAVE_کل_MEM

#تعریف_کرو HAVE_PUTENV		// at least Bcc 5.2 and MSC have it

#إذا defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
# define NO_CONSOLE		// don't include console-only code
#جے_انت

// toupper() is not really broken, but it's very slow.	Probably because of
// using Unicode characters on Windows NT
#تعریف_کرو BROKEN_TOUPPER

#تعریف_کرو FNAME_ILLEGAL "\"*?><|" // illegal characters in a file name

#ضمن <signal.h>
#ضمن <stdlib.h>
#ضمن <time.h>
#ضمن <sys/types.h>

#جے_بنا_تعریف STRICT
# define STRICT
#جے_انت
#جے_بنا_تعریف COBJMACROS
# define COBJMACROS	// For OLE: Enable "friendlier" access to objects
#جے_انت

// Must include winsock2.h before windows.h since it conflicts with winsock.h
// (included in windows.h).
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#ضمن <windows.h>

// Weird: rpcndr.h defines "small" to "char", which causes trouble
#تعریف_رد small

#جے_بنا_تعریف SM_CXPADDEDBORDER
# define SM_CXPADDEDBORDER     92
#جے_انت

تعريفنوع فراغ (*sighandler_T)(صحيح, صحيح);

/*
 * Win32 has plenty of memory, use large buffers
 */
#تعریف_کرو CMDBUFFSIZE 1024	// size of the command processing buffer

// _MAX_PATH is only 260 (stdlib.h), but we want more for the 'path' option,
// thus use a larger number.
#تعریف_کرو MAXPATHL	1024

#جے_بنا_تعریف BASENAMELEN
# define BASENAMELEN	(MAX_رستہ_2 - 5)	// length of base of file name
#جے_انت

#تعریف_کرو TEMPNAMELEN	MAX_رستہ_2	// length of temp file name path

#جے_بنا_تعریف DFLT_MAXMEM
# define DFLT_MAXMEM	(2*1024)    // use up to 2 Mbyte for a buffer
#جے_انت

#جے_بنا_تعریف DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	(5*1024)    // use up to 5 Mbyte for Vim
#جے_انت

/*
 * Reparse Point
 */
#جے_بنا_تعریف FILE_ATTRIBUTE_REPARSE_نقطہ
# define FILE_ATTRIBUTE_REPARSE_نقطہ	0x00000400
#جے_انت
#جے_بنا_تعریف IO_REPARSE_ٹیگ_MOUNT_نقطہ
# define IO_REPARSE_ٹیگ_MOUNT_نقطہ	0xA0000003
#جے_انت
#جے_بنا_تعریف IO_REPARSE_ٹیگ_SYMLINK
# define IO_REPARSE_ٹیگ_SYMLINK		0xA000000C
#جے_انت
#جے_بنا_تعریف IO_REPARSE_ٹیگ_APPEXECLINK
# define IO_REPARSE_ٹیگ_APPEXECLINK	0x8000001B
#جے_انت

/*
 * Definition of the reparse point buffer.
 * This is usually defined in the DDK, copy the definition here to avoid
 * adding it as a dependence only for a single structure.
 */
تعريفنوع بنية REPARSE_انکڑے_بفر_2 {
    ULONG  Reparse_ٹیگ;
    USHORT Reparse_انکڑے_لمائی;
    USHORT Reserved;
    اتحاد {
	بنية {
	    USHORT Substitute_Name_وتھ;
	    USHORT Substitute_Name_لمائی;
	    USHORT Print_Name_وتھ;
	    USHORT Print_Name_لمائی;
	    ULONG  Flags;
	    WCHAR  رستہ_بفر[1];
	} Symbolic_Link_Reparse_بفر;
	بنية {
	    USHORT Substitute_Name_وتھ;
	    USHORT Substitute_Name_لمائی;
	    USHORT Print_Name_وتھ;
	    USHORT Print_Name_لمائی;
	    WCHAR  رستہ_بفر[1];
	} Mount_نقطہ_Reparse_بفر;
	بنية {
	    UCHAR انکڑے_بفر[1];
	} Generic_Reparse_بفر;
	بنية
	{
	    ULONG String_گنتی;
	    WCHAR StringList[1];
	} App_Exec_Link_Reparse_بفر;
    } DUMMYUNIONNAME;
} REPARSE_انکڑے_بفر, *PREPARSE_انکڑے_بفر;

#جے_تعریف_شدہ _MSC_VER
    // Support for __try / __except.  All versions of MSVC are
    // expected to have this.  Any other compilers that support it?
# define HAVE_TRY_EXCEPT 1
# include <malloc.h>		// for _resetstkoflw()
#جے_انت

/*
 * Some simple debugging macros that look and behave a lot like their
 * namesakes in MFC.
 */

#جے_تعریف_شدہ _DEBUG

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
# define TRACE			1 ? (فراغ)0 : (فراغ)لکھو
# define TRACE0(sz)
# define TRACE1(sz, p1)
# define TRACE2(sz, p1, p2)
# define TRACE3(sz, p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4)

#جے_انت // !_DEBUG


#تعریف_کرو ASSERT_اشارہ_گر(p, type) \
    ASSERT(((p) != عدم)  &&  Is_Valid_پتہ((p), حجم(type), FALSE))

#تعریف_کرو ASSERT_NULL_OR_اشارہ_گر(p, type) \
    ASSERT(((p) == عدم)  ||  Is_Valid_پتہ((p), حجم(type), FALSE))

#جے_بنا_تعریف HAVE_SETENV
# define HAVE_SETENV
#جے_انت
#تعریف_کرو mch_getenv(x) (char_u *)getenv((حرف *)(x))
#تعریف_کرو vim_mkdir(x, y) mch_mkdir(x)

// Windows Version
#تعریف_کرو MAKE_VER(major, minor, build) \
    (((major) << 24) | ((minor) << 16) | (build))

// The Windows CRT does not declare strptime(); Vim provides it in strptime.c.
حرف *strptime(ثابت حرف *buf, ثابت حرف *fmt, بنية tm *tm);
