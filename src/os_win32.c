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

#ضمن "vim.h"

#جے_تعریف_شدہ FEAT_MZSCHEME
# include "if_mzsch.h"
#جے_انت

#ضمن <sys/types.h>
#ضمن <signal.h>
#ضمن <limits.h>

#ضمن <process.h>
#ضمن <winternl.h>
#ضمن <direct.h>

#إذا !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#جے_انت

#جے_تعریف_شدہ FEAT_کم_CHANNEL
# include <tlhelp32.h>
#جے_انت

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#جے_تعریف_شدہ MCH_WRITE_DUMP
FILE* fdDump = عدم;
#جے_انت

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
ساكن HANDLE g_h_Con_وچ  = INVALID_HANDLE_قدر;
ساكن HANDLE g_h_Con_باہرلا = INVALID_HANDLE_قدر;

// Win32 Screen buffer,coordinate,console I/O information
ساكن SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
ساكن COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
ساكن لفظ_2  g_attrDefault = 7;  // lightgray text on black background
ساكن لفظ_2  g_attrCurrent;

ساكن صحيح g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
ساكن صحيح g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
ساكن صحيح g_fForceExit = FALSE;    // set when forcefully exiting

ساكن فراغ scroll(بنانشانی c_سطریں);
ساكن فراغ سیٹ_scroll_region(بنانشانی بایاں, بنانشانی چوٹی,
			      بنانشانی دایاں, بنانشانی bottom);
ساكن فراغ سیٹ_scroll_region_tb(بنانشانی چوٹی, بنانشانی bottom);
ساكن فراغ سیٹ_scroll_region_lr(بنانشانی بایاں, بنانشانی دایاں);
ساكن فراغ داخل_کرو_سطریں(بنانشانی c_سطریں);
ساكن فراغ delete_سطریں(بنانشانی c_سطریں);
ساكن فراغ gotoxy(بنانشانی x, بنانشانی y);
ساكن فراغ standout(فراغ);
ساكن صحيح s_cursor_visible = TRUE;
ساكن صحيح did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
تعريفنوع بنية داخلہ_ریکارڈ_بفر_گرہ_S
{
    داخلہ_ریکارڈ ir;
    بنية داخلہ_ریکارڈ_بفر_گرہ_S *اگلا;
} داخلہ_ریکارڈ_بفر_گرہ_T;
تعريفنوع بنية داخلہ_ریکارڈ_بفر_S
{
    داخلہ_ریکارڈ_بفر_گرہ_T *سر;
    داخلہ_ریکارڈ_بفر_گرہ_T *پوچھ;
    صحيح لمائی;
} داخلہ_ریکارڈ_بفر_T;
ساكن داخلہ_ریکارڈ_بفر_T داخلہ_ریکارڈ_بفر;
ساكن صحيح read_داخلہ_ریکارڈ_بفر(داخلہ_ریکارڈ* irEvents, صحيح n_Max_لمائی);
ساكن صحيح write_داخلہ_ریکارڈ_بفر(داخلہ_ریکارڈ* irEvents, صحيح n_لمائی);
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_MSWIN
ساكن صحيح s_dont_use_vimrun = TRUE;
ساكن صحيح need_vimrun_warning = FALSE;
ساكن string_T vimrun_رستہ = {(char_u *)"vimrun ", 7};
ساكن صحيح vimrun_رستہ_allocated = FALSE;
#جے_انت

ساكن صحيح win32_getattrs(char_u *name);
ساكن صحيح win32_setattrs(char_u *name, صحيح attrs);
ساكن صحيح win32_سیٹ_archive(char_u *name);

ساكن صحيح conpty_working = 0;
ساكن صحيح conpty_type = 0;
ساكن صحيح conpty_stable = 0;
ساكن صحيح conpty_fix_type = 0;
ساكن فراغ vtp_flag_init(فراغ);

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساكن صحيح vtp_working = 0;
ساكن فراغ vtp_init(فراغ);
ساكن فراغ vtp_exit(فراغ);
ساكن فراغ vtp_sgr_bulk(صحيح arg);
ساكن فراغ vtp_sgr_bulks(صحيح argc, صحيح *argv);

ساكن صحيح wt_working = 0;
ساكن فراغ wt_init(فراغ);

ساكن صحيح g_color_اشاریہ_bg = 0;
ساكن صحيح g_color_اشاریہ_fg = 7;

# ifdef FEAT_TERMGUICOLORS
ساكن guicolor_T save_console_bg_rgb;
ساكن guicolor_T save_console_fg_rgb;
ساكن guicolor_T store_console_bg_rgb;
ساكن guicolor_T store_console_fg_rgb;
ساكن صحيح default_console_color_bg = 0x000000; // black
ساكن صحيح default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_رکن_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# وإلا
#  define USE_VTP		0
#  define USE_WT		0
# endif

ساكن فراغ سیٹ_console_color_rgb(فراغ);
ساكن فراغ reset_console_color_rgb(فراغ);
ساكن فراغ restore_console_color_rgb(فراغ);
#جے_انت  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#جے_بنا_تعریف ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#جے_انت

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساكن صحيح suppress_winsize = 1;	// don't fiddle with console
#جے_انت

ساكن WCHAR *exe_pathw = عدم;

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساكن BOOL use_alternate_screen_بفر = FALSE;
#جے_انت

خارجي DWORD win_نسخہ; // this is in os_mswin.c

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ساكن BOOL
is_ambiwidth_event(
    داخلہ_ریکارڈ *ir)
{
    أرجع ir->EventType == کلید_EVENT
		&& ir->Event.کلید_Event_2.b_کلید_تھلے
		&& ir->Event.کلید_Event_2.w_Repeat_گنتی == 1
		&& ir->Event.کلید_Event_2.w_Virtual_کلید_کوڈ == 0x12
		&& ir->Event.کلید_Event_2.w_Virtual_جانچ_کوڈ == 0x38
		&& ir->Event.کلید_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.کلید_Event_2.dw_Control_کلید_State == 2;
}

    ساكن فراغ
make_ambiwidth_event(
    داخلہ_ریکارڈ *تھلے,
    داخلہ_ریکارڈ *اتے)
{
    تھلے->Event.کلید_Event_2.w_Virtual_کلید_کوڈ = 0;
    تھلے->Event.کلید_Event_2.w_Virtual_جانچ_کوڈ = 0;
    تھلے->Event.کلید_Event_2.uChar.UnicodeChar
				    = اتے->Event.کلید_Event_2.uChar.UnicodeChar;
    تھلے->Event.کلید_Event_2.dw_Control_کلید_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    ساكن BOOL
read_console_داخلہ(
    HANDLE	    h_داخلہ,
    داخلہ_ریکارڈ    *lp_بفر,
    صحيح		    n_لمائی,
    LPDWORD	    lpEvents)
{
    تعداد
    {
	IRSIZE = 10
    };
    ساكن داخلہ_ریکارڈ s_irCache[IRSIZE];
    ساكن DWORD s_dw_اشاریہ = 0;
    ساكن DWORD s_dwMax = 0;
    DWORD dwEvents;
    صحيح سر;
    صحيح پوچھ;
    صحيح i;
    ساكن داخلہ_ریکارڈ s_irPseudo;

    إذا (s_dwMax == 0 && داخلہ_ریکارڈ_بفر.لمائی > 0)
    {
	dwEvents = read_داخلہ_ریکارڈ_بفر(s_irCache, IRSIZE);
	s_dw_اشاریہ = 0;
	s_dwMax = dwEvents;
    }

    إذا (n_لمائی == -2)
	أرجع (s_dwMax > 0) ? TRUE : FALSE;

    إذا (win_نسخہ < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	إذا (n_لمائی == -1)
	    أرجع Peek_Console_داخلہ_W(h_داخلہ, lp_بفر, 1, lpEvents);
	أرجع Read_Console_داخلہ_W(h_داخلہ, lp_بفر, 1, &dwEvents);
    }

    إذا (s_dwMax == 0)
    {
	إذا (!vtp_working && n_لمائی == -1)
	    أرجع Peek_Console_داخلہ_W(h_داخلہ, lp_بفر, 1, lpEvents);
	Get_نمبر_دا_Console_داخلہ_Events(h_داخلہ, &dwEvents);
	إذا (dwEvents == 0 && n_لمائی == -1)
	    أرجع Peek_Console_داخلہ_W(h_داخلہ, lp_بفر, 1, lpEvents);
	Read_Console_داخلہ_W(h_داخلہ, s_irCache, IRSIZE, &dwEvents);
	s_dw_اشاریہ = 0;
	s_dwMax = dwEvents;
	إذا (dwEvents == 0)
	{
	    *lpEvents = 0;
	    أرجع TRUE;
	}

	لكل (i = s_dw_اشاریہ; i < (صحيح)s_dwMax - 1; ++i)
	    إذا (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	إذا (s_dwMax > 1)
	{
	    سر = 0;
	    پوچھ = s_dwMax - 1;
	    طالما (سر != پوچھ)
	    {
		إذا (s_irCache[سر].EventType == کھڑکی_بفر_ناپ_EVENT
			&& s_irCache[سر + 1].EventType
						  == کھڑکی_بفر_ناپ_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    لكل (i = سر; i < پوچھ; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --پوچھ;
		    استمر;
		}
		سر++;
	    }
	    s_dwMax = پوچھ + 1;
	}
    }

    إذا (s_irCache[s_dw_اشاریہ].EventType == کلید_EVENT)
    {
	إذا (s_irCache[s_dw_اشاریہ].Event.کلید_Event_2.w_Repeat_گنتی > 1)
	{
	    s_irPseudo = s_irCache[s_dw_اشاریہ];
	    s_irPseudo.Event.کلید_Event_2.w_Repeat_گنتی = 1;
	    s_irCache[s_dw_اشاریہ].Event.کلید_Event_2.w_Repeat_گنتی--;
	    *lp_بفر = s_irPseudo;
	    *lpEvents = 1;
	    أرجع TRUE;
	}
    }

    *lp_بفر = s_irCache[s_dw_اشاریہ];
    إذا (!(n_لمائی == -1 || n_لمائی == -2) && ++s_dw_اشاریہ >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    أرجع TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    ساكن BOOL
peek_console_داخلہ(
    HANDLE	    h_داخلہ,
    داخلہ_ریکارڈ    *lp_بفر,
    DWORD	    n_لمائی بن_ورتیا,
    LPDWORD	    lpEvents)
{
    أرجع read_console_داخلہ(h_داخلہ, lp_بفر, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    ساكن DWORD
پیغام_wait_for_مضرب_objects(
    DWORD    n_گنتی,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dw_Wake_نقاب)
{
    إذا (read_console_داخلہ(عدم, عدم, -2, عدم))
	أرجع WAIT_OBJECT_0;
    أرجع پیغام_Wait_For_مضرب_Objects(n_گنتی, pHandles, fWaitAll,
				     dwMilliseconds, dw_Wake_نقاب);
}
# endif

# ifndef FEAT_CLIENTSERVER
    ساكن DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    إذا (read_console_داخلہ(عدم, عدم, -2, عدم))
	أرجع WAIT_OBJECT_0;
    أرجع WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#جے_انت   // !FEAT_GUI_MSWIN || VIMDLL

    فراغ
mch_get_exe_name(فراغ)
{
    صحيح		updated = FALSE;
    ساكن صحيح	enc_prev = -1;
    WCHAR	*رستہ;
    size_t	pathlen;

    إذا (exe_name == عدم || exe_pathw == عدم || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_رستہ];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(عدم, buf, MAX_رستہ);
	إذا (p > buf)
	{
	    إذا (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_منزل_enc(buf, عدم);
	    enc_prev = enc_codepage;

	    // truncate the buffer at the last path separator
	    // to isolate the path.
	    افعل
	    {
		--p;
		إذا (*p == L'\\')
		{
		    *p = L'\0';
		    اكسر;
		}
	    } طالما (p > buf);

	    vim_free(exe_pathw);
	    exe_pathw = _wcsdup(buf);
	    updated = TRUE;
	}
    }

    إذا (exe_pathw == عدم || !updated)
	أرجع;

    // Append our starting directory to $PATH, so that when doing
    // "!xxd" it's found in our starting directory.  Needed because
    // SearchPath() also looks there.
    رستہ = _wgetenv(L"PATH");
    pathlen = (رستہ == عدم) ? 0 : wcslen(رستہ);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#تعریف_کرو EXTRA_LEN 7
#تعریف_کرو MAX_ENV_STRING_LEN 8191

    إذا (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   عارضی[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	إذا (pathlen == 0)
	    wcscpy(عارضی + templen, exe_pathw);
	وإلا
	{
	    wcscpy(عارضی + templen, رستہ);

	    // Check if exe_path is already included in $PATH.
	    إذا (wcsstr(عارضی, exe_pathw) == عدم)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		إذا (عارضی[templen - 1] != L';')
		{
		    wcscpy(عارضی + templen, L";");
		    ++templen;
		}

		wcscpy(عارضی + templen, exe_pathw);
	    }
	}

	_wputenv(عارضی);
#جے_تعریف_شدہ libintl_wputenv
	libintl_wputenv(عارضی);
#جے_انت
    }
#تعریف_رد EXTRA_LEN
#تعریف_رد MAX_ENV_رستہ_LEN
}

/*
 * Unescape characters in "p" that appear in "escaped".
 */
    ساكن فراغ
unescape_shellxquote(char_u *p, char_u *escaped)
{
    صحيح	    l = (صحيح)STRLEN(p);
    صحيح	    n;

    طالما (*p != NUL)
    {
	إذا (*p == '^' && vim_strchr(escaped, p[1]) != عدم)
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
vim_لدو_Lib(ثابت حرف *name)
{
    HINSTANCE	dll = عدم;

    // No need to load any library when registering OLE.
    إذا (لبھیا_رجسٹر_arg)
	أرجع عدم;

    // NOTE: Do not use mch_dirname() and mch_chdir() here, they may call
    // vimLoadLib() recursively, which causes a stack overflow.
    إذا (exe_pathw == عدم)
    {
	mch_get_exe_name();
	إذا (exe_pathw == عدم)
	    أرجع عدم;
    }

    WCHAR old_dirw[MAXPATHL];

    إذا (GetCurrentDirectoryW(MAXPATHL, old_dirw) == 0)
	أرجع عدم;

    // Change directory to where the executable is, both to make
    // sure we find a .dll there and to avoid looking for a .dll
    // in the current directory.
    سیٹ_Current_Directory_W(exe_pathw);
    dll = لدو_Library(name);
    سیٹ_Current_Directory_W(old_dirw);
    أرجع dll;
}

#إذا defined(VIMDLL)
/*
 * Check if the current executable file is for the GUI subsystem.
 */
    صحيح
mch_is_gui_executable(فراغ)
{
    PBYTE		pImage = (PBYTE)GetModuleHandle(عدم);
    PIMAGE_DOS_سرخی	pDOS = (PIMAGE_DOS_سرخی)pImage;
    PIMAGE_NT_سرخیاں	pPE;

    إذا (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	أرجع FALSE;
    pPE = (PIMAGE_NT_سرخیاں)(pImage + pDOS->e_lfanew);
    إذا (pPE->Signature != IMAGE_NT_SIGNATURE)
	أرجع FALSE;
    إذا (pPE->Optional_سرخی.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	أرجع TRUE;
    أرجع FALSE;
}
#جے_انت

#إذا defined(DYNAMIC_ICONV) || defined(DYNAMIC_GETTEXT) \
    || defined(FEAT_PYTHON3)
/*
 * Get related information about 'funcname' which is imported by 'hInst'.
 * If 'info' is 0, return the function address.
 * If 'info' is 1, return the module name which the function is imported from.
 * If 'info' is 2, hook the function with 'ptr', and return the original
 * function address.
 */
    ساكن فراغ *
get_imported_func_info(HINSTANCE hInst, ثابت حرف *funcname, صحيح info,
	ثابت فراغ *ptr)
{
    PBYTE			pImage = (PBYTE)hInst;
    PIMAGE_DOS_سرخی		pDOS = (PIMAGE_DOS_سرخی)hInst;
    PIMAGE_NT_سرخیاں		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_انکڑے		pIAT;	    // Import Address Table
    PIMAGE_THUNK_انکڑے		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    إذا (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	أرجع عدم;
    pPE = (PIMAGE_NT_سرخیاں)(pImage + pDOS->e_lfanew);
    إذا (pPE->Signature != IMAGE_NT_SIGNATURE)
	أرجع عدم;

    ImpVA = pPE->Optional_سرخی
		.انکڑے_Directory[IMAGE_DIRECTORY_اندراج_IMPORT].Virtual_پتہ;
    إذا (ImpVA == 0)
	أرجع عدم;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    لكل (; pImpDesc->پہلا_Thunk; ++pImpDesc)
    {
	إذا (!pImpDesc->Original_پہلا_Thunk)
	    استمر;
	pIAT = (PIMAGE_THUNK_انکڑے)(pImage + pImpDesc->پہلا_Thunk);
	pINT = (PIMAGE_THUNK_انکڑے)(pImage + pImpDesc->Original_پہلا_Thunk);
	لكل (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    إذا (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		استمر;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.پتہ_دا_انکڑے));
	    إذا (strcmp((حرف *)pImpName->Name, funcname) == 0)
	    {
		فراغ *original;
		DWORD old, new = ورقہ_READWRITE;

		بدل (info)
		{
		    حالة 0:
			أرجع (فراغ *)pIAT->u1.Function;
		    حالة 1:
			أرجع (فراغ *)(pImage + pImpDesc->Name);
		    حالة 2:
			original = (فراغ *)pIAT->u1.Function;
			VirtualProtect(&pIAT->u1.Function, حجم(فراغ *),
				new, &old);
			pIAT->u1.Function = (UINT_PTR)ptr;
			VirtualProtect(&pIAT->u1.Function, حجم(فراغ *),
				old, &new);
			أرجع original;
		    افتراضي:
			أرجع عدم;
		}
	    }
	}
    }
    أرجع عدم;
}

/*
 * Get the module handle which 'funcname' in 'hInst' is imported from.
 */
    HINSTANCE
لبھو_imported_module_by_funcname(HINSTANCE hInst, ثابت حرف *funcname)
{
    حرف    *modulename;

    modulename = (حرف *)get_imported_func_info(hInst, funcname, 1, عدم);
    إذا (modulename != عدم)
	أرجع GetModuleHandleA(modulename);
    أرجع عدم;
}

/*
 * Get the address of 'funcname' which is imported by 'hInst' DLL.
 */
    فراغ *
get_dll_import_func(HINSTANCE hInst, ثابت حرف *funcname)
{
    أرجع get_imported_func_info(hInst, funcname, 0, عدم);
}

/*
 * Hook the function named 'funcname' which is imported by 'hInst' DLL,
 * and return the original function address.
 */
    فراغ *
hook_dll_import_func(HINSTANCE hInst, ثابت حرف *funcname, ثابت فراغ *hook)
{
    أرجع get_imported_func_info(hInst, funcname, 2, hook);
}
#جے_انت

#إذا defined(FEAT_PYTHON3)
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
    DWORD			Exp_ناپ;
    LPDWORD			p_Function_جدول;

    إذا (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	أرجع عدم;
    pPE = (PIMAGE_NT_سرخیاں)(pImage + pDOS->e_lfanew);
    إذا (pPE->Signature != IMAGE_NT_SIGNATURE)
	أرجع عدم;

    ExpVA = pPE->Optional_سرخی
		.انکڑے_Directory[IMAGE_DIRECTORY_اندراج_EXPORT].Virtual_پتہ;
    Exp_ناپ = pPE->Optional_سرخی
		.انکڑے_Directory[IMAGE_DIRECTORY_اندراج_EXPORT].ناپ_2;
    إذا (ExpVA == 0)
	أرجع hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    p_Function_جدول = (LPDWORD)(pImage + pExpDir->پتہ_دا_Functions);

    إذا (pExpDir->نمبر_دا_ناں == 0)
	أرجع hInst;	// No export names.

    // Check only the first entry.
    إذا ((p_Function_جدول[0] < ExpVA) || (p_Function_جدول[0] >= ExpVA + Exp_ناپ))
	// The first entry is not a function forwarder.
	أرجع hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    ثابت حرف *name = (ثابت حرف *)(pImage + p_Function_جدول[0]);
    ثابت حرف *p = strchr(name, '.');
    إذا (p == عدم)
	أرجع hInst;

    // Extract DllName.
    حرف buf[MAX_رستہ];
    إذا (p - name + 1 > حجم(buf))
	أرجع عدم;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    أرجع GetModuleHandleA(buf);
}
#جے_انت

#إذا defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
ساكن حرف *null_libintl_gettext(ثابت حرف *);
ساكن حرف *null_libintl_ngettext(ثابت حرف *, ثابت حرف *, بنانشانی لما n);
ساكن حرف *null_libintl_textdomain(ثابت حرف *);
ساكن حرف *null_libintl_bindtextdomain(ثابت حرف *, ثابت حرف *);
ساكن حرف *null_libintl_bind_textdomain_codeset(ثابت حرف *, ثابت حرف *);
ساكن صحيح null_libintl_wputenv(ثابت wchar_t *);

ساكن HINSTANCE hLibintlDLL = عدم;
حرف *(*dyn_libintl_gettext)(ثابت حرف *) = null_libintl_gettext;
حرف *(*dyn_libintl_ngettext)(ثابت حرف *, ثابت حرف *, بنانشانی لما n)
						= null_libintl_ngettext;
حرف *(*dyn_libintl_textdomain)(ثابت حرف *) = null_libintl_textdomain;
حرف *(*dyn_libintl_bindtextdomain)(ثابت حرف *, ثابت حرف *)
						= null_libintl_bindtextdomain;
حرف *(*dyn_libintl_bind_textdomain_codeset)(ثابت حرف *, ثابت حرف *)
				       = null_libintl_bind_textdomain_codeset;
صحيح (*dyn_libintl_wputenv)(ثابت wchar_t *) = null_libintl_wputenv;
صحيح *dyn_libintl_nl_پیغام_cat_cntr = عدم;

    صحيح
dyn_libintl_init(فراغ)
{
    صحيح i;
    ساكن بنية
    {
	حرف	    *name;
	FARPROC	    *ptr;
    } libintl_اندراج[] =
    {
	{"gettext", (FARPROC*)&dyn_libintl_gettext},
	{"ngettext", (FARPROC*)&dyn_libintl_ngettext},
	{"textdomain", (FARPROC*)&dyn_libintl_textdomain},
	{"bindtextdomain", (FARPROC*)&dyn_libintl_bindtextdomain},
	{عدم, عدم}
    };
    HINSTANCE hmsvcrt;

    // No need to initialize twice.
    إذا (hLibintlDLL != عدم)
	أرجع 1;
    // Load gettext library (libintl.dll and other names).
    hLibintlDLL = vim_لدو_Lib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    إذا (!hLibintlDLL)
	hLibintlDLL = vim_لدو_Lib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    إذا (!hLibintlDLL)
	hLibintlDLL = vim_لدو_Lib(GETTEXT_DLL_ALT2);
# endif
    إذا (!hLibintlDLL)
    {
	إذا (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_لدو_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	أرجع 0;
    }
    لكل (i = 0; libintl_اندراج[i].name != عدم
					 && libintl_اندراج[i].ptr != عدم; ++i)
    {
	إذا ((*libintl_اندراج[i].ptr = Get_Proc_پتہ(hLibintlDLL,
					      libintl_اندراج[i].name)) == عدم)
	{
	    dyn_libintl_سرا();
	    إذا (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_لدو_library_function_str), libintl_اندراج[i].name);
		verbose_leave();
	    }
	    أرجع 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (حرف *(*)(ثابت حرف *, ثابت حرف *))
			Get_Proc_پتہ(hLibintlDLL, "bind_textdomain_codeset");
    إذا (dyn_libintl_bind_textdomain_codeset == عدم)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = لبھو_imported_module_by_funcname(hLibintlDLL, "getenv");
    إذا (hmsvcrt != عدم)
	dyn_libintl_wputenv = (صحيح (*)(ثابت wchar_t *))
					Get_Proc_پتہ(hmsvcrt, "_wputenv");
    إذا (dyn_libintl_wputenv == عدم || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_پیغام_cat_cntr = (صحيح *)Get_Proc_پتہ(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    أرجع 1;
}

    فراغ
dyn_libintl_سرا(فراغ)
{
    إذا (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= عدم;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_ngettext	= null_libintl_ngettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset = null_libintl_bind_textdomain_codeset;
    dyn_libintl_wputenv		= null_libintl_wputenv;
    dyn_libintl_nl_پیغام_cat_cntr	= عدم;
}

    ساكن حرف *
null_libintl_gettext(ثابت حرف *msgid)
{
    أرجع (حرف*)msgid;
}

    ساكن حرف *
null_libintl_ngettext(
	ثابت حرف *msgid,
	ثابت حرف *msgid_plural,
	بنانشانی لما n)
{
    أرجع (حرف *)(n == 1 ? msgid : msgid_plural);
}

    ساكن حرف *
null_libintl_bindtextdomain(
	ثابت حرف *domainname بن_ورتیا,
	ثابت حرف *dirname بن_ورتیا)
{
    أرجع عدم;
}

    ساكن حرف *
null_libintl_bind_textdomain_codeset(
	ثابت حرف *domainname بن_ورتیا,
	ثابت حرف *codeset بن_ورتیا)
{
    أرجع عدم;
}

    ساكن حرف *
null_libintl_textdomain(ثابت حرف *domainname بن_ورتیا)
{
    أرجع عدم;
}

    ساكن صحيح
null_libintl_wputenv(ثابت wchar_t *envstring بن_ورتیا)
{
    أرجع 0;
}

#جے_انت // DYNAMIC_GETTEXT

// This symbol is not defined in older versions of the SDK or Visual C++

#جے_بنا_تعریف VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#جے_انت

#جے_تعریف_شدہ HAVE_ACL
# include <aclapi.h>
# ifndef PROTECTED_DACL_SECURITY_INFORMATION
#  define PROTECTED_DACL_SECURITY_INFORMATION	0x80000000L
# endif
#جے_انت

#جے_تعریف_شدہ HAVE_ACL
/*
 * Enables or disables the specified privilege.
 */
    ساكن BOOL
win32_enable_privilege(LPTSTR lpszPrivilege)
{
    LUID		luid;
    HANDLE		hToken;
    TOKEN_PRIVILEGES	tokenPrivileges;

    إذا (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	أرجع FALSE;

    إذا (!Lookup_Privilege_قدر(عدم, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	أرجع FALSE;
    }

    tokenPrivileges.Privilege_گنتی	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].گن_2 = SE_PRIVILEGE_ENABLED;

    إذا (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, عدم, عدم))
    {
	CloseHandle(hToken);
	أرجع FALSE;
    }

    إذا (Get_آخری_Error() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	أرجع FALSE;
    }

    CloseHandle(hToken);
    أرجع TRUE;
}
#جے_انت

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    فراغ
PlatformId(فراغ)
{
#جے_تعریف_شدہ FEAT_EVAL
    DWORD major = (win_نسخہ >> 24) & 0xFF;
    DWORD minor = (win_نسخہ >> 16) & 0xFF;
    vim_snprintf(windows_نسخہ, حجم(windows_نسخہ), "%d.%d", major, minor);
#جے_انت

#جے_تعریف_شدہ HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    إذا (!win32_enable_privilege(SE_SECURITY_NAME))
	أرجع;
#جے_انت
}

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define کھسکاؤ_2  (کھسکاؤ_PRESSED)
# define CTRL   (دایاں_CTRL_PRESSED | بایاں_CTRL_PRESSED)
# define ALT    (دایاں_ALT_PRESSED  | بایاں_ALT_PRESSED)
# define ALT_GR (دایاں_ALT_PRESSED  | بایاں_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

ساكن ثابت بنية
{
    لفظ_2    w_Virt_کلید;
    BOOL    f_Ansi_کلید;
    صحيح	    chAlone;
    صحيح	    ch_کھسکاؤ;
    صحيح	    chCtrl;
    صحيح	    chAlt;
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
    { VK_اتے,	TRUE,	'H',	'\304',	'\305',	    '\306', },
    { VK_پہلوں,	TRUE,	'I',	'\307',	'\204',	    '\310', }, // PgUp
    { VK_بایاں,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_دایاں,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_سرا,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_تھلے,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_اگلا,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_داخل_کرو,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_پٹھ,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, // PrtScrn

# إذا 0
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
    { VK_گھٹاؤ, TRUE,'J',	'J',    'J',	'J',	}, // keyp '-'
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
    ساكن صحيح
win32_kbd_patch_کلید(
    کلید_EVENT_ریکارڈ *pker)
{
    UINT uMods = pker->dw_Control_کلید_State;
    ساكن صحيح s_iIsDead = 0;
    ساكن لفظ_2 aw_Ansi_کوڈ[2];
    ساكن BYTE abKeystate[256];

    إذا (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_کوڈ[1];
	s_iIsDead = 0;
	أرجع 1;
    }

    // check if it already has a valid unicode character.
    إذا (pker->uChar.UnicodeChar != 0)
	أرجع 1;

    CLEAR_میدان(abKeystate);

    // Clear any pending dead keys
    منزل_Unicode(VK_SPACE, Map_Virtual_کلید(VK_SPACE, 0), abKeystate, aw_Ansi_کوڈ, 2, 0);

    إذا (uMods & کھسکاؤ_PRESSED)
	abKeystate[VK_کھسکاؤ] = 0x80;
    إذا (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    إذا ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = منزل_Unicode(pker->w_Virtual_کلید_کوڈ, pker->w_Virtual_جانچ_کوڈ,
			abKeystate, aw_Ansi_کوڈ, 2, 0);

    إذا (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_کوڈ[0];

    أرجع s_iIsDead;
}

ساكن BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    ساكن BOOL
decode_کلید_event(
    کلید_EVENT_ریکارڈ	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    صحيح			*pmodifiers,
    BOOL		fDoPost بن_ورتیا)
{
    صحيح i;
    ثابت صحيح nModifs = pker->dw_Control_کلید_State & (کھسکاؤ_2 | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    إذا (!pker->b_کلید_تھلے)
	أرجع FALSE;

    // ignore some keystrokes
    بدل (pker->w_Virtual_کلید_کوڈ)
    {
    // modifiers
    حالة VK_کھسکاؤ:
    حالة VK_CONTROL:
    حالة VK_MENU:   // Alt key
	أرجع FALSE;

    افتراضي:
	اكسر;
    }

    // Shift-TAB
    إذا (pker->w_Virtual_کلید_کوڈ == VK_TAB && (nModifs & کھسکاؤ_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	أرجع TRUE;
    }

    لكل (i = ARRAY_لمائی(Virt_کلید_Map); --i >= 0; )
    {
	إذا (Virt_کلید_Map[i].w_Virt_کلید == pker->w_Virtual_کلید_کوڈ)
	{
	    *pch = Virt_کلید_Map[i].chAlone;
	    إذا ((nModifs & کھسکاؤ_2) != 0)
		*pch = Virt_کلید_Map[i].ch_کھسکاؤ;
	    وإلا إذا ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = Virt_کلید_Map[i].chCtrl;
	    وإلا إذا ((nModifs & ALT) != 0)
		*pch = Virt_کلید_Map[i].chAlt;

	    إذا (*pch != 0)
	    {
		إذا (Virt_کلید_Map[i].f_Ansi_کلید)
		{
		    *pch2 = *pch;
		    *pch = K_NUL;
		    إذا (pmodifiers)
		    {
			إذا (pker->w_Virtual_کلید_کوڈ >= VK_F1
			    && pker->w_Virtual_کلید_کوڈ <= VK_F12)
			{
			    إذا ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_نقاب_ALT;
				إذا ((nModifs & کھسکاؤ_2) == 0)
				    *pch2 = Virt_کلید_Map[i].chAlone;
			    }
			    إذا ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_نقاب_CTRL;
				إذا ((nModifs & کھسکاؤ_2) == 0)
				    *pch2 = Virt_کلید_Map[i].chAlone;
			    }
			}
			وإلا إذا (pker->w_Virtual_کلید_کوڈ >= VK_سرا
				&& pker->w_Virtual_کلید_کوڈ <= VK_تھلے)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_کلید_Map[i].chAlone;
			    إذا ((nModifs & کھسکاؤ_2) != 0
						    && (nModifs & ~کھسکاؤ_2) == 0)
			    {
				*pch2 = Virt_کلید_Map[i].ch_کھسکاؤ;
			    }
			    إذا ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_کلید_Map[i].chCtrl;
				إذا (pker->w_Virtual_کلید_کوڈ == VK_اتے
				    || pker->w_Virtual_کلید_کوڈ == VK_تھلے)
				{
				    *pmodifiers |= MOD_نقاب_CTRL;
				    *pch2 = Virt_کلید_Map[i].chAlone;
				}
			    }
			    إذا ((nModifs & کھسکاؤ_2) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_نقاب_CTRL;
				*pch2 = Virt_کلید_Map[i].ch_کھسکاؤ;
			    }
			    إذا ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_کلید_Map[i].chAlt;
				*pmodifiers |= MOD_نقاب_ALT;
				إذا ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_کلید_Map[i].chAlone;
				}
				وإلا إذا ((nModifs & کھسکاؤ_2) != 0)
				{
				    *pch2 = Virt_کلید_Map[i].ch_کھسکاؤ;
				}
				وإلا إذا ((nModifs & CTRL) != 0)
				{
				    إذا (pker->w_Virtual_کلید_کوڈ == VK_اتے
					|| pker->w_Virtual_کلید_کوڈ == VK_تھلے)
				    {
					*pmodifiers |= MOD_نقاب_CTRL;
					*pch2 = Virt_کلید_Map[i].chAlone;
				    }
				    وإلا
				    {
					*pch2 = Virt_کلید_Map[i].chCtrl;
				    }
				}
			    }
			}
			وإلا إذا (pker->w_Virtual_کلید_کوڈ == VK_داخل_کرو
					&& (nModifs & کھسکاؤ_2) != 0
					&& (nModifs & ~کھسکاؤ_2) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_کلید_Map[i].ch_کھسکاؤ;
			}
			وإلا
			{
			    *pch2 = Virt_کلید_Map[i].chAlone;
			    إذا ((nModifs & کھسکاؤ_2) != 0)
				*pmodifiers |= MOD_نقاب_کھسکاؤ;
			    إذا ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_نقاب_CTRL;
			    إذا ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_نقاب_ALT;
			}
		    }
		}

		أرجع TRUE;
	    }
	}
    }

    i = win32_kbd_patch_کلید(pker);

    إذا (i < 0)
	*pch = NUL;
    وإلا
    {
	*pch = (i > 0) ? pker->uChar.UnicodeChar : NUL;

	إذا (pmodifiers != عدم)
	{
	    // Pass on the ALT key as a modifier, but only when not combined
	    // with CTRL (which is ALTGR).
	    إذا ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
		*pmodifiers |= MOD_نقاب_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    إذا ((nModifs & کھسکاؤ_2) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_نقاب_کھسکاؤ;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    إذا ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_نقاب_CTRL;
	}
    }

    أرجع (*pch != NUL);
}

# إذا defined(FEAT_EVAL)
    ساكن صحيح
encode_کلید_event(dict_T *دلیلاں, داخلہ_ریکارڈ *ir)
{
    ساكن صحيح s_dwMods = 0;

    char_u *action = dict_get_string(دلیلاں, "event", TRUE);
    إذا (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_کلید_تھلے = STRICMP(action, "keydown") == 0;
	لفظ_2 vk_کوڈ = dict_get_نمبر_def(دلیلاں, "keycode", 0);
	إذا (vk_کوڈ <= 0 || vk_کوڈ >= 0xFF)
	{
	    semsg(_(e_invalid_دلیل_nr), (لما)vk_کوڈ);
	    أرجع FALSE;
	}

	ir->EventType = کلید_EVENT;
	کلید_EVENT_ریکارڈ ker;
	Zero_حافظہ(&ker, حجم(ker));
	ker.b_کلید_تھلے = is_کلید_تھلے;
	ker.w_Repeat_گنتی = 1;
	ker.w_Virtual_جانچ_کوڈ = 0;
	ker.dw_Control_کلید_State = 0;
	صحيح mods = (صحيح)dict_get_نمبر(دلیلاں, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	إذا (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    إذا (mods & MOD_نقاب_کھسکاؤ)
		ker.dw_Control_کلید_State |= کھسکاؤ_PRESSED;
	    إذا (mods & MOD_نقاب_CTRL)
		ker.dw_Control_کلید_State |= بایاں_CTRL_PRESSED;
	    إذا (mods & MOD_نقاب_ALT)
		ker.dw_Control_کلید_State |= بایاں_ALT_PRESSED;
	}

	إذا (vk_کوڈ == VK_LSHIFT || vk_کوڈ == VK_RSHIFT || vk_کوڈ == VK_کھسکاؤ)
	{
	    إذا (is_کلید_تھلے)
		s_dwMods |= کھسکاؤ_PRESSED;
	    وإلا
		s_dwMods &= ~کھسکاؤ_PRESSED;
	}
	وإلا إذا (vk_کوڈ == VK_LCONTROL || vk_کوڈ == VK_CONTROL)
	{
	    إذا (is_کلید_تھلے)
		s_dwMods |= بایاں_CTRL_PRESSED;
	    وإلا
		s_dwMods &= ~بایاں_CTRL_PRESSED;
	}
	وإلا إذا (vk_کوڈ == VK_RCONTROL)
	{
	    إذا (is_کلید_تھلے)
		s_dwMods |= دایاں_CTRL_PRESSED;
	    وإلا
		s_dwMods &= ~دایاں_CTRL_PRESSED;
	}
	وإلا إذا (vk_کوڈ == VK_LMENU || vk_کوڈ == VK_MENU)
	{
	    إذا (is_کلید_تھلے)
		s_dwMods |= بایاں_ALT_PRESSED;
	    وإلا
		s_dwMods &= ~بایاں_ALT_PRESSED;
	}
	وإلا إذا (vk_کوڈ == VK_RMENU)
	{
	    إذا (is_کلید_تھلے)
		s_dwMods |= دایاں_ALT_PRESSED;
	    وإلا
		s_dwMods &= ~دایاں_ALT_PRESSED;
	}
	ker.dw_Control_کلید_State |= s_dwMods;
	ker.w_Virtual_کلید_کوڈ = vk_کوڈ;
	ker.uChar.UnicodeChar = 0;
	ir->Event.کلید_Event_2 = ker;
	vim_free(action);
    }
    وإلا
    {
	إذا (action == عدم)
	{
	    semsg(_(e_missing_دلیل_str), "event");
	}
	وإلا
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str_str), "event", action);
	    vim_free(action);
	}
	أرجع FALSE;
    }
    أرجع TRUE;
}
# endif  // FEAT_EVAL
#جے_انت // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساكن صحيح g_fMouseAvail = FALSE;   // mouse present
ساكن صحيح g_f_Mouse_سرگرم = FALSE;  // mouse enabled
ساكن صحيح g_nMouseClick = -1;	    // mouse status
ساكن صحيح g_xMouse;		    // mouse x coordinate
ساكن صحيح g_yMouse;		    // mouse y coordinate
ساكن DWORD g_cmodein = 0;	    // Original console input mode
ساكن DWORD g_cmodeout = 0;	    // Original console output mode

/*
 * Enable or disable mouse input
 */
    فراغ
mch_setmouse(صحيح on)
{
    DWORD cmodein;

# ifdef VIMDLL
    إذا (gui.وچ_use)
	أرجع;
# endif
    إذا (!g_fMouseAvail)
	أرجع;

    g_f_Mouse_سرگرم = on;
    Get_Console_منوال(g_h_Con_وچ, &cmodein);

    إذا (g_f_Mouse_سرگرم)
    {
	cmodein |= ENABLE_MOUSE_داخلہ;
	cmodein &= ~ENABLE_تیز_EDIT_منوال;
    }
    وإلا
    {
	cmodein &= ~ENABLE_MOUSE_داخلہ;
	cmodein |= g_cmodein & ENABLE_تیز_EDIT_منوال;
    }

    سیٹ_Console_منوال(g_h_Con_وچ, cmodein | ENABLE_EXTENDED_FLAGS);
}


# إذا defined(FEAT_BEVAL_رکن)
/*
 * Called when 'balloonevalterm' changed.
 */
    فراغ
mch_bevalterm_بدلیا(فراغ)
{
    mch_setmouse(g_f_Mouse_سرگرم);
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
    ساكن فراغ
decode_mouse_wheel(MOUSE_EVENT_ریکارڈ *pmer)
{
    صحيح	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    صحيح	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    صحيح lcol = g_xMouse;
    صحيح lrow = g_yMouse;
    win_T *wp = mouse_لبھو_win(&lrow, &lcol, لبھو_POPUP);
    إذا (wp != عدم && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_میدان(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	إذا (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_بایاں : MSCR_دایاں;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	وإلا
	{
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	    cap.arg = zDelta < 0 ? MSCR_اتے : MSCR_تھلے;
	}

	// Mouse hovers over popup window, scroll it if possible.
	mouse_صف = wp->w_winrow;
	mouse_خانہ = wp->w_wincol;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	باہرلا_flush();
    }
# endif
    mouse_خانہ = g_xMouse;
    mouse_صف = g_yMouse;

    char_u modifiers = 0;
    char_u direction = 0;

    // Decode the direction into an event that Vim can process
    إذا (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    وإلا
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    إذا (pmer->dw_Control_کلید_State & کھسکاؤ_PRESSED)
	modifiers |= MOD_نقاب_کھسکاؤ; // MOUSE_SHIFT;
    إذا (pmer->dw_Control_کلید_State & (دایاں_CTRL_PRESSED | بایاں_CTRL_PRESSED))
	modifiers |= MOD_نقاب_CTRL; // MOUSE_CTRL;
    إذا (pmer->dw_Control_کلید_State & (دایاں_ALT_PRESSED  | بایاں_ALT_PRESSED))
	modifiers |= MOD_نقاب_ALT; // MOUSE_ALT;

    // add (bitwise or) the scroll direction and the key modifier chars
    // together.
    g_nMouseClick = ((direction << 8) | modifiers);

    أرجع;
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
    ساكن BOOL
decode_mouse_event(
    MOUSE_EVENT_ریکارڈ *pmer)
{
    ساكن صحيح s_nOldButton = -1;
    ساكن صحيح s_nOldMouseClick = -1;
    ساكن صحيح s_xOldMouse = -1;
    ساكن صحيح s_yOldMouse = -1;
    ساكن linenr_T s_old_topline = 0;
# ifdef FEAT_DIFF
    ساكن صحيح s_old_topfill = 0;
# endif
    ساكن صحيح s_cClicks = 1;
    ساكن BOOL s_fReleased = TRUE;
    ساكن DWORD s_dw_آخری_Click_ویلا = 0;
    ساكن BOOL s_f_اگلا_Is_درمیان = FALSE;

    ساكن DWORD cButtons = 0;	// number of buttons supported

    ثابت DWORD بایاں_2 = FROM_بایاں_1_ST_BUTTON_PRESSED;
    ثابت DWORD درمیان = FROM_بایاں_2_ND_BUTTON_PRESSED;
    ثابت DWORD دایاں_2 = RIGHTMOST_BUTTON_PRESSED;
    ثابت DWORD بایاں_دایاں = بایاں_2 | دایاں_2;

    صحيح nButton;

    إذا (cButtons == 0 && !Get_نمبر_دا_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    إذا (!g_fMouseAvail || !g_f_Mouse_سرگرم)
    {
	g_nMouseClick = -1;
	أرجع FALSE;
    }

    // get a spurious MOUSE_EVENT immediately after receiving focus; ignore
    إذا (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	أرجع FALSE;
    }

    // If there is an unprocessed mouse click drop this one.
    إذا (g_nMouseClick != -1)
	أرجع TRUE;

    إذا (pmer->dwEventFlags == MOUSE_WHEELED
				       || pmer->dwEventFlags == MOUSE_HWHEELED)
    {
	decode_mouse_wheel(pmer);
	أرجع TRUE;  // we now should have a mouse scroll in g_nMouseClick
    }

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    إذا (pmer->dwEventFlags == MOUSE_MOVED)
    {
	// Ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	// events even when the mouse moves only within a char cell.)
	إذا (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    أرجع FALSE;
    }

    // If no buttons are pressed...
    إذا ((pmer->dwButtonState & ((1 << cButtons) - 1)) == 0)
    {
	nButton = MOUSE_RELEASE;

	// If the last thing returned was MOUSE_RELEASE, ignore this
	إذا (s_fReleased)
	{
# ifdef FEAT_BEVAL_رکن
	    // do return mouse move events when we want them
	    إذا (p_bevalterm)
		nButton = MOUSE_DRAG;
	    وإلا
# endif
		أرجع FALSE;
	}

	s_fReleased = TRUE;
    }
    وإلا    // one or more buttons pressed
    {
	// on a 2-button mouse, hold down left and right buttons
	// simultaneously to get MIDDLE.

	إذا (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & بایاں_دایاں);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    إذا (dwLR == بایاں_2 || dwLR == دایاں_2)
	    {
		لكل (;;)
		{
		    // wait a short time for next input event
		    إذا (WaitForSingleObject(g_h_Con_وچ, p_mouset / 3)
							     != WAIT_OBJECT_0)
			اكسر;
		    وإلا
		    {
			DWORD c_ریکارڈاں = 0;
			داخلہ_ریکارڈ ir;
			MOUSE_EVENT_ریکارڈ* pmer2 = &ir.Event.MouseEvent;

			peek_console_داخلہ(g_h_Con_وچ, &ir, 1, &c_ریکارڈاں);

			إذا (c_ریکارڈاں == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & بایاں_دایاں))
			    اكسر;
			وإلا
			{
			    إذا (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_داخلہ(g_h_Con_وچ, &ir, 1, &c_ریکارڈاں);

				أرجع decode_mouse_event(pmer2);
			    }
			    وإلا إذا (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_داخلہ(g_h_Con_وچ, &ir, 1, &c_ریکارڈاں);

				// are there any more mouse events in queue?
				peek_console_داخلہ(g_h_Con_وچ, &ir, 1, &c_ریکارڈاں);

				إذا (c_ریکارڈاں==0 || ir.EventType != MOUSE_EVENT)
				    اكسر;
			    }
			    وإلا
				اكسر;
			}
		    }
		}
	    }
	}

	إذا (s_f_اگلا_Is_درمیان)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_درمیان;
	    s_f_اگلا_Is_درمیان = FALSE;
	}
	وإلا إذا (cButtons == 2	&&
	    ((pmer->dwButtonState & بایاں_دایاں) == بایاں_دایاں))
	{
	    nButton = MOUSE_درمیان;

	    إذا (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_اگلا_Is_درمیان = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	وإلا إذا ((pmer->dwButtonState & بایاں_2) == بایاں_2)
	    nButton = MOUSE_بایاں;
	وإلا إذا ((pmer->dwButtonState & درمیان) == درمیان)
	    nButton = MOUSE_درمیان;
	وإلا إذا ((pmer->dwButtonState & دایاں_2) == دایاں_2)
	    nButton = MOUSE_دایاں;

	إذا (! s_fReleased && ! s_f_اگلا_Is_درمیان
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    أرجع FALSE;

	s_fReleased = s_f_اگلا_Is_درمیان;
    }

    إذا (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	إذا (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dw_Current_ویلا = Get_Tick_گنتی();

	    إذا (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (صحيح)(dw_Current_ویلا - s_dw_آخری_Click_ویلا) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    وإلا إذا (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dw_آخری_Click_ویلا = dw_Current_ویلا;
	}
    }
    وإلا إذا (pmer->dwEventFlags == MOUSE_MOVED)
    {
	إذا (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    إذا (nButton == -1)
	أرجع FALSE;

    إذا (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    إذا (pmer->dw_Control_کلید_State & کھسکاؤ_PRESSED)
	g_nMouseClick |= MOUSE_کھسکاؤ;
    إذا (pmer->dw_Control_کلید_State & (دایاں_CTRL_PRESSED | بایاں_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    إذا (pmer->dw_Control_کلید_State & (دایاں_ALT_PRESSED  | بایاں_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    إذا (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	سیٹ_عدد_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    // only pass on interesting (i.e., different) mouse events
    إذا (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	أرجع FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    s_old_topfill = curwin->w_topfill;
# endif
    s_nOldMouseClick = g_nMouseClick;

    أرجع TRUE;
}

# ifdef FEAT_EVAL
    ساكن صحيح
encode_mouse_event(dict_T *دلیلاں, داخلہ_ریکارڈ *ir)
{
    صحيح		button;
    صحيح		صف;
    صحيح		خانہ;
    صحيح		repeated_click;
    int_u	mods = 0;
    صحيح		move;

    إذا (!dict_has_کلید(دلیلاں, "row") || !dict_has_کلید(دلیلاں, "col"))
	أرجع FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (صحيح)dict_get_bool(دلیلاں, "move", FALSE);
    إذا (!move && (!dict_has_کلید(دلیلاں, "button")
	    || !dict_has_کلید(دلیلاں, "multiclick")
	    || !dict_has_کلید(دلیلاں, "modifiers")))
	أرجع FALSE;

    صف = (صحيح)dict_get_نمبر(دلیلاں, "row") - 1;
    خانہ = (صحيح)dict_get_نمبر(دلیلاں, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_ریکارڈ mer;
    Zero_حافظہ(&mer, حجم(mer));
    mer.dwMousePosition.X  = خانہ;
    mer.dwMousePosition.Y  = صف;

    إذا (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    وإلا
    {
	button = (صحيح)dict_get_نمبر(دلیلاں, "button");
	repeated_click = (صحيح)dict_get_نمبر(دلیلاں, "multiclick");
	mods = (صحيح)dict_get_نمبر(دلیلاں, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_سیٹ_hor_scroll_قدم(6);
	mouse_سیٹ_vert_scroll_قدم(3);

	بدل (button)
	{
	    حالة MOUSE_بایاں:
		mer.dwButtonState = FROM_بایاں_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		اكسر;
	    حالة MOUSE_درمیان:
		mer.dwButtonState = FROM_بایاں_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		اكسر;
	    حالة MOUSE_دایاں:
		mer.dwButtonState = RIGHTMOST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		اكسر;
	    حالة MOUSE_RELEASE:
		// umm?  Assume Left Release?
		mer.dwEventFlags = 0;

	    حالة MOUSE_MOVE:
		mer.dwButtonState = 0;
		mer.dwEventFlags = MOUSE_MOVED;
		اكسر;
	    حالة MOUSE_X1:
		mer.dwButtonState = FROM_بایاں_3_RD_BUTTON_PRESSED;
		اكسر;
	    حالة MOUSE_X2:
		mer.dwButtonState = FROM_بایاں_4_TH_BUTTON_PRESSED;
		اكسر;
	    حالة MOUSE_4:  // KE_MOUSEDOWN;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_WHEELED;
		اكسر;
	    حالة MOUSE_5:  // KE_MOUSEUP;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_WHEELED;
		اكسر;
	    حالة MOUSE_6:  // KE_MOUSELEFT;
		mer.dwButtonState = -1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		اكسر;
	    حالة MOUSE_7:  // KE_MOUSERIGHT;
		mer.dwButtonState = +1;
		mer.dwEventFlags = MOUSE_HWHEELED;
		اكسر;
	    افتراضي:
		semsg(_(e_invalid_دلیل_str), "button");
		أرجع FALSE;
	}
    }

    mer.dw_Control_کلید_State = 0;
    إذا (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	إذا (mods & MOUSE_کھسکاؤ)
	    mer.dw_Control_کلید_State |= کھسکاؤ_PRESSED;
	إذا (mods & MOUSE_CTRL)
	    mer.dw_Control_کلید_State |= بایاں_CTRL_PRESSED;
	إذا (mods & MOUSE_ALT)
	    mer.dw_Control_کلید_State |= بایاں_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    أرجع TRUE;
}
# endif  // FEAT_EVAL

    ساكن صحيح
write_داخلہ_ریکارڈ_بفر(داخلہ_ریکارڈ* irEvents, صحيح n_لمائی)
{
    صحيح				n_گنتی = 0;
    داخلہ_ریکارڈ_بفر_گرہ_T	*event_گرہ;

    طالما (n_گنتی < n_لمائی)
    {
	event_گرہ = alloc(حجم(داخلہ_ریکارڈ_بفر_گرہ_T));
	إذا (event_گرہ == عدم)
	    أرجع -1;

	event_گرہ->ir = irEvents[n_گنتی++];
	event_گرہ->اگلا = عدم;

	داخلہ_ریکارڈ_بفر.لمائی++;
	إذا (داخلہ_ریکارڈ_بفر.پوچھ == عدم)
	{
	    داخلہ_ریکارڈ_بفر.سر = event_گرہ;
	    داخلہ_ریکارڈ_بفر.پوچھ = event_گرہ;
	}
	وإلا
	{
	    داخلہ_ریکارڈ_بفر.پوچھ->اگلا = event_گرہ;
	    داخلہ_ریکارڈ_بفر.پوچھ = داخلہ_ریکارڈ_بفر.پوچھ->اگلا;
	}
    }
    أرجع n_گنتی;
}

    ساكن صحيح
read_داخلہ_ریکارڈ_بفر(داخلہ_ریکارڈ* irEvents, صحيح n_Max_لمائی)
{
    صحيح n_گنتی = 0;
    طالما (n_گنتی < n_Max_لمائی && داخلہ_ریکارڈ_بفر.سر != عدم)
    {
	داخلہ_ریکارڈ_بفر.لمائی--;
	داخلہ_ریکارڈ_بفر_گرہ_T *نکالو_سر = داخلہ_ریکارڈ_بفر.سر;
	irEvents[n_گنتی++] = نکالو_سر->ir;
	داخلہ_ریکارڈ_بفر.سر = نکالو_سر->اگلا;
	vim_free(نکالو_سر);
	إذا (داخلہ_ریکارڈ_بفر.لمائی == 0)
	    داخلہ_ریکارڈ_بفر.پوچھ = عدم;
    }
    أرجع n_گنتی;
}
#وإلا  // FEAT_GUI_MSWIN && !VIMDLL
    فراغ
mch_setmouse(صحيح on بن_ورتیا)
{
}
#جے_انت // FEAT_GUI_MSWIN && !VIMDLL

#جے_تعریف_شدہ FEAT_EVAL
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
    صحيح
test_mswin_event(char_u *event, dict_T *دلیلاں)
{
    صحيح lpEventsWritten = 0;

# إذا defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    إذا (gui.وچ_use)
	أرجع test_gui_w32_sendevent(event, دلیلاں);
# endif

# إذا defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    داخلہ_ریکارڈ ir;
    BOOL داخلہ_encoded = FALSE;
    BOOL execute = FALSE;
    إذا (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(دلیلاں, "execute", FALSE);
	إذا (dict_has_کلید(دلیلاں, "event"))
	    داخلہ_encoded = encode_کلید_event(دلیلاں, &ir);
	وإلا إذا (!execute)
	{
	    semsg(_(e_missing_دلیل_str), "event");
	    أرجع FALSE;
	}
    }
    وإلا إذا (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	داخلہ_encoded = encode_mouse_event(دلیلاں, &ir);
    }
    وإلا
    {
	semsg(_(e_invalid_قدر_for_دلیل_str_str), "event", event);
	أرجع FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    إذا (داخلہ_encoded)
    {
	إذا ((lpEventsWritten = write_داخلہ_ریکارڈ_بفر(&ir, 1)) < 0)
	{
	    semsg(_(e_باہرلا_دا_حافظہ), "event");
	    أرجع FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    إذا (execute)
    {
	صحيح save_پیغام_scroll = پیغام_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	پیغام_scroll = FALSE;
	ch_log(عدم, "test_mswin_event() executing");
	exec_عام(TRUE, TRUE, TRUE);
	پیغام_scroll |= save_پیغام_scroll;
    }

# endif
    أرجع lpEventsWritten;
}
#جے_انت // FEAT_EVAL

#إذا defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    ساكن فراغ
mch_سیٹ_cursor_shape(صحيح thickness)
{
    إذا (vtp_working)
    {
	إذا (*T_CSI == NUL)
	{
	    // If 't_SI' is not set, use the default cursor styles.
	    إذا (thickness < 50)
		vtp_printf("\033[3 q");	// underline
	    وإلا
		vtp_printf("\033[0 q");	// default
	}
    }
    وإلا
    {
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.dw_ناپ = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	سیٹ_Console_Cursor_Info(g_h_Con_باہرلا, &ConsoleCursorInfo);
	إذا (s_cursor_visible)
	    سیٹ_Console_Cursor_Position(g_h_Con_باہرلا, g_coord);
    }
}

    فراغ
mch_update_cursor(فراغ)
{
    صحيح		idx;
    صحيح		thickness;

# ifdef VIMDLL
    إذا (gui.وچ_use)
	أرجع;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    إذا (shape_جدول[idx].shape == SHAPE_بلاک)
	thickness = 99;	// 100 doesn't work on W95
    وإلا
	thickness = shape_جدول[idx].percentage;
    mch_سیٹ_cursor_shape(thickness);
}
#جے_انت

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    ساكن فراغ
handle_focus_event(داخلہ_ریکارڈ ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.b_سیٹ_Focus;
    ui_focus_change((صحيح)g_fJustGotFocus);
}

ساكن فراغ ResizeConBuf(HANDLE hConsole, COORD coordScreen);

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * When "ignore_input" is TRUE even wait when input is available.
 * Return TRUE if something is available FALSE if not.
 */
    ساكن صحيح
WaitForChar(لما msec, صحيح ignore_داخلہ)
{
    DWORD	    dwNow = 0, dw_سرا_ویلا = 0;
    داخلہ_ریکارڈ    ir;
    DWORD	    c_ریکارڈاں;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    صحيح		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    إذا (msec > 0)
	// Wait until the specified time has elapsed.
	dw_سرا_ویلا = Get_Tick_گنتی() + msec;
    وإلا إذا (msec < 0)
	// Wait forever.
	dw_سرا_ویلا = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    لكل (;;)
    {
	// Only process messages when waiting.
	إذا (msec != 0)
	{
# ifdef سنیہا_قطار
	    parse_queued_messages();
# endif
# ifdef FEAT_MZSCHEME
	    mzvim_check_دھاگے();
# endif
# ifdef FEAT_CLIENTSERVER
	    serverProcessPendingMessages();
# endif
	}

	إذا (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_داخلہ && داخلہ_دستیاب())
# endif
	   )
	    أرجع TRUE;

	إذا (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_گنتی();
	    إذا ((صحيح)(dwNow - dw_سرا_ویلا) >= 0)
		اكسر;
	}
	إذا (msec != 0)
	{
	    DWORD dw_Wait_ویلا = dw_سرا_ویلا - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    إذا (dw_Wait_ویلا > 11)
		dw_Wait_ویلا = 11;

# ifdef FEAT_MZSCHEME
	    إذا (mzthreads_allowed() && p_mzq > 0 && (لما)dw_Wait_ویلا > p_mzq)
		dw_Wait_ویلا = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    إذا (dw_Wait_ویلا > 10)
	    {
		لما	due_ویلا;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_ویلا = check_due_timer();
		إذا (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    أرجع FALSE;
		}
		إذا (due_ویلا > 0 && dw_Wait_ویلا > (DWORD)due_ویلا)
		    dw_Wait_ویلا = due_ویلا;
	    }
# endif
	    إذا (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    پیغام_wait_for_مضرب_objects(1, &g_h_Con_وچ, FALSE,
				  dw_Wait_ویلا, QS_SENDMESSAGE) != WAIT_OBJECT_0
# وإلا
		    wait_for_single_object(g_h_Con_وچ, dw_Wait_ویلا)
							      != WAIT_OBJECT_0
# endif
		    )
		استمر;
	}

	c_ریکارڈاں = 0;
	peek_console_داخلہ(g_h_Con_وچ, &ir, 1, &c_ریکارڈاں);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	إذا (State == منوال_CMDLINE && پیغام_صف == صفاں - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_بفر_INFO csbi;

	    إذا (Get_Console_Screen_بفر_Info(g_h_Con_باہرلا, &csbi))
	    {
		إذا (csbi.dwCursorPosition.Y != پیغام_صف)
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

	إذا (c_ریکارڈاں > 0)
	{
	    إذا (ir.EventType == کلید_EVENT && ir.Event.کلید_Event_2.b_کلید_تھلے)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		إذا (ir.Event.کلید_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.کلید_Event_2.w_Virtual_کلید_کوڈ == 13)
		{
		    read_console_داخلہ(g_h_Con_وچ, &ir, 1, &c_ریکارڈاں);
		    استمر;
		}
# endif
		إذا (decode_کلید_event(&ir.Event.کلید_Event_2, &ch, &ch2,
								 عدم, FALSE))
		    أرجع TRUE;
	    }

	    read_console_داخلہ(g_h_Con_وچ, &ir, 1, &c_ریکارڈاں);

	    إذا (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    وإلا إذا (ir.EventType == کھڑکی_بفر_ناپ_EVENT)
	    {
		COORD dw_ناپ = ir.Event.کھڑکی_بفر_ناپ_Event.dw_ناپ;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		إذا (dw_ناپ.X != Columns || dw_ناپ.Y != صفاں)
		{
		    CONSOLE_SCREEN_بفر_INFO csbi;
		    Get_Console_Screen_بفر_Info(g_h_Con_باہرلا, &csbi);
		    dw_ناپ.X = csbi.sr_کھڑکی.دایاں_3 - csbi.sr_کھڑکی.بایاں_3 + 1;
		    dw_ناپ.Y = csbi.sr_کھڑکی.Bottom - csbi.sr_کھڑکی.چوٹی_2 + 1;
		    إذا (dw_ناپ.X != Columns || dw_ناپ.Y != صفاں)
		    {
			ResizeConBuf(g_h_Con_باہرلا, dw_ناپ);
			shell_resized();
		    }
		}
	    }
	    وإلا إذا (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		أرجع TRUE;
	}
	وإلا إذا (msec == 0)
	    اكسر;
    }

# ifdef FEAT_CLIENTSERVER
    // Something might have been received while we were waiting.
    إذا (داخلہ_دستیاب())
	أرجع TRUE;
# endif

    أرجع FALSE;
}

/*
 * return non-zero if a character is available
 */
    صحيح
mch_char_avail(فراغ)
{
# ifdef VIMDLL
    إذا (gui.وچ_use)
	أرجع TRUE;
# endif
    أرجع WaitForChar(0L, FALSE);
}

# إذا defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    صحيح
mch_check_messages(فراغ)
{
#  ifdef VIMDLL
    إذا (gui.وچ_use)
	أرجع TRUE;
#  endif
    أرجع WaitForChar(0L, TRUE);
}
# endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    ساكن فراغ
create_conin(فراغ)
{
    g_h_Con_وچ =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_گن) عدم,
			OPEN_EXISTING, 0, (HANDLE)عدم);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event, use a blocking wait.
 */
    ساكن WCHAR
tgetch(صحيح *pmodifiers, WCHAR *pch2)
{
    WCHAR ch;

    لكل (;;)
    {
	داخلہ_ریکارڈ ir;
	DWORD c_ریکارڈاں = 0;

# ifdef FEAT_CLIENTSERVER
	(فراغ)WaitForChar(-1L, FALSE);
	إذا (داخلہ_دستیاب())
	    أرجع 0;
	إذا (g_nMouseClick != -1)
	    أرجع 0;
# endif
	إذا (read_console_داخلہ(g_h_Con_وچ, &ir, 1, &c_ریکارڈاں) == 0)
	{
	    إذا (did_create_conin)
		read_error_exit();
	    create_conin();
	    استمر;
	}

	إذا (ir.EventType == کلید_EVENT)
	{
	    إذا (decode_کلید_event(&ir.Event.کلید_Event_2, &ch, pch2,
							    pmodifiers, TRUE))
		أرجع ch;
	}
	وإلا إذا (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	وإلا إذا (ir.EventType == کھڑکی_بفر_ناپ_EVENT)
	    shell_resized();
	وإلا إذا (ir.EventType == MOUSE_EVENT)
	{
	    إذا (decode_mouse_event(&ir.Event.MouseEvent))
		أرجع 0;
	}
    }
}
#جے_انت // !FEAT_GUI_MSWIN


/*
 * mch_inchar(): low-level input function.
 * Get one or more characters from the keyboard or the mouse.
 * If time == 0, do not wait for characters.
 * If time == n, wait a short time for characters.
 * If time == -1, wait forever for characters.
 * Returns the number of characters read into buf.
 */
    صحيح
mch_inchar(
    char_u	*buf بن_ورتیا,
    صحيح		maxlen بن_ورتیا,
    لما	ویلا بن_ورتیا,
    صحيح		tb_change_cnt بن_ورتیا)
{
#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    صحيح		len;
    صحيح		c;
# ifdef VIMDLL
// Extra space for maximum three CSIs. E.g. U+1B6DB -> 0xF0 0x9B 0x9B 0x9B.
#  define TYPEAHEADSPACE    6
# وإلا
#  define TYPEAHEADSPACE    0
# endif
# define TYPEAHEADLEN	    (20 + TYPEAHEADSPACE)
    ساكن char_u   typeahead[TYPEAHEADLEN];	// previously typed bytes.
    ساكن صحيح	    typeaheadlen = 0;

# ifdef VIMDLL
    إذا (gui.وچ_use)
	أرجع 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    إذا (typeaheadlen > 0)
	اذهب theend;

    إذا (ویلا >= 0)
    {
	إذا (!WaitForChar(ویلا, FALSE))     // no character available
	    أرجع 0;
    }
    وإلا    // time == -1, wait forever
    {
	mch_سیٹ_winsize_now();	// Allow winsize changes from now on

	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	إذا (!WaitForChar(p_ut, FALSE))
	{
	    إذا (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (صحيح)KE_CURSORHOLD;
		أرجع 3;
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
    إذا (fdDump)
	fputc('[', fdDump);
# endif

    // Keep looping until there is something in the typeahead buffer and more
    // to get and still room in the buffer.  A mouse event uses up to
    // 10 bytes: 3 (modifier) + 3 (scroll event) + 4 (coordinates), and a
    // keyboard input uses up to 7 bytes: 3 (modifier) + 4 (UTF-8 char).
    طالما ((typeaheadlen == 0 || WaitForChar(0L, FALSE))
			  && typeaheadlen + 10 + TYPEAHEADSPACE <= TYPEAHEADLEN)
    {
	إذا (typebuf_بدلیا(tb_change_cnt))
	{
	    // "buf" may be invalid now if a client put something in the
	    // typeahead buffer and "buf" is in the typeahead buffer.
	    typeaheadlen = 0;
	    اكسر;
	}
	إذا (g_nMouseClick != -1)
	{
# ifdef MCH_WRITE_DUMP
	    إذا (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif
	    char_u modifiers = ((char_u *)(&g_nMouseClick))[0];
	    char_u scroll_dir = ((char_u *)(&g_nMouseClick))[1];

	    إذا (scroll_dir == KE_MOUSEDOWN
		    || scroll_dir == KE_MOUSEUP
		    || scroll_dir == KE_MOUSELEFT
		    || scroll_dir == KE_MOUSERIGHT)
	    {
		إذا (modifiers > 0)
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
	    وإلا
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
	وإلا
	{
	    WCHAR	ch2 = NUL;
	    صحيح		modifiers = 0;

	    c = tgetch(&modifiers, &ch2);

	    c = simplify_کلید(c, &modifiers);

	    // Some chars need adjustment when the Ctrl modifier is used.
	    ++no_reduce_کلیدیں;
	    c = may_adjust_کلید_for_ctrl(modifiers, c);
	    --no_reduce_کلیدیں;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_کھسکاؤ_modifier(modifiers, c);

	    إذا (typebuf_بدلیا(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		اكسر;
	    }

	    إذا (c == Ctrl_C && ctrl_c_interrupts)
	    {
# إذا defined(FEAT_CLIENTSERVER)
		trash_داخلہ_buf();
# endif
		got_int = TRUE;
	    }

	    إذا (g_nMouseClick == -1)
	    {
		صحيح	n = 1;

		إذا (ch2 == NUL)
		{
		    صحيح	    i, j;
		    char_u  *p;
		    WCHAR   ch[2];

		    ch[0] = c;
		    إذا (c >= 0xD800 && c <= 0xDBFF)	// High surrogate
		    {
			ch[1] = tgetch(&modifiers, &ch2);
			n++;
		    }
		    p = utf16_منزل_enc(ch, &n);
		    إذا (p != عدم)
		    {
			لكل (i = 0, j = 0; i < n; i++)
			{
			    typeahead[typeaheadlen + j++] = p[i];
# ifdef VIMDLL
			    إذا (p[i] == CSI)
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
		وإلا
		{
		    typeahead[typeaheadlen] = c;
# ifdef VIMDLL
		    إذا (c == CSI)
		    {
			typeahead[typeaheadlen + 1] = KS_EXTRA;
			typeahead[typeaheadlen + 2] = KE_CSI;
			n = 3;
		    }
# endif
		}
		إذا (ch2 != NUL)
		{
		    إذا (c == K_NUL)
		    {
			بدل (ch2)
			{
			حالة (WCHAR)'\324': // SHIFT+Insert
			حالة (WCHAR)'\325': // CTRL+Insert
			حالة (WCHAR)'\327': // SHIFT+Delete
			حالة (WCHAR)'\330': // CTRL+Delete
			    typeahead[typeaheadlen + n] = (char_u)ch2;
			    n++;
			    اكسر;

			افتراضي:
			    typeahead[typeaheadlen + n] = 3;
			    typeahead[typeaheadlen + n + 1] = (char_u)ch2;
			    n += 2;
			    اكسر;
			}
		    }
		    وإلا
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
		إذا ((modifiers & MOD_نقاب_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_نقاب_ALT;
		}

		إذا (modifiers != 0)
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
		إذا (fdDump)
		    fputc(c, fdDump);
# endif
	    }
	}
    }

# ifdef MCH_WRITE_DUMP
    إذا (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
# endif

theend:
    // Move typeahead to "buf", as much as fits.
    len = 0;
    طالما (len < maxlen && typeaheadlen > 0)
    {
	buf[len++] = typeahead[0];
	mch_memmove(typeahead, typeahead + 1, --typeaheadlen);
    }
# ifdef FEAT_EVAL
    إذا (len > 0)
    {
	buf[len] = NUL;
	ch_log(عدم, "raw key input: \"%s\"", buf);
    }
# endif
    أرجع len;

#وإلا // FEAT_GUI_MSWIN && !VIMDLL
    أرجع 0;
#جے_انت // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    ساكن صحيح
executable_file(حرف *name, char_u **رستہ)
{
    صحيح attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    إذا (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	أرجع FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    إذا (attrs & FILE_ATTRIBUTE_REPARSE_نقطہ)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	إذا (res == عدم)
	    res = resolve_reparse_نقطہ((char_u *)name);
	إذا (res == عدم)
	    أرجع FALSE;
	// The path is already absolute.
	إذا (رستہ != عدم)
	    *رستہ = res;
	وإلا
	    vim_free(res);
    }
    وإلا إذا (رستہ != عدم)
	*رستہ = بھریا_Name_save((char_u *)name, FALSE);
    أرجع TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    ساكن bool
executable_اگیتر_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_لبھو_DATAW	انکڑے;
    HANDLE		handle;
    DWORD		error;

    wn = enc_منزل_utf16(name, عدم);
    إذا (wn == عدم)
	أرجع true;
    handle = لبھو_پہلا_File_Ex_W(wn, لبھو_Ex_Info_Basic, &انکڑے,
					    لبھو_Ex_تلاش_Name_میل, عدم, 0);
    error = Get_آخری_Error();
    vim_free(wn);
    إذا (handle != INVALID_HANDLE_قدر)
    {
	لبھو_Close(handle);
	أرجع true;
    }
    أرجع error != ERROR_FILE_NOT_لبھیا && error != ERROR_رستہ_NOT_لبھیا;
}

/*
 * If "use_path" is TRUE: Return TRUE if "name" is in $PATH.
 * If "use_path" is FALSE: Return TRUE if "name" exists.
 * If "use_pathext" is TRUE search "name" with extensions in $PATHEXT.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    ساكن صحيح
executable_exists(
    حرف	*name,
    size_t	namelen,
    char_u	**رستہ,
    صحيح		use_رستہ,
    صحيح		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[MAX_رستہ_2 * 3];
    char_u	*p;
    string_T	pathbuf = {عدم, 0};
    صحيح		pathbuf_allocated = FALSE;
    string_T	pathext = {عدم, 0};
    صحيح		pathext_allocated = FALSE;
    char_u	*shname = عدم;
    صحيح		noext = FALSE;
    bool	simple_pathext = true;
    صحيح		ext_گنتی = 0;
    صحيح		retval = FALSE;

    إذا (namelen >= حجم(buf))	// safety check
	أرجع FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    إذا (لڑی_کھوج((حرف *)shname, "sh") != عدم &&
	!(لڑی_کھوج((حرف *)shname, "powershell") != عدم
				    || لڑی_کھوج((حرف *)shname, "pwsh") != عدم))
	noext = TRUE;

    إذا (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	إذا (pathext.string == عدم)
	    STR_LITERAL_سیٹ(pathext, ".com;.exe;.bat;.cmd");
	وإلا
	    pathext.لمائی = STRLEN(pathext.string);

	إذا (noext == FALSE)
	{
	    char_u  *e;
	    char_u  *q;
	    size_t  plen;

	    /*
	     * Loop over all extensions in $PATHEXT.
	     * Check "name" ends with extension.
	     */
	    p = pathext.string;
	    طالما (*p)
	    {
		إذا (p[0] == ';'
			    || (p[0] == '.' && (p[1] == NUL || p[1] == ';')))
		{
		    // Skip empty or single ".".
		    إذا (*p == '.')
			simple_pathext = false;
		    ++p;
		    استمر;
		}
		e = vim_strchr(p, ';');
		إذا (e == عدم)
		    e = pathext.string + pathext.لمائی;
		plen = (size_t)(e - p);
		++ext_گنتی;
		إذا (*p != '.')
		    simple_pathext = false;
		لكل (q = p + 1; q < e; ++q)
		    إذا (!ASCII_ISALNUM(*q))
			simple_pathext = false;

		إذا (_strnicoll(name + namelen - plen, (حرف *)p, plen) == 0)
		{
		    noext = TRUE;
		    اكسر;
		}

		p = e;
	    }
	}
    }

    // Prepend single "." to pathext, it means no extension added.
    إذا (pathext.string == عدم)
	STR_LITERAL_سیٹ(pathext, ".");
    وإلا إذا (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.لمائی + 3);
	إذا (tmp == عدم)
	{
	    retval = FALSE;
	    اذهب theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.لمائی += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    إذا (use_رستہ && gettail((char_u *)name) == (char_u *)name)
    {
	p = mch_getenv("PATH");
	إذا (p != عدم)
	{
	    size_t  plen = STRLEN(p);

	    pathbuf.string = alloc(plen + 3);
	    إذا (pathbuf.string == عدم)
	    {
		retval = FALSE;
		اذهب theend;
	    }

	    إذا (mch_getenv("NoDefaultCurrentDirectoryInExePath") == عدم &&
		    لڑی_کھوج((حرف *)gettail(p_sh), "cmd.exe") != عدم)
	    {
		STRCPY(pathbuf.string, ".;");
		pathbuf.لمائی = 2;
	    }
	    STRCPY(pathbuf.string + pathbuf.لمائی, p);
	    pathbuf.لمائی += plen;
	    pathbuf_allocated = TRUE;
	}
    }

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    إذا (pathbuf.string == عدم)
	STR_LITERAL_سیٹ(pathbuf, ".");
    p = pathbuf.string;
    طالما (*p)
    {
	char_u	*e;
	size_t	buflen;

	إذا (*p == ';') // Skip empty entry
	{
	    ++p;
	    استمر;
	}
	e = vim_strchr(p, ';');
	إذا (e == عدم)
	    e = pathbuf.string + pathbuf.لمائی;

	إذا (e - p + namelen + 2 > حجم(buf))
	{
	    retval = FALSE;
	    اذهب theend;
	}
	// A single "." that means current dir.
	إذا (e - p == 1 && *p == '.')
	{
	    STRCPY(buf, name);
	    buflen = namelen;
	}
	وإلا
	{
	    buflen = vim_snprintf_safelen(
		(حرف *)buf,
		حجم(buf),
		"%.*s%s%s", (صحيح)(e - p), p,
		!after_pathsep(p, e) ? PATHSEPSTR : "",
		name);
	}

	// Most directories in $PATH do not contain this command.  One prefix
	// search can avoid a separate file query for every extension.  With
	// few extensions, or an explicit executable suffix, it is cheaper to
	// use the normal checks.  Leave unusual paths and suffixes to those
	// checks as well, to preserve their path normalization and semantics.
	إذا (!noext && simple_pathext && ext_گنتی >= 3 && buflen > 0
		&& buflen + 2 <= حجم(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == عدم
		&& vim_strchr(gettail(buf), ':') == عدم)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_اگیتر_exists(buf);
	    buf[buflen] = NUL;
	    إذا (!exists)
	    {
		p = e;
		استمر;
	    }
	}

	/*
	 * Loop over all extensions in $PATHEXT.
	 * Check "name" with extension added.
	 */
	p = pathext.string;
	طالما (*p)
	{
	    char_u  *e2;

	    إذا (*p == ';')
	    {
		// Skip empty entry
		++p;
		استمر;
	    }
	    e2 = vim_strchr(p, ';');
	    إذا (e2 == عدم)
		e2 = pathext.string + pathext.لمائی;

	    إذا (!(p[0] == '.' && (p[1] == NUL || p[1] == ';')))
	    {
		// Not a single "." that means no extension is added.
		إذا (e2 - p + buflen + 1 > حجم(buf))
		{
		    retval = FALSE;
		    اذهب theend;
		}
		vim_strncpy(buf + buflen, p, e2 - p);
	    }
	    إذا (executable_file((حرف *)buf, رستہ))
	    {
		retval = TRUE;
		اذهب theend;
	    }

	    p = e2;
	}

	p = e;
    }

theend:
    إذا (pathbuf_allocated)
	حرر(pathbuf.string);
    إذا (pathext_allocated)
	حرر(pathext.string);
    أرجع retval;
}

#إذا (defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x800) || \
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
    ساكن فراغ
bad_param_handler(ثابت wchar_t *عبارت بن_ورتیا,
    ثابت wchar_t *function بن_ورتیا,
    ثابت wchar_t *file بن_ورتیا,
    بنانشانی صحيح سطر بن_ورتیا,
    uintptr_t pReserved بن_ورتیا)
{
}

# define سیٹ_INVALID_PARAM_HANDLER \
	((فراغ)سیٹ_invalid_parameter_handler(bad_param_handler))
#وإلا
# define سیٹ_INVALID_PARAM_HANDLER
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_MSWIN

/*
 * GUI version of mch_init().
 */
    ساكن فراغ
mch_init_g(فراغ)
{
# ifndef __MINGW32__
    خارجي صحيح _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    سیٹ_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    سیٹ_Error_منوال(SEM_FAILCRITICALERRORS);

    _fmode = O_ثنائی;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    صفاں = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[MAX_رستہ_2 + 4];
	size_t	exe_pathlen = (size_t)(gettail(exe_name) - exe_name);

	// Note: 10 is length of 'vimrun.exe'.
	إذا (exe_pathlen + 10 >= حجم(vimrun_location))
	{
	    إذا (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    عدم, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}
	وإلا
	{
	    // First try in same directory as gvim.exe.
	    إذا (exe_pathlen > 0)
		vim_strncpy(vimrun_location, exe_name, exe_pathlen);
	    STRCPY(vimrun_location + exe_pathlen, "vimrun.exe");

	    إذا (mch_getperm(vimrun_location) >= 0)
	    {
		char_u  *p;
		size_t  plen;

		إذا (exe_pathlen > 0 && *skiptowhite(vimrun_location) != NUL)
		{
		    // Enclose path with white space in double quotes.
		    plen = vim_snprintf_safelen(
			(حرف *)vimrun_location,
			حجم(vimrun_location),
			"\"%.*svimrun\" ",
			(صحيح)exe_pathlen, exe_name);
		}
		وإلا
		{
		    // Remove the suffix ('.exe').
		    vimrun_location[exe_pathlen + 6] = ' ';
		    vimrun_location[exe_pathlen + 7] = NUL;
		    plen = exe_pathlen + 7;
		}

		p = vim_strnsave(vimrun_location, plen);
		إذا (p != عدم)
		{
		    vimrun_رستہ.string = p;
		    vimrun_رستہ.لمائی = plen;
		    vimrun_رستہ_allocated = TRUE;
		    s_dont_use_vimrun = FALSE;
		}
	    }
	    وإلا إذا (executable_exists("vimrun.exe", STRLEN_LITERAL("vimrun.exe"),
		    عدم, TRUE, FALSE))
		s_dont_use_vimrun = FALSE;
	}

	// Don't give the warning for a missing vimrun.exe right now, but only
	// when vimrun was supposed to be used.  Don't bother people that do
	// not need vimrun.exe.
	إذا (s_dont_use_vimrun)
	    need_vimrun_warning = TRUE;
    }

    /*
     * If "findstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    إذا (!executable_exists("findstr.exe", STRLEN_LITERAL("findstr.exe"),
	    عدم, TRUE, FALSE))
	سیٹ_option_قدر_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#جے_انت // FEAT_GUI_MSWIN

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

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
    ساكن BOOL
Clear_Console_بفر(لفظ_2 wAttribute)
{
    CONSOLE_SCREEN_بفر_INFO csbi;
    COORD coord;
    DWORD عدد_Cells, dummy;

    إذا (!Get_Console_Screen_بفر_Info(g_h_Con_باہرلا, &csbi))
	أرجع FALSE;

    عدد_Cells = csbi.dw_ناپ.X * csbi.dw_ناپ.Y;
    coord.X = 0;
    coord.Y = 0;
    إذا (!FillConsoleOutputCharacter(g_h_Con_باہرلا, ' ', عدد_Cells,
	    coord, &dummy))
	أرجع FALSE;
    إذا (!FillConsoleOutputAttribute(g_h_Con_باہرلا, wAttribute, عدد_Cells,
	    coord, &dummy))
	أرجع FALSE;

    أرجع TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    ساكن BOOL
Fit_Console_کھڑکی(
    COORD dw_بفر_ناپ,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_بفر_INFO csbi;
    COORD dw_کھڑکی_ناپ;
    BOOL NeedAdjust = FALSE;

    إذا (!Get_Console_Screen_بفر_Info(g_h_Con_باہرلا, &csbi))
	أرجع FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    إذا (csbi.sr_کھڑکی.دایاں_3 >= dw_بفر_ناپ.X)
    {
	dw_کھڑکی_ناپ.X = SRWIDTH(csbi.sr_کھڑکی);
	إذا (dw_کھڑکی_ناپ.X > dw_بفر_ناپ.X)
	    dw_کھڑکی_ناپ.X = dw_بفر_ناپ.X;
	csbi.sr_کھڑکی.دایاں_3 = dw_بفر_ناپ.X - 1;
	csbi.sr_کھڑکی.بایاں_3 = dw_بفر_ناپ.X - dw_کھڑکی_ناپ.X;
	NeedAdjust = TRUE;
    }
    إذا (csbi.sr_کھڑکی.Bottom >= dw_بفر_ناپ.Y)
    {
	dw_کھڑکی_ناپ.Y = SRHEIGHT(csbi.sr_کھڑکی);
	إذا (dw_کھڑکی_ناپ.Y > dw_بفر_ناپ.Y)
	    dw_کھڑکی_ناپ.Y = dw_بفر_ناپ.Y;
	csbi.sr_کھڑکی.Bottom = dw_بفر_ناپ.Y - 1;
	csbi.sr_کھڑکی.چوٹی_2 = dw_بفر_ناپ.Y - dw_کھڑکی_ناپ.Y;
	NeedAdjust = TRUE;
    }
    إذا (NeedAdjust && WantAdjust)
    {
	إذا (!سیٹ_Console_کھڑکی_Info(g_h_Con_باہرلا, TRUE, &csbi.sr_کھڑکی))
	    أرجع FALSE;
    }
    أرجع TRUE;
}

تعريفنوع بنية Console_بفر_Struct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_بفر_INFO	Info;
    PCHAR_INFO			بفر_2;
    COORD			بفر_ناپ_2;
    PSMALL_RECT			Regions;
    صحيح				عدد_Regions;
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
    ساكن BOOL
Save_Console_بفر(
    Console_بفر *cb)
{
    DWORD عدد_Cells;
    COORD بفر_Coord;
    SMALL_RECT ReadRegion;
    لفظ_2 Y, Y_incr;
    صحيح i, numregions;

    إذا (cb == عدم)
	أرجع FALSE;

    إذا (!Get_Console_Screen_بفر_Info(g_h_Con_باہرلا, &cb->Info))
    {
	cb->IsValid = FALSE;
	أرجع FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    إذا (use_alternate_screen_بفر)
	أرجع TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    إذا (!cb->IsValid || cb->بفر_2 == عدم ||
	    cb->بفر_ناپ_2.X != cb->Info.dw_ناپ.X ||
	    cb->بفر_ناپ_2.Y != cb->Info.dw_ناپ.Y)
    {
	cb->بفر_ناپ_2.X = cb->Info.dw_ناپ.X;
	cb->بفر_ناپ_2.Y = cb->Info.dw_ناپ.Y;
	عدد_Cells = cb->بفر_ناپ_2.X * cb->بفر_ناپ_2.Y;
	vim_free(cb->بفر_2);
	cb->بفر_2 = ALLOC_MULT(CHAR_INFO, عدد_Cells);
	إذا (cb->بفر_2 == عدم)
	    أرجع FALSE;
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
    ReadRegion.دایاں_3 = cb->Info.dw_ناپ.X - 1;
    Y_incr = 12000 / cb->Info.dw_ناپ.X;

    numregions = (cb->Info.dw_ناپ.Y + Y_incr - 1) / Y_incr;
    إذا (cb->Regions == عدم || numregions != cb->عدد_Regions)
    {
	cb->عدد_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->عدد_Regions);
	إذا (cb->Regions == عدم)
	{
	    VIM_CLEAR(cb->بفر_2);
	    أرجع FALSE;
	}
    }

    لكل (i = 0, Y = 0; i < cb->عدد_Regions; i++, Y += Y_incr)
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
	إذا (!ReadConsoleOutputW(g_h_Con_باہرلا,	// output handle
		cb->بفر_2,			// our buffer
		cb->بفر_ناپ_2,			// dimensions of our buffer
		بفر_Coord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->بفر_2);
	    VIM_CLEAR(cb->Regions);
	    أرجع FALSE;
	}
	cb->Regions[i] = ReadRegion;
    }

    أرجع TRUE;
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
    ساكن BOOL
Restore_Console_بفر(
    Console_بفر   *cb,
    BOOL	    RestoreScreen)
{
    COORD بفر_Coord;
    SMALL_RECT WriteRegion;
    صحيح i;

    إذا (cb == عدم || !cb->IsValid)
	أرجع FALSE;

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    إذا (RestoreScreen)
	Clear_Console_بفر(cb->Info.w_گن);

    Fit_Console_کھڑکی(cb->Info.dw_ناپ, TRUE);
    إذا (!سیٹ_Console_Screen_بفر_ناپ(g_h_Con_باہرلا, cb->Info.dw_ناپ))
	أرجع FALSE;
    إذا (!سیٹ_Console_متن_Attribute(g_h_Con_باہرلا, cb->Info.w_گن))
	أرجع FALSE;

    إذا (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	أرجع TRUE;
    }

    إذا (!سیٹ_Console_Cursor_Position(g_h_Con_باہرلا, cb->Info.dwCursorPosition))
	أرجع FALSE;
    إذا (!سیٹ_Console_کھڑکی_Info(g_h_Con_باہرلا, TRUE, &cb->Info.sr_کھڑکی))
	أرجع FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    إذا (use_alternate_screen_بفر)
	أرجع TRUE;

    /*
     * Restore the screen buffer contents.
     */
    إذا (cb->بفر_2 != عدم)
    {
	لكل (i = 0; i < cb->عدد_Regions; i++)
	{
	    بفر_Coord.X = cb->Regions[i].بایاں_3;
	    بفر_Coord.Y = cb->Regions[i].چوٹی_2;
	    WriteRegion = cb->Regions[i];
	    إذا (!WriteConsoleOutputW(g_h_Con_باہرلا,	// output handle
			cb->بفر_2,		// our buffer
			cb->بفر_ناپ_2,		// dimensions of our buffer
			بفر_Coord,		// offset in our buffer
			&WriteRegion))		// region to restore
		أرجع FALSE;
	}
    }

    أرجع TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
ساكن Console_بفر g_cbOrig = { 0 };
# endif
ساكن Console_بفر g_cbNonTermcap = { 0 };
ساكن Console_بفر g_cbTermcap = { 0 };

حرف g_szOrigTitle[256] = { 0 };
HWND g_hWnd = عدم;	// also used in os_mswin.c
ساكن HICON g_hOrigIconSmall = عدم;
ساكن HICON g_hOrigIcon = عدم;
ساكن HICON g_hVimIcon = عدم;
ساكن BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ساكن BOOL
GetConsoleIcon(
    HWND	hWnd,
    HICON	*phIconSmall,
    HICON	*phIcon)
{
    إذا (hWnd == عدم)
	أرجع FALSE;

    إذا (phIconSmall != عدم)
	*phIconSmall = (HICON)Send_سنیہا(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    إذا (phIcon != عدم)
	*phIcon = (HICON)Send_سنیہا(hWnd, WM_GETICON,
						 (WPARAM)ICON_BIG, (LPARAM)0);
    أرجع TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    ساكن BOOL
سیٹ_Console_Icon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    إذا (hWnd == عدم)
	أرجع FALSE;

    إذا (hIconSmall != عدم)
	Send_سنیہا(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    إذا (hIcon != عدم)
	Send_سنیہا(hWnd, WM_SETICON,
			    (WPARAM)ICON_BIG, (LPARAM) hIcon);
    أرجع TRUE;
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
    ساكن فراغ
SaveConsoleTitleAndIcon(فراغ)
{
    // Save the original title.
    إذا (!GetConsoleTitle(g_szOrigTitle, حجم(g_szOrigTitle)))
	أرجع;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    g_hWnd = Get_Console_کھڑکی();
    إذا (g_hWnd == عدم)
	أرجع;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    إذا (g_hOrigIconSmall == عدم || g_hOrigIcon == عدم)
	أرجع;

    // Extract the first icon contained in the Vim executable.
    إذا (
# ifdef FEAT_LIBCALL
	    mch_icon_لدو((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == عدم)
	g_hVimIcon = کڈھو_Icon(عدم, (LPCSTR)exe_name, 0);
    إذا (g_hVimIcon != عدم)
	g_fCanChangeIcon = TRUE;
}

ساكن صحيح g_fWindInitCalled = FALSE;
ساكن صحيح g_f_Termcap_منوال = FALSE;
ساكن CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    ساكن فراغ
mch_init_c(فراغ)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_بفر_INFO csbi;
# endif
# ifndef __MINGW32__
    خارجي صحيح _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    سیٹ_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    سیٹ_Error_منوال(SEM_FAILCRITICALERRORS);

    _fmode = O_ثنائی;		// we do our own CR-LF translation
    باہرلا_flush();

    // Obtain handles for the standard Console I/O devices
    إذا (read_cmd_fd == 0)
	g_h_Con_وچ =  GetStdHandle(STD_داخلہ_HANDLE);
    وإلا
	create_conin();
    g_h_Con_باہرلا = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    Save_Console_بفر(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.w_گن;
# وإلا
    // Get current text attributes
    Get_Console_Screen_بفر_Info(g_h_Con_باہرلا, &csbi);
    g_attrCurrent = g_attrDefault = csbi.w_گن;
# endif
    إذا (cterm_عام_fg_color == 0)
	cterm_عام_fg_color = (g_attrCurrent & 0xf) + 1;
    إذا (cterm_عام_bg_color == 0)
	cterm_عام_bg_color = ((g_attrCurrent >> 4) & 0xf) + 1;

    // Fg and Bg color index number at startup
    g_color_اشاریہ_fg = g_attrDefault & 0xf;
    g_color_اشاریہ_bg = (g_attrDefault >> 4) & 0xf;

    // set termcap codes to current text attributes
    update_tcap(g_attrCurrent);

    GetConsoleCursorInfo(g_h_Con_باہرلا, &g_cci);
    Get_Console_منوال(g_h_Con_وچ,  &g_cmodein);
    Get_Console_منوال(g_h_Con_باہرلا, &g_cmodeout);

    SaveConsoleTitleAndIcon();
    /*
     * Set both the small and big icons of the console window to Vim's icon.
     * Note that Vim presently only has one size of icon (32x32), but it
     * automatically gets scaled down to 16x16 when setting the small icon.
     */
    إذا (g_fCanChangeIcon)
	سیٹ_Console_Icon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    إذا (use_alternate_screen_بفر)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    إذا (fdDump)
    {
	time_t t;

	ویلا(&t);
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
    ساكن فراغ
mch_exit_c(صحيح r)
{
    // Copy flag since stoptermcap() will clear the flag.
    صحيح f_Termcap_منوال = g_f_Termcap_منوال;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    إذا (!f_Termcap_منوال &&  use_alternate_screen_بفر)
	vtp_printf("\033[?1049l");

    إذا (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		// remove all memfiles

    إذا (g_fWindInitCalled)
    {
	mch_restore_title(SAVE_RESTORE_BOTH);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.  Don't do this when the window is
	 * closed, Vim would hang here.
	 */
	إذا (g_fCanChangeIcon && !g_fForceExit)
	    سیٹ_Console_Icon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	إذا (fdDump)
	{
	    time_t t;

	    ویلا(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = عدم;
# endif
    }

    سیٹ_Console_Cursor_Info(g_h_Con_باہرلا, &g_cci);
    سیٹ_Console_منوال(g_h_Con_وچ,  g_cmodein | ENABLE_EXTENDED_FLAGS);
    سیٹ_Console_منوال(g_h_Con_باہرلا, g_cmodeout);

# ifdef DYNAMIC_GETTEXT
    dyn_libintl_سرا();
# endif

    باہر_نکلو(r);
}
#جے_انت // !FEAT_GUI_MSWIN || VIMDLL

    فراغ
mch_init(فراغ)
{
#جے_تعریف_شدہ VIMDLL
    إذا (gui.starting)
	mch_init_g();
    وإلا
	mch_init_c();
#وإلاإذا defined(FEAT_GUI_MSWIN)
    mch_init_g();
#وإلا
    mch_init_c();
#جے_انت
}

    فراغ
mch_exit(صحيح r)
{
#جے_تعریف_شدہ FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#جے_انت

#جے_تعریف_شدہ VIMDLL
    إذا (vimrun_رستہ_allocated)
	vim_free(vimrun_رستہ.string);

    إذا (gui.وچ_use || gui.starting)
	mch_exit_g(r);
    وإلا
	mch_exit_c(r);
#وإلاإذا defined(FEAT_GUI_MSWIN)
    إذا (vimrun_رستہ_allocated)
	vim_free(vimrun_رستہ.string);
    mch_exit_g(r);
#وإلا
    mch_exit_c(r);
#جے_انت
}

/*
 * Do we have an interactive window?
 */
    صحيح
mch_check_win(
    صحيح argc بن_ورتیا,
    حرف **argv بن_ورتیا)
{
    mch_get_exe_name();

#إذا defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    أرجع OK;	    // GUI always has a tty
#وإلا
# ifdef VIMDLL
    إذا (gui.وچ_use)
	أرجع OK;
# endif
    إذا (isatty(1))
	أرجع OK;
    أرجع FAIL;
#جے_انت
}

/*
 * Set the case of the file name, if it already exists.
 * When "len" is > 0, also expand short to long filenames.
 */
    فراغ
fname_case(
    char_u	*name,
    صحيح		len)
{
    WCHAR   *p;
    WCHAR   buf[MAX_رستہ_2 + 1];

    إذا (*name == NUL)
	أرجع;

    slash_adjust(name);

    p = enc_منزل_utf16(name, عدم);
    إذا (p == عدم)
	أرجع;

    إذا (Get_Long_رستہ_Name_W(p, buf, MAX_رستہ_2))
    {
	char_u	*q = utf16_منزل_enc(buf, عدم);

	إذا (q != عدم)
	{
	    إذا (len > 0)
		vim_strncpy(name, q, len - 1);
	    وإلا
	    {
		size_t  namelen = STRLEN(name);

		إذا (namelen >= STRLEN(q))
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
    صحيح
mch_get_user_name(
    char_u  *s,
    صحيح	    len)
{
    WCHAR wszUserName[256 + 1];	// UNLEN is 256
    DWORD wcch = ARRAY_لمائی(wszUserName);

    إذا (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_منزل_enc(wszUserName, عدم);

	إذا (p != عدم)
	{
	    vim_strncpy(s, p, len - 1);
	    vim_free(p);
	    أرجع OK;
	}
    }
    s[0] = NUL;
    أرجع FAIL;
}


/*
 * Insert host name in s[len].
 */
    فراغ
mch_get_host_name(
    char_u	*s,
    صحيح		len)
{
    WCHAR wszHostName[256 + 1];
    DWORD wcch = ARRAY_لمائی(wszHostName);

    إذا (!GetComputerNameW(wszHostName, &wcch))
	أرجع;

    char_u  *p = utf16_منزل_enc(wszHostName, عدم);
    إذا (p == عدم)
	أرجع;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    لما
mch_get_pid(فراغ)
{
    أرجع (لما)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    صحيح
mch_process_running(لما pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   حالت = 0;
    صحيح	    ret = FALSE;

    إذا (hProcess == عدم)
	أرجع FALSE;  // might not have access
    إذا (Get_Exit_کوڈ_Process(hProcess, &حالت))
	ret = حالت == STILL_سرگرم;
    CloseHandle(hProcess);
    أرجع ret;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    صحيح
mch_dirname(
    char_u	*buf,
    صحيح		len)
{
    WCHAR   wbuf[MAX_رستہ_2 + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    إذا (GetCurrentDirectoryW(MAX_رستہ_2, wbuf) == 0)
	أرجع FAIL;

    WCHAR   wcbuf[MAX_رستہ_2 + 1];
    char_u  *p = عدم;

    إذا (Get_Long_رستہ_Name_W(wbuf, wcbuf, MAX_رستہ_2) != 0)
    {
	p = utf16_منزل_enc(wcbuf, عدم);
	إذا (p != عدم)
	{
	    إذا (STRLEN(p) >= (size_t)len)
	    {
		// long path name is too long, fall back to short one
		VIM_CLEAR(p);
	    }
	}
    }
    إذا (p == عدم)
	p = utf16_منزل_enc(wbuf, عدم);

    إذا (p == عدم)
	أرجع FAIL;

    vim_strncpy(buf, p, len - 1);
    vim_free(p);
    أرجع OK;
}

/*
 * Get file permissions for "name".
 * Return mode_t or -1 for error.
 */
    لما
mch_getperm(char_u *name)
{
    stat_T	st;

    أرجع (mch_stat((حرف *)name, &st) == 0)
	? (لما)(بنانشانی چھوٹا)st.st_منوال
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
mch_setperm(char_u *name, لما perm)
{
    لما	n;
    WCHAR	*p;

    p = enc_منزل_utf16(name, عدم);
    إذا (p == عدم)
	أرجع FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    إذا (n == -1)
	أرجع FAIL;

    win32_سیٹ_archive(name);

    أرجع OK;
}

/*
 * Set hidden flag for "name".
 */
    فراغ
mch_hide(char_u *name)
{
    صحيح attrs = win32_getattrs(name);
    إذا (attrs == -1)
	أرجع;

    attrs |= FILE_ATTRIBUTE_HIDDEN;
    win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file "name" exists and is hidden.
 */
    صحيح
mch_ishidden(char_u *name)
{
    صحيح f = win32_getattrs(name);

    إذا (f == -1)
	أرجع FALSE;		    // file does not exist at all

    أرجع (f & FILE_ATTRIBUTE_HIDDEN) != 0;
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    صحيح
mch_isdir(char_u *name)
{
    صحيح f = win32_getattrs(name);

    إذا (f == -1)
	أرجع FALSE;		    // file does not exist at all

    أرجع (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if "name" is a directory or does not exist.
 * Return FALSE otherwise, also when the attributes cannot be obtained for
 * another reason.
 */
    صحيح
mch_isdir_or_missing(char_u *name)
{
    WCHAR	*p;
    DWORD	attr;
    DWORD	error = 0;

    p = enc_منزل_utf16(name, عدم);
    إذا (p == عدم)
	أرجع FALSE;
    attr = Get_File_گن_W(p);
    إذا (attr == INVALID_FILE_گن)
	error = Get_آخری_Error();
    vim_free(p);

    إذا (attr == INVALID_FILE_گن)
	أرجع error == ERROR_FILE_NOT_لبھیا || error == ERROR_رستہ_NOT_لبھیا;
    أرجع (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    صحيح
mch_isrealdir(char_u *name)
{
    أرجع mch_isdir(name) && !mch_is_symbolic_link(name);
}

/*
 * Create directory "name".
 * Return 0 on success, -1 on error.
 */
    صحيح
mch_mkdir(char_u *name)
{
    WCHAR   *p;
    صحيح	    retval;

    p = enc_منزل_utf16(name, عدم);
    إذا (p == عدم)
	أرجع -1;
    retval = _wmkdir(p);
    vim_free(p);
    أرجع retval;
}

/*
 * Delete directory "name".
 * Return 0 on success, -1 on error.
 */
    صحيح
mch_rmdir(char_u *name)
{
    WCHAR   *p;
    صحيح	    retval;

    p = enc_منزل_utf16(name, عدم);
    إذا (p == عدم)
	أرجع -1;
    retval = _wrmdir(p);
    vim_free(p);
    أرجع retval;
}

/*
 * Return TRUE if file "fname" has more than one link.
 */
    صحيح
mch_is_hard_link(char_u *fname)
{
    BY_HANDLE_FILE_INFORMATION info;

    أرجع win32_fileinfo(fname, &info) == FILEINFO_OK
						   && info.n_نمبر_دا_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    صحيح
mch_is_symbolic_link(char_u *name)
{
    HANDLE		h_لبھو;
    صحيح			res = FALSE;
    DWORD		fileFlags = 0, reparse_ٹیگ = 0;
    WCHAR		*wn;
    WIN32_لبھو_DATAW	لبھو_انکڑے_W;

    wn = enc_منزل_utf16(name, عدم);
    إذا (wn == عدم)
	أرجع FALSE;

    h_لبھو = لبھو_پہلا_File_W(wn, &لبھو_انکڑے_W);
    vim_free(wn);
    إذا (h_لبھو != INVALID_HANDLE_قدر)
    {
	fileFlags = لبھو_انکڑے_W.dw_File_گن;
	reparse_ٹیگ = لبھو_انکڑے_W.dwReserved0;
	لبھو_Close(h_لبھو);
    }

    إذا ((fileFlags & FILE_ATTRIBUTE_REPARSE_نقطہ)
	    && (reparse_ٹیگ == IO_REPARSE_ٹیگ_SYMLINK
		|| reparse_ٹیگ == IO_REPARSE_ٹیگ_MOUNT_نقطہ))
	res = TRUE;

    أرجع res;
}

/*
 * Return TRUE if file "fname" has more than one link or if it is a symbolic
 * link.
 */
    صحيح
mch_is_linked(char_u *fname)
{
    إذا (mch_is_hard_link(fname) || mch_is_symbolic_link(fname))
	أرجع TRUE;
    أرجع FALSE;
}

/*
 * Get the by-handle-file-information for "fname".
 * Returns FILEINFO_OK when OK.
 * Returns FILEINFO_ENC_FAIL when enc_to_utf16() failed.
 * Returns FILEINFO_READ_FAIL when CreateFile() failed.
 * Returns FILEINFO_INFO_FAIL when GetFileInformationByHandle() failed.
 */
    صحيح
win32_fileinfo(char_u *fname, BY_HANDLE_FILE_INFORMATION *info)
{
    HANDLE	hFile;
    صحيح		res = FILEINFO_READ_FAIL;
    WCHAR	*wn;

    wn = enc_منزل_utf16(fname, عدم);
    إذا (wn == عدم)
	أرجع FILEINFO_ENC_FAIL;

    hFile = CreateFileW(wn,	// file name
	    GENERIC_READ,	// access mode
	    FILE_SHARE_READ | FILE_SHARE_WRITE,	// share mode
	    عدم,		// security descriptor
	    OPEN_EXISTING,	// creation disposition
	    FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
	    عدم);		// handle to template file
    vim_free(wn);

    إذا (hFile == INVALID_HANDLE_قدر)
	أرجع FILEINFO_READ_FAIL;

    إذا (GetFileInformationByHandle(hFile, info) != 0)
	res = FILEINFO_OK;
    وإلا
	res = FILEINFO_INFO_FAIL;
    CloseHandle(hFile);

    أرجع res;
}

/*
 * get file attributes for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    ساكن صحيح
win32_getattrs(char_u *name)
{
    صحيح		attr;
    WCHAR	*p;

    p = enc_منزل_utf16(name, عدم);
    إذا (p == عدم)
	أرجع INVALID_FILE_گن;

    attr = Get_File_گن_W(p);
    vim_free(p);

    أرجع attr;
}

/*
 * set file attributes for `name' to `attrs'
 *
 * return -1 for failure, 0 otherwise
 */
    ساكن صحيح
win32_setattrs(char_u *name, صحيح attrs)
{
    صحيح	    res;
    WCHAR   *p;

    p = enc_منزل_utf16(name, عدم);
    إذا (p == عدم)
	أرجع -1;

    res = سیٹ_File_گن_W(p, attrs);
    vim_free(p);

    أرجع res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    ساكن صحيح
win32_سیٹ_archive(char_u *name)
{
    صحيح attrs = win32_getattrs(name);
    إذا (attrs == -1)
	أرجع -1;

    attrs |= FILE_ATTRIBUTE_ARCHIVE;
    أرجع win32_setattrs(name, attrs);
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 * Strange semantics of Win32: a readonly directory is writable, but you can't
 * delete a file.  Let's say this means it is writable.
 */
    صحيح
mch_writable(char_u *name)
{
    صحيح attrs = win32_getattrs(name);

    أرجع (attrs != -1 && (!(attrs & FILE_ATTRIBUTE_READONLY)
			  || (attrs & FILE_ATTRIBUTE_DIRECTORY)));
}

/*
 * Return TRUE if "name" can be executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 */
    صحيح
mch_can_exe(char_u *name, char_u **رستہ, صحيح use_رستہ بن_ورتیا)
{
    أرجع executable_exists((حرف *)name, STRLEN(name), رستہ, TRUE, TRUE);
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    صحيح
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    صحيح		type;
    WCHAR	*wn;

    // We can't open a file with a name "\\.\con" or "\\.\prn" and trying to
    // read from it later will cause Vim to hang.  Thus return NODE_WRITABLE
    // here.
    إذا (STRNCMP(name, "\\\\.\\", 4) == 0)
	أرجع گرہ_WRITABLE;

    wn = enc_منزل_utf16(name, عدم);
    إذا (wn == عدم)
	أرجع گرہ_عام;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    عدم,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    عدم);		    // handle to template file
    vim_free(wn);
    إذا (hFile == INVALID_HANDLE_قدر)
	أرجع گرہ_عام;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    إذا (type == FILE_TYPE_CHAR)
	أرجع گرہ_WRITABLE;
    إذا (type == FILE_TYPE_DISK)
	أرجع گرہ_عام;
    أرجع گرہ_OTHER;
}

#جے_تعریف_شدہ HAVE_ACL
بنية my_acl
{
    PSECURITY_DESCRIPTOR    pSecurityDescriptor;
    PSID		    pSidOwner;
    PSID		    p_Sid_گروہ;
    PACL		    pDacl;
    PACL		    pSacl;
};
#جے_انت

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname)
{
#جے_بنا_تعریف HAVE_ACL
    أرجع (vim_acl_T)عدم;
#وإلا
    بنية my_acl   *p = عدم;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(بنية my_acl);
    إذا (p != عدم)
    {
	WCHAR	*wn;

	wn = enc_منزل_utf16(fname, عدم);
	إذا (wn == عدم)
	{
	    vim_free(p);
	    أرجع عدم;
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
	إذا (err == ERROR_ACCESS_DENIED ||
		err == ERROR_PRIVILEGE_NOT_HELD)
	{
	    // Retrieve only DACL.
	    (فراغ)GetNamedSecurityInfoW(
		    wn,
		    SE_FILE_OBJECT,
		    DACL_SECURITY_INFORMATION,
		    عدم,
		    عدم,
		    &p->pDacl,
		    عدم,
		    &p->pSecurityDescriptor);
	}
	إذا (p->pSecurityDescriptor == عدم)
	{
	    mch_free_acl((vim_acl_T)p);
	    p = عدم;
	}
	vim_free(wn);
    }

    أرجع (vim_acl_T)p;
#جے_انت
}

#جے_تعریف_شدہ HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    ساكن BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_ناپ_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_گنتی = 0;
    GetAclInformation(acl, &acl_info, حجم(acl_info), Acl_ناپ_Information);
    لكل (i = 0; i < acl_info.Ace_گنتی; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	إذا (ace->سرخی_2.AceFlags & INHERITED_ACE)
	    أرجع TRUE;
    }
    أرجع FALSE;
}
#جے_انت

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    فراغ
mch_سیٹ_acl(char_u *fname, vim_acl_T acl)
{
#جے_تعریف_شدہ HAVE_ACL
    بنية my_acl   *p = (بنية my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    إذا (p == عدم)
	أرجع;

    wn = enc_منزل_utf16(fname, عدم);
    إذا (wn == عدم)
	أرجع;

    // Set security flags
    إذا (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    إذا (p->p_Sid_گروہ)
	sec_info |= گروہ_SECURITY_INFORMATION;
    إذا (p->pDacl)
    {
	sec_info |= DACL_SECURITY_INFORMATION;
	// Do not inherit its parent's DACL.
	// If the DACL is inherited, Cygwin permissions would be changed.
	إذا (!is_acl_inherited(p->pDacl))
	    sec_info |= PROTECTED_DACL_SECURITY_INFORMATION;
    }
    إذا (p->pSacl)
	sec_info |= SACL_SECURITY_INFORMATION;

    (فراغ)سیٹ_Named_Security_Info_W(
	    wn,			// Abstract filename
	    SE_FILE_OBJECT,	// File Object
	    sec_info,
	    p->pSidOwner,	// Ownership information.
	    p->p_Sid_گروہ,	// Group membership.
	    p->pDacl,		// Discretionary information.
	    p->pSacl		// For auditing purposes.
	    );
    vim_free(wn);
#جے_انت
}

    فراغ
mch_free_acl(vim_acl_T acl)
{
#جے_تعریف_شدہ HAVE_ACL
    بنية my_acl   *p = (بنية my_acl *)acl;

    إذا (p != عدم)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#جے_انت
}

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    ساكن BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    داخلہ_ریکارڈ ir;
    DWORD باہرلا;

    بدل (dwCtrlType)
    {
    حالة CTRL_C_EVENT:
	إذا (ctrl_c_interrupts)
	    g_fCtrlCPressed = TRUE;
	أرجع TRUE;

    حالة CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	ctrl_break_was_pressed = TRUE;
	// ReadConsoleInput is blocking, send a key event to continue.
	ir.EventType = کلید_EVENT;
	ir.Event.کلید_Event_2.b_کلید_تھلے = TRUE;
	ir.Event.کلید_Event_2.w_Repeat_گنتی = 1;
	ir.Event.کلید_Event_2.w_Virtual_کلید_کوڈ = VK_CANCEL;
	ir.Event.کلید_Event_2.w_Virtual_جانچ_کوڈ = 0;
	ir.Event.کلید_Event_2.dw_Control_کلید_State = 0;
	ir.Event.کلید_Event_2.uChar.UnicodeChar = 0;
	Write_Console_داخلہ(g_h_Con_وچ, &ir, 1, &باہرلا);
	أرجع TRUE;

    // fatal events: shut down gracefully
    حالة CTRL_CLOSE_EVENT:
    حالة CTRL_LOGOFF_EVENT:
    حالة CTRL_SHUTDOWN_EVENT:
	windgoto((صحيح)صفاں - 1, cmdline_خانہ_off);
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

	أرجع TRUE;		// not reached

    افتراضي:
	أرجع FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    فراغ
mch_settmode(tmode_T tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

# ifdef VIMDLL
    إذا (gui.وچ_use)
	أرجع;
# endif
    Get_Console_منوال(g_h_Con_وچ, &cmodein);
    Get_Console_منوال(g_h_Con_باہرلا, &cmodeout);
    إذا (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_سطر_داخلہ | ENABLE_PROCESSED_داخلہ |
		     ENABLE_ECHO_داخلہ);
	إذا (g_f_Mouse_سرگرم)
	{
	    cmodein |= ENABLE_MOUSE_داخلہ;
	    cmodein &= ~ENABLE_تیز_EDIT_منوال;
	}
	وإلا
	{
	    cmodein |= g_cmodein & ENABLE_تیز_EDIT_منوال;
	}
	cmodeout &= ~(
# ifdef FEAT_TERMGUICOLORS
	    // Do not turn off the ENABLE_PROCESSED_OUTPUT flag when using
	    // VTP.
	    ((vtp_working) ? 0 : ENABLE_PROCESSED_OUTPUT) |
# وإلا
	    ENABLE_PROCESSED_OUTPUT |
# endif
	    ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    وإلا // cooked
    {
	cmodein |= (ENABLE_سطر_داخلہ | ENABLE_PROCESSED_داخلہ |
		    ENABLE_ECHO_داخلہ);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    سیٹ_Console_منوال(g_h_Con_وچ, cmodein | ENABLE_EXTENDED_FLAGS);
    سیٹ_Console_منوال(g_h_Con_باہرلا, cmodeout);
    سیٹ_Console_Ctrl_Handler(handler_routine, bEnableHandler);

# ifdef MCH_WRITE_DUMP
    إذا (fdDump)
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
    صحيح
mch_get_shellsize(فراغ)
{
    CONSOLE_SCREEN_بفر_INFO csbi;

# ifdef VIMDLL
    إذا (gui.وچ_use)
	أرجع OK;
# endif
    إذا (!g_f_Termcap_منوال && g_cbTermcap.IsValid)
    {
	/*
	 * For some reason, we are trying to get the screen dimensions
	 * even though we are not in termcap mode.  The 'Rows' and 'Columns'
	 * variables are really intended to mean the size of Vim screen
	 * while in termcap mode.
	 */
	صفاں = g_cbTermcap.Info.dw_ناپ.Y;
	Columns = g_cbTermcap.Info.dw_ناپ.X;
    }
    وإلا إذا (Get_Console_Screen_بفر_Info(g_h_Con_باہرلا, &csbi))
    {
	صفاں = csbi.sr_کھڑکی.Bottom - csbi.sr_کھڑکی.چوٹی_2 + 1;
	Columns = csbi.sr_کھڑکی.دایاں_3 - csbi.sr_کھڑکی.بایاں_3 + 1;
    }
    وإلا
    {
	صفاں = 25;
	Columns = 80;
    }
    أرجع OK;
}

/*
 * Resize console buffer to 'COORD'
 */
    ساكن فراغ
ResizeConBuf(
    HANDLE  hConsole,
    COORD   coordScreen)
{
    إذا (use_alternate_screen_بفر)
	أرجع;

    إذا (!سیٹ_Console_Screen_بفر_ناپ(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	إذا (fdDump)
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
    ساكن فراغ
Resize_کھڑکی(
    HANDLE     hConsole,
    SMALL_RECT sr_کھڑکی_Rect)
{
    إذا (!سیٹ_Console_کھڑکی_Info(hConsole, TRUE, &sr_کھڑکی_Rect))
    {
# ifdef MCH_WRITE_DUMP
	إذا (fdDump)
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
    ساكن فراغ
Resize_Con_Buf_And_کھڑکی(
    HANDLE  hConsole,
    صحيح	    x_ناپ,
    صحيح	    y_ناپ_2)
{
    CONSOLE_SCREEN_بفر_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    sr_کھڑکی_Rect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    ساكن صحيح	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    إذا (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", x_ناپ, y_ناپ_2);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = Get_سبھ_توں_وڈا_Console_کھڑکی_ناپ(hConsole);

    // define the new console window size and scroll position
    sr_کھڑکی_Rect.بایاں_3 = sr_کھڑکی_Rect.چوٹی_2 = (SHORT) 0;
    sr_کھڑکی_Rect.دایاں_3 =  (SHORT) (min(x_ناپ, coordScreen.X) - 1);
    sr_کھڑکی_Rect.Bottom = (SHORT) (min(y_ناپ_2, coordScreen.Y) - 1);

    إذا (Get_Console_Screen_بفر_Info(g_h_Con_باہرلا, &csbi))
    {
	صحيح sx, sy;

	sx = csbi.sr_کھڑکی.دایاں_3 - csbi.sr_کھڑکی.بایاں_3 + 1;
	sy = csbi.sr_کھڑکی.Bottom - csbi.sr_کھڑکی.چوٹی_2 + 1;
	إذا (sy < y_ناپ_2 || sx < x_ناپ)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    إذا (sy < y_ناپ_2)
		coordScreen.Y = y_ناپ_2;
	    وإلا
		coordScreen.Y = sy;
	    إذا (sx < x_ناپ)
		coordScreen.X = x_ناپ;
	    وإلا
		coordScreen.X = sx;
	    سیٹ_Console_Screen_بفر_ناپ(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = x_ناپ;
    coordScreen.Y = y_ناپ_2;

    // In the new console call API, only the first time in reverse order
    إذا (!vtp_working || resized)
    {
	Resize_کھڑکی(hConsole, sr_کھڑکی_Rect);
	ResizeConBuf(hConsole, coordScreen);
    }
    وإلا
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
    فراغ
mch_سیٹ_shellsize(فراغ)
{
    COORD coordScreen;

# ifdef VIMDLL
    إذا (gui.وچ_use)
	أرجع;
# endif
    // Don't change window size while still starting up
    إذا (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	أرجع;
    }

    إذا (رکن_console)
    {
	coordScreen = Get_سبھ_توں_وڈا_Console_کھڑکی_ناپ(g_h_Con_باہرلا);

	// Clamp Rows and Columns to reasonable values
	إذا (صفاں > coordScreen.Y)
	    صفاں = coordScreen.Y;
	إذا (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	Resize_Con_Buf_And_کھڑکی(g_h_Con_باہرلا, Columns, صفاں);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    فراغ
mch_new_shellsize(فراغ)
{
# ifdef VIMDLL
    إذا (gui.وچ_use)
	أرجع;
# endif
    سیٹ_scroll_region(0, 0, Columns - 1, صفاں - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    فراغ
mch_سیٹ_winsize_now(فراغ)
{
    إذا (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_سیٹ_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# إذا defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    فراغ
mch_calc_cell_ناپ(بنية cellsize *cs_باہرلا)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_باہرلا->cs_xpixel = -1;
    cs_باہرلا->cs_ypixel = -1;

    إذا (g_h_Con_باہرلا == INVALID_HANDLE_قدر)
	أرجع;

    cfi.cb_ناپ = حجم(cfi);
    إذا (GetCurrentConsoleFontEx(g_h_Con_باہرلا, FALSE, &cfi)
	    && cfi.dw_Font_ناپ.X > 0 && cfi.dw_Font_ناپ.Y > 0)
    {
	cs_باہرلا->cs_xpixel = cfi.dw_Font_ناپ.X;
	cs_باہرلا->cs_ypixel = cfi.dw_Font_ناپ.Y;
	أرجع;
    }

    Get_Console_منوال(g_h_Con_وچ, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_سطر_داخلہ | ENABLE_ECHO_داخلہ
	    | ENABLE_PROCESSED_داخلہ);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_داخلہ;

    إذا (سیٹ_Console_منوال(g_h_Con_وچ, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	سیٹ_Console_منوال(g_h_Con_وچ, cmodeold);
    }
}
# endif

#جے_انت

    ساكن BOOL
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
    WCHAR	*wcmd, *wcwd = عدم;

    wcmd = enc_منزل_utf16((char_u *)cmd, عدم);
    إذا (wcmd == عدم)
	أرجع FALSE;
    إذا (cwd != عدم)
    {
	wcwd = enc_منزل_utf16((char_u *)cwd, عدم);
	إذا (wcwd == عدم)
	    اذهب theend;
    }

    ret = CreateProcessW(
	    عدم,		// Executable name
	    wcmd,		// Command to execute
	    عدم,		// Process security attributes
	    عدم,		// Thread security attributes
	    inherit_handles,	// Inherit handles
	    flags,		// Creation flags
	    env,		// Environment
	    wcwd,		// Current directory
	    (LPSTARTUPINFOW)si,	// Startup information
	    pi);		// Process information
theend:
    vim_free(wcmd);
    vim_free(wcwd);
    أرجع ret;
}


    ساكن HINSTANCE
vim_shell_execute(
    حرف *cmd,
    INT	 n_show_cmd)
{
    HINSTANCE	ret;
    WCHAR	*wcmd;

    wcmd = enc_منزل_utf16((char_u *)cmd, عدم);
    إذا (wcmd == عدم)
	أرجع (HINSTANCE) 0;

    ret = ShellExecuteW(عدم, عدم, wcmd, عدم, عدم, n_show_cmd);
    vim_free(wcmd);
    أرجع ret;
}


#إذا defined(FEAT_GUI_MSWIN)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    ساكن صحيح
mch_system_classic(حرف *cmd, صحيح options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = حجم(si);
    si.lpReserved = عدم;
    si.lpDesktop = عدم;
    si.lpTitle = عدم;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window.
     * Don't activate the window to keep focus on Vim.
     */
    إذا (options & SHELL_DOOUT)
	si.w_Show_کھڑکی = SW_SHOWMINNOACTIVE;
    وإلا
	si.w_Show_کھڑکی = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = عدم;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_منوال |	CREATE_NEW_CONSOLE,
	    &si, &pi, عدم, عدم);

    // Wait for the command to terminate before continuing
    {
	صحيح	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	لكل (;;)
	{
	    پیغام_2	پیغام;

	    إذا (Peek_سنیہا_W(&پیغام, (HWND)عدم, 0, 0, PM_REMOVE))
	    {
		Translate_سنیہا(&پیغام);
		Dispatch_سنیہا_W(&پیغام);
		delay = 1;
		استمر;
	    }
	    إذا (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
		اكسر;

	    // We start waiting for a very short time and then increase it, so
	    // that we respond quickly when the process is quick, and don't
	    // consume too much overhead when it's slow.
	    إذا (delay < 50)
		delay += 10;
	}

	// Get the command exit code
	Get_Exit_کوڈ_Process(pi.hProcess, &ret);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // Try to get input focus back.  Doesn't always work though.
    Post_سنیہا(hwnd, WM_SETFOCUS, 0, 0);
    // To increase the chances that WM_SETFOCUS will work, run the message loop
    // here.  In addition, it prevents problems caused by delayed WM_SETFOCUS
    // processing.
    gui_mch_update();

    أرجع ret;
}

/*
 * Thread launched by the gui to send the current buffer data to the
 * process. This way avoid to hang up vim totally if the children
 * process take a long time to process the lines.
 */
    ساكن بنانشانی صحيح __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_h_بچہ_Std_وچ_Wr = param;
    linenr_T	    lnum = curbuf->b_op_شروع.lnum;
    DWORD	    len = 0;
    DWORD	    l;
    char_u	    *lp = ml_get(lnum);
    char_u	    *s;
    صحيح		    written = 0;

    لكل (;;)
    {
	l = (DWORD)STRLEN(lp + written);
	إذا (l == 0)
	    len = 0;
	وإلا إذا (lp[written] == NL)
	{
	    // NL -> NUL translation
	    WriteFile(g_h_بچہ_Std_وچ_Wr, "", 1, &len, عدم);
	}
	وإلا
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_h_بچہ_Std_وچ_Wr, (حرف *)lp + written,
		      s == عدم ? l : (DWORD)(s - (lp + written)),
		      &len, عدم);
	}
	إذا (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    إذا (lnum != curbuf->b_op_سرا.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_سطر_گنتی
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_h_بچہ_Std_وچ_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, عدم);
	    }

	    ++lnum;
	    إذا (lnum > curbuf->b_op_سرا.lnum)
		اكسر;

	    lp = ml_get(lnum);
	    written = 0;
	}
	وإلا إذا (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_h_بچہ_Std_وچ_Wr);
    أرجع 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    ساكن فراغ
dump_pipe(صحيح	    options,
	  HANDLE    g_h_بچہ_Std_باہرلا_Rd,
	  garray_T  *ga,
	  char_u    بفر[],
	  DWORD	    *بفر_off)
{
    DWORD	دستیاب_بائٹاں = 0;
    DWORD	i;
    صحيح		ret;
    DWORD	len;
    DWORD	منزل_Read;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_h_بچہ_Std_باہرلا_Rd, // pipe to query
			عدم,		    // optional buffer
			0,		    // buffer size
			عدم,		    // number of read bytes
			&دستیاب_بائٹاں,    // available bytes total
			عدم);		    // byteLeft

    // We got real data in the pipe, read it
    طالما (ret != 0 && دستیاب_بائٹاں > 0)
    {
	منزل_Read = (DWORD)(BUFLEN - *بفر_off);
	منزل_Read = دستیاب_بائٹاں < منزل_Read ? دستیاب_بائٹاں : منزل_Read;
	ReadFile(g_h_بچہ_Std_باہرلا_Rd, بفر + *بفر_off, منزل_Read , &len, عدم);

	// If we haven't read anything, there is a problem
	إذا (len == 0)
	    اكسر;

	دستیاب_بائٹاں -= len;

	إذا (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    لكل (i = 0; i < len; ++i)
	    {
		إذا (بفر[i] == NL)
		    append_ga_سطر(ga);
		وإلا إذا (بفر[i] == NUL)
		    ga_append(ga, NL);
		وإلا
		    ga_append(ga, بفر[i]);
	    }
	}
	وإلا إذا (has_mbyte)
	{
	    صحيح		l;
	    صحيح		c;
	    char_u	*p;

	    len += *بفر_off;
	    بفر[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    لكل (p = بفر; p < بفر + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		إذا (l == 0)
		    l = 1;  // NUL byte?
		وإلا إذا (MB_BYTE2LEN(*p) != l)
		    اكسر;
	    }
	    إذا (p == بفر)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		إذا (len >= 12)
		    ++p;
		وإلا
		{
		    *بفر_off = len;
		    أرجع;
		}
	    }
	    c = *p;
	    *p = NUL;
	    پیغام_puts((حرف *)بفر);
	    إذا (p < بفر + len)
	    {
		*p = c;
		*بفر_off = (DWORD)((بفر + len) - p);
		mch_memmove(بفر, p, *بفر_off);
		أرجع;
	    }
	    *بفر_off = 0;
	}
	وإلا
	{
	    بفر[len] = NUL;
	    پیغام_puts((حرف *)بفر);
	}

	windgoto(پیغام_صف, cmdline_خانہ_off + پیغام_خانہ);
	cursor_on();
	باہرلا_flush();
    }
}

/*
 * Version of system to use for windows NT > 5.0 (Win2K), use pipe
 * for communication and doesn't open any new window.
 */
    ساكن صحيح
mch_system_piped(حرف *cmd, صحيح options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;

    HANDLE g_h_بچہ_Std_وچ_Rd = عدم;
    HANDLE g_h_بچہ_Std_وچ_Wr = عدم;
    HANDLE g_h_بچہ_Std_باہرلا_Rd = عدم;
    HANDLE g_h_بچہ_Std_باہرلا_Wr = عدم;

    char_u	بفر[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    صحيح		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    صحيح		noread_cnt = 0;
    garray_T	ga;
    صحيح		delay = 1;
    DWORD	بفر_off = 0;	// valid bytes in buffer[]
    حرف	*p = عدم;

    SECURITY_گن saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.n_لمائی = حجم(SECURITY_گن);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = عدم;

    إذا ( ! CreatePipe(&g_h_بچہ_Std_باہرلا_Rd, &g_h_بچہ_Std_باہرلا_Wr, &saAttr, 0)
	// Ensure the read handle to the pipe for STDOUT is not inherited.
       || ! سیٹ_Handle_Information(g_h_بچہ_Std_باہرلا_Rd, HANDLE_FLAG_INHERIT, 0)
	// Create a pipe for the child process's STDIN.
       || ! CreatePipe(&g_h_بچہ_Std_وچ_Rd, &g_h_بچہ_Std_وچ_Wr, &saAttr, 0)
	// Ensure the write handle to the pipe for STDIN is not inherited.
       || ! سیٹ_Handle_Information(g_h_بچہ_Std_وچ_Wr, HANDLE_FLAG_INHERIT, 0) )
    {
	CloseHandle(g_h_بچہ_Std_وچ_Rd);
	CloseHandle(g_h_بچہ_Std_وچ_Wr);
	CloseHandle(g_h_بچہ_Std_باہرلا_Rd);
	CloseHandle(g_h_بچہ_Std_باہرلا_Wr);
	پیغام_puts(_("\nCannot create pipes\n"));
    }

    si.cb = حجم(si);
    si.lpReserved = عدم;
    si.lpDesktop = عدم;
    si.lpTitle = عدم;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_h_بچہ_Std_باہرلا_Wr;
    si.hStdOutput = g_h_بچہ_Std_باہرلا_Wr;
    si.h_Std_داخلہ = g_h_بچہ_Std_وچ_Rd;
    si.w_Show_کھڑکی = SW_HIDE;
    si.cbReserved2 = 0;
    si.lpReserved2 = عدم;

    إذا (options & SHELL_READ)
	ga_init2(&ga, 1, BUFLEN);

    إذا (cmd != عدم)
    {
	p = (حرف *)vim_strsave((char_u *)cmd);
	إذا (p != عدم)
	    unescape_shellxquote((char_u *)p, p_sxe);
	وإلا
	    p = cmd;
    }

    // Now, run the command.
    // About "Inherit handles" being TRUE: this command can be litigious,
    // handle inheritance was deactivated for pending temp file, but, if we
    // deactivate it, the pipes don't work for some reason.
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_منوال,
						&si, &pi, عدم, عدم);

    إذا (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_h_بچہ_Std_وچ_Rd);
    CloseHandle(g_h_بچہ_Std_باہرلا_Wr);

    إذا (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(عدم,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_h_بچہ_Std_وچ_Wr,  // parameter
			0,		 // creation flag, start immediately
			عدم);		    // we don't care about thread id
	CloseHandle(thread);
	g_h_بچہ_Std_وچ_Wr = عدم;
    }

    // Keep updating the window while waiting for the shell to finish.
    لكل (;;)
    {
	پیغام_2	پیغام;

	إذا (Peek_سنیہا_W(&پیغام, (HWND)عدم, 0, 0, PM_REMOVE))
	{
	    Translate_سنیہا(&پیغام);
	    Dispatch_سنیہا_W(&پیغام);
	}

	// write pipe information in the window
	إذا ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.وچ_use
# endif
	    )
	{
	    len = 0;
	    إذا (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.وچ_use
# endif
		    )
		&& (ta_len > 0 || noread_cnt > 4))
	    {
		إذا (ta_len == 0)
		{
		    // Get extra characters when we don't have any.  Reset the
		    // counter and timer.
		    noread_cnt = 0;
		    len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		}
		إذا (ta_len > 0 || len > 0)
		{
		    /*
		     * For pipes: Check for CTRL-C: send interrupt signal to
		     * child.
		     */
		    إذا (len == 1 && cmd != عدم)
		    {
			إذا (ta_buf[ta_len] == Ctrl_C)
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
		    إذا ((len == 1 || len == 4 ) && cmd != عدم)
		    {
			إذا (ta_buf[0] == Ctrl_D
			    || (ta_buf[0] == CSI
				&& ta_buf[1] == KS_MODIFIER
				&& ta_buf[3] == Ctrl_D))
			{
			    CloseHandle(g_h_بچہ_Std_وچ_Wr);
			    g_h_بچہ_Std_وچ_Wr = عدم;
			    len = 0;
			}
		    }

		    len = رکن_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    لكل (i = ta_len; i < ta_len + len; ++i)
		    {
			إذا (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    پیغام_putchar(ta_buf[i]);
			وإلا إذا (has_mbyte)
			{
			    صحيح l = (*mb_ptr2len)(ta_buf + i);

			    پیغام_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			وإلا
			    پیغام_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(پیغام_صف, cmdline_خانہ_off + پیغام_خانہ);
		    باہرلا_flush();

		    ta_len += len;

		    /*
		     * Write the characters to the child, unless EOF has been
		     * typed for pipes.  Write one character at a time, to
		     * avoid losing too much typeahead.  When writing buffer
		     * lines, drop the typed characters (only check for
		     * CTRL-C).
		     */
		    إذا (options & SHELL_WRITE)
			ta_len = 0;
		    وإلا إذا (g_h_بچہ_Std_وچ_Wr != عدم)
		    {
			WriteFile(g_h_بچہ_Std_وچ_Wr, (حرف*)ta_buf,
				    1, &len, عدم);
			// if we are typing in, we want to keep things reactive
			delay = 1;
			إذا (len > 0)
			{
			    ta_len -= len;
			    mch_memmove(ta_buf, ta_buf + len, ta_len);
			}
		    }
		}
	    }
	}

	إذا (ta_len)
	    ui_inchar_undo(ta_buf, ta_len);

	إذا (WaitForSingleObject(pi.hProcess, delay) != WAIT_TIMEOUT)
	{
	    dump_pipe(options, g_h_بچہ_Std_باہرلا_Rd, &ga, بفر, &بفر_off);
	    اكسر;
	}

	++noread_cnt;
	dump_pipe(options, g_h_بچہ_Std_باہرلا_Rd, &ga, بفر, &بفر_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	إذا (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_h_بچہ_Std_باہرلا_Rd);
    إذا (g_h_بچہ_Std_وچ_Wr != عدم)
	CloseHandle(g_h_بچہ_Std_وچ_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    Get_Exit_کوڈ_Process(pi.hProcess, &ret);

    إذا (options & SHELL_READ)
    {
	إذا (ga.ga_len > 0)
	{
	    append_ga_سطر(&ga);
	    // remember that the NL was missing
	    curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
	}
	وإلا
	    curbuf->b_no_eol_lnum = 0;
	ga_clear(&ga);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    أرجع ret;
}

    ساكن صحيح
mch_system_g(حرف *cmd, صحيح options)
{
    // if we can pipe and the shelltemp option is off
    إذا (!p_stmp)
	أرجع mch_system_piped(cmd, options);
    وإلا
	أرجع mch_system_classic(cmd, options);
}
#جے_انت

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ساكن صحيح
mch_system_c(حرف *cmd, صحيح options بن_ورتیا)
{
    صحيح		ret;
    WCHAR	*wcmd;
    char_u	*buf;
    size_t	len;

    // If the command starts and ends with double quotes, enclose the command
    // in parentheses.
    len = STRLEN(cmd);
    إذا (len >= 2 && cmd[0] == '"' && cmd[len - 1] == '"')
    {
	len += 3;
	buf = alloc(len);
	إذا (buf == عدم)
	    أرجع -1;
	vim_snprintf((حرف *)buf, len, "(%s)", cmd);
	wcmd = enc_منزل_utf16(buf, عدم);
	حرر(buf);
    }
    وإلا
	wcmd = enc_منزل_utf16((char_u *)cmd, عدم);

    إذا (wcmd == عدم)
	أرجع -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    أرجع ret;
}

#جے_انت

    ساكن صحيح
mch_system(حرف *cmd, صحيح options)
{
#جے_تعریف_شدہ VIMDLL
    إذا (gui.وچ_use || gui.starting)
	أرجع mch_system_g(cmd, options);
    وإلا
	أرجع mch_system_c(cmd, options);
#وإلاإذا defined(FEAT_GUI_MSWIN)
    أرجع mch_system_g(cmd, options);
#وإلا
    أرجع mch_system_c(cmd, options);
#جے_انت
}

#إذا defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ساكن صحيح
mch_call_shell_terminal(
    char_u	*cmd,
    صحيح		options بن_ورتیا)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = عدم;
    typval_T	argvar[2];
    long_u	cmdlen;
    صحيح		retval = -1;
    buf_T	*buf;
    کم_T	*کم;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    إذا (cmd == عدم)
	cmdlen = STRLEN(p_sh) + 1;
    وإلا
	cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
    newcmd = alloc(cmdlen);
    إذا (newcmd == عدم)
	أرجع 255;
    إذا (cmd == عدم)
    {
	STRCPY(newcmd, p_sh);
	ch_log(عدم, "starting terminal to run a shell");
    }
    وإلا
    {
	vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	ch_log(عدم, "starting terminal for system command '%s'", cmd);
    }

    init_کم_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = رکن_شروع(argvar, عدم, &opt, رکن_شروع_SYSTEM);
    إذا (buf == عدم)
    {
	vim_free(newcmd);
	أرجع 255;
    }

    کم = رکن_getjob(buf->b_رکن);
    ++کم->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    إذا (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	طالما (رکن_use_loop())
	{
	    إذا (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_سرگرم)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		إذا (terminal_loop(TRUE) == OK)
		    عام_cmd(&oa, TRUE);
	    }
	    وإلا
		عام_cmd(&oa, TRUE);
	}
	retval = کم->jv_exitval;
	ch_log(عدم, "system command finished");

	کم_unref(کم);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_بفر(DOBUF_WIPE, DOBUF_پہلا, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    أرجع retval;
}
#جے_انت
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    ساكن فراغ
restore_env_var(char_u *name, char_u *old_قدر, صحيح must_free)
{
    إذا (old_قدر != عدم)
    {
	vim_setenv(name, old_قدر);
	إذا (must_free)
	    vim_free(old_قدر);
	أرجع;
    }
    vim_unsetenv(name);
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    صحيح
mch_call_shell(
    char_u  *cmd,
    صحيح	    options)	// SHELL_*, see vim.h
{
    صحيح		x = 0;
    صحيح		tmode = cur_tmode;
    WCHAR	szShellTitle[512];
    صحيح		must_free;
    char_u	*oldval;

#جے_تعریف_شدہ FEAT_EVAL
    ch_log(عدم, "executing shell command: %s", cmd);
#جے_انت
    // Change the title to reflect that we are in a subshell.
    إذا (GetConsoleTitleW(szShellTitle, ARRAY_لمائی(szShellTitle) - 4) > 0)
    {
	إذا (cmd == عدم)
	    wcscat(szShellTitle, L" :sh");
	وإلا
	{
	    WCHAR *wn = enc_منزل_utf16((char_u *)cmd, عدم);

	    إذا (wn != عدم)
	    {
		wcscat(szShellTitle, L" - !");
		إذا ((wcslen(szShellTitle) + wcslen(wn) <
			    ARRAY_لمائی(szShellTitle)))
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

    باہرلا_flush();

#جے_تعریف_شدہ MCH_WRITE_DUMP
    إذا (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#جے_انت
#إذا defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    إذا (
# ifdef VIMDLL
	    gui.وچ_use &&
# endif
	    vim_strchr(p_go, GO_TERMINAL) != عدم
	 && (options & (SHELL_FILTER|SHELL_DOOUT|SHELL_WRITE|SHELL_READ)) == 0)
    {
	char_u	*cmdbase = cmd;

	إذا (cmdbase != عدم)
	    // Skip a leading quote and (.
	    طالما (*cmdbase == '"' || *cmdbase == '(')
		++cmdbase;

	// Check the command does not begin with "start "
	إذا (cmdbase == عدم || STRNICMP(cmdbase, "start", 5) != 0
						   || !VIM_ISWHITE(cmdbase[5]))
	{
	    // Use a terminal window to run the command in.
	    x = mch_call_shell_terminal(cmd, options);
	    resettitle();
	    restore_env_var((char_u *)"NoDefaultCurrentDirectoryInExePath",
		    oldval, must_free);
	    أرجع x;
	}
    }
#جے_انت

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

    إذا (options & SHELL_COOKED)
	settmode(TMODE_COOK);	// set to normal mode

    إذا (cmd == عدم)
    {
	x = mch_system((حرف *)p_sh, options);
    }
    وإلا
    {
	// we use "command" or "cmd" to start the shell; slow but easy
	char_u	*newcmd = عدم;
	char_u	*cmdbase = cmd;
	long_u	cmdlen;

	// Skip a leading ", ( and "(.
	إذا (*cmdbase == '"' )
	    ++cmdbase;
	إذا (*cmdbase == '(')
	    ++cmdbase;

	إذا ((STRNICMP(cmdbase, "start", 5) == 0) && VIM_ISWHITE(cmdbase[5]))
	{
	    STARTUPINFO		si;
	    PROCESS_INFORMATION	pi;
	    DWORD		flags = CREATE_NEW_CONSOLE;
	    INT			n_show_cmd = SW_SHOWNORMAL;
	    char_u		*p;

	    Zero_حافظہ(&si, حجم(si));
	    si.cb = حجم(si);
	    si.lpReserved = عدم;
	    si.lpDesktop = عدم;
	    si.lpTitle = عدم;
	    si.dwFlags = 0;
	    si.cbReserved2 = 0;
	    si.lpReserved2 = عدم;

	    cmdbase = skipwhite(cmdbase + 5);
	    إذا ((STRNICMP(cmdbase, "/min", 4) == 0)
		    && VIM_ISWHITE(cmdbase[4]))
	    {
		cmdbase = skipwhite(cmdbase + 4);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.w_Show_کھڑکی = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    وإلا إذا ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_کھڑکی;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_داخلہ = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    عدم,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_عام,		// File att.
		    عدم);				// Temp file
		si.hStdOutput = si.h_Std_داخلہ;
		si.hStdError = si.h_Std_داخلہ;
	    }

	    // Remove a trailing ", ) and )" if they have a match
	    // at the start of the command.
	    إذا (cmdbase > cmd)
	    {
		p = cmdbase + STRLEN(cmdbase);
		إذا (p > cmdbase && p[-1] == '"' && *cmd == '"')
		    *--p = NUL;
		إذا (p > cmdbase && p[-1] == ')'
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
	    إذا (flags != CREATE_NEW_CONSOLE)
	    {
		char_u	*subcmd;
		char_u	*cmd_shell = mch_getenv("COMSPEC");

		إذا (cmd_shell == عدم || *cmd_shell == NUL)
		    cmd_shell = (char_u *)default_shell();

		subcmd = vim_strsave_escaped_ext(cmdbase,
			(char_u *)"|", '^', FALSE);
		إذا (subcmd != عدم)
		{
		    // make "cmd.exe /c arguments"
		    cmdlen = STRLEN(cmd_shell) + STRLEN(subcmd) + 5;
		    newcmd = alloc(cmdlen);
		    إذا (newcmd != عدم)
			vim_snprintf((حرف *)newcmd, cmdlen, "%s /c %s",
						       cmd_shell, subcmd);
		    وإلا
			newcmd = cmdbase;
		    vim_free(subcmd);
		}
	    }

	    /*
	     * Now, start the command as a process, so that it doesn't
	     * inherit our handles which causes unpleasant dangling swap
	     * files if we exit before the spawned process
	     */
	    إذا (vim_create_process((حرف *)newcmd, FALSE, flags,
			&si, &pi, عدم, عدم))
		x = 0;
	    وإلا إذا (vim_shell_execute((حرف *)newcmd, n_show_cmd)
							       > (HINSTANCE)32)
		x = 0;
	    وإلا
	    {
		x = -1;
#جے_تعریف_شدہ FEAT_GUI_MSWIN
# ifdef VIMDLL
		إذا (gui.وچ_use)
# endif
		    emsg(_(e_command_not_لبھیا));
#جے_انت
	    }

	    إذا (newcmd != cmdbase)
		vim_free(newcmd);

	    إذا (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_داخلہ != عدم)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_داخلہ);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	وإلا
	{
	    cmdlen =
#جے_تعریف_شدہ FEAT_GUI_MSWIN
		((gui.وچ_use || gui.starting) ?
		    (!s_dont_use_vimrun && p_stmp ?
			vimrun_رستہ.لمائی : STRLEN(p_sh) + STRLEN(p_shcf))
		    : 0) +
#جے_انت
		STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;

	    newcmd = alloc(cmdlen);
	    إذا (newcmd != عدم)
	    {
#إذا defined(FEAT_GUI_MSWIN)
		إذا (
# ifdef VIMDLL
		    (gui.وچ_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    حرف *پیغام = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    حرف *title = _("Vim Warning");
		    WCHAR *wmsg = enc_منزل_utf16((char_u *)پیغام, عدم);
		    WCHAR *wtitle = enc_منزل_utf16((char_u *)title, عدم);

		    إذا (wmsg != عدم && wtitle != عدم)
			سنیہا_Box_W(عدم, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		إذا (
# ifdef VIMDLL
		    (gui.وچ_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((حرف *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_رستہ.string,
			    (پیغام_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		وإلا إذا (
# ifdef VIMDLL
			(gui.وچ_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		وإلا
#جے_انت
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

    إذا (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }

    // Print the return value, unless "vimrun" was used.
    إذا (x != 0 && !(options & SHELL_SILENT) && !emsg_silent
#إذا defined(FEAT_GUI_MSWIN)
	    && ((gui.وچ_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#جے_انت
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

    أرجع x;
}

#إذا defined(FEAT_کم_CHANNEL)
    ساكن HANDLE
کم_io_file_open(
	char_u *fname,
	DWORD dwDesiredAccess,
	DWORD dw_Share_منوال,
	LPSECURITY_گن lp_Security_گن,
	DWORD dwCreationDisposition,
	DWORD dw_Flags_And_گن)
{
    HANDLE h;
    WCHAR *wn;

    wn = enc_منزل_utf16(fname, عدم);
    إذا (wn == عدم)
	أرجع INVALID_HANDLE_قدر;

    h = CreateFileW(wn, dwDesiredAccess, dw_Share_منوال,
	    lp_Security_گن, dwCreationDisposition,
	    dw_Flags_And_گن, عدم);
    vim_free(wn);
    أرجع h;
}

/*
 * Turn the dictionary "env" into a NUL separated list that can be used as the
 * environment argument of vim_create_process().
 */
    فراغ
win32_build_env(dict_T *env, garray_T *gap, صحيح is_terminal)
{
    hashitem_T	*hi;
    long_u	todo = env != عدم ? env->dv_hashtab.ht_ورتیا : 0;
    LPVOID	ادھار = Get_Environment_لڑیاں_W();

    // for last \0
    إذا (ga_grow(gap, 1) == FAIL)
	أرجع;

    إذا (env != عدم)
    {
	FOR_ALL_HASHTAB_اشیاء(&env->dv_hashtab, hi, todo)
	{
	    إذا (!HASHITEM_خالی_تھاواں(hi))
	    {
		typval_T *شے = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_منزل_utf16((char_u *)hi->hi_کلید, عدم);
		WCHAR   *wval = enc_منزل_utf16(tv_get_string(شے), عدم);
		--todo;
		إذا (wkey != عدم && wval != عدم)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    إذا (ga_grow(gap, (صحيح)(lkey + lval + 2)) == FAIL)
			استمر;
		    لكل (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_انکڑے + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_انکڑے + gap->ga_len++) = L'=';
		    لكل (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_انکڑے + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_انکڑے + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    إذا (ادھار)
    {
	WCHAR	*p = (WCHAR*) ادھار;

	// for last \0
	إذا (ga_grow(gap, 1) == FAIL)
	    أرجع;

	طالما (*p != 0 || *(p + 1) != 0)
	{
	    إذا (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_انکڑے + gap->ga_len++) = *p;
	    p++;
	}
	Free_Environment_لڑیاں_W(ادھار);
	*((WCHAR*)gap->ga_انکڑے + gap->ga_len++) = L'\0';
    }

# إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
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

	إذا (ga_grow(gap, (صحيح)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    لكل (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_انکڑے + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    لكل (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_انکڑے + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_انکڑے + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    إذا (is_terminal)
	    {
		لكل (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_انکڑے + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		لكل (n = 0; n < نسخہ_len; n++)
		    *((WCHAR*)gap->ga_انکڑے + gap->ga_len++) =
			(WCHAR)نسخہ_2[n];
		*((WCHAR*)gap->ga_انکڑے + gap->ga_len++) = L'\0';
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
    ساكن BOOL
create_pipe_pair(HANDLE handles[2])
{
    ساكن LONG		s;
    حرف		name[64];
    SECURITY_گن sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_ناپ, 0, 0, عدم);

    إذا (handles[1] == INVALID_HANDLE_قدر)
	أرجع FALSE;

    sa.n_لمائی = حجم(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = عدم;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_عام, 0);

    إذا (handles[0] == INVALID_HANDLE_قدر)
    {
	CloseHandle(handles[1]);
	أرجع FALSE;
    }

    أرجع TRUE;
}

#جے_انت // FEAT_JOB_CHANNEL

#إذا defined(FEAT_EVAL)
/*
 * Execute "argv" directly without the shell and return the output.
 * Used by system() and systemlist() when the command is a List.
 * "infile" is an optional temp file for stdin input.
 * When "ret_len" is not NULL, set it to the length of the output.
 * Returns the output in allocated memory (or NULL on error).
 * Sets v:shell_error to the exit status.
 */
    char_u *
mch_get_cmd_output_سدھا(
    حرف	**argv,
    char_u	*infile,
    صحيح		flags بن_ورتیا,
    صحيح		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_گن saAttr;
    HANDLE		h_بچہ_Stdout_Rd = INVALID_HANDLE_قدر;
    HANDLE		h_بچہ_Stdout_Wr = INVALID_HANDLE_قدر;
    HANDLE		h_بچہ_Stdin_Rd = INVALID_HANDLE_قدر;
    garray_T		cmd_ga;
    garray_T		باہرلا_ga;
    char_u		*بفر = عدم;
    DWORD		exit_کوڈ = (DWORD)-1;
    صحيح			i;

    // Build a command string from argv.
    ga_init2(&cmd_ga, 1, 256);
    لكل (i = 0; argv[i] != عدم; i++)
    {
	char_u	*arg = (char_u *)argv[i];
	char_u	*s = arg;
	صحيح	has_spaces = FALSE;
	صحيح	j;

	لكل (j = 0; s[j] != NUL; j++)
	    إذا (s[j] == ' ' || s[j] == '\t' || s[j] == '"')
	    {
		has_spaces = TRUE;
		اكسر;
	    }

	إذا (i > 0)
	    ga_append(&cmd_ga, ' ');

	إذا (has_spaces)
	{
	    صحيح	عدد_bs;

	    ga_append(&cmd_ga, '"');
	    لكل (j = 0; arg[j] != NUL; j++)
	    {
		عدد_bs = 0;
		طالما (arg[j] == '\\')
		{
		    عدد_bs++;
		    j++;
		}

		إذا (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    طالما (عدد_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    اكسر;
		}
		وإلا إذا (arg[j] == '"')
		{
		    // Backslashes before a double quote must be doubled,
		    // and the double quote must be escaped.
		    طالما (عدد_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		وإلا
		{
		    طالما (عدد_bs-- > 0)
			ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, arg[j]);
		}
	    }
	    ga_append(&cmd_ga, '"');
	}
	وإلا
	    ga_concat(&cmd_ga, arg);
    }
    ga_append(&cmd_ga, NUL);

    ga_init2(&باہرلا_ga, 1, 4096);

    saAttr.n_لمائی = حجم(SECURITY_گن);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = عدم;

    // Create a pipe for the child's stdout.
    إذا (!CreatePipe(&h_بچہ_Stdout_Rd, &h_بچہ_Stdout_Wr, &saAttr, 0)
	    || !سیٹ_Handle_Information(h_بچہ_Stdout_Rd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	اذهب پورا;
    }

    // Set up stdin from infile if provided.
    إذا (infile != عدم)
    {
	WCHAR *winfile = enc_منزل_utf16(infile, عدم);

	إذا (winfile != عدم)
	{
	    h_بچہ_Stdin_Rd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_عام, عدم);
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

    ch_log(عدم, "directly executing: %s", (حرف *)cmd_ga.ga_انکڑے);

    // Create the child process directly, without going through the shell.
    إذا (!vim_create_process((حرف *)cmd_ga.ga_انکڑے, TRUE,
		CREATE_DEFAULT_ERROR_منوال | CREATE_NEW_PROCESS_گروہ,
		&si, &pi, عدم, عدم))
    {
	semsg(_(e_invalid_دلیل_str), cmd_ga.ga_انکڑے);
	اذهب پورا;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(h_بچہ_Stdout_Wr);
    h_بچہ_Stdout_Wr = INVALID_HANDLE_قدر;
    إذا (h_بچہ_Stdin_Rd != INVALID_HANDLE_قدر)
    {
	CloseHandle(h_بچہ_Stdin_Rd);
	h_بچہ_Stdin_Rd = INVALID_HANDLE_قدر;
    }

    // Read output from child process.
    لكل (;;)
    {
	حرف	buf[4096];
	DWORD	n;

	إذا (!ReadFile(h_بچہ_Stdout_Rd, buf, حجم(buf), &n, عدم) || n == 0)
	    اكسر;
	إذا (ga_grow(&باہرلا_ga, (صحيح)n) == OK)
	{
	    mch_memmove((حرف *)باہرلا_ga.ga_انکڑے + باہرلا_ga.ga_len, buf, n);
	    باہرلا_ga.ga_len += (صحيح)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    Get_Exit_کوڈ_Process(pi.hProcess, &exit_کوڈ);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    سیٹ_vim_var_nr(VV_SHELL_ERROR, (لما)exit_کوڈ);

    إذا (باہرلا_ga.ga_len > 0)
    {
	بفر = alloc(باہرلا_ga.ga_len + 1);
	إذا (بفر != عدم)
	{
	    mch_memmove(بفر, باہرلا_ga.ga_انکڑے, باہرلا_ga.ga_len);
	    إذا (ret_len == عدم)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		لكل (i = 0; i < باہرلا_ga.ga_len; ++i)
		    إذا (بفر[i] == NUL)
			بفر[i] = 1;
		بفر[باہرلا_ga.ga_len] = NUL;
	    }
	    وإلا
		*ret_len = باہرلا_ga.ga_len;
	}
    }
    وإلا إذا (ret_len != عدم)
	*ret_len = 0;

پورا:
    ga_clear(&cmd_ga);
    ga_clear(&باہرلا_ga);
    إذا (h_بچہ_Stdout_Rd != INVALID_HANDLE_قدر)
	CloseHandle(h_بچہ_Stdout_Rd);
    إذا (h_بچہ_Stdout_Wr != INVALID_HANDLE_قدر)
	CloseHandle(h_بچہ_Stdout_Wr);
    إذا (h_بچہ_Stdin_Rd != INVALID_HANDLE_قدر)
	CloseHandle(h_بچہ_Stdin_Rd);
    أرجع بفر;
}
#جے_انت // FEAT_EVAL

#إذا defined(FEAT_کم_CHANNEL)
    فراغ
mch_کم_شروع(حرف *cmd, کم_T *کم, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_گن saAttr;
    channel_T		*channel = عدم;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    صحيح		use_null_for_وچ = options->jo_io[PART_وچ] == JIO_NULL;
    صحيح		use_null_for_باہرلا = options->jo_io[PART_باہرلا] == JIO_NULL;
    صحيح		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    صحيح		use_file_for_وچ = options->jo_io[PART_وچ] == JIO_FILE;
    صحيح		use_file_for_باہرلا = options->jo_io[PART_باہرلا] == JIO_FILE;
    صحيح		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    صحيح		use_باہرلا_for_err = options->jo_io[PART_ERR] == JIO_باہرلا;

    إذا (use_باہرلا_for_err && use_null_for_باہرلا)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_قدر;
    ifd[1] = INVALID_HANDLE_قدر;
    ofd[0] = INVALID_HANDLE_قدر;
    ofd[1] = INVALID_HANDLE_قدر;
    efd[0] = INVALID_HANDLE_قدر;
    efd[1] = INVALID_HANDLE_قدر;
    ga_init2(&ga, حجم(wchar_t), 500);

    jo = Create_کم_Object(عدم, عدم);
    إذا (jo == عدم)
    {
	کم->jv_حالت = کم_FAILED;
	اذهب failed;
    }

    إذا (options->jo_env != عدم)
	win32_build_env(options->jo_env, &ga, FALSE);

    Zero_حافظہ(&pi, حجم(pi));
    Zero_حافظہ(&si, حجم(si));
    si.cb = حجم(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.w_Show_کھڑکی = SW_HIDE;

    saAttr.n_لمائی = حجم(SECURITY_گن);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = عدم;

    إذا (use_file_for_وچ)
    {
	char_u *fname = options->jo_io_name[PART_وچ];

	ifd[0] = کم_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_عام);
	إذا (ifd[0] == INVALID_HANDLE_قدر)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    وإلا إذا (!use_null_for_وچ
	    && (!create_pipe_pair(ifd)
		|| !سیٹ_Handle_Information(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	اذهب failed;

    إذا (use_file_for_باہرلا)
    {
	char_u *fname = options->jo_io_name[PART_باہرلا];

	ofd[1] = کم_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_عام);
	إذا (ofd[1] == INVALID_HANDLE_قدر)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    وإلا إذا (!use_null_for_باہرلا &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !سیٹ_Handle_Information(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	اذهب failed;

    إذا (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = کم_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_عام);
	إذا (efd[1] == INVALID_HANDLE_قدر)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    وإلا إذا (!use_باہرلا_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !سیٹ_Handle_Information(efd[0], HANDLE_FLAG_INHERIT, 0)))
	اذهب failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_داخلہ = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_باہرلا_for_err ? ofd[1] : efd[1];

    إذا (!use_null_for_وچ || !use_null_for_باہرلا || !use_null_for_err)
    {
	إذا (options->jo_سیٹ & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    إذا (channel != عدم)
		++channel->ch_refcount;
	}
	وإلا
	    channel = جوڑو_channel();
	إذا (channel == عدم)
	    اذهب failed;
    }

    إذا (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_منوال |
	    CREATE_NEW_PROCESS_گروہ |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_انکڑے,
	    (حرف *)options->jo_cwd))
    {
	CloseHandle(jo);
	کم->jv_حالت = کم_FAILED;
	اذهب failed;
    }

    ga_clear(&ga);

    إذا (!سونپو_Process_منزل_کم_Object(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = عدم;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    کم->jv_proc_info = pi;
    کم->jv_کم_object = jo;
    کم->jv_حالت = کم_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    إذا (!use_باہرلا_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    کم->jv_channel = channel;
    إذا (channel != عدم)
    {
	channel_سیٹ_pipes(channel,
		      use_file_for_وچ || use_null_for_وچ
					      ? INVALID_FD : (sock_T)ifd[1],
		      use_file_for_باہرلا || use_null_for_باہرلا
					     ? INVALID_FD : (sock_T)ofd[0],
		      use_باہرلا_for_err || use_file_for_err || use_null_for_err
					    ? INVALID_FD : (sock_T)efd[0]);
	channel_سیٹ_کم(channel, کم, options);
    }
    أرجع;

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
mch_کم_حالت(کم_T *کم)
{
    DWORD dw_Exit_کوڈ = 0;

    إذا (!Get_Exit_کوڈ_Process(کم->jv_proc_info.hProcess, &dw_Exit_کوڈ)
	    || dw_Exit_کوڈ != STILL_سرگرم)
    {
	کم->jv_exitval = (صحيح)dw_Exit_کوڈ;
	إذا (کم->jv_حالت < کم_ENDED)
	{
	    ch_log(کم->jv_channel, "Job ended");
	    کم->jv_حالت = کم_ENDED;
	}
	أرجع "dead";
    }
    أرجع "run";
}

    کم_T *
mch_detect_ended_کم(کم_T *کم_list)
{
    HANDLE کم_Handles[ودھ_توں_ودھ_WAIT_OBJECTS];
    کم_T *کم_Array[ودھ_توں_ودھ_WAIT_OBJECTS];
    کم_T *کم = کم_list;

    طالما (کم != عدم)
    {
	DWORD n;
	DWORD نتیجہ;

	لكل (n = 0; n < ودھ_توں_ودھ_WAIT_OBJECTS
				       && کم != عدم; کم = کم->jv_اگلا)
	{
	    إذا (کم->jv_حالت == کم_STARTED)
	    {
		کم_Handles[n] = کم->jv_proc_info.hProcess;
		کم_Array[n] = کم;
		++n;
	    }
	}
	إذا (n == 0)
	    استمر;
	نتیجہ = Wait_For_مضرب_Objects(n, کم_Handles, FALSE, 0);
	إذا (نتیجہ >= WAIT_OBJECT_0 && نتیجہ < WAIT_OBJECT_0 + n)
	{
	    کم_T *wait_کم = کم_Array[نتیجہ - WAIT_OBJECT_0];

	    إذا (STRCMP(mch_کم_حالت(wait_کم), "dead") == 0)
		أرجع wait_کم;
	}
    }
    أرجع عدم;
}

    ساكن BOOL
terminate_all(HANDLE process, صحيح کوڈ)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_قدر;
    DWORD	    pid = GetProcessId(process);

    إذا (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	إذا (h != INVALID_HANDLE_قدر)
	{
	    pe.dw_ناپ = حجم(PROCESSENTRY32);
	    إذا (!Process32_پہلا(h, &pe))
		اذهب theend;

	    افعل
	    {
		إذا (pe.th32_ماپا_Process_ID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    إذا (ph != عدم)
		    {
			terminate_all(ph, کوڈ);
			CloseHandle(ph);
		    }
		}
	    } طالما (Process32_اگلا(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    أرجع TerminateProcess(process, کوڈ);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    صحيح
mch_signal_کم(کم_T *کم, char_u *how)
{
    صحيح ret;

    إذا (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	إذا (کم->jv_کم_object != عدم)
	{
	    إذا (کم->jv_channel != عدم && کم->jv_channel->ch_anonymous_pipe)
		کم->jv_channel->ch_killing = TRUE;
	    أرجع Terminate_کم_Object(کم->jv_کم_object, (UINT)-1) ? OK : FAIL;
	}
	أرجع terminate_all(کم->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    إذا (!AttachConsole(کم->jv_proc_info.dwProcessId))
	أرجع FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		کم->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    أرجع ret;
}

/*
 * Clear the data related to "job".
 */
    فراغ
mch_clear_کم(کم_T *کم)
{
    إذا (کم->jv_حالت == کم_FAILED)
	أرجع;

    إذا (کم->jv_کم_object != عدم)
	CloseHandle(کم->jv_کم_object);
    CloseHandle(کم->jv_proc_info.hProcess);
}
#جے_انت


#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    ساكن فراغ
termcap_منوال_شروع(فراغ)
{
    DWORD cmodein;

    إذا (g_f_Termcap_منوال)
	أرجع;

    Save_Console_بفر(&g_cbNonTermcap);

    إذا (g_cbTermcap.IsValid)
    {
	/*
	 * We've been in termcap mode before.  Restore certain screen
	 * characteristics, including the buffer size and the window
	 * size.  Since we will be redrawing the screen, we don't need
	 * to restore the actual contents of the buffer.
	 */
	Restore_Console_بفر(&g_cbTermcap, FALSE);
	reset_console_color_rgb();
	سیٹ_Console_کھڑکی_Info(g_h_Con_باہرلا, TRUE, &g_cbTermcap.Info.sr_کھڑکی);
	صفاں = g_cbTermcap.Info.dw_ناپ.Y;
	Columns = g_cbTermcap.Info.dw_ناپ.X;
    }
    وإلا
    {
	/*
	 * This is our first time entering termcap mode.  Clear the console
	 * screen buffer, and resize the buffer to match the current window
	 * size.  We will use this as the size of our editing environment.
	 */
	Clear_Console_بفر(g_attrCurrent);
	سیٹ_console_color_rgb();
	Resize_Con_Buf_And_کھڑکی(g_h_Con_باہرلا, Columns, صفاں);
    }

    resettitle();

    Get_Console_منوال(g_h_Con_وچ, &cmodein);
    إذا (g_f_Mouse_سرگرم)
    {
	cmodein |= ENABLE_MOUSE_داخلہ;
	cmodein &= ~ENABLE_تیز_EDIT_منوال;
    }
    وإلا
    {
	cmodein &= ~ENABLE_MOUSE_داخلہ;
	cmodein |= g_cmodein & ENABLE_تیز_EDIT_منوال;
    }
    cmodein |= ENABLE_کھڑکی_داخلہ;
    سیٹ_Console_منوال(g_h_Con_وچ, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_f_Termcap_منوال = TRUE;
}


/*
 * End termcap mode
 */
    ساكن فراغ
termcap_منوال_سرا(فراغ)
{
    DWORD cmodein;
    Console_بفر *cb;
    COORD coord;
    DWORD dwDummy;

    إذا (!g_f_Termcap_منوال)
	أرجع;

    Save_Console_بفر(&g_cbTermcap);

    Get_Console_منوال(g_h_Con_وچ, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_داخلہ | ENABLE_کھڑکی_داخلہ);
    cmodein |= g_cmodein & ENABLE_تیز_EDIT_منوال;
    سیٹ_Console_منوال(g_h_Con_وچ, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# وإلا
    cb = &g_cbNonTermcap;
# endif
    Restore_Console_بفر(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    إذا (exiting && use_alternate_screen_بفر)
	vtp_printf("\033[?1049l");

    إذا (!USE_WT && (p_rs || exiting))
    {
	/*
	 * Clear anything that happens to be on the current line.
	 */
	coord.X = 0;
	coord.Y = (SHORT) (p_rs ? cb->Info.dwCursorPosition.Y : (صفاں - 1));
	FillConsoleOutputCharacter(g_h_Con_باہرلا, ' ',
		cb->Info.dw_ناپ.X, coord, &dwDummy);
	/*
	 * The following is just for aesthetics.  If we are exiting without
	 * restoring the screen, then we want to have a prompt string
	 * appear at the bottom line.  However, the command interpreter
	 * seems to always advance the cursor one line before displaying
	 * the prompt string, which causes the screen to scroll.  To
	 * counter this, move the cursor up one line before exiting.
	 */
	إذا (exiting && !p_rs)
	    coord.Y--;
	/*
	 * Position the cursor at the leftmost column of the desired row.
	 */
	سیٹ_Console_Cursor_Position(g_h_Con_باہرلا, coord);
    }
    سیٹ_Console_Cursor_Info(g_h_Con_باہرلا, &g_cci);
    g_f_Termcap_منوال = FALSE;
}
#جے_انت


#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * clear `n' chars, starting from `coord'
 */
    ساكن فراغ
clear_chars(
    COORD coord,
    DWORD n)
{
    إذا (!vtp_working)
    {
	DWORD dwDummy;

	FillConsoleOutputCharacter(g_h_Con_باہرلا, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_h_Con_باہرلا, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    وإلا
    {
	سیٹ_console_color_rgb();
	gotoxy(coord.X + 1, coord.Y + 1);
	vtp_printf("\033[%dX", n);
    }
}


/*
 * Clear the screen
 */
    ساكن فراغ
clear_screen(فراغ)
{
    g_coord.X = g_coord.Y = 0;

    إذا (!vtp_working)
	clear_chars(g_coord, صفاں * Columns);
    وإلا
    {
	سیٹ_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    ساكن فراغ
clear_منزل_سرا_دا_display(فراغ)
{
    COORD save = g_coord;

    إذا (!vtp_working)
	clear_chars(g_coord, (صفاں - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    وإلا
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
    ساكن فراغ
clear_منزل_سرا_دا_سطر(فراغ)
{
    COORD save = g_coord;

    إذا (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    وإلا
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
    ساكن فراغ
scroll(بنانشانی c_سطریں)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.بایاں_3 + 1, g_srScrollRegion.چوٹی_2 + 1);
    delete_سطریں(c_سطریں);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    ساكن فراغ
سیٹ_scroll_region(
    بنانشانی بایاں,
    بنانشانی چوٹی,
    بنانشانی دایاں,
    بنانشانی bottom)
{
    إذا (بایاں >= دایاں
	    || چوٹی >= bottom
	    || دایاں > (بنانشانی) Columns - 1
	    || bottom > (بنانشانی) صفاں - 1)
	أرجع;

    g_srScrollRegion.بایاں_3 =   بایاں;
    g_srScrollRegion.چوٹی_2 =    چوٹی;
    g_srScrollRegion.دایاں_3 =  دایاں;
    g_srScrollRegion.Bottom = bottom;
}

    ساكن فراغ
سیٹ_scroll_region_tb(
    بنانشانی چوٹی,
    بنانشانی bottom)
{
    إذا (چوٹی >= bottom || bottom > (بنانشانی)صفاں - 1)
	أرجع;

    g_srScrollRegion.چوٹی_2 = چوٹی;
    g_srScrollRegion.Bottom = bottom;
}

    ساكن فراغ
سیٹ_scroll_region_lr(
    بنانشانی بایاں,
    بنانشانی دایاں)
{
    إذا (بایاں >= دایاں || دایاں > (بنانشانی)Columns - 1)
	أرجع;

    g_srScrollRegion.بایاں_3 = بایاں;
    g_srScrollRegion.دایاں_3 = دایاں;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    ساكن فراغ
داخل_کرو_سطریں(بنانشانی c_سطریں)
{
    SMALL_RECT	    سروت, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.بایاں_3 + 1, g_srScrollRegion.چوٹی_2 + 1);

    dest.X = g_srScrollRegion.بایاں_3;
    dest.Y = g_coord.Y + c_سطریں;

    سروت.بایاں_3   = g_srScrollRegion.بایاں_3;
    سروت.چوٹی_2	  = g_coord.Y;
    سروت.دایاں_3  = g_srScrollRegion.دایاں_3;
    سروت.Bottom = g_srScrollRegion.Bottom - c_سطریں;

    clip.بایاں_3   = g_srScrollRegion.بایاں_3;
    clip.چوٹی_2    = g_coord.Y;
    clip.دایاں_3  = g_srScrollRegion.دایاں_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    إذا (!USE_VTP)
	fill.گن_2 = g_attrCurrent;
    وإلا
	fill.گن_2 = g_attrDefault;

    سیٹ_console_color_rgb();

    Scroll_Console_Screen_بفر(g_h_Con_باہرلا, &سروت, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    إذا (سروت.Bottom < dest.Y)
    {
	COORD coord;
	صحيح   i;

	coord.X = سروت.بایاں_3;
	لكل (i = clip.چوٹی_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, سروت.دایاں_3 - سروت.بایاں_3 + 1);
	}
    }

    إذا (vtp_working)
    {
	COORD coord;
	صحيح i;

	coord.X = سروت.بایاں_3;
	لكل (i = سروت.چوٹی_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, سروت.دایاں_3 - سروت.بایاں_3 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    ساكن فراغ
delete_سطریں(بنانشانی c_سطریں)
{
    SMALL_RECT	    سروت, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    صحيح		    nb;

    gotoxy(g_srScrollRegion.بایاں_3 + 1, g_srScrollRegion.چوٹی_2 + 1);

    dest.X = g_srScrollRegion.بایاں_3;
    dest.Y = g_coord.Y;

    سروت.بایاں_3   = g_srScrollRegion.بایاں_3;
    سروت.چوٹی_2	  = g_coord.Y + c_سطریں;
    سروت.دایاں_3  = g_srScrollRegion.دایاں_3;
    سروت.Bottom = g_srScrollRegion.Bottom;

    clip.بایاں_3   = g_srScrollRegion.بایاں_3;
    clip.چوٹی_2    = g_coord.Y;
    clip.دایاں_3  = g_srScrollRegion.دایاں_3;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    إذا (!vtp_working)
	fill.گن_2 = g_attrCurrent;
    وإلا
	fill.گن_2 = g_attrDefault;

    سیٹ_console_color_rgb();

    Scroll_Console_Screen_بفر(g_h_Con_باہرلا, &سروت, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (سروت.Bottom - سروت.چوٹی_2) + 1;

    إذا (nb < سروت.چوٹی_2)
    {
	COORD coord;
	صحيح   i;

	coord.X = سروت.بایاں_3;
	لكل (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, سروت.دایاں_3 - سروت.بایاں_3 + 1);
	}
    }

    إذا (vtp_working)
    {
	COORD coord;
	صحيح i;

	coord.X = سروت.بایاں_3;
	لكل (i = nb; i <= سروت.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, سروت.دایاں_3 - سروت.بایاں_3 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    ساكن فراغ
gotoxy(
    بنانشانی x,
    بنانشانی y)
{
    إذا (x < 1 || x > (بنانشانی)Columns || y < 1 || y > (بنانشانی)صفاں)
	أرجع;

    إذا (!USE_VTP)
    {
	// There are reports of double-width characters not displayed
	// correctly.  This workaround should fix it, similar to how it's done
	// for VTP.
	g_coord.X = 0;
	سیٹ_Console_Cursor_Position(g_h_Con_باہرلا, g_coord);

	// external cursor coords are 1-based; internal are 0-based
	g_coord.X = x - 1;
	g_coord.Y = y - 1;
	سیٹ_Console_Cursor_Position(g_h_Con_باہرلا, g_coord);
    }
    وإلا
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
    ساكن فراغ
textattr(لفظ_2 wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    سیٹ_Console_متن_Attribute(g_h_Con_باہرلا, wAttr);
}


    ساكن فراغ
textcolor(لفظ_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    إذا (!vtp_working)
	سیٹ_Console_متن_Attribute(g_h_Con_باہرلا, g_attrCurrent);
    وإلا
	vtp_sgr_bulk(wAttr);
}


    ساكن فراغ
textbackground(لفظ_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    إذا (!vtp_working)
	سیٹ_Console_متن_Attribute(g_h_Con_باہرلا, g_attrCurrent);
    وإلا
	vtp_sgr_bulk(wAttr);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    ساكن فراغ
normvideo(فراغ)
{
    إذا (!vtp_working)
	textattr(g_attrDefault);
    وإلا
	vtp_sgr_bulk(0);
}


ساكن لفظ_2 g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    ساكن فراغ
standout(فراغ)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((لفظ_2) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    ساكن فراغ
standend(فراغ)
{
    إذا (g_attrPreStandout)
	textattr(g_attrPreStandout);

    g_attrPreStandout = 0;
}


/*
 * Set normal fg/bg color, based on T_ME.  Called when t_me has been set.
 */
    فراغ
mch_سیٹ_عام_colors(فراغ)
{
    char_u	*p;
    صحيح		n;

    cterm_عام_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_عام_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    إذا (
# ifdef FEAT_TERMGUICOLORS
	!p_tgc &&
# endif
	T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	إذا (*p == 'm' && n > 0)
	{
	    cterm_عام_fg_color = (n & 0xf) + 1;
	    cterm_عام_bg_color = ((n >> 4) & 0xf) + 1;
	}
    }
# ifdef FEAT_TERMGUICOLORS
    cterm_عام_fg_gui_color = INVALCOLOR;
    cterm_عام_bg_gui_color = INVALCOLOR;
# endif
}


/*
 * visual bell: flash the screen
 */
    ساكن فراغ
visual_bell(فراغ)
{
    COORD   coordOrigin = {0, 0};
    لفظ_2    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = عدم;

# ifdef FEAT_TERMGUICOLORS
    إذا (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(لفظ_2, صفاں * Columns);
	إذا (oldattrs == عدم)
	    أرجع;
	ReadConsoleOutputAttribute(g_h_Con_باہرلا, oldattrs, صفاں * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_h_Con_باہرلا, attrFlash, صفاں * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    إذا (oldattrs != عدم)
    {
	WriteConsoleOutputAttribute(g_h_Con_باہرلا, oldattrs, صفاں * Columns,
				coordOrigin, &dwDummy);
	vim_free(oldattrs);
    }
}


/*
 * Make the cursor visible or invisible
 */
    ساكن فراغ
cursor_visible(BOOL fVisible)
{
    s_cursor_visible = fVisible;

    إذا (vtp_working)
    {
	// In vtp mode, visibility is controlled solely by DECTCEM.  Skip
	// mch_update_cursor() since shape is independent of visibility and
	// re-emitting DECSCUSR can cause the terminal to briefly redisplay
	// the cursor while a redraw is in progress.
	vtp_printf("\033[?25%c", fVisible ? 'h' : 'l');
	أرجع;
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
    ساكن DWORD
write_chars(
    char_u *pchBuf,
    DWORD  cb_منزل_Write)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    ساكن DWORD    cells;
    ساكن WCHAR    *unicodebuf = عدم;
    ساكن صحيح	    unibuflen = 0;
    ساكن صحيح	    لمائی;
    صحيح		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    ساكن WCHAR    *utf8spbuf = عدم;
    ساكن صحيح	    utf8splength;
    ساكن DWORD    utf8spcells;
    ساكن WCHAR    **utf8usingbuf = &unicodebuf;

    إذا (cb_منزل_Write != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	افعل
	{
	    لمائی = Multi_Byte_منزل_Wide_Char(cp, 0, (LPCSTR)pchBuf, cb_منزل_Write,
							unicodebuf, unibuflen);
	    إذا (لمائی && لمائی <= unibuflen)
		اكسر;
	    vim_free(unicodebuf);
	    unicodebuf = لمائی ? LALLOC_MULT(WCHAR, لمائی) : عدم;
	    unibuflen = unibuflen ? 0 : لمائی;
	} طالما (TRUE);
	cells = mb_string2cells(pchBuf, cb_منزل_Write);
    }
    وإلا // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	إذا (utf8usingbuf != &utf8spbuf)
	{
	    إذا (utf8spbuf == عدم)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		لمائی = Multi_Byte_منزل_Wide_Char(CP_UTF8, 0, " ", 1, عدم, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, لمائی);
		إذا (utf8spbuf != عدم)
		{
		    Multi_Byte_منزل_Wide_Char(CP_UTF8, 0, " ", 1, utf8spbuf, لمائی);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = لمائی;
		    utf8spcells = cells;
		}
	    }
	    وإلا
	    {
		utf8usingbuf = &utf8spbuf;
		لمائی = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    إذا (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_h_Con_باہرلا, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	إذا (WriteConsoleOutputCharacterW(g_h_Con_باہرلا, *utf8usingbuf, لمائی,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    وإلا
    {
	إذا (WriteConsoleW(g_h_Con_باہرلا, *utf8usingbuf, لمائی, &cchwritten,
		    عدم) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    إذا (cchwritten == (DWORD)لمائی)
    {
	written = cb_منزل_Write;
	g_coord.X += (SHORT)cells;
    }
    وإلا
    {
	char_u *p = pchBuf;
	لكل (n = 0; n < cchwritten; n++)
	    MB_CPTR_ADV(p);
	written = p - pchBuf;
	g_coord.X += (SHORT)mb_string2cells(pchBuf, written);
    }

    طالما (g_coord.X > g_srScrollRegion.دایاں_3)
    {
	g_coord.X -= (SHORT) Columns;
	إذا (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    // Cursor under VTP is always in the correct position, no need to reset.
    إذا (!USE_VTP)
	gotoxy(g_coord.X + 1, g_coord.Y + 1);

    أرجع written;
}

    ساكن char_u *
get_seq(
    صحيح *دلیلاں,
    صحيح *گنتی,
    char_u *سر)
{
    صحيح argc;
    char_u *p;

    إذا (سر == عدم || *سر != '\033')
	أرجع عدم;

    argc = 0;
    p = سر;
    ++p;
    افعل
    {
	++p;
	دلیلاں[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } طالما (*p == ';');
    *گنتی = argc;

    أرجع p;
}

    ساكن char_u *
get_sgr(
    صحيح *دلیلاں,
    صحيح *گنتی,
    char_u *سر)
{
    char_u *p = get_seq(دلیلاں, گنتی, سر);

    أرجع (p && *p == 'm') ? ++p : عدم;
}

/*
 * Pointer to next if SGR (^[[n;2;*;*;*m), NULL otherwise.
 */
    ساكن char_u *
sgrn2(
    char_u *سر,
    صحيح	   n)
{
    صحيح argc;
    صحيح دلیلاں[16];
    char_u *p = get_sgr(دلیلاں, &argc, سر);

    أرجع p && argc == 5 && دلیلاں[0] == n && دلیلاں[1] == 2 ? p : عدم;
}

/*
 * Pointer to next if SGR(^[[nm)<space>ESC, NULL otherwise.
 */
    ساكن char_u *
sgrnc(
    char_u *سر,
    صحيح	   n)
{
    صحيح argc;
    صحيح دلیلاں[16];
    char_u *p = get_sgr(دلیلاں, &argc, سر);

    أرجع p && argc == 1 && دلیلاں[0] == n && (p = skipwhite(p)) && *p == '\033'
								    ? p : عدم;
}

    ساكن char_u *
skipblank(char_u *q)
{
    char_u *p = q;

    طالما (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
	++p;
    أرجع p;
}

/*
 * Pointer to the next if any whitespace that may follow SGR is ESC, otherwise
 * NULL.
 */
    ساكن char_u *
sgrn2c(
    char_u *سر,
    صحيح	   n)
{
    char_u *p = sgrn2(سر, n);

    أرجع p && *p != NUL && (p = skipblank(p)) && *p == '\033' ? p : عدم;
}

/*
 * If there is only a newline between the sequence immediately following it,
 * a pointer to the character following the newline is returned.
 * Otherwise NULL.
 */
    ساكن char_u *
sgrn2cn(
    char_u *سر,
    صحيح n)
{
    char_u *p = sgrn2(سر, n);

    أرجع p && p[0] == 0x0a && p[1] == '\033' ? ++p : عدم;
}

ساكن bool وچ_dcs = false;	// inside a DCS (sixel) sequence
ساكن bool dcs_esc = false;	// last DCS byte written was ESC

/*
 * Write "len" bytes at "s" with WriteConsoleW, so that the console's own VT
 * parser processes them.  Needed for DCS sequences when USE_VTP is off.
 */
    ساكن فراغ
write_vt(char_u *s, صحيح len)
{
    صحيح		wlen;
    WCHAR	*ws;
    DWORD	written;

    wlen = Multi_Byte_منزل_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, عدم, 0);
    إذا (wlen <= 0)
	أرجع;
    ws = ALLOC_MULT(WCHAR, wlen);
    إذا (ws == عدم)
	أرجع;
    Multi_Byte_منزل_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_h_Con_باہرلا, ws, wlen, &written, عدم);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    ساكن char_u *
skip_dcs(char_u *s, char_u *سرا)
{
    لكل (char_u *p = s; p < سرا; ++p)
    {
	إذا (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    وچ_dcs = false;
	    أرجع p + 1;
	}
	dcs_esc = *p == ESC;
    }
    أرجع سرا;
}

/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    فراغ
mch_write(
    char_u  *s,
    صحيح	    len)
{
    char_u  *سرا = s + len;

# ifdef VIMDLL
    إذا (gui.وچ_use)
	أرجع;
# endif

    إذا (!رکن_console)
    {
	اكتب(1, s, (بنانشانی)len);
	أرجع;
    }

    // translate ESC | sequences into faked bios calls
    طالما (len--)
    {
	صحيح اگیتر = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	إذا (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    أرجع;
	}

	إذا (وچ_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    صحيح	l = (صحيح)(skip_dcs(s, سرا) - s);

	    إذا (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    استمر;
	}

	طالما (s + ++اگیتر < سرا)
	{
	    ch = s[اگیتر];
	    إذا (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		اكسر;
	}

	إذا (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    إذا (اگیتر != 0)
		اگیتر = 1;
	}

	إذا (اگیتر != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, اگیتر);
# ifdef MCH_WRITE_DUMP
	    إذا (fdDump)
	    {
		fputc('>', fdDump);
		fwrite(s, حجم(char_u), nWritten, fdDump);
		fputs("<\n", fdDump);
	    }
# endif
	    len -= (nWritten - 1);
	    s += nWritten;
	}
	وإلا إذا (s[0] == '\n')
	{
	    // \n, newline: go to the beginning of the next line or scroll
	    إذا (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.بایاں_3 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    وإلا
	    {
		gotoxy(g_srScrollRegion.بایاں_3 + 1, g_coord.Y + 2);
	    }
# ifdef MCH_WRITE_DUMP
	    إذا (fdDump)
		fputs("\\n\n", fdDump);
# endif
	    s++;
	}
	وإلا إذا (s[0] == '\r')
	{
	    // \r, carriage return: go to beginning of line
	    gotoxy(g_srScrollRegion.بایاں_3+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    إذا (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	وإلا إذا (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    إذا (g_coord.X > g_srScrollRegion.بایاں_3)
		g_coord.X--;
	    وإلا إذا (g_coord.Y > g_srScrollRegion.چوٹی_2)
	    {
		g_coord.X = g_srScrollRegion.دایاں_3;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    إذا (fdDump)
		fputs("\\b\n", fdDump);
# endif
	    s++;
	}
	وإلا إذا (s[0] == '\a')
	{
	    // \a, bell
	    سنیہا_Beep(0xFFFFFFFF);
# ifdef MCH_WRITE_DUMP
	    إذا (fdDump)
		fputs("\\a\n", fdDump);
# endif
	    s++;
	}
	وإلا إذا (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
# ifdef MCH_WRITE_DUMP
	    char_u  *old_s = s;
# endif
	    char_u  *p;
	    صحيح	    arg1 = 0, arg2 = 0, argc = 0, دلیلاں[16];
	    char_u  *sp;

	    بدل (s[2])
	    {
	    حالة '0': حالة '1': حالة '2': حالة '3': حالة '4':
	    حالة '5': حالة '6': حالة '7': حالة '8': حالة '9':
		إذا (*(p = get_seq(دلیلاں, &argc, s)) != 'm')
		    اذهب notsgr;

		p = s;

		// Handling frequent optional sequences.  Output to the screen
		// takes too long, so do not output as much as possible.

		// If resetFG,FG,BG,<cr>,BG,FG are connected, the preceding
		// resetFG,FG,BG are omitted.
		إذا (sgrn2(sgrn2(sgrn2cn(sgrn2(sgrnc(p, 39), 38), 48), 48), 38))
		{
		    p = sgrn2(sgrn2(sgrnc(p, 39), 38), 48);
		    len = len + 1 - (صحيح)(p - s);
		    s = p;
		    اكسر;
		}

		// If FG,BG,BG,FG of SGR are connected, the first FG can be
		// omitted.
		إذا (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 48), 38))
		    p = sp;

		// If FG,BG,FG,BG of SGR are connected, the first FG can be
		// omitted.
		إذا (sgrn2(sgrn2(sgrn2c((sp = sgrn2(p, 38)), 48), 38), 48))
		    p = sp;

		// If BG,BG of SGR are connected, the first BG can be omitted.
		إذا (sgrn2((sp = sgrn2(p, 48)), 48))
		    p = sp;

		// If restoreFG and FG are connected, the restoreFG can be
		// omitted.
		إذا (sgrn2((sp = sgrnc(p, 39)), 38))
		    p = sp;

		p = get_seq(دلیلاں, &argc, p);

notsgr:
		arg1 = دلیلاں[0];
		arg2 = دلیلاں[1];
		إذا (*p == 'm')
		{
		    إذا (argc == 1 && دلیلاں[0] == 0)
			normvideo();
		    وإلا إذا (argc == 1)
		    {
			إذا (USE_VTP)
			    textcolor((لفظ_2)arg1);
			وإلا
			    textattr((لفظ_2)arg1);
		    }
		    وإلا إذا (vtp_working)
			vtp_sgr_bulks(argc, دلیلاں);
		}
		وإلا إذا (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		وإلا إذا (argc == 2 && *p == 'r')
		{
		    سیٹ_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		وإلا إذا (argc == 2 && *p == 'R')
		{
		    سیٹ_scroll_region_tb(arg1, arg2);
		}
		وإلا إذا (argc == 2 && *p == 'V')
		{
		    سیٹ_scroll_region_lr(arg1, arg2);
		}
		وإلا إذا (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.چوٹی_2, g_coord.Y - arg1) + 1);
		}
		وإلا إذا (argc == 1 && *p == 'b')
		{
		    textbackground((لفظ_2) arg1);
		}
		وإلا إذا (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.دایاں_3, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		وإلا إذا (argc == 1 && *p == 'f')
		{
		    textcolor((لفظ_2) arg1);
		}
		وإلا إذا (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		وإلا إذا (argc == 1 && *p == 'L')
		{
		    داخل_کرو_سطریں(arg1);
		}
		وإلا إذا (argc == 1 && *p == 'M')
		{
		    delete_سطریں(arg1);
		}

		len -= (صحيح)(p - s);
		s = p + 1;
		اكسر;

	    حالة 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.چوٹی_2, g_coord.Y - 1) + 1);
		اذهب got3;

	    حالة 'B':
		visual_bell();
		اذهب got3;

	    حالة 'C':
		gotoxy(min(g_srScrollRegion.دایاں_3, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		اذهب got3;

	    حالة 'E':
		termcap_منوال_سرا();
		اذهب got3;

	    حالة 'F':
		standout();
		اذهب got3;

	    حالة 'f':
		standend();
		اذهب got3;

	    حالة 'H':
		gotoxy(1, 1);
		اذهب got3;

	    حالة 'j':
		clear_منزل_سرا_دا_display();
		اذهب got3;

	    حالة 'J':
		clear_screen();
		اذهب got3;

	    حالة 'K':
		clear_منزل_سرا_دا_سطر();
		اذهب got3;

	    حالة 'L':
		داخل_کرو_سطریں(1);
		اذهب got3;

	    حالة 'M':
		delete_سطریں(1);
		اذهب got3;

	    حالة 'S':
		termcap_منوال_شروع();
		اذهب got3;

	    حالة 'V':
		cursor_visible(TRUE);
		اذهب got3;

	    حالة 'v':
		cursor_visible(FALSE);
		اذهب got3;

	    got3:
		s += 3;
		len -= 2;
	    }

# ifdef MCH_WRITE_DUMP
	    إذا (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, حجم(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
# endif
	}
	وإلا إذا (s[0] == ESC && len >= 3-1 && s[1] == '[')
	{
	    // CSI sequences should not be written as plain text to the
	    // console.  Parse the sequence and skip over it.  When
	    // USE_VTP is active, pass through known safe ones (e.g.
	    // DECSCUSR for cursor shape) via vtp_printf().
	    صحيح l = 2;

	    // skip parameter and intermediate bytes (0x20-0x3F)
	    طالما (s + l < سرا && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    إذا (s + l < سرا && s[l] >= 0x40 && s[l] <= 0x7E)
	    {
		// DECSCUSR (cursor style): pass through to terminal
		إذا (USE_VTP && s[l] == 'q')
		    vtp_printf("%.*s", l + 1, s);
		l++;
	    }
	    len -= l - 1;
	    s += l;
	}
	وإلا إذا (s[0] == ESC && len >= 2-1 && s[1] == 'P')
	{
	    // DCS (e.g. sixel image): write_chars() would print it as raw text
	    // when USE_VTP is off, so let the console's VT parser handle it.
	    صحيح	l;

	    وچ_dcs = true;
	    dcs_esc = false;
	    l = (صحيح)(skip_dcs(s + 2, سرا) - s);
	    إذا (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	}
	وإلا
	{
	    // Write a single character
	    DWORD nWritten;

	    nWritten = write_chars(s, 1);
# ifdef MCH_WRITE_DUMP
	    إذا (fdDump)
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
    إذا (fdDump)
	fflush(fdDump);
# endif
}

#وإلا // FEAT_GUI_MSWIN && !VIMDLL
    فراغ
mch_write(
    char_u  *s بن_ورتیا,
    صحيح	    len بن_ورتیا)
{
    // never used
}

#جے_انت


/*
 * Delay for "msec" milliseconds.
 */
    فراغ
mch_delay(
    لما    msec,
    صحيح	    flags بن_ورتیا)
{
#إذا defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((صحيح)msec);	    // never wait for input
#وإلا // Console
# ifdef VIMDLL
    إذا (gui.وچ_use)
    {
	Sleep((صحيح)msec);	    // never wait for input
	أرجع;
    }
# endif
    إذا (flags & MCH_DELAY_IGNOREINPUT)
# ifdef FEAT_MZSCHEME
	إذا (mzthreads_allowed() && p_mzq > 0 && msec > p_mzq)
	{
	    صحيح towait = p_mzq;

	    // if msec is large enough, wait by portions in p_mzq
	    طالما (msec > 0)
	    {
		mzvim_check_دھاگے();
		إذا (msec < towait)
		    towait = msec;
		Sleep(towait);
		msec -= towait;
	    }
	}
	وإلا
# endif
	    Sleep((صحيح)msec);
    وإلا
	WaitForChar(msec, FALSE);
#جے_انت
}


/*
 * This version of remove is not scared by a readonly (backup) file.
 * This can also remove a symbolic link like Unix.
 * Return 0 for success, -1 for failure.
 */
    صحيح
mch_remove(char_u *name)
{
    WCHAR	*wn;
    صحيح		n;

    /*
     * On Windows, deleting a directory's symbolic link is done by
     * RemoveDirectory(): mch_rmdir.  It seems unnatural, but it is fact.
     */
    إذا (mch_isdir(name) && mch_is_symbolic_link(name))
	أرجع mch_rmdir(name);

    win32_setattrs(name, FILE_ATTRIBUTE_عام);

    wn = enc_منزل_utf16(name, عدم);
    إذا (wn == عدم)
	أرجع -1;

    n = DeleteFileW(wn) ? 0 : -1;
    vim_free(wn);
    أرجع n;
}


/*
 * Check for an "interrupt signal": CTRL-break or CTRL-C.
 */
    فراغ
mch_breakcheck(صحيح force بن_ورتیا)
{
#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    إذا (!gui.وچ_use)
# endif
	إذا (g_fCtrlCPressed || g_fCBrkPressed)
	{
	    ctrl_break_was_pressed = g_fCBrkPressed;
	    g_fCtrlCPressed = g_fCBrkPressed = FALSE;
	    got_int = TRUE;
	}
#جے_انت
}

// physical RAM to leave for the OS
#تعریف_کرو WINNT_RESERVE_بائٹاں     (256*1024*1024)

/*
 * How much main memory in KiB that can be used by VIM.
 */
    long_u
mch_کل_mem(صحيح special بن_ورتیا)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dw_لمائی = حجم(MEMORYSTATUSEX);
    Global_حافظہ_حالت_Ex(&ms);
    إذا (ms.ullAvailVirtual < ms.ull_کل_Phys)
    {
	// Process address space fits in physical RAM, use all of it.
	أرجع (long_u)(ms.ullAvailVirtual / 1024);
    }
    إذا (ms.ull_کل_Phys <= WINNT_RESERVE_بائٹاں)
    {
	// Catch old NT box or perverse hardware setup.
	أرجع (long_u)((ms.ull_کل_Phys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    أرجع (long_u)((ms.ull_کل_Phys - WINNT_RESERVE_بائٹاں) / 1024);
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    صحيح
mch_expandpath(
    garray_T	*gap,
    char_u	*رستہ,
    صحيح		flags)		// EW_* flags
{
    أرجع dos_expandpath(gap, رستہ, 0, flags, FALSE);
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
    صحيح
mch_wrename(WCHAR *wold, WCHAR *wnew)
{
    WCHAR	*p;
    WCHAR	*q;
    WCHAR	sz_عارضی_File[MAX_رستہ_2 + 1];
    WCHAR	sz_New_رستہ[MAX_رستہ_2 + 1];
    HANDLE	hf;

    // No need to play tricks unless the file name contains a "~" as the
    // seventh character.
    لكل (p = q = wold; *p != L'\0'; ++p)
    {
	بدل (*p)
	{
	حالة L'/':
	حالة L'\\':
	حالة L':':
	    إذا (*(p + 1) != L'\0')
		q = p + 1;	    // point to the character after the path
				    // separator.
	    اكسر;

	افتراضي:
	    اكسر;
	}
    }

    // If the length of the file name is less than 8 characters or the seventh
    // character is not a "~', do a normal move.
    إذا ((صحيح)(p - q) < 8 || q[6] != L'~')
	أرجع (MoveFileW(wold, wnew) == 0);

    // Get base path of new file name.  Undocumented feature: If pszNewFile is
    // a directory, no error is returned and pszFilePart will be NULL.
    إذا (Get_بھریا_رستہ_Name_W(wnew, MAX_رستہ_2, sz_New_رستہ, &p) == 0 || p == عدم)
	أرجع -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    إذا (Get_عارضی_File_Name_W(sz_New_رستہ, L"VIM", 0, sz_عارضی_File) == 0)
	أرجع -2;

    // blow the temp file away
    إذا (!DeleteFileW(sz_عارضی_File))
	أرجع -3;

    // rename old file to the temp file
    إذا (!MoveFileW(wold, sz_عارضی_File))
	أرجع -4;

    // now create an empty file called pszOldFile; this prevents the operating
    // system using pszOldFile as an alias (SFN) if we're renaming within the
    // same directory.  For example, we're editing a file called
    // filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
    // to filena~1.txt~ (i.e., we're making a backup while writing it), the
    // SFN for filena~1.txt~ will be filena~1.txt, by default, which will
    // cause all sorts of problems later in buf_write().  So, we create an
    // empty file called filena~1.txt and the system will have to find some
    // other SFN for filena~1.txt~, such as filena~2.txt
    إذا ((hf = CreateFileW(wold, GENERIC_WRITE, 0, عدم, CREATE_NEW,
		    FILE_ATTRIBUTE_عام, عدم)) == INVALID_HANDLE_قدر)
	أرجع -5;
    إذا (!CloseHandle(hf))
	أرجع -6;

    // rename the temp file to the new file
    إذا (!MoveFileW(sz_عارضی_File, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(فراغ)MoveFileW(sz_عارضی_File, wold);
	أرجع -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(sz_عارضی_File);

    // finally, remove the empty old file
    إذا (!DeleteFileW(wold))
	أرجع -8;

    أرجع 0;
}


/*
 * Converts the filenames to UTF-16, then call mch_wrename().
 * Like rename(), returns 0 upon success, non-zero upon failure.
 */
    صحيح
mch_rename(
    ثابت حرف	*pszOldFile,
    ثابت حرف	*pszNewFile)
{
    WCHAR	*wold = عدم;
    WCHAR	*wnew = عدم;
    صحيح		retval = -1;

    wold = enc_منزل_utf16((char_u *)pszOldFile, عدم);
    wnew = enc_منزل_utf16((char_u *)pszNewFile, عدم);
    إذا (wold != عدم && wnew != عدم)
	retval = mch_wrename(wold, wnew);
    vim_free(wold);
    vim_free(wnew);
    أرجع retval;
}

/*
 * Get the default shell for the current hardware platform
 */
    حرف *
default_shell(فراغ)
{
    أرجع "cmd.exe";
}

/*
 * mch_access() extends access() to do more detailed check on network drives.
 * Returns 0 if file "n" has access rights according to "p", -1 otherwise.
 */
    صحيح
mch_access(حرف *n, صحيح p)
{
    HANDLE	hFile;
    صحيح		retval = -1;	    // default: fail
    WCHAR	*wn;

    wn = enc_منزل_utf16((char_u *)n, عدم);
    إذا (wn == عدم)
	أرجع -1;

    إذا (mch_isdir((char_u *)n))
    {
	WCHAR عارضی_Name_W[MAX_رستہ_2 + 16] = L"";

	إذا (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    صحيح			i;
	    WIN32_لبھو_DATAW    d;

	    لكل (i = 0; i < MAX_رستہ_2 && wn[i] != 0; ++i)
		عارضی_Name_W[i] = wn[i];
	    إذا (عارضی_Name_W[i - 1] != '\\' && عارضی_Name_W[i - 1] != '/')
		عارضی_Name_W[i++] = '\\';
	    عارضی_Name_W[i++] = '*';
	    عارضی_Name_W[i++] = 0;

	    hFile = لبھو_پہلا_File_W(عارضی_Name_W, &d);
	    إذا (hFile == INVALID_HANDLE_قدر)
		اذهب getout;
	    وإلا
		(فراغ)لبھو_Close(hFile);
	}

	إذا (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    إذا (!Get_عارضی_File_Name_W(wn, L"VIM", 0, عارضی_Name_W))
		اذهب getout;
	    وإلا
		DeleteFileW(عارضی_Name_W);
	}
    }
    وإلا
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_منوال = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_منوال = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_منوال, share_منوال,
					     عدم, OPEN_EXISTING, 0, عدم);
	إذا (hFile == INVALID_HANDLE_قدر)
	    اذهب getout;
	CloseHandle(hFile);
    }

    retval = 0;	    // success
getout:
    vim_free(wn);
    أرجع retval;
}

/*
 * Version of open() that may use UTF-16 file name.
 */
    صحيح
mch_open(ثابت حرف *name, صحيح flags, صحيح منوال)
{
    WCHAR	*wn;
    صحيح		f;

    wn = enc_منزل_utf16((char_u *)name, عدم);
    إذا (wn == عدم)
	أرجع -1;

    f = _wopen(wn, flags, منوال);
    vim_free(wn);
    أرجع f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(ثابت حرف *name, ثابت حرف *منوال)
{
    WCHAR	*wn, *wm;
    FILE	*f = عدم;

#إذا defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    حرف new_منوال = منوال[لڑی_لمبائی(منوال) - 1];
    صحيح old_منوال_2 = 0;

    _get_fmode(&old_منوال_2);
    إذا (new_منوال == 't')
	سیٹ_fmode(O_متن);
    وإلا إذا (new_منوال == 'b')
	سیٹ_fmode(O_ثنائی_2);
#جے_انت
    wn = enc_منزل_utf16((char_u *)name, عدم);
    wm = enc_منزل_utf16((char_u *)منوال, عدم);
    إذا (wn != عدم && wm != عدم)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#إذا defined(DEBUG) && _MSC_VER >= 1400
    إذا (old_منوال_2 != 0)
	سیٹ_fmode(old_منوال_2);
#جے_انت
    أرجع f;
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
    ساكن فراغ
copy_substream(HANDLE sh, فراغ *context, WCHAR *منزل, WCHAR *substream, لما len)
{
    HANDLE  h_منزل;
    WCHAR   *منزل_name;

    منزل_name = alloc((wcslen(منزل) + wcslen(substream) + 1) * حجم(WCHAR));
    إذا (منزل_name == عدم)
	أرجع;

    wcscpy(منزل_name, منزل);
    wcscat(منزل_name, substream);

    h_منزل = CreateFileW(منزل_name, GENERIC_WRITE, 0, عدم, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_عام, عدم);
    إذا (h_منزل != INVALID_HANDLE_قدر)
    {
	لما	پورا;
	DWORD	todo;
	DWORD	readcnt, written;
	حرف	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	لكل (پورا = 0; پورا < len; پورا += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - پورا) > حجم(buf) ? حجم(buf)
						       : (size_t)(len - پورا));
	    إذا (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(h_منزل, buf, todo, &written, عدم)
		    || written != todo)
		اكسر;
	}
	CloseHandle(h_منزل);
    }

    حرر(منزل_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    ساكن فراغ
copy_infostreams(char_u *from, char_u *منزل)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_دھارا_ID	sid;
    صحيح			headersize;
    WCHAR		streamname[MAX_رستہ_2];
    DWORD		readcount;
    فراغ		*context = عدم;
    DWORD		lo, hi;
    صحيح			len;

    // Convert the file names to wide characters.
    fromw = enc_منزل_utf16(from, عدم);
    tow = enc_منزل_utf16(منزل, عدم);
    إذا (fromw != عدم && tow != عدم)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, عدم,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, عدم);
	إذا (sh != INVALID_HANDLE_قدر)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    لكل (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		Zero_حافظہ(&sid, حجم(WIN32_دھارا_ID));
		headersize = (صحيح)((حرف *)&sid.c_دھارا_Name - (حرف *)&sid.dw_دھارا_Id);
		إذا (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    اكسر;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		إذا (sid.dw_دھارا_Name_ناپ > 0)
		{
		    // Read the stream name.
		    إذا (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dw_دھارا_Name_ناپ,
					  &readcount, FALSE, FALSE, &context))
			اكسر;

		    // Copy an info stream with a name ":anything:$DATA".
		    // Skip "::$DATA", it has no stream name (examples suggest
		    // it might be used for the normal file contents).
		    // Note that BackupRead() counts bytes, but the name is in
		    // wide characters.
		    len = readcount / حجم(WCHAR);
		    streamname[len] = 0;
		    إذا (len > 7 && wcsicmp(streamname + len - 6,
							      L":$DATA") == 0)
		    {
			streamname[len - 6] = 0;
			copy_substream(sh, &context, tow, streamname,
						    (لما)sid.ناپ_2.u.کم_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(فراغ)BackupSeek(sh, sid.ناپ_2.u.کم_Part, sid.ناپ_2.u.بلند_Part,
							  &lo, &hi, &context);
	    }

	    // Clear the context.
	    (فراغ)BackupRead(sh, عدم, 0, &readcount, TRUE, FALSE, &context);

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
#جے_بنا_تعریف حالت_SUCCESS
# define حالت_SUCCESS	    ((NTSTATUS) 0x00000000L)
#جے_انت

تعريفنوع بنية FILE_بھریا_EA_INFORMATION {
    ULONG  اگلا_اندراج_وتھ;
    UCHAR  Flags;
    UCHAR  Ea_Name_لمائی;
    USHORT Ea_قدر_لمائی;
    CHAR   EaName[1];
} FILE_بھریا_EA_INFORMATION_2, *PFILE_بھریا_EA_INFORMATION;

تعريفنوع بنية _FILE_EA_INFORMATION_ {
    ULONG Ea_ناپ;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#جے_بنا_تعریف PROTO
تعريفنوع NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_نقاب DesiredAccess,
	POBJECT_گن Object_گن,
	PIO_حالت_بلاک Io_حالت_بلاک,
	ULONG ShareAccess,
	ULONG OpenOptions);
تعريفنوع NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
تعريفنوع NTSTATUS (NTAPI *Pfn_Nt_سیٹ_Ea_File)(
	HANDLE		    FileHandle,
	PIO_حالت_بلاک    Io_حالت_بلاک,
	PVOID		    بفر_2,
	ULONG		    لمائی_2);
تعريفنوع NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_حالت_بلاک Io_حالت_بلاک,
	PVOID بفر_2,
	ULONG لمائی_2,
	BOOLEAN Return_Single_اندراج,
	PVOID EaList,
	ULONG Ea_List_لمائی,
	PULONG Ea_اشاریہ,
	BOOLEAN Restart_جانچ);
تعريفنوع NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_حالت_بلاک	Io_حالت_بلاک,
	PVOID			FileInformation,
	ULONG			لمائی_2,
	FILE_INFORMATION_CLASS FileInformationClass);
تعريفنوع VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR سروت_String);

PfnNtOpenFile pNtOpenFile = عدم;
PfnNtClose pNtClose = عدم;
Pfn_Nt_سیٹ_Ea_File p_Nt_سیٹ_Ea_File = عدم;
PfnNtQueryEaFile pNtQueryEaFile = عدم;
PfnNtQueryInformationFile pNtQueryInformationFile = عدم;
PfnRtlInitUnicodeString pRtlInitUnicodeString = عدم;
#جے_انت

/*
 * Load ntdll.dll functions.
 */
    ساكن BOOL
لدو_ntdll(فراغ)
{
    ساكن صحيح	loaded = -1;

    إذا (loaded != -1)
	أرجع (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    إذا (hNtdll != عدم)
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
    إذا (pNtOpenFile == عدم
	    || pNtClose == عدم
	    || p_Nt_سیٹ_Ea_File == عدم
	    || pNtQueryEaFile == عدم
	    || pNtQueryInformationFile == عدم
	    || pRtlInitUnicodeString == عدم)
	loaded = FALSE;
    وإلا
	loaded = TRUE;
    أرجع (BOOL) loaded;
}

/*
 * Copy extended attributes (EA) from file "from" to file "to".
 */
    ساكن فراغ
copy_extattr(char_u *from, char_u *منزل)
{
    char_u		    *fromf = عدم;
    char_u		    *tof = عدم;
    WCHAR		    *fromw = عدم;
    WCHAR		    *tow = عدم;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_گن	    oa;
    IO_حالت_بلاک	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    فراغ		    *ea = عدم;

    إذا (!لدو_ntdll())
	أرجع;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(منزل) + 5);
    إذا (fromf == عدم || tof == عدم)
	اذهب theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), منزل);

    // Convert the names to wide characters.
    fromw = enc_منزل_utf16(fromf, عدم);
    tow = enc_منزل_utf16(tof, عدم);
    إذا (fromw == عدم || tow == عدم)
	اذهب theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    Initialize_Object_گن(&oa, &u, 0, عدم, عدم);
    إذا (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != حالت_SUCCESS)
	اذهب theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, حجم(eainfo),
	    FileEaInformation);
    إذا (eainfo.Ea_ناپ != 0)
    {
	ea = alloc(eainfo.Ea_ناپ);
	إذا (ea != عدم)
	{
	    إذا (pNtQueryEaFile(h, &iosb, ea, eainfo.Ea_ناپ, FALSE,
			عدم, 0, عدم, TRUE) != حالت_SUCCESS)
	    {
		VIM_CLEAR(ea);
	    }
	}
    }
    pNtClose(h);

    // Set the EA.
    إذا (ea != عدم)
    {
	pRtlInitUnicodeString(&u, tow);
	Initialize_Object_گن(&oa, &u, 0, عدم, عدم);
	إذا (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != حالت_SUCCESS)
	    اذهب theend;

	p_Nt_سیٹ_Ea_File(h, &iosb, ea, eainfo.Ea_ناپ);
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
    صحيح
mch_copy_file_attribute(char_u *from, char_u *منزل)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, منزل);
    copy_extattr(from, منزل);
    أرجع 0;
}


/*
 * The command line arguments in UTF-16
 */
ساكن صحيح	n_دلیلاں_W = 0;
ساكن LPWSTR	*ArglistW = عدم;
ساكن صحيح	global_argc = 0;
ساكن حرف	**global_argv;

ساكن صحيح	ورتیا_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
ساكن صحيح	*ورتیا_file_indexes = عدم; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
ساكن صحيح	ورتیا_file_گنتی = 0;	// nr of entries in used_file_indexes
ساكن صحيح	ورتیا_file_literal = FALSE;  // take file names literally
ساكن صحيح	ورتیا_file_بھریا_رستہ = FALSE;  // file name was full path
#جے_تعریف_شدہ FEAT_DIFF
ساكن صحيح	ورتیا_file_diff_منوال = FALSE;  // file name was with diff mode
#جے_انت
ساكن صحيح	ورتیا_alist_گنتی = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    صحيح
get_cmd_دلیلاں_W(حرف ***argvp)
{
    حرف	**argv = عدم;
    صحيح		argc = 0;
    صحيح		i;

    free_cmd_دلیلاں_W();
    ArglistW = Command_سطر_منزل_Argv_W(Get_Command_سطر_W(), &n_دلیلاں_W);
    إذا (ArglistW != عدم)
    {
	argv = alloc((n_دلیلاں_W + 1) * حجم(حرف *));
	إذا (argv != عدم)
	{
	    argc = n_دلیلاں_W;
	    argv[argc] = عدم;
	    لكل (i = 0; i < argc; ++i)
	    {
		صحيح	len;

		// Convert each Unicode argument to UTF-8.
		Wide_Char_منزل_Multi_Byte_alloc(CP_UTF8, 0,
				ArglistW[i], (صحيح)wcslen(ArglistW[i]) + 1,
				(LPSTR *)&argv[i], &len, 0, 0);
		إذا (argv[i] == عدم)
		{
		    // Out of memory, clear everything.
		    طالما (i > 0)
			حرر(argv[--i]);
		    حرر(argv);
		    argv = عدم;
		    argc = 0;
		}
	    }
	}
    }

    global_argc = argc;
    global_argv = argv;
    إذا (argc > 0)
    {
	إذا (ورتیا_file_indexes != عدم)
	    حرر(ورتیا_file_indexes);
	ورتیا_file_indexes = alloc(argc * حجم(صحيح));
    }

    إذا (argvp != عدم)
	*argvp = argv;
    أرجع argc;
}

    فراغ
free_cmd_دلیلاں_W(فراغ)
{
    إذا (ArglistW == عدم)
	أرجع;

    GlobalFree(ArglistW);
    ArglistW = عدم;
}

/*
 * Remember "name" is an argument that was added to the argument list.
 * This avoids that we have to re-parse the argument list when fix_arg_enc()
 * is called.
 */
    فراغ
ورتیا_file_arg(حرف *name, صحيح literal, صحيح بھریا_رستہ, صحيح diff_منوال)
{
    صحيح		i;

    إذا (ورتیا_file_indexes == عدم)
	أرجع;
    لكل (i = ورتیا_file_argc + 1; i < global_argc; ++i)
	إذا (STRCMP(global_argv[i], name) == 0)
	{
	    ورتیا_file_argc = i;
	    ورتیا_file_indexes[ورتیا_file_گنتی++] = i;
	    اكسر;
	}
    ورتیا_file_literal = literal;
    ورتیا_file_بھریا_رستہ = بھریا_رستہ;
#جے_تعریف_شدہ FEAT_DIFF
    ورتیا_file_diff_منوال = diff_منوال;
#جے_انت
}

/*
 * Remember the length of the argument list as it was.  If it changes then we
 * leave it alone when 'encoding' is set.
 */
    فراغ
سیٹ_alist_گنتی(فراغ)
{
    ورتیا_alist_گنتی = GARGCOUNT;
}

/*
 * Fix the encoding of the command line arguments.  Invoked when 'encoding'
 * has been changed while starting up.  Use the UTF-16 command line arguments
 * and convert them to 'encoding'.
 */
    فراغ
fix_arg_enc(فراغ)
{
    صحيح		i;
    صحيح		idx;
    char_u	*str;
    صحيح		*fnum_list;

    // Safety checks:
    // - if argument count differs between the wide and non-wide argument
    //   list, something must be wrong.
    // - the file name arguments must have been located.
    // - the length of the argument list wasn't changed by the user.
    إذا (global_argc != n_دلیلاں_W
	    || ArglistW == عدم
	    || ورتیا_file_indexes == عدم
	    || ورتیا_file_گنتی == 0
	    || ورتیا_alist_گنتی != GARGCOUNT)
	أرجع;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(صحيح, GARGCOUNT);
    إذا (fnum_list == عدم)
	أرجع;		// out of memory
    لكل (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    إذا (ga_grow(&global_alist.al_ga, ورتیا_file_گنتی) == FAIL)
	أرجع;		// out of memory

    لكل (i = 0; i < ورتیا_file_گنتی; ++i)
    {
	idx = ورتیا_file_indexes[i];
	str = utf16_منزل_enc(ArglistW[idx], عدم);
	إذا (str != عدم)
	{
	    صحيح literal = ورتیا_file_literal;

#جے_تعریف_شدہ FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    إذا (ورتیا_file_diff_منوال && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *پوچھ;
		string_T    ret;

		پوچھ = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), پوچھ, STRLEN(پوچھ), TRUE, &ret);
		إذا (ret.string != عدم)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#جے_انت
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    إذا (ورتیا_file_literal)
		buf_سیٹ_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    إذا (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		إذا (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_جوڑو(&global_alist, str, literal ? 2 : 0);
	}
    }

    إذا (!ورتیا_file_literal)
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
	alist_expand(fnum_list, ورتیا_alist_گنتی);
	do_cmdline_cmd(
		(char_u *)":legacy let &isf = g:SaVe_ISF|unlet g:SaVe_ISF");
	do_cmdline_cmd(
		(char_u *)":legacy let &wig = g:SaVe_WIG|unlet g:SaVe_WIG");
    }

    // If wildcard expansion failed, we are editing the first file of the
    // arglist and there is no file name: Edit the first argument now.
    إذا (curwin->w_arg_idx == 0 && curbuf->b_fname == عدم)
    {
	do_cmdline_cmd((char_u *)":rewind");
	إذا (GARGCOUNT == 1 && ورتیا_file_بھریا_رستہ
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    آخری_chdir_reason = "drop";
    }

    سیٹ_alist_گنتی();
}

    صحيح
mch_setenv(حرف *var, حرف *قدر, صحيح x بن_ورتیا)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(قدر) + 2);
    إذا (envbuf == عدم)
	أرجع -1;

    sprintf((حرف *)envbuf, "%s=%s", var, قدر);

    p = enc_منزل_utf16(envbuf, عدم);

    vim_free(envbuf);
    إذا (p == عدم)
	أرجع -1;
    _wputenv(p);
#جے_تعریف_شدہ libintl_wputenv
    libintl_wputenv(p);
#جے_انت
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    أرجع 0;
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

    ساكن فراغ
vtp_flag_init(فراغ)
{
#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    DWORD   منوال;
    HANDLE  باہرلا;

# ifdef VIMDLL
    إذا (!gui.وچ_use)
# endif
    {
	باہرلا = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_نسخہ >= VTP_پہلا_SUPPORT_BUILD) ? 1 : 0;
	Get_Console_منوال(باہرلا, &منوال);
	منوال |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	إذا (سیٹ_Console_منوال(باہرلا, منوال) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_بفر = win_نسخہ >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#جے_انت

    إذا (win_نسخہ >= CONPTY_پہلا_SUPPORT_BUILD)
	conpty_working = 1;
    إذا (win_نسخہ >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    إذا (win_نسخہ <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    إذا (win_نسخہ <= CONPTY_1909_BUILD)
	conpty_type = 2;
    إذا (win_نسخہ <= CONPTY_1903_BUILD)
	conpty_type = 2;
    إذا (win_نسخہ < CONPTY_پہلا_SUPPORT_BUILD)
	conpty_type = 1;

    إذا (win_نسخہ >= CONPTY_اگلا_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ساكن فراغ
vtp_init(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_بفر_INFOEX csbi;
    csbi.cb_ناپ = حجم(csbi);
    Get_Console_Screen_بفر_Info_Ex(g_h_Con_باہرلا, &csbi);
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

    ساكن فراغ
vtp_exit(فراغ)
{
    restore_console_color_rgb();
}

    صحيح
vtp_printf(
    حرف *format,
    ...)
{
    char_u  buf[100];
    va_list list;
    DWORD   نتیجہ;
    صحيح	    len;

    إذا (silent_منوال)
	أرجع 0;

    va_start(list, format);
    len = vim_vsnprintf((حرف *)buf, 100, (حرف *)format, list);
    va_end(list);
    WriteConsoleA(g_h_Con_باہرلا, buf, (DWORD)len, &نتیجہ, عدم);
    أرجع (صحيح)نتیجہ;
}

    ساكن فراغ
vtp_sgr_bulk(
    صحيح arg)
{
    صحيح دلیلاں[1];

    دلیلاں[0] = arg;
    vtp_sgr_bulks(1, دلیلاں);
}

# define FAST256(x) \
    إذا ((*p-- = "0123456789"[(n = x % 10)]) \
	    && x >= 10 && (*p-- = "0123456789"[((m = x % 100) - n) / 10]) \
	    && x >= 100 && (*p-- = "012"[((x & 0xff) - m) / 100]));

# define FAST256CASE(x) \
    حالة x: \
	FAST256(newargs[x - 1]);

    ساكن فراغ
vtp_sgr_bulks(
    صحيح argc,
    صحيح *دلیلاں)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    صحيح	    وچ, باہرلا;
    صحيح	    newargs[16];
    ساكن صحيح sgrfgr = -1, sgrfgg, sgrfgb;
    ساكن صحيح sgrbgr = -1, sgrbgg, sgrbgb;

    إذا (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	أرجع;
    }

    وچ = باہرلا = 0;
    طالما (وچ < argc)
    {
	صحيح s = دلیلاں[وچ];
	صحيح copylen = 1;

	إذا (s == 38)
	{
	    إذا (argc - وچ >= 5 && دلیلاں[وچ + 1] == 2)
	    {
		إذا (sgrfgr == دلیلاں[وچ + 2] && sgrfgg == دلیلاں[وچ + 3]
						     && sgrfgb == دلیلاں[وچ + 4])
		{
		    وچ += 5;
		    copylen = 0;
		}
		وإلا
		{
		    sgrfgr = دلیلاں[وچ + 2];
		    sgrfgg = دلیلاں[وچ + 3];
		    sgrfgb = دلیلاں[وچ + 4];
		    copylen = 5;
		}
	    }
	    وإلا إذا (argc - وچ >= 3 && دلیلاں[وچ + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	وإلا إذا (s == 48)
	{
	    إذا (argc - وچ >= 5 && دلیلاں[وچ + 1] == 2)
	    {
		إذا (sgrbgr == دلیلاں[وچ + 2] && sgrbgg == دلیلاں[وچ + 3]
						     && sgrbgb == دلیلاں[وچ + 4])
		{
		    وچ += 5;
		    copylen = 0;
		}
		وإلا
		{
		    sgrbgr = دلیلاں[وچ + 2];
		    sgrbgg = دلیلاں[وچ + 3];
		    sgrbgb = دلیلاں[وچ + 4];
		    copylen = 5;
		}
	    }
	    وإلا إذا (argc - وچ >= 3 && دلیلاں[وچ + 1] == 5)
	    {
		sgrbgr = -1;
		copylen = 3;
	    }
	}
	وإلا إذا (30 <= s && s <= 39)
	    sgrfgr = -1;
	وإلا إذا (90 <= s && s <= 97)
	    sgrfgr = -1;
	وإلا إذا (40 <= s && s <= 49)
	    sgrbgr = -1;
	وإلا إذا (100 <= s && s <= 107)
	    sgrbgr = -1;
	وإلا إذا (s == 0)
	    sgrfgr = sgrbgr = -1;

	طالما (copylen--)
	    newargs[باہرلا++] = دلیلاں[وچ++];
    }

    p = &buf[حجم(buf) - 1];
    *p-- = 'm';

    بدل (باہرلا)
    {
	صحيح	n, m;
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
	WriteConsoleA(g_h_Con_باہرلا, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, عدم);
    افتراضي:
	اكسر;
    }
}

    ساكن فراغ
wt_init(فراغ)
{
    wt_working = mch_getenv("WT_SESSION") != عدم;
}

# ifdef FEAT_TERMGUICOLORS
    ساكن صحيح
ctermtoxterm(
    صحيح cterm)
{
    char_u r, g, b, idx;

    cterm_color2rgb(cterm, &r, &g, &b, &idx);
    أرجع (((صحيح)r << 16) | ((صحيح)g << 8) | (صحيح)b);
}
# endif

    ساكن فراغ
سیٹ_console_color_rgb(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_بفر_INFOEX csbi;
    guicolor_T	fg, bg;
    صحيح		ctermfg, ctermbg;

    إذا (!vtp_working)
	أرجع;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    إذا (p_tgc || t_colors >= 256)
    {
	رکن_fg_rgb_color(fg);
	رکن_bg_rgb_color(bg);
	أرجع;
    }

    إذا (use_alternate_screen_بفر)
	أرجع;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cb_ناپ = حجم(csbi);
    Get_Console_Screen_بفر_Info_Ex(g_h_Con_باہرلا, &csbi);

    csbi.cb_ناپ = حجم(csbi);
    csbi.sr_کھڑکی.دایاں_3 += 1;
    csbi.sr_کھڑکی.Bottom += 1;
    store_console_bg_rgb = csbi.Color_جدول[g_color_اشاریہ_bg];
    store_console_fg_rgb = csbi.Color_جدول[g_color_اشاریہ_fg];
    csbi.Color_جدول[g_color_اشاریہ_bg] = (COLORREF)bg;
    csbi.Color_جدول[g_color_اشاریہ_fg] = (COLORREF)fg;
    سیٹ_Console_Screen_بفر_Info_Ex(g_h_Con_باہرلا, &csbi);
# endif
}

# إذا defined(FEAT_TERMGUICOLORS)
    فراغ
get_default_console_color(
    صحيح *cterm_fg,
    صحيح *cterm_bg,
    guicolor_T *gui_fg,
    guicolor_T *gui_bg)
{
    صحيح id;
    guicolor_T guifg = INVALCOLOR;
    guicolor_T guibg = INVALCOLOR;
    صحيح ctermfg = 0;
    صحيح ctermbg = 0;
    صحيح dummynull = 0;

    id = syn_name2id((char_u *)"Normal");
    إذا (id > 0 && p_tgc)
	syn_id2colors(id, &guifg, &guibg);
    إذا (guifg == INVALCOLOR)
    {
	ctermfg = -1;
	إذا (id > 0)
	    syn_id2cterm_bg(id, &ctermfg, &dummynull);
	إذا (ctermfg != -1)
	    guifg = ctermtoxterm(ctermfg);
	وإلا
	    guifg = USE_WT ? INVALCOLOR : default_console_color_fg;
	cterm_عام_fg_gui_color = guifg;
	ctermfg = ctermfg < 0 ? 0 : ctermfg;
    }
    إذا (guibg == INVALCOLOR)
    {
	ctermbg = -1;
	إذا (id > 0)
	    syn_id2cterm_bg(id, &dummynull, &ctermbg);
	إذا (ctermbg != -1)
	    guibg = ctermtoxterm(ctermbg);
	وإلا
	    guibg = USE_WT ? INVALCOLOR : default_console_color_bg;
	cterm_عام_bg_gui_color = guibg;
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
    ساكن فراغ
reset_console_color_rgb(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    إذا (use_alternate_screen_بفر)
	أرجع;

    CONSOLE_SCREEN_بفر_INFOEX csbi;

    csbi.cb_ناپ = حجم(csbi);
    Get_Console_Screen_بفر_Info_Ex(g_h_Con_باہرلا, &csbi);

    csbi.cb_ناپ = حجم(csbi);
    csbi.sr_کھڑکی.دایاں_3 += 1;
    csbi.sr_کھڑکی.Bottom += 1;
    csbi.Color_جدول[g_color_اشاریہ_bg] = (COLORREF)store_console_bg_rgb;
    csbi.Color_جدول[g_color_اشاریہ_fg] = (COLORREF)store_console_fg_rgb;
    سیٹ_Console_Screen_بفر_Info_Ex(g_h_Con_باہرلا, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    ساكن فراغ
restore_console_color_rgb(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    إذا (use_alternate_screen_بفر)
	أرجع;

    CONSOLE_SCREEN_بفر_INFOEX csbi;

    csbi.cb_ناپ = حجم(csbi);
    Get_Console_Screen_بفر_Info_Ex(g_h_Con_باہرلا, &csbi);

    csbi.cb_ناپ = حجم(csbi);
    csbi.sr_کھڑکی.دایاں_3 += 1;
    csbi.sr_کھڑکی.Bottom += 1;
    csbi.Color_جدول[g_color_اشاریہ_bg] = (COLORREF)save_console_bg_rgb;
    csbi.Color_جدول[g_color_اشاریہ_fg] = (COLORREF)save_console_fg_rgb;
    سیٹ_Console_Screen_بفر_Info_Ex(g_h_Con_باہرلا, &csbi);
# endif
}

    فراغ
control_console_color_rgb(فراغ)
{
    إذا (vtp_working)
	سیٹ_console_color_rgb();
    وإلا
	reset_console_color_rgb();
}

    صحيح
use_vtp(فراغ)
{
    أرجع USE_VTP;
}

    صحيح
is_رکن_win32(فراغ)
{
    أرجع T_NAME != عدم && STRCMP(T_NAME, "win32") == 0;
}

    صحيح
has_vtp_working(فراغ)
{
    أرجع vtp_working;
}

#جے_انت

    صحيح
has_conpty_working(فراغ)
{
    أرجع conpty_working;
}

    صحيح
get_conpty_type(فراغ)
{
    أرجع conpty_type;
}

    صحيح
is_conpty_stable(فراغ)
{
    أرجع conpty_stable;
}

    صحيح
get_conpty_fix_type(فراغ)
{
    أرجع conpty_fix_type;
}

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    فراغ
resize_console_buf(فراغ)
{
    إذا (use_alternate_screen_بفر)
	أرجع;

    CONSOLE_SCREEN_بفر_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    إذا (!Get_Console_Screen_بفر_Info(g_h_Con_باہرلا, &csbi))
	أرجع;

    coord.X = SRWIDTH(csbi.sr_کھڑکی);
    coord.Y = SRHEIGHT(csbi.sr_کھڑکی);
    سیٹ_Console_Screen_بفر_ناپ(g_h_Con_باہرلا, coord);

    newsize.بایاں_3 = 0;
    newsize.چوٹی_2 = 0;
    newsize.دایاں_3 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    سیٹ_Console_کھڑکی_Info(g_h_Con_باہرلا, TRUE, &newsize);

    سیٹ_Console_Screen_بفر_ناپ(g_h_Con_باہرلا, coord);
}
#جے_انت

    حرف *
GetWin32Error(فراغ)
{
    ساكن حرف	*oldmsg = عدم;
    حرف	*acp_پیغام = عدم;
    DWORD	acp_len;
    حرف	*enc_پیغام = عدم;
    صحيح		enc_len = 0;

    // get formatted message from OS
    acp_len = Format_سنیہا(
	    FORMAT_سنیہا_ALLOCATE_بفر|FORMAT_سنیہا_FROM_SYSTEM,
	    عدم, Get_آخری_Error(), 0, (LPSTR)&acp_پیغام, 0, عدم);
    إذا (acp_len == 0 || acp_پیغام == عدم)
	أرجع عدم;

    // clean oldmsg if remained.
    إذا (oldmsg != عدم)
    {
	vim_free(oldmsg);
	oldmsg = عدم;
    }

    acp_منزل_enc((char_u *)acp_پیغام, (صحيح)acp_len, (char_u **)&enc_پیغام, &enc_len);
    LocalFree(acp_پیغام);
    إذا (enc_پیغام == عدم)
	أرجع عدم;

    // remove trailing \r\n
    حرف *pcrlf = لڑی_کھوج(enc_پیغام, "\r\n");
    إذا (pcrlf != عدم)
	*pcrlf = NUL;

    oldmsg = enc_پیغام;
    أرجع enc_پیغام;
}

#إذا defined(FEAT_RELTIME)
ساكن HANDLE   timer_handle;
ساكن صحيح      timer_سرگرم = FALSE;

/*
 * Calls to start_timeout alternate the return value pointer between the two
 * entries in timeout_flags. If the previously active timeout is very close to
 * expiring when start_timeout() is called then a race condition means that the
 * set_flag() function may still be invoked after the previous timer is
 * deleted. Ping-ponging between the two flags prevents this causing 'fake'
 * timeouts.
 */
ساكن sig_atomic_t timeout_flags[2];
ساكن صحيح	    timeout_flag_idx = 0;
ساكن sig_atomic_t *timeout_flag = &timeout_flags[0];


    ساكن فراغ CALLBACK
سیٹ_flag(فراغ *param, BOOLEAN unused2 بن_ورتیا)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    فراغ
stop_timeout(فراغ)
{
    إذا (timer_سرگرم)
    {
	BOOL ret = Delete_Timer_قطار_Timer(عدم, timer_handle, عدم);
	timer_سرگرم = FALSE;
	إذا (!ret && Get_آخری_Error() != ERROR_IO_PENDING)
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
    متطاير sig_atomic_t *
شروع_timeout(لما msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = Create_Timer_قطار_Timer(
	    &timer_handle, عدم, سیٹ_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    إذا (!ret)
    {
	semsg(_(e_could_not_سیٹ_timeout_str), GetWin32Error());
    }
    وإلا
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_سرگرم = TRUE;
	*timeout_flag = FALSE;
    }
    أرجع timeout_flag;
}
#جے_انت
