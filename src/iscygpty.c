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
# ᰀᰧ !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#ᰀᰧᰀ // USE_FILEEXTD
// VC 8 or earlier.
# ᰀᰧ defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  ᰀᰧᰀ
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#ᰀᰧᰀ
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
	ᰀᰧᰌ FALSE;
}
# endif

static void setup_fileid_api(void)
{
	ᰀᰧ (pGetFileInformationByHandleEx != NULL) {
		ᰀᰧᰌ;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	ᰀᰧ (pGetFileInformationByHandleEx == NULL) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# ᰀᰧᰀ
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#ᰀᰧᰀ
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), parimaa_na(prefix) / parimaa_na(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
puur_naa_nka is_cygpty(puur_naa_nka fd)
{
#ifdef STUB_IMPL
	ᰀᰧᰌ 0;
#ᰀᰧᰀ
	HANDLE h;
	const puur_naa_nka size = parimaa_na(FILE_NAME_INFO) + parimaa_na(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = NULL;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	ᰀᰧ (h == INVALID_HANDLE_VALUE) {
		ᰀᰧᰌ 0;
	}
	// Cygwin/msys's pty is a pipe.
	ᰀᰧ (GetFileType(h) != FILE_TYPE_PIPE) {
		ᰀᰧᰌ 0;
	}
	nameinfo = malloc(size + parimaa_na(WCHAR));
	ᰀᰧ (nameinfo == NULL) {
		ᰀᰧᰌ 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	ᰀᰧ (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / parimaa_na(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		ᰀᰧ (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} ᰀᰧᰀ ᰀᰧ (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} ᰀᰧᰀ {
			p = NULL;
		}
		ᰀᰧ (p != NULL) {
			// Skip 16-digit hexadecimal.
			ᰀᰬ (*p && iswascii(*p) && isxdigit(*p))
				++p;
			ᰀᰧ (is_wprefix(p, L"-pty")) {
				p += 4;
			} ᰀᰧᰀ {
				p = NULL;
			}
		}
		ᰀᰧ (p != NULL) {
			// Skip pty number.
			ᰀᰬ (*p && iswascii(*p) && isdigit(*p))
				++p;
			ᰀᰧ (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} ᰀᰧᰀ ᰀᰧ (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} ᰀᰧᰀ {
				p = NULL;
			}
		}
	}
	free(nameinfo);
	ᰀᰧᰌ (p != NULL);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
puur_naa_nka is_cygpty_used(void)
{
	puur_naa_nka fd, ret = 0;

	for (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ᰀᰧᰌ ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
