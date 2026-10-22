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

#അന്തര്ഭാവ <ctype.h>
#അന്തര്ഭാവ <io.h>
#അന്തര്ഭാവ <wchar.h>
#അന്തര്ഭാവ <windows.h>

#ifdef USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# എങ്കിൽ !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#അല്ലെങ്കിൽ // USE_FILEEXTD
// VC 8 or earlier.
# എങ്കിൽ defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  അല്ലെങ്കിൽ
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#അല്ലെങ്കിൽ
# define UNUSED
#endif

#അന്തര്ഭാവ "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
തരനിർവചനം BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dwBufferSize);
സ്ഥിതം pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = ശൂന്യ;

# ifndef USE_FILEEXTD
സ്ഥിതം BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dwBufferSize UNUSED)
{
	തിരികെ FALSE;
}
# endif

സ്ഥിതം ശൂന്യം setup_fileid_api(ശൂന്യം)
{
	എങ്കിൽ (pGetFileInformationByHandleEx != ശൂന്യ) {
		തിരികെ;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	എങ്കിൽ (pGetFileInformationByHandleEx == ശൂന്യ) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# അല്ലെങ്കിൽ
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#അല്ലെങ്കിൽ
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), വലുപ്പം(prefix) / വലുപ്പം(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
പൂർണ്ണസംഖ്യ is_cygpty(പൂർണ്ണസംഖ്യ fd)
{
#ifdef STUB_IMPL
	തിരികെ 0;
#അല്ലെങ്കിൽ
	HANDLE h;
	സ്ഥിരം പൂർണ്ണസംഖ്യ size = വലുപ്പം(FILE_NAME_INFO) + വലുപ്പം(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = ശൂന്യ;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	എങ്കിൽ (h == INVALID_HANDLE_മൂല്യം) {
		തിരികെ 0;
	}
	// Cygwin/msys's pty is a pipe.
	എങ്കിൽ (GetFileType(h) != FILE_TYPE_PIPE) {
		തിരികെ 0;
	}
	nameinfo = malloc(size + വലുപ്പം(WCHAR));
	എങ്കിൽ (nameinfo == ശൂന്യ) {
		തിരികെ 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	എങ്കിൽ (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / വലുപ്പം(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		എങ്കിൽ (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} അല്ലെങ്കിൽ എങ്കിൽ (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} അല്ലെങ്കിൽ {
			p = ശൂന്യ;
		}
		എങ്കിൽ (p != ശൂന്യ) {
			// Skip 16-digit hexadecimal.
			ആകുമ്പോൾ (*p && iswascii(*p) && isxdigit(*p))
				++p;
			എങ്കിൽ (is_wprefix(p, L"-pty")) {
				p += 4;
			} അല്ലെങ്കിൽ {
				p = ശൂന്യ;
			}
		}
		എങ്കിൽ (p != ശൂന്യ) {
			// Skip pty number.
			ആകുമ്പോൾ (*p && iswascii(*p) && isdigit(*p))
				++p;
			എങ്കിൽ (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} അല്ലെങ്കിൽ എങ്കിൽ (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} അല്ലെങ്കിൽ {
				p = ശൂന്യ;
			}
		}
	}
	സ്വതന്ത്രം(nameinfo);
	തിരികെ (p != ശൂന്യ);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
പൂർണ്ണസംഖ്യ is_cygpty_used(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ fd, ret = 0;

	വേണ്ടി (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	തിരികെ ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
