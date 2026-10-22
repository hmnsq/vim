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

#شامل <ctype.h>
#شامل <io.h>
#شامل <wchar.h>
#شامل <windows.h>

#ifdef USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# اگر !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#وگرنه // USE_FILEEXTD
// VC 8 or earlier.
# اگر defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  وگرنه
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#وگرنه
# define UNUSED
#endif

#شامل "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
تعریفنوع BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dwBufferSize);
ایستا pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = پوچ;

# ifndef USE_FILEEXTD
ایستا BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dwBufferSize UNUSED)
{
	بازگشت FALSE;
}
# endif

ایستا تهی setup_fileid_api(تهی)
{
	اگر (pGetFileInformationByHandleEx != پوچ) {
		بازگشت;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	اگر (pGetFileInformationByHandleEx == پوچ) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# وگرنه
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#وگرنه
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), اندازه(prefix) / اندازه(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
صحیح is_cygpty(صحیح fd)
{
#ifdef STUB_IMPL
	بازگشت 0;
#وگرنه
	HANDLE h;
	ثابت صحیح size = اندازه(FILE_NAME_INFO) + اندازه(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = پوچ;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	اگر (h == INVALID_HANDLE_مقدار) {
		بازگشت 0;
	}
	// Cygwin/msys's pty is a pipe.
	اگر (GetFileType(h) != FILE_TYPE_PIPE) {
		بازگشت 0;
	}
	nameinfo = malloc(size + اندازه(WCHAR));
	اگر (nameinfo == پوچ) {
		بازگشت 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	اگر (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / اندازه(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		اگر (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} وگرنه اگر (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} وگرنه {
			p = پوچ;
		}
		اگر (p != پوچ) {
			// Skip 16-digit hexadecimal.
			وقتی (*p && iswascii(*p) && isxdigit(*p))
				++p;
			اگر (is_wprefix(p, L"-pty")) {
				p += 4;
			} وگرنه {
				p = پوچ;
			}
		}
		اگر (p != پوچ) {
			// Skip pty number.
			وقتی (*p && iswascii(*p) && isdigit(*p))
				++p;
			اگر (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} وگرنه اگر (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} وگرنه {
				p = پوچ;
			}
		}
	}
	آزاد(nameinfo);
	بازگشت (p != پوچ);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
صحیح is_cygpty_used(تهی)
{
	صحیح fd, ret = 0;

	برای (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	بازگشت ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
