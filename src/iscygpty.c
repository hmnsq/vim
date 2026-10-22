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

#ضمن <ctype.h>
#ضمن <io.h>
#ضمن <wchar.h>
#ضمن <windows.h>

#ifdef USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# אם !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#אחרת // USE_FILEEXTD
// VC 8 or earlier.
# אם defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  אחרת
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#אחרת
# define UNUSED
#endif

#ضمن "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
تعريفنوع BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dwBufferSize);
ساكن pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = عدم;

# ifndef USE_FILEEXTD
ساكن BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dwBufferSize UNUSED)
{
	החזר FALSE;
}
# endif

ساكن فراغ setup_fileid_api(فراغ)
{
	אם (pGetFileInformationByHandleEx != عدم) {
		החזר;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	אם (pGetFileInformationByHandleEx == عدم) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# אחרת
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#אחרת
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), حجم(prefix) / حجم(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
صحيح is_cygpty(صحيح fd)
{
#ifdef STUB_IMPL
	החזר 0;
#אחרת
	HANDLE h;
	ثابت صحيح size = حجم(FILE_NAME_INFO) + حجم(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = عدم;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	אם (h == INVALID_HANDLE_VALUE) {
		החזר 0;
	}
	// Cygwin/msys's pty is a pipe.
	אם (GetFileType(h) != FILE_TYPE_PIPE) {
		החזר 0;
	}
	nameinfo = malloc(size + حجم(WCHAR));
	אם (nameinfo == عدم) {
		החזר 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	אם (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / حجم(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		אם (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} אחרת אם (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} אחרת {
			p = عدم;
		}
		אם (p != عدم) {
			// Skip 16-digit hexadecimal.
			כלעוד (*p && iswascii(*p) && isxdigit(*p))
				++p;
			אם (is_wprefix(p, L"-pty")) {
				p += 4;
			} אחרת {
				p = عدم;
			}
		}
		אם (p != عدم) {
			// Skip pty number.
			כלעוד (*p && iswascii(*p) && isdigit(*p))
				++p;
			אם (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} אחרת אם (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} אחרת {
				p = عدم;
			}
		}
	}
	حرر(nameinfo);
	החזר (p != عدم);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
صحيح is_cygpty_used(فراغ)
{
	صحيح fd, ret = 0;

	עבור (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	החזר ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
