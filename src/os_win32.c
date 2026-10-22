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

#অন্তর্ভাৱ "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#অন্তর্ভাৱ <sys/types.h>
#অন্তর্ভাৱ <signal.h>
#অন্তর্ভাৱ <limits.h>

#অন্তর্ভাৱ <process.h>
#অন্তর্ভাৱ <winternl.h>
#অন্তর্ভাৱ <direct.h>

#যদি !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#ifdef FEAT_JOB_CHANNEL
# include <tlhelp32.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = রিক্ত;
#endif

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
স্থির HANDLE g_hConIn  = INVALID_HANDLE_মান;
স্থির HANDLE g_hConOut = INVALID_HANDLE_মান;

// Win32 Screen buffer,coordinate,console I/O information
স্থির SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
স্থির COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
স্থির WORD  g_attrDefault = 7;  // lightgray text on black background
স্থির WORD  g_attrCurrent;

স্থির পূর্ণাঙ্ক g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
স্থির পূর্ণাঙ্ক g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
স্থির পূর্ণাঙ্ক g_fForceExit = FALSE;    // set when forcefully exiting

স্থির শূন্য scroll(unsigned c_সাৰিগুলি);
স্থির শূন্য set_scroll_region(unsigned বাম, unsigned শীৰ্ষ,
			      unsigned ডান, unsigned bottom);
স্থির শূন্য set_scroll_region_tb(unsigned শীৰ্ষ, unsigned bottom);
স্থির শূন্য set_scroll_region_lr(unsigned বাম, unsigned ডান);
স্থির শূন্য ঢোকাও_সাৰিগুলি(unsigned c_সাৰিগুলি);
স্থির শূন্য delete_সাৰিগুলি(unsigned c_সাৰিগুলি);
স্থির শূন্য gotoxy(unsigned x, unsigned y);
স্থির শূন্য standout(শূন্য);
স্থির পূর্ণাঙ্ক s_cursor_visible = TRUE;
স্থির পূর্ণাঙ্ক did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
প্রকারনির্দেশ সংরচনা প্ৰবেশ_record_buffer_গ্ৰন্থি_S
{
    প্ৰবেশ_RECORD ir;
    সংরচনা প্ৰবেশ_record_buffer_গ্ৰন্থি_S *next;
} প্ৰবেশ_record_buffer_গ্ৰন্থি_T;
প্রকারনির্দেশ সংরচনা প্ৰবেশ_record_buffer_S
{
    প্ৰবেশ_record_buffer_গ্ৰন্থি_T *head;
    প্ৰবেশ_record_buffer_গ্ৰন্থি_T *tail;
    পূর্ণাঙ্ক length;
} প্ৰবেশ_record_buffer_T;
স্থির প্ৰবেশ_record_buffer_T প্ৰবেশ_record_buffer;
স্থির পূর্ণাঙ্ক read_প্ৰবেশ_record_buffer(প্ৰবেশ_RECORD* irEvents, পূর্ণাঙ্ক nMaxLength);
স্থির পূর্ণাঙ্ক write_প্ৰবেশ_record_buffer(প্ৰবেশ_RECORD* irEvents, পূর্ণাঙ্ক nLength);
#endif
#ifdef FEAT_GUI_MSWIN
স্থির পূর্ণাঙ্ক s_dont_use_vimrun = TRUE;
স্থির পূর্ণাঙ্ক need_vimrun_warning = FALSE;
স্থির string_T vimrun_path = {(char_u *)"vimrun ", 7};
স্থির পূর্ণাঙ্ক vimrun_path_allocated = FALSE;
#endif

স্থির পূর্ণাঙ্ক win32_getattrs(char_u *name);
স্থির পূর্ণাঙ্ক win32_setattrs(char_u *name, পূর্ণাঙ্ক attrs);
স্থির পূর্ণাঙ্ক win32_set_archive(char_u *name);

স্থির পূর্ণাঙ্ক conpty_working = 0;
স্থির পূর্ণাঙ্ক conpty_type = 0;
স্থির পূর্ণাঙ্ক conpty_stable = 0;
স্থির পূর্ণাঙ্ক conpty_fix_type = 0;
স্থির শূন্য vtp_flag_init(শূন্য);

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
স্থির পূর্ণাঙ্ক vtp_working = 0;
স্থির শূন্য vtp_init(শূন্য);
স্থির শূন্য vtp_exit(শূন্য);
স্থির শূন্য vtp_sgr_bulk(পূর্ণাঙ্ক arg);
স্থির শূন্য vtp_sgr_bulks(পূর্ণাঙ্ক argc, পূর্ণাঙ্ক *argv);

স্থির পূর্ণাঙ্ক wt_working = 0;
স্থির শূন্য wt_init(শূন্য);

স্থির পূর্ণাঙ্ক g_color_index_bg = 0;
স্থির পূর্ণাঙ্ক g_color_index_fg = 7;

# ifdef FEAT_TERMGUICOLORS
স্থির guicolor_T save_console_bg_rgb;
স্থির guicolor_T save_console_fg_rgb;
স্থির guicolor_T store_console_bg_rgb;
স্থির guicolor_T store_console_fg_rgb;
স্থির পূর্ণাঙ্ক default_console_color_bg = 0x000000; // black
স্থির পূর্ণাঙ্ক default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_term_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# অন্যথা
#  define USE_VTP		0
#  define USE_WT		0
# endif

স্থির শূন্য set_console_color_rgb(শূন্য);
স্থির শূন্য reset_console_color_rgb(শূন্য);
স্থির শূন্য restore_console_color_rgb(শূন্য);
#endif  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
স্থির পূর্ণাঙ্ক suppress_winsize = 1;	// don't fiddle with console
#endif

স্থির WCHAR *exe_pathw = রিক্ত;

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
স্থির BOOL use_alternate_screen_buffer = FALSE;
#endif

বাহ্য DWORD win_version; // this is in os_mswin.c

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    স্থির BOOL
is_ambiwidth_event(
    প্ৰবেশ_RECORD *ir)
{
    ঘূৰাই ir->EventType == চাবি_EVENT
		&& ir->Event.চাবি_Event_2.b_চাবি_Down
		&& ir->Event.চাবি_Event_2.w_Repeat_গুনতি == 1
		&& ir->Event.চাবি_Event_2.w_Virtual_চাবি_Code == 0x12
		&& ir->Event.চাবি_Event_2.wVirtualScanCode == 0x38
		&& ir->Event.চাবি_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.চাবি_Event_2.dw_Control_চাবি_State == 2;
}

    স্থির শূন্য
make_ambiwidth_event(
    প্ৰবেশ_RECORD *down,
    প্ৰবেশ_RECORD *up)
{
    down->Event.চাবি_Event_2.w_Virtual_চাবি_Code = 0;
    down->Event.চাবি_Event_2.wVirtualScanCode = 0;
    down->Event.চাবি_Event_2.uChar.UnicodeChar
				    = up->Event.চাবি_Event_2.uChar.UnicodeChar;
    down->Event.চাবি_Event_2.dw_Control_চাবি_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    স্থির BOOL
read_console_প্ৰবেশ(
    HANDLE	    h_প্ৰবেশ,
    প্ৰবেশ_RECORD    *lpBuffer,
    পূর্ণাঙ্ক		    nLength,
    LPDWORD	    lpEvents)
{
    গণনা
    {
	IRSIZE = 10
    };
    স্থির প্ৰবেশ_RECORD s_irCache[IRSIZE];
    স্থির DWORD s_dwIndex = 0;
    স্থির DWORD s_dwMax = 0;
    DWORD dwEvents;
    পূর্ণাঙ্ক head;
    পূর্ণাঙ্ক tail;
    পূর্ণাঙ্ক i;
    স্থির প্ৰবেশ_RECORD s_irPseudo;

    যদি (s_dwMax == 0 && প্ৰবেশ_record_buffer.length > 0)
    {
	dwEvents = read_প্ৰবেশ_record_buffer(s_irCache, IRSIZE);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
    }

    যদি (nLength == -2)
	ঘূৰাই (s_dwMax > 0) ? TRUE : FALSE;

    যদি (win_version < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	যদি (nLength == -1)
	    ঘূৰাই Peek_Console_প্ৰবেশ_W(h_প্ৰবেশ, lpBuffer, 1, lpEvents);
	ঘূৰাই Read_Console_প্ৰবেশ_W(h_প্ৰবেশ, lpBuffer, 1, &dwEvents);
    }

    যদি (s_dwMax == 0)
    {
	যদি (!vtp_working && nLength == -1)
	    ঘূৰাই Peek_Console_প্ৰবেশ_W(h_প্ৰবেশ, lpBuffer, 1, lpEvents);
	Get_সংখ্যা_Of_Console_প্ৰবেশ_Events(h_প্ৰবেশ, &dwEvents);
	যদি (dwEvents == 0 && nLength == -1)
	    ঘূৰাই Peek_Console_প্ৰবেশ_W(h_প্ৰবেশ, lpBuffer, 1, lpEvents);
	Read_Console_প্ৰবেশ_W(h_প্ৰবেশ, s_irCache, IRSIZE, &dwEvents);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
	যদি (dwEvents == 0)
	{
	    *lpEvents = 0;
	    ঘূৰাই TRUE;
	}

	বাবে (i = s_dwIndex; i < (পূর্ণাঙ্ক)s_dwMax - 1; ++i)
	    যদি (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	যদি (s_dwMax > 1)
	{
	    head = 0;
	    tail = s_dwMax - 1;
	    যেতিয়ালৈ (head != tail)
	    {
		যদি (s_irCache[head].EventType == WINDOW_BUFFER_SIZE_EVENT
			&& s_irCache[head + 1].EventType
						  == WINDOW_BUFFER_SIZE_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    বাবে (i = head; i < tail; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --tail;
		    অনুৱর্ত;
		}
		head++;
	    }
	    s_dwMax = tail + 1;
	}
    }

    যদি (s_irCache[s_dwIndex].EventType == চাবি_EVENT)
    {
	যদি (s_irCache[s_dwIndex].Event.চাবি_Event_2.w_Repeat_গুনতি > 1)
	{
	    s_irPseudo = s_irCache[s_dwIndex];
	    s_irPseudo.Event.চাবি_Event_2.w_Repeat_গুনতি = 1;
	    s_irCache[s_dwIndex].Event.চাবি_Event_2.w_Repeat_গুনতি--;
	    *lpBuffer = s_irPseudo;
	    *lpEvents = 1;
	    ঘূৰাই TRUE;
	}
    }

    *lpBuffer = s_irCache[s_dwIndex];
    যদি (!(nLength == -1 || nLength == -2) && ++s_dwIndex >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    ঘূৰাই TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    স্থির BOOL
peek_console_প্ৰবেশ(
    HANDLE	    h_প্ৰবেশ,
    প্ৰবেশ_RECORD    *lpBuffer,
    DWORD	    nLength UNUSED,
    LPDWORD	    lpEvents)
{
    ঘূৰাই read_console_প্ৰবেশ(h_প্ৰবেশ, lpBuffer, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    স্থির DWORD
বাৰ্তা_wait_for_multiple_objects(
    DWORD    n_গুনতি,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dwWakeMask)
{
    যদি (read_console_প্ৰবেশ(রিক্ত, রিক্ত, -2, রিক্ত))
	ঘূৰাই WAIT_OBJECT_0;
    ঘূৰাই বাৰ্তা_Wait_For_Multiple_Objects(n_গুনতি, pHandles, fWaitAll,
				     dwMilliseconds, dwWakeMask);
}
# endif

# ifndef FEAT_CLIENTSERVER
    স্থির DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    যদি (read_console_প্ৰবেশ(রিক্ত, রিক্ত, -2, রিক্ত))
	ঘূৰাই WAIT_OBJECT_0;
    ঘূৰাই WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#endif   // !FEAT_GUI_MSWIN || VIMDLL

    শূন্য
mch_get_exe_name(শূন্য)
{
    পূর্ণাঙ্ক		updated = FALSE;
    স্থির পূর্ণাঙ্ক	enc_prev = -1;
    WCHAR	*path;
    size_t	pathlen;

    যদি (exe_name == রিক্ত || exe_pathw == রিক্ত || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_PATH];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(রিক্ত, buf, MAX_PATH);
	যদি (p > buf)
	{
	    যদি (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_to_enc(buf, রিক্ত);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    কৰা
	    {
		--p;
		যদি (*p == L'\\')
		{
		    *p = L'\0';
		    ভঙ্গ;
		}
	    } যেতিয়ালৈ (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    যদি (exe_pathw == রিক্ত || !updated)
	ঘূৰাই;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    path = _wgetenv(L"PATH");
    pathlen = (path == রিক্ত) ? 0 : wcslen(path);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#define EXTRA_LEN 7
#define MAX_ENV_STRING_LEN 8191

    যদি (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   temp[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	যদি (pathlen == 0)
	    wcscpy(temp + templen, exe_pathw);
	অন্যথা
	{
	    wcscpy(temp + templen, path);

	    // Check if exe_path is already included in $PATH.
	    যদি (wcsstr(temp, exe_pathw) == রিক্ত)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		যদি (temp[templen - 1] != L';')
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
    স্থির শূন্য
unescape_shellxquote(char_u *p, char_u *escaped)
{
    পূর্ণাঙ্ক	    l = (পূর্ণাঙ্ক)STRLEN(p);
    পূর্ণাঙ্ক	    n;

    যেতিয়ালৈ (*p != NUL)
    {
	যদি (*p == '^' && vim_strchr(escaped, p[1]) != রিক্ত)
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
vimLoadLib(ধ্রুৱ ৱর্ণ *name)
{
    HINSTANCE	dll = রিক্ত;

    // No need to load any library when registering OLE.
    যদি (found_register_arg)
	ঘূৰাই রিক্ত;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    যদি (exe_pathw == রিক্ত)
    {
	mch_get_exe_name();
	যদি (exe_pathw == রিক্ত)
	    ঘূৰাই রিক্ত;
    }

    WCHAR old_dirw[MAXPATHL];

    যদি (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	ঘূৰাই রিক্ত;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    SetCurrentDirectoryW(exe_pathw);
    dll = LoadLibrary(name);
    SetCurrentDirectoryW(old_dirw);
    ঘূৰাই dll;
}

#যদি defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    পূর্ণাঙ্ক
mch_is_gui_executable(শূন্য)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(রিক্ত);
    PIMAGE_DOS_HEADER	pDOS = (PIMAGE_DOS_HEADER)pImage;
    PIMAGE_NT_HEADERS	pPE;

    যদি (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ঘূৰাই FALSE;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    যদি (pPE->Signature != IMAGE_NT_SIGNATURE)
	ঘূৰাই FALSE;
    যদি (pPE->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}
#endif

#যদি defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    স্থির শূন্য *
get_imported_func_info(HINSTANCE hInst, ধ্রুৱ ৱর্ণ *funcname, পূর্ণাঙ্ক info,
	ধ্রুৱ শূন্য *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_HEADER		pDOS = (PIMAGE_DOS_HEADER)hInst;
    PIMAGE_NT_HEADERS		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_DATA		pIAT;	    // Import Address Table
    PIMAGE_THUNK_DATA		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    যদি (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ঘূৰাই রিক্ত;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    যদি (pPE->Signature != IMAGE_NT_SIGNATURE)
	ঘূৰাই রিক্ত;

    ImpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    যদি (ImpVA == 0)
	ঘূৰাই রিক্ত;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    বাবে (; pImpDesc->FirstThunk; ++pImpDesc)
    {
	যদি (!pImpDesc->OriginalFirstThunk)
	    অনুৱর্ত;
	pIAT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->FirstThunk);
	pINT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->OriginalFirstThunk);
	বাবে (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    যদি (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		অনুৱর্ত;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.AddressOfData));
	    যদি (strcmp((ৱর্ণ *)pImpName->Name, funcname) == 0)
	    {
		শূন্য *original;
		DWORD old, new = PAGE_READWRITE;

		ৱিকল্প (info)
		{
		    পক্ষ 0:
			ঘূৰাই (শূন্য *)pIAT->u1.Function;
		    পক্ষ 1:
			ঘূৰাই (শূন্য *)(pImage + pImpDesc->Name);
		    পক্ষ 2:
			original = (শূন্য *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, পরিমাণ(শূন্য *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, পরিমাণ(শূন্য *),
				old, &new);
			ঘূৰাই original;
		    স্ৱভাৱ:
			ঘূৰাই রিক্ত;
		}
	    }
	}
    }
    ঘূৰাই রিক্ত;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
find_imported_module_by_funcname(HINSTANCE hInst, ধ্রুৱ ৱর্ণ *funcname)
{
    ৱর্ণ    *modulename;

    modulename = (ৱর্ণ *)get_imported_func_info(hInst, funcname, 1, রিক্ত);
    যদি (modulename != রিক্ত)
	ঘূৰাই GetModuleHandleA(modulename);
    ঘূৰাই রিক্ত;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    শূন্য *
get_dll_import_func(HINSTANCE hInst, ধ্রুৱ ৱর্ণ *funcname)
{
    ঘূৰাই get_imported_func_info(hInst, funcname, 0, রিক্ত);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    শূন্য *
hook_dll_import_func(HINSTANCE hInst, ধ্রুৱ ৱর্ণ *funcname, ধ্রুৱ শূন্য *hook)
{
    ঘূৰাই get_imported_func_info(hInst, funcname, 2, hook);
}
#endif

#যদি defined(FEAT_PYTHON3)
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

    যদি (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ঘূৰাই রিক্ত;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    যদি (pPE->Signature != IMAGE_NT_SIGNATURE)
	ঘূৰাই রিক্ত;

    ExpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    ExpSize = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    যদি (ExpVA == 0)
	ঘূৰাই hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    pFunctionTable = (LPDWORD)(pImage + pExpDir->AddressOfFunctions);

    যদি (pExpDir->সংখ্যা_Of_Names == 0)
	ঘূৰাই hInst;	// No export names.

    // Check only the first entry.
    যদি ((pFunctionTable[0] < ExpVA) || (pFunctionTable[0] >= ExpVA + ExpSize))
	// The first entry is not a function forwarder.
	ঘূৰাই hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    ধ্রুৱ ৱর্ণ *name = (ধ্রুৱ ৱর্ণ *)(pImage + pFunctionTable[0]);
    ধ্রুৱ ৱর্ণ *p = strchr(name, '.');
    যদি (p == রিক্ত)
	ঘূৰাই hInst;

    // Extract DllName.
    ৱর্ণ buf[MAX_PATH];
    যদি (p - name + 1 > পরিমাণ(buf))
	ঘূৰাই রিক্ত;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    ঘূৰাই GetModuleHandleA(buf);
}
#endif

#যদি defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
স্থির ৱর্ণ *null_libintl_gettext(ধ্রুৱ ৱর্ণ *);
স্থির ৱর্ণ *null_libintl_ngettext(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *, unsigned long n);
স্থির ৱর্ণ *null_libintl_textdomain(ধ্রুৱ ৱর্ণ *);
স্থির ৱর্ণ *null_libintl_bindtextdomain(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *);
স্থির ৱর্ণ *null_libintl_bind_textdomain_codeset(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *);
স্থির পূর্ণাঙ্ক null_libintl_wputenv(ধ্রুৱ wchar_t *);

স্থির HINSTANCE hLibintlDLL = রিক্ত;
ৱর্ণ *(*dyn_libintl_gettext)(ধ্রুৱ ৱর্ণ *) = null_libintl_gettext;
ৱর্ণ *(*dyn_libintl_ngettext)(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *, unsigned long n)
						= null_libintl_ngettext;
ৱর্ণ *(*dyn_libintl_textdomain)(ধ্রুৱ ৱর্ণ *) = null_libintl_textdomain;
ৱর্ণ *(*dyn_libintl_bindtextdomain)(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *)
						= null_libintl_bindtextdomain;
ৱর্ণ *(*dyn_libintl_bind_textdomain_codeset)(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *)
				       = null_libintl_bind_textdomain_codeset;
পূর্ণাঙ্ক (*dyn_libintl_wputenv)(ধ্রুৱ wchar_t *) = null_libintl_wputenv;
পূর্ণাঙ্ক *dyn_libintl_nl_বাৰ্তা_cat_cntr = রিক্ত;

    পূর্ণাঙ্ক
dyn_libintl_init(শূন্য)
{
    পূর্ণাঙ্ক i;
    স্থির সংরচনা
    {
	ৱর্ণ	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{রিক্ত, রিক্ত}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    যদি (hLibintlDLL != রিক্ত)
	ঘূৰাই 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vimLoadLib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    যদি (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    যদি (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT2);
# endif
    যদি (!hLibintlDLL)
    {
	যদি (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_load_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	ঘূৰাই 0;
    }
    বাবে (i = 0; libintl_entry[i].name != রিক্ত
					 && libintl_entry[i].ptr != রিক্ত; ++i)
    {
	যদি ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == রিক্ত)
	{
	    dyn_libintl_end();
	    যদি (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_load_library_function_str), libintl_entry[i].name);
		verbose_leave();
	    }
	    ঘূৰাই 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (ৱর্ণ *(*)(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *))
			GetProcAddress(hLibintlDLL, "bind_textdomain_codeset");
    যদি (dyn_libintl_bind_textdomain_codeset == রিক্ত)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = find_imported_module_by_funcname(hLibintlDLL, "getenv");
    যদি (hmsvcrt != রিক্ত)
	dyn_libintl_wputenv = (পূর্ণাঙ্ক (*)(ধ্রুৱ wchar_t *))
					GetProcAddress(hmsvcrt, "_wputenv");
    যদি (dyn_libintl_wputenv == রিক্ত || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_বাৰ্তা_cat_cntr = (পূর্ণাঙ্ক *)GetProcAddress(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    ঘূৰাই 1;
}

    শূন্য
dyn_libintl_end(শূন্য)
{
    যদি (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= রিক্ত;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_বাৰ্তা_cat_cntr	= রিক্ত;
}

    স্থির ৱর্ণ *
null_libintl_gettext(ধ্রুৱ ৱর্ণ *msgid)
{
    ঘূৰাই (ৱর্ণ*)msgid;
}

    স্থির ৱর্ণ *
null_libintl_ngettext(
	ধ্রুৱ ৱর্ণ *msgid,
	ধ্রুৱ ৱর্ণ *msgid_plural,
	unsigned long n)
{
    ঘূৰাই (ৱর্ণ *)(n == 1 ? msgid : msgid_plural);
}

    স্থির ৱর্ণ *
null_libintl_bindtextdomain(
	ধ্রুৱ ৱর্ণ *domainname UNUSED,
	ধ্রুৱ ৱর্ণ *dirname UNUSED)
{
    ঘূৰাই রিক্ত;
}

    স্থির ৱর্ণ *
null_libintl_bind_textdomain_codeset(
	ধ্রুৱ ৱর্ণ *domainname UNUSED,
	ধ্রুৱ ৱর্ণ *codeset UNUSED)
{
    ঘূৰাই রিক্ত;
}

    স্থির ৱর্ণ *
null_libintl_textdomain(ধ্রুৱ ৱর্ণ *domainname UNUSED)
{
    ঘূৰাই রিক্ত;
}

    স্থির পূর্ণাঙ্ক
null_libintl_wputenv(ধ্রুৱ wchar_t *envstring UNUSED)
{
    ঘূৰাই 0;
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
    স্থির BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    যদি (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	ঘূৰাই FALSE;

    যদি (!Lookup_Privilege_মান(রিক্ত, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	ঘূৰাই FALSE;
    }

    tokenPrivileges.Privilege_গুনতি	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    যদি (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, রিক্ত, রিক্ত))
    {
	CloseHandle(hToken);
	ঘূৰাই FALSE;
    }

    যদি (GetLastError() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	ঘূৰাই FALSE;
    }

    CloseHandle(hToken);
    ঘূৰাই TRUE;
}
#endif

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    শূন্য
PlatformId(শূন্য)
{
#ifdef FEAT_EVAL
    DWORD major = (win_version >> 24) & 0xFF;
    DWORD minor = (win_version >> 16) & 0xFF;
    vim_snprintf(windowsVersion, পরিমাণ(windowsVersion), "%d.%d", major, minor);
#endif

#ifdef HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    যদি (!win32_enable_privilege(SE_SECURITY_NAME))
	ঘূৰাই;
#endif
}

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SHIFT  (SHIFT_PRESSED)
# define CTRL   (ডান_CTRL_PRESSED | বাম_CTRL_PRESSED)
# define ALT    (ডান_ALT_PRESSED  | বাম_ALT_PRESSED)
# define ALT_GR (ডান_ALT_PRESSED  | বাম_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

স্থির ধ্রুৱ সংরচনা
{
    WORD    w_Virt_চাবি;
    BOOL    f_Ansi_চাবি;
    পূর্ণাঙ্ক	    chAlone;
    পূর্ণাঙ্ক	    chShift;
    পূর্ণাঙ্ক	    chCtrl;
    পূর্ণাঙ্ক	    chAlt;
} Virt_চাবি_Map[] =
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
    { VK_বাম,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_ডান,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_END,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_DOWN,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_NEXT,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_ঢোকাও,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_BACK,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# যদি 0
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
    স্থির পূর্ণাঙ্ক
win32_kbd_patch_চাবি(
    চাবি_EVENT_RECORD *pker)
{
    UINT uMods = pker->dw_Control_চাবি_State;
    স্থির পূর্ণাঙ্ক s_iIsDead = 0;
    স্থির WORD awAnsiCode[2];
    স্থির BYTE abKeystate[256];

    যদি (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[1];
	s_iIsDead = 0;
	ঘূৰাই 1;
    }

    // check if it already has a valid unicode character.
    যদি (pker->uChar.UnicodeChar != 0)
	ঘূৰাই 1;

    CLEAR_FIELD(abKeystate);

    // Clear any pending dead keys
    ToUnicode(VK_SPACE, Map_Virtual_চাবি(VK_SPACE, 0), abKeystate, awAnsiCode, 2, 0);

    যদি (uMods & SHIFT_PRESSED)
	abKeystate[VK_SHIFT] = 0x80;
    যদি (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    যদি ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ToUnicode(pker->w_Virtual_চাবি_Code, pker->wVirtualScanCode,
			abKeystate, awAnsiCode, 2, 0);

    যদি (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[0];

    ঘূৰাই s_iIsDead;
}

স্থির BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    স্থির BOOL
decode_চাবি_event(
    চাবি_EVENT_RECORD	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    পূর্ণাঙ্ক			*pmodifiers,
    BOOL		fDoPost UNUSED)
{
    পূর্ণাঙ্ক i;
    ধ্রুৱ পূর্ণাঙ্ক nModifs = pker->dw_Control_চাবি_State & (SHIFT | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    যদি (!pker->b_চাবি_Down)
	ঘূৰাই FALSE;

    // ignore some keystrokes
    ৱিকল্প (pker->w_Virtual_চাবি_Code)
    {
    // modifiers
    পক্ষ VK_SHIFT:
    পক্ষ VK_CONTROL:
    পক্ষ VK_MENU:   // Alt key
	ঘূৰাই FALSE;

    স্ৱভাৱ:
	ভঙ্গ;
    }

    // Shift-TAB
    যদি (pker->w_Virtual_চাবি_Code == VK_TAB && (nModifs & SHIFT_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	ঘূৰাই TRUE;
    }

    বাবে (i = ARRAY_LENGTH(Virt_চাবি_Map); --i >= 0; )
    {
	যদি (Virt_চাবি_Map[i].w_Virt_চাবি == pker->w_Virtual_চাবি_Code)
	{
	    *pch = Virt_চাবি_Map[i].chAlone;
	    যদি ((nModifs & SHIFT) != 0)
		*pch = Virt_চাবি_Map[i].chShift;
	    অন্যথা যদি ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_চাবি_Map[i].chCtrl;
	    অন্যথা যদি ((nModifs & ALT) != 0)
		*pch = Virt_চাবি_Map[i].chAlt;

	    যদি (*pch != 0)
	    {
		যদি (Virt_চাবি_Map[i].f_Ansi_চাবি)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    যদি (pmodifiers)
		    {
			যদি (pker->w_Virtual_চাবি_Code >= VK_F1
			    && pker->w_Virtual_চাবি_Code <= VK_F12)
			{
			    যদি ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_MASK_ALT;
				যদি ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_চাবি_Map[i].chAlone;
			    }
			    যদি ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				যদি ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_চাবি_Map[i].chAlone;
			    }
			}
			অন্যথা যদি (pker->w_Virtual_চাবি_Code >= VK_END
				&& pker->w_Virtual_চাবি_Code <= VK_DOWN)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_চাবি_Map[i].chAlone;
			    যদি ((nModifs & SHIFT) != 0
						    && (nModifs & ~SHIFT) == 0)
			    {
				*pch2 = Virt_চাবি_Map[i].chShift;
			    }
			    যদি ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_চাবি_Map[i].chCtrl;
				যদি (pker->w_Virtual_চাবি_Code == VK_UP
				    || pker->w_Virtual_চাবি_Code == VK_DOWN)
				{
				    *pmodifiers |= MOD_MASK_CTRL;
				    *pch2 = Virt_চাবি_Map[i].chAlone;
				}
			    }
			    যদি ((nModifs & SHIFT) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				*pch2 = Virt_চাবি_Map[i].chShift;
			    }
			    যদি ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_চাবি_Map[i].chAlt;
				*pmodifiers |= MOD_MASK_ALT;
				যদি ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_চাবি_Map[i].chAlone;
				}
				অন্যথা যদি ((nModifs & SHIFT) != 0)
				{
				    *pch2 = Virt_চাবি_Map[i].chShift;
				}
				অন্যথা যদি ((nModifs & CTRL) != 0)
				{
				    যদি (pker->w_Virtual_চাবি_Code == VK_UP
					|| pker->w_Virtual_চাবি_Code == VK_DOWN)
				    {
					*pmodifiers |= MOD_MASK_CTRL;
					*pch2 = Virt_চাবি_Map[i].chAlone;
				    }
				    অন্যথা
				    {
					*pch2 = Virt_চাবি_Map[i].chCtrl;
				    }
				}
			    }
			}
			অন্যথা যদি (pker->w_Virtual_চাবি_Code == VK_ঢোকাও
					&& (nModifs & SHIFT) != 0
					&& (nModifs & ~SHIFT) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_চাবি_Map[i].chShift;
			}
			অন্যথা
			{
			    *pch2 = Virt_চাবি_Map[i].chAlone;
			    যদি ((nModifs & SHIFT) != 0)
				*pmodifiers |= MOD_MASK_SHIFT;
			    যদি ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_MASK_CTRL;
			    যদি ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_MASK_ALT;
			}
		    }
		}

		ঘূৰাই TRUE;
	    }
	}
    }

    i = win32_kbd_patch_চাবি(pker);

    যদি (i < 0)
	*pch = NUL;
    অন্যথা
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	যদি (pmodifiers != রিক্ত)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    যদি ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_MASK_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    যদি ((nModifs & SHIFT) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_MASK_SHIFT;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    যদি ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_MASK_CTRL;
	}
    }

    ঘূৰাই (*pch != NUL);
}

# যদি defined(FEAT_EVAL)
    স্থির পূর্ণাঙ্ক
encode_চাবি_event(dict_T *args, প্ৰবেশ_RECORD *ir)
{
    স্থির পূর্ণাঙ্ক s_dwMods = 0;

    char_u *action = dict_get_string(args, "event", TRUE);
    যদি (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_চাবি_Down = STRICMP(action, "keydown") == 0;
	WORD vkCode = dict_get_সংখ্যা_def(args, "keycode", 0);
	যদি (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    ঘূৰাই FALSE;
	}

	ir->EventType = চাবি_EVENT;
	চাবি_EVENT_RECORD ker;
	ZeroMemory(&ker, পরিমাণ(ker));
	ker.b_চাবি_Down = is_চাবি_Down;
	ker.w_Repeat_গুনতি = 1;
	ker.wVirtualScanCode = 0;
	ker.dw_Control_চাবি_State = 0;
	পূর্ণাঙ্ক mods = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	যদি (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    যদি (mods & MOD_MASK_SHIFT)
		ker.dw_Control_চাবি_State |= SHIFT_PRESSED;
	    যদি (mods & MOD_MASK_CTRL)
		ker.dw_Control_চাবি_State |= বাম_CTRL_PRESSED;
	    যদি (mods & MOD_MASK_ALT)
		ker.dw_Control_চাবি_State |= বাম_ALT_PRESSED;
	}

	যদি (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT)
	{
	    যদি (is_চাবি_Down)
		s_dwMods |= SHIFT_PRESSED;
	    অন্যথা
		s_dwMods &= ~SHIFT_PRESSED;
	}
	অন্যথা যদি (vkCode == VK_LCONTROL || vkCode == VK_CONTROL)
	{
	    যদি (is_চাবি_Down)
		s_dwMods |= বাম_CTRL_PRESSED;
	    অন্যথা
		s_dwMods &= ~বাম_CTRL_PRESSED;
	}
	অন্যথা যদি (vkCode == VK_RCONTROL)
	{
	    যদি (is_চাবি_Down)
		s_dwMods |= ডান_CTRL_PRESSED;
	    অন্যথা
		s_dwMods &= ~ডান_CTRL_PRESSED;
	}
	অন্যথা যদি (vkCode == VK_LMENU || vkCode == VK_MENU)
	{
	    যদি (is_চাবি_Down)
		s_dwMods |= বাম_ALT_PRESSED;
	    অন্যথা
		s_dwMods &= ~বাম_ALT_PRESSED;
	}
	অন্যথা যদি (vkCode == VK_RMENU)
	{
	    যদি (is_চাবি_Down)
		s_dwMods |= ডান_ALT_PRESSED;
	    অন্যথা
		s_dwMods &= ~ডান_ALT_PRESSED;
	}
	ker.dw_Control_চাবি_State |= s_dwMods;
	ker.w_Virtual_চাবি_Code = vkCode;
	ker.uChar.UnicodeChar = 0;
	ir->Event.চাবি_Event_2 = ker;
	vim_free(action);
    }
    অন্যথা
    {
	যদি (action == রিক্ত)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	অন্যথা
	{
	    semsg(_(e_invalid_মান_for_argument_str_str), "event", action);
	    vim_free(action);
	}
	ঘূৰাই FALSE;
    }
    ঘূৰাই TRUE;
}
# endif  // FEAT_EVAL
#endif // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
স্থির পূর্ণাঙ্ক g_fMouseAvail = FALSE;   // mouse present
স্থির পূর্ণাঙ্ক g_fMouseActive = FALSE;  // mouse enabled
স্থির পূর্ণাঙ্ক g_nMouseClick = -1;	    // mouse status
স্থির পূর্ণাঙ্ক g_xMouse;		    // mouse x coordinate
স্থির পূর্ণাঙ্ক g_yMouse;		    // mouse y coordinate
স্থির DWORD g_cmodein = 0;	    // Original console input mode
স্থির DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    শূন্য
mch_setmouse(পূর্ণাঙ্ক on)
{
    DWORD cmodein;

# ifdef VIMDLL
    যদি (gui.in_use)
	ঘূৰাই;
# endif
    যদি (!g_fMouseAvail)
	ঘূৰাই;

    g_fMouseActive = on;
    GetConsoleMode(g_hConIn, &cmodein);

    যদি (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_প্ৰবেশ;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    অন্যথা
    {
	cmodein &= ~ENABLE_MOUSE_প্ৰবেশ;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }

    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
}


# যদি defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    শূন্য
mch_bevalterm_changed(শূন্য)
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
    স্থির শূন্য
decode_mouse_wheel(MOUSE_EVENT_RECORD *pmer)
{
    পূর্ণাঙ্ক	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    পূর্ণাঙ্ক	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    পূর্ণাঙ্ক lcol = g_xMouse;
    পূর্ণাঙ্ক lrow = g_yMouse;
    win_T *wp = mouse_find_win(&lrow, &lcol, FIND_POPUP);
    যদি (wp != রিক্ত && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_FIELD(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	যদি (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_বাম : MSCR_ডান;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	অন্যথা
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
    যদি (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    অন্যথা
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    যদি (pmer->dw_Control_চাবি_State & SHIFT_PRESSED)
	modifiers |= MOD_MASK_SHIFT; // MOUSE_SHIFT;
    যদি (pmer->dw_Control_চাবি_State & (ডান_CTRL_PRESSED | বাম_CTRL_PRESSED))
	modifiers |= MOD_MASK_CTRL; // MOUSE_CTRL;
    যদি (pmer->dw_Control_চাবি_State & (ডান_ALT_PRESSED  | বাম_ALT_PRESSED))
	modifiers |= MOD_MASK_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    ঘূৰাই;
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
    স্থির BOOL
decode_mouse_event(
    MOUSE_EVENT_RECORD *pmer)
{
    স্থির পূর্ণাঙ্ক s_nOldButton = -1;
    স্থির পূর্ণাঙ্ক s_nOldMouseClick = -1;
    স্থির পূর্ণাঙ্ক s_xOldMouse = -1;
    স্থির পূর্ণাঙ্ক s_yOldMouse = -1;
    স্থির linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    স্থির পূর্ণাঙ্ক s_old_topfill = 0;
# endif
    স্থির পূর্ণাঙ্ক s_cClicks = 1;
    স্থির BOOL s_fReleased = TRUE;
    স্থির DWORD s_dwLastClickTime = 0;
    স্থির BOOL s_f_Next_Is_মধ্য = FALSE;

    স্থির DWORD cButtons = 0;	// number of buttons supported

    ধ্রুৱ DWORD বাম_2 = FROM_বাম_1_ST_BUTTON_PRESSED;
    ধ্রুৱ DWORD মধ্য = FROM_বাম_2_ND_BUTTON_PRESSED;
    ধ্রুৱ DWORD ডান_2 = RIGHTMOST_BUTTON_PRESSED;
    ধ্রুৱ DWORD বাম_ডান = বাম_2 | ডান_2;

    পূর্ণাঙ্ক nButton;

    যদি (cButtons == 0 && !Get_সংখ্যা_Of_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    যদি (!g_fMouseAvail || !g_fMouseActive)
    {
	g_nMouseClick = -1;
	ঘূৰাই FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    যদি (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	ঘূৰাই FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    যদি (g_nMouseClick != -1)
	ঘূৰাই TRUE;

    যদি (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	ঘূৰাই TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    যদি (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	যদি (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    ঘূৰাই FALSE;
    }

    // If no buttons are pressed...
    যদি ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	যদি (s_fReleased)
	{
# ifdef FEAT_BEVAL_TERM
	    // do return mouse move events when we want them
	    যদি (p_bevalterm)
		nButton = MOUSE_DRAG;
	    অন্যথা
# endif
		ঘূৰাই FALSE;
	}

	s_fReleased = TRUE;
    }
    অন্যথা    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	যদি (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & বাম_ডান);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    যদি (dwLR == বাম_2 || dwLR == ডান_2)
	    {
		বাবে (;;)
		{
		    // wait a short time for next input event
		    যদি (WaitForSingleObject(g_hConIn, p_mouset / 3)
							     != WAIT_OBJECT_0)
			ভঙ্গ;
		    অন্যথা
		    {
			DWORD cRecords = 0;
			প্ৰবেশ_RECORD ir;
			MOUSE_EVENT_RECORD* pmer2 = &ir.Event.MouseEvent;

			peek_console_প্ৰবেশ(g_hConIn, &ir, 1, &cRecords);

			যদি (cRecords == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & বাম_ডান))
			    ভঙ্গ;
			অন্যথা
			{
			    যদি (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_প্ৰবেশ(g_hConIn, &ir, 1, &cRecords);

				ঘূৰাই decode_mouse_event(pmer2);
			    }
			    অন্যথা যদি (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_প্ৰবেশ(g_hConIn, &ir, 1, &cRecords);

				// are there any more mouse events in queue?
				peek_console_প্ৰবেশ(g_hConIn, &ir, 1, &cRecords);

				যদি (cRecords==0 || ir.EventType != MOUSE_EVENT)
				    ভঙ্গ;
			    }
			    অন্যথা
				ভঙ্গ;
			}
		    }
		}
	    }
	}

	যদি (s_f_Next_Is_মধ্য)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_মধ্য;
	    s_f_Next_Is_মধ্য = FALSE;
	}
	অন্যথা যদি (cButtons == 2	&&
	    ((pmer->dwButtonState & বাম_ডান) == বাম_ডান))
	{
	    nButton = MOUSE_মধ্য;

	    যদি (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_Next_Is_মধ্য = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	অন্যথা যদি ((pmer->dwButtonState & বাম_2) == বাম_2)
	    nButton = MOUSE_বাম;
	অন্যথা যদি ((pmer->dwButtonState & মধ্য) == মধ্য)
	    nButton = MOUSE_মধ্য;
	অন্যথা যদি ((pmer->dwButtonState & ডান_2) == ডান_2)
	    nButton = MOUSE_ডান;

	যদি (! s_fReleased && ! s_f_Next_Is_মধ্য
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    ঘূৰাই FALSE;

	s_fReleased = s_f_Next_Is_মধ্য;
    }

    যদি (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	যদি (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dwCurrentTime = Get_Tick_গুনতি();

	    যদি (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (পূর্ণাঙ্ক)(dwCurrentTime - s_dwLastClickTime) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    অন্যথা যদি (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dwLastClickTime = dwCurrentTime;
	}
    }
    অন্যথা যদি (pmer->dwEventFlags == MOUSE_MOVED)
    {
	যদি (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    যদি (nButton == -1)
	ঘূৰাই FALSE;

    যদি (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    যদি (pmer->dw_Control_চাবি_State & SHIFT_PRESSED)
	g_nMouseClick |= MOUSE_SHIFT;
    যদি (pmer->dw_Control_চাবি_State & (ডান_CTRL_PRESSED | বাম_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    যদি (pmer->dw_Control_চাবি_State & (ডান_ALT_PRESSED  | বাম_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    যদি (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	SET_অঙ্ক_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    যদি (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	ঘূৰাই FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    ঘূৰাই TRUE;
}

# ifdef FEAT_EVAL
    স্থির পূর্ণাঙ্ক
encode_mouse_event(dict_T *args, প্ৰবেশ_RECORD *ir)
{
    পূর্ণাঙ্ক		button;
    পূর্ণাঙ্ক		row;
    পূর্ণাঙ্ক		col;
    পূর্ণাঙ্ক		repeated_click;
    int_u	mods = 0;
    পূর্ণাঙ্ক		move;

    যদি (!dict_has_চাবি(args, "row") || !dict_has_চাবি(args, "col"))
	ঘূৰাই FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (পূর্ণাঙ্ক)dict_get_bool(args, "move", FALSE);
    যদি (!move && (!dict_has_চাবি(args, "button")
	    || !dict_has_চাবি(args, "multiclick")
	    || !dict_has_চাবি(args, "modifiers")))
	ঘূৰাই FALSE;

    row = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "row") - 1;
    col = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_RECORD mer;
    ZeroMemory(&mer, পরিমাণ(mer));
    mer.dwMousePosition.X  = col;
    mer.dwMousePosition.Y  = row;

    যদি (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    অন্যথা
    {
	button = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "button");
	repeated_click = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "multiclick");
	mods = (পূর্ণাঙ্ক)dict_get_সংখ্যা(args, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	ৱিকল্প (button)
	{
	    পক্ষ MOUSE_বাম:
		mer.dwButtonState = FROM_বাম_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ভঙ্গ;
	    পক্ষ MOUSE_মধ্য:
		mer.dwButtonState = FROM_বাম_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ভঙ্গ;
	    পক্ষ MOUSE_ডান:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ভঙ্গ;
	    পক্ষ MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    পক্ষ MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		ভঙ্গ;
	    পক্ষ MOUSE_X1:
		mer.dwButtonState = FROM_বাম_3_RD_BUTTON_PRESSED;
		ভঙ্গ;
	    পক্ষ MOUSE_X2:
		mer.dwButtonState = FROM_বাম_4_TH_BUTTON_PRESSED;
		ভঙ্গ;
	    পক্ষ MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ভঙ্গ;
	    পক্ষ MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ভঙ্গ;
	    পক্ষ MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ভঙ্গ;
	    পক্ষ MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ভঙ্গ;
	    স্ৱভাৱ:
		semsg(_(e_invalid_argument_str), "button");
		ঘূৰাই FALSE;
	}
    }

    mer.dw_Control_চাবি_State = 0;
    যদি (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	যদি (mods & MOUSE_SHIFT)
	    mer.dw_Control_চাবি_State |= SHIFT_PRESSED;
	যদি (mods & MOUSE_CTRL)
	    mer.dw_Control_চাবি_State |= বাম_CTRL_PRESSED;
	যদি (mods & MOUSE_ALT)
	    mer.dw_Control_চাবি_State |= বাম_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    ঘূৰাই TRUE;
}
# endif  // FEAT_EVAL

    স্থির পূর্ণাঙ্ক
write_প্ৰবেশ_record_buffer(প্ৰবেশ_RECORD* irEvents, পূর্ণাঙ্ক nLength)
{
    পূর্ণাঙ্ক				n_গুনতি = 0;
    প্ৰবেশ_record_buffer_গ্ৰন্থি_T	*event_গ্ৰন্থি;

    যেতিয়ালৈ (n_গুনতি < nLength)
    {
	event_গ্ৰন্থি = alloc(পরিমাণ(প্ৰবেশ_record_buffer_গ্ৰন্থি_T));
	যদি (event_গ্ৰন্থি == রিক্ত)
	    ঘূৰাই -1;

	event_গ্ৰন্থি->ir = irEvents[n_গুনতি++];
	event_গ্ৰন্থি->next = রিক্ত;

	প্ৰবেশ_record_buffer.length++;
	যদি (প্ৰবেশ_record_buffer.tail == রিক্ত)
	{
	    প্ৰবেশ_record_buffer.head = event_গ্ৰন্থি;
	    প্ৰবেশ_record_buffer.tail = event_গ্ৰন্থি;
	}
	অন্যথা
	{
	    প্ৰবেশ_record_buffer.tail->next = event_গ্ৰন্থি;
	    প্ৰবেশ_record_buffer.tail = প্ৰবেশ_record_buffer.tail->next;
	}
    }
    ঘূৰাই n_গুনতি;
}

    স্থির পূর্ণাঙ্ক
read_প্ৰবেশ_record_buffer(প্ৰবেশ_RECORD* irEvents, পূর্ণাঙ্ক nMaxLength)
{
    পূর্ণাঙ্ক n_গুনতি = 0;
    যেতিয়ালৈ (n_গুনতি < nMaxLength && প্ৰবেশ_record_buffer.head != রিক্ত)
    {
	প্ৰবেশ_record_buffer.length--;
	প্ৰবেশ_record_buffer_গ্ৰন্থি_T *বেৰ_কৰো_head = প্ৰবেশ_record_buffer.head;
	irEvents[n_গুনতি++] = বেৰ_কৰো_head->ir;
	প্ৰবেশ_record_buffer.head = বেৰ_কৰো_head->next;
	vim_free(বেৰ_কৰো_head);
	যদি (প্ৰবেশ_record_buffer.length == 0)
	    প্ৰবেশ_record_buffer.tail = রিক্ত;
    }
    ঘূৰাই n_গুনতি;
}
#অন্যথা  // FEAT_GUI_MSWIN && !VIMDLL
    শূন্য
mch_setmouse(পূর্ণাঙ্ক on UNUSED)
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
    পূর্ণাঙ্ক
test_mswin_event(char_u *event, dict_T *args)
{
    পূর্ণাঙ্ক lpEventsWritten = 0;

# যদি defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    যদি (gui.in_use)
	ঘূৰাই test_gui_w32_sendevent(event, args);
# endif

# যদি defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    প্ৰবেশ_RECORD ir;
    BOOL প্ৰবেশ_encoded = FALSE;
    BOOL execute = FALSE;
    যদি (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(args, "execute", FALSE);
	যদি (dict_has_চাবি(args, "event"))
	    প্ৰবেশ_encoded = encode_চাবি_event(args, &ir);
	অন্যথা যদি (!execute)
	{
	    semsg(_(e_missing_argument_str), "event");
	    ঘূৰাই FALSE;
	}
    }
    অন্যথা যদি (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	প্ৰবেশ_encoded = encode_mouse_event(args, &ir);
    }
    অন্যথা
    {
	semsg(_(e_invalid_মান_for_argument_str_str), "event", event);
	ঘূৰাই FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    যদি (প্ৰবেশ_encoded)
    {
	যদি ((lpEventsWritten = write_প্ৰবেশ_record_buffer(&ir, 1)) < 0)
	{
	    semsg(_(e_out_of_memory), "event");
	    ঘূৰাই FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    যদি (execute)
    {
	পূর্ণাঙ্ক save_বাৰ্তা_scroll = বাৰ্তা_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	বাৰ্তা_scroll = FALSE;
	ch_log(রিক্ত, "test_mswin_event() executing");
	exec_normal(TRUE, TRUE, TRUE);
	বাৰ্তা_scroll |= save_বাৰ্তা_scroll;
    }

# endif
    ঘূৰাই lpEventsWritten;
}
#endif // FEAT_EVAL

#যদি defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    স্থির শূন্য
mch_set_cursor_shape(পূর্ণাঙ্ক thickness)
{
    যদি (vtp_working)
    {
	যদি (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    যদি (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    অন্যথা
		vtp_printf("\033[0 q");	// default
	}
    }
    অন্যথা
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dwSize = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	SetConsoleCursorInfo(g_hConOut, &ConsoleCursorInfo);
	যদি (s_cursor_visible)
	    SetConsoleCursorPosition(g_hConOut, g_coord);
    }
}

    শূন্য
mch_update_cursor(শূন্য)
{
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		thickness;

# ifdef VIMDLL
    যদি (gui.in_use)
	ঘূৰাই;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    যদি (shape_table[idx].shape == SHAPE_BLOCK)
	thickness = 99;	// 100 doesn't work on W95
    অন্যথা
	thickness = shape_table[idx].percentage;
    mch_set_cursor_shape(thickness);
}
#endif

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    স্থির শূন্য
handle_focus_event(প্ৰবেশ_RECORD ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
    ui_focus_change((পূর্ণাঙ্ক)g_fJustGotFocus);
}

স্থির শূন্য ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    স্থির পূর্ণাঙ্ক
WaitForChar(long msec, পূর্ণাঙ্ক ignore_প্ৰবেশ)
{
    DWORD	    dwNow = 0, dwEndTime = 0;
    প্ৰবেশ_RECORD    ir;
    DWORD	    cRecords;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    পূর্ণাঙ্ক		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    যদি (msec > 0)
	// Wait until the specified time has elapsed.
	dwEndTime = Get_Tick_গুনতি() + msec;
    অন্যথা যদি (msec < 0)
	// Wait forever.
	dwEndTime = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    বাবে (;;)
    {
	// Only process messages when waiting.
	যদি (msec != 0)
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

	যদি (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_প্ৰবেশ && প্ৰবেশ_available())
# endif
	   )
	    ঘূৰাই TRUE;

	যদি (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_গুনতি();
	    যদি ((পূর্ণাঙ্ক)(dwNow - dwEndTime) >= 0)
		ভঙ্গ;
	}
	যদি (msec != 0)
	{
	    DWORD dwWaitTime = dwEndTime - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    যদি (dwWaitTime > 11)
		dwWaitTime = 11;

# ifdef FEAT_MZSCHEME
	    যদি (mzthreads_allowed() && p_mzq > 0 && (long)dwWaitTime > p_mzq)
		dwWaitTime = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    যদি (dwWaitTime > 10)
	    {
		long	due_time;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_time = check_due_timer();
		যদি (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    ঘূৰাই FALSE;
		}
		যদি (due_time > 0 && dwWaitTime > (DWORD)due_time)
		    dwWaitTime = due_time;
	    }
# endif
	    যদি (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    বাৰ্তা_wait_for_multiple_objects(1, &g_hConIn, FALSE,
				  dwWaitTime, QS_SENDMESSAGE) != WAIT_OBJECT_0
# অন্যথা
		    wait_for_single_object(g_hConIn, dwWaitTime)
							      != WAIT_OBJECT_0
# endif
		    )
		অনুৱর্ত;
	}

	cRecords = 0;
	peek_console_প্ৰবেশ(g_hConIn, &ir, 1, &cRecords);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	যদি (State == MODE_CMDLINE && বাৰ্তা_row == Rows - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_BUFFER_INFO csbi;

	    যদি (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	    {
		যদি (csbi.dwCursorPosition.Y != বাৰ্তা_row)
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

	যদি (cRecords > 0)
	{
	    যদি (ir.EventType == চাবি_EVENT && ir.Event.চাবি_Event_2.b_চাবি_Down)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		যদি (ir.Event.চাবি_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.চাবি_Event_2.w_Virtual_চাবি_Code == 13)
		{
		    read_console_প্ৰবেশ(g_hConIn, &ir, 1, &cRecords);
		    অনুৱর্ত;
		}
# endif
		যদি (decode_চাবি_event(&ir.Event.চাবি_Event_2, &ch, &ch2,
								 রিক্ত, FALSE))
		    ঘূৰাই TRUE;
	    }

	    read_console_প্ৰবেশ(g_hConIn, &ir, 1, &cRecords);

	    যদি (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    অন্যথা যদি (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    {
		COORD dwSize = ir.Event.WindowBufferSizeEvent.dwSize;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		যদি (dwSize.X != Columns || dwSize.Y != Rows)
		{
		    CONSOLE_SCREEN_BUFFER_INFO csbi;
		    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
		    dwSize.X = csbi.srWindow.ডান_3 - csbi.srWindow.বাম_3 + 1;
		    dwSize.Y = csbi.srWindow.Bottom - csbi.srWindow.শীৰ্ষ_2 + 1;
		    যদি (dwSize.X != Columns || dwSize.Y != Rows)
		    {
			ResizeConBuf(g_hConOut, dwSize);
			shell_resized();
		    }
		}
	    }
	    অন্যথা যদি (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		ঘূৰাই TRUE;
	}
	অন্যথা যদি (msec == 0)
	    ভঙ্গ;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    যদি (প্ৰবেশ_available())
	ঘূৰাই TRUE;
# endif

    ঘূৰাই FALSE;
}

/*
 * return non-zero if a character is available
 */
    পূর্ণাঙ্ক
mch_char_avail(শূন্য)
{
# ifdef VIMDLL
    যদি (gui.in_use)
	ঘূৰাই TRUE;
# endif
    ঘূৰাই WaitForChar(0L, FALSE);
}

# যদি defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    পূর্ণাঙ্ক
mch_check_messages(শূন্য)
{
#  ifdef VIMDLL
    যদি (gui.in_use)
	ঘূৰাই TRUE;
#  endif
    ঘূৰাই WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    স্থির শূন্য
create_conin(শূন্য)
{
    g_hConIn =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES) রিক্ত,
			OPEN_EXISTING, 0, (HANDLE)রিক্ত);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    স্থির WCHAR
tgetch(পূর্ণাঙ্ক *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    বাবে (;;)
    {
	প্ৰবেশ_RECORD ir;
	DWORD cRecords = 0;

# ifdef FEAT_CLIENTSERVER
	(শূন্য)WaitForChar(-1L, FALSE);
	যদি (প্ৰবেশ_available())
	    ঘূৰাই 0;
	যদি (g_nMouseClick != -1)
	    ঘূৰাই 0;
# endif
	যদি (read_console_প্ৰবেশ(g_hConIn, &ir, 1, &cRecords) == 0)
	{
	    যদি (did_create_conin)
		read_error_exit();
	    create_conin();
	    অনুৱর্ত;
	}

	যদি (ir.EventType == চাবি_EVENT)
	{
	    যদি (decode_চাবি_event(&ir.Event.চাবি_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		ঘূৰাই ch;
	}
	অন্যথা যদি (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	অন্যথা যদি (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    shell_resized();
	অন্যথা যদি (ir.EventType == MOUSE_EVENT)
	{
	    যদি (decode_mouse_event(&ir.Event.MouseEvent))
		ঘূৰাই 0;
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
    পূর্ণাঙ্ক
mch_inchar(
    char_u	*buf UNUSED,
    পূর্ণাঙ্ক		maxlen UNUSED,
    long	time UNUSED,
    পূর্ণাঙ্ক		tb_change_cnt UNUSED)
{
#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# অন্যথা
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    স্থির char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    স্থির পূর্ণাঙ্ক	    typeaheadlen = 0;

# ifdef VIMDLL
    যদি (gui.in_use)
	ঘূৰাই 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    যদি (typeaheadlen > 0)
	গচ্ছ theend;

    যদি (time >= 0)
    {
	যদি (!WaitForChar(time, FALSE))     // no character available
	    ঘূৰাই 0;
    }
    অন্যথা    // time == -1, wait forever
    {
	mch_set_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	যদি (!WaitForChar(p_ut, FALSE))
	{
	    যদি (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (পূর্ণাঙ্ক)KE_CURSORHOLD;
		ঘূৰাই 3;
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
    যদি (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    যেতিয়ালৈ ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	যদি (typebuf_changed(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    ভঙ্গ;
	}
	যদি (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    যদি (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    যদি (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		যদি (modifiers > 0)
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
	    অন্যথা
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
	অন্যথা
	{
	    WCHAR	ch2 = NUL;
	    পূর্ণাঙ্ক		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_চাবি(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_চাবিগুলি;
	    c = may_adjust_চাবি_for_ctrl(modifiers, c);
	    --no_reduce_চাবিগুলি;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_shift_modifier(modifiers, c);

	    যদি (typebuf_changed(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		ভঙ্গ;
	    }

	    যদি (c == Ctrl_C && ctrl_c_interrupts)
	    {
# যদি defined(FEAT_CLIENTSERVER)
		trash_প্ৰবেশ_buf();
# endif
		got_int = TRUE;
	    }

	    যদি (g_nMouseClick == -1)
	    {
		পূর্ণাঙ্ক	n = 1;

		যদি (ch2 == NUL)
		{
		    পূর্ণাঙ্ক	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    যদি (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_to_enc(ch, &n);
		    যদি (p != রিক্ত)
		    {
			বাবে (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    যদি (p[i] == CSI)
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
		অন্যথা
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    যদি (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		যদি (ch2 != NUL)
		{
		    যদি (c == K_NUL)
		    {
			ৱিকল্প (ch2)
			{
			পক্ষ (WCHAR)'\324': // SHIFT+Insert
			পক্ষ (WCHAR)'\325': // CTRL+Insert
			পক্ষ (WCHAR)'\327': // SHIFT+Delete
			পক্ষ (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    ভঙ্গ;

			স্ৱভাৱ:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    ভঙ্গ;
			}
		    }
		    অন্যথা
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
		যদি ((modifiers & MOD_MASK_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_MASK_ALT;
		}

		যদি (modifiers != 0)
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
		যদি (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    যদি (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    যেতিয়ালৈ (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    যদি (len > 0)
    {
	buf[len] = NUL;
	ch_log(রিক্ত, "raw key input: \"%s\"", buf);
    }
# endif
    ঘূৰাই len;

#অন্যথা // FEAT_GUI_MSWIN && !VIMDLL
    ঘূৰাই 0;
#endif // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    স্থির পূর্ণাঙ্ক
executable_file(ৱর্ণ *name, char_u **path)
{
    পূর্ণাঙ্ক attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    যদি (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	ঘূৰাই FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    যদি (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	যদি (res == রিক্ত)
	    res = resolve_reparse_point((char_u *)name);
	যদি (res == রিক্ত)
	    ঘূৰাই FALSE;
	// The path is already absolute.
	যদি (path != রিক্ত)
	    *path = res;
	অন্যথা
	    vim_free(res);
    }
    অন্যথা যদি (path != রিক্ত)
	*path = FullName_save((char_u *)name, FALSE);
    ঘূৰাই TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    স্থির bool
executable_prefix_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_FIND_DATAW	data;
    HANDLE		handle;
    DWORD		error;

    wn = enc_to_utf16(name, রিক্ত);
    যদি (wn == রিক্ত)
	ঘূৰাই true;
    handle = FindFirstFileExW(wn, FindExInfoBasic, &data,
					    Find_Ex_অনুসন্ধান_Name_Match, রিক্ত, 0);
    error = GetLastError();
    vim_free(wn);
    যদি (handle != INVALID_HANDLE_মান)
    {
	FindClose(handle);
	ঘূৰাই true;
    }
    ঘূৰাই error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    স্থির পূর্ণাঙ্ক
executable_exists(
    ৱর্ণ	*name,
    size_t	namelen,
    char_u	**path,
    পূর্ণাঙ্ক		use_path,
    পূর্ণাঙ্ক		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3];
    char_u	*p;
    string_T	pathbuf = {রিক্ত, 0};
    পূর্ণাঙ্ক		pathbuf_allocated = FALSE;
    string_T	pathext = {রিক্ত, 0};
    পূর্ণাঙ্ক		pathext_allocated = FALSE;
    char_u	*shname = রিক্ত;
    পূর্ণাঙ্ক		noext = FALSE;
    bool	simple_pathext = true;
    পূর্ণাঙ্ক		ext_গুনতি = 0;
    পূর্ণাঙ্ক		retval = FALSE;

    যদি (namelen >= পরিমাণ(buf))	// safety check
	ঘূৰাই FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    যদি (strstr((ৱর্ণ *)shname, "sh") != রিক্ত &&
	!(strstr((ৱর্ণ *)shname, "powershell") != রিক্ত
				    || strstr((ৱর্ণ *)shname, "pwsh") != রিক্ত))
	noext = TRUE;

    যদি (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	যদি (pathext.string == রিক্ত)
	    STR_LITERAL_SET(pathext, ".com;.exe;.bat;.cmd");
	অন্যথা
	    pathext.length = STRLEN(pathext.string);

	যদি (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    যেতিয়ালৈ (*p)
	    {
		যদি (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    যদি (*p == '.')
			simple_pathext = false;
		    ++p;
		    অনুৱর্ত;
		}
		e = vim_strchr(p, ';');
		যদি (e == রিক্ত)
		    e = pathext.string + pathext.length;
		plen = (size_t)(e - p);
		++ext_গুনতি;
		যদি (*p != '.')
		    simple_pathext = false;
		বাবে (q = p + 1; q < e; ++q)
		    যদি (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		যদি (_strnicoll(name + namelen - plen, (ৱর্ণ *)p, plen) == 0)
		{
		    noext = TRUE;
		    ভঙ্গ;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    যদি (pathext.string == রিক্ত)
	STR_LITERAL_SET(pathext, ".");
    অন্যথা যদি (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.length + 3);
	যদি (tmp == রিক্ত)
	{
	    retval = FALSE;
	    গচ্ছ theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.length += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    যদি (use_path && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	যদি (p != রিক্ত)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    যদি (pathbuf.string == রিক্ত)
	    {
		retval = FALSE;
		গচ্ছ theend;
	    }

	    যদি (mch_getenv("NoDefaultCurrentDirectoryInExePath") == রিক্ত &&
		    strstr((ৱর্ণ *)gettail(p_sh), "cmd.exe") != রিক্ত)
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
    যদি (pathbuf.string == রিক্ত)
	STR_LITERAL_SET(pathbuf, ".");
    p = pathbuf.string;
    যেতিয়ালৈ (*p)
    {
	char_u	*e;
	size_t	buflen;

	যদি (*p == ';') // Skip empty entry
	{
	    ++p;
	    অনুৱর্ত;
	}
	e = vim_strchr(p, ';');
	যদি (e == রিক্ত)
	    e = pathbuf.string + pathbuf.length;

	যদি (e - p + namelen + 2 > পরিমাণ(buf))
	{
	    retval = FALSE;
	    গচ্ছ theend;
	}
	// A single "." that means current dir.
	যদি (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	অন্যথা
	{
	    buflen = vim_snprintf_safelen(
		(ৱর্ণ *)buf,
		পরিমাণ(buf),
		"%.*s%s%s", (পূর্ণাঙ্ক)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	যদি (!noext && simple_pathext && ext_গুনতি >= 3 && buflen > 0
		&& buflen + 2 <= পরিমাণ(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == রিক্ত
		&& vim_strchr(gettail(buf), ':') == রিক্ত)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_prefix_exists(buf);
	    buf[buflen] = NUL;
	    যদি (!exists)
	    {
		p = e;
		অনুৱর্ত;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	যেতিয়ালৈ (*p)
	{
	    char_u  *e2;

	    যদি (*p == ';')
	    {
		// Skip empty entry
		++p;
		অনুৱর্ত;
	    }
	    e2 = vim_strchr(p, ';');
	    যদি (e2 == রিক্ত)
		e2 = pathext.string + pathext.length;

	    যদি (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		যদি (e2 - p + buflen + 1 > পরিমাণ(buf))
		{
		    retval = FALSE;
		    গচ্ছ theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    যদি (executable_file((ৱর্ণ *)buf, path))
	    {
		retval = TRUE;
		গচ্ছ theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    যদি (pathbuf_allocated)
	মুক্ত(pathbuf.string);
    যদি (pathext_allocated)
	মুক্ত(pathext.string);
    ঘূৰাই retval;
}

#যদি (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    স্থির শূন্য
bad_param_handler(ধ্রুৱ wchar_t *expression UNUSED,
    ধ্রুৱ wchar_t *function UNUSED,
    ধ্রুৱ wchar_t *file UNUSED,
    unsigned পূর্ণাঙ্ক সাৰি UNUSED,
    uintptr_t pReserved UNUSED)
{
}

# define SET_INVALID_PARAM_HANDLER \
	((শূন্য)_set_invalid_parameter_handler(bad_param_handler))
#অন্যথা
# define SET_INVALID_PARAM_HANDLER
#endif

#ifdef FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    স্থির শূন্য
mch_init_g(শূন্য)
{
# ifndef __MINGW32__
    বাহ্য পূর্ণাঙ্ক _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_দ্বিমিক;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    Rows = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[_MAX_PATH + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	যদি (exe_pathlen + 10 >= পরিমাণ(vimrun_location))
	{
	    যদি (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    রিক্ত, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	অন্যথা
	{
	    // First try in same directory as gvim.exe.
	    যদি (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    যদি (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		যদি (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(ৱর্ণ *)vimrun_location,
			পরিমাণ(vimrun_location),
			"\"%.*svimrun\" ",
			(পূর্ণাঙ্ক)exe_pathlen, exe_name);
		}
		অন্যথা
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		যদি (p != রিক্ত)
		{
		    vimrun_path.string = p;
		    vimrun_path.length = plen;
		    vimrun_path_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    অন্যথা যদি (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    রিক্ত, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	যদি (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    যদি (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    রিক্ত, TRUE, FALSE))
	set_option_মান_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#endif // FEAT_GUI_MSWIN

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).ডান_3 - (sr).বাম_3 + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).শীৰ্ষ_2 + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    স্থির BOOL
ClearConsoleBuffer(WORD wAttribute)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    DWORD অঙ্ক_Cells, dummy;

    যদি (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ঘূৰাই FALSE;

    অঙ্ক_Cells = csbi.dwSize.X * csbi.dwSize.Y;
    coord.X = 0;
    coord.Y = 0;
    যদি (!FillConsoleOutputCharacter(g_hConOut, ' ', অঙ্ক_Cells,
	    coord, &dummy))
	ঘূৰাই FALSE;
    যদি (!FillConsoleOutputAttribute(g_hConOut, wAttribute, অঙ্ক_Cells,
	    coord, &dummy))
	ঘূৰাই FALSE;

    ঘূৰাই TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    স্থির BOOL
FitConsoleWindow(
    COORD dwBufferSize,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD dwWindowSize;
    BOOL NeedAdjust = FALSE;

    যদি (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ঘূৰাই FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    যদি (csbi.srWindow.ডান_3 >= dwBufferSize.X)
    {
	dwWindowSize.X = SRWIDTH(csbi.srWindow);
	যদি (dwWindowSize.X > dwBufferSize.X)
	    dwWindowSize.X = dwBufferSize.X;
	csbi.srWindow.ডান_3 = dwBufferSize.X - 1;
	csbi.srWindow.বাম_3 = dwBufferSize.X - dwWindowSize.X;
	NeedAdjust = TRUE;
    }
    যদি (csbi.srWindow.Bottom >= dwBufferSize.Y)
    {
	dwWindowSize.Y = SRHEIGHT(csbi.srWindow);
	যদি (dwWindowSize.Y > dwBufferSize.Y)
	    dwWindowSize.Y = dwBufferSize.Y;
	csbi.srWindow.Bottom = dwBufferSize.Y - 1;
	csbi.srWindow.শীৰ্ষ_2 = dwBufferSize.Y - dwWindowSize.Y;
	NeedAdjust = TRUE;
    }
    যদি (NeedAdjust && WantAdjust)
    {
	যদি (!SetConsoleWindowInfo(g_hConOut, TRUE, &csbi.srWindow))
	    ঘূৰাই FALSE;
    }
    ঘূৰাই TRUE;
}

প্রকারনির্দেশ সংরচনা ConsoleBufferStruct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_BUFFER_INFO	Info;
    PCHAR_INFO			Buffer;
    COORD			BufferSize;
    PSMALL_RECT			Regions;
    পূর্ণাঙ্ক				অঙ্ক_Regions;
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
    স্থির BOOL
SaveConsoleBuffer(
    ConsoleBuffer *cb)
{
    DWORD অঙ্ক_Cells;
    COORD BufferCoord;
    SMALL_RECT ReadRegion;
    WORD Y, Y_incr;
    পূর্ণাঙ্ক i, numregions;

    যদি (cb == রিক্ত)
	ঘূৰাই FALSE;

    যদি (!GetConsoleScreenBufferInfo(g_hConOut, &cb->Info))
    {
	cb->IsValid = FALSE;
	ঘূৰাই FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    যদি (use_alternate_screen_buffer)
	ঘূৰাই TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    যদি (!cb->IsValid || cb->Buffer == রিক্ত ||
	    cb->BufferSize.X != cb->Info.dwSize.X ||
	    cb->BufferSize.Y != cb->Info.dwSize.Y)
    {
	cb->BufferSize.X = cb->Info.dwSize.X;
	cb->BufferSize.Y = cb->Info.dwSize.Y;
	অঙ্ক_Cells = cb->BufferSize.X * cb->BufferSize.Y;
	vim_free(cb->Buffer);
	cb->Buffer = ALLOC_MULT(CHAR_INFO, অঙ্ক_Cells);
	যদি (cb->Buffer == রিক্ত)
	    ঘূৰাই FALSE;
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
    ReadRegion.বাম_3 = 0;
    ReadRegion.ডান_3 = cb->Info.dwSize.X - 1;
    Y_incr = 12000 / cb->Info.dwSize.X;

    numregions = (cb->Info.dwSize.Y + Y_incr - 1) / Y_incr;
    যদি (cb->Regions == রিক্ত || numregions != cb->অঙ্ক_Regions)
    {
	cb->অঙ্ক_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->অঙ্ক_Regions);
	যদি (cb->Regions == রিক্ত)
	{
	    VIM_CLEAR(cb->Buffer);
	    ঘূৰাই FALSE;
	}
    }

    বাবে (i = 0, Y = 0; i < cb->অঙ্ক_Regions; i++, Y += Y_incr)
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
	ReadRegion.শীৰ্ষ_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	যদি (!ReadConsoleOutputW(g_hConOut,	// output handle
		cb->Buffer,			// our buffer
		cb->BufferSize,			// dimensions of our buffer
		BufferCoord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->Buffer);
	    VIM_CLEAR(cb->Regions);
	    ঘূৰাই FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    ঘূৰাই TRUE;
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
    স্থির BOOL
RestoreConsoleBuffer(
    ConsoleBuffer   *cb,
    BOOL	    RestoreScreen)
{
    COORD BufferCoord;
    SMALL_RECT WriteRegion;
    পূর্ণাঙ্ক i;

    যদি (cb == রিক্ত || !cb->IsValid)
	ঘূৰাই FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    যদি (RestoreScreen)
	ClearConsoleBuffer(cb->Info.wAttributes);

    FitConsoleWindow(cb->Info.dwSize, TRUE);
    যদি (!SetConsoleScreenBufferSize(g_hConOut, cb->Info.dwSize))
	ঘূৰাই FALSE;
    যদি (!SetConsoleTextAttribute(g_hConOut, cb->Info.wAttributes))
	ঘূৰাই FALSE;

    যদি (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	ঘূৰাই TRUE;
    }

    যদি (!SetConsoleCursorPosition(g_hConOut, cb->Info.dwCursorPosition))
	ঘূৰাই FALSE;
    যদি (!SetConsoleWindowInfo(g_hConOut, TRUE, &cb->Info.srWindow))
	ঘূৰাই FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    যদি (use_alternate_screen_buffer)
	ঘূৰাই TRUE;

    /*
     * Restore the screen buffer contents.
     */
    যদি (cb->Buffer != রিক্ত)
    {
	বাবে (i = 0; i < cb->অঙ্ক_Regions; i++)
	{
	    BufferCoord.X = cb->Regions[i].বাম_3;
	    BufferCoord.Y = cb->Regions[i].শীৰ্ষ_2;
	    WriteRegion = cb->Regions[i];
	    যদি (!WriteConsoleOutputW(g_hConOut,	// output handle
			cb->Buffer,		// our buffer
			cb->BufferSize,		// dimensions of our buffer
			BufferCoord,		// offset in our buffer
			&WriteRegion))		// region to restore
		ঘূৰাই FALSE;
	}
    }

    ঘূৰাই TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
স্থির ConsoleBuffer g_cbOrig = { 0 };
# endif
স্থির ConsoleBuffer g_cbNonTermcap = { 0 };
স্থির ConsoleBuffer g_cbTermcap = { 0 };

ৱর্ণ g_szOrigTitle[256] = { 0 };
HWND g_hWnd = রিক্ত;	// also used in os_mswin.c
স্থির HICON g_hOrigIconSmall = রিক্ত;
স্থির HICON g_hOrigIcon = রিক্ত;
স্থির HICON g_hVimIcon = রিক্ত;
স্থির BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    স্থির BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    যদি (hWnd == রিক্ত)
	ঘূৰাই FALSE;

    যদি (phIconSmall != রিক্ত)
	*phIconSmall = (HICON)SendMessage(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    যদি (phIcon != রিক্ত)
	*phIcon = (HICON)SendMessage(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    ঘূৰাই TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    স্থির BOOL
SetConsoleIcon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    যদি (hWnd == রিক্ত)
	ঘূৰাই FALSE;

    যদি (hIconSmall != রিক্ত)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    যদি (hIcon != রিক্ত)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    ঘূৰাই TRUE;
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
    স্থির শূন্য
SaveConsoleTitleAndIcon(শূন্য)
{
    // Save the original title.
    যদি (!GetConsoleTitle(g_szOrigTitle, পরিমাণ(g_szOrigTitle)))
	ঘূৰাই;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = GetConsoleWindow();
    যদি (g_hWnd == রিক্ত)
	ঘূৰাই;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    যদি (g_hOrigIconSmall == রিক্ত || g_hOrigIcon == রিক্ত)
	ঘূৰাই;

    // Extract the first icon contained in the Vim executable.
    যদি (
# ifdef FEAT_LIBCALL
	    mch_icon_load((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == রিক্ত)
	g_hVimIcon = ExtractIcon(রিক্ত, (LPCSTR)exe_name, 0);
    যদি (g_hVimIcon != রিক্ত)
	g_fCanChangeIcon = TRUE;
}

স্থির পূর্ণাঙ্ক g_fWindInitCalled = FALSE;
স্থির পূর্ণাঙ্ক g_fTermcapMode = FALSE;
স্থির CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    স্থির শূন্য
mch_init_c(শূন্য)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_BUFFER_INFO csbi;
# endif
# ifndef __MINGW32__
    বাহ্য পূর্ণাঙ্ক _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_দ্বিমিক;		// we do our own CR-LF translation
    out_flush();

    // Obtain handles for the standard Console I/O devices
    যদি (read_cmd_fd == 0)
	g_hConIn =  GetStdHandle(STD_প্ৰবেশ_HANDLE);
    অন্যথা
	create_conin();
    g_hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    SaveConsoleBuffer(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.wAttributes;
# অন্যথা
    // Get current text attributes
    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
# endif
    যদি (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (g_attrCurrent & 0xf) + 1;
    যদি (cterm_normal_bg_color == 0)
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
    যদি (g_fCanChangeIcon)
	SetConsoleIcon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    যদি (use_alternate_screen_buffer)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    যদি (fdDump)
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
    স্থির শূন্য
mch_exit_c(পূর্ণাঙ্ক r)
{
    // Copy flag since stoptermcap() will clear the flag.
    পূর্ণাঙ্ক fTermcapMode = g_fTermcapMode;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    যদি (!fTermcapMode &&  use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    যদি (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    যদি (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	যদি (g_fCanChangeIcon && !g_fForceExit)
	    SetConsoleIcon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	যদি (fdDump)
	{
	    time_t t;

	    time(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = রিক্ত;
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

    শূন্য
mch_init(শূন্য)
{
#ifdef VIMDLL
    যদি (gui.starting)
	mch_init_g();
    অন্যথা
	mch_init_c();
#নোচেদ্যদি defined(FEAT_GUI_MSWIN)
    mch_init_g();
#অন্যথা
    mch_init_c();
#endif
}

    শূন্য
mch_exit(পূর্ণাঙ্ক r)
{
#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef VIMDLL
    যদি (vimrun_path_allocated)
	vim_free(vimrun_path.string);

    যদি (gui.in_use || gui.starting)
	mch_exit_g(r);
    অন্যথা
	mch_exit_c(r);
#নোচেদ্যদি defined(FEAT_GUI_MSWIN)
    যদি (vimrun_path_allocated)
	vim_free(vimrun_path.string);
    mch_exit_g(r);
#অন্যথা
    mch_exit_c(r);
#endif
}

/*
 * Do we have an interactive window?
 */
    পূর্ণাঙ্ক
mch_check_win(
    পূর্ণাঙ্ক argc UNUSED,
    ৱর্ণ **argv UNUSED)
{
    mch_get_exe_name();

#যদি defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    ঘূৰাই OK;	    // GUI always has a tty
#অন্যথা
# ifdef VIMDLL
    যদি (gui.in_use)
	ঘূৰাই OK;
# endif
    যদি (isatty(1))
	ঘূৰাই OK;
    ঘূৰাই FAIL;
#endif
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    শূন্য
fname_case(
    char_u	*name,
    পূর্ণাঙ্ক		len)
{
    WCHAR   *p;
    WCHAR   buf[_MAX_PATH + 1];

    যদি (*name == NUL)
	ঘূৰাই;

    slash_adjust(name);

    p = enc_to_utf16(name, রিক্ত);
    যদি (p == রিক্ত)
	ঘূৰাই;

    যদি (GetLongPathNameW(p, buf, _MAX_PATH))
    {
	char_u	*q = utf16_to_enc(buf, রিক্ত);

	যদি (q != রিক্ত)
	{
	    যদি (len > 0)
		vim_strncpy(name, q, len - 1);
	    অন্যথা
	    {
		size_t  namelen = STRLEN(name);

		যদি (namelen >= STRLEN(q))
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
    পূর্ণাঙ্ক
mch_get_user_name(
    char_u  *s,
    পূর্ণাঙ্ক	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_LENGTH(wszUserName);

    যদি (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_to_enc(wszUserName, রিক্ত);

	যদি (p != রিক্ত)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    ঘূৰাই OK;
	}
    }
    s[0] = NUL;
    ঘূৰাই FAIL;
}


/*
 * Insert host name in s[len].
 */
    শূন্য
mch_get_host_name(
    char_u	*s,
    পূর্ণাঙ্ক		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_LENGTH(wszHostName);

    যদি (!GetComputerNameW(wszHostName, &wcch))
	ঘূৰাই;

    char_u  *p = utf16_to_enc(wszHostName, রিক্ত);
    যদি (p == রিক্ত)
	ঘূৰাই;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    long
mch_get_pid(শূন্য)
{
    ঘূৰাই (long)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    পূর্ণাঙ্ক
mch_process_running(long pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   status = 0;
    পূর্ণাঙ্ক	    ret = FALSE;

    যদি (hProcess == রিক্ত)
	ঘূৰাই FALSE;  // might not have access
    যদি (GetExitCodeProcess(hProcess, &status))
	ret = status == STILL_ACTIVE;
    CloseHandle(hProcess);
    ঘূৰাই ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    পূর্ণাঙ্ক
mch_dirname(
    char_u	*buf,
    পূর্ণাঙ্ক		len)
{
    WCHAR   wbuf[_MAX_PATH + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    যদি (GetCurrentDirectoryW(_MAX_PATH, wbuf) == 0)
	ঘূৰাই FAIL;

    WCHAR   wcbuf[_MAX_PATH + 1];
    char_u  *p = রিক্ত;

    যদি (GetLongPathNameW(wbuf, wcbuf, _MAX_PATH) != 0)
    {
	p = utf16_to_enc(wcbuf, রিক্ত);
	যদি (p != রিক্ত)
	{
	    যদি (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    যদি (p == রিক্ত)
	p = utf16_to_enc(wbuf, রিক্ত);

    যদি (p == রিক্ত)
	ঘূৰাই FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    ঘূৰাই OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    long
mch_getperm(char_u *name)
{
    stat_T	st;

    ঘূৰাই (mch_stat((ৱর্ণ *)name, &st) == 0)
	? (long)(unsigned short)st.st_mode
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    পূর্ণাঙ্ক
mch_setperm(char_u *name, long perm)
{
    long	n;
    WCHAR	*p;

    p = enc_to_utf16(name, রিক্ত);
    যদি (p == রিক্ত)
	ঘূৰাই FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    যদি (n == -1)
	ঘূৰাই FAIL;

    win32_set_archive(name);

    ঘূৰাই OK;
}

/*
 * Set hidden flag for "name".
 */
    শূন্য
mch_hide(char_u *name)
{
    পূর্ণাঙ্ক attrs = win32_getattrs(name);
    যদি (attrs == -1)
	ঘূৰাই;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    পূর্ণাঙ্ক
mch_ishidden(char_u *name)
{
    পূর্ণাঙ্ক f = win32_getattrs(name);

    যদি (f == -1)
	ঘূৰাই FALSE;		    // file does not exist at all

    ঘূৰাই (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    পূর্ণাঙ্ক
mch_isdir(char_u *name)
{
    পূর্ণাঙ্ক f = win32_getattrs(name);

    যদি (f == -1)
	ঘূৰাই FALSE;		    // file does not exist at all

    ঘূৰাই (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    পূর্ণাঙ্ক
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_to_utf16(name, রিক্ত);
    যদি (p == রিক্ত)
	ঘূৰাই FALSE;
    attr = GetFileAttributesW(p);
    যদি (attr == INVALID_FILE_ATTRIBUTES)
	error = GetLastError();
    vim_free(p);

    যদি (attr == INVALID_FILE_ATTRIBUTES)
	ঘূৰাই error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    ঘূৰাই (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    পূর্ণাঙ্ক
mch_isrealdir(char_u *name)
{
    ঘূৰাই mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    পূর্ণাঙ্ক
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    পূর্ণাঙ্ক	    retval;

    p = enc_to_utf16(name, রিক্ত);
    যদি (p == রিক্ত)
	ঘূৰাই -1;
    retval = _wmkdir(p);
    vim_free(p);
    ঘূৰাই retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    পূর্ণাঙ্ক
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    পূর্ণাঙ্ক	    retval;

    p = enc_to_utf16(name, রিক্ত);
    যদি (p == রিক্ত)
	ঘূৰাই -1;
    retval = _wrmdir(p);
    vim_free(p);
    ঘূৰাই retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    পূর্ণাঙ্ক
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    ঘূৰাই win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_সংখ্যা_Of_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    পূর্ণাঙ্ক
mch_is_symbolic_link(char_u *name)
{
    HANDLE		hFind;
    পূর্ণাঙ্ক			res = FALSE;
    DWORD		fileFlags = 0, reparseTag = 0;
    WCHAR		*wn;
    WIN32_FIND_DATAW	findDataW;

    wn = enc_to_utf16(name, রিক্ত);
    যদি (wn == রিক্ত)
	ঘূৰাই FALSE;

    hFind = FindFirstFileW(wn, &findDataW);
    vim_free(wn);
    যদি (hFind != INVALID_HANDLE_মান)
    {
	fileFlags = findDataW.dwFileAttributes;
	reparseTag = findDataW.dwReserved0;
	FindClose(hFind);
    }

    যদি ((fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
	    && (reparseTag == IO_REPARSE_TAG_SYMLINK
		|| reparseTag == IO_REPARSE_TAG_MOUNT_POINT))
	res = TRUE;

    ঘূৰাই res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    পূর্ণাঙ্ক
mch_is_linked(char_u *fname)
{
    যদি (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    পূর্ণাঙ্ক
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    পূর্ণাঙ্ক		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_to_utf16(fname, রিক্ত);
    যদি (wn == রিক্ত)
	ঘূৰাই FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    রিক্ত,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    রিক্ত);		// handle to template file
    vim_free(wn);

    যদি (hFile == INVALID_HANDLE_মান)
	ঘূৰাই FILEINFO_READ_FAIL;

    যদি (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    অন্যথা
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    ঘূৰাই res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    স্থির পূর্ণাঙ্ক
win32_getattrs(char_u *name)
{
    পূর্ণাঙ্ক		attr;
    WCHAR	*p;

    p = enc_to_utf16(name, রিক্ত);
    যদি (p == রিক্ত)
	ঘূৰাই INVALID_FILE_ATTRIBUTES;

    attr = GetFileAttributesW(p);
    vim_free(p);

    ঘূৰাই attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    স্থির পূর্ণাঙ্ক
win32_setattrs(char_u *name, পূর্ণাঙ্ক attrs)
{
    পূর্ণাঙ্ক	    res;
    WCHAR   *p;

    p = enc_to_utf16(name, রিক্ত);
    যদি (p == রিক্ত)
	ঘূৰাই -1;

    res = SetFileAttributesW(p, attrs);
    vim_free(p);

    ঘূৰাই res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    স্থির পূর্ণাঙ্ক
win32_set_archive(char_u *name)
{
    পূর্ণাঙ্ক attrs = win32_getattrs(name);
    যদি (attrs == -1)
	ঘূৰাই -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    ঘূৰাই win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    পূর্ণাঙ্ক
mch_writable(char_u *name)
{
    পূর্ণাঙ্ক attrs = win32_getattrs(name);

    ঘূৰাই (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    পূর্ণাঙ্ক
mch_can_exe(char_u *name, char_u **path, পূর্ণাঙ্ক use_path UNUSED)
{
    ঘূৰাই executable_exists((ৱর্ণ *)name, STRLEN(name), path, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    পূর্ণাঙ্ক
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    পূর্ণাঙ্ক		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    যদি (STRNCMP(name, "\\\\.\\", 4) == 0)
	ঘূৰাই গ্ৰন্থি_WRITABLE;

    wn = enc_to_utf16(name, রিক্ত);
    যদি (wn == রিক্ত)
	ঘূৰাই গ্ৰন্থি_NORMAL;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    রিক্ত,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    রিক্ত);		    // handle to template file
    vim_free(wn);
    যদি (hFile == INVALID_HANDLE_মান)
	ঘূৰাই গ্ৰন্থি_NORMAL;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    যদি (type == FILE_TYPE_CHAR)
	ঘূৰাই গ্ৰন্থি_WRITABLE;
    যদি (type == FILE_TYPE_DISK)
	ঘূৰাই গ্ৰন্থি_NORMAL;
    ঘূৰাই গ্ৰন্থি_OTHER;
}

#ifdef HAVE_ACL
সংরচনা my_acl
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
    ঘূৰাই (vim_acl_T)রিক্ত;
#অন্যথা
    সংরচনা my_acl   *p = রিক্ত;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(সংরচনা my_acl);
    যদি (p != রিক্ত)
    {
	WCHAR	*wn;

	wn = enc_to_utf16(fname, রিক্ত);
	যদি (wn == রিক্ত)
	{
	    vim_free(p);
	    ঘূৰাই রিক্ত;
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
	যদি (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (শূন্য)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    রিক্ত,
		    রিক্ত,
		    &p->pDacl,
		    রিক্ত,
		    &p->pSecurityDescriptor);
	}
	যদি (p->pSecurityDescriptor == রিক্ত)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = রিক্ত;
	}
	vim_free(wn);
    }

    ঘূৰাই (vim_acl_T)p;
#endif
}

#ifdef HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    স্থির BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_SIZE_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_গুনতি = 0;
    GetAclInformation(acl, &acl_info, পরিমাণ(acl_info), AclSizeInformation);
    বাবে (i = 0; i < acl_info.Ace_গুনতি; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	যদি (ace->Header.AceFlags & INHERITED_ACE)
	    ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}
#endif

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    শূন্য
mch_set_acl(char_u *fname, vim_acl_T acl)
{
#ifdef HAVE_ACL
    সংরচনা my_acl   *p = (সংরচনা my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    যদি (p == রিক্ত)
	ঘূৰাই;

    wn = enc_to_utf16(fname, রিক্ত);
    যদি (wn == রিক্ত)
	ঘূৰাই;

    // Set security flags
    যদি (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    যদি (p->pSidGroup)
	sec_info |= GROUP_SECURITY_INFORMATION;
    যদি (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	যদি (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    যদি (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (শূন্য)SetNamedSecurityInfoW(
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

    শূন্য
mch_free_acl(vim_acl_T acl)
{
#ifdef HAVE_ACL
    সংরচনা my_acl   *p = (সংরচনা my_acl *)acl;

    যদি (p != রিক্ত)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#endif
}

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    স্থির BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    প্ৰবেশ_RECORD ir;
    DWORD out;

    ৱিকল্প (dwCtrlType)
    {
    পক্ষ CTRL_C_EVENT:
	যদি (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	ঘূৰাই TRUE;

    পক্ষ CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = চাবি_EVENT;
	ir.Event.চাবি_Event_2.b_চাবি_Down = TRUE;
	ir.Event.চাবি_Event_2.w_Repeat_গুনতি = 1;
	ir.Event.চাবি_Event_2.w_Virtual_চাবি_Code = VK_CANCEL;
	ir.Event.চাবি_Event_2.wVirtualScanCode = 0;
	ir.Event.চাবি_Event_2.dw_Control_চাবি_State = 0;
	ir.Event.চাবি_Event_2.uChar.UnicodeChar = 0;
	Write_Console_প্ৰবেশ(g_hConIn, &ir, 1, &out);
	ঘূৰাই TRUE;

    // fatal events: shut down gracefully
    পক্ষ CTRL_CLOSE_EVENT:
    পক্ষ CTRL_LOGOFF_EVENT:
    পক্ষ CTRL_SHUTDOWN_EVENT:
	windgoto((পূর্ণাঙ্ক)Rows - 1, cmdline_col_off);
	g_fForceExit = TRUE;

	vim_snprintf((ৱর্ণ *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	ঘূৰাই TRUE;		// not reached

    স্ৱভাৱ:
	ঘূৰাই FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    শূন্য
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    যদি (gui.in_use)
	ঘূৰাই;
# endif
    GetConsoleMode(g_hConIn, &cmodein);
    GetConsoleMode(g_hConOut, &cmodeout);
    যদি (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_সাৰি_প্ৰবেশ | ENABLE_PROCESSED_প্ৰবেশ |
		     ENABLE_ECHO_প্ৰবেশ);
	যদি (g_fMouseActive)
	{
	    cmodein |= ENABLE_MOUSE_প্ৰবেশ;
	    cmodein &= ~ENABLE_QUICK_EDIT_MODE;
	}
	অন্যথা
	{
	    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# অন্যথা
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    অন্যথা // cooked
    {
	cmodein |= (ENABLE_সাৰি_প্ৰবেশ | ENABLE_PROCESSED_প্ৰবেশ |
		    ENABLE_ECHO_প্ৰবেশ);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
    SetConsoleMode(g_hConOut, cmodeout);
    SetConsoleCtrlHandler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    যদি (fdDump)
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
    পূর্ণাঙ্ক
mch_get_shellsize(শূন্য)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

# ifdef VIMDLL
    যদি (gui.in_use)
	ঘূৰাই OK;
# endif
    যদি (!g_fTermcapMode && g_cbTermcap.IsValid)
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
    অন্যথা যদি (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	Rows = csbi.srWindow.Bottom - csbi.srWindow.শীৰ্ষ_2 + 1;
	Columns = csbi.srWindow.ডান_3 - csbi.srWindow.বাম_3 + 1;
    }
    অন্যথা
    {
	Rows = 25;
	Columns = 80;
    }
    ঘূৰাই OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    স্থির শূন্য
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    যদি (use_alternate_screen_buffer)
	ঘূৰাই;

    যদি (!SetConsoleScreenBufferSize(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	যদি (fdDump)
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
    স্থির শূন্য
ResizeWindow(
    HANDLE     hConsole,
    SMALL_RECT srWindowRect)
{
    যদি (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
    {
# ifdef MCH_WRITE_DUMP
	যদি (fdDump)
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
    স্থির শূন্য
ResizeConBufAndWindow(
    HANDLE  hConsole,
    পূর্ণাঙ্ক	    xSize,
    পূর্ণাঙ্ক	    ySize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    srWindowRect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    স্থির পূর্ণাঙ্ক	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    যদি (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", xSize, ySize);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    // define the new console window size and scroll position
    srWindowRect.বাম_3 = srWindowRect.শীৰ্ষ_2 = (SHORT) 0;
    srWindowRect.ডান_3 =  (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);

    যদি (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	পূর্ণাঙ্ক sx, sy;

	sx = csbi.srWindow.ডান_3 - csbi.srWindow.বাম_3 + 1;
	sy = csbi.srWindow.Bottom - csbi.srWindow.শীৰ্ষ_2 + 1;
	যদি (sy < ySize || sx < xSize)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    যদি (sy < ySize)
		coordScreen.Y = ySize;
	    অন্যথা
		coordScreen.Y = sy;
	    যদি (sx < xSize)
		coordScreen.X = xSize;
	    অন্যথা
		coordScreen.X = sx;
	    SetConsoleScreenBufferSize(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = xSize;
    coordScreen.Y = ySize;

    // In the new console call API, only the first time in reverse order
    যদি (!vtp_working || resized)
    {
	ResizeWindow(hConsole, srWindowRect);
	ResizeConBuf(hConsole, coordScreen);
    }
    অন্যথা
    {
	// Workaround for a Windows 10 bug
	cursor.X = srWindowRect.বাম_3;
	cursor.Y = srWindowRect.শীৰ্ষ_2;
	SetConsoleCursorPosition(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	ResizeWindow(hConsole, srWindowRect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    শূন্য
mch_set_shellsize(শূন্য)
{
    COORD coordScreen;

# ifdef VIMDLL
    যদি (gui.in_use)
	ঘূৰাই;
# endif
    // Don't change window size while still starting up
    যদি (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	ঘূৰাই;
    }

    যদি (term_console)
    {
	coordScreen = GetLargestConsoleWindowSize(g_hConOut);

	// Clamp Rows and Columns to reasonable values
	যদি (Rows > coordScreen.Y)
	    Rows = coordScreen.Y;
	যদি (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	ResizeConBufAndWindow(g_hConOut, Columns, Rows);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    শূন্য
mch_new_shellsize(শূন্য)
{
# ifdef VIMDLL
    যদি (gui.in_use)
	ঘূৰাই;
# endif
    set_scroll_region(0, 0, Columns - 1, Rows - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    শূন্য
mch_set_winsize_now(শূন্য)
{
    যদি (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_set_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# যদি defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    শূন্য
mch_calc_cell_size(সংরচনা cellsize *cs_out)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_out->cs_xpixel = -1;
    cs_out->cs_ypixel = -1;

    যদি (g_hConOut == INVALID_HANDLE_মান)
	ঘূৰাই;

    cfi.cbSize = পরিমাণ(cfi);
    যদি (GetCurrentConsoleFontEx(g_hConOut, FALSE, &cfi)
	    && cfi.dwFontSize.X > 0 && cfi.dwFontSize.Y > 0)
    {
	cs_out->cs_xpixel = cfi.dwFontSize.X;
	cs_out->cs_ypixel = cfi.dwFontSize.Y;
	ঘূৰাই;
    }

    GetConsoleMode(g_hConIn, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_সাৰি_প্ৰবেশ | ENABLE_ECHO_প্ৰবেশ
	    | ENABLE_PROCESSED_প্ৰবেশ);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_প্ৰবেশ;

    যদি (SetConsoleMode(g_hConIn, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	SetConsoleMode(g_hConIn, cmodeold);
    }
}
# endif

#endif

    স্থির BOOL
vim_create_process(
    ৱর্ণ		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    ৱর্ণ		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = রিক্ত;

    wcmd = enc_to_utf16((char_u *)cmd, রিক্ত);
    যদি (wcmd == রিক্ত)
	ঘূৰাই FALSE;
    যদি (cwd != রিক্ত)
    {
	wcwd = enc_to_utf16((char_u *)cwd, রিক্ত);
	যদি (wcwd == রিক্ত)
	    গচ্ছ theend;
    }

    ret = CreateProcessW(
	    রিক্ত,		// Executable name
	    wcmd,		// Command to execute
	    রিক্ত,		// Process security attributes
	    রিক্ত,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    ঘূৰাই ret;
}


    স্থির HINSTANCE
vim_shell_execute(
    ৱর্ণ *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_to_utf16((char_u *)cmd, রিক্ত);
    যদি (wcmd == রিক্ত)
	ঘূৰাই (HINSTANCE) 0;

    ret = ShellExecuteW(রিক্ত, রিক্ত, wcmd, রিক্ত, রিক্ত, n_show_cmd);
    vim_free(wcmd);
    ঘূৰাই ret;
}


#যদি defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    স্থির পূর্ণাঙ্ক
mch_system_classic(ৱর্ণ *cmd, পূর্ণাঙ্ক options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = পরিমাণ(si);
    si.lpReserved = রিক্ত;
    si.lpDesktop = রিক্ত;
    si.lpTitle = রিক্ত;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    যদি (options & SHELL_DOOUT)
	si.wShowWindow = SW_SHOWMINNOACTIVE;
    অন্যথা
	si.wShowWindow = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = রিক্ত;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_MODE |	CREATE_NEW_CONSOLE,
	    &si, &pi, রিক্ত, রিক্ত);

    // Wait for the command to terminate before continuing
    {
	পূর্ণাঙ্ক	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	বাবে (;;)
	{
	    বাৰ্তা_2	বাৰ্তা;

	    যদি (PeekMessageW(&বাৰ্তা, (HWND)রিক্ত, 0, 0, PM_REMOVE))
	    {
		TranslateMessage(&বাৰ্তা);
		DispatchMessageW(&বাৰ্তা);
		delay = 1;
		অনুৱর্ত;
	    }
	    যদি (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		ভঙ্গ;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    যদি (delay < 50)
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

    ঘূৰাই ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    স্থির unsigned পূর্ণাঙ্ক __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_hChildStd_IN_Wr = param;
    linenr_T	    lnum = curbuf->b_op_start.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    পূর্ণাঙ্ক		    written = 0;

    বাবে (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	যদি (l == 0)
	    len = 0;
	অন্যথা যদি (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_hChildStd_IN_Wr, "", 1, &len, রিক্ত);
	}
	অন্যথা
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_hChildStd_IN_Wr, (ৱর্ণ *)lp + written,
		      s == রিক্ত ? l : (DWORD)(s - (lp + written)),
		      &len, রিক্ত);
	}
	যদি (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    যদি (lnum != curbuf->b_op_end.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_সাৰি_গুনতি
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_hChildStd_IN_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, রিক্ত);
	    }

	    ++lnum;
	    যদি (lnum > curbuf->b_op_end.lnum)
		ভঙ্গ;

	    lp = ml_get(lnum);
	    written = 0;
	}
	অন্যথা যদি (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_hChildStd_IN_Wr);
    ঘূৰাই 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    স্থির শূন্য
dump_pipe(পূর্ণাঙ্ক	    options,
	  HANDLE    g_hChildStd_OUT_Rd,
	  garray_T  *ga,
	  char_u    buffer[],
	  DWORD	    *buffer_off)
{
    DWORD	availableBytes = 0;
    DWORD	i;
    পূর্ণাঙ্ক		ret;
    DWORD	len;
    DWORD	toRead;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_hChildStd_OUT_Rd, // pipe to query
			রিক্ত,		    // optional buffer
			0,		    // buffer size
			রিক্ত,		    // number of read bytes
			&availableBytes,    // available bytes total
			রিক্ত);		    // byteLeft

    // We got real data in the pipe, read it
    যেতিয়ালৈ (ret != 0 && availableBytes > 0)
    {
	toRead = (DWORD)(BUFLEN - *buffer_off);
	toRead = availableBytes < toRead ? availableBytes : toRead;
	ReadFile(g_hChildStd_OUT_Rd, buffer + *buffer_off, toRead , &len, রিক্ত);

	// If we haven't read anything, there is a problem
	যদি (len == 0)
	    ভঙ্গ;

	availableBytes -= len;

	যদি (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    বাবে (i = 0; i < len; ++i)
	    {
		যদি (buffer[i] == NL)
		    append_ga_সাৰি(ga);
		অন্যথা যদি (buffer[i] == NUL)
		    ga_append(ga, NL);
		অন্যথা
		    ga_append(ga, buffer[i]);
	    }
	}
	অন্যথা যদি (has_mbyte)
	{
	    পূর্ণাঙ্ক		l;
	    পূর্ণাঙ্ক		c;
	    char_u	*p;

	    len += *buffer_off;
	    buffer[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    বাবে (p = buffer; p < buffer + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		যদি (l == 0)
		    l = 1;  // NUL byte?
		অন্যথা যদি (MB_BYTE2LEN(*p) != l)
		    ভঙ্গ;
	    }
	    যদি (p == buffer)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		যদি (len >= 12)
		    ++p;
		অন্যথা
		{
		    *buffer_off = len;
		    ঘূৰাই;
		}
	    }
	    c = *p;
	    *p = NUL;
	    বাৰ্তা_puts((ৱর্ণ *)buffer);
	    যদি (p < buffer + len)
	    {
		*p = c;
		*buffer_off = (DWORD)((buffer + len) - p);
		mch_memmove(buffer, p, *buffer_off);
		ঘূৰাই;
	    }
	    *buffer_off = 0;
	}
	অন্যথা
	{
	    buffer[len] = NUL;
	    বাৰ্তা_puts((ৱর্ণ *)buffer);
	}

	windgoto(বাৰ্তা_row, cmdline_col_off + বাৰ্তা_col);
	cursor_on();
	out_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    স্থির পূর্ণাঙ্ক
mch_system_piped(ৱর্ণ *cmd, পূর্ণাঙ্ক options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_hChildStd_IN_Rd = রিক্ত;
    HANDLE g_hChildStd_IN_Wr = রিক্ত;
    HANDLE g_hChildStd_OUT_Rd = রিক্ত;
    HANDLE g_hChildStd_OUT_Wr = রিক্ত;

    char_u	buffer[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    পূর্ণাঙ্ক		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    পূর্ণাঙ্ক		noread_cnt = 0;
    garray_T	ga;
    পূর্ণাঙ্ক		delay = 1;
    DWORD	buffer_off = 0;	// valid bytes in buffer[]
    ৱর্ণ	*p = রিক্ত;

    SECURITY_ATTRIBUTES saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = পরিমাণ(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = রিক্ত;

    যদি ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)
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
	বাৰ্তা_puts(_("\nCannot create pipes\n"));
    }

    si.cb = পরিমাণ(si);
    si.lpReserved = রিক্ত;
    si.lpDesktop = রিক্ত;
    si.lpTitle = রিক্ত;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_hChildStd_OUT_Wr;
    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.h_Std_প্ৰবেশ = g_hChildStd_IN_Rd;
    si.wShowWindow = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = রিক্ত;

    যদি (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    যদি (cmd != রিক্ত)
    {
	p = (ৱর্ণ *)vim_strsave((char_u *)cmd);
	যদি (p != রিক্ত)
	    unescape_shellxquote((char_u *)p, p_sxe);
	অন্যথা
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_MODE,
						&si, &pi, রিক্ত, রিক্ত);

    যদি (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(g_hChildStd_OUT_Wr);

    যদি (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(রিক্ত,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_hChildStd_IN_Wr,  // parameter
			0,		 // creation flag, start immediately
			রিক্ত);		    // we don't care about thread id
	CloseHandle(thread);
	g_hChildStd_IN_Wr = রিক্ত;
    }

    // Keep updating the window while waiting for the shell to finish.
    বাবে (;;)
    {
	বাৰ্তা_2	বাৰ্তা;

	যদি (PeekMessageW(&বাৰ্তা, (HWND)রিক্ত, 0, 0, PM_REMOVE))
	{
	    TranslateMessage(&বাৰ্তা);
	    DispatchMessageW(&বাৰ্তা);
	}

	// write pipe information in the window
	যদি ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.in_use
# endif
	    )
	{
	    len = 0;
	    যদি (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		যদি (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		যদি (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    যদি (len == 1 && cmd != রিক্ত)
		    {
			যদি (ta_buf[ta_len] == Ctrl_C)
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
		    যদি ((len == 1 || len == 4 ) && cmd != রিক্ত)
		    {
			যদি (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_hChildStd_IN_Wr);
			    g_hChildStd_IN_Wr = রিক্ত;
			    len = 0;
			}
		    }

		    len = term_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    বাবে (i = ta_len; i < ta_len + len; ++i)
		    {
			যদি (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    বাৰ্তা_putchar(ta_buf[i]);
			অন্যথা যদি (has_mbyte)
			{
			    পূর্ণাঙ্ক l = (*mb_ptr2len)(ta_buf + i);

			    বাৰ্তা_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			অন্যথা
			    বাৰ্তা_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(বাৰ্তা_row, cmdline_col_off + বাৰ্তা_col);
		    out_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    যদি (options & SHELL_WRITE)
			ta_len = 0;
		    অন্যথা যদি (g_hChildStd_IN_Wr != রিক্ত)
		    {
			WriteFile(g_hChildStd_IN_Wr, (ৱর্ণ*)ta_buf,
				    1, &len, রিক্ত);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			যদি (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	যদি (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	যদি (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);
	    ভঙ্গ;
	}

	++noread_cnt;
	dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	যদি (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_hChildStd_OUT_Rd);
    যদি (g_hChildStd_IN_Wr != রিক্ত)
	CloseHandle(g_hChildStd_IN_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    GetExitCodeProcess(pi.hProcess, &ret);

    যদি (options & SHELL_READ)
    {
	যদি (ga.ga_len > 0)
	{
	    append_ga_সাৰি(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	অন্যথা
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    ঘূৰাই ret;
}

    স্থির পূর্ণাঙ্ক
mch_system_g(ৱর্ণ *cmd, পূর্ণাঙ্ক options)
{
    // if we can pipe and the shelltemp option is off
    যদি (!p_stmp)
	ঘূৰাই mch_system_piped(cmd, options);
    অন্যথা
	ঘূৰাই mch_system_classic(cmd, options);
}
#endif

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    স্থির পূর্ণাঙ্ক
mch_system_c(ৱর্ণ *cmd, পূর্ণাঙ্ক options UNUSED)
{
    পূর্ণাঙ্ক		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    যদি (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	যদি (buf == রিক্ত)
	    ঘূৰাই -1;
	vim_snprintf((ৱর্ণ *)buf, len, "(%s)", cmd);
	wcmd = enc_to_utf16(buf, রিক্ত);
	মুক্ত(buf);
    }
    অন্যথা
	wcmd = enc_to_utf16((char_u *)cmd, রিক্ত);

    যদি (wcmd == রিক্ত)
	ঘূৰাই -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    ঘূৰাই ret;
}

#endif

    স্থির পূর্ণাঙ্ক
mch_system(ৱর্ণ *cmd, পূর্ণাঙ্ক options)
{
#ifdef VIMDLL
    যদি (gui.in_use || gui.starting)
	ঘূৰাই mch_system_g(cmd, options);
    অন্যথা
	ঘূৰাই mch_system_c(cmd, options);
#নোচেদ্যদি defined(FEAT_GUI_MSWIN)
    ঘূৰাই mch_system_g(cmd, options);
#অন্যথা
    ঘূৰাই mch_system_c(cmd, options);
#endif
}

#যদি defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    স্থির পূর্ণাঙ্ক
mch_call_shell_terminal(
    char_u	*cmd,
    পূর্ণাঙ্ক		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = রিক্ত;
    typval_T	argvar[2];
    long_u	cmdlen;
    পূর্ণাঙ্ক		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    যদি (cmd == রিক্ত)
	cmdlen = STRLEN(p_sh) + 1;
    অন্যথা
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    যদি (newcmd == রিক্ত)
	ঘূৰাই 255;
    যদি (cmd == রিক্ত)
    {
	STRCPY(newcmd, p_sh);
	ch_log(রিক্ত, "starting terminal to run a shell");
    }
    অন্যথা
    {
	vim_snprintf((ৱর্ণ *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(রিক্ত, "starting terminal for system command '%s'", cmd);
    }

    init_job_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = term_start(argvar, রিক্ত, &opt, TERM_START_SYSTEM);
    যদি (buf == রিক্ত)
    {
	vim_free(newcmd);
	ঘূৰাই 255;
    }

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    যদি (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	যেতিয়ালৈ (term_use_loop())
	{
	    যদি (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		যদি (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    অন্যথা
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(রিক্ত, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    ঘূৰাই retval;
}
#endif
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    স্থির শূন্য
restore_env_var(char_u *name, char_u *old_মান, পূর্ণাঙ্ক must_free)
{
    যদি (old_মান != রিক্ত)
    {
	vim_setenv(name, old_মান);
	যদি (must_free)
	    vim_free(old_মান);
	ঘূৰাই;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    পূর্ণাঙ্ক
mch_call_shell(
    char_u  *cmd,
    পূর্ণাঙ্ক	    options)	// SHELL_*, see vim.h
{
    পূর্ণাঙ্ক		x = 0;
    পূর্ণাঙ্ক		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    পূর্ণাঙ্ক		must_free;
    char_u	*oldval;

#ifdef FEAT_EVAL
    ch_log(রিক্ত, "executing shell command: %s", cmd);
#endif
    // Change the title to reflect that we are in a subshell.
    যদি (GetConsoleTitleW(szShellTitle, ARRAY_LENGTH(szShellTitle) - 4) > 0)
    {
	যদি (cmd == রিক্ত)
	    wcscat(szShellTitle, L" :sh");
	অন্যথা
	{
	    WCHAR *wn = enc_to_utf16((char_u *)cmd, রিক্ত);

	    যদি (wn != রিক্ত)
	    {
		wcscat(szShellTitle, L" - !");
		যদি ((wcslen(szShellTitle) + wcslen(wn) <
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
    যদি (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#endif
#যদি defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    যদি (
# ifdef VIMDLL
	    gui.in_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != রিক্ত
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	যদি (cmdbase != রিক্ত)
	    // Skip a leading quote and (.
	    যেতিয়ালৈ (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	যদি (cmdbase == রিক্ত || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    ঘূৰাই x;
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

    যদি (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    যদি (cmd == রিক্ত)
    {
	x = mch_system((ৱর্ণ *)p_sh, options);
    }
    অন্যথা
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = রিক্ত;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	যদি (*cmdbase == '"' )
	    ++cmdbase;
	যদি (*cmdbase == '(')
	    ++cmdbase;

	যদি ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    ZeroMemory(&si, পরিমাণ(si));
	    si.cb = পরিমাণ(si);
	    si.lpReserved = রিক্ত;
	    si.lpDesktop = রিক্ত;
	    si.lpTitle = রিক্ত;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = রিক্ত;

	    cmdbase = skipwhite(cmdbase + 5);
	    যদি ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    অন্যথা যদি ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_WINDOW;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_প্ৰবেশ = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    রিক্ত,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_NORMAL,		// File att.
		    রিক্ত);				// Temp file
		si.hStdOutput = si.h_Std_প্ৰবেশ;
		si.hStdError = si.h_Std_প্ৰবেশ;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    যদি (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		যদি (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		যদি (p > cmdbase && p[-1] == ')'
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
	    যদি (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		যদি (cmd_shell == রিক্ত || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		যদি (subcmd != রিক্ত)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    যদি (newcmd != রিক্ত)
			vim_snprintf((ৱর্ণ *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    অন্যথা
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    যদি (vim_create_process((ৱর্ণ *)newcmd, FALSE, flags,
			&si, &pi, রিক্ত, রিক্ত))
		x = 0;
	    অন্যথা যদি (vim_shell_execute((ৱর্ণ *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    অন্যথা
	    {
		x = -1;
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
		যদি (gui.in_use)
# endif
		    emsg(_(e_command_not_found));
#endif
	    }

	    যদি (newcmd != cmdbase)
		vim_free(newcmd);

	    যদি (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_প্ৰবেশ != রিক্ত)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_প্ৰবেশ);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	অন্যথা
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
	    যদি (newcmd != রিক্ত)
	    {
#যদি defined(FEAT_GUI_MSWIN)
		যদি (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    ৱর্ণ *বাৰ্তা = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    ৱর্ণ *title = _("Vim Warning");
		    WCHAR *wmsg = enc_to_utf16((char_u *)বাৰ্তা, রিক্ত);
		    WCHAR *wtitle = enc_to_utf16((char_u *)title, রিক্ত);

		    যদি (wmsg != রিক্ত && wtitle != রিক্ত)
			MessageBoxW(রিক্ত, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		যদি (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((ৱর্ণ *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_path.string,
			    (বাৰ্তা_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		অন্যথা যদি (
# ifdef VIMDLL
			(gui.in_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((ৱর্ণ *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		অন্যথা
#endif
		    vim_snprintf((ৱর্ণ *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((ৱর্ণ *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    যদি (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    যদি (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#যদি defined(FEAT_GUI_MSWIN)
	    && ((gui.in_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#endif
	    )
    {
	smsg(_("shell returned %d"), x);
	বাৰ্তা_putchar('\n');
    }
    resettitle();

    mch_signal(SIGINT, SIG_DFL);
    mch_signal(SIGBREAK, SIG_DFL);
    mch_signal(SIGILL, SIG_DFL);
    mch_signal(SIGFPE, SIG_DFL);
    mch_signal(SIGSEGV, SIG_DFL);
    mch_signal(SIGTERM, SIG_DFL);
    mch_signal(SIGABRT, SIG_DFL);

    ঘূৰাই x;
}

#যদি defined(FEAT_JOB_CHANNEL)
    স্থির HANDLE
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

    wn = enc_to_utf16(fname, রিক্ত);
    যদি (wn == রিক্ত)
	ঘূৰাই INVALID_HANDLE_মান;

    h = CreateFileW(wn, dwDesiredAccess, dwShareMode,
	    lpSecurityAttributes, dwCreationDisposition,
	    dwFlagsAndAttributes, রিক্ত);
    vim_free(wn);
    ঘূৰাই h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    শূন্য
win32_build_env(dict_T *env, garray_T *gap, পূর্ণাঙ্ক is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != রিক্ত ? env->dv_hashtab.ht_used : 0;
    LPVOID	base = GetEnvironmentStringsW();

    // for last \0
    যদি (ga_grow(gap, 1) == FAIL)
	ঘূৰাই;

    যদি (env != রিক্ত)
    {
	FOR_ALL_HASHTAB_বস্তুসমূহ(&env->dv_hashtab, hi, todo)
	{
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		typval_T *item = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_to_utf16((char_u *)hi->hi_চাবি, রিক্ত);
		WCHAR   *wval = enc_to_utf16(tv_get_string(item), রিক্ত);
		--todo;
		যদি (wkey != রিক্ত && wval != রিক্ত)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    যদি (ga_grow(gap, (পূর্ণাঙ্ক)(lkey + lval + 2)) == FAIL)
			অনুৱর্ত;
		    বাবে (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'=';
		    বাবে (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    যদি (base)
    {
	WCHAR	*p = (WCHAR*) base;

	// for last \0
	যদি (ga_grow(gap, 1) == FAIL)
	    ঘূৰাই;

	যেতিয়ালৈ (*p != 0 || *(p + 1) != 0)
	{
	    যদি (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_data + gap->ga_len++) = *p;
	    p++;
	}
	FreeEnvironmentStringsW(base);
	*((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
    }

# যদি defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
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

	যদি (ga_grow(gap, (পূর্ণাঙ্ক)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    বাবে (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    বাবে (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    যদি (is_terminal)
	    {
		বাবে (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_data + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		বাবে (n = 0; n < version_len; n++)
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
    স্থির BOOL
create_pipe_pair(HANDLE handles[2])
{
    স্থির LONG		s;
    ৱর্ণ		name[64];
    SECURITY_ATTRIBUTES sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_SIZE, 0, 0, রিক্ত);

    যদি (handles[1] == INVALID_HANDLE_মান)
	ঘূৰাই FALSE;

    sa.nLength = পরিমাণ(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = রিক্ত;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    যদি (handles[0] == INVALID_HANDLE_মান)
    {
	CloseHandle(handles[1]);
	ঘূৰাই FALSE;
    }

    ঘূৰাই TRUE;
}

#endif // FEAT_JOB_CHANNEL

#যদি defined(FEAT_EVAL)
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
    ৱর্ণ	**argv,
    char_u	*infile,
    পূর্ণাঙ্ক		flags UNUSED,
    পূর্ণাঙ্ক		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;
    HANDLE		hChildStdoutRd = INVALID_HANDLE_মান;
    HANDLE		hChildStdoutWr = INVALID_HANDLE_মান;
    HANDLE		hChildStdinRd = INVALID_HANDLE_মান;
    garray_T		cmd_ga;
    garray_T		out_ga;
    char_u		*buffer = রিক্ত;
    DWORD		exit_code = (DWORD)-1;
    পূর্ণাঙ্ক			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    বাবে (i = 0; argv[i] != রিক্ত; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	পূর্ণাঙ্ক	has_spaces = FALSE;
	পূর্ণাঙ্ক	j;

	বাবে (j = 0; s[j] != NUL; j++)
	    যদি (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		ভঙ্গ;
	    }

	যদি (i > 0)
	    ga_append(&cmd_ga, ' ');

	যদি (has_spaces)
	{
	    পূর্ণাঙ্ক	অঙ্ক_bs;

	    ga_append(&cmd_ga, '"');
	    বাবে (j = 0; arg[j] != NUL; j++)
	    {
		অঙ্ক_bs = 0;
		যেতিয়ালৈ (arg[j] == '\\')
		{
		    অঙ্ক_bs++;
		    j++;
		}

		যদি (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    যেতিয়ালৈ (অঙ্ক_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ভঙ্গ;
		}
		অন্যথা যদি (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    যেতিয়ালৈ (অঙ্ক_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		অন্যথা
		{
		    যেতিয়ালৈ (অঙ্ক_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	অন্যথা
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&out_ga, 1, 4096);

    saAttr.nLength = পরিমাণ(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = রিক্ত;

    // Create a pipe for the child's stdout.
    যদি (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)
	    || !SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	গচ্ছ done;
    }

    // Set up stdin from infile if provided.
    যদি (infile != রিক্ত)
    {
	WCHAR *winfile = enc_to_utf16(infile, রিক্ত);

	যদি (winfile != রিক্ত)
	{
	    hChildStdinRd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL, রিক্ত);
	    vim_free(winfile);
	}
    }

    ZeroMemory(&pi, পরিমাণ(pi));
    ZeroMemory(&si, পরিমাণ(si));
    si.cb = পরিমাণ(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;
    si.h_Std_প্ৰবেশ = (hChildStdinRd != INVALID_HANDLE_মান)
		    ? hChildStdinRd : INVALID_HANDLE_মান;

    ch_log(রিক্ত, "directly executing: %s", (ৱর্ণ *)cmd_ga.ga_data);

    // Create the child process directly, without going through the shell.
    যদি (!vim_create_process((ৱর্ণ *)cmd_ga.ga_data, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
		&si, &pi, রিক্ত, রিক্ত))
    {
	semsg(_(e_invalid_argument_str), cmd_ga.ga_data);
	গচ্ছ done;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(hChildStdoutWr);
    hChildStdoutWr = INVALID_HANDLE_মান;
    যদি (hChildStdinRd != INVALID_HANDLE_মান)
    {
	CloseHandle(hChildStdinRd);
	hChildStdinRd = INVALID_HANDLE_মান;
    }

    // Read output from child process.
    বাবে (;;)
    {
	ৱর্ণ	buf[4096];
	DWORD	n;

	যদি (!ReadFile(hChildStdoutRd, buf, পরিমাণ(buf), &n, রিক্ত) || n == 0)
	    ভঙ্গ;
	যদি (ga_grow(&out_ga, (পূর্ণাঙ্ক)n) == OK)
	{
	    mch_memmove((ৱর্ণ *)out_ga.ga_data + out_ga.ga_len, buf, n);
	    out_ga.ga_len += (পূর্ণাঙ্ক)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    set_vim_var_nr(VV_SHELL_ERROR, (long)exit_code);

    যদি (out_ga.ga_len > 0)
    {
	buffer = alloc(out_ga.ga_len + 1);
	যদি (buffer != রিক্ত)
	{
	    mch_memmove(buffer, out_ga.ga_data, out_ga.ga_len);
	    যদি (ret_len == রিক্ত)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		বাবে (i = 0; i < out_ga.ga_len; ++i)
		    যদি (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[out_ga.ga_len] = NUL;
	    }
	    অন্যথা
		*ret_len = out_ga.ga_len;
	}
    }
    অন্যথা যদি (ret_len != রিক্ত)
	*ret_len = 0;

done:
    ga_clear(&cmd_ga);
    ga_clear(&out_ga);
    যদি (hChildStdoutRd != INVALID_HANDLE_মান)
	CloseHandle(hChildStdoutRd);
    যদি (hChildStdoutWr != INVALID_HANDLE_মান)
	CloseHandle(hChildStdoutWr);
    যদি (hChildStdinRd != INVALID_HANDLE_মান)
	CloseHandle(hChildStdinRd);
    ঘূৰাই buffer;
}
#endif // FEAT_EVAL

#যদি defined(FEAT_JOB_CHANNEL)
    শূন্য
mch_job_start(ৱর্ণ *cmd, job_T *job, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_ATTRIBUTES saAttr;
    channel_T		*channel = রিক্ত;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    পূর্ণাঙ্ক		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    পূর্ণাঙ্ক		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    পূর্ণাঙ্ক		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    পূর্ণাঙ্ক		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    পূর্ণাঙ্ক		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    পূর্ণাঙ্ক		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    পূর্ণাঙ্ক		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;

    যদি (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_মান;
    ifd[1] = INVALID_HANDLE_মান;
    ofd[0] = INVALID_HANDLE_মান;
    ofd[1] = INVALID_HANDLE_মান;
    efd[0] = INVALID_HANDLE_মান;
    efd[1] = INVALID_HANDLE_মান;
    ga_init2(&ga, পরিমাণ(wchar_t), 500);

    jo = CreateJobObject(রিক্ত, রিক্ত);
    যদি (jo == রিক্ত)
    {
	job->jv_status = JOB_FAILED;
	গচ্ছ failed;
    }

    যদি (options->jo_env != রিক্ত)
	win32_build_env(options->jo_env, &ga, FALSE);

    ZeroMemory(&pi, পরিমাণ(pi));
    ZeroMemory(&si, পরিমাণ(si));
    si.cb = পরিমাণ(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    saAttr.nLength = পরিমাণ(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = রিক্ত;

    যদি (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	ifd[0] = job_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
	যদি (ifd[0] == INVALID_HANDLE_মান)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    গচ্ছ failed;
	}
    }
    অন্যথা যদি (!use_null_for_in
	    && (!create_pipe_pair(ifd)
		|| !SetHandleInformation(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	গচ্ছ failed;

    যদি (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	ofd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	যদি (ofd[1] == INVALID_HANDLE_মান)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    গচ্ছ failed;
	}
    }
    অন্যথা যদি (!use_null_for_out &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !SetHandleInformation(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	গচ্ছ failed;

    যদি (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	যদি (efd[1] == INVALID_HANDLE_মান)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    গচ্ছ failed;
	}
    }
    অন্যথা যদি (!use_out_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !SetHandleInformation(efd[0], HANDLE_FLAG_INHERIT, 0)))
	গচ্ছ failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_প্ৰবেশ = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_out_for_err ? ofd[1] : efd[1];

    যদি (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	যদি (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    যদি (channel != রিক্ত)
		++channel->ch_refcount;
	}
	অন্যথা
	    channel = add_channel();
	যদি (channel == রিক্ত)
	    গচ্ছ failed;
    }

    যদি (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_MODE |
	    CREATE_NEW_PROCESS_GROUP |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_data,
	    (ৱর্ণ *)options->jo_cwd))
    {
	CloseHandle(jo);
	job->jv_status = JOB_FAILED;
	গচ্ছ failed;
    }

    ga_clear(&ga);

    যদি (!AssignProcessToJobObject(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = রিক্ত;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    job->jv_proc_info = pi;
    job->jv_job_object = jo;
    job->jv_status = JOB_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    যদি (!use_out_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    job->jv_channel = channel;
    যদি (channel != রিক্ত)
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
    ঘূৰাই;

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

    ৱর্ণ *
mch_job_status(job_T *job)
{
    DWORD dwExitCode = 0;

    যদি (!GetExitCodeProcess(job->jv_proc_info.hProcess, &dwExitCode)
	    || dwExitCode != STILL_ACTIVE)
    {
	job->jv_exitval = (পূর্ণাঙ্ক)dwExitCode;
	যদি (job->jv_status < JOB_ENDED)
	{
	    ch_log(job->jv_channel, "Job ended");
	    job->jv_status = JOB_ENDED;
	}
	ঘূৰাই "dead";
    }
    ঘূৰাই "run";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    HANDLE jobHandles[MAXIMUM_WAIT_OBJECTS];
    job_T *jobArray[MAXIMUM_WAIT_OBJECTS];
    job_T *job = job_list;

    যেতিয়ালৈ (job != রিক্ত)
    {
	DWORD n;
	DWORD result;

	বাবে (n = 0; n < MAXIMUM_WAIT_OBJECTS
				       && job != রিক্ত; job = job->jv_next)
	{
	    যদি (job->jv_status == JOB_STARTED)
	    {
		jobHandles[n] = job->jv_proc_info.hProcess;
		jobArray[n] = job;
		++n;
	    }
	}
	যদি (n == 0)
	    অনুৱর্ত;
	result = WaitForMultipleObjects(n, jobHandles, FALSE, 0);
	যদি (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + n)
	{
	    job_T *wait_job = jobArray[result - WAIT_OBJECT_0];

	    যদি (STRCMP(mch_job_status(wait_job), "dead") == 0)
		ঘূৰাই wait_job;
	}
    }
    ঘূৰাই রিক্ত;
}

    স্থির BOOL
terminate_all(HANDLE process, পূর্ণাঙ্ক code)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_মান;
    DWORD	    pid = GetProcessId(process);

    যদি (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	যদি (h != INVALID_HANDLE_মান)
	{
	    pe.dwSize = পরিমাণ(PROCESSENTRY32);
	    যদি (!Process32First(h, &pe))
		গচ্ছ theend;

	    কৰা
	    {
		যদি (pe.th32ParentProcessID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    যদি (ph != রিক্ত)
		    {
			terminate_all(ph, code);
			CloseHandle(ph);
		    }
		}
	    } যেতিয়ালৈ (Process32Next(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    ঘূৰাই TerminateProcess(process, code);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    পূর্ণাঙ্ক
mch_signal_job(job_T *job, char_u *how)
{
    পূর্ণাঙ্ক ret;

    যদি (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	যদি (job->jv_job_object != রিক্ত)
	{
	    যদি (job->jv_channel != রিক্ত && job->jv_channel->ch_anonymous_pipe)
		job->jv_channel->ch_killing = TRUE;
	    ঘূৰাই TerminateJobObject(job->jv_job_object, (UINT)-1) ? OK : FAIL;
	}
	ঘূৰাই terminate_all(job->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    যদি (!AttachConsole(job->jv_proc_info.dwProcessId))
	ঘূৰাই FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		job->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    ঘূৰাই ret;
}

/*
 * Clear the data related to "job".
 */
    শূন্য
mch_clear_job(job_T *job)
{
    যদি (job->jv_status == JOB_FAILED)
	ঘূৰাই;

    যদি (job->jv_job_object != রিক্ত)
	CloseHandle(job->jv_job_object);
    CloseHandle(job->jv_proc_info.hProcess);
}
#endif


#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    স্থির শূন্য
termcap_mode_start(শূন্য)
{
    DWORD cmodein;

    যদি (g_fTermcapMode)
	ঘূৰাই;

    SaveConsoleBuffer(&g_cbNonTermcap);

    যদি (g_cbTermcap.IsValid)
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
    অন্যথা
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
    যদি (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_প্ৰবেশ;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    অন্যথা
    {
	cmodein &= ~ENABLE_MOUSE_প্ৰবেশ;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }
    cmodein |= ENABLE_WINDOW_প্ৰবেশ;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_fTermcapMode = TRUE;
}


/*
 * End termcap mode
 */
    স্থির শূন্য
termcap_mode_end(শূন্য)
{
    DWORD cmodein;
    ConsoleBuffer *cb;
    COORD coord;
    DWORD dwDummy;

    যদি (!g_fTermcapMode)
	ঘূৰাই;

    SaveConsoleBuffer(&g_cbTermcap);

    GetConsoleMode(g_hConIn, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_প্ৰবেশ | ENABLE_WINDOW_প্ৰবেশ);
    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# অন্যথা
    cb = &g_cbNonTermcap;
# endif
    RestoreConsoleBuffer(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    যদি (exiting && use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    যদি (!USE_WT && (p_rs || exiting))
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
	যদি (exiting && !p_rs)
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


#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    স্থির শূন্য
clear_chars(
    COORD coord,
    DWORD n)
{
    যদি (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_hConOut, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    অন্যথা
    {
	set_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    স্থির শূন্য
clear_screen(শূন্য)
{
    g_coord.X = g_coord.Y = 0;

    যদি (!vtp_working)
	clear_chars(g_coord, Rows * Columns);
    অন্যথা
    {
	set_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    স্থির শূন্য
clear_to_end_of_display(শূন্য)
{
    COORD save = g_coord;

    যদি (!vtp_working)
	clear_chars(g_coord, (Rows - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    অন্যথা
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
    স্থির শূন্য
clear_to_end_of_সাৰি(শূন্য)
{
    COORD save = g_coord;

    যদি (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    অন্যথা
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
    স্থির শূন্য
scroll(unsigned c_সাৰিগুলি)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.বাম_3 + 1, g_srScrollRegion.শীৰ্ষ_2 + 1);
    delete_সাৰিগুলি(c_সাৰিগুলি);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    স্থির শূন্য
set_scroll_region(
    unsigned বাম,
    unsigned শীৰ্ষ,
    unsigned ডান,
    unsigned bottom)
{
    যদি (বাম >= ডান
	    || শীৰ্ষ >= bottom
	    || ডান > (unsigned) Columns - 1
	    || bottom > (unsigned) Rows - 1)
	ঘূৰাই;

    g_srScrollRegion.বাম_3 =   বাম;
    g_srScrollRegion.শীৰ্ষ_2 =    শীৰ্ষ;
    g_srScrollRegion.ডান_3 =  ডান;
    g_srScrollRegion.Bottom = bottom;
}

    স্থির শূন্য
set_scroll_region_tb(
    unsigned শীৰ্ষ,
    unsigned bottom)
{
    যদি (শীৰ্ষ >= bottom || bottom > (unsigned)Rows - 1)
	ঘূৰাই;

    g_srScrollRegion.শীৰ্ষ_2 = শীৰ্ষ;
    g_srScrollRegion.Bottom = bottom;
}

    স্থির শূন্য
set_scroll_region_lr(
    unsigned বাম,
    unsigned ডান)
{
    যদি (বাম >= ডান || ডান > (unsigned)Columns - 1)
	ঘূৰাই;

    g_srScrollRegion.বাম_3 = বাম;
    g_srScrollRegion.ডান_3 = ডান;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    স্থির শূন্য
ঢোকাও_সাৰিগুলি(unsigned c_সাৰিগুলি)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.বাম_3 + 1, g_srScrollRegion.শীৰ্ষ_2 + 1);

    dest.X = g_srScrollRegion.বাম_3;
    dest.Y = g_coord.Y + c_সাৰিগুলি;

    source.বাম_3   = g_srScrollRegion.বাম_3;
    source.শীৰ্ষ_2	  = g_coord.Y;
    source.ডান_3  = g_srScrollRegion.ডান_3;
    source.Bottom = g_srScrollRegion.Bottom - c_সাৰিগুলি;

    clip.বাম_3   = g_srScrollRegion.বাম_3;
    clip.শীৰ্ষ_2    = g_coord.Y;
    clip.ডান_3  = g_srScrollRegion.ডান_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    যদি (!USE_VTP)
	fill.Attributes = g_attrCurrent;
    অন্যথা
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    যদি (source.Bottom < dest.Y)
    {
	COORD coord;
	পূর্ণাঙ্ক   i;

	coord.X = source.বাম_3;
	বাবে (i = clip.শীৰ্ষ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ডান_3 - source.বাম_3 + 1);
	}
    }

    যদি (vtp_working)
    {
	COORD coord;
	পূর্ণাঙ্ক i;

	coord.X = source.বাম_3;
	বাবে (i = source.শীৰ্ষ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ডান_3 - source.বাম_3 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    স্থির শূন্য
delete_সাৰিগুলি(unsigned c_সাৰিগুলি)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    পূর্ণাঙ্ক		    nb;

    gotoxy(g_srScrollRegion.বাম_3 + 1, g_srScrollRegion.শীৰ্ষ_2 + 1);

    dest.X = g_srScrollRegion.বাম_3;
    dest.Y = g_coord.Y;

    source.বাম_3   = g_srScrollRegion.বাম_3;
    source.শীৰ্ষ_2	  = g_coord.Y + c_সাৰিগুলি;
    source.ডান_3  = g_srScrollRegion.ডান_3;
    source.Bottom = g_srScrollRegion.Bottom;

    clip.বাম_3   = g_srScrollRegion.বাম_3;
    clip.শীৰ্ষ_2    = g_coord.Y;
    clip.ডান_3  = g_srScrollRegion.ডান_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    যদি (!vtp_working)
	fill.Attributes = g_attrCurrent;
    অন্যথা
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (source.Bottom - source.শীৰ্ষ_2) + 1;

    যদি (nb < source.শীৰ্ষ_2)
    {
	COORD coord;
	পূর্ণাঙ্ক   i;

	coord.X = source.বাম_3;
	বাবে (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ডান_3 - source.বাম_3 + 1);
	}
    }

    যদি (vtp_working)
    {
	COORD coord;
	পূর্ণাঙ্ক i;

	coord.X = source.বাম_3;
	বাবে (i = nb; i <= source.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ডান_3 - source.বাম_3 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    স্থির শূন্য
gotoxy(
    unsigned x,
    unsigned y)
{
    যদি (x < 1 || x > (unsigned)Columns || y < 1 || y > (unsigned)Rows)
	ঘূৰাই;

    যদি (!USE_VTP)
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
    অন্যথা
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
    স্থির শূন্য
textattr(WORD wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    SetConsoleTextAttribute(g_hConOut, wAttr);
}


    স্থির শূন্য
textcolor(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    যদি (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    অন্যথা
	vtp_sgr_bulk(wAttr);
}


    স্থির শূন্য
textbackground(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    যদি (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    অন্যথা
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    স্থির শূন্য
normvideo(শূন্য)
{
    যদি (!vtp_working)
	textattr(g_attrDefault);
    অন্যথা
	vtp_sgr_bulk(0);
}


স্থির WORD g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    স্থির শূন্য
standout(শূন্য)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((WORD) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    স্থির শূন্য
standend(শূন্য)
{
    যদি (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    শূন্য
mch_set_normal_colors(শূন্য)
{
    char_u	*p;
    পূর্ণাঙ্ক		n;

    cterm_normal_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_normal_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    যদি (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	যদি (*p == 'm' && n > 0)
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
    স্থির শূন্য
visual_bell(শূন্য)
{
    COORD   coordOrigin = {0, 0};
    WORD    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = রিক্ত;

# ifdef FEAT_TERMGUICOLORS
    যদি (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(WORD, Rows * Columns);
	যদি (oldattrs == রিক্ত)
	    ঘূৰাই;
	ReadConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_hConOut, attrFlash, Rows * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    যদি (oldattrs != রিক্ত)
    {
	WriteConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    স্থির শূন্য
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    যদি (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	ঘূৰাই;
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
    স্থির DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cbToWrite)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    স্থির DWORD    cells;
    স্থির WCHAR    *unicodebuf = রিক্ত;
    স্থির পূর্ণাঙ্ক	    unibuflen = 0;
    স্থির পূর্ণাঙ্ক	    length;
    পূর্ণাঙ্ক		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    স্থির WCHAR    *utf8spbuf = রিক্ত;
    স্থির পূর্ণাঙ্ক	    utf8splength;
    স্থির DWORD    utf8spcells;
    স্থির WCHAR    **utf8usingbuf = &unicodebuf;

    যদি (cbToWrite != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	কৰা
	{
	    length = MultiByteToWideChar(cp, 0, (LPCSTR)pchBuf, cbToWrite,
							unicodebuf, unibuflen);
	    যদি (length && length <= unibuflen)
		ভঙ্গ;
	    vim_free(unicodebuf);
	    unicodebuf = length ? LALLOC_MULT(WCHAR, length) : রিক্ত;
	    unibuflen = unibuflen ? 0 : length;
	} যেতিয়ালৈ (TRUE);
	cells = mb_string2cells(pchBuf, cbToWrite);
    }
    অন্যথা // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	যদি (utf8usingbuf != &utf8spbuf)
	{
	    যদি (utf8spbuf == রিক্ত)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		length = MultiByteToWideChar(CP_UTF8, 0, " ", 1, রিক্ত, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, length);
		যদি (utf8spbuf != রিক্ত)
		{
		    MultiByteToWideChar(CP_UTF8, 0, " ", 1, utf8spbuf, length);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = length;
		    utf8spcells = cells;
		}
	    }
	    অন্যথা
	    {
		utf8usingbuf = &utf8spbuf;
		length = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    যদি (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	যদি (WriteConsoleOutputCharacterW(g_hConOut, *utf8usingbuf, length,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    অন্যথা
    {
	যদি (WriteConsoleW(g_hConOut, *utf8usingbuf, length, &cchwritten,
		    রিক্ত) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    যদি (cchwritten == (DWORD)length)
    {
	written = cbToWrite;
	g_coord.X += (SHORT)cells;
    }
    অন্যথা
    {
	char_u *p = pchBuf;
	বাবে (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    যেতিয়ালৈ (g_coord.X > g_srScrollRegion.ডান_3)
    {
	g_coord.X -= (SHORT) Columns;
	যদি (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    যদি (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    ঘূৰাই written;
}

    স্থির char_u *
get_seq(
    পূর্ণাঙ্ক *args,
    পূর্ণাঙ্ক *গুনতি,
    char_u *head)
{
    পূর্ণাঙ্ক argc;
    char_u *p;

    যদি (head == রিক্ত || *head != '\033')
	ঘূৰাই রিক্ত;

    argc = 0;
    p = head;
    ++p;
    কৰা
    {
	++p;
	args[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } যেতিয়ালৈ (*p == ';');
    *গুনতি = argc;

    ঘূৰাই p;
}

    স্থির char_u *
get_sgr(
    পূর্ণাঙ্ক *args,
    পূর্ণাঙ্ক *গুনতি,
    char_u *head)
{
    char_u *p = get_seq(args, গুনতি, head);

    ঘূৰাই (p && *p == 'm') ? ++p : রিক্ত;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    স্থির char_u *
sgrn2(
    char_u *head,
    পূর্ণাঙ্ক	   n)
{
    পূর্ণাঙ্ক argc;
    পূর্ণাঙ্ক args[16];
    char_u *p = get_sgr(args, &argc, head);

    ঘূৰাই p && argc == 5 && args[0] == n && args[1] == 2 ? p : রিক্ত;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    স্থির char_u *
sgrnc(
    char_u *head,
    পূর্ণাঙ্ক	   n)
{
    পূর্ণাঙ্ক argc;
    পূর্ণাঙ্ক args[16];
    char_u *p = get_sgr(args, &argc, head);

    ঘূৰাই p && argc == 1 && args[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : রিক্ত;
}

    স্থির char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    যেতিয়ালৈ (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    ঘূৰাই p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    স্থির char_u *
sgrn2c(
    char_u *head,
    পূর্ণাঙ্ক	   n)
{
    char_u *p = sgrn2(head, n);

    ঘূৰাই p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : রিক্ত;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    স্থির char_u *
sgrn2cn(
    char_u *head,
    পূর্ণাঙ্ক n)
{
    char_u *p = sgrn2(head, n);

    ঘূৰাই p && p[0] == 0x0a && p[1] == '\033' ? ++p : রিক্ত;
}

স্থির bool in_dcs = false;	// inside a DCS (sixel) sequence
স্থির bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    স্থির শূন্য
write_vt(char_u *s, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, রিক্ত, 0);
    যদি (wlen <= 0)
	ঘূৰাই;
    ws = ALLOC_MULT(WCHAR, wlen);
    যদি (ws == রিক্ত)
	ঘূৰাই;
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_hConOut, ws, wlen, &written, রিক্ত);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    স্থির char_u *
skip_dcs(char_u *s, char_u *end)
{
    বাবে (char_u *p = s; p < end; ++p)
    {
	যদি (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    in_dcs = false;
	    ঘূৰাই p + 1;
	}
	dcs_esc = *p == ESC;
    }
    ঘূৰাই end;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    শূন্য
mch_write(
    char_u  *s,
    পূর্ণাঙ্ক	    len)
{
    char_u  *end = s + len;

# ifdef VIMDLL
    যদি (gui.in_use)
	ঘূৰাই;
# endif

    যদি (!term_console)
    {
	লিখ(1, s, (unsigned)len);
	ঘূৰাই;
    }

    // translate ESC | sequences into faked bios calls
    যেতিয়ালৈ (len--)
    {
	পূর্ণাঙ্ক prefix = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	যদি (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    ঘূৰাই;
	}

	যদি (in_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    পূর্ণাঙ্ক	l = (পূর্ণাঙ্ক)(skip_dcs(s, end) - s);

	    যদি (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    অনুৱর্ত;
	}

	যেতিয়ালৈ (s + ++prefix < end)
	{
	    ch = s[prefix];
	    যদি (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		ভঙ্গ;
	}

	যদি (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    যদি (prefix != 0)
		prefix = 1;
	}

	যদি (prefix != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, prefix);
# ifdef MCH_WRITE_DUMP
	    যদি (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, পরিমাণ(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	অন্যথা যদি (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    যদি (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.বাম_3 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    অন্যথা
	    {
		gotoxy(g_srScrollRegion.বাম_3 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    যদি (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	অন্যথা যদি (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.বাম_3+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    যদি (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	অন্যথা যদি (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    যদি (g_coord.X > g_srScrollRegion.বাম_3)
		g_coord.X--;
	    অন্যথা যদি (g_coord.Y > g_srScrollRegion.শীৰ্ষ_2)
	    {
		g_coord.X = g_srScrollRegion.ডান_3;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    যদি (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	অন্যথা যদি (s[0] == '\a')
	{
	    // \a, bell
	    MessageBeep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    যদি (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	অন্যথা যদি (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    পূর্ণাঙ্ক	    arg1 = 0, arg2 = 0, argc = 0, args[16];
	    char_u  *sp;

	    ৱিকল্প (s[2])
	    {
	    পক্ষ '0': পক্ষ '1': পক্ষ '2': পক্ষ '3': পক্ষ '4':
	    পক্ষ '5': পক্ষ '6': পক্ষ '7': পক্ষ '8': পক্ষ '9':
		যদি (*(p = get_seq(args, &argc, s)) != 'm')
		    গচ্ছ notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		যদি (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (পূর্ণাঙ্ক)(p - s);
		    s = p;
		    ভঙ্গ;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		যদি (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		যদি (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		যদি (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		যদি (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(args, &argc, p);

notsgr:
		arg1 = args[0];
		arg2 = args[1];
		যদি (*p == 'm')
		{
		    যদি (argc == 1 && args[0] == 0)
			normvideo();
		    অন্যথা যদি (argc == 1)
		    {
			যদি (USE_VTP)
			    textcolor((WORD)arg1);
			অন্যথা
			    textattr((WORD)arg1);
		    }
		    অন্যথা যদি (vtp_working)
			vtp_sgr_bulks(argc, args);
		}
		অন্যথা যদি (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		অন্যথা যদি (argc == 2 && *p == 'r')
		{
		    set_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		অন্যথা যদি (argc == 2 && *p == 'R')
		{
		    set_scroll_region_tb(arg1, arg2);
		}
		অন্যথা যদি (argc == 2 && *p == 'V')
		{
		    set_scroll_region_lr(arg1, arg2);
		}
		অন্যথা যদি (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.শীৰ্ষ_2, g_coord.Y - arg1) + 1);
		}
		অন্যথা যদি (argc == 1 && *p == 'b')
		{
		    textbackground((WORD) arg1);
		}
		অন্যথা যদি (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.ডান_3, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		অন্যথা যদি (argc == 1 && *p == 'f')
		{
		    textcolor((WORD) arg1);
		}
		অন্যথা যদি (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		অন্যথা যদি (argc == 1 && *p == 'L')
		{
		    ঢোকাও_সাৰিগুলি(arg1);
		}
		অন্যথা যদি (argc == 1 && *p == 'M')
		{
		    delete_সাৰিগুলি(arg1);
		}

		len -= (পূর্ণাঙ্ক)(p - s);
		s = p + 1;
		ভঙ্গ;

	    পক্ষ 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.শীৰ্ষ_2, g_coord.Y - 1) + 1);
		গচ্ছ got3;

	    পক্ষ 'B':
		visual_bell();
		গচ্ছ got3;

	    পক্ষ 'C':
		gotoxy(min(g_srScrollRegion.ডান_3, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		গচ্ছ got3;

	    পক্ষ 'E':
		termcap_mode_end();
		গচ্ছ got3;

	    পক্ষ 'F':
		standout();
		গচ্ছ got3;

	    পক্ষ 'f':
		standend();
		গচ্ছ got3;

	    পক্ষ 'H':
		gotoxy(1, 1);
		গচ্ছ got3;

	    পক্ষ 'j':
		clear_to_end_of_display();
		গচ্ছ got3;

	    পক্ষ 'J':
		clear_screen();
		গচ্ছ got3;

	    পক্ষ 'K':
		clear_to_end_of_সাৰি();
		গচ্ছ got3;

	    পক্ষ 'L':
		ঢোকাও_সাৰিগুলি(1);
		গচ্ছ got3;

	    পক্ষ 'M':
		delete_সাৰিগুলি(1);
		গচ্ছ got3;

	    পক্ষ 'S':
		termcap_mode_start();
		গচ্ছ got3;

	    পক্ষ 'V':
		cursor_visible(TRUE);
		গচ্ছ got3;

	    পক্ষ 'v':
		cursor_visible(FALSE);
		গচ্ছ got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    যদি (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, পরিমাণ(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	অন্যথা যদি (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    পূর্ণাঙ্ক l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    যেতিয়ালৈ (s + l < end && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    যদি (s + l < end && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		যদি (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	অন্যথা যদি (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    পূর্ণাঙ্ক	l;

	    in_dcs = true;
	    dcs_esc = false;
	    l = (পূর্ণাঙ্ক)(skip_dcs(s + 2, end) - s);
	    যদি (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	অন্যথা
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    যদি (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, পরিমাণ(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    যদি (fdDump)
	fflush(fdDump);
# endif
}

#অন্যথা // FEAT_GUI_MSWIN && !VIMDLL
    শূন্য
mch_write(
    char_u  *s UNUSED,
    পূর্ণাঙ্ক	    len UNUSED)
{
    // never used
}

#endif


/*
 * Delay for "msec" milliseconds.
 */
    শূন্য
mch_delay(
    long    msec,
    পূর্ণাঙ্ক	    flags UNUSED)
{
#যদি defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((পূর্ণাঙ্ক)msec);	    // never wait for input
#অন্যথা // Console
# ifdef VIMDLL
    যদি (gui.in_use)
    {
	Sleep((পূর্ণাঙ্ক)msec);	    // never wait for input
	ঘূৰাই;
    }
# endif
    যদি (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	যদি (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    পূর্ণাঙ্ক towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    যেতিয়ালৈ (msec > 0)
	    {
		mzvim_check_threads();
		যদি (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	অন্যথা
# endif
	    Sleep((পূর্ণাঙ্ক)msec);
    অন্যথা
	WaitForChar(msec, FALSE);
#endif
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    পূর্ণাঙ্ক
mch_remove(char_u *name)
{
    WCHAR	*wn;
    পূর্ণাঙ্ক		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    যদি (mch_isdir(name) && mch_is_symbolic_link(name))
	ঘূৰাই mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_NORMAL);

    wn = enc_to_utf16(name, রিক্ত);
    যদি (wn == রিক্ত)
	ঘূৰাই -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    ঘূৰাই n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    শূন্য
mch_breakcheck(পূর্ণাঙ্ক force UNUSED)
{
#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    যদি (!gui.in_use)
# endif
	যদি (g_fCtrlCPressed || g_fCBrkPressed)
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
mch_total_mem(পূর্ণাঙ্ক special UNUSED)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dwLength = পরিমাণ(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&ms);
    যদি (ms.ullAvailVirtual < ms.ullTotalPhys)
    {
	// Process address space fits in physical RAM, use all of it.
	ঘূৰাই (long_u)(ms.ullAvailVirtual / 1024);
    }
    যদি (ms.ullTotalPhys <= WINNT_RESERVE_BYTES)
    {
	// Catch old NT box or perverse hardware setup.
	ঘূৰাই (long_u)((ms.ullTotalPhys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    ঘূৰাই (long_u)((ms.ullTotalPhys - WINNT_RESERVE_BYTES) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    পূর্ণাঙ্ক
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    পূর্ণাঙ্ক		flags)		// EW_* flags
{
    ঘূৰাই dos_expandpath(gap, path, 0, flags, FALSE);
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
    পূর্ণাঙ্ক
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	szTempFile[_MAX_PATH + 1];
    WCHAR	szNewPath[_MAX_PATH + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    বাবে (p = q = wold; *p != L'\0'; ++p)
    {
	ৱিকল্প (*p)
	{
	পক্ষ L'/':
	পক্ষ L'\\':
	পক্ষ L':':
	    যদি (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    ভঙ্গ;

	স্ৱভাৱ:
	    ভঙ্গ;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    যদি ((পূর্ণাঙ্ক)(p - q) < 8 || q[6] != L'~')
	ঘূৰাই (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    যদি (GetFullPathNameW(wnew, _MAX_PATH, szNewPath, &p) == 0 || p == রিক্ত)
	ঘূৰাই -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    যদি (GetTempFileNameW(szNewPath, L"VIM", 0, szTempFile) == 0)
	ঘূৰাই -2;

    // blow the temp file away
    যদি (!DeleteFileW(szTempFile))
	ঘূৰাই -3;

    // rename old file to the temp file
    যদি (!MoveFileW(wold, szTempFile))
	ঘূৰাই -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    যদি ((hf = CreateFileW(wold, GENERIC_WRITE, 0, রিক্ত, CREATE_NEW,
		    FILE_ATTRIBUTE_NORMAL, রিক্ত)) == INVALID_HANDLE_মান)
	ঘূৰাই -5;
    যদি (!CloseHandle(hf))
	ঘূৰাই -6;

    // rename the temp file to the new file
    যদি (!MoveFileW(szTempFile, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(শূন্য)MoveFileW(szTempFile, wold);
	ঘূৰাই -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(szTempFile);

    // finally, remove the empty old file
    যদি (!DeleteFileW(wold))
	ঘূৰাই -8;

    ঘূৰাই 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    পূর্ণাঙ্ক
mch_rename(
    ধ্রুৱ ৱর্ণ	*pszOldFile,
    ধ্রুৱ ৱর্ণ	*pszNewFile)
{
    WCHAR	*wold = রিক্ত;
    WCHAR	*wnew = রিক্ত;
    পূর্ণাঙ্ক		retval = -1;

    wold = enc_to_utf16((char_u *)pszOldFile, রিক্ত);
    wnew = enc_to_utf16((char_u *)pszNewFile, রিক্ত);
    যদি (wold != রিক্ত && wnew != রিক্ত)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    ঘূৰাই retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    ৱর্ণ *
default_shell(শূন্য)
{
    ঘূৰাই "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    পূর্ণাঙ্ক
mch_access(ৱর্ণ *n, পূর্ণাঙ্ক p)
{
    HANDLE	hFile;
    পূর্ণাঙ্ক		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_to_utf16((char_u *)n, রিক্ত);
    যদি (wn == রিক্ত)
	ঘূৰাই -1;

    যদি (mch_isdir((char_u *)n))
    {
	WCHAR TempNameW[_MAX_PATH + 16] = L"";

	যদি (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    পূর্ণাঙ্ক			i;
	    WIN32_FIND_DATAW    d;

	    বাবে (i = 0; i < _MAX_PATH && wn[i] != 0; ++i)
		TempNameW[i] = wn[i];
	    যদি (TempNameW[i - 1] != '\\' && TempNameW[i - 1] != '/')
		TempNameW[i++] = '\\';
	    TempNameW[i++] = '*';
	    TempNameW[i++] = 0;

	    hFile = FindFirstFileW(TempNameW, &d);
	    যদি (hFile == INVALID_HANDLE_মান)
		গচ্ছ getout;
	    অন্যথা
		(শূন্য)FindClose(hFile);
	}

	যদি (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    যদি (!GetTempFileNameW(wn, L"VIM", 0, TempNameW))
		গচ্ছ getout;
	    অন্যথা
		DeleteFileW(TempNameW);
	}
    }
    অন্যথা
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_mode = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_mode, share_mode,
					     রিক্ত, OPEN_EXISTING, 0, রিক্ত);
	যদি (hFile == INVALID_HANDLE_মান)
	    গচ্ছ getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    ঘূৰাই retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    পূর্ণাঙ্ক
mch_open(ধ্রুৱ ৱর্ণ *name, পূর্ণাঙ্ক flags, পূর্ণাঙ্ক mode)
{
    WCHAR	*wn;
    পূর্ণাঙ্ক		f;

    wn = enc_to_utf16((char_u *)name, রিক্ত);
    যদি (wn == রিক্ত)
	ঘূৰাই -1;

    f = _wopen(wn, flags, mode);
    vim_free(wn);
    ঘূৰাই f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ ৱর্ণ *mode)
{
    WCHAR	*wn, *wm;
    FILE	*f = রিক্ত;

#যদি defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    ৱর্ণ newMode = mode[strlen(mode) - 1];
    পূর্ণাঙ্ক oldMode = 0;

    _get_fmode(&oldMode);
    যদি (newMode == 't')
	_set_fmode(_O_TEXT);
    অন্যথা যদি (newMode == 'b')
	_set_fmode(O_দ্বিমিক_2);
#endif
    wn = enc_to_utf16((char_u *)name, রিক্ত);
    wm = enc_to_utf16((char_u *)mode, রিক্ত);
    যদি (wn != রিক্ত && wm != রিক্ত)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#যদি defined(DEBUG) && _MSC_VER >= 1400
    যদি (oldMode != 0)
	_set_fmode(oldMode);
#endif
    ঘূৰাই f;
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
    স্থির শূন্য
copy_substream(HANDLE sh, শূন্য *context, WCHAR *to, WCHAR *substream, long len)
{
    HANDLE  hTo;
    WCHAR   *to_name;

    to_name = alloc((wcslen(to) + wcslen(substream) + 1) * পরিমাণ(WCHAR));
    যদি (to_name == রিক্ত)
	ঘূৰাই;

    wcscpy(to_name, to);
    wcscat(to_name, substream);

    hTo = CreateFileW(to_name, GENERIC_WRITE, 0, রিক্ত, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL, রিক্ত);
    যদি (hTo != INVALID_HANDLE_মান)
    {
	long	done;
	DWORD	todo;
	DWORD	readcnt, written;
	ৱর্ণ	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	বাবে (done = 0; done < len; done += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - done) > পরিমাণ(buf) ? পরিমাণ(buf)
						       : (size_t)(len - done));
	    যদি (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(hTo, buf, todo, &written, রিক্ত)
		    || written != todo)
		ভঙ্গ;
	}
	CloseHandle(hTo);
    }

    মুক্ত(to_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    স্থির শূন্য
copy_infostreams(char_u *from, char_u *to)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_STREAM_ID	sid;
    পূর্ণাঙ্ক			headersize;
    WCHAR		streamname[_MAX_PATH];
    DWORD		readcount;
    শূন্য		*context = রিক্ত;
    DWORD		lo, hi;
    পূর্ণাঙ্ক			len;

    // Convert the file names to wide characters.
    fromw = enc_to_utf16(from, রিক্ত);
    tow = enc_to_utf16(to, রিক্ত);
    যদি (fromw != রিক্ত && tow != রিক্ত)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, রিক্ত,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, রিক্ত);
	যদি (sh != INVALID_HANDLE_মান)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    বাবে (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		ZeroMemory(&sid, পরিমাণ(WIN32_STREAM_ID));
		headersize = (পূর্ণাঙ্ক)((ৱর্ণ *)&sid.cStreamName - (ৱর্ণ *)&sid.dwStreamId);
		যদি (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    ভঙ্গ;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		যদি (sid.dwStreamNameSize > 0)
		{
		    // Read the stream name.
		    যদি (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dwStreamNameSize,
					  &readcount, FALSE, FALSE, &context))
			ভঙ্গ;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / পরিমাণ(WCHAR);
		    streamname[len] = 0;
		    যদি (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (long)sid.Size.u.নিচেৰ_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(শূন্য)BackupSeek(sh, sid.Size.u.নিচেৰ_Part, sid.Size.u.উপৰেৰ_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (শূন্য)BackupRead(sh, রিক্ত, 0, &readcount, TRUE, FALSE, &context);

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

প্রকারনির্দেশ সংরচনা _FILE_FULL_EA_INFORMATION_ {
    ULONG  NextEntryOffset;
    UCHAR  Flags;
    UCHAR  EaNameLength;
    USHORT Ea_মান_Length;
    CHAR   EaName[1];
} FILE_FULL_EA_INFORMATION_, *PFILE_FULL_EA_INFORMATION_;

প্রকারনির্দেশ সংরচনা _FILE_EA_INFORMATION_ {
    ULONG EaSize;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#ifndef PROTO
প্রকারনির্দেশ NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions);
প্রকারনির্দেশ NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
প্রকারনির্দেশ NTSTATUS (NTAPI *PfnNtSetEaFile)(
	HANDLE		    FileHandle,
	PIO_STATUS_BLOCK    IoStatusBlock,
	PVOID		    Buffer,
	ULONG		    Length);
প্রকারনির্দেশ NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	BOOLEAN ReturnSingleEntry,
	PVOID EaList,
	ULONG EaListLength,
	PULONG EaIndex,
	BOOLEAN RestartScan);
প্রকারনির্দেশ NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_STATUS_BLOCK	IoStatusBlock,
	PVOID			FileInformation,
	ULONG			Length,
	FILE_INFORMATION_CLASS FileInformationClass);
প্রকারনির্দেশ VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString);

PfnNtOpenFile pNtOpenFile = রিক্ত;
PfnNtClose pNtClose = রিক্ত;
PfnNtSetEaFile pNtSetEaFile = রিক্ত;
PfnNtQueryEaFile pNtQueryEaFile = রিক্ত;
PfnNtQueryInformationFile pNtQueryInformationFile = রিক্ত;
PfnRtlInitUnicodeString pRtlInitUnicodeString = রিক্ত;
#endif

/*
 * Load ntdll.dll functions.
 */
    স্থির BOOL
load_ntdll(শূন্য)
{
    স্থির পূর্ণাঙ্ক	loaded = -1;

    যদি (loaded != -1)
	ঘূৰাই (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    যদি (hNtdll != রিক্ত)
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
    যদি (pNtOpenFile == রিক্ত
	    || pNtClose == রিক্ত
	    || pNtSetEaFile == রিক্ত
	    || pNtQueryEaFile == রিক্ত
	    || pNtQueryInformationFile == রিক্ত
	    || pRtlInitUnicodeString == রিক্ত)
	loaded = FALSE;
    অন্যথা
	loaded = TRUE;
    ঘূৰাই (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    স্থির শূন্য
copy_extattr(char_u *from, char_u *to)
{
    char_u		    *fromf = রিক্ত;
    char_u		    *tof = রিক্ত;
    WCHAR		    *fromw = রিক্ত;
    WCHAR		    *tow = রিক্ত;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_ATTRIBUTES	    oa;
    IO_STATUS_BLOCK	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    শূন্য		    *ea = রিক্ত;

    যদি (!load_ntdll())
	ঘূৰাই;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(to) + 5);
    যদি (fromf == রিক্ত || tof == রিক্ত)
	গচ্ছ theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), to);

    // Convert the names to wide characters.
    fromw = enc_to_utf16(fromf, রিক্ত);
    tow = enc_to_utf16(tof, রিক্ত);
    যদি (fromw == রিক্ত || tow == রিক্ত)
	গচ্ছ theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    InitializeObjectAttributes(&oa, &u, 0, রিক্ত, রিক্ত);
    যদি (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	গচ্ছ theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, পরিমাণ(eainfo),
	    FileEaInformation);
    যদি (eainfo.EaSize != 0)
    {
	ea = alloc(eainfo.EaSize);
	যদি (ea != রিক্ত)
	{
	    যদি (pNtQueryEaFile(h, &iosb, ea, eainfo.EaSize, FALSE,
			রিক্ত, 0, রিক্ত, TRUE) != STATUS_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    যদি (ea != রিক্ত)
    {
	pRtlInitUnicodeString(&u, tow);
	InitializeObjectAttributes(&oa, &u, 0, রিক্ত, রিক্ত);
	যদি (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	    গচ্ছ theend;

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
    পূর্ণাঙ্ক
mch_copy_file_attribute(char_u *from, char_u *to)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, to);
    copy_extattr(from, to);
    ঘূৰাই 0;
}


/*
 * The command line arguments in UTF-16
 */
স্থির পূর্ণাঙ্ক	nArgsW = 0;
স্থির LPWSTR	*ArglistW = রিক্ত;
স্থির পূর্ণাঙ্ক	global_argc = 0;
স্থির ৱর্ণ	**global_argv;

স্থির পূর্ণাঙ্ক	used_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
স্থির পূর্ণাঙ্ক	*used_file_indexes = রিক্ত; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
স্থির পূর্ণাঙ্ক	used_file_গুনতি = 0;	// nr of entries in used_file_indexes
স্থির পূর্ণাঙ্ক	used_file_literal = FALSE;  // take file names literally
স্থির পূর্ণাঙ্ক	used_file_full_path = FALSE;  // file name was full path
#ifdef FEAT_DIFF
স্থির পূর্ণাঙ্ক	used_file_diff_mode = FALSE;  // file name was with diff mode
#endif
স্থির পূর্ণাঙ্ক	used_alist_গুনতি = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    পূর্ণাঙ্ক
get_cmd_argsW(ৱর্ণ ***argvp)
{
    ৱর্ণ	**argv = রিক্ত;
    পূর্ণাঙ্ক		argc = 0;
    পূর্ণাঙ্ক		i;

    free_cmd_argsW();
    ArglistW = Command_সাৰি_To_Argv_W(Get_Command_সাৰি_W(), &nArgsW);
    যদি (ArglistW != রিক্ত)
    {
	argv = alloc((nArgsW + 1) * পরিমাণ(ৱর্ণ *));
	যদি (argv != রিক্ত)
	{
	    argc = nArgsW;
	    argv[argc] = রিক্ত;
	    বাবে (i = 0; i < argc; ++i)
	    {
		পূর্ণাঙ্ক	len;

		// Convert each Unicode argument to UTF-8.
		WideCharToMultiByte_alloc(CP_UTF8, 0,
				ArglistW[i], (পূর্ণাঙ্ক)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		যদি (argv[i] == রিক্ত)
		{
		    // Out of memory, clear everything.
		    যেতিয়ালৈ (i > 0)
			মুক্ত(argv[--i]);
		    মুক্ত(argv);
		    argv = রিক্ত;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    যদি (argc > 0)
    {
	যদি (used_file_indexes != রিক্ত)
	    মুক্ত(used_file_indexes);
	used_file_indexes = alloc(argc * পরিমাণ(পূর্ণাঙ্ক));
    }

    যদি (argvp != রিক্ত)
	*argvp = argv;
    ঘূৰাই argc;
}

    শূন্য
free_cmd_argsW(শূন্য)
{
    যদি (ArglistW == রিক্ত)
	ঘূৰাই;

    GlobalFree(ArglistW);
    ArglistW = রিক্ত;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    শূন্য
used_file_arg(ৱর্ণ *name, পূর্ণাঙ্ক literal, পূর্ণাঙ্ক full_path, পূর্ণাঙ্ক diff_mode)
{
    পূর্ণাঙ্ক		i;

    যদি (used_file_indexes == রিক্ত)
	ঘূৰাই;
    বাবে (i = used_file_argc + 1; i < global_argc; ++i)
	যদি (STRCMP(global_argv[i], name) == 0)
	{
	    used_file_argc = i;
	    used_file_indexes[used_file_গুনতি++] = i;
	    ভঙ্গ;
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
    শূন্য
set_alist_গুনতি(শূন্য)
{
    used_alist_গুনতি = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    শূন্য
fix_arg_enc(শূন্য)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		idx;
    char_u	*str;
    পূর্ণাঙ্ক		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    যদি (global_argc != nArgsW
	    || ArglistW == রিক্ত
	    || used_file_indexes == রিক্ত
	    || used_file_গুনতি == 0
	    || used_alist_গুনতি != GARGCOUNT)
	ঘূৰাই;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(পূর্ণাঙ্ক, GARGCOUNT);
    যদি (fnum_list == রিক্ত)
	ঘূৰাই;		// out of memory
    বাবে (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    যদি (ga_grow(&global_alist.al_ga, used_file_গুনতি) == FAIL)
	ঘূৰাই;		// out of memory

    বাবে (i = 0; i < used_file_গুনতি; ++i)
    {
	idx = used_file_indexes[i];
	str = utf16_to_enc(ArglistW[idx], রিক্ত);
	যদি (str != রিক্ত)
	{
	    পূর্ণাঙ্ক literal = used_file_literal;

#ifdef FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    যদি (used_file_diff_mode && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), tail, STRLEN(tail), TRUE, &ret);
		যদি (ret.string != রিক্ত)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#endif
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    যদি (used_file_literal)
		buf_set_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    যদি (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		যদি (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_add(&global_alist, str, literal ? 2 : 0);
	}
    }

    যদি (!used_file_literal)
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
	alist_expand(fnum_list, used_alist_গুনতি);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    যদি (curwin->w_arg_idx == 0 && curbuf->b_fname == রিক্ত)
    {
	do_cmdline_cmd((char_u *)":rewind");
	যদি (GARGCOUNT == 1 && used_file_full_path
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    last_chdir_reason = "drop";
    }

    set_alist_গুনতি();
}

    পূর্ণাঙ্ক
mch_setenv(ৱর্ণ *var, ৱর্ণ *মান, পূর্ণাঙ্ক x UNUSED)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(মান) + 2);
    যদি (envbuf == রিক্ত)
	ঘূৰাই -1;

    sprintf((ৱর্ণ *)envbuf, "%s=%s", var, মান);

    p = enc_to_utf16(envbuf, রিক্ত);

    vim_free(envbuf);
    যদি (p == রিক্ত)
	ঘূৰাই -1;
    _wputenv(p);
#ifdef libintl_wputenv
    libintl_wputenv(p);
#endif
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    ঘূৰাই 0;
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

    স্থির শূন্য
vtp_flag_init(শূন্য)
{
#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   mode;
    HANDLE  out;

# ifdef VIMDLL
    যদি (!gui.in_use)
# endif
    {
	out = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_version >= VTP_FIRST_SUPPORT_BUILD) ? 1 : 0;
	GetConsoleMode(out, &mode);
	mode |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	যদি (SetConsoleMode(out, mode) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_buffer = win_version >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#endif

    যদি (win_version >= CONPTY_FIRST_SUPPORT_BUILD)
	conpty_working = 1;
    যদি (win_version >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    যদি (win_version <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    যদি (win_version <= CONPTY_1909_BUILD)
	conpty_type = 2;
    যদি (win_version <= CONPTY_1903_BUILD)
	conpty_type = 2;
    যদি (win_version < CONPTY_FIRST_SUPPORT_BUILD)
	conpty_type = 1;

    যদি (win_version >= CONPTY_NEXT_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    স্থির শূন্য
vtp_init(শূন্য)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = পরিমাণ(csbi);
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

    স্থির শূন্য
vtp_exit(শূন্য)
{
    restore_console_color_rgb();
}

    পূর্ণাঙ্ক
vtp_printf(
    ৱর্ণ *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   result;
    পূর্ণাঙ্ক	    len;

    যদি (silent_mode)
	ঘূৰাই 0;

    va_start(list, format);
    len = vim_vsnprintf((ৱর্ণ *)buf, 100, (ৱর্ণ *)format, list);
    va_end(list);
    WriteConsoleA(g_hConOut, buf, (DWORD)len, &result, রিক্ত);
    ঘূৰাই (পূর্ণাঙ্ক)result;
}

    স্থির শূন্য
vtp_sgr_bulk(
    পূর্ণাঙ্ক arg)
{
    পূর্ণাঙ্ক args[1];

    args[0] = arg;
    vtp_sgr_bulks(1, args);
}

# define FAST256(x) \
    যদি ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    পক্ষ x: \
	FAST256(newargs[x - 1]);

    স্থির শূন্য
vtp_sgr_bulks(
    পূর্ণাঙ্ক argc,
    পূর্ণাঙ্ক *args)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    পূর্ণাঙ্ক	    in, out;
    পূর্ণাঙ্ক	    newargs[16];
    স্থির পূর্ণাঙ্ক sgrfgr = -1, sgrfgg, sgrfgb;
    স্থির পূর্ণাঙ্ক sgrbgr = -1, sgrbgg, sgrbgb;

    যদি (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	ঘূৰাই;
    }

    in = out = 0;
    যেতিয়ালৈ (in < argc)
    {
	পূর্ণাঙ্ক s = args[in];
	পূর্ণাঙ্ক copylen = 1;

	যদি (s == 38)
	{
	    যদি (argc - in >= 5 && args[in + 1] == 2)
	    {
		যদি (sgrfgr == args[in + 2] && sgrfgg == args[in + 3]
						     && sgrfgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		অন্যথা
		{
		    sgrfgr = args[in + 2];
		    sgrfgg = args[in + 3];
		    sgrfgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    অন্যথা যদি (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	অন্যথা যদি (s == 48)
	{
	    যদি (argc - in >= 5 && args[in + 1] == 2)
	    {
		যদি (sgrbgr == args[in + 2] && sgrbgg == args[in + 3]
						     && sgrbgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		অন্যথা
		{
		    sgrbgr = args[in + 2];
		    sgrbgg = args[in + 3];
		    sgrbgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    অন্যথা যদি (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	অন্যথা যদি (30 <= s && s <= 39)
	    sgrfgr = -1;
	অন্যথা যদি (90 <= s && s <= 97)
	    sgrfgr = -1;
	অন্যথা যদি (40 <= s && s <= 49)
	    sgrbgr = -1;
	অন্যথা যদি (100 <= s && s <= 107)
	    sgrbgr = -1;
	অন্যথা যদি (s == 0)
	    sgrfgr = sgrbgr = -1;

	যেতিয়ালৈ (copylen--)
	    newargs[out++] = args[in++];
    }

    p = &buf[পরিমাণ(buf) - 1];
    *p-- = 'm';

    ৱিকল্প (out)
    {
	পূর্ণাঙ্ক	n, m;
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
	WriteConsoleA(g_hConOut, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, রিক্ত);
    স্ৱভাৱ:
	ভঙ্গ;
    }
}

    স্থির শূন্য
wt_init(শূন্য)
{
    wt_working = mch_getenv("WT_SESSION") != রিক্ত;
}

# ifdef FEAT_TERMGUICOLORS
    স্থির পূর্ণাঙ্ক
ctermtoxterm(
    পূর্ণাঙ্ক cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    ঘূৰাই (((পূর্ণাঙ্ক)r << 16) | ((পূর্ণাঙ্ক)g << 8) | (পূর্ণাঙ্ক)b);
}
# endif

    স্থির শূন্য
set_console_color_rgb(শূন্য)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    guicolor_T	fg, bg;
    পূর্ণাঙ্ক		ctermfg, ctermbg;

    যদি (!vtp_working)
	ঘূৰাই;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    যদি (p_tgc || t_colors >= 256)
    {
	term_fg_rgb_color(fg);
	term_bg_rgb_color(bg);
	ঘূৰাই;
    }

    যদি (use_alternate_screen_buffer)
	ঘূৰাই;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cbSize = পরিমাণ(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = পরিমাণ(csbi);
    csbi.srWindow.ডান_3 += 1;
    csbi.srWindow.Bottom += 1;
    store_console_bg_rgb = csbi.ColorTable[g_color_index_bg];
    store_console_fg_rgb = csbi.ColorTable[g_color_index_fg];
    csbi.ColorTable[g_color_index_bg] = (COLORREF)bg;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)fg;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

# যদি defined(FEAT_TERMGUICOLORS)
    শূন্য
get_default_console_color(
    পূর্ণাঙ্ক *cterm_fg,
    পূর্ণাঙ্ক *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    পূর্ণাঙ্ক id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    পূর্ণাঙ্ক ctermfg = 0;
    পূর্ণাঙ্ক ctermbg = 0;
    পূর্ণাঙ্ক dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    যদি (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    যদি (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	যদি (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	যদি (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	অন্যথা
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_normal_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    যদি (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	যদি (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	যদি (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	অন্যথা
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
    স্থির শূন্য
reset_console_color_rgb(শূন্য)
{
# ifdef FEAT_TERMGUICOLORS
    যদি (use_alternate_screen_buffer)
	ঘূৰাই;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = পরিমাণ(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = পরিমাণ(csbi);
    csbi.srWindow.ডান_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)store_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)store_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    স্থির শূন্য
restore_console_color_rgb(শূন্য)
{
# ifdef FEAT_TERMGUICOLORS
    যদি (use_alternate_screen_buffer)
	ঘূৰাই;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = পরিমাণ(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = পরিমাণ(csbi);
    csbi.srWindow.ডান_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)save_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)save_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

    শূন্য
control_console_color_rgb(শূন্য)
{
    যদি (vtp_working)
	set_console_color_rgb();
    অন্যথা
	reset_console_color_rgb();
}

    পূর্ণাঙ্ক
use_vtp(শূন্য)
{
    ঘূৰাই USE_VTP;
}

    পূর্ণাঙ্ক
is_term_win32(শূন্য)
{
    ঘূৰাই T_NAME != রিক্ত && STRCMP(T_NAME, "win32") == 0;
}

    পূর্ণাঙ্ক
has_vtp_working(শূন্য)
{
    ঘূৰাই vtp_working;
}

#endif

    পূর্ণাঙ্ক
has_conpty_working(শূন্য)
{
    ঘূৰাই conpty_working;
}

    পূর্ণাঙ্ক
get_conpty_type(শূন্য)
{
    ঘূৰাই conpty_type;
}

    পূর্ণাঙ্ক
is_conpty_stable(শূন্য)
{
    ঘূৰাই conpty_stable;
}

    পূর্ণাঙ্ক
get_conpty_fix_type(শূন্য)
{
    ঘূৰাই conpty_fix_type;
}

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    শূন্য
resize_console_buf(শূন্য)
{
    যদি (use_alternate_screen_buffer)
	ঘূৰাই;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    যদি (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ঘূৰাই;

    coord.X = SRWIDTH(csbi.srWindow);
    coord.Y = SRHEIGHT(csbi.srWindow);
    SetConsoleScreenBufferSize(g_hConOut, coord);

    newsize.বাম_3 = 0;
    newsize.শীৰ্ষ_2 = 0;
    newsize.ডান_3 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    SetConsoleWindowInfo(g_hConOut, TRUE, &newsize);

    SetConsoleScreenBufferSize(g_hConOut, coord);
}
#endif

    ৱর্ণ *
GetWin32Error(শূন্য)
{
    স্থির ৱর্ণ	*oldmsg = রিক্ত;
    ৱর্ণ	*acp_বাৰ্তা = রিক্ত;
    DWORD	acp_len;
    ৱর্ণ	*enc_বাৰ্তা = রিক্ত;
    পূর্ণাঙ্ক		enc_len = 0;

    // get formatted message from OS
    acp_len = FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
	    রিক্ত, GetLastError(), 0, (LPSTR)&acp_বাৰ্তা, 0, রিক্ত);
    যদি (acp_len == 0 || acp_বাৰ্তা == রিক্ত)
	ঘূৰাই রিক্ত;

    // clean oldmsg if remained.
    যদি (oldmsg != রিক্ত)
    {
	vim_free(oldmsg);
	oldmsg = রিক্ত;
    }

    acp_to_enc((char_u *)acp_বাৰ্তা, (পূর্ণাঙ্ক)acp_len, (char_u **)&enc_বাৰ্তা, &enc_len);
    LocalFree(acp_বাৰ্তা);
    যদি (enc_বাৰ্তা == রিক্ত)
	ঘূৰাই রিক্ত;

    // remove trailing \r\n
    ৱর্ণ *pcrlf = strstr(enc_বাৰ্তা, "\r\n");
    যদি (pcrlf != রিক্ত)
	*pcrlf = NUL;

    oldmsg = enc_বাৰ্তা;
    ঘূৰাই enc_বাৰ্তা;
}

#যদি defined(FEAT_RELTIME)
স্থির HANDLE   timer_handle;
স্থির পূর্ণাঙ্ক      timer_active = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
স্থির sig_atomic_t timeout_flags[2];
স্থির পূর্ণাঙ্ক	    timeout_flag_idx = 0;
স্থির sig_atomic_t *timeout_flag = &timeout_flags[0];


    স্থির শূন্য CALLBACK
set_flag(শূন্য *param, BOOLEAN unused2 UNUSED)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    শূন্য
stop_timeout(শূন্য)
{
    যদি (timer_active)
    {
	BOOL ret = DeleteTimerQueueTimer(রিক্ত, timer_handle, রিক্ত);
	timer_active = FALSE;
	যদি (!ret && GetLastError() != ERROR_IO_PENDING)
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
    চঞ্চল sig_atomic_t *
start_timeout(long msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = CreateTimerQueueTimer(
	    &timer_handle, রিক্ত, set_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    যদি (!ret)
    {
	semsg(_(e_could_not_set_timeout_str), GetWin32Error());
    }
    অন্যথা
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_active = TRUE;
	*timeout_flag = FALSE;
    }
    ঘূৰাই timeout_flag;
}
#endif
