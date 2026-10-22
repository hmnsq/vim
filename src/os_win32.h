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

#شامل "os_dos.h"		// common MS-DOS and Win32 stuff
#شامل <direct.h>		// for _mkdir()

#تعریف_کرو ثنائی_FILE_IO
#تعریف_کرو USE_EXE_NAME		// use argv[0] for $VIM
#تعریف_کرو USE_رکن_CONSOLE
#اگر_غیر_تعریف_شدہ HAVE_STRING_H
# define HAVE_STRING_H
#اگر_ختم
#اگر_غیر_تعریف_شدہ HAVE_MATH_H
# define HAVE_MATH_H
#اگر_ختم
#اگر_غیر_تعریف_شدہ __GNUC__
# define HAVE_STRICMP
# define HAVE_STRNICMP
#اگر_ختم
#اگر_غیر_تعریف_شدہ HAVE_STRFTIME
# define HAVE_STRFTIME		// guessed
#اگر_ختم
#اگر_غیر_تعریف_شدہ HAVE_STRPTIME
# define HAVE_STRPTIME		// provided by src/strptime.c
#اگر_ختم
#اگر_غیر_تعریف_شدہ HAVE_TZSET
# define HAVE_TZSET		// CRT has tzset() / _tzset()
#اگر_ختم
#تعریف_کرو HAVE_MEMSET
#اگر_غیر_تعریف_شدہ HAVE_LOCALE_H
# define HAVE_LOCALE_H 1
#اگر_ختم
#اگر_غیر_تعریف_شدہ HAVE_FCNTL_H
# define HAVE_FCNTL_H
#اگر_ختم
#تعریف_کرو HAVE_QSORT
#تعریف_کرو HAVE_ST_منوال		// have stat.st_mode

#تعریف_کرو FEAT_SHORTCUT		// resolve shortcuts

// Access Control List (actually security info).
#تعریف_کرو HAVE_ACL

#تعریف_کرو USE_FNAME_CASE		// adjust case of file names
#اگر !defined(FEAT_CLIPBOARD)
# define FEAT_CLIPBOARD		// include clipboard support
#اگر_ختم
#اگر defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_وقت
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_GUI_MSWIN		// GUI works different
# define BREAKCHECK_SKIP    1	// call mch_breakcheck() each time, it's fast
#اگر_ختم

#تعریف_کرو HAVE_کل_MEM

#تعریف_کرو HAVE_PUTENV		// at least Bcc 5.2 and MSC have it

#اگر defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
# define NO_CONSOLE		// don't include console-only code
#اگر_ختم

// toupper() is not really broken, but it's very slow.	Probably because of
// using Unicode characters on Windows NT
#تعریف_کرو BROKEN_TOUPPER

#تعریف_کرو FNAME_ILLEGAL "\"*?><|" // illegal characters in a file name

#شامل <signal.h>
#شامل <stdlib.h>
#شامل <time.h>
#شامل <sys/types.h>

#اگر_غیر_تعریف_شدہ STRICT
# define STRICT
#اگر_ختم
#اگر_غیر_تعریف_شدہ COBJMACROS
# define COBJMACROS	// For OLE: Enable "friendlier" access to objects
#اگر_ختم

// Must include winsock2.h before windows.h since it conflicts with winsock.h
// (included in windows.h).
#شامل <winsock2.h>
#شامل <ws2tcpip.h>
#شامل <windows.h>

// Weird: rpcndr.h defines "small" to "char", which causes trouble
#تعریف_منسوخ small

#اگر_غیر_تعریف_شدہ SM_CXPADDEDBORDER
# define SM_CXPADDEDBORDER     92
#اگر_ختم

قسمتعریف خالی (*sighandler_T)(صحیح, صحیح);

/*
 * Win32 has plenty of memory, use large buffers
 */
#تعریف_کرو CMDBUFFSIZE 1024	// size of the command processing buffer

// _MAX_PATH is only 260 (stdlib.h), but we want more for the 'path' option,
// thus use a larger number.
#تعریف_کرو MAXPATHL	1024

#اگر_غیر_تعریف_شدہ BASENAMELEN
# define BASENAMELEN	(MAX_راستہ_2 - 5)	// length of base of file name
#اگر_ختم

#تعریف_کرو TEMPNAMELEN	MAX_راستہ_2	// length of temp file name path

#اگر_غیر_تعریف_شدہ DFLT_MAXMEM
# define DFLT_MAXMEM	(2*1024)    // use up to 2 Mbyte for a buffer
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	(5*1024)    // use up to 5 Mbyte for Vim
#اگر_ختم

/*
 * Reparse Point
 */
#اگر_غیر_تعریف_شدہ FILE_ATTRIBUTE_REPARSE_نقطہ
# define FILE_ATTRIBUTE_REPARSE_نقطہ	0x00000400
#اگر_ختم
#اگر_غیر_تعریف_شدہ IO_REPARSE_لیبل_MOUNT_نقطہ
# define IO_REPARSE_لیبل_MOUNT_نقطہ	0xA0000003
#اگر_ختم
#اگر_غیر_تعریف_شدہ IO_REPARSE_لیبل_SYMLINK
# define IO_REPARSE_لیبل_SYMLINK		0xA000000C
#اگر_ختم
#اگر_غیر_تعریف_شدہ IO_REPARSE_لیبل_APPEXECLINK
# define IO_REPARSE_لیبل_APPEXECLINK	0x8000001B
#اگر_ختم

/*
 * Definition of the reparse point buffer.
 * This is usually defined in the DDK, copy the definition here to avoid
 * adding it as a dependence only for a single structure.
 */
قسمتعریف ساخت REPARSE_ڈیٹا_بفر_2 {
    ULONG  Reparse_لیبل;
    USHORT Reparse_ڈیٹا_لمبائی;
    USHORT Reserved;
    اتحاد {
	ساخت {
	    USHORT Substitute_Name_انحراف;
	    USHORT Substitute_Name_لمبائی;
	    USHORT Print_Name_انحراف;
	    USHORT Print_Name_لمبائی;
	    ULONG  Flags;
	    WCHAR  راستہ_بفر[1];
	} Symbolic_Link_Reparse_بفر;
	ساخت {
	    USHORT Substitute_Name_انحراف;
	    USHORT Substitute_Name_لمبائی;
	    USHORT Print_Name_انحراف;
	    USHORT Print_Name_لمبائی;
	    WCHAR  راستہ_بفر[1];
	} Mount_نقطہ_Reparse_بفر;
	ساخت {
	    UCHAR ڈیٹا_بفر[1];
	} Generic_Reparse_بفر;
	ساخت
	{
	    ULONG String_گنتی;
	    WCHAR StringList[1];
	} App_Exec_Link_Reparse_بفر;
    } DUMMYUNIONNAME;
} REPARSE_ڈیٹا_بفر, *PREPARSE_ڈیٹا_بفر;

#اگر_تعریف_شدہ _MSC_VER
    // Support for __try / __except.  All versions of MSVC are
    // expected to have this.  Any other compilers that support it?
# define HAVE_TRY_EXCEPT 1
# include <malloc.h>		// for _resetstkoflw()
#اگر_ختم

/*
 * Some simple debugging macros that look and behave a lot like their
 * namesakes in MFC.
 */

#اگر_تعریف_شدہ _DEBUG

# include <crtdbg.h>
# define ASSERT(f) _ASSERT(f)

# define TRACE			Trace
# define TRACE0(sz)		Trace(_T("%s"), _T(sz))
# define TRACE1(sz, p1)		Trace(_T(sz), p1)
# define TRACE2(sz, p1, p2)	Trace(_T(sz), p1, p2)
# define TRACE3(sz, p1, p2, p3) Trace(_T(sz), p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4) Trace(_T(sz), p1, p2, p3, p4)

// In debug version, writes trace messages to debug stream
خالی __cdecl
Trace(حرف *pszFormat, ...);

#ورنہ // !_DEBUG

  // These macros should all compile away to nothing
# define ASSERT(f)		((خالی)0)
# define TRACE			1 ? (خالی)0 : (خالی)لکھو
# define TRACE0(sz)
# define TRACE1(sz, p1)
# define TRACE2(sz, p1, p2)
# define TRACE3(sz, p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4)

#اگر_ختم // !_DEBUG


#تعریف_کرو ASSERT_اشارہ_گر(p, type) \
    ASSERT(((p) != تہی)  &&  Is_Valid_پتہ((p), حجم(type), FALSE))

#تعریف_کرو ASSERT_NULL_OR_اشارہ_گر(p, type) \
    ASSERT(((p) == تہی)  ||  Is_Valid_پتہ((p), حجم(type), FALSE))

#اگر_غیر_تعریف_شدہ HAVE_SETENV
# define HAVE_SETENV
#اگر_ختم
#تعریف_کرو mch_getenv(x) (char_u *)getenv((حرف *)(x))
#تعریف_کرو vim_mkdir(x, y) mch_mkdir(x)

// Windows Version
#تعریف_کرو MAKE_VER(major, minor, build) \
    (((major) << 24) | ((minor) << 16) | (build))

// The Windows CRT does not declare strptime(); Vim provides it in strptime.c.
حرف *strptime(ثابت حرف *buf, ثابت حرف *fmt, ساخت tm *tm);
