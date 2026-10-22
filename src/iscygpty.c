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
# ᱡᱩᱫᱤ !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#ᱵᱟᱝ // USE_FILEEXTD
// VC 8 or earlier.
# ᱡᱩᱫᱤ defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  ᱵᱟᱝ
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#ᱵᱟᱝ
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
	ᱨᱩᱣᱟᱹᱲ FALSE;
}
# endif

static void setup_fileid_api(void)
{
	ᱡᱩᱫᱤ (pGetFileInformationByHandleEx != NULL) {
		ᱨᱩᱣᱟᱹᱲ;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	ᱡᱩᱫᱤ (pGetFileInformationByHandleEx == NULL) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# ᱵᱟᱝ
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#ᱵᱟᱝ
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), parimaa_na(prefix) / parimaa_na(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
puur_naa_nka is_cygpty(puur_naa_nka fd)
{
#ifdef STUB_IMPL
	ᱨᱩᱣᱟᱹᱲ 0;
#ᱵᱟᱝ
	HANDLE h;
	const puur_naa_nka size = parimaa_na(FILE_NAME_INFO) + parimaa_na(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = NULL;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	ᱡᱩᱫᱤ (h == INVALID_HANDLE_VALUE) {
		ᱨᱩᱣᱟᱹᱲ 0;
	}
	// Cygwin/msys's pty is a pipe.
	ᱡᱩᱫᱤ (GetFileType(h) != FILE_TYPE_PIPE) {
		ᱨᱩᱣᱟᱹᱲ 0;
	}
	nameinfo = malloc(size + parimaa_na(WCHAR));
	ᱡᱩᱫᱤ (nameinfo == NULL) {
		ᱨᱩᱣᱟᱹᱲ 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	ᱡᱩᱫᱤ (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / parimaa_na(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		ᱡᱩᱫᱤ (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} ᱵᱟᱝ ᱡᱩᱫᱤ (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} ᱵᱟᱝ {
			p = NULL;
		}
		ᱡᱩᱫᱤ (p != NULL) {
			// Skip 16-digit hexadecimal.
			ᱡᱚᱠᱷᱚᱱ (*p && iswascii(*p) && isxdigit(*p))
				++p;
			ᱡᱩᱫᱤ (is_wprefix(p, L"-pty")) {
				p += 4;
			} ᱵᱟᱝ {
				p = NULL;
			}
		}
		ᱡᱩᱫᱤ (p != NULL) {
			// Skip pty number.
			ᱡᱚᱠᱷᱚᱱ (*p && iswascii(*p) && isdigit(*p))
				++p;
			ᱡᱩᱫᱤ (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} ᱵᱟᱝ ᱡᱩᱫᱤ (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} ᱵᱟᱝ {
				p = NULL;
			}
		}
	}
	free(nameinfo);
	ᱨᱩᱣᱟᱹᱲ (p != NULL);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
puur_naa_nka is_cygpty_used(void)
{
	puur_naa_nka fd, ret = 0;

	for (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ᱨᱩᱣᱟᱹᱲ ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
