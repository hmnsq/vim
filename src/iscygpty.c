/*
 * iscygpty.c -- part of ptycheck
 * https://github.com/k-takata/ptycheck
 *
 * Copyright (c) 2015-2023 K.Takata
 *
 * You can redistribute it and/or modify it under the terms of either
 * the MIT license (as described below) or the Vim license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifdef _WIN32

#अन्तर्भाव <ctype.h>
#अन्तर्भाव <io.h>
#अन्तर्भाव <wchar.h>
#अन्तर्भाव <windows.h>

#ifdef USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# सचे !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#अञ्ञथा // USE_FILEEXTD
// VC 8 or earlier.
# सचे defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  अञ्ञथा
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#अञ्ञथा
# define UNUSED
#endif

#अन्तर्भाव "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
प्रकारनिर्देश BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dwBufferSize);
स्थिर pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = रिक्त;

# ifndef USE_FILEEXTD
स्थिर BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dwBufferSize UNUSED)
{
	पटिनिवत्त FALSE;
}
# endif

स्थिर शून्य setup_fileid_api(शून्य)
{
	सचे (pGetFileInformationByHandleEx != रिक्त) {
		पटिनिवत्त;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	सचे (pGetFileInformationByHandleEx == रिक्त) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# अञ्ञथा
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#अञ्ञथा
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), परिमाण(prefix) / परिमाण(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
पूर्णाङ्क is_cygpty(पूर्णाङ्क fd)
{
#ifdef STUB_IMPL
	पटिनिवत्त 0;
#अञ्ञथा
	HANDLE h;
	ध्रुव पूर्णाङ्क size = परिमाण(FILE_NAME_INFO) + परिमाण(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = रिक्त;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	सचे (h == INVALID_HANDLE_मानम्) {
		पटिनिवत्त 0;
	}
	// Cygwin/msys's pty is a pipe.
	सचे (GetFileType(h) != FILE_TYPE_PIPE) {
		पटिनिवत्त 0;
	}
	nameinfo = malloc(size + परिमाण(WCHAR));
	सचे (nameinfo == रिक्त) {
		पटिनिवत्त 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	सचे (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / परिमाण(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		सचे (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} अञ्ञथा सचे (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} अञ्ञथा {
			p = रिक्त;
		}
		सचे (p != रिक्त) {
			// Skip 16-digit hexadecimal.
			याव (*p && iswascii(*p) && isxdigit(*p))
				++p;
			सचे (is_wprefix(p, L"-pty")) {
				p += 4;
			} अञ्ञथा {
				p = रिक्त;
			}
		}
		सचे (p != रिक्त) {
			// Skip pty number.
			याव (*p && iswascii(*p) && isdigit(*p))
				++p;
			सचे (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} अञ्ञथा सचे (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} अञ्ञथा {
				p = रिक्त;
			}
		}
	}
	मुक्त(nameinfo);
	पटिनिवत्त (p != रिक्त);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
पूर्णाङ्क is_cygpty_used(शून्य)
{
	पूर्णाङ्क fd, ret = 0;

	अत्थाय (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	पटिनिवत्त ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
