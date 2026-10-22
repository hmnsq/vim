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

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_MZSCHEME
# include "if_mzsch.h"
#అయితే_ముగింపు

#అన్తర్భావ <sys/types.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <limits.h>

#అన్తర్భావ <process.h>
#అన్తర్భావ <winternl.h>
#అన్తర్భావ <direct.h>

#ఒకవేళ !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_పని_CHANNEL
# include <tlhelp32.h>
#అయితే_ముగింపు

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#నిర్వచితమైతే MCH_WRITE_DUMP
FILE* fdDump = ఖాళీ;
#అయితే_ముగింపు

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
స్థిర HANDLE g_h_Con_లోపల  = INVALID_HANDLE_విలువ;
స్థిర HANDLE g_h_Con_బహిర్గతం = INVALID_HANDLE_విలువ;

// Win32 Screen buffer,coordinate,console I/O information
స్థిర SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
స్థిర COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
స్థిర మాట_2  g_attrDefault = 7;  // lightgray text on black background
స్థిర మాట_2  g_attrCurrent;

స్థిర పూర్ణాంకం g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
స్థిర పూర్ణాంకం g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
స్థిర పూర్ణాంకం g_fForceExit = FALSE;    // set when forcefully exiting

స్థిర శూన్య scroll(గుర్తులేని c_పంక్తులు);
స్థిర శూన్య సమితి_scroll_region(గుర్తులేని ఎడమ_2, గుర్తులేని శిఖరం,
			      గుర్తులేని కుడి_2, గుర్తులేని bottom);
స్థిర శూన్య సమితి_scroll_region_tb(గుర్తులేని శిఖరం, గుర్తులేని bottom);
స్థిర శూన్య సమితి_scroll_region_lr(గుర్తులేని ఎడమ_2, గుర్తులేని కుడి_2);
స్థిర శూన్య చొప్పించు_పంక్తులు(గుర్తులేని c_పంక్తులు);
స్థిర శూన్య delete_పంక్తులు(గుర్తులేని c_పంక్తులు);
స్థిర శూన్య gotoxy(గుర్తులేని x, గుర్తులేని y);
స్థిర శూన్య standout(శూన్య);
స్థిర పూర్ణాంకం s_cursor_visible = TRUE;
స్థిర పూర్ణాంకం did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
రకనిర్వచనం నిర్మాణం ఆదానం_రికార్డు_బఫరు_కణుపు_S
{
    ఆదానం_రికార్డు ir;
    నిర్మాణం ఆదానం_రికార్డు_బఫరు_కణుపు_S *తదుపరి;
} ఆదానం_రికార్డు_బఫరు_కణుపు_T;
రకనిర్వచనం నిర్మాణం ఆదానం_రికార్డు_బఫరు_S
{
    ఆదానం_రికార్డు_బఫరు_కణుపు_T *తల;
    ఆదానం_రికార్డు_బఫరు_కణుపు_T *తోక;
    పూర్ణాంకం నిడివి;
} ఆదానం_రికార్డు_బఫరు_T;
స్థిర ఆదానం_రికార్డు_బఫరు_T ఆదానం_రికార్డు_బఫరు;
స్థిర పూర్ణాంకం read_ఆదానం_రికార్డు_బఫరు(ఆదానం_రికార్డు* irEvents, పూర్ణాంకం n_Max_నిడివి);
స్థిర పూర్ణాంకం write_ఆదానం_రికార్డు_బఫరు(ఆదానం_రికార్డు* irEvents, పూర్ణాంకం n_నిడివి);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_MSWIN
స్థిర పూర్ణాంకం s_dont_use_vimrun = TRUE;
స్థిర పూర్ణాంకం need_vimrun_warning = FALSE;
స్థిర string_T vimrun_మార్గం = {(char_u *)"vimrun ", 7};
స్థిర పూర్ణాంకం vimrun_మార్గం_allocated = FALSE;
#అయితే_ముగింపు

స్థిర పూర్ణాంకం win32_getattrs(char_u *name);
స్థిర పూర్ణాంకం win32_setattrs(char_u *name, పూర్ణాంకం attrs);
స్థిర పూర్ణాంకం win32_సమితి_archive(char_u *name);

స్థిర పూర్ణాంకం conpty_working = 0;
స్థిర పూర్ణాంకం conpty_type = 0;
స్థిర పూర్ణాంకం conpty_stable = 0;
స్థిర పూర్ణాంకం conpty_fix_type = 0;
స్థిర శూన్య vtp_flag_init(శూన్య);

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
స్థిర పూర్ణాంకం vtp_working = 0;
స్థిర శూన్య vtp_init(శూన్య);
స్థిర శూన్య vtp_exit(శూన్య);
స్థిర శూన్య vtp_sgr_bulk(పూర్ణాంకం arg);
స్థిర శూన్య vtp_sgr_bulks(పూర్ణాంకం argc, పూర్ణాంకం *argv);

స్థిర పూర్ణాంకం wt_working = 0;
స్థిర శూన్య wt_init(శూన్య);

స్థిర పూర్ణాంకం g_color_సూచీ_bg = 0;
స్థిర పూర్ణాంకం g_color_సూచీ_fg = 7;

# ifdef FEAT_TERMGUICOLORS
స్థిర guicolor_T save_console_bg_rgb;
స్థిర guicolor_T save_console_fg_rgb;
స్థిర guicolor_T store_console_bg_rgb;
స్థిర guicolor_T store_console_fg_rgb;
స్థిర పూర్ణాంకం default_console_color_bg = 0x000000; // black
స్థిర పూర్ణాంకం default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_పదం_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# లేకపోతే
#  define USE_VTP		0
#  define USE_WT		0
# endif

స్థిర శూన్య సమితి_console_color_rgb(శూన్య);
స్థిర శూన్య reset_console_color_rgb(శూన్య);
స్థిర శూన్య restore_console_color_rgb(శూన్య);
#అయితే_ముగింపు  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#నిర్వచితం_కాకపోతే ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#అయితే_ముగింపు

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
స్థిర పూర్ణాంకం suppress_winsize = 1;	// don't fiddle with console
#అయితే_ముగింపు

స్థిర WCHAR *exe_pathw = ఖాళీ;

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
స్థిర BOOL use_alternate_screen_బఫరు = FALSE;
#అయితే_ముగింపు

బాహ్య DWORD win_సంస్కరణ; // this is in os_mswin.c

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    స్థిర BOOL
is_ambiwidth_event(
    ఆదానం_రికార్డు *ir)
{
    తిరిగి ir->EventType == కీలకం_EVENT
		&& ir->Event.కీలకం_Event_2.b_కీలకం_కిందికి
		&& ir->Event.కీలకం_Event_2.w_Repeat_లెక్క == 1
		&& ir->Event.కీలకం_Event_2.w_Virtual_కీలకం_సంకేతం == 0x12
		&& ir->Event.కీలకం_Event_2.w_Virtual_పరిశీలన_సంకేతం == 0x38
		&& ir->Event.కీలకం_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.కీలకం_Event_2.dw_Control_కీలకం_State == 2;
}

    స్థిర శూన్య
make_ambiwidth_event(
    ఆదానం_రికార్డు *కిందికి,
    ఆదానం_రికార్డు *పైకి)
{
    కిందికి->Event.కీలకం_Event_2.w_Virtual_కీలకం_సంకేతం = 0;
    కిందికి->Event.కీలకం_Event_2.w_Virtual_పరిశీలన_సంకేతం = 0;
    కిందికి->Event.కీలకం_Event_2.uChar.UnicodeChar
				    = పైకి->Event.కీలకం_Event_2.uChar.UnicodeChar;
    కిందికి->Event.కీలకం_Event_2.dw_Control_కీలకం_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    స్థిర BOOL
read_console_ఆదానం(
    HANDLE	    h_ఆదానం,
    ఆదానం_రికార్డు    *lp_బఫరు,
    పూర్ణాంకం		    n_నిడివి,
    LPDWORD	    lpEvents)
{
    గణన
    {
	IRSIZE = 10
    };
    స్థిర ఆదానం_రికార్డు s_irCache[IRSIZE];
    స్థిర DWORD s_dw_సూచీ = 0;
    స్థిర DWORD s_dwMax = 0;
    DWORD dwEvents;
    పూర్ణాంకం తల;
    పూర్ణాంకం తోక;
    పూర్ణాంకం i;
    స్థిర ఆదానం_రికార్డు s_irPseudo;

    ఒకవేళ (s_dwMax == 0 && ఆదానం_రికార్డు_బఫరు.నిడివి > 0)
    {
	dwEvents = read_ఆదానం_రికార్డు_బఫరు(s_irCache, IRSIZE);
	s_dw_సూచీ = 0;
	s_dwMax = dwEvents;
    }

    ఒకవేళ (n_నిడివి == -2)
	తిరిగి (s_dwMax > 0) ? TRUE : FALSE;

    ఒకవేళ (win_సంస్కరణ < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	ఒకవేళ (n_నిడివి == -1)
	    తిరిగి Peek_Console_ఆదానం_W(h_ఆదానం, lp_బఫరు, 1, lpEvents);
	తిరిగి Read_Console_ఆదానం_W(h_ఆదానం, lp_బఫరు, 1, &dwEvents);
    }

    ఒకవేళ (s_dwMax == 0)
    {
	ఒకవేళ (!vtp_working && n_నిడివి == -1)
	    తిరిగి Peek_Console_ఆదానం_W(h_ఆదానం, lp_బఫరు, 1, lpEvents);
	Get_సంఖ్య_యొక్క_Console_ఆదానం_Events(h_ఆదానం, &dwEvents);
	ఒకవేళ (dwEvents == 0 && n_నిడివి == -1)
	    తిరిగి Peek_Console_ఆదానం_W(h_ఆదానం, lp_బఫరు, 1, lpEvents);
	Read_Console_ఆదానం_W(h_ఆదానం, s_irCache, IRSIZE, &dwEvents);
	s_dw_సూచీ = 0;
	s_dwMax = dwEvents;
	ఒకవేళ (dwEvents == 0)
	{
	    *lpEvents = 0;
	    తిరిగి TRUE;
	}

	కొరకు (i = s_dw_సూచీ; i < (పూర్ణాంకం)s_dwMax - 1; ++i)
	    ఒకవేళ (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	ఒకవేళ (s_dwMax > 1)
	{
	    తల = 0;
	    తోక = s_dwMax - 1;
	    అయితే (తల != తోక)
	    {
		ఒకవేళ (s_irCache[తల].EventType == కిటికీ_బఫరు_కొలత_EVENT
			&& s_irCache[తల + 1].EventType
						  == కిటికీ_బఫరు_కొలత_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    కొరకు (i = తల; i < తోక; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --తోక;
		    కొనసాగు;
		}
		తల++;
	    }
	    s_dwMax = తోక + 1;
	}
    }

    ఒకవేళ (s_irCache[s_dw_సూచీ].EventType == కీలకం_EVENT)
    {
	ఒకవేళ (s_irCache[s_dw_సూచీ].Event.కీలకం_Event_2.w_Repeat_లెక్క > 1)
	{
	    s_irPseudo = s_irCache[s_dw_సూచీ];
	    s_irPseudo.Event.కీలకం_Event_2.w_Repeat_లెక్క = 1;
	    s_irCache[s_dw_సూచీ].Event.కీలకం_Event_2.w_Repeat_లెక్క--;
	    *lp_బఫరు = s_irPseudo;
	    *lpEvents = 1;
	    తిరిగి TRUE;
	}
    }

    *lp_బఫరు = s_irCache[s_dw_సూచీ];
    ఒకవేళ (!(n_నిడివి == -1 || n_నిడివి == -2) && ++s_dw_సూచీ >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    తిరిగి TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    స్థిర BOOL
peek_console_ఆదానం(
    HANDLE	    h_ఆదానం,
    ఆదానం_రికార్డు    *lp_బఫరు,
    DWORD	    n_నిడివి వాడని,
    LPDWORD	    lpEvents)
{
    తిరిగి read_console_ఆదానం(h_ఆదానం, lp_బఫరు, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    స్థిర DWORD
సందేశం_wait_for_గుణిజం_objects(
    DWORD    n_లెక్క,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dw_Wake_ముసుగు)
{
    ఒకవేళ (read_console_ఆదానం(ఖాళీ, ఖాళీ, -2, ఖాళీ))
	తిరిగి WAIT_OBJECT_0;
    తిరిగి సందేశం_Wait_For_గుణిజం_Objects(n_లెక్క, pHandles, fWaitAll,
				     dwMilliseconds, dw_Wake_ముసుగు);
}
# endif

# ifndef FEAT_CLIENTSERVER
    స్థిర DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    ఒకవేళ (read_console_ఆదానం(ఖాళీ, ఖాళీ, -2, ఖాళీ))
	తిరిగి WAIT_OBJECT_0;
    తిరిగి WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#అయితే_ముగింపు   // !FEAT_GUI_MSWIN || VIMDLL

    శూన్య
mch_get_exe_name(శూన్య)
{
    పూర్ణాంకం		updated = FALSE;
    స్థిర పూర్ణాంకం	enc_prev = -1;
    WCHAR	*మార్గం;
    size_t	pathlen;

    ఒకవేళ (exe_name == ఖాళీ || exe_pathw == ఖాళీ || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_మార్గం];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(ఖాళీ, buf, MAX_మార్గం);
	ఒకవేళ (p > buf)
	{
	    ఒకవేళ (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_గమ్యం_enc(buf, ఖాళీ);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    చేయి
	    {
		--p;
		ఒకవేళ (*p == L'\\')
		{
		    *p = L'\0';
		    ఆపు;
		}
	    } అయితే (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    ఒకవేళ (exe_pathw == ఖాళీ || !updated)
	తిరిగి;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    మార్గం = _wgetenv(L"PATH");
    pathlen = (మార్గం == ఖాళీ) ? 0 : wcslen(మార్గం);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#నిర్వచించు EXTRA_LEN 7
#నిర్వచించు MAX_ENV_STRING_LEN 8191

    ఒకవేళ (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   తాత్కాలిక[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	ఒకవేళ (pathlen == 0)
	    wcscpy(తాత్కాలిక + templen, exe_pathw);
	లేకపోతే
	{
	    wcscpy(తాత్కాలిక + templen, మార్గం);

	    // Check if exe_path is already included in $PATH.
	    ఒకవేళ (wcsstr(తాత్కాలిక, exe_pathw) == ఖాళీ)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		ఒకవేళ (తాత్కాలిక[templen - 1] != L';')
		{
		    wcscpy(తాత్కాలిక + templen, L";");
		    ++templen;
		}

		wcscpy(తాత్కాలిక + templen, exe_pathw);
	    }
	}

	_wputenv(తాత్కాలిక);
#నిర్వచితమైతే libintl_wputenv
	libintl_wputenv(తాత్కాలిక);
#అయితే_ముగింపు
    }
#నిర్వచనం_రద్దు EXTRA_LEN
#నిర్వచనం_రద్దు MAX_ENV_మార్గం_LEN
}

/*
 * Unescape characters in "p" that appear in "escaped".
 */
    స్థిర శూన్య
unescape_shellxquote(char_u *p, char_u *escaped)
{
    పూర్ణాంకం	    l = (పూర్ణాంకం)STRLEN(p);
    పూర్ణాంకం	    n;

    అయితే (*p != NUL)
    {
	ఒకవేళ (*p == '^' && vim_strchr(escaped, p[1]) != ఖాళీ)
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
vim_ఎక్కించు_Lib(స్థిరరాశి అక్షరం *name)
{
    HINSTANCE	dll = ఖాళీ;

    // No need to load any library when registering OLE.
    ఒకవేళ (దొరికింది_రిజిస్టరు_arg)
	తిరిగి ఖాళీ;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    ఒకవేళ (exe_pathw == ఖాళీ)
    {
	mch_get_exe_name();
	ఒకవేళ (exe_pathw == ఖాళీ)
	    తిరిగి ఖాళీ;
    }

    WCHAR old_dirw[MAXPATHL];

    ఒకవేళ (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	తిరిగి ఖాళీ;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    సమితి_Current_Directory_W(exe_pathw);
    dll = ఎక్కించు_Library(name);
    సమితి_Current_Directory_W(old_dirw);
    తిరిగి dll;
}

#ఒకవేళ defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    పూర్ణాంకం
mch_is_gui_executable(శూన్య)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(ఖాళీ);
    PIMAGE_DOS_శీర్షిక	pDOS = (PIMAGE_DOS_శీర్షిక)pImage;
    PIMAGE_NT_శీర్షికలు	pPE;

    ఒకవేళ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	తిరిగి FALSE;
    pPE = (PIMAGE_NT_శీర్షికలు)(pImage + pDOS->e_lfanew);
    ఒకవేళ (pPE->Signature != IMAGE_NT_SIGNATURE)
	తిరిగి FALSE;
    ఒకవేళ (pPE->Optional_శీర్షిక.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	తిరిగి TRUE;
    తిరిగి FALSE;
}
#అయితే_ముగింపు

#ఒకవేళ defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    స్థిర శూన్య *
get_imported_func_info(HINSTANCE hInst, స్థిరరాశి అక్షరం *funcname, పూర్ణాంకం info,
	స్థిరరాశి శూన్య *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_శీర్షిక		pDOS = (PIMAGE_DOS_శీర్షిక)hInst;
    PIMAGE_NT_శీర్షికలు		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_దత్తాంశం		pIAT;	    // Import Address Table
    PIMAGE_THUNK_దత్తాంశం		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    ఒకవేళ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	తిరిగి ఖాళీ;
    pPE = (PIMAGE_NT_శీర్షికలు)(pImage + pDOS->e_lfanew);
    ఒకవేళ (pPE->Signature != IMAGE_NT_SIGNATURE)
	తిరిగి ఖాళీ;

    ImpVA = pPE->Optional_శీర్షిక
		.దత్తాంశం_Directory[IMAGE_DIRECTORY_నమోదు_IMPORT].Virtual_చిరునామా;
    ఒకవేళ (ImpVA == 0)
	తిరిగి ఖాళీ;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    కొరకు (; pImpDesc->మొదటి_Thunk; ++pImpDesc)
    {
	ఒకవేళ (!pImpDesc->Original_మొదటి_Thunk)
	    కొనసాగు;
	pIAT = (PIMAGE_THUNK_దత్తాంశం)(pImage + pImpDesc->మొదటి_Thunk);
	pINT = (PIMAGE_THUNK_దత్తాంశం)(pImage + pImpDesc->Original_మొదటి_Thunk);
	కొరకు (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    ఒకవేళ (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		కొనసాగు;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.చిరునామా_యొక్క_దత్తాంశం));
	    ఒకవేళ (strcmp((అక్షరం *)pImpName->Name, funcname) == 0)
	    {
		శూన్య *original;
		DWORD old, new = పుట_READWRITE;

		ఎంపిక (info)
		{
		    స్థితి 0:
			తిరిగి (శూన్య *)pIAT->u1.Function;
		    స్థితి 1:
			తిరిగి (శూన్య *)(pImage + pImpDesc->Name);
		    స్థితి 2:
			original = (శూన్య *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, పరిమాణం(శూన్య *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, పరిమాణం(శూన్య *),
				old, &new);
			తిరిగి original;
		    అప్రమేయ:
			తిరిగి ఖాళీ;
		}
	    }
	}
    }
    తిరిగి ఖాళీ;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
కనుగొను_imported_module_by_funcname(HINSTANCE hInst, స్థిరరాశి అక్షరం *funcname)
{
    అక్షరం    *modulename;

    modulename = (అక్షరం *)get_imported_func_info(hInst, funcname, 1, ఖాళీ);
    ఒకవేళ (modulename != ఖాళీ)
	తిరిగి GetModuleHandleA(modulename);
    తిరిగి ఖాళీ;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    శూన్య *
get_dll_import_func(HINSTANCE hInst, స్థిరరాశి అక్షరం *funcname)
{
    తిరిగి get_imported_func_info(hInst, funcname, 0, ఖాళీ);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    శూన్య *
hook_dll_import_func(HINSTANCE hInst, స్థిరరాశి అక్షరం *funcname, స్థిరరాశి శూన్య *hook)
{
    తిరిగి get_imported_func_info(hInst, funcname, 2, hook);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PYTHON3)
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
    PIMAGE_DOS_శీర్షిక		pDOS = (PIMAGE_DOS_శీర్షిక)hInst;
    PIMAGE_NT_శీర్షికలు		pPE;
    PIMAGE_EXPORT_DIRECTORY	pExpDir;
    DWORD			ExpVA;
    DWORD			Exp_కొలత;
    LPDWORD			p_Function_పట్టిక;

    ఒకవేళ (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	తిరిగి ఖాళీ;
    pPE = (PIMAGE_NT_శీర్షికలు)(pImage + pDOS->e_lfanew);
    ఒకవేళ (pPE->Signature != IMAGE_NT_SIGNATURE)
	తిరిగి ఖాళీ;

    ExpVA = pPE->Optional_శీర్షిక
		.దత్తాంశం_Directory[IMAGE_DIRECTORY_నమోదు_EXPORT].Virtual_చిరునామా;
    Exp_కొలత = pPE->Optional_శీర్షిక
		.దత్తాంశం_Directory[IMAGE_DIRECTORY_నమోదు_EXPORT].కొలత_2;
    ఒకవేళ (ExpVA == 0)
	తిరిగి hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    p_Function_పట్టిక = (LPDWORD)(pImage + pExpDir->చిరునామా_యొక్క_Functions);

    ఒకవేళ (pExpDir->సంఖ్య_యొక్క_పేర్లు == 0)
	తిరిగి hInst;	// No export names.

    // Check only the first entry.
    ఒకవేళ ((p_Function_పట్టిక[0] < ExpVA) || (p_Function_పట్టిక[0] >= ExpVA + Exp_కొలత))
	// The first entry is not a function forwarder.
	తిరిగి hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    స్థిరరాశి అక్షరం *name = (స్థిరరాశి అక్షరం *)(pImage + p_Function_పట్టిక[0]);
    స్థిరరాశి అక్షరం *p = strchr(name, '.');
    ఒకవేళ (p == ఖాళీ)
	తిరిగి hInst;

    // Extract DllName.
    అక్షరం buf[MAX_మార్గం];
    ఒకవేళ (p - name + 1 > పరిమాణం(buf))
	తిరిగి ఖాళీ;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    తిరిగి GetModuleHandleA(buf);
}
#అయితే_ముగింపు

#ఒకవేళ defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
స్థిర అక్షరం *null_libintl_gettext(స్థిరరాశి అక్షరం *);
స్థిర అక్షరం *null_libintl_ngettext(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *, గుర్తులేని దీర్ఘ n);
స్థిర అక్షరం *null_libintl_textdomain(స్థిరరాశి అక్షరం *);
స్థిర అక్షరం *null_libintl_bindtextdomain(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *);
స్థిర అక్షరం *null_libintl_bind_textdomain_codeset(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *);
స్థిర పూర్ణాంకం null_libintl_wputenv(స్థిరరాశి wchar_t *);

స్థిర HINSTANCE hLibintlDLL = ఖాళీ;
అక్షరం *(*dyn_libintl_gettext)(స్థిరరాశి అక్షరం *) = null_libintl_gettext;
అక్షరం *(*dyn_libintl_ngettext)(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *, గుర్తులేని దీర్ఘ n)
						= null_libintl_ngettext;
అక్షరం *(*dyn_libintl_textdomain)(స్థిరరాశి అక్షరం *) = null_libintl_textdomain;
అక్షరం *(*dyn_libintl_bindtextdomain)(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *)
						= null_libintl_bindtextdomain;
అక్షరం *(*dyn_libintl_bind_textdomain_codeset)(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *)
				       = null_libintl_bind_textdomain_codeset;
పూర్ణాంకం (*dyn_libintl_wputenv)(స్థిరరాశి wchar_t *) = null_libintl_wputenv;
పూర్ణాంకం *dyn_libintl_nl_సందేశం_cat_cntr = ఖాళీ;

    పూర్ణాంకం
dyn_libintl_init(శూన్య)
{
    పూర్ణాంకం i;
    స్థిర నిర్మాణం
    {
	అక్షరం	    *name;
	FARPROC	    *ptr;
    } libintl_నమోదు[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{ఖాళీ, ఖాళీ}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    ఒకవేళ (hLibintlDLL != ఖాళీ)
	తిరిగి 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vim_ఎక్కించు_Lib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    ఒకవేళ (!hLibintlDLL)
	hLibintlDLL = vim_ఎక్కించు_Lib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    ఒకవేళ (!hLibintlDLL)
	hLibintlDLL = vim_ఎక్కించు_Lib(GETTEXT_DLL_ALT2);
# endif
    ఒకవేళ (!hLibintlDLL)
    {
	ఒకవేళ (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_ఎక్కించు_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	తిరిగి 0;
    }
    కొరకు (i = 0; libintl_నమోదు[i].name != ఖాళీ
					 && libintl_నమోదు[i].ptr != ఖాళీ; ++i)
    {
	ఒకవేళ ((*libintl_నమోదు[i].ptr = Get_Proc_చిరునామా(hLibintlDLL,
					      libintl_నమోదు[i].name)) == ఖాళీ)
	{
	    dyn_libintl_చివర();
	    ఒకవేళ (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_ఎక్కించు_library_function_str), libintl_నమోదు[i].name);
		verbose_leave();
	    }
	    తిరిగి 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (అక్షరం *(*)(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *))
			Get_Proc_చిరునామా(hLibintlDLL, "bind_textdomain_codeset");
    ఒకవేళ (dyn_libintl_bind_textdomain_codeset == ఖాళీ)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = కనుగొను_imported_module_by_funcname(hLibintlDLL, "getenv");
    ఒకవేళ (hmsvcrt != ఖాళీ)
	dyn_libintl_wputenv = (పూర్ణాంకం (*)(స్థిరరాశి wchar_t *))
					Get_Proc_చిరునామా(hmsvcrt, "_wputenv");
    ఒకవేళ (dyn_libintl_wputenv == ఖాళీ || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_సందేశం_cat_cntr = (పూర్ణాంకం *)Get_Proc_చిరునామా(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    తిరిగి 1;
}

    శూన్య
dyn_libintl_చివర(శూన్య)
{
    ఒకవేళ (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ఖాళీ;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_సందేశం_cat_cntr	= ఖాళీ;
}

    స్థిర అక్షరం *
null_libintl_gettext(స్థిరరాశి అక్షరం *msgid)
{
    తిరిగి (అక్షరం*)msgid;
}

    స్థిర అక్షరం *
null_libintl_ngettext(
	స్థిరరాశి అక్షరం *msgid,
	స్థిరరాశి అక్షరం *msgid_plural,
	గుర్తులేని దీర్ఘ n)
{
    తిరిగి (అక్షరం *)(n == 1 ? msgid : msgid_plural);
}

    స్థిర అక్షరం *
null_libintl_bindtextdomain(
	స్థిరరాశి అక్షరం *domainname వాడని,
	స్థిరరాశి అక్షరం *dirname వాడని)
{
    తిరిగి ఖాళీ;
}

    స్థిర అక్షరం *
null_libintl_bind_textdomain_codeset(
	స్థిరరాశి అక్షరం *domainname వాడని,
	స్థిరరాశి అక్షరం *codeset వాడని)
{
    తిరిగి ఖాళీ;
}

    స్థిర అక్షరం *
null_libintl_textdomain(స్థిరరాశి అక్షరం *domainname వాడని)
{
    తిరిగి ఖాళీ;
}

    స్థిర పూర్ణాంకం
null_libintl_wputenv(స్థిరరాశి wchar_t *envstring వాడని)
{
    తిరిగి 0;
}

#అయితే_ముగింపు // DYNAMIC_GETTEXT

// This symbol is not defined in older versions of the SDK or Visual C++

#నిర్వచితం_కాకపోతే VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ACL
# include <aclapi.h>
# ifndef PROTECTED_DACL_SECURITY_INFORMATION
#  define PROTECTED_DACL_SECURITY_INFORMATION	0x80000000L
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ACL
/*
 * Enables or disables the specified privilege.
 */
    స్థిర BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    ఒకవేళ (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	తిరిగి FALSE;

    ఒకవేళ (!Lookup_Privilege_విలువ(ఖాళీ, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	తిరిగి FALSE;
    }

    tokenPrivileges.Privilege_లెక్క	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].లక్షణాలు_2 = SE_PRIVILEGE_ENABLED;

    ఒకవేళ (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, ఖాళీ, ఖాళీ))
    {
	CloseHandle(hToken);
	తిరిగి FALSE;
    }

    ఒకవేళ (Get_చివరి_Error() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	తిరిగి FALSE;
    }

    CloseHandle(hToken);
    తిరిగి TRUE;
}
#అయితే_ముగింపు

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    శూన్య
PlatformId(శూన్య)
{
#నిర్వచితమైతే FEAT_EVAL
    DWORD major = (win_సంస్కరణ >> 24) & 0xFF;
    DWORD minor = (win_సంస్కరణ >> 16) & 0xFF;
    vim_snprintf(windows_సంస్కరణ, పరిమాణం(windows_సంస్కరణ), "%d.%d", major, minor);
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    ఒకవేళ (!win32_enable_privilege(SE_SECURITY_NAME))
	తిరిగి;
#అయితే_ముగింపు
}

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define జరుపు_2  (జరుపు_PRESSED)
# define CTRL   (కుడి_CTRL_PRESSED | ఎడమ_CTRL_PRESSED)
# define ALT    (కుడి_ALT_PRESSED  | ఎడమ_ALT_PRESSED)
# define ALT_GR (కుడి_ALT_PRESSED  | ఎడమ_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

స్థిర స్థిరరాశి నిర్మాణం
{
    మాట_2    w_Virt_కీలకం;
    BOOL    f_Ansi_కీలకం;
    పూర్ణాంకం	    chAlone;
    పూర్ణాంకం	    ch_జరుపు;
    పూర్ణాంకం	    chCtrl;
    పూర్ణాంకం	    chAlt;
} Virt_కీలకం_Map[] =
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
    { VK_పైకి,	TRUE,	'H',	'\304',	'\305',	    '\306', },
    { VK_పూర్వ,	TRUE,	'I',	'\307',	'\204',	    '\310', }, // PgUp
    { VK_ఎడమ,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_కుడి,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_చివర,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_కిందికి,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_తదుపరి,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_చొప్పించు,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_వెనుక,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# ఒకవేళ 0
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
    { VK_కూడు,	TRUE,   'N',    'N',    'N',	'N',	}, // keyp '+'
    { VK_వ్యవకలనం, TRUE,'J',	'J',    'J',	'J',	}, // keyp '-'
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
    స్థిర పూర్ణాంకం
win32_kbd_patch_కీలకం(
    కీలకం_EVENT_రికార్డు *pker)
{
    UINT uMods = pker->dw_Control_కీలకం_State;
    స్థిర పూర్ణాంకం s_iIsDead = 0;
    స్థిర మాట_2 aw_Ansi_సంకేతం[2];
    స్థిర BYTE abKeystate[256];

    ఒకవేళ (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_సంకేతం[1];
	s_iIsDead = 0;
	తిరిగి 1;
    }

    // check if it already has a valid unicode character.
    ఒకవేళ (pker->uChar.UnicodeChar != 0)
	తిరిగి 1;

    CLEAR_క్షేత్రం(abKeystate);

    // Clear any pending dead keys
    గమ్యం_Unicode(VK_SPACE, Map_Virtual_కీలకం(VK_SPACE, 0), abKeystate, aw_Ansi_సంకేతం, 2, 0);

    ఒకవేళ (uMods & జరుపు_PRESSED)
	abKeystate[VK_జరుపు] = 0x80;
    ఒకవేళ (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    ఒకవేళ ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = గమ్యం_Unicode(pker->w_Virtual_కీలకం_సంకేతం, pker->w_Virtual_పరిశీలన_సంకేతం,
			abKeystate, aw_Ansi_సంకేతం, 2, 0);

    ఒకవేళ (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_సంకేతం[0];

    తిరిగి s_iIsDead;
}

స్థిర BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    స్థిర BOOL
decode_కీలకం_event(
    కీలకం_EVENT_రికార్డు	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    పూర్ణాంకం			*pmodifiers,
    BOOL		fDoPost వాడని)
{
    పూర్ణాంకం i;
    స్థిరరాశి పూర్ణాంకం nModifs = pker->dw_Control_కీలకం_State & (జరుపు_2 | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    ఒకవేళ (!pker->b_కీలకం_కిందికి)
	తిరిగి FALSE;

    // ignore some keystrokes
    ఎంపిక (pker->w_Virtual_కీలకం_సంకేతం)
    {
    // modifiers
    స్థితి VK_జరుపు:
    స్థితి VK_CONTROL:
    స్థితి VK_MENU:   // Alt key
	తిరిగి FALSE;

    అప్రమేయ:
	ఆపు;
    }

    // Shift-TAB
    ఒకవేళ (pker->w_Virtual_కీలకం_సంకేతం == VK_TAB && (nModifs & జరుపు_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	తిరిగి TRUE;
    }

    కొరకు (i = ARRAY_నిడివి(Virt_కీలకం_Map); --i >= 0; )
    {
	ఒకవేళ (Virt_కీలకం_Map[i].w_Virt_కీలకం == pker->w_Virtual_కీలకం_సంకేతం)
	{
	    *pch = Virt_కీలకం_Map[i].chAlone;
	    ఒకవేళ ((nModifs & జరుపు_2) != 0)
		*pch = Virt_కీలకం_Map[i].ch_జరుపు;
	    లేకపోతే ఒకవేళ ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_కీలకం_Map[i].chCtrl;
	    లేకపోతే ఒకవేళ ((nModifs & ALT) != 0)
		*pch = Virt_కీలకం_Map[i].chAlt;

	    ఒకవేళ (*pch != 0)
	    {
		ఒకవేళ (Virt_కీలకం_Map[i].f_Ansi_కీలకం)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    ఒకవేళ (pmodifiers)
		    {
			ఒకవేళ (pker->w_Virtual_కీలకం_సంకేతం >= VK_F1
			    && pker->w_Virtual_కీలకం_సంకేతం <= VK_F12)
			{
			    ఒకవేళ ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_ముసుగు_ALT;
				ఒకవేళ ((nModifs & జరుపు_2) == 0)
				    *pch2 = Virt_కీలకం_Map[i].chAlone;
			    }
			    ఒకవేళ ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_ముసుగు_CTRL;
				ఒకవేళ ((nModifs & జరుపు_2) == 0)
				    *pch2 = Virt_కీలకం_Map[i].chAlone;
			    }
			}
			లేకపోతే ఒకవేళ (pker->w_Virtual_కీలకం_సంకేతం >= VK_చివర
				&& pker->w_Virtual_కీలకం_సంకేతం <= VK_కిందికి)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_కీలకం_Map[i].chAlone;
			    ఒకవేళ ((nModifs & జరుపు_2) != 0
						    && (nModifs & ~జరుపు_2) == 0)
			    {
				*pch2 = Virt_కీలకం_Map[i].ch_జరుపు;
			    }
			    ఒకవేళ ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_కీలకం_Map[i].chCtrl;
				ఒకవేళ (pker->w_Virtual_కీలకం_సంకేతం == VK_పైకి
				    || pker->w_Virtual_కీలకం_సంకేతం == VK_కిందికి)
				{
				    *pmodifiers |= MOD_ముసుగు_CTRL;
				    *pch2 = Virt_కీలకం_Map[i].chAlone;
				}
			    }
			    ఒకవేళ ((nModifs & జరుపు_2) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_ముసుగు_CTRL;
				*pch2 = Virt_కీలకం_Map[i].ch_జరుపు;
			    }
			    ఒకవేళ ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_కీలకం_Map[i].chAlt;
				*pmodifiers |= MOD_ముసుగు_ALT;
				ఒకవేళ ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_కీలకం_Map[i].chAlone;
				}
				లేకపోతే ఒకవేళ ((nModifs & జరుపు_2) != 0)
				{
				    *pch2 = Virt_కీలకం_Map[i].ch_జరుపు;
				}
				లేకపోతే ఒకవేళ ((nModifs & CTRL) != 0)
				{
				    ఒకవేళ (pker->w_Virtual_కీలకం_సంకేతం == VK_పైకి
					|| pker->w_Virtual_కీలకం_సంకేతం == VK_కిందికి)
				    {
					*pmodifiers |= MOD_ముసుగు_CTRL;
					*pch2 = Virt_కీలకం_Map[i].chAlone;
				    }
				    లేకపోతే
				    {
					*pch2 = Virt_కీలకం_Map[i].chCtrl;
				    }
				}
			    }
			}
			లేకపోతే ఒకవేళ (pker->w_Virtual_కీలకం_సంకేతం == VK_చొప్పించు
					&& (nModifs & జరుపు_2) != 0
					&& (nModifs & ~జరుపు_2) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_కీలకం_Map[i].ch_జరుపు;
			}
			లేకపోతే
			{
			    *pch2 = Virt_కీలకం_Map[i].chAlone;
			    ఒకవేళ ((nModifs & జరుపు_2) != 0)
				*pmodifiers |= MOD_ముసుగు_జరుపు;
			    ఒకవేళ ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_ముసుగు_CTRL;
			    ఒకవేళ ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_ముసుగు_ALT;
			}
		    }
		}

		తిరిగి TRUE;
	    }
	}
    }

    i = win32_kbd_patch_కీలకం(pker);

    ఒకవేళ (i < 0)
	*pch = NUL;
    లేకపోతే
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	ఒకవేళ (pmodifiers != ఖాళీ)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    ఒకవేళ ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_ముసుగు_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    ఒకవేళ ((nModifs & జరుపు_2) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_ముసుగు_జరుపు;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    ఒకవేళ ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_ముసుగు_CTRL;
	}
    }

    తిరిగి (*pch != NUL);
}

# ఒకవేళ defined(FEAT_EVAL)
    స్థిర పూర్ణాంకం
encode_కీలకం_event(dict_T *వాదనలు, ఆదానం_రికార్డు *ir)
{
    స్థిర పూర్ణాంకం s_dwMods = 0;

    char_u *action = dict_get_string(వాదనలు, "event", TRUE);
    ఒకవేళ (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_కీలకం_కిందికి = STRICMP(action, "keydown") == 0;
	మాట_2 vk_సంకేతం = dict_get_సంఖ్య_def(వాదనలు, "keycode", 0);
	ఒకవేళ (vk_సంకేతం <= 0 || vk_సంకేతం >= 0xFF)
	{
	    semsg(_(e_invalid_వాదం_nr), (దీర్ఘ)vk_సంకేతం);
	    తిరిగి FALSE;
	}

	ir->EventType = కీలకం_EVENT;
	కీలకం_EVENT_రికార్డు ker;
	Zero_జ్ఞాపకం(&ker, పరిమాణం(ker));
	ker.b_కీలకం_కిందికి = is_కీలకం_కిందికి;
	ker.w_Repeat_లెక్క = 1;
	ker.w_Virtual_పరిశీలన_సంకేతం = 0;
	ker.dw_Control_కీలకం_State = 0;
	పూర్ణాంకం mods = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	ఒకవేళ (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    ఒకవేళ (mods & MOD_ముసుగు_జరుపు)
		ker.dw_Control_కీలకం_State |= జరుపు_PRESSED;
	    ఒకవేళ (mods & MOD_ముసుగు_CTRL)
		ker.dw_Control_కీలకం_State |= ఎడమ_CTRL_PRESSED;
	    ఒకవేళ (mods & MOD_ముసుగు_ALT)
		ker.dw_Control_కీలకం_State |= ఎడమ_ALT_PRESSED;
	}

	ఒకవేళ (vk_సంకేతం == VK_LSHIFT || vk_సంకేతం == VK_RSHIFT || vk_సంకేతం == VK_జరుపు)
	{
	    ఒకవేళ (is_కీలకం_కిందికి)
		s_dwMods |= జరుపు_PRESSED;
	    లేకపోతే
		s_dwMods &= ~జరుపు_PRESSED;
	}
	లేకపోతే ఒకవేళ (vk_సంకేతం == VK_LCONTROL || vk_సంకేతం == VK_CONTROL)
	{
	    ఒకవేళ (is_కీలకం_కిందికి)
		s_dwMods |= ఎడమ_CTRL_PRESSED;
	    లేకపోతే
		s_dwMods &= ~ఎడమ_CTRL_PRESSED;
	}
	లేకపోతే ఒకవేళ (vk_సంకేతం == VK_RCONTROL)
	{
	    ఒకవేళ (is_కీలకం_కిందికి)
		s_dwMods |= కుడి_CTRL_PRESSED;
	    లేకపోతే
		s_dwMods &= ~కుడి_CTRL_PRESSED;
	}
	లేకపోతే ఒకవేళ (vk_సంకేతం == VK_LMENU || vk_సంకేతం == VK_MENU)
	{
	    ఒకవేళ (is_కీలకం_కిందికి)
		s_dwMods |= ఎడమ_ALT_PRESSED;
	    లేకపోతే
		s_dwMods &= ~ఎడమ_ALT_PRESSED;
	}
	లేకపోతే ఒకవేళ (vk_సంకేతం == VK_RMENU)
	{
	    ఒకవేళ (is_కీలకం_కిందికి)
		s_dwMods |= కుడి_ALT_PRESSED;
	    లేకపోతే
		s_dwMods &= ~కుడి_ALT_PRESSED;
	}
	ker.dw_Control_కీలకం_State |= s_dwMods;
	ker.w_Virtual_కీలకం_సంకేతం = vk_సంకేతం;
	ker.uChar.UnicodeChar = 0;
	ir->Event.కీలకం_Event_2 = ker;
	vim_free(action);
    }
    లేకపోతే
    {
	ఒకవేళ (action == ఖాళీ)
	{
	    semsg(_(e_missing_వాదం_str), "event");
	}
	లేకపోతే
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str_str), "event", action);
	    vim_free(action);
	}
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}
# endif  // FEAT_EVAL
#అయితే_ముగింపు // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
స్థిర పూర్ణాంకం g_fMouseAvail = FALSE;   // mouse present
స్థిర పూర్ణాంకం g_f_Mouse_చురుకైన = FALSE;  // mouse enabled
స్థిర పూర్ణాంకం g_nMouseClick = -1;	    // mouse status
స్థిర పూర్ణాంకం g_xMouse;		    // mouse x coordinate
స్థిర పూర్ణాంకం g_yMouse;		    // mouse y coordinate
స్థిర DWORD g_cmodein = 0;	    // Original console input mode
స్థిర DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    శూన్య
mch_setmouse(పూర్ణాంకం on)
{
    DWORD cmodein;

# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
	తిరిగి;
# endif
    ఒకవేళ (!g_fMouseAvail)
	తిరిగి;

    g_f_Mouse_చురుకైన = on;
    Get_Console_బహుళకం(g_h_Con_లోపల, &cmodein);

    ఒకవేళ (g_f_Mouse_చురుకైన)
    {
	cmodein |= ENABLE_MOUSE_ఆదానం;
	cmodein &= ~ENABLE_శీఘ్ర_EDIT_బహుళకం;
    }
    లేకపోతే
    {
	cmodein &= ~ENABLE_MOUSE_ఆదానం;
	cmodein |= g_cmodein & ENABLE_శీఘ్ర_EDIT_బహుళకం;
    }

    సమితి_Console_బహుళకం(g_h_Con_లోపల, cmodein | ENABLE_EXTENDED_FLAGS);
}


# ఒకవేళ defined(FEAT_BEVAL_పదం)
/*
 * Called when 'balloonevalterm' changed.
 */
    శూన్య
mch_bevalterm_మారింది(శూన్య)
{
    mch_setmouse(g_f_Mouse_చురుకైన);
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
    స్థిర శూన్య
decode_mouse_wheel(MOUSE_EVENT_రికార్డు *pmer)
{
    పూర్ణాంకం	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    పూర్ణాంకం	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    పూర్ణాంకం lcol = g_xMouse;
    పూర్ణాంకం lrow = g_yMouse;
    win_T *wp = mouse_కనుగొను_win(&lrow, &lcol, కనుగొను_POPUP);
    ఒకవేళ (wp != ఖాళీ && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_క్షేత్రం(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	ఒకవేళ (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ఎడమ : MSCR_కుడి;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	లేకపోతే
	{
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	    cap.arg = zDelta < 0 ? MSCR_పైకి : MSCR_కిందికి;
	}

	// Mouse hovers over popup window, scroll it if possible.
	mouse_అడ్డువరుస = wp->w_winrow;
	mouse_నిలువు = wp->w_wincol;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	బహిర్గతం_flush();
    }
# endif
    mouse_నిలువు = g_xMouse;
    mouse_అడ్డువరుస = g_yMouse;

    char_u modifiers = 0;
    char_u direction = 0;

    // Decode the direction into an event that Vim can process
    ఒకవేళ (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    లేకపోతే
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    ఒకవేళ (pmer->dw_Control_కీలకం_State & జరుపు_PRESSED)
	modifiers |= MOD_ముసుగు_జరుపు; // MOUSE_SHIFT;
    ఒకవేళ (pmer->dw_Control_కీలకం_State & (కుడి_CTRL_PRESSED | ఎడమ_CTRL_PRESSED))
	modifiers |= MOD_ముసుగు_CTRL; // MOUSE_CTRL;
    ఒకవేళ (pmer->dw_Control_కీలకం_State & (కుడి_ALT_PRESSED  | ఎడమ_ALT_PRESSED))
	modifiers |= MOD_ముసుగు_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    తిరిగి;
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
    స్థిర BOOL
decode_mouse_event(
    MOUSE_EVENT_రికార్డు *pmer)
{
    స్థిర పూర్ణాంకం s_nOldButton = -1;
    స్థిర పూర్ణాంకం s_nOldMouseClick = -1;
    స్థిర పూర్ణాంకం s_xOldMouse = -1;
    స్థిర పూర్ణాంకం s_yOldMouse = -1;
    స్థిర linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    స్థిర పూర్ణాంకం s_old_topfill = 0;
# endif
    స్థిర పూర్ణాంకం s_cClicks = 1;
    స్థిర BOOL s_fReleased = TRUE;
    స్థిర DWORD s_dw_చివరి_Click_సమయం = 0;
    స్థిర BOOL s_f_తదుపరి_Is_మధ్య = FALSE;

    స్థిర DWORD cButtons = 0;	// number of buttons supported

    స్థిరరాశి DWORD ఎడమ_3 = FROM_ఎడమ_1_ST_BUTTON_PRESSED;
    స్థిరరాశి DWORD మధ్య = FROM_ఎడమ_2_ND_BUTTON_PRESSED;
    స్థిరరాశి DWORD కుడి_3 = RIGHTMOST_BUTTON_PRESSED;
    స్థిరరాశి DWORD ఎడమ_కుడి = ఎడమ_3 | కుడి_3;

    పూర్ణాంకం nButton;

    ఒకవేళ (cButtons == 0 && !Get_సంఖ్య_యొక్క_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    ఒకవేళ (!g_fMouseAvail || !g_f_Mouse_చురుకైన)
    {
	g_nMouseClick = -1;
	తిరిగి FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    ఒకవేళ (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	తిరిగి FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    ఒకవేళ (g_nMouseClick != -1)
	తిరిగి TRUE;

    ఒకవేళ (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	తిరిగి TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    ఒకవేళ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	ఒకవేళ (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    తిరిగి FALSE;
    }

    // If no buttons are pressed...
    ఒకవేళ ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	ఒకవేళ (s_fReleased)
	{
# ifdef FEAT_BEVAL_పదం
	    // do return mouse move events when we want them
	    ఒకవేళ (p_bevalterm)
		nButton = MOUSE_DRAG;
	    లేకపోతే
# endif
		తిరిగి FALSE;
	}

	s_fReleased = TRUE;
    }
    లేకపోతే    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	ఒకవేళ (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & ఎడమ_కుడి);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    ఒకవేళ (dwLR == ఎడమ_3 || dwLR == కుడి_3)
	    {
		కొరకు (;;)
		{
		    // wait a short time for next input event
		    ఒకవేళ (WaitForSingleObject(g_h_Con_లోపల, p_mouset / 3)
							     != WAIT_OBJECT_0)
			ఆపు;
		    లేకపోతే
		    {
			DWORD c_రికార్డులు = 0;
			ఆదానం_రికార్డు ir;
			MOUSE_EVENT_రికార్డు* pmer2 = &ir.Event.MouseEvent;

			peek_console_ఆదానం(g_h_Con_లోపల, &ir, 1, &c_రికార్డులు);

			ఒకవేళ (c_రికార్డులు == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & ఎడమ_కుడి))
			    ఆపు;
			లేకపోతే
			{
			    ఒకవేళ (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_ఆదానం(g_h_Con_లోపల, &ir, 1, &c_రికార్డులు);

				తిరిగి decode_mouse_event(pmer2);
			    }
			    లేకపోతే ఒకవేళ (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_ఆదానం(g_h_Con_లోపల, &ir, 1, &c_రికార్డులు);

				// are there any more mouse events in queue?
				peek_console_ఆదానం(g_h_Con_లోపల, &ir, 1, &c_రికార్డులు);

				ఒకవేళ (c_రికార్డులు==0 || ir.EventType != MOUSE_EVENT)
				    ఆపు;
			    }
			    లేకపోతే
				ఆపు;
			}
		    }
		}
	    }
	}

	ఒకవేళ (s_f_తదుపరి_Is_మధ్య)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_మధ్య;
	    s_f_తదుపరి_Is_మధ్య = FALSE;
	}
	లేకపోతే ఒకవేళ (cButtons == 2	&&
	    ((pmer->dwButtonState & ఎడమ_కుడి) == ఎడమ_కుడి))
	{
	    nButton = MOUSE_మధ్య;

	    ఒకవేళ (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_తదుపరి_Is_మధ్య = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	లేకపోతే ఒకవేళ ((pmer->dwButtonState & ఎడమ_3) == ఎడమ_3)
	    nButton = MOUSE_ఎడమ;
	లేకపోతే ఒకవేళ ((pmer->dwButtonState & మధ్య) == మధ్య)
	    nButton = MOUSE_మధ్య;
	లేకపోతే ఒకవేళ ((pmer->dwButtonState & కుడి_3) == కుడి_3)
	    nButton = MOUSE_కుడి;

	ఒకవేళ (! s_fReleased && ! s_f_తదుపరి_Is_మధ్య
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    తిరిగి FALSE;

	s_fReleased = s_f_తదుపరి_Is_మధ్య;
    }

    ఒకవేళ (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	ఒకవేళ (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dw_Current_సమయం = Get_Tick_లెక్క();

	    ఒకవేళ (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (పూర్ణాంకం)(dw_Current_సమయం - s_dw_చివరి_Click_సమయం) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    లేకపోతే ఒకవేళ (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dw_చివరి_Click_సమయం = dw_Current_సమయం;
	}
    }
    లేకపోతే ఒకవేళ (pmer->dwEventFlags == MOUSE_MOVED)
    {
	ఒకవేళ (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    ఒకవేళ (nButton == -1)
	తిరిగి FALSE;

    ఒకవేళ (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    ఒకవేళ (pmer->dw_Control_కీలకం_State & జరుపు_PRESSED)
	g_nMouseClick |= MOUSE_జరుపు;
    ఒకవేళ (pmer->dw_Control_కీలకం_State & (కుడి_CTRL_PRESSED | ఎడమ_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    ఒకవేళ (pmer->dw_Control_కీలకం_State & (కుడి_ALT_PRESSED  | ఎడమ_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    ఒకవేళ (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	సమితి_అంకె_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    ఒకవేళ (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	తిరిగి FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    తిరిగి TRUE;
}

# ifdef FEAT_EVAL
    స్థిర పూర్ణాంకం
encode_mouse_event(dict_T *వాదనలు, ఆదానం_రికార్డు *ir)
{
    పూర్ణాంకం		button;
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		repeated_click;
    int_u	mods = 0;
    పూర్ణాంకం		move;

    ఒకవేళ (!dict_has_కీలకం(వాదనలు, "row") || !dict_has_కీలకం(వాదనలు, "col"))
	తిరిగి FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (పూర్ణాంకం)dict_get_bool(వాదనలు, "move", FALSE);
    ఒకవేళ (!move && (!dict_has_కీలకం(వాదనలు, "button")
	    || !dict_has_కీలకం(వాదనలు, "multiclick")
	    || !dict_has_కీలకం(వాదనలు, "modifiers")))
	తిరిగి FALSE;

    అడ్డువరుస = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "row") - 1;
    నిలువు = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_రికార్డు mer;
    Zero_జ్ఞాపకం(&mer, పరిమాణం(mer));
    mer.dwMousePosition.X  = నిలువు;
    mer.dwMousePosition.Y  = అడ్డువరుస;

    ఒకవేళ (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    లేకపోతే
    {
	button = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "button");
	repeated_click = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "multiclick");
	mods = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_సమితి_hor_scroll_అడుగు(6);
	mouse_సమితి_vert_scroll_అడుగు(3);

	ఎంపిక (button)
	{
	    స్థితి MOUSE_ఎడమ:
		mer.dwButtonState = FROM_ఎడమ_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ఆపు;
	    స్థితి MOUSE_మధ్య:
		mer.dwButtonState = FROM_ఎడమ_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ఆపు;
	    స్థితి MOUSE_కుడి:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		ఆపు;
	    స్థితి MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    స్థితి MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		ఆపు;
	    స్థితి MOUSE_X1:
		mer.dwButtonState = FROM_ఎడమ_3_RD_BUTTON_PRESSED;
		ఆపు;
	    స్థితి MOUSE_X2:
		mer.dwButtonState = FROM_ఎడమ_4_TH_BUTTON_PRESSED;
		ఆపు;
	    స్థితి MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ఆపు;
	    స్థితి MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		ఆపు;
	    స్థితి MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ఆపు;
	    స్థితి MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		ఆపు;
	    అప్రమేయ:
		semsg(_(e_invalid_వాదం_str), "button");
		తిరిగి FALSE;
	}
    }

    mer.dw_Control_కీలకం_State = 0;
    ఒకవేళ (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	ఒకవేళ (mods & MOUSE_జరుపు)
	    mer.dw_Control_కీలకం_State |= జరుపు_PRESSED;
	ఒకవేళ (mods & MOUSE_CTRL)
	    mer.dw_Control_కీలకం_State |= ఎడమ_CTRL_PRESSED;
	ఒకవేళ (mods & MOUSE_ALT)
	    mer.dw_Control_కీలకం_State |= ఎడమ_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    తిరిగి TRUE;
}
# endif  // FEAT_EVAL

    స్థిర పూర్ణాంకం
write_ఆదానం_రికార్డు_బఫరు(ఆదానం_రికార్డు* irEvents, పూర్ణాంకం n_నిడివి)
{
    పూర్ణాంకం				n_లెక్క = 0;
    ఆదానం_రికార్డు_బఫరు_కణుపు_T	*event_కణుపు;

    అయితే (n_లెక్క < n_నిడివి)
    {
	event_కణుపు = alloc(పరిమాణం(ఆదానం_రికార్డు_బఫరు_కణుపు_T));
	ఒకవేళ (event_కణుపు == ఖాళీ)
	    తిరిగి -1;

	event_కణుపు->ir = irEvents[n_లెక్క++];
	event_కణుపు->తదుపరి = ఖాళీ;

	ఆదానం_రికార్డు_బఫరు.నిడివి++;
	ఒకవేళ (ఆదానం_రికార్డు_బఫరు.తోక == ఖాళీ)
	{
	    ఆదానం_రికార్డు_బఫరు.తల = event_కణుపు;
	    ఆదానం_రికార్డు_బఫరు.తోక = event_కణుపు;
	}
	లేకపోతే
	{
	    ఆదానం_రికార్డు_బఫరు.తోక->తదుపరి = event_కణుపు;
	    ఆదానం_రికార్డు_బఫరు.తోక = ఆదానం_రికార్డు_బఫరు.తోక->తదుపరి;
	}
    }
    తిరిగి n_లెక్క;
}

    స్థిర పూర్ణాంకం
read_ఆదానం_రికార్డు_బఫరు(ఆదానం_రికార్డు* irEvents, పూర్ణాంకం n_Max_నిడివి)
{
    పూర్ణాంకం n_లెక్క = 0;
    అయితే (n_లెక్క < n_Max_నిడివి && ఆదానం_రికార్డు_బఫరు.తల != ఖాళీ)
    {
	ఆదానం_రికార్డు_బఫరు.నిడివి--;
	ఆదానం_రికార్డు_బఫరు_కణుపు_T *తీయి_తల = ఆదానం_రికార్డు_బఫరు.తల;
	irEvents[n_లెక్క++] = తీయి_తల->ir;
	ఆదానం_రికార్డు_బఫరు.తల = తీయి_తల->తదుపరి;
	vim_free(తీయి_తల);
	ఒకవేళ (ఆదానం_రికార్డు_బఫరు.నిడివి == 0)
	    ఆదానం_రికార్డు_బఫరు.తోక = ఖాళీ;
    }
    తిరిగి n_లెక్క;
}
#లేకపోతే  // FEAT_GUI_MSWIN && !VIMDLL
    శూన్య
mch_setmouse(పూర్ణాంకం on వాడని)
{
}
#అయితే_ముగింపు // FEAT_GUI_MSWIN && !VIMDLL

#నిర్వచితమైతే FEAT_EVAL
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
    పూర్ణాంకం
test_mswin_event(char_u *event, dict_T *వాదనలు)
{
    పూర్ణాంకం lpEventsWritten = 0;

# ఒకవేళ defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    ఒకవేళ (gui.లోపల_use)
	తిరిగి test_gui_w32_sendevent(event, వాదనలు);
# endif

# ఒకవేళ defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    ఆదానం_రికార్డు ir;
    BOOL ఆదానం_encoded = FALSE;
    BOOL execute = FALSE;
    ఒకవేళ (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(వాదనలు, "execute", FALSE);
	ఒకవేళ (dict_has_కీలకం(వాదనలు, "event"))
	    ఆదానం_encoded = encode_కీలకం_event(వాదనలు, &ir);
	లేకపోతే ఒకవేళ (!execute)
	{
	    semsg(_(e_missing_వాదం_str), "event");
	    తిరిగి FALSE;
	}
    }
    లేకపోతే ఒకవేళ (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	ఆదానం_encoded = encode_mouse_event(వాదనలు, &ir);
    }
    లేకపోతే
    {
	semsg(_(e_invalid_విలువ_for_వాదం_str_str), "event", event);
	తిరిగి FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    ఒకవేళ (ఆదానం_encoded)
    {
	ఒకవేళ ((lpEventsWritten = write_ఆదానం_రికార్డు_బఫరు(&ir, 1)) < 0)
	{
	    semsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం), "event");
	    తిరిగి FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    ఒకవేళ (execute)
    {
	పూర్ణాంకం save_సందేశం_scroll = సందేశం_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	సందేశం_scroll = FALSE;
	ch_log(ఖాళీ, "test_mswin_event() executing");
	exec_సాధారణ(TRUE, TRUE, TRUE);
	సందేశం_scroll |= save_సందేశం_scroll;
    }

# endif
    తిరిగి lpEventsWritten;
}
#అయితే_ముగింపు // FEAT_EVAL

#ఒకవేళ defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    స్థిర శూన్య
mch_సమితి_cursor_shape(పూర్ణాంకం thickness)
{
    ఒకవేళ (vtp_working)
    {
	ఒకవేళ (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    ఒకవేళ (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    లేకపోతే
		vtp_printf("\033[0 q");	// default
	}
    }
    లేకపోతే
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dw_కొలత = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	సమితి_Console_Cursor_Info(g_h_Con_బహిర్గతం, &ConsoleCursorInfo);
	ఒకవేళ (s_cursor_visible)
	    సమితి_Console_Cursor_Position(g_h_Con_బహిర్గతం, g_coord);
    }
}

    శూన్య
mch_update_cursor(శూన్య)
{
    పూర్ణాంకం		idx;
    పూర్ణాంకం		thickness;

# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
	తిరిగి;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    ఒకవేళ (shape_పట్టిక[idx].shape == SHAPE_ఖండం)
	thickness = 99;	// 100 doesn't work on W95
    లేకపోతే
	thickness = shape_పట్టిక[idx].percentage;
    mch_సమితి_cursor_shape(thickness);
}
#అయితే_ముగింపు

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    స్థిర శూన్య
handle_focus_event(ఆదానం_రికార్డు ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.b_సమితి_Focus;
    ui_focus_change((పూర్ణాంకం)g_fJustGotFocus);
}

స్థిర శూన్య ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    స్థిర పూర్ణాంకం
WaitForChar(దీర్ఘ msec, పూర్ణాంకం ignore_ఆదానం)
{
    DWORD	    dwNow = 0, dw_చివర_సమయం = 0;
    ఆదానం_రికార్డు    ir;
    DWORD	    c_రికార్డులు;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    పూర్ణాంకం		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    ఒకవేళ (msec > 0)
	// Wait until the specified time has elapsed.
	dw_చివర_సమయం = Get_Tick_లెక్క() + msec;
    లేకపోతే ఒకవేళ (msec < 0)
	// Wait forever.
	dw_చివర_సమయం = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    కొరకు (;;)
    {
	// Only process messages when waiting.
	ఒకవేళ (msec != 0)
	{
# ifdef సందేశం_వరుస
	    parse_queued_messages();
# endif
# ifdef FEAT_MZSCHEME
	    mzvim_check_దారాలు();
# endif
# ifdef FEAT_CLIENTSERVER
	    serverProcessPendingMessages();
# endif
	}

	ఒకవేళ (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_ఆదానం && ఆదానం_అందుబాటు())
# endif
	   )
	    తిరిగి TRUE;

	ఒకవేళ (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_లెక్క();
	    ఒకవేళ ((పూర్ణాంకం)(dwNow - dw_చివర_సమయం) >= 0)
		ఆపు;
	}
	ఒకవేళ (msec != 0)
	{
	    DWORD dw_Wait_సమయం = dw_చివర_సమయం - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    ఒకవేళ (dw_Wait_సమయం > 11)
		dw_Wait_సమయం = 11;

# ifdef FEAT_MZSCHEME
	    ఒకవేళ (mzthreads_allowed() && p_mzq > 0 && (దీర్ఘ)dw_Wait_సమయం > p_mzq)
		dw_Wait_సమయం = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    ఒకవేళ (dw_Wait_సమయం > 10)
	    {
		దీర్ఘ	due_సమయం;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_సమయం = check_due_timer();
		ఒకవేళ (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    తిరిగి FALSE;
		}
		ఒకవేళ (due_సమయం > 0 && dw_Wait_సమయం > (DWORD)due_సమయం)
		    dw_Wait_సమయం = due_సమయం;
	    }
# endif
	    ఒకవేళ (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    సందేశం_wait_for_గుణిజం_objects(1, &g_h_Con_లోపల, FALSE,
				  dw_Wait_సమయం, QS_SENDMESSAGE) != WAIT_OBJECT_0
# లేకపోతే
		    wait_for_single_object(g_h_Con_లోపల, dw_Wait_సమయం)
							      != WAIT_OBJECT_0
# endif
		    )
		కొనసాగు;
	}

	c_రికార్డులు = 0;
	peek_console_ఆదానం(g_h_Con_లోపల, &ir, 1, &c_రికార్డులు);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	ఒకవేళ (State == బహుళకం_CMDLINE && సందేశం_అడ్డువరుస == అడ్డువరుసలు - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_బఫరు_INFO csbi;

	    ఒకవేళ (Get_Console_Screen_బఫరు_Info(g_h_Con_బహిర్గతం, &csbi))
	    {
		ఒకవేళ (csbi.dwCursorPosition.Y != సందేశం_అడ్డువరుస)
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

	ఒకవేళ (c_రికార్డులు > 0)
	{
	    ఒకవేళ (ir.EventType == కీలకం_EVENT && ir.Event.కీలకం_Event_2.b_కీలకం_కిందికి)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		ఒకవేళ (ir.Event.కీలకం_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.కీలకం_Event_2.w_Virtual_కీలకం_సంకేతం == 13)
		{
		    read_console_ఆదానం(g_h_Con_లోపల, &ir, 1, &c_రికార్డులు);
		    కొనసాగు;
		}
# endif
		ఒకవేళ (decode_కీలకం_event(&ir.Event.కీలకం_Event_2, &ch, &ch2,
								 ఖాళీ, FALSE))
		    తిరిగి TRUE;
	    }

	    read_console_ఆదానం(g_h_Con_లోపల, &ir, 1, &c_రికార్డులు);

	    ఒకవేళ (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    లేకపోతే ఒకవేళ (ir.EventType == కిటికీ_బఫరు_కొలత_EVENT)
	    {
		COORD dw_కొలత = ir.Event.కిటికీ_బఫరు_కొలత_Event.dw_కొలత;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		ఒకవేళ (dw_కొలత.X != Columns || dw_కొలత.Y != అడ్డువరుసలు)
		{
		    CONSOLE_SCREEN_బఫరు_INFO csbi;
		    Get_Console_Screen_బఫరు_Info(g_h_Con_బహిర్గతం, &csbi);
		    dw_కొలత.X = csbi.sr_కిటికీ.కుడి_4 - csbi.sr_కిటికీ.ఎడమ_4 + 1;
		    dw_కొలత.Y = csbi.sr_కిటికీ.Bottom - csbi.sr_కిటికీ.శిఖరం_2 + 1;
		    ఒకవేళ (dw_కొలత.X != Columns || dw_కొలత.Y != అడ్డువరుసలు)
		    {
			ResizeConBuf(g_h_Con_బహిర్గతం, dw_కొలత);
			shell_resized();
		    }
		}
	    }
	    లేకపోతే ఒకవేళ (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		తిరిగి TRUE;
	}
	లేకపోతే ఒకవేళ (msec == 0)
	    ఆపు;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    ఒకవేళ (ఆదానం_అందుబాటు())
	తిరిగి TRUE;
# endif

    తిరిగి FALSE;
}

/*
 * return non-zero if a character is available
 */
    పూర్ణాంకం
mch_char_avail(శూన్య)
{
# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
	తిరిగి TRUE;
# endif
    తిరిగి WaitForChar(0L, FALSE);
}

# ఒకవేళ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    పూర్ణాంకం
mch_check_messages(శూన్య)
{
#  ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
	తిరిగి TRUE;
#  endif
    తిరిగి WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    స్థిర శూన్య
create_conin(శూన్య)
{
    g_h_Con_లోపల =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_లక్షణాలు) ఖాళీ,
			OPEN_EXISTING, 0, (HANDLE)ఖాళీ);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    స్థిర WCHAR
tgetch(పూర్ణాంకం *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    కొరకు (;;)
    {
	ఆదానం_రికార్డు ir;
	DWORD c_రికార్డులు = 0;

# ifdef FEAT_CLIENTSERVER
	(శూన్య)WaitForChar(-1L, FALSE);
	ఒకవేళ (ఆదానం_అందుబాటు())
	    తిరిగి 0;
	ఒకవేళ (g_nMouseClick != -1)
	    తిరిగి 0;
# endif
	ఒకవేళ (read_console_ఆదానం(g_h_Con_లోపల, &ir, 1, &c_రికార్డులు) == 0)
	{
	    ఒకవేళ (did_create_conin)
		read_error_exit();
	    create_conin();
	    కొనసాగు;
	}

	ఒకవేళ (ir.EventType == కీలకం_EVENT)
	{
	    ఒకవేళ (decode_కీలకం_event(&ir.Event.కీలకం_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		తిరిగి ch;
	}
	లేకపోతే ఒకవేళ (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	లేకపోతే ఒకవేళ (ir.EventType == కిటికీ_బఫరు_కొలత_EVENT)
	    shell_resized();
	లేకపోతే ఒకవేళ (ir.EventType == MOUSE_EVENT)
	{
	    ఒకవేళ (decode_mouse_event(&ir.Event.MouseEvent))
		తిరిగి 0;
	}
    }
}
#అయితే_ముగింపు // !FEAT_GUI_MSWIN


/*
 * mch_inchar(): low-level input function.
 * Get one or more characters from the keyboard or the mouse.
 * If time == 0, do not wait for characters.
 * If time == n, wait a short time for characters.
 * If time == -1, wait forever for characters.
 * Returns the number of characters read into buf.
 */
    పూర్ణాంకం
mch_inchar(
    char_u	*buf వాడని,
    పూర్ణాంకం		maxlen వాడని,
    దీర్ఘ	సమయం వాడని,
    పూర్ణాంకం		tb_change_cnt వాడని)
{
#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    పూర్ణాంకం		len;
    పూర్ణాంకం		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# లేకపోతే
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    స్థిర char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    స్థిర పూర్ణాంకం	    typeaheadlen = 0;

# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
	తిరిగి 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    ఒకవేళ (typeaheadlen > 0)
	వెళ్ళు theend;

    ఒకవేళ (సమయం >= 0)
    {
	ఒకవేళ (!WaitForChar(సమయం, FALSE))     // no character available
	    తిరిగి 0;
    }
    లేకపోతే    // time == -1, wait forever
    {
	mch_సమితి_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ఒకవేళ (!WaitForChar(p_ut, FALSE))
	{
	    ఒకవేళ (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (పూర్ణాంకం)KE_CURSORHOLD;
		తిరిగి 3;
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
    ఒకవేళ (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    అయితే ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	ఒకవేళ (typebuf_మారింది(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    ఆపు;
	}
	ఒకవేళ (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    ఒకవేళ (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    ఒకవేళ (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		ఒకవేళ (modifiers > 0)
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
	    లేకపోతే
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
	లేకపోతే
	{
	    WCHAR	ch2 = NUL;
	    పూర్ణాంకం		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_కీలకం(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_కీలకాలు;
	    c = may_adjust_కీలకం_for_ctrl(modifiers, c);
	    --no_reduce_కీలకాలు;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_జరుపు_modifier(modifiers, c);

	    ఒకవేళ (typebuf_మారింది(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		ఆపు;
	    }

	    ఒకవేళ (c == Ctrl_C && ctrl_c_interrupts)
	    {
# ఒకవేళ defined(FEAT_CLIENTSERVER)
		trash_ఆదానం_buf();
# endif
		got_int = TRUE;
	    }

	    ఒకవేళ (g_nMouseClick == -1)
	    {
		పూర్ణాంకం	n = 1;

		ఒకవేళ (ch2 == NUL)
		{
		    పూర్ణాంకం	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    ఒకవేళ (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_గమ్యం_enc(ch, &n);
		    ఒకవేళ (p != ఖాళీ)
		    {
			కొరకు (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    ఒకవేళ (p[i] == CSI)
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
		లేకపోతే
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    ఒకవేళ (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		ఒకవేళ (ch2 != NUL)
		{
		    ఒకవేళ (c == K_NUL)
		    {
			ఎంపిక (ch2)
			{
			స్థితి (WCHAR)'\324': // SHIFT+Insert
			స్థితి (WCHAR)'\325': // CTRL+Insert
			స్థితి (WCHAR)'\327': // SHIFT+Delete
			స్థితి (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    ఆపు;

			అప్రమేయ:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    ఆపు;
			}
		    }
		    లేకపోతే
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
		ఒకవేళ ((modifiers & MOD_ముసుగు_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_ముసుగు_ALT;
		}

		ఒకవేళ (modifiers != 0)
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
		ఒకవేళ (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    ఒకవేళ (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    అయితే (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    ఒకవేళ (len > 0)
    {
	buf[len] = NUL;
	ch_log(ఖాళీ, "raw key input: \"%s\"", buf);
    }
# endif
    తిరిగి len;

#లేకపోతే // FEAT_GUI_MSWIN && !VIMDLL
    తిరిగి 0;
#అయితే_ముగింపు // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    స్థిర పూర్ణాంకం
executable_file(అక్షరం *name, char_u **మార్గం)
{
    పూర్ణాంకం attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    ఒకవేళ (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	తిరిగి FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    ఒకవేళ (attrs & FILE_ATTRIBUTE_REPARSE_బిందువు)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	ఒకవేళ (res == ఖాళీ)
	    res = resolve_reparse_బిందువు((char_u *)name);
	ఒకవేళ (res == ఖాళీ)
	    తిరిగి FALSE;
	// The path is already absolute.
	ఒకవేళ (మార్గం != ఖాళీ)
	    *మార్గం = res;
	లేకపోతే
	    vim_free(res);
    }
    లేకపోతే ఒకవేళ (మార్గం != ఖాళీ)
	*మార్గం = నిండిన_Name_save((char_u *)name, FALSE);
    తిరిగి TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    స్థిర bool
executable_ఉపసర్గ_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_కనుగొను_DATAW	దత్తాంశం;
    HANDLE		handle;
    DWORD		error;

    wn = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (wn == ఖాళీ)
	తిరిగి true;
    handle = కనుగొను_మొదటి_File_Ex_W(wn, కనుగొను_Ex_Info_Basic, &దత్తాంశం,
					    కనుగొను_Ex_శోధన_Name_సరిపోలిక, ఖాళీ, 0);
    error = Get_చివరి_Error();
    vim_free(wn);
    ఒకవేళ (handle != INVALID_HANDLE_విలువ)
    {
	కనుగొను_Close(handle);
	తిరిగి true;
    }
    తిరిగి error != ERROR_FILE_NOT_దొరికింది && error != ERROR_మార్గం_NOT_దొరికింది;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    స్థిర పూర్ణాంకం
executable_exists(
    అక్షరం	*name,
    size_t	namelen,
    char_u	**మార్గం,
    పూర్ణాంకం		use_మార్గం,
    పూర్ణాంకం		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[MAX_మార్గం_2 * 3];
    char_u	*p;
    string_T	pathbuf = {ఖాళీ, 0};
    పూర్ణాంకం		pathbuf_allocated = FALSE;
    string_T	pathext = {ఖాళీ, 0};
    పూర్ణాంకం		pathext_allocated = FALSE;
    char_u	*shname = ఖాళీ;
    పూర్ణాంకం		noext = FALSE;
    bool	simple_pathext = true;
    పూర్ణాంకం		ext_లెక్క = 0;
    పూర్ణాంకం		retval = FALSE;

    ఒకవేళ (namelen >= పరిమాణం(buf))	// safety check
	తిరిగి FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)shname, "sh") != ఖాళీ &&
	!(అక్షరశ్రేణి_శోధన((అక్షరం *)shname, "powershell") != ఖాళీ
				    || అక్షరశ్రేణి_శోధన((అక్షరం *)shname, "pwsh") != ఖాళీ))
	noext = TRUE;

    ఒకవేళ (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	ఒకవేళ (pathext.string == ఖాళీ)
	    STR_LITERAL_సమితి(pathext, ".com;.exe;.bat;.cmd");
	లేకపోతే
	    pathext.నిడివి = STRLEN(pathext.string);

	ఒకవేళ (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    అయితే (*p)
	    {
		ఒకవేళ (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    ఒకవేళ (*p == '.')
			simple_pathext = false;
		    ++p;
		    కొనసాగు;
		}
		e = vim_strchr(p, ';');
		ఒకవేళ (e == ఖాళీ)
		    e = pathext.string + pathext.నిడివి;
		plen = (size_t)(e - p);
		++ext_లెక్క;
		ఒకవేళ (*p != '.')
		    simple_pathext = false;
		కొరకు (q = p + 1; q < e; ++q)
		    ఒకవేళ (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		ఒకవేళ (_strnicoll(name + namelen - plen, (అక్షరం *)p, plen) == 0)
		{
		    noext = TRUE;
		    ఆపు;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    ఒకవేళ (pathext.string == ఖాళీ)
	STR_LITERAL_సమితి(pathext, ".");
    లేకపోతే ఒకవేళ (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.నిడివి + 3);
	ఒకవేళ (tmp == ఖాళీ)
	{
	    retval = FALSE;
	    వెళ్ళు theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.నిడివి += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    ఒకవేళ (use_మార్గం && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	ఒకవేళ (p != ఖాళీ)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    ఒకవేళ (pathbuf.string == ఖాళీ)
	    {
		retval = FALSE;
		వెళ్ళు theend;
	    }

	    ఒకవేళ (mch_getenv("NoDefaultCurrentDirectoryInExePath") == ఖాళీ &&
		    అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(p_sh), "cmd.exe") != ఖాళీ)
	    {
		STRCPY(pathbuf.string, ".;");
		pathbuf.నిడివి = 2;
	    }
	    STRCPY(pathbuf.string + pathbuf.నిడివి, p);
	    pathbuf.నిడివి += plen;
	    pathbuf_allocated = TRUE;
	}
    }

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    ఒకవేళ (pathbuf.string == ఖాళీ)
	STR_LITERAL_సమితి(pathbuf, ".");
    p = pathbuf.string;
    అయితే (*p)
    {
	char_u	*e;
	size_t	buflen;

	ఒకవేళ (*p == ';') // Skip empty entry
	{
	    ++p;
	    కొనసాగు;
	}
	e = vim_strchr(p, ';');
	ఒకవేళ (e == ఖాళీ)
	    e = pathbuf.string + pathbuf.నిడివి;

	ఒకవేళ (e - p + namelen + 2 > పరిమాణం(buf))
	{
	    retval = FALSE;
	    వెళ్ళు theend;
	}
	// A single "." that means current dir.
	ఒకవేళ (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	లేకపోతే
	{
	    buflen = vim_snprintf_safelen(
		(అక్షరం *)buf,
		పరిమాణం(buf),
		"%.*s%s%s", (పూర్ణాంకం)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	ఒకవేళ (!noext && simple_pathext && ext_లెక్క >= 3 && buflen > 0
		&& buflen + 2 <= పరిమాణం(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == ఖాళీ
		&& vim_strchr(gettail(buf), ':') == ఖాళీ)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_ఉపసర్గ_exists(buf);
	    buf[buflen] = NUL;
	    ఒకవేళ (!exists)
	    {
		p = e;
		కొనసాగు;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	అయితే (*p)
	{
	    char_u  *e2;

	    ఒకవేళ (*p == ';')
	    {
		// Skip empty entry
		++p;
		కొనసాగు;
	    }
	    e2 = vim_strchr(p, ';');
	    ఒకవేళ (e2 == ఖాళీ)
		e2 = pathext.string + pathext.నిడివి;

	    ఒకవేళ (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		ఒకవేళ (e2 - p + buflen + 1 > పరిమాణం(buf))
		{
		    retval = FALSE;
		    వెళ్ళు theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    ఒకవేళ (executable_file((అక్షరం *)buf, మార్గం))
	    {
		retval = TRUE;
		వెళ్ళు theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    ఒకవేళ (pathbuf_allocated)
	విడుదల(pathbuf.string);
    ఒకవేళ (pathext_allocated)
	విడుదల(pathext.string);
    తిరిగి retval;
}

#ఒకవేళ (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    స్థిర శూన్య
bad_param_handler(స్థిరరాశి wchar_t *వ్యక్తీకరణ వాడని,
    స్థిరరాశి wchar_t *function వాడని,
    స్థిరరాశి wchar_t *file వాడని,
    గుర్తులేని పూర్ణాంకం పంక్తి వాడని,
    uintptr_t pReserved వాడని)
{
}

# define సమితి_INVALID_PARAM_HANDLER \
	((శూన్య)సమితి_invalid_parameter_handler(bad_param_handler))
#లేకపోతే
# define సమితి_INVALID_PARAM_HANDLER
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    స్థిర శూన్య
mch_init_g(శూన్య)
{
# ifndef __MINGW32__
    బాహ్య పూర్ణాంకం _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    సమితి_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    సమితి_Error_బహుళకం(SEM_FAILCRITICALERRORS);

    _fmode = O_ద్విమాన;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    అడ్డువరుసలు = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[MAX_మార్గం_2 + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	ఒకవేళ (exe_pathlen + 10 >= పరిమాణం(vimrun_location))
	{
	    ఒకవేళ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ఖాళీ, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	లేకపోతే
	{
	    // First try in same directory as gvim.exe.
	    ఒకవేళ (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    ఒకవేళ (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		ఒకవేళ (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(అక్షరం *)vimrun_location,
			పరిమాణం(vimrun_location),
			"\"%.*svimrun\" ",
			(పూర్ణాంకం)exe_pathlen, exe_name);
		}
		లేకపోతే
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		ఒకవేళ (p != ఖాళీ)
		{
		    vimrun_మార్గం.string = p;
		    vimrun_మార్గం.నిడివి = plen;
		    vimrun_మార్గం_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    లేకపోతే ఒకవేళ (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    ఖాళీ, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	ఒకవేళ (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    ఒకవేళ (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    ఖాళీ, TRUE, FALSE))
	సమితి_option_విలువ_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#అయితే_ముగింపు // FEAT_GUI_MSWIN

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).కుడి_4 - (sr).ఎడమ_4 + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).శిఖరం_2 + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    స్థిర BOOL
Clear_Console_బఫరు(మాట_2 wAttribute)
{
    CONSOLE_SCREEN_బఫరు_INFO csbi;
    COORD coord;
    DWORD అంకె_Cells, dummy;

    ఒకవేళ (!Get_Console_Screen_బఫరు_Info(g_h_Con_బహిర్గతం, &csbi))
	తిరిగి FALSE;

    అంకె_Cells = csbi.dw_కొలత.X * csbi.dw_కొలత.Y;
    coord.X = 0;
    coord.Y = 0;
    ఒకవేళ (!FillConsoleOutputCharacter(g_h_Con_బహిర్గతం, ' ', అంకె_Cells,
	    coord, &dummy))
	తిరిగి FALSE;
    ఒకవేళ (!FillConsoleOutputAttribute(g_h_Con_బహిర్గతం, wAttribute, అంకె_Cells,
	    coord, &dummy))
	తిరిగి FALSE;

    తిరిగి TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    స్థిర BOOL
Fit_Console_కిటికీ(
    COORD dw_బఫరు_కొలత,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_బఫరు_INFO csbi;
    COORD dw_కిటికీ_కొలత;
    BOOL NeedAdjust = FALSE;

    ఒకవేళ (!Get_Console_Screen_బఫరు_Info(g_h_Con_బహిర్గతం, &csbi))
	తిరిగి FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    ఒకవేళ (csbi.sr_కిటికీ.కుడి_4 >= dw_బఫరు_కొలత.X)
    {
	dw_కిటికీ_కొలత.X = SRWIDTH(csbi.sr_కిటికీ);
	ఒకవేళ (dw_కిటికీ_కొలత.X > dw_బఫరు_కొలత.X)
	    dw_కిటికీ_కొలత.X = dw_బఫరు_కొలత.X;
	csbi.sr_కిటికీ.కుడి_4 = dw_బఫరు_కొలత.X - 1;
	csbi.sr_కిటికీ.ఎడమ_4 = dw_బఫరు_కొలత.X - dw_కిటికీ_కొలత.X;
	NeedAdjust = TRUE;
    }
    ఒకవేళ (csbi.sr_కిటికీ.Bottom >= dw_బఫరు_కొలత.Y)
    {
	dw_కిటికీ_కొలత.Y = SRHEIGHT(csbi.sr_కిటికీ);
	ఒకవేళ (dw_కిటికీ_కొలత.Y > dw_బఫరు_కొలత.Y)
	    dw_కిటికీ_కొలత.Y = dw_బఫరు_కొలత.Y;
	csbi.sr_కిటికీ.Bottom = dw_బఫరు_కొలత.Y - 1;
	csbi.sr_కిటికీ.శిఖరం_2 = dw_బఫరు_కొలత.Y - dw_కిటికీ_కొలత.Y;
	NeedAdjust = TRUE;
    }
    ఒకవేళ (NeedAdjust && WantAdjust)
    {
	ఒకవేళ (!సమితి_Console_కిటికీ_Info(g_h_Con_బహిర్గతం, TRUE, &csbi.sr_కిటికీ))
	    తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

రకనిర్వచనం నిర్మాణం Console_బఫరు_Struct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_బఫరు_INFO	Info;
    PCHAR_INFO			బఫరు_2;
    COORD			బఫరు_కొలత_2;
    PSMALL_RECT			Regions;
    పూర్ణాంకం				అంకె_Regions;
} Console_బఫరు;

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
    స్థిర BOOL
Save_Console_బఫరు(
    Console_బఫరు *cb)
{
    DWORD అంకె_Cells;
    COORD బఫరు_Coord;
    SMALL_RECT ReadRegion;
    మాట_2 Y, Y_incr;
    పూర్ణాంకం i, numregions;

    ఒకవేళ (cb == ఖాళీ)
	తిరిగి FALSE;

    ఒకవేళ (!Get_Console_Screen_బఫరు_Info(g_h_Con_బహిర్గతం, &cb->Info))
    {
	cb->IsValid = FALSE;
	తిరిగి FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    ఒకవేళ (use_alternate_screen_బఫరు)
	తిరిగి TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    ఒకవేళ (!cb->IsValid || cb->బఫరు_2 == ఖాళీ ||
	    cb->బఫరు_కొలత_2.X != cb->Info.dw_కొలత.X ||
	    cb->బఫరు_కొలత_2.Y != cb->Info.dw_కొలత.Y)
    {
	cb->బఫరు_కొలత_2.X = cb->Info.dw_కొలత.X;
	cb->బఫరు_కొలత_2.Y = cb->Info.dw_కొలత.Y;
	అంకె_Cells = cb->బఫరు_కొలత_2.X * cb->బఫరు_కొలత_2.Y;
	vim_free(cb->బఫరు_2);
	cb->బఫరు_2 = ALLOC_MULT(CHAR_INFO, అంకె_Cells);
	ఒకవేళ (cb->బఫరు_2 == ఖాళీ)
	    తిరిగి FALSE;
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
    బఫరు_Coord.X = 0;
    ReadRegion.ఎడమ_4 = 0;
    ReadRegion.కుడి_4 = cb->Info.dw_కొలత.X - 1;
    Y_incr = 12000 / cb->Info.dw_కొలత.X;

    numregions = (cb->Info.dw_కొలత.Y + Y_incr - 1) / Y_incr;
    ఒకవేళ (cb->Regions == ఖాళీ || numregions != cb->అంకె_Regions)
    {
	cb->అంకె_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->అంకె_Regions);
	ఒకవేళ (cb->Regions == ఖాళీ)
	{
	    VIM_CLEAR(cb->బఫరు_2);
	    తిరిగి FALSE;
	}
    }

    కొరకు (i = 0, Y = 0; i < cb->అంకె_Regions; i++, Y += Y_incr)
    {
	/*
	 * Read into position (0, Y) in our buffer.
	 */
	బఫరు_Coord.Y = Y;
	/*
	 * Read the region whose top left corner is (0, Y) and whose bottom
	 * right corner is (width - 1, Y + Y_incr - 1).  This should define
	 * a region of size width by Y_incr.  Don't worry if this region is
	 * too large for the remaining buffer; it will be cropped.
	 */
	ReadRegion.శిఖరం_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	ఒకవేళ (!ReadConsoleOutputW(g_h_Con_బహిర్గతం,	// output handle
		cb->బఫరు_2,			// our buffer
		cb->బఫరు_కొలత_2,			// dimensions of our buffer
		బఫరు_Coord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->బఫరు_2);
	    VIM_CLEAR(cb->Regions);
	    తిరిగి FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    తిరిగి TRUE;
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
    స్థిర BOOL
Restore_Console_బఫరు(
    Console_బఫరు   *cb,
    BOOL	    RestoreScreen)
{
    COORD బఫరు_Coord;
    SMALL_RECT WriteRegion;
    పూర్ణాంకం i;

    ఒకవేళ (cb == ఖాళీ || !cb->IsValid)
	తిరిగి FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    ఒకవేళ (RestoreScreen)
	Clear_Console_బఫరు(cb->Info.w_లక్షణాలు);

    Fit_Console_కిటికీ(cb->Info.dw_కొలత, TRUE);
    ఒకవేళ (!సమితి_Console_Screen_బఫరు_కొలత(g_h_Con_బహిర్గతం, cb->Info.dw_కొలత))
	తిరిగి FALSE;
    ఒకవేళ (!సమితి_Console_పాఠ్యం_Attribute(g_h_Con_బహిర్గతం, cb->Info.w_లక్షణాలు))
	తిరిగి FALSE;

    ఒకవేళ (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	తిరిగి TRUE;
    }

    ఒకవేళ (!సమితి_Console_Cursor_Position(g_h_Con_బహిర్గతం, cb->Info.dwCursorPosition))
	తిరిగి FALSE;
    ఒకవేళ (!సమితి_Console_కిటికీ_Info(g_h_Con_బహిర్గతం, TRUE, &cb->Info.sr_కిటికీ))
	తిరిగి FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    ఒకవేళ (use_alternate_screen_బఫరు)
	తిరిగి TRUE;

    /*
     * Restore the screen buffer contents.
     */
    ఒకవేళ (cb->బఫరు_2 != ఖాళీ)
    {
	కొరకు (i = 0; i < cb->అంకె_Regions; i++)
	{
	    బఫరు_Coord.X = cb->Regions[i].ఎడమ_4;
	    బఫరు_Coord.Y = cb->Regions[i].శిఖరం_2;
	    WriteRegion = cb->Regions[i];
	    ఒకవేళ (!WriteConsoleOutputW(g_h_Con_బహిర్గతం,	// output handle
			cb->బఫరు_2,		// our buffer
			cb->బఫరు_కొలత_2,		// dimensions of our buffer
			బఫరు_Coord,		// offset in our buffer
			&WriteRegion))		// region to restore
		తిరిగి FALSE;
	}
    }

    తిరిగి TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
స్థిర Console_బఫరు g_cbOrig = { 0 };
# endif
స్థిర Console_బఫరు g_cbNonTermcap = { 0 };
స్థిర Console_బఫరు g_cbTermcap = { 0 };

అక్షరం g_szOrigTitle[256] = { 0 };
HWND g_hWnd = ఖాళీ;	// also used in os_mswin.c
స్థిర HICON g_hOrigIconSmall = ఖాళీ;
స్థిర HICON g_hOrigIcon = ఖాళీ;
స్థిర HICON g_hVimIcon = ఖాళీ;
స్థిర BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    స్థిర BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    ఒకవేళ (hWnd == ఖాళీ)
	తిరిగి FALSE;

    ఒకవేళ (phIconSmall != ఖాళీ)
	*phIconSmall = (HICON)Send_సందేశం(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    ఒకవేళ (phIcon != ఖాళీ)
	*phIcon = (HICON)Send_సందేశం(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    తిరిగి TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    స్థిర BOOL
సమితి_Console_Icon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    ఒకవేళ (hWnd == ఖాళీ)
	తిరిగి FALSE;

    ఒకవేళ (hIconSmall != ఖాళీ)
	Send_సందేశం(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    ఒకవేళ (hIcon != ఖాళీ)
	Send_సందేశం(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    తిరిగి TRUE;
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
    స్థిర శూన్య
SaveConsoleTitleAndIcon(శూన్య)
{
    // Save the original title.
    ఒకవేళ (!GetConsoleTitle(g_szOrigTitle, పరిమాణం(g_szOrigTitle)))
	తిరిగి;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = Get_Console_కిటికీ();
    ఒకవేళ (g_hWnd == ఖాళీ)
	తిరిగి;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    ఒకవేళ (g_hOrigIconSmall == ఖాళీ || g_hOrigIcon == ఖాళీ)
	తిరిగి;

    // Extract the first icon contained in the Vim executable.
    ఒకవేళ (
# ifdef FEAT_LIBCALL
	    mch_icon_ఎక్కించు((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == ఖాళీ)
	g_hVimIcon = వెలికితీయి_Icon(ఖాళీ, (LPCSTR)exe_name, 0);
    ఒకవేళ (g_hVimIcon != ఖాళీ)
	g_fCanChangeIcon = TRUE;
}

స్థిర పూర్ణాంకం g_fWindInitCalled = FALSE;
స్థిర పూర్ణాంకం g_f_Termcap_బహుళకం = FALSE;
స్థిర CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    స్థిర శూన్య
mch_init_c(శూన్య)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_బఫరు_INFO csbi;
# endif
# ifndef __MINGW32__
    బాహ్య పూర్ణాంకం _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    సమితి_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    సమితి_Error_బహుళకం(SEM_FAILCRITICALERRORS);

    _fmode = O_ద్విమాన;		// we do our own CR-LF translation
    బహిర్గతం_flush();

    // Obtain handles for the standard Console I/O devices
    ఒకవేళ (read_cmd_fd == 0)
	g_h_Con_లోపల =  GetStdHandle(STD_ఆదానం_HANDLE);
    లేకపోతే
	create_conin();
    g_h_Con_బహిర్గతం = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    Save_Console_బఫరు(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.w_లక్షణాలు;
# లేకపోతే
    // Get current text attributes
    Get_Console_Screen_బఫరు_Info(g_h_Con_బహిర్గతం, &csbi);
    g_attrCurrent = g_attrDefault = csbi.w_లక్షణాలు;
# endif
    ఒకవేళ (cterm_సాధారణ_fg_color == 0)
	cterm_సాధారణ_fg_color = (g_attrCurrent & 0xf) + 1;
    ఒకవేళ (cterm_సాధారణ_bg_color == 0)
	cterm_సాధారణ_bg_color = ((g_attrCurrent >> 4) & 0xf) + 1;

    // Fg and Bg color index number at startup
    g_color_సూచీ_fg = g_attrDefault & 0xf;
    g_color_సూచీ_bg = (g_attrDefault >> 4) & 0xf;

    // set termcap codes to current text attributes
    update_tcap(g_attrCurrent);

    GetConsoleCursorInfo(g_h_Con_బహిర్గతం, &g_cci);
    Get_Console_బహుళకం(g_h_Con_లోపల,  &g_cmodein);
    Get_Console_బహుళకం(g_h_Con_బహిర్గతం, &g_cmodeout);

    SaveConsoleTitleAndIcon();
    /*
     * Set both the small and big icons of the console window to Vim's icon.
     * Note that Vim presently only has one size of icon (32x32), but it
     * automatically gets scaled down to 16x16 when setting the small icon.
     */
    ఒకవేళ (g_fCanChangeIcon)
	సమితి_Console_Icon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    ఒకవేళ (use_alternate_screen_బఫరు)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    ఒకవేళ (fdDump)
    {
	time_t t;

	సమయం(&t);
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
    స్థిర శూన్య
mch_exit_c(పూర్ణాంకం r)
{
    // Copy flag since stoptermcap() will clear the flag.
    పూర్ణాంకం f_Termcap_బహుళకం = g_f_Termcap_బహుళకం;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    ఒకవేళ (!f_Termcap_బహుళకం &&  use_alternate_screen_బఫరు)
	vtp_printf("\033[?1049l");

    ఒకవేళ (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    ఒకవేళ (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	ఒకవేళ (g_fCanChangeIcon && !g_fForceExit)
	    సమితి_Console_Icon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	ఒకవేళ (fdDump)
	{
	    time_t t;

	    సమయం(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = ఖాళీ;
# endif
    }

    సమితి_Console_Cursor_Info(g_h_Con_బహిర్గతం, &g_cci);
    సమితి_Console_బహుళకం(g_h_Con_లోపల,  g_cmodein | ENABLE_EXTENDED_FLAGS);
    సమితి_Console_బహుళకం(g_h_Con_బహిర్గతం, g_cmodeout);

# ifdef DYNAMIC_GETTEXT
    dyn_libintl_చివర();
# endif

    నిష్క్రమించు(r);
}
#అయితే_ముగింపు // !FEAT_GUI_MSWIN || VIMDLL

    శూన్య
mch_init(శూన్య)
{
#నిర్వచితమైతే VIMDLL
    ఒకవేళ (gui.starting)
	mch_init_g();
    లేకపోతే
	mch_init_c();
#లేదావేళ defined(FEAT_GUI_MSWIN)
    mch_init_g();
#లేకపోతే
    mch_init_c();
#అయితే_ముగింపు
}

    శూన్య
mch_exit(పూర్ణాంకం r)
{
#నిర్వచితమైతే FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#అయితే_ముగింపు

#నిర్వచితమైతే VIMDLL
    ఒకవేళ (vimrun_మార్గం_allocated)
	vim_free(vimrun_మార్గం.string);

    ఒకవేళ (gui.లోపల_use || gui.starting)
	mch_exit_g(r);
    లేకపోతే
	mch_exit_c(r);
#లేదావేళ defined(FEAT_GUI_MSWIN)
    ఒకవేళ (vimrun_మార్గం_allocated)
	vim_free(vimrun_మార్గం.string);
    mch_exit_g(r);
#లేకపోతే
    mch_exit_c(r);
#అయితే_ముగింపు
}

/*
 * Do we have an interactive window?
 */
    పూర్ణాంకం
mch_check_win(
    పూర్ణాంకం argc వాడని,
    అక్షరం **argv వాడని)
{
    mch_get_exe_name();

#ఒకవేళ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    తిరిగి OK;	    // GUI always has a tty
#లేకపోతే
# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
	తిరిగి OK;
# endif
    ఒకవేళ (isatty(1))
	తిరిగి OK;
    తిరిగి FAIL;
#అయితే_ముగింపు
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    శూన్య
fname_case(
    char_u	*name,
    పూర్ణాంకం		len)
{
    WCHAR   *p;
    WCHAR   buf[MAX_మార్గం_2 + 1];

    ఒకవేళ (*name == NUL)
	తిరిగి;

    slash_adjust(name);

    p = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    ఒకవేళ (Get_Long_మార్గం_Name_W(p, buf, MAX_మార్గం_2))
    {
	char_u	*q = utf16_గమ్యం_enc(buf, ఖాళీ);

	ఒకవేళ (q != ఖాళీ)
	{
	    ఒకవేళ (len > 0)
		vim_strncpy(name, q, len - 1);
	    లేకపోతే
	    {
		size_t  namelen = STRLEN(name);

		ఒకవేళ (namelen >= STRLEN(q))
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
    పూర్ణాంకం
mch_get_user_name(
    char_u  *s,
    పూర్ణాంకం	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_నిడివి(wszUserName);

    ఒకవేళ (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_గమ్యం_enc(wszUserName, ఖాళీ);

	ఒకవేళ (p != ఖాళీ)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    తిరిగి OK;
	}
    }
    s[0] = NUL;
    తిరిగి FAIL;
}


/*
 * Insert host name in s[len].
 */
    శూన్య
mch_get_host_name(
    char_u	*s,
    పూర్ణాంకం		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_నిడివి(wszHostName);

    ఒకవేళ (!GetComputerNameW(wszHostName, &wcch))
	తిరిగి;

    char_u  *p = utf16_గమ్యం_enc(wszHostName, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    దీర్ఘ
mch_get_pid(శూన్య)
{
    తిరిగి (దీర్ఘ)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    పూర్ణాంకం
mch_process_running(దీర్ఘ pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   స్థితి_2 = 0;
    పూర్ణాంకం	    ret = FALSE;

    ఒకవేళ (hProcess == ఖాళీ)
	తిరిగి FALSE;  // might not have access
    ఒకవేళ (Get_Exit_సంకేతం_Process(hProcess, &స్థితి_2))
	ret = స్థితి_2 == STILL_చురుకైన;
    CloseHandle(hProcess);
    తిరిగి ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    పూర్ణాంకం
mch_dirname(
    char_u	*buf,
    పూర్ణాంకం		len)
{
    WCHAR   wbuf[MAX_మార్గం_2 + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    ఒకవేళ (GetCurrentDirectoryW(MAX_మార్గం_2, wbuf) == 0)
	తిరిగి FAIL;

    WCHAR   wcbuf[MAX_మార్గం_2 + 1];
    char_u  *p = ఖాళీ;

    ఒకవేళ (Get_Long_మార్గం_Name_W(wbuf, wcbuf, MAX_మార్గం_2) != 0)
    {
	p = utf16_గమ్యం_enc(wcbuf, ఖాళీ);
	ఒకవేళ (p != ఖాళీ)
	{
	    ఒకవేళ (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    ఒకవేళ (p == ఖాళీ)
	p = utf16_గమ్యం_enc(wbuf, ఖాళీ);

    ఒకవేళ (p == ఖాళీ)
	తిరిగి FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    తిరిగి OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    దీర్ఘ
mch_getperm(char_u *name)
{
    stat_T	st;

    తిరిగి (mch_stat((అక్షరం *)name, &st) == 0)
	? (దీర్ఘ)(గుర్తులేని పొట్టి)st.st_బహుళకం
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
mch_setperm(char_u *name, దీర్ఘ perm)
{
    దీర్ఘ	n;
    WCHAR	*p;

    p = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    ఒకవేళ (n == -1)
	తిరిగి FAIL;

    win32_సమితి_archive(name);

    తిరిగి OK;
}

/*
 * Set hidden flag for "name".
 */
    శూన్య
mch_hide(char_u *name)
{
    పూర్ణాంకం attrs = win32_getattrs(name);
    ఒకవేళ (attrs == -1)
	తిరిగి;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    పూర్ణాంకం
mch_ishidden(char_u *name)
{
    పూర్ణాంకం f = win32_getattrs(name);

    ఒకవేళ (f == -1)
	తిరిగి FALSE;		    // file does not exist at all

    తిరిగి (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    పూర్ణాంకం
mch_isdir(char_u *name)
{
    పూర్ణాంకం f = win32_getattrs(name);

    ఒకవేళ (f == -1)
	తిరిగి FALSE;		    // file does not exist at all

    తిరిగి (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    పూర్ణాంకం
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి FALSE;
    attr = Get_File_లక్షణాలు_W(p);
    ఒకవేళ (attr == INVALID_FILE_లక్షణాలు)
	error = Get_చివరి_Error();
    vim_free(p);

    ఒకవేళ (attr == INVALID_FILE_లక్షణాలు)
	తిరిగి error == ERROR_FILE_NOT_దొరికింది || error == ERROR_మార్గం_NOT_దొరికింది;
    తిరిగి (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    పూర్ణాంకం
mch_isrealdir(char_u *name)
{
    తిరిగి mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    పూర్ణాంకం
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    పూర్ణాంకం	    retval;

    p = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి -1;
    retval = _wmkdir(p);
    vim_free(p);
    తిరిగి retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    పూర్ణాంకం
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    పూర్ణాంకం	    retval;

    p = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి -1;
    retval = _wrmdir(p);
    vim_free(p);
    తిరిగి retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    పూర్ణాంకం
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    తిరిగి win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_సంఖ్య_యొక్క_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    పూర్ణాంకం
mch_is_symbolic_link(char_u *name)
{
    HANDLE		h_కనుగొను;
    పూర్ణాంకం			res = FALSE;
    DWORD		fileFlags = 0, reparse_ట్యాగ్ = 0;
    WCHAR		*wn;
    WIN32_కనుగొను_DATAW	కనుగొను_దత్తాంశం_W;

    wn = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (wn == ఖాళీ)
	తిరిగి FALSE;

    h_కనుగొను = కనుగొను_మొదటి_File_W(wn, &కనుగొను_దత్తాంశం_W);
    vim_free(wn);
    ఒకవేళ (h_కనుగొను != INVALID_HANDLE_విలువ)
    {
	fileFlags = కనుగొను_దత్తాంశం_W.dw_File_లక్షణాలు;
	reparse_ట్యాగ్ = కనుగొను_దత్తాంశం_W.dwReserved0;
	కనుగొను_Close(h_కనుగొను);
    }

    ఒకవేళ ((fileFlags & FILE_ATTRIBUTE_REPARSE_బిందువు)
	    && (reparse_ట్యాగ్ == IO_REPARSE_ట్యాగ్_SYMLINK
		|| reparse_ట్యాగ్ == IO_REPARSE_ట్యాగ్_MOUNT_బిందువు))
	res = TRUE;

    తిరిగి res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    పూర్ణాంకం
mch_is_linked(char_u *fname)
{
    ఒకవేళ (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    పూర్ణాంకం
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    పూర్ణాంకం		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_గమ్యం_utf16(fname, ఖాళీ);
    ఒకవేళ (wn == ఖాళీ)
	తిరిగి FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    ఖాళీ,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    ఖాళీ);		// handle to template file
    vim_free(wn);

    ఒకవేళ (hFile == INVALID_HANDLE_విలువ)
	తిరిగి FILEINFO_READ_FAIL;

    ఒకవేళ (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    లేకపోతే
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    తిరిగి res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    స్థిర పూర్ణాంకం
win32_getattrs(char_u *name)
{
    పూర్ణాంకం		attr;
    WCHAR	*p;

    p = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి INVALID_FILE_లక్షణాలు;

    attr = Get_File_లక్షణాలు_W(p);
    vim_free(p);

    తిరిగి attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    స్థిర పూర్ణాంకం
win32_setattrs(char_u *name, పూర్ణాంకం attrs)
{
    పూర్ణాంకం	    res;
    WCHAR   *p;

    p = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి -1;

    res = సమితి_File_లక్షణాలు_W(p, attrs);
    vim_free(p);

    తిరిగి res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    స్థిర పూర్ణాంకం
win32_సమితి_archive(char_u *name)
{
    పూర్ణాంకం attrs = win32_getattrs(name);
    ఒకవేళ (attrs == -1)
	తిరిగి -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    తిరిగి win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    పూర్ణాంకం
mch_writable(char_u *name)
{
    పూర్ణాంకం attrs = win32_getattrs(name);

    తిరిగి (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    పూర్ణాంకం
mch_can_exe(char_u *name, char_u **మార్గం, పూర్ణాంకం use_మార్గం వాడని)
{
    తిరిగి executable_exists((అక్షరం *)name, STRLEN(name), మార్గం, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    పూర్ణాంకం
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    పూర్ణాంకం		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    ఒకవేళ (STRNCMP(name, "\\\\.\\", 4) == 0)
	తిరిగి కణుపు_WRITABLE;

    wn = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (wn == ఖాళీ)
	తిరిగి కణుపు_సాధారణ;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    ఖాళీ,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    ఖాళీ);		    // handle to template file
    vim_free(wn);
    ఒకవేళ (hFile == INVALID_HANDLE_విలువ)
	తిరిగి కణుపు_సాధారణ;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    ఒకవేళ (type == FILE_TYPE_CHAR)
	తిరిగి కణుపు_WRITABLE;
    ఒకవేళ (type == FILE_TYPE_DISK)
	తిరిగి కణుపు_సాధారణ;
    తిరిగి కణుపు_OTHER;
}

#నిర్వచితమైతే HAVE_ACL
నిర్మాణం my_acl
{
    PSECURITY_DESCRIPTOR    pSecurityDescriptor;
    PSID		    pSidOwner;
    PSID		    p_Sid_సమూహం;
    PACL		    pDacl;
    PACL		    pSacl;
};
#అయితే_ముగింపు

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname)
{
#నిర్వచితం_కాకపోతే HAVE_ACL
    తిరిగి (vim_acl_T)ఖాళీ;
#లేకపోతే
    నిర్మాణం my_acl   *p = ఖాళీ;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(నిర్మాణం my_acl);
    ఒకవేళ (p != ఖాళీ)
    {
	WCHAR	*wn;

	wn = enc_గమ్యం_utf16(fname, ఖాళీ);
	ఒకవేళ (wn == ఖాళీ)
	{
	    vim_free(p);
	    తిరిగి ఖాళీ;
	}

	// Try to retrieve the entire security descriptor.
	err = GetNamedSecurityInfoW(
		wn,			// Abstract filename
		SE_FILE_OBJECT,		// File Object
		OWNER_SECURITY_INFORMATION |
		సమూహం_SECURITY_INFORMATION |
		DACL_SECURITY_INFORMATION |
		SACL_SECURITY_INFORMATION,
		&p->pSidOwner,		// Ownership information.
		&p->p_Sid_సమూహం,		// Group membership.
		&p->pDacl,		// Discretionary information.
		&p->pSacl,		// For auditing purposes.
		&p->pSecurityDescriptor);
	ఒకవేళ (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (శూన్య)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    ఖాళీ,
		    ఖాళీ,
		    &p->pDacl,
		    ఖాళీ,
		    &p->pSecurityDescriptor);
	}
	ఒకవేళ (p->pSecurityDescriptor == ఖాళీ)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = ఖాళీ;
	}
	vim_free(wn);
    }

    తిరిగి (vim_acl_T)p;
#అయితే_ముగింపు
}

#నిర్వచితమైతే HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    స్థిర BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_కొలత_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_లెక్క = 0;
    GetAclInformation(acl, &acl_info, పరిమాణం(acl_info), Acl_కొలత_Information);
    కొరకు (i = 0; i < acl_info.Ace_లెక్క; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	ఒకవేళ (ace->శీర్షిక_2.AceFlags & INHERITED_ACE)
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    శూన్య
mch_సమితి_acl(char_u *fname, vim_acl_T acl)
{
#నిర్వచితమైతే HAVE_ACL
    నిర్మాణం my_acl   *p = (నిర్మాణం my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    wn = enc_గమ్యం_utf16(fname, ఖాళీ);
    ఒకవేళ (wn == ఖాళీ)
	తిరిగి;

    // Set security flags
    ఒకవేళ (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    ఒకవేళ (p->p_Sid_సమూహం)
	sec_info |= సమూహం_SECURITY_INFORMATION;
    ఒకవేళ (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	ఒకవేళ (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    ఒకవేళ (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (శూన్య)సమితి_Named_Security_Info_W(
	    wn,			// Abstract filename
	    SE_FILE_OBJECT,	// File Object
	    sec_info,
	    p->pSidOwner,	// Ownership information.
	    p->p_Sid_సమూహం,	// Group membership.
	    p->pDacl,		// Discretionary information.
	    p->pSacl		// For auditing purposes.
	    );
    vim_free(wn);
#అయితే_ముగింపు
}

    శూన్య
mch_free_acl(vim_acl_T acl)
{
#నిర్వచితమైతే HAVE_ACL
    నిర్మాణం my_acl   *p = (నిర్మాణం my_acl *)acl;

    ఒకవేళ (p != ఖాళీ)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#అయితే_ముగింపు
}

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    స్థిర BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    ఆదానం_రికార్డు ir;
    DWORD బహిర్గతం;

    ఎంపిక (dwCtrlType)
    {
    స్థితి CTRL_C_EVENT:
	ఒకవేళ (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	తిరిగి TRUE;

    స్థితి CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = కీలకం_EVENT;
	ir.Event.కీలకం_Event_2.b_కీలకం_కిందికి = TRUE;
	ir.Event.కీలకం_Event_2.w_Repeat_లెక్క = 1;
	ir.Event.కీలకం_Event_2.w_Virtual_కీలకం_సంకేతం = VK_CANCEL;
	ir.Event.కీలకం_Event_2.w_Virtual_పరిశీలన_సంకేతం = 0;
	ir.Event.కీలకం_Event_2.dw_Control_కీలకం_State = 0;
	ir.Event.కీలకం_Event_2.uChar.UnicodeChar = 0;
	Write_Console_ఆదానం(g_h_Con_లోపల, &ir, 1, &బహిర్గతం);
	తిరిగి TRUE;

    // fatal events: shut down gracefully
    స్థితి CTRL_CLOSE_EVENT:
    స్థితి CTRL_LOGOFF_EVENT:
    స్థితి CTRL_SHUTDOWN_EVENT:
	windgoto((పూర్ణాంకం)అడ్డువరుసలు - 1, cmdline_నిలువు_off);
	g_fForceExit = TRUE;

	vim_snprintf((అక్షరం *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	తిరిగి TRUE;		// not reached

    అప్రమేయ:
	తిరిగి FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    శూన్య
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
	తిరిగి;
# endif
    Get_Console_బహుళకం(g_h_Con_లోపల, &cmodein);
    Get_Console_బహుళకం(g_h_Con_బహిర్గతం, &cmodeout);
    ఒకవేళ (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_పంక్తి_ఆదానం | ENABLE_PROCESSED_ఆదానం |
		     ENABLE_ECHO_ఆదానం);
	ఒకవేళ (g_f_Mouse_చురుకైన)
	{
	    cmodein |= ENABLE_MOUSE_ఆదానం;
	    cmodein &= ~ENABLE_శీఘ్ర_EDIT_బహుళకం;
	}
	లేకపోతే
	{
	    cmodein |= g_cmodein & ENABLE_శీఘ్ర_EDIT_బహుళకం;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# లేకపోతే
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    లేకపోతే // cooked
    {
	cmodein |= (ENABLE_పంక్తి_ఆదానం | ENABLE_PROCESSED_ఆదానం |
		    ENABLE_ECHO_ఆదానం);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    సమితి_Console_బహుళకం(g_h_Con_లోపల, cmodein | ENABLE_EXTENDED_FLAGS);
    సమితి_Console_బహుళకం(g_h_Con_బహిర్గతం, cmodeout);
    సమితి_Console_Ctrl_Handler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    ఒకవేళ (fdDump)
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
    పూర్ణాంకం
mch_get_shellsize(శూన్య)
{
    CONSOLE_SCREEN_బఫరు_INFO csbi;

# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
	తిరిగి OK;
# endif
    ఒకవేళ (!g_f_Termcap_బహుళకం && g_cbTermcap.IsValid)
    {
	/*
	 * For some reason, we are trying to get the screen dimensions
	 * even though we are not in termcap mode.  The 'Rows' and 'Columns'
	 * variables are really intended to mean the size of Vim screen
	 * while in termcap mode.
	 */
	అడ్డువరుసలు = g_cbTermcap.Info.dw_కొలత.Y;
	Columns = g_cbTermcap.Info.dw_కొలత.X;
    }
    లేకపోతే ఒకవేళ (Get_Console_Screen_బఫరు_Info(g_h_Con_బహిర్గతం, &csbi))
    {
	అడ్డువరుసలు = csbi.sr_కిటికీ.Bottom - csbi.sr_కిటికీ.శిఖరం_2 + 1;
	Columns = csbi.sr_కిటికీ.కుడి_4 - csbi.sr_కిటికీ.ఎడమ_4 + 1;
    }
    లేకపోతే
    {
	అడ్డువరుసలు = 25;
	Columns = 80;
    }
    తిరిగి OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    స్థిర శూన్య
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    ఒకవేళ (use_alternate_screen_బఫరు)
	తిరిగి;

    ఒకవేళ (!సమితి_Console_Screen_బఫరు_కొలత(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	ఒకవేళ (fdDump)
	{
	    fprintf(fdDump, "SetConsoleScreenBufferSize failed: %lx\n",
		    Get_చివరి_Error());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Resize console window size to 'srWindowRect'
 */
    స్థిర శూన్య
Resize_కిటికీ(
    HANDLE     hConsole,
    SMALL_RECT sr_కిటికీ_Rect)
{
    ఒకవేళ (!సమితి_Console_కిటికీ_Info(hConsole, TRUE, &sr_కిటికీ_Rect))
    {
# ifdef MCH_WRITE_DUMP
	ఒకవేళ (fdDump)
	{
	    fprintf(fdDump, "SetConsoleWindowInfo failed: %lx\n",
		    Get_చివరి_Error());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Set a console window to `xSize' * `ySize'
 */
    స్థిర శూన్య
Resize_Con_Buf_And_కిటికీ(
    HANDLE  hConsole,
    పూర్ణాంకం	    x_కొలత,
    పూర్ణాంకం	    y_కొలత_2)
{
    CONSOLE_SCREEN_బఫరు_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    sr_కిటికీ_Rect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    స్థిర పూర్ణాంకం	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    ఒకవేళ (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", x_కొలత, y_కొలత_2);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = Get_అతిపెద్ద_Console_కిటికీ_కొలత(hConsole);

    // define the new console window size and scroll position
    sr_కిటికీ_Rect.ఎడమ_4 = sr_కిటికీ_Rect.శిఖరం_2 = (SHORT) 0;
    sr_కిటికీ_Rect.కుడి_4 =  (SHORT) (min(x_కొలత, coordScreen.X) - 1);
    sr_కిటికీ_Rect.Bottom = (SHORT) (min(y_కొలత_2, coordScreen.Y) - 1);

    ఒకవేళ (Get_Console_Screen_బఫరు_Info(g_h_Con_బహిర్గతం, &csbi))
    {
	పూర్ణాంకం sx, sy;

	sx = csbi.sr_కిటికీ.కుడి_4 - csbi.sr_కిటికీ.ఎడమ_4 + 1;
	sy = csbi.sr_కిటికీ.Bottom - csbi.sr_కిటికీ.శిఖరం_2 + 1;
	ఒకవేళ (sy < y_కొలత_2 || sx < x_కొలత)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    ఒకవేళ (sy < y_కొలత_2)
		coordScreen.Y = y_కొలత_2;
	    లేకపోతే
		coordScreen.Y = sy;
	    ఒకవేళ (sx < x_కొలత)
		coordScreen.X = x_కొలత;
	    లేకపోతే
		coordScreen.X = sx;
	    సమితి_Console_Screen_బఫరు_కొలత(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = x_కొలత;
    coordScreen.Y = y_కొలత_2;

    // In the new console call API, only the first time in reverse order
    ఒకవేళ (!vtp_working || resized)
    {
	Resize_కిటికీ(hConsole, sr_కిటికీ_Rect);
	ResizeConBuf(hConsole, coordScreen);
    }
    లేకపోతే
    {
	// Workaround for a Windows 10 bug
	cursor.X = sr_కిటికీ_Rect.ఎడమ_4;
	cursor.Y = sr_కిటికీ_Rect.శిఖరం_2;
	సమితి_Console_Cursor_Position(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	Resize_కిటికీ(hConsole, sr_కిటికీ_Rect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    శూన్య
mch_సమితి_shellsize(శూన్య)
{
    COORD coordScreen;

# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
	తిరిగి;
# endif
    // Don't change window size while still starting up
    ఒకవేళ (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	తిరిగి;
    }

    ఒకవేళ (పదం_console)
    {
	coordScreen = Get_అతిపెద్ద_Console_కిటికీ_కొలత(g_h_Con_బహిర్గతం);

	// Clamp Rows and Columns to reasonable values
	ఒకవేళ (అడ్డువరుసలు > coordScreen.Y)
	    అడ్డువరుసలు = coordScreen.Y;
	ఒకవేళ (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	Resize_Con_Buf_And_కిటికీ(g_h_Con_బహిర్గతం, Columns, అడ్డువరుసలు);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    శూన్య
mch_new_shellsize(శూన్య)
{
# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
	తిరిగి;
# endif
    సమితి_scroll_region(0, 0, Columns - 1, అడ్డువరుసలు - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    శూన్య
mch_సమితి_winsize_now(శూన్య)
{
    ఒకవేళ (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_సమితి_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# ఒకవేళ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    శూన్య
mch_calc_cell_కొలత(నిర్మాణం cellsize *cs_బహిర్గతం)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_బహిర్గతం->cs_xpixel = -1;
    cs_బహిర్గతం->cs_ypixel = -1;

    ఒకవేళ (g_h_Con_బహిర్గతం == INVALID_HANDLE_విలువ)
	తిరిగి;

    cfi.cb_కొలత = పరిమాణం(cfi);
    ఒకవేళ (GetCurrentConsoleFontEx(g_h_Con_బహిర్గతం, FALSE, &cfi)
	    && cfi.dw_Font_కొలత.X > 0 && cfi.dw_Font_కొలత.Y > 0)
    {
	cs_బహిర్గతం->cs_xpixel = cfi.dw_Font_కొలత.X;
	cs_బహిర్గతం->cs_ypixel = cfi.dw_Font_కొలత.Y;
	తిరిగి;
    }

    Get_Console_బహుళకం(g_h_Con_లోపల, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_పంక్తి_ఆదానం | ENABLE_ECHO_ఆదానం
	    | ENABLE_PROCESSED_ఆదానం);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_ఆదానం;

    ఒకవేళ (సమితి_Console_బహుళకం(g_h_Con_లోపల, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	సమితి_Console_బహుళకం(g_h_Con_లోపల, cmodeold);
    }
}
# endif

#అయితే_ముగింపు

    స్థిర BOOL
vim_create_process(
    అక్షరం		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    అక్షరం		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = ఖాళీ;

    wcmd = enc_గమ్యం_utf16((char_u *)cmd, ఖాళీ);
    ఒకవేళ (wcmd == ఖాళీ)
	తిరిగి FALSE;
    ఒకవేళ (cwd != ఖాళీ)
    {
	wcwd = enc_గమ్యం_utf16((char_u *)cwd, ఖాళీ);
	ఒకవేళ (wcwd == ఖాళీ)
	    వెళ్ళు theend;
    }

    ret = CreateProcessW(
	    ఖాళీ,		// Executable name
	    wcmd,		// Command to execute
	    ఖాళీ,		// Process security attributes
	    ఖాళీ,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    తిరిగి ret;
}


    స్థిర HINSTANCE
vim_shell_execute(
    అక్షరం *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_గమ్యం_utf16((char_u *)cmd, ఖాళీ);
    ఒకవేళ (wcmd == ఖాళీ)
	తిరిగి (HINSTANCE) 0;

    ret = ShellExecuteW(ఖాళీ, ఖాళీ, wcmd, ఖాళీ, ఖాళీ, n_show_cmd);
    vim_free(wcmd);
    తిరిగి ret;
}


#ఒకవేళ defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    స్థిర పూర్ణాంకం
mch_system_classic(అక్షరం *cmd, పూర్ణాంకం options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = పరిమాణం(si);
    si.lpReserved = ఖాళీ;
    si.lpDesktop = ఖాళీ;
    si.lpTitle = ఖాళీ;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    ఒకవేళ (options & SHELL_DOOUT)
	si.w_Show_కిటికీ = SW_SHOWMINNOACTIVE;
    లేకపోతే
	si.w_Show_కిటికీ = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = ఖాళీ;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_బహుళకం |	CREATE_NEW_CONSOLE,
	    &si, &pi, ఖాళీ, ఖాళీ);

    // Wait for the command to terminate before continuing
    {
	పూర్ణాంకం	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	కొరకు (;;)
	{
	    సందేశం_3	సందేశం;

	    ఒకవేళ (Peek_సందేశం_W(&సందేశం, (HWND)ఖాళీ, 0, 0, PM_REMOVE))
	    {
		Translate_సందేశం(&సందేశం);
		Dispatch_సందేశం_W(&సందేశం);
		delay = 1;
		కొనసాగు;
	    }
	    ఒకవేళ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		ఆపు;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    ఒకవేళ (delay < 50)
		delay += 10;
	}

	// Get the command exit code
	Get_Exit_సంకేతం_Process(pi.hProcess, &ret);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // Try to get input focus back.  Doesn't always work though.
    Post_సందేశం(hwnd, WM_SETFOCUS, 0, 0);
    // To increase the chances that WM_SETFOCUS will work, run the message loop
    // here.  In addition, it prevents problems caused by delayed WM_SETFOCUS
    // processing.
    gui_mch_update();

    తిరిగి ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    స్థిర గుర్తులేని పూర్ణాంకం __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_h_బిడ్డ_Std_లోపల_Wr = param;
    linenr_T	    lnum = curbuf->b_op_ప్రారంభం.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    పూర్ణాంకం		    written = 0;

    కొరకు (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	ఒకవేళ (l == 0)
	    len = 0;
	లేకపోతే ఒకవేళ (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_h_బిడ్డ_Std_లోపల_Wr, "", 1, &len, ఖాళీ);
	}
	లేకపోతే
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_h_బిడ్డ_Std_లోపల_Wr, (అక్షరం *)lp + written,
		      s == ఖాళీ ? l : (DWORD)(s - (lp + written)),
		      &len, ఖాళీ);
	}
	ఒకవేళ (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    ఒకవేళ (lnum != curbuf->b_op_చివర.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_పంక్తి_లెక్క
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_h_బిడ్డ_Std_లోపల_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, ఖాళీ);
	    }

	    ++lnum;
	    ఒకవేళ (lnum > curbuf->b_op_చివర.lnum)
		ఆపు;

	    lp = ml_get(lnum);
	    written = 0;
	}
	లేకపోతే ఒకవేళ (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_h_బిడ్డ_Std_లోపల_Wr);
    తిరిగి 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    స్థిర శూన్య
dump_pipe(పూర్ణాంకం	    options,
	  HANDLE    g_h_బిడ్డ_Std_బహిర్గతం_Rd,
	  garray_T  *ga,
	  char_u    బఫరు[],
	  DWORD	    *బఫరు_off)
{
    DWORD	అందుబాటు_బైట్లు = 0;
    DWORD	i;
    పూర్ణాంకం		ret;
    DWORD	len;
    DWORD	గమ్యం_Read;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_h_బిడ్డ_Std_బహిర్గతం_Rd, // pipe to query
			ఖాళీ,		    // optional buffer
			0,		    // buffer size
			ఖాళీ,		    // number of read bytes
			&అందుబాటు_బైట్లు,    // available bytes total
			ఖాళీ);		    // byteLeft

    // We got real data in the pipe, read it
    అయితే (ret != 0 && అందుబాటు_బైట్లు > 0)
    {
	గమ్యం_Read = (DWORD)(BUFLEN - *బఫరు_off);
	గమ్యం_Read = అందుబాటు_బైట్లు < గమ్యం_Read ? అందుబాటు_బైట్లు : గమ్యం_Read;
	ReadFile(g_h_బిడ్డ_Std_బహిర్గతం_Rd, బఫరు + *బఫరు_off, గమ్యం_Read , &len, ఖాళీ);

	// If we haven't read anything, there is a problem
	ఒకవేళ (len == 0)
	    ఆపు;

	అందుబాటు_బైట్లు -= len;

	ఒకవేళ (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    కొరకు (i = 0; i < len; ++i)
	    {
		ఒకవేళ (బఫరు[i] == NL)
		    append_ga_పంక్తి(ga);
		లేకపోతే ఒకవేళ (బఫరు[i] == NUL)
		    ga_append(ga, NL);
		లేకపోతే
		    ga_append(ga, బఫరు[i]);
	    }
	}
	లేకపోతే ఒకవేళ (has_mbyte)
	{
	    పూర్ణాంకం		l;
	    పూర్ణాంకం		c;
	    char_u	*p;

	    len += *బఫరు_off;
	    బఫరు[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    కొరకు (p = బఫరు; p < బఫరు + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		ఒకవేళ (l == 0)
		    l = 1;  // NUL byte?
		లేకపోతే ఒకవేళ (MB_BYTE2LEN(*p) != l)
		    ఆపు;
	    }
	    ఒకవేళ (p == బఫరు)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		ఒకవేళ (len >= 12)
		    ++p;
		లేకపోతే
		{
		    *బఫరు_off = len;
		    తిరిగి;
		}
	    }
	    c = *p;
	    *p = NUL;
	    సందేశం_puts((అక్షరం *)బఫరు);
	    ఒకవేళ (p < బఫరు + len)
	    {
		*p = c;
		*బఫరు_off = (DWORD)((బఫరు + len) - p);
		mch_memmove(బఫరు, p, *బఫరు_off);
		తిరిగి;
	    }
	    *బఫరు_off = 0;
	}
	లేకపోతే
	{
	    బఫరు[len] = NUL;
	    సందేశం_puts((అక్షరం *)బఫరు);
	}

	windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);
	cursor_on();
	బహిర్గతం_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    స్థిర పూర్ణాంకం
mch_system_piped(అక్షరం *cmd, పూర్ణాంకం options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_h_బిడ్డ_Std_లోపల_Rd = ఖాళీ;
    HANDLE g_h_బిడ్డ_Std_లోపల_Wr = ఖాళీ;
    HANDLE g_h_బిడ్డ_Std_బహిర్గతం_Rd = ఖాళీ;
    HANDLE g_h_బిడ్డ_Std_బహిర్గతం_Wr = ఖాళీ;

    char_u	బఫరు[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    పూర్ణాంకం		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    పూర్ణాంకం		noread_cnt = 0;
    garray_T	ga;
    పూర్ణాంకం		delay = 1;
    DWORD	బఫరు_off = 0;	// valid bytes in buffer[]
    అక్షరం	*p = ఖాళీ;

    SECURITY_లక్షణాలు saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.n_నిడివి = పరిమాణం(SECURITY_లక్షణాలు);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ఖాళీ;

    ఒకవేళ ( ! CreatePipe(&g_h_బిడ్డ_Std_బహిర్గతం_Rd, &g_h_బిడ్డ_Std_బహిర్గతం_Wr, &saAttr, 0)
	// Ensure the read handle to the pipe for STDOUT is not inherited.
       || ! సమితి_Handle_Information(g_h_బిడ్డ_Std_బహిర్గతం_Rd, HANDLE_FLAG_INHERIT, 0)
	// Create a pipe for the child process's STDIN.
       || ! CreatePipe(&g_h_బిడ్డ_Std_లోపల_Rd, &g_h_బిడ్డ_Std_లోపల_Wr, &saAttr, 0)
	// Ensure the write handle to the pipe for STDIN is not inherited.
       || ! సమితి_Handle_Information(g_h_బిడ్డ_Std_లోపల_Wr, HANDLE_FLAG_INHERIT, 0) )
    {
	CloseHandle(g_h_బిడ్డ_Std_లోపల_Rd);
	CloseHandle(g_h_బిడ్డ_Std_లోపల_Wr);
	CloseHandle(g_h_బిడ్డ_Std_బహిర్గతం_Rd);
	CloseHandle(g_h_బిడ్డ_Std_బహిర్గతం_Wr);
	సందేశం_puts(_("\nCannot create pipes\n"));
    }

    si.cb = పరిమాణం(si);
    si.lpReserved = ఖాళీ;
    si.lpDesktop = ఖాళీ;
    si.lpTitle = ఖాళీ;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_h_బిడ్డ_Std_బహిర్గతం_Wr;
    si.hStdOutput = g_h_బిడ్డ_Std_బహిర్గతం_Wr;
    si.h_Std_ఆదానం = g_h_బిడ్డ_Std_లోపల_Rd;
    si.w_Show_కిటికీ = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = ఖాళీ;

    ఒకవేళ (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    ఒకవేళ (cmd != ఖాళీ)
    {
	p = (అక్షరం *)vim_strsave((char_u *)cmd);
	ఒకవేళ (p != ఖాళీ)
	    unescape_shellxquote((char_u *)p, p_sxe);
	లేకపోతే
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_బహుళకం,
						&si, &pi, ఖాళీ, ఖాళీ);

    ఒకవేళ (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_h_బిడ్డ_Std_లోపల_Rd);
    CloseHandle(g_h_బిడ్డ_Std_బహిర్గతం_Wr);

    ఒకవేళ (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(ఖాళీ,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_h_బిడ్డ_Std_లోపల_Wr,  // parameter
			0,		 // creation flag, start immediately
			ఖాళీ);		    // we don't care about thread id
	CloseHandle(thread);
	g_h_బిడ్డ_Std_లోపల_Wr = ఖాళీ;
    }

    // Keep updating the window while waiting for the shell to finish.
    కొరకు (;;)
    {
	సందేశం_3	సందేశం;

	ఒకవేళ (Peek_సందేశం_W(&సందేశం, (HWND)ఖాళీ, 0, 0, PM_REMOVE))
	{
	    Translate_సందేశం(&సందేశం);
	    Dispatch_సందేశం_W(&సందేశం);
	}

	// write pipe information in the window
	ఒకవేళ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.లోపల_use
# endif
	    )
	{
	    len = 0;
	    ఒకవేళ (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.లోపల_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		ఒకవేళ (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		ఒకవేళ (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    ఒకవేళ (len == 1 && cmd != ఖాళీ)
		    {
			ఒకవేళ (ta_buf[ta_len] == Ctrl_C)
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
		    ఒకవేళ ((len == 1 || len == 4 ) && cmd != ఖాళీ)
		    {
			ఒకవేళ (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_h_బిడ్డ_Std_లోపల_Wr);
			    g_h_బిడ్డ_Std_లోపల_Wr = ఖాళీ;
			    len = 0;
			}
		    }

		    len = పదం_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    కొరకు (i = ta_len; i < ta_len + len; ++i)
		    {
			ఒకవేళ (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    సందేశం_putchar(ta_buf[i]);
			లేకపోతే ఒకవేళ (has_mbyte)
			{
			    పూర్ణాంకం l = (*mb_ptr2len)(ta_buf + i);

			    సందేశం_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			లేకపోతే
			    సందేశం_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);
		    బహిర్గతం_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    ఒకవేళ (options & SHELL_WRITE)
			ta_len = 0;
		    లేకపోతే ఒకవేళ (g_h_బిడ్డ_Std_లోపల_Wr != ఖాళీ)
		    {
			WriteFile(g_h_బిడ్డ_Std_లోపల_Wr, (అక్షరం*)ta_buf,
				    1, &len, ఖాళీ);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			ఒకవేళ (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	ఒకవేళ (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	ఒకవేళ (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_h_బిడ్డ_Std_బహిర్గతం_Rd, &ga, బఫరు, &బఫరు_off);
	    ఆపు;
	}

	++noread_cnt;
	dump_pipe(options, g_h_బిడ్డ_Std_బహిర్గతం_Rd, &ga, బఫరు, &బఫరు_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	ఒకవేళ (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_h_బిడ్డ_Std_బహిర్గతం_Rd);
    ఒకవేళ (g_h_బిడ్డ_Std_లోపల_Wr != ఖాళీ)
	CloseHandle(g_h_బిడ్డ_Std_లోపల_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    Get_Exit_సంకేతం_Process(pi.hProcess, &ret);

    ఒకవేళ (options & SHELL_READ)
    {
	ఒకవేళ (ga.ga_len > 0)
	{
	    append_ga_పంక్తి(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	లేకపోతే
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    తిరిగి ret;
}

    స్థిర పూర్ణాంకం
mch_system_g(అక్షరం *cmd, పూర్ణాంకం options)
{
    // if we can pipe and the shelltemp option is off
    ఒకవేళ (!p_stmp)
	తిరిగి mch_system_piped(cmd, options);
    లేకపోతే
	తిరిగి mch_system_classic(cmd, options);
}
#అయితే_ముగింపు

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    స్థిర పూర్ణాంకం
mch_system_c(అక్షరం *cmd, పూర్ణాంకం options వాడని)
{
    పూర్ణాంకం		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    ఒకవేళ (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	ఒకవేళ (buf == ఖాళీ)
	    తిరిగి -1;
	vim_snprintf((అక్షరం *)buf, len, "(%s)", cmd);
	wcmd = enc_గమ్యం_utf16(buf, ఖాళీ);
	విడుదల(buf);
    }
    లేకపోతే
	wcmd = enc_గమ్యం_utf16((char_u *)cmd, ఖాళీ);

    ఒకవేళ (wcmd == ఖాళీ)
	తిరిగి -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    తిరిగి ret;
}

#అయితే_ముగింపు

    స్థిర పూర్ణాంకం
mch_system(అక్షరం *cmd, పూర్ణాంకం options)
{
#నిర్వచితమైతే VIMDLL
    ఒకవేళ (gui.లోపల_use || gui.starting)
	తిరిగి mch_system_g(cmd, options);
    లేకపోతే
	తిరిగి mch_system_c(cmd, options);
#లేదావేళ defined(FEAT_GUI_MSWIN)
    తిరిగి mch_system_g(cmd, options);
#లేకపోతే
    తిరిగి mch_system_c(cmd, options);
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    స్థిర పూర్ణాంకం
mch_call_shell_terminal(
    char_u	*cmd,
    పూర్ణాంకం		options వాడని)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = ఖాళీ;
    typval_T	argvar[2];
    long_u	cmdlen;
    పూర్ణాంకం		retval = -1;
    buf_T	*buf;
    పని_T	*పని;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    ఒకవేళ (cmd == ఖాళీ)
	cmdlen = STRLEN(p_sh) + 1;
    లేకపోతే
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    ఒకవేళ (newcmd == ఖాళీ)
	తిరిగి 255;
    ఒకవేళ (cmd == ఖాళీ)
    {
	STRCPY(newcmd, p_sh);
	ch_log(ఖాళీ, "starting terminal to run a shell");
    }
    లేకపోతే
    {
	vim_snprintf((అక్షరం *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(ఖాళీ, "starting terminal for system command '%s'", cmd);
    }

    init_పని_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = పదం_ప్రారంభం(argvar, ఖాళీ, &opt, పదం_ప్రారంభం_SYSTEM);
    ఒకవేళ (buf == ఖాళీ)
    {
	vim_free(newcmd);
	తిరిగి 255;
    }

    పని = పదం_getjob(buf->b_పదం);
    ++పని->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    ఒకవేళ (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	అయితే (పదం_use_loop())
	{
	    ఒకవేళ (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_చురుకైన)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		ఒకవేళ (terminal_loop(TRUE) == OK)
		    సాధారణ_cmd(&oa, TRUE);
	    }
	    లేకపోతే
		సాధారణ_cmd(&oa, TRUE);
	}
	retval = పని->jv_exitval;
	ch_log(ఖాళీ, "system command finished");

	పని_unref(పని);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_బఫరు(DOBUF_WIPE, DOBUF_మొదటి, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    తిరిగి retval;
}
#అయితే_ముగింపు
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    స్థిర శూన్య
restore_env_var(char_u *name, char_u *old_విలువ, పూర్ణాంకం must_free)
{
    ఒకవేళ (old_విలువ != ఖాళీ)
    {
	vim_setenv(name, old_విలువ);
	ఒకవేళ (must_free)
	    vim_free(old_విలువ);
	తిరిగి;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    పూర్ణాంకం
mch_call_shell(
    char_u  *cmd,
    పూర్ణాంకం	    options)	// SHELL_*, see vim.h
{
    పూర్ణాంకం		x = 0;
    పూర్ణాంకం		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    పూర్ణాంకం		must_free;
    char_u	*oldval;

#నిర్వచితమైతే FEAT_EVAL
    ch_log(ఖాళీ, "executing shell command: %s", cmd);
#అయితే_ముగింపు
    // Change the title to reflect that we are in a subshell.
    ఒకవేళ (GetConsoleTitleW(szShellTitle, ARRAY_నిడివి(szShellTitle) - 4) > 0)
    {
	ఒకవేళ (cmd == ఖాళీ)
	    wcscat(szShellTitle, L" :sh");
	లేకపోతే
	{
	    WCHAR *wn = enc_గమ్యం_utf16((char_u *)cmd, ఖాళీ);

	    ఒకవేళ (wn != ఖాళీ)
	    {
		wcscat(szShellTitle, L" - !");
		ఒకవేళ ((wcslen(szShellTitle) + wcslen(wn) <
			    ARRAY_నిడివి(szShellTitle)))
		    wcscat(szShellTitle, wn);
		సమితి_Console_Title_W(szShellTitle);
		vim_free(wn);
	    }
	}
    }
    // do not execute anything from the current directory by setting the
    // environment variable $NoDefaultCurrentDirectoryInExePath
    oldval = vim_getenv((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    &must_free);
    vim_setenv((char_u *)"NoDefaultCurrentDirectoryInExePath", (char_u *)"1");

    బహిర్గతం_flush();

#నిర్వచితమైతే MCH_WRITE_DUMP
    ఒకవేళ (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    ఒకవేళ (
# ifdef VIMDLL
	    gui.లోపల_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != ఖాళీ
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	ఒకవేళ (cmdbase != ఖాళీ)
	    // Skip a leading quote and (.
	    అయితే (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	ఒకవేళ (cmdbase == ఖాళీ || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    తిరిగి x;
	}
    }
#అయితే_ముగింపు

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

    ఒకవేళ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    ఒకవేళ (cmd == ఖాళీ)
    {
	x = mch_system((అక్షరం *)p_sh, options);
    }
    లేకపోతే
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = ఖాళీ;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	ఒకవేళ (*cmdbase == '"' )
	    ++cmdbase;
	ఒకవేళ (*cmdbase == '(')
	    ++cmdbase;

	ఒకవేళ ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    Zero_జ్ఞాపకం(&si, పరిమాణం(si));
	    si.cb = పరిమాణం(si);
	    si.lpReserved = ఖాళీ;
	    si.lpDesktop = ఖాళీ;
	    si.lpTitle = ఖాళీ;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = ఖాళీ;

	    cmdbase = skipwhite(cmdbase + 5);
	    ఒకవేళ ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.w_Show_కిటికీ = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    లేకపోతే ఒకవేళ ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_కిటికీ;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_ఆదానం = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    ఖాళీ,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_సాధారణ,		// File att.
		    ఖాళీ);				// Temp file
		si.hStdOutput = si.h_Std_ఆదానం;
		si.hStdError = si.h_Std_ఆదానం;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    ఒకవేళ (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		ఒకవేళ (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		ఒకవేళ (p > cmdbase && p[-1] == ')'
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
	    ఒకవేళ (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		ఒకవేళ (cmd_shell == ఖాళీ || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		ఒకవేళ (subcmd != ఖాళీ)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    ఒకవేళ (newcmd != ఖాళీ)
			vim_snprintf((అక్షరం *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    లేకపోతే
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    ఒకవేళ (vim_create_process((అక్షరం *)newcmd, FALSE, flags,
			&si, &pi, ఖాళీ, ఖాళీ))
		x = 0;
	    లేకపోతే ఒకవేళ (vim_shell_execute((అక్షరం *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    లేకపోతే
	    {
		x = -1;
#నిర్వచితమైతే FEAT_GUI_MSWIN
# ifdef VIMDLL
		ఒకవేళ (gui.లోపల_use)
# endif
		    emsg(_(e_command_not_దొరికింది));
#అయితే_ముగింపు
	    }

	    ఒకవేళ (newcmd != cmdbase)
		vim_free(newcmd);

	    ఒకవేళ (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_ఆదానం != ఖాళీ)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_ఆదానం);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	లేకపోతే
	{
	    cmdlen =
#నిర్వచితమైతే FEAT_GUI_MSWIN
		((gui.లోపల_use || gui.starting) ?
		    (!s_dont_use_vimrun && p_stmp ?
			vimrun_మార్గం.నిడివి : STRLEN(p_sh) + STRLEN(p_shcf))
		    : 0) +
#అయితే_ముగింపు
		STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;

	    newcmd = alloc(cmdlen);
	    ఒకవేళ (newcmd != ఖాళీ)
	    {
#ఒకవేళ defined(FEAT_GUI_MSWIN)
		ఒకవేళ (
# ifdef VIMDLL
		    (gui.లోపల_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    అక్షరం *సందేశం = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    అక్షరం *title = _("Vim Warning");
		    WCHAR *wmsg = enc_గమ్యం_utf16((char_u *)సందేశం, ఖాళీ);
		    WCHAR *wtitle = enc_గమ్యం_utf16((char_u *)title, ఖాళీ);

		    ఒకవేళ (wmsg != ఖాళీ && wtitle != ఖాళీ)
			సందేశం_Box_W(ఖాళీ, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		ఒకవేళ (
# ifdef VIMDLL
		    (gui.లోపల_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((అక్షరం *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_మార్గం.string,
			    (సందేశం_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		లేకపోతే ఒకవేళ (
# ifdef VIMDLL
			(gui.లోపల_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((అక్షరం *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		లేకపోతే
#అయితే_ముగింపు
		    vim_snprintf((అక్షరం *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((అక్షరం *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    ఒకవేళ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    ఒకవేళ (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#ఒకవేళ defined(FEAT_GUI_MSWIN)
	    && ((gui.లోపల_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#అయితే_ముగింపు
	    )
    {
	smsg(_("shell returned %d"), x);
	సందేశం_putchar('\n');
    }
    resettitle();

    mch_signal(SIGINT, SIG_DFL);
    mch_signal(SIGBREAK, SIG_DFL);
    mch_signal(SIGILL, SIG_DFL);
    mch_signal(SIGFPE, SIG_DFL);
    mch_signal(SIGSEGV, SIG_DFL);
    mch_signal(SIGTERM, SIG_DFL);
    mch_signal(SIGABRT, SIG_DFL);

    తిరిగి x;
}

#ఒకవేళ defined(FEAT_పని_CHANNEL)
    స్థిర HANDLE
పని_io_file_open(
	char_u *fname,
	DWORD dwDesiredAccess,
	DWORD dw_Share_బహుళకం,
	LPSECURITY_లక్షణాలు lp_Security_లక్షణాలు,
	DWORD dwCreationDisposition,
	DWORD dw_Flags_And_లక్షణాలు)
{
    HANDLE h;
    WCHAR *wn;

    wn = enc_గమ్యం_utf16(fname, ఖాళీ);
    ఒకవేళ (wn == ఖాళీ)
	తిరిగి INVALID_HANDLE_విలువ;

    h = CreateFileW(wn, dwDesiredAccess, dw_Share_బహుళకం,
	    lp_Security_లక్షణాలు, dwCreationDisposition,
	    dw_Flags_And_లక్షణాలు, ఖాళీ);
    vim_free(wn);
    తిరిగి h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    శూన్య
win32_build_env(dict_T *env, garray_T *gap, పూర్ణాంకం is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != ఖాళీ ? env->dv_hashtab.ht_వాడిన : 0;
    LPVOID	పునాది = Get_Environment_అక్షరశ్రేణులు_W();

    // for last \0
    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి;

    ఒకవేళ (env != ఖాళీ)
    {
	FOR_ALL_HASHTAB_వస్తువులు(&env->dv_hashtab, hi, todo)
	{
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		typval_T *అంశం = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_గమ్యం_utf16((char_u *)hi->hi_కీలకం, ఖాళీ);
		WCHAR   *wval = enc_గమ్యం_utf16(tv_get_string(అంశం), ఖాళీ);
		--todo;
		ఒకవేళ (wkey != ఖాళీ && wval != ఖాళీ)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    ఒకవేళ (ga_grow(gap, (పూర్ణాంకం)(lkey + lval + 2)) == FAIL)
			కొనసాగు;
		    కొరకు (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) = L'=';
		    కొరకు (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    ఒకవేళ (పునాది)
    {
	WCHAR	*p = (WCHAR*) పునాది;

	// for last \0
	ఒకవేళ (ga_grow(gap, 1) == FAIL)
	    తిరిగి;

	అయితే (*p != 0 || *(p + 1) != 0)
	{
	    ఒకవేళ (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) = *p;
	    p++;
	}
	Free_Environment_అక్షరశ్రేణులు_W(పునాది);
	*((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) = L'\0';
    }

# ఒకవేళ defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
    {
#  ifdef FEAT_CLIENTSERVER
	char_u	*servername = get_vim_var_str(VV_SEND_SERVER);
	size_t	servername_len = STRLEN(servername);
#  endif
#  ifdef FEAT_TERMINAL
	char_u	*సంస్కరణ_2 = get_vim_var_str(VV_సంస్కరణ);
	size_t	సంస్కరణ_len = STRLEN(సంస్కరణ_2);
#  endif
	// size of "VIM_SERVERNAME=" and value,
	// plus "VIM_TERMINAL=" and value,
	// plus two terminating NULs
	size_t	n = 0
#  ifdef FEAT_CLIENTSERVER
		    + 15 + servername_len
#  endif
#  ifdef FEAT_TERMINAL
		    + 13 + సంస్కరణ_len + 2
#  endif
		    ;

	ఒకవేళ (ga_grow(gap, (పూర్ణాంకం)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    కొరకు (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    కొరకు (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    ఒకవేళ (is_terminal)
	    {
		కొరకు (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		కొరకు (n = 0; n < సంస్కరణ_len; n++)
		    *((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) =
			(WCHAR)సంస్కరణ_2[n];
		*((WCHAR*)gap->ga_దత్తాంశం + gap->ga_len++) = L'\0';
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
    స్థిర BOOL
create_pipe_pair(HANDLE handles[2])
{
    స్థిర LONG		s;
    అక్షరం		name[64];
    SECURITY_లక్షణాలు sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_కొలత, 0, 0, ఖాళీ);

    ఒకవేళ (handles[1] == INVALID_HANDLE_విలువ)
	తిరిగి FALSE;

    sa.n_నిడివి = పరిమాణం(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = ఖాళీ;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_సాధారణ, 0);

    ఒకవేళ (handles[0] == INVALID_HANDLE_విలువ)
    {
	CloseHandle(handles[1]);
	తిరిగి FALSE;
    }

    తిరిగి TRUE;
}

#అయితే_ముగింపు // FEAT_JOB_CHANNEL

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Execute "argv" directly without the shell and return the output.
 * Used by system() and systemlist() when the command is a List.
 * "infile" is an optional temp file for stdin input.
 * When "ret_len" is not NULL, set it to the length of the output.
 * Returns the output in allocated memory (or NULL on error).
 * Sets v:shell_error to the exit status.
 */
    char_u *
mch_get_cmd_output_ప్రత్యక్ష(
    అక్షరం	**argv,
    char_u	*infile,
    పూర్ణాంకం		flags వాడని,
    పూర్ణాంకం		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_లక్షణాలు saAttr;
    HANDLE		h_బిడ్డ_Stdout_Rd = INVALID_HANDLE_విలువ;
    HANDLE		h_బిడ్డ_Stdout_Wr = INVALID_HANDLE_విలువ;
    HANDLE		h_బిడ్డ_Stdin_Rd = INVALID_HANDLE_విలువ;
    garray_T		cmd_ga;
    garray_T		బహిర్గతం_ga;
    char_u		*బఫరు = ఖాళీ;
    DWORD		exit_సంకేతం = (DWORD)-1;
    పూర్ణాంకం			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    కొరకు (i = 0; argv[i] != ఖాళీ; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	పూర్ణాంకం	has_spaces = FALSE;
	పూర్ణాంకం	j;

	కొరకు (j = 0; s[j] != NUL; j++)
	    ఒకవేళ (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		ఆపు;
	    }

	ఒకవేళ (i > 0)
	    ga_append(&cmd_ga, ' ');

	ఒకవేళ (has_spaces)
	{
	    పూర్ణాంకం	అంకె_bs;

	    ga_append(&cmd_ga, '"');
	    కొరకు (j = 0; arg[j] != NUL; j++)
	    {
		అంకె_bs = 0;
		అయితే (arg[j] == '\\')
		{
		    అంకె_bs++;
		    j++;
		}

		ఒకవేళ (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    అయితే (అంకె_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ఆపు;
		}
		లేకపోతే ఒకవేళ (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    అయితే (అంకె_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		లేకపోతే
		{
		    అయితే (అంకె_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	లేకపోతే
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&బహిర్గతం_ga, 1, 4096);

    saAttr.n_నిడివి = పరిమాణం(SECURITY_లక్షణాలు);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ఖాళీ;

    // Create a pipe for the child's stdout.
    ఒకవేళ (!CreatePipe(&h_బిడ్డ_Stdout_Rd, &h_బిడ్డ_Stdout_Wr, &saAttr, 0)
	    || !సమితి_Handle_Information(h_బిడ్డ_Stdout_Rd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	వెళ్ళు పూర్తి;
    }

    // Set up stdin from infile if provided.
    ఒకవేళ (infile != ఖాళీ)
    {
	WCHAR *winfile = enc_గమ్యం_utf16(infile, ఖాళీ);

	ఒకవేళ (winfile != ఖాళీ)
	{
	    h_బిడ్డ_Stdin_Rd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_సాధారణ, ఖాళీ);
	    vim_free(winfile);
	}
    }

    Zero_జ్ఞాపకం(&pi, పరిమాణం(pi));
    Zero_జ్ఞాపకం(&si, పరిమాణం(si));
    si.cb = పరిమాణం(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.w_Show_కిటికీ = SW_HIDE;
    si.hStdOutput = h_బిడ్డ_Stdout_Wr;
    si.hStdError = h_బిడ్డ_Stdout_Wr;
    si.h_Std_ఆదానం = (h_బిడ్డ_Stdin_Rd != INVALID_HANDLE_విలువ)
		    ? h_బిడ్డ_Stdin_Rd : INVALID_HANDLE_విలువ;

    ch_log(ఖాళీ, "directly executing: %s", (అక్షరం *)cmd_ga.ga_దత్తాంశం);

    // Create the child process directly, without going through the shell.
    ఒకవేళ (!vim_create_process((అక్షరం *)cmd_ga.ga_దత్తాంశం, TRUE,
		CREATE_DEFAULT_ERROR_బహుళకం | CREATE_NEW_PROCESS_సమూహం,
		&si, &pi, ఖాళీ, ఖాళీ))
    {
	semsg(_(e_invalid_వాదం_str), cmd_ga.ga_దత్తాంశం);
	వెళ్ళు పూర్తి;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(h_బిడ్డ_Stdout_Wr);
    h_బిడ్డ_Stdout_Wr = INVALID_HANDLE_విలువ;
    ఒకవేళ (h_బిడ్డ_Stdin_Rd != INVALID_HANDLE_విలువ)
    {
	CloseHandle(h_బిడ్డ_Stdin_Rd);
	h_బిడ్డ_Stdin_Rd = INVALID_HANDLE_విలువ;
    }

    // Read output from child process.
    కొరకు (;;)
    {
	అక్షరం	buf[4096];
	DWORD	n;

	ఒకవేళ (!ReadFile(h_బిడ్డ_Stdout_Rd, buf, పరిమాణం(buf), &n, ఖాళీ) || n == 0)
	    ఆపు;
	ఒకవేళ (ga_grow(&బహిర్గతం_ga, (పూర్ణాంకం)n) == OK)
	{
	    mch_memmove((అక్షరం *)బహిర్గతం_ga.ga_దత్తాంశం + బహిర్గతం_ga.ga_len, buf, n);
	    బహిర్గతం_ga.ga_len += (పూర్ణాంకం)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    Get_Exit_సంకేతం_Process(pi.hProcess, &exit_సంకేతం);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    సమితి_vim_var_nr(VV_SHELL_ERROR, (దీర్ఘ)exit_సంకేతం);

    ఒకవేళ (బహిర్గతం_ga.ga_len > 0)
    {
	బఫరు = alloc(బహిర్గతం_ga.ga_len + 1);
	ఒకవేళ (బఫరు != ఖాళీ)
	{
	    mch_memmove(బఫరు, బహిర్గతం_ga.ga_దత్తాంశం, బహిర్గతం_ga.ga_len);
	    ఒకవేళ (ret_len == ఖాళీ)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		కొరకు (i = 0; i < బహిర్గతం_ga.ga_len; ++i)
		    ఒకవేళ (బఫరు[i] == NUL)
			బఫరు[i] = 1;
		బఫరు[బహిర్గతం_ga.ga_len] = NUL;
	    }
	    లేకపోతే
		*ret_len = బహిర్గతం_ga.ga_len;
	}
    }
    లేకపోతే ఒకవేళ (ret_len != ఖాళీ)
	*ret_len = 0;

పూర్తి:
    ga_clear(&cmd_ga);
    ga_clear(&బహిర్గతం_ga);
    ఒకవేళ (h_బిడ్డ_Stdout_Rd != INVALID_HANDLE_విలువ)
	CloseHandle(h_బిడ్డ_Stdout_Rd);
    ఒకవేళ (h_బిడ్డ_Stdout_Wr != INVALID_HANDLE_విలువ)
	CloseHandle(h_బిడ్డ_Stdout_Wr);
    ఒకవేళ (h_బిడ్డ_Stdin_Rd != INVALID_HANDLE_విలువ)
	CloseHandle(h_బిడ్డ_Stdin_Rd);
    తిరిగి బఫరు;
}
#అయితే_ముగింపు // FEAT_EVAL

#ఒకవేళ defined(FEAT_పని_CHANNEL)
    శూన్య
mch_పని_ప్రారంభం(అక్షరం *cmd, పని_T *పని, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_లక్షణాలు saAttr;
    channel_T		*channel = ఖాళీ;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    పూర్ణాంకం		use_null_for_లోపల = options->jo_io[PART_లోపల] == JIO_NULL;
    పూర్ణాంకం		use_null_for_బహిర్గతం = options->jo_io[PART_బహిర్గతం] == JIO_NULL;
    పూర్ణాంకం		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    పూర్ణాంకం		use_file_for_లోపల = options->jo_io[PART_లోపల] == JIO_FILE;
    పూర్ణాంకం		use_file_for_బహిర్గతం = options->jo_io[PART_బహిర్గతం] == JIO_FILE;
    పూర్ణాంకం		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    పూర్ణాంకం		use_బహిర్గతం_for_err = options->jo_io[PART_ERR] == JIO_బహిర్గతం;

    ఒకవేళ (use_బహిర్గతం_for_err && use_null_for_బహిర్గతం)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_విలువ;
    ifd[1] = INVALID_HANDLE_విలువ;
    ofd[0] = INVALID_HANDLE_విలువ;
    ofd[1] = INVALID_HANDLE_విలువ;
    efd[0] = INVALID_HANDLE_విలువ;
    efd[1] = INVALID_HANDLE_విలువ;
    ga_init2(&ga, పరిమాణం(wchar_t), 500);

    jo = Create_పని_Object(ఖాళీ, ఖాళీ);
    ఒకవేళ (jo == ఖాళీ)
    {
	పని->jv_స్థితి = పని_FAILED;
	వెళ్ళు failed;
    }

    ఒకవేళ (options->jo_env != ఖాళీ)
	win32_build_env(options->jo_env, &ga, FALSE);

    Zero_జ్ఞాపకం(&pi, పరిమాణం(pi));
    Zero_జ్ఞాపకం(&si, పరిమాణం(si));
    si.cb = పరిమాణం(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.w_Show_కిటికీ = SW_HIDE;

    saAttr.n_నిడివి = పరిమాణం(SECURITY_లక్షణాలు);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = ఖాళీ;

    ఒకవేళ (use_file_for_లోపల)
    {
	char_u *fname = options->jo_io_name[PART_లోపల];

	ifd[0] = పని_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_సాధారణ);
	ఒకవేళ (ifd[0] == INVALID_HANDLE_విలువ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    వెళ్ళు failed;
	}
    }
    లేకపోతే ఒకవేళ (!use_null_for_లోపల
	    && (!create_pipe_pair(ifd)
		|| !సమితి_Handle_Information(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	వెళ్ళు failed;

    ఒకవేళ (use_file_for_బహిర్గతం)
    {
	char_u *fname = options->jo_io_name[PART_బహిర్గతం];

	ofd[1] = పని_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_సాధారణ);
	ఒకవేళ (ofd[1] == INVALID_HANDLE_విలువ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    వెళ్ళు failed;
	}
    }
    లేకపోతే ఒకవేళ (!use_null_for_బహిర్గతం &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !సమితి_Handle_Information(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	వెళ్ళు failed;

    ఒకవేళ (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = పని_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_సాధారణ);
	ఒకవేళ (efd[1] == INVALID_HANDLE_విలువ)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    వెళ్ళు failed;
	}
    }
    లేకపోతే ఒకవేళ (!use_బహిర్గతం_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !సమితి_Handle_Information(efd[0], HANDLE_FLAG_INHERIT, 0)))
	వెళ్ళు failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_ఆదానం = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_బహిర్గతం_for_err ? ofd[1] : efd[1];

    ఒకవేళ (!use_null_for_లోపల || !use_null_for_బహిర్గతం || !use_null_for_err)
    {
	ఒకవేళ (options->jo_సమితి & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    ఒకవేళ (channel != ఖాళీ)
		++channel->ch_refcount;
	}
	లేకపోతే
	    channel = కూడు_channel();
	ఒకవేళ (channel == ఖాళీ)
	    వెళ్ళు failed;
    }

    ఒకవేళ (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_బహుళకం |
	    CREATE_NEW_PROCESS_సమూహం |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_దత్తాంశం,
	    (అక్షరం *)options->jo_cwd))
    {
	CloseHandle(jo);
	పని->jv_స్థితి = పని_FAILED;
	వెళ్ళు failed;
    }

    ga_clear(&ga);

    ఒకవేళ (!కేటాయించు_Process_గమ్యం_పని_Object(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = ఖాళీ;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    పని->jv_proc_info = pi;
    పని->jv_పని_object = jo;
    పని->jv_స్థితి = పని_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    ఒకవేళ (!use_బహిర్గతం_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    పని->jv_channel = channel;
    ఒకవేళ (channel != ఖాళీ)
    {
	channel_సమితి_pipes(channel,
		      use_file_for_లోపల || use_null_for_లోపల
					      ? INVALID_FD : (sock_T)ifd[1],
		      use_file_for_బహిర్గతం || use_null_for_బహిర్గతం
					     ? INVALID_FD : (sock_T)ofd[0],
		      use_బహిర్గతం_for_err || use_file_for_err || use_null_for_err
					    ? INVALID_FD : (sock_T)efd[0]);
	channel_సమితి_పని(channel, పని, options);
    }
    తిరిగి;

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

    అక్షరం *
mch_పని_స్థితి(పని_T *పని)
{
    DWORD dw_Exit_సంకేతం = 0;

    ఒకవేళ (!Get_Exit_సంకేతం_Process(పని->jv_proc_info.hProcess, &dw_Exit_సంకేతం)
	    || dw_Exit_సంకేతం != STILL_చురుకైన)
    {
	పని->jv_exitval = (పూర్ణాంకం)dw_Exit_సంకేతం;
	ఒకవేళ (పని->jv_స్థితి < పని_ENDED)
	{
	    ch_log(పని->jv_channel, "Job ended");
	    పని->jv_స్థితి = పని_ENDED;
	}
	తిరిగి "dead";
    }
    తిరిగి "run";
}

    పని_T *
mch_detect_ended_పని(పని_T *పని_list)
{
    HANDLE పని_Handles[గరిష్ఠ_WAIT_OBJECTS];
    పని_T *పని_Array[గరిష్ఠ_WAIT_OBJECTS];
    పని_T *పని = పని_list;

    అయితే (పని != ఖాళీ)
    {
	DWORD n;
	DWORD ఫలితం;

	కొరకు (n = 0; n < గరిష్ఠ_WAIT_OBJECTS
				       && పని != ఖాళీ; పని = పని->jv_తదుపరి)
	{
	    ఒకవేళ (పని->jv_స్థితి == పని_STARTED)
	    {
		పని_Handles[n] = పని->jv_proc_info.hProcess;
		పని_Array[n] = పని;
		++n;
	    }
	}
	ఒకవేళ (n == 0)
	    కొనసాగు;
	ఫలితం = Wait_For_గుణిజం_Objects(n, పని_Handles, FALSE, 0);
	ఒకవేళ (ఫలితం >= WAIT_OBJECT_0 && ఫలితం < WAIT_OBJECT_0 + n)
	{
	    పని_T *wait_పని = పని_Array[ఫలితం - WAIT_OBJECT_0];

	    ఒకవేళ (STRCMP(mch_పని_స్థితి(wait_పని), "dead") == 0)
		తిరిగి wait_పని;
	}
    }
    తిరిగి ఖాళీ;
}

    స్థిర BOOL
terminate_all(HANDLE process, పూర్ణాంకం సంకేతం_2)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_విలువ;
    DWORD	    pid = GetProcessId(process);

    ఒకవేళ (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	ఒకవేళ (h != INVALID_HANDLE_విలువ)
	{
	    pe.dw_కొలత = పరిమాణం(PROCESSENTRY32);
	    ఒకవేళ (!Process32_మొదటి(h, &pe))
		వెళ్ళు theend;

	    చేయి
	    {
		ఒకవేళ (pe.th32_మాతృ_Process_ID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    ఒకవేళ (ph != ఖాళీ)
		    {
			terminate_all(ph, సంకేతం_2);
			CloseHandle(ph);
		    }
		}
	    } అయితే (Process32_తదుపరి(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    తిరిగి TerminateProcess(process, సంకేతం_2);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    పూర్ణాంకం
mch_signal_పని(పని_T *పని, char_u *how)
{
    పూర్ణాంకం ret;

    ఒకవేళ (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	ఒకవేళ (పని->jv_పని_object != ఖాళీ)
	{
	    ఒకవేళ (పని->jv_channel != ఖాళీ && పని->jv_channel->ch_anonymous_pipe)
		పని->jv_channel->ch_killing = TRUE;
	    తిరిగి Terminate_పని_Object(పని->jv_పని_object, (UINT)-1) ? OK : FAIL;
	}
	తిరిగి terminate_all(పని->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    ఒకవేళ (!AttachConsole(పని->jv_proc_info.dwProcessId))
	తిరిగి FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		పని->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    తిరిగి ret;
}

/*
 * Clear the data related to "job".
 */
    శూన్య
mch_clear_పని(పని_T *పని)
{
    ఒకవేళ (పని->jv_స్థితి == పని_FAILED)
	తిరిగి;

    ఒకవేళ (పని->jv_పని_object != ఖాళీ)
	CloseHandle(పని->jv_పని_object);
    CloseHandle(పని->jv_proc_info.hProcess);
}
#అయితే_ముగింపు


#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    స్థిర శూన్య
termcap_బహుళకం_ప్రారంభం(శూన్య)
{
    DWORD cmodein;

    ఒకవేళ (g_f_Termcap_బహుళకం)
	తిరిగి;

    Save_Console_బఫరు(&g_cbNonTermcap);

    ఒకవేళ (g_cbTermcap.IsValid)
    {
	/*
	 * We've been in termcap mode before.  Restore certain screen
	 * characteristics, including the buffer size and the window
	 * size.  Since we will be redrawing the screen, we don't need
	 * to restore the actual contents of the buffer.
	 */
	Restore_Console_బఫరు(&g_cbTermcap, FALSE);
	reset_console_color_rgb();
	సమితి_Console_కిటికీ_Info(g_h_Con_బహిర్గతం, TRUE, &g_cbTermcap.Info.sr_కిటికీ);
	అడ్డువరుసలు = g_cbTermcap.Info.dw_కొలత.Y;
	Columns = g_cbTermcap.Info.dw_కొలత.X;
    }
    లేకపోతే
    {
	/*
	 * This is our first time entering termcap mode.  Clear the console
	 * screen buffer, and resize the buffer to match the current window
	 * size.  We will use this as the size of our editing environment.
	 */
	Clear_Console_బఫరు(g_attrCurrent);
	సమితి_console_color_rgb();
	Resize_Con_Buf_And_కిటికీ(g_h_Con_బహిర్గతం, Columns, అడ్డువరుసలు);
    }

    resettitle();

    Get_Console_బహుళకం(g_h_Con_లోపల, &cmodein);
    ఒకవేళ (g_f_Mouse_చురుకైన)
    {
	cmodein |= ENABLE_MOUSE_ఆదానం;
	cmodein &= ~ENABLE_శీఘ్ర_EDIT_బహుళకం;
    }
    లేకపోతే
    {
	cmodein &= ~ENABLE_MOUSE_ఆదానం;
	cmodein |= g_cmodein & ENABLE_శీఘ్ర_EDIT_బహుళకం;
    }
    cmodein |= ENABLE_కిటికీ_ఆదానం;
    సమితి_Console_బహుళకం(g_h_Con_లోపల, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_f_Termcap_బహుళకం = TRUE;
}


/*
 * End termcap mode
 */
    స్థిర శూన్య
termcap_బహుళకం_చివర(శూన్య)
{
    DWORD cmodein;
    Console_బఫరు *cb;
    COORD coord;
    DWORD dwDummy;

    ఒకవేళ (!g_f_Termcap_బహుళకం)
	తిరిగి;

    Save_Console_బఫరు(&g_cbTermcap);

    Get_Console_బహుళకం(g_h_Con_లోపల, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_ఆదానం | ENABLE_కిటికీ_ఆదానం);
    cmodein |= g_cmodein & ENABLE_శీఘ్ర_EDIT_బహుళకం;
    సమితి_Console_బహుళకం(g_h_Con_లోపల, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# లేకపోతే
    cb = &g_cbNonTermcap;
# endif
    Restore_Console_బఫరు(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    ఒకవేళ (exiting && use_alternate_screen_బఫరు)
	vtp_printf("\033[?1049l");

    ఒకవేళ (!USE_WT && (p_rs || exiting))
    {
	/*
	 * Clear anything that happens to be on the current line.
	 */
	coord.X = 0;
	coord.Y = (SHORT) (p_rs ? cb->Info.dwCursorPosition.Y : (అడ్డువరుసలు - 1));
	FillConsoleOutputCharacter(g_h_Con_బహిర్గతం, ' ',
		cb->Info.dw_కొలత.X, coord, &dwDummy);
	/*
	 * The following is just for aesthetics.  If we are exiting without
	 * restoring the screen, then we want to have a prompt string
	 * appear at the bottom line.  However, the command interpreter
	 * seems to always advance the cursor one line before displaying
	 * the prompt string, which causes the screen to scroll.  To
	 * counter this, move the cursor up one line before exiting.
	 */
	ఒకవేళ (exiting && !p_rs)
	    coord.Y--;
	/*
	 * Position the cursor at the leftmost column of the desired row.
	 */
	సమితి_Console_Cursor_Position(g_h_Con_బహిర్గతం, coord);
    }
    సమితి_Console_Cursor_Info(g_h_Con_బహిర్గతం, &g_cci);
    g_f_Termcap_బహుళకం = FALSE;
}
#అయితే_ముగింపు


#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    స్థిర శూన్య
clear_chars(
    COORD coord,
    DWORD n)
{
    ఒకవేళ (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_h_Con_బహిర్గతం, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_h_Con_బహిర్గతం, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    లేకపోతే
    {
	సమితి_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    స్థిర శూన్య
clear_screen(శూన్య)
{
    g_coord.X = g_coord.Y = 0;

    ఒకవేళ (!vtp_working)
	clear_chars(g_coord, అడ్డువరుసలు * Columns);
    లేకపోతే
    {
	సమితి_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    స్థిర శూన్య
clear_గమ్యం_చివర_యొక్క_display(శూన్య)
{
    COORD save = g_coord;

    ఒకవేళ (!vtp_working)
	clear_chars(g_coord, (అడ్డువరుసలు - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    లేకపోతే
    {
	సమితి_console_color_rgb();
	gotoxy(g_coord.X + 1, g_coord.Y + 1);
	vtp_printf("\033[0J");

	gotoxy(save.X + 1, save.Y + 1);
	g_coord = save;
    }
}


/*
 * Clear to end of line
 */
    స్థిర శూన్య
clear_గమ్యం_చివర_యొక్క_పంక్తి(శూన్య)
{
    COORD save = g_coord;

    ఒకవేళ (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    లేకపోతే
    {
	సమితి_console_color_rgb();
	gotoxy(g_coord.X + 1, g_coord.Y + 1);
	vtp_printf("\033[0K");

	gotoxy(save.X + 1, save.Y + 1);
	g_coord = save;
    }
}


/*
 * Scroll the scroll region up by `cLines' lines
 */
    స్థిర శూన్య
scroll(గుర్తులేని c_పంక్తులు)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.ఎడమ_4 + 1, g_srScrollRegion.శిఖరం_2 + 1);
    delete_పంక్తులు(c_పంక్తులు);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    స్థిర శూన్య
సమితి_scroll_region(
    గుర్తులేని ఎడమ_2,
    గుర్తులేని శిఖరం,
    గుర్తులేని కుడి_2,
    గుర్తులేని bottom)
{
    ఒకవేళ (ఎడమ_2 >= కుడి_2
	    || శిఖరం >= bottom
	    || కుడి_2 > (గుర్తులేని) Columns - 1
	    || bottom > (గుర్తులేని) అడ్డువరుసలు - 1)
	తిరిగి;

    g_srScrollRegion.ఎడమ_4 =   ఎడమ_2;
    g_srScrollRegion.శిఖరం_2 =    శిఖరం;
    g_srScrollRegion.కుడి_4 =  కుడి_2;
    g_srScrollRegion.Bottom = bottom;
}

    స్థిర శూన్య
సమితి_scroll_region_tb(
    గుర్తులేని శిఖరం,
    గుర్తులేని bottom)
{
    ఒకవేళ (శిఖరం >= bottom || bottom > (గుర్తులేని)అడ్డువరుసలు - 1)
	తిరిగి;

    g_srScrollRegion.శిఖరం_2 = శిఖరం;
    g_srScrollRegion.Bottom = bottom;
}

    స్థిర శూన్య
సమితి_scroll_region_lr(
    గుర్తులేని ఎడమ_2,
    గుర్తులేని కుడి_2)
{
    ఒకవేళ (ఎడమ_2 >= కుడి_2 || కుడి_2 > (గుర్తులేని)Columns - 1)
	తిరిగి;

    g_srScrollRegion.ఎడమ_4 = ఎడమ_2;
    g_srScrollRegion.కుడి_4 = కుడి_2;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    స్థిర శూన్య
చొప్పించు_పంక్తులు(గుర్తులేని c_పంక్తులు)
{
    SMALL_RECT	    ఆధారం, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.ఎడమ_4 + 1, g_srScrollRegion.శిఖరం_2 + 1);

    dest.X = g_srScrollRegion.ఎడమ_4;
    dest.Y = g_coord.Y + c_పంక్తులు;

    ఆధారం.ఎడమ_4   = g_srScrollRegion.ఎడమ_4;
    ఆధారం.శిఖరం_2	  = g_coord.Y;
    ఆధారం.కుడి_4  = g_srScrollRegion.కుడి_4;
    ఆధారం.Bottom = g_srScrollRegion.Bottom - c_పంక్తులు;

    clip.ఎడమ_4   = g_srScrollRegion.ఎడమ_4;
    clip.శిఖరం_2    = g_coord.Y;
    clip.కుడి_4  = g_srScrollRegion.కుడి_4;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    ఒకవేళ (!USE_VTP)
	fill.లక్షణాలు_2 = g_attrCurrent;
    లేకపోతే
	fill.లక్షణాలు_2 = g_attrDefault;

    సమితి_console_color_rgb();

    Scroll_Console_Screen_బఫరు(g_h_Con_బహిర్గతం, &ఆధారం, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    ఒకవేళ (ఆధారం.Bottom < dest.Y)
    {
	COORD coord;
	పూర్ణాంకం   i;

	coord.X = ఆధారం.ఎడమ_4;
	కొరకు (i = clip.శిఖరం_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ఆధారం.కుడి_4 - ఆధారం.ఎడమ_4 + 1);
	}
    }

    ఒకవేళ (vtp_working)
    {
	COORD coord;
	పూర్ణాంకం i;

	coord.X = ఆధారం.ఎడమ_4;
	కొరకు (i = ఆధారం.శిఖరం_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ఆధారం.కుడి_4 - ఆధారం.ఎడమ_4 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    స్థిర శూన్య
delete_పంక్తులు(గుర్తులేని c_పంక్తులు)
{
    SMALL_RECT	    ఆధారం, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    పూర్ణాంకం		    nb;

    gotoxy(g_srScrollRegion.ఎడమ_4 + 1, g_srScrollRegion.శిఖరం_2 + 1);

    dest.X = g_srScrollRegion.ఎడమ_4;
    dest.Y = g_coord.Y;

    ఆధారం.ఎడమ_4   = g_srScrollRegion.ఎడమ_4;
    ఆధారం.శిఖరం_2	  = g_coord.Y + c_పంక్తులు;
    ఆధారం.కుడి_4  = g_srScrollRegion.కుడి_4;
    ఆధారం.Bottom = g_srScrollRegion.Bottom;

    clip.ఎడమ_4   = g_srScrollRegion.ఎడమ_4;
    clip.శిఖరం_2    = g_coord.Y;
    clip.కుడి_4  = g_srScrollRegion.కుడి_4;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    ఒకవేళ (!vtp_working)
	fill.లక్షణాలు_2 = g_attrCurrent;
    లేకపోతే
	fill.లక్షణాలు_2 = g_attrDefault;

    సమితి_console_color_rgb();

    Scroll_Console_Screen_బఫరు(g_h_Con_బహిర్గతం, &ఆధారం, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (ఆధారం.Bottom - ఆధారం.శిఖరం_2) + 1;

    ఒకవేళ (nb < ఆధారం.శిఖరం_2)
    {
	COORD coord;
	పూర్ణాంకం   i;

	coord.X = ఆధారం.ఎడమ_4;
	కొరకు (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ఆధారం.కుడి_4 - ఆధారం.ఎడమ_4 + 1);
	}
    }

    ఒకవేళ (vtp_working)
    {
	COORD coord;
	పూర్ణాంకం i;

	coord.X = ఆధారం.ఎడమ_4;
	కొరకు (i = nb; i <= ఆధారం.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ఆధారం.కుడి_4 - ఆధారం.ఎడమ_4 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    స్థిర శూన్య
gotoxy(
    గుర్తులేని x,
    గుర్తులేని y)
{
    ఒకవేళ (x < 1 || x > (గుర్తులేని)Columns || y < 1 || y > (గుర్తులేని)అడ్డువరుసలు)
	తిరిగి;

    ఒకవేళ (!USE_VTP)
    {
	// There are reports of double-width characters not displayed
	// correctly.  This workaround should fix it, similar to how it's done
	// for VTP.
	g_coord.X = 0;
	సమితి_Console_Cursor_Position(g_h_Con_బహిర్గతం, g_coord);

	// external cursor coords are 1-based; internal are 0-based
	g_coord.X = x - 1;
	g_coord.Y = y - 1;
	సమితి_Console_Cursor_Position(g_h_Con_బహిర్గతం, g_coord);
    }
    లేకపోతే
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
    స్థిర శూన్య
textattr(మాట_2 wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    సమితి_Console_పాఠ్యం_Attribute(g_h_Con_బహిర్గతం, wAttr);
}


    స్థిర శూన్య
textcolor(మాట_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    ఒకవేళ (!vtp_working)
	సమితి_Console_పాఠ్యం_Attribute(g_h_Con_బహిర్గతం, g_attrCurrent);
    లేకపోతే
	vtp_sgr_bulk(wAttr);
}


    స్థిర శూన్య
textbackground(మాట_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    ఒకవేళ (!vtp_working)
	సమితి_Console_పాఠ్యం_Attribute(g_h_Con_బహిర్గతం, g_attrCurrent);
    లేకపోతే
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    స్థిర శూన్య
normvideo(శూన్య)
{
    ఒకవేళ (!vtp_working)
	textattr(g_attrDefault);
    లేకపోతే
	vtp_sgr_bulk(0);
}


స్థిర మాట_2 g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    స్థిర శూన్య
standout(శూన్య)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((మాట_2) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    స్థిర శూన్య
standend(శూన్య)
{
    ఒకవేళ (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    శూన్య
mch_సమితి_సాధారణ_colors(శూన్య)
{
    char_u	*p;
    పూర్ణాంకం		n;

    cterm_సాధారణ_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_సాధారణ_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    ఒకవేళ (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	ఒకవేళ (*p == 'm' && n > 0)
	{
	    cterm_సాధారణ_fg_color = (n & 0xf) + 1;
	    cterm_సాధారణ_bg_color = ((n >> 4) & 0xf) + 1;
	}
    }
# ifdef FEAT_TERMGUICOLORS
    cterm_సాధారణ_fg_gui_color = INVALCOLOR;
    cterm_సాధారణ_bg_gui_color = INVALCOLOR;
# endif
}


/*
 * visual bell: flash the screen
 */
    స్థిర శూన్య
visual_bell(శూన్య)
{
    COORD   coordOrigin = {0, 0};
    మాట_2    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = ఖాళీ;

# ifdef FEAT_TERMGUICOLORS
    ఒకవేళ (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(మాట_2, అడ్డువరుసలు * Columns);
	ఒకవేళ (oldattrs == ఖాళీ)
	    తిరిగి;
	ReadConsoleOutputAttribute(g_h_Con_బహిర్గతం, oldattrs, అడ్డువరుసలు * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_h_Con_బహిర్గతం, attrFlash, అడ్డువరుసలు * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    ఒకవేళ (oldattrs != ఖాళీ)
    {
	WriteConsoleOutputAttribute(g_h_Con_బహిర్గతం, oldattrs, అడ్డువరుసలు * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    స్థిర శూన్య
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    ఒకవేళ (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	తిరిగి;
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
    స్థిర DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cb_గమ్యం_Write)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    స్థిర DWORD    cells;
    స్థిర WCHAR    *unicodebuf = ఖాళీ;
    స్థిర పూర్ణాంకం	    unibuflen = 0;
    స్థిర పూర్ణాంకం	    నిడివి;
    పూర్ణాంకం		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    స్థిర WCHAR    *utf8spbuf = ఖాళీ;
    స్థిర పూర్ణాంకం	    utf8splength;
    స్థిర DWORD    utf8spcells;
    స్థిర WCHAR    **utf8usingbuf = &unicodebuf;

    ఒకవేళ (cb_గమ్యం_Write != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	చేయి
	{
	    నిడివి = Multi_Byte_గమ్యం_Wide_Char(cp, 0, (LPCSTR)pchBuf, cb_గమ్యం_Write,
							unicodebuf, unibuflen);
	    ఒకవేళ (నిడివి && నిడివి <= unibuflen)
		ఆపు;
	    vim_free(unicodebuf);
	    unicodebuf = నిడివి ? LALLOC_MULT(WCHAR, నిడివి) : ఖాళీ;
	    unibuflen = unibuflen ? 0 : నిడివి;
	} అయితే (TRUE);
	cells = mb_string2cells(pchBuf, cb_గమ్యం_Write);
    }
    లేకపోతే // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	ఒకవేళ (utf8usingbuf != &utf8spbuf)
	{
	    ఒకవేళ (utf8spbuf == ఖాళీ)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		నిడివి = Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, 0, " ", 1, ఖాళీ, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, నిడివి);
		ఒకవేళ (utf8spbuf != ఖాళీ)
		{
		    Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, 0, " ", 1, utf8spbuf, నిడివి);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = నిడివి;
		    utf8spcells = cells;
		}
	    }
	    లేకపోతే
	    {
		utf8usingbuf = &utf8spbuf;
		నిడివి = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    ఒకవేళ (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_h_Con_బహిర్గతం, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	ఒకవేళ (WriteConsoleOutputCharacterW(g_h_Con_బహిర్గతం, *utf8usingbuf, నిడివి,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    లేకపోతే
    {
	ఒకవేళ (WriteConsoleW(g_h_Con_బహిర్గతం, *utf8usingbuf, నిడివి, &cchwritten,
		    ఖాళీ) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    ఒకవేళ (cchwritten == (DWORD)నిడివి)
    {
	written = cb_గమ్యం_Write;
	g_coord.X += (SHORT)cells;
    }
    లేకపోతే
    {
	char_u *p = pchBuf;
	కొరకు (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    అయితే (g_coord.X > g_srScrollRegion.కుడి_4)
    {
	g_coord.X -= (SHORT) Columns;
	ఒకవేళ (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    ఒకవేళ (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    తిరిగి written;
}

    స్థిర char_u *
get_seq(
    పూర్ణాంకం *వాదనలు,
    పూర్ణాంకం *లెక్క,
    char_u *తల)
{
    పూర్ణాంకం argc;
    char_u *p;

    ఒకవేళ (తల == ఖాళీ || *తల != '\033')
	తిరిగి ఖాళీ;

    argc = 0;
    p = తల;
    ++p;
    చేయి
    {
	++p;
	వాదనలు[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } అయితే (*p == ';');
    *లెక్క = argc;

    తిరిగి p;
}

    స్థిర char_u *
get_sgr(
    పూర్ణాంకం *వాదనలు,
    పూర్ణాంకం *లెక్క,
    char_u *తల)
{
    char_u *p = get_seq(వాదనలు, లెక్క, తల);

    తిరిగి (p && *p == 'm') ? ++p : ఖాళీ;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    స్థిర char_u *
sgrn2(
    char_u *తల,
    పూర్ణాంకం	   n)
{
    పూర్ణాంకం argc;
    పూర్ణాంకం వాదనలు[16];
    char_u *p = get_sgr(వాదనలు, &argc, తల);

    తిరిగి p && argc == 5 && వాదనలు[0] == n && వాదనలు[1] == 2 ? p : ఖాళీ;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    స్థిర char_u *
sgrnc(
    char_u *తల,
    పూర్ణాంకం	   n)
{
    పూర్ణాంకం argc;
    పూర్ణాంకం వాదనలు[16];
    char_u *p = get_sgr(వాదనలు, &argc, తల);

    తిరిగి p && argc == 1 && వాదనలు[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : ఖాళీ;
}

    స్థిర char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    అయితే (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    తిరిగి p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    స్థిర char_u *
sgrn2c(
    char_u *తల,
    పూర్ణాంకం	   n)
{
    char_u *p = sgrn2(తల, n);

    తిరిగి p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : ఖాళీ;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    స్థిర char_u *
sgrn2cn(
    char_u *తల,
    పూర్ణాంకం n)
{
    char_u *p = sgrn2(తల, n);

    తిరిగి p && p[0] == 0x0a && p[1] == '\033' ? ++p : ఖాళీ;
}

స్థిర bool లోపల_dcs = false;	// inside a DCS (sixel) sequence
స్థిర bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    స్థిర శూన్య
write_vt(char_u *s, పూర్ణాంకం len)
{
    పూర్ణాంకం		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, ఖాళీ, 0);
    ఒకవేళ (wlen <= 0)
	తిరిగి;
    ws = ALLOC_MULT(WCHAR, wlen);
    ఒకవేళ (ws == ఖాళీ)
	తిరిగి;
    Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_h_Con_బహిర్గతం, ws, wlen, &written, ఖాళీ);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    స్థిర char_u *
skip_dcs(char_u *s, char_u *చివర)
{
    కొరకు (char_u *p = s; p < చివర; ++p)
    {
	ఒకవేళ (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    లోపల_dcs = false;
	    తిరిగి p + 1;
	}
	dcs_esc = *p == ESC;
    }
    తిరిగి చివర;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    శూన్య
mch_write(
    char_u  *s,
    పూర్ణాంకం	    len)
{
    char_u  *చివర = s + len;

# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
	తిరిగి;
# endif

    ఒకవేళ (!పదం_console)
    {
	లిఖ(1, s, (గుర్తులేని)len);
	తిరిగి;
    }

    // translate ESC | sequences into faked bios calls
    అయితే (len--)
    {
	పూర్ణాంకం ఉపసర్గ = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	ఒకవేళ (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    తిరిగి;
	}

	ఒకవేళ (లోపల_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    పూర్ణాంకం	l = (పూర్ణాంకం)(skip_dcs(s, చివర) - s);

	    ఒకవేళ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    కొనసాగు;
	}

	అయితే (s + ++ఉపసర్గ < చివర)
	{
	    ch = s[ఉపసర్గ];
	    ఒకవేళ (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		ఆపు;
	}

	ఒకవేళ (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    ఒకవేళ (ఉపసర్గ != 0)
		ఉపసర్గ = 1;
	}

	ఒకవేళ (ఉపసర్గ != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, ఉపసర్గ);
# ifdef MCH_WRITE_DUMP
	    ఒకవేళ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, పరిమాణం(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	లేకపోతే ఒకవేళ (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    ఒకవేళ (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.ఎడమ_4 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    లేకపోతే
	    {
		gotoxy(g_srScrollRegion.ఎడమ_4 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    ఒకవేళ (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	లేకపోతే ఒకవేళ (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.ఎడమ_4+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ఒకవేళ (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	లేకపోతే ఒకవేళ (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    ఒకవేళ (g_coord.X > g_srScrollRegion.ఎడమ_4)
		g_coord.X--;
	    లేకపోతే ఒకవేళ (g_coord.Y > g_srScrollRegion.శిఖరం_2)
	    {
		g_coord.X = g_srScrollRegion.కుడి_4;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    ఒకవేళ (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	లేకపోతే ఒకవేళ (s[0] == '\a')
	{
	    // \a, bell
	    సందేశం_Beep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    ఒకవేళ (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	లేకపోతే ఒకవేళ (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    పూర్ణాంకం	    arg1 = 0, arg2 = 0, argc = 0, వాదనలు[16];
	    char_u  *sp;

	    ఎంపిక (s[2])
	    {
	    స్థితి '0': స్థితి '1': స్థితి '2': స్థితి '3': స్థితి '4':
	    స్థితి '5': స్థితి '6': స్థితి '7': స్థితి '8': స్థితి '9':
		ఒకవేళ (*(p = get_seq(వాదనలు, &argc, s)) != 'm')
		    వెళ్ళు notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		ఒకవేళ (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (పూర్ణాంకం)(p - s);
		    s = p;
		    ఆపు;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		ఒకవేళ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		ఒకవేళ (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		ఒకవేళ (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		ఒకవేళ (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(వాదనలు, &argc, p);

notsgr:
		arg1 = వాదనలు[0];
		arg2 = వాదనలు[1];
		ఒకవేళ (*p == 'm')
		{
		    ఒకవేళ (argc == 1 && వాదనలు[0] == 0)
			normvideo();
		    లేకపోతే ఒకవేళ (argc == 1)
		    {
			ఒకవేళ (USE_VTP)
			    textcolor((మాట_2)arg1);
			లేకపోతే
			    textattr((మాట_2)arg1);
		    }
		    లేకపోతే ఒకవేళ (vtp_working)
			vtp_sgr_bulks(argc, వాదనలు);
		}
		లేకపోతే ఒకవేళ (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		లేకపోతే ఒకవేళ (argc == 2 && *p == 'r')
		{
		    సమితి_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		లేకపోతే ఒకవేళ (argc == 2 && *p == 'R')
		{
		    సమితి_scroll_region_tb(arg1, arg2);
		}
		లేకపోతే ఒకవేళ (argc == 2 && *p == 'V')
		{
		    సమితి_scroll_region_lr(arg1, arg2);
		}
		లేకపోతే ఒకవేళ (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.శిఖరం_2, g_coord.Y - arg1) + 1);
		}
		లేకపోతే ఒకవేళ (argc == 1 && *p == 'b')
		{
		    textbackground((మాట_2) arg1);
		}
		లేకపోతే ఒకవేళ (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.కుడి_4, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		లేకపోతే ఒకవేళ (argc == 1 && *p == 'f')
		{
		    textcolor((మాట_2) arg1);
		}
		లేకపోతే ఒకవేళ (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		లేకపోతే ఒకవేళ (argc == 1 && *p == 'L')
		{
		    చొప్పించు_పంక్తులు(arg1);
		}
		లేకపోతే ఒకవేళ (argc == 1 && *p == 'M')
		{
		    delete_పంక్తులు(arg1);
		}

		len -= (పూర్ణాంకం)(p - s);
		s = p + 1;
		ఆపు;

	    స్థితి 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.శిఖరం_2, g_coord.Y - 1) + 1);
		వెళ్ళు got3;

	    స్థితి 'B':
		visual_bell();
		వెళ్ళు got3;

	    స్థితి 'C':
		gotoxy(min(g_srScrollRegion.కుడి_4, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		వెళ్ళు got3;

	    స్థితి 'E':
		termcap_బహుళకం_చివర();
		వెళ్ళు got3;

	    స్థితి 'F':
		standout();
		వెళ్ళు got3;

	    స్థితి 'f':
		standend();
		వెళ్ళు got3;

	    స్థితి 'H':
		gotoxy(1, 1);
		వెళ్ళు got3;

	    స్థితి 'j':
		clear_గమ్యం_చివర_యొక్క_display();
		వెళ్ళు got3;

	    స్థితి 'J':
		clear_screen();
		వెళ్ళు got3;

	    స్థితి 'K':
		clear_గమ్యం_చివర_యొక్క_పంక్తి();
		వెళ్ళు got3;

	    స్థితి 'L':
		చొప్పించు_పంక్తులు(1);
		వెళ్ళు got3;

	    స్థితి 'M':
		delete_పంక్తులు(1);
		వెళ్ళు got3;

	    స్థితి 'S':
		termcap_బహుళకం_ప్రారంభం();
		వెళ్ళు got3;

	    స్థితి 'V':
		cursor_visible(TRUE);
		వెళ్ళు got3;

	    స్థితి 'v':
		cursor_visible(FALSE);
		వెళ్ళు got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    ఒకవేళ (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, పరిమాణం(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	లేకపోతే ఒకవేళ (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    పూర్ణాంకం l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    అయితే (s + l < చివర && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    ఒకవేళ (s + l < చివర && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		ఒకవేళ (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	లేకపోతే ఒకవేళ (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    పూర్ణాంకం	l;

	    లోపల_dcs = true;
	    dcs_esc = false;
	    l = (పూర్ణాంకం)(skip_dcs(s + 2, చివర) - s);
	    ఒకవేళ (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	లేకపోతే
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    ఒకవేళ (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, పరిమాణం(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    ఒకవేళ (fdDump)
	fflush(fdDump);
# endif
}

#లేకపోతే // FEAT_GUI_MSWIN && !VIMDLL
    శూన్య
mch_write(
    char_u  *s వాడని,
    పూర్ణాంకం	    len వాడని)
{
    // never used
}

#అయితే_ముగింపు


/*
 * Delay for "msec" milliseconds.
 */
    శూన్య
mch_delay(
    దీర్ఘ    msec,
    పూర్ణాంకం	    flags వాడని)
{
#ఒకవేళ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((పూర్ణాంకం)msec);	    // never wait for input
#లేకపోతే // Console
# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
    {
	Sleep((పూర్ణాంకం)msec);	    // never wait for input
	తిరిగి;
    }
# endif
    ఒకవేళ (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	ఒకవేళ (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    పూర్ణాంకం towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    అయితే (msec > 0)
	    {
		mzvim_check_దారాలు();
		ఒకవేళ (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	లేకపోతే
# endif
	    Sleep((పూర్ణాంకం)msec);
    లేకపోతే
	WaitForChar(msec, FALSE);
#అయితే_ముగింపు
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    పూర్ణాంకం
mch_remove(char_u *name)
{
    WCHAR	*wn;
    పూర్ణాంకం		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    ఒకవేళ (mch_isdir(name) && mch_is_symbolic_link(name))
	తిరిగి mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_సాధారణ);

    wn = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (wn == ఖాళీ)
	తిరిగి -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    తిరిగి n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    శూన్య
mch_breakcheck(పూర్ణాంకం force వాడని)
{
#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use)
# endif
	ఒకవేళ (g_fCtrlCPressed || g_fCBrkPressed)
	{
	    ctrl_break_was_pressed = g_fCBrkPressed;
	    g_fCtrlCPressed = g_fCBrkPressed = FALSE;
	    got_int = TRUE;
	}
#అయితే_ముగింపు
}

// physical RAM to leave for the OS
#నిర్వచించు WINNT_RESERVE_బైట్లు     (256*1024*1024)

/*
 * How much main memory in KiB that can be used by VIM.
 */
    long_u
mch_మొత్తం_mem(పూర్ణాంకం special వాడని)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dw_నిడివి = పరిమాణం(MEMORYSTATUSEX);
    Global_జ్ఞాపకం_స్థితి_Ex(&ms);
    ఒకవేళ (ms.ullAvailVirtual < ms.ull_మొత్తం_Phys)
    {
	// Process address space fits in physical RAM, use all of it.
	తిరిగి (long_u)(ms.ullAvailVirtual / 1024);
    }
    ఒకవేళ (ms.ull_మొత్తం_Phys <= WINNT_RESERVE_బైట్లు)
    {
	// Catch old NT box or perverse hardware setup.
	తిరిగి (long_u)((ms.ull_మొత్తం_Phys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    తిరిగి (long_u)((ms.ull_మొత్తం_Phys - WINNT_RESERVE_బైట్లు) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    పూర్ణాంకం
mch_expandpath(
    garray_T	*gap,
    char_u	*మార్గం,
    పూర్ణాంకం		flags)		// EW_* flags
{
    తిరిగి dos_expandpath(gap, మార్గం, 0, flags, FALSE);
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
    పూర్ణాంకం
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	sz_తాత్కాలిక_File[MAX_మార్గం_2 + 1];
    WCHAR	sz_New_మార్గం[MAX_మార్గం_2 + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    కొరకు (p = q = wold; *p != L'\0'; ++p)
    {
	ఎంపిక (*p)
	{
	స్థితి L'/':
	స్థితి L'\\':
	స్థితి L':':
	    ఒకవేళ (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    ఆపు;

	అప్రమేయ:
	    ఆపు;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    ఒకవేళ ((పూర్ణాంకం)(p - q) < 8 || q[6] != L'~')
	తిరిగి (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    ఒకవేళ (Get_నిండిన_మార్గం_Name_W(wnew, MAX_మార్గం_2, sz_New_మార్గం, &p) == 0 || p == ఖాళీ)
	తిరిగి -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    ఒకవేళ (Get_తాత్కాలిక_File_Name_W(sz_New_మార్గం, L"VIM", 0, sz_తాత్కాలిక_File) == 0)
	తిరిగి -2;

    // blow the temp file away
    ఒకవేళ (!DeleteFileW(sz_తాత్కాలిక_File))
	తిరిగి -3;

    // rename old file to the temp file
    ఒకవేళ (!MoveFileW(wold, sz_తాత్కాలిక_File))
	తిరిగి -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    ఒకవేళ ((hf = CreateFileW(wold, GENERIC_WRITE, 0, ఖాళీ, CREATE_NEW,
		    FILE_ATTRIBUTE_సాధారణ, ఖాళీ)) == INVALID_HANDLE_విలువ)
	తిరిగి -5;
    ఒకవేళ (!CloseHandle(hf))
	తిరిగి -6;

    // rename the temp file to the new file
    ఒకవేళ (!MoveFileW(sz_తాత్కాలిక_File, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(శూన్య)MoveFileW(sz_తాత్కాలిక_File, wold);
	తిరిగి -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(sz_తాత్కాలిక_File);

    // finally, remove the empty old file
    ఒకవేళ (!DeleteFileW(wold))
	తిరిగి -8;

    తిరిగి 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    పూర్ణాంకం
mch_rename(
    స్థిరరాశి అక్షరం	*pszOldFile,
    స్థిరరాశి అక్షరం	*pszNewFile)
{
    WCHAR	*wold = ఖాళీ;
    WCHAR	*wnew = ఖాళీ;
    పూర్ణాంకం		retval = -1;

    wold = enc_గమ్యం_utf16((char_u *)pszOldFile, ఖాళీ);
    wnew = enc_గమ్యం_utf16((char_u *)pszNewFile, ఖాళీ);
    ఒకవేళ (wold != ఖాళీ && wnew != ఖాళీ)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    తిరిగి retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    అక్షరం *
default_shell(శూన్య)
{
    తిరిగి "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    పూర్ణాంకం
mch_access(అక్షరం *n, పూర్ణాంకం p)
{
    HANDLE	hFile;
    పూర్ణాంకం		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_గమ్యం_utf16((char_u *)n, ఖాళీ);
    ఒకవేళ (wn == ఖాళీ)
	తిరిగి -1;

    ఒకవేళ (mch_isdir((char_u *)n))
    {
	WCHAR తాత్కాలిక_Name_W[MAX_మార్గం_2 + 16] = L"";

	ఒకవేళ (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    పూర్ణాంకం			i;
	    WIN32_కనుగొను_DATAW    d;

	    కొరకు (i = 0; i < MAX_మార్గం_2 && wn[i] != 0; ++i)
		తాత్కాలిక_Name_W[i] = wn[i];
	    ఒకవేళ (తాత్కాలిక_Name_W[i - 1] != '\\' && తాత్కాలిక_Name_W[i - 1] != '/')
		తాత్కాలిక_Name_W[i++] = '\\';
	    తాత్కాలిక_Name_W[i++] = '*';
	    తాత్కాలిక_Name_W[i++] = 0;

	    hFile = కనుగొను_మొదటి_File_W(తాత్కాలిక_Name_W, &d);
	    ఒకవేళ (hFile == INVALID_HANDLE_విలువ)
		వెళ్ళు getout;
	    లేకపోతే
		(శూన్య)కనుగొను_Close(hFile);
	}

	ఒకవేళ (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    ఒకవేళ (!Get_తాత్కాలిక_File_Name_W(wn, L"VIM", 0, తాత్కాలిక_Name_W))
		వెళ్ళు getout;
	    లేకపోతే
		DeleteFileW(తాత్కాలిక_Name_W);
	}
    }
    లేకపోతే
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_బహుళకం = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_బహుళకం = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_బహుళకం, share_బహుళకం,
					     ఖాళీ, OPEN_EXISTING, 0, ఖాళీ);
	ఒకవేళ (hFile == INVALID_HANDLE_విలువ)
	    వెళ్ళు getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    తిరిగి retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    పూర్ణాంకం
mch_open(స్థిరరాశి అక్షరం *name, పూర్ణాంకం flags, పూర్ణాంకం బహుళకం)
{
    WCHAR	*wn;
    పూర్ణాంకం		f;

    wn = enc_గమ్యం_utf16((char_u *)name, ఖాళీ);
    ఒకవేళ (wn == ఖాళీ)
	తిరిగి -1;

    f = _wopen(wn, flags, బహుళకం);
    vim_free(wn);
    తిరిగి f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *బహుళకం)
{
    WCHAR	*wn, *wm;
    FILE	*f = ఖాళీ;

#ఒకవేళ defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    అక్షరం new_బహుళకం = బహుళకం[అక్షరశ్రేణి_పొడవు(బహుళకం) - 1];
    పూర్ణాంకం old_బహుళకం_2 = 0;

    _get_fmode(&old_బహుళకం_2);
    ఒకవేళ (new_బహుళకం == 't')
	సమితి_fmode(O_పాఠ్యం);
    లేకపోతే ఒకవేళ (new_బహుళకం == 'b')
	సమితి_fmode(O_ద్విమాన_2);
#అయితే_ముగింపు
    wn = enc_గమ్యం_utf16((char_u *)name, ఖాళీ);
    wm = enc_గమ్యం_utf16((char_u *)బహుళకం, ఖాళీ);
    ఒకవేళ (wn != ఖాళీ && wm != ఖాళీ)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#ఒకవేళ defined(DEBUG) && _MSC_VER >= 1400
    ఒకవేళ (old_బహుళకం_2 != 0)
	సమితి_fmode(old_బహుళకం_2);
#అయితే_ముగింపు
    తిరిగి f;
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
    స్థిర శూన్య
copy_substream(HANDLE sh, శూన్య *context, WCHAR *గమ్యం, WCHAR *substream, దీర్ఘ len)
{
    HANDLE  h_గమ్యం;
    WCHAR   *గమ్యం_name;

    గమ్యం_name = alloc((wcslen(గమ్యం) + wcslen(substream) + 1) * పరిమాణం(WCHAR));
    ఒకవేళ (గమ్యం_name == ఖాళీ)
	తిరిగి;

    wcscpy(గమ్యం_name, గమ్యం);
    wcscat(గమ్యం_name, substream);

    h_గమ్యం = CreateFileW(గమ్యం_name, GENERIC_WRITE, 0, ఖాళీ, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_సాధారణ, ఖాళీ);
    ఒకవేళ (h_గమ్యం != INVALID_HANDLE_విలువ)
    {
	దీర్ఘ	పూర్తి;
	DWORD	todo;
	DWORD	readcnt, written;
	అక్షరం	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	కొరకు (పూర్తి = 0; పూర్తి < len; పూర్తి += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - పూర్తి) > పరిమాణం(buf) ? పరిమాణం(buf)
						       : (size_t)(len - పూర్తి));
	    ఒకవేళ (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(h_గమ్యం, buf, todo, &written, ఖాళీ)
		    || written != todo)
		ఆపు;
	}
	CloseHandle(h_గమ్యం);
    }

    విడుదల(గమ్యం_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    స్థిర శూన్య
copy_infostreams(char_u *from, char_u *గమ్యం)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_ప్రవాహం_ID	sid;
    పూర్ణాంకం			headersize;
    WCHAR		streamname[MAX_మార్గం_2];
    DWORD		readcount;
    శూన్య		*context = ఖాళీ;
    DWORD		lo, hi;
    పూర్ణాంకం			len;

    // Convert the file names to wide characters.
    fromw = enc_గమ్యం_utf16(from, ఖాళీ);
    tow = enc_గమ్యం_utf16(గమ్యం, ఖాళీ);
    ఒకవేళ (fromw != ఖాళీ && tow != ఖాళీ)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, ఖాళీ,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, ఖాళీ);
	ఒకవేళ (sh != INVALID_HANDLE_విలువ)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    కొరకు (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		Zero_జ్ఞాపకం(&sid, పరిమాణం(WIN32_ప్రవాహం_ID));
		headersize = (పూర్ణాంకం)((అక్షరం *)&sid.c_ప్రవాహం_Name - (అక్షరం *)&sid.dw_ప్రవాహం_Id);
		ఒకవేళ (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    ఆపు;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		ఒకవేళ (sid.dw_ప్రవాహం_Name_కొలత > 0)
		{
		    // Read the stream name.
		    ఒకవేళ (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dw_ప్రవాహం_Name_కొలత,
					  &readcount, FALSE, FALSE, &context))
			ఆపు;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / పరిమాణం(WCHAR);
		    streamname[len] = 0;
		    ఒకవేళ (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (దీర్ఘ)sid.కొలత_2.u.దిగువ_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(శూన్య)BackupSeek(sh, sid.కొలత_2.u.దిగువ_Part, sid.కొలత_2.u.ఎగువ_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (శూన్య)BackupRead(sh, ఖాళీ, 0, &readcount, TRUE, FALSE, &context);

	    CloseHandle(sh);
	}
    }
    vim_free(fromw);
    vim_free(tow);
}

/*
 * ntdll.dll definitions
 */
#నిర్వచించు FileEaInformation   7
#నిర్వచితం_కాకపోతే స్థితి_SUCCESS
# define స్థితి_SUCCESS	    ((NTSTATUS) 0x00000000L)
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం FILE_నిండిన_EA_INFORMATION {
    ULONG  తదుపరి_నమోదు_స్థానభ్రంశం;
    UCHAR  Flags;
    UCHAR  Ea_Name_నిడివి;
    USHORT Ea_విలువ_నిడివి;
    CHAR   EaName[1];
} FILE_నిండిన_EA_INFORMATION_2, *PFILE_నిండిన_EA_INFORMATION;

రకనిర్వచనం నిర్మాణం _FILE_EA_INFORMATION_ {
    ULONG Ea_కొలత;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#నిర్వచితం_కాకపోతే PROTO
రకనిర్వచనం NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_ముసుగు DesiredAccess,
	POBJECT_లక్షణాలు Object_లక్షణాలు,
	PIO_స్థితి_ఖండం Io_స్థితి_ఖండం,
	ULONG ShareAccess,
	ULONG OpenOptions);
రకనిర్వచనం NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
రకనిర్వచనం NTSTATUS (NTAPI *Pfn_Nt_సమితి_Ea_File)(
	HANDLE		    FileHandle,
	PIO_స్థితి_ఖండం    Io_స్థితి_ఖండం,
	PVOID		    బఫరు_2,
	ULONG		    నిడివి_2);
రకనిర్వచనం NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_స్థితి_ఖండం Io_స్థితి_ఖండం,
	PVOID బఫరు_2,
	ULONG నిడివి_2,
	BOOLEAN Return_Single_నమోదు,
	PVOID EaList,
	ULONG Ea_List_నిడివి,
	PULONG Ea_సూచీ,
	BOOLEAN Restart_పరిశీలన);
రకనిర్వచనం NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_స్థితి_ఖండం	Io_స్థితి_ఖండం,
	PVOID			FileInformation,
	ULONG			నిడివి_2,
	FILE_INFORMATION_CLASS FileInformationClass);
రకనిర్వచనం VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR ఆధారం_String);

PfnNtOpenFile pNtOpenFile = ఖాళీ;
PfnNtClose pNtClose = ఖాళీ;
Pfn_Nt_సమితి_Ea_File p_Nt_సమితి_Ea_File = ఖాళీ;
PfnNtQueryEaFile pNtQueryEaFile = ఖాళీ;
PfnNtQueryInformationFile pNtQueryInformationFile = ఖాళీ;
PfnRtlInitUnicodeString pRtlInitUnicodeString = ఖాళీ;
#అయితే_ముగింపు

/*
 * Load ntdll.dll functions.
 */
    స్థిర BOOL
ఎక్కించు_ntdll(శూన్య)
{
    స్థిర పూర్ణాంకం	loaded = -1;

    ఒకవేళ (loaded != -1)
	తిరిగి (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    ఒకవేళ (hNtdll != ఖాళీ)
    {
	pNtOpenFile = (PfnNtOpenFile) Get_Proc_చిరునామా(hNtdll, "NtOpenFile");
	pNtClose = (PfnNtClose) Get_Proc_చిరునామా(hNtdll, "NtClose");
	p_Nt_సమితి_Ea_File = (Pfn_Nt_సమితి_Ea_File)
	    Get_Proc_చిరునామా(hNtdll, "NtSetEaFile");
	pNtQueryEaFile = (PfnNtQueryEaFile)
	    Get_Proc_చిరునామా(hNtdll, "NtQueryEaFile");
	pNtQueryInformationFile = (PfnNtQueryInformationFile)
	    Get_Proc_చిరునామా(hNtdll, "NtQueryInformationFile");
	pRtlInitUnicodeString = (PfnRtlInitUnicodeString)
	    Get_Proc_చిరునామా(hNtdll, "RtlInitUnicodeString");
    }
    ఒకవేళ (pNtOpenFile == ఖాళీ
	    || pNtClose == ఖాళీ
	    || p_Nt_సమితి_Ea_File == ఖాళీ
	    || pNtQueryEaFile == ఖాళీ
	    || pNtQueryInformationFile == ఖాళీ
	    || pRtlInitUnicodeString == ఖాళీ)
	loaded = FALSE;
    లేకపోతే
	loaded = TRUE;
    తిరిగి (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    స్థిర శూన్య
copy_extattr(char_u *from, char_u *గమ్యం)
{
    char_u		    *fromf = ఖాళీ;
    char_u		    *tof = ఖాళీ;
    WCHAR		    *fromw = ఖాళీ;
    WCHAR		    *tow = ఖాళీ;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_లక్షణాలు	    oa;
    IO_స్థితి_ఖండం	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    శూన్య		    *ea = ఖాళీ;

    ఒకవేళ (!ఎక్కించు_ntdll())
	తిరిగి;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(గమ్యం) + 5);
    ఒకవేళ (fromf == ఖాళీ || tof == ఖాళీ)
	వెళ్ళు theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), గమ్యం);

    // Convert the names to wide characters.
    fromw = enc_గమ్యం_utf16(fromf, ఖాళీ);
    tow = enc_గమ్యం_utf16(tof, ఖాళీ);
    ఒకవేళ (fromw == ఖాళీ || tow == ఖాళీ)
	వెళ్ళు theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    Initialize_Object_లక్షణాలు(&oa, &u, 0, ఖాళీ, ఖాళీ);
    ఒకవేళ (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != స్థితి_SUCCESS)
	వెళ్ళు theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, పరిమాణం(eainfo),
	    FileEaInformation);
    ఒకవేళ (eainfo.Ea_కొలత != 0)
    {
	ea = alloc(eainfo.Ea_కొలత);
	ఒకవేళ (ea != ఖాళీ)
	{
	    ఒకవేళ (pNtQueryEaFile(h, &iosb, ea, eainfo.Ea_కొలత, FALSE,
			ఖాళీ, 0, ఖాళీ, TRUE) != స్థితి_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    ఒకవేళ (ea != ఖాళీ)
    {
	pRtlInitUnicodeString(&u, tow);
	Initialize_Object_లక్షణాలు(&oa, &u, 0, ఖాళీ, ఖాళీ);
	ఒకవేళ (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != స్థితి_SUCCESS)
	    వెళ్ళు theend;

	p_Nt_సమితి_Ea_File(h, &iosb, ea, eainfo.Ea_కొలత);
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
    పూర్ణాంకం
mch_copy_file_attribute(char_u *from, char_u *గమ్యం)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, గమ్యం);
    copy_extattr(from, గమ్యం);
    తిరిగి 0;
}


/*
 * The command line arguments in UTF-16
 */
స్థిర పూర్ణాంకం	n_వాదనలు_W = 0;
స్థిర LPWSTR	*ArglistW = ఖాళీ;
స్థిర పూర్ణాంకం	global_argc = 0;
స్థిర అక్షరం	**global_argv;

స్థిర పూర్ణాంకం	వాడిన_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
స్థిర పూర్ణాంకం	*వాడిన_file_indexes = ఖాళీ; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
స్థిర పూర్ణాంకం	వాడిన_file_లెక్క = 0;	// nr of entries in used_file_indexes
స్థిర పూర్ణాంకం	వాడిన_file_literal = FALSE;  // take file names literally
స్థిర పూర్ణాంకం	వాడిన_file_నిండిన_మార్గం = FALSE;  // file name was full path
#నిర్వచితమైతే FEAT_DIFF
స్థిర పూర్ణాంకం	వాడిన_file_diff_బహుళకం = FALSE;  // file name was with diff mode
#అయితే_ముగింపు
స్థిర పూర్ణాంకం	వాడిన_alist_లెక్క = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    పూర్ణాంకం
get_cmd_వాదనలు_W(అక్షరం ***argvp)
{
    అక్షరం	**argv = ఖాళీ;
    పూర్ణాంకం		argc = 0;
    పూర్ణాంకం		i;

    free_cmd_వాదనలు_W();
    ArglistW = Command_పంక్తి_గమ్యం_Argv_W(Get_Command_పంక్తి_W(), &n_వాదనలు_W);
    ఒకవేళ (ArglistW != ఖాళీ)
    {
	argv = alloc((n_వాదనలు_W + 1) * పరిమాణం(అక్షరం *));
	ఒకవేళ (argv != ఖాళీ)
	{
	    argc = n_వాదనలు_W;
	    argv[argc] = ఖాళీ;
	    కొరకు (i = 0; i < argc; ++i)
	    {
		పూర్ణాంకం	len;

		// Convert each Unicode argument to UTF-8.
		Wide_Char_గమ్యం_Multi_Byte_alloc(CP_UTF8, 0,
				ArglistW[i], (పూర్ణాంకం)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		ఒకవేళ (argv[i] == ఖాళీ)
		{
		    // Out of memory, clear everything.
		    అయితే (i > 0)
			విడుదల(argv[--i]);
		    విడుదల(argv);
		    argv = ఖాళీ;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    ఒకవేళ (argc > 0)
    {
	ఒకవేళ (వాడిన_file_indexes != ఖాళీ)
	    విడుదల(వాడిన_file_indexes);
	వాడిన_file_indexes = alloc(argc * పరిమాణం(పూర్ణాంకం));
    }

    ఒకవేళ (argvp != ఖాళీ)
	*argvp = argv;
    తిరిగి argc;
}

    శూన్య
free_cmd_వాదనలు_W(శూన్య)
{
    ఒకవేళ (ArglistW == ఖాళీ)
	తిరిగి;

    GlobalFree(ArglistW);
    ArglistW = ఖాళీ;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    శూన్య
వాడిన_file_arg(అక్షరం *name, పూర్ణాంకం literal, పూర్ణాంకం నిండిన_మార్గం, పూర్ణాంకం diff_బహుళకం)
{
    పూర్ణాంకం		i;

    ఒకవేళ (వాడిన_file_indexes == ఖాళీ)
	తిరిగి;
    కొరకు (i = వాడిన_file_argc + 1; i < global_argc; ++i)
	ఒకవేళ (STRCMP(global_argv[i], name) == 0)
	{
	    వాడిన_file_argc = i;
	    వాడిన_file_indexes[వాడిన_file_లెక్క++] = i;
	    ఆపు;
	}
    వాడిన_file_literal = literal;
    వాడిన_file_నిండిన_మార్గం = నిండిన_మార్గం;
#నిర్వచితమైతే FEAT_DIFF
    వాడిన_file_diff_బహుళకం = diff_బహుళకం;
#అయితే_ముగింపు
}

/*
 * Remember the length of the argument list as it was.  If it changes then we
 * leave it alone when 'encoding' is set.
 */
    శూన్య
సమితి_alist_లెక్క(శూన్య)
{
    వాడిన_alist_లెక్క = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    శూన్య
fix_arg_enc(శూన్య)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		idx;
    char_u	*str;
    పూర్ణాంకం		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    ఒకవేళ (global_argc != n_వాదనలు_W
	    || ArglistW == ఖాళీ
	    || వాడిన_file_indexes == ఖాళీ
	    || వాడిన_file_లెక్క == 0
	    || వాడిన_alist_లెక్క != GARGCOUNT)
	తిరిగి;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(పూర్ణాంకం, GARGCOUNT);
    ఒకవేళ (fnum_list == ఖాళీ)
	తిరిగి;		// out of memory
    కొరకు (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    ఒకవేళ (ga_grow(&global_alist.al_ga, వాడిన_file_లెక్క) == FAIL)
	తిరిగి;		// out of memory

    కొరకు (i = 0; i < వాడిన_file_లెక్క; ++i)
    {
	idx = వాడిన_file_indexes[i];
	str = utf16_గమ్యం_enc(ArglistW[idx], ఖాళీ);
	ఒకవేళ (str != ఖాళీ)
	{
	    పూర్ణాంకం literal = వాడిన_file_literal;

#నిర్వచితమైతే FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    ఒకవేళ (వాడిన_file_diff_బహుళకం && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *తోక;
		string_T    ret;

		తోక = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), తోక, STRLEN(తోక), TRUE, &ret);
		ఒకవేళ (ret.string != ఖాళీ)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#అయితే_ముగింపు
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    ఒకవేళ (వాడిన_file_literal)
		buf_సమితి_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    ఒకవేళ (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		ఒకవేళ (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_కూడు(&global_alist, str, literal ? 2 : 0);
	}
    }

    ఒకవేళ (!వాడిన_file_literal)
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
	alist_expand(fnum_list, వాడిన_alist_లెక్క);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    ఒకవేళ (curwin->w_arg_idx == 0 && curbuf->b_fname == ఖాళీ)
    {
	do_cmdline_cmd((char_u *)":rewind");
	ఒకవేళ (GARGCOUNT == 1 && వాడిన_file_నిండిన_మార్గం
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    చివరి_chdir_reason = "drop";
    }

    సమితి_alist_లెక్క();
}

    పూర్ణాంకం
mch_setenv(అక్షరం *var, అక్షరం *విలువ, పూర్ణాంకం x వాడని)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(విలువ) + 2);
    ఒకవేళ (envbuf == ఖాళీ)
	తిరిగి -1;

    sprintf((అక్షరం *)envbuf, "%s=%s", var, విలువ);

    p = enc_గమ్యం_utf16(envbuf, ఖాళీ);

    vim_free(envbuf);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి -1;
    _wputenv(p);
#నిర్వచితమైతే libintl_wputenv
    libintl_wputenv(p);
#అయితే_ముగింపు
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    తిరిగి 0;
}

/*
 * Support for 256 colors and 24-bit colors was added in Windows 10
 * version 1703 (Creators update).
 */
#నిర్వచించు VTP_మొదటి_SUPPORT_BUILD	    MAKE_VER(10, 0, 15063)

/*
 * Support for pseudo-console (ConPTY) was added in windows 10
 * version 1809 (October 2018 update).
 */
#నిర్వచించు CONPTY_మొదటి_SUPPORT_BUILD  MAKE_VER(10, 0, 17763)

/*
 * ConPTY differences between versions, need different logic.
 * version 1903 (May 2019 update).
 */
#నిర్వచించు CONPTY_1903_BUILD	    MAKE_VER(10, 0, 18362)

/*
 * version 1909 (November 2019 update).
 */
#నిర్వచించు CONPTY_1909_BUILD	    MAKE_VER(10, 0, 18363)

/*
 * Stay ahead of the next update, and when it's done, fix this.
 * version ? (2020 update, temporarily use the build number of insider preview)
 */
#నిర్వచించు CONPTY_తదుపరి_UPDATE_BUILD    MAKE_VER(10, 0, 19587)

/*
 * Confirm until this version.  Also the logic changes.
 * insider preview.
 */
#నిర్వచించు CONPTY_INSIDER_BUILD	    MAKE_VER(10, 0, 18995)

/*
 * Make conpty default on Windows 11
 */
#నిర్వచించు CONPTY_STABLE_BUILD	    MAKE_VER(10, 0, 22000)
// Notes:
// Win 10 22H2 Final is build 19045, its conpty is widely used.
// Strangely, 19045 is newer but is a lower build number than the 2020 insider
// preview which had a build 19587.  And, not sure how stable that was?
// Win Server 2022 (May 10, 2022) is build 20348, its conpty is widely used.
// Win 11 starts from build 22000, even though the major version says 10!

    స్థిర శూన్య
vtp_flag_init(శూన్య)
{
#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   బహుళకం;
    HANDLE  బహిర్గతం;

# ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use)
# endif
    {
	బహిర్గతం = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_సంస్కరణ >= VTP_మొదటి_SUPPORT_BUILD) ? 1 : 0;
	Get_Console_బహుళకం(బహిర్గతం, &బహుళకం);
	బహుళకం |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	ఒకవేళ (సమితి_Console_బహుళకం(బహిర్గతం, బహుళకం) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_బఫరు = win_సంస్కరణ >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#అయితే_ముగింపు

    ఒకవేళ (win_సంస్కరణ >= CONPTY_మొదటి_SUPPORT_BUILD)
	conpty_working = 1;
    ఒకవేళ (win_సంస్కరణ >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    ఒకవేళ (win_సంస్కరణ <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    ఒకవేళ (win_సంస్కరణ <= CONPTY_1909_BUILD)
	conpty_type = 2;
    ఒకవేళ (win_సంస్కరణ <= CONPTY_1903_BUILD)
	conpty_type = 2;
    ఒకవేళ (win_సంస్కరణ < CONPTY_మొదటి_SUPPORT_BUILD)
	conpty_type = 1;

    ఒకవేళ (win_సంస్కరణ >= CONPTY_తదుపరి_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    స్థిర శూన్య
vtp_init(శూన్య)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_బఫరు_INFOEX csbi;
    csbi.cb_కొలత = పరిమాణం(csbi);
    Get_Console_Screen_బఫరు_Info_Ex(g_h_Con_బహిర్గతం, &csbi);
    save_console_bg_rgb = (guicolor_T)csbi.Color_పట్టిక[g_color_సూచీ_bg];
    save_console_fg_rgb = (guicolor_T)csbi.Color_పట్టిక[g_color_సూచీ_fg];
    store_console_bg_rgb = save_console_bg_rgb;
    store_console_fg_rgb = save_console_fg_rgb;

    COLORREF bg;
    bg = (COLORREF)csbi.Color_పట్టిక[g_color_సూచీ_bg];
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);
    default_console_color_bg = bg;

    COLORREF fg;
    fg = (COLORREF)csbi.Color_పట్టిక[g_color_సూచీ_fg];
    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    default_console_color_fg = fg;
# endif
    సమితి_console_color_rgb();
}

    స్థిర శూన్య
vtp_exit(శూన్య)
{
    restore_console_color_rgb();
}

    పూర్ణాంకం
vtp_printf(
    అక్షరం *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   ఫలితం;
    పూర్ణాంకం	    len;

    ఒకవేళ (silent_బహుళకం)
	తిరిగి 0;

    va_start(list, format);
    len = vim_vsnprintf((అక్షరం *)buf, 100, (అక్షరం *)format, list);
    va_end(list);
    WriteConsoleA(g_h_Con_బహిర్గతం, buf, (DWORD)len, &ఫలితం, ఖాళీ);
    తిరిగి (పూర్ణాంకం)ఫలితం;
}

    స్థిర శూన్య
vtp_sgr_bulk(
    పూర్ణాంకం arg)
{
    పూర్ణాంకం వాదనలు[1];

    వాదనలు[0] = arg;
    vtp_sgr_bulks(1, వాదనలు);
}

# define FAST256(x) \
    ఒకవేళ ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    స్థితి x: \
	FAST256(newargs[x - 1]);

    స్థిర శూన్య
vtp_sgr_bulks(
    పూర్ణాంకం argc,
    పూర్ణాంకం *వాదనలు)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    పూర్ణాంకం	    లోపల, బహిర్గతం;
    పూర్ణాంకం	    newargs[16];
    స్థిర పూర్ణాంకం sgrfgr = -1, sgrfgg, sgrfgb;
    స్థిర పూర్ణాంకం sgrbgr = -1, sgrbgg, sgrbgb;

    ఒకవేళ (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	తిరిగి;
    }

    లోపల = బహిర్గతం = 0;
    అయితే (లోపల < argc)
    {
	పూర్ణాంకం s = వాదనలు[లోపల];
	పూర్ణాంకం copylen = 1;

	ఒకవేళ (s == 38)
	{
	    ఒకవేళ (argc - లోపల >= 5 && వాదనలు[లోపల + 1] == 2)
	    {
		ఒకవేళ (sgrfgr == వాదనలు[లోపల + 2] && sgrfgg == వాదనలు[లోపల + 3]
						     && sgrfgb == వాదనలు[లోపల + 4])
		{
		    లోపల += 5;
		    copylen = 0;
		}
		లేకపోతే
		{
		    sgrfgr = వాదనలు[లోపల + 2];
		    sgrfgg = వాదనలు[లోపల + 3];
		    sgrfgb = వాదనలు[లోపల + 4];
		    copylen = 5;
		}
	    }
	    లేకపోతే ఒకవేళ (argc - లోపల >= 3 && వాదనలు[లోపల + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	లేకపోతే ఒకవేళ (s == 48)
	{
	    ఒకవేళ (argc - లోపల >= 5 && వాదనలు[లోపల + 1] == 2)
	    {
		ఒకవేళ (sgrbgr == వాదనలు[లోపల + 2] && sgrbgg == వాదనలు[లోపల + 3]
						     && sgrbgb == వాదనలు[లోపల + 4])
		{
		    లోపల += 5;
		    copylen = 0;
		}
		లేకపోతే
		{
		    sgrbgr = వాదనలు[లోపల + 2];
		    sgrbgg = వాదనలు[లోపల + 3];
		    sgrbgb = వాదనలు[లోపల + 4];
		    copylen = 5;
		}
	    }
	    లేకపోతే ఒకవేళ (argc - లోపల >= 3 && వాదనలు[లోపల + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	లేకపోతే ఒకవేళ (30 <= s && s <= 39)
	    sgrfgr = -1;
	లేకపోతే ఒకవేళ (90 <= s && s <= 97)
	    sgrfgr = -1;
	లేకపోతే ఒకవేళ (40 <= s && s <= 49)
	    sgrbgr = -1;
	లేకపోతే ఒకవేళ (100 <= s && s <= 107)
	    sgrbgr = -1;
	లేకపోతే ఒకవేళ (s == 0)
	    sgrfgr = sgrbgr = -1;

	అయితే (copylen--)
	    newargs[బహిర్గతం++] = వాదనలు[లోపల++];
    }

    p = &buf[పరిమాణం(buf) - 1];
    *p-- = 'm';

    ఎంపిక (బహిర్గతం)
    {
	పూర్ణాంకం	n, m;
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
	WriteConsoleA(g_h_Con_బహిర్గతం, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, ఖాళీ);
    అప్రమేయ:
	ఆపు;
    }
}

    స్థిర శూన్య
wt_init(శూన్య)
{
    wt_working = mch_getenv("WT_SESSION") != ఖాళీ;
}

# ifdef FEAT_TERMGUICOLORS
    స్థిర పూర్ణాంకం
ctermtoxterm(
    పూర్ణాంకం cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    తిరిగి (((పూర్ణాంకం)r << 16) | ((పూర్ణాంకం)g << 8) | (పూర్ణాంకం)b);
}
# endif

    స్థిర శూన్య
సమితి_console_color_rgb(శూన్య)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_బఫరు_INFOEX csbi;
    guicolor_T	fg, bg;
    పూర్ణాంకం		ctermfg, ctermbg;

    ఒకవేళ (!vtp_working)
	తిరిగి;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    ఒకవేళ (p_tgc || t_colors >= 256)
    {
	పదం_fg_rgb_color(fg);
	పదం_bg_rgb_color(bg);
	తిరిగి;
    }

    ఒకవేళ (use_alternate_screen_బఫరు)
	తిరిగి;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cb_కొలత = పరిమాణం(csbi);
    Get_Console_Screen_బఫరు_Info_Ex(g_h_Con_బహిర్గతం, &csbi);

    csbi.cb_కొలత = పరిమాణం(csbi);
    csbi.sr_కిటికీ.కుడి_4 += 1;
    csbi.sr_కిటికీ.Bottom += 1;
    store_console_bg_rgb = csbi.Color_పట్టిక[g_color_సూచీ_bg];
    store_console_fg_rgb = csbi.Color_పట్టిక[g_color_సూచీ_fg];
    csbi.Color_పట్టిక[g_color_సూచీ_bg] = (COLORREF)bg;
    csbi.Color_పట్టిక[g_color_సూచీ_fg] = (COLORREF)fg;
    సమితి_Console_Screen_బఫరు_Info_Ex(g_h_Con_బహిర్గతం, &csbi);
# endif
}

# ఒకవేళ defined(FEAT_TERMGUICOLORS)
    శూన్య
get_default_console_color(
    పూర్ణాంకం *cterm_fg,
    పూర్ణాంకం *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    పూర్ణాంకం id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    పూర్ణాంకం ctermfg = 0;
    పూర్ణాంకం ctermbg = 0;
    పూర్ణాంకం dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    ఒకవేళ (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    ఒకవేళ (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	ఒకవేళ (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	ఒకవేళ (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	లేకపోతే
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_సాధారణ_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    ఒకవేళ (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	ఒకవేళ (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	ఒకవేళ (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	లేకపోతే
	    guibg = USE_WT ? INVALCOLOR : default_console_color_bg;
	cterm_సాధారణ_bg_gui_color = guibg;
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
    స్థిర శూన్య
reset_console_color_rgb(శూన్య)
{
# ifdef FEAT_TERMGUICOLORS
    ఒకవేళ (use_alternate_screen_బఫరు)
	తిరిగి;

    CONSOLE_SCREEN_బఫరు_INFOEX csbi;

    csbi.cb_కొలత = పరిమాణం(csbi);
    Get_Console_Screen_బఫరు_Info_Ex(g_h_Con_బహిర్గతం, &csbi);

    csbi.cb_కొలత = పరిమాణం(csbi);
    csbi.sr_కిటికీ.కుడి_4 += 1;
    csbi.sr_కిటికీ.Bottom += 1;
    csbi.Color_పట్టిక[g_color_సూచీ_bg] = (COLORREF)store_console_bg_rgb;
    csbi.Color_పట్టిక[g_color_సూచీ_fg] = (COLORREF)store_console_fg_rgb;
    సమితి_Console_Screen_బఫరు_Info_Ex(g_h_Con_బహిర్గతం, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    స్థిర శూన్య
restore_console_color_rgb(శూన్య)
{
# ifdef FEAT_TERMGUICOLORS
    ఒకవేళ (use_alternate_screen_బఫరు)
	తిరిగి;

    CONSOLE_SCREEN_బఫరు_INFOEX csbi;

    csbi.cb_కొలత = పరిమాణం(csbi);
    Get_Console_Screen_బఫరు_Info_Ex(g_h_Con_బహిర్గతం, &csbi);

    csbi.cb_కొలత = పరిమాణం(csbi);
    csbi.sr_కిటికీ.కుడి_4 += 1;
    csbi.sr_కిటికీ.Bottom += 1;
    csbi.Color_పట్టిక[g_color_సూచీ_bg] = (COLORREF)save_console_bg_rgb;
    csbi.Color_పట్టిక[g_color_సూచీ_fg] = (COLORREF)save_console_fg_rgb;
    సమితి_Console_Screen_బఫరు_Info_Ex(g_h_Con_బహిర్గతం, &csbi);
# endif
}

    శూన్య
control_console_color_rgb(శూన్య)
{
    ఒకవేళ (vtp_working)
	సమితి_console_color_rgb();
    లేకపోతే
	reset_console_color_rgb();
}

    పూర్ణాంకం
use_vtp(శూన్య)
{
    తిరిగి USE_VTP;
}

    పూర్ణాంకం
is_పదం_win32(శూన్య)
{
    తిరిగి T_NAME != ఖాళీ && STRCMP(T_NAME, "win32") == 0;
}

    పూర్ణాంకం
has_vtp_working(శూన్య)
{
    తిరిగి vtp_working;
}

#అయితే_ముగింపు

    పూర్ణాంకం
has_conpty_working(శూన్య)
{
    తిరిగి conpty_working;
}

    పూర్ణాంకం
get_conpty_type(శూన్య)
{
    తిరిగి conpty_type;
}

    పూర్ణాంకం
is_conpty_stable(శూన్య)
{
    తిరిగి conpty_stable;
}

    పూర్ణాంకం
get_conpty_fix_type(శూన్య)
{
    తిరిగి conpty_fix_type;
}

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    శూన్య
resize_console_buf(శూన్య)
{
    ఒకవేళ (use_alternate_screen_బఫరు)
	తిరిగి;

    CONSOLE_SCREEN_బఫరు_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    ఒకవేళ (!Get_Console_Screen_బఫరు_Info(g_h_Con_బహిర్గతం, &csbi))
	తిరిగి;

    coord.X = SRWIDTH(csbi.sr_కిటికీ);
    coord.Y = SRHEIGHT(csbi.sr_కిటికీ);
    సమితి_Console_Screen_బఫరు_కొలత(g_h_Con_బహిర్గతం, coord);

    newsize.ఎడమ_4 = 0;
    newsize.శిఖరం_2 = 0;
    newsize.కుడి_4 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    సమితి_Console_కిటికీ_Info(g_h_Con_బహిర్గతం, TRUE, &newsize);

    సమితి_Console_Screen_బఫరు_కొలత(g_h_Con_బహిర్గతం, coord);
}
#అయితే_ముగింపు

    అక్షరం *
GetWin32Error(శూన్య)
{
    స్థిర అక్షరం	*oldmsg = ఖాళీ;
    అక్షరం	*acp_సందేశం = ఖాళీ;
    DWORD	acp_len;
    అక్షరం	*enc_సందేశం = ఖాళీ;
    పూర్ణాంకం		enc_len = 0;

    // get formatted message from OS
    acp_len = Format_సందేశం(
	    FORMAT_సందేశం_ALLOCATE_బఫరు|FORMAT_సందేశం_FROM_SYSTEM,
	    ఖాళీ, Get_చివరి_Error(), 0, (LPSTR)&acp_సందేశం, 0, ఖాళీ);
    ఒకవేళ (acp_len == 0 || acp_సందేశం == ఖాళీ)
	తిరిగి ఖాళీ;

    // clean oldmsg if remained.
    ఒకవేళ (oldmsg != ఖాళీ)
    {
	vim_free(oldmsg);
	oldmsg = ఖాళీ;
    }

    acp_గమ్యం_enc((char_u *)acp_సందేశం, (పూర్ణాంకం)acp_len, (char_u **)&enc_సందేశం, &enc_len);
    LocalFree(acp_సందేశం);
    ఒకవేళ (enc_సందేశం == ఖాళీ)
	తిరిగి ఖాళీ;

    // remove trailing \r\n
    అక్షరం *pcrlf = అక్షరశ్రేణి_శోధన(enc_సందేశం, "\r\n");
    ఒకవేళ (pcrlf != ఖాళీ)
	*pcrlf = NUL;

    oldmsg = enc_సందేశం;
    తిరిగి enc_సందేశం;
}

#ఒకవేళ defined(FEAT_RELTIME)
స్థిర HANDLE   timer_handle;
స్థిర పూర్ణాంకం      timer_చురుకైన = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
స్థిర sig_atomic_t timeout_flags[2];
స్థిర పూర్ణాంకం	    timeout_flag_idx = 0;
స్థిర sig_atomic_t *timeout_flag = &timeout_flags[0];


    స్థిర శూన్య CALLBACK
సమితి_flag(శూన్య *param, BOOLEAN unused2 వాడని)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    శూన్య
stop_timeout(శూన్య)
{
    ఒకవేళ (timer_చురుకైన)
    {
	BOOL ret = Delete_Timer_వరుస_Timer(ఖాళీ, timer_handle, ఖాళీ);
	timer_చురుకైన = FALSE;
	ఒకవేళ (!ret && Get_చివరి_Error() != ERROR_IO_PENDING)
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
    అస్థిర sig_atomic_t *
ప్రారంభం_timeout(దీర్ఘ msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = Create_Timer_వరుస_Timer(
	    &timer_handle, ఖాళీ, సమితి_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    ఒకవేళ (!ret)
    {
	semsg(_(e_could_not_సమితి_timeout_str), GetWin32Error());
    }
    లేకపోతే
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_చురుకైన = TRUE;
	*timeout_flag = FALSE;
    }
    తిరిగి timeout_flag;
}
#అయితే_ముగింపు
