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

#అన్తర్భావ "os_dos.h"		// common MS-DOS and Win32 stuff
#అన్తర్భావ <direct.h>		// for _mkdir()

#నిర్వచించు ద్విమాన_FILE_IO
#నిర్వచించు USE_EXE_NAME		// use argv[0] for $VIM
#నిర్వచించు USE_పదం_CONSOLE
#నిర్వచితం_కాకపోతే HAVE_STRING_H
# define HAVE_STRING_H
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే HAVE_MATH_H
# define HAVE_MATH_H
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే __GNUC__
# define HAVE_STRICMP
# define HAVE_STRNICMP
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే HAVE_STRFTIME
# define HAVE_STRFTIME		// guessed
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే HAVE_STRPTIME
# define HAVE_STRPTIME		// provided by src/strptime.c
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే HAVE_TZSET
# define HAVE_TZSET		// CRT has tzset() / _tzset()
#అయితే_ముగింపు
#నిర్వచించు HAVE_MEMSET
#నిర్వచితం_కాకపోతే HAVE_LOCALE_H
# define HAVE_LOCALE_H 1
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే HAVE_FCNTL_H
# define HAVE_FCNTL_H
#అయితే_ముగింపు
#నిర్వచించు HAVE_QSORT
#నిర్వచించు HAVE_ST_బహుళకం		// have stat.st_mode

#నిర్వచించు FEAT_SHORTCUT		// resolve shortcuts

// Access Control List (actually security info).
#నిర్వచించు HAVE_ACL

#నిర్వచించు USE_FNAME_CASE		// adjust case of file names
#ఒకవేళ !defined(FEAT_CLIPBOARD)
# define FEAT_CLIPBOARD		// include clipboard support
#అయితే_ముగింపు
#ఒకవేళ defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_సమయం
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_GUI_MSWIN		// GUI works different
# define BREAKCHECK_SKIP    1	// call mch_breakcheck() each time, it's fast
#అయితే_ముగింపు

#నిర్వచించు HAVE_మొత్తం_MEM

#నిర్వచించు HAVE_PUTENV		// at least Bcc 5.2 and MSC have it

#ఒకవేళ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
# define NO_CONSOLE		// don't include console-only code
#అయితే_ముగింపు

// toupper() is not really broken, but it's very slow.	Probably because of
// using Unicode characters on Windows NT
#నిర్వచించు BROKEN_TOUPPER

#నిర్వచించు FNAME_ILLEGAL "\"*?><|" // illegal characters in a file name

#అన్తర్భావ <signal.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <time.h>
#అన్తర్భావ <sys/types.h>

#నిర్వచితం_కాకపోతే STRICT
# define STRICT
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే COBJMACROS
# define COBJMACROS	// For OLE: Enable "friendlier" access to objects
#అయితే_ముగింపు

// Must include winsock2.h before windows.h since it conflicts with winsock.h
// (included in windows.h).
#అన్తర్భావ <winsock2.h>
#అన్తర్భావ <ws2tcpip.h>
#అన్తర్భావ <windows.h>

// Weird: rpcndr.h defines "small" to "char", which causes trouble
#నిర్వచనం_రద్దు small

#నిర్వచితం_కాకపోతే SM_CXPADDEDBORDER
# define SM_CXPADDEDBORDER     92
#అయితే_ముగింపు

రకనిర్వచనం శూన్య (*sighandler_T)(పూర్ణాంకం, పూర్ణాంకం);

/*
 * Win32 has plenty of memory, use large buffers
 */
#నిర్వచించు CMDBUFFSIZE 1024	// size of the command processing buffer

// _MAX_PATH is only 260 (stdlib.h), but we want more for the 'path' option,
// thus use a larger number.
#నిర్వచించు MAXPATHL	1024

#నిర్వచితం_కాకపోతే BASENAMELEN
# define BASENAMELEN	(MAX_మార్గం_2 - 5)	// length of base of file name
#అయితే_ముగింపు

#నిర్వచించు TEMPNAMELEN	MAX_మార్గం_2	// length of temp file name path

#నిర్వచితం_కాకపోతే DFLT_MAXMEM
# define DFLT_MAXMEM	(2*1024)    // use up to 2 Mbyte for a buffer
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	(5*1024)    // use up to 5 Mbyte for Vim
#అయితే_ముగింపు

/*
 * Reparse Point
 */
#నిర్వచితం_కాకపోతే FILE_ATTRIBUTE_REPARSE_బిందువు
# define FILE_ATTRIBUTE_REPARSE_బిందువు	0x00000400
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే IO_REPARSE_ట్యాగ్_MOUNT_బిందువు
# define IO_REPARSE_ట్యాగ్_MOUNT_బిందువు	0xA0000003
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే IO_REPARSE_ట్యాగ్_SYMLINK
# define IO_REPARSE_ట్యాగ్_SYMLINK		0xA000000C
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే IO_REPARSE_ట్యాగ్_APPEXECLINK
# define IO_REPARSE_ట్యాగ్_APPEXECLINK	0x8000001B
#అయితే_ముగింపు

/*
 * Definition of the reparse point buffer.
 * This is usually defined in the DDK, copy the definition here to avoid
 * adding it as a dependence only for a single structure.
 */
రకనిర్వచనం నిర్మాణం REPARSE_దత్తాంశం_బఫరు_2 {
    ULONG  Reparse_ట్యాగ్;
    USHORT Reparse_దత్తాంశం_నిడివి;
    USHORT Reserved;
    సంఘం {
	నిర్మాణం {
	    USHORT Substitute_Name_స్థానభ్రంశం;
	    USHORT Substitute_Name_నిడివి;
	    USHORT Print_Name_స్థానభ్రంశం;
	    USHORT Print_Name_నిడివి;
	    ULONG  Flags;
	    WCHAR  మార్గం_బఫరు[1];
	} Symbolic_Link_Reparse_బఫరు;
	నిర్మాణం {
	    USHORT Substitute_Name_స్థానభ్రంశం;
	    USHORT Substitute_Name_నిడివి;
	    USHORT Print_Name_స్థానభ్రంశం;
	    USHORT Print_Name_నిడివి;
	    WCHAR  మార్గం_బఫరు[1];
	} Mount_బిందువు_Reparse_బఫరు;
	నిర్మాణం {
	    UCHAR దత్తాంశం_బఫరు[1];
	} Generic_Reparse_బఫరు;
	నిర్మాణం
	{
	    ULONG String_లెక్క;
	    WCHAR StringList[1];
	} App_Exec_Link_Reparse_బఫరు;
    } DUMMYUNIONNAME;
} REPARSE_దత్తాంశం_బఫరు, *PREPARSE_దత్తాంశం_బఫరు;

#నిర్వచితమైతే _MSC_VER
    // Support for __try / __except.  All versions of MSVC are
    // expected to have this.  Any other compilers that support it?
# define HAVE_TRY_EXCEPT 1
# include <malloc.h>		// for _resetstkoflw()
#అయితే_ముగింపు

/*
 * Some simple debugging macros that look and behave a lot like their
 * namesakes in MFC.
 */

#నిర్వచితమైతే _DEBUG

# include <crtdbg.h>
# define ASSERT(f) _ASSERT(f)

# define TRACE			Trace
# define TRACE0(sz)		Trace(_T("%s"), _T(sz))
# define TRACE1(sz, p1)		Trace(_T(sz), p1)
# define TRACE2(sz, p1, p2)	Trace(_T(sz), p1, p2)
# define TRACE3(sz, p1, p2, p3) Trace(_T(sz), p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4) Trace(_T(sz), p1, p2, p3, p4)

// In debug version, writes trace messages to debug stream
శూన్య __cdecl
Trace(అక్షరం *pszFormat, ...);

#లేకపోతే // !_DEBUG

  // These macros should all compile away to nothing
# define ASSERT(f)		((శూన్య)0)
# define TRACE			1 ? (శూన్య)0 : (శూన్య)రాయి
# define TRACE0(sz)
# define TRACE1(sz, p1)
# define TRACE2(sz, p1, p2)
# define TRACE3(sz, p1, p2, p3)
# define TRACE4(sz, p1, p2, p3, p4)

#అయితే_ముగింపు // !_DEBUG


#నిర్వచించు ASSERT_సూచిక(p, type) \
    ASSERT(((p) != ఖాళీ)  &&  Is_Valid_చిరునామా((p), పరిమాణం(type), FALSE))

#నిర్వచించు ASSERT_NULL_OR_సూచిక(p, type) \
    ASSERT(((p) == ఖాళీ)  ||  Is_Valid_చిరునామా((p), పరిమాణం(type), FALSE))

#నిర్వచితం_కాకపోతే HAVE_SETENV
# define HAVE_SETENV
#అయితే_ముగింపు
#నిర్వచించు mch_getenv(x) (char_u *)getenv((అక్షరం *)(x))
#నిర్వచించు vim_mkdir(x, y) mch_mkdir(x)

// Windows Version
#నిర్వచించు MAKE_VER(major, minor, build) \
    (((major) << 24) | ((minor) << 16) | (build))

// The Windows CRT does not declare strptime(); Vim provides it in strptime.c.
అక్షరం *strptime(స్థిరరాశి అక్షరం *buf, స్థిరరాశి అక్షరం *fmt, నిర్మాణం tm *tm);
