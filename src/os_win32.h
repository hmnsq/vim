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

#وصف_ڪريو ٻٽو_FILE_IO
#وصف_ڪريو USE_EXE_NAME		// use argv[0] for $VIM
#وصف_ڪريو USE_رڪن_CONSOLE
#جيڪڏهن_وصف_نه_ٿيل HAVE_STRING_H
# define HAVE_STRING_H
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل HAVE_MATH_H
# define HAVE_MATH_H
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل __GNUC__
# define HAVE_STRICMP
# define HAVE_STRNICMP
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل HAVE_STRFTIME
# define HAVE_STRFTIME		// guessed
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل HAVE_STRPTIME
# define HAVE_STRPTIME		// provided by src/strptime.c
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل HAVE_TZSET
# define HAVE_TZSET		// CRT has tzset() / _tzset()
#جيڪڏهن_پورو
#وصف_ڪريو HAVE_MEMSET
#جيڪڏهن_وصف_نه_ٿيل HAVE_LOCALE_H
# define HAVE_LOCALE_H 1
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل HAVE_FCNTL_H
# define HAVE_FCNTL_H
#جيڪڏهن_پورو
#وصف_ڪريو HAVE_QSORT
#وصف_ڪريو HAVE_ST_گهڻائي		// have stat.st_mode

#وصف_ڪريو FEAT_SHORTCUT		// resolve shortcuts

// Access Control List (actually security info).
#وصف_ڪريو HAVE_ACL

#وصف_ڪريو USE_FNAME_CASE		// adjust case of file names
#إذا !defined(FEAT_CLIPBOARD)
# define FEAT_CLIPBOARD		// include clipboard support
#جيڪڏهن_پورو
#إذا defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_وقت
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل FEAT_GUI_MSWIN		// GUI works different
# define BREAKCHECK_SKIP    1	// call mch_breakcheck() each time, it's fast
#جيڪڏهن_پورو

#وصف_ڪريو HAVE_ڪل_MEM

#وصف_ڪريو HAVE_PUTENV		// at least Bcc 5.2 and MSC have it

#إذا defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
# define NO_CONSOLE		// don't include console-only code
#جيڪڏهن_پورو

// toupper() is not really broken, but it's very slow.	Probably because of
// using Unicode characters on Windows NT
#وصف_ڪريو BROKEN_TOUPPER

#وصف_ڪريو FNAME_ILLEGAL "\"*?><|" // illegal characters in a file name

#ضمن <signal.h>
#ضمن <stdlib.h>
#ضمن <time.h>
#ضمن <sys/types.h>

#جيڪڏهن_وصف_نه_ٿيل STRICT
# define STRICT
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل COBJMACROS
# define COBJMACROS	// For OLE: Enable "friendlier" access to objects
#جيڪڏهن_پورو

// Must include winsock2.h before windows.h since it conflicts with winsock.h
// (included in windows.h).
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#ضمن <windows.h>

// Weird: rpcndr.h defines "small" to "char", which causes trouble
#وصف_رد small

#جيڪڏهن_وصف_نه_ٿيل SM_CXPADDEDBORDER
# define SM_CXPADDEDBORDER     92
#جيڪڏهن_پورو

تعريفنوع فراغ (*sighandler_T)(صحيح, صحيح);

/*
 * Win32 has plenty of memory, use large buffers
 */
#وصف_ڪريو CMDBUFFSIZE 1024	// size of the command processing buffer

// _MAX_PATH is only 260 (stdlib.h), but we want more for the 'path' option,
// thus use a larger number.
#وصف_ڪريو MAXPATHL	1024

#جيڪڏهن_وصف_نه_ٿيل BASENAMELEN
# define BASENAMELEN	(MAX_رستو_2 - 5)	// length of base of file name
#جيڪڏهن_پورو

#وصف_ڪريو TEMPNAMELEN	MAX_رستو_2	// length of temp file name path

#جيڪڏهن_وصف_نه_ٿيل DFLT_MAXMEM
# define DFLT_MAXMEM	(2*1024)    // use up to 2 Mbyte for a buffer
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	(5*1024)    // use up to 5 Mbyte for Vim
#جيڪڏهن_پورو

/*
 * Reparse Point
 */
#جيڪڏهن_وصف_نه_ٿيل FILE_ATTRIBUTE_REPARSE_نقطو
# define FILE_ATTRIBUTE_REPARSE_نقطو	0x00000400
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل IO_REPARSE_ٽيگ_MOUNT_نقطو
# define IO_REPARSE_ٽيگ_MOUNT_نقطو	0xA0000003
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل IO_REPARSE_ٽيگ_SYMLINK
# define IO_REPARSE_ٽيگ_SYMLINK		0xA000000C
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل IO_REPARSE_ٽيگ_APPEXECLINK
# define IO_REPARSE_ٽيگ_APPEXECLINK	0x8000001B
#جيڪڏهن_پورو

/*
 * Definition of the reparse point buffer.
 * This is usually defined in the DDK, copy the definition here to avoid
 * adding it as a dependence only for a single structure.
 */
تعريفنوع بنية REPARSE_معلومات_بفر_2 {
    ULONG  Reparse_ٽيگ;
    USHORT Reparse_معلومات_ڊيگهائي;
    USHORT Reserved;
    اتحاد {
	بنية {
	    USHORT Substitute_Name_وٿي;
	    USHORT Substitute_Name_ڊيگهائي;
	    USHORT Print_Name_وٿي;
	    USHORT Print_Name_ڊيگهائي;
	    ULONG  Flags;
	    WCHAR  رستو_بفر[1];
	} Symbolic_Link_Reparse_بفر;
	بنية {
	    USHORT Substitute_Name_وٿي;
	    USHORT Substitute_Name_ڊيگهائي;
	    USHORT Print_Name_وٿي;
	    USHORT Print_Name_ڊيگهائي;
	    WCHAR  رستو_بفر[1];
	} Mount_نقطو_Reparse_بفر;
	بنية {
	    UCHAR معلومات_بفر[1];
	} Generic_Reparse_بفر;
	بنية
	{
	    ULONG String_ڳڻپ;
	    WCHAR StringList[1];
	} App_Exec_Link_Reparse_بفر;
    } DUMMYUNIONNAME;
} REPARSE_معلومات_بفر, *PREPARSE_معلومات_بفر;

#جيڪڏهن_وصف_ٿيل _MSC_VER
    // Support for __try / __except.  All versions of MSVC are
    // expected to have this.  Any other compilers that support it?
# define HAVE_TRY_EXCEPT 1
# include <malloc.h>		// for _resetstkoflw()
#جيڪڏهن_پورو

/*
 * Some simple debugging macros that look and behave a lot like their
 * namesakes in MFC.
 */

#جيڪڏهن_وصف_ٿيل _DEBUG

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
# define TRACE			1 ? (فراغ)0 : (فراغ)لکو
# define TRACE0(sz)
# define TRACE1(sz, p1)
# define TRACE2(sz, p1, p2)
# define TRACE3(sz, p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4)

#جيڪڏهن_پورو // !_DEBUG


#وصف_ڪريو ASSERT_اشارو(p, type) \
    ASSERT(((p) != عدم)  &&  Is_Valid_پتو((p), حجم(type), FALSE))

#وصف_ڪريو ASSERT_NULL_OR_اشارو(p, type) \
    ASSERT(((p) == عدم)  ||  Is_Valid_پتو((p), حجم(type), FALSE))

#جيڪڏهن_وصف_نه_ٿيل HAVE_SETENV
# define HAVE_SETENV
#جيڪڏهن_پورو
#وصف_ڪريو mch_getenv(x) (char_u *)getenv((حرف *)(x))
#وصف_ڪريو vim_mkdir(x, y) mch_mkdir(x)

// Windows Version
#وصف_ڪريو MAKE_VER(major, minor, build) \
    (((major) << 24) | ((minor) << 16) | (build))

// The Windows CRT does not declare strptime(); Vim provides it in strptime.c.
حرف *strptime(ثابت حرف *buf, ثابت حرف *fmt, بنية tm *tm);
