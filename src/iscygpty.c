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

#অন্তর্ভাব <ctype.h>
#অন্তর্ভাব <io.h>
#অন্তর্ভাব <wchar.h>
#অন্তর্ভাব <windows.h>

#ifdef USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# যদি !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#অন্যথা // USE_FILEEXTD
// VC 8 or earlier.
# যদি defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  অন্যথা
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#অন্যথা
# define UNUSED
#endif

#অন্তর্ভাব "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
প্রকারলেখ BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dw_বাফার_আকার);
স্থির pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = ফাঁকা;

# ifndef USE_FILEEXTD
স্থির BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dw_বাফার_আকার UNUSED)
{
	ফেরত FALSE;
}
# endif

স্থির শূন্য setup_fileid_api(শূন্য)
{
	যদি (pGetFileInformationByHandleEx != ফাঁকা) {
		ফেরত;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	যদি (pGetFileInformationByHandleEx == ফাঁকা) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# অন্যথা
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#অন্যথা
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), আকার(prefix) / আকার(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
পূর্ণসংখ্যা is_cygpty(পূর্ণসংখ্যা fd)
{
#ifdef STUB_IMPL
	ফেরত 0;
#অন্যথা
	HANDLE h;
	ধ্রুবক পূর্ণসংখ্যা আকার_2 = আকার(FILE_NAME_INFO) + আকার(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = ফাঁকা;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	যদি (h == INVALID_HANDLE_মান) {
		ফেরত 0;
	}
	// Cygwin/msys's pty is a pipe.
	যদি (GetFileType(h) != FILE_TYPE_PIPE) {
		ফেরত 0;
	}
	nameinfo = malloc(আকার_2 + আকার(WCHAR));
	যদি (nameinfo == ফাঁকা) {
		ফেরত 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	যদি (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, আকার_2)) {
		nameinfo->FileName[nameinfo->File_Name_দৈর্ঘ্য / আকার(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		যদি (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} অন্যথা যদি (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} অন্যথা {
			p = ফাঁকা;
		}
		যদি (p != ফাঁকা) {
			// Skip 16-digit hexadecimal.
			যখন (*p && iswascii(*p) && isxdigit(*p))
				++p;
			যদি (is_wprefix(p, L"-pty")) {
				p += 4;
			} অন্যথা {
				p = ফাঁকা;
			}
		}
		যদি (p != ফাঁকা) {
			// Skip pty number.
			যখন (*p && iswascii(*p) && isdigit(*p))
				++p;
			যদি (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} অন্যথা যদি (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} অন্যথা {
				p = ফাঁকা;
			}
		}
	}
	মুক্ত(nameinfo);
	ফেরত (p != ফাঁকা);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
পূর্ণসংখ্যা is_cygpty_used(শূন্য)
{
	পূর্ণসংখ্যা fd, ret = 0;

	জন্য (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ফেরত ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
