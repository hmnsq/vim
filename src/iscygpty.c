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
# གལ་སྲིད !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#དེ་མིན // USE_FILEEXTD
// VC 8 or earlier.
# གལ་སྲིད defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  དེ་མིན
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#དེ་མིན
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
	ལོག FALSE;
}
# endif

static void setup_fileid_api(void)
{
	གལ་སྲིད (pGetFileInformationByHandleEx != NULL) {
		ལོག;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	གལ་སྲིད (pGetFileInformationByHandleEx == NULL) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# དེ་མིན
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#དེ་མིན
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), parimaa_na(prefix) / parimaa_na(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
ཧྲིལ is_cygpty(ཧྲིལ fd)
{
#ifdef STUB_IMPL
	ལོག 0;
#དེ་མིན
	HANDLE h;
	const ཧྲིལ size = parimaa_na(FILE_NAME_INFO) + parimaa_na(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = NULL;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	གལ་སྲིད (h == INVALID_HANDLE_VALUE) {
		ལོག 0;
	}
	// Cygwin/msys's pty is a pipe.
	གལ་སྲིད (GetFileType(h) != FILE_TYPE_PIPE) {
		ལོག 0;
	}
	nameinfo = malloc(size + parimaa_na(WCHAR));
	གལ་སྲིད (nameinfo == NULL) {
		ལོག 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	གལ་སྲིད (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / parimaa_na(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		གལ་སྲིད (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} དེ་མིན གལ་སྲིད (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} དེ་མིན {
			p = NULL;
		}
		གལ་སྲིད (p != NULL) {
			// Skip 16-digit hexadecimal.
			སྐབས (*p && iswascii(*p) && isxdigit(*p))
				++p;
			གལ་སྲིད (is_wprefix(p, L"-pty")) {
				p += 4;
			} དེ་མིན {
				p = NULL;
			}
		}
		གལ་སྲིད (p != NULL) {
			// Skip pty number.
			སྐབས (*p && iswascii(*p) && isdigit(*p))
				++p;
			གལ་སྲིད (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} དེ་མིན གལ་སྲིད (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} དེ་མིན {
				p = NULL;
			}
		}
	}
	free(nameinfo);
	ལོག (p != NULL);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
ཧྲིལ is_cygpty_used(void)
{
	ཧྲིལ fd, ret = 0;

	དོན་ལུ (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ལོག ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
