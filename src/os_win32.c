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

#اگر_تعریف_شده FEAT_MZSCHEME
# include "if_mzsch.h"
#پایان_اگر

#ضمن <sys/types.h>
#ضمن <signal.h>
#ضمن <limits.h>

#ضمن <process.h>
#ضمن <winternl.h>
#ضمن <direct.h>

#إذا !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#پایان_اگر

#اگر_تعریف_شده FEAT_کار_CHANNEL
# include <tlhelp32.h>
#پایان_اگر

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#اگر_تعریف_شده MCH_WRITE_DUMP
FILE* fdDump = عدم;
#پایان_اگر

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
// Win32 Console handles for input and output
ساكن HANDLE g_h_Con_درون  = INVALID_HANDLE_مقدار;
ساكن HANDLE g_h_Con_بیرون = INVALID_HANDLE_مقدار;

// Win32 Screen buffer,coordinate,console I/O information
ساكن SMALL_RECT g_srScrollRegion;
// This is explicitly initialised to work around a LTCG issue on Windows ARM64
// (at least of 19.39.33321).  This pushes this into the `.data` rather than
// `.bss` which corrects code generation in `write_chars` (#13453).
ساكن COORD	  g_coord = {0, 0};  // 0-based, but external coords are 1-based

// The attribute of the screen when the editor was started
ساكن کلمه_2  g_attrDefault = 7;  // lightgray text on black background
ساكن کلمه_2  g_attrCurrent;

ساكن صحيح g_fCBrkPressed = FALSE;  // set by ctrl-break interrupt
ساكن صحيح g_fCtrlCPressed = FALSE; // set when ctrl-C or ctrl-break detected
ساكن صحيح g_fForceExit = FALSE;    // set when forcefully exiting

ساكن فراغ scroll(بیعلامت c_سطرها);
ساكن فراغ مجموعه_scroll_region(بیعلامت چپ_2, بیعلامت بالاترین,
			      بیعلامت راست_2, بیعلامت bottom);
ساكن فراغ مجموعه_scroll_region_tb(بیعلامت بالاترین, بیعلامت bottom);
ساكن فراغ مجموعه_scroll_region_lr(بیعلامت چپ_2, بیعلامت راست_2);
ساكن فراغ درج_سطرها(بیعلامت c_سطرها);
ساكن فراغ delete_سطرها(بیعلامت c_سطرها);
ساكن فراغ gotoxy(بیعلامت x, بیعلامت y);
ساكن فراغ standout(فراغ);
ساكن صحيح s_cursor_visible = TRUE;
ساكن صحيح did_create_conin = FALSE;
// The 'input_record_buffer' is an internal dynamic fifo queue of MS-Windows
// console INPUT_RECORD events that are normally read from the console input
// buffer.  This provides an injection point for testing the low-level handling
// of INPUT_RECORDs.
تعريفنوع بنية درون_داد_رکورد_میانگیر_گره_S
{
    درون_داد_رکورد ir;
    بنية درون_داد_رکورد_میانگیر_گره_S *بعدی;
} درون_داد_رکورد_میانگیر_گره_T;
تعريفنوع بنية درون_داد_رکورد_میانگیر_S
{
    درون_داد_رکورد_میانگیر_گره_T *سر;
    درون_داد_رکورد_میانگیر_گره_T *دم;
    صحيح درازی;
} درون_داد_رکورد_میانگیر_T;
ساكن درون_داد_رکورد_میانگیر_T درون_داد_رکورد_میانگیر;
ساكن صحيح read_درون_داد_رکورد_میانگیر(درون_داد_رکورد* irEvents, صحيح n_Max_درازی);
ساكن صحيح write_درون_داد_رکورد_میانگیر(درون_داد_رکورد* irEvents, صحيح n_درازی);
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_MSWIN
ساكن صحيح s_dont_use_vimrun = TRUE;
ساكن صحيح need_vimrun_warning = FALSE;
ساكن string_T vimrun_مسیر = {(char_u *)"vimrun ", 7};
ساكن صحيح vimrun_مسیر_allocated = FALSE;
#پایان_اگر

ساكن صحيح win32_getattrs(char_u *name);
ساكن صحيح win32_setattrs(char_u *name, صحيح attrs);
ساكن صحيح win32_مجموعه_archive(char_u *name);

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

ساكن صحيح g_color_شاخص_bg = 0;
ساكن صحيح g_color_شاخص_fg = 7;

# ifdef FEAT_TERMGUICOLORS
ساكن guicolor_T save_console_bg_rgb;
ساكن guicolor_T save_console_fg_rgb;
ساكن guicolor_T store_console_bg_rgb;
ساكن guicolor_T store_console_fg_rgb;
ساكن صحيح default_console_color_bg = 0x000000; // black
ساكن صحيح default_console_color_fg = 0xc0c0c0; // white
#  define USE_VTP		(vtp_working && is_جمله_win32() \
						 && (p_tgc || t_colors >= 256))
#  define USE_WT		(wt_working)
# وإلا
#  define USE_VTP		0
#  define USE_WT		0
# endif

ساكن فراغ مجموعه_console_color_rgb(فراغ);
ساكن فراغ reset_console_color_rgb(فراغ);
ساكن فراغ restore_console_color_rgb(فراغ);
#پایان_اگر  // !FEAT_GUI_MSWIN || VIMDLL

// This flag is newly created from Windows 10
#اگر_تعریف_نشده ENABLE_VIRTUAL_TERMINAL_PROCESSING
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#پایان_اگر

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساكن صحيح suppress_winsize = 1;	// don't fiddle with console
#پایان_اگر

ساكن WCHAR *exe_pathw = عدم;

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساكن BOOL use_alternate_screen_میانگیر = FALSE;
#پایان_اگر

خارجي DWORD win_نسخه; // this is in os_mswin.c

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ساكن BOOL
is_ambiwidth_event(
    درون_داد_رکورد *ir)
{
    أرجع ir->EventType == کلید_EVENT
		&& ir->Event.کلید_Event_2.b_کلید_پایین
		&& ir->Event.کلید_Event_2.w_Repeat_شمار == 1
		&& ir->Event.کلید_Event_2.w_Virtual_کلید_کد == 0x12
		&& ir->Event.کلید_Event_2.w_Virtual_پیمایش_کد == 0x38
		&& ir->Event.کلید_Event_2.uChar.UnicodeChar == 0
		&& ir->Event.کلید_Event_2.dw_Control_کلید_State == 2;
}

    ساكن فراغ
make_ambiwidth_event(
    درون_داد_رکورد *پایین,
    درون_داد_رکورد *به_بالا)
{
    پایین->Event.کلید_Event_2.w_Virtual_کلید_کد = 0;
    پایین->Event.کلید_Event_2.w_Virtual_پیمایش_کد = 0;
    پایین->Event.کلید_Event_2.uChar.UnicodeChar
				    = به_بالا->Event.کلید_Event_2.uChar.UnicodeChar;
    پایین->Event.کلید_Event_2.dw_Control_کلید_State = 0;
}

/*
 * Version of ReadConsoleInput() that works with IME.
 * Works around problems on Windows 8.
 */
    ساكن BOOL
read_console_درون_داد(
    HANDLE	    h_درون_داد,
    درون_داد_رکورد    *lp_میانگیر,
    صحيح		    n_درازی,
    LPDWORD	    lpEvents)
{
    تعداد
    {
	IRSIZE = 10
    };
    ساكن درون_داد_رکورد s_irCache[IRSIZE];
    ساكن DWORD s_dw_شاخص = 0;
    ساكن DWORD s_dwMax = 0;
    DWORD dwEvents;
    صحيح سر;
    صحيح دم;
    صحيح i;
    ساكن درون_داد_رکورد s_irPseudo;

    إذا (s_dwMax == 0 && درون_داد_رکورد_میانگیر.درازی > 0)
    {
	dwEvents = read_درون_داد_رکورد_میانگیر(s_irCache, IRSIZE);
	s_dw_شاخص = 0;
	s_dwMax = dwEvents;
    }

    إذا (n_درازی == -2)
	أرجع (s_dwMax > 0) ? TRUE : FALSE;

    إذا (win_نسخه < MAKE_VER(6, 2, 0)) // Before Windows 8
    {
	إذا (n_درازی == -1)
	    أرجع Peek_Console_درون_داد_W(h_درون_داد, lp_میانگیر, 1, lpEvents);
	أرجع Read_Console_درون_داد_W(h_درون_داد, lp_میانگیر, 1, &dwEvents);
    }

    إذا (s_dwMax == 0)
    {
	إذا (!vtp_working && n_درازی == -1)
	    أرجع Peek_Console_درون_داد_W(h_درون_داد, lp_میانگیر, 1, lpEvents);
	Get_عدد_از_Console_درون_داد_Events(h_درون_داد, &dwEvents);
	إذا (dwEvents == 0 && n_درازی == -1)
	    أرجع Peek_Console_درون_داد_W(h_درون_داد, lp_میانگیر, 1, lpEvents);
	Read_Console_درون_داد_W(h_درون_داد, s_irCache, IRSIZE, &dwEvents);
	s_dw_شاخص = 0;
	s_dwMax = dwEvents;
	إذا (dwEvents == 0)
	{
	    *lpEvents = 0;
	    أرجع TRUE;
	}

	لكل (i = s_dw_شاخص; i < (صحيح)s_dwMax - 1; ++i)
	    إذا (is_ambiwidth_event(&s_irCache[i]))
		make_ambiwidth_event(&s_irCache[i], &s_irCache[i + 1]);

	إذا (s_dwMax > 1)
	{
	    سر = 0;
	    دم = s_dwMax - 1;
	    طالما (سر != دم)
	    {
		إذا (s_irCache[سر].EventType == پنجره_میانگیر_اندازه_EVENT
			&& s_irCache[سر + 1].EventType
						  == پنجره_میانگیر_اندازه_EVENT)
		{
		    // Remove duplicate event to avoid flicker.
		    لكل (i = سر; i < دم; ++i)
			s_irCache[i] = s_irCache[i + 1];
		    --دم;
		    استمر;
		}
		سر++;
	    }
	    s_dwMax = دم + 1;
	}
    }

    إذا (s_irCache[s_dw_شاخص].EventType == کلید_EVENT)
    {
	إذا (s_irCache[s_dw_شاخص].Event.کلید_Event_2.w_Repeat_شمار > 1)
	{
	    s_irPseudo = s_irCache[s_dw_شاخص];
	    s_irPseudo.Event.کلید_Event_2.w_Repeat_شمار = 1;
	    s_irCache[s_dw_شاخص].Event.کلید_Event_2.w_Repeat_شمار--;
	    *lp_میانگیر = s_irPseudo;
	    *lpEvents = 1;
	    أرجع TRUE;
	}
    }

    *lp_میانگیر = s_irCache[s_dw_شاخص];
    إذا (!(n_درازی == -1 || n_درازی == -2) && ++s_dw_شاخص >= s_dwMax)
	s_dwMax = 0;
    *lpEvents = 1;
    أرجع TRUE;
}

/*
 * Version of PeekConsoleInput() that works with IME.
 */
    ساكن BOOL
peek_console_درون_داد(
    HANDLE	    h_درون_داد,
    درون_داد_رکورد    *lp_میانگیر,
    DWORD	    n_درازی استفادهنشده,
    LPDWORD	    lpEvents)
{
    أرجع read_console_درون_داد(h_درون_داد, lp_میانگیر, -1, lpEvents);
}

# ifdef FEAT_CLIENTSERVER
    ساكن DWORD
پیام_wait_for_مضرب_objects(
    DWORD    n_شمار,
    LPHANDLE pHandles,
    BOOL     fWaitAll,
    DWORD    dwMilliseconds,
    DWORD    dw_Wake_ماسک)
{
    إذا (read_console_درون_داد(عدم, عدم, -2, عدم))
	أرجع WAIT_OBJECT_0;
    أرجع پیام_Wait_For_مضرب_Objects(n_شمار, pHandles, fWaitAll,
				     dwMilliseconds, dw_Wake_ماسک);
}
# endif

# ifndef FEAT_CLIENTSERVER
    ساكن DWORD
wait_for_single_object(
    HANDLE hHandle,
    DWORD dwMilliseconds)
{
    إذا (read_console_درون_داد(عدم, عدم, -2, عدم))
	أرجع WAIT_OBJECT_0;
    أرجع WaitForSingleObject(hHandle, dwMilliseconds);
}
# endif
#پایان_اگر   // !FEAT_GUI_MSWIN || VIMDLL

    فراغ
mch_get_exe_name(فراغ)
{
    صحيح		updated = FALSE;
    ساكن صحيح	enc_prev = -1;
    WCHAR	*مسیر;
    size_t	pathlen;

    إذا (exe_name == عدم || exe_pathw == عدم || enc_prev != enc_codepage)
    {
	WCHAR	buf[MAX_مسیر];
	WCHAR	*p;

	// store the name of the executable, may be used for $VIM
	p = buf + GetModuleFileNameW(عدم, buf, MAX_مسیر);
	إذا (p > buf)
	{
	    إذا (enc_codepage == -1)
		enc_codepage = GetACP();
	    vim_free(exe_name);
	    exe_name = utf16_مقصد_enc(buf, عدم);
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
    مسیر = _wgetenv(L"PATH");
    pathlen = (مسیر == عدم) ? 0 : wcslen(مسیر);

    // Maximum length of $PATH is more than MAXPATHL. 8191 is often mentioned
    // as the maximum length that works. Add an extra 7 characters (5 for
    // "PATH=", 1 for a potential ";" and 1 for the NUL byte).
#تعریف_کن EXTRA_LEN 7
#تعریف_کن MAX_ENV_STRING_LEN 8191

    إذا (pathlen + wcslen(exe_pathw) < MAX_ENV_STRING_LEN)
    {
	WCHAR   موقت[MAX_ENV_STRING_LEN + EXTRA_LEN] = L"PATH=";
	size_t  templen = 5;

	إذا (pathlen == 0)
	    wcscpy(موقت + templen, exe_pathw);
	وإلا
	{
	    wcscpy(موقت + templen, مسیر);

	    // Check if exe_path is already included in $PATH.
	    إذا (wcsstr(موقت, exe_pathw) == عدم)
	    {
		templen += pathlen;

		// Append ';' if $PATH doesn't end with it.
		إذا (موقت[templen - 1] != L';')
		{
		    wcscpy(موقت + templen, L";");
		    ++templen;
		}

		wcscpy(موقت + templen, exe_pathw);
	    }
	}

	_wputenv(موقت);
#اگر_تعریف_شده libintl_wputenv
	libintl_wputenv(موقت);
#پایان_اگر
    }
#لغو_تعریف EXTRA_LEN
#لغو_تعریف MAX_ENV_مسیر_LEN
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
vim_بارگیری_Lib(ثابت حرف *name)
{
    HINSTANCE	dll = عدم;

    // No need to load any library when registering OLE.
    إذا (یافت_ثبات_arg)
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
    مجموعه_Current_Directory_W(exe_pathw);
    dll = بارگیری_Library(name);
    مجموعه_Current_Directory_W(old_dirw);
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
    PIMAGE_DOS_سرآیند	pDOS = (PIMAGE_DOS_سرآیند)pImage;
    PIMAGE_NT_سرآیندها	pPE;

    إذا (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	أرجع FALSE;
    pPE = (PIMAGE_NT_سرآیندها)(pImage + pDOS->e_lfanew);
    إذا (pPE->Signature != IMAGE_NT_SIGNATURE)
	أرجع FALSE;
    إذا (pPE->Optional_سرآیند.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	أرجع TRUE;
    أرجع FALSE;
}
#پایان_اگر

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
    PIMAGE_DOS_سرآیند		pDOS = (PIMAGE_DOS_سرآیند)hInst;
    PIMAGE_NT_سرآیندها		pPE;
    PIMAGE_IMPORT_DESCRIPTOR	pImpDesc;
    PIMAGE_THUNK_داده		pIAT;	    // Import Address Table
    PIMAGE_THUNK_داده		pINT;	    // Import Name Table
    PIMAGE_IMPORT_BY_NAME	pImpName;
    DWORD			ImpVA;

    إذا (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	أرجع عدم;
    pPE = (PIMAGE_NT_سرآیندها)(pImage + pDOS->e_lfanew);
    إذا (pPE->Signature != IMAGE_NT_SIGNATURE)
	أرجع عدم;

    ImpVA = pPE->Optional_سرآیند
		.داده_Directory[IMAGE_DIRECTORY_مدخل_IMPORT].Virtual_آدرس;
    إذا (ImpVA == 0)
	أرجع عدم;	// No Import Table
    pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImage + ImpVA);

    لكل (; pImpDesc->اولی_Thunk; ++pImpDesc)
    {
	إذا (!pImpDesc->Original_اولی_Thunk)
	    استمر;
	pIAT = (PIMAGE_THUNK_داده)(pImage + pImpDesc->اولی_Thunk);
	pINT = (PIMAGE_THUNK_داده)(pImage + pImpDesc->Original_اولی_Thunk);
	لكل (; pIAT->u1.Function; ++pIAT, ++pINT)
	{
	    إذا (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
		استمر;
	    pImpName = (PIMAGE_IMPORT_BY_NAME)(pImage
					+ (UINT_PTR)(pINT->u1.آدرس_از_داده));
	    إذا (strcmp((حرف *)pImpName->Name, funcname) == 0)
	    {
		فراغ *original;
		DWORD old, new = صفحه_READWRITE;

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
پیدا_کن_imported_module_by_funcname(HINSTANCE hInst, ثابت حرف *funcname)
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
#پایان_اگر

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
    PIMAGE_DOS_سرآیند		pDOS = (PIMAGE_DOS_سرآیند)hInst;
    PIMAGE_NT_سرآیندها		pPE;
    PIMAGE_EXPORT_DIRECTORY	pExpDir;
    DWORD			ExpVA;
    DWORD			Exp_اندازه;
    LPDWORD			p_Function_جدول;

    إذا (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
	أرجع عدم;
    pPE = (PIMAGE_NT_سرآیندها)(pImage + pDOS->e_lfanew);
    إذا (pPE->Signature != IMAGE_NT_SIGNATURE)
	أرجع عدم;

    ExpVA = pPE->Optional_سرآیند
		.داده_Directory[IMAGE_DIRECTORY_مدخل_EXPORT].Virtual_آدرس;
    Exp_اندازه = pPE->Optional_سرآیند
		.داده_Directory[IMAGE_DIRECTORY_مدخل_EXPORT].اندازه_2;
    إذا (ExpVA == 0)
	أرجع hInst;	// No Export Directory
    pExpDir = (PIMAGE_EXPORT_DIRECTORY)(pImage + ExpVA);
    p_Function_جدول = (LPDWORD)(pImage + pExpDir->آدرس_از_Functions);

    إذا (pExpDir->عدد_از_نامها == 0)
	أرجع hInst;	// No export names.

    // Check only the first entry.
    إذا ((p_Function_جدول[0] < ExpVA) || (p_Function_جدول[0] >= ExpVA + Exp_اندازه))
	// The first entry is not a function forwarder.
	أرجع hInst;

    // The first entry is a function forwarder.
    // The name is represented as "DllName.FunctionName".
    ثابت حرف *name = (ثابت حرف *)(pImage + p_Function_جدول[0]);
    ثابت حرف *p = strchr(name, '.');
    إذا (p == عدم)
	أرجع hInst;

    // Extract DllName.
    حرف buf[MAX_مسیر];
    إذا (p - name + 1 > حجم(buf))
	أرجع عدم;
    strncpy(buf, name, p - name);
    buf[p - name] = NUL;
    أرجع GetModuleHandleA(buf);
}
#پایان_اگر

#إذا defined(DYNAMIC_GETTEXT)
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT1 "libintl-8.dll"
#  define GETTEXT_DLL_ALT2 "intl.dll"
# endif
// Dummy functions
ساكن حرف *null_libintl_gettext(ثابت حرف *);
ساكن حرف *null_libintl_ngettext(ثابت حرف *, ثابت حرف *, بیعلامت دراز n);
ساكن حرف *null_libintl_textdomain(ثابت حرف *);
ساكن حرف *null_libintl_bindtextdomain(ثابت حرف *, ثابت حرف *);
ساكن حرف *null_libintl_bind_textdomain_codeset(ثابت حرف *, ثابت حرف *);
ساكن صحيح null_libintl_wputenv(ثابت wchar_t *);

ساكن HINSTANCE hLibintlDLL = عدم;
حرف *(*dyn_libintl_gettext)(ثابت حرف *) = null_libintl_gettext;
حرف *(*dyn_libintl_ngettext)(ثابت حرف *, ثابت حرف *, بیعلامت دراز n)
						= null_libintl_ngettext;
حرف *(*dyn_libintl_textdomain)(ثابت حرف *) = null_libintl_textdomain;
حرف *(*dyn_libintl_bindtextdomain)(ثابت حرف *, ثابت حرف *)
						= null_libintl_bindtextdomain;
حرف *(*dyn_libintl_bind_textdomain_codeset)(ثابت حرف *, ثابت حرف *)
				       = null_libintl_bind_textdomain_codeset;
صحيح (*dyn_libintl_wputenv)(ثابت wchar_t *) = null_libintl_wputenv;
صحيح *dyn_libintl_nl_پیام_cat_cntr = عدم;

    صحيح
dyn_libintl_init(فراغ)
{
    صحيح i;
    ساكن بنية
    {
	حرف	    *name;
	FARPROC	    *ptr;
    } libintl_مدخل[] =
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
    hLibintlDLL = vim_بارگیری_Lib(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT1
    إذا (!hLibintlDLL)
	hLibintlDLL = vim_بارگیری_Lib(GETTEXT_DLL_ALT1);
# endif
# ifdef GETTEXT_DLL_ALT2
    إذا (!hLibintlDLL)
	hLibintlDLL = vim_بارگیری_Lib(GETTEXT_DLL_ALT2);
# endif
    إذا (!hLibintlDLL)
    {
	إذا (p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_بارگیری_library_str_str), GETTEXT_DLL, GetWin32Error());
	    verbose_leave();
	}
	أرجع 0;
    }
    لكل (i = 0; libintl_مدخل[i].name != عدم
					 && libintl_مدخل[i].ptr != عدم; ++i)
    {
	إذا ((*libintl_مدخل[i].ptr = Get_Proc_آدرس(hLibintlDLL,
					      libintl_مدخل[i].name)) == عدم)
	{
	    dyn_libintl_انتها();
	    إذا (p_verbose > 0)
	    {
		verbose_enter();
		semsg(_(e_could_not_بارگیری_library_function_str), libintl_مدخل[i].name);
		verbose_leave();
	    }
	    أرجع 0;
	}
    }

    // The bind_textdomain_codeset() function is optional.
    dyn_libintl_bind_textdomain_codeset = (حرف *(*)(ثابت حرف *, ثابت حرف *))
			Get_Proc_آدرس(hLibintlDLL, "bind_textdomain_codeset");
    إذا (dyn_libintl_bind_textdomain_codeset == عدم)
	dyn_libintl_bind_textdomain_codeset =
					 null_libintl_bind_textdomain_codeset;

    // _wputenv() function for the libintl.dll is optional.
    hmsvcrt = پیدا_کن_imported_module_by_funcname(hLibintlDLL, "getenv");
    إذا (hmsvcrt != عدم)
	dyn_libintl_wputenv = (صحيح (*)(ثابت wchar_t *))
					Get_Proc_آدرس(hmsvcrt, "_wputenv");
    إذا (dyn_libintl_wputenv == عدم || dyn_libintl_wputenv == _wputenv)
	dyn_libintl_wputenv = null_libintl_wputenv;

    // The _nl_msg_cat_cntr variable is optional.  It is used to make gettext
    // drop the translations it cached.
    dyn_libintl_nl_پیام_cat_cntr = (صحيح *)Get_Proc_آدرس(hLibintlDLL,
							 "_nl_msg_cat_cntr");

    أرجع 1;
}

    فراغ
dyn_libintl_انتها(فراغ)
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
    dyn_libintl_nl_پیام_cat_cntr	= عدم;
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
	بیعلامت دراز n)
{
    أرجع (حرف *)(n == 1 ? msgid : msgid_plural);
}

    ساكن حرف *
null_libintl_bindtextdomain(
	ثابت حرف *domainname استفادهنشده,
	ثابت حرف *dirname استفادهنشده)
{
    أرجع عدم;
}

    ساكن حرف *
null_libintl_bind_textdomain_codeset(
	ثابت حرف *domainname استفادهنشده,
	ثابت حرف *codeset استفادهنشده)
{
    أرجع عدم;
}

    ساكن حرف *
null_libintl_textdomain(ثابت حرف *domainname استفادهنشده)
{
    أرجع عدم;
}

    ساكن صحيح
null_libintl_wputenv(ثابت wchar_t *envstring استفادهنشده)
{
    أرجع 0;
}

#پایان_اگر // DYNAMIC_GETTEXT

// This symbol is not defined in older versions of the SDK or Visual C++

#اگر_تعریف_نشده VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#پایان_اگر

#اگر_تعریف_شده HAVE_ACL
# include <aclapi.h>
# ifndef PROTECTED_DACL_SECURITY_INFORMATION
#  define PROTECTED_DACL_SECURITY_INFORMATION	0x80000000L
# endif
#پایان_اگر

#اگر_تعریف_شده HAVE_ACL
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

    إذا (!Lookup_Privilege_مقدار(عدم, lpszPrivilege, &luid))
    {
	CloseHandle(hToken);
	أرجع FALSE;
    }

    tokenPrivileges.Privilege_شمار	     = 1;
    tokenPrivileges.Privileges[0].Luid       = luid;
    tokenPrivileges.Privileges[0].صفات_2 = SE_PRIVILEGE_ENABLED;

    إذا (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, عدم, عدم))
    {
	CloseHandle(hToken);
	أرجع FALSE;
    }

    إذا (Get_آخرین_Error() != ERROR_SUCCESS)
    {
	CloseHandle(hToken);
	أرجع FALSE;
    }

    CloseHandle(hToken);
    أرجع TRUE;
}
#پایان_اگر

/*
 * Fill in "windowsVersion" if possible and enable security privilege for ACL.
 */
    فراغ
PlatformId(فراغ)
{
#اگر_تعریف_شده FEAT_EVAL
    DWORD major = (win_نسخه >> 24) & 0xFF;
    DWORD minor = (win_نسخه >> 16) & 0xFF;
    vim_snprintf(windows_نسخه, حجم(windows_نسخه), "%d.%d", major, minor);
#پایان_اگر

#اگر_تعریف_شده HAVE_ACL
    // Enable privilege for getting or setting SACLs.
    إذا (!win32_enable_privilege(SE_SECURITY_NAME))
	أرجع;
#پایان_اگر
}

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define لغزش_2  (لغزش_PRESSED)
# define CTRL   (راست_CTRL_PRESSED | چپ_CTRL_PRESSED)
# define ALT    (راست_ALT_PRESSED  | چپ_ALT_PRESSED)
# define ALT_GR (راست_ALT_PRESSED  | چپ_CTRL_PRESSED)


// When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
// We map function keys to their ANSI terminal equivalents, as produced
// by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
// ANSI key with a value >= '\300' is nonstandard, but provided anyway
// so that the user can have access to all SHIFT-, CTRL-, and ALT-
// combinations of function/arrow/etc keys.

ساكن ثابت بنية
{
    کلمه_2    w_Virt_کلید;
    BOOL    f_Ansi_کلید;
    صحيح	    chAlone;
    صحيح	    ch_لغزش;
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
    { VK_به_بالا,	TRUE,	'H',	'\304',	'\305',	    '\306', },
    { VK_پیشین,	TRUE,	'I',	'\307',	'\204',	    '\310', }, // PgUp
    { VK_چپ,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_راست,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_انتها,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_پایین,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_بعدی,	TRUE,	'Q',	'\322',	'v',	    '\323', }, // PgDn
    { VK_درج,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },
    { VK_پشت,	TRUE,	'x',	'y',	'z',	    '{', }, // Backspace

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
    { VK_جمع_کن,	TRUE,   'N',    'N',    'N',	'N',	}, // keyp '+'
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
    ساكن صحيح
win32_kbd_patch_کلید(
    کلید_EVENT_رکورد *pker)
{
    UINT uMods = pker->dw_Control_کلید_State;
    ساكن صحيح s_iIsDead = 0;
    ساكن کلمه_2 aw_Ansi_کد[2];
    ساكن BYTE abKeystate[256];

    إذا (s_iIsDead == 2)
    {
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_کد[1];
	s_iIsDead = 0;
	أرجع 1;
    }

    // check if it already has a valid unicode character.
    إذا (pker->uChar.UnicodeChar != 0)
	أرجع 1;

    CLEAR_فیلد(abKeystate);

    // Clear any pending dead keys
    مقصد_Unicode(VK_SPACE, Map_Virtual_کلید(VK_SPACE, 0), abKeystate, aw_Ansi_کد, 2, 0);

    إذا (uMods & لغزش_PRESSED)
	abKeystate[VK_لغزش] = 0x80;
    إذا (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    إذا ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = مقصد_Unicode(pker->w_Virtual_کلید_کد, pker->w_Virtual_پیمایش_کد,
			abKeystate, aw_Ansi_کد, 2, 0);

    إذا (s_iIsDead > 0)
	pker->uChar.UnicodeChar = (WCHAR) aw_Ansi_کد[0];

    أرجع s_iIsDead;
}

ساكن BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    ساكن BOOL
decode_کلید_event(
    کلید_EVENT_رکورد	*pker,
    WCHAR		*pch,
    WCHAR		*pch2,
    صحيح			*pmodifiers,
    BOOL		fDoPost استفادهنشده)
{
    صحيح i;
    ثابت صحيح nModifs = pker->dw_Control_کلید_State & (لغزش_2 | ALT | CTRL);

    *pch = *pch2 = NUL;
    g_fJustGotFocus = FALSE;

    // ignore key up events
    إذا (!pker->b_کلید_پایین)
	أرجع FALSE;

    // ignore some keystrokes
    بدل (pker->w_Virtual_کلید_کد)
    {
    // modifiers
    حالة VK_لغزش:
    حالة VK_CONTROL:
    حالة VK_MENU:   // Alt key
	أرجع FALSE;

    افتراضي:
	اكسر;
    }

    // Shift-TAB
    إذا (pker->w_Virtual_کلید_کد == VK_TAB && (nModifs & لغزش_PRESSED))
    {
	*pch = K_NUL;
	*pch2 = '\017';
	أرجع TRUE;
    }

    لكل (i = ARRAY_درازی(Virt_کلید_Map); --i >= 0; )
    {
	إذا (Virt_کلید_Map[i].w_Virt_کلید == pker->w_Virtual_کلید_کد)
	{
	    *pch = Virt_کلید_Map[i].chAlone;
	    إذا ((nModifs & لغزش_2) != 0)
		*pch = Virt_کلید_Map[i].ch_لغزش;
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
			إذا (pker->w_Virtual_کلید_کد >= VK_F1
			    && pker->w_Virtual_کلید_کد <= VK_F12)
			{
			    إذا ((nModifs & ALT) != 0)
			    {
				*pmodifiers |= MOD_ماسک_ALT;
				إذا ((nModifs & لغزش_2) == 0)
				    *pch2 = Virt_کلید_Map[i].chAlone;
			    }
			    إذا ((nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_ماسک_CTRL;
				إذا ((nModifs & لغزش_2) == 0)
				    *pch2 = Virt_کلید_Map[i].chAlone;
			    }
			}
			وإلا إذا (pker->w_Virtual_کلید_کد >= VK_انتها
				&& pker->w_Virtual_کلید_کد <= VK_پایین)
			{
			    // (0x23 - 0x28): VK_END, VK_HOME,
			    // VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN

			    *pmodifiers = 0;
			    *pch2 = Virt_کلید_Map[i].chAlone;
			    إذا ((nModifs & لغزش_2) != 0
						    && (nModifs & ~لغزش_2) == 0)
			    {
				*pch2 = Virt_کلید_Map[i].ch_لغزش;
			    }
			    إذا ((nModifs & CTRL) != 0
						     && (nModifs & ~CTRL) == 0)
			    {
				*pch2 = Virt_کلید_Map[i].chCtrl;
				إذا (pker->w_Virtual_کلید_کد == VK_به_بالا
				    || pker->w_Virtual_کلید_کد == VK_پایین)
				{
				    *pmodifiers |= MOD_ماسک_CTRL;
				    *pch2 = Virt_کلید_Map[i].chAlone;
				}
			    }
			    إذا ((nModifs & لغزش_2) != 0
						      && (nModifs & CTRL) != 0)
			    {
				*pmodifiers |= MOD_ماسک_CTRL;
				*pch2 = Virt_کلید_Map[i].ch_لغزش;
			    }
			    إذا ((nModifs & ALT) != 0)
			    {
				*pch2 = Virt_کلید_Map[i].chAlt;
				*pmodifiers |= MOD_ماسک_ALT;
				إذا ((nModifs & ~ALT) == 0)
				{
				    *pch2 = Virt_کلید_Map[i].chAlone;
				}
				وإلا إذا ((nModifs & لغزش_2) != 0)
				{
				    *pch2 = Virt_کلید_Map[i].ch_لغزش;
				}
				وإلا إذا ((nModifs & CTRL) != 0)
				{
				    إذا (pker->w_Virtual_کلید_کد == VK_به_بالا
					|| pker->w_Virtual_کلید_کد == VK_پایین)
				    {
					*pmodifiers |= MOD_ماسک_CTRL;
					*pch2 = Virt_کلید_Map[i].chAlone;
				    }
				    وإلا
				    {
					*pch2 = Virt_کلید_Map[i].chCtrl;
				    }
				}
			    }
			}
			وإلا إذا (pker->w_Virtual_کلید_کد == VK_درج
					&& (nModifs & لغزش_2) != 0
					&& (nModifs & ~لغزش_2) == 0)
			{
			    *pmodifiers = 0;
			    *pch2 = Virt_کلید_Map[i].ch_لغزش;
			}
			وإلا
			{
			    *pch2 = Virt_کلید_Map[i].chAlone;
			    إذا ((nModifs & لغزش_2) != 0)
				*pmodifiers |= MOD_ماسک_لغزش;
			    إذا ((nModifs & CTRL) != 0)
				*pmodifiers |= MOD_ماسک_CTRL;
			    إذا ((nModifs & ALT) != 0)
				*pmodifiers |= MOD_ماسک_ALT;
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
		*pmodifiers |= MOD_ماسک_ALT;

	    // Pass on SHIFT only for special keys, because we don't know when
	    // it's already included with the character.
	    إذا ((nModifs & لغزش_2) != 0 && *pch <= 0x20)
		*pmodifiers |= MOD_ماسک_لغزش;

	    // Pass on CTRL only for non-special keys, because we don't know
	    // when it's already included with the character.  And not when
	    // combined with ALT (which is ALTGR).
	    إذا ((nModifs & CTRL) != 0 && (nModifs & ALT) == 0
					       && *pch >= 0x20 && *pch < 0x80)
		*pmodifiers |= MOD_ماسک_CTRL;
	}
    }

    أرجع (*pch != NUL);
}

# إذا defined(FEAT_EVAL)
    ساكن صحيح
encode_کلید_event(dict_T *آرگومانها, درون_داد_رکورد *ir)
{
    ساكن صحيح s_dwMods = 0;

    char_u *action = dict_get_string(آرگومانها, "event", TRUE);
    إذا (action && (STRICMP(action, "keydown") == 0
					|| STRICMP(action, "keyup") == 0))
    {
	BOOL is_کلید_پایین = STRICMP(action, "keydown") == 0;
	کلمه_2 vk_کد = dict_get_عدد_def(آرگومانها, "keycode", 0);
	إذا (vk_کد <= 0 || vk_کد >= 0xFF)
	{
	    semsg(_(e_invalid_آرگومان_nr), (دراز)vk_کد);
	    أرجع FALSE;
	}

	ir->EventType = کلید_EVENT;
	کلید_EVENT_رکورد ker;
	Zero_حافظه(&ker, حجم(ker));
	ker.b_کلید_پایین = is_کلید_پایین;
	ker.w_Repeat_شمار = 1;
	ker.w_Virtual_پیمایش_کد = 0;
	ker.dw_Control_کلید_State = 0;
	صحيح mods = (صحيح)dict_get_عدد(آرگومانها, "modifiers");
	// Encode the win32 console key modifiers from Vim keyboard modifiers.
	إذا (mods)
	{
	    // If "modifiers" is explicitly set in the args, then we reset any
	    // remembered modifier key state that may have been set from
	    // earlier mod-key-down events, even if they are not yet unset by
	    // earlier mod-key-up events.
	    s_dwMods = 0;
	    إذا (mods & MOD_ماسک_لغزش)
		ker.dw_Control_کلید_State |= لغزش_PRESSED;
	    إذا (mods & MOD_ماسک_CTRL)
		ker.dw_Control_کلید_State |= چپ_CTRL_PRESSED;
	    إذا (mods & MOD_ماسک_ALT)
		ker.dw_Control_کلید_State |= چپ_ALT_PRESSED;
	}

	إذا (vk_کد == VK_LSHIFT || vk_کد == VK_RSHIFT || vk_کد == VK_لغزش)
	{
	    إذا (is_کلید_پایین)
		s_dwMods |= لغزش_PRESSED;
	    وإلا
		s_dwMods &= ~لغزش_PRESSED;
	}
	وإلا إذا (vk_کد == VK_LCONTROL || vk_کد == VK_CONTROL)
	{
	    إذا (is_کلید_پایین)
		s_dwMods |= چپ_CTRL_PRESSED;
	    وإلا
		s_dwMods &= ~چپ_CTRL_PRESSED;
	}
	وإلا إذا (vk_کد == VK_RCONTROL)
	{
	    إذا (is_کلید_پایین)
		s_dwMods |= راست_CTRL_PRESSED;
	    وإلا
		s_dwMods &= ~راست_CTRL_PRESSED;
	}
	وإلا إذا (vk_کد == VK_LMENU || vk_کد == VK_MENU)
	{
	    إذا (is_کلید_پایین)
		s_dwMods |= چپ_ALT_PRESSED;
	    وإلا
		s_dwMods &= ~چپ_ALT_PRESSED;
	}
	وإلا إذا (vk_کد == VK_RMENU)
	{
	    إذا (is_کلید_پایین)
		s_dwMods |= راست_ALT_PRESSED;
	    وإلا
		s_dwMods &= ~راست_ALT_PRESSED;
	}
	ker.dw_Control_کلید_State |= s_dwMods;
	ker.w_Virtual_کلید_کد = vk_کد;
	ker.uChar.UnicodeChar = 0;
	ir->Event.کلید_Event_2 = ker;
	vim_free(action);
    }
    وإلا
    {
	إذا (action == عدم)
	{
	    semsg(_(e_missing_آرگومان_str), "event");
	}
	وإلا
	{
	    semsg(_(e_invalid_مقدار_for_آرگومان_str_str), "event", action);
	    vim_free(action);
	}
	أرجع FALSE;
    }
    أرجع TRUE;
}
# endif  // FEAT_EVAL
#پایان_اگر // !FEAT_GUI_MSWIN || VIMDLL


/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ساكن صحيح g_fMouseAvail = FALSE;   // mouse present
ساكن صحيح g_f_Mouse_فعال = FALSE;  // mouse enabled
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
    إذا (gui.درون_use)
	أرجع;
# endif
    إذا (!g_fMouseAvail)
	أرجع;

    g_f_Mouse_فعال = on;
    Get_Console_نما(g_h_Con_درون, &cmodein);

    إذا (g_f_Mouse_فعال)
    {
	cmodein |= ENABLE_MOUSE_درون_داد;
	cmodein &= ~ENABLE_سریع_EDIT_نما;
    }
    وإلا
    {
	cmodein &= ~ENABLE_MOUSE_درون_داد;
	cmodein |= g_cmodein & ENABLE_سریع_EDIT_نما;
    }

    مجموعه_Console_نما(g_h_Con_درون, cmodein | ENABLE_EXTENDED_FLAGS);
}


# إذا defined(FEAT_BEVAL_جمله)
/*
 * Called when 'balloonevalterm' changed.
 */
    فراغ
mch_bevalterm_تغییریافته(فراغ)
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
    ساكن فراغ
decode_mouse_wheel(MOUSE_EVENT_رکورد *pmer)
{
    صحيح	    horizontal = (pmer->dwEventFlags == MOUSE_HWHEELED);
    صحيح	    zDelta = pmer->dwButtonState;

    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

# ifdef FEAT_PROP_POPUP
    صحيح lcol = g_xMouse;
    صحيح lrow = g_yMouse;
    win_T *wp = mouse_پیدا_کن_win(&lrow, &lcol, پیدا_کن_POPUP);
    إذا (wp != عدم && popup_is_popup(wp))
    {
	g_nMouseClick = -1;
	cmdarg_T cap;
	oparg_T oa;
	CLEAR_فیلد(cap);
	clear_oparg(&oa);
	cap.oap = &oa;
	إذا (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_چپ : MSCR_راست;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	وإلا
	{
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	    cap.arg = zDelta < 0 ? MSCR_به_بالا : MSCR_پایین;
	}

	// Mouse hovers over popup window, scroll it if possible.
	mouse_ردیف = wp->w_winrow;
	mouse_ستون = wp->w_wincol;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	بیرون_flush();
    }
# endif
    mouse_ستون = g_xMouse;
    mouse_ردیف = g_yMouse;

    char_u modifiers = 0;
    char_u direction = 0;

    // Decode the direction into an event that Vim can process
    إذا (horizontal)
	direction = zDelta >= 0 ? KE_MOUSELEFT : KE_MOUSERIGHT;
    وإلا
	direction = zDelta >= 0 ? KE_MOUSEDOWN : KE_MOUSEUP;

    // Decode the win32 console key modifiers into Vim mouse modifiers.
    إذا (pmer->dw_Control_کلید_State & لغزش_PRESSED)
	modifiers |= MOD_ماسک_لغزش; // MOUSE_SHIFT;
    إذا (pmer->dw_Control_کلید_State & (راست_CTRL_PRESSED | چپ_CTRL_PRESSED))
	modifiers |= MOD_ماسک_CTRL; // MOUSE_CTRL;
    إذا (pmer->dw_Control_کلید_State & (راست_ALT_PRESSED  | چپ_ALT_PRESSED))
	modifiers |= MOD_ماسک_ALT; // MOUSE_ALT;

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
    MOUSE_EVENT_رکورد *pmer)
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
    ساكن DWORD s_dw_آخرین_Click_زمان = 0;
    ساكن BOOL s_f_بعدی_Is_میانه = FALSE;

    ساكن DWORD cButtons = 0;	// number of buttons supported

    ثابت DWORD چپ_3 = FROM_چپ_1_ST_BUTTON_PRESSED;
    ثابت DWORD میانه = FROM_چپ_2_ND_BUTTON_PRESSED;
    ثابت DWORD راست_3 = RIGHTMOST_BUTTON_PRESSED;
    ثابت DWORD چپ_راست = چپ_3 | راست_3;

    صحيح nButton;

    إذا (cButtons == 0 && !Get_عدد_از_Console_Mouse_Buttons(&cButtons))
	cButtons = 2;

    إذا (!g_fMouseAvail || !g_f_Mouse_فعال)
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
# ifdef FEAT_BEVAL_جمله
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
	    DWORD dwLR = (pmer->dwButtonState & چپ_راست);

	    // if either left or right button only is pressed, see if the
	    // next mouse event has both of them pressed
	    إذا (dwLR == چپ_3 || dwLR == راست_3)
	    {
		لكل (;;)
		{
		    // wait a short time for next input event
		    إذا (WaitForSingleObject(g_h_Con_درون, p_mouset / 3)
							     != WAIT_OBJECT_0)
			اكسر;
		    وإلا
		    {
			DWORD c_رکوردها = 0;
			درون_داد_رکورد ir;
			MOUSE_EVENT_رکورد* pmer2 = &ir.Event.MouseEvent;

			peek_console_درون_داد(g_h_Con_درون, &ir, 1, &c_رکوردها);

			إذا (c_رکوردها == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & چپ_راست))
			    اكسر;
			وإلا
			{
			    إذا (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				read_console_درون_داد(g_h_Con_درون, &ir, 1, &c_رکوردها);

				أرجع decode_mouse_event(pmer2);
			    }
			    وإلا إذا (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				// throw away spurious mouse move
				read_console_درون_داد(g_h_Con_درون, &ir, 1, &c_رکوردها);

				// are there any more mouse events in queue?
				peek_console_درون_داد(g_h_Con_درون, &ir, 1, &c_رکوردها);

				إذا (c_رکوردها==0 || ir.EventType != MOUSE_EVENT)
				    اكسر;
			    }
			    وإلا
				اكسر;
			}
		    }
		}
	    }
	}

	إذا (s_f_بعدی_Is_میانه)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_میانه;
	    s_f_بعدی_Is_میانه = FALSE;
	}
	وإلا إذا (cButtons == 2	&&
	    ((pmer->dwButtonState & چپ_راست) == چپ_راست))
	{
	    nButton = MOUSE_میانه;

	    إذا (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_f_بعدی_Is_میانه = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	وإلا إذا ((pmer->dwButtonState & چپ_3) == چپ_3)
	    nButton = MOUSE_چپ;
	وإلا إذا ((pmer->dwButtonState & میانه) == میانه)
	    nButton = MOUSE_میانه;
	وإلا إذا ((pmer->dwButtonState & راست_3) == راست_3)
	    nButton = MOUSE_راست;

	إذا (! s_fReleased && ! s_f_بعدی_Is_میانه
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    أرجع FALSE;

	s_fReleased = s_f_بعدی_Is_میانه;
    }

    إذا (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	// button pressed or released, without mouse moving
	إذا (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dw_Current_زمان = Get_Tick_شمار();

	    إذا (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
# ifdef FEAT_DIFF
		    || s_old_topfill != curwin->w_topfill
# endif
		    || (صحيح)(dw_Current_زمان - s_dw_آخرین_Click_زمان) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    وإلا إذا (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dw_آخرین_Click_زمان = dw_Current_زمان;
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

    إذا (pmer->dw_Control_کلید_State & لغزش_PRESSED)
	g_nMouseClick |= MOUSE_لغزش;
    إذا (pmer->dw_Control_کلید_State & (راست_CTRL_PRESSED | چپ_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    إذا (pmer->dw_Control_کلید_State & (راست_ALT_PRESSED  | چپ_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    إذا (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	مجموعه_رقم_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

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
encode_mouse_event(dict_T *آرگومانها, درون_داد_رکورد *ir)
{
    صحيح		button;
    صحيح		ردیف;
    صحيح		ستون;
    صحيح		repeated_click;
    int_u	mods = 0;
    صحيح		move;

    إذا (!dict_has_کلید(آرگومانها, "row") || !dict_has_کلید(آرگومانها, "col"))
	أرجع FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (صحيح)dict_get_bool(آرگومانها, "move", FALSE);
    إذا (!move && (!dict_has_کلید(آرگومانها, "button")
	    || !dict_has_کلید(آرگومانها, "multiclick")
	    || !dict_has_کلید(آرگومانها, "modifiers")))
	أرجع FALSE;

    ردیف = (صحيح)dict_get_عدد(آرگومانها, "row") - 1;
    ستون = (صحيح)dict_get_عدد(آرگومانها, "col") - 1;

    ir->EventType = MOUSE_EVENT;
    MOUSE_EVENT_رکورد mer;
    Zero_حافظه(&mer, حجم(mer));
    mer.dwMousePosition.X  = ستون;
    mer.dwMousePosition.Y  = ردیف;

    إذا (move)
    {
	mer.dwButtonState = 0;
	mer.dwEventFlags = MOUSE_MOVED;
    }
    وإلا
    {
	button = (صحيح)dict_get_عدد(آرگومانها, "button");
	repeated_click = (صحيح)dict_get_عدد(آرگومانها, "multiclick");
	mods = (صحيح)dict_get_عدد(آرگومانها, "modifiers");
	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_مجموعه_hor_scroll_گام(6);
	mouse_مجموعه_vert_scroll_گام(3);

	بدل (button)
	{
	    حالة MOUSE_چپ:
		mer.dwButtonState = FROM_چپ_1_ST_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		اكسر;
	    حالة MOUSE_میانه:
		mer.dwButtonState = FROM_چپ_2_ND_BUTTON_PRESSED;
		mer.dwEventFlags = repeated_click == 1 ? DOUBLE_CLICK : 0;
		اكسر;
	    حالة MOUSE_راست:
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
		mer.dwButtonState = FROM_چپ_3_RD_BUTTON_PRESSED;
		اكسر;
	    حالة MOUSE_X2:
		mer.dwButtonState = FROM_چپ_4_TH_BUTTON_PRESSED;
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
		semsg(_(e_invalid_آرگومان_str), "button");
		أرجع FALSE;
	}
    }

    mer.dw_Control_کلید_State = 0;
    إذا (mods != 0)
    {
	// Encode the win32 console key modifiers from Vim MOUSE modifiers.
	إذا (mods & MOUSE_لغزش)
	    mer.dw_Control_کلید_State |= لغزش_PRESSED;
	إذا (mods & MOUSE_CTRL)
	    mer.dw_Control_کلید_State |= چپ_CTRL_PRESSED;
	إذا (mods & MOUSE_ALT)
	    mer.dw_Control_کلید_State |= چپ_ALT_PRESSED;
    }
    ir->Event.MouseEvent = mer;
    أرجع TRUE;
}
# endif  // FEAT_EVAL

    ساكن صحيح
write_درون_داد_رکورد_میانگیر(درون_داد_رکورد* irEvents, صحيح n_درازی)
{
    صحيح				n_شمار = 0;
    درون_داد_رکورد_میانگیر_گره_T	*event_گره;

    طالما (n_شمار < n_درازی)
    {
	event_گره = alloc(حجم(درون_داد_رکورد_میانگیر_گره_T));
	إذا (event_گره == عدم)
	    أرجع -1;

	event_گره->ir = irEvents[n_شمار++];
	event_گره->بعدی = عدم;

	درون_داد_رکورد_میانگیر.درازی++;
	إذا (درون_داد_رکورد_میانگیر.دم == عدم)
	{
	    درون_داد_رکورد_میانگیر.سر = event_گره;
	    درون_داد_رکورد_میانگیر.دم = event_گره;
	}
	وإلا
	{
	    درون_داد_رکورد_میانگیر.دم->بعدی = event_گره;
	    درون_داد_رکورد_میانگیر.دم = درون_داد_رکورد_میانگیر.دم->بعدی;
	}
    }
    أرجع n_شمار;
}

    ساكن صحيح
read_درون_داد_رکورد_میانگیر(درون_داد_رکورد* irEvents, صحيح n_Max_درازی)
{
    صحيح n_شمار = 0;
    طالما (n_شمار < n_Max_درازی && درون_داد_رکورد_میانگیر.سر != عدم)
    {
	درون_داد_رکورد_میانگیر.درازی--;
	درون_داد_رکورد_میانگیر_گره_T *برداشت_سر = درون_داد_رکورد_میانگیر.سر;
	irEvents[n_شمار++] = برداشت_سر->ir;
	درون_داد_رکورد_میانگیر.سر = برداشت_سر->بعدی;
	vim_free(برداشت_سر);
	إذا (درون_داد_رکورد_میانگیر.درازی == 0)
	    درون_داد_رکورد_میانگیر.دم = عدم;
    }
    أرجع n_شمار;
}
#وإلا  // FEAT_GUI_MSWIN && !VIMDLL
    فراغ
mch_setmouse(صحيح on استفادهنشده)
{
}
#پایان_اگر // FEAT_GUI_MSWIN && !VIMDLL

#اگر_تعریف_شده FEAT_EVAL
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
test_mswin_event(char_u *event, dict_T *آرگومانها)
{
    صحيح lpEventsWritten = 0;

# إذا defined(VIMDLL) || defined(FEAT_GUI_MSWIN)
    إذا (gui.درون_use)
	أرجع test_gui_w32_sendevent(event, آرگومانها);
# endif

# إذا defined(VIMDLL) || !defined(FEAT_GUI_MSWIN)

// Currently implemented event record types are; KEY_EVENT and MOUSE_EVENT
// Potentially could also implement: FOCUS_EVENT and WINDOW_BUFFER_SIZE_EVENT
// Maybe also:  MENU_EVENT

    درون_داد_رکورد ir;
    BOOL درون_داد_encoded = FALSE;
    BOOL execute = FALSE;
    إذا (STRCMP(event, "key") == 0)
    {
	execute = dict_get_bool(آرگومانها, "execute", FALSE);
	إذا (dict_has_کلید(آرگومانها, "event"))
	    درون_داد_encoded = encode_کلید_event(آرگومانها, &ir);
	وإلا إذا (!execute)
	{
	    semsg(_(e_missing_آرگومان_str), "event");
	    أرجع FALSE;
	}
    }
    وإلا إذا (STRCMP(event, "mouse") == 0)
    {
	execute = TRUE;
	درون_داد_encoded = encode_mouse_event(آرگومانها, &ir);
    }
    وإلا
    {
	semsg(_(e_invalid_مقدار_for_آرگومان_str_str), "event", event);
	أرجع FALSE;
    }

    // Ideally, WriteConsoleInput would be used to inject these low-level
    // events.  But, this doesn't work well in the CI test environment.  So
    // implementing an input_record_buffer instead.
    إذا (درون_داد_encoded)
    {
	إذا ((lpEventsWritten = write_درون_داد_رکورد_میانگیر(&ir, 1)) < 0)
	{
	    semsg(_(e_بیرون_از_حافظه), "event");
	    أرجع FALSE;
	}
    }

    // Set flags to execute the event, ie. like feedkeys mode X.
    إذا (execute)
    {
	صحيح save_پیام_scroll = پیام_scroll;
	// Avoid a 1 second delay when the keys start Insert mode.
	پیام_scroll = FALSE;
	ch_log(عدم, "test_mswin_event() executing");
	exec_نرمال(TRUE, TRUE, TRUE);
	پیام_scroll |= save_پیام_scroll;
    }

# endif
    أرجع lpEventsWritten;
}
#پایان_اگر // FEAT_EVAL

#إذا defined(MCH_CURSOR_SHAPE)
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    ساكن فراغ
mch_مجموعه_cursor_shape(صحيح thickness)
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
	ConsoleCursorInfo.dw_اندازه = thickness;
	ConsoleCursorInfo.bVisible = s_cursor_visible;

	مجموعه_Console_Cursor_Info(g_h_Con_بیرون, &ConsoleCursorInfo);
	إذا (s_cursor_visible)
	    مجموعه_Console_Cursor_Position(g_h_Con_بیرون, g_coord);
    }
}

    فراغ
mch_update_cursor(فراغ)
{
    صحيح		idx;
    صحيح		thickness;

# ifdef VIMDLL
    إذا (gui.درون_use)
	أرجع;
# endif

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    إذا (shape_جدول[idx].shape == SHAPE_بلوک)
	thickness = 99;	// 100 doesn't work on W95
    وإلا
	thickness = shape_جدول[idx].percentage;
    mch_مجموعه_cursor_shape(thickness);
}
#پایان_اگر

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Handle FOCUS_EVENT.
 */
    ساكن فراغ
handle_focus_event(درون_داد_رکورد ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.b_مجموعه_Focus;
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
WaitForChar(دراز msec, صحيح ignore_درون_داد)
{
    DWORD	    dwNow = 0, dw_انتها_زمان = 0;
    درون_داد_رکورد    ir;
    DWORD	    c_رکوردها;
    WCHAR	    ch, ch2;
# ifdef FEAT_TIMERS
    صحيح		    tb_change_cnt = typebuf.tb_change_cnt;
# endif

    إذا (msec > 0)
	// Wait until the specified time has elapsed.
	dw_انتها_زمان = Get_Tick_شمار() + msec;
    وإلا إذا (msec < 0)
	// Wait forever.
	dw_انتها_زمان = INFINITE;

    // We need to loop until the end of the time period, because
    // we might get multiple unusable mouse events in that time.
    لكل (;;)
    {
	// Only process messages when waiting.
	إذا (msec != 0)
	{
# ifdef پیام_صف
	    parse_queued_messages();
# endif
# ifdef FEAT_MZSCHEME
	    mzvim_check_نخها();
# endif
# ifdef FEAT_CLIENTSERVER
	    serverProcessPendingMessages();
# endif
	}

	إذا (g_nMouseClick != -1
# ifdef FEAT_CLIENTSERVER
		|| (!ignore_درون_داد && درون_داد_موجود())
# endif
	   )
	    أرجع TRUE;

	إذا (msec > 0)
	{
	    // If the specified wait time has passed, return.  Beware that
	    // GetTickCount() may wrap around (overflow).
	    dwNow = Get_Tick_شمار();
	    إذا ((صحيح)(dwNow - dw_انتها_زمان) >= 0)
		اكسر;
	}
	إذا (msec != 0)
	{
	    DWORD dw_Wait_زمان = dw_انتها_زمان - dwNow;

	    // Don't wait for more than 11 msec to avoid dropping characters,
	    // check channel while waiting for input and handle a callback from
	    // 'balloonexpr'.
	    إذا (dw_Wait_زمان > 11)
		dw_Wait_زمان = 11;

# ifdef FEAT_MZSCHEME
	    إذا (mzthreads_allowed() && p_mzq > 0 && (دراز)dw_Wait_زمان > p_mzq)
		dw_Wait_زمان = p_mzq; // don't wait longer than 'mzquantum'
# endif
# ifdef FEAT_TIMERS
	    // When waiting very briefly don't trigger timers.
	    إذا (dw_Wait_زمان > 10)
	    {
		دراز	due_زمان;

		// Trigger timers and then get the time in msec until the next
		// one is due.  Wait up to that time.
		due_زمان = check_due_timer();
		إذا (typebuf.tb_change_cnt != tb_change_cnt)
		{
		    // timer may have used feedkeys().
		    أرجع FALSE;
		}
		إذا (due_زمان > 0 && dw_Wait_زمان > (DWORD)due_زمان)
		    dw_Wait_زمان = due_زمان;
	    }
# endif
	    إذا (
# ifdef FEAT_CLIENTSERVER
		    // Wait for either an event on the console input or a
		    // message in the client-server window.
		    پیام_wait_for_مضرب_objects(1, &g_h_Con_درون, FALSE,
				  dw_Wait_زمان, QS_SENDMESSAGE) != WAIT_OBJECT_0
# وإلا
		    wait_for_single_object(g_h_Con_درون, dw_Wait_زمان)
							      != WAIT_OBJECT_0
# endif
		    )
		استمر;
	}

	c_رکوردها = 0;
	peek_console_درون_داد(g_h_Con_درون, &ir, 1, &c_رکوردها);

# ifdef FEAT_MBYTE_IME
	// May have to redraw if the cursor ends up in the wrong place.
	// Only when not peeking.
	إذا (State == نما_CMDLINE && پیام_ردیف == ردیفها - 1 && msec != 0)
	{
	    CONSOLE_SCREEN_میانگیر_INFO csbi;

	    إذا (Get_Console_Screen_میانگیر_Info(g_h_Con_بیرون, &csbi))
	    {
		إذا (csbi.dwCursorPosition.Y != پیام_ردیف)
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

	إذا (c_رکوردها > 0)
	{
	    إذا (ir.EventType == کلید_EVENT && ir.Event.کلید_Event_2.b_کلید_پایین)
	    {
# ifdef FEAT_MBYTE_IME
		// Windows IME sends two '\n's with only one 'ENTER'.  First:
		// wVirtualKeyCode == 13. second: wVirtualKeyCode == 0
		إذا (ir.Event.کلید_Event_2.uChar.UnicodeChar == 0
			&& ir.Event.کلید_Event_2.w_Virtual_کلید_کد == 13)
		{
		    read_console_درون_داد(g_h_Con_درون, &ir, 1, &c_رکوردها);
		    استمر;
		}
# endif
		إذا (decode_کلید_event(&ir.Event.کلید_Event_2, &ch, &ch2,
								 عدم, FALSE))
		    أرجع TRUE;
	    }

	    read_console_درون_داد(g_h_Con_درون, &ir, 1, &c_رکوردها);

	    إذا (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    وإلا إذا (ir.EventType == پنجره_میانگیر_اندازه_EVENT)
	    {
		COORD dw_اندازه = ir.Event.پنجره_میانگیر_اندازه_Event.dw_اندازه;

		// Only call shell_resized() when the size actually changed to
		// avoid the screen is cleared.
		إذا (dw_اندازه.X != Columns || dw_اندازه.Y != ردیفها)
		{
		    CONSOLE_SCREEN_میانگیر_INFO csbi;
		    Get_Console_Screen_میانگیر_Info(g_h_Con_بیرون, &csbi);
		    dw_اندازه.X = csbi.sr_پنجره.راست_4 - csbi.sr_پنجره.چپ_4 + 1;
		    dw_اندازه.Y = csbi.sr_پنجره.Bottom - csbi.sr_پنجره.بالاترین_2 + 1;
		    إذا (dw_اندازه.X != Columns || dw_اندازه.Y != ردیفها)
		    {
			ResizeConBuf(g_h_Con_بیرون, dw_اندازه);
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
    إذا (درون_داد_موجود())
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
    إذا (gui.درون_use)
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
    إذا (gui.درون_use)
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
    g_h_Con_درون =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_صفات) عدم,
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
	درون_داد_رکورد ir;
	DWORD c_رکوردها = 0;

# ifdef FEAT_CLIENTSERVER
	(فراغ)WaitForChar(-1L, FALSE);
	إذا (درون_داد_موجود())
	    أرجع 0;
	إذا (g_nMouseClick != -1)
	    أرجع 0;
# endif
	إذا (read_console_درون_داد(g_h_Con_درون, &ir, 1, &c_رکوردها) == 0)
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
	وإلا إذا (ir.EventType == پنجره_میانگیر_اندازه_EVENT)
	    shell_resized();
	وإلا إذا (ir.EventType == MOUSE_EVENT)
	{
	    إذا (decode_mouse_event(&ir.Event.MouseEvent))
		أرجع 0;
	}
    }
}
#پایان_اگر // !FEAT_GUI_MSWIN


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
    char_u	*buf استفادهنشده,
    صحيح		maxlen استفادهنشده,
    دراز	زمان استفادهنشده,
    صحيح		tb_change_cnt استفادهنشده)
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
    إذا (gui.درون_use)
	أرجع 0;
# endif

    // First use any typeahead that was kept because "buf" was too small.
    إذا (typeaheadlen > 0)
	اذهب theend;

    إذا (زمان >= 0)
    {
	إذا (!WaitForChar(زمان, FALSE))     // no character available
	    أرجع 0;
    }
    وإلا    // time == -1, wait forever
    {
	mch_مجموعه_winsize_now();	// Allow winsize changes from now on

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
	إذا (typebuf_تغییریافته(tb_change_cnt))
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
	    ++no_reduce_کلیدها;
	    c = may_adjust_کلید_for_ctrl(modifiers, c);
	    --no_reduce_کلیدها;

	    // remove the SHIFT modifier for keys where it's already included,
	    // e.g., '(' and '*'
	    modifiers = may_remove_لغزش_modifier(modifiers, c);

	    إذا (typebuf_تغییریافته(tb_change_cnt))
	    {
		// "buf" may be invalid now if a client put something in the
		// typeahead buffer and "buf" is in the typeahead buffer.
		typeaheadlen = 0;
		اكسر;
	    }

	    إذا (c == Ctrl_C && ctrl_c_interrupts)
	    {
# إذا defined(FEAT_CLIENTSERVER)
		trash_درون_داد_buf();
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
		    p = utf16_مقصد_enc(ch, &n);
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
		إذا ((modifiers & MOD_ماسک_ALT)
			&& n == 1
			&& (typeahead[typeaheadlen] & 0x80) == 0
			&& !enc_dbcs
		   )
		{
		    n = (*mb_char2bytes)(typeahead[typeaheadlen] | 0x80,
						    typeahead + typeaheadlen);
		    modifiers &= ~MOD_ماسک_ALT;
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
#پایان_اگر // FEAT_GUI_MSWIN && !VIMDLL
}

/*
 * Return TRUE if "name" is an executable file, FALSE if not or it doesn't exist.
 * When returning TRUE and "path" is not NULL save the path and set "*path" to
 * the allocated memory.
 * TODO: Should somehow check if it's really executable.
 */
    ساكن صحيح
executable_file(حرف *name, char_u **مسیر)
{
    صحيح attrs = win32_getattrs((char_u *)name);

    // The file doesn't exist or is a folder.
    إذا (attrs == -1 || (attrs & FILE_ATTRIBUTE_DIRECTORY))
	أرجع FALSE;
    // Check if the file is an AppExecLink, a special alias used by Windows
    // Store for its apps.
    إذا (attrs & FILE_ATTRIBUTE_REPARSE_نقطه)
    {
	char_u	*res = resolve_appexeclink((char_u *)name);
	إذا (res == عدم)
	    res = resolve_reparse_نقطه((char_u *)name);
	إذا (res == عدم)
	    أرجع FALSE;
	// The path is already absolute.
	إذا (مسیر != عدم)
	    *مسیر = res;
	وإلا
	    vim_free(res);
    }
    وإلا إذا (مسیر != عدم)
	*مسیر = پر_Name_save((char_u *)name, FALSE);
    أرجع TRUE;
}

/*
 * Check for a matching prefix; "name" includes the trailing wildcard.
 * An inconclusive search (e.g. a directory which cannot be enumerated) must
 * not skip the normal checks.
 */
    ساكن bool
executable_پیشوند_exists(char_u *name)
{
    WCHAR		*wn;
    WIN32_پیدا_کن_DATAW	داده;
    HANDLE		handle;
    DWORD		error;

    wn = enc_مقصد_utf16(name, عدم);
    إذا (wn == عدم)
	أرجع true;
    handle = پیدا_کن_اولی_File_Ex_W(wn, پیدا_کن_Ex_Info_Basic, &داده,
					    پیدا_کن_Ex_جستجو_Name_تطابق, عدم, 0);
    error = Get_آخرین_Error();
    vim_free(wn);
    إذا (handle != INVALID_HANDLE_مقدار)
    {
	پیدا_کن_Close(handle);
	أرجع true;
    }
    أرجع error != ERROR_FILE_NOT_یافت && error != ERROR_مسیر_NOT_یافت;
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
    char_u	**مسیر,
    صحيح		use_مسیر,
    صحيح		use_pathext)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[MAX_مسیر_2 * 3];
    char_u	*p;
    string_T	pathbuf = {عدم, 0};
    صحيح		pathbuf_allocated = FALSE;
    string_T	pathext = {عدم, 0};
    صحيح		pathext_allocated = FALSE;
    char_u	*shname = عدم;
    صحيح		noext = FALSE;
    bool	simple_pathext = true;
    صحيح		ext_شمار = 0;
    صحيح		retval = FALSE;

    إذا (namelen >= حجم(buf))	// safety check
	أرجع FALSE;

    // Using the name directly when a Unix-shell like 'shell'.
    shname = gettail(p_sh);
    إذا (جستجوی_رشته((حرف *)shname, "sh") != عدم &&
	!(جستجوی_رشته((حرف *)shname, "powershell") != عدم
				    || جستجوی_رشته((حرف *)shname, "pwsh") != عدم))
	noext = TRUE;

    إذا (use_pathext)
    {
	pathext.string = mch_getenv("PATHEXT");
	إذا (pathext.string == عدم)
	    STR_LITERAL_مجموعه(pathext, ".com;.exe;.bat;.cmd");
	وإلا
	    pathext.درازی = STRLEN(pathext.string);

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
		    e = pathext.string + pathext.درازی;
		plen = (size_t)(e - p);
		++ext_شمار;
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
	STR_LITERAL_مجموعه(pathext, ".");
    وإلا إذا (noext == TRUE)
    {
	char_u  *tmp;

	tmp = alloc(pathext.درازی + 3);
	إذا (tmp == عدم)
	{
	    retval = FALSE;
	    اذهب theend;
	}

	STRCPY(tmp, ".;");
	STRCPY(tmp + 2, pathext.string);
	pathext.string = tmp;
	pathext.درازی += 2;
	pathext_allocated = TRUE;
    }

    // Use $PATH when "use_path" is TRUE and "name" is basename.
    إذا (use_مسیر && gettail((char_u *)name) == (char_u *)name)
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
		    جستجوی_رشته((حرف *)gettail(p_sh), "cmd.exe") != عدم)
	    {
		STRCPY(pathbuf.string, ".;");
		pathbuf.درازی = 2;
	    }
	    STRCPY(pathbuf.string + pathbuf.درازی, p);
	    pathbuf.درازی += plen;
	    pathbuf_allocated = TRUE;
	}
    }

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    إذا (pathbuf.string == عدم)
	STR_LITERAL_مجموعه(pathbuf, ".");
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
	    e = pathbuf.string + pathbuf.درازی;

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
	إذا (!noext && simple_pathext && ext_شمار >= 3 && buflen > 0
		&& buflen + 2 <= حجم(buf)
		&& buf[buflen - 1] != '.' && buf[buflen - 1] != ' '
		&& vim_strpbrk(buf, (char_u *)"*?") == عدم
		&& vim_strchr(gettail(buf), ':') == عدم)
	{
	    bool exists;

	    buf[buflen] = '*';
	    buf[buflen + 1] = NUL;
	    exists = executable_پیشوند_exists(buf);
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
		e2 = pathext.string + pathext.درازی;

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
	    إذا (executable_file((حرف *)buf, مسیر))
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
bad_param_handler(ثابت wchar_t *عبارت_2 استفادهنشده,
    ثابت wchar_t *function استفادهنشده,
    ثابت wchar_t *file استفادهنشده,
    بیعلامت صحيح سطر استفادهنشده,
    uintptr_t pReserved استفادهنشده)
{
}

# define مجموعه_INVALID_PARAM_HANDLER \
	((فراغ)مجموعه_invalid_parameter_handler(bad_param_handler))
#وإلا
# define مجموعه_INVALID_PARAM_HANDLER
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_MSWIN

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
    مجموعه_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    مجموعه_Error_نما(SEM_FAILCRITICALERRORS);

    _fmode = O_دودویی;		// we do our own CR-LF translation

    // Specify window size.  Is there a place to get the default from?
    ردیفها = 25;
    Columns = 80;

    // Look for 'vimrun'.
    {
	char_u	vimrun_location[MAX_مسیر_2 + 4];
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
		    vimrun_مسیر.string = p;
		    vimrun_مسیر.درازی = plen;
		    vimrun_مسیر_allocated = TRUE;
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
	مجموعه_option_مقدار_give_err((char_u *)"grepprg",
						    0, (char_u *)"grep -n", 0);

# ifdef FEAT_CLIPBOARD
    win_clip_init();
# endif

    vtp_flag_init();
}


#پایان_اگر // FEAT_GUI_MSWIN

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

# define SRWIDTH(sr) ((sr).راست_4 - (sr).چپ_4 + 1)
# define SRHEIGHT(sr) ((sr).Bottom - (sr).بالاترین_2 + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    ساكن BOOL
Clear_Console_میانگیر(کلمه_2 wAttribute)
{
    CONSOLE_SCREEN_میانگیر_INFO csbi;
    COORD coord;
    DWORD رقم_Cells, dummy;

    إذا (!Get_Console_Screen_میانگیر_Info(g_h_Con_بیرون, &csbi))
	أرجع FALSE;

    رقم_Cells = csbi.dw_اندازه.X * csbi.dw_اندازه.Y;
    coord.X = 0;
    coord.Y = 0;
    إذا (!FillConsoleOutputCharacter(g_h_Con_بیرون, ' ', رقم_Cells,
	    coord, &dummy))
	أرجع FALSE;
    إذا (!FillConsoleOutputAttribute(g_h_Con_بیرون, wAttribute, رقم_Cells,
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
Fit_Console_پنجره(
    COORD dw_میانگیر_اندازه,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_میانگیر_INFO csbi;
    COORD dw_پنجره_اندازه;
    BOOL NeedAdjust = FALSE;

    إذا (!Get_Console_Screen_میانگیر_Info(g_h_Con_بیرون, &csbi))
	أرجع FALSE;

    /*
     * A buffer resize will fail if the current console window does
     * not lie completely within that buffer.  To avoid this, we might
     * have to move and possibly shrink the window.
     */
    إذا (csbi.sr_پنجره.راست_4 >= dw_میانگیر_اندازه.X)
    {
	dw_پنجره_اندازه.X = SRWIDTH(csbi.sr_پنجره);
	إذا (dw_پنجره_اندازه.X > dw_میانگیر_اندازه.X)
	    dw_پنجره_اندازه.X = dw_میانگیر_اندازه.X;
	csbi.sr_پنجره.راست_4 = dw_میانگیر_اندازه.X - 1;
	csbi.sr_پنجره.چپ_4 = dw_میانگیر_اندازه.X - dw_پنجره_اندازه.X;
	NeedAdjust = TRUE;
    }
    إذا (csbi.sr_پنجره.Bottom >= dw_میانگیر_اندازه.Y)
    {
	dw_پنجره_اندازه.Y = SRHEIGHT(csbi.sr_پنجره);
	إذا (dw_پنجره_اندازه.Y > dw_میانگیر_اندازه.Y)
	    dw_پنجره_اندازه.Y = dw_میانگیر_اندازه.Y;
	csbi.sr_پنجره.Bottom = dw_میانگیر_اندازه.Y - 1;
	csbi.sr_پنجره.بالاترین_2 = dw_میانگیر_اندازه.Y - dw_پنجره_اندازه.Y;
	NeedAdjust = TRUE;
    }
    إذا (NeedAdjust && WantAdjust)
    {
	إذا (!مجموعه_Console_پنجره_Info(g_h_Con_بیرون, TRUE, &csbi.sr_پنجره))
	    أرجع FALSE;
    }
    أرجع TRUE;
}

تعريفنوع بنية Console_میانگیر_Struct
{
    BOOL			IsValid;
    CONSOLE_SCREEN_میانگیر_INFO	Info;
    PCHAR_INFO			میانگیر_2;
    COORD			میانگیر_اندازه_2;
    PSMALL_RECT			Regions;
    صحيح				رقم_Regions;
} Console_میانگیر;

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
Save_Console_میانگیر(
    Console_میانگیر *cb)
{
    DWORD رقم_Cells;
    COORD میانگیر_Coord;
    SMALL_RECT ReadRegion;
    کلمه_2 Y, Y_incr;
    صحيح i, numregions;

    إذا (cb == عدم)
	أرجع FALSE;

    إذا (!Get_Console_Screen_میانگیر_Info(g_h_Con_بیرون, &cb->Info))
    {
	cb->IsValid = FALSE;
	أرجع FALSE;
    }
    cb->IsValid = TRUE;

    // VTP uses alternate screen buffer.
    // No need to save buffer contents for restoration.
    إذا (use_alternate_screen_میانگیر)
	أرجع TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    إذا (!cb->IsValid || cb->میانگیر_2 == عدم ||
	    cb->میانگیر_اندازه_2.X != cb->Info.dw_اندازه.X ||
	    cb->میانگیر_اندازه_2.Y != cb->Info.dw_اندازه.Y)
    {
	cb->میانگیر_اندازه_2.X = cb->Info.dw_اندازه.X;
	cb->میانگیر_اندازه_2.Y = cb->Info.dw_اندازه.Y;
	رقم_Cells = cb->میانگیر_اندازه_2.X * cb->میانگیر_اندازه_2.Y;
	vim_free(cb->میانگیر_2);
	cb->میانگیر_2 = ALLOC_MULT(CHAR_INFO, رقم_Cells);
	إذا (cb->میانگیر_2 == عدم)
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
    میانگیر_Coord.X = 0;
    ReadRegion.چپ_4 = 0;
    ReadRegion.راست_4 = cb->Info.dw_اندازه.X - 1;
    Y_incr = 12000 / cb->Info.dw_اندازه.X;

    numregions = (cb->Info.dw_اندازه.Y + Y_incr - 1) / Y_incr;
    إذا (cb->Regions == عدم || numregions != cb->رقم_Regions)
    {
	cb->رقم_Regions = numregions;
	vim_free(cb->Regions);
	cb->Regions = ALLOC_MULT(SMALL_RECT, cb->رقم_Regions);
	إذا (cb->Regions == عدم)
	{
	    VIM_CLEAR(cb->میانگیر_2);
	    أرجع FALSE;
	}
    }

    لكل (i = 0, Y = 0; i < cb->رقم_Regions; i++, Y += Y_incr)
    {
	/*
	 * Read into position (0, Y) in our buffer.
	 */
	میانگیر_Coord.Y = Y;
	/*
	 * Read the region whose top left corner is (0, Y) and whose bottom
	 * right corner is (width - 1, Y + Y_incr - 1).  This should define
	 * a region of size width by Y_incr.  Don't worry if this region is
	 * too large for the remaining buffer; it will be cropped.
	 */
	ReadRegion.بالاترین_2 = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	إذا (!ReadConsoleOutputW(g_h_Con_بیرون,	// output handle
		cb->میانگیر_2,			// our buffer
		cb->میانگیر_اندازه_2,			// dimensions of our buffer
		میانگیر_Coord,			// offset in our buffer
		&ReadRegion))			// region to save
	{
	    VIM_CLEAR(cb->میانگیر_2);
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
Restore_Console_میانگیر(
    Console_میانگیر   *cb,
    BOOL	    RestoreScreen)
{
    COORD میانگیر_Coord;
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
	Clear_Console_میانگیر(cb->Info.w_صفات);

    Fit_Console_پنجره(cb->Info.dw_اندازه, TRUE);
    إذا (!مجموعه_Console_Screen_میانگیر_اندازه(g_h_Con_بیرون, cb->Info.dw_اندازه))
	أرجع FALSE;
    إذا (!مجموعه_Console_متن_Attribute(g_h_Con_بیرون, cb->Info.w_صفات))
	أرجع FALSE;

    إذا (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	أرجع TRUE;
    }

    إذا (!مجموعه_Console_Cursor_Position(g_h_Con_بیرون, cb->Info.dwCursorPosition))
	أرجع FALSE;
    إذا (!مجموعه_Console_پنجره_Info(g_h_Con_بیرون, TRUE, &cb->Info.sr_پنجره))
	أرجع FALSE;

    // VTP uses alternate screen buffer.
    // No need to restore buffer contents.
    إذا (use_alternate_screen_میانگیر)
	أرجع TRUE;

    /*
     * Restore the screen buffer contents.
     */
    إذا (cb->میانگیر_2 != عدم)
    {
	لكل (i = 0; i < cb->رقم_Regions; i++)
	{
	    میانگیر_Coord.X = cb->Regions[i].چپ_4;
	    میانگیر_Coord.Y = cb->Regions[i].بالاترین_2;
	    WriteRegion = cb->Regions[i];
	    إذا (!WriteConsoleOutputW(g_h_Con_بیرون,	// output handle
			cb->میانگیر_2,		// our buffer
			cb->میانگیر_اندازه_2,		// dimensions of our buffer
			میانگیر_Coord,		// offset in our buffer
			&WriteRegion))		// region to restore
		أرجع FALSE;
	}
    }

    أرجع TRUE;
}

# define FEAT_RESTORE_ORIG_SCREEN
# ifdef FEAT_RESTORE_ORIG_SCREEN
ساكن Console_میانگیر g_cbOrig = { 0 };
# endif
ساكن Console_میانگیر g_cbNonTermcap = { 0 };
ساكن Console_میانگیر g_cbTermcap = { 0 };

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
	*phIconSmall = (HICON)Send_پیام(hWnd, WM_GETICON,
					       (WPARAM)ICON_SMALL, (LPARAM)0);
    إذا (phIcon != عدم)
	*phIcon = (HICON)Send_پیام(hWnd, WM_GETICON,
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
مجموعه_Console_Icon(
    HWND    hWnd,
    HICON   hIconSmall,
    HICON   hIcon)
{
    إذا (hWnd == عدم)
	أرجع FALSE;

    إذا (hIconSmall != عدم)
	Send_پیام(hWnd, WM_SETICON,
			    (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
    إذا (hIcon != عدم)
	Send_پیام(hWnd, WM_SETICON,
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
    g_hWnd = Get_Console_پنجره();
    إذا (g_hWnd == عدم)
	أرجع;

    // Save the original console window icon.
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    إذا (g_hOrigIconSmall == عدم || g_hOrigIcon == عدم)
	أرجع;

    // Extract the first icon contained in the Vim executable.
    إذا (
# ifdef FEAT_LIBCALL
	    mch_icon_بارگیری((HANDLE *)&g_hVimIcon) == FAIL ||
# endif
	    g_hVimIcon == عدم)
	g_hVimIcon = استخراج_Icon(عدم, (LPCSTR)exe_name, 0);
    إذا (g_hVimIcon != عدم)
	g_fCanChangeIcon = TRUE;
}

ساكن صحيح g_fWindInitCalled = FALSE;
ساكن صحيح g_f_Termcap_نما = FALSE;
ساكن CONSOLE_CURSOR_INFO g_cci;

/*
 * non-GUI version of mch_init().
 */
    ساكن فراغ
mch_init_c(فراغ)
{
# ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_میانگیر_INFO csbi;
# endif
# ifndef __MINGW32__
    خارجي صحيح _fmode;
# endif

    // Silently handle invalid parameters to CRT functions
    مجموعه_INVALID_PARAM_HANDLER;

    // Let critical errors result in a failure, not in a dialog box.  Required
    // for the timestamp test to work on removed floppies.
    مجموعه_Error_نما(SEM_FAILCRITICALERRORS);

    _fmode = O_دودویی;		// we do our own CR-LF translation
    بیرون_flush();

    // Obtain handles for the standard Console I/O devices
    إذا (read_cmd_fd == 0)
	g_h_Con_درون =  GetStdHandle(STD_درون_داد_HANDLE);
    وإلا
	create_conin();
    g_h_Con_بیرون = GetStdHandle(STD_OUTPUT_HANDLE);

    wt_init();
    vtp_flag_init();
# ifdef FEAT_RESTORE_ORIG_SCREEN
    // Save the initial console buffer for later restoration
    Save_Console_میانگیر(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.w_صفات;
# وإلا
    // Get current text attributes
    Get_Console_Screen_میانگیر_Info(g_h_Con_بیرون, &csbi);
    g_attrCurrent = g_attrDefault = csbi.w_صفات;
# endif
    إذا (cterm_نرمال_fg_color == 0)
	cterm_نرمال_fg_color = (g_attrCurrent & 0xf) + 1;
    إذا (cterm_نرمال_bg_color == 0)
	cterm_نرمال_bg_color = ((g_attrCurrent >> 4) & 0xf) + 1;

    // Fg and Bg color index number at startup
    g_color_شاخص_fg = g_attrDefault & 0xf;
    g_color_شاخص_bg = (g_attrDefault >> 4) & 0xf;

    // set termcap codes to current text attributes
    update_tcap(g_attrCurrent);

    GetConsoleCursorInfo(g_h_Con_بیرون, &g_cci);
    Get_Console_نما(g_h_Con_درون,  &g_cmodein);
    Get_Console_نما(g_h_Con_بیرون, &g_cmodeout);

    SaveConsoleTitleAndIcon();
    /*
     * Set both the small and big icons of the console window to Vim's icon.
     * Note that Vim presently only has one size of icon (32x32), but it
     * automatically gets scaled down to 16x16 when setting the small icon.
     */
    إذا (g_fCanChangeIcon)
	مجموعه_Console_Icon(g_hWnd, g_hVimIcon, g_hVimIcon);

    ui_get_shellsize();

    vtp_init();
    // Switch to a new alternate screen buffer.
    إذا (use_alternate_screen_میانگیر)
	vtp_printf("\033[?1049h");

# ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    إذا (fdDump)
    {
	time_t t;

	زمان(&t);
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
    صحيح f_Termcap_نما = g_f_Termcap_نما;

    exiting = TRUE;

    vtp_exit();

    stoptermcap();
    // Switch back to main screen buffer if TermcapMode was not active.
    إذا (!f_Termcap_نما &&  use_alternate_screen_میانگیر)
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
	    مجموعه_Console_Icon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);

# ifdef MCH_WRITE_DUMP
	إذا (fdDump)
	{
	    time_t t;

	    زمان(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = عدم;
# endif
    }

    مجموعه_Console_Cursor_Info(g_h_Con_بیرون, &g_cci);
    مجموعه_Console_نما(g_h_Con_درون,  g_cmodein | ENABLE_EXTENDED_FLAGS);
    مجموعه_Console_نما(g_h_Con_بیرون, g_cmodeout);

# ifdef DYNAMIC_GETTEXT
    dyn_libintl_انتها();
# endif

    خروج(r);
}
#پایان_اگر // !FEAT_GUI_MSWIN || VIMDLL

    فراغ
mch_init(فراغ)
{
#اگر_تعریف_شده VIMDLL
    إذا (gui.starting)
	mch_init_g();
    وإلا
	mch_init_c();
#وإلاإذا defined(FEAT_GUI_MSWIN)
    mch_init_g();
#وإلا
    mch_init_c();
#پایان_اگر
}

    فراغ
mch_exit(صحيح r)
{
#اگر_تعریف_شده FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#پایان_اگر

#اگر_تعریف_شده VIMDLL
    إذا (vimrun_مسیر_allocated)
	vim_free(vimrun_مسیر.string);

    إذا (gui.درون_use || gui.starting)
	mch_exit_g(r);
    وإلا
	mch_exit_c(r);
#وإلاإذا defined(FEAT_GUI_MSWIN)
    إذا (vimrun_مسیر_allocated)
	vim_free(vimrun_مسیر.string);
    mch_exit_g(r);
#وإلا
    mch_exit_c(r);
#پایان_اگر
}

/*
 * Do we have an interactive window?
 */
    صحيح
mch_check_win(
    صحيح argc استفادهنشده,
    حرف **argv استفادهنشده)
{
    mch_get_exe_name();

#إذا defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    أرجع OK;	    // GUI always has a tty
#وإلا
# ifdef VIMDLL
    إذا (gui.درون_use)
	أرجع OK;
# endif
    إذا (isatty(1))
	أرجع OK;
    أرجع FAIL;
#پایان_اگر
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
    WCHAR   buf[MAX_مسیر_2 + 1];

    إذا (*name == NUL)
	أرجع;

    slash_adjust(name);

    p = enc_مقصد_utf16(name, عدم);
    إذا (p == عدم)
	أرجع;

    إذا (Get_Long_مسیر_Name_W(p, buf, MAX_مسیر_2))
    {
	char_u	*q = utf16_مقصد_enc(buf, عدم);

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
    DWORD wcch = ARRAY_درازی(wszUserName);

    إذا (GetUserNameW(wszUserName, &wcch))
    {
	char_u  *p = utf16_مقصد_enc(wszUserName, عدم);

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
    DWORD wcch = ARRAY_درازی(wszHostName);

    إذا (!GetComputerNameW(wszHostName, &wcch))
	أرجع;

    char_u  *p = utf16_مقصد_enc(wszHostName, عدم);
    إذا (p == عدم)
	أرجع;

    vim_strncpy(s, p, len - 1);
    vim_free(p);
}


/*
 * return process ID
 */
    دراز
mch_get_pid(فراغ)
{
    أرجع (دراز)GetCurrentProcessId();
}

/*
 * return TRUE if process "pid" is still running
 */
    صحيح
mch_process_running(دراز pid)
{
    HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
    DWORD   وضعیت = 0;
    صحيح	    ret = FALSE;

    إذا (hProcess == عدم)
	أرجع FALSE;  // might not have access
    إذا (Get_Exit_کد_Process(hProcess, &وضعیت))
	ret = وضعیت == STILL_فعال;
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
    WCHAR   wbuf[MAX_مسیر_2 + 1];

    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    إذا (GetCurrentDirectoryW(MAX_مسیر_2, wbuf) == 0)
	أرجع FAIL;

    WCHAR   wcbuf[MAX_مسیر_2 + 1];
    char_u  *p = عدم;

    إذا (Get_Long_مسیر_Name_W(wbuf, wcbuf, MAX_مسیر_2) != 0)
    {
	p = utf16_مقصد_enc(wcbuf, عدم);
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
	p = utf16_مقصد_enc(wbuf, عدم);

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
    دراز
mch_getperm(char_u *name)
{
    stat_T	st;

    أرجع (mch_stat((حرف *)name, &st) == 0)
	? (دراز)(بیعلامت کوتاه)st.st_نما
	: -1L;
}


/*
 * Set file permission for "name" to "perm".
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
mch_setperm(char_u *name, دراز perm)
{
    دراز	n;
    WCHAR	*p;

    p = enc_مقصد_utf16(name, عدم);
    إذا (p == عدم)
	أرجع FAIL;

    n = _wchmod(p, perm);
    vim_free(p);
    إذا (n == -1)
	أرجع FAIL;

    win32_مجموعه_archive(name);

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

    p = enc_مقصد_utf16(name, عدم);
    إذا (p == عدم)
	أرجع FALSE;
    attr = Get_File_صفات_W(p);
    إذا (attr == INVALID_FILE_صفات)
	error = Get_آخرین_Error();
    vim_free(p);

    إذا (attr == INVALID_FILE_صفات)
	أرجع error == ERROR_FILE_NOT_یافت || error == ERROR_مسیر_NOT_یافت;
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

    p = enc_مقصد_utf16(name, عدم);
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

    p = enc_مقصد_utf16(name, عدم);
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
						   && info.n_عدد_از_Links > 1;
}

/*
 * Return TRUE if "name" is a symbolic link (or a junction).
 */
    صحيح
mch_is_symbolic_link(char_u *name)
{
    HANDLE		h_پیدا_کن;
    صحيح			res = FALSE;
    DWORD		fileFlags = 0, reparse_برچسب = 0;
    WCHAR		*wn;
    WIN32_پیدا_کن_DATAW	پیدا_کن_داده_W;

    wn = enc_مقصد_utf16(name, عدم);
    إذا (wn == عدم)
	أرجع FALSE;

    h_پیدا_کن = پیدا_کن_اولی_File_W(wn, &پیدا_کن_داده_W);
    vim_free(wn);
    إذا (h_پیدا_کن != INVALID_HANDLE_مقدار)
    {
	fileFlags = پیدا_کن_داده_W.dw_File_صفات;
	reparse_برچسب = پیدا_کن_داده_W.dwReserved0;
	پیدا_کن_Close(h_پیدا_کن);
    }

    إذا ((fileFlags & FILE_ATTRIBUTE_REPARSE_نقطه)
	    && (reparse_برچسب == IO_REPARSE_برچسب_SYMLINK
		|| reparse_برچسب == IO_REPARSE_برچسب_MOUNT_نقطه))
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

    wn = enc_مقصد_utf16(fname, عدم);
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

    إذا (hFile == INVALID_HANDLE_مقدار)
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

    p = enc_مقصد_utf16(name, عدم);
    إذا (p == عدم)
	أرجع INVALID_FILE_صفات;

    attr = Get_File_صفات_W(p);
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

    p = enc_مقصد_utf16(name, عدم);
    إذا (p == عدم)
	أرجع -1;

    res = مجموعه_File_صفات_W(p, attrs);
    vim_free(p);

    أرجع res ? 0 : -1;
}

/*
 * Set archive flag for "name".
 */
    ساكن صحيح
win32_مجموعه_archive(char_u *name)
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
mch_can_exe(char_u *name, char_u **مسیر, صحيح use_مسیر استفادهنشده)
{
    أرجع executable_exists((حرف *)name, STRLEN(name), مسیر, TRUE, TRUE);
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
	أرجع گره_WRITABLE;

    wn = enc_مقصد_utf16(name, عدم);
    إذا (wn == عدم)
	أرجع گره_نرمال;

    hFile = CreateFileW(wn,	    // file name
	    GENERIC_WRITE,	    // access mode
	    0,			    // share mode
	    عدم,		    // security descriptor
	    OPEN_EXISTING,	    // creation disposition
	    0,			    // file attributes
	    عدم);		    // handle to template file
    vim_free(wn);
    إذا (hFile == INVALID_HANDLE_مقدار)
	أرجع گره_نرمال;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    إذا (type == FILE_TYPE_CHAR)
	أرجع گره_WRITABLE;
    إذا (type == FILE_TYPE_DISK)
	أرجع گره_نرمال;
    أرجع گره_OTHER;
}

#اگر_تعریف_شده HAVE_ACL
بنية my_acl
{
    PSECURITY_DESCRIPTOR    pSecurityDescriptor;
    PSID		    pSidOwner;
    PSID		    p_Sid_گروه;
    PACL		    pDacl;
    PACL		    pSacl;
};
#پایان_اگر

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname)
{
#اگر_تعریف_نشده HAVE_ACL
    أرجع (vim_acl_T)عدم;
#وإلا
    بنية my_acl   *p = عدم;
    DWORD   err;

    p = ALLOC_CLEAR_ONE(بنية my_acl);
    إذا (p != عدم)
    {
	WCHAR	*wn;

	wn = enc_مقصد_utf16(fname, عدم);
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
		گروه_SECURITY_INFORMATION |
		DACL_SECURITY_INFORMATION |
		SACL_SECURITY_INFORMATION,
		&p->pSidOwner,		// Ownership information.
		&p->p_Sid_گروه,		// Group membership.
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
#پایان_اگر
}

#اگر_تعریف_شده HAVE_ACL
/*
 * Check if "acl" contains inherited ACE.
 */
    ساكن BOOL
is_acl_inherited(PACL acl)
{
    DWORD   i;
    ACL_اندازه_INFORMATION    acl_info;
    PACCESS_ALLOWED_ACE	    ace;

    acl_info.Ace_شمار = 0;
    GetAclInformation(acl, &acl_info, حجم(acl_info), Acl_اندازه_Information);
    لكل (i = 0; i < acl_info.Ace_شمار; i++)
    {
	GetAce(acl, i, (LPVOID *)&ace);
	إذا (ace->سرآیند_2.AceFlags & INHERITED_ACE)
	    أرجع TRUE;
    }
    أرجع FALSE;
}
#پایان_اگر

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    فراغ
mch_مجموعه_acl(char_u *fname, vim_acl_T acl)
{
#اگر_تعریف_شده HAVE_ACL
    بنية my_acl   *p = (بنية my_acl *)acl;
    SECURITY_INFORMATION    sec_info = 0;
    WCHAR	    *wn;

    إذا (p == عدم)
	أرجع;

    wn = enc_مقصد_utf16(fname, عدم);
    إذا (wn == عدم)
	أرجع;

    // Set security flags
    إذا (p->pSidOwner)
	sec_info |= OWNER_SECURITY_INFORMATION;
    إذا (p->p_Sid_گروه)
	sec_info |= گروه_SECURITY_INFORMATION;
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

    (فراغ)مجموعه_Named_Security_Info_W(
	    wn,			// Abstract filename
	    SE_FILE_OBJECT,	// File Object
	    sec_info,
	    p->pSidOwner,	// Ownership information.
	    p->p_Sid_گروه,	// Group membership.
	    p->pDacl,		// Discretionary information.
	    p->pSacl		// For auditing purposes.
	    );
    vim_free(wn);
#پایان_اگر
}

    فراغ
mch_free_acl(vim_acl_T acl)
{
#اگر_تعریف_شده HAVE_ACL
    بنية my_acl   *p = (بنية my_acl *)acl;

    إذا (p != عدم)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#پایان_اگر
}

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    ساكن BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    درون_داد_رکورد ir;
    DWORD بیرون;

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
	ir.Event.کلید_Event_2.b_کلید_پایین = TRUE;
	ir.Event.کلید_Event_2.w_Repeat_شمار = 1;
	ir.Event.کلید_Event_2.w_Virtual_کلید_کد = VK_CANCEL;
	ir.Event.کلید_Event_2.w_Virtual_پیمایش_کد = 0;
	ir.Event.کلید_Event_2.dw_Control_کلید_State = 0;
	ir.Event.کلید_Event_2.uChar.UnicodeChar = 0;
	Write_Console_درون_داد(g_h_Con_درون, &ir, 1, &بیرون);
	أرجع TRUE;

    // fatal events: shut down gracefully
    حالة CTRL_CLOSE_EVENT:
    حالة CTRL_LOGOFF_EVENT:
    حالة CTRL_SHUTDOWN_EVENT:
	windgoto((صحيح)ردیفها - 1, cmdline_ستون_off);
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
    إذا (gui.درون_use)
	أرجع;
# endif
    Get_Console_نما(g_h_Con_درون, &cmodein);
    Get_Console_نما(g_h_Con_بیرون, &cmodeout);
    إذا (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_سطر_درون_داد | ENABLE_PROCESSED_درون_داد |
		     ENABLE_ECHO_درون_داد);
	إذا (g_f_Mouse_فعال)
	{
	    cmodein |= ENABLE_MOUSE_درون_داد;
	    cmodein &= ~ENABLE_سریع_EDIT_نما;
	}
	وإلا
	{
	    cmodein |= g_cmodein & ENABLE_سریع_EDIT_نما;
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
	cmodein |= (ENABLE_سطر_درون_داد | ENABLE_PROCESSED_درون_داد |
		    ENABLE_ECHO_درون_داد);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    مجموعه_Console_نما(g_h_Con_درون, cmodein | ENABLE_EXTENDED_FLAGS);
    مجموعه_Console_نما(g_h_Con_بیرون, cmodeout);
    مجموعه_Console_Ctrl_Handler(handler_routine, bEnableHandler);

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
    CONSOLE_SCREEN_میانگیر_INFO csbi;

# ifdef VIMDLL
    إذا (gui.درون_use)
	أرجع OK;
# endif
    إذا (!g_f_Termcap_نما && g_cbTermcap.IsValid)
    {
	/*
	 * For some reason, we are trying to get the screen dimensions
	 * even though we are not in termcap mode.  The 'Rows' and 'Columns'
	 * variables are really intended to mean the size of Vim screen
	 * while in termcap mode.
	 */
	ردیفها = g_cbTermcap.Info.dw_اندازه.Y;
	Columns = g_cbTermcap.Info.dw_اندازه.X;
    }
    وإلا إذا (Get_Console_Screen_میانگیر_Info(g_h_Con_بیرون, &csbi))
    {
	ردیفها = csbi.sr_پنجره.Bottom - csbi.sr_پنجره.بالاترین_2 + 1;
	Columns = csbi.sr_پنجره.راست_4 - csbi.sr_پنجره.چپ_4 + 1;
    }
    وإلا
    {
	ردیفها = 25;
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
    إذا (use_alternate_screen_میانگیر)
	أرجع;

    إذا (!مجموعه_Console_Screen_میانگیر_اندازه(hConsole, coordScreen))
    {
# ifdef MCH_WRITE_DUMP
	إذا (fdDump)
	{
	    fprintf(fdDump, "SetConsoleScreenBufferSize failed: %lx\n",
		    Get_آخرین_Error());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Resize console window size to 'srWindowRect'
 */
    ساكن فراغ
Resize_پنجره(
    HANDLE     hConsole,
    SMALL_RECT sr_پنجره_Rect)
{
    إذا (!مجموعه_Console_پنجره_Info(hConsole, TRUE, &sr_پنجره_Rect))
    {
# ifdef MCH_WRITE_DUMP
	إذا (fdDump)
	{
	    fprintf(fdDump, "SetConsoleWindowInfo failed: %lx\n",
		    Get_آخرین_Error());
	    fflush(fdDump);
	}
# endif
    }
}

/*
 * Set a console window to `xSize' * `ySize'
 */
    ساكن فراغ
Resize_Con_Buf_And_پنجره(
    HANDLE  hConsole,
    صحيح	    x_اندازه,
    صحيح	    y_اندازه_2)
{
    CONSOLE_SCREEN_میانگیر_INFO csbi;	// hold current console buffer info
    SMALL_RECT	    sr_پنجره_Rect;	// hold the new console size
    COORD	    coordScreen;
    COORD	    cursor;
    ساكن صحيح	    resized = FALSE;

# ifdef MCH_WRITE_DUMP
    إذا (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", x_اندازه, y_اندازه_2);
	fflush(fdDump);
    }
# endif

    // get the largest size we can size the console window to
    coordScreen = Get_بزرگترین_Console_پنجره_اندازه(hConsole);

    // define the new console window size and scroll position
    sr_پنجره_Rect.چپ_4 = sr_پنجره_Rect.بالاترین_2 = (SHORT) 0;
    sr_پنجره_Rect.راست_4 =  (SHORT) (min(x_اندازه, coordScreen.X) - 1);
    sr_پنجره_Rect.Bottom = (SHORT) (min(y_اندازه_2, coordScreen.Y) - 1);

    إذا (Get_Console_Screen_میانگیر_Info(g_h_Con_بیرون, &csbi))
    {
	صحيح sx, sy;

	sx = csbi.sr_پنجره.راست_4 - csbi.sr_پنجره.چپ_4 + 1;
	sy = csbi.sr_پنجره.Bottom - csbi.sr_پنجره.بالاترین_2 + 1;
	إذا (sy < y_اندازه_2 || sx < x_اندازه)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    إذا (sy < y_اندازه_2)
		coordScreen.Y = y_اندازه_2;
	    وإلا
		coordScreen.Y = sy;
	    إذا (sx < x_اندازه)
		coordScreen.X = x_اندازه;
	    وإلا
		coordScreen.X = sx;
	    مجموعه_Console_Screen_میانگیر_اندازه(hConsole, coordScreen);
	}
    }

    // define the new console buffer size
    coordScreen.X = x_اندازه;
    coordScreen.Y = y_اندازه_2;

    // In the new console call API, only the first time in reverse order
    إذا (!vtp_working || resized)
    {
	Resize_پنجره(hConsole, sr_پنجره_Rect);
	ResizeConBuf(hConsole, coordScreen);
    }
    وإلا
    {
	// Workaround for a Windows 10 bug
	cursor.X = sr_پنجره_Rect.چپ_4;
	cursor.Y = sr_پنجره_Rect.بالاترین_2;
	مجموعه_Console_Cursor_Position(hConsole, cursor);

	ResizeConBuf(hConsole, coordScreen);
	Resize_پنجره(hConsole, sr_پنجره_Rect);
	resized = TRUE;
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    فراغ
mch_مجموعه_shellsize(فراغ)
{
    COORD coordScreen;

# ifdef VIMDLL
    إذا (gui.درون_use)
	أرجع;
# endif
    // Don't change window size while still starting up
    إذا (suppress_winsize != 0)
    {
	suppress_winsize = 2;
	أرجع;
    }

    إذا (جمله_console)
    {
	coordScreen = Get_بزرگترین_Console_پنجره_اندازه(g_h_Con_بیرون);

	// Clamp Rows and Columns to reasonable values
	إذا (ردیفها > coordScreen.Y)
	    ردیفها = coordScreen.Y;
	إذا (Columns > coordScreen.X)
	    Columns = coordScreen.X;

	Resize_Con_Buf_And_پنجره(g_h_Con_بیرون, Columns, ردیفها);
    }
}

/*
 * Rows and/or Columns has changed.
 */
    فراغ
mch_new_shellsize(فراغ)
{
# ifdef VIMDLL
    إذا (gui.درون_use)
	أرجع;
# endif
    مجموعه_scroll_region(0, 0, Columns - 1, ردیفها - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    فراغ
mch_مجموعه_winsize_now(فراغ)
{
    إذا (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_مجموعه_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}

# إذا defined(FEAT_IMAGE) || defined(FEAT_EVAL)
/*
 * Try to get the current terminal cell size in pixels.
 */
    فراغ
mch_calc_cell_اندازه(بنية cellsize *cs_بیرون)
{
    CONSOLE_FONT_INFOEX cfi;
    DWORD cmodeold;
    DWORD cmodein;

    cs_بیرون->cs_xpixel = -1;
    cs_بیرون->cs_ypixel = -1;

    إذا (g_h_Con_بیرون == INVALID_HANDLE_مقدار)
	أرجع;

    cfi.cb_اندازه = حجم(cfi);
    إذا (GetCurrentConsoleFontEx(g_h_Con_بیرون, FALSE, &cfi)
	    && cfi.dw_Font_اندازه.X > 0 && cfi.dw_Font_اندازه.Y > 0)
    {
	cs_بیرون->cs_xpixel = cfi.dw_Font_اندازه.X;
	cs_بیرون->cs_ypixel = cfi.dw_Font_اندازه.Y;
	أرجع;
    }

    Get_Console_نما(g_h_Con_درون, &cmodein);
    cmodeold = cmodein;
    cmodein &= ~(ENABLE_سطر_درون_داد | ENABLE_ECHO_درون_داد
	    | ENABLE_PROCESSED_درون_داد);
    cmodein |= ENABLE_VIRTUAL_TERMINAL_درون_داد;

    إذا (مجموعه_Console_نما(g_h_Con_درون, cmodein))
    {
	// We will receive the window dimensions later
	vtp_printf("\033[14t");
	مجموعه_Console_نما(g_h_Con_درون, cmodeold);
    }
}
# endif

#پایان_اگر

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

    wcmd = enc_مقصد_utf16((char_u *)cmd, عدم);
    إذا (wcmd == عدم)
	أرجع FALSE;
    إذا (cwd != عدم)
    {
	wcwd = enc_مقصد_utf16((char_u *)cwd, عدم);
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

    wcmd = enc_مقصد_utf16((char_u *)cmd, عدم);
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
	si.w_Show_پنجره = SW_SHOWMINNOACTIVE;
    وإلا
	si.w_Show_پنجره = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = عدم;

    // Now, run the command
    vim_create_process(cmd, FALSE,
	    CREATE_DEFAULT_ERROR_نما |	CREATE_NEW_CONSOLE,
	    &si, &pi, عدم, عدم);

    // Wait for the command to terminate before continuing
    {
	صحيح	    delay = 1;

	// Keep updating the window while waiting for the shell to finish.
	لكل (;;)
	{
	    پیام_3	پیام;

	    إذا (Peek_پیام_W(&پیام, (HWND)عدم, 0, 0, PM_REMOVE))
	    {
		Translate_پیام(&پیام);
		Dispatch_پیام_W(&پیام);
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
	Get_Exit_کد_Process(pi.hProcess, &ret);
    }

    // Close the handles to the subprocess, so that it goes away
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // Try to get input focus back.  Doesn't always work though.
    Post_پیام(hwnd, WM_SETFOCUS, 0, 0);
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
    ساكن بیعلامت صحيح __stdcall
sub_process_writer(LPVOID param)
{
    HANDLE	    g_h_فرزند_Std_درون_Wr = param;
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
	    WriteFile(g_h_فرزند_Std_درون_Wr, "", 1, &len, عدم);
	}
	وإلا
	{
	    s = vim_strchr(lp + written, NL);
	    WriteFile(g_h_فرزند_Std_درون_Wr, (حرف *)lp + written,
		      s == عدم ? l : (DWORD)(s - (lp + written)),
		      &len, عدم);
	}
	إذا (len == l)
	{
	    // Finished a line, add a NL, unless this line should not have
	    // one.
	    إذا (lnum != curbuf->b_op_انتها.lnum
		|| (!curbuf->b_p_bin
		    && curbuf->b_p_fixeol)
		|| (lnum != curbuf->b_no_eol_lnum
		    && (lnum != curbuf->b_ml.ml_سطر_شمار
			|| curbuf->b_p_eol)))
	    {
		WriteFile(g_h_فرزند_Std_درون_Wr, "\n", 1,
						  (LPDWORD)&vim_ignored, عدم);
	    }

	    ++lnum;
	    إذا (lnum > curbuf->b_op_انتها.lnum)
		اكسر;

	    lp = ml_get(lnum);
	    written = 0;
	}
	وإلا إذا (len > 0)
	    written += len;
    }

    // finished all the lines, close pipe
    CloseHandle(g_h_فرزند_Std_درون_Wr);
    أرجع 0;
}


# define BUFLEN 100	// length for buffer, stolen from unix version

/*
 * This function read from the children's stdout and write the
 * data on screen or in the buffer accordingly.
 */
    ساكن فراغ
dump_pipe(صحيح	    options,
	  HANDLE    g_h_فرزند_Std_بیرون_Rd,
	  garray_T  *ga,
	  char_u    میانگیر[],
	  DWORD	    *میانگیر_off)
{
    DWORD	موجود_بایتها = 0;
    DWORD	i;
    صحيح		ret;
    DWORD	len;
    DWORD	مقصد_Read;

    // we query the pipe to see if there is any data to read
    // to avoid to perform a blocking read
    ret = PeekNamedPipe(g_h_فرزند_Std_بیرون_Rd, // pipe to query
			عدم,		    // optional buffer
			0,		    // buffer size
			عدم,		    // number of read bytes
			&موجود_بایتها,    // available bytes total
			عدم);		    // byteLeft

    // We got real data in the pipe, read it
    طالما (ret != 0 && موجود_بایتها > 0)
    {
	مقصد_Read = (DWORD)(BUFLEN - *میانگیر_off);
	مقصد_Read = موجود_بایتها < مقصد_Read ? موجود_بایتها : مقصد_Read;
	ReadFile(g_h_فرزند_Std_بیرون_Rd, میانگیر + *میانگیر_off, مقصد_Read , &len, عدم);

	// If we haven't read anything, there is a problem
	إذا (len == 0)
	    اكسر;

	موجود_بایتها -= len;

	إذا (options & SHELL_READ)
	{
	    // Do NUL -> NL translation, append NL separated
	    // lines to the current buffer.
	    لكل (i = 0; i < len; ++i)
	    {
		إذا (میانگیر[i] == NL)
		    append_ga_سطر(ga);
		وإلا إذا (میانگیر[i] == NUL)
		    ga_append(ga, NL);
		وإلا
		    ga_append(ga, میانگیر[i]);
	    }
	}
	وإلا إذا (has_mbyte)
	{
	    صحيح		l;
	    صحيح		c;
	    char_u	*p;

	    len += *میانگیر_off;
	    میانگیر[len] = NUL;

	    // Check if the last character in buffer[] is
	    // incomplete, keep these bytes for the next
	    // round.
	    لكل (p = میانگیر; p < میانگیر + len; p += l)
	    {
		l = MB_CPTR2LEN(p);
		إذا (l == 0)
		    l = 1;  // NUL byte?
		وإلا إذا (MB_BYTE2LEN(*p) != l)
		    اكسر;
	    }
	    إذا (p == میانگیر)	// no complete character
	    {
		// avoid getting stuck at an illegal byte
		إذا (len >= 12)
		    ++p;
		وإلا
		{
		    *میانگیر_off = len;
		    أرجع;
		}
	    }
	    c = *p;
	    *p = NUL;
	    پیام_puts((حرف *)میانگیر);
	    إذا (p < میانگیر + len)
	    {
		*p = c;
		*میانگیر_off = (DWORD)((میانگیر + len) - p);
		mch_memmove(میانگیر, p, *میانگیر_off);
		أرجع;
	    }
	    *میانگیر_off = 0;
	}
	وإلا
	{
	    میانگیر[len] = NUL;
	    پیام_puts((حرف *)میانگیر);
	}

	windgoto(پیام_ردیف, cmdline_ستون_off + پیام_ستون);
	cursor_on();
	بیرون_flush();
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

    HANDLE g_h_فرزند_Std_درون_Rd = عدم;
    HANDLE g_h_فرزند_Std_درون_Wr = عدم;
    HANDLE g_h_فرزند_Std_بیرون_Rd = عدم;
    HANDLE g_h_فرزند_Std_بیرون_Wr = عدم;

    char_u	میانگیر[BUFLEN + 1]; // reading buffer + size
    DWORD	len;

    // buffer used to receive keys
    char_u	ta_buf[BUFLEN + 1];	// TypeAHead
    صحيح		ta_len = 0;		// valid bytes in ta_buf[]

    DWORD	i;
    صحيح		noread_cnt = 0;
    garray_T	ga;
    صحيح		delay = 1;
    DWORD	میانگیر_off = 0;	// valid bytes in buffer[]
    حرف	*p = عدم;

    SECURITY_صفات saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.n_درازی = حجم(SECURITY_صفات);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = عدم;

    إذا ( ! CreatePipe(&g_h_فرزند_Std_بیرون_Rd, &g_h_فرزند_Std_بیرون_Wr, &saAttr, 0)
	// Ensure the read handle to the pipe for STDOUT is not inherited.
       || ! مجموعه_Handle_Information(g_h_فرزند_Std_بیرون_Rd, HANDLE_FLAG_INHERIT, 0)
	// Create a pipe for the child process's STDIN.
       || ! CreatePipe(&g_h_فرزند_Std_درون_Rd, &g_h_فرزند_Std_درون_Wr, &saAttr, 0)
	// Ensure the write handle to the pipe for STDIN is not inherited.
       || ! مجموعه_Handle_Information(g_h_فرزند_Std_درون_Wr, HANDLE_FLAG_INHERIT, 0) )
    {
	CloseHandle(g_h_فرزند_Std_درون_Rd);
	CloseHandle(g_h_فرزند_Std_درون_Wr);
	CloseHandle(g_h_فرزند_Std_بیرون_Rd);
	CloseHandle(g_h_فرزند_Std_بیرون_Wr);
	پیام_puts(_("\nCannot create pipes\n"));
    }

    si.cb = حجم(si);
    si.lpReserved = عدم;
    si.lpDesktop = عدم;
    si.lpTitle = عدم;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // set-up our file redirection
    si.hStdError = g_h_فرزند_Std_بیرون_Wr;
    si.hStdOutput = g_h_فرزند_Std_بیرون_Wr;
    si.h_Std_درون_داد = g_h_فرزند_Std_درون_Rd;
    si.w_Show_پنجره = SW_HIDE;
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
    vim_create_process(p, TRUE, CREATE_DEFAULT_ERROR_نما,
						&si, &pi, عدم, عدم);

    إذا (p != cmd)
	vim_free(p);

    // Close our unused side of the pipes
    CloseHandle(g_h_فرزند_Std_درون_Rd);
    CloseHandle(g_h_فرزند_Std_بیرون_Wr);

    إذا (options & SHELL_WRITE)
    {
	HANDLE thread = (HANDLE)
	 _beginthreadex(عدم,  // security attributes
			0,     // default stack size
			sub_process_writer, // function to be executed
			g_h_فرزند_Std_درون_Wr,  // parameter
			0,		 // creation flag, start immediately
			عدم);		    // we don't care about thread id
	CloseHandle(thread);
	g_h_فرزند_Std_درون_Wr = عدم;
    }

    // Keep updating the window while waiting for the shell to finish.
    لكل (;;)
    {
	پیام_3	پیام;

	إذا (Peek_پیام_W(&پیام, (HWND)عدم, 0, 0, PM_REMOVE))
	{
	    Translate_پیام(&پیام);
	    Dispatch_پیام_W(&پیام);
	}

	// write pipe information in the window
	إذا ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| gui.درون_use
# endif
	    )
	{
	    len = 0;
	    إذا (((options &
			(SHELL_READ|SHELL_WRITE|SHELL_COOKED))
		    != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
		    || gui.درون_use
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
			    CloseHandle(g_h_فرزند_Std_درون_Wr);
			    g_h_فرزند_Std_درون_Wr = عدم;
			    len = 0;
			}
		    }

		    len = جمله_replace_keycodes(ta_buf, ta_len, len);

		    /*
		     * For pipes: echo the typed characters.  For a pty this
		     * does not seem to work.
		     */
		    لكل (i = ta_len; i < ta_len + len; ++i)
		    {
			إذا (ta_buf[i] == '\n' || ta_buf[i] == '\b')
			    پیام_putchar(ta_buf[i]);
			وإلا إذا (has_mbyte)
			{
			    صحيح l = (*mb_ptr2len)(ta_buf + i);

			    پیام_outtrans_len(ta_buf + i, l);
			    i += l - 1;
			}
			وإلا
			    پیام_outtrans_len(ta_buf + i, 1);
		    }
		    windgoto(پیام_ردیف, cmdline_ستون_off + پیام_ستون);
		    بیرون_flush();

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
		    وإلا إذا (g_h_فرزند_Std_درون_Wr != عدم)
		    {
			WriteFile(g_h_فرزند_Std_درون_Wr, (حرف*)ta_buf,
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
	    dump_pipe(options, g_h_فرزند_Std_بیرون_Rd, &ga, میانگیر, &میانگیر_off);
	    اكسر;
	}

	++noread_cnt;
	dump_pipe(options, g_h_فرزند_Std_بیرون_Rd, &ga, میانگیر, &میانگیر_off);

	// We start waiting for a very short time and then increase it, so
	// that we respond quickly when the process is quick, and don't
	// consume too much overhead when it's slow.
	إذا (delay < 50)
	    delay += 10;
    }

    // Close the pipe
    CloseHandle(g_h_فرزند_Std_بیرون_Rd);
    إذا (g_h_فرزند_Std_درون_Wr != عدم)
	CloseHandle(g_h_فرزند_Std_درون_Wr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the command exit code
    Get_Exit_کد_Process(pi.hProcess, &ret);

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
#پایان_اگر

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ساكن صحيح
mch_system_c(حرف *cmd, صحيح options استفادهنشده)
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
	wcmd = enc_مقصد_utf16(buf, عدم);
	حرر(buf);
    }
    وإلا
	wcmd = enc_مقصد_utf16((char_u *)cmd, عدم);

    إذا (wcmd == عدم)
	أرجع -1;

    ret = _wsystem(wcmd);
    vim_free(wcmd);
    أرجع ret;
}

#پایان_اگر

    ساكن صحيح
mch_system(حرف *cmd, صحيح options)
{
#اگر_تعریف_شده VIMDLL
    إذا (gui.درون_use || gui.starting)
	أرجع mch_system_g(cmd, options);
    وإلا
	أرجع mch_system_c(cmd, options);
#وإلاإذا defined(FEAT_GUI_MSWIN)
    أرجع mch_system_g(cmd, options);
#وإلا
    أرجع mch_system_c(cmd, options);
#پایان_اگر
}

#إذا defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ساكن صحيح
mch_call_shell_terminal(
    char_u	*cmd,
    صحيح		options استفادهنشده)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    char_u	*newcmd = عدم;
    typval_T	argvar[2];
    long_u	cmdlen;
    صحيح		retval = -1;
    buf_T	*buf;
    کار_T	*کار;
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

    init_کار_options(&opt);

    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = newcmd;
    argvar[1].v_type = VAR_UNKNOWN;
    buf = جمله_شروع(argvar, عدم, &opt, جمله_شروع_SYSTEM);
    إذا (buf == عدم)
    {
	vim_free(newcmd);
	أرجع 255;
    }

    کار = جمله_getjob(buf->b_جمله);
    ++کار->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    إذا (curbuf == buf)
    {
	// Only do this when a window was found for "buf".
	clear_oparg(&oa);
	طالما (جمله_use_loop())
	{
	    إذا (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_فعال)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		إذا (terminal_loop(TRUE) == OK)
		    نرمال_cmd(&oa, TRUE);
	    }
	    وإلا
		نرمال_cmd(&oa, TRUE);
	}
	retval = کار->jv_exitval;
	ch_log(عدم, "system command finished");

	کار_unref(کار);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    wait_return(TRUE);
    do_میانگیر(DOBUF_WIPE, DOBUF_اولی, FORWARD, buf->b_fnum, TRUE);

    vim_free(newcmd);
    أرجع retval;
}
#پایان_اگر
/* Restore a previous environment variable value, or unset it if NULL.
 * 'must_free' indicates whether 'old_value' was allocated.
 */
    ساكن فراغ
restore_env_var(char_u *name, char_u *old_مقدار, صحيح must_free)
{
    إذا (old_مقدار != عدم)
    {
	vim_setenv(name, old_مقدار);
	إذا (must_free)
	    vim_free(old_مقدار);
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

#اگر_تعریف_شده FEAT_EVAL
    ch_log(عدم, "executing shell command: %s", cmd);
#پایان_اگر
    // Change the title to reflect that we are in a subshell.
    إذا (GetConsoleTitleW(szShellTitle, ARRAY_درازی(szShellTitle) - 4) > 0)
    {
	إذا (cmd == عدم)
	    wcscat(szShellTitle, L" :sh");
	وإلا
	{
	    WCHAR *wn = enc_مقصد_utf16((char_u *)cmd, عدم);

	    إذا (wn != عدم)
	    {
		wcscat(szShellTitle, L" - !");
		إذا ((wcslen(szShellTitle) + wcslen(wn) <
			    ARRAY_درازی(szShellTitle)))
		    wcscat(szShellTitle, wn);
		مجموعه_Console_Title_W(szShellTitle);
		vim_free(wn);
	    }
	}
    }
    // do not execute anything from the current directory by setting the
    // environment variable $NoDefaultCurrentDirectoryInExePath
    oldval = vim_getenv((char_u *)"NoDefaultCurrentDirectoryInExePath",
	    &must_free);
    vim_setenv((char_u *)"NoDefaultCurrentDirectoryInExePath", (char_u *)"1");

    بیرون_flush();

#اگر_تعریف_شده MCH_WRITE_DUMP
    إذا (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#پایان_اگر
#إذا defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    // TODO: make the terminal window work with input or output redirected.
    إذا (
# ifdef VIMDLL
	    gui.درون_use &&
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
#پایان_اگر

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

	    Zero_حافظه(&si, حجم(si));
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
		si.w_Show_پنجره = SW_SHOWMINNOACTIVE;
		n_show_cmd = SW_SHOWMINNOACTIVE;
	    }
	    وإلا إذا ((STRNICMP(cmdbase, "/b", 2) == 0)
		    && VIM_ISWHITE(cmdbase[2]))
	    {
		cmdbase = skipwhite(cmdbase + 2);
		flags = CREATE_NO_پنجره;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.h_Std_درون_داد = CreateFile("\\\\.\\NUL",	// File name
		    GENERIC_READ,			// Access flags
		    0,					// Share flags
		    عدم,				// Security att.
		    OPEN_EXISTING,			// Open flags
		    FILE_ATTRIBUTE_نرمال,		// File att.
		    عدم);				// Temp file
		si.hStdOutput = si.h_Std_درون_داد;
		si.hStdError = si.h_Std_درون_داد;
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
#اگر_تعریف_شده FEAT_GUI_MSWIN
# ifdef VIMDLL
		إذا (gui.درون_use)
# endif
		    emsg(_(e_command_not_یافت));
#پایان_اگر
	    }

	    إذا (newcmd != cmdbase)
		vim_free(newcmd);

	    إذا (si.dwFlags == STARTF_USESTDHANDLES && si.h_Std_درون_داد != عدم)
	    {
		// Close the handle to \\.\NUL created above.
		CloseHandle(si.h_Std_درون_داد);
	    }
	    // Close the handles to the subprocess, so that it goes away
	    CloseHandle(pi.hThread);
	    CloseHandle(pi.hProcess);
	}
	وإلا
	{
	    cmdlen =
#اگر_تعریف_شده FEAT_GUI_MSWIN
		((gui.درون_use || gui.starting) ?
		    (!s_dont_use_vimrun && p_stmp ?
			vimrun_مسیر.درازی : STRLEN(p_sh) + STRLEN(p_shcf))
		    : 0) +
#پایان_اگر
		STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;

	    newcmd = alloc(cmdlen);
	    إذا (newcmd != عدم)
	    {
#إذا defined(FEAT_GUI_MSWIN)
		إذا (
# ifdef VIMDLL
		    (gui.درون_use || gui.starting) &&
# endif
		    need_vimrun_warning)
		{
		    حرف *پیام = _("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information.");
		    حرف *title = _("Vim Warning");
		    WCHAR *wmsg = enc_مقصد_utf16((char_u *)پیام, عدم);
		    WCHAR *wtitle = enc_مقصد_utf16((char_u *)title, عدم);

		    إذا (wmsg != عدم && wtitle != عدم)
			پیام_Box_W(عدم, wmsg, wtitle, MB_ICONWARNING);
		    vim_free(wmsg);
		    vim_free(wtitle);
		    need_vimrun_warning = FALSE;
		}
		إذا (
# ifdef VIMDLL
		    (gui.درون_use || gui.starting) &&
# endif
		    !s_dont_use_vimrun && p_stmp)
		    // Use vimrun to execute the command.  It opens a console
		    // window, which can be closed without killing Vim.
		    vim_snprintf((حرف *)newcmd, cmdlen, "%s%s%s %s %s",
			    vimrun_مسیر.string,
			    (پیام_silent != 0 || (options & SHELL_DOOUT))
								 ? "-s " : "",
			    p_sh, p_shcf, cmd);
		وإلا إذا (
# ifdef VIMDLL
			(gui.درون_use || gui.starting) &&
# endif
			s_dont_use_vimrun && STRCMP(p_shcf, "/c") == 0)
		    // workaround for the case that "vimrun" does not exist
		    vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s %s %s",
					   p_sh, p_shcf, p_sh, p_shcf, cmd);
		وإلا
#پایان_اگر
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
	    && ((gui.درون_use || gui.starting) ?
		((options & SHELL_DOOUT) || s_dont_use_vimrun || !p_stmp) : 1)
#پایان_اگر
	    )
    {
	smsg(_("shell returned %d"), x);
	پیام_putchar('\n');
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

#إذا defined(FEAT_کار_CHANNEL)
    ساكن HANDLE
کار_io_file_open(
	char_u *fname,
	DWORD dwDesiredAccess,
	DWORD dw_Share_نما,
	LPSECURITY_صفات lp_Security_صفات,
	DWORD dwCreationDisposition,
	DWORD dw_Flags_And_صفات)
{
    HANDLE h;
    WCHAR *wn;

    wn = enc_مقصد_utf16(fname, عدم);
    إذا (wn == عدم)
	أرجع INVALID_HANDLE_مقدار;

    h = CreateFileW(wn, dwDesiredAccess, dw_Share_نما,
	    lp_Security_صفات, dwCreationDisposition,
	    dw_Flags_And_صفات, عدم);
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
    long_u	todo = env != عدم ? env->dv_hashtab.ht_استفاده_شده : 0;
    LPVOID	پایه = Get_Environment_رشتهها_W();

    // for last \0
    إذا (ga_grow(gap, 1) == FAIL)
	أرجع;

    إذا (env != عدم)
    {
	FOR_ALL_HASHTAB_اقلام(&env->dv_hashtab, hi, todo)
	{
	    إذا (!HASHITEM_خالی(hi))
	    {
		typval_T *قلم = &dict_lookup(hi)->di_tv;
		WCHAR   *wkey = enc_مقصد_utf16((char_u *)hi->hi_کلید, عدم);
		WCHAR   *wval = enc_مقصد_utf16(tv_get_string(قلم), عدم);
		--todo;
		إذا (wkey != عدم && wval != عدم)
		{
		    size_t	n;
		    size_t	lkey = wcslen(wkey);
		    size_t	lval = wcslen(wval);

		    إذا (ga_grow(gap, (صحيح)(lkey + lval + 2)) == FAIL)
			استمر;
		    لكل (n = 0; n < lkey; n++)
			*((WCHAR*)gap->ga_داده + gap->ga_len++) = wkey[n];
		    *((WCHAR*)gap->ga_داده + gap->ga_len++) = L'=';
		    لكل (n = 0; n < lval; n++)
			*((WCHAR*)gap->ga_داده + gap->ga_len++) = wval[n];
		    *((WCHAR*)gap->ga_داده + gap->ga_len++) = L'\0';
		}
		vim_free(wkey);
		vim_free(wval);
	    }
	}
    }

    إذا (پایه)
    {
	WCHAR	*p = (WCHAR*) پایه;

	// for last \0
	إذا (ga_grow(gap, 1) == FAIL)
	    أرجع;

	طالما (*p != 0 || *(p + 1) != 0)
	{
	    إذا (ga_grow(gap, 1) == OK)
		*((WCHAR*)gap->ga_داده + gap->ga_len++) = *p;
	    p++;
	}
	Free_Environment_رشتهها_W(پایه);
	*((WCHAR*)gap->ga_داده + gap->ga_len++) = L'\0';
    }

# إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_TERMINAL)
    {
#  ifdef FEAT_CLIENTSERVER
	char_u	*servername = get_vim_var_str(VV_SEND_SERVER);
	size_t	servername_len = STRLEN(servername);
#  endif
#  ifdef FEAT_TERMINAL
	char_u	*نسخه_2 = get_vim_var_str(VV_نسخه);
	size_t	نسخه_len = STRLEN(نسخه_2);
#  endif
	// size of "VIM_SERVERNAME=" and value,
	// plus "VIM_TERMINAL=" and value,
	// plus two terminating NULs
	size_t	n = 0
#  ifdef FEAT_CLIENTSERVER
		    + 15 + servername_len
#  endif
#  ifdef FEAT_TERMINAL
		    + 13 + نسخه_len + 2
#  endif
		    ;

	إذا (ga_grow(gap, (صحيح)n) == OK)
	{
#  ifdef FEAT_CLIENTSERVER
	    لكل (n = 0; n < 15; n++)
		*((WCHAR*)gap->ga_داده + gap->ga_len++) =
		    (WCHAR)"VIM_SERVERNAME="[n];
	    لكل (n = 0; n < servername_len; n++)
		*((WCHAR*)gap->ga_داده + gap->ga_len++) =
		    (WCHAR)servername[n];
	    *((WCHAR*)gap->ga_داده + gap->ga_len++) = L'\0';
#  endif
#  ifdef FEAT_TERMINAL
	    إذا (is_terminal)
	    {
		لكل (n = 0; n < 13; n++)
		    *((WCHAR*)gap->ga_داده + gap->ga_len++) =
			(WCHAR)"VIM_TERMINAL="[n];
		لكل (n = 0; n < نسخه_len; n++)
		    *((WCHAR*)gap->ga_داده + gap->ga_len++) =
			(WCHAR)نسخه_2[n];
		*((WCHAR*)gap->ga_داده + gap->ga_len++) = L'\0';
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
    SECURITY_صفات sa;

    sprintf(name, "\\\\?\\pipe\\vim-%08lx-%08lx",
	    GetCurrentProcessId(),
	    InterlockedIncrement(&s));

    // Create named pipe. Max size of named pipe is 65535.
    handles[1] = CreateNamedPipe(
	    name,
	    PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
	    PIPE_TYPE_BYTE | PIPE_NOWAIT,
	    1, MAX_NAMED_PIPE_اندازه, 0, 0, عدم);

    إذا (handles[1] == INVALID_HANDLE_مقدار)
	أرجع FALSE;

    sa.n_درازی = حجم(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = عدم;

    handles[0] = CreateFile(name,
	    FILE_GENERIC_READ,
	    FILE_SHARE_READ, &sa,
	    OPEN_EXISTING, FILE_ATTRIBUTE_نرمال, 0);

    إذا (handles[0] == INVALID_HANDLE_مقدار)
    {
	CloseHandle(handles[1]);
	أرجع FALSE;
    }

    أرجع TRUE;
}

#پایان_اگر // FEAT_JOB_CHANNEL

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
mch_get_cmd_output_مستقیم(
    حرف	**argv,
    char_u	*infile,
    صحيح		flags استفادهنشده,
    صحيح		*ret_len)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    SECURITY_صفات saAttr;
    HANDLE		h_فرزند_Stdout_Rd = INVALID_HANDLE_مقدار;
    HANDLE		h_فرزند_Stdout_Wr = INVALID_HANDLE_مقدار;
    HANDLE		h_فرزند_Stdin_Rd = INVALID_HANDLE_مقدار;
    garray_T		cmd_ga;
    garray_T		بیرون_ga;
    char_u		*میانگیر = عدم;
    DWORD		exit_کد = (DWORD)-1;
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
	    صحيح	رقم_bs;

	    ga_append(&cmd_ga, '"');
	    لكل (j = 0; arg[j] != NUL; j++)
	    {
		رقم_bs = 0;
		طالما (arg[j] == '\\')
		{
		    رقم_bs++;
		    j++;
		}

		إذا (arg[j] == NUL)
		{
		    // Backslashes before closing quote must be doubled.
		    طالما (رقم_bs-- > 0)
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
		    طالما (رقم_bs-- > 0)
		    {
			ga_append(&cmd_ga, '\\');
			ga_append(&cmd_ga, '\\');
		    }
		    ga_append(&cmd_ga, '\\');
		    ga_append(&cmd_ga, '"');
		}
		وإلا
		{
		    طالما (رقم_bs-- > 0)
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

    ga_init2(&بیرون_ga, 1, 4096);

    saAttr.n_درازی = حجم(SECURITY_صفات);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = عدم;

    // Create a pipe for the child's stdout.
    إذا (!CreatePipe(&h_فرزند_Stdout_Rd, &h_فرزند_Stdout_Wr, &saAttr, 0)
	    || !مجموعه_Handle_Information(h_فرزند_Stdout_Rd, HANDLE_FLAG_INHERIT, 0))
    {
	emsg(_(e_cannot_create_pipes));
	اذهب انجامشده;
    }

    // Set up stdin from infile if provided.
    إذا (infile != عدم)
    {
	WCHAR *winfile = enc_مقصد_utf16(infile, عدم);

	إذا (winfile != عدم)
	{
	    h_فرزند_Stdin_Rd = CreateFileW(winfile, GENERIC_READ,
		    FILE_SHARE_READ, &saAttr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_نرمال, عدم);
	    vim_free(winfile);
	}
    }

    Zero_حافظه(&pi, حجم(pi));
    Zero_حافظه(&si, حجم(si));
    si.cb = حجم(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.w_Show_پنجره = SW_HIDE;
    si.hStdOutput = h_فرزند_Stdout_Wr;
    si.hStdError = h_فرزند_Stdout_Wr;
    si.h_Std_درون_داد = (h_فرزند_Stdin_Rd != INVALID_HANDLE_مقدار)
		    ? h_فرزند_Stdin_Rd : INVALID_HANDLE_مقدار;

    ch_log(عدم, "directly executing: %s", (حرف *)cmd_ga.ga_داده);

    // Create the child process directly, without going through the shell.
    إذا (!vim_create_process((حرف *)cmd_ga.ga_داده, TRUE,
		CREATE_DEFAULT_ERROR_نما | CREATE_NEW_PROCESS_گروه,
		&si, &pi, عدم, عدم))
    {
	semsg(_(e_invalid_آرگومان_str), cmd_ga.ga_داده);
	اذهب انجامشده;
    }

    // Close the write end of stdout pipe and stdin in the parent so that
    // ReadFile() will get EOF when the child process exits.
    CloseHandle(h_فرزند_Stdout_Wr);
    h_فرزند_Stdout_Wr = INVALID_HANDLE_مقدار;
    إذا (h_فرزند_Stdin_Rd != INVALID_HANDLE_مقدار)
    {
	CloseHandle(h_فرزند_Stdin_Rd);
	h_فرزند_Stdin_Rd = INVALID_HANDLE_مقدار;
    }

    // Read output from child process.
    لكل (;;)
    {
	حرف	buf[4096];
	DWORD	n;

	إذا (!ReadFile(h_فرزند_Stdout_Rd, buf, حجم(buf), &n, عدم) || n == 0)
	    اكسر;
	إذا (ga_grow(&بیرون_ga, (صحيح)n) == OK)
	{
	    mch_memmove((حرف *)بیرون_ga.ga_داده + بیرون_ga.ga_len, buf, n);
	    بیرون_ga.ga_len += (صحيح)n;
	}
    }

    // Wait for child to finish and get exit code.
    WaitForSingleObject(pi.hProcess, INFINITE);
    Get_Exit_کد_Process(pi.hProcess, &exit_کد);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    مجموعه_vim_var_nr(VV_SHELL_ERROR, (دراز)exit_کد);

    إذا (بیرون_ga.ga_len > 0)
    {
	میانگیر = alloc(بیرون_ga.ga_len + 1);
	إذا (میانگیر != عدم)
	{
	    mch_memmove(میانگیر, بیرون_ga.ga_داده, بیرون_ga.ga_len);
	    إذا (ret_len == عدم)
	    {
		// Change NUL into SOH, otherwise the string is truncated.
		لكل (i = 0; i < بیرون_ga.ga_len; ++i)
		    إذا (میانگیر[i] == NUL)
			میانگیر[i] = 1;
		میانگیر[بیرون_ga.ga_len] = NUL;
	    }
	    وإلا
		*ret_len = بیرون_ga.ga_len;
	}
    }
    وإلا إذا (ret_len != عدم)
	*ret_len = 0;

انجامشده:
    ga_clear(&cmd_ga);
    ga_clear(&بیرون_ga);
    إذا (h_فرزند_Stdout_Rd != INVALID_HANDLE_مقدار)
	CloseHandle(h_فرزند_Stdout_Rd);
    إذا (h_فرزند_Stdout_Wr != INVALID_HANDLE_مقدار)
	CloseHandle(h_فرزند_Stdout_Wr);
    إذا (h_فرزند_Stdin_Rd != INVALID_HANDLE_مقدار)
	CloseHandle(h_فرزند_Stdin_Rd);
    أرجع میانگیر;
}
#پایان_اگر // FEAT_EVAL

#إذا defined(FEAT_کار_CHANNEL)
    فراغ
mch_کار_شروع(حرف *cmd, کار_T *کار, jobopt_T *options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION	pi;
    HANDLE		jo;
    SECURITY_صفات saAttr;
    channel_T		*channel = عدم;
    HANDLE		ifd[2];
    HANDLE		ofd[2];
    HANDLE		efd[2];
    garray_T		ga;

    صحيح		use_null_for_درون = options->jo_io[PART_درون] == JIO_NULL;
    صحيح		use_null_for_بیرون = options->jo_io[PART_بیرون] == JIO_NULL;
    صحيح		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    صحيح		use_file_for_درون = options->jo_io[PART_درون] == JIO_FILE;
    صحيح		use_file_for_بیرون = options->jo_io[PART_بیرون] == JIO_FILE;
    صحيح		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    صحيح		use_بیرون_for_err = options->jo_io[PART_ERR] == JIO_بیرون;

    إذا (use_بیرون_for_err && use_null_for_بیرون)
	use_null_for_err = TRUE;

    ifd[0] = INVALID_HANDLE_مقدار;
    ifd[1] = INVALID_HANDLE_مقدار;
    ofd[0] = INVALID_HANDLE_مقدار;
    ofd[1] = INVALID_HANDLE_مقدار;
    efd[0] = INVALID_HANDLE_مقدار;
    efd[1] = INVALID_HANDLE_مقدار;
    ga_init2(&ga, حجم(wchar_t), 500);

    jo = Create_کار_Object(عدم, عدم);
    إذا (jo == عدم)
    {
	کار->jv_وضعیت = کار_FAILED;
	اذهب failed;
    }

    إذا (options->jo_env != عدم)
	win32_build_env(options->jo_env, &ga, FALSE);

    Zero_حافظه(&pi, حجم(pi));
    Zero_حافظه(&si, حجم(si));
    si.cb = حجم(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.w_Show_پنجره = SW_HIDE;

    saAttr.n_درازی = حجم(SECURITY_صفات);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = عدم;

    إذا (use_file_for_درون)
    {
	char_u *fname = options->jo_io_name[PART_درون];

	ifd[0] = کار_io_file_open(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_نرمال);
	إذا (ifd[0] == INVALID_HANDLE_مقدار)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    وإلا إذا (!use_null_for_درون
	    && (!create_pipe_pair(ifd)
		|| !مجموعه_Handle_Information(ifd[1], HANDLE_FLAG_INHERIT, 0)))
	اذهب failed;

    إذا (use_file_for_بیرون)
    {
	char_u *fname = options->jo_io_name[PART_بیرون];

	ofd[1] = کار_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_نرمال);
	إذا (ofd[1] == INVALID_HANDLE_مقدار)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    وإلا إذا (!use_null_for_بیرون &&
	    (!CreatePipe(&ofd[0], &ofd[1], &saAttr, 0)
	    || !مجموعه_Handle_Information(ofd[0], HANDLE_FLAG_INHERIT, 0)))
	اذهب failed;

    إذا (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	efd[1] = کار_io_file_open(fname, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_نرمال);
	إذا (efd[1] == INVALID_HANDLE_مقدار)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    وإلا إذا (!use_بیرون_for_err && !use_null_for_err &&
	    (!CreatePipe(&efd[0], &efd[1], &saAttr, 0)
	    || !مجموعه_Handle_Information(efd[0], HANDLE_FLAG_INHERIT, 0)))
	اذهب failed;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.h_Std_درون_داد = ifd[0];
    si.hStdOutput = ofd[1];
    si.hStdError = use_بیرون_for_err ? ofd[1] : efd[1];

    إذا (!use_null_for_درون || !use_null_for_بیرون || !use_null_for_err)
    {
	إذا (options->jo_مجموعه & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    إذا (channel != عدم)
		++channel->ch_refcount;
	}
	وإلا
	    channel = جمع_کن_channel();
	إذا (channel == عدم)
	    اذهب failed;
    }

    إذا (!vim_create_process(cmd, TRUE,
	    CREATE_SUSPENDED |
	    CREATE_DEFAULT_ERROR_نما |
	    CREATE_NEW_PROCESS_گروه |
	    CREATE_UNICODE_ENVIRONMENT |
	    CREATE_NEW_CONSOLE,
	    &si, &pi,
	    ga.ga_داده,
	    (حرف *)options->jo_cwd))
    {
	CloseHandle(jo);
	کار->jv_وضعیت = کار_FAILED;
	اذهب failed;
    }

    ga_clear(&ga);

    إذا (!تخصیص_بده_Process_مقصد_کار_Object(jo, pi.hProcess))
    {
	// if failing, switch the way to terminate
	// process with TerminateProcess.
	CloseHandle(jo);
	jo = عدم;
    }
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    کار->jv_proc_info = pi;
    کار->jv_کار_object = jo;
    کار->jv_وضعیت = کار_STARTED;

    CloseHandle(ifd[0]);
    CloseHandle(ofd[1]);
    إذا (!use_بیرون_for_err && !use_null_for_err)
	CloseHandle(efd[1]);

    کار->jv_channel = channel;
    إذا (channel != عدم)
    {
	channel_مجموعه_pipes(channel,
		      use_file_for_درون || use_null_for_درون
					      ? INVALID_FD : (sock_T)ifd[1],
		      use_file_for_بیرون || use_null_for_بیرون
					     ? INVALID_FD : (sock_T)ofd[0],
		      use_بیرون_for_err || use_file_for_err || use_null_for_err
					    ? INVALID_FD : (sock_T)efd[0]);
	channel_مجموعه_کار(channel, کار, options);
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
mch_کار_وضعیت(کار_T *کار)
{
    DWORD dw_Exit_کد = 0;

    إذا (!Get_Exit_کد_Process(کار->jv_proc_info.hProcess, &dw_Exit_کد)
	    || dw_Exit_کد != STILL_فعال)
    {
	کار->jv_exitval = (صحيح)dw_Exit_کد;
	إذا (کار->jv_وضعیت < کار_ENDED)
	{
	    ch_log(کار->jv_channel, "Job ended");
	    کار->jv_وضعیت = کار_ENDED;
	}
	أرجع "dead";
    }
    أرجع "run";
}

    کار_T *
mch_detect_ended_کار(کار_T *کار_list)
{
    HANDLE کار_Handles[بیشینه_WAIT_OBJECTS];
    کار_T *کار_Array[بیشینه_WAIT_OBJECTS];
    کار_T *کار = کار_list;

    طالما (کار != عدم)
    {
	DWORD n;
	DWORD نتیجه;

	لكل (n = 0; n < بیشینه_WAIT_OBJECTS
				       && کار != عدم; کار = کار->jv_بعدی)
	{
	    إذا (کار->jv_وضعیت == کار_STARTED)
	    {
		کار_Handles[n] = کار->jv_proc_info.hProcess;
		کار_Array[n] = کار;
		++n;
	    }
	}
	إذا (n == 0)
	    استمر;
	نتیجه = Wait_For_مضرب_Objects(n, کار_Handles, FALSE, 0);
	إذا (نتیجه >= WAIT_OBJECT_0 && نتیجه < WAIT_OBJECT_0 + n)
	{
	    کار_T *wait_کار = کار_Array[نتیجه - WAIT_OBJECT_0];

	    إذا (STRCMP(mch_کار_وضعیت(wait_کار), "dead") == 0)
		أرجع wait_کار;
	}
    }
    أرجع عدم;
}

    ساكن BOOL
terminate_all(HANDLE process, صحيح کد)
{
    PROCESSENTRY32  pe;
    HANDLE	    h = INVALID_HANDLE_مقدار;
    DWORD	    pid = GetProcessId(process);

    إذا (pid != 0)
    {
	h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	إذا (h != INVALID_HANDLE_مقدار)
	{
	    pe.dw_اندازه = حجم(PROCESSENTRY32);
	    إذا (!Process32_اولی(h, &pe))
		اذهب theend;

	    افعل
	    {
		إذا (pe.th32_والد_Process_ID == pid)
		{
		    HANDLE ph = OpenProcess(
				  PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		    إذا (ph != عدم)
		    {
			terminate_all(ph, کد);
			CloseHandle(ph);
		    }
		}
	    } طالما (Process32_بعدی(h, &pe));

	    CloseHandle(h);
	}
    }

theend:
    أرجع TerminateProcess(process, کد);
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if it didn't work.
 */
    صحيح
mch_signal_کار(کار_T *کار, char_u *how)
{
    صحيح ret;

    إذا (STRCMP(how, "term") == 0 || STRCMP(how, "kill") == 0 || *how == NUL)
    {
	// deadly signal
	إذا (کار->jv_کار_object != عدم)
	{
	    إذا (کار->jv_channel != عدم && کار->jv_channel->ch_anonymous_pipe)
		کار->jv_channel->ch_killing = TRUE;
	    أرجع Terminate_کار_Object(کار->jv_کار_object, (UINT)-1) ? OK : FAIL;
	}
	أرجع terminate_all(کار->jv_proc_info.hProcess, -1) ? OK : FAIL;
    }

    إذا (!AttachConsole(کار->jv_proc_info.dwProcessId))
	أرجع FAIL;
    ret = GenerateConsoleCtrlEvent(
		STRCMP(how, "int") == 0 ? CTRL_C_EVENT : CTRL_BREAK_EVENT,
		کار->jv_proc_info.dwProcessId)
	    ? OK : FAIL;
    FreeConsole();
    أرجع ret;
}

/*
 * Clear the data related to "job".
 */
    فراغ
mch_clear_کار(کار_T *کار)
{
    إذا (کار->jv_وضعیت == کار_FAILED)
	أرجع;

    إذا (کار->jv_کار_object != عدم)
	CloseHandle(کار->jv_کار_object);
    CloseHandle(کار->jv_proc_info.hProcess);
}
#پایان_اگر


#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

/*
 * Start termcap mode
 */
    ساكن فراغ
termcap_نما_شروع(فراغ)
{
    DWORD cmodein;

    إذا (g_f_Termcap_نما)
	أرجع;

    Save_Console_میانگیر(&g_cbNonTermcap);

    إذا (g_cbTermcap.IsValid)
    {
	/*
	 * We've been in termcap mode before.  Restore certain screen
	 * characteristics, including the buffer size and the window
	 * size.  Since we will be redrawing the screen, we don't need
	 * to restore the actual contents of the buffer.
	 */
	Restore_Console_میانگیر(&g_cbTermcap, FALSE);
	reset_console_color_rgb();
	مجموعه_Console_پنجره_Info(g_h_Con_بیرون, TRUE, &g_cbTermcap.Info.sr_پنجره);
	ردیفها = g_cbTermcap.Info.dw_اندازه.Y;
	Columns = g_cbTermcap.Info.dw_اندازه.X;
    }
    وإلا
    {
	/*
	 * This is our first time entering termcap mode.  Clear the console
	 * screen buffer, and resize the buffer to match the current window
	 * size.  We will use this as the size of our editing environment.
	 */
	Clear_Console_میانگیر(g_attrCurrent);
	مجموعه_console_color_rgb();
	Resize_Con_Buf_And_پنجره(g_h_Con_بیرون, Columns, ردیفها);
    }

    resettitle();

    Get_Console_نما(g_h_Con_درون, &cmodein);
    إذا (g_f_Mouse_فعال)
    {
	cmodein |= ENABLE_MOUSE_درون_داد;
	cmodein &= ~ENABLE_سریع_EDIT_نما;
    }
    وإلا
    {
	cmodein &= ~ENABLE_MOUSE_درون_داد;
	cmodein |= g_cmodein & ENABLE_سریع_EDIT_نما;
    }
    cmodein |= ENABLE_پنجره_درون_داد;
    مجموعه_Console_نما(g_h_Con_درون, cmodein | ENABLE_EXTENDED_FLAGS);

    redraw_later_clear();
    g_f_Termcap_نما = TRUE;
}


/*
 * End termcap mode
 */
    ساكن فراغ
termcap_نما_انتها(فراغ)
{
    DWORD cmodein;
    Console_میانگیر *cb;
    COORD coord;
    DWORD dwDummy;

    إذا (!g_f_Termcap_نما)
	أرجع;

    Save_Console_میانگیر(&g_cbTermcap);

    Get_Console_نما(g_h_Con_درون, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_درون_داد | ENABLE_پنجره_درون_داد);
    cmodein |= g_cmodein & ENABLE_سریع_EDIT_نما;
    مجموعه_Console_نما(g_h_Con_درون, cmodein | ENABLE_EXTENDED_FLAGS);

# ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
# وإلا
    cb = &g_cbNonTermcap;
# endif
    Restore_Console_میانگیر(cb, p_rs);
    restore_console_color_rgb();

    // Switch back to main screen buffer.
    إذا (exiting && use_alternate_screen_میانگیر)
	vtp_printf("\033[?1049l");

    إذا (!USE_WT && (p_rs || exiting))
    {
	/*
	 * Clear anything that happens to be on the current line.
	 */
	coord.X = 0;
	coord.Y = (SHORT) (p_rs ? cb->Info.dwCursorPosition.Y : (ردیفها - 1));
	FillConsoleOutputCharacter(g_h_Con_بیرون, ' ',
		cb->Info.dw_اندازه.X, coord, &dwDummy);
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
	مجموعه_Console_Cursor_Position(g_h_Con_بیرون, coord);
    }
    مجموعه_Console_Cursor_Info(g_h_Con_بیرون, &g_cci);
    g_f_Termcap_نما = FALSE;
}
#پایان_اگر


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

	FillConsoleOutputCharacter(g_h_Con_بیرون, ' ', n, coord, &dwDummy);
	FillConsoleOutputAttribute(g_h_Con_بیرون, g_attrCurrent, n, coord,
								     &dwDummy);
    }
    وإلا
    {
	مجموعه_console_color_rgb();
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
	clear_chars(g_coord, ردیفها * Columns);
    وإلا
    {
	مجموعه_console_color_rgb();
	gotoxy(1, 1);
	vtp_printf("\033[2J");
    }
}


/*
 * Clear to end of display
 */
    ساكن فراغ
clear_مقصد_انتها_از_display(فراغ)
{
    COORD save = g_coord;

    إذا (!vtp_working)
	clear_chars(g_coord, (ردیفها - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
    وإلا
    {
	مجموعه_console_color_rgb();
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
clear_مقصد_انتها_از_سطر(فراغ)
{
    COORD save = g_coord;

    إذا (!vtp_working)
	clear_chars(g_coord, Columns - g_coord.X);
    وإلا
    {
	مجموعه_console_color_rgb();
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
scroll(بیعلامت c_سطرها)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.چپ_4 + 1, g_srScrollRegion.بالاترین_2 + 1);
    delete_سطرها(c_سطرها);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    ساكن فراغ
مجموعه_scroll_region(
    بیعلامت چپ_2,
    بیعلامت بالاترین,
    بیعلامت راست_2,
    بیعلامت bottom)
{
    إذا (چپ_2 >= راست_2
	    || بالاترین >= bottom
	    || راست_2 > (بیعلامت) Columns - 1
	    || bottom > (بیعلامت) ردیفها - 1)
	أرجع;

    g_srScrollRegion.چپ_4 =   چپ_2;
    g_srScrollRegion.بالاترین_2 =    بالاترین;
    g_srScrollRegion.راست_4 =  راست_2;
    g_srScrollRegion.Bottom = bottom;
}

    ساكن فراغ
مجموعه_scroll_region_tb(
    بیعلامت بالاترین,
    بیعلامت bottom)
{
    إذا (بالاترین >= bottom || bottom > (بیعلامت)ردیفها - 1)
	أرجع;

    g_srScrollRegion.بالاترین_2 = بالاترین;
    g_srScrollRegion.Bottom = bottom;
}

    ساكن فراغ
مجموعه_scroll_region_lr(
    بیعلامت چپ_2,
    بیعلامت راست_2)
{
    إذا (چپ_2 >= راست_2 || راست_2 > (بیعلامت)Columns - 1)
	أرجع;

    g_srScrollRegion.چپ_4 = چپ_2;
    g_srScrollRegion.راست_4 = راست_2;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    ساكن فراغ
درج_سطرها(بیعلامت c_سطرها)
{
    SMALL_RECT	    منبع, clip;
    COORD	    dest;
    CHAR_INFO	    fill;

    gotoxy(g_srScrollRegion.چپ_4 + 1, g_srScrollRegion.بالاترین_2 + 1);

    dest.X = g_srScrollRegion.چپ_4;
    dest.Y = g_coord.Y + c_سطرها;

    منبع.چپ_4   = g_srScrollRegion.چپ_4;
    منبع.بالاترین_2	  = g_coord.Y;
    منبع.راست_4  = g_srScrollRegion.راست_4;
    منبع.Bottom = g_srScrollRegion.Bottom - c_سطرها;

    clip.چپ_4   = g_srScrollRegion.چپ_4;
    clip.بالاترین_2    = g_coord.Y;
    clip.راست_4  = g_srScrollRegion.راست_4;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    إذا (!USE_VTP)
	fill.صفات_2 = g_attrCurrent;
    وإلا
	fill.صفات_2 = g_attrDefault;

    مجموعه_console_color_rgb();

    Scroll_Console_Screen_میانگیر(g_h_Con_بیرون, &منبع, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake: If the scroll
    // region looks like abc and we scroll c to a and fill with d we get
    // cbd... if we scroll block c one line at a time to a, we get cdd...
    // vim expects cdd consistently... So we have to deal with that
    // here... (this also occurs scrolling the same way in the other
    // direction).

    إذا (منبع.Bottom < dest.Y)
    {
	COORD coord;
	صحيح   i;

	coord.X = منبع.چپ_4;
	لكل (i = clip.بالاترین_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, منبع.راست_4 - منبع.چپ_4 + 1);
	}
    }

    إذا (vtp_working)
    {
	COORD coord;
	صحيح i;

	coord.X = منبع.چپ_4;
	لكل (i = منبع.بالاترین_2; i < dest.Y; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, منبع.راست_4 - منبع.چپ_4 + 1);
	}
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    ساكن فراغ
delete_سطرها(بیعلامت c_سطرها)
{
    SMALL_RECT	    منبع, clip;
    COORD	    dest;
    CHAR_INFO	    fill;
    صحيح		    nb;

    gotoxy(g_srScrollRegion.چپ_4 + 1, g_srScrollRegion.بالاترین_2 + 1);

    dest.X = g_srScrollRegion.چپ_4;
    dest.Y = g_coord.Y;

    منبع.چپ_4   = g_srScrollRegion.چپ_4;
    منبع.بالاترین_2	  = g_coord.Y + c_سطرها;
    منبع.راست_4  = g_srScrollRegion.راست_4;
    منبع.Bottom = g_srScrollRegion.Bottom;

    clip.چپ_4   = g_srScrollRegion.چپ_4;
    clip.بالاترین_2    = g_coord.Y;
    clip.راست_4  = g_srScrollRegion.راست_4;
    clip.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    إذا (!vtp_working)
	fill.صفات_2 = g_attrCurrent;
    وإلا
	fill.صفات_2 = g_attrDefault;

    مجموعه_console_color_rgb();

    Scroll_Console_Screen_میانگیر(g_h_Con_بیرون, &منبع, &clip, dest, &fill);

    // Here we have to deal with a win32 console flake; See insert_lines()
    // above.

    nb = dest.Y + (منبع.Bottom - منبع.بالاترین_2) + 1;

    إذا (nb < منبع.بالاترین_2)
    {
	COORD coord;
	صحيح   i;

	coord.X = منبع.چپ_4;
	لكل (i = nb; i < clip.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, منبع.راست_4 - منبع.چپ_4 + 1);
	}
    }

    إذا (vtp_working)
    {
	COORD coord;
	صحيح i;

	coord.X = منبع.چپ_4;
	لكل (i = nb; i <= منبع.Bottom; ++i)
	{
	    coord.Y = i;
	    clear_chars(coord, منبع.راست_4 - منبع.چپ_4 + 1);
	}
    }
}


/*
 * Set the cursor position to (x,y) (1-based).
 */
    ساكن فراغ
gotoxy(
    بیعلامت x,
    بیعلامت y)
{
    إذا (x < 1 || x > (بیعلامت)Columns || y < 1 || y > (بیعلامت)ردیفها)
	أرجع;

    إذا (!USE_VTP)
    {
	// There are reports of double-width characters not displayed
	// correctly.  This workaround should fix it, similar to how it's done
	// for VTP.
	g_coord.X = 0;
	مجموعه_Console_Cursor_Position(g_h_Con_بیرون, g_coord);

	// external cursor coords are 1-based; internal are 0-based
	g_coord.X = x - 1;
	g_coord.Y = y - 1;
	مجموعه_Console_Cursor_Position(g_h_Con_بیرون, g_coord);
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
textattr(کلمه_2 wAttr)
{
    g_attrCurrent = wAttr & 0xff;

    مجموعه_Console_متن_Attribute(g_h_Con_بیرون, wAttr);
}


    ساكن فراغ
textcolor(کلمه_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + (wAttr & 0x0f);

    إذا (!vtp_working)
	مجموعه_Console_متن_Attribute(g_h_Con_بیرون, g_attrCurrent);
    وإلا
	vtp_sgr_bulk(wAttr);
}


    ساكن فراغ
textbackground(کلمه_2 wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + ((wAttr & 0x0f) << 4);

    إذا (!vtp_working)
	مجموعه_Console_متن_Attribute(g_h_Con_بیرون, g_attrCurrent);
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


ساكن کلمه_2 g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    ساكن فراغ
standout(فراغ)
{
    g_attrPreStandout = g_attrCurrent;

    textattr((کلمه_2) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
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
mch_مجموعه_نرمال_colors(فراغ)
{
    char_u	*p;
    صحيح		n;

    cterm_نرمال_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_نرمال_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
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
	    cterm_نرمال_fg_color = (n & 0xf) + 1;
	    cterm_نرمال_bg_color = ((n >> 4) & 0xf) + 1;
	}
    }
# ifdef FEAT_TERMGUICOLORS
    cterm_نرمال_fg_gui_color = INVALCOLOR;
    cterm_نرمال_bg_gui_color = INVALCOLOR;
# endif
}


/*
 * visual bell: flash the screen
 */
    ساكن فراغ
visual_bell(فراغ)
{
    COORD   coordOrigin = {0, 0};
    کلمه_2    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = عدم;

# ifdef FEAT_TERMGUICOLORS
    إذا (!(p_tgc || t_colors >= 256))
# endif
    {
	oldattrs = ALLOC_MULT(کلمه_2, ردیفها * Columns);
	إذا (oldattrs == عدم)
	    أرجع;
	ReadConsoleOutputAttribute(g_h_Con_بیرون, oldattrs, ردیفها * Columns,
			       coordOrigin, &dwDummy);
    }

    FillConsoleOutputAttribute(g_h_Con_بیرون, attrFlash, ردیفها * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    // wait for 15 msec

    إذا (oldattrs != عدم)
    {
	WriteConsoleOutputAttribute(g_h_Con_بیرون, oldattrs, ردیفها * Columns,
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
    DWORD  cb_مقصد_Write)
{
    COORD	    coord = g_coord;
    DWORD	    written;
    DWORD	    n, cchwritten;
    ساكن DWORD    cells;
    ساكن WCHAR    *unicodebuf = عدم;
    ساكن صحيح	    unibuflen = 0;
    ساكن صحيح	    درازی;
    صحيح		    cp = enc_utf8 ? CP_UTF8 : enc_codepage;
    ساكن WCHAR    *utf8spbuf = عدم;
    ساكن صحيح	    utf8splength;
    ساكن DWORD    utf8spcells;
    ساكن WCHAR    **utf8usingbuf = &unicodebuf;

    إذا (cb_مقصد_Write != 1 || *pchBuf != ' ' || !enc_utf8)
    {
	utf8usingbuf = &unicodebuf;
	افعل
	{
	    درازی = Multi_Byte_مقصد_Wide_Char(cp, 0, (LPCSTR)pchBuf, cb_مقصد_Write,
							unicodebuf, unibuflen);
	    إذا (درازی && درازی <= unibuflen)
		اكسر;
	    vim_free(unicodebuf);
	    unicodebuf = درازی ? LALLOC_MULT(WCHAR, درازی) : عدم;
	    unibuflen = unibuflen ? 0 : درازی;
	} طالما (TRUE);
	cells = mb_string2cells(pchBuf, cb_مقصد_Write);
    }
    وإلا // cbToWrite == 1 && *pchBuf == ' ' && enc_utf8
    {
	إذا (utf8usingbuf != &utf8spbuf)
	{
	    إذا (utf8spbuf == عدم)
	    {
		cells = mb_string2cells((char_u *)" ", 1);
		درازی = Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, " ", 1, عدم, 0);
		utf8spbuf = LALLOC_MULT(WCHAR, درازی);
		إذا (utf8spbuf != عدم)
		{
		    Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, " ", 1, utf8spbuf, درازی);
		    utf8usingbuf = &utf8spbuf;
		    utf8splength = درازی;
		    utf8spcells = cells;
		}
	    }
	    وإلا
	    {
		utf8usingbuf = &utf8spbuf;
		درازی = utf8splength;
		cells = utf8spcells;
	    }
	}
    }

    إذا (!USE_VTP)
    {
	FillConsoleOutputAttribute(g_h_Con_بیرون, g_attrCurrent, cells,
				    coord, &written);
	// When writing fails or didn't write a single character, pretend one
	// character was written, otherwise we get stuck.
	إذا (WriteConsoleOutputCharacterW(g_h_Con_بیرون, *utf8usingbuf, درازی,
		    coord, &cchwritten) == 0
		|| cchwritten == 0 || cchwritten == (DWORD)-1)
	    cchwritten = 1;
    }
    وإلا
    {
	إذا (WriteConsoleW(g_h_Con_بیرون, *utf8usingbuf, درازی, &cchwritten,
		    عدم) == 0 || cchwritten == 0)
	    cchwritten = 1;
    }

    إذا (cchwritten == (DWORD)درازی)
    {
	written = cb_مقصد_Write;
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

    طالما (g_coord.X > g_srScrollRegion.راست_4)
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
    صحيح *آرگومانها,
    صحيح *شمار,
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
	آرگومانها[argc] = getdigits(&p);
	argc += (argc < 15) ? 1 : 0;
    } طالما (*p == ';');
    *شمار = argc;

    أرجع p;
}

    ساكن char_u *
get_sgr(
    صحيح *آرگومانها,
    صحيح *شمار,
    char_u *سر)
{
    char_u *p = get_seq(آرگومانها, شمار, سر);

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
    صحيح آرگومانها[16];
    char_u *p = get_sgr(آرگومانها, &argc, سر);

    أرجع p && argc == 5 && آرگومانها[0] == n && آرگومانها[1] == 2 ? p : عدم;
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
    صحيح آرگومانها[16];
    char_u *p = get_sgr(آرگومانها, &argc, سر);

    أرجع p && argc == 1 && آرگومانها[0] == n && (p = skipwhite(p)) && *p == '\033'
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

ساكن bool درون_dcs = false;	// inside a DCS (sixel) sequence
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

    wlen = Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, عدم, 0);
    إذا (wlen <= 0)
	أرجع;
    ws = ALLOC_MULT(WCHAR, wlen);
    إذا (ws == عدم)
	أرجع;
    Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, (LPCSTR)s, len, ws, wlen);
    WriteConsoleW(g_h_Con_بیرون, ws, wlen, &written, عدم);
    vim_free(ws);
}

/*
 * Return the pointer just after the ST (ESC '\') that ends a DCS sequence.
 * Returns "end" when the ST is not in this chunk; "in_dcs" then stays set so
 * that the next mch_write() call continues the scan.
 */
    ساكن char_u *
skip_dcs(char_u *s, char_u *انتها)
{
    لكل (char_u *p = s; p < انتها; ++p)
    {
	إذا (dcs_esc && *p == '\\')
	{
	    dcs_esc = false;
	    درون_dcs = false;
	    أرجع p + 1;
	}
	dcs_esc = *p == ESC;
    }
    أرجع انتها;
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
    char_u  *انتها = s + len;

# ifdef VIMDLL
    إذا (gui.درون_use)
	أرجع;
# endif

    إذا (!جمله_console)
    {
	اكتب(1, s, (بیعلامت)len);
	أرجع;
    }

    // translate ESC | sequences into faked bios calls
    طالما (len--)
    {
	صحيح پیشوند = -1;
	char_u ch;

	// While processing a sequence, on rare occasions it seems that another
	// sequence may be inserted asynchronously.
	إذا (len < 0)
	{
	    redraw_all_later(UPD_CLEAR);
	    أرجع;
	}

	إذا (درون_dcs)
	{
	    // Continuation of a DCS sequence split over several writes.
	    صحيح	l = (صحيح)(skip_dcs(s, انتها) - s);

	    إذا (vtp_working)
		write_vt(s, l);
	    len -= l - 1;
	    s += l;
	    استمر;
	}

	طالما (s + ++پیشوند < انتها)
	{
	    ch = s[پیشوند];
	    إذا (ch <= 0x1e && !(ch != '\n' && ch != '\r' && ch != '\b'
						&& ch != '\a' && ch != '\033'))
		اكسر;
	}

	إذا (p_wd)
	{
	    WaitForChar(p_wd, FALSE);
	    إذا (پیشوند != 0)
		پیشوند = 1;
	}

	إذا (پیشوند != 0)
	{
	    DWORD nWritten;

	    nWritten = write_chars(s, پیشوند);
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
		gotoxy(g_srScrollRegion.چپ_4 + 1, g_srScrollRegion.Bottom + 1);
	    }
	    وإلا
	    {
		gotoxy(g_srScrollRegion.چپ_4 + 1, g_coord.Y + 2);
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
	    gotoxy(g_srScrollRegion.چپ_4+1, g_coord.Y + 1);
# ifdef MCH_WRITE_DUMP
	    إذا (fdDump)
		fputs("\\r\n", fdDump);
# endif
	    s++;
	}
	وإلا إذا (s[0] == '\b')
	{
	    // \b, backspace: move cursor one position left
	    إذا (g_coord.X > g_srScrollRegion.چپ_4)
		g_coord.X--;
	    وإلا إذا (g_coord.Y > g_srScrollRegion.بالاترین_2)
	    {
		g_coord.X = g_srScrollRegion.راست_4;
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
	    پیام_Beep(0xFFFFFFFF);
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
	    صحيح	    arg1 = 0, arg2 = 0, argc = 0, آرگومانها[16];
	    char_u  *sp;

	    بدل (s[2])
	    {
	    حالة '0': حالة '1': حالة '2': حالة '3': حالة '4':
	    حالة '5': حالة '6': حالة '7': حالة '8': حالة '9':
		إذا (*(p = get_seq(آرگومانها, &argc, s)) != 'm')
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

		p = get_seq(آرگومانها, &argc, p);

notsgr:
		arg1 = آرگومانها[0];
		arg2 = آرگومانها[1];
		إذا (*p == 'm')
		{
		    إذا (argc == 1 && آرگومانها[0] == 0)
			normvideo();
		    وإلا إذا (argc == 1)
		    {
			إذا (USE_VTP)
			    textcolor((کلمه_2)arg1);
			وإلا
			    textattr((کلمه_2)arg1);
		    }
		    وإلا إذا (vtp_working)
			vtp_sgr_bulks(argc, آرگومانها);
		}
		وإلا إذا (argc == 2 && *p == 'H')
		{
		    gotoxy(arg2, arg1);
		}
		وإلا إذا (argc == 2 && *p == 'r')
		{
		    مجموعه_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		وإلا إذا (argc == 2 && *p == 'R')
		{
		    مجموعه_scroll_region_tb(arg1, arg2);
		}
		وإلا إذا (argc == 2 && *p == 'V')
		{
		    مجموعه_scroll_region_lr(arg1, arg2);
		}
		وإلا إذا (argc == 1 && *p == 'A')
		{
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.بالاترین_2, g_coord.Y - arg1) + 1);
		}
		وإلا إذا (argc == 1 && *p == 'b')
		{
		    textbackground((کلمه_2) arg1);
		}
		وإلا إذا (argc == 1 && *p == 'C')
		{
		    gotoxy(min(g_srScrollRegion.راست_4, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		وإلا إذا (argc == 1 && *p == 'f')
		{
		    textcolor((کلمه_2) arg1);
		}
		وإلا إذا (argc == 1 && *p == 'H')
		{
		    gotoxy(1, arg1);
		}
		وإلا إذا (argc == 1 && *p == 'L')
		{
		    درج_سطرها(arg1);
		}
		وإلا إذا (argc == 1 && *p == 'M')
		{
		    delete_سطرها(arg1);
		}

		len -= (صحيح)(p - s);
		s = p + 1;
		اكسر;

	    حالة 'A':
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.بالاترین_2, g_coord.Y - 1) + 1);
		اذهب got3;

	    حالة 'B':
		visual_bell();
		اذهب got3;

	    حالة 'C':
		gotoxy(min(g_srScrollRegion.راست_4, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		اذهب got3;

	    حالة 'E':
		termcap_نما_انتها();
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
		clear_مقصد_انتها_از_display();
		اذهب got3;

	    حالة 'J':
		clear_screen();
		اذهب got3;

	    حالة 'K':
		clear_مقصد_انتها_از_سطر();
		اذهب got3;

	    حالة 'L':
		درج_سطرها(1);
		اذهب got3;

	    حالة 'M':
		delete_سطرها(1);
		اذهب got3;

	    حالة 'S':
		termcap_نما_شروع();
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
	    طالما (s + l < انتها && s[l] >= 0x20 && s[l] <= 0x3F)
		l++;
	    // skip the final byte (0x40-0x7E)
	    إذا (s + l < انتها && s[l] >= 0x40 && s[l] <= 0x7E)
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

	    درون_dcs = true;
	    dcs_esc = false;
	    l = (صحيح)(skip_dcs(s + 2, انتها) - s);
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
    char_u  *s استفادهنشده,
    صحيح	    len استفادهنشده)
{
    // never used
}

#پایان_اگر


/*
 * Delay for "msec" milliseconds.
 */
    فراغ
mch_delay(
    دراز    msec,
    صحيح	    flags استفادهنشده)
{
#إذا defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    Sleep((صحيح)msec);	    // never wait for input
#وإلا // Console
# ifdef VIMDLL
    إذا (gui.درون_use)
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
		mzvim_check_نخها();
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
#پایان_اگر
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

    win32_setattrs(name, FILE_ATTRIBUTE_نرمال);

    wn = enc_مقصد_utf16(name, عدم);
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
mch_breakcheck(صحيح force استفادهنشده)
{
#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    إذا (!gui.درون_use)
# endif
	إذا (g_fCtrlCPressed || g_fCBrkPressed)
	{
	    ctrl_break_was_pressed = g_fCBrkPressed;
	    g_fCtrlCPressed = g_fCBrkPressed = FALSE;
	    got_int = TRUE;
	}
#پایان_اگر
}

// physical RAM to leave for the OS
#تعریف_کن WINNT_RESERVE_بایتها     (256*1024*1024)

/*
 * How much main memory in KiB that can be used by VIM.
 */
    long_u
mch_مجموع_mem(صحيح special استفادهنشده)
{
    MEMORYSTATUSEX  ms;

    // Need to use GlobalMemoryStatusEx() when there is more memory than
    // what fits in 32 bits.
    ms.dw_درازی = حجم(MEMORYSTATUSEX);
    Global_حافظه_وضعیت_Ex(&ms);
    إذا (ms.ullAvailVirtual < ms.ull_مجموع_Phys)
    {
	// Process address space fits in physical RAM, use all of it.
	أرجع (long_u)(ms.ullAvailVirtual / 1024);
    }
    إذا (ms.ull_مجموع_Phys <= WINNT_RESERVE_بایتها)
    {
	// Catch old NT box or perverse hardware setup.
	أرجع (long_u)((ms.ull_مجموع_Phys / 2) / 1024);
    }
    // Use physical RAM less reserve for OS + data.
    أرجع (long_u)((ms.ull_مجموع_Phys - WINNT_RESERVE_بایتها) / 1024);
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
    char_u	*مسیر,
    صحيح		flags)		// EW_* flags
{
    أرجع dos_expandpath(gap, مسیر, 0, flags, FALSE);
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
    WCHAR	sz_موقت_File[MAX_مسیر_2 + 1];
    WCHAR	sz_New_مسیر[MAX_مسیر_2 + 1];
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
    إذا (Get_پر_مسیر_Name_W(wnew, MAX_مسیر_2, sz_New_مسیر, &p) == 0 || p == عدم)
	أرجع -1;
    *p = NUL;

    // Get (and create) a unique temporary file name in directory of new file
    إذا (Get_موقت_File_Name_W(sz_New_مسیر, L"VIM", 0, sz_موقت_File) == 0)
	أرجع -2;

    // blow the temp file away
    إذا (!DeleteFileW(sz_موقت_File))
	أرجع -3;

    // rename old file to the temp file
    إذا (!MoveFileW(wold, sz_موقت_File))
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
		    FILE_ATTRIBUTE_نرمال, عدم)) == INVALID_HANDLE_مقدار)
	أرجع -5;
    إذا (!CloseHandle(hf))
	أرجع -6;

    // rename the temp file to the new file
    إذا (!MoveFileW(sz_موقت_File, wnew))
    {
	// Renaming failed.  Rename the file back to its old name, so that it
	// looks like nothing happened.
	(فراغ)MoveFileW(sz_موقت_File, wold);
	أرجع -7;
    }

    // Seems to be left around on Novell filesystems
    DeleteFileW(sz_موقت_File);

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

    wold = enc_مقصد_utf16((char_u *)pszOldFile, عدم);
    wnew = enc_مقصد_utf16((char_u *)pszNewFile, عدم);
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

    wn = enc_مقصد_utf16((char_u *)n, عدم);
    إذا (wn == عدم)
	أرجع -1;

    إذا (mch_isdir((char_u *)n))
    {
	WCHAR موقت_Name_W[MAX_مسیر_2 + 16] = L"";

	إذا (p & R_OK)
	{
	    // Read check is performed by seeing if we can do a find file on
	    // the directory for any file.
	    صحيح			i;
	    WIN32_پیدا_کن_DATAW    d;

	    لكل (i = 0; i < MAX_مسیر_2 && wn[i] != 0; ++i)
		موقت_Name_W[i] = wn[i];
	    إذا (موقت_Name_W[i - 1] != '\\' && موقت_Name_W[i - 1] != '/')
		موقت_Name_W[i++] = '\\';
	    موقت_Name_W[i++] = '*';
	    موقت_Name_W[i++] = 0;

	    hFile = پیدا_کن_اولی_File_W(موقت_Name_W, &d);
	    إذا (hFile == INVALID_HANDLE_مقدار)
		اذهب getout;
	    وإلا
		(فراغ)پیدا_کن_Close(hFile);
	}

	إذا (p & W_OK)
	{
	    // Trying to create a temporary file in the directory should catch
	    // directories on read-only network shares.  However, in
	    // directories whose ACL allows writes but denies deletes will end
	    // up keeping the temporary file :-(.
	    إذا (!Get_موقت_File_Name_W(wn, L"VIM", 0, موقت_Name_W))
		اذهب getout;
	    وإلا
		DeleteFileW(موقت_Name_W);
	}
    }
    وإلا
    {
	// Don't consider a file read-only if another process has opened it.
	DWORD share_نما = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// Trying to open the file for the required access does ACL, read-only
	// network share, and file attribute checks.
	DWORD access_نما = ((p & W_OK) ? GENERIC_WRITE : 0)
					     | ((p & R_OK) ? GENERIC_READ : 0);

	hFile = CreateFileW(wn, access_نما, share_نما,
					     عدم, OPEN_EXISTING, 0, عدم);
	إذا (hFile == INVALID_HANDLE_مقدار)
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
mch_open(ثابت حرف *name, صحيح flags, صحيح نما)
{
    WCHAR	*wn;
    صحيح		f;

    wn = enc_مقصد_utf16((char_u *)name, عدم);
    إذا (wn == عدم)
	أرجع -1;

    f = _wopen(wn, flags, نما);
    vim_free(wn);
    أرجع f;
}

/*
 * Version of fopen() that uses UTF-16 file name.
 */
    FILE *
mch_fopen(ثابت حرف *name, ثابت حرف *نما)
{
    WCHAR	*wn, *wm;
    FILE	*f = عدم;

#إذا defined(DEBUG) && _MSC_VER >= 1400
    // Work around an annoying assertion in the Microsoft debug CRT
    // when mode's text/binary setting doesn't match _get_fmode().
    حرف new_نما = نما[طول_رشته(نما) - 1];
    صحيح old_نما_2 = 0;

    _get_fmode(&old_نما_2);
    إذا (new_نما == 't')
	مجموعه_fmode(O_متن);
    وإلا إذا (new_نما == 'b')
	مجموعه_fmode(O_دودویی_2);
#پایان_اگر
    wn = enc_مقصد_utf16((char_u *)name, عدم);
    wm = enc_مقصد_utf16((char_u *)نما, عدم);
    إذا (wn != عدم && wm != عدم)
	f = _wfopen(wn, wm);
    vim_free(wn);
    vim_free(wm);

#إذا defined(DEBUG) && _MSC_VER >= 1400
    إذا (old_نما_2 != 0)
	مجموعه_fmode(old_نما_2);
#پایان_اگر
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
copy_substream(HANDLE sh, فراغ *context, WCHAR *مقصد, WCHAR *substream, دراز len)
{
    HANDLE  h_مقصد;
    WCHAR   *مقصد_name;

    مقصد_name = alloc((wcslen(مقصد) + wcslen(substream) + 1) * حجم(WCHAR));
    إذا (مقصد_name == عدم)
	أرجع;

    wcscpy(مقصد_name, مقصد);
    wcscat(مقصد_name, substream);

    h_مقصد = CreateFileW(مقصد_name, GENERIC_WRITE, 0, عدم, OPEN_ALWAYS,
						 FILE_ATTRIBUTE_نرمال, عدم);
    إذا (h_مقصد != INVALID_HANDLE_مقدار)
    {
	دراز	انجامشده;
	DWORD	todo;
	DWORD	readcnt, written;
	حرف	buf[4096];

	// Copy block of bytes at a time.  Abort when something goes wrong.
	لكل (انجامشده = 0; انجامشده < len; انجامشده += written)
	{
	    // (size_t) cast for Borland C 5.5
	    todo = (DWORD)((size_t)(len - انجامشده) > حجم(buf) ? حجم(buf)
						       : (size_t)(len - انجامشده));
	    إذا (!BackupRead(sh, (LPBYTE)buf, todo, &readcnt,
						       FALSE, FALSE, context)
		    || readcnt != todo
		    || !WriteFile(h_مقصد, buf, todo, &written, عدم)
		    || written != todo)
		اكسر;
	}
	CloseHandle(h_مقصد);
    }

    حرر(مقصد_name);
}

/*
 * Copy info streams from file "from" to file "to".
 */
    ساكن فراغ
copy_infostreams(char_u *from, char_u *مقصد)
{
    WCHAR		*fromw;
    WCHAR		*tow;
    HANDLE		sh;
    WIN32_جریان_ID	sid;
    صحيح			headersize;
    WCHAR		streamname[MAX_مسیر_2];
    DWORD		readcount;
    فراغ		*context = عدم;
    DWORD		lo, hi;
    صحيح			len;

    // Convert the file names to wide characters.
    fromw = enc_مقصد_utf16(from, عدم);
    tow = enc_مقصد_utf16(مقصد, عدم);
    إذا (fromw != عدم && tow != عدم)
    {
	// Open the file for reading.
	sh = CreateFileW(fromw, GENERIC_READ, FILE_SHARE_READ, عدم,
			     OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, عدم);
	إذا (sh != INVALID_HANDLE_مقدار)
	{
	    // Use BackupRead() to find the info streams.  Repeat until we
	    // have done them all.
	    لكل (;;)
	    {
		// Get the header to find the length of the stream name.  If
		// the "readcount" is zero we have done all info streams.
		Zero_حافظه(&sid, حجم(WIN32_جریان_ID));
		headersize = (صحيح)((حرف *)&sid.c_جریان_Name - (حرف *)&sid.dw_جریان_Id);
		إذا (!BackupRead(sh, (LPBYTE)&sid, headersize,
					   &readcount, FALSE, FALSE, &context)
			|| readcount == 0)
		    اكسر;

		// We only deal with streams that have a name.  The normal
		// file data appears to be without a name, even though docs
		// suggest it is called "::$DATA".
		إذا (sid.dw_جریان_Name_اندازه > 0)
		{
		    // Read the stream name.
		    إذا (!BackupRead(sh, (LPBYTE)streamname,
							 sid.dw_جریان_Name_اندازه,
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
						    (دراز)sid.اندازه_2.u.کم_Part);
		    }
		}

		// Advance to the next stream.  We might try seeking too far,
		// but BackupSeek() doesn't skip over stream borders, thus
		// that's OK.
		(فراغ)BackupSeek(sh, sid.اندازه_2.u.کم_Part, sid.اندازه_2.u.بالا_Part,
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
#تعریف_کن FileEaInformation   7
#اگر_تعریف_نشده وضعیت_SUCCESS
# define وضعیت_SUCCESS	    ((NTSTATUS) 0x00000000L)
#پایان_اگر

تعريفنوع بنية FILE_پر_EA_INFORMATION {
    ULONG  بعدی_مدخل_جابه_جایی;
    UCHAR  Flags;
    UCHAR  Ea_Name_درازی;
    USHORT Ea_مقدار_درازی;
    CHAR   EaName[1];
} FILE_پر_EA_INFORMATION_2, *PFILE_پر_EA_INFORMATION;

تعريفنوع بنية _FILE_EA_INFORMATION_ {
    ULONG Ea_اندازه;
} FILE_EA_INFORMATION_, *PFILE_EA_INFORMATION_;

#اگر_تعریف_نشده PROTO
تعريفنوع NTSTATUS (NTAPI *PfnNtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_ماسک DesiredAccess,
	POBJECT_صفات Object_صفات,
	PIO_وضعیت_بلوک Io_وضعیت_بلوک,
	ULONG ShareAccess,
	ULONG OpenOptions);
تعريفنوع NTSTATUS (NTAPI *PfnNtClose)(
	HANDLE Handle);
تعريفنوع NTSTATUS (NTAPI *Pfn_Nt_مجموعه_Ea_File)(
	HANDLE		    FileHandle,
	PIO_وضعیت_بلوک    Io_وضعیت_بلوک,
	PVOID		    میانگیر_2,
	ULONG		    درازی_2);
تعريفنوع NTSTATUS (NTAPI *PfnNtQueryEaFile)(
	HANDLE FileHandle,
	PIO_وضعیت_بلوک Io_وضعیت_بلوک,
	PVOID میانگیر_2,
	ULONG درازی_2,
	BOOLEAN Return_Single_مدخل,
	PVOID EaList,
	ULONG Ea_List_درازی,
	PULONG Ea_شاخص,
	BOOLEAN Restart_پیمایش);
تعريفنوع NTSTATUS (NTAPI *PfnNtQueryInformationFile)(
	HANDLE			FileHandle,
	PIO_وضعیت_بلوک	Io_وضعیت_بلوک,
	PVOID			FileInformation,
	ULONG			درازی_2,
	FILE_INFORMATION_CLASS FileInformationClass);
تعريفنوع VOID (NTAPI *PfnRtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR منبع_String);

PfnNtOpenFile pNtOpenFile = عدم;
PfnNtClose pNtClose = عدم;
Pfn_Nt_مجموعه_Ea_File p_Nt_مجموعه_Ea_File = عدم;
PfnNtQueryEaFile pNtQueryEaFile = عدم;
PfnNtQueryInformationFile pNtQueryInformationFile = عدم;
PfnRtlInitUnicodeString pRtlInitUnicodeString = عدم;
#پایان_اگر

/*
 * Load ntdll.dll functions.
 */
    ساكن BOOL
بارگیری_ntdll(فراغ)
{
    ساكن صحيح	loaded = -1;

    إذا (loaded != -1)
	أرجع (BOOL) loaded;

    HMODULE hNtdll = GetModuleHandle("ntdll.dll");
    إذا (hNtdll != عدم)
    {
	pNtOpenFile = (PfnNtOpenFile) Get_Proc_آدرس(hNtdll, "NtOpenFile");
	pNtClose = (PfnNtClose) Get_Proc_آدرس(hNtdll, "NtClose");
	p_Nt_مجموعه_Ea_File = (Pfn_Nt_مجموعه_Ea_File)
	    Get_Proc_آدرس(hNtdll, "NtSetEaFile");
	pNtQueryEaFile = (PfnNtQueryEaFile)
	    Get_Proc_آدرس(hNtdll, "NtQueryEaFile");
	pNtQueryInformationFile = (PfnNtQueryInformationFile)
	    Get_Proc_آدرس(hNtdll, "NtQueryInformationFile");
	pRtlInitUnicodeString = (PfnRtlInitUnicodeString)
	    Get_Proc_آدرس(hNtdll, "RtlInitUnicodeString");
    }
    إذا (pNtOpenFile == عدم
	    || pNtClose == عدم
	    || p_Nt_مجموعه_Ea_File == عدم
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
copy_extattr(char_u *from, char_u *مقصد)
{
    char_u		    *fromf = عدم;
    char_u		    *tof = عدم;
    WCHAR		    *fromw = عدم;
    WCHAR		    *tow = عدم;
    UNICODE_STRING	    u;
    HANDLE		    h;
    OBJECT_صفات	    oa;
    IO_وضعیت_بلوک	    iosb;
    FILE_EA_INFORMATION_    eainfo = {0};
    فراغ		    *ea = عدم;

    إذا (!بارگیری_ntdll())
	أرجع;

    // Convert the file names to the fully qualified object names.
    fromf = alloc(STRLEN(from) + 5);
    tof = alloc(STRLEN(مقصد) + 5);
    إذا (fromf == عدم || tof == عدم)
	اذهب theend;
    STRCPY(fromf, "\\??\\");
    STRCPY(fromf + STRLEN_LITERAL("\\??\\"), from);
    STRCPY(tof, "\\??\\");
    STRCPY(tof + STRLEN_LITERAL("\\??\\"), مقصد);

    // Convert the names to wide characters.
    fromw = enc_مقصد_utf16(fromf, عدم);
    tow = enc_مقصد_utf16(tof, عدم);
    إذا (fromw == عدم || tow == عدم)
	اذهب theend;

    // Get the EA.
    pRtlInitUnicodeString(&u, fromw);
    Initialize_Object_صفات(&oa, &u, 0, عدم, عدم);
    إذا (pNtOpenFile(&h, FILE_READ_EA, &oa, &iosb, 0,
		FILE_NON_DIRECTORY_FILE) != وضعیت_SUCCESS)
	اذهب theend;
    pNtQueryInformationFile(h, &iosb, &eainfo, حجم(eainfo),
	    FileEaInformation);
    إذا (eainfo.Ea_اندازه != 0)
    {
	ea = alloc(eainfo.Ea_اندازه);
	إذا (ea != عدم)
	{
	    إذا (pNtQueryEaFile(h, &iosb, ea, eainfo.Ea_اندازه, FALSE,
			عدم, 0, عدم, TRUE) != وضعیت_SUCCESS)
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
	Initialize_Object_صفات(&oa, &u, 0, عدم, عدم);
	إذا (pNtOpenFile(&h, FILE_WRITE_EA, &oa, &iosb, 0,
		    FILE_NON_DIRECTORY_FILE) != وضعیت_SUCCESS)
	    اذهب theend;

	p_Nt_مجموعه_Ea_File(h, &iosb, ea, eainfo.Ea_اندازه);
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
mch_copy_file_attribute(char_u *from, char_u *مقصد)
{
    // File streams only work on Windows NT and later.
    copy_infostreams(from, مقصد);
    copy_extattr(from, مقصد);
    أرجع 0;
}


/*
 * The command line arguments in UTF-16
 */
ساكن صحيح	n_آرگومانها_W = 0;
ساكن LPWSTR	*ArglistW = عدم;
ساكن صحيح	global_argc = 0;
ساكن حرف	**global_argv;

ساكن صحيح	استفاده_شده_file_argc = 0;	// last argument in global_argv[] used
					// for the argument list.
ساكن صحيح	*استفاده_شده_file_indexes = عدم; // indexes in global_argv[] for
					   // command line arguments added to
					   // the argument list
ساكن صحيح	استفاده_شده_file_شمار = 0;	// nr of entries in used_file_indexes
ساكن صحيح	استفاده_شده_file_literal = FALSE;  // take file names literally
ساكن صحيح	استفاده_شده_file_پر_مسیر = FALSE;  // file name was full path
#اگر_تعریف_شده FEAT_DIFF
ساكن صحيح	استفاده_شده_file_diff_نما = FALSE;  // file name was with diff mode
#پایان_اگر
ساكن صحيح	استفاده_شده_alist_شمار = 0;


/*
 * Get the command line arguments.  Unicode version.
 * Returns argc.  Zero when something fails.
 */
    صحيح
get_cmd_آرگومانها_W(حرف ***argvp)
{
    حرف	**argv = عدم;
    صحيح		argc = 0;
    صحيح		i;

    free_cmd_آرگومانها_W();
    ArglistW = Command_سطر_مقصد_Argv_W(Get_Command_سطر_W(), &n_آرگومانها_W);
    إذا (ArglistW != عدم)
    {
	argv = alloc((n_آرگومانها_W + 1) * حجم(حرف *));
	إذا (argv != عدم)
	{
	    argc = n_آرگومانها_W;
	    argv[argc] = عدم;
	    لكل (i = 0; i < argc; ++i)
	    {
		صحيح	len;

		// Convert each Unicode argument to UTF-8.
		Wide_Char_مقصد_Multi_Byte_alloc(CP_UTF8, 0,
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
	إذا (استفاده_شده_file_indexes != عدم)
	    حرر(استفاده_شده_file_indexes);
	استفاده_شده_file_indexes = alloc(argc * حجم(صحيح));
    }

    إذا (argvp != عدم)
	*argvp = argv;
    أرجع argc;
}

    فراغ
free_cmd_آرگومانها_W(فراغ)
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
استفاده_شده_file_arg(حرف *name, صحيح literal, صحيح پر_مسیر, صحيح diff_نما)
{
    صحيح		i;

    إذا (استفاده_شده_file_indexes == عدم)
	أرجع;
    لكل (i = استفاده_شده_file_argc + 1; i < global_argc; ++i)
	إذا (STRCMP(global_argv[i], name) == 0)
	{
	    استفاده_شده_file_argc = i;
	    استفاده_شده_file_indexes[استفاده_شده_file_شمار++] = i;
	    اكسر;
	}
    استفاده_شده_file_literal = literal;
    استفاده_شده_file_پر_مسیر = پر_مسیر;
#اگر_تعریف_شده FEAT_DIFF
    استفاده_شده_file_diff_نما = diff_نما;
#پایان_اگر
}

/*
 * Remember the length of the argument list as it was.  If it changes then we
 * leave it alone when 'encoding' is set.
 */
    فراغ
مجموعه_alist_شمار(فراغ)
{
    استفاده_شده_alist_شمار = GARGCOUNT;
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
    إذا (global_argc != n_آرگومانها_W
	    || ArglistW == عدم
	    || استفاده_شده_file_indexes == عدم
	    || استفاده_شده_file_شمار == 0
	    || استفاده_شده_alist_شمار != GARGCOUNT)
	أرجع;

    // Remember the buffer numbers for the arguments.
    fnum_list = ALLOC_MULT(صحيح, GARGCOUNT);
    إذا (fnum_list == عدم)
	أرجع;		// out of memory
    لكل (i = 0; i < GARGCOUNT; ++i)
	fnum_list[i] = GARGLIST[i].ae_fnum;

    // Clear the argument list.  Make room for the new arguments.
    alist_clear(&global_alist);
    إذا (ga_grow(&global_alist.al_ga, استفاده_شده_file_شمار) == FAIL)
	أرجع;		// out of memory

    لكل (i = 0; i < استفاده_شده_file_شمار; ++i)
    {
	idx = استفاده_شده_file_indexes[i];
	str = utf16_مقصد_enc(ArglistW[idx], عدم);
	إذا (str != عدم)
	{
	    صحيح literal = استفاده_شده_file_literal;

#اگر_تعریف_شده FEAT_DIFF
	    // When using diff mode may need to concatenate file name to
	    // directory name.  Just like it's done in main().
	    إذا (استفاده_شده_file_diff_نما && mch_isdir(str) && GARGCOUNT > 0
				      && !mch_isdir(alist_name(&GARGLIST[0])))
	    {
		char_u	    *دم;
		string_T    ret;

		دم = gettail(alist_name(&GARGLIST[0]));
		concat_fnames(str, STRLEN(str), دم, STRLEN(دم), TRUE, &ret);
		إذا (ret.string != عدم)
		{
		    vim_free(str);
		    str = ret.string;
		}
	    }
#پایان_اگر
	    // Re-use the old buffer by renaming it.  When not using literal
	    // names it's done by alist_expand() below.
	    إذا (استفاده_شده_file_literal)
		buf_مجموعه_name(fnum_list[i], str);

	    // Check backtick literal. backtick literal is already expanded in
	    // main.c, so this part add str as literal.
	    إذا (literal == FALSE)
	    {
		size_t len = STRLEN(str);

		إذا (len > 2 && *str == '`' && *(str + len - 1) == '`')
		    literal = TRUE;
	    }
	    alist_جمع_کن(&global_alist, str, literal ? 2 : 0);
	}
    }

    إذا (!استفاده_شده_file_literal)
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
	alist_expand(fnum_list, استفاده_شده_alist_شمار);
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
	إذا (GARGCOUNT == 1 && استفاده_شده_file_پر_مسیر
		&& vim_chdirfile(alist_name(&GARGLIST[0]), "drop") == OK)
	    آخرین_chdir_reason = "drop";
    }

    مجموعه_alist_شمار();
}

    صحيح
mch_setenv(حرف *var, حرف *مقدار, صحيح x استفادهنشده)
{
    char_u	*envbuf;
    WCHAR	*p;

    envbuf = alloc(STRLEN(var) + STRLEN(مقدار) + 2);
    إذا (envbuf == عدم)
	أرجع -1;

    sprintf((حرف *)envbuf, "%s=%s", var, مقدار);

    p = enc_مقصد_utf16(envbuf, عدم);

    vim_free(envbuf);
    إذا (p == عدم)
	أرجع -1;
    _wputenv(p);
#اگر_تعریف_شده libintl_wputenv
    libintl_wputenv(p);
#پایان_اگر
    // Unlike Un*x systems, we can free the string for _wputenv().
    vim_free(p);

    أرجع 0;
}

/*
 * Support for 256 colors and 24-bit colors was added in Windows 10
 * version 1703 (Creators update).
 */
#تعریف_کن VTP_اولی_SUPPORT_BUILD	    MAKE_VER(10, 0, 15063)

/*
 * Support for pseudo-console (ConPTY) was added in windows 10
 * version 1809 (October 2018 update).
 */
#تعریف_کن CONPTY_اولی_SUPPORT_BUILD  MAKE_VER(10, 0, 17763)

/*
 * ConPTY differences between versions, need different logic.
 * version 1903 (May 2019 update).
 */
#تعریف_کن CONPTY_1903_BUILD	    MAKE_VER(10, 0, 18362)

/*
 * version 1909 (November 2019 update).
 */
#تعریف_کن CONPTY_1909_BUILD	    MAKE_VER(10, 0, 18363)

/*
 * Stay ahead of the next update, and when it's done, fix this.
 * version ? (2020 update, temporarily use the build number of insider preview)
 */
#تعریف_کن CONPTY_بعدی_UPDATE_BUILD    MAKE_VER(10, 0, 19587)

/*
 * Confirm until this version.  Also the logic changes.
 * insider preview.
 */
#تعریف_کن CONPTY_INSIDER_BUILD	    MAKE_VER(10, 0, 18995)

/*
 * Make conpty default on Windows 11
 */
#تعریف_کن CONPTY_STABLE_BUILD	    MAKE_VER(10, 0, 22000)
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
    DWORD   نما;
    HANDLE  بیرون;

# ifdef VIMDLL
    إذا (!gui.درون_use)
# endif
    {
	بیرون = GetStdHandle(STD_OUTPUT_HANDLE);

	vtp_working = (win_نسخه >= VTP_اولی_SUPPORT_BUILD) ? 1 : 0;
	Get_Console_نما(بیرون, &نما);
	نما |= (ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	إذا (مجموعه_Console_نما(بیرون, نما) == 0)
	    vtp_working = 0;

	// VTP uses alternate screen buffer.
	// But, not if running in a nested terminal
	use_alternate_screen_میانگیر = win_نسخه >= MAKE_VER(10, 0, 19045)
	    && p_rs && vtp_working && !mch_getenv("VIM_TERMINAL");
    }
#پایان_اگر

    إذا (win_نسخه >= CONPTY_اولی_SUPPORT_BUILD)
	conpty_working = 1;
    إذا (win_نسخه >= CONPTY_STABLE_BUILD)
	conpty_stable = 1;

    إذا (win_نسخه <= CONPTY_INSIDER_BUILD)
	conpty_type = 3;
    إذا (win_نسخه <= CONPTY_1909_BUILD)
	conpty_type = 2;
    إذا (win_نسخه <= CONPTY_1903_BUILD)
	conpty_type = 2;
    إذا (win_نسخه < CONPTY_اولی_SUPPORT_BUILD)
	conpty_type = 1;

    إذا (win_نسخه >= CONPTY_بعدی_UPDATE_BUILD)
	conpty_fix_type = 1;
}

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)

    ساكن فراغ
vtp_init(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_میانگیر_INFOEX csbi;
    csbi.cb_اندازه = حجم(csbi);
    Get_Console_Screen_میانگیر_Info_Ex(g_h_Con_بیرون, &csbi);
    save_console_bg_rgb = (guicolor_T)csbi.Color_جدول[g_color_شاخص_bg];
    save_console_fg_rgb = (guicolor_T)csbi.Color_جدول[g_color_شاخص_fg];
    store_console_bg_rgb = save_console_bg_rgb;
    store_console_fg_rgb = save_console_fg_rgb;

    COLORREF bg;
    bg = (COLORREF)csbi.Color_جدول[g_color_شاخص_bg];
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);
    default_console_color_bg = bg;

    COLORREF fg;
    fg = (COLORREF)csbi.Color_جدول[g_color_شاخص_fg];
    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    default_console_color_fg = fg;
# endif
    مجموعه_console_color_rgb();
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
    DWORD   نتیجه;
    صحيح	    len;

    إذا (silent_نما)
	أرجع 0;

    va_start(list, format);
    len = vim_vsnprintf((حرف *)buf, 100, (حرف *)format, list);
    va_end(list);
    WriteConsoleA(g_h_Con_بیرون, buf, (DWORD)len, &نتیجه, عدم);
    أرجع (صحيح)نتیجه;
}

    ساكن فراغ
vtp_sgr_bulk(
    صحيح arg)
{
    صحيح آرگومانها[1];

    آرگومانها[0] = arg;
    vtp_sgr_bulks(1, آرگومانها);
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
    صحيح *آرگومانها)
{
# define MAXSGR 16
# define SGRBUFSIZE 2 + 4 * MAXSGR + 1 // '\033[' + SGR + 'm'
    char_u  buf[SGRBUFSIZE];
    char_u  *p;
    صحيح	    درون, بیرون;
    صحيح	    newargs[16];
    ساكن صحيح sgrfgr = -1, sgrfgg, sgrfgb;
    ساكن صحيح sgrbgr = -1, sgrbgg, sgrbgb;

    إذا (argc == 0)
    {
	sgrfgr = sgrbgr = -1;
	vtp_printf("\033[m");
	أرجع;
    }

    درون = بیرون = 0;
    طالما (درون < argc)
    {
	صحيح s = آرگومانها[درون];
	صحيح copylen = 1;

	إذا (s == 38)
	{
	    إذا (argc - درون >= 5 && آرگومانها[درون + 1] == 2)
	    {
		إذا (sgrfgr == آرگومانها[درون + 2] && sgrfgg == آرگومانها[درون + 3]
						     && sgrfgb == آرگومانها[درون + 4])
		{
		    درون += 5;
		    copylen = 0;
		}
		وإلا
		{
		    sgrfgr = آرگومانها[درون + 2];
		    sgrfgg = آرگومانها[درون + 3];
		    sgrfgb = آرگومانها[درون + 4];
		    copylen = 5;
		}
	    }
	    وإلا إذا (argc - درون >= 3 && آرگومانها[درون + 1] == 5)
	    {
		sgrfgr = -1;
		copylen = 3;
	    }
	}
	وإلا إذا (s == 48)
	{
	    إذا (argc - درون >= 5 && آرگومانها[درون + 1] == 2)
	    {
		إذا (sgrbgr == آرگومانها[درون + 2] && sgrbgg == آرگومانها[درون + 3]
						     && sgrbgb == آرگومانها[درون + 4])
		{
		    درون += 5;
		    copylen = 0;
		}
		وإلا
		{
		    sgrbgr = آرگومانها[درون + 2];
		    sgrbgg = آرگومانها[درون + 3];
		    sgrbgb = آرگومانها[درون + 4];
		    copylen = 5;
		}
	    }
	    وإلا إذا (argc - درون >= 3 && آرگومانها[درون + 1] == 5)
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
	    newargs[بیرون++] = آرگومانها[درون++];
    }

    p = &buf[حجم(buf) - 1];
    *p-- = 'm';

    بدل (بیرون)
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
	WriteConsoleA(g_h_Con_بیرون, p, (DWORD)(&buf[SGRBUFSIZE] - p), &r, عدم);
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
مجموعه_console_color_rgb(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    CONSOLE_SCREEN_میانگیر_INFOEX csbi;
    guicolor_T	fg, bg;
    صحيح		ctermfg, ctermbg;

    إذا (!vtp_working)
	أرجع;

    get_default_console_color(&ctermfg, &ctermbg, &fg, &bg);

    إذا (p_tgc || t_colors >= 256)
    {
	جمله_fg_rgb_color(fg);
	جمله_bg_rgb_color(bg);
	أرجع;
    }

    إذا (use_alternate_screen_میانگیر)
	أرجع;

    fg = (GetRValue(fg) << 16) | (GetGValue(fg) << 8) | GetBValue(fg);
    bg = (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);

    csbi.cb_اندازه = حجم(csbi);
    Get_Console_Screen_میانگیر_Info_Ex(g_h_Con_بیرون, &csbi);

    csbi.cb_اندازه = حجم(csbi);
    csbi.sr_پنجره.راست_4 += 1;
    csbi.sr_پنجره.Bottom += 1;
    store_console_bg_rgb = csbi.Color_جدول[g_color_شاخص_bg];
    store_console_fg_rgb = csbi.Color_جدول[g_color_شاخص_fg];
    csbi.Color_جدول[g_color_شاخص_bg] = (COLORREF)bg;
    csbi.Color_جدول[g_color_شاخص_fg] = (COLORREF)fg;
    مجموعه_Console_Screen_میانگیر_Info_Ex(g_h_Con_بیرون, &csbi);
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
	cterm_نرمال_fg_gui_color = guifg;
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
	cterm_نرمال_bg_gui_color = guibg;
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
    إذا (use_alternate_screen_میانگیر)
	أرجع;

    CONSOLE_SCREEN_میانگیر_INFOEX csbi;

    csbi.cb_اندازه = حجم(csbi);
    Get_Console_Screen_میانگیر_Info_Ex(g_h_Con_بیرون, &csbi);

    csbi.cb_اندازه = حجم(csbi);
    csbi.sr_پنجره.راست_4 += 1;
    csbi.sr_پنجره.Bottom += 1;
    csbi.Color_جدول[g_color_شاخص_bg] = (COLORREF)store_console_bg_rgb;
    csbi.Color_جدول[g_color_شاخص_fg] = (COLORREF)store_console_fg_rgb;
    مجموعه_Console_Screen_میانگیر_Info_Ex(g_h_Con_بیرون, &csbi);
# endif
}

/*
 * Set the console colors to the original colors.
 */
    ساكن فراغ
restore_console_color_rgb(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    إذا (use_alternate_screen_میانگیر)
	أرجع;

    CONSOLE_SCREEN_میانگیر_INFOEX csbi;

    csbi.cb_اندازه = حجم(csbi);
    Get_Console_Screen_میانگیر_Info_Ex(g_h_Con_بیرون, &csbi);

    csbi.cb_اندازه = حجم(csbi);
    csbi.sr_پنجره.راست_4 += 1;
    csbi.sr_پنجره.Bottom += 1;
    csbi.Color_جدول[g_color_شاخص_bg] = (COLORREF)save_console_bg_rgb;
    csbi.Color_جدول[g_color_شاخص_fg] = (COLORREF)save_console_fg_rgb;
    مجموعه_Console_Screen_میانگیر_Info_Ex(g_h_Con_بیرون, &csbi);
# endif
}

    فراغ
control_console_color_rgb(فراغ)
{
    إذا (vtp_working)
	مجموعه_console_color_rgb();
    وإلا
	reset_console_color_rgb();
}

    صحيح
use_vtp(فراغ)
{
    أرجع USE_VTP;
}

    صحيح
is_جمله_win32(فراغ)
{
    أرجع T_NAME != عدم && STRCMP(T_NAME, "win32") == 0;
}

    صحيح
has_vtp_working(فراغ)
{
    أرجع vtp_working;
}

#پایان_اگر

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
    إذا (use_alternate_screen_میانگیر)
	أرجع;

    CONSOLE_SCREEN_میانگیر_INFO csbi;
    COORD coord;
    SMALL_RECT newsize;

    إذا (!Get_Console_Screen_میانگیر_Info(g_h_Con_بیرون, &csbi))
	أرجع;

    coord.X = SRWIDTH(csbi.sr_پنجره);
    coord.Y = SRHEIGHT(csbi.sr_پنجره);
    مجموعه_Console_Screen_میانگیر_اندازه(g_h_Con_بیرون, coord);

    newsize.چپ_4 = 0;
    newsize.بالاترین_2 = 0;
    newsize.راست_4 = coord.X - 1;
    newsize.Bottom = coord.Y - 1;
    مجموعه_Console_پنجره_Info(g_h_Con_بیرون, TRUE, &newsize);

    مجموعه_Console_Screen_میانگیر_اندازه(g_h_Con_بیرون, coord);
}
#پایان_اگر

    حرف *
GetWin32Error(فراغ)
{
    ساكن حرف	*oldmsg = عدم;
    حرف	*acp_پیام = عدم;
    DWORD	acp_len;
    حرف	*enc_پیام = عدم;
    صحيح		enc_len = 0;

    // get formatted message from OS
    acp_len = Format_پیام(
	    FORMAT_پیام_ALLOCATE_میانگیر|FORMAT_پیام_FROM_SYSTEM,
	    عدم, Get_آخرین_Error(), 0, (LPSTR)&acp_پیام, 0, عدم);
    إذا (acp_len == 0 || acp_پیام == عدم)
	أرجع عدم;

    // clean oldmsg if remained.
    إذا (oldmsg != عدم)
    {
	vim_free(oldmsg);
	oldmsg = عدم;
    }

    acp_مقصد_enc((char_u *)acp_پیام, (صحيح)acp_len, (char_u **)&enc_پیام, &enc_len);
    LocalFree(acp_پیام);
    إذا (enc_پیام == عدم)
	أرجع عدم;

    // remove trailing \r\n
    حرف *pcrlf = جستجوی_رشته(enc_پیام, "\r\n");
    إذا (pcrlf != عدم)
	*pcrlf = NUL;

    oldmsg = enc_پیام;
    أرجع enc_پیام;
}

#إذا defined(FEAT_RELTIME)
ساكن HANDLE   timer_handle;
ساكن صحيح      timer_فعال = FALSE;

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
مجموعه_flag(فراغ *param, BOOLEAN unused2 استفادهنشده)
{
    *(sig_atomic_t *)param = TRUE;
}

/*
 * Stop any active timeout.
 */
    فراغ
stop_timeout(فراغ)
{
    إذا (timer_فعال)
    {
	BOOL ret = Delete_Timer_صف_Timer(عدم, timer_handle, عدم);
	timer_فعال = FALSE;
	إذا (!ret && Get_آخرین_Error() != ERROR_IO_PENDING)
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
شروع_timeout(دراز msec)
{
    BOOL ret;

    timeout_flag = &timeout_flags[timeout_flag_idx];

    stop_timeout();
    ret = Create_Timer_صف_Timer(
	    &timer_handle, عدم, مجموعه_flag, timeout_flag,
	    (DWORD)msec, 0, WT_EXECUTEDEFAULT);
    إذا (!ret)
    {
	semsg(_(e_could_not_مجموعه_timeout_str), GetWin32Error());
    }
    وإلا
    {
	timeout_flag_idx = (timeout_flag_idx + 1) % 2;
	timer_فعال = TRUE;
	*timeout_flag = FALSE;
    }
    أرجع timeout_flag;
}
#پایان_اگر
