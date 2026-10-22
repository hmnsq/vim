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
# නම් !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#නැතහොත් // USE_FILEEXTD
// VC 8 or earlier.
# නම් defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  නැතහොත්
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#නැතහොත්
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
	ආපසු FALSE;
}
# endif

static void setup_fileid_api(void)
{
	නම් (pGetFileInformationByHandleEx != NULL) {
		ආපසු;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	නම් (pGetFileInformationByHandleEx == NULL) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# නැතහොත්
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#නැතහොත්
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), parimaa_na(prefix) / parimaa_na(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
පූර්ණ_සංඛ්‍යා is_cygpty(පූර්ණ_සංඛ්‍යා fd)
{
#ifdef STUB_IMPL
	ආපසු 0;
#නැතහොත්
	HANDLE h;
	const පූර්ණ_සංඛ්‍යා size = parimaa_na(FILE_NAME_INFO) + parimaa_na(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = NULL;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	නම් (h == INVALID_HANDLE_VALUE) {
		ආපසු 0;
	}
	// Cygwin/msys's pty is a pipe.
	නම් (GetFileType(h) != FILE_TYPE_PIPE) {
		ආපසු 0;
	}
	nameinfo = malloc(size + parimaa_na(WCHAR));
	නම් (nameinfo == NULL) {
		ආපසු 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	නම් (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / parimaa_na(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		නම් (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} නැතහොත් නම් (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} නැතහොත් {
			p = NULL;
		}
		නම් (p != NULL) {
			// Skip 16-digit hexadecimal.
			අතරතුර (*p && iswascii(*p) && isxdigit(*p))
				++p;
			නම් (is_wprefix(p, L"-pty")) {
				p += 4;
			} නැතහොත් {
				p = NULL;
			}
		}
		නම් (p != NULL) {
			// Skip pty number.
			අතරතුර (*p && iswascii(*p) && isdigit(*p))
				++p;
			නම් (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} නැතහොත් නම් (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} නැතහොත් {
				p = NULL;
			}
		}
	}
	free(nameinfo);
	ආපසු (p != NULL);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
පූර්ණ_සංඛ්‍යා is_cygpty_used(void)
{
	පූර්ණ_සංඛ්‍යා fd, ret = 0;

	සඳහා (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ආපසු ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
