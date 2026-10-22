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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
# include "if_mzsch.h"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <process.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <winternl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <direct.h>

#ਜੇ !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
# include <tlhelp32.h>
#ਜੇ_ਅੰਤ

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MCH_WRITE_DUMP
FILE* fdDump = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
ਸਥਿਰ HANDLE g_h_Con_ਵਿੱਚ  = INVALID_HANDLE_ਮੁੱਲ;
ਸਥਿਰ HANDLE g_h_Con_ਬਾਹਰਲਾ = INVALID_HANDLE_ਮੁੱਲ;

// Win32 Screen buffer,coordinate,console I/O information
ਸਥਿਰ SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
ਸਥਿਰ COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
ਸਥਿਰ ਸ਼ਬਦ_2  g_attrDefault = 7;  // lightgray text on black background
ਸਥਿਰ ਸ਼ਬਦ_2  g_attrCurrent;

ਸਥਿਰ ਪੂਰਨ g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
ਸਥਿਰ ਪੂਰਨ g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
ਸਥਿਰ ਪੂਰਨ g_fForceExit = FALSE;    // set when forcefully exiting

ਸਥਿਰ ਖਾਲੀ scroll(ਬਿਨਾਨਿਸ਼ਾਨੀ c_ਸਤਰਾਂ);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_scroll_region(ਬਿਨਾਨਿਸ਼ਾਨੀ ਖੱਬਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਿਖਰ,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੱਜਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ bottom);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_scroll_region_tb(ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਿਖਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ bottom);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_scroll_region_lr(ਬਿਨਾਨਿਸ਼ਾਨੀ ਖੱਬਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੱਜਾ);
ਸਥਿਰ ਖਾਲੀ ਪਾਓ_ਸਤਰਾਂ(ਬਿਨਾਨਿਸ਼ਾਨੀ c_ਸਤਰਾਂ);
ਸਥਿਰ ਖਾਲੀ delete_ਸਤਰਾਂ(ਬਿਨਾਨਿਸ਼ਾਨੀ c_ਸਤਰਾਂ);
ਸਥਿਰ ਖਾਲੀ gotoxy(ਬਿਨਾਨਿਸ਼ਾਨੀ x, ਬਿਨਾਨਿਸ਼ਾਨੀ y);
ਸਥਿਰ ਖਾਲੀ standout(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ s_cursor_visible = TRUE;
ਸਥਿਰ ਪੂਰਨ did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ_ਗੰਢ_S
{
    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ ir;
    ਬਣਤਰ ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ_ਗੰਢ_S *ਅਗਲਾ;
} ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ_ਗੰਢ_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ_S
{
    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ_ਗੰਢ_T *ਸਿਰ;
    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ_ਗੰਢ_T *ਪੂਛ;
    ਪੂਰਨ ਲੰਮਾਈ;
} ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ_T;
ਸਥਿਰ ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ_T ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ;
ਸਥਿਰ ਪੂਰਨ read_ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ(ਨਿਵੇਸ਼_ਰਿਕਾਰਡ* irEvents, ਪੂਰਨ n_Max_ਲੰਮਾਈ);
ਸਥਿਰ ਪੂਰਨ write_ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ(ਨਿਵੇਸ਼_ਰਿਕਾਰਡ* irEvents, ਪੂਰਨ n_ਲੰਮਾਈ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
ਸਥਿਰ ਪੂਰਨ s_dont_use_vimrun = TRUE;
ਸਥਿਰ ਪੂਰਨ need_vimrun_warning = FALSE;
ਸਥਿਰ string_T vimrun_ਰਸਤਾ = {(char_u *)"vimrun ", 7};
ਸਥਿਰ ਪੂਰਨ vimrun_ਰਸਤਾ_allocated = FALSE;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ win32_getattrs(char_u *name);
ਸਥਿਰ ਪੂਰਨ win32_setattrs(char_u *name, ਪੂਰਨ attrs);
ਸਥਿਰ ਪੂਰਨ win32_ਸੈੱਟ_archive(char_u *name);

ਸਥਿਰ ਪੂਰਨ conpty_working = 0;
ਸਥਿਰ ਪੂਰਨ conpty_type = 0;
ਸਥਿਰ ਪੂਰਨ conpty_stable = 0;
ਸਥਿਰ ਪੂਰਨ conpty_fix_type = 0;
ਸਥਿਰ ਖਾਲੀ vtp_flag_init(ਖਾਲੀ);

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ਸਥਿਰ ਪੂਰਨ vtp_working = 0;
ਸਥਿਰ ਖਾਲੀ vtp_init(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ vtp_exit(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ vtp_sgr_bulk(ਪੂਰਨ arg);
ਸਥਿਰ ਖਾਲੀ vtp_sgr_bulks(ਪੂਰਨ argc, ਪੂਰਨ *argv);

ਸਥਿਰ ਪੂਰਨ wt_working = 0;
ਸਥਿਰ ਖਾਲੀ wt_init(ਖਾਲੀ);

ਸਥਿਰ ਪੂਰਨ g_color_ਸੂਚਕਾਂਕ_bg = 0;
ਸਥਿਰ ਪੂਰਨ g_color_ਸੂਚਕਾਂਕ_fg = 7;

# ifdef FEAT_TERMGUICOLORS
ਸਥਿਰ guicolor_T save_console_bg_rgb;
ਸਥਿਰ guicolor_T save_console_fg_rgb;
ਸਥਿਰ guicolor_T store_console_bg_rgb;
ਸਥਿਰ guicolor_T store_console_fg_rgb;
ਸਥਿਰ ਪੂਰਨ default_console_color_bg = 0x000000; // black
ਸਥਿਰ ਪੂਰਨ default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_ਪਦ_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# ਨਹੀਂਤਾਂ
#  define USE_VTP		0
#  define USE_WT		0
# endif

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_console_color_rgb(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ reset_console_color_rgb(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ restore_console_color_rgb(ਖਾਲੀ);
#ਜੇ_ਅੰਤ  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#ਜੇ_ਅੰਤ

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ਸਥਿਰ ਪੂਰਨ suppress_winsize = 1;	// don't fiddle with console
#ਜੇ_ਅੰਤ

ਸਥਿਰ WCHAR *exe_pathw = ਨਿਰਾ;

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ਸਥਿਰ BOOL use_alternate_screen_ਬਫਰ = FALSE;
#ਜੇ_ਅੰਤ

ਬਾਹਰੀ DWORD win_ਸੰਸਕਰਣ; // this is in os_mswin.c

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ਸਥਿਰ BOOL
is_ambiwidth_event(
    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ *ir)
{
    ਵਾਪਸ ir->EventType == ਕੁੰਜੀ_EVENT
		&& ir->Event.ਕੁੰਜੀ_Event_2.b_ਕੁੰਜੀ_ਥੱਲੇ
		&& ir->Event.ਕੁੰਜੀ_Event_2.w_Repeat_ਗਿਣਤੀ == 1
		&& ir->Event.ਕੁੰਜੀ_Event_2.w_Virtual_ਕੁੰਜੀ_ਕੋਡ == 0x12
		&& ir->Event.ਕੁੰਜੀ_Event_2.w_Virtual_ਜਾਂਚ_ਕੋਡ == 0x38
		&& ir->Event.ਕੁੰਜੀ_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.ਕੁੰਜੀ_Event_2.dw_Control_ਕੁੰਜੀ_State == 2;
}

    ਸਥਿਰ ਖਾਲੀ
make_ambiwidth_event(
    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ *ਥੱਲੇ,
    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ *ਉੱਪਰ)
{
    ਥੱਲੇ->Event.ਕੁੰਜੀ_Event_2.w_Virtual_ਕੁੰਜੀ_ਕੋਡ = 0;
    ਥੱਲੇ->Event.ਕੁੰਜੀ_Event_2.w_Virtual_ਜਾਂਚ_ਕੋਡ = 0;
    ਥੱਲੇ->Event.ਕੁੰਜੀ_Event_2.uChar.UnicodeChar
				    = ਉੱਪਰ->Event.ਕੁੰਜੀ_Event_2.uChar.UnicodeChar;
    ਥੱਲੇ->Event.ਕੁੰਜੀ_Event_2.dw_Control_ਕੁੰਜੀ_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    ਸਥਿਰ BOOL
read_console_ਨਿਵੇਸ਼(
    HANDLE	    h_ਨਿਵੇਸ਼,
    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ    *lp_ਬਫਰ,
    ਪੂਰਨ		    n_ਲੰਮਾਈ,
    LPDWORD	    lpEvents)
{
    ਗਿਣਤੀ
    {
	IRSIZE = 10
    };
    ਸਥਿਰ ਨਿਵੇਸ਼_ਰਿਕਾਰਡ s_irCache[IRSIZE];
    ਸਥਿਰ DWORD s_dw_ਸੂਚਕਾਂਕ = 0;
    ਸਥਿਰ DWORD s_dwMax = 0;
    DWORD dwEvents;
    ਪੂਰਨ ਸਿਰ;
    ਪੂਰਨ ਪੂਛ;
    ਪੂਰਨ i;
    ਸਥਿਰ ਨਿਵੇਸ਼_ਰਿਕਾਰਡ s_irPseudo;

    ਜੇ (s_dwMax == 0 && ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਲੰਮਾਈ > 0)
    {
	dwEvents = read_ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ(s_irCache, IRSIZE);
	s_dw_ਸੂਚਕਾਂਕ = 0;
	s_dwMax = dwEvents;
    }

    ਜੇ (n_ਲੰਮਾਈ == -2)
	ਵਾਪਸ (s_dwMax > 0) ? TRUE : FALSE;

    ਜੇ (win_ਸੰਸਕਰਣ < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	ਜੇ (n_ਲੰਮਾਈ == -1)
	    ਵਾਪਸ Peek_Console_ਨਿਵੇਸ਼_W(h_ਨਿਵੇਸ਼, lp_ਬਫਰ, 1, lpEvents);
	ਵਾਪਸ Read_Console_ਨਿਵੇਸ਼_W(h_ਨਿਵੇਸ਼, lp_ਬਫਰ, 1, &dwEvents);
    }

    ਜੇ (s_dwMax == 0)
    {
	ਜੇ (!vtp_working && n_ਲੰਮਾਈ == -1)
	    ਵਾਪਸ Peek_Console_ਨਿਵੇਸ਼_W(h_ਨਿਵੇਸ਼, lp_ਬਫਰ, 1, lpEvents);
	Get_ਸੰਖਿਆ_ਦਾ_Console_ਨਿਵੇਸ਼_Events(h_ਨਿਵੇਸ਼, &dwEvents);
	ਜੇ (dwEvents == 0 && n_ਲੰਮਾਈ == -1)
	    ਵਾਪਸ Peek_Console_ਨਿਵੇਸ਼_W(h_ਨਿਵੇਸ਼, lp_ਬਫਰ, 1, lpEvents);
	Read_Console_ਨਿਵੇਸ਼_W(h_ਨਿਵੇਸ਼, s_irCache, IRSIZE, &dwEvents);
	s_dw_ਸੂਚਕਾਂਕ = 0;
	s_dwMax = dwEvents;
	ਜੇ (dwEvents == 0)
	{
	    *lpEvents = 0;
	    ਵਾਪਸ TRUE;
	}

	ਲਈ (i = s_dw_ਸੂਚਕਾਂਕ; i < (ਪੂਰਨ)s_dwMax - 1; ++i)
	    ਜੇ (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	ਜੇ (s_dwMax > 1)
	{
	    ਸਿਰ = 0;
	    ਪੂਛ = s_dwMax - 1;
	    ਜਦੋਂ (ਸਿਰ != ਪੂਛ)
	    {
		ਜੇ (s_irCache[ਸਿਰ].EventType == ਖਿੜਕੀ_ਬਫਰ_ਮਾਪ_EVENT
			&& s_irCache[ਸਿਰ + 1].EventType
						  == ਖਿੜਕੀ_ਬਫਰ_ਮਾਪ_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    ਲਈ (i = ਸਿਰ; i < ਪੂਛ; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --ਪੂਛ;
		    ਜਾਰੀ;
		}
		ਸਿਰ++;
	    }
	    s_dwMax = ਪੂਛ + 1;
	}
    }

    ਜੇ (s_irCache[s_dw_ਸੂਚਕਾਂਕ].EventType == ਕੁੰਜੀ_EVENT)
    {
	ਜੇ (s_irCache[s_dw_ਸੂਚਕਾਂਕ].Event.ਕੁੰਜੀ_Event_2.w_Repeat_ਗਿਣਤੀ > 1)
	{
	    s_irPseudo = s_irCache[s_dw_ਸੂਚਕਾਂਕ];
	    s_irPseudo.Event.ਕੁੰਜੀ_Event_2.w_Repeat_ਗਿਣਤੀ = 1;
	    s_irCache[s_dw_ਸੂਚਕਾਂਕ].Event.ਕੁੰਜੀ_Event_2.w_Repeat_ਗਿਣਤੀ--;
	    *lp_ਬਫਰ = s_irPseudo;
	    *lpEvents = 1;
	    ਵਾਪਸ TRUE;
	}
    }

    *lp_ਬਫਰ = s_irCache[s_dw_ਸੂਚਕਾਂਕ];
    ਜੇ (!(n_ਲੰਮਾਈ == -1 || n_ਲੰਮਾਈ == -2) && ++s_dw_ਸੂਚਕਾਂਕ >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    ਵਾਪਸ TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    ਸਥਿਰ BOOL
peek_console_ਨਿਵੇਸ਼(
    HANDLE	    h_ਨਿਵੇਸ਼,
    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ    *lp_ਬਫਰ,
    DWORD	    n_ਲੰਮਾਈ ਅਣਵਰਤਿਆ,
    LPDWORD	    lpEvents)
{
    ਵਾਪਸ read_console_ਨਿਵੇਸ਼(h_ਨਿਵੇਸ਼, lp_ਬਫਰ, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    ਸਥਿਰ DWORD
ਸੁਨੇਹਾ_wait_for_ਗੁਣਜ_objects(
    DWORD    n_ਗਿਣਤੀ,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dw_Wake_ਨਕਾਬ)
{
    ਜੇ (read_console_ਨਿਵੇਸ਼(ਨਿਰਾ, ਨਿਰਾ, -2, ਨਿਰਾ))
	ਵਾਪਸ WAIT_OBJECT_0;
    ਵਾਪਸ ਸੁਨੇਹਾ_Wait_For_ਗੁਣਜ_Objects(n_ਗਿਣਤੀ, pHandles, fWaitAll,
				     dwMilliseconds, dw_Wake_ਨਕਾਬ);
}
# endif

# ifndef FEAT_CLIENTSERVER
    ਸਥਿਰ DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    ਜੇ (read_console_ਨਿਵੇਸ਼(ਨਿਰਾ, ਨਿਰਾ, -2, ਨਿਰਾ))
	ਵਾਪਸ WAIT_OBJECT_0;
    ਵਾਪਸ WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#ਜੇ_ਅੰਤ   // !FEAT_GUI_MSWIN || VIMDLL

    ਖਾਲੀ
mch_get_exe_name(ਖਾਲੀ)
{
    ਪੂਰਨ		updated = FALSE;
    ਸਥਿਰ ਪੂਰਨ	enc_prev = -1;
    WCHAR	*ਰਸਤਾ;
    size_t	pathlen;

    ਜੇ (exe_name == ਨਿਰਾ || exe_pathw == ਨਿਰਾ || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_ਰਸਤਾ];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(ਨਿਰਾ, buf, MAX_ਰਸਤਾ);
	ਜੇ (p > buf)
	{
	    ਜੇ (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_ਮੰਜ਼ਿਲ_enc(buf, ਨਿਰਾ);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    ਕਰੋ
	    {
		--p;
		ਜੇ (*p == L'\\')
		{
		    *p = L'\0';
		    ਰੋਕੋ;
		}
	    } ਜਦੋਂ (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    ਜੇ (exe_pathw == ਨਿਰਾ || !updated)
	ਵਾਪਸ;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    ਰਸਤਾ = _wgetenv(L"PATH");
    pathlen = (ਰਸਤਾ == ਨਿਰਾ) ? 0 : wcslen(ਰਸਤਾ);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXTRA_LEN 7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ENV_STRING_LEN 8191

    ਜੇ (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   ਆਰਜ਼ੀ[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	ਜੇ (pathlen == 0)
	    wcscpy(ਆਰਜ਼ੀ + templen, exe_pathw);
	ਨਹੀਂਤਾਂ
	{
	    wcscpy(ਆਰਜ਼ੀ + templen, ਰਸਤਾ);

	    // Check if exe_path is already included in $PATH.
	    ਜੇ (wcsstr(ਆਰਜ਼ੀ, exe_pathw) == ਨਿਰਾ)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		ਜੇ (ਆਰਜ਼ੀ[templen - 1] != L';')
		{
		    wcscpy(ਆਰਜ਼ੀ + templen, L";");
		    ++templen;
		}

		wcscpy(ਆਰਜ਼ੀ + templen, exe_pathw);
	    }
	}

	_wputenv(ਆਰਜ਼ੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ libintl_wputenv
	libintl_wputenv(ਆਰਜ਼ੀ);
#ਜੇ_ਅੰਤ
    }
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ EXTRA_LEN
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ MAX_ENV_ਰਸਤਾ_LEN
}

/*
 * Unescape characters in "p" that appear in "escaped".
 */
    ਸਥਿਰ ਖਾਲੀ
unescape_shellxquote(char_u *p, char_u *escaped)
{
    ਪੂਰਨ	    l = (ਪੂਰਨ)STRLEN(p);
    ਪੂਰਨ	    n;

    ਜਦੋਂ (*p != NUL)
    {
	ਜੇ (*p == '^' && vim_strchr(escaped, p[1]) != ਨਿਰਾ)
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
vim_ਲੱਦੋ_Lib(ਅਚਲ ਅੱਖਰ *name)
{
    HINSTANCE	dll = ਨਿਰਾ;

    // No need to load any library when registering OLE.
    ਜੇ (ਮਿਲਿਆ_ਰਜਿਸਟਰ_arg)
	ਵਾਪਸ ਨਿਰਾ;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    ਜੇ (exe_pathw == ਨਿਰਾ)
    {
	mch_get_exe_name();
	ਜੇ (exe_pathw == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
    }

    WCHAR old_dirw[MAXPATHL];

    ਜੇ (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	ਵਾਪਸ ਨਿਰਾ;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    ਸੈੱਟ_Current_Directory_W(exe_pathw);
    dll = ਲੱਦੋ_Library(name);
    ਸੈੱਟ_Current_Directory_W(old_dirw);
    ਵਾਪਸ dll;
}

#ਜੇ defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    ਪੂਰਨ
mch_is_gui_executable(ਖਾਲੀ)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(ਨਿਰਾ);
    PIMAGE_DOS_ਸਿਰਲੇਖ	pDOS = (PIMAGE_DOS_ਸਿਰਲੇਖ)pImage;
    PIMAGE_NT_ਸਿਰਲੇਖਾਂ	pPE;

    ਜੇ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ਵਾਪਸ FALSE;
    pPE = (PIMAGE_NT_ਸਿਰਲੇਖਾਂ)(pImage + pDOS->e_lfanew);
    ਜੇ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ਵਾਪਸ FALSE;
    ਜੇ (pPE->Optional_ਸਿਰਲੇਖ.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    ਸਥਿਰ ਖਾਲੀ *
get_imported_func_info(HINSTANCE hInst, ਅਚਲ ਅੱਖਰ *funcname, ਪੂਰਨ info,
	ਅਚਲ ਖਾਲੀ *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_ਸਿਰਲੇਖ		pDOS = (PIMAGE_DOS_ਸਿਰਲੇਖ)hInst;
    PIMAGE_NT_ਸਿਰਲੇਖਾਂ		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_ਅੰਕੜੇ		pIAT;	    // Import Address Table
    PIMAGE_THUNK_ਅੰਕੜੇ		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    ਜੇ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ਵਾਪਸ ਨਿਰਾ;
    pPE = (PIMAGE_NT_ਸਿਰਲੇਖਾਂ)(pImage + pDOS->e_lfanew);
    ਜੇ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ਵਾਪਸ ਨਿਰਾ;

    ImpVA = pPE->Optional_ਸਿਰਲੇਖ
		.ਅੰਕੜੇ_Directory[IMAGE_DIRECTORY_ਇੰਦਰਾਜ_IMPORT].Virtual_ਪਤਾ;
    ਜੇ (ImpVA == 0)
	ਵਾਪਸ ਨਿਰਾ;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    ਲਈ (; pImpDesc->ਪਹਿਲਾ_Thunk; ++pImpDesc)
    {
	ਜੇ (!pImpDesc->Original_ਪਹਿਲਾ_Thunk)
	    ਜਾਰੀ;
	pIAT = (PIMAGE_THUNK_ਅੰਕੜੇ)(pImage + pImpDesc->ਪਹਿਲਾ_Thunk);
	pINT = (PIMAGE_THUNK_ਅੰਕੜੇ)(pImage + pImpDesc->Original_ਪਹਿਲਾ_Thunk);
	ਲਈ (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    ਜੇ (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		ਜਾਰੀ;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.ਪਤਾ_ਦਾ_ਅੰਕੜੇ));
	    ਜੇ (strcmp((ਅੱਖਰ *)pImpName->Name, funcname) == 0)
	    {
		ਖਾਲੀ *original;
		DWORD old, new = ਪੰਨਾ_READWRITE;

		ਚੋਣ (info)
		{
		    ਹਾਲਤ 0:
			ਵਾਪਸ (ਖਾਲੀ *)pIAT->u1.Function;
		    ਹਾਲਤ 1:
			ਵਾਪਸ (ਖਾਲੀ *)(pImage + pImpDesc->Name);
		    ਹਾਲਤ 2:
			original = (ਖਾਲੀ *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, ਆਕਾਰ(ਖਾਲੀ *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, ਆਕਾਰ(ਖਾਲੀ *),
				old, &new);
			ਵਾਪਸ original;
		    ਮੂਲ:
			ਵਾਪਸ ਨਿਰਾ;
		}
	    }
	}
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
ਲੱਭੋ_imported_module_by_funcname(HINSTANCE hInst, ਅਚਲ ਅੱਖਰ *funcname)
{
    ਅੱਖਰ    *modulename;

    modulename = (ਅੱਖਰ *)get_imported_func_info(hInst, funcname, 1, ਨਿਰਾ);
    ਜੇ (modulename != ਨਿਰਾ)
	ਵਾਪਸ GetModuleHandleA(modulename);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    ਖਾਲੀ *
get_dll_import_func(HINSTANCE hInst, ਅਚਲ ਅੱਖਰ *funcname)
{
    ਵਾਪਸ get_imported_func_info(hInst, funcname, 0, ਨਿਰਾ);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    ਖਾਲੀ *
hook_dll_import_func(HINSTANCE hInst, ਅਚਲ ਅੱਖਰ *funcname, ਅਚਲ ਖਾਲੀ *hook)
{
    ਵਾਪਸ get_imported_func_info(hInst, funcname, 2, hook);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PYTHON3)
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
    PIMAGE_DOS_ਸਿਰਲੇਖ		pDOS = (PIMAGE_DOS_ਸਿਰਲੇਖ)hInst;
    PIMAGE_NT_ਸਿਰਲੇਖਾਂ		pPE;
    PIMAGE_EXPORT_DIRECTORY	pExpDir;
    DWORD			ExpVA;
    DWORD			Exp_ਮਾਪ;
    LPDWORD			p_Function_ਸਾਰਣੀ;

    ਜੇ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ਵਾਪਸ ਨਿਰਾ;
    pPE = (PIMAGE_NT_ਸਿਰਲੇਖਾਂ)(pImage + pDOS->e_lfanew);
    ਜੇ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ਵਾਪਸ ਨਿਰਾ;

    ExpVA = pPE->Optional_ਸਿਰਲੇਖ
		.ਅੰਕੜੇ_Directory[IMAGE_DIRECTORY_ਇੰਦਰਾਜ_EXPORT].Virtual_ਪਤਾ;
    Exp_ਮਾਪ = pPE->Optional_ਸਿਰਲੇਖ
		.ਅੰਕੜੇ_Directory[IMAGE_DIRECTORY_ਇੰਦਰਾਜ_EXPORT].ਮਾਪ_2;
    ਜੇ (ExpVA == 0)
	ਵਾਪਸ hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    p_Function_ਸਾਰਣੀ = (LPDWORD)(pImage + pExpDir->ਪਤਾ_ਦਾ_Functions);

    ਜੇ (pExpDir->ਸੰਖਿਆ_ਦਾ_ਨਾਂ == 0)
	ਵਾਪਸ hInst;	// No export names.

    // Check only the first entry.
    ਜੇ ((p_Function_ਸਾਰਣੀ[0] < ExpVA) || (p_Function_ਸਾਰਣੀ[0] >= ExpVA + Exp_ਮਾਪ))
	// The first entry is not a function forwarder.
	ਵਾਪਸ hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    ਅਚਲ ਅੱਖਰ *name = (ਅਚਲ ਅੱਖਰ *)(pImage + p_Function_ਸਾਰਣੀ[0]);
    ਅਚਲ ਅੱਖਰ *p = strchr(name, '.');
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ hInst;

    // Extract DllName.
    ਅੱਖਰ buf[MAX_ਰਸਤਾ];
    ਜੇ (p - name + 1 > ਆਕਾਰ(buf))
	ਵਾਪਸ ਨਿਰਾ;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    ਵਾਪਸ GetModuleHandleA(buf);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
ਸਥਿਰ ਅੱਖਰ *null_libintl_gettext(ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਅੱਖਰ *null_libintl_ngettext(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ n);
ਸਥਿਰ ਅੱਖਰ *null_libintl_textdomain(ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਅੱਖਰ *null_libintl_bindtextdomain(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਅੱਖਰ *null_libintl_bind_textdomain_codeset(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ null_libintl_wputenv(ਅਚਲ wchar_t *);

ਸਥਿਰ HINSTANCE hLibintlDLL = ਨਿਰਾ;
ਅੱਖਰ *(*dyn_libintl_gettext)(ਅਚਲ ਅੱਖਰ *) = null_libintl_gettext;
ਅੱਖਰ *(*dyn_libintl_ngettext)(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ n)
						= null_libintl_ngettext;
ਅੱਖਰ *(*dyn_libintl_textdomain)(ਅਚਲ ਅੱਖਰ *) = null_libintl_textdomain;
ਅੱਖਰ *(*dyn_libintl_bindtextdomain)(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *)
						= null_libintl_bindtextdomain;
ਅੱਖਰ *(*dyn_libintl_bind_textdomain_codeset)(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *)
				       = null_libintl_bind_textdomain_codeset;
ਪੂਰਨ (*dyn_libintl_wputenv)(ਅਚਲ wchar_t *) = null_libintl_wputenv;
ਪੂਰਨ *dyn_libintl_nl_ਸੁਨੇਹਾ_cat_cntr = ਨਿਰਾ;

    ਪੂਰਨ
dyn_libintl_init(ਖਾਲੀ)
{
    ਪੂਰਨ i;
    ਸਥਿਰ ਬਣਤਰ
    {
	ਅੱਖਰ	    *name;
	FARPROC	    *ptr;
    } libintl_ਇੰਦਰਾਜ[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{ਨਿਰਾ, ਨਿਰਾ}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    ਜੇ (hLibintlDLL != ਨਿਰਾ)
	ਵਾਪਸ 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vim_ਲੱਦੋ_Lib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    ਜੇ (!hLibintlDLL)
	hLibintlDLL = vim_ਲੱਦੋ_Lib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    ਜੇ (!hLibintlDLL)
	hLibintlDLL = vim_ਲੱਦੋ_Lib(GETTEXT_DLL_ALT2);
# endif
    ਜੇ (!hLibintlDLL)
    {
	ਜੇ (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_ਲੱਦੋ_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	ਵਾਪਸ 0;
    }
    ਲਈ (i = 0; libintl_ਇੰਦਰਾਜ[i].name != ਨਿਰਾ
					 && libintl_ਇੰਦਰਾਜ[i].ptr != ਨਿਰਾ; ++i)
    {
	ਜੇ ((*libintl_ਇੰਦਰਾਜ[i].ptr = Get_Proc_ਪਤਾ(hLibintlDLL,
					      libintl_ਇੰਦਰਾਜ[i].name)) == ਨਿਰਾ)
	{
	    dyn_libintl_ਸਿਰਾ();
	    ਜੇ (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_ਲੱਦੋ_library_function_str), libintl_ਇੰਦਰਾਜ[i].name);
		verbose_leave();
	    }
	    ਵਾਪਸ 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (ਅੱਖਰ *(*)(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *))
			Get_Proc_ਪਤਾ(hLibintlDLL, "bind_textdomain_codeset");
    ਜੇ (dyn_libintl_bind_textdomain_codeset == ਨਿਰਾ)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = ਲੱਭੋ_imported_module_by_funcname(hLibintlDLL, "getenv");
    ਜੇ (hmsvcrt != ਨਿਰਾ)
	dyn_libintl_wputenv = (ਪੂਰਨ (*)(ਅਚਲ wchar_t *))
					Get_Proc_ਪਤਾ(hmsvcrt, "_wputenv");
    ਜੇ (dyn_libintl_wputenv == ਨਿਰਾ || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_ਸੁਨੇਹਾ_cat_cntr = (ਪੂਰਨ *)Get_Proc_ਪਤਾ(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    ਵਾਪਸ 1;
}

    ਖਾਲੀ
dyn_libintl_ਸਿਰਾ(ਖਾਲੀ)
{
    ਜੇ (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ਨਿਰਾ;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_ਸੁਨੇਹਾ_cat_cntr	= ਨਿਰਾ;
}

    ਸਥਿਰ ਅੱਖਰ *
null_libintl_gettext(ਅਚਲ ਅੱਖਰ *msgid)
{
    ਵਾਪਸ (ਅੱਖਰ*)msgid;
}

    ਸਥਿਰ ਅੱਖਰ *
null_libintl_ngettext(
	ਅਚਲ ਅੱਖਰ *msgid,
	ਅਚਲ ਅੱਖਰ *msgid_plural,
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ n)
{
    ਵਾਪਸ (ਅੱਖਰ *)(n == 1 ? msgid : msgid_plural);
}

    ਸਥਿਰ ਅੱਖਰ *
null_libintl_bindtextdomain(
	ਅਚਲ ਅੱਖਰ *domainname ਅਣਵਰਤਿਆ,
	ਅਚਲ ਅੱਖਰ *dirname ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਅੱਖਰ *
null_libintl_bind_textdomain_codeset(
	ਅਚਲ ਅੱਖਰ *domainname ਅਣਵਰਤਿਆ,
	ਅਚਲ ਅੱਖਰ *codeset ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਅੱਖਰ *
null_libintl_textdomain(ਅਚਲ ਅੱਖਰ *domainname ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਪੂਰਨ
null_libintl_wputenv(ਅਚਲ wchar_t *envstring ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ 0;
}

#ਜੇ_ਅੰਤ // DYNAMIC_GETTEXT

// This symbol is not defined in older versions of the SDK or Visual C++

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
# include <aclapi.h>
# ifndef PROTECTED_DACL_SECURITY_INFORMATION
#  define PROTECTED_DACL_SECURITY_INFORMATION	0x80000000L
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
/*
 * Enables or disables the specified privilege.
 */
    ਸਥਿਰ BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    ਜੇ (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	ਵਾਪਸ FALSE;

    ਜੇ (!Lookup_Privilege_ਮੁੱਲ(ਨਿਰਾ, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	ਵਾਪਸ FALSE;
    }

    tokenPrivileges.Privilege_ਗਿਣਤੀ	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].ਗੁਣ_2 = SE_PRIVILEGE_ENABLED;

    ਜੇ (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, ਨਿਰਾ, ਨਿਰਾ))
    {
	CloseHandle(hToken);
	ਵਾਪਸ FALSE;
    }

    ਜੇ (Get_ਆਖਰੀ_Error() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	ਵਾਪਸ FALSE;
    }

    CloseHandle(hToken);
    ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    ਖਾਲੀ
PlatformId(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    DWORD major = (win_ਸੰਸਕਰਣ >> 24) & 0xFF;
    DWORD minor = (win_ਸੰਸਕਰਣ >> 16) & 0xFF;
    vim_snprintf(windows_ਸੰਸਕਰਣ, ਆਕਾਰ(windows_ਸੰਸਕਰਣ), "%d.%d", major, minor);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    ਜੇ (!win32_enable_privilege(SE_SECURITY_NAME))
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
}

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define ਖਿਸਕਾਓ_2  (ਖਿਸਕਾਓ_PRESSED)
# define CTRL   (ਸੱਜਾ_CTRL_PRESSED | ਖੱਬਾ_CTRL_PRESSED)
# define ALT    (ਸੱਜਾ_ALT_PRESSED  | ਖੱਬਾ_ALT_PRESSED)
# define ALT_GR (ਸੱਜਾ_ALT_PRESSED  | ਖੱਬਾ_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

ਸਥਿਰ ਅਚਲ ਬਣਤਰ
{
    ਸ਼ਬਦ_2    w_Virt_ਕੁੰਜੀ;
    BOOL    f_Ansi_ਕੁੰਜੀ;
    ਪੂਰਨ	    chAlone;
    ਪੂਰਨ	    ch_ਖਿਸਕਾਓ;
    ਪੂਰਨ	    chCtrl;
    ਪੂਰਨ	    chAlt;
} Virt_ਕੁੰਜੀ_Map[] =
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
    { VK_ਉੱਪਰ,	TRUE,	'H',	'\304',	'\305',	    '\306', },
    { VK_ਪੂਰਵ,	TRUE,	'I',	'\307',	'\204',	    '\310', }, // PgUp
    { VK_ਖੱਬਾ,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_ਸੱਜਾ,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_ਸਿਰਾ,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_ਥੱਲੇ,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_ਅਗਲਾ,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_ਪਾਓ,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_ਪਿੱਠ,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# ਜੇ 0
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
    { VK_ਜੋੜੋ,	TRUE,   'N',    'N',    'N',	'N',	}, // keyp '+'
    { VK_ਘਟਾਓ, TRUE,'J',	'J',    'J',	'J',	}, // keyp '-'
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
    ਸਥਿਰ ਪੂਰਨ
win32_kbd_patch_ਕੁੰਜੀ(
    ਕੁੰਜੀ_EVENT_ਰਿਕਾਰਡ *pker)
{
    UINT uMods = pker->dw_Control_ਕੁੰਜੀ_State;
    ਸਥਿਰ ਪੂਰਨ s_iIsDead = 0;
    ਸਥਿਰ ਸ਼ਬਦ_2 aw_Ansi_ਕੋਡ[2];
    ਸਥਿਰ BYTE abKeystate[256];

    ਜੇ (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_ਕੋਡ[1];
	s_iIsDead = 0;
	ਵਾਪਸ 1;
    }

    // check if it already has a valid unicode character.
    ਜੇ (pker->uChar.UnicodeChar != 0)
	ਵਾਪਸ 1;

    CLEAR_ਖੇਤਰ(abKeystate);

    // Clear any pending dead keys
    ਮੰਜ਼ਿਲ_Unicode(VK_SPACE, Map_Virtual_ਕੁੰਜੀ(VK_SPACE, 0), abKeystate, aw_Ansi_ਕੋਡ, 2, 0);

    ਜੇ (uMods & ਖਿਸਕਾਓ_PRESSED)
	abKeystate[VK_ਖਿਸਕਾਓ] = 0x80;
    ਜੇ (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    ਜੇ ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ਮੰਜ਼ਿਲ_Unicode(pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ, pker->w_Virtual_ਜਾਂਚ_ਕੋਡ,
			abKeystate, aw_Ansi_ਕੋਡ, 2, 0);

    ਜੇ (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_ਕੋਡ[0];

    ਵਾਪਸ s_iIsDead;
}

ਸਥਿਰ BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    ਸਥਿਰ BOOL
decode_ਕੁੰਜੀ_event(
    ਕੁੰਜੀ_EVENT_ਰਿਕਾਰਡ	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    ਪੂਰਨ			*pmodifiers,
    BOOL		fDoPost ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ i;
    ਅਚਲ ਪੂਰਨ nModifs = pker->dw_Control_ਕੁੰਜੀ_State & (ਖਿਸਕਾਓ_2 | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    ਜੇ (!pker->b_ਕੁੰਜੀ_ਥੱਲੇ)
	ਵਾਪਸ FALSE;

    // ignore some keystrokes
    ਚੋਣ (pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ)
    {
    // modifiers
    ਹਾਲਤ VK_ਖਿਸਕਾਓ:
    ਹਾਲਤ VK_CONTROL:
    ਹਾਲਤ VK_MENU:   // Alt key
	ਵਾਪਸ FALSE;

    ਮੂਲ:
	ਰੋਕੋ;
    }

    // Shift-TAB
    ਜੇ (pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ == VK_TAB && (nModifs & ਖਿਸਕਾਓ_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	ਵਾਪਸ TRUE;
    }

    ਲਈ (i = ARRAY_ਲੰਮਾਈ(Virt_ਕੁੰਜੀ_Map); --i >= 0; )
    {
	ਜੇ (Virt_ਕੁੰਜੀ_Map[i].w_Virt_ਕੁੰਜੀ == pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ)
	{
	    *pch = Virt_ਕੁੰਜੀ_Map[i].chAlone;
	    ਜੇ ((nModifs & ਖਿਸਕਾਓ_2) != 0)
		*pch = Virt_ਕੁੰਜੀ_Map[i].ch_ਖਿਸਕਾਓ;
	    ਨਹੀਂਤਾਂ ਜੇ ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_ਕੁੰਜੀ_Map[i].chCtrl;
	    ਨਹੀਂਤਾਂ ਜੇ ((nModifs & ALT) != 0)
		*pch = Virt_ਕੁੰਜੀ_Map[i].chAlt;

	    ਜੇ (*pch != 0)
	    {
		ਜੇ (Virt_ਕੁੰਜੀ_Map[i].f_Ansi_ਕੁੰਜੀ)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    ਜੇ (pmodifiers)
		    {
			ਜੇ (pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ >= VK_F1
			    && pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ <= VK_F12)
			{
			    ਜੇ ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_ਨਕਾਬ_ALT;
				ਜੇ ((nModifs & ਖਿਸਕਾਓ_2) == 0)
				    *pch2 = Virt_ਕੁੰਜੀ_Map[i].chAlone;
			    }
			    ਜੇ ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_ਨਕਾਬ_CTRL;
				ਜੇ ((nModifs & ਖਿਸਕਾਓ_2) == 0)
				    *pch2 = Virt_ਕੁੰਜੀ_Map[i].chAlone;
			    }
			}
			ਨਹੀਂਤਾਂ ਜੇ (pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ >= VK_ਸਿਰਾ
				&& pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ <= VK_ਥੱਲੇ)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_ਕੁੰਜੀ_Map[i].chAlone;
			    ਜੇ ((nModifs & ਖਿਸਕਾਓ_2) != 0
						    && (nModifs & ~ਖਿਸਕਾਓ_2) == 0)
			    {
				*pch2 = Virt_ਕੁੰਜੀ_Map[i].ch_ਖਿਸਕਾਓ;
			    }
			    ਜੇ ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_ਕੁੰਜੀ_Map[i].chCtrl;
				ਜੇ (pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ == VK_ਉੱਪਰ
				    || pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ == VK_ਥੱਲੇ)
				{
				    *pmodifiers |= MOD_ਨਕਾਬ_CTRL;
				    *pch2 = Virt_ਕੁੰਜੀ_Map[i].chAlone;
				}
			    }
			    ਜੇ ((nModifs & ਖਿਸਕਾਓ_2) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_ਨਕਾਬ_CTRL;
				*pch2 = Virt_ਕੁੰਜੀ_Map[i].ch_ਖਿਸਕਾਓ;
			    }
			    ਜੇ ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_ਕੁੰਜੀ_Map[i].chAlt;
				*pmodifiers |= MOD_ਨਕਾਬ_ALT;
				ਜੇ ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_ਕੁੰਜੀ_Map[i].chAlone;
				}
				ਨਹੀਂਤਾਂ ਜੇ ((nModifs & ਖਿਸਕਾਓ_2) != 0)
				{
				    *pch2 = Virt_ਕੁੰਜੀ_Map[i].ch_ਖਿਸਕਾਓ;
				}
				ਨਹੀਂਤਾਂ ਜੇ ((nModifs & CTRL) != 0)
				{
				    ਜੇ (pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ == VK_ਉੱਪਰ
					|| pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ == VK_ਥੱਲੇ)
				    {
					*pmodifiers |= MOD_ਨਕਾਬ_CTRL;
					*pch2 = Virt_ਕੁੰਜੀ_Map[i].chAlone;
				    }
				    ਨਹੀਂਤਾਂ
				    {
					*pch2 = Virt_ਕੁੰਜੀ_Map[i].chCtrl;
				    }
				}
			    }
			}
			ਨਹੀਂਤਾਂ ਜੇ (pker->w_Virtual_ਕੁੰਜੀ_ਕੋਡ == VK_ਪਾਓ
					&& (nModifs & ਖਿਸਕਾਓ_2) != 0
					&& (nModifs & ~ਖਿਸਕਾਓ_2) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_ਕੁੰਜੀ_Map[i].ch_ਖਿਸਕਾਓ;
			}
			ਨਹੀਂਤਾਂ
			{
			    *pch2 = Virt_ਕੁੰਜੀ_Map[i].chAlone;
			    ਜੇ ((nModifs & ਖਿਸਕਾਓ_2) != 0)
				*pmodifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
			    ਜੇ ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_ਨਕਾਬ_CTRL;
			    ਜੇ ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_ਨਕਾਬ_ALT;
			}
		    }
		}

		ਵਾਪਸ TRUE;
	    }
	}
    }

    i = win32_kbd_patch_ਕੁੰਜੀ(pker);

    ਜੇ (i < 0)
	*pch = NUL;
    ਨਹੀਂਤਾਂ
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	ਜੇ (pmodifiers != ਨਿਰਾ)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    ਜੇ ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_ਨਕਾਬ_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    ਜੇ ((nModifs & ਖਿਸਕਾਓ_2) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    ਜੇ ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_ਨਕਾਬ_CTRL;
	}
    }

    ਵਾਪਸ (*pch != NUL);
}

# ਜੇ defined(FEAT_EVAL)
    ਸਥਿਰ ਪੂਰਨ
encode_ਕੁੰਜੀ_event(dict_T *ਦਲੀਲਾਂ, ਨਿਵੇਸ਼_ਰਿਕਾਰਡ *ir)
{
    ਸਥਿਰ ਪੂਰਨ s_dwMods = 0;

    char_u *action = dict_get_string(ਦਲੀਲਾਂ, "event", TRUE);
    ਜੇ (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_ਕੁੰਜੀ_ਥੱਲੇ = STRICMP(action, "keydown") == 0;
	ਸ਼ਬਦ_2 vk_ਕੋਡ = dict_get_ਸੰਖਿਆ_def(ਦਲੀਲਾਂ, "keycode", 0);
	ਜੇ (vk_ਕੋਡ <= 0 || vk_ਕੋਡ >= 0xFF)
	{
	    semsg(_(e_invalid_ਦਲੀਲ_nr), (ਲੰਮਾ)vk_ਕੋਡ);
	    ਵਾਪਸ FALSE;
	}

	ir->EventType = ਕੁੰਜੀ_EVENT;
	ਕੁੰਜੀ_EVENT_ਰਿਕਾਰਡ ker;
	Zero_ਸਮ੍ਰਿਤੀ(&ker, ਆਕਾਰ(ker));
	ker.b_ਕੁੰਜੀ_ਥੱਲੇ = is_ਕੁੰਜੀ_ਥੱਲੇ;
	ker.w_Repeat_ਗਿਣਤੀ = 1;
	ker.w_Virtual_ਜਾਂਚ_ਕੋਡ = 0;
	ker.dw_Control_ਕੁੰਜੀ_State = 0;
	ਪੂਰਨ mods = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	ਜੇ (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    ਜੇ (mods & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)
		ker.dw_Control_ਕੁੰਜੀ_State |= ਖਿਸਕਾਓ_PRESSED;
	    ਜੇ (mods & MOD_ਨਕਾਬ_CTRL)
		ker.dw_Control_ਕੁੰਜੀ_State |= ਖੱਬਾ_CTRL_PRESSED;
	    ਜੇ (mods & MOD_ਨਕਾਬ_ALT)
		ker.dw_Control_ਕੁੰਜੀ_State |= ਖੱਬਾ_ALT_PRESSED;
	}

	ਜੇ (vk_ਕੋਡ == VK_LSHIFT || vk_ਕੋਡ == VK_RSHIFT || vk_ਕੋਡ == VK_ਖਿਸਕਾਓ)
	{
	    ਜੇ (is_ਕੁੰਜੀ_ਥੱਲੇ)
		s_dwMods |= ਖਿਸਕਾਓ_PRESSED;
	    ਨਹੀਂਤਾਂ
		s_dwMods &= ~ਖਿਸਕਾਓ_PRESSED;
	}
	ਨਹੀਂਤਾਂ ਜੇ (vk_ਕੋਡ == VK_LCONTROL || vk_ਕੋਡ == VK_CONTROL)
	{
	    ਜੇ (is_ਕੁੰਜੀ_ਥੱਲੇ)
		s_dwMods |= ਖੱਬਾ_CTRL_PRESSED;
	    ਨਹੀਂਤਾਂ
		s_dwMods &= ~ਖੱਬਾ_CTRL_PRESSED;
	}
	ਨਹੀਂਤਾਂ ਜੇ (vk_ਕੋਡ == VK_RCONTROL)
	{
	    ਜੇ (is_ਕੁੰਜੀ_ਥੱਲੇ)
		s_dwMods |= ਸੱਜਾ_CTRL_PRESSED;
	    ਨਹੀਂਤਾਂ
		s_dwMods &= ~ਸੱਜਾ_CTRL_PRESSED;
	}
	ਨਹੀਂਤਾਂ ਜੇ (vk_ਕੋਡ == VK_LMENU || vk_ਕੋਡ == VK_MENU)
	{
	    ਜੇ (is_ਕੁੰਜੀ_ਥੱਲੇ)
		s_dwMods |= ਖੱਬਾ_ALT_PRESSED;
	    ਨਹੀਂਤਾਂ
		s_dwMods &= ~ਖੱਬਾ_ALT_PRESSED;
	}
	ਨਹੀਂਤਾਂ ਜੇ (vk_ਕੋਡ == VK_RMENU)
	{
	    ਜੇ (is_ਕੁੰਜੀ_ਥੱਲੇ)
		s_dwMods |= ਸੱਜਾ_ALT_PRESSED;
	    ਨਹੀਂਤਾਂ
		s_dwMods &= ~ਸੱਜਾ_ALT_PRESSED;
	}
	ker.dw_Control_ਕੁੰਜੀ_State |= s_dwMods;
	ker.w_Virtual_ਕੁੰਜੀ_ਕੋਡ = vk_ਕੋਡ;
	ker.uChar.UnicodeChar = 0;
	ir->Event.ਕੁੰਜੀ_Event_2 = ker;
	vim_free(action);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (action == ਨਿਰਾ)
	{
	    semsg(_(e_missing_ਦਲੀਲ_str), "event");
	}
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "event", action);
	    vim_free(action);
	}
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}
# endif  // FEAT_EVAL
#ਜੇ_ਅੰਤ // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ਸਥਿਰ ਪੂਰਨ g_fMouseAvail = FALSE;   // mouse present
ਸਥਿਰ ਪੂਰਨ g_f_Mouse_ਸਰਗਰਮ = FALSE;  // mouse enabled
ਸਥਿਰ ਪੂਰਨ g_nMouseClick = -1;	    // mouse status
ਸਥਿਰ ਪੂਰਨ g_xMouse;		    // mouse x coordinate
ਸਥਿਰ ਪੂਰਨ g_yMouse;		    // mouse y coordinate
ਸਥਿਰ DWORD g_cmodein = 0;	    // Original console input mode
ਸਥਿਰ DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    ਖਾਲੀ
mch_setmouse(ਪੂਰਨ on)
{
    DWORD cmodein;

# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ;
# endif
    ਜੇ (!g_fMouseAvail)
	ਵਾਪਸ;

    g_f_Mouse_ਸਰਗਰਮ = on;
    Get_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ, &cmodein);

    ਜੇ (g_f_Mouse_ਸਰਗਰਮ)
    {
	cmodein |= ENABLE_MOUSE_ਨਿਵੇਸ਼;
	cmodein &= ~ENABLE_ਤੇਜ਼_EDIT_ਬਹੁਲਕ;
    }
    ਨਹੀਂਤਾਂ
    {
	cmodein &= ~ENABLE_MOUSE_ਨਿਵੇਸ਼;
	cmodein |= g_cmodein & ENABLE_ਤੇਜ਼_EDIT_ਬਹੁਲਕ;
    }

    ਸੈੱਟ_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ, cmodein | ENABLE_EXTENDED_FLAGS);
}


# ਜੇ defined(FEAT_BEVAL_ਪਦ)
/*
 * Called when 'balloonevalterm' changed.
 */
    ਖਾਲੀ
mch_bevalterm_ਬਦਲਿਆ(ਖਾਲੀ)
{
    mch_setmouse(g_f_Mouse_ਸਰਗਰਮ);
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
    ਸਥਿਰ ਖਾਲੀ
decode_mouse_wheel(MOUSE_EVENT_ਰਿਕਾਰਡ *pmer)
{
    ਪੂਰਨ	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    ਪੂਰਨ	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    ਪੂਰਨ lcol = g_xMouse;
    ਪੂਰਨ lrow = g_yMouse;
    win_T *wp = mouse_ਲੱਭੋ_win(&lrow, &lcol, ਲੱਭੋ_POPUP);
    ਜੇ (wp != ਨਿਰਾ && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_ਖੇਤਰ(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	ਜੇ (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ਖੱਬਾ : MSCR_ਸੱਜਾ;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	ਨਹੀਂਤਾਂ
	{
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	    cap.arg = zDelta < 0 ? MSCR_ਉੱਪਰ : MSCR_ਥੱਲੇ;
	}

	// Mouse hovers over popup window, scroll it if possible.
	mouse_ਪੰਕਤੀ = wp->w_winrow;
	mouse_ਖਾਨਾ = wp->w_wincol;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	ਬਾਹਰਲਾ_flush();
    }
# endif
    mouse_ਖਾਨਾ = g_xMouse;
    mouse_ਪੰਕਤੀ = g_yMouse;

    char_u modifiers = 0;
    char_u direction = 0;

    // Decode the direction into an event that Vim can process
    ਜੇ (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    ਨਹੀਂਤਾਂ
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    ਜੇ (pmer->dw_Control_ਕੁੰਜੀ_State & ਖਿਸਕਾਓ_PRESSED)
	modifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ; // MOUSE_SHIFT;
    ਜੇ (pmer->dw_Control_ਕੁੰਜੀ_State & (ਸੱਜਾ_CTRL_PRESSED | ਖੱਬਾ_CTRL_PRESSED))
	modifiers |= MOD_ਨਕਾਬ_CTRL; // MOUSE_CTRL;
    ਜੇ (pmer->dw_Control_ਕੁੰਜੀ_State & (ਸੱਜਾ_ALT_PRESSED  | ਖੱਬਾ_ALT_PRESSED))
	modifiers |= MOD_ਨਕਾਬ_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    ਵਾਪਸ;
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
    ਸਥਿਰ BOOL
decode_mouse_event(
    MOUSE_EVENT_ਰਿਕਾਰਡ *pmer)
{
    ਸਥਿਰ ਪੂਰਨ s_nOldButton = -1;
    ਸਥਿਰ ਪੂਰਨ s_nOldMouseClick = -1;
    ਸਥਿਰ ਪੂਰਨ s_xOldMouse = -1;
    ਸਥਿਰ ਪੂਰਨ s_yOldMouse = -1;
    ਸਥਿਰ linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    ਸਥਿਰ ਪੂਰਨ s_old_topfill = 0;
# endif
    ਸਥਿਰ ਪੂਰਨ s_cClicks = 1;
    ਸਥਿਰ BOOL s_fReleased = TRUE;
    ਸਥਿਰ DWORD s_dw_ਆਖਰੀ_Click_ਸਮਾਂ = 0;
    ਸਥਿਰ BOOL s_f_ਅਗਲਾ_Is_ਵਿਚਕਾਰ = FALSE;

    ਸਥਿਰ DWORD cButtons = 0;	// number of buttons supported

    ਅਚਲ DWORD ਖੱਬਾ_2 = FROM_ਖੱਬਾ_1_ST_BUTTON_PRESSED;
    ਅਚਲ DWORD ਵਿਚਕਾਰ = FROM_ਖੱਬਾ_2_ND_BUTTON_PRESSED;
    ਅਚਲ DWORD ਸੱਜਾ_2 = RIGHTMOST_BUTTON_PRESSED;
    ਅਚਲ DWORD ਖੱਬਾ_ਸੱਜਾ = ਖੱਬਾ_2 | ਸੱਜਾ_2;

    ਪੂਰਨ nButton;

    ਜੇ (cButtons == 0 && !Get_ਸੰਖਿਆ_ਦਾ_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    ਜੇ (!g_fMouseAvail || !g_f_Mouse_ਸਰਗਰਮ)
    {
	g_nMouseClick = -1;
	ਵਾਪਸ FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    ਜੇ (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	ਵਾਪਸ FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    ਜੇ (g_nMouseClick != -1)
	ਵਾਪਸ TRUE;

    ਜੇ (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	ਵਾਪਸ TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    ਜੇ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	ਜੇ (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    ਵਾਪਸ FALSE;
    }

    // If no buttons are pressed...
    ਜੇ ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	ਜੇ (s_fReleased)
	{
# ifdef FEAT_BEVAL_ਪਦ
	    // do return mouse move events when we want them
	    ਜੇ (p_bevalterm)
		nButton = MOUSE_DRAG;
	    ਨਹੀਂਤਾਂ
# endif
		ਵਾਪਸ FALSE;
	}

	s_fReleased = TRUE;
    }
    ਨਹੀਂਤਾਂ    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	ਜੇ (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & ਖੱਬਾ_ਸੱਜਾ);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    ਜੇ (dwLR == ਖੱਬਾ_2 || dwLR == ਸੱਜਾ_2)
	    {
		ਲਈ (;;)
		{
		    // wait a short time for next input event
		    ਜੇ (WaitForSingleObject(g_h_Con_ਵਿੱਚ, p_mouset / 3)
							     != WAIT_OBJECT_0)
			ਰੋਕੋ;
		    ਨਹੀਂਤਾਂ
		    {
			DWORD c_ਰਿਕਾਰਡਾਂ = 0;
			ਨਿਵੇਸ਼_ਰਿਕਾਰਡ ir;
			MOUSE_EVENT_ਰਿਕਾਰਡ* pmer2 = &ir.Event.MouseEvent;

			peek_console_ਨਿਵੇਸ਼(g_h_Con_ਵਿੱਚ, &ir, 1, &c_ਰਿਕਾਰਡਾਂ);

			ਜੇ (c_ਰਿਕਾਰਡਾਂ == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & ਖੱਬਾ_ਸੱਜਾ))
			    ਰੋਕੋ;
			ਨਹੀਂਤਾਂ
			{
			    ਜੇ (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_ਨਿਵੇਸ਼(g_h_Con_ਵਿੱਚ, &ir, 1, &c_ਰਿਕਾਰਡਾਂ);

				ਵਾਪਸ decode_mouse_event(pmer2);
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_ਨਿਵੇਸ਼(g_h_Con_ਵਿੱਚ, &ir, 1, &c_ਰਿਕਾਰਡਾਂ);

				// are there any more mouse events in queue?
				peek_console_ਨਿਵੇਸ਼(g_h_Con_ਵਿੱਚ, &ir, 1, &c_ਰਿਕਾਰਡਾਂ);

				ਜੇ (c_ਰਿਕਾਰਡਾਂ==0 || ir.EventType != MOUSE_EVENT)
				    ਰੋਕੋ;
			    }
			    ਨਹੀਂਤਾਂ
				ਰੋਕੋ;
			}
		    }
		}
	    }
	}

	ਜੇ (s_f_ਅਗਲਾ_Is_ਵਿਚਕਾਰ)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_ਵਿਚਕਾਰ;
	    s_f_ਅਗਲਾ_Is_ਵਿਚਕਾਰ = FALSE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (cButtons == 2	&&
	    ((pmer->dwButtonState & ਖੱਬਾ_ਸੱਜਾ) == ਖੱਬਾ_ਸੱਜਾ))
	{
	    nButton = MOUSE_ਵਿਚਕਾਰ;

	    ਜੇ (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_ਅਗਲਾ_Is_ਵਿਚਕਾਰ = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ ((pmer->dwButtonState & ਖੱਬਾ_2) == ਖੱਬਾ_2)
	    nButton = MOUSE_ਖੱਬਾ;
	ਨਹੀਂਤਾਂ ਜੇ ((pmer->dwButtonState & ਵਿਚਕਾਰ) == ਵਿਚਕਾਰ)
	    nButton = MOUSE_ਵਿਚਕਾਰ;
	ਨਹੀਂਤਾਂ ਜੇ ((pmer->dwButtonState & ਸੱਜਾ_2) == ਸੱਜਾ_2)
	    nButton = MOUSE_ਸੱਜਾ;

	ਜੇ (! s_fReleased && ! s_f_ਅਗਲਾ_Is_ਵਿਚਕਾਰ
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    ਵਾਪਸ FALSE;

	s_fReleased = s_f_ਅਗਲਾ_Is_ਵਿਚਕਾਰ;
    }

    ਜੇ (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	ਜੇ (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dw_Current_ਸਮਾਂ = Get_Tick_ਗਿਣਤੀ();

	    ਜੇ (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (ਪੂਰਨ)(dw_Current_ਸਮਾਂ - s_dw_ਆਖਰੀ_Click_ਸਮਾਂ) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dw_ਆਖਰੀ_Click_ਸਮਾਂ = dw_Current_ਸਮਾਂ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	ਜੇ (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    ਜੇ (nButton == -1)
	ਵਾਪਸ FALSE;

    ਜੇ (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    ਜੇ (pmer->dw_Control_ਕੁੰਜੀ_State & ਖਿਸਕਾਓ_PRESSED)
	g_nMouseClick |= MOUSE_ਖਿਸਕਾਓ;
    ਜੇ (pmer->dw_Control_ਕੁੰਜੀ_State & (ਸੱਜਾ_CTRL_PRESSED | ਖੱਬਾ_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    ਜੇ (pmer->dw_Control_ਕੁੰਜੀ_State & (ਸੱਜਾ_ALT_PRESSED  | ਖੱਬਾ_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    ਜੇ (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	ਸੈੱਟ_ਅੰਕ_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    ਜੇ (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	ਵਾਪਸ FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    ਵਾਪਸ TRUE;
}

# ifdef FEAT_EVAL
    ਸਥਿਰ ਪੂਰਨ
encode_mouse_event(dict_T *ਦਲੀਲਾਂ, ਨਿਵੇਸ਼_ਰਿਕਾਰਡ *ir)
{
    ਪੂਰਨ		button;
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		repeated_click;
    int_u	mods = 0;
    ਪੂਰਨ		move;

    ਜੇ (!dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "row") || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "col"))
	ਵਾਪਸ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (ਪੂਰਨ)dict_get_bool(ਦਲੀਲਾਂ, "move", FALSE);
    ਜੇ (!move && (!dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "button")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "multiclick")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "modifiers")))
	ਵਾਪਸ FALSE;

    ਪੰਕਤੀ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "row") - 1;
    ਖਾਨਾ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_ਰਿਕਾਰਡ mer;
    Zero_ਸਮ੍ਰਿਤੀ(&mer, ਆਕਾਰ(mer));
    mer.dwMousePosition.X  = ਖਾਨਾ;
    mer.dwMousePosition.Y  = ਪੰਕਤੀ;

    ਜੇ (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    ਨਹੀਂਤਾਂ
    {
	button = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "button");
	repeated_click = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "multiclick");
	mods = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_ਸੈੱਟ_hor_scroll_ਕਦਮ(6);
	mouse_ਸੈੱਟ_vert_scroll_ਕਦਮ(3);

	ਚੋਣ (button)
	{
	    ਹਾਲਤ MOUSE_ਖੱਬਾ:
		mer.dwButtonState = FROM_ਖੱਬਾ_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ਰੋਕੋ;
	    ਹਾਲਤ MOUSE_ਵਿਚਕਾਰ:
		mer.dwButtonState = FROM_ਖੱਬਾ_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ਰੋਕੋ;
	    ਹਾਲਤ MOUSE_ਸੱਜਾ:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ਰੋਕੋ;
	    ਹਾਲਤ MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    ਹਾਲਤ MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		ਰੋਕੋ;
	    ਹਾਲਤ MOUSE_X1:
		mer.dwButtonState = FROM_ਖੱਬਾ_3_RD_BUTTON_PRESSED;
		ਰੋਕੋ;
	    ਹਾਲਤ MOUSE_X2:
		mer.dwButtonState = FROM_ਖੱਬਾ_4_TH_BUTTON_PRESSED;
		ਰੋਕੋ;
	    ਹਾਲਤ MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ਰੋਕੋ;
	    ਹਾਲਤ MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ਰੋਕੋ;
	    ਹਾਲਤ MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ਰੋਕੋ;
	    ਹਾਲਤ MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ਰੋਕੋ;
	    ਮੂਲ:
		semsg(_(e_invalid_ਦਲੀਲ_str), "button");
		ਵਾਪਸ FALSE;
	}
    }

    mer.dw_Control_ਕੁੰਜੀ_State = 0;
    ਜੇ (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	ਜੇ (mods & MOUSE_ਖਿਸਕਾਓ)
	    mer.dw_Control_ਕੁੰਜੀ_State |= ਖਿਸਕਾਓ_PRESSED;
	ਜੇ (mods & MOUSE_CTRL)
	    mer.dw_Control_ਕੁੰਜੀ_State |= ਖੱਬਾ_CTRL_PRESSED;
	ਜੇ (mods & MOUSE_ALT)
	    mer.dw_Control_ਕੁੰਜੀ_State |= ਖੱਬਾ_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    ਵਾਪਸ TRUE;
}
# endif  // FEAT_EVAL

    ਸਥਿਰ ਪੂਰਨ
write_ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ(ਨਿਵੇਸ਼_ਰਿਕਾਰਡ* irEvents, ਪੂਰਨ n_ਲੰਮਾਈ)
{
    ਪੂਰਨ				n_ਗਿਣਤੀ = 0;
    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ_ਗੰਢ_T	*event_ਗੰਢ;

    ਜਦੋਂ (n_ਗਿਣਤੀ < n_ਲੰਮਾਈ)
    {
	event_ਗੰਢ = alloc(ਆਕਾਰ(ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ_ਗੰਢ_T));
	ਜੇ (event_ਗੰਢ == ਨਿਰਾ)
	    ਵਾਪਸ -1;

	event_ਗੰਢ->ir = irEvents[n_ਗਿਣਤੀ++];
	event_ਗੰਢ->ਅਗਲਾ = ਨਿਰਾ;

	ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਲੰਮਾਈ++;
	ਜੇ (ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਪੂਛ == ਨਿਰਾ)
	{
	    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਸਿਰ = event_ਗੰਢ;
	    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਪੂਛ = event_ਗੰਢ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਪੂਛ->ਅਗਲਾ = event_ਗੰਢ;
	    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਪੂਛ = ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਪੂਛ->ਅਗਲਾ;
	}
    }
    ਵਾਪਸ n_ਗਿਣਤੀ;
}

    ਸਥਿਰ ਪੂਰਨ
read_ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ(ਨਿਵੇਸ਼_ਰਿਕਾਰਡ* irEvents, ਪੂਰਨ n_Max_ਲੰਮਾਈ)
{
    ਪੂਰਨ n_ਗਿਣਤੀ = 0;
    ਜਦੋਂ (n_ਗਿਣਤੀ < n_Max_ਲੰਮਾਈ && ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਸਿਰ != ਨਿਰਾ)
    {
	ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਲੰਮਾਈ--;
	ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ_ਗੰਢ_T *ਕੱਢੋ_ਸਿਰ = ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਸਿਰ;
	irEvents[n_ਗਿਣਤੀ++] = ਕੱਢੋ_ਸਿਰ->ir;
	ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਸਿਰ = ਕੱਢੋ_ਸਿਰ->ਅਗਲਾ;
	vim_free(ਕੱਢੋ_ਸਿਰ);
	ਜੇ (ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਲੰਮਾਈ == 0)
	    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ.ਪੂਛ = ਨਿਰਾ;
    }
    ਵਾਪਸ n_ਗਿਣਤੀ;
}
#ਨਹੀਂਤਾਂ  // FEAT_GUI_MSWIN && !VIMDLL
    ਖਾਲੀ
mch_setmouse(ਪੂਰਨ on ਅਣਵਰਤਿਆ)
{
}
#ਜੇ_ਅੰਤ // FEAT_GUI_MSWIN && !VIMDLL

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
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
    ਪੂਰਨ
test_mswin_event(char_u *event, dict_T *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ lpEventsWritten = 0;

# ਜੇ defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ test_gui_w32_sendevent(event, ਦਲੀਲਾਂ);
# endif

# ਜੇ defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ ir;
    BOOL ਨਿਵੇਸ਼_encoded = FALSE;
    BOOL execute = FALSE;
    ਜੇ (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(ਦਲੀਲਾਂ, "execute", FALSE);
	ਜੇ (dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "event"))
	    ਨਿਵੇਸ਼_encoded = encode_ਕੁੰਜੀ_event(ਦਲੀਲਾਂ, &ir);
	ਨਹੀਂਤਾਂ ਜੇ (!execute)
	{
	    semsg(_(e_missing_ਦਲੀਲ_str), "event");
	    ਵਾਪਸ FALSE;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	ਨਿਵੇਸ਼_encoded = encode_mouse_event(ਦਲੀਲਾਂ, &ir);
    }
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "event", event);
	ਵਾਪਸ FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    ਜੇ (ਨਿਵੇਸ਼_encoded)
    {
	ਜੇ ((lpEventsWritten = write_ਨਿਵੇਸ਼_ਰਿਕਾਰਡ_ਬਫਰ(&ir, 1)) < 0)
	{
	    semsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ), "event");
	    ਵਾਪਸ FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    ਜੇ (execute)
    {
	ਪੂਰਨ save_ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	ਸੁਨੇਹਾ_scroll = FALSE;
	ch_log(ਨਿਰਾ, "test_mswin_event() executing");
	exec_ਸਾਧਾਰਨ(TRUE, TRUE, TRUE);
	ਸੁਨੇਹਾ_scroll |= save_ਸੁਨੇਹਾ_scroll;
    }

# endif
    ਵਾਪਸ lpEventsWritten;
}
#ਜੇ_ਅੰਤ // FEAT_EVAL

#ਜੇ defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    ਸਥਿਰ ਖਾਲੀ
mch_ਸੈੱਟ_cursor_shape(ਪੂਰਨ thickness)
{
    ਜੇ (vtp_working)
    {
	ਜੇ (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    ਜੇ (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    ਨਹੀਂਤਾਂ
		vtp_printf("\033[0 q");	// default
	}
    }
    ਨਹੀਂਤਾਂ
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dw_ਮਾਪ = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	ਸੈੱਟ_Console_Cursor_Info(g_h_Con_ਬਾਹਰਲਾ, &ConsoleCursorInfo);
	ਜੇ (s_cursor_visible)
	    ਸੈੱਟ_Console_Cursor_Position(g_h_Con_ਬਾਹਰਲਾ, g_coord);
    }
}

    ਖਾਲੀ
mch_update_cursor(ਖਾਲੀ)
{
    ਪੂਰਨ		idx;
    ਪੂਰਨ		thickness;

# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    ਜੇ (shape_ਸਾਰਣੀ[idx].shape == SHAPE_ਖੰਡ)
	thickness = 99;	// 100 doesn't work on W95
    ਨਹੀਂਤਾਂ
	thickness = shape_ਸਾਰਣੀ[idx].percentage;
    mch_ਸੈੱਟ_cursor_shape(thickness);
}
#ਜੇ_ਅੰਤ

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_focus_event(ਨਿਵੇਸ਼_ਰਿਕਾਰਡ ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.b_ਸੈੱਟ_Focus;
    ui_focus_change((ਪੂਰਨ)g_fJustGotFocus);
}

ਸਥਿਰ ਖਾਲੀ ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    ਸਥਿਰ ਪੂਰਨ
WaitForChar(ਲੰਮਾ msec, ਪੂਰਨ ignore_ਨਿਵੇਸ਼)
{
    DWORD	    dwNow = 0, dw_ਸਿਰਾ_ਸਮਾਂ = 0;
    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ    ir;
    DWORD	    c_ਰਿਕਾਰਡਾਂ;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    ਪੂਰਨ		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    ਜੇ (msec > 0)
	// Wait until the specified time has elapsed.
	dw_ਸਿਰਾ_ਸਮਾਂ = Get_Tick_ਗਿਣਤੀ() + msec;
    ਨਹੀਂਤਾਂ ਜੇ (msec < 0)
	// Wait forever.
	dw_ਸਿਰਾ_ਸਮਾਂ = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    ਲਈ (;;)
    {
	// Only process messages when waiting.
	ਜੇ (msec != 0)
	{
# ifdef ਸੁਨੇਹਾ_ਕਤਾਰ
	    parse_queued_messages();
# endif
# ifdef FEAT_MZSCHEME
	    mzvim_check_ਧਾਗੇ();
# endif
# ifdef FEAT_CLIENTSERVER
	    serverProcessPendingMessages();
# endif
	}

	ਜੇ (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_ਨਿਵੇਸ਼ && ਨਿਵੇਸ਼_ਉਪਲਬਧ())
# endif
	   )
	    ਵਾਪਸ TRUE;

	ਜੇ (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_ਗਿਣਤੀ();
	    ਜੇ ((ਪੂਰਨ)(dwNow - dw_ਸਿਰਾ_ਸਮਾਂ) >= 0)
		ਰੋਕੋ;
	}
	ਜੇ (msec != 0)
	{
	    DWORD dw_Wait_ਸਮਾਂ = dw_ਸਿਰਾ_ਸਮਾਂ - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    ਜੇ (dw_Wait_ਸਮਾਂ > 11)
		dw_Wait_ਸਮਾਂ = 11;

# ifdef FEAT_MZSCHEME
	    ਜੇ (mzthreads_allowed() && p_mzq > 0 && (ਲੰਮਾ)dw_Wait_ਸਮਾਂ > p_mzq)
		dw_Wait_ਸਮਾਂ = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    ਜੇ (dw_Wait_ਸਮਾਂ > 10)
	    {
		ਲੰਮਾ	due_ਸਮਾਂ;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_ਸਮਾਂ = check_due_timer();
		ਜੇ (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    ਵਾਪਸ FALSE;
		}
		ਜੇ (due_ਸਮਾਂ > 0 && dw_Wait_ਸਮਾਂ > (DWORD)due_ਸਮਾਂ)
		    dw_Wait_ਸਮਾਂ = due_ਸਮਾਂ;
	    }
# endif
	    ਜੇ (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    ਸੁਨੇਹਾ_wait_for_ਗੁਣਜ_objects(1, &g_h_Con_ਵਿੱਚ, FALSE,
				  dw_Wait_ਸਮਾਂ, QS_SENDMESSAGE) != WAIT_OBJECT_0
# ਨਹੀਂਤਾਂ
		    wait_for_single_object(g_h_Con_ਵਿੱਚ, dw_Wait_ਸਮਾਂ)
							      != WAIT_OBJECT_0
# endif
		    )
		ਜਾਰੀ;
	}

	c_ਰਿਕਾਰਡਾਂ = 0;
	peek_console_ਨਿਵੇਸ਼(g_h_Con_ਵਿੱਚ, &ir, 1, &c_ਰਿਕਾਰਡਾਂ);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	ਜੇ (State == ਬਹੁਲਕ_CMDLINE && ਸੁਨੇਹਾ_ਪੰਕਤੀ == ਪੰਕਤੀਆਂ - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_ਬਫਰ_INFO csbi;

	    ਜੇ (Get_Console_Screen_ਬਫਰ_Info(g_h_Con_ਬਾਹਰਲਾ, &csbi))
	    {
		ਜੇ (csbi.dwCursorPosition.Y != ਸੁਨੇਹਾ_ਪੰਕਤੀ)
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

	ਜੇ (c_ਰਿਕਾਰਡਾਂ > 0)
	{
	    ਜੇ (ir.EventType == ਕੁੰਜੀ_EVENT && ir.Event.ਕੁੰਜੀ_Event_2.b_ਕੁੰਜੀ_ਥੱਲੇ)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		ਜੇ (ir.Event.ਕੁੰਜੀ_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.ਕੁੰਜੀ_Event_2.w_Virtual_ਕੁੰਜੀ_ਕੋਡ == 13)
		{
		    read_console_ਨਿਵੇਸ਼(g_h_Con_ਵਿੱਚ, &ir, 1, &c_ਰਿਕਾਰਡਾਂ);
		    ਜਾਰੀ;
		}
# endif
		ਜੇ (decode_ਕੁੰਜੀ_event(&ir.Event.ਕੁੰਜੀ_Event_2, &ch, &ch2,
								 ਨਿਰਾ, FALSE))
		    ਵਾਪਸ TRUE;
	    }

	    read_console_ਨਿਵੇਸ਼(g_h_Con_ਵਿੱਚ, &ir, 1, &c_ਰਿਕਾਰਡਾਂ);

	    ਜੇ (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    ਨਹੀਂਤਾਂ ਜੇ (ir.EventType == ਖਿੜਕੀ_ਬਫਰ_ਮਾਪ_EVENT)
	    {
		COORD dw_ਮਾਪ = ir.Event.ਖਿੜਕੀ_ਬਫਰ_ਮਾਪ_Event.dw_ਮਾਪ;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		ਜੇ (dw_ਮਾਪ.X != Columns || dw_ਮਾਪ.Y != ਪੰਕਤੀਆਂ)
		{
		    CONSOLE_SCREEN_ਬਫਰ_INFO csbi;
		    Get_Console_Screen_ਬਫਰ_Info(g_h_Con_ਬਾਹਰਲਾ, &csbi);
		    dw_ਮਾਪ.X = csbi.sr_ਖਿੜਕੀ.ਸੱਜਾ_3 - csbi.sr_ਖਿੜਕੀ.ਖੱਬਾ_3 + 1;
		    dw_ਮਾਪ.Y = csbi.sr_ਖਿੜਕੀ.Bottom - csbi.sr_ਖਿੜਕੀ.ਸਿਖਰ_2 + 1;
		    ਜੇ (dw_ਮਾਪ.X != Columns || dw_ਮਾਪ.Y != ਪੰਕਤੀਆਂ)
		    {
			ResizeConBuf(g_h_Con_ਬਾਹਰਲਾ, dw_ਮਾਪ);
			shell_resized();
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		ਵਾਪਸ TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (msec == 0)
	    ਰੋਕੋ;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    ਜੇ (ਨਿਵੇਸ਼_ਉਪਲਬਧ())
	ਵਾਪਸ TRUE;
# endif

    ਵਾਪਸ FALSE;
}

/*
 * return non-zero if a character is available
 */
    ਪੂਰਨ
mch_char_avail(ਖਾਲੀ)
{
# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ TRUE;
# endif
    ਵਾਪਸ WaitForChar(0L, FALSE);
}

# ਜੇ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ਪੂਰਨ
mch_check_messages(ਖਾਲੀ)
{
#  ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ TRUE;
#  endif
    ਵਾਪਸ WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    ਸਥਿਰ ਖਾਲੀ
create_conin(ਖਾਲੀ)
{
    g_h_Con_ਵਿੱਚ =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ਗੁਣ) ਨਿਰਾ,
			OPEN_EXISTING, 0, (HANDLE)ਨਿਰਾ);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    ਸਥਿਰ WCHAR
tgetch(ਪੂਰਨ *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    ਲਈ (;;)
    {
	ਨਿਵੇਸ਼_ਰਿਕਾਰਡ ir;
	DWORD c_ਰਿਕਾਰਡਾਂ = 0;

# ifdef FEAT_CLIENTSERVER
	(ਖਾਲੀ)WaitForChar(-1L, FALSE);
	ਜੇ (ਨਿਵੇਸ਼_ਉਪਲਬਧ())
	    ਵਾਪਸ 0;
	ਜੇ (g_nMouseClick != -1)
	    ਵਾਪਸ 0;
# endif
	ਜੇ (read_console_ਨਿਵੇਸ਼(g_h_Con_ਵਿੱਚ, &ir, 1, &c_ਰਿਕਾਰਡਾਂ) == 0)
	{
	    ਜੇ (did_create_conin)
		read_error_exit();
	    create_conin();
	    ਜਾਰੀ;
	}

	ਜੇ (ir.EventType == ਕੁੰਜੀ_EVENT)
	{
	    ਜੇ (decode_ਕੁੰਜੀ_event(&ir.Event.ਕੁੰਜੀ_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		ਵਾਪਸ ch;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	ਨਹੀਂਤਾਂ ਜੇ (ir.EventType == ਖਿੜਕੀ_ਬਫਰ_ਮਾਪ_EVENT)
	    shell_resized();
	ਨਹੀਂਤਾਂ ਜੇ (ir.EventType == MOUSE_EVENT)
	{
	    ਜੇ (decode_mouse_event(&ir.Event.MouseEvent))
		ਵਾਪਸ 0;
	}
    }
}
#ਜੇ_ਅੰਤ // !FEAT_GUI_MSWIN


/*
 * mch_inchar(): low-level input function.
 * Get one or more characters from the keyboard or the mouse.
 * If time == 0, do not wait for characters.
 * If time == n, wait a short time for characters.
 * If time == -1, wait forever for characters.
 * Returns the number of characters read into buf.
 */
    ਪੂਰਨ
mch_inchar(
    char_u	*buf ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		maxlen ਅਣਵਰਤਿਆ,
    ਲੰਮਾ	ਸਮਾਂ ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		tb_change_cnt ਅਣਵਰਤਿਆ)
{
#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ਪੂਰਨ		len;
    ਪੂਰਨ		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# ਨਹੀਂਤਾਂ
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    ਸਥਿਰ char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    ਸਥਿਰ ਪੂਰਨ	    typeaheadlen = 0;

# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    ਜੇ (typeaheadlen > 0)
	ਜਾਓ theend;

    ਜੇ (ਸਮਾਂ >= 0)
    {
	ਜੇ (!WaitForChar(ਸਮਾਂ, FALSE))     // no character available
	    ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ    // time == -1, wait forever
    {
	mch_ਸੈੱਟ_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ਜੇ (!WaitForChar(p_ut, FALSE))
	{
	    ਜੇ (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (ਪੂਰਨ)KE_CURSORHOLD;
		ਵਾਪਸ 3;
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
    ਜੇ (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    ਜਦੋਂ ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	ਜੇ (typebuf_ਬਦਲਿਆ(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    ਰੋਕੋ;
	}
	ਜੇ (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    ਜੇ (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    ਜੇ (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		ਜੇ (modifiers > 0)
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
	    ਨਹੀਂਤਾਂ
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
	ਨਹੀਂਤਾਂ
	{
	    WCHAR	ch2 = NUL;
	    ਪੂਰਨ		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_ਕੁੰਜੀ(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_ਕੁੰਜੀਆਂ;
	    c = may_adjust_ਕੁੰਜੀ_for_ctrl(modifiers, c);
	    --no_reduce_ਕੁੰਜੀਆਂ;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_ਖਿਸਕਾਓ_modifier(modifiers, c);

	    ਜੇ (typebuf_ਬਦਲਿਆ(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		ਰੋਕੋ;
	    }

	    ਜੇ (c == Ctrl_C && ctrl_c_interrupts)
	    {
# ਜੇ defined(FEAT_CLIENTSERVER)
		trash_ਨਿਵੇਸ਼_buf();
# endif
		got_int = TRUE;
	    }

	    ਜੇ (g_nMouseClick == -1)
	    {
		ਪੂਰਨ	n = 1;

		ਜੇ (ch2 == NUL)
		{
		    ਪੂਰਨ	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    ਜੇ (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_ਮੰਜ਼ਿਲ_enc(ch, &n);
		    ਜੇ (p != ਨਿਰਾ)
		    {
			ਲਈ (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    ਜੇ (p[i] == CSI)
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
		ਨਹੀਂਤਾਂ
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    ਜੇ (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		ਜੇ (ch2 != NUL)
		{
		    ਜੇ (c == K_NUL)
		    {
			ਚੋਣ (ch2)
			{
			ਹਾਲਤ (WCHAR)'\324': // SHIFT+Insert
			ਹਾਲਤ (WCHAR)'\325': // CTRL+Insert
			ਹਾਲਤ (WCHAR)'\327': // SHIFT+Delete
			ਹਾਲਤ (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    ਰੋਕੋ;

			ਮੂਲ:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    ਰੋਕੋ;
			}
		    }
		    ਨਹੀਂਤਾਂ
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
		ਜੇ ((modifiers & MOD_ਨਕਾਬ_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_ਨਕਾਬ_ALT;
		}

		ਜੇ (modifiers != 0)
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
		ਜੇ (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    ਜੇ (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    ਜਦੋਂ (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    ਜੇ (len > 0)
    {
	buf[len] = NUL;
	ch_log(ਨਿਰਾ, "raw key input: \"%s\"", buf);
    }
# endif
    ਵਾਪਸ len;

#ਨਹੀਂਤਾਂ // FEAT_GUI_MSWIN && !VIMDLL
    ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    ਸਥਿਰ ਪੂਰਨ
executable_file(ਅੱਖਰ *name, char_u **ਰਸਤਾ)
{
    ਪੂਰਨ attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    ਜੇ (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	ਵਾਪਸ FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    ਜੇ (attrs & FILE_ATTRIBUTE_REPARSE_ਬਿੰਦੂ)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	ਜੇ (res == ਨਿਰਾ)
	    res = resolve_reparse_ਬਿੰਦੂ((char_u *)name);
	ਜੇ (res == ਨਿਰਾ)
	    ਵਾਪਸ FALSE;
	// The path is already absolute.
	ਜੇ (ਰਸਤਾ != ਨਿਰਾ)
	    *ਰਸਤਾ = res;
	ਨਹੀਂਤਾਂ
	    vim_free(res);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਰਸਤਾ != ਨਿਰਾ)
	*ਰਸਤਾ = ਭਰਿਆ_Name_save((char_u *)name, FALSE);
    ਵਾਪਸ TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    ਸਥਿਰ bool
executable_ਅਗੇਤਰ_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_ਲੱਭੋ_DATAW	ਅੰਕੜੇ;
    HANDLE		handle;
    DWORD		error;

    wn = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (wn == ਨਿਰਾ)
	ਵਾਪਸ true;
    handle = ਲੱਭੋ_ਪਹਿਲਾ_File_Ex_W(wn, ਲੱਭੋ_Ex_Info_Basic, &ਅੰਕੜੇ,
					    ਲੱਭੋ_Ex_ਖੋਜ_Name_ਮੇਲ, ਨਿਰਾ, 0);
    error = Get_ਆਖਰੀ_Error();
    vim_free(wn);
    ਜੇ (handle != INVALID_HANDLE_ਮੁੱਲ)
    {
	ਲੱਭੋ_Close(handle);
	ਵਾਪਸ true;
    }
    ਵਾਪਸ error != ERROR_FILE_NOT_ਮਿਲਿਆ && error != ERROR_ਰਸਤਾ_NOT_ਮਿਲਿਆ;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    ਸਥਿਰ ਪੂਰਨ
executable_exists(
    ਅੱਖਰ	*name,
    size_t	namelen,
    char_u	**ਰਸਤਾ,
    ਪੂਰਨ		use_ਰਸਤਾ,
    ਪੂਰਨ		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[MAX_ਰਸਤਾ_2 * 3];
    char_u	*p;
    string_T	pathbuf = {ਨਿਰਾ, 0};
    ਪੂਰਨ		pathbuf_allocated = FALSE;
    string_T	pathext = {ਨਿਰਾ, 0};
    ਪੂਰਨ		pathext_allocated = FALSE;
    char_u	*shname = ਨਿਰਾ;
    ਪੂਰਨ		noext = FALSE;
    bool	simple_pathext = true;
    ਪੂਰਨ		ext_ਗਿਣਤੀ = 0;
    ਪੂਰਨ		retval = FALSE;

    ਜੇ (namelen >= ਆਕਾਰ(buf))	// safety check
	ਵਾਪਸ FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)shname, "sh") != ਨਿਰਾ &&
	!(ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)shname, "powershell") != ਨਿਰਾ
				    || ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)shname, "pwsh") != ਨਿਰਾ))
	noext = TRUE;

    ਜੇ (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	ਜੇ (pathext.string == ਨਿਰਾ)
	    STR_LITERAL_ਸੈੱਟ(pathext, ".com;.exe;.bat;.cmd");
	ਨਹੀਂਤਾਂ
	    pathext.ਲੰਮਾਈ = STRLEN(pathext.string);

	ਜੇ (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    ਜਦੋਂ (*p)
	    {
		ਜੇ (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    ਜੇ (*p == '.')
			simple_pathext = false;
		    ++p;
		    ਜਾਰੀ;
		}
		e = vim_strchr(p, ';');
		ਜੇ (e == ਨਿਰਾ)
		    e = pathext.string + pathext.ਲੰਮਾਈ;
		plen = (size_t)(e - p);
		++ext_ਗਿਣਤੀ;
		ਜੇ (*p != '.')
		    simple_pathext = false;
		ਲਈ (q = p + 1; q < e; ++q)
		    ਜੇ (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		ਜੇ (_strnicoll(name + namelen - plen, (ਅੱਖਰ *)p, plen) == 0)
		{
		    noext = TRUE;
		    ਰੋਕੋ;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    ਜੇ (pathext.string == ਨਿਰਾ)
	STR_LITERAL_ਸੈੱਟ(pathext, ".");
    ਨਹੀਂਤਾਂ ਜੇ (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.ਲੰਮਾਈ + 3);
	ਜੇ (tmp == ਨਿਰਾ)
	{
	    retval = FALSE;
	    ਜਾਓ theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.ਲੰਮਾਈ += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    ਜੇ (use_ਰਸਤਾ && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	ਜੇ (p != ਨਿਰਾ)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    ਜੇ (pathbuf.string == ਨਿਰਾ)
	    {
		retval = FALSE;
		ਜਾਓ theend;
	    }

	    ਜੇ (mch_getenv("NoDefaultCurrentDirectoryInExePath") == ਨਿਰਾ &&
		    ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)gettail(p_sh), "cmd.exe") != ਨਿਰਾ)
	    {
		STRCPY(pathbuf.string, ".;");
		pathbuf.ਲੰਮਾਈ = 2;
	    }
	    STRCPY(pathbuf.string + pathbuf.ਲੰਮਾਈ, p);
	    pathbuf.ਲੰਮਾਈ += plen;
	    pathbuf_allocated = TRUE;
	}
    }

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    ਜੇ (pathbuf.string == ਨਿਰਾ)
	STR_LITERAL_ਸੈੱਟ(pathbuf, ".");
    p = pathbuf.string;
    ਜਦੋਂ (*p)
    {
	char_u	*e;
	size_t	buflen;

	ਜੇ (*p == ';') // Skip empty entry
	{
	    ++p;
	    ਜਾਰੀ;
	}
	e = vim_strchr(p, ';');
	ਜੇ (e == ਨਿਰਾ)
	    e = pathbuf.string + pathbuf.ਲੰਮਾਈ;

	ਜੇ (e - p + namelen + 2 > ਆਕਾਰ(buf))
	{
	    retval = FALSE;
	    ਜਾਓ theend;
	}
	// A single "." that means current dir.
	ਜੇ (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	ਨਹੀਂਤਾਂ
	{
	    buflen = vim_snprintf_safelen(
		(ਅੱਖਰ *)buf,
		ਆਕਾਰ(buf),
		"%.*s%s%s", (ਪੂਰਨ)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	ਜੇ (!noext && simple_pathext && ext_ਗਿਣਤੀ >= 3 && buflen > 0
		&& buflen + 2 <= ਆਕਾਰ(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == ਨਿਰਾ
		&& vim_strchr(gettail(buf), ':') == ਨਿਰਾ)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_ਅਗੇਤਰ_exists(buf);
	    buf[buflen] = NUL;
	    ਜੇ (!exists)
	    {
		p = e;
		ਜਾਰੀ;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	ਜਦੋਂ (*p)
	{
	    char_u  *e2;

	    ਜੇ (*p == ';')
	    {
		// Skip empty entry
		++p;
		ਜਾਰੀ;
	    }
	    e2 = vim_strchr(p, ';');
	    ਜੇ (e2 == ਨਿਰਾ)
		e2 = pathext.string + pathext.ਲੰਮਾਈ;

	    ਜੇ (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		ਜੇ (e2 - p + buflen + 1 > ਆਕਾਰ(buf))
		{
		    retval = FALSE;
		    ਜਾਓ theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    ਜੇ (executable_file((ਅੱਖਰ *)buf, ਰਸਤਾ))
	    {
		retval = TRUE;
		ਜਾਓ theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    ਜੇ (pathbuf_allocated)
	ਮੁਕਤ(pathbuf.string);
    ਜੇ (pathext_allocated)
	ਮੁਕਤ(pathext.string);
    ਵਾਪਸ retval;
}

#ਜੇ (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    ਸਥਿਰ ਖਾਲੀ
bad_param_handler(ਅਚਲ wchar_t *ਵਿਅੰਜਕ_2 ਅਣਵਰਤਿਆ,
    ਅਚਲ wchar_t *function ਅਣਵਰਤਿਆ,
    ਅਚਲ wchar_t *file ਅਣਵਰਤਿਆ,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸਤਰ ਅਣਵਰਤਿਆ,
    uintptr_t pReserved ਅਣਵਰਤਿਆ)
{
}

# define ਸੈੱਟ_INVALID_PARAM_HANDLER \
	((ਖਾਲੀ)ਸੈੱਟ_invalid_parameter_handler(bad_param_handler))
#ਨਹੀਂਤਾਂ
# define ਸੈੱਟ_INVALID_PARAM_HANDLER
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    ਸਥਿਰ ਖਾਲੀ
mch_init_g(ਖਾਲੀ)
{
# ifndef __MINGW32__
    ਬਾਹਰੀ ਪੂਰਨ _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    ਸੈੱਟ_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    ਸੈੱਟ_Error_ਬਹੁਲਕ(SEM_FAILCRITICALERRORS);

    _fmode = O_ਦੁਆਧਾਰੀ;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    ਪੰਕਤੀਆਂ = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[MAX_ਰਸਤਾ_2 + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	ਜੇ (exe_pathlen + 10 >= ਆਕਾਰ(vimrun_location))
	{
	    ਜੇ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ਨਿਰਾ, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // First try in same directory as gvim.exe.
	    ਜੇ (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    ਜੇ (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		ਜੇ (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(ਅੱਖਰ *)vimrun_location,
			ਆਕਾਰ(vimrun_location),
			"\"%.*svimrun\" ",
			(ਪੂਰਨ)exe_pathlen, exe_name);
		}
		ਨਹੀਂਤਾਂ
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		ਜੇ (p != ਨਿਰਾ)
		{
		    vimrun_ਰਸਤਾ.string = p;
		    vimrun_ਰਸਤਾ.ਲੰਮਾਈ = plen;
		    vimrun_ਰਸਤਾ_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ਨਿਰਾ, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	ਜੇ (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    ਜੇ (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    ਨਿਰਾ, TRUE, FALSE))
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#ਜੇ_ਅੰਤ // FEAT_GUI_MSWIN

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).ਸੱਜਾ_3 - (sr).ਖੱਬਾ_3 + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).ਸਿਖਰ_2 + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    ਸਥਿਰ BOOL
Clear_Console_ਬਫਰ(ਸ਼ਬਦ_2 wAttribute)
{
    CONSOLE_SCREEN_ਬਫਰ_INFO csbi;
    COORD coord;
    DWORD ਅੰਕ_Cells, dummy;

    ਜੇ (!Get_Console_Screen_ਬਫਰ_Info(g_h_Con_ਬਾਹਰਲਾ, &csbi))
	ਵਾਪਸ FALSE;

    ਅੰਕ_Cells = csbi.dw_ਮਾਪ.X * csbi.dw_ਮਾਪ.Y;
    coord.X = 0;
    coord.Y = 0;
    ਜੇ (!FillConsoleOutputCharacter(g_h_Con_ਬਾਹਰਲਾ, ' ', ਅੰਕ_Cells,
	    coord, &dummy))
	ਵਾਪਸ FALSE;
    ਜੇ (!FillConsoleOutputAttribute(g_h_Con_ਬਾਹਰਲਾ, wAttribute, ਅੰਕ_Cells,
	    coord, &dummy))
	ਵਾਪਸ FALSE;

    ਵਾਪਸ TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    ਸਥਿਰ BOOL
Fit_Console_ਖਿੜਕੀ(
    COORD dw_ਬਫਰ_ਮਾਪ,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_ਬਫਰ_INFO csbi;
    COORD dw_ਖਿੜਕੀ_ਮਾਪ;
    BOOL NeedAdjust = FALSE;

    ਜੇ (!Get_Console_Screen_ਬਫਰ_Info(g_h_Con_ਬਾਹਰਲਾ, &csbi))
	ਵਾਪਸ FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    ਜੇ (csbi.sr_ਖਿੜਕੀ.ਸੱਜਾ_3 >= dw_ਬਫਰ_ਮਾਪ.X)
    {
	dw_ਖਿੜਕੀ_ਮਾਪ.X = SRWIDTH(csbi.sr_ਖਿੜਕੀ);
	ਜੇ (dw_ਖਿੜਕੀ_ਮਾਪ.X > dw_ਬਫਰ_ਮਾਪ.X)
	    dw_ਖਿੜਕੀ_ਮਾਪ.X = dw_ਬਫਰ_ਮਾਪ.X;
	csbi.sr_ਖਿੜਕੀ.ਸੱਜਾ_3 = dw_ਬਫਰ_ਮਾਪ.X - 1;
	csbi.sr_ਖਿੜਕੀ.ਖੱਬਾ_3 = dw_ਬਫਰ_ਮਾਪ.X - dw_ਖਿੜਕੀ_ਮਾਪ.X;
	NeedAdjust = TRUE;
    }
    ਜੇ (csbi.sr_ਖਿੜਕੀ.Bottom >= dw_ਬਫਰ_ਮਾਪ.Y)
    {
	dw_ਖਿੜਕੀ_ਮਾਪ.Y = SRHEIGHT(csbi.sr_ਖਿੜਕੀ);
	ਜੇ (dw_ਖਿੜਕੀ_ਮਾਪ.Y > dw_ਬਫਰ_ਮਾਪ.Y)
	    dw_ਖਿੜਕੀ_ਮਾਪ.Y = dw_ਬਫਰ_ਮਾਪ.Y;
	csbi.sr_ਖਿੜਕੀ.Bottom = dw_ਬਫਰ_ਮਾਪ.Y - 1;
	csbi.sr_ਖਿੜਕੀ.ਸਿਖਰ_2 = dw_ਬਫਰ_ਮਾਪ.Y - dw_ਖਿੜਕੀ_ਮਾਪ.Y;
	NeedAdjust = TRUE;
    }
    ਜੇ (NeedAdjust && WantAdjust)
    {
	ਜੇ (!ਸੈੱਟ_Console_ਖਿੜਕੀ_Info(g_h_Con_ਬਾਹਰਲਾ, TRUE, &csbi.sr_ਖਿੜਕੀ))
	    ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Console_ਬਫਰ_Struct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_ਬਫਰ_INFO	Info;
    PCHAR_INFO			ਬਫਰ_2;
    COORD			ਬਫਰ_ਮਾਪ_2;
    PSMALL_RECT			Regions;
    ਪੂਰਨ				ਅੰਕ_Regions;
} Console_ਬਫਰ;

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
    ਸਥਿਰ BOOL
Save_Console_ਬਫਰ(
    Console_ਬਫਰ *cb)
{
    DWORD ਅੰਕ_Cells;
    COORD ਬਫਰ_Coord;
    SMALL_RECT ReadRegion;
    ਸ਼ਬਦ_2 Y, Y_incr;
    ਪੂਰਨ i, numregions;

    ਜੇ (cb == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਜੇ (!Get_Console_Screen_ਬਫਰ_Info(g_h_Con_ਬਾਹਰਲਾ, &cb->Info))
    {
	cb->IsValid = FALSE;
	ਵਾਪਸ FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    ਜੇ (use_alternate_screen_ਬਫਰ)
	ਵਾਪਸ TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    ਜੇ (!cb->IsValid || cb->ਬਫਰ_2 == ਨਿਰਾ ||
	    cb->ਬਫਰ_ਮਾਪ_2.X != cb->Info.dw_ਮਾਪ.X ||
	    cb->ਬਫਰ_ਮਾਪ_2.Y != cb->Info.dw_ਮਾਪ.Y)
    {
	cb->ਬਫਰ_ਮਾਪ_2.X = cb->Info.dw_ਮਾਪ.X;
	cb->ਬਫਰ_ਮਾਪ_2.Y = cb->Info.dw_ਮਾਪ.Y;
	ਅੰਕ_Cells = cb->ਬਫਰ_ਮਾਪ_2.X * cb->ਬਫਰ_ਮਾਪ_2.Y;
	vim_free(cb->ਬਫਰ_2);
	cb->ਬਫਰ_2 = ALLOC_MULT(CHAR_INFO, ਅੰਕ_Cells);
	ਜੇ (cb->ਬਫਰ_2 == ਨਿਰਾ)
	    ਵਾਪਸ FALSE;
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
    ਬਫਰ_Coord.X = 0;
    ReadRegion.ਖੱਬਾ_3 = 0;
    ReadRegion.ਸੱਜਾ_3 = cb->Info.dw_ਮਾਪ.X - 1;
    Y_incr = 12000 / cb->Info.dw_ਮਾਪ.X;

    numregions = (cb->Info.dw_ਮਾਪ.Y + Y_incr - 1) / Y_incr;
    ਜੇ (cb->Regions == ਨਿਰਾ || numregions != cb->ਅੰਕ_Regions)
    {
	cb->ਅੰਕ_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->ਅੰਕ_Regions);
	ਜੇ (cb->Regions == ਨਿਰਾ)
	{
	    VIM_CLEAR(cb->ਬਫਰ_2);
	    ਵਾਪਸ FALSE;
	}
    }

    ਲਈ (i = 0, Y = 0; i < cb->ਅੰਕ_Regions; i++, Y += Y_incr)
    {
	/*
	 * Read into position (0, Y) in our buffer.
	 */
	ਬਫਰ_Coord.Y = Y;
	/*
	 * Read the region whose top left corner is (0, Y) and whose bottom
	 * right corner is (width - 1, Y + Y_incr - 1).  This should define
	 * a region of size width by Y_incr.  Don't worry if this region is
	 * too large for the remaining buffer; it will be cropped.
	 */
	ReadRegion.ਸਿਖਰ_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	ਜੇ (!ReadConsoleOutputW(g_h_Con_ਬਾਹਰਲਾ,	// output handle
		cb->ਬਫਰ_2,			// our buffer
		cb->ਬਫਰ_ਮਾਪ_2,			// dimensions of our buffer
		ਬਫਰ_Coord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->ਬਫਰ_2);
	    VIM_CLEAR(cb->Regions);
	    ਵਾਪਸ FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    ਵਾਪਸ TRUE;
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
    ਸਥਿਰ BOOL
Restore_Console_ਬਫਰ(
    Console_ਬਫਰ   *cb,
    BOOL	    RestoreScreen)
{
    COORD ਬਫਰ_Coord;
    SMALL_RECT WriteRegion;
    ਪੂਰਨ i;

    ਜੇ (cb == ਨਿਰਾ || !cb->IsValid)
	ਵਾਪਸ FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    ਜੇ (RestoreScreen)
	Clear_Console_ਬਫਰ(cb->Info.w_ਗੁਣ);

    Fit_Console_ਖਿੜਕੀ(cb->Info.dw_ਮਾਪ, TRUE);
    ਜੇ (!ਸੈੱਟ_Console_Screen_ਬਫਰ_ਮਾਪ(g_h_Con_ਬਾਹਰਲਾ, cb->Info.dw_ਮਾਪ))
	ਵਾਪਸ FALSE;
    ਜੇ (!ਸੈੱਟ_Console_ਪਾਠ_Attribute(g_h_Con_ਬਾਹਰਲਾ, cb->Info.w_ਗੁਣ))
	ਵਾਪਸ FALSE;

    ਜੇ (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	ਵਾਪਸ TRUE;
    }

    ਜੇ (!ਸੈੱਟ_Console_Cursor_Position(g_h_Con_ਬਾਹਰਲਾ, cb->Info.dwCursorPosition))
	ਵਾਪਸ FALSE;
    ਜੇ (!ਸੈੱਟ_Console_ਖਿੜਕੀ_Info(g_h_Con_ਬਾਹਰਲਾ, TRUE, &cb->Info.sr_ਖਿੜਕੀ))
	ਵਾਪਸ FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    ਜੇ (use_alternate_screen_ਬਫਰ)
	ਵਾਪਸ TRUE;

    /*
     * Restore the screen buffer contents.
     */
    ਜੇ (cb->ਬਫਰ_2 != ਨਿਰਾ)
    {
	ਲਈ (i = 0; i < cb->ਅੰਕ_Regions; i++)
	{
	    ਬਫਰ_Coord.X = cb->Regions[i].ਖੱਬਾ_3;
	    ਬਫਰ_Coord.Y = cb->Regions[i].ਸਿਖਰ_2;
	    WriteRegion = cb->Regions[i];
	    ਜੇ (!WriteConsoleOutputW(g_h_Con_ਬਾਹਰਲਾ,	// output handle
			cb->ਬਫਰ_2,		// our buffer
			cb->ਬਫਰ_ਮਾਪ_2,		// dimensions of our buffer
			ਬਫਰ_Coord,		// offset in our buffer
			&WriteRegion))		// region to restore
		ਵਾਪਸ FALSE;
	}
    }

    ਵਾਪਸ TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
ਸਥਿਰ Console_ਬਫਰ g_cbOrig = { 0 };
# endif
ਸਥਿਰ Console_ਬਫਰ g_cbNonTermcap = { 0 };
ਸਥਿਰ Console_ਬਫਰ g_cbTermcap = { 0 };

ਅੱਖਰ g_szOrigTitle[256] = { 0 };
HWND g_hWnd = ਨਿਰਾ;	// also used in os_mswin.c
ਸਥਿਰ HICON g_hOrigIconSmall = ਨਿਰਾ;
ਸਥਿਰ HICON g_hOrigIcon = ਨਿਰਾ;
ਸਥਿਰ HICON g_hVimIcon = ਨਿਰਾ;
ਸਥਿਰ BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ਸਥਿਰ BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    ਜੇ (hWnd == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਜੇ (phIconSmall != ਨਿਰਾ)
	*phIconSmall = (HICON)Send_ਸੁਨੇਹਾ(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    ਜੇ (phIcon != ਨਿਰਾ)
	*phIcon = (HICON)Send_ਸੁਨੇਹਾ(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    ਵਾਪਸ TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ਸਥਿਰ BOOL
ਸੈੱਟ_Console_Icon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    ਜੇ (hWnd == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਜੇ (hIconSmall != ਨਿਰਾ)
	Send_ਸੁਨੇਹਾ(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    ਜੇ (hIcon != ਨਿਰਾ)
	Send_ਸੁਨੇਹਾ(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    ਵਾਪਸ TRUE;
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
    ਸਥਿਰ ਖਾਲੀ
SaveConsoleTitleAndIcon(ਖਾਲੀ)
{
    // Save the original title.
    ਜੇ (!GetConsoleTitle(g_szOrigTitle, ਆਕਾਰ(g_szOrigTitle)))
	ਵਾਪਸ;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = Get_Console_ਖਿੜਕੀ();
    ਜੇ (g_hWnd == ਨਿਰਾ)
	ਵਾਪਸ;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    ਜੇ (g_hOrigIconSmall == ਨਿਰਾ || g_hOrigIcon == ਨਿਰਾ)
	ਵਾਪਸ;

    // Extract the first icon contained in the Vim executable.
    ਜੇ (
# ifdef FEAT_LIBCALL
	    mch_icon_ਲੱਦੋ((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == ਨਿਰਾ)
	g_hVimIcon = ਕੱਢੋ_Icon(ਨਿਰਾ, (LPCSTR)exe_name, 0);
    ਜੇ (g_hVimIcon != ਨਿਰਾ)
	g_fCanChangeIcon = TRUE;
}

ਸਥਿਰ ਪੂਰਨ g_fWindInitCalled = FALSE;
ਸਥਿਰ ਪੂਰਨ g_f_Termcap_ਬਹੁਲਕ = FALSE;
ਸਥਿਰ CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    ਸਥਿਰ ਖਾਲੀ
mch_init_c(ਖਾਲੀ)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_ਬਫਰ_INFO csbi;
# endif
# ifndef __MINGW32__
    ਬਾਹਰੀ ਪੂਰਨ _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    ਸੈੱਟ_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    ਸੈੱਟ_Error_ਬਹੁਲਕ(SEM_FAILCRITICALERRORS);

    _fmode = O_ਦੁਆਧਾਰੀ;		// we do our own CR-LF translation
    ਬਾਹਰਲਾ_flush();

    // Obtain handles for the standard Console I/O devices
    ਜੇ (read_cmd_fd == 0)
	g_h_Con_ਵਿੱਚ =  GetStdHandle(STD_ਨਿਵੇਸ਼_HANDLE);
    ਨਹੀਂਤਾਂ
	create_conin();
    g_h_Con_ਬਾਹਰਲਾ = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    Save_Console_ਬਫਰ(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.w_ਗੁਣ;
# ਨਹੀਂਤਾਂ
    // Get current text attributes
    Get_Console_Screen_ਬਫਰ_Info(g_h_Con_ਬਾਹਰਲਾ, &csbi);
    g_attrCurrent = g_attrDefault = csbi.w_ਗੁਣ;
# endif
    ਜੇ (cterm_ਸਾਧਾਰਨ_fg_color == 0)
	cterm_ਸਾਧਾਰਨ_fg_color = (g_attrCurrent & 0xf) + 1;
    ਜੇ (cterm_ਸਾਧਾਰਨ_bg_color == 0)
	cterm_ਸਾਧਾਰਨ_bg_color = ((g_attrCurrent >> 4) & 0xf) + 1;

    // Fg and Bg color index number at startup
    g_color_ਸੂਚਕਾਂਕ_fg = g_attrDefault & 0xf;
    g_color_ਸੂਚਕਾਂਕ_bg = (g_attrDefault >> 4) & 0xf;

    // set termcap codes to current text attributes
    update_tcap(g_attrCurrent);

    GetConsoleCursorInfo(g_h_Con_ਬਾਹਰਲਾ, &g_cci);
    Get_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ,  &g_cmodein);
    Get_Console_ਬਹੁਲਕ(g_h_Con_ਬਾਹਰਲਾ, &g_cmodeout);

    SaveConsoleTitleAndIcon();
    /*
     * Set both the small and big icons of the console window to Vim's icon.
     * Note that Vim presently only has one size of icon (32x32), but it
     * automatically gets scaled down to 16x16 when setting the small icon.
     */
    ਜੇ (g_fCanChangeIcon)
	ਸੈੱਟ_Console_Icon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    ਜੇ (use_alternate_screen_ਬਫਰ)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    ਜੇ (fdDump)
    {
	time_t t;

	ਸਮਾਂ(&t);
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
    ਸਥਿਰ ਖਾਲੀ
mch_exit_c(ਪੂਰਨ r)
{
    // Copy flag since stoptermcap() will clear the flag.
    ਪੂਰਨ f_Termcap_ਬਹੁਲਕ = g_f_Termcap_ਬਹੁਲਕ;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    ਜੇ (!f_Termcap_ਬਹੁਲਕ &&  use_alternate_screen_ਬਫਰ)
	vtp_printf("\033[?1049l");

    ਜੇ (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    ਜੇ (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	ਜੇ (g_fCanChangeIcon && !g_fForceExit)
	    ਸੈੱਟ_Console_Icon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	ਜੇ (fdDump)
	{
	    time_t t;

	    ਸਮਾਂ(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = ਨਿਰਾ;
# endif
    }

    ਸੈੱਟ_Console_Cursor_Info(g_h_Con_ਬਾਹਰਲਾ, &g_cci);
    ਸੈੱਟ_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ,  g_cmodein | ENABLE_EXTENDED_FLAGS);
    ਸੈੱਟ_Console_ਬਹੁਲਕ(g_h_Con_ਬਾਹਰਲਾ, g_cmodeout);

# ifdef DYNAMIC_GETTEXT
    dyn_libintl_ਸਿਰਾ();
# endif

    ਬਾਹਰ_ਨਿਕਲੋ(r);
}
#ਜੇ_ਅੰਤ // !FEAT_GUI_MSWIN || VIMDLL

    ਖਾਲੀ
mch_init(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VIMDLL
    ਜੇ (gui.starting)
	mch_init_g();
    ਨਹੀਂਤਾਂ
	mch_init_c();
#ਜਾਂਜੇ defined(FEAT_GUI_MSWIN)
    mch_init_g();
#ਨਹੀਂਤਾਂ
    mch_init_c();
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
mch_exit(ਪੂਰਨ r)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VIMDLL
    ਜੇ (vimrun_ਰਸਤਾ_allocated)
	vim_free(vimrun_ਰਸਤਾ.string);

    ਜੇ (gui.ਵਿੱਚ_use || gui.starting)
	mch_exit_g(r);
    ਨਹੀਂਤਾਂ
	mch_exit_c(r);
#ਜਾਂਜੇ defined(FEAT_GUI_MSWIN)
    ਜੇ (vimrun_ਰਸਤਾ_allocated)
	vim_free(vimrun_ਰਸਤਾ.string);
    mch_exit_g(r);
#ਨਹੀਂਤਾਂ
    mch_exit_c(r);
#ਜੇ_ਅੰਤ
}

/*
 * Do we have an interactive window?
 */
    ਪੂਰਨ
mch_check_win(
    ਪੂਰਨ argc ਅਣਵਰਤਿਆ,
    ਅੱਖਰ **argv ਅਣਵਰਤਿਆ)
{
    mch_get_exe_name();

#ਜੇ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    ਵਾਪਸ OK;	    // GUI always has a tty
#ਨਹੀਂਤਾਂ
# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ OK;
# endif
    ਜੇ (isatty(1))
	ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    ਖਾਲੀ
fname_case(
    char_u	*name,
    ਪੂਰਨ		len)
{
    WCHAR   *p;
    WCHAR   buf[MAX_ਰਸਤਾ_2 + 1];

    ਜੇ (*name == NUL)
	ਵਾਪਸ;

    slash_adjust(name);

    p = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (Get_Long_ਰਸਤਾ_Name_W(p, buf, MAX_ਰਸਤਾ_2))
    {
	char_u	*q = utf16_ਮੰਜ਼ਿਲ_enc(buf, ਨਿਰਾ);

	ਜੇ (q != ਨਿਰਾ)
	{
	    ਜੇ (len > 0)
		vim_strncpy(name, q, len - 1);
	    ਨਹੀਂਤਾਂ
	    {
		size_t  namelen = STRLEN(name);

		ਜੇ (namelen >= STRLEN(q))
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
    ਪੂਰਨ
mch_get_user_name(
    char_u  *s,
    ਪੂਰਨ	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_ਲੰਮਾਈ(wszUserName);

    ਜੇ (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_ਮੰਜ਼ਿਲ_enc(wszUserName, ਨਿਰਾ);

	ਜੇ (p != ਨਿਰਾ)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    ਵਾਪਸ OK;
	}
    }
    s[0] = NUL;
    ਵਾਪਸ FAIL;
}


/*
 * Insert host name in s[len].
 */
    ਖਾਲੀ
mch_get_host_name(
    char_u	*s,
    ਪੂਰਨ		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_ਲੰਮਾਈ(wszHostName);

    ਜੇ (!GetComputerNameW(wszHostName, &wcch))
	ਵਾਪਸ;

    char_u  *p = utf16_ਮੰਜ਼ਿਲ_enc(wszHostName, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    ਲੰਮਾ
mch_get_pid(ਖਾਲੀ)
{
    ਵਾਪਸ (ਲੰਮਾ)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    ਪੂਰਨ
mch_process_running(ਲੰਮਾ pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   ਸਥਿਤੀ = 0;
    ਪੂਰਨ	    ret = FALSE;

    ਜੇ (hProcess == ਨਿਰਾ)
	ਵਾਪਸ FALSE;  // might not have access
    ਜੇ (Get_Exit_ਕੋਡ_Process(hProcess, &ਸਥਿਤੀ))
	ret = ਸਥਿਤੀ == STILL_ਸਰਗਰਮ;
    CloseHandle(hProcess);
    ਵਾਪਸ ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    ਪੂਰਨ
mch_dirname(
    char_u	*buf,
    ਪੂਰਨ		len)
{
    WCHAR   wbuf[MAX_ਰਸਤਾ_2 + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    ਜੇ (GetCurrentDirectoryW(MAX_ਰਸਤਾ_2, wbuf) == 0)
	ਵਾਪਸ FAIL;

    WCHAR   wcbuf[MAX_ਰਸਤਾ_2 + 1];
    char_u  *p = ਨਿਰਾ;

    ਜੇ (Get_Long_ਰਸਤਾ_Name_W(wbuf, wcbuf, MAX_ਰਸਤਾ_2) != 0)
    {
	p = utf16_ਮੰਜ਼ਿਲ_enc(wcbuf, ਨਿਰਾ);
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਜੇ (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    ਜੇ (p == ਨਿਰਾ)
	p = utf16_ਮੰਜ਼ਿਲ_enc(wbuf, ਨਿਰਾ);

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    ਵਾਪਸ OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    ਲੰਮਾ
mch_getperm(char_u *name)
{
    stat_T	st;

    ਵਾਪਸ (mch_stat((ਅੱਖਰ *)name, &st) == 0)
	? (ਲੰਮਾ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)st.st_ਬਹੁਲਕ
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
mch_setperm(char_u *name, ਲੰਮਾ perm)
{
    ਲੰਮਾ	n;
    WCHAR	*p;

    p = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    ਜੇ (n == -1)
	ਵਾਪਸ FAIL;

    win32_ਸੈੱਟ_archive(name);

    ਵਾਪਸ OK;
}

/*
 * Set hidden flag for "name".
 */
    ਖਾਲੀ
mch_hide(char_u *name)
{
    ਪੂਰਨ attrs = win32_getattrs(name);
    ਜੇ (attrs == -1)
	ਵਾਪਸ;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    ਪੂਰਨ
mch_ishidden(char_u *name)
{
    ਪੂਰਨ f = win32_getattrs(name);

    ਜੇ (f == -1)
	ਵਾਪਸ FALSE;		    // file does not exist at all

    ਵਾਪਸ (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    ਪੂਰਨ
mch_isdir(char_u *name)
{
    ਪੂਰਨ f = win32_getattrs(name);

    ਜੇ (f == -1)
	ਵਾਪਸ FALSE;		    // file does not exist at all

    ਵਾਪਸ (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    ਪੂਰਨ
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    attr = Get_File_ਗੁਣ_W(p);
    ਜੇ (attr == INVALID_FILE_ਗੁਣ)
	error = Get_ਆਖਰੀ_Error();
    vim_free(p);

    ਜੇ (attr == INVALID_FILE_ਗੁਣ)
	ਵਾਪਸ error == ERROR_FILE_NOT_ਮਿਲਿਆ || error == ERROR_ਰਸਤਾ_NOT_ਮਿਲਿਆ;
    ਵਾਪਸ (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ਪੂਰਨ
mch_isrealdir(char_u *name)
{
    ਵਾਪਸ mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    ਪੂਰਨ
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    ਪੂਰਨ	    retval;

    p = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ -1;
    retval = _wmkdir(p);
    vim_free(p);
    ਵਾਪਸ retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    ਪੂਰਨ
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    ਪੂਰਨ	    retval;

    p = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ -1;
    retval = _wrmdir(p);
    vim_free(p);
    ਵਾਪਸ retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    ਪੂਰਨ
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    ਵਾਪਸ win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_ਸੰਖਿਆ_ਦਾ_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    ਪੂਰਨ
mch_is_symbolic_link(char_u *name)
{
    HANDLE		h_ਲੱਭੋ;
    ਪੂਰਨ			res = FALSE;
    DWORD		fileFlags = 0, reparse_ਟੈਗ = 0;
    WCHAR		*wn;
    WIN32_ਲੱਭੋ_DATAW	ਲੱਭੋ_ਅੰਕੜੇ_W;

    wn = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (wn == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    h_ਲੱਭੋ = ਲੱਭੋ_ਪਹਿਲਾ_File_W(wn, &ਲੱਭੋ_ਅੰਕੜੇ_W);
    vim_free(wn);
    ਜੇ (h_ਲੱਭੋ != INVALID_HANDLE_ਮੁੱਲ)
    {
	fileFlags = ਲੱਭੋ_ਅੰਕੜੇ_W.dw_File_ਗੁਣ;
	reparse_ਟੈਗ = ਲੱਭੋ_ਅੰਕੜੇ_W.dwReserved0;
	ਲੱਭੋ_Close(h_ਲੱਭੋ);
    }

    ਜੇ ((fileFlags & FILE_ATTRIBUTE_REPARSE_ਬਿੰਦੂ)
	    && (reparse_ਟੈਗ == IO_REPARSE_ਟੈਗ_SYMLINK
		|| reparse_ਟੈਗ == IO_REPARSE_ਟੈਗ_MOUNT_ਬਿੰਦੂ))
	res = TRUE;

    ਵਾਪਸ res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    ਪੂਰਨ
mch_is_linked(char_u *fname)
{
    ਜੇ (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    ਪੂਰਨ
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    ਪੂਰਨ		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_ਮੰਜ਼ਿਲ_utf16(fname, ਨਿਰਾ);
    ਜੇ (wn == ਨਿਰਾ)
	ਵਾਪਸ FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    ਨਿਰਾ,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    ਨਿਰਾ);		// handle to template file
    vim_free(wn);

    ਜੇ (hFile == INVALID_HANDLE_ਮੁੱਲ)
	ਵਾਪਸ FILEINFO_READ_FAIL;

    ਜੇ (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    ਨਹੀਂਤਾਂ
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    ਵਾਪਸ res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    ਸਥਿਰ ਪੂਰਨ
win32_getattrs(char_u *name)
{
    ਪੂਰਨ		attr;
    WCHAR	*p;

    p = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ INVALID_FILE_ਗੁਣ;

    attr = Get_File_ਗੁਣ_W(p);
    vim_free(p);

    ਵਾਪਸ attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    ਸਥਿਰ ਪੂਰਨ
win32_setattrs(char_u *name, ਪੂਰਨ attrs)
{
    ਪੂਰਨ	    res;
    WCHAR   *p;

    p = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ -1;

    res = ਸੈੱਟ_File_ਗੁਣ_W(p, attrs);
    vim_free(p);

    ਵਾਪਸ res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    ਸਥਿਰ ਪੂਰਨ
win32_ਸੈੱਟ_archive(char_u *name)
{
    ਪੂਰਨ attrs = win32_getattrs(name);
    ਜੇ (attrs == -1)
	ਵਾਪਸ -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    ਵਾਪਸ win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    ਪੂਰਨ
mch_writable(char_u *name)
{
    ਪੂਰਨ attrs = win32_getattrs(name);

    ਵਾਪਸ (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    ਪੂਰਨ
mch_can_exe(char_u *name, char_u **ਰਸਤਾ, ਪੂਰਨ use_ਰਸਤਾ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ executable_exists((ਅੱਖਰ *)name, STRLEN(name), ਰਸਤਾ, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ਪੂਰਨ
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    ਪੂਰਨ		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    ਜੇ (STRNCMP(name, "\\\\.\\", 4) == 0)
	ਵਾਪਸ ਗੰਢ_WRITABLE;

    wn = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (wn == ਨਿਰਾ)
	ਵਾਪਸ ਗੰਢ_ਸਾਧਾਰਨ;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    ਨਿਰਾ,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    ਨਿਰਾ);		    // handle to template file
    vim_free(wn);
    ਜੇ (hFile == INVALID_HANDLE_ਮੁੱਲ)
	ਵਾਪਸ ਗੰਢ_ਸਾਧਾਰਨ;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    ਜੇ (type == FILE_TYPE_CHAR)
	ਵਾਪਸ ਗੰਢ_WRITABLE;
    ਜੇ (type == FILE_TYPE_DISK)
	ਵਾਪਸ ਗੰਢ_ਸਾਧਾਰਨ;
    ਵਾਪਸ ਗੰਢ_OTHER;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
ਬਣਤਰ my_acl
{
    PSECURITY_DESCRIPTOR    pSecurityDescriptor;
    PSID		    pSidOwner;
    PSID		    p_Sid_ਸਮੂਹ;
    PACL		    pDacl;
    PACL		    pSacl;
};
#ਜੇ_ਅੰਤ

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
    ਵਾਪਸ (vim_acl_T)ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
    ਬਣਤਰ my_acl   *p = ਨਿਰਾ;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(ਬਣਤਰ my_acl);
    ਜੇ (p != ਨਿਰਾ)
    {
	WCHAR	*wn;

	wn = enc_ਮੰਜ਼ਿਲ_utf16(fname, ਨਿਰਾ);
	ਜੇ (wn == ਨਿਰਾ)
	{
	    vim_free(p);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	// Try to retrieve the entire security descriptor.
	err = GetNamedSecurityInfoW(
		wn,			// Abstract filename
		SE_FILE_OBJECT,		// File Object
		OWNER_SECURITY_INFORMATION |
		ਸਮੂਹ_SECURITY_INFORMATION |
		DACL_SECURITY_INFORMATION |
		SACL_SECURITY_INFORMATION,
		&p->pSidOwner,		// Ownership information.
		&p->p_Sid_ਸਮੂਹ,		// Group membership.
		&p->pDacl,		// Discretionary information.
		&p->pSacl,		// For auditing purposes.
		&p->pSecurityDescriptor);
	ਜੇ (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (ਖਾਲੀ)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    ਨਿਰਾ,
		    ਨਿਰਾ,
		    &p->pDacl,
		    ਨਿਰਾ,
		    &p->pSecurityDescriptor);
	}
	ਜੇ (p->pSecurityDescriptor == ਨਿਰਾ)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = ਨਿਰਾ;
	}
	vim_free(wn);
    }

    ਵਾਪਸ (vim_acl_T)p;
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    ਸਥਿਰ BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_ਮਾਪ_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_ਗਿਣਤੀ = 0;
    GetAclInformation(acl, &acl_info, ਆਕਾਰ(acl_info), Acl_ਮਾਪ_Information);
    ਲਈ (i = 0; i < acl_info.Ace_ਗਿਣਤੀ; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	ਜੇ (ace->ਸਿਰਲੇਖ_2.AceFlags & INHERITED_ACE)
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    ਖਾਲੀ
mch_ਸੈੱਟ_acl(char_u *fname, vim_acl_T acl)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
    ਬਣਤਰ my_acl   *p = (ਬਣਤਰ my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    wn = enc_ਮੰਜ਼ਿਲ_utf16(fname, ਨਿਰਾ);
    ਜੇ (wn == ਨਿਰਾ)
	ਵਾਪਸ;

    // Set security flags
    ਜੇ (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    ਜੇ (p->p_Sid_ਸਮੂਹ)
	sec_info |= ਸਮੂਹ_SECURITY_INFORMATION;
    ਜੇ (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	ਜੇ (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    ਜੇ (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (ਖਾਲੀ)ਸੈੱਟ_Named_Security_Info_W(
	    wn,			// Abstract filename
	    SE_FILE_OBJECT,	// File Object
	    sec_info,
	    p->pSidOwner,	// Ownership information.
	    p->p_Sid_ਸਮੂਹ,	// Group membership.
	    p->pDacl,		// Discretionary information.
	    p->pSacl		// For auditing purposes.
	    );
    vim_free(wn);
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
mch_free_acl(vim_acl_T acl)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
    ਬਣਤਰ my_acl   *p = (ਬਣਤਰ my_acl *)acl;

    ਜੇ (p != ਨਿਰਾ)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#ਜੇ_ਅੰਤ
}

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    ਸਥਿਰ BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    ਨਿਵੇਸ਼_ਰਿਕਾਰਡ ir;
    DWORD ਬਾਹਰਲਾ;

    ਚੋਣ (dwCtrlType)
    {
    ਹਾਲਤ CTRL_C_EVENT:
	ਜੇ (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	ਵਾਪਸ TRUE;

    ਹਾਲਤ CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = ਕੁੰਜੀ_EVENT;
	ir.Event.ਕੁੰਜੀ_Event_2.b_ਕੁੰਜੀ_ਥੱਲੇ = TRUE;
	ir.Event.ਕੁੰਜੀ_Event_2.w_Repeat_ਗਿਣਤੀ = 1;
	ir.Event.ਕੁੰਜੀ_Event_2.w_Virtual_ਕੁੰਜੀ_ਕੋਡ = VK_CANCEL;
	ir.Event.ਕੁੰਜੀ_Event_2.w_Virtual_ਜਾਂਚ_ਕੋਡ = 0;
	ir.Event.ਕੁੰਜੀ_Event_2.dw_Control_ਕੁੰਜੀ_State = 0;
	ir.Event.ਕੁੰਜੀ_Event_2.uChar.UnicodeChar = 0;
	Write_Console_ਨਿਵੇਸ਼(g_h_Con_ਵਿੱਚ, &ir, 1, &ਬਾਹਰਲਾ);
	ਵਾਪਸ TRUE;

    // fatal events: shut down gracefully
    ਹਾਲਤ CTRL_CLOSE_EVENT:
    ਹਾਲਤ CTRL_LOGOFF_EVENT:
    ਹਾਲਤ CTRL_SHUTDOWN_EVENT:
	windgoto((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, cmdline_ਖਾਨਾ_off);
	g_fForceExit = TRUE;

	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	ਵਾਪਸ TRUE;		// not reached

    ਮੂਲ:
	ਵਾਪਸ FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    ਖਾਲੀ
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ;
# endif
    Get_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ, &cmodein);
    Get_Console_ਬਹੁਲਕ(g_h_Con_ਬਾਹਰਲਾ, &cmodeout);
    ਜੇ (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_ਸਤਰ_ਨਿਵੇਸ਼ | ENABLE_PROCESSED_ਨਿਵੇਸ਼ |
		     ENABLE_ECHO_ਨਿਵੇਸ਼);
	ਜੇ (g_f_Mouse_ਸਰਗਰਮ)
	{
	    cmodein |= ENABLE_MOUSE_ਨਿਵੇਸ਼;
	    cmodein &= ~ENABLE_ਤੇਜ਼_EDIT_ਬਹੁਲਕ;
	}
	ਨਹੀਂਤਾਂ
	{
	    cmodein |= g_cmodein & ENABLE_ਤੇਜ਼_EDIT_ਬਹੁਲਕ;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# ਨਹੀਂਤਾਂ
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    ਨਹੀਂਤਾਂ // cooked
    {
	cmodein |= (ENABLE_ਸਤਰ_ਨਿਵੇਸ਼ | ENABLE_PROCESSED_ਨਿਵੇਸ਼ |
		    ENABLE_ECHO_ਨਿਵੇਸ਼);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    ਸੈੱਟ_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ, cmodein | ENABLE_EXTENDED_FLAGS);
    ਸੈੱਟ_Console_ਬਹੁਲਕ(g_h_Con_ਬਾਹਰਲਾ, cmodeout);
    ਸੈੱਟ_Console_Ctrl_Handler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    ਜੇ (fdDump)
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
    ਪੂਰਨ
mch_get_shellsize(ਖਾਲੀ)
{
    CONSOLE_SCREEN_ਬਫਰ_INFO csbi;

# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ OK;
# endif
    ਜੇ (!g_f_Termcap_ਬਹੁਲਕ && g_cbTermcap.IsValid)
    {
	/*
	 * For some reason, we are trying to get the screen dimensions
	 * even though we are not in termcap mode.  The 'Rows' and 'Columns'
	 * variables are really intended to mean the size of Vim screen
	 * while in termcap mode.
	 */
	ਪੰਕਤੀਆਂ = g_cbTermcap.Info.dw_ਮਾਪ.Y;
	Columns = g_cbTermcap.Info.dw_ਮਾਪ.X;
    }
    ਨਹੀਂਤਾਂ ਜੇ (Get_Console_Screen_ਬਫਰ_Info(g_h_Con_ਬਾਹਰਲਾ, &csbi))
    {
	ਪੰਕਤੀਆਂ = csbi.sr_ਖਿੜਕੀ.Bottom - csbi.sr_ਖਿੜਕੀ.ਸਿਖਰ_2 + 1;
	Columns = csbi.sr_ਖਿੜਕੀ.ਸੱਜਾ_3 - csbi.sr_ਖਿੜਕੀ.ਖੱਬਾ_3 + 1;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੰਕਤੀਆਂ = 25;
	Columns = 80;
    }
    ਵਾਪਸ OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    ਸਥਿਰ ਖਾਲੀ
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    ਜੇ (use_alternate_screen_ਬਫਰ)
	ਵਾਪਸ;

    ਜੇ (!ਸੈੱਟ_Console_Screen_ਬਫਰ_ਮਾਪ(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	ਜੇ (fdDump)
	{
	    fprintf(fdDump, "SetConsoleScreenBufferSize failed: %lx\n",
		    Get_ਆਖਰੀ_Error());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Resize console window size to 'srWindowRect'
 */
    ਸਥਿਰ ਖਾਲੀ
Resize_ਖਿੜਕੀ(
    HANDLE     hConsole,
    SMALL_RECT sr_ਖਿੜਕੀ_Rect)
{
    ਜੇ (!ਸੈੱਟ_Console_ਖਿੜਕੀ_Info(hConsole, TRUE, &sr_ਖਿੜਕੀ_Rect))
    {
# ifdef MCH_WRITE_DUMP
	ਜੇ (fdDump)
	{
	    fprintf(fdDump, "SetConsoleWindowInfo failed: %lx\n",
		    Get_ਆਖਰੀ_Error());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Set a console window to `xSize' * `ySize'
 */
    ਸਥਿਰ ਖਾਲੀ
Resize_Con_Buf_And_ਖਿੜਕੀ(
    HANDLE  hConsole,
    ਪੂਰਨ	    x_ਮਾਪ,
    ਪੂਰਨ	    y_ਮਾਪ_2)
{
    CONSOLE_SCREEN_ਬਫਰ_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    sr_ਖਿੜਕੀ_Rect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    ਸਥਿਰ ਪੂਰਨ	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    ਜੇ (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", x_ਮਾਪ, y_ਮਾਪ_2);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = Get_ਸਭ_ਤੋਂ_ਵੱਡਾ_Console_ਖਿੜਕੀ_ਮਾਪ(hConsole);

    // define the new console window size and scroll position
    sr_ਖਿੜਕੀ_Rect.ਖੱਬਾ_3 = sr_ਖਿੜਕੀ_Rect.ਸਿਖਰ_2 = (SHORT) 0;
    sr_ਖਿੜਕੀ_Rect.ਸੱਜਾ_3 =  (SHORT) (min(x_ਮਾਪ, coordScreen.X) - 1);
    sr_ਖਿੜਕੀ_Rect.Bottom = (SHORT) (min(y_ਮਾਪ_2, coordScreen.Y) - 1);

    ਜੇ (Get_Console_Screen_ਬਫਰ_Info(g_h_Con_ਬਾਹਰਲਾ, &csbi))
    {
	ਪੂਰਨ sx, sy;

	sx = csbi.sr_ਖਿੜਕੀ.ਸੱਜਾ_3 - csbi.sr_ਖਿੜਕੀ.ਖੱਬਾ_3 + 1;
	sy = csbi.sr_ਖਿੜਕੀ.Bottom - csbi.sr_ਖਿੜਕੀ.ਸਿਖਰ_2 + 1;
	ਜੇ (sy < y_ਮਾਪ_2 || sx < x_ਮਾਪ)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    ਜੇ (sy < y_ਮਾਪ_2)
		coordScreen.Y = y_ਮਾਪ_2;
	    ਨਹੀਂਤਾਂ
		coordScreen.Y = sy;
	    ਜੇ (sx < x_ਮਾਪ)
		coordScreen.X = x_ਮਾਪ;
	    ਨਹੀਂਤਾਂ
		coordScreen.X = sx;
	    ਸੈੱਟ_Console_Screen_ਬਫਰ_ਮਾਪ(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = x_ਮਾਪ;
    coordScreen.Y = y_ਮਾਪ_2;

    // In the new console call API, only the first time in reverse order
    ਜੇ (!vtp_working || resized)
    {
	Resize_ਖਿੜਕੀ(hConsole, sr_ਖਿੜਕੀ_Rect);
	ResizeConBuf(hConsole, coordScreen);
    }
    ਨਹੀਂਤਾਂ
    {
	// Workaround for a Windows 10 bug
	cursor.X = sr_ਖਿੜਕੀ_Rect.ਖੱਬਾ_3;
	cursor.Y = sr_ਖਿੜਕੀ_Rect.ਸਿਖਰ_2;
	ਸੈੱਟ_Console_Cursor_Position(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	Resize_ਖਿੜਕੀ(hConsole, sr_ਖਿੜਕੀ_Rect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    ਖਾਲੀ
mch_ਸੈੱਟ_shellsize(ਖਾਲੀ)
{
    COORD coordScreen;

# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ;
# endif
    // Don't change window size while still starting up
    ਜੇ (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	ਵਾਪਸ;
    }

    ਜੇ (ਪਦ_console)
    {
	coordScreen = Get_ਸਭ_ਤੋਂ_ਵੱਡਾ_Console_ਖਿੜਕੀ_ਮਾਪ(g_h_Con_ਬਾਹਰਲਾ);

	// Clamp Rows and Columns to reasonable values
	ਜੇ (ਪੰਕਤੀਆਂ > coordScreen.Y)
	    ਪੰਕਤੀਆਂ = coordScreen.Y;
	ਜੇ (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	Resize_Con_Buf_And_ਖਿੜਕੀ(g_h_Con_ਬਾਹਰਲਾ, Columns, ਪੰਕਤੀਆਂ);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    ਖਾਲੀ
mch_new_shellsize(ਖਾਲੀ)
{
# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ;
# endif
    ਸੈੱਟ_scroll_region(0, 0, Columns - 1, ਪੰਕਤੀਆਂ - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    ਖਾਲੀ
mch_ਸੈੱਟ_winsize_now(ਖਾਲੀ)
{
    ਜੇ (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_ਸੈੱਟ_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# ਜੇ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    ਖਾਲੀ
mch_calc_cell_ਮਾਪ(ਬਣਤਰ cellsize *cs_ਬਾਹਰਲਾ)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_ਬਾਹਰਲਾ->cs_xpixel = -1;
    cs_ਬਾਹਰਲਾ->cs_ypixel = -1;

    ਜੇ (g_h_Con_ਬਾਹਰਲਾ == INVALID_HANDLE_ਮੁੱਲ)
	ਵਾਪਸ;

    cfi.cb_ਮਾਪ = ਆਕਾਰ(cfi);
    ਜੇ (GetCurrentConsoleFontEx(g_h_Con_ਬਾਹਰਲਾ, FALSE, &cfi)
	    && cfi.dw_Font_ਮਾਪ.X > 0 && cfi.dw_Font_ਮਾਪ.Y > 0)
    {
	cs_ਬਾਹਰਲਾ->cs_xpixel = cfi.dw_Font_ਮਾਪ.X;
	cs_ਬਾਹਰਲਾ->cs_ypixel = cfi.dw_Font_ਮਾਪ.Y;
	ਵਾਪਸ;
    }

    Get_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_ਸਤਰ_ਨਿਵੇਸ਼ | ENABLE_ECHO_ਨਿਵੇਸ਼
	    | ENABLE_PROCESSED_ਨਿਵੇਸ਼);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_ਨਿਵੇਸ਼;

    ਜੇ (ਸੈੱਟ_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	ਸੈੱਟ_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ, cmodeold);
    }
}
# endif

#ਜੇ_ਅੰਤ

    ਸਥਿਰ BOOL
vim_create_process(
    ਅੱਖਰ		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    ਅੱਖਰ		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = ਨਿਰਾ;

    wcmd = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)cmd, ਨਿਰਾ);
    ਜੇ (wcmd == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਜੇ (cwd != ਨਿਰਾ)
    {
	wcwd = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)cwd, ਨਿਰਾ);
	ਜੇ (wcwd == ਨਿਰਾ)
	    ਜਾਓ theend;
    }

    ret = CreateProcessW(
	    ਨਿਰਾ,		// Executable name
	    wcmd,		// Command to execute
	    ਨਿਰਾ,		// Process security attributes
	    ਨਿਰਾ,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    ਵਾਪਸ ret;
}


    ਸਥਿਰ HINSTANCE
vim_shell_execute(
    ਅੱਖਰ *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)cmd, ਨਿਰਾ);
    ਜੇ (wcmd == ਨਿਰਾ)
	ਵਾਪਸ (HINSTANCE) 0;

    ret = ShellExecuteW(ਨਿਰਾ, ਨਿਰਾ, wcmd, ਨਿਰਾ, ਨਿਰਾ, n_show_cmd);
    vim_free(wcmd);
    ਵਾਪਸ ret;
}


#ਜੇ defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    ਸਥਿਰ ਪੂਰਨ
mch_system_classic(ਅੱਖਰ *cmd, ਪੂਰਨ options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = ਆਕਾਰ(si);
    si.lpReserved = ਨਿਰਾ;
    si.lpDesktop = ਨਿਰਾ;
    si.lpTitle = ਨਿਰਾ;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    ਜੇ (options & SHELL_DOOUT)
	si.w_Show_ਖਿੜਕੀ = SW_SHOWMINNOACTIVE;
    ਨਹੀਂਤਾਂ
	si.w_Show_ਖਿੜਕੀ = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = ਨਿਰਾ;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_ਬਹੁਲਕ |	CREATE_NEW_CONSOLE,
	    &si, &pi, ਨਿਰਾ, ਨਿਰਾ);

    // Wait for the command to terminate before continuing
    {
	ਪੂਰਨ	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	ਲਈ (;;)
	{
	    ਸੁਨੇਹਾ_3	ਸੁਨੇਹਾ;

	    ਜੇ (Peek_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ, (HWND)ਨਿਰਾ, 0, 0, PM_REMOVE))
	    {
		Translate_ਸੁਨੇਹਾ(&ਸੁਨੇਹਾ);
		Dispatch_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ);
		delay = 1;
		ਜਾਰੀ;
	    }
	    ਜੇ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		ਰੋਕੋ;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    ਜੇ (delay < 50)
		delay += 10;
	}

	// Get the command exit code
	Get_Exit_ਕੋਡ_Process(pi.hProcess, &ret);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // Try to get input focus back.  Doesn't always work though.
    Post_ਸੁਨੇਹਾ(hwnd, WM_SETFOCUS, 0, 0);
    // To increase the chances that WM_SETFOCUS will work, run the message loop
    // here.  In addition, it prevents problems caused by delayed WM_SETFOCUS
    // processing.
    gui_mch_update();

    ਵਾਪਸ ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr = param;
    linenr_T	    lnum = curbuf->b_op_ਸ਼ੁਰੂ.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    ਪੂਰਨ		    written = 0;

    ਲਈ (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	ਜੇ (l == 0)
	    len = 0;
	ਨਹੀਂਤਾਂ ਜੇ (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr, "", 1, &len, ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr, (ਅੱਖਰ *)lp + written,
		      s == ਨਿਰਾ ? l : (DWORD)(s - (lp + written)),
		      &len, ਨਿਰਾ);
	}
	ਜੇ (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    ਜੇ (lnum != curbuf->b_op_ਸਿਰਾ.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, ਨਿਰਾ);
	    }

	    ++lnum;
	    ਜੇ (lnum > curbuf->b_op_ਸਿਰਾ.lnum)
		ਰੋਕੋ;

	    lp = ml_get(lnum);
	    written = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr);
    ਵਾਪਸ 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    ਸਥਿਰ ਖਾਲੀ
dump_pipe(ਪੂਰਨ	    options,
	  HANDLE    g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Rd,
	  garray_T  *ga,
	  char_u    ਬਫਰ[],
	  DWORD	    *ਬਫਰ_off)
{
    DWORD	ਉਪਲਬਧ_ਬਾਈਟਾਂ = 0;
    DWORD	i;
    ਪੂਰਨ		ret;
    DWORD	len;
    DWORD	ਮੰਜ਼ਿਲ_Read;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Rd, // pipe to query
			ਨਿਰਾ,		    // optional buffer
			0,		    // buffer size
			ਨਿਰਾ,		    // number of read bytes
			&ਉਪਲਬਧ_ਬਾਈਟਾਂ,    // available bytes total
			ਨਿਰਾ);		    // byteLeft

    // We got real data in the pipe, read it
    ਜਦੋਂ (ret != 0 && ਉਪਲਬਧ_ਬਾਈਟਾਂ > 0)
    {
	ਮੰਜ਼ਿਲ_Read = (DWORD)(BUFLEN - *ਬਫਰ_off);
	ਮੰਜ਼ਿਲ_Read = ਉਪਲਬਧ_ਬਾਈਟਾਂ < ਮੰਜ਼ਿਲ_Read ? ਉਪਲਬਧ_ਬਾਈਟਾਂ : ਮੰਜ਼ਿਲ_Read;
	ReadFile(g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Rd, ਬਫਰ + *ਬਫਰ_off, ਮੰਜ਼ਿਲ_Read , &len, ਨਿਰਾ);

	// If we haven't read anything, there is a problem
	ਜੇ (len == 0)
	    ਰੋਕੋ;

	ਉਪਲਬਧ_ਬਾਈਟਾਂ -= len;

	ਜੇ (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    ਲਈ (i = 0; i < len; ++i)
	    {
		ਜੇ (ਬਫਰ[i] == NL)
		    append_ga_ਸਤਰ(ga);
		ਨਹੀਂਤਾਂ ਜੇ (ਬਫਰ[i] == NUL)
		    ga_append(ga, NL);
		ਨਹੀਂਤਾਂ
		    ga_append(ga, ਬਫਰ[i]);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	{
	    ਪੂਰਨ		l;
	    ਪੂਰਨ		c;
	    char_u	*p;

	    len += *ਬਫਰ_off;
	    ਬਫਰ[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    ਲਈ (p = ਬਫਰ; p < ਬਫਰ + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		ਜੇ (l == 0)
		    l = 1;  // NUL byte?
		ਨਹੀਂਤਾਂ ਜੇ (MB_BYTE2LEN(*p) != l)
		    ਰੋਕੋ;
	    }
	    ਜੇ (p == ਬਫਰ)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		ਜੇ (len >= 12)
		    ++p;
		ਨਹੀਂਤਾਂ
		{
		    *ਬਫਰ_off = len;
		    ਵਾਪਸ;
		}
	    }
	    c = *p;
	    *p = NUL;
	    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)ਬਫਰ);
	    ਜੇ (p < ਬਫਰ + len)
	    {
		*p = c;
		*ਬਫਰ_off = (DWORD)((ਬਫਰ + len) - p);
		mch_memmove(ਬਫਰ, p, *ਬਫਰ_off);
		ਵਾਪਸ;
	    }
	    *ਬਫਰ_off = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਬਫਰ[len] = NUL;
	    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)ਬਫਰ);
	}

	windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);
	cursor_on();
	ਬਾਹਰਲਾ_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    ਸਥਿਰ ਪੂਰਨ
mch_system_piped(ਅੱਖਰ *cmd, ਪੂਰਨ options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Rd = ਨਿਰਾ;
    HANDLE g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr = ਨਿਰਾ;
    HANDLE g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Rd = ਨਿਰਾ;
    HANDLE g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Wr = ਨਿਰਾ;

    char_u	ਬਫਰ[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    ਪੂਰਨ		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    ਪੂਰਨ		noread_cnt = 0;
    garray_T	ga;
    ਪੂਰਨ		delay = 1;
    DWORD	ਬਫਰ_off = 0;	// valid bytes in buffer[]
    ਅੱਖਰ	*p = ਨਿਰਾ;

    SECURITY_ਗੁਣ saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.n_ਲੰਮਾਈ = ਆਕਾਰ(SECURITY_ਗੁਣ);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ਨਿਰਾ;

    ਜੇ ( ! CreatePipe(&g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Rd, &g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Wr, &saAttr, 0)
	// Ensure the read handle to the pipe for STDOUT is not inherited.
       || ! ਸੈੱਟ_Handle_Information(g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Rd, HANDLE_FLAG_INHERIT, 0)
	// Create a pipe for the child process's STDIN.
       || ! CreatePipe(&g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Rd, &g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr, &saAttr, 0)
	// Ensure the write handle to the pipe for STDIN is not inherited.
       || ! ਸੈੱਟ_Handle_Information(g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr, HANDLE_FLAG_INHERIT, 0) )
    {
	CloseHandle(g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Rd);
	CloseHandle(g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr);
	CloseHandle(g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Rd);
	CloseHandle(g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Wr);
	ਸੁਨੇਹਾ_puts(_("\nCannot create pipes\n"));
    }

    si.cb = ਆਕਾਰ(si);
    si.lpReserved = ਨਿਰਾ;
    si.lpDesktop = ਨਿਰਾ;
    si.lpTitle = ਨਿਰਾ;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Wr;
    si.hStdOutput = g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Wr;
    si.h_Std_ਨਿਵੇਸ਼ = g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Rd;
    si.w_Show_ਖਿੜਕੀ = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = ਨਿਰਾ;

    ਜੇ (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    ਜੇ (cmd != ਨਿਰਾ)
    {
	p = (ਅੱਖਰ *)vim_strsave((char_u *)cmd);
	ਜੇ (p != ਨਿਰਾ)
	    unescape_shellxquote((char_u *)p, p_sxe);
	ਨਹੀਂਤਾਂ
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_ਬਹੁਲਕ,
						&si, &pi, ਨਿਰਾ, ਨਿਰਾ);

    ਜੇ (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Rd);
    CloseHandle(g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Wr);

    ਜੇ (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(ਨਿਰਾ,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr,  // parameter
			0,		 // creation flag, start immediately
			ਨਿਰਾ);		    // we don't care about thread id
	CloseHandle(thread);
	g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr = ਨਿਰਾ;
    }

    // Keep updating the window while waiting for the shell to finish.
    ਲਈ (;;)
    {
	ਸੁਨੇਹਾ_3	ਸੁਨੇਹਾ;

	ਜੇ (Peek_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ, (HWND)ਨਿਰਾ, 0, 0, PM_REMOVE))
	{
	    Translate_ਸੁਨੇਹਾ(&ਸੁਨੇਹਾ);
	    Dispatch_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ);
	}

	// write pipe information in the window
	ਜੇ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.ਵਿੱਚ_use
# endif
	    )
	{
	    len = 0;
	    ਜੇ (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.ਵਿੱਚ_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		ਜੇ (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		ਜੇ (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    ਜੇ (len == 1 && cmd != ਨਿਰਾ)
		    {
			ਜੇ (ta_buf[ta_len] == Ctrl_C)
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
		    ਜੇ ((len == 1 || len == 4 ) && cmd != ਨਿਰਾ)
		    {
			ਜੇ (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr);
			    g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr = ਨਿਰਾ;
			    len = 0;
			}
		    }

		    len = ਪਦ_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    ਲਈ (i = ta_len; i < ta_len + len; ++i)
		    {
			ਜੇ (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    ਸੁਨੇਹਾ_putchar(ta_buf[i]);
			ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
			{
			    ਪੂਰਨ l = (*mb_ptr2len)(ta_buf + i);

			    ਸੁਨੇਹਾ_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			ਨਹੀਂਤਾਂ
			    ਸੁਨੇਹਾ_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);
		    ਬਾਹਰਲਾ_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    ਜੇ (options & SHELL_WRITE)
			ta_len = 0;
		    ਨਹੀਂਤਾਂ ਜੇ (g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr != ਨਿਰਾ)
		    {
			WriteFile(g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr, (ਅੱਖਰ*)ta_buf,
				    1, &len, ਨਿਰਾ);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			ਜੇ (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	ਜੇ (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	ਜੇ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Rd, &ga, ਬਫਰ, &ਬਫਰ_off);
	    ਰੋਕੋ;
	}

	++noread_cnt;
	dump_pipe(options, g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Rd, &ga, ਬਫਰ, &ਬਫਰ_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	ਜੇ (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_h_ਬੱਚਾ_Std_ਬਾਹਰਲਾ_Rd);
    ਜੇ (g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr != ਨਿਰਾ)
	CloseHandle(g_h_ਬੱਚਾ_Std_ਵਿੱਚ_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    Get_Exit_ਕੋਡ_Process(pi.hProcess, &ret);

    ਜੇ (options & SHELL_READ)
    {
	ਜੇ (ga.ga_len > 0)
	{
	    append_ga_ਸਤਰ(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	ਨਹੀਂਤਾਂ
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    ਵਾਪਸ ret;
}

    ਸਥਿਰ ਪੂਰਨ
mch_system_g(ਅੱਖਰ *cmd, ਪੂਰਨ options)
{
    // if we can pipe and the shelltemp option is off
    ਜੇ (!p_stmp)
	ਵਾਪਸ mch_system_piped(cmd, options);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ mch_system_classic(cmd, options);
}
#ਜੇ_ਅੰਤ

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ਸਥਿਰ ਪੂਰਨ
mch_system_c(ਅੱਖਰ *cmd, ਪੂਰਨ options ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    ਜੇ (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	ਜੇ (buf == ਨਿਰਾ)
	    ਵਾਪਸ -1;
	vim_snprintf((ਅੱਖਰ *)buf, len, "(%s)", cmd);
	wcmd = enc_ਮੰਜ਼ਿਲ_utf16(buf, ਨਿਰਾ);
	ਮੁਕਤ(buf);
    }
    ਨਹੀਂਤਾਂ
	wcmd = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)cmd, ਨਿਰਾ);

    ਜੇ (wcmd == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    ਵਾਪਸ ret;
}

#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ
mch_system(ਅੱਖਰ *cmd, ਪੂਰਨ options)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use || gui.starting)
	ਵਾਪਸ mch_system_g(cmd, options);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ mch_system_c(cmd, options);
#ਜਾਂਜੇ defined(FEAT_GUI_MSWIN)
    ਵਾਪਸ mch_system_g(cmd, options);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ mch_system_c(cmd, options);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ਸਥਿਰ ਪੂਰਨ
mch_call_shell_terminal(
    char_u	*cmd,
    ਪੂਰਨ		options ਅਣਵਰਤਿਆ)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = ਨਿਰਾ;
    typval_T	argvar[2];
    long_u	cmdlen;
    ਪੂਰਨ		retval = -1;
    buf_T	*buf;
    ਕੰਮ_T	*ਕੰਮ;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    ਜੇ (cmd == ਨਿਰਾ)
	cmdlen = STRLEN(p_sh) + 1;
    ਨਹੀਂਤਾਂ
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    ਜੇ (newcmd == ਨਿਰਾ)
	ਵਾਪਸ 255;
    ਜੇ (cmd == ਨਿਰਾ)
    {
	STRCPY(newcmd, p_sh);
	ch_log(ਨਿਰਾ, "starting terminal to run a shell");
    }
    ਨਹੀਂਤਾਂ
    {
	vim_snprintf((ਅੱਖਰ *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(ਨਿਰਾ, "starting terminal for system command '%s'", cmd);
    }

    init_ਕੰਮ_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = ਪਦ_ਸ਼ੁਰੂ(argvar, ਨਿਰਾ, &opt, ਪਦ_ਸ਼ੁਰੂ_SYSTEM);
    ਜੇ (buf == ਨਿਰਾ)
    {
	vim_free(newcmd);
	ਵਾਪਸ 255;
    }

    ਕੰਮ = ਪਦ_getjob(buf->b_ਪਦ);
    ++ਕੰਮ->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    ਜੇ (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	ਜਦੋਂ (ਪਦ_use_loop())
	{
	    ਜੇ (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_ਸਰਗਰਮ)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		ਜੇ (terminal_loop(TRUE) == OK)
		    ਸਾਧਾਰਨ_cmd(&oa, TRUE);
	    }
	    ਨਹੀਂਤਾਂ
		ਸਾਧਾਰਨ_cmd(&oa, TRUE);
	}
	retval = ਕੰਮ->jv_exitval;
	ch_log(ਨਿਰਾ, "system command finished");

	ਕੰਮ_unref(ਕੰਮ);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_ਬਫਰ(DOBUF_WIPE, DOBUF_ਪਹਿਲਾ, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    ਵਾਪਸ retval;
}
#ਜੇ_ਅੰਤ
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    ਸਥਿਰ ਖਾਲੀ
restore_env_var(char_u *name, char_u *old_ਮੁੱਲ, ਪੂਰਨ must_free)
{
    ਜੇ (old_ਮੁੱਲ != ਨਿਰਾ)
    {
	vim_setenv(name, old_ਮੁੱਲ);
	ਜੇ (must_free)
	    vim_free(old_ਮੁੱਲ);
	ਵਾਪਸ;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    ਪੂਰਨ
mch_call_shell(
    char_u  *cmd,
    ਪੂਰਨ	    options)	// SHELL_*, see vim.h
{
    ਪੂਰਨ		x = 0;
    ਪੂਰਨ		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    ਪੂਰਨ		must_free;
    char_u	*oldval;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ch_log(ਨਿਰਾ, "executing shell command: %s", cmd);
#ਜੇ_ਅੰਤ
    // Change the title to reflect that we are in a subshell.
    ਜੇ (GetConsoleTitleW(szShellTitle, ARRAY_ਲੰਮਾਈ(szShellTitle) - 4) > 0)
    {
	ਜੇ (cmd == ਨਿਰਾ)
	    wcscat(szShellTitle, L" :sh");
	ਨਹੀਂਤਾਂ
	{
	    WCHAR *wn = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)cmd, ਨਿਰਾ);

	    ਜੇ (wn != ਨਿਰਾ)
	    {
		wcscat(szShellTitle, L" - !");
		ਜੇ ((wcslen(szShellTitle) + wcslen(wn) <
			    ARRAY_ਲੰਮਾਈ(szShellTitle)))
		    wcscat(szShellTitle, wn);
		ਸੈੱਟ_Console_Title_W(szShellTitle);
		vim_free(wn);
	    }
	}
    }
    // do not execute anything from the current directory by setting the
    // environment variable $NoDefaultCurrentDirectoryInExePath
    oldval = vim_getenv((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    &must_free);
    vim_setenv((char_u *)"NoDefaultCurrentDirectoryInExePath", (char_u *)"1");

    ਬਾਹਰਲਾ_flush();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MCH_WRITE_DUMP
    ਜੇ (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    ਜੇ (
# ifdef VIMDLL
	    gui.ਵਿੱਚ_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != ਨਿਰਾ
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	ਜੇ (cmdbase != ਨਿਰਾ)
	    // Skip a leading quote and (.
	    ਜਦੋਂ (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	ਜੇ (cmdbase == ਨਿਰਾ || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    ਵਾਪਸ x;
	}
    }
#ਜੇ_ਅੰਤ

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

    ਜੇ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    ਜੇ (cmd == ਨਿਰਾ)
    {
	x = mch_system((ਅੱਖਰ *)p_sh, options);
    }
    ਨਹੀਂਤਾਂ
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = ਨਿਰਾ;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	ਜੇ (*cmdbase == '"' )
	    ++cmdbase;
	ਜੇ (*cmdbase == '(')
	    ++cmdbase;

	ਜੇ ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    Zero_ਸਮ੍ਰਿਤੀ(&si, ਆਕਾਰ(si));
	    si.cb = ਆਕਾਰ(si);
	    si.lpReserved = ਨਿਰਾ;
	    si.lpDesktop = ਨਿਰਾ;
	    si.lpTitle = ਨਿਰਾ;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = ਨਿਰਾ;

	    cmdbase = skipwhite(cmdbase + 5);
	    ਜੇ ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.w_Show_ਖਿੜਕੀ = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_ਖਿੜਕੀ;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_ਨਿਵੇਸ਼ = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    ਨਿਰਾ,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_ਸਾਧਾਰਨ,		// File att.
		    ਨਿਰਾ);				// Temp file
		si.hStdOutput = si.h_Std_ਨਿਵੇਸ਼;
		si.hStdError = si.h_Std_ਨਿਵੇਸ਼;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    ਜੇ (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		ਜੇ (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		ਜੇ (p > cmdbase && p[-1] == ')'
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
	    ਜੇ (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		ਜੇ (cmd_shell == ਨਿਰਾ || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		ਜੇ (subcmd != ਨਿਰਾ)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    ਜੇ (newcmd != ਨਿਰਾ)
			vim_snprintf((ਅੱਖਰ *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    ਨਹੀਂਤਾਂ
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    ਜੇ (vim_create_process((ਅੱਖਰ *)newcmd, FALSE, flags,
			&si, &pi, ਨਿਰਾ, ਨਿਰਾ))
		x = 0;
	    ਨਹੀਂਤਾਂ ਜੇ (vim_shell_execute((ਅੱਖਰ *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    ਨਹੀਂਤਾਂ
	    {
		x = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
# ifdef VIMDLL
		ਜੇ (gui.ਵਿੱਚ_use)
# endif
		    emsg(_(e_command_not_ਮਿਲਿਆ));
#ਜੇ_ਅੰਤ
	    }

	    ਜੇ (newcmd != cmdbase)
		vim_free(newcmd);

	    ਜੇ (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_ਨਿਵੇਸ਼ != ਨਿਰਾ)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_ਨਿਵੇਸ਼);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	ਨਹੀਂਤਾਂ
	{
	    cmdlen =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
		((gui.ਵਿੱਚ_use || gui.starting) ?
		    (!s_dont_use_vimrun && p_stmp ?
			vimrun_ਰਸਤਾ.ਲੰਮਾਈ : STRLEN(p_sh) + STRLEN(p_shcf))
		    : 0) +
#ਜੇ_ਅੰਤ
		STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;

	    newcmd = alloc(cmdlen);
	    ਜੇ (newcmd != ਨਿਰਾ)
	    {
#ਜੇ defined(FEAT_GUI_MSWIN)
		ਜੇ (
# ifdef VIMDLL
		    (gui.ਵਿੱਚ_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    ਅੱਖਰ *ਸੁਨੇਹਾ = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    ਅੱਖਰ *title = _("Vim Warning");
		    WCHAR *wmsg = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)ਸੁਨੇਹਾ, ਨਿਰਾ);
		    WCHAR *wtitle = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)title, ਨਿਰਾ);

		    ਜੇ (wmsg != ਨਿਰਾ && wtitle != ਨਿਰਾ)
			ਸੁਨੇਹਾ_Box_W(ਨਿਰਾ, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		ਜੇ (
# ifdef VIMDLL
		    (gui.ਵਿੱਚ_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((ਅੱਖਰ *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_ਰਸਤਾ.string,
			    (ਸੁਨੇਹਾ_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		ਨਹੀਂਤਾਂ ਜੇ (
# ifdef VIMDLL
			(gui.ਵਿੱਚ_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((ਅੱਖਰ *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    vim_snprintf((ਅੱਖਰ *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((ਅੱਖਰ *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    ਜੇ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    ਜੇ (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#ਜੇ defined(FEAT_GUI_MSWIN)
	    && ((gui.ਵਿੱਚ_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#ਜੇ_ਅੰਤ
	    )
    {
	smsg(_("shell returned %d"), x);
	ਸੁਨੇਹਾ_putchar('\n');
    }
    resettitle();

    mch_signal(SIGINT, SIG_DFL);
    mch_signal(SIGBREAK, SIG_DFL);
    mch_signal(SIGILL, SIG_DFL);
    mch_signal(SIGFPE, SIG_DFL);
    mch_signal(SIGSEGV, SIG_DFL);
    mch_signal(SIGTERM, SIG_DFL);
    mch_signal(SIGABRT, SIG_DFL);

    ਵਾਪਸ x;
}

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
    ਸਥਿਰ HANDLE
ਕੰਮ_io_file_open(
	char_u *fname,
	DWORD dwDesiredAccess,
	DWORD dw_Share_ਬਹੁਲਕ,
	LPSECURITY_ਗੁਣ lp_Security_ਗੁਣ,
	DWORD dwCreationDisposition,
	DWORD dw_Flags_And_ਗੁਣ)
{
    HANDLE h;
    WCHAR *wn;

    wn = enc_ਮੰਜ਼ਿਲ_utf16(fname, ਨਿਰਾ);
    ਜੇ (wn == ਨਿਰਾ)
	ਵਾਪਸ INVALID_HANDLE_ਮੁੱਲ;

    h = CreateFileW(wn, dwDesiredAccess, dw_Share_ਬਹੁਲਕ,
	    lp_Security_ਗੁਣ, dwCreationDisposition,
	    dw_Flags_And_ਗੁਣ, ਨਿਰਾ);
    vim_free(wn);
    ਵਾਪਸ h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    ਖਾਲੀ
win32_build_env(dict_T *env, garray_T *gap, ਪੂਰਨ is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != ਨਿਰਾ ? env->dv_hashtab.ht_ਵਰਤਿਆ : 0;
    LPVOID	ਅਧਾਰ = Get_Environment_ਲੜੀਆਂ_W();

    // for last \0
    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ;

    ਜੇ (env != ਨਿਰਾ)
    {
	FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&env->dv_hashtab, hi, todo)
	{
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		typval_T *ਵਸਤੂ = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)hi->hi_ਕੁੰਜੀ, ਨਿਰਾ);
		WCHAR   *wval = enc_ਮੰਜ਼ਿਲ_utf16(tv_get_string(ਵਸਤੂ), ਨਿਰਾ);
		--todo;
		ਜੇ (wkey != ਨਿਰਾ && wval != ਨਿਰਾ)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    ਜੇ (ga_grow(gap, (ਪੂਰਨ)(lkey + lval + 2)) == FAIL)
			ਜਾਰੀ;
		    ਲਈ (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) = L'=';
		    ਲਈ (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    ਜੇ (ਅਧਾਰ)
    {
	WCHAR	*p = (WCHAR*) ਅਧਾਰ;

	// for last \0
	ਜੇ (ga_grow(gap, 1) == FAIL)
	    ਵਾਪਸ;

	ਜਦੋਂ (*p != 0 || *(p + 1) != 0)
	{
	    ਜੇ (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) = *p;
	    p++;
	}
	Free_Environment_ਲੜੀਆਂ_W(ਅਧਾਰ);
	*((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) = L'\0';
    }

# ਜੇ defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
    {
#  ifdef FEAT_CLIENTSERVER
	char_u	*servername = get_vim_var_str(VV_SEND_SERVER);
	size_t	servername_len = STRLEN(servername);
#  endif
#  ifdef FEAT_TERMINAL
	char_u	*ਸੰਸਕਰਣ_2 = get_vim_var_str(VV_ਸੰਸਕਰਣ);
	size_t	ਸੰਸਕਰਣ_len = STRLEN(ਸੰਸਕਰਣ_2);
#  endif
	// size of "VIM_SERVERNAME=" and value,
	// plus "VIM_TERMINAL=" and value,
	// plus two terminating NULs
	size_t	n = 0
#  ifdef FEAT_CLIENTSERVER
		    + 15 + servername_len
#  endif
#  ifdef FEAT_TERMINAL
		    + 13 + ਸੰਸਕਰਣ_len + 2
#  endif
		    ;

	ਜੇ (ga_grow(gap, (ਪੂਰਨ)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    ਲਈ (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    ਲਈ (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    ਜੇ (is_terminal)
	    {
		ਲਈ (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		ਲਈ (n = 0; n < ਸੰਸਕਰਣ_len; n++)
		    *((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) =
			(WCHAR)ਸੰਸਕਰਣ_2[n];
		*((WCHAR*)gap->ga_ਅੰਕੜੇ + gap->ga_len++) = L'\0';
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
    ਸਥਿਰ BOOL
create_pipe_pair(HANDLE handles[2])
{
    ਸਥਿਰ LONG		s;
    ਅੱਖਰ		name[64];
    SECURITY_ਗੁਣ sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_ਮਾਪ, 0, 0, ਨਿਰਾ);

    ਜੇ (handles[1] == INVALID_HANDLE_ਮੁੱਲ)
	ਵਾਪਸ FALSE;

    sa.n_ਲੰਮਾਈ = ਆਕਾਰ(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = ਨਿਰਾ;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_ਸਾਧਾਰਨ, 0);

    ਜੇ (handles[0] == INVALID_HANDLE_ਮੁੱਲ)
    {
	CloseHandle(handles[1]);
	ਵਾਪਸ FALSE;
    }

    ਵਾਪਸ TRUE;
}

#ਜੇ_ਅੰਤ // FEAT_JOB_CHANNEL

#ਜੇ defined(FEAT_EVAL)
/*
 * Execute "argv" directly without the shell and return the output.
 * Used by system() and systemlist() when the command is a List.
 * "infile" is an optional temp file for stdin input.
 * When "ret_len" is not NULL, set it to the length of the output.
 * Returns the output in allocated memory (or NULL on error).
 * Sets v:shell_error to the exit status.
 */
    char_u *
mch_get_cmd_output_ਸਿੱਧਾ(
    ਅੱਖਰ	**argv,
    char_u	*infile,
    ਪੂਰਨ		flags ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_ਗੁਣ saAttr;
    HANDLE		h_ਬੱਚਾ_Stdout_Rd = INVALID_HANDLE_ਮੁੱਲ;
    HANDLE		h_ਬੱਚਾ_Stdout_Wr = INVALID_HANDLE_ਮੁੱਲ;
    HANDLE		h_ਬੱਚਾ_Stdin_Rd = INVALID_HANDLE_ਮੁੱਲ;
    garray_T		cmd_ga;
    garray_T		ਬਾਹਰਲਾ_ga;
    char_u		*ਬਫਰ = ਨਿਰਾ;
    DWORD		exit_ਕੋਡ = (DWORD)-1;
    ਪੂਰਨ			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    ਲਈ (i = 0; argv[i] != ਨਿਰਾ; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	ਪੂਰਨ	has_spaces = FALSE;
	ਪੂਰਨ	j;

	ਲਈ (j = 0; s[j] != NUL; j++)
	    ਜੇ (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		ਰੋਕੋ;
	    }

	ਜੇ (i > 0)
	    ga_append(&cmd_ga, ' ');

	ਜੇ (has_spaces)
	{
	    ਪੂਰਨ	ਅੰਕ_bs;

	    ga_append(&cmd_ga, '"');
	    ਲਈ (j = 0; arg[j] != NUL; j++)
	    {
		ਅੰਕ_bs = 0;
		ਜਦੋਂ (arg[j] == '\\')
		{
		    ਅੰਕ_bs++;
		    j++;
		}

		ਜੇ (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    ਜਦੋਂ (ਅੰਕ_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    ਜਦੋਂ (ਅੰਕ_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜਦੋਂ (ਅੰਕ_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	ਨਹੀਂਤਾਂ
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&ਬਾਹਰਲਾ_ga, 1, 4096);

    saAttr.n_ਲੰਮਾਈ = ਆਕਾਰ(SECURITY_ਗੁਣ);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ਨਿਰਾ;

    // Create a pipe for the child's stdout.
    ਜੇ (!CreatePipe(&h_ਬੱਚਾ_Stdout_Rd, &h_ਬੱਚਾ_Stdout_Wr, &saAttr, 0)
	    || !ਸੈੱਟ_Handle_Information(h_ਬੱਚਾ_Stdout_Rd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	ਜਾਓ ਪੂਰਾ;
    }

    // Set up stdin from infile if provided.
    ਜੇ (infile != ਨਿਰਾ)
    {
	WCHAR *winfile = enc_ਮੰਜ਼ਿਲ_utf16(infile, ਨਿਰਾ);

	ਜੇ (winfile != ਨਿਰਾ)
	{
	    h_ਬੱਚਾ_Stdin_Rd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_ਸਾਧਾਰਨ, ਨਿਰਾ);
	    vim_free(winfile);
	}
    }

    Zero_ਸਮ੍ਰਿਤੀ(&pi, ਆਕਾਰ(pi));
    Zero_ਸਮ੍ਰਿਤੀ(&si, ਆਕਾਰ(si));
    si.cb = ਆਕਾਰ(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.w_Show_ਖਿੜਕੀ = SW_HIDE;
    si.hStdOutput = h_ਬੱਚਾ_Stdout_Wr;
    si.hStdError = h_ਬੱਚਾ_Stdout_Wr;
    si.h_Std_ਨਿਵੇਸ਼ = (h_ਬੱਚਾ_Stdin_Rd != INVALID_HANDLE_ਮੁੱਲ)
		    ? h_ਬੱਚਾ_Stdin_Rd : INVALID_HANDLE_ਮੁੱਲ;

    ch_log(ਨਿਰਾ, "directly executing: %s", (ਅੱਖਰ *)cmd_ga.ga_ਅੰਕੜੇ);

    // Create the child process directly, without going through the shell.
    ਜੇ (!vim_create_process((ਅੱਖਰ *)cmd_ga.ga_ਅੰਕੜੇ, TRUE,
		CREATE_DEFAULT_ERROR_ਬਹੁਲਕ | CREATE_NEW_PROCESS_ਸਮੂਹ,
		&si, &pi, ਨਿਰਾ, ਨਿਰਾ))
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), cmd_ga.ga_ਅੰਕੜੇ);
	ਜਾਓ ਪੂਰਾ;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(h_ਬੱਚਾ_Stdout_Wr);
    h_ਬੱਚਾ_Stdout_Wr = INVALID_HANDLE_ਮੁੱਲ;
    ਜੇ (h_ਬੱਚਾ_Stdin_Rd != INVALID_HANDLE_ਮੁੱਲ)
    {
	CloseHandle(h_ਬੱਚਾ_Stdin_Rd);
	h_ਬੱਚਾ_Stdin_Rd = INVALID_HANDLE_ਮੁੱਲ;
    }

    // Read output from child process.
    ਲਈ (;;)
    {
	ਅੱਖਰ	buf[4096];
	DWORD	n;

	ਜੇ (!ReadFile(h_ਬੱਚਾ_Stdout_Rd, buf, ਆਕਾਰ(buf), &n, ਨਿਰਾ) || n == 0)
	    ਰੋਕੋ;
	ਜੇ (ga_grow(&ਬਾਹਰਲਾ_ga, (ਪੂਰਨ)n) == OK)
	{
	    mch_memmove((ਅੱਖਰ *)ਬਾਹਰਲਾ_ga.ga_ਅੰਕੜੇ + ਬਾਹਰਲਾ_ga.ga_len, buf, n);
	    ਬਾਹਰਲਾ_ga.ga_len += (ਪੂਰਨ)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    Get_Exit_ਕੋਡ_Process(pi.hProcess, &exit_ਕੋਡ);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    ਸੈੱਟ_vim_var_nr(VV_SHELL_ERROR, (ਲੰਮਾ)exit_ਕੋਡ);

    ਜੇ (ਬਾਹਰਲਾ_ga.ga_len > 0)
    {
	ਬਫਰ = alloc(ਬਾਹਰਲਾ_ga.ga_len + 1);
	ਜੇ (ਬਫਰ != ਨਿਰਾ)
	{
	    mch_memmove(ਬਫਰ, ਬਾਹਰਲਾ_ga.ga_ਅੰਕੜੇ, ਬਾਹਰਲਾ_ga.ga_len);
	    ਜੇ (ret_len == ਨਿਰਾ)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		ਲਈ (i = 0; i < ਬਾਹਰਲਾ_ga.ga_len; ++i)
		    ਜੇ (ਬਫਰ[i] == NUL)
			ਬਫਰ[i] = 1;
		ਬਫਰ[ਬਾਹਰਲਾ_ga.ga_len] = NUL;
	    }
	    ਨਹੀਂਤਾਂ
		*ret_len = ਬਾਹਰਲਾ_ga.ga_len;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ret_len != ਨਿਰਾ)
	*ret_len = 0;

ਪੂਰਾ:
    ga_clear(&cmd_ga);
    ga_clear(&ਬਾਹਰਲਾ_ga);
    ਜੇ (h_ਬੱਚਾ_Stdout_Rd != INVALID_HANDLE_ਮੁੱਲ)
	CloseHandle(h_ਬੱਚਾ_Stdout_Rd);
    ਜੇ (h_ਬੱਚਾ_Stdout_Wr != INVALID_HANDLE_ਮੁੱਲ)
	CloseHandle(h_ਬੱਚਾ_Stdout_Wr);
    ਜੇ (h_ਬੱਚਾ_Stdin_Rd != INVALID_HANDLE_ਮੁੱਲ)
	CloseHandle(h_ਬੱਚਾ_Stdin_Rd);
    ਵਾਪਸ ਬਫਰ;
}
#ਜੇ_ਅੰਤ // FEAT_EVAL

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
    ਖਾਲੀ
mch_ਕੰਮ_ਸ਼ੁਰੂ(ਅੱਖਰ *cmd, ਕੰਮ_T *ਕੰਮ, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_ਗੁਣ saAttr;
    channel_T		*channel = ਨਿਰਾ;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    ਪੂਰਨ		use_null_for_ਵਿੱਚ = options->jo_io[PART_ਵਿੱਚ] == JIO_NULL;
    ਪੂਰਨ		use_null_for_ਬਾਹਰਲਾ = options->jo_io[PART_ਬਾਹਰਲਾ] == JIO_NULL;
    ਪੂਰਨ		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    ਪੂਰਨ		use_file_for_ਵਿੱਚ = options->jo_io[PART_ਵਿੱਚ] == JIO_FILE;
    ਪੂਰਨ		use_file_for_ਬਾਹਰਲਾ = options->jo_io[PART_ਬਾਹਰਲਾ] == JIO_FILE;
    ਪੂਰਨ		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    ਪੂਰਨ		use_ਬਾਹਰਲਾ_for_err = options->jo_io[PART_ERR] == JIO_ਬਾਹਰਲਾ;

    ਜੇ (use_ਬਾਹਰਲਾ_for_err && use_null_for_ਬਾਹਰਲਾ)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_ਮੁੱਲ;
    ifd[1] = INVALID_HANDLE_ਮੁੱਲ;
    ofd[0] = INVALID_HANDLE_ਮੁੱਲ;
    ofd[1] = INVALID_HANDLE_ਮੁੱਲ;
    efd[0] = INVALID_HANDLE_ਮੁੱਲ;
    efd[1] = INVALID_HANDLE_ਮੁੱਲ;
    ga_init2(&ga, ਆਕਾਰ(wchar_t), 500);

    jo = Create_ਕੰਮ_Object(ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (jo == ਨਿਰਾ)
    {
	ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_FAILED;
	ਜਾਓ failed;
    }

    ਜੇ (options->jo_env != ਨਿਰਾ)
	win32_build_env(options->jo_env, &ga, FALSE);

    Zero_ਸਮ੍ਰਿਤੀ(&pi, ਆਕਾਰ(pi));
    Zero_ਸਮ੍ਰਿਤੀ(&si, ਆਕਾਰ(si));
    si.cb = ਆਕਾਰ(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.w_Show_ਖਿੜਕੀ = SW_HIDE;

    saAttr.n_ਲੰਮਾਈ = ਆਕਾਰ(SECURITY_ਗੁਣ);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ਨਿਰਾ;

    ਜੇ (use_file_for_ਵਿੱਚ)
    {
	char_u *fname = options->jo_io_name[PART_ਵਿੱਚ];

	ifd[0] = ਕੰਮ_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_ਸਾਧਾਰਨ);
	ਜੇ (ifd[0] == INVALID_HANDLE_ਮੁੱਲ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ਜਾਓ failed;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (!use_null_for_ਵਿੱਚ
	    && (!create_pipe_pair(ifd)
		|| !ਸੈੱਟ_Handle_Information(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	ਜਾਓ failed;

    ਜੇ (use_file_for_ਬਾਹਰਲਾ)
    {
	char_u *fname = options->jo_io_name[PART_ਬਾਹਰਲਾ];

	ofd[1] = ਕੰਮ_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_ਸਾਧਾਰਨ);
	ਜੇ (ofd[1] == INVALID_HANDLE_ਮੁੱਲ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ਜਾਓ failed;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (!use_null_for_ਬਾਹਰਲਾ &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !ਸੈੱਟ_Handle_Information(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	ਜਾਓ failed;

    ਜੇ (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = ਕੰਮ_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_ਸਾਧਾਰਨ);
	ਜੇ (efd[1] == INVALID_HANDLE_ਮੁੱਲ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ਜਾਓ failed;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (!use_ਬਾਹਰਲਾ_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !ਸੈੱਟ_Handle_Information(efd[0], HANDLE_FLAG_INHERIT, 0)))
	ਜਾਓ failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_ਨਿਵੇਸ਼ = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_ਬਾਹਰਲਾ_for_err ? ofd[1] : efd[1];

    ਜੇ (!use_null_for_ਵਿੱਚ || !use_null_for_ਬਾਹਰਲਾ || !use_null_for_err)
    {
	ਜੇ (options->jo_ਸੈੱਟ & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    ਜੇ (channel != ਨਿਰਾ)
		++channel->ch_refcount;
	}
	ਨਹੀਂਤਾਂ
	    channel = ਜੋੜੋ_channel();
	ਜੇ (channel == ਨਿਰਾ)
	    ਜਾਓ failed;
    }

    ਜੇ (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_ਬਹੁਲਕ |
	    CREATE_NEW_PROCESS_ਸਮੂਹ |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_ਅੰਕੜੇ,
	    (ਅੱਖਰ *)options->jo_cwd))
    {
	CloseHandle(jo);
	ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_FAILED;
	ਜਾਓ failed;
    }

    ga_clear(&ga);

    ਜੇ (!ਸੌਂਪੋ_Process_ਮੰਜ਼ਿਲ_ਕੰਮ_Object(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = ਨਿਰਾ;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    ਕੰਮ->jv_proc_info = pi;
    ਕੰਮ->jv_ਕੰਮ_object = jo;
    ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    ਜੇ (!use_ਬਾਹਰਲਾ_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    ਕੰਮ->jv_channel = channel;
    ਜੇ (channel != ਨਿਰਾ)
    {
	channel_ਸੈੱਟ_pipes(channel,
		      use_file_for_ਵਿੱਚ || use_null_for_ਵਿੱਚ
					      ? INVALID_FD : (sock_T)ifd[1],
		      use_file_for_ਬਾਹਰਲਾ || use_null_for_ਬਾਹਰਲਾ
					     ? INVALID_FD : (sock_T)ofd[0],
		      use_ਬਾਹਰਲਾ_for_err || use_file_for_err || use_null_for_err
					    ? INVALID_FD : (sock_T)efd[0]);
	channel_ਸੈੱਟ_ਕੰਮ(channel, ਕੰਮ, options);
    }
    ਵਾਪਸ;

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

    ਅੱਖਰ *
mch_ਕੰਮ_ਸਥਿਤੀ(ਕੰਮ_T *ਕੰਮ)
{
    DWORD dw_Exit_ਕੋਡ = 0;

    ਜੇ (!Get_Exit_ਕੋਡ_Process(ਕੰਮ->jv_proc_info.hProcess, &dw_Exit_ਕੋਡ)
	    || dw_Exit_ਕੋਡ != STILL_ਸਰਗਰਮ)
    {
	ਕੰਮ->jv_exitval = (ਪੂਰਨ)dw_Exit_ਕੋਡ;
	ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ < ਕੰਮ_ENDED)
	{
	    ch_log(ਕੰਮ->jv_channel, "Job ended");
	    ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_ENDED;
	}
	ਵਾਪਸ "dead";
    }
    ਵਾਪਸ "run";
}

    ਕੰਮ_T *
mch_detect_ended_ਕੰਮ(ਕੰਮ_T *ਕੰਮ_list)
{
    HANDLE ਕੰਮ_Handles[ਵੱਧ_ਤੋਂ_ਵੱਧ_WAIT_OBJECTS];
    ਕੰਮ_T *ਕੰਮ_Array[ਵੱਧ_ਤੋਂ_ਵੱਧ_WAIT_OBJECTS];
    ਕੰਮ_T *ਕੰਮ = ਕੰਮ_list;

    ਜਦੋਂ (ਕੰਮ != ਨਿਰਾ)
    {
	DWORD n;
	DWORD ਨਤੀਜਾ;

	ਲਈ (n = 0; n < ਵੱਧ_ਤੋਂ_ਵੱਧ_WAIT_OBJECTS
				       && ਕੰਮ != ਨਿਰਾ; ਕੰਮ = ਕੰਮ->jv_ਅਗਲਾ)
	{
	    ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_STARTED)
	    {
		ਕੰਮ_Handles[n] = ਕੰਮ->jv_proc_info.hProcess;
		ਕੰਮ_Array[n] = ਕੰਮ;
		++n;
	    }
	}
	ਜੇ (n == 0)
	    ਜਾਰੀ;
	ਨਤੀਜਾ = Wait_For_ਗੁਣਜ_Objects(n, ਕੰਮ_Handles, FALSE, 0);
	ਜੇ (ਨਤੀਜਾ >= WAIT_OBJECT_0 && ਨਤੀਜਾ < WAIT_OBJECT_0 + n)
	{
	    ਕੰਮ_T *wait_ਕੰਮ = ਕੰਮ_Array[ਨਤੀਜਾ - WAIT_OBJECT_0];

	    ਜੇ (STRCMP(mch_ਕੰਮ_ਸਥਿਤੀ(wait_ਕੰਮ), "dead") == 0)
		ਵਾਪਸ wait_ਕੰਮ;
	}
    }
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ BOOL
terminate_all(HANDLE process, ਪੂਰਨ ਕੋਡ)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_ਮੁੱਲ;
    DWORD	    pid = GetProcessId(process);

    ਜੇ (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	ਜੇ (h != INVALID_HANDLE_ਮੁੱਲ)
	{
	    pe.dw_ਮਾਪ = ਆਕਾਰ(PROCESSENTRY32);
	    ਜੇ (!Process32_ਪਹਿਲਾ(h, &pe))
		ਜਾਓ theend;

	    ਕਰੋ
	    {
		ਜੇ (pe.th32_ਮਾਪਾ_Process_ID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    ਜੇ (ph != ਨਿਰਾ)
		    {
			terminate_all(ph, ਕੋਡ);
			CloseHandle(ph);
		    }
		}
	    } ਜਦੋਂ (Process32_ਅਗਲਾ(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    ਵਾਪਸ TerminateProcess(process, ਕੋਡ);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    ਪੂਰਨ
mch_signal_ਕੰਮ(ਕੰਮ_T *ਕੰਮ, char_u *how)
{
    ਪੂਰਨ ret;

    ਜੇ (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	ਜੇ (ਕੰਮ->jv_ਕੰਮ_object != ਨਿਰਾ)
	{
	    ਜੇ (ਕੰਮ->jv_channel != ਨਿਰਾ && ਕੰਮ->jv_channel->ch_anonymous_pipe)
		ਕੰਮ->jv_channel->ch_killing = TRUE;
	    ਵਾਪਸ Terminate_ਕੰਮ_Object(ਕੰਮ->jv_ਕੰਮ_object, (UINT)-1) ? OK : FAIL;
	}
	ਵਾਪਸ terminate_all(ਕੰਮ->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    ਜੇ (!AttachConsole(ਕੰਮ->jv_proc_info.dwProcessId))
	ਵਾਪਸ FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		ਕੰਮ->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    ਵਾਪਸ ret;
}

/*
 * Clear the data related to "job".
 */
    ਖਾਲੀ
mch_clear_ਕੰਮ(ਕੰਮ_T *ਕੰਮ)
{
    ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_FAILED)
	ਵਾਪਸ;

    ਜੇ (ਕੰਮ->jv_ਕੰਮ_object != ਨਿਰਾ)
	CloseHandle(ਕੰਮ->jv_ਕੰਮ_object);
    CloseHandle(ਕੰਮ->jv_proc_info.hProcess);
}
#ਜੇ_ਅੰਤ


#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    ਸਥਿਰ ਖਾਲੀ
termcap_ਬਹੁਲਕ_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
    DWORD cmodein;

    ਜੇ (g_f_Termcap_ਬਹੁਲਕ)
	ਵਾਪਸ;

    Save_Console_ਬਫਰ(&g_cbNonTermcap);

    ਜੇ (g_cbTermcap.IsValid)
    {
	/*
	 * We've been in termcap mode before.  Restore certain screen
	 * characteristics, including the buffer size and the window
	 * size.  Since we will be redrawing the screen, we don't need
	 * to restore the actual contents of the buffer.
	 */
	Restore_Console_ਬਫਰ(&g_cbTermcap, FALSE);
	reset_console_color_rgb();
	ਸੈੱਟ_Console_ਖਿੜਕੀ_Info(g_h_Con_ਬਾਹਰਲਾ, TRUE, &g_cbTermcap.Info.sr_ਖਿੜਕੀ);
	ਪੰਕਤੀਆਂ = g_cbTermcap.Info.dw_ਮਾਪ.Y;
	Columns = g_cbTermcap.Info.dw_ਮਾਪ.X;
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * This is our first time entering termcap mode.  Clear the console
	 * screen buffer, and resize the buffer to match the current window
	 * size.  We will use this as the size of our editing environment.
	 */
	Clear_Console_ਬਫਰ(g_attrCurrent);
	ਸੈੱਟ_console_color_rgb();
	Resize_Con_Buf_And_ਖਿੜਕੀ(g_h_Con_ਬਾਹਰਲਾ, Columns, ਪੰਕਤੀਆਂ);
    }

    resettitle();

    Get_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ, &cmodein);
    ਜੇ (g_f_Mouse_ਸਰਗਰਮ)
    {
	cmodein |= ENABLE_MOUSE_ਨਿਵੇਸ਼;
	cmodein &= ~ENABLE_ਤੇਜ਼_EDIT_ਬਹੁਲਕ;
    }
    ਨਹੀਂਤਾਂ
    {
	cmodein &= ~ENABLE_MOUSE_ਨਿਵੇਸ਼;
	cmodein |= g_cmodein & ENABLE_ਤੇਜ਼_EDIT_ਬਹੁਲਕ;
    }
    cmodein |= ENABLE_ਖਿੜਕੀ_ਨਿਵੇਸ਼;
    ਸੈੱਟ_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_f_Termcap_ਬਹੁਲਕ = TRUE;
}


/*
 * End termcap mode
 */
    ਸਥਿਰ ਖਾਲੀ
termcap_ਬਹੁਲਕ_ਸਿਰਾ(ਖਾਲੀ)
{
    DWORD cmodein;
    Console_ਬਫਰ *cb;
    COORD coord;
    DWORD dwDummy;

    ਜੇ (!g_f_Termcap_ਬਹੁਲਕ)
	ਵਾਪਸ;

    Save_Console_ਬਫਰ(&g_cbTermcap);

    Get_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_ਨਿਵੇਸ਼ | ENABLE_ਖਿੜਕੀ_ਨਿਵੇਸ਼);
    cmodein |= g_cmodein & ENABLE_ਤੇਜ਼_EDIT_ਬਹੁਲਕ;
    ਸੈੱਟ_Console_ਬਹੁਲਕ(g_h_Con_ਵਿੱਚ, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# ਨਹੀਂਤਾਂ
    cb = &g_cbNonTermcap;
# endif
    Restore_Console_ਬਫਰ(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    ਜੇ (exiting && use_alternate_screen_ਬਫਰ)
	vtp_printf("\033[?1049l");

    ਜੇ (!USE_WT && (p_rs || exiting))
    {
	/*
	 * Clear anything that happens to be on the current line.
	 */
	coord.X = 0;
	coord.Y = (SHORT) (p_rs ? cb->Info.dwCursorPosition.Y : (ਪੰਕਤੀਆਂ - 1));
	FillConsoleOutputCharacter(g_h_Con_ਬਾਹਰਲਾ, ' ',
		cb->Info.dw_ਮਾਪ.X, coord, &dwDummy);
	/*
	 * The following is just for aesthetics.  If we are exiting without
	 * restoring the screen, then we want to have a prompt string
	 * appear at the bottom line.  However, the command interpreter
	 * seems to always advance the cursor one line before displaying
	 * the prompt string, which causes the screen to scroll.  To
	 * counter this, move the cursor up one line before exiting.
	 */
	ਜੇ (exiting && !p_rs)
	    coord.Y--;
	/*
	 * Position the cursor at the leftmost column of the desired row.
	 */
	ਸੈੱਟ_Console_Cursor_Position(g_h_Con_ਬਾਹਰਲਾ, coord);
    }
    ਸੈੱਟ_Console_Cursor_Info(g_h_Con_ਬਾਹਰਲਾ, &g_cci);
    g_f_Termcap_ਬਹੁਲਕ = FALSE;
}
#ਜੇ_ਅੰਤ


#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    ਸਥਿਰ ਖਾਲੀ
clear_chars(
    COORD coord,
    DWORD n)
{
    ਜੇ (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_h_Con_ਬਾਹਰਲਾ, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_h_Con_ਬਾਹਰਲਾ, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    ਨਹੀਂਤਾਂ
    {
	ਸੈੱਟ_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    ਸਥਿਰ ਖਾਲੀ
clear_screen(ਖਾਲੀ)
{
    g_coord.X = g_coord.Y = 0;

    ਜੇ (!vtp_working)
	clear_chars(g_coord, ਪੰਕਤੀਆਂ * Columns);
    ਨਹੀਂਤਾਂ
    {
	ਸੈੱਟ_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    ਸਥਿਰ ਖਾਲੀ
clear_ਮੰਜ਼ਿਲ_ਸਿਰਾ_ਦਾ_display(ਖਾਲੀ)
{
    COORD save = g_coord;

    ਜੇ (!vtp_working)
	clear_chars(g_coord, (ਪੰਕਤੀਆਂ - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    ਨਹੀਂਤਾਂ
    {
	ਸੈੱਟ_console_color_rgb();
	gotoxy(g_coord.X + 1, g_coord.Y + 1);
	vtp_printf("\033[0J");

	gotoxy(save.X + 1, save.Y + 1);
	g_coord = save;
    }
}


/*
 * Clear to end of line
 */
    ਸਥਿਰ ਖਾਲੀ
clear_ਮੰਜ਼ਿਲ_ਸਿਰਾ_ਦਾ_ਸਤਰ(ਖਾਲੀ)
{
    COORD save = g_coord;

    ਜੇ (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    ਨਹੀਂਤਾਂ
    {
	ਸੈੱਟ_console_color_rgb();
	gotoxy(g_coord.X + 1, g_coord.Y + 1);
	vtp_printf("\033[0K");

	gotoxy(save.X + 1, save.Y + 1);
	g_coord = save;
    }
}


/*
 * Scroll the scroll region up by `cLines' lines
 */
    ਸਥਿਰ ਖਾਲੀ
scroll(ਬਿਨਾਨਿਸ਼ਾਨੀ c_ਸਤਰਾਂ)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.ਖੱਬਾ_3 + 1, g_srScrollRegion.ਸਿਖਰ_2 + 1);
    delete_ਸਤਰਾਂ(c_ਸਤਰਾਂ);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_scroll_region(
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਖੱਬਾ,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਿਖਰ,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੱਜਾ,
    ਬਿਨਾਨਿਸ਼ਾਨੀ bottom)
{
    ਜੇ (ਖੱਬਾ >= ਸੱਜਾ
	    || ਸਿਖਰ >= bottom
	    || ਸੱਜਾ > (ਬਿਨਾਨਿਸ਼ਾਨੀ) Columns - 1
	    || bottom > (ਬਿਨਾਨਿਸ਼ਾਨੀ) ਪੰਕਤੀਆਂ - 1)
	ਵਾਪਸ;

    g_srScrollRegion.ਖੱਬਾ_3 =   ਖੱਬਾ;
    g_srScrollRegion.ਸਿਖਰ_2 =    ਸਿਖਰ;
    g_srScrollRegion.ਸੱਜਾ_3 =  ਸੱਜਾ;
    g_srScrollRegion.Bottom = bottom;
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_scroll_region_tb(
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਿਖਰ,
    ਬਿਨਾਨਿਸ਼ਾਨੀ bottom)
{
    ਜੇ (ਸਿਖਰ >= bottom || bottom > (ਬਿਨਾਨਿਸ਼ਾਨੀ)ਪੰਕਤੀਆਂ - 1)
	ਵਾਪਸ;

    g_srScrollRegion.ਸਿਖਰ_2 = ਸਿਖਰ;
    g_srScrollRegion.Bottom = bottom;
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_scroll_region_lr(
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਖੱਬਾ,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੱਜਾ)
{
    ਜੇ (ਖੱਬਾ >= ਸੱਜਾ || ਸੱਜਾ > (ਬਿਨਾਨਿਸ਼ਾਨੀ)Columns - 1)
	ਵਾਪਸ;

    g_srScrollRegion.ਖੱਬਾ_3 = ਖੱਬਾ;
    g_srScrollRegion.ਸੱਜਾ_3 = ਸੱਜਾ;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਾਓ_ਸਤਰਾਂ(ਬਿਨਾਨਿਸ਼ਾਨੀ c_ਸਤਰਾਂ)
{
    SMALL_RECT	    ਸਰੋਤ, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.ਖੱਬਾ_3 + 1, g_srScrollRegion.ਸਿਖਰ_2 + 1);

    dest.X = g_srScrollRegion.ਖੱਬਾ_3;
    dest.Y = g_coord.Y + c_ਸਤਰਾਂ;

    ਸਰੋਤ.ਖੱਬਾ_3   = g_srScrollRegion.ਖੱਬਾ_3;
    ਸਰੋਤ.ਸਿਖਰ_2	  = g_coord.Y;
    ਸਰੋਤ.ਸੱਜਾ_3  = g_srScrollRegion.ਸੱਜਾ_3;
    ਸਰੋਤ.Bottom = g_srScrollRegion.Bottom - c_ਸਤਰਾਂ;

    clip.ਖੱਬਾ_3   = g_srScrollRegion.ਖੱਬਾ_3;
    clip.ਸਿਖਰ_2    = g_coord.Y;
    clip.ਸੱਜਾ_3  = g_srScrollRegion.ਸੱਜਾ_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    ਜੇ (!USE_VTP)
	fill.ਗੁਣ_2 = g_attrCurrent;
    ਨਹੀਂਤਾਂ
	fill.ਗੁਣ_2 = g_attrDefault;

    ਸੈੱਟ_console_color_rgb();

    Scroll_Console_Screen_ਬਫਰ(g_h_Con_ਬਾਹਰਲਾ, &ਸਰੋਤ, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    ਜੇ (ਸਰੋਤ.Bottom < dest.Y)
    {
	COORD coord;
	ਪੂਰਨ   i;

	coord.X = ਸਰੋਤ.ਖੱਬਾ_3;
	ਲਈ (i = clip.ਸਿਖਰ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ਸਰੋਤ.ਸੱਜਾ_3 - ਸਰੋਤ.ਖੱਬਾ_3 + 1);
	}
    }

    ਜੇ (vtp_working)
    {
	COORD coord;
	ਪੂਰਨ i;

	coord.X = ਸਰੋਤ.ਖੱਬਾ_3;
	ਲਈ (i = ਸਰੋਤ.ਸਿਖਰ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ਸਰੋਤ.ਸੱਜਾ_3 - ਸਰੋਤ.ਖੱਬਾ_3 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    ਸਥਿਰ ਖਾਲੀ
delete_ਸਤਰਾਂ(ਬਿਨਾਨਿਸ਼ਾਨੀ c_ਸਤਰਾਂ)
{
    SMALL_RECT	    ਸਰੋਤ, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    ਪੂਰਨ		    nb;

    gotoxy(g_srScrollRegion.ਖੱਬਾ_3 + 1, g_srScrollRegion.ਸਿਖਰ_2 + 1);

    dest.X = g_srScrollRegion.ਖੱਬਾ_3;
    dest.Y = g_coord.Y;

    ਸਰੋਤ.ਖੱਬਾ_3   = g_srScrollRegion.ਖੱਬਾ_3;
    ਸਰੋਤ.ਸਿਖਰ_2	  = g_coord.Y + c_ਸਤਰਾਂ;
    ਸਰੋਤ.ਸੱਜਾ_3  = g_srScrollRegion.ਸੱਜਾ_3;
    ਸਰੋਤ.Bottom = g_srScrollRegion.Bottom;

    clip.ਖੱਬਾ_3   = g_srScrollRegion.ਖੱਬਾ_3;
    clip.ਸਿਖਰ_2    = g_coord.Y;
    clip.ਸੱਜਾ_3  = g_srScrollRegion.ਸੱਜਾ_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    ਜੇ (!vtp_working)
	fill.ਗੁਣ_2 = g_attrCurrent;
    ਨਹੀਂਤਾਂ
	fill.ਗੁਣ_2 = g_attrDefault;

    ਸੈੱਟ_console_color_rgb();

    Scroll_Console_Screen_ਬਫਰ(g_h_Con_ਬਾਹਰਲਾ, &ਸਰੋਤ, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (ਸਰੋਤ.Bottom - ਸਰੋਤ.ਸਿਖਰ_2) + 1;

    ਜੇ (nb < ਸਰੋਤ.ਸਿਖਰ_2)
    {
	COORD coord;
	ਪੂਰਨ   i;

	coord.X = ਸਰੋਤ.ਖੱਬਾ_3;
	ਲਈ (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ਸਰੋਤ.ਸੱਜਾ_3 - ਸਰੋਤ.ਖੱਬਾ_3 + 1);
	}
    }

    ਜੇ (vtp_working)
    {
	COORD coord;
	ਪੂਰਨ i;

	coord.X = ਸਰੋਤ.ਖੱਬਾ_3;
	ਲਈ (i = nb; i <= ਸਰੋਤ.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ਸਰੋਤ.ਸੱਜਾ_3 - ਸਰੋਤ.ਖੱਬਾ_3 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    ਸਥਿਰ ਖਾਲੀ
gotoxy(
    ਬਿਨਾਨਿਸ਼ਾਨੀ x,
    ਬਿਨਾਨਿਸ਼ਾਨੀ y)
{
    ਜੇ (x < 1 || x > (ਬਿਨਾਨਿਸ਼ਾਨੀ)Columns || y < 1 || y > (ਬਿਨਾਨਿਸ਼ਾਨੀ)ਪੰਕਤੀਆਂ)
	ਵਾਪਸ;

    ਜੇ (!USE_VTP)
    {
	// There are reports of double-width characters not displayed
	// correctly.  This workaround should fix it, similar to how it's done
	// for VTP.
	g_coord.X = 0;
	ਸੈੱਟ_Console_Cursor_Position(g_h_Con_ਬਾਹਰਲਾ, g_coord);

	// external cursor coords are 1-based; internal are 0-based
	g_coord.X = x - 1;
	g_coord.Y = y - 1;
	ਸੈੱਟ_Console_Cursor_Position(g_h_Con_ਬਾਹਰਲਾ, g_coord);
    }
    ਨਹੀਂਤਾਂ
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
    ਸਥਿਰ ਖਾਲੀ
textattr(ਸ਼ਬਦ_2 wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    ਸੈੱਟ_Console_ਪਾਠ_Attribute(g_h_Con_ਬਾਹਰਲਾ, wAttr);
}


    ਸਥਿਰ ਖਾਲੀ
textcolor(ਸ਼ਬਦ_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    ਜੇ (!vtp_working)
	ਸੈੱਟ_Console_ਪਾਠ_Attribute(g_h_Con_ਬਾਹਰਲਾ, g_attrCurrent);
    ਨਹੀਂਤਾਂ
	vtp_sgr_bulk(wAttr);
}


    ਸਥਿਰ ਖਾਲੀ
textbackground(ਸ਼ਬਦ_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    ਜੇ (!vtp_working)
	ਸੈੱਟ_Console_ਪਾਠ_Attribute(g_h_Con_ਬਾਹਰਲਾ, g_attrCurrent);
    ਨਹੀਂਤਾਂ
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    ਸਥਿਰ ਖਾਲੀ
normvideo(ਖਾਲੀ)
{
    ਜੇ (!vtp_working)
	textattr(g_attrDefault);
    ਨਹੀਂਤਾਂ
	vtp_sgr_bulk(0);
}


ਸਥਿਰ ਸ਼ਬਦ_2 g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    ਸਥਿਰ ਖਾਲੀ
standout(ਖਾਲੀ)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((ਸ਼ਬਦ_2) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    ਸਥਿਰ ਖਾਲੀ
standend(ਖਾਲੀ)
{
    ਜੇ (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    ਖਾਲੀ
mch_ਸੈੱਟ_ਸਾਧਾਰਨ_colors(ਖਾਲੀ)
{
    char_u	*p;
    ਪੂਰਨ		n;

    cterm_ਸਾਧਾਰਨ_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_ਸਾਧਾਰਨ_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    ਜੇ (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	ਜੇ (*p == 'm' && n > 0)
	{
	    cterm_ਸਾਧਾਰਨ_fg_color = (n & 0xf) + 1;
	    cterm_ਸਾਧਾਰਨ_bg_color = ((n >> 4) & 0xf) + 1;
	}
    }
# ifdef FEAT_TERMGUICOLORS
    cterm_ਸਾਧਾਰਨ_fg_gui_color = INVALCOLOR;
    cterm_ਸਾਧਾਰਨ_bg_gui_color = INVALCOLOR;
# endif
}


/*
 * visual bell: flash the screen
 */
    ਸਥਿਰ ਖਾਲੀ
visual_bell(ਖਾਲੀ)
{
    COORD   coordOrigin = {0, 0};
    ਸ਼ਬਦ_2    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = ਨਿਰਾ;

# ifdef FEAT_TERMGUICOLORS
    ਜੇ (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(ਸ਼ਬਦ_2, ਪੰਕਤੀਆਂ * Columns);
	ਜੇ (oldattrs == ਨਿਰਾ)
	    ਵਾਪਸ;
	ReadConsoleOutputAttribute(g_h_Con_ਬਾਹਰਲਾ, oldattrs, ਪੰਕਤੀਆਂ * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_h_Con_ਬਾਹਰਲਾ, attrFlash, ਪੰਕਤੀਆਂ * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    ਜੇ (oldattrs != ਨਿਰਾ)
    {
	WriteConsoleOutputAttribute(g_h_Con_ਬਾਹਰਲਾ, oldattrs, ਪੰਕਤੀਆਂ * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    ਸਥਿਰ ਖਾਲੀ
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    ਜੇ (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	ਵਾਪਸ;
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
    ਸਥਿਰ DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cb_ਮੰਜ਼ਿਲ_Write)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    ਸਥਿਰ DWORD    cells;
    ਸਥਿਰ WCHAR    *unicodebuf = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ	    unibuflen = 0;
    ਸਥਿਰ ਪੂਰਨ	    ਲੰਮਾਈ;
    ਪੂਰਨ		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    ਸਥਿਰ WCHAR    *utf8spbuf = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ	    utf8splength;
    ਸਥਿਰ DWORD    utf8spcells;
    ਸਥਿਰ WCHAR    **utf8usingbuf = &unicodebuf;

    ਜੇ (cb_ਮੰਜ਼ਿਲ_Write != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	ਕਰੋ
	{
	    ਲੰਮਾਈ = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(cp, 0, (LPCSTR)pchBuf, cb_ਮੰਜ਼ਿਲ_Write,
							unicodebuf, unibuflen);
	    ਜੇ (ਲੰਮਾਈ && ਲੰਮਾਈ <= unibuflen)
		ਰੋਕੋ;
	    vim_free(unicodebuf);
	    unicodebuf = ਲੰਮਾਈ ? LALLOC_MULT(WCHAR, ਲੰਮਾਈ) : ਨਿਰਾ;
	    unibuflen = unibuflen ? 0 : ਲੰਮਾਈ;
	} ਜਦੋਂ (TRUE);
	cells = mb_string2cells(pchBuf, cb_ਮੰਜ਼ਿਲ_Write);
    }
    ਨਹੀਂਤਾਂ // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	ਜੇ (utf8usingbuf != &utf8spbuf)
	{
	    ਜੇ (utf8spbuf == ਨਿਰਾ)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		ਲੰਮਾਈ = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, 0, " ", 1, ਨਿਰਾ, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, ਲੰਮਾਈ);
		ਜੇ (utf8spbuf != ਨਿਰਾ)
		{
		    Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, 0, " ", 1, utf8spbuf, ਲੰਮਾਈ);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = ਲੰਮਾਈ;
		    utf8spcells = cells;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		utf8usingbuf = &utf8spbuf;
		ਲੰਮਾਈ = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    ਜੇ (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_h_Con_ਬਾਹਰਲਾ, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	ਜੇ (WriteConsoleOutputCharacterW(g_h_Con_ਬਾਹਰਲਾ, *utf8usingbuf, ਲੰਮਾਈ,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (WriteConsoleW(g_h_Con_ਬਾਹਰਲਾ, *utf8usingbuf, ਲੰਮਾਈ, &cchwritten,
		    ਨਿਰਾ) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    ਜੇ (cchwritten == (DWORD)ਲੰਮਾਈ)
    {
	written = cb_ਮੰਜ਼ਿਲ_Write;
	g_coord.X += (SHORT)cells;
    }
    ਨਹੀਂਤਾਂ
    {
	char_u *p = pchBuf;
	ਲਈ (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    ਜਦੋਂ (g_coord.X > g_srScrollRegion.ਸੱਜਾ_3)
    {
	g_coord.X -= (SHORT) Columns;
	ਜੇ (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    ਜੇ (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    ਵਾਪਸ written;
}

    ਸਥਿਰ char_u *
get_seq(
    ਪੂਰਨ *ਦਲੀਲਾਂ,
    ਪੂਰਨ *ਗਿਣਤੀ_2,
    char_u *ਸਿਰ)
{
    ਪੂਰਨ argc;
    char_u *p;

    ਜੇ (ਸਿਰ == ਨਿਰਾ || *ਸਿਰ != '\033')
	ਵਾਪਸ ਨਿਰਾ;

    argc = 0;
    p = ਸਿਰ;
    ++p;
    ਕਰੋ
    {
	++p;
	ਦਲੀਲਾਂ[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } ਜਦੋਂ (*p == ';');
    *ਗਿਣਤੀ_2 = argc;

    ਵਾਪਸ p;
}

    ਸਥਿਰ char_u *
get_sgr(
    ਪੂਰਨ *ਦਲੀਲਾਂ,
    ਪੂਰਨ *ਗਿਣਤੀ_2,
    char_u *ਸਿਰ)
{
    char_u *p = get_seq(ਦਲੀਲਾਂ, ਗਿਣਤੀ_2, ਸਿਰ);

    ਵਾਪਸ (p && *p == 'm') ? ++p : ਨਿਰਾ;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    ਸਥਿਰ char_u *
sgrn2(
    char_u *ਸਿਰ,
    ਪੂਰਨ	   n)
{
    ਪੂਰਨ argc;
    ਪੂਰਨ ਦਲੀਲਾਂ[16];
    char_u *p = get_sgr(ਦਲੀਲਾਂ, &argc, ਸਿਰ);

    ਵਾਪਸ p && argc == 5 && ਦਲੀਲਾਂ[0] == n && ਦਲੀਲਾਂ[1] == 2 ? p : ਨਿਰਾ;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    ਸਥਿਰ char_u *
sgrnc(
    char_u *ਸਿਰ,
    ਪੂਰਨ	   n)
{
    ਪੂਰਨ argc;
    ਪੂਰਨ ਦਲੀਲਾਂ[16];
    char_u *p = get_sgr(ਦਲੀਲਾਂ, &argc, ਸਿਰ);

    ਵਾਪਸ p && argc == 1 && ਦਲੀਲਾਂ[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : ਨਿਰਾ;
}

    ਸਥਿਰ char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    ਜਦੋਂ (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    ਵਾਪਸ p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    ਸਥਿਰ char_u *
sgrn2c(
    char_u *ਸਿਰ,
    ਪੂਰਨ	   n)
{
    char_u *p = sgrn2(ਸਿਰ, n);

    ਵਾਪਸ p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : ਨਿਰਾ;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    ਸਥਿਰ char_u *
sgrn2cn(
    char_u *ਸਿਰ,
    ਪੂਰਨ n)
{
    char_u *p = sgrn2(ਸਿਰ, n);

    ਵਾਪਸ p && p[0] == 0x0a && p[1] == '\033' ? ++p : ਨਿਰਾ;
}

ਸਥਿਰ bool ਵਿੱਚ_dcs = false;	// inside a DCS (sixel) sequence
ਸਥਿਰ bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    ਸਥਿਰ ਖਾਲੀ
write_vt(char_u *s, ਪੂਰਨ len)
{
    ਪੂਰਨ		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, ਨਿਰਾ, 0);
    ਜੇ (wlen <= 0)
	ਵਾਪਸ;
    ws = ALLOC_MULT(WCHAR, wlen);
    ਜੇ (ws == ਨਿਰਾ)
	ਵਾਪਸ;
    Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_h_Con_ਬਾਹਰਲਾ, ws, wlen, &written, ਨਿਰਾ);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    ਸਥਿਰ char_u *
skip_dcs(char_u *s, char_u *ਸਿਰਾ)
{
    ਲਈ (char_u *p = s; p < ਸਿਰਾ; ++p)
    {
	ਜੇ (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    ਵਿੱਚ_dcs = false;
	    ਵਾਪਸ p + 1;
	}
	dcs_esc = *p == ESC;
    }
    ਵਾਪਸ ਸਿਰਾ;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    ਖਾਲੀ
mch_write(
    char_u  *s,
    ਪੂਰਨ	    len)
{
    char_u  *ਸਿਰਾ = s + len;

# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ;
# endif

    ਜੇ (!ਪਦ_console)
    {
	ਲਿਖ(1, s, (ਬਿਨਾਨਿਸ਼ਾਨੀ)len);
	ਵਾਪਸ;
    }

    // translate ESC | sequences into faked bios calls
    ਜਦੋਂ (len--)
    {
	ਪੂਰਨ ਅਗੇਤਰ = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	ਜੇ (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    ਵਾਪਸ;
	}

	ਜੇ (ਵਿੱਚ_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    ਪੂਰਨ	l = (ਪੂਰਨ)(skip_dcs(s, ਸਿਰਾ) - s);

	    ਜੇ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    ਜਾਰੀ;
	}

	ਜਦੋਂ (s + ++ਅਗੇਤਰ < ਸਿਰਾ)
	{
	    ch = s[ਅਗੇਤਰ];
	    ਜੇ (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		ਰੋਕੋ;
	}

	ਜੇ (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    ਜੇ (ਅਗੇਤਰ != 0)
		ਅਗੇਤਰ = 1;
	}

	ਜੇ (ਅਗੇਤਰ != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, ਅਗੇਤਰ);
# ifdef MCH_WRITE_DUMP
	    ਜੇ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, ਆਕਾਰ(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	ਨਹੀਂਤਾਂ ਜੇ (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    ਜੇ (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.ਖੱਬਾ_3 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		gotoxy(g_srScrollRegion.ਖੱਬਾ_3 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    ਜੇ (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	ਨਹੀਂਤਾਂ ਜੇ (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.ਖੱਬਾ_3+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ਜੇ (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	ਨਹੀਂਤਾਂ ਜੇ (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    ਜੇ (g_coord.X > g_srScrollRegion.ਖੱਬਾ_3)
		g_coord.X--;
	    ਨਹੀਂਤਾਂ ਜੇ (g_coord.Y > g_srScrollRegion.ਸਿਖਰ_2)
	    {
		g_coord.X = g_srScrollRegion.ਸੱਜਾ_3;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ਜੇ (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	ਨਹੀਂਤਾਂ ਜੇ (s[0] == '\a')
	{
	    // \a, bell
	    ਸੁਨੇਹਾ_Beep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    ਜੇ (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	ਨਹੀਂਤਾਂ ਜੇ (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    ਪੂਰਨ	    arg1 = 0, arg2 = 0, argc = 0, ਦਲੀਲਾਂ[16];
	    char_u  *sp;

	    ਚੋਣ (s[2])
	    {
	    ਹਾਲਤ '0': ਹਾਲਤ '1': ਹਾਲਤ '2': ਹਾਲਤ '3': ਹਾਲਤ '4':
	    ਹਾਲਤ '5': ਹਾਲਤ '6': ਹਾਲਤ '7': ਹਾਲਤ '8': ਹਾਲਤ '9':
		ਜੇ (*(p = get_seq(ਦਲੀਲਾਂ, &argc, s)) != 'm')
		    ਜਾਓ notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		ਜੇ (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (ਪੂਰਨ)(p - s);
		    s = p;
		    ਰੋਕੋ;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		ਜੇ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		ਜੇ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		ਜੇ (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		ਜੇ (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(ਦਲੀਲਾਂ, &argc, p);

notsgr:
		arg1 = ਦਲੀਲਾਂ[0];
		arg2 = ਦਲੀਲਾਂ[1];
		ਜੇ (*p == 'm')
		{
		    ਜੇ (argc == 1 && ਦਲੀਲਾਂ[0] == 0)
			normvideo();
		    ਨਹੀਂਤਾਂ ਜੇ (argc == 1)
		    {
			ਜੇ (USE_VTP)
			    textcolor((ਸ਼ਬਦ_2)arg1);
			ਨਹੀਂਤਾਂ
			    textattr((ਸ਼ਬਦ_2)arg1);
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (vtp_working)
			vtp_sgr_bulks(argc, ਦਲੀਲਾਂ);
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc == 2 && *p == 'r')
		{
		    ਸੈੱਟ_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc == 2 && *p == 'R')
		{
		    ਸੈੱਟ_scroll_region_tb(arg1, arg2);
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc == 2 && *p == 'V')
		{
		    ਸੈੱਟ_scroll_region_lr(arg1, arg2);
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.ਸਿਖਰ_2, g_coord.Y - arg1) + 1);
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc == 1 && *p == 'b')
		{
		    textbackground((ਸ਼ਬਦ_2) arg1);
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.ਸੱਜਾ_3, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc == 1 && *p == 'f')
		{
		    textcolor((ਸ਼ਬਦ_2) arg1);
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc == 1 && *p == 'L')
		{
		    ਪਾਓ_ਸਤਰਾਂ(arg1);
		}
		ਨਹੀਂਤਾਂ ਜੇ (argc == 1 && *p == 'M')
		{
		    delete_ਸਤਰਾਂ(arg1);
		}

		len -= (ਪੂਰਨ)(p - s);
		s = p + 1;
		ਰੋਕੋ;

	    ਹਾਲਤ 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.ਸਿਖਰ_2, g_coord.Y - 1) + 1);
		ਜਾਓ got3;

	    ਹਾਲਤ 'B':
		visual_bell();
		ਜਾਓ got3;

	    ਹਾਲਤ 'C':
		gotoxy(min(g_srScrollRegion.ਸੱਜਾ_3, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		ਜਾਓ got3;

	    ਹਾਲਤ 'E':
		termcap_ਬਹੁਲਕ_ਸਿਰਾ();
		ਜਾਓ got3;

	    ਹਾਲਤ 'F':
		standout();
		ਜਾਓ got3;

	    ਹਾਲਤ 'f':
		standend();
		ਜਾਓ got3;

	    ਹਾਲਤ 'H':
		gotoxy(1, 1);
		ਜਾਓ got3;

	    ਹਾਲਤ 'j':
		clear_ਮੰਜ਼ਿਲ_ਸਿਰਾ_ਦਾ_display();
		ਜਾਓ got3;

	    ਹਾਲਤ 'J':
		clear_screen();
		ਜਾਓ got3;

	    ਹਾਲਤ 'K':
		clear_ਮੰਜ਼ਿਲ_ਸਿਰਾ_ਦਾ_ਸਤਰ();
		ਜਾਓ got3;

	    ਹਾਲਤ 'L':
		ਪਾਓ_ਸਤਰਾਂ(1);
		ਜਾਓ got3;

	    ਹਾਲਤ 'M':
		delete_ਸਤਰਾਂ(1);
		ਜਾਓ got3;

	    ਹਾਲਤ 'S':
		termcap_ਬਹੁਲਕ_ਸ਼ੁਰੂ();
		ਜਾਓ got3;

	    ਹਾਲਤ 'V':
		cursor_visible(TRUE);
		ਜਾਓ got3;

	    ਹਾਲਤ 'v':
		cursor_visible(FALSE);
		ਜਾਓ got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    ਜੇ (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, ਆਕਾਰ(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	ਨਹੀਂਤਾਂ ਜੇ (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    ਪੂਰਨ l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    ਜਦੋਂ (s + l < ਸਿਰਾ && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    ਜੇ (s + l < ਸਿਰਾ && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		ਜੇ (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	ਨਹੀਂਤਾਂ ਜੇ (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    ਪੂਰਨ	l;

	    ਵਿੱਚ_dcs = true;
	    dcs_esc = false;
	    l = (ਪੂਰਨ)(skip_dcs(s + 2, ਸਿਰਾ) - s);
	    ਜੇ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    ਜੇ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, ਆਕਾਰ(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    ਜੇ (fdDump)
	fflush(fdDump);
# endif
}

#ਨਹੀਂਤਾਂ // FEAT_GUI_MSWIN && !VIMDLL
    ਖਾਲੀ
mch_write(
    char_u  *s ਅਣਵਰਤਿਆ,
    ਪੂਰਨ	    len ਅਣਵਰਤਿਆ)
{
    // never used
}

#ਜੇ_ਅੰਤ


/*
 * Delay for "msec" milliseconds.
 */
    ਖਾਲੀ
mch_delay(
    ਲੰਮਾ    msec,
    ਪੂਰਨ	    flags ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((ਪੂਰਨ)msec);	    // never wait for input
#ਨਹੀਂਤਾਂ // Console
# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	Sleep((ਪੂਰਨ)msec);	    // never wait for input
	ਵਾਪਸ;
    }
# endif
    ਜੇ (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	ਜੇ (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    ਪੂਰਨ towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    ਜਦੋਂ (msec > 0)
	    {
		mzvim_check_ਧਾਗੇ();
		ਜੇ (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	ਨਹੀਂਤਾਂ
# endif
	    Sleep((ਪੂਰਨ)msec);
    ਨਹੀਂਤਾਂ
	WaitForChar(msec, FALSE);
#ਜੇ_ਅੰਤ
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    ਪੂਰਨ
mch_remove(char_u *name)
{
    WCHAR	*wn;
    ਪੂਰਨ		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    ਜੇ (mch_isdir(name) && mch_is_symbolic_link(name))
	ਵਾਪਸ mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_ਸਾਧਾਰਨ);

    wn = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (wn == ਨਿਰਾ)
	ਵਾਪਸ -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    ਵਾਪਸ n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    ਖਾਲੀ
mch_breakcheck(ਪੂਰਨ force ਅਣਵਰਤਿਆ)
{
#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
	ਜੇ (g_fCtrlCPressed || g_fCBrkPressed)
	{
	    ctrl_break_was_pressed = g_fCBrkPressed;
	    g_fCtrlCPressed = g_fCBrkPressed = FALSE;
	    got_int = TRUE;
	}
#ਜੇ_ਅੰਤ
}

// physical RAM to leave for the OS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WINNT_RESERVE_ਬਾਈਟਾਂ     (256*1024*1024)

/*
 * How much main memory in KiB that can be used by VIM.
 */
    long_u
mch_ਕੁੱਲ_mem(ਪੂਰਨ special ਅਣਵਰਤਿਆ)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dw_ਲੰਮਾਈ = ਆਕਾਰ(MEMORYSTATUSEX);
    Global_ਸਮ੍ਰਿਤੀ_ਸਥਿਤੀ_Ex(&ms);
    ਜੇ (ms.ullAvailVirtual < ms.ull_ਕੁੱਲ_Phys)
    {
	// Process address space fits in physical RAM, use all of it.
	ਵਾਪਸ (long_u)(ms.ullAvailVirtual / 1024);
    }
    ਜੇ (ms.ull_ਕੁੱਲ_Phys <= WINNT_RESERVE_ਬਾਈਟਾਂ)
    {
	// Catch old NT box or perverse hardware setup.
	ਵਾਪਸ (long_u)((ms.ull_ਕੁੱਲ_Phys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    ਵਾਪਸ (long_u)((ms.ull_ਕੁੱਲ_Phys - WINNT_RESERVE_ਬਾਈਟਾਂ) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    ਪੂਰਨ
mch_expandpath(
    garray_T	*gap,
    char_u	*ਰਸਤਾ,
    ਪੂਰਨ		flags)		// EW_* flags
{
    ਵਾਪਸ dos_expandpath(gap, ਰਸਤਾ, 0, flags, FALSE);
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
    ਪੂਰਨ
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	sz_ਆਰਜ਼ੀ_File[MAX_ਰਸਤਾ_2 + 1];
    WCHAR	sz_New_ਰਸਤਾ[MAX_ਰਸਤਾ_2 + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    ਲਈ (p = q = wold; *p != L'\0'; ++p)
    {
	ਚੋਣ (*p)
	{
	ਹਾਲਤ L'/':
	ਹਾਲਤ L'\\':
	ਹਾਲਤ L':':
	    ਜੇ (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    ਰੋਕੋ;

	ਮੂਲ:
	    ਰੋਕੋ;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    ਜੇ ((ਪੂਰਨ)(p - q) < 8 || q[6] != L'~')
	ਵਾਪਸ (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    ਜੇ (Get_ਭਰਿਆ_ਰਸਤਾ_Name_W(wnew, MAX_ਰਸਤਾ_2, sz_New_ਰਸਤਾ, &p) == 0 || p == ਨਿਰਾ)
	ਵਾਪਸ -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    ਜੇ (Get_ਆਰਜ਼ੀ_File_Name_W(sz_New_ਰਸਤਾ, L"VIM", 0, sz_ਆਰਜ਼ੀ_File) == 0)
	ਵਾਪਸ -2;

    // blow the temp file away
    ਜੇ (!DeleteFileW(sz_ਆਰਜ਼ੀ_File))
	ਵਾਪਸ -3;

    // rename old file to the temp file
    ਜੇ (!MoveFileW(wold, sz_ਆਰਜ਼ੀ_File))
	ਵਾਪਸ -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    ਜੇ ((hf = CreateFileW(wold, GENERIC_WRITE, 0, ਨਿਰਾ, CREATE_NEW,
		    FILE_ATTRIBUTE_ਸਾਧਾਰਨ, ਨਿਰਾ)) == INVALID_HANDLE_ਮੁੱਲ)
	ਵਾਪਸ -5;
    ਜੇ (!CloseHandle(hf))
	ਵਾਪਸ -6;

    // rename the temp file to the new file
    ਜੇ (!MoveFileW(sz_ਆਰਜ਼ੀ_File, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(ਖਾਲੀ)MoveFileW(sz_ਆਰਜ਼ੀ_File, wold);
	ਵਾਪਸ -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(sz_ਆਰਜ਼ੀ_File);

    // finally, remove the empty old file
    ਜੇ (!DeleteFileW(wold))
	ਵਾਪਸ -8;

    ਵਾਪਸ 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    ਪੂਰਨ
mch_rename(
    ਅਚਲ ਅੱਖਰ	*pszOldFile,
    ਅਚਲ ਅੱਖਰ	*pszNewFile)
{
    WCHAR	*wold = ਨਿਰਾ;
    WCHAR	*wnew = ਨਿਰਾ;
    ਪੂਰਨ		retval = -1;

    wold = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)pszOldFile, ਨਿਰਾ);
    wnew = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)pszNewFile, ਨਿਰਾ);
    ਜੇ (wold != ਨਿਰਾ && wnew != ਨਿਰਾ)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    ਵਾਪਸ retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    ਅੱਖਰ *
default_shell(ਖਾਲੀ)
{
    ਵਾਪਸ "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    ਪੂਰਨ
mch_access(ਅੱਖਰ *n, ਪੂਰਨ p)
{
    HANDLE	hFile;
    ਪੂਰਨ		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)n, ਨਿਰਾ);
    ਜੇ (wn == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ਜੇ (mch_isdir((char_u *)n))
    {
	WCHAR ਆਰਜ਼ੀ_Name_W[MAX_ਰਸਤਾ_2 + 16] = L"";

	ਜੇ (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    ਪੂਰਨ			i;
	    WIN32_ਲੱਭੋ_DATAW    d;

	    ਲਈ (i = 0; i < MAX_ਰਸਤਾ_2 && wn[i] != 0; ++i)
		ਆਰਜ਼ੀ_Name_W[i] = wn[i];
	    ਜੇ (ਆਰਜ਼ੀ_Name_W[i - 1] != '\\' && ਆਰਜ਼ੀ_Name_W[i - 1] != '/')
		ਆਰਜ਼ੀ_Name_W[i++] = '\\';
	    ਆਰਜ਼ੀ_Name_W[i++] = '*';
	    ਆਰਜ਼ੀ_Name_W[i++] = 0;

	    hFile = ਲੱਭੋ_ਪਹਿਲਾ_File_W(ਆਰਜ਼ੀ_Name_W, &d);
	    ਜੇ (hFile == INVALID_HANDLE_ਮੁੱਲ)
		ਜਾਓ getout;
	    ਨਹੀਂਤਾਂ
		(ਖਾਲੀ)ਲੱਭੋ_Close(hFile);
	}

	ਜੇ (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    ਜੇ (!Get_ਆਰਜ਼ੀ_File_Name_W(wn, L"VIM", 0, ਆਰਜ਼ੀ_Name_W))
		ਜਾਓ getout;
	    ਨਹੀਂਤਾਂ
		DeleteFileW(ਆਰਜ਼ੀ_Name_W);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_ਬਹੁਲਕ = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_ਬਹੁਲਕ = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_ਬਹੁਲਕ, share_ਬਹੁਲਕ,
					     ਨਿਰਾ, OPEN_EXISTING, 0, ਨਿਰਾ);
	ਜੇ (hFile == INVALID_HANDLE_ਮੁੱਲ)
	    ਜਾਓ getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    ਵਾਪਸ retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    ਪੂਰਨ
mch_open(ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ flags, ਪੂਰਨ ਬਹੁਲਕ)
{
    WCHAR	*wn;
    ਪੂਰਨ		f;

    wn = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)name, ਨਿਰਾ);
    ਜੇ (wn == ਨਿਰਾ)
	ਵਾਪਸ -1;

    f = _wopen(wn, flags, ਬਹੁਲਕ);
    vim_free(wn);
    ਵਾਪਸ f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ)
{
    WCHAR	*wn, *wm;
    FILE	*f = ਨਿਰਾ;

#ਜੇ defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    ਅੱਖਰ new_ਬਹੁਲਕ = ਬਹੁਲਕ[ਲੜੀ_ਲੰਬਾਈ(ਬਹੁਲਕ) - 1];
    ਪੂਰਨ old_ਬਹੁਲਕ_2 = 0;

    _get_fmode(&old_ਬਹੁਲਕ_2);
    ਜੇ (new_ਬਹੁਲਕ == 't')
	ਸੈੱਟ_fmode(O_ਪਾਠ);
    ਨਹੀਂਤਾਂ ਜੇ (new_ਬਹੁਲਕ == 'b')
	ਸੈੱਟ_fmode(O_ਦੁਆਧਾਰੀ_2);
#ਜੇ_ਅੰਤ
    wn = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)name, ਨਿਰਾ);
    wm = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)ਬਹੁਲਕ, ਨਿਰਾ);
    ਜੇ (wn != ਨਿਰਾ && wm != ਨਿਰਾ)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#ਜੇ defined(DEBUG) && _MSC_VER >= 1400
    ਜੇ (old_ਬਹੁਲਕ_2 != 0)
	ਸੈੱਟ_fmode(old_ਬਹੁਲਕ_2);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ f;
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
    ਸਥਿਰ ਖਾਲੀ
copy_substream(HANDLE sh, ਖਾਲੀ *context, WCHAR *ਮੰਜ਼ਿਲ, WCHAR *substream, ਲੰਮਾ len)
{
    HANDLE  h_ਮੰਜ਼ਿਲ;
    WCHAR   *ਮੰਜ਼ਿਲ_name;

    ਮੰਜ਼ਿਲ_name = alloc((wcslen(ਮੰਜ਼ਿਲ) + wcslen(substream) + 1) * ਆਕਾਰ(WCHAR));
    ਜੇ (ਮੰਜ਼ਿਲ_name == ਨਿਰਾ)
	ਵਾਪਸ;

    wcscpy(ਮੰਜ਼ਿਲ_name, ਮੰਜ਼ਿਲ);
    wcscat(ਮੰਜ਼ਿਲ_name, substream);

    h_ਮੰਜ਼ਿਲ = CreateFileW(ਮੰਜ਼ਿਲ_name, GENERIC_WRITE, 0, ਨਿਰਾ, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_ਸਾਧਾਰਨ, ਨਿਰਾ);
    ਜੇ (h_ਮੰਜ਼ਿਲ != INVALID_HANDLE_ਮੁੱਲ)
    {
	ਲੰਮਾ	ਪੂਰਾ;
	DWORD	todo;
	DWORD	readcnt, written;
	ਅੱਖਰ	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	ਲਈ (ਪੂਰਾ = 0; ਪੂਰਾ < len; ਪੂਰਾ += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - ਪੂਰਾ) > ਆਕਾਰ(buf) ? ਆਕਾਰ(buf)
						       : (size_t)(len - ਪੂਰਾ));
	    ਜੇ (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(h_ਮੰਜ਼ਿਲ, buf, todo, &written, ਨਿਰਾ)
		    || written != todo)
		ਰੋਕੋ;
	}
	CloseHandle(h_ਮੰਜ਼ਿਲ);
    }

    ਮੁਕਤ(ਮੰਜ਼ਿਲ_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    ਸਥਿਰ ਖਾਲੀ
copy_infostreams(char_u *from, char_u *ਮੰਜ਼ਿਲ)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_ਧਾਰਾ_ID	sid;
    ਪੂਰਨ			headersize;
    WCHAR		streamname[MAX_ਰਸਤਾ_2];
    DWORD		readcount;
    ਖਾਲੀ		*context = ਨਿਰਾ;
    DWORD		lo, hi;
    ਪੂਰਨ			len;

    // Convert the file names to wide characters.
    fromw = enc_ਮੰਜ਼ਿਲ_utf16(from, ਨਿਰਾ);
    tow = enc_ਮੰਜ਼ਿਲ_utf16(ਮੰਜ਼ਿਲ, ਨਿਰਾ);
    ਜੇ (fromw != ਨਿਰਾ && tow != ਨਿਰਾ)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, ਨਿਰਾ,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, ਨਿਰਾ);
	ਜੇ (sh != INVALID_HANDLE_ਮੁੱਲ)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    ਲਈ (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		Zero_ਸਮ੍ਰਿਤੀ(&sid, ਆਕਾਰ(WIN32_ਧਾਰਾ_ID));
		headersize = (ਪੂਰਨ)((ਅੱਖਰ *)&sid.c_ਧਾਰਾ_Name - (ਅੱਖਰ *)&sid.dw_ਧਾਰਾ_Id);
		ਜੇ (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    ਰੋਕੋ;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		ਜੇ (sid.dw_ਧਾਰਾ_Name_ਮਾਪ > 0)
		{
		    // Read the stream name.
		    ਜੇ (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dw_ਧਾਰਾ_Name_ਮਾਪ,
					  &readcount, FALSE, FALSE, &context))
			ਰੋਕੋ;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / ਆਕਾਰ(WCHAR);
		    streamname[len] = 0;
		    ਜੇ (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (ਲੰਮਾ)sid.ਮਾਪ_2.u.ਨੀਵਾਂ_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(ਖਾਲੀ)BackupSeek(sh, sid.ਮਾਪ_2.u.ਨੀਵਾਂ_Part, sid.ਮਾਪ_2.u.ਉੱਚਾ_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (ਖਾਲੀ)BackupRead(sh, ਨਿਰਾ, 0, &readcount, TRUE, FALSE, &context);

	    CloseHandle(sh);
	}
    }
    vim_free(fromw);
    vim_free(tow);
}

/*
 * ntdll.dll definitions
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FileEaInformation   7
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਥਿਤੀ_SUCCESS
# define ਸਥਿਤੀ_SUCCESS	    ((NTSTATUS) 0x00000000L)
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ FILE_ਭਰਿਆ_EA_INFORMATION {
    ULONG  ਅਗਲਾ_ਇੰਦਰਾਜ_ਵਿੱਥ;
    UCHAR  Flags;
    UCHAR  Ea_Name_ਲੰਮਾਈ;
    USHORT Ea_ਮੁੱਲ_ਲੰਮਾਈ;
    CHAR   EaName[1];
} FILE_ਭਰਿਆ_EA_INFORMATION_2, *PFILE_ਭਰਿਆ_EA_INFORMATION;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _FILE_EA_INFORMATION_ {
    ULONG Ea_ਮਾਪ;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO
ਕਿਸਮਪਰਿਭਾਸ਼ਾ NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_ਨਕਾਬ DesiredAccess,
	POBJECT_ਗੁਣ Object_ਗੁਣ,
	PIO_ਸਥਿਤੀ_ਖੰਡ Io_ਸਥਿਤੀ_ਖੰਡ,
	ULONG ShareAccess,
	ULONG OpenOptions);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ NTSTATUS (NTAPI *Pfn_Nt_ਸੈੱਟ_Ea_File)(
	HANDLE		    FileHandle,
	PIO_ਸਥਿਤੀ_ਖੰਡ    Io_ਸਥਿਤੀ_ਖੰਡ,
	PVOID		    ਬਫਰ_2,
	ULONG		    ਲੰਮਾਈ_2);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_ਸਥਿਤੀ_ਖੰਡ Io_ਸਥਿਤੀ_ਖੰਡ,
	PVOID ਬਫਰ_2,
	ULONG ਲੰਮਾਈ_2,
	BOOLEAN Return_Single_ਇੰਦਰਾਜ,
	PVOID EaList,
	ULONG Ea_List_ਲੰਮਾਈ,
	PULONG Ea_ਸੂਚਕਾਂਕ,
	BOOLEAN Restart_ਜਾਂਚ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_ਸਥਿਤੀ_ਖੰਡ	Io_ਸਥਿਤੀ_ਖੰਡ,
	PVOID			FileInformation,
	ULONG			ਲੰਮਾਈ_2,
	FILE_INFORMATION_CLASS FileInformationClass);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR ਸਰੋਤ_String);

PfnNtOpenFile pNtOpenFile = ਨਿਰਾ;
PfnNtClose pNtClose = ਨਿਰਾ;
Pfn_Nt_ਸੈੱਟ_Ea_File p_Nt_ਸੈੱਟ_Ea_File = ਨਿਰਾ;
PfnNtQueryEaFile pNtQueryEaFile = ਨਿਰਾ;
PfnNtQueryInformationFile pNtQueryInformationFile = ਨਿਰਾ;
PfnRtlInitUnicodeString pRtlInitUnicodeString = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

/*
 * Load ntdll.dll functions.
 */
    ਸਥਿਰ BOOL
ਲੱਦੋ_ntdll(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ	loaded = -1;

    ਜੇ (loaded != -1)
	ਵਾਪਸ (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    ਜੇ (hNtdll != ਨਿਰਾ)
    {
	pNtOpenFile = (PfnNtOpenFile) Get_Proc_ਪਤਾ(hNtdll, "NtOpenFile");
	pNtClose = (PfnNtClose) Get_Proc_ਪਤਾ(hNtdll, "NtClose");
	p_Nt_ਸੈੱਟ_Ea_File = (Pfn_Nt_ਸੈੱਟ_Ea_File)
	    Get_Proc_ਪਤਾ(hNtdll, "NtSetEaFile");
	pNtQueryEaFile = (PfnNtQueryEaFile)
	    Get_Proc_ਪਤਾ(hNtdll, "NtQueryEaFile");
	pNtQueryInformationFile = (PfnNtQueryInformationFile)
	    Get_Proc_ਪਤਾ(hNtdll, "NtQueryInformationFile");
	pRtlInitUnicodeString = (PfnRtlInitUnicodeString)
	    Get_Proc_ਪਤਾ(hNtdll, "RtlInitUnicodeString");
    }
    ਜੇ (pNtOpenFile == ਨਿਰਾ
	    || pNtClose == ਨਿਰਾ
	    || p_Nt_ਸੈੱਟ_Ea_File == ਨਿਰਾ
	    || pNtQueryEaFile == ਨਿਰਾ
	    || pNtQueryInformationFile == ਨਿਰਾ
	    || pRtlInitUnicodeString == ਨਿਰਾ)
	loaded = FALSE;
    ਨਹੀਂਤਾਂ
	loaded = TRUE;
    ਵਾਪਸ (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    ਸਥਿਰ ਖਾਲੀ
copy_extattr(char_u *from, char_u *ਮੰਜ਼ਿਲ)
{
    char_u		    *fromf = ਨਿਰਾ;
    char_u		    *tof = ਨਿਰਾ;
    WCHAR		    *fromw = ਨਿਰਾ;
    WCHAR		    *tow = ਨਿਰਾ;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_ਗੁਣ	    oa;
    IO_ਸਥਿਤੀ_ਖੰਡ	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    ਖਾਲੀ		    *ea = ਨਿਰਾ;

    ਜੇ (!ਲੱਦੋ_ntdll())
	ਵਾਪਸ;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(ਮੰਜ਼ਿਲ) + 5);
    ਜੇ (fromf == ਨਿਰਾ || tof == ਨਿਰਾ)
	ਜਾਓ theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), ਮੰਜ਼ਿਲ);

    // Convert the names to wide characters.
    fromw = enc_ਮੰਜ਼ਿਲ_utf16(fromf, ਨਿਰਾ);
    tow = enc_ਮੰਜ਼ਿਲ_utf16(tof, ਨਿਰਾ);
    ਜੇ (fromw == ਨਿਰਾ || tow == ਨਿਰਾ)
	ਜਾਓ theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    Initialize_Object_ਗੁਣ(&oa, &u, 0, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != ਸਥਿਤੀ_SUCCESS)
	ਜਾਓ theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, ਆਕਾਰ(eainfo),
	    FileEaInformation);
    ਜੇ (eainfo.Ea_ਮਾਪ != 0)
    {
	ea = alloc(eainfo.Ea_ਮਾਪ);
	ਜੇ (ea != ਨਿਰਾ)
	{
	    ਜੇ (pNtQueryEaFile(h, &iosb, ea, eainfo.Ea_ਮਾਪ, FALSE,
			ਨਿਰਾ, 0, ਨਿਰਾ, TRUE) != ਸਥਿਤੀ_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    ਜੇ (ea != ਨਿਰਾ)
    {
	pRtlInitUnicodeString(&u, tow);
	Initialize_Object_ਗੁਣ(&oa, &u, 0, ਨਿਰਾ, ਨਿਰਾ);
	ਜੇ (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != ਸਥਿਤੀ_SUCCESS)
	    ਜਾਓ theend;

	p_Nt_ਸੈੱਟ_Ea_File(h, &iosb, ea, eainfo.Ea_ਮਾਪ);
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
    ਪੂਰਨ
mch_copy_file_attribute(char_u *from, char_u *ਮੰਜ਼ਿਲ)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, ਮੰਜ਼ਿਲ);
    copy_extattr(from, ਮੰਜ਼ਿਲ);
    ਵਾਪਸ 0;
}


/*
 * The command line arguments in UTF-16
 */
ਸਥਿਰ ਪੂਰਨ	n_ਦਲੀਲਾਂ_W = 0;
ਸਥਿਰ LPWSTR	*ArglistW = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	global_argc = 0;
ਸਥਿਰ ਅੱਖਰ	**global_argv;

ਸਥਿਰ ਪੂਰਨ	ਵਰਤਿਆ_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
ਸਥਿਰ ਪੂਰਨ	*ਵਰਤਿਆ_file_indexes = ਨਿਰਾ; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
ਸਥਿਰ ਪੂਰਨ	ਵਰਤਿਆ_file_ਗਿਣਤੀ = 0;	// nr of entries in used_file_indexes
ਸਥਿਰ ਪੂਰਨ	ਵਰਤਿਆ_file_literal = FALSE;  // take file names literally
ਸਥਿਰ ਪੂਰਨ	ਵਰਤਿਆ_file_ਭਰਿਆ_ਰਸਤਾ = FALSE;  // file name was full path
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
ਸਥਿਰ ਪੂਰਨ	ਵਰਤਿਆ_file_diff_ਬਹੁਲਕ = FALSE;  // file name was with diff mode
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ	ਵਰਤਿਆ_alist_ਗਿਣਤੀ = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    ਪੂਰਨ
get_cmd_ਦਲੀਲਾਂ_W(ਅੱਖਰ ***argvp)
{
    ਅੱਖਰ	**argv = ਨਿਰਾ;
    ਪੂਰਨ		argc = 0;
    ਪੂਰਨ		i;

    free_cmd_ਦਲੀਲਾਂ_W();
    ArglistW = Command_ਸਤਰ_ਮੰਜ਼ਿਲ_Argv_W(Get_Command_ਸਤਰ_W(), &n_ਦਲੀਲਾਂ_W);
    ਜੇ (ArglistW != ਨਿਰਾ)
    {
	argv = alloc((n_ਦਲੀਲਾਂ_W + 1) * ਆਕਾਰ(ਅੱਖਰ *));
	ਜੇ (argv != ਨਿਰਾ)
	{
	    argc = n_ਦਲੀਲਾਂ_W;
	    argv[argc] = ਨਿਰਾ;
	    ਲਈ (i = 0; i < argc; ++i)
	    {
		ਪੂਰਨ	len;

		// Convert each Unicode argument to UTF-8.
		Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte_alloc(CP_UTF8, 0,
				ArglistW[i], (ਪੂਰਨ)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		ਜੇ (argv[i] == ਨਿਰਾ)
		{
		    // Out of memory, clear everything.
		    ਜਦੋਂ (i > 0)
			ਮੁਕਤ(argv[--i]);
		    ਮੁਕਤ(argv);
		    argv = ਨਿਰਾ;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    ਜੇ (argc > 0)
    {
	ਜੇ (ਵਰਤਿਆ_file_indexes != ਨਿਰਾ)
	    ਮੁਕਤ(ਵਰਤਿਆ_file_indexes);
	ਵਰਤਿਆ_file_indexes = alloc(argc * ਆਕਾਰ(ਪੂਰਨ));
    }

    ਜੇ (argvp != ਨਿਰਾ)
	*argvp = argv;
    ਵਾਪਸ argc;
}

    ਖਾਲੀ
free_cmd_ਦਲੀਲਾਂ_W(ਖਾਲੀ)
{
    ਜੇ (ArglistW == ਨਿਰਾ)
	ਵਾਪਸ;

    GlobalFree(ArglistW);
    ArglistW = ਨਿਰਾ;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    ਖਾਲੀ
ਵਰਤਿਆ_file_arg(ਅੱਖਰ *name, ਪੂਰਨ literal, ਪੂਰਨ ਭਰਿਆ_ਰਸਤਾ, ਪੂਰਨ diff_ਬਹੁਲਕ)
{
    ਪੂਰਨ		i;

    ਜੇ (ਵਰਤਿਆ_file_indexes == ਨਿਰਾ)
	ਵਾਪਸ;
    ਲਈ (i = ਵਰਤਿਆ_file_argc + 1; i < global_argc; ++i)
	ਜੇ (STRCMP(global_argv[i], name) == 0)
	{
	    ਵਰਤਿਆ_file_argc = i;
	    ਵਰਤਿਆ_file_indexes[ਵਰਤਿਆ_file_ਗਿਣਤੀ++] = i;
	    ਰੋਕੋ;
	}
    ਵਰਤਿਆ_file_literal = literal;
    ਵਰਤਿਆ_file_ਭਰਿਆ_ਰਸਤਾ = ਭਰਿਆ_ਰਸਤਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਵਰਤਿਆ_file_diff_ਬਹੁਲਕ = diff_ਬਹੁਲਕ;
#ਜੇ_ਅੰਤ
}

/*
 * Remember the length of the argument list as it was.  If it changes then we
 * leave it alone when 'encoding' is set.
 */
    ਖਾਲੀ
ਸੈੱਟ_alist_ਗਿਣਤੀ(ਖਾਲੀ)
{
    ਵਰਤਿਆ_alist_ਗਿਣਤੀ = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    ਖਾਲੀ
fix_arg_enc(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		idx;
    char_u	*str;
    ਪੂਰਨ		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    ਜੇ (global_argc != n_ਦਲੀਲਾਂ_W
	    || ArglistW == ਨਿਰਾ
	    || ਵਰਤਿਆ_file_indexes == ਨਿਰਾ
	    || ਵਰਤਿਆ_file_ਗਿਣਤੀ == 0
	    || ਵਰਤਿਆ_alist_ਗਿਣਤੀ != GARGCOUNT)
	ਵਾਪਸ;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(ਪੂਰਨ, GARGCOUNT);
    ਜੇ (fnum_list == ਨਿਰਾ)
	ਵਾਪਸ;		// out of memory
    ਲਈ (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    ਜੇ (ga_grow(&global_alist.al_ga, ਵਰਤਿਆ_file_ਗਿਣਤੀ) == FAIL)
	ਵਾਪਸ;		// out of memory

    ਲਈ (i = 0; i < ਵਰਤਿਆ_file_ਗਿਣਤੀ; ++i)
    {
	idx = ਵਰਤਿਆ_file_indexes[i];
	str = utf16_ਮੰਜ਼ਿਲ_enc(ArglistW[idx], ਨਿਰਾ);
	ਜੇ (str != ਨਿਰਾ)
	{
	    ਪੂਰਨ literal = ਵਰਤਿਆ_file_literal;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    ਜੇ (ਵਰਤਿਆ_file_diff_ਬਹੁਲਕ && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *ਪੂਛ;
		string_T    ret;

		ਪੂਛ = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), ਪੂਛ, STRLEN(ਪੂਛ), TRUE, &ret);
		ਜੇ (ret.string != ਨਿਰਾ)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#ਜੇ_ਅੰਤ
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    ਜੇ (ਵਰਤਿਆ_file_literal)
		buf_ਸੈੱਟ_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    ਜੇ (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		ਜੇ (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_ਜੋੜੋ(&global_alist, str, literal ? 2 : 0);
	}
    }

    ਜੇ (!ਵਰਤਿਆ_file_literal)
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
	alist_expand(fnum_list, ਵਰਤਿਆ_alist_ਗਿਣਤੀ);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    ਜੇ (curwin->w_arg_idx == 0 && curbuf->b_fname == ਨਿਰਾ)
    {
	do_cmdline_cmd((char_u *)":rewind");
	ਜੇ (GARGCOUNT == 1 && ਵਰਤਿਆ_file_ਭਰਿਆ_ਰਸਤਾ
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    ਆਖਰੀ_chdir_reason = "drop";
    }

    ਸੈੱਟ_alist_ਗਿਣਤੀ();
}

    ਪੂਰਨ
mch_setenv(ਅੱਖਰ *var, ਅੱਖਰ *ਮੁੱਲ, ਪੂਰਨ x ਅਣਵਰਤਿਆ)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(ਮੁੱਲ) + 2);
    ਜੇ (envbuf == ਨਿਰਾ)
	ਵਾਪਸ -1;

    sprintf((ਅੱਖਰ *)envbuf, "%s=%s", var, ਮੁੱਲ);

    p = enc_ਮੰਜ਼ਿਲ_utf16(envbuf, ਨਿਰਾ);

    vim_free(envbuf);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ -1;
    _wputenv(p);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ libintl_wputenv
    libintl_wputenv(p);
#ਜੇ_ਅੰਤ
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    ਵਾਪਸ 0;
}

/*
 * Support for 256 colors and 24-bit colors was added in Windows 10
 * version 1703 (Creators update).
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTP_ਪਹਿਲਾ_SUPPORT_BUILD	    MAKE_VER(10, 0, 15063)

/*
 * Support for pseudo-console (ConPTY) was added in windows 10
 * version 1809 (October 2018 update).
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONPTY_ਪਹਿਲਾ_SUPPORT_BUILD  MAKE_VER(10, 0, 17763)

/*
 * ConPTY differences between versions, need different logic.
 * version 1903 (May 2019 update).
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONPTY_1903_BUILD	    MAKE_VER(10, 0, 18362)

/*
 * version 1909 (November 2019 update).
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONPTY_1909_BUILD	    MAKE_VER(10, 0, 18363)

/*
 * Stay ahead of the next update, and when it's done, fix this.
 * version ? (2020 update, temporarily use the build number of insider preview)
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONPTY_ਅਗਲਾ_UPDATE_BUILD    MAKE_VER(10, 0, 19587)

/*
 * Confirm until this version.  Also the logic changes.
 * insider preview.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONPTY_INSIDER_BUILD	    MAKE_VER(10, 0, 18995)

/*
 * Make conpty default on Windows 11
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONPTY_STABLE_BUILD	    MAKE_VER(10, 0, 22000)
// Notes:
// Win 10 22H2 Final is build 19045, its conpty is widely used.
// Strangely, 19045 is newer but is a lower build number than the 2020 insider
// preview which had a build 19587.  And, not sure how stable that was?
// Win Server 2022 (May 10, 2022) is build 20348, its conpty is widely used.
// Win 11 starts from build 22000, even though the major version says 10!

    ਸਥਿਰ ਖਾਲੀ
vtp_flag_init(ਖਾਲੀ)
{
#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   ਬਹੁਲਕ;
    HANDLE  ਬਾਹਰਲਾ;

# ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
    {
	ਬਾਹਰਲਾ = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_ਸੰਸਕਰਣ >= VTP_ਪਹਿਲਾ_SUPPORT_BUILD) ? 1 : 0;
	Get_Console_ਬਹੁਲਕ(ਬਾਹਰਲਾ, &ਬਹੁਲਕ);
	ਬਹੁਲਕ |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	ਜੇ (ਸੈੱਟ_Console_ਬਹੁਲਕ(ਬਾਹਰਲਾ, ਬਹੁਲਕ) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_ਬਫਰ = win_ਸੰਸਕਰਣ >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#ਜੇ_ਅੰਤ

    ਜੇ (win_ਸੰਸਕਰਣ >= CONPTY_ਪਹਿਲਾ_SUPPORT_BUILD)
	conpty_working = 1;
    ਜੇ (win_ਸੰਸਕਰਣ >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    ਜੇ (win_ਸੰਸਕਰਣ <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    ਜੇ (win_ਸੰਸਕਰਣ <= CONPTY_1909_BUILD)
	conpty_type = 2;
    ਜੇ (win_ਸੰਸਕਰਣ <= CONPTY_1903_BUILD)
	conpty_type = 2;
    ਜੇ (win_ਸੰਸਕਰਣ < CONPTY_ਪਹਿਲਾ_SUPPORT_BUILD)
	conpty_type = 1;

    ਜੇ (win_ਸੰਸਕਰਣ >= CONPTY_ਅਗਲਾ_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ਸਥਿਰ ਖਾਲੀ
vtp_init(ਖਾਲੀ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_ਬਫਰ_INFOEX csbi;
    csbi.cb_ਮਾਪ = ਆਕਾਰ(csbi);
    Get_Console_Screen_ਬਫਰ_Info_Ex(g_h_Con_ਬਾਹਰਲਾ, &csbi);
    save_console_bg_rgb = (guicolor_T)csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_bg];
    save_console_fg_rgb = (guicolor_T)csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_fg];
    store_console_bg_rgb = save_console_bg_rgb;
    store_console_fg_rgb = save_console_fg_rgb;

    COLORREF bg;
    bg = (COLORREF)csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_bg];
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);
    default_console_color_bg = bg;

    COLORREF fg;
    fg = (COLORREF)csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_fg];
    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    default_console_color_fg = fg;
# endif
    ਸੈੱਟ_console_color_rgb();
}

    ਸਥਿਰ ਖਾਲੀ
vtp_exit(ਖਾਲੀ)
{
    restore_console_color_rgb();
}

    ਪੂਰਨ
vtp_printf(
    ਅੱਖਰ *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   ਨਤੀਜਾ;
    ਪੂਰਨ	    len;

    ਜੇ (silent_ਬਹੁਲਕ)
	ਵਾਪਸ 0;

    va_start(list, format);
    len = vim_vsnprintf((ਅੱਖਰ *)buf, 100, (ਅੱਖਰ *)format, list);
    va_end(list);
    WriteConsoleA(g_h_Con_ਬਾਹਰਲਾ, buf, (DWORD)len, &ਨਤੀਜਾ, ਨਿਰਾ);
    ਵਾਪਸ (ਪੂਰਨ)ਨਤੀਜਾ;
}

    ਸਥਿਰ ਖਾਲੀ
vtp_sgr_bulk(
    ਪੂਰਨ arg)
{
    ਪੂਰਨ ਦਲੀਲਾਂ[1];

    ਦਲੀਲਾਂ[0] = arg;
    vtp_sgr_bulks(1, ਦਲੀਲਾਂ);
}

# define FAST256(x) \
    ਜੇ ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    ਹਾਲਤ x: \
	FAST256(newargs[x - 1]);

    ਸਥਿਰ ਖਾਲੀ
vtp_sgr_bulks(
    ਪੂਰਨ argc,
    ਪੂਰਨ *ਦਲੀਲਾਂ)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    ਪੂਰਨ	    ਵਿੱਚ, ਬਾਹਰਲਾ;
    ਪੂਰਨ	    newargs[16];
    ਸਥਿਰ ਪੂਰਨ sgrfgr = -1, sgrfgg, sgrfgb;
    ਸਥਿਰ ਪੂਰਨ sgrbgr = -1, sgrbgg, sgrbgb;

    ਜੇ (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	ਵਾਪਸ;
    }

    ਵਿੱਚ = ਬਾਹਰਲਾ = 0;
    ਜਦੋਂ (ਵਿੱਚ < argc)
    {
	ਪੂਰਨ s = ਦਲੀਲਾਂ[ਵਿੱਚ];
	ਪੂਰਨ copylen = 1;

	ਜੇ (s == 38)
	{
	    ਜੇ (argc - ਵਿੱਚ >= 5 && ਦਲੀਲਾਂ[ਵਿੱਚ + 1] == 2)
	    {
		ਜੇ (sgrfgr == ਦਲੀਲਾਂ[ਵਿੱਚ + 2] && sgrfgg == ਦਲੀਲਾਂ[ਵਿੱਚ + 3]
						     && sgrfgb == ਦਲੀਲਾਂ[ਵਿੱਚ + 4])
		{
		    ਵਿੱਚ += 5;
		    copylen = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    sgrfgr = ਦਲੀਲਾਂ[ਵਿੱਚ + 2];
		    sgrfgg = ਦਲੀਲਾਂ[ਵਿੱਚ + 3];
		    sgrfgb = ਦਲੀਲਾਂ[ਵਿੱਚ + 4];
		    copylen = 5;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (argc - ਵਿੱਚ >= 3 && ਦਲੀਲਾਂ[ਵਿੱਚ + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (s == 48)
	{
	    ਜੇ (argc - ਵਿੱਚ >= 5 && ਦਲੀਲਾਂ[ਵਿੱਚ + 1] == 2)
	    {
		ਜੇ (sgrbgr == ਦਲੀਲਾਂ[ਵਿੱਚ + 2] && sgrbgg == ਦਲੀਲਾਂ[ਵਿੱਚ + 3]
						     && sgrbgb == ਦਲੀਲਾਂ[ਵਿੱਚ + 4])
		{
		    ਵਿੱਚ += 5;
		    copylen = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    sgrbgr = ਦਲੀਲਾਂ[ਵਿੱਚ + 2];
		    sgrbgg = ਦਲੀਲਾਂ[ਵਿੱਚ + 3];
		    sgrbgb = ਦਲੀਲਾਂ[ਵਿੱਚ + 4];
		    copylen = 5;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (argc - ਵਿੱਚ >= 3 && ਦਲੀਲਾਂ[ਵਿੱਚ + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (30 <= s && s <= 39)
	    sgrfgr = -1;
	ਨਹੀਂਤਾਂ ਜੇ (90 <= s && s <= 97)
	    sgrfgr = -1;
	ਨਹੀਂਤਾਂ ਜੇ (40 <= s && s <= 49)
	    sgrbgr = -1;
	ਨਹੀਂਤਾਂ ਜੇ (100 <= s && s <= 107)
	    sgrbgr = -1;
	ਨਹੀਂਤਾਂ ਜੇ (s == 0)
	    sgrfgr = sgrbgr = -1;

	ਜਦੋਂ (copylen--)
	    newargs[ਬਾਹਰਲਾ++] = ਦਲੀਲਾਂ[ਵਿੱਚ++];
    }

    p = &buf[ਆਕਾਰ(buf) - 1];
    *p-- = 'm';

    ਚੋਣ (ਬਾਹਰਲਾ)
    {
	ਪੂਰਨ	n, m;
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
	WriteConsoleA(g_h_Con_ਬਾਹਰਲਾ, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, ਨਿਰਾ);
    ਮੂਲ:
	ਰੋਕੋ;
    }
}

    ਸਥਿਰ ਖਾਲੀ
wt_init(ਖਾਲੀ)
{
    wt_working = mch_getenv("WT_SESSION") != ਨਿਰਾ;
}

# ifdef FEAT_TERMGUICOLORS
    ਸਥਿਰ ਪੂਰਨ
ctermtoxterm(
    ਪੂਰਨ cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    ਵਾਪਸ (((ਪੂਰਨ)r << 16) | ((ਪੂਰਨ)g << 8) | (ਪੂਰਨ)b);
}
# endif

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_console_color_rgb(ਖਾਲੀ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_ਬਫਰ_INFOEX csbi;
    guicolor_T	fg, bg;
    ਪੂਰਨ		ctermfg, ctermbg;

    ਜੇ (!vtp_working)
	ਵਾਪਸ;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    ਜੇ (p_tgc || t_colors >= 256)
    {
	ਪਦ_fg_rgb_color(fg);
	ਪਦ_bg_rgb_color(bg);
	ਵਾਪਸ;
    }

    ਜੇ (use_alternate_screen_ਬਫਰ)
	ਵਾਪਸ;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cb_ਮਾਪ = ਆਕਾਰ(csbi);
    Get_Console_Screen_ਬਫਰ_Info_Ex(g_h_Con_ਬਾਹਰਲਾ, &csbi);

    csbi.cb_ਮਾਪ = ਆਕਾਰ(csbi);
    csbi.sr_ਖਿੜਕੀ.ਸੱਜਾ_3 += 1;
    csbi.sr_ਖਿੜਕੀ.Bottom += 1;
    store_console_bg_rgb = csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_bg];
    store_console_fg_rgb = csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_fg];
    csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_bg] = (COLORREF)bg;
    csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_fg] = (COLORREF)fg;
    ਸੈੱਟ_Console_Screen_ਬਫਰ_Info_Ex(g_h_Con_ਬਾਹਰਲਾ, &csbi);
# endif
}

# ਜੇ defined(FEAT_TERMGUICOLORS)
    ਖਾਲੀ
get_default_console_color(
    ਪੂਰਨ *cterm_fg,
    ਪੂਰਨ *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    ਪੂਰਨ id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    ਪੂਰਨ ctermfg = 0;
    ਪੂਰਨ ctermbg = 0;
    ਪੂਰਨ dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    ਜੇ (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    ਜੇ (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	ਜੇ (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	ਜੇ (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	ਨਹੀਂਤਾਂ
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_ਸਾਧਾਰਨ_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    ਜੇ (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	ਜੇ (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	ਜੇ (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	ਨਹੀਂਤਾਂ
	    guibg = USE_WT ? INVALCOLOR : default_console_color_bg;
	cterm_ਸਾਧਾਰਨ_bg_gui_color = guibg;
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
    ਸਥਿਰ ਖਾਲੀ
reset_console_color_rgb(ਖਾਲੀ)
{
# ifdef FEAT_TERMGUICOLORS
    ਜੇ (use_alternate_screen_ਬਫਰ)
	ਵਾਪਸ;

    CONSOLE_SCREEN_ਬਫਰ_INFOEX csbi;

    csbi.cb_ਮਾਪ = ਆਕਾਰ(csbi);
    Get_Console_Screen_ਬਫਰ_Info_Ex(g_h_Con_ਬਾਹਰਲਾ, &csbi);

    csbi.cb_ਮਾਪ = ਆਕਾਰ(csbi);
    csbi.sr_ਖਿੜਕੀ.ਸੱਜਾ_3 += 1;
    csbi.sr_ਖਿੜਕੀ.Bottom += 1;
    csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_bg] = (COLORREF)store_console_bg_rgb;
    csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_fg] = (COLORREF)store_console_fg_rgb;
    ਸੈੱਟ_Console_Screen_ਬਫਰ_Info_Ex(g_h_Con_ਬਾਹਰਲਾ, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    ਸਥਿਰ ਖਾਲੀ
restore_console_color_rgb(ਖਾਲੀ)
{
# ifdef FEAT_TERMGUICOLORS
    ਜੇ (use_alternate_screen_ਬਫਰ)
	ਵਾਪਸ;

    CONSOLE_SCREEN_ਬਫਰ_INFOEX csbi;

    csbi.cb_ਮਾਪ = ਆਕਾਰ(csbi);
    Get_Console_Screen_ਬਫਰ_Info_Ex(g_h_Con_ਬਾਹਰਲਾ, &csbi);

    csbi.cb_ਮਾਪ = ਆਕਾਰ(csbi);
    csbi.sr_ਖਿੜਕੀ.ਸੱਜਾ_3 += 1;
    csbi.sr_ਖਿੜਕੀ.Bottom += 1;
    csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_bg] = (COLORREF)save_console_bg_rgb;
    csbi.Color_ਸਾਰਣੀ[g_color_ਸੂਚਕਾਂਕ_fg] = (COLORREF)save_console_fg_rgb;
    ਸੈੱਟ_Console_Screen_ਬਫਰ_Info_Ex(g_h_Con_ਬਾਹਰਲਾ, &csbi);
# endif
}

    ਖਾਲੀ
control_console_color_rgb(ਖਾਲੀ)
{
    ਜੇ (vtp_working)
	ਸੈੱਟ_console_color_rgb();
    ਨਹੀਂਤਾਂ
	reset_console_color_rgb();
}

    ਪੂਰਨ
use_vtp(ਖਾਲੀ)
{
    ਵਾਪਸ USE_VTP;
}

    ਪੂਰਨ
is_ਪਦ_win32(ਖਾਲੀ)
{
    ਵਾਪਸ T_NAME != ਨਿਰਾ && STRCMP(T_NAME, "win32") == 0;
}

    ਪੂਰਨ
has_vtp_working(ਖਾਲੀ)
{
    ਵਾਪਸ vtp_working;
}

#ਜੇ_ਅੰਤ

    ਪੂਰਨ
has_conpty_working(ਖਾਲੀ)
{
    ਵਾਪਸ conpty_working;
}

    ਪੂਰਨ
get_conpty_type(ਖਾਲੀ)
{
    ਵਾਪਸ conpty_type;
}

    ਪੂਰਨ
is_conpty_stable(ਖਾਲੀ)
{
    ਵਾਪਸ conpty_stable;
}

    ਪੂਰਨ
get_conpty_fix_type(ਖਾਲੀ)
{
    ਵਾਪਸ conpty_fix_type;
}

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ਖਾਲੀ
resize_console_buf(ਖਾਲੀ)
{
    ਜੇ (use_alternate_screen_ਬਫਰ)
	ਵਾਪਸ;

    CONSOLE_SCREEN_ਬਫਰ_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    ਜੇ (!Get_Console_Screen_ਬਫਰ_Info(g_h_Con_ਬਾਹਰਲਾ, &csbi))
	ਵਾਪਸ;

    coord.X = SRWIDTH(csbi.sr_ਖਿੜਕੀ);
    coord.Y = SRHEIGHT(csbi.sr_ਖਿੜਕੀ);
    ਸੈੱਟ_Console_Screen_ਬਫਰ_ਮਾਪ(g_h_Con_ਬਾਹਰਲਾ, coord);

    newsize.ਖੱਬਾ_3 = 0;
    newsize.ਸਿਖਰ_2 = 0;
    newsize.ਸੱਜਾ_3 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    ਸੈੱਟ_Console_ਖਿੜਕੀ_Info(g_h_Con_ਬਾਹਰਲਾ, TRUE, &newsize);

    ਸੈੱਟ_Console_Screen_ਬਫਰ_ਮਾਪ(g_h_Con_ਬਾਹਰਲਾ, coord);
}
#ਜੇ_ਅੰਤ

    ਅੱਖਰ *
GetWin32Error(ਖਾਲੀ)
{
    ਸਥਿਰ ਅੱਖਰ	*oldmsg = ਨਿਰਾ;
    ਅੱਖਰ	*acp_ਸੁਨੇਹਾ = ਨਿਰਾ;
    DWORD	acp_len;
    ਅੱਖਰ	*enc_ਸੁਨੇਹਾ = ਨਿਰਾ;
    ਪੂਰਨ		enc_len = 0;

    // get formatted message from OS
    acp_len = Format_ਸੁਨੇਹਾ(
	    FORMAT_ਸੁਨੇਹਾ_ALLOCATE_ਬਫਰ|FORMAT_ਸੁਨੇਹਾ_FROM_SYSTEM,
	    ਨਿਰਾ, Get_ਆਖਰੀ_Error(), 0, (LPSTR)&acp_ਸੁਨੇਹਾ, 0, ਨਿਰਾ);
    ਜੇ (acp_len == 0 || acp_ਸੁਨੇਹਾ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // clean oldmsg if remained.
    ਜੇ (oldmsg != ਨਿਰਾ)
    {
	vim_free(oldmsg);
	oldmsg = ਨਿਰਾ;
    }

    acp_ਮੰਜ਼ਿਲ_enc((char_u *)acp_ਸੁਨੇਹਾ, (ਪੂਰਨ)acp_len, (char_u **)&enc_ਸੁਨੇਹਾ, &enc_len);
    LocalFree(acp_ਸੁਨੇਹਾ);
    ਜੇ (enc_ਸੁਨੇਹਾ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // remove trailing \r\n
    ਅੱਖਰ *pcrlf = ਲੜੀ_ਖੋਜ(enc_ਸੁਨੇਹਾ, "\r\n");
    ਜੇ (pcrlf != ਨਿਰਾ)
	*pcrlf = NUL;

    oldmsg = enc_ਸੁਨੇਹਾ;
    ਵਾਪਸ enc_ਸੁਨੇਹਾ;
}

#ਜੇ defined(FEAT_RELTIME)
ਸਥਿਰ HANDLE   timer_handle;
ਸਥਿਰ ਪੂਰਨ      timer_ਸਰਗਰਮ = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
ਸਥਿਰ sig_atomic_t timeout_flags[2];
ਸਥਿਰ ਪੂਰਨ	    timeout_flag_idx = 0;
ਸਥਿਰ sig_atomic_t *timeout_flag = &timeout_flags[0];


    ਸਥਿਰ ਖਾਲੀ CALLBACK
ਸੈੱਟ_flag(ਖਾਲੀ *param, BOOLEAN unused2 ਅਣਵਰਤਿਆ)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    ਖਾਲੀ
stop_timeout(ਖਾਲੀ)
{
    ਜੇ (timer_ਸਰਗਰਮ)
    {
	BOOL ret = Delete_Timer_ਕਤਾਰ_Timer(ਨਿਰਾ, timer_handle, ਨਿਰਾ);
	timer_ਸਰਗਰਮ = FALSE;
	ਜੇ (!ret && Get_ਆਖਰੀ_Error() != ERROR_IO_PENDING)
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
    ਅਸਥਿਰ sig_atomic_t *
ਸ਼ੁਰੂ_timeout(ਲੰਮਾ msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = Create_Timer_ਕਤਾਰ_Timer(
	    &timer_handle, ਨਿਰਾ, ਸੈੱਟ_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    ਜੇ (!ret)
    {
	semsg(_(e_could_not_ਸੈੱਟ_timeout_str), GetWin32Error());
    }
    ਨਹੀਂਤਾਂ
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_ਸਰਗਰਮ = TRUE;
	*timeout_flag = FALSE;
    }
    ਵਾਪਸ timeout_flag;
}
#ਜੇ_ਅੰਤ
