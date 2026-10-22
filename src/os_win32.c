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

#அந்தர்பாவ "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#அந்தர்பாவ <sys/types.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <limits.h>

#அந்தர்பாவ <process.h>
#அந்தர்பாவ <winternl.h>
#அந்தர்பாவ <direct.h>

#எனில் !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#ifdef FEAT_JOB_CHANNEL
# include <tlhelp32.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = வெற்று;
#endif

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
நிலையான HANDLE g_hConIn  = INVALID_HANDLE_மதிப்பு;
நிலையான HANDLE g_hConOut = INVALID_HANDLE_மதிப்பு;

// Win32 Screen buffer,coordinate,console I/O information
நிலையான SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
நிலையான COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
நிலையான WORD  g_attrDefault = 7;  // lightgray text on black background
நிலையான WORD  g_attrCurrent;

நிலையான முழு g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
நிலையான முழு g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
நிலையான முழு g_fForceExit = FALSE;    // set when forcefully exiting

நிலையான வெறுமை scroll(unsigned c_வரிகள்);
நிலையான வெறுமை set_scroll_region(unsigned இடது, unsigned உச்சி,
			      unsigned வலது, unsigned bottom);
நிலையான வெறுமை set_scroll_region_tb(unsigned உச்சி, unsigned bottom);
நிலையான வெறுமை set_scroll_region_lr(unsigned இடது, unsigned வலது);
நிலையான வெறுமை செருகு_வரிகள்(unsigned c_வரிகள்);
நிலையான வெறுமை delete_வரிகள்(unsigned c_வரிகள்);
நிலையான வெறுமை gotoxy(unsigned x, unsigned y);
நிலையான வெறுமை standout(வெறுமை);
நிலையான முழு s_cursor_visible = TRUE;
நிலையான முழு did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
வகைவரை கட்டமைப்பு உள்ளீடு_record_buffer_முனை_S
{
    உள்ளீடு_RECORD ir;
    கட்டமைப்பு உள்ளீடு_record_buffer_முனை_S *next;
} உள்ளீடு_record_buffer_முனை_T;
வகைவரை கட்டமைப்பு உள்ளீடு_record_buffer_S
{
    உள்ளீடு_record_buffer_முனை_T *head;
    உள்ளீடு_record_buffer_முனை_T *tail;
    முழு length;
} உள்ளீடு_record_buffer_T;
நிலையான உள்ளீடு_record_buffer_T உள்ளீடு_record_buffer;
நிலையான முழு read_உள்ளீடு_record_buffer(உள்ளீடு_RECORD* irEvents, முழு nMaxLength);
நிலையான முழு write_உள்ளீடு_record_buffer(உள்ளீடு_RECORD* irEvents, முழு nLength);
#endif
#ifdef FEAT_GUI_MSWIN
நிலையான முழு s_dont_use_vimrun = TRUE;
நிலையான முழு need_vimrun_warning = FALSE;
நிலையான string_T vimrun_path = {(char_u *)"vimrun ", 7};
நிலையான முழு vimrun_path_allocated = FALSE;
#endif

நிலையான முழு win32_getattrs(char_u *name);
நிலையான முழு win32_setattrs(char_u *name, முழு attrs);
நிலையான முழு win32_set_archive(char_u *name);

நிலையான முழு conpty_working = 0;
நிலையான முழு conpty_type = 0;
நிலையான முழு conpty_stable = 0;
நிலையான முழு conpty_fix_type = 0;
நிலையான வெறுமை vtp_flag_init(வெறுமை);

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
நிலையான முழு vtp_working = 0;
நிலையான வெறுமை vtp_init(வெறுமை);
நிலையான வெறுமை vtp_exit(வெறுமை);
நிலையான வெறுமை vtp_sgr_bulk(முழு arg);
நிலையான வெறுமை vtp_sgr_bulks(முழு argc, முழு *argv);

நிலையான முழு wt_working = 0;
நிலையான வெறுமை wt_init(வெறுமை);

நிலையான முழு g_color_index_bg = 0;
நிலையான முழு g_color_index_fg = 7;

# ifdef FEAT_TERMGUICOLORS
நிலையான guicolor_T save_console_bg_rgb;
நிலையான guicolor_T save_console_fg_rgb;
நிலையான guicolor_T store_console_bg_rgb;
நிலையான guicolor_T store_console_fg_rgb;
நிலையான முழு default_console_color_bg = 0x000000; // black
நிலையான முழு default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_term_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# இல்லையெனில்
#  define USE_VTP		0
#  define USE_WT		0
# endif

நிலையான வெறுமை set_console_color_rgb(வெறுமை);
நிலையான வெறுமை reset_console_color_rgb(வெறுமை);
நிலையான வெறுமை restore_console_color_rgb(வெறுமை);
#endif  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
நிலையான முழு suppress_winsize = 1;	// don't fiddle with console
#endif

நிலையான WCHAR *exe_pathw = வெற்று;

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
நிலையான BOOL use_alternate_screen_buffer = FALSE;
#endif

வெளி DWORD win_version; // this is in os_mswin.c

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    நிலையான BOOL
is_ambiwidth_event(
    உள்ளீடு_RECORD *ir)
{
    திரும்பு ir->EventType == சாவி_EVENT
		&& ir->Event.சாவி_Event_2.b_சாவி_Down
		&& ir->Event.சாவி_Event_2.w_Repeat_மொத்தம் == 1
		&& ir->Event.சாவி_Event_2.w_Virtual_சாவி_Code == 0x12
		&& ir->Event.சாவி_Event_2.wVirtualScanCode == 0x38
		&& ir->Event.சாவி_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.சாவி_Event_2.dw_Control_சாவி_State == 2;
}

    நிலையான வெறுமை
make_ambiwidth_event(
    உள்ளீடு_RECORD *down,
    உள்ளீடு_RECORD *up)
{
    down->Event.சாவி_Event_2.w_Virtual_சாவி_Code = 0;
    down->Event.சாவி_Event_2.wVirtualScanCode = 0;
    down->Event.சாவி_Event_2.uChar.UnicodeChar
				    = up->Event.சாவி_Event_2.uChar.UnicodeChar;
    down->Event.சாவி_Event_2.dw_Control_சாவி_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    நிலையான BOOL
read_console_உள்ளீடு(
    HANDLE	    h_உள்ளீடு,
    உள்ளீடு_RECORD    *lpBuffer,
    முழு		    nLength,
    LPDWORD	    lpEvents)
{
    எண்ணிக்கை
    {
	IRSIZE = 10
    };
    நிலையான உள்ளீடு_RECORD s_irCache[IRSIZE];
    நிலையான DWORD s_dwIndex = 0;
    நிலையான DWORD s_dwMax = 0;
    DWORD dwEvents;
    முழு head;
    முழு tail;
    முழு i;
    நிலையான உள்ளீடு_RECORD s_irPseudo;

    எனில் (s_dwMax == 0 && உள்ளீடு_record_buffer.length > 0)
    {
	dwEvents = read_உள்ளீடு_record_buffer(s_irCache, IRSIZE);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
    }

    எனில் (nLength == -2)
	திரும்பு (s_dwMax > 0) ? TRUE : FALSE;

    எனில் (win_version < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	எனில் (nLength == -1)
	    திரும்பு Peek_Console_உள்ளீடு_W(h_உள்ளீடு, lpBuffer, 1, lpEvents);
	திரும்பு Read_Console_உள்ளீடு_W(h_உள்ளீடு, lpBuffer, 1, &dwEvents);
    }

    எனில் (s_dwMax == 0)
    {
	எனில் (!vtp_working && nLength == -1)
	    திரும்பு Peek_Console_உள்ளீடு_W(h_உள்ளீடு, lpBuffer, 1, lpEvents);
	Get_எண்_Of_Console_உள்ளீடு_Events(h_உள்ளீடு, &dwEvents);
	எனில் (dwEvents == 0 && nLength == -1)
	    திரும்பு Peek_Console_உள்ளீடு_W(h_உள்ளீடு, lpBuffer, 1, lpEvents);
	Read_Console_உள்ளீடு_W(h_உள்ளீடு, s_irCache, IRSIZE, &dwEvents);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
	எனில் (dwEvents == 0)
	{
	    *lpEvents = 0;
	    திரும்பு TRUE;
	}

	க்கு (i = s_dwIndex; i < (முழு)s_dwMax - 1; ++i)
	    எனில் (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	எனில் (s_dwMax > 1)
	{
	    head = 0;
	    tail = s_dwMax - 1;
	    போது (head != tail)
	    {
		எனில் (s_irCache[head].EventType == WINDOW_BUFFER_SIZE_EVENT
			&& s_irCache[head + 1].EventType
						  == WINDOW_BUFFER_SIZE_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    க்கு (i = head; i < tail; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --tail;
		    தொடர்;
		}
		head++;
	    }
	    s_dwMax = tail + 1;
	}
    }

    எனில் (s_irCache[s_dwIndex].EventType == சாவி_EVENT)
    {
	எனில் (s_irCache[s_dwIndex].Event.சாவி_Event_2.w_Repeat_மொத்தம் > 1)
	{
	    s_irPseudo = s_irCache[s_dwIndex];
	    s_irPseudo.Event.சாவி_Event_2.w_Repeat_மொத்தம் = 1;
	    s_irCache[s_dwIndex].Event.சாவி_Event_2.w_Repeat_மொத்தம்--;
	    *lpBuffer = s_irPseudo;
	    *lpEvents = 1;
	    திரும்பு TRUE;
	}
    }

    *lpBuffer = s_irCache[s_dwIndex];
    எனில் (!(nLength == -1 || nLength == -2) && ++s_dwIndex >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    திரும்பு TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    நிலையான BOOL
peek_console_உள்ளீடு(
    HANDLE	    h_உள்ளீடு,
    உள்ளீடு_RECORD    *lpBuffer,
    DWORD	    nLength UNUSED,
    LPDWORD	    lpEvents)
{
    திரும்பு read_console_உள்ளீடு(h_உள்ளீடு, lpBuffer, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    நிலையான DWORD
செய்தி_wait_for_multiple_objects(
    DWORD    n_மொத்தம்,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dwWakeMask)
{
    எனில் (read_console_உள்ளீடு(வெற்று, வெற்று, -2, வெற்று))
	திரும்பு WAIT_OBJECT_0;
    திரும்பு செய்தி_Wait_For_Multiple_Objects(n_மொத்தம், pHandles, fWaitAll,
				     dwMilliseconds, dwWakeMask);
}
# endif

# ifndef FEAT_CLIENTSERVER
    நிலையான DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    எனில் (read_console_உள்ளீடு(வெற்று, வெற்று, -2, வெற்று))
	திரும்பு WAIT_OBJECT_0;
    திரும்பு WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#endif   // !FEAT_GUI_MSWIN || VIMDLL

    வெறுமை
mch_get_exe_name(வெறுமை)
{
    முழு		updated = FALSE;
    நிலையான முழு	enc_prev = -1;
    WCHAR	*path;
    size_t	pathlen;

    எனில் (exe_name == வெற்று || exe_pathw == வெற்று || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_PATH];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(வெற்று, buf, MAX_PATH);
	எனில் (p > buf)
	{
	    எனில் (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_to_enc(buf, வெற்று);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    செய்
	    {
		--p;
		எனில் (*p == L'\\')
		{
		    *p = L'\0';
		    நிறுத்து;
		}
	    } போது (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    எனில் (exe_pathw == வெற்று || !updated)
	திரும்பு;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    path = _wgetenv(L"PATH");
    pathlen = (path == வெற்று) ? 0 : wcslen(path);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#define EXTRA_LEN 7
#define MAX_ENV_STRING_LEN 8191

    எனில் (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   temp[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	எனில் (pathlen == 0)
	    wcscpy(temp + templen, exe_pathw);
	இல்லையெனில்
	{
	    wcscpy(temp + templen, path);

	    // Check if exe_path is already included in $PATH.
	    எனில் (wcsstr(temp, exe_pathw) == வெற்று)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		எனில் (temp[templen - 1] != L';')
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
    நிலையான வெறுமை
unescape_shellxquote(char_u *p, char_u *escaped)
{
    முழு	    l = (முழு)STRLEN(p);
    முழு	    n;

    போது (*p != NUL)
    {
	எனில் (*p == '^' && vim_strchr(escaped, p[1]) != வெற்று)
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
vimLoadLib(மாறிலி எழுத்து *name)
{
    HINSTANCE	dll = வெற்று;

    // No need to load any library when registering OLE.
    எனில் (found_register_arg)
	திரும்பு வெற்று;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    எனில் (exe_pathw == வெற்று)
    {
	mch_get_exe_name();
	எனில் (exe_pathw == வெற்று)
	    திரும்பு வெற்று;
    }

    WCHAR old_dirw[MAXPATHL];

    எனில் (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	திரும்பு வெற்று;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    SetCurrentDirectoryW(exe_pathw);
    dll = LoadLibrary(name);
    SetCurrentDirectoryW(old_dirw);
    திரும்பு dll;
}

#எனில் defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    முழு
mch_is_gui_executable(வெறுமை)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(வெற்று);
    PIMAGE_DOS_HEADER	pDOS = (PIMAGE_DOS_HEADER)pImage;
    PIMAGE_NT_HEADERS	pPE;

    எனில் (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	திரும்பு FALSE;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    எனில் (pPE->Signature != IMAGE_NT_SIGNATURE)
	திரும்பு FALSE;
    எனில் (pPE->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	திரும்பு TRUE;
    திரும்பு FALSE;
}
#endif

#எனில் defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    நிலையான வெறுமை *
get_imported_func_info(HINSTANCE hInst, மாறிலி எழுத்து *funcname, முழு info,
	மாறிலி வெறுமை *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_HEADER		pDOS = (PIMAGE_DOS_HEADER)hInst;
    PIMAGE_NT_HEADERS		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_DATA		pIAT;	    // Import Address Table
    PIMAGE_THUNK_DATA		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    எனில் (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	திரும்பு வெற்று;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    எனில் (pPE->Signature != IMAGE_NT_SIGNATURE)
	திரும்பு வெற்று;

    ImpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    எனில் (ImpVA == 0)
	திரும்பு வெற்று;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    க்கு (; pImpDesc->FirstThunk; ++pImpDesc)
    {
	எனில் (!pImpDesc->OriginalFirstThunk)
	    தொடர்;
	pIAT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->FirstThunk);
	pINT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->OriginalFirstThunk);
	க்கு (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    எனில் (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		தொடர்;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.AddressOfData));
	    எனில் (strcmp((எழுத்து *)pImpName->Name, funcname) == 0)
	    {
		வெறுமை *original;
		DWORD old, new = PAGE_READWRITE;

		தேர்வு (info)
		{
		    நிலை 0:
			திரும்பு (வெறுமை *)pIAT->u1.Function;
		    நிலை 1:
			திரும்பு (வெறுமை *)(pImage + pImpDesc->Name);
		    நிலை 2:
			original = (வெறுமை *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, அளவு(வெறுமை *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, அளவு(வெறுமை *),
				old, &new);
			திரும்பு original;
		    இயல்பு:
			திரும்பு வெற்று;
		}
	    }
	}
    }
    திரும்பு வெற்று;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
find_imported_module_by_funcname(HINSTANCE hInst, மாறிலி எழுத்து *funcname)
{
    எழுத்து    *modulename;

    modulename = (எழுத்து *)get_imported_func_info(hInst, funcname, 1, வெற்று);
    எனில் (modulename != வெற்று)
	திரும்பு GetModuleHandleA(modulename);
    திரும்பு வெற்று;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    வெறுமை *
get_dll_import_func(HINSTANCE hInst, மாறிலி எழுத்து *funcname)
{
    திரும்பு get_imported_func_info(hInst, funcname, 0, வெற்று);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    வெறுமை *
hook_dll_import_func(HINSTANCE hInst, மாறிலி எழுத்து *funcname, மாறிலி வெறுமை *hook)
{
    திரும்பு get_imported_func_info(hInst, funcname, 2, hook);
}
#endif

#எனில் defined(FEAT_PYTHON3)
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

    எனில் (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	திரும்பு வெற்று;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    எனில் (pPE->Signature != IMAGE_NT_SIGNATURE)
	திரும்பு வெற்று;

    ExpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    ExpSize = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    எனில் (ExpVA == 0)
	திரும்பு hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    pFunctionTable = (LPDWORD)(pImage + pExpDir->AddressOfFunctions);

    எனில் (pExpDir->எண்_Of_Names == 0)
	திரும்பு hInst;	// No export names.

    // Check only the first entry.
    எனில் ((pFunctionTable[0] < ExpVA) || (pFunctionTable[0] >= ExpVA + ExpSize))
	// The first entry is not a function forwarder.
	திரும்பு hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    மாறிலி எழுத்து *name = (மாறிலி எழுத்து *)(pImage + pFunctionTable[0]);
    மாறிலி எழுத்து *p = strchr(name, '.');
    எனில் (p == வெற்று)
	திரும்பு hInst;

    // Extract DllName.
    எழுத்து buf[MAX_PATH];
    எனில் (p - name + 1 > அளவு(buf))
	திரும்பு வெற்று;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    திரும்பு GetModuleHandleA(buf);
}
#endif

#எனில் defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
நிலையான எழுத்து *null_libintl_gettext(மாறிலி எழுத்து *);
நிலையான எழுத்து *null_libintl_ngettext(மாறிலி எழுத்து *, மாறிலி எழுத்து *, unsigned long n);
நிலையான எழுத்து *null_libintl_textdomain(மாறிலி எழுத்து *);
நிலையான எழுத்து *null_libintl_bindtextdomain(மாறிலி எழுத்து *, மாறிலி எழுத்து *);
நிலையான எழுத்து *null_libintl_bind_textdomain_codeset(மாறிலி எழுத்து *, மாறிலி எழுத்து *);
நிலையான முழு null_libintl_wputenv(மாறிலி wchar_t *);

நிலையான HINSTANCE hLibintlDLL = வெற்று;
எழுத்து *(*dyn_libintl_gettext)(மாறிலி எழுத்து *) = null_libintl_gettext;
எழுத்து *(*dyn_libintl_ngettext)(மாறிலி எழுத்து *, மாறிலி எழுத்து *, unsigned long n)
						= null_libintl_ngettext;
எழுத்து *(*dyn_libintl_textdomain)(மாறிலி எழுத்து *) = null_libintl_textdomain;
எழுத்து *(*dyn_libintl_bindtextdomain)(மாறிலி எழுத்து *, மாறிலி எழுத்து *)
						= null_libintl_bindtextdomain;
எழுத்து *(*dyn_libintl_bind_textdomain_codeset)(மாறிலி எழுத்து *, மாறிலி எழுத்து *)
				       = null_libintl_bind_textdomain_codeset;
முழு (*dyn_libintl_wputenv)(மாறிலி wchar_t *) = null_libintl_wputenv;
முழு *dyn_libintl_nl_செய்தி_cat_cntr = வெற்று;

    முழு
dyn_libintl_init(வெறுமை)
{
    முழு i;
    நிலையான கட்டமைப்பு
    {
	எழுத்து	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{வெற்று, வெற்று}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    எனில் (hLibintlDLL != வெற்று)
	திரும்பு 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vimLoadLib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    எனில் (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    எனில் (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT2);
# endif
    எனில் (!hLibintlDLL)
    {
	எனில் (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_load_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	திரும்பு 0;
    }
    க்கு (i = 0; libintl_entry[i].name != வெற்று
					 && libintl_entry[i].ptr != வெற்று; ++i)
    {
	எனில் ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == வெற்று)
	{
	    dyn_libintl_end();
	    எனில் (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_load_library_function_str), libintl_entry[i].name);
		verbose_leave();
	    }
	    திரும்பு 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (எழுத்து *(*)(மாறிலி எழுத்து *, மாறிலி எழுத்து *))
			GetProcAddress(hLibintlDLL, "bind_textdomain_codeset");
    எனில் (dyn_libintl_bind_textdomain_codeset == வெற்று)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = find_imported_module_by_funcname(hLibintlDLL, "getenv");
    எனில் (hmsvcrt != வெற்று)
	dyn_libintl_wputenv = (முழு (*)(மாறிலி wchar_t *))
					GetProcAddress(hmsvcrt, "_wputenv");
    எனில் (dyn_libintl_wputenv == வெற்று || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_செய்தி_cat_cntr = (முழு *)GetProcAddress(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    திரும்பு 1;
}

    வெறுமை
dyn_libintl_end(வெறுமை)
{
    எனில் (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= வெற்று;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_செய்தி_cat_cntr	= வெற்று;
}

    நிலையான எழுத்து *
null_libintl_gettext(மாறிலி எழுத்து *msgid)
{
    திரும்பு (எழுத்து*)msgid;
}

    நிலையான எழுத்து *
null_libintl_ngettext(
	மாறிலி எழுத்து *msgid,
	மாறிலி எழுத்து *msgid_plural,
	unsigned long n)
{
    திரும்பு (எழுத்து *)(n == 1 ? msgid : msgid_plural);
}

    நிலையான எழுத்து *
null_libintl_bindtextdomain(
	மாறிலி எழுத்து *domainname UNUSED,
	மாறிலி எழுத்து *dirname UNUSED)
{
    திரும்பு வெற்று;
}

    நிலையான எழுத்து *
null_libintl_bind_textdomain_codeset(
	மாறிலி எழுத்து *domainname UNUSED,
	மாறிலி எழுத்து *codeset UNUSED)
{
    திரும்பு வெற்று;
}

    நிலையான எழுத்து *
null_libintl_textdomain(மாறிலி எழுத்து *domainname UNUSED)
{
    திரும்பு வெற்று;
}

    நிலையான முழு
null_libintl_wputenv(மாறிலி wchar_t *envstring UNUSED)
{
    திரும்பு 0;
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
    நிலையான BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    எனில் (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	திரும்பு FALSE;

    எனில் (!Lookup_Privilege_மதிப்பு(வெற்று, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	திரும்பு FALSE;
    }

    tokenPrivileges.Privilege_மொத்தம்	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    எனில் (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, வெற்று, வெற்று))
    {
	CloseHandle(hToken);
	திரும்பு FALSE;
    }

    எனில் (GetLastError() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	திரும்பு FALSE;
    }

    CloseHandle(hToken);
    திரும்பு TRUE;
}
#endif

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    வெறுமை
PlatformId(வெறுமை)
{
#ifdef FEAT_EVAL
    DWORD major = (win_version >> 24) & 0xFF;
    DWORD minor = (win_version >> 16) & 0xFF;
    vim_snprintf(windowsVersion, அளவு(windowsVersion), "%d.%d", major, minor);
#endif

#ifdef HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    எனில் (!win32_enable_privilege(SE_SECURITY_NAME))
	திரும்பு;
#endif
}

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SHIFT  (SHIFT_PRESSED)
# define CTRL   (வலது_CTRL_PRESSED | இடது_CTRL_PRESSED)
# define ALT    (வலது_ALT_PRESSED  | இடது_ALT_PRESSED)
# define ALT_GR (வலது_ALT_PRESSED  | இடது_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

நிலையான மாறிலி கட்டமைப்பு
{
    WORD    w_Virt_சாவி;
    BOOL    f_Ansi_சாவி;
    முழு	    chAlone;
    முழு	    chShift;
    முழு	    chCtrl;
    முழு	    chAlt;
} Virt_சாவி_Map[] =
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
    { VK_இடது,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_வலது,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_END,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_DOWN,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_NEXT,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_செருகு,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_BACK,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# எனில் 0
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
    நிலையான முழு
win32_kbd_patch_சாவி(
    சாவி_EVENT_RECORD *pker)
{
    UINT uMods = pker->dw_Control_சாவி_State;
    நிலையான முழு s_iIsDead = 0;
    நிலையான WORD awAnsiCode[2];
    நிலையான BYTE abKeystate[256];

    எனில் (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[1];
	s_iIsDead = 0;
	திரும்பு 1;
    }

    // check if it already has a valid unicode character.
    எனில் (pker->uChar.UnicodeChar != 0)
	திரும்பு 1;

    CLEAR_FIELD(abKeystate);

    // Clear any pending dead keys
    ToUnicode(VK_SPACE, Map_Virtual_சாவி(VK_SPACE, 0), abKeystate, awAnsiCode, 2, 0);

    எனில் (uMods & SHIFT_PRESSED)
	abKeystate[VK_SHIFT] = 0x80;
    எனில் (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    எனில் ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ToUnicode(pker->w_Virtual_சாவி_Code, pker->wVirtualScanCode,
			abKeystate, awAnsiCode, 2, 0);

    எனில் (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[0];

    திரும்பு s_iIsDead;
}

நிலையான BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    நிலையான BOOL
decode_சாவி_event(
    சாவி_EVENT_RECORD	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    முழு			*pmodifiers,
    BOOL		fDoPost UNUSED)
{
    முழு i;
    மாறிலி முழு nModifs = pker->dw_Control_சாவி_State & (SHIFT | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    எனில் (!pker->b_சாவி_Down)
	திரும்பு FALSE;

    // ignore some keystrokes
    தேர்வு (pker->w_Virtual_சாவி_Code)
    {
    // modifiers
    நிலை VK_SHIFT:
    நிலை VK_CONTROL:
    நிலை VK_MENU:   // Alt key
	திரும்பு FALSE;

    இயல்பு:
	நிறுத்து;
    }

    // Shift-TAB
    எனில் (pker->w_Virtual_சாவி_Code == VK_TAB && (nModifs & SHIFT_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	திரும்பு TRUE;
    }

    க்கு (i = ARRAY_LENGTH(Virt_சாவி_Map); --i >= 0; )
    {
	எனில் (Virt_சாவி_Map[i].w_Virt_சாவி == pker->w_Virtual_சாவி_Code)
	{
	    *pch = Virt_சாவி_Map[i].chAlone;
	    எனில் ((nModifs & SHIFT) != 0)
		*pch = Virt_சாவி_Map[i].chShift;
	    இல்லையெனில் எனில் ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_சாவி_Map[i].chCtrl;
	    இல்லையெனில் எனில் ((nModifs & ALT) != 0)
		*pch = Virt_சாவி_Map[i].chAlt;

	    எனில் (*pch != 0)
	    {
		எனில் (Virt_சாவி_Map[i].f_Ansi_சாவி)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    எனில் (pmodifiers)
		    {
			எனில் (pker->w_Virtual_சாவி_Code >= VK_F1
			    && pker->w_Virtual_சாவி_Code <= VK_F12)
			{
			    எனில் ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_MASK_ALT;
				எனில் ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_சாவி_Map[i].chAlone;
			    }
			    எனில் ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				எனில் ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_சாவி_Map[i].chAlone;
			    }
			}
			இல்லையெனில் எனில் (pker->w_Virtual_சாவி_Code >= VK_END
				&& pker->w_Virtual_சாவி_Code <= VK_DOWN)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_சாவி_Map[i].chAlone;
			    எனில் ((nModifs & SHIFT) != 0
						    && (nModifs & ~SHIFT) == 0)
			    {
				*pch2 = Virt_சாவி_Map[i].chShift;
			    }
			    எனில் ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_சாவி_Map[i].chCtrl;
				எனில் (pker->w_Virtual_சாவி_Code == VK_UP
				    || pker->w_Virtual_சாவி_Code == VK_DOWN)
				{
				    *pmodifiers |= MOD_MASK_CTRL;
				    *pch2 = Virt_சாவி_Map[i].chAlone;
				}
			    }
			    எனில் ((nModifs & SHIFT) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				*pch2 = Virt_சாவி_Map[i].chShift;
			    }
			    எனில் ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_சாவி_Map[i].chAlt;
				*pmodifiers |= MOD_MASK_ALT;
				எனில் ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_சாவி_Map[i].chAlone;
				}
				இல்லையெனில் எனில் ((nModifs & SHIFT) != 0)
				{
				    *pch2 = Virt_சாவி_Map[i].chShift;
				}
				இல்லையெனில் எனில் ((nModifs & CTRL) != 0)
				{
				    எனில் (pker->w_Virtual_சாவி_Code == VK_UP
					|| pker->w_Virtual_சாவி_Code == VK_DOWN)
				    {
					*pmodifiers |= MOD_MASK_CTRL;
					*pch2 = Virt_சாவி_Map[i].chAlone;
				    }
				    இல்லையெனில்
				    {
					*pch2 = Virt_சாவி_Map[i].chCtrl;
				    }
				}
			    }
			}
			இல்லையெனில் எனில் (pker->w_Virtual_சாவி_Code == VK_செருகு
					&& (nModifs & SHIFT) != 0
					&& (nModifs & ~SHIFT) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_சாவி_Map[i].chShift;
			}
			இல்லையெனில்
			{
			    *pch2 = Virt_சாவி_Map[i].chAlone;
			    எனில் ((nModifs & SHIFT) != 0)
				*pmodifiers |= MOD_MASK_SHIFT;
			    எனில் ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_MASK_CTRL;
			    எனில் ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_MASK_ALT;
			}
		    }
		}

		திரும்பு TRUE;
	    }
	}
    }

    i = win32_kbd_patch_சாவி(pker);

    எனில் (i < 0)
	*pch = NUL;
    இல்லையெனில்
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	எனில் (pmodifiers != வெற்று)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    எனில் ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_MASK_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    எனில் ((nModifs & SHIFT) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_MASK_SHIFT;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    எனில் ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_MASK_CTRL;
	}
    }

    திரும்பு (*pch != NUL);
}

# எனில் defined(FEAT_EVAL)
    நிலையான முழு
encode_சாவி_event(dict_T *args, உள்ளீடு_RECORD *ir)
{
    நிலையான முழு s_dwMods = 0;

    char_u *action = dict_get_string(args, "event", TRUE);
    எனில் (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_சாவி_Down = STRICMP(action, "keydown") == 0;
	WORD vkCode = dict_get_எண்_def(args, "keycode", 0);
	எனில் (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    திரும்பு FALSE;
	}

	ir->EventType = சாவி_EVENT;
	சாவி_EVENT_RECORD ker;
	ZeroMemory(&ker, அளவு(ker));
	ker.b_சாவி_Down = is_சாவி_Down;
	ker.w_Repeat_மொத்தம் = 1;
	ker.wVirtualScanCode = 0;
	ker.dw_Control_சாவி_State = 0;
	முழு mods = (முழு)dict_get_எண்(args, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	எனில் (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    எனில் (mods & MOD_MASK_SHIFT)
		ker.dw_Control_சாவி_State |= SHIFT_PRESSED;
	    எனில் (mods & MOD_MASK_CTRL)
		ker.dw_Control_சாவி_State |= இடது_CTRL_PRESSED;
	    எனில் (mods & MOD_MASK_ALT)
		ker.dw_Control_சாவி_State |= இடது_ALT_PRESSED;
	}

	எனில் (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT)
	{
	    எனில் (is_சாவி_Down)
		s_dwMods |= SHIFT_PRESSED;
	    இல்லையெனில்
		s_dwMods &= ~SHIFT_PRESSED;
	}
	இல்லையெனில் எனில் (vkCode == VK_LCONTROL || vkCode == VK_CONTROL)
	{
	    எனில் (is_சாவி_Down)
		s_dwMods |= இடது_CTRL_PRESSED;
	    இல்லையெனில்
		s_dwMods &= ~இடது_CTRL_PRESSED;
	}
	இல்லையெனில் எனில் (vkCode == VK_RCONTROL)
	{
	    எனில் (is_சாவி_Down)
		s_dwMods |= வலது_CTRL_PRESSED;
	    இல்லையெனில்
		s_dwMods &= ~வலது_CTRL_PRESSED;
	}
	இல்லையெனில் எனில் (vkCode == VK_LMENU || vkCode == VK_MENU)
	{
	    எனில் (is_சாவி_Down)
		s_dwMods |= இடது_ALT_PRESSED;
	    இல்லையெனில்
		s_dwMods &= ~இடது_ALT_PRESSED;
	}
	இல்லையெனில் எனில் (vkCode == VK_RMENU)
	{
	    எனில் (is_சாவி_Down)
		s_dwMods |= வலது_ALT_PRESSED;
	    இல்லையெனில்
		s_dwMods &= ~வலது_ALT_PRESSED;
	}
	ker.dw_Control_சாவி_State |= s_dwMods;
	ker.w_Virtual_சாவி_Code = vkCode;
	ker.uChar.UnicodeChar = 0;
	ir->Event.சாவி_Event_2 = ker;
	vim_free(action);
    }
    இல்லையெனில்
    {
	எனில் (action == வெற்று)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	இல்லையெனில்
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "event", action);
	    vim_free(action);
	}
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}
# endif  // FEAT_EVAL
#endif // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
நிலையான முழு g_fMouseAvail = FALSE;   // mouse present
நிலையான முழு g_fMouseActive = FALSE;  // mouse enabled
நிலையான முழு g_nMouseClick = -1;	    // mouse status
நிலையான முழு g_xMouse;		    // mouse x coordinate
நிலையான முழு g_yMouse;		    // mouse y coordinate
நிலையான DWORD g_cmodein = 0;	    // Original console input mode
நிலையான DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    வெறுமை
mch_setmouse(முழு on)
{
    DWORD cmodein;

# ifdef VIMDLL
    எனில் (gui.in_use)
	திரும்பு;
# endif
    எனில் (!g_fMouseAvail)
	திரும்பு;

    g_fMouseActive = on;
    GetConsoleMode(g_hConIn, &cmodein);

    எனில் (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_உள்ளீடு;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    இல்லையெனில்
    {
	cmodein &= ~ENABLE_MOUSE_உள்ளீடு;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }

    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
}


# எனில் defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    வெறுமை
mch_bevalterm_changed(வெறுமை)
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
    நிலையான வெறுமை
decode_mouse_wheel(MOUSE_EVENT_RECORD *pmer)
{
    முழு	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    முழு	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    முழு lcol = g_xMouse;
    முழு lrow = g_yMouse;
    win_T *wp = mouse_find_win(&lrow, &lcol, FIND_POPUP);
    எனில் (wp != வெற்று && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_FIELD(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	எனில் (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_இடது : MSCR_வலது;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	இல்லையெனில்
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
    எனில் (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    இல்லையெனில்
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    எனில் (pmer->dw_Control_சாவி_State & SHIFT_PRESSED)
	modifiers |= MOD_MASK_SHIFT; // MOUSE_SHIFT;
    எனில் (pmer->dw_Control_சாவி_State & (வலது_CTRL_PRESSED | இடது_CTRL_PRESSED))
	modifiers |= MOD_MASK_CTRL; // MOUSE_CTRL;
    எனில் (pmer->dw_Control_சாவி_State & (வலது_ALT_PRESSED  | இடது_ALT_PRESSED))
	modifiers |= MOD_MASK_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    திரும்பு;
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
    நிலையான BOOL
decode_mouse_event(
    MOUSE_EVENT_RECORD *pmer)
{
    நிலையான முழு s_nOldButton = -1;
    நிலையான முழு s_nOldMouseClick = -1;
    நிலையான முழு s_xOldMouse = -1;
    நிலையான முழு s_yOldMouse = -1;
    நிலையான linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    நிலையான முழு s_old_topfill = 0;
# endif
    நிலையான முழு s_cClicks = 1;
    நிலையான BOOL s_fReleased = TRUE;
    நிலையான DWORD s_dwLastClickTime = 0;
    நிலையான BOOL s_f_Next_Is_நடு = FALSE;

    நிலையான DWORD cButtons = 0;	// number of buttons supported

    மாறிலி DWORD இடது_2 = FROM_இடது_1_ST_BUTTON_PRESSED;
    மாறிலி DWORD நடு = FROM_இடது_2_ND_BUTTON_PRESSED;
    மாறிலி DWORD வலது_2 = RIGHTMOST_BUTTON_PRESSED;
    மாறிலி DWORD இடது_வலது = இடது_2 | வலது_2;

    முழு nButton;

    எனில் (cButtons == 0 && !Get_எண்_Of_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    எனில் (!g_fMouseAvail || !g_fMouseActive)
    {
	g_nMouseClick = -1;
	திரும்பு FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    எனில் (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	திரும்பு FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    எனில் (g_nMouseClick != -1)
	திரும்பு TRUE;

    எனில் (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	திரும்பு TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    எனில் (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	எனில் (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    திரும்பு FALSE;
    }

    // If no buttons are pressed...
    எனில் ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	எனில் (s_fReleased)
	{
# ifdef FEAT_BEVAL_TERM
	    // do return mouse move events when we want them
	    எனில் (p_bevalterm)
		nButton = MOUSE_DRAG;
	    இல்லையெனில்
# endif
		திரும்பு FALSE;
	}

	s_fReleased = TRUE;
    }
    இல்லையெனில்    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	எனில் (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & இடது_வலது);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    எனில் (dwLR == இடது_2 || dwLR == வலது_2)
	    {
		க்கு (;;)
		{
		    // wait a short time for next input event
		    எனில் (WaitForSingleObject(g_hConIn, p_mouset / 3)
							     != WAIT_OBJECT_0)
			நிறுத்து;
		    இல்லையெனில்
		    {
			DWORD cRecords = 0;
			உள்ளீடு_RECORD ir;
			MOUSE_EVENT_RECORD* pmer2 = &ir.Event.MouseEvent;

			peek_console_உள்ளீடு(g_hConIn, &ir, 1, &cRecords);

			எனில் (cRecords == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & இடது_வலது))
			    நிறுத்து;
			இல்லையெனில்
			{
			    எனில் (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_உள்ளீடு(g_hConIn, &ir, 1, &cRecords);

				திரும்பு decode_mouse_event(pmer2);
			    }
			    இல்லையெனில் எனில் (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_உள்ளீடு(g_hConIn, &ir, 1, &cRecords);

				// are there any more mouse events in queue?
				peek_console_உள்ளீடு(g_hConIn, &ir, 1, &cRecords);

				எனில் (cRecords==0 || ir.EventType != MOUSE_EVENT)
				    நிறுத்து;
			    }
			    இல்லையெனில்
				நிறுத்து;
			}
		    }
		}
	    }
	}

	எனில் (s_f_Next_Is_நடு)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_நடு;
	    s_f_Next_Is_நடு = FALSE;
	}
	இல்லையெனில் எனில் (cButtons == 2	&&
	    ((pmer->dwButtonState & இடது_வலது) == இடது_வலது))
	{
	    nButton = MOUSE_நடு;

	    எனில் (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_Next_Is_நடு = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	இல்லையெனில் எனில் ((pmer->dwButtonState & இடது_2) == இடது_2)
	    nButton = MOUSE_இடது;
	இல்லையெனில் எனில் ((pmer->dwButtonState & நடு) == நடு)
	    nButton = MOUSE_நடு;
	இல்லையெனில் எனில் ((pmer->dwButtonState & வலது_2) == வலது_2)
	    nButton = MOUSE_வலது;

	எனில் (! s_fReleased && ! s_f_Next_Is_நடு
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    திரும்பு FALSE;

	s_fReleased = s_f_Next_Is_நடு;
    }

    எனில் (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	எனில் (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dwCurrentTime = Get_Tick_மொத்தம்();

	    எனில் (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (முழு)(dwCurrentTime - s_dwLastClickTime) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    இல்லையெனில் எனில் (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dwLastClickTime = dwCurrentTime;
	}
    }
    இல்லையெனில் எனில் (pmer->dwEventFlags == MOUSE_MOVED)
    {
	எனில் (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    எனில் (nButton == -1)
	திரும்பு FALSE;

    எனில் (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    எனில் (pmer->dw_Control_சாவி_State & SHIFT_PRESSED)
	g_nMouseClick |= MOUSE_SHIFT;
    எனில் (pmer->dw_Control_சாவி_State & (வலது_CTRL_PRESSED | இடது_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    எனில் (pmer->dw_Control_சாவி_State & (வலது_ALT_PRESSED  | இடது_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    எனில் (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	SET_இலக்கம்_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    எனில் (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	திரும்பு FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    திரும்பு TRUE;
}

# ifdef FEAT_EVAL
    நிலையான முழு
encode_mouse_event(dict_T *args, உள்ளீடு_RECORD *ir)
{
    முழு		button;
    முழு		row;
    முழு		col;
    முழு		repeated_click;
    int_u	mods = 0;
    முழு		move;

    எனில் (!dict_has_சாவி(args, "row") || !dict_has_சாவி(args, "col"))
	திரும்பு FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (முழு)dict_get_bool(args, "move", FALSE);
    எனில் (!move && (!dict_has_சாவி(args, "button")
	    || !dict_has_சாவி(args, "multiclick")
	    || !dict_has_சாவி(args, "modifiers")))
	திரும்பு FALSE;

    row = (முழு)dict_get_எண்(args, "row") - 1;
    col = (முழு)dict_get_எண்(args, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_RECORD mer;
    ZeroMemory(&mer, அளவு(mer));
    mer.dwMousePosition.X  = col;
    mer.dwMousePosition.Y  = row;

    எனில் (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    இல்லையெனில்
    {
	button = (முழு)dict_get_எண்(args, "button");
	repeated_click = (முழு)dict_get_எண்(args, "multiclick");
	mods = (முழு)dict_get_எண்(args, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	தேர்வு (button)
	{
	    நிலை MOUSE_இடது:
		mer.dwButtonState = FROM_இடது_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		நிறுத்து;
	    நிலை MOUSE_நடு:
		mer.dwButtonState = FROM_இடது_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		நிறுத்து;
	    நிலை MOUSE_வலது:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		நிறுத்து;
	    நிலை MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    நிலை MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		நிறுத்து;
	    நிலை MOUSE_X1:
		mer.dwButtonState = FROM_இடது_3_RD_BUTTON_PRESSED;
		நிறுத்து;
	    நிலை MOUSE_X2:
		mer.dwButtonState = FROM_இடது_4_TH_BUTTON_PRESSED;
		நிறுத்து;
	    நிலை MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		நிறுத்து;
	    நிலை MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		நிறுத்து;
	    நிலை MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		நிறுத்து;
	    நிலை MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		நிறுத்து;
	    இயல்பு:
		semsg(_(e_invalid_argument_str), "button");
		திரும்பு FALSE;
	}
    }

    mer.dw_Control_சாவி_State = 0;
    எனில் (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	எனில் (mods & MOUSE_SHIFT)
	    mer.dw_Control_சாவி_State |= SHIFT_PRESSED;
	எனில் (mods & MOUSE_CTRL)
	    mer.dw_Control_சாவி_State |= இடது_CTRL_PRESSED;
	எனில் (mods & MOUSE_ALT)
	    mer.dw_Control_சாவி_State |= இடது_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    திரும்பு TRUE;
}
# endif  // FEAT_EVAL

    நிலையான முழு
write_உள்ளீடு_record_buffer(உள்ளீடு_RECORD* irEvents, முழு nLength)
{
    முழு				n_மொத்தம் = 0;
    உள்ளீடு_record_buffer_முனை_T	*event_முனை;

    போது (n_மொத்தம் < nLength)
    {
	event_முனை = alloc(அளவு(உள்ளீடு_record_buffer_முனை_T));
	எனில் (event_முனை == வெற்று)
	    திரும்பு -1;

	event_முனை->ir = irEvents[n_மொத்தம்++];
	event_முனை->next = வெற்று;

	உள்ளீடு_record_buffer.length++;
	எனில் (உள்ளீடு_record_buffer.tail == வெற்று)
	{
	    உள்ளீடு_record_buffer.head = event_முனை;
	    உள்ளீடு_record_buffer.tail = event_முனை;
	}
	இல்லையெனில்
	{
	    உள்ளீடு_record_buffer.tail->next = event_முனை;
	    உள்ளீடு_record_buffer.tail = உள்ளீடு_record_buffer.tail->next;
	}
    }
    திரும்பு n_மொத்தம்;
}

    நிலையான முழு
read_உள்ளீடு_record_buffer(உள்ளீடு_RECORD* irEvents, முழு nMaxLength)
{
    முழு n_மொத்தம் = 0;
    போது (n_மொத்தம் < nMaxLength && உள்ளீடு_record_buffer.head != வெற்று)
    {
	உள்ளீடு_record_buffer.length--;
	உள்ளீடு_record_buffer_முனை_T *எடு_head = உள்ளீடு_record_buffer.head;
	irEvents[n_மொத்தம்++] = எடு_head->ir;
	உள்ளீடு_record_buffer.head = எடு_head->next;
	vim_free(எடு_head);
	எனில் (உள்ளீடு_record_buffer.length == 0)
	    உள்ளீடு_record_buffer.tail = வெற்று;
    }
    திரும்பு n_மொத்தம்;
}
#இல்லையெனில்  // FEAT_GUI_MSWIN && !VIMDLL
    வெறுமை
mch_setmouse(முழு on UNUSED)
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
    முழு
test_mswin_event(char_u *event, dict_T *args)
{
    முழு lpEventsWritten = 0;

# எனில் defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    எனில் (gui.in_use)
	திரும்பு test_gui_w32_sendevent(event, args);
# endif

# எனில் defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    உள்ளீடு_RECORD ir;
    BOOL உள்ளீடு_encoded = FALSE;
    BOOL execute = FALSE;
    எனில் (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(args, "execute", FALSE);
	எனில் (dict_has_சாவி(args, "event"))
	    உள்ளீடு_encoded = encode_சாவி_event(args, &ir);
	இல்லையெனில் எனில் (!execute)
	{
	    semsg(_(e_missing_argument_str), "event");
	    திரும்பு FALSE;
	}
    }
    இல்லையெனில் எனில் (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	உள்ளீடு_encoded = encode_mouse_event(args, &ir);
    }
    இல்லையெனில்
    {
	semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "event", event);
	திரும்பு FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    எனில் (உள்ளீடு_encoded)
    {
	எனில் ((lpEventsWritten = write_உள்ளீடு_record_buffer(&ir, 1)) < 0)
	{
	    semsg(_(e_out_of_memory), "event");
	    திரும்பு FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    எனில் (execute)
    {
	முழு save_செய்தி_scroll = செய்தி_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	செய்தி_scroll = FALSE;
	ch_log(வெற்று, "test_mswin_event() executing");
	exec_normal(TRUE, TRUE, TRUE);
	செய்தி_scroll |= save_செய்தி_scroll;
    }

# endif
    திரும்பு lpEventsWritten;
}
#endif // FEAT_EVAL

#எனில் defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    நிலையான வெறுமை
mch_set_cursor_shape(முழு thickness)
{
    எனில் (vtp_working)
    {
	எனில் (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    எனில் (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    இல்லையெனில்
		vtp_printf("\033[0 q");	// default
	}
    }
    இல்லையெனில்
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dwSize = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	SetConsoleCursorInfo(g_hConOut, &ConsoleCursorInfo);
	எனில் (s_cursor_visible)
	    SetConsoleCursorPosition(g_hConOut, g_coord);
    }
}

    வெறுமை
mch_update_cursor(வெறுமை)
{
    முழு		idx;
    முழு		thickness;

# ifdef VIMDLL
    எனில் (gui.in_use)
	திரும்பு;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    எனில் (shape_table[idx].shape == SHAPE_BLOCK)
	thickness = 99;	// 100 doesn't work on W95
    இல்லையெனில்
	thickness = shape_table[idx].percentage;
    mch_set_cursor_shape(thickness);
}
#endif

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    நிலையான வெறுமை
handle_focus_event(உள்ளீடு_RECORD ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
    ui_focus_change((முழு)g_fJustGotFocus);
}

நிலையான வெறுமை ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    நிலையான முழு
WaitForChar(long msec, முழு ignore_உள்ளீடு)
{
    DWORD	    dwNow = 0, dwEndTime = 0;
    உள்ளீடு_RECORD    ir;
    DWORD	    cRecords;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    முழு		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    எனில் (msec > 0)
	// Wait until the specified time has elapsed.
	dwEndTime = Get_Tick_மொத்தம்() + msec;
    இல்லையெனில் எனில் (msec < 0)
	// Wait forever.
	dwEndTime = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    க்கு (;;)
    {
	// Only process messages when waiting.
	எனில் (msec != 0)
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

	எனில் (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_உள்ளீடு && உள்ளீடு_available())
# endif
	   )
	    திரும்பு TRUE;

	எனில் (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_மொத்தம்();
	    எனில் ((முழு)(dwNow - dwEndTime) >= 0)
		நிறுத்து;
	}
	எனில் (msec != 0)
	{
	    DWORD dwWaitTime = dwEndTime - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    எனில் (dwWaitTime > 11)
		dwWaitTime = 11;

# ifdef FEAT_MZSCHEME
	    எனில் (mzthreads_allowed() && p_mzq > 0 && (long)dwWaitTime > p_mzq)
		dwWaitTime = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    எனில் (dwWaitTime > 10)
	    {
		long	due_time;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_time = check_due_timer();
		எனில் (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    திரும்பு FALSE;
		}
		எனில் (due_time > 0 && dwWaitTime > (DWORD)due_time)
		    dwWaitTime = due_time;
	    }
# endif
	    எனில் (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    செய்தி_wait_for_multiple_objects(1, &g_hConIn, FALSE,
				  dwWaitTime, QS_SENDMESSAGE) != WAIT_OBJECT_0
# இல்லையெனில்
		    wait_for_single_object(g_hConIn, dwWaitTime)
							      != WAIT_OBJECT_0
# endif
		    )
		தொடர்;
	}

	cRecords = 0;
	peek_console_உள்ளீடு(g_hConIn, &ir, 1, &cRecords);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	எனில் (State == MODE_CMDLINE && செய்தி_row == Rows - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_BUFFER_INFO csbi;

	    எனில் (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	    {
		எனில் (csbi.dwCursorPosition.Y != செய்தி_row)
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

	எனில் (cRecords > 0)
	{
	    எனில் (ir.EventType == சாவி_EVENT && ir.Event.சாவி_Event_2.b_சாவி_Down)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		எனில் (ir.Event.சாவி_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.சாவி_Event_2.w_Virtual_சாவி_Code == 13)
		{
		    read_console_உள்ளீடு(g_hConIn, &ir, 1, &cRecords);
		    தொடர்;
		}
# endif
		எனில் (decode_சாவி_event(&ir.Event.சாவி_Event_2, &ch, &ch2,
								 வெற்று, FALSE))
		    திரும்பு TRUE;
	    }

	    read_console_உள்ளீடு(g_hConIn, &ir, 1, &cRecords);

	    எனில் (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    இல்லையெனில் எனில் (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    {
		COORD dwSize = ir.Event.WindowBufferSizeEvent.dwSize;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		எனில் (dwSize.X != Columns || dwSize.Y != Rows)
		{
		    CONSOLE_SCREEN_BUFFER_INFO csbi;
		    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
		    dwSize.X = csbi.srWindow.வலது_3 - csbi.srWindow.இடது_3 + 1;
		    dwSize.Y = csbi.srWindow.Bottom - csbi.srWindow.உச்சி_2 + 1;
		    எனில் (dwSize.X != Columns || dwSize.Y != Rows)
		    {
			ResizeConBuf(g_hConOut, dwSize);
			shell_resized();
		    }
		}
	    }
	    இல்லையெனில் எனில் (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		திரும்பு TRUE;
	}
	இல்லையெனில் எனில் (msec == 0)
	    நிறுத்து;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    எனில் (உள்ளீடு_available())
	திரும்பு TRUE;
# endif

    திரும்பு FALSE;
}

/*
 * return non-zero if a character is available
 */
    முழு
mch_char_avail(வெறுமை)
{
# ifdef VIMDLL
    எனில் (gui.in_use)
	திரும்பு TRUE;
# endif
    திரும்பு WaitForChar(0L, FALSE);
}

# எனில் defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    முழு
mch_check_messages(வெறுமை)
{
#  ifdef VIMDLL
    எனில் (gui.in_use)
	திரும்பு TRUE;
#  endif
    திரும்பு WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    நிலையான வெறுமை
create_conin(வெறுமை)
{
    g_hConIn =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES) வெற்று,
			OPEN_EXISTING, 0, (HANDLE)வெற்று);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    நிலையான WCHAR
tgetch(முழு *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    க்கு (;;)
    {
	உள்ளீடு_RECORD ir;
	DWORD cRecords = 0;

# ifdef FEAT_CLIENTSERVER
	(வெறுமை)WaitForChar(-1L, FALSE);
	எனில் (உள்ளீடு_available())
	    திரும்பு 0;
	எனில் (g_nMouseClick != -1)
	    திரும்பு 0;
# endif
	எனில் (read_console_உள்ளீடு(g_hConIn, &ir, 1, &cRecords) == 0)
	{
	    எனில் (did_create_conin)
		read_error_exit();
	    create_conin();
	    தொடர்;
	}

	எனில் (ir.EventType == சாவி_EVENT)
	{
	    எனில் (decode_சாவி_event(&ir.Event.சாவி_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		திரும்பு ch;
	}
	இல்லையெனில் எனில் (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	இல்லையெனில் எனில் (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    shell_resized();
	இல்லையெனில் எனில் (ir.EventType == MOUSE_EVENT)
	{
	    எனில் (decode_mouse_event(&ir.Event.MouseEvent))
		திரும்பு 0;
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
    முழு
mch_inchar(
    char_u	*buf UNUSED,
    முழு		maxlen UNUSED,
    long	time UNUSED,
    முழு		tb_change_cnt UNUSED)
{
#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    முழு		len;
    முழு		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# இல்லையெனில்
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    நிலையான char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    நிலையான முழு	    typeaheadlen = 0;

# ifdef VIMDLL
    எனில் (gui.in_use)
	திரும்பு 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    எனில் (typeaheadlen > 0)
	செல் theend;

    எனில் (time >= 0)
    {
	எனில் (!WaitForChar(time, FALSE))     // no character available
	    திரும்பு 0;
    }
    இல்லையெனில்    // time == -1, wait forever
    {
	mch_set_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	எனில் (!WaitForChar(p_ut, FALSE))
	{
	    எனில் (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (முழு)KE_CURSORHOLD;
		திரும்பு 3;
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
    எனில் (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    போது ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	எனில் (typebuf_changed(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    நிறுத்து;
	}
	எனில் (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    எனில் (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    எனில் (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		எனில் (modifiers > 0)
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
	    இல்லையெனில்
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
	இல்லையெனில்
	{
	    WCHAR	ch2 = NUL;
	    முழு		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_சாவி(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_சாவிகள்;
	    c = may_adjust_சாவி_for_ctrl(modifiers, c);
	    --no_reduce_சாவிகள்;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_shift_modifier(modifiers, c);

	    எனில் (typebuf_changed(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		நிறுத்து;
	    }

	    எனில் (c == Ctrl_C && ctrl_c_interrupts)
	    {
# எனில் defined(FEAT_CLIENTSERVER)
		trash_உள்ளீடு_buf();
# endif
		got_int = TRUE;
	    }

	    எனில் (g_nMouseClick == -1)
	    {
		முழு	n = 1;

		எனில் (ch2 == NUL)
		{
		    முழு	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    எனில் (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_to_enc(ch, &n);
		    எனில் (p != வெற்று)
		    {
			க்கு (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    எனில் (p[i] == CSI)
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
		இல்லையெனில்
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    எனில் (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		எனில் (ch2 != NUL)
		{
		    எனில் (c == K_NUL)
		    {
			தேர்வு (ch2)
			{
			நிலை (WCHAR)'\324': // SHIFT+Insert
			நிலை (WCHAR)'\325': // CTRL+Insert
			நிலை (WCHAR)'\327': // SHIFT+Delete
			நிலை (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    நிறுத்து;

			இயல்பு:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    நிறுத்து;
			}
		    }
		    இல்லையெனில்
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
		எனில் ((modifiers & MOD_MASK_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_MASK_ALT;
		}

		எனில் (modifiers != 0)
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
		எனில் (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    எனில் (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    போது (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    எனில் (len > 0)
    {
	buf[len] = NUL;
	ch_log(வெற்று, "raw key input: \"%s\"", buf);
    }
# endif
    திரும்பு len;

#இல்லையெனில் // FEAT_GUI_MSWIN && !VIMDLL
    திரும்பு 0;
#endif // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    நிலையான முழு
executable_file(எழுத்து *name, char_u **path)
{
    முழு attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    எனில் (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	திரும்பு FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    எனில் (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	எனில் (res == வெற்று)
	    res = resolve_reparse_point((char_u *)name);
	எனில் (res == வெற்று)
	    திரும்பு FALSE;
	// The path is already absolute.
	எனில் (path != வெற்று)
	    *path = res;
	இல்லையெனில்
	    vim_free(res);
    }
    இல்லையெனில் எனில் (path != வெற்று)
	*path = FullName_save((char_u *)name, FALSE);
    திரும்பு TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    நிலையான bool
executable_prefix_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_FIND_DATAW	data;
    HANDLE		handle;
    DWORD		error;

    wn = enc_to_utf16(name, வெற்று);
    எனில் (wn == வெற்று)
	திரும்பு true;
    handle = FindFirstFileExW(wn, FindExInfoBasic, &data,
					    Find_Ex_தேடல்_Name_Match, வெற்று, 0);
    error = GetLastError();
    vim_free(wn);
    எனில் (handle != INVALID_HANDLE_மதிப்பு)
    {
	FindClose(handle);
	திரும்பு true;
    }
    திரும்பு error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    நிலையான முழு
executable_exists(
    எழுத்து	*name,
    size_t	namelen,
    char_u	**path,
    முழு		use_path,
    முழு		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3];
    char_u	*p;
    string_T	pathbuf = {வெற்று, 0};
    முழு		pathbuf_allocated = FALSE;
    string_T	pathext = {வெற்று, 0};
    முழு		pathext_allocated = FALSE;
    char_u	*shname = வெற்று;
    முழு		noext = FALSE;
    bool	simple_pathext = true;
    முழு		ext_மொத்தம் = 0;
    முழு		retval = FALSE;

    எனில் (namelen >= அளவு(buf))	// safety check
	திரும்பு FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    எனில் (strstr((எழுத்து *)shname, "sh") != வெற்று &&
	!(strstr((எழுத்து *)shname, "powershell") != வெற்று
				    || strstr((எழுத்து *)shname, "pwsh") != வெற்று))
	noext = TRUE;

    எனில் (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	எனில் (pathext.string == வெற்று)
	    STR_LITERAL_SET(pathext, ".com;.exe;.bat;.cmd");
	இல்லையெனில்
	    pathext.length = STRLEN(pathext.string);

	எனில் (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    போது (*p)
	    {
		எனில் (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    எனில் (*p == '.')
			simple_pathext = false;
		    ++p;
		    தொடர்;
		}
		e = vim_strchr(p, ';');
		எனில் (e == வெற்று)
		    e = pathext.string + pathext.length;
		plen = (size_t)(e - p);
		++ext_மொத்தம்;
		எனில் (*p != '.')
		    simple_pathext = false;
		க்கு (q = p + 1; q < e; ++q)
		    எனில் (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		எனில் (_strnicoll(name + namelen - plen, (எழுத்து *)p, plen) == 0)
		{
		    noext = TRUE;
		    நிறுத்து;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    எனில் (pathext.string == வெற்று)
	STR_LITERAL_SET(pathext, ".");
    இல்லையெனில் எனில் (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.length + 3);
	எனில் (tmp == வெற்று)
	{
	    retval = FALSE;
	    செல் theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.length += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    எனில் (use_path && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	எனில் (p != வெற்று)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    எனில் (pathbuf.string == வெற்று)
	    {
		retval = FALSE;
		செல் theend;
	    }

	    எனில் (mch_getenv("NoDefaultCurrentDirectoryInExePath") == வெற்று &&
		    strstr((எழுத்து *)gettail(p_sh), "cmd.exe") != வெற்று)
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
    எனில் (pathbuf.string == வெற்று)
	STR_LITERAL_SET(pathbuf, ".");
    p = pathbuf.string;
    போது (*p)
    {
	char_u	*e;
	size_t	buflen;

	எனில் (*p == ';') // Skip empty entry
	{
	    ++p;
	    தொடர்;
	}
	e = vim_strchr(p, ';');
	எனில் (e == வெற்று)
	    e = pathbuf.string + pathbuf.length;

	எனில் (e - p + namelen + 2 > அளவு(buf))
	{
	    retval = FALSE;
	    செல் theend;
	}
	// A single "." that means current dir.
	எனில் (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	இல்லையெனில்
	{
	    buflen = vim_snprintf_safelen(
		(எழுத்து *)buf,
		அளவு(buf),
		"%.*s%s%s", (முழு)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	எனில் (!noext && simple_pathext && ext_மொத்தம் >= 3 && buflen > 0
		&& buflen + 2 <= அளவு(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == வெற்று
		&& vim_strchr(gettail(buf), ':') == வெற்று)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_prefix_exists(buf);
	    buf[buflen] = NUL;
	    எனில் (!exists)
	    {
		p = e;
		தொடர்;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	போது (*p)
	{
	    char_u  *e2;

	    எனில் (*p == ';')
	    {
		// Skip empty entry
		++p;
		தொடர்;
	    }
	    e2 = vim_strchr(p, ';');
	    எனில் (e2 == வெற்று)
		e2 = pathext.string + pathext.length;

	    எனில் (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		எனில் (e2 - p + buflen + 1 > அளவு(buf))
		{
		    retval = FALSE;
		    செல் theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    எனில் (executable_file((எழுத்து *)buf, path))
	    {
		retval = TRUE;
		செல் theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    எனில் (pathbuf_allocated)
	விடுவி(pathbuf.string);
    எனில் (pathext_allocated)
	விடுவி(pathext.string);
    திரும்பு retval;
}

#எனில் (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    நிலையான வெறுமை
bad_param_handler(மாறிலி wchar_t *expression UNUSED,
    மாறிலி wchar_t *function UNUSED,
    மாறிலி wchar_t *file UNUSED,
    unsigned முழு வரி UNUSED,
    uintptr_t pReserved UNUSED)
{
}

# define SET_INVALID_PARAM_HANDLER \
	((வெறுமை)_set_invalid_parameter_handler(bad_param_handler))
#இல்லையெனில்
# define SET_INVALID_PARAM_HANDLER
#endif

#ifdef FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    நிலையான வெறுமை
mch_init_g(வெறுமை)
{
# ifndef __MINGW32__
    வெளி முழு _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_இருமம்;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    Rows = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[_MAX_PATH + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	எனில் (exe_pathlen + 10 >= அளவு(vimrun_location))
	{
	    எனில் (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    வெற்று, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	இல்லையெனில்
	{
	    // First try in same directory as gvim.exe.
	    எனில் (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    எனில் (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		எனில் (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(எழுத்து *)vimrun_location,
			அளவு(vimrun_location),
			"\"%.*svimrun\" ",
			(முழு)exe_pathlen, exe_name);
		}
		இல்லையெனில்
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		எனில் (p != வெற்று)
		{
		    vimrun_path.string = p;
		    vimrun_path.length = plen;
		    vimrun_path_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    இல்லையெனில் எனில் (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    வெற்று, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	எனில் (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    எனில் (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    வெற்று, TRUE, FALSE))
	set_option_மதிப்பு_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#endif // FEAT_GUI_MSWIN

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).வலது_3 - (sr).இடது_3 + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).உச்சி_2 + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    நிலையான BOOL
ClearConsoleBuffer(WORD wAttribute)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    DWORD இலக்கம்_Cells, dummy;

    எனில் (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	திரும்பு FALSE;

    இலக்கம்_Cells = csbi.dwSize.X * csbi.dwSize.Y;
    coord.X = 0;
    coord.Y = 0;
    எனில் (!FillConsoleOutputCharacter(g_hConOut, ' ', இலக்கம்_Cells,
	    coord, &dummy))
	திரும்பு FALSE;
    எனில் (!FillConsoleOutputAttribute(g_hConOut, wAttribute, இலக்கம்_Cells,
	    coord, &dummy))
	திரும்பு FALSE;

    திரும்பு TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    நிலையான BOOL
FitConsoleWindow(
    COORD dwBufferSize,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD dwWindowSize;
    BOOL NeedAdjust = FALSE;

    எனில் (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	திரும்பு FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    எனில் (csbi.srWindow.வலது_3 >= dwBufferSize.X)
    {
	dwWindowSize.X = SRWIDTH(csbi.srWindow);
	எனில் (dwWindowSize.X > dwBufferSize.X)
	    dwWindowSize.X = dwBufferSize.X;
	csbi.srWindow.வலது_3 = dwBufferSize.X - 1;
	csbi.srWindow.இடது_3 = dwBufferSize.X - dwWindowSize.X;
	NeedAdjust = TRUE;
    }
    எனில் (csbi.srWindow.Bottom >= dwBufferSize.Y)
    {
	dwWindowSize.Y = SRHEIGHT(csbi.srWindow);
	எனில் (dwWindowSize.Y > dwBufferSize.Y)
	    dwWindowSize.Y = dwBufferSize.Y;
	csbi.srWindow.Bottom = dwBufferSize.Y - 1;
	csbi.srWindow.உச்சி_2 = dwBufferSize.Y - dwWindowSize.Y;
	NeedAdjust = TRUE;
    }
    எனில் (NeedAdjust && WantAdjust)
    {
	எனில் (!SetConsoleWindowInfo(g_hConOut, TRUE, &csbi.srWindow))
	    திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

வகைவரை கட்டமைப்பு ConsoleBufferStruct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_BUFFER_INFO	Info;
    PCHAR_INFO			Buffer;
    COORD			BufferSize;
    PSMALL_RECT			Regions;
    முழு				இலக்கம்_Regions;
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
    நிலையான BOOL
SaveConsoleBuffer(
    ConsoleBuffer *cb)
{
    DWORD இலக்கம்_Cells;
    COORD BufferCoord;
    SMALL_RECT ReadRegion;
    WORD Y, Y_incr;
    முழு i, numregions;

    எனில் (cb == வெற்று)
	திரும்பு FALSE;

    எனில் (!GetConsoleScreenBufferInfo(g_hConOut, &cb->Info))
    {
	cb->IsValid = FALSE;
	திரும்பு FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    எனில் (use_alternate_screen_buffer)
	திரும்பு TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    எனில் (!cb->IsValid || cb->Buffer == வெற்று ||
	    cb->BufferSize.X != cb->Info.dwSize.X ||
	    cb->BufferSize.Y != cb->Info.dwSize.Y)
    {
	cb->BufferSize.X = cb->Info.dwSize.X;
	cb->BufferSize.Y = cb->Info.dwSize.Y;
	இலக்கம்_Cells = cb->BufferSize.X * cb->BufferSize.Y;
	vim_free(cb->Buffer);
	cb->Buffer = ALLOC_MULT(CHAR_INFO, இலக்கம்_Cells);
	எனில் (cb->Buffer == வெற்று)
	    திரும்பு FALSE;
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
    ReadRegion.இடது_3 = 0;
    ReadRegion.வலது_3 = cb->Info.dwSize.X - 1;
    Y_incr = 12000 / cb->Info.dwSize.X;

    numregions = (cb->Info.dwSize.Y + Y_incr - 1) / Y_incr;
    எனில் (cb->Regions == வெற்று || numregions != cb->இலக்கம்_Regions)
    {
	cb->இலக்கம்_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->இலக்கம்_Regions);
	எனில் (cb->Regions == வெற்று)
	{
	    VIM_CLEAR(cb->Buffer);
	    திரும்பு FALSE;
	}
    }

    க்கு (i = 0, Y = 0; i < cb->இலக்கம்_Regions; i++, Y += Y_incr)
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
	ReadRegion.உச்சி_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	எனில் (!ReadConsoleOutputW(g_hConOut,	// output handle
		cb->Buffer,			// our buffer
		cb->BufferSize,			// dimensions of our buffer
		BufferCoord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->Buffer);
	    VIM_CLEAR(cb->Regions);
	    திரும்பு FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    திரும்பு TRUE;
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
    நிலையான BOOL
RestoreConsoleBuffer(
    ConsoleBuffer   *cb,
    BOOL	    RestoreScreen)
{
    COORD BufferCoord;
    SMALL_RECT WriteRegion;
    முழு i;

    எனில் (cb == வெற்று || !cb->IsValid)
	திரும்பு FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    எனில் (RestoreScreen)
	ClearConsoleBuffer(cb->Info.wAttributes);

    FitConsoleWindow(cb->Info.dwSize, TRUE);
    எனில் (!SetConsoleScreenBufferSize(g_hConOut, cb->Info.dwSize))
	திரும்பு FALSE;
    எனில் (!SetConsoleTextAttribute(g_hConOut, cb->Info.wAttributes))
	திரும்பு FALSE;

    எனில் (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	திரும்பு TRUE;
    }

    எனில் (!SetConsoleCursorPosition(g_hConOut, cb->Info.dwCursorPosition))
	திரும்பு FALSE;
    எனில் (!SetConsoleWindowInfo(g_hConOut, TRUE, &cb->Info.srWindow))
	திரும்பு FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    எனில் (use_alternate_screen_buffer)
	திரும்பு TRUE;

    /*
     * Restore the screen buffer contents.
     */
    எனில் (cb->Buffer != வெற்று)
    {
	க்கு (i = 0; i < cb->இலக்கம்_Regions; i++)
	{
	    BufferCoord.X = cb->Regions[i].இடது_3;
	    BufferCoord.Y = cb->Regions[i].உச்சி_2;
	    WriteRegion = cb->Regions[i];
	    எனில் (!WriteConsoleOutputW(g_hConOut,	// output handle
			cb->Buffer,		// our buffer
			cb->BufferSize,		// dimensions of our buffer
			BufferCoord,		// offset in our buffer
			&WriteRegion))		// region to restore
		திரும்பு FALSE;
	}
    }

    திரும்பு TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
நிலையான ConsoleBuffer g_cbOrig = { 0 };
# endif
நிலையான ConsoleBuffer g_cbNonTermcap = { 0 };
நிலையான ConsoleBuffer g_cbTermcap = { 0 };

எழுத்து g_szOrigTitle[256] = { 0 };
HWND g_hWnd = வெற்று;	// also used in os_mswin.c
நிலையான HICON g_hOrigIconSmall = வெற்று;
நிலையான HICON g_hOrigIcon = வெற்று;
நிலையான HICON g_hVimIcon = வெற்று;
நிலையான BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    நிலையான BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    எனில் (hWnd == வெற்று)
	திரும்பு FALSE;

    எனில் (phIconSmall != வெற்று)
	*phIconSmall = (HICON)SendMessage(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    எனில் (phIcon != வெற்று)
	*phIcon = (HICON)SendMessage(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    திரும்பு TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    நிலையான BOOL
SetConsoleIcon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    எனில் (hWnd == வெற்று)
	திரும்பு FALSE;

    எனில் (hIconSmall != வெற்று)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    எனில் (hIcon != வெற்று)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    திரும்பு TRUE;
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
    நிலையான வெறுமை
SaveConsoleTitleAndIcon(வெறுமை)
{
    // Save the original title.
    எனில் (!GetConsoleTitle(g_szOrigTitle, அளவு(g_szOrigTitle)))
	திரும்பு;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = GetConsoleWindow();
    எனில் (g_hWnd == வெற்று)
	திரும்பு;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    எனில் (g_hOrigIconSmall == வெற்று || g_hOrigIcon == வெற்று)
	திரும்பு;

    // Extract the first icon contained in the Vim executable.
    எனில் (
# ifdef FEAT_LIBCALL
	    mch_icon_load((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == வெற்று)
	g_hVimIcon = ExtractIcon(வெற்று, (LPCSTR)exe_name, 0);
    எனில் (g_hVimIcon != வெற்று)
	g_fCanChangeIcon = TRUE;
}

நிலையான முழு g_fWindInitCalled = FALSE;
நிலையான முழு g_fTermcapMode = FALSE;
நிலையான CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    நிலையான வெறுமை
mch_init_c(வெறுமை)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_BUFFER_INFO csbi;
# endif
# ifndef __MINGW32__
    வெளி முழு _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_இருமம்;		// we do our own CR-LF translation
    out_flush();

    // Obtain handles for the standard Console I/O devices
    எனில் (read_cmd_fd == 0)
	g_hConIn =  GetStdHandle(STD_உள்ளீடு_HANDLE);
    இல்லையெனில்
	create_conin();
    g_hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    SaveConsoleBuffer(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.wAttributes;
# இல்லையெனில்
    // Get current text attributes
    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
# endif
    எனில் (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (g_attrCurrent & 0xf) + 1;
    எனில் (cterm_normal_bg_color == 0)
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
    எனில் (g_fCanChangeIcon)
	SetConsoleIcon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    எனில் (use_alternate_screen_buffer)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    எனில் (fdDump)
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
    நிலையான வெறுமை
mch_exit_c(முழு r)
{
    // Copy flag since stoptermcap() will clear the flag.
    முழு fTermcapMode = g_fTermcapMode;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    எனில் (!fTermcapMode &&  use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    எனில் (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    எனில் (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	எனில் (g_fCanChangeIcon && !g_fForceExit)
	    SetConsoleIcon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	எனில் (fdDump)
	{
	    time_t t;

	    time(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = வெற்று;
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

    வெறுமை
mch_init(வெறுமை)
{
#ifdef VIMDLL
    எனில் (gui.starting)
	mch_init_g();
    இல்லையெனில்
	mch_init_c();
#அல்லதெனில் defined(FEAT_GUI_MSWIN)
    mch_init_g();
#இல்லையெனில்
    mch_init_c();
#endif
}

    வெறுமை
mch_exit(முழு r)
{
#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef VIMDLL
    எனில் (vimrun_path_allocated)
	vim_free(vimrun_path.string);

    எனில் (gui.in_use || gui.starting)
	mch_exit_g(r);
    இல்லையெனில்
	mch_exit_c(r);
#அல்லதெனில் defined(FEAT_GUI_MSWIN)
    எனில் (vimrun_path_allocated)
	vim_free(vimrun_path.string);
    mch_exit_g(r);
#இல்லையெனில்
    mch_exit_c(r);
#endif
}

/*
 * Do we have an interactive window?
 */
    முழு
mch_check_win(
    முழு argc UNUSED,
    எழுத்து **argv UNUSED)
{
    mch_get_exe_name();

#எனில் defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    திரும்பு OK;	    // GUI always has a tty
#இல்லையெனில்
# ifdef VIMDLL
    எனில் (gui.in_use)
	திரும்பு OK;
# endif
    எனில் (isatty(1))
	திரும்பு OK;
    திரும்பு FAIL;
#endif
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    வெறுமை
fname_case(
    char_u	*name,
    முழு		len)
{
    WCHAR   *p;
    WCHAR   buf[_MAX_PATH + 1];

    எனில் (*name == NUL)
	திரும்பு;

    slash_adjust(name);

    p = enc_to_utf16(name, வெற்று);
    எனில் (p == வெற்று)
	திரும்பு;

    எனில் (GetLongPathNameW(p, buf, _MAX_PATH))
    {
	char_u	*q = utf16_to_enc(buf, வெற்று);

	எனில் (q != வெற்று)
	{
	    எனில் (len > 0)
		vim_strncpy(name, q, len - 1);
	    இல்லையெனில்
	    {
		size_t  namelen = STRLEN(name);

		எனில் (namelen >= STRLEN(q))
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
    முழு
mch_get_user_name(
    char_u  *s,
    முழு	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_LENGTH(wszUserName);

    எனில் (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_to_enc(wszUserName, வெற்று);

	எனில் (p != வெற்று)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    திரும்பு OK;
	}
    }
    s[0] = NUL;
    திரும்பு FAIL;
}


/*
 * Insert host name in s[len].
 */
    வெறுமை
mch_get_host_name(
    char_u	*s,
    முழு		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_LENGTH(wszHostName);

    எனில் (!GetComputerNameW(wszHostName, &wcch))
	திரும்பு;

    char_u  *p = utf16_to_enc(wszHostName, வெற்று);
    எனில் (p == வெற்று)
	திரும்பு;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    long
mch_get_pid(வெறுமை)
{
    திரும்பு (long)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    முழு
mch_process_running(long pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   status = 0;
    முழு	    ret = FALSE;

    எனில் (hProcess == வெற்று)
	திரும்பு FALSE;  // might not have access
    எனில் (GetExitCodeProcess(hProcess, &status))
	ret = status == STILL_ACTIVE;
    CloseHandle(hProcess);
    திரும்பு ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    முழு
mch_dirname(
    char_u	*buf,
    முழு		len)
{
    WCHAR   wbuf[_MAX_PATH + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    எனில் (GetCurrentDirectoryW(_MAX_PATH, wbuf) == 0)
	திரும்பு FAIL;

    WCHAR   wcbuf[_MAX_PATH + 1];
    char_u  *p = வெற்று;

    எனில் (GetLongPathNameW(wbuf, wcbuf, _MAX_PATH) != 0)
    {
	p = utf16_to_enc(wcbuf, வெற்று);
	எனில் (p != வெற்று)
	{
	    எனில் (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    எனில் (p == வெற்று)
	p = utf16_to_enc(wbuf, வெற்று);

    எனில் (p == வெற்று)
	திரும்பு FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    திரும்பு OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    long
mch_getperm(char_u *name)
{
    stat_T	st;

    திரும்பு (mch_stat((எழுத்து *)name, &st) == 0)
	? (long)(unsigned short)st.st_mode
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    முழு
mch_setperm(char_u *name, long perm)
{
    long	n;
    WCHAR	*p;

    p = enc_to_utf16(name, வெற்று);
    எனில் (p == வெற்று)
	திரும்பு FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    எனில் (n == -1)
	திரும்பு FAIL;

    win32_set_archive(name);

    திரும்பு OK;
}

/*
 * Set hidden flag for "name".
 */
    வெறுமை
mch_hide(char_u *name)
{
    முழு attrs = win32_getattrs(name);
    எனில் (attrs == -1)
	திரும்பு;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    முழு
mch_ishidden(char_u *name)
{
    முழு f = win32_getattrs(name);

    எனில் (f == -1)
	திரும்பு FALSE;		    // file does not exist at all

    திரும்பு (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    முழு
mch_isdir(char_u *name)
{
    முழு f = win32_getattrs(name);

    எனில் (f == -1)
	திரும்பு FALSE;		    // file does not exist at all

    திரும்பு (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    முழு
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_to_utf16(name, வெற்று);
    எனில் (p == வெற்று)
	திரும்பு FALSE;
    attr = GetFileAttributesW(p);
    எனில் (attr == INVALID_FILE_ATTRIBUTES)
	error = GetLastError();
    vim_free(p);

    எனில் (attr == INVALID_FILE_ATTRIBUTES)
	திரும்பு error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    திரும்பு (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    முழு
mch_isrealdir(char_u *name)
{
    திரும்பு mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    முழு
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    முழு	    retval;

    p = enc_to_utf16(name, வெற்று);
    எனில் (p == வெற்று)
	திரும்பு -1;
    retval = _wmkdir(p);
    vim_free(p);
    திரும்பு retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    முழு
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    முழு	    retval;

    p = enc_to_utf16(name, வெற்று);
    எனில் (p == வெற்று)
	திரும்பு -1;
    retval = _wrmdir(p);
    vim_free(p);
    திரும்பு retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    முழு
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    திரும்பு win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_எண்_Of_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    முழு
mch_is_symbolic_link(char_u *name)
{
    HANDLE		hFind;
    முழு			res = FALSE;
    DWORD		fileFlags = 0, reparseTag = 0;
    WCHAR		*wn;
    WIN32_FIND_DATAW	findDataW;

    wn = enc_to_utf16(name, வெற்று);
    எனில் (wn == வெற்று)
	திரும்பு FALSE;

    hFind = FindFirstFileW(wn, &findDataW);
    vim_free(wn);
    எனில் (hFind != INVALID_HANDLE_மதிப்பு)
    {
	fileFlags = findDataW.dwFileAttributes;
	reparseTag = findDataW.dwReserved0;
	FindClose(hFind);
    }

    எனில் ((fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
	    && (reparseTag == IO_REPARSE_TAG_SYMLINK
		|| reparseTag == IO_REPARSE_TAG_MOUNT_POINT))
	res = TRUE;

    திரும்பு res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    முழு
mch_is_linked(char_u *fname)
{
    எனில் (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    முழு
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    முழு		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_to_utf16(fname, வெற்று);
    எனில் (wn == வெற்று)
	திரும்பு FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    வெற்று,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    வெற்று);		// handle to template file
    vim_free(wn);

    எனில் (hFile == INVALID_HANDLE_மதிப்பு)
	திரும்பு FILEINFO_READ_FAIL;

    எனில் (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    இல்லையெனில்
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    திரும்பு res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    நிலையான முழு
win32_getattrs(char_u *name)
{
    முழு		attr;
    WCHAR	*p;

    p = enc_to_utf16(name, வெற்று);
    எனில் (p == வெற்று)
	திரும்பு INVALID_FILE_ATTRIBUTES;

    attr = GetFileAttributesW(p);
    vim_free(p);

    திரும்பு attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    நிலையான முழு
win32_setattrs(char_u *name, முழு attrs)
{
    முழு	    res;
    WCHAR   *p;

    p = enc_to_utf16(name, வெற்று);
    எனில் (p == வெற்று)
	திரும்பு -1;

    res = SetFileAttributesW(p, attrs);
    vim_free(p);

    திரும்பு res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    நிலையான முழு
win32_set_archive(char_u *name)
{
    முழு attrs = win32_getattrs(name);
    எனில் (attrs == -1)
	திரும்பு -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    திரும்பு win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    முழு
mch_writable(char_u *name)
{
    முழு attrs = win32_getattrs(name);

    திரும்பு (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    முழு
mch_can_exe(char_u *name, char_u **path, முழு use_path UNUSED)
{
    திரும்பு executable_exists((எழுத்து *)name, STRLEN(name), path, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    முழு
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    முழு		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    எனில் (STRNCMP(name, "\\\\.\\", 4) == 0)
	திரும்பு முனை_WRITABLE;

    wn = enc_to_utf16(name, வெற்று);
    எனில் (wn == வெற்று)
	திரும்பு முனை_NORMAL;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    வெற்று,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    வெற்று);		    // handle to template file
    vim_free(wn);
    எனில் (hFile == INVALID_HANDLE_மதிப்பு)
	திரும்பு முனை_NORMAL;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    எனில் (type == FILE_TYPE_CHAR)
	திரும்பு முனை_WRITABLE;
    எனில் (type == FILE_TYPE_DISK)
	திரும்பு முனை_NORMAL;
    திரும்பு முனை_OTHER;
}

#ifdef HAVE_ACL
கட்டமைப்பு my_acl
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
    திரும்பு (vim_acl_T)வெற்று;
#இல்லையெனில்
    கட்டமைப்பு my_acl   *p = வெற்று;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(கட்டமைப்பு my_acl);
    எனில் (p != வெற்று)
    {
	WCHAR	*wn;

	wn = enc_to_utf16(fname, வெற்று);
	எனில் (wn == வெற்று)
	{
	    vim_free(p);
	    திரும்பு வெற்று;
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
	எனில் (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (வெறுமை)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    வெற்று,
		    வெற்று,
		    &p->pDacl,
		    வெற்று,
		    &p->pSecurityDescriptor);
	}
	எனில் (p->pSecurityDescriptor == வெற்று)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = வெற்று;
	}
	vim_free(wn);
    }

    திரும்பு (vim_acl_T)p;
#endif
}

#ifdef HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    நிலையான BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_SIZE_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_மொத்தம் = 0;
    GetAclInformation(acl, &acl_info, அளவு(acl_info), AclSizeInformation);
    க்கு (i = 0; i < acl_info.Ace_மொத்தம்; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	எனில் (ace->Header.AceFlags & INHERITED_ACE)
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}
#endif

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    வெறுமை
mch_set_acl(char_u *fname, vim_acl_T acl)
{
#ifdef HAVE_ACL
    கட்டமைப்பு my_acl   *p = (கட்டமைப்பு my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    எனில் (p == வெற்று)
	திரும்பு;

    wn = enc_to_utf16(fname, வெற்று);
    எனில் (wn == வெற்று)
	திரும்பு;

    // Set security flags
    எனில் (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    எனில் (p->pSidGroup)
	sec_info |= GROUP_SECURITY_INFORMATION;
    எனில் (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	எனில் (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    எனில் (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (வெறுமை)SetNamedSecurityInfoW(
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

    வெறுமை
mch_free_acl(vim_acl_T acl)
{
#ifdef HAVE_ACL
    கட்டமைப்பு my_acl   *p = (கட்டமைப்பு my_acl *)acl;

    எனில் (p != வெற்று)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#endif
}

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    நிலையான BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    உள்ளீடு_RECORD ir;
    DWORD out;

    தேர்வு (dwCtrlType)
    {
    நிலை CTRL_C_EVENT:
	எனில் (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	திரும்பு TRUE;

    நிலை CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = சாவி_EVENT;
	ir.Event.சாவி_Event_2.b_சாவி_Down = TRUE;
	ir.Event.சாவி_Event_2.w_Repeat_மொத்தம் = 1;
	ir.Event.சாவி_Event_2.w_Virtual_சாவி_Code = VK_CANCEL;
	ir.Event.சாவி_Event_2.wVirtualScanCode = 0;
	ir.Event.சாவி_Event_2.dw_Control_சாவி_State = 0;
	ir.Event.சாவி_Event_2.uChar.UnicodeChar = 0;
	Write_Console_உள்ளீடு(g_hConIn, &ir, 1, &out);
	திரும்பு TRUE;

    // fatal events: shut down gracefully
    நிலை CTRL_CLOSE_EVENT:
    நிலை CTRL_LOGOFF_EVENT:
    நிலை CTRL_SHUTDOWN_EVENT:
	windgoto((முழு)Rows - 1, cmdline_col_off);
	g_fForceExit = TRUE;

	vim_snprintf((எழுத்து *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	திரும்பு TRUE;		// not reached

    இயல்பு:
	திரும்பு FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    வெறுமை
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    எனில் (gui.in_use)
	திரும்பு;
# endif
    GetConsoleMode(g_hConIn, &cmodein);
    GetConsoleMode(g_hConOut, &cmodeout);
    எனில் (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_வரி_உள்ளீடு | ENABLE_PROCESSED_உள்ளீடு |
		     ENABLE_ECHO_உள்ளீடு);
	எனில் (g_fMouseActive)
	{
	    cmodein |= ENABLE_MOUSE_உள்ளீடு;
	    cmodein &= ~ENABLE_QUICK_EDIT_MODE;
	}
	இல்லையெனில்
	{
	    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# இல்லையெனில்
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    இல்லையெனில் // cooked
    {
	cmodein |= (ENABLE_வரி_உள்ளீடு | ENABLE_PROCESSED_உள்ளீடு |
		    ENABLE_ECHO_உள்ளீடு);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
    SetConsoleMode(g_hConOut, cmodeout);
    SetConsoleCtrlHandler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    எனில் (fdDump)
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
    முழு
mch_get_shellsize(வெறுமை)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

# ifdef VIMDLL
    எனில் (gui.in_use)
	திரும்பு OK;
# endif
    எனில் (!g_fTermcapMode && g_cbTermcap.IsValid)
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
    இல்லையெனில் எனில் (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	Rows = csbi.srWindow.Bottom - csbi.srWindow.உச்சி_2 + 1;
	Columns = csbi.srWindow.வலது_3 - csbi.srWindow.இடது_3 + 1;
    }
    இல்லையெனில்
    {
	Rows = 25;
	Columns = 80;
    }
    திரும்பு OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    நிலையான வெறுமை
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    எனில் (use_alternate_screen_buffer)
	திரும்பு;

    எனில் (!SetConsoleScreenBufferSize(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	எனில் (fdDump)
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
    நிலையான வெறுமை
ResizeWindow(
    HANDLE     hConsole,
    SMALL_RECT srWindowRect)
{
    எனில் (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
    {
# ifdef MCH_WRITE_DUMP
	எனில் (fdDump)
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
    நிலையான வெறுமை
ResizeConBufAndWindow(
    HANDLE  hConsole,
    முழு	    xSize,
    முழு	    ySize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    srWindowRect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    நிலையான முழு	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    எனில் (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", xSize, ySize);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    // define the new console window size and scroll position
    srWindowRect.இடது_3 = srWindowRect.உச்சி_2 = (SHORT) 0;
    srWindowRect.வலது_3 =  (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);

    எனில் (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	முழு sx, sy;

	sx = csbi.srWindow.வலது_3 - csbi.srWindow.இடது_3 + 1;
	sy = csbi.srWindow.Bottom - csbi.srWindow.உச்சி_2 + 1;
	எனில் (sy < ySize || sx < xSize)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    எனில் (sy < ySize)
		coordScreen.Y = ySize;
	    இல்லையெனில்
		coordScreen.Y = sy;
	    எனில் (sx < xSize)
		coordScreen.X = xSize;
	    இல்லையெனில்
		coordScreen.X = sx;
	    SetConsoleScreenBufferSize(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = xSize;
    coordScreen.Y = ySize;

    // In the new console call API, only the first time in reverse order
    எனில் (!vtp_working || resized)
    {
	ResizeWindow(hConsole, srWindowRect);
	ResizeConBuf(hConsole, coordScreen);
    }
    இல்லையெனில்
    {
	// Workaround for a Windows 10 bug
	cursor.X = srWindowRect.இடது_3;
	cursor.Y = srWindowRect.உச்சி_2;
	SetConsoleCursorPosition(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	ResizeWindow(hConsole, srWindowRect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    வெறுமை
mch_set_shellsize(வெறுமை)
{
    COORD coordScreen;

# ifdef VIMDLL
    எனில் (gui.in_use)
	திரும்பு;
# endif
    // Don't change window size while still starting up
    எனில் (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	திரும்பு;
    }

    எனில் (term_console)
    {
	coordScreen = GetLargestConsoleWindowSize(g_hConOut);

	// Clamp Rows and Columns to reasonable values
	எனில் (Rows > coordScreen.Y)
	    Rows = coordScreen.Y;
	எனில் (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	ResizeConBufAndWindow(g_hConOut, Columns, Rows);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    வெறுமை
mch_new_shellsize(வெறுமை)
{
# ifdef VIMDLL
    எனில் (gui.in_use)
	திரும்பு;
# endif
    set_scroll_region(0, 0, Columns - 1, Rows - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    வெறுமை
mch_set_winsize_now(வெறுமை)
{
    எனில் (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_set_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# எனில் defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    வெறுமை
mch_calc_cell_size(கட்டமைப்பு cellsize *cs_out)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_out->cs_xpixel = -1;
    cs_out->cs_ypixel = -1;

    எனில் (g_hConOut == INVALID_HANDLE_மதிப்பு)
	திரும்பு;

    cfi.cbSize = அளவு(cfi);
    எனில் (GetCurrentConsoleFontEx(g_hConOut, FALSE, &cfi)
	    && cfi.dwFontSize.X > 0 && cfi.dwFontSize.Y > 0)
    {
	cs_out->cs_xpixel = cfi.dwFontSize.X;
	cs_out->cs_ypixel = cfi.dwFontSize.Y;
	திரும்பு;
    }

    GetConsoleMode(g_hConIn, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_வரி_உள்ளீடு | ENABLE_ECHO_உள்ளீடு
	    | ENABLE_PROCESSED_உள்ளீடு);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_உள்ளீடு;

    எனில் (SetConsoleMode(g_hConIn, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	SetConsoleMode(g_hConIn, cmodeold);
    }
}
# endif

#endif

    நிலையான BOOL
vim_create_process(
    எழுத்து		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    எழுத்து		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = வெற்று;

    wcmd = enc_to_utf16((char_u *)cmd, வெற்று);
    எனில் (wcmd == வெற்று)
	திரும்பு FALSE;
    எனில் (cwd != வெற்று)
    {
	wcwd = enc_to_utf16((char_u *)cwd, வெற்று);
	எனில் (wcwd == வெற்று)
	    செல் theend;
    }

    ret = CreateProcessW(
	    வெற்று,		// Executable name
	    wcmd,		// Command to execute
	    வெற்று,		// Process security attributes
	    வெற்று,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    திரும்பு ret;
}


    நிலையான HINSTANCE
vim_shell_execute(
    எழுத்து *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_to_utf16((char_u *)cmd, வெற்று);
    எனில் (wcmd == வெற்று)
	திரும்பு (HINSTANCE) 0;

    ret = ShellExecuteW(வெற்று, வெற்று, wcmd, வெற்று, வெற்று, n_show_cmd);
    vim_free(wcmd);
    திரும்பு ret;
}


#எனில் defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    நிலையான முழு
mch_system_classic(எழுத்து *cmd, முழு options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = அளவு(si);
    si.lpReserved = வெற்று;
    si.lpDesktop = வெற்று;
    si.lpTitle = வெற்று;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    எனில் (options & SHELL_DOOUT)
	si.wShowWindow = SW_SHOWMINNOACTIVE;
    இல்லையெனில்
	si.wShowWindow = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = வெற்று;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_MODE |	CREATE_NEW_CONSOLE,
	    &si, &pi, வெற்று, வெற்று);

    // Wait for the command to terminate before continuing
    {
	முழு	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	க்கு (;;)
	{
	    செய்தி_2	செய்தி;

	    எனில் (PeekMessageW(&செய்தி, (HWND)வெற்று, 0, 0, PM_REMOVE))
	    {
		TranslateMessage(&செய்தி);
		DispatchMessageW(&செய்தி);
		delay = 1;
		தொடர்;
	    }
	    எனில் (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		நிறுத்து;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    எனில் (delay < 50)
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

    திரும்பு ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    நிலையான unsigned முழு __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_hChildStd_IN_Wr = param;
    linenr_T	    lnum = curbuf->b_op_start.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    முழு		    written = 0;

    க்கு (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	எனில் (l == 0)
	    len = 0;
	இல்லையெனில் எனில் (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_hChildStd_IN_Wr, "", 1, &len, வெற்று);
	}
	இல்லையெனில்
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_hChildStd_IN_Wr, (எழுத்து *)lp + written,
		      s == வெற்று ? l : (DWORD)(s - (lp + written)),
		      &len, வெற்று);
	}
	எனில் (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    எனில் (lnum != curbuf->b_op_end.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_வரி_மொத்தம்
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_hChildStd_IN_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, வெற்று);
	    }

	    ++lnum;
	    எனில் (lnum > curbuf->b_op_end.lnum)
		நிறுத்து;

	    lp = ml_get(lnum);
	    written = 0;
	}
	இல்லையெனில் எனில் (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_hChildStd_IN_Wr);
    திரும்பு 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    நிலையான வெறுமை
dump_pipe(முழு	    options,
	  HANDLE    g_hChildStd_OUT_Rd,
	  garray_T  *ga,
	  char_u    buffer[],
	  DWORD	    *buffer_off)
{
    DWORD	availableBytes = 0;
    DWORD	i;
    முழு		ret;
    DWORD	len;
    DWORD	toRead;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_hChildStd_OUT_Rd, // pipe to query
			வெற்று,		    // optional buffer
			0,		    // buffer size
			வெற்று,		    // number of read bytes
			&availableBytes,    // available bytes total
			வெற்று);		    // byteLeft

    // We got real data in the pipe, read it
    போது (ret != 0 && availableBytes > 0)
    {
	toRead = (DWORD)(BUFLEN - *buffer_off);
	toRead = availableBytes < toRead ? availableBytes : toRead;
	ReadFile(g_hChildStd_OUT_Rd, buffer + *buffer_off, toRead , &len, வெற்று);

	// If we haven't read anything, there is a problem
	எனில் (len == 0)
	    நிறுத்து;

	availableBytes -= len;

	எனில் (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    க்கு (i = 0; i < len; ++i)
	    {
		எனில் (buffer[i] == NL)
		    append_ga_வரி(ga);
		இல்லையெனில் எனில் (buffer[i] == NUL)
		    ga_append(ga, NL);
		இல்லையெனில்
		    ga_append(ga, buffer[i]);
	    }
	}
	இல்லையெனில் எனில் (has_mbyte)
	{
	    முழு		l;
	    முழு		c;
	    char_u	*p;

	    len += *buffer_off;
	    buffer[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    க்கு (p = buffer; p < buffer + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		எனில் (l == 0)
		    l = 1;  // NUL byte?
		இல்லையெனில் எனில் (MB_BYTE2LEN(*p) != l)
		    நிறுத்து;
	    }
	    எனில் (p == buffer)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		எனில் (len >= 12)
		    ++p;
		இல்லையெனில்
		{
		    *buffer_off = len;
		    திரும்பு;
		}
	    }
	    c = *p;
	    *p = NUL;
	    செய்தி_puts((எழுத்து *)buffer);
	    எனில் (p < buffer + len)
	    {
		*p = c;
		*buffer_off = (DWORD)((buffer + len) - p);
		mch_memmove(buffer, p, *buffer_off);
		திரும்பு;
	    }
	    *buffer_off = 0;
	}
	இல்லையெனில்
	{
	    buffer[len] = NUL;
	    செய்தி_puts((எழுத்து *)buffer);
	}

	windgoto(செய்தி_row, cmdline_col_off + செய்தி_col);
	cursor_on();
	out_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    நிலையான முழு
mch_system_piped(எழுத்து *cmd, முழு options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_hChildStd_IN_Rd = வெற்று;
    HANDLE g_hChildStd_IN_Wr = வெற்று;
    HANDLE g_hChildStd_OUT_Rd = வெற்று;
    HANDLE g_hChildStd_OUT_Wr = வெற்று;

    char_u	buffer[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    முழு		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    முழு		noread_cnt = 0;
    garray_T	ga;
    முழு		delay = 1;
    DWORD	buffer_off = 0;	// valid bytes in buffer[]
    எழுத்து	*p = வெற்று;

    SECURITY_ATTRIBUTES saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = அளவு(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = வெற்று;

    எனில் ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)
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
	செய்தி_puts(_("\nCannot create pipes\n"));
    }

    si.cb = அளவு(si);
    si.lpReserved = வெற்று;
    si.lpDesktop = வெற்று;
    si.lpTitle = வெற்று;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_hChildStd_OUT_Wr;
    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.h_Std_உள்ளீடு = g_hChildStd_IN_Rd;
    si.wShowWindow = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = வெற்று;

    எனில் (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    எனில் (cmd != வெற்று)
    {
	p = (எழுத்து *)vim_strsave((char_u *)cmd);
	எனில் (p != வெற்று)
	    unescape_shellxquote((char_u *)p, p_sxe);
	இல்லையெனில்
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_MODE,
						&si, &pi, வெற்று, வெற்று);

    எனில் (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(g_hChildStd_OUT_Wr);

    எனில் (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(வெற்று,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_hChildStd_IN_Wr,  // parameter
			0,		 // creation flag, start immediately
			வெற்று);		    // we don't care about thread id
	CloseHandle(thread);
	g_hChildStd_IN_Wr = வெற்று;
    }

    // Keep updating the window while waiting for the shell to finish.
    க்கு (;;)
    {
	செய்தி_2	செய்தி;

	எனில் (PeekMessageW(&செய்தி, (HWND)வெற்று, 0, 0, PM_REMOVE))
	{
	    TranslateMessage(&செய்தி);
	    DispatchMessageW(&செய்தி);
	}

	// write pipe information in the window
	எனில் ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.in_use
# endif
	    )
	{
	    len = 0;
	    எனில் (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		எனில் (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		எனில் (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    எனில் (len == 1 && cmd != வெற்று)
		    {
			எனில் (ta_buf[ta_len] == Ctrl_C)
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
		    எனில் ((len == 1 || len == 4 ) && cmd != வெற்று)
		    {
			எனில் (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_hChildStd_IN_Wr);
			    g_hChildStd_IN_Wr = வெற்று;
			    len = 0;
			}
		    }

		    len = term_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    க்கு (i = ta_len; i < ta_len + len; ++i)
		    {
			எனில் (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    செய்தி_putchar(ta_buf[i]);
			இல்லையெனில் எனில் (has_mbyte)
			{
			    முழு l = (*mb_ptr2len)(ta_buf + i);

			    செய்தி_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			இல்லையெனில்
			    செய்தி_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(செய்தி_row, cmdline_col_off + செய்தி_col);
		    out_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    எனில் (options & SHELL_WRITE)
			ta_len = 0;
		    இல்லையெனில் எனில் (g_hChildStd_IN_Wr != வெற்று)
		    {
			WriteFile(g_hChildStd_IN_Wr, (எழுத்து*)ta_buf,
				    1, &len, வெற்று);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			எனில் (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	எனில் (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	எனில் (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);
	    நிறுத்து;
	}

	++noread_cnt;
	dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	எனில் (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_hChildStd_OUT_Rd);
    எனில் (g_hChildStd_IN_Wr != வெற்று)
	CloseHandle(g_hChildStd_IN_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    GetExitCodeProcess(pi.hProcess, &ret);

    எனில் (options & SHELL_READ)
    {
	எனில் (ga.ga_len > 0)
	{
	    append_ga_வரி(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	இல்லையெனில்
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    திரும்பு ret;
}

    நிலையான முழு
mch_system_g(எழுத்து *cmd, முழு options)
{
    // if we can pipe and the shelltemp option is off
    எனில் (!p_stmp)
	திரும்பு mch_system_piped(cmd, options);
    இல்லையெனில்
	திரும்பு mch_system_classic(cmd, options);
}
#endif

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    நிலையான முழு
mch_system_c(எழுத்து *cmd, முழு options UNUSED)
{
    முழு		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    எனில் (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	எனில் (buf == வெற்று)
	    திரும்பு -1;
	vim_snprintf((எழுத்து *)buf, len, "(%s)", cmd);
	wcmd = enc_to_utf16(buf, வெற்று);
	விடுவி(buf);
    }
    இல்லையெனில்
	wcmd = enc_to_utf16((char_u *)cmd, வெற்று);

    எனில் (wcmd == வெற்று)
	திரும்பு -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    திரும்பு ret;
}

#endif

    நிலையான முழு
mch_system(எழுத்து *cmd, முழு options)
{
#ifdef VIMDLL
    எனில் (gui.in_use || gui.starting)
	திரும்பு mch_system_g(cmd, options);
    இல்லையெனில்
	திரும்பு mch_system_c(cmd, options);
#அல்லதெனில் defined(FEAT_GUI_MSWIN)
    திரும்பு mch_system_g(cmd, options);
#இல்லையெனில்
    திரும்பு mch_system_c(cmd, options);
#endif
}

#எனில் defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    நிலையான முழு
mch_call_shell_terminal(
    char_u	*cmd,
    முழு		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = வெற்று;
    typval_T	argvar[2];
    long_u	cmdlen;
    முழு		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    எனில் (cmd == வெற்று)
	cmdlen = STRLEN(p_sh) + 1;
    இல்லையெனில்
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    எனில் (newcmd == வெற்று)
	திரும்பு 255;
    எனில் (cmd == வெற்று)
    {
	STRCPY(newcmd, p_sh);
	ch_log(வெற்று, "starting terminal to run a shell");
    }
    இல்லையெனில்
    {
	vim_snprintf((எழுத்து *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(வெற்று, "starting terminal for system command '%s'", cmd);
    }

    init_job_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = term_start(argvar, வெற்று, &opt, TERM_START_SYSTEM);
    எனில் (buf == வெற்று)
    {
	vim_free(newcmd);
	திரும்பு 255;
    }

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    எனில் (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	போது (term_use_loop())
	{
	    எனில் (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		எனில் (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    இல்லையெனில்
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(வெற்று, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    திரும்பு retval;
}
#endif
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    நிலையான வெறுமை
restore_env_var(char_u *name, char_u *old_மதிப்பு, முழு must_free)
{
    எனில் (old_மதிப்பு != வெற்று)
    {
	vim_setenv(name, old_மதிப்பு);
	எனில் (must_free)
	    vim_free(old_மதிப்பு);
	திரும்பு;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    முழு
mch_call_shell(
    char_u  *cmd,
    முழு	    options)	// SHELL_*, see vim.h
{
    முழு		x = 0;
    முழு		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    முழு		must_free;
    char_u	*oldval;

#ifdef FEAT_EVAL
    ch_log(வெற்று, "executing shell command: %s", cmd);
#endif
    // Change the title to reflect that we are in a subshell.
    எனில் (GetConsoleTitleW(szShellTitle, ARRAY_LENGTH(szShellTitle) - 4) > 0)
    {
	எனில் (cmd == வெற்று)
	    wcscat(szShellTitle, L" :sh");
	இல்லையெனில்
	{
	    WCHAR *wn = enc_to_utf16((char_u *)cmd, வெற்று);

	    எனில் (wn != வெற்று)
	    {
		wcscat(szShellTitle, L" - !");
		எனில் ((wcslen(szShellTitle) + wcslen(wn) <
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
    எனில் (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#endif
#எனில் defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    எனில் (
# ifdef VIMDLL
	    gui.in_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != வெற்று
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	எனில் (cmdbase != வெற்று)
	    // Skip a leading quote and (.
	    போது (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	எனில் (cmdbase == வெற்று || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    திரும்பு x;
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

    எனில் (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    எனில் (cmd == வெற்று)
    {
	x = mch_system((எழுத்து *)p_sh, options);
    }
    இல்லையெனில்
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = வெற்று;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	எனில் (*cmdbase == '"' )
	    ++cmdbase;
	எனில் (*cmdbase == '(')
	    ++cmdbase;

	எனில் ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    ZeroMemory(&si, அளவு(si));
	    si.cb = அளவு(si);
	    si.lpReserved = வெற்று;
	    si.lpDesktop = வெற்று;
	    si.lpTitle = வெற்று;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = வெற்று;

	    cmdbase = skipwhite(cmdbase + 5);
	    எனில் ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    இல்லையெனில் எனில் ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_WINDOW;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_உள்ளீடு = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    வெற்று,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_NORMAL,		// File att.
		    வெற்று);				// Temp file
		si.hStdOutput = si.h_Std_உள்ளீடு;
		si.hStdError = si.h_Std_உள்ளீடு;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    எனில் (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		எனில் (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		எனில் (p > cmdbase && p[-1] == ')'
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
	    எனில் (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		எனில் (cmd_shell == வெற்று || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		எனில் (subcmd != வெற்று)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    எனில் (newcmd != வெற்று)
			vim_snprintf((எழுத்து *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    இல்லையெனில்
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    எனில் (vim_create_process((எழுத்து *)newcmd, FALSE, flags,
			&si, &pi, வெற்று, வெற்று))
		x = 0;
	    இல்லையெனில் எனில் (vim_shell_execute((எழுத்து *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    இல்லையெனில்
	    {
		x = -1;
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
		எனில் (gui.in_use)
# endif
		    emsg(_(e_command_not_found));
#endif
	    }

	    எனில் (newcmd != cmdbase)
		vim_free(newcmd);

	    எனில் (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_உள்ளீடு != வெற்று)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_உள்ளீடு);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	இல்லையெனில்
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
	    எனில் (newcmd != வெற்று)
	    {
#எனில் defined(FEAT_GUI_MSWIN)
		எனில் (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    எழுத்து *செய்தி = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    எழுத்து *title = _("Vim Warning");
		    WCHAR *wmsg = enc_to_utf16((char_u *)செய்தி, வெற்று);
		    WCHAR *wtitle = enc_to_utf16((char_u *)title, வெற்று);

		    எனில் (wmsg != வெற்று && wtitle != வெற்று)
			MessageBoxW(வெற்று, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		எனில் (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((எழுத்து *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_path.string,
			    (செய்தி_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		இல்லையெனில் எனில் (
# ifdef VIMDLL
			(gui.in_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((எழுத்து *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		இல்லையெனில்
#endif
		    vim_snprintf((எழுத்து *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((எழுத்து *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    எனில் (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    எனில் (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#எனில் defined(FEAT_GUI_MSWIN)
	    && ((gui.in_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#endif
	    )
    {
	smsg(_("shell returned %d"), x);
	செய்தி_putchar('\n');
    }
    resettitle();

    mch_signal(SIGINT, SIG_DFL);
    mch_signal(SIGBREAK, SIG_DFL);
    mch_signal(SIGILL, SIG_DFL);
    mch_signal(SIGFPE, SIG_DFL);
    mch_signal(SIGSEGV, SIG_DFL);
    mch_signal(SIGTERM, SIG_DFL);
    mch_signal(SIGABRT, SIG_DFL);

    திரும்பு x;
}

#எனில் defined(FEAT_JOB_CHANNEL)
    நிலையான HANDLE
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

    wn = enc_to_utf16(fname, வெற்று);
    எனில் (wn == வெற்று)
	திரும்பு INVALID_HANDLE_மதிப்பு;

    h = CreateFileW(wn, dwDesiredAccess, dwShareMode,
	    lpSecurityAttributes, dwCreationDisposition,
	    dwFlagsAndAttributes, வெற்று);
    vim_free(wn);
    திரும்பு h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    வெறுமை
win32_build_env(dict_T *env, garray_T *gap, முழு is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != வெற்று ? env->dv_hashtab.ht_used : 0;
    LPVOID	base = GetEnvironmentStringsW();

    // for last \0
    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு;

    எனில் (env != வெற்று)
    {
	FOR_ALL_HASHTAB_பொருட்கள்(&env->dv_hashtab, hi, todo)
	{
	    எனில் (!HASHITEM_EMPTY(hi))
	    {
		typval_T *item = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_to_utf16((char_u *)hi->hi_சாவி, வெற்று);
		WCHAR   *wval = enc_to_utf16(tv_get_string(item), வெற்று);
		--todo;
		எனில் (wkey != வெற்று && wval != வெற்று)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    எனில் (ga_grow(gap, (முழு)(lkey + lval + 2)) == FAIL)
			தொடர்;
		    க்கு (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'=';
		    க்கு (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    எனில் (base)
    {
	WCHAR	*p = (WCHAR*) base;

	// for last \0
	எனில் (ga_grow(gap, 1) == FAIL)
	    திரும்பு;

	போது (*p != 0 || *(p + 1) != 0)
	{
	    எனில் (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_data + gap->ga_len++) = *p;
	    p++;
	}
	FreeEnvironmentStringsW(base);
	*((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
    }

# எனில் defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
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

	எனில் (ga_grow(gap, (முழு)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    க்கு (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    க்கு (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    எனில் (is_terminal)
	    {
		க்கு (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_data + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		க்கு (n = 0; n < version_len; n++)
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
    நிலையான BOOL
create_pipe_pair(HANDLE handles[2])
{
    நிலையான LONG		s;
    எழுத்து		name[64];
    SECURITY_ATTRIBUTES sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_SIZE, 0, 0, வெற்று);

    எனில் (handles[1] == INVALID_HANDLE_மதிப்பு)
	திரும்பு FALSE;

    sa.nLength = அளவு(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = வெற்று;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    எனில் (handles[0] == INVALID_HANDLE_மதிப்பு)
    {
	CloseHandle(handles[1]);
	திரும்பு FALSE;
    }

    திரும்பு TRUE;
}

#endif // FEAT_JOB_CHANNEL

#எனில் defined(FEAT_EVAL)
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
    எழுத்து	**argv,
    char_u	*infile,
    முழு		flags UNUSED,
    முழு		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;
    HANDLE		hChildStdoutRd = INVALID_HANDLE_மதிப்பு;
    HANDLE		hChildStdoutWr = INVALID_HANDLE_மதிப்பு;
    HANDLE		hChildStdinRd = INVALID_HANDLE_மதிப்பு;
    garray_T		cmd_ga;
    garray_T		out_ga;
    char_u		*buffer = வெற்று;
    DWORD		exit_code = (DWORD)-1;
    முழு			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    க்கு (i = 0; argv[i] != வெற்று; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	முழு	has_spaces = FALSE;
	முழு	j;

	க்கு (j = 0; s[j] != NUL; j++)
	    எனில் (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		நிறுத்து;
	    }

	எனில் (i > 0)
	    ga_append(&cmd_ga, ' ');

	எனில் (has_spaces)
	{
	    முழு	இலக்கம்_bs;

	    ga_append(&cmd_ga, '"');
	    க்கு (j = 0; arg[j] != NUL; j++)
	    {
		இலக்கம்_bs = 0;
		போது (arg[j] == '\\')
		{
		    இலக்கம்_bs++;
		    j++;
		}

		எனில் (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    போது (இலக்கம்_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    நிறுத்து;
		}
		இல்லையெனில் எனில் (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    போது (இலக்கம்_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		இல்லையெனில்
		{
		    போது (இலக்கம்_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	இல்லையெனில்
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&out_ga, 1, 4096);

    saAttr.nLength = அளவு(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = வெற்று;

    // Create a pipe for the child's stdout.
    எனில் (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)
	    || !SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	செல் done;
    }

    // Set up stdin from infile if provided.
    எனில் (infile != வெற்று)
    {
	WCHAR *winfile = enc_to_utf16(infile, வெற்று);

	எனில் (winfile != வெற்று)
	{
	    hChildStdinRd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL, வெற்று);
	    vim_free(winfile);
	}
    }

    ZeroMemory(&pi, அளவு(pi));
    ZeroMemory(&si, அளவு(si));
    si.cb = அளவு(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;
    si.h_Std_உள்ளீடு = (hChildStdinRd != INVALID_HANDLE_மதிப்பு)
		    ? hChildStdinRd : INVALID_HANDLE_மதிப்பு;

    ch_log(வெற்று, "directly executing: %s", (எழுத்து *)cmd_ga.ga_data);

    // Create the child process directly, without going through the shell.
    எனில் (!vim_create_process((எழுத்து *)cmd_ga.ga_data, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
		&si, &pi, வெற்று, வெற்று))
    {
	semsg(_(e_invalid_argument_str), cmd_ga.ga_data);
	செல் done;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(hChildStdoutWr);
    hChildStdoutWr = INVALID_HANDLE_மதிப்பு;
    எனில் (hChildStdinRd != INVALID_HANDLE_மதிப்பு)
    {
	CloseHandle(hChildStdinRd);
	hChildStdinRd = INVALID_HANDLE_மதிப்பு;
    }

    // Read output from child process.
    க்கு (;;)
    {
	எழுத்து	buf[4096];
	DWORD	n;

	எனில் (!ReadFile(hChildStdoutRd, buf, அளவு(buf), &n, வெற்று) || n == 0)
	    நிறுத்து;
	எனில் (ga_grow(&out_ga, (முழு)n) == OK)
	{
	    mch_memmove((எழுத்து *)out_ga.ga_data + out_ga.ga_len, buf, n);
	    out_ga.ga_len += (முழு)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    set_vim_var_nr(VV_SHELL_ERROR, (long)exit_code);

    எனில் (out_ga.ga_len > 0)
    {
	buffer = alloc(out_ga.ga_len + 1);
	எனில் (buffer != வெற்று)
	{
	    mch_memmove(buffer, out_ga.ga_data, out_ga.ga_len);
	    எனில் (ret_len == வெற்று)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		க்கு (i = 0; i < out_ga.ga_len; ++i)
		    எனில் (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[out_ga.ga_len] = NUL;
	    }
	    இல்லையெனில்
		*ret_len = out_ga.ga_len;
	}
    }
    இல்லையெனில் எனில் (ret_len != வெற்று)
	*ret_len = 0;

done:
    ga_clear(&cmd_ga);
    ga_clear(&out_ga);
    எனில் (hChildStdoutRd != INVALID_HANDLE_மதிப்பு)
	CloseHandle(hChildStdoutRd);
    எனில் (hChildStdoutWr != INVALID_HANDLE_மதிப்பு)
	CloseHandle(hChildStdoutWr);
    எனில் (hChildStdinRd != INVALID_HANDLE_மதிப்பு)
	CloseHandle(hChildStdinRd);
    திரும்பு buffer;
}
#endif // FEAT_EVAL

#எனில் defined(FEAT_JOB_CHANNEL)
    வெறுமை
mch_job_start(எழுத்து *cmd, job_T *job, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_ATTRIBUTES saAttr;
    channel_T		*channel = வெற்று;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    முழு		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    முழு		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    முழு		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    முழு		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    முழு		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    முழு		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    முழு		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;

    எனில் (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_மதிப்பு;
    ifd[1] = INVALID_HANDLE_மதிப்பு;
    ofd[0] = INVALID_HANDLE_மதிப்பு;
    ofd[1] = INVALID_HANDLE_மதிப்பு;
    efd[0] = INVALID_HANDLE_மதிப்பு;
    efd[1] = INVALID_HANDLE_மதிப்பு;
    ga_init2(&ga, அளவு(wchar_t), 500);

    jo = CreateJobObject(வெற்று, வெற்று);
    எனில் (jo == வெற்று)
    {
	job->jv_status = JOB_FAILED;
	செல் failed;
    }

    எனில் (options->jo_env != வெற்று)
	win32_build_env(options->jo_env, &ga, FALSE);

    ZeroMemory(&pi, அளவு(pi));
    ZeroMemory(&si, அளவு(si));
    si.cb = அளவு(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    saAttr.nLength = அளவு(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = வெற்று;

    எனில் (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	ifd[0] = job_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
	எனில் (ifd[0] == INVALID_HANDLE_மதிப்பு)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    செல் failed;
	}
    }
    இல்லையெனில் எனில் (!use_null_for_in
	    && (!create_pipe_pair(ifd)
		|| !SetHandleInformation(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	செல் failed;

    எனில் (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	ofd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	எனில் (ofd[1] == INVALID_HANDLE_மதிப்பு)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    செல் failed;
	}
    }
    இல்லையெனில் எனில் (!use_null_for_out &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !SetHandleInformation(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	செல் failed;

    எனில் (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	எனில் (efd[1] == INVALID_HANDLE_மதிப்பு)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    செல் failed;
	}
    }
    இல்லையெனில் எனில் (!use_out_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !SetHandleInformation(efd[0], HANDLE_FLAG_INHERIT, 0)))
	செல் failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_உள்ளீடு = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_out_for_err ? ofd[1] : efd[1];

    எனில் (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	எனில் (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    எனில் (channel != வெற்று)
		++channel->ch_refcount;
	}
	இல்லையெனில்
	    channel = add_channel();
	எனில் (channel == வெற்று)
	    செல் failed;
    }

    எனில் (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_MODE |
	    CREATE_NEW_PROCESS_GROUP |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_data,
	    (எழுத்து *)options->jo_cwd))
    {
	CloseHandle(jo);
	job->jv_status = JOB_FAILED;
	செல் failed;
    }

    ga_clear(&ga);

    எனில் (!AssignProcessToJobObject(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = வெற்று;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    job->jv_proc_info = pi;
    job->jv_job_object = jo;
    job->jv_status = JOB_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    எனில் (!use_out_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    job->jv_channel = channel;
    எனில் (channel != வெற்று)
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
    திரும்பு;

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

    எழுத்து *
mch_job_status(job_T *job)
{
    DWORD dwExitCode = 0;

    எனில் (!GetExitCodeProcess(job->jv_proc_info.hProcess, &dwExitCode)
	    || dwExitCode != STILL_ACTIVE)
    {
	job->jv_exitval = (முழு)dwExitCode;
	எனில் (job->jv_status < JOB_ENDED)
	{
	    ch_log(job->jv_channel, "Job ended");
	    job->jv_status = JOB_ENDED;
	}
	திரும்பு "dead";
    }
    திரும்பு "run";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    HANDLE jobHandles[MAXIMUM_WAIT_OBJECTS];
    job_T *jobArray[MAXIMUM_WAIT_OBJECTS];
    job_T *job = job_list;

    போது (job != வெற்று)
    {
	DWORD n;
	DWORD result;

	க்கு (n = 0; n < MAXIMUM_WAIT_OBJECTS
				       && job != வெற்று; job = job->jv_next)
	{
	    எனில் (job->jv_status == JOB_STARTED)
	    {
		jobHandles[n] = job->jv_proc_info.hProcess;
		jobArray[n] = job;
		++n;
	    }
	}
	எனில் (n == 0)
	    தொடர்;
	result = WaitForMultipleObjects(n, jobHandles, FALSE, 0);
	எனில் (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + n)
	{
	    job_T *wait_job = jobArray[result - WAIT_OBJECT_0];

	    எனில் (STRCMP(mch_job_status(wait_job), "dead") == 0)
		திரும்பு wait_job;
	}
    }
    திரும்பு வெற்று;
}

    நிலையான BOOL
terminate_all(HANDLE process, முழு code)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_மதிப்பு;
    DWORD	    pid = GetProcessId(process);

    எனில் (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	எனில் (h != INVALID_HANDLE_மதிப்பு)
	{
	    pe.dwSize = அளவு(PROCESSENTRY32);
	    எனில் (!Process32First(h, &pe))
		செல் theend;

	    செய்
	    {
		எனில் (pe.th32ParentProcessID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    எனில் (ph != வெற்று)
		    {
			terminate_all(ph, code);
			CloseHandle(ph);
		    }
		}
	    } போது (Process32Next(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    திரும்பு TerminateProcess(process, code);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    முழு
mch_signal_job(job_T *job, char_u *how)
{
    முழு ret;

    எனில் (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	எனில் (job->jv_job_object != வெற்று)
	{
	    எனில் (job->jv_channel != வெற்று && job->jv_channel->ch_anonymous_pipe)
		job->jv_channel->ch_killing = TRUE;
	    திரும்பு TerminateJobObject(job->jv_job_object, (UINT)-1) ? OK : FAIL;
	}
	திரும்பு terminate_all(job->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    எனில் (!AttachConsole(job->jv_proc_info.dwProcessId))
	திரும்பு FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		job->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    திரும்பு ret;
}

/*
 * Clear the data related to "job".
 */
    வெறுமை
mch_clear_job(job_T *job)
{
    எனில் (job->jv_status == JOB_FAILED)
	திரும்பு;

    எனில் (job->jv_job_object != வெற்று)
	CloseHandle(job->jv_job_object);
    CloseHandle(job->jv_proc_info.hProcess);
}
#endif


#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    நிலையான வெறுமை
termcap_mode_start(வெறுமை)
{
    DWORD cmodein;

    எனில் (g_fTermcapMode)
	திரும்பு;

    SaveConsoleBuffer(&g_cbNonTermcap);

    எனில் (g_cbTermcap.IsValid)
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
    இல்லையெனில்
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
    எனில் (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_உள்ளீடு;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    இல்லையெனில்
    {
	cmodein &= ~ENABLE_MOUSE_உள்ளீடு;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }
    cmodein |= ENABLE_WINDOW_உள்ளீடு;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_fTermcapMode = TRUE;
}


/*
 * End termcap mode
 */
    நிலையான வெறுமை
termcap_mode_end(வெறுமை)
{
    DWORD cmodein;
    ConsoleBuffer *cb;
    COORD coord;
    DWORD dwDummy;

    எனில் (!g_fTermcapMode)
	திரும்பு;

    SaveConsoleBuffer(&g_cbTermcap);

    GetConsoleMode(g_hConIn, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_உள்ளீடு | ENABLE_WINDOW_உள்ளீடு);
    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# இல்லையெனில்
    cb = &g_cbNonTermcap;
# endif
    RestoreConsoleBuffer(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    எனில் (exiting && use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    எனில் (!USE_WT && (p_rs || exiting))
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
	எனில் (exiting && !p_rs)
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


#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    நிலையான வெறுமை
clear_chars(
    COORD coord,
    DWORD n)
{
    எனில் (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_hConOut, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    இல்லையெனில்
    {
	set_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    நிலையான வெறுமை
clear_screen(வெறுமை)
{
    g_coord.X = g_coord.Y = 0;

    எனில் (!vtp_working)
	clear_chars(g_coord, Rows * Columns);
    இல்லையெனில்
    {
	set_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    நிலையான வெறுமை
clear_to_end_of_display(வெறுமை)
{
    COORD save = g_coord;

    எனில் (!vtp_working)
	clear_chars(g_coord, (Rows - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    இல்லையெனில்
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
    நிலையான வெறுமை
clear_to_end_of_வரி(வெறுமை)
{
    COORD save = g_coord;

    எனில் (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    இல்லையெனில்
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
    நிலையான வெறுமை
scroll(unsigned c_வரிகள்)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.இடது_3 + 1, g_srScrollRegion.உச்சி_2 + 1);
    delete_வரிகள்(c_வரிகள்);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    நிலையான வெறுமை
set_scroll_region(
    unsigned இடது,
    unsigned உச்சி,
    unsigned வலது,
    unsigned bottom)
{
    எனில் (இடது >= வலது
	    || உச்சி >= bottom
	    || வலது > (unsigned) Columns - 1
	    || bottom > (unsigned) Rows - 1)
	திரும்பு;

    g_srScrollRegion.இடது_3 =   இடது;
    g_srScrollRegion.உச்சி_2 =    உச்சி;
    g_srScrollRegion.வலது_3 =  வலது;
    g_srScrollRegion.Bottom = bottom;
}

    நிலையான வெறுமை
set_scroll_region_tb(
    unsigned உச்சி,
    unsigned bottom)
{
    எனில் (உச்சி >= bottom || bottom > (unsigned)Rows - 1)
	திரும்பு;

    g_srScrollRegion.உச்சி_2 = உச்சி;
    g_srScrollRegion.Bottom = bottom;
}

    நிலையான வெறுமை
set_scroll_region_lr(
    unsigned இடது,
    unsigned வலது)
{
    எனில் (இடது >= வலது || வலது > (unsigned)Columns - 1)
	திரும்பு;

    g_srScrollRegion.இடது_3 = இடது;
    g_srScrollRegion.வலது_3 = வலது;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    நிலையான வெறுமை
செருகு_வரிகள்(unsigned c_வரிகள்)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.இடது_3 + 1, g_srScrollRegion.உச்சி_2 + 1);

    dest.X = g_srScrollRegion.இடது_3;
    dest.Y = g_coord.Y + c_வரிகள்;

    source.இடது_3   = g_srScrollRegion.இடது_3;
    source.உச்சி_2	  = g_coord.Y;
    source.வலது_3  = g_srScrollRegion.வலது_3;
    source.Bottom = g_srScrollRegion.Bottom - c_வரிகள்;

    clip.இடது_3   = g_srScrollRegion.இடது_3;
    clip.உச்சி_2    = g_coord.Y;
    clip.வலது_3  = g_srScrollRegion.வலது_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    எனில் (!USE_VTP)
	fill.Attributes = g_attrCurrent;
    இல்லையெனில்
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    எனில் (source.Bottom < dest.Y)
    {
	COORD coord;
	முழு   i;

	coord.X = source.இடது_3;
	க்கு (i = clip.உச்சி_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.வலது_3 - source.இடது_3 + 1);
	}
    }

    எனில் (vtp_working)
    {
	COORD coord;
	முழு i;

	coord.X = source.இடது_3;
	க்கு (i = source.உச்சி_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.வலது_3 - source.இடது_3 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    நிலையான வெறுமை
delete_வரிகள்(unsigned c_வரிகள்)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    முழு		    nb;

    gotoxy(g_srScrollRegion.இடது_3 + 1, g_srScrollRegion.உச்சி_2 + 1);

    dest.X = g_srScrollRegion.இடது_3;
    dest.Y = g_coord.Y;

    source.இடது_3   = g_srScrollRegion.இடது_3;
    source.உச்சி_2	  = g_coord.Y + c_வரிகள்;
    source.வலது_3  = g_srScrollRegion.வலது_3;
    source.Bottom = g_srScrollRegion.Bottom;

    clip.இடது_3   = g_srScrollRegion.இடது_3;
    clip.உச்சி_2    = g_coord.Y;
    clip.வலது_3  = g_srScrollRegion.வலது_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    எனில் (!vtp_working)
	fill.Attributes = g_attrCurrent;
    இல்லையெனில்
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (source.Bottom - source.உச்சி_2) + 1;

    எனில் (nb < source.உச்சி_2)
    {
	COORD coord;
	முழு   i;

	coord.X = source.இடது_3;
	க்கு (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.வலது_3 - source.இடது_3 + 1);
	}
    }

    எனில் (vtp_working)
    {
	COORD coord;
	முழு i;

	coord.X = source.இடது_3;
	க்கு (i = nb; i <= source.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.வலது_3 - source.இடது_3 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    நிலையான வெறுமை
gotoxy(
    unsigned x,
    unsigned y)
{
    எனில் (x < 1 || x > (unsigned)Columns || y < 1 || y > (unsigned)Rows)
	திரும்பு;

    எனில் (!USE_VTP)
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
    இல்லையெனில்
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
    நிலையான வெறுமை
textattr(WORD wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    SetConsoleTextAttribute(g_hConOut, wAttr);
}


    நிலையான வெறுமை
textcolor(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    எனில் (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    இல்லையெனில்
	vtp_sgr_bulk(wAttr);
}


    நிலையான வெறுமை
textbackground(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    எனில் (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    இல்லையெனில்
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    நிலையான வெறுமை
normvideo(வெறுமை)
{
    எனில் (!vtp_working)
	textattr(g_attrDefault);
    இல்லையெனில்
	vtp_sgr_bulk(0);
}


நிலையான WORD g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    நிலையான வெறுமை
standout(வெறுமை)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((WORD) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    நிலையான வெறுமை
standend(வெறுமை)
{
    எனில் (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    வெறுமை
mch_set_normal_colors(வெறுமை)
{
    char_u	*p;
    முழு		n;

    cterm_normal_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_normal_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    எனில் (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	எனில் (*p == 'm' && n > 0)
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
    நிலையான வெறுமை
visual_bell(வெறுமை)
{
    COORD   coordOrigin = {0, 0};
    WORD    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = வெற்று;

# ifdef FEAT_TERMGUICOLORS
    எனில் (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(WORD, Rows * Columns);
	எனில் (oldattrs == வெற்று)
	    திரும்பு;
	ReadConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_hConOut, attrFlash, Rows * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    எனில் (oldattrs != வெற்று)
    {
	WriteConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    நிலையான வெறுமை
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    எனில் (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	திரும்பு;
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
    நிலையான DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cbToWrite)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    நிலையான DWORD    cells;
    நிலையான WCHAR    *unicodebuf = வெற்று;
    நிலையான முழு	    unibuflen = 0;
    நிலையான முழு	    length;
    முழு		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    நிலையான WCHAR    *utf8spbuf = வெற்று;
    நிலையான முழு	    utf8splength;
    நிலையான DWORD    utf8spcells;
    நிலையான WCHAR    **utf8usingbuf = &unicodebuf;

    எனில் (cbToWrite != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	செய்
	{
	    length = MultiByteToWideChar(cp, 0, (LPCSTR)pchBuf, cbToWrite,
							unicodebuf, unibuflen);
	    எனில் (length && length <= unibuflen)
		நிறுத்து;
	    vim_free(unicodebuf);
	    unicodebuf = length ? LALLOC_MULT(WCHAR, length) : வெற்று;
	    unibuflen = unibuflen ? 0 : length;
	} போது (TRUE);
	cells = mb_string2cells(pchBuf, cbToWrite);
    }
    இல்லையெனில் // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	எனில் (utf8usingbuf != &utf8spbuf)
	{
	    எனில் (utf8spbuf == வெற்று)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		length = MultiByteToWideChar(CP_UTF8, 0, " ", 1, வெற்று, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, length);
		எனில் (utf8spbuf != வெற்று)
		{
		    MultiByteToWideChar(CP_UTF8, 0, " ", 1, utf8spbuf, length);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = length;
		    utf8spcells = cells;
		}
	    }
	    இல்லையெனில்
	    {
		utf8usingbuf = &utf8spbuf;
		length = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    எனில் (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	எனில் (WriteConsoleOutputCharacterW(g_hConOut, *utf8usingbuf, length,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    இல்லையெனில்
    {
	எனில் (WriteConsoleW(g_hConOut, *utf8usingbuf, length, &cchwritten,
		    வெற்று) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    எனில் (cchwritten == (DWORD)length)
    {
	written = cbToWrite;
	g_coord.X += (SHORT)cells;
    }
    இல்லையெனில்
    {
	char_u *p = pchBuf;
	க்கு (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    போது (g_coord.X > g_srScrollRegion.வலது_3)
    {
	g_coord.X -= (SHORT) Columns;
	எனில் (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    எனில் (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    திரும்பு written;
}

    நிலையான char_u *
get_seq(
    முழு *args,
    முழு *மொத்தம்,
    char_u *head)
{
    முழு argc;
    char_u *p;

    எனில் (head == வெற்று || *head != '\033')
	திரும்பு வெற்று;

    argc = 0;
    p = head;
    ++p;
    செய்
    {
	++p;
	args[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } போது (*p == ';');
    *மொத்தம் = argc;

    திரும்பு p;
}

    நிலையான char_u *
get_sgr(
    முழு *args,
    முழு *மொத்தம்,
    char_u *head)
{
    char_u *p = get_seq(args, மொத்தம், head);

    திரும்பு (p && *p == 'm') ? ++p : வெற்று;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    நிலையான char_u *
sgrn2(
    char_u *head,
    முழு	   n)
{
    முழு argc;
    முழு args[16];
    char_u *p = get_sgr(args, &argc, head);

    திரும்பு p && argc == 5 && args[0] == n && args[1] == 2 ? p : வெற்று;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    நிலையான char_u *
sgrnc(
    char_u *head,
    முழு	   n)
{
    முழு argc;
    முழு args[16];
    char_u *p = get_sgr(args, &argc, head);

    திரும்பு p && argc == 1 && args[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : வெற்று;
}

    நிலையான char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    போது (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    திரும்பு p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    நிலையான char_u *
sgrn2c(
    char_u *head,
    முழு	   n)
{
    char_u *p = sgrn2(head, n);

    திரும்பு p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : வெற்று;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    நிலையான char_u *
sgrn2cn(
    char_u *head,
    முழு n)
{
    char_u *p = sgrn2(head, n);

    திரும்பு p && p[0] == 0x0a && p[1] == '\033' ? ++p : வெற்று;
}

நிலையான bool in_dcs = false;	// inside a DCS (sixel) sequence
நிலையான bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    நிலையான வெறுமை
write_vt(char_u *s, முழு len)
{
    முழு		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, வெற்று, 0);
    எனில் (wlen <= 0)
	திரும்பு;
    ws = ALLOC_MULT(WCHAR, wlen);
    எனில் (ws == வெற்று)
	திரும்பு;
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_hConOut, ws, wlen, &written, வெற்று);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    நிலையான char_u *
skip_dcs(char_u *s, char_u *end)
{
    க்கு (char_u *p = s; p < end; ++p)
    {
	எனில் (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    in_dcs = false;
	    திரும்பு p + 1;
	}
	dcs_esc = *p == ESC;
    }
    திரும்பு end;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    வெறுமை
mch_write(
    char_u  *s,
    முழு	    len)
{
    char_u  *end = s + len;

# ifdef VIMDLL
    எனில் (gui.in_use)
	திரும்பு;
# endif

    எனில் (!term_console)
    {
	லிக(1, s, (unsigned)len);
	திரும்பு;
    }

    // translate ESC | sequences into faked bios calls
    போது (len--)
    {
	முழு prefix = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	எனில் (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    திரும்பு;
	}

	எனில் (in_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    முழு	l = (முழு)(skip_dcs(s, end) - s);

	    எனில் (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    தொடர்;
	}

	போது (s + ++prefix < end)
	{
	    ch = s[prefix];
	    எனில் (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		நிறுத்து;
	}

	எனில் (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    எனில் (prefix != 0)
		prefix = 1;
	}

	எனில் (prefix != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, prefix);
# ifdef MCH_WRITE_DUMP
	    எனில் (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, அளவு(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	இல்லையெனில் எனில் (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    எனில் (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.இடது_3 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    இல்லையெனில்
	    {
		gotoxy(g_srScrollRegion.இடது_3 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    எனில் (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	இல்லையெனில் எனில் (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.இடது_3+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    எனில் (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	இல்லையெனில் எனில் (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    எனில் (g_coord.X > g_srScrollRegion.இடது_3)
		g_coord.X--;
	    இல்லையெனில் எனில் (g_coord.Y > g_srScrollRegion.உச்சி_2)
	    {
		g_coord.X = g_srScrollRegion.வலது_3;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    எனில் (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	இல்லையெனில் எனில் (s[0] == '\a')
	{
	    // \a, bell
	    MessageBeep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    எனில் (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	இல்லையெனில் எனில் (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    முழு	    arg1 = 0, arg2 = 0, argc = 0, args[16];
	    char_u  *sp;

	    தேர்வு (s[2])
	    {
	    நிலை '0': நிலை '1': நிலை '2': நிலை '3': நிலை '4':
	    நிலை '5': நிலை '6': நிலை '7': நிலை '8': நிலை '9':
		எனில் (*(p = get_seq(args, &argc, s)) != 'm')
		    செல் notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		எனில் (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (முழு)(p - s);
		    s = p;
		    நிறுத்து;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		எனில் (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		எனில் (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		எனில் (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		எனில் (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(args, &argc, p);

notsgr:
		arg1 = args[0];
		arg2 = args[1];
		எனில் (*p == 'm')
		{
		    எனில் (argc == 1 && args[0] == 0)
			normvideo();
		    இல்லையெனில் எனில் (argc == 1)
		    {
			எனில் (USE_VTP)
			    textcolor((WORD)arg1);
			இல்லையெனில்
			    textattr((WORD)arg1);
		    }
		    இல்லையெனில் எனில் (vtp_working)
			vtp_sgr_bulks(argc, args);
		}
		இல்லையெனில் எனில் (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		இல்லையெனில் எனில் (argc == 2 && *p == 'r')
		{
		    set_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		இல்லையெனில் எனில் (argc == 2 && *p == 'R')
		{
		    set_scroll_region_tb(arg1, arg2);
		}
		இல்லையெனில் எனில் (argc == 2 && *p == 'V')
		{
		    set_scroll_region_lr(arg1, arg2);
		}
		இல்லையெனில் எனில் (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.உச்சி_2, g_coord.Y - arg1) + 1);
		}
		இல்லையெனில் எனில் (argc == 1 && *p == 'b')
		{
		    textbackground((WORD) arg1);
		}
		இல்லையெனில் எனில் (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.வலது_3, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		இல்லையெனில் எனில் (argc == 1 && *p == 'f')
		{
		    textcolor((WORD) arg1);
		}
		இல்லையெனில் எனில் (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		இல்லையெனில் எனில் (argc == 1 && *p == 'L')
		{
		    செருகு_வரிகள்(arg1);
		}
		இல்லையெனில் எனில் (argc == 1 && *p == 'M')
		{
		    delete_வரிகள்(arg1);
		}

		len -= (முழு)(p - s);
		s = p + 1;
		நிறுத்து;

	    நிலை 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.உச்சி_2, g_coord.Y - 1) + 1);
		செல் got3;

	    நிலை 'B':
		visual_bell();
		செல் got3;

	    நிலை 'C':
		gotoxy(min(g_srScrollRegion.வலது_3, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		செல் got3;

	    நிலை 'E':
		termcap_mode_end();
		செல் got3;

	    நிலை 'F':
		standout();
		செல் got3;

	    நிலை 'f':
		standend();
		செல் got3;

	    நிலை 'H':
		gotoxy(1, 1);
		செல் got3;

	    நிலை 'j':
		clear_to_end_of_display();
		செல் got3;

	    நிலை 'J':
		clear_screen();
		செல் got3;

	    நிலை 'K':
		clear_to_end_of_வரி();
		செல் got3;

	    நிலை 'L':
		செருகு_வரிகள்(1);
		செல் got3;

	    நிலை 'M':
		delete_வரிகள்(1);
		செல் got3;

	    நிலை 'S':
		termcap_mode_start();
		செல் got3;

	    நிலை 'V':
		cursor_visible(TRUE);
		செல் got3;

	    நிலை 'v':
		cursor_visible(FALSE);
		செல் got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    எனில் (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, அளவு(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	இல்லையெனில் எனில் (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    முழு l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    போது (s + l < end && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    எனில் (s + l < end && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		எனில் (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	இல்லையெனில் எனில் (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    முழு	l;

	    in_dcs = true;
	    dcs_esc = false;
	    l = (முழு)(skip_dcs(s + 2, end) - s);
	    எனில் (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	இல்லையெனில்
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    எனில் (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, அளவு(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    எனில் (fdDump)
	fflush(fdDump);
# endif
}

#இல்லையெனில் // FEAT_GUI_MSWIN && !VIMDLL
    வெறுமை
mch_write(
    char_u  *s UNUSED,
    முழு	    len UNUSED)
{
    // never used
}

#endif


/*
 * Delay for "msec" milliseconds.
 */
    வெறுமை
mch_delay(
    long    msec,
    முழு	    flags UNUSED)
{
#எனில் defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((முழு)msec);	    // never wait for input
#இல்லையெனில் // Console
# ifdef VIMDLL
    எனில் (gui.in_use)
    {
	Sleep((முழு)msec);	    // never wait for input
	திரும்பு;
    }
# endif
    எனில் (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	எனில் (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    முழு towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    போது (msec > 0)
	    {
		mzvim_check_threads();
		எனில் (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	இல்லையெனில்
# endif
	    Sleep((முழு)msec);
    இல்லையெனில்
	WaitForChar(msec, FALSE);
#endif
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    முழு
mch_remove(char_u *name)
{
    WCHAR	*wn;
    முழு		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    எனில் (mch_isdir(name) && mch_is_symbolic_link(name))
	திரும்பு mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_NORMAL);

    wn = enc_to_utf16(name, வெற்று);
    எனில் (wn == வெற்று)
	திரும்பு -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    திரும்பு n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    வெறுமை
mch_breakcheck(முழு force UNUSED)
{
#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    எனில் (!gui.in_use)
# endif
	எனில் (g_fCtrlCPressed || g_fCBrkPressed)
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
mch_total_mem(முழு special UNUSED)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dwLength = அளவு(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&ms);
    எனில் (ms.ullAvailVirtual < ms.ullTotalPhys)
    {
	// Process address space fits in physical RAM, use all of it.
	திரும்பு (long_u)(ms.ullAvailVirtual / 1024);
    }
    எனில் (ms.ullTotalPhys <= WINNT_RESERVE_BYTES)
    {
	// Catch old NT box or perverse hardware setup.
	திரும்பு (long_u)((ms.ullTotalPhys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    திரும்பு (long_u)((ms.ullTotalPhys - WINNT_RESERVE_BYTES) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    முழு
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    முழு		flags)		// EW_* flags
{
    திரும்பு dos_expandpath(gap, path, 0, flags, FALSE);
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
    முழு
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	szTempFile[_MAX_PATH + 1];
    WCHAR	szNewPath[_MAX_PATH + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    க்கு (p = q = wold; *p != L'\0'; ++p)
    {
	தேர்வு (*p)
	{
	நிலை L'/':
	நிலை L'\\':
	நிலை L':':
	    எனில் (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    நிறுத்து;

	இயல்பு:
	    நிறுத்து;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    எனில் ((முழு)(p - q) < 8 || q[6] != L'~')
	திரும்பு (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    எனில் (GetFullPathNameW(wnew, _MAX_PATH, szNewPath, &p) == 0 || p == வெற்று)
	திரும்பு -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    எனில் (GetTempFileNameW(szNewPath, L"VIM", 0, szTempFile) == 0)
	திரும்பு -2;

    // blow the temp file away
    எனில் (!DeleteFileW(szTempFile))
	திரும்பு -3;

    // rename old file to the temp file
    எனில் (!MoveFileW(wold, szTempFile))
	திரும்பு -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    எனில் ((hf = CreateFileW(wold, GENERIC_WRITE, 0, வெற்று, CREATE_NEW,
		    FILE_ATTRIBUTE_NORMAL, வெற்று)) == INVALID_HANDLE_மதிப்பு)
	திரும்பு -5;
    எனில் (!CloseHandle(hf))
	திரும்பு -6;

    // rename the temp file to the new file
    எனில் (!MoveFileW(szTempFile, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(வெறுமை)MoveFileW(szTempFile, wold);
	திரும்பு -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(szTempFile);

    // finally, remove the empty old file
    எனில் (!DeleteFileW(wold))
	திரும்பு -8;

    திரும்பு 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    முழு
mch_rename(
    மாறிலி எழுத்து	*pszOldFile,
    மாறிலி எழுத்து	*pszNewFile)
{
    WCHAR	*wold = வெற்று;
    WCHAR	*wnew = வெற்று;
    முழு		retval = -1;

    wold = enc_to_utf16((char_u *)pszOldFile, வெற்று);
    wnew = enc_to_utf16((char_u *)pszNewFile, வெற்று);
    எனில் (wold != வெற்று && wnew != வெற்று)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    திரும்பு retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    எழுத்து *
default_shell(வெறுமை)
{
    திரும்பு "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    முழு
mch_access(எழுத்து *n, முழு p)
{
    HANDLE	hFile;
    முழு		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_to_utf16((char_u *)n, வெற்று);
    எனில் (wn == வெற்று)
	திரும்பு -1;

    எனில் (mch_isdir((char_u *)n))
    {
	WCHAR TempNameW[_MAX_PATH + 16] = L"";

	எனில் (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    முழு			i;
	    WIN32_FIND_DATAW    d;

	    க்கு (i = 0; i < _MAX_PATH && wn[i] != 0; ++i)
		TempNameW[i] = wn[i];
	    எனில் (TempNameW[i - 1] != '\\' && TempNameW[i - 1] != '/')
		TempNameW[i++] = '\\';
	    TempNameW[i++] = '*';
	    TempNameW[i++] = 0;

	    hFile = FindFirstFileW(TempNameW, &d);
	    எனில் (hFile == INVALID_HANDLE_மதிப்பு)
		செல் getout;
	    இல்லையெனில்
		(வெறுமை)FindClose(hFile);
	}

	எனில் (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    எனில் (!GetTempFileNameW(wn, L"VIM", 0, TempNameW))
		செல் getout;
	    இல்லையெனில்
		DeleteFileW(TempNameW);
	}
    }
    இல்லையெனில்
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_mode = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_mode, share_mode,
					     வெற்று, OPEN_EXISTING, 0, வெற்று);
	எனில் (hFile == INVALID_HANDLE_மதிப்பு)
	    செல் getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    திரும்பு retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    முழு
mch_open(மாறிலி எழுத்து *name, முழு flags, முழு mode)
{
    WCHAR	*wn;
    முழு		f;

    wn = enc_to_utf16((char_u *)name, வெற்று);
    எனில் (wn == வெற்று)
	திரும்பு -1;

    f = _wopen(wn, flags, mode);
    vim_free(wn);
    திரும்பு f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(மாறிலி எழுத்து *name, மாறிலி எழுத்து *mode)
{
    WCHAR	*wn, *wm;
    FILE	*f = வெற்று;

#எனில் defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    எழுத்து newMode = mode[strlen(mode) - 1];
    முழு oldMode = 0;

    _get_fmode(&oldMode);
    எனில் (newMode == 't')
	_set_fmode(_O_TEXT);
    இல்லையெனில் எனில் (newMode == 'b')
	_set_fmode(O_இருமம்_2);
#endif
    wn = enc_to_utf16((char_u *)name, வெற்று);
    wm = enc_to_utf16((char_u *)mode, வெற்று);
    எனில் (wn != வெற்று && wm != வெற்று)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#எனில் defined(DEBUG) && _MSC_VER >= 1400
    எனில் (oldMode != 0)
	_set_fmode(oldMode);
#endif
    திரும்பு f;
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
    நிலையான வெறுமை
copy_substream(HANDLE sh, வெறுமை *context, WCHAR *to, WCHAR *substream, long len)
{
    HANDLE  hTo;
    WCHAR   *to_name;

    to_name = alloc((wcslen(to) + wcslen(substream) + 1) * அளவு(WCHAR));
    எனில் (to_name == வெற்று)
	திரும்பு;

    wcscpy(to_name, to);
    wcscat(to_name, substream);

    hTo = CreateFileW(to_name, GENERIC_WRITE, 0, வெற்று, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL, வெற்று);
    எனில் (hTo != INVALID_HANDLE_மதிப்பு)
    {
	long	done;
	DWORD	todo;
	DWORD	readcnt, written;
	எழுத்து	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	க்கு (done = 0; done < len; done += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - done) > அளவு(buf) ? அளவு(buf)
						       : (size_t)(len - done));
	    எனில் (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(hTo, buf, todo, &written, வெற்று)
		    || written != todo)
		நிறுத்து;
	}
	CloseHandle(hTo);
    }

    விடுவி(to_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    நிலையான வெறுமை
copy_infostreams(char_u *from, char_u *to)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_STREAM_ID	sid;
    முழு			headersize;
    WCHAR		streamname[_MAX_PATH];
    DWORD		readcount;
    வெறுமை		*context = வெற்று;
    DWORD		lo, hi;
    முழு			len;

    // Convert the file names to wide characters.
    fromw = enc_to_utf16(from, வெற்று);
    tow = enc_to_utf16(to, வெற்று);
    எனில் (fromw != வெற்று && tow != வெற்று)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, வெற்று,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, வெற்று);
	எனில் (sh != INVALID_HANDLE_மதிப்பு)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    க்கு (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		ZeroMemory(&sid, அளவு(WIN32_STREAM_ID));
		headersize = (முழு)((எழுத்து *)&sid.cStreamName - (எழுத்து *)&sid.dwStreamId);
		எனில் (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    நிறுத்து;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		எனில் (sid.dwStreamNameSize > 0)
		{
		    // Read the stream name.
		    எனில் (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dwStreamNameSize,
					  &readcount, FALSE, FALSE, &context))
			நிறுத்து;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / அளவு(WCHAR);
		    streamname[len] = 0;
		    எனில் (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (long)sid.Size.u.கீழ்_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(வெறுமை)BackupSeek(sh, sid.Size.u.கீழ்_Part, sid.Size.u.உயர்வு_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (வெறுமை)BackupRead(sh, வெற்று, 0, &readcount, TRUE, FALSE, &context);

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

வகைவரை கட்டமைப்பு _FILE_FULL_EA_INFORMATION_ {
    ULONG  NextEntryOffset;
    UCHAR  Flags;
    UCHAR  EaNameLength;
    USHORT Ea_மதிப்பு_Length;
    CHAR   EaName[1];
} FILE_FULL_EA_INFORMATION_, *PFILE_FULL_EA_INFORMATION_;

வகைவரை கட்டமைப்பு _FILE_EA_INFORMATION_ {
    ULONG EaSize;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#ifndef PROTO
வகைவரை NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions);
வகைவரை NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
வகைவரை NTSTATUS (NTAPI *PfnNtSetEaFile)(
	HANDLE		    FileHandle,
	PIO_STATUS_BLOCK    IoStatusBlock,
	PVOID		    Buffer,
	ULONG		    Length);
வகைவரை NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	BOOLEAN ReturnSingleEntry,
	PVOID EaList,
	ULONG EaListLength,
	PULONG EaIndex,
	BOOLEAN RestartScan);
வகைவரை NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_STATUS_BLOCK	IoStatusBlock,
	PVOID			FileInformation,
	ULONG			Length,
	FILE_INFORMATION_CLASS FileInformationClass);
வகைவரை VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString);

PfnNtOpenFile pNtOpenFile = வெற்று;
PfnNtClose pNtClose = வெற்று;
PfnNtSetEaFile pNtSetEaFile = வெற்று;
PfnNtQueryEaFile pNtQueryEaFile = வெற்று;
PfnNtQueryInformationFile pNtQueryInformationFile = வெற்று;
PfnRtlInitUnicodeString pRtlInitUnicodeString = வெற்று;
#endif

/*
 * Load ntdll.dll functions.
 */
    நிலையான BOOL
load_ntdll(வெறுமை)
{
    நிலையான முழு	loaded = -1;

    எனில் (loaded != -1)
	திரும்பு (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    எனில் (hNtdll != வெற்று)
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
    எனில் (pNtOpenFile == வெற்று
	    || pNtClose == வெற்று
	    || pNtSetEaFile == வெற்று
	    || pNtQueryEaFile == வெற்று
	    || pNtQueryInformationFile == வெற்று
	    || pRtlInitUnicodeString == வெற்று)
	loaded = FALSE;
    இல்லையெனில்
	loaded = TRUE;
    திரும்பு (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    நிலையான வெறுமை
copy_extattr(char_u *from, char_u *to)
{
    char_u		    *fromf = வெற்று;
    char_u		    *tof = வெற்று;
    WCHAR		    *fromw = வெற்று;
    WCHAR		    *tow = வெற்று;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_ATTRIBUTES	    oa;
    IO_STATUS_BLOCK	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    வெறுமை		    *ea = வெற்று;

    எனில் (!load_ntdll())
	திரும்பு;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(to) + 5);
    எனில் (fromf == வெற்று || tof == வெற்று)
	செல் theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), to);

    // Convert the names to wide characters.
    fromw = enc_to_utf16(fromf, வெற்று);
    tow = enc_to_utf16(tof, வெற்று);
    எனில் (fromw == வெற்று || tow == வெற்று)
	செல் theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    InitializeObjectAttributes(&oa, &u, 0, வெற்று, வெற்று);
    எனில் (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	செல் theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, அளவு(eainfo),
	    FileEaInformation);
    எனில் (eainfo.EaSize != 0)
    {
	ea = alloc(eainfo.EaSize);
	எனில் (ea != வெற்று)
	{
	    எனில் (pNtQueryEaFile(h, &iosb, ea, eainfo.EaSize, FALSE,
			வெற்று, 0, வெற்று, TRUE) != STATUS_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    எனில் (ea != வெற்று)
    {
	pRtlInitUnicodeString(&u, tow);
	InitializeObjectAttributes(&oa, &u, 0, வெற்று, வெற்று);
	எனில் (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	    செல் theend;

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
    முழு
mch_copy_file_attribute(char_u *from, char_u *to)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, to);
    copy_extattr(from, to);
    திரும்பு 0;
}


/*
 * The command line arguments in UTF-16
 */
நிலையான முழு	nArgsW = 0;
நிலையான LPWSTR	*ArglistW = வெற்று;
நிலையான முழு	global_argc = 0;
நிலையான எழுத்து	**global_argv;

நிலையான முழு	used_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
நிலையான முழு	*used_file_indexes = வெற்று; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
நிலையான முழு	used_file_மொத்தம் = 0;	// nr of entries in used_file_indexes
நிலையான முழு	used_file_literal = FALSE;  // take file names literally
நிலையான முழு	used_file_full_path = FALSE;  // file name was full path
#ifdef FEAT_DIFF
நிலையான முழு	used_file_diff_mode = FALSE;  // file name was with diff mode
#endif
நிலையான முழு	used_alist_மொத்தம் = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    முழு
get_cmd_argsW(எழுத்து ***argvp)
{
    எழுத்து	**argv = வெற்று;
    முழு		argc = 0;
    முழு		i;

    free_cmd_argsW();
    ArglistW = Command_வரி_To_Argv_W(Get_Command_வரி_W(), &nArgsW);
    எனில் (ArglistW != வெற்று)
    {
	argv = alloc((nArgsW + 1) * அளவு(எழுத்து *));
	எனில் (argv != வெற்று)
	{
	    argc = nArgsW;
	    argv[argc] = வெற்று;
	    க்கு (i = 0; i < argc; ++i)
	    {
		முழு	len;

		// Convert each Unicode argument to UTF-8.
		WideCharToMultiByte_alloc(CP_UTF8, 0,
				ArglistW[i], (முழு)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		எனில் (argv[i] == வெற்று)
		{
		    // Out of memory, clear everything.
		    போது (i > 0)
			விடுவி(argv[--i]);
		    விடுவி(argv);
		    argv = வெற்று;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    எனில் (argc > 0)
    {
	எனில் (used_file_indexes != வெற்று)
	    விடுவி(used_file_indexes);
	used_file_indexes = alloc(argc * அளவு(முழு));
    }

    எனில் (argvp != வெற்று)
	*argvp = argv;
    திரும்பு argc;
}

    வெறுமை
free_cmd_argsW(வெறுமை)
{
    எனில் (ArglistW == வெற்று)
	திரும்பு;

    GlobalFree(ArglistW);
    ArglistW = வெற்று;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    வெறுமை
used_file_arg(எழுத்து *name, முழு literal, முழு full_path, முழு diff_mode)
{
    முழு		i;

    எனில் (used_file_indexes == வெற்று)
	திரும்பு;
    க்கு (i = used_file_argc + 1; i < global_argc; ++i)
	எனில் (STRCMP(global_argv[i], name) == 0)
	{
	    used_file_argc = i;
	    used_file_indexes[used_file_மொத்தம்++] = i;
	    நிறுத்து;
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
    வெறுமை
set_alist_மொத்தம்(வெறுமை)
{
    used_alist_மொத்தம் = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    வெறுமை
fix_arg_enc(வெறுமை)
{
    முழு		i;
    முழு		idx;
    char_u	*str;
    முழு		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    எனில் (global_argc != nArgsW
	    || ArglistW == வெற்று
	    || used_file_indexes == வெற்று
	    || used_file_மொத்தம் == 0
	    || used_alist_மொத்தம் != GARGCOUNT)
	திரும்பு;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(முழு, GARGCOUNT);
    எனில் (fnum_list == வெற்று)
	திரும்பு;		// out of memory
    க்கு (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    எனில் (ga_grow(&global_alist.al_ga, used_file_மொத்தம்) == FAIL)
	திரும்பு;		// out of memory

    க்கு (i = 0; i < used_file_மொத்தம்; ++i)
    {
	idx = used_file_indexes[i];
	str = utf16_to_enc(ArglistW[idx], வெற்று);
	எனில் (str != வெற்று)
	{
	    முழு literal = used_file_literal;

#ifdef FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    எனில் (used_file_diff_mode && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), tail, STRLEN(tail), TRUE, &ret);
		எனில் (ret.string != வெற்று)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#endif
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    எனில் (used_file_literal)
		buf_set_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    எனில் (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		எனில் (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_add(&global_alist, str, literal ? 2 : 0);
	}
    }

    எனில் (!used_file_literal)
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
	alist_expand(fnum_list, used_alist_மொத்தம்);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    எனில் (curwin->w_arg_idx == 0 && curbuf->b_fname == வெற்று)
    {
	do_cmdline_cmd((char_u *)":rewind");
	எனில் (GARGCOUNT == 1 && used_file_full_path
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    last_chdir_reason = "drop";
    }

    set_alist_மொத்தம்();
}

    முழு
mch_setenv(எழுத்து *var, எழுத்து *மதிப்பு, முழு x UNUSED)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(மதிப்பு) + 2);
    எனில் (envbuf == வெற்று)
	திரும்பு -1;

    sprintf((எழுத்து *)envbuf, "%s=%s", var, மதிப்பு);

    p = enc_to_utf16(envbuf, வெற்று);

    vim_free(envbuf);
    எனில் (p == வெற்று)
	திரும்பு -1;
    _wputenv(p);
#ifdef libintl_wputenv
    libintl_wputenv(p);
#endif
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    திரும்பு 0;
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

    நிலையான வெறுமை
vtp_flag_init(வெறுமை)
{
#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   mode;
    HANDLE  out;

# ifdef VIMDLL
    எனில் (!gui.in_use)
# endif
    {
	out = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_version >= VTP_FIRST_SUPPORT_BUILD) ? 1 : 0;
	GetConsoleMode(out, &mode);
	mode |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	எனில் (SetConsoleMode(out, mode) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_buffer = win_version >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#endif

    எனில் (win_version >= CONPTY_FIRST_SUPPORT_BUILD)
	conpty_working = 1;
    எனில் (win_version >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    எனில் (win_version <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    எனில் (win_version <= CONPTY_1909_BUILD)
	conpty_type = 2;
    எனில் (win_version <= CONPTY_1903_BUILD)
	conpty_type = 2;
    எனில் (win_version < CONPTY_FIRST_SUPPORT_BUILD)
	conpty_type = 1;

    எனில் (win_version >= CONPTY_NEXT_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    நிலையான வெறுமை
vtp_init(வெறுமை)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = அளவு(csbi);
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

    நிலையான வெறுமை
vtp_exit(வெறுமை)
{
    restore_console_color_rgb();
}

    முழு
vtp_printf(
    எழுத்து *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   result;
    முழு	    len;

    எனில் (silent_mode)
	திரும்பு 0;

    va_start(list, format);
    len = vim_vsnprintf((எழுத்து *)buf, 100, (எழுத்து *)format, list);
    va_end(list);
    WriteConsoleA(g_hConOut, buf, (DWORD)len, &result, வெற்று);
    திரும்பு (முழு)result;
}

    நிலையான வெறுமை
vtp_sgr_bulk(
    முழு arg)
{
    முழு args[1];

    args[0] = arg;
    vtp_sgr_bulks(1, args);
}

# define FAST256(x) \
    எனில் ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    நிலை x: \
	FAST256(newargs[x - 1]);

    நிலையான வெறுமை
vtp_sgr_bulks(
    முழு argc,
    முழு *args)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    முழு	    in, out;
    முழு	    newargs[16];
    நிலையான முழு sgrfgr = -1, sgrfgg, sgrfgb;
    நிலையான முழு sgrbgr = -1, sgrbgg, sgrbgb;

    எனில் (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	திரும்பு;
    }

    in = out = 0;
    போது (in < argc)
    {
	முழு s = args[in];
	முழு copylen = 1;

	எனில் (s == 38)
	{
	    எனில் (argc - in >= 5 && args[in + 1] == 2)
	    {
		எனில் (sgrfgr == args[in + 2] && sgrfgg == args[in + 3]
						     && sgrfgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		இல்லையெனில்
		{
		    sgrfgr = args[in + 2];
		    sgrfgg = args[in + 3];
		    sgrfgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    இல்லையெனில் எனில் (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	இல்லையெனில் எனில் (s == 48)
	{
	    எனில் (argc - in >= 5 && args[in + 1] == 2)
	    {
		எனில் (sgrbgr == args[in + 2] && sgrbgg == args[in + 3]
						     && sgrbgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		இல்லையெனில்
		{
		    sgrbgr = args[in + 2];
		    sgrbgg = args[in + 3];
		    sgrbgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    இல்லையெனில் எனில் (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	இல்லையெனில் எனில் (30 <= s && s <= 39)
	    sgrfgr = -1;
	இல்லையெனில் எனில் (90 <= s && s <= 97)
	    sgrfgr = -1;
	இல்லையெனில் எனில் (40 <= s && s <= 49)
	    sgrbgr = -1;
	இல்லையெனில் எனில் (100 <= s && s <= 107)
	    sgrbgr = -1;
	இல்லையெனில் எனில் (s == 0)
	    sgrfgr = sgrbgr = -1;

	போது (copylen--)
	    newargs[out++] = args[in++];
    }

    p = &buf[அளவு(buf) - 1];
    *p-- = 'm';

    தேர்வு (out)
    {
	முழு	n, m;
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
	WriteConsoleA(g_hConOut, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, வெற்று);
    இயல்பு:
	நிறுத்து;
    }
}

    நிலையான வெறுமை
wt_init(வெறுமை)
{
    wt_working = mch_getenv("WT_SESSION") != வெற்று;
}

# ifdef FEAT_TERMGUICOLORS
    நிலையான முழு
ctermtoxterm(
    முழு cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    திரும்பு (((முழு)r << 16) | ((முழு)g << 8) | (முழு)b);
}
# endif

    நிலையான வெறுமை
set_console_color_rgb(வெறுமை)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    guicolor_T	fg, bg;
    முழு		ctermfg, ctermbg;

    எனில் (!vtp_working)
	திரும்பு;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    எனில் (p_tgc || t_colors >= 256)
    {
	term_fg_rgb_color(fg);
	term_bg_rgb_color(bg);
	திரும்பு;
    }

    எனில் (use_alternate_screen_buffer)
	திரும்பு;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cbSize = அளவு(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = அளவு(csbi);
    csbi.srWindow.வலது_3 += 1;
    csbi.srWindow.Bottom += 1;
    store_console_bg_rgb = csbi.ColorTable[g_color_index_bg];
    store_console_fg_rgb = csbi.ColorTable[g_color_index_fg];
    csbi.ColorTable[g_color_index_bg] = (COLORREF)bg;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)fg;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

# எனில் defined(FEAT_TERMGUICOLORS)
    வெறுமை
get_default_console_color(
    முழு *cterm_fg,
    முழு *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    முழு id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    முழு ctermfg = 0;
    முழு ctermbg = 0;
    முழு dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    எனில் (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    எனில் (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	எனில் (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	எனில் (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	இல்லையெனில்
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_normal_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    எனில் (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	எனில் (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	எனில் (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	இல்லையெனில்
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
    நிலையான வெறுமை
reset_console_color_rgb(வெறுமை)
{
# ifdef FEAT_TERMGUICOLORS
    எனில் (use_alternate_screen_buffer)
	திரும்பு;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = அளவு(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = அளவு(csbi);
    csbi.srWindow.வலது_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)store_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)store_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    நிலையான வெறுமை
restore_console_color_rgb(வெறுமை)
{
# ifdef FEAT_TERMGUICOLORS
    எனில் (use_alternate_screen_buffer)
	திரும்பு;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = அளவு(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = அளவு(csbi);
    csbi.srWindow.வலது_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)save_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)save_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

    வெறுமை
control_console_color_rgb(வெறுமை)
{
    எனில் (vtp_working)
	set_console_color_rgb();
    இல்லையெனில்
	reset_console_color_rgb();
}

    முழு
use_vtp(வெறுமை)
{
    திரும்பு USE_VTP;
}

    முழு
is_term_win32(வெறுமை)
{
    திரும்பு T_NAME != வெற்று && STRCMP(T_NAME, "win32") == 0;
}

    முழு
has_vtp_working(வெறுமை)
{
    திரும்பு vtp_working;
}

#endif

    முழு
has_conpty_working(வெறுமை)
{
    திரும்பு conpty_working;
}

    முழு
get_conpty_type(வெறுமை)
{
    திரும்பு conpty_type;
}

    முழு
is_conpty_stable(வெறுமை)
{
    திரும்பு conpty_stable;
}

    முழு
get_conpty_fix_type(வெறுமை)
{
    திரும்பு conpty_fix_type;
}

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    வெறுமை
resize_console_buf(வெறுமை)
{
    எனில் (use_alternate_screen_buffer)
	திரும்பு;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    எனில் (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	திரும்பு;

    coord.X = SRWIDTH(csbi.srWindow);
    coord.Y = SRHEIGHT(csbi.srWindow);
    SetConsoleScreenBufferSize(g_hConOut, coord);

    newsize.இடது_3 = 0;
    newsize.உச்சி_2 = 0;
    newsize.வலது_3 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    SetConsoleWindowInfo(g_hConOut, TRUE, &newsize);

    SetConsoleScreenBufferSize(g_hConOut, coord);
}
#endif

    எழுத்து *
GetWin32Error(வெறுமை)
{
    நிலையான எழுத்து	*oldmsg = வெற்று;
    எழுத்து	*acp_செய்தி = வெற்று;
    DWORD	acp_len;
    எழுத்து	*enc_செய்தி = வெற்று;
    முழு		enc_len = 0;

    // get formatted message from OS
    acp_len = FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
	    வெற்று, GetLastError(), 0, (LPSTR)&acp_செய்தி, 0, வெற்று);
    எனில் (acp_len == 0 || acp_செய்தி == வெற்று)
	திரும்பு வெற்று;

    // clean oldmsg if remained.
    எனில் (oldmsg != வெற்று)
    {
	vim_free(oldmsg);
	oldmsg = வெற்று;
    }

    acp_to_enc((char_u *)acp_செய்தி, (முழு)acp_len, (char_u **)&enc_செய்தி, &enc_len);
    LocalFree(acp_செய்தி);
    எனில் (enc_செய்தி == வெற்று)
	திரும்பு வெற்று;

    // remove trailing \r\n
    எழுத்து *pcrlf = strstr(enc_செய்தி, "\r\n");
    எனில் (pcrlf != வெற்று)
	*pcrlf = NUL;

    oldmsg = enc_செய்தி;
    திரும்பு enc_செய்தி;
}

#எனில் defined(FEAT_RELTIME)
நிலையான HANDLE   timer_handle;
நிலையான முழு      timer_active = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
நிலையான sig_atomic_t timeout_flags[2];
நிலையான முழு	    timeout_flag_idx = 0;
நிலையான sig_atomic_t *timeout_flag = &timeout_flags[0];


    நிலையான வெறுமை CALLBACK
set_flag(வெறுமை *param, BOOLEAN unused2 UNUSED)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    வெறுமை
stop_timeout(வெறுமை)
{
    எனில் (timer_active)
    {
	BOOL ret = DeleteTimerQueueTimer(வெற்று, timer_handle, வெற்று);
	timer_active = FALSE;
	எனில் (!ret && GetLastError() != ERROR_IO_PENDING)
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
    நிலையற்ற sig_atomic_t *
start_timeout(long msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = CreateTimerQueueTimer(
	    &timer_handle, வெற்று, set_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    எனில் (!ret)
    {
	semsg(_(e_could_not_set_timeout_str), GetWin32Error());
    }
    இல்லையெனில்
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_active = TRUE;
	*timeout_flag = FALSE;
    }
    திரும்பு timeout_flag;
}
#endif
