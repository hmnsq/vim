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

#अन्तर्भाव "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#अन्तर्भाव <sys/types.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <limits.h>

#अन्तर्भाव <process.h>
#अन्तर्भाव <winternl.h>
#अन्तर्भाव <direct.h>

#जर !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#ifdef FEAT_JOB_CHANNEL
# include <tlhelp32.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
खाता* fdDump = रिक्त;
#endif

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
स्थिर HANDLE g_hConIn  = INVALID_HANDLE_मूल्य;
स्थिर HANDLE g_hConOut = INVALID_HANDLE_मूल्य;

// Win32 Screen buffer,coordinate,console I/O information
स्थिर SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
स्थिर COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
स्थिर WORD  g_attrDefault = 7;  // lightgray text on black background
स्थिर WORD  g_attrCurrent;

स्थिर पूर्णांक g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
स्थिर पूर्णांक g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
स्थिर पूर्णांक g_fForceExit = FALSE;    // set when forcefully exiting

स्थिर शून्य scroll(अचिन्हित c_ओळी);
स्थिर शून्य set_scroll_region(अचिन्हित डावा, अचिन्हित शीर्ष,
			      अचिन्हित उजवा, अचिन्हित bottom);
स्थिर शून्य set_scroll_region_tb(अचिन्हित शीर्ष, अचिन्हित bottom);
स्थिर शून्य set_scroll_region_lr(अचिन्हित डावा, अचिन्हित उजवा);
स्थिर शून्य घाला_ओळी(अचिन्हित c_ओळी);
स्थिर शून्य delete_ओळी(अचिन्हित c_ओळी);
स्थिर शून्य gotoxy(अचिन्हित x, अचिन्हित y);
स्थिर शून्य standout(शून्य);
स्थिर पूर्णांक s_cursor_visible = TRUE;
स्थिर पूर्णांक did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
प्रकारव्याख्या संरचना आदान_record_buffer_गाठ_S
{
    आदान_RECORD ir;
    संरचना आदान_record_buffer_गाठ_S *next;
} आदान_record_buffer_गाठ_T;
प्रकारव्याख्या संरचना आदान_record_buffer_S
{
    आदान_record_buffer_गाठ_T *head;
    आदान_record_buffer_गाठ_T *tail;
    पूर्णांक length;
} आदान_record_buffer_T;
स्थिर आदान_record_buffer_T आदान_record_buffer;
स्थिर पूर्णांक read_आदान_record_buffer(आदान_RECORD* irEvents, पूर्णांक nMaxLength);
स्थिर पूर्णांक write_आदान_record_buffer(आदान_RECORD* irEvents, पूर्णांक nLength);
#endif
#ifdef FEAT_GUI_MSWIN
स्थिर पूर्णांक s_dont_use_vimrun = TRUE;
स्थिर पूर्णांक need_vimrun_warning = FALSE;
स्थिर string_T vimrun_path = {(char_u *)"vimrun ", 7};
स्थिर पूर्णांक vimrun_path_allocated = FALSE;
#endif

स्थिर पूर्णांक win32_getattrs(char_u *name);
स्थिर पूर्णांक win32_setattrs(char_u *name, पूर्णांक attrs);
स्थिर पूर्णांक win32_set_archive(char_u *name);

स्थिर पूर्णांक conpty_working = 0;
स्थिर पूर्णांक conpty_type = 0;
स्थिर पूर्णांक conpty_stable = 0;
स्थिर पूर्णांक conpty_fix_type = 0;
स्थिर शून्य vtp_flag_init(शून्य);

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
स्थिर पूर्णांक vtp_working = 0;
स्थिर शून्य vtp_init(शून्य);
स्थिर शून्य vtp_exit(शून्य);
स्थिर शून्य vtp_sgr_bulk(पूर्णांक arg);
स्थिर शून्य vtp_sgr_bulks(पूर्णांक argc, पूर्णांक *argv);

स्थिर पूर्णांक wt_working = 0;
स्थिर शून्य wt_init(शून्य);

स्थिर पूर्णांक g_color_index_bg = 0;
स्थिर पूर्णांक g_color_index_fg = 7;

# ifdef FEAT_TERMGUICOLORS
स्थिर guicolor_T save_console_bg_rgb;
स्थिर guicolor_T save_console_fg_rgb;
स्थिर guicolor_T store_console_bg_rgb;
स्थिर guicolor_T store_console_fg_rgb;
स्थिर पूर्णांक default_console_color_bg = 0x000000; // black
स्थिर पूर्णांक default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_term_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# अन्यथा
#  define USE_VTP		0
#  define USE_WT		0
# endif

स्थिर शून्य set_console_color_rgb(शून्य);
स्थिर शून्य reset_console_color_rgb(शून्य);
स्थिर शून्य restore_console_color_rgb(शून्य);
#endif  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
स्थिर पूर्णांक suppress_winsize = 1;	// don't fiddle with console
#endif

स्थिर WCHAR *exe_pathw = रिक्त;

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
स्थिर BOOL use_alternate_screen_buffer = FALSE;
#endif

बाह्य DWORD win_version; // this is in os_mswin.c

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    स्थिर BOOL
is_ambiwidth_event(
    आदान_RECORD *ir)
{
    परत ir->EventType == किल्ली_EVENT
		&& ir->Event.किल्ली_Event_2.b_किल्ली_Down
		&& ir->Event.किल्ली_Event_2.w_Repeat_मोजणी == 1
		&& ir->Event.किल्ली_Event_2.w_Virtual_किल्ली_Code == 0x12
		&& ir->Event.किल्ली_Event_2.wVirtualScanCode == 0x38
		&& ir->Event.किल्ली_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.किल्ली_Event_2.dw_Control_किल्ली_State == 2;
}

    स्थिर शून्य
make_ambiwidth_event(
    आदान_RECORD *down,
    आदान_RECORD *up)
{
    down->Event.किल्ली_Event_2.w_Virtual_किल्ली_Code = 0;
    down->Event.किल्ली_Event_2.wVirtualScanCode = 0;
    down->Event.किल्ली_Event_2.uChar.UnicodeChar
				    = up->Event.किल्ली_Event_2.uChar.UnicodeChar;
    down->Event.किल्ली_Event_2.dw_Control_किल्ली_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    स्थिर BOOL
read_console_आदान(
    HANDLE	    h_आदान,
    आदान_RECORD    *lpBuffer,
    पूर्णांक		    nLength,
    LPDWORD	    lpEvents)
{
    गणना
    {
	IRSIZE = 10
    };
    स्थिर आदान_RECORD s_irCache[IRSIZE];
    स्थिर DWORD s_dwIndex = 0;
    स्थिर DWORD s_dwMax = 0;
    DWORD dwEvents;
    पूर्णांक head;
    पूर्णांक tail;
    पूर्णांक i;
    स्थिर आदान_RECORD s_irPseudo;

    जर (s_dwMax == 0 && आदान_record_buffer.length > 0)
    {
	dwEvents = read_आदान_record_buffer(s_irCache, IRSIZE);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
    }

    जर (nLength == -2)
	परत (s_dwMax > 0) ? TRUE : FALSE;

    जर (win_version < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	जर (nLength == -1)
	    परत Peek_Console_आदान_W(h_आदान, lpBuffer, 1, lpEvents);
	परत Read_Console_आदान_W(h_आदान, lpBuffer, 1, &dwEvents);
    }

    जर (s_dwMax == 0)
    {
	जर (!vtp_working && nLength == -1)
	    परत Peek_Console_आदान_W(h_आदान, lpBuffer, 1, lpEvents);
	Get_संख्या_Of_Console_आदान_Events(h_आदान, &dwEvents);
	जर (dwEvents == 0 && nLength == -1)
	    परत Peek_Console_आदान_W(h_आदान, lpBuffer, 1, lpEvents);
	Read_Console_आदान_W(h_आदान, s_irCache, IRSIZE, &dwEvents);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
	जर (dwEvents == 0)
	{
	    *lpEvents = 0;
	    परत TRUE;
	}

	साठी (i = s_dwIndex; i < (पूर्णांक)s_dwMax - 1; ++i)
	    जर (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	जर (s_dwMax > 1)
	{
	    head = 0;
	    tail = s_dwMax - 1;
	    पर्यंत (head != tail)
	    {
		जर (s_irCache[head].EventType == WINDOW_BUFFER_SIZE_EVENT
			&& s_irCache[head + 1].EventType
						  == WINDOW_BUFFER_SIZE_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    साठी (i = head; i < tail; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --tail;
		    सुरूठेवा;
		}
		head++;
	    }
	    s_dwMax = tail + 1;
	}
    }

    जर (s_irCache[s_dwIndex].EventType == किल्ली_EVENT)
    {
	जर (s_irCache[s_dwIndex].Event.किल्ली_Event_2.w_Repeat_मोजणी > 1)
	{
	    s_irPseudo = s_irCache[s_dwIndex];
	    s_irPseudo.Event.किल्ली_Event_2.w_Repeat_मोजणी = 1;
	    s_irCache[s_dwIndex].Event.किल्ली_Event_2.w_Repeat_मोजणी--;
	    *lpBuffer = s_irPseudo;
	    *lpEvents = 1;
	    परत TRUE;
	}
    }

    *lpBuffer = s_irCache[s_dwIndex];
    जर (!(nLength == -1 || nLength == -2) && ++s_dwIndex >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    परत TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    स्थिर BOOL
peek_console_आदान(
    HANDLE	    h_आदान,
    आदान_RECORD    *lpBuffer,
    DWORD	    nLength UNUSED,
    LPDWORD	    lpEvents)
{
    परत read_console_आदान(h_आदान, lpBuffer, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    स्थिर DWORD
संदेश_wait_for_multiple_objects(
    DWORD    n_मोजणी,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dwWakeMask)
{
    जर (read_console_आदान(रिक्त, रिक्त, -2, रिक्त))
	परत WAIT_OBJECT_0;
    परत संदेश_Wait_For_Multiple_Objects(n_मोजणी, pHandles, fWaitAll,
				     dwMilliseconds, dwWakeMask);
}
# endif

# ifndef FEAT_CLIENTSERVER
    स्थिर DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    जर (read_console_आदान(रिक्त, रिक्त, -2, रिक्त))
	परत WAIT_OBJECT_0;
    परत WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#endif   // !FEAT_GUI_MSWIN || VIMDLL

    शून्य
mch_get_exe_name(शून्य)
{
    पूर्णांक		updated = FALSE;
    स्थिर पूर्णांक	enc_prev = -1;
    WCHAR	*path;
    माप_प्रकार	pathlen;

    जर (exe_name == रिक्त || exe_pathw == रिक्त || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_PATH];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(रिक्त, buf, MAX_PATH);
	जर (p > buf)
	{
	    जर (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_to_enc(buf, रिक्त);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    करा
	    {
		--p;
		जर (*p == L'\\')
		{
		    *p = L'\0';
		    थांबा;
		}
	    } पर्यंत (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    जर (exe_pathw == रिक्त || !updated)
	परत;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    path = _wgetenv(L"PATH");
    pathlen = (path == रिक्त) ? 0 : wcslen(path);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#define EXTRA_LEN 7
#define MAX_ENV_STRING_LEN 8191

    जर (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   temp[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	माप_प्रकार  templen = 5;

	जर (pathlen == 0)
	    wcscpy(temp + templen, exe_pathw);
	अन्यथा
	{
	    wcscpy(temp + templen, path);

	    // Check if exe_path is already included in $PATH.
	    जर (wcsstr(temp, exe_pathw) == रिक्त)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		जर (temp[templen - 1] != L';')
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
    स्थिर शून्य
unescape_shellxquote(char_u *p, char_u *escaped)
{
    पूर्णांक	    l = (पूर्णांक)STRLEN(p);
    पूर्णांक	    n;

    पर्यंत (*p != NUL)
    {
	जर (*p == '^' && vim_strchr(escaped, p[1]) != रिक्त)
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
vimLoadLib(स्थिरांक अक्षर *name)
{
    HINSTANCE	dll = रिक्त;

    // No need to load any library when registering OLE.
    जर (found_register_arg)
	परत रिक्त;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    जर (exe_pathw == रिक्त)
    {
	mch_get_exe_name();
	जर (exe_pathw == रिक्त)
	    परत रिक्त;
    }

    WCHAR old_dirw[MAXPATHL];

    जर (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	परत रिक्त;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    SetCurrentDirectoryW(exe_pathw);
    dll = LoadLibrary(name);
    SetCurrentDirectoryW(old_dirw);
    परत dll;
}

#जर defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    पूर्णांक
mch_is_gui_executable(शून्य)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(रिक्त);
    PIMAGE_DOS_HEADER	pDOS = (PIMAGE_DOS_HEADER)pImage;
    PIMAGE_NT_HEADERS	pPE;

    जर (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	परत FALSE;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    जर (pPE->Signature != IMAGE_NT_SIGNATURE)
	परत FALSE;
    जर (pPE->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	परत TRUE;
    परत FALSE;
}
#endif

#जर defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    स्थिर शून्य *
get_imported_func_info(HINSTANCE hInst, स्थिरांक अक्षर *funcname, पूर्णांक info,
	स्थिरांक शून्य *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_HEADER		pDOS = (PIMAGE_DOS_HEADER)hInst;
    PIMAGE_NT_HEADERS		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_DATA		pIAT;	    // Import Address Table
    PIMAGE_THUNK_DATA		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    जर (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	परत रिक्त;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    जर (pPE->Signature != IMAGE_NT_SIGNATURE)
	परत रिक्त;

    ImpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    जर (ImpVA == 0)
	परत रिक्त;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    साठी (; pImpDesc->FirstThunk; ++pImpDesc)
    {
	जर (!pImpDesc->OriginalFirstThunk)
	    सुरूठेवा;
	pIAT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->FirstThunk);
	pINT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->OriginalFirstThunk);
	साठी (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    जर (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		सुरूठेवा;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.AddressOfData));
	    जर (म_भेद((अक्षर *)pImpName->Name, funcname) == 0)
	    {
		शून्य *original;
		DWORD old, new = PAGE_READWRITE;

		निवड (info)
		{
		    स्थिती 0:
			परत (शून्य *)pIAT->u1.Function;
		    स्थिती 1:
			परत (शून्य *)(pImage + pImpDesc->Name);
		    स्थिती 2:
			original = (शून्य *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, आकार(शून्य *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, आकार(शून्य *),
				old, &new);
			परत original;
		    पूर्वनिर्धारित:
			परत रिक्त;
		}
	    }
	}
    }
    परत रिक्त;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
find_imported_module_by_funcname(HINSTANCE hInst, स्थिरांक अक्षर *funcname)
{
    अक्षर    *modulename;

    modulename = (अक्षर *)get_imported_func_info(hInst, funcname, 1, रिक्त);
    जर (modulename != रिक्त)
	परत GetModuleHandleA(modulename);
    परत रिक्त;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    शून्य *
get_dll_import_func(HINSTANCE hInst, स्थिरांक अक्षर *funcname)
{
    परत get_imported_func_info(hInst, funcname, 0, रिक्त);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    शून्य *
hook_dll_import_func(HINSTANCE hInst, स्थिरांक अक्षर *funcname, स्थिरांक शून्य *hook)
{
    परत get_imported_func_info(hInst, funcname, 2, hook);
}
#endif

#जर defined(FEAT_PYTHON3)
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

    जर (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	परत रिक्त;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    जर (pPE->Signature != IMAGE_NT_SIGNATURE)
	परत रिक्त;

    ExpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    ExpSize = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    जर (ExpVA == 0)
	परत hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    pFunctionTable = (LPDWORD)(pImage + pExpDir->AddressOfFunctions);

    जर (pExpDir->संख्या_Of_Names == 0)
	परत hInst;	// No export names.

    // Check only the first entry.
    जर ((pFunctionTable[0] < ExpVA) || (pFunctionTable[0] >= ExpVA + ExpSize))
	// The first entry is not a function forwarder.
	परत hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    स्थिरांक अक्षर *name = (स्थिरांक अक्षर *)(pImage + pFunctionTable[0]);
    स्थिरांक अक्षर *p = म_अक्षर(name, '.');
    जर (p == रिक्त)
	परत hInst;

    // Extract DllName.
    अक्षर buf[MAX_PATH];
    जर (p - name + 1 > आकार(buf))
	परत रिक्त;
    म_नकलन(buf, name, p - name);
    buf[p - name] = NUL;
    परत GetModuleHandleA(buf);
}
#endif

#जर defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
स्थिर अक्षर *null_libintl_gettext(स्थिरांक अक्षर *);
स्थिर अक्षर *null_libintl_ngettext(स्थिरांक अक्षर *, स्थिरांक अक्षर *, अचिन्हित दीर्घ n);
स्थिर अक्षर *null_libintl_textdomain(स्थिरांक अक्षर *);
स्थिर अक्षर *null_libintl_bindtextdomain(स्थिरांक अक्षर *, स्थिरांक अक्षर *);
स्थिर अक्षर *null_libintl_bind_textdomain_codeset(स्थिरांक अक्षर *, स्थिरांक अक्षर *);
स्थिर पूर्णांक null_libintl_wputenv(स्थिरांक ब_अक्षर_प्रकार *);

स्थिर HINSTANCE hLibintlDLL = रिक्त;
अक्षर *(*dyn_libintl_gettext)(स्थिरांक अक्षर *) = null_libintl_gettext;
अक्षर *(*dyn_libintl_ngettext)(स्थिरांक अक्षर *, स्थिरांक अक्षर *, अचिन्हित दीर्घ n)
						= null_libintl_ngettext;
अक्षर *(*dyn_libintl_textdomain)(स्थिरांक अक्षर *) = null_libintl_textdomain;
अक्षर *(*dyn_libintl_bindtextdomain)(स्थिरांक अक्षर *, स्थिरांक अक्षर *)
						= null_libintl_bindtextdomain;
अक्षर *(*dyn_libintl_bind_textdomain_codeset)(स्थिरांक अक्षर *, स्थिरांक अक्षर *)
				       = null_libintl_bind_textdomain_codeset;
पूर्णांक (*dyn_libintl_wputenv)(स्थिरांक ब_अक्षर_प्रकार *) = null_libintl_wputenv;
पूर्णांक *dyn_libintl_nl_संदेश_cat_cntr = रिक्त;

    पूर्णांक
dyn_libintl_init(शून्य)
{
    पूर्णांक i;
    स्थिर संरचना
    {
	अक्षर	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{रिक्त, रिक्त}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    जर (hLibintlDLL != रिक्त)
	परत 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vimLoadLib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    जर (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    जर (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT2);
# endif
    जर (!hLibintlDLL)
    {
	जर (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_load_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	परत 0;
    }
    साठी (i = 0; libintl_entry[i].name != रिक्त
					 && libintl_entry[i].ptr != रिक्त; ++i)
    {
	जर ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == रिक्त)
	{
	    dyn_libintl_end();
	    जर (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_load_library_function_str), libintl_entry[i].name);
		verbose_leave();
	    }
	    परत 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (अक्षर *(*)(स्थिरांक अक्षर *, स्थिरांक अक्षर *))
			GetProcAddress(hLibintlDLL, "bind_textdomain_codeset");
    जर (dyn_libintl_bind_textdomain_codeset == रिक्त)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = find_imported_module_by_funcname(hLibintlDLL, "getenv");
    जर (hmsvcrt != रिक्त)
	dyn_libintl_wputenv = (पूर्णांक (*)(स्थिरांक ब_अक्षर_प्रकार *))
					GetProcAddress(hmsvcrt, "_wputenv");
    जर (dyn_libintl_wputenv == रिक्त || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_संदेश_cat_cntr = (पूर्णांक *)GetProcAddress(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    परत 1;
}

    शून्य
dyn_libintl_end(शून्य)
{
    जर (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= रिक्त;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_संदेश_cat_cntr	= रिक्त;
}

    स्थिर अक्षर *
null_libintl_gettext(स्थिरांक अक्षर *msgid)
{
    परत (अक्षर*)msgid;
}

    स्थिर अक्षर *
null_libintl_ngettext(
	स्थिरांक अक्षर *msgid,
	स्थिरांक अक्षर *msgid_plural,
	अचिन्हित दीर्घ n)
{
    परत (अक्षर *)(n == 1 ? msgid : msgid_plural);
}

    स्थिर अक्षर *
null_libintl_bindtextdomain(
	स्थिरांक अक्षर *domainname UNUSED,
	स्थिरांक अक्षर *स_नाम UNUSED)
{
    परत रिक्त;
}

    स्थिर अक्षर *
null_libintl_bind_textdomain_codeset(
	स्थिरांक अक्षर *domainname UNUSED,
	स्थिरांक अक्षर *codeset UNUSED)
{
    परत रिक्त;
}

    स्थिर अक्षर *
null_libintl_textdomain(स्थिरांक अक्षर *domainname UNUSED)
{
    परत रिक्त;
}

    स्थिर पूर्णांक
null_libintl_wputenv(स्थिरांक ब_अक्षर_प्रकार *envstring UNUSED)
{
    परत 0;
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
    स्थिर BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    जर (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	परत FALSE;

    जर (!Lookup_Privilege_मूल्य(रिक्त, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	परत FALSE;
    }

    tokenPrivileges.Privilege_मोजणी	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    जर (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, रिक्त, रिक्त))
    {
	CloseHandle(hToken);
	परत FALSE;
    }

    जर (GetLastError() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	परत FALSE;
    }

    CloseHandle(hToken);
    परत TRUE;
}
#endif

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    शून्य
PlatformId(शून्य)
{
#ifdef FEAT_EVAL
    DWORD major = (win_version >> 24) & 0xFF;
    DWORD minor = (win_version >> 16) & 0xFF;
    vim_snprintf(windowsVersion, आकार(windowsVersion), "%d.%d", major, minor);
#endif

#ifdef HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    जर (!win32_enable_privilege(SE_SECURITY_NAME))
	परत;
#endif
}

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SHIFT  (SHIFT_PRESSED)
# define CTRL   (उजवा_CTRL_PRESSED | डावा_CTRL_PRESSED)
# define ALT    (उजवा_ALT_PRESSED  | डावा_ALT_PRESSED)
# define ALT_GR (उजवा_ALT_PRESSED  | डावा_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

स्थिर स्थिरांक संरचना
{
    WORD    w_Virt_किल्ली;
    BOOL    f_Ansi_किल्ली;
    पूर्णांक	    chAlone;
    पूर्णांक	    chShift;
    पूर्णांक	    chCtrl;
    पूर्णांक	    chAlt;
} Virt_किल्ली_Map[] =
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
    { VK_डावा,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_उजवा,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_END,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_DOWN,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_NEXT,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_घाला,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_BACK,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# जर 0
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
    स्थिर पूर्णांक
win32_kbd_patch_किल्ली(
    किल्ली_EVENT_RECORD *pker)
{
    UINT uMods = pker->dw_Control_किल्ली_State;
    स्थिर पूर्णांक s_iIsDead = 0;
    स्थिर WORD awAnsiCode[2];
    स्थिर BYTE abKeystate[256];

    जर (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[1];
	s_iIsDead = 0;
	परत 1;
    }

    // check if it already has a valid unicode character.
    जर (pker->uChar.UnicodeChar != 0)
	परत 1;

    CLEAR_FIELD(abKeystate);

    // Clear any pending dead keys
    ToUnicode(VK_SPACE, Map_Virtual_किल्ली(VK_SPACE, 0), abKeystate, awAnsiCode, 2, 0);

    जर (uMods & SHIFT_PRESSED)
	abKeystate[VK_SHIFT] = 0x80;
    जर (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    जर ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ToUnicode(pker->w_Virtual_किल्ली_Code, pker->wVirtualScanCode,
			abKeystate, awAnsiCode, 2, 0);

    जर (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[0];

    परत s_iIsDead;
}

स्थिर BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    स्थिर BOOL
decode_किल्ली_event(
    किल्ली_EVENT_RECORD	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    पूर्णांक			*pmodifiers,
    BOOL		fDoPost UNUSED)
{
    पूर्णांक i;
    स्थिरांक पूर्णांक nModifs = pker->dw_Control_किल्ली_State & (SHIFT | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    जर (!pker->b_किल्ली_Down)
	परत FALSE;

    // ignore some keystrokes
    निवड (pker->w_Virtual_किल्ली_Code)
    {
    // modifiers
    स्थिती VK_SHIFT:
    स्थिती VK_CONTROL:
    स्थिती VK_MENU:   // Alt key
	परत FALSE;

    पूर्वनिर्धारित:
	थांबा;
    }

    // Shift-TAB
    जर (pker->w_Virtual_किल्ली_Code == VK_TAB && (nModifs & SHIFT_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	परत TRUE;
    }

    साठी (i = ARRAY_LENGTH(Virt_किल्ली_Map); --i >= 0; )
    {
	जर (Virt_किल्ली_Map[i].w_Virt_किल्ली == pker->w_Virtual_किल्ली_Code)
	{
	    *pch = Virt_किल्ली_Map[i].chAlone;
	    जर ((nModifs & SHIFT) != 0)
		*pch = Virt_किल्ली_Map[i].chShift;
	    अन्यथा जर ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_किल्ली_Map[i].chCtrl;
	    अन्यथा जर ((nModifs & ALT) != 0)
		*pch = Virt_किल्ली_Map[i].chAlt;

	    जर (*pch != 0)
	    {
		जर (Virt_किल्ली_Map[i].f_Ansi_किल्ली)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    जर (pmodifiers)
		    {
			जर (pker->w_Virtual_किल्ली_Code >= VK_F1
			    && pker->w_Virtual_किल्ली_Code <= VK_F12)
			{
			    जर ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_MASK_ALT;
				जर ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_किल्ली_Map[i].chAlone;
			    }
			    जर ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				जर ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_किल्ली_Map[i].chAlone;
			    }
			}
			अन्यथा जर (pker->w_Virtual_किल्ली_Code >= VK_END
				&& pker->w_Virtual_किल्ली_Code <= VK_DOWN)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_किल्ली_Map[i].chAlone;
			    जर ((nModifs & SHIFT) != 0
						    && (nModifs & ~SHIFT) == 0)
			    {
				*pch2 = Virt_किल्ली_Map[i].chShift;
			    }
			    जर ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_किल्ली_Map[i].chCtrl;
				जर (pker->w_Virtual_किल्ली_Code == VK_UP
				    || pker->w_Virtual_किल्ली_Code == VK_DOWN)
				{
				    *pmodifiers |= MOD_MASK_CTRL;
				    *pch2 = Virt_किल्ली_Map[i].chAlone;
				}
			    }
			    जर ((nModifs & SHIFT) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				*pch2 = Virt_किल्ली_Map[i].chShift;
			    }
			    जर ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_किल्ली_Map[i].chAlt;
				*pmodifiers |= MOD_MASK_ALT;
				जर ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_किल्ली_Map[i].chAlone;
				}
				अन्यथा जर ((nModifs & SHIFT) != 0)
				{
				    *pch2 = Virt_किल्ली_Map[i].chShift;
				}
				अन्यथा जर ((nModifs & CTRL) != 0)
				{
				    जर (pker->w_Virtual_किल्ली_Code == VK_UP
					|| pker->w_Virtual_किल्ली_Code == VK_DOWN)
				    {
					*pmodifiers |= MOD_MASK_CTRL;
					*pch2 = Virt_किल्ली_Map[i].chAlone;
				    }
				    अन्यथा
				    {
					*pch2 = Virt_किल्ली_Map[i].chCtrl;
				    }
				}
			    }
			}
			अन्यथा जर (pker->w_Virtual_किल्ली_Code == VK_घाला
					&& (nModifs & SHIFT) != 0
					&& (nModifs & ~SHIFT) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_किल्ली_Map[i].chShift;
			}
			अन्यथा
			{
			    *pch2 = Virt_किल्ली_Map[i].chAlone;
			    जर ((nModifs & SHIFT) != 0)
				*pmodifiers |= MOD_MASK_SHIFT;
			    जर ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_MASK_CTRL;
			    जर ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_MASK_ALT;
			}
		    }
		}

		परत TRUE;
	    }
	}
    }

    i = win32_kbd_patch_किल्ली(pker);

    जर (i < 0)
	*pch = NUL;
    अन्यथा
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	जर (pmodifiers != रिक्त)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    जर ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_MASK_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    जर ((nModifs & SHIFT) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_MASK_SHIFT;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    जर ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_MASK_CTRL;
	}
    }

    परत (*pch != NUL);
}

# जर defined(FEAT_EVAL)
    स्थिर पूर्णांक
encode_किल्ली_event(dict_T *args, आदान_RECORD *ir)
{
    स्थिर पूर्णांक s_dwMods = 0;

    char_u *action = dict_get_string(args, "event", TRUE);
    जर (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_किल्ली_Down = STRICMP(action, "keydown") == 0;
	WORD vkCode = dict_get_संख्या_def(args, "keycode", 0);
	जर (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (दीर्घ)vkCode);
	    परत FALSE;
	}

	ir->EventType = किल्ली_EVENT;
	किल्ली_EVENT_RECORD ker;
	ZeroMemory(&ker, आकार(ker));
	ker.b_किल्ली_Down = is_किल्ली_Down;
	ker.w_Repeat_मोजणी = 1;
	ker.wVirtualScanCode = 0;
	ker.dw_Control_किल्ली_State = 0;
	पूर्णांक mods = (पूर्णांक)dict_get_संख्या(args, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	जर (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    जर (mods & MOD_MASK_SHIFT)
		ker.dw_Control_किल्ली_State |= SHIFT_PRESSED;
	    जर (mods & MOD_MASK_CTRL)
		ker.dw_Control_किल्ली_State |= डावा_CTRL_PRESSED;
	    जर (mods & MOD_MASK_ALT)
		ker.dw_Control_किल्ली_State |= डावा_ALT_PRESSED;
	}

	जर (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT)
	{
	    जर (is_किल्ली_Down)
		s_dwMods |= SHIFT_PRESSED;
	    अन्यथा
		s_dwMods &= ~SHIFT_PRESSED;
	}
	अन्यथा जर (vkCode == VK_LCONTROL || vkCode == VK_CONTROL)
	{
	    जर (is_किल्ली_Down)
		s_dwMods |= डावा_CTRL_PRESSED;
	    अन्यथा
		s_dwMods &= ~डावा_CTRL_PRESSED;
	}
	अन्यथा जर (vkCode == VK_RCONTROL)
	{
	    जर (is_किल्ली_Down)
		s_dwMods |= उजवा_CTRL_PRESSED;
	    अन्यथा
		s_dwMods &= ~उजवा_CTRL_PRESSED;
	}
	अन्यथा जर (vkCode == VK_LMENU || vkCode == VK_MENU)
	{
	    जर (is_किल्ली_Down)
		s_dwMods |= डावा_ALT_PRESSED;
	    अन्यथा
		s_dwMods &= ~डावा_ALT_PRESSED;
	}
	अन्यथा जर (vkCode == VK_RMENU)
	{
	    जर (is_किल्ली_Down)
		s_dwMods |= उजवा_ALT_PRESSED;
	    अन्यथा
		s_dwMods &= ~उजवा_ALT_PRESSED;
	}
	ker.dw_Control_किल्ली_State |= s_dwMods;
	ker.w_Virtual_किल्ली_Code = vkCode;
	ker.uChar.UnicodeChar = 0;
	ir->Event.किल्ली_Event_2 = ker;
	vim_free(action);
    }
    अन्यथा
    {
	जर (action == रिक्त)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	अन्यथा
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str_str), "event", action);
	    vim_free(action);
	}
	परत FALSE;
    }
    परत TRUE;
}
# endif  // FEAT_EVAL
#endif // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
स्थिर पूर्णांक g_fMouseAvail = FALSE;   // mouse present
स्थिर पूर्णांक g_fMouseActive = FALSE;  // mouse enabled
स्थिर पूर्णांक g_nMouseClick = -1;	    // mouse status
स्थिर पूर्णांक g_xMouse;		    // mouse x coordinate
स्थिर पूर्णांक g_yMouse;		    // mouse y coordinate
स्थिर DWORD g_cmodein = 0;	    // Original console input mode
स्थिर DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    शून्य
mch_setmouse(पूर्णांक on)
{
    DWORD cmodein;

# ifdef VIMDLL
    जर (gui.in_use)
	परत;
# endif
    जर (!g_fMouseAvail)
	परत;

    g_fMouseActive = on;
    GetConsoleMode(g_hConIn, &cmodein);

    जर (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_आदान;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    अन्यथा
    {
	cmodein &= ~ENABLE_MOUSE_आदान;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }

    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
}


# जर defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    शून्य
mch_bevalterm_changed(शून्य)
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
    स्थिर शून्य
decode_mouse_wheel(MOUSE_EVENT_RECORD *pmer)
{
    पूर्णांक	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    पूर्णांक	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    पूर्णांक lcol = g_xMouse;
    पूर्णांक lrow = g_yMouse;
    win_T *wp = mouse_find_win(&lrow, &lcol, FIND_POPUP);
    जर (wp != रिक्त && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_FIELD(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	जर (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_डावा : MSCR_उजवा;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	अन्यथा
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
    जर (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    अन्यथा
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    जर (pmer->dw_Control_किल्ली_State & SHIFT_PRESSED)
	modifiers |= MOD_MASK_SHIFT; // MOUSE_SHIFT;
    जर (pmer->dw_Control_किल्ली_State & (उजवा_CTRL_PRESSED | डावा_CTRL_PRESSED))
	modifiers |= MOD_MASK_CTRL; // MOUSE_CTRL;
    जर (pmer->dw_Control_किल्ली_State & (उजवा_ALT_PRESSED  | डावा_ALT_PRESSED))
	modifiers |= MOD_MASK_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    परत;
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
    स्थिर BOOL
decode_mouse_event(
    MOUSE_EVENT_RECORD *pmer)
{
    स्थिर पूर्णांक s_nOldButton = -1;
    स्थिर पूर्णांक s_nOldMouseClick = -1;
    स्थिर पूर्णांक s_xOldMouse = -1;
    स्थिर पूर्णांक s_yOldMouse = -1;
    स्थिर linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    स्थिर पूर्णांक s_old_topfill = 0;
# endif
    स्थिर पूर्णांक s_cClicks = 1;
    स्थिर BOOL s_fReleased = TRUE;
    स्थिर DWORD s_dwLastClickTime = 0;
    स्थिर BOOL s_f_Next_Is_मधला = FALSE;

    स्थिर DWORD cButtons = 0;	// number of buttons supported

    स्थिरांक DWORD डावा_2 = FROM_डावा_1_ST_BUTTON_PRESSED;
    स्थिरांक DWORD मधला = FROM_डावा_2_ND_BUTTON_PRESSED;
    स्थिरांक DWORD उजवा_2 = RIGHTMOST_BUTTON_PRESSED;
    स्थिरांक DWORD डावा_उजवा = डावा_2 | उजवा_2;

    पूर्णांक nButton;

    जर (cButtons == 0 && !Get_संख्या_Of_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    जर (!g_fMouseAvail || !g_fMouseActive)
    {
	g_nMouseClick = -1;
	परत FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    जर (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	परत FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    जर (g_nMouseClick != -1)
	परत TRUE;

    जर (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	परत TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    जर (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	जर (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    परत FALSE;
    }

    // If no buttons are pressed...
    जर ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	जर (s_fReleased)
	{
# ifdef FEAT_BEVAL_TERM
	    // do return mouse move events when we want them
	    जर (p_bevalterm)
		nButton = MOUSE_DRAG;
	    अन्यथा
# endif
		परत FALSE;
	}

	s_fReleased = TRUE;
    }
    अन्यथा    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	जर (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & डावा_उजवा);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    जर (dwLR == डावा_2 || dwLR == उजवा_2)
	    {
		साठी (;;)
		{
		    // wait a short time for next input event
		    जर (WaitForSingleObject(g_hConIn, p_mouset / 3)
							     != WAIT_OBJECT_0)
			थांबा;
		    अन्यथा
		    {
			DWORD cRecords = 0;
			आदान_RECORD ir;
			MOUSE_EVENT_RECORD* pmer2 = &ir.Event.MouseEvent;

			peek_console_आदान(g_hConIn, &ir, 1, &cRecords);

			जर (cRecords == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & डावा_उजवा))
			    थांबा;
			अन्यथा
			{
			    जर (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_आदान(g_hConIn, &ir, 1, &cRecords);

				परत decode_mouse_event(pmer2);
			    }
			    अन्यथा जर (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_आदान(g_hConIn, &ir, 1, &cRecords);

				// are there any more mouse events in queue?
				peek_console_आदान(g_hConIn, &ir, 1, &cRecords);

				जर (cRecords==0 || ir.EventType != MOUSE_EVENT)
				    थांबा;
			    }
			    अन्यथा
				थांबा;
			}
		    }
		}
	    }
	}

	जर (s_f_Next_Is_मधला)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_मधला;
	    s_f_Next_Is_मधला = FALSE;
	}
	अन्यथा जर (cButtons == 2	&&
	    ((pmer->dwButtonState & डावा_उजवा) == डावा_उजवा))
	{
	    nButton = MOUSE_मधला;

	    जर (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_Next_Is_मधला = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	अन्यथा जर ((pmer->dwButtonState & डावा_2) == डावा_2)
	    nButton = MOUSE_डावा;
	अन्यथा जर ((pmer->dwButtonState & मधला) == मधला)
	    nButton = MOUSE_मधला;
	अन्यथा जर ((pmer->dwButtonState & उजवा_2) == उजवा_2)
	    nButton = MOUSE_उजवा;

	जर (! s_fReleased && ! s_f_Next_Is_मधला
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    परत FALSE;

	s_fReleased = s_f_Next_Is_मधला;
    }

    जर (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	जर (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dwCurrentTime = Get_Tick_मोजणी();

	    जर (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (पूर्णांक)(dwCurrentTime - s_dwLastClickTime) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    अन्यथा जर (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dwLastClickTime = dwCurrentTime;
	}
    }
    अन्यथा जर (pmer->dwEventFlags == MOUSE_MOVED)
    {
	जर (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    जर (nButton == -1)
	परत FALSE;

    जर (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    जर (pmer->dw_Control_किल्ली_State & SHIFT_PRESSED)
	g_nMouseClick |= MOUSE_SHIFT;
    जर (pmer->dw_Control_किल्ली_State & (उजवा_CTRL_PRESSED | डावा_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    जर (pmer->dw_Control_किल्ली_State & (उजवा_ALT_PRESSED  | डावा_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    जर (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	SET_अंक_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    जर (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	परत FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    परत TRUE;
}

# ifdef FEAT_EVAL
    स्थिर पूर्णांक
encode_mouse_event(dict_T *args, आदान_RECORD *ir)
{
    पूर्णांक		button;
    पूर्णांक		row;
    पूर्णांक		col;
    पूर्णांक		repeated_click;
    int_u	mods = 0;
    पूर्णांक		move;

    जर (!dict_has_किल्ली(args, "row") || !dict_has_किल्ली(args, "col"))
	परत FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (पूर्णांक)dict_get_bool(args, "move", FALSE);
    जर (!move && (!dict_has_किल्ली(args, "button")
	    || !dict_has_किल्ली(args, "multiclick")
	    || !dict_has_किल्ली(args, "modifiers")))
	परत FALSE;

    row = (पूर्णांक)dict_get_संख्या(args, "row") - 1;
    col = (पूर्णांक)dict_get_संख्या(args, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_RECORD mer;
    ZeroMemory(&mer, आकार(mer));
    mer.dwMousePosition.X  = col;
    mer.dwMousePosition.Y  = row;

    जर (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    अन्यथा
    {
	button = (पूर्णांक)dict_get_संख्या(args, "button");
	repeated_click = (पूर्णांक)dict_get_संख्या(args, "multiclick");
	mods = (पूर्णांक)dict_get_संख्या(args, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	निवड (button)
	{
	    स्थिती MOUSE_डावा:
		mer.dwButtonState = FROM_डावा_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		थांबा;
	    स्थिती MOUSE_मधला:
		mer.dwButtonState = FROM_डावा_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		थांबा;
	    स्थिती MOUSE_उजवा:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		थांबा;
	    स्थिती MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    स्थिती MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		थांबा;
	    स्थिती MOUSE_X1:
		mer.dwButtonState = FROM_डावा_3_RD_BUTTON_PRESSED;
		थांबा;
	    स्थिती MOUSE_X2:
		mer.dwButtonState = FROM_डावा_4_TH_BUTTON_PRESSED;
		थांबा;
	    स्थिती MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		थांबा;
	    स्थिती MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		थांबा;
	    स्थिती MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		थांबा;
	    स्थिती MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		थांबा;
	    पूर्वनिर्धारित:
		semsg(_(e_invalid_argument_str), "button");
		परत FALSE;
	}
    }

    mer.dw_Control_किल्ली_State = 0;
    जर (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	जर (mods & MOUSE_SHIFT)
	    mer.dw_Control_किल्ली_State |= SHIFT_PRESSED;
	जर (mods & MOUSE_CTRL)
	    mer.dw_Control_किल्ली_State |= डावा_CTRL_PRESSED;
	जर (mods & MOUSE_ALT)
	    mer.dw_Control_किल्ली_State |= डावा_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    परत TRUE;
}
# endif  // FEAT_EVAL

    स्थिर पूर्णांक
write_आदान_record_buffer(आदान_RECORD* irEvents, पूर्णांक nLength)
{
    पूर्णांक				n_मोजणी = 0;
    आदान_record_buffer_गाठ_T	*event_गाठ;

    पर्यंत (n_मोजणी < nLength)
    {
	event_गाठ = alloc(आकार(आदान_record_buffer_गाठ_T));
	जर (event_गाठ == रिक्त)
	    परत -1;

	event_गाठ->ir = irEvents[n_मोजणी++];
	event_गाठ->next = रिक्त;

	आदान_record_buffer.length++;
	जर (आदान_record_buffer.tail == रिक्त)
	{
	    आदान_record_buffer.head = event_गाठ;
	    आदान_record_buffer.tail = event_गाठ;
	}
	अन्यथा
	{
	    आदान_record_buffer.tail->next = event_गाठ;
	    आदान_record_buffer.tail = आदान_record_buffer.tail->next;
	}
    }
    परत n_मोजणी;
}

    स्थिर पूर्णांक
read_आदान_record_buffer(आदान_RECORD* irEvents, पूर्णांक nMaxLength)
{
    पूर्णांक n_मोजणी = 0;
    पर्यंत (n_मोजणी < nMaxLength && आदान_record_buffer.head != रिक्त)
    {
	आदान_record_buffer.length--;
	आदान_record_buffer_गाठ_T *काढा_head = आदान_record_buffer.head;
	irEvents[n_मोजणी++] = काढा_head->ir;
	आदान_record_buffer.head = काढा_head->next;
	vim_free(काढा_head);
	जर (आदान_record_buffer.length == 0)
	    आदान_record_buffer.tail = रिक्त;
    }
    परत n_मोजणी;
}
#अन्यथा  // FEAT_GUI_MSWIN && !VIMDLL
    शून्य
mch_setmouse(पूर्णांक on UNUSED)
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
    पूर्णांक
test_mswin_event(char_u *event, dict_T *args)
{
    पूर्णांक lpEventsWritten = 0;

# जर defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    जर (gui.in_use)
	परत test_gui_w32_sendevent(event, args);
# endif

# जर defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    आदान_RECORD ir;
    BOOL आदान_encoded = FALSE;
    BOOL execute = FALSE;
    जर (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(args, "execute", FALSE);
	जर (dict_has_किल्ली(args, "event"))
	    आदान_encoded = encode_किल्ली_event(args, &ir);
	अन्यथा जर (!execute)
	{
	    semsg(_(e_missing_argument_str), "event");
	    परत FALSE;
	}
    }
    अन्यथा जर (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	आदान_encoded = encode_mouse_event(args, &ir);
    }
    अन्यथा
    {
	semsg(_(e_invalid_मूल्य_for_argument_str_str), "event", event);
	परत FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    जर (आदान_encoded)
    {
	जर ((lpEventsWritten = write_आदान_record_buffer(&ir, 1)) < 0)
	{
	    semsg(_(e_out_of_memory), "event");
	    परत FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    जर (execute)
    {
	पूर्णांक save_संदेश_scroll = संदेश_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	संदेश_scroll = FALSE;
	ch_log(रिक्त, "test_mswin_event() executing");
	exec_normal(TRUE, TRUE, TRUE);
	संदेश_scroll |= save_संदेश_scroll;
    }

# endif
    परत lpEventsWritten;
}
#endif // FEAT_EVAL

#जर defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    स्थिर शून्य
mch_set_cursor_shape(पूर्णांक thickness)
{
    जर (vtp_working)
    {
	जर (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    जर (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    अन्यथा
		vtp_printf("\033[0 q");	// default
	}
    }
    अन्यथा
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dwSize = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	SetConsoleCursorInfo(g_hConOut, &ConsoleCursorInfo);
	जर (s_cursor_visible)
	    SetConsoleCursorPosition(g_hConOut, g_coord);
    }
}

    शून्य
mch_update_cursor(शून्य)
{
    पूर्णांक		idx;
    पूर्णांक		thickness;

# ifdef VIMDLL
    जर (gui.in_use)
	परत;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    जर (shape_table[idx].shape == SHAPE_BLOCK)
	thickness = 99;	// 100 doesn't work on W95
    अन्यथा
	thickness = shape_table[idx].percentage;
    mch_set_cursor_shape(thickness);
}
#endif

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    स्थिर शून्य
handle_focus_event(आदान_RECORD ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
    ui_focus_change((पूर्णांक)g_fJustGotFocus);
}

स्थिर शून्य ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    स्थिर पूर्णांक
WaitForChar(दीर्घ msec, पूर्णांक ignore_आदान)
{
    DWORD	    dwNow = 0, dwEndTime = 0;
    आदान_RECORD    ir;
    DWORD	    cRecords;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    पूर्णांक		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    जर (msec > 0)
	// Wait until the specified time has elapsed.
	dwEndTime = Get_Tick_मोजणी() + msec;
    अन्यथा जर (msec < 0)
	// Wait forever.
	dwEndTime = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    साठी (;;)
    {
	// Only process messages when waiting.
	जर (msec != 0)
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

	जर (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_आदान && आदान_available())
# endif
	   )
	    परत TRUE;

	जर (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_मोजणी();
	    जर ((पूर्णांक)(dwNow - dwEndTime) >= 0)
		थांबा;
	}
	जर (msec != 0)
	{
	    DWORD dwWaitTime = dwEndTime - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    जर (dwWaitTime > 11)
		dwWaitTime = 11;

# ifdef FEAT_MZSCHEME
	    जर (mzthreads_allowed() && p_mzq > 0 && (दीर्घ)dwWaitTime > p_mzq)
		dwWaitTime = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    जर (dwWaitTime > 10)
	    {
		दीर्घ	due_time;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_time = check_due_timer();
		जर (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    परत FALSE;
		}
		जर (due_time > 0 && dwWaitTime > (DWORD)due_time)
		    dwWaitTime = due_time;
	    }
# endif
	    जर (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    संदेश_wait_for_multiple_objects(1, &g_hConIn, FALSE,
				  dwWaitTime, QS_SENDMESSAGE) != WAIT_OBJECT_0
# अन्यथा
		    wait_for_single_object(g_hConIn, dwWaitTime)
							      != WAIT_OBJECT_0
# endif
		    )
		सुरूठेवा;
	}

	cRecords = 0;
	peek_console_आदान(g_hConIn, &ir, 1, &cRecords);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	जर (State == MODE_CMDLINE && संदेश_row == Rows - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_BUFFER_INFO csbi;

	    जर (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	    {
		जर (csbi.dwCursorPosition.Y != संदेश_row)
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

	जर (cRecords > 0)
	{
	    जर (ir.EventType == किल्ली_EVENT && ir.Event.किल्ली_Event_2.b_किल्ली_Down)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		जर (ir.Event.किल्ली_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.किल्ली_Event_2.w_Virtual_किल्ली_Code == 13)
		{
		    read_console_आदान(g_hConIn, &ir, 1, &cRecords);
		    सुरूठेवा;
		}
# endif
		जर (decode_किल्ली_event(&ir.Event.किल्ली_Event_2, &ch, &ch2,
								 रिक्त, FALSE))
		    परत TRUE;
	    }

	    read_console_आदान(g_hConIn, &ir, 1, &cRecords);

	    जर (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    अन्यथा जर (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    {
		COORD dwSize = ir.Event.WindowBufferSizeEvent.dwSize;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		जर (dwSize.X != Columns || dwSize.Y != Rows)
		{
		    CONSOLE_SCREEN_BUFFER_INFO csbi;
		    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
		    dwSize.X = csbi.srWindow.उजवा_3 - csbi.srWindow.डावा_3 + 1;
		    dwSize.Y = csbi.srWindow.Bottom - csbi.srWindow.शीर्ष_2 + 1;
		    जर (dwSize.X != Columns || dwSize.Y != Rows)
		    {
			ResizeConBuf(g_hConOut, dwSize);
			shell_resized();
		    }
		}
	    }
	    अन्यथा जर (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		परत TRUE;
	}
	अन्यथा जर (msec == 0)
	    थांबा;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    जर (आदान_available())
	परत TRUE;
# endif

    परत FALSE;
}

/*
 * return non-zero if a character is available
 */
    पूर्णांक
mch_char_avail(शून्य)
{
# ifdef VIMDLL
    जर (gui.in_use)
	परत TRUE;
# endif
    परत WaitForChar(0L, FALSE);
}

# जर defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    पूर्णांक
mch_check_messages(शून्य)
{
#  ifdef VIMDLL
    जर (gui.in_use)
	परत TRUE;
#  endif
    परत WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    स्थिर शून्य
create_conin(शून्य)
{
    g_hConIn =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES) रिक्त,
			OPEN_EXISTING, 0, (HANDLE)रिक्त);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    स्थिर WCHAR
tgetch(पूर्णांक *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    साठी (;;)
    {
	आदान_RECORD ir;
	DWORD cRecords = 0;

# ifdef FEAT_CLIENTSERVER
	(शून्य)WaitForChar(-1L, FALSE);
	जर (आदान_available())
	    परत 0;
	जर (g_nMouseClick != -1)
	    परत 0;
# endif
	जर (read_console_आदान(g_hConIn, &ir, 1, &cRecords) == 0)
	{
	    जर (did_create_conin)
		read_error_exit();
	    create_conin();
	    सुरूठेवा;
	}

	जर (ir.EventType == किल्ली_EVENT)
	{
	    जर (decode_किल्ली_event(&ir.Event.किल्ली_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		परत ch;
	}
	अन्यथा जर (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	अन्यथा जर (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    shell_resized();
	अन्यथा जर (ir.EventType == MOUSE_EVENT)
	{
	    जर (decode_mouse_event(&ir.Event.MouseEvent))
		परत 0;
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
    पूर्णांक
mch_inchar(
    char_u	*buf UNUSED,
    पूर्णांक		maxlen UNUSED,
    दीर्घ	समय UNUSED,
    पूर्णांक		tb_change_cnt UNUSED)
{
#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    पूर्णांक		len;
    पूर्णांक		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# अन्यथा
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    स्थिर char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    स्थिर पूर्णांक	    typeaheadlen = 0;

# ifdef VIMDLL
    जर (gui.in_use)
	परत 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    जर (typeaheadlen > 0)
	जा theend;

    जर (समय >= 0)
    {
	जर (!WaitForChar(समय, FALSE))     // no character available
	    परत 0;
    }
    अन्यथा    // time == -1, wait forever
    {
	mch_set_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	जर (!WaitForChar(p_ut, FALSE))
	{
	    जर (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (पूर्णांक)KE_CURSORHOLD;
		परत 3;
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
    जर (fdDump)
	ख_अक्षर_दो('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    पर्यंत ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	जर (typebuf_changed(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    थांबा;
	}
	जर (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    जर (fdDump)
		ख_लिखो(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    जर (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		जर (modifiers > 0)
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
	    अन्यथा
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
	अन्यथा
	{
	    WCHAR	ch2 = NUL;
	    पूर्णांक		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_किल्ली(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_किल्ल्या;
	    c = may_adjust_किल्ली_for_ctrl(modifiers, c);
	    --no_reduce_किल्ल्या;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_shift_modifier(modifiers, c);

	    जर (typebuf_changed(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		थांबा;
	    }

	    जर (c == Ctrl_C && ctrl_c_interrupts)
	    {
# जर defined(FEAT_CLIENTSERVER)
		trash_आदान_buf();
# endif
		got_int = TRUE;
	    }

	    जर (g_nMouseClick == -1)
	    {
		पूर्णांक	n = 1;

		जर (ch2 == NUL)
		{
		    पूर्णांक	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    जर (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_to_enc(ch, &n);
		    जर (p != रिक्त)
		    {
			साठी (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    जर (p[i] == CSI)
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
		अन्यथा
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    जर (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		जर (ch2 != NUL)
		{
		    जर (c == K_NUL)
		    {
			निवड (ch2)
			{
			स्थिती (WCHAR)'\324': // SHIFT+Insert
			स्थिती (WCHAR)'\325': // CTRL+Insert
			स्थिती (WCHAR)'\327': // SHIFT+Delete
			स्थिती (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    थांबा;

			पूर्वनिर्धारित:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    थांबा;
			}
		    }
		    अन्यथा
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
		जर ((modifiers & MOD_MASK_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_MASK_ALT;
		}

		जर (modifiers != 0)
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
		जर (fdDump)
		    ख_अक्षर_दो(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    जर (fdDump)
    {
	ख_माला_दो("]\n", fdDump);
	ख_साफ(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    पर्यंत (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    जर (len > 0)
    {
	buf[len] = NUL;
	ch_log(रिक्त, "raw key input: \"%s\"", buf);
    }
# endif
    परत len;

#अन्यथा // FEAT_GUI_MSWIN && !VIMDLL
    परत 0;
#endif // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    स्थिर पूर्णांक
executable_file(अक्षर *name, char_u **path)
{
    पूर्णांक attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    जर (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	परत FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    जर (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	जर (res == रिक्त)
	    res = resolve_reparse_point((char_u *)name);
	जर (res == रिक्त)
	    परत FALSE;
	// The path is already absolute.
	जर (path != रिक्त)
	    *path = res;
	अन्यथा
	    vim_free(res);
    }
    अन्यथा जर (path != रिक्त)
	*path = FullName_save((char_u *)name, FALSE);
    परत TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    स्थिर bool
executable_prefix_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_FIND_DATAW	data;
    HANDLE		handle;
    DWORD		error;

    wn = enc_to_utf16(name, रिक्त);
    जर (wn == रिक्त)
	परत true;
    handle = FindFirstFileExW(wn, FindExInfoBasic, &data,
					    Find_Ex_शोध_Name_Match, रिक्त, 0);
    error = GetLastError();
    vim_free(wn);
    जर (handle != INVALID_HANDLE_मूल्य)
    {
	FindClose(handle);
	परत true;
    }
    परत error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    स्थिर पूर्णांक
executable_exists(
    अक्षर	*name,
    माप_प्रकार	namelen,
    char_u	**path,
    पूर्णांक		use_path,
    पूर्णांक		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3];
    char_u	*p;
    string_T	pathbuf = {रिक्त, 0};
    पूर्णांक		pathbuf_allocated = FALSE;
    string_T	pathext = {रिक्त, 0};
    पूर्णांक		pathext_allocated = FALSE;
    char_u	*shname = रिक्त;
    पूर्णांक		noext = FALSE;
    bool	simple_pathext = true;
    पूर्णांक		ext_मोजणी = 0;
    पूर्णांक		retval = FALSE;

    जर (namelen >= आकार(buf))	// safety check
	परत FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    जर (म_माला((अक्षर *)shname, "sh") != रिक्त &&
	!(म_माला((अक्षर *)shname, "powershell") != रिक्त
				    || म_माला((अक्षर *)shname, "pwsh") != रिक्त))
	noext = TRUE;

    जर (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	जर (pathext.string == रिक्त)
	    STR_LITERAL_SET(pathext, ".com;.exe;.bat;.cmd");
	अन्यथा
	    pathext.length = STRLEN(pathext.string);

	जर (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    माप_प्रकार  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    पर्यंत (*p)
	    {
		जर (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    जर (*p == '.')
			simple_pathext = false;
		    ++p;
		    सुरूठेवा;
		}
		e = vim_strchr(p, ';');
		जर (e == रिक्त)
		    e = pathext.string + pathext.length;
		plen = (माप_प्रकार)(e - p);
		++ext_मोजणी;
		जर (*p != '.')
		    simple_pathext = false;
		साठी (q = p + 1; q < e; ++q)
		    जर (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		जर (_strnicoll(name + namelen - plen, (अक्षर *)p, plen) == 0)
		{
		    noext = TRUE;
		    थांबा;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    जर (pathext.string == रिक्त)
	STR_LITERAL_SET(pathext, ".");
    अन्यथा जर (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.length + 3);
	जर (tmp == रिक्त)
	{
	    retval = FALSE;
	    जा theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.length += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    जर (use_path && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	जर (p != रिक्त)
	{
	    माप_प्रकार  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    जर (pathbuf.string == रिक्त)
	    {
		retval = FALSE;
		जा theend;
	    }

	    जर (mch_getenv("NoDefaultCurrentDirectoryInExePath") == रिक्त &&
		    म_माला((अक्षर *)gettail(p_sh), "cmd.exe") != रिक्त)
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
    जर (pathbuf.string == रिक्त)
	STR_LITERAL_SET(pathbuf, ".");
    p = pathbuf.string;
    पर्यंत (*p)
    {
	char_u	*e;
	माप_प्रकार	buflen;

	जर (*p == ';') // Skip empty entry
	{
	    ++p;
	    सुरूठेवा;
	}
	e = vim_strchr(p, ';');
	जर (e == रिक्त)
	    e = pathbuf.string + pathbuf.length;

	जर (e - p + namelen + 2 > आकार(buf))
	{
	    retval = FALSE;
	    जा theend;
	}
	// A single "." that means current dir.
	जर (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	अन्यथा
	{
	    buflen = vim_snprintf_safelen(
		(अक्षर *)buf,
		आकार(buf),
		"%.*s%s%s", (पूर्णांक)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	जर (!noext && simple_pathext && ext_मोजणी >= 3 && buflen > 0
		&& buflen + 2 <= आकार(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == रिक्त
		&& vim_strchr(gettail(buf), ':') == रिक्त)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_prefix_exists(buf);
	    buf[buflen] = NUL;
	    जर (!exists)
	    {
		p = e;
		सुरूठेवा;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	पर्यंत (*p)
	{
	    char_u  *e2;

	    जर (*p == ';')
	    {
		// Skip empty entry
		++p;
		सुरूठेवा;
	    }
	    e2 = vim_strchr(p, ';');
	    जर (e2 == रिक्त)
		e2 = pathext.string + pathext.length;

	    जर (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		जर (e2 - p + buflen + 1 > आकार(buf))
		{
		    retval = FALSE;
		    जा theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    जर (executable_file((अक्षर *)buf, path))
	    {
		retval = TRUE;
		जा theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    जर (pathbuf_allocated)
	मुक्त(pathbuf.string);
    जर (pathext_allocated)
	मुक्त(pathext.string);
    परत retval;
}

#जर (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    स्थिर शून्य
bad_param_handler(स्थिरांक ब_अक्षर_प्रकार *expression UNUSED,
    स्थिरांक ब_अक्षर_प्रकार *function UNUSED,
    स्थिरांक ब_अक्षर_प्रकार *file UNUSED,
    अचिन्हित पूर्णांक ओळ UNUSED,
    uintptr_t pReserved UNUSED)
{
}

# define SET_INVALID_PARAM_HANDLER \
	((शून्य)_set_invalid_parameter_handler(bad_param_handler))
#अन्यथा
# define SET_INVALID_PARAM_HANDLER
#endif

#ifdef FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    स्थिर शून्य
mch_init_g(शून्य)
{
# ifndef __MINGW32__
    बाह्य पूर्णांक _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_द्विमान;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    Rows = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[_MAX_PATH + 4];
	माप_प्रकार	exe_pathlen = (माप_प्रकार)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	जर (exe_pathlen + 10 >= आकार(vimrun_location))
	{
	    जर (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    रिक्त, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	अन्यथा
	{
	    // First try in same directory as gvim.exe.
	    जर (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    जर (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		माप_प्रकार  plen;

		जर (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(अक्षर *)vimrun_location,
			आकार(vimrun_location),
			"\"%.*svimrun\" ",
			(पूर्णांक)exe_pathlen, exe_name);
		}
		अन्यथा
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		जर (p != रिक्त)
		{
		    vimrun_path.string = p;
		    vimrun_path.length = plen;
		    vimrun_path_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    अन्यथा जर (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    रिक्त, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	जर (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    जर (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    रिक्त, TRUE, FALSE))
	set_option_मूल्य_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#endif // FEAT_GUI_MSWIN

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).उजवा_3 - (sr).डावा_3 + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).शीर्ष_2 + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    स्थिर BOOL
ClearConsoleBuffer(WORD wAttribute)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    DWORD अंक_Cells, dummy;

    जर (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	परत FALSE;

    अंक_Cells = csbi.dwSize.X * csbi.dwSize.Y;
    coord.X = 0;
    coord.Y = 0;
    जर (!FillConsoleOutputCharacter(g_hConOut, ' ', अंक_Cells,
	    coord, &dummy))
	परत FALSE;
    जर (!FillConsoleOutputAttribute(g_hConOut, wAttribute, अंक_Cells,
	    coord, &dummy))
	परत FALSE;

    परत TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    स्थिर BOOL
FitConsoleWindow(
    COORD dwBufferSize,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD dwWindowSize;
    BOOL NeedAdjust = FALSE;

    जर (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	परत FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    जर (csbi.srWindow.उजवा_3 >= dwBufferSize.X)
    {
	dwWindowSize.X = SRWIDTH(csbi.srWindow);
	जर (dwWindowSize.X > dwBufferSize.X)
	    dwWindowSize.X = dwBufferSize.X;
	csbi.srWindow.उजवा_3 = dwBufferSize.X - 1;
	csbi.srWindow.डावा_3 = dwBufferSize.X - dwWindowSize.X;
	NeedAdjust = TRUE;
    }
    जर (csbi.srWindow.Bottom >= dwBufferSize.Y)
    {
	dwWindowSize.Y = SRHEIGHT(csbi.srWindow);
	जर (dwWindowSize.Y > dwBufferSize.Y)
	    dwWindowSize.Y = dwBufferSize.Y;
	csbi.srWindow.Bottom = dwBufferSize.Y - 1;
	csbi.srWindow.शीर्ष_2 = dwBufferSize.Y - dwWindowSize.Y;
	NeedAdjust = TRUE;
    }
    जर (NeedAdjust && WantAdjust)
    {
	जर (!SetConsoleWindowInfo(g_hConOut, TRUE, &csbi.srWindow))
	    परत FALSE;
    }
    परत TRUE;
}

प्रकारव्याख्या संरचना ConsoleBufferStruct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_BUFFER_INFO	Info;
    PCHAR_INFO			Buffer;
    COORD			BufferSize;
    PSMALL_RECT			Regions;
    पूर्णांक				अंक_Regions;
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
    स्थिर BOOL
SaveConsoleBuffer(
    ConsoleBuffer *cb)
{
    DWORD अंक_Cells;
    COORD BufferCoord;
    SMALL_RECT ReadRegion;
    WORD Y, Y_incr;
    पूर्णांक i, numregions;

    जर (cb == रिक्त)
	परत FALSE;

    जर (!GetConsoleScreenBufferInfo(g_hConOut, &cb->Info))
    {
	cb->IsValid = FALSE;
	परत FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    जर (use_alternate_screen_buffer)
	परत TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    जर (!cb->IsValid || cb->Buffer == रिक्त ||
	    cb->BufferSize.X != cb->Info.dwSize.X ||
	    cb->BufferSize.Y != cb->Info.dwSize.Y)
    {
	cb->BufferSize.X = cb->Info.dwSize.X;
	cb->BufferSize.Y = cb->Info.dwSize.Y;
	अंक_Cells = cb->BufferSize.X * cb->BufferSize.Y;
	vim_free(cb->Buffer);
	cb->Buffer = ALLOC_MULT(CHAR_INFO, अंक_Cells);
	जर (cb->Buffer == रिक्त)
	    परत FALSE;
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
    ReadRegion.डावा_3 = 0;
    ReadRegion.उजवा_3 = cb->Info.dwSize.X - 1;
    Y_incr = 12000 / cb->Info.dwSize.X;

    numregions = (cb->Info.dwSize.Y + Y_incr - 1) / Y_incr;
    जर (cb->Regions == रिक्त || numregions != cb->अंक_Regions)
    {
	cb->अंक_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->अंक_Regions);
	जर (cb->Regions == रिक्त)
	{
	    VIM_CLEAR(cb->Buffer);
	    परत FALSE;
	}
    }

    साठी (i = 0, Y = 0; i < cb->अंक_Regions; i++, Y += Y_incr)
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
	ReadRegion.शीर्ष_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	जर (!ReadConsoleOutputW(g_hConOut,	// output handle
		cb->Buffer,			// our buffer
		cb->BufferSize,			// dimensions of our buffer
		BufferCoord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->Buffer);
	    VIM_CLEAR(cb->Regions);
	    परत FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    परत TRUE;
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
    स्थिर BOOL
RestoreConsoleBuffer(
    ConsoleBuffer   *cb,
    BOOL	    RestoreScreen)
{
    COORD BufferCoord;
    SMALL_RECT WriteRegion;
    पूर्णांक i;

    जर (cb == रिक्त || !cb->IsValid)
	परत FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    जर (RestoreScreen)
	ClearConsoleBuffer(cb->Info.wAttributes);

    FitConsoleWindow(cb->Info.dwSize, TRUE);
    जर (!SetConsoleScreenBufferSize(g_hConOut, cb->Info.dwSize))
	परत FALSE;
    जर (!SetConsoleTextAttribute(g_hConOut, cb->Info.wAttributes))
	परत FALSE;

    जर (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	परत TRUE;
    }

    जर (!SetConsoleCursorPosition(g_hConOut, cb->Info.dwCursorPosition))
	परत FALSE;
    जर (!SetConsoleWindowInfo(g_hConOut, TRUE, &cb->Info.srWindow))
	परत FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    जर (use_alternate_screen_buffer)
	परत TRUE;

    /*
     * Restore the screen buffer contents.
     */
    जर (cb->Buffer != रिक्त)
    {
	साठी (i = 0; i < cb->अंक_Regions; i++)
	{
	    BufferCoord.X = cb->Regions[i].डावा_3;
	    BufferCoord.Y = cb->Regions[i].शीर्ष_2;
	    WriteRegion = cb->Regions[i];
	    जर (!WriteConsoleOutputW(g_hConOut,	// output handle
			cb->Buffer,		// our buffer
			cb->BufferSize,		// dimensions of our buffer
			BufferCoord,		// offset in our buffer
			&WriteRegion))		// region to restore
		परत FALSE;
	}
    }

    परत TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
स्थिर ConsoleBuffer g_cbOrig = { 0 };
# endif
स्थिर ConsoleBuffer g_cbNonTermcap = { 0 };
स्थिर ConsoleBuffer g_cbTermcap = { 0 };

अक्षर g_szOrigTitle[256] = { 0 };
HWND g_hWnd = रिक्त;	// also used in os_mswin.c
स्थिर HICON g_hOrigIconSmall = रिक्त;
स्थिर HICON g_hOrigIcon = रिक्त;
स्थिर HICON g_hVimIcon = रिक्त;
स्थिर BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    स्थिर BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    जर (hWnd == रिक्त)
	परत FALSE;

    जर (phIconSmall != रिक्त)
	*phIconSmall = (HICON)SendMessage(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    जर (phIcon != रिक्त)
	*phIcon = (HICON)SendMessage(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    परत TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    स्थिर BOOL
SetConsoleIcon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    जर (hWnd == रिक्त)
	परत FALSE;

    जर (hIconSmall != रिक्त)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    जर (hIcon != रिक्त)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    परत TRUE;
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
    स्थिर शून्य
SaveConsoleTitleAndIcon(शून्य)
{
    // Save the original title.
    जर (!GetConsoleTitle(g_szOrigTitle, आकार(g_szOrigTitle)))
	परत;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = GetConsoleWindow();
    जर (g_hWnd == रिक्त)
	परत;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    जर (g_hOrigIconSmall == रिक्त || g_hOrigIcon == रिक्त)
	परत;

    // Extract the first icon contained in the Vim executable.
    जर (
# ifdef FEAT_LIBCALL
	    mch_icon_load((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == रिक्त)
	g_hVimIcon = ExtractIcon(रिक्त, (LPCSTR)exe_name, 0);
    जर (g_hVimIcon != रिक्त)
	g_fCanChangeIcon = TRUE;
}

स्थिर पूर्णांक g_fWindInitCalled = FALSE;
स्थिर पूर्णांक g_fTermcapMode = FALSE;
स्थिर CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    स्थिर शून्य
mch_init_c(शून्य)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_BUFFER_INFO csbi;
# endif
# ifndef __MINGW32__
    बाह्य पूर्णांक _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_द्विमान;		// we do our own CR-LF translation
    out_flush();

    // Obtain handles for the standard Console I/O devices
    जर (read_cmd_fd == 0)
	g_hConIn =  GetStdHandle(STD_आदान_HANDLE);
    अन्यथा
	create_conin();
    g_hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    SaveConsoleBuffer(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.wAttributes;
# अन्यथा
    // Get current text attributes
    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
# endif
    जर (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (g_attrCurrent & 0xf) + 1;
    जर (cterm_normal_bg_color == 0)
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
    जर (g_fCanChangeIcon)
	SetConsoleIcon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    जर (use_alternate_screen_buffer)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = ख_खोलो("dump", "wt");

    जर (fdDump)
    {
	समय_प्रकार t;

	समय(&t);
	ख_माला_दो(स_समय(&t), fdDump);
	ख_साफ(fdDump);
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
    स्थिर शून्य
mch_exit_c(पूर्णांक r)
{
    // Copy flag since stoptermcap() will clear the flag.
    पूर्णांक fTermcapMode = g_fTermcapMode;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    जर (!fTermcapMode &&  use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    जर (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    जर (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	जर (g_fCanChangeIcon && !g_fForceExit)
	    SetConsoleIcon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	जर (fdDump)
	{
	    समय_प्रकार t;

	    समय(&t);
	    ख_माला_दो(स_समय(&t), fdDump);
	    ख_बंद(fdDump);
	}
	fdDump = रिक्त;
# endif
    }

    SetConsoleCursorInfo(g_hConOut, &g_cci);
    SetConsoleMode(g_hConIn,  g_cmodein | ENABLE_EXTENDED_FLAGS);
    SetConsoleMode(g_hConOut, g_cmodeout);

# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    निकास(r);
}
#endif // !FEAT_GUI_MSWIN || VIMDLL

    शून्य
mch_init(शून्य)
{
#ifdef VIMDLL
    जर (gui.starting)
	mch_init_g();
    अन्यथा
	mch_init_c();
#किंवाजर defined(FEAT_GUI_MSWIN)
    mch_init_g();
#अन्यथा
    mch_init_c();
#endif
}

    शून्य
mch_exit(पूर्णांक r)
{
#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef VIMDLL
    जर (vimrun_path_allocated)
	vim_free(vimrun_path.string);

    जर (gui.in_use || gui.starting)
	mch_exit_g(r);
    अन्यथा
	mch_exit_c(r);
#किंवाजर defined(FEAT_GUI_MSWIN)
    जर (vimrun_path_allocated)
	vim_free(vimrun_path.string);
    mch_exit_g(r);
#अन्यथा
    mch_exit_c(r);
#endif
}

/*
 * Do we have an interactive window?
 */
    पूर्णांक
mch_check_win(
    पूर्णांक argc UNUSED,
    अक्षर **argv UNUSED)
{
    mch_get_exe_name();

#जर defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    परत OK;	    // GUI always has a tty
#अन्यथा
# ifdef VIMDLL
    जर (gui.in_use)
	परत OK;
# endif
    जर (isatty(1))
	परत OK;
    परत FAIL;
#endif
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    शून्य
fname_case(
    char_u	*name,
    पूर्णांक		len)
{
    WCHAR   *p;
    WCHAR   buf[_MAX_PATH + 1];

    जर (*name == NUL)
	परत;

    slash_adjust(name);

    p = enc_to_utf16(name, रिक्त);
    जर (p == रिक्त)
	परत;

    जर (GetLongPathNameW(p, buf, _MAX_PATH))
    {
	char_u	*q = utf16_to_enc(buf, रिक्त);

	जर (q != रिक्त)
	{
	    जर (len > 0)
		vim_strncpy(name, q, len - 1);
	    अन्यथा
	    {
		माप_प्रकार  namelen = STRLEN(name);

		जर (namelen >= STRLEN(q))
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
    पूर्णांक
mch_get_user_name(
    char_u  *s,
    पूर्णांक	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_LENGTH(wszUserName);

    जर (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_to_enc(wszUserName, रिक्त);

	जर (p != रिक्त)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    परत OK;
	}
    }
    s[0] = NUL;
    परत FAIL;
}


/*
 * Insert host name in s[len].
 */
    शून्य
mch_get_host_name(
    char_u	*s,
    पूर्णांक		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_LENGTH(wszHostName);

    जर (!GetComputerNameW(wszHostName, &wcch))
	परत;

    char_u  *p = utf16_to_enc(wszHostName, रिक्त);
    जर (p == रिक्त)
	परत;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    दीर्घ
mch_get_pid(शून्य)
{
    परत (दीर्घ)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    पूर्णांक
mch_process_running(दीर्घ pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   status = 0;
    पूर्णांक	    ret = FALSE;

    जर (hProcess == रिक्त)
	परत FALSE;  // might not have access
    जर (GetExitCodeProcess(hProcess, &status))
	ret = status == STILL_ACTIVE;
    CloseHandle(hProcess);
    परत ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    पूर्णांक
mch_dirname(
    char_u	*buf,
    पूर्णांक		len)
{
    WCHAR   wbuf[_MAX_PATH + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    जर (GetCurrentDirectoryW(_MAX_PATH, wbuf) == 0)
	परत FAIL;

    WCHAR   wcbuf[_MAX_PATH + 1];
    char_u  *p = रिक्त;

    जर (GetLongPathNameW(wbuf, wcbuf, _MAX_PATH) != 0)
    {
	p = utf16_to_enc(wcbuf, रिक्त);
	जर (p != रिक्त)
	{
	    जर (STRLEN(p) >= (माप_प्रकार)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    जर (p == रिक्त)
	p = utf16_to_enc(wbuf, रिक्त);

    जर (p == रिक्त)
	परत FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    परत OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    दीर्घ
mch_getperm(char_u *name)
{
    stat_T	st;

    परत (mch_stat((अक्षर *)name, &st) == 0)
	? (दीर्घ)(अचिन्हित लघु)st.st_mode
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णांक
mch_setperm(char_u *name, दीर्घ perm)
{
    दीर्घ	n;
    WCHAR	*p;

    p = enc_to_utf16(name, रिक्त);
    जर (p == रिक्त)
	परत FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    जर (n == -1)
	परत FAIL;

    win32_set_archive(name);

    परत OK;
}

/*
 * Set hidden flag for "name".
 */
    शून्य
mch_hide(char_u *name)
{
    पूर्णांक attrs = win32_getattrs(name);
    जर (attrs == -1)
	परत;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    पूर्णांक
mch_ishidden(char_u *name)
{
    पूर्णांक f = win32_getattrs(name);

    जर (f == -1)
	परत FALSE;		    // file does not exist at all

    परत (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    पूर्णांक
mch_isdir(char_u *name)
{
    पूर्णांक f = win32_getattrs(name);

    जर (f == -1)
	परत FALSE;		    // file does not exist at all

    परत (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    पूर्णांक
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_to_utf16(name, रिक्त);
    जर (p == रिक्त)
	परत FALSE;
    attr = GetFileAttributesW(p);
    जर (attr == INVALID_FILE_ATTRIBUTES)
	error = GetLastError();
    vim_free(p);

    जर (attr == INVALID_FILE_ATTRIBUTES)
	परत error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    परत (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    पूर्णांक
mch_isrealdir(char_u *name)
{
    परत mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    पूर्णांक
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    पूर्णांक	    retval;

    p = enc_to_utf16(name, रिक्त);
    जर (p == रिक्त)
	परत -1;
    retval = _wmkdir(p);
    vim_free(p);
    परत retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    पूर्णांक
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    पूर्णांक	    retval;

    p = enc_to_utf16(name, रिक्त);
    जर (p == रिक्त)
	परत -1;
    retval = _wrmdir(p);
    vim_free(p);
    परत retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    पूर्णांक
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    परत win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_संख्या_Of_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    पूर्णांक
mch_is_symbolic_link(char_u *name)
{
    HANDLE		hFind;
    पूर्णांक			res = FALSE;
    DWORD		fileFlags = 0, reparseTag = 0;
    WCHAR		*wn;
    WIN32_FIND_DATAW	findDataW;

    wn = enc_to_utf16(name, रिक्त);
    जर (wn == रिक्त)
	परत FALSE;

    hFind = FindFirstFileW(wn, &findDataW);
    vim_free(wn);
    जर (hFind != INVALID_HANDLE_मूल्य)
    {
	fileFlags = findDataW.dwFileAttributes;
	reparseTag = findDataW.dwReserved0;
	FindClose(hFind);
    }

    जर ((fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
	    && (reparseTag == IO_REPARSE_TAG_SYMLINK
		|| reparseTag == IO_REPARSE_TAG_MOUNT_POINT))
	res = TRUE;

    परत res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    पूर्णांक
mch_is_linked(char_u *fname)
{
    जर (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	परत TRUE;
    परत FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    पूर्णांक
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    पूर्णांक		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_to_utf16(fname, रिक्त);
    जर (wn == रिक्त)
	परत FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    रिक्त,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    रिक्त);		// handle to template file
    vim_free(wn);

    जर (hFile == INVALID_HANDLE_मूल्य)
	परत FILEINFO_READ_FAIL;

    जर (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    अन्यथा
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    परत res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    स्थिर पूर्णांक
win32_getattrs(char_u *name)
{
    पूर्णांक		attr;
    WCHAR	*p;

    p = enc_to_utf16(name, रिक्त);
    जर (p == रिक्त)
	परत INVALID_FILE_ATTRIBUTES;

    attr = GetFileAttributesW(p);
    vim_free(p);

    परत attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    स्थिर पूर्णांक
win32_setattrs(char_u *name, पूर्णांक attrs)
{
    पूर्णांक	    res;
    WCHAR   *p;

    p = enc_to_utf16(name, रिक्त);
    जर (p == रिक्त)
	परत -1;

    res = SetFileAttributesW(p, attrs);
    vim_free(p);

    परत res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    स्थिर पूर्णांक
win32_set_archive(char_u *name)
{
    पूर्णांक attrs = win32_getattrs(name);
    जर (attrs == -1)
	परत -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    परत win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    पूर्णांक
mch_writable(char_u *name)
{
    पूर्णांक attrs = win32_getattrs(name);

    परत (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    पूर्णांक
mch_can_exe(char_u *name, char_u **path, पूर्णांक use_path UNUSED)
{
    परत executable_exists((अक्षर *)name, STRLEN(name), path, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    पूर्णांक
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    पूर्णांक		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    जर (STRNCMP(name, "\\\\.\\", 4) == 0)
	परत गाठ_WRITABLE;

    wn = enc_to_utf16(name, रिक्त);
    जर (wn == रिक्त)
	परत गाठ_NORMAL;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    रिक्त,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    रिक्त);		    // handle to template file
    vim_free(wn);
    जर (hFile == INVALID_HANDLE_मूल्य)
	परत गाठ_NORMAL;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    जर (type == FILE_TYPE_CHAR)
	परत गाठ_WRITABLE;
    जर (type == FILE_TYPE_DISK)
	परत गाठ_NORMAL;
    परत गाठ_OTHER;
}

#ifdef HAVE_ACL
संरचना my_acl
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
    परत (vim_acl_T)रिक्त;
#अन्यथा
    संरचना my_acl   *p = रिक्त;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(संरचना my_acl);
    जर (p != रिक्त)
    {
	WCHAR	*wn;

	wn = enc_to_utf16(fname, रिक्त);
	जर (wn == रिक्त)
	{
	    vim_free(p);
	    परत रिक्त;
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
	जर (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (शून्य)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    रिक्त,
		    रिक्त,
		    &p->pDacl,
		    रिक्त,
		    &p->pSecurityDescriptor);
	}
	जर (p->pSecurityDescriptor == रिक्त)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = रिक्त;
	}
	vim_free(wn);
    }

    परत (vim_acl_T)p;
#endif
}

#ifdef HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    स्थिर BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_SIZE_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_मोजणी = 0;
    GetAclInformation(acl, &acl_info, आकार(acl_info), AclSizeInformation);
    साठी (i = 0; i < acl_info.Ace_मोजणी; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	जर (ace->Header.AceFlags & INHERITED_ACE)
	    परत TRUE;
    }
    परत FALSE;
}
#endif

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    शून्य
mch_set_acl(char_u *fname, vim_acl_T acl)
{
#ifdef HAVE_ACL
    संरचना my_acl   *p = (संरचना my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    जर (p == रिक्त)
	परत;

    wn = enc_to_utf16(fname, रिक्त);
    जर (wn == रिक्त)
	परत;

    // Set security flags
    जर (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    जर (p->pSidGroup)
	sec_info |= GROUP_SECURITY_INFORMATION;
    जर (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	जर (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    जर (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (शून्य)SetNamedSecurityInfoW(
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

    शून्य
mch_free_acl(vim_acl_T acl)
{
#ifdef HAVE_ACL
    संरचना my_acl   *p = (संरचना my_acl *)acl;

    जर (p != रिक्त)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#endif
}

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    स्थिर BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    आदान_RECORD ir;
    DWORD out;

    निवड (dwCtrlType)
    {
    स्थिती CTRL_C_EVENT:
	जर (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	परत TRUE;

    स्थिती CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = किल्ली_EVENT;
	ir.Event.किल्ली_Event_2.b_किल्ली_Down = TRUE;
	ir.Event.किल्ली_Event_2.w_Repeat_मोजणी = 1;
	ir.Event.किल्ली_Event_2.w_Virtual_किल्ली_Code = VK_CANCEL;
	ir.Event.किल्ली_Event_2.wVirtualScanCode = 0;
	ir.Event.किल्ली_Event_2.dw_Control_किल्ली_State = 0;
	ir.Event.किल्ली_Event_2.uChar.UnicodeChar = 0;
	Write_Console_आदान(g_hConIn, &ir, 1, &out);
	परत TRUE;

    // fatal events: shut down gracefully
    स्थिती CTRL_CLOSE_EVENT:
    स्थिती CTRL_LOGOFF_EVENT:
    स्थिती CTRL_SHUTDOWN_EVENT:
	windgoto((पूर्णांक)Rows - 1, cmdline_col_off);
	g_fForceExit = TRUE;

	vim_snprintf((अक्षर *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	परत TRUE;		// not reached

    पूर्वनिर्धारित:
	परत FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    शून्य
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    जर (gui.in_use)
	परत;
# endif
    GetConsoleMode(g_hConIn, &cmodein);
    GetConsoleMode(g_hConOut, &cmodeout);
    जर (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_ओळ_आदान | ENABLE_PROCESSED_आदान |
		     ENABLE_ECHO_आदान);
	जर (g_fMouseActive)
	{
	    cmodein |= ENABLE_MOUSE_आदान;
	    cmodein &= ~ENABLE_QUICK_EDIT_MODE;
	}
	अन्यथा
	{
	    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# अन्यथा
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    अन्यथा // cooked
    {
	cmodein |= (ENABLE_ओळ_आदान | ENABLE_PROCESSED_आदान |
		    ENABLE_ECHO_आदान);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
    SetConsoleMode(g_hConOut, cmodeout);
    SetConsoleCtrlHandler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    जर (fdDump)
    {
	ख_लिखो(fdDump, "mch_settmode(%s, in = %x, out = %x)\n",
		tmode == TMODE_RAW ? "raw" :
				    tmode == TMODE_COOK ? "cooked" : "normal",
		cmodein, cmodeout);
	ख_साफ(fdDump);
    }
# endif
}


/*
 * Get the size of the current window in `Rows' and `Columns'
 * Return OK when size could be determined, FAIL otherwise.
 */
    पूर्णांक
mch_get_shellsize(शून्य)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

# ifdef VIMDLL
    जर (gui.in_use)
	परत OK;
# endif
    जर (!g_fTermcapMode && g_cbTermcap.IsValid)
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
    अन्यथा जर (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	Rows = csbi.srWindow.Bottom - csbi.srWindow.शीर्ष_2 + 1;
	Columns = csbi.srWindow.उजवा_3 - csbi.srWindow.डावा_3 + 1;
    }
    अन्यथा
    {
	Rows = 25;
	Columns = 80;
    }
    परत OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    स्थिर शून्य
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    जर (use_alternate_screen_buffer)
	परत;

    जर (!SetConsoleScreenBufferSize(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	जर (fdDump)
	{
	    ख_लिखो(fdDump, "SetConsoleScreenBufferSize failed: %lx\n",
		    GetLastError());
	    ख_साफ(fdDump);
	}
# endif
    }
}

/*
 * Resize console window size to 'srWindowRect'
 */
    स्थिर शून्य
ResizeWindow(
    HANDLE     hConsole,
    SMALL_RECT srWindowRect)
{
    जर (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
    {
# ifdef MCH_WRITE_DUMP
	जर (fdDump)
	{
	    ख_लिखो(fdDump, "SetConsoleWindowInfo failed: %lx\n",
		    GetLastError());
	    ख_साफ(fdDump);
	}
# endif
    }
}

/*
 * Set a console window to `xSize' * `ySize'
 */
    स्थिर शून्य
ResizeConBufAndWindow(
    HANDLE  hConsole,
    पूर्णांक	    xSize,
    पूर्णांक	    ySize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    srWindowRect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    स्थिर पूर्णांक	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    जर (fdDump)
    {
	ख_लिखो(fdDump, "ResizeConBufAndWindow(%d, %d)\n", xSize, ySize);
	ख_साफ(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    // define the new console window size and scroll position
    srWindowRect.डावा_3 = srWindowRect.शीर्ष_2 = (SHORT) 0;
    srWindowRect.उजवा_3 =  (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);

    जर (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	पूर्णांक sx, sy;

	sx = csbi.srWindow.उजवा_3 - csbi.srWindow.डावा_3 + 1;
	sy = csbi.srWindow.Bottom - csbi.srWindow.शीर्ष_2 + 1;
	जर (sy < ySize || sx < xSize)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    जर (sy < ySize)
		coordScreen.Y = ySize;
	    अन्यथा
		coordScreen.Y = sy;
	    जर (sx < xSize)
		coordScreen.X = xSize;
	    अन्यथा
		coordScreen.X = sx;
	    SetConsoleScreenBufferSize(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = xSize;
    coordScreen.Y = ySize;

    // In the new console call API, only the first time in reverse order
    जर (!vtp_working || resized)
    {
	ResizeWindow(hConsole, srWindowRect);
	ResizeConBuf(hConsole, coordScreen);
    }
    अन्यथा
    {
	// Workaround for a Windows 10 bug
	cursor.X = srWindowRect.डावा_3;
	cursor.Y = srWindowRect.शीर्ष_2;
	SetConsoleCursorPosition(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	ResizeWindow(hConsole, srWindowRect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    शून्य
mch_set_shellsize(शून्य)
{
    COORD coordScreen;

# ifdef VIMDLL
    जर (gui.in_use)
	परत;
# endif
    // Don't change window size while still starting up
    जर (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	परत;
    }

    जर (term_console)
    {
	coordScreen = GetLargestConsoleWindowSize(g_hConOut);

	// Clamp Rows and Columns to reasonable values
	जर (Rows > coordScreen.Y)
	    Rows = coordScreen.Y;
	जर (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	ResizeConBufAndWindow(g_hConOut, Columns, Rows);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    शून्य
mch_new_shellsize(शून्य)
{
# ifdef VIMDLL
    जर (gui.in_use)
	परत;
# endif
    set_scroll_region(0, 0, Columns - 1, Rows - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    शून्य
mch_set_winsize_now(शून्य)
{
    जर (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_set_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# जर defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    शून्य
mch_calc_cell_size(संरचना cellsize *cs_out)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_out->cs_xpixel = -1;
    cs_out->cs_ypixel = -1;

    जर (g_hConOut == INVALID_HANDLE_मूल्य)
	परत;

    cfi.cbSize = आकार(cfi);
    जर (GetCurrentConsoleFontEx(g_hConOut, FALSE, &cfi)
	    && cfi.dwFontSize.X > 0 && cfi.dwFontSize.Y > 0)
    {
	cs_out->cs_xpixel = cfi.dwFontSize.X;
	cs_out->cs_ypixel = cfi.dwFontSize.Y;
	परत;
    }

    GetConsoleMode(g_hConIn, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_ओळ_आदान | ENABLE_ECHO_आदान
	    | ENABLE_PROCESSED_आदान);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_आदान;

    जर (SetConsoleMode(g_hConIn, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	SetConsoleMode(g_hConIn, cmodeold);
    }
}
# endif

#endif

    स्थिर BOOL
vim_create_process(
    अक्षर		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    अक्षर		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = रिक्त;

    wcmd = enc_to_utf16((char_u *)cmd, रिक्त);
    जर (wcmd == रिक्त)
	परत FALSE;
    जर (cwd != रिक्त)
    {
	wcwd = enc_to_utf16((char_u *)cwd, रिक्त);
	जर (wcwd == रिक्त)
	    जा theend;
    }

    ret = CreateProcessW(
	    रिक्त,		// Executable name
	    wcmd,		// Command to execute
	    रिक्त,		// Process security attributes
	    रिक्त,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    परत ret;
}


    स्थिर HINSTANCE
vim_shell_execute(
    अक्षर *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_to_utf16((char_u *)cmd, रिक्त);
    जर (wcmd == रिक्त)
	परत (HINSTANCE) 0;

    ret = ShellExecuteW(रिक्त, रिक्त, wcmd, रिक्त, रिक्त, n_show_cmd);
    vim_free(wcmd);
    परत ret;
}


#जर defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    स्थिर पूर्णांक
mch_system_classic(अक्षर *cmd, पूर्णांक options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = आकार(si);
    si.lpReserved = रिक्त;
    si.lpDesktop = रिक्त;
    si.lpTitle = रिक्त;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    जर (options & SHELL_DOOUT)
	si.wShowWindow = SW_SHOWMINNOACTIVE;
    अन्यथा
	si.wShowWindow = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = रिक्त;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_MODE |	CREATE_NEW_CONSOLE,
	    &si, &pi, रिक्त, रिक्त);

    // Wait for the command to terminate before continuing
    {
	पूर्णांक	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	साठी (;;)
	{
	    संदेश_2	संदेश;

	    जर (PeekMessageW(&संदेश, (HWND)रिक्त, 0, 0, PM_REMOVE))
	    {
		TranslateMessage(&संदेश);
		DispatchMessageW(&संदेश);
		delay = 1;
		सुरूठेवा;
	    }
	    जर (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		थांबा;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    जर (delay < 50)
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

    परत ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    स्थिर अचिन्हित पूर्णांक __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_hChildStd_IN_Wr = param;
    linenr_T	    lnum = curbuf->b_op_start.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    पूर्णांक		    written = 0;

    साठी (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	जर (l == 0)
	    len = 0;
	अन्यथा जर (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_hChildStd_IN_Wr, "", 1, &len, रिक्त);
	}
	अन्यथा
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_hChildStd_IN_Wr, (अक्षर *)lp + written,
		      s == रिक्त ? l : (DWORD)(s - (lp + written)),
		      &len, रिक्त);
	}
	जर (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    जर (lnum != curbuf->b_op_end.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_ओळ_मोजणी
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_hChildStd_IN_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, रिक्त);
	    }

	    ++lnum;
	    जर (lnum > curbuf->b_op_end.lnum)
		थांबा;

	    lp = ml_get(lnum);
	    written = 0;
	}
	अन्यथा जर (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_hChildStd_IN_Wr);
    परत 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    स्थिर शून्य
dump_pipe(पूर्णांक	    options,
	  HANDLE    g_hChildStd_OUT_Rd,
	  garray_T  *ga,
	  char_u    buffer[],
	  DWORD	    *buffer_off)
{
    DWORD	availableBytes = 0;
    DWORD	i;
    पूर्णांक		ret;
    DWORD	len;
    DWORD	toRead;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_hChildStd_OUT_Rd, // pipe to query
			रिक्त,		    // optional buffer
			0,		    // buffer size
			रिक्त,		    // number of read bytes
			&availableBytes,    // available bytes total
			रिक्त);		    // byteLeft

    // We got real data in the pipe, read it
    पर्यंत (ret != 0 && availableBytes > 0)
    {
	toRead = (DWORD)(BUFLEN - *buffer_off);
	toRead = availableBytes < toRead ? availableBytes : toRead;
	ReadFile(g_hChildStd_OUT_Rd, buffer + *buffer_off, toRead , &len, रिक्त);

	// If we haven't read anything, there is a problem
	जर (len == 0)
	    थांबा;

	availableBytes -= len;

	जर (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    साठी (i = 0; i < len; ++i)
	    {
		जर (buffer[i] == NL)
		    append_ga_ओळ(ga);
		अन्यथा जर (buffer[i] == NUL)
		    ga_append(ga, NL);
		अन्यथा
		    ga_append(ga, buffer[i]);
	    }
	}
	अन्यथा जर (has_mbyte)
	{
	    पूर्णांक		l;
	    पूर्णांक		c;
	    char_u	*p;

	    len += *buffer_off;
	    buffer[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    साठी (p = buffer; p < buffer + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		जर (l == 0)
		    l = 1;  // NUL byte?
		अन्यथा जर (MB_BYTE2LEN(*p) != l)
		    थांबा;
	    }
	    जर (p == buffer)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		जर (len >= 12)
		    ++p;
		अन्यथा
		{
		    *buffer_off = len;
		    परत;
		}
	    }
	    c = *p;
	    *p = NUL;
	    संदेश_puts((अक्षर *)buffer);
	    जर (p < buffer + len)
	    {
		*p = c;
		*buffer_off = (DWORD)((buffer + len) - p);
		mch_memmove(buffer, p, *buffer_off);
		परत;
	    }
	    *buffer_off = 0;
	}
	अन्यथा
	{
	    buffer[len] = NUL;
	    संदेश_puts((अक्षर *)buffer);
	}

	windgoto(संदेश_row, cmdline_col_off + संदेश_col);
	cursor_on();
	out_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    स्थिर पूर्णांक
mch_system_piped(अक्षर *cmd, पूर्णांक options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_hChildStd_IN_Rd = रिक्त;
    HANDLE g_hChildStd_IN_Wr = रिक्त;
    HANDLE g_hChildStd_OUT_Rd = रिक्त;
    HANDLE g_hChildStd_OUT_Wr = रिक्त;

    char_u	buffer[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    पूर्णांक		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    पूर्णांक		noread_cnt = 0;
    garray_T	ga;
    पूर्णांक		delay = 1;
    DWORD	buffer_off = 0;	// valid bytes in buffer[]
    अक्षर	*p = रिक्त;

    SECURITY_ATTRIBUTES saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = आकार(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = रिक्त;

    जर ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)
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
	संदेश_puts(_("\nCannot create pipes\n"));
    }

    si.cb = आकार(si);
    si.lpReserved = रिक्त;
    si.lpDesktop = रिक्त;
    si.lpTitle = रिक्त;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_hChildStd_OUT_Wr;
    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.h_Std_आदान = g_hChildStd_IN_Rd;
    si.wShowWindow = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = रिक्त;

    जर (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    जर (cmd != रिक्त)
    {
	p = (अक्षर *)vim_strsave((char_u *)cmd);
	जर (p != रिक्त)
	    unescape_shellxquote((char_u *)p, p_sxe);
	अन्यथा
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_MODE,
						&si, &pi, रिक्त, रिक्त);

    जर (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(g_hChildStd_OUT_Wr);

    जर (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(रिक्त,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_hChildStd_IN_Wr,  // parameter
			0,		 // creation flag, start immediately
			रिक्त);		    // we don't care about thread id
	CloseHandle(thread);
	g_hChildStd_IN_Wr = रिक्त;
    }

    // Keep updating the window while waiting for the shell to finish.
    साठी (;;)
    {
	संदेश_2	संदेश;

	जर (PeekMessageW(&संदेश, (HWND)रिक्त, 0, 0, PM_REMOVE))
	{
	    TranslateMessage(&संदेश);
	    DispatchMessageW(&संदेश);
	}

	// write pipe information in the window
	जर ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.in_use
# endif
	    )
	{
	    len = 0;
	    जर (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		जर (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		जर (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    जर (len == 1 && cmd != रिक्त)
		    {
			जर (ta_buf[ta_len] == Ctrl_C)
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
		    जर ((len == 1 || len == 4 ) && cmd != रिक्त)
		    {
			जर (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_hChildStd_IN_Wr);
			    g_hChildStd_IN_Wr = रिक्त;
			    len = 0;
			}
		    }

		    len = term_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    साठी (i = ta_len; i < ta_len + len; ++i)
		    {
			जर (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    संदेश_putchar(ta_buf[i]);
			अन्यथा जर (has_mbyte)
			{
			    पूर्णांक l = (*mb_ptr2len)(ta_buf + i);

			    संदेश_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			अन्यथा
			    संदेश_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(संदेश_row, cmdline_col_off + संदेश_col);
		    out_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    जर (options & SHELL_WRITE)
			ta_len = 0;
		    अन्यथा जर (g_hChildStd_IN_Wr != रिक्त)
		    {
			WriteFile(g_hChildStd_IN_Wr, (अक्षर*)ta_buf,
				    1, &len, रिक्त);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			जर (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	जर (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	जर (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);
	    थांबा;
	}

	++noread_cnt;
	dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	जर (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_hChildStd_OUT_Rd);
    जर (g_hChildStd_IN_Wr != रिक्त)
	CloseHandle(g_hChildStd_IN_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    GetExitCodeProcess(pi.hProcess, &ret);

    जर (options & SHELL_READ)
    {
	जर (ga.ga_len > 0)
	{
	    append_ga_ओळ(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	अन्यथा
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    परत ret;
}

    स्थिर पूर्णांक
mch_system_g(अक्षर *cmd, पूर्णांक options)
{
    // if we can pipe and the shelltemp option is off
    जर (!p_stmp)
	परत mch_system_piped(cmd, options);
    अन्यथा
	परत mch_system_classic(cmd, options);
}
#endif

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    स्थिर पूर्णांक
mch_system_c(अक्षर *cmd, पूर्णांक options UNUSED)
{
    पूर्णांक		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    माप_प्रकार	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    जर (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	जर (buf == रिक्त)
	    परत -1;
	vim_snprintf((अक्षर *)buf, len, "(%s)", cmd);
	wcmd = enc_to_utf16(buf, रिक्त);
	मुक्त(buf);
    }
    अन्यथा
	wcmd = enc_to_utf16((char_u *)cmd, रिक्त);

    जर (wcmd == रिक्त)
	परत -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    परत ret;
}

#endif

    स्थिर पूर्णांक
mch_system(अक्षर *cmd, पूर्णांक options)
{
#ifdef VIMDLL
    जर (gui.in_use || gui.starting)
	परत mch_system_g(cmd, options);
    अन्यथा
	परत mch_system_c(cmd, options);
#किंवाजर defined(FEAT_GUI_MSWIN)
    परत mch_system_g(cmd, options);
#अन्यथा
    परत mch_system_c(cmd, options);
#endif
}

#जर defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    स्थिर पूर्णांक
mch_call_shell_terminal(
    char_u	*cmd,
    पूर्णांक		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = रिक्त;
    typval_T	argvar[2];
    long_u	cmdlen;
    पूर्णांक		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    जर (cmd == रिक्त)
	cmdlen = STRLEN(p_sh) + 1;
    अन्यथा
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    जर (newcmd == रिक्त)
	परत 255;
    जर (cmd == रिक्त)
    {
	STRCPY(newcmd, p_sh);
	ch_log(रिक्त, "starting terminal to run a shell");
    }
    अन्यथा
    {
	vim_snprintf((अक्षर *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(रिक्त, "starting terminal for system command '%s'", cmd);
    }

    init_job_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = term_start(argvar, रिक्त, &opt, TERM_START_SYSTEM);
    जर (buf == रिक्त)
    {
	vim_free(newcmd);
	परत 255;
    }

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    जर (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	पर्यंत (term_use_loop())
	{
	    जर (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		जर (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    अन्यथा
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(रिक्त, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    परत retval;
}
#endif
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    स्थिर शून्य
restore_env_var(char_u *name, char_u *old_मूल्य, पूर्णांक must_free)
{
    जर (old_मूल्य != रिक्त)
    {
	vim_setenv(name, old_मूल्य);
	जर (must_free)
	    vim_free(old_मूल्य);
	परत;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    पूर्णांक
mch_call_shell(
    char_u  *cmd,
    पूर्णांक	    options)	// SHELL_*, see vim.h
{
    पूर्णांक		x = 0;
    पूर्णांक		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    पूर्णांक		must_free;
    char_u	*oldval;

#ifdef FEAT_EVAL
    ch_log(रिक्त, "executing shell command: %s", cmd);
#endif
    // Change the title to reflect that we are in a subshell.
    जर (GetConsoleTitleW(szShellTitle, ARRAY_LENGTH(szShellTitle) - 4) > 0)
    {
	जर (cmd == रिक्त)
	    wcscat(szShellTitle, L" :sh");
	अन्यथा
	{
	    WCHAR *wn = enc_to_utf16((char_u *)cmd, रिक्त);

	    जर (wn != रिक्त)
	    {
		wcscat(szShellTitle, L" - !");
		जर ((wcslen(szShellTitle) + wcslen(wn) <
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
    जर (fdDump)
    {
	ख_लिखो(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	ख_साफ(fdDump);
    }
#endif
#जर defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    जर (
# ifdef VIMDLL
	    gui.in_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != रिक्त
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	जर (cmdbase != रिक्त)
	    // Skip a leading quote and (.
	    पर्यंत (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	जर (cmdbase == रिक्त || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    परत x;
	}
    }
#endif

    /*
     * Catch all deadly signals while running the external command, because a
     * CTRL-C, Ctrl-Break or illegal instruction  might otherwise kill us.
     */
    mch_signal(संक_विघ्न, संक_छोड़ो);
    mch_signal(SIGBREAK, संक_छोड़ो);
    mch_signal(संक_अवैध, संक_छोड़ो);
    mch_signal(संक_भ_त्रुटि, संक_छोड़ो);
    mch_signal(संक_अंश, संक_छोड़ो);
    mch_signal(संक_इति, संक_छोड़ो);
    mch_signal(SIGABRT, संक_छोड़ो);

    जर (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    जर (cmd == रिक्त)
    {
	x = mch_system((अक्षर *)p_sh, options);
    }
    अन्यथा
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = रिक्त;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	जर (*cmdbase == '"' )
	    ++cmdbase;
	जर (*cmdbase == '(')
	    ++cmdbase;

	जर ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    ZeroMemory(&si, आकार(si));
	    si.cb = आकार(si);
	    si.lpReserved = रिक्त;
	    si.lpDesktop = रिक्त;
	    si.lpTitle = रिक्त;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = रिक्त;

	    cmdbase = skipwhite(cmdbase + 5);
	    जर ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    अन्यथा जर ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_WINDOW;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_आदान = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    रिक्त,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_NORMAL,		// File att.
		    रिक्त);				// Temp file
		si.hStdOutput = si.h_Std_आदान;
		si.hStdError = si.h_Std_आदान;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    जर (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		जर (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		जर (p > cmdbase && p[-1] == ')'
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
	    जर (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		जर (cmd_shell == रिक्त || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		जर (subcmd != रिक्त)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    जर (newcmd != रिक्त)
			vim_snprintf((अक्षर *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    अन्यथा
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    जर (vim_create_process((अक्षर *)newcmd, FALSE, flags,
			&si, &pi, रिक्त, रिक्त))
		x = 0;
	    अन्यथा जर (vim_shell_execute((अक्षर *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    अन्यथा
	    {
		x = -1;
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
		जर (gui.in_use)
# endif
		    emsg(_(e_command_not_found));
#endif
	    }

	    जर (newcmd != cmdbase)
		vim_free(newcmd);

	    जर (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_आदान != रिक्त)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_आदान);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	अन्यथा
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
	    जर (newcmd != रिक्त)
	    {
#जर defined(FEAT_GUI_MSWIN)
		जर (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    अक्षर *संदेश = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    अक्षर *title = _("Vim Warning");
		    WCHAR *wmsg = enc_to_utf16((char_u *)संदेश, रिक्त);
		    WCHAR *wtitle = enc_to_utf16((char_u *)title, रिक्त);

		    जर (wmsg != रिक्त && wtitle != रिक्त)
			MessageBoxW(रिक्त, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		जर (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((अक्षर *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_path.string,
			    (संदेश_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		अन्यथा जर (
# ifdef VIMDLL
			(gui.in_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((अक्षर *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		अन्यथा
#endif
		    vim_snprintf((अक्षर *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((अक्षर *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    जर (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    जर (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#जर defined(FEAT_GUI_MSWIN)
	    && ((gui.in_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#endif
	    )
    {
	smsg(_("shell returned %d"), x);
	संदेश_putchar('\n');
    }
    resettitle();

    mch_signal(संक_विघ्न, संक_पूर्व);
    mch_signal(SIGBREAK, संक_पूर्व);
    mch_signal(संक_अवैध, संक_पूर्व);
    mch_signal(संक_भ_त्रुटि, संक_पूर्व);
    mch_signal(संक_अंश, संक_पूर्व);
    mch_signal(संक_इति, संक_पूर्व);
    mch_signal(SIGABRT, संक_पूर्व);

    परत x;
}

#जर defined(FEAT_JOB_CHANNEL)
    स्थिर HANDLE
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

    wn = enc_to_utf16(fname, रिक्त);
    जर (wn == रिक्त)
	परत INVALID_HANDLE_मूल्य;

    h = CreateFileW(wn, dwDesiredAccess, dwShareMode,
	    lpSecurityAttributes, dwCreationDisposition,
	    dwFlagsAndAttributes, रिक्त);
    vim_free(wn);
    परत h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    शून्य
win32_build_env(dict_T *env, garray_T *gap, पूर्णांक is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != रिक्त ? env->dv_hashtab.ht_used : 0;
    LPVOID	base = GetEnvironmentStringsW();

    // for last \0
    जर (ga_grow(gap, 1) == FAIL)
	परत;

    जर (env != रिक्त)
    {
	FOR_ALL_HASHTAB_वस्तुसंच(&env->dv_hashtab, hi, todo)
	{
	    जर (!HASHITEM_EMPTY(hi))
	    {
		typval_T *item = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_to_utf16((char_u *)hi->hi_किल्ली, रिक्त);
		WCHAR   *wval = enc_to_utf16(tv_get_string(item), रिक्त);
		--todo;
		जर (wkey != रिक्त && wval != रिक्त)
		{
		    माप_प्रकार	n;
		    माप_प्रकार	lkey = wcslen(wkey);
		    माप_प्रकार	lval = wcslen(wval);

		    जर (ga_grow(gap, (पूर्णांक)(lkey + lval + 2)) == FAIL)
			सुरूठेवा;
		    साठी (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'=';
		    साठी (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    जर (base)
    {
	WCHAR	*p = (WCHAR*) base;

	// for last \0
	जर (ga_grow(gap, 1) == FAIL)
	    परत;

	पर्यंत (*p != 0 || *(p + 1) != 0)
	{
	    जर (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_data + gap->ga_len++) = *p;
	    p++;
	}
	FreeEnvironmentStringsW(base);
	*((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
    }

# जर defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
    {
#  ifdef FEAT_CLIENTSERVER
	char_u	*servername = get_vim_var_str(VV_SEND_SERVER);
	माप_प्रकार	servername_len = STRLEN(servername);
#  endif
#  ifdef FEAT_TERMINAL
	char_u	*version = get_vim_var_str(VV_VERSION);
	माप_प्रकार	version_len = STRLEN(version);
#  endif
	// size of "VIM_SERVERNAME=" and value,
	// plus "VIM_TERMINAL=" and value,
	// plus two terminating NULs
	माप_प्रकार	n = 0
#  ifdef FEAT_CLIENTSERVER
		    + 15 + servername_len
#  endif
#  ifdef FEAT_TERMINAL
		    + 13 + version_len + 2
#  endif
		    ;

	जर (ga_grow(gap, (पूर्णांक)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    साठी (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    साठी (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    जर (is_terminal)
	    {
		साठी (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_data + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		साठी (n = 0; n < version_len; n++)
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
    स्थिर BOOL
create_pipe_pair(HANDLE handles[2])
{
    स्थिर LONG		s;
    अक्षर		name[64];
    SECURITY_ATTRIBUTES sa;

    प्र_लिखो(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_SIZE, 0, 0, रिक्त);

    जर (handles[1] == INVALID_HANDLE_मूल्य)
	परत FALSE;

    sa.nLength = आकार(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = रिक्त;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    जर (handles[0] == INVALID_HANDLE_मूल्य)
    {
	CloseHandle(handles[1]);
	परत FALSE;
    }

    परत TRUE;
}

#endif // FEAT_JOB_CHANNEL

#जर defined(FEAT_EVAL)
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
    अक्षर	**argv,
    char_u	*infile,
    पूर्णांक		flags UNUSED,
    पूर्णांक		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;
    HANDLE		hChildStdoutRd = INVALID_HANDLE_मूल्य;
    HANDLE		hChildStdoutWr = INVALID_HANDLE_मूल्य;
    HANDLE		hChildStdinRd = INVALID_HANDLE_मूल्य;
    garray_T		cmd_ga;
    garray_T		out_ga;
    char_u		*buffer = रिक्त;
    DWORD		exit_code = (DWORD)-1;
    पूर्णांक			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    साठी (i = 0; argv[i] != रिक्त; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	पूर्णांक	has_spaces = FALSE;
	पूर्णांक	j;

	साठी (j = 0; s[j] != NUL; j++)
	    जर (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		थांबा;
	    }

	जर (i > 0)
	    ga_append(&cmd_ga, ' ');

	जर (has_spaces)
	{
	    पूर्णांक	अंक_bs;

	    ga_append(&cmd_ga, '"');
	    साठी (j = 0; arg[j] != NUL; j++)
	    {
		अंक_bs = 0;
		पर्यंत (arg[j] == '\\')
		{
		    अंक_bs++;
		    j++;
		}

		जर (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    पर्यंत (अंक_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    थांबा;
		}
		अन्यथा जर (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    पर्यंत (अंक_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		अन्यथा
		{
		    पर्यंत (अंक_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	अन्यथा
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&out_ga, 1, 4096);

    saAttr.nLength = आकार(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = रिक्त;

    // Create a pipe for the child's stdout.
    जर (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)
	    || !SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	जा done;
    }

    // Set up stdin from infile if provided.
    जर (infile != रिक्त)
    {
	WCHAR *winfile = enc_to_utf16(infile, रिक्त);

	जर (winfile != रिक्त)
	{
	    hChildStdinRd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL, रिक्त);
	    vim_free(winfile);
	}
    }

    ZeroMemory(&pi, आकार(pi));
    ZeroMemory(&si, आकार(si));
    si.cb = आकार(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;
    si.h_Std_आदान = (hChildStdinRd != INVALID_HANDLE_मूल्य)
		    ? hChildStdinRd : INVALID_HANDLE_मूल्य;

    ch_log(रिक्त, "directly executing: %s", (अक्षर *)cmd_ga.ga_data);

    // Create the child process directly, without going through the shell.
    जर (!vim_create_process((अक्षर *)cmd_ga.ga_data, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
		&si, &pi, रिक्त, रिक्त))
    {
	semsg(_(e_invalid_argument_str), cmd_ga.ga_data);
	जा done;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(hChildStdoutWr);
    hChildStdoutWr = INVALID_HANDLE_मूल्य;
    जर (hChildStdinRd != INVALID_HANDLE_मूल्य)
    {
	CloseHandle(hChildStdinRd);
	hChildStdinRd = INVALID_HANDLE_मूल्य;
    }

    // Read output from child process.
    साठी (;;)
    {
	अक्षर	buf[4096];
	DWORD	n;

	जर (!ReadFile(hChildStdoutRd, buf, आकार(buf), &n, रिक्त) || n == 0)
	    थांबा;
	जर (ga_grow(&out_ga, (पूर्णांक)n) == OK)
	{
	    mch_memmove((अक्षर *)out_ga.ga_data + out_ga.ga_len, buf, n);
	    out_ga.ga_len += (पूर्णांक)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    set_vim_var_nr(VV_SHELL_ERROR, (दीर्घ)exit_code);

    जर (out_ga.ga_len > 0)
    {
	buffer = alloc(out_ga.ga_len + 1);
	जर (buffer != रिक्त)
	{
	    mch_memmove(buffer, out_ga.ga_data, out_ga.ga_len);
	    जर (ret_len == रिक्त)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		साठी (i = 0; i < out_ga.ga_len; ++i)
		    जर (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[out_ga.ga_len] = NUL;
	    }
	    अन्यथा
		*ret_len = out_ga.ga_len;
	}
    }
    अन्यथा जर (ret_len != रिक्त)
	*ret_len = 0;

done:
    ga_clear(&cmd_ga);
    ga_clear(&out_ga);
    जर (hChildStdoutRd != INVALID_HANDLE_मूल्य)
	CloseHandle(hChildStdoutRd);
    जर (hChildStdoutWr != INVALID_HANDLE_मूल्य)
	CloseHandle(hChildStdoutWr);
    जर (hChildStdinRd != INVALID_HANDLE_मूल्य)
	CloseHandle(hChildStdinRd);
    परत buffer;
}
#endif // FEAT_EVAL

#जर defined(FEAT_JOB_CHANNEL)
    शून्य
mch_job_start(अक्षर *cmd, job_T *job, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_ATTRIBUTES saAttr;
    channel_T		*channel = रिक्त;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    पूर्णांक		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    पूर्णांक		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    पूर्णांक		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    पूर्णांक		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    पूर्णांक		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    पूर्णांक		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    पूर्णांक		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;

    जर (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_मूल्य;
    ifd[1] = INVALID_HANDLE_मूल्य;
    ofd[0] = INVALID_HANDLE_मूल्य;
    ofd[1] = INVALID_HANDLE_मूल्य;
    efd[0] = INVALID_HANDLE_मूल्य;
    efd[1] = INVALID_HANDLE_मूल्य;
    ga_init2(&ga, आकार(ब_अक्षर_प्रकार), 500);

    jo = CreateJobObject(रिक्त, रिक्त);
    जर (jo == रिक्त)
    {
	job->jv_status = JOB_FAILED;
	जा failed;
    }

    जर (options->jo_env != रिक्त)
	win32_build_env(options->jo_env, &ga, FALSE);

    ZeroMemory(&pi, आकार(pi));
    ZeroMemory(&si, आकार(si));
    si.cb = आकार(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    saAttr.nLength = आकार(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = रिक्त;

    जर (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	ifd[0] = job_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
	जर (ifd[0] == INVALID_HANDLE_मूल्य)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    जा failed;
	}
    }
    अन्यथा जर (!use_null_for_in
	    && (!create_pipe_pair(ifd)
		|| !SetHandleInformation(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	जा failed;

    जर (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	ofd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	जर (ofd[1] == INVALID_HANDLE_मूल्य)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    जा failed;
	}
    }
    अन्यथा जर (!use_null_for_out &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !SetHandleInformation(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	जा failed;

    जर (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	जर (efd[1] == INVALID_HANDLE_मूल्य)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    जा failed;
	}
    }
    अन्यथा जर (!use_out_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !SetHandleInformation(efd[0], HANDLE_FLAG_INHERIT, 0)))
	जा failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_आदान = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_out_for_err ? ofd[1] : efd[1];

    जर (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	जर (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    जर (channel != रिक्त)
		++channel->ch_refcount;
	}
	अन्यथा
	    channel = add_channel();
	जर (channel == रिक्त)
	    जा failed;
    }

    जर (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_MODE |
	    CREATE_NEW_PROCESS_GROUP |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_data,
	    (अक्षर *)options->jo_cwd))
    {
	CloseHandle(jo);
	job->jv_status = JOB_FAILED;
	जा failed;
    }

    ga_clear(&ga);

    जर (!AssignProcessToJobObject(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = रिक्त;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    job->jv_proc_info = pi;
    job->jv_job_object = jo;
    job->jv_status = JOB_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    जर (!use_out_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    job->jv_channel = channel;
    जर (channel != रिक्त)
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
    परत;

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

    अक्षर *
mch_job_status(job_T *job)
{
    DWORD dwExitCode = 0;

    जर (!GetExitCodeProcess(job->jv_proc_info.hProcess, &dwExitCode)
	    || dwExitCode != STILL_ACTIVE)
    {
	job->jv_exitval = (पूर्णांक)dwExitCode;
	जर (job->jv_status < JOB_ENDED)
	{
	    ch_log(job->jv_channel, "Job ended");
	    job->jv_status = JOB_ENDED;
	}
	परत "dead";
    }
    परत "run";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    HANDLE jobHandles[MAXIMUM_WAIT_OBJECTS];
    job_T *jobArray[MAXIMUM_WAIT_OBJECTS];
    job_T *job = job_list;

    पर्यंत (job != रिक्त)
    {
	DWORD n;
	DWORD result;

	साठी (n = 0; n < MAXIMUM_WAIT_OBJECTS
				       && job != रिक्त; job = job->jv_next)
	{
	    जर (job->jv_status == JOB_STARTED)
	    {
		jobHandles[n] = job->jv_proc_info.hProcess;
		jobArray[n] = job;
		++n;
	    }
	}
	जर (n == 0)
	    सुरूठेवा;
	result = WaitForMultipleObjects(n, jobHandles, FALSE, 0);
	जर (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + n)
	{
	    job_T *wait_job = jobArray[result - WAIT_OBJECT_0];

	    जर (STRCMP(mch_job_status(wait_job), "dead") == 0)
		परत wait_job;
	}
    }
    परत रिक्त;
}

    स्थिर BOOL
terminate_all(HANDLE process, पूर्णांक code)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_मूल्य;
    DWORD	    pid = GetProcessId(process);

    जर (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	जर (h != INVALID_HANDLE_मूल्य)
	{
	    pe.dwSize = आकार(PROCESSENTRY32);
	    जर (!Process32First(h, &pe))
		जा theend;

	    करा
	    {
		जर (pe.th32ParentProcessID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    जर (ph != रिक्त)
		    {
			terminate_all(ph, code);
			CloseHandle(ph);
		    }
		}
	    } पर्यंत (Process32Next(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    परत TerminateProcess(process, code);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    पूर्णांक
mch_signal_job(job_T *job, char_u *how)
{
    पूर्णांक ret;

    जर (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	जर (job->jv_job_object != रिक्त)
	{
	    जर (job->jv_channel != रिक्त && job->jv_channel->ch_anonymous_pipe)
		job->jv_channel->ch_killing = TRUE;
	    परत TerminateJobObject(job->jv_job_object, (UINT)-1) ? OK : FAIL;
	}
	परत terminate_all(job->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    जर (!AttachConsole(job->jv_proc_info.dwProcessId))
	परत FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		job->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    परत ret;
}

/*
 * Clear the data related to "job".
 */
    शून्य
mch_clear_job(job_T *job)
{
    जर (job->jv_status == JOB_FAILED)
	परत;

    जर (job->jv_job_object != रिक्त)
	CloseHandle(job->jv_job_object);
    CloseHandle(job->jv_proc_info.hProcess);
}
#endif


#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    स्थिर शून्य
termcap_mode_start(शून्य)
{
    DWORD cmodein;

    जर (g_fTermcapMode)
	परत;

    SaveConsoleBuffer(&g_cbNonTermcap);

    जर (g_cbTermcap.IsValid)
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
    अन्यथा
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
    जर (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_आदान;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    अन्यथा
    {
	cmodein &= ~ENABLE_MOUSE_आदान;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }
    cmodein |= ENABLE_WINDOW_आदान;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_fTermcapMode = TRUE;
}


/*
 * End termcap mode
 */
    स्थिर शून्य
termcap_mode_end(शून्य)
{
    DWORD cmodein;
    ConsoleBuffer *cb;
    COORD coord;
    DWORD dwDummy;

    जर (!g_fTermcapMode)
	परत;

    SaveConsoleBuffer(&g_cbTermcap);

    GetConsoleMode(g_hConIn, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_आदान | ENABLE_WINDOW_आदान);
    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# अन्यथा
    cb = &g_cbNonTermcap;
# endif
    RestoreConsoleBuffer(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    जर (exiting && use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    जर (!USE_WT && (p_rs || exiting))
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
	जर (exiting && !p_rs)
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


#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    स्थिर शून्य
clear_chars(
    COORD coord,
    DWORD n)
{
    जर (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_hConOut, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    अन्यथा
    {
	set_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    स्थिर शून्य
clear_screen(शून्य)
{
    g_coord.X = g_coord.Y = 0;

    जर (!vtp_working)
	clear_chars(g_coord, Rows * Columns);
    अन्यथा
    {
	set_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    स्थिर शून्य
clear_to_end_of_display(शून्य)
{
    COORD save = g_coord;

    जर (!vtp_working)
	clear_chars(g_coord, (Rows - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    अन्यथा
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
    स्थिर शून्य
clear_to_end_of_ओळ(शून्य)
{
    COORD save = g_coord;

    जर (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    अन्यथा
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
    स्थिर शून्य
scroll(अचिन्हित c_ओळी)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.डावा_3 + 1, g_srScrollRegion.शीर्ष_2 + 1);
    delete_ओळी(c_ओळी);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    स्थिर शून्य
set_scroll_region(
    अचिन्हित डावा,
    अचिन्हित शीर्ष,
    अचिन्हित उजवा,
    अचिन्हित bottom)
{
    जर (डावा >= उजवा
	    || शीर्ष >= bottom
	    || उजवा > (अचिन्हित) Columns - 1
	    || bottom > (अचिन्हित) Rows - 1)
	परत;

    g_srScrollRegion.डावा_3 =   डावा;
    g_srScrollRegion.शीर्ष_2 =    शीर्ष;
    g_srScrollRegion.उजवा_3 =  उजवा;
    g_srScrollRegion.Bottom = bottom;
}

    स्थिर शून्य
set_scroll_region_tb(
    अचिन्हित शीर्ष,
    अचिन्हित bottom)
{
    जर (शीर्ष >= bottom || bottom > (अचिन्हित)Rows - 1)
	परत;

    g_srScrollRegion.शीर्ष_2 = शीर्ष;
    g_srScrollRegion.Bottom = bottom;
}

    स्थिर शून्य
set_scroll_region_lr(
    अचिन्हित डावा,
    अचिन्हित उजवा)
{
    जर (डावा >= उजवा || उजवा > (अचिन्हित)Columns - 1)
	परत;

    g_srScrollRegion.डावा_3 = डावा;
    g_srScrollRegion.उजवा_3 = उजवा;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    स्थिर शून्य
घाला_ओळी(अचिन्हित c_ओळी)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.डावा_3 + 1, g_srScrollRegion.शीर्ष_2 + 1);

    dest.X = g_srScrollRegion.डावा_3;
    dest.Y = g_coord.Y + c_ओळी;

    source.डावा_3   = g_srScrollRegion.डावा_3;
    source.शीर्ष_2	  = g_coord.Y;
    source.उजवा_3  = g_srScrollRegion.उजवा_3;
    source.Bottom = g_srScrollRegion.Bottom - c_ओळी;

    clip.डावा_3   = g_srScrollRegion.डावा_3;
    clip.शीर्ष_2    = g_coord.Y;
    clip.उजवा_3  = g_srScrollRegion.उजवा_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    जर (!USE_VTP)
	fill.Attributes = g_attrCurrent;
    अन्यथा
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    जर (source.Bottom < dest.Y)
    {
	COORD coord;
	पूर्णांक   i;

	coord.X = source.डावा_3;
	साठी (i = clip.शीर्ष_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.उजवा_3 - source.डावा_3 + 1);
	}
    }

    जर (vtp_working)
    {
	COORD coord;
	पूर्णांक i;

	coord.X = source.डावा_3;
	साठी (i = source.शीर्ष_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.उजवा_3 - source.डावा_3 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    स्थिर शून्य
delete_ओळी(अचिन्हित c_ओळी)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    पूर्णांक		    nb;

    gotoxy(g_srScrollRegion.डावा_3 + 1, g_srScrollRegion.शीर्ष_2 + 1);

    dest.X = g_srScrollRegion.डावा_3;
    dest.Y = g_coord.Y;

    source.डावा_3   = g_srScrollRegion.डावा_3;
    source.शीर्ष_2	  = g_coord.Y + c_ओळी;
    source.उजवा_3  = g_srScrollRegion.उजवा_3;
    source.Bottom = g_srScrollRegion.Bottom;

    clip.डावा_3   = g_srScrollRegion.डावा_3;
    clip.शीर्ष_2    = g_coord.Y;
    clip.उजवा_3  = g_srScrollRegion.उजवा_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    जर (!vtp_working)
	fill.Attributes = g_attrCurrent;
    अन्यथा
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (source.Bottom - source.शीर्ष_2) + 1;

    जर (nb < source.शीर्ष_2)
    {
	COORD coord;
	पूर्णांक   i;

	coord.X = source.डावा_3;
	साठी (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.उजवा_3 - source.डावा_3 + 1);
	}
    }

    जर (vtp_working)
    {
	COORD coord;
	पूर्णांक i;

	coord.X = source.डावा_3;
	साठी (i = nb; i <= source.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.उजवा_3 - source.डावा_3 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    स्थिर शून्य
gotoxy(
    अचिन्हित x,
    अचिन्हित y)
{
    जर (x < 1 || x > (अचिन्हित)Columns || y < 1 || y > (अचिन्हित)Rows)
	परत;

    जर (!USE_VTP)
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
    अन्यथा
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
    स्थिर शून्य
textattr(WORD wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    SetConsoleTextAttribute(g_hConOut, wAttr);
}


    स्थिर शून्य
textcolor(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    जर (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    अन्यथा
	vtp_sgr_bulk(wAttr);
}


    स्थिर शून्य
textbackground(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    जर (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    अन्यथा
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    स्थिर शून्य
normvideo(शून्य)
{
    जर (!vtp_working)
	textattr(g_attrDefault);
    अन्यथा
	vtp_sgr_bulk(0);
}


स्थिर WORD g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    स्थिर शून्य
standout(शून्य)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((WORD) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    स्थिर शून्य
standend(शून्य)
{
    जर (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    शून्य
mch_set_normal_colors(शून्य)
{
    char_u	*p;
    पूर्णांक		n;

    cterm_normal_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_normal_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    जर (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	जर (*p == 'm' && n > 0)
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
    स्थिर शून्य
visual_bell(शून्य)
{
    COORD   coordOrigin = {0, 0};
    WORD    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = रिक्त;

# ifdef FEAT_TERMGUICOLORS
    जर (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(WORD, Rows * Columns);
	जर (oldattrs == रिक्त)
	    परत;
	ReadConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_hConOut, attrFlash, Rows * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    जर (oldattrs != रिक्त)
    {
	WriteConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    स्थिर शून्य
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    जर (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	परत;
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
    स्थिर DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cbToWrite)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    स्थिर DWORD    cells;
    स्थिर WCHAR    *unicodebuf = रिक्त;
    स्थिर पूर्णांक	    unibuflen = 0;
    स्थिर पूर्णांक	    length;
    पूर्णांक		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    स्थिर WCHAR    *utf8spbuf = रिक्त;
    स्थिर पूर्णांक	    utf8splength;
    स्थिर DWORD    utf8spcells;
    स्थिर WCHAR    **utf8usingbuf = &unicodebuf;

    जर (cbToWrite != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	करा
	{
	    length = MultiByteToWideChar(cp, 0, (LPCSTR)pchBuf, cbToWrite,
							unicodebuf, unibuflen);
	    जर (length && length <= unibuflen)
		थांबा;
	    vim_free(unicodebuf);
	    unicodebuf = length ? LALLOC_MULT(WCHAR, length) : रिक्त;
	    unibuflen = unibuflen ? 0 : length;
	} पर्यंत (TRUE);
	cells = mb_string2cells(pchBuf, cbToWrite);
    }
    अन्यथा // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	जर (utf8usingbuf != &utf8spbuf)
	{
	    जर (utf8spbuf == रिक्त)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		length = MultiByteToWideChar(CP_UTF8, 0, " ", 1, रिक्त, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, length);
		जर (utf8spbuf != रिक्त)
		{
		    MultiByteToWideChar(CP_UTF8, 0, " ", 1, utf8spbuf, length);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = length;
		    utf8spcells = cells;
		}
	    }
	    अन्यथा
	    {
		utf8usingbuf = &utf8spbuf;
		length = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    जर (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	जर (WriteConsoleOutputCharacterW(g_hConOut, *utf8usingbuf, length,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    अन्यथा
    {
	जर (WriteConsoleW(g_hConOut, *utf8usingbuf, length, &cchwritten,
		    रिक्त) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    जर (cchwritten == (DWORD)length)
    {
	written = cbToWrite;
	g_coord.X += (SHORT)cells;
    }
    अन्यथा
    {
	char_u *p = pchBuf;
	साठी (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    पर्यंत (g_coord.X > g_srScrollRegion.उजवा_3)
    {
	g_coord.X -= (SHORT) Columns;
	जर (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    जर (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    परत written;
}

    स्थिर char_u *
get_seq(
    पूर्णांक *args,
    पूर्णांक *मोजणी,
    char_u *head)
{
    पूर्णांक argc;
    char_u *p;

    जर (head == रिक्त || *head != '\033')
	परत रिक्त;

    argc = 0;
    p = head;
    ++p;
    करा
    {
	++p;
	args[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } पर्यंत (*p == ';');
    *मोजणी = argc;

    परत p;
}

    स्थिर char_u *
get_sgr(
    पूर्णांक *args,
    पूर्णांक *मोजणी,
    char_u *head)
{
    char_u *p = get_seq(args, मोजणी, head);

    परत (p && *p == 'm') ? ++p : रिक्त;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    स्थिर char_u *
sgrn2(
    char_u *head,
    पूर्णांक	   n)
{
    पूर्णांक argc;
    पूर्णांक args[16];
    char_u *p = get_sgr(args, &argc, head);

    परत p && argc == 5 && args[0] == n && args[1] == 2 ? p : रिक्त;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    स्थिर char_u *
sgrnc(
    char_u *head,
    पूर्णांक	   n)
{
    पूर्णांक argc;
    पूर्णांक args[16];
    char_u *p = get_sgr(args, &argc, head);

    परत p && argc == 1 && args[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : रिक्त;
}

    स्थिर char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    पर्यंत (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    परत p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    स्थिर char_u *
sgrn2c(
    char_u *head,
    पूर्णांक	   n)
{
    char_u *p = sgrn2(head, n);

    परत p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : रिक्त;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    स्थिर char_u *
sgrn2cn(
    char_u *head,
    पूर्णांक n)
{
    char_u *p = sgrn2(head, n);

    परत p && p[0] == 0x0a && p[1] == '\033' ? ++p : रिक्त;
}

स्थिर bool in_dcs = false;	// inside a DCS (sixel) sequence
स्थिर bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    स्थिर शून्य
write_vt(char_u *s, पूर्णांक len)
{
    पूर्णांक		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, रिक्त, 0);
    जर (wlen <= 0)
	परत;
    ws = ALLOC_MULT(WCHAR, wlen);
    जर (ws == रिक्त)
	परत;
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_hConOut, ws, wlen, &written, रिक्त);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    स्थिर char_u *
skip_dcs(char_u *s, char_u *end)
{
    साठी (char_u *p = s; p < end; ++p)
    {
	जर (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    in_dcs = false;
	    परत p + 1;
	}
	dcs_esc = *p == ESC;
    }
    परत end;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    शून्य
mch_write(
    char_u  *s,
    पूर्णांक	    len)
{
    char_u  *end = s + len;

# ifdef VIMDLL
    जर (gui.in_use)
	परत;
# endif

    जर (!term_console)
    {
	लिख(1, s, (अचिन्हित)len);
	परत;
    }

    // translate ESC | sequences into faked bios calls
    पर्यंत (len--)
    {
	पूर्णांक prefix = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	जर (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    परत;
	}

	जर (in_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    पूर्णांक	l = (पूर्णांक)(skip_dcs(s, end) - s);

	    जर (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    सुरूठेवा;
	}

	पर्यंत (s + ++prefix < end)
	{
	    ch = s[prefix];
	    जर (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		थांबा;
	}

	जर (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    जर (prefix != 0)
		prefix = 1;
	}

	जर (prefix != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, prefix);
# ifdef MCH_WRITE_DUMP
	    जर (fdDump)
	    {
		ख_अक्षर_दो('>', fdDump);
		ख_डालो(s, आकार(char_u), nWritten, fdDump);
		ख_माला_दो("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	अन्यथा जर (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    जर (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.डावा_3 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    अन्यथा
	    {
		gotoxy(g_srScrollRegion.डावा_3 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    जर (fdDump)
		ख_माला_दो("\\n\n", fdDump);
# endif
	    s++;
	}
	अन्यथा जर (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.डावा_3+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    जर (fdDump)
		ख_माला_दो("\\r\n", fdDump);
# endif
	    s++;
	}
	अन्यथा जर (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    जर (g_coord.X > g_srScrollRegion.डावा_3)
		g_coord.X--;
	    अन्यथा जर (g_coord.Y > g_srScrollRegion.शीर्ष_2)
	    {
		g_coord.X = g_srScrollRegion.उजवा_3;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    जर (fdDump)
		ख_माला_दो("\\b\n", fdDump);
# endif
	    s++;
	}
	अन्यथा जर (s[0] == '\a')
	{
	    // \a, bell
	    MessageBeep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    जर (fdDump)
		ख_माला_दो("\\a\n", fdDump);
# endif
	    s++;
	}
	अन्यथा जर (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    पूर्णांक	    arg1 = 0, arg2 = 0, argc = 0, args[16];
	    char_u  *sp;

	    निवड (s[2])
	    {
	    स्थिती '0': स्थिती '1': स्थिती '2': स्थिती '3': स्थिती '4':
	    स्थिती '5': स्थिती '6': स्थिती '7': स्थिती '8': स्थिती '9':
		जर (*(p = get_seq(args, &argc, s)) != 'm')
		    जा notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		जर (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (पूर्णांक)(p - s);
		    s = p;
		    थांबा;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		जर (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		जर (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		जर (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		जर (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(args, &argc, p);

notsgr:
		arg1 = args[0];
		arg2 = args[1];
		जर (*p == 'm')
		{
		    जर (argc == 1 && args[0] == 0)
			normvideo();
		    अन्यथा जर (argc == 1)
		    {
			जर (USE_VTP)
			    textcolor((WORD)arg1);
			अन्यथा
			    textattr((WORD)arg1);
		    }
		    अन्यथा जर (vtp_working)
			vtp_sgr_bulks(argc, args);
		}
		अन्यथा जर (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		अन्यथा जर (argc == 2 && *p == 'r')
		{
		    set_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		अन्यथा जर (argc == 2 && *p == 'R')
		{
		    set_scroll_region_tb(arg1, arg2);
		}
		अन्यथा जर (argc == 2 && *p == 'V')
		{
		    set_scroll_region_lr(arg1, arg2);
		}
		अन्यथा जर (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.शीर्ष_2, g_coord.Y - arg1) + 1);
		}
		अन्यथा जर (argc == 1 && *p == 'b')
		{
		    textbackground((WORD) arg1);
		}
		अन्यथा जर (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.उजवा_3, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		अन्यथा जर (argc == 1 && *p == 'f')
		{
		    textcolor((WORD) arg1);
		}
		अन्यथा जर (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		अन्यथा जर (argc == 1 && *p == 'L')
		{
		    घाला_ओळी(arg1);
		}
		अन्यथा जर (argc == 1 && *p == 'M')
		{
		    delete_ओळी(arg1);
		}

		len -= (पूर्णांक)(p - s);
		s = p + 1;
		थांबा;

	    स्थिती 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.शीर्ष_2, g_coord.Y - 1) + 1);
		जा got3;

	    स्थिती 'B':
		visual_bell();
		जा got3;

	    स्थिती 'C':
		gotoxy(min(g_srScrollRegion.उजवा_3, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		जा got3;

	    स्थिती 'E':
		termcap_mode_end();
		जा got3;

	    स्थिती 'F':
		standout();
		जा got3;

	    स्थिती 'f':
		standend();
		जा got3;

	    स्थिती 'H':
		gotoxy(1, 1);
		जा got3;

	    स्थिती 'j':
		clear_to_end_of_display();
		जा got3;

	    स्थिती 'J':
		clear_screen();
		जा got3;

	    स्थिती 'K':
		clear_to_end_of_ओळ();
		जा got3;

	    स्थिती 'L':
		घाला_ओळी(1);
		जा got3;

	    स्थिती 'M':
		delete_ओळी(1);
		जा got3;

	    स्थिती 'S':
		termcap_mode_start();
		जा got3;

	    स्थिती 'V':
		cursor_visible(TRUE);
		जा got3;

	    स्थिती 'v':
		cursor_visible(FALSE);
		जा got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    जर (fdDump)
	    {
		ख_माला_दो("ESC | ", fdDump);
		ख_डालो(old_s + 2, आकार(char_u), s - old_s - 2, fdDump);
		ख_अक्षर_दो('\n', fdDump);
	    }
# endif
	}
	अन्यथा जर (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    पूर्णांक l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    पर्यंत (s + l < end && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    जर (s + l < end && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		जर (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	अन्यथा जर (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    पूर्णांक	l;

	    in_dcs = true;
	    dcs_esc = false;
	    l = (पूर्णांक)(skip_dcs(s + 2, end) - s);
	    जर (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	अन्यथा
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    जर (fdDump)
	    {
		ख_अक्षर_दो('>', fdDump);
		ख_डालो(s, आकार(char_u), nWritten, fdDump);
		ख_माला_दो("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    जर (fdDump)
	ख_साफ(fdDump);
# endif
}

#अन्यथा // FEAT_GUI_MSWIN && !VIMDLL
    शून्य
mch_write(
    char_u  *s UNUSED,
    पूर्णांक	    len UNUSED)
{
    // never used
}

#endif


/*
 * Delay for "msec" milliseconds.
 */
    शून्य
mch_delay(
    दीर्घ    msec,
    पूर्णांक	    flags UNUSED)
{
#जर defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((पूर्णांक)msec);	    // never wait for input
#अन्यथा // Console
# ifdef VIMDLL
    जर (gui.in_use)
    {
	Sleep((पूर्णांक)msec);	    // never wait for input
	परत;
    }
# endif
    जर (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	जर (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    पूर्णांक towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    पर्यंत (msec > 0)
	    {
		mzvim_check_threads();
		जर (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	अन्यथा
# endif
	    Sleep((पूर्णांक)msec);
    अन्यथा
	WaitForChar(msec, FALSE);
#endif
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    पूर्णांक
mch_remove(char_u *name)
{
    WCHAR	*wn;
    पूर्णांक		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    जर (mch_isdir(name) && mch_is_symbolic_link(name))
	परत mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_NORMAL);

    wn = enc_to_utf16(name, रिक्त);
    जर (wn == रिक्त)
	परत -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    परत n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    शून्य
mch_breakcheck(पूर्णांक force UNUSED)
{
#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    जर (!gui.in_use)
# endif
	जर (g_fCtrlCPressed || g_fCBrkPressed)
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
mch_total_mem(पूर्णांक special UNUSED)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dwLength = आकार(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&ms);
    जर (ms.ullAvailVirtual < ms.ullTotalPhys)
    {
	// Process address space fits in physical RAM, use all of it.
	परत (long_u)(ms.ullAvailVirtual / 1024);
    }
    जर (ms.ullTotalPhys <= WINNT_RESERVE_BYTES)
    {
	// Catch old NT box or perverse hardware setup.
	परत (long_u)((ms.ullTotalPhys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    परत (long_u)((ms.ullTotalPhys - WINNT_RESERVE_BYTES) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    पूर्णांक
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    पूर्णांक		flags)		// EW_* flags
{
    परत dos_expandpath(gap, path, 0, flags, FALSE);
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
    पूर्णांक
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	szTempFile[_MAX_PATH + 1];
    WCHAR	szNewPath[_MAX_PATH + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    साठी (p = q = wold; *p != L'\0'; ++p)
    {
	निवड (*p)
	{
	स्थिती L'/':
	स्थिती L'\\':
	स्थिती L':':
	    जर (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    थांबा;

	पूर्वनिर्धारित:
	    थांबा;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    जर ((पूर्णांक)(p - q) < 8 || q[6] != L'~')
	परत (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    जर (GetFullPathNameW(wnew, _MAX_PATH, szNewPath, &p) == 0 || p == रिक्त)
	परत -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    जर (GetTempFileNameW(szNewPath, L"VIM", 0, szTempFile) == 0)
	परत -2;

    // blow the temp file away
    जर (!DeleteFileW(szTempFile))
	परत -3;

    // rename old file to the temp file
    जर (!MoveFileW(wold, szTempFile))
	परत -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    जर ((hf = CreateFileW(wold, GENERIC_WRITE, 0, रिक्त, CREATE_NEW,
		    FILE_ATTRIBUTE_NORMAL, रिक्त)) == INVALID_HANDLE_मूल्य)
	परत -5;
    जर (!CloseHandle(hf))
	परत -6;

    // rename the temp file to the new file
    जर (!MoveFileW(szTempFile, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(शून्य)MoveFileW(szTempFile, wold);
	परत -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(szTempFile);

    // finally, remove the empty old file
    जर (!DeleteFileW(wold))
	परत -8;

    परत 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    पूर्णांक
mch_rename(
    स्थिरांक अक्षर	*pszOldFile,
    स्थिरांक अक्षर	*pszNewFile)
{
    WCHAR	*wold = रिक्त;
    WCHAR	*wnew = रिक्त;
    पूर्णांक		retval = -1;

    wold = enc_to_utf16((char_u *)pszOldFile, रिक्त);
    wnew = enc_to_utf16((char_u *)pszNewFile, रिक्त);
    जर (wold != रिक्त && wnew != रिक्त)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    परत retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    अक्षर *
default_shell(शून्य)
{
    परत "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    पूर्णांक
mch_access(अक्षर *n, पूर्णांक p)
{
    HANDLE	hFile;
    पूर्णांक		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_to_utf16((char_u *)n, रिक्त);
    जर (wn == रिक्त)
	परत -1;

    जर (mch_isdir((char_u *)n))
    {
	WCHAR TempNameW[_MAX_PATH + 16] = L"";

	जर (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    पूर्णांक			i;
	    WIN32_FIND_DATAW    d;

	    साठी (i = 0; i < _MAX_PATH && wn[i] != 0; ++i)
		TempNameW[i] = wn[i];
	    जर (TempNameW[i - 1] != '\\' && TempNameW[i - 1] != '/')
		TempNameW[i++] = '\\';
	    TempNameW[i++] = '*';
	    TempNameW[i++] = 0;

	    hFile = FindFirstFileW(TempNameW, &d);
	    जर (hFile == INVALID_HANDLE_मूल्य)
		जा getout;
	    अन्यथा
		(शून्य)FindClose(hFile);
	}

	जर (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    जर (!GetTempFileNameW(wn, L"VIM", 0, TempNameW))
		जा getout;
	    अन्यथा
		DeleteFileW(TempNameW);
	}
    }
    अन्यथा
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_mode = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_mode, share_mode,
					     रिक्त, OPEN_EXISTING, 0, रिक्त);
	जर (hFile == INVALID_HANDLE_मूल्य)
	    जा getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    परत retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    पूर्णांक
mch_open(स्थिरांक अक्षर *name, पूर्णांक flags, पूर्णांक mode)
{
    WCHAR	*wn;
    पूर्णांक		f;

    wn = enc_to_utf16((char_u *)name, रिक्त);
    जर (wn == रिक्त)
	परत -1;

    f = _wopen(wn, flags, mode);
    vim_free(wn);
    परत f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    खाता *
mch_fopen(स्थिरांक अक्षर *name, स्थिरांक अक्षर *mode)
{
    WCHAR	*wn, *wm;
    खाता	*f = रिक्त;

#जर defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    अक्षर newMode = mode[म_माप(mode) - 1];
    पूर्णांक oldMode = 0;

    _get_fmode(&oldMode);
    जर (newMode == 't')
	_set_fmode(_O_TEXT);
    अन्यथा जर (newMode == 'b')
	_set_fmode(O_द्विमान_2);
#endif
    wn = enc_to_utf16((char_u *)name, रिक्त);
    wm = enc_to_utf16((char_u *)mode, रिक्त);
    जर (wn != रिक्त && wm != रिक्त)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#जर defined(DEBUG) && _MSC_VER >= 1400
    जर (oldMode != 0)
	_set_fmode(oldMode);
#endif
    परत f;
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
    स्थिर शून्य
copy_substream(HANDLE sh, शून्य *context, WCHAR *to, WCHAR *substream, दीर्घ len)
{
    HANDLE  hTo;
    WCHAR   *to_name;

    to_name = alloc((wcslen(to) + wcslen(substream) + 1) * आकार(WCHAR));
    जर (to_name == रिक्त)
	परत;

    wcscpy(to_name, to);
    wcscat(to_name, substream);

    hTo = CreateFileW(to_name, GENERIC_WRITE, 0, रिक्त, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL, रिक्त);
    जर (hTo != INVALID_HANDLE_मूल्य)
    {
	दीर्घ	done;
	DWORD	todo;
	DWORD	readcnt, written;
	अक्षर	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	साठी (done = 0; done < len; done += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((माप_प्रकार)(len - done) > आकार(buf) ? आकार(buf)
						       : (माप_प्रकार)(len - done));
	    जर (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(hTo, buf, todo, &written, रिक्त)
		    || written != todo)
		थांबा;
	}
	CloseHandle(hTo);
    }

    मुक्त(to_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    स्थिर शून्य
copy_infostreams(char_u *from, char_u *to)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_STREAM_ID	sid;
    पूर्णांक			headersize;
    WCHAR		streamname[_MAX_PATH];
    DWORD		readcount;
    शून्य		*context = रिक्त;
    DWORD		lo, hi;
    पूर्णांक			len;

    // Convert the file names to wide characters.
    fromw = enc_to_utf16(from, रिक्त);
    tow = enc_to_utf16(to, रिक्त);
    जर (fromw != रिक्त && tow != रिक्त)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, रिक्त,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, रिक्त);
	जर (sh != INVALID_HANDLE_मूल्य)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    साठी (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		ZeroMemory(&sid, आकार(WIN32_STREAM_ID));
		headersize = (पूर्णांक)((अक्षर *)&sid.cStreamName - (अक्षर *)&sid.dwStreamId);
		जर (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    थांबा;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		जर (sid.dwStreamNameSize > 0)
		{
		    // Read the stream name.
		    जर (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dwStreamNameSize,
					  &readcount, FALSE, FALSE, &context))
			थांबा;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / आकार(WCHAR);
		    streamname[len] = 0;
		    जर (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (दीर्घ)sid.Size.u.खालचा_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(शून्य)BackupSeek(sh, sid.Size.u.खालचा_Part, sid.Size.u.वरचा_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (शून्य)BackupRead(sh, रिक्त, 0, &readcount, TRUE, FALSE, &context);

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

प्रकारव्याख्या संरचना _FILE_FULL_EA_INFORMATION_ {
    ULONG  NextEntryOffset;
    UCHAR  Flags;
    UCHAR  EaNameLength;
    USHORT Ea_मूल्य_Length;
    CHAR   EaName[1];
} FILE_FULL_EA_INFORMATION_, *PFILE_FULL_EA_INFORMATION_;

प्रकारव्याख्या संरचना _FILE_EA_INFORMATION_ {
    ULONG EaSize;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#ifndef PROTO
प्रकारव्याख्या NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions);
प्रकारव्याख्या NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
प्रकारव्याख्या NTSTATUS (NTAPI *PfnNtSetEaFile)(
	HANDLE		    FileHandle,
	PIO_STATUS_BLOCK    IoStatusBlock,
	PVOID		    Buffer,
	ULONG		    Length);
प्रकारव्याख्या NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	BOOLEAN ReturnSingleEntry,
	PVOID EaList,
	ULONG EaListLength,
	PULONG EaIndex,
	BOOLEAN RestartScan);
प्रकारव्याख्या NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_STATUS_BLOCK	IoStatusBlock,
	PVOID			FileInformation,
	ULONG			Length,
	FILE_INFORMATION_CLASS FileInformationClass);
प्रकारव्याख्या VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString);

PfnNtOpenFile pNtOpenFile = रिक्त;
PfnNtClose pNtClose = रिक्त;
PfnNtSetEaFile pNtSetEaFile = रिक्त;
PfnNtQueryEaFile pNtQueryEaFile = रिक्त;
PfnNtQueryInformationFile pNtQueryInformationFile = रिक्त;
PfnRtlInitUnicodeString pRtlInitUnicodeString = रिक्त;
#endif

/*
 * Load ntdll.dll functions.
 */
    स्थिर BOOL
load_ntdll(शून्य)
{
    स्थिर पूर्णांक	loaded = -1;

    जर (loaded != -1)
	परत (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    जर (hNtdll != रिक्त)
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
    जर (pNtOpenFile == रिक्त
	    || pNtClose == रिक्त
	    || pNtSetEaFile == रिक्त
	    || pNtQueryEaFile == रिक्त
	    || pNtQueryInformationFile == रिक्त
	    || pRtlInitUnicodeString == रिक्त)
	loaded = FALSE;
    अन्यथा
	loaded = TRUE;
    परत (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    स्थिर शून्य
copy_extattr(char_u *from, char_u *to)
{
    char_u		    *fromf = रिक्त;
    char_u		    *tof = रिक्त;
    WCHAR		    *fromw = रिक्त;
    WCHAR		    *tow = रिक्त;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_ATTRIBUTES	    oa;
    IO_STATUS_BLOCK	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    शून्य		    *ea = रिक्त;

    जर (!load_ntdll())
	परत;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(to) + 5);
    जर (fromf == रिक्त || tof == रिक्त)
	जा theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), to);

    // Convert the names to wide characters.
    fromw = enc_to_utf16(fromf, रिक्त);
    tow = enc_to_utf16(tof, रिक्त);
    जर (fromw == रिक्त || tow == रिक्त)
	जा theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    InitializeObjectAttributes(&oa, &u, 0, रिक्त, रिक्त);
    जर (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	जा theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, आकार(eainfo),
	    FileEaInformation);
    जर (eainfo.EaSize != 0)
    {
	ea = alloc(eainfo.EaSize);
	जर (ea != रिक्त)
	{
	    जर (pNtQueryEaFile(h, &iosb, ea, eainfo.EaSize, FALSE,
			रिक्त, 0, रिक्त, TRUE) != STATUS_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    जर (ea != रिक्त)
    {
	pRtlInitUnicodeString(&u, tow);
	InitializeObjectAttributes(&oa, &u, 0, रिक्त, रिक्त);
	जर (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	    जा theend;

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
    पूर्णांक
mch_copy_file_attribute(char_u *from, char_u *to)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, to);
    copy_extattr(from, to);
    परत 0;
}


/*
 * The command line arguments in UTF-16
 */
स्थिर पूर्णांक	nArgsW = 0;
स्थिर LPWSTR	*ArglistW = रिक्त;
स्थिर पूर्णांक	global_argc = 0;
स्थिर अक्षर	**global_argv;

स्थिर पूर्णांक	used_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
स्थिर पूर्णांक	*used_file_indexes = रिक्त; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
स्थिर पूर्णांक	used_file_मोजणी = 0;	// nr of entries in used_file_indexes
स्थिर पूर्णांक	used_file_literal = FALSE;  // take file names literally
स्थिर पूर्णांक	used_file_full_path = FALSE;  // file name was full path
#ifdef FEAT_DIFF
स्थिर पूर्णांक	used_file_diff_mode = FALSE;  // file name was with diff mode
#endif
स्थिर पूर्णांक	used_alist_मोजणी = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    पूर्णांक
get_cmd_argsW(अक्षर ***argvp)
{
    अक्षर	**argv = रिक्त;
    पूर्णांक		argc = 0;
    पूर्णांक		i;

    free_cmd_argsW();
    ArglistW = Command_ओळ_To_Argv_W(Get_Command_ओळ_W(), &nArgsW);
    जर (ArglistW != रिक्त)
    {
	argv = alloc((nArgsW + 1) * आकार(अक्षर *));
	जर (argv != रिक्त)
	{
	    argc = nArgsW;
	    argv[argc] = रिक्त;
	    साठी (i = 0; i < argc; ++i)
	    {
		पूर्णांक	len;

		// Convert each Unicode argument to UTF-8.
		WideCharToMultiByte_alloc(CP_UTF8, 0,
				ArglistW[i], (पूर्णांक)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		जर (argv[i] == रिक्त)
		{
		    // Out of memory, clear everything.
		    पर्यंत (i > 0)
			मुक्त(argv[--i]);
		    मुक्त(argv);
		    argv = रिक्त;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    जर (argc > 0)
    {
	जर (used_file_indexes != रिक्त)
	    मुक्त(used_file_indexes);
	used_file_indexes = alloc(argc * आकार(पूर्णांक));
    }

    जर (argvp != रिक्त)
	*argvp = argv;
    परत argc;
}

    शून्य
free_cmd_argsW(शून्य)
{
    जर (ArglistW == रिक्त)
	परत;

    GlobalFree(ArglistW);
    ArglistW = रिक्त;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    शून्य
used_file_arg(अक्षर *name, पूर्णांक literal, पूर्णांक full_path, पूर्णांक diff_mode)
{
    पूर्णांक		i;

    जर (used_file_indexes == रिक्त)
	परत;
    साठी (i = used_file_argc + 1; i < global_argc; ++i)
	जर (STRCMP(global_argv[i], name) == 0)
	{
	    used_file_argc = i;
	    used_file_indexes[used_file_मोजणी++] = i;
	    थांबा;
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
    शून्य
set_alist_मोजणी(शून्य)
{
    used_alist_मोजणी = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    शून्य
fix_arg_enc(शून्य)
{
    पूर्णांक		i;
    पूर्णांक		idx;
    char_u	*str;
    पूर्णांक		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    जर (global_argc != nArgsW
	    || ArglistW == रिक्त
	    || used_file_indexes == रिक्त
	    || used_file_मोजणी == 0
	    || used_alist_मोजणी != GARGCOUNT)
	परत;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(पूर्णांक, GARGCOUNT);
    जर (fnum_list == रिक्त)
	परत;		// out of memory
    साठी (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    जर (ga_grow(&global_alist.al_ga, used_file_मोजणी) == FAIL)
	परत;		// out of memory

    साठी (i = 0; i < used_file_मोजणी; ++i)
    {
	idx = used_file_indexes[i];
	str = utf16_to_enc(ArglistW[idx], रिक्त);
	जर (str != रिक्त)
	{
	    पूर्णांक literal = used_file_literal;

#ifdef FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    जर (used_file_diff_mode && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), tail, STRLEN(tail), TRUE, &ret);
		जर (ret.string != रिक्त)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#endif
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    जर (used_file_literal)
		buf_set_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    जर (literal == FALSE)
	    {
		माप_प्रकार len = STRLEN(str);

		जर (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_add(&global_alist, str, literal ? 2 : 0);
	}
    }

    जर (!used_file_literal)
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
	alist_expand(fnum_list, used_alist_मोजणी);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    जर (curwin->w_arg_idx == 0 && curbuf->b_fname == रिक्त)
    {
	do_cmdline_cmd((char_u *)":rewind");
	जर (GARGCOUNT == 1 && used_file_full_path
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    last_chdir_reason = "drop";
    }

    set_alist_मोजणी();
}

    पूर्णांक
mch_setenv(अक्षर *var, अक्षर *मूल्य, पूर्णांक x UNUSED)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(मूल्य) + 2);
    जर (envbuf == रिक्त)
	परत -1;

    प्र_लिखो((अक्षर *)envbuf, "%s=%s", var, मूल्य);

    p = enc_to_utf16(envbuf, रिक्त);

    vim_free(envbuf);
    जर (p == रिक्त)
	परत -1;
    _wputenv(p);
#ifdef libintl_wputenv
    libintl_wputenv(p);
#endif
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    परत 0;
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

    स्थिर शून्य
vtp_flag_init(शून्य)
{
#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   mode;
    HANDLE  out;

# ifdef VIMDLL
    जर (!gui.in_use)
# endif
    {
	out = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_version >= VTP_FIRST_SUPPORT_BUILD) ? 1 : 0;
	GetConsoleMode(out, &mode);
	mode |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	जर (SetConsoleMode(out, mode) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_buffer = win_version >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#endif

    जर (win_version >= CONPTY_FIRST_SUPPORT_BUILD)
	conpty_working = 1;
    जर (win_version >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    जर (win_version <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    जर (win_version <= CONPTY_1909_BUILD)
	conpty_type = 2;
    जर (win_version <= CONPTY_1903_BUILD)
	conpty_type = 2;
    जर (win_version < CONPTY_FIRST_SUPPORT_BUILD)
	conpty_type = 1;

    जर (win_version >= CONPTY_NEXT_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    स्थिर शून्य
vtp_init(शून्य)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = आकार(csbi);
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

    स्थिर शून्य
vtp_exit(शून्य)
{
    restore_console_color_rgb();
}

    पूर्णांक
vtp_printf(
    अक्षर *format,
    ...)
{
    char_u  buf[100];
    बहु_सूची list;
    DWORD   result;
    पूर्णांक	    len;

    जर (silent_mode)
	परत 0;

    बहु_शुरू(list, format);
    len = vim_vsnprintf((अक्षर *)buf, 100, (अक्षर *)format, list);
    बहु_पूर्ण(list);
    WriteConsoleA(g_hConOut, buf, (DWORD)len, &result, रिक्त);
    परत (पूर्णांक)result;
}

    स्थिर शून्य
vtp_sgr_bulk(
    पूर्णांक arg)
{
    पूर्णांक args[1];

    args[0] = arg;
    vtp_sgr_bulks(1, args);
}

# define FAST256(x) \
    जर ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    स्थिती x: \
	FAST256(newargs[x - 1]);

    स्थिर शून्य
vtp_sgr_bulks(
    पूर्णांक argc,
    पूर्णांक *args)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    पूर्णांक	    in, out;
    पूर्णांक	    newargs[16];
    स्थिर पूर्णांक sgrfgr = -1, sgrfgg, sgrfgb;
    स्थिर पूर्णांक sgrbgr = -1, sgrbgg, sgrbgb;

    जर (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	परत;
    }

    in = out = 0;
    पर्यंत (in < argc)
    {
	पूर्णांक s = args[in];
	पूर्णांक copylen = 1;

	जर (s == 38)
	{
	    जर (argc - in >= 5 && args[in + 1] == 2)
	    {
		जर (sgrfgr == args[in + 2] && sgrfgg == args[in + 3]
						     && sgrfgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		अन्यथा
		{
		    sgrfgr = args[in + 2];
		    sgrfgg = args[in + 3];
		    sgrfgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    अन्यथा जर (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	अन्यथा जर (s == 48)
	{
	    जर (argc - in >= 5 && args[in + 1] == 2)
	    {
		जर (sgrbgr == args[in + 2] && sgrbgg == args[in + 3]
						     && sgrbgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		अन्यथा
		{
		    sgrbgr = args[in + 2];
		    sgrbgg = args[in + 3];
		    sgrbgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    अन्यथा जर (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	अन्यथा जर (30 <= s && s <= 39)
	    sgrfgr = -1;
	अन्यथा जर (90 <= s && s <= 97)
	    sgrfgr = -1;
	अन्यथा जर (40 <= s && s <= 49)
	    sgrbgr = -1;
	अन्यथा जर (100 <= s && s <= 107)
	    sgrbgr = -1;
	अन्यथा जर (s == 0)
	    sgrfgr = sgrbgr = -1;

	पर्यंत (copylen--)
	    newargs[out++] = args[in++];
    }

    p = &buf[आकार(buf) - 1];
    *p-- = 'm';

    निवड (out)
    {
	पूर्णांक	n, m;
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
	WriteConsoleA(g_hConOut, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, रिक्त);
    पूर्वनिर्धारित:
	थांबा;
    }
}

    स्थिर शून्य
wt_init(शून्य)
{
    wt_working = mch_getenv("WT_SESSION") != रिक्त;
}

# ifdef FEAT_TERMGUICOLORS
    स्थिर पूर्णांक
ctermtoxterm(
    पूर्णांक cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    परत (((पूर्णांक)r << 16) | ((पूर्णांक)g << 8) | (पूर्णांक)b);
}
# endif

    स्थिर शून्य
set_console_color_rgb(शून्य)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    guicolor_T	fg, bg;
    पूर्णांक		ctermfg, ctermbg;

    जर (!vtp_working)
	परत;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    जर (p_tgc || t_colors >= 256)
    {
	term_fg_rgb_color(fg);
	term_bg_rgb_color(bg);
	परत;
    }

    जर (use_alternate_screen_buffer)
	परत;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cbSize = आकार(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = आकार(csbi);
    csbi.srWindow.उजवा_3 += 1;
    csbi.srWindow.Bottom += 1;
    store_console_bg_rgb = csbi.ColorTable[g_color_index_bg];
    store_console_fg_rgb = csbi.ColorTable[g_color_index_fg];
    csbi.ColorTable[g_color_index_bg] = (COLORREF)bg;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)fg;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

# जर defined(FEAT_TERMGUICOLORS)
    शून्य
get_default_console_color(
    पूर्णांक *cterm_fg,
    पूर्णांक *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    पूर्णांक id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    पूर्णांक ctermfg = 0;
    पूर्णांक ctermbg = 0;
    पूर्णांक dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    जर (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    जर (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	जर (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	जर (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	अन्यथा
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_normal_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    जर (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	जर (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	जर (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	अन्यथा
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
    स्थिर शून्य
reset_console_color_rgb(शून्य)
{
# ifdef FEAT_TERMGUICOLORS
    जर (use_alternate_screen_buffer)
	परत;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = आकार(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = आकार(csbi);
    csbi.srWindow.उजवा_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)store_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)store_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    स्थिर शून्य
restore_console_color_rgb(शून्य)
{
# ifdef FEAT_TERMGUICOLORS
    जर (use_alternate_screen_buffer)
	परत;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = आकार(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = आकार(csbi);
    csbi.srWindow.उजवा_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)save_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)save_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

    शून्य
control_console_color_rgb(शून्य)
{
    जर (vtp_working)
	set_console_color_rgb();
    अन्यथा
	reset_console_color_rgb();
}

    पूर्णांक
use_vtp(शून्य)
{
    परत USE_VTP;
}

    पूर्णांक
is_term_win32(शून्य)
{
    परत T_NAME != रिक्त && STRCMP(T_NAME, "win32") == 0;
}

    पूर्णांक
has_vtp_working(शून्य)
{
    परत vtp_working;
}

#endif

    पूर्णांक
has_conpty_working(शून्य)
{
    परत conpty_working;
}

    पूर्णांक
get_conpty_type(शून्य)
{
    परत conpty_type;
}

    पूर्णांक
is_conpty_stable(शून्य)
{
    परत conpty_stable;
}

    पूर्णांक
get_conpty_fix_type(शून्य)
{
    परत conpty_fix_type;
}

#जर !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    शून्य
resize_console_buf(शून्य)
{
    जर (use_alternate_screen_buffer)
	परत;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    जर (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	परत;

    coord.X = SRWIDTH(csbi.srWindow);
    coord.Y = SRHEIGHT(csbi.srWindow);
    SetConsoleScreenBufferSize(g_hConOut, coord);

    newsize.डावा_3 = 0;
    newsize.शीर्ष_2 = 0;
    newsize.उजवा_3 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    SetConsoleWindowInfo(g_hConOut, TRUE, &newsize);

    SetConsoleScreenBufferSize(g_hConOut, coord);
}
#endif

    अक्षर *
GetWin32Error(शून्य)
{
    स्थिर अक्षर	*oldmsg = रिक्त;
    अक्षर	*acp_संदेश = रिक्त;
    DWORD	acp_len;
    अक्षर	*enc_संदेश = रिक्त;
    पूर्णांक		enc_len = 0;

    // get formatted message from OS
    acp_len = FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
	    रिक्त, GetLastError(), 0, (LPSTR)&acp_संदेश, 0, रिक्त);
    जर (acp_len == 0 || acp_संदेश == रिक्त)
	परत रिक्त;

    // clean oldmsg if remained.
    जर (oldmsg != रिक्त)
    {
	vim_free(oldmsg);
	oldmsg = रिक्त;
    }

    acp_to_enc((char_u *)acp_संदेश, (पूर्णांक)acp_len, (char_u **)&enc_संदेश, &enc_len);
    LocalFree(acp_संदेश);
    जर (enc_संदेश == रिक्त)
	परत रिक्त;

    // remove trailing \r\n
    अक्षर *pcrlf = म_माला(enc_संदेश, "\r\n");
    जर (pcrlf != रिक्त)
	*pcrlf = NUL;

    oldmsg = enc_संदेश;
    परत enc_संदेश;
}

#जर defined(FEAT_RELTIME)
स्थिर HANDLE   timer_handle;
स्थिर पूर्णांक      timer_active = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
स्थिर संक_पूर्ण_प्रकार timeout_flags[2];
स्थिर पूर्णांक	    timeout_flag_idx = 0;
स्थिर संक_पूर्ण_प्रकार *timeout_flag = &timeout_flags[0];


    स्थिर शून्य CALLBACK
set_flag(शून्य *param, BOOLEAN unused2 UNUSED)
{
    *(संक_पूर्ण_प्रकार *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    शून्य
stop_timeout(शून्य)
{
    जर (timer_active)
    {
	BOOL ret = DeleteTimerQueueTimer(रिक्त, timer_handle, रिक्त);
	timer_active = FALSE;
	जर (!ret && GetLastError() != ERROR_IO_PENDING)
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
    अस्थिर संक_पूर्ण_प्रकार *
start_timeout(दीर्घ msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = CreateTimerQueueTimer(
	    &timer_handle, रिक्त, set_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    जर (!ret)
    {
	semsg(_(e_could_not_set_timeout_str), GetWin32Error());
    }
    अन्यथा
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_active = TRUE;
	*timeout_flag = FALSE;
    }
    परत timeout_flag;
}
#endif
