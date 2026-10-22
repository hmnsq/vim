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

#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <io.h>
#ଅନ୍ତର୍ଭାଵ <wchar.h>
#ଅନ୍ତର୍ଭାଵ <windows.h>

#ifdef USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# ଯଦି !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#ଅନ୍ୟଥା // USE_FILEEXTD
// VC 8 or earlier.
# ଯଦି defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  ଅନ୍ୟଥା
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#ଅନ୍ୟଥା
# define UNUSED
#endif

#ଅନ୍ତର୍ଭାଵ "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
ପ୍ରକାରବ୍ୟାଖ୍ୟା BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dwBufferSize);
ସ୍ଥିର pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = ଖାଲି;

# ifndef USE_FILEEXTD
ସ୍ଥିର BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dwBufferSize UNUSED)
{
	ଫେରନ୍ତୁ FALSE;
}
# endif

ସ୍ଥିର ଶୂନ୍ୟ setup_fileid_api(ଶୂନ୍ୟ)
{
	ଯଦି (pGetFileInformationByHandleEx != ଖାଲି) {
		ଫେରନ୍ତୁ;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	ଯଦି (pGetFileInformationByHandleEx == ଖାଲି) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# ଅନ୍ୟଥା
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#ଅନ୍ୟଥା
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), ଆକାର(prefix) / ଆକାର(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_cygpty(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
#ifdef STUB_IMPL
	ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
	HANDLE h;
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = ଆକାର(FILE_NAME_INFO) + ଆକାର(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = ଖାଲି;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	ଯଦି (h == INVALID_HANDLE_ମୂଲ୍ୟ) {
		ଫେରନ୍ତୁ 0;
	}
	// Cygwin/msys's pty is a pipe.
	ଯଦି (GetFileType(h) != FILE_TYPE_PIPE) {
		ଫେରନ୍ତୁ 0;
	}
	nameinfo = malloc(size + ଆକାର(WCHAR));
	ଯଦି (nameinfo == ଖାଲି) {
		ଫେରନ୍ତୁ 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	ଯଦି (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / ଆକାର(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		ଯଦି (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} ଅନ୍ୟଥା ଯଦି (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} ଅନ୍ୟଥା {
			p = ଖାଲି;
		}
		ଯଦି (p != ଖାଲି) {
			// Skip 16-digit hexadecimal.
			ଯେତେବେଳେ (*p && iswascii(*p) && isxdigit(*p))
				++p;
			ଯଦି (is_wprefix(p, L"-pty")) {
				p += 4;
			} ଅନ୍ୟଥା {
				p = ଖାଲି;
			}
		}
		ଯଦି (p != ଖାଲି) {
			// Skip pty number.
			ଯେତେବେଳେ (*p && iswascii(*p) && isdigit(*p))
				++p;
			ଯଦି (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} ଅନ୍ୟଥା ଯଦି (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} ଅନ୍ୟଥା {
				p = ଖାଲି;
			}
		}
	}
	ମୁକ୍ତ(nameinfo);
	ଫେରନ୍ତୁ (p != ଖାଲି);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_cygpty_used(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ret = 0;

	ପାଇଁ (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ଫେରନ୍ତୁ ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
