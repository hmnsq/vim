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

#അന്തര്ഭാവ "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#അന്തര്ഭാവ <sys/types.h>
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <limits.h>

#അന്തര്ഭാവ <process.h>
#അന്തര്ഭാവ <winternl.h>
#അന്തര്ഭാവ <direct.h>

#എങ്കിൽ !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#ifdef FEAT_JOB_CHANNEL
# include <tlhelp32.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = ശൂന്യ;
#endif

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
സ്ഥിതം HANDLE g_hConIn  = INVALID_HANDLE_മൂല്യം;
സ്ഥിതം HANDLE g_hConOut = INVALID_HANDLE_മൂല്യം;

// Win32 Screen buffer,coordinate,console I/O information
സ്ഥിതം SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
സ്ഥിതം COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
സ്ഥിതം WORD  g_attrDefault = 7;  // lightgray text on black background
സ്ഥിതം WORD  g_attrCurrent;

സ്ഥിതം പൂർണ്ണസംഖ്യ g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
സ്ഥിതം പൂർണ്ണസംഖ്യ g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
സ്ഥിതം പൂർണ്ണസംഖ്യ g_fForceExit = FALSE;    // set when forcefully exiting

സ്ഥിതം ശൂന്യം scroll(unsigned c_വരികൾ);
സ്ഥിതം ശൂന്യം set_scroll_region(unsigned ഇടത്, unsigned മുകൾ,
			      unsigned വലത്, unsigned bottom);
സ്ഥിതം ശൂന്യം set_scroll_region_tb(unsigned മുകൾ, unsigned bottom);
സ്ഥിതം ശൂന്യം set_scroll_region_lr(unsigned ഇടത്, unsigned വലത്);
സ്ഥിതം ശൂന്യം തിരുകുക_വരികൾ(unsigned c_വരികൾ);
സ്ഥിതം ശൂന്യം delete_വരികൾ(unsigned c_വരികൾ);
സ്ഥിതം ശൂന്യം gotoxy(unsigned x, unsigned y);
സ്ഥിതം ശൂന്യം standout(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ s_cursor_visible = TRUE;
സ്ഥിതം പൂർണ്ണസംഖ്യ did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
തരനിർവചനം ഘടന നിവേശം_record_buffer_കണ്ണി_S
{
    നിവേശം_RECORD ir;
    ഘടന നിവേശം_record_buffer_കണ്ണി_S *next;
} നിവേശം_record_buffer_കണ്ണി_T;
തരനിർവചനം ഘടന നിവേശം_record_buffer_S
{
    നിവേശം_record_buffer_കണ്ണി_T *head;
    നിവേശം_record_buffer_കണ്ണി_T *tail;
    പൂർണ്ണസംഖ്യ length;
} നിവേശം_record_buffer_T;
സ്ഥിതം നിവേശം_record_buffer_T നിവേശം_record_buffer;
സ്ഥിതം പൂർണ്ണസംഖ്യ read_നിവേശം_record_buffer(നിവേശം_RECORD* irEvents, പൂർണ്ണസംഖ്യ nMaxLength);
സ്ഥിതം പൂർണ്ണസംഖ്യ write_നിവേശം_record_buffer(നിവേശം_RECORD* irEvents, പൂർണ്ണസംഖ്യ nLength);
#endif
#ifdef FEAT_GUI_MSWIN
സ്ഥിതം പൂർണ്ണസംഖ്യ s_dont_use_vimrun = TRUE;
സ്ഥിതം പൂർണ്ണസംഖ്യ need_vimrun_warning = FALSE;
സ്ഥിതം string_T vimrun_path = {(char_u *)"vimrun ", 7};
സ്ഥിതം പൂർണ്ണസംഖ്യ vimrun_path_allocated = FALSE;
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ win32_getattrs(char_u *name);
സ്ഥിതം പൂർണ്ണസംഖ്യ win32_setattrs(char_u *name, പൂർണ്ണസംഖ്യ attrs);
സ്ഥിതം പൂർണ്ണസംഖ്യ win32_set_archive(char_u *name);

സ്ഥിതം പൂർണ്ണസംഖ്യ conpty_working = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ conpty_type = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ conpty_stable = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ conpty_fix_type = 0;
സ്ഥിതം ശൂന്യം vtp_flag_init(ശൂന്യം);

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
സ്ഥിതം പൂർണ്ണസംഖ്യ vtp_working = 0;
സ്ഥിതം ശൂന്യം vtp_init(ശൂന്യം);
സ്ഥിതം ശൂന്യം vtp_exit(ശൂന്യം);
സ്ഥിതം ശൂന്യം vtp_sgr_bulk(പൂർണ്ണസംഖ്യ arg);
സ്ഥിതം ശൂന്യം vtp_sgr_bulks(പൂർണ്ണസംഖ്യ argc, പൂർണ്ണസംഖ്യ *argv);

സ്ഥിതം പൂർണ്ണസംഖ്യ wt_working = 0;
സ്ഥിതം ശൂന്യം wt_init(ശൂന്യം);

സ്ഥിതം പൂർണ്ണസംഖ്യ g_color_index_bg = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ g_color_index_fg = 7;

# ifdef FEAT_TERMGUICOLORS
സ്ഥിതം guicolor_T save_console_bg_rgb;
സ്ഥിതം guicolor_T save_console_fg_rgb;
സ്ഥിതം guicolor_T store_console_bg_rgb;
സ്ഥിതം guicolor_T store_console_fg_rgb;
സ്ഥിതം പൂർണ്ണസംഖ്യ default_console_color_bg = 0x000000; // black
സ്ഥിതം പൂർണ്ണസംഖ്യ default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_term_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# അല്ലെങ്കിൽ
#  define USE_VTP		0
#  define USE_WT		0
# endif

സ്ഥിതം ശൂന്യം set_console_color_rgb(ശൂന്യം);
സ്ഥിതം ശൂന്യം reset_console_color_rgb(ശൂന്യം);
സ്ഥിതം ശൂന്യം restore_console_color_rgb(ശൂന്യം);
#endif  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
സ്ഥിതം പൂർണ്ണസംഖ്യ suppress_winsize = 1;	// don't fiddle with console
#endif

സ്ഥിതം WCHAR *exe_pathw = ശൂന്യ;

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
സ്ഥിതം BOOL use_alternate_screen_buffer = FALSE;
#endif

ബാഹ്യ DWORD win_version; // this is in os_mswin.c

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    സ്ഥിതം BOOL
is_ambiwidth_event(
    നിവേശം_RECORD *ir)
{
    തിരികെ ir->EventType == താക്കോൽ_EVENT
		&& ir->Event.താക്കോൽ_Event_2.b_താക്കോൽ_Down
		&& ir->Event.താക്കോൽ_Event_2.w_Repeat_എണ്ണം == 1
		&& ir->Event.താക്കോൽ_Event_2.w_Virtual_താക്കോൽ_Code == 0x12
		&& ir->Event.താക്കോൽ_Event_2.wVirtualScanCode == 0x38
		&& ir->Event.താക്കോൽ_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.താക്കോൽ_Event_2.dw_Control_താക്കോൽ_State == 2;
}

    സ്ഥിതം ശൂന്യം
make_ambiwidth_event(
    നിവേശം_RECORD *down,
    നിവേശം_RECORD *up)
{
    down->Event.താക്കോൽ_Event_2.w_Virtual_താക്കോൽ_Code = 0;
    down->Event.താക്കോൽ_Event_2.wVirtualScanCode = 0;
    down->Event.താക്കോൽ_Event_2.uChar.UnicodeChar
				    = up->Event.താക്കോൽ_Event_2.uChar.UnicodeChar;
    down->Event.താക്കോൽ_Event_2.dw_Control_താക്കോൽ_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    സ്ഥിതം BOOL
read_console_നിവേശം(
    HANDLE	    h_നിവേശം,
    നിവേശം_RECORD    *lpBuffer,
    പൂർണ്ണസംഖ്യ		    nLength,
    LPDWORD	    lpEvents)
{
    എണ്ണൽ
    {
	IRSIZE = 10
    };
    സ്ഥിതം നിവേശം_RECORD s_irCache[IRSIZE];
    സ്ഥിതം DWORD s_dwIndex = 0;
    സ്ഥിതം DWORD s_dwMax = 0;
    DWORD dwEvents;
    പൂർണ്ണസംഖ്യ head;
    പൂർണ്ണസംഖ്യ tail;
    പൂർണ്ണസംഖ്യ i;
    സ്ഥിതം നിവേശം_RECORD s_irPseudo;

    എങ്കിൽ (s_dwMax == 0 && നിവേശം_record_buffer.length > 0)
    {
	dwEvents = read_നിവേശം_record_buffer(s_irCache, IRSIZE);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
    }

    എങ്കിൽ (nLength == -2)
	തിരികെ (s_dwMax > 0) ? TRUE : FALSE;

    എങ്കിൽ (win_version < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	എങ്കിൽ (nLength == -1)
	    തിരികെ Peek_Console_നിവേശം_W(h_നിവേശം, lpBuffer, 1, lpEvents);
	തിരികെ Read_Console_നിവേശം_W(h_നിവേശം, lpBuffer, 1, &dwEvents);
    }

    എങ്കിൽ (s_dwMax == 0)
    {
	എങ്കിൽ (!vtp_working && nLength == -1)
	    തിരികെ Peek_Console_നിവേശം_W(h_നിവേശം, lpBuffer, 1, lpEvents);
	Get_സംഖ്യ_Of_Console_നിവേശം_Events(h_നിവേശം, &dwEvents);
	എങ്കിൽ (dwEvents == 0 && nLength == -1)
	    തിരികെ Peek_Console_നിവേശം_W(h_നിവേശം, lpBuffer, 1, lpEvents);
	Read_Console_നിവേശം_W(h_നിവേശം, s_irCache, IRSIZE, &dwEvents);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
	എങ്കിൽ (dwEvents == 0)
	{
	    *lpEvents = 0;
	    തിരികെ TRUE;
	}

	വേണ്ടി (i = s_dwIndex; i < (പൂർണ്ണസംഖ്യ)s_dwMax - 1; ++i)
	    എങ്കിൽ (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	എങ്കിൽ (s_dwMax > 1)
	{
	    head = 0;
	    tail = s_dwMax - 1;
	    ആകുമ്പോൾ (head != tail)
	    {
		എങ്കിൽ (s_irCache[head].EventType == WINDOW_BUFFER_SIZE_EVENT
			&& s_irCache[head + 1].EventType
						  == WINDOW_BUFFER_SIZE_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    വേണ്ടി (i = head; i < tail; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --tail;
		    തുടരുക;
		}
		head++;
	    }
	    s_dwMax = tail + 1;
	}
    }

    എങ്കിൽ (s_irCache[s_dwIndex].EventType == താക്കോൽ_EVENT)
    {
	എങ്കിൽ (s_irCache[s_dwIndex].Event.താക്കോൽ_Event_2.w_Repeat_എണ്ണം > 1)
	{
	    s_irPseudo = s_irCache[s_dwIndex];
	    s_irPseudo.Event.താക്കോൽ_Event_2.w_Repeat_എണ്ണം = 1;
	    s_irCache[s_dwIndex].Event.താക്കോൽ_Event_2.w_Repeat_എണ്ണം--;
	    *lpBuffer = s_irPseudo;
	    *lpEvents = 1;
	    തിരികെ TRUE;
	}
    }

    *lpBuffer = s_irCache[s_dwIndex];
    എങ്കിൽ (!(nLength == -1 || nLength == -2) && ++s_dwIndex >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    തിരികെ TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    സ്ഥിതം BOOL
peek_console_നിവേശം(
    HANDLE	    h_നിവേശം,
    നിവേശം_RECORD    *lpBuffer,
    DWORD	    nLength UNUSED,
    LPDWORD	    lpEvents)
{
    തിരികെ read_console_നിവേശം(h_നിവേശം, lpBuffer, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    സ്ഥിതം DWORD
സന്ദേശം_wait_for_multiple_objects(
    DWORD    n_എണ്ണം,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dwWakeMask)
{
    എങ്കിൽ (read_console_നിവേശം(ശൂന്യ, ശൂന്യ, -2, ശൂന്യ))
	തിരികെ WAIT_OBJECT_0;
    തിരികെ സന്ദേശം_Wait_For_Multiple_Objects(n_എണ്ണം, pHandles, fWaitAll,
				     dwMilliseconds, dwWakeMask);
}
# endif

# ifndef FEAT_CLIENTSERVER
    സ്ഥിതം DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    എങ്കിൽ (read_console_നിവേശം(ശൂന്യ, ശൂന്യ, -2, ശൂന്യ))
	തിരികെ WAIT_OBJECT_0;
    തിരികെ WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#endif   // !FEAT_GUI_MSWIN || VIMDLL

    ശൂന്യം
mch_get_exe_name(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		updated = FALSE;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	enc_prev = -1;
    WCHAR	*path;
    size_t	pathlen;

    എങ്കിൽ (exe_name == ശൂന്യ || exe_pathw == ശൂന്യ || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_PATH];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(ശൂന്യ, buf, MAX_PATH);
	എങ്കിൽ (p > buf)
	{
	    എങ്കിൽ (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_to_enc(buf, ശൂന്യ);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    ചെയ്യുക
	    {
		--p;
		എങ്കിൽ (*p == L'\\')
		{
		    *p = L'\0';
		    നിർത്തുക;
		}
	    } ആകുമ്പോൾ (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    എങ്കിൽ (exe_pathw == ശൂന്യ || !updated)
	തിരികെ;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    path = _wgetenv(L"PATH");
    pathlen = (path == ശൂന്യ) ? 0 : wcslen(path);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#define EXTRA_LEN 7
#define MAX_ENV_STRING_LEN 8191

    എങ്കിൽ (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   temp[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	എങ്കിൽ (pathlen == 0)
	    wcscpy(temp + templen, exe_pathw);
	അല്ലെങ്കിൽ
	{
	    wcscpy(temp + templen, path);

	    // Check if exe_path is already included in $PATH.
	    എങ്കിൽ (wcsstr(temp, exe_pathw) == ശൂന്യ)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		എങ്കിൽ (temp[templen - 1] != L';')
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
    സ്ഥിതം ശൂന്യം
unescape_shellxquote(char_u *p, char_u *escaped)
{
    പൂർണ്ണസംഖ്യ	    l = (പൂർണ്ണസംഖ്യ)STRLEN(p);
    പൂർണ്ണസംഖ്യ	    n;

    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (*p == '^' && vim_strchr(escaped, p[1]) != ശൂന്യ)
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
vimLoadLib(സ്ഥിരം അക്ഷരം *name)
{
    HINSTANCE	dll = ശൂന്യ;

    // No need to load any library when registering OLE.
    എങ്കിൽ (found_register_arg)
	തിരികെ ശൂന്യ;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    എങ്കിൽ (exe_pathw == ശൂന്യ)
    {
	mch_get_exe_name();
	എങ്കിൽ (exe_pathw == ശൂന്യ)
	    തിരികെ ശൂന്യ;
    }

    WCHAR old_dirw[MAXPATHL];

    എങ്കിൽ (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	തിരികെ ശൂന്യ;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    SetCurrentDirectoryW(exe_pathw);
    dll = LoadLibrary(name);
    SetCurrentDirectoryW(old_dirw);
    തിരികെ dll;
}

#എങ്കിൽ defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    പൂർണ്ണസംഖ്യ
mch_is_gui_executable(ശൂന്യം)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(ശൂന്യ);
    PIMAGE_DOS_HEADER	pDOS = (PIMAGE_DOS_HEADER)pImage;
    PIMAGE_NT_HEADERS	pPE;

    എങ്കിൽ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	തിരികെ FALSE;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    എങ്കിൽ (pPE->Signature != IMAGE_NT_SIGNATURE)
	തിരികെ FALSE;
    എങ്കിൽ (pPE->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	തിരികെ TRUE;
    തിരികെ FALSE;
}
#endif

#എങ്കിൽ defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    സ്ഥിതം ശൂന്യം *
get_imported_func_info(HINSTANCE hInst, സ്ഥിരം അക്ഷരം *funcname, പൂർണ്ണസംഖ്യ info,
	സ്ഥിരം ശൂന്യം *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_HEADER		pDOS = (PIMAGE_DOS_HEADER)hInst;
    PIMAGE_NT_HEADERS		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_DATA		pIAT;	    // Import Address Table
    PIMAGE_THUNK_DATA		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    എങ്കിൽ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	തിരികെ ശൂന്യ;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    എങ്കിൽ (pPE->Signature != IMAGE_NT_SIGNATURE)
	തിരികെ ശൂന്യ;

    ImpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    എങ്കിൽ (ImpVA == 0)
	തിരികെ ശൂന്യ;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    വേണ്ടി (; pImpDesc->FirstThunk; ++pImpDesc)
    {
	എങ്കിൽ (!pImpDesc->OriginalFirstThunk)
	    തുടരുക;
	pIAT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->FirstThunk);
	pINT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->OriginalFirstThunk);
	വേണ്ടി (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    എങ്കിൽ (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		തുടരുക;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.AddressOfData));
	    എങ്കിൽ (strcmp((അക്ഷരം *)pImpName->Name, funcname) == 0)
	    {
		ശൂന്യം *original;
		DWORD old, new = PAGE_READWRITE;

		തിരഞ്ഞെടുപ്പ് (info)
		{
		    അവസ്ഥ 0:
			തിരികെ (ശൂന്യം *)pIAT->u1.Function;
		    അവസ്ഥ 1:
			തിരികെ (ശൂന്യം *)(pImage + pImpDesc->Name);
		    അവസ്ഥ 2:
			original = (ശൂന്യം *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, വലുപ്പം(ശൂന്യം *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, വലുപ്പം(ശൂന്യം *),
				old, &new);
			തിരികെ original;
		    സ്ഥിരസ്ഥിതി:
			തിരികെ ശൂന്യ;
		}
	    }
	}
    }
    തിരികെ ശൂന്യ;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
find_imported_module_by_funcname(HINSTANCE hInst, സ്ഥിരം അക്ഷരം *funcname)
{
    അക്ഷരം    *modulename;

    modulename = (അക്ഷരം *)get_imported_func_info(hInst, funcname, 1, ശൂന്യ);
    എങ്കിൽ (modulename != ശൂന്യ)
	തിരികെ GetModuleHandleA(modulename);
    തിരികെ ശൂന്യ;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    ശൂന്യം *
get_dll_import_func(HINSTANCE hInst, സ്ഥിരം അക്ഷരം *funcname)
{
    തിരികെ get_imported_func_info(hInst, funcname, 0, ശൂന്യ);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    ശൂന്യം *
hook_dll_import_func(HINSTANCE hInst, സ്ഥിരം അക്ഷരം *funcname, സ്ഥിരം ശൂന്യം *hook)
{
    തിരികെ get_imported_func_info(hInst, funcname, 2, hook);
}
#endif

#എങ്കിൽ defined(FEAT_PYTHON3)
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

    എങ്കിൽ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	തിരികെ ശൂന്യ;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    എങ്കിൽ (pPE->Signature != IMAGE_NT_SIGNATURE)
	തിരികെ ശൂന്യ;

    ExpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    ExpSize = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    എങ്കിൽ (ExpVA == 0)
	തിരികെ hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    pFunctionTable = (LPDWORD)(pImage + pExpDir->AddressOfFunctions);

    എങ്കിൽ (pExpDir->സംഖ്യ_Of_Names == 0)
	തിരികെ hInst;	// No export names.

    // Check only the first entry.
    എങ്കിൽ ((pFunctionTable[0] < ExpVA) || (pFunctionTable[0] >= ExpVA + ExpSize))
	// The first entry is not a function forwarder.
	തിരികെ hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    സ്ഥിരം അക്ഷരം *name = (സ്ഥിരം അക്ഷരം *)(pImage + pFunctionTable[0]);
    സ്ഥിരം അക്ഷരം *p = strchr(name, '.');
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ hInst;

    // Extract DllName.
    അക്ഷരം buf[MAX_PATH];
    എങ്കിൽ (p - name + 1 > വലുപ്പം(buf))
	തിരികെ ശൂന്യ;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    തിരികെ GetModuleHandleA(buf);
}
#endif

#എങ്കിൽ defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
സ്ഥിതം അക്ഷരം *null_libintl_gettext(സ്ഥിരം അക്ഷരം *);
സ്ഥിതം അക്ഷരം *null_libintl_ngettext(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *, unsigned long n);
സ്ഥിതം അക്ഷരം *null_libintl_textdomain(സ്ഥിരം അക്ഷരം *);
സ്ഥിതം അക്ഷരം *null_libintl_bindtextdomain(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം അക്ഷരം *null_libintl_bind_textdomain_codeset(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ null_libintl_wputenv(സ്ഥിരം wchar_t *);

സ്ഥിതം HINSTANCE hLibintlDLL = ശൂന്യ;
അക്ഷരം *(*dyn_libintl_gettext)(സ്ഥിരം അക്ഷരം *) = null_libintl_gettext;
അക്ഷരം *(*dyn_libintl_ngettext)(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *, unsigned long n)
						= null_libintl_ngettext;
അക്ഷരം *(*dyn_libintl_textdomain)(സ്ഥിരം അക്ഷരം *) = null_libintl_textdomain;
അക്ഷരം *(*dyn_libintl_bindtextdomain)(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *)
						= null_libintl_bindtextdomain;
അക്ഷരം *(*dyn_libintl_bind_textdomain_codeset)(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *)
				       = null_libintl_bind_textdomain_codeset;
പൂർണ്ണസംഖ്യ (*dyn_libintl_wputenv)(സ്ഥിരം wchar_t *) = null_libintl_wputenv;
പൂർണ്ണസംഖ്യ *dyn_libintl_nl_സന്ദേശം_cat_cntr = ശൂന്യ;

    പൂർണ്ണസംഖ്യ
dyn_libintl_init(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;
    സ്ഥിതം ഘടന
    {
	അക്ഷരം	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{ശൂന്യ, ശൂന്യ}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    എങ്കിൽ (hLibintlDLL != ശൂന്യ)
	തിരികെ 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vimLoadLib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    എങ്കിൽ (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    എങ്കിൽ (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT2);
# endif
    എങ്കിൽ (!hLibintlDLL)
    {
	എങ്കിൽ (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_load_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	തിരികെ 0;
    }
    വേണ്ടി (i = 0; libintl_entry[i].name != ശൂന്യ
					 && libintl_entry[i].ptr != ശൂന്യ; ++i)
    {
	എങ്കിൽ ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == ശൂന്യ)
	{
	    dyn_libintl_end();
	    എങ്കിൽ (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_load_library_function_str), libintl_entry[i].name);
		verbose_leave();
	    }
	    തിരികെ 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (അക്ഷരം *(*)(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *))
			GetProcAddress(hLibintlDLL, "bind_textdomain_codeset");
    എങ്കിൽ (dyn_libintl_bind_textdomain_codeset == ശൂന്യ)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = find_imported_module_by_funcname(hLibintlDLL, "getenv");
    എങ്കിൽ (hmsvcrt != ശൂന്യ)
	dyn_libintl_wputenv = (പൂർണ്ണസംഖ്യ (*)(സ്ഥിരം wchar_t *))
					GetProcAddress(hmsvcrt, "_wputenv");
    എങ്കിൽ (dyn_libintl_wputenv == ശൂന്യ || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_സന്ദേശം_cat_cntr = (പൂർണ്ണസംഖ്യ *)GetProcAddress(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    തിരികെ 1;
}

    ശൂന്യം
dyn_libintl_end(ശൂന്യം)
{
    എങ്കിൽ (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ശൂന്യ;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_സന്ദേശം_cat_cntr	= ശൂന്യ;
}

    സ്ഥിതം അക്ഷരം *
null_libintl_gettext(സ്ഥിരം അക്ഷരം *msgid)
{
    തിരികെ (അക്ഷരം*)msgid;
}

    സ്ഥിതം അക്ഷരം *
null_libintl_ngettext(
	സ്ഥിരം അക്ഷരം *msgid,
	സ്ഥിരം അക്ഷരം *msgid_plural,
	unsigned long n)
{
    തിരികെ (അക്ഷരം *)(n == 1 ? msgid : msgid_plural);
}

    സ്ഥിതം അക്ഷരം *
null_libintl_bindtextdomain(
	സ്ഥിരം അക്ഷരം *domainname UNUSED,
	സ്ഥിരം അക്ഷരം *dirname UNUSED)
{
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം അക്ഷരം *
null_libintl_bind_textdomain_codeset(
	സ്ഥിരം അക്ഷരം *domainname UNUSED,
	സ്ഥിരം അക്ഷരം *codeset UNUSED)
{
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം അക്ഷരം *
null_libintl_textdomain(സ്ഥിരം അക്ഷരം *domainname UNUSED)
{
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
null_libintl_wputenv(സ്ഥിരം wchar_t *envstring UNUSED)
{
    തിരികെ 0;
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
    സ്ഥിതം BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    എങ്കിൽ (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	തിരികെ FALSE;

    എങ്കിൽ (!Lookup_Privilege_മൂല്യം(ശൂന്യ, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	തിരികെ FALSE;
    }

    tokenPrivileges.Privilege_എണ്ണം	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    എങ്കിൽ (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, ശൂന്യ, ശൂന്യ))
    {
	CloseHandle(hToken);
	തിരികെ FALSE;
    }

    എങ്കിൽ (GetLastError() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	തിരികെ FALSE;
    }

    CloseHandle(hToken);
    തിരികെ TRUE;
}
#endif

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    ശൂന്യം
PlatformId(ശൂന്യം)
{
#ifdef FEAT_EVAL
    DWORD major = (win_version >> 24) & 0xFF;
    DWORD minor = (win_version >> 16) & 0xFF;
    vim_snprintf(windowsVersion, വലുപ്പം(windowsVersion), "%d.%d", major, minor);
#endif

#ifdef HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    എങ്കിൽ (!win32_enable_privilege(SE_SECURITY_NAME))
	തിരികെ;
#endif
}

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SHIFT  (SHIFT_PRESSED)
# define CTRL   (വലത്_CTRL_PRESSED | ഇടത്_CTRL_PRESSED)
# define ALT    (വലത്_ALT_PRESSED  | ഇടത്_ALT_PRESSED)
# define ALT_GR (വലത്_ALT_PRESSED  | ഇടത്_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

സ്ഥിതം സ്ഥിരം ഘടന
{
    WORD    w_Virt_താക്കോൽ;
    BOOL    f_Ansi_താക്കോൽ;
    പൂർണ്ണസംഖ്യ	    chAlone;
    പൂർണ്ണസംഖ്യ	    chShift;
    പൂർണ്ണസംഖ്യ	    chCtrl;
    പൂർണ്ണസംഖ്യ	    chAlt;
} Virt_താക്കോൽ_Map[] =
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
    { VK_ഇടത്,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_വലത്,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_END,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_DOWN,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_NEXT,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_തിരുകുക,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_BACK,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# എങ്കിൽ 0
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
win32_kbd_patch_താക്കോൽ(
    താക്കോൽ_EVENT_RECORD *pker)
{
    UINT uMods = pker->dw_Control_താക്കോൽ_State;
    സ്ഥിതം പൂർണ്ണസംഖ്യ s_iIsDead = 0;
    സ്ഥിതം WORD awAnsiCode[2];
    സ്ഥിതം BYTE abKeystate[256];

    എങ്കിൽ (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[1];
	s_iIsDead = 0;
	തിരികെ 1;
    }

    // check if it already has a valid unicode character.
    എങ്കിൽ (pker->uChar.UnicodeChar != 0)
	തിരികെ 1;

    CLEAR_FIELD(abKeystate);

    // Clear any pending dead keys
    ToUnicode(VK_SPACE, Map_Virtual_താക്കോൽ(VK_SPACE, 0), abKeystate, awAnsiCode, 2, 0);

    എങ്കിൽ (uMods & SHIFT_PRESSED)
	abKeystate[VK_SHIFT] = 0x80;
    എങ്കിൽ (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    എങ്കിൽ ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ToUnicode(pker->w_Virtual_താക്കോൽ_Code, pker->wVirtualScanCode,
			abKeystate, awAnsiCode, 2, 0);

    എങ്കിൽ (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[0];

    തിരികെ s_iIsDead;
}

സ്ഥിതം BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    സ്ഥിതം BOOL
decode_താക്കോൽ_event(
    താക്കോൽ_EVENT_RECORD	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    പൂർണ്ണസംഖ്യ			*pmodifiers,
    BOOL		fDoPost UNUSED)
{
    പൂർണ്ണസംഖ്യ i;
    സ്ഥിരം പൂർണ്ണസംഖ്യ nModifs = pker->dw_Control_താക്കോൽ_State & (SHIFT | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    എങ്കിൽ (!pker->b_താക്കോൽ_Down)
	തിരികെ FALSE;

    // ignore some keystrokes
    തിരഞ്ഞെടുപ്പ് (pker->w_Virtual_താക്കോൽ_Code)
    {
    // modifiers
    അവസ്ഥ VK_SHIFT:
    അവസ്ഥ VK_CONTROL:
    അവസ്ഥ VK_MENU:   // Alt key
	തിരികെ FALSE;

    സ്ഥിരസ്ഥിതി:
	നിർത്തുക;
    }

    // Shift-TAB
    എങ്കിൽ (pker->w_Virtual_താക്കോൽ_Code == VK_TAB && (nModifs & SHIFT_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	തിരികെ TRUE;
    }

    വേണ്ടി (i = ARRAY_LENGTH(Virt_താക്കോൽ_Map); --i >= 0; )
    {
	എങ്കിൽ (Virt_താക്കോൽ_Map[i].w_Virt_താക്കോൽ == pker->w_Virtual_താക്കോൽ_Code)
	{
	    *pch = Virt_താക്കോൽ_Map[i].chAlone;
	    എങ്കിൽ ((nModifs & SHIFT) != 0)
		*pch = Virt_താക്കോൽ_Map[i].chShift;
	    അല്ലെങ്കിൽ എങ്കിൽ ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_താക്കോൽ_Map[i].chCtrl;
	    അല്ലെങ്കിൽ എങ്കിൽ ((nModifs & ALT) != 0)
		*pch = Virt_താക്കോൽ_Map[i].chAlt;

	    എങ്കിൽ (*pch != 0)
	    {
		എങ്കിൽ (Virt_താക്കോൽ_Map[i].f_Ansi_താക്കോൽ)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    എങ്കിൽ (pmodifiers)
		    {
			എങ്കിൽ (pker->w_Virtual_താക്കോൽ_Code >= VK_F1
			    && pker->w_Virtual_താക്കോൽ_Code <= VK_F12)
			{
			    എങ്കിൽ ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_MASK_ALT;
				എങ്കിൽ ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_താക്കോൽ_Map[i].chAlone;
			    }
			    എങ്കിൽ ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				എങ്കിൽ ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_താക്കോൽ_Map[i].chAlone;
			    }
			}
			അല്ലെങ്കിൽ എങ്കിൽ (pker->w_Virtual_താക്കോൽ_Code >= VK_END
				&& pker->w_Virtual_താക്കോൽ_Code <= VK_DOWN)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_താക്കോൽ_Map[i].chAlone;
			    എങ്കിൽ ((nModifs & SHIFT) != 0
						    && (nModifs & ~SHIFT) == 0)
			    {
				*pch2 = Virt_താക്കോൽ_Map[i].chShift;
			    }
			    എങ്കിൽ ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_താക്കോൽ_Map[i].chCtrl;
				എങ്കിൽ (pker->w_Virtual_താക്കോൽ_Code == VK_UP
				    || pker->w_Virtual_താക്കോൽ_Code == VK_DOWN)
				{
				    *pmodifiers |= MOD_MASK_CTRL;
				    *pch2 = Virt_താക്കോൽ_Map[i].chAlone;
				}
			    }
			    എങ്കിൽ ((nModifs & SHIFT) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				*pch2 = Virt_താക്കോൽ_Map[i].chShift;
			    }
			    എങ്കിൽ ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_താക്കോൽ_Map[i].chAlt;
				*pmodifiers |= MOD_MASK_ALT;
				എങ്കിൽ ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_താക്കോൽ_Map[i].chAlone;
				}
				അല്ലെങ്കിൽ എങ്കിൽ ((nModifs & SHIFT) != 0)
				{
				    *pch2 = Virt_താക്കോൽ_Map[i].chShift;
				}
				അല്ലെങ്കിൽ എങ്കിൽ ((nModifs & CTRL) != 0)
				{
				    എങ്കിൽ (pker->w_Virtual_താക്കോൽ_Code == VK_UP
					|| pker->w_Virtual_താക്കോൽ_Code == VK_DOWN)
				    {
					*pmodifiers |= MOD_MASK_CTRL;
					*pch2 = Virt_താക്കോൽ_Map[i].chAlone;
				    }
				    അല്ലെങ്കിൽ
				    {
					*pch2 = Virt_താക്കോൽ_Map[i].chCtrl;
				    }
				}
			    }
			}
			അല്ലെങ്കിൽ എങ്കിൽ (pker->w_Virtual_താക്കോൽ_Code == VK_തിരുകുക
					&& (nModifs & SHIFT) != 0
					&& (nModifs & ~SHIFT) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_താക്കോൽ_Map[i].chShift;
			}
			അല്ലെങ്കിൽ
			{
			    *pch2 = Virt_താക്കോൽ_Map[i].chAlone;
			    എങ്കിൽ ((nModifs & SHIFT) != 0)
				*pmodifiers |= MOD_MASK_SHIFT;
			    എങ്കിൽ ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_MASK_CTRL;
			    എങ്കിൽ ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_MASK_ALT;
			}
		    }
		}

		തിരികെ TRUE;
	    }
	}
    }

    i = win32_kbd_patch_താക്കോൽ(pker);

    എങ്കിൽ (i < 0)
	*pch = NUL;
    അല്ലെങ്കിൽ
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	എങ്കിൽ (pmodifiers != ശൂന്യ)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    എങ്കിൽ ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_MASK_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    എങ്കിൽ ((nModifs & SHIFT) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_MASK_SHIFT;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    എങ്കിൽ ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_MASK_CTRL;
	}
    }

    തിരികെ (*pch != NUL);
}

# എങ്കിൽ defined(FEAT_EVAL)
    സ്ഥിതം പൂർണ്ണസംഖ്യ
encode_താക്കോൽ_event(dict_T *args, നിവേശം_RECORD *ir)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ s_dwMods = 0;

    char_u *action = dict_get_string(args, "event", TRUE);
    എങ്കിൽ (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_താക്കോൽ_Down = STRICMP(action, "keydown") == 0;
	WORD vkCode = dict_get_സംഖ്യ_def(args, "keycode", 0);
	എങ്കിൽ (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    തിരികെ FALSE;
	}

	ir->EventType = താക്കോൽ_EVENT;
	താക്കോൽ_EVENT_RECORD ker;
	ZeroMemory(&ker, വലുപ്പം(ker));
	ker.b_താക്കോൽ_Down = is_താക്കോൽ_Down;
	ker.w_Repeat_എണ്ണം = 1;
	ker.wVirtualScanCode = 0;
	ker.dw_Control_താക്കോൽ_State = 0;
	പൂർണ്ണസംഖ്യ mods = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	എങ്കിൽ (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    എങ്കിൽ (mods & MOD_MASK_SHIFT)
		ker.dw_Control_താക്കോൽ_State |= SHIFT_PRESSED;
	    എങ്കിൽ (mods & MOD_MASK_CTRL)
		ker.dw_Control_താക്കോൽ_State |= ഇടത്_CTRL_PRESSED;
	    എങ്കിൽ (mods & MOD_MASK_ALT)
		ker.dw_Control_താക്കോൽ_State |= ഇടത്_ALT_PRESSED;
	}

	എങ്കിൽ (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT)
	{
	    എങ്കിൽ (is_താക്കോൽ_Down)
		s_dwMods |= SHIFT_PRESSED;
	    അല്ലെങ്കിൽ
		s_dwMods &= ~SHIFT_PRESSED;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vkCode == VK_LCONTROL || vkCode == VK_CONTROL)
	{
	    എങ്കിൽ (is_താക്കോൽ_Down)
		s_dwMods |= ഇടത്_CTRL_PRESSED;
	    അല്ലെങ്കിൽ
		s_dwMods &= ~ഇടത്_CTRL_PRESSED;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vkCode == VK_RCONTROL)
	{
	    എങ്കിൽ (is_താക്കോൽ_Down)
		s_dwMods |= വലത്_CTRL_PRESSED;
	    അല്ലെങ്കിൽ
		s_dwMods &= ~വലത്_CTRL_PRESSED;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vkCode == VK_LMENU || vkCode == VK_MENU)
	{
	    എങ്കിൽ (is_താക്കോൽ_Down)
		s_dwMods |= ഇടത്_ALT_PRESSED;
	    അല്ലെങ്കിൽ
		s_dwMods &= ~ഇടത്_ALT_PRESSED;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vkCode == VK_RMENU)
	{
	    എങ്കിൽ (is_താക്കോൽ_Down)
		s_dwMods |= വലത്_ALT_PRESSED;
	    അല്ലെങ്കിൽ
		s_dwMods &= ~വലത്_ALT_PRESSED;
	}
	ker.dw_Control_താക്കോൽ_State |= s_dwMods;
	ker.w_Virtual_താക്കോൽ_Code = vkCode;
	ker.uChar.UnicodeChar = 0;
	ir->Event.താക്കോൽ_Event_2 = ker;
	vim_free(action);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (action == ശൂന്യ)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "event", action);
	    vim_free(action);
	}
	തിരികെ FALSE;
    }
    തിരികെ TRUE;
}
# endif  // FEAT_EVAL
#endif // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
സ്ഥിതം പൂർണ്ണസംഖ്യ g_fMouseAvail = FALSE;   // mouse present
സ്ഥിതം പൂർണ്ണസംഖ്യ g_fMouseActive = FALSE;  // mouse enabled
സ്ഥിതം പൂർണ്ണസംഖ്യ g_nMouseClick = -1;	    // mouse status
സ്ഥിതം പൂർണ്ണസംഖ്യ g_xMouse;		    // mouse x coordinate
സ്ഥിതം പൂർണ്ണസംഖ്യ g_yMouse;		    // mouse y coordinate
സ്ഥിതം DWORD g_cmodein = 0;	    // Original console input mode
സ്ഥിതം DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    ശൂന്യം
mch_setmouse(പൂർണ്ണസംഖ്യ on)
{
    DWORD cmodein;

# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
	തിരികെ;
# endif
    എങ്കിൽ (!g_fMouseAvail)
	തിരികെ;

    g_fMouseActive = on;
    GetConsoleMode(g_hConIn, &cmodein);

    എങ്കിൽ (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_നിവേശം;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    അല്ലെങ്കിൽ
    {
	cmodein &= ~ENABLE_MOUSE_നിവേശം;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }

    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
}


# എങ്കിൽ defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    ശൂന്യം
mch_bevalterm_changed(ശൂന്യം)
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
    സ്ഥിതം ശൂന്യം
decode_mouse_wheel(MOUSE_EVENT_RECORD *pmer)
{
    പൂർണ്ണസംഖ്യ	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    പൂർണ്ണസംഖ്യ	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    പൂർണ്ണസംഖ്യ lcol = g_xMouse;
    പൂർണ്ണസംഖ്യ lrow = g_yMouse;
    win_T *wp = mouse_find_win(&lrow, &lcol, FIND_POPUP);
    എങ്കിൽ (wp != ശൂന്യ && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_FIELD(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	എങ്കിൽ (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ഇടത് : MSCR_വലത്;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	അല്ലെങ്കിൽ
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
    എങ്കിൽ (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    അല്ലെങ്കിൽ
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    എങ്കിൽ (pmer->dw_Control_താക്കോൽ_State & SHIFT_PRESSED)
	modifiers |= MOD_MASK_SHIFT; // MOUSE_SHIFT;
    എങ്കിൽ (pmer->dw_Control_താക്കോൽ_State & (വലത്_CTRL_PRESSED | ഇടത്_CTRL_PRESSED))
	modifiers |= MOD_MASK_CTRL; // MOUSE_CTRL;
    എങ്കിൽ (pmer->dw_Control_താക്കോൽ_State & (വലത്_ALT_PRESSED  | ഇടത്_ALT_PRESSED))
	modifiers |= MOD_MASK_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    തിരികെ;
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
    സ്ഥിതം BOOL
decode_mouse_event(
    MOUSE_EVENT_RECORD *pmer)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ s_nOldButton = -1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ s_nOldMouseClick = -1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ s_xOldMouse = -1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ s_yOldMouse = -1;
    സ്ഥിതം linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    സ്ഥിതം പൂർണ്ണസംഖ്യ s_old_topfill = 0;
# endif
    സ്ഥിതം പൂർണ്ണസംഖ്യ s_cClicks = 1;
    സ്ഥിതം BOOL s_fReleased = TRUE;
    സ്ഥിതം DWORD s_dwLastClickTime = 0;
    സ്ഥിതം BOOL s_f_Next_Is_നടുവ് = FALSE;

    സ്ഥിതം DWORD cButtons = 0;	// number of buttons supported

    സ്ഥിരം DWORD ഇടത്_2 = FROM_ഇടത്_1_ST_BUTTON_PRESSED;
    സ്ഥിരം DWORD നടുവ് = FROM_ഇടത്_2_ND_BUTTON_PRESSED;
    സ്ഥിരം DWORD വലത്_2 = RIGHTMOST_BUTTON_PRESSED;
    സ്ഥിരം DWORD ഇടത്_വലത് = ഇടത്_2 | വലത്_2;

    പൂർണ്ണസംഖ്യ nButton;

    എങ്കിൽ (cButtons == 0 && !Get_സംഖ്യ_Of_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    എങ്കിൽ (!g_fMouseAvail || !g_fMouseActive)
    {
	g_nMouseClick = -1;
	തിരികെ FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    എങ്കിൽ (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	തിരികെ FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    എങ്കിൽ (g_nMouseClick != -1)
	തിരികെ TRUE;

    എങ്കിൽ (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	തിരികെ TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    എങ്കിൽ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	എങ്കിൽ (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    തിരികെ FALSE;
    }

    // If no buttons are pressed...
    എങ്കിൽ ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	എങ്കിൽ (s_fReleased)
	{
# ifdef FEAT_BEVAL_TERM
	    // do return mouse move events when we want them
	    എങ്കിൽ (p_bevalterm)
		nButton = MOUSE_DRAG;
	    അല്ലെങ്കിൽ
# endif
		തിരികെ FALSE;
	}

	s_fReleased = TRUE;
    }
    അല്ലെങ്കിൽ    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	എങ്കിൽ (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & ഇടത്_വലത്);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    എങ്കിൽ (dwLR == ഇടത്_2 || dwLR == വലത്_2)
	    {
		വേണ്ടി (;;)
		{
		    // wait a short time for next input event
		    എങ്കിൽ (WaitForSingleObject(g_hConIn, p_mouset / 3)
							     != WAIT_OBJECT_0)
			നിർത്തുക;
		    അല്ലെങ്കിൽ
		    {
			DWORD cRecords = 0;
			നിവേശം_RECORD ir;
			MOUSE_EVENT_RECORD* pmer2 = &ir.Event.MouseEvent;

			peek_console_നിവേശം(g_hConIn, &ir, 1, &cRecords);

			എങ്കിൽ (cRecords == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & ഇടത്_വലത്))
			    നിർത്തുക;
			അല്ലെങ്കിൽ
			{
			    എങ്കിൽ (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_നിവേശം(g_hConIn, &ir, 1, &cRecords);

				തിരികെ decode_mouse_event(pmer2);
			    }
			    അല്ലെങ്കിൽ എങ്കിൽ (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_നിവേശം(g_hConIn, &ir, 1, &cRecords);

				// are there any more mouse events in queue?
				peek_console_നിവേശം(g_hConIn, &ir, 1, &cRecords);

				എങ്കിൽ (cRecords==0 || ir.EventType != MOUSE_EVENT)
				    നിർത്തുക;
			    }
			    അല്ലെങ്കിൽ
				നിർത്തുക;
			}
		    }
		}
	    }
	}

	എങ്കിൽ (s_f_Next_Is_നടുവ്)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_നടുവ്;
	    s_f_Next_Is_നടുവ് = FALSE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cButtons == 2	&&
	    ((pmer->dwButtonState & ഇടത്_വലത്) == ഇടത്_വലത്))
	{
	    nButton = MOUSE_നടുവ്;

	    എങ്കിൽ (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_Next_Is_നടുവ് = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((pmer->dwButtonState & ഇടത്_2) == ഇടത്_2)
	    nButton = MOUSE_ഇടത്;
	അല്ലെങ്കിൽ എങ്കിൽ ((pmer->dwButtonState & നടുവ്) == നടുവ്)
	    nButton = MOUSE_നടുവ്;
	അല്ലെങ്കിൽ എങ്കിൽ ((pmer->dwButtonState & വലത്_2) == വലത്_2)
	    nButton = MOUSE_വലത്;

	എങ്കിൽ (! s_fReleased && ! s_f_Next_Is_നടുവ്
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    തിരികെ FALSE;

	s_fReleased = s_f_Next_Is_നടുവ്;
    }

    എങ്കിൽ (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	എങ്കിൽ (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dwCurrentTime = Get_Tick_എണ്ണം();

	    എങ്കിൽ (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (പൂർണ്ണസംഖ്യ)(dwCurrentTime - s_dwLastClickTime) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dwLastClickTime = dwCurrentTime;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	എങ്കിൽ (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    എങ്കിൽ (nButton == -1)
	തിരികെ FALSE;

    എങ്കിൽ (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    എങ്കിൽ (pmer->dw_Control_താക്കോൽ_State & SHIFT_PRESSED)
	g_nMouseClick |= MOUSE_SHIFT;
    എങ്കിൽ (pmer->dw_Control_താക്കോൽ_State & (വലത്_CTRL_PRESSED | ഇടത്_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    എങ്കിൽ (pmer->dw_Control_താക്കോൽ_State & (വലത്_ALT_PRESSED  | ഇടത്_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    എങ്കിൽ (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	SET_അക്കം_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    എങ്കിൽ (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	തിരികെ FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    തിരികെ TRUE;
}

# ifdef FEAT_EVAL
    സ്ഥിതം പൂർണ്ണസംഖ്യ
encode_mouse_event(dict_T *args, നിവേശം_RECORD *ir)
{
    പൂർണ്ണസംഖ്യ		button;
    പൂർണ്ണസംഖ്യ		row;
    പൂർണ്ണസംഖ്യ		col;
    പൂർണ്ണസംഖ്യ		repeated_click;
    int_u	mods = 0;
    പൂർണ്ണസംഖ്യ		move;

    എങ്കിൽ (!dict_has_താക്കോൽ(args, "row") || !dict_has_താക്കോൽ(args, "col"))
	തിരികെ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (പൂർണ്ണസംഖ്യ)dict_get_bool(args, "move", FALSE);
    എങ്കിൽ (!move && (!dict_has_താക്കോൽ(args, "button")
	    || !dict_has_താക്കോൽ(args, "multiclick")
	    || !dict_has_താക്കോൽ(args, "modifiers")))
	തിരികെ FALSE;

    row = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "row") - 1;
    col = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_RECORD mer;
    ZeroMemory(&mer, വലുപ്പം(mer));
    mer.dwMousePosition.X  = col;
    mer.dwMousePosition.Y  = row;

    എങ്കിൽ (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    അല്ലെങ്കിൽ
    {
	button = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "button");
	repeated_click = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "multiclick");
	mods = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	തിരഞ്ഞെടുപ്പ് (button)
	{
	    അവസ്ഥ MOUSE_ഇടത്:
		mer.dwButtonState = FROM_ഇടത്_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		നിർത്തുക;
	    അവസ്ഥ MOUSE_നടുവ്:
		mer.dwButtonState = FROM_ഇടത്_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		നിർത്തുക;
	    അവസ്ഥ MOUSE_വലത്:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		നിർത്തുക;
	    അവസ്ഥ MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    അവസ്ഥ MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		നിർത്തുക;
	    അവസ്ഥ MOUSE_X1:
		mer.dwButtonState = FROM_ഇടത്_3_RD_BUTTON_PRESSED;
		നിർത്തുക;
	    അവസ്ഥ MOUSE_X2:
		mer.dwButtonState = FROM_ഇടത്_4_TH_BUTTON_PRESSED;
		നിർത്തുക;
	    അവസ്ഥ MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		നിർത്തുക;
	    അവസ്ഥ MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		നിർത്തുക;
	    അവസ്ഥ MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		നിർത്തുക;
	    അവസ്ഥ MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		semsg(_(e_invalid_argument_str), "button");
		തിരികെ FALSE;
	}
    }

    mer.dw_Control_താക്കോൽ_State = 0;
    എങ്കിൽ (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	എങ്കിൽ (mods & MOUSE_SHIFT)
	    mer.dw_Control_താക്കോൽ_State |= SHIFT_PRESSED;
	എങ്കിൽ (mods & MOUSE_CTRL)
	    mer.dw_Control_താക്കോൽ_State |= ഇടത്_CTRL_PRESSED;
	എങ്കിൽ (mods & MOUSE_ALT)
	    mer.dw_Control_താക്കോൽ_State |= ഇടത്_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    തിരികെ TRUE;
}
# endif  // FEAT_EVAL

    സ്ഥിതം പൂർണ്ണസംഖ്യ
write_നിവേശം_record_buffer(നിവേശം_RECORD* irEvents, പൂർണ്ണസംഖ്യ nLength)
{
    പൂർണ്ണസംഖ്യ				n_എണ്ണം = 0;
    നിവേശം_record_buffer_കണ്ണി_T	*event_കണ്ണി;

    ആകുമ്പോൾ (n_എണ്ണം < nLength)
    {
	event_കണ്ണി = alloc(വലുപ്പം(നിവേശം_record_buffer_കണ്ണി_T));
	എങ്കിൽ (event_കണ്ണി == ശൂന്യ)
	    തിരികെ -1;

	event_കണ്ണി->ir = irEvents[n_എണ്ണം++];
	event_കണ്ണി->next = ശൂന്യ;

	നിവേശം_record_buffer.length++;
	എങ്കിൽ (നിവേശം_record_buffer.tail == ശൂന്യ)
	{
	    നിവേശം_record_buffer.head = event_കണ്ണി;
	    നിവേശം_record_buffer.tail = event_കണ്ണി;
	}
	അല്ലെങ്കിൽ
	{
	    നിവേശം_record_buffer.tail->next = event_കണ്ണി;
	    നിവേശം_record_buffer.tail = നിവേശം_record_buffer.tail->next;
	}
    }
    തിരികെ n_എണ്ണം;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_നിവേശം_record_buffer(നിവേശം_RECORD* irEvents, പൂർണ്ണസംഖ്യ nMaxLength)
{
    പൂർണ്ണസംഖ്യ n_എണ്ണം = 0;
    ആകുമ്പോൾ (n_എണ്ണം < nMaxLength && നിവേശം_record_buffer.head != ശൂന്യ)
    {
	നിവേശം_record_buffer.length--;
	നിവേശം_record_buffer_കണ്ണി_T *എടുക്കുക_head = നിവേശം_record_buffer.head;
	irEvents[n_എണ്ണം++] = എടുക്കുക_head->ir;
	നിവേശം_record_buffer.head = എടുക്കുക_head->next;
	vim_free(എടുക്കുക_head);
	എങ്കിൽ (നിവേശം_record_buffer.length == 0)
	    നിവേശം_record_buffer.tail = ശൂന്യ;
    }
    തിരികെ n_എണ്ണം;
}
#അല്ലെങ്കിൽ  // FEAT_GUI_MSWIN && !VIMDLL
    ശൂന്യം
mch_setmouse(പൂർണ്ണസംഖ്യ on UNUSED)
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
    പൂർണ്ണസംഖ്യ
test_mswin_event(char_u *event, dict_T *args)
{
    പൂർണ്ണസംഖ്യ lpEventsWritten = 0;

# എങ്കിൽ defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    എങ്കിൽ (gui.in_use)
	തിരികെ test_gui_w32_sendevent(event, args);
# endif

# എങ്കിൽ defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    നിവേശം_RECORD ir;
    BOOL നിവേശം_encoded = FALSE;
    BOOL execute = FALSE;
    എങ്കിൽ (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(args, "execute", FALSE);
	എങ്കിൽ (dict_has_താക്കോൽ(args, "event"))
	    നിവേശം_encoded = encode_താക്കോൽ_event(args, &ir);
	അല്ലെങ്കിൽ എങ്കിൽ (!execute)
	{
	    semsg(_(e_missing_argument_str), "event");
	    തിരികെ FALSE;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	നിവേശം_encoded = encode_mouse_event(args, &ir);
    }
    അല്ലെങ്കിൽ
    {
	semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "event", event);
	തിരികെ FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    എങ്കിൽ (നിവേശം_encoded)
    {
	എങ്കിൽ ((lpEventsWritten = write_നിവേശം_record_buffer(&ir, 1)) < 0)
	{
	    semsg(_(e_out_of_memory), "event");
	    തിരികെ FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    എങ്കിൽ (execute)
    {
	പൂർണ്ണസംഖ്യ save_സന്ദേശം_scroll = സന്ദേശം_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	സന്ദേശം_scroll = FALSE;
	ch_log(ശൂന്യ, "test_mswin_event() executing");
	exec_normal(TRUE, TRUE, TRUE);
	സന്ദേശം_scroll |= save_സന്ദേശം_scroll;
    }

# endif
    തിരികെ lpEventsWritten;
}
#endif // FEAT_EVAL

#എങ്കിൽ defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    സ്ഥിതം ശൂന്യം
mch_set_cursor_shape(പൂർണ്ണസംഖ്യ thickness)
{
    എങ്കിൽ (vtp_working)
    {
	എങ്കിൽ (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    എങ്കിൽ (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    അല്ലെങ്കിൽ
		vtp_printf("\033[0 q");	// default
	}
    }
    അല്ലെങ്കിൽ
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dwSize = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	SetConsoleCursorInfo(g_hConOut, &ConsoleCursorInfo);
	എങ്കിൽ (s_cursor_visible)
	    SetConsoleCursorPosition(g_hConOut, g_coord);
    }
}

    ശൂന്യം
mch_update_cursor(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		thickness;

# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
	തിരികെ;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    എങ്കിൽ (shape_table[idx].shape == SHAPE_BLOCK)
	thickness = 99;	// 100 doesn't work on W95
    അല്ലെങ്കിൽ
	thickness = shape_table[idx].percentage;
    mch_set_cursor_shape(thickness);
}
#endif

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    സ്ഥിതം ശൂന്യം
handle_focus_event(നിവേശം_RECORD ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
    ui_focus_change((പൂർണ്ണസംഖ്യ)g_fJustGotFocus);
}

സ്ഥിതം ശൂന്യം ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
WaitForChar(long msec, പൂർണ്ണസംഖ്യ ignore_നിവേശം)
{
    DWORD	    dwNow = 0, dwEndTime = 0;
    നിവേശം_RECORD    ir;
    DWORD	    cRecords;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    പൂർണ്ണസംഖ്യ		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    എങ്കിൽ (msec > 0)
	// Wait until the specified time has elapsed.
	dwEndTime = Get_Tick_എണ്ണം() + msec;
    അല്ലെങ്കിൽ എങ്കിൽ (msec < 0)
	// Wait forever.
	dwEndTime = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    വേണ്ടി (;;)
    {
	// Only process messages when waiting.
	എങ്കിൽ (msec != 0)
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

	എങ്കിൽ (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_നിവേശം && നിവേശം_available())
# endif
	   )
	    തിരികെ TRUE;

	എങ്കിൽ (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_എണ്ണം();
	    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)(dwNow - dwEndTime) >= 0)
		നിർത്തുക;
	}
	എങ്കിൽ (msec != 0)
	{
	    DWORD dwWaitTime = dwEndTime - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    എങ്കിൽ (dwWaitTime > 11)
		dwWaitTime = 11;

# ifdef FEAT_MZSCHEME
	    എങ്കിൽ (mzthreads_allowed() && p_mzq > 0 && (long)dwWaitTime > p_mzq)
		dwWaitTime = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    എങ്കിൽ (dwWaitTime > 10)
	    {
		long	due_time;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_time = check_due_timer();
		എങ്കിൽ (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    തിരികെ FALSE;
		}
		എങ്കിൽ (due_time > 0 && dwWaitTime > (DWORD)due_time)
		    dwWaitTime = due_time;
	    }
# endif
	    എങ്കിൽ (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    സന്ദേശം_wait_for_multiple_objects(1, &g_hConIn, FALSE,
				  dwWaitTime, QS_SENDMESSAGE) != WAIT_OBJECT_0
# അല്ലെങ്കിൽ
		    wait_for_single_object(g_hConIn, dwWaitTime)
							      != WAIT_OBJECT_0
# endif
		    )
		തുടരുക;
	}

	cRecords = 0;
	peek_console_നിവേശം(g_hConIn, &ir, 1, &cRecords);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	എങ്കിൽ (State == MODE_CMDLINE && സന്ദേശം_row == Rows - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_BUFFER_INFO csbi;

	    എങ്കിൽ (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	    {
		എങ്കിൽ (csbi.dwCursorPosition.Y != സന്ദേശം_row)
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

	എങ്കിൽ (cRecords > 0)
	{
	    എങ്കിൽ (ir.EventType == താക്കോൽ_EVENT && ir.Event.താക്കോൽ_Event_2.b_താക്കോൽ_Down)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		എങ്കിൽ (ir.Event.താക്കോൽ_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.താക്കോൽ_Event_2.w_Virtual_താക്കോൽ_Code == 13)
		{
		    read_console_നിവേശം(g_hConIn, &ir, 1, &cRecords);
		    തുടരുക;
		}
# endif
		എങ്കിൽ (decode_താക്കോൽ_event(&ir.Event.താക്കോൽ_Event_2, &ch, &ch2,
								 ശൂന്യ, FALSE))
		    തിരികെ TRUE;
	    }

	    read_console_നിവേശം(g_hConIn, &ir, 1, &cRecords);

	    എങ്കിൽ (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    അല്ലെങ്കിൽ എങ്കിൽ (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    {
		COORD dwSize = ir.Event.WindowBufferSizeEvent.dwSize;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		എങ്കിൽ (dwSize.X != Columns || dwSize.Y != Rows)
		{
		    CONSOLE_SCREEN_BUFFER_INFO csbi;
		    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
		    dwSize.X = csbi.srWindow.വലത്_3 - csbi.srWindow.ഇടത്_3 + 1;
		    dwSize.Y = csbi.srWindow.Bottom - csbi.srWindow.മുകൾ_2 + 1;
		    എങ്കിൽ (dwSize.X != Columns || dwSize.Y != Rows)
		    {
			ResizeConBuf(g_hConOut, dwSize);
			shell_resized();
		    }
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		തിരികെ TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (msec == 0)
	    നിർത്തുക;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    എങ്കിൽ (നിവേശം_available())
	തിരികെ TRUE;
# endif

    തിരികെ FALSE;
}

/*
 * return non-zero if a character is available
 */
    പൂർണ്ണസംഖ്യ
mch_char_avail(ശൂന്യം)
{
# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
	തിരികെ TRUE;
# endif
    തിരികെ WaitForChar(0L, FALSE);
}

# എങ്കിൽ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    പൂർണ്ണസംഖ്യ
mch_check_messages(ശൂന്യം)
{
#  ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
	തിരികെ TRUE;
#  endif
    തിരികെ WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    സ്ഥിതം ശൂന്യം
create_conin(ശൂന്യം)
{
    g_hConIn =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES) ശൂന്യ,
			OPEN_EXISTING, 0, (HANDLE)ശൂന്യ);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    സ്ഥിതം WCHAR
tgetch(പൂർണ്ണസംഖ്യ *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    വേണ്ടി (;;)
    {
	നിവേശം_RECORD ir;
	DWORD cRecords = 0;

# ifdef FEAT_CLIENTSERVER
	(ശൂന്യം)WaitForChar(-1L, FALSE);
	എങ്കിൽ (നിവേശം_available())
	    തിരികെ 0;
	എങ്കിൽ (g_nMouseClick != -1)
	    തിരികെ 0;
# endif
	എങ്കിൽ (read_console_നിവേശം(g_hConIn, &ir, 1, &cRecords) == 0)
	{
	    എങ്കിൽ (did_create_conin)
		read_error_exit();
	    create_conin();
	    തുടരുക;
	}

	എങ്കിൽ (ir.EventType == താക്കോൽ_EVENT)
	{
	    എങ്കിൽ (decode_താക്കോൽ_event(&ir.Event.താക്കോൽ_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		തിരികെ ch;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	അല്ലെങ്കിൽ എങ്കിൽ (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    shell_resized();
	അല്ലെങ്കിൽ എങ്കിൽ (ir.EventType == MOUSE_EVENT)
	{
	    എങ്കിൽ (decode_mouse_event(&ir.Event.MouseEvent))
		തിരികെ 0;
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
    പൂർണ്ണസംഖ്യ
mch_inchar(
    char_u	*buf UNUSED,
    പൂർണ്ണസംഖ്യ		maxlen UNUSED,
    long	time UNUSED,
    പൂർണ്ണസംഖ്യ		tb_change_cnt UNUSED)
{
#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# അല്ലെങ്കിൽ
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    സ്ഥിതം char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    typeaheadlen = 0;

# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
	തിരികെ 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    എങ്കിൽ (typeaheadlen > 0)
	പോകുക theend;

    എങ്കിൽ (time >= 0)
    {
	എങ്കിൽ (!WaitForChar(time, FALSE))     // no character available
	    തിരികെ 0;
    }
    അല്ലെങ്കിൽ    // time == -1, wait forever
    {
	mch_set_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	എങ്കിൽ (!WaitForChar(p_ut, FALSE))
	{
	    എങ്കിൽ (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (പൂർണ്ണസംഖ്യ)KE_CURSORHOLD;
		തിരികെ 3;
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
    എങ്കിൽ (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    ആകുമ്പോൾ ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	എങ്കിൽ (typebuf_changed(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    നിർത്തുക;
	}
	എങ്കിൽ (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    എങ്കിൽ (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    എങ്കിൽ (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		എങ്കിൽ (modifiers > 0)
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
	    അല്ലെങ്കിൽ
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
	അല്ലെങ്കിൽ
	{
	    WCHAR	ch2 = NUL;
	    പൂർണ്ണസംഖ്യ		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_താക്കോൽ(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_താക്കോലുകൾ;
	    c = may_adjust_താക്കോൽ_for_ctrl(modifiers, c);
	    --no_reduce_താക്കോലുകൾ;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_shift_modifier(modifiers, c);

	    എങ്കിൽ (typebuf_changed(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		നിർത്തുക;
	    }

	    എങ്കിൽ (c == Ctrl_C && ctrl_c_interrupts)
	    {
# എങ്കിൽ defined(FEAT_CLIENTSERVER)
		trash_നിവേശം_buf();
# endif
		got_int = TRUE;
	    }

	    എങ്കിൽ (g_nMouseClick == -1)
	    {
		പൂർണ്ണസംഖ്യ	n = 1;

		എങ്കിൽ (ch2 == NUL)
		{
		    പൂർണ്ണസംഖ്യ	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    എങ്കിൽ (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_to_enc(ch, &n);
		    എങ്കിൽ (p != ശൂന്യ)
		    {
			വേണ്ടി (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    എങ്കിൽ (p[i] == CSI)
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
		അല്ലെങ്കിൽ
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    എങ്കിൽ (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		എങ്കിൽ (ch2 != NUL)
		{
		    എങ്കിൽ (c == K_NUL)
		    {
			തിരഞ്ഞെടുപ്പ് (ch2)
			{
			അവസ്ഥ (WCHAR)'\324': // SHIFT+Insert
			അവസ്ഥ (WCHAR)'\325': // CTRL+Insert
			അവസ്ഥ (WCHAR)'\327': // SHIFT+Delete
			അവസ്ഥ (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    നിർത്തുക;

			സ്ഥിരസ്ഥിതി:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    നിർത്തുക;
			}
		    }
		    അല്ലെങ്കിൽ
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
		എങ്കിൽ ((modifiers & MOD_MASK_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_MASK_ALT;
		}

		എങ്കിൽ (modifiers != 0)
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
		എങ്കിൽ (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    എങ്കിൽ (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    ആകുമ്പോൾ (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    എങ്കിൽ (len > 0)
    {
	buf[len] = NUL;
	ch_log(ശൂന്യ, "raw key input: \"%s\"", buf);
    }
# endif
    തിരികെ len;

#അല്ലെങ്കിൽ // FEAT_GUI_MSWIN && !VIMDLL
    തിരികെ 0;
#endif // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
executable_file(അക്ഷരം *name, char_u **path)
{
    പൂർണ്ണസംഖ്യ attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    എങ്കിൽ (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	തിരികെ FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    എങ്കിൽ (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	എങ്കിൽ (res == ശൂന്യ)
	    res = resolve_reparse_point((char_u *)name);
	എങ്കിൽ (res == ശൂന്യ)
	    തിരികെ FALSE;
	// The path is already absolute.
	എങ്കിൽ (path != ശൂന്യ)
	    *path = res;
	അല്ലെങ്കിൽ
	    vim_free(res);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (path != ശൂന്യ)
	*path = FullName_save((char_u *)name, FALSE);
    തിരികെ TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    സ്ഥിതം bool
executable_prefix_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_FIND_DATAW	data;
    HANDLE		handle;
    DWORD		error;

    wn = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (wn == ശൂന്യ)
	തിരികെ true;
    handle = FindFirstFileExW(wn, FindExInfoBasic, &data,
					    Find_Ex_തിരയൽ_Name_Match, ശൂന്യ, 0);
    error = GetLastError();
    vim_free(wn);
    എങ്കിൽ (handle != INVALID_HANDLE_മൂല്യം)
    {
	FindClose(handle);
	തിരികെ true;
    }
    തിരികെ error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
executable_exists(
    അക്ഷരം	*name,
    size_t	namelen,
    char_u	**path,
    പൂർണ്ണസംഖ്യ		use_path,
    പൂർണ്ണസംഖ്യ		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3];
    char_u	*p;
    string_T	pathbuf = {ശൂന്യ, 0};
    പൂർണ്ണസംഖ്യ		pathbuf_allocated = FALSE;
    string_T	pathext = {ശൂന്യ, 0};
    പൂർണ്ണസംഖ്യ		pathext_allocated = FALSE;
    char_u	*shname = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		noext = FALSE;
    bool	simple_pathext = true;
    പൂർണ്ണസംഖ്യ		ext_എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		retval = FALSE;

    എങ്കിൽ (namelen >= വലുപ്പം(buf))	// safety check
	തിരികെ FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    എങ്കിൽ (strstr((അക്ഷരം *)shname, "sh") != ശൂന്യ &&
	!(strstr((അക്ഷരം *)shname, "powershell") != ശൂന്യ
				    || strstr((അക്ഷരം *)shname, "pwsh") != ശൂന്യ))
	noext = TRUE;

    എങ്കിൽ (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	എങ്കിൽ (pathext.string == ശൂന്യ)
	    STR_LITERAL_SET(pathext, ".com;.exe;.bat;.cmd");
	അല്ലെങ്കിൽ
	    pathext.length = STRLEN(pathext.string);

	എങ്കിൽ (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    ആകുമ്പോൾ (*p)
	    {
		എങ്കിൽ (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    എങ്കിൽ (*p == '.')
			simple_pathext = false;
		    ++p;
		    തുടരുക;
		}
		e = vim_strchr(p, ';');
		എങ്കിൽ (e == ശൂന്യ)
		    e = pathext.string + pathext.length;
		plen = (size_t)(e - p);
		++ext_എണ്ണം;
		എങ്കിൽ (*p != '.')
		    simple_pathext = false;
		വേണ്ടി (q = p + 1; q < e; ++q)
		    എങ്കിൽ (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		എങ്കിൽ (_strnicoll(name + namelen - plen, (അക്ഷരം *)p, plen) == 0)
		{
		    noext = TRUE;
		    നിർത്തുക;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    എങ്കിൽ (pathext.string == ശൂന്യ)
	STR_LITERAL_SET(pathext, ".");
    അല്ലെങ്കിൽ എങ്കിൽ (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.length + 3);
	എങ്കിൽ (tmp == ശൂന്യ)
	{
	    retval = FALSE;
	    പോകുക theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.length += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    എങ്കിൽ (use_path && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	എങ്കിൽ (p != ശൂന്യ)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    എങ്കിൽ (pathbuf.string == ശൂന്യ)
	    {
		retval = FALSE;
		പോകുക theend;
	    }

	    എങ്കിൽ (mch_getenv("NoDefaultCurrentDirectoryInExePath") == ശൂന്യ &&
		    strstr((അക്ഷരം *)gettail(p_sh), "cmd.exe") != ശൂന്യ)
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
    എങ്കിൽ (pathbuf.string == ശൂന്യ)
	STR_LITERAL_SET(pathbuf, ".");
    p = pathbuf.string;
    ആകുമ്പോൾ (*p)
    {
	char_u	*e;
	size_t	buflen;

	എങ്കിൽ (*p == ';') // Skip empty entry
	{
	    ++p;
	    തുടരുക;
	}
	e = vim_strchr(p, ';');
	എങ്കിൽ (e == ശൂന്യ)
	    e = pathbuf.string + pathbuf.length;

	എങ്കിൽ (e - p + namelen + 2 > വലുപ്പം(buf))
	{
	    retval = FALSE;
	    പോകുക theend;
	}
	// A single "." that means current dir.
	എങ്കിൽ (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	അല്ലെങ്കിൽ
	{
	    buflen = vim_snprintf_safelen(
		(അക്ഷരം *)buf,
		വലുപ്പം(buf),
		"%.*s%s%s", (പൂർണ്ണസംഖ്യ)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	എങ്കിൽ (!noext && simple_pathext && ext_എണ്ണം >= 3 && buflen > 0
		&& buflen + 2 <= വലുപ്പം(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == ശൂന്യ
		&& vim_strchr(gettail(buf), ':') == ശൂന്യ)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_prefix_exists(buf);
	    buf[buflen] = NUL;
	    എങ്കിൽ (!exists)
	    {
		p = e;
		തുടരുക;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	ആകുമ്പോൾ (*p)
	{
	    char_u  *e2;

	    എങ്കിൽ (*p == ';')
	    {
		// Skip empty entry
		++p;
		തുടരുക;
	    }
	    e2 = vim_strchr(p, ';');
	    എങ്കിൽ (e2 == ശൂന്യ)
		e2 = pathext.string + pathext.length;

	    എങ്കിൽ (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		എങ്കിൽ (e2 - p + buflen + 1 > വലുപ്പം(buf))
		{
		    retval = FALSE;
		    പോകുക theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    എങ്കിൽ (executable_file((അക്ഷരം *)buf, path))
	    {
		retval = TRUE;
		പോകുക theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    എങ്കിൽ (pathbuf_allocated)
	സ്വതന്ത്രം(pathbuf.string);
    എങ്കിൽ (pathext_allocated)
	സ്വതന്ത്രം(pathext.string);
    തിരികെ retval;
}

#എങ്കിൽ (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    സ്ഥിതം ശൂന്യം
bad_param_handler(സ്ഥിരം wchar_t *expression UNUSED,
    സ്ഥിരം wchar_t *function UNUSED,
    സ്ഥിരം wchar_t *file UNUSED,
    unsigned പൂർണ്ണസംഖ്യ വരി UNUSED,
    uintptr_t pReserved UNUSED)
{
}

# define SET_INVALID_PARAM_HANDLER \
	((ശൂന്യം)_set_invalid_parameter_handler(bad_param_handler))
#അല്ലെങ്കിൽ
# define SET_INVALID_PARAM_HANDLER
#endif

#ifdef FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    സ്ഥിതം ശൂന്യം
mch_init_g(ശൂന്യം)
{
# ifndef __MINGW32__
    ബാഹ്യ പൂർണ്ണസംഖ്യ _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_ദ്വിമാന;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    Rows = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[_MAX_PATH + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	എങ്കിൽ (exe_pathlen + 10 >= വലുപ്പം(vimrun_location))
	{
	    എങ്കിൽ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ശൂന്യ, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    // First try in same directory as gvim.exe.
	    എങ്കിൽ (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    എങ്കിൽ (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		എങ്കിൽ (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(അക്ഷരം *)vimrun_location,
			വലുപ്പം(vimrun_location),
			"\"%.*svimrun\" ",
			(പൂർണ്ണസംഖ്യ)exe_pathlen, exe_name);
		}
		അല്ലെങ്കിൽ
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		എങ്കിൽ (p != ശൂന്യ)
		{
		    vimrun_path.string = p;
		    vimrun_path.length = plen;
		    vimrun_path_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ശൂന്യ, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	എങ്കിൽ (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    എങ്കിൽ (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    ശൂന്യ, TRUE, FALSE))
	set_option_മൂല്യം_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#endif // FEAT_GUI_MSWIN

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).വലത്_3 - (sr).ഇടത്_3 + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).മുകൾ_2 + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    സ്ഥിതം BOOL
ClearConsoleBuffer(WORD wAttribute)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    DWORD അക്കം_Cells, dummy;

    എങ്കിൽ (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	തിരികെ FALSE;

    അക്കം_Cells = csbi.dwSize.X * csbi.dwSize.Y;
    coord.X = 0;
    coord.Y = 0;
    എങ്കിൽ (!FillConsoleOutputCharacter(g_hConOut, ' ', അക്കം_Cells,
	    coord, &dummy))
	തിരികെ FALSE;
    എങ്കിൽ (!FillConsoleOutputAttribute(g_hConOut, wAttribute, അക്കം_Cells,
	    coord, &dummy))
	തിരികെ FALSE;

    തിരികെ TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    സ്ഥിതം BOOL
FitConsoleWindow(
    COORD dwBufferSize,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD dwWindowSize;
    BOOL NeedAdjust = FALSE;

    എങ്കിൽ (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	തിരികെ FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    എങ്കിൽ (csbi.srWindow.വലത്_3 >= dwBufferSize.X)
    {
	dwWindowSize.X = SRWIDTH(csbi.srWindow);
	എങ്കിൽ (dwWindowSize.X > dwBufferSize.X)
	    dwWindowSize.X = dwBufferSize.X;
	csbi.srWindow.വലത്_3 = dwBufferSize.X - 1;
	csbi.srWindow.ഇടത്_3 = dwBufferSize.X - dwWindowSize.X;
	NeedAdjust = TRUE;
    }
    എങ്കിൽ (csbi.srWindow.Bottom >= dwBufferSize.Y)
    {
	dwWindowSize.Y = SRHEIGHT(csbi.srWindow);
	എങ്കിൽ (dwWindowSize.Y > dwBufferSize.Y)
	    dwWindowSize.Y = dwBufferSize.Y;
	csbi.srWindow.Bottom = dwBufferSize.Y - 1;
	csbi.srWindow.മുകൾ_2 = dwBufferSize.Y - dwWindowSize.Y;
	NeedAdjust = TRUE;
    }
    എങ്കിൽ (NeedAdjust && WantAdjust)
    {
	എങ്കിൽ (!SetConsoleWindowInfo(g_hConOut, TRUE, &csbi.srWindow))
	    തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

തരനിർവചനം ഘടന ConsoleBufferStruct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_BUFFER_INFO	Info;
    PCHAR_INFO			Buffer;
    COORD			BufferSize;
    PSMALL_RECT			Regions;
    പൂർണ്ണസംഖ്യ				അക്കം_Regions;
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
    സ്ഥിതം BOOL
SaveConsoleBuffer(
    ConsoleBuffer *cb)
{
    DWORD അക്കം_Cells;
    COORD BufferCoord;
    SMALL_RECT ReadRegion;
    WORD Y, Y_incr;
    പൂർണ്ണസംഖ്യ i, numregions;

    എങ്കിൽ (cb == ശൂന്യ)
	തിരികെ FALSE;

    എങ്കിൽ (!GetConsoleScreenBufferInfo(g_hConOut, &cb->Info))
    {
	cb->IsValid = FALSE;
	തിരികെ FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    എങ്കിൽ (use_alternate_screen_buffer)
	തിരികെ TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    എങ്കിൽ (!cb->IsValid || cb->Buffer == ശൂന്യ ||
	    cb->BufferSize.X != cb->Info.dwSize.X ||
	    cb->BufferSize.Y != cb->Info.dwSize.Y)
    {
	cb->BufferSize.X = cb->Info.dwSize.X;
	cb->BufferSize.Y = cb->Info.dwSize.Y;
	അക്കം_Cells = cb->BufferSize.X * cb->BufferSize.Y;
	vim_free(cb->Buffer);
	cb->Buffer = ALLOC_MULT(CHAR_INFO, അക്കം_Cells);
	എങ്കിൽ (cb->Buffer == ശൂന്യ)
	    തിരികെ FALSE;
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
    ReadRegion.ഇടത്_3 = 0;
    ReadRegion.വലത്_3 = cb->Info.dwSize.X - 1;
    Y_incr = 12000 / cb->Info.dwSize.X;

    numregions = (cb->Info.dwSize.Y + Y_incr - 1) / Y_incr;
    എങ്കിൽ (cb->Regions == ശൂന്യ || numregions != cb->അക്കം_Regions)
    {
	cb->അക്കം_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->അക്കം_Regions);
	എങ്കിൽ (cb->Regions == ശൂന്യ)
	{
	    VIM_CLEAR(cb->Buffer);
	    തിരികെ FALSE;
	}
    }

    വേണ്ടി (i = 0, Y = 0; i < cb->അക്കം_Regions; i++, Y += Y_incr)
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
	ReadRegion.മുകൾ_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	എങ്കിൽ (!ReadConsoleOutputW(g_hConOut,	// output handle
		cb->Buffer,			// our buffer
		cb->BufferSize,			// dimensions of our buffer
		BufferCoord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->Buffer);
	    VIM_CLEAR(cb->Regions);
	    തിരികെ FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    തിരികെ TRUE;
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
    സ്ഥിതം BOOL
RestoreConsoleBuffer(
    ConsoleBuffer   *cb,
    BOOL	    RestoreScreen)
{
    COORD BufferCoord;
    SMALL_RECT WriteRegion;
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (cb == ശൂന്യ || !cb->IsValid)
	തിരികെ FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    എങ്കിൽ (RestoreScreen)
	ClearConsoleBuffer(cb->Info.wAttributes);

    FitConsoleWindow(cb->Info.dwSize, TRUE);
    എങ്കിൽ (!SetConsoleScreenBufferSize(g_hConOut, cb->Info.dwSize))
	തിരികെ FALSE;
    എങ്കിൽ (!SetConsoleTextAttribute(g_hConOut, cb->Info.wAttributes))
	തിരികെ FALSE;

    എങ്കിൽ (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	തിരികെ TRUE;
    }

    എങ്കിൽ (!SetConsoleCursorPosition(g_hConOut, cb->Info.dwCursorPosition))
	തിരികെ FALSE;
    എങ്കിൽ (!SetConsoleWindowInfo(g_hConOut, TRUE, &cb->Info.srWindow))
	തിരികെ FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    എങ്കിൽ (use_alternate_screen_buffer)
	തിരികെ TRUE;

    /*
     * Restore the screen buffer contents.
     */
    എങ്കിൽ (cb->Buffer != ശൂന്യ)
    {
	വേണ്ടി (i = 0; i < cb->അക്കം_Regions; i++)
	{
	    BufferCoord.X = cb->Regions[i].ഇടത്_3;
	    BufferCoord.Y = cb->Regions[i].മുകൾ_2;
	    WriteRegion = cb->Regions[i];
	    എങ്കിൽ (!WriteConsoleOutputW(g_hConOut,	// output handle
			cb->Buffer,		// our buffer
			cb->BufferSize,		// dimensions of our buffer
			BufferCoord,		// offset in our buffer
			&WriteRegion))		// region to restore
		തിരികെ FALSE;
	}
    }

    തിരികെ TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
സ്ഥിതം ConsoleBuffer g_cbOrig = { 0 };
# endif
സ്ഥിതം ConsoleBuffer g_cbNonTermcap = { 0 };
സ്ഥിതം ConsoleBuffer g_cbTermcap = { 0 };

അക്ഷരം g_szOrigTitle[256] = { 0 };
HWND g_hWnd = ശൂന്യ;	// also used in os_mswin.c
സ്ഥിതം HICON g_hOrigIconSmall = ശൂന്യ;
സ്ഥിതം HICON g_hOrigIcon = ശൂന്യ;
സ്ഥിതം HICON g_hVimIcon = ശൂന്യ;
സ്ഥിതം BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    സ്ഥിതം BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    എങ്കിൽ (hWnd == ശൂന്യ)
	തിരികെ FALSE;

    എങ്കിൽ (phIconSmall != ശൂന്യ)
	*phIconSmall = (HICON)SendMessage(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    എങ്കിൽ (phIcon != ശൂന്യ)
	*phIcon = (HICON)SendMessage(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    തിരികെ TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    സ്ഥിതം BOOL
SetConsoleIcon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    എങ്കിൽ (hWnd == ശൂന്യ)
	തിരികെ FALSE;

    എങ്കിൽ (hIconSmall != ശൂന്യ)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    എങ്കിൽ (hIcon != ശൂന്യ)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    തിരികെ TRUE;
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
    സ്ഥിതം ശൂന്യം
SaveConsoleTitleAndIcon(ശൂന്യം)
{
    // Save the original title.
    എങ്കിൽ (!GetConsoleTitle(g_szOrigTitle, വലുപ്പം(g_szOrigTitle)))
	തിരികെ;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = GetConsoleWindow();
    എങ്കിൽ (g_hWnd == ശൂന്യ)
	തിരികെ;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    എങ്കിൽ (g_hOrigIconSmall == ശൂന്യ || g_hOrigIcon == ശൂന്യ)
	തിരികെ;

    // Extract the first icon contained in the Vim executable.
    എങ്കിൽ (
# ifdef FEAT_LIBCALL
	    mch_icon_load((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == ശൂന്യ)
	g_hVimIcon = ExtractIcon(ശൂന്യ, (LPCSTR)exe_name, 0);
    എങ്കിൽ (g_hVimIcon != ശൂന്യ)
	g_fCanChangeIcon = TRUE;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ g_fWindInitCalled = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ g_fTermcapMode = FALSE;
സ്ഥിതം CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    സ്ഥിതം ശൂന്യം
mch_init_c(ശൂന്യം)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_BUFFER_INFO csbi;
# endif
# ifndef __MINGW32__
    ബാഹ്യ പൂർണ്ണസംഖ്യ _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_ദ്വിമാന;		// we do our own CR-LF translation
    out_flush();

    // Obtain handles for the standard Console I/O devices
    എങ്കിൽ (read_cmd_fd == 0)
	g_hConIn =  GetStdHandle(STD_നിവേശം_HANDLE);
    അല്ലെങ്കിൽ
	create_conin();
    g_hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    SaveConsoleBuffer(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.wAttributes;
# അല്ലെങ്കിൽ
    // Get current text attributes
    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
# endif
    എങ്കിൽ (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (g_attrCurrent & 0xf) + 1;
    എങ്കിൽ (cterm_normal_bg_color == 0)
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
    എങ്കിൽ (g_fCanChangeIcon)
	SetConsoleIcon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    എങ്കിൽ (use_alternate_screen_buffer)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    എങ്കിൽ (fdDump)
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
    സ്ഥിതം ശൂന്യം
mch_exit_c(പൂർണ്ണസംഖ്യ r)
{
    // Copy flag since stoptermcap() will clear the flag.
    പൂർണ്ണസംഖ്യ fTermcapMode = g_fTermcapMode;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    എങ്കിൽ (!fTermcapMode &&  use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    എങ്കിൽ (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    എങ്കിൽ (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	എങ്കിൽ (g_fCanChangeIcon && !g_fForceExit)
	    SetConsoleIcon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	എങ്കിൽ (fdDump)
	{
	    time_t t;

	    time(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = ശൂന്യ;
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

    ശൂന്യം
mch_init(ശൂന്യം)
{
#ifdef VIMDLL
    എങ്കിൽ (gui.starting)
	mch_init_g();
    അല്ലെങ്കിൽ
	mch_init_c();
#അല്ലെങ്കിലെങ്കിൽ defined(FEAT_GUI_MSWIN)
    mch_init_g();
#അല്ലെങ്കിൽ
    mch_init_c();
#endif
}

    ശൂന്യം
mch_exit(പൂർണ്ണസംഖ്യ r)
{
#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef VIMDLL
    എങ്കിൽ (vimrun_path_allocated)
	vim_free(vimrun_path.string);

    എങ്കിൽ (gui.in_use || gui.starting)
	mch_exit_g(r);
    അല്ലെങ്കിൽ
	mch_exit_c(r);
#അല്ലെങ്കിലെങ്കിൽ defined(FEAT_GUI_MSWIN)
    എങ്കിൽ (vimrun_path_allocated)
	vim_free(vimrun_path.string);
    mch_exit_g(r);
#അല്ലെങ്കിൽ
    mch_exit_c(r);
#endif
}

/*
 * Do we have an interactive window?
 */
    പൂർണ്ണസംഖ്യ
mch_check_win(
    പൂർണ്ണസംഖ്യ argc UNUSED,
    അക്ഷരം **argv UNUSED)
{
    mch_get_exe_name();

#എങ്കിൽ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    തിരികെ OK;	    // GUI always has a tty
#അല്ലെങ്കിൽ
# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
	തിരികെ OK;
# endif
    എങ്കിൽ (isatty(1))
	തിരികെ OK;
    തിരികെ FAIL;
#endif
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    ശൂന്യം
fname_case(
    char_u	*name,
    പൂർണ്ണസംഖ്യ		len)
{
    WCHAR   *p;
    WCHAR   buf[_MAX_PATH + 1];

    എങ്കിൽ (*name == NUL)
	തിരികെ;

    slash_adjust(name);

    p = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (GetLongPathNameW(p, buf, _MAX_PATH))
    {
	char_u	*q = utf16_to_enc(buf, ശൂന്യ);

	എങ്കിൽ (q != ശൂന്യ)
	{
	    എങ്കിൽ (len > 0)
		vim_strncpy(name, q, len - 1);
	    അല്ലെങ്കിൽ
	    {
		size_t  namelen = STRLEN(name);

		എങ്കിൽ (namelen >= STRLEN(q))
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
    പൂർണ്ണസംഖ്യ
mch_get_user_name(
    char_u  *s,
    പൂർണ്ണസംഖ്യ	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_LENGTH(wszUserName);

    എങ്കിൽ (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_to_enc(wszUserName, ശൂന്യ);

	എങ്കിൽ (p != ശൂന്യ)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    തിരികെ OK;
	}
    }
    s[0] = NUL;
    തിരികെ FAIL;
}


/*
 * Insert host name in s[len].
 */
    ശൂന്യം
mch_get_host_name(
    char_u	*s,
    പൂർണ്ണസംഖ്യ		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_LENGTH(wszHostName);

    എങ്കിൽ (!GetComputerNameW(wszHostName, &wcch))
	തിരികെ;

    char_u  *p = utf16_to_enc(wszHostName, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    long
mch_get_pid(ശൂന്യം)
{
    തിരികെ (long)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    പൂർണ്ണസംഖ്യ
mch_process_running(long pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   status = 0;
    പൂർണ്ണസംഖ്യ	    ret = FALSE;

    എങ്കിൽ (hProcess == ശൂന്യ)
	തിരികെ FALSE;  // might not have access
    എങ്കിൽ (GetExitCodeProcess(hProcess, &status))
	ret = status == STILL_ACTIVE;
    CloseHandle(hProcess);
    തിരികെ ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    പൂർണ്ണസംഖ്യ
mch_dirname(
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		len)
{
    WCHAR   wbuf[_MAX_PATH + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    എങ്കിൽ (GetCurrentDirectoryW(_MAX_PATH, wbuf) == 0)
	തിരികെ FAIL;

    WCHAR   wcbuf[_MAX_PATH + 1];
    char_u  *p = ശൂന്യ;

    എങ്കിൽ (GetLongPathNameW(wbuf, wcbuf, _MAX_PATH) != 0)
    {
	p = utf16_to_enc(wcbuf, ശൂന്യ);
	എങ്കിൽ (p != ശൂന്യ)
	{
	    എങ്കിൽ (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    എങ്കിൽ (p == ശൂന്യ)
	p = utf16_to_enc(wbuf, ശൂന്യ);

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    തിരികെ OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    long
mch_getperm(char_u *name)
{
    stat_T	st;

    തിരികെ (mch_stat((അക്ഷരം *)name, &st) == 0)
	? (long)(unsigned short)st.st_mode
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
mch_setperm(char_u *name, long perm)
{
    long	n;
    WCHAR	*p;

    p = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    എങ്കിൽ (n == -1)
	തിരികെ FAIL;

    win32_set_archive(name);

    തിരികെ OK;
}

/*
 * Set hidden flag for "name".
 */
    ശൂന്യം
mch_hide(char_u *name)
{
    പൂർണ്ണസംഖ്യ attrs = win32_getattrs(name);
    എങ്കിൽ (attrs == -1)
	തിരികെ;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    പൂർണ്ണസംഖ്യ
mch_ishidden(char_u *name)
{
    പൂർണ്ണസംഖ്യ f = win32_getattrs(name);

    എങ്കിൽ (f == -1)
	തിരികെ FALSE;		    // file does not exist at all

    തിരികെ (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    പൂർണ്ണസംഖ്യ
mch_isdir(char_u *name)
{
    പൂർണ്ണസംഖ്യ f = win32_getattrs(name);

    എങ്കിൽ (f == -1)
	തിരികെ FALSE;		    // file does not exist at all

    തിരികെ (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    പൂർണ്ണസംഖ്യ
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ FALSE;
    attr = GetFileAttributesW(p);
    എങ്കിൽ (attr == INVALID_FILE_ATTRIBUTES)
	error = GetLastError();
    vim_free(p);

    എങ്കിൽ (attr == INVALID_FILE_ATTRIBUTES)
	തിരികെ error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    തിരികെ (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    പൂർണ്ണസംഖ്യ
mch_isrealdir(char_u *name)
{
    തിരികെ mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    പൂർണ്ണസംഖ്യ
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    പൂർണ്ണസംഖ്യ	    retval;

    p = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ -1;
    retval = _wmkdir(p);
    vim_free(p);
    തിരികെ retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    പൂർണ്ണസംഖ്യ
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    പൂർണ്ണസംഖ്യ	    retval;

    p = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ -1;
    retval = _wrmdir(p);
    vim_free(p);
    തിരികെ retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    പൂർണ്ണസംഖ്യ
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    തിരികെ win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_സംഖ്യ_Of_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    പൂർണ്ണസംഖ്യ
mch_is_symbolic_link(char_u *name)
{
    HANDLE		hFind;
    പൂർണ്ണസംഖ്യ			res = FALSE;
    DWORD		fileFlags = 0, reparseTag = 0;
    WCHAR		*wn;
    WIN32_FIND_DATAW	findDataW;

    wn = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (wn == ശൂന്യ)
	തിരികെ FALSE;

    hFind = FindFirstFileW(wn, &findDataW);
    vim_free(wn);
    എങ്കിൽ (hFind != INVALID_HANDLE_മൂല്യം)
    {
	fileFlags = findDataW.dwFileAttributes;
	reparseTag = findDataW.dwReserved0;
	FindClose(hFind);
    }

    എങ്കിൽ ((fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
	    && (reparseTag == IO_REPARSE_TAG_SYMLINK
		|| reparseTag == IO_REPARSE_TAG_MOUNT_POINT))
	res = TRUE;

    തിരികെ res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    പൂർണ്ണസംഖ്യ
mch_is_linked(char_u *fname)
{
    എങ്കിൽ (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    പൂർണ്ണസംഖ്യ
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    പൂർണ്ണസംഖ്യ		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_to_utf16(fname, ശൂന്യ);
    എങ്കിൽ (wn == ശൂന്യ)
	തിരികെ FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    ശൂന്യ,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    ശൂന്യ);		// handle to template file
    vim_free(wn);

    എങ്കിൽ (hFile == INVALID_HANDLE_മൂല്യം)
	തിരികെ FILEINFO_READ_FAIL;

    എങ്കിൽ (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    അല്ലെങ്കിൽ
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    തിരികെ res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
win32_getattrs(char_u *name)
{
    പൂർണ്ണസംഖ്യ		attr;
    WCHAR	*p;

    p = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ INVALID_FILE_ATTRIBUTES;

    attr = GetFileAttributesW(p);
    vim_free(p);

    തിരികെ attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
win32_setattrs(char_u *name, പൂർണ്ണസംഖ്യ attrs)
{
    പൂർണ്ണസംഖ്യ	    res;
    WCHAR   *p;

    p = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ -1;

    res = SetFileAttributesW(p, attrs);
    vim_free(p);

    തിരികെ res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
win32_set_archive(char_u *name)
{
    പൂർണ്ണസംഖ്യ attrs = win32_getattrs(name);
    എങ്കിൽ (attrs == -1)
	തിരികെ -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    തിരികെ win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    പൂർണ്ണസംഖ്യ
mch_writable(char_u *name)
{
    പൂർണ്ണസംഖ്യ attrs = win32_getattrs(name);

    തിരികെ (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    പൂർണ്ണസംഖ്യ
mch_can_exe(char_u *name, char_u **path, പൂർണ്ണസംഖ്യ use_path UNUSED)
{
    തിരികെ executable_exists((അക്ഷരം *)name, STRLEN(name), path, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    പൂർണ്ണസംഖ്യ
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    പൂർണ്ണസംഖ്യ		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    എങ്കിൽ (STRNCMP(name, "\\\\.\\", 4) == 0)
	തിരികെ കണ്ണി_WRITABLE;

    wn = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (wn == ശൂന്യ)
	തിരികെ കണ്ണി_NORMAL;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    ശൂന്യ,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    ശൂന്യ);		    // handle to template file
    vim_free(wn);
    എങ്കിൽ (hFile == INVALID_HANDLE_മൂല്യം)
	തിരികെ കണ്ണി_NORMAL;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    എങ്കിൽ (type == FILE_TYPE_CHAR)
	തിരികെ കണ്ണി_WRITABLE;
    എങ്കിൽ (type == FILE_TYPE_DISK)
	തിരികെ കണ്ണി_NORMAL;
    തിരികെ കണ്ണി_OTHER;
}

#ifdef HAVE_ACL
ഘടന my_acl
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
    തിരികെ (vim_acl_T)ശൂന്യ;
#അല്ലെങ്കിൽ
    ഘടന my_acl   *p = ശൂന്യ;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(ഘടന my_acl);
    എങ്കിൽ (p != ശൂന്യ)
    {
	WCHAR	*wn;

	wn = enc_to_utf16(fname, ശൂന്യ);
	എങ്കിൽ (wn == ശൂന്യ)
	{
	    vim_free(p);
	    തിരികെ ശൂന്യ;
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
	എങ്കിൽ (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (ശൂന്യം)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    ശൂന്യ,
		    ശൂന്യ,
		    &p->pDacl,
		    ശൂന്യ,
		    &p->pSecurityDescriptor);
	}
	എങ്കിൽ (p->pSecurityDescriptor == ശൂന്യ)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = ശൂന്യ;
	}
	vim_free(wn);
    }

    തിരികെ (vim_acl_T)p;
#endif
}

#ifdef HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    സ്ഥിതം BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_SIZE_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_എണ്ണം = 0;
    GetAclInformation(acl, &acl_info, വലുപ്പം(acl_info), AclSizeInformation);
    വേണ്ടി (i = 0; i < acl_info.Ace_എണ്ണം; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	എങ്കിൽ (ace->Header.AceFlags & INHERITED_ACE)
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}
#endif

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    ശൂന്യം
mch_set_acl(char_u *fname, vim_acl_T acl)
{
#ifdef HAVE_ACL
    ഘടന my_acl   *p = (ഘടന my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    wn = enc_to_utf16(fname, ശൂന്യ);
    എങ്കിൽ (wn == ശൂന്യ)
	തിരികെ;

    // Set security flags
    എങ്കിൽ (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    എങ്കിൽ (p->pSidGroup)
	sec_info |= GROUP_SECURITY_INFORMATION;
    എങ്കിൽ (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	എങ്കിൽ (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    എങ്കിൽ (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (ശൂന്യം)SetNamedSecurityInfoW(
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

    ശൂന്യം
mch_free_acl(vim_acl_T acl)
{
#ifdef HAVE_ACL
    ഘടന my_acl   *p = (ഘടന my_acl *)acl;

    എങ്കിൽ (p != ശൂന്യ)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#endif
}

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    സ്ഥിതം BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    നിവേശം_RECORD ir;
    DWORD out;

    തിരഞ്ഞെടുപ്പ് (dwCtrlType)
    {
    അവസ്ഥ CTRL_C_EVENT:
	എങ്കിൽ (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	തിരികെ TRUE;

    അവസ്ഥ CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = താക്കോൽ_EVENT;
	ir.Event.താക്കോൽ_Event_2.b_താക്കോൽ_Down = TRUE;
	ir.Event.താക്കോൽ_Event_2.w_Repeat_എണ്ണം = 1;
	ir.Event.താക്കോൽ_Event_2.w_Virtual_താക്കോൽ_Code = VK_CANCEL;
	ir.Event.താക്കോൽ_Event_2.wVirtualScanCode = 0;
	ir.Event.താക്കോൽ_Event_2.dw_Control_താക്കോൽ_State = 0;
	ir.Event.താക്കോൽ_Event_2.uChar.UnicodeChar = 0;
	Write_Console_നിവേശം(g_hConIn, &ir, 1, &out);
	തിരികെ TRUE;

    // fatal events: shut down gracefully
    അവസ്ഥ CTRL_CLOSE_EVENT:
    അവസ്ഥ CTRL_LOGOFF_EVENT:
    അവസ്ഥ CTRL_SHUTDOWN_EVENT:
	windgoto((പൂർണ്ണസംഖ്യ)Rows - 1, cmdline_col_off);
	g_fForceExit = TRUE;

	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	തിരികെ TRUE;		// not reached

    സ്ഥിരസ്ഥിതി:
	തിരികെ FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    ശൂന്യം
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
	തിരികെ;
# endif
    GetConsoleMode(g_hConIn, &cmodein);
    GetConsoleMode(g_hConOut, &cmodeout);
    എങ്കിൽ (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_വരി_നിവേശം | ENABLE_PROCESSED_നിവേശം |
		     ENABLE_ECHO_നിവേശം);
	എങ്കിൽ (g_fMouseActive)
	{
	    cmodein |= ENABLE_MOUSE_നിവേശം;
	    cmodein &= ~ENABLE_QUICK_EDIT_MODE;
	}
	അല്ലെങ്കിൽ
	{
	    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# അല്ലെങ്കിൽ
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    അല്ലെങ്കിൽ // cooked
    {
	cmodein |= (ENABLE_വരി_നിവേശം | ENABLE_PROCESSED_നിവേശം |
		    ENABLE_ECHO_നിവേശം);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
    SetConsoleMode(g_hConOut, cmodeout);
    SetConsoleCtrlHandler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    എങ്കിൽ (fdDump)
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
    പൂർണ്ണസംഖ്യ
mch_get_shellsize(ശൂന്യം)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
	തിരികെ OK;
# endif
    എങ്കിൽ (!g_fTermcapMode && g_cbTermcap.IsValid)
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
    അല്ലെങ്കിൽ എങ്കിൽ (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	Rows = csbi.srWindow.Bottom - csbi.srWindow.മുകൾ_2 + 1;
	Columns = csbi.srWindow.വലത്_3 - csbi.srWindow.ഇടത്_3 + 1;
    }
    അല്ലെങ്കിൽ
    {
	Rows = 25;
	Columns = 80;
    }
    തിരികെ OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    സ്ഥിതം ശൂന്യം
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    എങ്കിൽ (use_alternate_screen_buffer)
	തിരികെ;

    എങ്കിൽ (!SetConsoleScreenBufferSize(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	എങ്കിൽ (fdDump)
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
    സ്ഥിതം ശൂന്യം
ResizeWindow(
    HANDLE     hConsole,
    SMALL_RECT srWindowRect)
{
    എങ്കിൽ (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
    {
# ifdef MCH_WRITE_DUMP
	എങ്കിൽ (fdDump)
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
    സ്ഥിതം ശൂന്യം
ResizeConBufAndWindow(
    HANDLE  hConsole,
    പൂർണ്ണസംഖ്യ	    xSize,
    പൂർണ്ണസംഖ്യ	    ySize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    srWindowRect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    എങ്കിൽ (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", xSize, ySize);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    // define the new console window size and scroll position
    srWindowRect.ഇടത്_3 = srWindowRect.മുകൾ_2 = (SHORT) 0;
    srWindowRect.വലത്_3 =  (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);

    എങ്കിൽ (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	പൂർണ്ണസംഖ്യ sx, sy;

	sx = csbi.srWindow.വലത്_3 - csbi.srWindow.ഇടത്_3 + 1;
	sy = csbi.srWindow.Bottom - csbi.srWindow.മുകൾ_2 + 1;
	എങ്കിൽ (sy < ySize || sx < xSize)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    എങ്കിൽ (sy < ySize)
		coordScreen.Y = ySize;
	    അല്ലെങ്കിൽ
		coordScreen.Y = sy;
	    എങ്കിൽ (sx < xSize)
		coordScreen.X = xSize;
	    അല്ലെങ്കിൽ
		coordScreen.X = sx;
	    SetConsoleScreenBufferSize(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = xSize;
    coordScreen.Y = ySize;

    // In the new console call API, only the first time in reverse order
    എങ്കിൽ (!vtp_working || resized)
    {
	ResizeWindow(hConsole, srWindowRect);
	ResizeConBuf(hConsole, coordScreen);
    }
    അല്ലെങ്കിൽ
    {
	// Workaround for a Windows 10 bug
	cursor.X = srWindowRect.ഇടത്_3;
	cursor.Y = srWindowRect.മുകൾ_2;
	SetConsoleCursorPosition(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	ResizeWindow(hConsole, srWindowRect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    ശൂന്യം
mch_set_shellsize(ശൂന്യം)
{
    COORD coordScreen;

# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
	തിരികെ;
# endif
    // Don't change window size while still starting up
    എങ്കിൽ (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	തിരികെ;
    }

    എങ്കിൽ (term_console)
    {
	coordScreen = GetLargestConsoleWindowSize(g_hConOut);

	// Clamp Rows and Columns to reasonable values
	എങ്കിൽ (Rows > coordScreen.Y)
	    Rows = coordScreen.Y;
	എങ്കിൽ (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	ResizeConBufAndWindow(g_hConOut, Columns, Rows);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    ശൂന്യം
mch_new_shellsize(ശൂന്യം)
{
# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
	തിരികെ;
# endif
    set_scroll_region(0, 0, Columns - 1, Rows - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    ശൂന്യം
mch_set_winsize_now(ശൂന്യം)
{
    എങ്കിൽ (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_set_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# എങ്കിൽ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    ശൂന്യം
mch_calc_cell_size(ഘടന cellsize *cs_out)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_out->cs_xpixel = -1;
    cs_out->cs_ypixel = -1;

    എങ്കിൽ (g_hConOut == INVALID_HANDLE_മൂല്യം)
	തിരികെ;

    cfi.cbSize = വലുപ്പം(cfi);
    എങ്കിൽ (GetCurrentConsoleFontEx(g_hConOut, FALSE, &cfi)
	    && cfi.dwFontSize.X > 0 && cfi.dwFontSize.Y > 0)
    {
	cs_out->cs_xpixel = cfi.dwFontSize.X;
	cs_out->cs_ypixel = cfi.dwFontSize.Y;
	തിരികെ;
    }

    GetConsoleMode(g_hConIn, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_വരി_നിവേശം | ENABLE_ECHO_നിവേശം
	    | ENABLE_PROCESSED_നിവേശം);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_നിവേശം;

    എങ്കിൽ (SetConsoleMode(g_hConIn, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	SetConsoleMode(g_hConIn, cmodeold);
    }
}
# endif

#endif

    സ്ഥിതം BOOL
vim_create_process(
    അക്ഷരം		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    അക്ഷരം		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = ശൂന്യ;

    wcmd = enc_to_utf16((char_u *)cmd, ശൂന്യ);
    എങ്കിൽ (wcmd == ശൂന്യ)
	തിരികെ FALSE;
    എങ്കിൽ (cwd != ശൂന്യ)
    {
	wcwd = enc_to_utf16((char_u *)cwd, ശൂന്യ);
	എങ്കിൽ (wcwd == ശൂന്യ)
	    പോകുക theend;
    }

    ret = CreateProcessW(
	    ശൂന്യ,		// Executable name
	    wcmd,		// Command to execute
	    ശൂന്യ,		// Process security attributes
	    ശൂന്യ,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    തിരികെ ret;
}


    സ്ഥിതം HINSTANCE
vim_shell_execute(
    അക്ഷരം *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_to_utf16((char_u *)cmd, ശൂന്യ);
    എങ്കിൽ (wcmd == ശൂന്യ)
	തിരികെ (HINSTANCE) 0;

    ret = ShellExecuteW(ശൂന്യ, ശൂന്യ, wcmd, ശൂന്യ, ശൂന്യ, n_show_cmd);
    vim_free(wcmd);
    തിരികെ ret;
}


#എങ്കിൽ defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mch_system_classic(അക്ഷരം *cmd, പൂർണ്ണസംഖ്യ options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = വലുപ്പം(si);
    si.lpReserved = ശൂന്യ;
    si.lpDesktop = ശൂന്യ;
    si.lpTitle = ശൂന്യ;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    എങ്കിൽ (options & SHELL_DOOUT)
	si.wShowWindow = SW_SHOWMINNOACTIVE;
    അല്ലെങ്കിൽ
	si.wShowWindow = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = ശൂന്യ;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_MODE |	CREATE_NEW_CONSOLE,
	    &si, &pi, ശൂന്യ, ശൂന്യ);

    // Wait for the command to terminate before continuing
    {
	പൂർണ്ണസംഖ്യ	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	വേണ്ടി (;;)
	{
	    സന്ദേശം_2	സന്ദേശം;

	    എങ്കിൽ (PeekMessageW(&സന്ദേശം, (HWND)ശൂന്യ, 0, 0, PM_REMOVE))
	    {
		TranslateMessage(&സന്ദേശം);
		DispatchMessageW(&സന്ദേശം);
		delay = 1;
		തുടരുക;
	    }
	    എങ്കിൽ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		നിർത്തുക;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    എങ്കിൽ (delay < 50)
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

    തിരികെ ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_hChildStd_IN_Wr = param;
    linenr_T	    lnum = curbuf->b_op_start.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    പൂർണ്ണസംഖ്യ		    written = 0;

    വേണ്ടി (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	എങ്കിൽ (l == 0)
	    len = 0;
	അല്ലെങ്കിൽ എങ്കിൽ (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_hChildStd_IN_Wr, "", 1, &len, ശൂന്യ);
	}
	അല്ലെങ്കിൽ
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_hChildStd_IN_Wr, (അക്ഷരം *)lp + written,
		      s == ശൂന്യ ? l : (DWORD)(s - (lp + written)),
		      &len, ശൂന്യ);
	}
	എങ്കിൽ (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    എങ്കിൽ (lnum != curbuf->b_op_end.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_വരി_എണ്ണം
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_hChildStd_IN_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, ശൂന്യ);
	    }

	    ++lnum;
	    എങ്കിൽ (lnum > curbuf->b_op_end.lnum)
		നിർത്തുക;

	    lp = ml_get(lnum);
	    written = 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_hChildStd_IN_Wr);
    തിരികെ 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    സ്ഥിതം ശൂന്യം
dump_pipe(പൂർണ്ണസംഖ്യ	    options,
	  HANDLE    g_hChildStd_OUT_Rd,
	  garray_T  *ga,
	  char_u    buffer[],
	  DWORD	    *buffer_off)
{
    DWORD	availableBytes = 0;
    DWORD	i;
    പൂർണ്ണസംഖ്യ		ret;
    DWORD	len;
    DWORD	toRead;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_hChildStd_OUT_Rd, // pipe to query
			ശൂന്യ,		    // optional buffer
			0,		    // buffer size
			ശൂന്യ,		    // number of read bytes
			&availableBytes,    // available bytes total
			ശൂന്യ);		    // byteLeft

    // We got real data in the pipe, read it
    ആകുമ്പോൾ (ret != 0 && availableBytes > 0)
    {
	toRead = (DWORD)(BUFLEN - *buffer_off);
	toRead = availableBytes < toRead ? availableBytes : toRead;
	ReadFile(g_hChildStd_OUT_Rd, buffer + *buffer_off, toRead , &len, ശൂന്യ);

	// If we haven't read anything, there is a problem
	എങ്കിൽ (len == 0)
	    നിർത്തുക;

	availableBytes -= len;

	എങ്കിൽ (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    വേണ്ടി (i = 0; i < len; ++i)
	    {
		എങ്കിൽ (buffer[i] == NL)
		    append_ga_വരി(ga);
		അല്ലെങ്കിൽ എങ്കിൽ (buffer[i] == NUL)
		    ga_append(ga, NL);
		അല്ലെങ്കിൽ
		    ga_append(ga, buffer[i]);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
	{
	    പൂർണ്ണസംഖ്യ		l;
	    പൂർണ്ണസംഖ്യ		c;
	    char_u	*p;

	    len += *buffer_off;
	    buffer[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    വേണ്ടി (p = buffer; p < buffer + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		എങ്കിൽ (l == 0)
		    l = 1;  // NUL byte?
		അല്ലെങ്കിൽ എങ്കിൽ (MB_BYTE2LEN(*p) != l)
		    നിർത്തുക;
	    }
	    എങ്കിൽ (p == buffer)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		എങ്കിൽ (len >= 12)
		    ++p;
		അല്ലെങ്കിൽ
		{
		    *buffer_off = len;
		    തിരികെ;
		}
	    }
	    c = *p;
	    *p = NUL;
	    സന്ദേശം_puts((അക്ഷരം *)buffer);
	    എങ്കിൽ (p < buffer + len)
	    {
		*p = c;
		*buffer_off = (DWORD)((buffer + len) - p);
		mch_memmove(buffer, p, *buffer_off);
		തിരികെ;
	    }
	    *buffer_off = 0;
	}
	അല്ലെങ്കിൽ
	{
	    buffer[len] = NUL;
	    സന്ദേശം_puts((അക്ഷരം *)buffer);
	}

	windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col);
	cursor_on();
	out_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mch_system_piped(അക്ഷരം *cmd, പൂർണ്ണസംഖ്യ options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_hChildStd_IN_Rd = ശൂന്യ;
    HANDLE g_hChildStd_IN_Wr = ശൂന്യ;
    HANDLE g_hChildStd_OUT_Rd = ശൂന്യ;
    HANDLE g_hChildStd_OUT_Wr = ശൂന്യ;

    char_u	buffer[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    പൂർണ്ണസംഖ്യ		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    പൂർണ്ണസംഖ്യ		noread_cnt = 0;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		delay = 1;
    DWORD	buffer_off = 0;	// valid bytes in buffer[]
    അക്ഷരം	*p = ശൂന്യ;

    SECURITY_ATTRIBUTES saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = വലുപ്പം(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ശൂന്യ;

    എങ്കിൽ ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)
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
	സന്ദേശം_puts(_("\nCannot create pipes\n"));
    }

    si.cb = വലുപ്പം(si);
    si.lpReserved = ശൂന്യ;
    si.lpDesktop = ശൂന്യ;
    si.lpTitle = ശൂന്യ;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_hChildStd_OUT_Wr;
    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.h_Std_നിവേശം = g_hChildStd_IN_Rd;
    si.wShowWindow = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = ശൂന്യ;

    എങ്കിൽ (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    എങ്കിൽ (cmd != ശൂന്യ)
    {
	p = (അക്ഷരം *)vim_strsave((char_u *)cmd);
	എങ്കിൽ (p != ശൂന്യ)
	    unescape_shellxquote((char_u *)p, p_sxe);
	അല്ലെങ്കിൽ
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_MODE,
						&si, &pi, ശൂന്യ, ശൂന്യ);

    എങ്കിൽ (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(g_hChildStd_OUT_Wr);

    എങ്കിൽ (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(ശൂന്യ,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_hChildStd_IN_Wr,  // parameter
			0,		 // creation flag, start immediately
			ശൂന്യ);		    // we don't care about thread id
	CloseHandle(thread);
	g_hChildStd_IN_Wr = ശൂന്യ;
    }

    // Keep updating the window while waiting for the shell to finish.
    വേണ്ടി (;;)
    {
	സന്ദേശം_2	സന്ദേശം;

	എങ്കിൽ (PeekMessageW(&സന്ദേശം, (HWND)ശൂന്യ, 0, 0, PM_REMOVE))
	{
	    TranslateMessage(&സന്ദേശം);
	    DispatchMessageW(&സന്ദേശം);
	}

	// write pipe information in the window
	എങ്കിൽ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.in_use
# endif
	    )
	{
	    len = 0;
	    എങ്കിൽ (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		എങ്കിൽ (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		എങ്കിൽ (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    എങ്കിൽ (len == 1 && cmd != ശൂന്യ)
		    {
			എങ്കിൽ (ta_buf[ta_len] == Ctrl_C)
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
		    എങ്കിൽ ((len == 1 || len == 4 ) && cmd != ശൂന്യ)
		    {
			എങ്കിൽ (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_hChildStd_IN_Wr);
			    g_hChildStd_IN_Wr = ശൂന്യ;
			    len = 0;
			}
		    }

		    len = term_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    വേണ്ടി (i = ta_len; i < ta_len + len; ++i)
		    {
			എങ്കിൽ (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    സന്ദേശം_putchar(ta_buf[i]);
			അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
			{
			    പൂർണ്ണസംഖ്യ l = (*mb_ptr2len)(ta_buf + i);

			    സന്ദേശം_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			അല്ലെങ്കിൽ
			    സന്ദേശം_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col);
		    out_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    എങ്കിൽ (options & SHELL_WRITE)
			ta_len = 0;
		    അല്ലെങ്കിൽ എങ്കിൽ (g_hChildStd_IN_Wr != ശൂന്യ)
		    {
			WriteFile(g_hChildStd_IN_Wr, (അക്ഷരം*)ta_buf,
				    1, &len, ശൂന്യ);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			എങ്കിൽ (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	എങ്കിൽ (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	എങ്കിൽ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);
	    നിർത്തുക;
	}

	++noread_cnt;
	dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	എങ്കിൽ (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_hChildStd_OUT_Rd);
    എങ്കിൽ (g_hChildStd_IN_Wr != ശൂന്യ)
	CloseHandle(g_hChildStd_IN_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    GetExitCodeProcess(pi.hProcess, &ret);

    എങ്കിൽ (options & SHELL_READ)
    {
	എങ്കിൽ (ga.ga_len > 0)
	{
	    append_ga_വരി(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	അല്ലെങ്കിൽ
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    തിരികെ ret;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
mch_system_g(അക്ഷരം *cmd, പൂർണ്ണസംഖ്യ options)
{
    // if we can pipe and the shelltemp option is off
    എങ്കിൽ (!p_stmp)
	തിരികെ mch_system_piped(cmd, options);
    അല്ലെങ്കിൽ
	തിരികെ mch_system_classic(cmd, options);
}
#endif

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mch_system_c(അക്ഷരം *cmd, പൂർണ്ണസംഖ്യ options UNUSED)
{
    പൂർണ്ണസംഖ്യ		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    എങ്കിൽ (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	എങ്കിൽ (buf == ശൂന്യ)
	    തിരികെ -1;
	vim_snprintf((അക്ഷരം *)buf, len, "(%s)", cmd);
	wcmd = enc_to_utf16(buf, ശൂന്യ);
	സ്വതന്ത്രം(buf);
    }
    അല്ലെങ്കിൽ
	wcmd = enc_to_utf16((char_u *)cmd, ശൂന്യ);

    എങ്കിൽ (wcmd == ശൂന്യ)
	തിരികെ -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    തിരികെ ret;
}

#endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
mch_system(അക്ഷരം *cmd, പൂർണ്ണസംഖ്യ options)
{
#ifdef VIMDLL
    എങ്കിൽ (gui.in_use || gui.starting)
	തിരികെ mch_system_g(cmd, options);
    അല്ലെങ്കിൽ
	തിരികെ mch_system_c(cmd, options);
#അല്ലെങ്കിലെങ്കിൽ defined(FEAT_GUI_MSWIN)
    തിരികെ mch_system_g(cmd, options);
#അല്ലെങ്കിൽ
    തിരികെ mch_system_c(cmd, options);
#endif
}

#എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mch_call_shell_terminal(
    char_u	*cmd,
    പൂർണ്ണസംഖ്യ		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = ശൂന്യ;
    typval_T	argvar[2];
    long_u	cmdlen;
    പൂർണ്ണസംഖ്യ		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    എങ്കിൽ (cmd == ശൂന്യ)
	cmdlen = STRLEN(p_sh) + 1;
    അല്ലെങ്കിൽ
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    എങ്കിൽ (newcmd == ശൂന്യ)
	തിരികെ 255;
    എങ്കിൽ (cmd == ശൂന്യ)
    {
	STRCPY(newcmd, p_sh);
	ch_log(ശൂന്യ, "starting terminal to run a shell");
    }
    അല്ലെങ്കിൽ
    {
	vim_snprintf((അക്ഷരം *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(ശൂന്യ, "starting terminal for system command '%s'", cmd);
    }

    init_job_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = term_start(argvar, ശൂന്യ, &opt, TERM_START_SYSTEM);
    എങ്കിൽ (buf == ശൂന്യ)
    {
	vim_free(newcmd);
	തിരികെ 255;
    }

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    എങ്കിൽ (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	ആകുമ്പോൾ (term_use_loop())
	{
	    എങ്കിൽ (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		എങ്കിൽ (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    അല്ലെങ്കിൽ
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(ശൂന്യ, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    തിരികെ retval;
}
#endif
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    സ്ഥിതം ശൂന്യം
restore_env_var(char_u *name, char_u *old_മൂല്യം, പൂർണ്ണസംഖ്യ must_free)
{
    എങ്കിൽ (old_മൂല്യം != ശൂന്യ)
    {
	vim_setenv(name, old_മൂല്യം);
	എങ്കിൽ (must_free)
	    vim_free(old_മൂല്യം);
	തിരികെ;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    പൂർണ്ണസംഖ്യ
mch_call_shell(
    char_u  *cmd,
    പൂർണ്ണസംഖ്യ	    options)	// SHELL_*, see vim.h
{
    പൂർണ്ണസംഖ്യ		x = 0;
    പൂർണ്ണസംഖ്യ		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    പൂർണ്ണസംഖ്യ		must_free;
    char_u	*oldval;

#ifdef FEAT_EVAL
    ch_log(ശൂന്യ, "executing shell command: %s", cmd);
#endif
    // Change the title to reflect that we are in a subshell.
    എങ്കിൽ (GetConsoleTitleW(szShellTitle, ARRAY_LENGTH(szShellTitle) - 4) > 0)
    {
	എങ്കിൽ (cmd == ശൂന്യ)
	    wcscat(szShellTitle, L" :sh");
	അല്ലെങ്കിൽ
	{
	    WCHAR *wn = enc_to_utf16((char_u *)cmd, ശൂന്യ);

	    എങ്കിൽ (wn != ശൂന്യ)
	    {
		wcscat(szShellTitle, L" - !");
		എങ്കിൽ ((wcslen(szShellTitle) + wcslen(wn) <
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
    എങ്കിൽ (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#endif
#എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    എങ്കിൽ (
# ifdef VIMDLL
	    gui.in_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != ശൂന്യ
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	എങ്കിൽ (cmdbase != ശൂന്യ)
	    // Skip a leading quote and (.
	    ആകുമ്പോൾ (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	എങ്കിൽ (cmdbase == ശൂന്യ || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    തിരികെ x;
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

    എങ്കിൽ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    എങ്കിൽ (cmd == ശൂന്യ)
    {
	x = mch_system((അക്ഷരം *)p_sh, options);
    }
    അല്ലെങ്കിൽ
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = ശൂന്യ;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	എങ്കിൽ (*cmdbase == '"' )
	    ++cmdbase;
	എങ്കിൽ (*cmdbase == '(')
	    ++cmdbase;

	എങ്കിൽ ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    ZeroMemory(&si, വലുപ്പം(si));
	    si.cb = വലുപ്പം(si);
	    si.lpReserved = ശൂന്യ;
	    si.lpDesktop = ശൂന്യ;
	    si.lpTitle = ശൂന്യ;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = ശൂന്യ;

	    cmdbase = skipwhite(cmdbase + 5);
	    എങ്കിൽ ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_WINDOW;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_നിവേശം = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    ശൂന്യ,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_NORMAL,		// File att.
		    ശൂന്യ);				// Temp file
		si.hStdOutput = si.h_Std_നിവേശം;
		si.hStdError = si.h_Std_നിവേശം;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    എങ്കിൽ (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		എങ്കിൽ (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		എങ്കിൽ (p > cmdbase && p[-1] == ')'
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
	    എങ്കിൽ (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		എങ്കിൽ (cmd_shell == ശൂന്യ || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		എങ്കിൽ (subcmd != ശൂന്യ)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    എങ്കിൽ (newcmd != ശൂന്യ)
			vim_snprintf((അക്ഷരം *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    അല്ലെങ്കിൽ
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    എങ്കിൽ (vim_create_process((അക്ഷരം *)newcmd, FALSE, flags,
			&si, &pi, ശൂന്യ, ശൂന്യ))
		x = 0;
	    അല്ലെങ്കിൽ എങ്കിൽ (vim_shell_execute((അക്ഷരം *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    അല്ലെങ്കിൽ
	    {
		x = -1;
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
		എങ്കിൽ (gui.in_use)
# endif
		    emsg(_(e_command_not_found));
#endif
	    }

	    എങ്കിൽ (newcmd != cmdbase)
		vim_free(newcmd);

	    എങ്കിൽ (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_നിവേശം != ശൂന്യ)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_നിവേശം);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	അല്ലെങ്കിൽ
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
	    എങ്കിൽ (newcmd != ശൂന്യ)
	    {
#എങ്കിൽ defined(FEAT_GUI_MSWIN)
		എങ്കിൽ (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    അക്ഷരം *സന്ദേശം = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    അക്ഷരം *title = _("Vim Warning");
		    WCHAR *wmsg = enc_to_utf16((char_u *)സന്ദേശം, ശൂന്യ);
		    WCHAR *wtitle = enc_to_utf16((char_u *)title, ശൂന്യ);

		    എങ്കിൽ (wmsg != ശൂന്യ && wtitle != ശൂന്യ)
			MessageBoxW(ശൂന്യ, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		എങ്കിൽ (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((അക്ഷരം *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_path.string,
			    (സന്ദേശം_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		അല്ലെങ്കിൽ എങ്കിൽ (
# ifdef VIMDLL
			(gui.in_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((അക്ഷരം *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		അല്ലെങ്കിൽ
#endif
		    vim_snprintf((അക്ഷരം *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((അക്ഷരം *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    എങ്കിൽ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    എങ്കിൽ (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#എങ്കിൽ defined(FEAT_GUI_MSWIN)
	    && ((gui.in_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#endif
	    )
    {
	smsg(_("shell returned %d"), x);
	സന്ദേശം_putchar('\n');
    }
    resettitle();

    mch_signal(SIGINT, SIG_DFL);
    mch_signal(SIGBREAK, SIG_DFL);
    mch_signal(SIGILL, SIG_DFL);
    mch_signal(SIGFPE, SIG_DFL);
    mch_signal(SIGSEGV, SIG_DFL);
    mch_signal(SIGTERM, SIG_DFL);
    mch_signal(SIGABRT, SIG_DFL);

    തിരികെ x;
}

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
    സ്ഥിതം HANDLE
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

    wn = enc_to_utf16(fname, ശൂന്യ);
    എങ്കിൽ (wn == ശൂന്യ)
	തിരികെ INVALID_HANDLE_മൂല്യം;

    h = CreateFileW(wn, dwDesiredAccess, dwShareMode,
	    lpSecurityAttributes, dwCreationDisposition,
	    dwFlagsAndAttributes, ശൂന്യ);
    vim_free(wn);
    തിരികെ h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    ശൂന്യം
win32_build_env(dict_T *env, garray_T *gap, പൂർണ്ണസംഖ്യ is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != ശൂന്യ ? env->dv_hashtab.ht_used : 0;
    LPVOID	base = GetEnvironmentStringsW();

    // for last \0
    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ (env != ശൂന്യ)
    {
	FOR_ALL_HASHTAB_ഇനങ്ങൾ(&env->dv_hashtab, hi, todo)
	{
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	    {
		typval_T *item = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_to_utf16((char_u *)hi->hi_താക്കോൽ, ശൂന്യ);
		WCHAR   *wval = enc_to_utf16(tv_get_string(item), ശൂന്യ);
		--todo;
		എങ്കിൽ (wkey != ശൂന്യ && wval != ശൂന്യ)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    എങ്കിൽ (ga_grow(gap, (പൂർണ്ണസംഖ്യ)(lkey + lval + 2)) == FAIL)
			തുടരുക;
		    വേണ്ടി (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'=';
		    വേണ്ടി (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    എങ്കിൽ (base)
    {
	WCHAR	*p = (WCHAR*) base;

	// for last \0
	എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	    തിരികെ;

	ആകുമ്പോൾ (*p != 0 || *(p + 1) != 0)
	{
	    എങ്കിൽ (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_data + gap->ga_len++) = *p;
	    p++;
	}
	FreeEnvironmentStringsW(base);
	*((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
    }

# എങ്കിൽ defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
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

	എങ്കിൽ (ga_grow(gap, (പൂർണ്ണസംഖ്യ)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    വേണ്ടി (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    വേണ്ടി (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    എങ്കിൽ (is_terminal)
	    {
		വേണ്ടി (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_data + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		വേണ്ടി (n = 0; n < version_len; n++)
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
    സ്ഥിതം BOOL
create_pipe_pair(HANDLE handles[2])
{
    സ്ഥിതം LONG		s;
    അക്ഷരം		name[64];
    SECURITY_ATTRIBUTES sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_SIZE, 0, 0, ശൂന്യ);

    എങ്കിൽ (handles[1] == INVALID_HANDLE_മൂല്യം)
	തിരികെ FALSE;

    sa.nLength = വലുപ്പം(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = ശൂന്യ;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    എങ്കിൽ (handles[0] == INVALID_HANDLE_മൂല്യം)
    {
	CloseHandle(handles[1]);
	തിരികെ FALSE;
    }

    തിരികെ TRUE;
}

#endif // FEAT_JOB_CHANNEL

#എങ്കിൽ defined(FEAT_EVAL)
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
    അക്ഷരം	**argv,
    char_u	*infile,
    പൂർണ്ണസംഖ്യ		flags UNUSED,
    പൂർണ്ണസംഖ്യ		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;
    HANDLE		hChildStdoutRd = INVALID_HANDLE_മൂല്യം;
    HANDLE		hChildStdoutWr = INVALID_HANDLE_മൂല്യം;
    HANDLE		hChildStdinRd = INVALID_HANDLE_മൂല്യം;
    garray_T		cmd_ga;
    garray_T		out_ga;
    char_u		*buffer = ശൂന്യ;
    DWORD		exit_code = (DWORD)-1;
    പൂർണ്ണസംഖ്യ			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    വേണ്ടി (i = 0; argv[i] != ശൂന്യ; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	പൂർണ്ണസംഖ്യ	has_spaces = FALSE;
	പൂർണ്ണസംഖ്യ	j;

	വേണ്ടി (j = 0; s[j] != NUL; j++)
	    എങ്കിൽ (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		നിർത്തുക;
	    }

	എങ്കിൽ (i > 0)
	    ga_append(&cmd_ga, ' ');

	എങ്കിൽ (has_spaces)
	{
	    പൂർണ്ണസംഖ്യ	അക്കം_bs;

	    ga_append(&cmd_ga, '"');
	    വേണ്ടി (j = 0; arg[j] != NUL; j++)
	    {
		അക്കം_bs = 0;
		ആകുമ്പോൾ (arg[j] == '\\')
		{
		    അക്കം_bs++;
		    j++;
		}

		എങ്കിൽ (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    ആകുമ്പോൾ (അക്കം_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    നിർത്തുക;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    ആകുമ്പോൾ (അക്കം_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		അല്ലെങ്കിൽ
		{
		    ആകുമ്പോൾ (അക്കം_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	അല്ലെങ്കിൽ
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&out_ga, 1, 4096);

    saAttr.nLength = വലുപ്പം(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ശൂന്യ;

    // Create a pipe for the child's stdout.
    എങ്കിൽ (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)
	    || !SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	പോകുക done;
    }

    // Set up stdin from infile if provided.
    എങ്കിൽ (infile != ശൂന്യ)
    {
	WCHAR *winfile = enc_to_utf16(infile, ശൂന്യ);

	എങ്കിൽ (winfile != ശൂന്യ)
	{
	    hChildStdinRd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL, ശൂന്യ);
	    vim_free(winfile);
	}
    }

    ZeroMemory(&pi, വലുപ്പം(pi));
    ZeroMemory(&si, വലുപ്പം(si));
    si.cb = വലുപ്പം(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;
    si.h_Std_നിവേശം = (hChildStdinRd != INVALID_HANDLE_മൂല്യം)
		    ? hChildStdinRd : INVALID_HANDLE_മൂല്യം;

    ch_log(ശൂന്യ, "directly executing: %s", (അക്ഷരം *)cmd_ga.ga_data);

    // Create the child process directly, without going through the shell.
    എങ്കിൽ (!vim_create_process((അക്ഷരം *)cmd_ga.ga_data, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
		&si, &pi, ശൂന്യ, ശൂന്യ))
    {
	semsg(_(e_invalid_argument_str), cmd_ga.ga_data);
	പോകുക done;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(hChildStdoutWr);
    hChildStdoutWr = INVALID_HANDLE_മൂല്യം;
    എങ്കിൽ (hChildStdinRd != INVALID_HANDLE_മൂല്യം)
    {
	CloseHandle(hChildStdinRd);
	hChildStdinRd = INVALID_HANDLE_മൂല്യം;
    }

    // Read output from child process.
    വേണ്ടി (;;)
    {
	അക്ഷരം	buf[4096];
	DWORD	n;

	എങ്കിൽ (!ReadFile(hChildStdoutRd, buf, വലുപ്പം(buf), &n, ശൂന്യ) || n == 0)
	    നിർത്തുക;
	എങ്കിൽ (ga_grow(&out_ga, (പൂർണ്ണസംഖ്യ)n) == OK)
	{
	    mch_memmove((അക്ഷരം *)out_ga.ga_data + out_ga.ga_len, buf, n);
	    out_ga.ga_len += (പൂർണ്ണസംഖ്യ)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    set_vim_var_nr(VV_SHELL_ERROR, (long)exit_code);

    എങ്കിൽ (out_ga.ga_len > 0)
    {
	buffer = alloc(out_ga.ga_len + 1);
	എങ്കിൽ (buffer != ശൂന്യ)
	{
	    mch_memmove(buffer, out_ga.ga_data, out_ga.ga_len);
	    എങ്കിൽ (ret_len == ശൂന്യ)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		വേണ്ടി (i = 0; i < out_ga.ga_len; ++i)
		    എങ്കിൽ (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[out_ga.ga_len] = NUL;
	    }
	    അല്ലെങ്കിൽ
		*ret_len = out_ga.ga_len;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ret_len != ശൂന്യ)
	*ret_len = 0;

done:
    ga_clear(&cmd_ga);
    ga_clear(&out_ga);
    എങ്കിൽ (hChildStdoutRd != INVALID_HANDLE_മൂല്യം)
	CloseHandle(hChildStdoutRd);
    എങ്കിൽ (hChildStdoutWr != INVALID_HANDLE_മൂല്യം)
	CloseHandle(hChildStdoutWr);
    എങ്കിൽ (hChildStdinRd != INVALID_HANDLE_മൂല്യം)
	CloseHandle(hChildStdinRd);
    തിരികെ buffer;
}
#endif // FEAT_EVAL

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
    ശൂന്യം
mch_job_start(അക്ഷരം *cmd, job_T *job, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_ATTRIBUTES saAttr;
    channel_T		*channel = ശൂന്യ;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    പൂർണ്ണസംഖ്യ		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    പൂർണ്ണസംഖ്യ		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    പൂർണ്ണസംഖ്യ		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    പൂർണ്ണസംഖ്യ		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    പൂർണ്ണസംഖ്യ		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    പൂർണ്ണസംഖ്യ		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    പൂർണ്ണസംഖ്യ		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;

    എങ്കിൽ (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_മൂല്യം;
    ifd[1] = INVALID_HANDLE_മൂല്യം;
    ofd[0] = INVALID_HANDLE_മൂല്യം;
    ofd[1] = INVALID_HANDLE_മൂല്യം;
    efd[0] = INVALID_HANDLE_മൂല്യം;
    efd[1] = INVALID_HANDLE_മൂല്യം;
    ga_init2(&ga, വലുപ്പം(wchar_t), 500);

    jo = CreateJobObject(ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (jo == ശൂന്യ)
    {
	job->jv_status = JOB_FAILED;
	പോകുക failed;
    }

    എങ്കിൽ (options->jo_env != ശൂന്യ)
	win32_build_env(options->jo_env, &ga, FALSE);

    ZeroMemory(&pi, വലുപ്പം(pi));
    ZeroMemory(&si, വലുപ്പം(si));
    si.cb = വലുപ്പം(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    saAttr.nLength = വലുപ്പം(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ശൂന്യ;

    എങ്കിൽ (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	ifd[0] = job_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
	എങ്കിൽ (ifd[0] == INVALID_HANDLE_മൂല്യം)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    പോകുക failed;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!use_null_for_in
	    && (!create_pipe_pair(ifd)
		|| !SetHandleInformation(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	പോകുക failed;

    എങ്കിൽ (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	ofd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	എങ്കിൽ (ofd[1] == INVALID_HANDLE_മൂല്യം)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    പോകുക failed;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!use_null_for_out &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !SetHandleInformation(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	പോകുക failed;

    എങ്കിൽ (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	എങ്കിൽ (efd[1] == INVALID_HANDLE_മൂല്യം)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    പോകുക failed;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!use_out_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !SetHandleInformation(efd[0], HANDLE_FLAG_INHERIT, 0)))
	പോകുക failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_നിവേശം = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_out_for_err ? ofd[1] : efd[1];

    എങ്കിൽ (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	എങ്കിൽ (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    എങ്കിൽ (channel != ശൂന്യ)
		++channel->ch_refcount;
	}
	അല്ലെങ്കിൽ
	    channel = add_channel();
	എങ്കിൽ (channel == ശൂന്യ)
	    പോകുക failed;
    }

    എങ്കിൽ (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_MODE |
	    CREATE_NEW_PROCESS_GROUP |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_data,
	    (അക്ഷരം *)options->jo_cwd))
    {
	CloseHandle(jo);
	job->jv_status = JOB_FAILED;
	പോകുക failed;
    }

    ga_clear(&ga);

    എങ്കിൽ (!AssignProcessToJobObject(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = ശൂന്യ;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    job->jv_proc_info = pi;
    job->jv_job_object = jo;
    job->jv_status = JOB_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    എങ്കിൽ (!use_out_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    job->jv_channel = channel;
    എങ്കിൽ (channel != ശൂന്യ)
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
    തിരികെ;

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

    അക്ഷരം *
mch_job_status(job_T *job)
{
    DWORD dwExitCode = 0;

    എങ്കിൽ (!GetExitCodeProcess(job->jv_proc_info.hProcess, &dwExitCode)
	    || dwExitCode != STILL_ACTIVE)
    {
	job->jv_exitval = (പൂർണ്ണസംഖ്യ)dwExitCode;
	എങ്കിൽ (job->jv_status < JOB_ENDED)
	{
	    ch_log(job->jv_channel, "Job ended");
	    job->jv_status = JOB_ENDED;
	}
	തിരികെ "dead";
    }
    തിരികെ "run";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    HANDLE jobHandles[MAXIMUM_WAIT_OBJECTS];
    job_T *jobArray[MAXIMUM_WAIT_OBJECTS];
    job_T *job = job_list;

    ആകുമ്പോൾ (job != ശൂന്യ)
    {
	DWORD n;
	DWORD result;

	വേണ്ടി (n = 0; n < MAXIMUM_WAIT_OBJECTS
				       && job != ശൂന്യ; job = job->jv_next)
	{
	    എങ്കിൽ (job->jv_status == JOB_STARTED)
	    {
		jobHandles[n] = job->jv_proc_info.hProcess;
		jobArray[n] = job;
		++n;
	    }
	}
	എങ്കിൽ (n == 0)
	    തുടരുക;
	result = WaitForMultipleObjects(n, jobHandles, FALSE, 0);
	എങ്കിൽ (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + n)
	{
	    job_T *wait_job = jobArray[result - WAIT_OBJECT_0];

	    എങ്കിൽ (STRCMP(mch_job_status(wait_job), "dead") == 0)
		തിരികെ wait_job;
	}
    }
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം BOOL
terminate_all(HANDLE process, പൂർണ്ണസംഖ്യ code)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_മൂല്യം;
    DWORD	    pid = GetProcessId(process);

    എങ്കിൽ (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	എങ്കിൽ (h != INVALID_HANDLE_മൂല്യം)
	{
	    pe.dwSize = വലുപ്പം(PROCESSENTRY32);
	    എങ്കിൽ (!Process32First(h, &pe))
		പോകുക theend;

	    ചെയ്യുക
	    {
		എങ്കിൽ (pe.th32ParentProcessID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    എങ്കിൽ (ph != ശൂന്യ)
		    {
			terminate_all(ph, code);
			CloseHandle(ph);
		    }
		}
	    } ആകുമ്പോൾ (Process32Next(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    തിരികെ TerminateProcess(process, code);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    പൂർണ്ണസംഖ്യ
mch_signal_job(job_T *job, char_u *how)
{
    പൂർണ്ണസംഖ്യ ret;

    എങ്കിൽ (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	എങ്കിൽ (job->jv_job_object != ശൂന്യ)
	{
	    എങ്കിൽ (job->jv_channel != ശൂന്യ && job->jv_channel->ch_anonymous_pipe)
		job->jv_channel->ch_killing = TRUE;
	    തിരികെ TerminateJobObject(job->jv_job_object, (UINT)-1) ? OK : FAIL;
	}
	തിരികെ terminate_all(job->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    എങ്കിൽ (!AttachConsole(job->jv_proc_info.dwProcessId))
	തിരികെ FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		job->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    തിരികെ ret;
}

/*
 * Clear the data related to "job".
 */
    ശൂന്യം
mch_clear_job(job_T *job)
{
    എങ്കിൽ (job->jv_status == JOB_FAILED)
	തിരികെ;

    എങ്കിൽ (job->jv_job_object != ശൂന്യ)
	CloseHandle(job->jv_job_object);
    CloseHandle(job->jv_proc_info.hProcess);
}
#endif


#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    സ്ഥിതം ശൂന്യം
termcap_mode_start(ശൂന്യം)
{
    DWORD cmodein;

    എങ്കിൽ (g_fTermcapMode)
	തിരികെ;

    SaveConsoleBuffer(&g_cbNonTermcap);

    എങ്കിൽ (g_cbTermcap.IsValid)
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
    അല്ലെങ്കിൽ
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
    എങ്കിൽ (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_നിവേശം;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    അല്ലെങ്കിൽ
    {
	cmodein &= ~ENABLE_MOUSE_നിവേശം;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }
    cmodein |= ENABLE_WINDOW_നിവേശം;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_fTermcapMode = TRUE;
}


/*
 * End termcap mode
 */
    സ്ഥിതം ശൂന്യം
termcap_mode_end(ശൂന്യം)
{
    DWORD cmodein;
    ConsoleBuffer *cb;
    COORD coord;
    DWORD dwDummy;

    എങ്കിൽ (!g_fTermcapMode)
	തിരികെ;

    SaveConsoleBuffer(&g_cbTermcap);

    GetConsoleMode(g_hConIn, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_നിവേശം | ENABLE_WINDOW_നിവേശം);
    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# അല്ലെങ്കിൽ
    cb = &g_cbNonTermcap;
# endif
    RestoreConsoleBuffer(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    എങ്കിൽ (exiting && use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    എങ്കിൽ (!USE_WT && (p_rs || exiting))
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
	എങ്കിൽ (exiting && !p_rs)
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


#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    സ്ഥിതം ശൂന്യം
clear_chars(
    COORD coord,
    DWORD n)
{
    എങ്കിൽ (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_hConOut, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    അല്ലെങ്കിൽ
    {
	set_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    സ്ഥിതം ശൂന്യം
clear_screen(ശൂന്യം)
{
    g_coord.X = g_coord.Y = 0;

    എങ്കിൽ (!vtp_working)
	clear_chars(g_coord, Rows * Columns);
    അല്ലെങ്കിൽ
    {
	set_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    സ്ഥിതം ശൂന്യം
clear_to_end_of_display(ശൂന്യം)
{
    COORD save = g_coord;

    എങ്കിൽ (!vtp_working)
	clear_chars(g_coord, (Rows - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
clear_to_end_of_വരി(ശൂന്യം)
{
    COORD save = g_coord;

    എങ്കിൽ (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
scroll(unsigned c_വരികൾ)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.ഇടത്_3 + 1, g_srScrollRegion.മുകൾ_2 + 1);
    delete_വരികൾ(c_വരികൾ);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    സ്ഥിതം ശൂന്യം
set_scroll_region(
    unsigned ഇടത്,
    unsigned മുകൾ,
    unsigned വലത്,
    unsigned bottom)
{
    എങ്കിൽ (ഇടത് >= വലത്
	    || മുകൾ >= bottom
	    || വലത് > (unsigned) Columns - 1
	    || bottom > (unsigned) Rows - 1)
	തിരികെ;

    g_srScrollRegion.ഇടത്_3 =   ഇടത്;
    g_srScrollRegion.മുകൾ_2 =    മുകൾ;
    g_srScrollRegion.വലത്_3 =  വലത്;
    g_srScrollRegion.Bottom = bottom;
}

    സ്ഥിതം ശൂന്യം
set_scroll_region_tb(
    unsigned മുകൾ,
    unsigned bottom)
{
    എങ്കിൽ (മുകൾ >= bottom || bottom > (unsigned)Rows - 1)
	തിരികെ;

    g_srScrollRegion.മുകൾ_2 = മുകൾ;
    g_srScrollRegion.Bottom = bottom;
}

    സ്ഥിതം ശൂന്യം
set_scroll_region_lr(
    unsigned ഇടത്,
    unsigned വലത്)
{
    എങ്കിൽ (ഇടത് >= വലത് || വലത് > (unsigned)Columns - 1)
	തിരികെ;

    g_srScrollRegion.ഇടത്_3 = ഇടത്;
    g_srScrollRegion.വലത്_3 = വലത്;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    സ്ഥിതം ശൂന്യം
തിരുകുക_വരികൾ(unsigned c_വരികൾ)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.ഇടത്_3 + 1, g_srScrollRegion.മുകൾ_2 + 1);

    dest.X = g_srScrollRegion.ഇടത്_3;
    dest.Y = g_coord.Y + c_വരികൾ;

    source.ഇടത്_3   = g_srScrollRegion.ഇടത്_3;
    source.മുകൾ_2	  = g_coord.Y;
    source.വലത്_3  = g_srScrollRegion.വലത്_3;
    source.Bottom = g_srScrollRegion.Bottom - c_വരികൾ;

    clip.ഇടത്_3   = g_srScrollRegion.ഇടത്_3;
    clip.മുകൾ_2    = g_coord.Y;
    clip.വലത്_3  = g_srScrollRegion.വലത്_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    എങ്കിൽ (!USE_VTP)
	fill.Attributes = g_attrCurrent;
    അല്ലെങ്കിൽ
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    എങ്കിൽ (source.Bottom < dest.Y)
    {
	COORD coord;
	പൂർണ്ണസംഖ്യ   i;

	coord.X = source.ഇടത്_3;
	വേണ്ടി (i = clip.മുകൾ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.വലത്_3 - source.ഇടത്_3 + 1);
	}
    }

    എങ്കിൽ (vtp_working)
    {
	COORD coord;
	പൂർണ്ണസംഖ്യ i;

	coord.X = source.ഇടത്_3;
	വേണ്ടി (i = source.മുകൾ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.വലത്_3 - source.ഇടത്_3 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    സ്ഥിതം ശൂന്യം
delete_വരികൾ(unsigned c_വരികൾ)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    പൂർണ്ണസംഖ്യ		    nb;

    gotoxy(g_srScrollRegion.ഇടത്_3 + 1, g_srScrollRegion.മുകൾ_2 + 1);

    dest.X = g_srScrollRegion.ഇടത്_3;
    dest.Y = g_coord.Y;

    source.ഇടത്_3   = g_srScrollRegion.ഇടത്_3;
    source.മുകൾ_2	  = g_coord.Y + c_വരികൾ;
    source.വലത്_3  = g_srScrollRegion.വലത്_3;
    source.Bottom = g_srScrollRegion.Bottom;

    clip.ഇടത്_3   = g_srScrollRegion.ഇടത്_3;
    clip.മുകൾ_2    = g_coord.Y;
    clip.വലത്_3  = g_srScrollRegion.വലത്_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    എങ്കിൽ (!vtp_working)
	fill.Attributes = g_attrCurrent;
    അല്ലെങ്കിൽ
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (source.Bottom - source.മുകൾ_2) + 1;

    എങ്കിൽ (nb < source.മുകൾ_2)
    {
	COORD coord;
	പൂർണ്ണസംഖ്യ   i;

	coord.X = source.ഇടത്_3;
	വേണ്ടി (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.വലത്_3 - source.ഇടത്_3 + 1);
	}
    }

    എങ്കിൽ (vtp_working)
    {
	COORD coord;
	പൂർണ്ണസംഖ്യ i;

	coord.X = source.ഇടത്_3;
	വേണ്ടി (i = nb; i <= source.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.വലത്_3 - source.ഇടത്_3 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    സ്ഥിതം ശൂന്യം
gotoxy(
    unsigned x,
    unsigned y)
{
    എങ്കിൽ (x < 1 || x > (unsigned)Columns || y < 1 || y > (unsigned)Rows)
	തിരികെ;

    എങ്കിൽ (!USE_VTP)
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
    അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
textattr(WORD wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    SetConsoleTextAttribute(g_hConOut, wAttr);
}


    സ്ഥിതം ശൂന്യം
textcolor(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    എങ്കിൽ (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    അല്ലെങ്കിൽ
	vtp_sgr_bulk(wAttr);
}


    സ്ഥിതം ശൂന്യം
textbackground(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    എങ്കിൽ (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    അല്ലെങ്കിൽ
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    സ്ഥിതം ശൂന്യം
normvideo(ശൂന്യം)
{
    എങ്കിൽ (!vtp_working)
	textattr(g_attrDefault);
    അല്ലെങ്കിൽ
	vtp_sgr_bulk(0);
}


സ്ഥിതം WORD g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    സ്ഥിതം ശൂന്യം
standout(ശൂന്യം)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((WORD) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    സ്ഥിതം ശൂന്യം
standend(ശൂന്യം)
{
    എങ്കിൽ (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    ശൂന്യം
mch_set_normal_colors(ശൂന്യം)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		n;

    cterm_normal_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_normal_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    എങ്കിൽ (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	എങ്കിൽ (*p == 'm' && n > 0)
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
    സ്ഥിതം ശൂന്യം
visual_bell(ശൂന്യം)
{
    COORD   coordOrigin = {0, 0};
    WORD    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = ശൂന്യ;

# ifdef FEAT_TERMGUICOLORS
    എങ്കിൽ (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(WORD, Rows * Columns);
	എങ്കിൽ (oldattrs == ശൂന്യ)
	    തിരികെ;
	ReadConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_hConOut, attrFlash, Rows * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    എങ്കിൽ (oldattrs != ശൂന്യ)
    {
	WriteConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    സ്ഥിതം ശൂന്യം
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    എങ്കിൽ (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	തിരികെ;
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
    സ്ഥിതം DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cbToWrite)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    സ്ഥിതം DWORD    cells;
    സ്ഥിതം WCHAR    *unicodebuf = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    unibuflen = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    length;
    പൂർണ്ണസംഖ്യ		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    സ്ഥിതം WCHAR    *utf8spbuf = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    utf8splength;
    സ്ഥിതം DWORD    utf8spcells;
    സ്ഥിതം WCHAR    **utf8usingbuf = &unicodebuf;

    എങ്കിൽ (cbToWrite != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	ചെയ്യുക
	{
	    length = MultiByteToWideChar(cp, 0, (LPCSTR)pchBuf, cbToWrite,
							unicodebuf, unibuflen);
	    എങ്കിൽ (length && length <= unibuflen)
		നിർത്തുക;
	    vim_free(unicodebuf);
	    unicodebuf = length ? LALLOC_MULT(WCHAR, length) : ശൂന്യ;
	    unibuflen = unibuflen ? 0 : length;
	} ആകുമ്പോൾ (TRUE);
	cells = mb_string2cells(pchBuf, cbToWrite);
    }
    അല്ലെങ്കിൽ // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	എങ്കിൽ (utf8usingbuf != &utf8spbuf)
	{
	    എങ്കിൽ (utf8spbuf == ശൂന്യ)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		length = MultiByteToWideChar(CP_UTF8, 0, " ", 1, ശൂന്യ, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, length);
		എങ്കിൽ (utf8spbuf != ശൂന്യ)
		{
		    MultiByteToWideChar(CP_UTF8, 0, " ", 1, utf8spbuf, length);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = length;
		    utf8spcells = cells;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		utf8usingbuf = &utf8spbuf;
		length = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    എങ്കിൽ (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	എങ്കിൽ (WriteConsoleOutputCharacterW(g_hConOut, *utf8usingbuf, length,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (WriteConsoleW(g_hConOut, *utf8usingbuf, length, &cchwritten,
		    ശൂന്യ) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    എങ്കിൽ (cchwritten == (DWORD)length)
    {
	written = cbToWrite;
	g_coord.X += (SHORT)cells;
    }
    അല്ലെങ്കിൽ
    {
	char_u *p = pchBuf;
	വേണ്ടി (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    ആകുമ്പോൾ (g_coord.X > g_srScrollRegion.വലത്_3)
    {
	g_coord.X -= (SHORT) Columns;
	എങ്കിൽ (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    എങ്കിൽ (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    തിരികെ written;
}

    സ്ഥിതം char_u *
get_seq(
    പൂർണ്ണസംഖ്യ *args,
    പൂർണ്ണസംഖ്യ *എണ്ണം,
    char_u *head)
{
    പൂർണ്ണസംഖ്യ argc;
    char_u *p;

    എങ്കിൽ (head == ശൂന്യ || *head != '\033')
	തിരികെ ശൂന്യ;

    argc = 0;
    p = head;
    ++p;
    ചെയ്യുക
    {
	++p;
	args[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } ആകുമ്പോൾ (*p == ';');
    *എണ്ണം = argc;

    തിരികെ p;
}

    സ്ഥിതം char_u *
get_sgr(
    പൂർണ്ണസംഖ്യ *args,
    പൂർണ്ണസംഖ്യ *എണ്ണം,
    char_u *head)
{
    char_u *p = get_seq(args, എണ്ണം, head);

    തിരികെ (p && *p == 'm') ? ++p : ശൂന്യ;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    സ്ഥിതം char_u *
sgrn2(
    char_u *head,
    പൂർണ്ണസംഖ്യ	   n)
{
    പൂർണ്ണസംഖ്യ argc;
    പൂർണ്ണസംഖ്യ args[16];
    char_u *p = get_sgr(args, &argc, head);

    തിരികെ p && argc == 5 && args[0] == n && args[1] == 2 ? p : ശൂന്യ;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    സ്ഥിതം char_u *
sgrnc(
    char_u *head,
    പൂർണ്ണസംഖ്യ	   n)
{
    പൂർണ്ണസംഖ്യ argc;
    പൂർണ്ണസംഖ്യ args[16];
    char_u *p = get_sgr(args, &argc, head);

    തിരികെ p && argc == 1 && args[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : ശൂന്യ;
}

    സ്ഥിതം char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    ആകുമ്പോൾ (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    തിരികെ p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    സ്ഥിതം char_u *
sgrn2c(
    char_u *head,
    പൂർണ്ണസംഖ്യ	   n)
{
    char_u *p = sgrn2(head, n);

    തിരികെ p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : ശൂന്യ;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    സ്ഥിതം char_u *
sgrn2cn(
    char_u *head,
    പൂർണ്ണസംഖ്യ n)
{
    char_u *p = sgrn2(head, n);

    തിരികെ p && p[0] == 0x0a && p[1] == '\033' ? ++p : ശൂന്യ;
}

സ്ഥിതം bool in_dcs = false;	// inside a DCS (sixel) sequence
സ്ഥിതം bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    സ്ഥിതം ശൂന്യം
write_vt(char_u *s, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ശൂന്യ, 0);
    എങ്കിൽ (wlen <= 0)
	തിരികെ;
    ws = ALLOC_MULT(WCHAR, wlen);
    എങ്കിൽ (ws == ശൂന്യ)
	തിരികെ;
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_hConOut, ws, wlen, &written, ശൂന്യ);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    സ്ഥിതം char_u *
skip_dcs(char_u *s, char_u *end)
{
    വേണ്ടി (char_u *p = s; p < end; ++p)
    {
	എങ്കിൽ (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    in_dcs = false;
	    തിരികെ p + 1;
	}
	dcs_esc = *p == ESC;
    }
    തിരികെ end;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    ശൂന്യം
mch_write(
    char_u  *s,
    പൂർണ്ണസംഖ്യ	    len)
{
    char_u  *end = s + len;

# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
	തിരികെ;
# endif

    എങ്കിൽ (!term_console)
    {
	ലിഖ(1, s, (unsigned)len);
	തിരികെ;
    }

    // translate ESC | sequences into faked bios calls
    ആകുമ്പോൾ (len--)
    {
	പൂർണ്ണസംഖ്യ prefix = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	എങ്കിൽ (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    തിരികെ;
	}

	എങ്കിൽ (in_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    പൂർണ്ണസംഖ്യ	l = (പൂർണ്ണസംഖ്യ)(skip_dcs(s, end) - s);

	    എങ്കിൽ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    തുടരുക;
	}

	ആകുമ്പോൾ (s + ++prefix < end)
	{
	    ch = s[prefix];
	    എങ്കിൽ (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		നിർത്തുക;
	}

	എങ്കിൽ (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    എങ്കിൽ (prefix != 0)
		prefix = 1;
	}

	എങ്കിൽ (prefix != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, prefix);
# ifdef MCH_WRITE_DUMP
	    എങ്കിൽ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, വലുപ്പം(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    എങ്കിൽ (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.ഇടത്_3 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    അല്ലെങ്കിൽ
	    {
		gotoxy(g_srScrollRegion.ഇടത്_3 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    എങ്കിൽ (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.ഇടത്_3+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    എങ്കിൽ (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    എങ്കിൽ (g_coord.X > g_srScrollRegion.ഇടത്_3)
		g_coord.X--;
	    അല്ലെങ്കിൽ എങ്കിൽ (g_coord.Y > g_srScrollRegion.മുകൾ_2)
	    {
		g_coord.X = g_srScrollRegion.വലത്_3;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    എങ്കിൽ (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (s[0] == '\a')
	{
	    // \a, bell
	    MessageBeep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    എങ്കിൽ (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    പൂർണ്ണസംഖ്യ	    arg1 = 0, arg2 = 0, argc = 0, args[16];
	    char_u  *sp;

	    തിരഞ്ഞെടുപ്പ് (s[2])
	    {
	    അവസ്ഥ '0': അവസ്ഥ '1': അവസ്ഥ '2': അവസ്ഥ '3': അവസ്ഥ '4':
	    അവസ്ഥ '5': അവസ്ഥ '6': അവസ്ഥ '7': അവസ്ഥ '8': അവസ്ഥ '9':
		എങ്കിൽ (*(p = get_seq(args, &argc, s)) != 'm')
		    പോകുക notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		എങ്കിൽ (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (പൂർണ്ണസംഖ്യ)(p - s);
		    s = p;
		    നിർത്തുക;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		എങ്കിൽ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		എങ്കിൽ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		എങ്കിൽ (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		എങ്കിൽ (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(args, &argc, p);

notsgr:
		arg1 = args[0];
		arg2 = args[1];
		എങ്കിൽ (*p == 'm')
		{
		    എങ്കിൽ (argc == 1 && args[0] == 0)
			normvideo();
		    അല്ലെങ്കിൽ എങ്കിൽ (argc == 1)
		    {
			എങ്കിൽ (USE_VTP)
			    textcolor((WORD)arg1);
			അല്ലെങ്കിൽ
			    textattr((WORD)arg1);
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (vtp_working)
			vtp_sgr_bulks(argc, args);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argc == 2 && *p == 'r')
		{
		    set_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argc == 2 && *p == 'R')
		{
		    set_scroll_region_tb(arg1, arg2);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argc == 2 && *p == 'V')
		{
		    set_scroll_region_lr(arg1, arg2);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.മുകൾ_2, g_coord.Y - arg1) + 1);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argc == 1 && *p == 'b')
		{
		    textbackground((WORD) arg1);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.വലത്_3, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argc == 1 && *p == 'f')
		{
		    textcolor((WORD) arg1);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argc == 1 && *p == 'L')
		{
		    തിരുകുക_വരികൾ(arg1);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (argc == 1 && *p == 'M')
		{
		    delete_വരികൾ(arg1);
		}

		len -= (പൂർണ്ണസംഖ്യ)(p - s);
		s = p + 1;
		നിർത്തുക;

	    അവസ്ഥ 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.മുകൾ_2, g_coord.Y - 1) + 1);
		പോകുക got3;

	    അവസ്ഥ 'B':
		visual_bell();
		പോകുക got3;

	    അവസ്ഥ 'C':
		gotoxy(min(g_srScrollRegion.വലത്_3, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		പോകുക got3;

	    അവസ്ഥ 'E':
		termcap_mode_end();
		പോകുക got3;

	    അവസ്ഥ 'F':
		standout();
		പോകുക got3;

	    അവസ്ഥ 'f':
		standend();
		പോകുക got3;

	    അവസ്ഥ 'H':
		gotoxy(1, 1);
		പോകുക got3;

	    അവസ്ഥ 'j':
		clear_to_end_of_display();
		പോകുക got3;

	    അവസ്ഥ 'J':
		clear_screen();
		പോകുക got3;

	    അവസ്ഥ 'K':
		clear_to_end_of_വരി();
		പോകുക got3;

	    അവസ്ഥ 'L':
		തിരുകുക_വരികൾ(1);
		പോകുക got3;

	    അവസ്ഥ 'M':
		delete_വരികൾ(1);
		പോകുക got3;

	    അവസ്ഥ 'S':
		termcap_mode_start();
		പോകുക got3;

	    അവസ്ഥ 'V':
		cursor_visible(TRUE);
		പോകുക got3;

	    അവസ്ഥ 'v':
		cursor_visible(FALSE);
		പോകുക got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    എങ്കിൽ (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, വലുപ്പം(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    പൂർണ്ണസംഖ്യ l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    ആകുമ്പോൾ (s + l < end && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    എങ്കിൽ (s + l < end && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		എങ്കിൽ (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    പൂർണ്ണസംഖ്യ	l;

	    in_dcs = true;
	    dcs_esc = false;
	    l = (പൂർണ്ണസംഖ്യ)(skip_dcs(s + 2, end) - s);
	    എങ്കിൽ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	അല്ലെങ്കിൽ
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    എങ്കിൽ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, വലുപ്പം(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    എങ്കിൽ (fdDump)
	fflush(fdDump);
# endif
}

#അല്ലെങ്കിൽ // FEAT_GUI_MSWIN && !VIMDLL
    ശൂന്യം
mch_write(
    char_u  *s UNUSED,
    പൂർണ്ണസംഖ്യ	    len UNUSED)
{
    // never used
}

#endif


/*
 * Delay for "msec" milliseconds.
 */
    ശൂന്യം
mch_delay(
    long    msec,
    പൂർണ്ണസംഖ്യ	    flags UNUSED)
{
#എങ്കിൽ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((പൂർണ്ണസംഖ്യ)msec);	    // never wait for input
#അല്ലെങ്കിൽ // Console
# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
    {
	Sleep((പൂർണ്ണസംഖ്യ)msec);	    // never wait for input
	തിരികെ;
    }
# endif
    എങ്കിൽ (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	എങ്കിൽ (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    പൂർണ്ണസംഖ്യ towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    ആകുമ്പോൾ (msec > 0)
	    {
		mzvim_check_threads();
		എങ്കിൽ (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	അല്ലെങ്കിൽ
# endif
	    Sleep((പൂർണ്ണസംഖ്യ)msec);
    അല്ലെങ്കിൽ
	WaitForChar(msec, FALSE);
#endif
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    പൂർണ്ണസംഖ്യ
mch_remove(char_u *name)
{
    WCHAR	*wn;
    പൂർണ്ണസംഖ്യ		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    എങ്കിൽ (mch_isdir(name) && mch_is_symbolic_link(name))
	തിരികെ mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_NORMAL);

    wn = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (wn == ശൂന്യ)
	തിരികെ -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    തിരികെ n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    ശൂന്യം
mch_breakcheck(പൂർണ്ണസംഖ്യ force UNUSED)
{
#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    എങ്കിൽ (!gui.in_use)
# endif
	എങ്കിൽ (g_fCtrlCPressed || g_fCBrkPressed)
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
mch_total_mem(പൂർണ്ണസംഖ്യ special UNUSED)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dwLength = വലുപ്പം(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&ms);
    എങ്കിൽ (ms.ullAvailVirtual < ms.ullTotalPhys)
    {
	// Process address space fits in physical RAM, use all of it.
	തിരികെ (long_u)(ms.ullAvailVirtual / 1024);
    }
    എങ്കിൽ (ms.ullTotalPhys <= WINNT_RESERVE_BYTES)
    {
	// Catch old NT box or perverse hardware setup.
	തിരികെ (long_u)((ms.ullTotalPhys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    തിരികെ (long_u)((ms.ullTotalPhys - WINNT_RESERVE_BYTES) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    പൂർണ്ണസംഖ്യ
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    പൂർണ്ണസംഖ്യ		flags)		// EW_* flags
{
    തിരികെ dos_expandpath(gap, path, 0, flags, FALSE);
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
    പൂർണ്ണസംഖ്യ
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	szTempFile[_MAX_PATH + 1];
    WCHAR	szNewPath[_MAX_PATH + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    വേണ്ടി (p = q = wold; *p != L'\0'; ++p)
    {
	തിരഞ്ഞെടുപ്പ് (*p)
	{
	അവസ്ഥ L'/':
	അവസ്ഥ L'\\':
	അവസ്ഥ L':':
	    എങ്കിൽ (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)(p - q) < 8 || q[6] != L'~')
	തിരികെ (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    എങ്കിൽ (GetFullPathNameW(wnew, _MAX_PATH, szNewPath, &p) == 0 || p == ശൂന്യ)
	തിരികെ -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    എങ്കിൽ (GetTempFileNameW(szNewPath, L"VIM", 0, szTempFile) == 0)
	തിരികെ -2;

    // blow the temp file away
    എങ്കിൽ (!DeleteFileW(szTempFile))
	തിരികെ -3;

    // rename old file to the temp file
    എങ്കിൽ (!MoveFileW(wold, szTempFile))
	തിരികെ -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    എങ്കിൽ ((hf = CreateFileW(wold, GENERIC_WRITE, 0, ശൂന്യ, CREATE_NEW,
		    FILE_ATTRIBUTE_NORMAL, ശൂന്യ)) == INVALID_HANDLE_മൂല്യം)
	തിരികെ -5;
    എങ്കിൽ (!CloseHandle(hf))
	തിരികെ -6;

    // rename the temp file to the new file
    എങ്കിൽ (!MoveFileW(szTempFile, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(ശൂന്യം)MoveFileW(szTempFile, wold);
	തിരികെ -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(szTempFile);

    // finally, remove the empty old file
    എങ്കിൽ (!DeleteFileW(wold))
	തിരികെ -8;

    തിരികെ 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    പൂർണ്ണസംഖ്യ
mch_rename(
    സ്ഥിരം അക്ഷരം	*pszOldFile,
    സ്ഥിരം അക്ഷരം	*pszNewFile)
{
    WCHAR	*wold = ശൂന്യ;
    WCHAR	*wnew = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		retval = -1;

    wold = enc_to_utf16((char_u *)pszOldFile, ശൂന്യ);
    wnew = enc_to_utf16((char_u *)pszNewFile, ശൂന്യ);
    എങ്കിൽ (wold != ശൂന്യ && wnew != ശൂന്യ)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    തിരികെ retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    അക്ഷരം *
default_shell(ശൂന്യം)
{
    തിരികെ "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    പൂർണ്ണസംഖ്യ
mch_access(അക്ഷരം *n, പൂർണ്ണസംഖ്യ p)
{
    HANDLE	hFile;
    പൂർണ്ണസംഖ്യ		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_to_utf16((char_u *)n, ശൂന്യ);
    എങ്കിൽ (wn == ശൂന്യ)
	തിരികെ -1;

    എങ്കിൽ (mch_isdir((char_u *)n))
    {
	WCHAR TempNameW[_MAX_PATH + 16] = L"";

	എങ്കിൽ (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    പൂർണ്ണസംഖ്യ			i;
	    WIN32_FIND_DATAW    d;

	    വേണ്ടി (i = 0; i < _MAX_PATH && wn[i] != 0; ++i)
		TempNameW[i] = wn[i];
	    എങ്കിൽ (TempNameW[i - 1] != '\\' && TempNameW[i - 1] != '/')
		TempNameW[i++] = '\\';
	    TempNameW[i++] = '*';
	    TempNameW[i++] = 0;

	    hFile = FindFirstFileW(TempNameW, &d);
	    എങ്കിൽ (hFile == INVALID_HANDLE_മൂല്യം)
		പോകുക getout;
	    അല്ലെങ്കിൽ
		(ശൂന്യം)FindClose(hFile);
	}

	എങ്കിൽ (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    എങ്കിൽ (!GetTempFileNameW(wn, L"VIM", 0, TempNameW))
		പോകുക getout;
	    അല്ലെങ്കിൽ
		DeleteFileW(TempNameW);
	}
    }
    അല്ലെങ്കിൽ
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_mode = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_mode, share_mode,
					     ശൂന്യ, OPEN_EXISTING, 0, ശൂന്യ);
	എങ്കിൽ (hFile == INVALID_HANDLE_മൂല്യം)
	    പോകുക getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    തിരികെ retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    പൂർണ്ണസംഖ്യ
mch_open(സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ flags, പൂർണ്ണസംഖ്യ mode)
{
    WCHAR	*wn;
    പൂർണ്ണസംഖ്യ		f;

    wn = enc_to_utf16((char_u *)name, ശൂന്യ);
    എങ്കിൽ (wn == ശൂന്യ)
	തിരികെ -1;

    f = _wopen(wn, flags, mode);
    vim_free(wn);
    തിരികെ f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *mode)
{
    WCHAR	*wn, *wm;
    FILE	*f = ശൂന്യ;

#എങ്കിൽ defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    അക്ഷരം newMode = mode[strlen(mode) - 1];
    പൂർണ്ണസംഖ്യ oldMode = 0;

    _get_fmode(&oldMode);
    എങ്കിൽ (newMode == 't')
	_set_fmode(_O_TEXT);
    അല്ലെങ്കിൽ എങ്കിൽ (newMode == 'b')
	_set_fmode(O_ദ്വിമാന_2);
#endif
    wn = enc_to_utf16((char_u *)name, ശൂന്യ);
    wm = enc_to_utf16((char_u *)mode, ശൂന്യ);
    എങ്കിൽ (wn != ശൂന്യ && wm != ശൂന്യ)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#എങ്കിൽ defined(DEBUG) && _MSC_VER >= 1400
    എങ്കിൽ (oldMode != 0)
	_set_fmode(oldMode);
#endif
    തിരികെ f;
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
    സ്ഥിതം ശൂന്യം
copy_substream(HANDLE sh, ശൂന്യം *context, WCHAR *to, WCHAR *substream, long len)
{
    HANDLE  hTo;
    WCHAR   *to_name;

    to_name = alloc((wcslen(to) + wcslen(substream) + 1) * വലുപ്പം(WCHAR));
    എങ്കിൽ (to_name == ശൂന്യ)
	തിരികെ;

    wcscpy(to_name, to);
    wcscat(to_name, substream);

    hTo = CreateFileW(to_name, GENERIC_WRITE, 0, ശൂന്യ, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL, ശൂന്യ);
    എങ്കിൽ (hTo != INVALID_HANDLE_മൂല്യം)
    {
	long	done;
	DWORD	todo;
	DWORD	readcnt, written;
	അക്ഷരം	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	വേണ്ടി (done = 0; done < len; done += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - done) > വലുപ്പം(buf) ? വലുപ്പം(buf)
						       : (size_t)(len - done));
	    എങ്കിൽ (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(hTo, buf, todo, &written, ശൂന്യ)
		    || written != todo)
		നിർത്തുക;
	}
	CloseHandle(hTo);
    }

    സ്വതന്ത്രം(to_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    സ്ഥിതം ശൂന്യം
copy_infostreams(char_u *from, char_u *to)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_STREAM_ID	sid;
    പൂർണ്ണസംഖ്യ			headersize;
    WCHAR		streamname[_MAX_PATH];
    DWORD		readcount;
    ശൂന്യം		*context = ശൂന്യ;
    DWORD		lo, hi;
    പൂർണ്ണസംഖ്യ			len;

    // Convert the file names to wide characters.
    fromw = enc_to_utf16(from, ശൂന്യ);
    tow = enc_to_utf16(to, ശൂന്യ);
    എങ്കിൽ (fromw != ശൂന്യ && tow != ശൂന്യ)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, ശൂന്യ,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, ശൂന്യ);
	എങ്കിൽ (sh != INVALID_HANDLE_മൂല്യം)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    വേണ്ടി (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		ZeroMemory(&sid, വലുപ്പം(WIN32_STREAM_ID));
		headersize = (പൂർണ്ണസംഖ്യ)((അക്ഷരം *)&sid.cStreamName - (അക്ഷരം *)&sid.dwStreamId);
		എങ്കിൽ (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    നിർത്തുക;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		എങ്കിൽ (sid.dwStreamNameSize > 0)
		{
		    // Read the stream name.
		    എങ്കിൽ (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dwStreamNameSize,
					  &readcount, FALSE, FALSE, &context))
			നിർത്തുക;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / വലുപ്പം(WCHAR);
		    streamname[len] = 0;
		    എങ്കിൽ (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (long)sid.Size.u.താഴെയുള്ള_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(ശൂന്യം)BackupSeek(sh, sid.Size.u.താഴെയുള്ള_Part, sid.Size.u.മുകളിലെ_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (ശൂന്യം)BackupRead(sh, ശൂന്യ, 0, &readcount, TRUE, FALSE, &context);

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

തരനിർവചനം ഘടന _FILE_FULL_EA_INFORMATION_ {
    ULONG  NextEntryOffset;
    UCHAR  Flags;
    UCHAR  EaNameLength;
    USHORT Ea_മൂല്യം_Length;
    CHAR   EaName[1];
} FILE_FULL_EA_INFORMATION_, *PFILE_FULL_EA_INFORMATION_;

തരനിർവചനം ഘടന _FILE_EA_INFORMATION_ {
    ULONG EaSize;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#ifndef PROTO
തരനിർവചനം NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions);
തരനിർവചനം NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
തരനിർവചനം NTSTATUS (NTAPI *PfnNtSetEaFile)(
	HANDLE		    FileHandle,
	PIO_STATUS_BLOCK    IoStatusBlock,
	PVOID		    Buffer,
	ULONG		    Length);
തരനിർവചനം NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	BOOLEAN ReturnSingleEntry,
	PVOID EaList,
	ULONG EaListLength,
	PULONG EaIndex,
	BOOLEAN RestartScan);
തരനിർവചനം NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_STATUS_BLOCK	IoStatusBlock,
	PVOID			FileInformation,
	ULONG			Length,
	FILE_INFORMATION_CLASS FileInformationClass);
തരനിർവചനം VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString);

PfnNtOpenFile pNtOpenFile = ശൂന്യ;
PfnNtClose pNtClose = ശൂന്യ;
PfnNtSetEaFile pNtSetEaFile = ശൂന്യ;
PfnNtQueryEaFile pNtQueryEaFile = ശൂന്യ;
PfnNtQueryInformationFile pNtQueryInformationFile = ശൂന്യ;
PfnRtlInitUnicodeString pRtlInitUnicodeString = ശൂന്യ;
#endif

/*
 * Load ntdll.dll functions.
 */
    സ്ഥിതം BOOL
load_ntdll(ശൂന്യം)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	loaded = -1;

    എങ്കിൽ (loaded != -1)
	തിരികെ (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    എങ്കിൽ (hNtdll != ശൂന്യ)
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
    എങ്കിൽ (pNtOpenFile == ശൂന്യ
	    || pNtClose == ശൂന്യ
	    || pNtSetEaFile == ശൂന്യ
	    || pNtQueryEaFile == ശൂന്യ
	    || pNtQueryInformationFile == ശൂന്യ
	    || pRtlInitUnicodeString == ശൂന്യ)
	loaded = FALSE;
    അല്ലെങ്കിൽ
	loaded = TRUE;
    തിരികെ (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    സ്ഥിതം ശൂന്യം
copy_extattr(char_u *from, char_u *to)
{
    char_u		    *fromf = ശൂന്യ;
    char_u		    *tof = ശൂന്യ;
    WCHAR		    *fromw = ശൂന്യ;
    WCHAR		    *tow = ശൂന്യ;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_ATTRIBUTES	    oa;
    IO_STATUS_BLOCK	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    ശൂന്യം		    *ea = ശൂന്യ;

    എങ്കിൽ (!load_ntdll())
	തിരികെ;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(to) + 5);
    എങ്കിൽ (fromf == ശൂന്യ || tof == ശൂന്യ)
	പോകുക theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), to);

    // Convert the names to wide characters.
    fromw = enc_to_utf16(fromf, ശൂന്യ);
    tow = enc_to_utf16(tof, ശൂന്യ);
    എങ്കിൽ (fromw == ശൂന്യ || tow == ശൂന്യ)
	പോകുക theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    InitializeObjectAttributes(&oa, &u, 0, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	പോകുക theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, വലുപ്പം(eainfo),
	    FileEaInformation);
    എങ്കിൽ (eainfo.EaSize != 0)
    {
	ea = alloc(eainfo.EaSize);
	എങ്കിൽ (ea != ശൂന്യ)
	{
	    എങ്കിൽ (pNtQueryEaFile(h, &iosb, ea, eainfo.EaSize, FALSE,
			ശൂന്യ, 0, ശൂന്യ, TRUE) != STATUS_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    എങ്കിൽ (ea != ശൂന്യ)
    {
	pRtlInitUnicodeString(&u, tow);
	InitializeObjectAttributes(&oa, &u, 0, ശൂന്യ, ശൂന്യ);
	എങ്കിൽ (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	    പോകുക theend;

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
    പൂർണ്ണസംഖ്യ
mch_copy_file_attribute(char_u *from, char_u *to)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, to);
    copy_extattr(from, to);
    തിരികെ 0;
}


/*
 * The command line arguments in UTF-16
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ	nArgsW = 0;
സ്ഥിതം LPWSTR	*ArglistW = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ	global_argc = 0;
സ്ഥിതം അക്ഷരം	**global_argv;

സ്ഥിതം പൂർണ്ണസംഖ്യ	used_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
സ്ഥിതം പൂർണ്ണസംഖ്യ	*used_file_indexes = ശൂന്യ; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
സ്ഥിതം പൂർണ്ണസംഖ്യ	used_file_എണ്ണം = 0;	// nr of entries in used_file_indexes
സ്ഥിതം പൂർണ്ണസംഖ്യ	used_file_literal = FALSE;  // take file names literally
സ്ഥിതം പൂർണ്ണസംഖ്യ	used_file_full_path = FALSE;  // file name was full path
#ifdef FEAT_DIFF
സ്ഥിതം പൂർണ്ണസംഖ്യ	used_file_diff_mode = FALSE;  // file name was with diff mode
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ	used_alist_എണ്ണം = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    പൂർണ്ണസംഖ്യ
get_cmd_argsW(അക്ഷരം ***argvp)
{
    അക്ഷരം	**argv = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		argc = 0;
    പൂർണ്ണസംഖ്യ		i;

    free_cmd_argsW();
    ArglistW = Command_വരി_To_Argv_W(Get_Command_വരി_W(), &nArgsW);
    എങ്കിൽ (ArglistW != ശൂന്യ)
    {
	argv = alloc((nArgsW + 1) * വലുപ്പം(അക്ഷരം *));
	എങ്കിൽ (argv != ശൂന്യ)
	{
	    argc = nArgsW;
	    argv[argc] = ശൂന്യ;
	    വേണ്ടി (i = 0; i < argc; ++i)
	    {
		പൂർണ്ണസംഖ്യ	len;

		// Convert each Unicode argument to UTF-8.
		WideCharToMultiByte_alloc(CP_UTF8, 0,
				ArglistW[i], (പൂർണ്ണസംഖ്യ)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		എങ്കിൽ (argv[i] == ശൂന്യ)
		{
		    // Out of memory, clear everything.
		    ആകുമ്പോൾ (i > 0)
			സ്വതന്ത്രം(argv[--i]);
		    സ്വതന്ത്രം(argv);
		    argv = ശൂന്യ;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    എങ്കിൽ (argc > 0)
    {
	എങ്കിൽ (used_file_indexes != ശൂന്യ)
	    സ്വതന്ത്രം(used_file_indexes);
	used_file_indexes = alloc(argc * വലുപ്പം(പൂർണ്ണസംഖ്യ));
    }

    എങ്കിൽ (argvp != ശൂന്യ)
	*argvp = argv;
    തിരികെ argc;
}

    ശൂന്യം
free_cmd_argsW(ശൂന്യം)
{
    എങ്കിൽ (ArglistW == ശൂന്യ)
	തിരികെ;

    GlobalFree(ArglistW);
    ArglistW = ശൂന്യ;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    ശൂന്യം
used_file_arg(അക്ഷരം *name, പൂർണ്ണസംഖ്യ literal, പൂർണ്ണസംഖ്യ full_path, പൂർണ്ണസംഖ്യ diff_mode)
{
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (used_file_indexes == ശൂന്യ)
	തിരികെ;
    വേണ്ടി (i = used_file_argc + 1; i < global_argc; ++i)
	എങ്കിൽ (STRCMP(global_argv[i], name) == 0)
	{
	    used_file_argc = i;
	    used_file_indexes[used_file_എണ്ണം++] = i;
	    നിർത്തുക;
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
    ശൂന്യം
set_alist_എണ്ണം(ശൂന്യം)
{
    used_alist_എണ്ണം = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    ശൂന്യം
fix_arg_enc(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		idx;
    char_u	*str;
    പൂർണ്ണസംഖ്യ		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    എങ്കിൽ (global_argc != nArgsW
	    || ArglistW == ശൂന്യ
	    || used_file_indexes == ശൂന്യ
	    || used_file_എണ്ണം == 0
	    || used_alist_എണ്ണം != GARGCOUNT)
	തിരികെ;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(പൂർണ്ണസംഖ്യ, GARGCOUNT);
    എങ്കിൽ (fnum_list == ശൂന്യ)
	തിരികെ;		// out of memory
    വേണ്ടി (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    എങ്കിൽ (ga_grow(&global_alist.al_ga, used_file_എണ്ണം) == FAIL)
	തിരികെ;		// out of memory

    വേണ്ടി (i = 0; i < used_file_എണ്ണം; ++i)
    {
	idx = used_file_indexes[i];
	str = utf16_to_enc(ArglistW[idx], ശൂന്യ);
	എങ്കിൽ (str != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ literal = used_file_literal;

#ifdef FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    എങ്കിൽ (used_file_diff_mode && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), tail, STRLEN(tail), TRUE, &ret);
		എങ്കിൽ (ret.string != ശൂന്യ)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#endif
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    എങ്കിൽ (used_file_literal)
		buf_set_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    എങ്കിൽ (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		എങ്കിൽ (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_add(&global_alist, str, literal ? 2 : 0);
	}
    }

    എങ്കിൽ (!used_file_literal)
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
	alist_expand(fnum_list, used_alist_എണ്ണം);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    എങ്കിൽ (curwin->w_arg_idx == 0 && curbuf->b_fname == ശൂന്യ)
    {
	do_cmdline_cmd((char_u *)":rewind");
	എങ്കിൽ (GARGCOUNT == 1 && used_file_full_path
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    last_chdir_reason = "drop";
    }

    set_alist_എണ്ണം();
}

    പൂർണ്ണസംഖ്യ
mch_setenv(അക്ഷരം *var, അക്ഷരം *മൂല്യം, പൂർണ്ണസംഖ്യ x UNUSED)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(മൂല്യം) + 2);
    എങ്കിൽ (envbuf == ശൂന്യ)
	തിരികെ -1;

    sprintf((അക്ഷരം *)envbuf, "%s=%s", var, മൂല്യം);

    p = enc_to_utf16(envbuf, ശൂന്യ);

    vim_free(envbuf);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ -1;
    _wputenv(p);
#ifdef libintl_wputenv
    libintl_wputenv(p);
#endif
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    തിരികെ 0;
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

    സ്ഥിതം ശൂന്യം
vtp_flag_init(ശൂന്യം)
{
#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   mode;
    HANDLE  out;

# ifdef VIMDLL
    എങ്കിൽ (!gui.in_use)
# endif
    {
	out = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_version >= VTP_FIRST_SUPPORT_BUILD) ? 1 : 0;
	GetConsoleMode(out, &mode);
	mode |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	എങ്കിൽ (SetConsoleMode(out, mode) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_buffer = win_version >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#endif

    എങ്കിൽ (win_version >= CONPTY_FIRST_SUPPORT_BUILD)
	conpty_working = 1;
    എങ്കിൽ (win_version >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    എങ്കിൽ (win_version <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    എങ്കിൽ (win_version <= CONPTY_1909_BUILD)
	conpty_type = 2;
    എങ്കിൽ (win_version <= CONPTY_1903_BUILD)
	conpty_type = 2;
    എങ്കിൽ (win_version < CONPTY_FIRST_SUPPORT_BUILD)
	conpty_type = 1;

    എങ്കിൽ (win_version >= CONPTY_NEXT_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    സ്ഥിതം ശൂന്യം
vtp_init(ശൂന്യം)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = വലുപ്പം(csbi);
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

    സ്ഥിതം ശൂന്യം
vtp_exit(ശൂന്യം)
{
    restore_console_color_rgb();
}

    പൂർണ്ണസംഖ്യ
vtp_printf(
    അക്ഷരം *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   result;
    പൂർണ്ണസംഖ്യ	    len;

    എങ്കിൽ (silent_mode)
	തിരികെ 0;

    va_start(list, format);
    len = vim_vsnprintf((അക്ഷരം *)buf, 100, (അക്ഷരം *)format, list);
    va_end(list);
    WriteConsoleA(g_hConOut, buf, (DWORD)len, &result, ശൂന്യ);
    തിരികെ (പൂർണ്ണസംഖ്യ)result;
}

    സ്ഥിതം ശൂന്യം
vtp_sgr_bulk(
    പൂർണ്ണസംഖ്യ arg)
{
    പൂർണ്ണസംഖ്യ args[1];

    args[0] = arg;
    vtp_sgr_bulks(1, args);
}

# define FAST256(x) \
    എങ്കിൽ ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    അവസ്ഥ x: \
	FAST256(newargs[x - 1]);

    സ്ഥിതം ശൂന്യം
vtp_sgr_bulks(
    പൂർണ്ണസംഖ്യ argc,
    പൂർണ്ണസംഖ്യ *args)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    in, out;
    പൂർണ്ണസംഖ്യ	    newargs[16];
    സ്ഥിതം പൂർണ്ണസംഖ്യ sgrfgr = -1, sgrfgg, sgrfgb;
    സ്ഥിതം പൂർണ്ണസംഖ്യ sgrbgr = -1, sgrbgg, sgrbgb;

    എങ്കിൽ (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	തിരികെ;
    }

    in = out = 0;
    ആകുമ്പോൾ (in < argc)
    {
	പൂർണ്ണസംഖ്യ s = args[in];
	പൂർണ്ണസംഖ്യ copylen = 1;

	എങ്കിൽ (s == 38)
	{
	    എങ്കിൽ (argc - in >= 5 && args[in + 1] == 2)
	    {
		എങ്കിൽ (sgrfgr == args[in + 2] && sgrfgg == args[in + 3]
						     && sgrfgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		അല്ലെങ്കിൽ
		{
		    sgrfgr = args[in + 2];
		    sgrfgg = args[in + 3];
		    sgrfgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (s == 48)
	{
	    എങ്കിൽ (argc - in >= 5 && args[in + 1] == 2)
	    {
		എങ്കിൽ (sgrbgr == args[in + 2] && sgrbgg == args[in + 3]
						     && sgrbgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		അല്ലെങ്കിൽ
		{
		    sgrbgr = args[in + 2];
		    sgrbgg = args[in + 3];
		    sgrbgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (30 <= s && s <= 39)
	    sgrfgr = -1;
	അല്ലെങ്കിൽ എങ്കിൽ (90 <= s && s <= 97)
	    sgrfgr = -1;
	അല്ലെങ്കിൽ എങ്കിൽ (40 <= s && s <= 49)
	    sgrbgr = -1;
	അല്ലെങ്കിൽ എങ്കിൽ (100 <= s && s <= 107)
	    sgrbgr = -1;
	അല്ലെങ്കിൽ എങ്കിൽ (s == 0)
	    sgrfgr = sgrbgr = -1;

	ആകുമ്പോൾ (copylen--)
	    newargs[out++] = args[in++];
    }

    p = &buf[വലുപ്പം(buf) - 1];
    *p-- = 'm';

    തിരഞ്ഞെടുപ്പ് (out)
    {
	പൂർണ്ണസംഖ്യ	n, m;
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
	WriteConsoleA(g_hConOut, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, ശൂന്യ);
    സ്ഥിരസ്ഥിതി:
	നിർത്തുക;
    }
}

    സ്ഥിതം ശൂന്യം
wt_init(ശൂന്യം)
{
    wt_working = mch_getenv("WT_SESSION") != ശൂന്യ;
}

# ifdef FEAT_TERMGUICOLORS
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ctermtoxterm(
    പൂർണ്ണസംഖ്യ cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    തിരികെ (((പൂർണ്ണസംഖ്യ)r << 16) | ((പൂർണ്ണസംഖ്യ)g << 8) | (പൂർണ്ണസംഖ്യ)b);
}
# endif

    സ്ഥിതം ശൂന്യം
set_console_color_rgb(ശൂന്യം)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    guicolor_T	fg, bg;
    പൂർണ്ണസംഖ്യ		ctermfg, ctermbg;

    എങ്കിൽ (!vtp_working)
	തിരികെ;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    എങ്കിൽ (p_tgc || t_colors >= 256)
    {
	term_fg_rgb_color(fg);
	term_bg_rgb_color(bg);
	തിരികെ;
    }

    എങ്കിൽ (use_alternate_screen_buffer)
	തിരികെ;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cbSize = വലുപ്പം(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = വലുപ്പം(csbi);
    csbi.srWindow.വലത്_3 += 1;
    csbi.srWindow.Bottom += 1;
    store_console_bg_rgb = csbi.ColorTable[g_color_index_bg];
    store_console_fg_rgb = csbi.ColorTable[g_color_index_fg];
    csbi.ColorTable[g_color_index_bg] = (COLORREF)bg;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)fg;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

# എങ്കിൽ defined(FEAT_TERMGUICOLORS)
    ശൂന്യം
get_default_console_color(
    പൂർണ്ണസംഖ്യ *cterm_fg,
    പൂർണ്ണസംഖ്യ *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    പൂർണ്ണസംഖ്യ id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    പൂർണ്ണസംഖ്യ ctermfg = 0;
    പൂർണ്ണസംഖ്യ ctermbg = 0;
    പൂർണ്ണസംഖ്യ dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    എങ്കിൽ (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    എങ്കിൽ (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	എങ്കിൽ (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	എങ്കിൽ (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	അല്ലെങ്കിൽ
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_normal_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    എങ്കിൽ (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	എങ്കിൽ (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	എങ്കിൽ (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
reset_console_color_rgb(ശൂന്യം)
{
# ifdef FEAT_TERMGUICOLORS
    എങ്കിൽ (use_alternate_screen_buffer)
	തിരികെ;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = വലുപ്പം(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = വലുപ്പം(csbi);
    csbi.srWindow.വലത്_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)store_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)store_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    സ്ഥിതം ശൂന്യം
restore_console_color_rgb(ശൂന്യം)
{
# ifdef FEAT_TERMGUICOLORS
    എങ്കിൽ (use_alternate_screen_buffer)
	തിരികെ;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = വലുപ്പം(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = വലുപ്പം(csbi);
    csbi.srWindow.വലത്_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)save_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)save_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

    ശൂന്യം
control_console_color_rgb(ശൂന്യം)
{
    എങ്കിൽ (vtp_working)
	set_console_color_rgb();
    അല്ലെങ്കിൽ
	reset_console_color_rgb();
}

    പൂർണ്ണസംഖ്യ
use_vtp(ശൂന്യം)
{
    തിരികെ USE_VTP;
}

    പൂർണ്ണസംഖ്യ
is_term_win32(ശൂന്യം)
{
    തിരികെ T_NAME != ശൂന്യ && STRCMP(T_NAME, "win32") == 0;
}

    പൂർണ്ണസംഖ്യ
has_vtp_working(ശൂന്യം)
{
    തിരികെ vtp_working;
}

#endif

    പൂർണ്ണസംഖ്യ
has_conpty_working(ശൂന്യം)
{
    തിരികെ conpty_working;
}

    പൂർണ്ണസംഖ്യ
get_conpty_type(ശൂന്യം)
{
    തിരികെ conpty_type;
}

    പൂർണ്ണസംഖ്യ
is_conpty_stable(ശൂന്യം)
{
    തിരികെ conpty_stable;
}

    പൂർണ്ണസംഖ്യ
get_conpty_fix_type(ശൂന്യം)
{
    തിരികെ conpty_fix_type;
}

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ശൂന്യം
resize_console_buf(ശൂന്യം)
{
    എങ്കിൽ (use_alternate_screen_buffer)
	തിരികെ;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    എങ്കിൽ (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	തിരികെ;

    coord.X = SRWIDTH(csbi.srWindow);
    coord.Y = SRHEIGHT(csbi.srWindow);
    SetConsoleScreenBufferSize(g_hConOut, coord);

    newsize.ഇടത്_3 = 0;
    newsize.മുകൾ_2 = 0;
    newsize.വലത്_3 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    SetConsoleWindowInfo(g_hConOut, TRUE, &newsize);

    SetConsoleScreenBufferSize(g_hConOut, coord);
}
#endif

    അക്ഷരം *
GetWin32Error(ശൂന്യം)
{
    സ്ഥിതം അക്ഷരം	*oldmsg = ശൂന്യ;
    അക്ഷരം	*acp_സന്ദേശം = ശൂന്യ;
    DWORD	acp_len;
    അക്ഷരം	*enc_സന്ദേശം = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		enc_len = 0;

    // get formatted message from OS
    acp_len = FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
	    ശൂന്യ, GetLastError(), 0, (LPSTR)&acp_സന്ദേശം, 0, ശൂന്യ);
    എങ്കിൽ (acp_len == 0 || acp_സന്ദേശം == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // clean oldmsg if remained.
    എങ്കിൽ (oldmsg != ശൂന്യ)
    {
	vim_free(oldmsg);
	oldmsg = ശൂന്യ;
    }

    acp_to_enc((char_u *)acp_സന്ദേശം, (പൂർണ്ണസംഖ്യ)acp_len, (char_u **)&enc_സന്ദേശം, &enc_len);
    LocalFree(acp_സന്ദേശം);
    എങ്കിൽ (enc_സന്ദേശം == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // remove trailing \r\n
    അക്ഷരം *pcrlf = strstr(enc_സന്ദേശം, "\r\n");
    എങ്കിൽ (pcrlf != ശൂന്യ)
	*pcrlf = NUL;

    oldmsg = enc_സന്ദേശം;
    തിരികെ enc_സന്ദേശം;
}

#എങ്കിൽ defined(FEAT_RELTIME)
സ്ഥിതം HANDLE   timer_handle;
സ്ഥിതം പൂർണ്ണസംഖ്യ      timer_active = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
സ്ഥിതം sig_atomic_t timeout_flags[2];
സ്ഥിതം പൂർണ്ണസംഖ്യ	    timeout_flag_idx = 0;
സ്ഥിതം sig_atomic_t *timeout_flag = &timeout_flags[0];


    സ്ഥിതം ശൂന്യം CALLBACK
set_flag(ശൂന്യം *param, BOOLEAN unused2 UNUSED)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    ശൂന്യം
stop_timeout(ശൂന്യം)
{
    എങ്കിൽ (timer_active)
    {
	BOOL ret = DeleteTimerQueueTimer(ശൂന്യ, timer_handle, ശൂന്യ);
	timer_active = FALSE;
	എങ്കിൽ (!ret && GetLastError() != ERROR_IO_PENDING)
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
    അസ്ഥിര sig_atomic_t *
start_timeout(long msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = CreateTimerQueueTimer(
	    &timer_handle, ശൂന്യ, set_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    എങ്കിൽ (!ret)
    {
	semsg(_(e_could_not_set_timeout_str), GetWin32Error());
    }
    അല്ലെങ്കിൽ
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_active = TRUE;
	*timeout_flag = FALSE;
    }
    തിരികെ timeout_flag;
}
#endif
