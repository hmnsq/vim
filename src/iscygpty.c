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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32

#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <io.h>
#ಅನ್ತರ್ಭಾವ <wchar.h>
#ಅನ್ತರ್ಭಾವ <windows.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# ಆದರೆ !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#ಇಲ್ಲದಿದ್ದರೆ // USE_FILEEXTD
// VC 8 or earlier.
# ಆದರೆ defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  ಇಲ್ಲದಿದ್ದರೆ
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ // USE_FILEEXTD

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
# define ಬಳಸದ __attribute__((ಬಳಸದ_2))
#ಇಲ್ಲದಿದ್ದರೆ
# define ಬಳಸದ
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "iscygpty.h"

//#define USE_DYNFILEID
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_DYNFILEID
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dw_ಬಫರ್_ಗಾತ್ರ);
ಸ್ಥಿರ pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = ಖಾಲಿ;

# ifndef USE_FILEEXTD
ಸ್ಥಿರ BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile ಬಳಸದ,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass ಬಳಸದ,
		LPVOID						lpFileInformation ಬಳಸದ,
		DWORD						dw_ಬಫರ್_ಗಾತ್ರ ಬಳಸದ)
{
	ಹಿಂತಿರುಗಿ FALSE;
}
# endif

ಸ್ಥಿರ ಶೂನ್ಯ ಸಿದ್ಧತೆ_fileid_api(ಶೂನ್ಯ)
{
	ಆದರೆ (pGetFileInformationByHandleEx != ಖಾಲಿ) {
		ಹಿಂತಿರುಗಿ;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		Get_Proc_ವಿಳಾಸ(GetModuleHandle(ಪಠ್ಯ_3("kernel32.dll")),
				"GetFileInformationByHandleEx");
	ಆದರೆ (pGetFileInformationByHandleEx == ಖಾಲಿ) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# ಇಲ್ಲದಿದ್ದರೆ
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#ಇಲ್ಲದಿದ್ದರೆ
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define ಸಿದ್ಧತೆ_fileid_api()
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿಸು is_wprefix(s, ಪೂರ್ವಪ್ರತ್ಯಯ) \
	(wcsncmp((s), (ಪೂರ್ವಪ್ರತ್ಯಯ), ಗಾತ್ರ(ಪೂರ್ವಪ್ರತ್ಯಯ) / ಗಾತ್ರ(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
ಪೂರ್ಣಾಂಕ is_cygpty(ಪೂರ್ಣಾಂಕ fd)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ STUB_IMPL
	ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
	HANDLE h;
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = ಗಾತ್ರ(FILE_NAME_INFO) + ಗಾತ್ರ(WCHAR) * (MAX_ಮಾರ್ಗ - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = ಖಾಲಿ;

	ಸಿದ್ಧತೆ_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	ಆದರೆ (h == INVALID_HANDLE_ಮೌಲ್ಯ) {
		ಹಿಂತಿರುಗಿ 0;
	}
	// Cygwin/msys's pty is a pipe.
	ಆದರೆ (GetFileType(h) != FILE_TYPE_PIPE) {
		ಹಿಂತಿರುಗಿ 0;
	}
	nameinfo = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ_2 + ಗಾತ್ರ(WCHAR));
	ಆದರೆ (nameinfo == ಖಾಲಿ) {
		ಹಿಂತಿರುಗಿ 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	ಆದರೆ (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, ಗಾತ್ರ_2)) {
		nameinfo->FileName[nameinfo->File_Name_ಉದ್ದಳತೆ / ಗಾತ್ರ(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		ಆದರೆ (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} ಇಲ್ಲದಿದ್ದರೆ {
			p = ಖಾಲಿ;
		}
		ಆದರೆ (p != ಖಾಲಿ) {
			// Skip 16-digit hexadecimal.
			ಆಗ (*p && iswascii(*p) && isxdigit(*p))
				++p;
			ಆದರೆ (is_wprefix(p, L"-pty")) {
				p += 4;
			} ಇಲ್ಲದಿದ್ದರೆ {
				p = ಖಾಲಿ;
			}
		}
		ಆದರೆ (p != ಖಾಲಿ) {
			// Skip pty number.
			ಆಗ (*p && iswascii(*p) && ಅಂಕಿಯೇ(*p))
				++p;
			ಆದರೆ (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} ಇಲ್ಲದಿದ್ದರೆ {
				p = ಖಾಲಿ;
			}
		}
	}
	ಬಿಡುಗಡೆ(nameinfo);
	ಹಿಂತಿರುಗಿ (p != ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
ಪೂರ್ಣಾಂಕ is_cygpty_ಬಳಸಿದ(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ fd, ret = 0;

	ಗಾಗಿ (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ಹಿಂತಿರುಗಿ ret;
}

#ಆದರೆ_ಅಂತ್ಯ // _WIN32

// vim: set ts=4 sw=4:
