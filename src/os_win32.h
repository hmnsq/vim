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

#ಅನ್ತರ್ಭಾವ "os_dos.h"		// common MS-DOS and Win32 stuff
#ಅನ್ತರ್ಭಾವ <direct.h>		// for _mkdir()

#ವ್ಯಾಖ್ಯಾನಿಸು ದ್ವಿಮಾನ_FILE_IO
#ವ್ಯಾಖ್ಯಾನಿಸು USE_EXE_NAME		// use argv[0] for $VIM
#ವ್ಯಾಖ್ಯಾನಿಸು USE_ಪದ_CONSOLE
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_STRING_H
# define HAVE_STRING_H
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_MATH_H
# define HAVE_MATH_H
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __GNUC__
# define HAVE_STRICMP
# define HAVE_STRNICMP
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_STRFTIME
# define HAVE_STRFTIME		// guessed
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_STRPTIME
# define HAVE_STRPTIME		// provided by src/strptime.c
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_TZSET
# define HAVE_TZSET		// CRT has tzset() / _tzset()
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_MEMSET
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_LOCALE_H
# define HAVE_LOCALE_H 1
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_FCNTL_H
# define HAVE_FCNTL_H
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_QSORT
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_ST_ಬಹುಲಕ		// have stat.st_mode

#ವ್ಯಾಖ್ಯಾನಿಸು FEAT_SHORTCUT		// resolve shortcuts

// Access Control List (actually security info).
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_ACL

#ವ್ಯಾಖ್ಯಾನಿಸು USE_FNAME_CASE		// adjust case of file names
#ಆದರೆ !defined(FEAT_CLIPBOARD)
# define FEAT_CLIPBOARD		// include clipboard support
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_ಸಮಯ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_GUI_MSWIN		// GUI works different
# define BREAKCHECK_SKIP    1	// call mch_breakcheck() each time, it's fast
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_ಒಟ್ಟು_MEM

#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_PUTENV		// at least Bcc 5.2 and MSC have it

#ಆದರೆ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
# define NO_CONSOLE		// don't include console-only code
#ಆದರೆ_ಅಂತ್ಯ

// toupper() is not really broken, but it's very slow.	Probably because of
// using Unicode characters on Windows NT
#ವ್ಯಾಖ್ಯಾನಿಸು BROKEN_TOUPPER

#ವ್ಯಾಖ್ಯಾನಿಸು FNAME_ILLEGAL "\"*?><|" // illegal characters in a file name

#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ <sys/types.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ STRICT
# define STRICT
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ COBJMACROS
# define COBJMACROS	// For OLE: Enable "friendlier" access to objects
#ಆದರೆ_ಅಂತ್ಯ

// Must include winsock2.h before windows.h since it conflicts with winsock.h
// (included in windows.h).
#ಅನ್ತರ್ಭಾವ <winsock2.h>
#ಅನ್ತರ್ಭಾವ <ws2tcpip.h>
#ಅನ್ತರ್ಭಾವ <windows.h>

// Weird: rpcndr.h defines "small" to "char", which causes trouble
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು small

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SM_CXPADDEDBORDER
# define SM_CXPADDEDBORDER     92
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*sighandler_T)(ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);

/*
 * Win32 has plenty of memory, use large buffers
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CMDBUFFSIZE 1024	// size of the command processing buffer

// _MAX_PATH is only 260 (stdlib.h), but we want more for the 'path' option,
// thus use a larger number.
#ವ್ಯಾಖ್ಯಾನಿಸು MAXPATHL	1024

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ BASENAMELEN
# define BASENAMELEN	(MAX_ಮಾರ್ಗ_2 - 5)	// length of base of file name
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು TEMPNAMELEN	MAX_ಮಾರ್ಗ_2	// length of temp file name path

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_MAXMEM
# define DFLT_MAXMEM	(2*1024)    // use up to 2 Mbyte for a buffer
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	(5*1024)    // use up to 5 Mbyte for Vim
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Reparse Point
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FILE_ATTRIBUTE_REPARSE_ಬಿಂದು
# define FILE_ATTRIBUTE_REPARSE_ಬಿಂದು	0x00000400
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IO_REPARSE_ಟ್ಯಾಗ್_MOUNT_ಬಿಂದು
# define IO_REPARSE_ಟ್ಯಾಗ್_MOUNT_ಬಿಂದು	0xA0000003
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IO_REPARSE_ಟ್ಯಾಗ್_SYMLINK
# define IO_REPARSE_ಟ್ಯಾಗ್_SYMLINK		0xA000000C
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IO_REPARSE_ಟ್ಯಾಗ್_APPEXECLINK
# define IO_REPARSE_ಟ್ಯಾಗ್_APPEXECLINK	0x8000001B
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Definition of the reparse point buffer.
 * This is usually defined in the DDK, copy the definition here to avoid
 * adding it as a dependence only for a single structure.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ REPARSE_ದತ್ತಾಂಶ_ಬಫರ್_2 {
    ULONG  Reparse_ಟ್ಯಾಗ್;
    USHORT Reparse_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    USHORT Reserved;
    ಒಕ್ಕೂಟ {
	ರಚನೆ {
	    USHORT Substitute_Name_ಸ್ಥಾನಾಂತರ;
	    USHORT Substitute_Name_ಉದ್ದಳತೆ;
	    USHORT Print_Name_ಸ್ಥಾನಾಂತರ;
	    USHORT Print_Name_ಉದ್ದಳತೆ;
	    ULONG  Flags;
	    WCHAR  ಮಾರ್ಗ_ಬಫರ್[1];
	} Symbolic_Link_Reparse_ಬಫರ್;
	ರಚನೆ {
	    USHORT Substitute_Name_ಸ್ಥಾನಾಂತರ;
	    USHORT Substitute_Name_ಉದ್ದಳತೆ;
	    USHORT Print_Name_ಸ್ಥಾನಾಂತರ;
	    USHORT Print_Name_ಉದ್ದಳತೆ;
	    WCHAR  ಮಾರ್ಗ_ಬಫರ್[1];
	} Mount_ಬಿಂದು_Reparse_ಬಫರ್;
	ರಚನೆ {
	    UCHAR ದತ್ತಾಂಶ_ಬಫರ್[1];
	} Generic_Reparse_ಬಫರ್;
	ರಚನೆ
	{
	    ULONG String_ಎಣಿಕೆ;
	    WCHAR StringList[1];
	} App_Exec_Link_Reparse_ಬಫರ್;
    } DUMMYUNIONNAME;
} REPARSE_ದತ್ತಾಂಶ_ಬಫರ್, *PREPARSE_ದತ್ತಾಂಶ_ಬಫರ್;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _MSC_VER
    // Support for __try / __except.  All versions of MSVC are
    // expected to have this.  Any other compilers that support it?
# define HAVE_TRY_EXCEPT 1
# include <malloc.h>		// for _resetstkoflw()
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Some simple debugging macros that look and behave a lot like their
 * namesakes in MFC.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _DEBUG

# include <crtdbg.h>
# define ASSERT(f) _ASSERT(f)

# define TRACE			Trace
# define TRACE0(sz)		Trace(_T("%s"), _T(sz))
# define TRACE1(sz, p1)		Trace(_T(sz), p1)
# define TRACE2(sz, p1, p2)	Trace(_T(sz), p1, p2)
# define TRACE3(sz, p1, p2, p3) Trace(_T(sz), p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4) Trace(_T(sz), p1, p2, p3, p4)

// In debug version, writes trace messages to debug stream
ಶೂನ್ಯ __cdecl
Trace(ಅಕ್ಷರ *pszFormat, ...);

#ಇಲ್ಲದಿದ್ದರೆ // !_DEBUG

  // These macros should all compile away to nothing
# define ASSERT(f)		((ಶೂನ್ಯ)0)
# define TRACE			1 ? (ಶೂನ್ಯ)0 : (ಶೂನ್ಯ)ಬರೆ
# define TRACE0(sz)
# define TRACE1(sz, p1)
# define TRACE2(sz, p1, p2)
# define TRACE3(sz, p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4)

#ಆದರೆ_ಅಂತ್ಯ // !_DEBUG


#ವ್ಯಾಖ್ಯಾನಿಸು ASSERT_ಸೂಚಕ(p, type) \
    ASSERT(((p) != ಖಾಲಿ)  &&  Is_Valid_ವಿಳಾಸ((p), ಗಾತ್ರ(type), FALSE))

#ವ್ಯಾಖ್ಯಾನಿಸು ASSERT_NULL_OR_ಸೂಚಕ(p, type) \
    ASSERT(((p) == ಖಾಲಿ)  ||  Is_Valid_ವಿಳಾಸ((p), ಗಾತ್ರ(type), FALSE))

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_SETENV
# define HAVE_SETENV
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು mch_getenv(x) (char_u *)getenv((ಅಕ್ಷರ *)(x))
#ವ್ಯಾಖ್ಯಾನಿಸು vim_mkdir(x, y) mch_mkdir(x)

// Windows Version
#ವ್ಯಾಖ್ಯಾನಿಸು MAKE_VER(major, minor, build) \
    (((major) << 24) | ((minor) << 16) | (build))

// The Windows CRT does not declare strptime(); Vim provides it in strptime.c.
ಅಕ್ಷರ *strptime(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ರಚನೆ tm *tm);
