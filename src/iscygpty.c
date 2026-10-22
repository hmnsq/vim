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

#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <io.h>
#ಅನ್ತರ್ಭಾವ <wchar.h>
#ಅನ್ತರ್ಭಾವ <windows.h>

#ifdef USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# ಯದಿ !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#ಅನ್ಯಥಾ // USE_FILEEXTD
// VC 8 or earlier.
# ಯದಿ defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  ಅನ್ಯಥಾ
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#ಅನ್ಯಥಾ
# define UNUSED
#endif

#ಅನ್ತರ್ಭಾವ "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
ಪ್ರಕಾರನಿರ್ದೇಶ BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dwBufferSize);
ಸ್ಥಿರ pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = ರಿಕ್ತ;

# ifndef USE_FILEEXTD
ಸ್ಥಿರ BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dwBufferSize UNUSED)
{
	ಪ್ರತ್ಯಾಗಮ FALSE;
}
# endif

ಸ್ಥಿರ ಶೂನ್ಯ setup_fileid_api(ಶೂನ್ಯ)
{
	ಯದಿ (pGetFileInformationByHandleEx != ರಿಕ್ತ) {
		ಪ್ರತ್ಯಾಗಮ;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	ಯದಿ (pGetFileInformationByHandleEx == ರಿಕ್ತ) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# ಅನ್ಯಥಾ
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#ಅನ್ಯಥಾ
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), ಪರಿಮಾಣ(prefix) / ಪರಿಮಾಣ(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
ಪೂರ್ಣಾಙ್ಕ is_cygpty(ಪೂರ್ಣಾಙ್ಕ fd)
{
#ifdef STUB_IMPL
	ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
	HANDLE h;
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ size = ಪರಿಮಾಣ(FILE_NAME_INFO) + ಪರಿಮಾಣ(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = ರಿಕ್ತ;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	ಯದಿ (h == INVALID_HANDLE_ಮೌಲ್ಯ) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	// Cygwin/msys's pty is a pipe.
	ಯದಿ (GetFileType(h) != FILE_TYPE_PIPE) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	nameinfo = malloc(size + ಪರಿಮಾಣ(WCHAR));
	ಯದಿ (nameinfo == ರಿಕ್ತ) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	ಯದಿ (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / ಪರಿಮಾಣ(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		ಯದಿ (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} ಅನ್ಯಥಾ ಯದಿ (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} ಅನ್ಯಥಾ {
			p = ರಿಕ್ತ;
		}
		ಯದಿ (p != ರಿಕ್ತ) {
			// Skip 16-digit hexadecimal.
			ಯಾವತ್ (*p && iswascii(*p) && isxdigit(*p))
				++p;
			ಯದಿ (is_wprefix(p, L"-pty")) {
				p += 4;
			} ಅನ್ಯಥಾ {
				p = ರಿಕ್ತ;
			}
		}
		ಯದಿ (p != ರಿಕ್ತ) {
			// Skip pty number.
			ಯಾವತ್ (*p && iswascii(*p) && isdigit(*p))
				++p;
			ಯದಿ (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} ಅನ್ಯಥಾ ಯದಿ (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} ಅನ್ಯಥಾ {
				p = ರಿಕ್ತ;
			}
		}
	}
	ಮುಕ್ತ(nameinfo);
	ಪ್ರತ್ಯಾಗಮ (p != ರಿಕ್ತ);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
ಪೂರ್ಣಾಙ್ಕ is_cygpty_used(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ fd, ret = 0;

	ಪ್ರತಿ (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ಪ್ರತ್ಯಾಗಮ ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
