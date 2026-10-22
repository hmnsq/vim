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

#تعریف_کړه دوګونی_FILE_IO
#تعریف_کړه USE_EXE_NAME		// use argv[0] for $VIM
#تعریف_کړه USE_غړی_CONSOLE
#که_نه_تعریف_شوی HAVE_STRING_H
# define HAVE_STRING_H
#د_که_پای
#که_نه_تعریف_شوی HAVE_MATH_H
# define HAVE_MATH_H
#د_که_پای
#که_نه_تعریف_شوی __GNUC__
# define HAVE_STRICMP
# define HAVE_STRNICMP
#د_که_پای
#که_نه_تعریف_شوی HAVE_STRFTIME
# define HAVE_STRFTIME		// guessed
#د_که_پای
#که_نه_تعریف_شوی HAVE_STRPTIME
# define HAVE_STRPTIME		// provided by src/strptime.c
#د_که_پای
#که_نه_تعریف_شوی HAVE_TZSET
# define HAVE_TZSET		// CRT has tzset() / _tzset()
#د_که_پای
#تعریف_کړه HAVE_MEMSET
#که_نه_تعریف_شوی HAVE_LOCALE_H
# define HAVE_LOCALE_H 1
#د_که_پای
#که_نه_تعریف_شوی HAVE_FCNTL_H
# define HAVE_FCNTL_H
#د_که_پای
#تعریف_کړه HAVE_QSORT
#تعریف_کړه HAVE_ST_ډېرپېښ		// have stat.st_mode

#تعریف_کړه FEAT_SHORTCUT		// resolve shortcuts

// Access Control List (actually security info).
#تعریف_کړه HAVE_ACL

#تعریف_کړه USE_FNAME_CASE		// adjust case of file names
#إذا !defined(FEAT_CLIPBOARD)
# define FEAT_CLIPBOARD		// include clipboard support
#د_که_پای
#إذا defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_وخت
#د_که_پای
#که_نه_تعریف_شوی FEAT_GUI_MSWIN		// GUI works different
# define BREAKCHECK_SKIP    1	// call mch_breakcheck() each time, it's fast
#د_که_پای

#تعریف_کړه HAVE_ټول_MEM

#تعریف_کړه HAVE_PUTENV		// at least Bcc 5.2 and MSC have it

#إذا defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
# define NO_CONSOLE		// don't include console-only code
#د_که_پای

// toupper() is not really broken, but it's very slow.	Probably because of
// using Unicode characters on Windows NT
#تعریف_کړه BROKEN_TOUPPER

#تعریف_کړه FNAME_ILLEGAL "\"*?><|" // illegal characters in a file name

#ضمن <signal.h>
#ضمن <stdlib.h>
#ضمن <time.h>
#ضمن <sys/types.h>

#که_نه_تعریف_شوی STRICT
# define STRICT
#د_که_پای
#که_نه_تعریف_شوی COBJMACROS
# define COBJMACROS	// For OLE: Enable "friendlier" access to objects
#د_که_پای

// Must include winsock2.h before windows.h since it conflicts with winsock.h
// (included in windows.h).
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#ضمن <windows.h>

// Weird: rpcndr.h defines "small" to "char", which causes trouble
#تعریف_لغوه small

#که_نه_تعریف_شوی SM_CXPADDEDBORDER
# define SM_CXPADDEDBORDER     92
#د_که_پای

تعريفنوع فراغ (*sighandler_T)(صحيح, صحيح);

/*
 * Win32 has plenty of memory, use large buffers
 */
#تعریف_کړه CMDBUFFSIZE 1024	// size of the command processing buffer

// _MAX_PATH is only 260 (stdlib.h), but we want more for the 'path' option,
// thus use a larger number.
#تعریف_کړه MAXPATHL	1024

#که_نه_تعریف_شوی BASENAMELEN
# define BASENAMELEN	(MAX_لار_2 - 5)	// length of base of file name
#د_که_پای

#تعریف_کړه TEMPNAMELEN	MAX_لار_2	// length of temp file name path

#که_نه_تعریف_شوی DFLT_MAXMEM
# define DFLT_MAXMEM	(2*1024)    // use up to 2 Mbyte for a buffer
#د_که_پای

#که_نه_تعریف_شوی DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	(5*1024)    // use up to 5 Mbyte for Vim
#د_که_پای

/*
 * Reparse Point
 */
#که_نه_تعریف_شوی FILE_ATTRIBUTE_REPARSE_ټکی
# define FILE_ATTRIBUTE_REPARSE_ټکی	0x00000400
#د_که_پای
#که_نه_تعریف_شوی IO_REPARSE_نښان_MOUNT_ټکی
# define IO_REPARSE_نښان_MOUNT_ټکی	0xA0000003
#د_که_پای
#که_نه_تعریف_شوی IO_REPARSE_نښان_SYMLINK
# define IO_REPARSE_نښان_SYMLINK		0xA000000C
#د_که_پای
#که_نه_تعریف_شوی IO_REPARSE_نښان_APPEXECLINK
# define IO_REPARSE_نښان_APPEXECLINK	0x8000001B
#د_که_پای

/*
 * Definition of the reparse point buffer.
 * This is usually defined in the DDK, copy the definition here to avoid
 * adding it as a dependence only for a single structure.
 */
تعريفنوع بنية REPARSE_معلومات_بفر_2 {
    ULONG  Reparse_نښان;
    USHORT Reparse_معلومات_اوږدتیا;
    USHORT Reserved;
    اتحاد {
	بنية {
	    USHORT Substitute_Name_دننه_واټن;
	    USHORT Substitute_Name_اوږدتیا;
	    USHORT Print_Name_دننه_واټن;
	    USHORT Print_Name_اوږدتیا;
	    ULONG  Flags;
	    WCHAR  لار_بفر[1];
	} Symbolic_Link_Reparse_بفر;
	بنية {
	    USHORT Substitute_Name_دننه_واټن;
	    USHORT Substitute_Name_اوږدتیا;
	    USHORT Print_Name_دننه_واټن;
	    USHORT Print_Name_اوږدتیا;
	    WCHAR  لار_بفر[1];
	} Mount_ټکی_Reparse_بفر;
	بنية {
	    UCHAR معلومات_بفر[1];
	} Generic_Reparse_بفر;
	بنية
	{
	    ULONG String_شمېر;
	    WCHAR StringList[1];
	} App_Exec_Link_Reparse_بفر;
    } DUMMYUNIONNAME;
} REPARSE_معلومات_بفر, *PREPARSE_معلومات_بفر;

#که_تعریف_شوی _MSC_VER
    // Support for __try / __except.  All versions of MSVC are
    // expected to have this.  Any other compilers that support it?
# define HAVE_TRY_EXCEPT 1
# include <malloc.h>		// for _resetstkoflw()
#د_که_پای

/*
 * Some simple debugging macros that look and behave a lot like their
 * namesakes in MFC.
 */

#که_تعریف_شوی _DEBUG

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
# define TRACE			1 ? (فراغ)0 : (فراغ)چاپ_کړه
# define TRACE0(sz)
# define TRACE1(sz, p1)
# define TRACE2(sz, p1, p2)
# define TRACE3(sz, p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4)

#د_که_پای // !_DEBUG


#تعریف_کړه ASSERT_اشاره_ګر(p, type) \
    ASSERT(((p) != عدم)  &&  Is_Valid_پته((p), حجم(type), FALSE))

#تعریف_کړه ASSERT_NULL_OR_اشاره_ګر(p, type) \
    ASSERT(((p) == عدم)  ||  Is_Valid_پته((p), حجم(type), FALSE))

#که_نه_تعریف_شوی HAVE_SETENV
# define HAVE_SETENV
#د_که_پای
#تعریف_کړه mch_getenv(x) (char_u *)getenv((حرف *)(x))
#تعریف_کړه vim_mkdir(x, y) mch_mkdir(x)

// Windows Version
#تعریف_کړه MAKE_VER(major, minor, build) \
    (((major) << 24) | ((minor) << 16) | (build))

// The Windows CRT does not declare strptime(); Vim provides it in strptime.c.
حرف *strptime(ثابت حرف *buf, ثابت حرف *fmt, بنية tm *tm);
