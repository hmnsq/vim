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

#include "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#include <sys/types.h>
#include <signal.h>
#include <limits.h>

#include <process.h>
#include <winternl.h>
#include <direct.h>

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#ifdef FEAT_JOB_CHANNEL
# include <tlhelp32.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = NULL;
#endif

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
static HANDLE g_hConIn  = INVALID_HANDLE_VALUE;
static HANDLE g_hConOut = INVALID_HANDLE_VALUE;

// Win32 Screen buffer,coordinate,console I/O information
static SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
static COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
static WORD  g_attrDefault = 7;  // lightgray text on black background
static WORD  g_attrCurrent;

static puur_naa_nka g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
static puur_naa_nka g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
static puur_naa_nka g_fForceExit = FALSE;    // set when forcefully exiting

static void scroll(unsigned cLines);
static void set_scroll_region(unsigned left, unsigned top,
			      unsigned right, unsigned bottom);
static void set_scroll_region_tb(unsigned top, unsigned bottom);
static void set_scroll_region_lr(unsigned left, unsigned right);
static void insert_lines(unsigned cLines);
static void delete_lines(unsigned cLines);
static void gotoxy(unsigned x, unsigned y);
static void standout(void);
static puur_naa_nka s_cursor_visible = TRUE;
static puur_naa_nka did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
typedef sa_nracanaa input_record_buffer_node_S
{
    INPUT_RECORD ir;
    sa_nracanaa input_record_buffer_node_S *next;
} input_record_buffer_node_T;
typedef sa_nracanaa input_record_buffer_S
{
    input_record_buffer_node_T *head;
    input_record_buffer_node_T *tail;
    puur_naa_nka length;
} input_record_buffer_T;
static input_record_buffer_T input_record_buffer;
static puur_naa_nka read_input_record_buffer(INPUT_RECORD* irEvents, puur_naa_nka nMaxLength);
static puur_naa_nka write_input_record_buffer(INPUT_RECORD* irEvents, puur_naa_nka nLength);
#endif
#ifdef FEAT_GUI_MSWIN
static puur_naa_nka s_dont_use_vimrun = TRUE;
static puur_naa_nka need_vimrun_warning = FALSE;
static string_T vimrun_path = {(char_u *)"vimrun ", 7};
static puur_naa_nka vimrun_path_allocated = FALSE;
#endif

static puur_naa_nka win32_getattrs(char_u *name);
static puur_naa_nka win32_setattrs(char_u *name, puur_naa_nka attrs);
static puur_naa_nka win32_set_archive(char_u *name);

static puur_naa_nka conpty_working = 0;
static puur_naa_nka conpty_type = 0;
static puur_naa_nka conpty_stable = 0;
static puur_naa_nka conpty_fix_type = 0;
static void vtp_flag_init(void);

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
static puur_naa_nka vtp_working = 0;
static void vtp_init(void);
static void vtp_exit(void);
static void vtp_sgr_bulk(puur_naa_nka arg);
static void vtp_sgr_bulks(puur_naa_nka argc, puur_naa_nka *argv);

static puur_naa_nka wt_working = 0;
static void wt_init(void);

static puur_naa_nka g_color_index_bg = 0;
static puur_naa_nka g_color_index_fg = 7;

# ifdef FEAT_TERMGUICOLORS
static guicolor_T save_console_bg_rgb;
static guicolor_T save_console_fg_rgb;
static guicolor_T store_console_bg_rgb;
static guicolor_T store_console_fg_rgb;
static puur_naa_nka default_console_color_bg = 0x000000; // black
static puur_naa_nka default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_term_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# ᱵᱟᱝ
#  define USE_VTP		0
#  define USE_WT		0
# endif

static void set_console_color_rgb(void);
static void reset_console_color_rgb(void);
static void restore_console_color_rgb(void);
#endif  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
static puur_naa_nka suppress_winsize = 1;	// don't fiddle with console
#endif

static WCHAR *exe_pathw = NULL;

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
static BOOL use_alternate_screen_buffer = FALSE;
#endif

extern DWORD win_version; // this is in os_mswin.c

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    static BOOL
is_ambiwidth_event(
    INPUT_RECORD *ir)
{
    ᱨᱩᱣᱟᱹᱲ ir->EventType == KEY_EVENT
		&& ir->Event.KeyEvent.bKeyDown
		&& ir->Event.KeyEvent.wRepeatCount == 1
		&& ir->Event.KeyEvent.wVirtualKeyCode == 0x12
		&& ir->Event.KeyEvent.wVirtualScanCode == 0x38
		&& ir->Event.KeyEvent.uChar.UnicodeChar == 0
		&& ir->Event.KeyEvent.dwControlKeyState == 2;
}

    static void
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
    static BOOL
read_console_input(
    HANDLE	    hInput,
    INPUT_RECORD    *lpBuffer,
    puur_naa_nka		    nLength,
    LPDWORD	    lpEvents)
{
    ga_nanaa
    {
	IRSIZE = 10
    };
    static INPUT_RECORD s_irCache[IRSIZE];
    static DWORD s_dwIndex = 0;
    static DWORD s_dwMax = 0;
    DWORD dwEvents;
    puur_naa_nka head;
    puur_naa_nka tail;
    puur_naa_nka i;
    static INPUT_RECORD s_irPseudo;

    ᱡᱩᱫᱤ (s_dwMax == 0 && input_record_buffer.length > 0)
    {
	dwEvents = read_input_record_buffer(s_irCache, IRSIZE);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
    }

    ᱡᱩᱫᱤ (nLength == -2)
	ᱨᱩᱣᱟᱹᱲ (s_dwMax > 0) ? TRUE : FALSE;

    ᱡᱩᱫᱤ (win_version < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	ᱡᱩᱫᱤ (nLength == -1)
	    ᱨᱩᱣᱟᱹᱲ PeekConsoleInputW(hInput, lpBuffer, 1, lpEvents);
	ᱨᱩᱣᱟᱹᱲ ReadConsoleInputW(hInput, lpBuffer, 1, &dwEvents);
    }

    ᱡᱩᱫᱤ (s_dwMax == 0)
    {
	ᱡᱩᱫᱤ (!vtp_working && nLength == -1)
	    ᱨᱩᱣᱟᱹᱲ PeekConsoleInputW(hInput, lpBuffer, 1, lpEvents);
	GetNumberOfConsoleInputEvents(hInput, &dwEvents);
	ᱡᱩᱫᱤ (dwEvents == 0 && nLength == -1)
	    ᱨᱩᱣᱟᱹᱲ PeekConsoleInputW(hInput, lpBuffer, 1, lpEvents);
	ReadConsoleInputW(hInput, s_irCache, IRSIZE, &dwEvents);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
	ᱡᱩᱫᱤ (dwEvents == 0)
	{
	    *lpEvents = 0;
	    ᱨᱩᱣᱟᱹᱲ TRUE;
	}

	for (i = s_dwIndex; i < (puur_naa_nka)s_dwMax - 1; ++i)
	    ᱡᱩᱫᱤ (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	ᱡᱩᱫᱤ (s_dwMax > 1)
	{
	    head = 0;
	    tail = s_dwMax - 1;
	    ᱡᱚᱠᱷᱚᱱ (head != tail)
	    {
		ᱡᱩᱫᱤ (s_irCache[head].EventType == WINDOW_BUFFER_SIZE_EVENT
			&& s_irCache[head + 1].EventType
						  == WINDOW_BUFFER_SIZE_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    for (i = head; i < tail; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --tail;
		    continue;
		}
		head++;
	    }
	    s_dwMax = tail + 1;
	}
    }

    ᱡᱩᱫᱤ (s_irCache[s_dwIndex].EventType == KEY_EVENT)
    {
	ᱡᱩᱫᱤ (s_irCache[s_dwIndex].Event.KeyEvent.wRepeatCount > 1)
	{
	    s_irPseudo = s_irCache[s_dwIndex];
	    s_irPseudo.Event.KeyEvent.wRepeatCount = 1;
	    s_irCache[s_dwIndex].Event.KeyEvent.wRepeatCount--;
	    *lpBuffer = s_irPseudo;
	    *lpEvents = 1;
	    ᱨᱩᱣᱟᱹᱲ TRUE;
	}
    }

    *lpBuffer = s_irCache[s_dwIndex];
    ᱡᱩᱫᱤ (!(nLength == -1 || nLength == -2) && ++s_dwIndex >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    ᱨᱩᱣᱟᱹᱲ TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    static BOOL
peek_console_input(
    HANDLE	    hInput,
    INPUT_RECORD    *lpBuffer,
    DWORD	    nLength UNUSED,
    LPDWORD	    lpEvents)
{
    ᱨᱩᱣᱟᱹᱲ read_console_input(hInput, lpBuffer, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    static DWORD
msg_wait_for_multiple_objects(
    DWORD    nCount,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dwWakeMask)
{
    ᱡᱩᱫᱤ (read_console_input(NULL, NULL, -2, NULL))
	ᱨᱩᱣᱟᱹᱲ WAIT_OBJECT_0;
    ᱨᱩᱣᱟᱹᱲ MsgWaitForMultipleObjects(nCount, pHandles, fWaitAll,
				     dwMilliseconds, dwWakeMask);
}
# endif

# ifndef FEAT_CLIENTSERVER
    static DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    ᱡᱩᱫᱤ (read_console_input(NULL, NULL, -2, NULL))
	ᱨᱩᱣᱟᱹᱲ WAIT_OBJECT_0;
    ᱨᱩᱣᱟᱹᱲ WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#endif   // !FEAT_GUI_MSWIN || VIMDLL

    void
mch_get_exe_name(void)
{
    puur_naa_nka		updated = FALSE;
    static puur_naa_nka	enc_prev = -1;
    WCHAR	*path;
    size_t	pathlen;

    ᱡᱩᱫᱤ (exe_name == NULL || exe_pathw == NULL || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_PATH];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(NULL, buf, MAX_PATH);
	ᱡᱩᱫᱤ (p > buf)
	{
	    ᱡᱩᱫᱤ (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_to_enc(buf, NULL);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    do
	    {
		--p;
		ᱡᱩᱫᱤ (*p == L'\\')
		{
		    *p = L'\0';
		    bha_nga;
		}
	    } ᱡᱚᱠᱷᱚᱱ (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    ᱡᱩᱫᱤ (exe_pathw == NULL || !updated)
	ᱨᱩᱣᱟᱹᱲ;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    path = _wgetenv(L"PATH");
    pathlen = (path == NULL) ? 0 : wcslen(path);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#define EXTRA_LEN 7
#define MAX_ENV_STRING_LEN 8191

    ᱡᱩᱫᱤ (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   temp[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	ᱡᱩᱫᱤ (pathlen == 0)
	    wcscpy(temp + templen, exe_pathw);
	ᱵᱟᱝ
	{
	    wcscpy(temp + templen, path);

	    // Check if exe_path is already included in $PATH.
	    ᱡᱩᱫᱤ (wcsstr(temp, exe_pathw) == NULL)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		ᱡᱩᱫᱤ (temp[templen - 1] != L';')
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
    static void
unescape_shellxquote(char_u *p, char_u *escaped)
{
    puur_naa_nka	    l = (puur_naa_nka)STRLEN(p);
    puur_naa_nka	    n;

    ᱡᱚᱠᱷᱚᱱ (*p != NUL)
    {
	ᱡᱩᱫᱤ (*p == '^' && vim_strchr(escaped, p[1]) != NULL)
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
vimLoadLib(const var_na *name)
{
    HINSTANCE	dll = NULL;

    // No need to load any library when registering OLE.
    ᱡᱩᱫᱤ (found_register_arg)
	ᱨᱩᱣᱟᱹᱲ NULL;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    ᱡᱩᱫᱤ (exe_pathw == NULL)
    {
	mch_get_exe_name();
	ᱡᱩᱫᱤ (exe_pathw == NULL)
	    ᱨᱩᱣᱟᱹᱲ NULL;
    }

    WCHAR old_dirw[MAXPATHL];

    ᱡᱩᱫᱤ (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	ᱨᱩᱣᱟᱹᱲ NULL;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    SetCurrentDirectoryW(exe_pathw);
    dll = LoadLibrary(name);
    SetCurrentDirectoryW(old_dirw);
    ᱨᱩᱣᱟᱹᱲ dll;
}

#ᱡᱩᱫᱤ defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    puur_naa_nka
mch_is_gui_executable(void)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(NULL);
    PIMAGE_DOS_HEADER	pDOS = (PIMAGE_DOS_HEADER)pImage;
    PIMAGE_NT_HEADERS	pPE;

    ᱡᱩᱫᱤ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ᱨᱩᱣᱟᱹᱲ FALSE;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    ᱡᱩᱫᱤ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ᱨᱩᱣᱟᱹᱲ FALSE;
    ᱡᱩᱫᱤ (pPE->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	ᱨᱩᱣᱟᱹᱲ TRUE;
    ᱨᱩᱣᱟᱹᱲ FALSE;
}
#endif

#ᱡᱩᱫᱤ defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    static void *
get_imported_func_info(HINSTANCE hInst, const var_na *funcname, puur_naa_nka info,
	const void *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_HEADER		pDOS = (PIMAGE_DOS_HEADER)hInst;
    PIMAGE_NT_HEADERS		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_DATA		pIAT;	    // Import Address Table
    PIMAGE_THUNK_DATA		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    ᱡᱩᱫᱤ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ᱨᱩᱣᱟᱹᱲ NULL;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    ᱡᱩᱫᱤ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ᱨᱩᱣᱟᱹᱲ NULL;

    ImpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    ᱡᱩᱫᱤ (ImpVA == 0)
	ᱨᱩᱣᱟᱹᱲ NULL;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    for (; pImpDesc->FirstThunk; ++pImpDesc)
    {
	ᱡᱩᱫᱤ (!pImpDesc->OriginalFirstThunk)
	    continue;
	pIAT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->FirstThunk);
	pINT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->OriginalFirstThunk);
	for (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    ᱡᱩᱫᱤ (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		continue;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.AddressOfData));
	    ᱡᱩᱫᱤ (strcmp((var_na *)pImpName->Name, funcname) == 0)
	    {
		void *original;
		DWORD old, new = PAGE_READWRITE;

		switch (info)
		{
		    pak_sa 0:
			ᱨᱩᱣᱟᱹᱲ (void *)pIAT->u1.Function;
		    pak_sa 1:
			ᱨᱩᱣᱟᱹᱲ (void *)(pImage + pImpDesc->Name);
		    pak_sa 2:
			original = (void *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, parimaa_na(void *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, parimaa_na(void *),
				old, &new);
			ᱨᱩᱣᱟᱹᱲ original;
		    default:
			ᱨᱩᱣᱟᱹᱲ NULL;
		}
	    }
	}
    }
    ᱨᱩᱣᱟᱹᱲ NULL;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
find_imported_module_by_funcname(HINSTANCE hInst, const var_na *funcname)
{
    var_na    *modulename;

    modulename = (var_na *)get_imported_func_info(hInst, funcname, 1, NULL);
    ᱡᱩᱫᱤ (modulename != NULL)
	ᱨᱩᱣᱟᱹᱲ GetModuleHandleA(modulename);
    ᱨᱩᱣᱟᱹᱲ NULL;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    void *
get_dll_import_func(HINSTANCE hInst, const var_na *funcname)
{
    ᱨᱩᱣᱟᱹᱲ get_imported_func_info(hInst, funcname, 0, NULL);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    void *
hook_dll_import_func(HINSTANCE hInst, const var_na *funcname, const void *hook)
{
    ᱨᱩᱣᱟᱹᱲ get_imported_func_info(hInst, funcname, 2, hook);
}
#endif

#ᱡᱩᱫᱤ defined(FEAT_PYTHON3)
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

    ᱡᱩᱫᱤ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ᱨᱩᱣᱟᱹᱲ NULL;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    ᱡᱩᱫᱤ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ᱨᱩᱣᱟᱹᱲ NULL;

    ExpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    ExpSize = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    ᱡᱩᱫᱤ (ExpVA == 0)
	ᱨᱩᱣᱟᱹᱲ hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    pFunctionTable = (LPDWORD)(pImage + pExpDir->AddressOfFunctions);

    ᱡᱩᱫᱤ (pExpDir->NumberOfNames == 0)
	ᱨᱩᱣᱟᱹᱲ hInst;	// No export names.

    // Check only the first entry.
    ᱡᱩᱫᱤ ((pFunctionTable[0] < ExpVA) || (pFunctionTable[0] >= ExpVA + ExpSize))
	// The first entry is not a function forwarder.
	ᱨᱩᱣᱟᱹᱲ hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    const var_na *name = (const var_na *)(pImage + pFunctionTable[0]);
    const var_na *p = strchr(name, '.');
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ hInst;

    // Extract DllName.
    var_na buf[MAX_PATH];
    ᱡᱩᱫᱤ (p - name + 1 > parimaa_na(buf))
	ᱨᱩᱣᱟᱹᱲ NULL;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    ᱨᱩᱣᱟᱹᱲ GetModuleHandleA(buf);
}
#endif

#ᱡᱩᱫᱤ defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
static var_na *null_libintl_gettext(const var_na *);
static var_na *null_libintl_ngettext(const var_na *, const var_na *, unsigned long n);
static var_na *null_libintl_textdomain(const var_na *);
static var_na *null_libintl_bindtextdomain(const var_na *, const var_na *);
static var_na *null_libintl_bind_textdomain_codeset(const var_na *, const var_na *);
static puur_naa_nka null_libintl_wputenv(const wchar_t *);

static HINSTANCE hLibintlDLL = NULL;
var_na *(*dyn_libintl_gettext)(const var_na *) = null_libintl_gettext;
var_na *(*dyn_libintl_ngettext)(const var_na *, const var_na *, unsigned long n)
						= null_libintl_ngettext;
var_na *(*dyn_libintl_textdomain)(const var_na *) = null_libintl_textdomain;
var_na *(*dyn_libintl_bindtextdomain)(const var_na *, const var_na *)
						= null_libintl_bindtextdomain;
var_na *(*dyn_libintl_bind_textdomain_codeset)(const var_na *, const var_na *)
				       = null_libintl_bind_textdomain_codeset;
puur_naa_nka (*dyn_libintl_wputenv)(const wchar_t *) = null_libintl_wputenv;
puur_naa_nka *dyn_libintl_nl_msg_cat_cntr = NULL;

    puur_naa_nka
dyn_libintl_init(void)
{
    puur_naa_nka i;
    static sa_nracanaa
    {
	var_na	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{NULL, NULL}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    ᱡᱩᱫᱤ (hLibintlDLL != NULL)
	ᱨᱩᱣᱟᱹᱲ 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vimLoadLib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    ᱡᱩᱫᱤ (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    ᱡᱩᱫᱤ (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT2);
# endif
    ᱡᱩᱫᱤ (!hLibintlDLL)
    {
	ᱡᱩᱫᱤ (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_load_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	ᱨᱩᱣᱟᱹᱲ 0;
    }
    for (i = 0; libintl_entry[i].name != NULL
					 && libintl_entry[i].ptr != NULL; ++i)
    {
	ᱡᱩᱫᱤ ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == NULL)
	{
	    dyn_libintl_end();
	    ᱡᱩᱫᱤ (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_load_library_function_str), libintl_entry[i].name);
		verbose_leave();
	    }
	    ᱨᱩᱣᱟᱹᱲ 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (var_na *(*)(const var_na *, const var_na *))
			GetProcAddress(hLibintlDLL, "bind_textdomain_codeset");
    ᱡᱩᱫᱤ (dyn_libintl_bind_textdomain_codeset == NULL)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = find_imported_module_by_funcname(hLibintlDLL, "getenv");
    ᱡᱩᱫᱤ (hmsvcrt != NULL)
	dyn_libintl_wputenv = (puur_naa_nka (*)(const wchar_t *))
					GetProcAddress(hmsvcrt, "_wputenv");
    ᱡᱩᱫᱤ (dyn_libintl_wputenv == NULL || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_msg_cat_cntr = (puur_naa_nka *)GetProcAddress(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    ᱨᱩᱣᱟᱹᱲ 1;
}

    void
dyn_libintl_end(void)
{
    ᱡᱩᱫᱤ (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= NULL;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_msg_cat_cntr	= NULL;
}

    static var_na *
null_libintl_gettext(const var_na *msgid)
{
    ᱨᱩᱣᱟᱹᱲ (var_na*)msgid;
}

    static var_na *
null_libintl_ngettext(
	const var_na *msgid,
	const var_na *msgid_plural,
	unsigned long n)
{
    ᱨᱩᱣᱟᱹᱲ (var_na *)(n == 1 ? msgid : msgid_plural);
}

    static var_na *
null_libintl_bindtextdomain(
	const var_na *domainname UNUSED,
	const var_na *dirname UNUSED)
{
    ᱨᱩᱣᱟᱹᱲ NULL;
}

    static var_na *
null_libintl_bind_textdomain_codeset(
	const var_na *domainname UNUSED,
	const var_na *codeset UNUSED)
{
    ᱨᱩᱣᱟᱹᱲ NULL;
}

    static var_na *
null_libintl_textdomain(const var_na *domainname UNUSED)
{
    ᱨᱩᱣᱟᱹᱲ NULL;
}

    static puur_naa_nka
null_libintl_wputenv(const wchar_t *envstring UNUSED)
{
    ᱨᱩᱣᱟᱹᱲ 0;
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
    static BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    ᱡᱩᱫᱤ (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	ᱨᱩᱣᱟᱹᱲ FALSE;

    ᱡᱩᱫᱤ (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

    tokenPrivileges.PrivilegeCount	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    ᱡᱩᱫᱤ (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, NULL, NULL))
    {
	CloseHandle(hToken);
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

    ᱡᱩᱫᱤ (GetLastError() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

    CloseHandle(hToken);
    ᱨᱩᱣᱟᱹᱲ TRUE;
}
#endif

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    void
PlatformId(void)
{
#ifdef FEAT_EVAL
    DWORD major = (win_version >> 24) & 0xFF;
    DWORD minor = (win_version >> 16) & 0xFF;
    vim_snprintf(windowsVersion, parimaa_na(windowsVersion), "%d.%d", major, minor);
#endif

#ifdef HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    ᱡᱩᱫᱤ (!win32_enable_privilege(SE_SECURITY_NAME))
	ᱨᱩᱣᱟᱹᱲ;
#endif
}

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

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

static const sa_nracanaa
{
    WORD    wVirtKey;
    BOOL    fAnsiKey;
    puur_naa_nka	    chAlone;
    puur_naa_nka	    chShift;
    puur_naa_nka	    chCtrl;
    puur_naa_nka	    chAlt;
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

# ᱡᱩᱫᱤ 0
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
    static puur_naa_nka
win32_kbd_patch_key(
    KEY_EVENT_RECORD *pker)
{
    UINT uMods = pker->dwControlKeyState;
    static puur_naa_nka s_iIsDead = 0;
    static WORD awAnsiCode[2];
    static BYTE abKeystate[256];

    ᱡᱩᱫᱤ (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[1];
	s_iIsDead = 0;
	ᱨᱩᱣᱟᱹᱲ 1;
    }

    // check if it already has a valid unicode character.
    ᱡᱩᱫᱤ (pker->uChar.UnicodeChar != 0)
	ᱨᱩᱣᱟᱹᱲ 1;

    CLEAR_FIELD(abKeystate);

    // Clear any pending dead keys
    ToUnicode(VK_SPACE, MapVirtualKey(VK_SPACE, 0), abKeystate, awAnsiCode, 2, 0);

    ᱡᱩᱫᱤ (uMods & SHIFT_PRESSED)
	abKeystate[VK_SHIFT] = 0x80;
    ᱡᱩᱫᱤ (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    ᱡᱩᱫᱤ ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ToUnicode(pker->wVirtualKeyCode, pker->wVirtualScanCode,
			abKeystate, awAnsiCode, 2, 0);

    ᱡᱩᱫᱤ (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[0];

    ᱨᱩᱣᱟᱹᱲ s_iIsDead;
}

static BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    static BOOL
decode_key_event(
    KEY_EVENT_RECORD	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    puur_naa_nka			*pmodifiers,
    BOOL		fDoPost UNUSED)
{
    puur_naa_nka i;
    const puur_naa_nka nModifs = pker->dwControlKeyState & (SHIFT | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    ᱡᱩᱫᱤ (!pker->bKeyDown)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    // ignore some keystrokes
    switch (pker->wVirtualKeyCode)
    {
    // modifiers
    pak_sa VK_SHIFT:
    pak_sa VK_CONTROL:
    pak_sa VK_MENU:   // Alt key
	ᱨᱩᱣᱟᱹᱲ FALSE;

    default:
	bha_nga;
    }

    // Shift-TAB
    ᱡᱩᱫᱤ (pker->wVirtualKeyCode == VK_TAB && (nModifs & SHIFT_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	ᱨᱩᱣᱟᱹᱲ TRUE;
    }

    for (i = ARRAY_LENGTH(VirtKeyMap); --i >= 0; )
    {
	ᱡᱩᱫᱤ (VirtKeyMap[i].wVirtKey == pker->wVirtualKeyCode)
	{
	    *pch = VirtKeyMap[i].chAlone;
	    ᱡᱩᱫᱤ ((nModifs & SHIFT) != 0)
		*pch = VirtKeyMap[i].chShift;
	    ᱵᱟᱝ ᱡᱩᱫᱤ ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = VirtKeyMap[i].chCtrl;
	    ᱵᱟᱝ ᱡᱩᱫᱤ ((nModifs & ALT) != 0)
		*pch = VirtKeyMap[i].chAlt;

	    ᱡᱩᱫᱤ (*pch != 0)
	    {
		ᱡᱩᱫᱤ (VirtKeyMap[i].fAnsiKey)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    ᱡᱩᱫᱤ (pmodifiers)
		    {
			ᱡᱩᱫᱤ (pker->wVirtualKeyCode >= VK_F1
			    && pker->wVirtualKeyCode <= VK_F12)
			{
			    ᱡᱩᱫᱤ ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_MASK_ALT;
				ᱡᱩᱫᱤ ((nModifs & SHIFT) == 0)
				    *pch2 = VirtKeyMap[i].chAlone;
			    }
			    ᱡᱩᱫᱤ ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				ᱡᱩᱫᱤ ((nModifs & SHIFT) == 0)
				    *pch2 = VirtKeyMap[i].chAlone;
			    }
			}
			ᱵᱟᱝ ᱡᱩᱫᱤ (pker->wVirtualKeyCode >= VK_END
				&& pker->wVirtualKeyCode <= VK_DOWN)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = VirtKeyMap[i].chAlone;
			    ᱡᱩᱫᱤ ((nModifs & SHIFT) != 0
						    && (nModifs & ~SHIFT) == 0)
			    {
				*pch2 = VirtKeyMap[i].chShift;
			    }
			    ᱡᱩᱫᱤ ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = VirtKeyMap[i].chCtrl;
				ᱡᱩᱫᱤ (pker->wVirtualKeyCode == VK_UP
				    || pker->wVirtualKeyCode == VK_DOWN)
				{
				    *pmodifiers |= MOD_MASK_CTRL;
				    *pch2 = VirtKeyMap[i].chAlone;
				}
			    }
			    ᱡᱩᱫᱤ ((nModifs & SHIFT) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				*pch2 = VirtKeyMap[i].chShift;
			    }
			    ᱡᱩᱫᱤ ((nModifs & ALT) != 0)
			    {
				*pch2 = VirtKeyMap[i].chAlt;
				*pmodifiers |= MOD_MASK_ALT;
				ᱡᱩᱫᱤ ((nModifs & ~ALT) == 0)
				{
				    *pch2 = VirtKeyMap[i].chAlone;
				}
				ᱵᱟᱝ ᱡᱩᱫᱤ ((nModifs & SHIFT) != 0)
				{
				    *pch2 = VirtKeyMap[i].chShift;
				}
				ᱵᱟᱝ ᱡᱩᱫᱤ ((nModifs & CTRL) != 0)
				{
				    ᱡᱩᱫᱤ (pker->wVirtualKeyCode == VK_UP
					|| pker->wVirtualKeyCode == VK_DOWN)
				    {
					*pmodifiers |= MOD_MASK_CTRL;
					*pch2 = VirtKeyMap[i].chAlone;
				    }
				    ᱵᱟᱝ
				    {
					*pch2 = VirtKeyMap[i].chCtrl;
				    }
				}
			    }
			}
			ᱵᱟᱝ ᱡᱩᱫᱤ (pker->wVirtualKeyCode == VK_INSERT
					&& (nModifs & SHIFT) != 0
					&& (nModifs & ~SHIFT) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = VirtKeyMap[i].chShift;
			}
			ᱵᱟᱝ
			{
			    *pch2 = VirtKeyMap[i].chAlone;
			    ᱡᱩᱫᱤ ((nModifs & SHIFT) != 0)
				*pmodifiers |= MOD_MASK_SHIFT;
			    ᱡᱩᱫᱤ ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_MASK_CTRL;
			    ᱡᱩᱫᱤ ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_MASK_ALT;
			}
		    }
		}

		ᱨᱩᱣᱟᱹᱲ TRUE;
	    }
	}
    }

    i = win32_kbd_patch_key(pker);

    ᱡᱩᱫᱤ (i < 0)
	*pch = NUL;
    ᱵᱟᱝ
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	ᱡᱩᱫᱤ (pmodifiers != NULL)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    ᱡᱩᱫᱤ ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_MASK_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    ᱡᱩᱫᱤ ((nModifs & SHIFT) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_MASK_SHIFT;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    ᱡᱩᱫᱤ ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_MASK_CTRL;
	}
    }

    ᱨᱩᱣᱟᱹᱲ (*pch != NUL);
}

# ᱡᱩᱫᱤ defined(FEAT_EVAL)
    static puur_naa_nka
encode_key_event(dict_T *args, INPUT_RECORD *ir)
{
    static puur_naa_nka s_dwMods = 0;

    char_u *action = dict_get_string(args, "event", TRUE);
    ᱡᱩᱫᱤ (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL isKeyDown = STRICMP(action, "keydown") == 0;
	WORD vkCode = dict_get_number_def(args, "keycode", 0);
	ᱡᱩᱫᱤ (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    ᱨᱩᱣᱟᱹᱲ FALSE;
	}

	ir->EventType = KEY_EVENT;
	KEY_EVENT_RECORD ker;
	ZeroMemory(&ker, parimaa_na(ker));
	ker.bKeyDown = isKeyDown;
	ker.wRepeatCount = 1;
	ker.wVirtualScanCode = 0;
	ker.dwControlKeyState = 0;
	puur_naa_nka mods = (puur_naa_nka)dict_get_number(args, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	ᱡᱩᱫᱤ (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    ᱡᱩᱫᱤ (mods & MOD_MASK_SHIFT)
		ker.dwControlKeyState |= SHIFT_PRESSED;
	    ᱡᱩᱫᱤ (mods & MOD_MASK_CTRL)
		ker.dwControlKeyState |= LEFT_CTRL_PRESSED;
	    ᱡᱩᱫᱤ (mods & MOD_MASK_ALT)
		ker.dwControlKeyState |= LEFT_ALT_PRESSED;
	}

	ᱡᱩᱫᱤ (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT)
	{
	    ᱡᱩᱫᱤ (isKeyDown)
		s_dwMods |= SHIFT_PRESSED;
	    ᱵᱟᱝ
		s_dwMods &= ~SHIFT_PRESSED;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (vkCode == VK_LCONTROL || vkCode == VK_CONTROL)
	{
	    ᱡᱩᱫᱤ (isKeyDown)
		s_dwMods |= LEFT_CTRL_PRESSED;
	    ᱵᱟᱝ
		s_dwMods &= ~LEFT_CTRL_PRESSED;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (vkCode == VK_RCONTROL)
	{
	    ᱡᱩᱫᱤ (isKeyDown)
		s_dwMods |= RIGHT_CTRL_PRESSED;
	    ᱵᱟᱝ
		s_dwMods &= ~RIGHT_CTRL_PRESSED;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (vkCode == VK_LMENU || vkCode == VK_MENU)
	{
	    ᱡᱩᱫᱤ (isKeyDown)
		s_dwMods |= LEFT_ALT_PRESSED;
	    ᱵᱟᱝ
		s_dwMods &= ~LEFT_ALT_PRESSED;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (vkCode == VK_RMENU)
	{
	    ᱡᱩᱫᱤ (isKeyDown)
		s_dwMods |= RIGHT_ALT_PRESSED;
	    ᱵᱟᱝ
		s_dwMods &= ~RIGHT_ALT_PRESSED;
	}
	ker.dwControlKeyState |= s_dwMods;
	ker.wVirtualKeyCode = vkCode;
	ker.uChar.UnicodeChar = 0;
	ir->Event.KeyEvent = ker;
	vim_free(action);
    }
    ᱵᱟᱝ
    {
	ᱡᱩᱫᱤ (action == NULL)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	ᱵᱟᱝ
	{
	    semsg(_(e_invalid_value_for_argument_str_str), "event", action);
	    vim_free(action);
	}
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }
    ᱨᱩᱣᱟᱹᱲ TRUE;
}
# endif  // FEAT_EVAL
#endif // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
static puur_naa_nka g_fMouseAvail = FALSE;   // mouse present
static puur_naa_nka g_fMouseActive = FALSE;  // mouse enabled
static puur_naa_nka g_nMouseClick = -1;	    // mouse status
static puur_naa_nka g_xMouse;		    // mouse x coordinate
static puur_naa_nka g_yMouse;		    // mouse y coordinate
static DWORD g_cmodein = 0;	    // Original console input mode
static DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    void
mch_setmouse(puur_naa_nka on)
{
    DWORD cmodein;

# ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ;
# endif
    ᱡᱩᱫᱤ (!g_fMouseAvail)
	ᱨᱩᱣᱟᱹᱲ;

    g_fMouseActive = on;
    GetConsoleMode(g_hConIn, &cmodein);

    ᱡᱩᱫᱤ (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_INPUT;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    ᱵᱟᱝ
    {
	cmodein &= ~ENABLE_MOUSE_INPUT;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }

    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
}


# ᱡᱩᱫᱤ defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    void
mch_bevalterm_changed(void)
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
    static void
decode_mouse_wheel(MOUSE_EVENT_RECORD *pmer)
{
    puur_naa_nka	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    puur_naa_nka	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    puur_naa_nka lcol = g_xMouse;
    puur_naa_nka lrow = g_yMouse;
    win_T *wp = mouse_find_win(&lrow, &lcol, FIND_POPUP);
    ᱡᱩᱫᱤ (wp != NULL && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_FIELD(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	ᱡᱩᱫᱤ (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_LEFT : MSCR_RIGHT;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	ᱵᱟᱝ
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
    ᱡᱩᱫᱤ (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    ᱵᱟᱝ
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    ᱡᱩᱫᱤ (pmer->dwControlKeyState & SHIFT_PRESSED)
	modifiers |= MOD_MASK_SHIFT; // MOUSE_SHIFT;
    ᱡᱩᱫᱤ (pmer->dwControlKeyState & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED))
	modifiers |= MOD_MASK_CTRL; // MOUSE_CTRL;
    ᱡᱩᱫᱤ (pmer->dwControlKeyState & (RIGHT_ALT_PRESSED  | LEFT_ALT_PRESSED))
	modifiers |= MOD_MASK_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    ᱨᱩᱣᱟᱹᱲ;
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
    static BOOL
decode_mouse_event(
    MOUSE_EVENT_RECORD *pmer)
{
    static puur_naa_nka s_nOldButton = -1;
    static puur_naa_nka s_nOldMouseClick = -1;
    static puur_naa_nka s_xOldMouse = -1;
    static puur_naa_nka s_yOldMouse = -1;
    static linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    static puur_naa_nka s_old_topfill = 0;
# endif
    static puur_naa_nka s_cClicks = 1;
    static BOOL s_fReleased = TRUE;
    static DWORD s_dwLastClickTime = 0;
    static BOOL s_fNextIsMiddle = FALSE;

    static DWORD cButtons = 0;	// number of buttons supported

    const DWORD LEFT = FROM_LEFT_1ST_BUTTON_PRESSED;
    const DWORD MIDDLE = FROM_LEFT_2ND_BUTTON_PRESSED;
    const DWORD RIGHT = RIGHTMOST_BUTTON_PRESSED;
    const DWORD LEFT_RIGHT = LEFT | RIGHT;

    puur_naa_nka nButton;

    ᱡᱩᱫᱤ (cButtons == 0 && !GetNumberOfConsoleMouseButtons(&cButtons))
	cButtons = 2;

    ᱡᱩᱫᱤ (!g_fMouseAvail || !g_fMouseActive)
    {
	g_nMouseClick = -1;
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    ᱡᱩᱫᱤ (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    ᱡᱩᱫᱤ (g_nMouseClick != -1)
	ᱨᱩᱣᱟᱹᱲ TRUE;

    ᱡᱩᱫᱤ (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	ᱨᱩᱣᱟᱹᱲ TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    ᱡᱩᱫᱤ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	ᱡᱩᱫᱤ (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    ᱨᱩᱣᱟᱹᱲ FALSE;
    }

    // If no buttons are pressed...
    ᱡᱩᱫᱤ ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	ᱡᱩᱫᱤ (s_fReleased)
	{
# ifdef FEAT_BEVAL_TERM
	    // do return mouse move events when we want them
	    ᱡᱩᱫᱤ (p_bevalterm)
		nButton = MOUSE_DRAG;
	    ᱵᱟᱝ
# endif
		ᱨᱩᱣᱟᱹᱲ FALSE;
	}

	s_fReleased = TRUE;
    }
    ᱵᱟᱝ    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	ᱡᱩᱫᱤ (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & LEFT_RIGHT);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    ᱡᱩᱫᱤ (dwLR == LEFT || dwLR == RIGHT)
	    {
		for (;;)
		{
		    // wait a short time for next input event
		    ᱡᱩᱫᱤ (WaitForSingleObject(g_hConIn, p_mouset / 3)
							     != WAIT_OBJECT_0)
			bha_nga;
		    ᱵᱟᱝ
		    {
			DWORD cRecords = 0;
			INPUT_RECORD ir;
			MOUSE_EVENT_RECORD* pmer2 = &ir.Event.MouseEvent;

			peek_console_input(g_hConIn, &ir, 1, &cRecords);

			ᱡᱩᱫᱤ (cRecords == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & LEFT_RIGHT))
			    bha_nga;
			ᱵᱟᱝ
			{
			    ᱡᱩᱫᱤ (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_input(g_hConIn, &ir, 1, &cRecords);

				ᱨᱩᱣᱟᱹᱲ decode_mouse_event(pmer2);
			    }
			    ᱵᱟᱝ ᱡᱩᱫᱤ (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_input(g_hConIn, &ir, 1, &cRecords);

				// are there any more mouse events in queue?
				peek_console_input(g_hConIn, &ir, 1, &cRecords);

				ᱡᱩᱫᱤ (cRecords==0 || ir.EventType != MOUSE_EVENT)
				    bha_nga;
			    }
			    ᱵᱟᱝ
				bha_nga;
			}
		    }
		}
	    }
	}

	ᱡᱩᱫᱤ (s_fNextIsMiddle)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_MIDDLE;
	    s_fNextIsMiddle = FALSE;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (cButtons == 2	&&
	    ((pmer->dwButtonState & LEFT_RIGHT) == LEFT_RIGHT))
	{
	    nButton = MOUSE_MIDDLE;

	    ᱡᱩᱫᱤ (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_fNextIsMiddle = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ ((pmer->dwButtonState & LEFT) == LEFT)
	    nButton = MOUSE_LEFT;
	ᱵᱟᱝ ᱡᱩᱫᱤ ((pmer->dwButtonState & MIDDLE) == MIDDLE)
	    nButton = MOUSE_MIDDLE;
	ᱵᱟᱝ ᱡᱩᱫᱤ ((pmer->dwButtonState & RIGHT) == RIGHT)
	    nButton = MOUSE_RIGHT;

	ᱡᱩᱫᱤ (! s_fReleased && ! s_fNextIsMiddle
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    ᱨᱩᱣᱟᱹᱲ FALSE;

	s_fReleased = s_fNextIsMiddle;
    }

    ᱡᱩᱫᱤ (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	ᱡᱩᱫᱤ (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dwCurrentTime = GetTickCount();

	    ᱡᱩᱫᱤ (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (puur_naa_nka)(dwCurrentTime - s_dwLastClickTime) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dwLastClickTime = dwCurrentTime;
	}
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	ᱡᱩᱫᱤ (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    ᱡᱩᱫᱤ (nButton == -1)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    ᱡᱩᱫᱤ (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    ᱡᱩᱫᱤ (pmer->dwControlKeyState & SHIFT_PRESSED)
	g_nMouseClick |= MOUSE_SHIFT;
    ᱡᱩᱫᱤ (pmer->dwControlKeyState & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    ᱡᱩᱫᱤ (pmer->dwControlKeyState & (RIGHT_ALT_PRESSED  | LEFT_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    ᱡᱩᱫᱤ (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	SET_NUM_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    ᱡᱩᱫᱤ (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    ᱨᱩᱣᱟᱹᱲ TRUE;
}

# ifdef FEAT_EVAL
    static puur_naa_nka
encode_mouse_event(dict_T *args, INPUT_RECORD *ir)
{
    puur_naa_nka		button;
    puur_naa_nka		row;
    puur_naa_nka		col;
    puur_naa_nka		repeated_click;
    int_u	mods = 0;
    puur_naa_nka		move;

    ᱡᱩᱫᱤ (!dict_has_key(args, "row") || !dict_has_key(args, "col"))
	ᱨᱩᱣᱟᱹᱲ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (puur_naa_nka)dict_get_bool(args, "move", FALSE);
    ᱡᱩᱫᱤ (!move && (!dict_has_key(args, "button")
	    || !dict_has_key(args, "multiclick")
	    || !dict_has_key(args, "modifiers")))
	ᱨᱩᱣᱟᱹᱲ FALSE;

    row = (puur_naa_nka)dict_get_number(args, "row") - 1;
    col = (puur_naa_nka)dict_get_number(args, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_RECORD mer;
    ZeroMemory(&mer, parimaa_na(mer));
    mer.dwMousePosition.X  = col;
    mer.dwMousePosition.Y  = row;

    ᱡᱩᱫᱤ (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    ᱵᱟᱝ
    {
	button = (puur_naa_nka)dict_get_number(args, "button");
	repeated_click = (puur_naa_nka)dict_get_number(args, "multiclick");
	mods = (puur_naa_nka)dict_get_number(args, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	switch (button)
	{
	    pak_sa MOUSE_LEFT:
		mer.dwButtonState = FROM_LEFT_1ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		bha_nga;
	    pak_sa MOUSE_MIDDLE:
		mer.dwButtonState = FROM_LEFT_2ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		bha_nga;
	    pak_sa MOUSE_RIGHT:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		bha_nga;
	    pak_sa MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    pak_sa MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		bha_nga;
	    pak_sa MOUSE_X1:
		mer.dwButtonState = FROM_LEFT_3RD_BUTTON_PRESSED;
		bha_nga;
	    pak_sa MOUSE_X2:
		mer.dwButtonState = FROM_LEFT_4TH_BUTTON_PRESSED;
		bha_nga;
	    pak_sa MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		bha_nga;
	    pak_sa MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		bha_nga;
	    pak_sa MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		bha_nga;
	    pak_sa MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		bha_nga;
	    default:
		semsg(_(e_invalid_argument_str), "button");
		ᱨᱩᱣᱟᱹᱲ FALSE;
	}
    }

    mer.dwControlKeyState = 0;
    ᱡᱩᱫᱤ (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	ᱡᱩᱫᱤ (mods & MOUSE_SHIFT)
	    mer.dwControlKeyState |= SHIFT_PRESSED;
	ᱡᱩᱫᱤ (mods & MOUSE_CTRL)
	    mer.dwControlKeyState |= LEFT_CTRL_PRESSED;
	ᱡᱩᱫᱤ (mods & MOUSE_ALT)
	    mer.dwControlKeyState |= LEFT_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    ᱨᱩᱣᱟᱹᱲ TRUE;
}
# endif  // FEAT_EVAL

    static puur_naa_nka
write_input_record_buffer(INPUT_RECORD* irEvents, puur_naa_nka nLength)
{
    puur_naa_nka				nCount = 0;
    input_record_buffer_node_T	*event_node;

    ᱡᱚᱠᱷᱚᱱ (nCount < nLength)
    {
	event_node = alloc(parimaa_na(input_record_buffer_node_T));
	ᱡᱩᱫᱤ (event_node == NULL)
	    ᱨᱩᱣᱟᱹᱲ -1;

	event_node->ir = irEvents[nCount++];
	event_node->next = NULL;

	input_record_buffer.length++;
	ᱡᱩᱫᱤ (input_record_buffer.tail == NULL)
	{
	    input_record_buffer.head = event_node;
	    input_record_buffer.tail = event_node;
	}
	ᱵᱟᱝ
	{
	    input_record_buffer.tail->next = event_node;
	    input_record_buffer.tail = input_record_buffer.tail->next;
	}
    }
    ᱨᱩᱣᱟᱹᱲ nCount;
}

    static puur_naa_nka
read_input_record_buffer(INPUT_RECORD* irEvents, puur_naa_nka nMaxLength)
{
    puur_naa_nka nCount = 0;
    ᱡᱚᱠᱷᱚᱱ (nCount < nMaxLength && input_record_buffer.head != NULL)
    {
	input_record_buffer.length--;
	input_record_buffer_node_T *pop_head = input_record_buffer.head;
	irEvents[nCount++] = pop_head->ir;
	input_record_buffer.head = pop_head->next;
	vim_free(pop_head);
	ᱡᱩᱫᱤ (input_record_buffer.length == 0)
	    input_record_buffer.tail = NULL;
    }
    ᱨᱩᱣᱟᱹᱲ nCount;
}
#ᱵᱟᱝ  // FEAT_GUI_MSWIN && !VIMDLL
    void
mch_setmouse(puur_naa_nka on UNUSED)
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
    puur_naa_nka
test_mswin_event(char_u *event, dict_T *args)
{
    puur_naa_nka lpEventsWritten = 0;

# ᱡᱩᱫᱤ defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ test_gui_w32_sendevent(event, args);
# endif

# ᱡᱩᱫᱤ defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    INPUT_RECORD ir;
    BOOL input_encoded = FALSE;
    BOOL execute = FALSE;
    ᱡᱩᱫᱤ (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(args, "execute", FALSE);
	ᱡᱩᱫᱤ (dict_has_key(args, "event"))
	    input_encoded = encode_key_event(args, &ir);
	ᱵᱟᱝ ᱡᱩᱫᱤ (!execute)
	{
	    semsg(_(e_missing_argument_str), "event");
	    ᱨᱩᱣᱟᱹᱲ FALSE;
	}
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	input_encoded = encode_mouse_event(args, &ir);
    }
    ᱵᱟᱝ
    {
	semsg(_(e_invalid_value_for_argument_str_str), "event", event);
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    ᱡᱩᱫᱤ (input_encoded)
    {
	ᱡᱩᱫᱤ ((lpEventsWritten = write_input_record_buffer(&ir, 1)) < 0)
	{
	    semsg(_(e_out_of_memory), "event");
	    ᱨᱩᱣᱟᱹᱲ FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    ᱡᱩᱫᱤ (execute)
    {
	puur_naa_nka save_msg_scroll = msg_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	msg_scroll = FALSE;
	ch_log(NULL, "test_mswin_event() executing");
	exec_normal(TRUE, TRUE, TRUE);
	msg_scroll |= save_msg_scroll;
    }

# endif
    ᱨᱩᱣᱟᱹᱲ lpEventsWritten;
}
#endif // FEAT_EVAL

#ᱡᱩᱫᱤ defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    static void
mch_set_cursor_shape(puur_naa_nka thickness)
{
    ᱡᱩᱫᱤ (vtp_working)
    {
	ᱡᱩᱫᱤ (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    ᱡᱩᱫᱤ (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    ᱵᱟᱝ
		vtp_printf("\033[0 q");	// default
	}
    }
    ᱵᱟᱝ
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dwSize = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	SetConsoleCursorInfo(g_hConOut, &ConsoleCursorInfo);
	ᱡᱩᱫᱤ (s_cursor_visible)
	    SetConsoleCursorPosition(g_hConOut, g_coord);
    }
}

    void
mch_update_cursor(void)
{
    puur_naa_nka		idx;
    puur_naa_nka		thickness;

# ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    ᱡᱩᱫᱤ (shape_table[idx].shape == SHAPE_BLOCK)
	thickness = 99;	// 100 doesn't work on W95
    ᱵᱟᱝ
	thickness = shape_table[idx].percentage;
    mch_set_cursor_shape(thickness);
}
#endif

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    static void
handle_focus_event(INPUT_RECORD ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
    ui_focus_change((puur_naa_nka)g_fJustGotFocus);
}

static void ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    static puur_naa_nka
WaitForChar(long msec, puur_naa_nka ignore_input)
{
    DWORD	    dwNow = 0, dwEndTime = 0;
    INPUT_RECORD    ir;
    DWORD	    cRecords;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    puur_naa_nka		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    ᱡᱩᱫᱤ (msec > 0)
	// Wait until the specified time has elapsed.
	dwEndTime = GetTickCount() + msec;
    ᱵᱟᱝ ᱡᱩᱫᱤ (msec < 0)
	// Wait forever.
	dwEndTime = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    for (;;)
    {
	// Only process messages when waiting.
	ᱡᱩᱫᱤ (msec != 0)
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

	ᱡᱩᱫᱤ (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_input && input_available())
# endif
	   )
	    ᱨᱩᱣᱟᱹᱲ TRUE;

	ᱡᱩᱫᱤ (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = GetTickCount();
	    ᱡᱩᱫᱤ ((puur_naa_nka)(dwNow - dwEndTime) >= 0)
		bha_nga;
	}
	ᱡᱩᱫᱤ (msec != 0)
	{
	    DWORD dwWaitTime = dwEndTime - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    ᱡᱩᱫᱤ (dwWaitTime > 11)
		dwWaitTime = 11;

# ifdef FEAT_MZSCHEME
	    ᱡᱩᱫᱤ (mzthreads_allowed() && p_mzq > 0 && (long)dwWaitTime > p_mzq)
		dwWaitTime = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    ᱡᱩᱫᱤ (dwWaitTime > 10)
	    {
		long	due_time;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_time = check_due_timer();
		ᱡᱩᱫᱤ (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    ᱨᱩᱣᱟᱹᱲ FALSE;
		}
		ᱡᱩᱫᱤ (due_time > 0 && dwWaitTime > (DWORD)due_time)
		    dwWaitTime = due_time;
	    }
# endif
	    ᱡᱩᱫᱤ (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    msg_wait_for_multiple_objects(1, &g_hConIn, FALSE,
				  dwWaitTime, QS_SENDMESSAGE) != WAIT_OBJECT_0
# ᱵᱟᱝ
		    wait_for_single_object(g_hConIn, dwWaitTime)
							      != WAIT_OBJECT_0
# endif
		    )
		continue;
	}

	cRecords = 0;
	peek_console_input(g_hConIn, &ir, 1, &cRecords);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	ᱡᱩᱫᱤ (State == MODE_CMDLINE && msg_row == Rows - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_BUFFER_INFO csbi;

	    ᱡᱩᱫᱤ (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	    {
		ᱡᱩᱫᱤ (csbi.dwCursorPosition.Y != msg_row)
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

	ᱡᱩᱫᱤ (cRecords > 0)
	{
	    ᱡᱩᱫᱤ (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		ᱡᱩᱫᱤ (ir.Event.KeyEvent.uChar.UnicodeChar == 0
			&& ir.Event.KeyEvent.wVirtualKeyCode == 13)
		{
		    read_console_input(g_hConIn, &ir, 1, &cRecords);
		    continue;
		}
# endif
		ᱡᱩᱫᱤ (decode_key_event(&ir.Event.KeyEvent, &ch, &ch2,
								 NULL, FALSE))
		    ᱨᱩᱣᱟᱹᱲ TRUE;
	    }

	    read_console_input(g_hConIn, &ir, 1, &cRecords);

	    ᱡᱩᱫᱤ (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    ᱵᱟᱝ ᱡᱩᱫᱤ (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    {
		COORD dwSize = ir.Event.WindowBufferSizeEvent.dwSize;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		ᱡᱩᱫᱤ (dwSize.X != Columns || dwSize.Y != Rows)
		{
		    CONSOLE_SCREEN_BUFFER_INFO csbi;
		    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
		    dwSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		    dwSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		    ᱡᱩᱫᱤ (dwSize.X != Columns || dwSize.Y != Rows)
		    {
			ResizeConBuf(g_hConOut, dwSize);
			shell_resized();
		    }
		}
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		ᱨᱩᱣᱟᱹᱲ TRUE;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (msec == 0)
	    bha_nga;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    ᱡᱩᱫᱤ (input_available())
	ᱨᱩᱣᱟᱹᱲ TRUE;
# endif

    ᱨᱩᱣᱟᱹᱲ FALSE;
}

/*
 * return non-zero if a character is available
 */
    puur_naa_nka
mch_char_avail(void)
{
# ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ TRUE;
# endif
    ᱨᱩᱣᱟᱹᱲ WaitForChar(0L, FALSE);
}

# ᱡᱩᱫᱤ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    puur_naa_nka
mch_check_messages(void)
{
#  ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ TRUE;
#  endif
    ᱨᱩᱣᱟᱹᱲ WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    static void
create_conin(void)
{
    g_hConIn =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES) NULL,
			OPEN_EXISTING, 0, (HANDLE)NULL);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    static WCHAR
tgetch(puur_naa_nka *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    for (;;)
    {
	INPUT_RECORD ir;
	DWORD cRecords = 0;

# ifdef FEAT_CLIENTSERVER
	(void)WaitForChar(-1L, FALSE);
	ᱡᱩᱫᱤ (input_available())
	    ᱨᱩᱣᱟᱹᱲ 0;
	ᱡᱩᱫᱤ (g_nMouseClick != -1)
	    ᱨᱩᱣᱟᱹᱲ 0;
# endif
	ᱡᱩᱫᱤ (read_console_input(g_hConIn, &ir, 1, &cRecords) == 0)
	{
	    ᱡᱩᱫᱤ (did_create_conin)
		read_error_exit();
	    create_conin();
	    continue;
	}

	ᱡᱩᱫᱤ (ir.EventType == KEY_EVENT)
	{
	    ᱡᱩᱫᱤ (decode_key_event(&ir.Event.KeyEvent, &ch, pch2,
							    pmodifiers, TRUE))
		ᱨᱩᱣᱟᱹᱲ ch;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	ᱵᱟᱝ ᱡᱩᱫᱤ (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    shell_resized();
	ᱵᱟᱝ ᱡᱩᱫᱤ (ir.EventType == MOUSE_EVENT)
	{
	    ᱡᱩᱫᱤ (decode_mouse_event(&ir.Event.MouseEvent))
		ᱨᱩᱣᱟᱹᱲ 0;
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
    puur_naa_nka
mch_inchar(
    char_u	*buf UNUSED,
    puur_naa_nka		maxlen UNUSED,
    long	time UNUSED,
    puur_naa_nka		tb_change_cnt UNUSED)
{
#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    puur_naa_nka		len;
    puur_naa_nka		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# ᱵᱟᱝ
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    static char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    static puur_naa_nka	    typeaheadlen = 0;

# ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    ᱡᱩᱫᱤ (typeaheadlen > 0)
	goto theend;

    ᱡᱩᱫᱤ (time >= 0)
    {
	ᱡᱩᱫᱤ (!WaitForChar(time, FALSE))     // no character available
	    ᱨᱩᱣᱟᱹᱲ 0;
    }
    ᱵᱟᱝ    // time == -1, wait forever
    {
	mch_set_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ᱡᱩᱫᱤ (!WaitForChar(p_ut, FALSE))
	{
	    ᱡᱩᱫᱤ (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (puur_naa_nka)KE_CURSORHOLD;
		ᱨᱩᱣᱟᱹᱲ 3;
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
    ᱡᱩᱫᱤ (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    ᱡᱚᱠᱷᱚᱱ ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	ᱡᱩᱫᱤ (typebuf_changed(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    bha_nga;
	}
	ᱡᱩᱫᱤ (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    ᱡᱩᱫᱤ (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    ᱡᱩᱫᱤ (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		ᱡᱩᱫᱤ (modifiers > 0)
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
	    ᱵᱟᱝ
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
	ᱵᱟᱝ
	{
	    WCHAR	ch2 = NUL;
	    puur_naa_nka		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_key(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_keys;
	    c = may_adjust_key_for_ctrl(modifiers, c);
	    --no_reduce_keys;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_shift_modifier(modifiers, c);

	    ᱡᱩᱫᱤ (typebuf_changed(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		bha_nga;
	    }

	    ᱡᱩᱫᱤ (c == Ctrl_C && ctrl_c_interrupts)
	    {
# ᱡᱩᱫᱤ defined(FEAT_CLIENTSERVER)
		trash_input_buf();
# endif
		got_int = TRUE;
	    }

	    ᱡᱩᱫᱤ (g_nMouseClick == -1)
	    {
		puur_naa_nka	n = 1;

		ᱡᱩᱫᱤ (ch2 == NUL)
		{
		    puur_naa_nka	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    ᱡᱩᱫᱤ (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_to_enc(ch, &n);
		    ᱡᱩᱫᱤ (p != NULL)
		    {
			for (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    ᱡᱩᱫᱤ (p[i] == CSI)
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
		ᱵᱟᱝ
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    ᱡᱩᱫᱤ (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		ᱡᱩᱫᱤ (ch2 != NUL)
		{
		    ᱡᱩᱫᱤ (c == K_NUL)
		    {
			switch (ch2)
			{
			pak_sa (WCHAR)'\324': // SHIFT+Insert
			pak_sa (WCHAR)'\325': // CTRL+Insert
			pak_sa (WCHAR)'\327': // SHIFT+Delete
			pak_sa (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    bha_nga;

			default:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    bha_nga;
			}
		    }
		    ᱵᱟᱝ
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
		ᱡᱩᱫᱤ ((modifiers & MOD_MASK_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_MASK_ALT;
		}

		ᱡᱩᱫᱤ (modifiers != 0)
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
		ᱡᱩᱫᱤ (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    ᱡᱩᱫᱤ (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    ᱡᱚᱠᱷᱚᱱ (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    ᱡᱩᱫᱤ (len > 0)
    {
	buf[len] = NUL;
	ch_log(NULL, "raw key input: \"%s\"", buf);
    }
# endif
    ᱨᱩᱣᱟᱹᱲ len;

#ᱵᱟᱝ // FEAT_GUI_MSWIN && !VIMDLL
    ᱨᱩᱣᱟᱹᱲ 0;
#endif // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    static puur_naa_nka
executable_file(var_na *name, char_u **path)
{
    puur_naa_nka attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    ᱡᱩᱫᱤ (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	ᱨᱩᱣᱟᱹᱲ FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    ᱡᱩᱫᱤ (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	ᱡᱩᱫᱤ (res == NULL)
	    res = resolve_reparse_point((char_u *)name);
	ᱡᱩᱫᱤ (res == NULL)
	    ᱨᱩᱣᱟᱹᱲ FALSE;
	// The path is already absolute.
	ᱡᱩᱫᱤ (path != NULL)
	    *path = res;
	ᱵᱟᱝ
	    vim_free(res);
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (path != NULL)
	*path = FullName_save((char_u *)name, FALSE);
    ᱨᱩᱣᱟᱹᱲ TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    static bool
executable_prefix_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_FIND_DATAW	data;
    HANDLE		handle;
    DWORD		error;

    wn = enc_to_utf16(name, NULL);
    ᱡᱩᱫᱤ (wn == NULL)
	ᱨᱩᱣᱟᱹᱲ true;
    handle = FindFirstFileExW(wn, FindExInfoBasic, &data,
					    FindExSearchNameMatch, NULL, 0);
    error = GetLastError();
    vim_free(wn);
    ᱡᱩᱫᱤ (handle != INVALID_HANDLE_VALUE)
    {
	FindClose(handle);
	ᱨᱩᱣᱟᱹᱲ true;
    }
    ᱨᱩᱣᱟᱹᱲ error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    static puur_naa_nka
executable_exists(
    var_na	*name,
    size_t	namelen,
    char_u	**path,
    puur_naa_nka		use_path,
    puur_naa_nka		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3];
    char_u	*p;
    string_T	pathbuf = {NULL, 0};
    puur_naa_nka		pathbuf_allocated = FALSE;
    string_T	pathext = {NULL, 0};
    puur_naa_nka		pathext_allocated = FALSE;
    char_u	*shname = NULL;
    puur_naa_nka		noext = FALSE;
    bool	simple_pathext = true;
    puur_naa_nka		ext_count = 0;
    puur_naa_nka		retval = FALSE;

    ᱡᱩᱫᱤ (namelen >= parimaa_na(buf))	// safety check
	ᱨᱩᱣᱟᱹᱲ FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    ᱡᱩᱫᱤ (strstr((var_na *)shname, "sh") != NULL &&
	!(strstr((var_na *)shname, "powershell") != NULL
				    || strstr((var_na *)shname, "pwsh") != NULL))
	noext = TRUE;

    ᱡᱩᱫᱤ (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	ᱡᱩᱫᱤ (pathext.string == NULL)
	    STR_LITERAL_SET(pathext, ".com;.exe;.bat;.cmd");
	ᱵᱟᱝ
	    pathext.length = STRLEN(pathext.string);

	ᱡᱩᱫᱤ (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    ᱡᱚᱠᱷᱚᱱ (*p)
	    {
		ᱡᱩᱫᱤ (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    ᱡᱩᱫᱤ (*p == '.')
			simple_pathext = false;
		    ++p;
		    continue;
		}
		e = vim_strchr(p, ';');
		ᱡᱩᱫᱤ (e == NULL)
		    e = pathext.string + pathext.length;
		plen = (size_t)(e - p);
		++ext_count;
		ᱡᱩᱫᱤ (*p != '.')
		    simple_pathext = false;
		for (q = p + 1; q < e; ++q)
		    ᱡᱩᱫᱤ (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		ᱡᱩᱫᱤ (_strnicoll(name + namelen - plen, (var_na *)p, plen) == 0)
		{
		    noext = TRUE;
		    bha_nga;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    ᱡᱩᱫᱤ (pathext.string == NULL)
	STR_LITERAL_SET(pathext, ".");
    ᱵᱟᱝ ᱡᱩᱫᱤ (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.length + 3);
	ᱡᱩᱫᱤ (tmp == NULL)
	{
	    retval = FALSE;
	    goto theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.length += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    ᱡᱩᱫᱤ (use_path && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	ᱡᱩᱫᱤ (p != NULL)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    ᱡᱩᱫᱤ (pathbuf.string == NULL)
	    {
		retval = FALSE;
		goto theend;
	    }

	    ᱡᱩᱫᱤ (mch_getenv("NoDefaultCurrentDirectoryInExePath") == NULL &&
		    strstr((var_na *)gettail(p_sh), "cmd.exe") != NULL)
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
    ᱡᱩᱫᱤ (pathbuf.string == NULL)
	STR_LITERAL_SET(pathbuf, ".");
    p = pathbuf.string;
    ᱡᱚᱠᱷᱚᱱ (*p)
    {
	char_u	*e;
	size_t	buflen;

	ᱡᱩᱫᱤ (*p == ';') // Skip empty entry
	{
	    ++p;
	    continue;
	}
	e = vim_strchr(p, ';');
	ᱡᱩᱫᱤ (e == NULL)
	    e = pathbuf.string + pathbuf.length;

	ᱡᱩᱫᱤ (e - p + namelen + 2 > parimaa_na(buf))
	{
	    retval = FALSE;
	    goto theend;
	}
	// A single "." that means current dir.
	ᱡᱩᱫᱤ (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	ᱵᱟᱝ
	{
	    buflen = vim_snprintf_safelen(
		(var_na *)buf,
		parimaa_na(buf),
		"%.*s%s%s", (puur_naa_nka)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	ᱡᱩᱫᱤ (!noext && simple_pathext && ext_count >= 3 && buflen > 0
		&& buflen + 2 <= parimaa_na(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == NULL
		&& vim_strchr(gettail(buf), ':') == NULL)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_prefix_exists(buf);
	    buf[buflen] = NUL;
	    ᱡᱩᱫᱤ (!exists)
	    {
		p = e;
		continue;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	ᱡᱚᱠᱷᱚᱱ (*p)
	{
	    char_u  *e2;

	    ᱡᱩᱫᱤ (*p == ';')
	    {
		// Skip empty entry
		++p;
		continue;
	    }
	    e2 = vim_strchr(p, ';');
	    ᱡᱩᱫᱤ (e2 == NULL)
		e2 = pathext.string + pathext.length;

	    ᱡᱩᱫᱤ (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		ᱡᱩᱫᱤ (e2 - p + buflen + 1 > parimaa_na(buf))
		{
		    retval = FALSE;
		    goto theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    ᱡᱩᱫᱤ (executable_file((var_na *)buf, path))
	    {
		retval = TRUE;
		goto theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    ᱡᱩᱫᱤ (pathbuf_allocated)
	free(pathbuf.string);
    ᱡᱩᱫᱤ (pathext_allocated)
	free(pathext.string);
    ᱨᱩᱣᱟᱹᱲ retval;
}

#ᱡᱩᱫᱤ (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    static void
bad_param_handler(const wchar_t *expression UNUSED,
    const wchar_t *function UNUSED,
    const wchar_t *file UNUSED,
    unsigned puur_naa_nka line UNUSED,
    uintptr_t pReserved UNUSED)
{
}

# define SET_INVALID_PARAM_HANDLER \
	((void)_set_invalid_parameter_handler(bad_param_handler))
#ᱵᱟᱝ
# define SET_INVALID_PARAM_HANDLER
#endif

#ifdef FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    static void
mch_init_g(void)
{
# ifndef __MINGW32__
    extern puur_naa_nka _fmode;
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
	ᱡᱩᱫᱤ (exe_pathlen + 10 >= parimaa_na(vimrun_location))
	{
	    ᱡᱩᱫᱤ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    NULL, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	ᱵᱟᱝ
	{
	    // First try in same directory as gvim.exe.
	    ᱡᱩᱫᱤ (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    ᱡᱩᱫᱤ (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		ᱡᱩᱫᱤ (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(var_na *)vimrun_location,
			parimaa_na(vimrun_location),
			"\"%.*svimrun\" ",
			(puur_naa_nka)exe_pathlen, exe_name);
		}
		ᱵᱟᱝ
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		ᱡᱩᱫᱤ (p != NULL)
		{
		    vimrun_path.string = p;
		    vimrun_path.length = plen;
		    vimrun_path_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    NULL, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	ᱡᱩᱫᱤ (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    ᱡᱩᱫᱤ (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    NULL, TRUE, FALSE))
	set_option_value_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#endif // FEAT_GUI_MSWIN

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

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
    static BOOL
ClearConsoleBuffer(WORD wAttribute)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    DWORD NumCells, dummy;

    ᱡᱩᱫᱤ (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ᱨᱩᱣᱟᱹᱲ FALSE;

    NumCells = csbi.dwSize.X * csbi.dwSize.Y;
    coord.X = 0;
    coord.Y = 0;
    ᱡᱩᱫᱤ (!FillConsoleOutputCharacter(g_hConOut, ' ', NumCells,
	    coord, &dummy))
	ᱨᱩᱣᱟᱹᱲ FALSE;
    ᱡᱩᱫᱤ (!FillConsoleOutputAttribute(g_hConOut, wAttribute, NumCells,
	    coord, &dummy))
	ᱨᱩᱣᱟᱹᱲ FALSE;

    ᱨᱩᱣᱟᱹᱲ TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    static BOOL
FitConsoleWindow(
    COORD dwBufferSize,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD dwWindowSize;
    BOOL NeedAdjust = FALSE;

    ᱡᱩᱫᱤ (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ᱨᱩᱣᱟᱹᱲ FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    ᱡᱩᱫᱤ (csbi.srWindow.Right >= dwBufferSize.X)
    {
	dwWindowSize.X = SRWIDTH(csbi.srWindow);
	ᱡᱩᱫᱤ (dwWindowSize.X > dwBufferSize.X)
	    dwWindowSize.X = dwBufferSize.X;
	csbi.srWindow.Right = dwBufferSize.X - 1;
	csbi.srWindow.Left = dwBufferSize.X - dwWindowSize.X;
	NeedAdjust = TRUE;
    }
    ᱡᱩᱫᱤ (csbi.srWindow.Bottom >= dwBufferSize.Y)
    {
	dwWindowSize.Y = SRHEIGHT(csbi.srWindow);
	ᱡᱩᱫᱤ (dwWindowSize.Y > dwBufferSize.Y)
	    dwWindowSize.Y = dwBufferSize.Y;
	csbi.srWindow.Bottom = dwBufferSize.Y - 1;
	csbi.srWindow.Top = dwBufferSize.Y - dwWindowSize.Y;
	NeedAdjust = TRUE;
    }
    ᱡᱩᱫᱤ (NeedAdjust && WantAdjust)
    {
	ᱡᱩᱫᱤ (!SetConsoleWindowInfo(g_hConOut, TRUE, &csbi.srWindow))
	    ᱨᱩᱣᱟᱹᱲ FALSE;
    }
    ᱨᱩᱣᱟᱹᱲ TRUE;
}

typedef sa_nracanaa ConsoleBufferStruct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_BUFFER_INFO	Info;
    PCHAR_INFO			Buffer;
    COORD			BufferSize;
    PSMALL_RECT			Regions;
    puur_naa_nka				NumRegions;
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
    static BOOL
SaveConsoleBuffer(
    ConsoleBuffer *cb)
{
    DWORD NumCells;
    COORD BufferCoord;
    SMALL_RECT ReadRegion;
    WORD Y, Y_incr;
    puur_naa_nka i, numregions;

    ᱡᱩᱫᱤ (cb == NULL)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    ᱡᱩᱫᱤ (!GetConsoleScreenBufferInfo(g_hConOut, &cb->Info))
    {
	cb->IsValid = FALSE;
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    ᱡᱩᱫᱤ (use_alternate_screen_buffer)
	ᱨᱩᱣᱟᱹᱲ TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    ᱡᱩᱫᱤ (!cb->IsValid || cb->Buffer == NULL ||
	    cb->BufferSize.X != cb->Info.dwSize.X ||
	    cb->BufferSize.Y != cb->Info.dwSize.Y)
    {
	cb->BufferSize.X = cb->Info.dwSize.X;
	cb->BufferSize.Y = cb->Info.dwSize.Y;
	NumCells = cb->BufferSize.X * cb->BufferSize.Y;
	vim_free(cb->Buffer);
	cb->Buffer = ALLOC_MULT(CHAR_INFO, NumCells);
	ᱡᱩᱫᱤ (cb->Buffer == NULL)
	    ᱨᱩᱣᱟᱹᱲ FALSE;
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
    ᱡᱩᱫᱤ (cb->Regions == NULL || numregions != cb->NumRegions)
    {
	cb->NumRegions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->NumRegions);
	ᱡᱩᱫᱤ (cb->Regions == NULL)
	{
	    VIM_CLEAR(cb->Buffer);
	    ᱨᱩᱣᱟᱹᱲ FALSE;
	}
    }

    for (i = 0, Y = 0; i < cb->NumRegions; i++, Y += Y_incr)
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
	ᱡᱩᱫᱤ (!ReadConsoleOutputW(g_hConOut,	// output handle
		cb->Buffer,			// our buffer
		cb->BufferSize,			// dimensions of our buffer
		BufferCoord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->Buffer);
	    VIM_CLEAR(cb->Regions);
	    ᱨᱩᱣᱟᱹᱲ FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    ᱨᱩᱣᱟᱹᱲ TRUE;
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
    static BOOL
RestoreConsoleBuffer(
    ConsoleBuffer   *cb,
    BOOL	    RestoreScreen)
{
    COORD BufferCoord;
    SMALL_RECT WriteRegion;
    puur_naa_nka i;

    ᱡᱩᱫᱤ (cb == NULL || !cb->IsValid)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    ᱡᱩᱫᱤ (RestoreScreen)
	ClearConsoleBuffer(cb->Info.wAttributes);

    FitConsoleWindow(cb->Info.dwSize, TRUE);
    ᱡᱩᱫᱤ (!SetConsoleScreenBufferSize(g_hConOut, cb->Info.dwSize))
	ᱨᱩᱣᱟᱹᱲ FALSE;
    ᱡᱩᱫᱤ (!SetConsoleTextAttribute(g_hConOut, cb->Info.wAttributes))
	ᱨᱩᱣᱟᱹᱲ FALSE;

    ᱡᱩᱫᱤ (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	ᱨᱩᱣᱟᱹᱲ TRUE;
    }

    ᱡᱩᱫᱤ (!SetConsoleCursorPosition(g_hConOut, cb->Info.dwCursorPosition))
	ᱨᱩᱣᱟᱹᱲ FALSE;
    ᱡᱩᱫᱤ (!SetConsoleWindowInfo(g_hConOut, TRUE, &cb->Info.srWindow))
	ᱨᱩᱣᱟᱹᱲ FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    ᱡᱩᱫᱤ (use_alternate_screen_buffer)
	ᱨᱩᱣᱟᱹᱲ TRUE;

    /*
     * Restore the screen buffer contents.
     */
    ᱡᱩᱫᱤ (cb->Buffer != NULL)
    {
	for (i = 0; i < cb->NumRegions; i++)
	{
	    BufferCoord.X = cb->Regions[i].Left;
	    BufferCoord.Y = cb->Regions[i].Top;
	    WriteRegion = cb->Regions[i];
	    ᱡᱩᱫᱤ (!WriteConsoleOutputW(g_hConOut,	// output handle
			cb->Buffer,		// our buffer
			cb->BufferSize,		// dimensions of our buffer
			BufferCoord,		// offset in our buffer
			&WriteRegion))		// region to restore
		ᱨᱩᱣᱟᱹᱲ FALSE;
	}
    }

    ᱨᱩᱣᱟᱹᱲ TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
static ConsoleBuffer g_cbOrig = { 0 };
# endif
static ConsoleBuffer g_cbNonTermcap = { 0 };
static ConsoleBuffer g_cbTermcap = { 0 };

var_na g_szOrigTitle[256] = { 0 };
HWND g_hWnd = NULL;	// also used in os_mswin.c
static HICON g_hOrigIconSmall = NULL;
static HICON g_hOrigIcon = NULL;
static HICON g_hVimIcon = NULL;
static BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    static BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    ᱡᱩᱫᱤ (hWnd == NULL)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    ᱡᱩᱫᱤ (phIconSmall != NULL)
	*phIconSmall = (HICON)SendMessage(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    ᱡᱩᱫᱤ (phIcon != NULL)
	*phIcon = (HICON)SendMessage(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    ᱨᱩᱣᱟᱹᱲ TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    static BOOL
SetConsoleIcon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    ᱡᱩᱫᱤ (hWnd == NULL)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    ᱡᱩᱫᱤ (hIconSmall != NULL)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    ᱡᱩᱫᱤ (hIcon != NULL)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    ᱨᱩᱣᱟᱹᱲ TRUE;
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
    static void
SaveConsoleTitleAndIcon(void)
{
    // Save the original title.
    ᱡᱩᱫᱤ (!GetConsoleTitle(g_szOrigTitle, parimaa_na(g_szOrigTitle)))
	ᱨᱩᱣᱟᱹᱲ;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = GetConsoleWindow();
    ᱡᱩᱫᱤ (g_hWnd == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    ᱡᱩᱫᱤ (g_hOrigIconSmall == NULL || g_hOrigIcon == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    // Extract the first icon contained in the Vim executable.
    ᱡᱩᱫᱤ (
# ifdef FEAT_LIBCALL
	    mch_icon_load((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == NULL)
	g_hVimIcon = ExtractIcon(NULL, (LPCSTR)exe_name, 0);
    ᱡᱩᱫᱤ (g_hVimIcon != NULL)
	g_fCanChangeIcon = TRUE;
}

static puur_naa_nka g_fWindInitCalled = FALSE;
static puur_naa_nka g_fTermcapMode = FALSE;
static CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    static void
mch_init_c(void)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_BUFFER_INFO csbi;
# endif
# ifndef __MINGW32__
    extern puur_naa_nka _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_BINARY;		// we do our own CR-LF translation
    out_flush();

    // Obtain handles for the standard Console I/O devices
    ᱡᱩᱫᱤ (read_cmd_fd == 0)
	g_hConIn =  GetStdHandle(STD_INPUT_HANDLE);
    ᱵᱟᱝ
	create_conin();
    g_hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    SaveConsoleBuffer(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.wAttributes;
# ᱵᱟᱝ
    // Get current text attributes
    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
# endif
    ᱡᱩᱫᱤ (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (g_attrCurrent & 0xf) + 1;
    ᱡᱩᱫᱤ (cterm_normal_bg_color == 0)
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
    ᱡᱩᱫᱤ (g_fCanChangeIcon)
	SetConsoleIcon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    ᱡᱩᱫᱤ (use_alternate_screen_buffer)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    ᱡᱩᱫᱤ (fdDump)
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
    static void
mch_exit_c(puur_naa_nka r)
{
    // Copy flag since stoptermcap() will clear the flag.
    puur_naa_nka fTermcapMode = g_fTermcapMode;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    ᱡᱩᱫᱤ (!fTermcapMode &&  use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    ᱡᱩᱫᱤ (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    ᱡᱩᱫᱤ (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	ᱡᱩᱫᱤ (g_fCanChangeIcon && !g_fForceExit)
	    SetConsoleIcon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	ᱡᱩᱫᱤ (fdDump)
	{
	    time_t t;

	    time(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = NULL;
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

    void
mch_init(void)
{
#ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.starting)
	mch_init_g();
    ᱵᱟᱝ
	mch_init_c();
#elif defined(FEAT_GUI_MSWIN)
    mch_init_g();
#ᱵᱟᱝ
    mch_init_c();
#endif
}

    void
mch_exit(puur_naa_nka r)
{
#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef VIMDLL
    ᱡᱩᱫᱤ (vimrun_path_allocated)
	vim_free(vimrun_path.string);

    ᱡᱩᱫᱤ (gui.in_use || gui.starting)
	mch_exit_g(r);
    ᱵᱟᱝ
	mch_exit_c(r);
#elif defined(FEAT_GUI_MSWIN)
    ᱡᱩᱫᱤ (vimrun_path_allocated)
	vim_free(vimrun_path.string);
    mch_exit_g(r);
#ᱵᱟᱝ
    mch_exit_c(r);
#endif
}

/*
 * Do we have an interactive window?
 */
    puur_naa_nka
mch_check_win(
    puur_naa_nka argc UNUSED,
    var_na **argv UNUSED)
{
    mch_get_exe_name();

#ᱡᱩᱫᱤ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    ᱨᱩᱣᱟᱹᱲ OK;	    // GUI always has a tty
#ᱵᱟᱝ
# ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ OK;
# endif
    ᱡᱩᱫᱤ (isatty(1))
	ᱨᱩᱣᱟᱹᱲ OK;
    ᱨᱩᱣᱟᱹᱲ FAIL;
#endif
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    void
fname_case(
    char_u	*name,
    puur_naa_nka		len)
{
    WCHAR   *p;
    WCHAR   buf[_MAX_PATH + 1];

    ᱡᱩᱫᱤ (*name == NUL)
	ᱨᱩᱣᱟᱹᱲ;

    slash_adjust(name);

    p = enc_to_utf16(name, NULL);
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (GetLongPathNameW(p, buf, _MAX_PATH))
    {
	char_u	*q = utf16_to_enc(buf, NULL);

	ᱡᱩᱫᱤ (q != NULL)
	{
	    ᱡᱩᱫᱤ (len > 0)
		vim_strncpy(name, q, len - 1);
	    ᱵᱟᱝ
	    {
		size_t  namelen = STRLEN(name);

		ᱡᱩᱫᱤ (namelen >= STRLEN(q))
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
    puur_naa_nka
mch_get_user_name(
    char_u  *s,
    puur_naa_nka	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_LENGTH(wszUserName);

    ᱡᱩᱫᱤ (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_to_enc(wszUserName, NULL);

	ᱡᱩᱫᱤ (p != NULL)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    ᱨᱩᱣᱟᱹᱲ OK;
	}
    }
    s[0] = NUL;
    ᱨᱩᱣᱟᱹᱲ FAIL;
}


/*
 * Insert host name in s[len].
 */
    void
mch_get_host_name(
    char_u	*s,
    puur_naa_nka		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_LENGTH(wszHostName);

    ᱡᱩᱫᱤ (!GetComputerNameW(wszHostName, &wcch))
	ᱨᱩᱣᱟᱹᱲ;

    char_u  *p = utf16_to_enc(wszHostName, NULL);
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    long
mch_get_pid(void)
{
    ᱨᱩᱣᱟᱹᱲ (long)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    puur_naa_nka
mch_process_running(long pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   status = 0;
    puur_naa_nka	    ret = FALSE;

    ᱡᱩᱫᱤ (hProcess == NULL)
	ᱨᱩᱣᱟᱹᱲ FALSE;  // might not have access
    ᱡᱩᱫᱤ (GetExitCodeProcess(hProcess, &status))
	ret = status == STILL_ACTIVE;
    CloseHandle(hProcess);
    ᱨᱩᱣᱟᱹᱲ ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    puur_naa_nka
mch_dirname(
    char_u	*buf,
    puur_naa_nka		len)
{
    WCHAR   wbuf[_MAX_PATH + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    ᱡᱩᱫᱤ (GetCurrentDirectoryW(_MAX_PATH, wbuf) == 0)
	ᱨᱩᱣᱟᱹᱲ FAIL;

    WCHAR   wcbuf[_MAX_PATH + 1];
    char_u  *p = NULL;

    ᱡᱩᱫᱤ (GetLongPathNameW(wbuf, wcbuf, _MAX_PATH) != 0)
    {
	p = utf16_to_enc(wcbuf, NULL);
	ᱡᱩᱫᱤ (p != NULL)
	{
	    ᱡᱩᱫᱤ (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    ᱡᱩᱫᱤ (p == NULL)
	p = utf16_to_enc(wbuf, NULL);

    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    ᱨᱩᱣᱟᱹᱲ OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    long
mch_getperm(char_u *name)
{
    stat_T	st;

    ᱨᱩᱣᱟᱹᱲ (mch_stat((var_na *)name, &st) == 0)
	? (long)(unsigned short)st.st_mode
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    puur_naa_nka
mch_setperm(char_u *name, long perm)
{
    long	n;
    WCHAR	*p;

    p = enc_to_utf16(name, NULL);
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    ᱡᱩᱫᱤ (n == -1)
	ᱨᱩᱣᱟᱹᱲ FAIL;

    win32_set_archive(name);

    ᱨᱩᱣᱟᱹᱲ OK;
}

/*
 * Set hidden flag for "name".
 */
    void
mch_hide(char_u *name)
{
    puur_naa_nka attrs = win32_getattrs(name);
    ᱡᱩᱫᱤ (attrs == -1)
	ᱨᱩᱣᱟᱹᱲ;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    puur_naa_nka
mch_ishidden(char_u *name)
{
    puur_naa_nka f = win32_getattrs(name);

    ᱡᱩᱫᱤ (f == -1)
	ᱨᱩᱣᱟᱹᱲ FALSE;		    // file does not exist at all

    ᱨᱩᱣᱟᱹᱲ (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    puur_naa_nka
mch_isdir(char_u *name)
{
    puur_naa_nka f = win32_getattrs(name);

    ᱡᱩᱫᱤ (f == -1)
	ᱨᱩᱣᱟᱹᱲ FALSE;		    // file does not exist at all

    ᱨᱩᱣᱟᱹᱲ (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    puur_naa_nka
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_to_utf16(name, NULL);
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ FALSE;
    attr = GetFileAttributesW(p);
    ᱡᱩᱫᱤ (attr == INVALID_FILE_ATTRIBUTES)
	error = GetLastError();
    vim_free(p);

    ᱡᱩᱫᱤ (attr == INVALID_FILE_ATTRIBUTES)
	ᱨᱩᱣᱟᱹᱲ error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    ᱨᱩᱣᱟᱹᱲ (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    puur_naa_nka
mch_isrealdir(char_u *name)
{
    ᱨᱩᱣᱟᱹᱲ mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    puur_naa_nka
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    puur_naa_nka	    retval;

    p = enc_to_utf16(name, NULL);
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ -1;
    retval = _wmkdir(p);
    vim_free(p);
    ᱨᱩᱣᱟᱹᱲ retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    puur_naa_nka
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    puur_naa_nka	    retval;

    p = enc_to_utf16(name, NULL);
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ -1;
    retval = _wrmdir(p);
    vim_free(p);
    ᱨᱩᱣᱟᱹᱲ retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    puur_naa_nka
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    ᱨᱩᱣᱟᱹᱲ win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.nNumberOfLinks > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    puur_naa_nka
mch_is_symbolic_link(char_u *name)
{
    HANDLE		hFind;
    puur_naa_nka			res = FALSE;
    DWORD		fileFlags = 0, reparseTag = 0;
    WCHAR		*wn;
    WIN32_FIND_DATAW	findDataW;

    wn = enc_to_utf16(name, NULL);
    ᱡᱩᱫᱤ (wn == NULL)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    hFind = FindFirstFileW(wn, &findDataW);
    vim_free(wn);
    ᱡᱩᱫᱤ (hFind != INVALID_HANDLE_VALUE)
    {
	fileFlags = findDataW.dwFileAttributes;
	reparseTag = findDataW.dwReserved0;
	FindClose(hFind);
    }

    ᱡᱩᱫᱤ ((fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
	    && (reparseTag == IO_REPARSE_TAG_SYMLINK
		|| reparseTag == IO_REPARSE_TAG_MOUNT_POINT))
	res = TRUE;

    ᱨᱩᱣᱟᱹᱲ res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    puur_naa_nka
mch_is_linked(char_u *fname)
{
    ᱡᱩᱫᱤ (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	ᱨᱩᱣᱟᱹᱲ TRUE;
    ᱨᱩᱣᱟᱹᱲ FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    puur_naa_nka
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    puur_naa_nka		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_to_utf16(fname, NULL);
    ᱡᱩᱫᱤ (wn == NULL)
	ᱨᱩᱣᱟᱹᱲ FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    NULL,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    NULL);		// handle to template file
    vim_free(wn);

    ᱡᱩᱫᱤ (hFile == INVALID_HANDLE_VALUE)
	ᱨᱩᱣᱟᱹᱲ FILEINFO_READ_FAIL;

    ᱡᱩᱫᱤ (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    ᱵᱟᱝ
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    ᱨᱩᱣᱟᱹᱲ res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    static puur_naa_nka
win32_getattrs(char_u *name)
{
    puur_naa_nka		attr;
    WCHAR	*p;

    p = enc_to_utf16(name, NULL);
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ INVALID_FILE_ATTRIBUTES;

    attr = GetFileAttributesW(p);
    vim_free(p);

    ᱨᱩᱣᱟᱹᱲ attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    static puur_naa_nka
win32_setattrs(char_u *name, puur_naa_nka attrs)
{
    puur_naa_nka	    res;
    WCHAR   *p;

    p = enc_to_utf16(name, NULL);
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ -1;

    res = SetFileAttributesW(p, attrs);
    vim_free(p);

    ᱨᱩᱣᱟᱹᱲ res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    static puur_naa_nka
win32_set_archive(char_u *name)
{
    puur_naa_nka attrs = win32_getattrs(name);
    ᱡᱩᱫᱤ (attrs == -1)
	ᱨᱩᱣᱟᱹᱲ -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    ᱨᱩᱣᱟᱹᱲ win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    puur_naa_nka
mch_writable(char_u *name)
{
    puur_naa_nka attrs = win32_getattrs(name);

    ᱨᱩᱣᱟᱹᱲ (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    puur_naa_nka
mch_can_exe(char_u *name, char_u **path, puur_naa_nka use_path UNUSED)
{
    ᱨᱩᱣᱟᱹᱲ executable_exists((var_na *)name, STRLEN(name), path, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    puur_naa_nka
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    puur_naa_nka		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    ᱡᱩᱫᱤ (STRNCMP(name, "\\\\.\\", 4) == 0)
	ᱨᱩᱣᱟᱹᱲ NODE_WRITABLE;

    wn = enc_to_utf16(name, NULL);
    ᱡᱩᱫᱤ (wn == NULL)
	ᱨᱩᱣᱟᱹᱲ NODE_NORMAL;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    NULL,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    NULL);		    // handle to template file
    vim_free(wn);
    ᱡᱩᱫᱤ (hFile == INVALID_HANDLE_VALUE)
	ᱨᱩᱣᱟᱹᱲ NODE_NORMAL;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    ᱡᱩᱫᱤ (type == FILE_TYPE_CHAR)
	ᱨᱩᱣᱟᱹᱲ NODE_WRITABLE;
    ᱡᱩᱫᱤ (type == FILE_TYPE_DISK)
	ᱨᱩᱣᱟᱹᱲ NODE_NORMAL;
    ᱨᱩᱣᱟᱹᱲ NODE_OTHER;
}

#ifdef HAVE_ACL
sa_nracanaa my_acl
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
    ᱨᱩᱣᱟᱹᱲ (vim_acl_T)NULL;
#ᱵᱟᱝ
    sa_nracanaa my_acl   *p = NULL;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(sa_nracanaa my_acl);
    ᱡᱩᱫᱤ (p != NULL)
    {
	WCHAR	*wn;

	wn = enc_to_utf16(fname, NULL);
	ᱡᱩᱫᱤ (wn == NULL)
	{
	    vim_free(p);
	    ᱨᱩᱣᱟᱹᱲ NULL;
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
	ᱡᱩᱫᱤ (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (void)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    NULL,
		    NULL,
		    &p->pDacl,
		    NULL,
		    &p->pSecurityDescriptor);
	}
	ᱡᱩᱫᱤ (p->pSecurityDescriptor == NULL)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = NULL;
	}
	vim_free(wn);
    }

    ᱨᱩᱣᱟᱹᱲ (vim_acl_T)p;
#endif
}

#ifdef HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    static BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_SIZE_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.AceCount = 0;
    GetAclInformation(acl, &acl_info, parimaa_na(acl_info), AclSizeInformation);
    for (i = 0; i < acl_info.AceCount; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	ᱡᱩᱫᱤ (ace->Header.AceFlags & INHERITED_ACE)
	    ᱨᱩᱣᱟᱹᱲ TRUE;
    }
    ᱨᱩᱣᱟᱹᱲ FALSE;
}
#endif

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    void
mch_set_acl(char_u *fname, vim_acl_T acl)
{
#ifdef HAVE_ACL
    sa_nracanaa my_acl   *p = (sa_nracanaa my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    wn = enc_to_utf16(fname, NULL);
    ᱡᱩᱫᱤ (wn == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    // Set security flags
    ᱡᱩᱫᱤ (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    ᱡᱩᱫᱤ (p->pSidGroup)
	sec_info |= GROUP_SECURITY_INFORMATION;
    ᱡᱩᱫᱤ (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	ᱡᱩᱫᱤ (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    ᱡᱩᱫᱤ (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (void)SetNamedSecurityInfoW(
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

    void
mch_free_acl(vim_acl_T acl)
{
#ifdef HAVE_ACL
    sa_nracanaa my_acl   *p = (sa_nracanaa my_acl *)acl;

    ᱡᱩᱫᱤ (p != NULL)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#endif
}

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    static BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    INPUT_RECORD ir;
    DWORD out;

    switch (dwCtrlType)
    {
    pak_sa CTRL_C_EVENT:
	ᱡᱩᱫᱤ (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	ᱨᱩᱣᱟᱹᱲ TRUE;

    pak_sa CTRL_BREAK_EVENT:
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
	ᱨᱩᱣᱟᱹᱲ TRUE;

    // fatal events: shut down gracefully
    pak_sa CTRL_CLOSE_EVENT:
    pak_sa CTRL_LOGOFF_EVENT:
    pak_sa CTRL_SHUTDOWN_EVENT:
	windgoto((puur_naa_nka)Rows - 1, cmdline_col_off);
	g_fForceExit = TRUE;

	vim_snprintf((var_na *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	ᱨᱩᱣᱟᱹᱲ TRUE;		// not reached

    default:
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    void
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ;
# endif
    GetConsoleMode(g_hConIn, &cmodein);
    GetConsoleMode(g_hConOut, &cmodeout);
    ᱡᱩᱫᱤ (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT |
		     ENABLE_ECHO_INPUT);
	ᱡᱩᱫᱤ (g_fMouseActive)
	{
	    cmodein |= ENABLE_MOUSE_INPUT;
	    cmodein &= ~ENABLE_QUICK_EDIT_MODE;
	}
	ᱵᱟᱝ
	{
	    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# ᱵᱟᱝ
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    ᱵᱟᱝ // cooked
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
    ᱡᱩᱫᱤ (fdDump)
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
    puur_naa_nka
mch_get_shellsize(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

# ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ OK;
# endif
    ᱡᱩᱫᱤ (!g_fTermcapMode && g_cbTermcap.IsValid)
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
    ᱵᱟᱝ ᱡᱩᱫᱤ (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	Rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	Columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    ᱵᱟᱝ
    {
	Rows = 25;
	Columns = 80;
    }
    ᱨᱩᱣᱟᱹᱲ OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    static void
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    ᱡᱩᱫᱤ (use_alternate_screen_buffer)
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (!SetConsoleScreenBufferSize(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	ᱡᱩᱫᱤ (fdDump)
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
    static void
ResizeWindow(
    HANDLE     hConsole,
    SMALL_RECT srWindowRect)
{
    ᱡᱩᱫᱤ (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
    {
# ifdef MCH_WRITE_DUMP
	ᱡᱩᱫᱤ (fdDump)
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
    static void
ResizeConBufAndWindow(
    HANDLE  hConsole,
    puur_naa_nka	    xSize,
    puur_naa_nka	    ySize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    srWindowRect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    static puur_naa_nka	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    ᱡᱩᱫᱤ (fdDump)
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

    ᱡᱩᱫᱤ (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	puur_naa_nka sx, sy;

	sx = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	sy = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	ᱡᱩᱫᱤ (sy < ySize || sx < xSize)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    ᱡᱩᱫᱤ (sy < ySize)
		coordScreen.Y = ySize;
	    ᱵᱟᱝ
		coordScreen.Y = sy;
	    ᱡᱩᱫᱤ (sx < xSize)
		coordScreen.X = xSize;
	    ᱵᱟᱝ
		coordScreen.X = sx;
	    SetConsoleScreenBufferSize(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = xSize;
    coordScreen.Y = ySize;

    // In the new console call API, only the first time in reverse order
    ᱡᱩᱫᱤ (!vtp_working || resized)
    {
	ResizeWindow(hConsole, srWindowRect);
	ResizeConBuf(hConsole, coordScreen);
    }
    ᱵᱟᱝ
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
    void
mch_set_shellsize(void)
{
    COORD coordScreen;

# ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ;
# endif
    // Don't change window size while still starting up
    ᱡᱩᱫᱤ (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	ᱨᱩᱣᱟᱹᱲ;
    }

    ᱡᱩᱫᱤ (term_console)
    {
	coordScreen = GetLargestConsoleWindowSize(g_hConOut);

	// Clamp Rows and Columns to reasonable values
	ᱡᱩᱫᱤ (Rows > coordScreen.Y)
	    Rows = coordScreen.Y;
	ᱡᱩᱫᱤ (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	ResizeConBufAndWindow(g_hConOut, Columns, Rows);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    void
mch_new_shellsize(void)
{
# ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ;
# endif
    set_scroll_region(0, 0, Columns - 1, Rows - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    void
mch_set_winsize_now(void)
{
    ᱡᱩᱫᱤ (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_set_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# ᱡᱩᱫᱤ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    void
mch_calc_cell_size(sa_nracanaa cellsize *cs_out)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_out->cs_xpixel = -1;
    cs_out->cs_ypixel = -1;

    ᱡᱩᱫᱤ (g_hConOut == INVALID_HANDLE_VALUE)
	ᱨᱩᱣᱟᱹᱲ;

    cfi.cbSize = parimaa_na(cfi);
    ᱡᱩᱫᱤ (GetCurrentConsoleFontEx(g_hConOut, FALSE, &cfi)
	    && cfi.dwFontSize.X > 0 && cfi.dwFontSize.Y > 0)
    {
	cs_out->cs_xpixel = cfi.dwFontSize.X;
	cs_out->cs_ypixel = cfi.dwFontSize.Y;
	ᱨᱩᱣᱟᱹᱲ;
    }

    GetConsoleMode(g_hConIn, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT
	    | ENABLE_PROCESSED_INPUT);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_INPUT;

    ᱡᱩᱫᱤ (SetConsoleMode(g_hConIn, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	SetConsoleMode(g_hConIn, cmodeold);
    }
}
# endif

#endif

    static BOOL
vim_create_process(
    var_na		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    var_na		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = NULL;

    wcmd = enc_to_utf16((char_u *)cmd, NULL);
    ᱡᱩᱫᱤ (wcmd == NULL)
	ᱨᱩᱣᱟᱹᱲ FALSE;
    ᱡᱩᱫᱤ (cwd != NULL)
    {
	wcwd = enc_to_utf16((char_u *)cwd, NULL);
	ᱡᱩᱫᱤ (wcwd == NULL)
	    goto theend;
    }

    ret = CreateProcessW(
	    NULL,		// Executable name
	    wcmd,		// Command to execute
	    NULL,		// Process security attributes
	    NULL,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    ᱨᱩᱣᱟᱹᱲ ret;
}


    static HINSTANCE
vim_shell_execute(
    var_na *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_to_utf16((char_u *)cmd, NULL);
    ᱡᱩᱫᱤ (wcmd == NULL)
	ᱨᱩᱣᱟᱹᱲ (HINSTANCE) 0;

    ret = ShellExecuteW(NULL, NULL, wcmd, NULL, NULL, n_show_cmd);
    vim_free(wcmd);
    ᱨᱩᱣᱟᱹᱲ ret;
}


#ᱡᱩᱫᱤ defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    static puur_naa_nka
mch_system_classic(var_na *cmd, puur_naa_nka options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = parimaa_na(si);
    si.lpReserved = NULL;
    si.lpDesktop = NULL;
    si.lpTitle = NULL;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    ᱡᱩᱫᱤ (options & SHELL_DOOUT)
	si.wShowWindow = SW_SHOWMINNOACTIVE;
    ᱵᱟᱝ
	si.wShowWindow = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = NULL;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_MODE |	CREATE_NEW_CONSOLE,
	    &si, &pi, NULL, NULL);

    // Wait for the command to terminate before continuing
    {
	puur_naa_nka	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	for (;;)
	{
	    MSG	msg;

	    ᱡᱩᱫᱤ (PeekMessageW(&msg, (HWND)NULL, 0, 0, PM_REMOVE))
	    {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
		delay = 1;
		continue;
	    }
	    ᱡᱩᱫᱤ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		bha_nga;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    ᱡᱩᱫᱤ (delay < 50)
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

    ᱨᱩᱣᱟᱹᱲ ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    static unsigned puur_naa_nka __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_hChildStd_IN_Wr = param;
    linenr_T	    lnum = curbuf->b_op_start.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    puur_naa_nka		    written = 0;

    for (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	ᱡᱩᱫᱤ (l == 0)
	    len = 0;
	ᱵᱟᱝ ᱡᱩᱫᱤ (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_hChildStd_IN_Wr, "", 1, &len, NULL);
	}
	ᱵᱟᱝ
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_hChildStd_IN_Wr, (var_na *)lp + written,
		      s == NULL ? l : (DWORD)(s - (lp + written)),
		      &len, NULL);
	}
	ᱡᱩᱫᱤ (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    ᱡᱩᱫᱤ (lnum != curbuf->b_op_end.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_line_count
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_hChildStd_IN_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, NULL);
	    }

	    ++lnum;
	    ᱡᱩᱫᱤ (lnum > curbuf->b_op_end.lnum)
		bha_nga;

	    lp = ml_get(lnum);
	    written = 0;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_hChildStd_IN_Wr);
    ᱨᱩᱣᱟᱹᱲ 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    static void
dump_pipe(puur_naa_nka	    options,
	  HANDLE    g_hChildStd_OUT_Rd,
	  garray_T  *ga,
	  char_u    buffer[],
	  DWORD	    *buffer_off)
{
    DWORD	availableBytes = 0;
    DWORD	i;
    puur_naa_nka		ret;
    DWORD	len;
    DWORD	toRead;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_hChildStd_OUT_Rd, // pipe to query
			NULL,		    // optional buffer
			0,		    // buffer size
			NULL,		    // number of read bytes
			&availableBytes,    // available bytes total
			NULL);		    // byteLeft

    // We got real data in the pipe, read it
    ᱡᱚᱠᱷᱚᱱ (ret != 0 && availableBytes > 0)
    {
	toRead = (DWORD)(BUFLEN - *buffer_off);
	toRead = availableBytes < toRead ? availableBytes : toRead;
	ReadFile(g_hChildStd_OUT_Rd, buffer + *buffer_off, toRead , &len, NULL);

	// If we haven't read anything, there is a problem
	ᱡᱩᱫᱤ (len == 0)
	    bha_nga;

	availableBytes -= len;

	ᱡᱩᱫᱤ (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    for (i = 0; i < len; ++i)
	    {
		ᱡᱩᱫᱤ (buffer[i] == NL)
		    append_ga_line(ga);
		ᱵᱟᱝ ᱡᱩᱫᱤ (buffer[i] == NUL)
		    ga_append(ga, NL);
		ᱵᱟᱝ
		    ga_append(ga, buffer[i]);
	    }
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (has_mbyte)
	{
	    puur_naa_nka		l;
	    puur_naa_nka		c;
	    char_u	*p;

	    len += *buffer_off;
	    buffer[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    for (p = buffer; p < buffer + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		ᱡᱩᱫᱤ (l == 0)
		    l = 1;  // NUL byte?
		ᱵᱟᱝ ᱡᱩᱫᱤ (MB_BYTE2LEN(*p) != l)
		    bha_nga;
	    }
	    ᱡᱩᱫᱤ (p == buffer)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		ᱡᱩᱫᱤ (len >= 12)
		    ++p;
		ᱵᱟᱝ
		{
		    *buffer_off = len;
		    ᱨᱩᱣᱟᱹᱲ;
		}
	    }
	    c = *p;
	    *p = NUL;
	    msg_puts((var_na *)buffer);
	    ᱡᱩᱫᱤ (p < buffer + len)
	    {
		*p = c;
		*buffer_off = (DWORD)((buffer + len) - p);
		mch_memmove(buffer, p, *buffer_off);
		ᱨᱩᱣᱟᱹᱲ;
	    }
	    *buffer_off = 0;
	}
	ᱵᱟᱝ
	{
	    buffer[len] = NUL;
	    msg_puts((var_na *)buffer);
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
    static puur_naa_nka
mch_system_piped(var_na *cmd, puur_naa_nka options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_hChildStd_IN_Rd = NULL;
    HANDLE g_hChildStd_IN_Wr = NULL;
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;

    char_u	buffer[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    puur_naa_nka		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    puur_naa_nka		noread_cnt = 0;
    garray_T	ga;
    puur_naa_nka		delay = 1;
    DWORD	buffer_off = 0;	// valid bytes in buffer[]
    var_na	*p = NULL;

    SECURITY_ATTRIBUTES saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = parimaa_na(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    ᱡᱩᱫᱤ ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)
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

    si.cb = parimaa_na(si);
    si.lpReserved = NULL;
    si.lpDesktop = NULL;
    si.lpTitle = NULL;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_hChildStd_OUT_Wr;
    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.hStdInput = g_hChildStd_IN_Rd;
    si.wShowWindow = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = NULL;

    ᱡᱩᱫᱤ (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    ᱡᱩᱫᱤ (cmd != NULL)
    {
	p = (var_na *)vim_strsave((char_u *)cmd);
	ᱡᱩᱫᱤ (p != NULL)
	    unescape_shellxquote((char_u *)p, p_sxe);
	ᱵᱟᱝ
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_MODE,
						&si, &pi, NULL, NULL);

    ᱡᱩᱫᱤ (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(g_hChildStd_OUT_Wr);

    ᱡᱩᱫᱤ (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(NULL,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_hChildStd_IN_Wr,  // parameter
			0,		 // creation flag, start immediately
			NULL);		    // we don't care about thread id
	CloseHandle(thread);
	g_hChildStd_IN_Wr = NULL;
    }

    // Keep updating the window while waiting for the shell to finish.
    for (;;)
    {
	MSG	msg;

	ᱡᱩᱫᱤ (PeekMessageW(&msg, (HWND)NULL, 0, 0, PM_REMOVE))
	{
	    TranslateMessage(&msg);
	    DispatchMessageW(&msg);
	}

	// write pipe information in the window
	ᱡᱩᱫᱤ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.in_use
# endif
	    )
	{
	    len = 0;
	    ᱡᱩᱫᱤ (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		ᱡᱩᱫᱤ (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		ᱡᱩᱫᱤ (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    ᱡᱩᱫᱤ (len == 1 && cmd != NULL)
		    {
			ᱡᱩᱫᱤ (ta_buf[ta_len] == Ctrl_C)
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
		    ᱡᱩᱫᱤ ((len == 1 || len == 4 ) && cmd != NULL)
		    {
			ᱡᱩᱫᱤ (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_hChildStd_IN_Wr);
			    g_hChildStd_IN_Wr = NULL;
			    len = 0;
			}
		    }

		    len = term_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    for (i = ta_len; i < ta_len + len; ++i)
		    {
			ᱡᱩᱫᱤ (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    msg_putchar(ta_buf[i]);
			ᱵᱟᱝ ᱡᱩᱫᱤ (has_mbyte)
			{
			    puur_naa_nka l = (*mb_ptr2len)(ta_buf + i);

			    msg_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			ᱵᱟᱝ
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
		    ᱡᱩᱫᱤ (options & SHELL_WRITE)
			ta_len = 0;
		    ᱵᱟᱝ ᱡᱩᱫᱤ (g_hChildStd_IN_Wr != NULL)
		    {
			WriteFile(g_hChildStd_IN_Wr, (var_na*)ta_buf,
				    1, &len, NULL);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			ᱡᱩᱫᱤ (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	ᱡᱩᱫᱤ (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	ᱡᱩᱫᱤ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);
	    bha_nga;
	}

	++noread_cnt;
	dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	ᱡᱩᱫᱤ (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_hChildStd_OUT_Rd);
    ᱡᱩᱫᱤ (g_hChildStd_IN_Wr != NULL)
	CloseHandle(g_hChildStd_IN_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    GetExitCodeProcess(pi.hProcess, &ret);

    ᱡᱩᱫᱤ (options & SHELL_READ)
    {
	ᱡᱩᱫᱤ (ga.ga_len > 0)
	{
	    append_ga_line(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	ᱵᱟᱝ
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    ᱨᱩᱣᱟᱹᱲ ret;
}

    static puur_naa_nka
mch_system_g(var_na *cmd, puur_naa_nka options)
{
    // if we can pipe and the shelltemp option is off
    ᱡᱩᱫᱤ (!p_stmp)
	ᱨᱩᱣᱟᱹᱲ mch_system_piped(cmd, options);
    ᱵᱟᱝ
	ᱨᱩᱣᱟᱹᱲ mch_system_classic(cmd, options);
}
#endif

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    static puur_naa_nka
mch_system_c(var_na *cmd, puur_naa_nka options UNUSED)
{
    puur_naa_nka		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    ᱡᱩᱫᱤ (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	ᱡᱩᱫᱤ (buf == NULL)
	    ᱨᱩᱣᱟᱹᱲ -1;
	vim_snprintf((var_na *)buf, len, "(%s)", cmd);
	wcmd = enc_to_utf16(buf, NULL);
	free(buf);
    }
    ᱵᱟᱝ
	wcmd = enc_to_utf16((char_u *)cmd, NULL);

    ᱡᱩᱫᱤ (wcmd == NULL)
	ᱨᱩᱣᱟᱹᱲ -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    ᱨᱩᱣᱟᱹᱲ ret;
}

#endif

    static puur_naa_nka
mch_system(var_na *cmd, puur_naa_nka options)
{
#ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use || gui.starting)
	ᱨᱩᱣᱟᱹᱲ mch_system_g(cmd, options);
    ᱵᱟᱝ
	ᱨᱩᱣᱟᱹᱲ mch_system_c(cmd, options);
#elif defined(FEAT_GUI_MSWIN)
    ᱨᱩᱣᱟᱹᱲ mch_system_g(cmd, options);
#ᱵᱟᱝ
    ᱨᱩᱣᱟᱹᱲ mch_system_c(cmd, options);
#endif
}

#ᱡᱩᱫᱤ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    static puur_naa_nka
mch_call_shell_terminal(
    char_u	*cmd,
    puur_naa_nka		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = NULL;
    typval_T	argvar[2];
    long_u	cmdlen;
    puur_naa_nka		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    ᱡᱩᱫᱤ (cmd == NULL)
	cmdlen = STRLEN(p_sh) + 1;
    ᱵᱟᱝ
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    ᱡᱩᱫᱤ (newcmd == NULL)
	ᱨᱩᱣᱟᱹᱲ 255;
    ᱡᱩᱫᱤ (cmd == NULL)
    {
	STRCPY(newcmd, p_sh);
	ch_log(NULL, "starting terminal to run a shell");
    }
    ᱵᱟᱝ
    {
	vim_snprintf((var_na *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(NULL, "starting terminal for system command '%s'", cmd);
    }

    init_job_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = term_start(argvar, NULL, &opt, TERM_START_SYSTEM);
    ᱡᱩᱫᱤ (buf == NULL)
    {
	vim_free(newcmd);
	ᱨᱩᱣᱟᱹᱲ 255;
    }

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    ᱡᱩᱫᱤ (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	ᱡᱚᱠᱷᱚᱱ (term_use_loop())
	{
	    ᱡᱩᱫᱤ (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		ᱡᱩᱫᱤ (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    ᱵᱟᱝ
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(NULL, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    ᱨᱩᱣᱟᱹᱲ retval;
}
#endif
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    static void
restore_env_var(char_u *name, char_u *old_value, puur_naa_nka must_free)
{
    ᱡᱩᱫᱤ (old_value != NULL)
    {
	vim_setenv(name, old_value);
	ᱡᱩᱫᱤ (must_free)
	    vim_free(old_value);
	ᱨᱩᱣᱟᱹᱲ;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    puur_naa_nka
mch_call_shell(
    char_u  *cmd,
    puur_naa_nka	    options)	// SHELL_*, see vim.h
{
    puur_naa_nka		x = 0;
    puur_naa_nka		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    puur_naa_nka		must_free;
    char_u	*oldval;

#ifdef FEAT_EVAL
    ch_log(NULL, "executing shell command: %s", cmd);
#endif
    // Change the title to reflect that we are in a subshell.
    ᱡᱩᱫᱤ (GetConsoleTitleW(szShellTitle, ARRAY_LENGTH(szShellTitle) - 4) > 0)
    {
	ᱡᱩᱫᱤ (cmd == NULL)
	    wcscat(szShellTitle, L" :sh");
	ᱵᱟᱝ
	{
	    WCHAR *wn = enc_to_utf16((char_u *)cmd, NULL);

	    ᱡᱩᱫᱤ (wn != NULL)
	    {
		wcscat(szShellTitle, L" - !");
		ᱡᱩᱫᱤ ((wcslen(szShellTitle) + wcslen(wn) <
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
    ᱡᱩᱫᱤ (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#endif
#ᱡᱩᱫᱤ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    ᱡᱩᱫᱤ (
# ifdef VIMDLL
	    gui.in_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != NULL
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	ᱡᱩᱫᱤ (cmdbase != NULL)
	    // Skip a leading quote and (.
	    ᱡᱚᱠᱷᱚᱱ (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	ᱡᱩᱫᱤ (cmdbase == NULL || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    ᱨᱩᱣᱟᱹᱲ x;
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

    ᱡᱩᱫᱤ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    ᱡᱩᱫᱤ (cmd == NULL)
    {
	x = mch_system((var_na *)p_sh, options);
    }
    ᱵᱟᱝ
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = NULL;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	ᱡᱩᱫᱤ (*cmdbase == '"' )
	    ++cmdbase;
	ᱡᱩᱫᱤ (*cmdbase == '(')
	    ++cmdbase;

	ᱡᱩᱫᱤ ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    ZeroMemory(&si, parimaa_na(si));
	    si.cb = parimaa_na(si);
	    si.lpReserved = NULL;
	    si.lpDesktop = NULL;
	    si.lpTitle = NULL;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = NULL;

	    cmdbase = skipwhite(cmdbase + 5);
	    ᱡᱩᱫᱤ ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_WINDOW;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdInput = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    NULL,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_NORMAL,		// File att.
		    NULL);				// Temp file
		si.hStdOutput = si.hStdInput;
		si.hStdError = si.hStdInput;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    ᱡᱩᱫᱤ (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		ᱡᱩᱫᱤ (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		ᱡᱩᱫᱤ (p > cmdbase && p[-1] == ')'
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
	    ᱡᱩᱫᱤ (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		ᱡᱩᱫᱤ (cmd_shell == NULL || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		ᱡᱩᱫᱤ (subcmd != NULL)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    ᱡᱩᱫᱤ (newcmd != NULL)
			vim_snprintf((var_na *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    ᱵᱟᱝ
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    ᱡᱩᱫᱤ (vim_create_process((var_na *)newcmd, FALSE, flags,
			&si, &pi, NULL, NULL))
		x = 0;
	    ᱵᱟᱝ ᱡᱩᱫᱤ (vim_shell_execute((var_na *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    ᱵᱟᱝ
	    {
		x = -1;
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
		ᱡᱩᱫᱤ (gui.in_use)
# endif
		    emsg(_(e_command_not_found));
#endif
	    }

	    ᱡᱩᱫᱤ (newcmd != cmdbase)
		vim_free(newcmd);

	    ᱡᱩᱫᱤ (si.dwFlags == STARTF_USESTDHANDLES && si.hStdInput != NULL)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.hStdInput);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	ᱵᱟᱝ
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
	    ᱡᱩᱫᱤ (newcmd != NULL)
	    {
#ᱡᱩᱫᱤ defined(FEAT_GUI_MSWIN)
		ᱡᱩᱫᱤ (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    var_na *msg = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    var_na *title = _("Vim Warning");
		    WCHAR *wmsg = enc_to_utf16((char_u *)msg, NULL);
		    WCHAR *wtitle = enc_to_utf16((char_u *)title, NULL);

		    ᱡᱩᱫᱤ (wmsg != NULL && wtitle != NULL)
			MessageBoxW(NULL, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		ᱡᱩᱫᱤ (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((var_na *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_path.string,
			    (msg_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		ᱵᱟᱝ ᱡᱩᱫᱤ (
# ifdef VIMDLL
			(gui.in_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((var_na *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		ᱵᱟᱝ
#endif
		    vim_snprintf((var_na *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((var_na *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    ᱡᱩᱫᱤ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    ᱡᱩᱫᱤ (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#ᱡᱩᱫᱤ defined(FEAT_GUI_MSWIN)
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

    ᱨᱩᱣᱟᱹᱲ x;
}

#ᱡᱩᱫᱤ defined(FEAT_JOB_CHANNEL)
    static HANDLE
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

    wn = enc_to_utf16(fname, NULL);
    ᱡᱩᱫᱤ (wn == NULL)
	ᱨᱩᱣᱟᱹᱲ INVALID_HANDLE_VALUE;

    h = CreateFileW(wn, dwDesiredAccess, dwShareMode,
	    lpSecurityAttributes, dwCreationDisposition,
	    dwFlagsAndAttributes, NULL);
    vim_free(wn);
    ᱨᱩᱣᱟᱹᱲ h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    void
win32_build_env(dict_T *env, garray_T *gap, puur_naa_nka is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != NULL ? env->dv_hashtab.ht_used : 0;
    LPVOID	base = GetEnvironmentStringsW();

    // for last \0
    ᱡᱩᱫᱤ (ga_grow(gap, 1) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (env != NULL)
    {
	FOR_ALL_HASHTAB_ITEMS(&env->dv_hashtab, hi, todo)
	{
	    ᱡᱩᱫᱤ (!HASHITEM_EMPTY(hi))
	    {
		typval_T *item = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_to_utf16((char_u *)hi->hi_key, NULL);
		WCHAR   *wval = enc_to_utf16(tv_get_string(item), NULL);
		--todo;
		ᱡᱩᱫᱤ (wkey != NULL && wval != NULL)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    ᱡᱩᱫᱤ (ga_grow(gap, (puur_naa_nka)(lkey + lval + 2)) == FAIL)
			continue;
		    for (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'=';
		    for (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    ᱡᱩᱫᱤ (base)
    {
	WCHAR	*p = (WCHAR*) base;

	// for last \0
	ᱡᱩᱫᱤ (ga_grow(gap, 1) == FAIL)
	    ᱨᱩᱣᱟᱹᱲ;

	ᱡᱚᱠᱷᱚᱱ (*p != 0 || *(p + 1) != 0)
	{
	    ᱡᱩᱫᱤ (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_data + gap->ga_len++) = *p;
	    p++;
	}
	FreeEnvironmentStringsW(base);
	*((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
    }

# ᱡᱩᱫᱤ defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
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

	ᱡᱩᱫᱤ (ga_grow(gap, (puur_naa_nka)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    for (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    for (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    ᱡᱩᱫᱤ (is_terminal)
	    {
		for (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_data + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		for (n = 0; n < version_len; n++)
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
    static BOOL
create_pipe_pair(HANDLE handles[2])
{
    static LONG		s;
    var_na		name[64];
    SECURITY_ATTRIBUTES sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_SIZE, 0, 0, NULL);

    ᱡᱩᱫᱤ (handles[1] == INVALID_HANDLE_VALUE)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    sa.nLength = parimaa_na(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    ᱡᱩᱫᱤ (handles[0] == INVALID_HANDLE_VALUE)
    {
	CloseHandle(handles[1]);
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

    ᱨᱩᱣᱟᱹᱲ TRUE;
}

#endif // FEAT_JOB_CHANNEL

#ᱡᱩᱫᱤ defined(FEAT_EVAL)
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
    var_na	**argv,
    char_u	*infile,
    puur_naa_nka		flags UNUSED,
    puur_naa_nka		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;
    HANDLE		hChildStdoutRd = INVALID_HANDLE_VALUE;
    HANDLE		hChildStdoutWr = INVALID_HANDLE_VALUE;
    HANDLE		hChildStdinRd = INVALID_HANDLE_VALUE;
    garray_T		cmd_ga;
    garray_T		out_ga;
    char_u		*buffer = NULL;
    DWORD		exit_code = (DWORD)-1;
    puur_naa_nka			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    for (i = 0; argv[i] != NULL; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	puur_naa_nka	has_spaces = FALSE;
	puur_naa_nka	j;

	for (j = 0; s[j] != NUL; j++)
	    ᱡᱩᱫᱤ (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		bha_nga;
	    }

	ᱡᱩᱫᱤ (i > 0)
	    ga_append(&cmd_ga, ' ');

	ᱡᱩᱫᱤ (has_spaces)
	{
	    puur_naa_nka	num_bs;

	    ga_append(&cmd_ga, '"');
	    for (j = 0; arg[j] != NUL; j++)
	    {
		num_bs = 0;
		ᱡᱚᱠᱷᱚᱱ (arg[j] == '\\')
		{
		    num_bs++;
		    j++;
		}

		ᱡᱩᱫᱤ (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    ᱡᱚᱠᱷᱚᱱ (num_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    bha_nga;
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    ᱡᱚᱠᱷᱚᱱ (num_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		ᱵᱟᱝ
		{
		    ᱡᱚᱠᱷᱚᱱ (num_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	ᱵᱟᱝ
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&out_ga, 1, 4096);

    saAttr.nLength = parimaa_na(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child's stdout.
    ᱡᱩᱫᱤ (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)
	    || !SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	goto done;
    }

    // Set up stdin from infile if provided.
    ᱡᱩᱫᱤ (infile != NULL)
    {
	WCHAR *winfile = enc_to_utf16(infile, NULL);

	ᱡᱩᱫᱤ (winfile != NULL)
	{
	    hChildStdinRd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL, NULL);
	    vim_free(winfile);
	}
    }

    ZeroMemory(&pi, parimaa_na(pi));
    ZeroMemory(&si, parimaa_na(si));
    si.cb = parimaa_na(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;
    si.hStdInput = (hChildStdinRd != INVALID_HANDLE_VALUE)
		    ? hChildStdinRd : INVALID_HANDLE_VALUE;

    ch_log(NULL, "directly executing: %s", (var_na *)cmd_ga.ga_data);

    // Create the child process directly, without going through the shell.
    ᱡᱩᱫᱤ (!vim_create_process((var_na *)cmd_ga.ga_data, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
		&si, &pi, NULL, NULL))
    {
	semsg(_(e_invalid_argument_str), cmd_ga.ga_data);
	goto done;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(hChildStdoutWr);
    hChildStdoutWr = INVALID_HANDLE_VALUE;
    ᱡᱩᱫᱤ (hChildStdinRd != INVALID_HANDLE_VALUE)
    {
	CloseHandle(hChildStdinRd);
	hChildStdinRd = INVALID_HANDLE_VALUE;
    }

    // Read output from child process.
    for (;;)
    {
	var_na	buf[4096];
	DWORD	n;

	ᱡᱩᱫᱤ (!ReadFile(hChildStdoutRd, buf, parimaa_na(buf), &n, NULL) || n == 0)
	    bha_nga;
	ᱡᱩᱫᱤ (ga_grow(&out_ga, (puur_naa_nka)n) == OK)
	{
	    mch_memmove((var_na *)out_ga.ga_data + out_ga.ga_len, buf, n);
	    out_ga.ga_len += (puur_naa_nka)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    set_vim_var_nr(VV_SHELL_ERROR, (long)exit_code);

    ᱡᱩᱫᱤ (out_ga.ga_len > 0)
    {
	buffer = alloc(out_ga.ga_len + 1);
	ᱡᱩᱫᱤ (buffer != NULL)
	{
	    mch_memmove(buffer, out_ga.ga_data, out_ga.ga_len);
	    ᱡᱩᱫᱤ (ret_len == NULL)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		for (i = 0; i < out_ga.ga_len; ++i)
		    ᱡᱩᱫᱤ (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[out_ga.ga_len] = NUL;
	    }
	    ᱵᱟᱝ
		*ret_len = out_ga.ga_len;
	}
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (ret_len != NULL)
	*ret_len = 0;

done:
    ga_clear(&cmd_ga);
    ga_clear(&out_ga);
    ᱡᱩᱫᱤ (hChildStdoutRd != INVALID_HANDLE_VALUE)
	CloseHandle(hChildStdoutRd);
    ᱡᱩᱫᱤ (hChildStdoutWr != INVALID_HANDLE_VALUE)
	CloseHandle(hChildStdoutWr);
    ᱡᱩᱫᱤ (hChildStdinRd != INVALID_HANDLE_VALUE)
	CloseHandle(hChildStdinRd);
    ᱨᱩᱣᱟᱹᱲ buffer;
}
#endif // FEAT_EVAL

#ᱡᱩᱫᱤ defined(FEAT_JOB_CHANNEL)
    void
mch_job_start(var_na *cmd, job_T *job, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_ATTRIBUTES saAttr;
    channel_T		*channel = NULL;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    puur_naa_nka		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    puur_naa_nka		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    puur_naa_nka		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    puur_naa_nka		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    puur_naa_nka		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    puur_naa_nka		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    puur_naa_nka		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;

    ᱡᱩᱫᱤ (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_VALUE;
    ifd[1] = INVALID_HANDLE_VALUE;
    ofd[0] = INVALID_HANDLE_VALUE;
    ofd[1] = INVALID_HANDLE_VALUE;
    efd[0] = INVALID_HANDLE_VALUE;
    efd[1] = INVALID_HANDLE_VALUE;
    ga_init2(&ga, parimaa_na(wchar_t), 500);

    jo = CreateJobObject(NULL, NULL);
    ᱡᱩᱫᱤ (jo == NULL)
    {
	job->jv_status = JOB_FAILED;
	goto failed;
    }

    ᱡᱩᱫᱤ (options->jo_env != NULL)
	win32_build_env(options->jo_env, &ga, FALSE);

    ZeroMemory(&pi, parimaa_na(pi));
    ZeroMemory(&si, parimaa_na(si));
    si.cb = parimaa_na(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    saAttr.nLength = parimaa_na(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    ᱡᱩᱫᱤ (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	ifd[0] = job_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
	ᱡᱩᱫᱤ (ifd[0] == INVALID_HANDLE_VALUE)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    goto failed;
	}
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (!use_null_for_in
	    && (!create_pipe_pair(ifd)
		|| !SetHandleInformation(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	goto failed;

    ᱡᱩᱫᱤ (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	ofd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	ᱡᱩᱫᱤ (ofd[1] == INVALID_HANDLE_VALUE)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    goto failed;
	}
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (!use_null_for_out &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !SetHandleInformation(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	goto failed;

    ᱡᱩᱫᱤ (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	ᱡᱩᱫᱤ (efd[1] == INVALID_HANDLE_VALUE)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    goto failed;
	}
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (!use_out_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !SetHandleInformation(efd[0], HANDLE_FLAG_INHERIT, 0)))
	goto failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_out_for_err ? ofd[1] : efd[1];

    ᱡᱩᱫᱤ (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	ᱡᱩᱫᱤ (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    ᱡᱩᱫᱤ (channel != NULL)
		++channel->ch_refcount;
	}
	ᱵᱟᱝ
	    channel = add_channel();
	ᱡᱩᱫᱤ (channel == NULL)
	    goto failed;
    }

    ᱡᱩᱫᱤ (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_MODE |
	    CREATE_NEW_PROCESS_GROUP |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_data,
	    (var_na *)options->jo_cwd))
    {
	CloseHandle(jo);
	job->jv_status = JOB_FAILED;
	goto failed;
    }

    ga_clear(&ga);

    ᱡᱩᱫᱤ (!AssignProcessToJobObject(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = NULL;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    job->jv_proc_info = pi;
    job->jv_job_object = jo;
    job->jv_status = JOB_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    ᱡᱩᱫᱤ (!use_out_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    job->jv_channel = channel;
    ᱡᱩᱫᱤ (channel != NULL)
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
    ᱨᱩᱣᱟᱹᱲ;

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

    var_na *
mch_job_status(job_T *job)
{
    DWORD dwExitCode = 0;

    ᱡᱩᱫᱤ (!GetExitCodeProcess(job->jv_proc_info.hProcess, &dwExitCode)
	    || dwExitCode != STILL_ACTIVE)
    {
	job->jv_exitval = (puur_naa_nka)dwExitCode;
	ᱡᱩᱫᱤ (job->jv_status < JOB_ENDED)
	{
	    ch_log(job->jv_channel, "Job ended");
	    job->jv_status = JOB_ENDED;
	}
	ᱨᱩᱣᱟᱹᱲ "dead";
    }
    ᱨᱩᱣᱟᱹᱲ "run";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    HANDLE jobHandles[MAXIMUM_WAIT_OBJECTS];
    job_T *jobArray[MAXIMUM_WAIT_OBJECTS];
    job_T *job = job_list;

    ᱡᱚᱠᱷᱚᱱ (job != NULL)
    {
	DWORD n;
	DWORD result;

	for (n = 0; n < MAXIMUM_WAIT_OBJECTS
				       && job != NULL; job = job->jv_next)
	{
	    ᱡᱩᱫᱤ (job->jv_status == JOB_STARTED)
	    {
		jobHandles[n] = job->jv_proc_info.hProcess;
		jobArray[n] = job;
		++n;
	    }
	}
	ᱡᱩᱫᱤ (n == 0)
	    continue;
	result = WaitForMultipleObjects(n, jobHandles, FALSE, 0);
	ᱡᱩᱫᱤ (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + n)
	{
	    job_T *wait_job = jobArray[result - WAIT_OBJECT_0];

	    ᱡᱩᱫᱤ (STRCMP(mch_job_status(wait_job), "dead") == 0)
		ᱨᱩᱣᱟᱹᱲ wait_job;
	}
    }
    ᱨᱩᱣᱟᱹᱲ NULL;
}

    static BOOL
terminate_all(HANDLE process, puur_naa_nka code)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_VALUE;
    DWORD	    pid = GetProcessId(process);

    ᱡᱩᱫᱤ (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	ᱡᱩᱫᱤ (h != INVALID_HANDLE_VALUE)
	{
	    pe.dwSize = parimaa_na(PROCESSENTRY32);
	    ᱡᱩᱫᱤ (!Process32First(h, &pe))
		goto theend;

	    do
	    {
		ᱡᱩᱫᱤ (pe.th32ParentProcessID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    ᱡᱩᱫᱤ (ph != NULL)
		    {
			terminate_all(ph, code);
			CloseHandle(ph);
		    }
		}
	    } ᱡᱚᱠᱷᱚᱱ (Process32Next(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    ᱨᱩᱣᱟᱹᱲ TerminateProcess(process, code);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    puur_naa_nka
mch_signal_job(job_T *job, char_u *how)
{
    puur_naa_nka ret;

    ᱡᱩᱫᱤ (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	ᱡᱩᱫᱤ (job->jv_job_object != NULL)
	{
	    ᱡᱩᱫᱤ (job->jv_channel != NULL && job->jv_channel->ch_anonymous_pipe)
		job->jv_channel->ch_killing = TRUE;
	    ᱨᱩᱣᱟᱹᱲ TerminateJobObject(job->jv_job_object, (UINT)-1) ? OK : FAIL;
	}
	ᱨᱩᱣᱟᱹᱲ terminate_all(job->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    ᱡᱩᱫᱤ (!AttachConsole(job->jv_proc_info.dwProcessId))
	ᱨᱩᱣᱟᱹᱲ FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		job->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    ᱨᱩᱣᱟᱹᱲ ret;
}

/*
 * Clear the data related to "job".
 */
    void
mch_clear_job(job_T *job)
{
    ᱡᱩᱫᱤ (job->jv_status == JOB_FAILED)
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (job->jv_job_object != NULL)
	CloseHandle(job->jv_job_object);
    CloseHandle(job->jv_proc_info.hProcess);
}
#endif


#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    static void
termcap_mode_start(void)
{
    DWORD cmodein;

    ᱡᱩᱫᱤ (g_fTermcapMode)
	ᱨᱩᱣᱟᱹᱲ;

    SaveConsoleBuffer(&g_cbNonTermcap);

    ᱡᱩᱫᱤ (g_cbTermcap.IsValid)
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
    ᱵᱟᱝ
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
    ᱡᱩᱫᱤ (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_INPUT;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    ᱵᱟᱝ
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
    static void
termcap_mode_end(void)
{
    DWORD cmodein;
    ConsoleBuffer *cb;
    COORD coord;
    DWORD dwDummy;

    ᱡᱩᱫᱤ (!g_fTermcapMode)
	ᱨᱩᱣᱟᱹᱲ;

    SaveConsoleBuffer(&g_cbTermcap);

    GetConsoleMode(g_hConIn, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# ᱵᱟᱝ
    cb = &g_cbNonTermcap;
# endif
    RestoreConsoleBuffer(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    ᱡᱩᱫᱤ (exiting && use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    ᱡᱩᱫᱤ (!USE_WT && (p_rs || exiting))
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
	ᱡᱩᱫᱤ (exiting && !p_rs)
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


#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    static void
clear_chars(
    COORD coord,
    DWORD n)
{
    ᱡᱩᱫᱤ (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_hConOut, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    ᱵᱟᱝ
    {
	set_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    static void
clear_screen(void)
{
    g_coord.X = g_coord.Y = 0;

    ᱡᱩᱫᱤ (!vtp_working)
	clear_chars(g_coord, Rows * Columns);
    ᱵᱟᱝ
    {
	set_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    static void
clear_to_end_of_display(void)
{
    COORD save = g_coord;

    ᱡᱩᱫᱤ (!vtp_working)
	clear_chars(g_coord, (Rows - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    ᱵᱟᱝ
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
    static void
clear_to_end_of_line(void)
{
    COORD save = g_coord;

    ᱡᱩᱫᱤ (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    ᱵᱟᱝ
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
    static void
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
    static void
set_scroll_region(
    unsigned left,
    unsigned top,
    unsigned right,
    unsigned bottom)
{
    ᱡᱩᱫᱤ (left >= right
	    || top >= bottom
	    || right > (unsigned) Columns - 1
	    || bottom > (unsigned) Rows - 1)
	ᱨᱩᱣᱟᱹᱲ;

    g_srScrollRegion.Left =   left;
    g_srScrollRegion.Top =    top;
    g_srScrollRegion.Right =  right;
    g_srScrollRegion.Bottom = bottom;
}

    static void
set_scroll_region_tb(
    unsigned top,
    unsigned bottom)
{
    ᱡᱩᱫᱤ (top >= bottom || bottom > (unsigned)Rows - 1)
	ᱨᱩᱣᱟᱹᱲ;

    g_srScrollRegion.Top = top;
    g_srScrollRegion.Bottom = bottom;
}

    static void
set_scroll_region_lr(
    unsigned left,
    unsigned right)
{
    ᱡᱩᱫᱤ (left >= right || right > (unsigned)Columns - 1)
	ᱨᱩᱣᱟᱹᱲ;

    g_srScrollRegion.Left = left;
    g_srScrollRegion.Right = right;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    static void
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
    ᱡᱩᱫᱤ (!USE_VTP)
	fill.Attributes = g_attrCurrent;
    ᱵᱟᱝ
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    ᱡᱩᱫᱤ (source.Bottom < dest.Y)
    {
	COORD coord;
	puur_naa_nka   i;

	coord.X = source.Left;
	for (i = clip.Top; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.Right - source.Left + 1);
	}
    }

    ᱡᱩᱫᱤ (vtp_working)
    {
	COORD coord;
	puur_naa_nka i;

	coord.X = source.Left;
	for (i = source.Top; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.Right - source.Left + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    static void
delete_lines(unsigned cLines)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    puur_naa_nka		    nb;

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
    ᱡᱩᱫᱤ (!vtp_working)
	fill.Attributes = g_attrCurrent;
    ᱵᱟᱝ
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (source.Bottom - source.Top) + 1;

    ᱡᱩᱫᱤ (nb < source.Top)
    {
	COORD coord;
	puur_naa_nka   i;

	coord.X = source.Left;
	for (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.Right - source.Left + 1);
	}
    }

    ᱡᱩᱫᱤ (vtp_working)
    {
	COORD coord;
	puur_naa_nka i;

	coord.X = source.Left;
	for (i = nb; i <= source.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.Right - source.Left + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    static void
gotoxy(
    unsigned x,
    unsigned y)
{
    ᱡᱩᱫᱤ (x < 1 || x > (unsigned)Columns || y < 1 || y > (unsigned)Rows)
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (!USE_VTP)
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
    ᱵᱟᱝ
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
    static void
textattr(WORD wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    SetConsoleTextAttribute(g_hConOut, wAttr);
}


    static void
textcolor(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    ᱡᱩᱫᱤ (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    ᱵᱟᱝ
	vtp_sgr_bulk(wAttr);
}


    static void
textbackground(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    ᱡᱩᱫᱤ (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    ᱵᱟᱝ
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    static void
normvideo(void)
{
    ᱡᱩᱫᱤ (!vtp_working)
	textattr(g_attrDefault);
    ᱵᱟᱝ
	vtp_sgr_bulk(0);
}


static WORD g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    static void
standout(void)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((WORD) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    static void
standend(void)
{
    ᱡᱩᱫᱤ (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    void
mch_set_normal_colors(void)
{
    char_u	*p;
    puur_naa_nka		n;

    cterm_normal_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_normal_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    ᱡᱩᱫᱤ (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	ᱡᱩᱫᱤ (*p == 'm' && n > 0)
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
    static void
visual_bell(void)
{
    COORD   coordOrigin = {0, 0};
    WORD    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = NULL;

# ifdef FEAT_TERMGUICOLORS
    ᱡᱩᱫᱤ (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(WORD, Rows * Columns);
	ᱡᱩᱫᱤ (oldattrs == NULL)
	    ᱨᱩᱣᱟᱹᱲ;
	ReadConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_hConOut, attrFlash, Rows * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    ᱡᱩᱫᱤ (oldattrs != NULL)
    {
	WriteConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    static void
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    ᱡᱩᱫᱤ (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	ᱨᱩᱣᱟᱹᱲ;
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
    static DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cbToWrite)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    static DWORD    cells;
    static WCHAR    *unicodebuf = NULL;
    static puur_naa_nka	    unibuflen = 0;
    static puur_naa_nka	    length;
    puur_naa_nka		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    static WCHAR    *utf8spbuf = NULL;
    static puur_naa_nka	    utf8splength;
    static DWORD    utf8spcells;
    static WCHAR    **utf8usingbuf = &unicodebuf;

    ᱡᱩᱫᱤ (cbToWrite != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	do
	{
	    length = MultiByteToWideChar(cp, 0, (LPCSTR)pchBuf, cbToWrite,
							unicodebuf, unibuflen);
	    ᱡᱩᱫᱤ (length && length <= unibuflen)
		bha_nga;
	    vim_free(unicodebuf);
	    unicodebuf = length ? LALLOC_MULT(WCHAR, length) : NULL;
	    unibuflen = unibuflen ? 0 : length;
	} ᱡᱚᱠᱷᱚᱱ (TRUE);
	cells = mb_string2cells(pchBuf, cbToWrite);
    }
    ᱵᱟᱝ // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	ᱡᱩᱫᱤ (utf8usingbuf != &utf8spbuf)
	{
	    ᱡᱩᱫᱤ (utf8spbuf == NULL)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		length = MultiByteToWideChar(CP_UTF8, 0, " ", 1, NULL, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, length);
		ᱡᱩᱫᱤ (utf8spbuf != NULL)
		{
		    MultiByteToWideChar(CP_UTF8, 0, " ", 1, utf8spbuf, length);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = length;
		    utf8spcells = cells;
		}
	    }
	    ᱵᱟᱝ
	    {
		utf8usingbuf = &utf8spbuf;
		length = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    ᱡᱩᱫᱤ (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	ᱡᱩᱫᱤ (WriteConsoleOutputCharacterW(g_hConOut, *utf8usingbuf, length,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    ᱵᱟᱝ
    {
	ᱡᱩᱫᱤ (WriteConsoleW(g_hConOut, *utf8usingbuf, length, &cchwritten,
		    NULL) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    ᱡᱩᱫᱤ (cchwritten == (DWORD)length)
    {
	written = cbToWrite;
	g_coord.X += (SHORT)cells;
    }
    ᱵᱟᱝ
    {
	char_u *p = pchBuf;
	for (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    ᱡᱚᱠᱷᱚᱱ (g_coord.X > g_srScrollRegion.Right)
    {
	g_coord.X -= (SHORT) Columns;
	ᱡᱩᱫᱤ (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    ᱡᱩᱫᱤ (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    ᱨᱩᱣᱟᱹᱲ written;
}

    static char_u *
get_seq(
    puur_naa_nka *args,
    puur_naa_nka *count,
    char_u *head)
{
    puur_naa_nka argc;
    char_u *p;

    ᱡᱩᱫᱤ (head == NULL || *head != '\033')
	ᱨᱩᱣᱟᱹᱲ NULL;

    argc = 0;
    p = head;
    ++p;
    do
    {
	++p;
	args[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } ᱡᱚᱠᱷᱚᱱ (*p == ';');
    *count = argc;

    ᱨᱩᱣᱟᱹᱲ p;
}

    static char_u *
get_sgr(
    puur_naa_nka *args,
    puur_naa_nka *count,
    char_u *head)
{
    char_u *p = get_seq(args, count, head);

    ᱨᱩᱣᱟᱹᱲ (p && *p == 'm') ? ++p : NULL;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    static char_u *
sgrn2(
    char_u *head,
    puur_naa_nka	   n)
{
    puur_naa_nka argc;
    puur_naa_nka args[16];
    char_u *p = get_sgr(args, &argc, head);

    ᱨᱩᱣᱟᱹᱲ p && argc == 5 && args[0] == n && args[1] == 2 ? p : NULL;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    static char_u *
sgrnc(
    char_u *head,
    puur_naa_nka	   n)
{
    puur_naa_nka argc;
    puur_naa_nka args[16];
    char_u *p = get_sgr(args, &argc, head);

    ᱨᱩᱣᱟᱹᱲ p && argc == 1 && args[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : NULL;
}

    static char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    ᱡᱚᱠᱷᱚᱱ (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    ᱨᱩᱣᱟᱹᱲ p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    static char_u *
sgrn2c(
    char_u *head,
    puur_naa_nka	   n)
{
    char_u *p = sgrn2(head, n);

    ᱨᱩᱣᱟᱹᱲ p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : NULL;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    static char_u *
sgrn2cn(
    char_u *head,
    puur_naa_nka n)
{
    char_u *p = sgrn2(head, n);

    ᱨᱩᱣᱟᱹᱲ p && p[0] == 0x0a && p[1] == '\033' ? ++p : NULL;
}

static bool in_dcs = false;	// inside a DCS (sixel) sequence
static bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    static void
write_vt(char_u *s, puur_naa_nka len)
{
    puur_naa_nka		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, NULL, 0);
    ᱡᱩᱫᱤ (wlen <= 0)
	ᱨᱩᱣᱟᱹᱲ;
    ws = ALLOC_MULT(WCHAR, wlen);
    ᱡᱩᱫᱤ (ws == NULL)
	ᱨᱩᱣᱟᱹᱲ;
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_hConOut, ws, wlen, &written, NULL);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    static char_u *
skip_dcs(char_u *s, char_u *end)
{
    for (char_u *p = s; p < end; ++p)
    {
	ᱡᱩᱫᱤ (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    in_dcs = false;
	    ᱨᱩᱣᱟᱹᱲ p + 1;
	}
	dcs_esc = *p == ESC;
    }
    ᱨᱩᱣᱟᱹᱲ end;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    void
mch_write(
    char_u  *s,
    puur_naa_nka	    len)
{
    char_u  *end = s + len;

# ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ;
# endif

    ᱡᱩᱫᱤ (!term_console)
    {
	write(1, s, (unsigned)len);
	ᱨᱩᱣᱟᱹᱲ;
    }

    // translate ESC | sequences into faked bios calls
    ᱡᱚᱠᱷᱚᱱ (len--)
    {
	puur_naa_nka prefix = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	ᱡᱩᱫᱤ (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    ᱨᱩᱣᱟᱹᱲ;
	}

	ᱡᱩᱫᱤ (in_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    puur_naa_nka	l = (puur_naa_nka)(skip_dcs(s, end) - s);

	    ᱡᱩᱫᱤ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    continue;
	}

	ᱡᱚᱠᱷᱚᱱ (s + ++prefix < end)
	{
	    ch = s[prefix];
	    ᱡᱩᱫᱤ (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		bha_nga;
	}

	ᱡᱩᱫᱤ (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    ᱡᱩᱫᱤ (prefix != 0)
		prefix = 1;
	}

	ᱡᱩᱫᱤ (prefix != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, prefix);
# ifdef MCH_WRITE_DUMP
	    ᱡᱩᱫᱤ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, parimaa_na(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    ᱡᱩᱫᱤ (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.Left + 1, g_srScrollRegion.Bottom + 1);
	    }
	    ᱵᱟᱝ
	    {
		gotoxy(g_srScrollRegion.Left + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    ᱡᱩᱫᱤ (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.Left+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ᱡᱩᱫᱤ (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    ᱡᱩᱫᱤ (g_coord.X > g_srScrollRegion.Left)
		g_coord.X--;
	    ᱵᱟᱝ ᱡᱩᱫᱤ (g_coord.Y > g_srScrollRegion.Top)
	    {
		g_coord.X = g_srScrollRegion.Right;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ᱡᱩᱫᱤ (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (s[0] == '\a')
	{
	    // \a, bell
	    MessageBeep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    ᱡᱩᱫᱤ (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    puur_naa_nka	    arg1 = 0, arg2 = 0, argc = 0, args[16];
	    char_u  *sp;

	    switch (s[2])
	    {
	    pak_sa '0': pak_sa '1': pak_sa '2': pak_sa '3': pak_sa '4':
	    pak_sa '5': pak_sa '6': pak_sa '7': pak_sa '8': pak_sa '9':
		ᱡᱩᱫᱤ (*(p = get_seq(args, &argc, s)) != 'm')
		    goto notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		ᱡᱩᱫᱤ (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (puur_naa_nka)(p - s);
		    s = p;
		    bha_nga;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		ᱡᱩᱫᱤ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		ᱡᱩᱫᱤ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		ᱡᱩᱫᱤ (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		ᱡᱩᱫᱤ (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(args, &argc, p);

notsgr:
		arg1 = args[0];
		arg2 = args[1];
		ᱡᱩᱫᱤ (*p == 'm')
		{
		    ᱡᱩᱫᱤ (argc == 1 && args[0] == 0)
			normvideo();
		    ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 1)
		    {
			ᱡᱩᱫᱤ (USE_VTP)
			    textcolor((WORD)arg1);
			ᱵᱟᱝ
			    textattr((WORD)arg1);
		    }
		    ᱵᱟᱝ ᱡᱩᱫᱤ (vtp_working)
			vtp_sgr_bulks(argc, args);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 2 && *p == 'r')
		{
		    set_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 2 && *p == 'R')
		{
		    set_scroll_region_tb(arg1, arg2);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 2 && *p == 'V')
		{
		    set_scroll_region_lr(arg1, arg2);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.Top, g_coord.Y - arg1) + 1);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 1 && *p == 'b')
		{
		    textbackground((WORD) arg1);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.Right, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 1 && *p == 'f')
		{
		    textcolor((WORD) arg1);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 1 && *p == 'L')
		{
		    insert_lines(arg1);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (argc == 1 && *p == 'M')
		{
		    delete_lines(arg1);
		}

		len -= (puur_naa_nka)(p - s);
		s = p + 1;
		bha_nga;

	    pak_sa 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.Top, g_coord.Y - 1) + 1);
		goto got3;

	    pak_sa 'B':
		visual_bell();
		goto got3;

	    pak_sa 'C':
		gotoxy(min(g_srScrollRegion.Right, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		goto got3;

	    pak_sa 'E':
		termcap_mode_end();
		goto got3;

	    pak_sa 'F':
		standout();
		goto got3;

	    pak_sa 'f':
		standend();
		goto got3;

	    pak_sa 'H':
		gotoxy(1, 1);
		goto got3;

	    pak_sa 'j':
		clear_to_end_of_display();
		goto got3;

	    pak_sa 'J':
		clear_screen();
		goto got3;

	    pak_sa 'K':
		clear_to_end_of_line();
		goto got3;

	    pak_sa 'L':
		insert_lines(1);
		goto got3;

	    pak_sa 'M':
		delete_lines(1);
		goto got3;

	    pak_sa 'S':
		termcap_mode_start();
		goto got3;

	    pak_sa 'V':
		cursor_visible(TRUE);
		goto got3;

	    pak_sa 'v':
		cursor_visible(FALSE);
		goto got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    ᱡᱩᱫᱤ (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, parimaa_na(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    puur_naa_nka l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    ᱡᱚᱠᱷᱚᱱ (s + l < end && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    ᱡᱩᱫᱤ (s + l < end && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		ᱡᱩᱫᱤ (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    puur_naa_nka	l;

	    in_dcs = true;
	    dcs_esc = false;
	    l = (puur_naa_nka)(skip_dcs(s + 2, end) - s);
	    ᱡᱩᱫᱤ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	ᱵᱟᱝ
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    ᱡᱩᱫᱤ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, parimaa_na(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    ᱡᱩᱫᱤ (fdDump)
	fflush(fdDump);
# endif
}

#ᱵᱟᱝ // FEAT_GUI_MSWIN && !VIMDLL
    void
mch_write(
    char_u  *s UNUSED,
    puur_naa_nka	    len UNUSED)
{
    // never used
}

#endif


/*
 * Delay for "msec" milliseconds.
 */
    void
mch_delay(
    long    msec,
    puur_naa_nka	    flags UNUSED)
{
#ᱡᱩᱫᱤ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((puur_naa_nka)msec);	    // never wait for input
#ᱵᱟᱝ // Console
# ifdef VIMDLL
    ᱡᱩᱫᱤ (gui.in_use)
    {
	Sleep((puur_naa_nka)msec);	    // never wait for input
	ᱨᱩᱣᱟᱹᱲ;
    }
# endif
    ᱡᱩᱫᱤ (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	ᱡᱩᱫᱤ (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    puur_naa_nka towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    ᱡᱚᱠᱷᱚᱱ (msec > 0)
	    {
		mzvim_check_threads();
		ᱡᱩᱫᱤ (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	ᱵᱟᱝ
# endif
	    Sleep((puur_naa_nka)msec);
    ᱵᱟᱝ
	WaitForChar(msec, FALSE);
#endif
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    puur_naa_nka
mch_remove(char_u *name)
{
    WCHAR	*wn;
    puur_naa_nka		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    ᱡᱩᱫᱤ (mch_isdir(name) && mch_is_symbolic_link(name))
	ᱨᱩᱣᱟᱹᱲ mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_NORMAL);

    wn = enc_to_utf16(name, NULL);
    ᱡᱩᱫᱤ (wn == NULL)
	ᱨᱩᱣᱟᱹᱲ -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    ᱨᱩᱣᱟᱹᱲ n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    void
mch_breakcheck(puur_naa_nka force UNUSED)
{
#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    ᱡᱩᱫᱤ (!gui.in_use)
# endif
	ᱡᱩᱫᱤ (g_fCtrlCPressed || g_fCBrkPressed)
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
mch_total_mem(puur_naa_nka special UNUSED)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dwLength = parimaa_na(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&ms);
    ᱡᱩᱫᱤ (ms.ullAvailVirtual < ms.ullTotalPhys)
    {
	// Process address space fits in physical RAM, use all of it.
	ᱨᱩᱣᱟᱹᱲ (long_u)(ms.ullAvailVirtual / 1024);
    }
    ᱡᱩᱫᱤ (ms.ullTotalPhys <= WINNT_RESERVE_BYTES)
    {
	// Catch old NT box or perverse hardware setup.
	ᱨᱩᱣᱟᱹᱲ (long_u)((ms.ullTotalPhys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    ᱨᱩᱣᱟᱹᱲ (long_u)((ms.ullTotalPhys - WINNT_RESERVE_BYTES) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    puur_naa_nka
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    puur_naa_nka		flags)		// EW_* flags
{
    ᱨᱩᱣᱟᱹᱲ dos_expandpath(gap, path, 0, flags, FALSE);
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
    puur_naa_nka
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	szTempFile[_MAX_PATH + 1];
    WCHAR	szNewPath[_MAX_PATH + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    for (p = q = wold; *p != L'\0'; ++p)
    {
	switch (*p)
	{
	pak_sa L'/':
	pak_sa L'\\':
	pak_sa L':':
	    ᱡᱩᱫᱤ (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    bha_nga;

	default:
	    bha_nga;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    ᱡᱩᱫᱤ ((puur_naa_nka)(p - q) < 8 || q[6] != L'~')
	ᱨᱩᱣᱟᱹᱲ (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    ᱡᱩᱫᱤ (GetFullPathNameW(wnew, _MAX_PATH, szNewPath, &p) == 0 || p == NULL)
	ᱨᱩᱣᱟᱹᱲ -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    ᱡᱩᱫᱤ (GetTempFileNameW(szNewPath, L"VIM", 0, szTempFile) == 0)
	ᱨᱩᱣᱟᱹᱲ -2;

    // blow the temp file away
    ᱡᱩᱫᱤ (!DeleteFileW(szTempFile))
	ᱨᱩᱣᱟᱹᱲ -3;

    // rename old file to the temp file
    ᱡᱩᱫᱤ (!MoveFileW(wold, szTempFile))
	ᱨᱩᱣᱟᱹᱲ -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    ᱡᱩᱫᱤ ((hf = CreateFileW(wold, GENERIC_WRITE, 0, NULL, CREATE_NEW,
		    FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
	ᱨᱩᱣᱟᱹᱲ -5;
    ᱡᱩᱫᱤ (!CloseHandle(hf))
	ᱨᱩᱣᱟᱹᱲ -6;

    // rename the temp file to the new file
    ᱡᱩᱫᱤ (!MoveFileW(szTempFile, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(void)MoveFileW(szTempFile, wold);
	ᱨᱩᱣᱟᱹᱲ -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(szTempFile);

    // finally, remove the empty old file
    ᱡᱩᱫᱤ (!DeleteFileW(wold))
	ᱨᱩᱣᱟᱹᱲ -8;

    ᱨᱩᱣᱟᱹᱲ 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    puur_naa_nka
mch_rename(
    const var_na	*pszOldFile,
    const var_na	*pszNewFile)
{
    WCHAR	*wold = NULL;
    WCHAR	*wnew = NULL;
    puur_naa_nka		retval = -1;

    wold = enc_to_utf16((char_u *)pszOldFile, NULL);
    wnew = enc_to_utf16((char_u *)pszNewFile, NULL);
    ᱡᱩᱫᱤ (wold != NULL && wnew != NULL)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    ᱨᱩᱣᱟᱹᱲ retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    var_na *
default_shell(void)
{
    ᱨᱩᱣᱟᱹᱲ "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    puur_naa_nka
mch_access(var_na *n, puur_naa_nka p)
{
    HANDLE	hFile;
    puur_naa_nka		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_to_utf16((char_u *)n, NULL);
    ᱡᱩᱫᱤ (wn == NULL)
	ᱨᱩᱣᱟᱹᱲ -1;

    ᱡᱩᱫᱤ (mch_isdir((char_u *)n))
    {
	WCHAR TempNameW[_MAX_PATH + 16] = L"";

	ᱡᱩᱫᱤ (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    puur_naa_nka			i;
	    WIN32_FIND_DATAW    d;

	    for (i = 0; i < _MAX_PATH && wn[i] != 0; ++i)
		TempNameW[i] = wn[i];
	    ᱡᱩᱫᱤ (TempNameW[i - 1] != '\\' && TempNameW[i - 1] != '/')
		TempNameW[i++] = '\\';
	    TempNameW[i++] = '*';
	    TempNameW[i++] = 0;

	    hFile = FindFirstFileW(TempNameW, &d);
	    ᱡᱩᱫᱤ (hFile == INVALID_HANDLE_VALUE)
		goto getout;
	    ᱵᱟᱝ
		(void)FindClose(hFile);
	}

	ᱡᱩᱫᱤ (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    ᱡᱩᱫᱤ (!GetTempFileNameW(wn, L"VIM", 0, TempNameW))
		goto getout;
	    ᱵᱟᱝ
		DeleteFileW(TempNameW);
	}
    }
    ᱵᱟᱝ
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_mode = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_mode, share_mode,
					     NULL, OPEN_EXISTING, 0, NULL);
	ᱡᱩᱫᱤ (hFile == INVALID_HANDLE_VALUE)
	    goto getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    ᱨᱩᱣᱟᱹᱲ retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    puur_naa_nka
mch_open(const var_na *name, puur_naa_nka flags, puur_naa_nka mode)
{
    WCHAR	*wn;
    puur_naa_nka		f;

    wn = enc_to_utf16((char_u *)name, NULL);
    ᱡᱩᱫᱤ (wn == NULL)
	ᱨᱩᱣᱟᱹᱲ -1;

    f = _wopen(wn, flags, mode);
    vim_free(wn);
    ᱨᱩᱣᱟᱹᱲ f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(const var_na *name, const var_na *mode)
{
    WCHAR	*wn, *wm;
    FILE	*f = NULL;

#ᱡᱩᱫᱤ defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    var_na newMode = mode[strlen(mode) - 1];
    puur_naa_nka oldMode = 0;

    _get_fmode(&oldMode);
    ᱡᱩᱫᱤ (newMode == 't')
	_set_fmode(_O_TEXT);
    ᱵᱟᱝ ᱡᱩᱫᱤ (newMode == 'b')
	_set_fmode(_O_BINARY);
#endif
    wn = enc_to_utf16((char_u *)name, NULL);
    wm = enc_to_utf16((char_u *)mode, NULL);
    ᱡᱩᱫᱤ (wn != NULL && wm != NULL)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#ᱡᱩᱫᱤ defined(DEBUG) && _MSC_VER >= 1400
    ᱡᱩᱫᱤ (oldMode != 0)
	_set_fmode(oldMode);
#endif
    ᱨᱩᱣᱟᱹᱲ f;
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
    static void
copy_substream(HANDLE sh, void *context, WCHAR *to, WCHAR *substream, long len)
{
    HANDLE  hTo;
    WCHAR   *to_name;

    to_name = alloc((wcslen(to) + wcslen(substream) + 1) * parimaa_na(WCHAR));
    ᱡᱩᱫᱤ (to_name == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    wcscpy(to_name, to);
    wcscat(to_name, substream);

    hTo = CreateFileW(to_name, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL, NULL);
    ᱡᱩᱫᱤ (hTo != INVALID_HANDLE_VALUE)
    {
	long	done;
	DWORD	todo;
	DWORD	readcnt, written;
	var_na	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	for (done = 0; done < len; done += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - done) > parimaa_na(buf) ? parimaa_na(buf)
						       : (size_t)(len - done));
	    ᱡᱩᱫᱤ (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(hTo, buf, todo, &written, NULL)
		    || written != todo)
		bha_nga;
	}
	CloseHandle(hTo);
    }

    free(to_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    static void
copy_infostreams(char_u *from, char_u *to)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_STREAM_ID	sid;
    puur_naa_nka			headersize;
    WCHAR		streamname[_MAX_PATH];
    DWORD		readcount;
    void		*context = NULL;
    DWORD		lo, hi;
    puur_naa_nka			len;

    // Convert the file names to wide characters.
    fromw = enc_to_utf16(from, NULL);
    tow = enc_to_utf16(to, NULL);
    ᱡᱩᱫᱤ (fromw != NULL && tow != NULL)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, NULL,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	ᱡᱩᱫᱤ (sh != INVALID_HANDLE_VALUE)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    for (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		ZeroMemory(&sid, parimaa_na(WIN32_STREAM_ID));
		headersize = (puur_naa_nka)((var_na *)&sid.cStreamName - (var_na *)&sid.dwStreamId);
		ᱡᱩᱫᱤ (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    bha_nga;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		ᱡᱩᱫᱤ (sid.dwStreamNameSize > 0)
		{
		    // Read the stream name.
		    ᱡᱩᱫᱤ (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dwStreamNameSize,
					  &readcount, FALSE, FALSE, &context))
			bha_nga;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / parimaa_na(WCHAR);
		    streamname[len] = 0;
		    ᱡᱩᱫᱤ (len > 7 && wcsicmp(streamname + len - 6,
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
		(void)BackupSeek(sh, sid.Size.u.LowPart, sid.Size.u.HighPart,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (void)BackupRead(sh, NULL, 0, &readcount, TRUE, FALSE, &context);

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

typedef sa_nracanaa _FILE_FULL_EA_INFORMATION_ {
    ULONG  NextEntryOffset;
    UCHAR  Flags;
    UCHAR  EaNameLength;
    USHORT EaValueLength;
    CHAR   EaName[1];
} FILE_FULL_EA_INFORMATION_, *PFILE_FULL_EA_INFORMATION_;

typedef sa_nracanaa _FILE_EA_INFORMATION_ {
    ULONG EaSize;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#ifndef PROTO
typedef NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions);
typedef NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
typedef NTSTATUS (NTAPI *PfnNtSetEaFile)(
	HANDLE		    FileHandle,
	PIO_STATUS_BLOCK    IoStatusBlock,
	PVOID		    Buffer,
	ULONG		    Length);
typedef NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	BOOLEAN ReturnSingleEntry,
	PVOID EaList,
	ULONG EaListLength,
	PULONG EaIndex,
	BOOLEAN RestartScan);
typedef NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_STATUS_BLOCK	IoStatusBlock,
	PVOID			FileInformation,
	ULONG			Length,
	FILE_INFORMATION_CLASS FileInformationClass);
typedef VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString);

PfnNtOpenFile pNtOpenFile = NULL;
PfnNtClose pNtClose = NULL;
PfnNtSetEaFile pNtSetEaFile = NULL;
PfnNtQueryEaFile pNtQueryEaFile = NULL;
PfnNtQueryInformationFile pNtQueryInformationFile = NULL;
PfnRtlInitUnicodeString pRtlInitUnicodeString = NULL;
#endif

/*
 * Load ntdll.dll functions.
 */
    static BOOL
load_ntdll(void)
{
    static puur_naa_nka	loaded = -1;

    ᱡᱩᱫᱤ (loaded != -1)
	ᱨᱩᱣᱟᱹᱲ (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    ᱡᱩᱫᱤ (hNtdll != NULL)
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
    ᱡᱩᱫᱤ (pNtOpenFile == NULL
	    || pNtClose == NULL
	    || pNtSetEaFile == NULL
	    || pNtQueryEaFile == NULL
	    || pNtQueryInformationFile == NULL
	    || pRtlInitUnicodeString == NULL)
	loaded = FALSE;
    ᱵᱟᱝ
	loaded = TRUE;
    ᱨᱩᱣᱟᱹᱲ (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    static void
copy_extattr(char_u *from, char_u *to)
{
    char_u		    *fromf = NULL;
    char_u		    *tof = NULL;
    WCHAR		    *fromw = NULL;
    WCHAR		    *tow = NULL;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_ATTRIBUTES	    oa;
    IO_STATUS_BLOCK	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    void		    *ea = NULL;

    ᱡᱩᱫᱤ (!load_ntdll())
	ᱨᱩᱣᱟᱹᱲ;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(to) + 5);
    ᱡᱩᱫᱤ (fromf == NULL || tof == NULL)
	goto theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), to);

    // Convert the names to wide characters.
    fromw = enc_to_utf16(fromf, NULL);
    tow = enc_to_utf16(tof, NULL);
    ᱡᱩᱫᱤ (fromw == NULL || tow == NULL)
	goto theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    InitializeObjectAttributes(&oa, &u, 0, NULL, NULL);
    ᱡᱩᱫᱤ (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	goto theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, parimaa_na(eainfo),
	    FileEaInformation);
    ᱡᱩᱫᱤ (eainfo.EaSize != 0)
    {
	ea = alloc(eainfo.EaSize);
	ᱡᱩᱫᱤ (ea != NULL)
	{
	    ᱡᱩᱫᱤ (pNtQueryEaFile(h, &iosb, ea, eainfo.EaSize, FALSE,
			NULL, 0, NULL, TRUE) != STATUS_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    ᱡᱩᱫᱤ (ea != NULL)
    {
	pRtlInitUnicodeString(&u, tow);
	InitializeObjectAttributes(&oa, &u, 0, NULL, NULL);
	ᱡᱩᱫᱤ (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	    goto theend;

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
    puur_naa_nka
mch_copy_file_attribute(char_u *from, char_u *to)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, to);
    copy_extattr(from, to);
    ᱨᱩᱣᱟᱹᱲ 0;
}


/*
 * The command line arguments in UTF-16
 */
static puur_naa_nka	nArgsW = 0;
static LPWSTR	*ArglistW = NULL;
static puur_naa_nka	global_argc = 0;
static var_na	**global_argv;

static puur_naa_nka	used_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
static puur_naa_nka	*used_file_indexes = NULL; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
static puur_naa_nka	used_file_count = 0;	// nr of entries in used_file_indexes
static puur_naa_nka	used_file_literal = FALSE;  // take file names literally
static puur_naa_nka	used_file_full_path = FALSE;  // file name was full path
#ifdef FEAT_DIFF
static puur_naa_nka	used_file_diff_mode = FALSE;  // file name was with diff mode
#endif
static puur_naa_nka	used_alist_count = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    puur_naa_nka
get_cmd_argsW(var_na ***argvp)
{
    var_na	**argv = NULL;
    puur_naa_nka		argc = 0;
    puur_naa_nka		i;

    free_cmd_argsW();
    ArglistW = CommandLineToArgvW(GetCommandLineW(), &nArgsW);
    ᱡᱩᱫᱤ (ArglistW != NULL)
    {
	argv = alloc((nArgsW + 1) * parimaa_na(var_na *));
	ᱡᱩᱫᱤ (argv != NULL)
	{
	    argc = nArgsW;
	    argv[argc] = NULL;
	    for (i = 0; i < argc; ++i)
	    {
		puur_naa_nka	len;

		// Convert each Unicode argument to UTF-8.
		WideCharToMultiByte_alloc(CP_UTF8, 0,
				ArglistW[i], (puur_naa_nka)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		ᱡᱩᱫᱤ (argv[i] == NULL)
		{
		    // Out of memory, clear everything.
		    ᱡᱚᱠᱷᱚᱱ (i > 0)
			free(argv[--i]);
		    free(argv);
		    argv = NULL;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    ᱡᱩᱫᱤ (argc > 0)
    {
	ᱡᱩᱫᱤ (used_file_indexes != NULL)
	    free(used_file_indexes);
	used_file_indexes = alloc(argc * parimaa_na(puur_naa_nka));
    }

    ᱡᱩᱫᱤ (argvp != NULL)
	*argvp = argv;
    ᱨᱩᱣᱟᱹᱲ argc;
}

    void
free_cmd_argsW(void)
{
    ᱡᱩᱫᱤ (ArglistW == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    GlobalFree(ArglistW);
    ArglistW = NULL;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    void
used_file_arg(var_na *name, puur_naa_nka literal, puur_naa_nka full_path, puur_naa_nka diff_mode)
{
    puur_naa_nka		i;

    ᱡᱩᱫᱤ (used_file_indexes == NULL)
	ᱨᱩᱣᱟᱹᱲ;
    for (i = used_file_argc + 1; i < global_argc; ++i)
	ᱡᱩᱫᱤ (STRCMP(global_argv[i], name) == 0)
	{
	    used_file_argc = i;
	    used_file_indexes[used_file_count++] = i;
	    bha_nga;
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
    void
set_alist_count(void)
{
    used_alist_count = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    void
fix_arg_enc(void)
{
    puur_naa_nka		i;
    puur_naa_nka		idx;
    char_u	*str;
    puur_naa_nka		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    ᱡᱩᱫᱤ (global_argc != nArgsW
	    || ArglistW == NULL
	    || used_file_indexes == NULL
	    || used_file_count == 0
	    || used_alist_count != GARGCOUNT)
	ᱨᱩᱣᱟᱹᱲ;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(puur_naa_nka, GARGCOUNT);
    ᱡᱩᱫᱤ (fnum_list == NULL)
	ᱨᱩᱣᱟᱹᱲ;		// out of memory
    for (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    ᱡᱩᱫᱤ (ga_grow(&global_alist.al_ga, used_file_count) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;		// out of memory

    for (i = 0; i < used_file_count; ++i)
    {
	idx = used_file_indexes[i];
	str = utf16_to_enc(ArglistW[idx], NULL);
	ᱡᱩᱫᱤ (str != NULL)
	{
	    puur_naa_nka literal = used_file_literal;

#ifdef FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    ᱡᱩᱫᱤ (used_file_diff_mode && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), tail, STRLEN(tail), TRUE, &ret);
		ᱡᱩᱫᱤ (ret.string != NULL)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#endif
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    ᱡᱩᱫᱤ (used_file_literal)
		buf_set_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    ᱡᱩᱫᱤ (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		ᱡᱩᱫᱤ (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_add(&global_alist, str, literal ? 2 : 0);
	}
    }

    ᱡᱩᱫᱤ (!used_file_literal)
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
    ᱡᱩᱫᱤ (curwin->w_arg_idx == 0 && curbuf->b_fname == NULL)
    {
	do_cmdline_cmd((char_u *)":rewind");
	ᱡᱩᱫᱤ (GARGCOUNT == 1 && used_file_full_path
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    last_chdir_reason = "drop";
    }

    set_alist_count();
}

    puur_naa_nka
mch_setenv(var_na *var, var_na *value, puur_naa_nka x UNUSED)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(value) + 2);
    ᱡᱩᱫᱤ (envbuf == NULL)
	ᱨᱩᱣᱟᱹᱲ -1;

    sprintf((var_na *)envbuf, "%s=%s", var, value);

    p = enc_to_utf16(envbuf, NULL);

    vim_free(envbuf);
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ -1;
    _wputenv(p);
#ifdef libintl_wputenv
    libintl_wputenv(p);
#endif
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    ᱨᱩᱣᱟᱹᱲ 0;
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

    static void
vtp_flag_init(void)
{
#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   mode;
    HANDLE  out;

# ifdef VIMDLL
    ᱡᱩᱫᱤ (!gui.in_use)
# endif
    {
	out = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_version >= VTP_FIRST_SUPPORT_BUILD) ? 1 : 0;
	GetConsoleMode(out, &mode);
	mode |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	ᱡᱩᱫᱤ (SetConsoleMode(out, mode) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_buffer = win_version >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#endif

    ᱡᱩᱫᱤ (win_version >= CONPTY_FIRST_SUPPORT_BUILD)
	conpty_working = 1;
    ᱡᱩᱫᱤ (win_version >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    ᱡᱩᱫᱤ (win_version <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    ᱡᱩᱫᱤ (win_version <= CONPTY_1909_BUILD)
	conpty_type = 2;
    ᱡᱩᱫᱤ (win_version <= CONPTY_1903_BUILD)
	conpty_type = 2;
    ᱡᱩᱫᱤ (win_version < CONPTY_FIRST_SUPPORT_BUILD)
	conpty_type = 1;

    ᱡᱩᱫᱤ (win_version >= CONPTY_NEXT_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    static void
vtp_init(void)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = parimaa_na(csbi);
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

    static void
vtp_exit(void)
{
    restore_console_color_rgb();
}

    puur_naa_nka
vtp_printf(
    var_na *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   result;
    puur_naa_nka	    len;

    ᱡᱩᱫᱤ (silent_mode)
	ᱨᱩᱣᱟᱹᱲ 0;

    va_start(list, format);
    len = vim_vsnprintf((var_na *)buf, 100, (var_na *)format, list);
    va_end(list);
    WriteConsoleA(g_hConOut, buf, (DWORD)len, &result, NULL);
    ᱨᱩᱣᱟᱹᱲ (puur_naa_nka)result;
}

    static void
vtp_sgr_bulk(
    puur_naa_nka arg)
{
    puur_naa_nka args[1];

    args[0] = arg;
    vtp_sgr_bulks(1, args);
}

# define FAST256(x) \
    ᱡᱩᱫᱤ ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    pak_sa x: \
	FAST256(newargs[x - 1]);

    static void
vtp_sgr_bulks(
    puur_naa_nka argc,
    puur_naa_nka *args)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    puur_naa_nka	    in, out;
    puur_naa_nka	    newargs[16];
    static puur_naa_nka sgrfgr = -1, sgrfgg, sgrfgb;
    static puur_naa_nka sgrbgr = -1, sgrbgg, sgrbgb;

    ᱡᱩᱫᱤ (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	ᱨᱩᱣᱟᱹᱲ;
    }

    in = out = 0;
    ᱡᱚᱠᱷᱚᱱ (in < argc)
    {
	puur_naa_nka s = args[in];
	puur_naa_nka copylen = 1;

	ᱡᱩᱫᱤ (s == 38)
	{
	    ᱡᱩᱫᱤ (argc - in >= 5 && args[in + 1] == 2)
	    {
		ᱡᱩᱫᱤ (sgrfgr == args[in + 2] && sgrfgg == args[in + 3]
						     && sgrfgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		ᱵᱟᱝ
		{
		    sgrfgr = args[in + 2];
		    sgrfgg = args[in + 3];
		    sgrfgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (s == 48)
	{
	    ᱡᱩᱫᱤ (argc - in >= 5 && args[in + 1] == 2)
	    {
		ᱡᱩᱫᱤ (sgrbgr == args[in + 2] && sgrbgg == args[in + 3]
						     && sgrbgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		ᱵᱟᱝ
		{
		    sgrbgr = args[in + 2];
		    sgrbgg = args[in + 3];
		    sgrbgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (30 <= s && s <= 39)
	    sgrfgr = -1;
	ᱵᱟᱝ ᱡᱩᱫᱤ (90 <= s && s <= 97)
	    sgrfgr = -1;
	ᱵᱟᱝ ᱡᱩᱫᱤ (40 <= s && s <= 49)
	    sgrbgr = -1;
	ᱵᱟᱝ ᱡᱩᱫᱤ (100 <= s && s <= 107)
	    sgrbgr = -1;
	ᱵᱟᱝ ᱡᱩᱫᱤ (s == 0)
	    sgrfgr = sgrbgr = -1;

	ᱡᱚᱠᱷᱚᱱ (copylen--)
	    newargs[out++] = args[in++];
    }

    p = &buf[parimaa_na(buf) - 1];
    *p-- = 'm';

    switch (out)
    {
	puur_naa_nka	n, m;
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
	WriteConsoleA(g_hConOut, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, NULL);
    default:
	bha_nga;
    }
}

    static void
wt_init(void)
{
    wt_working = mch_getenv("WT_SESSION") != NULL;
}

# ifdef FEAT_TERMGUICOLORS
    static puur_naa_nka
ctermtoxterm(
    puur_naa_nka cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    ᱨᱩᱣᱟᱹᱲ (((puur_naa_nka)r << 16) | ((puur_naa_nka)g << 8) | (puur_naa_nka)b);
}
# endif

    static void
set_console_color_rgb(void)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    guicolor_T	fg, bg;
    puur_naa_nka		ctermfg, ctermbg;

    ᱡᱩᱫᱤ (!vtp_working)
	ᱨᱩᱣᱟᱹᱲ;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    ᱡᱩᱫᱤ (p_tgc || t_colors >= 256)
    {
	term_fg_rgb_color(fg);
	term_bg_rgb_color(bg);
	ᱨᱩᱣᱟᱹᱲ;
    }

    ᱡᱩᱫᱤ (use_alternate_screen_buffer)
	ᱨᱩᱣᱟᱹᱲ;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cbSize = parimaa_na(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = parimaa_na(csbi);
    csbi.srWindow.Right += 1;
    csbi.srWindow.Bottom += 1;
    store_console_bg_rgb = csbi.ColorTable[g_color_index_bg];
    store_console_fg_rgb = csbi.ColorTable[g_color_index_fg];
    csbi.ColorTable[g_color_index_bg] = (COLORREF)bg;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)fg;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

# ᱡᱩᱫᱤ defined(FEAT_TERMGUICOLORS)
    void
get_default_console_color(
    puur_naa_nka *cterm_fg,
    puur_naa_nka *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    puur_naa_nka id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    puur_naa_nka ctermfg = 0;
    puur_naa_nka ctermbg = 0;
    puur_naa_nka dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    ᱡᱩᱫᱤ (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    ᱡᱩᱫᱤ (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	ᱡᱩᱫᱤ (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	ᱡᱩᱫᱤ (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	ᱵᱟᱝ
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_normal_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    ᱡᱩᱫᱤ (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	ᱡᱩᱫᱤ (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	ᱡᱩᱫᱤ (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	ᱵᱟᱝ
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
    static void
reset_console_color_rgb(void)
{
# ifdef FEAT_TERMGUICOLORS
    ᱡᱩᱫᱤ (use_alternate_screen_buffer)
	ᱨᱩᱣᱟᱹᱲ;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = parimaa_na(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = parimaa_na(csbi);
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
    static void
restore_console_color_rgb(void)
{
# ifdef FEAT_TERMGUICOLORS
    ᱡᱩᱫᱤ (use_alternate_screen_buffer)
	ᱨᱩᱣᱟᱹᱲ;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = parimaa_na(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = parimaa_na(csbi);
    csbi.srWindow.Right += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)save_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)save_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

    void
control_console_color_rgb(void)
{
    ᱡᱩᱫᱤ (vtp_working)
	set_console_color_rgb();
    ᱵᱟᱝ
	reset_console_color_rgb();
}

    puur_naa_nka
use_vtp(void)
{
    ᱨᱩᱣᱟᱹᱲ USE_VTP;
}

    puur_naa_nka
is_term_win32(void)
{
    ᱨᱩᱣᱟᱹᱲ T_NAME != NULL && STRCMP(T_NAME, "win32") == 0;
}

    puur_naa_nka
has_vtp_working(void)
{
    ᱨᱩᱣᱟᱹᱲ vtp_working;
}

#endif

    puur_naa_nka
has_conpty_working(void)
{
    ᱨᱩᱣᱟᱹᱲ conpty_working;
}

    puur_naa_nka
get_conpty_type(void)
{
    ᱨᱩᱣᱟᱹᱲ conpty_type;
}

    puur_naa_nka
is_conpty_stable(void)
{
    ᱨᱩᱣᱟᱹᱲ conpty_stable;
}

    puur_naa_nka
get_conpty_fix_type(void)
{
    ᱨᱩᱣᱟᱹᱲ conpty_fix_type;
}

#ᱡᱩᱫᱤ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    void
resize_console_buf(void)
{
    ᱡᱩᱫᱤ (use_alternate_screen_buffer)
	ᱨᱩᱣᱟᱹᱲ;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    ᱡᱩᱫᱤ (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ᱨᱩᱣᱟᱹᱲ;

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

    var_na *
GetWin32Error(void)
{
    static var_na	*oldmsg = NULL;
    var_na	*acp_msg = NULL;
    DWORD	acp_len;
    var_na	*enc_msg = NULL;
    puur_naa_nka		enc_len = 0;

    // get formatted message from OS
    acp_len = FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
	    NULL, GetLastError(), 0, (LPSTR)&acp_msg, 0, NULL);
    ᱡᱩᱫᱤ (acp_len == 0 || acp_msg == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    // clean oldmsg if remained.
    ᱡᱩᱫᱤ (oldmsg != NULL)
    {
	vim_free(oldmsg);
	oldmsg = NULL;
    }

    acp_to_enc((char_u *)acp_msg, (puur_naa_nka)acp_len, (char_u **)&enc_msg, &enc_len);
    LocalFree(acp_msg);
    ᱡᱩᱫᱤ (enc_msg == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    // remove trailing \r\n
    var_na *pcrlf = strstr(enc_msg, "\r\n");
    ᱡᱩᱫᱤ (pcrlf != NULL)
	*pcrlf = NUL;

    oldmsg = enc_msg;
    ᱨᱩᱣᱟᱹᱲ enc_msg;
}

#ᱡᱩᱫᱤ defined(FEAT_RELTIME)
static HANDLE   timer_handle;
static puur_naa_nka      timer_active = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
static sig_atomic_t timeout_flags[2];
static puur_naa_nka	    timeout_flag_idx = 0;
static sig_atomic_t *timeout_flag = &timeout_flags[0];


    static void CALLBACK
set_flag(void *param, BOOLEAN unused2 UNUSED)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    void
stop_timeout(void)
{
    ᱡᱩᱫᱤ (timer_active)
    {
	BOOL ret = DeleteTimerQueueTimer(NULL, timer_handle, NULL);
	timer_active = FALSE;
	ᱡᱩᱫᱤ (!ret && GetLastError() != ERROR_IO_PENDING)
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
    ca_ncala sig_atomic_t *
start_timeout(long msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = CreateTimerQueueTimer(
	    &timer_handle, NULL, set_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    ᱡᱩᱫᱤ (!ret)
    {
	semsg(_(e_could_not_set_timeout_str), GetWin32Error());
    }
    ᱵᱟᱝ
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_active = TRUE;
	*timeout_flag = FALSE;
    }
    ᱨᱩᱣᱟᱹᱲ timeout_flag;
}
#endif
