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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#ଅନ୍ତର୍ଭାଵ <sys/types.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <limits.h>

#ଅନ୍ତର୍ଭାଵ <process.h>
#ଅନ୍ତର୍ଭାଵ <winternl.h>
#ଅନ୍ତର୍ଭାଵ <direct.h>

#ଯଦି !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#ifdef FEAT_JOB_CHANNEL
# include <tlhelp32.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = ଖାଲି;
#endif

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
ସ୍ଥିର HANDLE g_hConIn  = INVALID_HANDLE_ମୂଲ୍ୟ;
ସ୍ଥିର HANDLE g_hConOut = INVALID_HANDLE_ମୂଲ୍ୟ;

// Win32 Screen buffer,coordinate,console I/O information
ସ୍ଥିର SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
ସ୍ଥିର COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
ସ୍ଥିର WORD  g_attrDefault = 7;  // lightgray text on black background
ସ୍ଥିର WORD  g_attrCurrent;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_fForceExit = FALSE;    // set when forcefully exiting

ସ୍ଥିର ଶୂନ୍ୟ scroll(unsigned c_ଧାଡ଼ିଗୁଡ଼ିକ);
ସ୍ଥିର ଶୂନ୍ୟ set_scroll_region(unsigned ବାମ, unsigned ଶୀର୍ଷ,
			      unsigned ଡାହାଣ, unsigned bottom);
ସ୍ଥିର ଶୂନ୍ୟ set_scroll_region_tb(unsigned ଶୀର୍ଷ, unsigned bottom);
ସ୍ଥିର ଶୂନ୍ୟ set_scroll_region_lr(unsigned ବାମ, unsigned ଡାହାଣ);
ସ୍ଥିର ଶୂନ୍ୟ ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(unsigned c_ଧାଡ଼ିଗୁଡ଼ିକ);
ସ୍ଥିର ଶୂନ୍ୟ delete_ଧାଡ଼ିଗୁଡ଼ିକ(unsigned c_ଧାଡ଼ିଗୁଡ଼ିକ);
ସ୍ଥିର ଶୂନ୍ୟ gotoxy(unsigned x, unsigned y);
ସ୍ଥିର ଶୂନ୍ୟ standout(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_cursor_visible = TRUE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ନିବେଶ_record_buffer_ଗଣ୍ଠି_S
{
    ନିବେଶ_RECORD ir;
    ସଂରଚନା ନିବେଶ_record_buffer_ଗଣ୍ଠି_S *next;
} ନିବେଶ_record_buffer_ଗଣ୍ଠି_T;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ନିବେଶ_record_buffer_S
{
    ନିବେଶ_record_buffer_ଗଣ୍ଠି_T *head;
    ନିବେଶ_record_buffer_ଗଣ୍ଠି_T *tail;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା length;
} ନିବେଶ_record_buffer_T;
ସ୍ଥିର ନିବେଶ_record_buffer_T ନିବେଶ_record_buffer;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_ନିବେଶ_record_buffer(ନିବେଶ_RECORD* irEvents, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nMaxLength);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_ନିବେଶ_record_buffer(ନିବେଶ_RECORD* irEvents, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nLength);
#endif
#ifdef FEAT_GUI_MSWIN
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_dont_use_vimrun = TRUE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା need_vimrun_warning = FALSE;
ସ୍ଥିର string_T vimrun_path = {(char_u *)"vimrun ", 7};
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vimrun_path_allocated = FALSE;
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା win32_getattrs(char_u *name);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା win32_setattrs(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attrs);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା win32_set_archive(char_u *name);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conpty_working = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conpty_type = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conpty_stable = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conpty_fix_type = 0;
ସ୍ଥିର ଶୂନ୍ୟ vtp_flag_init(ଶୂନ୍ୟ);

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vtp_working = 0;
ସ୍ଥିର ଶୂନ୍ୟ vtp_init(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ vtp_exit(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ vtp_sgr_bulk(ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg);
ସ୍ଥିର ଶୂନ୍ୟ vtp_sgr_bulks(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *argv);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା wt_working = 0;
ସ୍ଥିର ଶୂନ୍ୟ wt_init(ଶୂନ୍ୟ);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_color_index_bg = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_color_index_fg = 7;

# ifdef FEAT_TERMGUICOLORS
ସ୍ଥିର guicolor_T save_console_bg_rgb;
ସ୍ଥିର guicolor_T save_console_fg_rgb;
ସ୍ଥିର guicolor_T store_console_bg_rgb;
ସ୍ଥିର guicolor_T store_console_fg_rgb;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା default_console_color_bg = 0x000000; // black
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_term_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# ଅନ୍ୟଥା
#  define USE_VTP		0
#  define USE_WT		0
# endif

ସ୍ଥିର ଶୂନ୍ୟ set_console_color_rgb(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ reset_console_color_rgb(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ restore_console_color_rgb(ଶୂନ୍ୟ);
#endif  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା suppress_winsize = 1;	// don't fiddle with console
#endif

ସ୍ଥିର WCHAR *exe_pathw = ଖାଲି;

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ସ୍ଥିର BOOL use_alternate_screen_buffer = FALSE;
#endif

ବାହ୍ୟ DWORD win_version; // this is in os_mswin.c

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ସ୍ଥିର BOOL
is_ambiwidth_event(
    ନିବେଶ_RECORD *ir)
{
    ଫେରନ୍ତୁ ir->EventType == ଚାବି_EVENT
		&& ir->Event.ଚାବି_Event_2.b_ଚାବି_Down
		&& ir->Event.ଚାବି_Event_2.w_Repeat_ଗଣନା == 1
		&& ir->Event.ଚାବି_Event_2.w_Virtual_ଚାବି_Code == 0x12
		&& ir->Event.ଚାବି_Event_2.wVirtualScanCode == 0x38
		&& ir->Event.ଚାବି_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.ଚାବି_Event_2.dw_Control_ଚାବି_State == 2;
}

    ସ୍ଥିର ଶୂନ୍ୟ
make_ambiwidth_event(
    ନିବେଶ_RECORD *down,
    ନିବେଶ_RECORD *up)
{
    down->Event.ଚାବି_Event_2.w_Virtual_ଚାବି_Code = 0;
    down->Event.ଚାବି_Event_2.wVirtualScanCode = 0;
    down->Event.ଚାବି_Event_2.uChar.UnicodeChar
				    = up->Event.ଚାବି_Event_2.uChar.UnicodeChar;
    down->Event.ଚାବି_Event_2.dw_Control_ଚାବି_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    ସ୍ଥିର BOOL
read_console_ନିବେଶ(
    HANDLE	    h_ନିବେଶ,
    ନିବେଶ_RECORD    *lpBuffer,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    nLength,
    LPDWORD	    lpEvents)
{
    ଗଣନା
    {
	IRSIZE = 10
    };
    ସ୍ଥିର ନିବେଶ_RECORD s_irCache[IRSIZE];
    ସ୍ଥିର DWORD s_dwIndex = 0;
    ସ୍ଥିର DWORD s_dwMax = 0;
    DWORD dwEvents;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା head;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tail;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ସ୍ଥିର ନିବେଶ_RECORD s_irPseudo;

    ଯଦି (s_dwMax == 0 && ନିବେଶ_record_buffer.length > 0)
    {
	dwEvents = read_ନିବେଶ_record_buffer(s_irCache, IRSIZE);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
    }

    ଯଦି (nLength == -2)
	ଫେରନ୍ତୁ (s_dwMax > 0) ? TRUE : FALSE;

    ଯଦି (win_version < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	ଯଦି (nLength == -1)
	    ଫେରନ୍ତୁ Peek_Console_ନିବେଶ_W(h_ନିବେଶ, lpBuffer, 1, lpEvents);
	ଫେରନ୍ତୁ Read_Console_ନିବେଶ_W(h_ନିବେଶ, lpBuffer, 1, &dwEvents);
    }

    ଯଦି (s_dwMax == 0)
    {
	ଯଦି (!vtp_working && nLength == -1)
	    ଫେରନ୍ତୁ Peek_Console_ନିବେଶ_W(h_ନିବେଶ, lpBuffer, 1, lpEvents);
	Get_ସଂଖ୍ୟା_Of_Console_ନିବେଶ_Events(h_ନିବେଶ, &dwEvents);
	ଯଦି (dwEvents == 0 && nLength == -1)
	    ଫେରନ୍ତୁ Peek_Console_ନିବେଶ_W(h_ନିବେଶ, lpBuffer, 1, lpEvents);
	Read_Console_ନିବେଶ_W(h_ନିବେଶ, s_irCache, IRSIZE, &dwEvents);
	s_dwIndex = 0;
	s_dwMax = dwEvents;
	ଯଦି (dwEvents == 0)
	{
	    *lpEvents = 0;
	    ଫେରନ୍ତୁ TRUE;
	}

	ପାଇଁ (i = s_dwIndex; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)s_dwMax - 1; ++i)
	    ଯଦି (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	ଯଦି (s_dwMax > 1)
	{
	    head = 0;
	    tail = s_dwMax - 1;
	    ଯେତେବେଳେ (head != tail)
	    {
		ଯଦି (s_irCache[head].EventType == WINDOW_BUFFER_SIZE_EVENT
			&& s_irCache[head + 1].EventType
						  == WINDOW_BUFFER_SIZE_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    ପାଇଁ (i = head; i < tail; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --tail;
		    ଜାରି;
		}
		head++;
	    }
	    s_dwMax = tail + 1;
	}
    }

    ଯଦି (s_irCache[s_dwIndex].EventType == ଚାବି_EVENT)
    {
	ଯଦି (s_irCache[s_dwIndex].Event.ଚାବି_Event_2.w_Repeat_ଗଣନା > 1)
	{
	    s_irPseudo = s_irCache[s_dwIndex];
	    s_irPseudo.Event.ଚାବି_Event_2.w_Repeat_ଗଣନା = 1;
	    s_irCache[s_dwIndex].Event.ଚାବି_Event_2.w_Repeat_ଗଣନା--;
	    *lpBuffer = s_irPseudo;
	    *lpEvents = 1;
	    ଫେରନ୍ତୁ TRUE;
	}
    }

    *lpBuffer = s_irCache[s_dwIndex];
    ଯଦି (!(nLength == -1 || nLength == -2) && ++s_dwIndex >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    ସ୍ଥିର BOOL
peek_console_ନିବେଶ(
    HANDLE	    h_ନିବେଶ,
    ନିବେଶ_RECORD    *lpBuffer,
    DWORD	    nLength UNUSED,
    LPDWORD	    lpEvents)
{
    ଫେରନ୍ତୁ read_console_ନିବେଶ(h_ନିବେଶ, lpBuffer, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    ସ୍ଥିର DWORD
ସନ୍ଦେଶ_wait_for_multiple_objects(
    DWORD    n_ଗଣନା,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dwWakeMask)
{
    ଯଦି (read_console_ନିବେଶ(ଖାଲି, ଖାଲି, -2, ଖାଲି))
	ଫେରନ୍ତୁ WAIT_OBJECT_0;
    ଫେରନ୍ତୁ ସନ୍ଦେଶ_Wait_For_Multiple_Objects(n_ଗଣନା, pHandles, fWaitAll,
				     dwMilliseconds, dwWakeMask);
}
# endif

# ifndef FEAT_CLIENTSERVER
    ସ୍ଥିର DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    ଯଦି (read_console_ନିବେଶ(ଖାଲି, ଖାଲି, -2, ଖାଲି))
	ଫେରନ୍ତୁ WAIT_OBJECT_0;
    ଫେରନ୍ତୁ WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#endif   // !FEAT_GUI_MSWIN || VIMDLL

    ଶୂନ୍ୟ
mch_get_exe_name(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		updated = FALSE;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	enc_prev = -1;
    WCHAR	*path;
    size_t	pathlen;

    ଯଦି (exe_name == ଖାଲି || exe_pathw == ଖାଲି || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_PATH];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(ଖାଲି, buf, MAX_PATH);
	ଯଦି (p > buf)
	{
	    ଯଦି (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_to_enc(buf, ଖାଲି);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    କର
	    {
		--p;
		ଯଦି (*p == L'\\')
		{
		    *p = L'\0';
		    ଭାଙ୍ଗ;
		}
	    } ଯେତେବେଳେ (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    ଯଦି (exe_pathw == ଖାଲି || !updated)
	ଫେରନ୍ତୁ;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    path = _wgetenv(L"PATH");
    pathlen = (path == ଖାଲି) ? 0 : wcslen(path);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#define EXTRA_LEN 7
#define MAX_ENV_STRING_LEN 8191

    ଯଦି (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   temp[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	ଯଦି (pathlen == 0)
	    wcscpy(temp + templen, exe_pathw);
	ଅନ୍ୟଥା
	{
	    wcscpy(temp + templen, path);

	    // Check if exe_path is already included in $PATH.
	    ଯଦି (wcsstr(temp, exe_pathw) == ଖାଲି)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		ଯଦି (temp[templen - 1] != L';')
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
    ସ୍ଥିର ଶୂନ୍ୟ
unescape_shellxquote(char_u *p, char_u *escaped)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n;

    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (*p == '^' && vim_strchr(escaped, p[1]) != ଖାଲି)
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
vimLoadLib(ଅଚଳ ଅକ୍ଷର *name)
{
    HINSTANCE	dll = ଖାଲି;

    // No need to load any library when registering OLE.
    ଯଦି (found_register_arg)
	ଫେରନ୍ତୁ ଖାଲି;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    ଯଦି (exe_pathw == ଖାଲି)
    {
	mch_get_exe_name();
	ଯଦି (exe_pathw == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    WCHAR old_dirw[MAXPATHL];

    ଯଦି (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	ଫେରନ୍ତୁ ଖାଲି;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    SetCurrentDirectoryW(exe_pathw);
    dll = LoadLibrary(name);
    SetCurrentDirectoryW(old_dirw);
    ଫେରନ୍ତୁ dll;
}

#ଯଦି defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_is_gui_executable(ଶୂନ୍ୟ)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(ଖାଲି);
    PIMAGE_DOS_HEADER	pDOS = (PIMAGE_DOS_HEADER)pImage;
    PIMAGE_NT_HEADERS	pPE;

    ଯଦି (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ଫେରନ୍ତୁ FALSE;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    ଯଦି (pPE->Signature != IMAGE_NT_SIGNATURE)
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (pPE->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}
#endif

#ଯଦି defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    ସ୍ଥିର ଶୂନ୍ୟ *
get_imported_func_info(HINSTANCE hInst, ଅଚଳ ଅକ୍ଷର *funcname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା info,
	ଅଚଳ ଶୂନ୍ୟ *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_HEADER		pDOS = (PIMAGE_DOS_HEADER)hInst;
    PIMAGE_NT_HEADERS		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_DATA		pIAT;	    // Import Address Table
    PIMAGE_THUNK_DATA		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    ଯଦି (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ଫେରନ୍ତୁ ଖାଲି;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    ଯଦି (pPE->Signature != IMAGE_NT_SIGNATURE)
	ଫେରନ୍ତୁ ଖାଲି;

    ImpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    ଯଦି (ImpVA == 0)
	ଫେରନ୍ତୁ ଖାଲି;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    ପାଇଁ (; pImpDesc->FirstThunk; ++pImpDesc)
    {
	ଯଦି (!pImpDesc->OriginalFirstThunk)
	    ଜାରି;
	pIAT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->FirstThunk);
	pINT = (PIMAGE_THUNK_DATA)(pImage + pImpDesc->OriginalFirstThunk);
	ପାଇଁ (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    ଯଦି (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		ଜାରି;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.AddressOfData));
	    ଯଦି (strcmp((ଅକ୍ଷର *)pImpName->Name, funcname) == 0)
	    {
		ଶୂନ୍ୟ *original;
		DWORD old, new = PAGE_READWRITE;

		ବାଛ (info)
		{
		    ସ୍ଥିତି 0:
			ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)pIAT->u1.Function;
		    ସ୍ଥିତି 1:
			ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)(pImage + pImpDesc->Name);
		    ସ୍ଥିତି 2:
			original = (ଶୂନ୍ୟ *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, ଆକାର(ଶୂନ୍ୟ *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, ଆକାର(ଶୂନ୍ୟ *),
				old, &new);
			ଫେରନ୍ତୁ original;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଫେରନ୍ତୁ ଖାଲି;
		}
	    }
	}
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
find_imported_module_by_funcname(HINSTANCE hInst, ଅଚଳ ଅକ୍ଷର *funcname)
{
    ଅକ୍ଷର    *modulename;

    modulename = (ଅକ୍ଷର *)get_imported_func_info(hInst, funcname, 1, ଖାଲି);
    ଯଦି (modulename != ଖାଲି)
	ଫେରନ୍ତୁ GetModuleHandleA(modulename);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    ଶୂନ୍ୟ *
get_dll_import_func(HINSTANCE hInst, ଅଚଳ ଅକ୍ଷର *funcname)
{
    ଫେରନ୍ତୁ get_imported_func_info(hInst, funcname, 0, ଖାଲି);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    ଶୂନ୍ୟ *
hook_dll_import_func(HINSTANCE hInst, ଅଚଳ ଅକ୍ଷର *funcname, ଅଚଳ ଶୂନ୍ୟ *hook)
{
    ଫେରନ୍ତୁ get_imported_func_info(hInst, funcname, 2, hook);
}
#endif

#ଯଦି defined(FEAT_PYTHON3)
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

    ଯଦି (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ଫେରନ୍ତୁ ଖାଲି;
    pPE = (PIMAGE_NT_HEADERS)(pImage + pDOS->e_lfanew);
    ଯଦି (pPE->Signature != IMAGE_NT_SIGNATURE)
	ଫେରନ୍ତୁ ଖାଲି;

    ExpVA = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    ExpSize = pPE->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    ଯଦି (ExpVA == 0)
	ଫେରନ୍ତୁ hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    pFunctionTable = (LPDWORD)(pImage + pExpDir->AddressOfFunctions);

    ଯଦି (pExpDir->ସଂଖ୍ୟା_Of_Names == 0)
	ଫେରନ୍ତୁ hInst;	// No export names.

    // Check only the first entry.
    ଯଦି ((pFunctionTable[0] < ExpVA) || (pFunctionTable[0] >= ExpVA + ExpSize))
	// The first entry is not a function forwarder.
	ଫେରନ୍ତୁ hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    ଅଚଳ ଅକ୍ଷର *name = (ଅଚଳ ଅକ୍ଷର *)(pImage + pFunctionTable[0]);
    ଅଚଳ ଅକ୍ଷର *p = strchr(name, '.');
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ hInst;

    // Extract DllName.
    ଅକ୍ଷର buf[MAX_PATH];
    ଯଦି (p - name + 1 > ଆକାର(buf))
	ଫେରନ୍ତୁ ଖାଲି;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    ଫେରନ୍ତୁ GetModuleHandleA(buf);
}
#endif

#ଯଦି defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
ସ୍ଥିର ଅକ୍ଷର *null_libintl_gettext(ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ଅକ୍ଷର *null_libintl_ngettext(ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *, unsigned long n);
ସ୍ଥିର ଅକ୍ଷର *null_libintl_textdomain(ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ଅକ୍ଷର *null_libintl_bindtextdomain(ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ଅକ୍ଷର *null_libintl_bind_textdomain_codeset(ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା null_libintl_wputenv(ଅଚଳ wchar_t *);

ସ୍ଥିର HINSTANCE hLibintlDLL = ଖାଲି;
ଅକ୍ଷର *(*dyn_libintl_gettext)(ଅଚଳ ଅକ୍ଷର *) = null_libintl_gettext;
ଅକ୍ଷର *(*dyn_libintl_ngettext)(ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *, unsigned long n)
						= null_libintl_ngettext;
ଅକ୍ଷର *(*dyn_libintl_textdomain)(ଅଚଳ ଅକ୍ଷର *) = null_libintl_textdomain;
ଅକ୍ଷର *(*dyn_libintl_bindtextdomain)(ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *)
						= null_libintl_bindtextdomain;
ଅକ୍ଷର *(*dyn_libintl_bind_textdomain_codeset)(ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *)
				       = null_libintl_bind_textdomain_codeset;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dyn_libintl_wputenv)(ଅଚଳ wchar_t *) = null_libintl_wputenv;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dyn_libintl_nl_ସନ୍ଦେଶ_cat_cntr = ଖାଲି;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dyn_libintl_init(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ସ୍ଥିର ସଂରଚନା
    {
	ଅକ୍ଷର	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{ଖାଲି, ଖାଲି}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    ଯଦି (hLibintlDLL != ଖାଲି)
	ଫେରନ୍ତୁ 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vimLoadLib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    ଯଦି (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    ଯଦି (!hLibintlDLL)
	hLibintlDLL = vimLoadLib(GETTEXT_DLL_ALT2);
# endif
    ଯଦି (!hLibintlDLL)
    {
	ଯଦି (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_load_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	ଫେରନ୍ତୁ 0;
    }
    ପାଇଁ (i = 0; libintl_entry[i].name != ଖାଲି
					 && libintl_entry[i].ptr != ଖାଲି; ++i)
    {
	ଯଦି ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == ଖାଲି)
	{
	    dyn_libintl_end();
	    ଯଦି (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_load_library_function_str), libintl_entry[i].name);
		verbose_leave();
	    }
	    ଫେରନ୍ତୁ 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (ଅକ୍ଷର *(*)(ଅଚଳ ଅକ୍ଷର *, ଅଚଳ ଅକ୍ଷର *))
			GetProcAddress(hLibintlDLL, "bind_textdomain_codeset");
    ଯଦି (dyn_libintl_bind_textdomain_codeset == ଖାଲି)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = find_imported_module_by_funcname(hLibintlDLL, "getenv");
    ଯଦି (hmsvcrt != ଖାଲି)
	dyn_libintl_wputenv = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*)(ଅଚଳ wchar_t *))
					GetProcAddress(hmsvcrt, "_wputenv");
    ଯଦି (dyn_libintl_wputenv == ଖାଲି || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_ସନ୍ଦେଶ_cat_cntr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)GetProcAddress(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    ଫେରନ୍ତୁ 1;
}

    ଶୂନ୍ୟ
dyn_libintl_end(ଶୂନ୍ୟ)
{
    ଯଦି (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ଖାଲି;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_ସନ୍ଦେଶ_cat_cntr	= ଖାଲି;
}

    ସ୍ଥିର ଅକ୍ଷର *
null_libintl_gettext(ଅଚଳ ଅକ୍ଷର *msgid)
{
    ଫେରନ୍ତୁ (ଅକ୍ଷର*)msgid;
}

    ସ୍ଥିର ଅକ୍ଷର *
null_libintl_ngettext(
	ଅଚଳ ଅକ୍ଷର *msgid,
	ଅଚଳ ଅକ୍ଷର *msgid_plural,
	unsigned long n)
{
    ଫେରନ୍ତୁ (ଅକ୍ଷର *)(n == 1 ? msgid : msgid_plural);
}

    ସ୍ଥିର ଅକ୍ଷର *
null_libintl_bindtextdomain(
	ଅଚଳ ଅକ୍ଷର *domainname UNUSED,
	ଅଚଳ ଅକ୍ଷର *dirname UNUSED)
{
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ଅକ୍ଷର *
null_libintl_bind_textdomain_codeset(
	ଅଚଳ ଅକ୍ଷର *domainname UNUSED,
	ଅଚଳ ଅକ୍ଷର *codeset UNUSED)
{
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ଅକ୍ଷର *
null_libintl_textdomain(ଅଚଳ ଅକ୍ଷର *domainname UNUSED)
{
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
null_libintl_wputenv(ଅଚଳ wchar_t *envstring UNUSED)
{
    ଫେରନ୍ତୁ 0;
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
    ସ୍ଥିର BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    ଯଦି (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (!Lookup_Privilege_ମୂଲ୍ୟ(ଖାଲି, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	ଫେରନ୍ତୁ FALSE;
    }

    tokenPrivileges.Privilege_ଗଣନା	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    ଯଦି (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, ଖାଲି, ଖାଲି))
    {
	CloseHandle(hToken);
	ଫେରନ୍ତୁ FALSE;
    }

    ଯଦି (GetLastError() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	ଫେରନ୍ତୁ FALSE;
    }

    CloseHandle(hToken);
    ଫେରନ୍ତୁ TRUE;
}
#endif

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    ଶୂନ୍ୟ
PlatformId(ଶୂନ୍ୟ)
{
#ifdef FEAT_EVAL
    DWORD major = (win_version >> 24) & 0xFF;
    DWORD minor = (win_version >> 16) & 0xFF;
    vim_snprintf(windowsVersion, ଆକାର(windowsVersion), "%d.%d", major, minor);
#endif

#ifdef HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    ଯଦି (!win32_enable_privilege(SE_SECURITY_NAME))
	ଫେରନ୍ତୁ;
#endif
}

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SHIFT  (SHIFT_PRESSED)
# define CTRL   (ଡାହାଣ_CTRL_PRESSED | ବାମ_CTRL_PRESSED)
# define ALT    (ଡାହାଣ_ALT_PRESSED  | ବାମ_ALT_PRESSED)
# define ALT_GR (ଡାହାଣ_ALT_PRESSED  | ବାମ_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

ସ୍ଥିର ଅଚଳ ସଂରଚନା
{
    WORD    w_Virt_ଚାବି;
    BOOL    f_Ansi_ଚାବି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    chAlone;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    chShift;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    chCtrl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    chAlt;
} Virt_ଚାବି_Map[] =
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
    { VK_ବାମ,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_ଡାହାଣ,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_END,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_DOWN,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_NEXT,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_ଭର୍ତ୍ତି,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_BACK,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# ଯଦି 0
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win32_kbd_patch_ଚାବି(
    ଚାବି_EVENT_RECORD *pker)
{
    UINT uMods = pker->dw_Control_ଚାବି_State;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_iIsDead = 0;
    ସ୍ଥିର WORD awAnsiCode[2];
    ସ୍ଥିର BYTE abKeystate[256];

    ଯଦି (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[1];
	s_iIsDead = 0;
	ଫେରନ୍ତୁ 1;
    }

    // check if it already has a valid unicode character.
    ଯଦି (pker->uChar.UnicodeChar != 0)
	ଫେରନ୍ତୁ 1;

    CLEAR_FIELD(abKeystate);

    // Clear any pending dead keys
    ToUnicode(VK_SPACE, Map_Virtual_ଚାବି(VK_SPACE, 0), abKeystate, awAnsiCode, 2, 0);

    ଯଦି (uMods & SHIFT_PRESSED)
	abKeystate[VK_SHIFT] = 0x80;
    ଯଦି (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    ଯଦି ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ToUnicode(pker->w_Virtual_ଚାବି_Code, pker->wVirtualScanCode,
			abKeystate, awAnsiCode, 2, 0);

    ଯଦି (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) awAnsiCode[0];

    ଫେରନ୍ତୁ s_iIsDead;
}

ସ୍ଥିର BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    ସ୍ଥିର BOOL
decode_ଚାବି_event(
    ଚାବି_EVENT_RECORD	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*pmodifiers,
    BOOL		fDoPost UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା nModifs = pker->dw_Control_ଚାବି_State & (SHIFT | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    ଯଦି (!pker->b_ଚାବି_Down)
	ଫେରନ୍ତୁ FALSE;

    // ignore some keystrokes
    ବାଛ (pker->w_Virtual_ଚାବି_Code)
    {
    // modifiers
    ସ୍ଥିତି VK_SHIFT:
    ସ୍ଥିତି VK_CONTROL:
    ସ୍ଥିତି VK_MENU:   // Alt key
	ଫେରନ୍ତୁ FALSE;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ଭାଙ୍ଗ;
    }

    // Shift-TAB
    ଯଦି (pker->w_Virtual_ଚାବି_Code == VK_TAB && (nModifs & SHIFT_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	ଫେରନ୍ତୁ TRUE;
    }

    ପାଇଁ (i = ARRAY_LENGTH(Virt_ଚାବି_Map); --i >= 0; )
    {
	ଯଦି (Virt_ଚାବି_Map[i].w_Virt_ଚାବି == pker->w_Virtual_ଚାବି_Code)
	{
	    *pch = Virt_ଚାବି_Map[i].chAlone;
	    ଯଦି ((nModifs & SHIFT) != 0)
		*pch = Virt_ଚାବି_Map[i].chShift;
	    ଅନ୍ୟଥା ଯଦି ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_ଚାବି_Map[i].chCtrl;
	    ଅନ୍ୟଥା ଯଦି ((nModifs & ALT) != 0)
		*pch = Virt_ଚାବି_Map[i].chAlt;

	    ଯଦି (*pch != 0)
	    {
		ଯଦି (Virt_ଚାବି_Map[i].f_Ansi_ଚାବି)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    ଯଦି (pmodifiers)
		    {
			ଯଦି (pker->w_Virtual_ଚାବି_Code >= VK_F1
			    && pker->w_Virtual_ଚାବି_Code <= VK_F12)
			{
			    ଯଦି ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_MASK_ALT;
				ଯଦି ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_ଚାବି_Map[i].chAlone;
			    }
			    ଯଦି ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				ଯଦି ((nModifs & SHIFT) == 0)
				    *pch2 = Virt_ଚାବି_Map[i].chAlone;
			    }
			}
			ଅନ୍ୟଥା ଯଦି (pker->w_Virtual_ଚାବି_Code >= VK_END
				&& pker->w_Virtual_ଚାବି_Code <= VK_DOWN)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_ଚାବି_Map[i].chAlone;
			    ଯଦି ((nModifs & SHIFT) != 0
						    && (nModifs & ~SHIFT) == 0)
			    {
				*pch2 = Virt_ଚାବି_Map[i].chShift;
			    }
			    ଯଦି ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_ଚାବି_Map[i].chCtrl;
				ଯଦି (pker->w_Virtual_ଚାବି_Code == VK_UP
				    || pker->w_Virtual_ଚାବି_Code == VK_DOWN)
				{
				    *pmodifiers |= MOD_MASK_CTRL;
				    *pch2 = Virt_ଚାବି_Map[i].chAlone;
				}
			    }
			    ଯଦି ((nModifs & SHIFT) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_MASK_CTRL;
				*pch2 = Virt_ଚାବି_Map[i].chShift;
			    }
			    ଯଦି ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_ଚାବି_Map[i].chAlt;
				*pmodifiers |= MOD_MASK_ALT;
				ଯଦି ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_ଚାବି_Map[i].chAlone;
				}
				ଅନ୍ୟଥା ଯଦି ((nModifs & SHIFT) != 0)
				{
				    *pch2 = Virt_ଚାବି_Map[i].chShift;
				}
				ଅନ୍ୟଥା ଯଦି ((nModifs & CTRL) != 0)
				{
				    ଯଦି (pker->w_Virtual_ଚାବି_Code == VK_UP
					|| pker->w_Virtual_ଚାବି_Code == VK_DOWN)
				    {
					*pmodifiers |= MOD_MASK_CTRL;
					*pch2 = Virt_ଚାବି_Map[i].chAlone;
				    }
				    ଅନ୍ୟଥା
				    {
					*pch2 = Virt_ଚାବି_Map[i].chCtrl;
				    }
				}
			    }
			}
			ଅନ୍ୟଥା ଯଦି (pker->w_Virtual_ଚାବି_Code == VK_ଭର୍ତ୍ତି
					&& (nModifs & SHIFT) != 0
					&& (nModifs & ~SHIFT) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_ଚାବି_Map[i].chShift;
			}
			ଅନ୍ୟଥା
			{
			    *pch2 = Virt_ଚାବି_Map[i].chAlone;
			    ଯଦି ((nModifs & SHIFT) != 0)
				*pmodifiers |= MOD_MASK_SHIFT;
			    ଯଦି ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_MASK_CTRL;
			    ଯଦି ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_MASK_ALT;
			}
		    }
		}

		ଫେରନ୍ତୁ TRUE;
	    }
	}
    }

    i = win32_kbd_patch_ଚାବି(pker);

    ଯଦି (i < 0)
	*pch = NUL;
    ଅନ୍ୟଥା
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	ଯଦି (pmodifiers != ଖାଲି)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    ଯଦି ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_MASK_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    ଯଦି ((nModifs & SHIFT) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_MASK_SHIFT;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    ଯଦି ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_MASK_CTRL;
	}
    }

    ଫେରନ୍ତୁ (*pch != NUL);
}

# ଯଦି defined(FEAT_EVAL)
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
encode_ଚାବି_event(dict_T *args, ନିବେଶ_RECORD *ir)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_dwMods = 0;

    char_u *action = dict_get_string(args, "event", TRUE);
    ଯଦି (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_ଚାବି_Down = STRICMP(action, "keydown") == 0;
	WORD vkCode = dict_get_ସଂଖ୍ୟା_def(args, "keycode", 0);
	ଯଦି (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    ଫେରନ୍ତୁ FALSE;
	}

	ir->EventType = ଚାବି_EVENT;
	ଚାବି_EVENT_RECORD ker;
	ZeroMemory(&ker, ଆକାର(ker));
	ker.b_ଚାବି_Down = is_ଚାବି_Down;
	ker.w_Repeat_ଗଣନା = 1;
	ker.wVirtualScanCode = 0;
	ker.dw_Control_ଚାବି_State = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mods = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	ଯଦି (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    ଯଦି (mods & MOD_MASK_SHIFT)
		ker.dw_Control_ଚାବି_State |= SHIFT_PRESSED;
	    ଯଦି (mods & MOD_MASK_CTRL)
		ker.dw_Control_ଚାବି_State |= ବାମ_CTRL_PRESSED;
	    ଯଦି (mods & MOD_MASK_ALT)
		ker.dw_Control_ଚାବି_State |= ବାମ_ALT_PRESSED;
	}

	ଯଦି (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT)
	{
	    ଯଦି (is_ଚାବି_Down)
		s_dwMods |= SHIFT_PRESSED;
	    ଅନ୍ୟଥା
		s_dwMods &= ~SHIFT_PRESSED;
	}
	ଅନ୍ୟଥା ଯଦି (vkCode == VK_LCONTROL || vkCode == VK_CONTROL)
	{
	    ଯଦି (is_ଚାବି_Down)
		s_dwMods |= ବାମ_CTRL_PRESSED;
	    ଅନ୍ୟଥା
		s_dwMods &= ~ବାମ_CTRL_PRESSED;
	}
	ଅନ୍ୟଥା ଯଦି (vkCode == VK_RCONTROL)
	{
	    ଯଦି (is_ଚାବି_Down)
		s_dwMods |= ଡାହାଣ_CTRL_PRESSED;
	    ଅନ୍ୟଥା
		s_dwMods &= ~ଡାହାଣ_CTRL_PRESSED;
	}
	ଅନ୍ୟଥା ଯଦି (vkCode == VK_LMENU || vkCode == VK_MENU)
	{
	    ଯଦି (is_ଚାବି_Down)
		s_dwMods |= ବାମ_ALT_PRESSED;
	    ଅନ୍ୟଥା
		s_dwMods &= ~ବାମ_ALT_PRESSED;
	}
	ଅନ୍ୟଥା ଯଦି (vkCode == VK_RMENU)
	{
	    ଯଦି (is_ଚାବି_Down)
		s_dwMods |= ଡାହାଣ_ALT_PRESSED;
	    ଅନ୍ୟଥା
		s_dwMods &= ~ଡାହାଣ_ALT_PRESSED;
	}
	ker.dw_Control_ଚାବି_State |= s_dwMods;
	ker.w_Virtual_ଚାବି_Code = vkCode;
	ker.uChar.UnicodeChar = 0;
	ir->Event.ଚାବି_Event_2 = ker;
	vim_free(action);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (action == ଖାଲି)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	ଅନ୍ୟଥା
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "event", action);
	    vim_free(action);
	}
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}
# endif  // FEAT_EVAL
#endif // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_fMouseAvail = FALSE;   // mouse present
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_fMouseActive = FALSE;  // mouse enabled
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_nMouseClick = -1;	    // mouse status
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_xMouse;		    // mouse x coordinate
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_yMouse;		    // mouse y coordinate
ସ୍ଥିର DWORD g_cmodein = 0;	    // Original console input mode
ସ୍ଥିର DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    ଶୂନ୍ୟ
mch_setmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା on)
{
    DWORD cmodein;

# ifdef VIMDLL
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ;
# endif
    ଯଦି (!g_fMouseAvail)
	ଫେରନ୍ତୁ;

    g_fMouseActive = on;
    GetConsoleMode(g_hConIn, &cmodein);

    ଯଦି (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_ନିବେଶ;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    ଅନ୍ୟଥା
    {
	cmodein &= ~ENABLE_MOUSE_ନିବେଶ;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }

    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
}


# ଯଦି defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    ଶୂନ୍ୟ
mch_bevalterm_changed(ଶୂନ୍ୟ)
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
    ସ୍ଥିର ଶୂନ୍ୟ
decode_mouse_wheel(MOUSE_EVENT_RECORD *pmer)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lcol = g_xMouse;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lrow = g_yMouse;
    win_T *wp = mouse_find_win(&lrow, &lcol, FIND_POPUP);
    ଯଦି (wp != ଖାଲି && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_FIELD(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	ଯଦି (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ବାମ : MSCR_ଡାହାଣ;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	ଅନ୍ୟଥା
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
    ଯଦି (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    ଅନ୍ୟଥା
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    ଯଦି (pmer->dw_Control_ଚାବି_State & SHIFT_PRESSED)
	modifiers |= MOD_MASK_SHIFT; // MOUSE_SHIFT;
    ଯଦି (pmer->dw_Control_ଚାବି_State & (ଡାହାଣ_CTRL_PRESSED | ବାମ_CTRL_PRESSED))
	modifiers |= MOD_MASK_CTRL; // MOUSE_CTRL;
    ଯଦି (pmer->dw_Control_ଚାବି_State & (ଡାହାଣ_ALT_PRESSED  | ବାମ_ALT_PRESSED))
	modifiers |= MOD_MASK_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    ଫେରନ୍ତୁ;
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
    ସ୍ଥିର BOOL
decode_mouse_event(
    MOUSE_EVENT_RECORD *pmer)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_nOldButton = -1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_nOldMouseClick = -1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_xOldMouse = -1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_yOldMouse = -1;
    ସ୍ଥିର linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_old_topfill = 0;
# endif
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_cClicks = 1;
    ସ୍ଥିର BOOL s_fReleased = TRUE;
    ସ୍ଥିର DWORD s_dwLastClickTime = 0;
    ସ୍ଥିର BOOL s_f_Next_Is_ମଧ୍ୟ = FALSE;

    ସ୍ଥିର DWORD cButtons = 0;	// number of buttons supported

    ଅଚଳ DWORD ବାମ_2 = FROM_ବାମ_1_ST_BUTTON_PRESSED;
    ଅଚଳ DWORD ମଧ୍ୟ = FROM_ବାମ_2_ND_BUTTON_PRESSED;
    ଅଚଳ DWORD ଡାହାଣ_2 = RIGHTMOST_BUTTON_PRESSED;
    ଅଚଳ DWORD ବାମ_ଡାହାଣ = ବାମ_2 | ଡାହାଣ_2;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nButton;

    ଯଦି (cButtons == 0 && !Get_ସଂଖ୍ୟା_Of_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    ଯଦି (!g_fMouseAvail || !g_fMouseActive)
    {
	g_nMouseClick = -1;
	ଫେରନ୍ତୁ FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    ଯଦି (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	ଫେରନ୍ତୁ FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    ଯଦି (g_nMouseClick != -1)
	ଫେରନ୍ତୁ TRUE;

    ଯଦି (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	ଫେରନ୍ତୁ TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    ଯଦି (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	ଯଦି (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    ଫେରନ୍ତୁ FALSE;
    }

    // If no buttons are pressed...
    ଯଦି ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	ଯଦି (s_fReleased)
	{
# ifdef FEAT_BEVAL_TERM
	    // do return mouse move events when we want them
	    ଯଦି (p_bevalterm)
		nButton = MOUSE_DRAG;
	    ଅନ୍ୟଥା
# endif
		ଫେରନ୍ତୁ FALSE;
	}

	s_fReleased = TRUE;
    }
    ଅନ୍ୟଥା    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	ଯଦି (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & ବାମ_ଡାହାଣ);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    ଯଦି (dwLR == ବାମ_2 || dwLR == ଡାହାଣ_2)
	    {
		ପାଇଁ (;;)
		{
		    // wait a short time for next input event
		    ଯଦି (WaitForSingleObject(g_hConIn, p_mouset / 3)
							     != WAIT_OBJECT_0)
			ଭାଙ୍ଗ;
		    ଅନ୍ୟଥା
		    {
			DWORD cRecords = 0;
			ନିବେଶ_RECORD ir;
			MOUSE_EVENT_RECORD* pmer2 = &ir.Event.MouseEvent;

			peek_console_ନିବେଶ(g_hConIn, &ir, 1, &cRecords);

			ଯଦି (cRecords == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & ବାମ_ଡାହାଣ))
			    ଭାଙ୍ଗ;
			ଅନ୍ୟଥା
			{
			    ଯଦି (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_ନିବେଶ(g_hConIn, &ir, 1, &cRecords);

				ଫେରନ୍ତୁ decode_mouse_event(pmer2);
			    }
			    ଅନ୍ୟଥା ଯଦି (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_ନିବେଶ(g_hConIn, &ir, 1, &cRecords);

				// are there any more mouse events in queue?
				peek_console_ନିବେଶ(g_hConIn, &ir, 1, &cRecords);

				ଯଦି (cRecords==0 || ir.EventType != MOUSE_EVENT)
				    ଭାଙ୍ଗ;
			    }
			    ଅନ୍ୟଥା
				ଭାଙ୍ଗ;
			}
		    }
		}
	    }
	}

	ଯଦି (s_f_Next_Is_ମଧ୍ୟ)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_ମଧ୍ୟ;
	    s_f_Next_Is_ମଧ୍ୟ = FALSE;
	}
	ଅନ୍ୟଥା ଯଦି (cButtons == 2	&&
	    ((pmer->dwButtonState & ବାମ_ଡାହାଣ) == ବାମ_ଡାହାଣ))
	{
	    nButton = MOUSE_ମଧ୍ୟ;

	    ଯଦି (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_Next_Is_ମଧ୍ୟ = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	ଅନ୍ୟଥା ଯଦି ((pmer->dwButtonState & ବାମ_2) == ବାମ_2)
	    nButton = MOUSE_ବାମ;
	ଅନ୍ୟଥା ଯଦି ((pmer->dwButtonState & ମଧ୍ୟ) == ମଧ୍ୟ)
	    nButton = MOUSE_ମଧ୍ୟ;
	ଅନ୍ୟଥା ଯଦି ((pmer->dwButtonState & ଡାହାଣ_2) == ଡାହାଣ_2)
	    nButton = MOUSE_ଡାହାଣ;

	ଯଦି (! s_fReleased && ! s_f_Next_Is_ମଧ୍ୟ
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    ଫେରନ୍ତୁ FALSE;

	s_fReleased = s_f_Next_Is_ମଧ୍ୟ;
    }

    ଯଦି (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	ଯଦି (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dwCurrentTime = Get_Tick_ଗଣନା();

	    ଯଦି (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(dwCurrentTime - s_dwLastClickTime) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    ଅନ୍ୟଥା ଯଦି (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dwLastClickTime = dwCurrentTime;
	}
    }
    ଅନ୍ୟଥା ଯଦି (pmer->dwEventFlags == MOUSE_MOVED)
    {
	ଯଦି (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    ଯଦି (nButton == -1)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    ଯଦି (pmer->dw_Control_ଚାବି_State & SHIFT_PRESSED)
	g_nMouseClick |= MOUSE_SHIFT;
    ଯଦି (pmer->dw_Control_ଚାବି_State & (ଡାହାଣ_CTRL_PRESSED | ବାମ_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    ଯଦି (pmer->dw_Control_ଚାବି_State & (ଡାହାଣ_ALT_PRESSED  | ବାମ_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    ଯଦି (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	SET_ଅଙ୍କ_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    ଯଦି (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	ଫେରନ୍ତୁ FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    ଫେରନ୍ତୁ TRUE;
}

# ifdef FEAT_EVAL
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
encode_mouse_event(dict_T *args, ନିବେଶ_RECORD *ir)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		button;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		repeated_click;
    int_u	mods = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		move;

    ଯଦି (!dict_has_ଚାବି(args, "row") || !dict_has_ଚାବି(args, "col"))
	ଫେରନ୍ତୁ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_bool(args, "move", FALSE);
    ଯଦି (!move && (!dict_has_ଚାବି(args, "button")
	    || !dict_has_ଚାବି(args, "multiclick")
	    || !dict_has_ଚାବି(args, "modifiers")))
	ଫେରନ୍ତୁ FALSE;

    row = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "row") - 1;
    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_RECORD mer;
    ZeroMemory(&mer, ଆକାର(mer));
    mer.dwMousePosition.X  = col;
    mer.dwMousePosition.Y  = row;

    ଯଦି (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    ଅନ୍ୟଥା
    {
	button = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "button");
	repeated_click = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "multiclick");
	mods = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	ବାଛ (button)
	{
	    ସ୍ଥିତି MOUSE_ବାମ:
		mer.dwButtonState = FROM_ବାମ_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MOUSE_ମଧ୍ୟ:
		mer.dwButtonState = FROM_ବାମ_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MOUSE_ଡାହାଣ:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    ସ୍ଥିତି MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MOUSE_X1:
		mer.dwButtonState = FROM_ବାମ_3_RD_BUTTON_PRESSED;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MOUSE_X2:
		mer.dwButtonState = FROM_ବାମ_4_TH_BUTTON_PRESSED;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		semsg(_(e_invalid_argument_str), "button");
		ଫେରନ୍ତୁ FALSE;
	}
    }

    mer.dw_Control_ଚାବି_State = 0;
    ଯଦି (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	ଯଦି (mods & MOUSE_SHIFT)
	    mer.dw_Control_ଚାବି_State |= SHIFT_PRESSED;
	ଯଦି (mods & MOUSE_CTRL)
	    mer.dw_Control_ଚାବି_State |= ବାମ_CTRL_PRESSED;
	ଯଦି (mods & MOUSE_ALT)
	    mer.dw_Control_ଚାବି_State |= ବାମ_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    ଫେରନ୍ତୁ TRUE;
}
# endif  // FEAT_EVAL

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_ନିବେଶ_record_buffer(ନିବେଶ_RECORD* irEvents, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nLength)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା				n_ଗଣନା = 0;
    ନିବେଶ_record_buffer_ଗଣ୍ଠି_T	*event_ଗଣ୍ଠି;

    ଯେତେବେଳେ (n_ଗଣନା < nLength)
    {
	event_ଗଣ୍ଠି = alloc(ଆକାର(ନିବେଶ_record_buffer_ଗଣ୍ଠି_T));
	ଯଦି (event_ଗଣ୍ଠି == ଖାଲି)
	    ଫେରନ୍ତୁ -1;

	event_ଗଣ୍ଠି->ir = irEvents[n_ଗଣନା++];
	event_ଗଣ୍ଠି->next = ଖାଲି;

	ନିବେଶ_record_buffer.length++;
	ଯଦି (ନିବେଶ_record_buffer.tail == ଖାଲି)
	{
	    ନିବେଶ_record_buffer.head = event_ଗଣ୍ଠି;
	    ନିବେଶ_record_buffer.tail = event_ଗଣ୍ଠି;
	}
	ଅନ୍ୟଥା
	{
	    ନିବେଶ_record_buffer.tail->next = event_ଗଣ୍ଠି;
	    ନିବେଶ_record_buffer.tail = ନିବେଶ_record_buffer.tail->next;
	}
    }
    ଫେରନ୍ତୁ n_ଗଣନା;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_ନିବେଶ_record_buffer(ନିବେଶ_RECORD* irEvents, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nMaxLength)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_ଗଣନା = 0;
    ଯେତେବେଳେ (n_ଗଣନା < nMaxLength && ନିବେଶ_record_buffer.head != ଖାଲି)
    {
	ନିବେଶ_record_buffer.length--;
	ନିବେଶ_record_buffer_ଗଣ୍ଠି_T *କାଢ଼_head = ନିବେଶ_record_buffer.head;
	irEvents[n_ଗଣନା++] = କାଢ଼_head->ir;
	ନିବେଶ_record_buffer.head = କାଢ଼_head->next;
	vim_free(କାଢ଼_head);
	ଯଦି (ନିବେଶ_record_buffer.length == 0)
	    ନିବେଶ_record_buffer.tail = ଖାଲି;
    }
    ଫେରନ୍ତୁ n_ଗଣନା;
}
#ଅନ୍ୟଥା  // FEAT_GUI_MSWIN && !VIMDLL
    ଶୂନ୍ୟ
mch_setmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା on UNUSED)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_mswin_event(char_u *event, dict_T *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lpEventsWritten = 0;

# ଯଦି defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ test_gui_w32_sendevent(event, args);
# endif

# ଯଦି defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    ନିବେଶ_RECORD ir;
    BOOL ନିବେଶ_encoded = FALSE;
    BOOL execute = FALSE;
    ଯଦି (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(args, "execute", FALSE);
	ଯଦି (dict_has_ଚାବି(args, "event"))
	    ନିବେଶ_encoded = encode_ଚାବି_event(args, &ir);
	ଅନ୍ୟଥା ଯଦି (!execute)
	{
	    semsg(_(e_missing_argument_str), "event");
	    ଫେରନ୍ତୁ FALSE;
	}
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	ନିବେଶ_encoded = encode_mouse_event(args, &ir);
    }
    ଅନ୍ୟଥା
    {
	semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "event", event);
	ଫେରନ୍ତୁ FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    ଯଦି (ନିବେଶ_encoded)
    {
	ଯଦି ((lpEventsWritten = write_ନିବେଶ_record_buffer(&ir, 1)) < 0)
	{
	    semsg(_(e_out_of_memory), "event");
	    ଫେରନ୍ତୁ FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    ଯଦି (execute)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ସନ୍ଦେଶ_scroll = ସନ୍ଦେଶ_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	ସନ୍ଦେଶ_scroll = FALSE;
	ch_log(ଖାଲି, "test_mswin_event() executing");
	exec_normal(TRUE, TRUE, TRUE);
	ସନ୍ଦେଶ_scroll |= save_ସନ୍ଦେଶ_scroll;
    }

# endif
    ଫେରନ୍ତୁ lpEventsWritten;
}
#endif // FEAT_EVAL

#ଯଦି defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mch_set_cursor_shape(ପୂର୍ଣ୍ଣସଂଖ୍ୟା thickness)
{
    ଯଦି (vtp_working)
    {
	ଯଦି (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    ଯଦି (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    ଅନ୍ୟଥା
		vtp_printf("\033[0 q");	// default
	}
    }
    ଅନ୍ୟଥା
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dwSize = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	SetConsoleCursorInfo(g_hConOut, &ConsoleCursorInfo);
	ଯଦି (s_cursor_visible)
	    SetConsoleCursorPosition(g_hConOut, g_coord);
    }
}

    ଶୂନ୍ୟ
mch_update_cursor(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		thickness;

# ifdef VIMDLL
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    ଯଦି (shape_table[idx].shape == SHAPE_BLOCK)
	thickness = 99;	// 100 doesn't work on W95
    ଅନ୍ୟଥା
	thickness = shape_table[idx].percentage;
    mch_set_cursor_shape(thickness);
}
#endif

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_focus_event(ନିବେଶ_RECORD ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
    ui_focus_change((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)g_fJustGotFocus);
}

ସ୍ଥିର ଶୂନ୍ୟ ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
WaitForChar(long msec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_ନିବେଶ)
{
    DWORD	    dwNow = 0, dwEndTime = 0;
    ନିବେଶ_RECORD    ir;
    DWORD	    cRecords;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    ଯଦି (msec > 0)
	// Wait until the specified time has elapsed.
	dwEndTime = Get_Tick_ଗଣନା() + msec;
    ଅନ୍ୟଥା ଯଦି (msec < 0)
	// Wait forever.
	dwEndTime = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    ପାଇଁ (;;)
    {
	// Only process messages when waiting.
	ଯଦି (msec != 0)
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

	ଯଦି (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_ନିବେଶ && ନିବେଶ_available())
# endif
	   )
	    ଫେରନ୍ତୁ TRUE;

	ଯଦି (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_ଗଣନା();
	    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(dwNow - dwEndTime) >= 0)
		ଭାଙ୍ଗ;
	}
	ଯଦି (msec != 0)
	{
	    DWORD dwWaitTime = dwEndTime - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    ଯଦି (dwWaitTime > 11)
		dwWaitTime = 11;

# ifdef FEAT_MZSCHEME
	    ଯଦି (mzthreads_allowed() && p_mzq > 0 && (long)dwWaitTime > p_mzq)
		dwWaitTime = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    ଯଦି (dwWaitTime > 10)
	    {
		long	due_time;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_time = check_due_timer();
		ଯଦି (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    ଫେରନ୍ତୁ FALSE;
		}
		ଯଦି (due_time > 0 && dwWaitTime > (DWORD)due_time)
		    dwWaitTime = due_time;
	    }
# endif
	    ଯଦି (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    ସନ୍ଦେଶ_wait_for_multiple_objects(1, &g_hConIn, FALSE,
				  dwWaitTime, QS_SENDMESSAGE) != WAIT_OBJECT_0
# ଅନ୍ୟଥା
		    wait_for_single_object(g_hConIn, dwWaitTime)
							      != WAIT_OBJECT_0
# endif
		    )
		ଜାରି;
	}

	cRecords = 0;
	peek_console_ନିବେଶ(g_hConIn, &ir, 1, &cRecords);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	ଯଦି (State == MODE_CMDLINE && ସନ୍ଦେଶ_row == Rows - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_BUFFER_INFO csbi;

	    ଯଦି (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	    {
		ଯଦି (csbi.dwCursorPosition.Y != ସନ୍ଦେଶ_row)
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

	ଯଦି (cRecords > 0)
	{
	    ଯଦି (ir.EventType == ଚାବି_EVENT && ir.Event.ଚାବି_Event_2.b_ଚାବି_Down)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		ଯଦି (ir.Event.ଚାବି_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.ଚାବି_Event_2.w_Virtual_ଚାବି_Code == 13)
		{
		    read_console_ନିବେଶ(g_hConIn, &ir, 1, &cRecords);
		    ଜାରି;
		}
# endif
		ଯଦି (decode_ଚାବି_event(&ir.Event.ଚାବି_Event_2, &ch, &ch2,
								 ଖାଲି, FALSE))
		    ଫେରନ୍ତୁ TRUE;
	    }

	    read_console_ନିବେଶ(g_hConIn, &ir, 1, &cRecords);

	    ଯଦି (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    ଅନ୍ୟଥା ଯଦି (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    {
		COORD dwSize = ir.Event.WindowBufferSizeEvent.dwSize;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		ଯଦି (dwSize.X != Columns || dwSize.Y != Rows)
		{
		    CONSOLE_SCREEN_BUFFER_INFO csbi;
		    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
		    dwSize.X = csbi.srWindow.ଡାହାଣ_3 - csbi.srWindow.ବାମ_3 + 1;
		    dwSize.Y = csbi.srWindow.Bottom - csbi.srWindow.ଶୀର୍ଷ_2 + 1;
		    ଯଦି (dwSize.X != Columns || dwSize.Y != Rows)
		    {
			ResizeConBuf(g_hConOut, dwSize);
			shell_resized();
		    }
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		ଫେରନ୍ତୁ TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (msec == 0)
	    ଭାଙ୍ଗ;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    ଯଦି (ନିବେଶ_available())
	ଫେରନ୍ତୁ TRUE;
# endif

    ଫେରନ୍ତୁ FALSE;
}

/*
 * return non-zero if a character is available
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_char_avail(ଶୂନ୍ୟ)
{
# ifdef VIMDLL
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ TRUE;
# endif
    ଫେରନ୍ତୁ WaitForChar(0L, FALSE);
}

# ଯଦି defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_check_messages(ଶୂନ୍ୟ)
{
#  ifdef VIMDLL
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ TRUE;
#  endif
    ଫେରନ୍ତୁ WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
create_conin(ଶୂନ୍ୟ)
{
    g_hConIn =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES) ଖାଲି,
			OPEN_EXISTING, 0, (HANDLE)ଖାଲି);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    ସ୍ଥିର WCHAR
tgetch(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    ପାଇଁ (;;)
    {
	ନିବେଶ_RECORD ir;
	DWORD cRecords = 0;

# ifdef FEAT_CLIENTSERVER
	(ଶୂନ୍ୟ)WaitForChar(-1L, FALSE);
	ଯଦି (ନିବେଶ_available())
	    ଫେରନ୍ତୁ 0;
	ଯଦି (g_nMouseClick != -1)
	    ଫେରନ୍ତୁ 0;
# endif
	ଯଦି (read_console_ନିବେଶ(g_hConIn, &ir, 1, &cRecords) == 0)
	{
	    ଯଦି (did_create_conin)
		read_error_exit();
	    create_conin();
	    ଜାରି;
	}

	ଯଦି (ir.EventType == ଚାବି_EVENT)
	{
	    ଯଦି (decode_ଚାବି_event(&ir.Event.ଚାବି_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		ଫେରନ୍ତୁ ch;
	}
	ଅନ୍ୟଥା ଯଦି (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	ଅନ୍ୟଥା ଯଦି (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	    shell_resized();
	ଅନ୍ୟଥା ଯଦି (ir.EventType == MOUSE_EVENT)
	{
	    ଯଦି (decode_mouse_event(&ir.Event.MouseEvent))
		ଫେରନ୍ତୁ 0;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_inchar(
    char_u	*buf UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen UNUSED,
    long	time UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tb_change_cnt UNUSED)
{
#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# ଅନ୍ୟଥା
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    ସ୍ଥିର char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    typeaheadlen = 0;

# ifdef VIMDLL
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    ଯଦି (typeaheadlen > 0)
	ଯାଅ theend;

    ଯଦି (time >= 0)
    {
	ଯଦି (!WaitForChar(time, FALSE))     // no character available
	    ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା    // time == -1, wait forever
    {
	mch_set_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ଯଦି (!WaitForChar(p_ut, FALSE))
	{
	    ଯଦି (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CURSORHOLD;
		ଫେରନ୍ତୁ 3;
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
    ଯଦି (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    ଯେତେବେଳେ ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	ଯଦି (typebuf_changed(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    ଯଦି (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    ଯଦି (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		ଯଦି (modifiers > 0)
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
	    ଅନ୍ୟଥା
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
	ଅନ୍ୟଥା
	{
	    WCHAR	ch2 = NUL;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_ଚାବି(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_ଚାବିଗୁଡ଼ିକ;
	    c = may_adjust_ଚାବି_for_ctrl(modifiers, c);
	    --no_reduce_ଚାବିଗୁଡ଼ିକ;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_shift_modifier(modifiers, c);

	    ଯଦି (typebuf_changed(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (c == Ctrl_C && ctrl_c_interrupts)
	    {
# ଯଦି defined(FEAT_CLIENTSERVER)
		trash_ନିବେଶ_buf();
# endif
		got_int = TRUE;
	    }

	    ଯଦି (g_nMouseClick == -1)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n = 1;

		ଯଦି (ch2 == NUL)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    ଯଦି (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_to_enc(ch, &n);
		    ଯଦି (p != ଖାଲି)
		    {
			ପାଇଁ (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    ଯଦି (p[i] == CSI)
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
		ଅନ୍ୟଥା
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    ଯଦି (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		ଯଦି (ch2 != NUL)
		{
		    ଯଦି (c == K_NUL)
		    {
			ବାଛ (ch2)
			{
			ସ୍ଥିତି (WCHAR)'\324': // SHIFT+Insert
			ସ୍ଥିତି (WCHAR)'\325': // CTRL+Insert
			ସ୍ଥିତି (WCHAR)'\327': // SHIFT+Delete
			ସ୍ଥିତି (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    ଭାଙ୍ଗ;

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    ଭାଙ୍ଗ;
			}
		    }
		    ଅନ୍ୟଥା
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
		ଯଦି ((modifiers & MOD_MASK_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_MASK_ALT;
		}

		ଯଦି (modifiers != 0)
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
		ଯଦି (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    ଯଦି (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    ଯେତେବେଳେ (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    ଯଦି (len > 0)
    {
	buf[len] = NUL;
	ch_log(ଖାଲି, "raw key input: \"%s\"", buf);
    }
# endif
    ଫେରନ୍ତୁ len;

#ଅନ୍ୟଥା // FEAT_GUI_MSWIN && !VIMDLL
    ଫେରନ୍ତୁ 0;
#endif // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
executable_file(ଅକ୍ଷର *name, char_u **path)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    ଯଦି (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	ଫେରନ୍ତୁ FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    ଯଦି (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	ଯଦି (res == ଖାଲି)
	    res = resolve_reparse_point((char_u *)name);
	ଯଦି (res == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;
	// The path is already absolute.
	ଯଦି (path != ଖାଲି)
	    *path = res;
	ଅନ୍ୟଥା
	    vim_free(res);
    }
    ଅନ୍ୟଥା ଯଦି (path != ଖାଲି)
	*path = FullName_save((char_u *)name, FALSE);
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    ସ୍ଥିର bool
executable_prefix_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_FIND_DATAW	data;
    HANDLE		handle;
    DWORD		error;

    wn = enc_to_utf16(name, ଖାଲି);
    ଯଦି (wn == ଖାଲି)
	ଫେରନ୍ତୁ true;
    handle = FindFirstFileExW(wn, FindExInfoBasic, &data,
					    Find_Ex_ସନ୍ଧାନ_Name_Match, ଖାଲି, 0);
    error = GetLastError();
    vim_free(wn);
    ଯଦି (handle != INVALID_HANDLE_ମୂଲ୍ୟ)
    {
	FindClose(handle);
	ଫେରନ୍ତୁ true;
    }
    ଫେରନ୍ତୁ error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
executable_exists(
    ଅକ୍ଷର	*name,
    size_t	namelen,
    char_u	**path,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_path,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3];
    char_u	*p;
    string_T	pathbuf = {ଖାଲି, 0};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pathbuf_allocated = FALSE;
    string_T	pathext = {ଖାଲି, 0};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pathext_allocated = FALSE;
    char_u	*shname = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noext = FALSE;
    bool	simple_pathext = true;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ext_ଗଣନା = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;

    ଯଦି (namelen >= ଆକାର(buf))	// safety check
	ଫେରନ୍ତୁ FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    ଯଦି (strstr((ଅକ୍ଷର *)shname, "sh") != ଖାଲି &&
	!(strstr((ଅକ୍ଷର *)shname, "powershell") != ଖାଲି
				    || strstr((ଅକ୍ଷର *)shname, "pwsh") != ଖାଲି))
	noext = TRUE;

    ଯଦି (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	ଯଦି (pathext.string == ଖାଲି)
	    STR_LITERAL_SET(pathext, ".com;.exe;.bat;.cmd");
	ଅନ୍ୟଥା
	    pathext.length = STRLEN(pathext.string);

	ଯଦି (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    ଯେତେବେଳେ (*p)
	    {
		ଯଦି (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    ଯଦି (*p == '.')
			simple_pathext = false;
		    ++p;
		    ଜାରି;
		}
		e = vim_strchr(p, ';');
		ଯଦି (e == ଖାଲି)
		    e = pathext.string + pathext.length;
		plen = (size_t)(e - p);
		++ext_ଗଣନା;
		ଯଦି (*p != '.')
		    simple_pathext = false;
		ପାଇଁ (q = p + 1; q < e; ++q)
		    ଯଦି (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		ଯଦି (_strnicoll(name + namelen - plen, (ଅକ୍ଷର *)p, plen) == 0)
		{
		    noext = TRUE;
		    ଭାଙ୍ଗ;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    ଯଦି (pathext.string == ଖାଲି)
	STR_LITERAL_SET(pathext, ".");
    ଅନ୍ୟଥା ଯଦି (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.length + 3);
	ଯଦି (tmp == ଖାଲି)
	{
	    retval = FALSE;
	    ଯାଅ theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.length += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    ଯଦି (use_path && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	ଯଦି (p != ଖାଲି)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    ଯଦି (pathbuf.string == ଖାଲି)
	    {
		retval = FALSE;
		ଯାଅ theend;
	    }

	    ଯଦି (mch_getenv("NoDefaultCurrentDirectoryInExePath") == ଖାଲି &&
		    strstr((ଅକ୍ଷର *)gettail(p_sh), "cmd.exe") != ଖାଲି)
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
    ଯଦି (pathbuf.string == ଖାଲି)
	STR_LITERAL_SET(pathbuf, ".");
    p = pathbuf.string;
    ଯେତେବେଳେ (*p)
    {
	char_u	*e;
	size_t	buflen;

	ଯଦି (*p == ';') // Skip empty entry
	{
	    ++p;
	    ଜାରି;
	}
	e = vim_strchr(p, ';');
	ଯଦି (e == ଖାଲି)
	    e = pathbuf.string + pathbuf.length;

	ଯଦି (e - p + namelen + 2 > ଆକାର(buf))
	{
	    retval = FALSE;
	    ଯାଅ theend;
	}
	// A single "." that means current dir.
	ଯଦି (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	ଅନ୍ୟଥା
	{
	    buflen = vim_snprintf_safelen(
		(ଅକ୍ଷର *)buf,
		ଆକାର(buf),
		"%.*s%s%s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	ଯଦି (!noext && simple_pathext && ext_ଗଣନା >= 3 && buflen > 0
		&& buflen + 2 <= ଆକାର(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == ଖାଲି
		&& vim_strchr(gettail(buf), ':') == ଖାଲି)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_prefix_exists(buf);
	    buf[buflen] = NUL;
	    ଯଦି (!exists)
	    {
		p = e;
		ଜାରି;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	ଯେତେବେଳେ (*p)
	{
	    char_u  *e2;

	    ଯଦି (*p == ';')
	    {
		// Skip empty entry
		++p;
		ଜାରି;
	    }
	    e2 = vim_strchr(p, ';');
	    ଯଦି (e2 == ଖାଲି)
		e2 = pathext.string + pathext.length;

	    ଯଦି (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		ଯଦି (e2 - p + buflen + 1 > ଆକାର(buf))
		{
		    retval = FALSE;
		    ଯାଅ theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    ଯଦି (executable_file((ଅକ୍ଷର *)buf, path))
	    {
		retval = TRUE;
		ଯାଅ theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    ଯଦି (pathbuf_allocated)
	ମୁକ୍ତ(pathbuf.string);
    ଯଦି (pathext_allocated)
	ମୁକ୍ତ(pathext.string);
    ଫେରନ୍ତୁ retval;
}

#ଯଦି (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    ସ୍ଥିର ଶୂନ୍ୟ
bad_param_handler(ଅଚଳ wchar_t *expression UNUSED,
    ଅଚଳ wchar_t *function UNUSED,
    ଅଚଳ wchar_t *file UNUSED,
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି UNUSED,
    uintptr_t pReserved UNUSED)
{
}

# define SET_INVALID_PARAM_HANDLER \
	((ଶୂନ୍ୟ)_set_invalid_parameter_handler(bad_param_handler))
#ଅନ୍ୟଥା
# define SET_INVALID_PARAM_HANDLER
#endif

#ifdef FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mch_init_g(ଶୂନ୍ୟ)
{
# ifndef __MINGW32__
    ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_ଦ୍ୱିମିକ;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    Rows = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[_MAX_PATH + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	ଯଦି (exe_pathlen + 10 >= ଆକାର(vimrun_location))
	{
	    ଯଦି (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ଖାଲି, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	ଅନ୍ୟଥା
	{
	    // First try in same directory as gvim.exe.
	    ଯଦି (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    ଯଦି (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		ଯଦି (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(ଅକ୍ଷର *)vimrun_location,
			ଆକାର(vimrun_location),
			"\"%.*svimrun\" ",
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)exe_pathlen, exe_name);
		}
		ଅନ୍ୟଥା
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		ଯଦି (p != ଖାଲି)
		{
		    vimrun_path.string = p;
		    vimrun_path.length = plen;
		    vimrun_path_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ଖାଲି, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	ଯଦି (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    ଯଦି (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    ଖାଲି, TRUE, FALSE))
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#endif // FEAT_GUI_MSWIN

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).ଡାହାଣ_3 - (sr).ବାମ_3 + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).ଶୀର୍ଷ_2 + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    ସ୍ଥିର BOOL
ClearConsoleBuffer(WORD wAttribute)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    DWORD ଅଙ୍କ_Cells, dummy;

    ଯଦି (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ଫେରନ୍ତୁ FALSE;

    ଅଙ୍କ_Cells = csbi.dwSize.X * csbi.dwSize.Y;
    coord.X = 0;
    coord.Y = 0;
    ଯଦି (!FillConsoleOutputCharacter(g_hConOut, ' ', ଅଙ୍କ_Cells,
	    coord, &dummy))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (!FillConsoleOutputAttribute(g_hConOut, wAttribute, ଅଙ୍କ_Cells,
	    coord, &dummy))
	ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    ସ୍ଥିର BOOL
FitConsoleWindow(
    COORD dwBufferSize,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD dwWindowSize;
    BOOL NeedAdjust = FALSE;

    ଯଦି (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ଫେରନ୍ତୁ FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    ଯଦି (csbi.srWindow.ଡାହାଣ_3 >= dwBufferSize.X)
    {
	dwWindowSize.X = SRWIDTH(csbi.srWindow);
	ଯଦି (dwWindowSize.X > dwBufferSize.X)
	    dwWindowSize.X = dwBufferSize.X;
	csbi.srWindow.ଡାହାଣ_3 = dwBufferSize.X - 1;
	csbi.srWindow.ବାମ_3 = dwBufferSize.X - dwWindowSize.X;
	NeedAdjust = TRUE;
    }
    ଯଦି (csbi.srWindow.Bottom >= dwBufferSize.Y)
    {
	dwWindowSize.Y = SRHEIGHT(csbi.srWindow);
	ଯଦି (dwWindowSize.Y > dwBufferSize.Y)
	    dwWindowSize.Y = dwBufferSize.Y;
	csbi.srWindow.Bottom = dwBufferSize.Y - 1;
	csbi.srWindow.ଶୀର୍ଷ_2 = dwBufferSize.Y - dwWindowSize.Y;
	NeedAdjust = TRUE;
    }
    ଯଦି (NeedAdjust && WantAdjust)
    {
	ଯଦି (!SetConsoleWindowInfo(g_hConOut, TRUE, &csbi.srWindow))
	    ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ConsoleBufferStruct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_BUFFER_INFO	Info;
    PCHAR_INFO			Buffer;
    COORD			BufferSize;
    PSMALL_RECT			Regions;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା				ଅଙ୍କ_Regions;
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
    ସ୍ଥିର BOOL
SaveConsoleBuffer(
    ConsoleBuffer *cb)
{
    DWORD ଅଙ୍କ_Cells;
    COORD BufferCoord;
    SMALL_RECT ReadRegion;
    WORD Y, Y_incr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, numregions;

    ଯଦି (cb == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (!GetConsoleScreenBufferInfo(g_hConOut, &cb->Info))
    {
	cb->IsValid = FALSE;
	ଫେରନ୍ତୁ FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    ଯଦି (use_alternate_screen_buffer)
	ଫେରନ୍ତୁ TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    ଯଦି (!cb->IsValid || cb->Buffer == ଖାଲି ||
	    cb->BufferSize.X != cb->Info.dwSize.X ||
	    cb->BufferSize.Y != cb->Info.dwSize.Y)
    {
	cb->BufferSize.X = cb->Info.dwSize.X;
	cb->BufferSize.Y = cb->Info.dwSize.Y;
	ଅଙ୍କ_Cells = cb->BufferSize.X * cb->BufferSize.Y;
	vim_free(cb->Buffer);
	cb->Buffer = ALLOC_MULT(CHAR_INFO, ଅଙ୍କ_Cells);
	ଯଦି (cb->Buffer == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;
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
    ReadRegion.ବାମ_3 = 0;
    ReadRegion.ଡାହାଣ_3 = cb->Info.dwSize.X - 1;
    Y_incr = 12000 / cb->Info.dwSize.X;

    numregions = (cb->Info.dwSize.Y + Y_incr - 1) / Y_incr;
    ଯଦି (cb->Regions == ଖାଲି || numregions != cb->ଅଙ୍କ_Regions)
    {
	cb->ଅଙ୍କ_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->ଅଙ୍କ_Regions);
	ଯଦି (cb->Regions == ଖାଲି)
	{
	    VIM_CLEAR(cb->Buffer);
	    ଫେରନ୍ତୁ FALSE;
	}
    }

    ପାଇଁ (i = 0, Y = 0; i < cb->ଅଙ୍କ_Regions; i++, Y += Y_incr)
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
	ReadRegion.ଶୀର୍ଷ_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	ଯଦି (!ReadConsoleOutputW(g_hConOut,	// output handle
		cb->Buffer,			// our buffer
		cb->BufferSize,			// dimensions of our buffer
		BufferCoord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->Buffer);
	    VIM_CLEAR(cb->Regions);
	    ଫେରନ୍ତୁ FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    ଫେରନ୍ତୁ TRUE;
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
    ସ୍ଥିର BOOL
RestoreConsoleBuffer(
    ConsoleBuffer   *cb,
    BOOL	    RestoreScreen)
{
    COORD BufferCoord;
    SMALL_RECT WriteRegion;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (cb == ଖାଲି || !cb->IsValid)
	ଫେରନ୍ତୁ FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    ଯଦି (RestoreScreen)
	ClearConsoleBuffer(cb->Info.wAttributes);

    FitConsoleWindow(cb->Info.dwSize, TRUE);
    ଯଦି (!SetConsoleScreenBufferSize(g_hConOut, cb->Info.dwSize))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (!SetConsoleTextAttribute(g_hConOut, cb->Info.wAttributes))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	ଫେରନ୍ତୁ TRUE;
    }

    ଯଦି (!SetConsoleCursorPosition(g_hConOut, cb->Info.dwCursorPosition))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (!SetConsoleWindowInfo(g_hConOut, TRUE, &cb->Info.srWindow))
	ଫେରନ୍ତୁ FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    ଯଦି (use_alternate_screen_buffer)
	ଫେରନ୍ତୁ TRUE;

    /*
     * Restore the screen buffer contents.
     */
    ଯଦି (cb->Buffer != ଖାଲି)
    {
	ପାଇଁ (i = 0; i < cb->ଅଙ୍କ_Regions; i++)
	{
	    BufferCoord.X = cb->Regions[i].ବାମ_3;
	    BufferCoord.Y = cb->Regions[i].ଶୀର୍ଷ_2;
	    WriteRegion = cb->Regions[i];
	    ଯଦି (!WriteConsoleOutputW(g_hConOut,	// output handle
			cb->Buffer,		// our buffer
			cb->BufferSize,		// dimensions of our buffer
			BufferCoord,		// offset in our buffer
			&WriteRegion))		// region to restore
		ଫେରନ୍ତୁ FALSE;
	}
    }

    ଫେରନ୍ତୁ TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
ସ୍ଥିର ConsoleBuffer g_cbOrig = { 0 };
# endif
ସ୍ଥିର ConsoleBuffer g_cbNonTermcap = { 0 };
ସ୍ଥିର ConsoleBuffer g_cbTermcap = { 0 };

ଅକ୍ଷର g_szOrigTitle[256] = { 0 };
HWND g_hWnd = ଖାଲି;	// also used in os_mswin.c
ସ୍ଥିର HICON g_hOrigIconSmall = ଖାଲି;
ସ୍ଥିର HICON g_hOrigIcon = ଖାଲି;
ସ୍ଥିର HICON g_hVimIcon = ଖାଲି;
ସ୍ଥିର BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ସ୍ଥିର BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    ଯଦି (hWnd == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (phIconSmall != ଖାଲି)
	*phIconSmall = (HICON)SendMessage(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    ଯଦି (phIcon != ଖାଲି)
	*phIcon = (HICON)SendMessage(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    ଫେରନ୍ତୁ TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ସ୍ଥିର BOOL
SetConsoleIcon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    ଯଦି (hWnd == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (hIconSmall != ଖାଲି)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    ଯଦି (hIcon != ଖାଲି)
	SendMessage(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    ଫେରନ୍ତୁ TRUE;
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
    ସ୍ଥିର ଶୂନ୍ୟ
SaveConsoleTitleAndIcon(ଶୂନ୍ୟ)
{
    // Save the original title.
    ଯଦି (!GetConsoleTitle(g_szOrigTitle, ଆକାର(g_szOrigTitle)))
	ଫେରନ୍ତୁ;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = GetConsoleWindow();
    ଯଦି (g_hWnd == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    ଯଦି (g_hOrigIconSmall == ଖାଲି || g_hOrigIcon == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Extract the first icon contained in the Vim executable.
    ଯଦି (
# ifdef FEAT_LIBCALL
	    mch_icon_load((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == ଖାଲି)
	g_hVimIcon = ExtractIcon(ଖାଲି, (LPCSTR)exe_name, 0);
    ଯଦି (g_hVimIcon != ଖାଲି)
	g_fCanChangeIcon = TRUE;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_fWindInitCalled = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_fTermcapMode = FALSE;
ସ୍ଥିର CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mch_init_c(ଶୂନ୍ୟ)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_BUFFER_INFO csbi;
# endif
# ifndef __MINGW32__
    ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    SET_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_ଦ୍ୱିମିକ;		// we do our own CR-LF translation
    out_flush();

    // Obtain handles for the standard Console I/O devices
    ଯଦି (read_cmd_fd == 0)
	g_hConIn =  GetStdHandle(STD_ନିବେଶ_HANDLE);
    ଅନ୍ୟଥା
	create_conin();
    g_hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    SaveConsoleBuffer(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.wAttributes;
# ଅନ୍ୟଥା
    // Get current text attributes
    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
# endif
    ଯଦି (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (g_attrCurrent & 0xf) + 1;
    ଯଦି (cterm_normal_bg_color == 0)
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
    ଯଦି (g_fCanChangeIcon)
	SetConsoleIcon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    ଯଦି (use_alternate_screen_buffer)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    ଯଦି (fdDump)
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
    ସ୍ଥିର ଶୂନ୍ୟ
mch_exit_c(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r)
{
    // Copy flag since stoptermcap() will clear the flag.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fTermcapMode = g_fTermcapMode;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    ଯଦି (!fTermcapMode &&  use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    ଯଦି (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    ଯଦି (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	ଯଦି (g_fCanChangeIcon && !g_fForceExit)
	    SetConsoleIcon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	ଯଦି (fdDump)
	{
	    time_t t;

	    time(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = ଖାଲି;
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

    ଶୂନ୍ୟ
mch_init(ଶୂନ୍ୟ)
{
#ifdef VIMDLL
    ଯଦି (gui.starting)
	mch_init_g();
    ଅନ୍ୟଥା
	mch_init_c();
#ନଚେତଯଦି defined(FEAT_GUI_MSWIN)
    mch_init_g();
#ଅନ୍ୟଥା
    mch_init_c();
#endif
}

    ଶୂନ୍ୟ
mch_exit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r)
{
#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef VIMDLL
    ଯଦି (vimrun_path_allocated)
	vim_free(vimrun_path.string);

    ଯଦି (gui.in_use || gui.starting)
	mch_exit_g(r);
    ଅନ୍ୟଥା
	mch_exit_c(r);
#ନଚେତଯଦି defined(FEAT_GUI_MSWIN)
    ଯଦି (vimrun_path_allocated)
	vim_free(vimrun_path.string);
    mch_exit_g(r);
#ଅନ୍ୟଥା
    mch_exit_c(r);
#endif
}

/*
 * Do we have an interactive window?
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_check_win(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED,
    ଅକ୍ଷର **argv UNUSED)
{
    mch_get_exe_name();

#ଯଦି defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    ଫେରନ୍ତୁ OK;	    // GUI always has a tty
#ଅନ୍ୟଥା
# ifdef VIMDLL
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ OK;
# endif
    ଯଦି (isatty(1))
	ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ FAIL;
#endif
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    ଶୂନ୍ୟ
fname_case(
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len)
{
    WCHAR   *p;
    WCHAR   buf[_MAX_PATH + 1];

    ଯଦି (*name == NUL)
	ଫେରନ୍ତୁ;

    slash_adjust(name);

    p = enc_to_utf16(name, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (GetLongPathNameW(p, buf, _MAX_PATH))
    {
	char_u	*q = utf16_to_enc(buf, ଖାଲି);

	ଯଦି (q != ଖାଲି)
	{
	    ଯଦି (len > 0)
		vim_strncpy(name, q, len - 1);
	    ଅନ୍ୟଥା
	    {
		size_t  namelen = STRLEN(name);

		ଯଦି (namelen >= STRLEN(q))
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_user_name(
    char_u  *s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_LENGTH(wszUserName);

    ଯଦି (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_to_enc(wszUserName, ଖାଲି);

	ଯଦି (p != ଖାଲି)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    ଫେରନ୍ତୁ OK;
	}
    }
    s[0] = NUL;
    ଫେରନ୍ତୁ FAIL;
}


/*
 * Insert host name in s[len].
 */
    ଶୂନ୍ୟ
mch_get_host_name(
    char_u	*s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_LENGTH(wszHostName);

    ଯଦି (!GetComputerNameW(wszHostName, &wcch))
	ଫେରନ୍ତୁ;

    char_u  *p = utf16_to_enc(wszHostName, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    long
mch_get_pid(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (long)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_process_running(long pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   status = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ret = FALSE;

    ଯଦି (hProcess == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;  // might not have access
    ଯଦି (GetExitCodeProcess(hProcess, &status))
	ret = status == STILL_ACTIVE;
    CloseHandle(hProcess);
    ଫେରନ୍ତୁ ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_dirname(
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len)
{
    WCHAR   wbuf[_MAX_PATH + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    ଯଦି (GetCurrentDirectoryW(_MAX_PATH, wbuf) == 0)
	ଫେରନ୍ତୁ FAIL;

    WCHAR   wcbuf[_MAX_PATH + 1];
    char_u  *p = ଖାଲି;

    ଯଦି (GetLongPathNameW(wbuf, wcbuf, _MAX_PATH) != 0)
    {
	p = utf16_to_enc(wcbuf, ଖାଲି);
	ଯଦି (p != ଖାଲି)
	{
	    ଯଦି (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    ଯଦି (p == ଖାଲି)
	p = utf16_to_enc(wbuf, ଖାଲି);

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    ଫେରନ୍ତୁ OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    long
mch_getperm(char_u *name)
{
    stat_T	st;

    ଫେରନ୍ତୁ (mch_stat((ଅକ୍ଷର *)name, &st) == 0)
	? (long)(unsigned short)st.st_mode
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_setperm(char_u *name, long perm)
{
    long	n;
    WCHAR	*p;

    p = enc_to_utf16(name, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    ଯଦି (n == -1)
	ଫେରନ୍ତୁ FAIL;

    win32_set_archive(name);

    ଫେରନ୍ତୁ OK;
}

/*
 * Set hidden flag for "name".
 */
    ଶୂନ୍ୟ
mch_hide(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attrs = win32_getattrs(name);
    ଯଦି (attrs == -1)
	ଫେରନ୍ତୁ;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_ishidden(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା f = win32_getattrs(name);

    ଯଦି (f == -1)
	ଫେରନ୍ତୁ FALSE;		    // file does not exist at all

    ଫେରନ୍ତୁ (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_isdir(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା f = win32_getattrs(name);

    ଯଦି (f == -1)
	ଫେରନ୍ତୁ FALSE;		    // file does not exist at all

    ଫେରନ୍ତୁ (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_to_utf16(name, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    attr = GetFileAttributesW(p);
    ଯଦି (attr == INVALID_FILE_ATTRIBUTES)
	error = GetLastError();
    vim_free(p);

    ଯଦି (attr == INVALID_FILE_ATTRIBUTES)
	ଫେରନ୍ତୁ error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    ଫେରନ୍ତୁ (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_isrealdir(char_u *name)
{
    ଫେରନ୍ତୁ mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval;

    p = enc_to_utf16(name, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    retval = _wmkdir(p);
    vim_free(p);
    ଫେରନ୍ତୁ retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval;

    p = enc_to_utf16(name, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    retval = _wrmdir(p);
    vim_free(p);
    ଫେରନ୍ତୁ retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    ଫେରନ୍ତୁ win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_ସଂଖ୍ୟା_Of_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_is_symbolic_link(char_u *name)
{
    HANDLE		hFind;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			res = FALSE;
    DWORD		fileFlags = 0, reparseTag = 0;
    WCHAR		*wn;
    WIN32_FIND_DATAW	findDataW;

    wn = enc_to_utf16(name, ଖାଲି);
    ଯଦି (wn == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    hFind = FindFirstFileW(wn, &findDataW);
    vim_free(wn);
    ଯଦି (hFind != INVALID_HANDLE_ମୂଲ୍ୟ)
    {
	fileFlags = findDataW.dwFileAttributes;
	reparseTag = findDataW.dwReserved0;
	FindClose(hFind);
    }

    ଯଦି ((fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
	    && (reparseTag == IO_REPARSE_TAG_SYMLINK
		|| reparseTag == IO_REPARSE_TAG_MOUNT_POINT))
	res = TRUE;

    ଫେରନ୍ତୁ res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_is_linked(char_u *fname)
{
    ଯଦି (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_to_utf16(fname, ଖାଲି);
    ଯଦି (wn == ଖାଲି)
	ଫେରନ୍ତୁ FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    ଖାଲି,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    ଖାଲି);		// handle to template file
    vim_free(wn);

    ଯଦି (hFile == INVALID_HANDLE_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ FILEINFO_READ_FAIL;

    ଯଦି (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    ଅନ୍ୟଥା
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    ଫେରନ୍ତୁ res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win32_getattrs(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    WCHAR	*p;

    p = enc_to_utf16(name, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ INVALID_FILE_ATTRIBUTES;

    attr = GetFileAttributesW(p);
    vim_free(p);

    ଫେରନ୍ତୁ attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win32_setattrs(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attrs)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    res;
    WCHAR   *p;

    p = enc_to_utf16(name, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    res = SetFileAttributesW(p, attrs);
    vim_free(p);

    ଫେରନ୍ତୁ res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win32_set_archive(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attrs = win32_getattrs(name);
    ଯଦି (attrs == -1)
	ଫେରନ୍ତୁ -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    ଫେରନ୍ତୁ win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_writable(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attrs = win32_getattrs(name);

    ଫେରନ୍ତୁ (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_can_exe(char_u *name, char_u **path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_path UNUSED)
{
    ଫେରନ୍ତୁ executable_exists((ଅକ୍ଷର *)name, STRLEN(name), path, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    ଯଦି (STRNCMP(name, "\\\\.\\", 4) == 0)
	ଫେରନ୍ତୁ ଗଣ୍ଠି_WRITABLE;

    wn = enc_to_utf16(name, ଖାଲି);
    ଯଦି (wn == ଖାଲି)
	ଫେରନ୍ତୁ ଗଣ୍ଠି_NORMAL;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    ଖାଲି,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    ଖାଲି);		    // handle to template file
    vim_free(wn);
    ଯଦି (hFile == INVALID_HANDLE_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ ଗଣ୍ଠି_NORMAL;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    ଯଦି (type == FILE_TYPE_CHAR)
	ଫେରନ୍ତୁ ଗଣ୍ଠି_WRITABLE;
    ଯଦି (type == FILE_TYPE_DISK)
	ଫେରନ୍ତୁ ଗଣ୍ଠି_NORMAL;
    ଫେରନ୍ତୁ ଗଣ୍ଠି_OTHER;
}

#ifdef HAVE_ACL
ସଂରଚନା my_acl
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
    ଫେରନ୍ତୁ (vim_acl_T)ଖାଲି;
#ଅନ୍ୟଥା
    ସଂରଚନା my_acl   *p = ଖାଲି;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(ସଂରଚନା my_acl);
    ଯଦି (p != ଖାଲି)
    {
	WCHAR	*wn;

	wn = enc_to_utf16(fname, ଖାଲି);
	ଯଦି (wn == ଖାଲି)
	{
	    vim_free(p);
	    ଫେରନ୍ତୁ ଖାଲି;
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
	ଯଦି (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (ଶୂନ୍ୟ)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    ଖାଲି,
		    ଖାଲି,
		    &p->pDacl,
		    ଖାଲି,
		    &p->pSecurityDescriptor);
	}
	ଯଦି (p->pSecurityDescriptor == ଖାଲି)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = ଖାଲି;
	}
	vim_free(wn);
    }

    ଫେରନ୍ତୁ (vim_acl_T)p;
#endif
}

#ifdef HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    ସ୍ଥିର BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_SIZE_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_ଗଣନା = 0;
    GetAclInformation(acl, &acl_info, ଆକାର(acl_info), AclSizeInformation);
    ପାଇଁ (i = 0; i < acl_info.Ace_ଗଣନା; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	ଯଦି (ace->Header.AceFlags & INHERITED_ACE)
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}
#endif

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    ଶୂନ୍ୟ
mch_set_acl(char_u *fname, vim_acl_T acl)
{
#ifdef HAVE_ACL
    ସଂରଚନା my_acl   *p = (ସଂରଚନା my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    wn = enc_to_utf16(fname, ଖାଲି);
    ଯଦି (wn == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Set security flags
    ଯଦି (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    ଯଦି (p->pSidGroup)
	sec_info |= GROUP_SECURITY_INFORMATION;
    ଯଦି (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	ଯଦି (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    ଯଦି (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (ଶୂନ୍ୟ)SetNamedSecurityInfoW(
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

    ଶୂନ୍ୟ
mch_free_acl(vim_acl_T acl)
{
#ifdef HAVE_ACL
    ସଂରଚନା my_acl   *p = (ସଂରଚନା my_acl *)acl;

    ଯଦି (p != ଖାଲି)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#endif
}

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    ସ୍ଥିର BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    ନିବେଶ_RECORD ir;
    DWORD out;

    ବାଛ (dwCtrlType)
    {
    ସ୍ଥିତି CTRL_C_EVENT:
	ଯଦି (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	ଫେରନ୍ତୁ TRUE;

    ସ୍ଥିତି CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = ଚାବି_EVENT;
	ir.Event.ଚାବି_Event_2.b_ଚାବି_Down = TRUE;
	ir.Event.ଚାବି_Event_2.w_Repeat_ଗଣନା = 1;
	ir.Event.ଚାବି_Event_2.w_Virtual_ଚାବି_Code = VK_CANCEL;
	ir.Event.ଚାବି_Event_2.wVirtualScanCode = 0;
	ir.Event.ଚାବି_Event_2.dw_Control_ଚାବି_State = 0;
	ir.Event.ଚାବି_Event_2.uChar.UnicodeChar = 0;
	Write_Console_ନିବେଶ(g_hConIn, &ir, 1, &out);
	ଫେରନ୍ତୁ TRUE;

    // fatal events: shut down gracefully
    ସ୍ଥିତି CTRL_CLOSE_EVENT:
    ସ୍ଥିତି CTRL_LOGOFF_EVENT:
    ସ୍ଥିତି CTRL_SHUTDOWN_EVENT:
	windgoto((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, cmdline_col_off);
	g_fForceExit = TRUE;

	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	ଫେରନ୍ତୁ TRUE;		// not reached

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ଫେରନ୍ତୁ FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    ଶୂନ୍ୟ
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ;
# endif
    GetConsoleMode(g_hConIn, &cmodein);
    GetConsoleMode(g_hConOut, &cmodeout);
    ଯଦି (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_ଧାଡ଼ି_ନିବେଶ | ENABLE_PROCESSED_ନିବେଶ |
		     ENABLE_ECHO_ନିବେଶ);
	ଯଦି (g_fMouseActive)
	{
	    cmodein |= ENABLE_MOUSE_ନିବେଶ;
	    cmodein &= ~ENABLE_QUICK_EDIT_MODE;
	}
	ଅନ୍ୟଥା
	{
	    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# ଅନ୍ୟଥା
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    ଅନ୍ୟଥା // cooked
    {
	cmodein |= (ENABLE_ଧାଡ଼ି_ନିବେଶ | ENABLE_PROCESSED_ନିବେଶ |
		    ENABLE_ECHO_ନିବେଶ);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);
    SetConsoleMode(g_hConOut, cmodeout);
    SetConsoleCtrlHandler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    ଯଦି (fdDump)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_shellsize(ଶୂନ୍ୟ)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

# ifdef VIMDLL
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ OK;
# endif
    ଯଦି (!g_fTermcapMode && g_cbTermcap.IsValid)
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
    ଅନ୍ୟଥା ଯଦି (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	Rows = csbi.srWindow.Bottom - csbi.srWindow.ଶୀର୍ଷ_2 + 1;
	Columns = csbi.srWindow.ଡାହାଣ_3 - csbi.srWindow.ବାମ_3 + 1;
    }
    ଅନ୍ୟଥା
    {
	Rows = 25;
	Columns = 80;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    ଯଦି (use_alternate_screen_buffer)
	ଫେରନ୍ତୁ;

    ଯଦି (!SetConsoleScreenBufferSize(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	ଯଦି (fdDump)
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
    ସ୍ଥିର ଶୂନ୍ୟ
ResizeWindow(
    HANDLE     hConsole,
    SMALL_RECT srWindowRect)
{
    ଯଦି (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
    {
# ifdef MCH_WRITE_DUMP
	ଯଦି (fdDump)
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
    ସ୍ଥିର ଶୂନ୍ୟ
ResizeConBufAndWindow(
    HANDLE  hConsole,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    xSize,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ySize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    srWindowRect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    ଯଦି (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", xSize, ySize);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    // define the new console window size and scroll position
    srWindowRect.ବାମ_3 = srWindowRect.ଶୀର୍ଷ_2 = (SHORT) 0;
    srWindowRect.ଡାହାଣ_3 =  (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);

    ଯଦି (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା sx, sy;

	sx = csbi.srWindow.ଡାହାଣ_3 - csbi.srWindow.ବାମ_3 + 1;
	sy = csbi.srWindow.Bottom - csbi.srWindow.ଶୀର୍ଷ_2 + 1;
	ଯଦି (sy < ySize || sx < xSize)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    ଯଦି (sy < ySize)
		coordScreen.Y = ySize;
	    ଅନ୍ୟଥା
		coordScreen.Y = sy;
	    ଯଦି (sx < xSize)
		coordScreen.X = xSize;
	    ଅନ୍ୟଥା
		coordScreen.X = sx;
	    SetConsoleScreenBufferSize(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = xSize;
    coordScreen.Y = ySize;

    // In the new console call API, only the first time in reverse order
    ଯଦି (!vtp_working || resized)
    {
	ResizeWindow(hConsole, srWindowRect);
	ResizeConBuf(hConsole, coordScreen);
    }
    ଅନ୍ୟଥା
    {
	// Workaround for a Windows 10 bug
	cursor.X = srWindowRect.ବାମ_3;
	cursor.Y = srWindowRect.ଶୀର୍ଷ_2;
	SetConsoleCursorPosition(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	ResizeWindow(hConsole, srWindowRect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    ଶୂନ୍ୟ
mch_set_shellsize(ଶୂନ୍ୟ)
{
    COORD coordScreen;

# ifdef VIMDLL
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ;
# endif
    // Don't change window size while still starting up
    ଯଦି (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (term_console)
    {
	coordScreen = GetLargestConsoleWindowSize(g_hConOut);

	// Clamp Rows and Columns to reasonable values
	ଯଦି (Rows > coordScreen.Y)
	    Rows = coordScreen.Y;
	ଯଦି (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	ResizeConBufAndWindow(g_hConOut, Columns, Rows);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    ଶୂନ୍ୟ
mch_new_shellsize(ଶୂନ୍ୟ)
{
# ifdef VIMDLL
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ;
# endif
    set_scroll_region(0, 0, Columns - 1, Rows - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    ଶୂନ୍ୟ
mch_set_winsize_now(ଶୂନ୍ୟ)
{
    ଯଦି (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_set_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# ଯଦି defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    ଶୂନ୍ୟ
mch_calc_cell_size(ସଂରଚନା cellsize *cs_out)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_out->cs_xpixel = -1;
    cs_out->cs_ypixel = -1;

    ଯଦି (g_hConOut == INVALID_HANDLE_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ;

    cfi.cbSize = ଆକାର(cfi);
    ଯଦି (GetCurrentConsoleFontEx(g_hConOut, FALSE, &cfi)
	    && cfi.dwFontSize.X > 0 && cfi.dwFontSize.Y > 0)
    {
	cs_out->cs_xpixel = cfi.dwFontSize.X;
	cs_out->cs_ypixel = cfi.dwFontSize.Y;
	ଫେରନ୍ତୁ;
    }

    GetConsoleMode(g_hConIn, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_ଧାଡ଼ି_ନିବେଶ | ENABLE_ECHO_ନିବେଶ
	    | ENABLE_PROCESSED_ନିବେଶ);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_ନିବେଶ;

    ଯଦି (SetConsoleMode(g_hConIn, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	SetConsoleMode(g_hConIn, cmodeold);
    }
}
# endif

#endif

    ସ୍ଥିର BOOL
vim_create_process(
    ଅକ୍ଷର		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    ଅକ୍ଷର		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = ଖାଲି;

    wcmd = enc_to_utf16((char_u *)cmd, ଖାଲି);
    ଯଦି (wcmd == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (cwd != ଖାଲି)
    {
	wcwd = enc_to_utf16((char_u *)cwd, ଖାଲି);
	ଯଦି (wcwd == ଖାଲି)
	    ଯାଅ theend;
    }

    ret = CreateProcessW(
	    ଖାଲି,		// Executable name
	    wcmd,		// Command to execute
	    ଖାଲି,		// Process security attributes
	    ଖାଲି,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    ଫେରନ୍ତୁ ret;
}


    ସ୍ଥିର HINSTANCE
vim_shell_execute(
    ଅକ୍ଷର *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_to_utf16((char_u *)cmd, ଖାଲି);
    ଯଦି (wcmd == ଖାଲି)
	ଫେରନ୍ତୁ (HINSTANCE) 0;

    ret = ShellExecuteW(ଖାଲି, ଖାଲି, wcmd, ଖାଲି, ଖାଲି, n_show_cmd);
    vim_free(wcmd);
    ଫେରନ୍ତୁ ret;
}


#ଯଦି defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_system_classic(ଅକ୍ଷର *cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = ଆକାର(si);
    si.lpReserved = ଖାଲି;
    si.lpDesktop = ଖାଲି;
    si.lpTitle = ଖାଲି;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    ଯଦି (options & SHELL_DOOUT)
	si.wShowWindow = SW_SHOWMINNOACTIVE;
    ଅନ୍ୟଥା
	si.wShowWindow = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = ଖାଲି;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_MODE |	CREATE_NEW_CONSOLE,
	    &si, &pi, ଖାଲି, ଖାଲି);

    // Wait for the command to terminate before continuing
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	ପାଇଁ (;;)
	{
	    ସନ୍ଦେଶ_2	ସନ୍ଦେଶ;

	    ଯଦି (PeekMessageW(&ସନ୍ଦେଶ, (HWND)ଖାଲି, 0, 0, PM_REMOVE))
	    {
		TranslateMessage(&ସନ୍ଦେଶ);
		DispatchMessageW(&ସନ୍ଦେଶ);
		delay = 1;
		ଜାରି;
	    }
	    ଯଦି (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		ଭାଙ୍ଗ;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    ଯଦି (delay < 50)
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

    ଫେରନ୍ତୁ ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_hChildStd_IN_Wr = param;
    linenr_T	    lnum = curbuf->b_op_start.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    written = 0;

    ପାଇଁ (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	ଯଦି (l == 0)
	    len = 0;
	ଅନ୍ୟଥା ଯଦି (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_hChildStd_IN_Wr, "", 1, &len, ଖାଲି);
	}
	ଅନ୍ୟଥା
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_hChildStd_IN_Wr, (ଅକ୍ଷର *)lp + written,
		      s == ଖାଲି ? l : (DWORD)(s - (lp + written)),
		      &len, ଖାଲି);
	}
	ଯଦି (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    ଯଦି (lnum != curbuf->b_op_end.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_hChildStd_IN_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, ଖାଲି);
	    }

	    ++lnum;
	    ଯଦି (lnum > curbuf->b_op_end.lnum)
		ଭାଙ୍ଗ;

	    lp = ml_get(lnum);
	    written = 0;
	}
	ଅନ୍ୟଥା ଯଦି (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_hChildStd_IN_Wr);
    ଫେରନ୍ତୁ 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
dump_pipe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    options,
	  HANDLE    g_hChildStd_OUT_Rd,
	  garray_T  *ga,
	  char_u    buffer[],
	  DWORD	    *buffer_off)
{
    DWORD	availableBytes = 0;
    DWORD	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    DWORD	len;
    DWORD	toRead;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_hChildStd_OUT_Rd, // pipe to query
			ଖାଲି,		    // optional buffer
			0,		    // buffer size
			ଖାଲି,		    // number of read bytes
			&availableBytes,    // available bytes total
			ଖାଲି);		    // byteLeft

    // We got real data in the pipe, read it
    ଯେତେବେଳେ (ret != 0 && availableBytes > 0)
    {
	toRead = (DWORD)(BUFLEN - *buffer_off);
	toRead = availableBytes < toRead ? availableBytes : toRead;
	ReadFile(g_hChildStd_OUT_Rd, buffer + *buffer_off, toRead , &len, ଖାଲି);

	// If we haven't read anything, there is a problem
	ଯଦି (len == 0)
	    ଭାଙ୍ଗ;

	availableBytes -= len;

	ଯଦି (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    ପାଇଁ (i = 0; i < len; ++i)
	    {
		ଯଦି (buffer[i] == NL)
		    append_ga_ଧାଡ଼ି(ga);
		ଅନ୍ୟଥା ଯଦି (buffer[i] == NUL)
		    ga_append(ga, NL);
		ଅନ୍ୟଥା
		    ga_append(ga, buffer[i]);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (has_mbyte)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
	    char_u	*p;

	    len += *buffer_off;
	    buffer[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    ପାଇଁ (p = buffer; p < buffer + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		ଯଦି (l == 0)
		    l = 1;  // NUL byte?
		ଅନ୍ୟଥା ଯଦି (MB_BYTE2LEN(*p) != l)
		    ଭାଙ୍ଗ;
	    }
	    ଯଦି (p == buffer)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		ଯଦି (len >= 12)
		    ++p;
		ଅନ୍ୟଥା
		{
		    *buffer_off = len;
		    ଫେରନ୍ତୁ;
		}
	    }
	    c = *p;
	    *p = NUL;
	    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)buffer);
	    ଯଦି (p < buffer + len)
	    {
		*p = c;
		*buffer_off = (DWORD)((buffer + len) - p);
		mch_memmove(buffer, p, *buffer_off);
		ଫେରନ୍ତୁ;
	    }
	    *buffer_off = 0;
	}
	ଅନ୍ୟଥା
	{
	    buffer[len] = NUL;
	    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)buffer);
	}

	windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);
	cursor_on();
	out_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_system_piped(ଅକ୍ଷର *cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_hChildStd_IN_Rd = ଖାଲି;
    HANDLE g_hChildStd_IN_Wr = ଖାଲି;
    HANDLE g_hChildStd_OUT_Rd = ଖାଲି;
    HANDLE g_hChildStd_OUT_Wr = ଖାଲି;

    char_u	buffer[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noread_cnt = 0;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		delay = 1;
    DWORD	buffer_off = 0;	// valid bytes in buffer[]
    ଅକ୍ଷର	*p = ଖାଲି;

    SECURITY_ATTRIBUTES saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = ଆକାର(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ଖାଲି;

    ଯଦି ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)
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
	ସନ୍ଦେଶ_puts(_("\nCannot create pipes\n"));
    }

    si.cb = ଆକାର(si);
    si.lpReserved = ଖାଲି;
    si.lpDesktop = ଖାଲି;
    si.lpTitle = ଖାଲି;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_hChildStd_OUT_Wr;
    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.h_Std_ନିବେଶ = g_hChildStd_IN_Rd;
    si.wShowWindow = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = ଖାଲି;

    ଯଦି (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    ଯଦି (cmd != ଖାଲି)
    {
	p = (ଅକ୍ଷର *)vim_strsave((char_u *)cmd);
	ଯଦି (p != ଖାଲି)
	    unescape_shellxquote((char_u *)p, p_sxe);
	ଅନ୍ୟଥା
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_MODE,
						&si, &pi, ଖାଲି, ଖାଲି);

    ଯଦି (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(g_hChildStd_OUT_Wr);

    ଯଦି (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(ଖାଲି,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_hChildStd_IN_Wr,  // parameter
			0,		 // creation flag, start immediately
			ଖାଲି);		    // we don't care about thread id
	CloseHandle(thread);
	g_hChildStd_IN_Wr = ଖାଲି;
    }

    // Keep updating the window while waiting for the shell to finish.
    ପାଇଁ (;;)
    {
	ସନ୍ଦେଶ_2	ସନ୍ଦେଶ;

	ଯଦି (PeekMessageW(&ସନ୍ଦେଶ, (HWND)ଖାଲି, 0, 0, PM_REMOVE))
	{
	    TranslateMessage(&ସନ୍ଦେଶ);
	    DispatchMessageW(&ସନ୍ଦେଶ);
	}

	// write pipe information in the window
	ଯଦି ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.in_use
# endif
	    )
	{
	    len = 0;
	    ଯଦି (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		ଯଦି (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		ଯଦି (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    ଯଦି (len == 1 && cmd != ଖାଲି)
		    {
			ଯଦି (ta_buf[ta_len] == Ctrl_C)
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
		    ଯଦି ((len == 1 || len == 4 ) && cmd != ଖାଲି)
		    {
			ଯଦି (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_hChildStd_IN_Wr);
			    g_hChildStd_IN_Wr = ଖାଲି;
			    len = 0;
			}
		    }

		    len = term_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    ପାଇଁ (i = ta_len; i < ta_len + len; ++i)
		    {
			ଯଦି (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    ସନ୍ଦେଶ_putchar(ta_buf[i]);
			ଅନ୍ୟଥା ଯଦି (has_mbyte)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = (*mb_ptr2len)(ta_buf + i);

			    ସନ୍ଦେଶ_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			ଅନ୍ୟଥା
			    ସନ୍ଦେଶ_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);
		    out_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    ଯଦି (options & SHELL_WRITE)
			ta_len = 0;
		    ଅନ୍ୟଥା ଯଦି (g_hChildStd_IN_Wr != ଖାଲି)
		    {
			WriteFile(g_hChildStd_IN_Wr, (ଅକ୍ଷର*)ta_buf,
				    1, &len, ଖାଲି);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			ଯଦି (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	ଯଦି (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	ଯଦି (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);
	    ଭାଙ୍ଗ;
	}

	++noread_cnt;
	dump_pipe(options, g_hChildStd_OUT_Rd, &ga, buffer, &buffer_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	ଯଦି (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_hChildStd_OUT_Rd);
    ଯଦି (g_hChildStd_IN_Wr != ଖାଲି)
	CloseHandle(g_hChildStd_IN_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    GetExitCodeProcess(pi.hProcess, &ret);

    ଯଦି (options & SHELL_READ)
    {
	ଯଦି (ga.ga_len > 0)
	{
	    append_ga_ଧାଡ଼ି(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	ଅନ୍ୟଥା
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_system_g(ଅକ୍ଷର *cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    // if we can pipe and the shelltemp option is off
    ଯଦି (!p_stmp)
	ଫେରନ୍ତୁ mch_system_piped(cmd, options);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ mch_system_classic(cmd, options);
}
#endif

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_system_c(ଅକ୍ଷର *cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    ଯଦି (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	ଯଦି (buf == ଖାଲି)
	    ଫେରନ୍ତୁ -1;
	vim_snprintf((ଅକ୍ଷର *)buf, len, "(%s)", cmd);
	wcmd = enc_to_utf16(buf, ଖାଲି);
	ମୁକ୍ତ(buf);
    }
    ଅନ୍ୟଥା
	wcmd = enc_to_utf16((char_u *)cmd, ଖାଲି);

    ଯଦି (wcmd == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    ଫେରନ୍ତୁ ret;
}

#endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_system(ଅକ୍ଷର *cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
#ifdef VIMDLL
    ଯଦି (gui.in_use || gui.starting)
	ଫେରନ୍ତୁ mch_system_g(cmd, options);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ mch_system_c(cmd, options);
#ନଚେତଯଦି defined(FEAT_GUI_MSWIN)
    ଫେରନ୍ତୁ mch_system_g(cmd, options);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ mch_system_c(cmd, options);
#endif
}

#ଯଦି defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_call_shell_terminal(
    char_u	*cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = ଖାଲି;
    typval_T	argvar[2];
    long_u	cmdlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    ଯଦି (cmd == ଖାଲି)
	cmdlen = STRLEN(p_sh) + 1;
    ଅନ୍ୟଥା
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    ଯଦି (newcmd == ଖାଲି)
	ଫେରନ୍ତୁ 255;
    ଯଦି (cmd == ଖାଲି)
    {
	STRCPY(newcmd, p_sh);
	ch_log(ଖାଲି, "starting terminal to run a shell");
    }
    ଅନ୍ୟଥା
    {
	vim_snprintf((ଅକ୍ଷର *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(ଖାଲି, "starting terminal for system command '%s'", cmd);
    }

    init_job_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = term_start(argvar, ଖାଲି, &opt, TERM_START_SYSTEM);
    ଯଦି (buf == ଖାଲି)
    {
	vim_free(newcmd);
	ଫେରନ୍ତୁ 255;
    }

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    ଯଦି (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	ଯେତେବେଳେ (term_use_loop())
	{
	    ଯଦି (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		ଯଦି (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    ଅନ୍ୟଥା
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(ଖାଲି, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    ଫେରନ୍ତୁ retval;
}
#endif
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
restore_env_var(char_u *name, char_u *old_ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା must_free)
{
    ଯଦି (old_ମୂଲ୍ୟ != ଖାଲି)
    {
	vim_setenv(name, old_ମୂଲ୍ୟ);
	ଯଦି (must_free)
	    vim_free(old_ମୂଲ୍ୟ);
	ଫେରନ୍ତୁ;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_call_shell(
    char_u  *cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    options)	// SHELL_*, see vim.h
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		must_free;
    char_u	*oldval;

#ifdef FEAT_EVAL
    ch_log(ଖାଲି, "executing shell command: %s", cmd);
#endif
    // Change the title to reflect that we are in a subshell.
    ଯଦି (GetConsoleTitleW(szShellTitle, ARRAY_LENGTH(szShellTitle) - 4) > 0)
    {
	ଯଦି (cmd == ଖାଲି)
	    wcscat(szShellTitle, L" :sh");
	ଅନ୍ୟଥା
	{
	    WCHAR *wn = enc_to_utf16((char_u *)cmd, ଖାଲି);

	    ଯଦି (wn != ଖାଲି)
	    {
		wcscat(szShellTitle, L" - !");
		ଯଦି ((wcslen(szShellTitle) + wcslen(wn) <
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
    ଯଦି (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#endif
#ଯଦି defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    ଯଦି (
# ifdef VIMDLL
	    gui.in_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != ଖାଲି
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	ଯଦି (cmdbase != ଖାଲି)
	    // Skip a leading quote and (.
	    ଯେତେବେଳେ (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	ଯଦି (cmdbase == ଖାଲି || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    ଫେରନ୍ତୁ x;
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

    ଯଦି (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    ଯଦି (cmd == ଖାଲି)
    {
	x = mch_system((ଅକ୍ଷର *)p_sh, options);
    }
    ଅନ୍ୟଥା
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = ଖାଲି;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	ଯଦି (*cmdbase == '"' )
	    ++cmdbase;
	ଯଦି (*cmdbase == '(')
	    ++cmdbase;

	ଯଦି ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    ZeroMemory(&si, ଆକାର(si));
	    si.cb = ଆକାର(si);
	    si.lpReserved = ଖାଲି;
	    si.lpDesktop = ଖାଲି;
	    si.lpTitle = ଖାଲି;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = ଖାଲି;

	    cmdbase = skipwhite(cmdbase + 5);
	    ଯଦି ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    ଅନ୍ୟଥା ଯଦି ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_WINDOW;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_ନିବେଶ = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    ଖାଲି,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_NORMAL,		// File att.
		    ଖାଲି);				// Temp file
		si.hStdOutput = si.h_Std_ନିବେଶ;
		si.hStdError = si.h_Std_ନିବେଶ;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    ଯଦି (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		ଯଦି (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		ଯଦି (p > cmdbase && p[-1] == ')'
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
	    ଯଦି (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		ଯଦି (cmd_shell == ଖାଲି || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		ଯଦି (subcmd != ଖାଲି)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    ଯଦି (newcmd != ଖାଲି)
			vim_snprintf((ଅକ୍ଷର *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    ଅନ୍ୟଥା
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    ଯଦି (vim_create_process((ଅକ୍ଷର *)newcmd, FALSE, flags,
			&si, &pi, ଖାଲି, ଖାଲି))
		x = 0;
	    ଅନ୍ୟଥା ଯଦି (vim_shell_execute((ଅକ୍ଷର *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    ଅନ୍ୟଥା
	    {
		x = -1;
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
		ଯଦି (gui.in_use)
# endif
		    emsg(_(e_command_not_found));
#endif
	    }

	    ଯଦି (newcmd != cmdbase)
		vim_free(newcmd);

	    ଯଦି (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_ନିବେଶ != ଖାଲି)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_ନିବେଶ);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	ଅନ୍ୟଥା
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
	    ଯଦି (newcmd != ଖାଲି)
	    {
#ଯଦି defined(FEAT_GUI_MSWIN)
		ଯଦି (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    ଅକ୍ଷର *ସନ୍ଦେଶ = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    ଅକ୍ଷର *title = _("Vim Warning");
		    WCHAR *wmsg = enc_to_utf16((char_u *)ସନ୍ଦେଶ, ଖାଲି);
		    WCHAR *wtitle = enc_to_utf16((char_u *)title, ଖାଲି);

		    ଯଦି (wmsg != ଖାଲି && wtitle != ଖାଲି)
			MessageBoxW(ଖାଲି, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		ଯଦି (
# ifdef VIMDLL
		    (gui.in_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((ଅକ୍ଷର *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_path.string,
			    (ସନ୍ଦେଶ_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		ଅନ୍ୟଥା ଯଦି (
# ifdef VIMDLL
			(gui.in_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((ଅକ୍ଷର *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		ଅନ୍ୟଥା
#endif
		    vim_snprintf((ଅକ୍ଷର *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((ଅକ୍ଷର *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    ଯଦି (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    ଯଦି (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#ଯଦି defined(FEAT_GUI_MSWIN)
	    && ((gui.in_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#endif
	    )
    {
	smsg(_("shell returned %d"), x);
	ସନ୍ଦେଶ_putchar('\n');
    }
    resettitle();

    mch_signal(SIGINT, SIG_DFL);
    mch_signal(SIGBREAK, SIG_DFL);
    mch_signal(SIGILL, SIG_DFL);
    mch_signal(SIGFPE, SIG_DFL);
    mch_signal(SIGSEGV, SIG_DFL);
    mch_signal(SIGTERM, SIG_DFL);
    mch_signal(SIGABRT, SIG_DFL);

    ଫେରନ୍ତୁ x;
}

#ଯଦି defined(FEAT_JOB_CHANNEL)
    ସ୍ଥିର HANDLE
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

    wn = enc_to_utf16(fname, ଖାଲି);
    ଯଦି (wn == ଖାଲି)
	ଫେରନ୍ତୁ INVALID_HANDLE_ମୂଲ୍ୟ;

    h = CreateFileW(wn, dwDesiredAccess, dwShareMode,
	    lpSecurityAttributes, dwCreationDisposition,
	    dwFlagsAndAttributes, ଖାଲି);
    vim_free(wn);
    ଫେରନ୍ତୁ h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    ଶୂନ୍ୟ
win32_build_env(dict_T *env, garray_T *gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != ଖାଲି ? env->dv_hashtab.ht_used : 0;
    LPVOID	base = GetEnvironmentStringsW();

    // for last \0
    ଯଦି (ga_grow(gap, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (env != ଖାଲି)
    {
	FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&env->dv_hashtab, hi, todo)
	{
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		typval_T *item = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_to_utf16((char_u *)hi->hi_ଚାବି, ଖାଲି);
		WCHAR   *wval = enc_to_utf16(tv_get_string(item), ଖାଲି);
		--todo;
		ଯଦି (wkey != ଖାଲି && wval != ଖାଲି)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    ଯଦି (ga_grow(gap, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(lkey + lval + 2)) == FAIL)
			ଜାରି;
		    ପାଇଁ (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'=';
		    ପାଇଁ (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_data + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    ଯଦି (base)
    {
	WCHAR	*p = (WCHAR*) base;

	// for last \0
	ଯଦି (ga_grow(gap, 1) == FAIL)
	    ଫେରନ୍ତୁ;

	ଯେତେବେଳେ (*p != 0 || *(p + 1) != 0)
	{
	    ଯଦି (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_data + gap->ga_len++) = *p;
	    p++;
	}
	FreeEnvironmentStringsW(base);
	*((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
    }

# ଯଦି defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
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

	ଯଦି (ga_grow(gap, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    ପାଇଁ (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    ପାଇଁ (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_data + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_data + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    ଯଦି (is_terminal)
	    {
		ପାଇଁ (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_data + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		ପାଇଁ (n = 0; n < version_len; n++)
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
    ସ୍ଥିର BOOL
create_pipe_pair(HANDLE handles[2])
{
    ସ୍ଥିର LONG		s;
    ଅକ୍ଷର		name[64];
    SECURITY_ATTRIBUTES sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_SIZE, 0, 0, ଖାଲି);

    ଯଦି (handles[1] == INVALID_HANDLE_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ FALSE;

    sa.nLength = ଆକାର(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = ଖାଲି;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    ଯଦି (handles[0] == INVALID_HANDLE_ମୂଲ୍ୟ)
    {
	CloseHandle(handles[1]);
	ଫେରନ୍ତୁ FALSE;
    }

    ଫେରନ୍ତୁ TRUE;
}

#endif // FEAT_JOB_CHANNEL

#ଯଦି defined(FEAT_EVAL)
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
    ଅକ୍ଷର	**argv,
    char_u	*infile,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;
    HANDLE		hChildStdoutRd = INVALID_HANDLE_ମୂଲ୍ୟ;
    HANDLE		hChildStdoutWr = INVALID_HANDLE_ମୂଲ୍ୟ;
    HANDLE		hChildStdinRd = INVALID_HANDLE_ମୂଲ୍ୟ;
    garray_T		cmd_ga;
    garray_T		out_ga;
    char_u		*buffer = ଖାଲି;
    DWORD		exit_code = (DWORD)-1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    ପାଇଁ (i = 0; argv[i] != ଖାଲି; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	has_spaces = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	j;

	ପାଇଁ (j = 0; s[j] != NUL; j++)
	    ଯଦି (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		ଭାଙ୍ଗ;
	    }

	ଯଦି (i > 0)
	    ga_append(&cmd_ga, ' ');

	ଯଦି (has_spaces)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଅଙ୍କ_bs;

	    ga_append(&cmd_ga, '"');
	    ପାଇଁ (j = 0; arg[j] != NUL; j++)
	    {
		ଅଙ୍କ_bs = 0;
		ଯେତେବେଳେ (arg[j] == '\\')
		{
		    ଅଙ୍କ_bs++;
		    j++;
		}

		ଯଦି (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    ଯେତେବେଳେ (ଅଙ୍କ_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା ଯଦି (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    ଯେତେବେଳେ (ଅଙ୍କ_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		ଅନ୍ୟଥା
		{
		    ଯେତେବେଳେ (ଅଙ୍କ_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	ଅନ୍ୟଥା
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&out_ga, 1, 4096);

    saAttr.nLength = ଆକାର(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ଖାଲି;

    // Create a pipe for the child's stdout.
    ଯଦି (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)
	    || !SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	ଯାଅ done;
    }

    // Set up stdin from infile if provided.
    ଯଦି (infile != ଖାଲି)
    {
	WCHAR *winfile = enc_to_utf16(infile, ଖାଲି);

	ଯଦି (winfile != ଖାଲି)
	{
	    hChildStdinRd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL, ଖାଲି);
	    vim_free(winfile);
	}
    }

    ZeroMemory(&pi, ଆକାର(pi));
    ZeroMemory(&si, ଆକାର(si));
    si.cb = ଆକାର(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;
    si.h_Std_ନିବେଶ = (hChildStdinRd != INVALID_HANDLE_ମୂଲ୍ୟ)
		    ? hChildStdinRd : INVALID_HANDLE_ମୂଲ୍ୟ;

    ch_log(ଖାଲି, "directly executing: %s", (ଅକ୍ଷର *)cmd_ga.ga_data);

    // Create the child process directly, without going through the shell.
    ଯଦି (!vim_create_process((ଅକ୍ଷର *)cmd_ga.ga_data, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
		&si, &pi, ଖାଲି, ଖାଲି))
    {
	semsg(_(e_invalid_argument_str), cmd_ga.ga_data);
	ଯାଅ done;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(hChildStdoutWr);
    hChildStdoutWr = INVALID_HANDLE_ମୂଲ୍ୟ;
    ଯଦି (hChildStdinRd != INVALID_HANDLE_ମୂଲ୍ୟ)
    {
	CloseHandle(hChildStdinRd);
	hChildStdinRd = INVALID_HANDLE_ମୂଲ୍ୟ;
    }

    // Read output from child process.
    ପାଇଁ (;;)
    {
	ଅକ୍ଷର	buf[4096];
	DWORD	n;

	ଯଦି (!ReadFile(hChildStdoutRd, buf, ଆକାର(buf), &n, ଖାଲି) || n == 0)
	    ଭାଙ୍ଗ;
	ଯଦି (ga_grow(&out_ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n) == OK)
	{
	    mch_memmove((ଅକ୍ଷର *)out_ga.ga_data + out_ga.ga_len, buf, n);
	    out_ga.ga_len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    set_vim_var_nr(VV_SHELL_ERROR, (long)exit_code);

    ଯଦି (out_ga.ga_len > 0)
    {
	buffer = alloc(out_ga.ga_len + 1);
	ଯଦି (buffer != ଖାଲି)
	{
	    mch_memmove(buffer, out_ga.ga_data, out_ga.ga_len);
	    ଯଦି (ret_len == ଖାଲି)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		ପାଇଁ (i = 0; i < out_ga.ga_len; ++i)
		    ଯଦି (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[out_ga.ga_len] = NUL;
	    }
	    ଅନ୍ୟଥା
		*ret_len = out_ga.ga_len;
	}
    }
    ଅନ୍ୟଥା ଯଦି (ret_len != ଖାଲି)
	*ret_len = 0;

done:
    ga_clear(&cmd_ga);
    ga_clear(&out_ga);
    ଯଦି (hChildStdoutRd != INVALID_HANDLE_ମୂଲ୍ୟ)
	CloseHandle(hChildStdoutRd);
    ଯଦି (hChildStdoutWr != INVALID_HANDLE_ମୂଲ୍ୟ)
	CloseHandle(hChildStdoutWr);
    ଯଦି (hChildStdinRd != INVALID_HANDLE_ମୂଲ୍ୟ)
	CloseHandle(hChildStdinRd);
    ଫେରନ୍ତୁ buffer;
}
#endif // FEAT_EVAL

#ଯଦି defined(FEAT_JOB_CHANNEL)
    ଶୂନ୍ୟ
mch_job_start(ଅକ୍ଷର *cmd, job_T *job, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_ATTRIBUTES saAttr;
    channel_T		*channel = ଖାଲି;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;

    ଯଦି (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_ମୂଲ୍ୟ;
    ifd[1] = INVALID_HANDLE_ମୂଲ୍ୟ;
    ofd[0] = INVALID_HANDLE_ମୂଲ୍ୟ;
    ofd[1] = INVALID_HANDLE_ମୂଲ୍ୟ;
    efd[0] = INVALID_HANDLE_ମୂଲ୍ୟ;
    efd[1] = INVALID_HANDLE_ମୂଲ୍ୟ;
    ga_init2(&ga, ଆକାର(wchar_t), 500);

    jo = CreateJobObject(ଖାଲି, ଖାଲି);
    ଯଦି (jo == ଖାଲି)
    {
	job->jv_status = JOB_FAILED;
	ଯାଅ failed;
    }

    ଯଦି (options->jo_env != ଖାଲି)
	win32_build_env(options->jo_env, &ga, FALSE);

    ZeroMemory(&pi, ଆକାର(pi));
    ZeroMemory(&si, ଆକାର(si));
    si.cb = ଆକାର(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    saAttr.nLength = ଆକାର(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ଖାଲି;

    ଯଦି (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	ifd[0] = job_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
	ଯଦି (ifd[0] == INVALID_HANDLE_ମୂଲ୍ୟ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ଯାଅ failed;
	}
    }
    ଅନ୍ୟଥା ଯଦି (!use_null_for_in
	    && (!create_pipe_pair(ifd)
		|| !SetHandleInformation(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	ଯାଅ failed;

    ଯଦି (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	ofd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	ଯଦି (ofd[1] == INVALID_HANDLE_ମୂଲ୍ୟ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ଯାଅ failed;
	}
    }
    ଅନ୍ୟଥା ଯଦି (!use_null_for_out &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !SetHandleInformation(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	ଯାଅ failed;

    ଯଦି (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = job_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	ଯଦି (efd[1] == INVALID_HANDLE_ମୂଲ୍ୟ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ଯାଅ failed;
	}
    }
    ଅନ୍ୟଥା ଯଦି (!use_out_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !SetHandleInformation(efd[0], HANDLE_FLAG_INHERIT, 0)))
	ଯାଅ failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_ନିବେଶ = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_out_for_err ? ofd[1] : efd[1];

    ଯଦି (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	ଯଦି (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    ଯଦି (channel != ଖାଲି)
		++channel->ch_refcount;
	}
	ଅନ୍ୟଥା
	    channel = add_channel();
	ଯଦି (channel == ଖାଲି)
	    ଯାଅ failed;
    }

    ଯଦି (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_MODE |
	    CREATE_NEW_PROCESS_GROUP |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_data,
	    (ଅକ୍ଷର *)options->jo_cwd))
    {
	CloseHandle(jo);
	job->jv_status = JOB_FAILED;
	ଯାଅ failed;
    }

    ga_clear(&ga);

    ଯଦି (!AssignProcessToJobObject(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = ଖାଲି;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    job->jv_proc_info = pi;
    job->jv_job_object = jo;
    job->jv_status = JOB_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    ଯଦି (!use_out_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    job->jv_channel = channel;
    ଯଦି (channel != ଖାଲି)
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
    ଫେରନ୍ତୁ;

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

    ଅକ୍ଷର *
mch_job_status(job_T *job)
{
    DWORD dwExitCode = 0;

    ଯଦି (!GetExitCodeProcess(job->jv_proc_info.hProcess, &dwExitCode)
	    || dwExitCode != STILL_ACTIVE)
    {
	job->jv_exitval = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dwExitCode;
	ଯଦି (job->jv_status < JOB_ENDED)
	{
	    ch_log(job->jv_channel, "Job ended");
	    job->jv_status = JOB_ENDED;
	}
	ଫେରନ୍ତୁ "dead";
    }
    ଫେରନ୍ତୁ "run";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    HANDLE jobHandles[MAXIMUM_WAIT_OBJECTS];
    job_T *jobArray[MAXIMUM_WAIT_OBJECTS];
    job_T *job = job_list;

    ଯେତେବେଳେ (job != ଖାଲି)
    {
	DWORD n;
	DWORD result;

	ପାଇଁ (n = 0; n < MAXIMUM_WAIT_OBJECTS
				       && job != ଖାଲି; job = job->jv_next)
	{
	    ଯଦି (job->jv_status == JOB_STARTED)
	    {
		jobHandles[n] = job->jv_proc_info.hProcess;
		jobArray[n] = job;
		++n;
	    }
	}
	ଯଦି (n == 0)
	    ଜାରି;
	result = WaitForMultipleObjects(n, jobHandles, FALSE, 0);
	ଯଦି (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + n)
	{
	    job_T *wait_job = jobArray[result - WAIT_OBJECT_0];

	    ଯଦି (STRCMP(mch_job_status(wait_job), "dead") == 0)
		ଫେରନ୍ତୁ wait_job;
	}
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର BOOL
terminate_all(HANDLE process, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_ମୂଲ୍ୟ;
    DWORD	    pid = GetProcessId(process);

    ଯଦି (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	ଯଦି (h != INVALID_HANDLE_ମୂଲ୍ୟ)
	{
	    pe.dwSize = ଆକାର(PROCESSENTRY32);
	    ଯଦି (!Process32First(h, &pe))
		ଯାଅ theend;

	    କର
	    {
		ଯଦି (pe.th32ParentProcessID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    ଯଦି (ph != ଖାଲି)
		    {
			terminate_all(ph, code);
			CloseHandle(ph);
		    }
		}
	    } ଯେତେବେଳେ (Process32Next(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    ଫେରନ୍ତୁ TerminateProcess(process, code);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_signal_job(job_T *job, char_u *how)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

    ଯଦି (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	ଯଦି (job->jv_job_object != ଖାଲି)
	{
	    ଯଦି (job->jv_channel != ଖାଲି && job->jv_channel->ch_anonymous_pipe)
		job->jv_channel->ch_killing = TRUE;
	    ଫେରନ୍ତୁ TerminateJobObject(job->jv_job_object, (UINT)-1) ? OK : FAIL;
	}
	ଫେରନ୍ତୁ terminate_all(job->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    ଯଦି (!AttachConsole(job->jv_proc_info.dwProcessId))
	ଫେରନ୍ତୁ FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		job->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    ଫେରନ୍ତୁ ret;
}

/*
 * Clear the data related to "job".
 */
    ଶୂନ୍ୟ
mch_clear_job(job_T *job)
{
    ଯଦି (job->jv_status == JOB_FAILED)
	ଫେରନ୍ତୁ;

    ଯଦି (job->jv_job_object != ଖାଲି)
	CloseHandle(job->jv_job_object);
    CloseHandle(job->jv_proc_info.hProcess);
}
#endif


#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    ସ୍ଥିର ଶୂନ୍ୟ
termcap_mode_start(ଶୂନ୍ୟ)
{
    DWORD cmodein;

    ଯଦି (g_fTermcapMode)
	ଫେରନ୍ତୁ;

    SaveConsoleBuffer(&g_cbNonTermcap);

    ଯଦି (g_cbTermcap.IsValid)
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
    ଅନ୍ୟଥା
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
    ଯଦି (g_fMouseActive)
    {
	cmodein |= ENABLE_MOUSE_ନିବେଶ;
	cmodein &= ~ENABLE_QUICK_EDIT_MODE;
    }
    ଅନ୍ୟଥା
    {
	cmodein &= ~ENABLE_MOUSE_ନିବେଶ;
	cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    }
    cmodein |= ENABLE_WINDOW_ନିବେଶ;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_fTermcapMode = TRUE;
}


/*
 * End termcap mode
 */
    ସ୍ଥିର ଶୂନ୍ୟ
termcap_mode_end(ଶୂନ୍ୟ)
{
    DWORD cmodein;
    ConsoleBuffer *cb;
    COORD coord;
    DWORD dwDummy;

    ଯଦି (!g_fTermcapMode)
	ଫେରନ୍ତୁ;

    SaveConsoleBuffer(&g_cbTermcap);

    GetConsoleMode(g_hConIn, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_ନିବେଶ | ENABLE_WINDOW_ନିବେଶ);
    cmodein |= g_cmodein & ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(g_hConIn, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# ଅନ୍ୟଥା
    cb = &g_cbNonTermcap;
# endif
    RestoreConsoleBuffer(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    ଯଦି (exiting && use_alternate_screen_buffer)
	vtp_printf("\033[?1049l");

    ଯଦି (!USE_WT && (p_rs || exiting))
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
	ଯଦି (exiting && !p_rs)
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


#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_chars(
    COORD coord,
    DWORD n)
{
    ଯଦି (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_hConOut, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    ଅନ୍ୟଥା
    {
	set_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_screen(ଶୂନ୍ୟ)
{
    g_coord.X = g_coord.Y = 0;

    ଯଦି (!vtp_working)
	clear_chars(g_coord, Rows * Columns);
    ଅନ୍ୟଥା
    {
	set_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_to_end_of_display(ଶୂନ୍ୟ)
{
    COORD save = g_coord;

    ଯଦି (!vtp_working)
	clear_chars(g_coord, (Rows - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
clear_to_end_of_ଧାଡ଼ି(ଶୂନ୍ୟ)
{
    COORD save = g_coord;

    ଯଦି (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
scroll(unsigned c_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.ବାମ_3 + 1, g_srScrollRegion.ଶୀର୍ଷ_2 + 1);
    delete_ଧାଡ଼ିଗୁଡ଼ିକ(c_ଧାଡ଼ିଗୁଡ଼ିକ);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_scroll_region(
    unsigned ବାମ,
    unsigned ଶୀର୍ଷ,
    unsigned ଡାହାଣ,
    unsigned bottom)
{
    ଯଦି (ବାମ >= ଡାହାଣ
	    || ଶୀର୍ଷ >= bottom
	    || ଡାହାଣ > (unsigned) Columns - 1
	    || bottom > (unsigned) Rows - 1)
	ଫେରନ୍ତୁ;

    g_srScrollRegion.ବାମ_3 =   ବାମ;
    g_srScrollRegion.ଶୀର୍ଷ_2 =    ଶୀର୍ଷ;
    g_srScrollRegion.ଡାହାଣ_3 =  ଡାହାଣ;
    g_srScrollRegion.Bottom = bottom;
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_scroll_region_tb(
    unsigned ଶୀର୍ଷ,
    unsigned bottom)
{
    ଯଦି (ଶୀର୍ଷ >= bottom || bottom > (unsigned)Rows - 1)
	ଫେରନ୍ତୁ;

    g_srScrollRegion.ଶୀର୍ଷ_2 = ଶୀର୍ଷ;
    g_srScrollRegion.Bottom = bottom;
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_scroll_region_lr(
    unsigned ବାମ,
    unsigned ଡାହାଣ)
{
    ଯଦି (ବାମ >= ଡାହାଣ || ଡାହାଣ > (unsigned)Columns - 1)
	ଫେରନ୍ତୁ;

    g_srScrollRegion.ବାମ_3 = ବାମ;
    g_srScrollRegion.ଡାହାଣ_3 = ଡାହାଣ;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(unsigned c_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.ବାମ_3 + 1, g_srScrollRegion.ଶୀର୍ଷ_2 + 1);

    dest.X = g_srScrollRegion.ବାମ_3;
    dest.Y = g_coord.Y + c_ଧାଡ଼ିଗୁଡ଼ିକ;

    source.ବାମ_3   = g_srScrollRegion.ବାମ_3;
    source.ଶୀର୍ଷ_2	  = g_coord.Y;
    source.ଡାହାଣ_3  = g_srScrollRegion.ଡାହାଣ_3;
    source.Bottom = g_srScrollRegion.Bottom - c_ଧାଡ଼ିଗୁଡ଼ିକ;

    clip.ବାମ_3   = g_srScrollRegion.ବାମ_3;
    clip.ଶୀର୍ଷ_2    = g_coord.Y;
    clip.ଡାହାଣ_3  = g_srScrollRegion.ଡାହାଣ_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    ଯଦି (!USE_VTP)
	fill.Attributes = g_attrCurrent;
    ଅନ୍ୟଥା
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    ଯଦି (source.Bottom < dest.Y)
    {
	COORD coord;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା   i;

	coord.X = source.ବାମ_3;
	ପାଇଁ (i = clip.ଶୀର୍ଷ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ଡାହାଣ_3 - source.ବାମ_3 + 1);
	}
    }

    ଯଦି (vtp_working)
    {
	COORD coord;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	coord.X = source.ବାମ_3;
	ପାଇଁ (i = source.ଶୀର୍ଷ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ଡାହାଣ_3 - source.ବାମ_3 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    ସ୍ଥିର ଶୂନ୍ୟ
delete_ଧାଡ଼ିଗୁଡ଼ିକ(unsigned c_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    SMALL_RECT	    source, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    nb;

    gotoxy(g_srScrollRegion.ବାମ_3 + 1, g_srScrollRegion.ଶୀର୍ଷ_2 + 1);

    dest.X = g_srScrollRegion.ବାମ_3;
    dest.Y = g_coord.Y;

    source.ବାମ_3   = g_srScrollRegion.ବାମ_3;
    source.ଶୀର୍ଷ_2	  = g_coord.Y + c_ଧାଡ଼ିଗୁଡ଼ିକ;
    source.ଡାହାଣ_3  = g_srScrollRegion.ଡାହାଣ_3;
    source.Bottom = g_srScrollRegion.Bottom;

    clip.ବାମ_3   = g_srScrollRegion.ବାମ_3;
    clip.ଶୀର୍ଷ_2    = g_coord.Y;
    clip.ଡାହାଣ_3  = g_srScrollRegion.ଡାହାଣ_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    ଯଦି (!vtp_working)
	fill.Attributes = g_attrCurrent;
    ଅନ୍ୟଥା
	fill.Attributes = g_attrDefault;

    set_console_color_rgb();

    ScrollConsoleScreenBuffer(g_hConOut, &source, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (source.Bottom - source.ଶୀର୍ଷ_2) + 1;

    ଯଦି (nb < source.ଶୀର୍ଷ_2)
    {
	COORD coord;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା   i;

	coord.X = source.ବାମ_3;
	ପାଇଁ (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ଡାହାଣ_3 - source.ବାମ_3 + 1);
	}
    }

    ଯଦି (vtp_working)
    {
	COORD coord;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	coord.X = source.ବାମ_3;
	ପାଇଁ (i = nb; i <= source.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, source.ଡାହାଣ_3 - source.ବାମ_3 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gotoxy(
    unsigned x,
    unsigned y)
{
    ଯଦି (x < 1 || x > (unsigned)Columns || y < 1 || y > (unsigned)Rows)
	ଫେରନ୍ତୁ;

    ଯଦି (!USE_VTP)
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
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
textattr(WORD wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    SetConsoleTextAttribute(g_hConOut, wAttr);
}


    ସ୍ଥିର ଶୂନ୍ୟ
textcolor(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    ଯଦି (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    ଅନ୍ୟଥା
	vtp_sgr_bulk(wAttr);
}


    ସ୍ଥିର ଶୂନ୍ୟ
textbackground(WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    ଯଦି (!vtp_working)
	SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
    ଅନ୍ୟଥା
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    ସ୍ଥିର ଶୂନ୍ୟ
normvideo(ଶୂନ୍ୟ)
{
    ଯଦି (!vtp_working)
	textattr(g_attrDefault);
    ଅନ୍ୟଥା
	vtp_sgr_bulk(0);
}


ସ୍ଥିର WORD g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    ସ୍ଥିର ଶୂନ୍ୟ
standout(ଶୂନ୍ୟ)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((WORD) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    ସ୍ଥିର ଶୂନ୍ୟ
standend(ଶୂନ୍ୟ)
{
    ଯଦି (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    ଶୂନ୍ୟ
mch_set_normal_colors(ଶୂନ୍ୟ)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    cterm_normal_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_normal_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    ଯଦି (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	ଯଦି (*p == 'm' && n > 0)
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
    ସ୍ଥିର ଶୂନ୍ୟ
visual_bell(ଶୂନ୍ୟ)
{
    COORD   coordOrigin = {0, 0};
    WORD    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = ଖାଲି;

# ifdef FEAT_TERMGUICOLORS
    ଯଦି (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(WORD, Rows * Columns);
	ଯଦି (oldattrs == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ReadConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_hConOut, attrFlash, Rows * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    ଯଦି (oldattrs != ଖାଲି)
    {
	WriteConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    ଯଦି (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	ଫେରନ୍ତୁ;
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
    ସ୍ଥିର DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cbToWrite)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    ସ୍ଥିର DWORD    cells;
    ସ୍ଥିର WCHAR    *unicodebuf = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    unibuflen = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    length;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    ସ୍ଥିର WCHAR    *utf8spbuf = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    utf8splength;
    ସ୍ଥିର DWORD    utf8spcells;
    ସ୍ଥିର WCHAR    **utf8usingbuf = &unicodebuf;

    ଯଦି (cbToWrite != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	କର
	{
	    length = MultiByteToWideChar(cp, 0, (LPCSTR)pchBuf, cbToWrite,
							unicodebuf, unibuflen);
	    ଯଦି (length && length <= unibuflen)
		ଭାଙ୍ଗ;
	    vim_free(unicodebuf);
	    unicodebuf = length ? LALLOC_MULT(WCHAR, length) : ଖାଲି;
	    unibuflen = unibuflen ? 0 : length;
	} ଯେତେବେଳେ (TRUE);
	cells = mb_string2cells(pchBuf, cbToWrite);
    }
    ଅନ୍ୟଥା // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	ଯଦି (utf8usingbuf != &utf8spbuf)
	{
	    ଯଦି (utf8spbuf == ଖାଲି)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		length = MultiByteToWideChar(CP_UTF8, 0, " ", 1, ଖାଲି, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, length);
		ଯଦି (utf8spbuf != ଖାଲି)
		{
		    MultiByteToWideChar(CP_UTF8, 0, " ", 1, utf8spbuf, length);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = length;
		    utf8spcells = cells;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		utf8usingbuf = &utf8spbuf;
		length = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    ଯଦି (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	ଯଦି (WriteConsoleOutputCharacterW(g_hConOut, *utf8usingbuf, length,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (WriteConsoleW(g_hConOut, *utf8usingbuf, length, &cchwritten,
		    ଖାଲି) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    ଯଦି (cchwritten == (DWORD)length)
    {
	written = cbToWrite;
	g_coord.X += (SHORT)cells;
    }
    ଅନ୍ୟଥା
    {
	char_u *p = pchBuf;
	ପାଇଁ (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    ଯେତେବେଳେ (g_coord.X > g_srScrollRegion.ଡାହାଣ_3)
    {
	g_coord.X -= (SHORT) Columns;
	ଯଦି (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    ଯଦି (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    ଫେରନ୍ତୁ written;
}

    ସ୍ଥିର char_u *
get_seq(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *args,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଗଣନା_2,
    char_u *head)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc;
    char_u *p;

    ଯଦି (head == ଖାଲି || *head != '\033')
	ଫେରନ୍ତୁ ଖାଲି;

    argc = 0;
    p = head;
    ++p;
    କର
    {
	++p;
	args[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } ଯେତେବେଳେ (*p == ';');
    *ଗଣନା_2 = argc;

    ଫେରନ୍ତୁ p;
}

    ସ୍ଥିର char_u *
get_sgr(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *args,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଗଣନା_2,
    char_u *head)
{
    char_u *p = get_seq(args, ଗଣନା_2, head);

    ଫେରନ୍ତୁ (p && *p == 'm') ? ++p : ଖାଲି;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    ସ୍ଥିର char_u *
sgrn2(
    char_u *head,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	   n)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା args[16];
    char_u *p = get_sgr(args, &argc, head);

    ଫେରନ୍ତୁ p && argc == 5 && args[0] == n && args[1] == 2 ? p : ଖାଲି;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    ସ୍ଥିର char_u *
sgrnc(
    char_u *head,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	   n)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା args[16];
    char_u *p = get_sgr(args, &argc, head);

    ଫେରନ୍ତୁ p && argc == 1 && args[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : ଖାଲି;
}

    ସ୍ଥିର char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    ଯେତେବେଳେ (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    ଫେରନ୍ତୁ p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    ସ୍ଥିର char_u *
sgrn2c(
    char_u *head,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	   n)
{
    char_u *p = sgrn2(head, n);

    ଫେରନ୍ତୁ p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : ଖାଲି;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    ସ୍ଥିର char_u *
sgrn2cn(
    char_u *head,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    char_u *p = sgrn2(head, n);

    ଫେରନ୍ତୁ p && p[0] == 0x0a && p[1] == '\033' ? ++p : ଖାଲି;
}

ସ୍ଥିର bool in_dcs = false;	// inside a DCS (sixel) sequence
ସ୍ଥିର bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
write_vt(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ଖାଲି, 0);
    ଯଦି (wlen <= 0)
	ଫେରନ୍ତୁ;
    ws = ALLOC_MULT(WCHAR, wlen);
    ଯଦି (ws == ଖାଲି)
	ଫେରନ୍ତୁ;
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_hConOut, ws, wlen, &written, ଖାଲି);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    ସ୍ଥିର char_u *
skip_dcs(char_u *s, char_u *end)
{
    ପାଇଁ (char_u *p = s; p < end; ++p)
    {
	ଯଦି (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    in_dcs = false;
	    ଫେରନ୍ତୁ p + 1;
	}
	dcs_esc = *p == ESC;
    }
    ଫେରନ୍ତୁ end;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    ଶୂନ୍ୟ
mch_write(
    char_u  *s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len)
{
    char_u  *end = s + len;

# ifdef VIMDLL
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ;
# endif

    ଯଦି (!term_console)
    {
	ଲିଖ(1, s, (unsigned)len);
	ଫେରନ୍ତୁ;
    }

    // translate ESC | sequences into faked bios calls
    ଯେତେବେଳେ (len--)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା prefix = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	ଯଦି (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    ଫେରନ୍ତୁ;
	}

	ଯଦି (in_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(skip_dcs(s, end) - s);

	    ଯଦି (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    ଜାରି;
	}

	ଯେତେବେଳେ (s + ++prefix < end)
	{
	    ch = s[prefix];
	    ଯଦି (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		ଭାଙ୍ଗ;
	}

	ଯଦି (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    ଯଦି (prefix != 0)
		prefix = 1;
	}

	ଯଦି (prefix != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, prefix);
# ifdef MCH_WRITE_DUMP
	    ଯଦି (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, ଆକାର(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	ଅନ୍ୟଥା ଯଦି (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    ଯଦି (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.ବାମ_3 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    ଅନ୍ୟଥା
	    {
		gotoxy(g_srScrollRegion.ବାମ_3 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    ଯଦି (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	ଅନ୍ୟଥା ଯଦି (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.ବାମ_3+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ଯଦି (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	ଅନ୍ୟଥା ଯଦି (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    ଯଦି (g_coord.X > g_srScrollRegion.ବାମ_3)
		g_coord.X--;
	    ଅନ୍ୟଥା ଯଦି (g_coord.Y > g_srScrollRegion.ଶୀର୍ଷ_2)
	    {
		g_coord.X = g_srScrollRegion.ଡାହାଣ_3;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ଯଦି (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	ଅନ୍ୟଥା ଯଦି (s[0] == '\a')
	{
	    // \a, bell
	    MessageBeep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    ଯଦି (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	ଅନ୍ୟଥା ଯଦି (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    arg1 = 0, arg2 = 0, argc = 0, args[16];
	    char_u  *sp;

	    ବାଛ (s[2])
	    {
	    ସ୍ଥିତି '0': ସ୍ଥିତି '1': ସ୍ଥିତି '2': ସ୍ଥିତି '3': ସ୍ଥିତି '4':
	    ସ୍ଥିତି '5': ସ୍ଥିତି '6': ସ୍ଥିତି '7': ସ୍ଥିତି '8': ସ୍ଥିତି '9':
		ଯଦି (*(p = get_seq(args, &argc, s)) != 'm')
		    ଯାଅ notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		ଯଦି (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - s);
		    s = p;
		    ଭାଙ୍ଗ;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		ଯଦି (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		ଯଦି (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		ଯଦି (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		ଯଦି (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(args, &argc, p);

notsgr:
		arg1 = args[0];
		arg2 = args[1];
		ଯଦି (*p == 'm')
		{
		    ଯଦି (argc == 1 && args[0] == 0)
			normvideo();
		    ଅନ୍ୟଥା ଯଦି (argc == 1)
		    {
			ଯଦି (USE_VTP)
			    textcolor((WORD)arg1);
			ଅନ୍ୟଥା
			    textattr((WORD)arg1);
		    }
		    ଅନ୍ୟଥା ଯଦି (vtp_working)
			vtp_sgr_bulks(argc, args);
		}
		ଅନ୍ୟଥା ଯଦି (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		ଅନ୍ୟଥା ଯଦି (argc == 2 && *p == 'r')
		{
		    set_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		ଅନ୍ୟଥା ଯଦି (argc == 2 && *p == 'R')
		{
		    set_scroll_region_tb(arg1, arg2);
		}
		ଅନ୍ୟଥା ଯଦି (argc == 2 && *p == 'V')
		{
		    set_scroll_region_lr(arg1, arg2);
		}
		ଅନ୍ୟଥା ଯଦି (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.ଶୀର୍ଷ_2, g_coord.Y - arg1) + 1);
		}
		ଅନ୍ୟଥା ଯଦି (argc == 1 && *p == 'b')
		{
		    textbackground((WORD) arg1);
		}
		ଅନ୍ୟଥା ଯଦି (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.ଡାହାଣ_3, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		ଅନ୍ୟଥା ଯଦି (argc == 1 && *p == 'f')
		{
		    textcolor((WORD) arg1);
		}
		ଅନ୍ୟଥା ଯଦି (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		ଅନ୍ୟଥା ଯଦି (argc == 1 && *p == 'L')
		{
		    ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(arg1);
		}
		ଅନ୍ୟଥା ଯଦି (argc == 1 && *p == 'M')
		{
		    delete_ଧାଡ଼ିଗୁଡ଼ିକ(arg1);
		}

		len -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - s);
		s = p + 1;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.ଶୀର୍ଷ_2, g_coord.Y - 1) + 1);
		ଯାଅ got3;

	    ସ୍ଥିତି 'B':
		visual_bell();
		ଯାଅ got3;

	    ସ୍ଥିତି 'C':
		gotoxy(min(g_srScrollRegion.ଡାହାଣ_3, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		ଯାଅ got3;

	    ସ୍ଥିତି 'E':
		termcap_mode_end();
		ଯାଅ got3;

	    ସ୍ଥିତି 'F':
		standout();
		ଯାଅ got3;

	    ସ୍ଥିତି 'f':
		standend();
		ଯାଅ got3;

	    ସ୍ଥିତି 'H':
		gotoxy(1, 1);
		ଯାଅ got3;

	    ସ୍ଥିତି 'j':
		clear_to_end_of_display();
		ଯାଅ got3;

	    ସ୍ଥିତି 'J':
		clear_screen();
		ଯାଅ got3;

	    ସ୍ଥିତି 'K':
		clear_to_end_of_ଧାଡ଼ି();
		ଯାଅ got3;

	    ସ୍ଥିତି 'L':
		ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(1);
		ଯାଅ got3;

	    ସ୍ଥିତି 'M':
		delete_ଧାଡ଼ିଗୁଡ଼ିକ(1);
		ଯାଅ got3;

	    ସ୍ଥିତି 'S':
		termcap_mode_start();
		ଯାଅ got3;

	    ସ୍ଥିତି 'V':
		cursor_visible(TRUE);
		ଯାଅ got3;

	    ସ୍ଥିତି 'v':
		cursor_visible(FALSE);
		ଯାଅ got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    ଯଦି (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, ଆକାର(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	ଅନ୍ୟଥା ଯଦି (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    ଯେତେବେଳେ (s + l < end && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    ଯଦି (s + l < end && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		ଯଦି (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	ଅନ୍ୟଥା ଯଦି (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	l;

	    in_dcs = true;
	    dcs_esc = false;
	    l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(skip_dcs(s + 2, end) - s);
	    ଯଦି (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	ଅନ୍ୟଥା
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    ଯଦି (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, ଆକାର(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    ଯଦି (fdDump)
	fflush(fdDump);
# endif
}

#ଅନ୍ୟଥା // FEAT_GUI_MSWIN && !VIMDLL
    ଶୂନ୍ୟ
mch_write(
    char_u  *s UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len UNUSED)
{
    // never used
}

#endif


/*
 * Delay for "msec" milliseconds.
 */
    ଶୂନ୍ୟ
mch_delay(
    long    msec,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags UNUSED)
{
#ଯଦି defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)msec);	    // never wait for input
#ଅନ୍ୟଥା // Console
# ifdef VIMDLL
    ଯଦି (gui.in_use)
    {
	Sleep((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)msec);	    // never wait for input
	ଫେରନ୍ତୁ;
    }
# endif
    ଯଦି (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	ଯଦି (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    ଯେତେବେଳେ (msec > 0)
	    {
		mzvim_check_threads();
		ଯଦି (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	ଅନ୍ୟଥା
# endif
	    Sleep((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)msec);
    ଅନ୍ୟଥା
	WaitForChar(msec, FALSE);
#endif
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_remove(char_u *name)
{
    WCHAR	*wn;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    ଯଦି (mch_isdir(name) && mch_is_symbolic_link(name))
	ଫେରନ୍ତୁ mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_NORMAL);

    wn = enc_to_utf16(name, ଖାଲି);
    ଯଦି (wn == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    ଫେରନ୍ତୁ n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    ଶୂନ୍ୟ
mch_breakcheck(ପୂର୍ଣ୍ଣସଂଖ୍ୟା force UNUSED)
{
#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    ଯଦି (!gui.in_use)
# endif
	ଯଦି (g_fCtrlCPressed || g_fCBrkPressed)
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
mch_total_mem(ପୂର୍ଣ୍ଣସଂଖ୍ୟା special UNUSED)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dwLength = ଆକାର(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&ms);
    ଯଦି (ms.ullAvailVirtual < ms.ullTotalPhys)
    {
	// Process address space fits in physical RAM, use all of it.
	ଫେରନ୍ତୁ (long_u)(ms.ullAvailVirtual / 1024);
    }
    ଯଦି (ms.ullTotalPhys <= WINNT_RESERVE_BYTES)
    {
	// Catch old NT box or perverse hardware setup.
	ଫେରନ୍ତୁ (long_u)((ms.ullTotalPhys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    ଫେରନ୍ତୁ (long_u)((ms.ullTotalPhys - WINNT_RESERVE_BYTES) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)		// EW_* flags
{
    ଫେରନ୍ତୁ dos_expandpath(gap, path, 0, flags, FALSE);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	szTempFile[_MAX_PATH + 1];
    WCHAR	szNewPath[_MAX_PATH + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    ପାଇଁ (p = q = wold; *p != L'\0'; ++p)
    {
	ବାଛ (*p)
	{
	ସ୍ଥିତି L'/':
	ସ୍ଥିତି L'\\':
	ସ୍ଥିତି L':':
	    ଯଦି (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - q) < 8 || q[6] != L'~')
	ଫେରନ୍ତୁ (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    ଯଦି (GetFullPathNameW(wnew, _MAX_PATH, szNewPath, &p) == 0 || p == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    ଯଦି (GetTempFileNameW(szNewPath, L"VIM", 0, szTempFile) == 0)
	ଫେରନ୍ତୁ -2;

    // blow the temp file away
    ଯଦି (!DeleteFileW(szTempFile))
	ଫେରନ୍ତୁ -3;

    // rename old file to the temp file
    ଯଦି (!MoveFileW(wold, szTempFile))
	ଫେରନ୍ତୁ -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    ଯଦି ((hf = CreateFileW(wold, GENERIC_WRITE, 0, ଖାଲି, CREATE_NEW,
		    FILE_ATTRIBUTE_NORMAL, ଖାଲି)) == INVALID_HANDLE_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ -5;
    ଯଦି (!CloseHandle(hf))
	ଫେରନ୍ତୁ -6;

    // rename the temp file to the new file
    ଯଦି (!MoveFileW(szTempFile, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(ଶୂନ୍ୟ)MoveFileW(szTempFile, wold);
	ଫେରନ୍ତୁ -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(szTempFile);

    // finally, remove the empty old file
    ଯଦି (!DeleteFileW(wold))
	ଫେରନ୍ତୁ -8;

    ଫେରନ୍ତୁ 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_rename(
    ଅଚଳ ଅକ୍ଷର	*pszOldFile,
    ଅଚଳ ଅକ୍ଷର	*pszNewFile)
{
    WCHAR	*wold = ଖାଲି;
    WCHAR	*wnew = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = -1;

    wold = enc_to_utf16((char_u *)pszOldFile, ଖାଲି);
    wnew = enc_to_utf16((char_u *)pszNewFile, ଖାଲି);
    ଯଦି (wold != ଖାଲି && wnew != ଖାଲି)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    ଫେରନ୍ତୁ retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    ଅକ୍ଷର *
default_shell(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_access(ଅକ୍ଷର *n, ପୂର୍ଣ୍ଣସଂଖ୍ୟା p)
{
    HANDLE	hFile;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_to_utf16((char_u *)n, ଖାଲି);
    ଯଦି (wn == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ଯଦି (mch_isdir((char_u *)n))
    {
	WCHAR TempNameW[_MAX_PATH + 16] = L"";

	ଯଦି (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
	    WIN32_FIND_DATAW    d;

	    ପାଇଁ (i = 0; i < _MAX_PATH && wn[i] != 0; ++i)
		TempNameW[i] = wn[i];
	    ଯଦି (TempNameW[i - 1] != '\\' && TempNameW[i - 1] != '/')
		TempNameW[i++] = '\\';
	    TempNameW[i++] = '*';
	    TempNameW[i++] = 0;

	    hFile = FindFirstFileW(TempNameW, &d);
	    ଯଦି (hFile == INVALID_HANDLE_ମୂଲ୍ୟ)
		ଯାଅ getout;
	    ଅନ୍ୟଥା
		(ଶୂନ୍ୟ)FindClose(hFile);
	}

	ଯଦି (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    ଯଦି (!GetTempFileNameW(wn, L"VIM", 0, TempNameW))
		ଯାଅ getout;
	    ଅନ୍ୟଥା
		DeleteFileW(TempNameW);
	}
    }
    ଅନ୍ୟଥା
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_mode = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_mode, share_mode,
					     ଖାଲି, OPEN_EXISTING, 0, ଖାଲି);
	ଯଦି (hFile == INVALID_HANDLE_ମୂଲ୍ୟ)
	    ଯାଅ getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    ଫେରନ୍ତୁ retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_open(ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode)
{
    WCHAR	*wn;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		f;

    wn = enc_to_utf16((char_u *)name, ଖାଲି);
    ଯଦି (wn == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    f = _wopen(wn, flags, mode);
    vim_free(wn);
    ଫେରନ୍ତୁ f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *mode)
{
    WCHAR	*wn, *wm;
    FILE	*f = ଖାଲି;

#ଯଦି defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    ଅକ୍ଷର newMode = mode[strlen(mode) - 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldMode = 0;

    _get_fmode(&oldMode);
    ଯଦି (newMode == 't')
	_set_fmode(_O_TEXT);
    ଅନ୍ୟଥା ଯଦି (newMode == 'b')
	_set_fmode(O_ଦ୍ୱିମିକ_2);
#endif
    wn = enc_to_utf16((char_u *)name, ଖାଲି);
    wm = enc_to_utf16((char_u *)mode, ଖାଲି);
    ଯଦି (wn != ଖାଲି && wm != ଖାଲି)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#ଯଦି defined(DEBUG) && _MSC_VER >= 1400
    ଯଦି (oldMode != 0)
	_set_fmode(oldMode);
#endif
    ଫେରନ୍ତୁ f;
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
    ସ୍ଥିର ଶୂନ୍ୟ
copy_substream(HANDLE sh, ଶୂନ୍ୟ *context, WCHAR *to, WCHAR *substream, long len)
{
    HANDLE  hTo;
    WCHAR   *to_name;

    to_name = alloc((wcslen(to) + wcslen(substream) + 1) * ଆକାର(WCHAR));
    ଯଦି (to_name == ଖାଲି)
	ଫେରନ୍ତୁ;

    wcscpy(to_name, to);
    wcscat(to_name, substream);

    hTo = CreateFileW(to_name, GENERIC_WRITE, 0, ଖାଲି, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL, ଖାଲି);
    ଯଦି (hTo != INVALID_HANDLE_ମୂଲ୍ୟ)
    {
	long	done;
	DWORD	todo;
	DWORD	readcnt, written;
	ଅକ୍ଷର	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	ପାଇଁ (done = 0; done < len; done += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - done) > ଆକାର(buf) ? ଆକାର(buf)
						       : (size_t)(len - done));
	    ଯଦି (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(hTo, buf, todo, &written, ଖାଲି)
		    || written != todo)
		ଭାଙ୍ଗ;
	}
	CloseHandle(hTo);
    }

    ମୁକ୍ତ(to_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
copy_infostreams(char_u *from, char_u *to)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_STREAM_ID	sid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			headersize;
    WCHAR		streamname[_MAX_PATH];
    DWORD		readcount;
    ଶୂନ୍ୟ		*context = ଖାଲି;
    DWORD		lo, hi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len;

    // Convert the file names to wide characters.
    fromw = enc_to_utf16(from, ଖାଲି);
    tow = enc_to_utf16(to, ଖାଲି);
    ଯଦି (fromw != ଖାଲି && tow != ଖାଲି)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, ଖାଲି,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, ଖାଲି);
	ଯଦି (sh != INVALID_HANDLE_ମୂଲ୍ୟ)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    ପାଇଁ (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		ZeroMemory(&sid, ଆକାର(WIN32_STREAM_ID));
		headersize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((ଅକ୍ଷର *)&sid.cStreamName - (ଅକ୍ଷର *)&sid.dwStreamId);
		ଯଦି (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    ଭାଙ୍ଗ;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		ଯଦି (sid.dwStreamNameSize > 0)
		{
		    // Read the stream name.
		    ଯଦି (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dwStreamNameSize,
					  &readcount, FALSE, FALSE, &context))
			ଭାଙ୍ଗ;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / ଆକାର(WCHAR);
		    streamname[len] = 0;
		    ଯଦି (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (long)sid.Size.u.ତଳ_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(ଶୂନ୍ୟ)BackupSeek(sh, sid.Size.u.ତଳ_Part, sid.Size.u.ଉପର_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (ଶୂନ୍ୟ)BackupRead(sh, ଖାଲି, 0, &readcount, TRUE, FALSE, &context);

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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _FILE_FULL_EA_INFORMATION_ {
    ULONG  NextEntryOffset;
    UCHAR  Flags;
    UCHAR  EaNameLength;
    USHORT Ea_ମୂଲ୍ୟ_Length;
    CHAR   EaName[1];
} FILE_FULL_EA_INFORMATION_, *PFILE_FULL_EA_INFORMATION_;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _FILE_EA_INFORMATION_ {
    ULONG EaSize;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#ifndef PROTO
ପ୍ରକାରବ୍ୟାଖ୍ୟା NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions);
ପ୍ରକାରବ୍ୟାଖ୍ୟା NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
ପ୍ରକାରବ୍ୟାଖ୍ୟା NTSTATUS (NTAPI *PfnNtSetEaFile)(
	HANDLE		    FileHandle,
	PIO_STATUS_BLOCK    IoStatusBlock,
	PVOID		    Buffer,
	ULONG		    Length);
ପ୍ରକାରବ୍ୟାଖ୍ୟା NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	BOOLEAN ReturnSingleEntry,
	PVOID EaList,
	ULONG EaListLength,
	PULONG EaIndex,
	BOOLEAN RestartScan);
ପ୍ରକାରବ୍ୟାଖ୍ୟା NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_STATUS_BLOCK	IoStatusBlock,
	PVOID			FileInformation,
	ULONG			Length,
	FILE_INFORMATION_CLASS FileInformationClass);
ପ୍ରକାରବ୍ୟାଖ୍ୟା VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString);

PfnNtOpenFile pNtOpenFile = ଖାଲି;
PfnNtClose pNtClose = ଖାଲି;
PfnNtSetEaFile pNtSetEaFile = ଖାଲି;
PfnNtQueryEaFile pNtQueryEaFile = ଖାଲି;
PfnNtQueryInformationFile pNtQueryInformationFile = ଖାଲି;
PfnRtlInitUnicodeString pRtlInitUnicodeString = ଖାଲି;
#endif

/*
 * Load ntdll.dll functions.
 */
    ସ୍ଥିର BOOL
load_ntdll(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	loaded = -1;

    ଯଦି (loaded != -1)
	ଫେରନ୍ତୁ (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    ଯଦି (hNtdll != ଖାଲି)
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
    ଯଦି (pNtOpenFile == ଖାଲି
	    || pNtClose == ଖାଲି
	    || pNtSetEaFile == ଖାଲି
	    || pNtQueryEaFile == ଖାଲି
	    || pNtQueryInformationFile == ଖାଲି
	    || pRtlInitUnicodeString == ଖାଲି)
	loaded = FALSE;
    ଅନ୍ୟଥା
	loaded = TRUE;
    ଫେରନ୍ତୁ (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
copy_extattr(char_u *from, char_u *to)
{
    char_u		    *fromf = ଖାଲି;
    char_u		    *tof = ଖାଲି;
    WCHAR		    *fromw = ଖାଲି;
    WCHAR		    *tow = ଖାଲି;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_ATTRIBUTES	    oa;
    IO_STATUS_BLOCK	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    ଶୂନ୍ୟ		    *ea = ଖାଲି;

    ଯଦି (!load_ntdll())
	ଫେରନ୍ତୁ;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(to) + 5);
    ଯଦି (fromf == ଖାଲି || tof == ଖାଲି)
	ଯାଅ theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), to);

    // Convert the names to wide characters.
    fromw = enc_to_utf16(fromf, ଖାଲି);
    tow = enc_to_utf16(tof, ଖାଲି);
    ଯଦି (fromw == ଖାଲି || tow == ଖାଲି)
	ଯାଅ theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    InitializeObjectAttributes(&oa, &u, 0, ଖାଲି, ଖାଲି);
    ଯଦି (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	ଯାଅ theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, ଆକାର(eainfo),
	    FileEaInformation);
    ଯଦି (eainfo.EaSize != 0)
    {
	ea = alloc(eainfo.EaSize);
	ଯଦି (ea != ଖାଲି)
	{
	    ଯଦି (pNtQueryEaFile(h, &iosb, ea, eainfo.EaSize, FALSE,
			ଖାଲି, 0, ଖାଲି, TRUE) != STATUS_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    ଯଦି (ea != ଖାଲି)
    {
	pRtlInitUnicodeString(&u, tow);
	InitializeObjectAttributes(&oa, &u, 0, ଖାଲି, ଖାଲି);
	ଯଦି (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != STATUS_SUCCESS)
	    ଯାଅ theend;

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_copy_file_attribute(char_u *from, char_u *to)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, to);
    copy_extattr(from, to);
    ଫେରନ୍ତୁ 0;
}


/*
 * The command line arguments in UTF-16
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nArgsW = 0;
ସ୍ଥିର LPWSTR	*ArglistW = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	global_argc = 0;
ସ୍ଥିର ଅକ୍ଷର	**global_argv;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	used_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*used_file_indexes = ଖାଲି; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	used_file_ଗଣନା = 0;	// nr of entries in used_file_indexes
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	used_file_literal = FALSE;  // take file names literally
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	used_file_full_path = FALSE;  // file name was full path
#ifdef FEAT_DIFF
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	used_file_diff_mode = FALSE;  // file name was with diff mode
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	used_alist_ଗଣନା = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_cmd_argsW(ଅକ୍ଷର ***argvp)
{
    ଅକ୍ଷର	**argv = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    free_cmd_argsW();
    ArglistW = Command_ଧାଡ଼ି_To_Argv_W(Get_Command_ଧାଡ଼ି_W(), &nArgsW);
    ଯଦି (ArglistW != ଖାଲି)
    {
	argv = alloc((nArgsW + 1) * ଆକାର(ଅକ୍ଷର *));
	ଯଦି (argv != ଖାଲି)
	{
	    argc = nArgsW;
	    argv[argc] = ଖାଲି;
	    ପାଇଁ (i = 0; i < argc; ++i)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;

		// Convert each Unicode argument to UTF-8.
		WideCharToMultiByte_alloc(CP_UTF8, 0,
				ArglistW[i], (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		ଯଦି (argv[i] == ଖାଲି)
		{
		    // Out of memory, clear everything.
		    ଯେତେବେଳେ (i > 0)
			ମୁକ୍ତ(argv[--i]);
		    ମୁକ୍ତ(argv);
		    argv = ଖାଲି;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    ଯଦି (argc > 0)
    {
	ଯଦି (used_file_indexes != ଖାଲି)
	    ମୁକ୍ତ(used_file_indexes);
	used_file_indexes = alloc(argc * ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
    }

    ଯଦି (argvp != ଖାଲି)
	*argvp = argv;
    ଫେରନ୍ତୁ argc;
}

    ଶୂନ୍ୟ
free_cmd_argsW(ଶୂନ୍ୟ)
{
    ଯଦି (ArglistW == ଖାଲି)
	ଫେରନ୍ତୁ;

    GlobalFree(ArglistW);
    ArglistW = ଖାଲି;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    ଶୂନ୍ୟ
used_file_arg(ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା literal, ପୂର୍ଣ୍ଣସଂଖ୍ୟା full_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_mode)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (used_file_indexes == ଖାଲି)
	ଫେରନ୍ତୁ;
    ପାଇଁ (i = used_file_argc + 1; i < global_argc; ++i)
	ଯଦି (STRCMP(global_argv[i], name) == 0)
	{
	    used_file_argc = i;
	    used_file_indexes[used_file_ଗଣନା++] = i;
	    ଭାଙ୍ଗ;
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
    ଶୂନ୍ୟ
set_alist_ଗଣନା(ଶୂନ୍ୟ)
{
    used_alist_ଗଣନା = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    ଶୂନ୍ୟ
fix_arg_enc(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    char_u	*str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    ଯଦି (global_argc != nArgsW
	    || ArglistW == ଖାଲି
	    || used_file_indexes == ଖାଲି
	    || used_file_ଗଣନା == 0
	    || used_alist_ଗଣନା != GARGCOUNT)
	ଫେରନ୍ତୁ;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, GARGCOUNT);
    ଯଦି (fnum_list == ଖାଲି)
	ଫେରନ୍ତୁ;		// out of memory
    ପାଇଁ (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    ଯଦି (ga_grow(&global_alist.al_ga, used_file_ଗଣନା) == FAIL)
	ଫେରନ୍ତୁ;		// out of memory

    ପାଇଁ (i = 0; i < used_file_ଗଣନା; ++i)
    {
	idx = used_file_indexes[i];
	str = utf16_to_enc(ArglistW[idx], ଖାଲି);
	ଯଦି (str != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା literal = used_file_literal;

#ifdef FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    ଯଦି (used_file_diff_mode && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *tail;
		string_T    ret;

		tail = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), tail, STRLEN(tail), TRUE, &ret);
		ଯଦି (ret.string != ଖାଲି)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#endif
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    ଯଦି (used_file_literal)
		buf_set_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    ଯଦି (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		ଯଦି (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_add(&global_alist, str, literal ? 2 : 0);
	}
    }

    ଯଦି (!used_file_literal)
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
	alist_expand(fnum_list, used_alist_ଗଣନା);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    ଯଦି (curwin->w_arg_idx == 0 && curbuf->b_fname == ଖାଲି)
    {
	do_cmdline_cmd((char_u *)":rewind");
	ଯଦି (GARGCOUNT == 1 && used_file_full_path
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    last_chdir_reason = "drop";
    }

    set_alist_ଗଣନା();
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_setenv(ଅକ୍ଷର *var, ଅକ୍ଷର *ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x UNUSED)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(ମୂଲ୍ୟ) + 2);
    ଯଦି (envbuf == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    sprintf((ଅକ୍ଷର *)envbuf, "%s=%s", var, ମୂଲ୍ୟ);

    p = enc_to_utf16(envbuf, ଖାଲି);

    vim_free(envbuf);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    _wputenv(p);
#ifdef libintl_wputenv
    libintl_wputenv(p);
#endif
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    ଫେରନ୍ତୁ 0;
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

    ସ୍ଥିର ଶୂନ୍ୟ
vtp_flag_init(ଶୂନ୍ୟ)
{
#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   mode;
    HANDLE  out;

# ifdef VIMDLL
    ଯଦି (!gui.in_use)
# endif
    {
	out = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_version >= VTP_FIRST_SUPPORT_BUILD) ? 1 : 0;
	GetConsoleMode(out, &mode);
	mode |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	ଯଦି (SetConsoleMode(out, mode) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_buffer = win_version >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#endif

    ଯଦି (win_version >= CONPTY_FIRST_SUPPORT_BUILD)
	conpty_working = 1;
    ଯଦି (win_version >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    ଯଦି (win_version <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    ଯଦି (win_version <= CONPTY_1909_BUILD)
	conpty_type = 2;
    ଯଦି (win_version <= CONPTY_1903_BUILD)
	conpty_type = 2;
    ଯଦି (win_version < CONPTY_FIRST_SUPPORT_BUILD)
	conpty_type = 1;

    ଯଦି (win_version >= CONPTY_NEXT_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ସ୍ଥିର ଶୂନ୍ୟ
vtp_init(ଶୂନ୍ୟ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = ଆକାର(csbi);
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

    ସ୍ଥିର ଶୂନ୍ୟ
vtp_exit(ଶୂନ୍ୟ)
{
    restore_console_color_rgb();
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vtp_printf(
    ଅକ୍ଷର *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   result;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;

    ଯଦି (silent_mode)
	ଫେରନ୍ତୁ 0;

    va_start(list, format);
    len = vim_vsnprintf((ଅକ୍ଷର *)buf, 100, (ଅକ୍ଷର *)format, list);
    va_end(list);
    WriteConsoleA(g_hConOut, buf, (DWORD)len, &result, ଖାଲି);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result;
}

    ସ୍ଥିର ଶୂନ୍ୟ
vtp_sgr_bulk(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା args[1];

    args[0] = arg;
    vtp_sgr_bulks(1, args);
}

# define FAST256(x) \
    ଯଦି ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    ସ୍ଥିତି x: \
	FAST256(newargs[x - 1]);

    ସ୍ଥିର ଶୂନ୍ୟ
vtp_sgr_bulks(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *args)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    in, out;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    newargs[16];
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sgrfgr = -1, sgrfgg, sgrfgb;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sgrbgr = -1, sgrbgg, sgrbgb;

    ଯଦି (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	ଫେରନ୍ତୁ;
    }

    in = out = 0;
    ଯେତେବେଳେ (in < argc)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା s = args[in];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା copylen = 1;

	ଯଦି (s == 38)
	{
	    ଯଦି (argc - in >= 5 && args[in + 1] == 2)
	    {
		ଯଦି (sgrfgr == args[in + 2] && sgrfgg == args[in + 3]
						     && sgrfgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		ଅନ୍ୟଥା
		{
		    sgrfgr = args[in + 2];
		    sgrfgg = args[in + 3];
		    sgrfgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (s == 48)
	{
	    ଯଦି (argc - in >= 5 && args[in + 1] == 2)
	    {
		ଯଦି (sgrbgr == args[in + 2] && sgrbgg == args[in + 3]
						     && sgrbgb == args[in + 4])
		{
		    in += 5;
		    copylen = 0;
		}
		ଅନ୍ୟଥା
		{
		    sgrbgr = args[in + 2];
		    sgrbgg = args[in + 3];
		    sgrbgb = args[in + 4];
		    copylen = 5;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (argc - in >= 3 && args[in + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (30 <= s && s <= 39)
	    sgrfgr = -1;
	ଅନ୍ୟଥା ଯଦି (90 <= s && s <= 97)
	    sgrfgr = -1;
	ଅନ୍ୟଥା ଯଦି (40 <= s && s <= 49)
	    sgrbgr = -1;
	ଅନ୍ୟଥା ଯଦି (100 <= s && s <= 107)
	    sgrbgr = -1;
	ଅନ୍ୟଥା ଯଦି (s == 0)
	    sgrfgr = sgrbgr = -1;

	ଯେତେବେଳେ (copylen--)
	    newargs[out++] = args[in++];
    }

    p = &buf[ଆକାର(buf) - 1];
    *p-- = 'm';

    ବାଛ (out)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n, m;
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
	WriteConsoleA(g_hConOut, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, ଖାଲି);
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ଭାଙ୍ଗ;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
wt_init(ଶୂନ୍ୟ)
{
    wt_working = mch_getenv("WT_SESSION") != ଖାଲି;
}

# ifdef FEAT_TERMGUICOLORS
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ctermtoxterm(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    ଫେରନ୍ତୁ (((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)r << 16) | ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)g << 8) | (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)b);
}
# endif

    ସ୍ଥିର ଶୂନ୍ୟ
set_console_color_rgb(ଶୂନ୍ୟ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    guicolor_T	fg, bg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ctermfg, ctermbg;

    ଯଦି (!vtp_working)
	ଫେରନ୍ତୁ;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    ଯଦି (p_tgc || t_colors >= 256)
    {
	term_fg_rgb_color(fg);
	term_bg_rgb_color(bg);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (use_alternate_screen_buffer)
	ଫେରନ୍ତୁ;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cbSize = ଆକାର(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = ଆକାର(csbi);
    csbi.srWindow.ଡାହାଣ_3 += 1;
    csbi.srWindow.Bottom += 1;
    store_console_bg_rgb = csbi.ColorTable[g_color_index_bg];
    store_console_fg_rgb = csbi.ColorTable[g_color_index_fg];
    csbi.ColorTable[g_color_index_bg] = (COLORREF)bg;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)fg;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

# ଯଦି defined(FEAT_TERMGUICOLORS)
    ଶୂନ୍ୟ
get_default_console_color(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cterm_fg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctermfg = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctermbg = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    ଯଦି (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    ଯଦି (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	ଯଦି (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	ଯଦି (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	ଅନ୍ୟଥା
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_normal_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    ଯଦି (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	ଯଦି (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	ଯଦି (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
reset_console_color_rgb(ଶୂନ୍ୟ)
{
# ifdef FEAT_TERMGUICOLORS
    ଯଦି (use_alternate_screen_buffer)
	ଫେରନ୍ତୁ;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = ଆକାର(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = ଆକାର(csbi);
    csbi.srWindow.ଡାହାଣ_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)store_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)store_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
restore_console_color_rgb(ଶୂନ୍ୟ)
{
# ifdef FEAT_TERMGUICOLORS
    ଯଦି (use_alternate_screen_buffer)
	ଫେରନ୍ତୁ;

    CONSOLE_SCREEN_BUFFER_INFOEX csbi;

    csbi.cbSize = ଆକାର(csbi);
    GetConsoleScreenBufferInfoEx(g_hConOut, &csbi);

    csbi.cbSize = ଆକାର(csbi);
    csbi.srWindow.ଡାହାଣ_3 += 1;
    csbi.srWindow.Bottom += 1;
    csbi.ColorTable[g_color_index_bg] = (COLORREF)save_console_bg_rgb;
    csbi.ColorTable[g_color_index_fg] = (COLORREF)save_console_fg_rgb;
    SetConsoleScreenBufferInfoEx(g_hConOut, &csbi);
# endif
}

    ଶୂନ୍ୟ
control_console_color_rgb(ଶୂନ୍ୟ)
{
    ଯଦି (vtp_working)
	set_console_color_rgb();
    ଅନ୍ୟଥା
	reset_console_color_rgb();
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
use_vtp(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ USE_VTP;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_term_win32(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ T_NAME != ଖାଲି && STRCMP(T_NAME, "win32") == 0;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_vtp_working(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ vtp_working;
}

#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_conpty_working(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ conpty_working;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_conpty_type(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ conpty_type;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_conpty_stable(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ conpty_stable;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_conpty_fix_type(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ conpty_fix_type;
}

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ଶୂନ୍ୟ
resize_console_buf(ଶୂନ୍ୟ)
{
    ଯଦି (use_alternate_screen_buffer)
	ଫେରନ୍ତୁ;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    ଯଦି (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	ଫେରନ୍ତୁ;

    coord.X = SRWIDTH(csbi.srWindow);
    coord.Y = SRHEIGHT(csbi.srWindow);
    SetConsoleScreenBufferSize(g_hConOut, coord);

    newsize.ବାମ_3 = 0;
    newsize.ଶୀର୍ଷ_2 = 0;
    newsize.ଡାହାଣ_3 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    SetConsoleWindowInfo(g_hConOut, TRUE, &newsize);

    SetConsoleScreenBufferSize(g_hConOut, coord);
}
#endif

    ଅକ୍ଷର *
GetWin32Error(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ଅକ୍ଷର	*oldmsg = ଖାଲି;
    ଅକ୍ଷର	*acp_ସନ୍ଦେଶ = ଖାଲି;
    DWORD	acp_len;
    ଅକ୍ଷର	*enc_ସନ୍ଦେଶ = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		enc_len = 0;

    // get formatted message from OS
    acp_len = FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
	    ଖାଲି, GetLastError(), 0, (LPSTR)&acp_ସନ୍ଦେଶ, 0, ଖାଲି);
    ଯଦି (acp_len == 0 || acp_ସନ୍ଦେଶ == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // clean oldmsg if remained.
    ଯଦି (oldmsg != ଖାଲି)
    {
	vim_free(oldmsg);
	oldmsg = ଖାଲି;
    }

    acp_to_enc((char_u *)acp_ସନ୍ଦେଶ, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)acp_len, (char_u **)&enc_ସନ୍ଦେଶ, &enc_len);
    LocalFree(acp_ସନ୍ଦେଶ);
    ଯଦି (enc_ସନ୍ଦେଶ == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // remove trailing \r\n
    ଅକ୍ଷର *pcrlf = strstr(enc_ସନ୍ଦେଶ, "\r\n");
    ଯଦି (pcrlf != ଖାଲି)
	*pcrlf = NUL;

    oldmsg = enc_ସନ୍ଦେଶ;
    ଫେରନ୍ତୁ enc_ସନ୍ଦେଶ;
}

#ଯଦି defined(FEAT_RELTIME)
ସ୍ଥିର HANDLE   timer_handle;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା      timer_active = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
ସ୍ଥିର sig_atomic_t timeout_flags[2];
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    timeout_flag_idx = 0;
ସ୍ଥିର sig_atomic_t *timeout_flag = &timeout_flags[0];


    ସ୍ଥିର ଶୂନ୍ୟ CALLBACK
set_flag(ଶୂନ୍ୟ *param, BOOLEAN unused2 UNUSED)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    ଶୂନ୍ୟ
stop_timeout(ଶୂନ୍ୟ)
{
    ଯଦି (timer_active)
    {
	BOOL ret = DeleteTimerQueueTimer(ଖାଲି, timer_handle, ଖାଲି);
	timer_active = FALSE;
	ଯଦି (!ret && GetLastError() != ERROR_IO_PENDING)
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
    ଅସ୍ଥିର sig_atomic_t *
start_timeout(long msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = CreateTimerQueueTimer(
	    &timer_handle, ଖାଲି, set_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    ଯଦି (!ret)
    {
	semsg(_(e_could_not_set_timeout_str), GetWin32Error());
    }
    ଅନ୍ୟଥା
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_active = TRUE;
	*timeout_flag = FALSE;
    }
    ଫେରନ୍ତୁ timeout_flag;
}
#endif
