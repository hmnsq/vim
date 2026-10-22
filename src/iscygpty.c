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

#अन्तर्भाव <ctype.h>
#अन्तर्भाव <io.h>
#अन्तर्भाव <wchar.h>
#अन्तर्भाव <windows.h>

#ifdef USE_FILEEXTD
// VC 7.1 or earlier doesn't support SAL.
# यदि !defined(_MSC_VER) || (_MSC_VER < 1400)
#  define __out
#  define __in
#  define __in_opt
# endif
// Win32 FileID API Library:
// defunct link: microsoft.com/en-us/download/details.aspx?id=22599
// Needed for WinXP.
# include <fileextd.h>
#अन्यथा // USE_FILEEXTD
// VC 8 or earlier.
# यदि defined(_MSC_VER) && (_MSC_VER < 1500)
#  ifdef ENABLE_STUB_IMPL
#   define STUB_IMPL
#  अन्यथा
#   error "Win32 FileID API Library is required for VC2005 or earlier."
#  endif
# endif
#endif // USE_FILEEXTD

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#अन्यथा
# define UNUSED
#endif

#अन्तर्भाव "iscygpty.h"

//#define USE_DYNFILEID
#ifdef USE_DYNFILEID
प्रकारनिर्देश BOOL (WINAPI *pfnGetFileInformationByHandleEx)(
		HANDLE						hFile,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass,
		LPVOID						lpFileInformation,
		DWORD						dwBufferSize);
स्थिर pfnGetFileInformationByHandleEx pGetFileInformationByHandleEx = रिक्त;

# ifndef USE_FILEEXTD
स्थिर BOOL WINAPI stub_GetFileInformationByHandleEx(
		HANDLE						hFile UNUSED,
		FILE_INFO_BY_HANDLE_CLASS	FileInformationClass UNUSED,
		LPVOID						lpFileInformation UNUSED,
		DWORD						dwBufferSize UNUSED)
{
	प्रत्यागम FALSE;
}
# endif

स्थिर शून्य setup_fileid_api(शून्य)
{
	यदि (pGetFileInformationByHandleEx != रिक्त) {
		प्रत्यागम;
	}
	pGetFileInformationByHandleEx = (pfnGetFileInformationByHandleEx)
		GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
				"GetFileInformationByHandleEx");
	यदि (pGetFileInformationByHandleEx == रिक्त) {
# ifdef USE_FILEEXTD
		pGetFileInformationByHandleEx = GetFileInformationByHandleEx;
# अन्यथा
		pGetFileInformationByHandleEx = stub_GetFileInformationByHandleEx;
# endif
	}
}
#अन्यथा
# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()
#endif


#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), परिमाण(prefix) / परिमाण(WCHAR) - 1) == 0)

// Check if the fd is a cygwin/msys's pty.
पूर्णाङ्क is_cygpty(पूर्णाङ्क fd)
{
#ifdef STUB_IMPL
	प्रत्यागम 0;
#अन्यथा
	HANDLE h;
	ध्रुव पूर्णाङ्क size = परिमाण(FILE_NAME_INFO) + परिमाण(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = रिक्त;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	यदि (h == INVALID_HANDLE_मूल्य) {
		प्रत्यागम 0;
	}
	// Cygwin/msys's pty is a pipe.
	यदि (GetFileType(h) != FILE_TYPE_PIPE) {
		प्रत्यागम 0;
	}
	nameinfo = दो_स्मृति(size + परिमाण(WCHAR));
	यदि (nameinfo == रिक्त) {
		प्रत्यागम 0;
	}
	// Check the name of the pipe:
	// "\\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master"
	यदि (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / परिमाण(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		यदि (is_wprefix(p, L"\\cygwin-")) {		// Cygwin
			p += 8;
		} अन्यथा यदि (is_wprefix(p, L"\\msys-")) {	// MSYS and MSYS2
			p += 6;
		} अन्यथा {
			p = रिक्त;
		}
		यदि (p != रिक्त) {
			// Skip 16-digit hexadecimal.
			यावत् (*p && iswascii(*p) && है_षष्ठादशक(*p))
				++p;
			यदि (is_wprefix(p, L"-pty")) {
				p += 4;
			} अन्यथा {
				p = रिक्त;
			}
		}
		यदि (p != रिक्त) {
			// Skip pty number.
			यावत् (*p && iswascii(*p) && है_अंक(*p))
				++p;
			यदि (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} अन्यथा यदि (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} अन्यथा {
				p = रिक्त;
			}
		}
	}
	मुक्त(nameinfo);
	प्रत्यागम (p != रिक्त);
#endif // STUB_IMPL
}

// Check if at least one cygwin/msys pty is used.
पूर्णाङ्क is_cygpty_used(शून्य)
{
	पूर्णाङ्क fd, ret = 0;

	प्रति (fd = 0; fd < 3; fd++) {
		ret |= is_cygpty(fd);
	}
	प्रत्यागम ret;
}

#endif // _WIN32

// vim: set ts=4 sw=4:
