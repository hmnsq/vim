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

#অন্তর্ভাৱ <ctype.h>
#অন্তর্ভাৱ <io.h>
#অন্তর্ভাৱ <wchar.h>
#অন্তর্ভাৱ <windows.h>

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

#অন্তর্ভাৱ "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
প্রকারনির্দেশ BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dwBufferSize);
স্থির pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = রিক্ত;

# ifndef USE_FILEEXTD
স্থির BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dwBufferSize UNUSED)
{
	ঘূৰাই FALSE;
}
# endif

স্থির শূন্য setup_fileid_api(শূন্য)
{
	যদি (pGetFileInformationByHandleEx != রিক্ত) {
		ঘূৰাই;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	যদি (pGetFileInformationByHandleEx == রিক্ত) {
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
	(wcsncmp((s), (prefix), পরিমাণ(prefix) / পরিমাণ(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
পূর্ণাঙ্ক is_cygpty(পূর্ণাঙ্ক fd)
{
#ifdef STUB_IMPL
	ঘূৰাই 0;
#অন্যথা
	HANDLE h;
	ধ্রুৱ পূর্ণাঙ্ক size = পরিমাণ(FILE_NAME_INFO) + পরিমাণ(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = রিক্ত;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	যদি (h == INVALID_HANDLE_মান) {
		ঘূৰাই 0;
	}
	// Cygwin/msys's pty is a pipe.
	যদি (GetFileType(h) != FILE_TYPE_PIPE) {
		ঘূৰাই 0;
	}
	nameinfo = malloc(size + পরিমাণ(WCHAR));
	যদি (nameinfo == রিক্ত) {
		ঘূৰাই 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	যদি (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / পরিমাণ(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		যদি (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} অন্যথা যদি (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} অন্যথা {
			p = রিক্ত;
		}
		যদি (p != রিক্ত) {
			// Skip 16-digit hexadecimal.
			যেতিয়ালৈ (*p && iswascii(*p) && isxdigit(*p))
				++p;
			যদি (is_wprefix(p, L"-pty")) {
				p += 4;
			} অন্যথা {
				p = রিক্ত;
			}
		}
		যদি (p != রিক্ত) {
			// Skip pty number.
			যেতিয়ালৈ (*p && iswascii(*p) && isdigit(*p))
				++p;
			যদি (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} অন্যথা যদি (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} অন্যথা {
				p = রিক্ত;
			}
		}
	}
	মুক্ত(nameinfo);
	ঘূৰাই (p != রিক্ত);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
পূর্ণাঙ্ক is_cygpty_used(শূন্য)
{
	পূর্ণাঙ্ক fd, ret = 0;

	বাবে (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ঘূৰাই ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
