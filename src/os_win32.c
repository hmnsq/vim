/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * os_win32.c
 *
 * Used for both the console version and the Win32 GUI.  A lot of code is for
 * the console version only, so there is a lot of "#ifndef FEAT_GUI_MSWIN".
 *
 * Win32 system-dependent routines.
 * Portions lifted from the Win32 SDK samples, the MSDOS-dependent code,
 * NetHack 3.1.3, GNU Emacs 19.30, and Vile 5.5.
 *
 * George V. Reilly <george@reilly.org> wrote most of this.
 * Roger Knobbe <rogerk@wonderware.com> did the initial port of Vim 3.0.
 */

#ضمن "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#ضمن <sys/types.h>
#ضمن <signal.h>
#ضمن <limits.h>

#ضمن <process.h>
#ضمن <winternl.h>
#ضمن <direct.h>

#אם !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#ifdef FEAT_JOB_CHANNEL
# include <tlhelp32.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = عدم;
#endif

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
ساكن HANDLE g_hConIn  = INVALID_HANDLE_VALUE;
ساكن HANDLE g_hConOut = INVALID_HANDLE_VALUE;

// Win32 Screen buffer,coordinate,console I/O information
ساكن SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
ساكن COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
ساكن WORD  g_attrDefault = 7;  // lightgray text on black background
ساكن WORD  g_attrCurrent;

ساكن صحيح g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
ساكن صحيح g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
ساكن صحيح g_fForceExit = FALSE;    // set when forcefully exiting

ساكن فراغ scroll(unsigned cLines);
ساكن فراغ set_scroll_region(unsigned left, unsigned top,
			      unsigned right, unsigned bottom);
ساكن فراغ set_scroll_region_tb(unsigned top, unsigned bottom);
ساكن فراغ set_scroll_region_lr(unsigned left, unsigned right);
ساكن فراغ insert_lines(unsigned cLines);
ساكن فراغ delete_lines(unsigned cLines);
ساكن فراغ gotoxy(unsigned x, unsigned y);
ساكن فراغ standout(فراغ);
ساكن صحيح s_cursor_visible = TRUE;
ساكن صحيح did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
تعريفنوع بنية input_record_buffer_node_S
{
    INPUT_RECORD ir;
    بنية input_record_buffer_node_S *next;
} input_record_buffer_node_T;
تعريفنوع بنية input_record_buffer_S
{
    input_record_buffer_node_T *head;
    input_record_buffer_node_T *tail;
    صحيح length;
} input_record_buffer_T;
ساكن input_record_buffer_T input_record_buffer;
ساكن صحيح read_input_record_buffer(INPUT_RECORD* irEvents, صحيح nMaxLength);
ساكن صحيح write_input_record_buffer(INPUT_RECORD* irEvents, صحيح nLength);
#endif
#ifdef FEAT_GUI_MSWIN
ساكن صحيح s_dont_use_vimrun = TRUE;
ساكن صحيح need_vimrun_warning = FALSE;
ساكن string_T vimrun_path = {(char_u *)"vimrun ", 7};
ساكن صحيح vimrun_path_allocated = FALSE;
#endif

ساكن صحيح win32_getattrs(char_u *name);
ساكن صحيح win32_setattrs(char_u *name, صحيح attrs);
ساكن صحيح win32_set_archive(char_u *name);

ساكن صحيح conpty_working = 0;
ساكن صحيح conpty_type = 0;
ساكن صحيح conpty_stable = 0;
ساكن صحيح conpty_fix_type = 0;
ساكن فراغ vtp_flag_init(فراغ);

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساكن صحيح vtp_working = 0;
ساكن فراغ vtp_init(فراغ);
ساكن فراغ vtp_exit(فراغ);
ساكن فراغ vtp_sgr_bulk(صحيح arg);
ساكن فراغ vtp_sgr_bulks(صحيح argc, صحيح *argv);

ساكن صحيح wt_working = 0;
ساكن فراغ wt_init(فراغ);

ساكن صحيح g_color_index_bg = 0;
ساكن صحيح g_color_index_fg = 7;

# ifdef FEAT_TERMGUICOLORS
ساكن guicolor_T save_console_bg_rgb;
ساكن guicolor_T save_console_fg_rgb;
ساكن guicolor_T store_console_bg_rgb;
ساكن guicolor_T store_console_fg_rgb;
ساكن صحيح default_console_color_bg = 0x000000; // black
ساكن صحيح default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_term_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# אחרת
#  define USE_VTP		0
#  define USE_WT		0
# endif

ساكن فراغ set_console_color_rgb(فراغ);
ساكن فراغ reset_console_color_rgb(فراغ);
ساكن فراغ restore_console_color_rgb(فراغ);
#endif  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساكن صحيح suppress_winsize = 1;	// don't fiddle with console
#endif

ساكن WCHAR *exe_pathw = عدم;

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساكن BOOL use_alternate_screen_buffer = FALSE;
#endif

خارجي DWORD win_version; // this is in os_mswin.c

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ساكن BOOL
is_ambiwidth_event(
    INPUT_RECORD *ir)
{
    החזר ir->EventType == KEY_EVENT
		&& ir->Event.KeyEvent.bKeyDown
		&& ir->Event.KeyEvent.wRepeatCount == 1
		&& ir->Event.KeyEvent.wVirtualKeyCode == 0x12
		&& ir->Event.KeyEvent.wVirtualScanCode == 0x38
		&& ir->Event.KeyEvent.uChar.UnicodeChar == 0
		&& ir->Event.KeyEvent.dwControlKeyState == 2;
}

    ساكن فراغ
make_ambiwidth_event(
    INPUT_RECORD *down,
    INPUT_RECORD *up)
{
    down->Event.KeyEvent.wVirtualKeyCode = 0;
    down->Event.KeyEvent.wVirtualScanCode = 0;
    down->Event.KeyEvent.uChar.UnicodeChar
				    = up->Event.KeyEvent.uChar.UnicodeChar;
    down->Event.KeyEvent.dwControlKeyState = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    ساكن BOOL
read_console_input(
    HANDLE	    hInput,
    INPUT_RECORD    *lpBuffer,
    صحيح		    nLength,
    LPDWORD	    lpEvents)
{
    تعداد
    {
	IRSIZE = 10
    };
    ساكن INPUT_RECORD s_irCache[IRSIZE];
    ساكن DWORD s_dwIndex = 0;
    ساكن DWORD s_dwMax = 0;
    DWORD dwEvents;
    صحيح head;
    صحيح tail;
    صحيح i;
    ساكن INPUT_RECORD s_irPseudo;

    אם (s_dwMax == 0 && input_record_buffer.length > 0)
    {
	dwEvents = read_input_record_buffer(s_irCache, IRSIZE);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
    }

    אם (nLength == -2)
	החזר (s_dwMax > 0) ? TRUE : FALSE;

    אם (win_version < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	אם (nLength == -1)
	    החזר PeekConsoleInputW(hInput, lpBuffer, 1, lpEvents);
	החזר ReadConsoleInputW(hInput, lpBuffer, 1, &dwEvents);
    }

    אם (s_dwMax == 0)
    {
	אם (!vtp_working && nLength == -1)
	    החזר PeekConsoleInputW(hInput, lpBuffer, 1, lpEvents);
	GetNumberOfConsoleInputEvents(hInput, &dwEvents);
	אם (dwEvents == 0 && nLength == -1)
	    החזר PeekConsoleInputW(hInput, lpBuffer, 1, lpEvents);
	ReadConsoleInputW(hInput, s_irCache, IRSIZE, &dwEvents);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
	אם (dwEvents == 0)
	{
	    *lpEvents = 0;
	    החזר TRUE;
	}

	עבור (i = s_dwIndex; i < (صحيح)s_dwMax - 1; ++i)
	    אם (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	אם (s_dwMax > 1)
	{
	    head = 0;
	    tail = s_dwMax - 1;
	    כלעוד (head != tail)
	    {
		אם (s_irCache[head].EventType == WINDOW_BUFFER_SIZE_EVENT
			&& s_irCache[head + 1].EventType
						  == WINDOW_BUFFER_SIZE_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    עבור (i = head; i < tail; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --tail;
		    استمر;
		}
		head++;
	    }
	    s_dwMax = tail + 1;
	}
    }

    אם (s_irCache[s_dwIndex].EventType == KEY_EVENT)
    {
	אם (s_irCache[s_dwIndex].Event.KeyEvent.wRepeatCount > 1)
	{
	    s_irPseudo = s_irCache[s_dwIndex];
	    s_irPseudo.Event.KeyEvent.wRepeatCount = 1;
	    s_irCache[s_dwIndex].Event.KeyEvent.wRepeatCount--;
	    *lpBuffer = s_irPseudo;
	    *lpEvents = 1;
	    החזר TRUE;
	}
    }

    *lpBuffer = s_irCache[s_dwIndex];
    אם (!(nLength == -1 || nLength == -2) && ++s_dwIndex >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    החזר TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    ساكن BOOL
peek_console_input(
    HANDLE	    hInput,
    INPUT_RECORD    *lpBuffer,
    DWORD	    nLength UNUSED,
    LPDWORD	    lpEvents)
{
    החזר read_console_input(hInput, lpBuffer, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    ساكن DWORD
msg_wait_for_multiple_objects(
    DWORD    nCount,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dwWakeMask)
{
    אם (read_console_input(عدم, عدم, -2, عدم))
	החזר WAIT_OBJECT_0;
    החזר MsgWaitForMultipleObjects(nCount, pHandles, fWaitAll,
				     dwMilliseconds, dwWakeMask);
}
# endif

# ifndef FEAT_CLIENTSERVER
    ساكن DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    אם (read_console_input(عدم, عدم, -2, عدم))
	החזר WAIT_OBJECT_0;
    החזר WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#endif   // !FEAT_GUI_MSWIN || VIMDLL

    فراغ
mch_get_exe_name(فراغ)
{
    صحيح		updated = FALSE;
    ساكن صحيح	enc_prev = -1;
    WCHAR	*path;
    size_t	pathlen;

    אם (exe_name == عدم || exe_pathw == عدم || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_PATH];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(عدم, buf, MAX_PATH);
	אם (p > buf)
	{
	    אם (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_to_enc(buf, عدم);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    עשה
	    {
		--p;
		אם (*p == L'\\')
		{
		    *p = L'\0';
		    اكسر;
		}
	    } כלעוד (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    אם (exe_pathw == عدم || !updated)
	החזר;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    path = _wgetenv(L"PATH");
    pathlen = (path == عدم) ? 0 : wcslen(path);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#define EXTRA_LEN 7
#define MAX_ENV_STRING_LEN 8191

    אם (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   temp[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	אם (pathlen == 0)
	    wcscpy(temp + templen, exe_pathw);
	אחרת
	{
	    wcscpy(temp + templen, path);

	    // Check if exe_path is already included in $PATH.
	    אם (wcsstr(temp, exe_pathw) == عدم)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		אם (temp[templen - 1] != L';')
		{
		    wcscpy(temp + templen, L";");
		    ++templen;
		}

		wcscpy(temp + templen, exe_pathw);
	    }
	}

	_wputenv(temp);
#ifdef libintl_wputenv
	libintl_wputenv(temp);
#endif
    }
#undef EXTRA_LEN
#undef MAX_ENV_PATH_LEN
}

/*
 * Unescape characters in "p" that appear in "escaped".
 */
    ساكن فراغ
unescape_shellxquote(char_u *p, char_u *escaped)
{
    صحيح	    l = (صحيح)STRLEN(p);
    صحيح	    n;

    כלעוד (*p != NUL)
    {
	אם (*p == '^' && vim_strchr(escaped, p[1]) != عدم)
	    mch_memmove(p, p + 1, l--);
	n = (*mb_ptr2len)(p);
	p += n;
	l -= n;
    }
}

/*
 * Load library "name".
 */
    HINSTANCE
vimLoadLib(ثابت حرف *name)
{
    HINSTANCE	dll = عدم;

    // No need to load any library when registering OLE.
    אם (found_register_arg)
	החזר عدم;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    אם (exe_pathw == عدم)
    {
	mch_get_exe_name();
	אם (exe_pathw == عدم)
	    החזר عدم;
    }

    WCHAR old_dirw[MAXPATHL];

    אם (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	החזר عدم;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    SetCurrentDirectoryW(exe_pathw);
    dll = LoadLibrary(name);
    SetCurrentDirectoryW(old_dirw);
    החזר dll;
}

#אם defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    صحيح
mch_is_gui_executable(فراغ)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(عدم);
    PIMAGE_DOS_HEADER	pDOS = (PIMAGE_DOS_HEADER)pImage;
    PIMAGE_NT_HEADERS	pPE;

    אם (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	החזר FALSE;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    אם (pPE->Signature != IMAGE_NT_SIGNATURE)
	החזר FALSE;
    אם (pPE->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	החזר TRUE;
    החזר FALSE;
}
#endif

#אם defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    ساكن فراغ *
get_imported_func_info(HINSTANCE hInst, ثابت حرف *funcname, صحيح info,
	ثابت فراغ *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_HEADER		pDOS = (PIMAGE_DOS_HEADER)hInst;
    PIMAGE_NT_HEADERS		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_DATA		pIAT;	    // Import Address Table
    PIMAGE_THUNK_DATA		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    אם (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	החזר عدم;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    אם (pPE->Signature != IMAGE_NT_SIGNATURE)
	החזר عدم;

    ImpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    אם (ImpVA == 0)
	החזר عدم;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    עבור (; pImpDesc->FirstThunk; ++pImpDesc)
    {
	אם (!pImpDesc->OriginalFirstThunk)
	    استمر;
	pIAT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->FirstThunk);
	pINT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->OriginalFirstThunk);
	עבור (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    אם (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		استمر;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.AddressOfData));
	    אם (strcmp((حرف *)pImpName->Name, funcname) == 0)
	    {
		فراغ *original;
		DWORD old, new = PAGE_READWRITE;

		بدل (info)
		{
		    حالة 0:
			החזר (فراغ *)pIAT->u1.Function;
		    حالة 1:
			החזר (فراغ *)(pImage + pImpDesc->Name);
		    حالة 2:
			original = (فراغ *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, حجم(فراغ *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, حجم(فراغ *),
				old, &new);
			החזר original;
		    افتراضي:
			החזר عدم;
		}
	    }
	}
    }
    החזר عدم;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
find_imported_module_by_funcname(HINSTANCE hInst, ثابت حرف *funcname)
{
    حرف    *modulename;

    modulename = (حرف *)get_imported_func_info(hInst, funcname, 1, عدم);
    אם (modulename != عدم)
	החזר GetModuleHandleA(modulename);
    החזר عدم;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    فراغ *
get_dll_import_func(HINSTANCE hInst, ثابت حرف *funcname)
{
    החזר get_imported_func_info(hInst, funcname, 0, عدم);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    فراغ *
hook_dll_import_func(HINSTANCE hInst, ثابت حرف *funcname, ثابت فراغ *hook)
{
    החזר get_imported_func_info(hInst, funcname, 2, hook);
}
#endif

#אם defined(FEAT_PYTHON3)
/*
 * Check if the specified DLL is a function forwarder.
 * If yes, return the instance of the forwarded DLL.
 * If no, return the specified DLL.
 * If error, return NULL.
 * This assumes that the DLL forwards all the function to a single DLL.
 */
    HINSTANCE
get_forwarded_dll(HINSTANCE hInst)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_HEADER		pDOS = (PIMAGE_DOS_HEADER)hInst;
    PIMAGE_NT_HEADERS		pPE;
    PIMAGE_EXPORT_DIRECTORY	pExpDir;
    DWORD			ExpVA;
    DWORD			ExpSize;
    LPDWORD			pFunctionTable;

    אם (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	החזר عدم;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    אם (pPE->Signature != IMAGE_NT_SIGNATURE)
	החזר عدم;

    ExpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    ExpSize = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    אם (ExpVA == 0)
	החזר hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    pFunctionTable = (LPDWORD)(pImage + pExpDir->AddressOfFunctions);

    אם (pExpDir->NumberOfNames == 0)
	החזר hInst;	// No export names.

    // Check only the first entry.
    אם ((pFunctionTable[0] < ExpVA) || (pFunctionTable[0] >= ExpVA + ExpSize))
	// The first entry is not a function forwarder.
	החזר hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    ثابت حرف *name = (ثابت حرف *)(pImage + pFunctionTable[0]);
    ثابت حرف *p = strchr(name, '.');
    אם (p == عدم)
	החזר hInst;

    // Extract DllName.
    حرف buf[MAX_PATH];
    אם (p - name + 1 > حجم(buf))
	החזר عدم;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    החזר GetModuleHandleA(buf);
}
#endif

#אם defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
ساكن حرف *null_libintl_gettext(ثابت حرف *);
ساكن حرف *null_libintl_ngettext(ثابت حرف *, ثابت حرف *, unsigned long n);
ساكن حرف *null_libintl_textdomain(ثابت حرف *);
ساكن حرف *null_libintl_bindtextdomain(ثابت حرف *, ثابت حرف *);
ساكن حرف *null_libintl_bind_textdomain_codeset(ثابت حرف *, ثابت حرف *);
ساكن صحيح null_libintl_wputenv(ثابت wchar_t *);

ساكن HINSTANCE hLibintlDLL = عدم;
حرف *(*dyn_libintl_gettext)(ثابت حرف *) = null_libintl_gettext;
حرف *(*dyn_libintl_ngettext)(ثابت حرف *, ثابت حرف *, unsigned long n)
						= null_libintl_ngettext;
حرف *(*dyn_libintl_textdomain)(ثابت حرف *) = null_libintl_textdomain;
حرف *(*dyn_libintl_bindtextdomain)(ثابت حرف *, ثابت حرف *)
						= null_libintl_bindtextdomain;
حرف *(*dyn_libintl_bind_textdomain_codeset)(ثابت حرف *, ثابت حرف *)
				       = null_libintl_bind_textdomain_codeset;
صحيح (*dyn_libintl_wputenv)(ثابت wchar_t *) = null_libintl_wputenv;
صحيح *dyn_libintl_nl_msg_cat_cntr = عدم;

    صحيح
dyn_libintl_init(فراغ)
{
    صحيح i;
    ساكن بنية
    {
	حرف	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{عدم, عدم}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    אם (hLibintlDLL != عدم)
	החזר 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vimLoadLib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    אם (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    אם (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT2);
# endif
    אם (!hLibintlDLL)
    {
	אם (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_load_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	החזר 0;
    }
    עבור (i = 0; libintl_entry[i].name != عدم
					 && libintl_entry[i].ptr != عدم; ++i)
    {
	אם ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == عدم)
	{
	    dyn_libintl_end();
	    אם (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_load_library_function_str), libintl_entry[i].name);
		verbose_leave();
	    }
	    החזר 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (حرف *(*)(ثابت حرف *, ثابت حرف *))
			GetProcAddress(hLibintlDLL, "bind_textdomain_codeset");
    אם (dyn_libintl_bind_textdomain_codeset == عدم)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = find_imported_module_by_funcname(hLibintlDLL, "getenv");
    אם (hmsvcrt != عدم)
	dyn_libintl_wputenv = (صحيح (*)(ثابت wchar_t *))
					GetProcAddress(hmsvcrt, "_wputenv");
    אם (dyn_libintl_wputenv == عدم || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_msg_cat_cntr = (صحيح *)GetProcAddress(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    החזר 1;
}

    فراغ
dyn_libintl_end(فراغ)
{
    אם (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= عدم;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_msg_cat_cntr	= عدم;
}

    ساكن حرف *
null_libintl_gettext(ثابت حرف *msgid)
{
    החזר (حرف*)msgid;
}

    ساكن حرف *
null_libintl_ngettext(
	ثابت حرف *msgid,
	ثابت حرف *msgid_plural,
	unsigned long n)
{
    החזר (حرف *)(n == 1 ? msgid : msgid_plural);
}

    ساكن حرف *
null_libintl_bindtextdomain(
	ثابت حرف *domainname UNUSED,
	ثابت حرف *dirname UNUSED)
{
    החזר عدم;
}

    ساكن حرف *
null_libintl_bind_textdomain_codeset(
	ثابت حرف *domainname UNUSED,
	ثابت حرف *codeset UNUSED)
{
    החזר عدم;
}

    ساكن حرف *
null_libintl_textdomain(ثابت حرف *domainname UNUSED)
{
    החזר عدم;
}

    ساكن صحيح
null_libintl_wputenv(ثابت wchar_t *envstring UNUSED)
{
    החזר 0;
}

#endif // DYNAMIC_GETTEXT

// This symbol is not defined in older versions of the SDK or Visual C++

#ifndef VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#endif

#ifdef HAVE_ACL
# include <aclapi.h>
# ifndef PROTECTED_DACL_SECURITY_INFORMATION
#  define PROTECTED_DACL_SECURITY_INFORMATION	0x80000000L
# endif
#endif

#ifdef HAVE_ACL
/*
 * Enables or disables the specified privilege.
 */
    ساكن BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    אם (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	החזר FALSE;

    אם (!LookupPrivilegeValue(عدم, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	החזר FALSE;
    }

    tokenPrivileges.PrivilegeCount	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    אם (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, عدم, عدم))
    {
	CloseHandle(hToken);
	החזר FALSE;
    }

    אם (GetLastError() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	החזר FALSE;
    }

    CloseHandle(hToken);
    החזר TRUE;
}
#endif

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    فراغ
PlatformId(فراغ)
{
#ifdef FEAT_EVAL
    DWORD major = (win_version >> 24) & 0xFF;
    DWORD minor = (win_version >> 16) & 0xFF;
    vim_snprintf(windowsVersion, حجم(windowsVersion), "%d.%d", major, minor);
#endif

#ifdef HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    אם (!win32_enable_privilege(SE_SECURITY_NAME))
	החזר;
#endif
}

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SHIFT  (SHIFT_PRESSED)
# define CTRL   (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)
# define ALT    (RIGHT_ALT_PRESSED  | LEFT_ALT_PRESSED)
# define ALT_GR (RIGHT_ALT_PRESSED  | LEFT_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

ساكن ثابت بنية
{
    WORD    wVirtKey;
    BOOL    fAnsiKey;
    صحيح	    chAlone;
    صحيح	    chShift;
    صحيح	    chCtrl;
    صحيح	    chAlt;
} VirtKeyMap[] =
{
//    Key	ANSI	alone	shift	ctrl	    alt
    { VK_ESCAPE,FALSE,	ESC,	ESC,	ESC,	    ESC,    },

    { VK_F1,	TRUE,	';',	'T',	'^',	    'h', },
    { VK_F2,	TRUE,	'<',	'U',	'_',	    'i', },
    { VK_F3,	TRUE,	'=',	'V',	'`',	    'j', },
    { VK_F4,	TRUE,	'>',	'W',	'a',	    'k', },
    { VK_F5,	TRUE,	'?',	'X',	'b',	    'l', },
    { VK_F6,	TRUE,	'@',	'Y',	'c',	    'm', },
    { VK_F7,	TRUE,	'A',	'Z',	'd',	    'n', },
    { VK_F8,	TRUE,	'B',	'[',	'e',	    'o', },
    { VK_F9,	TRUE,	'C',	'\\',	'f',	    'p', },
    { VK_F10,	TRUE,	'D',	']',	'g',	    'q', },
    { VK_F11,	TRUE,	'\205',	'\207',	'\211',	    '\213', },
    { VK_F12,	TRUE,	'\206',	'\210',	'\212',	    '\214', },

    { VK_HOME,	TRUE,	'G',	'\302',	'w',	    '\303', },
    { VK_UP,	TRUE,	'H',	'\304',	'\305',	    '\306', },
    { VK_PRIOR,	TRUE,	'I',	'\307',	'\204',	    '\310', }, // PgUp
    { VK_LEFT,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_RIGHT,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_END,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_DOWN,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_NEXT,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_INSERT,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_BACK,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# אם 0
    // Most people don't have F13-F20, but what the hell...
    { VK_F13,	TRUE,	'\332',	'\333',	'\334',	    '\335', },
    { VK_F14,	TRUE,	'\336',	'\337',	'\340',	    '\341', },
    { VK_F15,	TRUE,	'\342',	'\343',	'\344',	    '\345', },
    { VK_F16,	TRUE,	'\346',	'\347',	'\350',	    '\351', },
    { VK_F17,	TRUE,	'\352',	'\353',	'\354',	    '\355', },
    { VK_F18,	TRUE,	'\356',	'\357',	'\360',	    '\361', },
    { VK_F19,	TRUE,	'\362',	'\363',	'\364',	    '\365', },
    { VK_F20,	TRUE,	'\366',	'\367',	'\370',	    '\371', },
# endif
    { VK_ADD,	TRUE,   'N',    'N',    'N',	'N',	}, // keyp '+'
    { VK_SUBTRACT, TRUE,'J',	'J',    'J',	'J',	}, // keyp '-'
 // { VK_DIVIDE,   TRUE,'N',	'N',    'N',	'N',	}, // keyp '/'
    { VK_MULTIPLY, TRUE,'7',	'7',    '7',	'7',	}, // keyp '*'

    { VK_NUMPAD0,TRUE,  '\332',	'\333',	'\334',	    '\335', },
    { VK_NUMPAD1,TRUE,  '\336',	'\337',	'\340',	    '\341', },
    { VK_NUMPAD2,TRUE,  '\342',	'\343',	'\344',	    '\345', },
    { VK_NUMPAD3,TRUE,  '\346',	'\347',	'\350',	    '\351', },
    { VK_NUMPAD4,TRUE,  '\352',	'\353',	'\354',	    '\355', },
    { VK_NUMPAD5,TRUE,  '\356',	'\357',	'\360',	    '\361', },
    { VK_NUMPAD6,TRUE,  '\362',	'\363',	'\364',	    '\365', },
    { VK_NUMPAD7,TRUE,  '\366',	'\367',	'\370',	    '\371', },
    { VK_NUMPAD8,TRUE,  '\372',	'\373',	'\374',	    '\375', },
    // Sorry, out of number space! <negri>
    { VK_NUMPAD9,TRUE,  '\376',	'\377',	'|',	    '}', },
};


/*
 * The return code indicates key code size.
 */
    ساكن صحيح
win32_kbd_patch_key(
    KEY_EVENT_RECORD *pker)
{
    UINT uMods = pker->dwControlKeyState;
    ساكن صحيح s_iIsDead = 0;
    ساكن WORD awAnsiCode[2];
    ساكن BYTE abKeystate[256];

    אם (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[1];
	s_iIsDead = 0;
	החזר 1;
    }

    // check if it already has a valid unicode character.
    אם (pker->uChar.UnicodeChar != 0)
	החזר 1;

    CLEAR_FIELD(abKeystate);

    // Clear any pending dead keys
    ToUnicode(VK_SPACE, MapVirtualKey(VK_SPACE, 0), abKeystate, awAnsiCode, 2, 0);

    אם (uMods & SHIFT_PRESSED)
	abKeystate[VK_SHIFT] = 0x80;
    אם (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    אם ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ToUnicode(pker->wVirtualKeyCode, pker->wVirtualScanCode,
			abKeystate, awAnsiCode, 2, 0);

    אם (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[0];

    החזר s_iIsDead;
}

ساكن BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    ساكن BOOL
decode_key_event(
    KEY_EVENT_RECORD	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    صحيح			*pmodifiers,
    BOOL		fDoPost UNUSED)
{
    صحيح i;
    ثابت صحيح nModifs = pker->dwControlKeyState & (SHIFT | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    אם (!pker->bKeyDown)
	החזר FALSE;

    // ignore some keystrokes
    بدل (pker->wVirtualKeyCode)
    {
    // modifiers
    حالة VK_SHIFT:
    حالة VK_CONTROL:
    حالة VK_MENU:   // Alt key
	החזר FALSE;

    افتراضي:
	اكسر;
    }

    // Shift-TAB
    אם (pker->wVirtualKeyCode == VK_TAB && (nModifs & SHIFT_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	החזר TRUE;
    }

    עבור (i = ARRAY_LENGTH(VirtKeyMap); --i >= 0; )
    {
	אם (VirtKeyMap[i].wVirtKey == pker->wVirtualKeyCode)
	{
	    *pch = VirtKeyMap[i].chAlone;
	    אם ((nModifs & SHIFT) != 0)
		*pch = VirtKeyMap[i].chShift;
	    אחרת אם ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = VirtKeyMap[i].chCtrl;
	    אחרת אם ((nModifs & ALT) != 0)
		*pch = VirtKeyMap[i].chAlt;

	    אם (*pch != 0)
	    {
		אם (VirtKeyMap[i].fAnsiKey)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    אם (pmodifiers)
		    {
			אם (pker->wVirtualKeyCode >= VK_F1
			    && pker->wVirtualKeyCode <= VK_F12)
			{
			    אם ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_MASK_ALT;
				אם ((nModifs & SHIFT) == 0)
				    *pch2 = VirtKeyMap[i].chAlone;
			    }
			    אם ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				אם ((nModifs & SHIFT) == 0)
				    *pch2 = VirtKeyMap[i].chAlone;
			    }
			}
			אחרת אם (pker->wVirtualKeyCode >= VK_END
				&& pker->wVirtualKeyCode <= VK_DOWN)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = VirtKeyMap[i].chAlone;
			    אם ((nModifs & SHIFT) != 0
						    && (nModifs & ~SHIFT) == 0)
			    {
				*pch2 = VirtKeyMap[i].chShift;
			    }
			    אם ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = VirtKeyMap[i].chCtrl;
				אם (pker->wVirtualKeyCode == VK_UP
				    || pker->wVirtualKeyCode == VK_DOWN)
				{
				    *pmodifiers |= MOD_MASK_CTRL;
				    *pch2 = VirtKeyMap[i].chAlone;
				}
			    }
			    אם ((nModifs & SHIFT) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				*pch2 = VirtKeyMap[i].chShift;
			    }
			    אם ((nModifs & ALT) != 0)
			    {
				*pch2 = VirtKeyMap[i].chAlt;
				*pmodifiers |= MOD_MASK_ALT;
				אם ((nModifs & ~ALT) == 0)
				{
				    *pch2 = VirtKeyMap[i].chAlone;
				}
				אחרת אם ((nModifs & SHIFT) != 0)
				{
				    *pch2 = VirtKeyMap[i].chShift;
				}
				אחרת אם ((nModifs & CTRL) != 0)
				{
				    אם (pker->wVirtualKeyCode == VK_UP
					|| pker->wVirtualKeyCode == VK_DOWN)
				    {
					*pmodifiers |= MOD_MASK_CTRL;
					*pch2 = VirtKeyMap[i].chAlone;
				    }
				    אחרת
				    {
					*pch2 = VirtKeyMap[i].chCtrl;
				    }
				}
			    }
			}
			אחרת אם (pker->wVirtualKeyCode == VK_INSERT
					&& (nModifs & SHIFT) != 0
					&& (nModifs & ~SHIFT) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = VirtKeyMap[i].chShift;
			}
			אחרת
			{
			    *pch2 = VirtKeyMap[i].chAlone;
			    אם ((nModifs & SHIFT) != 0)
				*pmodifiers |= MOD_MASK_SHIFT;
			    אם ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_MASK_CTRL;
			    אם ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_MASK_ALT;
			}
		    }
		}

		החזר TRUE;
	    }
	}
    }

    i = win32_kbd_patch_key(pker);

    אם (i < 0)
	*pch = NUL;
    אחרת
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	אם (pmodifiers != عدم)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    אם ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_MASK_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    אם ((nModifs & SHIFT) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_MASK_SHIFT;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    אם ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_MASK_CTRL;
	}
    }

    החזר (*pch != NUL);
}

# אם defined(FEAT_EVAL)
    ساكن صحيح
encode_key_event(dict_T *args, INPUT_RECORD *ir)
{
    ساكن صحيح s_dwMods = 0;

    char_u *action = dict_get_string(args, "event", TRUE);
    אם (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL isKeyDown = STRICMP(action, "keydown") == 0;
	WORD vkCode = dict_get_number_def(args, "keycode", 0);
	אם (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    החזר FALSE;
	}

	ir->EventType = KEY_EVENT;
	KEY_EVENT_RECORD ker;
	ZeroMemory(&ker, حجم(ker));
	ker.bKeyDown = isKeyDown;
	ker.wRepeatCount = 1;
	ker.wVirtualScanCode = 0;
	ker.dwControlKeyState = 0;
	صحيح mods = (صحيح)dict_get_number(args, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	אם (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    אם (mods & MOD_MASK_SHIFT)
		ker.dwControlKeyState |= SHIFT_PRESSED;
	    אם (mods & MOD_MASK_CTRL)
		ker.dwControlKeyState |= LEFT_CTRL_PRESSED;
	    אם (mods & MOD_MASK_ALT)
		ker.dwControlKeyState |= LEFT_ALT_PRESSED;
	}

	אם (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT)
	{
	    אם (isKeyDown)
		s_dwMods |= SHIFT_PRESSED;
	    אחרת
		s_dwMods &= ~SHIFT_PRESSED;
	}
	אחרת אם (vkCode == VK_LCONTROL || vkCode == VK_CONTROL)
	{
	    אם (isKeyDown)
		s_dwMods |= LEFT_CTRL_PRESSED;
	    אחרת
		s_dwMods &= ~LEFT_CTRL_PRESSED;
	}
	אחרת אם (vkCode == VK_RCONTROL)
	{
	    אם (isKeyDown)
		s_dwMods |= RIGHT_CTRL_PRESSED;
	    אחרת
		s_dwMods &= ~RIGHT_CTRL_PRESSED;
	}
	אחרת אם (vkCode == VK_LMENU || vkCode == VK_MENU)
	{
	    אם (isKeyDown)
		s_dwMods |= LEFT_ALT_PRESSED;
	    אחרת
		s_dwMods &= ~LEFT_ALT_PRESSED;
	}
	אחרת אם (vkCode == VK_RMENU)
	{
	    אם (isKeyDown)
		s_dwMods |= RIGHT_ALT_PRESSED;
	    אחרת
		s_dwMods &= ~RIGHT_ALT_PRESSED;
	}
	ker.dwControlKeyState |= s_dwMods;
	ker.wVirtualKeyCode = vkCode;
	ker.uChar.UnicodeChar = 0;
	ir->Event.KeyEvent = ker;
	vim_free(action);
    }
    אחרת
    {
	אם (action == عدم)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	אחרת
	{
	    semsg(_(e_invalid_value_for_argument_str_str), "event", action);
	    vim_free(action);
	}
	החזר FALSE;
    }
    החזר TRUE;
}
# endif  // FEAT_EVAL
#endif // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساكن صحيح g_fMouseAvail = FALSE;   // mouse present
ساكن صحيح g_fMouseActive = FALSE;  // mouse enabled
ساكن صحيح g_nMouseClick = -1;	    // mouse status
ساكن صحيح g_xMouse;		    // mouse x coordinate
ساكن صحيح g_yMouse;		    // mouse y coordinate
ساكن DWORD g_cmodein = 0;	    // Original console input mode
ساكن DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    فراغ
mch_setmouse(صحيح on)
{
    DWORD cmodein;

# ifdef VIMDLL
    אם (gui.in_use)
	החזר;
# endif
    אם (!g_fMouseAvail)
	החזר;

    g_fMouseActive = on;
    GetConsoleMode(g_hConIn, &cmodein);

    אם (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_INPUT;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    אחרת
    {
	cmodein &= ~ENABLE_MOUSE_INPUT;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }

    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
}


# אם defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    فراغ
mch_bevalterm_changed(فراغ)
{
    mch_setmouse(g_fMouseActive);
}
# endif

/*
 * Win32 console mouse scroll event handler.
 * Console version of the _OnMouseWheel() function in gui_w32.c
 *
 * This encodes the mouse scroll direction and keyboard modifiers into
 * g_nMouseClick, and the mouse position into g_xMouse and g_yMouse
 *
 * The direction of the scroll is decoded from two fields of the win32 console
 * mouse event record;
 *    1. The orientation - vertical or horizontal flag - from dwEventFlags
 *    2. The sign - positive or negative (aka delta flag) - from dwButtonState
 *
 * When scroll orientation is HORIZONTAL
 *    -  If the high word of the dwButtonState member contains a positive
 *	 value, the wheel was rotated to the right.
 *    -  Otherwise, the wheel was rotated to the left.
 * When scroll orientation is VERTICAL
 *    -  If the high word of the dwButtonState member contains a positive value,
 *       the wheel was rotated forward, away from the user.
 *    -  Otherwise, the wheel was rotated backward, toward the user.
 */
    ساكن فراغ
decode_mouse_wheel(MOUSE_EVENT_RECORD *pmer)
{
    صحيح	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    صحيح	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    صحيح lcol = g_xMouse;
    صحيح lrow = g_yMouse;
    win_T *wp = mouse_find_win(&lrow, &lcol, FIND_POPUP);
    אם (wp != عدم && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_FIELD(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	אם (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_LEFT : MSCR_RIGHT;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	אחרת
	{
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	    cap.arg = zDelta < 0 ? MSCR_UP : MSCR_DOWN;
	}

	// Mouse hovers over popup window, scroll it if possible.
	mouse_row = wp->w_winrow;
	mouse_col = wp->w_wincol;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	out_flush();
    }
# endif
    mouse_col = g_xMouse;
    mouse_row = g_yMouse;

    char_u modifiers = 0;
    char_u direction = 0;

    // Decode the direction into an event that Vim can process
    אם (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    אחרת
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    אם (pmer->dwControlKeyState & SHIFT_PRESSED)
	modifiers |= MOD_MASK_SHIFT; // MOUSE_SHIFT;
    אם (pmer->dwControlKeyState & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED))
	modifiers |= MOD_MASK_CTRL; // MOUSE_CTRL;
    אם (pmer->dwControlKeyState & (RIGHT_ALT_PRESSED  | LEFT_ALT_PRESSED))
	modifiers |= MOD_MASK_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    החזר;
}

/*
 * Decode a MOUSE_EVENT.  If it's a valid event, return MOUSE_LEFT,
 * MOUSE_MIDDLE, or MOUSE_RIGHT for a click; MOUSE_DRAG for a mouse
 * move with a button held down; and MOUSE_RELEASE after a MOUSE_DRAG
 * or a MOUSE_LEFT, _MIDDLE, or _RIGHT.  We encode the button type,
 * the number of clicks, and the Shift/Ctrl/Alt modifiers in g_nMouseClick,
 * and we return the mouse position in g_xMouse and g_yMouse.
 *
 * Every MOUSE_LEFT, _MIDDLE, or _RIGHT will be followed by zero or more
 * MOUSE_DRAGs and one MOUSE_RELEASE.  MOUSE_RELEASE will be followed only
 * by MOUSE_LEFT, _MIDDLE, or _RIGHT.
 *
 * For multiple clicks, we send, say, MOUSE_LEFT/1 click, MOUSE_RELEASE,
 * MOUSE_LEFT/2 clicks, MOUSE_RELEASE, MOUSE_LEFT/3 clicks, MOUSE_RELEASE, ....
 *
 * Windows will send us MOUSE_MOVED notifications whenever the mouse
 * moves, even if it stays within the same character cell.  We ignore
 * all MOUSE_MOVED messages if the position hasn't really changed, and
 * we ignore all MOUSE_MOVED messages where no button is held down (i.e.,
 * we're only interested in MOUSE_DRAG).
 *
 * All of this is complicated by the code that fakes MOUSE_MIDDLE on
 * 2-button mouses by pressing the left & right buttons simultaneously.
 * In practice, it's almost impossible to click both at the same time,
 * so we need to delay a little.  Also, we tend not to get MOUSE_RELEASE
 * in such cases, if the user is clicking quickly.
 */
    ساكن BOOL
decode_mouse_event(
    MOUSE_EVENT_RECORD *pmer)
{
    ساكن صحيح s_nOldButton = -1;
    ساكن صحيح s_nOldMouseClick = -1;
    ساكن صحيح s_xOldMouse = -1;
    ساكن صحيح s_yOldMouse = -1;
    ساكن linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    ساكن صحيح s_old_topfill = 0;
# endif
    ساكن صحيح s_cClicks = 1;
    ساكن BOOL s_fReleased = TRUE;
    ساكن DWORD s_dwLastClickTime = 0;
    ساكن BOOL s_fNextIsMiddle = FALSE;

    ساكن DWORD cButtons = 0;	// number of buttons supported

    ثابت DWORD LEFT = FROM_LEFT_1ST_BUTTON_PRESSED;
    ثابت DWORD MIDDLE = FROM_LEFT_2ND_BUTTON_PRESSED;
    ثابت DWORD RIGHT = RIGHTMOST_BUTTON_PRESSED;
    ثابت DWORD LEFT_RIGHT = LEFT | RIGHT;

    صحيح nButton;

    אם (cButtons == 0 && !GetNumberOfConsoleMouseButtons(&cButtons))
	cButtons = 2;

    אם (!g_fMouseAvail || !g_fMouseActive)
    {
	g_nMouseClick = -1;
	החזר FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    אם (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	החזר FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    אם (g_nMouseClick != -1)
	החזר TRUE;

    אם (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	החזר TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    אם (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	אם (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    החזר FALSE;
    }

    // If no buttons are pressed...
    אם ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	אם (s_fReleased)
	{
# ifdef FEAT_BEVAL_TERM
	    // do return mouse move events when we want them
	    אם (p_bevalterm)
		nButton = MOUSE_DRAG;
	    אחרת
# endif
		החזר FALSE;
	}

	s_fReleased = TRUE;
    }
    אחרת    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	אם (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & LEFT_RIGHT);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    אם (dwLR == LEFT || dwLR == RIGHT)
	    {
		עבור (;;)
		{
		    // wait a short time for next input event
		    אם (WaitForSingleObject(g_hConIn, p_mouset / 3)
							     != WAIT_OBJECT_0)
			اكسر;
		    אחרת
		    {
			DWORD cRecords = 0;
			INPUT_RECORD ir;
			MOUSE_EVENT_RECORD* pmer2 = &ir.Event.MouseEvent;

			peek_console_input(g_hConIn, &ir, 1, &cRecords);

			אם (cRecords == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & LEFT_RIGHT))
			    اكسر;
			אחרת
			{
			    אם (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_input(g_hConIn, &ir, 1, &cRecords);

				החזר decode_mouse_event(pmer2);
			    }
			    אחרת אם (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_input(g_hConIn, &ir, 1, &cRecords);

				// are there any more mouse events in queue?
				peek_console_input(g_hConIn, &ir, 1, &cRecords);

				אם (cRecords==0 || ir.EventType != MOUSE_EVENT)
				    اكسر;
			    }
			    אחרת
				اكسر;
			}
		    }
		}
	    }
	}

	אם (s_fNextIsMiddle)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_MIDDLE;
	    s_fNextIsMiddle = FALSE;
	}
	אחרת אם (cButtons == 2	&&
	    ((pmer->dwButtonState & LEFT_RIGHT) == LEFT_RIGHT))
	{
	    nButton = MOUSE_MIDDLE;

	    אם (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_fNextIsMiddle = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	אחרת אם ((pmer->dwButtonState & LEFT) == LEFT)
	    nButton = MOUSE_LEFT;
	אחרת אם ((pmer->dwButtonState & MIDDLE) == MIDDLE)
	    nButton = MOUSE_MIDDLE;
	אחרת אם ((pmer->dwButtonState & RIGHT) == RIGHT)
	    nButton = MOUSE_RIGHT;

	אם (! s_fReleased && ! s_fNextIsMiddle
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    החזר FALSE;

	s_fReleased = s_fNextIsMiddle;
    }

    אם (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	אם (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dwCurrentTime = GetTickCount();

	    אם (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (صحيح)(dwCurrentTime - s_dwLastClickTime) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    אחרת אם (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dwLastClickTime = dwCurrentTime;
	}
    }
    אחרת אם (pmer->dwEventFlags == MOUSE_MOVED)
    {
	אם (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    אם (nButton == -1)
	החזר FALSE;

    אם (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    אם (pmer->dwControlKeyState & SHIFT_PRESSED)
	g_nMouseClick |= MOUSE_SHIFT;
    אם (pmer->dwControlKeyState & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    אם (pmer->dwControlKeyState & (RIGHT_ALT_PRESSED  | LEFT_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    אם (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	SET_NUM_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    אם (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	החזר FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    החזר TRUE;
}

# ifdef FEAT_EVAL
    ساكن صحيح
encode_mouse_event(dict_T *args, INPUT_RECORD *ir)
{
    صحيح		button;
    صحيح		row;
    صحيح		col;
    صحيح		repeated_click;
    int_u	mods = 0;
    صحيح		move;

    אם (!dict_has_key(args, "row") || !dict_has_key(args, "col"))
	החזר FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (صحيح)dict_get_bool(args, "move", FALSE);
    אם (!move && (!dict_has_key(args, "button")
	    || !dict_has_key(args, "multiclick")
	    || !dict_has_key(args, "modifiers")))
	החזר FALSE;

    row = (صحيح)dict_get_number(args, "row") - 1;
    col = (صحيح)dict_get_number(args, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_RECORD mer;
    ZeroMemory(&mer, حجم(mer));
    mer.dwMousePosition.X  = col;
    mer.dwMousePosition.Y  = row;

    אם (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    אחרת
    {
	button = (صحيح)dict_get_number(args, "button");
	repeated_click = (صحيح)dict_get_number(args, "multiclick");
	mods = (صحيح)dict_get_number(args, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	بدل (button)
	{
	    حالة MOUSE_LEFT:
		mer.dwButtonState = FROM_LEFT_1ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		اكسر;
	    حالة MOUSE_MIDDLE:
		mer.dwButtonState = FROM_LEFT_2ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		اكسر;
	    حالة MOUSE_RIGHT:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		اكسر;
	    حالة MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    حالة MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		اكسر;
	    حالة MOUSE_X1:
		mer.dwButtonState = FROM_LEFT_3RD_BUTTON_PRESSED;
		اكسر;
	    حالة MOUSE_X2:
		mer.dwButtonState = FROM_LEFT_4TH_BUTTON_PRESSED;
		اكسر;
	    حالة MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		اكسر;
	    حالة MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		اكسر;
	    حالة MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		اكسر;
	    حالة MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		اكسر;
	    افتراضي:
		semsg(_(e_invalid_argument_str), "button");
		החזר FALSE;
	}
    }

    mer.dwControlKeyState = 0;
    אם (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	אם (mods & MOUSE_SHIFT)
	    mer.dwControlKeyState |= SHIFT_PRESSED;
	אם (mods & MOUSE_CTRL)
	    mer.dwControlKeyState |= LEFT_CTRL_PRESSED;
	אם (mods & MOUSE_ALT)
	    mer.dwControlKeyState |= LEFT_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    החזר TRUE;
}
# endif  // FEAT_EVAL

    ساكن صحيح
write_input_record_buffer(INPUT_RECORD* irEvents, صحيح nLength)
{
    صحيح				nCount = 0;
    input_record_buffer_node_T	*event_node;

    כלעוד (nCount < nLength)
    {
	event_node = alloc(حجم(input_record_buffer_node_T));
	אם (event_node == عدم)
	    החזר -1;

	event_node->ir = irEvents[nCount++];
	event_node->next = عدم;

	input_record_buffer.length++;
	אם (input_record_buffer.tail == عدم)
	{
	    input_record_buffer.head = event_node;
	    input_record_buffer.tail = event_node;
	}
	אחרת
	{
	    input_record_buffer.tail->next = event_node;
	    input_record_buffer.tail = input_record_buffer.tail->next;
	}
    }
    החזר nCount;
}

    ساكن صحيح
read_input_record_buffer(INPUT_RECORD* irEvents, صحيح nMaxLength)
{
    صحيح nCount = 0;
    כלעוד (nCount < nMaxLength && input_record_buffer.head != عدم)
    {
	input_record_buffer.length--;
	input_record_buffer_node_T *pop_head = input_record_buffer.head;
	irEvents[nCount++] = pop_head->ir;
	input_record_buffer.head = pop_head->next;
	vim_free(pop_head);
	אם (input_record_buffer.length == 0)
	    input_record_buffer.tail = عدم;
    }
    החזר nCount;
}
#אחרת  // FEAT_GUI_MSWIN && !VIMDLL
    فراغ
mch_setmouse(صحيح on UNUSED)
{
}
#endif // FEAT_GUI_MSWIN && !VIMDLL

#ifdef FEAT_EVAL
/*
 * The 'test_mswin_event' function is for testing Vim's low-level handling of
 * user input events.  ie, this manages the encoding of INPUT_RECORD events
 * so that we have a way to test how Vim decodes INPUT_RECORD events in Windows
 * consoles.
 *
 * The 'test_mswin_event' function is based on 'test_gui_event'.  In fact, when
 * the Windows GUI is running, the arguments; 'event' and 'args', are the same.
 * So, it acts as an alias for 'test_gui_event' for the Windows GUI.
 *
 * When the Windows console is running, the arguments; 'event' and 'args', are
 * a subset of what 'test_gui_event' handles, ie, only "key" and "mouse"
 * events are encoded as INPUT_RECORD events.
 *
 * Note: INPUT_RECORDs are only used by the Windows console, not the GUI.  The
 * GUI sends MSG structs instead.
 */
    صحيح
test_mswin_event(char_u *event, dict_T *args)
{
    صحيح lpEventsWritten = 0;

# אם defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    אם (gui.in_use)
	החזר test_gui_w32_sendevent(event, args);
# endif

# אם defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    INPUT_RECORD ir;
    BOOL input_encoded = FALSE;
    BOOL execute = FALSE;
    אם (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(args, "execute", FALSE);
	אם (dict_has_key(args, "event"))
	    input_encoded = encode_key_event(args, &ir);
	אחרת אם (!execute)
	{
	    semsg(_(e_missing_argument_str), "event");
	    החזר FALSE;
	}
    }
    אחרת אם (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	input_encoded = encode_mouse_event(args, &ir);
    }
    אחרת
    {
	semsg(_(e_invalid_value_for_argument_str_str), "event", event);
	החזר FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    אם (input_encoded)
    {
	אם ((lpEventsWritten = write_input_record_buffer(&ir, 1)) < 0)
	{
	    semsg(_(e_out_of_memory), "event");
	    החזר FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    אם (execute)
    {
	صحيح save_msg_scroll = msg_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	msg_scroll = FALSE;
	ch_log(عدم, "test_mswin_event() executing");
	exec_normal(TRUE, TRUE, TRUE);
	msg_scroll |= save_msg_scroll;
    }

# endif
    החזר lpEventsWritten;
}
#endif // FEAT_EVAL

#אם defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    ساكن فراغ
mch_set_cursor_shape(صحيح thickness)
{
    אם (vtp_working)
    {
	אם (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    אם (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    אחרת
		vtp_printf("\033[0 q");	// default
	}
    }
    אחרת
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dwSize = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	SetConsoleCursorInfo(g_hConOut, &ConsoleCursorInfo);
	אם (s_cursor_visible)
	    SetConsoleCursorPosition(g_hConOut, g_coord);
    }
}

    فراغ
mch_update_cursor(فراغ)
{
    صحيح		idx;
    صحيح		thickness;

# ifdef VIMDLL
    אם (gui.in_use)
	החזר;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    אם (shape_table[idx].shape == SHAPE_BLOCK)
	thickness = 99;	// 100 doesn't work on W95
    אחרת
	thickness = shape_table[idx].percentage;
    mch_set_cursor_shape(thickness);
}
#endif

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    ساكن فراغ
handle_focus_event(INPUT_RECORD ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
    ui_focus_change((صحيح)g_fJustGotFocus);
}

ساكن فراغ ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    ساكن صحيح
WaitForChar(long msec, صحيح ignore_input)
{
    DWORD	    dwNow = 0, dwEndTime = 0;
    INPUT_RECORD    ir;
    DWORD	    cRecords;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    صحيح		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    אם (msec > 0)
	// Wait until the specified time has elapsed.
	dwEndTime = GetTickCount() + msec;
    אחרת אם (msec < 0)
	// Wait forever.
	dwEndTime = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    עבור (;;)
    {
	// Only process messages when waiting.
	אם (msec != 0)
	{
# ifdef MESSAGE_QUEUE
	    parse_queued_messages();
# endif
# ifdef FEAT_MZSCHEME
	    mzvim_check_threads();
# endif
# ifdef FEAT_CLIENTSERVER
	    serverProcessPendingMessages();
# endif
	}

	אם (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_input && input_available())
# endif
	   )
	    החזר TRUE;

	אם (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = GetTickCount();
	    אם ((صحيح)(dwNow - dwEndTime) >= 0)
		اكسر;
	}
	אם (msec != 0)
	{
	    DWORD dwWaitTime = dwEndTime - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    אם (dwWaitTime > 11)
		dwWaitTime = 11;

# ifdef FEAT_MZSCHEME
	    אם (mzthreads_allowed() && p_mzq > 0 && (long)dwWaitTime > p_mzq)
		dwWaitTime = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    אם (dwWaitTime > 10)
	    {
		long	due_time;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_time = check_due_timer();
		אם (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    החזר FALSE;
		}
		אם (due_time > 0 && dwWaitTime > (DWORD)due_time)
		    dwWaitTime = due_time;
	    }
# endif
	    אם (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    msg_wait_for_multiple_objects(1, &g_hConIn, FALSE,
				  dwWaitTime, QS_SENDMESSAGE) != WAIT_OBJECT_0
# אחרת
		    wait_for_single_object(g_hConIn, dwWaitTime)
							      != WAIT_OBJECT_0
# endif
		    )
		استمر;
	}

	cRecords = 0;
	peek_console_input(g_hConIn, &ir, 1, &cRecords);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	אם (State == MODE_CMDLINE && msg_row == Rows - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_BUFFER_INFO csbi;

	    אם (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	    {
		אם (csbi.dwCursorPosition.Y != msg_row)
		{
		    // The screen is now messed up, must redraw the command
		    // line and later all the windows.
		    redraw_all_later(UPD_CLEAR);
		    compute_cmdrow();
		    redrawcmd();
		}
	    }
	}
# endif

	אם (cRecords > 0)
	{
	    אם (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		אם (ir.Event.KeyEvent.uChar.UnicodeChar == 0
			&& ir.Event.KeyEvent.wVirtualKeyCode == 13)
		{
		    read_console_input(g_hConIn, &ir, 1, &cRecords);
		    استمر;
		}
# endif
		אם (decode_key_event(&ir.Event.KeyEvent, &ch, &ch2,
								 عدم, FALSE))
		    החזר TRUE;
	    }

	    read_console_input(g_hConIn, &ir, 1, &cRecords);

	    אם (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    אחרת אם (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    {
		COORD dwSize = ir.Event.WindowBufferSizeEvent.dwSize;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		אם (dwSize.X != Columns || dwSize.Y != Rows)
		{
		    CONSOLE_SCREEN_BUFFER_INFO csbi;
		    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
		    dwSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		    dwSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		    אם (dwSize.X != Columns || dwSize.Y != Rows)
		    {
			ResizeConBuf(g_hConOut, dwSize);
			shell_resized();
		    }
		}
	    }
	    אחרת אם (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		החזר TRUE;
	}
	אחרת אם (msec == 0)
	    اكسر;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    אם (input_available())
	החזר TRUE;
# endif

    החזר FALSE;
}

/*
 * return non-zero if a character is available
 */
    صحيح
mch_char_avail(فراغ)
{
# ifdef VIMDLL
    אם (gui.in_use)
	החזר TRUE;
# endif
    החזר WaitForChar(0L, FALSE);
}

# אם defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    صحيح
mch_check_messages(فراغ)
{
#  ifdef VIMDLL
    אם (gui.in_use)
	החזר TRUE;
#  endif
    החזר WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    ساكن فراغ
create_conin(فراغ)
{
    g_hConIn =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES) عدم,
			OPEN_EXISTING, 0, (HANDLE)عدم);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    ساكن WCHAR
tgetch(صحيح *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    עבור (;;)
    {
	INPUT_RECORD ir;
	DWORD cRecords = 0;

# ifdef FEAT_CLIENTSERVER
	(فراغ)WaitForChar(-1L, FALSE);
	אם (input_available())
	    החזר 0;
	אם (g_nMouseClick != -1)
	    החזר 0;
# endif
	אם (read_console_input(g_hConIn, &ir, 1, &cRecords) == 0)
	{
	    אם (did_create_conin)
		read_error_exit();
	    create_conin();
	    استمر;
	}

	אם (ir.EventType == KEY_EVENT)
	{
	    אם (decode_key_event(&ir.Event.KeyEvent, &ch, pch2,
							    pmodifiers, TRUE))
		החזר ch;
	}
	אחרת אם (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	אחרת אם (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    shell_resized();
	אחרת אם (ir.EventType == MOUSE_EVENT)
	{
	    אם (decode_mouse_event(&ir.Event.MouseEvent))
		החזר 0;
	}
    }
}
#endif // !FEAT_GUI_MSWIN


/*
 * mch_inchar(): low-level input function.
 * Get one or more characters from the keyboard or the mouse.
 * If time == 0, do not wait for characters.
 * If time == n, wait a short time for characters.
 * If time == -1, wait forever for characters.
 * Returns the number of characters read into buf.
 */
    صحيح
mch_inchar(
    char_u	*buf UNUSED,
    صحيح		maxlen UNUSED,
    long	time UNUSED,
    صحيح		tb_change_cnt UNUSED)
{
#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    صحيح		len;
    صحيح		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# אחרת
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    ساكن char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    ساكن صحيح	    typeaheadlen = 0;

# ifdef VIMDLL
    אם (gui.in_use)
	החזר 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    אם (typeaheadlen > 0)
	اذهب theend;

    אם (time >= 0)
    {
	אם (!WaitForChar(time, FALSE))     // no character available
	    החזר 0;
    }
    אחרת    // time == -1, wait forever
    {
	mch_set_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	אם (!WaitForChar(p_ut, FALSE))
	{
	    אם (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (صحيح)KE_CURSORHOLD;
		החזר 3;
	    }
	    before_blocking();
	}
    }

    /*
     * Try to read as many characters as there are, until the buffer is full.
     */

    // we will get at least one key. Get more if they are available.
    g_fCBrkPressed = FALSE;

# ifdef MCH_WRITE_DUMP
    אם (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    כלעוד ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	אם (typebuf_changed(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    اكسر;
	}
	אם (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    אם (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    אם (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		אם (modifiers > 0)
		{
		    // use K_SPECIAL instead of CSI to make mappings work
		    typeahead[typeaheadlen++] = K_SPECIAL;
		    typeahead[typeaheadlen++] = KS_MODIFIER;
		    typeahead[typeaheadlen++] = modifiers;
		}
		typeahead[typeaheadlen++] = CSI;
		typeahead[typeaheadlen++] = KS_EXTRA;
		typeahead[typeaheadlen++] = scroll_dir;
	    }
	    אחרת
	    {
		typeahead[typeaheadlen++] = ESC + 128;
		typeahead[typeaheadlen++] = 'M';
		typeahead[typeaheadlen++] = g_nMouseClick;
	    }

	    // Pass the pointer coordinates of the mouse event in 2 bytes,
	    // allowing for > 223 columns.  Both for click and scroll events.
	    // This is the same as what is used for the GUI.
	    typeahead[typeaheadlen++] = (char_u)(g_xMouse / 128 + ' ' + 1);
	    typeahead[typeaheadlen++] = (char_u)(g_xMouse % 128 + ' ' + 1);
	    typeahead[typeaheadlen++] = (char_u)(g_yMouse / 128 + ' ' + 1);
	    typeahead[typeaheadlen++] = (char_u)(g_yMouse % 128 + ' ' + 1);

	    g_nMouseClick = -1;
	}
	אחרת
	{
	    WCHAR	ch2 = NUL;
	    صحيح		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_key(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_keys;
	    c = may_adjust_key_for_ctrl(modifiers, c);
	    --no_reduce_keys;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_shift_modifier(modifiers, c);

	    אם (typebuf_changed(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		اكسر;
	    }

	    אם (c == Ctrl_C && ctrl_c_interrupts)
	    {
# אם defined(FEAT_CLIENTSERVER)
		trash_input_buf();
# endif
		got_int = TRUE;
	    }

	    אם (g_nMouseClick == -1)
	    {
		صحيح	n = 1;

		אם (ch2 == NUL)
		{
		    صحيح	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    אם (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_to_enc(ch, &n);
		    אם (p != عدم)
		    {
			עבור (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    אם (p[i] == CSI)
			    {
				typeahead[typeaheadlen + j++] = KS_EXTRA;
				typeahead[typeaheadlen + j++] = KE_CSI;
			    }
# endif
			}
			n = j;
			vim_free(p);
		    }
		}
		אחרת
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    אם (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		אם (ch2 != NUL)
		{
		    אם (c == K_NUL)
		    {
			بدل (ch2)
			{
			حالة (WCHAR)'\324': // SHIFT+Insert
			حالة (WCHAR)'\325': // CTRL+Insert
			حالة (WCHAR)'\327': // SHIFT+Delete
			حالة (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    اكسر;

			افتراضي:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    اكسر;
			}
		    }
		    אחרת
		    {
			typeahead[typeaheadlen + n] = 3;
			typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			n += 2;
		    }
		}

		// Use the ALT key to set the 8th bit of the character
		// when it's one byte, the 8th bit isn't set yet and not
		// using a double-byte encoding (would become a lead
		// byte).
		אם ((modifiers & MOD_MASK_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_MASK_ALT;
		}

		אם (modifiers != 0)
		{
		    // Prepend modifiers to the character.
		    mch_memmove(typeahead + typeaheadlen + 3,
						 typeahead + typeaheadlen, n);
		    typeahead[typeaheadlen++] = K_SPECIAL;
		    typeahead[typeaheadlen++] = (char_u)KS_MODIFIER;
		    typeahead[typeaheadlen++] =  modifiers;
		}

		typeaheadlen += n;

# ifdef MCH_WRITE_DUMP
		אם (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    אם (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    כלעוד (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    אם (len > 0)
    {
	buf[len] = NUL;
	ch_log(عدم, "raw key input: \"%s\"", buf);
    }
# endif
    החזר len;

#אחרת // FEAT_GUI_MSWIN && !VIMDLL
    החזר 0;
#endif // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    ساكن صحيح
executable_file(حرف *name, char_u **path)
{
    صحيح attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    אם (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	החזר FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    אם (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	אם (res == عدم)
	    res = resolve_reparse_point((char_u *)name);
	אם (res == عدم)
	    החזר FALSE;
	// The path is already absolute.
	אם (path != عدم)
	    *path = res;
	אחרת
	    vim_free(res);
    }
    אחרת אם (path != عدم)
	*path = FullName_save((char_u *)name, FALSE);
    החזר TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    ساكن bool
executable_prefix_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_FIND_DATAW	data;
    HANDLE		handle;
    DWORD		error;

    wn = enc_to_utf16(name, عدم);
    אם (wn == عدم)
	החזר true;
    handle = FindFirstFileExW(wn, FindExInfoBasic, &data,
					    FindExSearchNameMatch, عدم, 0);
    error = GetLastError();
    vim_free(wn);
    אם (handle != INVALID_HANDLE_VALUE)
    {
	FindClose(handle);
	החזר true;
    }
    החזר error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    ساكن صحيح
executable_exists(
    حرف	*name,
    size_t	namelen,
    char_u	**path,
    صحيح		use_path,
    صحيح		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3];
    char_u	*p;
    string_T	pathbuf = {عدم, 0};
    صحيح		pathbuf_allocated = FALSE;
    string_T	pathext = {عدم, 0};
    صحيح		pathext_allocated = FALSE;
    char_u	*shname = عدم;
    صحيح		noext = FALSE;
    bool	simple_pathext = true;
    صحيح		ext_count = 0;
    صحيح		retval = FALSE;

    אם (namelen >= حجم(buf))	// safety check
	החזר FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    אם (strstr((حرف *)shname, "sh") != عدم &&
	!(strstr((حرف *)shname, "powershell") != عدم
				    || strstr((حرف *)shname, "pwsh") != عدم))
	noext = TRUE;

    אם (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	אם (pathext.string == عدم)
	    STR_LITERAL_SET(pathext, ".com;.exe;.bat;.cmd");
	אחרת
	    pathext.length = STRLEN(pathext.string);

	אם (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    כלעוד (*p)
	    {
		אם (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    אם (*p == '.')
			simple_pathext = false;
		    ++p;
		    استمر;
		}
		e = vim_strchr(p, ';');
		אם (e == عدم)
		    e = pathext.string + pathext.length;
		plen = (size_t)(e - p);
		++ext_count;
		אם (*p != '.')
		    simple_pathext = false;
		עבור (q = p + 1; q < e; ++q)
		    אם (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		אם (_strnicoll(name + namelen - plen, (حرف *)p, plen) == 0)
		{
		    noext = TRUE;
		    اكسر;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    אם (pathext.string == عدم)
	STR_LITERAL_SET(pathext, ".");
    אחרת אם (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.length + 3);
	אם (tmp == عدم)
	{
	    retval = FALSE;
	    اذهب theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.length += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    אם (use_path && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	אם (p != عدم)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    אם (pathbuf.string == عدم)
	    {
		retval = FALSE;
		اذهب theend;
	    }

	    אם (mch_getenv("NoDefaultCurrentDirectoryInExePath") == عدم &&
		    strstr((حرف *)gettail(p_sh), "cmd.exe") != عدم)
	    {
		STRCPY(pathbuf.string, ".;");
		pathbuf.length = 2;
	    }
	    STRCPY(pathbuf.string + pathbuf.length, p);
	    pathbuf.length += plen;
	    pathbuf_allocated = TRUE;
	}
    }

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    אם (pathbuf.string == عدم)
	STR_LITERAL_SET(pathbuf, ".");
    p = pathbuf.string;
    כלעוד (*p)
    {
	char_u	*e;
	size_t	buflen;

	אם (*p == ';') // Skip empty entry
	{
	    ++p;
	    استمر;
	}
	e = vim_strchr(p, ';');
	אם (e == عدم)
	    e = pathbuf.string + pathbuf.length;

	אם (e - p + namelen + 2 > حجم(buf))
	{
	    retval = FALSE;
	    اذهب theend;
	}
	// A single "." that means current dir.
	אם (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	אחרת
	{
	    buflen = vim_snprintf_safelen(
		(حرف *)buf,
		حجم(buf),
		"%.*s%s%s", (صحيح)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	אם (!noext && simple_pathext && ext_count >= 3 && buflen > 0
		&& buflen + 2 <= حجم(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == عدم
		&& vim_strchr(gettail(buf), ':') == عدم)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_prefix_exists(buf);
	    buf[buflen] = NUL;
	    אם (!exists)
	    {
		p = e;
		استمر;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	כלעוד (*p)
	{
	    char_u  *e2;

	    אם (*p == ';')
	    {
		// Skip empty entry
		++p;
		استمر;
	    }
	    e2 = vim_strchr(p, ';');
	    אם (e2 == عدم)
		e2 = pathext.string + pathext.length;

	    אם (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		אם (e2 - p + buflen + 1 > حجم(buf))
		{
		    retval = FALSE;
		    اذهب theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    אם (executable_file((حرف *)buf, path))
	    {
		retval = TRUE;
		اذهب theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    אם (pathbuf_allocated)
	حرر(pathbuf.string);
    אם (pathext_allocated)
	حرر(pathext.string);
    החזר retval;
}

#אם (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
	(defined(_MSC_VER) && _MSC_VER >= 1400)
/*
 * Bad parameter handler.
 *
 * Certain MS CRT functions will intentionally crash when passed invalid
 * parameters to highlight possible security holes.  Setting this function as
 * the bad parameter handler will prevent the crash.
 *
 * In debug builds the parameters contain CRT information that might help track
 * down the source of a problem, but in non-debug builds the arguments are all
 * NULL/0.  Debug builds will also produce assert dialogs from the CRT, it is
 * worth allowing these to make debugging of issues easier.
 */
    ساكن فراغ
bad_param_handler(ثابت wchar_t *expression UNUSED,
    ثابت wchar_t *function UNUSED,
    ثابت wchar_t *file UNUSED,
    unsigned صحيح line UNUSED,
    uintptr_t pReserved UNUSED)
{
}

# define SET_INVALID_PARAM_HANDLER \
	((فراغ)_set_invalid_parameter_handler(bad_param_handler))
#אחרת
# define SET_INVALID_PARAM_HANDLER
#endif

#ifdef FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    ساكن فراغ
mch_init_g(فراغ)
{
# ifndef __MINGW32__
    خارجي صحيح _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_BINARY;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    Rows = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[_MAX_PATH + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	אם (exe_pathlen + 10 >= حجم(vimrun_location))
	{
	    אם (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    عدم, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	אחרת
	{
	    // First try in same directory as gvim.exe.
	    אם (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    אם (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		אם (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(حرف *)vimrun_location,
			حجم(vimrun_location),
			"\"%.*svimrun\" ",
			(صحيح)exe_pathlen, exe_name);
		}
		אחרת
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		אם (p != عدم)
		{
		    vimrun_path.string = p;
		    vimrun_path.length = plen;
		    vimrun_path_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    אחרת אם (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    عدم, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	אם (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    אם (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    عدم, TRUE, FALSE))
	set_option_value_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#endif // FEAT_GUI_MSWIN

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).Right - (sr).Left + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).Top + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    ساكن BOOL
ClearConsoleBuffer(WORD wAttribute)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    DWORD NumCells, dummy;

    אם (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	החזר FALSE;

    NumCells = csbi.dwSize.X * csbi.dwSize.Y;
    coord.X = 0;
    coord.Y = 0;
    אם (!FillConsoleOutputCharacter(g_hConOut, ' ', NumCells,
	    coord, &dummy))
	החזר FALSE;
    אם (!FillConsoleOutputAttribute(g_hConOut, wAttribute, NumCells,
	    coord, &dummy))
	החזר FALSE;

    החזר TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    ساكن BOOL
FitConsoleWindow(
    COORD dwBufferSize,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD dwWindowSize;
    BOOL NeedAdjust = FALSE;

    אם (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	החזר FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    אם (csbi.srWindow.Right >= dwBufferSize.X)
    {
	dwWindowSize.X = SRWIDTH(csbi.srWindow);
	אם (dwWindowSize.X > dwBufferSize.X)
	    dwWindowSize.X = dwBufferSize.X;
	csbi.srWindow.Right = dwBufferSize.X - 1;
	csbi.srWindow.Left = dwBufferSize.X - dwWindowSize.X;
	NeedAdjust = TRUE;
    }
    אם (csbi.srWindow.Bottom >= dwBufferSize.Y)
    {
	dwWindowSize.Y = SRHEIGHT(csbi.srWindow);
	אם (dwWindowSize.Y > dwBufferSize.Y)
	    dwWindowSize.Y = dwBufferSize.Y;
	csbi.srWindow.Bottom = dwBufferSize.Y - 1;
	csbi.srWindow.Top = dwBufferSize.Y - dwWindowSize.Y;
	NeedAdjust = TRUE;
    }
    אם (NeedAdjust && WantAdjust)
    {
	אם (!SetConsoleWindowInfo(g_hConOut, TRUE, &csbi.srWindow))
	    החזר FALSE;
    }
    החזר TRUE;
}

تعريفنوع بنية ConsoleBufferStruct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_BUFFER_INFO	Info;
    PCHAR_INFO			Buffer;
    COORD			BufferSize;
    PSMALL_RECT			Regions;
    صحيح				NumRegions;
} ConsoleBuffer;

/*
 * SaveConsoleBuffer()
 * Description:
 *  Saves important information about the console buffer, including the
 *  actual buffer contents.  The saved information is suitable for later
 *  restoration by RestoreConsoleBuffer().
 * Returns:
 *  TRUE if all information was saved; FALSE otherwise
 *  If FALSE, still sets cb->IsValid if buffer characteristics were saved.
 */
    ساكن BOOL
SaveConsoleBuffer(
    ConsoleBuffer *cb)
{
    DWORD NumCells;
    COORD BufferCoord;
    SMALL_RECT ReadRegion;
    WORD Y, Y_incr;
    صحيح i, numregions;

    אם (cb == عدم)
	החזר FALSE;

    אם (!GetConsoleScreenBufferInfo(g_hConOut, &cb->Info))
    {
	cb->IsValid = FALSE;
	החזר FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    אם (use_alternate_screen_buffer)
	החזר TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    אם (!cb->IsValid || cb->Buffer == عدم ||
	    cb->BufferSize.X != cb->Info.dwSize.X ||
	    cb->BufferSize.Y != cb->Info.dwSize.Y)
    {
	cb->BufferSize.X = cb->Info.dwSize.X;
	cb->BufferSize.Y = cb->Info.dwSize.Y;
	NumCells = cb->BufferSize.X * cb->BufferSize.Y;
	vim_free(cb->Buffer);
	cb->Buffer = ALLOC_MULT(CHAR_INFO, NumCells);
	אם (cb->Buffer == عدم)
	    החזר FALSE;
    }

    /*
     * We will now copy the console screen buffer into our buffer.
     * ReadConsoleOutput() seems to be limited as far as how much you
     * can read at a time.  Empirically, this number seems to be about
     * 12000 cells (rows * columns).  Start at position (0, 0) and copy
     * in chunks until it is all copied.  The chunks will all have the
     * same horizontal characteristics, so initialize them now.  The
     * height of each chunk will be (12000 / width).
     */
    BufferCoord.X = 0;
    ReadRegion.Left = 0;
    ReadRegion.Right = cb->Info.dwSize.X - 1;
    Y_incr = 12000 / cb->Info.dwSize.X;

    numregions = (cb->Info.dwSize.Y + Y_incr - 1) / Y_incr;
    אם (cb->Regions == عدم || numregions != cb->NumRegions)
    {
	cb->NumRegions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->NumRegions);
	אם (cb->Regions == عدم)
	{
	    VIM_CLEAR(cb->Buffer);
	    החזר FALSE;
	}
    }

    עבור (i = 0, Y = 0; i < cb->NumRegions; i++, Y += Y_incr)
    {
	/*
	 * Read into position (0, Y) in our buffer.
	 */
	BufferCoord.Y = Y;
	/*
	 * Read the region whose top left corner is (0, Y) and whose bottom
	 * right corner is (width - 1, Y + Y_incr - 1).  This should define
	 * a region of size width by Y_incr.  Don't worry if this region is
	 * too large for the remaining buffer; it will be cropped.
	 */
	ReadRegion.Top = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	אם (!ReadConsoleOutputW(g_hConOut,	// output handle
		cb->Buffer,			// our buffer
		cb->BufferSize,			// dimensions of our buffer
		BufferCoord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->Buffer);
	    VIM_CLEAR(cb->Regions);
	    החזר FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    החזר TRUE;
}

/*
 * RestoreConsoleBuffer()
 * Description:
 *  Restores important information about the console buffer, including the
 *  actual buffer contents, if desired.  The information to restore is in
 *  the same format used by SaveConsoleBuffer().
 * Returns:
 *  TRUE on success
 */
    ساكن BOOL
RestoreConsoleBuffer(
    ConsoleBuffer   *cb,
    BOOL	    RestoreScreen)
{
    COORD BufferCoord;
    SMALL_RECT WriteRegion;
    صحيح i;

    אם (cb == عدم || !cb->IsValid)
	החזר FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    אם (RestoreScreen)
	ClearConsoleBuffer(cb->Info.wAttributes);

    FitConsoleWindow(cb->Info.dwSize, TRUE);
    אם (!SetConsoleScreenBufferSize(g_hConOut, cb->Info.dwSize))
	החזר FALSE;
    אם (!SetConsoleTextAttribute(g_hConOut, cb->Info.wAttributes))
	החזר FALSE;

    אם (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	החזר TRUE;
    }

    אם (!SetConsoleCursorPosition(g_hConOut, cb->Info.dwCursorPosition))
	החזר FALSE;
    אם (!SetConsoleWindowInfo(g_hConOut, TRUE, &cb->Info.srWindow))
	החזר FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    אם (use_alternate_screen_buffer)
	החזר TRUE;

    /*
     * Restore the screen buffer contents.
     */
    אם (cb->Buffer != عدم)
    {
	עבור (i = 0; i < cb->NumRegions; i++)
	{
	    BufferCoord.X = cb->Regions[i].Left;
	    BufferCoord.Y = cb->Regions[i].Top;
	    WriteRegion = cb->Regions[i];
	    אם (!WriteConsoleOutputW(g_hConOut,	// output handle
			cb->Buffer,		// our buffer
			cb->BufferSize,		// dimensions of our buffer
			BufferCoord,		// offset in our buffer
			&WriteRegion))		// region to restore
		החזר FALSE;
	}
    }

    החזר TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
ساكن ConsoleBuffer g_cbOrig = { 0 };
# endif
ساكن ConsoleBuffer g_cbNonTermcap = { 0 };
ساكن ConsoleBuffer g_cbTermcap = { 0 };

حرف g_szOrigTitle[256] = { 0 };
HWND g_hWnd = عدم;	// also used in os_mswin.c
ساكن HICON g_hOrigIconSmall = عدم;
ساكن HICON g_hOrigIcon = عدم;
ساكن HICON g_hVimIcon = عدم;
ساكن BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ساكن BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    אם (hWnd == عدم)
	החזר FALSE;

    אם (phIconSmall != عدم)
	*phIconSmall = (HICON)SendMessage(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    אם (phIcon != عدم)
	*phIcon = (HICON)SendMessage(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    החזר TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ساكن BOOL
SetConsoleIcon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    אם (hWnd == عدم)
	החזר FALSE;

    אם (hIconSmall != عدم)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    אם (hIcon != عدم)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    החזר TRUE;
}

/*
 * SaveConsoleTitleAndIcon()
 * Description:
 *  Saves the current console window title in g_szOrigTitle, for later
 *  restoration.  Also, attempts to obtain a handle to the console window,
 *  and use it to save the small and big icons currently in use by the
 *  console window.  This is not always possible on some versions of Windows;
 *  nor is it possible when running Vim remotely using Telnet (since the
 *  console window the user sees is owned by a remote process).
 */
    ساكن فراغ
SaveConsoleTitleAndIcon(فراغ)
{
    // Save the original title.
    אם (!GetConsoleTitle(g_szOrigTitle, حجم(g_szOrigTitle)))
	החזר;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = GetConsoleWindow();
    אם (g_hWnd == عدم)
	החזר;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    אם (g_hOrigIconSmall == عدم || g_hOrigIcon == عدم)
	החזר;

    // Extract the first icon contained in the Vim executable.
    אם (
# ifdef FEAT_LIBCALL
	    mch_icon_load((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == عدم)
	g_hVimIcon = ExtractIcon(عدم, (LPCSTR)exe_name, 0);
    אם (g_hVimIcon != عدم)
	g_fCanChangeIcon = TRUE;
}

ساكن صحيح g_fWindInitCalled = FALSE;
ساكن صحيح g_fTermcapMode = FALSE;
ساكن CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    ساكن فراغ
mch_init_c(فراغ)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_BUFFER_INFO csbi;
# endif
# ifndef __MINGW32__
    خارجي صحيح _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_BINARY;		// we do our own CR-LF translation
    out_flush();

    // Obtain handles for the standard Console I/O devices
    אם (read_cmd_fd == 0)
	g_hConIn =  GetStdHandle(STD_INPUT_HANDLE);
    אחרת
	create_conin();
    g_hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    SaveConsoleBuffer(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.wAttributes;
# אחרת
    // Get current text attributes
    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
# endif
    אם (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (g_attrCurrent & 0xf) + 1;
    אם (cterm_normal_bg_color == 0)
	cterm_normal_bg_color = ((g_attrCurrent >> 4) & 0xf) + 1;

    // Fg and Bg color index number at startup
    g_color_index_fg = g_attrDefault & 0xf;
    g_color_index_bg = (g_attrDefault >> 4) & 0xf;

    // set termcap codes to current text attributes
    update_tcap(g_attrCurrent);

    GetConsoleCursorInfo(g_hConOut, &g_cci);
    GetConsoleMode(g_hConIn,  &g_cmodein);
    GetConsoleMode(g_hConOut, &g_cmodeout);

    SaveConsoleTitleAndIcon();
    /*
     * Set both the small and big icons of the console window to Vim's icon.
     * Note that Vim presently only has one size of icon (32x32), but it
     * automatically gets scaled down to 16x16 when setting the small icon.
     */
    אם (g_fCanChangeIcon)
	SetConsoleIcon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    אם (use_alternate_screen_buffer)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    אם (fdDump)
    {
	time_t t;

	time(&t);
	fputs(ctime(&t), fdDump);
	fflush(fdDump);
    }
# endif

    g_fWindInitCalled = TRUE;

    g_fMouseAvail = GetSystemMetrics(SM_MOUSEPRESENT);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif
}

/*
 * non-GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    ساكن فراغ
mch_exit_c(صحيح r)
{
    // Copy flag since stoptermcap() will clear the flag.
    صحيح fTermcapMode = g_fTermcapMode;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    אם (!fTermcapMode &&  use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    אם (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    אם (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	אם (g_fCanChangeIcon && !g_fForceExit)
	    SetConsoleIcon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	אם (fdDump)
	{
	    time_t t;

	    time(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = عدم;
# endif
    }

    SetConsoleCursorInfo(g_hConOut, &g_cci);
    SetConsoleMode(g_hConIn,  g_cmodein | ENABLE_EXTENDED_FLAGS);
    SetConsoleMode(g_hConOut, g_cmodeout);

# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    exit(r);
}
#endif // !FEAT_GUI_MSWIN || VIMDLL

    فراغ
mch_init(فراغ)
{
#ifdef VIMDLL
    אם (gui.starting)
	mch_init_g();
    אחרת
	mch_init_c();
#وإلاإذا defined(FEAT_GUI_MSWIN)
    mch_init_g();
#אחרת
    mch_init_c();
#endif
}

    فراغ
mch_exit(صحيح r)
{
#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef VIMDLL
    אם (vimrun_path_allocated)
	vim_free(vimrun_path.string);

    אם (gui.in_use || gui.starting)
	mch_exit_g(r);
    אחרת
	mch_exit_c(r);
#وإلاإذا defined(FEAT_GUI_MSWIN)
    אם (vimrun_path_allocated)
	vim_free(vimrun_path.string);
    mch_exit_g(r);
#אחרת
    mch_exit_c(r);
#endif
}

/*
 * Do we have an interactive window?
 */
    صحيح
mch_check_win(
    صحيح argc UNUSED,
    حرف **argv UNUSED)
{
    mch_get_exe_name();

#אם defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    החזר OK;	    // GUI always has a tty
#אחרת
# ifdef VIMDLL
    אם (gui.in_use)
	החזר OK;
# endif
    אם (isatty(1))
	החזר OK;
    החזר FAIL;
#endif
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    فراغ
fname_case(
    char_u	*name,
    صحيح		len)
{
    WCHAR   *p;
    WCHAR   buf[_MAX_PATH + 1];

    אם (*name == NUL)
	החזר;

    slash_adjust(name);

    p = enc_to_utf16(name, عدم);
    אם (p == عدم)
	החזר;

    אם (GetLongPathNameW(p, buf, _MAX_PATH))
    {
	char_u	*q = utf16_to_enc(buf, عدم);

	אם (q != عدم)
	{
	    אם (len > 0)
		vim_strncpy(name, q, len - 1);
	    אחרת
	    {
		size_t  namelen = STRLEN(name);

		אם (namelen >= STRLEN(q))
		    vim_strncpy(name, q, namelen);
	    }
	    vim_free(q);
	}
    }
    vim_free(p);
}


/*
 * Insert user name in s[len].
 */
    صحيح
mch_get_user_name(
    char_u  *s,
    صحيح	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_LENGTH(wszUserName);

    אם (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_to_enc(wszUserName, عدم);

	אם (p != عدم)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    החזר OK;
	}
    }
    s[0] = NUL;
    החזר FAIL;
}


/*
 * Insert host name in s[len].
 */
    فراغ
mch_get_host_name(
    char_u	*s,
    صحيح		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_LENGTH(wszHostName);

    אם (!GetComputerNameW(wszHostName, &wcch))
	החזר;

    char_u  *p = utf16_to_enc(wszHostName, عدم);
    אם (p == عدم)
	החזר;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    long
mch_get_pid(فراغ)
{
    החזר (long)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    صحيح
mch_process_running(long pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   status = 0;
    صحيح	    ret = FALSE;

    אם (hProcess == عدم)
	החזר FALSE;  // might not have access
    אם (GetExitCodeProcess(hProcess, &status))
	ret = status == STILL_ACTIVE;
    CloseHandle(hProcess);
    החזר ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    صحيح
mch_dirname(
    char_u	*buf,
    صحيح		len)
{
    WCHAR   wbuf[_MAX_PATH + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    אם (GetCurrentDirectoryW(_MAX_PATH, wbuf) == 0)
	החזר FAIL;

    WCHAR   wcbuf[_MAX_PATH + 1];
    char_u  *p = عدم;

    אם (GetLongPathNameW(wbuf, wcbuf, _MAX_PATH) != 0)
    {
	p = utf16_to_enc(wcbuf, عدم);
	אם (p != عدم)
	{
	    אם (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    אם (p == عدم)
	p = utf16_to_enc(wbuf, عدم);

    אם (p == عدم)
	החזר FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    החזר OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    long
mch_getperm(char_u *name)
{
    stat_T	st;

    החזר (mch_stat((حرف *)name, &st) == 0)
	? (long)(unsigned short)st.st_mode
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
mch_setperm(char_u *name, long perm)
{
    long	n;
    WCHAR	*p;

    p = enc_to_utf16(name, عدم);
    אם (p == عدم)
	החזר FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    אם (n == -1)
	החזר FAIL;

    win32_set_archive(name);

    החזר OK;
}

/*
 * Set hidden flag for "name".
 */
    فراغ
mch_hide(char_u *name)
{
    صحيح attrs = win32_getattrs(name);
    אם (attrs == -1)
	החזר;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    صحيح
mch_ishidden(char_u *name)
{
    صحيح f = win32_getattrs(name);

    אם (f == -1)
	החזר FALSE;		    // file does not exist at all

    החזר (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    صحيح
mch_isdir(char_u *name)
{
    صحيح f = win32_getattrs(name);

    אם (f == -1)
	החזר FALSE;		    // file does not exist at all

    החזר (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    صحيح
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_to_utf16(name, عدم);
    אם (p == عدم)
	החזר FALSE;
    attr = GetFileAttributesW(p);
    אם (attr == INVALID_FILE_ATTRIBUTES)
	error = GetLastError();
    vim_free(p);

    אם (attr == INVALID_FILE_ATTRIBUTES)
	החזר error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    החזר (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    صحيح
mch_isrealdir(char_u *name)
{
    החזר mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    صحيح
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    صحيح	    retval;

    p = enc_to_utf16(name, عدم);
    אם (p == عدم)
	החזר -1;
    retval = _wmkdir(p);
    vim_free(p);
    החזר retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    صحيح
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    صحيح	    retval;

    p = enc_to_utf16(name, عدم);
    אם (p == عدم)
	החזר -1;
    retval = _wrmdir(p);
    vim_free(p);
    החזר retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    صحيح
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    החזר win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.nNumberOfLinks > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    صحيح
mch_is_symbolic_link(char_u *name)
{
    HANDLE		hFind;
    صحيح			res = FALSE;
    DWORD		fileFlags = 0, reparseTag = 0;
    WCHAR		*wn;
    WIN32_FIND_DATAW	findDataW;

    wn = enc_to_utf16(name, عدم);
    אם (wn == عدم)
	החזר FALSE;

    hFind = FindFirstFileW(wn, &findDataW);
    vim_free(wn);
    אם (hFind != INVALID_HANDLE_VALUE)
    {
	fileFlags = findDataW.dwFileAttributes;
	reparseTag = findDataW.dwReserved0;
	FindClose(hFind);
    }

    אם ((fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
	    && (reparseTag == IO_REPARSE_TAG_SYMLINK
		|| reparseTag == IO_REPARSE_TAG_MOUNT_POINT))
	res = TRUE;

    החזר res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    صحيح
mch_is_linked(char_u *fname)
{
    אם (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	החזר TRUE;
    החזר FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    صحيح
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    صحيح		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_to_utf16(fname, عدم);
    אם (wn == عدم)
	החזר FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    عدم,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    عدم);		// handle to template file
    vim_free(wn);

    אם (hFile == INVALID_HANDLE_VALUE)
	החזר FILEINFO_READ_FAIL;

    אם (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    אחרת
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    החזר res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    ساكن صحيح
win32_getattrs(char_u *name)
{
    صحيح		attr;
    WCHAR	*p;

    p = enc_to_utf16(name, عدم);
    אם (p == عدم)
	החזר INVALID_FILE_ATTRIBUTES;

    attr = GetFileAttributesW(p);
    vim_free(p);

    החזר attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    ساكن صحيح
win32_setattrs(char_u *name, صحيح attrs)
{
    صحيح	    res;
    WCHAR   *p;

    p = enc_to_utf16(name, عدم);
    אם (p == عدم)
	החזר -1;

    res = SetFileAttributesW(p, attrs);
    vim_free(p);

    החזר res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    ساكن صحيح
win32_set_archive(char_u *name)
{
    صحيح attrs = win32_getattrs(name);
    אם (attrs == -1)
	החזר -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    החזר win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    صحيح
mch_writable(char_u *name)
{
    صحيح attrs = win32_getattrs(name);

    החזר (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    صحيح
mch_can_exe(char_u *name, char_u **path, صحيح use_path UNUSED)
{
    החזר executable_exists((حرف *)name, STRLEN(name), path, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    صحيح
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    صحيح		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    אם (STRNCMP(name, "\\\\.\\", 4) == 0)
	החזר NODE_WRITABLE;

    wn = enc_to_utf16(name, عدم);
    אם (wn == عدم)
	החזר NODE_NORMAL;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    عدم,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    عدم);		    // handle to template file
    vim_free(wn);
    אם (hFile == INVALID_HANDLE_VALUE)
	החזר NODE_NORMAL;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    אם (type == FILE_TYPE_CHAR)
	החזר NODE_WRITABLE;
    אם (type == FILE_TYPE_DISK)
	החזר NODE_NORMAL;
    החזר NODE_OTHER;
}

#ifdef HAVE_ACL
بنية my_acl
{
    PSECURITY_DESCRIPTOR    pSecurityDescriptor;
    PSID		    pSidOwner;
    PSID		    pSidGroup;
    PACL		    pDacl;
    PACL		    pSacl;
};
#endif

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname)
{
#ifndef HAVE_ACL
    החזר (vim_acl_T)عدم;
#אחרת
    بنية my_acl   *p = عدم;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(بنية my_acl);
    אם (p != عدم)
    {
	WCHAR	*wn;

	wn = enc_to_utf16(fname, عدم);
	אם (wn == عدم)
	{
	    vim_free(p);
	    החזר عدم;
	}

	// Try to retrieve the entire security descriptor.
	err = GetNamedSecurityInfoW(
		wn,			// Abstract filename
		SE_FILE_OBJECT,		// File Object
		OWNER_SECURITY_INFORMATION |
		GROUP_SECURITY_INFORMATION |
		DACL_SECURITY_INFORMATION |
		SACL_SECURITY_INFORMATION,
		&p->pSidOwner,		// Ownership information.
		&p->pSidGroup,		// Group membership.
		&p->pDacl,		// Discretionary information.
		&p->pSacl,		// For auditing purposes.
		&p->pSecurityDescriptor);
	אם (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (فراغ)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    عدم,
		    عدم,
		    &p->pDacl,
		    عدم,
		    &p->pSecurityDescriptor);
	}
	אם (p->pSecurityDescriptor == عدم)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = عدم;
	}
	vim_free(wn);
    }

    החזר (vim_acl_T)p;
#endif
}

#ifdef HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    ساكن BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_SIZE_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.AceCount = 0;
    GetAclInformation(acl, &acl_info, حجم(acl_info), AclSizeInformation);
    עבור (i = 0; i < acl_info.AceCount; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	אם (ace->Header.AceFlags & INHERITED_ACE)
	    החזר TRUE;
    }
    החזר FALSE;
}
#endif

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    فراغ
mch_set_acl(char_u *fname, vim_acl_T acl)
{
#ifdef HAVE_ACL
    بنية my_acl   *p = (بنية my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    אם (p == عدم)
	החזר;

    wn = enc_to_utf16(fname, عدم);
    אם (wn == عدم)
	החזר;

    // Set security flags
    אם (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    אם (p->pSidGroup)
	sec_info |= GROUP_SECURITY_INFORMATION;
    אם (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	אם (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    אם (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (فراغ)SetNamedSecurityInfoW(
	    wn,			// Abstract filename
	    SE_FILE_OBJECT,	// File Object
	    sec_info,
	    p->pSidOwner,	// Ownership information.
	    p->pSidGroup,	// Group membership.
	    p->pDacl,		// Discretionary information.
	    p->pSacl		// For auditing purposes.
	    );
    vim_free(wn);
#endif
}

    فراغ
mch_free_acl(vim_acl_T acl)
{
#ifdef HAVE_ACL
    بنية my_acl   *p = (بنية my_acl *)acl;

    אם (p != عدم)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#endif
}

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    ساكن BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    INPUT_RECORD ir;
    DWORD out;

    بدل (dwCtrlType)
    {
    حالة CTRL_C_EVENT:
	אם (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	החזר TRUE;

    حالة CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = KEY_EVENT;
	ir.Event.KeyEvent.bKeyDown = TRUE;
	ir.Event.KeyEvent.wRepeatCount = 1;
	ir.Event.KeyEvent.wVirtualKeyCode = VK_CANCEL;
	ir.Event.KeyEvent.wVirtualScanCode = 0;
	ir.Event.KeyEvent.dwControlKeyState = 0;
	ir.Event.KeyEvent.uChar.UnicodeChar = 0;
	WriteConsoleInput(g_hConIn, &ir, 1, &out);
	החזר TRUE;

    // fatal events: shut down gracefully
    حالة CTRL_CLOSE_EVENT:
    حالة CTRL_LOGOFF_EVENT:
    حالة CTRL_SHUTDOWN_EVENT:
	windgoto((صحيح)Rows - 1, cmdline_col_off);
	g_fForceExit = TRUE;

	vim_snprintf((حرف *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	החזר TRUE;		// not reached

    افتراضي:
	החזר FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    فراغ
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    אם (gui.in_use)
	החזר;
# endif
    GetConsoleMode(g_hConIn, &cmodein);
    GetConsoleMode(g_hConOut, &cmodeout);
    אם (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT |
		     ENABLE_ECHO_INPUT);
	אם (g_fMouseActive)
	{
	    cmodein |= ENABLE_MOUSE_INPUT;
	    cmodein &= ~ENABLE_QUICK_EDIT_MODE;
	}
	אחרת
	{
	    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# אחרת
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    אחרת // cooked
    {
	cmodein |= (ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT |
		    ENABLE_ECHO_INPUT);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
    SetConsoleMode(g_hConOut, cmodeout);
    SetConsoleCtrlHandler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    אם (fdDump)
    {
	fprintf(fdDump, "mch_settmode(%s, in = %x, out = %x)\n",
		tmode == TMODE_RAW ? "raw" :
				    tmode == TMODE_COOK ? "cooked" : "normal",
		cmodein, cmodeout);
	fflush(fdDump);
    }
# endif
}


/*
 * Get the size of the current window in `Rows' and `Columns'
 * Return OK when size could be determined, FAIL otherwise.
 */
    صحيح
mch_get_shellsize(فراغ)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

# ifdef VIMDLL
    אם (gui.in_use)
	החזר OK;
# endif
    אם (!g_fTermcapMode && g_cbTermcap.IsValid)
    {
	/*
	 * For some reason, we are trying to get the screen dimensions
	 * even though we are not in termcap mode.  The 'Rows' and 'Columns'
	 * variables are really intended to mean the size of Vim screen
	 * while in termcap mode.
	 */
	Rows = g_cbTermcap.Info.dwSize.Y;
	Columns = g_cbTermcap.Info.dwSize.X;
    }
    אחרת אם (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	Rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	Columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    אחרת
    {
	Rows = 25;
	Columns = 80;
    }
    החזר OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    ساكن فراغ
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    אם (use_alternate_screen_buffer)
	החזר;

    אם (!SetConsoleScreenBufferSize(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	אם (fdDump)
	{
	    fprintf(fdDump, "SetConsoleScreenBufferSize failed: %lx\n",
		    GetLastError());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Resize console window size to 'srWindowRect'
 */
    ساكن فراغ
ResizeWindow(
    HANDLE     hConsole,
    SMALL_RECT srWindowRect)
{
    אם (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
    {
# ifdef MCH_WRITE_DUMP
	אם (fdDump)
	{
	    fprintf(fdDump, "SetConsoleWindowInfo failed: %lx\n",
		    GetLastError());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Set a console window to `xSize' * `ySize'
 */
    ساكن فراغ
ResizeConBufAndWindow(
    HANDLE  hConsole,
    صحيح	    xSize,
    صحيح	    ySize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    srWindowRect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    ساكن صحيح	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    אם (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", xSize, ySize);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    // define the new console window size and scroll position
    srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
    srWindowRect.Right =  (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);

    אם (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	صحيح sx, sy;

	sx = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	sy = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	אם (sy < ySize || sx < xSize)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    אם (sy < ySize)
		coordScreen.Y = ySize;
	    אחרת
		coordScreen.Y = sy;
	    אם (sx < xSize)
		coordScreen.X = xSize;
	    אחרת
		coordScreen.X = sx;
	    SetConsoleScreenBufferSize(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = xSize;
    coordScreen.Y = ySize;

    // In the new console call API, only the first time in reverse order
    אם (!vtp_working || resized)
    {
	ResizeWindow(hConsole, srWindowRect);
	ResizeConBuf(hConsole, coordScreen);
    }
    אחרת
    {
	// Workaround for a Windows 10 bug
	cursor.X = srWindowRect.Left;
	cursor.Y = srWindowRect.Top;
	SetConsoleCursorPosition(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	ResizeWindow(hConsole, srWindowRect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    فراغ
mch_set_shellsize(فراغ)
{
    COORD coordScreen;

# ifdef VIMDLL
    אם (gui.in_use)
	החזר;
# endif
    // Don't change window size while still starting up
    אם (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	החזר;
    }

    אם (term_console)
    {
	coordScreen = GetLargestConsoleWindowSize(g_hConOut);

	// Clamp Rows and Columns to reasonable values
	אם (Rows > coordScreen.Y)
	    Rows = coordScreen.Y;
	אם (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	ResizeConBufAndWindow(g_hConOut, Columns, Rows);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    فراغ
mch_new_shellsize(فراغ)
{
# ifdef VIMDLL
    אם (gui.in_use)
	החזר;
# endif
    set_scroll_region(0, 0, Columns - 1, Rows - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    فراغ
mch_set_winsize_now(فراغ)
{
    אם (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_set_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# אם defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    فراغ
mch_calc_cell_size(بنية cellsize *cs_out)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_out->cs_xpixel = -1;
    cs_out->cs_ypixel = -1;

    אם (g_hConOut == INVALID_HANDLE_VALUE)
	החזר;

    cfi.cbSize = حجم(cfi);
    אם (GetCurrentConsoleFontEx(g_hConOut, FALSE, &cfi)
	    && cfi.dwFontSize.X > 0 && cfi.dwFontSize.Y > 0)
    {
	cs_out->cs_xpixel = cfi.dwFontSize.X;
	cs_out->cs_ypixel = cfi.dwFontSize.Y;
	החזר;
    }

    GetConsoleMode(g_hConIn, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT
	    | ENABLE_PROCESSED_INPUT);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_INPUT;

    אם (SetConsoleMode(g_hConIn, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	SetConsoleMode(g_hConIn, cmodeold);
    }
}
# endif

#endif

    ساكن BOOL
vim_create_process(
    حرف		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    حرف		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = عدم;

    wcmd = enc_to_utf16((char_u *)cmd, عدم);
    אם (wcmd == عدم)
	החזר FALSE;
    אם (cwd != عدم)
    {
	wcwd = enc_to_utf16((char_u *)cwd, عدم);
	אם (wcwd == عدم)
	    اذهب theend;
    }

    ret = CreateProcessW(
	    عدم,		// Executable name
	    wcmd,		// Command to execute
	    عدم,		// Process security attributes
	    عدم,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    החזר ret;
}


    ساكن HINSTANCE
vim_shell_execute(
    حرف *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_to_utf16((char_u *)cmd, عدم);
    אם (wcmd == عدم)
	החזר (HINSTANCE) 0;

    ret = ShellExecuteW(عدم, عدم, wcmd, عدم, عدم, n_show_cmd);
    vim_free(wcmd);
    החזר ret;
}


#אם defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    ساكن صحيح
mch_system_classic(حرف *cmd, صحيح options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = حجم(si);
    si.lpReserved = عدم;
    si.lpDesktop = عدم;
    si.lpTitle = عدم;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    אם (options & SHELL_DOOUT)
	si.wShowWindow = SW_SHOWMINNOACTIVE;
    אחרת
	si.wShowWindow = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = عدم;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_MODE |	CREATE_NEW_CONSOLE,
	    &si, &pi, عدم, عدم);

    // Wait for the command to terminate before continuing
    {
	صحيح	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	עבור (;;)
	{
	    MSG	msg;

	    אם (PeekMessageW(&msg, (HWND)عدم, 0, 0, PM_REMOVE))
	    {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
		delay = 1;
		استمر;
	    }
	    אם (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		اكسر;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    אם (delay < 50)
		delay += 10;
	}

	// Get the command exit code
	GetExitCodeProcess(pi.hProcess, &ret);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // Try to get input focus back.  Doesn't always work though.
    PostMessage(hwnd, WM_SETFOCUS, 0, 0);
    // To increase the chances that WM_SETFOCUS will work, run the message loop
    // here.  In addition, it prevents problems caused by delayed WM_SETFOCUS
    // processing.
    gui_mch_update();

    החזר ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    ساكن unsigned صحيح __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_hChildStd_IN_Wr = param;
    linenr_T	    lnum = curbuf->b_op_start.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    صحيح		    written = 0;

    עבור (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	אם (l == 0)
	    len = 0;
	אחרת אם (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_hChildStd_IN_Wr, "", 1, &len, عدم);
	}
	אחרת
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_hChildStd_IN_Wr, (حرف *)lp + written,
		      s == عدم ? l : (DWORD)(s - (lp + written)),
		      &len, عدم);
	}
	אם (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    אם (lnum != curbuf->b_op_end.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_line_count
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_hChildStd_IN_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, عدم);
	    }

	    ++lnum;
	    אם (lnum > curbuf->b_op_end.lnum)
		اكسر;

	    lp = ml_get(lnum);
	    written = 0;
	}
	אחרת אם (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_hChildStd_IN_Wr);
    החזר 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    ساكن فراغ
dump_pipe(صحيح	    options,
	  HANDLE    g_hChildStd_OUT_Rd,
	  garray_T  *ga,
	  char_u    buffer[],
	  DWORD	    *buffer_off)
{
    DWORD	availableBytes = 0;
    DWORD	i;
    صحيح		ret;
    DWORD	len;
    DWORD	toRead;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_hChildStd_OUT_Rd, // pipe to query
			عدم,		    // optional buffer
			0,		    // buffer size
			عدم,		    // number of read bytes
			&availableBytes,    // available bytes total
			عدم);		    // byteLeft

    // We got real data in the pipe, read it
    כלעוד (ret != 0 && availableBytes > 0)
    {
	toRead = (DWORD)(BUFLEN - *buffer_off);
	toRead = availableBytes < toRead ? availableBytes : toRead;
	ReadFile(g_hChildStd_OUT_Rd, buffer + *buffer_off, toRead , &len, عدم);

	// If we haven't read anything, there is a problem
	אם (len == 0)
	    اكسر;

	availableBytes -= len;

	אם (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    עבור (i = 0; i < len; ++i)
	    {
		אם (buffer[i] == NL)
		    append_ga_line(ga);
		אחרת אם (buffer[i] == NUL)
		    ga_append(ga, NL);
		אחרת
		    ga_append(ga, buffer[i]);
	    }
	}
	אחרת אם (has_mbyte)
	{
	    صحيح		l;
	    صحيح		c;
	    char_u	*p;

	    len += *buffer_off;
	    buffer[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    עבור (p = buffer; p < buffer + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		אם (l == 0)
		    l = 1;  // NUL byte?
		אחרת אם (MB_BYTE2LEN(*p) != l)
		    اكسر;
	    }
	    אם (p == buffer)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		אם (len >= 12)
		    ++p;
		אחרת
		{
		    *buffer_off = len;
		    החזר;
		}
	    }
	    c = *p;
	    *p = NUL;
	    msg_puts((حرف *)buffer);
	    אם (p < buffer + len)
	    {
		*p = c;
		*buffer_off = (DWORD)((buffer + len) - p);
		mch_memmove(buffer, p, *buffer_off);
		החזר;
	    }
	    *buffer_off = 0;
	}
	אחרת
	{
	    buffer[len] = NUL;
	    msg_puts((حرف *)buffer);
	}

	windgoto(msg_row, cmdline_col_off + msg_col);
	cursor_on();
	out_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    ساكن صحيح
mch_system_piped(حرف *cmd, صحيح options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_hChildStd_IN_Rd = عدم;
    HANDLE g_hChildStd_IN_Wr = عدم;
    HANDLE g_hChildStd_OUT_Rd = عدم;
    HANDLE g_hChildStd_OUT_Wr = عدم;

    char_u	buffer[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    صحيح		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    صحيح		noread_cnt = 0;
    garray_T	ga;
    صحيح		delay = 1;
    DWORD	buffer_off = 0;	// valid bytes in buffer[]
    حرف	*p = عدم;

    SECURITY_ATTRIBUTES saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = حجم(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = عدم;

    אם ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)
	// Ensure the read handle to the pipe for STDOUT is not inherited.
       || ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)
	// Create a pipe for the child process's STDIN.
       || ! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)
	// Ensure the write handle to the pipe for STDIN is not inherited.
       || ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
    {
	CloseHandle(g_hChildStd_IN_Rd);
	CloseHandle(g_hChildStd_IN_Wr);
	CloseHandle(g_hChildStd_OUT_Rd);
	CloseHandle(g_hChildStd_OUT_Wr);
	msg_puts(_("\nCannot create pipes\n"));
    }

    si.cb = حجم(si);
    si.lpReserved = عدم;
    si.lpDesktop = عدم;
    si.lpTitle = عدم;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_hChildStd_OUT_Wr;
    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.hStdInput = g_hChildStd_IN_Rd;
    si.wShowWindow = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = عدم;

    אם (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    אם (cmd != عدم)
    {
	p = (حرف *)vim_strsave((char_u *)cmd);
	אם (p != عدم)
	    unescape_shellxquote((char_u *)p, p_sxe);
	אחרת
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_MODE,
						&si, &pi, عدم, عدم);

    אם (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(g_hChildStd_OUT_Wr);

    אם (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(عدم,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_hChildStd_IN_Wr,  // parameter
			0,		 // creation flag, start immediately
			عدم);		    // we don't care about thread id
	CloseHandle(thread);
	g_hChildStd_IN_Wr = عدم;
    }

    // Keep updating the window while waiting for the shell to finish.
    עבור (;;)
    {
	MSG	msg;

	אם (PeekMessageW(&msg, (HWND)عدم, 0, 0, PM_REMOVE))
	{
	    TranslateMessage(&msg);
	    DispatchMessageW(&msg);
	}

	// write pipe information in the window
	אם ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.in_use
# endif
	    )
	{
	    len = 0;
	    אם (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		אם (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		אם (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    אם (len == 1 && cmd != عدم)
		    {
			אם (ta_buf[ta_len] == Ctrl_C)
			{
			    // Learn what exit code is expected, for
			// now put 9 as SIGKILL
			    TerminateProcess(pi.hProcess, 9);
			}
		    }

		    /*
		     * Check for CTRL-D: EOF, close pipe to child.
		     * Ctrl_D may be decorated by _OnChar()
		     */
		    אם ((len == 1 || len == 4 ) && cmd != عدم)
		    {
			אם (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_hChildStd_IN_Wr);
			    g_hChildStd_IN_Wr = عدم;
			    len = 0;
			}
		    }

		    len = term_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    עבור (i = ta_len; i < ta_len + len; ++i)
		    {
			אם (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    msg_putchar(ta_buf[i]);
			אחרת אם (has_mbyte)
			{
			    صحيح l = (*mb_ptr2len)(ta_buf + i);

			    msg_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			אחרת
			    msg_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(msg_row, cmdline_col_off + msg_col);
		    out_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    אם (options & SHELL_WRITE)
			ta_len = 0;
		    אחרת אם (g_hChildStd_IN_Wr != عدم)
		    {
			WriteFile(g_hChildStd_IN_Wr, (حرف*)ta_buf,
				    1, &len, عدم);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			אם (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	אם (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	אם (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);
	    اكسر;
	}

	++noread_cnt;
	dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	אם (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_hChildStd_OUT_Rd);
    אם (g_hChildStd_IN_Wr != عدم)
	CloseHandle(g_hChildStd_IN_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    GetExitCodeProcess(pi.hProcess, &ret);

    אם (options & SHELL_READ)
    {
	אם (ga.ga_len > 0)
	{
	    append_ga_line(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	אחרת
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    החזר ret;
}

    ساكن صحيح
mch_system_g(حرف *cmd, صحيح options)
{
    // if we can pipe and the shelltemp option is off
    אם (!p_stmp)
	החזר mch_system_piped(cmd, options);
    אחרת
	החזר mch_system_classic(cmd, options);
}
#endif

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ساكن صحيح
mch_system_c(حرف *cmd, صحيح options UNUSED)
{
    صحيح		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    אם (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	אם (buf == عدم)
	    החזר -1;
	vim_snprintf((حرف *)buf, len, "(%s)", cmd);
	wcmd = enc_to_utf16(buf, عدم);
	حرر(buf);
    }
    אחרת
	wcmd = enc_to_utf16((char_u *)cmd, عدم);

    אם (wcmd == عدم)
	החזר -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    החזר ret;
}

#endif

    ساكن صحيح
mch_system(حرف *cmd, صحيح options)
{
#ifdef VIMDLL
    אם (gui.in_use || gui.starting)
	החזר mch_system_g(cmd, options);
    אחרת
	החזר mch_system_c(cmd, options);
#وإلاإذا defined(FEAT_GUI_MSWIN)
    החזר mch_system_g(cmd, options);
#אחרת
    החזר mch_system_c(cmd, options);
#endif
}

#אם defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ساكن صحيح
mch_call_shell_terminal(
    char_u	*cmd,
    صحيح		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = عدم;
    typval_T	argvar[2];
    long_u	cmdlen;
    صحيح		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    אם (cmd == عدم)
	cmdlen = STRLEN(p_sh) + 1;
    אחרת
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    אם (newcmd == عدم)
	החזר 255;
    אם (cmd == عدم)
    {
	STRCPY(newcmd, p_sh);
	ch_log(عدم, "starting terminal to run a shell");
    }
    אחרת
    {
	vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(عدم, "starting terminal for system command '%s'", cmd);
    }

    init_job_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = term_start(argvar, عدم, &opt, TERM_START_SYSTEM);
    אם (buf == عدم)
    {
	vim_free(newcmd);
	החזר 255;
    }

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    אם (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	כלעוד (term_use_loop())
	{
	    אם (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		אם (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    אחרת
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(عدم, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    החזר retval;
}
#endif
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    ساكن فراغ
restore_env_var(char_u *name, char_u *old_value, صحيح must_free)
{
    אם (old_value != عدم)
    {
	vim_setenv(name, old_value);
	אם (must_free)
	    vim_free(old_value);
	החזר;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    صحيح
mch_call_shell(
    char_u  *cmd,
    صحيح	    options)	// SHELL_*, see vim.h
{
    صحيح		x = 0;
    صحيح		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    صحيح		must_free;
    char_u	*oldval;

#ifdef FEAT_EVAL
    ch_log(عدم, "executing shell command: %s", cmd);
#endif
    // Change the title to reflect that we are in a subshell.
    אם (GetConsoleTitleW(szShellTitle, ARRAY_LENGTH(szShellTitle) - 4) > 0)
    {
	אם (cmd == عدم)
	    wcscat(szShellTitle, L" :sh");
	אחרת
	{
	    WCHAR *wn = enc_to_utf16((char_u *)cmd, عدم);

	    אם (wn != عدم)
	    {
		wcscat(szShellTitle, L" - !");
		אם ((wcslen(szShellTitle) + wcslen(wn) <
			    ARRAY_LENGTH(szShellTitle)))
		    wcscat(szShellTitle, wn);
		SetConsoleTitleW(szShellTitle);
		vim_free(wn);
	    }
	}
    }
    // do not execute anything from the current directory by setting the
    // environment variable $NoDefaultCurrentDirectoryInExePath
    oldval = vim_getenv((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    &must_free);
    vim_setenv((char_u *)"NoDefaultCurrentDirectoryInExePath", (char_u *)"1");

    out_flush();

#ifdef MCH_WRITE_DUMP
    אם (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#endif
#אם defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    אם (
# ifdef VIMDLL
	    gui.in_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != عدم
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	אם (cmdbase != عدم)
	    // Skip a leading quote and (.
	    כלעוד (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	אם (cmdbase == عدم || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    החזר x;
	}
    }
#endif

    /*
     * Catch all deadly signals while running the external command, because a
     * CTRL-C, Ctrl-Break or illegal instruction  might otherwise kill us.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGBREAK, SIG_IGN);
    mch_signal(SIGILL, SIG_IGN);
    mch_signal(SIGFPE, SIG_IGN);
    mch_signal(SIGSEGV, SIG_IGN);
    mch_signal(SIGTERM, SIG_IGN);
    mch_signal(SIGABRT, SIG_IGN);

    אם (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    אם (cmd == عدم)
    {
	x = mch_system((حرف *)p_sh, options);
    }
    אחרת
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = عدم;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	אם (*cmdbase == '"' )
	    ++cmdbase;
	אם (*cmdbase == '(')
	    ++cmdbase;

	אם ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    ZeroMemory(&si, حجم(si));
	    si.cb = حجم(si);
	    si.lpReserved = عدم;
	    si.lpDesktop = عدم;
	    si.lpTitle = عدم;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = عدم;

	    cmdbase = skipwhite(cmdbase + 5);
	    אם ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    אחרת אם ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_WINDOW;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdInput = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    عدم,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_NORMAL,		// File att.
		    عدم);				// Temp file
		si.hStdOutput = si.hStdInput;
		si.hStdError = si.hStdInput;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    אם (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		אם (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		אם (p > cmdbase && p[-1] == ')'
			&& (*cmd =='(' || cmd[1] == '('))
		    *--p = NUL;
	    }

	    newcmd = cmdbase;
	    unescape_shellxquote(cmdbase, p_sxe);

	    /*
	     * If creating new console, arguments are passed to the
	     * 'cmd.exe' as-is. If it's not, arguments are not treated
	     * correctly for current 'cmd.exe'. So unescape characters in
	     * shellxescape except '|' for avoiding to be treated as
	     * argument to them. Pass the arguments to sub-shell.
	     */
	    אם (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		אם (cmd_shell == عدم || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		אם (subcmd != عدم)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    אם (newcmd != عدم)
			vim_snprintf((حرف *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    אחרת
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    אם (vim_create_process((حرف *)newcmd, FALSE, flags,
			&si, &pi, عدم, عدم))
		x = 0;
	    אחרת אם (vim_shell_execute((حرف *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    אחרת
	    {
		x = -1;
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
		אם (gui.in_use)
# endif
		    emsg(_(e_command_not_found));
#endif
	    }

	    אם (newcmd != cmdbase)
		vim_free(newcmd);

	    אם (si.dwFlags == STARTF_USESTDHANDLES && si.hStdInput != عدم)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.hStdInput);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	אחרת
	{
	    cmdlen =
#ifdef FEAT_GUI_MSWIN
		((gui.in_use || gui.starting) ?
		    (!s_dont_use_vimrun && p_stmp ?
			vimrun_path.length : STRLEN(p_sh) + STRLEN(p_shcf))
		    : 0) +
#endif
		STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;

	    newcmd = alloc(cmdlen);
	    אם (newcmd != عدم)
	    {
#אם defined(FEAT_GUI_MSWIN)
		אם (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    حرف *msg = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    حرف *title = _("Vim Warning");
		    WCHAR *wmsg = enc_to_utf16((char_u *)msg, عدم);
		    WCHAR *wtitle = enc_to_utf16((char_u *)title, عدم);

		    אם (wmsg != عدم && wtitle != عدم)
			MessageBoxW(عدم, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		אם (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((حرف *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_path.string,
			    (msg_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		אחרת אם (
# ifdef VIMDLL
			(gui.in_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		אחרת
#endif
		    vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((حرف *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    אם (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    אם (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#אם defined(FEAT_GUI_MSWIN)
	    && ((gui.in_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#endif
	    )
    {
	smsg(_("shell returned %d"), x);
	msg_putchar('\n');
    }
    resettitle();

    mch_signal(SIGINT, SIG_DFL);
    mch_signal(SIGBREAK, SIG_DFL);
    mch_signal(SIGILL, SIG_DFL);
    mch_signal(SIGFPE, SIG_DFL);
    mch_signal(SIGSEGV, SIG_DFL);
    mch_signal(SIGTERM, SIG_DFL);
    mch_signal(SIGABRT, SIG_DFL);

    החזר x;
}

#אם defined(FEAT_JOB_CHANNEL)
    ساكن HANDLE
job_io_file_open(
	char_u *fname,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes)
{
    HANDLE h;
    WCHAR *wn;

    wn = enc_to_utf16(fname, عدم);
    אם (wn == عدم)
	החזר INVALID_HANDLE_VALUE;

    h = CreateFileW(wn, dwDesiredAccess, dwShareMode,
	    lpSecurityAttributes, dwCreationDisposition,
	    dwFlagsAndAttributes, عدم);
    vim_free(wn);
    החזר h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    فراغ
win32_build_env(dict_T *env, garray_T *gap, صحيح is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != عدم ? env->dv_hashtab.ht_used : 0;
    LPVOID	base = GetEnvironmentStringsW();

    // for last \0
    אם (ga_grow(gap, 1) == FAIL)
	החזר;

    אם (env != عدم)
    {
	FOR_ALL_HASHTAB_ITEMS(&env->dv_hashtab, hi, todo)
	{
	    אם (!HASHITEM_EMPTY(hi))
	    {
		typval_T *item = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_to_utf16((char_u *)hi->hi_key, عدم);
		WCHAR   *wval = enc_to_utf16(tv_get_string(item), عدم);
		--todo;
		אם (wkey != عدم && wval != عدم)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    אם (ga_grow(gap, (صحيح)(lkey + lval + 2)) == FAIL)
			استمر;
		    עבור (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'=';
		    עבור (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    אם (base)
    {
	WCHAR	*p = (WCHAR*) base;

	// for last \0
	אם (ga_grow(gap, 1) == FAIL)
	    החזר;

	כלעוד (*p != 0 || *(p + 1) != 0)
	{
	    אם (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_data + gap->ga_len++) = *p;
	    p++;
	}
	FreeEnvironmentStringsW(base);
	*((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
    }

# אם defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
    {
#  ifdef FEAT_CLIENTSERVER
	char_u	*servername = get_vim_var_str(VV_SEND_SERVER);
	size_t	servername_len = STRLEN(servername);
#  endif
#  ifdef FEAT_TERMINAL
	char_u	*version = get_vim_var_str(VV_VERSION);
	size_t	version_len = STRLEN(version);
#  endif
	// size of "VIM_SERVERNAME=" and value,
	// plus "VIM_TERMINAL=" and value,
	// plus two terminating NULs
	size_t	n = 0
#  ifdef FEAT_CLIENTSERVER
		    + 15 + servername_len
#  endif
#  ifdef FEAT_TERMINAL
		    + 13 + version_len + 2
#  endif
		    ;

	אם (ga_grow(gap, (صحيح)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    עבור (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    עבור (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    אם (is_terminal)
	    {
		עבור (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_data + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		עבור (n = 0; n < version_len; n++)
		    *((WCHAR*)gap->ga_data + gap->ga_len++) =
			(WCHAR)version[n];
		*((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
	    }
#  endif
	}
    }
# endif
}

/*
 * Create a pair of pipes.
 * Return TRUE for success, FALSE for failure.
 */
    ساكن BOOL
create_pipe_pair(HANDLE handles[2])
{
    ساكن LONG		s;
    حرف		name[64];
    SECURITY_ATTRIBUTES sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_SIZE, 0, 0, عدم);

    אם (handles[1] == INVALID_HANDLE_VALUE)
	החזר FALSE;

    sa.nLength = حجم(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = عدم;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    אם (handles[0] == INVALID_HANDLE_VALUE)
    {
	CloseHandle(handles[1]);
	החזר FALSE;
    }

    החזר TRUE;
}

#endif // FEAT_JOB_CHANNEL

#אם defined(FEAT_EVAL)
/*
 * Execute "argv" directly without the shell and return the output.
 * Used by system() and systemlist() when the command is a List.
 * "infile" is an optional temp file for stdin input.
 * When "ret_len" is not NULL, set it to the length of the output.
 * Returns the output in allocated memory (or NULL on error).
 * Sets v:shell_error to the exit status.
 */
    char_u *
mch_get_cmd_output_direct(
    حرف	**argv,
    char_u	*infile,
    صحيح		flags UNUSED,
    صحيح		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;
    HANDLE		hChildStdoutRd = INVALID_HANDLE_VALUE;
    HANDLE		hChildStdoutWr = INVALID_HANDLE_VALUE;
    HANDLE		hChildStdinRd = INVALID_HANDLE_VALUE;
    garray_T		cmd_ga;
    garray_T		out_ga;
    char_u		*buffer = عدم;
    DWORD		exit_code = (DWORD)-1;
    صحيح			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    עבור (i = 0; argv[i] != عدم; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	صحيح	has_spaces = FALSE;
	صحيح	j;

	עבור (j = 0; s[j] != NUL; j++)
	    אם (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		اكسر;
	    }

	אם (i > 0)
	    ga_append(&cmd_ga, ' ');

	אם (has_spaces)
	{
	    صحيح	num_bs;

	    ga_append(&cmd_ga, '"');
	    עבור (j = 0; arg[j] != NUL; j++)
	    {
		num_bs = 0;
		כלעוד (arg[j] == '\\')
		{
		    num_bs++;
		    j++;
		}

		אם (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    כלעוד (num_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    اكسر;
		}
		אחרת אם (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    כלעוד (num_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		אחרת
		{
		    כלעוד (num_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	אחרת
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&out_ga, 1, 4096);

    saAttr.nLength = حجم(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = عدم;

    // Create a pipe for the child's stdout.
    אם (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)
	    || !SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	اذهب done;
    }

    // Set up stdin from infile if provided.
    אם (infile != عدم)
    {
	WCHAR *winfile = enc_to_utf16(infile, عدم);

	אם (winfile != عدم)
	{
	    hChildStdinRd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL, عدم);
	    vim_free(winfile);
	}
    }

    ZeroMemory(&pi, حجم(pi));
    ZeroMemory(&si, حجم(si));
    si.cb = حجم(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;
    si.hStdInput = (hChildStdinRd != INVALID_HANDLE_VALUE)
		    ? hChildStdinRd : INVALID_HANDLE_VALUE;

    ch_log(عدم, "directly executing: %s", (حرف *)cmd_ga.ga_data);

    // Create the child process directly, without going through the shell.
    אם (!vim_create_process((حرف *)cmd_ga.ga_data, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
		&si, &pi, عدم, عدم))
    {
	semsg(_(e_invalid_argument_str), cmd_ga.ga_data);
	اذهب done;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(hChildStdoutWr);
    hChildStdoutWr = INVALID_HANDLE_VALUE;
    אם (hChildStdinRd != INVALID_HANDLE_VALUE)
    {
	CloseHandle(hChildStdinRd);
	hChildStdinRd = INVALID_HANDLE_VALUE;
    }

    // Read output from child process.
    עבור (;;)
    {
	حرف	buf[4096];
	DWORD	n;

	אם (!ReadFile(hChildStdoutRd, buf, حجم(buf), &n, عدم) || n == 0)
	    اكسر;
	אם (ga_grow(&out_ga, (صحيح)n) == OK)
	{
	    mch_memmove((حرف *)out_ga.ga_data + out_ga.ga_len, buf, n);
	    out_ga.ga_len += (صحيح)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    set_vim_var_nr(VV_SHELL_ERROR, (long)exit_code);

    אם (out_ga.ga_len > 0)
    {
	buffer = alloc(out_ga.ga_len + 1);
	אם (buffer != عدم)
	{
	    mch_memmove(buffer, out_ga.ga_data, out_ga.ga_len);
	    אם (ret_len == عدم)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		עבור (i = 0; i < out_ga.ga_len; ++i)
		    אם (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[out_ga.ga_len] = NUL;
	    }
	    אחרת
		*ret_len = out_ga.ga_len;
	}
    }
    אחרת אם (ret_len != عدم)
	*ret_len = 0;

done:
    ga_clear(&cmd_ga);
    ga_clear(&out_ga);
    אם (hChildStdoutRd != INVALID_HANDLE_VALUE)
	CloseHandle(hChildStdoutRd);
    אם (hChildStdoutWr != INVALID_HANDLE_VALUE)
	CloseHandle(hChildStdoutWr);
    אם (hChildStdinRd != INVALID_HANDLE_VALUE)
	CloseHandle(hChildStdinRd);
    החזר buffer;
}
#endif // FEAT_EVAL

#אם defined(FEAT_JOB_CHANNEL)
    فراغ
mch_job_start(حرف *cmd, job_T *job, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_ATTRIBUTES saAttr;
    channel_T		*channel = عدم;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    صحيح		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    صحيح		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    صحيح		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    صحيح		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    صحيح		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    صحيح		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    صحيح		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;

    אם (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_VALUE;
    ifd[1] = INVALID_HANDLE_VALUE;
    ofd[0] = INVALID_HANDLE_VALUE;
    ofd[1] = INVALID_HANDLE_VALUE;
    efd[0] = INVALID_HANDLE_VALUE;
    efd[1] = INVALID_HANDLE_VALUE;
    ga_init2(&ga, حجم(wchar_t), 500);

    jo = CreateJobObject(عدم, عدم);
    אם (jo == عدم)
    {
	job->jv_status = JOB_FAILED;
	اذهب failed;
    }

    אם (options->jo_env != عدم)
	win32_build_env(options->jo_env, &ga, FALSE);

    ZeroMemory(&pi, حجم(pi));
    ZeroMemory(&si, حجم(si));
    si.cb = حجم(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    saAttr.nLength = حجم(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = عدم;

    אם (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	ifd[0] = job_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
	אם (ifd[0] == INVALID_HANDLE_VALUE)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    אחרת אם (!use_null_for_in
	    && (!create_pipe_pair(ifd)
		|| !SetHandleInformation(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	اذهب failed;

    אם (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	ofd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	אם (ofd[1] == INVALID_HANDLE_VALUE)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    אחרת אם (!use_null_for_out &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !SetHandleInformation(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	اذهب failed;

    אם (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	אם (efd[1] == INVALID_HANDLE_VALUE)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    אחרת אם (!use_out_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !SetHandleInformation(efd[0], HANDLE_FLAG_INHERIT, 0)))
	اذهب failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_out_for_err ? ofd[1] : efd[1];

    אם (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	אם (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    אם (channel != عدم)
		++channel->ch_refcount;
	}
	אחרת
	    channel = add_channel();
	אם (channel == عدم)
	    اذهب failed;
    }

    אם (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_MODE |
	    CREATE_NEW_PROCESS_GROUP |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_data,
	    (حرف *)options->jo_cwd))
    {
	CloseHandle(jo);
	job->jv_status = JOB_FAILED;
	اذهب failed;
    }

    ga_clear(&ga);

    אם (!AssignProcessToJobObject(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = عدم;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    job->jv_proc_info = pi;
    job->jv_job_object = jo;
    job->jv_status = JOB_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    אם (!use_out_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    job->jv_channel = channel;
    אם (channel != عدم)
    {
	channel_set_pipes(channel,
		      use_file_for_in || use_null_for_in
					      ? INVALID_FD : (sock_T)ifd[1],
		      use_file_for_out || use_null_for_out
					     ? INVALID_FD : (sock_T)ofd[0],
		      use_out_for_err || use_file_for_err || use_null_for_err
					    ? INVALID_FD : (sock_T)efd[0]);
	channel_set_job(channel, job, options);
    }
    החזר;

failed:
    CloseHandle(ifd[0]);
    CloseHandle(ofd[0]);
    CloseHandle(efd[0]);
    CloseHandle(ifd[1]);
    CloseHandle(ofd[1]);
    CloseHandle(efd[1]);
    channel_unref(channel);
    ga_clear(&ga);
}

    حرف *
mch_job_status(job_T *job)
{
    DWORD dwExitCode = 0;

    אם (!GetExitCodeProcess(job->jv_proc_info.hProcess, &dwExitCode)
	    || dwExitCode != STILL_ACTIVE)
    {
	job->jv_exitval = (صحيح)dwExitCode;
	אם (job->jv_status < JOB_ENDED)
	{
	    ch_log(job->jv_channel, "Job ended");
	    job->jv_status = JOB_ENDED;
	}
	החזר "dead";
    }
    החזר "run";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    HANDLE jobHandles[MAXIMUM_WAIT_OBJECTS];
    job_T *jobArray[MAXIMUM_WAIT_OBJECTS];
    job_T *job = job_list;

    כלעוד (job != عدم)
    {
	DWORD n;
	DWORD result;

	עבור (n = 0; n < MAXIMUM_WAIT_OBJECTS
				       && job != عدم; job = job->jv_next)
	{
	    אם (job->jv_status == JOB_STARTED)
	    {
		jobHandles[n] = job->jv_proc_info.hProcess;
		jobArray[n] = job;
		++n;
	    }
	}
	אם (n == 0)
	    استمر;
	result = WaitForMultipleObjects(n, jobHandles, FALSE, 0);
	אם (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + n)
	{
	    job_T *wait_job = jobArray[result - WAIT_OBJECT_0];

	    אם (STRCMP(mch_job_status(wait_job), "dead") == 0)
		החזר wait_job;
	}
    }
    החזר عدم;
}

    ساكن BOOL
terminate_all(HANDLE process, صحيح code)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_VALUE;
    DWORD	    pid = GetProcessId(process);

    אם (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	אם (h != INVALID_HANDLE_VALUE)
	{
	    pe.dwSize = حجم(PROCESSENTRY32);
	    אם (!Process32First(h, &pe))
		اذهب theend;

	    עשה
	    {
		אם (pe.th32ParentProcessID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    אם (ph != عدم)
		    {
			terminate_all(ph, code);
			CloseHandle(ph);
		    }
		}
	    } כלעוד (Process32Next(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    החזר TerminateProcess(process, code);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    صحيح
mch_signal_job(job_T *job, char_u *how)
{
    صحيح ret;

    אם (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	אם (job->jv_job_object != عدم)
	{
	    אם (job->jv_channel != عدم && job->jv_channel->ch_anonymous_pipe)
		job->jv_channel->ch_killing = TRUE;
	    החזר TerminateJobObject(job->jv_job_object, (UINT)-1) ? OK : FAIL;
	}
	החזר terminate_all(job->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    אם (!AttachConsole(job->jv_proc_info.dwProcessId))
	החזר FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		job->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    החזר ret;
}

/*
 * Clear the data related to "job".
 */
    فراغ
mch_clear_job(job_T *job)
{
    אם (job->jv_status == JOB_FAILED)
	החזר;

    אם (job->jv_job_object != عدم)
	CloseHandle(job->jv_job_object);
    CloseHandle(job->jv_proc_info.hProcess);
}
#endif


#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    ساكن فراغ
termcap_mode_start(فراغ)
{
    DWORD cmodein;

    אם (g_fTermcapMode)
	החזר;

    SaveConsoleBuffer(&g_cbNonTermcap);

    אם (g_cbTermcap.IsValid)
    {
	/*
	 * We've been in termcap mode before.  Restore certain screen
	 * characteristics, including the buffer size and the window
	 * size.  Since we will be redrawing the screen, we don't need
	 * to restore the actual contents of the buffer.
	 */
	RestoreConsoleBuffer(&g_cbTermcap, FALSE);
	reset_console_color_rgb();
	SetConsoleWindowInfo(g_hConOut, TRUE, &g_cbTermcap.Info.srWindow);
	Rows = g_cbTermcap.Info.dwSize.Y;
	Columns = g_cbTermcap.Info.dwSize.X;
    }
    אחרת
    {
	/*
	 * This is our first time entering termcap mode.  Clear the console
	 * screen buffer, and resize the buffer to match the current window
	 * size.  We will use this as the size of our editing environment.
	 */
	ClearConsoleBuffer(g_attrCurrent);
	set_console_color_rgb();
	ResizeConBufAndWindow(g_hConOut, Columns, Rows);
    }

    resettitle();

    GetConsoleMode(g_hConIn, &cmodein);
    אם (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_INPUT;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    אחרת
    {
	cmodein &= ~ENABLE_MOUSE_INPUT;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }
    cmodein |= ENABLE_WINDOW_INPUT;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_fTermcapMode = TRUE;
}


/*
 * End termcap mode
 */
    ساكن فراغ
termcap_mode_end(فراغ)
{
    DWORD cmodein;
    ConsoleBuffer *cb;
    COORD coord;
    DWORD dwDummy;

    אם (!g_fTermcapMode)
	החזר;

    SaveConsoleBuffer(&g_cbTermcap);

    GetConsoleMode(g_hConIn, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# אחרת
    cb = &g_cbNonTermcap;
# endif
    RestoreConsoleBuffer(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    אם (exiting && use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    אם (!USE_WT && (p_rs || exiting))
    {
	/*
	 * Clear anything that happens to be on the current line.
	 */
	coord.X = 0;
	coord.Y = (SHORT) (p_rs ? cb->Info.dwCursorPosition.Y : (Rows - 1));
	FillConsoleOutputCharacter(g_hConOut, ' ',
		cb->Info.dwSize.X, coord, &dwDummy);
	/*
	 * The following is just for aesthetics.  If we are exiting without
	 * restoring the screen, then we want to have a prompt string
	 * appear at the bottom line.  However, the command interpreter
	 * seems to always advance the cursor one line before displaying
	 * the prompt string, which causes the screen to scroll.  To
	 * counter this, move the cursor up one line before exiting.
	 */
	אם (exiting && !p_rs)
	    coord.Y--;
	/*
	 * Position the cursor at the leftmost column of the desired row.
	 */
	SetConsoleCursorPosition(g_hConOut, coord);
    }
    SetConsoleCursorInfo(g_hConOut, &g_cci);
    g_fTermcapMode = FALSE;
}
#endif


#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    ساكن فراغ
clear_chars(
    COORD coord,
    DWORD n)
{
    אם (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_hConOut, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    אחרת
    {
	set_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    ساكن فراغ
clear_screen(فراغ)
{
    g_coord.X = g_coord.Y = 0;

    אם (!vtp_working)
	clear_chars(g_coord, Rows * Columns);
    אחרת
    {
	set_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    ساكن فراغ
clear_to_end_of_display(فراغ)
{
    COORD save = g_coord;

    אם (!vtp_working)
	clear_chars(g_coord, (Rows - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    אחרת
    {
	set_console_color_rgb();
	gotoxy(g_coord.X + 1, g_coord.Y + 1);
	vtp_printf("\033[0J");

	gotoxy(save.X + 1, save.Y + 1);
	g_coord = save;
    }
}


/*
 * Clear to end of line
 */
    ساكن فراغ
clear_to_end_of_line(فراغ)
{
    COORD save = g_coord;

    אם (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    אחרת
    {
	set_console_color_rgb();
	gotoxy(g_coord.X + 1, g_coord.Y + 1);
	vtp_printf("\033[0K");

	gotoxy(save.X + 1, save.Y + 1);
	g_coord = save;
    }
}


/*
 * Scroll the scroll region up by `cLines' lines
 */
    ساكن فراغ
scroll(unsigned cLines)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.Left + 1, g_srScrollRegion.Top + 1);
    delete_lines(cLines);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    ساكن فراغ
set_scroll_region(
    unsigned left,
    unsigned top,
    unsigned right,
    unsigned bottom)
{
    אם (left >= right
	    || top >= bottom
	    || right > (unsigned) Columns - 1
	    || bottom > (unsigned) Rows - 1)
	החזר;

    g_srScrollRegion.Left =   left;
    g_srScrollRegion.Top =    top;
    g_srScrollRegion.Right =  right;
    g_srScrollRegion.Bottom = bottom;
}

    ساكن فراغ
set_scroll_region_tb(
    unsigned top,
    unsigned bottom)
{
    אם (top >= bottom || bottom > (unsigned)Rows - 1)
	החזר;

    g_srScrollRegion.Top = top;
    g_srScrollRegion.Bottom = bottom;
}

    ساكن فراغ
set_scroll_region_lr(
    unsigned left,
    unsigned right)
{
    אם (left >= right || right > (unsigned)Columns - 1)
	החזר;

    g_srScrollRegion.Left = left;
    g_srScrollRegion.Right = right;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    ساكن فراغ
insert_lines(unsigned cLines)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.Left + 1, g_srScrollRegion.Top + 1);

    dest.X = g_srScrollRegion.Left;
    dest.Y = g_coord.Y + cLines;

    source.Left   = g_srScrollRegion.Left;
    source.Top	  = g_coord.Y;
    source.Right  = g_srScrollRegion.Right;
    source.Bottom = g_srScrollRegion.Bottom - cLines;

    clip.Left   = g_srScrollRegion.Left;
    clip.Top    = g_coord.Y;
    clip.Right  = g_srScrollRegion.Right;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    אם (!USE_VTP)
	fill.Attributes = g_attrCurrent;
    אחרת
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    אם (source.Bottom < dest.Y)
    {
	COORD coord;
	صحيح   i;

	coord.X = source.Left;
	עבור (i = clip.Top; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.Right - source.Left + 1);
	}
    }

    אם (vtp_working)
    {
	COORD coord;
	صحيح i;

	coord.X = source.Left;
	עבור (i = source.Top; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.Right - source.Left + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    ساكن فراغ
delete_lines(unsigned cLines)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    صحيح		    nb;

    gotoxy(g_srScrollRegion.Left + 1, g_srScrollRegion.Top + 1);

    dest.X = g_srScrollRegion.Left;
    dest.Y = g_coord.Y;

    source.Left   = g_srScrollRegion.Left;
    source.Top	  = g_coord.Y + cLines;
    source.Right  = g_srScrollRegion.Right;
    source.Bottom = g_srScrollRegion.Bottom;

    clip.Left   = g_srScrollRegion.Left;
    clip.Top    = g_coord.Y;
    clip.Right  = g_srScrollRegion.Right;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    אם (!vtp_working)
	fill.Attributes = g_attrCurrent;
    אחרת
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (source.Bottom - source.Top) + 1;

    אם (nb < source.Top)
    {
	COORD coord;
	صحيح   i;

	coord.X = source.Left;
	עבור (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.Right - source.Left + 1);
	}
    }

    אם (vtp_working)
    {
	COORD coord;
	صحيح i;

	coord.X = source.Left;
	עבור (i = nb; i <= source.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.Right - source.Left + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    ساكن فراغ
gotoxy(
    unsigned x,
    unsigned y)
{
    אם (x < 1 || x > (unsigned)Columns || y < 1 || y > (unsigned)Rows)
	החזר;

    אם (!USE_VTP)
    {
	// There are reports of double-width characters not displayed
	// correctly.  This workaround should fix it, similar to how it's done
	// for VTP.
	g_coord.X = 0;
	SetConsoleCursorPosition(g_hConOut, g_coord);

	// external cursor coords are 1-based; internal are 0-based
	g_coord.X = x - 1;
	g_coord.Y = y - 1;
	SetConsoleCursorPosition(g_hConOut, g_coord);
    }
    אחרת
    {
	// Move the cursor to the left edge of the screen to prevent screen
	// destruction.  Insider build bug.  Always enabled because it's cheap
	// and avoids mistakes with recognizing the build.
	vtp_printf("\033[%d;%dH", g_coord.Y + 1, 1);

	vtp_printf("\033[%d;%dH", y, x);

	g_coord.X = x - 1;
	g_coord.Y = y - 1;
    }
}


/*
 * Set the current text attribute = (foreground | background)
 * See ../runtime/doc/os_win32.txt for the numbers.
 */
    ساكن فراغ
textattr(WORD wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    SetConsoleTextAttribute(g_hConOut, wAttr);
}


    ساكن فراغ
textcolor(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    אם (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    אחרת
	vtp_sgr_bulk(wAttr);
}


    ساكن فراغ
textbackground(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    אם (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    אחרת
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    ساكن فراغ
normvideo(فراغ)
{
    אם (!vtp_working)
	textattr(g_attrDefault);
    אחרת
	vtp_sgr_bulk(0);
}


ساكن WORD g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    ساكن فراغ
standout(فراغ)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((WORD) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    ساكن فراغ
standend(فراغ)
{
    אם (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    فراغ
mch_set_normal_colors(فراغ)
{
    char_u	*p;
    صحيح		n;

    cterm_normal_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_normal_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    אם (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	אם (*p == 'm' && n > 0)
	{
	    cterm_normal_fg_color = (n & 0xf) + 1;
	    cterm_normal_bg_color = ((n >> 4) & 0xf) + 1;
	}
    }
# ifdef FEAT_TERMGUICOLORS
    cterm_normal_fg_gui_color = INVALCOLOR;
    cterm_normal_bg_gui_color = INVALCOLOR;
# endif
}


/*
 * visual bell: flash the screen
 */
    ساكن فراغ
visual_bell(فراغ)
{
    COORD   coordOrigin = {0, 0};
    WORD    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = عدم;

# ifdef FEAT_TERMGUICOLORS
    אם (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(WORD, Rows * Columns);
	אם (oldattrs == عدم)
	    החזר;
	ReadConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_hConOut, attrFlash, Rows * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    אם (oldattrs != عدم)
    {
	WriteConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    ساكن فراغ
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    אם (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	החזר;
    }

# ifdef MCH_CURSOR_SHAPE
    // Non-vtp Windows console: SetConsoleCursorInfo() consults
    // s_cursor_visible inside mch_set_cursor_shape(), so the call is needed
    // to apply the new visibility.
    mch_update_cursor();
# endif
}


/*
 * Write "cbToWrite" bytes in `pchBuf' to the screen.
 * Returns the number of bytes actually written (at least one).
 */
    ساكن DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cbToWrite)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    ساكن DWORD    cells;
    ساكن WCHAR    *unicodebuf = عدم;
    ساكن صحيح	    unibuflen = 0;
    ساكن صحيح	    length;
    صحيح		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    ساكن WCHAR    *utf8spbuf = عدم;
    ساكن صحيح	    utf8splength;
    ساكن DWORD    utf8spcells;
    ساكن WCHAR    **utf8usingbuf = &unicodebuf;

    אם (cbToWrite != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	עשה
	{
	    length = MultiByteToWideChar(cp, 0, (LPCSTR)pchBuf, cbToWrite,
							unicodebuf, unibuflen);
	    אם (length && length <= unibuflen)
		اكسر;
	    vim_free(unicodebuf);
	    unicodebuf = length ? LALLOC_MULT(WCHAR, length) : عدم;
	    unibuflen = unibuflen ? 0 : length;
	} כלעוד (TRUE);
	cells = mb_string2cells(pchBuf, cbToWrite);
    }
    אחרת // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	אם (utf8usingbuf != &utf8spbuf)
	{
	    אם (utf8spbuf == عدم)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		length = MultiByteToWideChar(CP_UTF8, 0, " ", 1, عدم, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, length);
		אם (utf8spbuf != عدم)
		{
		    MultiByteToWideChar(CP_UTF8, 0, " ", 1, utf8spbuf, length);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = length;
		    utf8spcells = cells;
		}
	    }
	    אחרת
	    {
		utf8usingbuf = &utf8spbuf;
		length = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    אם (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	אם (WriteConsoleOutputCharacterW(g_hConOut, *utf8usingbuf, length,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    אחרת
    {
	אם (WriteConsoleW(g_hConOut, *utf8usingbuf, length, &cchwritten,
		    عدم) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    אם (cchwritten == (DWORD)length)
    {
	written = cbToWrite;
	g_coord.X += (SHORT)cells;
    }
    אחרת
    {
	char_u *p = pchBuf;
	עבור (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    כלעוד (g_coord.X > g_srScrollRegion.Right)
    {
	g_coord.X -= (SHORT) Columns;
	אם (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    אם (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    החזר written;
}

    ساكن char_u *
get_seq(
    صحيح *args,
    صحيح *count,
    char_u *head)
{
    صحيح argc;
    char_u *p;

    אם (head == عدم || *head != '\033')
	החזר عدم;

    argc = 0;
    p = head;
    ++p;
    עשה
    {
	++p;
	args[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } כלעוד (*p == ';');
    *count = argc;

    החזר p;
}

    ساكن char_u *
get_sgr(
    صحيح *args,
    صحيح *count,
    char_u *head)
{
    char_u *p = get_seq(args, count, head);

    החזר (p && *p == 'm') ? ++p : عدم;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    ساكن char_u *
sgrn2(
    char_u *head,
    صحيح	   n)
{
    صحيح argc;
    صحيح args[16];
    char_u *p = get_sgr(args, &argc, head);

    החזר p && argc == 5 && args[0] == n && args[1] == 2 ? p : عدم;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    ساكن char_u *
sgrnc(
    char_u *head,
    صحيح	   n)
{
    صحيح argc;
    صحيح args[16];
    char_u *p = get_sgr(args, &argc, head);

    החזר p && argc == 1 && args[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : عدم;
}

    ساكن char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    כלעוד (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    החזר p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    ساكن char_u *
sgrn2c(
    char_u *head,
    صحيح	   n)
{
    char_u *p = sgrn2(head, n);

    החזר p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : عدم;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    ساكن char_u *
sgrn2cn(
    char_u *head,
    صحيح n)
{
    char_u *p = sgrn2(head, n);

    החזר p && p[0] == 0x0a && p[1] == '\033' ? ++p : عدم;
}

ساكن bool in_dcs = false;	// inside a DCS (sixel) sequence
ساكن bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    ساكن فراغ
write_vt(char_u *s, صحيح len)
{
    صحيح		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, عدم, 0);
    אם (wlen <= 0)
	החזר;
    ws = ALLOC_MULT(WCHAR, wlen);
    אם (ws == عدم)
	החזר;
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_hConOut, ws, wlen, &written, عدم);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    ساكن char_u *
skip_dcs(char_u *s, char_u *end)
{
    עבור (char_u *p = s; p < end; ++p)
    {
	אם (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    in_dcs = false;
	    החזר p + 1;
	}
	dcs_esc = *p == ESC;
    }
    החזר end;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    فراغ
mch_write(
    char_u  *s,
    صحيح	    len)
{
    char_u  *end = s + len;

# ifdef VIMDLL
    אם (gui.in_use)
	החזר;
# endif

    אם (!term_console)
    {
	اكتب(1, s, (unsigned)len);
	החזר;
    }

    // translate ESC | sequences into faked bios calls
    כלעוד (len--)
    {
	صحيح prefix = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	אם (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    החזר;
	}

	אם (in_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    صحيح	l = (صحيح)(skip_dcs(s, end) - s);

	    אם (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    استمر;
	}

	כלעוד (s + ++prefix < end)
	{
	    ch = s[prefix];
	    אם (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		اكسر;
	}

	אם (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    אם (prefix != 0)
		prefix = 1;
	}

	אם (prefix != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, prefix);
# ifdef MCH_WRITE_DUMP
	    אם (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, حجم(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	אחרת אם (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    אם (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.Left + 1, g_srScrollRegion.Bottom + 1);
	    }
	    אחרת
	    {
		gotoxy(g_srScrollRegion.Left + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    אם (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	אחרת אם (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.Left+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    אם (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	אחרת אם (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    אם (g_coord.X > g_srScrollRegion.Left)
		g_coord.X--;
	    אחרת אם (g_coord.Y > g_srScrollRegion.Top)
	    {
		g_coord.X = g_srScrollRegion.Right;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    אם (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	אחרת אם (s[0] == '\a')
	{
	    // \a, bell
	    MessageBeep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    אם (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	אחרת אם (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    صحيح	    arg1 = 0, arg2 = 0, argc = 0, args[16];
	    char_u  *sp;

	    بدل (s[2])
	    {
	    حالة '0': حالة '1': حالة '2': حالة '3': حالة '4':
	    حالة '5': حالة '6': حالة '7': حالة '8': حالة '9':
		אם (*(p = get_seq(args, &argc, s)) != 'm')
		    اذهب notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		אם (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (صحيح)(p - s);
		    s = p;
		    اكسر;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		אם (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		אם (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		אם (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		אם (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(args, &argc, p);

notsgr:
		arg1 = args[0];
		arg2 = args[1];
		אם (*p == 'm')
		{
		    אם (argc == 1 && args[0] == 0)
			normvideo();
		    אחרת אם (argc == 1)
		    {
			אם (USE_VTP)
			    textcolor((WORD)arg1);
			אחרת
			    textattr((WORD)arg1);
		    }
		    אחרת אם (vtp_working)
			vtp_sgr_bulks(argc, args);
		}
		אחרת אם (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		אחרת אם (argc == 2 && *p == 'r')
		{
		    set_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		אחרת אם (argc == 2 && *p == 'R')
		{
		    set_scroll_region_tb(arg1, arg2);
		}
		אחרת אם (argc == 2 && *p == 'V')
		{
		    set_scroll_region_lr(arg1, arg2);
		}
		אחרת אם (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.Top, g_coord.Y - arg1) + 1);
		}
		אחרת אם (argc == 1 && *p == 'b')
		{
		    textbackground((WORD) arg1);
		}
		אחרת אם (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.Right, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		אחרת אם (argc == 1 && *p == 'f')
		{
		    textcolor((WORD) arg1);
		}
		אחרת אם (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		אחרת אם (argc == 1 && *p == 'L')
		{
		    insert_lines(arg1);
		}
		אחרת אם (argc == 1 && *p == 'M')
		{
		    delete_lines(arg1);
		}

		len -= (صحيح)(p - s);
		s = p + 1;
		اكسر;

	    حالة 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.Top, g_coord.Y - 1) + 1);
		اذهب got3;

	    حالة 'B':
		visual_bell();
		اذهب got3;

	    حالة 'C':
		gotoxy(min(g_srScrollRegion.Right, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		اذهب got3;

	    حالة 'E':
		termcap_mode_end();
		اذهب got3;

	    حالة 'F':
		standout();
		اذهب got3;

	    حالة 'f':
		standend();
		اذهب got3;

	    حالة 'H':
		gotoxy(1, 1);
		اذهب got3;

	    حالة 'j':
		clear_to_end_of_display();
		اذهب got3;

	    حالة 'J':
		clear_screen();
		اذهب got3;

	    حالة 'K':
		clear_to_end_of_line();
		اذهب got3;

	    حالة 'L':
		insert_lines(1);
		اذهب got3;

	    حالة 'M':
		delete_lines(1);
		اذهب got3;

	    حالة 'S':
		termcap_mode_start();
		اذهب got3;

	    حالة 'V':
		cursor_visible(TRUE);
		اذهب got3;

	    حالة 'v':
		cursor_visible(FALSE);
		اذهب got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    אם (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, حجم(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	אחרת אם (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    صحيح l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    כלעוד (s + l < end && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    אם (s + l < end && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		אם (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	אחרת אם (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    صحيح	l;

	    in_dcs = true;
	    dcs_esc = false;
	    l = (صحيح)(skip_dcs(s + 2, end) - s);
	    אם (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	אחרת
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    אם (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, حجم(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    אם (fdDump)
	fflush(fdDump);
# endif
}

#אחרת // FEAT_GUI_MSWIN && !VIMDLL
    فراغ
mch_write(
    char_u  *s UNUSED,
    صحيح	    len UNUSED)
{
    // never used
}

#endif


/*
 * Delay for "msec" milliseconds.
 */
    فراغ
mch_delay(
    long    msec,
    صحيح	    flags UNUSED)
{
#אם defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((صحيح)msec);	    // never wait for input
#אחרת // Console
# ifdef VIMDLL
    אם (gui.in_use)
    {
	Sleep((صحيح)msec);	    // never wait for input
	החזר;
    }
# endif
    אם (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	אם (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    صحيح towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    כלעוד (msec > 0)
	    {
		mzvim_check_threads();
		אם (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	אחרת
# endif
	    Sleep((صحيح)msec);
    אחרת
	WaitForChar(msec, FALSE);
#endif
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    صحيح
mch_remove(char_u *name)
{
    WCHAR	*wn;
    صحيح		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    אם (mch_isdir(name) && mch_is_symbolic_link(name))
	החזר mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_NORMAL);

    wn = enc_to_utf16(name, عدم);
    אם (wn == عدم)
	החזר -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    החזר n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    فراغ
mch_breakcheck(صحيح force UNUSED)
{
#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    אם (!gui.in_use)
# endif
	אם (g_fCtrlCPressed || g_fCBrkPressed)
	{
	    ctrl_break_was_pressed = g_fCBrkPressed;
	    g_fCtrlCPressed = g_fCBrkPressed = FALSE;
	    got_int = TRUE;
	}
#endif
}

// physical RAM to leave for the OS
#define WINNT_RESERVE_BYTES     (256*1024*1024)

/*
 * How much main memory in KiB that can be used by VIM.
 */
    long_u
mch_total_mem(صحيح special UNUSED)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dwLength = حجم(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&ms);
    אם (ms.ullAvailVirtual < ms.ullTotalPhys)
    {
	// Process address space fits in physical RAM, use all of it.
	החזר (long_u)(ms.ullAvailVirtual / 1024);
    }
    אם (ms.ullTotalPhys <= WINNT_RESERVE_BYTES)
    {
	// Catch old NT box or perverse hardware setup.
	החזר (long_u)((ms.ullTotalPhys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    החזר (long_u)((ms.ullTotalPhys - WINNT_RESERVE_BYTES) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    صحيح
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    صحيح		flags)		// EW_* flags
{
    החזר dos_expandpath(gap, path, 0, flags, FALSE);
}

/*
 * mch_wrename() works around a bug in rename (aka MoveFile) in
 * Windows, the bug can be demonstrated with the following scenario:
 * When renaming "abcdef~1.txt" to "abcdef~1.txt~", the short name can be
 * "abcdef~1.txt" again.
 *
 * Like rename(), returns 0 upon success, non-zero upon failure.
 * Should probably set errno appropriately when errors occur.
 */
    صحيح
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	szTempFile[_MAX_PATH + 1];
    WCHAR	szNewPath[_MAX_PATH + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    עבור (p = q = wold; *p != L'\0'; ++p)
    {
	بدل (*p)
	{
	حالة L'/':
	حالة L'\\':
	حالة L':':
	    אם (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    اكسر;

	افتراضي:
	    اكسر;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    אם ((صحيح)(p - q) < 8 || q[6] != L'~')
	החזר (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    אם (GetFullPathNameW(wnew, _MAX_PATH, szNewPath, &p) == 0 || p == عدم)
	החזר -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    אם (GetTempFileNameW(szNewPath, L"VIM", 0, szTempFile) == 0)
	החזר -2;

    // blow the temp file away
    אם (!DeleteFileW(szTempFile))
	החזר -3;

    // rename old file to the temp file
    אם (!MoveFileW(wold, szTempFile))
	החזר -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    אם ((hf = CreateFileW(wold, GENERIC_WRITE, 0, عدم, CREATE_NEW,
		    FILE_ATTRIBUTE_NORMAL, عدم)) == INVALID_HANDLE_VALUE)
	החזר -5;
    אם (!CloseHandle(hf))
	החזר -6;

    // rename the temp file to the new file
    אם (!MoveFileW(szTempFile, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(فراغ)MoveFileW(szTempFile, wold);
	החזר -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(szTempFile);

    // finally, remove the empty old file
    אם (!DeleteFileW(wold))
	החזר -8;

    החזר 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    صحيح
mch_rename(
    ثابت حرف	*pszOldFile,
    ثابت حرف	*pszNewFile)
{
    WCHAR	*wold = عدم;
    WCHAR	*wnew = عدم;
    صحيح		retval = -1;

    wold = enc_to_utf16((char_u *)pszOldFile, عدم);
    wnew = enc_to_utf16((char_u *)pszNewFile, عدم);
    אם (wold != عدم && wnew != عدم)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    החזר retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    حرف *
default_shell(فراغ)
{
    החזר "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    صحيح
mch_access(حرف *n, صحيح p)
{
    HANDLE	hFile;
    صحيح		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_to_utf16((char_u *)n, عدم);
    אם (wn == عدم)
	החזר -1;

    אם (mch_isdir((char_u *)n))
    {
	WCHAR TempNameW[_MAX_PATH + 16] = L"";

	אם (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    صحيح			i;
	    WIN32_FIND_DATAW    d;

	    עבור (i = 0; i < _MAX_PATH && wn[i] != 0; ++i)
		TempNameW[i] = wn[i];
	    אם (TempNameW[i - 1] != '\\' && TempNameW[i - 1] != '/')
		TempNameW[i++] = '\\';
	    TempNameW[i++] = '*';
	    TempNameW[i++] = 0;

	    hFile = FindFirstFileW(TempNameW, &d);
	    אם (hFile == INVALID_HANDLE_VALUE)
		اذهب getout;
	    אחרת
		(فراغ)FindClose(hFile);
	}

	אם (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    אם (!GetTempFileNameW(wn, L"VIM", 0, TempNameW))
		اذهب getout;
	    אחרת
		DeleteFileW(TempNameW);
	}
    }
    אחרת
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_mode = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_mode, share_mode,
					     عدم, OPEN_EXISTING, 0, عدم);
	אם (hFile == INVALID_HANDLE_VALUE)
	    اذهب getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    החזר retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    صحيح
mch_open(ثابت حرف *name, صحيح flags, صحيح mode)
{
    WCHAR	*wn;
    صحيح		f;

    wn = enc_to_utf16((char_u *)name, عدم);
    אם (wn == عدم)
	החזר -1;

    f = _wopen(wn, flags, mode);
    vim_free(wn);
    החזר f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(ثابت حرف *name, ثابت حرف *mode)
{
    WCHAR	*wn, *wm;
    FILE	*f = عدم;

#אם defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    حرف newMode = mode[strlen(mode) - 1];
    صحيح oldMode = 0;

    _get_fmode(&oldMode);
    אם (newMode == 't')
	_set_fmode(_O_TEXT);
    אחרת אם (newMode == 'b')
	_set_fmode(_O_BINARY);
#endif
    wn = enc_to_utf16((char_u *)name, عدم);
    wm = enc_to_utf16((char_u *)mode, عدم);
    אם (wn != عدم && wm != عدم)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#אם defined(DEBUG) && _MSC_VER >= 1400
    אם (oldMode != 0)
	_set_fmode(oldMode);
#endif
    החזר f;
}

/*
 * SUB STREAM (aka info stream) handling:
 *
 * NTFS can have sub streams for each file.  The normal contents of a file is
 * stored in the main stream, and extra contents (author information, title and
 * so on) can be stored in a sub stream.  After Windows 2000, the user can
 * access and store this information in sub streams via an explorer's property
 * menu item in the right click menu.  This information in sub streams was lost
 * when copying only the main stream.  Therefore we have to copy sub streams.
 *
 * Incomplete explanation:
 *	http://msdn.microsoft.com/library/en-us/dnw2k/html/ntfs5.asp
 * More useful info and an example:
 *	https://learn.microsoft.com/en-us/sysinternals/downloads/streams
 */

/*
 * Copy info stream data "substream".  Read from the file with BackupRead(sh)
 * and write to stream "substream" of file "to".
 * Errors are ignored.
 */
    ساكن فراغ
copy_substream(HANDLE sh, فراغ *context, WCHAR *to, WCHAR *substream, long len)
{
    HANDLE  hTo;
    WCHAR   *to_name;

    to_name = alloc((wcslen(to) + wcslen(substream) + 1) * حجم(WCHAR));
    אם (to_name == عدم)
	החזר;

    wcscpy(to_name, to);
    wcscat(to_name, substream);

    hTo = CreateFileW(to_name, GENERIC_WRITE, 0, عدم, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL, عدم);
    אם (hTo != INVALID_HANDLE_VALUE)
    {
	long	done;
	DWORD	todo;
	DWORD	readcnt, written;
	حرف	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	עבור (done = 0; done < len; done += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - done) > حجم(buf) ? حجم(buf)
						       : (size_t)(len - done));
	    אם (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(hTo, buf, todo, &written, عدم)
		    || written != todo)
		اكسر;
	}
	CloseHandle(hTo);
    }

    حرر(to_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    ساكن فراغ
copy_infostreams(char_u *from, char_u *to)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_STREAM_ID	sid;
    صحيح			headersize;
    WCHAR		streamname[_MAX_PATH];
    DWORD		readcount;
    فراغ		*context = عدم;
    DWORD		lo, hi;
    صحيح			len;

    // Convert the file names to wide characters.
    fromw = enc_to_utf16(from, عدم);
    tow = enc_to_utf16(to, عدم);
    אם (fromw != عدم && tow != عدم)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, عدم,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, عدم);
	אם (sh != INVALID_HANDLE_VALUE)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    עבור (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		ZeroMemory(&sid, حجم(WIN32_STREAM_ID));
		headersize = (صحيح)((حرف *)&sid.cStreamName - (حرف *)&sid.dwStreamId);
		אם (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    اكسر;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		אם (sid.dwStreamNameSize > 0)
		{
		    // Read the stream name.
		    אם (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dwStreamNameSize,
					  &readcount, FALSE, FALSE, &context))
			اكسر;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / حجم(WCHAR);
		    streamname[len] = 0;
		    אם (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (long)sid.Size.u.LowPart);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(فراغ)BackupSeek(sh, sid.Size.u.LowPart, sid.Size.u.HighPart,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (فراغ)BackupRead(sh, عدم, 0, &readcount, TRUE, FALSE, &context);

	    CloseHandle(sh);
	}
    }
    vim_free(fromw);
    vim_free(tow);
}

/*
 * ntdll.dll definitions
 */
#define FileEaInformation   7
#ifndef STATUS_SUCCESS
# define STATUS_SUCCESS	    ((NTSTATUS) 0x00000000L)
#endif

تعريفنوع بنية _FILE_FULL_EA_INFORMATION_ {
    ULONG  NextEntryOffset;
    UCHAR  Flags;
    UCHAR  EaNameLength;
    USHORT EaValueLength;
    CHAR   EaName[1];
} FILE_FULL_EA_INFORMATION_, *PFILE_FULL_EA_INFORMATION_;

تعريفنوع بنية _FILE_EA_INFORMATION_ {
    ULONG EaSize;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#ifndef PROTO
تعريفنوع NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions);
تعريفنوع NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
تعريفنوع NTSTATUS (NTAPI *PfnNtSetEaFile)(
	HANDLE		    FileHandle,
	PIO_STATUS_BLOCK    IoStatusBlock,
	PVOID		    Buffer,
	ULONG		    Length);
تعريفنوع NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	BOOLEAN ReturnSingleEntry,
	PVOID EaList,
	ULONG EaListLength,
	PULONG EaIndex,
	BOOLEAN RestartScan);
تعريفنوع NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_STATUS_BLOCK	IoStatusBlock,
	PVOID			FileInformation,
	ULONG			Length,
	FILE_INFORMATION_CLASS FileInformationClass);
تعريفنوع VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString);

PfnNtOpenFile pNtOpenFile = عدم;
PfnNtClose pNtClose = عدم;
PfnNtSetEaFile pNtSetEaFile = عدم;
PfnNtQueryEaFile pNtQueryEaFile = عدم;
PfnNtQueryInformationFile pNtQueryInformationFile = عدم;
PfnRtlInitUnicodeString pRtlInitUnicodeString = عدم;
#endif

/*
 * Load ntdll.dll functions.
 */
    ساكن BOOL
load_ntdll(فراغ)
{
    ساكن صحيح	loaded = -1;

    אם (loaded != -1)
	החזר (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    אם (hNtdll != عدم)
    {
	pNtOpenFile = (PfnNtOpenFile) GetProcAddress(hNtdll, "NtOpenFile");
	pNtClose = (PfnNtClose) GetProcAddress(hNtdll, "NtClose");
	pNtSetEaFile = (PfnNtSetEaFile)
	    GetProcAddress(hNtdll, "NtSetEaFile");
	pNtQueryEaFile = (PfnNtQueryEaFile)
	    GetProcAddress(hNtdll, "NtQueryEaFile");
	pNtQueryInformationFile = (PfnNtQueryInformationFile)
	    GetProcAddress(hNtdll, "NtQueryInformationFile");
	pRtlInitUnicodeString = (PfnRtlInitUnicodeString)
	    GetProcAddress(hNtdll, "RtlInitUnicodeString");
    }
    אם (pNtOpenFile == عدم
	    || pNtClose == عدم
	    || pNtSetEaFile == عدم
	    || pNtQueryEaFile == عدم
	    || pNtQueryInformationFile == عدم
	    || pRtlInitUnicodeString == عدم)
	loaded = FALSE;
    אחרת
	loaded = TRUE;
    החזר (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    ساكن فراغ
copy_extattr(char_u *from, char_u *to)
{
    char_u		    *fromf = عدم;
    char_u		    *tof = عدم;
    WCHAR		    *fromw = عدم;
    WCHAR		    *tow = عدم;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_ATTRIBUTES	    oa;
    IO_STATUS_BLOCK	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    فراغ		    *ea = عدم;

    אם (!load_ntdll())
	החזר;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(to) + 5);
    אם (fromf == عدم || tof == عدم)
	اذهب theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), to);

    // Convert the names to wide characters.
    fromw = enc_to_utf16(fromf, عدم);
    tow = enc_to_utf16(tof, عدم);
    אם (fromw == عدم || tow == عدم)
	اذهب theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    InitializeObjectAttributes(&oa, &u, 0, عدم, عدم);
    אם (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	اذهب theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, حجم(eainfo),
	    FileEaInformation);
    אם (eainfo.EaSize != 0)
    {
	ea = alloc(eainfo.EaSize);
	אם (ea != عدم)
	{
	    אם (pNtQueryEaFile(h, &iosb, ea, eainfo.EaSize, FALSE,
			عدم, 0, عدم, TRUE) != STATUS_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    אם (ea != عدم)
    {
	pRtlInitUnicodeString(&u, tow);
	InitializeObjectAttributes(&oa, &u, 0, عدم, عدم);
	אם (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	    اذهب theend;

	pNtSetEaFile(h, &iosb, ea, eainfo.EaSize);
	pNtClose(h);
    }

theend:
    vim_free(fromf);
    vim_free(tof);
    vim_free(fromw);
    vim_free(tow);
    vim_free(ea);
}

/*
 * Copy file attributes from file "from" to file "to".
 * For Windows NT and later we copy info streams.
 * Always returns zero, errors are ignored.
 */
    صحيح
mch_copy_file_attribute(char_u *from, char_u *to)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, to);
    copy_extattr(from, to);
    החזר 0;
}


/*
 * The command line arguments in UTF-16
 */
ساكن صحيح	nArgsW = 0;
ساكن LPWSTR	*ArglistW = عدم;
ساكن صحيح	global_argc = 0;
ساكن حرف	**global_argv;

ساكن صحيح	used_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
ساكن صحيح	*used_file_indexes = عدم; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
ساكن صحيح	used_file_count = 0;	// nr of entries in used_file_indexes
ساكن صحيح	used_file_literal = FALSE;  // take file names literally
ساكن صحيح	used_file_full_path = FALSE;  // file name was full path
#ifdef FEAT_DIFF
ساكن صحيح	used_file_diff_mode = FALSE;  // file name was with diff mode
#endif
ساكن صحيح	used_alist_count = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    صحيح
get_cmd_argsW(حرف ***argvp)
{
    حرف	**argv = عدم;
    صحيح		argc = 0;
    صحيح		i;

    free_cmd_argsW();
    ArglistW = CommandLineToArgvW(GetCommandLineW(), &nArgsW);
    אם (ArglistW != عدم)
    {
	argv = alloc((nArgsW + 1) * حجم(حرف *));
	אם (argv != عدم)
	{
	    argc = nArgsW;
	    argv[argc] = عدم;
	    עבור (i = 0; i < argc; ++i)
	    {
		صحيح	len;

		// Convert each Unicode argument to UTF-8.
		WideCharToMultiByte_alloc(CP_UTF8, 0,
				ArglistW[i], (صحيح)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		אם (argv[i] == عدم)
		{
		    // Out of memory, clear everything.
		    כלעוד (i > 0)
			حرر(argv[--i]);
		    حرر(argv);
		    argv = عدم;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    אם (argc > 0)
    {
	אם (used_file_indexes != عدم)
	    حرر(used_file_indexes);
	used_file_indexes = alloc(argc * حجم(صحيح));
    }

    אם (argvp != عدم)
	*argvp = argv;
    החזר argc;
}

    فراغ
free_cmd_argsW(فراغ)
{
    אם (ArglistW == عدم)
	החזר;

    GlobalFree(ArglistW);
    ArglistW = عدم;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    فراغ
used_file_arg(حرف *name, صحيح literal, صحيح full_path, صحيح diff_mode)
{
    صحيح		i;

    אם (used_file_indexes == عدم)
	החזר;
    עבור (i = used_file_argc + 1; i < global_argc; ++i)
	אם (STRCMP(global_argv[i], name) == 0)
	{
	    used_file_argc = i;
	    used_file_indexes[used_file_count++] = i;
	    اكسر;
	}
    used_file_literal = literal;
    used_file_full_path = full_path;
#ifdef FEAT_DIFF
    used_file_diff_mode = diff_mode;
#endif
}

/*
 * Remember the length of the argument list as it was.  If it changes then we
 * leave it alone when 'encoding' is set.
 */
    فراغ
set_alist_count(فراغ)
{
    used_alist_count = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    فراغ
fix_arg_enc(فراغ)
{
    صحيح		i;
    صحيح		idx;
    char_u	*str;
    صحيح		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    אם (global_argc != nArgsW
	    || ArglistW == عدم
	    || used_file_indexes == عدم
	    || used_file_count == 0
	    || used_alist_count != GARGCOUNT)
	החזר;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(صحيح, GARGCOUNT);
    אם (fnum_list == عدم)
	החזר;		// out of memory
    עבור (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    אם (ga_grow(&global_alist.al_ga, used_file_count) == FAIL)
	החזר;		// out of memory

    עבור (i = 0; i < used_file_count; ++i)
    {
	idx = used_file_indexes[i];
	str = utf16_to_enc(ArglistW[idx], عدم);
	אם (str != عدم)
	{
	    صحيح literal = used_file_literal;

#ifdef FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    אם (used_file_diff_mode && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), tail, STRLEN(tail), TRUE, &ret);
		אם (ret.string != عدم)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#endif
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    אם (used_file_literal)
		buf_set_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    אם (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		אם (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_add(&global_alist, str, literal ? 2 : 0);
	}
    }

    אם (!used_file_literal)
    {
	// Now expand wildcards in the arguments.
	// Temporarily add '(' and ')' to 'isfname'.  These are valid
	// filename characters but are excluded from 'isfname' to make
	// "gf" work on a file name in parentheses (e.g.: see vim.h).
	// Also, unset wildignore to not be influenced by this option.
	// The arguments specified in command-line should be kept even if
	// encoding options were changed.
	// Use :legacy so that it also works when in Vim9 script.
	do_cmdline_cmd((char_u *)":legacy let g:SaVe_ISF = &isf|set isf+=(,)");
	do_cmdline_cmd((char_u *)":legacy let g:SaVe_WIG = &wig|set wig=");
	alist_expand(fnum_list, used_alist_count);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    אם (curwin->w_arg_idx == 0 && curbuf->b_fname == عدم)
    {
	do_cmdline_cmd((char_u *)":rewind");
	אם (GARGCOUNT == 1 && used_file_full_path
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    last_chdir_reason = "drop";
    }

    set_alist_count();
}

    صحيح
mch_setenv(حرف *var, حرف *value, صحيح x UNUSED)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(value) + 2);
    אם (envbuf == عدم)
	החזר -1;

    sprintf((حرف *)envbuf, "%s=%s", var, value);

    p = enc_to_utf16(envbuf, عدم);

    vim_free(envbuf);
    אם (p == عدم)
	החזר -1;
    _wputenv(p);
#ifdef libintl_wputenv
    libintl_wputenv(p);
#endif
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    החזר 0;
}

/*
 * Support for 256 colors and 24-bit colors was added in Windows 10
 * version 1703 (Creators update).
 */
#define VTP_FIRST_SUPPORT_BUILD	    MAKE_VER(10, 0, 15063)

/*
 * Support for pseudo-console (ConPTY) was added in windows 10
 * version 1809 (October 2018 update).
 */
#define CONPTY_FIRST_SUPPORT_BUILD  MAKE_VER(10, 0, 17763)

/*
 * ConPTY differences between versions, need different logic.
 * version 1903 (May 2019 update).
 */
#define CONPTY_1903_BUILD	    MAKE_VER(10, 0, 18362)

/*
 * version 1909 (November 2019 update).
 */
#define CONPTY_1909_BUILD	    MAKE_VER(10, 0, 18363)

/*
 * Stay ahead of the next update, and when it's done, fix this.
 * version ? (2020 update, temporarily use the build number of insider preview)
 */
#define CONPTY_NEXT_UPDATE_BUILD    MAKE_VER(10, 0, 19587)

/*
 * Confirm until this version.  Also the logic changes.
 * insider preview.
 */
#define CONPTY_INSIDER_BUILD	    MAKE_VER(10, 0, 18995)

/*
 * Make conpty default on Windows 11
 */
#define CONPTY_STABLE_BUILD	    MAKE_VER(10, 0, 22000)
// Notes:
// Win 10 22H2 Final is build 19045, its conpty is widely used.
// Strangely, 19045 is newer but is a lower build number than the 2020 insider
// preview which had a build 19587.  And, not sure how stable that was?
// Win Server 2022 (May 10, 2022) is build 20348, its conpty is widely used.
// Win 11 starts from build 22000, even though the major version says 10!

    ساكن فراغ
vtp_flag_init(فراغ)
{
#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   mode;
    HANDLE  out;

# ifdef VIMDLL
    אם (!gui.in_use)
# endif
    {
	out = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_version >= VTP_FIRST_SUPPORT_BUILD) ? 1 : 0;
	GetConsoleMode(out, &mode);
	mode |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	אם (SetConsoleMode(out, mode) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_buffer = win_version >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#endif

    אם (win_version >= CONPTY_FIRST_SUPPORT_BUILD)
	conpty_working = 1;
    אם (win_version >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    אם (win_version <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    אם (win_version <= CONPTY_1909_BUILD)
	conpty_type = 2;
    אם (win_version <= CONPTY_1903_BUILD)
	conpty_type = 2;
    אם (win_version < CONPTY_FIRST_SUPPORT_BUILD)
	conpty_type = 1;

    אם (win_version >= CONPTY_NEXT_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ساكن فراغ
vtp_init(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = حجم(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
    save_console_bg_rgb = (guicolor_T)csbi.ColorTable[g_color_index_bg];
    save_console_fg_rgb = (guicolor_T)csbi.ColorTable[g_color_index_fg];
    store_console_bg_rgb = save_console_bg_rgb;
    store_console_fg_rgb = save_console_fg_rgb;

    COLORREF bg;
    bg = (COLORREF)csbi.ColorTable[g_color_index_bg];
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);
    default_console_color_bg = bg;

    COLORREF fg;
    fg = (COLORREF)csbi.ColorTable[g_color_index_fg];
    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    default_console_color_fg = fg;
# endif
    set_console_color_rgb();
}

    ساكن فراغ
vtp_exit(فراغ)
{
    restore_console_color_rgb();
}

    صحيح
vtp_printf(
    حرف *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   result;
    صحيح	    len;

    אם (silent_mode)
	החזר 0;

    va_start(list, format);
    len = vim_vsnprintf((حرف *)buf, 100, (حرف *)format, list);
    va_end(list);
    WriteConsoleA(g_hConOut, buf, (DWORD)len, &result, عدم);
    החזר (صحيح)result;
}

    ساكن فراغ
vtp_sgr_bulk(
    صحيح arg)
{
    صحيح args[1];

    args[0] = arg;
    vtp_sgr_bulks(1, args);
}

# define FAST256(x) \
    אם ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    حالة x: \
	FAST256(newargs[x - 1]);

    ساكن فراغ
vtp_sgr_bulks(
    صحيح argc,
    صحيح *args)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    صحيح	    in, out;
    صحيح	    newargs[16];
    ساكن صحيح sgrfgr = -1, sgrfgg, sgrfgb;
    ساكن صحيح sgrbgr = -1, sgrbgg, sgrbgb;

    אם (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	החזר;
    }

    in = out = 0;
    כלעוד (in < argc)
    {
	صحيح s = args[in];
	صحيح copylen = 1;

	אם (s == 38)
	{
	    אם (argc - in >= 5 && args[in + 1] == 2)
	    {
		אם (sgrfgr == args[in + 2] && sgrfgg == args[in + 3]
						     && sgrfgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		אחרת
		{
		    sgrfgr = args[in + 2];
		    sgrfgg = args[in + 3];
		    sgrfgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    אחרת אם (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	אחרת אם (s == 48)
	{
	    אם (argc - in >= 5 && args[in + 1] == 2)
	    {
		אם (sgrbgr == args[in + 2] && sgrbgg == args[in + 3]
						     && sgrbgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		אחרת
		{
		    sgrbgr = args[in + 2];
		    sgrbgg = args[in + 3];
		    sgrbgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    אחרת אם (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	אחרת אם (30 <= s && s <= 39)
	    sgrfgr = -1;
	אחרת אם (90 <= s && s <= 97)
	    sgrfgr = -1;
	אחרת אם (40 <= s && s <= 49)
	    sgrbgr = -1;
	אחרת אם (100 <= s && s <= 107)
	    sgrbgr = -1;
	אחרת אם (s == 0)
	    sgrfgr = sgrbgr = -1;

	כלעוד (copylen--)
	    newargs[out++] = args[in++];
    }

    p = &buf[حجم(buf) - 1];
    *p-- = 'm';

    بدل (out)
    {
	صحيح	n, m;
	DWORD	r;

	FAST256CASE(16);
	*p-- = ';';
	FAST256CASE(15);
	*p-- = ';';
	FAST256CASE(14);
	*p-- = ';';
	FAST256CASE(13);
	*p-- = ';';
	FAST256CASE(12);
	*p-- = ';';
	FAST256CASE(11);
	*p-- = ';';
	FAST256CASE(10);
	*p-- = ';';
	FAST256CASE(9);
	*p-- = ';';
	FAST256CASE(8);
	*p-- = ';';
	FAST256CASE(7);
	*p-- = ';';
	FAST256CASE(6);
	*p-- = ';';
	FAST256CASE(5);
	*p-- = ';';
	FAST256CASE(4);
	*p-- = ';';
	FAST256CASE(3);
	*p-- = ';';
	FAST256CASE(2);
	*p-- = ';';
	FAST256CASE(1);
	*p-- = '[';
	*p = '\033';
	WriteConsoleA(g_hConOut, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, عدم);
    افتراضي:
	اكسر;
    }
}

    ساكن فراغ
wt_init(فراغ)
{
    wt_working = mch_getenv("WT_SESSION") != عدم;
}

# ifdef FEAT_TERMGUICOLORS
    ساكن صحيح
ctermtoxterm(
    صحيح cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    החזר (((صحيح)r << 16) | ((صحيح)g << 8) | (صحيح)b);
}
# endif

    ساكن فراغ
set_console_color_rgb(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    guicolor_T	fg, bg;
    صحيح		ctermfg, ctermbg;

    אם (!vtp_working)
	החזר;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    אם (p_tgc || t_colors >= 256)
    {
	term_fg_rgb_color(fg);
	term_bg_rgb_color(bg);
	החזר;
    }

    אם (use_alternate_screen_buffer)
	החזר;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cbSize = حجم(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = حجم(csbi);
    csbi.srWindow.Right += 1;
    csbi.srWindow.Bottom += 1;
    store_console_bg_rgb = csbi.ColorTable[g_color_index_bg];
    store_console_fg_rgb = csbi.ColorTable[g_color_index_fg];
    csbi.ColorTable[g_color_index_bg] = (COLORREF)bg;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)fg;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

# אם defined(FEAT_TERMGUICOLORS)
    فراغ
get_default_console_color(
    صحيح *cterm_fg,
    صحيح *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    صحيح id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    صحيح ctermfg = 0;
    صحيح ctermbg = 0;
    صحيح dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    אם (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    אם (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	אם (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	אם (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	אחרת
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_normal_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    אם (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	אם (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	אם (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	אחרת
	    guibg = USE_WT ? INVALCOLOR : default_console_color_bg;
	cterm_normal_bg_gui_color = guibg;
	ctermbg = ctermbg < 0 ? 0 : ctermbg;
    }

    *cterm_fg = ctermfg;
    *cterm_bg = ctermbg;
    *gui_fg = guifg;
    *gui_bg = guibg;
}
# endif

/*
 * Set the console colors to the original colors or the last set colors.
 */
    ساكن فراغ
reset_console_color_rgb(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    אם (use_alternate_screen_buffer)
	החזר;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = حجم(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = حجم(csbi);
    csbi.srWindow.Right += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)store_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)store_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    ساكن فراغ
restore_console_color_rgb(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    אם (use_alternate_screen_buffer)
	החזר;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = حجم(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = حجم(csbi);
    csbi.srWindow.Right += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)save_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)save_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

    فراغ
control_console_color_rgb(فراغ)
{
    אם (vtp_working)
	set_console_color_rgb();
    אחרת
	reset_console_color_rgb();
}

    صحيح
use_vtp(فراغ)
{
    החזר USE_VTP;
}

    صحيح
is_term_win32(فراغ)
{
    החזר T_NAME != عدم && STRCMP(T_NAME, "win32") == 0;
}

    صحيح
has_vtp_working(فراغ)
{
    החזר vtp_working;
}

#endif

    صحيح
has_conpty_working(فراغ)
{
    החזר conpty_working;
}

    صحيح
get_conpty_type(فراغ)
{
    החזר conpty_type;
}

    صحيح
is_conpty_stable(فراغ)
{
    החזר conpty_stable;
}

    صحيح
get_conpty_fix_type(فراغ)
{
    החזר conpty_fix_type;
}

#אם !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    فراغ
resize_console_buf(فراغ)
{
    אם (use_alternate_screen_buffer)
	החזר;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    אם (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	החזר;

    coord.X = SRWIDTH(csbi.srWindow);
    coord.Y = SRHEIGHT(csbi.srWindow);
    SetConsoleScreenBufferSize(g_hConOut, coord);

    newsize.Left = 0;
    newsize.Top = 0;
    newsize.Right = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    SetConsoleWindowInfo(g_hConOut, TRUE, &newsize);

    SetConsoleScreenBufferSize(g_hConOut, coord);
}
#endif

    حرف *
GetWin32Error(فراغ)
{
    ساكن حرف	*oldmsg = عدم;
    حرف	*acp_msg = عدم;
    DWORD	acp_len;
    حرف	*enc_msg = عدم;
    صحيح		enc_len = 0;

    // get formatted message from OS
    acp_len = FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
	    عدم, GetLastError(), 0, (LPSTR)&acp_msg, 0, عدم);
    אם (acp_len == 0 || acp_msg == عدم)
	החזר عدم;

    // clean oldmsg if remained.
    אם (oldmsg != عدم)
    {
	vim_free(oldmsg);
	oldmsg = عدم;
    }

    acp_to_enc((char_u *)acp_msg, (صحيح)acp_len, (char_u **)&enc_msg, &enc_len);
    LocalFree(acp_msg);
    אם (enc_msg == عدم)
	החזר عدم;

    // remove trailing \r\n
    حرف *pcrlf = strstr(enc_msg, "\r\n");
    אם (pcrlf != عدم)
	*pcrlf = NUL;

    oldmsg = enc_msg;
    החזר enc_msg;
}

#אם defined(FEAT_RELTIME)
ساكن HANDLE   timer_handle;
ساكن صحيح      timer_active = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
ساكن sig_atomic_t timeout_flags[2];
ساكن صحيح	    timeout_flag_idx = 0;
ساكن sig_atomic_t *timeout_flag = &timeout_flags[0];


    ساكن فراغ CALLBACK
set_flag(فراغ *param, BOOLEAN unused2 UNUSED)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    فراغ
stop_timeout(فراغ)
{
    אם (timer_active)
    {
	BOOL ret = DeleteTimerQueueTimer(عدم, timer_handle, عدم);
	timer_active = FALSE;
	אם (!ret && GetLastError() != ERROR_IO_PENDING)
	{
	    semsg(_(e_could_not_clear_timeout_str), GetWin32Error());
	}
    }
    *timeout_flag = FALSE;
}

/*
 * Start the timeout timer.
 *
 * The period is defined in milliseconds.
 *
 * The return value is a pointer to a flag that is initialised to 0.  If the
 * timeout expires, the flag is set to 1. This will only return pointers to
 * static memory; i.e. any pointer returned by this function may always be
 * safely dereferenced.
 *
 * This function is not expected to fail, but if it does it still returns a
 * valid flag pointer; the flag will remain stuck at zero.
 */
    متطاير sig_atomic_t *
start_timeout(long msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = CreateTimerQueueTimer(
	    &timer_handle, عدم, set_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    אם (!ret)
    {
	semsg(_(e_could_not_set_timeout_str), GetWin32Error());
    }
    אחרת
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_active = TRUE;
	*timeout_flag = FALSE;
    }
    החזר timeout_flag;
}
#endif
