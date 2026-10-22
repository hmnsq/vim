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

#include <ctype.h>
#include <io.h>
#include <wchar.h>
#include <windows.h>

#ifdef USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# nếu !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#không_thì // USE_FILEEXTD
// VC 8 or earlier.
# nếu defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  không_thì
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#không_thì
# define UNUSED
#endif

#include "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
typedef BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dwBufferSize);
static pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = NULL;

# ifndef USE_FILEEXTD
static BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dwBufferSize UNUSED)
{
	trả_về FALSE;
}
# endif

static void setup_fileid_api(void)
{
	nếu (pGetFileInformationByHandleEx != NULL) {
		trả_về;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	nếu (pGetFileInformationByHandleEx == NULL) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# không_thì
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#không_thì
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), sizeof(prefix) / sizeof(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
số_nguyên is_cygpty(số_nguyên fd)
{
#ifdef STUB_IMPL
	trả_về 0;
#không_thì
	HANDLE h;
	const số_nguyên size = sizeof(FILE_NAME_INFO) + sizeof(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = NULL;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	nếu (h == INVALID_HANDLE_VALUE) {
		trả_về 0;
	}
	// Cygwin/msys's pty is a pipe.
	nếu (GetFileType(h) != FILE_TYPE_PIPE) {
		trả_về 0;
	}
	nameinfo = malloc(size + sizeof(WCHAR));
	nếu (nameinfo == NULL) {
		trả_về 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	nếu (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / sizeof(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		nếu (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} không_thì nếu (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} không_thì {
			p = NULL;
		}
		nếu (p != NULL) {
			// Skip 16-digit hexadecimal.
			trong_khi (*p && iswascii(*p) && isxdigit(*p))
				++p;
			nếu (is_wprefix(p, L"-pty")) {
				p += 4;
			} không_thì {
				p = NULL;
			}
		}
		nếu (p != NULL) {
			// Skip pty number.
			trong_khi (*p && iswascii(*p) && isdigit(*p))
				++p;
			nếu (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} không_thì nếu (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} không_thì {
				p = NULL;
			}
		}
	}
	free(nameinfo);
	trả_về (p != NULL);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
số_nguyên is_cygpty_used(void)
{
	số_nguyên fd, ret = 0;

	cho (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	trả_về ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
