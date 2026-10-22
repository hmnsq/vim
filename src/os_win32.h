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

#ਅਨ੍ਤਰ੍ਭਾਵ "os_dos.h"		// common MS-DOS and Win32 stuff
#ਅਨ੍ਤਰ੍ਭਾਵ <direct.h>		// for _mkdir()

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੁਆਧਾਰੀ_FILE_IO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_EXE_NAME		// use argv[0] for $VIM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_ਪਦ_CONSOLE
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_STRING_H
# define HAVE_STRING_H
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_MATH_H
# define HAVE_MATH_H
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __GNUC__
# define HAVE_STRICMP
# define HAVE_STRNICMP
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_STRFTIME
# define HAVE_STRFTIME		// guessed
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_STRPTIME
# define HAVE_STRPTIME		// provided by src/strptime.c
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_TZSET
# define HAVE_TZSET		// CRT has tzset() / _tzset()
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_MEMSET
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_LOCALE_H
# define HAVE_LOCALE_H 1
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_FCNTL_H
# define HAVE_FCNTL_H
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_QSORT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_ST_ਬਹੁਲਕ		// have stat.st_mode

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEAT_SHORTCUT		// resolve shortcuts

// Access Control List (actually security info).
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_ACL

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_FNAME_CASE		// adjust case of file names
#ਜੇ !defined(FEAT_CLIPBOARD)
# define FEAT_CLIPBOARD		// include clipboard support
#ਜੇ_ਅੰਤ
#ਜੇ defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_ਸਮਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN		// GUI works different
# define BREAKCHECK_SKIP    1	// call mch_breakcheck() each time, it's fast
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_ਕੁੱਲ_MEM

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_PUTENV		// at least Bcc 5.2 and MSC have it

#ਜੇ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
# define NO_CONSOLE		// don't include console-only code
#ਜੇ_ਅੰਤ

// toupper() is not really broken, but it's very slow.	Probably because of
// using Unicode characters on Windows NT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BROKEN_TOUPPER

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNAME_ILLEGAL "\"*?><|" // illegal characters in a file name

#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ STRICT
# define STRICT
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ COBJMACROS
# define COBJMACROS	// For OLE: Enable "friendlier" access to objects
#ਜੇ_ਅੰਤ

// Must include winsock2.h before windows.h since it conflicts with winsock.h
// (included in windows.h).
#ਅਨ੍ਤਰ੍ਭਾਵ <winsock2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ws2tcpip.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <windows.h>

// Weird: rpcndr.h defines "small" to "char", which causes trouble
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ small

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SM_CXPADDEDBORDER
# define SM_CXPADDEDBORDER     92
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*sighandler_T)(ਪੂਰਨ, ਪੂਰਨ);

/*
 * Win32 has plenty of memory, use large buffers
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMDBUFFSIZE 1024	// size of the command processing buffer

// _MAX_PATH is only 260 (stdlib.h), but we want more for the 'path' option,
// thus use a larger number.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXPATHL	1024

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BASENAMELEN
# define BASENAMELEN	(MAX_ਰਸਤਾ_2 - 5)	// length of base of file name
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEMPNAMELEN	MAX_ਰਸਤਾ_2	// length of temp file name path

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_MAXMEM
# define DFLT_MAXMEM	(2*1024)    // use up to 2 Mbyte for a buffer
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	(5*1024)    // use up to 5 Mbyte for Vim
#ਜੇ_ਅੰਤ

/*
 * Reparse Point
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FILE_ATTRIBUTE_REPARSE_ਬਿੰਦੂ
# define FILE_ATTRIBUTE_REPARSE_ਬਿੰਦੂ	0x00000400
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IO_REPARSE_ਟੈਗ_MOUNT_ਬਿੰਦੂ
# define IO_REPARSE_ਟੈਗ_MOUNT_ਬਿੰਦੂ	0xA0000003
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IO_REPARSE_ਟੈਗ_SYMLINK
# define IO_REPARSE_ਟੈਗ_SYMLINK		0xA000000C
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IO_REPARSE_ਟੈਗ_APPEXECLINK
# define IO_REPARSE_ਟੈਗ_APPEXECLINK	0x8000001B
#ਜੇ_ਅੰਤ

/*
 * Definition of the reparse point buffer.
 * This is usually defined in the DDK, copy the definition here to avoid
 * adding it as a dependence only for a single structure.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ REPARSE_ਅੰਕੜੇ_ਬਫਰ_2 {
    ULONG  Reparse_ਟੈਗ;
    USHORT Reparse_ਅੰਕੜੇ_ਲੰਮਾਈ;
    USHORT Reserved;
    ਸੰਘ {
	ਬਣਤਰ {
	    USHORT Substitute_Name_ਵਿੱਥ;
	    USHORT Substitute_Name_ਲੰਮਾਈ;
	    USHORT Print_Name_ਵਿੱਥ;
	    USHORT Print_Name_ਲੰਮਾਈ;
	    ULONG  Flags;
	    WCHAR  ਰਸਤਾ_ਬਫਰ[1];
	} Symbolic_Link_Reparse_ਬਫਰ;
	ਬਣਤਰ {
	    USHORT Substitute_Name_ਵਿੱਥ;
	    USHORT Substitute_Name_ਲੰਮਾਈ;
	    USHORT Print_Name_ਵਿੱਥ;
	    USHORT Print_Name_ਲੰਮਾਈ;
	    WCHAR  ਰਸਤਾ_ਬਫਰ[1];
	} Mount_ਬਿੰਦੂ_Reparse_ਬਫਰ;
	ਬਣਤਰ {
	    UCHAR ਅੰਕੜੇ_ਬਫਰ[1];
	} Generic_Reparse_ਬਫਰ;
	ਬਣਤਰ
	{
	    ULONG String_ਗਿਣਤੀ;
	    WCHAR StringList[1];
	} App_Exec_Link_Reparse_ਬਫਰ;
    } DUMMYUNIONNAME;
} REPARSE_ਅੰਕੜੇ_ਬਫਰ, *PREPARSE_ਅੰਕੜੇ_ਬਫਰ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _MSC_VER
    // Support for __try / __except.  All versions of MSVC are
    // expected to have this.  Any other compilers that support it?
# define HAVE_TRY_EXCEPT 1
# include <malloc.h>		// for _resetstkoflw()
#ਜੇ_ਅੰਤ

/*
 * Some simple debugging macros that look and behave a lot like their
 * namesakes in MFC.
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _DEBUG

# include <crtdbg.h>
# define ASSERT(f) _ASSERT(f)

# define TRACE			Trace
# define TRACE0(sz)		Trace(_T("%s"), _T(sz))
# define TRACE1(sz, p1)		Trace(_T(sz), p1)
# define TRACE2(sz, p1, p2)	Trace(_T(sz), p1, p2)
# define TRACE3(sz, p1, p2, p3) Trace(_T(sz), p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4) Trace(_T(sz), p1, p2, p3, p4)

// In debug version, writes trace messages to debug stream
ਖਾਲੀ __cdecl
Trace(ਅੱਖਰ *pszFormat, ...);

#ਨਹੀਂਤਾਂ // !_DEBUG

  // These macros should all compile away to nothing
# define ASSERT(f)		((ਖਾਲੀ)0)
# define TRACE			1 ? (ਖਾਲੀ)0 : (ਖਾਲੀ)ਲਿਖੋ
# define TRACE0(sz)
# define TRACE1(sz, p1)
# define TRACE2(sz, p1, p2)
# define TRACE3(sz, p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4)

#ਜੇ_ਅੰਤ // !_DEBUG


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASSERT_ਸੂਚਕ(p, type) \
    ASSERT(((p) != ਨਿਰਾ)  &&  Is_Valid_ਪਤਾ((p), ਆਕਾਰ(type), FALSE))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASSERT_NULL_OR_ਸੂਚਕ(p, type) \
    ASSERT(((p) == ਨਿਰਾ)  ||  Is_Valid_ਪਤਾ((p), ਆਕਾਰ(type), FALSE))

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_SETENV
# define HAVE_SETENV
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mch_getenv(x) (char_u *)getenv((ਅੱਖਰ *)(x))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vim_mkdir(x, y) mch_mkdir(x)

// Windows Version
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAKE_VER(major, minor, build) \
    (((major) << 24) | ((minor) << 16) | (build))

// The Windows CRT does not declare strptime(); Vim provides it in strptime.c.
ਅੱਖਰ *strptime(ਅਚਲ ਅੱਖਰ *buf, ਅਚਲ ਅੱਖਰ *fmt, ਬਣਤਰ tm *tm);
