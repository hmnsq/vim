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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32

#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wchar.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <windows.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# ਜੇ !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#ਨਹੀਂਤਾਂ // USE_FILEEXTD
// VC 8 or earlier.
# ਜੇ defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  ਨਹੀਂਤਾਂ
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#ਜੇ_ਅੰਤ // USE_FILEEXTD

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
# define ਅਣਵਰਤਿਆ __attribute__((ਅਣਵਰਤਿਆ_2))
#ਨਹੀਂਤਾਂ
# define ਅਣਵਰਤਿਆ
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "iscygpty.h"

//#define USE_DYNFILEID
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_DYNFILEID
ਕਿਸਮਪਰਿਭਾਸ਼ਾ BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dw_ਬਫਰ_ਮਾਪ);
ਸਥਿਰ pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = ਨਿਰਾ;

# ifndef USE_FILEEXTD
ਸਥਿਰ BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile ਅਣਵਰਤਿਆ,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass ਅਣਵਰਤਿਆ,
		LPVOID						lpFileInformation ਅਣਵਰਤਿਆ,
		DWORD						dw_ਬਫਰ_ਮਾਪ ਅਣਵਰਤਿਆ)
{
	ਵਾਪਸ FALSE;
}
# endif

ਸਥਿਰ ਖਾਲੀ ਤਿਆਰੀ_fileid_api(ਖਾਲੀ)
{
	ਜੇ (pGetFileInformationByHandleEx != ਨਿਰਾ) {
		ਵਾਪਸ;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		Get_Proc_ਪਤਾ(GetModuleHandle(ਪਾਠ_3("kernel32.dll")),
				"GetFileInformationByHandleEx");
	ਜੇ (pGetFileInformationByHandleEx == ਨਿਰਾ) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# ਨਹੀਂਤਾਂ
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#ਨਹੀਂਤਾਂ
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define ਤਿਆਰੀ_fileid_api()
#ਜੇ_ਅੰਤ


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ is_wprefix(s, ਅਗੇਤਰ) \
	(wcsncmp((s), (ਅਗੇਤਰ), ਆਕਾਰ(ਅਗੇਤਰ) / ਆਕਾਰ(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
ਪੂਰਨ is_cygpty(ਪੂਰਨ fd)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ STUB_IMPL
	ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
	HANDLE h;
	ਅਚਲ ਪੂਰਨ ਮਾਪ = ਆਕਾਰ(FILE_NAME_INFO) + ਆਕਾਰ(WCHAR) * (MAX_ਰਸਤਾ - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = ਨਿਰਾ;

	ਤਿਆਰੀ_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	ਜੇ (h == INVALID_HANDLE_ਮੁੱਲ) {
		ਵਾਪਸ 0;
	}
	// Cygwin/msys's pty is a pipe.
	ਜੇ (GetFileType(h) != FILE_TYPE_PIPE) {
		ਵਾਪਸ 0;
	}
	nameinfo = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਮਾਪ + ਆਕਾਰ(WCHAR));
	ਜੇ (nameinfo == ਨਿਰਾ) {
		ਵਾਪਸ 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	ਜੇ (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, ਮਾਪ)) {
		nameinfo->FileName[nameinfo->File_Name_ਲੰਮਾਈ / ਆਕਾਰ(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		ਜੇ (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} ਨਹੀਂਤਾਂ ਜੇ (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} ਨਹੀਂਤਾਂ {
			p = ਨਿਰਾ;
		}
		ਜੇ (p != ਨਿਰਾ) {
			// Skip 16-digit hexadecimal.
			ਜਦੋਂ (*p && iswascii(*p) && isxdigit(*p))
				++p;
			ਜੇ (is_wprefix(p, L"-pty")) {
				p += 4;
			} ਨਹੀਂਤਾਂ {
				p = ਨਿਰਾ;
			}
		}
		ਜੇ (p != ਨਿਰਾ) {
			// Skip pty number.
			ਜਦੋਂ (*p && iswascii(*p) && ਅੰਕ_ਹੈ(*p))
				++p;
			ਜੇ (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} ਨਹੀਂਤਾਂ ਜੇ (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} ਨਹੀਂਤਾਂ {
				p = ਨਿਰਾ;
			}
		}
	}
	ਮੁਕਤ(nameinfo);
	ਵਾਪਸ (p != ਨਿਰਾ);
#ਜੇ_ਅੰਤ // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
ਪੂਰਨ is_cygpty_ਵਰਤਿਆ(ਖਾਲੀ)
{
	ਪੂਰਨ fd, ret = 0;

	ਲਈ (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	ਵਾਪਸ ret;
}

#ਜੇ_ਅੰਤ // _WIN32

// vim: set ts=4 sw=4:
