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
# ᨀᨘ !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#ᨀᨘᨁ // USE_FILEEXTD
// VC 8 or earlier.
# ᨀᨘ defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  ᨀᨘᨁ
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#ᨀᨘᨁ
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
	ᨀᨘᨒ FALSE;
}
# endif

static void setup_fileid_api(void)
{
	ᨀᨘ (pGetFileInformationByHandleEx != NULL) {
		ᨀᨘᨒ;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	ᨀᨘ (pGetFileInformationByHandleEx == NULL) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# ᨀᨘᨁ
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#ᨀᨘᨁ
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), parimaa_na(prefix) / parimaa_na(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
puur_naa_nka is_cygpty(puur_naa_nka fd)
{
#ifdef STUB_IMPL
	ᨀᨘᨒ 0;
#ᨀᨘᨁ
	HANDLE h;
	const puur_naa_nka size = parimaa_na(FILE_NAME_INFO) + parimaa_na(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = NULL;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	ᨀᨘ (h == INVALID_HANDLE_VALUE) {
		ᨀᨘᨒ 0;
	}
	// Cygwin/msys's pty is a pipe.
	ᨀᨘ (GetFileType(h) != FILE_TYPE_PIPE) {
		ᨀᨘᨒ 0;
	}
	nameinfo = malloc(size + parimaa_na(WCHAR));
	ᨀᨘ (nameinfo == NULL) {
		ᨀᨘᨒ 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	ᨀᨘ (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / parimaa_na(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		ᨀᨘ (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} ᨀᨘᨁ ᨀᨘ (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} ᨀᨘᨁ {
			p = NULL;
		}
		ᨀᨘ (p != NULL) {
			// Skip 16-digit hexadecimal.
			ᨀᨛ (*p && iswascii(*p) && isxdigit(*p))
				++p;
			ᨀᨘ (is_wprefix(p, L"-pty")) {
				p += 4;
			} ᨀᨘᨁ {
				p = NULL;
			}
		}
		ᨀᨘ (p != NULL) {
			// Skip pty number.
			ᨀᨛ (*p && iswascii(*p) && isdigit(*p))
				++p;
			ᨀᨘ (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} ᨀᨘᨁ ᨀᨘ (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} ᨀᨘᨁ {
				p = NULL;
			}
		}
	}
	free(nameinfo);
	ᨀᨘᨒ (p != NULL);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
puur_naa_nka is_cygpty_used(void)
{
	puur_naa_nka fd, ret = 0;

	for (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ᨀᨘᨒ ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
