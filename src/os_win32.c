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

#شامل "vim.h"

#اگر_تعریف_شدہ FEAT_MZSCHEME
# include "if_mzsch.h"
#اگر_ختم

#شامل <sys/types.h>
#شامل <signal.h>
#شامل <limits.h>

#شامل <process.h>
#شامل <winternl.h>
#شامل <direct.h>

#اگر !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#اگر_ختم

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
# include <tlhelp32.h>
#اگر_ختم

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#اگر_تعریف_شدہ MCH_WRITE_DUMP
FILE* fdDump = تہی;
#اگر_ختم

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
ساکن HANDLE g_h_Con_اندر  = INVALID_HANDLE_قدر;
ساکن HANDLE g_h_Con_برآمد = INVALID_HANDLE_قدر;

// Win32 Screen buffer,coordinate,console I/O information
ساکن SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
ساکن COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
ساکن لفظ_2  g_attrDefault = 7;  // lightgray text on black background
ساکن لفظ_2  g_attrCurrent;

ساکن صحیح g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
ساکن صحیح g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
ساکن صحیح g_fForceExit = FALSE;    // set when forcefully exiting

ساکن خالی scroll(غیرعلامتی c_سطریں);
ساکن خالی سیٹ_scroll_region(غیرعلامتی بایاں, غیرعلامتی چوٹی,
			      غیرعلامتی دایاں, غیرعلامتی bottom);
ساکن خالی سیٹ_scroll_region_tb(غیرعلامتی چوٹی, غیرعلامتی bottom);
ساکن خالی سیٹ_scroll_region_lr(غیرعلامتی بایاں, غیرعلامتی دایاں);
ساکن خالی داخل_کرو_سطریں(غیرعلامتی c_سطریں);
ساکن خالی delete_سطریں(غیرعلامتی c_سطریں);
ساکن خالی gotoxy(غیرعلامتی x, غیرعلامتی y);
ساکن خالی standout(خالی);
ساکن صحیح s_cursor_visible = TRUE;
ساکن صحیح did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
قسمتعریف ساخت داخلہ_ریکارڈ_بفر_گرہ_S
{
    داخلہ_ریکارڈ ir;
    ساخت داخلہ_ریکارڈ_بفر_گرہ_S *اگلا;
} داخلہ_ریکارڈ_بفر_گرہ_T;
قسمتعریف ساخت داخلہ_ریکارڈ_بفر_S
{
    داخلہ_ریکارڈ_بفر_گرہ_T *سر;
    داخلہ_ریکارڈ_بفر_گرہ_T *دم;
    صحیح لمبائی;
} داخلہ_ریکارڈ_بفر_T;
ساکن داخلہ_ریکارڈ_بفر_T داخلہ_ریکارڈ_بفر;
ساکن صحیح read_داخلہ_ریکارڈ_بفر(داخلہ_ریکارڈ* irEvents, صحیح n_Max_لمبائی);
ساکن صحیح write_داخلہ_ریکارڈ_بفر(داخلہ_ریکارڈ* irEvents, صحیح n_لمبائی);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
ساکن صحیح s_dont_use_vimrun = TRUE;
ساکن صحیح need_vimrun_warning = FALSE;
ساکن string_T vimrun_راستہ = {(char_u *)"vimrun ", 7};
ساکن صحیح vimrun_راستہ_allocated = FALSE;
#اگر_ختم

ساکن صحیح win32_getattrs(char_u *name);
ساکن صحیح win32_setattrs(char_u *name, صحیح attrs);
ساکن صحیح win32_سیٹ_archive(char_u *name);

ساکن صحیح conpty_working = 0;
ساکن صحیح conpty_type = 0;
ساکن صحیح conpty_stable = 0;
ساکن صحیح conpty_fix_type = 0;
ساکن خالی vtp_flag_init(خالی);

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساکن صحیح vtp_working = 0;
ساکن خالی vtp_init(خالی);
ساکن خالی vtp_exit(خالی);
ساکن خالی vtp_sgr_bulk(صحیح arg);
ساکن خالی vtp_sgr_bulks(صحیح argc, صحیح *argv);

ساکن صحیح wt_working = 0;
ساکن خالی wt_init(خالی);

ساکن صحیح g_color_اشاریہ_bg = 0;
ساکن صحیح g_color_اشاریہ_fg = 7;

# ifdef FEAT_TERMGUICOLORS
ساکن guicolor_T save_console_bg_rgb;
ساکن guicolor_T save_console_fg_rgb;
ساکن guicolor_T store_console_bg_rgb;
ساکن guicolor_T store_console_fg_rgb;
ساکن صحیح default_console_color_bg = 0x000000; // black
ساکن صحیح default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_رکن_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# ورنہ
#  define USE_VTP		0
#  define USE_WT		0
# endif

ساکن خالی سیٹ_console_color_rgb(خالی);
ساکن خالی reset_console_color_rgb(خالی);
ساکن خالی restore_console_color_rgb(خالی);
#اگر_ختم  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#اگر_غیر_تعریف_شدہ ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#اگر_ختم

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساکن صحیح suppress_winsize = 1;	// don't fiddle with console
#اگر_ختم

ساکن WCHAR *exe_pathw = تہی;

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساکن BOOL use_alternate_screen_بفر = FALSE;
#اگر_ختم

خارجی DWORD win_نسخہ; // this is in os_mswin.c

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ساکن BOOL
is_ambiwidth_event(
    داخلہ_ریکارڈ *ir)
{
    واپس ir->EventType == کلید_EVENT
		&& ir->Event.کلید_Event_2.b_کلید_نیچے_کو
		&& ir->Event.کلید_Event_2.w_Repeat_گنتی == 1
		&& ir->Event.کلید_Event_2.w_Virtual_کلید_رمز == 0x12
		&& ir->Event.کلید_Event_2.w_Virtual_جائزہ_رمز == 0x38
		&& ir->Event.کلید_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.کلید_Event_2.dw_Control_کلید_State == 2;
}

    ساکن خالی
make_ambiwidth_event(
    داخلہ_ریکارڈ *نیچے_کو,
    داخلہ_ریکارڈ *اوپر)
{
    نیچے_کو->Event.کلید_Event_2.w_Virtual_کلید_رمز = 0;
    نیچے_کو->Event.کلید_Event_2.w_Virtual_جائزہ_رمز = 0;
    نیچے_کو->Event.کلید_Event_2.uChar.UnicodeChar
				    = اوپر->Event.کلید_Event_2.uChar.UnicodeChar;
    نیچے_کو->Event.کلید_Event_2.dw_Control_کلید_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    ساکن BOOL
read_console_داخلہ(
    HANDLE	    h_داخلہ,
    داخلہ_ریکارڈ    *lp_بفر,
    صحیح		    n_لمبائی,
    LPDWORD	    lpEvents)
{
    شمار
    {
	IRSIZE = 10
    };
    ساکن داخلہ_ریکارڈ s_irCache[IRSIZE];
    ساکن DWORD s_dw_اشاریہ = 0;
    ساکن DWORD s_dwMax = 0;
    DWORD dwEvents;
    صحیح سر;
    صحیح دم;
    صحیح i;
    ساکن داخلہ_ریکارڈ s_irPseudo;

    اگر (s_dwMax == 0 && داخلہ_ریکارڈ_بفر.لمبائی > 0)
    {
	dwEvents = read_داخلہ_ریکارڈ_بفر(s_irCache, IRSIZE);
	s_dw_اشاریہ = 0;
	s_dwMax = dwEvents;
    }

    اگر (n_لمبائی == -2)
	واپس (s_dwMax > 0) ? TRUE : FALSE;

    اگر (win_نسخہ < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	اگر (n_لمبائی == -1)
	    واپس Peek_Console_داخلہ_W(h_داخلہ, lp_بفر, 1, lpEvents);
	واپس Read_Console_داخلہ_W(h_داخلہ, lp_بفر, 1, &dwEvents);
    }

    اگر (s_dwMax == 0)
    {
	اگر (!vtp_working && n_لمبائی == -1)
	    واپس Peek_Console_داخلہ_W(h_داخلہ, lp_بفر, 1, lpEvents);
	Get_نمبر_کا_Console_داخلہ_Events(h_داخلہ, &dwEvents);
	اگر (dwEvents == 0 && n_لمبائی == -1)
	    واپس Peek_Console_داخلہ_W(h_داخلہ, lp_بفر, 1, lpEvents);
	Read_Console_داخلہ_W(h_داخلہ, s_irCache, IRSIZE, &dwEvents);
	s_dw_اشاریہ = 0;
	s_dwMax = dwEvents;
	اگر (dwEvents == 0)
	{
	    *lpEvents = 0;
	    واپس TRUE;
	}

	برائے (i = s_dw_اشاریہ; i < (صحیح)s_dwMax - 1; ++i)
	    اگر (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	اگر (s_dwMax > 1)
	{
	    سر = 0;
	    دم = s_dwMax - 1;
	    جب (سر != دم)
	    {
		اگر (s_irCache[سر].EventType == کھڑکی_بفر_سائز_EVENT
			&& s_irCache[سر + 1].EventType
						  == کھڑکی_بفر_سائز_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    برائے (i = سر; i < دم; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --دم;
		    جاری;
		}
		سر++;
	    }
	    s_dwMax = دم + 1;
	}
    }

    اگر (s_irCache[s_dw_اشاریہ].EventType == کلید_EVENT)
    {
	اگر (s_irCache[s_dw_اشاریہ].Event.کلید_Event_2.w_Repeat_گنتی > 1)
	{
	    s_irPseudo = s_irCache[s_dw_اشاریہ];
	    s_irPseudo.Event.کلید_Event_2.w_Repeat_گنتی = 1;
	    s_irCache[s_dw_اشاریہ].Event.کلید_Event_2.w_Repeat_گنتی--;
	    *lp_بفر = s_irPseudo;
	    *lpEvents = 1;
	    واپس TRUE;
	}
    }

    *lp_بفر = s_irCache[s_dw_اشاریہ];
    اگر (!(n_لمبائی == -1 || n_لمبائی == -2) && ++s_dw_اشاریہ >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    واپس TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    ساکن BOOL
peek_console_داخلہ(
    HANDLE	    h_داخلہ,
    داخلہ_ریکارڈ    *lp_بفر,
    DWORD	    n_لمبائی غیر_مستعمل,
    LPDWORD	    lpEvents)
{
    واپس read_console_داخلہ(h_داخلہ, lp_بفر, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    ساکن DWORD
پیغام_wait_for_مضرب_objects(
    DWORD    n_گنتی,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dw_Wake_نقاب)
{
    اگر (read_console_داخلہ(تہی, تہی, -2, تہی))
	واپس WAIT_OBJECT_0;
    واپس پیغام_Wait_For_مضرب_Objects(n_گنتی, pHandles, fWaitAll,
				     dwMilliseconds, dw_Wake_نقاب);
}
# endif

# ifndef FEAT_CLIENTSERVER
    ساکن DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    اگر (read_console_داخلہ(تہی, تہی, -2, تہی))
	واپس WAIT_OBJECT_0;
    واپس WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#اگر_ختم   // !FEAT_GUI_MSWIN || VIMDLL

    خالی
mch_get_exe_name(خالی)
{
    صحیح		updated = FALSE;
    ساکن صحیح	enc_پچھلا = -1;
    WCHAR	*راستہ;
    size_t	pathlen;

    اگر (exe_name == تہی || exe_pathw == تہی || enc_پچھلا != enc_codepage)
    {
	WCHAR	buf[MAX_راستہ];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(تہی, buf, MAX_راستہ);
	اگر (p > buf)
	{
	    اگر (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_مقصد_enc(buf, تہی);
	    enc_پچھلا = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    کرو
	    {
		--p;
		اگر (*p == L'\\')
		{
		    *p = L'\0';
		    توڑو;
		}
	    } جب (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    اگر (exe_pathw == تہی || !updated)
	واپس;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    راستہ = _wgetenv(L"PATH");
    pathlen = (راستہ == تہی) ? 0 : wcslen(راستہ);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#تعریف_کرو EXTRA_LEN 7
#تعریف_کرو MAX_ENV_STRING_LEN 8191

    اگر (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   عارضی[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	اگر (pathlen == 0)
	    wcscpy(عارضی + templen, exe_pathw);
	ورنہ
	{
	    wcscpy(عارضی + templen, راستہ);

	    // Check if exe_path is already included in $PATH.
	    اگر (wcsstr(عارضی, exe_pathw) == تہی)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		اگر (عارضی[templen - 1] != L';')
		{
		    wcscpy(عارضی + templen, L";");
		    ++templen;
		}

		wcscpy(عارضی + templen, exe_pathw);
	    }
	}

	_wputenv(عارضی);
#اگر_تعریف_شدہ libintl_wputenv
	libintl_wputenv(عارضی);
#اگر_ختم
    }
#تعریف_منسوخ EXTRA_LEN
#تعریف_منسوخ MAX_ENV_راستہ_LEN
}

/*
 * Unescape characters in "p" that appear in "escaped".
 */
    ساکن خالی
unescape_shellxquote(char_u *p, char_u *escaped)
{
    صحیح	    l = (صحیح)STRLEN(p);
    صحیح	    n;

    جب (*p != NUL)
    {
	اگر (*p == '^' && vim_strchr(escaped, p[1]) != تہی)
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
vim_بھرو_Lib(ثابت حرف *name)
{
    HINSTANCE	dll = تہی;

    // No need to load any library when registering OLE.
    اگر (ملا_رجسٹر_arg)
	واپس تہی;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    اگر (exe_pathw == تہی)
    {
	mch_get_exe_name();
	اگر (exe_pathw == تہی)
	    واپس تہی;
    }

    WCHAR old_dirw[MAXPATHL];

    اگر (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	واپس تہی;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    سیٹ_Current_Directory_W(exe_pathw);
    dll = بھرو_Library(name);
    سیٹ_Current_Directory_W(old_dirw);
    واپس dll;
}

#اگر defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    صحیح
mch_is_gui_executable(خالی)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(تہی);
    PIMAGE_DOS_سرخی	pDOS = (PIMAGE_DOS_سرخی)pImage;
    PIMAGE_NT_سرخیاں	pPE;

    اگر (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	واپس FALSE;
    pPE = (PIMAGE_NT_سرخیاں)(pImage + pDOS->e_lfanew);
    اگر (pPE->Signature != IMAGE_NT_SIGNATURE)
	واپس FALSE;
    اگر (pPE->Optional_سرخی.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	واپس TRUE;
    واپس FALSE;
}
#اگر_ختم

#اگر defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    ساکن خالی *
get_imported_func_info(HINSTANCE hInst, ثابت حرف *funcname, صحیح info,
	ثابت خالی *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_سرخی		pDOS = (PIMAGE_DOS_سرخی)hInst;
    PIMAGE_NT_سرخیاں		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_ڈیٹا		pIAT;	    // Import Address Table
    PIMAGE_THUNK_ڈیٹا		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    اگر (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	واپس تہی;
    pPE = (PIMAGE_NT_سرخیاں)(pImage + pDOS->e_lfanew);
    اگر (pPE->Signature != IMAGE_NT_SIGNATURE)
	واپس تہی;

    ImpVA = pPE->Optional_سرخی
		.ڈیٹا_Directory[IMAGE_DIRECTORY_اندراج_IMPORT].Virtual_پتہ;
    اگر (ImpVA == 0)
	واپس تہی;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    برائے (; pImpDesc->پہلا_Thunk; ++pImpDesc)
    {
	اگر (!pImpDesc->Original_پہلا_Thunk)
	    جاری;
	pIAT = (PIMAGE_THUNK_ڈیٹا)(pImage + pImpDesc->پہلا_Thunk);
	pINT = (PIMAGE_THUNK_ڈیٹا)(pImage + pImpDesc->Original_پہلا_Thunk);
	برائے (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    اگر (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		جاری;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.پتہ_کا_ڈیٹا));
	    اگر (strcmp((حرف *)pImpName->Name, funcname) == 0)
	    {
		خالی *original;
		DWORD old, new = صفحہ_READWRITE;

		انتخاب (info)
		{
		    حالت 0:
			واپس (خالی *)pIAT->u1.Function;
		    حالت 1:
			واپس (خالی *)(pImage + pImpDesc->Name);
		    حالت 2:
			original = (خالی *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, حجم(خالی *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, حجم(خالی *),
				old, &new);
			واپس original;
		    طےشدہ:
			واپس تہی;
		}
	    }
	}
    }
    واپس تہی;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
ڈھونڈو_imported_module_by_funcname(HINSTANCE hInst, ثابت حرف *funcname)
{
    حرف    *modulename;

    modulename = (حرف *)get_imported_func_info(hInst, funcname, 1, تہی);
    اگر (modulename != تہی)
	واپس GetModuleHandleA(modulename);
    واپس تہی;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    خالی *
get_dll_import_func(HINSTANCE hInst, ثابت حرف *funcname)
{
    واپس get_imported_func_info(hInst, funcname, 0, تہی);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    خالی *
hook_dll_import_func(HINSTANCE hInst, ثابت حرف *funcname, ثابت خالی *hook)
{
    واپس get_imported_func_info(hInst, funcname, 2, hook);
}
#اگر_ختم

#اگر defined(FEAT_PYTHON3)
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
    PIMAGE_DOS_سرخی		pDOS = (PIMAGE_DOS_سرخی)hInst;
    PIMAGE_NT_سرخیاں		pPE;
    PIMAGE_EXPORT_DIRECTORY	pExpDir;
    DWORD			ExpVA;
    DWORD			Exp_سائز;
    LPDWORD			p_Function_جدول;

    اگر (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	واپس تہی;
    pPE = (PIMAGE_NT_سرخیاں)(pImage + pDOS->e_lfanew);
    اگر (pPE->Signature != IMAGE_NT_SIGNATURE)
	واپس تہی;

    ExpVA = pPE->Optional_سرخی
		.ڈیٹا_Directory[IMAGE_DIRECTORY_اندراج_EXPORT].Virtual_پتہ;
    Exp_سائز = pPE->Optional_سرخی
		.ڈیٹا_Directory[IMAGE_DIRECTORY_اندراج_EXPORT].سائز_2;
    اگر (ExpVA == 0)
	واپس hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    p_Function_جدول = (LPDWORD)(pImage + pExpDir->پتہ_کا_Functions);

    اگر (pExpDir->نمبر_کا_نام == 0)
	واپس hInst;	// No export names.

    // Check only the first entry.
    اگر ((p_Function_جدول[0] < ExpVA) || (p_Function_جدول[0] >= ExpVA + Exp_سائز))
	// The first entry is not a function forwarder.
	واپس hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    ثابت حرف *name = (ثابت حرف *)(pImage + p_Function_جدول[0]);
    ثابت حرف *p = strchr(name, '.');
    اگر (p == تہی)
	واپس hInst;

    // Extract DllName.
    حرف buf[MAX_راستہ];
    اگر (p - name + 1 > حجم(buf))
	واپس تہی;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    واپس GetModuleHandleA(buf);
}
#اگر_ختم

#اگر defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
ساکن حرف *null_libintl_gettext(ثابت حرف *);
ساکن حرف *null_libintl_ngettext(ثابت حرف *, ثابت حرف *, غیرعلامتی طویل n);
ساکن حرف *null_libintl_textdomain(ثابت حرف *);
ساکن حرف *null_libintl_bindtextdomain(ثابت حرف *, ثابت حرف *);
ساکن حرف *null_libintl_bind_textdomain_codeset(ثابت حرف *, ثابت حرف *);
ساکن صحیح null_libintl_wputenv(ثابت wchar_t *);

ساکن HINSTANCE hLibintlDLL = تہی;
حرف *(*dyn_libintl_gettext)(ثابت حرف *) = null_libintl_gettext;
حرف *(*dyn_libintl_ngettext)(ثابت حرف *, ثابت حرف *, غیرعلامتی طویل n)
						= null_libintl_ngettext;
حرف *(*dyn_libintl_textdomain)(ثابت حرف *) = null_libintl_textdomain;
حرف *(*dyn_libintl_bindtextdomain)(ثابت حرف *, ثابت حرف *)
						= null_libintl_bindtextdomain;
حرف *(*dyn_libintl_bind_textdomain_codeset)(ثابت حرف *, ثابت حرف *)
				       = null_libintl_bind_textdomain_codeset;
صحیح (*dyn_libintl_wputenv)(ثابت wchar_t *) = null_libintl_wputenv;
صحیح *dyn_libintl_nl_پیغام_cat_cntr = تہی;

    صحیح
dyn_libintl_init(خالی)
{
    صحیح i;
    ساکن ساخت
    {
	حرف	    *name;
	FARPROC	    *ptr;
    } libintl_اندراج[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{تہی, تہی}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    اگر (hLibintlDLL != تہی)
	واپس 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vim_بھرو_Lib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    اگر (!hLibintlDLL)
	hLibintlDLL = vim_بھرو_Lib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    اگر (!hLibintlDLL)
	hLibintlDLL = vim_بھرو_Lib(GETTEXT_DLL_ALT2);
# endif
    اگر (!hLibintlDLL)
    {
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_بھرو_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	واپس 0;
    }
    برائے (i = 0; libintl_اندراج[i].name != تہی
					 && libintl_اندراج[i].ptr != تہی; ++i)
    {
	اگر ((*libintl_اندراج[i].ptr = Get_Proc_پتہ(hLibintlDLL,
					      libintl_اندراج[i].name)) == تہی)
	{
	    dyn_libintl_انتہا();
	    اگر (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_بھرو_library_function_str), libintl_اندراج[i].name);
		verbose_leave();
	    }
	    واپس 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (حرف *(*)(ثابت حرف *, ثابت حرف *))
			Get_Proc_پتہ(hLibintlDLL, "bind_textdomain_codeset");
    اگر (dyn_libintl_bind_textdomain_codeset == تہی)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = ڈھونڈو_imported_module_by_funcname(hLibintlDLL, "getenv");
    اگر (hmsvcrt != تہی)
	dyn_libintl_wputenv = (صحیح (*)(ثابت wchar_t *))
					Get_Proc_پتہ(hmsvcrt, "_wputenv");
    اگر (dyn_libintl_wputenv == تہی || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_پیغام_cat_cntr = (صحیح *)Get_Proc_پتہ(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    واپس 1;
}

    خالی
dyn_libintl_انتہا(خالی)
{
    اگر (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= تہی;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_پیغام_cat_cntr	= تہی;
}

    ساکن حرف *
null_libintl_gettext(ثابت حرف *msgid)
{
    واپس (حرف*)msgid;
}

    ساکن حرف *
null_libintl_ngettext(
	ثابت حرف *msgid,
	ثابت حرف *msgid_plural,
	غیرعلامتی طویل n)
{
    واپس (حرف *)(n == 1 ? msgid : msgid_plural);
}

    ساکن حرف *
null_libintl_bindtextdomain(
	ثابت حرف *domainname غیر_مستعمل,
	ثابت حرف *dirname غیر_مستعمل)
{
    واپس تہی;
}

    ساکن حرف *
null_libintl_bind_textdomain_codeset(
	ثابت حرف *domainname غیر_مستعمل,
	ثابت حرف *codeset غیر_مستعمل)
{
    واپس تہی;
}

    ساکن حرف *
null_libintl_textdomain(ثابت حرف *domainname غیر_مستعمل)
{
    واپس تہی;
}

    ساکن صحیح
null_libintl_wputenv(ثابت wchar_t *envstring غیر_مستعمل)
{
    واپس 0;
}

#اگر_ختم // DYNAMIC_GETTEXT

// This symbol is not defined in older versions of the SDK or Visual C++

#اگر_غیر_تعریف_شدہ VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#اگر_ختم

#اگر_تعریف_شدہ HAVE_ACL
# include <aclapi.h>
# ifndef PROTECTED_DACL_SECURITY_INFORMATION
#  define PROTECTED_DACL_SECURITY_INFORMATION	0x80000000L
# endif
#اگر_ختم

#اگر_تعریف_شدہ HAVE_ACL
/*
 * Enables or disables the specified privilege.
 */
    ساکن BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    اگر (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	واپس FALSE;

    اگر (!Lookup_Privilege_قدر(تہی, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	واپس FALSE;
    }

    tokenPrivileges.Privilege_گنتی	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].اوصاف_2 = SE_PRIVILEGE_ENABLED;

    اگر (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, تہی, تہی))
    {
	CloseHandle(hToken);
	واپس FALSE;
    }

    اگر (Get_آخری_Error() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	واپس FALSE;
    }

    CloseHandle(hToken);
    واپس TRUE;
}
#اگر_ختم

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    خالی
PlatformId(خالی)
{
#اگر_تعریف_شدہ FEAT_EVAL
    DWORD major = (win_نسخہ >> 24) & 0xFF;
    DWORD minor = (win_نسخہ >> 16) & 0xFF;
    vim_snprintf(windows_نسخہ, حجم(windows_نسخہ), "%d.%d", major, minor);
#اگر_ختم

#اگر_تعریف_شدہ HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    اگر (!win32_enable_privilege(SE_SECURITY_NAME))
	واپس;
#اگر_ختم
}

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define سرکاؤ_2  (سرکاؤ_PRESSED)
# define CTRL   (دایاں_CTRL_PRESSED | بایاں_CTRL_PRESSED)
# define ALT    (دایاں_ALT_PRESSED  | بایاں_ALT_PRESSED)
# define ALT_GR (دایاں_ALT_PRESSED  | بایاں_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

ساکن ثابت ساخت
{
    لفظ_2    w_Virt_کلید;
    BOOL    f_Ansi_کلید;
    صحیح	    chAlone;
    صحیح	    ch_سرکاؤ;
    صحیح	    chCtrl;
    صحیح	    chAlt;
} Virt_کلید_Map[] =
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
    { VK_اوپر,	TRUE,	'H',	'\304',	'\305',	    '\306', },
    { VK_پیشگی,	TRUE,	'I',	'\307',	'\204',	    '\310', }, // PgUp
    { VK_بایاں,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_دایاں,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_انتہا,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_نیچے_کو,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_اگلا,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_داخل_کرو,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_عقب,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# اگر 0
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
    { VK_جوڑو,	TRUE,   'N',    'N',    'N',	'N',	}, // keyp '+'
    { VK_تفریق, TRUE,'J',	'J',    'J',	'J',	}, // keyp '-'
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
    ساکن صحیح
win32_kbd_patch_کلید(
    کلید_EVENT_ریکارڈ *pker)
{
    UINT uMods = pker->dw_Control_کلید_State;
    ساکن صحیح s_iIsDead = 0;
    ساکن لفظ_2 aw_Ansi_رمز[2];
    ساکن BYTE abKeystate[256];

    اگر (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_رمز[1];
	s_iIsDead = 0;
	واپس 1;
    }

    // check if it already has a valid unicode character.
    اگر (pker->uChar.UnicodeChar != 0)
	واپس 1;

    CLEAR_میدان(abKeystate);

    // Clear any pending dead keys
    مقصد_Unicode(VK_SPACE, Map_Virtual_کلید(VK_SPACE, 0), abKeystate, aw_Ansi_رمز, 2, 0);

    اگر (uMods & سرکاؤ_PRESSED)
	abKeystate[VK_سرکاؤ] = 0x80;
    اگر (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    اگر ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = مقصد_Unicode(pker->w_Virtual_کلید_رمز, pker->w_Virtual_جائزہ_رمز,
			abKeystate, aw_Ansi_رمز, 2, 0);

    اگر (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_رمز[0];

    واپس s_iIsDead;
}

ساکن BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    ساکن BOOL
decode_کلید_event(
    کلید_EVENT_ریکارڈ	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    صحیح			*pmodifiers,
    BOOL		fDoPost غیر_مستعمل)
{
    صحیح i;
    ثابت صحیح nModifs = pker->dw_Control_کلید_State & (سرکاؤ_2 | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    اگر (!pker->b_کلید_نیچے_کو)
	واپس FALSE;

    // ignore some keystrokes
    انتخاب (pker->w_Virtual_کلید_رمز)
    {
    // modifiers
    حالت VK_سرکاؤ:
    حالت VK_CONTROL:
    حالت VK_MENU:   // Alt key
	واپس FALSE;

    طےشدہ:
	توڑو;
    }

    // Shift-TAB
    اگر (pker->w_Virtual_کلید_رمز == VK_TAB && (nModifs & سرکاؤ_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	واپس TRUE;
    }

    برائے (i = ARRAY_لمبائی(Virt_کلید_Map); --i >= 0; )
    {
	اگر (Virt_کلید_Map[i].w_Virt_کلید == pker->w_Virtual_کلید_رمز)
	{
	    *pch = Virt_کلید_Map[i].chAlone;
	    اگر ((nModifs & سرکاؤ_2) != 0)
		*pch = Virt_کلید_Map[i].ch_سرکاؤ;
	    ورنہ اگر ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_کلید_Map[i].chCtrl;
	    ورنہ اگر ((nModifs & ALT) != 0)
		*pch = Virt_کلید_Map[i].chAlt;

	    اگر (*pch != 0)
	    {
		اگر (Virt_کلید_Map[i].f_Ansi_کلید)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    اگر (pmodifiers)
		    {
			اگر (pker->w_Virtual_کلید_رمز >= VK_F1
			    && pker->w_Virtual_کلید_رمز <= VK_F12)
			{
			    اگر ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_نقاب_ALT;
				اگر ((nModifs & سرکاؤ_2) == 0)
				    *pch2 = Virt_کلید_Map[i].chAlone;
			    }
			    اگر ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_نقاب_CTRL;
				اگر ((nModifs & سرکاؤ_2) == 0)
				    *pch2 = Virt_کلید_Map[i].chAlone;
			    }
			}
			ورنہ اگر (pker->w_Virtual_کلید_رمز >= VK_انتہا
				&& pker->w_Virtual_کلید_رمز <= VK_نیچے_کو)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_کلید_Map[i].chAlone;
			    اگر ((nModifs & سرکاؤ_2) != 0
						    && (nModifs & ~سرکاؤ_2) == 0)
			    {
				*pch2 = Virt_کلید_Map[i].ch_سرکاؤ;
			    }
			    اگر ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_کلید_Map[i].chCtrl;
				اگر (pker->w_Virtual_کلید_رمز == VK_اوپر
				    || pker->w_Virtual_کلید_رمز == VK_نیچے_کو)
				{
				    *pmodifiers |= MOD_نقاب_CTRL;
				    *pch2 = Virt_کلید_Map[i].chAlone;
				}
			    }
			    اگر ((nModifs & سرکاؤ_2) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_نقاب_CTRL;
				*pch2 = Virt_کلید_Map[i].ch_سرکاؤ;
			    }
			    اگر ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_کلید_Map[i].chAlt;
				*pmodifiers |= MOD_نقاب_ALT;
				اگر ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_کلید_Map[i].chAlone;
				}
				ورنہ اگر ((nModifs & سرکاؤ_2) != 0)
				{
				    *pch2 = Virt_کلید_Map[i].ch_سرکاؤ;
				}
				ورنہ اگر ((nModifs & CTRL) != 0)
				{
				    اگر (pker->w_Virtual_کلید_رمز == VK_اوپر
					|| pker->w_Virtual_کلید_رمز == VK_نیچے_کو)
				    {
					*pmodifiers |= MOD_نقاب_CTRL;
					*pch2 = Virt_کلید_Map[i].chAlone;
				    }
				    ورنہ
				    {
					*pch2 = Virt_کلید_Map[i].chCtrl;
				    }
				}
			    }
			}
			ورنہ اگر (pker->w_Virtual_کلید_رمز == VK_داخل_کرو
					&& (nModifs & سرکاؤ_2) != 0
					&& (nModifs & ~سرکاؤ_2) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_کلید_Map[i].ch_سرکاؤ;
			}
			ورنہ
			{
			    *pch2 = Virt_کلید_Map[i].chAlone;
			    اگر ((nModifs & سرکاؤ_2) != 0)
				*pmodifiers |= MOD_نقاب_سرکاؤ;
			    اگر ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_نقاب_CTRL;
			    اگر ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_نقاب_ALT;
			}
		    }
		}

		واپس TRUE;
	    }
	}
    }

    i = win32_kbd_patch_کلید(pker);

    اگر (i < 0)
	*pch = NUL;
    ورنہ
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	اگر (pmodifiers != تہی)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    اگر ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_نقاب_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    اگر ((nModifs & سرکاؤ_2) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_نقاب_سرکاؤ;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    اگر ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_نقاب_CTRL;
	}
    }

    واپس (*pch != NUL);
}

# اگر defined(FEAT_EVAL)
    ساکن صحیح
encode_کلید_event(dict_T *دلائل, داخلہ_ریکارڈ *ir)
{
    ساکن صحیح s_dwMods = 0;

    char_u *action = dict_get_string(دلائل, "event", TRUE);
    اگر (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_کلید_نیچے_کو = STRICMP(action, "keydown") == 0;
	لفظ_2 vk_رمز = dict_get_نمبر_def(دلائل, "keycode", 0);
	اگر (vk_رمز <= 0 || vk_رمز >= 0xFF)
	{
	    semsg(_(e_invalid_دلیل_nr), (طویل)vk_رمز);
	    واپس FALSE;
	}

	ir->EventType = کلید_EVENT;
	کلید_EVENT_ریکارڈ ker;
	Zero_حافظہ(&ker, حجم(ker));
	ker.b_کلید_نیچے_کو = is_کلید_نیچے_کو;
	ker.w_Repeat_گنتی = 1;
	ker.w_Virtual_جائزہ_رمز = 0;
	ker.dw_Control_کلید_State = 0;
	صحیح mods = (صحیح)dict_get_نمبر(دلائل, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	اگر (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    اگر (mods & MOD_نقاب_سرکاؤ)
		ker.dw_Control_کلید_State |= سرکاؤ_PRESSED;
	    اگر (mods & MOD_نقاب_CTRL)
		ker.dw_Control_کلید_State |= بایاں_CTRL_PRESSED;
	    اگر (mods & MOD_نقاب_ALT)
		ker.dw_Control_کلید_State |= بایاں_ALT_PRESSED;
	}

	اگر (vk_رمز == VK_LSHIFT || vk_رمز == VK_RSHIFT || vk_رمز == VK_سرکاؤ)
	{
	    اگر (is_کلید_نیچے_کو)
		s_dwMods |= سرکاؤ_PRESSED;
	    ورنہ
		s_dwMods &= ~سرکاؤ_PRESSED;
	}
	ورنہ اگر (vk_رمز == VK_LCONTROL || vk_رمز == VK_CONTROL)
	{
	    اگر (is_کلید_نیچے_کو)
		s_dwMods |= بایاں_CTRL_PRESSED;
	    ورنہ
		s_dwMods &= ~بایاں_CTRL_PRESSED;
	}
	ورنہ اگر (vk_رمز == VK_RCONTROL)
	{
	    اگر (is_کلید_نیچے_کو)
		s_dwMods |= دایاں_CTRL_PRESSED;
	    ورنہ
		s_dwMods &= ~دایاں_CTRL_PRESSED;
	}
	ورنہ اگر (vk_رمز == VK_LMENU || vk_رمز == VK_MENU)
	{
	    اگر (is_کلید_نیچے_کو)
		s_dwMods |= بایاں_ALT_PRESSED;
	    ورنہ
		s_dwMods &= ~بایاں_ALT_PRESSED;
	}
	ورنہ اگر (vk_رمز == VK_RMENU)
	{
	    اگر (is_کلید_نیچے_کو)
		s_dwMods |= دایاں_ALT_PRESSED;
	    ورنہ
		s_dwMods &= ~دایاں_ALT_PRESSED;
	}
	ker.dw_Control_کلید_State |= s_dwMods;
	ker.w_Virtual_کلید_رمز = vk_رمز;
	ker.uChar.UnicodeChar = 0;
	ir->Event.کلید_Event_2 = ker;
	vim_free(action);
    }
    ورنہ
    {
	اگر (action == تہی)
	{
	    semsg(_(e_missing_دلیل_str), "event");
	}
	ورنہ
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str_str), "event", action);
	    vim_free(action);
	}
	واپس FALSE;
    }
    واپس TRUE;
}
# endif  // FEAT_EVAL
#اگر_ختم // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساکن صحیح g_fMouseAvail = FALSE;   // mouse present
ساکن صحیح g_f_Mouse_فعال = FALSE;  // mouse enabled
ساکن صحیح g_nMouseClick = -1;	    // mouse status
ساکن صحیح g_xMouse;		    // mouse x coordinate
ساکن صحیح g_yMouse;		    // mouse y coordinate
ساکن DWORD g_cmodein = 0;	    // Original console input mode
ساکن DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    خالی
mch_setmouse(صحیح on)
{
    DWORD cmodein;

# ifdef VIMDLL
    اگر (gui.اندر_use)
	واپس;
# endif
    اگر (!g_fMouseAvail)
	واپس;

    g_f_Mouse_فعال = on;
    Get_Console_منوال(g_h_Con_اندر, &cmodein);

    اگر (g_f_Mouse_فعال)
    {
	cmodein |= ENABLE_MOUSE_داخلہ;
	cmodein &= ~ENABLE_تیز_EDIT_منوال;
    }
    ورنہ
    {
	cmodein &= ~ENABLE_MOUSE_داخلہ;
	cmodein |= g_cmodein & ENABLE_تیز_EDIT_منوال;
    }

    سیٹ_Console_منوال(g_h_Con_اندر, cmodein | ENABLE_EXTENDED_FLAGS);
}


# اگر defined(FEAT_BEVAL_رکن)
/*
 * Called when 'balloonevalterm' changed.
 */
    خالی
mch_bevalterm_بدلا(خالی)
{
    mch_setmouse(g_f_Mouse_فعال);
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
    ساکن خالی
decode_mouse_wheel(MOUSE_EVENT_ریکارڈ *pmer)
{
    صحیح	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    صحیح	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    صحیح lcol = g_xMouse;
    صحیح lrow = g_yMouse;
    win_T *wp = mouse_ڈھونڈو_win(&lrow, &lcol, ڈھونڈو_POPUP);
    اگر (wp != تہی && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_میدان(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	اگر (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_بایاں : MSCR_دایاں;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	ورنہ
	{
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	    cap.arg = zDelta < 0 ? MSCR_اوپر : MSCR_نیچے_کو;
	}

	// Mouse hovers over popup window, scroll it if possible.
	mouse_صف = wp->w_winrow;
	mouse_ستون = wp->w_wincol;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	برآمد_flush();
    }
# endif
    mouse_ستون = g_xMouse;
    mouse_صف = g_yMouse;

    char_u modifiers = 0;
    char_u direction = 0;

    // Decode the direction into an event that Vim can process
    اگر (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    ورنہ
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    اگر (pmer->dw_Control_کلید_State & سرکاؤ_PRESSED)
	modifiers |= MOD_نقاب_سرکاؤ; // MOUSE_SHIFT;
    اگر (pmer->dw_Control_کلید_State & (دایاں_CTRL_PRESSED | بایاں_CTRL_PRESSED))
	modifiers |= MOD_نقاب_CTRL; // MOUSE_CTRL;
    اگر (pmer->dw_Control_کلید_State & (دایاں_ALT_PRESSED  | بایاں_ALT_PRESSED))
	modifiers |= MOD_نقاب_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    واپس;
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
    ساکن BOOL
decode_mouse_event(
    MOUSE_EVENT_ریکارڈ *pmer)
{
    ساکن صحیح s_nOldButton = -1;
    ساکن صحیح s_nOldMouseClick = -1;
    ساکن صحیح s_xOldMouse = -1;
    ساکن صحیح s_yOldMouse = -1;
    ساکن linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    ساکن صحیح s_old_topfill = 0;
# endif
    ساکن صحیح s_cClicks = 1;
    ساکن BOOL s_fReleased = TRUE;
    ساکن DWORD s_dw_آخری_Click_وقت = 0;
    ساکن BOOL s_f_اگلا_Is_درمیان = FALSE;

    ساکن DWORD cButtons = 0;	// number of buttons supported

    ثابت DWORD بایاں_2 = FROM_بایاں_1_ST_BUTTON_PRESSED;
    ثابت DWORD درمیان = FROM_بایاں_2_ND_BUTTON_PRESSED;
    ثابت DWORD دایاں_2 = RIGHTMOST_BUTTON_PRESSED;
    ثابت DWORD بایاں_دایاں = بایاں_2 | دایاں_2;

    صحیح nButton;

    اگر (cButtons == 0 && !Get_نمبر_کا_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    اگر (!g_fMouseAvail || !g_f_Mouse_فعال)
    {
	g_nMouseClick = -1;
	واپس FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    اگر (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	واپس FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    اگر (g_nMouseClick != -1)
	واپس TRUE;

    اگر (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	واپس TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    اگر (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	اگر (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    واپس FALSE;
    }

    // If no buttons are pressed...
    اگر ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	اگر (s_fReleased)
	{
# ifdef FEAT_BEVAL_رکن
	    // do return mouse move events when we want them
	    اگر (p_bevalterm)
		nButton = MOUSE_DRAG;
	    ورنہ
# endif
		واپس FALSE;
	}

	s_fReleased = TRUE;
    }
    ورنہ    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	اگر (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & بایاں_دایاں);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    اگر (dwLR == بایاں_2 || dwLR == دایاں_2)
	    {
		برائے (;;)
		{
		    // wait a short time for next input event
		    اگر (WaitForSingleObject(g_h_Con_اندر, p_mouset / 3)
							     != WAIT_OBJECT_0)
			توڑو;
		    ورنہ
		    {
			DWORD c_اندراجات = 0;
			داخلہ_ریکارڈ ir;
			MOUSE_EVENT_ریکارڈ* pmer2 = &ir.Event.MouseEvent;

			peek_console_داخلہ(g_h_Con_اندر, &ir, 1, &c_اندراجات);

			اگر (c_اندراجات == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & بایاں_دایاں))
			    توڑو;
			ورنہ
			{
			    اگر (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_داخلہ(g_h_Con_اندر, &ir, 1, &c_اندراجات);

				واپس decode_mouse_event(pmer2);
			    }
			    ورنہ اگر (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_داخلہ(g_h_Con_اندر, &ir, 1, &c_اندراجات);

				// are there any more mouse events in queue?
				peek_console_داخلہ(g_h_Con_اندر, &ir, 1, &c_اندراجات);

				اگر (c_اندراجات==0 || ir.EventType != MOUSE_EVENT)
				    توڑو;
			    }
			    ورنہ
				توڑو;
			}
		    }
		}
	    }
	}

	اگر (s_f_اگلا_Is_درمیان)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_درمیان;
	    s_f_اگلا_Is_درمیان = FALSE;
	}
	ورنہ اگر (cButtons == 2	&&
	    ((pmer->dwButtonState & بایاں_دایاں) == بایاں_دایاں))
	{
	    nButton = MOUSE_درمیان;

	    اگر (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_اگلا_Is_درمیان = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	ورنہ اگر ((pmer->dwButtonState & بایاں_2) == بایاں_2)
	    nButton = MOUSE_بایاں;
	ورنہ اگر ((pmer->dwButtonState & درمیان) == درمیان)
	    nButton = MOUSE_درمیان;
	ورنہ اگر ((pmer->dwButtonState & دایاں_2) == دایاں_2)
	    nButton = MOUSE_دایاں;

	اگر (! s_fReleased && ! s_f_اگلا_Is_درمیان
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    واپس FALSE;

	s_fReleased = s_f_اگلا_Is_درمیان;
    }

    اگر (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	اگر (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dw_Current_وقت = Get_Tick_گنتی();

	    اگر (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (صحیح)(dw_Current_وقت - s_dw_آخری_Click_وقت) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    ورنہ اگر (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dw_آخری_Click_وقت = dw_Current_وقت;
	}
    }
    ورنہ اگر (pmer->dwEventFlags == MOUSE_MOVED)
    {
	اگر (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    اگر (nButton == -1)
	واپس FALSE;

    اگر (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    اگر (pmer->dw_Control_کلید_State & سرکاؤ_PRESSED)
	g_nMouseClick |= MOUSE_سرکاؤ;
    اگر (pmer->dw_Control_کلید_State & (دایاں_CTRL_PRESSED | بایاں_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    اگر (pmer->dw_Control_کلید_State & (دایاں_ALT_PRESSED  | بایاں_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    اگر (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	سیٹ_عدد_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    اگر (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	واپس FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    واپس TRUE;
}

# ifdef FEAT_EVAL
    ساکن صحیح
encode_mouse_event(dict_T *دلائل, داخلہ_ریکارڈ *ir)
{
    صحیح		button;
    صحیح		صف;
    صحیح		ستون;
    صحیح		repeated_click;
    int_u	mods = 0;
    صحیح		move;

    اگر (!dict_has_کلید(دلائل, "row") || !dict_has_کلید(دلائل, "col"))
	واپس FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (صحیح)dict_get_bool(دلائل, "move", FALSE);
    اگر (!move && (!dict_has_کلید(دلائل, "button")
	    || !dict_has_کلید(دلائل, "multiclick")
	    || !dict_has_کلید(دلائل, "modifiers")))
	واپس FALSE;

    صف = (صحیح)dict_get_نمبر(دلائل, "row") - 1;
    ستون = (صحیح)dict_get_نمبر(دلائل, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_ریکارڈ mer;
    Zero_حافظہ(&mer, حجم(mer));
    mer.dwMousePosition.X  = ستون;
    mer.dwMousePosition.Y  = صف;

    اگر (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    ورنہ
    {
	button = (صحیح)dict_get_نمبر(دلائل, "button");
	repeated_click = (صحیح)dict_get_نمبر(دلائل, "multiclick");
	mods = (صحیح)dict_get_نمبر(دلائل, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_سیٹ_hor_scroll_قدم(6);
	mouse_سیٹ_vert_scroll_قدم(3);

	انتخاب (button)
	{
	    حالت MOUSE_بایاں:
		mer.dwButtonState = FROM_بایاں_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		توڑو;
	    حالت MOUSE_درمیان:
		mer.dwButtonState = FROM_بایاں_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		توڑو;
	    حالت MOUSE_دایاں:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		توڑو;
	    حالت MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    حالت MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		توڑو;
	    حالت MOUSE_X1:
		mer.dwButtonState = FROM_بایاں_3_RD_BUTTON_PRESSED;
		توڑو;
	    حالت MOUSE_X2:
		mer.dwButtonState = FROM_بایاں_4_TH_BUTTON_PRESSED;
		توڑو;
	    حالت MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		توڑو;
	    حالت MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		توڑو;
	    حالت MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		توڑو;
	    حالت MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		توڑو;
	    طےشدہ:
		semsg(_(e_invalid_دلیل_str), "button");
		واپس FALSE;
	}
    }

    mer.dw_Control_کلید_State = 0;
    اگر (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	اگر (mods & MOUSE_سرکاؤ)
	    mer.dw_Control_کلید_State |= سرکاؤ_PRESSED;
	اگر (mods & MOUSE_CTRL)
	    mer.dw_Control_کلید_State |= بایاں_CTRL_PRESSED;
	اگر (mods & MOUSE_ALT)
	    mer.dw_Control_کلید_State |= بایاں_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    واپس TRUE;
}
# endif  // FEAT_EVAL

    ساکن صحیح
write_داخلہ_ریکارڈ_بفر(داخلہ_ریکارڈ* irEvents, صحیح n_لمبائی)
{
    صحیح				n_گنتی = 0;
    داخلہ_ریکارڈ_بفر_گرہ_T	*event_گرہ;

    جب (n_گنتی < n_لمبائی)
    {
	event_گرہ = alloc(حجم(داخلہ_ریکارڈ_بفر_گرہ_T));
	اگر (event_گرہ == تہی)
	    واپس -1;

	event_گرہ->ir = irEvents[n_گنتی++];
	event_گرہ->اگلا = تہی;

	داخلہ_ریکارڈ_بفر.لمبائی++;
	اگر (داخلہ_ریکارڈ_بفر.دم == تہی)
	{
	    داخلہ_ریکارڈ_بفر.سر = event_گرہ;
	    داخلہ_ریکارڈ_بفر.دم = event_گرہ;
	}
	ورنہ
	{
	    داخلہ_ریکارڈ_بفر.دم->اگلا = event_گرہ;
	    داخلہ_ریکارڈ_بفر.دم = داخلہ_ریکارڈ_بفر.دم->اگلا;
	}
    }
    واپس n_گنتی;
}

    ساکن صحیح
read_داخلہ_ریکارڈ_بفر(داخلہ_ریکارڈ* irEvents, صحیح n_Max_لمبائی)
{
    صحیح n_گنتی = 0;
    جب (n_گنتی < n_Max_لمبائی && داخلہ_ریکارڈ_بفر.سر != تہی)
    {
	داخلہ_ریکارڈ_بفر.لمبائی--;
	داخلہ_ریکارڈ_بفر_گرہ_T *نکالو_سر = داخلہ_ریکارڈ_بفر.سر;
	irEvents[n_گنتی++] = نکالو_سر->ir;
	داخلہ_ریکارڈ_بفر.سر = نکالو_سر->اگلا;
	vim_free(نکالو_سر);
	اگر (داخلہ_ریکارڈ_بفر.لمبائی == 0)
	    داخلہ_ریکارڈ_بفر.دم = تہی;
    }
    واپس n_گنتی;
}
#ورنہ  // FEAT_GUI_MSWIN && !VIMDLL
    خالی
mch_setmouse(صحیح on غیر_مستعمل)
{
}
#اگر_ختم // FEAT_GUI_MSWIN && !VIMDLL

#اگر_تعریف_شدہ FEAT_EVAL
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
    صحیح
test_mswin_event(char_u *event, dict_T *دلائل)
{
    صحیح lpEventsWritten = 0;

# اگر defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    اگر (gui.اندر_use)
	واپس test_gui_w32_sendevent(event, دلائل);
# endif

# اگر defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    داخلہ_ریکارڈ ir;
    BOOL داخلہ_encoded = FALSE;
    BOOL execute = FALSE;
    اگر (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(دلائل, "execute", FALSE);
	اگر (dict_has_کلید(دلائل, "event"))
	    داخلہ_encoded = encode_کلید_event(دلائل, &ir);
	ورنہ اگر (!execute)
	{
	    semsg(_(e_missing_دلیل_str), "event");
	    واپس FALSE;
	}
    }
    ورنہ اگر (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	داخلہ_encoded = encode_mouse_event(دلائل, &ir);
    }
    ورنہ
    {
	semsg(_(e_invalid_قدر_for_دلیل_str_str), "event", event);
	واپس FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    اگر (داخلہ_encoded)
    {
	اگر ((lpEventsWritten = write_داخلہ_ریکارڈ_بفر(&ir, 1)) < 0)
	{
	    semsg(_(e_برآمد_کا_حافظہ), "event");
	    واپس FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    اگر (execute)
    {
	صحیح save_پیغام_scroll = پیغام_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	پیغام_scroll = FALSE;
	ch_log(تہی, "test_mswin_event() executing");
	exec_معمول(TRUE, TRUE, TRUE);
	پیغام_scroll |= save_پیغام_scroll;
    }

# endif
    واپس lpEventsWritten;
}
#اگر_ختم // FEAT_EVAL

#اگر defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    ساکن خالی
mch_سیٹ_cursor_shape(صحیح thickness)
{
    اگر (vtp_working)
    {
	اگر (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    اگر (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    ورنہ
		vtp_printf("\033[0 q");	// default
	}
    }
    ورنہ
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dw_سائز = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	سیٹ_Console_Cursor_Info(g_h_Con_برآمد, &ConsoleCursorInfo);
	اگر (s_cursor_visible)
	    سیٹ_Console_Cursor_Position(g_h_Con_برآمد, g_coord);
    }
}

    خالی
mch_update_cursor(خالی)
{
    صحیح		idx;
    صحیح		thickness;

# ifdef VIMDLL
    اگر (gui.اندر_use)
	واپس;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    اگر (shape_جدول[idx].shape == SHAPE_بلاک)
	thickness = 99;	// 100 doesn't work on W95
    ورنہ
	thickness = shape_جدول[idx].percentage;
    mch_سیٹ_cursor_shape(thickness);
}
#اگر_ختم

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    ساکن خالی
handle_focus_event(داخلہ_ریکارڈ ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.b_سیٹ_Focus;
    ui_focus_change((صحیح)g_fJustGotFocus);
}

ساکن خالی ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    ساکن صحیح
WaitForChar(طویل msec, صحیح ignore_داخلہ)
{
    DWORD	    dwNow = 0, dw_انتہا_وقت = 0;
    داخلہ_ریکارڈ    ir;
    DWORD	    c_اندراجات;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    صحیح		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    اگر (msec > 0)
	// Wait until the specified time has elapsed.
	dw_انتہا_وقت = Get_Tick_گنتی() + msec;
    ورنہ اگر (msec < 0)
	// Wait forever.
	dw_انتہا_وقت = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    برائے (;;)
    {
	// Only process messages when waiting.
	اگر (msec != 0)
	{
# ifdef پیغام_قطار
	    parse_queued_messages();
# endif
# ifdef FEAT_MZSCHEME
	    mzvim_check_دھاگے();
# endif
# ifdef FEAT_CLIENTSERVER
	    serverProcessPendingMessages();
# endif
	}

	اگر (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_داخلہ && داخلہ_دستیاب())
# endif
	   )
	    واپس TRUE;

	اگر (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_گنتی();
	    اگر ((صحیح)(dwNow - dw_انتہا_وقت) >= 0)
		توڑو;
	}
	اگر (msec != 0)
	{
	    DWORD dw_Wait_وقت = dw_انتہا_وقت - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    اگر (dw_Wait_وقت > 11)
		dw_Wait_وقت = 11;

# ifdef FEAT_MZSCHEME
	    اگر (mzthreads_allowed() && p_mzq > 0 && (طویل)dw_Wait_وقت > p_mzq)
		dw_Wait_وقت = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    اگر (dw_Wait_وقت > 10)
	    {
		طویل	due_وقت;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_وقت = check_due_timer();
		اگر (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    واپس FALSE;
		}
		اگر (due_وقت > 0 && dw_Wait_وقت > (DWORD)due_وقت)
		    dw_Wait_وقت = due_وقت;
	    }
# endif
	    اگر (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    پیغام_wait_for_مضرب_objects(1, &g_h_Con_اندر, FALSE,
				  dw_Wait_وقت, QS_SENDMESSAGE) != WAIT_OBJECT_0
# ورنہ
		    wait_for_single_object(g_h_Con_اندر, dw_Wait_وقت)
							      != WAIT_OBJECT_0
# endif
		    )
		جاری;
	}

	c_اندراجات = 0;
	peek_console_داخلہ(g_h_Con_اندر, &ir, 1, &c_اندراجات);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	اگر (State == منوال_CMDLINE && پیغام_صف == صفیں - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_بفر_INFO csbi;

	    اگر (Get_Console_Screen_بفر_Info(g_h_Con_برآمد, &csbi))
	    {
		اگر (csbi.dwCursorPosition.Y != پیغام_صف)
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

	اگر (c_اندراجات > 0)
	{
	    اگر (ir.EventType == کلید_EVENT && ir.Event.کلید_Event_2.b_کلید_نیچے_کو)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		اگر (ir.Event.کلید_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.کلید_Event_2.w_Virtual_کلید_رمز == 13)
		{
		    read_console_داخلہ(g_h_Con_اندر, &ir, 1, &c_اندراجات);
		    جاری;
		}
# endif
		اگر (decode_کلید_event(&ir.Event.کلید_Event_2, &ch, &ch2,
								 تہی, FALSE))
		    واپس TRUE;
	    }

	    read_console_داخلہ(g_h_Con_اندر, &ir, 1, &c_اندراجات);

	    اگر (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    ورنہ اگر (ir.EventType == کھڑکی_بفر_سائز_EVENT)
	    {
		COORD dw_سائز = ir.Event.کھڑکی_بفر_سائز_Event.dw_سائز;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		اگر (dw_سائز.X != Columns || dw_سائز.Y != صفیں)
		{
		    CONSOLE_SCREEN_بفر_INFO csbi;
		    Get_Console_Screen_بفر_Info(g_h_Con_برآمد, &csbi);
		    dw_سائز.X = csbi.sr_کھڑکی.دایاں_3 - csbi.sr_کھڑکی.بایاں_3 + 1;
		    dw_سائز.Y = csbi.sr_کھڑکی.Bottom - csbi.sr_کھڑکی.چوٹی_2 + 1;
		    اگر (dw_سائز.X != Columns || dw_سائز.Y != صفیں)
		    {
			ResizeConBuf(g_h_Con_برآمد, dw_سائز);
			shell_resized();
		    }
		}
	    }
	    ورنہ اگر (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		واپس TRUE;
	}
	ورنہ اگر (msec == 0)
	    توڑو;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    اگر (داخلہ_دستیاب())
	واپس TRUE;
# endif

    واپس FALSE;
}

/*
 * return non-zero if a character is available
 */
    صحیح
mch_char_avail(خالی)
{
# ifdef VIMDLL
    اگر (gui.اندر_use)
	واپس TRUE;
# endif
    واپس WaitForChar(0L, FALSE);
}

# اگر defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    صحیح
mch_check_messages(خالی)
{
#  ifdef VIMDLL
    اگر (gui.اندر_use)
	واپس TRUE;
#  endif
    واپس WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    ساکن خالی
create_conin(خالی)
{
    g_h_Con_اندر =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_اوصاف) تہی,
			OPEN_EXISTING, 0, (HANDLE)تہی);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    ساکن WCHAR
tgetch(صحیح *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    برائے (;;)
    {
	داخلہ_ریکارڈ ir;
	DWORD c_اندراجات = 0;

# ifdef FEAT_CLIENTSERVER
	(خالی)WaitForChar(-1L, FALSE);
	اگر (داخلہ_دستیاب())
	    واپس 0;
	اگر (g_nMouseClick != -1)
	    واپس 0;
# endif
	اگر (read_console_داخلہ(g_h_Con_اندر, &ir, 1, &c_اندراجات) == 0)
	{
	    اگر (did_create_conin)
		read_error_exit();
	    create_conin();
	    جاری;
	}

	اگر (ir.EventType == کلید_EVENT)
	{
	    اگر (decode_کلید_event(&ir.Event.کلید_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		واپس ch;
	}
	ورنہ اگر (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	ورنہ اگر (ir.EventType == کھڑکی_بفر_سائز_EVENT)
	    shell_resized();
	ورنہ اگر (ir.EventType == MOUSE_EVENT)
	{
	    اگر (decode_mouse_event(&ir.Event.MouseEvent))
		واپس 0;
	}
    }
}
#اگر_ختم // !FEAT_GUI_MSWIN


/*
 * mch_inchar(): low-level input function.
 * Get one or more characters from the keyboard or the mouse.
 * If time == 0, do not wait for characters.
 * If time == n, wait a short time for characters.
 * If time == -1, wait forever for characters.
 * Returns the number of characters read into buf.
 */
    صحیح
mch_inchar(
    char_u	*buf غیر_مستعمل,
    صحیح		maxlen غیر_مستعمل,
    طویل	وقت غیر_مستعمل,
    صحیح		tb_change_cnt غیر_مستعمل)
{
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    صحیح		len;
    صحیح		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# ورنہ
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    ساکن char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    ساکن صحیح	    typeaheadlen = 0;

# ifdef VIMDLL
    اگر (gui.اندر_use)
	واپس 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    اگر (typeaheadlen > 0)
	جاؤ theend;

    اگر (وقت >= 0)
    {
	اگر (!WaitForChar(وقت, FALSE))     // no character available
	    واپس 0;
    }
    ورنہ    // time == -1, wait forever
    {
	mch_سیٹ_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	اگر (!WaitForChar(p_ut, FALSE))
	{
	    اگر (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (صحیح)KE_CURSORHOLD;
		واپس 3;
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
    اگر (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    جب ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	اگر (typebuf_بدلا(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    توڑو;
	}
	اگر (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    اگر (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    اگر (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		اگر (modifiers > 0)
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
	    ورنہ
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
	ورنہ
	{
	    WCHAR	ch2 = NUL;
	    صحیح		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_کلید(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_کلیدیں;
	    c = may_adjust_کلید_for_ctrl(modifiers, c);
	    --no_reduce_کلیدیں;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_سرکاؤ_modifier(modifiers, c);

	    اگر (typebuf_بدلا(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		توڑو;
	    }

	    اگر (c == Ctrl_C && ctrl_c_interrupts)
	    {
# اگر defined(FEAT_CLIENTSERVER)
		trash_داخلہ_buf();
# endif
		got_int = TRUE;
	    }

	    اگر (g_nMouseClick == -1)
	    {
		صحیح	n = 1;

		اگر (ch2 == NUL)
		{
		    صحیح	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    اگر (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_مقصد_enc(ch, &n);
		    اگر (p != تہی)
		    {
			برائے (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    اگر (p[i] == CSI)
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
		ورنہ
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    اگر (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		اگر (ch2 != NUL)
		{
		    اگر (c == K_NUL)
		    {
			انتخاب (ch2)
			{
			حالت (WCHAR)'\324': // SHIFT+Insert
			حالت (WCHAR)'\325': // CTRL+Insert
			حالت (WCHAR)'\327': // SHIFT+Delete
			حالت (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    توڑو;

			طےشدہ:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    توڑو;
			}
		    }
		    ورنہ
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
		اگر ((modifiers & MOD_نقاب_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_نقاب_ALT;
		}

		اگر (modifiers != 0)
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
		اگر (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    اگر (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    جب (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    اگر (len > 0)
    {
	buf[len] = NUL;
	ch_log(تہی, "raw key input: \"%s\"", buf);
    }
# endif
    واپس len;

#ورنہ // FEAT_GUI_MSWIN && !VIMDLL
    واپس 0;
#اگر_ختم // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    ساکن صحیح
executable_file(حرف *name, char_u **راستہ)
{
    صحیح attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    اگر (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	واپس FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    اگر (attrs & FILE_ATTRIBUTE_REPARSE_نقطہ)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	اگر (res == تہی)
	    res = resolve_reparse_نقطہ((char_u *)name);
	اگر (res == تہی)
	    واپس FALSE;
	// The path is already absolute.
	اگر (راستہ != تہی)
	    *راستہ = res;
	ورنہ
	    vim_free(res);
    }
    ورنہ اگر (راستہ != تہی)
	*راستہ = بھرا_Name_save((char_u *)name, FALSE);
    واپس TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    ساکن bool
executable_سابقہ_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_ڈھونڈو_DATAW	ڈیٹا;
    HANDLE		handle;
    DWORD		error;

    wn = enc_مقصد_utf16(name, تہی);
    اگر (wn == تہی)
	واپس true;
    handle = ڈھونڈو_پہلا_File_Ex_W(wn, ڈھونڈو_Ex_Info_Basic, &ڈیٹا,
					    ڈھونڈو_Ex_تلاش_Name_مطابقت, تہی, 0);
    error = Get_آخری_Error();
    vim_free(wn);
    اگر (handle != INVALID_HANDLE_قدر)
    {
	ڈھونڈو_Close(handle);
	واپس true;
    }
    واپس error != ERROR_FILE_NOT_ملا && error != ERROR_راستہ_NOT_ملا;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    ساکن صحیح
executable_exists(
    حرف	*name,
    size_t	namelen,
    char_u	**راستہ,
    صحیح		use_راستہ,
    صحیح		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[MAX_راستہ_2 * 3];
    char_u	*p;
    string_T	pathbuf = {تہی, 0};
    صحیح		pathbuf_allocated = FALSE;
    string_T	pathext = {تہی, 0};
    صحیح		pathext_allocated = FALSE;
    char_u	*shname = تہی;
    صحیح		noext = FALSE;
    bool	simple_pathext = true;
    صحیح		ext_گنتی = 0;
    صحیح		retval = FALSE;

    اگر (namelen >= حجم(buf))	// safety check
	واپس FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    اگر (لڑی_تلاش((حرف *)shname, "sh") != تہی &&
	!(لڑی_تلاش((حرف *)shname, "powershell") != تہی
				    || لڑی_تلاش((حرف *)shname, "pwsh") != تہی))
	noext = TRUE;

    اگر (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	اگر (pathext.string == تہی)
	    STR_LITERAL_سیٹ(pathext, ".com;.exe;.bat;.cmd");
	ورنہ
	    pathext.لمبائی = STRLEN(pathext.string);

	اگر (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    جب (*p)
	    {
		اگر (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    اگر (*p == '.')
			simple_pathext = false;
		    ++p;
		    جاری;
		}
		e = vim_strchr(p, ';');
		اگر (e == تہی)
		    e = pathext.string + pathext.لمبائی;
		plen = (size_t)(e - p);
		++ext_گنتی;
		اگر (*p != '.')
		    simple_pathext = false;
		برائے (q = p + 1; q < e; ++q)
		    اگر (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		اگر (_strnicoll(name + namelen - plen, (حرف *)p, plen) == 0)
		{
		    noext = TRUE;
		    توڑو;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    اگر (pathext.string == تہی)
	STR_LITERAL_سیٹ(pathext, ".");
    ورنہ اگر (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.لمبائی + 3);
	اگر (tmp == تہی)
	{
	    retval = FALSE;
	    جاؤ theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.لمبائی += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    اگر (use_راستہ && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	اگر (p != تہی)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    اگر (pathbuf.string == تہی)
	    {
		retval = FALSE;
		جاؤ theend;
	    }

	    اگر (mch_getenv("NoDefaultCurrentDirectoryInExePath") == تہی &&
		    لڑی_تلاش((حرف *)gettail(p_sh), "cmd.exe") != تہی)
	    {
		STRCPY(pathbuf.string, ".;");
		pathbuf.لمبائی = 2;
	    }
	    STRCPY(pathbuf.string + pathbuf.لمبائی, p);
	    pathbuf.لمبائی += plen;
	    pathbuf_allocated = TRUE;
	}
    }

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    اگر (pathbuf.string == تہی)
	STR_LITERAL_سیٹ(pathbuf, ".");
    p = pathbuf.string;
    جب (*p)
    {
	char_u	*e;
	size_t	buflen;

	اگر (*p == ';') // Skip empty entry
	{
	    ++p;
	    جاری;
	}
	e = vim_strchr(p, ';');
	اگر (e == تہی)
	    e = pathbuf.string + pathbuf.لمبائی;

	اگر (e - p + namelen + 2 > حجم(buf))
	{
	    retval = FALSE;
	    جاؤ theend;
	}
	// A single "." that means current dir.
	اگر (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	ورنہ
	{
	    buflen = vim_snprintf_safelen(
		(حرف *)buf,
		حجم(buf),
		"%.*s%s%s", (صحیح)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	اگر (!noext && simple_pathext && ext_گنتی >= 3 && buflen > 0
		&& buflen + 2 <= حجم(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == تہی
		&& vim_strchr(gettail(buf), ':') == تہی)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_سابقہ_exists(buf);
	    buf[buflen] = NUL;
	    اگر (!exists)
	    {
		p = e;
		جاری;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	جب (*p)
	{
	    char_u  *e2;

	    اگر (*p == ';')
	    {
		// Skip empty entry
		++p;
		جاری;
	    }
	    e2 = vim_strchr(p, ';');
	    اگر (e2 == تہی)
		e2 = pathext.string + pathext.لمبائی;

	    اگر (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		اگر (e2 - p + buflen + 1 > حجم(buf))
		{
		    retval = FALSE;
		    جاؤ theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    اگر (executable_file((حرف *)buf, راستہ))
	    {
		retval = TRUE;
		جاؤ theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    اگر (pathbuf_allocated)
	آزاد(pathbuf.string);
    اگر (pathext_allocated)
	آزاد(pathext.string);
    واپس retval;
}

#اگر (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    ساکن خالی
bad_param_handler(ثابت wchar_t *عبارت غیر_مستعمل,
    ثابت wchar_t *function غیر_مستعمل,
    ثابت wchar_t *file غیر_مستعمل,
    غیرعلامتی صحیح سطر غیر_مستعمل,
    uintptr_t pReserved غیر_مستعمل)
{
}

# define سیٹ_INVALID_PARAM_HANDLER \
	((خالی)سیٹ_invalid_parameter_handler(bad_param_handler))
#ورنہ
# define سیٹ_INVALID_PARAM_HANDLER
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    ساکن خالی
mch_init_g(خالی)
{
# ifndef __MINGW32__
    خارجی صحیح _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    سیٹ_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    سیٹ_Error_منوال(SEM_FAILCRITICALERRORS);

    _fmode = O_ثنائی;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    صفیں = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[MAX_راستہ_2 + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	اگر (exe_pathlen + 10 >= حجم(vimrun_location))
	{
	    اگر (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    تہی, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	ورنہ
	{
	    // First try in same directory as gvim.exe.
	    اگر (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    اگر (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		اگر (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(حرف *)vimrun_location,
			حجم(vimrun_location),
			"\"%.*svimrun\" ",
			(صحیح)exe_pathlen, exe_name);
		}
		ورنہ
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		اگر (p != تہی)
		{
		    vimrun_راستہ.string = p;
		    vimrun_راستہ.لمبائی = plen;
		    vimrun_راستہ_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    ورنہ اگر (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    تہی, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	اگر (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    اگر (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    تہی, TRUE, FALSE))
	سیٹ_option_قدر_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#اگر_ختم // FEAT_GUI_MSWIN

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).دایاں_3 - (sr).بایاں_3 + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).چوٹی_2 + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    ساکن BOOL
Clear_Console_بفر(لفظ_2 wAttribute)
{
    CONSOLE_SCREEN_بفر_INFO csbi;
    COORD coord;
    DWORD عدد_Cells, dummy;

    اگر (!Get_Console_Screen_بفر_Info(g_h_Con_برآمد, &csbi))
	واپس FALSE;

    عدد_Cells = csbi.dw_سائز.X * csbi.dw_سائز.Y;
    coord.X = 0;
    coord.Y = 0;
    اگر (!FillConsoleOutputCharacter(g_h_Con_برآمد, ' ', عدد_Cells,
	    coord, &dummy))
	واپس FALSE;
    اگر (!FillConsoleOutputAttribute(g_h_Con_برآمد, wAttribute, عدد_Cells,
	    coord, &dummy))
	واپس FALSE;

    واپس TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    ساکن BOOL
Fit_Console_کھڑکی(
    COORD dw_بفر_سائز,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_بفر_INFO csbi;
    COORD dw_کھڑکی_سائز;
    BOOL NeedAdjust = FALSE;

    اگر (!Get_Console_Screen_بفر_Info(g_h_Con_برآمد, &csbi))
	واپس FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    اگر (csbi.sr_کھڑکی.دایاں_3 >= dw_بفر_سائز.X)
    {
	dw_کھڑکی_سائز.X = SRWIDTH(csbi.sr_کھڑکی);
	اگر (dw_کھڑکی_سائز.X > dw_بفر_سائز.X)
	    dw_کھڑکی_سائز.X = dw_بفر_سائز.X;
	csbi.sr_کھڑکی.دایاں_3 = dw_بفر_سائز.X - 1;
	csbi.sr_کھڑکی.بایاں_3 = dw_بفر_سائز.X - dw_کھڑکی_سائز.X;
	NeedAdjust = TRUE;
    }
    اگر (csbi.sr_کھڑکی.Bottom >= dw_بفر_سائز.Y)
    {
	dw_کھڑکی_سائز.Y = SRHEIGHT(csbi.sr_کھڑکی);
	اگر (dw_کھڑکی_سائز.Y > dw_بفر_سائز.Y)
	    dw_کھڑکی_سائز.Y = dw_بفر_سائز.Y;
	csbi.sr_کھڑکی.Bottom = dw_بفر_سائز.Y - 1;
	csbi.sr_کھڑکی.چوٹی_2 = dw_بفر_سائز.Y - dw_کھڑکی_سائز.Y;
	NeedAdjust = TRUE;
    }
    اگر (NeedAdjust && WantAdjust)
    {
	اگر (!سیٹ_Console_کھڑکی_Info(g_h_Con_برآمد, TRUE, &csbi.sr_کھڑکی))
	    واپس FALSE;
    }
    واپس TRUE;
}

قسمتعریف ساخت Console_بفر_Struct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_بفر_INFO	Info;
    PCHAR_INFO			بفر_2;
    COORD			بفر_سائز_2;
    PSMALL_RECT			Regions;
    صحیح				عدد_Regions;
} Console_بفر;

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
    ساکن BOOL
Save_Console_بفر(
    Console_بفر *cb)
{
    DWORD عدد_Cells;
    COORD بفر_Coord;
    SMALL_RECT ReadRegion;
    لفظ_2 Y, Y_incr;
    صحیح i, numregions;

    اگر (cb == تہی)
	واپس FALSE;

    اگر (!Get_Console_Screen_بفر_Info(g_h_Con_برآمد, &cb->Info))
    {
	cb->IsValid = FALSE;
	واپس FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    اگر (use_alternate_screen_بفر)
	واپس TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    اگر (!cb->IsValid || cb->بفر_2 == تہی ||
	    cb->بفر_سائز_2.X != cb->Info.dw_سائز.X ||
	    cb->بفر_سائز_2.Y != cb->Info.dw_سائز.Y)
    {
	cb->بفر_سائز_2.X = cb->Info.dw_سائز.X;
	cb->بفر_سائز_2.Y = cb->Info.dw_سائز.Y;
	عدد_Cells = cb->بفر_سائز_2.X * cb->بفر_سائز_2.Y;
	vim_free(cb->بفر_2);
	cb->بفر_2 = ALLOC_MULT(CHAR_INFO, عدد_Cells);
	اگر (cb->بفر_2 == تہی)
	    واپس FALSE;
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
    بفر_Coord.X = 0;
    ReadRegion.بایاں_3 = 0;
    ReadRegion.دایاں_3 = cb->Info.dw_سائز.X - 1;
    Y_incr = 12000 / cb->Info.dw_سائز.X;

    numregions = (cb->Info.dw_سائز.Y + Y_incr - 1) / Y_incr;
    اگر (cb->Regions == تہی || numregions != cb->عدد_Regions)
    {
	cb->عدد_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->عدد_Regions);
	اگر (cb->Regions == تہی)
	{
	    VIM_CLEAR(cb->بفر_2);
	    واپس FALSE;
	}
    }

    برائے (i = 0, Y = 0; i < cb->عدد_Regions; i++, Y += Y_incr)
    {
	/*
	 * Read into position (0, Y) in our buffer.
	 */
	بفر_Coord.Y = Y;
	/*
	 * Read the region whose top left corner is (0, Y) and whose bottom
	 * right corner is (width - 1, Y + Y_incr - 1).  This should define
	 * a region of size width by Y_incr.  Don't worry if this region is
	 * too large for the remaining buffer; it will be cropped.
	 */
	ReadRegion.چوٹی_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	اگر (!ReadConsoleOutputW(g_h_Con_برآمد,	// output handle
		cb->بفر_2,			// our buffer
		cb->بفر_سائز_2,			// dimensions of our buffer
		بفر_Coord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->بفر_2);
	    VIM_CLEAR(cb->Regions);
	    واپس FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    واپس TRUE;
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
    ساکن BOOL
Restore_Console_بفر(
    Console_بفر   *cb,
    BOOL	    RestoreScreen)
{
    COORD بفر_Coord;
    SMALL_RECT WriteRegion;
    صحیح i;

    اگر (cb == تہی || !cb->IsValid)
	واپس FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    اگر (RestoreScreen)
	Clear_Console_بفر(cb->Info.w_اوصاف);

    Fit_Console_کھڑکی(cb->Info.dw_سائز, TRUE);
    اگر (!سیٹ_Console_Screen_بفر_سائز(g_h_Con_برآمد, cb->Info.dw_سائز))
	واپس FALSE;
    اگر (!سیٹ_Console_متن_Attribute(g_h_Con_برآمد, cb->Info.w_اوصاف))
	واپس FALSE;

    اگر (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	واپس TRUE;
    }

    اگر (!سیٹ_Console_Cursor_Position(g_h_Con_برآمد, cb->Info.dwCursorPosition))
	واپس FALSE;
    اگر (!سیٹ_Console_کھڑکی_Info(g_h_Con_برآمد, TRUE, &cb->Info.sr_کھڑکی))
	واپس FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    اگر (use_alternate_screen_بفر)
	واپس TRUE;

    /*
     * Restore the screen buffer contents.
     */
    اگر (cb->بفر_2 != تہی)
    {
	برائے (i = 0; i < cb->عدد_Regions; i++)
	{
	    بفر_Coord.X = cb->Regions[i].بایاں_3;
	    بفر_Coord.Y = cb->Regions[i].چوٹی_2;
	    WriteRegion = cb->Regions[i];
	    اگر (!WriteConsoleOutputW(g_h_Con_برآمد,	// output handle
			cb->بفر_2,		// our buffer
			cb->بفر_سائز_2,		// dimensions of our buffer
			بفر_Coord,		// offset in our buffer
			&WriteRegion))		// region to restore
		واپس FALSE;
	}
    }

    واپس TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
ساکن Console_بفر g_cbOrig = { 0 };
# endif
ساکن Console_بفر g_cbNonTermcap = { 0 };
ساکن Console_بفر g_cbTermcap = { 0 };

حرف g_szOrigTitle[256] = { 0 };
HWND g_hWnd = تہی;	// also used in os_mswin.c
ساکن HICON g_hOrigIconSmall = تہی;
ساکن HICON g_hOrigIcon = تہی;
ساکن HICON g_hVimIcon = تہی;
ساکن BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ساکن BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    اگر (hWnd == تہی)
	واپس FALSE;

    اگر (phIconSmall != تہی)
	*phIconSmall = (HICON)Send_پیغام(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    اگر (phIcon != تہی)
	*phIcon = (HICON)Send_پیغام(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    واپس TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ساکن BOOL
سیٹ_Console_Icon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    اگر (hWnd == تہی)
	واپس FALSE;

    اگر (hIconSmall != تہی)
	Send_پیغام(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    اگر (hIcon != تہی)
	Send_پیغام(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    واپس TRUE;
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
    ساکن خالی
SaveConsoleTitleAndIcon(خالی)
{
    // Save the original title.
    اگر (!GetConsoleTitle(g_szOrigTitle, حجم(g_szOrigTitle)))
	واپس;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = Get_Console_کھڑکی();
    اگر (g_hWnd == تہی)
	واپس;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    اگر (g_hOrigIconSmall == تہی || g_hOrigIcon == تہی)
	واپس;

    // Extract the first icon contained in the Vim executable.
    اگر (
# ifdef FEAT_LIBCALL
	    mch_icon_بھرو((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == تہی)
	g_hVimIcon = نکالو_Icon(تہی, (LPCSTR)exe_name, 0);
    اگر (g_hVimIcon != تہی)
	g_fCanChangeIcon = TRUE;
}

ساکن صحیح g_fWindInitCalled = FALSE;
ساکن صحیح g_f_Termcap_منوال = FALSE;
ساکن CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    ساکن خالی
mch_init_c(خالی)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_بفر_INFO csbi;
# endif
# ifndef __MINGW32__
    خارجی صحیح _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    سیٹ_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    سیٹ_Error_منوال(SEM_FAILCRITICALERRORS);

    _fmode = O_ثنائی;		// we do our own CR-LF translation
    برآمد_flush();

    // Obtain handles for the standard Console I/O devices
    اگر (read_cmd_fd == 0)
	g_h_Con_اندر =  GetStdHandle(STD_داخلہ_HANDLE);
    ورنہ
	create_conin();
    g_h_Con_برآمد = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    Save_Console_بفر(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.w_اوصاف;
# ورنہ
    // Get current text attributes
    Get_Console_Screen_بفر_Info(g_h_Con_برآمد, &csbi);
    g_attrCurrent = g_attrDefault = csbi.w_اوصاف;
# endif
    اگر (cterm_معمول_fg_color == 0)
	cterm_معمول_fg_color = (g_attrCurrent & 0xf) + 1;
    اگر (cterm_معمول_bg_color == 0)
	cterm_معمول_bg_color = ((g_attrCurrent >> 4) & 0xf) + 1;

    // Fg and Bg color index number at startup
    g_color_اشاریہ_fg = g_attrDefault & 0xf;
    g_color_اشاریہ_bg = (g_attrDefault >> 4) & 0xf;

    // set termcap codes to current text attributes
    update_tcap(g_attrCurrent);

    GetConsoleCursorInfo(g_h_Con_برآمد, &g_cci);
    Get_Console_منوال(g_h_Con_اندر,  &g_cmodein);
    Get_Console_منوال(g_h_Con_برآمد, &g_cmodeout);

    SaveConsoleTitleAndIcon();
    /*
     * Set both the small and big icons of the console window to Vim's icon.
     * Note that Vim presently only has one size of icon (32x32), but it
     * automatically gets scaled down to 16x16 when setting the small icon.
     */
    اگر (g_fCanChangeIcon)
	سیٹ_Console_Icon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    اگر (use_alternate_screen_بفر)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    اگر (fdDump)
    {
	time_t t;

	وقت(&t);
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
    ساکن خالی
mch_exit_c(صحیح r)
{
    // Copy flag since stoptermcap() will clear the flag.
    صحیح f_Termcap_منوال = g_f_Termcap_منوال;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    اگر (!f_Termcap_منوال &&  use_alternate_screen_بفر)
	vtp_printf("\033[?1049l");

    اگر (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    اگر (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	اگر (g_fCanChangeIcon && !g_fForceExit)
	    سیٹ_Console_Icon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	اگر (fdDump)
	{
	    time_t t;

	    وقت(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = تہی;
# endif
    }

    سیٹ_Console_Cursor_Info(g_h_Con_برآمد, &g_cci);
    سیٹ_Console_منوال(g_h_Con_اندر,  g_cmodein | ENABLE_EXTENDED_FLAGS);
    سیٹ_Console_منوال(g_h_Con_برآمد, g_cmodeout);

# ifdef DYNAMIC_GETTEXT
    dyn_libintl_انتہا();
# endif

    باہر_نکلو(r);
}
#اگر_ختم // !FEAT_GUI_MSWIN || VIMDLL

    خالی
mch_init(خالی)
{
#اگر_تعریف_شدہ VIMDLL
    اگر (gui.starting)
	mch_init_g();
    ورنہ
	mch_init_c();
#وگرنہ_اگر defined(FEAT_GUI_MSWIN)
    mch_init_g();
#ورنہ
    mch_init_c();
#اگر_ختم
}

    خالی
mch_exit(صحیح r)
{
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#اگر_ختم

#اگر_تعریف_شدہ VIMDLL
    اگر (vimrun_راستہ_allocated)
	vim_free(vimrun_راستہ.string);

    اگر (gui.اندر_use || gui.starting)
	mch_exit_g(r);
    ورنہ
	mch_exit_c(r);
#وگرنہ_اگر defined(FEAT_GUI_MSWIN)
    اگر (vimrun_راستہ_allocated)
	vim_free(vimrun_راستہ.string);
    mch_exit_g(r);
#ورنہ
    mch_exit_c(r);
#اگر_ختم
}

/*
 * Do we have an interactive window?
 */
    صحیح
mch_check_win(
    صحیح argc غیر_مستعمل,
    حرف **argv غیر_مستعمل)
{
    mch_get_exe_name();

#اگر defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    واپس OK;	    // GUI always has a tty
#ورنہ
# ifdef VIMDLL
    اگر (gui.اندر_use)
	واپس OK;
# endif
    اگر (isatty(1))
	واپس OK;
    واپس FAIL;
#اگر_ختم
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    خالی
fname_case(
    char_u	*name,
    صحیح		len)
{
    WCHAR   *p;
    WCHAR   buf[MAX_راستہ_2 + 1];

    اگر (*name == NUL)
	واپس;

    slash_adjust(name);

    p = enc_مقصد_utf16(name, تہی);
    اگر (p == تہی)
	واپس;

    اگر (Get_Long_راستہ_Name_W(p, buf, MAX_راستہ_2))
    {
	char_u	*q = utf16_مقصد_enc(buf, تہی);

	اگر (q != تہی)
	{
	    اگر (len > 0)
		vim_strncpy(name, q, len - 1);
	    ورنہ
	    {
		size_t  namelen = STRLEN(name);

		اگر (namelen >= STRLEN(q))
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
    صحیح
mch_get_user_name(
    char_u  *s,
    صحیح	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_لمبائی(wszUserName);

    اگر (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_مقصد_enc(wszUserName, تہی);

	اگر (p != تہی)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    واپس OK;
	}
    }
    s[0] = NUL;
    واپس FAIL;
}


/*
 * Insert host name in s[len].
 */
    خالی
mch_get_host_name(
    char_u	*s,
    صحیح		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_لمبائی(wszHostName);

    اگر (!GetComputerNameW(wszHostName, &wcch))
	واپس;

    char_u  *p = utf16_مقصد_enc(wszHostName, تہی);
    اگر (p == تہی)
	واپس;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    طویل
mch_get_pid(خالی)
{
    واپس (طویل)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    صحیح
mch_process_running(طویل pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   حالت_2 = 0;
    صحیح	    ret = FALSE;

    اگر (hProcess == تہی)
	واپس FALSE;  // might not have access
    اگر (Get_Exit_رمز_Process(hProcess, &حالت_2))
	ret = حالت_2 == STILL_فعال;
    CloseHandle(hProcess);
    واپس ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    صحیح
mch_dirname(
    char_u	*buf,
    صحیح		len)
{
    WCHAR   wbuf[MAX_راستہ_2 + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    اگر (GetCurrentDirectoryW(MAX_راستہ_2, wbuf) == 0)
	واپس FAIL;

    WCHAR   wcbuf[MAX_راستہ_2 + 1];
    char_u  *p = تہی;

    اگر (Get_Long_راستہ_Name_W(wbuf, wcbuf, MAX_راستہ_2) != 0)
    {
	p = utf16_مقصد_enc(wcbuf, تہی);
	اگر (p != تہی)
	{
	    اگر (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    اگر (p == تہی)
	p = utf16_مقصد_enc(wbuf, تہی);

    اگر (p == تہی)
	واپس FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    واپس OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    طویل
mch_getperm(char_u *name)
{
    stat_T	st;

    واپس (mch_stat((حرف *)name, &st) == 0)
	? (طویل)(غیرعلامتی مختصر)st.st_منوال
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
mch_setperm(char_u *name, طویل perm)
{
    طویل	n;
    WCHAR	*p;

    p = enc_مقصد_utf16(name, تہی);
    اگر (p == تہی)
	واپس FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    اگر (n == -1)
	واپس FAIL;

    win32_سیٹ_archive(name);

    واپس OK;
}

/*
 * Set hidden flag for "name".
 */
    خالی
mch_hide(char_u *name)
{
    صحیح attrs = win32_getattrs(name);
    اگر (attrs == -1)
	واپس;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    صحیح
mch_ishidden(char_u *name)
{
    صحیح f = win32_getattrs(name);

    اگر (f == -1)
	واپس FALSE;		    // file does not exist at all

    واپس (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    صحیح
mch_isdir(char_u *name)
{
    صحیح f = win32_getattrs(name);

    اگر (f == -1)
	واپس FALSE;		    // file does not exist at all

    واپس (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    صحیح
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_مقصد_utf16(name, تہی);
    اگر (p == تہی)
	واپس FALSE;
    attr = Get_File_اوصاف_W(p);
    اگر (attr == INVALID_FILE_اوصاف)
	error = Get_آخری_Error();
    vim_free(p);

    اگر (attr == INVALID_FILE_اوصاف)
	واپس error == ERROR_FILE_NOT_ملا || error == ERROR_راستہ_NOT_ملا;
    واپس (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    صحیح
mch_isrealdir(char_u *name)
{
    واپس mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    صحیح
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    صحیح	    retval;

    p = enc_مقصد_utf16(name, تہی);
    اگر (p == تہی)
	واپس -1;
    retval = _wmkdir(p);
    vim_free(p);
    واپس retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    صحیح
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    صحیح	    retval;

    p = enc_مقصد_utf16(name, تہی);
    اگر (p == تہی)
	واپس -1;
    retval = _wrmdir(p);
    vim_free(p);
    واپس retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    صحیح
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    واپس win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_نمبر_کا_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    صحیح
mch_is_symbolic_link(char_u *name)
{
    HANDLE		h_ڈھونڈو;
    صحیح			res = FALSE;
    DWORD		fileFlags = 0, reparse_لیبل = 0;
    WCHAR		*wn;
    WIN32_ڈھونڈو_DATAW	ڈھونڈو_ڈیٹا_W;

    wn = enc_مقصد_utf16(name, تہی);
    اگر (wn == تہی)
	واپس FALSE;

    h_ڈھونڈو = ڈھونڈو_پہلا_File_W(wn, &ڈھونڈو_ڈیٹا_W);
    vim_free(wn);
    اگر (h_ڈھونڈو != INVALID_HANDLE_قدر)
    {
	fileFlags = ڈھونڈو_ڈیٹا_W.dw_File_اوصاف;
	reparse_لیبل = ڈھونڈو_ڈیٹا_W.dwReserved0;
	ڈھونڈو_Close(h_ڈھونڈو);
    }

    اگر ((fileFlags & FILE_ATTRIBUTE_REPARSE_نقطہ)
	    && (reparse_لیبل == IO_REPARSE_لیبل_SYMLINK
		|| reparse_لیبل == IO_REPARSE_لیبل_MOUNT_نقطہ))
	res = TRUE;

    واپس res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    صحیح
mch_is_linked(char_u *fname)
{
    اگر (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	واپس TRUE;
    واپس FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    صحیح
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    صحیح		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_مقصد_utf16(fname, تہی);
    اگر (wn == تہی)
	واپس FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    تہی,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    تہی);		// handle to template file
    vim_free(wn);

    اگر (hFile == INVALID_HANDLE_قدر)
	واپس FILEINFO_READ_FAIL;

    اگر (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    ورنہ
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    واپس res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    ساکن صحیح
win32_getattrs(char_u *name)
{
    صحیح		attr;
    WCHAR	*p;

    p = enc_مقصد_utf16(name, تہی);
    اگر (p == تہی)
	واپس INVALID_FILE_اوصاف;

    attr = Get_File_اوصاف_W(p);
    vim_free(p);

    واپس attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    ساکن صحیح
win32_setattrs(char_u *name, صحیح attrs)
{
    صحیح	    res;
    WCHAR   *p;

    p = enc_مقصد_utf16(name, تہی);
    اگر (p == تہی)
	واپس -1;

    res = سیٹ_File_اوصاف_W(p, attrs);
    vim_free(p);

    واپس res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    ساکن صحیح
win32_سیٹ_archive(char_u *name)
{
    صحیح attrs = win32_getattrs(name);
    اگر (attrs == -1)
	واپس -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    واپس win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    صحیح
mch_writable(char_u *name)
{
    صحیح attrs = win32_getattrs(name);

    واپس (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    صحیح
mch_can_exe(char_u *name, char_u **راستہ, صحیح use_راستہ غیر_مستعمل)
{
    واپس executable_exists((حرف *)name, STRLEN(name), راستہ, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    صحیح
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    صحیح		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    اگر (STRNCMP(name, "\\\\.\\", 4) == 0)
	واپس گرہ_WRITABLE;

    wn = enc_مقصد_utf16(name, تہی);
    اگر (wn == تہی)
	واپس گرہ_معمول;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    تہی,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    تہی);		    // handle to template file
    vim_free(wn);
    اگر (hFile == INVALID_HANDLE_قدر)
	واپس گرہ_معمول;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    اگر (type == FILE_TYPE_CHAR)
	واپس گرہ_WRITABLE;
    اگر (type == FILE_TYPE_DISK)
	واپس گرہ_معمول;
    واپس گرہ_OTHER;
}

#اگر_تعریف_شدہ HAVE_ACL
ساخت my_acl
{
    PSECURITY_DESCRIPTOR    pSecurityDescriptor;
    PSID		    pSidOwner;
    PSID		    p_Sid_گروہ;
    PACL		    pDacl;
    PACL		    pSacl;
};
#اگر_ختم

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname)
{
#اگر_غیر_تعریف_شدہ HAVE_ACL
    واپس (vim_acl_T)تہی;
#ورنہ
    ساخت my_acl   *p = تہی;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(ساخت my_acl);
    اگر (p != تہی)
    {
	WCHAR	*wn;

	wn = enc_مقصد_utf16(fname, تہی);
	اگر (wn == تہی)
	{
	    vim_free(p);
	    واپس تہی;
	}

	// Try to retrieve the entire security descriptor.
	err = GetNamedSecurityInfoW(
		wn,			// Abstract filename
		SE_FILE_OBJECT,		// File Object
		OWNER_SECURITY_INFORMATION |
		گروہ_SECURITY_INFORMATION |
		DACL_SECURITY_INFORMATION |
		SACL_SECURITY_INFORMATION,
		&p->pSidOwner,		// Ownership information.
		&p->p_Sid_گروہ,		// Group membership.
		&p->pDacl,		// Discretionary information.
		&p->pSacl,		// For auditing purposes.
		&p->pSecurityDescriptor);
	اگر (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (خالی)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    تہی,
		    تہی,
		    &p->pDacl,
		    تہی,
		    &p->pSecurityDescriptor);
	}
	اگر (p->pSecurityDescriptor == تہی)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = تہی;
	}
	vim_free(wn);
    }

    واپس (vim_acl_T)p;
#اگر_ختم
}

#اگر_تعریف_شدہ HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    ساکن BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_سائز_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_گنتی = 0;
    GetAclInformation(acl, &acl_info, حجم(acl_info), Acl_سائز_Information);
    برائے (i = 0; i < acl_info.Ace_گنتی; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	اگر (ace->سرخی_2.AceFlags & INHERITED_ACE)
	    واپس TRUE;
    }
    واپس FALSE;
}
#اگر_ختم

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    خالی
mch_سیٹ_acl(char_u *fname, vim_acl_T acl)
{
#اگر_تعریف_شدہ HAVE_ACL
    ساخت my_acl   *p = (ساخت my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    اگر (p == تہی)
	واپس;

    wn = enc_مقصد_utf16(fname, تہی);
    اگر (wn == تہی)
	واپس;

    // Set security flags
    اگر (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    اگر (p->p_Sid_گروہ)
	sec_info |= گروہ_SECURITY_INFORMATION;
    اگر (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	اگر (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    اگر (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (خالی)سیٹ_Named_Security_Info_W(
	    wn,			// Abstract filename
	    SE_FILE_OBJECT,	// File Object
	    sec_info,
	    p->pSidOwner,	// Ownership information.
	    p->p_Sid_گروہ,	// Group membership.
	    p->pDacl,		// Discretionary information.
	    p->pSacl		// For auditing purposes.
	    );
    vim_free(wn);
#اگر_ختم
}

    خالی
mch_free_acl(vim_acl_T acl)
{
#اگر_تعریف_شدہ HAVE_ACL
    ساخت my_acl   *p = (ساخت my_acl *)acl;

    اگر (p != تہی)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#اگر_ختم
}

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    ساکن BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    داخلہ_ریکارڈ ir;
    DWORD برآمد;

    انتخاب (dwCtrlType)
    {
    حالت CTRL_C_EVENT:
	اگر (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	واپس TRUE;

    حالت CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = کلید_EVENT;
	ir.Event.کلید_Event_2.b_کلید_نیچے_کو = TRUE;
	ir.Event.کلید_Event_2.w_Repeat_گنتی = 1;
	ir.Event.کلید_Event_2.w_Virtual_کلید_رمز = VK_CANCEL;
	ir.Event.کلید_Event_2.w_Virtual_جائزہ_رمز = 0;
	ir.Event.کلید_Event_2.dw_Control_کلید_State = 0;
	ir.Event.کلید_Event_2.uChar.UnicodeChar = 0;
	Write_Console_داخلہ(g_h_Con_اندر, &ir, 1, &برآمد);
	واپس TRUE;

    // fatal events: shut down gracefully
    حالت CTRL_CLOSE_EVENT:
    حالت CTRL_LOGOFF_EVENT:
    حالت CTRL_SHUTDOWN_EVENT:
	windgoto((صحیح)صفیں - 1, cmdline_ستون_off);
	g_fForceExit = TRUE;

	vim_snprintf((حرف *)IObuff, IOSIZE, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));
# ifdef DEBUG
	OutputDebugString(IObuff);
# endif

	preserve_exit();	// output IObuff, preserve files and exit

	واپس TRUE;		// not reached

    طےشدہ:
	واپس FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    خالی
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    اگر (gui.اندر_use)
	واپس;
# endif
    Get_Console_منوال(g_h_Con_اندر, &cmodein);
    Get_Console_منوال(g_h_Con_برآمد, &cmodeout);
    اگر (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_سطر_داخلہ | ENABLE_PROCESSED_داخلہ |
		     ENABLE_ECHO_داخلہ);
	اگر (g_f_Mouse_فعال)
	{
	    cmodein |= ENABLE_MOUSE_داخلہ;
	    cmodein &= ~ENABLE_تیز_EDIT_منوال;
	}
	ورنہ
	{
	    cmodein |= g_cmodein & ENABLE_تیز_EDIT_منوال;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# ورنہ
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    ورنہ // cooked
    {
	cmodein |= (ENABLE_سطر_داخلہ | ENABLE_PROCESSED_داخلہ |
		    ENABLE_ECHO_داخلہ);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    سیٹ_Console_منوال(g_h_Con_اندر, cmodein | ENABLE_EXTENDED_FLAGS);
    سیٹ_Console_منوال(g_h_Con_برآمد, cmodeout);
    سیٹ_Console_Ctrl_Handler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    اگر (fdDump)
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
    صحیح
mch_get_shellsize(خالی)
{
    CONSOLE_SCREEN_بفر_INFO csbi;

# ifdef VIMDLL
    اگر (gui.اندر_use)
	واپس OK;
# endif
    اگر (!g_f_Termcap_منوال && g_cbTermcap.IsValid)
    {
	/*
	 * For some reason, we are trying to get the screen dimensions
	 * even though we are not in termcap mode.  The 'Rows' and 'Columns'
	 * variables are really intended to mean the size of Vim screen
	 * while in termcap mode.
	 */
	صفیں = g_cbTermcap.Info.dw_سائز.Y;
	Columns = g_cbTermcap.Info.dw_سائز.X;
    }
    ورنہ اگر (Get_Console_Screen_بفر_Info(g_h_Con_برآمد, &csbi))
    {
	صفیں = csbi.sr_کھڑکی.Bottom - csbi.sr_کھڑکی.چوٹی_2 + 1;
	Columns = csbi.sr_کھڑکی.دایاں_3 - csbi.sr_کھڑکی.بایاں_3 + 1;
    }
    ورنہ
    {
	صفیں = 25;
	Columns = 80;
    }
    واپس OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    ساکن خالی
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    اگر (use_alternate_screen_بفر)
	واپس;

    اگر (!سیٹ_Console_Screen_بفر_سائز(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	اگر (fdDump)
	{
	    fprintf(fdDump, "SetConsoleScreenBufferSize failed: %lx\n",
		    Get_آخری_Error());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Resize console window size to 'srWindowRect'
 */
    ساکن خالی
Resize_کھڑکی(
    HANDLE     hConsole,
    SMALL_RECT sr_کھڑکی_Rect)
{
    اگر (!سیٹ_Console_کھڑکی_Info(hConsole, TRUE, &sr_کھڑکی_Rect))
    {
# ifdef MCH_WRITE_DUMP
	اگر (fdDump)
	{
	    fprintf(fdDump, "SetConsoleWindowInfo failed: %lx\n",
		    Get_آخری_Error());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Set a console window to `xSize' * `ySize'
 */
    ساکن خالی
Resize_Con_Buf_And_کھڑکی(
    HANDLE  hConsole,
    صحیح	    x_سائز,
    صحیح	    y_سائز_2)
{
    CONSOLE_SCREEN_بفر_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    sr_کھڑکی_Rect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    ساکن صحیح	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    اگر (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", x_سائز, y_سائز_2);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = Get_سب_سے_بڑا_Console_کھڑکی_سائز(hConsole);

    // define the new console window size and scroll position
    sr_کھڑکی_Rect.بایاں_3 = sr_کھڑکی_Rect.چوٹی_2 = (SHORT) 0;
    sr_کھڑکی_Rect.دایاں_3 =  (SHORT) (min(x_سائز, coordScreen.X) - 1);
    sr_کھڑکی_Rect.Bottom = (SHORT) (min(y_سائز_2, coordScreen.Y) - 1);

    اگر (Get_Console_Screen_بفر_Info(g_h_Con_برآمد, &csbi))
    {
	صحیح sx, sy;

	sx = csbi.sr_کھڑکی.دایاں_3 - csbi.sr_کھڑکی.بایاں_3 + 1;
	sy = csbi.sr_کھڑکی.Bottom - csbi.sr_کھڑکی.چوٹی_2 + 1;
	اگر (sy < y_سائز_2 || sx < x_سائز)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    اگر (sy < y_سائز_2)
		coordScreen.Y = y_سائز_2;
	    ورنہ
		coordScreen.Y = sy;
	    اگر (sx < x_سائز)
		coordScreen.X = x_سائز;
	    ورنہ
		coordScreen.X = sx;
	    سیٹ_Console_Screen_بفر_سائز(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = x_سائز;
    coordScreen.Y = y_سائز_2;

    // In the new console call API, only the first time in reverse order
    اگر (!vtp_working || resized)
    {
	Resize_کھڑکی(hConsole, sr_کھڑکی_Rect);
	ResizeConBuf(hConsole, coordScreen);
    }
    ورنہ
    {
	// Workaround for a Windows 10 bug
	cursor.X = sr_کھڑکی_Rect.بایاں_3;
	cursor.Y = sr_کھڑکی_Rect.چوٹی_2;
	سیٹ_Console_Cursor_Position(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	Resize_کھڑکی(hConsole, sr_کھڑکی_Rect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    خالی
mch_سیٹ_shellsize(خالی)
{
    COORD coordScreen;

# ifdef VIMDLL
    اگر (gui.اندر_use)
	واپس;
# endif
    // Don't change window size while still starting up
    اگر (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	واپس;
    }

    اگر (رکن_console)
    {
	coordScreen = Get_سب_سے_بڑا_Console_کھڑکی_سائز(g_h_Con_برآمد);

	// Clamp Rows and Columns to reasonable values
	اگر (صفیں > coordScreen.Y)
	    صفیں = coordScreen.Y;
	اگر (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	Resize_Con_Buf_And_کھڑکی(g_h_Con_برآمد, Columns, صفیں);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    خالی
mch_new_shellsize(خالی)
{
# ifdef VIMDLL
    اگر (gui.اندر_use)
	واپس;
# endif
    سیٹ_scroll_region(0, 0, Columns - 1, صفیں - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    خالی
mch_سیٹ_winsize_now(خالی)
{
    اگر (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_سیٹ_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# اگر defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    خالی
mch_calc_cell_سائز(ساخت cellsize *cs_برآمد)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_برآمد->cs_xpixel = -1;
    cs_برآمد->cs_ypixel = -1;

    اگر (g_h_Con_برآمد == INVALID_HANDLE_قدر)
	واپس;

    cfi.cb_سائز = حجم(cfi);
    اگر (GetCurrentConsoleFontEx(g_h_Con_برآمد, FALSE, &cfi)
	    && cfi.dw_Font_سائز.X > 0 && cfi.dw_Font_سائز.Y > 0)
    {
	cs_برآمد->cs_xpixel = cfi.dw_Font_سائز.X;
	cs_برآمد->cs_ypixel = cfi.dw_Font_سائز.Y;
	واپس;
    }

    Get_Console_منوال(g_h_Con_اندر, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_سطر_داخلہ | ENABLE_ECHO_داخلہ
	    | ENABLE_PROCESSED_داخلہ);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_داخلہ;

    اگر (سیٹ_Console_منوال(g_h_Con_اندر, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	سیٹ_Console_منوال(g_h_Con_اندر, cmodeold);
    }
}
# endif

#اگر_ختم

    ساکن BOOL
vim_create_process(
    حرف		*cmd,
    BOOL		inherit_handles,
    DWORD		flags,
    STARTUPINFO		*si,
    PROCESS_INFORMATION *pi,
    LPVOID		*env,
    حرف		*cwd)
{
    BOOL	ret = FALSE;
    WCHAR	*wcmd, *wcwd = تہی;

    wcmd = enc_مقصد_utf16((char_u *)cmd, تہی);
    اگر (wcmd == تہی)
	واپس FALSE;
    اگر (cwd != تہی)
    {
	wcwd = enc_مقصد_utf16((char_u *)cwd, تہی);
	اگر (wcwd == تہی)
	    جاؤ theend;
    }

    ret = CreateProcessW(
	    تہی,		// Executable name
	    wcmd,		// Command to execute
	    تہی,		// Process security attributes
	    تہی,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    واپس ret;
}


    ساکن HINSTANCE
vim_shell_execute(
    حرف *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_مقصد_utf16((char_u *)cmd, تہی);
    اگر (wcmd == تہی)
	واپس (HINSTANCE) 0;

    ret = ShellExecuteW(تہی, تہی, wcmd, تہی, تہی, n_show_cmd);
    vim_free(wcmd);
    واپس ret;
}


#اگر defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    ساکن صحیح
mch_system_classic(حرف *cmd, صحیح options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = حجم(si);
    si.lpReserved = تہی;
    si.lpDesktop = تہی;
    si.lpTitle = تہی;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    اگر (options & SHELL_DOOUT)
	si.w_Show_کھڑکی = SW_SHOWMINNOACTIVE;
    ورنہ
	si.w_Show_کھڑکی = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = تہی;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_منوال |	CREATE_NEW_CONSOLE,
	    &si, &pi, تہی, تہی);

    // Wait for the command to terminate before continuing
    {
	صحیح	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	برائے (;;)
	{
	    پیغام_3	پیغام;

	    اگر (Peek_پیغام_W(&پیغام, (HWND)تہی, 0, 0, PM_REMOVE))
	    {
		Translate_پیغام(&پیغام);
		Dispatch_پیغام_W(&پیغام);
		delay = 1;
		جاری;
	    }
	    اگر (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		توڑو;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    اگر (delay < 50)
		delay += 10;
	}

	// Get the command exit code
	Get_Exit_رمز_Process(pi.hProcess, &ret);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // Try to get input focus back.  Doesn't always work though.
    Post_پیغام(hwnd, WM_SETFOCUS, 0, 0);
    // To increase the chances that WM_SETFOCUS will work, run the message loop
    // here.  In addition, it prevents problems caused by delayed WM_SETFOCUS
    // processing.
    gui_mch_update();

    واپس ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    ساکن غیرعلامتی صحیح __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_h_بچہ_Std_اندر_Wr = param;
    linenr_T	    lnum = curbuf->b_op_آغاز.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    صحیح		    written = 0;

    برائے (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	اگر (l == 0)
	    len = 0;
	ورنہ اگر (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_h_بچہ_Std_اندر_Wr, "", 1, &len, تہی);
	}
	ورنہ
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_h_بچہ_Std_اندر_Wr, (حرف *)lp + written,
		      s == تہی ? l : (DWORD)(s - (lp + written)),
		      &len, تہی);
	}
	اگر (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    اگر (lnum != curbuf->b_op_انتہا.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_سطر_گنتی
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_h_بچہ_Std_اندر_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, تہی);
	    }

	    ++lnum;
	    اگر (lnum > curbuf->b_op_انتہا.lnum)
		توڑو;

	    lp = ml_get(lnum);
	    written = 0;
	}
	ورنہ اگر (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_h_بچہ_Std_اندر_Wr);
    واپس 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    ساکن خالی
dump_pipe(صحیح	    options,
	  HANDLE    g_h_بچہ_Std_برآمد_Rd,
	  garray_T  *ga,
	  char_u    بفر[],
	  DWORD	    *بفر_off)
{
    DWORD	دستیاب_بائٹیں = 0;
    DWORD	i;
    صحیح		ret;
    DWORD	len;
    DWORD	مقصد_Read;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_h_بچہ_Std_برآمد_Rd, // pipe to query
			تہی,		    // optional buffer
			0,		    // buffer size
			تہی,		    // number of read bytes
			&دستیاب_بائٹیں,    // available bytes total
			تہی);		    // byteLeft

    // We got real data in the pipe, read it
    جب (ret != 0 && دستیاب_بائٹیں > 0)
    {
	مقصد_Read = (DWORD)(BUFLEN - *بفر_off);
	مقصد_Read = دستیاب_بائٹیں < مقصد_Read ? دستیاب_بائٹیں : مقصد_Read;
	ReadFile(g_h_بچہ_Std_برآمد_Rd, بفر + *بفر_off, مقصد_Read , &len, تہی);

	// If we haven't read anything, there is a problem
	اگر (len == 0)
	    توڑو;

	دستیاب_بائٹیں -= len;

	اگر (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    برائے (i = 0; i < len; ++i)
	    {
		اگر (بفر[i] == NL)
		    append_ga_سطر(ga);
		ورنہ اگر (بفر[i] == NUL)
		    ga_append(ga, NL);
		ورنہ
		    ga_append(ga, بفر[i]);
	    }
	}
	ورنہ اگر (has_mbyte)
	{
	    صحیح		l;
	    صحیح		c;
	    char_u	*p;

	    len += *بفر_off;
	    بفر[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    برائے (p = بفر; p < بفر + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		اگر (l == 0)
		    l = 1;  // NUL byte?
		ورنہ اگر (MB_BYTE2LEN(*p) != l)
		    توڑو;
	    }
	    اگر (p == بفر)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		اگر (len >= 12)
		    ++p;
		ورنہ
		{
		    *بفر_off = len;
		    واپس;
		}
	    }
	    c = *p;
	    *p = NUL;
	    پیغام_puts((حرف *)بفر);
	    اگر (p < بفر + len)
	    {
		*p = c;
		*بفر_off = (DWORD)((بفر + len) - p);
		mch_memmove(بفر, p, *بفر_off);
		واپس;
	    }
	    *بفر_off = 0;
	}
	ورنہ
	{
	    بفر[len] = NUL;
	    پیغام_puts((حرف *)بفر);
	}

	windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);
	cursor_on();
	برآمد_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    ساکن صحیح
mch_system_piped(حرف *cmd, صحیح options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_h_بچہ_Std_اندر_Rd = تہی;
    HANDLE g_h_بچہ_Std_اندر_Wr = تہی;
    HANDLE g_h_بچہ_Std_برآمد_Rd = تہی;
    HANDLE g_h_بچہ_Std_برآمد_Wr = تہی;

    char_u	بفر[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    صحیح		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    صحیح		noread_cnt = 0;
    garray_T	ga;
    صحیح		delay = 1;
    DWORD	بفر_off = 0;	// valid bytes in buffer[]
    حرف	*p = تہی;

    SECURITY_اوصاف saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.n_لمبائی = حجم(SECURITY_اوصاف);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = تہی;

    اگر ( ! CreatePipe(&g_h_بچہ_Std_برآمد_Rd, &g_h_بچہ_Std_برآمد_Wr, &saAttr, 0)
	// Ensure the read handle to the pipe for STDOUT is not inherited.
       || ! سیٹ_Handle_Information(g_h_بچہ_Std_برآمد_Rd, HANDLE_FLAG_INHERIT, 0)
	// Create a pipe for the child process's STDIN.
       || ! CreatePipe(&g_h_بچہ_Std_اندر_Rd, &g_h_بچہ_Std_اندر_Wr, &saAttr, 0)
	// Ensure the write handle to the pipe for STDIN is not inherited.
       || ! سیٹ_Handle_Information(g_h_بچہ_Std_اندر_Wr, HANDLE_FLAG_INHERIT, 0) )
    {
	CloseHandle(g_h_بچہ_Std_اندر_Rd);
	CloseHandle(g_h_بچہ_Std_اندر_Wr);
	CloseHandle(g_h_بچہ_Std_برآمد_Rd);
	CloseHandle(g_h_بچہ_Std_برآمد_Wr);
	پیغام_puts(_("\nCannot create pipes\n"));
    }

    si.cb = حجم(si);
    si.lpReserved = تہی;
    si.lpDesktop = تہی;
    si.lpTitle = تہی;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_h_بچہ_Std_برآمد_Wr;
    si.hStdOutput = g_h_بچہ_Std_برآمد_Wr;
    si.h_Std_داخلہ = g_h_بچہ_Std_اندر_Rd;
    si.w_Show_کھڑکی = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = تہی;

    اگر (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    اگر (cmd != تہی)
    {
	p = (حرف *)vim_strsave((char_u *)cmd);
	اگر (p != تہی)
	    unescape_shellxquote((char_u *)p, p_sxe);
	ورنہ
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_منوال,
						&si, &pi, تہی, تہی);

    اگر (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_h_بچہ_Std_اندر_Rd);
    CloseHandle(g_h_بچہ_Std_برآمد_Wr);

    اگر (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(تہی,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_h_بچہ_Std_اندر_Wr,  // parameter
			0,		 // creation flag, start immediately
			تہی);		    // we don't care about thread id
	CloseHandle(thread);
	g_h_بچہ_Std_اندر_Wr = تہی;
    }

    // Keep updating the window while waiting for the shell to finish.
    برائے (;;)
    {
	پیغام_3	پیغام;

	اگر (Peek_پیغام_W(&پیغام, (HWND)تہی, 0, 0, PM_REMOVE))
	{
	    Translate_پیغام(&پیغام);
	    Dispatch_پیغام_W(&پیغام);
	}

	// write pipe information in the window
	اگر ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.اندر_use
# endif
	    )
	{
	    len = 0;
	    اگر (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.اندر_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		اگر (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		اگر (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    اگر (len == 1 && cmd != تہی)
		    {
			اگر (ta_buf[ta_len] == Ctrl_C)
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
		    اگر ((len == 1 || len == 4 ) && cmd != تہی)
		    {
			اگر (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_h_بچہ_Std_اندر_Wr);
			    g_h_بچہ_Std_اندر_Wr = تہی;
			    len = 0;
			}
		    }

		    len = رکن_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    برائے (i = ta_len; i < ta_len + len; ++i)
		    {
			اگر (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    پیغام_putchar(ta_buf[i]);
			ورنہ اگر (has_mbyte)
			{
			    صحیح l = (*mb_ptr2len)(ta_buf + i);

			    پیغام_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			ورنہ
			    پیغام_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);
		    برآمد_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    اگر (options & SHELL_WRITE)
			ta_len = 0;
		    ورنہ اگر (g_h_بچہ_Std_اندر_Wr != تہی)
		    {
			WriteFile(g_h_بچہ_Std_اندر_Wr, (حرف*)ta_buf,
				    1, &len, تہی);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			اگر (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	اگر (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	اگر (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_h_بچہ_Std_برآمد_Rd, &ga, بفر, &بفر_off);
	    توڑو;
	}

	++noread_cnt;
	dump_pipe(options, g_h_بچہ_Std_برآمد_Rd, &ga, بفر, &بفر_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	اگر (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_h_بچہ_Std_برآمد_Rd);
    اگر (g_h_بچہ_Std_اندر_Wr != تہی)
	CloseHandle(g_h_بچہ_Std_اندر_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    Get_Exit_رمز_Process(pi.hProcess, &ret);

    اگر (options & SHELL_READ)
    {
	اگر (ga.ga_len > 0)
	{
	    append_ga_سطر(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	ورنہ
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    واپس ret;
}

    ساکن صحیح
mch_system_g(حرف *cmd, صحیح options)
{
    // if we can pipe and the shelltemp option is off
    اگر (!p_stmp)
	واپس mch_system_piped(cmd, options);
    ورنہ
	واپس mch_system_classic(cmd, options);
}
#اگر_ختم

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ساکن صحیح
mch_system_c(حرف *cmd, صحیح options غیر_مستعمل)
{
    صحیح		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    اگر (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	اگر (buf == تہی)
	    واپس -1;
	vim_snprintf((حرف *)buf, len, "(%s)", cmd);
	wcmd = enc_مقصد_utf16(buf, تہی);
	آزاد(buf);
    }
    ورنہ
	wcmd = enc_مقصد_utf16((char_u *)cmd, تہی);

    اگر (wcmd == تہی)
	واپس -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    واپس ret;
}

#اگر_ختم

    ساکن صحیح
mch_system(حرف *cmd, صحیح options)
{
#اگر_تعریف_شدہ VIMDLL
    اگر (gui.اندر_use || gui.starting)
	واپس mch_system_g(cmd, options);
    ورنہ
	واپس mch_system_c(cmd, options);
#وگرنہ_اگر defined(FEAT_GUI_MSWIN)
    واپس mch_system_g(cmd, options);
#ورنہ
    واپس mch_system_c(cmd, options);
#اگر_ختم
}

#اگر defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ساکن صحیح
mch_call_shell_terminal(
    char_u	*cmd,
    صحیح		options غیر_مستعمل)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = تہی;
    typval_T	argvar[2];
    long_u	cmdlen;
    صحیح		retval = -1;
    buf_T	*buf;
    کام_T	*کام;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    اگر (cmd == تہی)
	cmdlen = STRLEN(p_sh) + 1;
    ورنہ
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    اگر (newcmd == تہی)
	واپس 255;
    اگر (cmd == تہی)
    {
	STRCPY(newcmd, p_sh);
	ch_log(تہی, "starting terminal to run a shell");
    }
    ورنہ
    {
	vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(تہی, "starting terminal for system command '%s'", cmd);
    }

    init_کام_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = رکن_آغاز(argvar, تہی, &opt, رکن_آغاز_SYSTEM);
    اگر (buf == تہی)
    {
	vim_free(newcmd);
	واپس 255;
    }

    کام = رکن_getjob(buf->b_رکن);
    ++کام->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    اگر (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	جب (رکن_use_loop())
	{
	    اگر (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_فعال)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		اگر (terminal_loop(TRUE) == OK)
		    معمول_cmd(&oa, TRUE);
	    }
	    ورنہ
		معمول_cmd(&oa, TRUE);
	}
	retval = کام->jv_exitval;
	ch_log(تہی, "system command finished");

	کام_unref(کام);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_بفر(DOBUF_WIPE, DOBUF_پہلا, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    واپس retval;
}
#اگر_ختم
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    ساکن خالی
restore_env_var(char_u *name, char_u *old_قدر, صحیح must_free)
{
    اگر (old_قدر != تہی)
    {
	vim_setenv(name, old_قدر);
	اگر (must_free)
	    vim_free(old_قدر);
	واپس;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    صحیح
mch_call_shell(
    char_u  *cmd,
    صحیح	    options)	// SHELL_*, see vim.h
{
    صحیح		x = 0;
    صحیح		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    صحیح		must_free;
    char_u	*oldval;

#اگر_تعریف_شدہ FEAT_EVAL
    ch_log(تہی, "executing shell command: %s", cmd);
#اگر_ختم
    // Change the title to reflect that we are in a subshell.
    اگر (GetConsoleTitleW(szShellTitle, ARRAY_لمبائی(szShellTitle) - 4) > 0)
    {
	اگر (cmd == تہی)
	    wcscat(szShellTitle, L" :sh");
	ورنہ
	{
	    WCHAR *wn = enc_مقصد_utf16((char_u *)cmd, تہی);

	    اگر (wn != تہی)
	    {
		wcscat(szShellTitle, L" - !");
		اگر ((wcslen(szShellTitle) + wcslen(wn) <
			    ARRAY_لمبائی(szShellTitle)))
		    wcscat(szShellTitle, wn);
		سیٹ_Console_Title_W(szShellTitle);
		vim_free(wn);
	    }
	}
    }
    // do not execute anything from the current directory by setting the
    // environment variable $NoDefaultCurrentDirectoryInExePath
    oldval = vim_getenv((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    &must_free);
    vim_setenv((char_u *)"NoDefaultCurrentDirectoryInExePath", (char_u *)"1");

    برآمد_flush();

#اگر_تعریف_شدہ MCH_WRITE_DUMP
    اگر (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#اگر_ختم
#اگر defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    اگر (
# ifdef VIMDLL
	    gui.اندر_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != تہی
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	اگر (cmdbase != تہی)
	    // Skip a leading quote and (.
	    جب (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	اگر (cmdbase == تہی || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    واپس x;
	}
    }
#اگر_ختم

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

    اگر (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    اگر (cmd == تہی)
    {
	x = mch_system((حرف *)p_sh, options);
    }
    ورنہ
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = تہی;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	اگر (*cmdbase == '"' )
	    ++cmdbase;
	اگر (*cmdbase == '(')
	    ++cmdbase;

	اگر ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    Zero_حافظہ(&si, حجم(si));
	    si.cb = حجم(si);
	    si.lpReserved = تہی;
	    si.lpDesktop = تہی;
	    si.lpTitle = تہی;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = تہی;

	    cmdbase = skipwhite(cmdbase + 5);
	    اگر ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.w_Show_کھڑکی = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    ورنہ اگر ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_کھڑکی;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_داخلہ = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    تہی,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_معمول,		// File att.
		    تہی);				// Temp file
		si.hStdOutput = si.h_Std_داخلہ;
		si.hStdError = si.h_Std_داخلہ;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    اگر (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		اگر (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		اگر (p > cmdbase && p[-1] == ')'
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
	    اگر (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		اگر (cmd_shell == تہی || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		اگر (subcmd != تہی)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    اگر (newcmd != تہی)
			vim_snprintf((حرف *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    ورنہ
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    اگر (vim_create_process((حرف *)newcmd, FALSE, flags,
			&si, &pi, تہی, تہی))
		x = 0;
	    ورنہ اگر (vim_shell_execute((حرف *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    ورنہ
	    {
		x = -1;
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
# ifdef VIMDLL
		اگر (gui.اندر_use)
# endif
		    emsg(_(e_command_not_ملا));
#اگر_ختم
	    }

	    اگر (newcmd != cmdbase)
		vim_free(newcmd);

	    اگر (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_داخلہ != تہی)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_داخلہ);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	ورنہ
	{
	    cmdlen =
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
		((gui.اندر_use || gui.starting) ?
		    (!s_dont_use_vimrun && p_stmp ?
			vimrun_راستہ.لمبائی : STRLEN(p_sh) + STRLEN(p_shcf))
		    : 0) +
#اگر_ختم
		STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;

	    newcmd = alloc(cmdlen);
	    اگر (newcmd != تہی)
	    {
#اگر defined(FEAT_GUI_MSWIN)
		اگر (
# ifdef VIMDLL
		    (gui.اندر_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    حرف *پیغام = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    حرف *title = _("Vim Warning");
		    WCHAR *wmsg = enc_مقصد_utf16((char_u *)پیغام, تہی);
		    WCHAR *wtitle = enc_مقصد_utf16((char_u *)title, تہی);

		    اگر (wmsg != تہی && wtitle != تہی)
			پیغام_Box_W(تہی, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		اگر (
# ifdef VIMDLL
		    (gui.اندر_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((حرف *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_راستہ.string,
			    (پیغام_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		ورنہ اگر (
# ifdef VIMDLL
			(gui.اندر_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		ورنہ
#اگر_ختم
		    vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s",
							   p_sh, p_shcf, cmd);
		x = mch_system((حرف *)newcmd, options);
		vim_free(newcmd);
	    }
	}
    }

    // Restore original value of NoDefaultCurrentDirectoryInExePath
    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    oldval, must_free);

    اگر (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    اگر (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#اگر defined(FEAT_GUI_MSWIN)
	    && ((gui.اندر_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#اگر_ختم
	    )
    {
	smsg(_("shell returned %d"), x);
	پیغام_putchar('\n');
    }
    resettitle();

    mch_signal(SIGINT, SIG_DFL);
    mch_signal(SIGBREAK, SIG_DFL);
    mch_signal(SIGILL, SIG_DFL);
    mch_signal(SIGFPE, SIG_DFL);
    mch_signal(SIGSEGV, SIG_DFL);
    mch_signal(SIGTERM, SIG_DFL);
    mch_signal(SIGABRT, SIG_DFL);

    واپس x;
}

#اگر defined(FEAT_کام_CHANNEL)
    ساکن HANDLE
کام_io_file_open(
	char_u *fname,
	DWORD dwDesiredAccess,
	DWORD dw_Share_منوال,
	LPSECURITY_اوصاف lp_Security_اوصاف,
	DWORD dwCreationDisposition,
	DWORD dw_Flags_And_اوصاف)
{
    HANDLE h;
    WCHAR *wn;

    wn = enc_مقصد_utf16(fname, تہی);
    اگر (wn == تہی)
	واپس INVALID_HANDLE_قدر;

    h = CreateFileW(wn, dwDesiredAccess, dw_Share_منوال,
	    lp_Security_اوصاف, dwCreationDisposition,
	    dw_Flags_And_اوصاف, تہی);
    vim_free(wn);
    واپس h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    خالی
win32_build_env(dict_T *env, garray_T *gap, صحیح is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != تہی ? env->dv_hashtab.ht_مستعمل : 0;
    LPVOID	بنیاد = Get_Environment_لڑیاں_W();

    // for last \0
    اگر (ga_grow(gap, 1) == FAIL)
	واپس;

    اگر (env != تہی)
    {
	FOR_ALL_HASHTAB_اشیاء(&env->dv_hashtab, hi, todo)
	{
	    اگر (!HASHITEM_خالی_خانے(hi))
	    {
		typval_T *شے = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_مقصد_utf16((char_u *)hi->hi_کلید, تہی);
		WCHAR   *wval = enc_مقصد_utf16(tv_get_string(شے), تہی);
		--todo;
		اگر (wkey != تہی && wval != تہی)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    اگر (ga_grow(gap, (صحیح)(lkey + lval + 2)) == FAIL)
			جاری;
		    برائے (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) = L'=';
		    برائے (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    اگر (بنیاد)
    {
	WCHAR	*p = (WCHAR*) بنیاد;

	// for last \0
	اگر (ga_grow(gap, 1) == FAIL)
	    واپس;

	جب (*p != 0 || *(p + 1) != 0)
	{
	    اگر (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) = *p;
	    p++;
	}
	Free_Environment_لڑیاں_W(بنیاد);
	*((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) = L'\0';
    }

# اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
    {
#  ifdef FEAT_CLIENTSERVER
	char_u	*servername = get_vim_var_str(VV_SEND_SERVER);
	size_t	servername_len = STRLEN(servername);
#  endif
#  ifdef FEAT_TERMINAL
	char_u	*نسخہ_2 = get_vim_var_str(VV_نسخہ);
	size_t	نسخہ_len = STRLEN(نسخہ_2);
#  endif
	// size of "VIM_SERVERNAME=" and value,
	// plus "VIM_TERMINAL=" and value,
	// plus two terminating NULs
	size_t	n = 0
#  ifdef FEAT_CLIENTSERVER
		    + 15 + servername_len
#  endif
#  ifdef FEAT_TERMINAL
		    + 13 + نسخہ_len + 2
#  endif
		    ;

	اگر (ga_grow(gap, (صحیح)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    برائے (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    برائے (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    اگر (is_terminal)
	    {
		برائے (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		برائے (n = 0; n < نسخہ_len; n++)
		    *((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) =
			(WCHAR)نسخہ_2[n];
		*((WCHAR*)gap->ga_ڈیٹا + gap->ga_len++) = L'\0';
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
    ساکن BOOL
create_pipe_pair(HANDLE handles[2])
{
    ساکن LONG		s;
    حرف		name[64];
    SECURITY_اوصاف sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_سائز, 0, 0, تہی);

    اگر (handles[1] == INVALID_HANDLE_قدر)
	واپس FALSE;

    sa.n_لمبائی = حجم(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = تہی;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_معمول, 0);

    اگر (handles[0] == INVALID_HANDLE_قدر)
    {
	CloseHandle(handles[1]);
	واپس FALSE;
    }

    واپس TRUE;
}

#اگر_ختم // FEAT_JOB_CHANNEL

#اگر defined(FEAT_EVAL)
/*
 * Execute "argv" directly without the shell and return the output.
 * Used by system() and systemlist() when the command is a List.
 * "infile" is an optional temp file for stdin input.
 * When "ret_len" is not NULL, set it to the length of the output.
 * Returns the output in allocated memory (or NULL on error).
 * Sets v:shell_error to the exit status.
 */
    char_u *
mch_get_cmd_output_براہ_راست(
    حرف	**argv,
    char_u	*infile,
    صحیح		flags غیر_مستعمل,
    صحیح		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_اوصاف saAttr;
    HANDLE		h_بچہ_Stdout_Rd = INVALID_HANDLE_قدر;
    HANDLE		h_بچہ_Stdout_Wr = INVALID_HANDLE_قدر;
    HANDLE		h_بچہ_Stdin_Rd = INVALID_HANDLE_قدر;
    garray_T		cmd_ga;
    garray_T		برآمد_ga;
    char_u		*بفر = تہی;
    DWORD		exit_رمز = (DWORD)-1;
    صحیح			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    برائے (i = 0; argv[i] != تہی; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	صحیح	has_spaces = FALSE;
	صحیح	j;

	برائے (j = 0; s[j] != NUL; j++)
	    اگر (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		توڑو;
	    }

	اگر (i > 0)
	    ga_append(&cmd_ga, ' ');

	اگر (has_spaces)
	{
	    صحیح	عدد_bs;

	    ga_append(&cmd_ga, '"');
	    برائے (j = 0; arg[j] != NUL; j++)
	    {
		عدد_bs = 0;
		جب (arg[j] == '\\')
		{
		    عدد_bs++;
		    j++;
		}

		اگر (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    جب (عدد_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    توڑو;
		}
		ورنہ اگر (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    جب (عدد_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		ورنہ
		{
		    جب (عدد_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	ورنہ
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&برآمد_ga, 1, 4096);

    saAttr.n_لمبائی = حجم(SECURITY_اوصاف);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = تہی;

    // Create a pipe for the child's stdout.
    اگر (!CreatePipe(&h_بچہ_Stdout_Rd, &h_بچہ_Stdout_Wr, &saAttr, 0)
	    || !سیٹ_Handle_Information(h_بچہ_Stdout_Rd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	جاؤ مکمل;
    }

    // Set up stdin from infile if provided.
    اگر (infile != تہی)
    {
	WCHAR *winfile = enc_مقصد_utf16(infile, تہی);

	اگر (winfile != تہی)
	{
	    h_بچہ_Stdin_Rd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_معمول, تہی);
	    vim_free(winfile);
	}
    }

    Zero_حافظہ(&pi, حجم(pi));
    Zero_حافظہ(&si, حجم(si));
    si.cb = حجم(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.w_Show_کھڑکی = SW_HIDE;
    si.hStdOutput = h_بچہ_Stdout_Wr;
    si.hStdError = h_بچہ_Stdout_Wr;
    si.h_Std_داخلہ = (h_بچہ_Stdin_Rd != INVALID_HANDLE_قدر)
		    ? h_بچہ_Stdin_Rd : INVALID_HANDLE_قدر;

    ch_log(تہی, "directly executing: %s", (حرف *)cmd_ga.ga_ڈیٹا);

    // Create the child process directly, without going through the shell.
    اگر (!vim_create_process((حرف *)cmd_ga.ga_ڈیٹا, TRUE,
		CREATE_DEFAULT_ERROR_منوال | CREATE_NEW_PROCESS_گروہ,
		&si, &pi, تہی, تہی))
    {
	semsg(_(e_invalid_دلیل_str), cmd_ga.ga_ڈیٹا);
	جاؤ مکمل;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(h_بچہ_Stdout_Wr);
    h_بچہ_Stdout_Wr = INVALID_HANDLE_قدر;
    اگر (h_بچہ_Stdin_Rd != INVALID_HANDLE_قدر)
    {
	CloseHandle(h_بچہ_Stdin_Rd);
	h_بچہ_Stdin_Rd = INVALID_HANDLE_قدر;
    }

    // Read output from child process.
    برائے (;;)
    {
	حرف	buf[4096];
	DWORD	n;

	اگر (!ReadFile(h_بچہ_Stdout_Rd, buf, حجم(buf), &n, تہی) || n == 0)
	    توڑو;
	اگر (ga_grow(&برآمد_ga, (صحیح)n) == OK)
	{
	    mch_memmove((حرف *)برآمد_ga.ga_ڈیٹا + برآمد_ga.ga_len, buf, n);
	    برآمد_ga.ga_len += (صحیح)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    Get_Exit_رمز_Process(pi.hProcess, &exit_رمز);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    سیٹ_vim_var_nr(VV_SHELL_ERROR, (طویل)exit_رمز);

    اگر (برآمد_ga.ga_len > 0)
    {
	بفر = alloc(برآمد_ga.ga_len + 1);
	اگر (بفر != تہی)
	{
	    mch_memmove(بفر, برآمد_ga.ga_ڈیٹا, برآمد_ga.ga_len);
	    اگر (ret_len == تہی)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		برائے (i = 0; i < برآمد_ga.ga_len; ++i)
		    اگر (بفر[i] == NUL)
			بفر[i] = 1;
		بفر[برآمد_ga.ga_len] = NUL;
	    }
	    ورنہ
		*ret_len = برآمد_ga.ga_len;
	}
    }
    ورنہ اگر (ret_len != تہی)
	*ret_len = 0;

مکمل:
    ga_clear(&cmd_ga);
    ga_clear(&برآمد_ga);
    اگر (h_بچہ_Stdout_Rd != INVALID_HANDLE_قدر)
	CloseHandle(h_بچہ_Stdout_Rd);
    اگر (h_بچہ_Stdout_Wr != INVALID_HANDLE_قدر)
	CloseHandle(h_بچہ_Stdout_Wr);
    اگر (h_بچہ_Stdin_Rd != INVALID_HANDLE_قدر)
	CloseHandle(h_بچہ_Stdin_Rd);
    واپس بفر;
}
#اگر_ختم // FEAT_EVAL

#اگر defined(FEAT_کام_CHANNEL)
    خالی
mch_کام_آغاز(حرف *cmd, کام_T *کام, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_اوصاف saAttr;
    channel_T		*channel = تہی;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    صحیح		use_null_for_اندر = options->jo_io[PART_اندر] == JIO_NULL;
    صحیح		use_null_for_برآمد = options->jo_io[PART_برآمد] == JIO_NULL;
    صحیح		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    صحیح		use_file_for_اندر = options->jo_io[PART_اندر] == JIO_FILE;
    صحیح		use_file_for_برآمد = options->jo_io[PART_برآمد] == JIO_FILE;
    صحیح		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    صحیح		use_برآمد_for_err = options->jo_io[PART_ERR] == JIO_برآمد;

    اگر (use_برآمد_for_err && use_null_for_برآمد)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_قدر;
    ifd[1] = INVALID_HANDLE_قدر;
    ofd[0] = INVALID_HANDLE_قدر;
    ofd[1] = INVALID_HANDLE_قدر;
    efd[0] = INVALID_HANDLE_قدر;
    efd[1] = INVALID_HANDLE_قدر;
    ga_init2(&ga, حجم(wchar_t), 500);

    jo = Create_کام_Object(تہی, تہی);
    اگر (jo == تہی)
    {
	کام->jv_حالت = کام_FAILED;
	جاؤ failed;
    }

    اگر (options->jo_env != تہی)
	win32_build_env(options->jo_env, &ga, FALSE);

    Zero_حافظہ(&pi, حجم(pi));
    Zero_حافظہ(&si, حجم(si));
    si.cb = حجم(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.w_Show_کھڑکی = SW_HIDE;

    saAttr.n_لمبائی = حجم(SECURITY_اوصاف);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = تہی;

    اگر (use_file_for_اندر)
    {
	char_u *fname = options->jo_io_name[PART_اندر];

	ifd[0] = کام_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_معمول);
	اگر (ifd[0] == INVALID_HANDLE_قدر)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    جاؤ failed;
	}
    }
    ورنہ اگر (!use_null_for_اندر
	    && (!create_pipe_pair(ifd)
		|| !سیٹ_Handle_Information(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	جاؤ failed;

    اگر (use_file_for_برآمد)
    {
	char_u *fname = options->jo_io_name[PART_برآمد];

	ofd[1] = کام_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_معمول);
	اگر (ofd[1] == INVALID_HANDLE_قدر)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    جاؤ failed;
	}
    }
    ورنہ اگر (!use_null_for_برآمد &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !سیٹ_Handle_Information(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	جاؤ failed;

    اگر (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = کام_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_معمول);
	اگر (efd[1] == INVALID_HANDLE_قدر)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    جاؤ failed;
	}
    }
    ورنہ اگر (!use_برآمد_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !سیٹ_Handle_Information(efd[0], HANDLE_FLAG_INHERIT, 0)))
	جاؤ failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_داخلہ = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_برآمد_for_err ? ofd[1] : efd[1];

    اگر (!use_null_for_اندر || !use_null_for_برآمد || !use_null_for_err)
    {
	اگر (options->jo_سیٹ & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    اگر (channel != تہی)
		++channel->ch_refcount;
	}
	ورنہ
	    channel = جوڑو_channel();
	اگر (channel == تہی)
	    جاؤ failed;
    }

    اگر (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_منوال |
	    CREATE_NEW_PROCESS_گروہ |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_ڈیٹا,
	    (حرف *)options->jo_cwd))
    {
	CloseHandle(jo);
	کام->jv_حالت = کام_FAILED;
	جاؤ failed;
    }

    ga_clear(&ga);

    اگر (!تفویض_Process_مقصد_کام_Object(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = تہی;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    کام->jv_proc_info = pi;
    کام->jv_کام_object = jo;
    کام->jv_حالت = کام_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    اگر (!use_برآمد_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    کام->jv_channel = channel;
    اگر (channel != تہی)
    {
	channel_سیٹ_pipes(channel,
		      use_file_for_اندر || use_null_for_اندر
					      ? INVALID_FD : (sock_T)ifd[1],
		      use_file_for_برآمد || use_null_for_برآمد
					     ? INVALID_FD : (sock_T)ofd[0],
		      use_برآمد_for_err || use_file_for_err || use_null_for_err
					    ? INVALID_FD : (sock_T)efd[0]);
	channel_سیٹ_کام(channel, کام, options);
    }
    واپس;

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

    حرف *
mch_کام_حالت(کام_T *کام)
{
    DWORD dw_Exit_رمز = 0;

    اگر (!Get_Exit_رمز_Process(کام->jv_proc_info.hProcess, &dw_Exit_رمز)
	    || dw_Exit_رمز != STILL_فعال)
    {
	کام->jv_exitval = (صحیح)dw_Exit_رمز;
	اگر (کام->jv_حالت < کام_ENDED)
	{
	    ch_log(کام->jv_channel, "Job ended");
	    کام->jv_حالت = کام_ENDED;
	}
	واپس "dead";
    }
    واپس "run";
}

    کام_T *
mch_detect_ended_کام(کام_T *کام_list)
{
    HANDLE کام_Handles[زیادہ_سے_زیادہ_WAIT_OBJECTS];
    کام_T *کام_Array[زیادہ_سے_زیادہ_WAIT_OBJECTS];
    کام_T *کام = کام_list;

    جب (کام != تہی)
    {
	DWORD n;
	DWORD نتیجہ;

	برائے (n = 0; n < زیادہ_سے_زیادہ_WAIT_OBJECTS
				       && کام != تہی; کام = کام->jv_اگلا)
	{
	    اگر (کام->jv_حالت == کام_STARTED)
	    {
		کام_Handles[n] = کام->jv_proc_info.hProcess;
		کام_Array[n] = کام;
		++n;
	    }
	}
	اگر (n == 0)
	    جاری;
	نتیجہ = Wait_For_مضرب_Objects(n, کام_Handles, FALSE, 0);
	اگر (نتیجہ >= WAIT_OBJECT_0 && نتیجہ < WAIT_OBJECT_0 + n)
	{
	    کام_T *wait_کام = کام_Array[نتیجہ - WAIT_OBJECT_0];

	    اگر (STRCMP(mch_کام_حالت(wait_کام), "dead") == 0)
		واپس wait_کام;
	}
    }
    واپس تہی;
}

    ساکن BOOL
terminate_all(HANDLE process, صحیح رمز)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_قدر;
    DWORD	    pid = GetProcessId(process);

    اگر (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	اگر (h != INVALID_HANDLE_قدر)
	{
	    pe.dw_سائز = حجم(PROCESSENTRY32);
	    اگر (!Process32_پہلا(h, &pe))
		جاؤ theend;

	    کرو
	    {
		اگر (pe.th32_والد_Process_ID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    اگر (ph != تہی)
		    {
			terminate_all(ph, رمز);
			CloseHandle(ph);
		    }
		}
	    } جب (Process32_اگلا(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    واپس TerminateProcess(process, رمز);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    صحیح
mch_signal_کام(کام_T *کام, char_u *how)
{
    صحیح ret;

    اگر (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	اگر (کام->jv_کام_object != تہی)
	{
	    اگر (کام->jv_channel != تہی && کام->jv_channel->ch_anonymous_pipe)
		کام->jv_channel->ch_killing = TRUE;
	    واپس Terminate_کام_Object(کام->jv_کام_object, (UINT)-1) ? OK : FAIL;
	}
	واپس terminate_all(کام->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    اگر (!AttachConsole(کام->jv_proc_info.dwProcessId))
	واپس FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		کام->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    واپس ret;
}

/*
 * Clear the data related to "job".
 */
    خالی
mch_clear_کام(کام_T *کام)
{
    اگر (کام->jv_حالت == کام_FAILED)
	واپس;

    اگر (کام->jv_کام_object != تہی)
	CloseHandle(کام->jv_کام_object);
    CloseHandle(کام->jv_proc_info.hProcess);
}
#اگر_ختم


#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    ساکن خالی
termcap_منوال_آغاز(خالی)
{
    DWORD cmodein;

    اگر (g_f_Termcap_منوال)
	واپس;

    Save_Console_بفر(&g_cbNonTermcap);

    اگر (g_cbTermcap.IsValid)
    {
	/*
	 * We've been in termcap mode before.  Restore certain screen
	 * characteristics, including the buffer size and the window
	 * size.  Since we will be redrawing the screen, we don't need
	 * to restore the actual contents of the buffer.
	 */
	Restore_Console_بفر(&g_cbTermcap, FALSE);
	reset_console_color_rgb();
	سیٹ_Console_کھڑکی_Info(g_h_Con_برآمد, TRUE, &g_cbTermcap.Info.sr_کھڑکی);
	صفیں = g_cbTermcap.Info.dw_سائز.Y;
	Columns = g_cbTermcap.Info.dw_سائز.X;
    }
    ورنہ
    {
	/*
	 * This is our first time entering termcap mode.  Clear the console
	 * screen buffer, and resize the buffer to match the current window
	 * size.  We will use this as the size of our editing environment.
	 */
	Clear_Console_بفر(g_attrCurrent);
	سیٹ_console_color_rgb();
	Resize_Con_Buf_And_کھڑکی(g_h_Con_برآمد, Columns, صفیں);
    }

    resettitle();

    Get_Console_منوال(g_h_Con_اندر, &cmodein);
    اگر (g_f_Mouse_فعال)
    {
	cmodein |= ENABLE_MOUSE_داخلہ;
	cmodein &= ~ENABLE_تیز_EDIT_منوال;
    }
    ورنہ
    {
	cmodein &= ~ENABLE_MOUSE_داخلہ;
	cmodein |= g_cmodein & ENABLE_تیز_EDIT_منوال;
    }
    cmodein |= ENABLE_کھڑکی_داخلہ;
    سیٹ_Console_منوال(g_h_Con_اندر, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_f_Termcap_منوال = TRUE;
}


/*
 * End termcap mode
 */
    ساکن خالی
termcap_منوال_انتہا(خالی)
{
    DWORD cmodein;
    Console_بفر *cb;
    COORD coord;
    DWORD dwDummy;

    اگر (!g_f_Termcap_منوال)
	واپس;

    Save_Console_بفر(&g_cbTermcap);

    Get_Console_منوال(g_h_Con_اندر, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_داخلہ | ENABLE_کھڑکی_داخلہ);
    cmodein |= g_cmodein & ENABLE_تیز_EDIT_منوال;
    سیٹ_Console_منوال(g_h_Con_اندر, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# ورنہ
    cb = &g_cbNonTermcap;
# endif
    Restore_Console_بفر(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    اگر (exiting && use_alternate_screen_بفر)
	vtp_printf("\033[?1049l");

    اگر (!USE_WT && (p_rs || exiting))
    {
	/*
	 * Clear anything that happens to be on the current line.
	 */
	coord.X = 0;
	coord.Y = (SHORT) (p_rs ? cb->Info.dwCursorPosition.Y : (صفیں - 1));
	FillConsoleOutputCharacter(g_h_Con_برآمد, ' ',
		cb->Info.dw_سائز.X, coord, &dwDummy);
	/*
	 * The following is just for aesthetics.  If we are exiting without
	 * restoring the screen, then we want to have a prompt string
	 * appear at the bottom line.  However, the command interpreter
	 * seems to always advance the cursor one line before displaying
	 * the prompt string, which causes the screen to scroll.  To
	 * counter this, move the cursor up one line before exiting.
	 */
	اگر (exiting && !p_rs)
	    coord.Y--;
	/*
	 * Position the cursor at the leftmost column of the desired row.
	 */
	سیٹ_Console_Cursor_Position(g_h_Con_برآمد, coord);
    }
    سیٹ_Console_Cursor_Info(g_h_Con_برآمد, &g_cci);
    g_f_Termcap_منوال = FALSE;
}
#اگر_ختم


#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    ساکن خالی
clear_chars(
    COORD coord,
    DWORD n)
{
    اگر (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_h_Con_برآمد, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_h_Con_برآمد, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    ورنہ
    {
	سیٹ_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    ساکن خالی
clear_screen(خالی)
{
    g_coord.X = g_coord.Y = 0;

    اگر (!vtp_working)
	clear_chars(g_coord, صفیں * Columns);
    ورنہ
    {
	سیٹ_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    ساکن خالی
clear_مقصد_انتہا_کا_display(خالی)
{
    COORD save = g_coord;

    اگر (!vtp_working)
	clear_chars(g_coord, (صفیں - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    ورنہ
    {
	سیٹ_console_color_rgb();
	gotoxy(g_coord.X + 1, g_coord.Y + 1);
	vtp_printf("\033[0J");

	gotoxy(save.X + 1, save.Y + 1);
	g_coord = save;
    }
}


/*
 * Clear to end of line
 */
    ساکن خالی
clear_مقصد_انتہا_کا_سطر(خالی)
{
    COORD save = g_coord;

    اگر (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    ورنہ
    {
	سیٹ_console_color_rgb();
	gotoxy(g_coord.X + 1, g_coord.Y + 1);
	vtp_printf("\033[0K");

	gotoxy(save.X + 1, save.Y + 1);
	g_coord = save;
    }
}


/*
 * Scroll the scroll region up by `cLines' lines
 */
    ساکن خالی
scroll(غیرعلامتی c_سطریں)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.بایاں_3 + 1, g_srScrollRegion.چوٹی_2 + 1);
    delete_سطریں(c_سطریں);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    ساکن خالی
سیٹ_scroll_region(
    غیرعلامتی بایاں,
    غیرعلامتی چوٹی,
    غیرعلامتی دایاں,
    غیرعلامتی bottom)
{
    اگر (بایاں >= دایاں
	    || چوٹی >= bottom
	    || دایاں > (غیرعلامتی) Columns - 1
	    || bottom > (غیرعلامتی) صفیں - 1)
	واپس;

    g_srScrollRegion.بایاں_3 =   بایاں;
    g_srScrollRegion.چوٹی_2 =    چوٹی;
    g_srScrollRegion.دایاں_3 =  دایاں;
    g_srScrollRegion.Bottom = bottom;
}

    ساکن خالی
سیٹ_scroll_region_tb(
    غیرعلامتی چوٹی,
    غیرعلامتی bottom)
{
    اگر (چوٹی >= bottom || bottom > (غیرعلامتی)صفیں - 1)
	واپس;

    g_srScrollRegion.چوٹی_2 = چوٹی;
    g_srScrollRegion.Bottom = bottom;
}

    ساکن خالی
سیٹ_scroll_region_lr(
    غیرعلامتی بایاں,
    غیرعلامتی دایاں)
{
    اگر (بایاں >= دایاں || دایاں > (غیرعلامتی)Columns - 1)
	واپس;

    g_srScrollRegion.بایاں_3 = بایاں;
    g_srScrollRegion.دایاں_3 = دایاں;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    ساکن خالی
داخل_کرو_سطریں(غیرعلامتی c_سطریں)
{
    SMALL_RECT	    ماخذ, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.بایاں_3 + 1, g_srScrollRegion.چوٹی_2 + 1);

    dest.X = g_srScrollRegion.بایاں_3;
    dest.Y = g_coord.Y + c_سطریں;

    ماخذ.بایاں_3   = g_srScrollRegion.بایاں_3;
    ماخذ.چوٹی_2	  = g_coord.Y;
    ماخذ.دایاں_3  = g_srScrollRegion.دایاں_3;
    ماخذ.Bottom = g_srScrollRegion.Bottom - c_سطریں;

    clip.بایاں_3   = g_srScrollRegion.بایاں_3;
    clip.چوٹی_2    = g_coord.Y;
    clip.دایاں_3  = g_srScrollRegion.دایاں_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    اگر (!USE_VTP)
	fill.اوصاف_2 = g_attrCurrent;
    ورنہ
	fill.اوصاف_2 = g_attrDefault;

    سیٹ_console_color_rgb();

    Scroll_Console_Screen_بفر(g_h_Con_برآمد, &ماخذ, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    اگر (ماخذ.Bottom < dest.Y)
    {
	COORD coord;
	صحیح   i;

	coord.X = ماخذ.بایاں_3;
	برائے (i = clip.چوٹی_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ماخذ.دایاں_3 - ماخذ.بایاں_3 + 1);
	}
    }

    اگر (vtp_working)
    {
	COORD coord;
	صحیح i;

	coord.X = ماخذ.بایاں_3;
	برائے (i = ماخذ.چوٹی_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ماخذ.دایاں_3 - ماخذ.بایاں_3 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    ساکن خالی
delete_سطریں(غیرعلامتی c_سطریں)
{
    SMALL_RECT	    ماخذ, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    صحیح		    nb;

    gotoxy(g_srScrollRegion.بایاں_3 + 1, g_srScrollRegion.چوٹی_2 + 1);

    dest.X = g_srScrollRegion.بایاں_3;
    dest.Y = g_coord.Y;

    ماخذ.بایاں_3   = g_srScrollRegion.بایاں_3;
    ماخذ.چوٹی_2	  = g_coord.Y + c_سطریں;
    ماخذ.دایاں_3  = g_srScrollRegion.دایاں_3;
    ماخذ.Bottom = g_srScrollRegion.Bottom;

    clip.بایاں_3   = g_srScrollRegion.بایاں_3;
    clip.چوٹی_2    = g_coord.Y;
    clip.دایاں_3  = g_srScrollRegion.دایاں_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    اگر (!vtp_working)
	fill.اوصاف_2 = g_attrCurrent;
    ورنہ
	fill.اوصاف_2 = g_attrDefault;

    سیٹ_console_color_rgb();

    Scroll_Console_Screen_بفر(g_h_Con_برآمد, &ماخذ, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (ماخذ.Bottom - ماخذ.چوٹی_2) + 1;

    اگر (nb < ماخذ.چوٹی_2)
    {
	COORD coord;
	صحیح   i;

	coord.X = ماخذ.بایاں_3;
	برائے (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ماخذ.دایاں_3 - ماخذ.بایاں_3 + 1);
	}
    }

    اگر (vtp_working)
    {
	COORD coord;
	صحیح i;

	coord.X = ماخذ.بایاں_3;
	برائے (i = nb; i <= ماخذ.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, ماخذ.دایاں_3 - ماخذ.بایاں_3 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    ساکن خالی
gotoxy(
    غیرعلامتی x,
    غیرعلامتی y)
{
    اگر (x < 1 || x > (غیرعلامتی)Columns || y < 1 || y > (غیرعلامتی)صفیں)
	واپس;

    اگر (!USE_VTP)
    {
	// There are reports of double-width characters not displayed
	// correctly.  This workaround should fix it, similar to how it's done
	// for VTP.
	g_coord.X = 0;
	سیٹ_Console_Cursor_Position(g_h_Con_برآمد, g_coord);

	// external cursor coords are 1-based; internal are 0-based
	g_coord.X = x - 1;
	g_coord.Y = y - 1;
	سیٹ_Console_Cursor_Position(g_h_Con_برآمد, g_coord);
    }
    ورنہ
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
    ساکن خالی
textattr(لفظ_2 wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    سیٹ_Console_متن_Attribute(g_h_Con_برآمد, wAttr);
}


    ساکن خالی
textcolor(لفظ_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    اگر (!vtp_working)
	سیٹ_Console_متن_Attribute(g_h_Con_برآمد, g_attrCurrent);
    ورنہ
	vtp_sgr_bulk(wAttr);
}


    ساکن خالی
textbackground(لفظ_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    اگر (!vtp_working)
	سیٹ_Console_متن_Attribute(g_h_Con_برآمد, g_attrCurrent);
    ورنہ
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    ساکن خالی
normvideo(خالی)
{
    اگر (!vtp_working)
	textattr(g_attrDefault);
    ورنہ
	vtp_sgr_bulk(0);
}


ساکن لفظ_2 g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    ساکن خالی
standout(خالی)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((لفظ_2) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    ساکن خالی
standend(خالی)
{
    اگر (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    خالی
mch_سیٹ_معمول_colors(خالی)
{
    char_u	*p;
    صحیح		n;

    cterm_معمول_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_معمول_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    اگر (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	اگر (*p == 'm' && n > 0)
	{
	    cterm_معمول_fg_color = (n & 0xf) + 1;
	    cterm_معمول_bg_color = ((n >> 4) & 0xf) + 1;
	}
    }
# ifdef FEAT_TERMGUICOLORS
    cterm_معمول_fg_gui_color = INVALCOLOR;
    cterm_معمول_bg_gui_color = INVALCOLOR;
# endif
}


/*
 * visual bell: flash the screen
 */
    ساکن خالی
visual_bell(خالی)
{
    COORD   coordOrigin = {0, 0};
    لفظ_2    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = تہی;

# ifdef FEAT_TERMGUICOLORS
    اگر (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(لفظ_2, صفیں * Columns);
	اگر (oldattrs == تہی)
	    واپس;
	ReadConsoleOutputAttribute(g_h_Con_برآمد, oldattrs, صفیں * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_h_Con_برآمد, attrFlash, صفیں * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    اگر (oldattrs != تہی)
    {
	WriteConsoleOutputAttribute(g_h_Con_برآمد, oldattrs, صفیں * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    ساکن خالی
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    اگر (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	واپس;
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
    ساکن DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cb_مقصد_Write)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    ساکن DWORD    cells;
    ساکن WCHAR    *unicodebuf = تہی;
    ساکن صحیح	    unibuflen = 0;
    ساکن صحیح	    لمبائی;
    صحیح		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    ساکن WCHAR    *utf8spbuf = تہی;
    ساکن صحیح	    utf8splength;
    ساکن DWORD    utf8spcells;
    ساکن WCHAR    **utf8usingbuf = &unicodebuf;

    اگر (cb_مقصد_Write != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	کرو
	{
	    لمبائی = Multi_Byte_مقصد_Wide_Char(cp, 0, (LPCSTR)pchBuf, cb_مقصد_Write,
							unicodebuf, unibuflen);
	    اگر (لمبائی && لمبائی <= unibuflen)
		توڑو;
	    vim_free(unicodebuf);
	    unicodebuf = لمبائی ? LALLOC_MULT(WCHAR, لمبائی) : تہی;
	    unibuflen = unibuflen ? 0 : لمبائی;
	} جب (TRUE);
	cells = mb_string2cells(pchBuf, cb_مقصد_Write);
    }
    ورنہ // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	اگر (utf8usingbuf != &utf8spbuf)
	{
	    اگر (utf8spbuf == تہی)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		لمبائی = Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, " ", 1, تہی, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, لمبائی);
		اگر (utf8spbuf != تہی)
		{
		    Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, " ", 1, utf8spbuf, لمبائی);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = لمبائی;
		    utf8spcells = cells;
		}
	    }
	    ورنہ
	    {
		utf8usingbuf = &utf8spbuf;
		لمبائی = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    اگر (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_h_Con_برآمد, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	اگر (WriteConsoleOutputCharacterW(g_h_Con_برآمد, *utf8usingbuf, لمبائی,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    ورنہ
    {
	اگر (WriteConsoleW(g_h_Con_برآمد, *utf8usingbuf, لمبائی, &cchwritten,
		    تہی) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    اگر (cchwritten == (DWORD)لمبائی)
    {
	written = cb_مقصد_Write;
	g_coord.X += (SHORT)cells;
    }
    ورنہ
    {
	char_u *p = pchBuf;
	برائے (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    جب (g_coord.X > g_srScrollRegion.دایاں_3)
    {
	g_coord.X -= (SHORT) Columns;
	اگر (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    اگر (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    واپس written;
}

    ساکن char_u *
get_seq(
    صحیح *دلائل,
    صحیح *گنتی,
    char_u *سر)
{
    صحیح argc;
    char_u *p;

    اگر (سر == تہی || *سر != '\033')
	واپس تہی;

    argc = 0;
    p = سر;
    ++p;
    کرو
    {
	++p;
	دلائل[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } جب (*p == ';');
    *گنتی = argc;

    واپس p;
}

    ساکن char_u *
get_sgr(
    صحیح *دلائل,
    صحیح *گنتی,
    char_u *سر)
{
    char_u *p = get_seq(دلائل, گنتی, سر);

    واپس (p && *p == 'm') ? ++p : تہی;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    ساکن char_u *
sgrn2(
    char_u *سر,
    صحیح	   n)
{
    صحیح argc;
    صحیح دلائل[16];
    char_u *p = get_sgr(دلائل, &argc, سر);

    واپس p && argc == 5 && دلائل[0] == n && دلائل[1] == 2 ? p : تہی;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    ساکن char_u *
sgrnc(
    char_u *سر,
    صحیح	   n)
{
    صحیح argc;
    صحیح دلائل[16];
    char_u *p = get_sgr(دلائل, &argc, سر);

    واپس p && argc == 1 && دلائل[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : تہی;
}

    ساکن char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    جب (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    واپس p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    ساکن char_u *
sgrn2c(
    char_u *سر,
    صحیح	   n)
{
    char_u *p = sgrn2(سر, n);

    واپس p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : تہی;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    ساکن char_u *
sgrn2cn(
    char_u *سر,
    صحیح n)
{
    char_u *p = sgrn2(سر, n);

    واپس p && p[0] == 0x0a && p[1] == '\033' ? ++p : تہی;
}

ساکن bool اندر_dcs = false;	// inside a DCS (sixel) sequence
ساکن bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    ساکن خالی
write_vt(char_u *s, صحیح len)
{
    صحیح		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, تہی, 0);
    اگر (wlen <= 0)
	واپس;
    ws = ALLOC_MULT(WCHAR, wlen);
    اگر (ws == تہی)
	واپس;
    Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_h_Con_برآمد, ws, wlen, &written, تہی);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    ساکن char_u *
skip_dcs(char_u *s, char_u *انتہا)
{
    برائے (char_u *p = s; p < انتہا; ++p)
    {
	اگر (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    اندر_dcs = false;
	    واپس p + 1;
	}
	dcs_esc = *p == ESC;
    }
    واپس انتہا;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    خالی
mch_write(
    char_u  *s,
    صحیح	    len)
{
    char_u  *انتہا = s + len;

# ifdef VIMDLL
    اگر (gui.اندر_use)
	واپس;
# endif

    اگر (!رکن_console)
    {
	اكتب(1, s, (غیرعلامتی)len);
	واپس;
    }

    // translate ESC | sequences into faked bios calls
    جب (len--)
    {
	صحیح سابقہ = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	اگر (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    واپس;
	}

	اگر (اندر_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    صحیح	l = (صحیح)(skip_dcs(s, انتہا) - s);

	    اگر (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    جاری;
	}

	جب (s + ++سابقہ < انتہا)
	{
	    ch = s[سابقہ];
	    اگر (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		توڑو;
	}

	اگر (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    اگر (سابقہ != 0)
		سابقہ = 1;
	}

	اگر (سابقہ != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, سابقہ);
# ifdef MCH_WRITE_DUMP
	    اگر (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, حجم(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	ورنہ اگر (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    اگر (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.بایاں_3 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    ورنہ
	    {
		gotoxy(g_srScrollRegion.بایاں_3 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    اگر (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	ورنہ اگر (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.بایاں_3+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    اگر (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	ورنہ اگر (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    اگر (g_coord.X > g_srScrollRegion.بایاں_3)
		g_coord.X--;
	    ورنہ اگر (g_coord.Y > g_srScrollRegion.چوٹی_2)
	    {
		g_coord.X = g_srScrollRegion.دایاں_3;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    اگر (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	ورنہ اگر (s[0] == '\a')
	{
	    // \a, bell
	    پیغام_Beep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    اگر (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	ورنہ اگر (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    صحیح	    arg1 = 0, arg2 = 0, argc = 0, دلائل[16];
	    char_u  *sp;

	    انتخاب (s[2])
	    {
	    حالت '0': حالت '1': حالت '2': حالت '3': حالت '4':
	    حالت '5': حالت '6': حالت '7': حالت '8': حالت '9':
		اگر (*(p = get_seq(دلائل, &argc, s)) != 'm')
		    جاؤ notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		اگر (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (صحیح)(p - s);
		    s = p;
		    توڑو;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		اگر (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		اگر (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		اگر (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		اگر (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(دلائل, &argc, p);

notsgr:
		arg1 = دلائل[0];
		arg2 = دلائل[1];
		اگر (*p == 'm')
		{
		    اگر (argc == 1 && دلائل[0] == 0)
			normvideo();
		    ورنہ اگر (argc == 1)
		    {
			اگر (USE_VTP)
			    textcolor((لفظ_2)arg1);
			ورنہ
			    textattr((لفظ_2)arg1);
		    }
		    ورنہ اگر (vtp_working)
			vtp_sgr_bulks(argc, دلائل);
		}
		ورنہ اگر (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		ورنہ اگر (argc == 2 && *p == 'r')
		{
		    سیٹ_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		ورنہ اگر (argc == 2 && *p == 'R')
		{
		    سیٹ_scroll_region_tb(arg1, arg2);
		}
		ورنہ اگر (argc == 2 && *p == 'V')
		{
		    سیٹ_scroll_region_lr(arg1, arg2);
		}
		ورنہ اگر (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.چوٹی_2, g_coord.Y - arg1) + 1);
		}
		ورنہ اگر (argc == 1 && *p == 'b')
		{
		    textbackground((لفظ_2) arg1);
		}
		ورنہ اگر (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.دایاں_3, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		ورنہ اگر (argc == 1 && *p == 'f')
		{
		    textcolor((لفظ_2) arg1);
		}
		ورنہ اگر (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		ورنہ اگر (argc == 1 && *p == 'L')
		{
		    داخل_کرو_سطریں(arg1);
		}
		ورنہ اگر (argc == 1 && *p == 'M')
		{
		    delete_سطریں(arg1);
		}

		len -= (صحیح)(p - s);
		s = p + 1;
		توڑو;

	    حالت 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.چوٹی_2, g_coord.Y - 1) + 1);
		جاؤ got3;

	    حالت 'B':
		visual_bell();
		جاؤ got3;

	    حالت 'C':
		gotoxy(min(g_srScrollRegion.دایاں_3, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		جاؤ got3;

	    حالت 'E':
		termcap_منوال_انتہا();
		جاؤ got3;

	    حالت 'F':
		standout();
		جاؤ got3;

	    حالت 'f':
		standend();
		جاؤ got3;

	    حالت 'H':
		gotoxy(1, 1);
		جاؤ got3;

	    حالت 'j':
		clear_مقصد_انتہا_کا_display();
		جاؤ got3;

	    حالت 'J':
		clear_screen();
		جاؤ got3;

	    حالت 'K':
		clear_مقصد_انتہا_کا_سطر();
		جاؤ got3;

	    حالت 'L':
		داخل_کرو_سطریں(1);
		جاؤ got3;

	    حالت 'M':
		delete_سطریں(1);
		جاؤ got3;

	    حالت 'S':
		termcap_منوال_آغاز();
		جاؤ got3;

	    حالت 'V':
		cursor_visible(TRUE);
		جاؤ got3;

	    حالت 'v':
		cursor_visible(FALSE);
		جاؤ got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    اگر (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, حجم(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	ورنہ اگر (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    صحیح l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    جب (s + l < انتہا && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    اگر (s + l < انتہا && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		اگر (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	ورنہ اگر (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    صحیح	l;

	    اندر_dcs = true;
	    dcs_esc = false;
	    l = (صحیح)(skip_dcs(s + 2, انتہا) - s);
	    اگر (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	ورنہ
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    اگر (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, حجم(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif

	    len -= (nWritten - 1);
	    s += nWritten;
	}
    }

# ifdef MCH_WRITE_DUMP
    اگر (fdDump)
	fflush(fdDump);
# endif
}

#ورنہ // FEAT_GUI_MSWIN && !VIMDLL
    خالی
mch_write(
    char_u  *s غیر_مستعمل,
    صحیح	    len غیر_مستعمل)
{
    // never used
}

#اگر_ختم


/*
 * Delay for "msec" milliseconds.
 */
    خالی
mch_delay(
    طویل    msec,
    صحیح	    flags غیر_مستعمل)
{
#اگر defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((صحیح)msec);	    // never wait for input
#ورنہ // Console
# ifdef VIMDLL
    اگر (gui.اندر_use)
    {
	Sleep((صحیح)msec);	    // never wait for input
	واپس;
    }
# endif
    اگر (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	اگر (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    صحیح towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    جب (msec > 0)
	    {
		mzvim_check_دھاگے();
		اگر (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	ورنہ
# endif
	    Sleep((صحیح)msec);
    ورنہ
	WaitForChar(msec, FALSE);
#اگر_ختم
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    صحیح
mch_remove(char_u *name)
{
    WCHAR	*wn;
    صحیح		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    اگر (mch_isdir(name) && mch_is_symbolic_link(name))
	واپس mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_معمول);

    wn = enc_مقصد_utf16(name, تہی);
    اگر (wn == تہی)
	واپس -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    واپس n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    خالی
mch_breakcheck(صحیح force غیر_مستعمل)
{
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    اگر (!gui.اندر_use)
# endif
	اگر (g_fCtrlCPressed || g_fCBrkPressed)
	{
	    ctrl_break_was_pressed = g_fCBrkPressed;
	    g_fCtrlCPressed = g_fCBrkPressed = FALSE;
	    got_int = TRUE;
	}
#اگر_ختم
}

// physical RAM to leave for the OS
#تعریف_کرو WINNT_RESERVE_بائٹیں     (256*1024*1024)

/*
 * How much main memory in KiB that can be used by VIM.
 */
    long_u
mch_کل_mem(صحیح special غیر_مستعمل)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dw_لمبائی = حجم(MEMORYSTATUSEX);
    Global_حافظہ_حالت_Ex(&ms);
    اگر (ms.ullAvailVirtual < ms.ull_کل_Phys)
    {
	// Process address space fits in physical RAM, use all of it.
	واپس (long_u)(ms.ullAvailVirtual / 1024);
    }
    اگر (ms.ull_کل_Phys <= WINNT_RESERVE_بائٹیں)
    {
	// Catch old NT box or perverse hardware setup.
	واپس (long_u)((ms.ull_کل_Phys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    واپس (long_u)((ms.ull_کل_Phys - WINNT_RESERVE_بائٹیں) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    صحیح
mch_expandpath(
    garray_T	*gap,
    char_u	*راستہ,
    صحیح		flags)		// EW_* flags
{
    واپس dos_expandpath(gap, راستہ, 0, flags, FALSE);
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
    صحیح
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	sz_عارضی_File[MAX_راستہ_2 + 1];
    WCHAR	sz_New_راستہ[MAX_راستہ_2 + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    برائے (p = q = wold; *p != L'\0'; ++p)
    {
	انتخاب (*p)
	{
	حالت L'/':
	حالت L'\\':
	حالت L':':
	    اگر (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    توڑو;

	طےشدہ:
	    توڑو;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    اگر ((صحیح)(p - q) < 8 || q[6] != L'~')
	واپس (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    اگر (Get_بھرا_راستہ_Name_W(wnew, MAX_راستہ_2, sz_New_راستہ, &p) == 0 || p == تہی)
	واپس -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    اگر (Get_عارضی_File_Name_W(sz_New_راستہ, L"VIM", 0, sz_عارضی_File) == 0)
	واپس -2;

    // blow the temp file away
    اگر (!DeleteFileW(sz_عارضی_File))
	واپس -3;

    // rename old file to the temp file
    اگر (!MoveFileW(wold, sz_عارضی_File))
	واپس -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    اگر ((hf = CreateFileW(wold, GENERIC_WRITE, 0, تہی, CREATE_NEW,
		    FILE_ATTRIBUTE_معمول, تہی)) == INVALID_HANDLE_قدر)
	واپس -5;
    اگر (!CloseHandle(hf))
	واپس -6;

    // rename the temp file to the new file
    اگر (!MoveFileW(sz_عارضی_File, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(خالی)MoveFileW(sz_عارضی_File, wold);
	واپس -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(sz_عارضی_File);

    // finally, remove the empty old file
    اگر (!DeleteFileW(wold))
	واپس -8;

    واپس 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    صحیح
mch_rename(
    ثابت حرف	*pszOldFile,
    ثابت حرف	*pszNewFile)
{
    WCHAR	*wold = تہی;
    WCHAR	*wnew = تہی;
    صحیح		retval = -1;

    wold = enc_مقصد_utf16((char_u *)pszOldFile, تہی);
    wnew = enc_مقصد_utf16((char_u *)pszNewFile, تہی);
    اگر (wold != تہی && wnew != تہی)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    واپس retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    حرف *
default_shell(خالی)
{
    واپس "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    صحیح
mch_access(حرف *n, صحیح p)
{
    HANDLE	hFile;
    صحیح		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_مقصد_utf16((char_u *)n, تہی);
    اگر (wn == تہی)
	واپس -1;

    اگر (mch_isdir((char_u *)n))
    {
	WCHAR عارضی_Name_W[MAX_راستہ_2 + 16] = L"";

	اگر (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    صحیح			i;
	    WIN32_ڈھونڈو_DATAW    d;

	    برائے (i = 0; i < MAX_راستہ_2 && wn[i] != 0; ++i)
		عارضی_Name_W[i] = wn[i];
	    اگر (عارضی_Name_W[i - 1] != '\\' && عارضی_Name_W[i - 1] != '/')
		عارضی_Name_W[i++] = '\\';
	    عارضی_Name_W[i++] = '*';
	    عارضی_Name_W[i++] = 0;

	    hFile = ڈھونڈو_پہلا_File_W(عارضی_Name_W, &d);
	    اگر (hFile == INVALID_HANDLE_قدر)
		جاؤ getout;
	    ورنہ
		(خالی)ڈھونڈو_Close(hFile);
	}

	اگر (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    اگر (!Get_عارضی_File_Name_W(wn, L"VIM", 0, عارضی_Name_W))
		جاؤ getout;
	    ورنہ
		DeleteFileW(عارضی_Name_W);
	}
    }
    ورنہ
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_منوال = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_منوال = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_منوال, share_منوال,
					     تہی, OPEN_EXISTING, 0, تہی);
	اگر (hFile == INVALID_HANDLE_قدر)
	    جاؤ getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    واپس retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    صحیح
mch_open(ثابت حرف *name, صحیح flags, صحیح منوال)
{
    WCHAR	*wn;
    صحیح		f;

    wn = enc_مقصد_utf16((char_u *)name, تہی);
    اگر (wn == تہی)
	واپس -1;

    f = _wopen(wn, flags, منوال);
    vim_free(wn);
    واپس f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(ثابت حرف *name, ثابت حرف *منوال)
{
    WCHAR	*wn, *wm;
    FILE	*f = تہی;

#اگر defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    حرف new_منوال = منوال[لڑی_لمبائی(منوال) - 1];
    صحیح old_منوال_2 = 0;

    _get_fmode(&old_منوال_2);
    اگر (new_منوال == 't')
	سیٹ_fmode(O_متن);
    ورنہ اگر (new_منوال == 'b')
	سیٹ_fmode(O_ثنائی_2);
#اگر_ختم
    wn = enc_مقصد_utf16((char_u *)name, تہی);
    wm = enc_مقصد_utf16((char_u *)منوال, تہی);
    اگر (wn != تہی && wm != تہی)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#اگر defined(DEBUG) && _MSC_VER >= 1400
    اگر (old_منوال_2 != 0)
	سیٹ_fmode(old_منوال_2);
#اگر_ختم
    واپس f;
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
    ساکن خالی
copy_substream(HANDLE sh, خالی *context, WCHAR *مقصد, WCHAR *substream, طویل len)
{
    HANDLE  h_مقصد;
    WCHAR   *مقصد_name;

    مقصد_name = alloc((wcslen(مقصد) + wcslen(substream) + 1) * حجم(WCHAR));
    اگر (مقصد_name == تہی)
	واپس;

    wcscpy(مقصد_name, مقصد);
    wcscat(مقصد_name, substream);

    h_مقصد = CreateFileW(مقصد_name, GENERIC_WRITE, 0, تہی, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_معمول, تہی);
    اگر (h_مقصد != INVALID_HANDLE_قدر)
    {
	طویل	مکمل;
	DWORD	todo;
	DWORD	readcnt, written;
	حرف	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	برائے (مکمل = 0; مکمل < len; مکمل += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - مکمل) > حجم(buf) ? حجم(buf)
						       : (size_t)(len - مکمل));
	    اگر (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(h_مقصد, buf, todo, &written, تہی)
		    || written != todo)
		توڑو;
	}
	CloseHandle(h_مقصد);
    }

    آزاد(مقصد_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    ساکن خالی
copy_infostreams(char_u *from, char_u *مقصد)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_دھارا_ID	sid;
    صحیح			headersize;
    WCHAR		streamname[MAX_راستہ_2];
    DWORD		readcount;
    خالی		*context = تہی;
    DWORD		lo, hi;
    صحیح			len;

    // Convert the file names to wide characters.
    fromw = enc_مقصد_utf16(from, تہی);
    tow = enc_مقصد_utf16(مقصد, تہی);
    اگر (fromw != تہی && tow != تہی)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, تہی,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, تہی);
	اگر (sh != INVALID_HANDLE_قدر)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    برائے (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		Zero_حافظہ(&sid, حجم(WIN32_دھارا_ID));
		headersize = (صحیح)((حرف *)&sid.c_دھارا_Name - (حرف *)&sid.dw_دھارا_Id);
		اگر (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    توڑو;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		اگر (sid.dw_دھارا_Name_سائز > 0)
		{
		    // Read the stream name.
		    اگر (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dw_دھارا_Name_سائز,
					  &readcount, FALSE, FALSE, &context))
			توڑو;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / حجم(WCHAR);
		    streamname[len] = 0;
		    اگر (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (طویل)sid.سائز_2.u.کم_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(خالی)BackupSeek(sh, sid.سائز_2.u.کم_Part, sid.سائز_2.u.بلند_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (خالی)BackupRead(sh, تہی, 0, &readcount, TRUE, FALSE, &context);

	    CloseHandle(sh);
	}
    }
    vim_free(fromw);
    vim_free(tow);
}

/*
 * ntdll.dll definitions
 */
#تعریف_کرو FileEaInformation   7
#اگر_غیر_تعریف_شدہ حالت_SUCCESS
# define حالت_SUCCESS	    ((NTSTATUS) 0x00000000L)
#اگر_ختم

قسمتعریف ساخت FILE_بھرا_EA_INFORMATION {
    ULONG  اگلا_اندراج_انحراف;
    UCHAR  Flags;
    UCHAR  Ea_Name_لمبائی;
    USHORT Ea_قدر_لمبائی;
    CHAR   EaName[1];
} FILE_بھرا_EA_INFORMATION_2, *PFILE_بھرا_EA_INFORMATION;

قسمتعریف ساخت _FILE_EA_INFORMATION_ {
    ULONG Ea_سائز;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#اگر_غیر_تعریف_شدہ PROTO
قسمتعریف NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_نقاب DesiredAccess,
	POBJECT_اوصاف Object_اوصاف,
	PIO_حالت_بلاک Io_حالت_بلاک,
	ULONG ShareAccess,
	ULONG OpenOptions);
قسمتعریف NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
قسمتعریف NTSTATUS (NTAPI *Pfn_Nt_سیٹ_Ea_File)(
	HANDLE		    FileHandle,
	PIO_حالت_بلاک    Io_حالت_بلاک,
	PVOID		    بفر_2,
	ULONG		    لمبائی_2);
قسمتعریف NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_حالت_بلاک Io_حالت_بلاک,
	PVOID بفر_2,
	ULONG لمبائی_2,
	BOOLEAN Return_Single_اندراج,
	PVOID EaList,
	ULONG Ea_List_لمبائی,
	PULONG Ea_اشاریہ,
	BOOLEAN Restart_جائزہ);
قسمتعریف NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_حالت_بلاک	Io_حالت_بلاک,
	PVOID			FileInformation,
	ULONG			لمبائی_2,
	FILE_INFORMATION_CLASS FileInformationClass);
قسمتعریف VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR ماخذ_String);

PfnNtOpenFile pNtOpenFile = تہی;
PfnNtClose pNtClose = تہی;
Pfn_Nt_سیٹ_Ea_File p_Nt_سیٹ_Ea_File = تہی;
PfnNtQueryEaFile pNtQueryEaFile = تہی;
PfnNtQueryInformationFile pNtQueryInformationFile = تہی;
PfnRtlInitUnicodeString pRtlInitUnicodeString = تہی;
#اگر_ختم

/*
 * Load ntdll.dll functions.
 */
    ساکن BOOL
بھرو_ntdll(خالی)
{
    ساکن صحیح	loaded = -1;

    اگر (loaded != -1)
	واپس (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    اگر (hNtdll != تہی)
    {
	pNtOpenFile = (PfnNtOpenFile) Get_Proc_پتہ(hNtdll, "NtOpenFile");
	pNtClose = (PfnNtClose) Get_Proc_پتہ(hNtdll, "NtClose");
	p_Nt_سیٹ_Ea_File = (Pfn_Nt_سیٹ_Ea_File)
	    Get_Proc_پتہ(hNtdll, "NtSetEaFile");
	pNtQueryEaFile = (PfnNtQueryEaFile)
	    Get_Proc_پتہ(hNtdll, "NtQueryEaFile");
	pNtQueryInformationFile = (PfnNtQueryInformationFile)
	    Get_Proc_پتہ(hNtdll, "NtQueryInformationFile");
	pRtlInitUnicodeString = (PfnRtlInitUnicodeString)
	    Get_Proc_پتہ(hNtdll, "RtlInitUnicodeString");
    }
    اگر (pNtOpenFile == تہی
	    || pNtClose == تہی
	    || p_Nt_سیٹ_Ea_File == تہی
	    || pNtQueryEaFile == تہی
	    || pNtQueryInformationFile == تہی
	    || pRtlInitUnicodeString == تہی)
	loaded = FALSE;
    ورنہ
	loaded = TRUE;
    واپس (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    ساکن خالی
copy_extattr(char_u *from, char_u *مقصد)
{
    char_u		    *fromf = تہی;
    char_u		    *tof = تہی;
    WCHAR		    *fromw = تہی;
    WCHAR		    *tow = تہی;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_اوصاف	    oa;
    IO_حالت_بلاک	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    خالی		    *ea = تہی;

    اگر (!بھرو_ntdll())
	واپس;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(مقصد) + 5);
    اگر (fromf == تہی || tof == تہی)
	جاؤ theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), مقصد);

    // Convert the names to wide characters.
    fromw = enc_مقصد_utf16(fromf, تہی);
    tow = enc_مقصد_utf16(tof, تہی);
    اگر (fromw == تہی || tow == تہی)
	جاؤ theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    Initialize_Object_اوصاف(&oa, &u, 0, تہی, تہی);
    اگر (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != حالت_SUCCESS)
	جاؤ theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, حجم(eainfo),
	    FileEaInformation);
    اگر (eainfo.Ea_سائز != 0)
    {
	ea = alloc(eainfo.Ea_سائز);
	اگر (ea != تہی)
	{
	    اگر (pNtQueryEaFile(h, &iosb, ea, eainfo.Ea_سائز, FALSE,
			تہی, 0, تہی, TRUE) != حالت_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    اگر (ea != تہی)
    {
	pRtlInitUnicodeString(&u, tow);
	Initialize_Object_اوصاف(&oa, &u, 0, تہی, تہی);
	اگر (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != حالت_SUCCESS)
	    جاؤ theend;

	p_Nt_سیٹ_Ea_File(h, &iosb, ea, eainfo.Ea_سائز);
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
    صحیح
mch_copy_file_attribute(char_u *from, char_u *مقصد)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, مقصد);
    copy_extattr(from, مقصد);
    واپس 0;
}


/*
 * The command line arguments in UTF-16
 */
ساکن صحیح	n_دلائل_W = 0;
ساکن LPWSTR	*ArglistW = تہی;
ساکن صحیح	global_argc = 0;
ساکن حرف	**global_argv;

ساکن صحیح	مستعمل_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
ساکن صحیح	*مستعمل_file_indexes = تہی; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
ساکن صحیح	مستعمل_file_گنتی = 0;	// nr of entries in used_file_indexes
ساکن صحیح	مستعمل_file_literal = FALSE;  // take file names literally
ساکن صحیح	مستعمل_file_بھرا_راستہ = FALSE;  // file name was full path
#اگر_تعریف_شدہ FEAT_DIFF
ساکن صحیح	مستعمل_file_diff_منوال = FALSE;  // file name was with diff mode
#اگر_ختم
ساکن صحیح	مستعمل_alist_گنتی = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    صحیح
get_cmd_دلائل_W(حرف ***argvp)
{
    حرف	**argv = تہی;
    صحیح		argc = 0;
    صحیح		i;

    free_cmd_دلائل_W();
    ArglistW = Command_سطر_مقصد_Argv_W(Get_Command_سطر_W(), &n_دلائل_W);
    اگر (ArglistW != تہی)
    {
	argv = alloc((n_دلائل_W + 1) * حجم(حرف *));
	اگر (argv != تہی)
	{
	    argc = n_دلائل_W;
	    argv[argc] = تہی;
	    برائے (i = 0; i < argc; ++i)
	    {
		صحیح	len;

		// Convert each Unicode argument to UTF-8.
		Wide_Char_مقصد_Multi_Byte_alloc(CP_UTF8, 0,
				ArglistW[i], (صحیح)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		اگر (argv[i] == تہی)
		{
		    // Out of memory, clear everything.
		    جب (i > 0)
			آزاد(argv[--i]);
		    آزاد(argv);
		    argv = تہی;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    اگر (argc > 0)
    {
	اگر (مستعمل_file_indexes != تہی)
	    آزاد(مستعمل_file_indexes);
	مستعمل_file_indexes = alloc(argc * حجم(صحیح));
    }

    اگر (argvp != تہی)
	*argvp = argv;
    واپس argc;
}

    خالی
free_cmd_دلائل_W(خالی)
{
    اگر (ArglistW == تہی)
	واپس;

    GlobalFree(ArglistW);
    ArglistW = تہی;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    خالی
مستعمل_file_arg(حرف *name, صحیح literal, صحیح بھرا_راستہ, صحیح diff_منوال)
{
    صحیح		i;

    اگر (مستعمل_file_indexes == تہی)
	واپس;
    برائے (i = مستعمل_file_argc + 1; i < global_argc; ++i)
	اگر (STRCMP(global_argv[i], name) == 0)
	{
	    مستعمل_file_argc = i;
	    مستعمل_file_indexes[مستعمل_file_گنتی++] = i;
	    توڑو;
	}
    مستعمل_file_literal = literal;
    مستعمل_file_بھرا_راستہ = بھرا_راستہ;
#اگر_تعریف_شدہ FEAT_DIFF
    مستعمل_file_diff_منوال = diff_منوال;
#اگر_ختم
}

/*
 * Remember the length of the argument list as it was.  If it changes then we
 * leave it alone when 'encoding' is set.
 */
    خالی
سیٹ_alist_گنتی(خالی)
{
    مستعمل_alist_گنتی = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    خالی
fix_arg_enc(خالی)
{
    صحیح		i;
    صحیح		idx;
    char_u	*str;
    صحیح		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    اگر (global_argc != n_دلائل_W
	    || ArglistW == تہی
	    || مستعمل_file_indexes == تہی
	    || مستعمل_file_گنتی == 0
	    || مستعمل_alist_گنتی != GARGCOUNT)
	واپس;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(صحیح, GARGCOUNT);
    اگر (fnum_list == تہی)
	واپس;		// out of memory
    برائے (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    اگر (ga_grow(&global_alist.al_ga, مستعمل_file_گنتی) == FAIL)
	واپس;		// out of memory

    برائے (i = 0; i < مستعمل_file_گنتی; ++i)
    {
	idx = مستعمل_file_indexes[i];
	str = utf16_مقصد_enc(ArglistW[idx], تہی);
	اگر (str != تہی)
	{
	    صحیح literal = مستعمل_file_literal;

#اگر_تعریف_شدہ FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    اگر (مستعمل_file_diff_منوال && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *دم;
		string_T    ret;

		دم = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), دم, STRLEN(دم), TRUE, &ret);
		اگر (ret.string != تہی)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#اگر_ختم
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    اگر (مستعمل_file_literal)
		buf_سیٹ_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    اگر (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		اگر (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_جوڑو(&global_alist, str, literal ? 2 : 0);
	}
    }

    اگر (!مستعمل_file_literal)
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
	alist_expand(fnum_list, مستعمل_alist_گنتی);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    اگر (curwin->w_arg_idx == 0 && curbuf->b_fname == تہی)
    {
	do_cmdline_cmd((char_u *)":rewind");
	اگر (GARGCOUNT == 1 && مستعمل_file_بھرا_راستہ
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    آخری_chdir_reason = "drop";
    }

    سیٹ_alist_گنتی();
}

    صحیح
mch_setenv(حرف *var, حرف *قدر, صحیح x غیر_مستعمل)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(قدر) + 2);
    اگر (envbuf == تہی)
	واپس -1;

    sprintf((حرف *)envbuf, "%s=%s", var, قدر);

    p = enc_مقصد_utf16(envbuf, تہی);

    vim_free(envbuf);
    اگر (p == تہی)
	واپس -1;
    _wputenv(p);
#اگر_تعریف_شدہ libintl_wputenv
    libintl_wputenv(p);
#اگر_ختم
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    واپس 0;
}

/*
 * Support for 256 colors and 24-bit colors was added in Windows 10
 * version 1703 (Creators update).
 */
#تعریف_کرو VTP_پہلا_SUPPORT_BUILD	    MAKE_VER(10, 0, 15063)

/*
 * Support for pseudo-console (ConPTY) was added in windows 10
 * version 1809 (October 2018 update).
 */
#تعریف_کرو CONPTY_پہلا_SUPPORT_BUILD  MAKE_VER(10, 0, 17763)

/*
 * ConPTY differences between versions, need different logic.
 * version 1903 (May 2019 update).
 */
#تعریف_کرو CONPTY_1903_BUILD	    MAKE_VER(10, 0, 18362)

/*
 * version 1909 (November 2019 update).
 */
#تعریف_کرو CONPTY_1909_BUILD	    MAKE_VER(10, 0, 18363)

/*
 * Stay ahead of the next update, and when it's done, fix this.
 * version ? (2020 update, temporarily use the build number of insider preview)
 */
#تعریف_کرو CONPTY_اگلا_UPDATE_BUILD    MAKE_VER(10, 0, 19587)

/*
 * Confirm until this version.  Also the logic changes.
 * insider preview.
 */
#تعریف_کرو CONPTY_INSIDER_BUILD	    MAKE_VER(10, 0, 18995)

/*
 * Make conpty default on Windows 11
 */
#تعریف_کرو CONPTY_STABLE_BUILD	    MAKE_VER(10, 0, 22000)
// Notes:
// Win 10 22H2 Final is build 19045, its conpty is widely used.
// Strangely, 19045 is newer but is a lower build number than the 2020 insider
// preview which had a build 19587.  And, not sure how stable that was?
// Win Server 2022 (May 10, 2022) is build 20348, its conpty is widely used.
// Win 11 starts from build 22000, even though the major version says 10!

    ساکن خالی
vtp_flag_init(خالی)
{
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   منوال;
    HANDLE  برآمد;

# ifdef VIMDLL
    اگر (!gui.اندر_use)
# endif
    {
	برآمد = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_نسخہ >= VTP_پہلا_SUPPORT_BUILD) ? 1 : 0;
	Get_Console_منوال(برآمد, &منوال);
	منوال |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	اگر (سیٹ_Console_منوال(برآمد, منوال) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_بفر = win_نسخہ >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#اگر_ختم

    اگر (win_نسخہ >= CONPTY_پہلا_SUPPORT_BUILD)
	conpty_working = 1;
    اگر (win_نسخہ >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    اگر (win_نسخہ <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    اگر (win_نسخہ <= CONPTY_1909_BUILD)
	conpty_type = 2;
    اگر (win_نسخہ <= CONPTY_1903_BUILD)
	conpty_type = 2;
    اگر (win_نسخہ < CONPTY_پہلا_SUPPORT_BUILD)
	conpty_type = 1;

    اگر (win_نسخہ >= CONPTY_اگلا_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ساکن خالی
vtp_init(خالی)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_بفر_INFOEX csbi;
    csbi.cb_سائز = حجم(csbi);
    Get_Console_Screen_بفر_Info_Ex(g_h_Con_برآمد, &csbi);
    save_console_bg_rgb = (guicolor_T)csbi.Color_جدول[g_color_اشاریہ_bg];
    save_console_fg_rgb = (guicolor_T)csbi.Color_جدول[g_color_اشاریہ_fg];
    store_console_bg_rgb = save_console_bg_rgb;
    store_console_fg_rgb = save_console_fg_rgb;

    COLORREF bg;
    bg = (COLORREF)csbi.Color_جدول[g_color_اشاریہ_bg];
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);
    default_console_color_bg = bg;

    COLORREF fg;
    fg = (COLORREF)csbi.Color_جدول[g_color_اشاریہ_fg];
    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    default_console_color_fg = fg;
# endif
    سیٹ_console_color_rgb();
}

    ساکن خالی
vtp_exit(خالی)
{
    restore_console_color_rgb();
}

    صحیح
vtp_printf(
    حرف *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   نتیجہ;
    صحیح	    len;

    اگر (silent_منوال)
	واپس 0;

    va_start(list, format);
    len = vim_vsnprintf((حرف *)buf, 100, (حرف *)format, list);
    va_end(list);
    WriteConsoleA(g_h_Con_برآمد, buf, (DWORD)len, &نتیجہ, تہی);
    واپس (صحیح)نتیجہ;
}

    ساکن خالی
vtp_sgr_bulk(
    صحیح arg)
{
    صحیح دلائل[1];

    دلائل[0] = arg;
    vtp_sgr_bulks(1, دلائل);
}

# define FAST256(x) \
    اگر ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    حالت x: \
	FAST256(newargs[x - 1]);

    ساکن خالی
vtp_sgr_bulks(
    صحیح argc,
    صحیح *دلائل)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    صحیح	    اندر, برآمد;
    صحیح	    newargs[16];
    ساکن صحیح sgrfgr = -1, sgrfgg, sgrfgb;
    ساکن صحیح sgrbgr = -1, sgrbgg, sgrbgb;

    اگر (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	واپس;
    }

    اندر = برآمد = 0;
    جب (اندر < argc)
    {
	صحیح s = دلائل[اندر];
	صحیح copylen = 1;

	اگر (s == 38)
	{
	    اگر (argc - اندر >= 5 && دلائل[اندر + 1] == 2)
	    {
		اگر (sgrfgr == دلائل[اندر + 2] && sgrfgg == دلائل[اندر + 3]
						     && sgrfgb == دلائل[اندر + 4])
		{
		    اندر += 5;
		    copylen = 0;
		}
		ورنہ
		{
		    sgrfgr = دلائل[اندر + 2];
		    sgrfgg = دلائل[اندر + 3];
		    sgrfgb = دلائل[اندر + 4];
		    copylen = 5;
		}
	    }
	    ورنہ اگر (argc - اندر >= 3 && دلائل[اندر + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	ورنہ اگر (s == 48)
	{
	    اگر (argc - اندر >= 5 && دلائل[اندر + 1] == 2)
	    {
		اگر (sgrbgr == دلائل[اندر + 2] && sgrbgg == دلائل[اندر + 3]
						     && sgrbgb == دلائل[اندر + 4])
		{
		    اندر += 5;
		    copylen = 0;
		}
		ورنہ
		{
		    sgrbgr = دلائل[اندر + 2];
		    sgrbgg = دلائل[اندر + 3];
		    sgrbgb = دلائل[اندر + 4];
		    copylen = 5;
		}
	    }
	    ورنہ اگر (argc - اندر >= 3 && دلائل[اندر + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	ورنہ اگر (30 <= s && s <= 39)
	    sgrfgr = -1;
	ورنہ اگر (90 <= s && s <= 97)
	    sgrfgr = -1;
	ورنہ اگر (40 <= s && s <= 49)
	    sgrbgr = -1;
	ورنہ اگر (100 <= s && s <= 107)
	    sgrbgr = -1;
	ورنہ اگر (s == 0)
	    sgrfgr = sgrbgr = -1;

	جب (copylen--)
	    newargs[برآمد++] = دلائل[اندر++];
    }

    p = &buf[حجم(buf) - 1];
    *p-- = 'm';

    انتخاب (برآمد)
    {
	صحیح	n, m;
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
	WriteConsoleA(g_h_Con_برآمد, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, تہی);
    طےشدہ:
	توڑو;
    }
}

    ساکن خالی
wt_init(خالی)
{
    wt_working = mch_getenv("WT_SESSION") != تہی;
}

# ifdef FEAT_TERMGUICOLORS
    ساکن صحیح
ctermtoxterm(
    صحیح cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    واپس (((صحیح)r << 16) | ((صحیح)g << 8) | (صحیح)b);
}
# endif

    ساکن خالی
سیٹ_console_color_rgb(خالی)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_بفر_INFOEX csbi;
    guicolor_T	fg, bg;
    صحیح		ctermfg, ctermbg;

    اگر (!vtp_working)
	واپس;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    اگر (p_tgc || t_colors >= 256)
    {
	رکن_fg_rgb_color(fg);
	رکن_bg_rgb_color(bg);
	واپس;
    }

    اگر (use_alternate_screen_بفر)
	واپس;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cb_سائز = حجم(csbi);
    Get_Console_Screen_بفر_Info_Ex(g_h_Con_برآمد, &csbi);

    csbi.cb_سائز = حجم(csbi);
    csbi.sr_کھڑکی.دایاں_3 += 1;
    csbi.sr_کھڑکی.Bottom += 1;
    store_console_bg_rgb = csbi.Color_جدول[g_color_اشاریہ_bg];
    store_console_fg_rgb = csbi.Color_جدول[g_color_اشاریہ_fg];
    csbi.Color_جدول[g_color_اشاریہ_bg] = (COLORREF)bg;
    csbi.Color_جدول[g_color_اشاریہ_fg] = (COLORREF)fg;
    سیٹ_Console_Screen_بفر_Info_Ex(g_h_Con_برآمد, &csbi);
# endif
}

# اگر defined(FEAT_TERMGUICOLORS)
    خالی
get_default_console_color(
    صحیح *cterm_fg,
    صحیح *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    صحیح id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    صحیح ctermfg = 0;
    صحیح ctermbg = 0;
    صحیح dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    اگر (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    اگر (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	اگر (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	اگر (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	ورنہ
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_معمول_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    اگر (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	اگر (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	اگر (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	ورنہ
	    guibg = USE_WT ? INVALCOLOR : default_console_color_bg;
	cterm_معمول_bg_gui_color = guibg;
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
    ساکن خالی
reset_console_color_rgb(خالی)
{
# ifdef FEAT_TERMGUICOLORS
    اگر (use_alternate_screen_بفر)
	واپس;

    CONSOLE_SCREEN_بفر_INFOEX csbi;

    csbi.cb_سائز = حجم(csbi);
    Get_Console_Screen_بفر_Info_Ex(g_h_Con_برآمد, &csbi);

    csbi.cb_سائز = حجم(csbi);
    csbi.sr_کھڑکی.دایاں_3 += 1;
    csbi.sr_کھڑکی.Bottom += 1;
    csbi.Color_جدول[g_color_اشاریہ_bg] = (COLORREF)store_console_bg_rgb;
    csbi.Color_جدول[g_color_اشاریہ_fg] = (COLORREF)store_console_fg_rgb;
    سیٹ_Console_Screen_بفر_Info_Ex(g_h_Con_برآمد, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    ساکن خالی
restore_console_color_rgb(خالی)
{
# ifdef FEAT_TERMGUICOLORS
    اگر (use_alternate_screen_بفر)
	واپس;

    CONSOLE_SCREEN_بفر_INFOEX csbi;

    csbi.cb_سائز = حجم(csbi);
    Get_Console_Screen_بفر_Info_Ex(g_h_Con_برآمد, &csbi);

    csbi.cb_سائز = حجم(csbi);
    csbi.sr_کھڑکی.دایاں_3 += 1;
    csbi.sr_کھڑکی.Bottom += 1;
    csbi.Color_جدول[g_color_اشاریہ_bg] = (COLORREF)save_console_bg_rgb;
    csbi.Color_جدول[g_color_اشاریہ_fg] = (COLORREF)save_console_fg_rgb;
    سیٹ_Console_Screen_بفر_Info_Ex(g_h_Con_برآمد, &csbi);
# endif
}

    خالی
control_console_color_rgb(خالی)
{
    اگر (vtp_working)
	سیٹ_console_color_rgb();
    ورنہ
	reset_console_color_rgb();
}

    صحیح
use_vtp(خالی)
{
    واپس USE_VTP;
}

    صحیح
is_رکن_win32(خالی)
{
    واپس T_NAME != تہی && STRCMP(T_NAME, "win32") == 0;
}

    صحیح
has_vtp_working(خالی)
{
    واپس vtp_working;
}

#اگر_ختم

    صحیح
has_conpty_working(خالی)
{
    واپس conpty_working;
}

    صحیح
get_conpty_type(خالی)
{
    واپس conpty_type;
}

    صحیح
is_conpty_stable(خالی)
{
    واپس conpty_stable;
}

    صحیح
get_conpty_fix_type(خالی)
{
    واپس conpty_fix_type;
}

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    خالی
resize_console_buf(خالی)
{
    اگر (use_alternate_screen_بفر)
	واپس;

    CONSOLE_SCREEN_بفر_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    اگر (!Get_Console_Screen_بفر_Info(g_h_Con_برآمد, &csbi))
	واپس;

    coord.X = SRWIDTH(csbi.sr_کھڑکی);
    coord.Y = SRHEIGHT(csbi.sr_کھڑکی);
    سیٹ_Console_Screen_بفر_سائز(g_h_Con_برآمد, coord);

    newsize.بایاں_3 = 0;
    newsize.چوٹی_2 = 0;
    newsize.دایاں_3 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    سیٹ_Console_کھڑکی_Info(g_h_Con_برآمد, TRUE, &newsize);

    سیٹ_Console_Screen_بفر_سائز(g_h_Con_برآمد, coord);
}
#اگر_ختم

    حرف *
GetWin32Error(خالی)
{
    ساکن حرف	*oldmsg = تہی;
    حرف	*acp_پیغام = تہی;
    DWORD	acp_len;
    حرف	*enc_پیغام = تہی;
    صحیح		enc_len = 0;

    // get formatted message from OS
    acp_len = Format_پیغام(
	    FORMAT_پیغام_ALLOCATE_بفر|FORMAT_پیغام_FROM_SYSTEM,
	    تہی, Get_آخری_Error(), 0, (LPSTR)&acp_پیغام, 0, تہی);
    اگر (acp_len == 0 || acp_پیغام == تہی)
	واپس تہی;

    // clean oldmsg if remained.
    اگر (oldmsg != تہی)
    {
	vim_free(oldmsg);
	oldmsg = تہی;
    }

    acp_مقصد_enc((char_u *)acp_پیغام, (صحیح)acp_len, (char_u **)&enc_پیغام, &enc_len);
    LocalFree(acp_پیغام);
    اگر (enc_پیغام == تہی)
	واپس تہی;

    // remove trailing \r\n
    حرف *pcrlf = لڑی_تلاش(enc_پیغام, "\r\n");
    اگر (pcrlf != تہی)
	*pcrlf = NUL;

    oldmsg = enc_پیغام;
    واپس enc_پیغام;
}

#اگر defined(FEAT_RELTIME)
ساکن HANDLE   timer_handle;
ساکن صحیح      timer_فعال = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
ساکن sig_atomic_t timeout_flags[2];
ساکن صحیح	    timeout_flag_idx = 0;
ساکن sig_atomic_t *timeout_flag = &timeout_flags[0];


    ساکن خالی CALLBACK
سیٹ_flag(خالی *param, BOOLEAN unused2 غیر_مستعمل)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    خالی
stop_timeout(خالی)
{
    اگر (timer_فعال)
    {
	BOOL ret = Delete_Timer_قطار_Timer(تہی, timer_handle, تہی);
	timer_فعال = FALSE;
	اگر (!ret && Get_آخری_Error() != ERROR_IO_PENDING)
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
    متلون sig_atomic_t *
آغاز_timeout(طویل msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = Create_Timer_قطار_Timer(
	    &timer_handle, تہی, سیٹ_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    اگر (!ret)
    {
	semsg(_(e_could_not_سیٹ_timeout_str), GetWin32Error());
    }
    ورنہ
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_فعال = TRUE;
	*timeout_flag = FALSE;
    }
    واپس timeout_flag;
}
#اگر_ختم
