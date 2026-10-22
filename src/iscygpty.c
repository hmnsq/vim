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

#جيڪڏهن_وصف_ٿيل _WIN32

#ضمن <ctype.h>
#ضمن <io.h>
#ضمن <wchar.h>
#ضمن <windows.h>

#جيڪڏهن_وصف_ٿيل USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# إذا !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#وإلا // USE_FILEEXTD
// VC 8 or earlier.
# إذا defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  وإلا
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#جيڪڏهن_پورو // USE_FILEEXTD

#جيڪڏهن_وصف_ٿيل __MINGW32__
# define اڻ_استعمال_ٿيل __attribute__((اڻ_استعمال_ٿيل_2))
#وإلا
# define اڻ_استعمال_ٿيل
#جيڪڏهن_پورو

#ضمن "iscygpty.h"

//#define USE_DYNFILEID
#جيڪڏهن_وصف_ٿيل USE_DYNFILEID
تعريفنوع BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dw_بفر_سائيز);
ساكن pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = عدم;

# ifndef USE_FILEEXTD
ساكن BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile اڻ_استعمال_ٿيل,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass اڻ_استعمال_ٿيل,
		LPVOID						lpFileInformation اڻ_استعمال_ٿيل,
		DWORD						dw_بفر_سائيز اڻ_استعمال_ٿيل)
{
	أرجع FALSE;
}
# endif

ساكن فراغ تياري_fileid_api(فراغ)
{
	إذا (pGetFileInformationByHandleEx != عدم) {
		أرجع;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		Get_Proc_پتو(GetModuleHandle(متن_3("kernel32.dll")),
				"GetFileInformationByHandleEx");
	إذا (pGetFileInformationByHandleEx == عدم) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# وإلا
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#وإلا
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define تياري_fileid_api()
#جيڪڏهن_پورو


#وصف_ڪريو is_wprefix(s, اڳياڙي) \
	(wcsncmp((s), (اڳياڙي), حجم(اڳياڙي) / حجم(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
صحيح is_cygpty(صحيح fd)
{
#جيڪڏهن_وصف_ٿيل STUB_IMPL
	أرجع 0;
#وإلا
	HANDLE h;
	ثابت صحيح سائيز = حجم(FILE_NAME_INFO) + حجم(WCHAR) * (MAX_رستو - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = عدم;

	تياري_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	إذا (h == INVALID_HANDLE_قدر) {
		أرجع 0;
	}
	// Cygwin/msys's pty is a pipe.
	إذا (GetFileType(h) != FILE_TYPE_PIPE) {
		أرجع 0;
	}
	nameinfo = يادگيري_ڏيو(سائيز + حجم(WCHAR));
	إذا (nameinfo == عدم) {
		أرجع 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	إذا (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, سائيز)) {
		nameinfo->FileName[nameinfo->File_Name_ڊيگهائي / حجم(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		إذا (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} وإلا إذا (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} وإلا {
			p = عدم;
		}
		إذا (p != عدم) {
			// Skip 16-digit hexadecimal.
			طالما (*p && iswascii(*p) && isxdigit(*p))
				++p;
			إذا (is_wprefix(p, L"-pty")) {
				p += 4;
			} وإلا {
				p = عدم;
			}
		}
		إذا (p != عدم) {
			// Skip pty number.
			طالما (*p && iswascii(*p) && انگ_آهي(*p))
				++p;
			إذا (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} وإلا إذا (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} وإلا {
				p = عدم;
			}
		}
	}
	حرر(nameinfo);
	أرجع (p != عدم);
#جيڪڏهن_پورو // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
صحيح is_cygpty_استعمال_ٿيل(فراغ)
{
	صحيح fd, ret = 0;

	لكل (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	أرجع ret;
}

#جيڪڏهن_پورو // _WIN32

// vim: set ts=4 sw=4:
