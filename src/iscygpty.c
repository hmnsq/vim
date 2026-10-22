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
# အကယ်၍ !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#မဟုတ်ပါက // USE_FILEEXTD
// VC 8 or earlier.
# အကယ်၍ defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  မဟုတ်ပါက
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#မဟုတ်ပါက
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
	ပြန်ပေး FALSE;
}
# endif

static void setup_fileid_api(void)
{
	အကယ်၍ (pGetFileInformationByHandleEx != NULL) {
		ပြန်ပေး;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	အကယ်၍ (pGetFileInformationByHandleEx == NULL) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# မဟုတ်ပါက
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#မဟုတ်ပါက
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), parimaa_na(prefix) / parimaa_na(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
ကိန်းပြည့် is_cygpty(ကိန်းပြည့် fd)
{
#ifdef STUB_IMPL
	ပြန်ပေး 0;
#မဟုတ်ပါက
	HANDLE h;
	const ကိန်းပြည့် size = parimaa_na(FILE_NAME_INFO) + parimaa_na(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = NULL;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	အကယ်၍ (h == INVALID_HANDLE_VALUE) {
		ပြန်ပေး 0;
	}
	// Cygwin/msys's pty is a pipe.
	အကယ်၍ (GetFileType(h) != FILE_TYPE_PIPE) {
		ပြန်ပေး 0;
	}
	nameinfo = malloc(size + parimaa_na(WCHAR));
	အကယ်၍ (nameinfo == NULL) {
		ပြန်ပေး 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	အကယ်၍ (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / parimaa_na(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		အကယ်၍ (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} မဟုတ်ပါက အကယ်၍ (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} မဟုတ်ပါက {
			p = NULL;
		}
		အကယ်၍ (p != NULL) {
			// Skip 16-digit hexadecimal.
			နေစဉ် (*p && iswascii(*p) && isxdigit(*p))
				++p;
			အကယ်၍ (is_wprefix(p, L"-pty")) {
				p += 4;
			} မဟုတ်ပါက {
				p = NULL;
			}
		}
		အကယ်၍ (p != NULL) {
			// Skip pty number.
			နေစဉ် (*p && iswascii(*p) && isdigit(*p))
				++p;
			အကယ်၍ (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} မဟုတ်ပါက အကယ်၍ (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} မဟုတ်ပါက {
				p = NULL;
			}
		}
	}
	free(nameinfo);
	ပြန်ပေး (p != NULL);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
ကိန်းပြည့် is_cygpty_used(void)
{
	ကိန်းပြည့် fd, ret = 0;

	အတွက် (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ပြန်ပေး ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
