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

#અન્તર્ભાવ "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#અન્તર્ભાવ <sys/types.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <limits.h>

#અન્તર્ભાવ <process.h>
#અન્તર્ભાવ <winternl.h>
#અન્તર્ભાવ <direct.h>

#જો !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#ifdef FEAT_JOB_CHANNEL
# include <tlhelp32.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = ખાલી;
#endif

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
સ્થિર HANDLE g_hConIn  = INVALID_HANDLE_કિંમત;
સ્થિર HANDLE g_hConOut = INVALID_HANDLE_કિંમત;

// Win32 Screen buffer,coordinate,console I/O information
સ્થિર SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
સ્થિર COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
સ્થિર WORD  g_attrDefault = 7;  // lightgray text on black background
સ્થિર WORD  g_attrCurrent;

સ્થિર પૂર્ણાંક g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
સ્થિર પૂર્ણાંક g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
સ્થિર પૂર્ણાંક g_fForceExit = FALSE;    // set when forcefully exiting

સ્થિર શૂન્ય scroll(unsigned c_લીટીઓ);
સ્થિર શૂન્ય set_scroll_region(unsigned ડાબું, unsigned ટોચ,
			      unsigned જમણું, unsigned bottom);
સ્થિર શૂન્ય set_scroll_region_tb(unsigned ટોચ, unsigned bottom);
સ્થિર શૂન્ય set_scroll_region_lr(unsigned ડાબું, unsigned જમણું);
સ્થિર શૂન્ય ઉમેરો_લીટીઓ(unsigned c_લીટીઓ);
સ્થિર શૂન્ય delete_લીટીઓ(unsigned c_લીટીઓ);
સ્થિર શૂન્ય gotoxy(unsigned x, unsigned y);
સ્થિર શૂન્ય standout(શૂન્ય);
સ્થિર પૂર્ણાંક s_cursor_visible = TRUE;
સ્થિર પૂર્ણાંક did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
પ્રકારવ્યાખ્યા સંરચના આગત_record_buffer_ગાંઠ_S
{
    આગત_RECORD ir;
    સંરચના આગત_record_buffer_ગાંઠ_S *next;
} આગત_record_buffer_ગાંઠ_T;
પ્રકારવ્યાખ્યા સંરચના આગત_record_buffer_S
{
    આગત_record_buffer_ગાંઠ_T *head;
    આગત_record_buffer_ગાંઠ_T *tail;
    પૂર્ણાંક length;
} આગત_record_buffer_T;
સ્થિર આગત_record_buffer_T આગત_record_buffer;
સ્થિર પૂર્ણાંક read_આગત_record_buffer(આગત_RECORD* irEvents, પૂર્ણાંક nMaxLength);
સ્થિર પૂર્ણાંક write_આગત_record_buffer(આગત_RECORD* irEvents, પૂર્ણાંક nLength);
#endif
#ifdef FEAT_GUI_MSWIN
સ્થિર પૂર્ણાંક s_dont_use_vimrun = TRUE;
સ્થિર પૂર્ણાંક need_vimrun_warning = FALSE;
સ્થિર string_T vimrun_path = {(char_u *)"vimrun ", 7};
સ્થિર પૂર્ણાંક vimrun_path_allocated = FALSE;
#endif

સ્થિર પૂર્ણાંક win32_getattrs(char_u *name);
સ્થિર પૂર્ણાંક win32_setattrs(char_u *name, પૂર્ણાંક attrs);
સ્થિર પૂર્ણાંક win32_set_archive(char_u *name);

સ્થિર પૂર્ણાંક conpty_working = 0;
સ્થિર પૂર્ણાંક conpty_type = 0;
સ્થિર પૂર્ણાંક conpty_stable = 0;
સ્થિર પૂર્ણાંક conpty_fix_type = 0;
સ્થિર શૂન્ય vtp_flag_init(શૂન્ય);

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
સ્થિર પૂર્ણાંક vtp_working = 0;
સ્થિર શૂન્ય vtp_init(શૂન્ય);
સ્થિર શૂન્ય vtp_exit(શૂન્ય);
સ્થિર શૂન્ય vtp_sgr_bulk(પૂર્ણાંક arg);
સ્થિર શૂન્ય vtp_sgr_bulks(પૂર્ણાંક argc, પૂર્ણાંક *argv);

સ્થિર પૂર્ણાંક wt_working = 0;
સ્થિર શૂન્ય wt_init(શૂન્ય);

સ્થિર પૂર્ણાંક g_color_index_bg = 0;
સ્થિર પૂર્ણાંક g_color_index_fg = 7;

# ifdef FEAT_TERMGUICOLORS
સ્થિર guicolor_T save_console_bg_rgb;
સ્થિર guicolor_T save_console_fg_rgb;
સ્થિર guicolor_T store_console_bg_rgb;
સ્થિર guicolor_T store_console_fg_rgb;
સ્થિર પૂર્ણાંક default_console_color_bg = 0x000000; // black
સ્થિર પૂર્ણાંક default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_term_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# નહીંતર
#  define USE_VTP		0
#  define USE_WT		0
# endif

સ્થિર શૂન્ય set_console_color_rgb(શૂન્ય);
સ્થિર શૂન્ય reset_console_color_rgb(શૂન્ય);
સ્થિર શૂન્ય restore_console_color_rgb(શૂન્ય);
#endif  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
સ્થિર પૂર્ણાંક suppress_winsize = 1;	// don't fiddle with console
#endif

સ્થિર WCHAR *exe_pathw = ખાલી;

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
સ્થિર BOOL use_alternate_screen_buffer = FALSE;
#endif

બાહ્ય DWORD win_version; // this is in os_mswin.c

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    સ્થિર BOOL
is_ambiwidth_event(
    આગત_RECORD *ir)
{
    પાછા ir->EventType == ચાવી_EVENT
		&& ir->Event.ચાવી_Event_2.b_ચાવી_Down
		&& ir->Event.ચાવી_Event_2.w_Repeat_ગણતરી == 1
		&& ir->Event.ચાવી_Event_2.w_Virtual_ચાવી_Code == 0x12
		&& ir->Event.ચાવી_Event_2.wVirtualScanCode == 0x38
		&& ir->Event.ચાવી_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.ચાવી_Event_2.dw_Control_ચાવી_State == 2;
}

    સ્થિર શૂન્ય
make_ambiwidth_event(
    આગત_RECORD *down,
    આગત_RECORD *up)
{
    down->Event.ચાવી_Event_2.w_Virtual_ચાવી_Code = 0;
    down->Event.ચાવી_Event_2.wVirtualScanCode = 0;
    down->Event.ચાવી_Event_2.uChar.UnicodeChar
				    = up->Event.ચાવી_Event_2.uChar.UnicodeChar;
    down->Event.ચાવી_Event_2.dw_Control_ચાવી_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    સ્થિર BOOL
read_console_આગત(
    HANDLE	    h_આગત,
    આગત_RECORD    *lpBuffer,
    પૂર્ણાંક		    nLength,
    LPDWORD	    lpEvents)
{
    ગણના
    {
	IRSIZE = 10
    };
    સ્થિર આગત_RECORD s_irCache[IRSIZE];
    સ્થિર DWORD s_dwIndex = 0;
    સ્થિર DWORD s_dwMax = 0;
    DWORD dwEvents;
    પૂર્ણાંક head;
    પૂર્ણાંક tail;
    પૂર્ણાંક i;
    સ્થિર આગત_RECORD s_irPseudo;

    જો (s_dwMax == 0 && આગત_record_buffer.length > 0)
    {
	dwEvents = read_આગત_record_buffer(s_irCache, IRSIZE);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
    }

    જો (nLength == -2)
	પાછા (s_dwMax > 0) ? TRUE : FALSE;

    જો (win_version < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	જો (nLength == -1)
	    પાછા Peek_Console_આગત_W(h_આગત, lpBuffer, 1, lpEvents);
	પાછા Read_Console_આગત_W(h_આગત, lpBuffer, 1, &dwEvents);
    }

    જો (s_dwMax == 0)
    {
	જો (!vtp_working && nLength == -1)
	    પાછા Peek_Console_આગત_W(h_આગત, lpBuffer, 1, lpEvents);
	Get_સંખ્યા_Of_Console_આગત_Events(h_આગત, &dwEvents);
	જો (dwEvents == 0 && nLength == -1)
	    પાછા Peek_Console_આગત_W(h_આગત, lpBuffer, 1, lpEvents);
	Read_Console_આગત_W(h_આગત, s_irCache, IRSIZE, &dwEvents);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
	જો (dwEvents == 0)
	{
	    *lpEvents = 0;
	    પાછા TRUE;
	}

	માટે (i = s_dwIndex; i < (પૂર્ણાંક)s_dwMax - 1; ++i)
	    જો (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	જો (s_dwMax > 1)
	{
	    head = 0;
	    tail = s_dwMax - 1;
	    જ્યારે (head != tail)
	    {
		જો (s_irCache[head].EventType == WINDOW_BUFFER_SIZE_EVENT
			&& s_irCache[head + 1].EventType
						  == WINDOW_BUFFER_SIZE_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    માટે (i = head; i < tail; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --tail;
		    ચાલુ;
		}
		head++;
	    }
	    s_dwMax = tail + 1;
	}
    }

    જો (s_irCache[s_dwIndex].EventType == ચાવી_EVENT)
    {
	જો (s_irCache[s_dwIndex].Event.ચાવી_Event_2.w_Repeat_ગણતરી > 1)
	{
	    s_irPseudo = s_irCache[s_dwIndex];
	    s_irPseudo.Event.ચાવી_Event_2.w_Repeat_ગણતરી = 1;
	    s_irCache[s_dwIndex].Event.ચાવી_Event_2.w_Repeat_ગણતરી--;
	    *lpBuffer = s_irPseudo;
	    *lpEvents = 1;
	    પાછા TRUE;
	}
    }

    *lpBuffer = s_irCache[s_dwIndex];
    જો (!(nLength == -1 || nLength == -2) && ++s_dwIndex >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    પાછા TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    સ્થિર BOOL
peek_console_આગત(
    HANDLE	    h_આગત,
    આગત_RECORD    *lpBuffer,
    DWORD	    nLength UNUSED,
    LPDWORD	    lpEvents)
{
    પાછા read_console_આગત(h_આગત, lpBuffer, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    સ્થિર DWORD
સંદેશ_wait_for_multiple_objects(
    DWORD    n_ગણતરી,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dwWakeMask)
{
    જો (read_console_આગત(ખાલી, ખાલી, -2, ખાલી))
	પાછા WAIT_OBJECT_0;
    પાછા સંદેશ_Wait_For_Multiple_Objects(n_ગણતરી, pHandles, fWaitAll,
				     dwMilliseconds, dwWakeMask);
}
# endif

# ifndef FEAT_CLIENTSERVER
    સ્થિર DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    જો (read_console_આગત(ખાલી, ખાલી, -2, ખાલી))
	પાછા WAIT_OBJECT_0;
    પાછા WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#endif   // !FEAT_GUI_MSWIN || VIMDLL

    શૂન્ય
mch_get_exe_name(શૂન્ય)
{
    પૂર્ણાંક		updated = FALSE;
    સ્થિર પૂર્ણાંક	enc_prev = -1;
    WCHAR	*path;
    size_t	pathlen;

    જો (exe_name == ખાલી || exe_pathw == ખાલી || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_PATH];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(ખાલી, buf, MAX_PATH);
	જો (p > buf)
	{
	    જો (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_to_enc(buf, ખાલી);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    કરો
	    {
		--p;
		જો (*p == L'\\')
		{
		    *p = L'\0';
		    તોડો;
		}
	    } જ્યારે (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    જો (exe_pathw == ખાલી || !updated)
	પાછા;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    path = _wgetenv(L"PATH");
    pathlen = (path == ખાલી) ? 0 : wcslen(path);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#define EXTRA_LEN 7
#define MAX_ENV_STRING_LEN 8191

    જો (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   temp[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	જો (pathlen == 0)
	    wcscpy(temp + templen, exe_pathw);
	નહીંતર
	{
	    wcscpy(temp + templen, path);

	    // Check if exe_path is already included in $PATH.
	    જો (wcsstr(temp, exe_pathw) == ખાલી)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		જો (temp[templen - 1] != L';')
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
    સ્થિર શૂન્ય
unescape_shellxquote(char_u *p, char_u *escaped)
{
    પૂર્ણાંક	    l = (પૂર્ણાંક)STRLEN(p);
    પૂર્ણાંક	    n;

    જ્યારે (*p != NUL)
    {
	જો (*p == '^' && vim_strchr(escaped, p[1]) != ખાલી)
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
vimLoadLib(અચલ અક્ષર *name)
{
    HINSTANCE	dll = ખાલી;

    // No need to load any library when registering OLE.
    જો (found_register_arg)
	પાછા ખાલી;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    જો (exe_pathw == ખાલી)
    {
	mch_get_exe_name();
	જો (exe_pathw == ખાલી)
	    પાછા ખાલી;
    }

    WCHAR old_dirw[MAXPATHL];

    જો (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	પાછા ખાલી;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    SetCurrentDirectoryW(exe_pathw);
    dll = LoadLibrary(name);
    SetCurrentDirectoryW(old_dirw);
    પાછા dll;
}

#જો defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    પૂર્ણાંક
mch_is_gui_executable(શૂન્ય)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(ખાલી);
    PIMAGE_DOS_HEADER	pDOS = (PIMAGE_DOS_HEADER)pImage;
    PIMAGE_NT_HEADERS	pPE;

    જો (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	પાછા FALSE;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    જો (pPE->Signature != IMAGE_NT_SIGNATURE)
	પાછા FALSE;
    જો (pPE->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	પાછા TRUE;
    પાછા FALSE;
}
#endif

#જો defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    સ્થિર શૂન્ય *
get_imported_func_info(HINSTANCE hInst, અચલ અક્ષર *funcname, પૂર્ણાંક info,
	અચલ શૂન્ય *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_HEADER		pDOS = (PIMAGE_DOS_HEADER)hInst;
    PIMAGE_NT_HEADERS		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_DATA		pIAT;	    // Import Address Table
    PIMAGE_THUNK_DATA		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    જો (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	પાછા ખાલી;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    જો (pPE->Signature != IMAGE_NT_SIGNATURE)
	પાછા ખાલી;

    ImpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    જો (ImpVA == 0)
	પાછા ખાલી;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    માટે (; pImpDesc->FirstThunk; ++pImpDesc)
    {
	જો (!pImpDesc->OriginalFirstThunk)
	    ચાલુ;
	pIAT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->FirstThunk);
	pINT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->OriginalFirstThunk);
	માટે (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    જો (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		ચાલુ;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.AddressOfData));
	    જો (strcmp((અક્ષર *)pImpName->Name, funcname) == 0)
	    {
		શૂન્ય *original;
		DWORD old, new = PAGE_READWRITE;

		પસંદગી (info)
		{
		    સ્થિતિ 0:
			પાછા (શૂન્ય *)pIAT->u1.Function;
		    સ્થિતિ 1:
			પાછા (શૂન્ય *)(pImage + pImpDesc->Name);
		    સ્થિતિ 2:
			original = (શૂન્ય *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, કદ(શૂન્ય *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, કદ(શૂન્ય *),
				old, &new);
			પાછા original;
		    મૂળભૂત:
			પાછા ખાલી;
		}
	    }
	}
    }
    પાછા ખાલી;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
find_imported_module_by_funcname(HINSTANCE hInst, અચલ અક્ષર *funcname)
{
    અક્ષર    *modulename;

    modulename = (અક્ષર *)get_imported_func_info(hInst, funcname, 1, ખાલી);
    જો (modulename != ખાલી)
	પાછા GetModuleHandleA(modulename);
    પાછા ખાલી;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    શૂન્ય *
get_dll_import_func(HINSTANCE hInst, અચલ અક્ષર *funcname)
{
    પાછા get_imported_func_info(hInst, funcname, 0, ખાલી);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    શૂન્ય *
hook_dll_import_func(HINSTANCE hInst, અચલ અક્ષર *funcname, અચલ શૂન્ય *hook)
{
    પાછા get_imported_func_info(hInst, funcname, 2, hook);
}
#endif

#જો defined(FEAT_PYTHON3)
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

    જો (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	પાછા ખાલી;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    જો (pPE->Signature != IMAGE_NT_SIGNATURE)
	પાછા ખાલી;

    ExpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    ExpSize = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    જો (ExpVA == 0)
	પાછા hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    pFunctionTable = (LPDWORD)(pImage + pExpDir->AddressOfFunctions);

    જો (pExpDir->સંખ્યા_Of_Names == 0)
	પાછા hInst;	// No export names.

    // Check only the first entry.
    જો ((pFunctionTable[0] < ExpVA) || (pFunctionTable[0] >= ExpVA + ExpSize))
	// The first entry is not a function forwarder.
	પાછા hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    અચલ અક્ષર *name = (અચલ અક્ષર *)(pImage + pFunctionTable[0]);
    અચલ અક્ષર *p = strchr(name, '.');
    જો (p == ખાલી)
	પાછા hInst;

    // Extract DllName.
    અક્ષર buf[MAX_PATH];
    જો (p - name + 1 > કદ(buf))
	પાછા ખાલી;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    પાછા GetModuleHandleA(buf);
}
#endif

#જો defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
સ્થિર અક્ષર *null_libintl_gettext(અચલ અક્ષર *);
સ્થિર અક્ષર *null_libintl_ngettext(અચલ અક્ષર *, અચલ અક્ષર *, unsigned long n);
સ્થિર અક્ષર *null_libintl_textdomain(અચલ અક્ષર *);
સ્થિર અક્ષર *null_libintl_bindtextdomain(અચલ અક્ષર *, અચલ અક્ષર *);
સ્થિર અક્ષર *null_libintl_bind_textdomain_codeset(અચલ અક્ષર *, અચલ અક્ષર *);
સ્થિર પૂર્ણાંક null_libintl_wputenv(અચલ wchar_t *);

સ્થિર HINSTANCE hLibintlDLL = ખાલી;
અક્ષર *(*dyn_libintl_gettext)(અચલ અક્ષર *) = null_libintl_gettext;
અક્ષર *(*dyn_libintl_ngettext)(અચલ અક્ષર *, અચલ અક્ષર *, unsigned long n)
						= null_libintl_ngettext;
અક્ષર *(*dyn_libintl_textdomain)(અચલ અક્ષર *) = null_libintl_textdomain;
અક્ષર *(*dyn_libintl_bindtextdomain)(અચલ અક્ષર *, અચલ અક્ષર *)
						= null_libintl_bindtextdomain;
અક્ષર *(*dyn_libintl_bind_textdomain_codeset)(અચલ અક્ષર *, અચલ અક્ષર *)
				       = null_libintl_bind_textdomain_codeset;
પૂર્ણાંક (*dyn_libintl_wputenv)(અચલ wchar_t *) = null_libintl_wputenv;
પૂર્ણાંક *dyn_libintl_nl_સંદેશ_cat_cntr = ખાલી;

    પૂર્ણાંક
dyn_libintl_init(શૂન્ય)
{
    પૂર્ણાંક i;
    સ્થિર સંરચના
    {
	અક્ષર	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{ખાલી, ખાલી}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    જો (hLibintlDLL != ખાલી)
	પાછા 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vimLoadLib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    જો (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    જો (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT2);
# endif
    જો (!hLibintlDLL)
    {
	જો (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_load_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	પાછા 0;
    }
    માટે (i = 0; libintl_entry[i].name != ખાલી
					 && libintl_entry[i].ptr != ખાલી; ++i)
    {
	જો ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == ખાલી)
	{
	    dyn_libintl_end();
	    જો (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_load_library_function_str), libintl_entry[i].name);
		verbose_leave();
	    }
	    પાછા 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (અક્ષર *(*)(અચલ અક્ષર *, અચલ અક્ષર *))
			GetProcAddress(hLibintlDLL, "bind_textdomain_codeset");
    જો (dyn_libintl_bind_textdomain_codeset == ખાલી)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = find_imported_module_by_funcname(hLibintlDLL, "getenv");
    જો (hmsvcrt != ખાલી)
	dyn_libintl_wputenv = (પૂર્ણાંક (*)(અચલ wchar_t *))
					GetProcAddress(hmsvcrt, "_wputenv");
    જો (dyn_libintl_wputenv == ખાલી || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_સંદેશ_cat_cntr = (પૂર્ણાંક *)GetProcAddress(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    પાછા 1;
}

    શૂન્ય
dyn_libintl_end(શૂન્ય)
{
    જો (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ખાલી;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_સંદેશ_cat_cntr	= ખાલી;
}

    સ્થિર અક્ષર *
null_libintl_gettext(અચલ અક્ષર *msgid)
{
    પાછા (અક્ષર*)msgid;
}

    સ્થિર અક્ષર *
null_libintl_ngettext(
	અચલ અક્ષર *msgid,
	અચલ અક્ષર *msgid_plural,
	unsigned long n)
{
    પાછા (અક્ષર *)(n == 1 ? msgid : msgid_plural);
}

    સ્થિર અક્ષર *
null_libintl_bindtextdomain(
	અચલ અક્ષર *domainname UNUSED,
	અચલ અક્ષર *dirname UNUSED)
{
    પાછા ખાલી;
}

    સ્થિર અક્ષર *
null_libintl_bind_textdomain_codeset(
	અચલ અક્ષર *domainname UNUSED,
	અચલ અક્ષર *codeset UNUSED)
{
    પાછા ખાલી;
}

    સ્થિર અક્ષર *
null_libintl_textdomain(અચલ અક્ષર *domainname UNUSED)
{
    પાછા ખાલી;
}

    સ્થિર પૂર્ણાંક
null_libintl_wputenv(અચલ wchar_t *envstring UNUSED)
{
    પાછા 0;
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
    સ્થિર BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    જો (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	પાછા FALSE;

    જો (!Lookup_Privilege_કિંમત(ખાલી, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	પાછા FALSE;
    }

    tokenPrivileges.Privilege_ગણતરી	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    જો (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, ખાલી, ખાલી))
    {
	CloseHandle(hToken);
	પાછા FALSE;
    }

    જો (GetLastError() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	પાછા FALSE;
    }

    CloseHandle(hToken);
    પાછા TRUE;
}
#endif

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    શૂન્ય
PlatformId(શૂન્ય)
{
#ifdef FEAT_EVAL
    DWORD major = (win_version >> 24) & 0xFF;
    DWORD minor = (win_version >> 16) & 0xFF;
    vim_snprintf(windowsVersion, કદ(windowsVersion), "%d.%d", major, minor);
#endif

#ifdef HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    જો (!win32_enable_privilege(SE_SECURITY_NAME))
	પાછા;
#endif
}

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SHIFT  (SHIFT_PRESSED)
# define CTRL   (જમણું_CTRL_PRESSED | ડાબું_CTRL_PRESSED)
# define ALT    (જમણું_ALT_PRESSED  | ડાબું_ALT_PRESSED)
# define ALT_GR (જમણું_ALT_PRESSED  | ડાબું_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

સ્થિર અચલ સંરચના
{
    WORD    w_Virt_ચાવી;
    BOOL    f_Ansi_ચાવી;
    પૂર્ણાંક	    chAlone;
    પૂર્ણાંક	    chShift;
    પૂર્ણાંક	    chCtrl;
    પૂર્ણાંક	    chAlt;
} Virt_ચાવી_Map[] =
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
    { VK_ડાબું,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_જમણું,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_END,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_DOWN,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_NEXT,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_ઉમેરો,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_BACK,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# જો 0
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
    સ્થિર પૂર્ણાંક
win32_kbd_patch_ચાવી(
    ચાવી_EVENT_RECORD *pker)
{
    UINT uMods = pker->dw_Control_ચાવી_State;
    સ્થિર પૂર્ણાંક s_iIsDead = 0;
    સ્થિર WORD awAnsiCode[2];
    સ્થિર BYTE abKeystate[256];

    જો (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[1];
	s_iIsDead = 0;
	પાછા 1;
    }

    // check if it already has a valid unicode character.
    જો (pker->uChar.UnicodeChar != 0)
	પાછા 1;

    CLEAR_FIELD(abKeystate);

    // Clear any pending dead keys
    ToUnicode(VK_SPACE, Map_Virtual_ચાવી(VK_SPACE, 0), abKeystate, awAnsiCode, 2, 0);

    જો (uMods & SHIFT_PRESSED)
	abKeystate[VK_SHIFT] = 0x80;
    જો (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    જો ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ToUnicode(pker->w_Virtual_ચાવી_Code, pker->wVirtualScanCode,
			abKeystate, awAnsiCode, 2, 0);

    જો (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[0];

    પાછા s_iIsDead;
}

સ્થિર BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    સ્થિર BOOL
decode_ચાવી_event(
    ચાવી_EVENT_RECORD	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    પૂર્ણાંક			*pmodifiers,
    BOOL		fDoPost UNUSED)
{
    પૂર્ણાંક i;
    અચલ પૂર્ણાંક nModifs = pker->dw_Control_ચાવી_State & (SHIFT | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    જો (!pker->b_ચાવી_Down)
	પાછા FALSE;

    // ignore some keystrokes
    પસંદગી (pker->w_Virtual_ચાવી_Code)
    {
    // modifiers
    સ્થિતિ VK_SHIFT:
    સ્થિતિ VK_CONTROL:
    સ્થિતિ VK_MENU:   // Alt key
	પાછા FALSE;

    મૂળભૂત:
	તોડો;
    }

    // Shift-TAB
    જો (pker->w_Virtual_ચાવી_Code == VK_TAB && (nModifs & SHIFT_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	પાછા TRUE;
    }

    માટે (i = ARRAY_LENGTH(Virt_ચાવી_Map); --i >= 0; )
    {
	જો (Virt_ચાવી_Map[i].w_Virt_ચાવી == pker->w_Virtual_ચાવી_Code)
	{
	    *pch = Virt_ચાવી_Map[i].chAlone;
	    જો ((nModifs & SHIFT) != 0)
		*pch = Virt_ચાવી_Map[i].chShift;
	    નહીંતર જો ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_ચાવી_Map[i].chCtrl;
	    નહીંતર જો ((nModifs & ALT) != 0)
		*pch = Virt_ચાવી_Map[i].chAlt;

	    જો (*pch != 0)
	    {
		જો (Virt_ચાવી_Map[i].f_Ansi_ચાવી)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    જો (pmodifiers)
		    {
			જો (pker->w_Virtual_ચાવી_Code >= VK_F1
			    && pker->w_Virtual_ચાવી_Code <= VK_F12)
			{
			    જો ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_MASK_ALT;
				જો ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_ચાવી_Map[i].chAlone;
			    }
			    જો ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				જો ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_ચાવી_Map[i].chAlone;
			    }
			}
			નહીંતર જો (pker->w_Virtual_ચાવી_Code >= VK_END
				&& pker->w_Virtual_ચાવી_Code <= VK_DOWN)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_ચાવી_Map[i].chAlone;
			    જો ((nModifs & SHIFT) != 0
						    && (nModifs & ~SHIFT) == 0)
			    {
				*pch2 = Virt_ચાવી_Map[i].chShift;
			    }
			    જો ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_ચાવી_Map[i].chCtrl;
				જો (pker->w_Virtual_ચાવી_Code == VK_UP
				    || pker->w_Virtual_ચાવી_Code == VK_DOWN)
				{
				    *pmodifiers |= MOD_MASK_CTRL;
				    *pch2 = Virt_ચાવી_Map[i].chAlone;
				}
			    }
			    જો ((nModifs & SHIFT) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				*pch2 = Virt_ચાવી_Map[i].chShift;
			    }
			    જો ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_ચાવી_Map[i].chAlt;
				*pmodifiers |= MOD_MASK_ALT;
				જો ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_ચાવી_Map[i].chAlone;
				}
				નહીંતર જો ((nModifs & SHIFT) != 0)
				{
				    *pch2 = Virt_ચાવી_Map[i].chShift;
				}
				નહીંતર જો ((nModifs & CTRL) != 0)
				{
				    જો (pker->w_Virtual_ચાવી_Code == VK_UP
					|| pker->w_Virtual_ચાવી_Code == VK_DOWN)
				    {
					*pmodifiers |= MOD_MASK_CTRL;
					*pch2 = Virt_ચાવી_Map[i].chAlone;
				    }
				    નહીંતર
				    {
					*pch2 = Virt_ચાવી_Map[i].chCtrl;
				    }
				}
			    }
			}
			નહીંતર જો (pker->w_Virtual_ચાવી_Code == VK_ઉમેરો
					&& (nModifs & SHIFT) != 0
					&& (nModifs & ~SHIFT) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_ચાવી_Map[i].chShift;
			}
			નહીંતર
			{
			    *pch2 = Virt_ચાવી_Map[i].chAlone;
			    જો ((nModifs & SHIFT) != 0)
				*pmodifiers |= MOD_MASK_SHIFT;
			    જો ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_MASK_CTRL;
			    જો ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_MASK_ALT;
			}
		    }
		}

		પાછા TRUE;
	    }
	}
    }

    i = win32_kbd_patch_ચાવી(pker);

    જો (i < 0)
	*pch = NUL;
    નહીંતર
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	જો (pmodifiers != ખાલી)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    જો ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_MASK_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    જો ((nModifs & SHIFT) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_MASK_SHIFT;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    જો ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_MASK_CTRL;
	}
    }

    પાછા (*pch != NUL);
}

# જો defined(FEAT_EVAL)
    સ્થિર પૂર્ણાંક
encode_ચાવી_event(dict_T *args, આગત_RECORD *ir)
{
    સ્થિર પૂર્ણાંક s_dwMods = 0;

    char_u *action = dict_get_string(args, "event", TRUE);
    જો (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_ચાવી_Down = STRICMP(action, "keydown") == 0;
	WORD vkCode = dict_get_સંખ્યા_def(args, "keycode", 0);
	જો (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    પાછા FALSE;
	}

	ir->EventType = ચાવી_EVENT;
	ચાવી_EVENT_RECORD ker;
	ZeroMemory(&ker, કદ(ker));
	ker.b_ચાવી_Down = is_ચાવી_Down;
	ker.w_Repeat_ગણતરી = 1;
	ker.wVirtualScanCode = 0;
	ker.dw_Control_ચાવી_State = 0;
	પૂર્ણાંક mods = (પૂર્ણાંક)dict_get_સંખ્યા(args, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	જો (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    જો (mods & MOD_MASK_SHIFT)
		ker.dw_Control_ચાવી_State |= SHIFT_PRESSED;
	    જો (mods & MOD_MASK_CTRL)
		ker.dw_Control_ચાવી_State |= ડાબું_CTRL_PRESSED;
	    જો (mods & MOD_MASK_ALT)
		ker.dw_Control_ચાવી_State |= ડાબું_ALT_PRESSED;
	}

	જો (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT)
	{
	    જો (is_ચાવી_Down)
		s_dwMods |= SHIFT_PRESSED;
	    નહીંતર
		s_dwMods &= ~SHIFT_PRESSED;
	}
	નહીંતર જો (vkCode == VK_LCONTROL || vkCode == VK_CONTROL)
	{
	    જો (is_ચાવી_Down)
		s_dwMods |= ડાબું_CTRL_PRESSED;
	    નહીંતર
		s_dwMods &= ~ડાબું_CTRL_PRESSED;
	}
	નહીંતર જો (vkCode == VK_RCONTROL)
	{
	    જો (is_ચાવી_Down)
		s_dwMods |= જમણું_CTRL_PRESSED;
	    નહીંતર
		s_dwMods &= ~જમણું_CTRL_PRESSED;
	}
	નહીંતર જો (vkCode == VK_LMENU || vkCode == VK_MENU)
	{
	    જો (is_ચાવી_Down)
		s_dwMods |= ડાબું_ALT_PRESSED;
	    નહીંતર
		s_dwMods &= ~ડાબું_ALT_PRESSED;
	}
	નહીંતર જો (vkCode == VK_RMENU)
	{
	    જો (is_ચાવી_Down)
		s_dwMods |= જમણું_ALT_PRESSED;
	    નહીંતર
		s_dwMods &= ~જમણું_ALT_PRESSED;
	}
	ker.dw_Control_ચાવી_State |= s_dwMods;
	ker.w_Virtual_ચાવી_Code = vkCode;
	ker.uChar.UnicodeChar = 0;
	ir->Event.ચાવી_Event_2 = ker;
	vim_free(action);
    }
    નહીંતર
    {
	જો (action == ખાલી)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	નહીંતર
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str_str), "event", action);
	    vim_free(action);
	}
	પાછા FALSE;
    }
    પાછા TRUE;
}
# endif  // FEAT_EVAL
#endif // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
સ્થિર પૂર્ણાંક g_fMouseAvail = FALSE;   // mouse present
સ્થિર પૂર્ણાંક g_fMouseActive = FALSE;  // mouse enabled
સ્થિર પૂર્ણાંક g_nMouseClick = -1;	    // mouse status
સ્થિર પૂર્ણાંક g_xMouse;		    // mouse x coordinate
સ્થિર પૂર્ણાંક g_yMouse;		    // mouse y coordinate
સ્થિર DWORD g_cmodein = 0;	    // Original console input mode
સ્થિર DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    શૂન્ય
mch_setmouse(પૂર્ણાંક on)
{
    DWORD cmodein;

# ifdef VIMDLL
    જો (gui.in_use)
	પાછા;
# endif
    જો (!g_fMouseAvail)
	પાછા;

    g_fMouseActive = on;
    GetConsoleMode(g_hConIn, &cmodein);

    જો (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_આગત;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    નહીંતર
    {
	cmodein &= ~ENABLE_MOUSE_આગત;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }

    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
}


# જો defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    શૂન્ય
mch_bevalterm_changed(શૂન્ય)
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
    સ્થિર શૂન્ય
decode_mouse_wheel(MOUSE_EVENT_RECORD *pmer)
{
    પૂર્ણાંક	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    પૂર્ણાંક	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    પૂર્ણાંક lcol = g_xMouse;
    પૂર્ણાંક lrow = g_yMouse;
    win_T *wp = mouse_find_win(&lrow, &lcol, FIND_POPUP);
    જો (wp != ખાલી && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_FIELD(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	જો (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ડાબું : MSCR_જમણું;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	નહીંતર
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
    જો (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    નહીંતર
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    જો (pmer->dw_Control_ચાવી_State & SHIFT_PRESSED)
	modifiers |= MOD_MASK_SHIFT; // MOUSE_SHIFT;
    જો (pmer->dw_Control_ચાવી_State & (જમણું_CTRL_PRESSED | ડાબું_CTRL_PRESSED))
	modifiers |= MOD_MASK_CTRL; // MOUSE_CTRL;
    જો (pmer->dw_Control_ચાવી_State & (જમણું_ALT_PRESSED  | ડાબું_ALT_PRESSED))
	modifiers |= MOD_MASK_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    પાછા;
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
    સ્થિર BOOL
decode_mouse_event(
    MOUSE_EVENT_RECORD *pmer)
{
    સ્થિર પૂર્ણાંક s_nOldButton = -1;
    સ્થિર પૂર્ણાંક s_nOldMouseClick = -1;
    સ્થિર પૂર્ણાંક s_xOldMouse = -1;
    સ્થિર પૂર્ણાંક s_yOldMouse = -1;
    સ્થિર linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    સ્થિર પૂર્ણાંક s_old_topfill = 0;
# endif
    સ્થિર પૂર્ણાંક s_cClicks = 1;
    સ્થિર BOOL s_fReleased = TRUE;
    સ્થિર DWORD s_dwLastClickTime = 0;
    સ્થિર BOOL s_f_Next_Is_મધ્ય = FALSE;

    સ્થિર DWORD cButtons = 0;	// number of buttons supported

    અચલ DWORD ડાબું_2 = FROM_ડાબું_1_ST_BUTTON_PRESSED;
    અચલ DWORD મધ્ય = FROM_ડાબું_2_ND_BUTTON_PRESSED;
    અચલ DWORD જમણું_2 = RIGHTMOST_BUTTON_PRESSED;
    અચલ DWORD ડાબું_જમણું = ડાબું_2 | જમણું_2;

    પૂર્ણાંક nButton;

    જો (cButtons == 0 && !Get_સંખ્યા_Of_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    જો (!g_fMouseAvail || !g_fMouseActive)
    {
	g_nMouseClick = -1;
	પાછા FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    જો (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	પાછા FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    જો (g_nMouseClick != -1)
	પાછા TRUE;

    જો (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	પાછા TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    જો (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	જો (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    પાછા FALSE;
    }

    // If no buttons are pressed...
    જો ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	જો (s_fReleased)
	{
# ifdef FEAT_BEVAL_TERM
	    // do return mouse move events when we want them
	    જો (p_bevalterm)
		nButton = MOUSE_DRAG;
	    નહીંતર
# endif
		પાછા FALSE;
	}

	s_fReleased = TRUE;
    }
    નહીંતર    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	જો (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & ડાબું_જમણું);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    જો (dwLR == ડાબું_2 || dwLR == જમણું_2)
	    {
		માટે (;;)
		{
		    // wait a short time for next input event
		    જો (WaitForSingleObject(g_hConIn, p_mouset / 3)
							     != WAIT_OBJECT_0)
			તોડો;
		    નહીંતર
		    {
			DWORD cRecords = 0;
			આગત_RECORD ir;
			MOUSE_EVENT_RECORD* pmer2 = &ir.Event.MouseEvent;

			peek_console_આગત(g_hConIn, &ir, 1, &cRecords);

			જો (cRecords == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & ડાબું_જમણું))
			    તોડો;
			નહીંતર
			{
			    જો (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_આગત(g_hConIn, &ir, 1, &cRecords);

				પાછા decode_mouse_event(pmer2);
			    }
			    નહીંતર જો (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_આગત(g_hConIn, &ir, 1, &cRecords);

				// are there any more mouse events in queue?
				peek_console_આગત(g_hConIn, &ir, 1, &cRecords);

				જો (cRecords==0 || ir.EventType != MOUSE_EVENT)
				    તોડો;
			    }
			    નહીંતર
				તોડો;
			}
		    }
		}
	    }
	}

	જો (s_f_Next_Is_મધ્ય)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_મધ્ય;
	    s_f_Next_Is_મધ્ય = FALSE;
	}
	નહીંતર જો (cButtons == 2	&&
	    ((pmer->dwButtonState & ડાબું_જમણું) == ડાબું_જમણું))
	{
	    nButton = MOUSE_મધ્ય;

	    જો (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_Next_Is_મધ્ય = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	નહીંતર જો ((pmer->dwButtonState & ડાબું_2) == ડાબું_2)
	    nButton = MOUSE_ડાબું;
	નહીંતર જો ((pmer->dwButtonState & મધ્ય) == મધ્ય)
	    nButton = MOUSE_મધ્ય;
	નહીંતર જો ((pmer->dwButtonState & જમણું_2) == જમણું_2)
	    nButton = MOUSE_જમણું;

	જો (! s_fReleased && ! s_f_Next_Is_મધ્ય
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    પાછા FALSE;

	s_fReleased = s_f_Next_Is_મધ્ય;
    }

    જો (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	જો (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dwCurrentTime = Get_Tick_ગણતરી();

	    જો (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (પૂર્ણાંક)(dwCurrentTime - s_dwLastClickTime) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    નહીંતર જો (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dwLastClickTime = dwCurrentTime;
	}
    }
    નહીંતર જો (pmer->dwEventFlags == MOUSE_MOVED)
    {
	જો (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    જો (nButton == -1)
	પાછા FALSE;

    જો (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    જો (pmer->dw_Control_ચાવી_State & SHIFT_PRESSED)
	g_nMouseClick |= MOUSE_SHIFT;
    જો (pmer->dw_Control_ચાવી_State & (જમણું_CTRL_PRESSED | ડાબું_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    જો (pmer->dw_Control_ચાવી_State & (જમણું_ALT_PRESSED  | ડાબું_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    જો (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	SET_અંક_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    જો (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	પાછા FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    પાછા TRUE;
}

# ifdef FEAT_EVAL
    સ્થિર પૂર્ણાંક
encode_mouse_event(dict_T *args, આગત_RECORD *ir)
{
    પૂર્ણાંક		button;
    પૂર્ણાંક		row;
    પૂર્ણાંક		col;
    પૂર્ણાંક		repeated_click;
    int_u	mods = 0;
    પૂર્ણાંક		move;

    જો (!dict_has_ચાવી(args, "row") || !dict_has_ચાવી(args, "col"))
	પાછા FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (પૂર્ણાંક)dict_get_bool(args, "move", FALSE);
    જો (!move && (!dict_has_ચાવી(args, "button")
	    || !dict_has_ચાવી(args, "multiclick")
	    || !dict_has_ચાવી(args, "modifiers")))
	પાછા FALSE;

    row = (પૂર્ણાંક)dict_get_સંખ્યા(args, "row") - 1;
    col = (પૂર્ણાંક)dict_get_સંખ્યા(args, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_RECORD mer;
    ZeroMemory(&mer, કદ(mer));
    mer.dwMousePosition.X  = col;
    mer.dwMousePosition.Y  = row;

    જો (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    નહીંતર
    {
	button = (પૂર્ણાંક)dict_get_સંખ્યા(args, "button");
	repeated_click = (પૂર્ણાંક)dict_get_સંખ્યા(args, "multiclick");
	mods = (પૂર્ણાંક)dict_get_સંખ્યા(args, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	પસંદગી (button)
	{
	    સ્થિતિ MOUSE_ડાબું:
		mer.dwButtonState = FROM_ડાબું_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		તોડો;
	    સ્થિતિ MOUSE_મધ્ય:
		mer.dwButtonState = FROM_ડાબું_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		તોડો;
	    સ્થિતિ MOUSE_જમણું:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		તોડો;
	    સ્થિતિ MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    સ્થિતિ MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		તોડો;
	    સ્થિતિ MOUSE_X1:
		mer.dwButtonState = FROM_ડાબું_3_RD_BUTTON_PRESSED;
		તોડો;
	    સ્થિતિ MOUSE_X2:
		mer.dwButtonState = FROM_ડાબું_4_TH_BUTTON_PRESSED;
		તોડો;
	    સ્થિતિ MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		તોડો;
	    સ્થિતિ MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		તોડો;
	    સ્થિતિ MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		તોડો;
	    સ્થિતિ MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		તોડો;
	    મૂળભૂત:
		semsg(_(e_invalid_argument_str), "button");
		પાછા FALSE;
	}
    }

    mer.dw_Control_ચાવી_State = 0;
    જો (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	જો (mods & MOUSE_SHIFT)
	    mer.dw_Control_ચાવી_State |= SHIFT_PRESSED;
	જો (mods & MOUSE_CTRL)
	    mer.dw_Control_ચાવી_State |= ડાબું_CTRL_PRESSED;
	જો (mods & MOUSE_ALT)
	    mer.dw_Control_ચાવી_State |= ડાબું_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    પાછા TRUE;
}
# endif  // FEAT_EVAL

    સ્થિર પૂર્ણાંક
write_આગત_record_buffer(આગત_RECORD* irEvents, પૂર્ણાંક nLength)
{
    પૂર્ણાંક				n_ગણતરી = 0;
    આગત_record_buffer_ગાંઠ_T	*event_ગાંઠ;

    જ્યારે (n_ગણતરી < nLength)
    {
	event_ગાંઠ = alloc(કદ(આગત_record_buffer_ગાંઠ_T));
	જો (event_ગાંઠ == ખાલી)
	    પાછા -1;

	event_ગાંઠ->ir = irEvents[n_ગણતરી++];
	event_ગાંઠ->next = ખાલી;

	આગત_record_buffer.length++;
	જો (આગત_record_buffer.tail == ખાલી)
	{
	    આગત_record_buffer.head = event_ગાંઠ;
	    આગત_record_buffer.tail = event_ગાંઠ;
	}
	નહીંતર
	{
	    આગત_record_buffer.tail->next = event_ગાંઠ;
	    આગત_record_buffer.tail = આગત_record_buffer.tail->next;
	}
    }
    પાછા n_ગણતરી;
}

    સ્થિર પૂર્ણાંક
read_આગત_record_buffer(આગત_RECORD* irEvents, પૂર્ણાંક nMaxLength)
{
    પૂર્ણાંક n_ગણતરી = 0;
    જ્યારે (n_ગણતરી < nMaxLength && આગત_record_buffer.head != ખાલી)
    {
	આગત_record_buffer.length--;
	આગત_record_buffer_ગાંઠ_T *કાઢો_head = આગત_record_buffer.head;
	irEvents[n_ગણતરી++] = કાઢો_head->ir;
	આગત_record_buffer.head = કાઢો_head->next;
	vim_free(કાઢો_head);
	જો (આગત_record_buffer.length == 0)
	    આગત_record_buffer.tail = ખાલી;
    }
    પાછા n_ગણતરી;
}
#નહીંતર  // FEAT_GUI_MSWIN && !VIMDLL
    શૂન્ય
mch_setmouse(પૂર્ણાંક on UNUSED)
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
    પૂર્ણાંક
test_mswin_event(char_u *event, dict_T *args)
{
    પૂર્ણાંક lpEventsWritten = 0;

# જો defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    જો (gui.in_use)
	પાછા test_gui_w32_sendevent(event, args);
# endif

# જો defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    આગત_RECORD ir;
    BOOL આગત_encoded = FALSE;
    BOOL execute = FALSE;
    જો (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(args, "execute", FALSE);
	જો (dict_has_ચાવી(args, "event"))
	    આગત_encoded = encode_ચાવી_event(args, &ir);
	નહીંતર જો (!execute)
	{
	    semsg(_(e_missing_argument_str), "event");
	    પાછા FALSE;
	}
    }
    નહીંતર જો (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	આગત_encoded = encode_mouse_event(args, &ir);
    }
    નહીંતર
    {
	semsg(_(e_invalid_કિંમત_for_argument_str_str), "event", event);
	પાછા FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    જો (આગત_encoded)
    {
	જો ((lpEventsWritten = write_આગત_record_buffer(&ir, 1)) < 0)
	{
	    semsg(_(e_out_of_memory), "event");
	    પાછા FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    જો (execute)
    {
	પૂર્ણાંક save_સંદેશ_scroll = સંદેશ_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	સંદેશ_scroll = FALSE;
	ch_log(ખાલી, "test_mswin_event() executing");
	exec_normal(TRUE, TRUE, TRUE);
	સંદેશ_scroll |= save_સંદેશ_scroll;
    }

# endif
    પાછા lpEventsWritten;
}
#endif // FEAT_EVAL

#જો defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    સ્થિર શૂન્ય
mch_set_cursor_shape(પૂર્ણાંક thickness)
{
    જો (vtp_working)
    {
	જો (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    જો (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    નહીંતર
		vtp_printf("\033[0 q");	// default
	}
    }
    નહીંતર
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dwSize = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	SetConsoleCursorInfo(g_hConOut, &ConsoleCursorInfo);
	જો (s_cursor_visible)
	    SetConsoleCursorPosition(g_hConOut, g_coord);
    }
}

    શૂન્ય
mch_update_cursor(શૂન્ય)
{
    પૂર્ણાંક		idx;
    પૂર્ણાંક		thickness;

# ifdef VIMDLL
    જો (gui.in_use)
	પાછા;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    જો (shape_table[idx].shape == SHAPE_BLOCK)
	thickness = 99;	// 100 doesn't work on W95
    નહીંતર
	thickness = shape_table[idx].percentage;
    mch_set_cursor_shape(thickness);
}
#endif

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    સ્થિર શૂન્ય
handle_focus_event(આગત_RECORD ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
    ui_focus_change((પૂર્ણાંક)g_fJustGotFocus);
}

સ્થિર શૂન્ય ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    સ્થિર પૂર્ણાંક
WaitForChar(long msec, પૂર્ણાંક ignore_આગત)
{
    DWORD	    dwNow = 0, dwEndTime = 0;
    આગત_RECORD    ir;
    DWORD	    cRecords;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    પૂર્ણાંક		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    જો (msec > 0)
	// Wait until the specified time has elapsed.
	dwEndTime = Get_Tick_ગણતરી() + msec;
    નહીંતર જો (msec < 0)
	// Wait forever.
	dwEndTime = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    માટે (;;)
    {
	// Only process messages when waiting.
	જો (msec != 0)
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

	જો (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_આગત && આગત_available())
# endif
	   )
	    પાછા TRUE;

	જો (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_ગણતરી();
	    જો ((પૂર્ણાંક)(dwNow - dwEndTime) >= 0)
		તોડો;
	}
	જો (msec != 0)
	{
	    DWORD dwWaitTime = dwEndTime - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    જો (dwWaitTime > 11)
		dwWaitTime = 11;

# ifdef FEAT_MZSCHEME
	    જો (mzthreads_allowed() && p_mzq > 0 && (long)dwWaitTime > p_mzq)
		dwWaitTime = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    જો (dwWaitTime > 10)
	    {
		long	due_time;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_time = check_due_timer();
		જો (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    પાછા FALSE;
		}
		જો (due_time > 0 && dwWaitTime > (DWORD)due_time)
		    dwWaitTime = due_time;
	    }
# endif
	    જો (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    સંદેશ_wait_for_multiple_objects(1, &g_hConIn, FALSE,
				  dwWaitTime, QS_SENDMESSAGE) != WAIT_OBJECT_0
# નહીંતર
		    wait_for_single_object(g_hConIn, dwWaitTime)
							      != WAIT_OBJECT_0
# endif
		    )
		ચાલુ;
	}

	cRecords = 0;
	peek_console_આગત(g_hConIn, &ir, 1, &cRecords);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	જો (State == MODE_CMDLINE && સંદેશ_row == Rows - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_BUFFER_INFO csbi;

	    જો (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	    {
		જો (csbi.dwCursorPosition.Y != સંદેશ_row)
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

	જો (cRecords > 0)
	{
	    જો (ir.EventType == ચાવી_EVENT && ir.Event.ચાવી_Event_2.b_ચાવી_Down)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		જો (ir.Event.ચાવી_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.ચાવી_Event_2.w_Virtual_ચાવી_Code == 13)
		{
		    read_console_આગત(g_hConIn, &ir, 1, &cRecords);
		    ચાલુ;
		}
# endif
		જો (decode_ચાવી_event(&ir.Event.ચાવી_Event_2, &ch, &ch2,
								 ખાલી, FALSE))
		    પાછા TRUE;
	    }

	    read_console_આગત(g_hConIn, &ir, 1, &cRecords);

	    જો (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    નહીંતર જો (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    {
		COORD dwSize = ir.Event.WindowBufferSizeEvent.dwSize;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		જો (dwSize.X != Columns || dwSize.Y != Rows)
		{
		    CONSOLE_SCREEN_BUFFER_INFO csbi;
		    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
		    dwSize.X = csbi.srWindow.જમણું_3 - csbi.srWindow.ડાબું_3 + 1;
		    dwSize.Y = csbi.srWindow.Bottom - csbi.srWindow.ટોચ_2 + 1;
		    જો (dwSize.X != Columns || dwSize.Y != Rows)
		    {
			ResizeConBuf(g_hConOut, dwSize);
			shell_resized();
		    }
		}
	    }
	    નહીંતર જો (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		પાછા TRUE;
	}
	નહીંતર જો (msec == 0)
	    તોડો;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    જો (આગત_available())
	પાછા TRUE;
# endif

    પાછા FALSE;
}

/*
 * return non-zero if a character is available
 */
    પૂર્ણાંક
mch_char_avail(શૂન્ય)
{
# ifdef VIMDLL
    જો (gui.in_use)
	પાછા TRUE;
# endif
    પાછા WaitForChar(0L, FALSE);
}

# જો defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    પૂર્ણાંક
mch_check_messages(શૂન્ય)
{
#  ifdef VIMDLL
    જો (gui.in_use)
	પાછા TRUE;
#  endif
    પાછા WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    સ્થિર શૂન્ય
create_conin(શૂન્ય)
{
    g_hConIn =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES) ખાલી,
			OPEN_EXISTING, 0, (HANDLE)ખાલી);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    સ્થિર WCHAR
tgetch(પૂર્ણાંક *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    માટે (;;)
    {
	આગત_RECORD ir;
	DWORD cRecords = 0;

# ifdef FEAT_CLIENTSERVER
	(શૂન્ય)WaitForChar(-1L, FALSE);
	જો (આગત_available())
	    પાછા 0;
	જો (g_nMouseClick != -1)
	    પાછા 0;
# endif
	જો (read_console_આગત(g_hConIn, &ir, 1, &cRecords) == 0)
	{
	    જો (did_create_conin)
		read_error_exit();
	    create_conin();
	    ચાલુ;
	}

	જો (ir.EventType == ચાવી_EVENT)
	{
	    જો (decode_ચાવી_event(&ir.Event.ચાવી_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		પાછા ch;
	}
	નહીંતર જો (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	નહીંતર જો (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    shell_resized();
	નહીંતર જો (ir.EventType == MOUSE_EVENT)
	{
	    જો (decode_mouse_event(&ir.Event.MouseEvent))
		પાછા 0;
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
    પૂર્ણાંક
mch_inchar(
    char_u	*buf UNUSED,
    પૂર્ણાંક		maxlen UNUSED,
    long	time UNUSED,
    પૂર્ણાંક		tb_change_cnt UNUSED)
{
#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    પૂર્ણાંક		len;
    પૂર્ણાંક		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# નહીંતર
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    સ્થિર char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    સ્થિર પૂર્ણાંક	    typeaheadlen = 0;

# ifdef VIMDLL
    જો (gui.in_use)
	પાછા 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    જો (typeaheadlen > 0)
	જાઓ theend;

    જો (time >= 0)
    {
	જો (!WaitForChar(time, FALSE))     // no character available
	    પાછા 0;
    }
    નહીંતર    // time == -1, wait forever
    {
	mch_set_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	જો (!WaitForChar(p_ut, FALSE))
	{
	    જો (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (પૂર્ણાંક)KE_CURSORHOLD;
		પાછા 3;
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
    જો (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    જ્યારે ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	જો (typebuf_changed(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    તોડો;
	}
	જો (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    જો (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    જો (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		જો (modifiers > 0)
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
	    નહીંતર
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
	નહીંતર
	{
	    WCHAR	ch2 = NUL;
	    પૂર્ણાંક		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_ચાવી(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_ચાવીઓ;
	    c = may_adjust_ચાવી_for_ctrl(modifiers, c);
	    --no_reduce_ચાવીઓ;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_shift_modifier(modifiers, c);

	    જો (typebuf_changed(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		તોડો;
	    }

	    જો (c == Ctrl_C && ctrl_c_interrupts)
	    {
# જો defined(FEAT_CLIENTSERVER)
		trash_આગત_buf();
# endif
		got_int = TRUE;
	    }

	    જો (g_nMouseClick == -1)
	    {
		પૂર્ણાંક	n = 1;

		જો (ch2 == NUL)
		{
		    પૂર્ણાંક	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    જો (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_to_enc(ch, &n);
		    જો (p != ખાલી)
		    {
			માટે (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    જો (p[i] == CSI)
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
		નહીંતર
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    જો (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		જો (ch2 != NUL)
		{
		    જો (c == K_NUL)
		    {
			પસંદગી (ch2)
			{
			સ્થિતિ (WCHAR)'\324': // SHIFT+Insert
			સ્થિતિ (WCHAR)'\325': // CTRL+Insert
			સ્થિતિ (WCHAR)'\327': // SHIFT+Delete
			સ્થિતિ (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    તોડો;

			મૂળભૂત:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    તોડો;
			}
		    }
		    નહીંતર
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
		જો ((modifiers & MOD_MASK_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_MASK_ALT;
		}

		જો (modifiers != 0)
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
		જો (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    જો (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    જ્યારે (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    જો (len > 0)
    {
	buf[len] = NUL;
	ch_log(ખાલી, "raw key input: \"%s\"", buf);
    }
# endif
    પાછા len;

#નહીંતર // FEAT_GUI_MSWIN && !VIMDLL
    પાછા 0;
#endif // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    સ્થિર પૂર્ણાંક
executable_file(અક્ષર *name, char_u **path)
{
    પૂર્ણાંક attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    જો (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	પાછા FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    જો (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	જો (res == ખાલી)
	    res = resolve_reparse_point((char_u *)name);
	જો (res == ખાલી)
	    પાછા FALSE;
	// The path is already absolute.
	જો (path != ખાલી)
	    *path = res;
	નહીંતર
	    vim_free(res);
    }
    નહીંતર જો (path != ખાલી)
	*path = FullName_save((char_u *)name, FALSE);
    પાછા TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    સ્થિર bool
executable_prefix_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_FIND_DATAW	data;
    HANDLE		handle;
    DWORD		error;

    wn = enc_to_utf16(name, ખાલી);
    જો (wn == ખાલી)
	પાછા true;
    handle = FindFirstFileExW(wn, FindExInfoBasic, &data,
					    Find_Ex_શોધ_Name_Match, ખાલી, 0);
    error = GetLastError();
    vim_free(wn);
    જો (handle != INVALID_HANDLE_કિંમત)
    {
	FindClose(handle);
	પાછા true;
    }
    પાછા error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    સ્થિર પૂર્ણાંક
executable_exists(
    અક્ષર	*name,
    size_t	namelen,
    char_u	**path,
    પૂર્ણાંક		use_path,
    પૂર્ણાંક		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3];
    char_u	*p;
    string_T	pathbuf = {ખાલી, 0};
    પૂર્ણાંક		pathbuf_allocated = FALSE;
    string_T	pathext = {ખાલી, 0};
    પૂર્ણાંક		pathext_allocated = FALSE;
    char_u	*shname = ખાલી;
    પૂર્ણાંક		noext = FALSE;
    bool	simple_pathext = true;
    પૂર્ણાંક		ext_ગણતરી = 0;
    પૂર્ણાંક		retval = FALSE;

    જો (namelen >= કદ(buf))	// safety check
	પાછા FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    જો (strstr((અક્ષર *)shname, "sh") != ખાલી &&
	!(strstr((અક્ષર *)shname, "powershell") != ખાલી
				    || strstr((અક્ષર *)shname, "pwsh") != ખાલી))
	noext = TRUE;

    જો (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	જો (pathext.string == ખાલી)
	    STR_LITERAL_SET(pathext, ".com;.exe;.bat;.cmd");
	નહીંતર
	    pathext.length = STRLEN(pathext.string);

	જો (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    જ્યારે (*p)
	    {
		જો (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    જો (*p == '.')
			simple_pathext = false;
		    ++p;
		    ચાલુ;
		}
		e = vim_strchr(p, ';');
		જો (e == ખાલી)
		    e = pathext.string + pathext.length;
		plen = (size_t)(e - p);
		++ext_ગણતરી;
		જો (*p != '.')
		    simple_pathext = false;
		માટે (q = p + 1; q < e; ++q)
		    જો (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		જો (_strnicoll(name + namelen - plen, (અક્ષર *)p, plen) == 0)
		{
		    noext = TRUE;
		    તોડો;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    જો (pathext.string == ખાલી)
	STR_LITERAL_SET(pathext, ".");
    નહીંતર જો (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.length + 3);
	જો (tmp == ખાલી)
	{
	    retval = FALSE;
	    જાઓ theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.length += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    જો (use_path && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	જો (p != ખાલી)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    જો (pathbuf.string == ખાલી)
	    {
		retval = FALSE;
		જાઓ theend;
	    }

	    જો (mch_getenv("NoDefaultCurrentDirectoryInExePath") == ખાલી &&
		    strstr((અક્ષર *)gettail(p_sh), "cmd.exe") != ખાલી)
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
    જો (pathbuf.string == ખાલી)
	STR_LITERAL_SET(pathbuf, ".");
    p = pathbuf.string;
    જ્યારે (*p)
    {
	char_u	*e;
	size_t	buflen;

	જો (*p == ';') // Skip empty entry
	{
	    ++p;
	    ચાલુ;
	}
	e = vim_strchr(p, ';');
	જો (e == ખાલી)
	    e = pathbuf.string + pathbuf.length;

	જો (e - p + namelen + 2 > કદ(buf))
	{
	    retval = FALSE;
	    જાઓ theend;
	}
	// A single "." that means current dir.
	જો (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	નહીંતર
	{
	    buflen = vim_snprintf_safelen(
		(અક્ષર *)buf,
		કદ(buf),
		"%.*s%s%s", (પૂર્ણાંક)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	જો (!noext && simple_pathext && ext_ગણતરી >= 3 && buflen > 0
		&& buflen + 2 <= કદ(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == ખાલી
		&& vim_strchr(gettail(buf), ':') == ખાલી)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_prefix_exists(buf);
	    buf[buflen] = NUL;
	    જો (!exists)
	    {
		p = e;
		ચાલુ;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	જ્યારે (*p)
	{
	    char_u  *e2;

	    જો (*p == ';')
	    {
		// Skip empty entry
		++p;
		ચાલુ;
	    }
	    e2 = vim_strchr(p, ';');
	    જો (e2 == ખાલી)
		e2 = pathext.string + pathext.length;

	    જો (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		જો (e2 - p + buflen + 1 > કદ(buf))
		{
		    retval = FALSE;
		    જાઓ theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    જો (executable_file((અક્ષર *)buf, path))
	    {
		retval = TRUE;
		જાઓ theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    જો (pathbuf_allocated)
	મુક્ત(pathbuf.string);
    જો (pathext_allocated)
	મુક્ત(pathext.string);
    પાછા retval;
}

#જો (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    સ્થિર શૂન્ય
bad_param_handler(અચલ wchar_t *expression UNUSED,
    અચલ wchar_t *function UNUSED,
    અચલ wchar_t *file UNUSED,
    unsigned પૂર્ણાંક લીટી UNUSED,
    uintptr_t pReserved UNUSED)
{
}

# define SET_INVALID_PARAM_HANDLER \
	((શૂન્ય)_set_invalid_parameter_handler(bad_param_handler))
#નહીંતર
# define SET_INVALID_PARAM_HANDLER
#endif

#ifdef FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    સ્થિર શૂન્ય
mch_init_g(શૂન્ય)
{
# ifndef __MINGW32__
    બાહ્ય પૂર્ણાંક _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_દ્વિઅંકી;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    Rows = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[_MAX_PATH + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	જો (exe_pathlen + 10 >= કદ(vimrun_location))
	{
	    જો (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ખાલી, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	નહીંતર
	{
	    // First try in same directory as gvim.exe.
	    જો (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    જો (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		જો (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(અક્ષર *)vimrun_location,
			કદ(vimrun_location),
			"\"%.*svimrun\" ",
			(પૂર્ણાંક)exe_pathlen, exe_name);
		}
		નહીંતર
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		જો (p != ખાલી)
		{
		    vimrun_path.string = p;
		    vimrun_path.length = plen;
		    vimrun_path_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    નહીંતર જો (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ખાલી, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	જો (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    જો (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    ખાલી, TRUE, FALSE))
	set_option_કિંમત_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#endif // FEAT_GUI_MSWIN

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).જમણું_3 - (sr).ડાબું_3 + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).ટોચ_2 + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    સ્થિર BOOL
ClearConsoleBuffer(WORD wAttribute)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    DWORD અંક_Cells, dummy;

    જો (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	પાછા FALSE;

    અંક_Cells = csbi.dwSize.X * csbi.dwSize.Y;
    coord.X = 0;
    coord.Y = 0;
    જો (!FillConsoleOutputCharacter(g_hConOut, ' ', અંક_Cells,
	    coord, &dummy))
	પાછા FALSE;
    જો (!FillConsoleOutputAttribute(g_hConOut, wAttribute, અંક_Cells,
	    coord, &dummy))
	પાછા FALSE;

    પાછા TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    સ્થિર BOOL
FitConsoleWindow(
    COORD dwBufferSize,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD dwWindowSize;
    BOOL NeedAdjust = FALSE;

    જો (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	પાછા FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    જો (csbi.srWindow.જમણું_3 >= dwBufferSize.X)
    {
	dwWindowSize.X = SRWIDTH(csbi.srWindow);
	જો (dwWindowSize.X > dwBufferSize.X)
	    dwWindowSize.X = dwBufferSize.X;
	csbi.srWindow.જમણું_3 = dwBufferSize.X - 1;
	csbi.srWindow.ડાબું_3 = dwBufferSize.X - dwWindowSize.X;
	NeedAdjust = TRUE;
    }
    જો (csbi.srWindow.Bottom >= dwBufferSize.Y)
    {
	dwWindowSize.Y = SRHEIGHT(csbi.srWindow);
	જો (dwWindowSize.Y > dwBufferSize.Y)
	    dwWindowSize.Y = dwBufferSize.Y;
	csbi.srWindow.Bottom = dwBufferSize.Y - 1;
	csbi.srWindow.ટોચ_2 = dwBufferSize.Y - dwWindowSize.Y;
	NeedAdjust = TRUE;
    }
    જો (NeedAdjust && WantAdjust)
    {
	જો (!SetConsoleWindowInfo(g_hConOut, TRUE, &csbi.srWindow))
	    પાછા FALSE;
    }
    પાછા TRUE;
}

પ્રકારવ્યાખ્યા સંરચના ConsoleBufferStruct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_BUFFER_INFO	Info;
    PCHAR_INFO			Buffer;
    COORD			BufferSize;
    PSMALL_RECT			Regions;
    પૂર્ણાંક				અંક_Regions;
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
    સ્થિર BOOL
SaveConsoleBuffer(
    ConsoleBuffer *cb)
{
    DWORD અંક_Cells;
    COORD BufferCoord;
    SMALL_RECT ReadRegion;
    WORD Y, Y_incr;
    પૂર્ણાંક i, numregions;

    જો (cb == ખાલી)
	પાછા FALSE;

    જો (!GetConsoleScreenBufferInfo(g_hConOut, &cb->Info))
    {
	cb->IsValid = FALSE;
	પાછા FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    જો (use_alternate_screen_buffer)
	પાછા TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    જો (!cb->IsValid || cb->Buffer == ખાલી ||
	    cb->BufferSize.X != cb->Info.dwSize.X ||
	    cb->BufferSize.Y != cb->Info.dwSize.Y)
    {
	cb->BufferSize.X = cb->Info.dwSize.X;
	cb->BufferSize.Y = cb->Info.dwSize.Y;
	અંક_Cells = cb->BufferSize.X * cb->BufferSize.Y;
	vim_free(cb->Buffer);
	cb->Buffer = ALLOC_MULT(CHAR_INFO, અંક_Cells);
	જો (cb->Buffer == ખાલી)
	    પાછા FALSE;
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
    ReadRegion.ડાબું_3 = 0;
    ReadRegion.જમણું_3 = cb->Info.dwSize.X - 1;
    Y_incr = 12000 / cb->Info.dwSize.X;

    numregions = (cb->Info.dwSize.Y + Y_incr - 1) / Y_incr;
    જો (cb->Regions == ખાલી || numregions != cb->અંક_Regions)
    {
	cb->અંક_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->અંક_Regions);
	જો (cb->Regions == ખાલી)
	{
	    VIM_CLEAR(cb->Buffer);
	    પાછા FALSE;
	}
    }

    માટે (i = 0, Y = 0; i < cb->અંક_Regions; i++, Y += Y_incr)
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
	ReadRegion.ટોચ_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	જો (!ReadConsoleOutputW(g_hConOut,	// output handle
		cb->Buffer,			// our buffer
		cb->BufferSize,			// dimensions of our buffer
		BufferCoord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->Buffer);
	    VIM_CLEAR(cb->Regions);
	    પાછા FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    પાછા TRUE;
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
    સ્થિર BOOL
RestoreConsoleBuffer(
    ConsoleBuffer   *cb,
    BOOL	    RestoreScreen)
{
    COORD BufferCoord;
    SMALL_RECT WriteRegion;
    પૂર્ણાંક i;

    જો (cb == ખાલી || !cb->IsValid)
	પાછા FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    જો (RestoreScreen)
	ClearConsoleBuffer(cb->Info.wAttributes);

    FitConsoleWindow(cb->Info.dwSize, TRUE);
    જો (!SetConsoleScreenBufferSize(g_hConOut, cb->Info.dwSize))
	પાછા FALSE;
    જો (!SetConsoleTextAttribute(g_hConOut, cb->Info.wAttributes))
	પાછા FALSE;

    જો (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	પાછા TRUE;
    }

    જો (!SetConsoleCursorPosition(g_hConOut, cb->Info.dwCursorPosition))
	પાછા FALSE;
    જો (!SetConsoleWindowInfo(g_hConOut, TRUE, &cb->Info.srWindow))
	પાછા FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    જો (use_alternate_screen_buffer)
	પાછા TRUE;

    /*
     * Restore the screen buffer contents.
     */
    જો (cb->Buffer != ખાલી)
    {
	માટે (i = 0; i < cb->અંક_Regions; i++)
	{
	    BufferCoord.X = cb->Regions[i].ડાબું_3;
	    BufferCoord.Y = cb->Regions[i].ટોચ_2;
	    WriteRegion = cb->Regions[i];
	    જો (!WriteConsoleOutputW(g_hConOut,	// output handle
			cb->Buffer,		// our buffer
			cb->BufferSize,		// dimensions of our buffer
			BufferCoord,		// offset in our buffer
			&WriteRegion))		// region to restore
		પાછા FALSE;
	}
    }

    પાછા TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
સ્થિર ConsoleBuffer g_cbOrig = { 0 };
# endif
સ્થિર ConsoleBuffer g_cbNonTermcap = { 0 };
સ્થિર ConsoleBuffer g_cbTermcap = { 0 };

અક્ષર g_szOrigTitle[256] = { 0 };
HWND g_hWnd = ખાલી;	// also used in os_mswin.c
સ્થિર HICON g_hOrigIconSmall = ખાલી;
સ્થિર HICON g_hOrigIcon = ખાલી;
સ્થિર HICON g_hVimIcon = ખાલી;
સ્થિર BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    સ્થિર BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    જો (hWnd == ખાલી)
	પાછા FALSE;

    જો (phIconSmall != ખાલી)
	*phIconSmall = (HICON)SendMessage(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    જો (phIcon != ખાલી)
	*phIcon = (HICON)SendMessage(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    પાછા TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    સ્થિર BOOL
SetConsoleIcon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    જો (hWnd == ખાલી)
	પાછા FALSE;

    જો (hIconSmall != ખાલી)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    જો (hIcon != ખાલી)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    પાછા TRUE;
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
    સ્થિર શૂન્ય
SaveConsoleTitleAndIcon(શૂન્ય)
{
    // Save the original title.
    જો (!GetConsoleTitle(g_szOrigTitle, કદ(g_szOrigTitle)))
	પાછા;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = GetConsoleWindow();
    જો (g_hWnd == ખાલી)
	પાછા;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    જો (g_hOrigIconSmall == ખાલી || g_hOrigIcon == ખાલી)
	પાછા;

    // Extract the first icon contained in the Vim executable.
    જો (
# ifdef FEAT_LIBCALL
	    mch_icon_load((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == ખાલી)
	g_hVimIcon = ExtractIcon(ખાલી, (LPCSTR)exe_name, 0);
    જો (g_hVimIcon != ખાલી)
	g_fCanChangeIcon = TRUE;
}

સ્થિર પૂર્ણાંક g_fWindInitCalled = FALSE;
સ્થિર પૂર્ણાંક g_fTermcapMode = FALSE;
સ્થિર CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    સ્થિર શૂન્ય
mch_init_c(શૂન્ય)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_BUFFER_INFO csbi;
# endif
# ifndef __MINGW32__
    બાહ્ય પૂર્ણાંક _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_દ્વિઅંકી;		// we do our own CR-LF translation
    out_flush();

    // Obtain handles for the standard Console I/O devices
    જો (read_cmd_fd == 0)
	g_hConIn =  GetStdHandle(STD_આગત_HANDLE);
    નહીંતર
	create_conin();
    g_hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    SaveConsoleBuffer(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.wAttributes;
# નહીંતર
    // Get current text attributes
    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
# endif
    જો (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (g_attrCurrent & 0xf) + 1;
    જો (cterm_normal_bg_color == 0)
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
    જો (g_fCanChangeIcon)
	SetConsoleIcon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    જો (use_alternate_screen_buffer)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    જો (fdDump)
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
    સ્થિર શૂન્ય
mch_exit_c(પૂર્ણાંક r)
{
    // Copy flag since stoptermcap() will clear the flag.
    પૂર્ણાંક fTermcapMode = g_fTermcapMode;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    જો (!fTermcapMode &&  use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    જો (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    જો (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	જો (g_fCanChangeIcon && !g_fForceExit)
	    SetConsoleIcon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	જો (fdDump)
	{
	    time_t t;

	    time(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = ખાલી;
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

    શૂન્ય
mch_init(શૂન્ય)
{
#ifdef VIMDLL
    જો (gui.starting)
	mch_init_g();
    નહીંતર
	mch_init_c();
#અથવાજો defined(FEAT_GUI_MSWIN)
    mch_init_g();
#નહીંતર
    mch_init_c();
#endif
}

    શૂન્ય
mch_exit(પૂર્ણાંક r)
{
#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef VIMDLL
    જો (vimrun_path_allocated)
	vim_free(vimrun_path.string);

    જો (gui.in_use || gui.starting)
	mch_exit_g(r);
    નહીંતર
	mch_exit_c(r);
#અથવાજો defined(FEAT_GUI_MSWIN)
    જો (vimrun_path_allocated)
	vim_free(vimrun_path.string);
    mch_exit_g(r);
#નહીંતર
    mch_exit_c(r);
#endif
}

/*
 * Do we have an interactive window?
 */
    પૂર્ણાંક
mch_check_win(
    પૂર્ણાંક argc UNUSED,
    અક્ષર **argv UNUSED)
{
    mch_get_exe_name();

#જો defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    પાછા OK;	    // GUI always has a tty
#નહીંતર
# ifdef VIMDLL
    જો (gui.in_use)
	પાછા OK;
# endif
    જો (isatty(1))
	પાછા OK;
    પાછા FAIL;
#endif
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    શૂન્ય
fname_case(
    char_u	*name,
    પૂર્ણાંક		len)
{
    WCHAR   *p;
    WCHAR   buf[_MAX_PATH + 1];

    જો (*name == NUL)
	પાછા;

    slash_adjust(name);

    p = enc_to_utf16(name, ખાલી);
    જો (p == ખાલી)
	પાછા;

    જો (GetLongPathNameW(p, buf, _MAX_PATH))
    {
	char_u	*q = utf16_to_enc(buf, ખાલી);

	જો (q != ખાલી)
	{
	    જો (len > 0)
		vim_strncpy(name, q, len - 1);
	    નહીંતર
	    {
		size_t  namelen = STRLEN(name);

		જો (namelen >= STRLEN(q))
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
    પૂર્ણાંક
mch_get_user_name(
    char_u  *s,
    પૂર્ણાંક	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_LENGTH(wszUserName);

    જો (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_to_enc(wszUserName, ખાલી);

	જો (p != ખાલી)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    પાછા OK;
	}
    }
    s[0] = NUL;
    પાછા FAIL;
}


/*
 * Insert host name in s[len].
 */
    શૂન્ય
mch_get_host_name(
    char_u	*s,
    પૂર્ણાંક		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_LENGTH(wszHostName);

    જો (!GetComputerNameW(wszHostName, &wcch))
	પાછા;

    char_u  *p = utf16_to_enc(wszHostName, ખાલી);
    જો (p == ખાલી)
	પાછા;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    long
mch_get_pid(શૂન્ય)
{
    પાછા (long)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    પૂર્ણાંક
mch_process_running(long pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   status = 0;
    પૂર્ણાંક	    ret = FALSE;

    જો (hProcess == ખાલી)
	પાછા FALSE;  // might not have access
    જો (GetExitCodeProcess(hProcess, &status))
	ret = status == STILL_ACTIVE;
    CloseHandle(hProcess);
    પાછા ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    પૂર્ણાંક
mch_dirname(
    char_u	*buf,
    પૂર્ણાંક		len)
{
    WCHAR   wbuf[_MAX_PATH + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    જો (GetCurrentDirectoryW(_MAX_PATH, wbuf) == 0)
	પાછા FAIL;

    WCHAR   wcbuf[_MAX_PATH + 1];
    char_u  *p = ખાલી;

    જો (GetLongPathNameW(wbuf, wcbuf, _MAX_PATH) != 0)
    {
	p = utf16_to_enc(wcbuf, ખાલી);
	જો (p != ખાલી)
	{
	    જો (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    જો (p == ખાલી)
	p = utf16_to_enc(wbuf, ખાલી);

    જો (p == ખાલી)
	પાછા FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    પાછા OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    long
mch_getperm(char_u *name)
{
    stat_T	st;

    પાછા (mch_stat((અક્ષર *)name, &st) == 0)
	? (long)(unsigned short)st.st_mode
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
mch_setperm(char_u *name, long perm)
{
    long	n;
    WCHAR	*p;

    p = enc_to_utf16(name, ખાલી);
    જો (p == ખાલી)
	પાછા FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    જો (n == -1)
	પાછા FAIL;

    win32_set_archive(name);

    પાછા OK;
}

/*
 * Set hidden flag for "name".
 */
    શૂન્ય
mch_hide(char_u *name)
{
    પૂર્ણાંક attrs = win32_getattrs(name);
    જો (attrs == -1)
	પાછા;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    પૂર્ણાંક
mch_ishidden(char_u *name)
{
    પૂર્ણાંક f = win32_getattrs(name);

    જો (f == -1)
	પાછા FALSE;		    // file does not exist at all

    પાછા (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    પૂર્ણાંક
mch_isdir(char_u *name)
{
    પૂર્ણાંક f = win32_getattrs(name);

    જો (f == -1)
	પાછા FALSE;		    // file does not exist at all

    પાછા (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    પૂર્ણાંક
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_to_utf16(name, ખાલી);
    જો (p == ખાલી)
	પાછા FALSE;
    attr = GetFileAttributesW(p);
    જો (attr == INVALID_FILE_ATTRIBUTES)
	error = GetLastError();
    vim_free(p);

    જો (attr == INVALID_FILE_ATTRIBUTES)
	પાછા error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    પાછા (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    પૂર્ણાંક
mch_isrealdir(char_u *name)
{
    પાછા mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    પૂર્ણાંક
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    પૂર્ણાંક	    retval;

    p = enc_to_utf16(name, ખાલી);
    જો (p == ખાલી)
	પાછા -1;
    retval = _wmkdir(p);
    vim_free(p);
    પાછા retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    પૂર્ણાંક
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    પૂર્ણાંક	    retval;

    p = enc_to_utf16(name, ખાલી);
    જો (p == ખાલી)
	પાછા -1;
    retval = _wrmdir(p);
    vim_free(p);
    પાછા retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    પૂર્ણાંક
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    પાછા win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_સંખ્યા_Of_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    પૂર્ણાંક
mch_is_symbolic_link(char_u *name)
{
    HANDLE		hFind;
    પૂર્ણાંક			res = FALSE;
    DWORD		fileFlags = 0, reparseTag = 0;
    WCHAR		*wn;
    WIN32_FIND_DATAW	findDataW;

    wn = enc_to_utf16(name, ખાલી);
    જો (wn == ખાલી)
	પાછા FALSE;

    hFind = FindFirstFileW(wn, &findDataW);
    vim_free(wn);
    જો (hFind != INVALID_HANDLE_કિંમત)
    {
	fileFlags = findDataW.dwFileAttributes;
	reparseTag = findDataW.dwReserved0;
	FindClose(hFind);
    }

    જો ((fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
	    && (reparseTag == IO_REPARSE_TAG_SYMLINK
		|| reparseTag == IO_REPARSE_TAG_MOUNT_POINT))
	res = TRUE;

    પાછા res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    પૂર્ણાંક
mch_is_linked(char_u *fname)
{
    જો (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	પાછા TRUE;
    પાછા FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    પૂર્ણાંક
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    પૂર્ણાંક		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_to_utf16(fname, ખાલી);
    જો (wn == ખાલી)
	પાછા FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    ખાલી,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    ખાલી);		// handle to template file
    vim_free(wn);

    જો (hFile == INVALID_HANDLE_કિંમત)
	પાછા FILEINFO_READ_FAIL;

    જો (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    નહીંતર
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    પાછા res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    સ્થિર પૂર્ણાંક
win32_getattrs(char_u *name)
{
    પૂર્ણાંક		attr;
    WCHAR	*p;

    p = enc_to_utf16(name, ખાલી);
    જો (p == ખાલી)
	પાછા INVALID_FILE_ATTRIBUTES;

    attr = GetFileAttributesW(p);
    vim_free(p);

    પાછા attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    સ્થિર પૂર્ણાંક
win32_setattrs(char_u *name, પૂર્ણાંક attrs)
{
    પૂર્ણાંક	    res;
    WCHAR   *p;

    p = enc_to_utf16(name, ખાલી);
    જો (p == ખાલી)
	પાછા -1;

    res = SetFileAttributesW(p, attrs);
    vim_free(p);

    પાછા res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    સ્થિર પૂર્ણાંક
win32_set_archive(char_u *name)
{
    પૂર્ણાંક attrs = win32_getattrs(name);
    જો (attrs == -1)
	પાછા -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    પાછા win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    પૂર્ણાંક
mch_writable(char_u *name)
{
    પૂર્ણાંક attrs = win32_getattrs(name);

    પાછા (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    પૂર્ણાંક
mch_can_exe(char_u *name, char_u **path, પૂર્ણાંક use_path UNUSED)
{
    પાછા executable_exists((અક્ષર *)name, STRLEN(name), path, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    પૂર્ણાંક
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    પૂર્ણાંક		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    જો (STRNCMP(name, "\\\\.\\", 4) == 0)
	પાછા ગાંઠ_WRITABLE;

    wn = enc_to_utf16(name, ખાલી);
    જો (wn == ખાલી)
	પાછા ગાંઠ_NORMAL;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    ખાલી,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    ખાલી);		    // handle to template file
    vim_free(wn);
    જો (hFile == INVALID_HANDLE_કિંમત)
	પાછા ગાંઠ_NORMAL;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    જો (type == FILE_TYPE_CHAR)
	પાછા ગાંઠ_WRITABLE;
    જો (type == FILE_TYPE_DISK)
	પાછા ગાંઠ_NORMAL;
    પાછા ગાંઠ_OTHER;
}

#ifdef HAVE_ACL
સંરચના my_acl
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
    પાછા (vim_acl_T)ખાલી;
#નહીંતર
    સંરચના my_acl   *p = ખાલી;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(સંરચના my_acl);
    જો (p != ખાલી)
    {
	WCHAR	*wn;

	wn = enc_to_utf16(fname, ખાલી);
	જો (wn == ખાલી)
	{
	    vim_free(p);
	    પાછા ખાલી;
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
	જો (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (શૂન્ય)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    ખાલી,
		    ખાલી,
		    &p->pDacl,
		    ખાલી,
		    &p->pSecurityDescriptor);
	}
	જો (p->pSecurityDescriptor == ખાલી)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = ખાલી;
	}
	vim_free(wn);
    }

    પાછા (vim_acl_T)p;
#endif
}

#ifdef HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    સ્થિર BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_SIZE_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_ગણતરી = 0;
    GetAclInformation(acl, &acl_info, કદ(acl_info), AclSizeInformation);
    માટે (i = 0; i < acl_info.Ace_ગણતરી; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	જો (ace->Header.AceFlags & INHERITED_ACE)
	    પાછા TRUE;
    }
    પાછા FALSE;
}
#endif

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    શૂન્ય
mch_set_acl(char_u *fname, vim_acl_T acl)
{
#ifdef HAVE_ACL
    સંરચના my_acl   *p = (સંરચના my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    જો (p == ખાલી)
	પાછા;

    wn = enc_to_utf16(fname, ખાલી);
    જો (wn == ખાલી)
	પાછા;

    // Set security flags
    જો (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    જો (p->pSidGroup)
	sec_info |= GROUP_SECURITY_INFORMATION;
    જો (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	જો (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    જો (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (શૂન્ય)SetNamedSecurityInfoW(
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

    શૂન્ય
mch_free_acl(vim_acl_T acl)
{
#ifdef HAVE_ACL
    સંરચના my_acl   *p = (સંરચના my_acl *)acl;

    જો (p != ખાલી)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#endif
}

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    સ્થિર BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    આગત_RECORD ir;
    DWORD out;

    પસંદગી (dwCtrlType)
    {
    સ્થિતિ CTRL_C_EVENT:
	જો (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	પાછા TRUE;

    સ્થિતિ CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = ચાવી_EVENT;
	ir.Event.ચાવી_Event_2.b_ચાવી_Down = TRUE;
	ir.Event.ચાવી_Event_2.w_Repeat_ગણતરી = 1;
	ir.Event.ચાવી_Event_2.w_Virtual_ચાવી_Code = VK_CANCEL;
	ir.Event.ચાવી_Event_2.wVirtualScanCode = 0;
	ir.Event.ચાવી_Event_2.dw_Control_ચાવી_State = 0;
	ir.Event.ચાવી_Event_2.uChar.UnicodeChar = 0;
	Write_Console_આગત(g_hConIn, &ir, 1, &out);
	પાછા TRUE;

    // fatal events: shut down gracefully
    સ્થિતિ CTRL_CLOSE_EVENT:
    સ્થિતિ CTRL_LOGOFF_EVENT:
    સ્થિતિ CTRL_SHUTDOWN_EVENT:
	windgoto((પૂર્ણાંક)Rows - 1, cmdline_col_off);
	g_fForceExit = TRUE;

	vim_snprintf((અક્ષર *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	પાછા TRUE;		// not reached

    મૂળભૂત:
	પાછા FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    શૂન્ય
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    જો (gui.in_use)
	પાછા;
# endif
    GetConsoleMode(g_hConIn, &cmodein);
    GetConsoleMode(g_hConOut, &cmodeout);
    જો (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_લીટી_આગત | ENABLE_PROCESSED_આગત |
		     ENABLE_ECHO_આગત);
	જો (g_fMouseActive)
	{
	    cmodein |= ENABLE_MOUSE_આગત;
	    cmodein &= ~ENABLE_QUICK_EDIT_MODE;
	}
	નહીંતર
	{
	    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# નહીંતર
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    નહીંતર // cooked
    {
	cmodein |= (ENABLE_લીટી_આગત | ENABLE_PROCESSED_આગત |
		    ENABLE_ECHO_આગત);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
    SetConsoleMode(g_hConOut, cmodeout);
    SetConsoleCtrlHandler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    જો (fdDump)
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
    પૂર્ણાંક
mch_get_shellsize(શૂન્ય)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

# ifdef VIMDLL
    જો (gui.in_use)
	પાછા OK;
# endif
    જો (!g_fTermcapMode && g_cbTermcap.IsValid)
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
    નહીંતર જો (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	Rows = csbi.srWindow.Bottom - csbi.srWindow.ટોચ_2 + 1;
	Columns = csbi.srWindow.જમણું_3 - csbi.srWindow.ડાબું_3 + 1;
    }
    નહીંતર
    {
	Rows = 25;
	Columns = 80;
    }
    પાછા OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    સ્થિર શૂન્ય
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    જો (use_alternate_screen_buffer)
	પાછા;

    જો (!SetConsoleScreenBufferSize(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	જો (fdDump)
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
    સ્થિર શૂન્ય
ResizeWindow(
    HANDLE     hConsole,
    SMALL_RECT srWindowRect)
{
    જો (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
    {
# ifdef MCH_WRITE_DUMP
	જો (fdDump)
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
    સ્થિર શૂન્ય
ResizeConBufAndWindow(
    HANDLE  hConsole,
    પૂર્ણાંક	    xSize,
    પૂર્ણાંક	    ySize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    srWindowRect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    સ્થિર પૂર્ણાંક	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    જો (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", xSize, ySize);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    // define the new console window size and scroll position
    srWindowRect.ડાબું_3 = srWindowRect.ટોચ_2 = (SHORT) 0;
    srWindowRect.જમણું_3 =  (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);

    જો (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	પૂર્ણાંક sx, sy;

	sx = csbi.srWindow.જમણું_3 - csbi.srWindow.ડાબું_3 + 1;
	sy = csbi.srWindow.Bottom - csbi.srWindow.ટોચ_2 + 1;
	જો (sy < ySize || sx < xSize)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    જો (sy < ySize)
		coordScreen.Y = ySize;
	    નહીંતર
		coordScreen.Y = sy;
	    જો (sx < xSize)
		coordScreen.X = xSize;
	    નહીંતર
		coordScreen.X = sx;
	    SetConsoleScreenBufferSize(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = xSize;
    coordScreen.Y = ySize;

    // In the new console call API, only the first time in reverse order
    જો (!vtp_working || resized)
    {
	ResizeWindow(hConsole, srWindowRect);
	ResizeConBuf(hConsole, coordScreen);
    }
    નહીંતર
    {
	// Workaround for a Windows 10 bug
	cursor.X = srWindowRect.ડાબું_3;
	cursor.Y = srWindowRect.ટોચ_2;
	SetConsoleCursorPosition(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	ResizeWindow(hConsole, srWindowRect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    શૂન્ય
mch_set_shellsize(શૂન્ય)
{
    COORD coordScreen;

# ifdef VIMDLL
    જો (gui.in_use)
	પાછા;
# endif
    // Don't change window size while still starting up
    જો (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	પાછા;
    }

    જો (term_console)
    {
	coordScreen = GetLargestConsoleWindowSize(g_hConOut);

	// Clamp Rows and Columns to reasonable values
	જો (Rows > coordScreen.Y)
	    Rows = coordScreen.Y;
	જો (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	ResizeConBufAndWindow(g_hConOut, Columns, Rows);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    શૂન્ય
mch_new_shellsize(શૂન્ય)
{
# ifdef VIMDLL
    જો (gui.in_use)
	પાછા;
# endif
    set_scroll_region(0, 0, Columns - 1, Rows - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    શૂન્ય
mch_set_winsize_now(શૂન્ય)
{
    જો (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_set_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# જો defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    શૂન્ય
mch_calc_cell_size(સંરચના cellsize *cs_out)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_out->cs_xpixel = -1;
    cs_out->cs_ypixel = -1;

    જો (g_hConOut == INVALID_HANDLE_કિંમત)
	પાછા;

    cfi.cbSize = કદ(cfi);
    જો (GetCurrentConsoleFontEx(g_hConOut, FALSE, &cfi)
	    && cfi.dwFontSize.X > 0 && cfi.dwFontSize.Y > 0)
    {
	cs_out->cs_xpixel = cfi.dwFontSize.X;
	cs_out->cs_ypixel = cfi.dwFontSize.Y;
	પાછા;
    }

    GetConsoleMode(g_hConIn, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_લીટી_આગત | ENABLE_ECHO_આગત
	    | ENABLE_PROCESSED_આગત);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_આગત;

    જો (SetConsoleMode(g_hConIn, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	SetConsoleMode(g_hConIn, cmodeold);
    }
}
# endif

#endif

    સ્થિર BOOL
vim_create_process(
    અક્ષર		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    અક્ષર		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = ખાલી;

    wcmd = enc_to_utf16((char_u *)cmd, ખાલી);
    જો (wcmd == ખાલી)
	પાછા FALSE;
    જો (cwd != ખાલી)
    {
	wcwd = enc_to_utf16((char_u *)cwd, ખાલી);
	જો (wcwd == ખાલી)
	    જાઓ theend;
    }

    ret = CreateProcessW(
	    ખાલી,		// Executable name
	    wcmd,		// Command to execute
	    ખાલી,		// Process security attributes
	    ખાલી,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    પાછા ret;
}


    સ્થિર HINSTANCE
vim_shell_execute(
    અક્ષર *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_to_utf16((char_u *)cmd, ખાલી);
    જો (wcmd == ખાલી)
	પાછા (HINSTANCE) 0;

    ret = ShellExecuteW(ખાલી, ખાલી, wcmd, ખાલી, ખાલી, n_show_cmd);
    vim_free(wcmd);
    પાછા ret;
}


#જો defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    સ્થિર પૂર્ણાંક
mch_system_classic(અક્ષર *cmd, પૂર્ણાંક options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = કદ(si);
    si.lpReserved = ખાલી;
    si.lpDesktop = ખાલી;
    si.lpTitle = ખાલી;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    જો (options & SHELL_DOOUT)
	si.wShowWindow = SW_SHOWMINNOACTIVE;
    નહીંતર
	si.wShowWindow = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = ખાલી;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_MODE |	CREATE_NEW_CONSOLE,
	    &si, &pi, ખાલી, ખાલી);

    // Wait for the command to terminate before continuing
    {
	પૂર્ણાંક	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	માટે (;;)
	{
	    સંદેશ_2	સંદેશ;

	    જો (PeekMessageW(&સંદેશ, (HWND)ખાલી, 0, 0, PM_REMOVE))
	    {
		TranslateMessage(&સંદેશ);
		DispatchMessageW(&સંદેશ);
		delay = 1;
		ચાલુ;
	    }
	    જો (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		તોડો;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    જો (delay < 50)
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

    પાછા ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    સ્થિર unsigned પૂર્ણાંક __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_hChildStd_IN_Wr = param;
    linenr_T	    lnum = curbuf->b_op_start.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    પૂર્ણાંક		    written = 0;

    માટે (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	જો (l == 0)
	    len = 0;
	નહીંતર જો (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_hChildStd_IN_Wr, "", 1, &len, ખાલી);
	}
	નહીંતર
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_hChildStd_IN_Wr, (અક્ષર *)lp + written,
		      s == ખાલી ? l : (DWORD)(s - (lp + written)),
		      &len, ખાલી);
	}
	જો (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    જો (lnum != curbuf->b_op_end.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_લીટી_ગણતરી
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_hChildStd_IN_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, ખાલી);
	    }

	    ++lnum;
	    જો (lnum > curbuf->b_op_end.lnum)
		તોડો;

	    lp = ml_get(lnum);
	    written = 0;
	}
	નહીંતર જો (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_hChildStd_IN_Wr);
    પાછા 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    સ્થિર શૂન્ય
dump_pipe(પૂર્ણાંક	    options,
	  HANDLE    g_hChildStd_OUT_Rd,
	  garray_T  *ga,
	  char_u    buffer[],
	  DWORD	    *buffer_off)
{
    DWORD	availableBytes = 0;
    DWORD	i;
    પૂર્ણાંક		ret;
    DWORD	len;
    DWORD	toRead;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_hChildStd_OUT_Rd, // pipe to query
			ખાલી,		    // optional buffer
			0,		    // buffer size
			ખાલી,		    // number of read bytes
			&availableBytes,    // available bytes total
			ખાલી);		    // byteLeft

    // We got real data in the pipe, read it
    જ્યારે (ret != 0 && availableBytes > 0)
    {
	toRead = (DWORD)(BUFLEN - *buffer_off);
	toRead = availableBytes < toRead ? availableBytes : toRead;
	ReadFile(g_hChildStd_OUT_Rd, buffer + *buffer_off, toRead , &len, ખાલી);

	// If we haven't read anything, there is a problem
	જો (len == 0)
	    તોડો;

	availableBytes -= len;

	જો (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    માટે (i = 0; i < len; ++i)
	    {
		જો (buffer[i] == NL)
		    append_ga_લીટી(ga);
		નહીંતર જો (buffer[i] == NUL)
		    ga_append(ga, NL);
		નહીંતર
		    ga_append(ga, buffer[i]);
	    }
	}
	નહીંતર જો (has_mbyte)
	{
	    પૂર્ણાંક		l;
	    પૂર્ણાંક		c;
	    char_u	*p;

	    len += *buffer_off;
	    buffer[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    માટે (p = buffer; p < buffer + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		જો (l == 0)
		    l = 1;  // NUL byte?
		નહીંતર જો (MB_BYTE2LEN(*p) != l)
		    તોડો;
	    }
	    જો (p == buffer)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		જો (len >= 12)
		    ++p;
		નહીંતર
		{
		    *buffer_off = len;
		    પાછા;
		}
	    }
	    c = *p;
	    *p = NUL;
	    સંદેશ_puts((અક્ષર *)buffer);
	    જો (p < buffer + len)
	    {
		*p = c;
		*buffer_off = (DWORD)((buffer + len) - p);
		mch_memmove(buffer, p, *buffer_off);
		પાછા;
	    }
	    *buffer_off = 0;
	}
	નહીંતર
	{
	    buffer[len] = NUL;
	    સંદેશ_puts((અક્ષર *)buffer);
	}

	windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col);
	cursor_on();
	out_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    સ્થિર પૂર્ણાંક
mch_system_piped(અક્ષર *cmd, પૂર્ણાંક options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_hChildStd_IN_Rd = ખાલી;
    HANDLE g_hChildStd_IN_Wr = ખાલી;
    HANDLE g_hChildStd_OUT_Rd = ખાલી;
    HANDLE g_hChildStd_OUT_Wr = ખાલી;

    char_u	buffer[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    પૂર્ણાંક		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    પૂર્ણાંક		noread_cnt = 0;
    garray_T	ga;
    પૂર્ણાંક		delay = 1;
    DWORD	buffer_off = 0;	// valid bytes in buffer[]
    અક્ષર	*p = ખાલી;

    SECURITY_ATTRIBUTES saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = કદ(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ખાલી;

    જો ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)
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
	સંદેશ_puts(_("\nCannot create pipes\n"));
    }

    si.cb = કદ(si);
    si.lpReserved = ખાલી;
    si.lpDesktop = ખાલી;
    si.lpTitle = ખાલી;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_hChildStd_OUT_Wr;
    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.h_Std_આગત = g_hChildStd_IN_Rd;
    si.wShowWindow = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = ખાલી;

    જો (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    જો (cmd != ખાલી)
    {
	p = (અક્ષર *)vim_strsave((char_u *)cmd);
	જો (p != ખાલી)
	    unescape_shellxquote((char_u *)p, p_sxe);
	નહીંતર
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_MODE,
						&si, &pi, ખાલી, ખાલી);

    જો (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(g_hChildStd_OUT_Wr);

    જો (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(ખાલી,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_hChildStd_IN_Wr,  // parameter
			0,		 // creation flag, start immediately
			ખાલી);		    // we don't care about thread id
	CloseHandle(thread);
	g_hChildStd_IN_Wr = ખાલી;
    }

    // Keep updating the window while waiting for the shell to finish.
    માટે (;;)
    {
	સંદેશ_2	સંદેશ;

	જો (PeekMessageW(&સંદેશ, (HWND)ખાલી, 0, 0, PM_REMOVE))
	{
	    TranslateMessage(&સંદેશ);
	    DispatchMessageW(&સંદેશ);
	}

	// write pipe information in the window
	જો ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.in_use
# endif
	    )
	{
	    len = 0;
	    જો (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		જો (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		જો (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    જો (len == 1 && cmd != ખાલી)
		    {
			જો (ta_buf[ta_len] == Ctrl_C)
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
		    જો ((len == 1 || len == 4 ) && cmd != ખાલી)
		    {
			જો (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_hChildStd_IN_Wr);
			    g_hChildStd_IN_Wr = ખાલી;
			    len = 0;
			}
		    }

		    len = term_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    માટે (i = ta_len; i < ta_len + len; ++i)
		    {
			જો (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    સંદેશ_putchar(ta_buf[i]);
			નહીંતર જો (has_mbyte)
			{
			    પૂર્ણાંક l = (*mb_ptr2len)(ta_buf + i);

			    સંદેશ_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			નહીંતર
			    સંદેશ_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col);
		    out_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    જો (options & SHELL_WRITE)
			ta_len = 0;
		    નહીંતર જો (g_hChildStd_IN_Wr != ખાલી)
		    {
			WriteFile(g_hChildStd_IN_Wr, (અક્ષર*)ta_buf,
				    1, &len, ખાલી);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			જો (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	જો (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	જો (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);
	    તોડો;
	}

	++noread_cnt;
	dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	જો (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_hChildStd_OUT_Rd);
    જો (g_hChildStd_IN_Wr != ખાલી)
	CloseHandle(g_hChildStd_IN_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    GetExitCodeProcess(pi.hProcess, &ret);

    જો (options & SHELL_READ)
    {
	જો (ga.ga_len > 0)
	{
	    append_ga_લીટી(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	નહીંતર
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    પાછા ret;
}

    સ્થિર પૂર્ણાંક
mch_system_g(અક્ષર *cmd, પૂર્ણાંક options)
{
    // if we can pipe and the shelltemp option is off
    જો (!p_stmp)
	પાછા mch_system_piped(cmd, options);
    નહીંતર
	પાછા mch_system_classic(cmd, options);
}
#endif

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    સ્થિર પૂર્ણાંક
mch_system_c(અક્ષર *cmd, પૂર્ણાંક options UNUSED)
{
    પૂર્ણાંક		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    જો (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	જો (buf == ખાલી)
	    પાછા -1;
	vim_snprintf((અક્ષર *)buf, len, "(%s)", cmd);
	wcmd = enc_to_utf16(buf, ખાલી);
	મુક્ત(buf);
    }
    નહીંતર
	wcmd = enc_to_utf16((char_u *)cmd, ખાલી);

    જો (wcmd == ખાલી)
	પાછા -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    પાછા ret;
}

#endif

    સ્થિર પૂર્ણાંક
mch_system(અક્ષર *cmd, પૂર્ણાંક options)
{
#ifdef VIMDLL
    જો (gui.in_use || gui.starting)
	પાછા mch_system_g(cmd, options);
    નહીંતર
	પાછા mch_system_c(cmd, options);
#અથવાજો defined(FEAT_GUI_MSWIN)
    પાછા mch_system_g(cmd, options);
#નહીંતર
    પાછા mch_system_c(cmd, options);
#endif
}

#જો defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    સ્થિર પૂર્ણાંક
mch_call_shell_terminal(
    char_u	*cmd,
    પૂર્ણાંક		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = ખાલી;
    typval_T	argvar[2];
    long_u	cmdlen;
    પૂર્ણાંક		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    જો (cmd == ખાલી)
	cmdlen = STRLEN(p_sh) + 1;
    નહીંતર
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    જો (newcmd == ખાલી)
	પાછા 255;
    જો (cmd == ખાલી)
    {
	STRCPY(newcmd, p_sh);
	ch_log(ખાલી, "starting terminal to run a shell");
    }
    નહીંતર
    {
	vim_snprintf((અક્ષર *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(ખાલી, "starting terminal for system command '%s'", cmd);
    }

    init_job_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = term_start(argvar, ખાલી, &opt, TERM_START_SYSTEM);
    જો (buf == ખાલી)
    {
	vim_free(newcmd);
	પાછા 255;
    }

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    જો (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	જ્યારે (term_use_loop())
	{
	    જો (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		જો (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    નહીંતર
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(ખાલી, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    પાછા retval;
}
#endif
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    સ્થિર શૂન્ય
restore_env_var(char_u *name, char_u *old_કિંમત, પૂર્ણાંક must_free)
{
    જો (old_કિંમત != ખાલી)
    {
	vim_setenv(name, old_કિંમત);
	જો (must_free)
	    vim_free(old_કિંમત);
	પાછા;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    પૂર્ણાંક
mch_call_shell(
    char_u  *cmd,
    પૂર્ણાંક	    options)	// SHELL_*, see vim.h
{
    પૂર્ણાંક		x = 0;
    પૂર્ણાંક		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    પૂર્ણાંક		must_free;
    char_u	*oldval;

#ifdef FEAT_EVAL
    ch_log(ખાલી, "executing shell command: %s", cmd);
#endif
    // Change the title to reflect that we are in a subshell.
    જો (GetConsoleTitleW(szShellTitle, ARRAY_LENGTH(szShellTitle) - 4) > 0)
    {
	જો (cmd == ખાલી)
	    wcscat(szShellTitle, L" :sh");
	નહીંતર
	{
	    WCHAR *wn = enc_to_utf16((char_u *)cmd, ખાલી);

	    જો (wn != ખાલી)
	    {
		wcscat(szShellTitle, L" - !");
		જો ((wcslen(szShellTitle) + wcslen(wn) <
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
    જો (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#endif
#જો defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    જો (
# ifdef VIMDLL
	    gui.in_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != ખાલી
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	જો (cmdbase != ખાલી)
	    // Skip a leading quote and (.
	    જ્યારે (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	જો (cmdbase == ખાલી || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    પાછા x;
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

    જો (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    જો (cmd == ખાલી)
    {
	x = mch_system((અક્ષર *)p_sh, options);
    }
    નહીંતર
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = ખાલી;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	જો (*cmdbase == '"' )
	    ++cmdbase;
	જો (*cmdbase == '(')
	    ++cmdbase;

	જો ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    ZeroMemory(&si, કદ(si));
	    si.cb = કદ(si);
	    si.lpReserved = ખાલી;
	    si.lpDesktop = ખાલી;
	    si.lpTitle = ખાલી;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = ખાલી;

	    cmdbase = skipwhite(cmdbase + 5);
	    જો ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    નહીંતર જો ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_WINDOW;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_આગત = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    ખાલી,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_NORMAL,		// File att.
		    ખાલી);				// Temp file
		si.hStdOutput = si.h_Std_આગત;
		si.hStdError = si.h_Std_આગત;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    જો (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		જો (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		જો (p > cmdbase && p[-1] == ')'
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
	    જો (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		જો (cmd_shell == ખાલી || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		જો (subcmd != ખાલી)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    જો (newcmd != ખાલી)
			vim_snprintf((અક્ષર *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    નહીંતર
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    જો (vim_create_process((અક્ષર *)newcmd, FALSE, flags,
			&si, &pi, ખાલી, ખાલી))
		x = 0;
	    નહીંતર જો (vim_shell_execute((અક્ષર *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    નહીંતર
	    {
		x = -1;
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
		જો (gui.in_use)
# endif
		    emsg(_(e_command_not_found));
#endif
	    }

	    જો (newcmd != cmdbase)
		vim_free(newcmd);

	    જો (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_આગત != ખાલી)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_આગત);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	નહીંતર
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
	    જો (newcmd != ખાલી)
	    {
#જો defined(FEAT_GUI_MSWIN)
		જો (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    અક્ષર *સંદેશ = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    અક્ષર *title = _("Vim Warning");
		    WCHAR *wmsg = enc_to_utf16((char_u *)સંદેશ, ખાલી);
		    WCHAR *wtitle = enc_to_utf16((char_u *)title, ખાલી);

		    જો (wmsg != ખાલી && wtitle != ખાલી)
			MessageBoxW(ખાલી, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		જો (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((અક્ષર *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_path.string,
			    (સંદેશ_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		નહીંતર જો (
# ifdef VIMDLL
			(gui.in_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((અક્ષર *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		નહીંતર
#endif
		    vim_snprintf((અક્ષર *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((અક્ષર *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    જો (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    જો (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#જો defined(FEAT_GUI_MSWIN)
	    && ((gui.in_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#endif
	    )
    {
	smsg(_("shell returned %d"), x);
	સંદેશ_putchar('\n');
    }
    resettitle();

    mch_signal(SIGINT, SIG_DFL);
    mch_signal(SIGBREAK, SIG_DFL);
    mch_signal(SIGILL, SIG_DFL);
    mch_signal(SIGFPE, SIG_DFL);
    mch_signal(SIGSEGV, SIG_DFL);
    mch_signal(SIGTERM, SIG_DFL);
    mch_signal(SIGABRT, SIG_DFL);

    પાછા x;
}

#જો defined(FEAT_JOB_CHANNEL)
    સ્થિર HANDLE
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

    wn = enc_to_utf16(fname, ખાલી);
    જો (wn == ખાલી)
	પાછા INVALID_HANDLE_કિંમત;

    h = CreateFileW(wn, dwDesiredAccess, dwShareMode,
	    lpSecurityAttributes, dwCreationDisposition,
	    dwFlagsAndAttributes, ખાલી);
    vim_free(wn);
    પાછા h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    શૂન્ય
win32_build_env(dict_T *env, garray_T *gap, પૂર્ણાંક is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != ખાલી ? env->dv_hashtab.ht_used : 0;
    LPVOID	base = GetEnvironmentStringsW();

    // for last \0
    જો (ga_grow(gap, 1) == FAIL)
	પાછા;

    જો (env != ખાલી)
    {
	FOR_ALL_HASHTAB_વસ્તુઓ(&env->dv_hashtab, hi, todo)
	{
	    જો (!HASHITEM_EMPTY(hi))
	    {
		typval_T *item = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_to_utf16((char_u *)hi->hi_ચાવી, ખાલી);
		WCHAR   *wval = enc_to_utf16(tv_get_string(item), ખાલી);
		--todo;
		જો (wkey != ખાલી && wval != ખાલી)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    જો (ga_grow(gap, (પૂર્ણાંક)(lkey + lval + 2)) == FAIL)
			ચાલુ;
		    માટે (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'=';
		    માટે (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    જો (base)
    {
	WCHAR	*p = (WCHAR*) base;

	// for last \0
	જો (ga_grow(gap, 1) == FAIL)
	    પાછા;

	જ્યારે (*p != 0 || *(p + 1) != 0)
	{
	    જો (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_data + gap->ga_len++) = *p;
	    p++;
	}
	FreeEnvironmentStringsW(base);
	*((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
    }

# જો defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
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

	જો (ga_grow(gap, (પૂર્ણાંક)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    માટે (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    માટે (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    જો (is_terminal)
	    {
		માટે (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_data + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		માટે (n = 0; n < version_len; n++)
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
    સ્થિર BOOL
create_pipe_pair(HANDLE handles[2])
{
    સ્થિર LONG		s;
    અક્ષર		name[64];
    SECURITY_ATTRIBUTES sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_SIZE, 0, 0, ખાલી);

    જો (handles[1] == INVALID_HANDLE_કિંમત)
	પાછા FALSE;

    sa.nLength = કદ(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = ખાલી;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    જો (handles[0] == INVALID_HANDLE_કિંમત)
    {
	CloseHandle(handles[1]);
	પાછા FALSE;
    }

    પાછા TRUE;
}

#endif // FEAT_JOB_CHANNEL

#જો defined(FEAT_EVAL)
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
    અક્ષર	**argv,
    char_u	*infile,
    પૂર્ણાંક		flags UNUSED,
    પૂર્ણાંક		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;
    HANDLE		hChildStdoutRd = INVALID_HANDLE_કિંમત;
    HANDLE		hChildStdoutWr = INVALID_HANDLE_કિંમત;
    HANDLE		hChildStdinRd = INVALID_HANDLE_કિંમત;
    garray_T		cmd_ga;
    garray_T		out_ga;
    char_u		*buffer = ખાલી;
    DWORD		exit_code = (DWORD)-1;
    પૂર્ણાંક			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    માટે (i = 0; argv[i] != ખાલી; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	પૂર્ણાંક	has_spaces = FALSE;
	પૂર્ણાંક	j;

	માટે (j = 0; s[j] != NUL; j++)
	    જો (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		તોડો;
	    }

	જો (i > 0)
	    ga_append(&cmd_ga, ' ');

	જો (has_spaces)
	{
	    પૂર્ણાંક	અંક_bs;

	    ga_append(&cmd_ga, '"');
	    માટે (j = 0; arg[j] != NUL; j++)
	    {
		અંક_bs = 0;
		જ્યારે (arg[j] == '\\')
		{
		    અંક_bs++;
		    j++;
		}

		જો (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    જ્યારે (અંક_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    તોડો;
		}
		નહીંતર જો (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    જ્યારે (અંક_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		નહીંતર
		{
		    જ્યારે (અંક_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	નહીંતર
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&out_ga, 1, 4096);

    saAttr.nLength = કદ(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ખાલી;

    // Create a pipe for the child's stdout.
    જો (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)
	    || !SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	જાઓ done;
    }

    // Set up stdin from infile if provided.
    જો (infile != ખાલી)
    {
	WCHAR *winfile = enc_to_utf16(infile, ખાલી);

	જો (winfile != ખાલી)
	{
	    hChildStdinRd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL, ખાલી);
	    vim_free(winfile);
	}
    }

    ZeroMemory(&pi, કદ(pi));
    ZeroMemory(&si, કદ(si));
    si.cb = કદ(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;
    si.h_Std_આગત = (hChildStdinRd != INVALID_HANDLE_કિંમત)
		    ? hChildStdinRd : INVALID_HANDLE_કિંમત;

    ch_log(ખાલી, "directly executing: %s", (અક્ષર *)cmd_ga.ga_data);

    // Create the child process directly, without going through the shell.
    જો (!vim_create_process((અક્ષર *)cmd_ga.ga_data, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
		&si, &pi, ખાલી, ખાલી))
    {
	semsg(_(e_invalid_argument_str), cmd_ga.ga_data);
	જાઓ done;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(hChildStdoutWr);
    hChildStdoutWr = INVALID_HANDLE_કિંમત;
    જો (hChildStdinRd != INVALID_HANDLE_કિંમત)
    {
	CloseHandle(hChildStdinRd);
	hChildStdinRd = INVALID_HANDLE_કિંમત;
    }

    // Read output from child process.
    માટે (;;)
    {
	અક્ષર	buf[4096];
	DWORD	n;

	જો (!ReadFile(hChildStdoutRd, buf, કદ(buf), &n, ખાલી) || n == 0)
	    તોડો;
	જો (ga_grow(&out_ga, (પૂર્ણાંક)n) == OK)
	{
	    mch_memmove((અક્ષર *)out_ga.ga_data + out_ga.ga_len, buf, n);
	    out_ga.ga_len += (પૂર્ણાંક)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    set_vim_var_nr(VV_SHELL_ERROR, (long)exit_code);

    જો (out_ga.ga_len > 0)
    {
	buffer = alloc(out_ga.ga_len + 1);
	જો (buffer != ખાલી)
	{
	    mch_memmove(buffer, out_ga.ga_data, out_ga.ga_len);
	    જો (ret_len == ખાલી)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		માટે (i = 0; i < out_ga.ga_len; ++i)
		    જો (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[out_ga.ga_len] = NUL;
	    }
	    નહીંતર
		*ret_len = out_ga.ga_len;
	}
    }
    નહીંતર જો (ret_len != ખાલી)
	*ret_len = 0;

done:
    ga_clear(&cmd_ga);
    ga_clear(&out_ga);
    જો (hChildStdoutRd != INVALID_HANDLE_કિંમત)
	CloseHandle(hChildStdoutRd);
    જો (hChildStdoutWr != INVALID_HANDLE_કિંમત)
	CloseHandle(hChildStdoutWr);
    જો (hChildStdinRd != INVALID_HANDLE_કિંમત)
	CloseHandle(hChildStdinRd);
    પાછા buffer;
}
#endif // FEAT_EVAL

#જો defined(FEAT_JOB_CHANNEL)
    શૂન્ય
mch_job_start(અક્ષર *cmd, job_T *job, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_ATTRIBUTES saAttr;
    channel_T		*channel = ખાલી;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    પૂર્ણાંક		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    પૂર્ણાંક		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    પૂર્ણાંક		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    પૂર્ણાંક		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    પૂર્ણાંક		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    પૂર્ણાંક		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    પૂર્ણાંક		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;

    જો (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_કિંમત;
    ifd[1] = INVALID_HANDLE_કિંમત;
    ofd[0] = INVALID_HANDLE_કિંમત;
    ofd[1] = INVALID_HANDLE_કિંમત;
    efd[0] = INVALID_HANDLE_કિંમત;
    efd[1] = INVALID_HANDLE_કિંમત;
    ga_init2(&ga, કદ(wchar_t), 500);

    jo = CreateJobObject(ખાલી, ખાલી);
    જો (jo == ખાલી)
    {
	job->jv_status = JOB_FAILED;
	જાઓ failed;
    }

    જો (options->jo_env != ખાલી)
	win32_build_env(options->jo_env, &ga, FALSE);

    ZeroMemory(&pi, કદ(pi));
    ZeroMemory(&si, કદ(si));
    si.cb = કદ(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    saAttr.nLength = કદ(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ખાલી;

    જો (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	ifd[0] = job_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
	જો (ifd[0] == INVALID_HANDLE_કિંમત)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    જાઓ failed;
	}
    }
    નહીંતર જો (!use_null_for_in
	    && (!create_pipe_pair(ifd)
		|| !SetHandleInformation(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	જાઓ failed;

    જો (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	ofd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	જો (ofd[1] == INVALID_HANDLE_કિંમત)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    જાઓ failed;
	}
    }
    નહીંતર જો (!use_null_for_out &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !SetHandleInformation(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	જાઓ failed;

    જો (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	જો (efd[1] == INVALID_HANDLE_કિંમત)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    જાઓ failed;
	}
    }
    નહીંતર જો (!use_out_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !SetHandleInformation(efd[0], HANDLE_FLAG_INHERIT, 0)))
	જાઓ failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_આગત = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_out_for_err ? ofd[1] : efd[1];

    જો (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	જો (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    જો (channel != ખાલી)
		++channel->ch_refcount;
	}
	નહીંતર
	    channel = add_channel();
	જો (channel == ખાલી)
	    જાઓ failed;
    }

    જો (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_MODE |
	    CREATE_NEW_PROCESS_GROUP |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_data,
	    (અક્ષર *)options->jo_cwd))
    {
	CloseHandle(jo);
	job->jv_status = JOB_FAILED;
	જાઓ failed;
    }

    ga_clear(&ga);

    જો (!AssignProcessToJobObject(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = ખાલી;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    job->jv_proc_info = pi;
    job->jv_job_object = jo;
    job->jv_status = JOB_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    જો (!use_out_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    job->jv_channel = channel;
    જો (channel != ખાલી)
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
    પાછા;

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

    અક્ષર *
mch_job_status(job_T *job)
{
    DWORD dwExitCode = 0;

    જો (!GetExitCodeProcess(job->jv_proc_info.hProcess, &dwExitCode)
	    || dwExitCode != STILL_ACTIVE)
    {
	job->jv_exitval = (પૂર્ણાંક)dwExitCode;
	જો (job->jv_status < JOB_ENDED)
	{
	    ch_log(job->jv_channel, "Job ended");
	    job->jv_status = JOB_ENDED;
	}
	પાછા "dead";
    }
    પાછા "run";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    HANDLE jobHandles[MAXIMUM_WAIT_OBJECTS];
    job_T *jobArray[MAXIMUM_WAIT_OBJECTS];
    job_T *job = job_list;

    જ્યારે (job != ખાલી)
    {
	DWORD n;
	DWORD result;

	માટે (n = 0; n < MAXIMUM_WAIT_OBJECTS
				       && job != ખાલી; job = job->jv_next)
	{
	    જો (job->jv_status == JOB_STARTED)
	    {
		jobHandles[n] = job->jv_proc_info.hProcess;
		jobArray[n] = job;
		++n;
	    }
	}
	જો (n == 0)
	    ચાલુ;
	result = WaitForMultipleObjects(n, jobHandles, FALSE, 0);
	જો (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + n)
	{
	    job_T *wait_job = jobArray[result - WAIT_OBJECT_0];

	    જો (STRCMP(mch_job_status(wait_job), "dead") == 0)
		પાછા wait_job;
	}
    }
    પાછા ખાલી;
}

    સ્થિર BOOL
terminate_all(HANDLE process, પૂર્ણાંક code)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_કિંમત;
    DWORD	    pid = GetProcessId(process);

    જો (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	જો (h != INVALID_HANDLE_કિંમત)
	{
	    pe.dwSize = કદ(PROCESSENTRY32);
	    જો (!Process32First(h, &pe))
		જાઓ theend;

	    કરો
	    {
		જો (pe.th32ParentProcessID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    જો (ph != ખાલી)
		    {
			terminate_all(ph, code);
			CloseHandle(ph);
		    }
		}
	    } જ્યારે (Process32Next(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    પાછા TerminateProcess(process, code);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    પૂર્ણાંક
mch_signal_job(job_T *job, char_u *how)
{
    પૂર્ણાંક ret;

    જો (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	જો (job->jv_job_object != ખાલી)
	{
	    જો (job->jv_channel != ખાલી && job->jv_channel->ch_anonymous_pipe)
		job->jv_channel->ch_killing = TRUE;
	    પાછા TerminateJobObject(job->jv_job_object, (UINT)-1) ? OK : FAIL;
	}
	પાછા terminate_all(job->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    જો (!AttachConsole(job->jv_proc_info.dwProcessId))
	પાછા FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		job->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    પાછા ret;
}

/*
 * Clear the data related to "job".
 */
    શૂન્ય
mch_clear_job(job_T *job)
{
    જો (job->jv_status == JOB_FAILED)
	પાછા;

    જો (job->jv_job_object != ખાલી)
	CloseHandle(job->jv_job_object);
    CloseHandle(job->jv_proc_info.hProcess);
}
#endif


#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    સ્થિર શૂન્ય
termcap_mode_start(શૂન્ય)
{
    DWORD cmodein;

    જો (g_fTermcapMode)
	પાછા;

    SaveConsoleBuffer(&g_cbNonTermcap);

    જો (g_cbTermcap.IsValid)
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
    નહીંતર
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
    જો (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_આગત;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    નહીંતર
    {
	cmodein &= ~ENABLE_MOUSE_આગત;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }
    cmodein |= ENABLE_WINDOW_આગત;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_fTermcapMode = TRUE;
}


/*
 * End termcap mode
 */
    સ્થિર શૂન્ય
termcap_mode_end(શૂન્ય)
{
    DWORD cmodein;
    ConsoleBuffer *cb;
    COORD coord;
    DWORD dwDummy;

    જો (!g_fTermcapMode)
	પાછા;

    SaveConsoleBuffer(&g_cbTermcap);

    GetConsoleMode(g_hConIn, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_આગત | ENABLE_WINDOW_આગત);
    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# નહીંતર
    cb = &g_cbNonTermcap;
# endif
    RestoreConsoleBuffer(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    જો (exiting && use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    જો (!USE_WT && (p_rs || exiting))
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
	જો (exiting && !p_rs)
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


#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    સ્થિર શૂન્ય
clear_chars(
    COORD coord,
    DWORD n)
{
    જો (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_hConOut, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    નહીંતર
    {
	set_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    સ્થિર શૂન્ય
clear_screen(શૂન્ય)
{
    g_coord.X = g_coord.Y = 0;

    જો (!vtp_working)
	clear_chars(g_coord, Rows * Columns);
    નહીંતર
    {
	set_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    સ્થિર શૂન્ય
clear_to_end_of_display(શૂન્ય)
{
    COORD save = g_coord;

    જો (!vtp_working)
	clear_chars(g_coord, (Rows - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    નહીંતર
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
    સ્થિર શૂન્ય
clear_to_end_of_લીટી(શૂન્ય)
{
    COORD save = g_coord;

    જો (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    નહીંતર
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
    સ્થિર શૂન્ય
scroll(unsigned c_લીટીઓ)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.ડાબું_3 + 1, g_srScrollRegion.ટોચ_2 + 1);
    delete_લીટીઓ(c_લીટીઓ);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    સ્થિર શૂન્ય
set_scroll_region(
    unsigned ડાબું,
    unsigned ટોચ,
    unsigned જમણું,
    unsigned bottom)
{
    જો (ડાબું >= જમણું
	    || ટોચ >= bottom
	    || જમણું > (unsigned) Columns - 1
	    || bottom > (unsigned) Rows - 1)
	પાછા;

    g_srScrollRegion.ડાબું_3 =   ડાબું;
    g_srScrollRegion.ટોચ_2 =    ટોચ;
    g_srScrollRegion.જમણું_3 =  જમણું;
    g_srScrollRegion.Bottom = bottom;
}

    સ્થિર શૂન્ય
set_scroll_region_tb(
    unsigned ટોચ,
    unsigned bottom)
{
    જો (ટોચ >= bottom || bottom > (unsigned)Rows - 1)
	પાછા;

    g_srScrollRegion.ટોચ_2 = ટોચ;
    g_srScrollRegion.Bottom = bottom;
}

    સ્થિર શૂન્ય
set_scroll_region_lr(
    unsigned ડાબું,
    unsigned જમણું)
{
    જો (ડાબું >= જમણું || જમણું > (unsigned)Columns - 1)
	પાછા;

    g_srScrollRegion.ડાબું_3 = ડાબું;
    g_srScrollRegion.જમણું_3 = જમણું;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    સ્થિર શૂન્ય
ઉમેરો_લીટીઓ(unsigned c_લીટીઓ)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.ડાબું_3 + 1, g_srScrollRegion.ટોચ_2 + 1);

    dest.X = g_srScrollRegion.ડાબું_3;
    dest.Y = g_coord.Y + c_લીટીઓ;

    source.ડાબું_3   = g_srScrollRegion.ડાબું_3;
    source.ટોચ_2	  = g_coord.Y;
    source.જમણું_3  = g_srScrollRegion.જમણું_3;
    source.Bottom = g_srScrollRegion.Bottom - c_લીટીઓ;

    clip.ડાબું_3   = g_srScrollRegion.ડાબું_3;
    clip.ટોચ_2    = g_coord.Y;
    clip.જમણું_3  = g_srScrollRegion.જમણું_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    જો (!USE_VTP)
	fill.Attributes = g_attrCurrent;
    નહીંતર
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    જો (source.Bottom < dest.Y)
    {
	COORD coord;
	પૂર્ણાંક   i;

	coord.X = source.ડાબું_3;
	માટે (i = clip.ટોચ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.જમણું_3 - source.ડાબું_3 + 1);
	}
    }

    જો (vtp_working)
    {
	COORD coord;
	પૂર્ણાંક i;

	coord.X = source.ડાબું_3;
	માટે (i = source.ટોચ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.જમણું_3 - source.ડાબું_3 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    સ્થિર શૂન્ય
delete_લીટીઓ(unsigned c_લીટીઓ)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    પૂર્ણાંક		    nb;

    gotoxy(g_srScrollRegion.ડાબું_3 + 1, g_srScrollRegion.ટોચ_2 + 1);

    dest.X = g_srScrollRegion.ડાબું_3;
    dest.Y = g_coord.Y;

    source.ડાબું_3   = g_srScrollRegion.ડાબું_3;
    source.ટોચ_2	  = g_coord.Y + c_લીટીઓ;
    source.જમણું_3  = g_srScrollRegion.જમણું_3;
    source.Bottom = g_srScrollRegion.Bottom;

    clip.ડાબું_3   = g_srScrollRegion.ડાબું_3;
    clip.ટોચ_2    = g_coord.Y;
    clip.જમણું_3  = g_srScrollRegion.જમણું_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    જો (!vtp_working)
	fill.Attributes = g_attrCurrent;
    નહીંતર
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (source.Bottom - source.ટોચ_2) + 1;

    જો (nb < source.ટોચ_2)
    {
	COORD coord;
	પૂર્ણાંક   i;

	coord.X = source.ડાબું_3;
	માટે (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.જમણું_3 - source.ડાબું_3 + 1);
	}
    }

    જો (vtp_working)
    {
	COORD coord;
	પૂર્ણાંક i;

	coord.X = source.ડાબું_3;
	માટે (i = nb; i <= source.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.જમણું_3 - source.ડાબું_3 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    સ્થિર શૂન્ય
gotoxy(
    unsigned x,
    unsigned y)
{
    જો (x < 1 || x > (unsigned)Columns || y < 1 || y > (unsigned)Rows)
	પાછા;

    જો (!USE_VTP)
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
    નહીંતર
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
    સ્થિર શૂન્ય
textattr(WORD wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    SetConsoleTextAttribute(g_hConOut, wAttr);
}


    સ્થિર શૂન્ય
textcolor(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    જો (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    નહીંતર
	vtp_sgr_bulk(wAttr);
}


    સ્થિર શૂન્ય
textbackground(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    જો (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    નહીંતર
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    સ્થિર શૂન્ય
normvideo(શૂન્ય)
{
    જો (!vtp_working)
	textattr(g_attrDefault);
    નહીંતર
	vtp_sgr_bulk(0);
}


સ્થિર WORD g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    સ્થિર શૂન્ય
standout(શૂન્ય)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((WORD) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    સ્થિર શૂન્ય
standend(શૂન્ય)
{
    જો (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    શૂન્ય
mch_set_normal_colors(શૂન્ય)
{
    char_u	*p;
    પૂર્ણાંક		n;

    cterm_normal_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_normal_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    જો (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	જો (*p == 'm' && n > 0)
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
    સ્થિર શૂન્ય
visual_bell(શૂન્ય)
{
    COORD   coordOrigin = {0, 0};
    WORD    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = ખાલી;

# ifdef FEAT_TERMGUICOLORS
    જો (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(WORD, Rows * Columns);
	જો (oldattrs == ખાલી)
	    પાછા;
	ReadConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_hConOut, attrFlash, Rows * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    જો (oldattrs != ખાલી)
    {
	WriteConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    સ્થિર શૂન્ય
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    જો (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	પાછા;
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
    સ્થિર DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cbToWrite)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    સ્થિર DWORD    cells;
    સ્થિર WCHAR    *unicodebuf = ખાલી;
    સ્થિર પૂર્ણાંક	    unibuflen = 0;
    સ્થિર પૂર્ણાંક	    length;
    પૂર્ણાંક		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    સ્થિર WCHAR    *utf8spbuf = ખાલી;
    સ્થિર પૂર્ણાંક	    utf8splength;
    સ્થિર DWORD    utf8spcells;
    સ્થિર WCHAR    **utf8usingbuf = &unicodebuf;

    જો (cbToWrite != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	કરો
	{
	    length = MultiByteToWideChar(cp, 0, (LPCSTR)pchBuf, cbToWrite,
							unicodebuf, unibuflen);
	    જો (length && length <= unibuflen)
		તોડો;
	    vim_free(unicodebuf);
	    unicodebuf = length ? LALLOC_MULT(WCHAR, length) : ખાલી;
	    unibuflen = unibuflen ? 0 : length;
	} જ્યારે (TRUE);
	cells = mb_string2cells(pchBuf, cbToWrite);
    }
    નહીંતર // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	જો (utf8usingbuf != &utf8spbuf)
	{
	    જો (utf8spbuf == ખાલી)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		length = MultiByteToWideChar(CP_UTF8, 0, " ", 1, ખાલી, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, length);
		જો (utf8spbuf != ખાલી)
		{
		    MultiByteToWideChar(CP_UTF8, 0, " ", 1, utf8spbuf, length);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = length;
		    utf8spcells = cells;
		}
	    }
	    નહીંતર
	    {
		utf8usingbuf = &utf8spbuf;
		length = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    જો (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	જો (WriteConsoleOutputCharacterW(g_hConOut, *utf8usingbuf, length,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    નહીંતર
    {
	જો (WriteConsoleW(g_hConOut, *utf8usingbuf, length, &cchwritten,
		    ખાલી) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    જો (cchwritten == (DWORD)length)
    {
	written = cbToWrite;
	g_coord.X += (SHORT)cells;
    }
    નહીંતર
    {
	char_u *p = pchBuf;
	માટે (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    જ્યારે (g_coord.X > g_srScrollRegion.જમણું_3)
    {
	g_coord.X -= (SHORT) Columns;
	જો (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    જો (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    પાછા written;
}

    સ્થિર char_u *
get_seq(
    પૂર્ણાંક *args,
    પૂર્ણાંક *ગણતરી,
    char_u *head)
{
    પૂર્ણાંક argc;
    char_u *p;

    જો (head == ખાલી || *head != '\033')
	પાછા ખાલી;

    argc = 0;
    p = head;
    ++p;
    કરો
    {
	++p;
	args[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } જ્યારે (*p == ';');
    *ગણતરી = argc;

    પાછા p;
}

    સ્થિર char_u *
get_sgr(
    પૂર્ણાંક *args,
    પૂર્ણાંક *ગણતરી,
    char_u *head)
{
    char_u *p = get_seq(args, ગણતરી, head);

    પાછા (p && *p == 'm') ? ++p : ખાલી;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    સ્થિર char_u *
sgrn2(
    char_u *head,
    પૂર્ણાંક	   n)
{
    પૂર્ણાંક argc;
    પૂર્ણાંક args[16];
    char_u *p = get_sgr(args, &argc, head);

    પાછા p && argc == 5 && args[0] == n && args[1] == 2 ? p : ખાલી;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    સ્થિર char_u *
sgrnc(
    char_u *head,
    પૂર્ણાંક	   n)
{
    પૂર્ણાંક argc;
    પૂર્ણાંક args[16];
    char_u *p = get_sgr(args, &argc, head);

    પાછા p && argc == 1 && args[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : ખાલી;
}

    સ્થિર char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    જ્યારે (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    પાછા p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    સ્થિર char_u *
sgrn2c(
    char_u *head,
    પૂર્ણાંક	   n)
{
    char_u *p = sgrn2(head, n);

    પાછા p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : ખાલી;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    સ્થિર char_u *
sgrn2cn(
    char_u *head,
    પૂર્ણાંક n)
{
    char_u *p = sgrn2(head, n);

    પાછા p && p[0] == 0x0a && p[1] == '\033' ? ++p : ખાલી;
}

સ્થિર bool in_dcs = false;	// inside a DCS (sixel) sequence
સ્થિર bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    સ્થિર શૂન્ય
write_vt(char_u *s, પૂર્ણાંક len)
{
    પૂર્ણાંક		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ખાલી, 0);
    જો (wlen <= 0)
	પાછા;
    ws = ALLOC_MULT(WCHAR, wlen);
    જો (ws == ખાલી)
	પાછા;
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_hConOut, ws, wlen, &written, ખાલી);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    સ્થિર char_u *
skip_dcs(char_u *s, char_u *end)
{
    માટે (char_u *p = s; p < end; ++p)
    {
	જો (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    in_dcs = false;
	    પાછા p + 1;
	}
	dcs_esc = *p == ESC;
    }
    પાછા end;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    શૂન્ય
mch_write(
    char_u  *s,
    પૂર્ણાંક	    len)
{
    char_u  *end = s + len;

# ifdef VIMDLL
    જો (gui.in_use)
	પાછા;
# endif

    જો (!term_console)
    {
	લિખ(1, s, (unsigned)len);
	પાછા;
    }

    // translate ESC | sequences into faked bios calls
    જ્યારે (len--)
    {
	પૂર્ણાંક prefix = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	જો (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    પાછા;
	}

	જો (in_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    પૂર્ણાંક	l = (પૂર્ણાંક)(skip_dcs(s, end) - s);

	    જો (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    ચાલુ;
	}

	જ્યારે (s + ++prefix < end)
	{
	    ch = s[prefix];
	    જો (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		તોડો;
	}

	જો (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    જો (prefix != 0)
		prefix = 1;
	}

	જો (prefix != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, prefix);
# ifdef MCH_WRITE_DUMP
	    જો (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, કદ(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	નહીંતર જો (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    જો (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.ડાબું_3 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    નહીંતર
	    {
		gotoxy(g_srScrollRegion.ડાબું_3 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    જો (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	નહીંતર જો (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.ડાબું_3+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    જો (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	નહીંતર જો (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    જો (g_coord.X > g_srScrollRegion.ડાબું_3)
		g_coord.X--;
	    નહીંતર જો (g_coord.Y > g_srScrollRegion.ટોચ_2)
	    {
		g_coord.X = g_srScrollRegion.જમણું_3;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    જો (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	નહીંતર જો (s[0] == '\a')
	{
	    // \a, bell
	    MessageBeep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    જો (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	નહીંતર જો (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    પૂર્ણાંક	    arg1 = 0, arg2 = 0, argc = 0, args[16];
	    char_u  *sp;

	    પસંદગી (s[2])
	    {
	    સ્થિતિ '0': સ્થિતિ '1': સ્થિતિ '2': સ્થિતિ '3': સ્થિતિ '4':
	    સ્થિતિ '5': સ્થિતિ '6': સ્થિતિ '7': સ્થિતિ '8': સ્થિતિ '9':
		જો (*(p = get_seq(args, &argc, s)) != 'm')
		    જાઓ notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		જો (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (પૂર્ણાંક)(p - s);
		    s = p;
		    તોડો;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		જો (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		જો (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		જો (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		જો (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(args, &argc, p);

notsgr:
		arg1 = args[0];
		arg2 = args[1];
		જો (*p == 'm')
		{
		    જો (argc == 1 && args[0] == 0)
			normvideo();
		    નહીંતર જો (argc == 1)
		    {
			જો (USE_VTP)
			    textcolor((WORD)arg1);
			નહીંતર
			    textattr((WORD)arg1);
		    }
		    નહીંતર જો (vtp_working)
			vtp_sgr_bulks(argc, args);
		}
		નહીંતર જો (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		નહીંતર જો (argc == 2 && *p == 'r')
		{
		    set_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		નહીંતર જો (argc == 2 && *p == 'R')
		{
		    set_scroll_region_tb(arg1, arg2);
		}
		નહીંતર જો (argc == 2 && *p == 'V')
		{
		    set_scroll_region_lr(arg1, arg2);
		}
		નહીંતર જો (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.ટોચ_2, g_coord.Y - arg1) + 1);
		}
		નહીંતર જો (argc == 1 && *p == 'b')
		{
		    textbackground((WORD) arg1);
		}
		નહીંતર જો (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.જમણું_3, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		નહીંતર જો (argc == 1 && *p == 'f')
		{
		    textcolor((WORD) arg1);
		}
		નહીંતર જો (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		નહીંતર જો (argc == 1 && *p == 'L')
		{
		    ઉમેરો_લીટીઓ(arg1);
		}
		નહીંતર જો (argc == 1 && *p == 'M')
		{
		    delete_લીટીઓ(arg1);
		}

		len -= (પૂર્ણાંક)(p - s);
		s = p + 1;
		તોડો;

	    સ્થિતિ 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.ટોચ_2, g_coord.Y - 1) + 1);
		જાઓ got3;

	    સ્થિતિ 'B':
		visual_bell();
		જાઓ got3;

	    સ્થિતિ 'C':
		gotoxy(min(g_srScrollRegion.જમણું_3, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		જાઓ got3;

	    સ્થિતિ 'E':
		termcap_mode_end();
		જાઓ got3;

	    સ્થિતિ 'F':
		standout();
		જાઓ got3;

	    સ્થિતિ 'f':
		standend();
		જાઓ got3;

	    સ્થિતિ 'H':
		gotoxy(1, 1);
		જાઓ got3;

	    સ્થિતિ 'j':
		clear_to_end_of_display();
		જાઓ got3;

	    સ્થિતિ 'J':
		clear_screen();
		જાઓ got3;

	    સ્થિતિ 'K':
		clear_to_end_of_લીટી();
		જાઓ got3;

	    સ્થિતિ 'L':
		ઉમેરો_લીટીઓ(1);
		જાઓ got3;

	    સ્થિતિ 'M':
		delete_લીટીઓ(1);
		જાઓ got3;

	    સ્થિતિ 'S':
		termcap_mode_start();
		જાઓ got3;

	    સ્થિતિ 'V':
		cursor_visible(TRUE);
		જાઓ got3;

	    સ્થિતિ 'v':
		cursor_visible(FALSE);
		જાઓ got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    જો (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, કદ(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	નહીંતર જો (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    પૂર્ણાંક l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    જ્યારે (s + l < end && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    જો (s + l < end && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		જો (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	નહીંતર જો (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    પૂર્ણાંક	l;

	    in_dcs = true;
	    dcs_esc = false;
	    l = (પૂર્ણાંક)(skip_dcs(s + 2, end) - s);
	    જો (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	નહીંતર
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    જો (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, કદ(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    જો (fdDump)
	fflush(fdDump);
# endif
}

#નહીંતર // FEAT_GUI_MSWIN && !VIMDLL
    શૂન્ય
mch_write(
    char_u  *s UNUSED,
    પૂર્ણાંક	    len UNUSED)
{
    // never used
}

#endif


/*
 * Delay for "msec" milliseconds.
 */
    શૂન્ય
mch_delay(
    long    msec,
    પૂર્ણાંક	    flags UNUSED)
{
#જો defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((પૂર્ણાંક)msec);	    // never wait for input
#નહીંતર // Console
# ifdef VIMDLL
    જો (gui.in_use)
    {
	Sleep((પૂર્ણાંક)msec);	    // never wait for input
	પાછા;
    }
# endif
    જો (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	જો (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    પૂર્ણાંક towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    જ્યારે (msec > 0)
	    {
		mzvim_check_threads();
		જો (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	નહીંતર
# endif
	    Sleep((પૂર્ણાંક)msec);
    નહીંતર
	WaitForChar(msec, FALSE);
#endif
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    પૂર્ણાંક
mch_remove(char_u *name)
{
    WCHAR	*wn;
    પૂર્ણાંક		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    જો (mch_isdir(name) && mch_is_symbolic_link(name))
	પાછા mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_NORMAL);

    wn = enc_to_utf16(name, ખાલી);
    જો (wn == ખાલી)
	પાછા -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    પાછા n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    શૂન્ય
mch_breakcheck(પૂર્ણાંક force UNUSED)
{
#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    જો (!gui.in_use)
# endif
	જો (g_fCtrlCPressed || g_fCBrkPressed)
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
mch_total_mem(પૂર્ણાંક special UNUSED)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dwLength = કદ(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&ms);
    જો (ms.ullAvailVirtual < ms.ullTotalPhys)
    {
	// Process address space fits in physical RAM, use all of it.
	પાછા (long_u)(ms.ullAvailVirtual / 1024);
    }
    જો (ms.ullTotalPhys <= WINNT_RESERVE_BYTES)
    {
	// Catch old NT box or perverse hardware setup.
	પાછા (long_u)((ms.ullTotalPhys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    પાછા (long_u)((ms.ullTotalPhys - WINNT_RESERVE_BYTES) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    પૂર્ણાંક
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    પૂર્ણાંક		flags)		// EW_* flags
{
    પાછા dos_expandpath(gap, path, 0, flags, FALSE);
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
    પૂર્ણાંક
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	szTempFile[_MAX_PATH + 1];
    WCHAR	szNewPath[_MAX_PATH + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    માટે (p = q = wold; *p != L'\0'; ++p)
    {
	પસંદગી (*p)
	{
	સ્થિતિ L'/':
	સ્થિતિ L'\\':
	સ્થિતિ L':':
	    જો (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    તોડો;

	મૂળભૂત:
	    તોડો;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    જો ((પૂર્ણાંક)(p - q) < 8 || q[6] != L'~')
	પાછા (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    જો (GetFullPathNameW(wnew, _MAX_PATH, szNewPath, &p) == 0 || p == ખાલી)
	પાછા -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    જો (GetTempFileNameW(szNewPath, L"VIM", 0, szTempFile) == 0)
	પાછા -2;

    // blow the temp file away
    જો (!DeleteFileW(szTempFile))
	પાછા -3;

    // rename old file to the temp file
    જો (!MoveFileW(wold, szTempFile))
	પાછા -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    જો ((hf = CreateFileW(wold, GENERIC_WRITE, 0, ખાલી, CREATE_NEW,
		    FILE_ATTRIBUTE_NORMAL, ખાલી)) == INVALID_HANDLE_કિંમત)
	પાછા -5;
    જો (!CloseHandle(hf))
	પાછા -6;

    // rename the temp file to the new file
    જો (!MoveFileW(szTempFile, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(શૂન્ય)MoveFileW(szTempFile, wold);
	પાછા -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(szTempFile);

    // finally, remove the empty old file
    જો (!DeleteFileW(wold))
	પાછા -8;

    પાછા 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    પૂર્ણાંક
mch_rename(
    અચલ અક્ષર	*pszOldFile,
    અચલ અક્ષર	*pszNewFile)
{
    WCHAR	*wold = ખાલી;
    WCHAR	*wnew = ખાલી;
    પૂર્ણાંક		retval = -1;

    wold = enc_to_utf16((char_u *)pszOldFile, ખાલી);
    wnew = enc_to_utf16((char_u *)pszNewFile, ખાલી);
    જો (wold != ખાલી && wnew != ખાલી)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    પાછા retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    અક્ષર *
default_shell(શૂન્ય)
{
    પાછા "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    પૂર્ણાંક
mch_access(અક્ષર *n, પૂર્ણાંક p)
{
    HANDLE	hFile;
    પૂર્ણાંક		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_to_utf16((char_u *)n, ખાલી);
    જો (wn == ખાલી)
	પાછા -1;

    જો (mch_isdir((char_u *)n))
    {
	WCHAR TempNameW[_MAX_PATH + 16] = L"";

	જો (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    પૂર્ણાંક			i;
	    WIN32_FIND_DATAW    d;

	    માટે (i = 0; i < _MAX_PATH && wn[i] != 0; ++i)
		TempNameW[i] = wn[i];
	    જો (TempNameW[i - 1] != '\\' && TempNameW[i - 1] != '/')
		TempNameW[i++] = '\\';
	    TempNameW[i++] = '*';
	    TempNameW[i++] = 0;

	    hFile = FindFirstFileW(TempNameW, &d);
	    જો (hFile == INVALID_HANDLE_કિંમત)
		જાઓ getout;
	    નહીંતર
		(શૂન્ય)FindClose(hFile);
	}

	જો (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    જો (!GetTempFileNameW(wn, L"VIM", 0, TempNameW))
		જાઓ getout;
	    નહીંતર
		DeleteFileW(TempNameW);
	}
    }
    નહીંતર
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_mode = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_mode, share_mode,
					     ખાલી, OPEN_EXISTING, 0, ખાલી);
	જો (hFile == INVALID_HANDLE_કિંમત)
	    જાઓ getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    પાછા retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    પૂર્ણાંક
mch_open(અચલ અક્ષર *name, પૂર્ણાંક flags, પૂર્ણાંક mode)
{
    WCHAR	*wn;
    પૂર્ણાંક		f;

    wn = enc_to_utf16((char_u *)name, ખાલી);
    જો (wn == ખાલી)
	પાછા -1;

    f = _wopen(wn, flags, mode);
    vim_free(wn);
    પાછા f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(અચલ અક્ષર *name, અચલ અક્ષર *mode)
{
    WCHAR	*wn, *wm;
    FILE	*f = ખાલી;

#જો defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    અક્ષર newMode = mode[strlen(mode) - 1];
    પૂર્ણાંક oldMode = 0;

    _get_fmode(&oldMode);
    જો (newMode == 't')
	_set_fmode(_O_TEXT);
    નહીંતર જો (newMode == 'b')
	_set_fmode(O_દ્વિઅંકી_2);
#endif
    wn = enc_to_utf16((char_u *)name, ખાલી);
    wm = enc_to_utf16((char_u *)mode, ખાલી);
    જો (wn != ખાલી && wm != ખાલી)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#જો defined(DEBUG) && _MSC_VER >= 1400
    જો (oldMode != 0)
	_set_fmode(oldMode);
#endif
    પાછા f;
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
    સ્થિર શૂન્ય
copy_substream(HANDLE sh, શૂન્ય *context, WCHAR *to, WCHAR *substream, long len)
{
    HANDLE  hTo;
    WCHAR   *to_name;

    to_name = alloc((wcslen(to) + wcslen(substream) + 1) * કદ(WCHAR));
    જો (to_name == ખાલી)
	પાછા;

    wcscpy(to_name, to);
    wcscat(to_name, substream);

    hTo = CreateFileW(to_name, GENERIC_WRITE, 0, ખાલી, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL, ખાલી);
    જો (hTo != INVALID_HANDLE_કિંમત)
    {
	long	done;
	DWORD	todo;
	DWORD	readcnt, written;
	અક્ષર	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	માટે (done = 0; done < len; done += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - done) > કદ(buf) ? કદ(buf)
						       : (size_t)(len - done));
	    જો (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(hTo, buf, todo, &written, ખાલી)
		    || written != todo)
		તોડો;
	}
	CloseHandle(hTo);
    }

    મુક્ત(to_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    સ્થિર શૂન્ય
copy_infostreams(char_u *from, char_u *to)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_STREAM_ID	sid;
    પૂર્ણાંક			headersize;
    WCHAR		streamname[_MAX_PATH];
    DWORD		readcount;
    શૂન્ય		*context = ખાલી;
    DWORD		lo, hi;
    પૂર્ણાંક			len;

    // Convert the file names to wide characters.
    fromw = enc_to_utf16(from, ખાલી);
    tow = enc_to_utf16(to, ખાલી);
    જો (fromw != ખાલી && tow != ખાલી)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, ખાલી,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, ખાલી);
	જો (sh != INVALID_HANDLE_કિંમત)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    માટે (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		ZeroMemory(&sid, કદ(WIN32_STREAM_ID));
		headersize = (પૂર્ણાંક)((અક્ષર *)&sid.cStreamName - (અક્ષર *)&sid.dwStreamId);
		જો (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    તોડો;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		જો (sid.dwStreamNameSize > 0)
		{
		    // Read the stream name.
		    જો (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dwStreamNameSize,
					  &readcount, FALSE, FALSE, &context))
			તોડો;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / કદ(WCHAR);
		    streamname[len] = 0;
		    જો (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (long)sid.Size.u.નીચલું_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(શૂન્ય)BackupSeek(sh, sid.Size.u.નીચલું_Part, sid.Size.u.ઉપલું_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (શૂન્ય)BackupRead(sh, ખાલી, 0, &readcount, TRUE, FALSE, &context);

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

પ્રકારવ્યાખ્યા સંરચના _FILE_FULL_EA_INFORMATION_ {
    ULONG  NextEntryOffset;
    UCHAR  Flags;
    UCHAR  EaNameLength;
    USHORT Ea_કિંમત_Length;
    CHAR   EaName[1];
} FILE_FULL_EA_INFORMATION_, *PFILE_FULL_EA_INFORMATION_;

પ્રકારવ્યાખ્યા સંરચના _FILE_EA_INFORMATION_ {
    ULONG EaSize;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#ifndef PROTO
પ્રકારવ્યાખ્યા NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions);
પ્રકારવ્યાખ્યા NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
પ્રકારવ્યાખ્યા NTSTATUS (NTAPI *PfnNtSetEaFile)(
	HANDLE		    FileHandle,
	PIO_STATUS_BLOCK    IoStatusBlock,
	PVOID		    Buffer,
	ULONG		    Length);
પ્રકારવ્યાખ્યા NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	BOOLEAN ReturnSingleEntry,
	PVOID EaList,
	ULONG EaListLength,
	PULONG EaIndex,
	BOOLEAN RestartScan);
પ્રકારવ્યાખ્યા NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_STATUS_BLOCK	IoStatusBlock,
	PVOID			FileInformation,
	ULONG			Length,
	FILE_INFORMATION_CLASS FileInformationClass);
પ્રકારવ્યાખ્યા VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString);

PfnNtOpenFile pNtOpenFile = ખાલી;
PfnNtClose pNtClose = ખાલી;
PfnNtSetEaFile pNtSetEaFile = ખાલી;
PfnNtQueryEaFile pNtQueryEaFile = ખાલી;
PfnNtQueryInformationFile pNtQueryInformationFile = ખાલી;
PfnRtlInitUnicodeString pRtlInitUnicodeString = ખાલી;
#endif

/*
 * Load ntdll.dll functions.
 */
    સ્થિર BOOL
load_ntdll(શૂન્ય)
{
    સ્થિર પૂર્ણાંક	loaded = -1;

    જો (loaded != -1)
	પાછા (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    જો (hNtdll != ખાલી)
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
    જો (pNtOpenFile == ખાલી
	    || pNtClose == ખાલી
	    || pNtSetEaFile == ખાલી
	    || pNtQueryEaFile == ખાલી
	    || pNtQueryInformationFile == ખાલી
	    || pRtlInitUnicodeString == ખાલી)
	loaded = FALSE;
    નહીંતર
	loaded = TRUE;
    પાછા (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    સ્થિર શૂન્ય
copy_extattr(char_u *from, char_u *to)
{
    char_u		    *fromf = ખાલી;
    char_u		    *tof = ખાલી;
    WCHAR		    *fromw = ખાલી;
    WCHAR		    *tow = ખાલી;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_ATTRIBUTES	    oa;
    IO_STATUS_BLOCK	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    શૂન્ય		    *ea = ખાલી;

    જો (!load_ntdll())
	પાછા;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(to) + 5);
    જો (fromf == ખાલી || tof == ખાલી)
	જાઓ theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), to);

    // Convert the names to wide characters.
    fromw = enc_to_utf16(fromf, ખાલી);
    tow = enc_to_utf16(tof, ખાલી);
    જો (fromw == ખાલી || tow == ખાલી)
	જાઓ theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    InitializeObjectAttributes(&oa, &u, 0, ખાલી, ખાલી);
    જો (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	જાઓ theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, કદ(eainfo),
	    FileEaInformation);
    જો (eainfo.EaSize != 0)
    {
	ea = alloc(eainfo.EaSize);
	જો (ea != ખાલી)
	{
	    જો (pNtQueryEaFile(h, &iosb, ea, eainfo.EaSize, FALSE,
			ખાલી, 0, ખાલી, TRUE) != STATUS_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    જો (ea != ખાલી)
    {
	pRtlInitUnicodeString(&u, tow);
	InitializeObjectAttributes(&oa, &u, 0, ખાલી, ખાલી);
	જો (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	    જાઓ theend;

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
    પૂર્ણાંક
mch_copy_file_attribute(char_u *from, char_u *to)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, to);
    copy_extattr(from, to);
    પાછા 0;
}


/*
 * The command line arguments in UTF-16
 */
સ્થિર પૂર્ણાંક	nArgsW = 0;
સ્થિર LPWSTR	*ArglistW = ખાલી;
સ્થિર પૂર્ણાંક	global_argc = 0;
સ્થિર અક્ષર	**global_argv;

સ્થિર પૂર્ણાંક	used_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
સ્થિર પૂર્ણાંક	*used_file_indexes = ખાલી; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
સ્થિર પૂર્ણાંક	used_file_ગણતરી = 0;	// nr of entries in used_file_indexes
સ્થિર પૂર્ણાંક	used_file_literal = FALSE;  // take file names literally
સ્થિર પૂર્ણાંક	used_file_full_path = FALSE;  // file name was full path
#ifdef FEAT_DIFF
સ્થિર પૂર્ણાંક	used_file_diff_mode = FALSE;  // file name was with diff mode
#endif
સ્થિર પૂર્ણાંક	used_alist_ગણતરી = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    પૂર્ણાંક
get_cmd_argsW(અક્ષર ***argvp)
{
    અક્ષર	**argv = ખાલી;
    પૂર્ણાંક		argc = 0;
    પૂર્ણાંક		i;

    free_cmd_argsW();
    ArglistW = Command_લીટી_To_Argv_W(Get_Command_લીટી_W(), &nArgsW);
    જો (ArglistW != ખાલી)
    {
	argv = alloc((nArgsW + 1) * કદ(અક્ષર *));
	જો (argv != ખાલી)
	{
	    argc = nArgsW;
	    argv[argc] = ખાલી;
	    માટે (i = 0; i < argc; ++i)
	    {
		પૂર્ણાંક	len;

		// Convert each Unicode argument to UTF-8.
		WideCharToMultiByte_alloc(CP_UTF8, 0,
				ArglistW[i], (પૂર્ણાંક)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		જો (argv[i] == ખાલી)
		{
		    // Out of memory, clear everything.
		    જ્યારે (i > 0)
			મુક્ત(argv[--i]);
		    મુક્ત(argv);
		    argv = ખાલી;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    જો (argc > 0)
    {
	જો (used_file_indexes != ખાલી)
	    મુક્ત(used_file_indexes);
	used_file_indexes = alloc(argc * કદ(પૂર્ણાંક));
    }

    જો (argvp != ખાલી)
	*argvp = argv;
    પાછા argc;
}

    શૂન્ય
free_cmd_argsW(શૂન્ય)
{
    જો (ArglistW == ખાલી)
	પાછા;

    GlobalFree(ArglistW);
    ArglistW = ખાલી;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    શૂન્ય
used_file_arg(અક્ષર *name, પૂર્ણાંક literal, પૂર્ણાંક full_path, પૂર્ણાંક diff_mode)
{
    પૂર્ણાંક		i;

    જો (used_file_indexes == ખાલી)
	પાછા;
    માટે (i = used_file_argc + 1; i < global_argc; ++i)
	જો (STRCMP(global_argv[i], name) == 0)
	{
	    used_file_argc = i;
	    used_file_indexes[used_file_ગણતરી++] = i;
	    તોડો;
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
    શૂન્ય
set_alist_ગણતરી(શૂન્ય)
{
    used_alist_ગણતરી = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    શૂન્ય
fix_arg_enc(શૂન્ય)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		idx;
    char_u	*str;
    પૂર્ણાંક		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    જો (global_argc != nArgsW
	    || ArglistW == ખાલી
	    || used_file_indexes == ખાલી
	    || used_file_ગણતરી == 0
	    || used_alist_ગણતરી != GARGCOUNT)
	પાછા;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(પૂર્ણાંક, GARGCOUNT);
    જો (fnum_list == ખાલી)
	પાછા;		// out of memory
    માટે (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    જો (ga_grow(&global_alist.al_ga, used_file_ગણતરી) == FAIL)
	પાછા;		// out of memory

    માટે (i = 0; i < used_file_ગણતરી; ++i)
    {
	idx = used_file_indexes[i];
	str = utf16_to_enc(ArglistW[idx], ખાલી);
	જો (str != ખાલી)
	{
	    પૂર્ણાંક literal = used_file_literal;

#ifdef FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    જો (used_file_diff_mode && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), tail, STRLEN(tail), TRUE, &ret);
		જો (ret.string != ખાલી)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#endif
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    જો (used_file_literal)
		buf_set_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    જો (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		જો (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_add(&global_alist, str, literal ? 2 : 0);
	}
    }

    જો (!used_file_literal)
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
	alist_expand(fnum_list, used_alist_ગણતરી);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    જો (curwin->w_arg_idx == 0 && curbuf->b_fname == ખાલી)
    {
	do_cmdline_cmd((char_u *)":rewind");
	જો (GARGCOUNT == 1 && used_file_full_path
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    last_chdir_reason = "drop";
    }

    set_alist_ગણતરી();
}

    પૂર્ણાંક
mch_setenv(અક્ષર *var, અક્ષર *કિંમત, પૂર્ણાંક x UNUSED)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(કિંમત) + 2);
    જો (envbuf == ખાલી)
	પાછા -1;

    sprintf((અક્ષર *)envbuf, "%s=%s", var, કિંમત);

    p = enc_to_utf16(envbuf, ખાલી);

    vim_free(envbuf);
    જો (p == ખાલી)
	પાછા -1;
    _wputenv(p);
#ifdef libintl_wputenv
    libintl_wputenv(p);
#endif
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    પાછા 0;
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

    સ્થિર શૂન્ય
vtp_flag_init(શૂન્ય)
{
#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   mode;
    HANDLE  out;

# ifdef VIMDLL
    જો (!gui.in_use)
# endif
    {
	out = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_version >= VTP_FIRST_SUPPORT_BUILD) ? 1 : 0;
	GetConsoleMode(out, &mode);
	mode |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	જો (SetConsoleMode(out, mode) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_buffer = win_version >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#endif

    જો (win_version >= CONPTY_FIRST_SUPPORT_BUILD)
	conpty_working = 1;
    જો (win_version >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    જો (win_version <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    જો (win_version <= CONPTY_1909_BUILD)
	conpty_type = 2;
    જો (win_version <= CONPTY_1903_BUILD)
	conpty_type = 2;
    જો (win_version < CONPTY_FIRST_SUPPORT_BUILD)
	conpty_type = 1;

    જો (win_version >= CONPTY_NEXT_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    સ્થિર શૂન્ય
vtp_init(શૂન્ય)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = કદ(csbi);
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

    સ્થિર શૂન્ય
vtp_exit(શૂન્ય)
{
    restore_console_color_rgb();
}

    પૂર્ણાંક
vtp_printf(
    અક્ષર *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   result;
    પૂર્ણાંક	    len;

    જો (silent_mode)
	પાછા 0;

    va_start(list, format);
    len = vim_vsnprintf((અક્ષર *)buf, 100, (અક્ષર *)format, list);
    va_end(list);
    WriteConsoleA(g_hConOut, buf, (DWORD)len, &result, ખાલી);
    પાછા (પૂર્ણાંક)result;
}

    સ્થિર શૂન્ય
vtp_sgr_bulk(
    પૂર્ણાંક arg)
{
    પૂર્ણાંક args[1];

    args[0] = arg;
    vtp_sgr_bulks(1, args);
}

# define FAST256(x) \
    જો ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    સ્થિતિ x: \
	FAST256(newargs[x - 1]);

    સ્થિર શૂન્ય
vtp_sgr_bulks(
    પૂર્ણાંક argc,
    પૂર્ણાંક *args)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    પૂર્ણાંક	    in, out;
    પૂર્ણાંક	    newargs[16];
    સ્થિર પૂર્ણાંક sgrfgr = -1, sgrfgg, sgrfgb;
    સ્થિર પૂર્ણાંક sgrbgr = -1, sgrbgg, sgrbgb;

    જો (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	પાછા;
    }

    in = out = 0;
    જ્યારે (in < argc)
    {
	પૂર્ણાંક s = args[in];
	પૂર્ણાંક copylen = 1;

	જો (s == 38)
	{
	    જો (argc - in >= 5 && args[in + 1] == 2)
	    {
		જો (sgrfgr == args[in + 2] && sgrfgg == args[in + 3]
						     && sgrfgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		નહીંતર
		{
		    sgrfgr = args[in + 2];
		    sgrfgg = args[in + 3];
		    sgrfgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    નહીંતર જો (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	નહીંતર જો (s == 48)
	{
	    જો (argc - in >= 5 && args[in + 1] == 2)
	    {
		જો (sgrbgr == args[in + 2] && sgrbgg == args[in + 3]
						     && sgrbgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		નહીંતર
		{
		    sgrbgr = args[in + 2];
		    sgrbgg = args[in + 3];
		    sgrbgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    નહીંતર જો (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	નહીંતર જો (30 <= s && s <= 39)
	    sgrfgr = -1;
	નહીંતર જો (90 <= s && s <= 97)
	    sgrfgr = -1;
	નહીંતર જો (40 <= s && s <= 49)
	    sgrbgr = -1;
	નહીંતર જો (100 <= s && s <= 107)
	    sgrbgr = -1;
	નહીંતર જો (s == 0)
	    sgrfgr = sgrbgr = -1;

	જ્યારે (copylen--)
	    newargs[out++] = args[in++];
    }

    p = &buf[કદ(buf) - 1];
    *p-- = 'm';

    પસંદગી (out)
    {
	પૂર્ણાંક	n, m;
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
	WriteConsoleA(g_hConOut, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, ખાલી);
    મૂળભૂત:
	તોડો;
    }
}

    સ્થિર શૂન્ય
wt_init(શૂન્ય)
{
    wt_working = mch_getenv("WT_SESSION") != ખાલી;
}

# ifdef FEAT_TERMGUICOLORS
    સ્થિર પૂર્ણાંક
ctermtoxterm(
    પૂર્ણાંક cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    પાછા (((પૂર્ણાંક)r << 16) | ((પૂર્ણાંક)g << 8) | (પૂર્ણાંક)b);
}
# endif

    સ્થિર શૂન્ય
set_console_color_rgb(શૂન્ય)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    guicolor_T	fg, bg;
    પૂર્ણાંક		ctermfg, ctermbg;

    જો (!vtp_working)
	પાછા;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    જો (p_tgc || t_colors >= 256)
    {
	term_fg_rgb_color(fg);
	term_bg_rgb_color(bg);
	પાછા;
    }

    જો (use_alternate_screen_buffer)
	પાછા;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cbSize = કદ(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = કદ(csbi);
    csbi.srWindow.જમણું_3 += 1;
    csbi.srWindow.Bottom += 1;
    store_console_bg_rgb = csbi.ColorTable[g_color_index_bg];
    store_console_fg_rgb = csbi.ColorTable[g_color_index_fg];
    csbi.ColorTable[g_color_index_bg] = (COLORREF)bg;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)fg;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

# જો defined(FEAT_TERMGUICOLORS)
    શૂન્ય
get_default_console_color(
    પૂર્ણાંક *cterm_fg,
    પૂર્ણાંક *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    પૂર્ણાંક id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    પૂર્ણાંક ctermfg = 0;
    પૂર્ણાંક ctermbg = 0;
    પૂર્ણાંક dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    જો (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    જો (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	જો (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	જો (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	નહીંતર
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_normal_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    જો (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	જો (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	જો (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	નહીંતર
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
    સ્થિર શૂન્ય
reset_console_color_rgb(શૂન્ય)
{
# ifdef FEAT_TERMGUICOLORS
    જો (use_alternate_screen_buffer)
	પાછા;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = કદ(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = કદ(csbi);
    csbi.srWindow.જમણું_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)store_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)store_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    સ્થિર શૂન્ય
restore_console_color_rgb(શૂન્ય)
{
# ifdef FEAT_TERMGUICOLORS
    જો (use_alternate_screen_buffer)
	પાછા;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = કદ(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = કદ(csbi);
    csbi.srWindow.જમણું_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)save_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)save_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

    શૂન્ય
control_console_color_rgb(શૂન્ય)
{
    જો (vtp_working)
	set_console_color_rgb();
    નહીંતર
	reset_console_color_rgb();
}

    પૂર્ણાંક
use_vtp(શૂન્ય)
{
    પાછા USE_VTP;
}

    પૂર્ણાંક
is_term_win32(શૂન્ય)
{
    પાછા T_NAME != ખાલી && STRCMP(T_NAME, "win32") == 0;
}

    પૂર્ણાંક
has_vtp_working(શૂન્ય)
{
    પાછા vtp_working;
}

#endif

    પૂર્ણાંક
has_conpty_working(શૂન્ય)
{
    પાછા conpty_working;
}

    પૂર્ણાંક
get_conpty_type(શૂન્ય)
{
    પાછા conpty_type;
}

    પૂર્ણાંક
is_conpty_stable(શૂન્ય)
{
    પાછા conpty_stable;
}

    પૂર્ણાંક
get_conpty_fix_type(શૂન્ય)
{
    પાછા conpty_fix_type;
}

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    શૂન્ય
resize_console_buf(શૂન્ય)
{
    જો (use_alternate_screen_buffer)
	પાછા;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    જો (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	પાછા;

    coord.X = SRWIDTH(csbi.srWindow);
    coord.Y = SRHEIGHT(csbi.srWindow);
    SetConsoleScreenBufferSize(g_hConOut, coord);

    newsize.ડાબું_3 = 0;
    newsize.ટોચ_2 = 0;
    newsize.જમણું_3 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    SetConsoleWindowInfo(g_hConOut, TRUE, &newsize);

    SetConsoleScreenBufferSize(g_hConOut, coord);
}
#endif

    અક્ષર *
GetWin32Error(શૂન્ય)
{
    સ્થિર અક્ષર	*oldmsg = ખાલી;
    અક્ષર	*acp_સંદેશ = ખાલી;
    DWORD	acp_len;
    અક્ષર	*enc_સંદેશ = ખાલી;
    પૂર્ણાંક		enc_len = 0;

    // get formatted message from OS
    acp_len = FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
	    ખાલી, GetLastError(), 0, (LPSTR)&acp_સંદેશ, 0, ખાલી);
    જો (acp_len == 0 || acp_સંદેશ == ખાલી)
	પાછા ખાલી;

    // clean oldmsg if remained.
    જો (oldmsg != ખાલી)
    {
	vim_free(oldmsg);
	oldmsg = ખાલી;
    }

    acp_to_enc((char_u *)acp_સંદેશ, (પૂર્ણાંક)acp_len, (char_u **)&enc_સંદેશ, &enc_len);
    LocalFree(acp_સંદેશ);
    જો (enc_સંદેશ == ખાલી)
	પાછા ખાલી;

    // remove trailing \r\n
    અક્ષર *pcrlf = strstr(enc_સંદેશ, "\r\n");
    જો (pcrlf != ખાલી)
	*pcrlf = NUL;

    oldmsg = enc_સંદેશ;
    પાછા enc_સંદેશ;
}

#જો defined(FEAT_RELTIME)
સ્થિર HANDLE   timer_handle;
સ્થિર પૂર્ણાંક      timer_active = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
સ્થિર sig_atomic_t timeout_flags[2];
સ્થિર પૂર્ણાંક	    timeout_flag_idx = 0;
સ્થિર sig_atomic_t *timeout_flag = &timeout_flags[0];


    સ્થિર શૂન્ય CALLBACK
set_flag(શૂન્ય *param, BOOLEAN unused2 UNUSED)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    શૂન્ય
stop_timeout(શૂન્ય)
{
    જો (timer_active)
    {
	BOOL ret = DeleteTimerQueueTimer(ખાલી, timer_handle, ખાલી);
	timer_active = FALSE;
	જો (!ret && GetLastError() != ERROR_IO_PENDING)
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
    અસ્થિર sig_atomic_t *
start_timeout(long msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = CreateTimerQueueTimer(
	    &timer_handle, ખાલી, set_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    જો (!ret)
    {
	semsg(_(e_could_not_set_timeout_str), GetWin32Error());
    }
    નહીંતર
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_active = TRUE;
	*timeout_flag = FALSE;
    }
    પાછા timeout_flag;
}
#endif
