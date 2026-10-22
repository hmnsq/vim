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

#اگر_تعریف_شدہ _WIN32

#شامل <ctype.h>
#شامل <io.h>
#شامل <wchar.h>
#شامل <windows.h>

#اگر_تعریف_شدہ USE_FILEEXTD
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
#ورنہ // USE_FILEEXTD
// VC 8 or earlier.
# اگر defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  ورنہ
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#اگر_ختم // USE_FILEEXTD

#اگر_تعریف_شدہ __MINGW32__
# define غیر_مستعمل __attribute__((غیر_مستعمل_2))
#ورنہ
# define غیر_مستعمل
#اگر_ختم

#شامل "iscygpty.h"

//#define USE_DYNFILEID
#اگر_تعریف_شدہ USE_DYNFILEID
قسمتعریف BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dw_بفر_سائز);
ساکن pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = تہی;

# ifndef USE_FILEEXTD
ساکن BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile غیر_مستعمل,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass غیر_مستعمل,
		LPVOID						lpFileInformation غیر_مستعمل,
		DWORD						dw_بفر_سائز غیر_مستعمل)
{
	واپس FALSE;
}
# endif

ساکن خالی تیاری_fileid_api(خالی)
{
	اگر (pGetFileInformationByHandleEx != تہی) {
		واپس;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		Get_Proc_پتہ(GetModuleHandle(متن_3("kernel32.dll")),
				"GetFileInformationByHandleEx");
	اگر (pGetFileInformationByHandleEx == تہی) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# ورنہ
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#ورنہ
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define تیاری_fileid_api()
#اگر_ختم


#تعریف_کرو is_wprefix(s, سابقہ) \
	(wcsncmp((s), (سابقہ), حجم(سابقہ) / حجم(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
صحیح is_cygpty(صحیح fd)
{
#اگر_تعریف_شدہ STUB_IMPL
	واپس 0;
#ورنہ
	HANDLE h;
	ثابت صحیح سائز = حجم(FILE_NAME_INFO) + حجم(WCHAR) * (MAX_راستہ - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = تہی;

	تیاری_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	اگر (h == INVALID_HANDLE_قدر) {
		واپس 0;
	}
	// Cygwin/msys's pty is a pipe.
	اگر (GetFileType(h) != FILE_TYPE_PIPE) {
		واپس 0;
	}
	nameinfo = حافظہ_دو(سائز + حجم(WCHAR));
	اگر (nameinfo == تہی) {
		واپس 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	اگر (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, سائز)) {
		nameinfo->FileName[nameinfo->File_Name_لمبائی / حجم(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		اگر (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} ورنہ اگر (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} ورنہ {
			p = تہی;
		}
		اگر (p != تہی) {
			// Skip 16-digit hexadecimal.
			جب (*p && iswascii(*p) && isxdigit(*p))
				++p;
			اگر (is_wprefix(p, L"-pty")) {
				p += 4;
			} ورنہ {
				p = تہی;
			}
		}
		اگر (p != تہی) {
			// Skip pty number.
			جب (*p && iswascii(*p) && ہندسہ_ہے(*p))
				++p;
			اگر (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} ورنہ اگر (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} ورنہ {
				p = تہی;
			}
		}
	}
	آزاد(nameinfo);
	واپس (p != تہی);
#اگر_ختم // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
صحیح is_cygpty_مستعمل(خالی)
{
	صحیح fd, ret = 0;

	برائے (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	واپس ret;
}

#اگر_ختم // _WIN32

// vim: set ts=4 sw=4:
