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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <limits.h>

#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <winternl.h>
#ಅನ್ತರ್ಭಾವ <direct.h>

#ಯದಿ !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#ifdef FEAT_JOB_CHANNEL
# include <tlhelp32.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = ರಿಕ್ತ;
#endif

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
ಸ್ಥಿರ HANDLE g_hConIn  = INVALID_HANDLE_ಮೌಲ್ಯ;
ಸ್ಥಿರ HANDLE g_hConOut = INVALID_HANDLE_ಮೌಲ್ಯ;

// Win32 Screen buffer,coordinate,console I/O information
ಸ್ಥಿರ SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
ಸ್ಥಿರ COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
ಸ್ಥಿರ WORD  g_attrDefault = 7;  // lightgray text on black background
ಸ್ಥಿರ WORD  g_attrCurrent;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_fForceExit = FALSE;    // set when forcefully exiting

ಸ್ಥಿರ ಶೂನ್ಯ scroll(unsigned c_ಸಾಲುಗಳು);
ಸ್ಥಿರ ಶೂನ್ಯ set_scroll_region(unsigned ಎಡ, unsigned ತುದಿ,
			      unsigned ಬಲ, unsigned bottom);
ಸ್ಥಿರ ಶೂನ್ಯ set_scroll_region_tb(unsigned ತುದಿ, unsigned bottom);
ಸ್ಥಿರ ಶೂನ್ಯ set_scroll_region_lr(unsigned ಎಡ, unsigned ಬಲ);
ಸ್ಥಿರ ಶೂನ್ಯ ಸೇರಿಸು_ಸಾಲುಗಳು(unsigned c_ಸಾಲುಗಳು);
ಸ್ಥಿರ ಶೂನ್ಯ delete_ಸಾಲುಗಳು(unsigned c_ಸಾಲುಗಳು);
ಸ್ಥಿರ ಶೂನ್ಯ gotoxy(unsigned x, unsigned y);
ಸ್ಥಿರ ಶೂನ್ಯ standout(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_cursor_visible = TRUE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ಒಳಹರಿವು_record_buffer_ಗಂಟು_S
{
    ಒಳಹರಿವು_RECORD ir;
    ಸಂರಚನಾ ಒಳಹರಿವು_record_buffer_ಗಂಟು_S *next;
} ಒಳಹರಿವು_record_buffer_ಗಂಟು_T;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ಒಳಹರಿವು_record_buffer_S
{
    ಒಳಹರಿವು_record_buffer_ಗಂಟು_T *head;
    ಒಳಹರಿವು_record_buffer_ಗಂಟು_T *tail;
    ಪೂರ್ಣಾಙ್ಕ length;
} ಒಳಹರಿವು_record_buffer_T;
ಸ್ಥಿರ ಒಳಹರಿವು_record_buffer_T ಒಳಹರಿವು_record_buffer;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_ಒಳಹರಿವು_record_buffer(ಒಳಹರಿವು_RECORD* irEvents, ಪೂರ್ಣಾಙ್ಕ nMaxLength);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ write_ಒಳಹರಿವು_record_buffer(ಒಳಹರಿವು_RECORD* irEvents, ಪೂರ್ಣಾಙ್ಕ nLength);
#endif
#ifdef FEAT_GUI_MSWIN
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_dont_use_vimrun = TRUE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ need_vimrun_warning = FALSE;
ಸ್ಥಿರ string_T vimrun_path = {(char_u *)"vimrun ", 7};
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vimrun_path_allocated = FALSE;
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ win32_getattrs(char_u *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ win32_setattrs(char_u *name, ಪೂರ್ಣಾಙ್ಕ attrs);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ win32_set_archive(char_u *name);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conpty_working = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conpty_type = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conpty_stable = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conpty_fix_type = 0;
ಸ್ಥಿರ ಶೂನ್ಯ vtp_flag_init(ಶೂನ್ಯ);

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vtp_working = 0;
ಸ್ಥಿರ ಶೂನ್ಯ vtp_init(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ vtp_exit(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ vtp_sgr_bulk(ಪೂರ್ಣಾಙ್ಕ arg);
ಸ್ಥಿರ ಶೂನ್ಯ vtp_sgr_bulks(ಪೂರ್ಣಾಙ್ಕ argc, ಪೂರ್ಣಾಙ್ಕ *argv);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ wt_working = 0;
ಸ್ಥಿರ ಶೂನ್ಯ wt_init(ಶೂನ್ಯ);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_color_index_bg = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_color_index_fg = 7;

# ifdef FEAT_TERMGUICOLORS
ಸ್ಥಿರ guicolor_T save_console_bg_rgb;
ಸ್ಥಿರ guicolor_T save_console_fg_rgb;
ಸ್ಥಿರ guicolor_T store_console_bg_rgb;
ಸ್ಥಿರ guicolor_T store_console_fg_rgb;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ default_console_color_bg = 0x000000; // black
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_term_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# ಅನ್ಯಥಾ
#  define USE_VTP		0
#  define USE_WT		0
# endif

ಸ್ಥಿರ ಶೂನ್ಯ set_console_color_rgb(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ reset_console_color_rgb(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ restore_console_color_rgb(ಶೂನ್ಯ);
#endif  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ suppress_winsize = 1;	// don't fiddle with console
#endif

ಸ್ಥಿರ WCHAR *exe_pathw = ರಿಕ್ತ;

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ಸ್ಥಿರ BOOL use_alternate_screen_buffer = FALSE;
#endif

ಬಾಹ್ಯ DWORD win_version; // this is in os_mswin.c

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ಸ್ಥಿರ BOOL
is_ambiwidth_event(
    ಒಳಹರಿವು_RECORD *ir)
{
    ಪ್ರತ್ಯಾಗಮ ir->EventType == ಕೀಲಿ_EVENT
		&& ir->Event.ಕೀಲಿ_Event_2.b_ಕೀಲಿ_Down
		&& ir->Event.ಕೀಲಿ_Event_2.w_Repeat_ಎಣಿಕೆ == 1
		&& ir->Event.ಕೀಲಿ_Event_2.w_Virtual_ಕೀಲಿ_Code == 0x12
		&& ir->Event.ಕೀಲಿ_Event_2.wVirtualScanCode == 0x38
		&& ir->Event.ಕೀಲಿ_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.ಕೀಲಿ_Event_2.dw_Control_ಕೀಲಿ_State == 2;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
make_ambiwidth_event(
    ಒಳಹರಿವು_RECORD *down,
    ಒಳಹರಿವು_RECORD *up)
{
    down->Event.ಕೀಲಿ_Event_2.w_Virtual_ಕೀಲಿ_Code = 0;
    down->Event.ಕೀಲಿ_Event_2.wVirtualScanCode = 0;
    down->Event.ಕೀಲಿ_Event_2.uChar.UnicodeChar
				    = up->Event.ಕೀಲಿ_Event_2.uChar.UnicodeChar;
    down->Event.ಕೀಲಿ_Event_2.dw_Control_ಕೀಲಿ_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    ಸ್ಥಿರ BOOL
read_console_ಒಳಹರಿವು(
    HANDLE	    h_ಒಳಹರಿವು,
    ಒಳಹರಿವು_RECORD    *lpBuffer,
    ಪೂರ್ಣಾಙ್ಕ		    nLength,
    LPDWORD	    lpEvents)
{
    ಗಣನಾ
    {
	IRSIZE = 10
    };
    ಸ್ಥಿರ ಒಳಹರಿವು_RECORD s_irCache[IRSIZE];
    ಸ್ಥಿರ DWORD s_dwIndex = 0;
    ಸ್ಥಿರ DWORD s_dwMax = 0;
    DWORD dwEvents;
    ಪೂರ್ಣಾಙ್ಕ head;
    ಪೂರ್ಣಾಙ್ಕ tail;
    ಪೂರ್ಣಾಙ್ಕ i;
    ಸ್ಥಿರ ಒಳಹರಿವು_RECORD s_irPseudo;

    ಯದಿ (s_dwMax == 0 && ಒಳಹರಿವು_record_buffer.length > 0)
    {
	dwEvents = read_ಒಳಹರಿವು_record_buffer(s_irCache, IRSIZE);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
    }

    ಯದಿ (nLength == -2)
	ಪ್ರತ್ಯಾಗಮ (s_dwMax > 0) ? TRUE : FALSE;

    ಯದಿ (win_version < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	ಯದಿ (nLength == -1)
	    ಪ್ರತ್ಯಾಗಮ Peek_Console_ಒಳಹರಿವು_W(h_ಒಳಹರಿವು, lpBuffer, 1, lpEvents);
	ಪ್ರತ್ಯಾಗಮ Read_Console_ಒಳಹರಿವು_W(h_ಒಳಹರಿವು, lpBuffer, 1, &dwEvents);
    }

    ಯದಿ (s_dwMax == 0)
    {
	ಯದಿ (!vtp_working && nLength == -1)
	    ಪ್ರತ್ಯಾಗಮ Peek_Console_ಒಳಹರಿವು_W(h_ಒಳಹರಿವು, lpBuffer, 1, lpEvents);
	Get_ಸಂಖ್ಯೆ_Of_Console_ಒಳಹರಿವು_Events(h_ಒಳಹರಿವು, &dwEvents);
	ಯದಿ (dwEvents == 0 && nLength == -1)
	    ಪ್ರತ್ಯಾಗಮ Peek_Console_ಒಳಹರಿವು_W(h_ಒಳಹರಿವು, lpBuffer, 1, lpEvents);
	Read_Console_ಒಳಹರಿವು_W(h_ಒಳಹರಿವು, s_irCache, IRSIZE, &dwEvents);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
	ಯದಿ (dwEvents == 0)
	{
	    *lpEvents = 0;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}

	ಪ್ರತಿ (i = s_dwIndex; i < (ಪೂರ್ಣಾಙ್ಕ)s_dwMax - 1; ++i)
	    ಯದಿ (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	ಯದಿ (s_dwMax > 1)
	{
	    head = 0;
	    tail = s_dwMax - 1;
	    ಯಾವತ್ (head != tail)
	    {
		ಯದಿ (s_irCache[head].EventType == WINDOW_BUFFER_SIZE_EVENT
			&& s_irCache[head + 1].EventType
						  == WINDOW_BUFFER_SIZE_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    ಪ್ರತಿ (i = head; i < tail; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --tail;
		    ಅನುವರ್ತ;
		}
		head++;
	    }
	    s_dwMax = tail + 1;
	}
    }

    ಯದಿ (s_irCache[s_dwIndex].EventType == ಕೀಲಿ_EVENT)
    {
	ಯದಿ (s_irCache[s_dwIndex].Event.ಕೀಲಿ_Event_2.w_Repeat_ಎಣಿಕೆ > 1)
	{
	    s_irPseudo = s_irCache[s_dwIndex];
	    s_irPseudo.Event.ಕೀಲಿ_Event_2.w_Repeat_ಎಣಿಕೆ = 1;
	    s_irCache[s_dwIndex].Event.ಕೀಲಿ_Event_2.w_Repeat_ಎಣಿಕೆ--;
	    *lpBuffer = s_irPseudo;
	    *lpEvents = 1;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }

    *lpBuffer = s_irCache[s_dwIndex];
    ಯದಿ (!(nLength == -1 || nLength == -2) && ++s_dwIndex >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    ಸ್ಥಿರ BOOL
peek_console_ಒಳಹರಿವು(
    HANDLE	    h_ಒಳಹರಿವು,
    ಒಳಹರಿವು_RECORD    *lpBuffer,
    DWORD	    nLength UNUSED,
    LPDWORD	    lpEvents)
{
    ಪ್ರತ್ಯಾಗಮ read_console_ಒಳಹರಿವು(h_ಒಳಹರಿವು, lpBuffer, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    ಸ್ಥಿರ DWORD
ಸಂದೇಶ_wait_for_multiple_objects(
    DWORD    n_ಎಣಿಕೆ,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dwWakeMask)
{
    ಯದಿ (read_console_ಒಳಹರಿವು(ರಿಕ್ತ, ರಿಕ್ತ, -2, ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ WAIT_OBJECT_0;
    ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_Wait_For_Multiple_Objects(n_ಎಣಿಕೆ, pHandles, fWaitAll,
				     dwMilliseconds, dwWakeMask);
}
# endif

# ifndef FEAT_CLIENTSERVER
    ಸ್ಥಿರ DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    ಯದಿ (read_console_ಒಳಹರಿವು(ರಿಕ್ತ, ರಿಕ್ತ, -2, ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ WAIT_OBJECT_0;
    ಪ್ರತ್ಯಾಗಮ WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#endif   // !FEAT_GUI_MSWIN || VIMDLL

    ಶೂನ್ಯ
mch_get_exe_name(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		updated = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	enc_prev = -1;
    WCHAR	*path;
    size_t	pathlen;

    ಯದಿ (exe_name == ರಿಕ್ತ || exe_pathw == ರಿಕ್ತ || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_PATH];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(ರಿಕ್ತ, buf, MAX_PATH);
	ಯದಿ (p > buf)
	{
	    ಯದಿ (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_to_enc(buf, ರಿಕ್ತ);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    ಕುರು
	    {
		--p;
		ಯದಿ (*p == L'\\')
		{
		    *p = L'\0';
		    ಭಙ್ಗ;
		}
	    } ಯಾವತ್ (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    ಯದಿ (exe_pathw == ರಿಕ್ತ || !updated)
	ಪ್ರತ್ಯಾಗಮ;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    path = _wgetenv(L"PATH");
    pathlen = (path == ರಿಕ್ತ) ? 0 : wcslen(path);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#define EXTRA_LEN 7
#define MAX_ENV_STRING_LEN 8191

    ಯದಿ (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   temp[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	ಯದಿ (pathlen == 0)
	    wcscpy(temp + templen, exe_pathw);
	ಅನ್ಯಥಾ
	{
	    wcscpy(temp + templen, path);

	    // Check if exe_path is already included in $PATH.
	    ಯದಿ (wcsstr(temp, exe_pathw) == ರಿಕ್ತ)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		ಯದಿ (temp[templen - 1] != L';')
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
    ಸ್ಥಿರ ಶೂನ್ಯ
unescape_shellxquote(char_u *p, char_u *escaped)
{
    ಪೂರ್ಣಾಙ್ಕ	    l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
    ಪೂರ್ಣಾಙ್ಕ	    n;

    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (*p == '^' && vim_strchr(escaped, p[1]) != ರಿಕ್ತ)
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
vimLoadLib(ಧ್ರುವ ವರ್ಣ *name)
{
    HINSTANCE	dll = ರಿಕ್ತ;

    // No need to load any library when registering OLE.
    ಯದಿ (found_register_arg)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    ಯದಿ (exe_pathw == ರಿಕ್ತ)
    {
	mch_get_exe_name();
	ಯದಿ (exe_pathw == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    WCHAR old_dirw[MAXPATHL];

    ಯದಿ (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    SetCurrentDirectoryW(exe_pathw);
    dll = LoadLibrary(name);
    SetCurrentDirectoryW(old_dirw);
    ಪ್ರತ್ಯಾಗಮ dll;
}

#ಯದಿ defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_is_gui_executable(ಶೂನ್ಯ)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(ರಿಕ್ತ);
    PIMAGE_DOS_HEADER	pDOS = (PIMAGE_DOS_HEADER)pImage;
    PIMAGE_NT_HEADERS	pPE;

    ಯದಿ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    ಯದಿ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (pPE->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

#ಯದಿ defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ *
get_imported_func_info(HINSTANCE hInst, ಧ್ರುವ ವರ್ಣ *funcname, ಪೂರ್ಣಾಙ್ಕ info,
	ಧ್ರುವ ಶೂನ್ಯ *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_HEADER		pDOS = (PIMAGE_DOS_HEADER)hInst;
    PIMAGE_NT_HEADERS		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_DATA		pIAT;	    // Import Address Table
    PIMAGE_THUNK_DATA		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    ಯದಿ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    ಯದಿ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ImpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    ಯದಿ (ImpVA == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    ಪ್ರತಿ (; pImpDesc->FirstThunk; ++pImpDesc)
    {
	ಯದಿ (!pImpDesc->OriginalFirstThunk)
	    ಅನುವರ್ತ;
	pIAT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->FirstThunk);
	pINT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->OriginalFirstThunk);
	ಪ್ರತಿ (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    ಯದಿ (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		ಅನುವರ್ತ;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.AddressOfData));
	    ಯದಿ (strcmp((ವರ್ಣ *)pImpName->Name, funcname) == 0)
	    {
		ಶೂನ್ಯ *original;
		DWORD old, new = PAGE_READWRITE;

		ವಿಕಲ್ಪ (info)
		{
		    ಪಕ್ಷ 0:
			ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)pIAT->u1.Function;
		    ಪಕ್ಷ 1:
			ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)(pImage + pImpDesc->Name);
		    ಪಕ್ಷ 2:
			original = (ಶೂನ್ಯ *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, ಪರಿಮಾಣ(ಶೂನ್ಯ *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, ಪರಿಮಾಣ(ಶೂನ್ಯ *),
				old, &new);
			ಪ್ರತ್ಯಾಗಮ original;
		    ಸ್ವಭಾವ:
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
find_imported_module_by_funcname(HINSTANCE hInst, ಧ್ರುವ ವರ್ಣ *funcname)
{
    ವರ್ಣ    *modulename;

    modulename = (ವರ್ಣ *)get_imported_func_info(hInst, funcname, 1, ರಿಕ್ತ);
    ಯದಿ (modulename != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ GetModuleHandleA(modulename);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    ಶೂನ್ಯ *
get_dll_import_func(HINSTANCE hInst, ಧ್ರುವ ವರ್ಣ *funcname)
{
    ಪ್ರತ್ಯಾಗಮ get_imported_func_info(hInst, funcname, 0, ರಿಕ್ತ);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    ಶೂನ್ಯ *
hook_dll_import_func(HINSTANCE hInst, ಧ್ರುವ ವರ್ಣ *funcname, ಧ್ರುವ ಶೂನ್ಯ *hook)
{
    ಪ್ರತ್ಯಾಗಮ get_imported_func_info(hInst, funcname, 2, hook);
}
#endif

#ಯದಿ defined(FEAT_PYTHON3)
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

    ಯದಿ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    ಯದಿ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ExpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    ExpSize = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    ಯದಿ (ExpVA == 0)
	ಪ್ರತ್ಯಾಗಮ hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    pFunctionTable = (LPDWORD)(pImage + pExpDir->AddressOfFunctions);

    ಯದಿ (pExpDir->ಸಂಖ್ಯೆ_Of_Names == 0)
	ಪ್ರತ್ಯಾಗಮ hInst;	// No export names.

    // Check only the first entry.
    ಯದಿ ((pFunctionTable[0] < ExpVA) || (pFunctionTable[0] >= ExpVA + ExpSize))
	// The first entry is not a function forwarder.
	ಪ್ರತ್ಯಾಗಮ hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    ಧ್ರುವ ವರ್ಣ *name = (ಧ್ರುವ ವರ್ಣ *)(pImage + pFunctionTable[0]);
    ಧ್ರುವ ವರ್ಣ *p = strchr(name, '.');
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ hInst;

    // Extract DllName.
    ವರ್ಣ buf[MAX_PATH];
    ಯದಿ (p - name + 1 > ಪರಿಮಾಣ(buf))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    ಪ್ರತ್ಯಾಗಮ GetModuleHandleA(buf);
}
#endif

#ಯದಿ defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
ಸ್ಥಿರ ವರ್ಣ *null_libintl_gettext(ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ವರ್ಣ *null_libintl_ngettext(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *, unsigned long n);
ಸ್ಥಿರ ವರ್ಣ *null_libintl_textdomain(ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ವರ್ಣ *null_libintl_bindtextdomain(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ವರ್ಣ *null_libintl_bind_textdomain_codeset(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ null_libintl_wputenv(ಧ್ರುವ wchar_t *);

ಸ್ಥಿರ HINSTANCE hLibintlDLL = ರಿಕ್ತ;
ವರ್ಣ *(*dyn_libintl_gettext)(ಧ್ರುವ ವರ್ಣ *) = null_libintl_gettext;
ವರ್ಣ *(*dyn_libintl_ngettext)(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *, unsigned long n)
						= null_libintl_ngettext;
ವರ್ಣ *(*dyn_libintl_textdomain)(ಧ್ರುವ ವರ್ಣ *) = null_libintl_textdomain;
ವರ್ಣ *(*dyn_libintl_bindtextdomain)(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *)
						= null_libintl_bindtextdomain;
ವರ್ಣ *(*dyn_libintl_bind_textdomain_codeset)(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *)
				       = null_libintl_bind_textdomain_codeset;
ಪೂರ್ಣಾಙ್ಕ (*dyn_libintl_wputenv)(ಧ್ರುವ wchar_t *) = null_libintl_wputenv;
ಪೂರ್ಣಾಙ್ಕ *dyn_libintl_nl_ಸಂದೇಶ_cat_cntr = ರಿಕ್ತ;

    ಪೂರ್ಣಾಙ್ಕ
dyn_libintl_init(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಸ್ಥಿರ ಸಂರಚನಾ
    {
	ವರ್ಣ	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{ರಿಕ್ತ, ರಿಕ್ತ}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    ಯದಿ (hLibintlDLL != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vimLoadLib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    ಯದಿ (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    ಯದಿ (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT2);
# endif
    ಯದಿ (!hLibintlDLL)
    {
	ಯದಿ (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_load_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತಿ (i = 0; libintl_entry[i].name != ರಿಕ್ತ
					 && libintl_entry[i].ptr != ರಿಕ್ತ; ++i)
    {
	ಯದಿ ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == ರಿಕ್ತ)
	{
	    dyn_libintl_end();
	    ಯದಿ (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_load_library_function_str), libintl_entry[i].name);
		verbose_leave();
	    }
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (ವರ್ಣ *(*)(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *))
			GetProcAddress(hLibintlDLL, "bind_textdomain_codeset");
    ಯದಿ (dyn_libintl_bind_textdomain_codeset == ರಿಕ್ತ)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = find_imported_module_by_funcname(hLibintlDLL, "getenv");
    ಯದಿ (hmsvcrt != ರಿಕ್ತ)
	dyn_libintl_wputenv = (ಪೂರ್ಣಾಙ್ಕ (*)(ಧ್ರುವ wchar_t *))
					GetProcAddress(hmsvcrt, "_wputenv");
    ಯದಿ (dyn_libintl_wputenv == ರಿಕ್ತ || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_ಸಂದೇಶ_cat_cntr = (ಪೂರ್ಣಾಙ್ಕ *)GetProcAddress(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಶೂನ್ಯ
dyn_libintl_end(ಶೂನ್ಯ)
{
    ಯದಿ (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ರಿಕ್ತ;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_ಸಂದೇಶ_cat_cntr	= ರಿಕ್ತ;
}

    ಸ್ಥಿರ ವರ್ಣ *
null_libintl_gettext(ಧ್ರುವ ವರ್ಣ *msgid)
{
    ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*)msgid;
}

    ಸ್ಥಿರ ವರ್ಣ *
null_libintl_ngettext(
	ಧ್ರುವ ವರ್ಣ *msgid,
	ಧ್ರುವ ವರ್ಣ *msgid_plural,
	unsigned long n)
{
    ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)(n == 1 ? msgid : msgid_plural);
}

    ಸ್ಥಿರ ವರ್ಣ *
null_libintl_bindtextdomain(
	ಧ್ರುವ ವರ್ಣ *domainname UNUSED,
	ಧ್ರುವ ವರ್ಣ *dirname UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ವರ್ಣ *
null_libintl_bind_textdomain_codeset(
	ಧ್ರುವ ವರ್ಣ *domainname UNUSED,
	ಧ್ರುವ ವರ್ಣ *codeset UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ವರ್ಣ *
null_libintl_textdomain(ಧ್ರುವ ವರ್ಣ *domainname UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
null_libintl_wputenv(ಧ್ರುವ wchar_t *envstring UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ 0;
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
    ಸ್ಥಿರ BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    ಯದಿ (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (!Lookup_Privilege_ಮೌಲ್ಯ(ರಿಕ್ತ, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    tokenPrivileges.Privilege_ಎಣಿಕೆ	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    ಯದಿ (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, ರಿಕ್ತ, ರಿಕ್ತ))
    {
	CloseHandle(hToken);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಯದಿ (GetLastError() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    CloseHandle(hToken);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}
#endif

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    ಶೂನ್ಯ
PlatformId(ಶೂನ್ಯ)
{
#ifdef FEAT_EVAL
    DWORD major = (win_version >> 24) & 0xFF;
    DWORD minor = (win_version >> 16) & 0xFF;
    vim_snprintf(windowsVersion, ಪರಿಮಾಣ(windowsVersion), "%d.%d", major, minor);
#endif

#ifdef HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    ಯದಿ (!win32_enable_privilege(SE_SECURITY_NAME))
	ಪ್ರತ್ಯಾಗಮ;
#endif
}

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SHIFT  (SHIFT_PRESSED)
# define CTRL   (ಬಲ_CTRL_PRESSED | ಎಡ_CTRL_PRESSED)
# define ALT    (ಬಲ_ALT_PRESSED  | ಎಡ_ALT_PRESSED)
# define ALT_GR (ಬಲ_ALT_PRESSED  | ಎಡ_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ
{
    WORD    w_Virt_ಕೀಲಿ;
    BOOL    f_Ansi_ಕೀಲಿ;
    ಪೂರ್ಣಾಙ್ಕ	    chAlone;
    ಪೂರ್ಣಾಙ್ಕ	    chShift;
    ಪೂರ್ಣಾಙ್ಕ	    chCtrl;
    ಪೂರ್ಣಾಙ್ಕ	    chAlt;
} Virt_ಕೀಲಿ_Map[] =
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
    { VK_ಎಡ,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_ಬಲ,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_END,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_DOWN,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_NEXT,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_ಸೇರಿಸು,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_BACK,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# ಯದಿ 0
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
win32_kbd_patch_ಕೀಲಿ(
    ಕೀಲಿ_EVENT_RECORD *pker)
{
    UINT uMods = pker->dw_Control_ಕೀಲಿ_State;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_iIsDead = 0;
    ಸ್ಥಿರ WORD awAnsiCode[2];
    ಸ್ಥಿರ BYTE abKeystate[256];

    ಯದಿ (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[1];
	s_iIsDead = 0;
	ಪ್ರತ್ಯಾಗಮ 1;
    }

    // check if it already has a valid unicode character.
    ಯದಿ (pker->uChar.UnicodeChar != 0)
	ಪ್ರತ್ಯಾಗಮ 1;

    CLEAR_FIELD(abKeystate);

    // Clear any pending dead keys
    ToUnicode(VK_SPACE, Map_Virtual_ಕೀಲಿ(VK_SPACE, 0), abKeystate, awAnsiCode, 2, 0);

    ಯದಿ (uMods & SHIFT_PRESSED)
	abKeystate[VK_SHIFT] = 0x80;
    ಯದಿ (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    ಯದಿ ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ToUnicode(pker->w_Virtual_ಕೀಲಿ_Code, pker->wVirtualScanCode,
			abKeystate, awAnsiCode, 2, 0);

    ಯದಿ (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[0];

    ಪ್ರತ್ಯಾಗಮ s_iIsDead;
}

ಸ್ಥಿರ BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    ಸ್ಥಿರ BOOL
decode_ಕೀಲಿ_event(
    ಕೀಲಿ_EVENT_RECORD	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    ಪೂರ್ಣಾಙ್ಕ			*pmodifiers,
    BOOL		fDoPost UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ nModifs = pker->dw_Control_ಕೀಲಿ_State & (SHIFT | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    ಯದಿ (!pker->b_ಕೀಲಿ_Down)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // ignore some keystrokes
    ವಿಕಲ್ಪ (pker->w_Virtual_ಕೀಲಿ_Code)
    {
    // modifiers
    ಪಕ್ಷ VK_SHIFT:
    ಪಕ್ಷ VK_CONTROL:
    ಪಕ್ಷ VK_MENU:   // Alt key
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಸ್ವಭಾವ:
	ಭಙ್ಗ;
    }

    // Shift-TAB
    ಯದಿ (pker->w_Virtual_ಕೀಲಿ_Code == VK_TAB && (nModifs & SHIFT_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಪ್ರತಿ (i = ARRAY_LENGTH(Virt_ಕೀಲಿ_Map); --i >= 0; )
    {
	ಯದಿ (Virt_ಕೀಲಿ_Map[i].w_Virt_ಕೀಲಿ == pker->w_Virtual_ಕೀಲಿ_Code)
	{
	    *pch = Virt_ಕೀಲಿ_Map[i].chAlone;
	    ಯದಿ ((nModifs & SHIFT) != 0)
		*pch = Virt_ಕೀಲಿ_Map[i].chShift;
	    ಅನ್ಯಥಾ ಯದಿ ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_ಕೀಲಿ_Map[i].chCtrl;
	    ಅನ್ಯಥಾ ಯದಿ ((nModifs & ALT) != 0)
		*pch = Virt_ಕೀಲಿ_Map[i].chAlt;

	    ಯದಿ (*pch != 0)
	    {
		ಯದಿ (Virt_ಕೀಲಿ_Map[i].f_Ansi_ಕೀಲಿ)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    ಯದಿ (pmodifiers)
		    {
			ಯದಿ (pker->w_Virtual_ಕೀಲಿ_Code >= VK_F1
			    && pker->w_Virtual_ಕೀಲಿ_Code <= VK_F12)
			{
			    ಯದಿ ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_MASK_ALT;
				ಯದಿ ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
			    }
			    ಯದಿ ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				ಯದಿ ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
			    }
			}
			ಅನ್ಯಥಾ ಯದಿ (pker->w_Virtual_ಕೀಲಿ_Code >= VK_END
				&& pker->w_Virtual_ಕೀಲಿ_Code <= VK_DOWN)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
			    ಯದಿ ((nModifs & SHIFT) != 0
						    && (nModifs & ~SHIFT) == 0)
			    {
				*pch2 = Virt_ಕೀಲಿ_Map[i].chShift;
			    }
			    ಯದಿ ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_ಕೀಲಿ_Map[i].chCtrl;
				ಯದಿ (pker->w_Virtual_ಕೀಲಿ_Code == VK_UP
				    || pker->w_Virtual_ಕೀಲಿ_Code == VK_DOWN)
				{
				    *pmodifiers |= MOD_MASK_CTRL;
				    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
				}
			    }
			    ಯದಿ ((nModifs & SHIFT) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				*pch2 = Virt_ಕೀಲಿ_Map[i].chShift;
			    }
			    ಯದಿ ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_ಕೀಲಿ_Map[i].chAlt;
				*pmodifiers |= MOD_MASK_ALT;
				ಯದಿ ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
				}
				ಅನ್ಯಥಾ ಯದಿ ((nModifs & SHIFT) != 0)
				{
				    *pch2 = Virt_ಕೀಲಿ_Map[i].chShift;
				}
				ಅನ್ಯಥಾ ಯದಿ ((nModifs & CTRL) != 0)
				{
				    ಯದಿ (pker->w_Virtual_ಕೀಲಿ_Code == VK_UP
					|| pker->w_Virtual_ಕೀಲಿ_Code == VK_DOWN)
				    {
					*pmodifiers |= MOD_MASK_CTRL;
					*pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
				    }
				    ಅನ್ಯಥಾ
				    {
					*pch2 = Virt_ಕೀಲಿ_Map[i].chCtrl;
				    }
				}
			    }
			}
			ಅನ್ಯಥಾ ಯದಿ (pker->w_Virtual_ಕೀಲಿ_Code == VK_ಸೇರಿಸು
					&& (nModifs & SHIFT) != 0
					&& (nModifs & ~SHIFT) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_ಕೀಲಿ_Map[i].chShift;
			}
			ಅನ್ಯಥಾ
			{
			    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
			    ಯದಿ ((nModifs & SHIFT) != 0)
				*pmodifiers |= MOD_MASK_SHIFT;
			    ಯದಿ ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_MASK_CTRL;
			    ಯದಿ ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_MASK_ALT;
			}
		    }
		}

		ಪ್ರತ್ಯಾಗಮ TRUE;
	    }
	}
    }

    i = win32_kbd_patch_ಕೀಲಿ(pker);

    ಯದಿ (i < 0)
	*pch = NUL;
    ಅನ್ಯಥಾ
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	ಯದಿ (pmodifiers != ರಿಕ್ತ)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    ಯದಿ ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_MASK_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    ಯದಿ ((nModifs & SHIFT) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_MASK_SHIFT;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    ಯದಿ ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_MASK_CTRL;
	}
    }

    ಪ್ರತ್ಯಾಗಮ (*pch != NUL);
}

# ಯದಿ defined(FEAT_EVAL)
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
encode_ಕೀಲಿ_event(dict_T *args, ಒಳಹರಿವು_RECORD *ir)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_dwMods = 0;

    char_u *action = dict_get_string(args, "event", TRUE);
    ಯದಿ (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_ಕೀಲಿ_Down = STRICMP(action, "keydown") == 0;
	WORD vkCode = dict_get_ಸಂಖ್ಯೆ_def(args, "keycode", 0);
	ಯದಿ (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	ir->EventType = ಕೀಲಿ_EVENT;
	ಕೀಲಿ_EVENT_RECORD ker;
	ZeroMemory(&ker, ಪರಿಮಾಣ(ker));
	ker.b_ಕೀಲಿ_Down = is_ಕೀಲಿ_Down;
	ker.w_Repeat_ಎಣಿಕೆ = 1;
	ker.wVirtualScanCode = 0;
	ker.dw_Control_ಕೀಲಿ_State = 0;
	ಪೂರ್ಣಾಙ್ಕ mods = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	ಯದಿ (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    ಯದಿ (mods & MOD_MASK_SHIFT)
		ker.dw_Control_ಕೀಲಿ_State |= SHIFT_PRESSED;
	    ಯದಿ (mods & MOD_MASK_CTRL)
		ker.dw_Control_ಕೀಲಿ_State |= ಎಡ_CTRL_PRESSED;
	    ಯದಿ (mods & MOD_MASK_ALT)
		ker.dw_Control_ಕೀಲಿ_State |= ಎಡ_ALT_PRESSED;
	}

	ಯದಿ (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT)
	{
	    ಯದಿ (is_ಕೀಲಿ_Down)
		s_dwMods |= SHIFT_PRESSED;
	    ಅನ್ಯಥಾ
		s_dwMods &= ~SHIFT_PRESSED;
	}
	ಅನ್ಯಥಾ ಯದಿ (vkCode == VK_LCONTROL || vkCode == VK_CONTROL)
	{
	    ಯದಿ (is_ಕೀಲಿ_Down)
		s_dwMods |= ಎಡ_CTRL_PRESSED;
	    ಅನ್ಯಥಾ
		s_dwMods &= ~ಎಡ_CTRL_PRESSED;
	}
	ಅನ್ಯಥಾ ಯದಿ (vkCode == VK_RCONTROL)
	{
	    ಯದಿ (is_ಕೀಲಿ_Down)
		s_dwMods |= ಬಲ_CTRL_PRESSED;
	    ಅನ್ಯಥಾ
		s_dwMods &= ~ಬಲ_CTRL_PRESSED;
	}
	ಅನ್ಯಥಾ ಯದಿ (vkCode == VK_LMENU || vkCode == VK_MENU)
	{
	    ಯದಿ (is_ಕೀಲಿ_Down)
		s_dwMods |= ಎಡ_ALT_PRESSED;
	    ಅನ್ಯಥಾ
		s_dwMods &= ~ಎಡ_ALT_PRESSED;
	}
	ಅನ್ಯಥಾ ಯದಿ (vkCode == VK_RMENU)
	{
	    ಯದಿ (is_ಕೀಲಿ_Down)
		s_dwMods |= ಬಲ_ALT_PRESSED;
	    ಅನ್ಯಥಾ
		s_dwMods &= ~ಬಲ_ALT_PRESSED;
	}
	ker.dw_Control_ಕೀಲಿ_State |= s_dwMods;
	ker.w_Virtual_ಕೀಲಿ_Code = vkCode;
	ker.uChar.UnicodeChar = 0;
	ir->Event.ಕೀಲಿ_Event_2 = ker;
	vim_free(action);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (action == ರಿಕ್ತ)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	ಅನ್ಯಥಾ
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "event", action);
	    vim_free(action);
	}
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}
# endif  // FEAT_EVAL
#endif // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_fMouseAvail = FALSE;   // mouse present
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_fMouseActive = FALSE;  // mouse enabled
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_nMouseClick = -1;	    // mouse status
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_xMouse;		    // mouse x coordinate
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_yMouse;		    // mouse y coordinate
ಸ್ಥಿರ DWORD g_cmodein = 0;	    // Original console input mode
ಸ್ಥಿರ DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    ಶೂನ್ಯ
mch_setmouse(ಪೂರ್ಣಾಙ್ಕ on)
{
    DWORD cmodein;

# ifdef VIMDLL
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;
# endif
    ಯದಿ (!g_fMouseAvail)
	ಪ್ರತ್ಯಾಗಮ;

    g_fMouseActive = on;
    GetConsoleMode(g_hConIn, &cmodein);

    ಯದಿ (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_ಒಳಹರಿವು;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    ಅನ್ಯಥಾ
    {
	cmodein &= ~ENABLE_MOUSE_ಒಳಹರಿವು;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }

    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
}


# ಯದಿ defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    ಶೂನ್ಯ
mch_bevalterm_changed(ಶೂನ್ಯ)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
decode_mouse_wheel(MOUSE_EVENT_RECORD *pmer)
{
    ಪೂರ್ಣಾಙ್ಕ	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    ಪೂರ್ಣಾಙ್ಕ	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    ಪೂರ್ಣಾಙ್ಕ lcol = g_xMouse;
    ಪೂರ್ಣಾಙ್ಕ lrow = g_yMouse;
    win_T *wp = mouse_find_win(&lrow, &lcol, FIND_POPUP);
    ಯದಿ (wp != ರಿಕ್ತ && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_FIELD(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	ಯದಿ (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ಎಡ : MSCR_ಬಲ;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	ಅನ್ಯಥಾ
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
    ಯದಿ (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    ಅನ್ಯಥಾ
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    ಯದಿ (pmer->dw_Control_ಕೀಲಿ_State & SHIFT_PRESSED)
	modifiers |= MOD_MASK_SHIFT; // MOUSE_SHIFT;
    ಯದಿ (pmer->dw_Control_ಕೀಲಿ_State & (ಬಲ_CTRL_PRESSED | ಎಡ_CTRL_PRESSED))
	modifiers |= MOD_MASK_CTRL; // MOUSE_CTRL;
    ಯದಿ (pmer->dw_Control_ಕೀಲಿ_State & (ಬಲ_ALT_PRESSED  | ಎಡ_ALT_PRESSED))
	modifiers |= MOD_MASK_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    ಪ್ರತ್ಯಾಗಮ;
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
    ಸ್ಥಿರ BOOL
decode_mouse_event(
    MOUSE_EVENT_RECORD *pmer)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_nOldButton = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_nOldMouseClick = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_xOldMouse = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_yOldMouse = -1;
    ಸ್ಥಿರ linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_old_topfill = 0;
# endif
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_cClicks = 1;
    ಸ್ಥಿರ BOOL s_fReleased = TRUE;
    ಸ್ಥಿರ DWORD s_dwLastClickTime = 0;
    ಸ್ಥಿರ BOOL s_f_Next_Is_ಮಧ್ಯ = FALSE;

    ಸ್ಥಿರ DWORD cButtons = 0;	// number of buttons supported

    ಧ್ರುವ DWORD ಎಡ_2 = FROM_ಎಡ_1_ST_BUTTON_PRESSED;
    ಧ್ರುವ DWORD ಮಧ್ಯ = FROM_ಎಡ_2_ND_BUTTON_PRESSED;
    ಧ್ರುವ DWORD ಬಲ_2 = RIGHTMOST_BUTTON_PRESSED;
    ಧ್ರುವ DWORD ಎಡ_ಬಲ = ಎಡ_2 | ಬಲ_2;

    ಪೂರ್ಣಾಙ್ಕ nButton;

    ಯದಿ (cButtons == 0 && !Get_ಸಂಖ್ಯೆ_Of_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    ಯದಿ (!g_fMouseAvail || !g_fMouseActive)
    {
	g_nMouseClick = -1;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    ಯದಿ (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    ಯದಿ (g_nMouseClick != -1)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಯದಿ (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	ಪ್ರತ್ಯಾಗಮ TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    ಯದಿ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	ಯದಿ (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // If no buttons are pressed...
    ಯದಿ ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	ಯದಿ (s_fReleased)
	{
# ifdef FEAT_BEVAL_TERM
	    // do return mouse move events when we want them
	    ಯದಿ (p_bevalterm)
		nButton = MOUSE_DRAG;
	    ಅನ್ಯಥಾ
# endif
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	s_fReleased = TRUE;
    }
    ಅನ್ಯಥಾ    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	ಯದಿ (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & ಎಡ_ಬಲ);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    ಯದಿ (dwLR == ಎಡ_2 || dwLR == ಬಲ_2)
	    {
		ಪ್ರತಿ (;;)
		{
		    // wait a short time for next input event
		    ಯದಿ (WaitForSingleObject(g_hConIn, p_mouset / 3)
							     != WAIT_OBJECT_0)
			ಭಙ್ಗ;
		    ಅನ್ಯಥಾ
		    {
			DWORD cRecords = 0;
			ಒಳಹರಿವು_RECORD ir;
			MOUSE_EVENT_RECORD* pmer2 = &ir.Event.MouseEvent;

			peek_console_ಒಳಹರಿವು(g_hConIn, &ir, 1, &cRecords);

			ಯದಿ (cRecords == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & ಎಡ_ಬಲ))
			    ಭಙ್ಗ;
			ಅನ್ಯಥಾ
			{
			    ಯದಿ (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_ಒಳಹರಿವು(g_hConIn, &ir, 1, &cRecords);

				ಪ್ರತ್ಯಾಗಮ decode_mouse_event(pmer2);
			    }
			    ಅನ್ಯಥಾ ಯದಿ (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_ಒಳಹರಿವು(g_hConIn, &ir, 1, &cRecords);

				// are there any more mouse events in queue?
				peek_console_ಒಳಹರಿವು(g_hConIn, &ir, 1, &cRecords);

				ಯದಿ (cRecords==0 || ir.EventType != MOUSE_EVENT)
				    ಭಙ್ಗ;
			    }
			    ಅನ್ಯಥಾ
				ಭಙ್ಗ;
			}
		    }
		}
	    }
	}

	ಯದಿ (s_f_Next_Is_ಮಧ್ಯ)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_ಮಧ್ಯ;
	    s_f_Next_Is_ಮಧ್ಯ = FALSE;
	}
	ಅನ್ಯಥಾ ಯದಿ (cButtons == 2	&&
	    ((pmer->dwButtonState & ಎಡ_ಬಲ) == ಎಡ_ಬಲ))
	{
	    nButton = MOUSE_ಮಧ್ಯ;

	    ಯದಿ (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_Next_Is_ಮಧ್ಯ = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ ((pmer->dwButtonState & ಎಡ_2) == ಎಡ_2)
	    nButton = MOUSE_ಎಡ;
	ಅನ್ಯಥಾ ಯದಿ ((pmer->dwButtonState & ಮಧ್ಯ) == ಮಧ್ಯ)
	    nButton = MOUSE_ಮಧ್ಯ;
	ಅನ್ಯಥಾ ಯದಿ ((pmer->dwButtonState & ಬಲ_2) == ಬಲ_2)
	    nButton = MOUSE_ಬಲ;

	ಯದಿ (! s_fReleased && ! s_f_Next_Is_ಮಧ್ಯ
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	s_fReleased = s_f_Next_Is_ಮಧ್ಯ;
    }

    ಯದಿ (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	ಯದಿ (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dwCurrentTime = Get_Tick_ಎಣಿಕೆ();

	    ಯದಿ (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (ಪೂರ್ಣಾಙ್ಕ)(dwCurrentTime - s_dwLastClickTime) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dwLastClickTime = dwCurrentTime;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	ಯದಿ (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    ಯದಿ (nButton == -1)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    ಯದಿ (pmer->dw_Control_ಕೀಲಿ_State & SHIFT_PRESSED)
	g_nMouseClick |= MOUSE_SHIFT;
    ಯದಿ (pmer->dw_Control_ಕೀಲಿ_State & (ಬಲ_CTRL_PRESSED | ಎಡ_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    ಯದಿ (pmer->dw_Control_ಕೀಲಿ_State & (ಬಲ_ALT_PRESSED  | ಎಡ_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    ಯದಿ (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	SET_ಅಂಕೆ_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    ಯದಿ (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

# ifdef FEAT_EVAL
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
encode_mouse_event(dict_T *args, ಒಳಹರಿವು_RECORD *ir)
{
    ಪೂರ್ಣಾಙ್ಕ		button;
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		repeated_click;
    int_u	mods = 0;
    ಪೂರ್ಣಾಙ್ಕ		move;

    ಯದಿ (!dict_has_ಕೀಲಿ(args, "row") || !dict_has_ಕೀಲಿ(args, "col"))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (ಪೂರ್ಣಾಙ್ಕ)dict_get_bool(args, "move", FALSE);
    ಯದಿ (!move && (!dict_has_ಕೀಲಿ(args, "button")
	    || !dict_has_ಕೀಲಿ(args, "multiclick")
	    || !dict_has_ಕೀಲಿ(args, "modifiers")))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    row = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "row") - 1;
    col = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_RECORD mer;
    ZeroMemory(&mer, ಪರಿಮಾಣ(mer));
    mer.dwMousePosition.X  = col;
    mer.dwMousePosition.Y  = row;

    ಯದಿ (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    ಅನ್ಯಥಾ
    {
	button = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "button");
	repeated_click = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "multiclick");
	mods = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	ವಿಕಲ್ಪ (button)
	{
	    ಪಕ್ಷ MOUSE_ಎಡ:
		mer.dwButtonState = FROM_ಎಡ_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ಭಙ್ಗ;
	    ಪಕ್ಷ MOUSE_ಮಧ್ಯ:
		mer.dwButtonState = FROM_ಎಡ_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ಭಙ್ಗ;
	    ಪಕ್ಷ MOUSE_ಬಲ:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ಭಙ್ಗ;
	    ಪಕ್ಷ MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    ಪಕ್ಷ MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		ಭಙ್ಗ;
	    ಪಕ್ಷ MOUSE_X1:
		mer.dwButtonState = FROM_ಎಡ_3_RD_BUTTON_PRESSED;
		ಭಙ್ಗ;
	    ಪಕ್ಷ MOUSE_X2:
		mer.dwButtonState = FROM_ಎಡ_4_TH_BUTTON_PRESSED;
		ಭಙ್ಗ;
	    ಪಕ್ಷ MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ಭಙ್ಗ;
	    ಪಕ್ಷ MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ಭಙ್ಗ;
	    ಪಕ್ಷ MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ಭಙ್ಗ;
	    ಪಕ್ಷ MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		semsg(_(e_invalid_argument_str), "button");
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }

    mer.dw_Control_ಕೀಲಿ_State = 0;
    ಯದಿ (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	ಯದಿ (mods & MOUSE_SHIFT)
	    mer.dw_Control_ಕೀಲಿ_State |= SHIFT_PRESSED;
	ಯದಿ (mods & MOUSE_CTRL)
	    mer.dw_Control_ಕೀಲಿ_State |= ಎಡ_CTRL_PRESSED;
	ಯದಿ (mods & MOUSE_ALT)
	    mer.dw_Control_ಕೀಲಿ_State |= ಎಡ_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}
# endif  // FEAT_EVAL

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
write_ಒಳಹರಿವು_record_buffer(ಒಳಹರಿವು_RECORD* irEvents, ಪೂರ್ಣಾಙ್ಕ nLength)
{
    ಪೂರ್ಣಾಙ್ಕ				n_ಎಣಿಕೆ = 0;
    ಒಳಹರಿವು_record_buffer_ಗಂಟು_T	*event_ಗಂಟು;

    ಯಾವತ್ (n_ಎಣಿಕೆ < nLength)
    {
	event_ಗಂಟು = alloc(ಪರಿಮಾಣ(ಒಳಹರಿವು_record_buffer_ಗಂಟು_T));
	ಯದಿ (event_ಗಂಟು == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;

	event_ಗಂಟು->ir = irEvents[n_ಎಣಿಕೆ++];
	event_ಗಂಟು->next = ರಿಕ್ತ;

	ಒಳಹರಿವು_record_buffer.length++;
	ಯದಿ (ಒಳಹರಿವು_record_buffer.tail == ರಿಕ್ತ)
	{
	    ಒಳಹರಿವು_record_buffer.head = event_ಗಂಟು;
	    ಒಳಹರಿವು_record_buffer.tail = event_ಗಂಟು;
	}
	ಅನ್ಯಥಾ
	{
	    ಒಳಹರಿವು_record_buffer.tail->next = event_ಗಂಟು;
	    ಒಳಹರಿವು_record_buffer.tail = ಒಳಹರಿವು_record_buffer.tail->next;
	}
    }
    ಪ್ರತ್ಯಾಗಮ n_ಎಣಿಕೆ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_ಒಳಹರಿವು_record_buffer(ಒಳಹರಿವು_RECORD* irEvents, ಪೂರ್ಣಾಙ್ಕ nMaxLength)
{
    ಪೂರ್ಣಾಙ್ಕ n_ಎಣಿಕೆ = 0;
    ಯಾವತ್ (n_ಎಣಿಕೆ < nMaxLength && ಒಳಹರಿವು_record_buffer.head != ರಿಕ್ತ)
    {
	ಒಳಹರಿವು_record_buffer.length--;
	ಒಳಹರಿವು_record_buffer_ಗಂಟು_T *ತೆಗೆ_head = ಒಳಹರಿವು_record_buffer.head;
	irEvents[n_ಎಣಿಕೆ++] = ತೆಗೆ_head->ir;
	ಒಳಹರಿವು_record_buffer.head = ತೆಗೆ_head->next;
	vim_free(ತೆಗೆ_head);
	ಯದಿ (ಒಳಹರಿವು_record_buffer.length == 0)
	    ಒಳಹರಿವು_record_buffer.tail = ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ n_ಎಣಿಕೆ;
}
#ಅನ್ಯಥಾ  // FEAT_GUI_MSWIN && !VIMDLL
    ಶೂನ್ಯ
mch_setmouse(ಪೂರ್ಣಾಙ್ಕ on UNUSED)
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
    ಪೂರ್ಣಾಙ್ಕ
test_mswin_event(char_u *event, dict_T *args)
{
    ಪೂರ್ಣಾಙ್ಕ lpEventsWritten = 0;

# ಯದಿ defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ test_gui_w32_sendevent(event, args);
# endif

# ಯದಿ defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    ಒಳಹರಿವು_RECORD ir;
    BOOL ಒಳಹರಿವು_encoded = FALSE;
    BOOL execute = FALSE;
    ಯದಿ (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(args, "execute", FALSE);
	ಯದಿ (dict_has_ಕೀಲಿ(args, "event"))
	    ಒಳಹರಿವು_encoded = encode_ಕೀಲಿ_event(args, &ir);
	ಅನ್ಯಥಾ ಯದಿ (!execute)
	{
	    semsg(_(e_missing_argument_str), "event");
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	ಒಳಹರಿವು_encoded = encode_mouse_event(args, &ir);
    }
    ಅನ್ಯಥಾ
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "event", event);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    ಯದಿ (ಒಳಹರಿವು_encoded)
    {
	ಯದಿ ((lpEventsWritten = write_ಒಳಹರಿವು_record_buffer(&ir, 1)) < 0)
	{
	    semsg(_(e_out_of_memory), "event");
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    ಯದಿ (execute)
    {
	ಪೂರ್ಣಾಙ್ಕ save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	ಸಂದೇಶ_scroll = FALSE;
	ch_log(ರಿಕ್ತ, "test_mswin_event() executing");
	exec_normal(TRUE, TRUE, TRUE);
	ಸಂದೇಶ_scroll |= save_ಸಂದೇಶ_scroll;
    }

# endif
    ಪ್ರತ್ಯಾಗಮ lpEventsWritten;
}
#endif // FEAT_EVAL

#ಯದಿ defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_set_cursor_shape(ಪೂರ್ಣಾಙ್ಕ thickness)
{
    ಯದಿ (vtp_working)
    {
	ಯದಿ (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    ಯದಿ (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    ಅನ್ಯಥಾ
		vtp_printf("\033[0 q");	// default
	}
    }
    ಅನ್ಯಥಾ
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dwSize = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	SetConsoleCursorInfo(g_hConOut, &ConsoleCursorInfo);
	ಯದಿ (s_cursor_visible)
	    SetConsoleCursorPosition(g_hConOut, g_coord);
    }
}

    ಶೂನ್ಯ
mch_update_cursor(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		thickness;

# ifdef VIMDLL
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    ಯದಿ (shape_table[idx].shape == SHAPE_BLOCK)
	thickness = 99;	// 100 doesn't work on W95
    ಅನ್ಯಥಾ
	thickness = shape_table[idx].percentage;
    mch_set_cursor_shape(thickness);
}
#endif

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_focus_event(ಒಳಹರಿವು_RECORD ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
    ui_focus_change((ಪೂರ್ಣಾಙ್ಕ)g_fJustGotFocus);
}

ಸ್ಥಿರ ಶೂನ್ಯ ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
WaitForChar(long msec, ಪೂರ್ಣಾಙ್ಕ ignore_ಒಳಹರಿವು)
{
    DWORD	    dwNow = 0, dwEndTime = 0;
    ಒಳಹರಿವು_RECORD    ir;
    DWORD	    cRecords;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    ಪೂರ್ಣಾಙ್ಕ		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    ಯದಿ (msec > 0)
	// Wait until the specified time has elapsed.
	dwEndTime = Get_Tick_ಎಣಿಕೆ() + msec;
    ಅನ್ಯಥಾ ಯದಿ (msec < 0)
	// Wait forever.
	dwEndTime = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    ಪ್ರತಿ (;;)
    {
	// Only process messages when waiting.
	ಯದಿ (msec != 0)
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

	ಯದಿ (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_ಒಳಹರಿವು && ಒಳಹರಿವು_available())
# endif
	   )
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	ಯದಿ (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_ಎಣಿಕೆ();
	    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)(dwNow - dwEndTime) >= 0)
		ಭಙ್ಗ;
	}
	ಯದಿ (msec != 0)
	{
	    DWORD dwWaitTime = dwEndTime - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    ಯದಿ (dwWaitTime > 11)
		dwWaitTime = 11;

# ifdef FEAT_MZSCHEME
	    ಯದಿ (mzthreads_allowed() && p_mzq > 0 && (long)dwWaitTime > p_mzq)
		dwWaitTime = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    ಯದಿ (dwWaitTime > 10)
	    {
		long	due_time;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_time = check_due_timer();
		ಯದಿ (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    ಪ್ರತ್ಯಾಗಮ FALSE;
		}
		ಯದಿ (due_time > 0 && dwWaitTime > (DWORD)due_time)
		    dwWaitTime = due_time;
	    }
# endif
	    ಯದಿ (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    ಸಂದೇಶ_wait_for_multiple_objects(1, &g_hConIn, FALSE,
				  dwWaitTime, QS_SENDMESSAGE) != WAIT_OBJECT_0
# ಅನ್ಯಥಾ
		    wait_for_single_object(g_hConIn, dwWaitTime)
							      != WAIT_OBJECT_0
# endif
		    )
		ಅನುವರ್ತ;
	}

	cRecords = 0;
	peek_console_ಒಳಹರಿವು(g_hConIn, &ir, 1, &cRecords);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	ಯದಿ (State == MODE_CMDLINE && ಸಂದೇಶ_row == Rows - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_BUFFER_INFO csbi;

	    ಯದಿ (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	    {
		ಯದಿ (csbi.dwCursorPosition.Y != ಸಂದೇಶ_row)
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

	ಯದಿ (cRecords > 0)
	{
	    ಯದಿ (ir.EventType == ಕೀಲಿ_EVENT && ir.Event.ಕೀಲಿ_Event_2.b_ಕೀಲಿ_Down)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		ಯದಿ (ir.Event.ಕೀಲಿ_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.ಕೀಲಿ_Event_2.w_Virtual_ಕೀಲಿ_Code == 13)
		{
		    read_console_ಒಳಹರಿವು(g_hConIn, &ir, 1, &cRecords);
		    ಅನುವರ್ತ;
		}
# endif
		ಯದಿ (decode_ಕೀಲಿ_event(&ir.Event.ಕೀಲಿ_Event_2, &ch, &ch2,
								 ರಿಕ್ತ, FALSE))
		    ಪ್ರತ್ಯಾಗಮ TRUE;
	    }

	    read_console_ಒಳಹರಿವು(g_hConIn, &ir, 1, &cRecords);

	    ಯದಿ (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    ಅನ್ಯಥಾ ಯದಿ (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    {
		COORD dwSize = ir.Event.WindowBufferSizeEvent.dwSize;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		ಯದಿ (dwSize.X != Columns || dwSize.Y != Rows)
		{
		    CONSOLE_SCREEN_BUFFER_INFO csbi;
		    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
		    dwSize.X = csbi.srWindow.ಬಲ_3 - csbi.srWindow.ಎಡ_3 + 1;
		    dwSize.Y = csbi.srWindow.Bottom - csbi.srWindow.ತುದಿ_2 + 1;
		    ಯದಿ (dwSize.X != Columns || dwSize.Y != Rows)
		    {
			ResizeConBuf(g_hConOut, dwSize);
			shell_resized();
		    }
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		ಪ್ರತ್ಯಾಗಮ TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (msec == 0)
	    ಭಙ್ಗ;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    ಯದಿ (ಒಳಹರಿವು_available())
	ಪ್ರತ್ಯಾಗಮ TRUE;
# endif

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * return non-zero if a character is available
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_char_avail(ಶೂನ್ಯ)
{
# ifdef VIMDLL
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ TRUE;
# endif
    ಪ್ರತ್ಯಾಗಮ WaitForChar(0L, FALSE);
}

# ಯದಿ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_check_messages(ಶೂನ್ಯ)
{
#  ifdef VIMDLL
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ TRUE;
#  endif
    ಪ್ರತ್ಯಾಗಮ WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
create_conin(ಶೂನ್ಯ)
{
    g_hConIn =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES) ರಿಕ್ತ,
			OPEN_EXISTING, 0, (HANDLE)ರಿಕ್ತ);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    ಸ್ಥಿರ WCHAR
tgetch(ಪೂರ್ಣಾಙ್ಕ *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    ಪ್ರತಿ (;;)
    {
	ಒಳಹರಿವು_RECORD ir;
	DWORD cRecords = 0;

# ifdef FEAT_CLIENTSERVER
	(ಶೂನ್ಯ)WaitForChar(-1L, FALSE);
	ಯದಿ (ಒಳಹರಿವು_available())
	    ಪ್ರತ್ಯಾಗಮ 0;
	ಯದಿ (g_nMouseClick != -1)
	    ಪ್ರತ್ಯಾಗಮ 0;
# endif
	ಯದಿ (read_console_ಒಳಹರಿವು(g_hConIn, &ir, 1, &cRecords) == 0)
	{
	    ಯದಿ (did_create_conin)
		read_error_exit();
	    create_conin();
	    ಅನುವರ್ತ;
	}

	ಯದಿ (ir.EventType == ಕೀಲಿ_EVENT)
	{
	    ಯದಿ (decode_ಕೀಲಿ_event(&ir.Event.ಕೀಲಿ_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		ಪ್ರತ್ಯಾಗಮ ch;
	}
	ಅನ್ಯಥಾ ಯದಿ (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	ಅನ್ಯಥಾ ಯದಿ (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    shell_resized();
	ಅನ್ಯಥಾ ಯದಿ (ir.EventType == MOUSE_EVENT)
	{
	    ಯದಿ (decode_mouse_event(&ir.Event.MouseEvent))
		ಪ್ರತ್ಯಾಗಮ 0;
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
    ಪೂರ್ಣಾಙ್ಕ
mch_inchar(
    char_u	*buf UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		maxlen UNUSED,
    long	time UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		tb_change_cnt UNUSED)
{
#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# ಅನ್ಯಥಾ
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    ಸ್ಥಿರ char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    typeaheadlen = 0;

# ifdef VIMDLL
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    ಯದಿ (typeaheadlen > 0)
	ಗಚ್ಛ theend;

    ಯದಿ (time >= 0)
    {
	ಯದಿ (!WaitForChar(time, FALSE))     // no character available
	    ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ    // time == -1, wait forever
    {
	mch_set_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ಯದಿ (!WaitForChar(p_ut, FALSE))
	{
	    ಯದಿ (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_CURSORHOLD;
		ಪ್ರತ್ಯಾಗಮ 3;
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
    ಯದಿ (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    ಯಾವತ್ ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	ಯದಿ (typebuf_changed(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    ಭಙ್ಗ;
	}
	ಯದಿ (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    ಯದಿ (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    ಯದಿ (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		ಯದಿ (modifiers > 0)
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
	    ಅನ್ಯಥಾ
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
	ಅನ್ಯಥಾ
	{
	    WCHAR	ch2 = NUL;
	    ಪೂರ್ಣಾಙ್ಕ		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_ಕೀಲಿ(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_ಕೀಲಿಗಳು;
	    c = may_adjust_ಕೀಲಿ_for_ctrl(modifiers, c);
	    --no_reduce_ಕೀಲಿಗಳು;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_shift_modifier(modifiers, c);

	    ಯದಿ (typebuf_changed(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		ಭಙ್ಗ;
	    }

	    ಯದಿ (c == Ctrl_C && ctrl_c_interrupts)
	    {
# ಯದಿ defined(FEAT_CLIENTSERVER)
		trash_ಒಳಹರಿವು_buf();
# endif
		got_int = TRUE;
	    }

	    ಯದಿ (g_nMouseClick == -1)
	    {
		ಪೂರ್ಣಾಙ್ಕ	n = 1;

		ಯದಿ (ch2 == NUL)
		{
		    ಪೂರ್ಣಾಙ್ಕ	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    ಯದಿ (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_to_enc(ch, &n);
		    ಯದಿ (p != ರಿಕ್ತ)
		    {
			ಪ್ರತಿ (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    ಯದಿ (p[i] == CSI)
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
		ಅನ್ಯಥಾ
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    ಯದಿ (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		ಯದಿ (ch2 != NUL)
		{
		    ಯದಿ (c == K_NUL)
		    {
			ವಿಕಲ್ಪ (ch2)
			{
			ಪಕ್ಷ (WCHAR)'\324': // SHIFT+Insert
			ಪಕ್ಷ (WCHAR)'\325': // CTRL+Insert
			ಪಕ್ಷ (WCHAR)'\327': // SHIFT+Delete
			ಪಕ್ಷ (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    ಭಙ್ಗ;

			ಸ್ವಭಾವ:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    ಭಙ್ಗ;
			}
		    }
		    ಅನ್ಯಥಾ
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
		ಯದಿ ((modifiers & MOD_MASK_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_MASK_ALT;
		}

		ಯದಿ (modifiers != 0)
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
		ಯದಿ (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    ಯದಿ (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    ಯಾವತ್ (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    ಯದಿ (len > 0)
    {
	buf[len] = NUL;
	ch_log(ರಿಕ್ತ, "raw key input: \"%s\"", buf);
    }
# endif
    ಪ್ರತ್ಯಾಗಮ len;

#ಅನ್ಯಥಾ // FEAT_GUI_MSWIN && !VIMDLL
    ಪ್ರತ್ಯಾಗಮ 0;
#endif // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
executable_file(ವರ್ಣ *name, char_u **path)
{
    ಪೂರ್ಣಾಙ್ಕ attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    ಯದಿ (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    ಯದಿ (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	ಯದಿ (res == ರಿಕ್ತ)
	    res = resolve_reparse_point((char_u *)name);
	ಯದಿ (res == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	// The path is already absolute.
	ಯದಿ (path != ರಿಕ್ತ)
	    *path = res;
	ಅನ್ಯಥಾ
	    vim_free(res);
    }
    ಅನ್ಯಥಾ ಯದಿ (path != ರಿಕ್ತ)
	*path = FullName_save((char_u *)name, FALSE);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    ಸ್ಥಿರ bool
executable_prefix_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_FIND_DATAW	data;
    HANDLE		handle;
    DWORD		error;

    wn = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (wn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ true;
    handle = FindFirstFileExW(wn, FindExInfoBasic, &data,
					    Find_Ex_ಹುಡುಕು_Name_Match, ರಿಕ್ತ, 0);
    error = GetLastError();
    vim_free(wn);
    ಯದಿ (handle != INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	FindClose(handle);
	ಪ್ರತ್ಯಾಗಮ true;
    }
    ಪ್ರತ್ಯಾಗಮ error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
executable_exists(
    ವರ್ಣ	*name,
    size_t	namelen,
    char_u	**path,
    ಪೂರ್ಣಾಙ್ಕ		use_path,
    ಪೂರ್ಣಾಙ್ಕ		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3];
    char_u	*p;
    string_T	pathbuf = {ರಿಕ್ತ, 0};
    ಪೂರ್ಣಾಙ್ಕ		pathbuf_allocated = FALSE;
    string_T	pathext = {ರಿಕ್ತ, 0};
    ಪೂರ್ಣಾಙ್ಕ		pathext_allocated = FALSE;
    char_u	*shname = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		noext = FALSE;
    bool	simple_pathext = true;
    ಪೂರ್ಣಾಙ್ಕ		ext_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;

    ಯದಿ (namelen >= ಪರಿಮಾಣ(buf))	// safety check
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    ಯದಿ (strstr((ವರ್ಣ *)shname, "sh") != ರಿಕ್ತ &&
	!(strstr((ವರ್ಣ *)shname, "powershell") != ರಿಕ್ತ
				    || strstr((ವರ್ಣ *)shname, "pwsh") != ರಿಕ್ತ))
	noext = TRUE;

    ಯದಿ (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	ಯದಿ (pathext.string == ರಿಕ್ತ)
	    STR_LITERAL_SET(pathext, ".com;.exe;.bat;.cmd");
	ಅನ್ಯಥಾ
	    pathext.length = STRLEN(pathext.string);

	ಯದಿ (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    ಯಾವತ್ (*p)
	    {
		ಯದಿ (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    ಯದಿ (*p == '.')
			simple_pathext = false;
		    ++p;
		    ಅನುವರ್ತ;
		}
		e = vim_strchr(p, ';');
		ಯದಿ (e == ರಿಕ್ತ)
		    e = pathext.string + pathext.length;
		plen = (size_t)(e - p);
		++ext_ಎಣಿಕೆ;
		ಯದಿ (*p != '.')
		    simple_pathext = false;
		ಪ್ರತಿ (q = p + 1; q < e; ++q)
		    ಯದಿ (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		ಯದಿ (_strnicoll(name + namelen - plen, (ವರ್ಣ *)p, plen) == 0)
		{
		    noext = TRUE;
		    ಭಙ್ಗ;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    ಯದಿ (pathext.string == ರಿಕ್ತ)
	STR_LITERAL_SET(pathext, ".");
    ಅನ್ಯಥಾ ಯದಿ (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.length + 3);
	ಯದಿ (tmp == ರಿಕ್ತ)
	{
	    retval = FALSE;
	    ಗಚ್ಛ theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.length += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    ಯದಿ (use_path && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    ಯದಿ (pathbuf.string == ರಿಕ್ತ)
	    {
		retval = FALSE;
		ಗಚ್ಛ theend;
	    }

	    ಯದಿ (mch_getenv("NoDefaultCurrentDirectoryInExePath") == ರಿಕ್ತ &&
		    strstr((ವರ್ಣ *)gettail(p_sh), "cmd.exe") != ರಿಕ್ತ)
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
    ಯದಿ (pathbuf.string == ರಿಕ್ತ)
	STR_LITERAL_SET(pathbuf, ".");
    p = pathbuf.string;
    ಯಾವತ್ (*p)
    {
	char_u	*e;
	size_t	buflen;

	ಯದಿ (*p == ';') // Skip empty entry
	{
	    ++p;
	    ಅನುವರ್ತ;
	}
	e = vim_strchr(p, ';');
	ಯದಿ (e == ರಿಕ್ತ)
	    e = pathbuf.string + pathbuf.length;

	ಯದಿ (e - p + namelen + 2 > ಪರಿಮಾಣ(buf))
	{
	    retval = FALSE;
	    ಗಚ್ಛ theend;
	}
	// A single "." that means current dir.
	ಯದಿ (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	ಅನ್ಯಥಾ
	{
	    buflen = vim_snprintf_safelen(
		(ವರ್ಣ *)buf,
		ಪರಿಮಾಣ(buf),
		"%.*s%s%s", (ಪೂರ್ಣಾಙ್ಕ)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	ಯದಿ (!noext && simple_pathext && ext_ಎಣಿಕೆ >= 3 && buflen > 0
		&& buflen + 2 <= ಪರಿಮಾಣ(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == ರಿಕ್ತ
		&& vim_strchr(gettail(buf), ':') == ರಿಕ್ತ)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_prefix_exists(buf);
	    buf[buflen] = NUL;
	    ಯದಿ (!exists)
	    {
		p = e;
		ಅನುವರ್ತ;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	ಯಾವತ್ (*p)
	{
	    char_u  *e2;

	    ಯದಿ (*p == ';')
	    {
		// Skip empty entry
		++p;
		ಅನುವರ್ತ;
	    }
	    e2 = vim_strchr(p, ';');
	    ಯದಿ (e2 == ರಿಕ್ತ)
		e2 = pathext.string + pathext.length;

	    ಯದಿ (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		ಯದಿ (e2 - p + buflen + 1 > ಪರಿಮಾಣ(buf))
		{
		    retval = FALSE;
		    ಗಚ್ಛ theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    ಯದಿ (executable_file((ವರ್ಣ *)buf, path))
	    {
		retval = TRUE;
		ಗಚ್ಛ theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    ಯದಿ (pathbuf_allocated)
	ಮುಕ್ತ(pathbuf.string);
    ಯದಿ (pathext_allocated)
	ಮುಕ್ತ(pathext.string);
    ಪ್ರತ್ಯಾಗಮ retval;
}

#ಯದಿ (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    ಸ್ಥಿರ ಶೂನ್ಯ
bad_param_handler(ಧ್ರುವ wchar_t *expression UNUSED,
    ಧ್ರುವ wchar_t *function UNUSED,
    ಧ್ರುವ wchar_t *file UNUSED,
    unsigned ಪೂರ್ಣಾಙ್ಕ ಸಾಲು UNUSED,
    uintptr_t pReserved UNUSED)
{
}

# define SET_INVALID_PARAM_HANDLER \
	((ಶೂನ್ಯ)_set_invalid_parameter_handler(bad_param_handler))
#ಅನ್ಯಥಾ
# define SET_INVALID_PARAM_HANDLER
#endif

#ifdef FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_init_g(ಶೂನ್ಯ)
{
# ifndef __MINGW32__
    ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_ದ್ವಿಮಾನ;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    Rows = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[_MAX_PATH + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	ಯದಿ (exe_pathlen + 10 >= ಪರಿಮಾಣ(vimrun_location))
	{
	    ಯದಿ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ರಿಕ್ತ, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    // First try in same directory as gvim.exe.
	    ಯದಿ (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    ಯದಿ (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		ಯದಿ (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(ವರ್ಣ *)vimrun_location,
			ಪರಿಮಾಣ(vimrun_location),
			"\"%.*svimrun\" ",
			(ಪೂರ್ಣಾಙ್ಕ)exe_pathlen, exe_name);
		}
		ಅನ್ಯಥಾ
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		ಯದಿ (p != ರಿಕ್ತ)
		{
		    vimrun_path.string = p;
		    vimrun_path.length = plen;
		    vimrun_path_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ರಿಕ್ತ, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	ಯದಿ (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    ಯದಿ (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    ರಿಕ್ತ, TRUE, FALSE))
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#endif // FEAT_GUI_MSWIN

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).ಬಲ_3 - (sr).ಎಡ_3 + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).ತುದಿ_2 + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    ಸ್ಥಿರ BOOL
ClearConsoleBuffer(WORD wAttribute)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    DWORD ಅಂಕೆ_Cells, dummy;

    ಯದಿ (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಅಂಕೆ_Cells = csbi.dwSize.X * csbi.dwSize.Y;
    coord.X = 0;
    coord.Y = 0;
    ಯದಿ (!FillConsoleOutputCharacter(g_hConOut, ' ', ಅಂಕೆ_Cells,
	    coord, &dummy))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (!FillConsoleOutputAttribute(g_hConOut, wAttribute, ಅಂಕೆ_Cells,
	    coord, &dummy))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    ಸ್ಥಿರ BOOL
FitConsoleWindow(
    COORD dwBufferSize,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD dwWindowSize;
    BOOL NeedAdjust = FALSE;

    ಯದಿ (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    ಯದಿ (csbi.srWindow.ಬಲ_3 >= dwBufferSize.X)
    {
	dwWindowSize.X = SRWIDTH(csbi.srWindow);
	ಯದಿ (dwWindowSize.X > dwBufferSize.X)
	    dwWindowSize.X = dwBufferSize.X;
	csbi.srWindow.ಬಲ_3 = dwBufferSize.X - 1;
	csbi.srWindow.ಎಡ_3 = dwBufferSize.X - dwWindowSize.X;
	NeedAdjust = TRUE;
    }
    ಯದಿ (csbi.srWindow.Bottom >= dwBufferSize.Y)
    {
	dwWindowSize.Y = SRHEIGHT(csbi.srWindow);
	ಯದಿ (dwWindowSize.Y > dwBufferSize.Y)
	    dwWindowSize.Y = dwBufferSize.Y;
	csbi.srWindow.Bottom = dwBufferSize.Y - 1;
	csbi.srWindow.ತುದಿ_2 = dwBufferSize.Y - dwWindowSize.Y;
	NeedAdjust = TRUE;
    }
    ಯದಿ (NeedAdjust && WantAdjust)
    {
	ಯದಿ (!SetConsoleWindowInfo(g_hConOut, TRUE, &csbi.srWindow))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ConsoleBufferStruct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_BUFFER_INFO	Info;
    PCHAR_INFO			Buffer;
    COORD			BufferSize;
    PSMALL_RECT			Regions;
    ಪೂರ್ಣಾಙ್ಕ				ಅಂಕೆ_Regions;
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
    ಸ್ಥಿರ BOOL
SaveConsoleBuffer(
    ConsoleBuffer *cb)
{
    DWORD ಅಂಕೆ_Cells;
    COORD BufferCoord;
    SMALL_RECT ReadRegion;
    WORD Y, Y_incr;
    ಪೂರ್ಣಾಙ್ಕ i, numregions;

    ಯದಿ (cb == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (!GetConsoleScreenBufferInfo(g_hConOut, &cb->Info))
    {
	cb->IsValid = FALSE;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    ಯದಿ (use_alternate_screen_buffer)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    ಯದಿ (!cb->IsValid || cb->Buffer == ರಿಕ್ತ ||
	    cb->BufferSize.X != cb->Info.dwSize.X ||
	    cb->BufferSize.Y != cb->Info.dwSize.Y)
    {
	cb->BufferSize.X = cb->Info.dwSize.X;
	cb->BufferSize.Y = cb->Info.dwSize.Y;
	ಅಂಕೆ_Cells = cb->BufferSize.X * cb->BufferSize.Y;
	vim_free(cb->Buffer);
	cb->Buffer = ALLOC_MULT(CHAR_INFO, ಅಂಕೆ_Cells);
	ಯದಿ (cb->Buffer == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ReadRegion.ಎಡ_3 = 0;
    ReadRegion.ಬಲ_3 = cb->Info.dwSize.X - 1;
    Y_incr = 12000 / cb->Info.dwSize.X;

    numregions = (cb->Info.dwSize.Y + Y_incr - 1) / Y_incr;
    ಯದಿ (cb->Regions == ರಿಕ್ತ || numregions != cb->ಅಂಕೆ_Regions)
    {
	cb->ಅಂಕೆ_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->ಅಂಕೆ_Regions);
	ಯದಿ (cb->Regions == ರಿಕ್ತ)
	{
	    VIM_CLEAR(cb->Buffer);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }

    ಪ್ರತಿ (i = 0, Y = 0; i < cb->ಅಂಕೆ_Regions; i++, Y += Y_incr)
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
	ReadRegion.ತುದಿ_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	ಯದಿ (!ReadConsoleOutputW(g_hConOut,	// output handle
		cb->Buffer,			// our buffer
		cb->BufferSize,			// dimensions of our buffer
		BufferCoord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->Buffer);
	    VIM_CLEAR(cb->Regions);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಸ್ಥಿರ BOOL
RestoreConsoleBuffer(
    ConsoleBuffer   *cb,
    BOOL	    RestoreScreen)
{
    COORD BufferCoord;
    SMALL_RECT WriteRegion;
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (cb == ರಿಕ್ತ || !cb->IsValid)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    ಯದಿ (RestoreScreen)
	ClearConsoleBuffer(cb->Info.wAttributes);

    FitConsoleWindow(cb->Info.dwSize, TRUE);
    ಯದಿ (!SetConsoleScreenBufferSize(g_hConOut, cb->Info.dwSize))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (!SetConsoleTextAttribute(g_hConOut, cb->Info.wAttributes))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಯದಿ (!SetConsoleCursorPosition(g_hConOut, cb->Info.dwCursorPosition))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (!SetConsoleWindowInfo(g_hConOut, TRUE, &cb->Info.srWindow))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    ಯದಿ (use_alternate_screen_buffer)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    /*
     * Restore the screen buffer contents.
     */
    ಯದಿ (cb->Buffer != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (i = 0; i < cb->ಅಂಕೆ_Regions; i++)
	{
	    BufferCoord.X = cb->Regions[i].ಎಡ_3;
	    BufferCoord.Y = cb->Regions[i].ತುದಿ_2;
	    WriteRegion = cb->Regions[i];
	    ಯದಿ (!WriteConsoleOutputW(g_hConOut,	// output handle
			cb->Buffer,		// our buffer
			cb->BufferSize,		// dimensions of our buffer
			BufferCoord,		// offset in our buffer
			&WriteRegion))		// region to restore
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
ಸ್ಥಿರ ConsoleBuffer g_cbOrig = { 0 };
# endif
ಸ್ಥಿರ ConsoleBuffer g_cbNonTermcap = { 0 };
ಸ್ಥಿರ ConsoleBuffer g_cbTermcap = { 0 };

ವರ್ಣ g_szOrigTitle[256] = { 0 };
HWND g_hWnd = ರಿಕ್ತ;	// also used in os_mswin.c
ಸ್ಥಿರ HICON g_hOrigIconSmall = ರಿಕ್ತ;
ಸ್ಥಿರ HICON g_hOrigIcon = ರಿಕ್ತ;
ಸ್ಥಿರ HICON g_hVimIcon = ರಿಕ್ತ;
ಸ್ಥಿರ BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ಸ್ಥಿರ BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    ಯದಿ (hWnd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (phIconSmall != ರಿಕ್ತ)
	*phIconSmall = (HICON)SendMessage(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    ಯದಿ (phIcon != ರಿಕ್ತ)
	*phIcon = (HICON)SendMessage(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ಸ್ಥಿರ BOOL
SetConsoleIcon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    ಯದಿ (hWnd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (hIconSmall != ರಿಕ್ತ)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    ಯದಿ (hIcon != ರಿಕ್ತ)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
SaveConsoleTitleAndIcon(ಶೂನ್ಯ)
{
    // Save the original title.
    ಯದಿ (!GetConsoleTitle(g_szOrigTitle, ಪರಿಮಾಣ(g_szOrigTitle)))
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = GetConsoleWindow();
    ಯದಿ (g_hWnd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    ಯದಿ (g_hOrigIconSmall == ರಿಕ್ತ || g_hOrigIcon == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Extract the first icon contained in the Vim executable.
    ಯದಿ (
# ifdef FEAT_LIBCALL
	    mch_icon_load((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == ರಿಕ್ತ)
	g_hVimIcon = ExtractIcon(ರಿಕ್ತ, (LPCSTR)exe_name, 0);
    ಯದಿ (g_hVimIcon != ರಿಕ್ತ)
	g_fCanChangeIcon = TRUE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_fWindInitCalled = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ g_fTermcapMode = FALSE;
ಸ್ಥಿರ CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_init_c(ಶೂನ್ಯ)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_BUFFER_INFO csbi;
# endif
# ifndef __MINGW32__
    ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_ದ್ವಿಮಾನ;		// we do our own CR-LF translation
    out_flush();

    // Obtain handles for the standard Console I/O devices
    ಯದಿ (read_cmd_fd == 0)
	g_hConIn =  GetStdHandle(STD_ಒಳಹರಿವು_HANDLE);
    ಅನ್ಯಥಾ
	create_conin();
    g_hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    SaveConsoleBuffer(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.wAttributes;
# ಅನ್ಯಥಾ
    // Get current text attributes
    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
# endif
    ಯದಿ (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (g_attrCurrent & 0xf) + 1;
    ಯದಿ (cterm_normal_bg_color == 0)
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
    ಯದಿ (g_fCanChangeIcon)
	SetConsoleIcon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    ಯದಿ (use_alternate_screen_buffer)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    ಯದಿ (fdDump)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_exit_c(ಪೂರ್ಣಾಙ್ಕ r)
{
    // Copy flag since stoptermcap() will clear the flag.
    ಪೂರ್ಣಾಙ್ಕ fTermcapMode = g_fTermcapMode;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    ಯದಿ (!fTermcapMode &&  use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    ಯದಿ (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    ಯದಿ (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	ಯದಿ (g_fCanChangeIcon && !g_fForceExit)
	    SetConsoleIcon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	ಯದಿ (fdDump)
	{
	    time_t t;

	    time(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = ರಿಕ್ತ;
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

    ಶೂನ್ಯ
mch_init(ಶೂನ್ಯ)
{
#ifdef VIMDLL
    ಯದಿ (gui.starting)
	mch_init_g();
    ಅನ್ಯಥಾ
	mch_init_c();
#ನೋಚೇದ್ಯದಿ defined(FEAT_GUI_MSWIN)
    mch_init_g();
#ಅನ್ಯಥಾ
    mch_init_c();
#endif
}

    ಶೂನ್ಯ
mch_exit(ಪೂರ್ಣಾಙ್ಕ r)
{
#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef VIMDLL
    ಯದಿ (vimrun_path_allocated)
	vim_free(vimrun_path.string);

    ಯದಿ (gui.in_use || gui.starting)
	mch_exit_g(r);
    ಅನ್ಯಥಾ
	mch_exit_c(r);
#ನೋಚೇದ್ಯದಿ defined(FEAT_GUI_MSWIN)
    ಯದಿ (vimrun_path_allocated)
	vim_free(vimrun_path.string);
    mch_exit_g(r);
#ಅನ್ಯಥಾ
    mch_exit_c(r);
#endif
}

/*
 * Do we have an interactive window?
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_check_win(
    ಪೂರ್ಣಾಙ್ಕ argc UNUSED,
    ವರ್ಣ **argv UNUSED)
{
    mch_get_exe_name();

#ಯದಿ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    ಪ್ರತ್ಯಾಗಮ OK;	    // GUI always has a tty
#ಅನ್ಯಥಾ
# ifdef VIMDLL
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ OK;
# endif
    ಯದಿ (isatty(1))
	ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ FAIL;
#endif
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    ಶೂನ್ಯ
fname_case(
    char_u	*name,
    ಪೂರ್ಣಾಙ್ಕ		len)
{
    WCHAR   *p;
    WCHAR   buf[_MAX_PATH + 1];

    ಯದಿ (*name == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    slash_adjust(name);

    p = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (GetLongPathNameW(p, buf, _MAX_PATH))
    {
	char_u	*q = utf16_to_enc(buf, ರಿಕ್ತ);

	ಯದಿ (q != ರಿಕ್ತ)
	{
	    ಯದಿ (len > 0)
		vim_strncpy(name, q, len - 1);
	    ಅನ್ಯಥಾ
	    {
		size_t  namelen = STRLEN(name);

		ಯದಿ (namelen >= STRLEN(q))
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
    ಪೂರ್ಣಾಙ್ಕ
mch_get_user_name(
    char_u  *s,
    ಪೂರ್ಣಾಙ್ಕ	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_LENGTH(wszUserName);

    ಯದಿ (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_to_enc(wszUserName, ರಿಕ್ತ);

	ಯದಿ (p != ರಿಕ್ತ)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
    }
    s[0] = NUL;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}


/*
 * Insert host name in s[len].
 */
    ಶೂನ್ಯ
mch_get_host_name(
    char_u	*s,
    ಪೂರ್ಣಾಙ್ಕ		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_LENGTH(wszHostName);

    ಯದಿ (!GetComputerNameW(wszHostName, &wcch))
	ಪ್ರತ್ಯಾಗಮ;

    char_u  *p = utf16_to_enc(wszHostName, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    long
mch_get_pid(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (long)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_process_running(long pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   status = 0;
    ಪೂರ್ಣಾಙ್ಕ	    ret = FALSE;

    ಯದಿ (hProcess == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;  // might not have access
    ಯದಿ (GetExitCodeProcess(hProcess, &status))
	ret = status == STILL_ACTIVE;
    CloseHandle(hProcess);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_dirname(
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		len)
{
    WCHAR   wbuf[_MAX_PATH + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    ಯದಿ (GetCurrentDirectoryW(_MAX_PATH, wbuf) == 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    WCHAR   wcbuf[_MAX_PATH + 1];
    char_u  *p = ರಿಕ್ತ;

    ಯದಿ (GetLongPathNameW(wbuf, wcbuf, _MAX_PATH) != 0)
    {
	p = utf16_to_enc(wcbuf, ರಿಕ್ತ);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ಯದಿ (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    ಯದಿ (p == ರಿಕ್ತ)
	p = utf16_to_enc(wbuf, ರಿಕ್ತ);

    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    long
mch_getperm(char_u *name)
{
    stat_T	st;

    ಪ್ರತ್ಯಾಗಮ (mch_stat((ವರ್ಣ *)name, &st) == 0)
	? (long)(unsigned short)st.st_mode
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_setperm(char_u *name, long perm)
{
    long	n;
    WCHAR	*p;

    p = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    ಯದಿ (n == -1)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    win32_set_archive(name);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set hidden flag for "name".
 */
    ಶೂನ್ಯ
mch_hide(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ attrs = win32_getattrs(name);
    ಯದಿ (attrs == -1)
	ಪ್ರತ್ಯಾಗಮ;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_ishidden(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ f = win32_getattrs(name);

    ಯದಿ (f == -1)
	ಪ್ರತ್ಯಾಗಮ FALSE;		    // file does not exist at all

    ಪ್ರತ್ಯಾಗಮ (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_isdir(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ f = win32_getattrs(name);

    ಯದಿ (f == -1)
	ಪ್ರತ್ಯಾಗಮ FALSE;		    // file does not exist at all

    ಪ್ರತ್ಯಾಗಮ (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    attr = GetFileAttributesW(p);
    ಯದಿ (attr == INVALID_FILE_ATTRIBUTES)
	error = GetLastError();
    vim_free(p);

    ಯದಿ (attr == INVALID_FILE_ATTRIBUTES)
	ಪ್ರತ್ಯಾಗಮ error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    ಪ್ರತ್ಯಾಗಮ (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_isrealdir(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    ಪೂರ್ಣಾಙ್ಕ	    retval;

    p = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    retval = _wmkdir(p);
    vim_free(p);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    ಪೂರ್ಣಾಙ್ಕ	    retval;

    p = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    retval = _wrmdir(p);
    vim_free(p);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    ಪ್ರತ್ಯಾಗಮ win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_ಸಂಖ್ಯೆ_Of_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_is_symbolic_link(char_u *name)
{
    HANDLE		hFind;
    ಪೂರ್ಣಾಙ್ಕ			res = FALSE;
    DWORD		fileFlags = 0, reparseTag = 0;
    WCHAR		*wn;
    WIN32_FIND_DATAW	findDataW;

    wn = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (wn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    hFind = FindFirstFileW(wn, &findDataW);
    vim_free(wn);
    ಯದಿ (hFind != INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	fileFlags = findDataW.dwFileAttributes;
	reparseTag = findDataW.dwReserved0;
	FindClose(hFind);
    }

    ಯದಿ ((fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
	    && (reparseTag == IO_REPARSE_TAG_SYMLINK
		|| reparseTag == IO_REPARSE_TAG_MOUNT_POINT))
	res = TRUE;

    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_is_linked(char_u *fname)
{
    ಯದಿ (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    ಪೂರ್ಣಾಙ್ಕ
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    ಪೂರ್ಣಾಙ್ಕ		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_to_utf16(fname, ರಿಕ್ತ);
    ಯದಿ (wn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    ರಿಕ್ತ,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    ರಿಕ್ತ);		// handle to template file
    vim_free(wn);

    ಯದಿ (hFile == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ FILEINFO_READ_FAIL;

    ಯದಿ (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    ಅನ್ಯಥಾ
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
win32_getattrs(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		attr;
    WCHAR	*p;

    p = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ INVALID_FILE_ATTRIBUTES;

    attr = GetFileAttributesW(p);
    vim_free(p);

    ಪ್ರತ್ಯಾಗಮ attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
win32_setattrs(char_u *name, ಪೂರ್ಣಾಙ್ಕ attrs)
{
    ಪೂರ್ಣಾಙ್ಕ	    res;
    WCHAR   *p;

    p = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    res = SetFileAttributesW(p, attrs);
    vim_free(p);

    ಪ್ರತ್ಯಾಗಮ res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
win32_set_archive(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ attrs = win32_getattrs(name);
    ಯದಿ (attrs == -1)
	ಪ್ರತ್ಯಾಗಮ -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    ಪ್ರತ್ಯಾಗಮ win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_writable(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ attrs = win32_getattrs(name);

    ಪ್ರತ್ಯಾಗಮ (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_can_exe(char_u *name, char_u **path, ಪೂರ್ಣಾಙ್ಕ use_path UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ executable_exists((ವರ್ಣ *)name, STRLEN(name), path, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    ಪೂರ್ಣಾಙ್ಕ		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    ಯದಿ (STRNCMP(name, "\\\\.\\", 4) == 0)
	ಪ್ರತ್ಯಾಗಮ ಗಂಟು_WRITABLE;

    wn = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (wn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ಗಂಟು_NORMAL;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    ರಿಕ್ತ,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    ರಿಕ್ತ);		    // handle to template file
    vim_free(wn);
    ಯದಿ (hFile == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ ಗಂಟು_NORMAL;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    ಯದಿ (type == FILE_TYPE_CHAR)
	ಪ್ರತ್ಯಾಗಮ ಗಂಟು_WRITABLE;
    ಯದಿ (type == FILE_TYPE_DISK)
	ಪ್ರತ್ಯಾಗಮ ಗಂಟು_NORMAL;
    ಪ್ರತ್ಯಾಗಮ ಗಂಟು_OTHER;
}

#ifdef HAVE_ACL
ಸಂರಚನಾ my_acl
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
    ಪ್ರತ್ಯಾಗಮ (vim_acl_T)ರಿಕ್ತ;
#ಅನ್ಯಥಾ
    ಸಂರಚನಾ my_acl   *p = ರಿಕ್ತ;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(ಸಂರಚನಾ my_acl);
    ಯದಿ (p != ರಿಕ್ತ)
    {
	WCHAR	*wn;

	wn = enc_to_utf16(fname, ರಿಕ್ತ);
	ಯದಿ (wn == ರಿಕ್ತ)
	{
	    vim_free(p);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
	ಯದಿ (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (ಶೂನ್ಯ)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    ರಿಕ್ತ,
		    ರಿಕ್ತ,
		    &p->pDacl,
		    ರಿಕ್ತ,
		    &p->pSecurityDescriptor);
	}
	ಯದಿ (p->pSecurityDescriptor == ರಿಕ್ತ)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = ರಿಕ್ತ;
	}
	vim_free(wn);
    }

    ಪ್ರತ್ಯಾಗಮ (vim_acl_T)p;
#endif
}

#ifdef HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    ಸ್ಥಿರ BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_SIZE_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_ಎಣಿಕೆ = 0;
    GetAclInformation(acl, &acl_info, ಪರಿಮಾಣ(acl_info), AclSizeInformation);
    ಪ್ರತಿ (i = 0; i < acl_info.Ace_ಎಣಿಕೆ; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	ಯದಿ (ace->Header.AceFlags & INHERITED_ACE)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    ಶೂನ್ಯ
mch_set_acl(char_u *fname, vim_acl_T acl)
{
#ifdef HAVE_ACL
    ಸಂರಚನಾ my_acl   *p = (ಸಂರಚನಾ my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    wn = enc_to_utf16(fname, ರಿಕ್ತ);
    ಯದಿ (wn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Set security flags
    ಯದಿ (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    ಯದಿ (p->pSidGroup)
	sec_info |= GROUP_SECURITY_INFORMATION;
    ಯದಿ (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	ಯದಿ (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    ಯದಿ (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (ಶೂನ್ಯ)SetNamedSecurityInfoW(
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

    ಶೂನ್ಯ
mch_free_acl(vim_acl_T acl)
{
#ifdef HAVE_ACL
    ಸಂರಚನಾ my_acl   *p = (ಸಂರಚನಾ my_acl *)acl;

    ಯದಿ (p != ರಿಕ್ತ)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#endif
}

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    ಸ್ಥಿರ BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    ಒಳಹರಿವು_RECORD ir;
    DWORD out;

    ವಿಕಲ್ಪ (dwCtrlType)
    {
    ಪಕ್ಷ CTRL_C_EVENT:
	ಯದಿ (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಪಕ್ಷ CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = ಕೀಲಿ_EVENT;
	ir.Event.ಕೀಲಿ_Event_2.b_ಕೀಲಿ_Down = TRUE;
	ir.Event.ಕೀಲಿ_Event_2.w_Repeat_ಎಣಿಕೆ = 1;
	ir.Event.ಕೀಲಿ_Event_2.w_Virtual_ಕೀಲಿ_Code = VK_CANCEL;
	ir.Event.ಕೀಲಿ_Event_2.wVirtualScanCode = 0;
	ir.Event.ಕೀಲಿ_Event_2.dw_Control_ಕೀಲಿ_State = 0;
	ir.Event.ಕೀಲಿ_Event_2.uChar.UnicodeChar = 0;
	Write_Console_ಒಳಹರಿವು(g_hConIn, &ir, 1, &out);
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // fatal events: shut down gracefully
    ಪಕ್ಷ CTRL_CLOSE_EVENT:
    ಪಕ್ಷ CTRL_LOGOFF_EVENT:
    ಪಕ್ಷ CTRL_SHUTDOWN_EVENT:
	windgoto((ಪೂರ್ಣಾಙ್ಕ)Rows - 1, cmdline_col_off);
	g_fForceExit = TRUE;

	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	ಪ್ರತ್ಯಾಗಮ TRUE;		// not reached

    ಸ್ವಭಾವ:
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    ಶೂನ್ಯ
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;
# endif
    GetConsoleMode(g_hConIn, &cmodein);
    GetConsoleMode(g_hConOut, &cmodeout);
    ಯದಿ (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_ಸಾಲು_ಒಳಹರಿವು | ENABLE_PROCESSED_ಒಳಹರಿವು |
		     ENABLE_ECHO_ಒಳಹರಿವು);
	ಯದಿ (g_fMouseActive)
	{
	    cmodein |= ENABLE_MOUSE_ಒಳಹರಿವು;
	    cmodein &= ~ENABLE_QUICK_EDIT_MODE;
	}
	ಅನ್ಯಥಾ
	{
	    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# ಅನ್ಯಥಾ
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    ಅನ್ಯಥಾ // cooked
    {
	cmodein |= (ENABLE_ಸಾಲು_ಒಳಹರಿವು | ENABLE_PROCESSED_ಒಳಹರಿವು |
		    ENABLE_ECHO_ಒಳಹರಿವು);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
    SetConsoleMode(g_hConOut, cmodeout);
    SetConsoleCtrlHandler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    ಯದಿ (fdDump)
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
    ಪೂರ್ಣಾಙ್ಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

# ifdef VIMDLL
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ OK;
# endif
    ಯದಿ (!g_fTermcapMode && g_cbTermcap.IsValid)
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
    ಅನ್ಯಥಾ ಯದಿ (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	Rows = csbi.srWindow.Bottom - csbi.srWindow.ತುದಿ_2 + 1;
	Columns = csbi.srWindow.ಬಲ_3 - csbi.srWindow.ಎಡ_3 + 1;
    }
    ಅನ್ಯಥಾ
    {
	Rows = 25;
	Columns = 80;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    ಯದಿ (use_alternate_screen_buffer)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!SetConsoleScreenBufferSize(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	ಯದಿ (fdDump)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ResizeWindow(
    HANDLE     hConsole,
    SMALL_RECT srWindowRect)
{
    ಯದಿ (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
    {
# ifdef MCH_WRITE_DUMP
	ಯದಿ (fdDump)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ResizeConBufAndWindow(
    HANDLE  hConsole,
    ಪೂರ್ಣಾಙ್ಕ	    xSize,
    ಪೂರ್ಣಾಙ್ಕ	    ySize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    srWindowRect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    ಯದಿ (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", xSize, ySize);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    // define the new console window size and scroll position
    srWindowRect.ಎಡ_3 = srWindowRect.ತುದಿ_2 = (SHORT) 0;
    srWindowRect.ಬಲ_3 =  (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);

    ಯದಿ (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	ಪೂರ್ಣಾಙ್ಕ sx, sy;

	sx = csbi.srWindow.ಬಲ_3 - csbi.srWindow.ಎಡ_3 + 1;
	sy = csbi.srWindow.Bottom - csbi.srWindow.ತುದಿ_2 + 1;
	ಯದಿ (sy < ySize || sx < xSize)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    ಯದಿ (sy < ySize)
		coordScreen.Y = ySize;
	    ಅನ್ಯಥಾ
		coordScreen.Y = sy;
	    ಯದಿ (sx < xSize)
		coordScreen.X = xSize;
	    ಅನ್ಯಥಾ
		coordScreen.X = sx;
	    SetConsoleScreenBufferSize(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = xSize;
    coordScreen.Y = ySize;

    // In the new console call API, only the first time in reverse order
    ಯದಿ (!vtp_working || resized)
    {
	ResizeWindow(hConsole, srWindowRect);
	ResizeConBuf(hConsole, coordScreen);
    }
    ಅನ್ಯಥಾ
    {
	// Workaround for a Windows 10 bug
	cursor.X = srWindowRect.ಎಡ_3;
	cursor.Y = srWindowRect.ತುದಿ_2;
	SetConsoleCursorPosition(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	ResizeWindow(hConsole, srWindowRect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    ಶೂನ್ಯ
mch_set_shellsize(ಶೂನ್ಯ)
{
    COORD coordScreen;

# ifdef VIMDLL
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;
# endif
    // Don't change window size while still starting up
    ಯದಿ (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (term_console)
    {
	coordScreen = GetLargestConsoleWindowSize(g_hConOut);

	// Clamp Rows and Columns to reasonable values
	ಯದಿ (Rows > coordScreen.Y)
	    Rows = coordScreen.Y;
	ಯದಿ (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	ResizeConBufAndWindow(g_hConOut, Columns, Rows);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    ಶೂನ್ಯ
mch_new_shellsize(ಶೂನ್ಯ)
{
# ifdef VIMDLL
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;
# endif
    set_scroll_region(0, 0, Columns - 1, Rows - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    ಶೂನ್ಯ
mch_set_winsize_now(ಶೂನ್ಯ)
{
    ಯದಿ (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_set_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# ಯದಿ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    ಶೂನ್ಯ
mch_calc_cell_size(ಸಂರಚನಾ cellsize *cs_out)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_out->cs_xpixel = -1;
    cs_out->cs_ypixel = -1;

    ಯದಿ (g_hConOut == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ;

    cfi.cbSize = ಪರಿಮಾಣ(cfi);
    ಯದಿ (GetCurrentConsoleFontEx(g_hConOut, FALSE, &cfi)
	    && cfi.dwFontSize.X > 0 && cfi.dwFontSize.Y > 0)
    {
	cs_out->cs_xpixel = cfi.dwFontSize.X;
	cs_out->cs_ypixel = cfi.dwFontSize.Y;
	ಪ್ರತ್ಯಾಗಮ;
    }

    GetConsoleMode(g_hConIn, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_ಸಾಲು_ಒಳಹರಿವು | ENABLE_ECHO_ಒಳಹರಿವು
	    | ENABLE_PROCESSED_ಒಳಹರಿವು);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_ಒಳಹರಿವು;

    ಯದಿ (SetConsoleMode(g_hConIn, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	SetConsoleMode(g_hConIn, cmodeold);
    }
}
# endif

#endif

    ಸ್ಥಿರ BOOL
vim_create_process(
    ವರ್ಣ		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    ವರ್ಣ		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = ರಿಕ್ತ;

    wcmd = enc_to_utf16((char_u *)cmd, ರಿಕ್ತ);
    ಯದಿ (wcmd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (cwd != ರಿಕ್ತ)
    {
	wcwd = enc_to_utf16((char_u *)cwd, ರಿಕ್ತ);
	ಯದಿ (wcwd == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
    }

    ret = CreateProcessW(
	    ರಿಕ್ತ,		// Executable name
	    wcmd,		// Command to execute
	    ರಿಕ್ತ,		// Process security attributes
	    ರಿಕ್ತ,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    ಪ್ರತ್ಯಾಗಮ ret;
}


    ಸ್ಥಿರ HINSTANCE
vim_shell_execute(
    ವರ್ಣ *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_to_utf16((char_u *)cmd, ರಿಕ್ತ);
    ಯದಿ (wcmd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ (HINSTANCE) 0;

    ret = ShellExecuteW(ರಿಕ್ತ, ರಿಕ್ತ, wcmd, ರಿಕ್ತ, ರಿಕ್ತ, n_show_cmd);
    vim_free(wcmd);
    ಪ್ರತ್ಯಾಗಮ ret;
}


#ಯದಿ defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mch_system_classic(ವರ್ಣ *cmd, ಪೂರ್ಣಾಙ್ಕ options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = ಪರಿಮಾಣ(si);
    si.lpReserved = ರಿಕ್ತ;
    si.lpDesktop = ರಿಕ್ತ;
    si.lpTitle = ರಿಕ್ತ;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    ಯದಿ (options & SHELL_DOOUT)
	si.wShowWindow = SW_SHOWMINNOACTIVE;
    ಅನ್ಯಥಾ
	si.wShowWindow = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = ರಿಕ್ತ;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_MODE |	CREATE_NEW_CONSOLE,
	    &si, &pi, ರಿಕ್ತ, ರಿಕ್ತ);

    // Wait for the command to terminate before continuing
    {
	ಪೂರ್ಣಾಙ್ಕ	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	ಪ್ರತಿ (;;)
	{
	    ಸಂದೇಶ_2	ಸಂದೇಶ;

	    ಯದಿ (PeekMessageW(&ಸಂದೇಶ, (HWND)ರಿಕ್ತ, 0, 0, PM_REMOVE))
	    {
		TranslateMessage(&ಸಂದೇಶ);
		DispatchMessageW(&ಸಂದೇಶ);
		delay = 1;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		ಭಙ್ಗ;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    ಯದಿ (delay < 50)
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

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_hChildStd_IN_Wr = param;
    linenr_T	    lnum = curbuf->b_op_start.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    ಪೂರ್ಣಾಙ್ಕ		    written = 0;

    ಪ್ರತಿ (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	ಯದಿ (l == 0)
	    len = 0;
	ಅನ್ಯಥಾ ಯದಿ (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_hChildStd_IN_Wr, "", 1, &len, ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_hChildStd_IN_Wr, (ವರ್ಣ *)lp + written,
		      s == ರಿಕ್ತ ? l : (DWORD)(s - (lp + written)),
		      &len, ರಿಕ್ತ);
	}
	ಯದಿ (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    ಯದಿ (lnum != curbuf->b_op_end.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_hChildStd_IN_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, ರಿಕ್ತ);
	    }

	    ++lnum;
	    ಯದಿ (lnum > curbuf->b_op_end.lnum)
		ಭಙ್ಗ;

	    lp = ml_get(lnum);
	    written = 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_hChildStd_IN_Wr);
    ಪ್ರತ್ಯಾಗಮ 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dump_pipe(ಪೂರ್ಣಾಙ್ಕ	    options,
	  HANDLE    g_hChildStd_OUT_Rd,
	  garray_T  *ga,
	  char_u    buffer[],
	  DWORD	    *buffer_off)
{
    DWORD	availableBytes = 0;
    DWORD	i;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    DWORD	len;
    DWORD	toRead;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_hChildStd_OUT_Rd, // pipe to query
			ರಿಕ್ತ,		    // optional buffer
			0,		    // buffer size
			ರಿಕ್ತ,		    // number of read bytes
			&availableBytes,    // available bytes total
			ರಿಕ್ತ);		    // byteLeft

    // We got real data in the pipe, read it
    ಯಾವತ್ (ret != 0 && availableBytes > 0)
    {
	toRead = (DWORD)(BUFLEN - *buffer_off);
	toRead = availableBytes < toRead ? availableBytes : toRead;
	ReadFile(g_hChildStd_OUT_Rd, buffer + *buffer_off, toRead , &len, ರಿಕ್ತ);

	// If we haven't read anything, there is a problem
	ಯದಿ (len == 0)
	    ಭಙ್ಗ;

	availableBytes -= len;

	ಯದಿ (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    ಪ್ರತಿ (i = 0; i < len; ++i)
	    {
		ಯದಿ (buffer[i] == NL)
		    append_ga_ಸಾಲು(ga);
		ಅನ್ಯಥಾ ಯದಿ (buffer[i] == NUL)
		    ga_append(ga, NL);
		ಅನ್ಯಥಾ
		    ga_append(ga, buffer[i]);
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
	{
	    ಪೂರ್ಣಾಙ್ಕ		l;
	    ಪೂರ್ಣಾಙ್ಕ		c;
	    char_u	*p;

	    len += *buffer_off;
	    buffer[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    ಪ್ರತಿ (p = buffer; p < buffer + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		ಯದಿ (l == 0)
		    l = 1;  // NUL byte?
		ಅನ್ಯಥಾ ಯದಿ (MB_BYTE2LEN(*p) != l)
		    ಭಙ್ಗ;
	    }
	    ಯದಿ (p == buffer)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		ಯದಿ (len >= 12)
		    ++p;
		ಅನ್ಯಥಾ
		{
		    *buffer_off = len;
		    ಪ್ರತ್ಯಾಗಮ;
		}
	    }
	    c = *p;
	    *p = NUL;
	    ಸಂದೇಶ_puts((ವರ್ಣ *)buffer);
	    ಯದಿ (p < buffer + len)
	    {
		*p = c;
		*buffer_off = (DWORD)((buffer + len) - p);
		mch_memmove(buffer, p, *buffer_off);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    *buffer_off = 0;
	}
	ಅನ್ಯಥಾ
	{
	    buffer[len] = NUL;
	    ಸಂದೇಶ_puts((ವರ್ಣ *)buffer);
	}

	windgoto(ಸಂದೇಶ_row, cmdline_col_off + ಸಂದೇಶ_col);
	cursor_on();
	out_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mch_system_piped(ವರ್ಣ *cmd, ಪೂರ್ಣಾಙ್ಕ options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_hChildStd_IN_Rd = ರಿಕ್ತ;
    HANDLE g_hChildStd_IN_Wr = ರಿಕ್ತ;
    HANDLE g_hChildStd_OUT_Rd = ರಿಕ್ತ;
    HANDLE g_hChildStd_OUT_Wr = ರಿಕ್ತ;

    char_u	buffer[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    ಪೂರ್ಣಾಙ್ಕ		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    ಪೂರ್ಣಾಙ್ಕ		noread_cnt = 0;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		delay = 1;
    DWORD	buffer_off = 0;	// valid bytes in buffer[]
    ವರ್ಣ	*p = ರಿಕ್ತ;

    SECURITY_ATTRIBUTES saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = ಪರಿಮಾಣ(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ರಿಕ್ತ;

    ಯದಿ ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)
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
	ಸಂದೇಶ_puts(_("\nCannot create pipes\n"));
    }

    si.cb = ಪರಿಮಾಣ(si);
    si.lpReserved = ರಿಕ್ತ;
    si.lpDesktop = ರಿಕ್ತ;
    si.lpTitle = ರಿಕ್ತ;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_hChildStd_OUT_Wr;
    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.h_Std_ಒಳಹರಿವು = g_hChildStd_IN_Rd;
    si.wShowWindow = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = ರಿಕ್ತ;

    ಯದಿ (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    ಯದಿ (cmd != ರಿಕ್ತ)
    {
	p = (ವರ್ಣ *)vim_strsave((char_u *)cmd);
	ಯದಿ (p != ರಿಕ್ತ)
	    unescape_shellxquote((char_u *)p, p_sxe);
	ಅನ್ಯಥಾ
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_MODE,
						&si, &pi, ರಿಕ್ತ, ರಿಕ್ತ);

    ಯದಿ (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(g_hChildStd_OUT_Wr);

    ಯದಿ (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(ರಿಕ್ತ,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_hChildStd_IN_Wr,  // parameter
			0,		 // creation flag, start immediately
			ರಿಕ್ತ);		    // we don't care about thread id
	CloseHandle(thread);
	g_hChildStd_IN_Wr = ರಿಕ್ತ;
    }

    // Keep updating the window while waiting for the shell to finish.
    ಪ್ರತಿ (;;)
    {
	ಸಂದೇಶ_2	ಸಂದೇಶ;

	ಯದಿ (PeekMessageW(&ಸಂದೇಶ, (HWND)ರಿಕ್ತ, 0, 0, PM_REMOVE))
	{
	    TranslateMessage(&ಸಂದೇಶ);
	    DispatchMessageW(&ಸಂದೇಶ);
	}

	// write pipe information in the window
	ಯದಿ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.in_use
# endif
	    )
	{
	    len = 0;
	    ಯದಿ (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		ಯದಿ (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		ಯದಿ (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    ಯದಿ (len == 1 && cmd != ರಿಕ್ತ)
		    {
			ಯದಿ (ta_buf[ta_len] == Ctrl_C)
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
		    ಯದಿ ((len == 1 || len == 4 ) && cmd != ರಿಕ್ತ)
		    {
			ಯದಿ (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_hChildStd_IN_Wr);
			    g_hChildStd_IN_Wr = ರಿಕ್ತ;
			    len = 0;
			}
		    }

		    len = term_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    ಪ್ರತಿ (i = ta_len; i < ta_len + len; ++i)
		    {
			ಯದಿ (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    ಸಂದೇಶ_putchar(ta_buf[i]);
			ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
			{
			    ಪೂರ್ಣಾಙ್ಕ l = (*mb_ptr2len)(ta_buf + i);

			    ಸಂದೇಶ_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			ಅನ್ಯಥಾ
			    ಸಂದೇಶ_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(ಸಂದೇಶ_row, cmdline_col_off + ಸಂದೇಶ_col);
		    out_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    ಯದಿ (options & SHELL_WRITE)
			ta_len = 0;
		    ಅನ್ಯಥಾ ಯದಿ (g_hChildStd_IN_Wr != ರಿಕ್ತ)
		    {
			WriteFile(g_hChildStd_IN_Wr, (ವರ್ಣ*)ta_buf,
				    1, &len, ರಿಕ್ತ);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			ಯದಿ (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	ಯದಿ (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	ಯದಿ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);
	    ಭಙ್ಗ;
	}

	++noread_cnt;
	dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	ಯದಿ (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_hChildStd_OUT_Rd);
    ಯದಿ (g_hChildStd_IN_Wr != ರಿಕ್ತ)
	CloseHandle(g_hChildStd_IN_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    GetExitCodeProcess(pi.hProcess, &ret);

    ಯದಿ (options & SHELL_READ)
    {
	ಯದಿ (ga.ga_len > 0)
	{
	    append_ga_ಸಾಲು(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	ಅನ್ಯಥಾ
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mch_system_g(ವರ್ಣ *cmd, ಪೂರ್ಣಾಙ್ಕ options)
{
    // if we can pipe and the shelltemp option is off
    ಯದಿ (!p_stmp)
	ಪ್ರತ್ಯಾಗಮ mch_system_piped(cmd, options);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ mch_system_classic(cmd, options);
}
#endif

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mch_system_c(ವರ್ಣ *cmd, ಪೂರ್ಣಾಙ್ಕ options UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    ಯದಿ (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	ಯದಿ (buf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
	vim_snprintf((ವರ್ಣ *)buf, len, "(%s)", cmd);
	wcmd = enc_to_utf16(buf, ರಿಕ್ತ);
	ಮುಕ್ತ(buf);
    }
    ಅನ್ಯಥಾ
	wcmd = enc_to_utf16((char_u *)cmd, ರಿಕ್ತ);

    ಯದಿ (wcmd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    ಪ್ರತ್ಯಾಗಮ ret;
}

#endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mch_system(ವರ್ಣ *cmd, ಪೂರ್ಣಾಙ್ಕ options)
{
#ifdef VIMDLL
    ಯದಿ (gui.in_use || gui.starting)
	ಪ್ರತ್ಯಾಗಮ mch_system_g(cmd, options);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ mch_system_c(cmd, options);
#ನೋಚೇದ್ಯದಿ defined(FEAT_GUI_MSWIN)
    ಪ್ರತ್ಯಾಗಮ mch_system_g(cmd, options);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ mch_system_c(cmd, options);
#endif
}

#ಯದಿ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mch_call_shell_terminal(
    char_u	*cmd,
    ಪೂರ್ಣಾಙ್ಕ		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = ರಿಕ್ತ;
    typval_T	argvar[2];
    long_u	cmdlen;
    ಪೂರ್ಣಾಙ್ಕ		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    ಯದಿ (cmd == ರಿಕ್ತ)
	cmdlen = STRLEN(p_sh) + 1;
    ಅನ್ಯಥಾ
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    ಯದಿ (newcmd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 255;
    ಯದಿ (cmd == ರಿಕ್ತ)
    {
	STRCPY(newcmd, p_sh);
	ch_log(ರಿಕ್ತ, "starting terminal to run a shell");
    }
    ಅನ್ಯಥಾ
    {
	vim_snprintf((ವರ್ಣ *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(ರಿಕ್ತ, "starting terminal for system command '%s'", cmd);
    }

    init_job_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = term_start(argvar, ರಿಕ್ತ, &opt, TERM_START_SYSTEM);
    ಯದಿ (buf == ರಿಕ್ತ)
    {
	vim_free(newcmd);
	ಪ್ರತ್ಯಾಗಮ 255;
    }

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    ಯದಿ (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	ಯಾವತ್ (term_use_loop())
	{
	    ಯದಿ (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		ಯದಿ (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    ಅನ್ಯಥಾ
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(ರಿಕ್ತ, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    ಪ್ರತ್ಯಾಗಮ retval;
}
#endif
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_env_var(char_u *name, char_u *old_ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ must_free)
{
    ಯದಿ (old_ಮೌಲ್ಯ != ರಿಕ್ತ)
    {
	vim_setenv(name, old_ಮೌಲ್ಯ);
	ಯದಿ (must_free)
	    vim_free(old_ಮೌಲ್ಯ);
	ಪ್ರತ್ಯಾಗಮ;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_call_shell(
    char_u  *cmd,
    ಪೂರ್ಣಾಙ್ಕ	    options)	// SHELL_*, see vim.h
{
    ಪೂರ್ಣಾಙ್ಕ		x = 0;
    ಪೂರ್ಣಾಙ್ಕ		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    ಪೂರ್ಣಾಙ್ಕ		must_free;
    char_u	*oldval;

#ifdef FEAT_EVAL
    ch_log(ರಿಕ್ತ, "executing shell command: %s", cmd);
#endif
    // Change the title to reflect that we are in a subshell.
    ಯದಿ (GetConsoleTitleW(szShellTitle, ARRAY_LENGTH(szShellTitle) - 4) > 0)
    {
	ಯದಿ (cmd == ರಿಕ್ತ)
	    wcscat(szShellTitle, L" :sh");
	ಅನ್ಯಥಾ
	{
	    WCHAR *wn = enc_to_utf16((char_u *)cmd, ರಿಕ್ತ);

	    ಯದಿ (wn != ರಿಕ್ತ)
	    {
		wcscat(szShellTitle, L" - !");
		ಯದಿ ((wcslen(szShellTitle) + wcslen(wn) <
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
    ಯದಿ (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#endif
#ಯದಿ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    ಯದಿ (
# ifdef VIMDLL
	    gui.in_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != ರಿಕ್ತ
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	ಯದಿ (cmdbase != ರಿಕ್ತ)
	    // Skip a leading quote and (.
	    ಯಾವತ್ (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	ಯದಿ (cmdbase == ರಿಕ್ತ || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    ಪ್ರತ್ಯಾಗಮ x;
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

    ಯದಿ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    ಯದಿ (cmd == ರಿಕ್ತ)
    {
	x = mch_system((ವರ್ಣ *)p_sh, options);
    }
    ಅನ್ಯಥಾ
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = ರಿಕ್ತ;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	ಯದಿ (*cmdbase == '"' )
	    ++cmdbase;
	ಯದಿ (*cmdbase == '(')
	    ++cmdbase;

	ಯದಿ ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    ZeroMemory(&si, ಪರಿಮಾಣ(si));
	    si.cb = ಪರಿಮಾಣ(si);
	    si.lpReserved = ರಿಕ್ತ;
	    si.lpDesktop = ರಿಕ್ತ;
	    si.lpTitle = ರಿಕ್ತ;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = ರಿಕ್ತ;

	    cmdbase = skipwhite(cmdbase + 5);
	    ಯದಿ ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_WINDOW;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_ಒಳಹರಿವು = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    ರಿಕ್ತ,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_NORMAL,		// File att.
		    ರಿಕ್ತ);				// Temp file
		si.hStdOutput = si.h_Std_ಒಳಹರಿವು;
		si.hStdError = si.h_Std_ಒಳಹರಿವು;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    ಯದಿ (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		ಯದಿ (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		ಯದಿ (p > cmdbase && p[-1] == ')'
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
	    ಯದಿ (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		ಯದಿ (cmd_shell == ರಿಕ್ತ || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		ಯದಿ (subcmd != ರಿಕ್ತ)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    ಯದಿ (newcmd != ರಿಕ್ತ)
			vim_snprintf((ವರ್ಣ *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    ಅನ್ಯಥಾ
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    ಯದಿ (vim_create_process((ವರ್ಣ *)newcmd, FALSE, flags,
			&si, &pi, ರಿಕ್ತ, ರಿಕ್ತ))
		x = 0;
	    ಅನ್ಯಥಾ ಯದಿ (vim_shell_execute((ವರ್ಣ *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    ಅನ್ಯಥಾ
	    {
		x = -1;
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
		ಯದಿ (gui.in_use)
# endif
		    emsg(_(e_command_not_found));
#endif
	    }

	    ಯದಿ (newcmd != cmdbase)
		vim_free(newcmd);

	    ಯದಿ (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_ಒಳಹರಿವು != ರಿಕ್ತ)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_ಒಳಹರಿವು);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	ಅನ್ಯಥಾ
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
	    ಯದಿ (newcmd != ರಿಕ್ತ)
	    {
#ಯದಿ defined(FEAT_GUI_MSWIN)
		ಯದಿ (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    ವರ್ಣ *ಸಂದೇಶ = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    ವರ್ಣ *title = _("Vim Warning");
		    WCHAR *wmsg = enc_to_utf16((char_u *)ಸಂದೇಶ, ರಿಕ್ತ);
		    WCHAR *wtitle = enc_to_utf16((char_u *)title, ರಿಕ್ತ);

		    ಯದಿ (wmsg != ರಿಕ್ತ && wtitle != ರಿಕ್ತ)
			MessageBoxW(ರಿಕ್ತ, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		ಯದಿ (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((ವರ್ಣ *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_path.string,
			    (ಸಂದೇಶ_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		ಅನ್ಯಥಾ ಯದಿ (
# ifdef VIMDLL
			(gui.in_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((ವರ್ಣ *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		ಅನ್ಯಥಾ
#endif
		    vim_snprintf((ವರ್ಣ *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((ವರ್ಣ *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    ಯದಿ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    ಯದಿ (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#ಯದಿ defined(FEAT_GUI_MSWIN)
	    && ((gui.in_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#endif
	    )
    {
	smsg(_("shell returned %d"), x);
	ಸಂದೇಶ_putchar('\n');
    }
    resettitle();

    mch_signal(SIGINT, SIG_DFL);
    mch_signal(SIGBREAK, SIG_DFL);
    mch_signal(SIGILL, SIG_DFL);
    mch_signal(SIGFPE, SIG_DFL);
    mch_signal(SIGSEGV, SIG_DFL);
    mch_signal(SIGTERM, SIG_DFL);
    mch_signal(SIGABRT, SIG_DFL);

    ಪ್ರತ್ಯಾಗಮ x;
}

#ಯದಿ defined(FEAT_JOB_CHANNEL)
    ಸ್ಥಿರ HANDLE
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

    wn = enc_to_utf16(fname, ರಿಕ್ತ);
    ಯದಿ (wn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ INVALID_HANDLE_ಮೌಲ್ಯ;

    h = CreateFileW(wn, dwDesiredAccess, dwShareMode,
	    lpSecurityAttributes, dwCreationDisposition,
	    dwFlagsAndAttributes, ರಿಕ್ತ);
    vim_free(wn);
    ಪ್ರತ್ಯಾಗಮ h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    ಶೂನ್ಯ
win32_build_env(dict_T *env, garray_T *gap, ಪೂರ್ಣಾಙ್ಕ is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != ರಿಕ್ತ ? env->dv_hashtab.ht_used : 0;
    LPVOID	base = GetEnvironmentStringsW();

    // for last \0
    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (env != ರಿಕ್ತ)
    {
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&env->dv_hashtab, hi, todo)
	{
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		typval_T *item = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_to_utf16((char_u *)hi->hi_ಕೀಲಿ, ರಿಕ್ತ);
		WCHAR   *wval = enc_to_utf16(tv_get_string(item), ರಿಕ್ತ);
		--todo;
		ಯದಿ (wkey != ರಿಕ್ತ && wval != ರಿಕ್ತ)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    ಯದಿ (ga_grow(gap, (ಪೂರ್ಣಾಙ್ಕ)(lkey + lval + 2)) == FAIL)
			ಅನುವರ್ತ;
		    ಪ್ರತಿ (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'=';
		    ಪ್ರತಿ (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    ಯದಿ (base)
    {
	WCHAR	*p = (WCHAR*) base;

	// for last \0
	ಯದಿ (ga_grow(gap, 1) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;

	ಯಾವತ್ (*p != 0 || *(p + 1) != 0)
	{
	    ಯದಿ (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_data + gap->ga_len++) = *p;
	    p++;
	}
	FreeEnvironmentStringsW(base);
	*((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
    }

# ಯದಿ defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
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

	ಯದಿ (ga_grow(gap, (ಪೂರ್ಣಾಙ್ಕ)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    ಪ್ರತಿ (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    ಪ್ರತಿ (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    ಯದಿ (is_terminal)
	    {
		ಪ್ರತಿ (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_data + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		ಪ್ರತಿ (n = 0; n < version_len; n++)
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
    ಸ್ಥಿರ BOOL
create_pipe_pair(HANDLE handles[2])
{
    ಸ್ಥಿರ LONG		s;
    ವರ್ಣ		name[64];
    SECURITY_ATTRIBUTES sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_SIZE, 0, 0, ರಿಕ್ತ);

    ಯದಿ (handles[1] == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    sa.nLength = ಪರಿಮಾಣ(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = ರಿಕ್ತ;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    ಯದಿ (handles[0] == INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	CloseHandle(handles[1]);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

#endif // FEAT_JOB_CHANNEL

#ಯದಿ defined(FEAT_EVAL)
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
    ವರ್ಣ	**argv,
    char_u	*infile,
    ಪೂರ್ಣಾಙ್ಕ		flags UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;
    HANDLE		hChildStdoutRd = INVALID_HANDLE_ಮೌಲ್ಯ;
    HANDLE		hChildStdoutWr = INVALID_HANDLE_ಮೌಲ್ಯ;
    HANDLE		hChildStdinRd = INVALID_HANDLE_ಮೌಲ್ಯ;
    garray_T		cmd_ga;
    garray_T		out_ga;
    char_u		*buffer = ರಿಕ್ತ;
    DWORD		exit_code = (DWORD)-1;
    ಪೂರ್ಣಾಙ್ಕ			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    ಪ್ರತಿ (i = 0; argv[i] != ರಿಕ್ತ; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	ಪೂರ್ಣಾಙ್ಕ	has_spaces = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	j;

	ಪ್ರತಿ (j = 0; s[j] != NUL; j++)
	    ಯದಿ (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		ಭಙ್ಗ;
	    }

	ಯದಿ (i > 0)
	    ga_append(&cmd_ga, ' ');

	ಯದಿ (has_spaces)
	{
	    ಪೂರ್ಣಾಙ್ಕ	ಅಂಕೆ_bs;

	    ga_append(&cmd_ga, '"');
	    ಪ್ರತಿ (j = 0; arg[j] != NUL; j++)
	    {
		ಅಂಕೆ_bs = 0;
		ಯಾವತ್ (arg[j] == '\\')
		{
		    ಅಂಕೆ_bs++;
		    j++;
		}

		ಯದಿ (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    ಯಾವತ್ (ಅಂಕೆ_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ ಯದಿ (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    ಯಾವತ್ (ಅಂಕೆ_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		ಅನ್ಯಥಾ
		{
		    ಯಾವತ್ (ಅಂಕೆ_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	ಅನ್ಯಥಾ
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&out_ga, 1, 4096);

    saAttr.nLength = ಪರಿಮಾಣ(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ರಿಕ್ತ;

    // Create a pipe for the child's stdout.
    ಯದಿ (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)
	    || !SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	ಗಚ್ಛ done;
    }

    // Set up stdin from infile if provided.
    ಯದಿ (infile != ರಿಕ್ತ)
    {
	WCHAR *winfile = enc_to_utf16(infile, ರಿಕ್ತ);

	ಯದಿ (winfile != ರಿಕ್ತ)
	{
	    hChildStdinRd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL, ರಿಕ್ತ);
	    vim_free(winfile);
	}
    }

    ZeroMemory(&pi, ಪರಿಮಾಣ(pi));
    ZeroMemory(&si, ಪರಿಮಾಣ(si));
    si.cb = ಪರಿಮಾಣ(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;
    si.h_Std_ಒಳಹರಿವು = (hChildStdinRd != INVALID_HANDLE_ಮೌಲ್ಯ)
		    ? hChildStdinRd : INVALID_HANDLE_ಮೌಲ್ಯ;

    ch_log(ರಿಕ್ತ, "directly executing: %s", (ವರ್ಣ *)cmd_ga.ga_data);

    // Create the child process directly, without going through the shell.
    ಯದಿ (!vim_create_process((ವರ್ಣ *)cmd_ga.ga_data, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
		&si, &pi, ರಿಕ್ತ, ರಿಕ್ತ))
    {
	semsg(_(e_invalid_argument_str), cmd_ga.ga_data);
	ಗಚ್ಛ done;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(hChildStdoutWr);
    hChildStdoutWr = INVALID_HANDLE_ಮೌಲ್ಯ;
    ಯದಿ (hChildStdinRd != INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	CloseHandle(hChildStdinRd);
	hChildStdinRd = INVALID_HANDLE_ಮೌಲ್ಯ;
    }

    // Read output from child process.
    ಪ್ರತಿ (;;)
    {
	ವರ್ಣ	buf[4096];
	DWORD	n;

	ಯದಿ (!ReadFile(hChildStdoutRd, buf, ಪರಿಮಾಣ(buf), &n, ರಿಕ್ತ) || n == 0)
	    ಭಙ್ಗ;
	ಯದಿ (ga_grow(&out_ga, (ಪೂರ್ಣಾಙ್ಕ)n) == OK)
	{
	    mch_memmove((ವರ್ಣ *)out_ga.ga_data + out_ga.ga_len, buf, n);
	    out_ga.ga_len += (ಪೂರ್ಣಾಙ್ಕ)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    set_vim_var_nr(VV_SHELL_ERROR, (long)exit_code);

    ಯದಿ (out_ga.ga_len > 0)
    {
	buffer = alloc(out_ga.ga_len + 1);
	ಯದಿ (buffer != ರಿಕ್ತ)
	{
	    mch_memmove(buffer, out_ga.ga_data, out_ga.ga_len);
	    ಯದಿ (ret_len == ರಿಕ್ತ)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		ಪ್ರತಿ (i = 0; i < out_ga.ga_len; ++i)
		    ಯದಿ (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[out_ga.ga_len] = NUL;
	    }
	    ಅನ್ಯಥಾ
		*ret_len = out_ga.ga_len;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (ret_len != ರಿಕ್ತ)
	*ret_len = 0;

done:
    ga_clear(&cmd_ga);
    ga_clear(&out_ga);
    ಯದಿ (hChildStdoutRd != INVALID_HANDLE_ಮೌಲ್ಯ)
	CloseHandle(hChildStdoutRd);
    ಯದಿ (hChildStdoutWr != INVALID_HANDLE_ಮೌಲ್ಯ)
	CloseHandle(hChildStdoutWr);
    ಯದಿ (hChildStdinRd != INVALID_HANDLE_ಮೌಲ್ಯ)
	CloseHandle(hChildStdinRd);
    ಪ್ರತ್ಯಾಗಮ buffer;
}
#endif // FEAT_EVAL

#ಯದಿ defined(FEAT_JOB_CHANNEL)
    ಶೂನ್ಯ
mch_job_start(ವರ್ಣ *cmd, job_T *job, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_ATTRIBUTES saAttr;
    channel_T		*channel = ರಿಕ್ತ;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    ಪೂರ್ಣಾಙ್ಕ		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    ಪೂರ್ಣಾಙ್ಕ		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    ಪೂರ್ಣಾಙ್ಕ		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    ಪೂರ್ಣಾಙ್ಕ		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    ಪೂರ್ಣಾಙ್ಕ		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    ಪೂರ್ಣಾಙ್ಕ		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    ಪೂರ್ಣಾಙ್ಕ		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;

    ಯದಿ (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_ಮೌಲ್ಯ;
    ifd[1] = INVALID_HANDLE_ಮೌಲ್ಯ;
    ofd[0] = INVALID_HANDLE_ಮೌಲ್ಯ;
    ofd[1] = INVALID_HANDLE_ಮೌಲ್ಯ;
    efd[0] = INVALID_HANDLE_ಮೌಲ್ಯ;
    efd[1] = INVALID_HANDLE_ಮೌಲ್ಯ;
    ga_init2(&ga, ಪರಿಮಾಣ(wchar_t), 500);

    jo = CreateJobObject(ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (jo == ರಿಕ್ತ)
    {
	job->jv_status = JOB_FAILED;
	ಗಚ್ಛ failed;
    }

    ಯದಿ (options->jo_env != ರಿಕ್ತ)
	win32_build_env(options->jo_env, &ga, FALSE);

    ZeroMemory(&pi, ಪರಿಮಾಣ(pi));
    ZeroMemory(&si, ಪರಿಮಾಣ(si));
    si.cb = ಪರಿಮಾಣ(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    saAttr.nLength = ಪರಿಮಾಣ(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ರಿಕ್ತ;

    ಯದಿ (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	ifd[0] = job_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
	ಯದಿ (ifd[0] == INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಗಚ್ಛ failed;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (!use_null_for_in
	    && (!create_pipe_pair(ifd)
		|| !SetHandleInformation(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	ಗಚ್ಛ failed;

    ಯದಿ (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	ofd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	ಯದಿ (ofd[1] == INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಗಚ್ಛ failed;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (!use_null_for_out &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !SetHandleInformation(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	ಗಚ್ಛ failed;

    ಯದಿ (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	ಯದಿ (efd[1] == INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಗಚ್ಛ failed;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (!use_out_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !SetHandleInformation(efd[0], HANDLE_FLAG_INHERIT, 0)))
	ಗಚ್ಛ failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_ಒಳಹರಿವು = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_out_for_err ? ofd[1] : efd[1];

    ಯದಿ (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	ಯದಿ (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    ಯದಿ (channel != ರಿಕ್ತ)
		++channel->ch_refcount;
	}
	ಅನ್ಯಥಾ
	    channel = add_channel();
	ಯದಿ (channel == ರಿಕ್ತ)
	    ಗಚ್ಛ failed;
    }

    ಯದಿ (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_MODE |
	    CREATE_NEW_PROCESS_GROUP |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_data,
	    (ವರ್ಣ *)options->jo_cwd))
    {
	CloseHandle(jo);
	job->jv_status = JOB_FAILED;
	ಗಚ್ಛ failed;
    }

    ga_clear(&ga);

    ಯದಿ (!AssignProcessToJobObject(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = ರಿಕ್ತ;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    job->jv_proc_info = pi;
    job->jv_job_object = jo;
    job->jv_status = JOB_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    ಯದಿ (!use_out_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    job->jv_channel = channel;
    ಯದಿ (channel != ರಿಕ್ತ)
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
    ಪ್ರತ್ಯಾಗಮ;

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

    ವರ್ಣ *
mch_job_status(job_T *job)
{
    DWORD dwExitCode = 0;

    ಯದಿ (!GetExitCodeProcess(job->jv_proc_info.hProcess, &dwExitCode)
	    || dwExitCode != STILL_ACTIVE)
    {
	job->jv_exitval = (ಪೂರ್ಣಾಙ್ಕ)dwExitCode;
	ಯದಿ (job->jv_status < JOB_ENDED)
	{
	    ch_log(job->jv_channel, "Job ended");
	    job->jv_status = JOB_ENDED;
	}
	ಪ್ರತ್ಯಾಗಮ "dead";
    }
    ಪ್ರತ್ಯಾಗಮ "run";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    HANDLE jobHandles[MAXIMUM_WAIT_OBJECTS];
    job_T *jobArray[MAXIMUM_WAIT_OBJECTS];
    job_T *job = job_list;

    ಯಾವತ್ (job != ರಿಕ್ತ)
    {
	DWORD n;
	DWORD result;

	ಪ್ರತಿ (n = 0; n < MAXIMUM_WAIT_OBJECTS
				       && job != ರಿಕ್ತ; job = job->jv_next)
	{
	    ಯದಿ (job->jv_status == JOB_STARTED)
	    {
		jobHandles[n] = job->jv_proc_info.hProcess;
		jobArray[n] = job;
		++n;
	    }
	}
	ಯದಿ (n == 0)
	    ಅನುವರ್ತ;
	result = WaitForMultipleObjects(n, jobHandles, FALSE, 0);
	ಯದಿ (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + n)
	{
	    job_T *wait_job = jobArray[result - WAIT_OBJECT_0];

	    ಯದಿ (STRCMP(mch_job_status(wait_job), "dead") == 0)
		ಪ್ರತ್ಯಾಗಮ wait_job;
	}
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ BOOL
terminate_all(HANDLE process, ಪೂರ್ಣಾಙ್ಕ code)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_ಮೌಲ್ಯ;
    DWORD	    pid = GetProcessId(process);

    ಯದಿ (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	ಯದಿ (h != INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    pe.dwSize = ಪರಿಮಾಣ(PROCESSENTRY32);
	    ಯದಿ (!Process32First(h, &pe))
		ಗಚ್ಛ theend;

	    ಕುರು
	    {
		ಯದಿ (pe.th32ParentProcessID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    ಯದಿ (ph != ರಿಕ್ತ)
		    {
			terminate_all(ph, code);
			CloseHandle(ph);
		    }
		}
	    } ಯಾವತ್ (Process32Next(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    ಪ್ರತ್ಯಾಗಮ TerminateProcess(process, code);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_signal_job(job_T *job, char_u *how)
{
    ಪೂರ್ಣಾಙ್ಕ ret;

    ಯದಿ (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	ಯದಿ (job->jv_job_object != ರಿಕ್ತ)
	{
	    ಯದಿ (job->jv_channel != ರಿಕ್ತ && job->jv_channel->ch_anonymous_pipe)
		job->jv_channel->ch_killing = TRUE;
	    ಪ್ರತ್ಯಾಗಮ TerminateJobObject(job->jv_job_object, (UINT)-1) ? OK : FAIL;
	}
	ಪ್ರತ್ಯಾಗಮ terminate_all(job->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    ಯದಿ (!AttachConsole(job->jv_proc_info.dwProcessId))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		job->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Clear the data related to "job".
 */
    ಶೂನ್ಯ
mch_clear_job(job_T *job)
{
    ಯದಿ (job->jv_status == JOB_FAILED)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (job->jv_job_object != ರಿಕ್ತ)
	CloseHandle(job->jv_job_object);
    CloseHandle(job->jv_proc_info.hProcess);
}
#endif


#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
termcap_mode_start(ಶೂನ್ಯ)
{
    DWORD cmodein;

    ಯದಿ (g_fTermcapMode)
	ಪ್ರತ್ಯಾಗಮ;

    SaveConsoleBuffer(&g_cbNonTermcap);

    ಯದಿ (g_cbTermcap.IsValid)
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
    ಅನ್ಯಥಾ
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
    ಯದಿ (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_ಒಳಹರಿವು;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    ಅನ್ಯಥಾ
    {
	cmodein &= ~ENABLE_MOUSE_ಒಳಹರಿವು;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }
    cmodein |= ENABLE_WINDOW_ಒಳಹರಿವು;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_fTermcapMode = TRUE;
}


/*
 * End termcap mode
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
termcap_mode_end(ಶೂನ್ಯ)
{
    DWORD cmodein;
    ConsoleBuffer *cb;
    COORD coord;
    DWORD dwDummy;

    ಯದಿ (!g_fTermcapMode)
	ಪ್ರತ್ಯಾಗಮ;

    SaveConsoleBuffer(&g_cbTermcap);

    GetConsoleMode(g_hConIn, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_ಒಳಹರಿವು | ENABLE_WINDOW_ಒಳಹರಿವು);
    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# ಅನ್ಯಥಾ
    cb = &g_cbNonTermcap;
# endif
    RestoreConsoleBuffer(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    ಯದಿ (exiting && use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    ಯದಿ (!USE_WT && (p_rs || exiting))
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
	ಯದಿ (exiting && !p_rs)
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


#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_chars(
    COORD coord,
    DWORD n)
{
    ಯದಿ (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_hConOut, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    ಅನ್ಯಥಾ
    {
	set_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_screen(ಶೂನ್ಯ)
{
    g_coord.X = g_coord.Y = 0;

    ಯದಿ (!vtp_working)
	clear_chars(g_coord, Rows * Columns);
    ಅನ್ಯಥಾ
    {
	set_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_to_end_of_display(ಶೂನ್ಯ)
{
    COORD save = g_coord;

    ಯದಿ (!vtp_working)
	clear_chars(g_coord, (Rows - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_to_end_of_ಸಾಲು(ಶೂನ್ಯ)
{
    COORD save = g_coord;

    ಯದಿ (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
scroll(unsigned c_ಸಾಲುಗಳು)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.ಎಡ_3 + 1, g_srScrollRegion.ತುದಿ_2 + 1);
    delete_ಸಾಲುಗಳು(c_ಸಾಲುಗಳು);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_scroll_region(
    unsigned ಎಡ,
    unsigned ತುದಿ,
    unsigned ಬಲ,
    unsigned bottom)
{
    ಯದಿ (ಎಡ >= ಬಲ
	    || ತುದಿ >= bottom
	    || ಬಲ > (unsigned) Columns - 1
	    || bottom > (unsigned) Rows - 1)
	ಪ್ರತ್ಯಾಗಮ;

    g_srScrollRegion.ಎಡ_3 =   ಎಡ;
    g_srScrollRegion.ತುದಿ_2 =    ತುದಿ;
    g_srScrollRegion.ಬಲ_3 =  ಬಲ;
    g_srScrollRegion.Bottom = bottom;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_scroll_region_tb(
    unsigned ತುದಿ,
    unsigned bottom)
{
    ಯದಿ (ತುದಿ >= bottom || bottom > (unsigned)Rows - 1)
	ಪ್ರತ್ಯಾಗಮ;

    g_srScrollRegion.ತುದಿ_2 = ತುದಿ;
    g_srScrollRegion.Bottom = bottom;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_scroll_region_lr(
    unsigned ಎಡ,
    unsigned ಬಲ)
{
    ಯದಿ (ಎಡ >= ಬಲ || ಬಲ > (unsigned)Columns - 1)
	ಪ್ರತ್ಯಾಗಮ;

    g_srScrollRegion.ಎಡ_3 = ಎಡ;
    g_srScrollRegion.ಬಲ_3 = ಬಲ;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೇರಿಸು_ಸಾಲುಗಳು(unsigned c_ಸಾಲುಗಳು)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.ಎಡ_3 + 1, g_srScrollRegion.ತುದಿ_2 + 1);

    dest.X = g_srScrollRegion.ಎಡ_3;
    dest.Y = g_coord.Y + c_ಸಾಲುಗಳು;

    source.ಎಡ_3   = g_srScrollRegion.ಎಡ_3;
    source.ತುದಿ_2	  = g_coord.Y;
    source.ಬಲ_3  = g_srScrollRegion.ಬಲ_3;
    source.Bottom = g_srScrollRegion.Bottom - c_ಸಾಲುಗಳು;

    clip.ಎಡ_3   = g_srScrollRegion.ಎಡ_3;
    clip.ತುದಿ_2    = g_coord.Y;
    clip.ಬಲ_3  = g_srScrollRegion.ಬಲ_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    ಯದಿ (!USE_VTP)
	fill.Attributes = g_attrCurrent;
    ಅನ್ಯಥಾ
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    ಯದಿ (source.Bottom < dest.Y)
    {
	COORD coord;
	ಪೂರ್ಣಾಙ್ಕ   i;

	coord.X = source.ಎಡ_3;
	ಪ್ರತಿ (i = clip.ತುದಿ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ಬಲ_3 - source.ಎಡ_3 + 1);
	}
    }

    ಯದಿ (vtp_working)
    {
	COORD coord;
	ಪೂರ್ಣಾಙ್ಕ i;

	coord.X = source.ಎಡ_3;
	ಪ್ರತಿ (i = source.ತುದಿ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ಬಲ_3 - source.ಎಡ_3 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
delete_ಸಾಲುಗಳು(unsigned c_ಸಾಲುಗಳು)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    ಪೂರ್ಣಾಙ್ಕ		    nb;

    gotoxy(g_srScrollRegion.ಎಡ_3 + 1, g_srScrollRegion.ತುದಿ_2 + 1);

    dest.X = g_srScrollRegion.ಎಡ_3;
    dest.Y = g_coord.Y;

    source.ಎಡ_3   = g_srScrollRegion.ಎಡ_3;
    source.ತುದಿ_2	  = g_coord.Y + c_ಸಾಲುಗಳು;
    source.ಬಲ_3  = g_srScrollRegion.ಬಲ_3;
    source.Bottom = g_srScrollRegion.Bottom;

    clip.ಎಡ_3   = g_srScrollRegion.ಎಡ_3;
    clip.ತುದಿ_2    = g_coord.Y;
    clip.ಬಲ_3  = g_srScrollRegion.ಬಲ_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    ಯದಿ (!vtp_working)
	fill.Attributes = g_attrCurrent;
    ಅನ್ಯಥಾ
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (source.Bottom - source.ತುದಿ_2) + 1;

    ಯದಿ (nb < source.ತುದಿ_2)
    {
	COORD coord;
	ಪೂರ್ಣಾಙ್ಕ   i;

	coord.X = source.ಎಡ_3;
	ಪ್ರತಿ (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ಬಲ_3 - source.ಎಡ_3 + 1);
	}
    }

    ಯದಿ (vtp_working)
    {
	COORD coord;
	ಪೂರ್ಣಾಙ್ಕ i;

	coord.X = source.ಎಡ_3;
	ಪ್ರತಿ (i = nb; i <= source.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ಬಲ_3 - source.ಎಡ_3 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gotoxy(
    unsigned x,
    unsigned y)
{
    ಯದಿ (x < 1 || x > (unsigned)Columns || y < 1 || y > (unsigned)Rows)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!USE_VTP)
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
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
textattr(WORD wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    SetConsoleTextAttribute(g_hConOut, wAttr);
}


    ಸ್ಥಿರ ಶೂನ್ಯ
textcolor(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    ಯದಿ (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    ಅನ್ಯಥಾ
	vtp_sgr_bulk(wAttr);
}


    ಸ್ಥಿರ ಶೂನ್ಯ
textbackground(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    ಯದಿ (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    ಅನ್ಯಥಾ
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
normvideo(ಶೂನ್ಯ)
{
    ಯದಿ (!vtp_working)
	textattr(g_attrDefault);
    ಅನ್ಯಥಾ
	vtp_sgr_bulk(0);
}


ಸ್ಥಿರ WORD g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
standout(ಶೂನ್ಯ)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((WORD) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
standend(ಶೂನ್ಯ)
{
    ಯದಿ (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    ಶೂನ್ಯ
mch_set_normal_colors(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		n;

    cterm_normal_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_normal_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    ಯದಿ (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	ಯದಿ (*p == 'm' && n > 0)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
visual_bell(ಶೂನ್ಯ)
{
    COORD   coordOrigin = {0, 0};
    WORD    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = ರಿಕ್ತ;

# ifdef FEAT_TERMGUICOLORS
    ಯದಿ (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(WORD, Rows * Columns);
	ಯದಿ (oldattrs == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ReadConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_hConOut, attrFlash, Rows * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    ಯದಿ (oldattrs != ರಿಕ್ತ)
    {
	WriteConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    ಯದಿ (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	ಪ್ರತ್ಯಾಗಮ;
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
    ಸ್ಥಿರ DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cbToWrite)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    ಸ್ಥಿರ DWORD    cells;
    ಸ್ಥಿರ WCHAR    *unicodebuf = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    unibuflen = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    length;
    ಪೂರ್ಣಾಙ್ಕ		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    ಸ್ಥಿರ WCHAR    *utf8spbuf = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    utf8splength;
    ಸ್ಥಿರ DWORD    utf8spcells;
    ಸ್ಥಿರ WCHAR    **utf8usingbuf = &unicodebuf;

    ಯದಿ (cbToWrite != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	ಕುರು
	{
	    length = MultiByteToWideChar(cp, 0, (LPCSTR)pchBuf, cbToWrite,
							unicodebuf, unibuflen);
	    ಯದಿ (length && length <= unibuflen)
		ಭಙ್ಗ;
	    vim_free(unicodebuf);
	    unicodebuf = length ? LALLOC_MULT(WCHAR, length) : ರಿಕ್ತ;
	    unibuflen = unibuflen ? 0 : length;
	} ಯಾವತ್ (TRUE);
	cells = mb_string2cells(pchBuf, cbToWrite);
    }
    ಅನ್ಯಥಾ // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	ಯದಿ (utf8usingbuf != &utf8spbuf)
	{
	    ಯದಿ (utf8spbuf == ರಿಕ್ತ)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		length = MultiByteToWideChar(CP_UTF8, 0, " ", 1, ರಿಕ್ತ, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, length);
		ಯದಿ (utf8spbuf != ರಿಕ್ತ)
		{
		    MultiByteToWideChar(CP_UTF8, 0, " ", 1, utf8spbuf, length);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = length;
		    utf8spcells = cells;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		utf8usingbuf = &utf8spbuf;
		length = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    ಯದಿ (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	ಯದಿ (WriteConsoleOutputCharacterW(g_hConOut, *utf8usingbuf, length,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (WriteConsoleW(g_hConOut, *utf8usingbuf, length, &cchwritten,
		    ರಿಕ್ತ) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    ಯದಿ (cchwritten == (DWORD)length)
    {
	written = cbToWrite;
	g_coord.X += (SHORT)cells;
    }
    ಅನ್ಯಥಾ
    {
	char_u *p = pchBuf;
	ಪ್ರತಿ (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    ಯಾವತ್ (g_coord.X > g_srScrollRegion.ಬಲ_3)
    {
	g_coord.X -= (SHORT) Columns;
	ಯದಿ (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    ಯದಿ (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    ಪ್ರತ್ಯಾಗಮ written;
}

    ಸ್ಥಿರ char_u *
get_seq(
    ಪೂರ್ಣಾಙ್ಕ *args,
    ಪೂರ್ಣಾಙ್ಕ *ಎಣಿಕೆ,
    char_u *head)
{
    ಪೂರ್ಣಾಙ್ಕ argc;
    char_u *p;

    ಯದಿ (head == ರಿಕ್ತ || *head != '\033')
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    argc = 0;
    p = head;
    ++p;
    ಕುರು
    {
	++p;
	args[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } ಯಾವತ್ (*p == ';');
    *ಎಣಿಕೆ = argc;

    ಪ್ರತ್ಯಾಗಮ p;
}

    ಸ್ಥಿರ char_u *
get_sgr(
    ಪೂರ್ಣಾಙ್ಕ *args,
    ಪೂರ್ಣಾಙ್ಕ *ಎಣಿಕೆ,
    char_u *head)
{
    char_u *p = get_seq(args, ಎಣಿಕೆ, head);

    ಪ್ರತ್ಯಾಗಮ (p && *p == 'm') ? ++p : ರಿಕ್ತ;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    ಸ್ಥಿರ char_u *
sgrn2(
    char_u *head,
    ಪೂರ್ಣಾಙ್ಕ	   n)
{
    ಪೂರ್ಣಾಙ್ಕ argc;
    ಪೂರ್ಣಾಙ್ಕ args[16];
    char_u *p = get_sgr(args, &argc, head);

    ಪ್ರತ್ಯಾಗಮ p && argc == 5 && args[0] == n && args[1] == 2 ? p : ರಿಕ್ತ;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    ಸ್ಥಿರ char_u *
sgrnc(
    char_u *head,
    ಪೂರ್ಣಾಙ್ಕ	   n)
{
    ಪೂರ್ಣಾಙ್ಕ argc;
    ಪೂರ್ಣಾಙ್ಕ args[16];
    char_u *p = get_sgr(args, &argc, head);

    ಪ್ರತ್ಯಾಗಮ p && argc == 1 && args[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : ರಿಕ್ತ;
}

    ಸ್ಥಿರ char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    ಯಾವತ್ (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    ಸ್ಥಿರ char_u *
sgrn2c(
    char_u *head,
    ಪೂರ್ಣಾಙ್ಕ	   n)
{
    char_u *p = sgrn2(head, n);

    ಪ್ರತ್ಯಾಗಮ p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : ರಿಕ್ತ;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    ಸ್ಥಿರ char_u *
sgrn2cn(
    char_u *head,
    ಪೂರ್ಣಾಙ್ಕ n)
{
    char_u *p = sgrn2(head, n);

    ಪ್ರತ್ಯಾಗಮ p && p[0] == 0x0a && p[1] == '\033' ? ++p : ರಿಕ್ತ;
}

ಸ್ಥಿರ bool in_dcs = false;	// inside a DCS (sixel) sequence
ಸ್ಥಿರ bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
write_vt(char_u *s, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ರಿಕ್ತ, 0);
    ಯದಿ (wlen <= 0)
	ಪ್ರತ್ಯಾಗಮ;
    ws = ALLOC_MULT(WCHAR, wlen);
    ಯದಿ (ws == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_hConOut, ws, wlen, &written, ರಿಕ್ತ);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    ಸ್ಥಿರ char_u *
skip_dcs(char_u *s, char_u *end)
{
    ಪ್ರತಿ (char_u *p = s; p < end; ++p)
    {
	ಯದಿ (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    in_dcs = false;
	    ಪ್ರತ್ಯಾಗಮ p + 1;
	}
	dcs_esc = *p == ESC;
    }
    ಪ್ರತ್ಯಾಗಮ end;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    ಶೂನ್ಯ
mch_write(
    char_u  *s,
    ಪೂರ್ಣಾಙ್ಕ	    len)
{
    char_u  *end = s + len;

# ifdef VIMDLL
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;
# endif

    ಯದಿ (!term_console)
    {
	ಲಿಖ(1, s, (unsigned)len);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // translate ESC | sequences into faked bios calls
    ಯಾವತ್ (len--)
    {
	ಪೂರ್ಣಾಙ್ಕ prefix = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	ಯದಿ (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (in_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    ಪೂರ್ಣಾಙ್ಕ	l = (ಪೂರ್ಣಾಙ್ಕ)(skip_dcs(s, end) - s);

	    ಯದಿ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    ಅನುವರ್ತ;
	}

	ಯಾವತ್ (s + ++prefix < end)
	{
	    ch = s[prefix];
	    ಯದಿ (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		ಭಙ್ಗ;
	}

	ಯದಿ (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    ಯದಿ (prefix != 0)
		prefix = 1;
	}

	ಯದಿ (prefix != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, prefix);
# ifdef MCH_WRITE_DUMP
	    ಯದಿ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, ಪರಿಮಾಣ(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	ಅನ್ಯಥಾ ಯದಿ (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    ಯದಿ (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.ಎಡ_3 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    ಅನ್ಯಥಾ
	    {
		gotoxy(g_srScrollRegion.ಎಡ_3 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    ಯದಿ (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	ಅನ್ಯಥಾ ಯದಿ (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.ಎಡ_3+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ಯದಿ (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	ಅನ್ಯಥಾ ಯದಿ (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    ಯದಿ (g_coord.X > g_srScrollRegion.ಎಡ_3)
		g_coord.X--;
	    ಅನ್ಯಥಾ ಯದಿ (g_coord.Y > g_srScrollRegion.ತುದಿ_2)
	    {
		g_coord.X = g_srScrollRegion.ಬಲ_3;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ಯದಿ (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	ಅನ್ಯಥಾ ಯದಿ (s[0] == '\a')
	{
	    // \a, bell
	    MessageBeep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    ಯದಿ (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	ಅನ್ಯಥಾ ಯದಿ (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    ಪೂರ್ಣಾಙ್ಕ	    arg1 = 0, arg2 = 0, argc = 0, args[16];
	    char_u  *sp;

	    ವಿಕಲ್ಪ (s[2])
	    {
	    ಪಕ್ಷ '0': ಪಕ್ಷ '1': ಪಕ್ಷ '2': ಪಕ್ಷ '3': ಪಕ್ಷ '4':
	    ಪಕ್ಷ '5': ಪಕ್ಷ '6': ಪಕ್ಷ '7': ಪಕ್ಷ '8': ಪಕ್ಷ '9':
		ಯದಿ (*(p = get_seq(args, &argc, s)) != 'm')
		    ಗಚ್ಛ notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		ಯದಿ (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (ಪೂರ್ಣಾಙ್ಕ)(p - s);
		    s = p;
		    ಭಙ್ಗ;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		ಯದಿ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		ಯದಿ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		ಯದಿ (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		ಯದಿ (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(args, &argc, p);

notsgr:
		arg1 = args[0];
		arg2 = args[1];
		ಯದಿ (*p == 'm')
		{
		    ಯದಿ (argc == 1 && args[0] == 0)
			normvideo();
		    ಅನ್ಯಥಾ ಯದಿ (argc == 1)
		    {
			ಯದಿ (USE_VTP)
			    textcolor((WORD)arg1);
			ಅನ್ಯಥಾ
			    textattr((WORD)arg1);
		    }
		    ಅನ್ಯಥಾ ಯದಿ (vtp_working)
			vtp_sgr_bulks(argc, args);
		}
		ಅನ್ಯಥಾ ಯದಿ (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		ಅನ್ಯಥಾ ಯದಿ (argc == 2 && *p == 'r')
		{
		    set_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		ಅನ್ಯಥಾ ಯದಿ (argc == 2 && *p == 'R')
		{
		    set_scroll_region_tb(arg1, arg2);
		}
		ಅನ್ಯಥಾ ಯದಿ (argc == 2 && *p == 'V')
		{
		    set_scroll_region_lr(arg1, arg2);
		}
		ಅನ್ಯಥಾ ಯದಿ (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.ತುದಿ_2, g_coord.Y - arg1) + 1);
		}
		ಅನ್ಯಥಾ ಯದಿ (argc == 1 && *p == 'b')
		{
		    textbackground((WORD) arg1);
		}
		ಅನ್ಯಥಾ ಯದಿ (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.ಬಲ_3, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		ಅನ್ಯಥಾ ಯದಿ (argc == 1 && *p == 'f')
		{
		    textcolor((WORD) arg1);
		}
		ಅನ್ಯಥಾ ಯದಿ (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		ಅನ್ಯಥಾ ಯದಿ (argc == 1 && *p == 'L')
		{
		    ಸೇರಿಸು_ಸಾಲುಗಳು(arg1);
		}
		ಅನ್ಯಥಾ ಯದಿ (argc == 1 && *p == 'M')
		{
		    delete_ಸಾಲುಗಳು(arg1);
		}

		len -= (ಪೂರ್ಣಾಙ್ಕ)(p - s);
		s = p + 1;
		ಭಙ್ಗ;

	    ಪಕ್ಷ 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.ತುದಿ_2, g_coord.Y - 1) + 1);
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'B':
		visual_bell();
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'C':
		gotoxy(min(g_srScrollRegion.ಬಲ_3, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'E':
		termcap_mode_end();
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'F':
		standout();
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'f':
		standend();
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'H':
		gotoxy(1, 1);
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'j':
		clear_to_end_of_display();
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'J':
		clear_screen();
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'K':
		clear_to_end_of_ಸಾಲು();
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'L':
		ಸೇರಿಸು_ಸಾಲುಗಳು(1);
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'M':
		delete_ಸಾಲುಗಳು(1);
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'S':
		termcap_mode_start();
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'V':
		cursor_visible(TRUE);
		ಗಚ್ಛ got3;

	    ಪಕ್ಷ 'v':
		cursor_visible(FALSE);
		ಗಚ್ಛ got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    ಯದಿ (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, ಪರಿಮಾಣ(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	ಅನ್ಯಥಾ ಯದಿ (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    ಪೂರ್ಣಾಙ್ಕ l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    ಯಾವತ್ (s + l < end && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    ಯದಿ (s + l < end && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		ಯದಿ (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	ಅನ್ಯಥಾ ಯದಿ (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    ಪೂರ್ಣಾಙ್ಕ	l;

	    in_dcs = true;
	    dcs_esc = false;
	    l = (ಪೂರ್ಣಾಙ್ಕ)(skip_dcs(s + 2, end) - s);
	    ಯದಿ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	ಅನ್ಯಥಾ
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    ಯದಿ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, ಪರಿಮಾಣ(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    ಯದಿ (fdDump)
	fflush(fdDump);
# endif
}

#ಅನ್ಯಥಾ // FEAT_GUI_MSWIN && !VIMDLL
    ಶೂನ್ಯ
mch_write(
    char_u  *s UNUSED,
    ಪೂರ್ಣಾಙ್ಕ	    len UNUSED)
{
    // never used
}

#endif


/*
 * Delay for "msec" milliseconds.
 */
    ಶೂನ್ಯ
mch_delay(
    long    msec,
    ಪೂರ್ಣಾಙ್ಕ	    flags UNUSED)
{
#ಯದಿ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((ಪೂರ್ಣಾಙ್ಕ)msec);	    // never wait for input
#ಅನ್ಯಥಾ // Console
# ifdef VIMDLL
    ಯದಿ (gui.in_use)
    {
	Sleep((ಪೂರ್ಣಾಙ್ಕ)msec);	    // never wait for input
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif
    ಯದಿ (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	ಯದಿ (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    ಪೂರ್ಣಾಙ್ಕ towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    ಯಾವತ್ (msec > 0)
	    {
		mzvim_check_threads();
		ಯದಿ (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	ಅನ್ಯಥಾ
# endif
	    Sleep((ಪೂರ್ಣಾಙ್ಕ)msec);
    ಅನ್ಯಥಾ
	WaitForChar(msec, FALSE);
#endif
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_remove(char_u *name)
{
    WCHAR	*wn;
    ಪೂರ್ಣಾಙ್ಕ		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    ಯದಿ (mch_isdir(name) && mch_is_symbolic_link(name))
	ಪ್ರತ್ಯಾಗಮ mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_NORMAL);

    wn = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (wn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    ಪ್ರತ್ಯಾಗಮ n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    ಶೂನ್ಯ
mch_breakcheck(ಪೂರ್ಣಾಙ್ಕ force UNUSED)
{
#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    ಯದಿ (!gui.in_use)
# endif
	ಯದಿ (g_fCtrlCPressed || g_fCBrkPressed)
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
mch_total_mem(ಪೂರ್ಣಾಙ್ಕ special UNUSED)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dwLength = ಪರಿಮಾಣ(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&ms);
    ಯದಿ (ms.ullAvailVirtual < ms.ullTotalPhys)
    {
	// Process address space fits in physical RAM, use all of it.
	ಪ್ರತ್ಯಾಗಮ (long_u)(ms.ullAvailVirtual / 1024);
    }
    ಯದಿ (ms.ullTotalPhys <= WINNT_RESERVE_BYTES)
    {
	// Catch old NT box or perverse hardware setup.
	ಪ್ರತ್ಯಾಗಮ (long_u)((ms.ullTotalPhys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    ಪ್ರತ್ಯಾಗಮ (long_u)((ms.ullTotalPhys - WINNT_RESERVE_BYTES) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    ಪೂರ್ಣಾಙ್ಕ		flags)		// EW_* flags
{
    ಪ್ರತ್ಯಾಗಮ dos_expandpath(gap, path, 0, flags, FALSE);
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
    ಪೂರ್ಣಾಙ್ಕ
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	szTempFile[_MAX_PATH + 1];
    WCHAR	szNewPath[_MAX_PATH + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    ಪ್ರತಿ (p = q = wold; *p != L'\0'; ++p)
    {
	ವಿಕಲ್ಪ (*p)
	{
	ಪಕ್ಷ L'/':
	ಪಕ್ಷ L'\\':
	ಪಕ್ಷ L':':
	    ಯದಿ (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)(p - q) < 8 || q[6] != L'~')
	ಪ್ರತ್ಯಾಗಮ (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    ಯದಿ (GetFullPathNameW(wnew, _MAX_PATH, szNewPath, &p) == 0 || p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    ಯದಿ (GetTempFileNameW(szNewPath, L"VIM", 0, szTempFile) == 0)
	ಪ್ರತ್ಯಾಗಮ -2;

    // blow the temp file away
    ಯದಿ (!DeleteFileW(szTempFile))
	ಪ್ರತ್ಯಾಗಮ -3;

    // rename old file to the temp file
    ಯದಿ (!MoveFileW(wold, szTempFile))
	ಪ್ರತ್ಯಾಗಮ -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    ಯದಿ ((hf = CreateFileW(wold, GENERIC_WRITE, 0, ರಿಕ್ತ, CREATE_NEW,
		    FILE_ATTRIBUTE_NORMAL, ರಿಕ್ತ)) == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ -5;
    ಯದಿ (!CloseHandle(hf))
	ಪ್ರತ್ಯಾಗಮ -6;

    // rename the temp file to the new file
    ಯದಿ (!MoveFileW(szTempFile, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(ಶೂನ್ಯ)MoveFileW(szTempFile, wold);
	ಪ್ರತ್ಯಾಗಮ -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(szTempFile);

    // finally, remove the empty old file
    ಯದಿ (!DeleteFileW(wold))
	ಪ್ರತ್ಯಾಗಮ -8;

    ಪ್ರತ್ಯಾಗಮ 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_rename(
    ಧ್ರುವ ವರ್ಣ	*pszOldFile,
    ಧ್ರುವ ವರ್ಣ	*pszNewFile)
{
    WCHAR	*wold = ರಿಕ್ತ;
    WCHAR	*wnew = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		retval = -1;

    wold = enc_to_utf16((char_u *)pszOldFile, ರಿಕ್ತ);
    wnew = enc_to_utf16((char_u *)pszNewFile, ರಿಕ್ತ);
    ಯದಿ (wold != ರಿಕ್ತ && wnew != ರಿಕ್ತ)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    ವರ್ಣ *
default_shell(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_access(ವರ್ಣ *n, ಪೂರ್ಣಾಙ್ಕ p)
{
    HANDLE	hFile;
    ಪೂರ್ಣಾಙ್ಕ		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_to_utf16((char_u *)n, ರಿಕ್ತ);
    ಯದಿ (wn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (mch_isdir((char_u *)n))
    {
	WCHAR TempNameW[_MAX_PATH + 16] = L"";

	ಯದಿ (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    ಪೂರ್ಣಾಙ್ಕ			i;
	    WIN32_FIND_DATAW    d;

	    ಪ್ರತಿ (i = 0; i < _MAX_PATH && wn[i] != 0; ++i)
		TempNameW[i] = wn[i];
	    ಯದಿ (TempNameW[i - 1] != '\\' && TempNameW[i - 1] != '/')
		TempNameW[i++] = '\\';
	    TempNameW[i++] = '*';
	    TempNameW[i++] = 0;

	    hFile = FindFirstFileW(TempNameW, &d);
	    ಯದಿ (hFile == INVALID_HANDLE_ಮೌಲ್ಯ)
		ಗಚ್ಛ getout;
	    ಅನ್ಯಥಾ
		(ಶೂನ್ಯ)FindClose(hFile);
	}

	ಯದಿ (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    ಯದಿ (!GetTempFileNameW(wn, L"VIM", 0, TempNameW))
		ಗಚ್ಛ getout;
	    ಅನ್ಯಥಾ
		DeleteFileW(TempNameW);
	}
    }
    ಅನ್ಯಥಾ
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_mode = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_mode, share_mode,
					     ರಿಕ್ತ, OPEN_EXISTING, 0, ರಿಕ್ತ);
	ಯದಿ (hFile == INVALID_HANDLE_ಮೌಲ್ಯ)
	    ಗಚ್ಛ getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_open(ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ flags, ಪೂರ್ಣಾಙ್ಕ mode)
{
    WCHAR	*wn;
    ಪೂರ್ಣಾಙ್ಕ		f;

    wn = enc_to_utf16((char_u *)name, ರಿಕ್ತ);
    ಯದಿ (wn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    f = _wopen(wn, flags, mode);
    vim_free(wn);
    ಪ್ರತ್ಯಾಗಮ f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *mode)
{
    WCHAR	*wn, *wm;
    FILE	*f = ರಿಕ್ತ;

#ಯದಿ defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    ವರ್ಣ newMode = mode[strlen(mode) - 1];
    ಪೂರ್ಣಾಙ್ಕ oldMode = 0;

    _get_fmode(&oldMode);
    ಯದಿ (newMode == 't')
	_set_fmode(_O_TEXT);
    ಅನ್ಯಥಾ ಯದಿ (newMode == 'b')
	_set_fmode(O_ದ್ವಿಮಾನ_2);
#endif
    wn = enc_to_utf16((char_u *)name, ರಿಕ್ತ);
    wm = enc_to_utf16((char_u *)mode, ರಿಕ್ತ);
    ಯದಿ (wn != ರಿಕ್ತ && wm != ರಿಕ್ತ)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#ಯದಿ defined(DEBUG) && _MSC_VER >= 1400
    ಯದಿ (oldMode != 0)
	_set_fmode(oldMode);
#endif
    ಪ್ರತ್ಯಾಗಮ f;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_substream(HANDLE sh, ಶೂನ್ಯ *context, WCHAR *to, WCHAR *substream, long len)
{
    HANDLE  hTo;
    WCHAR   *to_name;

    to_name = alloc((wcslen(to) + wcslen(substream) + 1) * ಪರಿಮಾಣ(WCHAR));
    ಯದಿ (to_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    wcscpy(to_name, to);
    wcscat(to_name, substream);

    hTo = CreateFileW(to_name, GENERIC_WRITE, 0, ರಿಕ್ತ, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL, ರಿಕ್ತ);
    ಯದಿ (hTo != INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	long	done;
	DWORD	todo;
	DWORD	readcnt, written;
	ವರ್ಣ	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	ಪ್ರತಿ (done = 0; done < len; done += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - done) > ಪರಿಮಾಣ(buf) ? ಪರಿಮಾಣ(buf)
						       : (size_t)(len - done));
	    ಯದಿ (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(hTo, buf, todo, &written, ರಿಕ್ತ)
		    || written != todo)
		ಭಙ್ಗ;
	}
	CloseHandle(hTo);
    }

    ಮುಕ್ತ(to_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_infostreams(char_u *from, char_u *to)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_STREAM_ID	sid;
    ಪೂರ್ಣಾಙ್ಕ			headersize;
    WCHAR		streamname[_MAX_PATH];
    DWORD		readcount;
    ಶೂನ್ಯ		*context = ರಿಕ್ತ;
    DWORD		lo, hi;
    ಪೂರ್ಣಾಙ್ಕ			len;

    // Convert the file names to wide characters.
    fromw = enc_to_utf16(from, ರಿಕ್ತ);
    tow = enc_to_utf16(to, ರಿಕ್ತ);
    ಯದಿ (fromw != ರಿಕ್ತ && tow != ರಿಕ್ತ)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, ರಿಕ್ತ,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, ರಿಕ್ತ);
	ಯದಿ (sh != INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    ಪ್ರತಿ (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		ZeroMemory(&sid, ಪರಿಮಾಣ(WIN32_STREAM_ID));
		headersize = (ಪೂರ್ಣಾಙ್ಕ)((ವರ್ಣ *)&sid.cStreamName - (ವರ್ಣ *)&sid.dwStreamId);
		ಯದಿ (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    ಭಙ್ಗ;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		ಯದಿ (sid.dwStreamNameSize > 0)
		{
		    // Read the stream name.
		    ಯದಿ (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dwStreamNameSize,
					  &readcount, FALSE, FALSE, &context))
			ಭಙ್ಗ;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / ಪರಿಮಾಣ(WCHAR);
		    streamname[len] = 0;
		    ಯದಿ (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (long)sid.Size.u.ಕೆಳಗಿನ_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(ಶೂನ್ಯ)BackupSeek(sh, sid.Size.u.ಕೆಳಗಿನ_Part, sid.Size.u.ಮೇಲಿನ_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (ಶೂನ್ಯ)BackupRead(sh, ರಿಕ್ತ, 0, &readcount, TRUE, FALSE, &context);

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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _FILE_FULL_EA_INFORMATION_ {
    ULONG  NextEntryOffset;
    UCHAR  Flags;
    UCHAR  EaNameLength;
    USHORT Ea_ಮೌಲ್ಯ_Length;
    CHAR   EaName[1];
} FILE_FULL_EA_INFORMATION_, *PFILE_FULL_EA_INFORMATION_;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _FILE_EA_INFORMATION_ {
    ULONG EaSize;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#ifndef PROTO
ಪ್ರಕಾರನಿರ್ದೇಶ NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions);
ಪ್ರಕಾರನಿರ್ದೇಶ NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
ಪ್ರಕಾರನಿರ್ದೇಶ NTSTATUS (NTAPI *PfnNtSetEaFile)(
	HANDLE		    FileHandle,
	PIO_STATUS_BLOCK    IoStatusBlock,
	PVOID		    Buffer,
	ULONG		    Length);
ಪ್ರಕಾರನಿರ್ದೇಶ NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	BOOLEAN ReturnSingleEntry,
	PVOID EaList,
	ULONG EaListLength,
	PULONG EaIndex,
	BOOLEAN RestartScan);
ಪ್ರಕಾರನಿರ್ದೇಶ NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_STATUS_BLOCK	IoStatusBlock,
	PVOID			FileInformation,
	ULONG			Length,
	FILE_INFORMATION_CLASS FileInformationClass);
ಪ್ರಕಾರನಿರ್ದೇಶ VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString);

PfnNtOpenFile pNtOpenFile = ರಿಕ್ತ;
PfnNtClose pNtClose = ರಿಕ್ತ;
PfnNtSetEaFile pNtSetEaFile = ರಿಕ್ತ;
PfnNtQueryEaFile pNtQueryEaFile = ರಿಕ್ತ;
PfnNtQueryInformationFile pNtQueryInformationFile = ರಿಕ್ತ;
PfnRtlInitUnicodeString pRtlInitUnicodeString = ರಿಕ್ತ;
#endif

/*
 * Load ntdll.dll functions.
 */
    ಸ್ಥಿರ BOOL
load_ntdll(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	loaded = -1;

    ಯದಿ (loaded != -1)
	ಪ್ರತ್ಯಾಗಮ (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    ಯದಿ (hNtdll != ರಿಕ್ತ)
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
    ಯದಿ (pNtOpenFile == ರಿಕ್ತ
	    || pNtClose == ರಿಕ್ತ
	    || pNtSetEaFile == ರಿಕ್ತ
	    || pNtQueryEaFile == ರಿಕ್ತ
	    || pNtQueryInformationFile == ರಿಕ್ತ
	    || pRtlInitUnicodeString == ರಿಕ್ತ)
	loaded = FALSE;
    ಅನ್ಯಥಾ
	loaded = TRUE;
    ಪ್ರತ್ಯಾಗಮ (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_extattr(char_u *from, char_u *to)
{
    char_u		    *fromf = ರಿಕ್ತ;
    char_u		    *tof = ರಿಕ್ತ;
    WCHAR		    *fromw = ರಿಕ್ತ;
    WCHAR		    *tow = ರಿಕ್ತ;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_ATTRIBUTES	    oa;
    IO_STATUS_BLOCK	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    ಶೂನ್ಯ		    *ea = ರಿಕ್ತ;

    ಯದಿ (!load_ntdll())
	ಪ್ರತ್ಯಾಗಮ;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(to) + 5);
    ಯದಿ (fromf == ರಿಕ್ತ || tof == ರಿಕ್ತ)
	ಗಚ್ಛ theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), to);

    // Convert the names to wide characters.
    fromw = enc_to_utf16(fromf, ರಿಕ್ತ);
    tow = enc_to_utf16(tof, ರಿಕ್ತ);
    ಯದಿ (fromw == ರಿಕ್ತ || tow == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    InitializeObjectAttributes(&oa, &u, 0, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	ಗಚ್ಛ theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, ಪರಿಮಾಣ(eainfo),
	    FileEaInformation);
    ಯದಿ (eainfo.EaSize != 0)
    {
	ea = alloc(eainfo.EaSize);
	ಯದಿ (ea != ರಿಕ್ತ)
	{
	    ಯದಿ (pNtQueryEaFile(h, &iosb, ea, eainfo.EaSize, FALSE,
			ರಿಕ್ತ, 0, ರಿಕ್ತ, TRUE) != STATUS_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    ಯದಿ (ea != ರಿಕ್ತ)
    {
	pRtlInitUnicodeString(&u, tow);
	InitializeObjectAttributes(&oa, &u, 0, ರಿಕ್ತ, ರಿಕ್ತ);
	ಯದಿ (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	    ಗಚ್ಛ theend;

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
    ಪೂರ್ಣಾಙ್ಕ
mch_copy_file_attribute(char_u *from, char_u *to)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, to);
    copy_extattr(from, to);
    ಪ್ರತ್ಯಾಗಮ 0;
}


/*
 * The command line arguments in UTF-16
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	nArgsW = 0;
ಸ್ಥಿರ LPWSTR	*ArglistW = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	global_argc = 0;
ಸ್ಥಿರ ವರ್ಣ	**global_argv;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	used_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	*used_file_indexes = ರಿಕ್ತ; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	used_file_ಎಣಿಕೆ = 0;	// nr of entries in used_file_indexes
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	used_file_literal = FALSE;  // take file names literally
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	used_file_full_path = FALSE;  // file name was full path
#ifdef FEAT_DIFF
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	used_file_diff_mode = FALSE;  // file name was with diff mode
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	used_alist_ಎಣಿಕೆ = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_cmd_argsW(ವರ್ಣ ***argvp)
{
    ವರ್ಣ	**argv = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		argc = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;

    free_cmd_argsW();
    ArglistW = Command_ಸಾಲು_To_Argv_W(Get_Command_ಸಾಲು_W(), &nArgsW);
    ಯದಿ (ArglistW != ರಿಕ್ತ)
    {
	argv = alloc((nArgsW + 1) * ಪರಿಮಾಣ(ವರ್ಣ *));
	ಯದಿ (argv != ರಿಕ್ತ)
	{
	    argc = nArgsW;
	    argv[argc] = ರಿಕ್ತ;
	    ಪ್ರತಿ (i = 0; i < argc; ++i)
	    {
		ಪೂರ್ಣಾಙ್ಕ	len;

		// Convert each Unicode argument to UTF-8.
		WideCharToMultiByte_alloc(CP_UTF8, 0,
				ArglistW[i], (ಪೂರ್ಣಾಙ್ಕ)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		ಯದಿ (argv[i] == ರಿಕ್ತ)
		{
		    // Out of memory, clear everything.
		    ಯಾವತ್ (i > 0)
			ಮುಕ್ತ(argv[--i]);
		    ಮುಕ್ತ(argv);
		    argv = ರಿಕ್ತ;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    ಯದಿ (argc > 0)
    {
	ಯದಿ (used_file_indexes != ರಿಕ್ತ)
	    ಮುಕ್ತ(used_file_indexes);
	used_file_indexes = alloc(argc * ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
    }

    ಯದಿ (argvp != ರಿಕ್ತ)
	*argvp = argv;
    ಪ್ರತ್ಯಾಗಮ argc;
}

    ಶೂನ್ಯ
free_cmd_argsW(ಶೂನ್ಯ)
{
    ಯದಿ (ArglistW == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    GlobalFree(ArglistW);
    ArglistW = ರಿಕ್ತ;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    ಶೂನ್ಯ
used_file_arg(ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ literal, ಪೂರ್ಣಾಙ್ಕ full_path, ಪೂರ್ಣಾಙ್ಕ diff_mode)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (used_file_indexes == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (i = used_file_argc + 1; i < global_argc; ++i)
	ಯದಿ (STRCMP(global_argv[i], name) == 0)
	{
	    used_file_argc = i;
	    used_file_indexes[used_file_ಎಣಿಕೆ++] = i;
	    ಭಙ್ಗ;
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
    ಶೂನ್ಯ
set_alist_ಎಣಿಕೆ(ಶೂನ್ಯ)
{
    used_alist_ಎಣಿಕೆ = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    ಶೂನ್ಯ
fix_arg_enc(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    char_u	*str;
    ಪೂರ್ಣಾಙ್ಕ		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    ಯದಿ (global_argc != nArgsW
	    || ArglistW == ರಿಕ್ತ
	    || used_file_indexes == ರಿಕ್ತ
	    || used_file_ಎಣಿಕೆ == 0
	    || used_alist_ಎಣಿಕೆ != GARGCOUNT)
	ಪ್ರತ್ಯಾಗಮ;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, GARGCOUNT);
    ಯದಿ (fnum_list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;		// out of memory
    ಪ್ರತಿ (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    ಯದಿ (ga_grow(&global_alist.al_ga, used_file_ಎಣಿಕೆ) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;		// out of memory

    ಪ್ರತಿ (i = 0; i < used_file_ಎಣಿಕೆ; ++i)
    {
	idx = used_file_indexes[i];
	str = utf16_to_enc(ArglistW[idx], ರಿಕ್ತ);
	ಯದಿ (str != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ literal = used_file_literal;

#ifdef FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    ಯದಿ (used_file_diff_mode && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), tail, STRLEN(tail), TRUE, &ret);
		ಯದಿ (ret.string != ರಿಕ್ತ)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#endif
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    ಯದಿ (used_file_literal)
		buf_set_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    ಯದಿ (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		ಯದಿ (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_add(&global_alist, str, literal ? 2 : 0);
	}
    }

    ಯದಿ (!used_file_literal)
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
	alist_expand(fnum_list, used_alist_ಎಣಿಕೆ);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    ಯದಿ (curwin->w_arg_idx == 0 && curbuf->b_fname == ರಿಕ್ತ)
    {
	do_cmdline_cmd((char_u *)":rewind");
	ಯದಿ (GARGCOUNT == 1 && used_file_full_path
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    last_chdir_reason = "drop";
    }

    set_alist_ಎಣಿಕೆ();
}

    ಪೂರ್ಣಾಙ್ಕ
mch_setenv(ವರ್ಣ *var, ವರ್ಣ *ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ x UNUSED)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(ಮೌಲ್ಯ) + 2);
    ಯದಿ (envbuf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    sprintf((ವರ್ಣ *)envbuf, "%s=%s", var, ಮೌಲ್ಯ);

    p = enc_to_utf16(envbuf, ರಿಕ್ತ);

    vim_free(envbuf);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    _wputenv(p);
#ifdef libintl_wputenv
    libintl_wputenv(p);
#endif
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    ಪ್ರತ್ಯಾಗಮ 0;
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

    ಸ್ಥಿರ ಶೂನ್ಯ
vtp_flag_init(ಶೂನ್ಯ)
{
#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   mode;
    HANDLE  out;

# ifdef VIMDLL
    ಯದಿ (!gui.in_use)
# endif
    {
	out = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_version >= VTP_FIRST_SUPPORT_BUILD) ? 1 : 0;
	GetConsoleMode(out, &mode);
	mode |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	ಯದಿ (SetConsoleMode(out, mode) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_buffer = win_version >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#endif

    ಯದಿ (win_version >= CONPTY_FIRST_SUPPORT_BUILD)
	conpty_working = 1;
    ಯದಿ (win_version >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    ಯದಿ (win_version <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    ಯದಿ (win_version <= CONPTY_1909_BUILD)
	conpty_type = 2;
    ಯದಿ (win_version <= CONPTY_1903_BUILD)
	conpty_type = 2;
    ಯದಿ (win_version < CONPTY_FIRST_SUPPORT_BUILD)
	conpty_type = 1;

    ಯದಿ (win_version >= CONPTY_NEXT_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ಸ್ಥಿರ ಶೂನ್ಯ
vtp_init(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = ಪರಿಮಾಣ(csbi);
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

    ಸ್ಥಿರ ಶೂನ್ಯ
vtp_exit(ಶೂನ್ಯ)
{
    restore_console_color_rgb();
}

    ಪೂರ್ಣಾಙ್ಕ
vtp_printf(
    ವರ್ಣ *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   result;
    ಪೂರ್ಣಾಙ್ಕ	    len;

    ಯದಿ (silent_mode)
	ಪ್ರತ್ಯಾಗಮ 0;

    va_start(list, format);
    len = vim_vsnprintf((ವರ್ಣ *)buf, 100, (ವರ್ಣ *)format, list);
    va_end(list);
    WriteConsoleA(g_hConOut, buf, (DWORD)len, &result, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)result;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vtp_sgr_bulk(
    ಪೂರ್ಣಾಙ್ಕ arg)
{
    ಪೂರ್ಣಾಙ್ಕ args[1];

    args[0] = arg;
    vtp_sgr_bulks(1, args);
}

# define FAST256(x) \
    ಯದಿ ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    ಪಕ್ಷ x: \
	FAST256(newargs[x - 1]);

    ಸ್ಥಿರ ಶೂನ್ಯ
vtp_sgr_bulks(
    ಪೂರ್ಣಾಙ್ಕ argc,
    ಪೂರ್ಣಾಙ್ಕ *args)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    ಪೂರ್ಣಾಙ್ಕ	    in, out;
    ಪೂರ್ಣಾಙ್ಕ	    newargs[16];
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sgrfgr = -1, sgrfgg, sgrfgb;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sgrbgr = -1, sgrbgg, sgrbgb;

    ಯದಿ (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	ಪ್ರತ್ಯಾಗಮ;
    }

    in = out = 0;
    ಯಾವತ್ (in < argc)
    {
	ಪೂರ್ಣಾಙ್ಕ s = args[in];
	ಪೂರ್ಣಾಙ್ಕ copylen = 1;

	ಯದಿ (s == 38)
	{
	    ಯದಿ (argc - in >= 5 && args[in + 1] == 2)
	    {
		ಯದಿ (sgrfgr == args[in + 2] && sgrfgg == args[in + 3]
						     && sgrfgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		ಅನ್ಯಥಾ
		{
		    sgrfgr = args[in + 2];
		    sgrfgg = args[in + 3];
		    sgrfgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (s == 48)
	{
	    ಯದಿ (argc - in >= 5 && args[in + 1] == 2)
	    {
		ಯದಿ (sgrbgr == args[in + 2] && sgrbgg == args[in + 3]
						     && sgrbgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		ಅನ್ಯಥಾ
		{
		    sgrbgr = args[in + 2];
		    sgrbgg = args[in + 3];
		    sgrbgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (30 <= s && s <= 39)
	    sgrfgr = -1;
	ಅನ್ಯಥಾ ಯದಿ (90 <= s && s <= 97)
	    sgrfgr = -1;
	ಅನ್ಯಥಾ ಯದಿ (40 <= s && s <= 49)
	    sgrbgr = -1;
	ಅನ್ಯಥಾ ಯದಿ (100 <= s && s <= 107)
	    sgrbgr = -1;
	ಅನ್ಯಥಾ ಯದಿ (s == 0)
	    sgrfgr = sgrbgr = -1;

	ಯಾವತ್ (copylen--)
	    newargs[out++] = args[in++];
    }

    p = &buf[ಪರಿಮಾಣ(buf) - 1];
    *p-- = 'm';

    ವಿಕಲ್ಪ (out)
    {
	ಪೂರ್ಣಾಙ್ಕ	n, m;
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
	WriteConsoleA(g_hConOut, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, ರಿಕ್ತ);
    ಸ್ವಭಾವ:
	ಭಙ್ಗ;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
wt_init(ಶೂನ್ಯ)
{
    wt_working = mch_getenv("WT_SESSION") != ರಿಕ್ತ;
}

# ifdef FEAT_TERMGUICOLORS
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ctermtoxterm(
    ಪೂರ್ಣಾಙ್ಕ cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    ಪ್ರತ್ಯಾಗಮ (((ಪೂರ್ಣಾಙ್ಕ)r << 16) | ((ಪೂರ್ಣಾಙ್ಕ)g << 8) | (ಪೂರ್ಣಾಙ್ಕ)b);
}
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
set_console_color_rgb(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    guicolor_T	fg, bg;
    ಪೂರ್ಣಾಙ್ಕ		ctermfg, ctermbg;

    ಯದಿ (!vtp_working)
	ಪ್ರತ್ಯಾಗಮ;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    ಯದಿ (p_tgc || t_colors >= 256)
    {
	term_fg_rgb_color(fg);
	term_bg_rgb_color(bg);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (use_alternate_screen_buffer)
	ಪ್ರತ್ಯಾಗಮ;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cbSize = ಪರಿಮಾಣ(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = ಪರಿಮಾಣ(csbi);
    csbi.srWindow.ಬಲ_3 += 1;
    csbi.srWindow.Bottom += 1;
    store_console_bg_rgb = csbi.ColorTable[g_color_index_bg];
    store_console_fg_rgb = csbi.ColorTable[g_color_index_fg];
    csbi.ColorTable[g_color_index_bg] = (COLORREF)bg;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)fg;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

# ಯದಿ defined(FEAT_TERMGUICOLORS)
    ಶೂನ್ಯ
get_default_console_color(
    ಪೂರ್ಣಾಙ್ಕ *cterm_fg,
    ಪೂರ್ಣಾಙ್ಕ *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    ಪೂರ್ಣಾಙ್ಕ id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    ಪೂರ್ಣಾಙ್ಕ ctermfg = 0;
    ಪೂರ್ಣಾಙ್ಕ ctermbg = 0;
    ಪೂರ್ಣಾಙ್ಕ dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    ಯದಿ (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    ಯದಿ (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	ಯದಿ (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	ಯದಿ (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	ಅನ್ಯಥಾ
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_normal_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    ಯದಿ (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	ಯದಿ (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	ಯದಿ (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
reset_console_color_rgb(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMGUICOLORS
    ಯದಿ (use_alternate_screen_buffer)
	ಪ್ರತ್ಯಾಗಮ;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = ಪರಿಮಾಣ(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = ಪರಿಮಾಣ(csbi);
    csbi.srWindow.ಬಲ_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)store_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)store_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_console_color_rgb(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMGUICOLORS
    ಯದಿ (use_alternate_screen_buffer)
	ಪ್ರತ್ಯಾಗಮ;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = ಪರಿಮಾಣ(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = ಪರಿಮಾಣ(csbi);
    csbi.srWindow.ಬಲ_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)save_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)save_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

    ಶೂನ್ಯ
control_console_color_rgb(ಶೂನ್ಯ)
{
    ಯದಿ (vtp_working)
	set_console_color_rgb();
    ಅನ್ಯಥಾ
	reset_console_color_rgb();
}

    ಪೂರ್ಣಾಙ್ಕ
use_vtp(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ USE_VTP;
}

    ಪೂರ್ಣಾಙ್ಕ
is_term_win32(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ T_NAME != ರಿಕ್ತ && STRCMP(T_NAME, "win32") == 0;
}

    ಪೂರ್ಣಾಙ್ಕ
has_vtp_working(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ vtp_working;
}

#endif

    ಪೂರ್ಣಾಙ್ಕ
has_conpty_working(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ conpty_working;
}

    ಪೂರ್ಣಾಙ್ಕ
get_conpty_type(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ conpty_type;
}

    ಪೂರ್ಣಾಙ್ಕ
is_conpty_stable(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ conpty_stable;
}

    ಪೂರ್ಣಾಙ್ಕ
get_conpty_fix_type(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ conpty_fix_type;
}

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ಶೂನ್ಯ
resize_console_buf(ಶೂನ್ಯ)
{
    ಯದಿ (use_alternate_screen_buffer)
	ಪ್ರತ್ಯಾಗಮ;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    ಯದಿ (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ಪ್ರತ್ಯಾಗಮ;

    coord.X = SRWIDTH(csbi.srWindow);
    coord.Y = SRHEIGHT(csbi.srWindow);
    SetConsoleScreenBufferSize(g_hConOut, coord);

    newsize.ಎಡ_3 = 0;
    newsize.ತುದಿ_2 = 0;
    newsize.ಬಲ_3 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    SetConsoleWindowInfo(g_hConOut, TRUE, &newsize);

    SetConsoleScreenBufferSize(g_hConOut, coord);
}
#endif

    ವರ್ಣ *
GetWin32Error(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ವರ್ಣ	*oldmsg = ರಿಕ್ತ;
    ವರ್ಣ	*acp_ಸಂದೇಶ = ರಿಕ್ತ;
    DWORD	acp_len;
    ವರ್ಣ	*enc_ಸಂದೇಶ = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		enc_len = 0;

    // get formatted message from OS
    acp_len = FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
	    ರಿಕ್ತ, GetLastError(), 0, (LPSTR)&acp_ಸಂದೇಶ, 0, ರಿಕ್ತ);
    ಯದಿ (acp_len == 0 || acp_ಸಂದೇಶ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // clean oldmsg if remained.
    ಯದಿ (oldmsg != ರಿಕ್ತ)
    {
	vim_free(oldmsg);
	oldmsg = ರಿಕ್ತ;
    }

    acp_to_enc((char_u *)acp_ಸಂದೇಶ, (ಪೂರ್ಣಾಙ್ಕ)acp_len, (char_u **)&enc_ಸಂದೇಶ, &enc_len);
    LocalFree(acp_ಸಂದೇಶ);
    ಯದಿ (enc_ಸಂದೇಶ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // remove trailing \r\n
    ವರ್ಣ *pcrlf = strstr(enc_ಸಂದೇಶ, "\r\n");
    ಯದಿ (pcrlf != ರಿಕ್ತ)
	*pcrlf = NUL;

    oldmsg = enc_ಸಂದೇಶ;
    ಪ್ರತ್ಯಾಗಮ enc_ಸಂದೇಶ;
}

#ಯದಿ defined(FEAT_RELTIME)
ಸ್ಥಿರ HANDLE   timer_handle;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ      timer_active = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
ಸ್ಥಿರ sig_atomic_t timeout_flags[2];
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    timeout_flag_idx = 0;
ಸ್ಥಿರ sig_atomic_t *timeout_flag = &timeout_flags[0];


    ಸ್ಥಿರ ಶೂನ್ಯ CALLBACK
set_flag(ಶೂನ್ಯ *param, BOOLEAN unused2 UNUSED)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    ಶೂನ್ಯ
stop_timeout(ಶೂನ್ಯ)
{
    ಯದಿ (timer_active)
    {
	BOOL ret = DeleteTimerQueueTimer(ರಿಕ್ತ, timer_handle, ರಿಕ್ತ);
	timer_active = FALSE;
	ಯದಿ (!ret && GetLastError() != ERROR_IO_PENDING)
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
    ಚಞ್ಚಲ sig_atomic_t *
start_timeout(long msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = CreateTimerQueueTimer(
	    &timer_handle, ರಿಕ್ತ, set_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    ಯದಿ (!ret)
    {
	semsg(_(e_could_not_set_timeout_str), GetWin32Error());
    }
    ಅನ್ಯಥಾ
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_active = TRUE;
	*timeout_flag = FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ timeout_flag;
}
#endif
