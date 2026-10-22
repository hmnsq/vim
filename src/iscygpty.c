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

#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <io.h>
#અન્તર્ભાવ <wchar.h>
#અન્તર્ભાવ <windows.h>

#ifdef USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# જો !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#નહીંતર // USE_FILEEXTD
// VC 8 or earlier.
# જો defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  નહીંતર
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#નહીંતર
# define UNUSED
#endif

#અન્તર્ભાવ "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
પ્રકારવ્યાખ્યા BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dwBufferSize);
સ્થિર pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = ખાલી;

# ifndef USE_FILEEXTD
સ્થિર BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dwBufferSize UNUSED)
{
	પાછા FALSE;
}
# endif

સ્થિર શૂન્ય setup_fileid_api(શૂન્ય)
{
	જો (pGetFileInformationByHandleEx != ખાલી) {
		પાછા;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	જો (pGetFileInformationByHandleEx == ખાલી) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# નહીંતર
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#નહીંતર
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), કદ(prefix) / કદ(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
પૂર્ણાંક is_cygpty(પૂર્ણાંક fd)
{
#ifdef STUB_IMPL
	પાછા 0;
#નહીંતર
	HANDLE h;
	અચલ પૂર્ણાંક size = કદ(FILE_NAME_INFO) + કદ(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = ખાલી;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	જો (h == INVALID_HANDLE_કિંમત) {
		પાછા 0;
	}
	// Cygwin/msys's pty is a pipe.
	જો (GetFileType(h) != FILE_TYPE_PIPE) {
		પાછા 0;
	}
	nameinfo = malloc(size + કદ(WCHAR));
	જો (nameinfo == ખાલી) {
		પાછા 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	જો (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / કદ(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		જો (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} નહીંતર જો (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} નહીંતર {
			p = ખાલી;
		}
		જો (p != ખાલી) {
			// Skip 16-digit hexadecimal.
			જ્યારે (*p && iswascii(*p) && isxdigit(*p))
				++p;
			જો (is_wprefix(p, L"-pty")) {
				p += 4;
			} નહીંતર {
				p = ખાલી;
			}
		}
		જો (p != ખાલી) {
			// Skip pty number.
			જ્યારે (*p && iswascii(*p) && isdigit(*p))
				++p;
			જો (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} નહીંતર જો (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} નહીંતર {
				p = ખાલી;
			}
		}
	}
	મુક્ત(nameinfo);
	પાછા (p != ખાલી);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
પૂર્ણાંક is_cygpty_used(શૂન્ય)
{
	પૂર્ણાંક fd, ret = 0;

	માટે (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	પાછા ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
