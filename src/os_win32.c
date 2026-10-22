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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
# include "if_mzsch.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <limits.h>

#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <winternl.h>
#ಅನ್ತರ್ಭಾವ <direct.h>

#ಆದರೆ !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
# include <tlhelp32.h>
#ಆದರೆ_ಅಂತ್ಯ

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MCH_WRITE_DUMP
FILE* fdDump = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
ಸ್ಥಿರ HANDLE g_h_Con_ಒಳ  = INVALID_HANDLE_ಮೌಲ್ಯ;
ಸ್ಥಿರ HANDLE g_h_Con_ಹೊರಹೋಗು = INVALID_HANDLE_ಮೌಲ್ಯ;

// Win32 Screen buffer,coordinate,console I/O information
ಸ್ಥಿರ SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
ಸ್ಥಿರ COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
ಸ್ಥಿರ ಶಬ್ದ_2  g_attrDefault = 7;  // lightgray text on black background
ಸ್ಥಿರ ಶಬ್ದ_2  g_attrCurrent;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_fForceExit = FALSE;    // set when forcefully exiting

ಸ್ಥಿರ ಶೂನ್ಯ scroll(ಚಿಹ್ನೆರಹಿತ c_ಸಾಲುಗಳು);
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_scroll_region(ಚಿಹ್ನೆರಹಿತ ಎಡ_2, ಚಿಹ್ನೆರಹಿತ ತುದಿ,
			      ಚಿಹ್ನೆರಹಿತ ಬಲ_2, ಚಿಹ್ನೆರಹಿತ bottom);
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_scroll_region_tb(ಚಿಹ್ನೆರಹಿತ ತುದಿ, ಚಿಹ್ನೆರಹಿತ bottom);
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_scroll_region_lr(ಚಿಹ್ನೆರಹಿತ ಎಡ_2, ಚಿಹ್ನೆರಹಿತ ಬಲ_2);
ಸ್ಥಿರ ಶೂನ್ಯ ಸೇರಿಸು_ಸಾಲುಗಳು(ಚಿಹ್ನೆರಹಿತ c_ಸಾಲುಗಳು);
ಸ್ಥಿರ ಶೂನ್ಯ delete_ಸಾಲುಗಳು(ಚಿಹ್ನೆರಹಿತ c_ಸಾಲುಗಳು);
ಸ್ಥಿರ ಶೂನ್ಯ gotoxy(ಚಿಹ್ನೆರಹಿತ x, ಚಿಹ್ನೆರಹಿತ y);
ಸ್ಥಿರ ಶೂನ್ಯ standout(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_cursor_visible = TRUE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್_ಗಂಟು_S
{
    ಒಳಹರಿವು_ದಾಖಲೆ ir;
    ರಚನೆ ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್_ಗಂಟು_S *ಮುಂದಿನ;
} ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್_ಗಂಟು_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್_S
{
    ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್_ಗಂಟು_T *ತಲೆ;
    ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್_ಗಂಟು_T *ಬಾಲ;
    ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ;
} ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್_T;
ಸ್ಥಿರ ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್_T ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್(ಒಳಹರಿವು_ದಾಖಲೆ* irEvents, ಪೂರ್ಣಾಂಕ n_Max_ಉದ್ದಳತೆ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ write_ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್(ಒಳಹರಿವು_ದಾಖಲೆ* irEvents, ಪೂರ್ಣಾಂಕ n_ಉದ್ದಳತೆ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_dont_use_vimrun = TRUE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ need_vimrun_warning = FALSE;
ಸ್ಥಿರ string_T vimrun_ಮಾರ್ಗ = {(char_u *)"vimrun ", 7};
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vimrun_ಮಾರ್ಗ_allocated = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ win32_getattrs(char_u *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ win32_setattrs(char_u *name, ಪೂರ್ಣಾಂಕ attrs);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ win32_ಗಣ_archive(char_u *name);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conpty_working = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conpty_type = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conpty_stable = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conpty_fix_type = 0;
ಸ್ಥಿರ ಶೂನ್ಯ vtp_flag_init(ಶೂನ್ಯ);

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vtp_working = 0;
ಸ್ಥಿರ ಶೂನ್ಯ vtp_init(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ vtp_exit(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ vtp_sgr_bulk(ಪೂರ್ಣಾಂಕ arg);
ಸ್ಥಿರ ಶೂನ್ಯ vtp_sgr_bulks(ಪೂರ್ಣಾಂಕ argc, ಪೂರ್ಣಾಂಕ *argv);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ wt_working = 0;
ಸ್ಥಿರ ಶೂನ್ಯ wt_init(ಶೂನ್ಯ);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_color_ಸೂಚ್ಯಂಕ_bg = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_color_ಸೂಚ್ಯಂಕ_fg = 7;

# ifdef FEAT_TERMGUICOLORS
ಸ್ಥಿರ guicolor_T save_console_bg_rgb;
ಸ್ಥಿರ guicolor_T save_console_fg_rgb;
ಸ್ಥಿರ guicolor_T store_console_bg_rgb;
ಸ್ಥಿರ guicolor_T store_console_fg_rgb;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ default_console_color_bg = 0x000000; // black
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_ಪದ_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# ಇಲ್ಲದಿದ್ದರೆ
#  define USE_VTP		0
#  define USE_WT		0
# endif

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_console_color_rgb(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ reset_console_color_rgb(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ restore_console_color_rgb(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ suppress_winsize = 1;	// don't fiddle with console
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ WCHAR *exe_pathw = ಖಾಲಿ;

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ಸ್ಥಿರ BOOL use_alternate_screen_ಬಫರ್ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

ಬಾಹ್ಯ DWORD win_ಆವೃತ್ತಿ; // this is in os_mswin.c

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ಸ್ಥಿರ BOOL
is_ambiwidth_event(
    ಒಳಹರಿವು_ದಾಖಲೆ *ir)
{
    ಹಿಂತಿರುಗಿ ir->EventType == ಕೀಲಿ_EVENT
		&& ir->Event.ಕೀಲಿ_Event_2.b_ಕೀಲಿ_ಕೆಳಕ್ಕೆ
		&& ir->Event.ಕೀಲಿ_Event_2.w_Repeat_ಎಣಿಕೆ == 1
		&& ir->Event.ಕೀಲಿ_Event_2.w_Virtual_ಕೀಲಿ_ಸಂಕೇತ == 0x12
		&& ir->Event.ಕೀಲಿ_Event_2.w_Virtual_ಪರಿಶೀಲನೆ_ಸಂಕೇತ == 0x38
		&& ir->Event.ಕೀಲಿ_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.ಕೀಲಿ_Event_2.dw_Control_ಕೀಲಿ_State == 2;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
make_ambiwidth_event(
    ಒಳಹರಿವು_ದಾಖಲೆ *ಕೆಳಕ್ಕೆ,
    ಒಳಹರಿವು_ದಾಖಲೆ *ಮೇಲೆ)
{
    ಕೆಳಕ್ಕೆ->Event.ಕೀಲಿ_Event_2.w_Virtual_ಕೀಲಿ_ಸಂಕೇತ = 0;
    ಕೆಳಕ್ಕೆ->Event.ಕೀಲಿ_Event_2.w_Virtual_ಪರಿಶೀಲನೆ_ಸಂಕೇತ = 0;
    ಕೆಳಕ್ಕೆ->Event.ಕೀಲಿ_Event_2.uChar.UnicodeChar
				    = ಮೇಲೆ->Event.ಕೀಲಿ_Event_2.uChar.UnicodeChar;
    ಕೆಳಕ್ಕೆ->Event.ಕೀಲಿ_Event_2.dw_Control_ಕೀಲಿ_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    ಸ್ಥಿರ BOOL
read_console_ಒಳಹರಿವು(
    HANDLE	    h_ಒಳಹರಿವು,
    ಒಳಹರಿವು_ದಾಖಲೆ    *lp_ಬಫರ್,
    ಪೂರ್ಣಾಂಕ		    n_ಉದ್ದಳತೆ,
    LPDWORD	    lpEvents)
{
    ಎಣಿಕೆ
    {
	IRSIZE = 10
    };
    ಸ್ಥಿರ ಒಳಹರಿವು_ದಾಖಲೆ s_irCache[IRSIZE];
    ಸ್ಥಿರ DWORD s_dw_ಸೂಚ್ಯಂಕ = 0;
    ಸ್ಥಿರ DWORD s_dwMax = 0;
    DWORD dwEvents;
    ಪೂರ್ಣಾಂಕ ತಲೆ;
    ಪೂರ್ಣಾಂಕ ಬಾಲ;
    ಪೂರ್ಣಾಂಕ i;
    ಸ್ಥಿರ ಒಳಹರಿವು_ದಾಖಲೆ s_irPseudo;

    ಆದರೆ (s_dwMax == 0 && ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ಉದ್ದಳತೆ > 0)
    {
	dwEvents = read_ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್(s_irCache, IRSIZE);
	s_dw_ಸೂಚ್ಯಂಕ = 0;
	s_dwMax = dwEvents;
    }

    ಆದರೆ (n_ಉದ್ದಳತೆ == -2)
	ಹಿಂತಿರುಗಿ (s_dwMax > 0) ? TRUE : FALSE;

    ಆದರೆ (win_ಆವೃತ್ತಿ < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	ಆದರೆ (n_ಉದ್ದಳತೆ == -1)
	    ಹಿಂತಿರುಗಿ Peek_Console_ಒಳಹರಿವು_W(h_ಒಳಹರಿವು, lp_ಬಫರ್, 1, lpEvents);
	ಹಿಂತಿರುಗಿ Read_Console_ಒಳಹರಿವು_W(h_ಒಳಹರಿವು, lp_ಬಫರ್, 1, &dwEvents);
    }

    ಆದರೆ (s_dwMax == 0)
    {
	ಆದರೆ (!vtp_working && n_ಉದ್ದಳತೆ == -1)
	    ಹಿಂತಿರುಗಿ Peek_Console_ಒಳಹರಿವು_W(h_ಒಳಹರಿವು, lp_ಬಫರ್, 1, lpEvents);
	Get_ಸಂಖ್ಯೆ_ದ_Console_ಒಳಹರಿವು_Events(h_ಒಳಹರಿವು, &dwEvents);
	ಆದರೆ (dwEvents == 0 && n_ಉದ್ದಳತೆ == -1)
	    ಹಿಂತಿರುಗಿ Peek_Console_ಒಳಹರಿವು_W(h_ಒಳಹರಿವು, lp_ಬಫರ್, 1, lpEvents);
	Read_Console_ಒಳಹರಿವು_W(h_ಒಳಹರಿವು, s_irCache, IRSIZE, &dwEvents);
	s_dw_ಸೂಚ್ಯಂಕ = 0;
	s_dwMax = dwEvents;
	ಆದರೆ (dwEvents == 0)
	{
	    *lpEvents = 0;
	    ಹಿಂತಿರುಗಿ TRUE;
	}

	ಗಾಗಿ (i = s_dw_ಸೂಚ್ಯಂಕ; i < (ಪೂರ್ಣಾಂಕ)s_dwMax - 1; ++i)
	    ಆದರೆ (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	ಆದರೆ (s_dwMax > 1)
	{
	    ತಲೆ = 0;
	    ಬಾಲ = s_dwMax - 1;
	    ಆಗ (ತಲೆ != ಬಾಲ)
	    {
		ಆದರೆ (s_irCache[ತಲೆ].EventType == ಕಿಟಕಿ_ಬಫರ್_ಗಾತ್ರ_EVENT
			&& s_irCache[ತಲೆ + 1].EventType
						  == ಕಿಟಕಿ_ಬಫರ್_ಗಾತ್ರ_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    ಗಾಗಿ (i = ತಲೆ; i < ಬಾಲ; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --ಬಾಲ;
		    ಮುಂದುವರಿ;
		}
		ತಲೆ++;
	    }
	    s_dwMax = ಬಾಲ + 1;
	}
    }

    ಆದರೆ (s_irCache[s_dw_ಸೂಚ್ಯಂಕ].EventType == ಕೀಲಿ_EVENT)
    {
	ಆದರೆ (s_irCache[s_dw_ಸೂಚ್ಯಂಕ].Event.ಕೀಲಿ_Event_2.w_Repeat_ಎಣಿಕೆ > 1)
	{
	    s_irPseudo = s_irCache[s_dw_ಸೂಚ್ಯಂಕ];
	    s_irPseudo.Event.ಕೀಲಿ_Event_2.w_Repeat_ಎಣಿಕೆ = 1;
	    s_irCache[s_dw_ಸೂಚ್ಯಂಕ].Event.ಕೀಲಿ_Event_2.w_Repeat_ಎಣಿಕೆ--;
	    *lp_ಬಫರ್ = s_irPseudo;
	    *lpEvents = 1;
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    }

    *lp_ಬಫರ್ = s_irCache[s_dw_ಸೂಚ್ಯಂಕ];
    ಆದರೆ (!(n_ಉದ್ದಳತೆ == -1 || n_ಉದ್ದಳತೆ == -2) && ++s_dw_ಸೂಚ್ಯಂಕ >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    ಸ್ಥಿರ BOOL
peek_console_ಒಳಹರಿವು(
    HANDLE	    h_ಒಳಹರಿವು,
    ಒಳಹರಿವು_ದಾಖಲೆ    *lp_ಬಫರ್,
    DWORD	    n_ಉದ್ದಳತೆ ಬಳಸದ,
    LPDWORD	    lpEvents)
{
    ಹಿಂತಿರುಗಿ read_console_ಒಳಹರಿವು(h_ಒಳಹರಿವು, lp_ಬಫರ್, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    ಸ್ಥಿರ DWORD
ಸಂದೇಶ_wait_for_ಅಪವರ್ತ್ಯ_objects(
    DWORD    n_ಎಣಿಕೆ,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dw_Wake_ಮುಖವಾಡ)
{
    ಆದರೆ (read_console_ಒಳಹರಿವು(ಖಾಲಿ, ಖಾಲಿ, -2, ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ WAIT_OBJECT_0;
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ_Wait_For_ಅಪವರ್ತ್ಯ_Objects(n_ಎಣಿಕೆ, pHandles, fWaitAll,
				     dwMilliseconds, dw_Wake_ಮುಖವಾಡ);
}
# endif

# ifndef FEAT_CLIENTSERVER
    ಸ್ಥಿರ DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    ಆದರೆ (read_console_ಒಳಹರಿವು(ಖಾಲಿ, ಖಾಲಿ, -2, ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ WAIT_OBJECT_0;
    ಹಿಂತಿರುಗಿ WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#ಆದರೆ_ಅಂತ್ಯ   // !FEAT_GUI_MSWIN || VIMDLL

    ಶೂನ್ಯ
mch_get_exe_name(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		updated = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	enc_prev = -1;
    WCHAR	*ಮಾರ್ಗ;
    size_t	pathlen;

    ಆದರೆ (exe_name == ಖಾಲಿ || exe_pathw == ಖಾಲಿ || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_ಮಾರ್ಗ];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(ಖಾಲಿ, buf, MAX_ಮಾರ್ಗ);
	ಆದರೆ (p > buf)
	{
	    ಆದರೆ (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_ಗಮ್ಯ_enc(buf, ಖಾಲಿ);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    ಮಾಡು
	    {
		--p;
		ಆದರೆ (*p == L'\\')
		{
		    *p = L'\0';
		    ನಿಲ್ಲಿಸು;
		}
	    } ಆಗ (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    ಆದರೆ (exe_pathw == ಖಾಲಿ || !updated)
	ಹಿಂತಿರುಗಿ;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    ಮಾರ್ಗ = _wgetenv(L"PATH");
    pathlen = (ಮಾರ್ಗ == ಖಾಲಿ) ? 0 : wcslen(ಮಾರ್ಗ);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#ವ್ಯಾಖ್ಯಾನಿಸು EXTRA_LEN 7
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ENV_STRING_LEN 8191

    ಆದರೆ (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   ತಾತ್ಕಾಲಿಕ[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	ಆದರೆ (pathlen == 0)
	    wcscpy(ತಾತ್ಕಾಲಿಕ + templen, exe_pathw);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wcscpy(ತಾತ್ಕಾಲಿಕ + templen, ಮಾರ್ಗ);

	    // Check if exe_path is already included in $PATH.
	    ಆದರೆ (wcsstr(ತಾತ್ಕಾಲಿಕ, exe_pathw) == ಖಾಲಿ)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		ಆದರೆ (ತಾತ್ಕಾಲಿಕ[templen - 1] != L';')
		{
		    wcscpy(ತಾತ್ಕಾಲಿಕ + templen, L";");
		    ++templen;
		}

		wcscpy(ತಾತ್ಕಾಲಿಕ + templen, exe_pathw);
	    }
	}

	_wputenv(ತಾತ್ಕಾಲಿಕ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ libintl_wputenv
	libintl_wputenv(ತಾತ್ಕಾಲಿಕ);
#ಆದರೆ_ಅಂತ್ಯ
    }
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು EXTRA_LEN
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು MAX_ENV_ಮಾರ್ಗ_LEN
}

/*
 * Unescape characters in "p" that appear in "escaped".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
unescape_shellxquote(char_u *p, char_u *escaped)
{
    ಪೂರ್ಣಾಂಕ	    l = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
    ಪೂರ್ಣಾಂಕ	    n;

    ಆಗ (*p != NUL)
    {
	ಆದರೆ (*p == '^' && vim_strchr(escaped, p[1]) != ಖಾಲಿ)
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
vim_ತುಂಬು_Lib(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
    HINSTANCE	dll = ಖಾಲಿ;

    // No need to load any library when registering OLE.
    ಆದರೆ (ಸಿಕ್ಕಿತು_ರಿಜಿಸ್ಟರ್_arg)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    ಆದರೆ (exe_pathw == ಖಾಲಿ)
    {
	mch_get_exe_name();
	ಆದರೆ (exe_pathw == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    WCHAR old_dirw[MAXPATHL];

    ಆದರೆ (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    ಗಣ_Current_Directory_W(exe_pathw);
    dll = ತುಂಬು_Library(name);
    ಗಣ_Current_Directory_W(old_dirw);
    ಹಿಂತಿರುಗಿ dll;
}

#ಆದರೆ defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    ಪೂರ್ಣಾಂಕ
mch_is_gui_executable(ಶೂನ್ಯ)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(ಖಾಲಿ);
    PIMAGE_DOS_ಶೀರ್ಷಿಕೆ	pDOS = (PIMAGE_DOS_ಶೀರ್ಷಿಕೆ)pImage;
    PIMAGE_NT_ಶೀರ್ಷಿಕೆಗಳು	pPE;

    ಆದರೆ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ಹಿಂತಿರುಗಿ FALSE;
    pPE = (PIMAGE_NT_ಶೀರ್ಷಿಕೆಗಳು)(pImage + pDOS->e_lfanew);
    ಆದರೆ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (pPE->Optional_ಶೀರ್ಷಿಕೆ.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ *
get_imported_func_info(HINSTANCE hInst, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *funcname, ಪೂರ್ಣಾಂಕ info,
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_ಶೀರ್ಷಿಕೆ		pDOS = (PIMAGE_DOS_ಶೀರ್ಷಿಕೆ)hInst;
    PIMAGE_NT_ಶೀರ್ಷಿಕೆಗಳು		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_ದತ್ತಾಂಶ		pIAT;	    // Import Address Table
    PIMAGE_THUNK_ದತ್ತಾಂಶ		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    ಆದರೆ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    pPE = (PIMAGE_NT_ಶೀರ್ಷಿಕೆಗಳು)(pImage + pDOS->e_lfanew);
    ಆದರೆ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ImpVA = pPE->Optional_ಶೀರ್ಷಿಕೆ
		.ದತ್ತಾಂಶ_Directory[IMAGE_DIRECTORY_ನಮೂದು_IMPORT].Virtual_ವಿಳಾಸ;
    ಆದರೆ (ImpVA == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    ಗಾಗಿ (; pImpDesc->ಮೊದಲ_Thunk; ++pImpDesc)
    {
	ಆದರೆ (!pImpDesc->Original_ಮೊದಲ_Thunk)
	    ಮುಂದುವರಿ;
	pIAT = (PIMAGE_THUNK_ದತ್ತಾಂಶ)(pImage + pImpDesc->ಮೊದಲ_Thunk);
	pINT = (PIMAGE_THUNK_ದತ್ತಾಂಶ)(pImage + pImpDesc->Original_ಮೊದಲ_Thunk);
	ಗಾಗಿ (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    ಆದರೆ (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		ಮುಂದುವರಿ;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.ವಿಳಾಸ_ದ_ದತ್ತಾಂಶ));
	    ಆದರೆ (strcmp((ಅಕ್ಷರ *)pImpName->Name, funcname) == 0)
	    {
		ಶೂನ್ಯ *original;
		DWORD old, new = ಪುಟ_READWRITE;

		ಆಯ್ಕೆ (info)
		{
		    ಸ್ಥಿತಿ 0:
			ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)pIAT->u1.Function;
		    ಸ್ಥಿತಿ 1:
			ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)(pImage + pImpDesc->Name);
		    ಸ್ಥಿತಿ 2:
			original = (ಶೂನ್ಯ *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, ಗಾತ್ರ(ಶೂನ್ಯ *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, ಗಾತ್ರ(ಶೂನ್ಯ *),
				old, &new);
			ಹಿಂತಿರುಗಿ original;
		    ಪೂರ್ವನಿಯೋಜಿತ:
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
	    }
	}
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
ಕಂಡುಹಿಡಿ_imported_module_by_funcname(HINSTANCE hInst, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *funcname)
{
    ಅಕ್ಷರ    *modulename;

    modulename = (ಅಕ್ಷರ *)get_imported_func_info(hInst, funcname, 1, ಖಾಲಿ);
    ಆದರೆ (modulename != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ GetModuleHandleA(modulename);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    ಶೂನ್ಯ *
get_dll_import_func(HINSTANCE hInst, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *funcname)
{
    ಹಿಂತಿರುಗಿ get_imported_func_info(hInst, funcname, 0, ಖಾಲಿ);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    ಶೂನ್ಯ *
hook_dll_import_func(HINSTANCE hInst, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *funcname, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *hook)
{
    ಹಿಂತಿರುಗಿ get_imported_func_info(hInst, funcname, 2, hook);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PYTHON3)
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
    PIMAGE_DOS_ಶೀರ್ಷಿಕೆ		pDOS = (PIMAGE_DOS_ಶೀರ್ಷಿಕೆ)hInst;
    PIMAGE_NT_ಶೀರ್ಷಿಕೆಗಳು		pPE;
    PIMAGE_EXPORT_DIRECTORY	pExpDir;
    DWORD			ExpVA;
    DWORD			Exp_ಗಾತ್ರ;
    LPDWORD			p_Function_ಕೋಷ್ಟಕ;

    ಆದರೆ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    pPE = (PIMAGE_NT_ಶೀರ್ಷಿಕೆಗಳು)(pImage + pDOS->e_lfanew);
    ಆದರೆ (pPE->Signature != IMAGE_NT_SIGNATURE)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ExpVA = pPE->Optional_ಶೀರ್ಷಿಕೆ
		.ದತ್ತಾಂಶ_Directory[IMAGE_DIRECTORY_ನಮೂದು_EXPORT].Virtual_ವಿಳಾಸ;
    Exp_ಗಾತ್ರ = pPE->Optional_ಶೀರ್ಷಿಕೆ
		.ದತ್ತಾಂಶ_Directory[IMAGE_DIRECTORY_ನಮೂದು_EXPORT].ಗಾತ್ರ_3;
    ಆದರೆ (ExpVA == 0)
	ಹಿಂತಿರುಗಿ hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    p_Function_ಕೋಷ್ಟಕ = (LPDWORD)(pImage + pExpDir->ವಿಳಾಸ_ದ_Functions);

    ಆದರೆ (pExpDir->ಸಂಖ್ಯೆ_ದ_ಹೆಸರುಗಳು == 0)
	ಹಿಂತಿರುಗಿ hInst;	// No export names.

    // Check only the first entry.
    ಆದರೆ ((p_Function_ಕೋಷ್ಟಕ[0] < ExpVA) || (p_Function_ಕೋಷ್ಟಕ[0] >= ExpVA + Exp_ಗಾತ್ರ))
	// The first entry is not a function forwarder.
	ಹಿಂತಿರುಗಿ hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(pImage + p_Function_ಕೋಷ್ಟಕ[0]);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = strchr(name, '.');
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ hInst;

    // Extract DllName.
    ಅಕ್ಷರ buf[MAX_ಮಾರ್ಗ];
    ಆದರೆ (p - name + 1 > ಗಾತ್ರ(buf))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    ಹಿಂತಿರುಗಿ GetModuleHandleA(buf);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
ಸ್ಥಿರ ಅಕ್ಷರ *null_libintl_gettext(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಅಕ್ಷರ *null_libintl_ngettext(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ n);
ಸ್ಥಿರ ಅಕ್ಷರ *null_libintl_textdomain(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಅಕ್ಷರ *null_libintl_bindtextdomain(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಅಕ್ಷರ *null_libintl_bind_textdomain_codeset(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ null_libintl_wputenv(ಸ್ಥಿರಾಂಕ wchar_t *);

ಸ್ಥಿರ HINSTANCE hLibintlDLL = ಖಾಲಿ;
ಅಕ್ಷರ *(*dyn_libintl_gettext)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) = null_libintl_gettext;
ಅಕ್ಷರ *(*dyn_libintl_ngettext)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ n)
						= null_libintl_ngettext;
ಅಕ್ಷರ *(*dyn_libintl_textdomain)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) = null_libintl_textdomain;
ಅಕ್ಷರ *(*dyn_libintl_bindtextdomain)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)
						= null_libintl_bindtextdomain;
ಅಕ್ಷರ *(*dyn_libintl_bind_textdomain_codeset)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)
				       = null_libintl_bind_textdomain_codeset;
ಪೂರ್ಣಾಂಕ (*dyn_libintl_wputenv)(ಸ್ಥಿರಾಂಕ wchar_t *) = null_libintl_wputenv;
ಪೂರ್ಣಾಂಕ *dyn_libintl_nl_ಸಂದೇಶ_cat_cntr = ಖಾಲಿ;

    ಪೂರ್ಣಾಂಕ
dyn_libintl_init(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;
    ಸ್ಥಿರ ರಚನೆ
    {
	ಅಕ್ಷರ	    *name;
	FARPROC	    *ptr;
    } libintl_ನಮೂದು[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{ಖಾಲಿ, ಖಾಲಿ}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    ಆದರೆ (hLibintlDLL != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vim_ತುಂಬು_Lib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    ಆದರೆ (!hLibintlDLL)
	hLibintlDLL = vim_ತುಂಬು_Lib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    ಆದರೆ (!hLibintlDLL)
	hLibintlDLL = vim_ತುಂಬು_Lib(GETTEXT_DLL_ALT2);
# endif
    ಆದರೆ (!hLibintlDLL)
    {
	ಆದರೆ (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_ತುಂಬು_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	ಹಿಂತಿರುಗಿ 0;
    }
    ಗಾಗಿ (i = 0; libintl_ನಮೂದು[i].name != ಖಾಲಿ
					 && libintl_ನಮೂದು[i].ptr != ಖಾಲಿ; ++i)
    {
	ಆದರೆ ((*libintl_ನಮೂದು[i].ptr = Get_Proc_ವಿಳಾಸ(hLibintlDLL,
					      libintl_ನಮೂದು[i].name)) == ಖಾಲಿ)
	{
	    dyn_libintl_ಕೊನೆ();
	    ಆದರೆ (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_ತುಂಬು_library_function_str), libintl_ನಮೂದು[i].name);
		verbose_leave();
	    }
	    ಹಿಂತಿರುಗಿ 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (ಅಕ್ಷರ *(*)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *))
			Get_Proc_ವಿಳಾಸ(hLibintlDLL, "bind_textdomain_codeset");
    ಆದರೆ (dyn_libintl_bind_textdomain_codeset == ಖಾಲಿ)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = ಕಂಡುಹಿಡಿ_imported_module_by_funcname(hLibintlDLL, "getenv");
    ಆದರೆ (hmsvcrt != ಖಾಲಿ)
	dyn_libintl_wputenv = (ಪೂರ್ಣಾಂಕ (*)(ಸ್ಥಿರಾಂಕ wchar_t *))
					Get_Proc_ವಿಳಾಸ(hmsvcrt, "_wputenv");
    ಆದರೆ (dyn_libintl_wputenv == ಖಾಲಿ || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_ಸಂದೇಶ_cat_cntr = (ಪೂರ್ಣಾಂಕ *)Get_Proc_ವಿಳಾಸ(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    ಹಿಂತಿರುಗಿ 1;
}

    ಶೂನ್ಯ
dyn_libintl_ಕೊನೆ(ಶೂನ್ಯ)
{
    ಆದರೆ (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ಖಾಲಿ;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_ಸಂದೇಶ_cat_cntr	= ಖಾಲಿ;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
null_libintl_gettext(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *msgid)
{
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*)msgid;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
null_libintl_ngettext(
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *msgid,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *msgid_plural,
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ n)
{
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)(n == 1 ? msgid : msgid_plural);
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
null_libintl_bindtextdomain(
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domainname ಬಳಸದ,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dirname ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
null_libintl_bind_textdomain_codeset(
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domainname ಬಳಸದ,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codeset ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
null_libintl_textdomain(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domainname ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
null_libintl_wputenv(ಸ್ಥಿರಾಂಕ wchar_t *envstring ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ_ಅಂತ್ಯ // DYNAMIC_GETTEXT

// This symbol is not defined in older versions of the SDK or Visual C++

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
# include <aclapi.h>
# ifndef PROTECTED_DACL_SECURITY_INFORMATION
#  define PROTECTED_DACL_SECURITY_INFORMATION	0x80000000L
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
/*
 * Enables or disables the specified privilege.
 */
    ಸ್ಥಿರ BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    ಆದರೆ (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (!Lookup_Privilege_ಮೌಲ್ಯ(ಖಾಲಿ, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    tokenPrivileges.Privilege_ಎಣಿಕೆ	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].ಗುಣಲಕ್ಷಣಗಳು_2 = SE_PRIVILEGE_ENABLED;

    ಆದರೆ (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, ಖಾಲಿ, ಖಾಲಿ))
    {
	CloseHandle(hToken);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಆದರೆ (Get_ಕೊನೆಯ_Error() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    CloseHandle(hToken);
    ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    ಶೂನ್ಯ
PlatformId(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    DWORD major = (win_ಆವೃತ್ತಿ >> 24) & 0xFF;
    DWORD minor = (win_ಆವೃತ್ತಿ >> 16) & 0xFF;
    vim_snprintf(windows_ಆವೃತ್ತಿ, ಗಾತ್ರ(windows_ಆವೃತ್ತಿ), "%d.%d", major, minor);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    ಆದರೆ (!win32_enable_privilege(SE_SECURITY_NAME))
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define ಸರಿಸು_2  (ಸರಿಸು_PRESSED)
# define CTRL   (ಬಲ_CTRL_PRESSED | ಎಡ_CTRL_PRESSED)
# define ALT    (ಬಲ_ALT_PRESSED  | ಎಡ_ALT_PRESSED)
# define ALT_GR (ಬಲ_ALT_PRESSED  | ಎಡ_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ
{
    ಶಬ್ದ_2    w_Virt_ಕೀಲಿ;
    BOOL    f_Ansi_ಕೀಲಿ;
    ಪೂರ್ಣಾಂಕ	    chAlone;
    ಪೂರ್ಣಾಂಕ	    ch_ಸರಿಸು;
    ಪೂರ್ಣಾಂಕ	    chCtrl;
    ಪೂರ್ಣಾಂಕ	    chAlt;
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
    { VK_ಮೇಲೆ,	TRUE,	'H',	'\304',	'\305',	    '\306', },
    { VK_ಪೂರ್ವ,	TRUE,	'I',	'\307',	'\204',	    '\310', }, // PgUp
    { VK_ಎಡ,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_ಬಲ,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_ಕೊನೆ,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_ಕೆಳಕ್ಕೆ,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_ಮುಂದಿನ,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_ಸೇರಿಸು,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_ಹಿಂಭಾಗ,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# ಆದರೆ 0
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
    { VK_ಕೂಡಿಸು,	TRUE,   'N',    'N',    'N',	'N',	}, // keyp '+'
    { VK_ಕಳೆ, TRUE,'J',	'J',    'J',	'J',	}, // keyp '-'
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
win32_kbd_patch_ಕೀಲಿ(
    ಕೀಲಿ_EVENT_ದಾಖಲೆ *pker)
{
    UINT uMods = pker->dw_Control_ಕೀಲಿ_State;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_iIsDead = 0;
    ಸ್ಥಿರ ಶಬ್ದ_2 aw_Ansi_ಸಂಕೇತ[2];
    ಸ್ಥಿರ BYTE abKeystate[256];

    ಆದರೆ (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_ಸಂಕೇತ[1];
	s_iIsDead = 0;
	ಹಿಂತಿರುಗಿ 1;
    }

    // check if it already has a valid unicode character.
    ಆದರೆ (pker->uChar.UnicodeChar != 0)
	ಹಿಂತಿರುಗಿ 1;

    CLEAR_ಕ್ಷೇತ್ರ(abKeystate);

    // Clear any pending dead keys
    ಗಮ್ಯ_Unicode(VK_SPACE, Map_Virtual_ಕೀಲಿ(VK_SPACE, 0), abKeystate, aw_Ansi_ಸಂಕೇತ, 2, 0);

    ಆದರೆ (uMods & ಸರಿಸು_PRESSED)
	abKeystate[VK_ಸರಿಸು] = 0x80;
    ಆದರೆ (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    ಆದರೆ ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ಗಮ್ಯ_Unicode(pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ, pker->w_Virtual_ಪರಿಶೀಲನೆ_ಸಂಕೇತ,
			abKeystate, aw_Ansi_ಸಂಕೇತ, 2, 0);

    ಆದರೆ (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_ಸಂಕೇತ[0];

    ಹಿಂತಿರುಗಿ s_iIsDead;
}

ಸ್ಥಿರ BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    ಸ್ಥಿರ BOOL
decode_ಕೀಲಿ_event(
    ಕೀಲಿ_EVENT_ದಾಖಲೆ	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    ಪೂರ್ಣಾಂಕ			*pmodifiers,
    BOOL		fDoPost ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ i;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ nModifs = pker->dw_Control_ಕೀಲಿ_State & (ಸರಿಸು_2 | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    ಆದರೆ (!pker->b_ಕೀಲಿ_ಕೆಳಕ್ಕೆ)
	ಹಿಂತಿರುಗಿ FALSE;

    // ignore some keystrokes
    ಆಯ್ಕೆ (pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ)
    {
    // modifiers
    ಸ್ಥಿತಿ VK_ಸರಿಸು:
    ಸ್ಥಿತಿ VK_CONTROL:
    ಸ್ಥಿತಿ VK_MENU:   // Alt key
	ಹಿಂತಿರುಗಿ FALSE;

    ಪೂರ್ವನಿಯೋಜಿತ:
	ನಿಲ್ಲಿಸು;
    }

    // Shift-TAB
    ಆದರೆ (pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ == VK_TAB && (nModifs & ಸರಿಸು_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಗಾಗಿ (i = ARRAY_ಉದ್ದಳತೆ(Virt_ಕೀಲಿ_Map); --i >= 0; )
    {
	ಆದರೆ (Virt_ಕೀಲಿ_Map[i].w_Virt_ಕೀಲಿ == pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ)
	{
	    *pch = Virt_ಕೀಲಿ_Map[i].chAlone;
	    ಆದರೆ ((nModifs & ಸರಿಸು_2) != 0)
		*pch = Virt_ಕೀಲಿ_Map[i].ch_ಸರಿಸು;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_ಕೀಲಿ_Map[i].chCtrl;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((nModifs & ALT) != 0)
		*pch = Virt_ಕೀಲಿ_Map[i].chAlt;

	    ಆದರೆ (*pch != 0)
	    {
		ಆದರೆ (Virt_ಕೀಲಿ_Map[i].f_Ansi_ಕೀಲಿ)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    ಆದರೆ (pmodifiers)
		    {
			ಆದರೆ (pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ >= VK_F1
			    && pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ <= VK_F12)
			{
			    ಆದರೆ ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_ಮುಖವಾಡ_ALT;
				ಆದರೆ ((nModifs & ಸರಿಸು_2) == 0)
				    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
			    }
			    ಆದರೆ ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_ಮುಖವಾಡ_CTRL;
				ಆದರೆ ((nModifs & ಸರಿಸು_2) == 0)
				    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ >= VK_ಕೊನೆ
				&& pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ <= VK_ಕೆಳಕ್ಕೆ)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
			    ಆದರೆ ((nModifs & ಸರಿಸು_2) != 0
						    && (nModifs & ~ಸರಿಸು_2) == 0)
			    {
				*pch2 = Virt_ಕೀಲಿ_Map[i].ch_ಸರಿಸು;
			    }
			    ಆದರೆ ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_ಕೀಲಿ_Map[i].chCtrl;
				ಆದರೆ (pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ == VK_ಮೇಲೆ
				    || pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ == VK_ಕೆಳಕ್ಕೆ)
				{
				    *pmodifiers |= MOD_ಮುಖವಾಡ_CTRL;
				    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
				}
			    }
			    ಆದರೆ ((nModifs & ಸರಿಸು_2) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_ಮುಖವಾಡ_CTRL;
				*pch2 = Virt_ಕೀಲಿ_Map[i].ch_ಸರಿಸು;
			    }
			    ಆದರೆ ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_ಕೀಲಿ_Map[i].chAlt;
				*pmodifiers |= MOD_ಮುಖವಾಡ_ALT;
				ಆದರೆ ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
				}
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((nModifs & ಸರಿಸು_2) != 0)
				{
				    *pch2 = Virt_ಕೀಲಿ_Map[i].ch_ಸರಿಸು;
				}
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((nModifs & CTRL) != 0)
				{
				    ಆದರೆ (pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ == VK_ಮೇಲೆ
					|| pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ == VK_ಕೆಳಕ್ಕೆ)
				    {
					*pmodifiers |= MOD_ಮುಖವಾಡ_CTRL;
					*pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
				    }
				    ಇಲ್ಲದಿದ್ದರೆ
				    {
					*pch2 = Virt_ಕೀಲಿ_Map[i].chCtrl;
				    }
				}
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pker->w_Virtual_ಕೀಲಿ_ಸಂಕೇತ == VK_ಸೇರಿಸು
					&& (nModifs & ಸರಿಸು_2) != 0
					&& (nModifs & ~ಸರಿಸು_2) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_ಕೀಲಿ_Map[i].ch_ಸರಿಸು;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    *pch2 = Virt_ಕೀಲಿ_Map[i].chAlone;
			    ಆದರೆ ((nModifs & ಸರಿಸು_2) != 0)
				*pmodifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;
			    ಆದರೆ ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_ಮುಖವಾಡ_CTRL;
			    ಆದರೆ ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_ಮುಖವಾಡ_ALT;
			}
		    }
		}

		ಹಿಂತಿರುಗಿ TRUE;
	    }
	}
    }

    i = win32_kbd_patch_ಕೀಲಿ(pker);

    ಆದರೆ (i < 0)
	*pch = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	ಆದರೆ (pmodifiers != ಖಾಲಿ)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    ಆದರೆ ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_ಮುಖವಾಡ_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    ಆದರೆ ((nModifs & ಸರಿಸು_2) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    ಆದರೆ ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_ಮುಖವಾಡ_CTRL;
	}
    }

    ಹಿಂತಿರುಗಿ (*pch != NUL);
}

# ಆದರೆ defined(FEAT_EVAL)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
encode_ಕೀಲಿ_event(dict_T *ವಾದಗಳು, ಒಳಹರಿವು_ದಾಖಲೆ *ir)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_dwMods = 0;

    char_u *action = dict_get_string(ವಾದಗಳು, "event", TRUE);
    ಆದರೆ (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_ಕೀಲಿ_ಕೆಳಕ್ಕೆ = STRICMP(action, "keydown") == 0;
	ಶಬ್ದ_2 vk_ಸಂಕೇತ = dict_get_ಸಂಖ್ಯೆ_def(ವಾದಗಳು, "keycode", 0);
	ಆದರೆ (vk_ಸಂಕೇತ <= 0 || vk_ಸಂಕೇತ >= 0xFF)
	{
	    semsg(_(e_invalid_ವಾದ_nr), (ದೀರ್ಘ)vk_ಸಂಕೇತ);
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	ir->EventType = ಕೀಲಿ_EVENT;
	ಕೀಲಿ_EVENT_ದಾಖಲೆ ker;
	Zero_ಸ್ಮರಣೆ(&ker, ಗಾತ್ರ(ker));
	ker.b_ಕೀಲಿ_ಕೆಳಕ್ಕೆ = is_ಕೀಲಿ_ಕೆಳಕ್ಕೆ;
	ker.w_Repeat_ಎಣಿಕೆ = 1;
	ker.w_Virtual_ಪರಿಶೀಲನೆ_ಸಂಕೇತ = 0;
	ker.dw_Control_ಕೀಲಿ_State = 0;
	ಪೂರ್ಣಾಂಕ mods = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	ಆದರೆ (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    ಆದರೆ (mods & MOD_ಮುಖವಾಡ_ಸರಿಸು)
		ker.dw_Control_ಕೀಲಿ_State |= ಸರಿಸು_PRESSED;
	    ಆದರೆ (mods & MOD_ಮುಖವಾಡ_CTRL)
		ker.dw_Control_ಕೀಲಿ_State |= ಎಡ_CTRL_PRESSED;
	    ಆದರೆ (mods & MOD_ಮುಖವಾಡ_ALT)
		ker.dw_Control_ಕೀಲಿ_State |= ಎಡ_ALT_PRESSED;
	}

	ಆದರೆ (vk_ಸಂಕೇತ == VK_LSHIFT || vk_ಸಂಕೇತ == VK_RSHIFT || vk_ಸಂಕೇತ == VK_ಸರಿಸು)
	{
	    ಆದರೆ (is_ಕೀಲಿ_ಕೆಳಕ್ಕೆ)
		s_dwMods |= ಸರಿಸು_PRESSED;
	    ಇಲ್ಲದಿದ್ದರೆ
		s_dwMods &= ~ಸರಿಸು_PRESSED;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vk_ಸಂಕೇತ == VK_LCONTROL || vk_ಸಂಕೇತ == VK_CONTROL)
	{
	    ಆದರೆ (is_ಕೀಲಿ_ಕೆಳಕ್ಕೆ)
		s_dwMods |= ಎಡ_CTRL_PRESSED;
	    ಇಲ್ಲದಿದ್ದರೆ
		s_dwMods &= ~ಎಡ_CTRL_PRESSED;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vk_ಸಂಕೇತ == VK_RCONTROL)
	{
	    ಆದರೆ (is_ಕೀಲಿ_ಕೆಳಕ್ಕೆ)
		s_dwMods |= ಬಲ_CTRL_PRESSED;
	    ಇಲ್ಲದಿದ್ದರೆ
		s_dwMods &= ~ಬಲ_CTRL_PRESSED;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vk_ಸಂಕೇತ == VK_LMENU || vk_ಸಂಕೇತ == VK_MENU)
	{
	    ಆದರೆ (is_ಕೀಲಿ_ಕೆಳಕ್ಕೆ)
		s_dwMods |= ಎಡ_ALT_PRESSED;
	    ಇಲ್ಲದಿದ್ದರೆ
		s_dwMods &= ~ಎಡ_ALT_PRESSED;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vk_ಸಂಕೇತ == VK_RMENU)
	{
	    ಆದರೆ (is_ಕೀಲಿ_ಕೆಳಕ್ಕೆ)
		s_dwMods |= ಬಲ_ALT_PRESSED;
	    ಇಲ್ಲದಿದ್ದರೆ
		s_dwMods &= ~ಬಲ_ALT_PRESSED;
	}
	ker.dw_Control_ಕೀಲಿ_State |= s_dwMods;
	ker.w_Virtual_ಕೀಲಿ_ಸಂಕೇತ = vk_ಸಂಕೇತ;
	ker.uChar.UnicodeChar = 0;
	ir->Event.ಕೀಲಿ_Event_2 = ker;
	vim_free(action);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (action == ಖಾಲಿ)
	{
	    semsg(_(e_missing_ವಾದ_str), "event");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "event", action);
	    vim_free(action);
	}
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}
# endif  // FEAT_EVAL
#ಆದರೆ_ಅಂತ್ಯ // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_fMouseAvail = FALSE;   // mouse present
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_f_Mouse_ಸಕ್ರಿಯ = FALSE;  // mouse enabled
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_nMouseClick = -1;	    // mouse status
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_xMouse;		    // mouse x coordinate
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_yMouse;		    // mouse y coordinate
ಸ್ಥಿರ DWORD g_cmodein = 0;	    // Original console input mode
ಸ್ಥಿರ DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    ಶೂನ್ಯ
mch_setmouse(ಪೂರ್ಣಾಂಕ on)
{
    DWORD cmodein;

# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;
# endif
    ಆದರೆ (!g_fMouseAvail)
	ಹಿಂತಿರುಗಿ;

    g_f_Mouse_ಸಕ್ರಿಯ = on;
    Get_Console_ಬಹುಲಕ(g_h_Con_ಒಳ, &cmodein);

    ಆದರೆ (g_f_Mouse_ಸಕ್ರಿಯ)
    {
	cmodein |= ENABLE_MOUSE_ಒಳಹರಿವು;
	cmodein &= ~ENABLE_ಶೀಘ್ರ_EDIT_ಬಹುಲಕ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	cmodein &= ~ENABLE_MOUSE_ಒಳಹರಿವು;
	cmodein |= g_cmodein & ENABLE_ಶೀಘ್ರ_EDIT_ಬಹುಲಕ;
    }

    ಗಣ_Console_ಬಹುಲಕ(g_h_Con_ಒಳ, cmodein | ENABLE_EXTENDED_FLAGS);
}


# ಆದರೆ defined(FEAT_BEVAL_ಪದ)
/*
 * Called when 'balloonevalterm' changed.
 */
    ಶೂನ್ಯ
mch_bevalterm_ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
    mch_setmouse(g_f_Mouse_ಸಕ್ರಿಯ);
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
decode_mouse_wheel(MOUSE_EVENT_ದಾಖಲೆ *pmer)
{
    ಪೂರ್ಣಾಂಕ	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    ಪೂರ್ಣಾಂಕ	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    ಪೂರ್ಣಾಂಕ lcol = g_xMouse;
    ಪೂರ್ಣಾಂಕ lrow = g_yMouse;
    win_T *wp = mouse_ಕಂಡುಹಿಡಿ_win(&lrow, &lcol, ಕಂಡುಹಿಡಿ_POPUP);
    ಆದರೆ (wp != ಖಾಲಿ && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_ಕ್ಷೇತ್ರ(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	ಆದರೆ (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ಎಡ : MSCR_ಬಲ;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	    cap.arg = zDelta < 0 ? MSCR_ಮೇಲೆ : MSCR_ಕೆಳಕ್ಕೆ;
	}

	// Mouse hovers over popup window, scroll it if possible.
	mouse_ಅಡ್ಡಸಾಲು = wp->w_winrow;
	mouse_ಲಂಬಸಾಲು = wp->w_wincol;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	ಹೊರಹೋಗು_flush();
    }
# endif
    mouse_ಲಂಬಸಾಲು = g_xMouse;
    mouse_ಅಡ್ಡಸಾಲು = g_yMouse;

    char_u modifiers = 0;
    char_u direction = 0;

    // Decode the direction into an event that Vim can process
    ಆದರೆ (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    ಇಲ್ಲದಿದ್ದರೆ
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    ಆದರೆ (pmer->dw_Control_ಕೀಲಿ_State & ಸರಿಸು_PRESSED)
	modifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು; // MOUSE_SHIFT;
    ಆದರೆ (pmer->dw_Control_ಕೀಲಿ_State & (ಬಲ_CTRL_PRESSED | ಎಡ_CTRL_PRESSED))
	modifiers |= MOD_ಮುಖವಾಡ_CTRL; // MOUSE_CTRL;
    ಆದರೆ (pmer->dw_Control_ಕೀಲಿ_State & (ಬಲ_ALT_PRESSED  | ಎಡ_ALT_PRESSED))
	modifiers |= MOD_ಮುಖವಾಡ_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    ಹಿಂತಿರುಗಿ;
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
    MOUSE_EVENT_ದಾಖಲೆ *pmer)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_nOldButton = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_nOldMouseClick = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_xOldMouse = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_yOldMouse = -1;
    ಸ್ಥಿರ linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_old_topfill = 0;
# endif
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_cClicks = 1;
    ಸ್ಥಿರ BOOL s_fReleased = TRUE;
    ಸ್ಥಿರ DWORD s_dw_ಕೊನೆಯ_Click_ಸಮಯ = 0;
    ಸ್ಥಿರ BOOL s_f_ಮುಂದಿನ_Is_ಮಧ್ಯ = FALSE;

    ಸ್ಥಿರ DWORD cButtons = 0;	// number of buttons supported

    ಸ್ಥಿರಾಂಕ DWORD ಎಡ_3 = FROM_ಎಡ_1_ST_BUTTON_PRESSED;
    ಸ್ಥಿರಾಂಕ DWORD ಮಧ್ಯ = FROM_ಎಡ_2_ND_BUTTON_PRESSED;
    ಸ್ಥಿರಾಂಕ DWORD ಬಲ_3 = RIGHTMOST_BUTTON_PRESSED;
    ಸ್ಥಿರಾಂಕ DWORD ಎಡ_ಬಲ = ಎಡ_3 | ಬಲ_3;

    ಪೂರ್ಣಾಂಕ nButton;

    ಆದರೆ (cButtons == 0 && !Get_ಸಂಖ್ಯೆ_ದ_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    ಆದರೆ (!g_fMouseAvail || !g_f_Mouse_ಸಕ್ರಿಯ)
    {
	g_nMouseClick = -1;
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    ಆದರೆ (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    ಆದರೆ (g_nMouseClick != -1)
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	ಹಿಂತಿರುಗಿ TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    ಆದರೆ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	ಆದರೆ (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    ಹಿಂತಿರುಗಿ FALSE;
    }

    // If no buttons are pressed...
    ಆದರೆ ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	ಆದರೆ (s_fReleased)
	{
# ifdef FEAT_BEVAL_ಪದ
	    // do return mouse move events when we want them
	    ಆದರೆ (p_bevalterm)
		nButton = MOUSE_DRAG;
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		ಹಿಂತಿರುಗಿ FALSE;
	}

	s_fReleased = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	ಆದರೆ (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & ಎಡ_ಬಲ);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    ಆದರೆ (dwLR == ಎಡ_3 || dwLR == ಬಲ_3)
	    {
		ಗಾಗಿ (;;)
		{
		    // wait a short time for next input event
		    ಆದರೆ (WaitForSingleObject(g_h_Con_ಒಳ, p_mouset / 3)
							     != WAIT_OBJECT_0)
			ನಿಲ್ಲಿಸು;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			DWORD c_ದಾಖಲೆಗಳು = 0;
			ಒಳಹರಿವು_ದಾಖಲೆ ir;
			MOUSE_EVENT_ದಾಖಲೆ* pmer2 = &ir.Event.MouseEvent;

			peek_console_ಒಳಹರಿವು(g_h_Con_ಒಳ, &ir, 1, &c_ದಾಖಲೆಗಳು);

			ಆದರೆ (c_ದಾಖಲೆಗಳು == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & ಎಡ_ಬಲ))
			    ನಿಲ್ಲಿಸು;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಆದರೆ (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_ಒಳಹರಿವು(g_h_Con_ಒಳ, &ir, 1, &c_ದಾಖಲೆಗಳು);

				ಹಿಂತಿರುಗಿ decode_mouse_event(pmer2);
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_ಒಳಹರಿವು(g_h_Con_ಒಳ, &ir, 1, &c_ದಾಖಲೆಗಳು);

				// are there any more mouse events in queue?
				peek_console_ಒಳಹರಿವು(g_h_Con_ಒಳ, &ir, 1, &c_ದಾಖಲೆಗಳು);

				ಆದರೆ (c_ದಾಖಲೆಗಳು==0 || ir.EventType != MOUSE_EVENT)
				    ನಿಲ್ಲಿಸು;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
				ನಿಲ್ಲಿಸು;
			}
		    }
		}
	    }
	}

	ಆದರೆ (s_f_ಮುಂದಿನ_Is_ಮಧ್ಯ)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_ಮಧ್ಯ;
	    s_f_ಮುಂದಿನ_Is_ಮಧ್ಯ = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cButtons == 2	&&
	    ((pmer->dwButtonState & ಎಡ_ಬಲ) == ಎಡ_ಬಲ))
	{
	    nButton = MOUSE_ಮಧ್ಯ;

	    ಆದರೆ (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_ಮುಂದಿನ_Is_ಮಧ್ಯ = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((pmer->dwButtonState & ಎಡ_3) == ಎಡ_3)
	    nButton = MOUSE_ಎಡ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((pmer->dwButtonState & ಮಧ್ಯ) == ಮಧ್ಯ)
	    nButton = MOUSE_ಮಧ್ಯ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((pmer->dwButtonState & ಬಲ_3) == ಬಲ_3)
	    nButton = MOUSE_ಬಲ;

	ಆದರೆ (! s_fReleased && ! s_f_ಮುಂದಿನ_Is_ಮಧ್ಯ
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    ಹಿಂತಿರುಗಿ FALSE;

	s_fReleased = s_f_ಮುಂದಿನ_Is_ಮಧ್ಯ;
    }

    ಆದರೆ (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	ಆದರೆ (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dw_Current_ಸಮಯ = Get_Tick_ಎಣಿಕೆ();

	    ಆದರೆ (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (ಪೂರ್ಣಾಂಕ)(dw_Current_ಸಮಯ - s_dw_ಕೊನೆಯ_Click_ಸಮಯ) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dw_ಕೊನೆಯ_Click_ಸಮಯ = dw_Current_ಸಮಯ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	ಆದರೆ (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    ಆದರೆ (nButton == -1)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    ಆದರೆ (pmer->dw_Control_ಕೀಲಿ_State & ಸರಿಸು_PRESSED)
	g_nMouseClick |= MOUSE_ಸರಿಸು;
    ಆದರೆ (pmer->dw_Control_ಕೀಲಿ_State & (ಬಲ_CTRL_PRESSED | ಎಡ_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    ಆದರೆ (pmer->dw_Control_ಕೀಲಿ_State & (ಬಲ_ALT_PRESSED  | ಎಡ_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    ಆದರೆ (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	ಗಣ_ಅಂಕೆ_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    ಆದರೆ (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	ಹಿಂತಿರುಗಿ FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    ಹಿಂತಿರುಗಿ TRUE;
}

# ifdef FEAT_EVAL
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
encode_mouse_event(dict_T *ವಾದಗಳು, ಒಳಹರಿವು_ದಾಖಲೆ *ir)
{
    ಪೂರ್ಣಾಂಕ		button;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		repeated_click;
    int_u	mods = 0;
    ಪೂರ್ಣಾಂಕ		move;

    ಆದರೆ (!dict_has_ಕೀಲಿ(ವಾದಗಳು, "row") || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "col"))
	ಹಿಂತಿರುಗಿ FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (ಪೂರ್ಣಾಂಕ)dict_get_bool(ವಾದಗಳು, "move", FALSE);
    ಆದರೆ (!move && (!dict_has_ಕೀಲಿ(ವಾದಗಳು, "button")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "multiclick")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "modifiers")))
	ಹಿಂತಿರುಗಿ FALSE;

    ಅಡ್ಡಸಾಲು = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "row") - 1;
    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_ದಾಖಲೆ mer;
    Zero_ಸ್ಮರಣೆ(&mer, ಗಾತ್ರ(mer));
    mer.dwMousePosition.X  = ಲಂಬಸಾಲು;
    mer.dwMousePosition.Y  = ಅಡ್ಡಸಾಲು;

    ಆದರೆ (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	button = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "button");
	repeated_click = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "multiclick");
	mods = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_ಗಣ_hor_scroll_ಹೆಜ್ಜೆ(6);
	mouse_ಗಣ_vert_scroll_ಹೆಜ್ಜೆ(3);

	ಆಯ್ಕೆ (button)
	{
	    ಸ್ಥಿತಿ MOUSE_ಎಡ:
		mer.dwButtonState = FROM_ಎಡ_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MOUSE_ಮಧ್ಯ:
		mer.dwButtonState = FROM_ಎಡ_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MOUSE_ಬಲ:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    ಸ್ಥಿತಿ MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MOUSE_X1:
		mer.dwButtonState = FROM_ಎಡ_3_RD_BUTTON_PRESSED;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MOUSE_X2:
		mer.dwButtonState = FROM_ಎಡ_4_TH_BUTTON_PRESSED;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		semsg(_(e_invalid_ವಾದ_str), "button");
		ಹಿಂತಿರುಗಿ FALSE;
	}
    }

    mer.dw_Control_ಕೀಲಿ_State = 0;
    ಆದರೆ (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	ಆದರೆ (mods & MOUSE_ಸರಿಸು)
	    mer.dw_Control_ಕೀಲಿ_State |= ಸರಿಸು_PRESSED;
	ಆದರೆ (mods & MOUSE_CTRL)
	    mer.dw_Control_ಕೀಲಿ_State |= ಎಡ_CTRL_PRESSED;
	ಆದರೆ (mods & MOUSE_ALT)
	    mer.dw_Control_ಕೀಲಿ_State |= ಎಡ_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    ಹಿಂತಿರುಗಿ TRUE;
}
# endif  // FEAT_EVAL

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
write_ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್(ಒಳಹರಿವು_ದಾಖಲೆ* irEvents, ಪೂರ್ಣಾಂಕ n_ಉದ್ದಳತೆ)
{
    ಪೂರ್ಣಾಂಕ				n_ಎಣಿಕೆ = 0;
    ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್_ಗಂಟು_T	*event_ಗಂಟು;

    ಆಗ (n_ಎಣಿಕೆ < n_ಉದ್ದಳತೆ)
    {
	event_ಗಂಟು = alloc(ಗಾತ್ರ(ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್_ಗಂಟು_T));
	ಆದರೆ (event_ಗಂಟು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;

	event_ಗಂಟು->ir = irEvents[n_ಎಣಿಕೆ++];
	event_ಗಂಟು->ಮುಂದಿನ = ಖಾಲಿ;

	ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ಉದ್ದಳತೆ++;
	ಆದರೆ (ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ಬಾಲ == ಖಾಲಿ)
	{
	    ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ತಲೆ = event_ಗಂಟು;
	    ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ಬಾಲ = event_ಗಂಟು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ಬಾಲ->ಮುಂದಿನ = event_ಗಂಟು;
	    ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ಬಾಲ = ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ಬಾಲ->ಮುಂದಿನ;
	}
    }
    ಹಿಂತಿರುಗಿ n_ಎಣಿಕೆ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್(ಒಳಹರಿವು_ದಾಖಲೆ* irEvents, ಪೂರ್ಣಾಂಕ n_Max_ಉದ್ದಳತೆ)
{
    ಪೂರ್ಣಾಂಕ n_ಎಣಿಕೆ = 0;
    ಆಗ (n_ಎಣಿಕೆ < n_Max_ಉದ್ದಳತೆ && ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ತಲೆ != ಖಾಲಿ)
    {
	ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ಉದ್ದಳತೆ--;
	ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್_ಗಂಟು_T *ತೆಗೆ_ತಲೆ = ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ತಲೆ;
	irEvents[n_ಎಣಿಕೆ++] = ತೆಗೆ_ತಲೆ->ir;
	ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ತಲೆ = ತೆಗೆ_ತಲೆ->ಮುಂದಿನ;
	vim_free(ತೆಗೆ_ತಲೆ);
	ಆದರೆ (ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ಉದ್ದಳತೆ == 0)
	    ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್.ಬಾಲ = ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ n_ಎಣಿಕೆ;
}
#ಇಲ್ಲದಿದ್ದರೆ  // FEAT_GUI_MSWIN && !VIMDLL
    ಶೂನ್ಯ
mch_setmouse(ಪೂರ್ಣಾಂಕ on ಬಳಸದ)
{
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_MSWIN && !VIMDLL

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
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
    ಪೂರ್ಣಾಂಕ
test_mswin_event(char_u *event, dict_T *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ lpEventsWritten = 0;

# ಆದರೆ defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ test_gui_w32_sendevent(event, ವಾದಗಳು);
# endif

# ಆದರೆ defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    ಒಳಹರಿವು_ದಾಖಲೆ ir;
    BOOL ಒಳಹರಿವು_encoded = FALSE;
    BOOL execute = FALSE;
    ಆದರೆ (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(ವಾದಗಳು, "execute", FALSE);
	ಆದರೆ (dict_has_ಕೀಲಿ(ವಾದಗಳು, "event"))
	    ಒಳಹರಿವು_encoded = encode_ಕೀಲಿ_event(ವಾದಗಳು, &ir);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!execute)
	{
	    semsg(_(e_missing_ವಾದ_str), "event");
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	ಒಳಹರಿವು_encoded = encode_mouse_event(ವಾದಗಳು, &ir);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "event", event);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    ಆದರೆ (ಒಳಹರಿವು_encoded)
    {
	ಆದರೆ ((lpEventsWritten = write_ಒಳಹರಿವು_ದಾಖಲೆ_ಬಫರ್(&ir, 1)) < 0)
	{
	    semsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ), "event");
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    ಆದರೆ (execute)
    {
	ಪೂರ್ಣಾಂಕ save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	ಸಂದೇಶ_scroll = FALSE;
	ch_log(ಖಾಲಿ, "test_mswin_event() executing");
	exec_ಸಾಮಾನ್ಯ(TRUE, TRUE, TRUE);
	ಸಂದೇಶ_scroll |= save_ಸಂದೇಶ_scroll;
    }

# endif
    ಹಿಂತಿರುಗಿ lpEventsWritten;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

#ಆದರೆ defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_ಗಣ_cursor_shape(ಪೂರ್ಣಾಂಕ thickness)
{
    ಆದರೆ (vtp_working)
    {
	ಆದರೆ (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    ಆದರೆ (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    ಇಲ್ಲದಿದ್ದರೆ
		vtp_printf("\033[0 q");	// default
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dw_ಗಾತ್ರ = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	ಗಣ_Console_Cursor_Info(g_h_Con_ಹೊರಹೋಗು, &ConsoleCursorInfo);
	ಆದರೆ (s_cursor_visible)
	    ಗಣ_Console_Cursor_Position(g_h_Con_ಹೊರಹೋಗು, g_coord);
    }
}

    ಶೂನ್ಯ
mch_update_cursor(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		thickness;

# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    ಆದರೆ (shape_ಕೋಷ್ಟಕ[idx].shape == SHAPE_ಖಂಡ)
	thickness = 99;	// 100 doesn't work on W95
    ಇಲ್ಲದಿದ್ದರೆ
	thickness = shape_ಕೋಷ್ಟಕ[idx].percentage;
    mch_ಗಣ_cursor_shape(thickness);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_focus_event(ಒಳಹರಿವು_ದಾಖಲೆ ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.b_ಗಣ_Focus;
    ui_focus_change((ಪೂರ್ಣಾಂಕ)g_fJustGotFocus);
}

ಸ್ಥಿರ ಶೂನ್ಯ ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
WaitForChar(ದೀರ್ಘ msec, ಪೂರ್ಣಾಂಕ ignore_ಒಳಹರಿವು)
{
    DWORD	    dwNow = 0, dw_ಕೊನೆ_ಸಮಯ = 0;
    ಒಳಹರಿವು_ದಾಖಲೆ    ir;
    DWORD	    c_ದಾಖಲೆಗಳು;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    ಪೂರ್ಣಾಂಕ		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    ಆದರೆ (msec > 0)
	// Wait until the specified time has elapsed.
	dw_ಕೊನೆ_ಸಮಯ = Get_Tick_ಎಣಿಕೆ() + msec;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (msec < 0)
	// Wait forever.
	dw_ಕೊನೆ_ಸಮಯ = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    ಗಾಗಿ (;;)
    {
	// Only process messages when waiting.
	ಆದರೆ (msec != 0)
	{
# ifdef ಸಂದೇಶ_ಸರತಿ
	    parse_queued_messages();
# endif
# ifdef FEAT_MZSCHEME
	    mzvim_check_ಎಳೆಗಳು();
# endif
# ifdef FEAT_CLIENTSERVER
	    serverProcessPendingMessages();
# endif
	}

	ಆದರೆ (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_ಒಳಹರಿವು && ಒಳಹರಿವು_ಲಭ್ಯ())
# endif
	   )
	    ಹಿಂತಿರುಗಿ TRUE;

	ಆದರೆ (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_ಎಣಿಕೆ();
	    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)(dwNow - dw_ಕೊನೆ_ಸಮಯ) >= 0)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (msec != 0)
	{
	    DWORD dw_Wait_ಸಮಯ = dw_ಕೊನೆ_ಸಮಯ - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    ಆದರೆ (dw_Wait_ಸಮಯ > 11)
		dw_Wait_ಸಮಯ = 11;

# ifdef FEAT_MZSCHEME
	    ಆದರೆ (mzthreads_allowed() && p_mzq > 0 && (ದೀರ್ಘ)dw_Wait_ಸಮಯ > p_mzq)
		dw_Wait_ಸಮಯ = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    ಆದರೆ (dw_Wait_ಸಮಯ > 10)
	    {
		ದೀರ್ಘ	due_ಸಮಯ;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_ಸಮಯ = check_due_timer();
		ಆದರೆ (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    ಹಿಂತಿರುಗಿ FALSE;
		}
		ಆದರೆ (due_ಸಮಯ > 0 && dw_Wait_ಸಮಯ > (DWORD)due_ಸಮಯ)
		    dw_Wait_ಸಮಯ = due_ಸಮಯ;
	    }
# endif
	    ಆದರೆ (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    ಸಂದೇಶ_wait_for_ಅಪವರ್ತ್ಯ_objects(1, &g_h_Con_ಒಳ, FALSE,
				  dw_Wait_ಸಮಯ, QS_SENDMESSAGE) != WAIT_OBJECT_0
# ಇಲ್ಲದಿದ್ದರೆ
		    wait_for_single_object(g_h_Con_ಒಳ, dw_Wait_ಸಮಯ)
							      != WAIT_OBJECT_0
# endif
		    )
		ಮುಂದುವರಿ;
	}

	c_ದಾಖಲೆಗಳು = 0;
	peek_console_ಒಳಹರಿವು(g_h_Con_ಒಳ, &ir, 1, &c_ದಾಖಲೆಗಳು);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	ಆದರೆ (State == ಬಹುಲಕ_CMDLINE && ಸಂದೇಶ_ಅಡ್ಡಸಾಲು == ಅಡ್ಡಸಾಲುಗಳು - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_ಬಫರ್_INFO csbi;

	    ಆದರೆ (Get_Console_Screen_ಬಫರ್_Info(g_h_Con_ಹೊರಹೋಗು, &csbi))
	    {
		ಆದರೆ (csbi.dwCursorPosition.Y != ಸಂದೇಶ_ಅಡ್ಡಸಾಲು)
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

	ಆದರೆ (c_ದಾಖಲೆಗಳು > 0)
	{
	    ಆದರೆ (ir.EventType == ಕೀಲಿ_EVENT && ir.Event.ಕೀಲಿ_Event_2.b_ಕೀಲಿ_ಕೆಳಕ್ಕೆ)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		ಆದರೆ (ir.Event.ಕೀಲಿ_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.ಕೀಲಿ_Event_2.w_Virtual_ಕೀಲಿ_ಸಂಕೇತ == 13)
		{
		    read_console_ಒಳಹರಿವು(g_h_Con_ಒಳ, &ir, 1, &c_ದಾಖಲೆಗಳು);
		    ಮುಂದುವರಿ;
		}
# endif
		ಆದರೆ (decode_ಕೀಲಿ_event(&ir.Event.ಕೀಲಿ_Event_2, &ch, &ch2,
								 ಖಾಲಿ, FALSE))
		    ಹಿಂತಿರುಗಿ TRUE;
	    }

	    read_console_ಒಳಹರಿವು(g_h_Con_ಒಳ, &ir, 1, &c_ದಾಖಲೆಗಳು);

	    ಆದರೆ (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ir.EventType == ಕಿಟಕಿ_ಬಫರ್_ಗಾತ್ರ_EVENT)
	    {
		COORD dw_ಗಾತ್ರ = ir.Event.ಕಿಟಕಿ_ಬಫರ್_ಗಾತ್ರ_Event.dw_ಗಾತ್ರ;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		ಆದರೆ (dw_ಗಾತ್ರ.X != Columns || dw_ಗಾತ್ರ.Y != ಅಡ್ಡಸಾಲುಗಳು)
		{
		    CONSOLE_SCREEN_ಬಫರ್_INFO csbi;
		    Get_Console_Screen_ಬಫರ್_Info(g_h_Con_ಹೊರಹೋಗು, &csbi);
		    dw_ಗಾತ್ರ.X = csbi.sr_ಕಿಟಕಿ.ಬಲ_4 - csbi.sr_ಕಿಟಕಿ.ಎಡ_4 + 1;
		    dw_ಗಾತ್ರ.Y = csbi.sr_ಕಿಟಕಿ.Bottom - csbi.sr_ಕಿಟಕಿ.ತುದಿ_2 + 1;
		    ಆದರೆ (dw_ಗಾತ್ರ.X != Columns || dw_ಗಾತ್ರ.Y != ಅಡ್ಡಸಾಲುಗಳು)
		    {
			ResizeConBuf(g_h_Con_ಹೊರಹೋಗು, dw_ಗಾತ್ರ);
			shell_resized();
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		ಹಿಂತಿರುಗಿ TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (msec == 0)
	    ನಿಲ್ಲಿಸು;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    ಆದರೆ (ಒಳಹರಿವು_ಲಭ್ಯ())
	ಹಿಂತಿರುಗಿ TRUE;
# endif

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * return non-zero if a character is available
 */
    ಪೂರ್ಣಾಂಕ
mch_char_avail(ಶೂನ್ಯ)
{
# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ TRUE;
# endif
    ಹಿಂತಿರುಗಿ WaitForChar(0L, FALSE);
}

# ಆದರೆ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ಪೂರ್ಣಾಂಕ
mch_check_messages(ಶೂನ್ಯ)
{
#  ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ TRUE;
#  endif
    ಹಿಂತಿರುಗಿ WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
create_conin(ಶೂನ್ಯ)
{
    g_h_Con_ಒಳ =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ಗುಣಲಕ್ಷಣಗಳು) ಖಾಲಿ,
			OPEN_EXISTING, 0, (HANDLE)ಖಾಲಿ);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    ಸ್ಥಿರ WCHAR
tgetch(ಪೂರ್ಣಾಂಕ *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    ಗಾಗಿ (;;)
    {
	ಒಳಹರಿವು_ದಾಖಲೆ ir;
	DWORD c_ದಾಖಲೆಗಳು = 0;

# ifdef FEAT_CLIENTSERVER
	(ಶೂನ್ಯ)WaitForChar(-1L, FALSE);
	ಆದರೆ (ಒಳಹರಿವು_ಲಭ್ಯ())
	    ಹಿಂತಿರುಗಿ 0;
	ಆದರೆ (g_nMouseClick != -1)
	    ಹಿಂತಿರುಗಿ 0;
# endif
	ಆದರೆ (read_console_ಒಳಹರಿವು(g_h_Con_ಒಳ, &ir, 1, &c_ದಾಖಲೆಗಳು) == 0)
	{
	    ಆದರೆ (did_create_conin)
		read_error_exit();
	    create_conin();
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (ir.EventType == ಕೀಲಿ_EVENT)
	{
	    ಆದರೆ (decode_ಕೀಲಿ_event(&ir.Event.ಕೀಲಿ_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		ಹಿಂತಿರುಗಿ ch;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ir.EventType == ಕಿಟಕಿ_ಬಫರ್_ಗಾತ್ರ_EVENT)
	    shell_resized();
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ir.EventType == MOUSE_EVENT)
	{
	    ಆದರೆ (decode_mouse_event(&ir.Event.MouseEvent))
		ಹಿಂತಿರುಗಿ 0;
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ // !FEAT_GUI_MSWIN


/*
 * mch_inchar(): low-level input function.
 * Get one or more characters from the keyboard or the mouse.
 * If time == 0, do not wait for characters.
 * If time == n, wait a short time for characters.
 * If time == -1, wait forever for characters.
 * Returns the number of characters read into buf.
 */
    ಪೂರ್ಣಾಂಕ
mch_inchar(
    char_u	*buf ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		maxlen ಬಳಸದ,
    ದೀರ್ಘ	ಸಮಯ ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		tb_change_cnt ಬಳಸದ)
{
#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# ಇಲ್ಲದಿದ್ದರೆ
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    ಸ್ಥಿರ char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    typeaheadlen = 0;

# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    ಆದರೆ (typeaheadlen > 0)
	ಹೋಗು theend;

    ಆದರೆ (ಸಮಯ >= 0)
    {
	ಆದರೆ (!WaitForChar(ಸಮಯ, FALSE))     // no character available
	    ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ    // time == -1, wait forever
    {
	mch_ಗಣ_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ಆದರೆ (!WaitForChar(p_ut, FALSE))
	{
	    ಆದರೆ (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (ಪೂರ್ಣಾಂಕ)KE_CURSORHOLD;
		ಹಿಂತಿರುಗಿ 3;
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
    ಆದರೆ (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    ಆಗ ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	ಆದರೆ (typebuf_ಬದಲಾಯಿತು(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    ಆದರೆ (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    ಆದರೆ (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		ಆದರೆ (modifiers > 0)
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
	    ಇಲ್ಲದಿದ್ದರೆ
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
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    WCHAR	ch2 = NUL;
	    ಪೂರ್ಣಾಂಕ		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_ಕೀಲಿ(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_ಕೀಲಿಗಳು;
	    c = may_adjust_ಕೀಲಿ_for_ctrl(modifiers, c);
	    --no_reduce_ಕೀಲಿಗಳು;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_ಸರಿಸು_modifier(modifiers, c);

	    ಆದರೆ (typebuf_ಬದಲಾಯಿತು(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (c == Ctrl_C && ctrl_c_interrupts)
	    {
# ಆದರೆ defined(FEAT_CLIENTSERVER)
		trash_ಒಳಹರಿವು_buf();
# endif
		got_int = TRUE;
	    }

	    ಆದರೆ (g_nMouseClick == -1)
	    {
		ಪೂರ್ಣಾಂಕ	n = 1;

		ಆದರೆ (ch2 == NUL)
		{
		    ಪೂರ್ಣಾಂಕ	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    ಆದರೆ (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_ಗಮ್ಯ_enc(ch, &n);
		    ಆದರೆ (p != ಖಾಲಿ)
		    {
			ಗಾಗಿ (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    ಆದರೆ (p[i] == CSI)
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
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    ಆದರೆ (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		ಆದರೆ (ch2 != NUL)
		{
		    ಆದರೆ (c == K_NUL)
		    {
			ಆಯ್ಕೆ (ch2)
			{
			ಸ್ಥಿತಿ (WCHAR)'\324': // SHIFT+Insert
			ಸ್ಥಿತಿ (WCHAR)'\325': // CTRL+Insert
			ಸ್ಥಿತಿ (WCHAR)'\327': // SHIFT+Delete
			ಸ್ಥಿತಿ (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    ನಿಲ್ಲಿಸು;

			ಪೂರ್ವನಿಯೋಜಿತ:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
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
		ಆದರೆ ((modifiers & MOD_ಮುಖವಾಡ_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_ಮುಖವಾಡ_ALT;
		}

		ಆದರೆ (modifiers != 0)
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
		ಆದರೆ (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    ಆದರೆ (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    ಆಗ (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    ಆದರೆ (len > 0)
    {
	buf[len] = NUL;
	ch_log(ಖಾಲಿ, "raw key input: \"%s\"", buf);
    }
# endif
    ಹಿಂತಿರುಗಿ len;

#ಇಲ್ಲದಿದ್ದರೆ // FEAT_GUI_MSWIN && !VIMDLL
    ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
executable_file(ಅಕ್ಷರ *name, char_u **ಮಾರ್ಗ)
{
    ಪೂರ್ಣಾಂಕ attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    ಆದರೆ (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	ಹಿಂತಿರುಗಿ FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    ಆದರೆ (attrs & FILE_ATTRIBUTE_REPARSE_ಬಿಂದು)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	ಆದರೆ (res == ಖಾಲಿ)
	    res = resolve_reparse_ಬಿಂದು((char_u *)name);
	ಆದರೆ (res == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;
	// The path is already absolute.
	ಆದರೆ (ಮಾರ್ಗ != ಖಾಲಿ)
	    *ಮಾರ್ಗ = res;
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_free(res);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮಾರ್ಗ != ಖಾಲಿ)
	*ಮಾರ್ಗ = ತುಂಬಿದ_Name_save((char_u *)name, FALSE);
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    ಸ್ಥಿರ bool
executable_ಪೂರ್ವಪ್ರತ್ಯಯ_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_ಕಂಡುಹಿಡಿ_DATAW	ದತ್ತಾಂಶ;
    HANDLE		handle;
    DWORD		error;

    wn = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (wn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ true;
    handle = ಕಂಡುಹಿಡಿ_ಮೊದಲ_File_Ex_W(wn, ಕಂಡುಹಿಡಿ_Ex_Info_Basic, &ದತ್ತಾಂಶ,
					    ಕಂಡುಹಿಡಿ_Ex_ಹುಡುಕು_Name_ಹೊಂದಿಕೆ, ಖಾಲಿ, 0);
    error = Get_ಕೊನೆಯ_Error();
    vim_free(wn);
    ಆದರೆ (handle != INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	ಕಂಡುಹಿಡಿ_Close(handle);
	ಹಿಂತಿರುಗಿ true;
    }
    ಹಿಂತಿರುಗಿ error != ERROR_FILE_NOT_ಸಿಕ್ಕಿತು && error != ERROR_ಮಾರ್ಗ_NOT_ಸಿಕ್ಕಿತು;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
executable_exists(
    ಅಕ್ಷರ	*name,
    size_t	namelen,
    char_u	**ಮಾರ್ಗ,
    ಪೂರ್ಣಾಂಕ		use_ಮಾರ್ಗ,
    ಪೂರ್ಣಾಂಕ		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[MAX_ಮಾರ್ಗ_2 * 3];
    char_u	*p;
    string_T	pathbuf = {ಖಾಲಿ, 0};
    ಪೂರ್ಣಾಂಕ		pathbuf_allocated = FALSE;
    string_T	pathext = {ಖಾಲಿ, 0};
    ಪೂರ್ಣಾಂಕ		pathext_allocated = FALSE;
    char_u	*shname = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		noext = FALSE;
    bool	simple_pathext = true;
    ಪೂರ್ಣಾಂಕ		ext_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;

    ಆದರೆ (namelen >= ಗಾತ್ರ(buf))	// safety check
	ಹಿಂತಿರುಗಿ FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)shname, "sh") != ಖಾಲಿ &&
	!(ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)shname, "powershell") != ಖಾಲಿ
				    || ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)shname, "pwsh") != ಖಾಲಿ))
	noext = TRUE;

    ಆದರೆ (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	ಆದರೆ (pathext.string == ಖಾಲಿ)
	    STR_LITERAL_ಗಣ(pathext, ".com;.exe;.bat;.cmd");
	ಇಲ್ಲದಿದ್ದರೆ
	    pathext.ಉದ್ದಳತೆ = STRLEN(pathext.string);

	ಆದರೆ (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    ಆಗ (*p)
	    {
		ಆದರೆ (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    ಆದರೆ (*p == '.')
			simple_pathext = false;
		    ++p;
		    ಮುಂದುವರಿ;
		}
		e = vim_strchr(p, ';');
		ಆದರೆ (e == ಖಾಲಿ)
		    e = pathext.string + pathext.ಉದ್ದಳತೆ;
		plen = (size_t)(e - p);
		++ext_ಎಣಿಕೆ;
		ಆದರೆ (*p != '.')
		    simple_pathext = false;
		ಗಾಗಿ (q = p + 1; q < e; ++q)
		    ಆದರೆ (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		ಆದರೆ (_strnicoll(name + namelen - plen, (ಅಕ್ಷರ *)p, plen) == 0)
		{
		    noext = TRUE;
		    ನಿಲ್ಲಿಸು;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    ಆದರೆ (pathext.string == ಖಾಲಿ)
	STR_LITERAL_ಗಣ(pathext, ".");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.ಉದ್ದಳತೆ + 3);
	ಆದರೆ (tmp == ಖಾಲಿ)
	{
	    retval = FALSE;
	    ಹೋಗು theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.ಉದ್ದಳತೆ += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    ಆದರೆ (use_ಮಾರ್ಗ && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    ಆದರೆ (pathbuf.string == ಖಾಲಿ)
	    {
		retval = FALSE;
		ಹೋಗು theend;
	    }

	    ಆದರೆ (mch_getenv("NoDefaultCurrentDirectoryInExePath") == ಖಾಲಿ &&
		    ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)gettail(p_sh), "cmd.exe") != ಖಾಲಿ)
	    {
		STRCPY(pathbuf.string, ".;");
		pathbuf.ಉದ್ದಳತೆ = 2;
	    }
	    STRCPY(pathbuf.string + pathbuf.ಉದ್ದಳತೆ, p);
	    pathbuf.ಉದ್ದಳತೆ += plen;
	    pathbuf_allocated = TRUE;
	}
    }

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    ಆದರೆ (pathbuf.string == ಖಾಲಿ)
	STR_LITERAL_ಗಣ(pathbuf, ".");
    p = pathbuf.string;
    ಆಗ (*p)
    {
	char_u	*e;
	size_t	buflen;

	ಆದರೆ (*p == ';') // Skip empty entry
	{
	    ++p;
	    ಮುಂದುವರಿ;
	}
	e = vim_strchr(p, ';');
	ಆದರೆ (e == ಖಾಲಿ)
	    e = pathbuf.string + pathbuf.ಉದ್ದಳತೆ;

	ಆದರೆ (e - p + namelen + 2 > ಗಾತ್ರ(buf))
	{
	    retval = FALSE;
	    ಹೋಗು theend;
	}
	// A single "." that means current dir.
	ಆದರೆ (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    buflen = vim_snprintf_safelen(
		(ಅಕ್ಷರ *)buf,
		ಗಾತ್ರ(buf),
		"%.*s%s%s", (ಪೂರ್ಣಾಂಕ)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	ಆದರೆ (!noext && simple_pathext && ext_ಎಣಿಕೆ >= 3 && buflen > 0
		&& buflen + 2 <= ಗಾತ್ರ(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == ಖಾಲಿ
		&& vim_strchr(gettail(buf), ':') == ಖಾಲಿ)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_ಪೂರ್ವಪ್ರತ್ಯಯ_exists(buf);
	    buf[buflen] = NUL;
	    ಆದರೆ (!exists)
	    {
		p = e;
		ಮುಂದುವರಿ;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	ಆಗ (*p)
	{
	    char_u  *e2;

	    ಆದರೆ (*p == ';')
	    {
		// Skip empty entry
		++p;
		ಮುಂದುವರಿ;
	    }
	    e2 = vim_strchr(p, ';');
	    ಆದರೆ (e2 == ಖಾಲಿ)
		e2 = pathext.string + pathext.ಉದ್ದಳತೆ;

	    ಆದರೆ (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		ಆದರೆ (e2 - p + buflen + 1 > ಗಾತ್ರ(buf))
		{
		    retval = FALSE;
		    ಹೋಗು theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    ಆದರೆ (executable_file((ಅಕ್ಷರ *)buf, ಮಾರ್ಗ))
	    {
		retval = TRUE;
		ಹೋಗು theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    ಆದರೆ (pathbuf_allocated)
	ಬಿಡುಗಡೆ(pathbuf.string);
    ಆದರೆ (pathext_allocated)
	ಬಿಡುಗಡೆ(pathext.string);
    ಹಿಂತಿರುಗಿ retval;
}

#ಆದರೆ (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
bad_param_handler(ಸ್ಥಿರಾಂಕ wchar_t *ಅಭಿವ್ಯಕ್ತಿ_2 ಬಳಸದ,
    ಸ್ಥಿರಾಂಕ wchar_t *function ಬಳಸದ,
    ಸ್ಥಿರಾಂಕ wchar_t *file ಬಳಸದ,
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸಾಲು ಬಳಸದ,
    uintptr_t pReserved ಬಳಸದ)
{
}

# define ಗಣ_INVALID_PARAM_HANDLER \
	((ಶೂನ್ಯ)ಗಣ_invalid_parameter_handler(bad_param_handler))
#ಇಲ್ಲದಿದ್ದರೆ
# define ಗಣ_INVALID_PARAM_HANDLER
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_init_g(ಶೂನ್ಯ)
{
# ifndef __MINGW32__
    ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    ಗಣ_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    ಗಣ_Error_ಬಹುಲಕ(SEM_FAILCRITICALERRORS);

    _fmode = O_ದ್ವಿಮಾನ;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    ಅಡ್ಡಸಾಲುಗಳು = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[MAX_ಮಾರ್ಗ_2 + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	ಆದರೆ (exe_pathlen + 10 >= ಗಾತ್ರ(vimrun_location))
	{
	    ಆದರೆ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ಖಾಲಿ, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // First try in same directory as gvim.exe.
	    ಆದರೆ (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    ಆದರೆ (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		ಆದರೆ (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(ಅಕ್ಷರ *)vimrun_location,
			ಗಾತ್ರ(vimrun_location),
			"\"%.*svimrun\" ",
			(ಪೂರ್ಣಾಂಕ)exe_pathlen, exe_name);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		ಆದರೆ (p != ಖಾಲಿ)
		{
		    vimrun_ಮಾರ್ಗ.string = p;
		    vimrun_ಮಾರ್ಗ.ಉದ್ದಳತೆ = plen;
		    vimrun_ಮಾರ್ಗ_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ಖಾಲಿ, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	ಆದರೆ (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    ಆದರೆ (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    ಖಾಲಿ, TRUE, FALSE))
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_MSWIN

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).ಬಲ_4 - (sr).ಎಡ_4 + 1)
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
Clear_Console_ಬಫರ್(ಶಬ್ದ_2 wAttribute)
{
    CONSOLE_SCREEN_ಬಫರ್_INFO csbi;
    COORD coord;
    DWORD ಅಂಕೆ_Cells, dummy;

    ಆದರೆ (!Get_Console_Screen_ಬಫರ್_Info(g_h_Con_ಹೊರಹೋಗು, &csbi))
	ಹಿಂತಿರುಗಿ FALSE;

    ಅಂಕೆ_Cells = csbi.dw_ಗಾತ್ರ.X * csbi.dw_ಗಾತ್ರ.Y;
    coord.X = 0;
    coord.Y = 0;
    ಆದರೆ (!FillConsoleOutputCharacter(g_h_Con_ಹೊರಹೋಗು, ' ', ಅಂಕೆ_Cells,
	    coord, &dummy))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (!FillConsoleOutputAttribute(g_h_Con_ಹೊರಹೋಗು, wAttribute, ಅಂಕೆ_Cells,
	    coord, &dummy))
	ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
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
Fit_Console_ಕಿಟಕಿ(
    COORD dw_ಬಫರ್_ಗಾತ್ರ,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_ಬಫರ್_INFO csbi;
    COORD dw_ಕಿಟಕಿ_ಗಾತ್ರ;
    BOOL NeedAdjust = FALSE;

    ಆದರೆ (!Get_Console_Screen_ಬಫರ್_Info(g_h_Con_ಹೊರಹೋಗು, &csbi))
	ಹಿಂತಿರುಗಿ FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    ಆದರೆ (csbi.sr_ಕಿಟಕಿ.ಬಲ_4 >= dw_ಬಫರ್_ಗಾತ್ರ.X)
    {
	dw_ಕಿಟಕಿ_ಗಾತ್ರ.X = SRWIDTH(csbi.sr_ಕಿಟಕಿ);
	ಆದರೆ (dw_ಕಿಟಕಿ_ಗಾತ್ರ.X > dw_ಬಫರ್_ಗಾತ್ರ.X)
	    dw_ಕಿಟಕಿ_ಗಾತ್ರ.X = dw_ಬಫರ್_ಗಾತ್ರ.X;
	csbi.sr_ಕಿಟಕಿ.ಬಲ_4 = dw_ಬಫರ್_ಗಾತ್ರ.X - 1;
	csbi.sr_ಕಿಟಕಿ.ಎಡ_4 = dw_ಬಫರ್_ಗಾತ್ರ.X - dw_ಕಿಟಕಿ_ಗಾತ್ರ.X;
	NeedAdjust = TRUE;
    }
    ಆದರೆ (csbi.sr_ಕಿಟಕಿ.Bottom >= dw_ಬಫರ್_ಗಾತ್ರ.Y)
    {
	dw_ಕಿಟಕಿ_ಗಾತ್ರ.Y = SRHEIGHT(csbi.sr_ಕಿಟಕಿ);
	ಆದರೆ (dw_ಕಿಟಕಿ_ಗಾತ್ರ.Y > dw_ಬಫರ್_ಗಾತ್ರ.Y)
	    dw_ಕಿಟಕಿ_ಗಾತ್ರ.Y = dw_ಬಫರ್_ಗಾತ್ರ.Y;
	csbi.sr_ಕಿಟಕಿ.Bottom = dw_ಬಫರ್_ಗಾತ್ರ.Y - 1;
	csbi.sr_ಕಿಟಕಿ.ತುದಿ_2 = dw_ಬಫರ್_ಗಾತ್ರ.Y - dw_ಕಿಟಕಿ_ಗಾತ್ರ.Y;
	NeedAdjust = TRUE;
    }
    ಆದರೆ (NeedAdjust && WantAdjust)
    {
	ಆದರೆ (!ಗಣ_Console_ಕಿಟಕಿ_Info(g_h_Con_ಹೊರಹೋಗು, TRUE, &csbi.sr_ಕಿಟಕಿ))
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Console_ಬಫರ್_Struct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_ಬಫರ್_INFO	Info;
    PCHAR_INFO			ಬಫರ್_2;
    COORD			ಬಫರ್_ಗಾತ್ರ_2;
    PSMALL_RECT			Regions;
    ಪೂರ್ಣಾಂಕ				ಅಂಕೆ_Regions;
} Console_ಬಫರ್;

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
Save_Console_ಬಫರ್(
    Console_ಬಫರ್ *cb)
{
    DWORD ಅಂಕೆ_Cells;
    COORD ಬಫರ್_Coord;
    SMALL_RECT ReadRegion;
    ಶಬ್ದ_2 Y, Y_incr;
    ಪೂರ್ಣಾಂಕ i, numregions;

    ಆದರೆ (cb == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (!Get_Console_Screen_ಬಫರ್_Info(g_h_Con_ಹೊರಹೋಗು, &cb->Info))
    {
	cb->IsValid = FALSE;
	ಹಿಂತಿರುಗಿ FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    ಆದರೆ (use_alternate_screen_ಬಫರ್)
	ಹಿಂತಿರುಗಿ TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    ಆದರೆ (!cb->IsValid || cb->ಬಫರ್_2 == ಖಾಲಿ ||
	    cb->ಬಫರ್_ಗಾತ್ರ_2.X != cb->Info.dw_ಗಾತ್ರ.X ||
	    cb->ಬಫರ್_ಗಾತ್ರ_2.Y != cb->Info.dw_ಗಾತ್ರ.Y)
    {
	cb->ಬಫರ್_ಗಾತ್ರ_2.X = cb->Info.dw_ಗಾತ್ರ.X;
	cb->ಬಫರ್_ಗಾತ್ರ_2.Y = cb->Info.dw_ಗಾತ್ರ.Y;
	ಅಂಕೆ_Cells = cb->ಬಫರ್_ಗಾತ್ರ_2.X * cb->ಬಫರ್_ಗಾತ್ರ_2.Y;
	vim_free(cb->ಬಫರ್_2);
	cb->ಬಫರ್_2 = ALLOC_MULT(CHAR_INFO, ಅಂಕೆ_Cells);
	ಆದರೆ (cb->ಬಫರ್_2 == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;
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
    ಬಫರ್_Coord.X = 0;
    ReadRegion.ಎಡ_4 = 0;
    ReadRegion.ಬಲ_4 = cb->Info.dw_ಗಾತ್ರ.X - 1;
    Y_incr = 12000 / cb->Info.dw_ಗಾತ್ರ.X;

    numregions = (cb->Info.dw_ಗಾತ್ರ.Y + Y_incr - 1) / Y_incr;
    ಆದರೆ (cb->Regions == ಖಾಲಿ || numregions != cb->ಅಂಕೆ_Regions)
    {
	cb->ಅಂಕೆ_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->ಅಂಕೆ_Regions);
	ಆದರೆ (cb->Regions == ಖಾಲಿ)
	{
	    VIM_CLEAR(cb->ಬಫರ್_2);
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }

    ಗಾಗಿ (i = 0, Y = 0; i < cb->ಅಂಕೆ_Regions; i++, Y += Y_incr)
    {
	/*
	 * Read into position (0, Y) in our buffer.
	 */
	ಬಫರ್_Coord.Y = Y;
	/*
	 * Read the region whose top left corner is (0, Y) and whose bottom
	 * right corner is (width - 1, Y + Y_incr - 1).  This should define
	 * a region of size width by Y_incr.  Don't worry if this region is
	 * too large for the remaining buffer; it will be cropped.
	 */
	ReadRegion.ತುದಿ_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	ಆದರೆ (!ReadConsoleOutputW(g_h_Con_ಹೊರಹೋಗು,	// output handle
		cb->ಬಫರ್_2,			// our buffer
		cb->ಬಫರ್_ಗಾತ್ರ_2,			// dimensions of our buffer
		ಬಫರ್_Coord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->ಬಫರ್_2);
	    VIM_CLEAR(cb->Regions);
	    ಹಿಂತಿರುಗಿ FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    ಹಿಂತಿರುಗಿ TRUE;
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
Restore_Console_ಬಫರ್(
    Console_ಬಫರ್   *cb,
    BOOL	    RestoreScreen)
{
    COORD ಬಫರ್_Coord;
    SMALL_RECT WriteRegion;
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (cb == ಖಾಲಿ || !cb->IsValid)
	ಹಿಂತಿರುಗಿ FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    ಆದರೆ (RestoreScreen)
	Clear_Console_ಬಫರ್(cb->Info.w_ಗುಣಲಕ್ಷಣಗಳು);

    Fit_Console_ಕಿಟಕಿ(cb->Info.dw_ಗಾತ್ರ, TRUE);
    ಆದರೆ (!ಗಣ_Console_Screen_ಬಫರ್_ಗಾತ್ರ(g_h_Con_ಹೊರಹೋಗು, cb->Info.dw_ಗಾತ್ರ))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (!ಗಣ_Console_ಪಠ್ಯ_Attribute(g_h_Con_ಹೊರಹೋಗು, cb->Info.w_ಗುಣಲಕ್ಷಣಗಳು))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಆದರೆ (!ಗಣ_Console_Cursor_Position(g_h_Con_ಹೊರಹೋಗು, cb->Info.dwCursorPosition))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (!ಗಣ_Console_ಕಿಟಕಿ_Info(g_h_Con_ಹೊರಹೋಗು, TRUE, &cb->Info.sr_ಕಿಟಕಿ))
	ಹಿಂತಿರುಗಿ FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    ಆದರೆ (use_alternate_screen_ಬಫರ್)
	ಹಿಂತಿರುಗಿ TRUE;

    /*
     * Restore the screen buffer contents.
     */
    ಆದರೆ (cb->ಬಫರ್_2 != ಖಾಲಿ)
    {
	ಗಾಗಿ (i = 0; i < cb->ಅಂಕೆ_Regions; i++)
	{
	    ಬಫರ್_Coord.X = cb->Regions[i].ಎಡ_4;
	    ಬಫರ್_Coord.Y = cb->Regions[i].ತುದಿ_2;
	    WriteRegion = cb->Regions[i];
	    ಆದರೆ (!WriteConsoleOutputW(g_h_Con_ಹೊರಹೋಗು,	// output handle
			cb->ಬಫರ್_2,		// our buffer
			cb->ಬಫರ್_ಗಾತ್ರ_2,		// dimensions of our buffer
			ಬಫರ್_Coord,		// offset in our buffer
			&WriteRegion))		// region to restore
		ಹಿಂತಿರುಗಿ FALSE;
	}
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
ಸ್ಥಿರ Console_ಬಫರ್ g_cbOrig = { 0 };
# endif
ಸ್ಥಿರ Console_ಬಫರ್ g_cbNonTermcap = { 0 };
ಸ್ಥಿರ Console_ಬಫರ್ g_cbTermcap = { 0 };

ಅಕ್ಷರ g_szOrigTitle[256] = { 0 };
HWND g_hWnd = ಖಾಲಿ;	// also used in os_mswin.c
ಸ್ಥಿರ HICON g_hOrigIconSmall = ಖಾಲಿ;
ಸ್ಥಿರ HICON g_hOrigIcon = ಖಾಲಿ;
ಸ್ಥಿರ HICON g_hVimIcon = ಖಾಲಿ;
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
    ಆದರೆ (hWnd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (phIconSmall != ಖಾಲಿ)
	*phIconSmall = (HICON)Send_ಸಂದೇಶ(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    ಆದರೆ (phIcon != ಖಾಲಿ)
	*phIcon = (HICON)Send_ಸಂದೇಶ(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    ಹಿಂತಿರುಗಿ TRUE;
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
ಗಣ_Console_Icon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    ಆದರೆ (hWnd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (hIconSmall != ಖಾಲಿ)
	Send_ಸಂದೇಶ(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    ಆದರೆ (hIcon != ಖಾಲಿ)
	Send_ಸಂದೇಶ(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    ಹಿಂತಿರುಗಿ TRUE;
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
    ಆದರೆ (!GetConsoleTitle(g_szOrigTitle, ಗಾತ್ರ(g_szOrigTitle)))
	ಹಿಂತಿರುಗಿ;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = Get_Console_ಕಿಟಕಿ();
    ಆದರೆ (g_hWnd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    ಆದರೆ (g_hOrigIconSmall == ಖಾಲಿ || g_hOrigIcon == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Extract the first icon contained in the Vim executable.
    ಆದರೆ (
# ifdef FEAT_LIBCALL
	    mch_icon_ತುಂಬು((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == ಖಾಲಿ)
	g_hVimIcon = ಹೊರತೆಗೆ_Icon(ಖಾಲಿ, (LPCSTR)exe_name, 0);
    ಆದರೆ (g_hVimIcon != ಖಾಲಿ)
	g_fCanChangeIcon = TRUE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_fWindInitCalled = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ g_f_Termcap_ಬಹುಲಕ = FALSE;
ಸ್ಥಿರ CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_init_c(ಶೂನ್ಯ)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_ಬಫರ್_INFO csbi;
# endif
# ifndef __MINGW32__
    ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    ಗಣ_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    ಗಣ_Error_ಬಹುಲಕ(SEM_FAILCRITICALERRORS);

    _fmode = O_ದ್ವಿಮಾನ;		// we do our own CR-LF translation
    ಹೊರಹೋಗು_flush();

    // Obtain handles for the standard Console I/O devices
    ಆದರೆ (read_cmd_fd == 0)
	g_h_Con_ಒಳ =  GetStdHandle(STD_ಒಳಹರಿವು_HANDLE);
    ಇಲ್ಲದಿದ್ದರೆ
	create_conin();
    g_h_Con_ಹೊರಹೋಗು = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    Save_Console_ಬಫರ್(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.w_ಗುಣಲಕ್ಷಣಗಳು;
# ಇಲ್ಲದಿದ್ದರೆ
    // Get current text attributes
    Get_Console_Screen_ಬಫರ್_Info(g_h_Con_ಹೊರಹೋಗು, &csbi);
    g_attrCurrent = g_attrDefault = csbi.w_ಗುಣಲಕ್ಷಣಗಳು;
# endif
    ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_fg_color == 0)
	cterm_ಸಾಮಾನ್ಯ_fg_color = (g_attrCurrent & 0xf) + 1;
    ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_bg_color == 0)
	cterm_ಸಾಮಾನ್ಯ_bg_color = ((g_attrCurrent >> 4) & 0xf) + 1;

    // Fg and Bg color index number at startup
    g_color_ಸೂಚ್ಯಂಕ_fg = g_attrDefault & 0xf;
    g_color_ಸೂಚ್ಯಂಕ_bg = (g_attrDefault >> 4) & 0xf;

    // set termcap codes to current text attributes
    update_tcap(g_attrCurrent);

    GetConsoleCursorInfo(g_h_Con_ಹೊರಹೋಗು, &g_cci);
    Get_Console_ಬಹುಲಕ(g_h_Con_ಒಳ,  &g_cmodein);
    Get_Console_ಬಹುಲಕ(g_h_Con_ಹೊರಹೋಗು, &g_cmodeout);

    SaveConsoleTitleAndIcon();
    /*
     * Set both the small and big icons of the console window to Vim's icon.
     * Note that Vim presently only has one size of icon (32x32), but it
     * automatically gets scaled down to 16x16 when setting the small icon.
     */
    ಆದರೆ (g_fCanChangeIcon)
	ಗಣ_Console_Icon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    ಆದರೆ (use_alternate_screen_ಬಫರ್)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    ಆದರೆ (fdDump)
    {
	time_t t;

	ಸಮಯ(&t);
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
mch_exit_c(ಪೂರ್ಣಾಂಕ r)
{
    // Copy flag since stoptermcap() will clear the flag.
    ಪೂರ್ಣಾಂಕ f_Termcap_ಬಹುಲಕ = g_f_Termcap_ಬಹುಲಕ;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    ಆದರೆ (!f_Termcap_ಬಹುಲಕ &&  use_alternate_screen_ಬಫರ್)
	vtp_printf("\033[?1049l");

    ಆದರೆ (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    ಆದರೆ (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	ಆದರೆ (g_fCanChangeIcon && !g_fForceExit)
	    ಗಣ_Console_Icon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	ಆದರೆ (fdDump)
	{
	    time_t t;

	    ಸಮಯ(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = ಖಾಲಿ;
# endif
    }

    ಗಣ_Console_Cursor_Info(g_h_Con_ಹೊರಹೋಗು, &g_cci);
    ಗಣ_Console_ಬಹುಲಕ(g_h_Con_ಒಳ,  g_cmodein | ENABLE_EXTENDED_FLAGS);
    ಗಣ_Console_ಬಹುಲಕ(g_h_Con_ಹೊರಹೋಗು, g_cmodeout);

# ifdef DYNAMIC_GETTEXT
    dyn_libintl_ಕೊನೆ();
# endif

    ನಿರ್ಗಮಿಸು(r);
}
#ಆದರೆ_ಅಂತ್ಯ // !FEAT_GUI_MSWIN || VIMDLL

    ಶೂನ್ಯ
mch_init(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VIMDLL
    ಆದರೆ (gui.starting)
	mch_init_g();
    ಇಲ್ಲದಿದ್ದರೆ
	mch_init_c();
#ಅಥವಾಆದರೆ defined(FEAT_GUI_MSWIN)
    mch_init_g();
#ಇಲ್ಲದಿದ್ದರೆ
    mch_init_c();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
mch_exit(ಪೂರ್ಣಾಂಕ r)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VIMDLL
    ಆದರೆ (vimrun_ಮಾರ್ಗ_allocated)
	vim_free(vimrun_ಮಾರ್ಗ.string);

    ಆದರೆ (gui.ಒಳ_use || gui.starting)
	mch_exit_g(r);
    ಇಲ್ಲದಿದ್ದರೆ
	mch_exit_c(r);
#ಅಥವಾಆದರೆ defined(FEAT_GUI_MSWIN)
    ಆದರೆ (vimrun_ಮಾರ್ಗ_allocated)
	vim_free(vimrun_ಮಾರ್ಗ.string);
    mch_exit_g(r);
#ಇಲ್ಲದಿದ್ದರೆ
    mch_exit_c(r);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Do we have an interactive window?
 */
    ಪೂರ್ಣಾಂಕ
mch_check_win(
    ಪೂರ್ಣಾಂಕ argc ಬಳಸದ,
    ಅಕ್ಷರ **argv ಬಳಸದ)
{
    mch_get_exe_name();

#ಆದರೆ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    ಹಿಂತಿರುಗಿ OK;	    // GUI always has a tty
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ OK;
# endif
    ಆದರೆ (isatty(1))
	ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    ಶೂನ್ಯ
fname_case(
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		len)
{
    WCHAR   *p;
    WCHAR   buf[MAX_ಮಾರ್ಗ_2 + 1];

    ಆದರೆ (*name == NUL)
	ಹಿಂತಿರುಗಿ;

    slash_adjust(name);

    p = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (Get_Long_ಮಾರ್ಗ_Name_W(p, buf, MAX_ಮಾರ್ಗ_2))
    {
	char_u	*q = utf16_ಗಮ್ಯ_enc(buf, ಖಾಲಿ);

	ಆದರೆ (q != ಖಾಲಿ)
	{
	    ಆದರೆ (len > 0)
		vim_strncpy(name, q, len - 1);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		size_t  namelen = STRLEN(name);

		ಆದರೆ (namelen >= STRLEN(q))
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
    ಪೂರ್ಣಾಂಕ
mch_get_user_name(
    char_u  *s,
    ಪೂರ್ಣಾಂಕ	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_ಉದ್ದಳತೆ(wszUserName);

    ಆದರೆ (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_ಗಮ್ಯ_enc(wszUserName, ಖಾಲಿ);

	ಆದರೆ (p != ಖಾಲಿ)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    ಹಿಂತಿರುಗಿ OK;
	}
    }
    s[0] = NUL;
    ಹಿಂತಿರುಗಿ FAIL;
}


/*
 * Insert host name in s[len].
 */
    ಶೂನ್ಯ
mch_get_host_name(
    char_u	*s,
    ಪೂರ್ಣಾಂಕ		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_ಉದ್ದಳತೆ(wszHostName);

    ಆದರೆ (!GetComputerNameW(wszHostName, &wcch))
	ಹಿಂತಿರುಗಿ;

    char_u  *p = utf16_ಗಮ್ಯ_enc(wszHostName, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    ದೀರ್ಘ
mch_get_pid(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    ಪೂರ್ಣಾಂಕ
mch_process_running(ದೀರ್ಘ pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   ಸ್ಥಿತಿ_2 = 0;
    ಪೂರ್ಣಾಂಕ	    ret = FALSE;

    ಆದರೆ (hProcess == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;  // might not have access
    ಆದರೆ (Get_Exit_ಸಂಕೇತ_Process(hProcess, &ಸ್ಥಿತಿ_2))
	ret = ಸ್ಥಿತಿ_2 == STILL_ಸಕ್ರಿಯ;
    CloseHandle(hProcess);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    ಪೂರ್ಣಾಂಕ
mch_dirname(
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		len)
{
    WCHAR   wbuf[MAX_ಮಾರ್ಗ_2 + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    ಆದರೆ (GetCurrentDirectoryW(MAX_ಮಾರ್ಗ_2, wbuf) == 0)
	ಹಿಂತಿರುಗಿ FAIL;

    WCHAR   wcbuf[MAX_ಮಾರ್ಗ_2 + 1];
    char_u  *p = ಖಾಲಿ;

    ಆದರೆ (Get_Long_ಮಾರ್ಗ_Name_W(wbuf, wcbuf, MAX_ಮಾರ್ಗ_2) != 0)
    {
	p = utf16_ಗಮ್ಯ_enc(wcbuf, ಖಾಲಿ);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಆದರೆ (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    ಆದರೆ (p == ಖಾಲಿ)
	p = utf16_ಗಮ್ಯ_enc(wbuf, ಖಾಲಿ);

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    ದೀರ್ಘ
mch_getperm(char_u *name)
{
    stat_T	st;

    ಹಿಂತಿರುಗಿ (mch_stat((ಅಕ್ಷರ *)name, &st) == 0)
	? (ದೀರ್ಘ)(ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)st.st_ಬಹುಲಕ
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
mch_setperm(char_u *name, ದೀರ್ಘ perm)
{
    ದೀರ್ಘ	n;
    WCHAR	*p;

    p = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    ಆದರೆ (n == -1)
	ಹಿಂತಿರುಗಿ FAIL;

    win32_ಗಣ_archive(name);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set hidden flag for "name".
 */
    ಶೂನ್ಯ
mch_hide(char_u *name)
{
    ಪೂರ್ಣಾಂಕ attrs = win32_getattrs(name);
    ಆದರೆ (attrs == -1)
	ಹಿಂತಿರುಗಿ;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    ಪೂರ್ಣಾಂಕ
mch_ishidden(char_u *name)
{
    ಪೂರ್ಣಾಂಕ f = win32_getattrs(name);

    ಆದರೆ (f == -1)
	ಹಿಂತಿರುಗಿ FALSE;		    // file does not exist at all

    ಹಿಂತಿರುಗಿ (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    ಪೂರ್ಣಾಂಕ
mch_isdir(char_u *name)
{
    ಪೂರ್ಣಾಂಕ f = win32_getattrs(name);

    ಆದರೆ (f == -1)
	ಹಿಂತಿರುಗಿ FALSE;		    // file does not exist at all

    ಹಿಂತಿರುಗಿ (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    ಪೂರ್ಣಾಂಕ
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    attr = Get_File_ಗುಣಲಕ್ಷಣಗಳು_W(p);
    ಆದರೆ (attr == INVALID_FILE_ಗುಣಲಕ್ಷಣಗಳು)
	error = Get_ಕೊನೆಯ_Error();
    vim_free(p);

    ಆದರೆ (attr == INVALID_FILE_ಗುಣಲಕ್ಷಣಗಳು)
	ಹಿಂತಿರುಗಿ error == ERROR_FILE_NOT_ಸಿಕ್ಕಿತು || error == ERROR_ಮಾರ್ಗ_NOT_ಸಿಕ್ಕಿತು;
    ಹಿಂತಿರುಗಿ (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ಪೂರ್ಣಾಂಕ
mch_isrealdir(char_u *name)
{
    ಹಿಂತಿರುಗಿ mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    ಪೂರ್ಣಾಂಕ
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    ಪೂರ್ಣಾಂಕ	    retval;

    p = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    retval = _wmkdir(p);
    vim_free(p);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    ಪೂರ್ಣಾಂಕ
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    ಪೂರ್ಣಾಂಕ	    retval;

    p = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    retval = _wrmdir(p);
    vim_free(p);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    ಪೂರ್ಣಾಂಕ
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    ಹಿಂತಿರುಗಿ win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_ಸಂಖ್ಯೆ_ದ_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    ಪೂರ್ಣಾಂಕ
mch_is_symbolic_link(char_u *name)
{
    HANDLE		h_ಕಂಡುಹಿಡಿ;
    ಪೂರ್ಣಾಂಕ			res = FALSE;
    DWORD		fileFlags = 0, reparse_ಟ್ಯಾಗ್ = 0;
    WCHAR		*wn;
    WIN32_ಕಂಡುಹಿಡಿ_DATAW	ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ_W;

    wn = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (wn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    h_ಕಂಡುಹಿಡಿ = ಕಂಡುಹಿಡಿ_ಮೊದಲ_File_W(wn, &ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ_W);
    vim_free(wn);
    ಆದರೆ (h_ಕಂಡುಹಿಡಿ != INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	fileFlags = ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ_W.dw_File_ಗುಣಲಕ್ಷಣಗಳು;
	reparse_ಟ್ಯಾಗ್ = ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ_W.dwReserved0;
	ಕಂಡುಹಿಡಿ_Close(h_ಕಂಡುಹಿಡಿ);
    }

    ಆದರೆ ((fileFlags & FILE_ATTRIBUTE_REPARSE_ಬಿಂದು)
	    && (reparse_ಟ್ಯಾಗ್ == IO_REPARSE_ಟ್ಯಾಗ್_SYMLINK
		|| reparse_ಟ್ಯಾಗ್ == IO_REPARSE_ಟ್ಯಾಗ್_MOUNT_ಬಿಂದು))
	res = TRUE;

    ಹಿಂತಿರುಗಿ res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    ಪೂರ್ಣಾಂಕ
mch_is_linked(char_u *fname)
{
    ಆದರೆ (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    ಪೂರ್ಣಾಂಕ
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    ಪೂರ್ಣಾಂಕ		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_ಗಮ್ಯ_utf16(fname, ಖಾಲಿ);
    ಆದರೆ (wn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    ಖಾಲಿ,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    ಖಾಲಿ);		// handle to template file
    vim_free(wn);

    ಆದರೆ (hFile == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ FILEINFO_READ_FAIL;

    ಆದರೆ (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    ಇಲ್ಲದಿದ್ದರೆ
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    ಹಿಂತಿರುಗಿ res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
win32_getattrs(char_u *name)
{
    ಪೂರ್ಣಾಂಕ		attr;
    WCHAR	*p;

    p = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ INVALID_FILE_ಗುಣಲಕ್ಷಣಗಳು;

    attr = Get_File_ಗುಣಲಕ್ಷಣಗಳು_W(p);
    vim_free(p);

    ಹಿಂತಿರುಗಿ attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
win32_setattrs(char_u *name, ಪೂರ್ಣಾಂಕ attrs)
{
    ಪೂರ್ಣಾಂಕ	    res;
    WCHAR   *p;

    p = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    res = ಗಣ_File_ಗುಣಲಕ್ಷಣಗಳು_W(p, attrs);
    vim_free(p);

    ಹಿಂತಿರುಗಿ res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
win32_ಗಣ_archive(char_u *name)
{
    ಪೂರ್ಣಾಂಕ attrs = win32_getattrs(name);
    ಆದರೆ (attrs == -1)
	ಹಿಂತಿರುಗಿ -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    ಹಿಂತಿರುಗಿ win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    ಪೂರ್ಣಾಂಕ
mch_writable(char_u *name)
{
    ಪೂರ್ಣಾಂಕ attrs = win32_getattrs(name);

    ಹಿಂತಿರುಗಿ (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    ಪೂರ್ಣಾಂಕ
mch_can_exe(char_u *name, char_u **ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ use_ಮಾರ್ಗ ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ executable_exists((ಅಕ್ಷರ *)name, STRLEN(name), ಮಾರ್ಗ, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ಪೂರ್ಣಾಂಕ
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    ಪೂರ್ಣಾಂಕ		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    ಆದರೆ (STRNCMP(name, "\\\\.\\", 4) == 0)
	ಹಿಂತಿರುಗಿ ಗಂಟು_WRITABLE;

    wn = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (wn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಗಂಟು_ಸಾಮಾನ್ಯ;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    ಖಾಲಿ,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    ಖಾಲಿ);		    // handle to template file
    vim_free(wn);
    ಆದರೆ (hFile == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ ಗಂಟು_ಸಾಮಾನ್ಯ;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    ಆದರೆ (type == FILE_TYPE_CHAR)
	ಹಿಂತಿರುಗಿ ಗಂಟು_WRITABLE;
    ಆದರೆ (type == FILE_TYPE_DISK)
	ಹಿಂತಿರುಗಿ ಗಂಟು_ಸಾಮಾನ್ಯ;
    ಹಿಂತಿರುಗಿ ಗಂಟು_OTHER;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
ರಚನೆ my_acl
{
    PSECURITY_DESCRIPTOR    pSecurityDescriptor;
    PSID		    pSidOwner;
    PSID		    p_Sid_ಗುಂಪು;
    PACL		    pDacl;
    PACL		    pSacl;
};
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_ACL
    ಹಿಂತಿರುಗಿ (vim_acl_T)ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
    ರಚನೆ my_acl   *p = ಖಾಲಿ;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(ರಚನೆ my_acl);
    ಆದರೆ (p != ಖಾಲಿ)
    {
	WCHAR	*wn;

	wn = enc_ಗಮ್ಯ_utf16(fname, ಖಾಲಿ);
	ಆದರೆ (wn == ಖಾಲಿ)
	{
	    vim_free(p);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	// Try to retrieve the entire security descriptor.
	err = GetNamedSecurityInfoW(
		wn,			// Abstract filename
		SE_FILE_OBJECT,		// File Object
		OWNER_SECURITY_INFORMATION |
		ಗುಂಪು_SECURITY_INFORMATION |
		DACL_SECURITY_INFORMATION |
		SACL_SECURITY_INFORMATION,
		&p->pSidOwner,		// Ownership information.
		&p->p_Sid_ಗುಂಪು,		// Group membership.
		&p->pDacl,		// Discretionary information.
		&p->pSacl,		// For auditing purposes.
		&p->pSecurityDescriptor);
	ಆದರೆ (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (ಶೂನ್ಯ)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    ಖಾಲಿ,
		    ಖಾಲಿ,
		    &p->pDacl,
		    ಖಾಲಿ,
		    &p->pSecurityDescriptor);
	}
	ಆದರೆ (p->pSecurityDescriptor == ಖಾಲಿ)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = ಖಾಲಿ;
	}
	vim_free(wn);
    }

    ಹಿಂತಿರುಗಿ (vim_acl_T)p;
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    ಸ್ಥಿರ BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_ಗಾತ್ರ_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_ಎಣಿಕೆ = 0;
    GetAclInformation(acl, &acl_info, ಗಾತ್ರ(acl_info), Acl_ಗಾತ್ರ_Information);
    ಗಾಗಿ (i = 0; i < acl_info.Ace_ಎಣಿಕೆ; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	ಆದರೆ (ace->ಶೀರ್ಷಿಕೆ_2.AceFlags & INHERITED_ACE)
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    ಶೂನ್ಯ
mch_ಗಣ_acl(char_u *fname, vim_acl_T acl)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
    ರಚನೆ my_acl   *p = (ರಚನೆ my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    wn = enc_ಗಮ್ಯ_utf16(fname, ಖಾಲಿ);
    ಆದರೆ (wn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Set security flags
    ಆದರೆ (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    ಆದರೆ (p->p_Sid_ಗುಂಪು)
	sec_info |= ಗುಂಪು_SECURITY_INFORMATION;
    ಆದರೆ (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	ಆದರೆ (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    ಆದರೆ (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (ಶೂನ್ಯ)ಗಣ_Named_Security_Info_W(
	    wn,			// Abstract filename
	    SE_FILE_OBJECT,	// File Object
	    sec_info,
	    p->pSidOwner,	// Ownership information.
	    p->p_Sid_ಗುಂಪು,	// Group membership.
	    p->pDacl,		// Discretionary information.
	    p->pSacl		// For auditing purposes.
	    );
    vim_free(wn);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
mch_free_acl(vim_acl_T acl)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
    ರಚನೆ my_acl   *p = (ರಚನೆ my_acl *)acl;

    ಆದರೆ (p != ಖಾಲಿ)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    ಸ್ಥಿರ BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    ಒಳಹರಿವು_ದಾಖಲೆ ir;
    DWORD ಹೊರಹೋಗು;

    ಆಯ್ಕೆ (dwCtrlType)
    {
    ಸ್ಥಿತಿ CTRL_C_EVENT:
	ಆದರೆ (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	ಹಿಂತಿರುಗಿ TRUE;

    ಸ್ಥಿತಿ CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = ಕೀಲಿ_EVENT;
	ir.Event.ಕೀಲಿ_Event_2.b_ಕೀಲಿ_ಕೆಳಕ್ಕೆ = TRUE;
	ir.Event.ಕೀಲಿ_Event_2.w_Repeat_ಎಣಿಕೆ = 1;
	ir.Event.ಕೀಲಿ_Event_2.w_Virtual_ಕೀಲಿ_ಸಂಕೇತ = VK_CANCEL;
	ir.Event.ಕೀಲಿ_Event_2.w_Virtual_ಪರಿಶೀಲನೆ_ಸಂಕೇತ = 0;
	ir.Event.ಕೀಲಿ_Event_2.dw_Control_ಕೀಲಿ_State = 0;
	ir.Event.ಕೀಲಿ_Event_2.uChar.UnicodeChar = 0;
	Write_Console_ಒಳಹರಿವು(g_h_Con_ಒಳ, &ir, 1, &ಹೊರಹೋಗು);
	ಹಿಂತಿರುಗಿ TRUE;

    // fatal events: shut down gracefully
    ಸ್ಥಿತಿ CTRL_CLOSE_EVENT:
    ಸ್ಥಿತಿ CTRL_LOGOFF_EVENT:
    ಸ್ಥಿತಿ CTRL_SHUTDOWN_EVENT:
	windgoto((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, cmdline_ಲಂಬಸಾಲು_off);
	g_fForceExit = TRUE;

	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	ಹಿಂತಿರುಗಿ TRUE;		// not reached

    ಪೂರ್ವನಿಯೋಜಿತ:
	ಹಿಂತಿರುಗಿ FALSE;
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
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;
# endif
    Get_Console_ಬಹುಲಕ(g_h_Con_ಒಳ, &cmodein);
    Get_Console_ಬಹುಲಕ(g_h_Con_ಹೊರಹೋಗು, &cmodeout);
    ಆದರೆ (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_ಸಾಲು_ಒಳಹರಿವು | ENABLE_PROCESSED_ಒಳಹರಿವು |
		     ENABLE_ECHO_ಒಳಹರಿವು);
	ಆದರೆ (g_f_Mouse_ಸಕ್ರಿಯ)
	{
	    cmodein |= ENABLE_MOUSE_ಒಳಹರಿವು;
	    cmodein &= ~ENABLE_ಶೀಘ್ರ_EDIT_ಬಹುಲಕ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cmodein |= g_cmodein & ENABLE_ಶೀಘ್ರ_EDIT_ಬಹುಲಕ;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# ಇಲ್ಲದಿದ್ದರೆ
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ // cooked
    {
	cmodein |= (ENABLE_ಸಾಲು_ಒಳಹರಿವು | ENABLE_PROCESSED_ಒಳಹರಿವು |
		    ENABLE_ECHO_ಒಳಹರಿವು);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    ಗಣ_Console_ಬಹುಲಕ(g_h_Con_ಒಳ, cmodein | ENABLE_EXTENDED_FLAGS);
    ಗಣ_Console_ಬಹುಲಕ(g_h_Con_ಹೊರಹೋಗು, cmodeout);
    ಗಣ_Console_Ctrl_Handler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    ಆದರೆ (fdDump)
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
    ಪೂರ್ಣಾಂಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    CONSOLE_SCREEN_ಬಫರ್_INFO csbi;

# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ OK;
# endif
    ಆದರೆ (!g_f_Termcap_ಬಹುಲಕ && g_cbTermcap.IsValid)
    {
	/*
	 * For some reason, we are trying to get the screen dimensions
	 * even though we are not in termcap mode.  The 'Rows' and 'Columns'
	 * variables are really intended to mean the size of Vim screen
	 * while in termcap mode.
	 */
	ಅಡ್ಡಸಾಲುಗಳು = g_cbTermcap.Info.dw_ಗಾತ್ರ.Y;
	Columns = g_cbTermcap.Info.dw_ಗಾತ್ರ.X;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Get_Console_Screen_ಬಫರ್_Info(g_h_Con_ಹೊರಹೋಗು, &csbi))
    {
	ಅಡ್ಡಸಾಲುಗಳು = csbi.sr_ಕಿಟಕಿ.Bottom - csbi.sr_ಕಿಟಕಿ.ತುದಿ_2 + 1;
	Columns = csbi.sr_ಕಿಟಕಿ.ಬಲ_4 - csbi.sr_ಕಿಟಕಿ.ಎಡ_4 + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಡ್ಡಸಾಲುಗಳು = 25;
	Columns = 80;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    ಆದರೆ (use_alternate_screen_ಬಫರ್)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!ಗಣ_Console_Screen_ಬಫರ್_ಗಾತ್ರ(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	ಆದರೆ (fdDump)
	{
	    fprintf(fdDump, "SetConsoleScreenBufferSize failed: %lx\n",
		    Get_ಕೊನೆಯ_Error());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Resize console window size to 'srWindowRect'
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Resize_ಕಿಟಕಿ(
    HANDLE     hConsole,
    SMALL_RECT sr_ಕಿಟಕಿ_Rect)
{
    ಆದರೆ (!ಗಣ_Console_ಕಿಟಕಿ_Info(hConsole, TRUE, &sr_ಕಿಟಕಿ_Rect))
    {
# ifdef MCH_WRITE_DUMP
	ಆದರೆ (fdDump)
	{
	    fprintf(fdDump, "SetConsoleWindowInfo failed: %lx\n",
		    Get_ಕೊನೆಯ_Error());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Set a console window to `xSize' * `ySize'
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Resize_Con_Buf_And_ಕಿಟಕಿ(
    HANDLE  hConsole,
    ಪೂರ್ಣಾಂಕ	    x_ಗಾತ್ರ,
    ಪೂರ್ಣಾಂಕ	    y_ಗಾತ್ರ_2)
{
    CONSOLE_SCREEN_ಬಫರ್_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    sr_ಕಿಟಕಿ_Rect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    ಆದರೆ (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", x_ಗಾತ್ರ, y_ಗಾತ್ರ_2);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = Get_ಅತಿದೊಡ್ಡ_Console_ಕಿಟಕಿ_ಗಾತ್ರ(hConsole);

    // define the new console window size and scroll position
    sr_ಕಿಟಕಿ_Rect.ಎಡ_4 = sr_ಕಿಟಕಿ_Rect.ತುದಿ_2 = (SHORT) 0;
    sr_ಕಿಟಕಿ_Rect.ಬಲ_4 =  (SHORT) (min(x_ಗಾತ್ರ, coordScreen.X) - 1);
    sr_ಕಿಟಕಿ_Rect.Bottom = (SHORT) (min(y_ಗಾತ್ರ_2, coordScreen.Y) - 1);

    ಆದರೆ (Get_Console_Screen_ಬಫರ್_Info(g_h_Con_ಹೊರಹೋಗು, &csbi))
    {
	ಪೂರ್ಣಾಂಕ sx, sy;

	sx = csbi.sr_ಕಿಟಕಿ.ಬಲ_4 - csbi.sr_ಕಿಟಕಿ.ಎಡ_4 + 1;
	sy = csbi.sr_ಕಿಟಕಿ.Bottom - csbi.sr_ಕಿಟಕಿ.ತುದಿ_2 + 1;
	ಆದರೆ (sy < y_ಗಾತ್ರ_2 || sx < x_ಗಾತ್ರ)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    ಆದರೆ (sy < y_ಗಾತ್ರ_2)
		coordScreen.Y = y_ಗಾತ್ರ_2;
	    ಇಲ್ಲದಿದ್ದರೆ
		coordScreen.Y = sy;
	    ಆದರೆ (sx < x_ಗಾತ್ರ)
		coordScreen.X = x_ಗಾತ್ರ;
	    ಇಲ್ಲದಿದ್ದರೆ
		coordScreen.X = sx;
	    ಗಣ_Console_Screen_ಬಫರ್_ಗಾತ್ರ(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = x_ಗಾತ್ರ;
    coordScreen.Y = y_ಗಾತ್ರ_2;

    // In the new console call API, only the first time in reverse order
    ಆದರೆ (!vtp_working || resized)
    {
	Resize_ಕಿಟಕಿ(hConsole, sr_ಕಿಟಕಿ_Rect);
	ResizeConBuf(hConsole, coordScreen);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Workaround for a Windows 10 bug
	cursor.X = sr_ಕಿಟಕಿ_Rect.ಎಡ_4;
	cursor.Y = sr_ಕಿಟಕಿ_Rect.ತುದಿ_2;
	ಗಣ_Console_Cursor_Position(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	Resize_ಕಿಟಕಿ(hConsole, sr_ಕಿಟಕಿ_Rect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    ಶೂನ್ಯ
mch_ಗಣ_shellsize(ಶೂನ್ಯ)
{
    COORD coordScreen;

# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;
# endif
    // Don't change window size while still starting up
    ಆದರೆ (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಪದ_console)
    {
	coordScreen = Get_ಅತಿದೊಡ್ಡ_Console_ಕಿಟಕಿ_ಗಾತ್ರ(g_h_Con_ಹೊರಹೋಗು);

	// Clamp Rows and Columns to reasonable values
	ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು > coordScreen.Y)
	    ಅಡ್ಡಸಾಲುಗಳು = coordScreen.Y;
	ಆದರೆ (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	Resize_Con_Buf_And_ಕಿಟಕಿ(g_h_Con_ಹೊರಹೋಗು, Columns, ಅಡ್ಡಸಾಲುಗಳು);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    ಶೂನ್ಯ
mch_new_shellsize(ಶೂನ್ಯ)
{
# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;
# endif
    ಗಣ_scroll_region(0, 0, Columns - 1, ಅಡ್ಡಸಾಲುಗಳು - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    ಶೂನ್ಯ
mch_ಗಣ_winsize_now(ಶೂನ್ಯ)
{
    ಆದರೆ (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_ಗಣ_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# ಆದರೆ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    ಶೂನ್ಯ
mch_calc_cell_ಗಾತ್ರ(ರಚನೆ cellsize *cs_ಹೊರಹೋಗು)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_ಹೊರಹೋಗು->cs_xpixel = -1;
    cs_ಹೊರಹೋಗು->cs_ypixel = -1;

    ಆದರೆ (g_h_Con_ಹೊರಹೋಗು == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ;

    cfi.cb_ಗಾತ್ರ = ಗಾತ್ರ(cfi);
    ಆದರೆ (GetCurrentConsoleFontEx(g_h_Con_ಹೊರಹೋಗು, FALSE, &cfi)
	    && cfi.dw_Font_ಗಾತ್ರ.X > 0 && cfi.dw_Font_ಗಾತ್ರ.Y > 0)
    {
	cs_ಹೊರಹೋಗು->cs_xpixel = cfi.dw_Font_ಗಾತ್ರ.X;
	cs_ಹೊರಹೋಗು->cs_ypixel = cfi.dw_Font_ಗಾತ್ರ.Y;
	ಹಿಂತಿರುಗಿ;
    }

    Get_Console_ಬಹುಲಕ(g_h_Con_ಒಳ, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_ಸಾಲು_ಒಳಹರಿವು | ENABLE_ECHO_ಒಳಹರಿವು
	    | ENABLE_PROCESSED_ಒಳಹರಿವು);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_ಒಳಹರಿವು;

    ಆದರೆ (ಗಣ_Console_ಬಹುಲಕ(g_h_Con_ಒಳ, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	ಗಣ_Console_ಬಹುಲಕ(g_h_Con_ಒಳ, cmodeold);
    }
}
# endif

#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ BOOL
vim_create_process(
    ಅಕ್ಷರ		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    ಅಕ್ಷರ		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = ಖಾಲಿ;

    wcmd = enc_ಗಮ್ಯ_utf16((char_u *)cmd, ಖಾಲಿ);
    ಆದರೆ (wcmd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (cwd != ಖಾಲಿ)
    {
	wcwd = enc_ಗಮ್ಯ_utf16((char_u *)cwd, ಖಾಲಿ);
	ಆದರೆ (wcwd == ಖಾಲಿ)
	    ಹೋಗು theend;
    }

    ret = CreateProcessW(
	    ಖಾಲಿ,		// Executable name
	    wcmd,		// Command to execute
	    ಖಾಲಿ,		// Process security attributes
	    ಖಾಲಿ,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    ಹಿಂತಿರುಗಿ ret;
}


    ಸ್ಥಿರ HINSTANCE
vim_shell_execute(
    ಅಕ್ಷರ *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_ಗಮ್ಯ_utf16((char_u *)cmd, ಖಾಲಿ);
    ಆದರೆ (wcmd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (HINSTANCE) 0;

    ret = ShellExecuteW(ಖಾಲಿ, ಖಾಲಿ, wcmd, ಖಾಲಿ, ಖಾಲಿ, n_show_cmd);
    vim_free(wcmd);
    ಹಿಂತಿರುಗಿ ret;
}


#ಆದರೆ defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mch_system_classic(ಅಕ್ಷರ *cmd, ಪೂರ್ಣಾಂಕ options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = ಗಾತ್ರ(si);
    si.lpReserved = ಖಾಲಿ;
    si.lpDesktop = ಖಾಲಿ;
    si.lpTitle = ಖಾಲಿ;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    ಆದರೆ (options & SHELL_DOOUT)
	si.w_Show_ಕಿಟಕಿ = SW_SHOWMINNOACTIVE;
    ಇಲ್ಲದಿದ್ದರೆ
	si.w_Show_ಕಿಟಕಿ = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = ಖಾಲಿ;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_ಬಹುಲಕ |	CREATE_NEW_CONSOLE,
	    &si, &pi, ಖಾಲಿ, ಖಾಲಿ);

    // Wait for the command to terminate before continuing
    {
	ಪೂರ್ಣಾಂಕ	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	ಗಾಗಿ (;;)
	{
	    ಸಂದೇಶ_3	ಸಂದೇಶ;

	    ಆದರೆ (Peek_ಸಂದೇಶ_W(&ಸಂದೇಶ, (HWND)ಖಾಲಿ, 0, 0, PM_REMOVE))
	    {
		Translate_ಸಂದೇಶ(&ಸಂದೇಶ);
		Dispatch_ಸಂದೇಶ_W(&ಸಂದೇಶ);
		delay = 1;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		ನಿಲ್ಲಿಸು;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    ಆದರೆ (delay < 50)
		delay += 10;
	}

	// Get the command exit code
	Get_Exit_ಸಂಕೇತ_Process(pi.hProcess, &ret);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // Try to get input focus back.  Doesn't always work though.
    Post_ಸಂದೇಶ(hwnd, WM_SETFOCUS, 0, 0);
    // To increase the chances that WM_SETFOCUS will work, run the message loop
    // here.  In addition, it prevents problems caused by delayed WM_SETFOCUS
    // processing.
    gui_mch_update();

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_h_ಮಗು_Std_ಒಳ_Wr = param;
    linenr_T	    lnum = curbuf->b_op_ಆರಂಭ.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    ಪೂರ್ಣಾಂಕ		    written = 0;

    ಗಾಗಿ (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	ಆದರೆ (l == 0)
	    len = 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_h_ಮಗು_Std_ಒಳ_Wr, "", 1, &len, ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_h_ಮಗು_Std_ಒಳ_Wr, (ಅಕ್ಷರ *)lp + written,
		      s == ಖಾಲಿ ? l : (DWORD)(s - (lp + written)),
		      &len, ಖಾಲಿ);
	}
	ಆದರೆ (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    ಆದರೆ (lnum != curbuf->b_op_ಕೊನೆ.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_h_ಮಗು_Std_ಒಳ_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, ಖಾಲಿ);
	    }

	    ++lnum;
	    ಆದರೆ (lnum > curbuf->b_op_ಕೊನೆ.lnum)
		ನಿಲ್ಲಿಸು;

	    lp = ml_get(lnum);
	    written = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_h_ಮಗು_Std_ಒಳ_Wr);
    ಹಿಂತಿರುಗಿ 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dump_pipe(ಪೂರ್ಣಾಂಕ	    options,
	  HANDLE    g_h_ಮಗು_Std_ಹೊರಹೋಗು_Rd,
	  garray_T  *ga,
	  char_u    ಬಫರ್[],
	  DWORD	    *ಬಫರ್_off)
{
    DWORD	ಲಭ್ಯ_ಬೈಟುಗಳು = 0;
    DWORD	i;
    ಪೂರ್ಣಾಂಕ		ret;
    DWORD	len;
    DWORD	ಗಮ್ಯ_Read;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_h_ಮಗು_Std_ಹೊರಹೋಗು_Rd, // pipe to query
			ಖಾಲಿ,		    // optional buffer
			0,		    // buffer size
			ಖಾಲಿ,		    // number of read bytes
			&ಲಭ್ಯ_ಬೈಟುಗಳು,    // available bytes total
			ಖಾಲಿ);		    // byteLeft

    // We got real data in the pipe, read it
    ಆಗ (ret != 0 && ಲಭ್ಯ_ಬೈಟುಗಳು > 0)
    {
	ಗಮ್ಯ_Read = (DWORD)(BUFLEN - *ಬಫರ್_off);
	ಗಮ್ಯ_Read = ಲಭ್ಯ_ಬೈಟುಗಳು < ಗಮ್ಯ_Read ? ಲಭ್ಯ_ಬೈಟುಗಳು : ಗಮ್ಯ_Read;
	ReadFile(g_h_ಮಗು_Std_ಹೊರಹೋಗು_Rd, ಬಫರ್ + *ಬಫರ್_off, ಗಮ್ಯ_Read , &len, ಖಾಲಿ);

	// If we haven't read anything, there is a problem
	ಆದರೆ (len == 0)
	    ನಿಲ್ಲಿಸು;

	ಲಭ್ಯ_ಬೈಟುಗಳು -= len;

	ಆದರೆ (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    ಗಾಗಿ (i = 0; i < len; ++i)
	    {
		ಆದರೆ (ಬಫರ್[i] == NL)
		    append_ga_ಸಾಲು(ga);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಫರ್[i] == NUL)
		    ga_append(ga, NL);
		ಇಲ್ಲದಿದ್ದರೆ
		    ga_append(ga, ಬಫರ್[i]);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	{
	    ಪೂರ್ಣಾಂಕ		l;
	    ಪೂರ್ಣಾಂಕ		c;
	    char_u	*p;

	    len += *ಬಫರ್_off;
	    ಬಫರ್[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    ಗಾಗಿ (p = ಬಫರ್; p < ಬಫರ್ + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		ಆದರೆ (l == 0)
		    l = 1;  // NUL byte?
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (MB_BYTE2LEN(*p) != l)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (p == ಬಫರ್)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		ಆದರೆ (len >= 12)
		    ++p;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    *ಬಫರ್_off = len;
		    ಹಿಂತಿರುಗಿ;
		}
	    }
	    c = *p;
	    *p = NUL;
	    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)ಬಫರ್);
	    ಆದರೆ (p < ಬಫರ್ + len)
	    {
		*p = c;
		*ಬಫರ್_off = (DWORD)((ಬಫರ್ + len) - p);
		mch_memmove(ಬಫರ್, p, *ಬಫರ್_off);
		ಹಿಂತಿರುಗಿ;
	    }
	    *ಬಫರ್_off = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಬಫರ್[len] = NUL;
	    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)ಬಫರ್);
	}

	windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);
	cursor_on();
	ಹೊರಹೋಗು_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mch_system_piped(ಅಕ್ಷರ *cmd, ಪೂರ್ಣಾಂಕ options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_h_ಮಗು_Std_ಒಳ_Rd = ಖಾಲಿ;
    HANDLE g_h_ಮಗು_Std_ಒಳ_Wr = ಖಾಲಿ;
    HANDLE g_h_ಮಗು_Std_ಹೊರಹೋಗು_Rd = ಖಾಲಿ;
    HANDLE g_h_ಮಗು_Std_ಹೊರಹೋಗು_Wr = ಖಾಲಿ;

    char_u	ಬಫರ್[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    ಪೂರ್ಣಾಂಕ		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    ಪೂರ್ಣಾಂಕ		noread_cnt = 0;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		delay = 1;
    DWORD	ಬಫರ್_off = 0;	// valid bytes in buffer[]
    ಅಕ್ಷರ	*p = ಖಾಲಿ;

    SECURITY_ಗುಣಲಕ್ಷಣಗಳು saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.n_ಉದ್ದಳತೆ = ಗಾತ್ರ(SECURITY_ಗುಣಲಕ್ಷಣಗಳು);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ಖಾಲಿ;

    ಆದರೆ ( ! CreatePipe(&g_h_ಮಗು_Std_ಹೊರಹೋಗು_Rd, &g_h_ಮಗು_Std_ಹೊರಹೋಗು_Wr, &saAttr, 0)
	// Ensure the read handle to the pipe for STDOUT is not inherited.
       || ! ಗಣ_Handle_Information(g_h_ಮಗು_Std_ಹೊರಹೋಗು_Rd, HANDLE_FLAG_INHERIT, 0)
	// Create a pipe for the child process's STDIN.
       || ! CreatePipe(&g_h_ಮಗು_Std_ಒಳ_Rd, &g_h_ಮಗು_Std_ಒಳ_Wr, &saAttr, 0)
	// Ensure the write handle to the pipe for STDIN is not inherited.
       || ! ಗಣ_Handle_Information(g_h_ಮಗು_Std_ಒಳ_Wr, HANDLE_FLAG_INHERIT, 0) )
    {
	CloseHandle(g_h_ಮಗು_Std_ಒಳ_Rd);
	CloseHandle(g_h_ಮಗು_Std_ಒಳ_Wr);
	CloseHandle(g_h_ಮಗು_Std_ಹೊರಹೋಗು_Rd);
	CloseHandle(g_h_ಮಗು_Std_ಹೊರಹೋಗು_Wr);
	ಸಂದೇಶ_puts(_("\nCannot create pipes\n"));
    }

    si.cb = ಗಾತ್ರ(si);
    si.lpReserved = ಖಾಲಿ;
    si.lpDesktop = ಖಾಲಿ;
    si.lpTitle = ಖಾಲಿ;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_h_ಮಗು_Std_ಹೊರಹೋಗು_Wr;
    si.hStdOutput = g_h_ಮಗು_Std_ಹೊರಹೋಗು_Wr;
    si.h_Std_ಒಳಹರಿವು = g_h_ಮಗು_Std_ಒಳ_Rd;
    si.w_Show_ಕಿಟಕಿ = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = ಖಾಲಿ;

    ಆದರೆ (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    ಆದರೆ (cmd != ಖಾಲಿ)
    {
	p = (ಅಕ್ಷರ *)vim_strsave((char_u *)cmd);
	ಆದರೆ (p != ಖಾಲಿ)
	    unescape_shellxquote((char_u *)p, p_sxe);
	ಇಲ್ಲದಿದ್ದರೆ
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_ಬಹುಲಕ,
						&si, &pi, ಖಾಲಿ, ಖಾಲಿ);

    ಆದರೆ (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_h_ಮಗು_Std_ಒಳ_Rd);
    CloseHandle(g_h_ಮಗು_Std_ಹೊರಹೋಗು_Wr);

    ಆದರೆ (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(ಖಾಲಿ,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_h_ಮಗು_Std_ಒಳ_Wr,  // parameter
			0,		 // creation flag, start immediately
			ಖಾಲಿ);		    // we don't care about thread id
	CloseHandle(thread);
	g_h_ಮಗು_Std_ಒಳ_Wr = ಖಾಲಿ;
    }

    // Keep updating the window while waiting for the shell to finish.
    ಗಾಗಿ (;;)
    {
	ಸಂದೇಶ_3	ಸಂದೇಶ;

	ಆದರೆ (Peek_ಸಂದೇಶ_W(&ಸಂದೇಶ, (HWND)ಖಾಲಿ, 0, 0, PM_REMOVE))
	{
	    Translate_ಸಂದೇಶ(&ಸಂದೇಶ);
	    Dispatch_ಸಂದೇಶ_W(&ಸಂದೇಶ);
	}

	// write pipe information in the window
	ಆದರೆ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.ಒಳ_use
# endif
	    )
	{
	    len = 0;
	    ಆದರೆ (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.ಒಳ_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		ಆದರೆ (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		ಆದರೆ (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    ಆದರೆ (len == 1 && cmd != ಖಾಲಿ)
		    {
			ಆದರೆ (ta_buf[ta_len] == Ctrl_C)
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
		    ಆದರೆ ((len == 1 || len == 4 ) && cmd != ಖಾಲಿ)
		    {
			ಆದರೆ (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_h_ಮಗು_Std_ಒಳ_Wr);
			    g_h_ಮಗು_Std_ಒಳ_Wr = ಖಾಲಿ;
			    len = 0;
			}
		    }

		    len = ಪದ_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    ಗಾಗಿ (i = ta_len; i < ta_len + len; ++i)
		    {
			ಆದರೆ (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    ಸಂದೇಶ_putchar(ta_buf[i]);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
			{
			    ಪೂರ್ಣಾಂಕ l = (*mb_ptr2len)(ta_buf + i);

			    ಸಂದೇಶ_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    ಸಂದೇಶ_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);
		    ಹೊರಹೋಗು_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    ಆದರೆ (options & SHELL_WRITE)
			ta_len = 0;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (g_h_ಮಗು_Std_ಒಳ_Wr != ಖಾಲಿ)
		    {
			WriteFile(g_h_ಮಗು_Std_ಒಳ_Wr, (ಅಕ್ಷರ*)ta_buf,
				    1, &len, ಖಾಲಿ);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			ಆದರೆ (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	ಆದರೆ (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	ಆದರೆ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_h_ಮಗು_Std_ಹೊರಹೋಗು_Rd, &ga, ಬಫರ್, &ಬಫರ್_off);
	    ನಿಲ್ಲಿಸು;
	}

	++noread_cnt;
	dump_pipe(options, g_h_ಮಗು_Std_ಹೊರಹೋಗು_Rd, &ga, ಬಫರ್, &ಬಫರ್_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	ಆದರೆ (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_h_ಮಗು_Std_ಹೊರಹೋಗು_Rd);
    ಆದರೆ (g_h_ಮಗು_Std_ಒಳ_Wr != ಖಾಲಿ)
	CloseHandle(g_h_ಮಗು_Std_ಒಳ_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    Get_Exit_ಸಂಕೇತ_Process(pi.hProcess, &ret);

    ಆದರೆ (options & SHELL_READ)
    {
	ಆದರೆ (ga.ga_len > 0)
	{
	    append_ga_ಸಾಲು(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mch_system_g(ಅಕ್ಷರ *cmd, ಪೂರ್ಣಾಂಕ options)
{
    // if we can pipe and the shelltemp option is off
    ಆದರೆ (!p_stmp)
	ಹಿಂತಿರುಗಿ mch_system_piped(cmd, options);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ mch_system_classic(cmd, options);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mch_system_c(ಅಕ್ಷರ *cmd, ಪೂರ್ಣಾಂಕ options ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    ಆದರೆ (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
	vim_snprintf((ಅಕ್ಷರ *)buf, len, "(%s)", cmd);
	wcmd = enc_ಗಮ್ಯ_utf16(buf, ಖಾಲಿ);
	ಬಿಡುಗಡೆ(buf);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	wcmd = enc_ಗಮ್ಯ_utf16((char_u *)cmd, ಖಾಲಿ);

    ಆದರೆ (wcmd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    ಹಿಂತಿರುಗಿ ret;
}

#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mch_system(ಅಕ್ಷರ *cmd, ಪೂರ್ಣಾಂಕ options)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VIMDLL
    ಆದರೆ (gui.ಒಳ_use || gui.starting)
	ಹಿಂತಿರುಗಿ mch_system_g(cmd, options);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ mch_system_c(cmd, options);
#ಅಥವಾಆದರೆ defined(FEAT_GUI_MSWIN)
    ಹಿಂತಿರುಗಿ mch_system_g(cmd, options);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ mch_system_c(cmd, options);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mch_call_shell_terminal(
    char_u	*cmd,
    ಪೂರ್ಣಾಂಕ		options ಬಳಸದ)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = ಖಾಲಿ;
    typval_T	argvar[2];
    long_u	cmdlen;
    ಪೂರ್ಣಾಂಕ		retval = -1;
    buf_T	*buf;
    ಕೆಲಸ_T	*ಕೆಲಸ;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    ಆದರೆ (cmd == ಖಾಲಿ)
	cmdlen = STRLEN(p_sh) + 1;
    ಇಲ್ಲದಿದ್ದರೆ
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    ಆದರೆ (newcmd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 255;
    ಆದರೆ (cmd == ಖಾಲಿ)
    {
	STRCPY(newcmd, p_sh);
	ch_log(ಖಾಲಿ, "starting terminal to run a shell");
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_snprintf((ಅಕ್ಷರ *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(ಖಾಲಿ, "starting terminal for system command '%s'", cmd);
    }

    init_ಕೆಲಸ_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = ಪದ_ಆರಂಭ(argvar, ಖಾಲಿ, &opt, ಪದ_ಆರಂಭ_SYSTEM);
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	vim_free(newcmd);
	ಹಿಂತಿರುಗಿ 255;
    }

    ಕೆಲಸ = ಪದ_getjob(buf->b_ಪದ);
    ++ಕೆಲಸ->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    ಆದರೆ (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	ಆಗ (ಪದ_use_loop())
	{
	    ಆದರೆ (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_ಸಕ್ರಿಯ)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		ಆದರೆ (terminal_loop(TRUE) == OK)
		    ಸಾಮಾನ್ಯ_cmd(&oa, TRUE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಾಮಾನ್ಯ_cmd(&oa, TRUE);
	}
	retval = ಕೆಲಸ->jv_exitval;
	ch_log(ಖಾಲಿ, "system command finished");

	ಕೆಲಸ_unref(ಕೆಲಸ);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_ಬಫರ್(DOBUF_WIPE, DOBUF_ಮೊದಲ, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    ಹಿಂತಿರುಗಿ retval;
}
#ಆದರೆ_ಅಂತ್ಯ
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_env_var(char_u *name, char_u *old_ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ must_free)
{
    ಆದರೆ (old_ಮೌಲ್ಯ != ಖಾಲಿ)
    {
	vim_setenv(name, old_ಮೌಲ್ಯ);
	ಆದರೆ (must_free)
	    vim_free(old_ಮೌಲ್ಯ);
	ಹಿಂತಿರುಗಿ;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    ಪೂರ್ಣಾಂಕ
mch_call_shell(
    char_u  *cmd,
    ಪೂರ್ಣಾಂಕ	    options)	// SHELL_*, see vim.h
{
    ಪೂರ್ಣಾಂಕ		x = 0;
    ಪೂರ್ಣಾಂಕ		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    ಪೂರ್ಣಾಂಕ		must_free;
    char_u	*oldval;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ch_log(ಖಾಲಿ, "executing shell command: %s", cmd);
#ಆದರೆ_ಅಂತ್ಯ
    // Change the title to reflect that we are in a subshell.
    ಆದರೆ (GetConsoleTitleW(szShellTitle, ARRAY_ಉದ್ದಳತೆ(szShellTitle) - 4) > 0)
    {
	ಆದರೆ (cmd == ಖಾಲಿ)
	    wcscat(szShellTitle, L" :sh");
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    WCHAR *wn = enc_ಗಮ್ಯ_utf16((char_u *)cmd, ಖಾಲಿ);

	    ಆದರೆ (wn != ಖಾಲಿ)
	    {
		wcscat(szShellTitle, L" - !");
		ಆದರೆ ((wcslen(szShellTitle) + wcslen(wn) <
			    ARRAY_ಉದ್ದಳತೆ(szShellTitle)))
		    wcscat(szShellTitle, wn);
		ಗಣ_Console_Title_W(szShellTitle);
		vim_free(wn);
	    }
	}
    }
    // do not execute anything from the current directory by setting the
    // environment variable $NoDefaultCurrentDirectoryInExePath
    oldval = vim_getenv((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    &must_free);
    vim_setenv((char_u *)"NoDefaultCurrentDirectoryInExePath", (char_u *)"1");

    ಹೊರಹೋಗು_flush();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MCH_WRITE_DUMP
    ಆದರೆ (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    ಆದರೆ (
# ifdef VIMDLL
	    gui.ಒಳ_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != ಖಾಲಿ
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	ಆದರೆ (cmdbase != ಖಾಲಿ)
	    // Skip a leading quote and (.
	    ಆಗ (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	ಆದರೆ (cmdbase == ಖಾಲಿ || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    ಹಿಂತಿರುಗಿ x;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

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

    ಆದರೆ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    ಆದರೆ (cmd == ಖಾಲಿ)
    {
	x = mch_system((ಅಕ್ಷರ *)p_sh, options);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = ಖಾಲಿ;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	ಆದರೆ (*cmdbase == '"' )
	    ++cmdbase;
	ಆದರೆ (*cmdbase == '(')
	    ++cmdbase;

	ಆದರೆ ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    Zero_ಸ್ಮರಣೆ(&si, ಗಾತ್ರ(si));
	    si.cb = ಗಾತ್ರ(si);
	    si.lpReserved = ಖಾಲಿ;
	    si.lpDesktop = ಖಾಲಿ;
	    si.lpTitle = ಖಾಲಿ;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = ಖಾಲಿ;

	    cmdbase = skipwhite(cmdbase + 5);
	    ಆದರೆ ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.w_Show_ಕಿಟಕಿ = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_ಕಿಟಕಿ;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_ಒಳಹರಿವು = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    ಖಾಲಿ,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ,		// File att.
		    ಖಾಲಿ);				// Temp file
		si.hStdOutput = si.h_Std_ಒಳಹರಿವು;
		si.hStdError = si.h_Std_ಒಳಹರಿವು;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    ಆದರೆ (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		ಆದರೆ (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		ಆದರೆ (p > cmdbase && p[-1] == ')'
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
	    ಆದರೆ (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		ಆದರೆ (cmd_shell == ಖಾಲಿ || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		ಆದರೆ (subcmd != ಖಾಲಿ)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    ಆದರೆ (newcmd != ಖಾಲಿ)
			vim_snprintf((ಅಕ್ಷರ *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    ಇಲ್ಲದಿದ್ದರೆ
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    ಆದರೆ (vim_create_process((ಅಕ್ಷರ *)newcmd, FALSE, flags,
			&si, &pi, ಖಾಲಿ, ಖಾಲಿ))
		x = 0;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_shell_execute((ಅಕ್ಷರ *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		x = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
# ifdef VIMDLL
		ಆದರೆ (gui.ಒಳ_use)
# endif
		    emsg(_(e_command_not_ಸಿಕ್ಕಿತು));
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    ಆದರೆ (newcmd != cmdbase)
		vim_free(newcmd);

	    ಆದರೆ (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_ಒಳಹರಿವು != ಖಾಲಿ)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_ಒಳಹರಿವು);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cmdlen =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
		((gui.ಒಳ_use || gui.starting) ?
		    (!s_dont_use_vimrun && p_stmp ?
			vimrun_ಮಾರ್ಗ.ಉದ್ದಳತೆ : STRLEN(p_sh) + STRLEN(p_shcf))
		    : 0) +
#ಆದರೆ_ಅಂತ್ಯ
		STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;

	    newcmd = alloc(cmdlen);
	    ಆದರೆ (newcmd != ಖಾಲಿ)
	    {
#ಆದರೆ defined(FEAT_GUI_MSWIN)
		ಆದರೆ (
# ifdef VIMDLL
		    (gui.ಒಳ_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    ಅಕ್ಷರ *ಸಂದೇಶ = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    ಅಕ್ಷರ *title = _("Vim Warning");
		    WCHAR *wmsg = enc_ಗಮ್ಯ_utf16((char_u *)ಸಂದೇಶ, ಖಾಲಿ);
		    WCHAR *wtitle = enc_ಗಮ್ಯ_utf16((char_u *)title, ಖಾಲಿ);

		    ಆದರೆ (wmsg != ಖಾಲಿ && wtitle != ಖಾಲಿ)
			ಸಂದೇಶ_Box_W(ಖಾಲಿ, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		ಆದರೆ (
# ifdef VIMDLL
		    (gui.ಒಳ_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((ಅಕ್ಷರ *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_ಮಾರ್ಗ.string,
			    (ಸಂದೇಶ_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (
# ifdef VIMDLL
			(gui.ಒಳ_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((ಅಕ್ಷರ *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    vim_snprintf((ಅಕ್ಷರ *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((ಅಕ್ಷರ *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    ಆದರೆ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    ಆದರೆ (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#ಆದರೆ defined(FEAT_GUI_MSWIN)
	    && ((gui.ಒಳ_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#ಆದರೆ_ಅಂತ್ಯ
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

    ಹಿಂತಿರುಗಿ x;
}

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
    ಸ್ಥಿರ HANDLE
ಕೆಲಸ_io_file_open(
	char_u *fname,
	DWORD dwDesiredAccess,
	DWORD dw_Share_ಬಹುಲಕ,
	LPSECURITY_ಗುಣಲಕ್ಷಣಗಳು lp_Security_ಗುಣಲಕ್ಷಣಗಳು,
	DWORD dwCreationDisposition,
	DWORD dw_Flags_And_ಗುಣಲಕ್ಷಣಗಳು)
{
    HANDLE h;
    WCHAR *wn;

    wn = enc_ಗಮ್ಯ_utf16(fname, ಖಾಲಿ);
    ಆದರೆ (wn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ INVALID_HANDLE_ಮೌಲ್ಯ;

    h = CreateFileW(wn, dwDesiredAccess, dw_Share_ಬಹುಲಕ,
	    lp_Security_ಗುಣಲಕ್ಷಣಗಳು, dwCreationDisposition,
	    dw_Flags_And_ಗುಣಲಕ್ಷಣಗಳು, ಖಾಲಿ);
    vim_free(wn);
    ಹಿಂತಿರುಗಿ h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    ಶೂನ್ಯ
win32_build_env(dict_T *env, garray_T *gap, ಪೂರ್ಣಾಂಕ is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != ಖಾಲಿ ? env->dv_hashtab.ht_ಬಳಸಿದ : 0;
    LPVOID	ತಳ = Get_Environment_ಅಕ್ಷರಸರಣಿಗಳು_W();

    // for last \0
    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (env != ಖಾಲಿ)
    {
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&env->dv_hashtab, hi, todo)
	{
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	    {
		typval_T *ಅಂಶ = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_ಗಮ್ಯ_utf16((char_u *)hi->hi_ಕೀಲಿ, ಖಾಲಿ);
		WCHAR   *wval = enc_ಗಮ್ಯ_utf16(tv_get_string(ಅಂಶ), ಖಾಲಿ);
		--todo;
		ಆದರೆ (wkey != ಖಾಲಿ && wval != ಖಾಲಿ)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    ಆದರೆ (ga_grow(gap, (ಪೂರ್ಣಾಂಕ)(lkey + lval + 2)) == FAIL)
			ಮುಂದುವರಿ;
		    ಗಾಗಿ (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) = L'=';
		    ಗಾಗಿ (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    ಆದರೆ (ತಳ)
    {
	WCHAR	*p = (WCHAR*) ತಳ;

	// for last \0
	ಆದರೆ (ga_grow(gap, 1) == FAIL)
	    ಹಿಂತಿರುಗಿ;

	ಆಗ (*p != 0 || *(p + 1) != 0)
	{
	    ಆದರೆ (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) = *p;
	    p++;
	}
	Free_Environment_ಅಕ್ಷರಸರಣಿಗಳು_W(ತಳ);
	*((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) = L'\0';
    }

# ಆದರೆ defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
    {
#  ifdef FEAT_CLIENTSERVER
	char_u	*servername = get_vim_var_str(VV_SEND_SERVER);
	size_t	servername_len = STRLEN(servername);
#  endif
#  ifdef FEAT_TERMINAL
	char_u	*ಆವೃತ್ತಿ_2 = get_vim_var_str(VV_ಆವೃತ್ತಿ);
	size_t	ಆವೃತ್ತಿ_len = STRLEN(ಆವೃತ್ತಿ_2);
#  endif
	// size of "VIM_SERVERNAME=" and value,
	// plus "VIM_TERMINAL=" and value,
	// plus two terminating NULs
	size_t	n = 0
#  ifdef FEAT_CLIENTSERVER
		    + 15 + servername_len
#  endif
#  ifdef FEAT_TERMINAL
		    + 13 + ಆವೃತ್ತಿ_len + 2
#  endif
		    ;

	ಆದರೆ (ga_grow(gap, (ಪೂರ್ಣಾಂಕ)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    ಗಾಗಿ (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    ಗಾಗಿ (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    ಆದರೆ (is_terminal)
	    {
		ಗಾಗಿ (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		ಗಾಗಿ (n = 0; n < ಆವೃತ್ತಿ_len; n++)
		    *((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) =
			(WCHAR)ಆವೃತ್ತಿ_2[n];
		*((WCHAR*)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++) = L'\0';
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
    ಅಕ್ಷರ		name[64];
    SECURITY_ಗುಣಲಕ್ಷಣಗಳು sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_ಗಾತ್ರ, 0, 0, ಖಾಲಿ);

    ಆದರೆ (handles[1] == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ FALSE;

    sa.n_ಉದ್ದಳತೆ = ಗಾತ್ರ(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = ಖಾಲಿ;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ, 0);

    ಆದರೆ (handles[0] == INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	CloseHandle(handles[1]);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_JOB_CHANNEL

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Execute "argv" directly without the shell and return the output.
 * Used by system() and systemlist() when the command is a List.
 * "infile" is an optional temp file for stdin input.
 * When "ret_len" is not NULL, set it to the length of the output.
 * Returns the output in allocated memory (or NULL on error).
 * Sets v:shell_error to the exit status.
 */
    char_u *
mch_get_cmd_output_ನೇರ(
    ಅಕ್ಷರ	**argv,
    char_u	*infile,
    ಪೂರ್ಣಾಂಕ		flags ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_ಗುಣಲಕ್ಷಣಗಳು saAttr;
    HANDLE		h_ಮಗು_Stdout_Rd = INVALID_HANDLE_ಮೌಲ್ಯ;
    HANDLE		h_ಮಗು_Stdout_Wr = INVALID_HANDLE_ಮೌಲ್ಯ;
    HANDLE		h_ಮಗು_Stdin_Rd = INVALID_HANDLE_ಮೌಲ್ಯ;
    garray_T		cmd_ga;
    garray_T		ಹೊರಹೋಗು_ga;
    char_u		*ಬಫರ್ = ಖಾಲಿ;
    DWORD		exit_ಸಂಕೇತ = (DWORD)-1;
    ಪೂರ್ಣಾಂಕ			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    ಗಾಗಿ (i = 0; argv[i] != ಖಾಲಿ; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	ಪೂರ್ಣಾಂಕ	has_spaces = FALSE;
	ಪೂರ್ಣಾಂಕ	j;

	ಗಾಗಿ (j = 0; s[j] != NUL; j++)
	    ಆದರೆ (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		ನಿಲ್ಲಿಸು;
	    }

	ಆದರೆ (i > 0)
	    ga_append(&cmd_ga, ' ');

	ಆದರೆ (has_spaces)
	{
	    ಪೂರ್ಣಾಂಕ	ಅಂಕೆ_bs;

	    ga_append(&cmd_ga, '"');
	    ಗಾಗಿ (j = 0; arg[j] != NUL; j++)
	    {
		ಅಂಕೆ_bs = 0;
		ಆಗ (arg[j] == '\\')
		{
		    ಅಂಕೆ_bs++;
		    j++;
		}

		ಆದರೆ (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    ಆಗ (ಅಂಕೆ_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    ಆಗ (ಅಂಕೆ_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆಗ (ಅಂಕೆ_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&ಹೊರಹೋಗು_ga, 1, 4096);

    saAttr.n_ಉದ್ದಳತೆ = ಗಾತ್ರ(SECURITY_ಗುಣಲಕ್ಷಣಗಳು);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ಖಾಲಿ;

    // Create a pipe for the child's stdout.
    ಆದರೆ (!CreatePipe(&h_ಮಗು_Stdout_Rd, &h_ಮಗು_Stdout_Wr, &saAttr, 0)
	    || !ಗಣ_Handle_Information(h_ಮಗು_Stdout_Rd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	ಹೋಗು ಮುಗಿದ;
    }

    // Set up stdin from infile if provided.
    ಆದರೆ (infile != ಖಾಲಿ)
    {
	WCHAR *winfile = enc_ಗಮ್ಯ_utf16(infile, ಖಾಲಿ);

	ಆದರೆ (winfile != ಖಾಲಿ)
	{
	    h_ಮಗು_Stdin_Rd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ, ಖಾಲಿ);
	    vim_free(winfile);
	}
    }

    Zero_ಸ್ಮರಣೆ(&pi, ಗಾತ್ರ(pi));
    Zero_ಸ್ಮರಣೆ(&si, ಗಾತ್ರ(si));
    si.cb = ಗಾತ್ರ(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.w_Show_ಕಿಟಕಿ = SW_HIDE;
    si.hStdOutput = h_ಮಗು_Stdout_Wr;
    si.hStdError = h_ಮಗು_Stdout_Wr;
    si.h_Std_ಒಳಹರಿವು = (h_ಮಗು_Stdin_Rd != INVALID_HANDLE_ಮೌಲ್ಯ)
		    ? h_ಮಗು_Stdin_Rd : INVALID_HANDLE_ಮೌಲ್ಯ;

    ch_log(ಖಾಲಿ, "directly executing: %s", (ಅಕ್ಷರ *)cmd_ga.ga_ದತ್ತಾಂಶ);

    // Create the child process directly, without going through the shell.
    ಆದರೆ (!vim_create_process((ಅಕ್ಷರ *)cmd_ga.ga_ದತ್ತಾಂಶ, TRUE,
		CREATE_DEFAULT_ERROR_ಬಹುಲಕ | CREATE_NEW_PROCESS_ಗುಂಪು,
		&si, &pi, ಖಾಲಿ, ಖಾಲಿ))
    {
	semsg(_(e_invalid_ವಾದ_str), cmd_ga.ga_ದತ್ತಾಂಶ);
	ಹೋಗು ಮುಗಿದ;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(h_ಮಗು_Stdout_Wr);
    h_ಮಗು_Stdout_Wr = INVALID_HANDLE_ಮೌಲ್ಯ;
    ಆದರೆ (h_ಮಗು_Stdin_Rd != INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	CloseHandle(h_ಮಗು_Stdin_Rd);
	h_ಮಗು_Stdin_Rd = INVALID_HANDLE_ಮೌಲ್ಯ;
    }

    // Read output from child process.
    ಗಾಗಿ (;;)
    {
	ಅಕ್ಷರ	buf[4096];
	DWORD	n;

	ಆದರೆ (!ReadFile(h_ಮಗು_Stdout_Rd, buf, ಗಾತ್ರ(buf), &n, ಖಾಲಿ) || n == 0)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (ga_grow(&ಹೊರಹೋಗು_ga, (ಪೂರ್ಣಾಂಕ)n) == OK)
	{
	    mch_memmove((ಅಕ್ಷರ *)ಹೊರಹೋಗು_ga.ga_ದತ್ತಾಂಶ + ಹೊರಹೋಗು_ga.ga_len, buf, n);
	    ಹೊರಹೋಗು_ga.ga_len += (ಪೂರ್ಣಾಂಕ)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    Get_Exit_ಸಂಕೇತ_Process(pi.hProcess, &exit_ಸಂಕೇತ);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    ಗಣ_vim_var_nr(VV_SHELL_ERROR, (ದೀರ್ಘ)exit_ಸಂಕೇತ);

    ಆದರೆ (ಹೊರಹೋಗು_ga.ga_len > 0)
    {
	ಬಫರ್ = alloc(ಹೊರಹೋಗು_ga.ga_len + 1);
	ಆದರೆ (ಬಫರ್ != ಖಾಲಿ)
	{
	    mch_memmove(ಬಫರ್, ಹೊರಹೋಗು_ga.ga_ದತ್ತಾಂಶ, ಹೊರಹೋಗು_ga.ga_len);
	    ಆದರೆ (ret_len == ಖಾಲಿ)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		ಗಾಗಿ (i = 0; i < ಹೊರಹೋಗು_ga.ga_len; ++i)
		    ಆದರೆ (ಬಫರ್[i] == NUL)
			ಬಫರ್[i] = 1;
		ಬಫರ್[ಹೊರಹೋಗು_ga.ga_len] = NUL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		*ret_len = ಹೊರಹೋಗು_ga.ga_len;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret_len != ಖಾಲಿ)
	*ret_len = 0;

ಮುಗಿದ:
    ga_clear(&cmd_ga);
    ga_clear(&ಹೊರಹೋಗು_ga);
    ಆದರೆ (h_ಮಗು_Stdout_Rd != INVALID_HANDLE_ಮೌಲ್ಯ)
	CloseHandle(h_ಮಗು_Stdout_Rd);
    ಆದರೆ (h_ಮಗು_Stdout_Wr != INVALID_HANDLE_ಮೌಲ್ಯ)
	CloseHandle(h_ಮಗು_Stdout_Wr);
    ಆದರೆ (h_ಮಗು_Stdin_Rd != INVALID_HANDLE_ಮೌಲ್ಯ)
	CloseHandle(h_ಮಗು_Stdin_Rd);
    ಹಿಂತಿರುಗಿ ಬಫರ್;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
    ಶೂನ್ಯ
mch_ಕೆಲಸ_ಆರಂಭ(ಅಕ್ಷರ *cmd, ಕೆಲಸ_T *ಕೆಲಸ, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_ಗುಣಲಕ್ಷಣಗಳು saAttr;
    channel_T		*channel = ಖಾಲಿ;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    ಪೂರ್ಣಾಂಕ		use_null_for_ಒಳ = options->jo_io[PART_ಒಳ] == JIO_NULL;
    ಪೂರ್ಣಾಂಕ		use_null_for_ಹೊರಹೋಗು = options->jo_io[PART_ಹೊರಹೋಗು] == JIO_NULL;
    ಪೂರ್ಣಾಂಕ		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    ಪೂರ್ಣಾಂಕ		use_file_for_ಒಳ = options->jo_io[PART_ಒಳ] == JIO_FILE;
    ಪೂರ್ಣಾಂಕ		use_file_for_ಹೊರಹೋಗು = options->jo_io[PART_ಹೊರಹೋಗು] == JIO_FILE;
    ಪೂರ್ಣಾಂಕ		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    ಪೂರ್ಣಾಂಕ		use_ಹೊರಹೋಗು_for_err = options->jo_io[PART_ERR] == JIO_ಹೊರಹೋಗು;

    ಆದರೆ (use_ಹೊರಹೋಗು_for_err && use_null_for_ಹೊರಹೋಗು)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_ಮೌಲ್ಯ;
    ifd[1] = INVALID_HANDLE_ಮೌಲ್ಯ;
    ofd[0] = INVALID_HANDLE_ಮೌಲ್ಯ;
    ofd[1] = INVALID_HANDLE_ಮೌಲ್ಯ;
    efd[0] = INVALID_HANDLE_ಮೌಲ್ಯ;
    efd[1] = INVALID_HANDLE_ಮೌಲ್ಯ;
    ga_init2(&ga, ಗಾತ್ರ(wchar_t), 500);

    jo = Create_ಕೆಲಸ_Object(ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (jo == ಖಾಲಿ)
    {
	ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_FAILED;
	ಹೋಗು failed;
    }

    ಆದರೆ (options->jo_env != ಖಾಲಿ)
	win32_build_env(options->jo_env, &ga, FALSE);

    Zero_ಸ್ಮರಣೆ(&pi, ಗಾತ್ರ(pi));
    Zero_ಸ್ಮರಣೆ(&si, ಗಾತ್ರ(si));
    si.cb = ಗಾತ್ರ(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.w_Show_ಕಿಟಕಿ = SW_HIDE;

    saAttr.n_ಉದ್ದಳತೆ = ಗಾತ್ರ(SECURITY_ಗುಣಲಕ್ಷಣಗಳು);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ಖಾಲಿ;

    ಆದರೆ (use_file_for_ಒಳ)
    {
	char_u *fname = options->jo_io_name[PART_ಒಳ];

	ifd[0] = ಕೆಲಸ_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ);
	ಆದರೆ (ifd[0] == INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಹೋಗು failed;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!use_null_for_ಒಳ
	    && (!create_pipe_pair(ifd)
		|| !ಗಣ_Handle_Information(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	ಹೋಗು failed;

    ಆದರೆ (use_file_for_ಹೊರಹೋಗು)
    {
	char_u *fname = options->jo_io_name[PART_ಹೊರಹೋಗು];

	ofd[1] = ಕೆಲಸ_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ);
	ಆದರೆ (ofd[1] == INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಹೋಗು failed;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!use_null_for_ಹೊರಹೋಗು &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !ಗಣ_Handle_Information(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	ಹೋಗು failed;

    ಆದರೆ (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = ಕೆಲಸ_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ);
	ಆದರೆ (efd[1] == INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಹೋಗು failed;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!use_ಹೊರಹೋಗು_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !ಗಣ_Handle_Information(efd[0], HANDLE_FLAG_INHERIT, 0)))
	ಹೋಗು failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_ಒಳಹರಿವು = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_ಹೊರಹೋಗು_for_err ? ofd[1] : efd[1];

    ಆದರೆ (!use_null_for_ಒಳ || !use_null_for_ಹೊರಹೋಗು || !use_null_for_err)
    {
	ಆದರೆ (options->jo_ಗಣ & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    ಆದರೆ (channel != ಖಾಲಿ)
		++channel->ch_refcount;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    channel = ಕೂಡಿಸು_channel();
	ಆದರೆ (channel == ಖಾಲಿ)
	    ಹೋಗು failed;
    }

    ಆದರೆ (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_ಬಹುಲಕ |
	    CREATE_NEW_PROCESS_ಗುಂಪು |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_ದತ್ತಾಂಶ,
	    (ಅಕ್ಷರ *)options->jo_cwd))
    {
	CloseHandle(jo);
	ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_FAILED;
	ಹೋಗು failed;
    }

    ga_clear(&ga);

    ಆದರೆ (!ನಿಯೋಜಿಸು_Process_ಗಮ್ಯ_ಕೆಲಸ_Object(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = ಖಾಲಿ;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    ಕೆಲಸ->jv_proc_info = pi;
    ಕೆಲಸ->jv_ಕೆಲಸ_object = jo;
    ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    ಆದರೆ (!use_ಹೊರಹೋಗು_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    ಕೆಲಸ->jv_channel = channel;
    ಆದರೆ (channel != ಖಾಲಿ)
    {
	channel_ಗಣ_pipes(channel,
		      use_file_for_ಒಳ || use_null_for_ಒಳ
					      ? INVALID_FD : (sock_T)ifd[1],
		      use_file_for_ಹೊರಹೋಗು || use_null_for_ಹೊರಹೋಗು
					     ? INVALID_FD : (sock_T)ofd[0],
		      use_ಹೊರಹೋಗು_for_err || use_file_for_err || use_null_for_err
					    ? INVALID_FD : (sock_T)efd[0]);
	channel_ಗಣ_ಕೆಲಸ(channel, ಕೆಲಸ, options);
    }
    ಹಿಂತಿರುಗಿ;

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

    ಅಕ್ಷರ *
mch_ಕೆಲಸ_ಸ್ಥಿತಿ(ಕೆಲಸ_T *ಕೆಲಸ)
{
    DWORD dw_Exit_ಸಂಕೇತ = 0;

    ಆದರೆ (!Get_Exit_ಸಂಕೇತ_Process(ಕೆಲಸ->jv_proc_info.hProcess, &dw_Exit_ಸಂಕೇತ)
	    || dw_Exit_ಸಂಕೇತ != STILL_ಸಕ್ರಿಯ)
    {
	ಕೆಲಸ->jv_exitval = (ಪೂರ್ಣಾಂಕ)dw_Exit_ಸಂಕೇತ;
	ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ < ಕೆಲಸ_ENDED)
	{
	    ch_log(ಕೆಲಸ->jv_channel, "Job ended");
	    ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_ENDED;
	}
	ಹಿಂತಿರುಗಿ "dead";
    }
    ಹಿಂತಿರುಗಿ "run";
}

    ಕೆಲಸ_T *
mch_detect_ended_ಕೆಲಸ(ಕೆಲಸ_T *ಕೆಲಸ_list)
{
    HANDLE ಕೆಲಸ_Handles[ಗರಿಷ್ಠ_WAIT_OBJECTS];
    ಕೆಲಸ_T *ಕೆಲಸ_Array[ಗರಿಷ್ಠ_WAIT_OBJECTS];
    ಕೆಲಸ_T *ಕೆಲಸ = ಕೆಲಸ_list;

    ಆಗ (ಕೆಲಸ != ಖಾಲಿ)
    {
	DWORD n;
	DWORD ಫಲಿತಾಂಶ;

	ಗಾಗಿ (n = 0; n < ಗರಿಷ್ಠ_WAIT_OBJECTS
				       && ಕೆಲಸ != ಖಾಲಿ; ಕೆಲಸ = ಕೆಲಸ->jv_ಮುಂದಿನ)
	{
	    ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_STARTED)
	    {
		ಕೆಲಸ_Handles[n] = ಕೆಲಸ->jv_proc_info.hProcess;
		ಕೆಲಸ_Array[n] = ಕೆಲಸ;
		++n;
	    }
	}
	ಆದರೆ (n == 0)
	    ಮುಂದುವರಿ;
	ಫಲಿತಾಂಶ = Wait_For_ಅಪವರ್ತ್ಯ_Objects(n, ಕೆಲಸ_Handles, FALSE, 0);
	ಆದರೆ (ಫಲಿತಾಂಶ >= WAIT_OBJECT_0 && ಫಲಿತಾಂಶ < WAIT_OBJECT_0 + n)
	{
	    ಕೆಲಸ_T *wait_ಕೆಲಸ = ಕೆಲಸ_Array[ಫಲಿತಾಂಶ - WAIT_OBJECT_0];

	    ಆದರೆ (STRCMP(mch_ಕೆಲಸ_ಸ್ಥಿತಿ(wait_ಕೆಲಸ), "dead") == 0)
		ಹಿಂತಿರುಗಿ wait_ಕೆಲಸ;
	}
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ BOOL
terminate_all(HANDLE process, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_ಮೌಲ್ಯ;
    DWORD	    pid = GetProcessId(process);

    ಆದರೆ (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	ಆದರೆ (h != INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    pe.dw_ಗಾತ್ರ = ಗಾತ್ರ(PROCESSENTRY32);
	    ಆದರೆ (!Process32_ಮೊದಲ(h, &pe))
		ಹೋಗು theend;

	    ಮಾಡು
	    {
		ಆದರೆ (pe.th32_ಪೋಷಕ_Process_ID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    ಆದರೆ (ph != ಖಾಲಿ)
		    {
			terminate_all(ph, ಸಂಕೇತ_2);
			CloseHandle(ph);
		    }
		}
	    } ಆಗ (Process32_ಮುಂದಿನ(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    ಹಿಂತಿರುಗಿ TerminateProcess(process, ಸಂಕೇತ_2);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    ಪೂರ್ಣಾಂಕ
mch_signal_ಕೆಲಸ(ಕೆಲಸ_T *ಕೆಲಸ, char_u *how)
{
    ಪೂರ್ಣಾಂಕ ret;

    ಆದರೆ (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	ಆದರೆ (ಕೆಲಸ->jv_ಕೆಲಸ_object != ಖಾಲಿ)
	{
	    ಆದರೆ (ಕೆಲಸ->jv_channel != ಖಾಲಿ && ಕೆಲಸ->jv_channel->ch_anonymous_pipe)
		ಕೆಲಸ->jv_channel->ch_killing = TRUE;
	    ಹಿಂತಿರುಗಿ Terminate_ಕೆಲಸ_Object(ಕೆಲಸ->jv_ಕೆಲಸ_object, (UINT)-1) ? OK : FAIL;
	}
	ಹಿಂತಿರುಗಿ terminate_all(ಕೆಲಸ->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    ಆದರೆ (!AttachConsole(ಕೆಲಸ->jv_proc_info.dwProcessId))
	ಹಿಂತಿರುಗಿ FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		ಕೆಲಸ->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Clear the data related to "job".
 */
    ಶೂನ್ಯ
mch_clear_ಕೆಲಸ(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_FAILED)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಕೆಲಸ->jv_ಕೆಲಸ_object != ಖಾಲಿ)
	CloseHandle(ಕೆಲಸ->jv_ಕೆಲಸ_object);
    CloseHandle(ಕೆಲಸ->jv_proc_info.hProcess);
}
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
termcap_ಬಹುಲಕ_ಆರಂಭ(ಶೂನ್ಯ)
{
    DWORD cmodein;

    ಆದರೆ (g_f_Termcap_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ;

    Save_Console_ಬಫರ್(&g_cbNonTermcap);

    ಆದರೆ (g_cbTermcap.IsValid)
    {
	/*
	 * We've been in termcap mode before.  Restore certain screen
	 * characteristics, including the buffer size and the window
	 * size.  Since we will be redrawing the screen, we don't need
	 * to restore the actual contents of the buffer.
	 */
	Restore_Console_ಬಫರ್(&g_cbTermcap, FALSE);
	reset_console_color_rgb();
	ಗಣ_Console_ಕಿಟಕಿ_Info(g_h_Con_ಹೊರಹೋಗು, TRUE, &g_cbTermcap.Info.sr_ಕಿಟಕಿ);
	ಅಡ್ಡಸಾಲುಗಳು = g_cbTermcap.Info.dw_ಗಾತ್ರ.Y;
	Columns = g_cbTermcap.Info.dw_ಗಾತ್ರ.X;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * This is our first time entering termcap mode.  Clear the console
	 * screen buffer, and resize the buffer to match the current window
	 * size.  We will use this as the size of our editing environment.
	 */
	Clear_Console_ಬಫರ್(g_attrCurrent);
	ಗಣ_console_color_rgb();
	Resize_Con_Buf_And_ಕಿಟಕಿ(g_h_Con_ಹೊರಹೋಗು, Columns, ಅಡ್ಡಸಾಲುಗಳು);
    }

    resettitle();

    Get_Console_ಬಹುಲಕ(g_h_Con_ಒಳ, &cmodein);
    ಆದರೆ (g_f_Mouse_ಸಕ್ರಿಯ)
    {
	cmodein |= ENABLE_MOUSE_ಒಳಹರಿವು;
	cmodein &= ~ENABLE_ಶೀಘ್ರ_EDIT_ಬಹುಲಕ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	cmodein &= ~ENABLE_MOUSE_ಒಳಹರಿವು;
	cmodein |= g_cmodein & ENABLE_ಶೀಘ್ರ_EDIT_ಬಹುಲಕ;
    }
    cmodein |= ENABLE_ಕಿಟಕಿ_ಒಳಹರಿವು;
    ಗಣ_Console_ಬಹುಲಕ(g_h_Con_ಒಳ, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_f_Termcap_ಬಹುಲಕ = TRUE;
}


/*
 * End termcap mode
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
termcap_ಬಹುಲಕ_ಕೊನೆ(ಶೂನ್ಯ)
{
    DWORD cmodein;
    Console_ಬಫರ್ *cb;
    COORD coord;
    DWORD dwDummy;

    ಆದರೆ (!g_f_Termcap_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ;

    Save_Console_ಬಫರ್(&g_cbTermcap);

    Get_Console_ಬಹುಲಕ(g_h_Con_ಒಳ, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_ಒಳಹರಿವು | ENABLE_ಕಿಟಕಿ_ಒಳಹರಿವು);
    cmodein |= g_cmodein & ENABLE_ಶೀಘ್ರ_EDIT_ಬಹುಲಕ;
    ಗಣ_Console_ಬಹುಲಕ(g_h_Con_ಒಳ, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# ಇಲ್ಲದಿದ್ದರೆ
    cb = &g_cbNonTermcap;
# endif
    Restore_Console_ಬಫರ್(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    ಆದರೆ (exiting && use_alternate_screen_ಬಫರ್)
	vtp_printf("\033[?1049l");

    ಆದರೆ (!USE_WT && (p_rs || exiting))
    {
	/*
	 * Clear anything that happens to be on the current line.
	 */
	coord.X = 0;
	coord.Y = (SHORT) (p_rs ? cb->Info.dwCursorPosition.Y : (ಅಡ್ಡಸಾಲುಗಳು - 1));
	FillConsoleOutputCharacter(g_h_Con_ಹೊರಹೋಗು, ' ',
		cb->Info.dw_ಗಾತ್ರ.X, coord, &dwDummy);
	/*
	 * The following is just for aesthetics.  If we are exiting without
	 * restoring the screen, then we want to have a prompt string
	 * appear at the bottom line.  However, the command interpreter
	 * seems to always advance the cursor one line before displaying
	 * the prompt string, which causes the screen to scroll.  To
	 * counter this, move the cursor up one line before exiting.
	 */
	ಆದರೆ (exiting && !p_rs)
	    coord.Y--;
	/*
	 * Position the cursor at the leftmost column of the desired row.
	 */
	ಗಣ_Console_Cursor_Position(g_h_Con_ಹೊರಹೋಗು, coord);
    }
    ಗಣ_Console_Cursor_Info(g_h_Con_ಹೊರಹೋಗು, &g_cci);
    g_f_Termcap_ಬಹುಲಕ = FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_chars(
    COORD coord,
    DWORD n)
{
    ಆದರೆ (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_h_Con_ಹೊರಹೋಗು, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_h_Con_ಹೊರಹೋಗು, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಣ_console_color_rgb();
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

    ಆದರೆ (!vtp_working)
	clear_chars(g_coord, ಅಡ್ಡಸಾಲುಗಳು * Columns);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಣ_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_ಗಮ್ಯ_ಕೊನೆ_ದ_display(ಶೂನ್ಯ)
{
    COORD save = g_coord;

    ಆದರೆ (!vtp_working)
	clear_chars(g_coord, (ಅಡ್ಡಸಾಲುಗಳು - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಣ_console_color_rgb();
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
clear_ಗಮ್ಯ_ಕೊನೆ_ದ_ಸಾಲು(ಶೂನ್ಯ)
{
    COORD save = g_coord;

    ಆದರೆ (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಣ_console_color_rgb();
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
scroll(ಚಿಹ್ನೆರಹಿತ c_ಸಾಲುಗಳು)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.ಎಡ_4 + 1, g_srScrollRegion.ತುದಿ_2 + 1);
    delete_ಸಾಲುಗಳು(c_ಸಾಲುಗಳು);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_scroll_region(
    ಚಿಹ್ನೆರಹಿತ ಎಡ_2,
    ಚಿಹ್ನೆರಹಿತ ತುದಿ,
    ಚಿಹ್ನೆರಹಿತ ಬಲ_2,
    ಚಿಹ್ನೆರಹಿತ bottom)
{
    ಆದರೆ (ಎಡ_2 >= ಬಲ_2
	    || ತುದಿ >= bottom
	    || ಬಲ_2 > (ಚಿಹ್ನೆರಹಿತ) Columns - 1
	    || bottom > (ಚಿಹ್ನೆರಹಿತ) ಅಡ್ಡಸಾಲುಗಳು - 1)
	ಹಿಂತಿರುಗಿ;

    g_srScrollRegion.ಎಡ_4 =   ಎಡ_2;
    g_srScrollRegion.ತುದಿ_2 =    ತುದಿ;
    g_srScrollRegion.ಬಲ_4 =  ಬಲ_2;
    g_srScrollRegion.Bottom = bottom;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_scroll_region_tb(
    ಚಿಹ್ನೆರಹಿತ ತುದಿ,
    ಚಿಹ್ನೆರಹಿತ bottom)
{
    ಆದರೆ (ತುದಿ >= bottom || bottom > (ಚಿಹ್ನೆರಹಿತ)ಅಡ್ಡಸಾಲುಗಳು - 1)
	ಹಿಂತಿರುಗಿ;

    g_srScrollRegion.ತುದಿ_2 = ತುದಿ;
    g_srScrollRegion.Bottom = bottom;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_scroll_region_lr(
    ಚಿಹ್ನೆರಹಿತ ಎಡ_2,
    ಚಿಹ್ನೆರಹಿತ ಬಲ_2)
{
    ಆದರೆ (ಎಡ_2 >= ಬಲ_2 || ಬಲ_2 > (ಚಿಹ್ನೆರಹಿತ)Columns - 1)
	ಹಿಂತಿರುಗಿ;

    g_srScrollRegion.ಎಡ_4 = ಎಡ_2;
    g_srScrollRegion.ಬಲ_4 = ಬಲ_2;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೇರಿಸು_ಸಾಲುಗಳು(ಚಿಹ್ನೆರಹಿತ c_ಸಾಲುಗಳು)
{
    SMALL_RECT	    ಮೂಲ, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.ಎಡ_4 + 1, g_srScrollRegion.ತುದಿ_2 + 1);

    dest.X = g_srScrollRegion.ಎಡ_4;
    dest.Y = g_coord.Y + c_ಸಾಲುಗಳು;

    ಮೂಲ.ಎಡ_4   = g_srScrollRegion.ಎಡ_4;
    ಮೂಲ.ತುದಿ_2	  = g_coord.Y;
    ಮೂಲ.ಬಲ_4  = g_srScrollRegion.ಬಲ_4;
    ಮೂಲ.Bottom = g_srScrollRegion.Bottom - c_ಸಾಲುಗಳು;

    clip.ಎಡ_4   = g_srScrollRegion.ಎಡ_4;
    clip.ತುದಿ_2    = g_coord.Y;
    clip.ಬಲ_4  = g_srScrollRegion.ಬಲ_4;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    ಆದರೆ (!USE_VTP)
	fill.ಗುಣಲಕ್ಷಣಗಳು_2 = g_attrCurrent;
    ಇಲ್ಲದಿದ್ದರೆ
	fill.ಗುಣಲಕ್ಷಣಗಳು_2 = g_attrDefault;

    ಗಣ_console_color_rgb();

    Scroll_Console_Screen_ಬಫರ್(g_h_Con_ಹೊರಹೋಗು, &ಮೂಲ, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    ಆದರೆ (ಮೂಲ.Bottom < dest.Y)
    {
	COORD coord;
	ಪೂರ್ಣಾಂಕ   i;

	coord.X = ಮೂಲ.ಎಡ_4;
	ಗಾಗಿ (i = clip.ತುದಿ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ಮೂಲ.ಬಲ_4 - ಮೂಲ.ಎಡ_4 + 1);
	}
    }

    ಆದರೆ (vtp_working)
    {
	COORD coord;
	ಪೂರ್ಣಾಂಕ i;

	coord.X = ಮೂಲ.ಎಡ_4;
	ಗಾಗಿ (i = ಮೂಲ.ತುದಿ_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ಮೂಲ.ಬಲ_4 - ಮೂಲ.ಎಡ_4 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
delete_ಸಾಲುಗಳು(ಚಿಹ್ನೆರಹಿತ c_ಸಾಲುಗಳು)
{
    SMALL_RECT	    ಮೂಲ, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    ಪೂರ್ಣಾಂಕ		    nb;

    gotoxy(g_srScrollRegion.ಎಡ_4 + 1, g_srScrollRegion.ತುದಿ_2 + 1);

    dest.X = g_srScrollRegion.ಎಡ_4;
    dest.Y = g_coord.Y;

    ಮೂಲ.ಎಡ_4   = g_srScrollRegion.ಎಡ_4;
    ಮೂಲ.ತುದಿ_2	  = g_coord.Y + c_ಸಾಲುಗಳು;
    ಮೂಲ.ಬಲ_4  = g_srScrollRegion.ಬಲ_4;
    ಮೂಲ.Bottom = g_srScrollRegion.Bottom;

    clip.ಎಡ_4   = g_srScrollRegion.ಎಡ_4;
    clip.ತುದಿ_2    = g_coord.Y;
    clip.ಬಲ_4  = g_srScrollRegion.ಬಲ_4;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    ಆದರೆ (!vtp_working)
	fill.ಗುಣಲಕ್ಷಣಗಳು_2 = g_attrCurrent;
    ಇಲ್ಲದಿದ್ದರೆ
	fill.ಗುಣಲಕ್ಷಣಗಳು_2 = g_attrDefault;

    ಗಣ_console_color_rgb();

    Scroll_Console_Screen_ಬಫರ್(g_h_Con_ಹೊರಹೋಗು, &ಮೂಲ, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (ಮೂಲ.Bottom - ಮೂಲ.ತುದಿ_2) + 1;

    ಆದರೆ (nb < ಮೂಲ.ತುದಿ_2)
    {
	COORD coord;
	ಪೂರ್ಣಾಂಕ   i;

	coord.X = ಮೂಲ.ಎಡ_4;
	ಗಾಗಿ (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ಮೂಲ.ಬಲ_4 - ಮೂಲ.ಎಡ_4 + 1);
	}
    }

    ಆದರೆ (vtp_working)
    {
	COORD coord;
	ಪೂರ್ಣಾಂಕ i;

	coord.X = ಮೂಲ.ಎಡ_4;
	ಗಾಗಿ (i = nb; i <= ಮೂಲ.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ಮೂಲ.ಬಲ_4 - ಮೂಲ.ಎಡ_4 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gotoxy(
    ಚಿಹ್ನೆರಹಿತ x,
    ಚಿಹ್ನೆರಹಿತ y)
{
    ಆದರೆ (x < 1 || x > (ಚಿಹ್ನೆರಹಿತ)Columns || y < 1 || y > (ಚಿಹ್ನೆರಹಿತ)ಅಡ್ಡಸಾಲುಗಳು)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!USE_VTP)
    {
	// There are reports of double-width characters not displayed
	// correctly.  This workaround should fix it, similar to how it's done
	// for VTP.
	g_coord.X = 0;
	ಗಣ_Console_Cursor_Position(g_h_Con_ಹೊರಹೋಗು, g_coord);

	// external cursor coords are 1-based; internal are 0-based
	g_coord.X = x - 1;
	g_coord.Y = y - 1;
	ಗಣ_Console_Cursor_Position(g_h_Con_ಹೊರಹೋಗು, g_coord);
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
textattr(ಶಬ್ದ_2 wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    ಗಣ_Console_ಪಠ್ಯ_Attribute(g_h_Con_ಹೊರಹೋಗು, wAttr);
}


    ಸ್ಥಿರ ಶೂನ್ಯ
textcolor(ಶಬ್ದ_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    ಆದರೆ (!vtp_working)
	ಗಣ_Console_ಪಠ್ಯ_Attribute(g_h_Con_ಹೊರಹೋಗು, g_attrCurrent);
    ಇಲ್ಲದಿದ್ದರೆ
	vtp_sgr_bulk(wAttr);
}


    ಸ್ಥಿರ ಶೂನ್ಯ
textbackground(ಶಬ್ದ_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    ಆದರೆ (!vtp_working)
	ಗಣ_Console_ಪಠ್ಯ_Attribute(g_h_Con_ಹೊರಹೋಗು, g_attrCurrent);
    ಇಲ್ಲದಿದ್ದರೆ
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
normvideo(ಶೂನ್ಯ)
{
    ಆದರೆ (!vtp_working)
	textattr(g_attrDefault);
    ಇಲ್ಲದಿದ್ದರೆ
	vtp_sgr_bulk(0);
}


ಸ್ಥಿರ ಶಬ್ದ_2 g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
standout(ಶೂನ್ಯ)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((ಶಬ್ದ_2) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
standend(ಶೂನ್ಯ)
{
    ಆದರೆ (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    ಶೂನ್ಯ
mch_ಗಣ_ಸಾಮಾನ್ಯ_colors(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		n;

    cterm_ಸಾಮಾನ್ಯ_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_ಸಾಮಾನ್ಯ_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    ಆದರೆ (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	ಆದರೆ (*p == 'm' && n > 0)
	{
	    cterm_ಸಾಮಾನ್ಯ_fg_color = (n & 0xf) + 1;
	    cterm_ಸಾಮಾನ್ಯ_bg_color = ((n >> 4) & 0xf) + 1;
	}
    }
# ifdef FEAT_TERMGUICOLORS
    cterm_ಸಾಮಾನ್ಯ_fg_gui_color = INVALCOLOR;
    cterm_ಸಾಮಾನ್ಯ_bg_gui_color = INVALCOLOR;
# endif
}


/*
 * visual bell: flash the screen
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
visual_bell(ಶೂನ್ಯ)
{
    COORD   coordOrigin = {0, 0};
    ಶಬ್ದ_2    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = ಖಾಲಿ;

# ifdef FEAT_TERMGUICOLORS
    ಆದರೆ (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(ಶಬ್ದ_2, ಅಡ್ಡಸಾಲುಗಳು * Columns);
	ಆದರೆ (oldattrs == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ReadConsoleOutputAttribute(g_h_Con_ಹೊರಹೋಗು, oldattrs, ಅಡ್ಡಸಾಲುಗಳು * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_h_Con_ಹೊರಹೋಗು, attrFlash, ಅಡ್ಡಸಾಲುಗಳು * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    ಆದರೆ (oldattrs != ಖಾಲಿ)
    {
	WriteConsoleOutputAttribute(g_h_Con_ಹೊರಹೋಗು, oldattrs, ಅಡ್ಡಸಾಲುಗಳು * Columns,
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

    ಆದರೆ (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	ಹಿಂತಿರುಗಿ;
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
    DWORD  cb_ಗಮ್ಯ_Write)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    ಸ್ಥಿರ DWORD    cells;
    ಸ್ಥಿರ WCHAR    *unicodebuf = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    unibuflen = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    ಉದ್ದಳತೆ;
    ಪೂರ್ಣಾಂಕ		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    ಸ್ಥಿರ WCHAR    *utf8spbuf = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    utf8splength;
    ಸ್ಥಿರ DWORD    utf8spcells;
    ಸ್ಥಿರ WCHAR    **utf8usingbuf = &unicodebuf;

    ಆದರೆ (cb_ಗಮ್ಯ_Write != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	ಮಾಡು
	{
	    ಉದ್ದಳತೆ = Multi_Byte_ಗಮ್ಯ_Wide_Char(cp, 0, (LPCSTR)pchBuf, cb_ಗಮ್ಯ_Write,
							unicodebuf, unibuflen);
	    ಆದರೆ (ಉದ್ದಳತೆ && ಉದ್ದಳತೆ <= unibuflen)
		ನಿಲ್ಲಿಸು;
	    vim_free(unicodebuf);
	    unicodebuf = ಉದ್ದಳತೆ ? LALLOC_MULT(WCHAR, ಉದ್ದಳತೆ) : ಖಾಲಿ;
	    unibuflen = unibuflen ? 0 : ಉದ್ದಳತೆ;
	} ಆಗ (TRUE);
	cells = mb_string2cells(pchBuf, cb_ಗಮ್ಯ_Write);
    }
    ಇಲ್ಲದಿದ್ದರೆ // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	ಆದರೆ (utf8usingbuf != &utf8spbuf)
	{
	    ಆದರೆ (utf8spbuf == ಖಾಲಿ)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		ಉದ್ದಳತೆ = Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, 0, " ", 1, ಖಾಲಿ, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, ಉದ್ದಳತೆ);
		ಆದರೆ (utf8spbuf != ಖಾಲಿ)
		{
		    Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, 0, " ", 1, utf8spbuf, ಉದ್ದಳತೆ);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = ಉದ್ದಳತೆ;
		    utf8spcells = cells;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		utf8usingbuf = &utf8spbuf;
		ಉದ್ದಳತೆ = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    ಆದರೆ (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_h_Con_ಹೊರಹೋಗು, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	ಆದರೆ (WriteConsoleOutputCharacterW(g_h_Con_ಹೊರಹೋಗು, *utf8usingbuf, ಉದ್ದಳತೆ,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (WriteConsoleW(g_h_Con_ಹೊರಹೋಗು, *utf8usingbuf, ಉದ್ದಳತೆ, &cchwritten,
		    ಖಾಲಿ) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    ಆದರೆ (cchwritten == (DWORD)ಉದ್ದಳತೆ)
    {
	written = cb_ಗಮ್ಯ_Write;
	g_coord.X += (SHORT)cells;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *p = pchBuf;
	ಗಾಗಿ (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    ಆಗ (g_coord.X > g_srScrollRegion.ಬಲ_4)
    {
	g_coord.X -= (SHORT) Columns;
	ಆದರೆ (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    ಆದರೆ (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    ಹಿಂತಿರುಗಿ written;
}

    ಸ್ಥಿರ char_u *
get_seq(
    ಪೂರ್ಣಾಂಕ *ವಾದಗಳು,
    ಪೂರ್ಣಾಂಕ *ಎಣಿಕೆ_2,
    char_u *ತಲೆ)
{
    ಪೂರ್ಣಾಂಕ argc;
    char_u *p;

    ಆದರೆ (ತಲೆ == ಖಾಲಿ || *ತಲೆ != '\033')
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    argc = 0;
    p = ತಲೆ;
    ++p;
    ಮಾಡು
    {
	++p;
	ವಾದಗಳು[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } ಆಗ (*p == ';');
    *ಎಣಿಕೆ_2 = argc;

    ಹಿಂತಿರುಗಿ p;
}

    ಸ್ಥಿರ char_u *
get_sgr(
    ಪೂರ್ಣಾಂಕ *ವಾದಗಳು,
    ಪೂರ್ಣಾಂಕ *ಎಣಿಕೆ_2,
    char_u *ತಲೆ)
{
    char_u *p = get_seq(ವಾದಗಳು, ಎಣಿಕೆ_2, ತಲೆ);

    ಹಿಂತಿರುಗಿ (p && *p == 'm') ? ++p : ಖಾಲಿ;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    ಸ್ಥಿರ char_u *
sgrn2(
    char_u *ತಲೆ,
    ಪೂರ್ಣಾಂಕ	   n)
{
    ಪೂರ್ಣಾಂಕ argc;
    ಪೂರ್ಣಾಂಕ ವಾದಗಳು[16];
    char_u *p = get_sgr(ವಾದಗಳು, &argc, ತಲೆ);

    ಹಿಂತಿರುಗಿ p && argc == 5 && ವಾದಗಳು[0] == n && ವಾದಗಳು[1] == 2 ? p : ಖಾಲಿ;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    ಸ್ಥಿರ char_u *
sgrnc(
    char_u *ತಲೆ,
    ಪೂರ್ಣಾಂಕ	   n)
{
    ಪೂರ್ಣಾಂಕ argc;
    ಪೂರ್ಣಾಂಕ ವಾದಗಳು[16];
    char_u *p = get_sgr(ವಾದಗಳು, &argc, ತಲೆ);

    ಹಿಂತಿರುಗಿ p && argc == 1 && ವಾದಗಳು[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : ಖಾಲಿ;
}

    ಸ್ಥಿರ char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    ಆಗ (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    ಸ್ಥಿರ char_u *
sgrn2c(
    char_u *ತಲೆ,
    ಪೂರ್ಣಾಂಕ	   n)
{
    char_u *p = sgrn2(ತಲೆ, n);

    ಹಿಂತಿರುಗಿ p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : ಖಾಲಿ;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    ಸ್ಥಿರ char_u *
sgrn2cn(
    char_u *ತಲೆ,
    ಪೂರ್ಣಾಂಕ n)
{
    char_u *p = sgrn2(ತಲೆ, n);

    ಹಿಂತಿರುಗಿ p && p[0] == 0x0a && p[1] == '\033' ? ++p : ಖಾಲಿ;
}

ಸ್ಥಿರ bool ಒಳ_dcs = false;	// inside a DCS (sixel) sequence
ಸ್ಥಿರ bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
write_vt(char_u *s, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, ಖಾಲಿ, 0);
    ಆದರೆ (wlen <= 0)
	ಹಿಂತಿರುಗಿ;
    ws = ALLOC_MULT(WCHAR, wlen);
    ಆದರೆ (ws == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_h_Con_ಹೊರಹೋಗು, ws, wlen, &written, ಖಾಲಿ);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    ಸ್ಥಿರ char_u *
skip_dcs(char_u *s, char_u *ಕೊನೆ)
{
    ಗಾಗಿ (char_u *p = s; p < ಕೊನೆ; ++p)
    {
	ಆದರೆ (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    ಒಳ_dcs = false;
	    ಹಿಂತಿರುಗಿ p + 1;
	}
	dcs_esc = *p == ESC;
    }
    ಹಿಂತಿರುಗಿ ಕೊನೆ;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    ಶೂನ್ಯ
mch_write(
    char_u  *s,
    ಪೂರ್ಣಾಂಕ	    len)
{
    char_u  *ಕೊನೆ = s + len;

# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;
# endif

    ಆದರೆ (!ಪದ_console)
    {
	ಲಿಖ(1, s, (ಚಿಹ್ನೆರಹಿತ)len);
	ಹಿಂತಿರುಗಿ;
    }

    // translate ESC | sequences into faked bios calls
    ಆಗ (len--)
    {
	ಪೂರ್ಣಾಂಕ ಪೂರ್ವಪ್ರತ್ಯಯ = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	ಆದರೆ (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (ಒಳ_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    ಪೂರ್ಣಾಂಕ	l = (ಪೂರ್ಣಾಂಕ)(skip_dcs(s, ಕೊನೆ) - s);

	    ಆದರೆ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    ಮುಂದುವರಿ;
	}

	ಆಗ (s + ++ಪೂರ್ವಪ್ರತ್ಯಯ < ಕೊನೆ)
	{
	    ch = s[ಪೂರ್ವಪ್ರತ್ಯಯ];
	    ಆದರೆ (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ != 0)
		ಪೂರ್ವಪ್ರತ್ಯಯ = 1;
	}

	ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, ಪೂರ್ವಪ್ರತ್ಯಯ);
# ifdef MCH_WRITE_DUMP
	    ಆದರೆ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, ಗಾತ್ರ(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    ಆದರೆ (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.ಎಡ_4 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		gotoxy(g_srScrollRegion.ಎಡ_4 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    ಆದರೆ (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.ಎಡ_4+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ಆದರೆ (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    ಆದರೆ (g_coord.X > g_srScrollRegion.ಎಡ_4)
		g_coord.X--;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (g_coord.Y > g_srScrollRegion.ತುದಿ_2)
	    {
		g_coord.X = g_srScrollRegion.ಬಲ_4;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ಆದರೆ (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == '\a')
	{
	    // \a, bell
	    ಸಂದೇಶ_Beep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    ಆದರೆ (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    ಪೂರ್ಣಾಂಕ	    arg1 = 0, arg2 = 0, argc = 0, ವಾದಗಳು[16];
	    char_u  *sp;

	    ಆಯ್ಕೆ (s[2])
	    {
	    ಸ್ಥಿತಿ '0': ಸ್ಥಿತಿ '1': ಸ್ಥಿತಿ '2': ಸ್ಥಿತಿ '3': ಸ್ಥಿತಿ '4':
	    ಸ್ಥಿತಿ '5': ಸ್ಥಿತಿ '6': ಸ್ಥಿತಿ '7': ಸ್ಥಿತಿ '8': ಸ್ಥಿತಿ '9':
		ಆದರೆ (*(p = get_seq(ವಾದಗಳು, &argc, s)) != 'm')
		    ಹೋಗು notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		ಆದರೆ (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (ಪೂರ್ಣಾಂಕ)(p - s);
		    s = p;
		    ನಿಲ್ಲಿಸು;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		ಆದರೆ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		ಆದರೆ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		ಆದರೆ (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		ಆದರೆ (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(ವಾದಗಳು, &argc, p);

notsgr:
		arg1 = ವಾದಗಳು[0];
		arg2 = ವಾದಗಳು[1];
		ಆದರೆ (*p == 'm')
		{
		    ಆದರೆ (argc == 1 && ವಾದಗಳು[0] == 0)
			normvideo();
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 1)
		    {
			ಆದರೆ (USE_VTP)
			    textcolor((ಶಬ್ದ_2)arg1);
			ಇಲ್ಲದಿದ್ದರೆ
			    textattr((ಶಬ್ದ_2)arg1);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vtp_working)
			vtp_sgr_bulks(argc, ವಾದಗಳು);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 2 && *p == 'r')
		{
		    ಗಣ_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 2 && *p == 'R')
		{
		    ಗಣ_scroll_region_tb(arg1, arg2);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 2 && *p == 'V')
		{
		    ಗಣ_scroll_region_lr(arg1, arg2);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.ತುದಿ_2, g_coord.Y - arg1) + 1);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 1 && *p == 'b')
		{
		    textbackground((ಶಬ್ದ_2) arg1);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.ಬಲ_4, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 1 && *p == 'f')
		{
		    textcolor((ಶಬ್ದ_2) arg1);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 1 && *p == 'L')
		{
		    ಸೇರಿಸು_ಸಾಲುಗಳು(arg1);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 1 && *p == 'M')
		{
		    delete_ಸಾಲುಗಳು(arg1);
		}

		len -= (ಪೂರ್ಣಾಂಕ)(p - s);
		s = p + 1;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.ತುದಿ_2, g_coord.Y - 1) + 1);
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'B':
		visual_bell();
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'C':
		gotoxy(min(g_srScrollRegion.ಬಲ_4, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'E':
		termcap_ಬಹುಲಕ_ಕೊನೆ();
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'F':
		standout();
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'f':
		standend();
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'H':
		gotoxy(1, 1);
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'j':
		clear_ಗಮ್ಯ_ಕೊನೆ_ದ_display();
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'J':
		clear_screen();
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'K':
		clear_ಗಮ್ಯ_ಕೊನೆ_ದ_ಸಾಲು();
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'L':
		ಸೇರಿಸು_ಸಾಲುಗಳು(1);
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'M':
		delete_ಸಾಲುಗಳು(1);
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'S':
		termcap_ಬಹುಲಕ_ಆರಂಭ();
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'V':
		cursor_visible(TRUE);
		ಹೋಗು got3;

	    ಸ್ಥಿತಿ 'v':
		cursor_visible(FALSE);
		ಹೋಗು got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    ಆದರೆ (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, ಗಾತ್ರ(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    ಪೂರ್ಣಾಂಕ l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    ಆಗ (s + l < ಕೊನೆ && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    ಆದರೆ (s + l < ಕೊನೆ && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		ಆದರೆ (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    ಪೂರ್ಣಾಂಕ	l;

	    ಒಳ_dcs = true;
	    dcs_esc = false;
	    l = (ಪೂರ್ಣಾಂಕ)(skip_dcs(s + 2, ಕೊನೆ) - s);
	    ಆದರೆ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    ಆದರೆ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, ಗಾತ್ರ(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    ಆದರೆ (fdDump)
	fflush(fdDump);
# endif
}

#ಇಲ್ಲದಿದ್ದರೆ // FEAT_GUI_MSWIN && !VIMDLL
    ಶೂನ್ಯ
mch_write(
    char_u  *s ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ	    len ಬಳಸದ)
{
    // never used
}

#ಆದರೆ_ಅಂತ್ಯ


/*
 * Delay for "msec" milliseconds.
 */
    ಶೂನ್ಯ
mch_delay(
    ದೀರ್ಘ    msec,
    ಪೂರ್ಣಾಂಕ	    flags ಬಳಸದ)
{
#ಆದರೆ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((ಪೂರ್ಣಾಂಕ)msec);	    // never wait for input
#ಇಲ್ಲದಿದ್ದರೆ // Console
# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
    {
	Sleep((ಪೂರ್ಣಾಂಕ)msec);	    // never wait for input
	ಹಿಂತಿರುಗಿ;
    }
# endif
    ಆದರೆ (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	ಆದರೆ (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    ಪೂರ್ಣಾಂಕ towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    ಆಗ (msec > 0)
	    {
		mzvim_check_ಎಳೆಗಳು();
		ಆದರೆ (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    Sleep((ಪೂರ್ಣಾಂಕ)msec);
    ಇಲ್ಲದಿದ್ದರೆ
	WaitForChar(msec, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    ಪೂರ್ಣಾಂಕ
mch_remove(char_u *name)
{
    WCHAR	*wn;
    ಪೂರ್ಣಾಂಕ		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    ಆದರೆ (mch_isdir(name) && mch_is_symbolic_link(name))
	ಹಿಂತಿರುಗಿ mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ);

    wn = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (wn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    ಹಿಂತಿರುಗಿ n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    ಶೂನ್ಯ
mch_breakcheck(ಪೂರ್ಣಾಂಕ force ಬಳಸದ)
{
#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use)
# endif
	ಆದರೆ (g_fCtrlCPressed || g_fCBrkPressed)
	{
	    ctrl_break_was_pressed = g_fCBrkPressed;
	    g_fCtrlCPressed = g_fCBrkPressed = FALSE;
	    got_int = TRUE;
	}
#ಆದರೆ_ಅಂತ್ಯ
}

// physical RAM to leave for the OS
#ವ್ಯಾಖ್ಯಾನಿಸು WINNT_RESERVE_ಬೈಟುಗಳು     (256*1024*1024)

/*
 * How much main memory in KiB that can be used by VIM.
 */
    long_u
mch_ಒಟ್ಟು_mem(ಪೂರ್ಣಾಂಕ special ಬಳಸದ)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dw_ಉದ್ದಳತೆ = ಗಾತ್ರ(MEMORYSTATUSEX);
    Global_ಸ್ಮರಣೆ_ಸ್ಥಿತಿ_Ex(&ms);
    ಆದರೆ (ms.ullAvailVirtual < ms.ull_ಒಟ್ಟು_Phys)
    {
	// Process address space fits in physical RAM, use all of it.
	ಹಿಂತಿರುಗಿ (long_u)(ms.ullAvailVirtual / 1024);
    }
    ಆದರೆ (ms.ull_ಒಟ್ಟು_Phys <= WINNT_RESERVE_ಬೈಟುಗಳು)
    {
	// Catch old NT box or perverse hardware setup.
	ಹಿಂತಿರುಗಿ (long_u)((ms.ull_ಒಟ್ಟು_Phys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    ಹಿಂತಿರುಗಿ (long_u)((ms.ull_ಒಟ್ಟು_Phys - WINNT_RESERVE_ಬೈಟುಗಳು) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    ಪೂರ್ಣಾಂಕ
mch_expandpath(
    garray_T	*gap,
    char_u	*ಮಾರ್ಗ,
    ಪೂರ್ಣಾಂಕ		flags)		// EW_* flags
{
    ಹಿಂತಿರುಗಿ dos_expandpath(gap, ಮಾರ್ಗ, 0, flags, FALSE);
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
    ಪೂರ್ಣಾಂಕ
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	sz_ತಾತ್ಕಾಲಿಕ_File[MAX_ಮಾರ್ಗ_2 + 1];
    WCHAR	sz_New_ಮಾರ್ಗ[MAX_ಮಾರ್ಗ_2 + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    ಗಾಗಿ (p = q = wold; *p != L'\0'; ++p)
    {
	ಆಯ್ಕೆ (*p)
	{
	ಸ್ಥಿತಿ L'/':
	ಸ್ಥಿತಿ L'\\':
	ಸ್ಥಿತಿ L':':
	    ಆದರೆ (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)(p - q) < 8 || q[6] != L'~')
	ಹಿಂತಿರುಗಿ (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    ಆದರೆ (Get_ತುಂಬಿದ_ಮಾರ್ಗ_Name_W(wnew, MAX_ಮಾರ್ಗ_2, sz_New_ಮಾರ್ಗ, &p) == 0 || p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    ಆದರೆ (Get_ತಾತ್ಕಾಲಿಕ_File_Name_W(sz_New_ಮಾರ್ಗ, L"VIM", 0, sz_ತಾತ್ಕಾಲಿಕ_File) == 0)
	ಹಿಂತಿರುಗಿ -2;

    // blow the temp file away
    ಆದರೆ (!DeleteFileW(sz_ತಾತ್ಕಾಲಿಕ_File))
	ಹಿಂತಿರುಗಿ -3;

    // rename old file to the temp file
    ಆದರೆ (!MoveFileW(wold, sz_ತಾತ್ಕಾಲಿಕ_File))
	ಹಿಂತಿರುಗಿ -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    ಆದರೆ ((hf = CreateFileW(wold, GENERIC_WRITE, 0, ಖಾಲಿ, CREATE_NEW,
		    FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ, ಖಾಲಿ)) == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ -5;
    ಆದರೆ (!CloseHandle(hf))
	ಹಿಂತಿರುಗಿ -6;

    // rename the temp file to the new file
    ಆದರೆ (!MoveFileW(sz_ತಾತ್ಕಾಲಿಕ_File, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(ಶೂನ್ಯ)MoveFileW(sz_ತಾತ್ಕಾಲಿಕ_File, wold);
	ಹಿಂತಿರುಗಿ -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(sz_ತಾತ್ಕಾಲಿಕ_File);

    // finally, remove the empty old file
    ಆದರೆ (!DeleteFileW(wold))
	ಹಿಂತಿರುಗಿ -8;

    ಹಿಂತಿರುಗಿ 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    ಪೂರ್ಣಾಂಕ
mch_rename(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*pszOldFile,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*pszNewFile)
{
    WCHAR	*wold = ಖಾಲಿ;
    WCHAR	*wnew = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		retval = -1;

    wold = enc_ಗಮ್ಯ_utf16((char_u *)pszOldFile, ಖಾಲಿ);
    wnew = enc_ಗಮ್ಯ_utf16((char_u *)pszNewFile, ಖಾಲಿ);
    ಆದರೆ (wold != ಖಾಲಿ && wnew != ಖಾಲಿ)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    ಅಕ್ಷರ *
default_shell(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    ಪೂರ್ಣಾಂಕ
mch_access(ಅಕ್ಷರ *n, ಪೂರ್ಣಾಂಕ p)
{
    HANDLE	hFile;
    ಪೂರ್ಣಾಂಕ		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_ಗಮ್ಯ_utf16((char_u *)n, ಖಾಲಿ);
    ಆದರೆ (wn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (mch_isdir((char_u *)n))
    {
	WCHAR ತಾತ್ಕಾಲಿಕ_Name_W[MAX_ಮಾರ್ಗ_2 + 16] = L"";

	ಆದರೆ (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    ಪೂರ್ಣಾಂಕ			i;
	    WIN32_ಕಂಡುಹಿಡಿ_DATAW    d;

	    ಗಾಗಿ (i = 0; i < MAX_ಮಾರ್ಗ_2 && wn[i] != 0; ++i)
		ತಾತ್ಕಾಲಿಕ_Name_W[i] = wn[i];
	    ಆದರೆ (ತಾತ್ಕಾಲಿಕ_Name_W[i - 1] != '\\' && ತಾತ್ಕಾಲಿಕ_Name_W[i - 1] != '/')
		ತಾತ್ಕಾಲಿಕ_Name_W[i++] = '\\';
	    ತಾತ್ಕಾಲಿಕ_Name_W[i++] = '*';
	    ತಾತ್ಕಾಲಿಕ_Name_W[i++] = 0;

	    hFile = ಕಂಡುಹಿಡಿ_ಮೊದಲ_File_W(ತಾತ್ಕಾಲಿಕ_Name_W, &d);
	    ಆದರೆ (hFile == INVALID_HANDLE_ಮೌಲ್ಯ)
		ಹೋಗು getout;
	    ಇಲ್ಲದಿದ್ದರೆ
		(ಶೂನ್ಯ)ಕಂಡುಹಿಡಿ_Close(hFile);
	}

	ಆದರೆ (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    ಆದರೆ (!Get_ತಾತ್ಕಾಲಿಕ_File_Name_W(wn, L"VIM", 0, ತಾತ್ಕಾಲಿಕ_Name_W))
		ಹೋಗು getout;
	    ಇಲ್ಲದಿದ್ದರೆ
		DeleteFileW(ತಾತ್ಕಾಲಿಕ_Name_W);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_ಬಹುಲಕ = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_ಬಹುಲಕ = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_ಬಹುಲಕ, share_ಬಹುಲಕ,
					     ಖಾಲಿ, OPEN_EXISTING, 0, ಖಾಲಿ);
	ಆದರೆ (hFile == INVALID_HANDLE_ಮೌಲ್ಯ)
	    ಹೋಗು getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    ಪೂರ್ಣಾಂಕ
mch_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ)
{
    WCHAR	*wn;
    ಪೂರ್ಣಾಂಕ		f;

    wn = enc_ಗಮ್ಯ_utf16((char_u *)name, ಖಾಲಿ);
    ಆದರೆ (wn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    f = _wopen(wn, flags, ಬಹುಲಕ);
    vim_free(wn);
    ಹಿಂತಿರುಗಿ f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ)
{
    WCHAR	*wn, *wm;
    FILE	*f = ಖಾಲಿ;

#ಆದರೆ defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    ಅಕ್ಷರ new_ಬಹುಲಕ = ಬಹುಲಕ[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಬಹುಲಕ) - 1];
    ಪೂರ್ಣಾಂಕ old_ಬಹುಲಕ_2 = 0;

    _get_fmode(&old_ಬಹುಲಕ_2);
    ಆದರೆ (new_ಬಹುಲಕ == 't')
	ಗಣ_fmode(O_ಪಠ್ಯ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (new_ಬಹುಲಕ == 'b')
	ಗಣ_fmode(O_ದ್ವಿಮಾನ_2);
#ಆದರೆ_ಅಂತ್ಯ
    wn = enc_ಗಮ್ಯ_utf16((char_u *)name, ಖಾಲಿ);
    wm = enc_ಗಮ್ಯ_utf16((char_u *)ಬಹುಲಕ, ಖಾಲಿ);
    ಆದರೆ (wn != ಖಾಲಿ && wm != ಖಾಲಿ)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#ಆದರೆ defined(DEBUG) && _MSC_VER >= 1400
    ಆದರೆ (old_ಬಹುಲಕ_2 != 0)
	ಗಣ_fmode(old_ಬಹುಲಕ_2);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ f;
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
copy_substream(HANDLE sh, ಶೂನ್ಯ *context, WCHAR *ಗಮ್ಯ, WCHAR *substream, ದೀರ್ಘ len)
{
    HANDLE  h_ಗಮ್ಯ;
    WCHAR   *ಗಮ್ಯ_name;

    ಗಮ್ಯ_name = alloc((wcslen(ಗಮ್ಯ) + wcslen(substream) + 1) * ಗಾತ್ರ(WCHAR));
    ಆದರೆ (ಗಮ್ಯ_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    wcscpy(ಗಮ್ಯ_name, ಗಮ್ಯ);
    wcscat(ಗಮ್ಯ_name, substream);

    h_ಗಮ್ಯ = CreateFileW(ಗಮ್ಯ_name, GENERIC_WRITE, 0, ಖಾಲಿ, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ, ಖಾಲಿ);
    ಆದರೆ (h_ಗಮ್ಯ != INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	ದೀರ್ಘ	ಮುಗಿದ;
	DWORD	todo;
	DWORD	readcnt, written;
	ಅಕ್ಷರ	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	ಗಾಗಿ (ಮುಗಿದ = 0; ಮುಗಿದ < len; ಮುಗಿದ += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - ಮುಗಿದ) > ಗಾತ್ರ(buf) ? ಗಾತ್ರ(buf)
						       : (size_t)(len - ಮುಗಿದ));
	    ಆದರೆ (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(h_ಗಮ್ಯ, buf, todo, &written, ಖಾಲಿ)
		    || written != todo)
		ನಿಲ್ಲಿಸು;
	}
	CloseHandle(h_ಗಮ್ಯ);
    }

    ಬಿಡುಗಡೆ(ಗಮ್ಯ_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_infostreams(char_u *from, char_u *ಗಮ್ಯ)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_ಹರಿವು_ID	sid;
    ಪೂರ್ಣಾಂಕ			headersize;
    WCHAR		streamname[MAX_ಮಾರ್ಗ_2];
    DWORD		readcount;
    ಶೂನ್ಯ		*context = ಖಾಲಿ;
    DWORD		lo, hi;
    ಪೂರ್ಣಾಂಕ			len;

    // Convert the file names to wide characters.
    fromw = enc_ಗಮ್ಯ_utf16(from, ಖಾಲಿ);
    tow = enc_ಗಮ್ಯ_utf16(ಗಮ್ಯ, ಖಾಲಿ);
    ಆದರೆ (fromw != ಖಾಲಿ && tow != ಖಾಲಿ)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, ಖಾಲಿ,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, ಖಾಲಿ);
	ಆದರೆ (sh != INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    ಗಾಗಿ (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		Zero_ಸ್ಮರಣೆ(&sid, ಗಾತ್ರ(WIN32_ಹರಿವು_ID));
		headersize = (ಪೂರ್ಣಾಂಕ)((ಅಕ್ಷರ *)&sid.c_ಹರಿವು_Name - (ಅಕ್ಷರ *)&sid.dw_ಹರಿವು_Id);
		ಆದರೆ (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    ನಿಲ್ಲಿಸು;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		ಆದರೆ (sid.dw_ಹರಿವು_Name_ಗಾತ್ರ > 0)
		{
		    // Read the stream name.
		    ಆದರೆ (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dw_ಹರಿವು_Name_ಗಾತ್ರ,
					  &readcount, FALSE, FALSE, &context))
			ನಿಲ್ಲಿಸು;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / ಗಾತ್ರ(WCHAR);
		    streamname[len] = 0;
		    ಆದರೆ (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (ದೀರ್ಘ)sid.ಗಾತ್ರ_3.u.ಕೆಳಗಿನ_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(ಶೂನ್ಯ)BackupSeek(sh, sid.ಗಾತ್ರ_3.u.ಕೆಳಗಿನ_Part, sid.ಗಾತ್ರ_3.u.ಮೇಲಿನ_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (ಶೂನ್ಯ)BackupRead(sh, ಖಾಲಿ, 0, &readcount, TRUE, FALSE, &context);

	    CloseHandle(sh);
	}
    }
    vim_free(fromw);
    vim_free(tow);
}

/*
 * ntdll.dll definitions
 */
#ವ್ಯಾಖ್ಯಾನಿಸು FileEaInformation   7
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಸ್ಥಿತಿ_SUCCESS
# define ಸ್ಥಿತಿ_SUCCESS	    ((NTSTATUS) 0x00000000L)
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ FILE_ತುಂಬಿದ_EA_INFORMATION {
    ULONG  ಮುಂದಿನ_ನಮೂದು_ಸ್ಥಾನಾಂತರ;
    UCHAR  Flags;
    UCHAR  Ea_Name_ಉದ್ದಳತೆ;
    USHORT Ea_ಮೌಲ್ಯ_ಉದ್ದಳತೆ;
    CHAR   EaName[1];
} FILE_ತುಂಬಿದ_EA_INFORMATION_2, *PFILE_ತುಂಬಿದ_EA_INFORMATION;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _FILE_EA_INFORMATION_ {
    ULONG Ea_ಗಾತ್ರ;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_ಮುಖವಾಡ DesiredAccess,
	POBJECT_ಗುಣಲಕ್ಷಣಗಳು Object_ಗುಣಲಕ್ಷಣಗಳು,
	PIO_ಸ್ಥಿತಿ_ಖಂಡ Io_ಸ್ಥಿತಿ_ಖಂಡ,
	ULONG ShareAccess,
	ULONG OpenOptions);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ NTSTATUS (NTAPI *Pfn_Nt_ಗಣ_Ea_File)(
	HANDLE		    FileHandle,
	PIO_ಸ್ಥಿತಿ_ಖಂಡ    Io_ಸ್ಥಿತಿ_ಖಂಡ,
	PVOID		    ಬಫರ್_2,
	ULONG		    ಉದ್ದಳತೆ_2);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_ಸ್ಥಿತಿ_ಖಂಡ Io_ಸ್ಥಿತಿ_ಖಂಡ,
	PVOID ಬಫರ್_2,
	ULONG ಉದ್ದಳತೆ_2,
	BOOLEAN Return_Single_ನಮೂದು,
	PVOID EaList,
	ULONG Ea_List_ಉದ್ದಳತೆ,
	PULONG Ea_ಸೂಚ್ಯಂಕ,
	BOOLEAN Restart_ಪರಿಶೀಲನೆ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_ಸ್ಥಿತಿ_ಖಂಡ	Io_ಸ್ಥಿತಿ_ಖಂಡ,
	PVOID			FileInformation,
	ULONG			ಉದ್ದಳತೆ_2,
	FILE_INFORMATION_CLASS FileInformationClass);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR ಮೂಲ_String);

PfnNtOpenFile pNtOpenFile = ಖಾಲಿ;
PfnNtClose pNtClose = ಖಾಲಿ;
Pfn_Nt_ಗಣ_Ea_File p_Nt_ಗಣ_Ea_File = ಖಾಲಿ;
PfnNtQueryEaFile pNtQueryEaFile = ಖಾಲಿ;
PfnNtQueryInformationFile pNtQueryInformationFile = ಖಾಲಿ;
PfnRtlInitUnicodeString pRtlInitUnicodeString = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Load ntdll.dll functions.
 */
    ಸ್ಥಿರ BOOL
ತುಂಬು_ntdll(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	loaded = -1;

    ಆದರೆ (loaded != -1)
	ಹಿಂತಿರುಗಿ (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    ಆದರೆ (hNtdll != ಖಾಲಿ)
    {
	pNtOpenFile = (PfnNtOpenFile) Get_Proc_ವಿಳಾಸ(hNtdll, "NtOpenFile");
	pNtClose = (PfnNtClose) Get_Proc_ವಿಳಾಸ(hNtdll, "NtClose");
	p_Nt_ಗಣ_Ea_File = (Pfn_Nt_ಗಣ_Ea_File)
	    Get_Proc_ವಿಳಾಸ(hNtdll, "NtSetEaFile");
	pNtQueryEaFile = (PfnNtQueryEaFile)
	    Get_Proc_ವಿಳಾಸ(hNtdll, "NtQueryEaFile");
	pNtQueryInformationFile = (PfnNtQueryInformationFile)
	    Get_Proc_ವಿಳಾಸ(hNtdll, "NtQueryInformationFile");
	pRtlInitUnicodeString = (PfnRtlInitUnicodeString)
	    Get_Proc_ವಿಳಾಸ(hNtdll, "RtlInitUnicodeString");
    }
    ಆದರೆ (pNtOpenFile == ಖಾಲಿ
	    || pNtClose == ಖಾಲಿ
	    || p_Nt_ಗಣ_Ea_File == ಖಾಲಿ
	    || pNtQueryEaFile == ಖಾಲಿ
	    || pNtQueryInformationFile == ಖಾಲಿ
	    || pRtlInitUnicodeString == ಖಾಲಿ)
	loaded = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
	loaded = TRUE;
    ಹಿಂತಿರುಗಿ (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_extattr(char_u *from, char_u *ಗಮ್ಯ)
{
    char_u		    *fromf = ಖಾಲಿ;
    char_u		    *tof = ಖಾಲಿ;
    WCHAR		    *fromw = ಖಾಲಿ;
    WCHAR		    *tow = ಖಾಲಿ;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_ಗುಣಲಕ್ಷಣಗಳು	    oa;
    IO_ಸ್ಥಿತಿ_ಖಂಡ	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    ಶೂನ್ಯ		    *ea = ಖಾಲಿ;

    ಆದರೆ (!ತುಂಬು_ntdll())
	ಹಿಂತಿರುಗಿ;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(ಗಮ್ಯ) + 5);
    ಆದರೆ (fromf == ಖಾಲಿ || tof == ಖಾಲಿ)
	ಹೋಗು theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), ಗಮ್ಯ);

    // Convert the names to wide characters.
    fromw = enc_ಗಮ್ಯ_utf16(fromf, ಖಾಲಿ);
    tow = enc_ಗಮ್ಯ_utf16(tof, ಖಾಲಿ);
    ಆದರೆ (fromw == ಖಾಲಿ || tow == ಖಾಲಿ)
	ಹೋಗು theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    Initialize_Object_ಗುಣಲಕ್ಷಣಗಳು(&oa, &u, 0, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != ಸ್ಥಿತಿ_SUCCESS)
	ಹೋಗು theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, ಗಾತ್ರ(eainfo),
	    FileEaInformation);
    ಆದರೆ (eainfo.Ea_ಗಾತ್ರ != 0)
    {
	ea = alloc(eainfo.Ea_ಗಾತ್ರ);
	ಆದರೆ (ea != ಖಾಲಿ)
	{
	    ಆದರೆ (pNtQueryEaFile(h, &iosb, ea, eainfo.Ea_ಗಾತ್ರ, FALSE,
			ಖಾಲಿ, 0, ಖಾಲಿ, TRUE) != ಸ್ಥಿತಿ_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    ಆದರೆ (ea != ಖಾಲಿ)
    {
	pRtlInitUnicodeString(&u, tow);
	Initialize_Object_ಗುಣಲಕ್ಷಣಗಳು(&oa, &u, 0, ಖಾಲಿ, ಖಾಲಿ);
	ಆದರೆ (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != ಸ್ಥಿತಿ_SUCCESS)
	    ಹೋಗು theend;

	p_Nt_ಗಣ_Ea_File(h, &iosb, ea, eainfo.Ea_ಗಾತ್ರ);
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
    ಪೂರ್ಣಾಂಕ
mch_copy_file_attribute(char_u *from, char_u *ಗಮ್ಯ)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, ಗಮ್ಯ);
    copy_extattr(from, ಗಮ್ಯ);
    ಹಿಂತಿರುಗಿ 0;
}


/*
 * The command line arguments in UTF-16
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	n_ವಾದಗಳು_W = 0;
ಸ್ಥಿರ LPWSTR	*ArglistW = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	global_argc = 0;
ಸ್ಥಿರ ಅಕ್ಷರ	**global_argv;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಬಳಸಿದ_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	*ಬಳಸಿದ_file_indexes = ಖಾಲಿ; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಬಳಸಿದ_file_ಎಣಿಕೆ = 0;	// nr of entries in used_file_indexes
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಬಳಸಿದ_file_literal = FALSE;  // take file names literally
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಬಳಸಿದ_file_ತುಂಬಿದ_ಮಾರ್ಗ = FALSE;  // file name was full path
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಬಳಸಿದ_file_diff_ಬಹುಲಕ = FALSE;  // file name was with diff mode
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಬಳಸಿದ_alist_ಎಣಿಕೆ = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    ಪೂರ್ಣಾಂಕ
get_cmd_ವಾದಗಳು_W(ಅಕ್ಷರ ***argvp)
{
    ಅಕ್ಷರ	**argv = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		argc = 0;
    ಪೂರ್ಣಾಂಕ		i;

    free_cmd_ವಾದಗಳು_W();
    ArglistW = Command_ಸಾಲು_ಗಮ್ಯ_Argv_W(Get_Command_ಸಾಲು_W(), &n_ವಾದಗಳು_W);
    ಆದರೆ (ArglistW != ಖಾಲಿ)
    {
	argv = alloc((n_ವಾದಗಳು_W + 1) * ಗಾತ್ರ(ಅಕ್ಷರ *));
	ಆದರೆ (argv != ಖಾಲಿ)
	{
	    argc = n_ವಾದಗಳು_W;
	    argv[argc] = ಖಾಲಿ;
	    ಗಾಗಿ (i = 0; i < argc; ++i)
	    {
		ಪೂರ್ಣಾಂಕ	len;

		// Convert each Unicode argument to UTF-8.
		Wide_Char_ಗಮ್ಯ_Multi_Byte_alloc(CP_UTF8, 0,
				ArglistW[i], (ಪೂರ್ಣಾಂಕ)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		ಆದರೆ (argv[i] == ಖಾಲಿ)
		{
		    // Out of memory, clear everything.
		    ಆಗ (i > 0)
			ಬಿಡುಗಡೆ(argv[--i]);
		    ಬಿಡುಗಡೆ(argv);
		    argv = ಖಾಲಿ;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    ಆದರೆ (argc > 0)
    {
	ಆದರೆ (ಬಳಸಿದ_file_indexes != ಖಾಲಿ)
	    ಬಿಡುಗಡೆ(ಬಳಸಿದ_file_indexes);
	ಬಳಸಿದ_file_indexes = alloc(argc * ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
    }

    ಆದರೆ (argvp != ಖಾಲಿ)
	*argvp = argv;
    ಹಿಂತಿರುಗಿ argc;
}

    ಶೂನ್ಯ
free_cmd_ವಾದಗಳು_W(ಶೂನ್ಯ)
{
    ಆದರೆ (ArglistW == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    GlobalFree(ArglistW);
    ArglistW = ಖಾಲಿ;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    ಶೂನ್ಯ
ಬಳಸಿದ_file_arg(ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ literal, ಪೂರ್ಣಾಂಕ ತುಂಬಿದ_ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ diff_ಬಹುಲಕ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (ಬಳಸಿದ_file_indexes == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (i = ಬಳಸಿದ_file_argc + 1; i < global_argc; ++i)
	ಆದರೆ (STRCMP(global_argv[i], name) == 0)
	{
	    ಬಳಸಿದ_file_argc = i;
	    ಬಳಸಿದ_file_indexes[ಬಳಸಿದ_file_ಎಣಿಕೆ++] = i;
	    ನಿಲ್ಲಿಸು;
	}
    ಬಳಸಿದ_file_literal = literal;
    ಬಳಸಿದ_file_ತುಂಬಿದ_ಮಾರ್ಗ = ತುಂಬಿದ_ಮಾರ್ಗ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಬಳಸಿದ_file_diff_ಬಹುಲಕ = diff_ಬಹುಲಕ;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Remember the length of the argument list as it was.  If it changes then we
 * leave it alone when 'encoding' is set.
 */
    ಶೂನ್ಯ
ಗಣ_alist_ಎಣಿಕೆ(ಶೂನ್ಯ)
{
    ಬಳಸಿದ_alist_ಎಣಿಕೆ = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    ಶೂನ್ಯ
fix_arg_enc(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		idx;
    char_u	*str;
    ಪೂರ್ಣಾಂಕ		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    ಆದರೆ (global_argc != n_ವಾದಗಳು_W
	    || ArglistW == ಖಾಲಿ
	    || ಬಳಸಿದ_file_indexes == ಖಾಲಿ
	    || ಬಳಸಿದ_file_ಎಣಿಕೆ == 0
	    || ಬಳಸಿದ_alist_ಎಣಿಕೆ != GARGCOUNT)
	ಹಿಂತಿರುಗಿ;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, GARGCOUNT);
    ಆದರೆ (fnum_list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;		// out of memory
    ಗಾಗಿ (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    ಆದರೆ (ga_grow(&global_alist.al_ga, ಬಳಸಿದ_file_ಎಣಿಕೆ) == FAIL)
	ಹಿಂತಿರುಗಿ;		// out of memory

    ಗಾಗಿ (i = 0; i < ಬಳಸಿದ_file_ಎಣಿಕೆ; ++i)
    {
	idx = ಬಳಸಿದ_file_indexes[i];
	str = utf16_ಗಮ್ಯ_enc(ArglistW[idx], ಖಾಲಿ);
	ಆದರೆ (str != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ literal = ಬಳಸಿದ_file_literal;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    ಆದರೆ (ಬಳಸಿದ_file_diff_ಬಹುಲಕ && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *ಬಾಲ;
		string_T    ret;

		ಬಾಲ = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), ಬಾಲ, STRLEN(ಬಾಲ), TRUE, &ret);
		ಆದರೆ (ret.string != ಖಾಲಿ)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    ಆದರೆ (ಬಳಸಿದ_file_literal)
		buf_ಗಣ_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    ಆದರೆ (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		ಆದರೆ (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_ಕೂಡಿಸು(&global_alist, str, literal ? 2 : 0);
	}
    }

    ಆದರೆ (!ಬಳಸಿದ_file_literal)
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
	alist_expand(fnum_list, ಬಳಸಿದ_alist_ಎಣಿಕೆ);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    ಆದರೆ (curwin->w_arg_idx == 0 && curbuf->b_fname == ಖಾಲಿ)
    {
	do_cmdline_cmd((char_u *)":rewind");
	ಆದರೆ (GARGCOUNT == 1 && ಬಳಸಿದ_file_ತುಂಬಿದ_ಮಾರ್ಗ
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    ಕೊನೆಯ_chdir_reason = "drop";
    }

    ಗಣ_alist_ಎಣಿಕೆ();
}

    ಪೂರ್ಣಾಂಕ
mch_setenv(ಅಕ್ಷರ *var, ಅಕ್ಷರ *ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ x ಬಳಸದ)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(ಮೌಲ್ಯ) + 2);
    ಆದರೆ (envbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    sprintf((ಅಕ್ಷರ *)envbuf, "%s=%s", var, ಮೌಲ್ಯ);

    p = enc_ಗಮ್ಯ_utf16(envbuf, ಖಾಲಿ);

    vim_free(envbuf);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    _wputenv(p);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ libintl_wputenv
    libintl_wputenv(p);
#ಆದರೆ_ಅಂತ್ಯ
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Support for 256 colors and 24-bit colors was added in Windows 10
 * version 1703 (Creators update).
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VTP_ಮೊದಲ_SUPPORT_BUILD	    MAKE_VER(10, 0, 15063)

/*
 * Support for pseudo-console (ConPTY) was added in windows 10
 * version 1809 (October 2018 update).
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CONPTY_ಮೊದಲ_SUPPORT_BUILD  MAKE_VER(10, 0, 17763)

/*
 * ConPTY differences between versions, need different logic.
 * version 1903 (May 2019 update).
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CONPTY_1903_BUILD	    MAKE_VER(10, 0, 18362)

/*
 * version 1909 (November 2019 update).
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CONPTY_1909_BUILD	    MAKE_VER(10, 0, 18363)

/*
 * Stay ahead of the next update, and when it's done, fix this.
 * version ? (2020 update, temporarily use the build number of insider preview)
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CONPTY_ಮುಂದಿನ_UPDATE_BUILD    MAKE_VER(10, 0, 19587)

/*
 * Confirm until this version.  Also the logic changes.
 * insider preview.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CONPTY_INSIDER_BUILD	    MAKE_VER(10, 0, 18995)

/*
 * Make conpty default on Windows 11
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CONPTY_STABLE_BUILD	    MAKE_VER(10, 0, 22000)
// Notes:
// Win 10 22H2 Final is build 19045, its conpty is widely used.
// Strangely, 19045 is newer but is a lower build number than the 2020 insider
// preview which had a build 19587.  And, not sure how stable that was?
// Win Server 2022 (May 10, 2022) is build 20348, its conpty is widely used.
// Win 11 starts from build 22000, even though the major version says 10!

    ಸ್ಥಿರ ಶೂನ್ಯ
vtp_flag_init(ಶೂನ್ಯ)
{
#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   ಬಹುಲಕ;
    HANDLE  ಹೊರಹೋಗು;

# ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use)
# endif
    {
	ಹೊರಹೋಗು = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_ಆವೃತ್ತಿ >= VTP_ಮೊದಲ_SUPPORT_BUILD) ? 1 : 0;
	Get_Console_ಬಹುಲಕ(ಹೊರಹೋಗು, &ಬಹುಲಕ);
	ಬಹುಲಕ |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	ಆದರೆ (ಗಣ_Console_ಬಹುಲಕ(ಹೊರಹೋಗು, ಬಹುಲಕ) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_ಬಫರ್ = win_ಆವೃತ್ತಿ >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (win_ಆವೃತ್ತಿ >= CONPTY_ಮೊದಲ_SUPPORT_BUILD)
	conpty_working = 1;
    ಆದರೆ (win_ಆವೃತ್ತಿ >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    ಆದರೆ (win_ಆವೃತ್ತಿ <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    ಆದರೆ (win_ಆವೃತ್ತಿ <= CONPTY_1909_BUILD)
	conpty_type = 2;
    ಆದರೆ (win_ಆವೃತ್ತಿ <= CONPTY_1903_BUILD)
	conpty_type = 2;
    ಆದರೆ (win_ಆವೃತ್ತಿ < CONPTY_ಮೊದಲ_SUPPORT_BUILD)
	conpty_type = 1;

    ಆದರೆ (win_ಆವೃತ್ತಿ >= CONPTY_ಮುಂದಿನ_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ಸ್ಥಿರ ಶೂನ್ಯ
vtp_init(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_ಬಫರ್_INFOEX csbi;
    csbi.cb_ಗಾತ್ರ = ಗಾತ್ರ(csbi);
    Get_Console_Screen_ಬಫರ್_Info_Ex(g_h_Con_ಹೊರಹೋಗು, &csbi);
    save_console_bg_rgb = (guicolor_T)csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_bg];
    save_console_fg_rgb = (guicolor_T)csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_fg];
    store_console_bg_rgb = save_console_bg_rgb;
    store_console_fg_rgb = save_console_fg_rgb;

    COLORREF bg;
    bg = (COLORREF)csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_bg];
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);
    default_console_color_bg = bg;

    COLORREF fg;
    fg = (COLORREF)csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_fg];
    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    default_console_color_fg = fg;
# endif
    ಗಣ_console_color_rgb();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vtp_exit(ಶೂನ್ಯ)
{
    restore_console_color_rgb();
}

    ಪೂರ್ಣಾಂಕ
vtp_printf(
    ಅಕ್ಷರ *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   ಫಲಿತಾಂಶ;
    ಪೂರ್ಣಾಂಕ	    len;

    ಆದರೆ (silent_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ 0;

    va_start(list, format);
    len = vim_vsnprintf((ಅಕ್ಷರ *)buf, 100, (ಅಕ್ಷರ *)format, list);
    va_end(list);
    WriteConsoleA(g_h_Con_ಹೊರಹೋಗು, buf, (DWORD)len, &ಫಲಿತಾಂಶ, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vtp_sgr_bulk(
    ಪೂರ್ಣಾಂಕ arg)
{
    ಪೂರ್ಣಾಂಕ ವಾದಗಳು[1];

    ವಾದಗಳು[0] = arg;
    vtp_sgr_bulks(1, ವಾದಗಳು);
}

# define FAST256(x) \
    ಆದರೆ ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    ಸ್ಥಿತಿ x: \
	FAST256(newargs[x - 1]);

    ಸ್ಥಿರ ಶೂನ್ಯ
vtp_sgr_bulks(
    ಪೂರ್ಣಾಂಕ argc,
    ಪೂರ್ಣಾಂಕ *ವಾದಗಳು)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    ಒಳ, ಹೊರಹೋಗು;
    ಪೂರ್ಣಾಂಕ	    newargs[16];
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sgrfgr = -1, sgrfgg, sgrfgb;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sgrbgr = -1, sgrbgg, sgrbgb;

    ಆದರೆ (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	ಹಿಂತಿರುಗಿ;
    }

    ಒಳ = ಹೊರಹೋಗು = 0;
    ಆಗ (ಒಳ < argc)
    {
	ಪೂರ್ಣಾಂಕ s = ವಾದಗಳು[ಒಳ];
	ಪೂರ್ಣಾಂಕ copylen = 1;

	ಆದರೆ (s == 38)
	{
	    ಆದರೆ (argc - ಒಳ >= 5 && ವಾದಗಳು[ಒಳ + 1] == 2)
	    {
		ಆದರೆ (sgrfgr == ವಾದಗಳು[ಒಳ + 2] && sgrfgg == ವಾದಗಳು[ಒಳ + 3]
						     && sgrfgb == ವಾದಗಳು[ಒಳ + 4])
		{
		    ಒಳ += 5;
		    copylen = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    sgrfgr = ವಾದಗಳು[ಒಳ + 2];
		    sgrfgg = ವಾದಗಳು[ಒಳ + 3];
		    sgrfgb = ವಾದಗಳು[ಒಳ + 4];
		    copylen = 5;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc - ಒಳ >= 3 && ವಾದಗಳು[ಒಳ + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s == 48)
	{
	    ಆದರೆ (argc - ಒಳ >= 5 && ವಾದಗಳು[ಒಳ + 1] == 2)
	    {
		ಆದರೆ (sgrbgr == ವಾದಗಳು[ಒಳ + 2] && sgrbgg == ವಾದಗಳು[ಒಳ + 3]
						     && sgrbgb == ವಾದಗಳು[ಒಳ + 4])
		{
		    ಒಳ += 5;
		    copylen = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    sgrbgr = ವಾದಗಳು[ಒಳ + 2];
		    sgrbgg = ವಾದಗಳು[ಒಳ + 3];
		    sgrbgb = ವಾದಗಳು[ಒಳ + 4];
		    copylen = 5;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc - ಒಳ >= 3 && ವಾದಗಳು[ಒಳ + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (30 <= s && s <= 39)
	    sgrfgr = -1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (90 <= s && s <= 97)
	    sgrfgr = -1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (40 <= s && s <= 49)
	    sgrbgr = -1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (100 <= s && s <= 107)
	    sgrbgr = -1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s == 0)
	    sgrfgr = sgrbgr = -1;

	ಆಗ (copylen--)
	    newargs[ಹೊರಹೋಗು++] = ವಾದಗಳು[ಒಳ++];
    }

    p = &buf[ಗಾತ್ರ(buf) - 1];
    *p-- = 'm';

    ಆಯ್ಕೆ (ಹೊರಹೋಗು)
    {
	ಪೂರ್ಣಾಂಕ	n, m;
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
	WriteConsoleA(g_h_Con_ಹೊರಹೋಗು, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, ಖಾಲಿ);
    ಪೂರ್ವನಿಯೋಜಿತ:
	ನಿಲ್ಲಿಸು;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
wt_init(ಶೂನ್ಯ)
{
    wt_working = mch_getenv("WT_SESSION") != ಖಾಲಿ;
}

# ifdef FEAT_TERMGUICOLORS
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ctermtoxterm(
    ಪೂರ್ಣಾಂಕ cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    ಹಿಂತಿರುಗಿ (((ಪೂರ್ಣಾಂಕ)r << 16) | ((ಪೂರ್ಣಾಂಕ)g << 8) | (ಪೂರ್ಣಾಂಕ)b);
}
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_console_color_rgb(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_ಬಫರ್_INFOEX csbi;
    guicolor_T	fg, bg;
    ಪೂರ್ಣಾಂಕ		ctermfg, ctermbg;

    ಆದರೆ (!vtp_working)
	ಹಿಂತಿರುಗಿ;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    ಆದರೆ (p_tgc || t_colors >= 256)
    {
	ಪದ_fg_rgb_color(fg);
	ಪದ_bg_rgb_color(bg);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (use_alternate_screen_ಬಫರ್)
	ಹಿಂತಿರುಗಿ;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cb_ಗಾತ್ರ = ಗಾತ್ರ(csbi);
    Get_Console_Screen_ಬಫರ್_Info_Ex(g_h_Con_ಹೊರಹೋಗು, &csbi);

    csbi.cb_ಗಾತ್ರ = ಗಾತ್ರ(csbi);
    csbi.sr_ಕಿಟಕಿ.ಬಲ_4 += 1;
    csbi.sr_ಕಿಟಕಿ.Bottom += 1;
    store_console_bg_rgb = csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_bg];
    store_console_fg_rgb = csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_fg];
    csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_bg] = (COLORREF)bg;
    csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_fg] = (COLORREF)fg;
    ಗಣ_Console_Screen_ಬಫರ್_Info_Ex(g_h_Con_ಹೊರಹೋಗು, &csbi);
# endif
}

# ಆದರೆ defined(FEAT_TERMGUICOLORS)
    ಶೂನ್ಯ
get_default_console_color(
    ಪೂರ್ಣಾಂಕ *cterm_fg,
    ಪೂರ್ಣಾಂಕ *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    ಪೂರ್ಣಾಂಕ id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    ಪೂರ್ಣಾಂಕ ctermfg = 0;
    ಪೂರ್ಣಾಂಕ ctermbg = 0;
    ಪೂರ್ಣಾಂಕ dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    ಆದರೆ (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    ಆದರೆ (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	ಆದರೆ (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	ಆದರೆ (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	ಇಲ್ಲದಿದ್ದರೆ
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_ಸಾಮಾನ್ಯ_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    ಆದರೆ (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	ಆದರೆ (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	ಆದರೆ (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	ಇಲ್ಲದಿದ್ದರೆ
	    guibg = USE_WT ? INVALCOLOR : default_console_color_bg;
	cterm_ಸಾಮಾನ್ಯ_bg_gui_color = guibg;
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
    ಆದರೆ (use_alternate_screen_ಬಫರ್)
	ಹಿಂತಿರುಗಿ;

    CONSOLE_SCREEN_ಬಫರ್_INFOEX csbi;

    csbi.cb_ಗಾತ್ರ = ಗಾತ್ರ(csbi);
    Get_Console_Screen_ಬಫರ್_Info_Ex(g_h_Con_ಹೊರಹೋಗು, &csbi);

    csbi.cb_ಗಾತ್ರ = ಗಾತ್ರ(csbi);
    csbi.sr_ಕಿಟಕಿ.ಬಲ_4 += 1;
    csbi.sr_ಕಿಟಕಿ.Bottom += 1;
    csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_bg] = (COLORREF)store_console_bg_rgb;
    csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_fg] = (COLORREF)store_console_fg_rgb;
    ಗಣ_Console_Screen_ಬಫರ್_Info_Ex(g_h_Con_ಹೊರಹೋಗು, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_console_color_rgb(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMGUICOLORS
    ಆದರೆ (use_alternate_screen_ಬಫರ್)
	ಹಿಂತಿರುಗಿ;

    CONSOLE_SCREEN_ಬಫರ್_INFOEX csbi;

    csbi.cb_ಗಾತ್ರ = ಗಾತ್ರ(csbi);
    Get_Console_Screen_ಬಫರ್_Info_Ex(g_h_Con_ಹೊರಹೋಗು, &csbi);

    csbi.cb_ಗಾತ್ರ = ಗಾತ್ರ(csbi);
    csbi.sr_ಕಿಟಕಿ.ಬಲ_4 += 1;
    csbi.sr_ಕಿಟಕಿ.Bottom += 1;
    csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_bg] = (COLORREF)save_console_bg_rgb;
    csbi.Color_ಕೋಷ್ಟಕ[g_color_ಸೂಚ್ಯಂಕ_fg] = (COLORREF)save_console_fg_rgb;
    ಗಣ_Console_Screen_ಬಫರ್_Info_Ex(g_h_Con_ಹೊರಹೋಗು, &csbi);
# endif
}

    ಶೂನ್ಯ
control_console_color_rgb(ಶೂನ್ಯ)
{
    ಆದರೆ (vtp_working)
	ಗಣ_console_color_rgb();
    ಇಲ್ಲದಿದ್ದರೆ
	reset_console_color_rgb();
}

    ಪೂರ್ಣಾಂಕ
use_vtp(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ USE_VTP;
}

    ಪೂರ್ಣಾಂಕ
is_ಪದ_win32(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ T_NAME != ಖಾಲಿ && STRCMP(T_NAME, "win32") == 0;
}

    ಪೂರ್ಣಾಂಕ
has_vtp_working(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ vtp_working;
}

#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
has_conpty_working(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ conpty_working;
}

    ಪೂರ್ಣಾಂಕ
get_conpty_type(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ conpty_type;
}

    ಪೂರ್ಣಾಂಕ
is_conpty_stable(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ conpty_stable;
}

    ಪೂರ್ಣಾಂಕ
get_conpty_fix_type(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ conpty_fix_type;
}

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ಶೂನ್ಯ
resize_console_buf(ಶೂನ್ಯ)
{
    ಆದರೆ (use_alternate_screen_ಬಫರ್)
	ಹಿಂತಿರುಗಿ;

    CONSOLE_SCREEN_ಬಫರ್_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    ಆದರೆ (!Get_Console_Screen_ಬಫರ್_Info(g_h_Con_ಹೊರಹೋಗು, &csbi))
	ಹಿಂತಿರುಗಿ;

    coord.X = SRWIDTH(csbi.sr_ಕಿಟಕಿ);
    coord.Y = SRHEIGHT(csbi.sr_ಕಿಟಕಿ);
    ಗಣ_Console_Screen_ಬಫರ್_ಗಾತ್ರ(g_h_Con_ಹೊರಹೋಗು, coord);

    newsize.ಎಡ_4 = 0;
    newsize.ತುದಿ_2 = 0;
    newsize.ಬಲ_4 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    ಗಣ_Console_ಕಿಟಕಿ_Info(g_h_Con_ಹೊರಹೋಗು, TRUE, &newsize);

    ಗಣ_Console_Screen_ಬಫರ್_ಗಾತ್ರ(g_h_Con_ಹೊರಹೋಗು, coord);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಅಕ್ಷರ *
GetWin32Error(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಅಕ್ಷರ	*oldmsg = ಖಾಲಿ;
    ಅಕ್ಷರ	*acp_ಸಂದೇಶ = ಖಾಲಿ;
    DWORD	acp_len;
    ಅಕ್ಷರ	*enc_ಸಂದೇಶ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		enc_len = 0;

    // get formatted message from OS
    acp_len = Format_ಸಂದೇಶ(
	    FORMAT_ಸಂದೇಶ_ALLOCATE_ಬಫರ್|FORMAT_ಸಂದೇಶ_FROM_SYSTEM,
	    ಖಾಲಿ, Get_ಕೊನೆಯ_Error(), 0, (LPSTR)&acp_ಸಂದೇಶ, 0, ಖಾಲಿ);
    ಆದರೆ (acp_len == 0 || acp_ಸಂದೇಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // clean oldmsg if remained.
    ಆದರೆ (oldmsg != ಖಾಲಿ)
    {
	vim_free(oldmsg);
	oldmsg = ಖಾಲಿ;
    }

    acp_ಗಮ್ಯ_enc((char_u *)acp_ಸಂದೇಶ, (ಪೂರ್ಣಾಂಕ)acp_len, (char_u **)&enc_ಸಂದೇಶ, &enc_len);
    LocalFree(acp_ಸಂದೇಶ);
    ಆದರೆ (enc_ಸಂದೇಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // remove trailing \r\n
    ಅಕ್ಷರ *pcrlf = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(enc_ಸಂದೇಶ, "\r\n");
    ಆದರೆ (pcrlf != ಖಾಲಿ)
	*pcrlf = NUL;

    oldmsg = enc_ಸಂದೇಶ;
    ಹಿಂತಿರುಗಿ enc_ಸಂದೇಶ;
}

#ಆದರೆ defined(FEAT_RELTIME)
ಸ್ಥಿರ HANDLE   timer_handle;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ      timer_ಸಕ್ರಿಯ = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
ಸ್ಥಿರ sig_atomic_t timeout_flags[2];
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    timeout_flag_idx = 0;
ಸ್ಥಿರ sig_atomic_t *timeout_flag = &timeout_flags[0];


    ಸ್ಥಿರ ಶೂನ್ಯ CALLBACK
ಗಣ_flag(ಶೂನ್ಯ *param, BOOLEAN unused2 ಬಳಸದ)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    ಶೂನ್ಯ
stop_timeout(ಶೂನ್ಯ)
{
    ಆದರೆ (timer_ಸಕ್ರಿಯ)
    {
	BOOL ret = Delete_Timer_ಸರತಿ_Timer(ಖಾಲಿ, timer_handle, ಖಾಲಿ);
	timer_ಸಕ್ರಿಯ = FALSE;
	ಆದರೆ (!ret && Get_ಕೊನೆಯ_Error() != ERROR_IO_PENDING)
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
    ಅಸ್ಥಿರ sig_atomic_t *
ಆರಂಭ_timeout(ದೀರ್ಘ msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = Create_Timer_ಸರತಿ_Timer(
	    &timer_handle, ಖಾಲಿ, ಗಣ_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    ಆದರೆ (!ret)
    {
	semsg(_(e_could_not_ಗಣ_timeout_str), GetWin32Error());
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_ಸಕ್ರಿಯ = TRUE;
	*timeout_flag = FALSE;
    }
    ಹಿಂತಿರುಗಿ timeout_flag;
}
#ಆದರೆ_ಅಂತ್ಯ
