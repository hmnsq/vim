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

#నిర్వచితమైతే _WIN32

#అన్తర్భావ <ctype.h>
#అన్తర్భావ <io.h>
#అన్తర్భావ <wchar.h>
#అన్తర్భావ <windows.h>

#నిర్వచితమైతే USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# ఒకవేళ !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#లేకపోతే // USE_FILEEXTD
// VC 8 or earlier.
# ఒకవేళ defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  లేకపోతే
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#అయితే_ముగింపు // USE_FILEEXTD

#నిర్వచితమైతే __MINGW32__
# define వాడని __attribute__((వాడని_2))
#లేకపోతే
# define వాడని
#అయితే_ముగింపు

#అన్తర్భావ "iscygpty.h"

//#define USE_DYNFILEID
#నిర్వచితమైతే USE_DYNFILEID
రకనిర్వచనం BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dw_బఫరు_కొలత);
స్థిర pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = ఖాళీ;

# ifndef USE_FILEEXTD
స్థిర BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile వాడని,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass వాడని,
		LPVOID						lpFileInformation వాడని,
		DWORD						dw_బఫరు_కొలత వాడని)
{
	తిరిగి FALSE;
}
# endif

స్థిర శూన్య అమరిక_fileid_api(శూన్య)
{
	ఒకవేళ (pGetFileInformationByHandleEx != ఖాళీ) {
		తిరిగి;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		Get_Proc_చిరునామా(GetModuleHandle(పాఠ్యం_3("kernel32.dll")),
				"GetFileInformationByHandleEx");
	ఒకవేళ (pGetFileInformationByHandleEx == ఖాళీ) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# లేకపోతే
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#లేకపోతే
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define అమరిక_fileid_api()
#అయితే_ముగింపు


#నిర్వచించు is_wprefix(s, ఉపసర్గ) \
	(wcsncmp((s), (ఉపసర్గ), పరిమాణం(ఉపసర్గ) / పరిమాణం(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
పూర్ణాంకం is_cygpty(పూర్ణాంకం fd)
{
#నిర్వచితమైతే STUB_IMPL
	తిరిగి 0;
#లేకపోతే
	HANDLE h;
	స్థిరరాశి పూర్ణాంకం కొలత = పరిమాణం(FILE_NAME_INFO) + పరిమాణం(WCHAR) * (MAX_మార్గం - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = ఖాళీ;

	అమరిక_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	ఒకవేళ (h == INVALID_HANDLE_విలువ) {
		తిరిగి 0;
	}
	// Cygwin/msys's pty is a pipe.
	ఒకవేళ (GetFileType(h) != FILE_TYPE_PIPE) {
		తిరిగి 0;
	}
	nameinfo = జ్ఞాపకం_ఇవ్వు(కొలత + పరిమాణం(WCHAR));
	ఒకవేళ (nameinfo == ఖాళీ) {
		తిరిగి 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	ఒకవేళ (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, కొలత)) {
		nameinfo->FileName[nameinfo->File_Name_నిడివి / పరిమాణం(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		ఒకవేళ (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} లేకపోతే ఒకవేళ (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} లేకపోతే {
			p = ఖాళీ;
		}
		ఒకవేళ (p != ఖాళీ) {
			// Skip 16-digit hexadecimal.
			అయితే (*p && iswascii(*p) && isxdigit(*p))
				++p;
			ఒకవేళ (is_wprefix(p, L"-pty")) {
				p += 4;
			} లేకపోతే {
				p = ఖాళీ;
			}
		}
		ఒకవేళ (p != ఖాళీ) {
			// Skip pty number.
			అయితే (*p && iswascii(*p) && అంకెనా(*p))
				++p;
			ఒకవేళ (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} లేకపోతే ఒకవేళ (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} లేకపోతే {
				p = ఖాళీ;
			}
		}
	}
	విడుదల(nameinfo);
	తిరిగి (p != ఖాళీ);
#అయితే_ముగింపు // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
పూర్ణాంకం is_cygpty_వాడిన(శూన్య)
{
	పూర్ణాంకం fd, ret = 0;

	కొరకు (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	తిరిగి ret;
}

#అయితే_ముగింపు // _WIN32

// vim: set ts=4 sw=4:
