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

#அந்தர்பாவ <ctype.h>
#அந்தர்பாவ <io.h>
#அந்தர்பாவ <wchar.h>
#அந்தர்பாவ <windows.h>

#ifdef USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# எனில் !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#இல்லையெனில் // USE_FILEEXTD
// VC 8 or earlier.
# எனில் defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  இல்லையெனில்
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#இல்லையெனில்
# define UNUSED
#endif

#அந்தர்பாவ "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
வகைவரை BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dwBufferSize);
நிலையான pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = வெற்று;

# ifndef USE_FILEEXTD
நிலையான BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dwBufferSize UNUSED)
{
	திரும்பு FALSE;
}
# endif

நிலையான வெறுமை setup_fileid_api(வெறுமை)
{
	எனில் (pGetFileInformationByHandleEx != வெற்று) {
		திரும்பு;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	எனில் (pGetFileInformationByHandleEx == வெற்று) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# இல்லையெனில்
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#இல்லையெனில்
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), அளவு(prefix) / அளவு(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
முழு is_cygpty(முழு fd)
{
#ifdef STUB_IMPL
	திரும்பு 0;
#இல்லையெனில்
	HANDLE h;
	மாறிலி முழு size = அளவு(FILE_NAME_INFO) + அளவு(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = வெற்று;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	எனில் (h == INVALID_HANDLE_மதிப்பு) {
		திரும்பு 0;
	}
	// Cygwin/msys's pty is a pipe.
	எனில் (GetFileType(h) != FILE_TYPE_PIPE) {
		திரும்பு 0;
	}
	nameinfo = malloc(size + அளவு(WCHAR));
	எனில் (nameinfo == வெற்று) {
		திரும்பு 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	எனில் (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / அளவு(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		எனில் (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} இல்லையெனில் எனில் (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} இல்லையெனில் {
			p = வெற்று;
		}
		எனில் (p != வெற்று) {
			// Skip 16-digit hexadecimal.
			போது (*p && iswascii(*p) && isxdigit(*p))
				++p;
			எனில் (is_wprefix(p, L"-pty")) {
				p += 4;
			} இல்லையெனில் {
				p = வெற்று;
			}
		}
		எனில் (p != வெற்று) {
			// Skip pty number.
			போது (*p && iswascii(*p) && isdigit(*p))
				++p;
			எனில் (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} இல்லையெனில் எனில் (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} இல்லையெனில் {
				p = வெற்று;
			}
		}
	}
	விடுவி(nameinfo);
	திரும்பு (p != வெற்று);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
முழு is_cygpty_used(வெறுமை)
{
	முழு fd, ret = 0;

	க்கு (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	திரும்பு ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
